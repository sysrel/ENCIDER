//===-- SpecialFunctionHandler.cpp ----------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Memory.h"
#include "SpecialFunctionHandler.h"
#include "TimingSolver.h"
#include "klee/MergeHandler.h"

#include "klee/ExecutionState.h"

#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/KModule.h"
#include "klee/Internal/Support/Debug.h"
#include "klee/Internal/Support/ErrorHandling.h"

#include "Executor.h"
#include "MemoryManager.h"
/* SYSREL extension */
#include "StatsTracker.h"
/* SYSREL extension */
#include "klee/CommandLine.h"

#include "llvm/IR/Module.h"
#include "llvm/ADT/Twine.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CallSite.h"


#include <errno.h>
#include <sstream>
#include <fstream>
using namespace llvm;
using namespace klee;

namespace {
  cl::opt<bool>
  ReadablePosix("readable-posix-inputs",
            cl::init(false),
            cl::desc("Prefer creation of POSIX inputs (command-line arguments, files, etc.) with human readable bytes. "
                     "Note: option is expensive when creating lots of tests (default=false)"));

  cl::opt<bool>
  SilentKleeAssume("silent-klee-assume",
                   cl::init(false),
                   cl::desc("Silently terminate paths with an infeasible "
                            "condition given to klee_assume() rather than "
                            "emitting an error (default=false)"));
}


/* SYSREL EXTENSION */
 typedef std::pair<ExecutionState*,ExecutionState*> StatePair;
std::set<std::string> assemblyFunctions;
 typedef std::pair<ExecutionState*,ExecutionState*> StatePair;
extern Interpreter *theInterpreter;
extern const Module * moduleHandle;
extern KModule *kmoduleExt;
extern std::string getTypeName(llvm::Type*);
extern bool isLazyInit(Type *t, bool &single, int &count);

inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from right
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

AllocAPIHandler *APIHandler::allocAPIHandler = NULL;
FreeAPIHandler *APIHandler::freeAPIHandler = NULL;
IgnoreAPIHandler *APIHandler::ignoreAPIHandler = NULL;
SideEffectAPIHandler *APIHandler::sideEffectAPIHandler = NULL;
RefCountAPIHandler *APIHandler::refcountAPIHandler = NULL;
CallbackAPIHandler *APIHandler::callbackAPIHandler = NULL;
TerminateAPIHandler *APIHandler::terminateAPIHandler = NULL;
std::map<std::string, std::set<APIAction*>> APIHandler::apiModel;
std::map<std::string, APIAction*> APIHandler::symbolTable;
/* SYSREL EXTENSION */

/// \todo Almost all of the demands in this file should be replaced
/// with terminateState calls.

///



// FIXME: We are more or less committed to requiring an intrinsic
// library these days. We can move some of this stuff there,
// especially things like realloc which have complicated semantics
// w.r.t. forking. Among other things this makes delayed query
// dispatch easier to implement.
static SpecialFunctionHandler::HandlerInfo handlerInfo[] = {
#define add(name, handler, ret) { name, \
                                  &SpecialFunctionHandler::handler, \
                                  false, ret, false }
#define addDNR(name, handler) { name, \
                                &SpecialFunctionHandler::handler, \
                                true, false, false }
  addDNR("__assert_rtn", handleAssertFail),
  addDNR("__assert_fail", handleAssertFail),
  addDNR("_assert", handleAssert),
  addDNR("abort", handleAbort),
  addDNR("_exit", handleExit),
  { "exit", &SpecialFunctionHandler::handleExit, true, false, true },
  addDNR("klee_abort", handleAbort),
  addDNR("klee_silent_exit", handleSilentExit),
  addDNR("klee_report_error", handleReportError),
  add("calloc", handleCalloc, true),
  add("free", handleFree, false),
  add("klee_assume", handleAssume, false),
  add("klee_check_memory_access", handleCheckMemoryAccess, false),
  add("klee_get_valuef", handleGetValue, true),
  add("klee_get_valued", handleGetValue, true),
  add("klee_get_valuel", handleGetValue, true),
  add("klee_get_valuell", handleGetValue, true),
  add("klee_get_value_i32", handleGetValue, true),
  add("klee_get_value_i64", handleGetValue, true),
  add("klee_define_fixed_object", handleDefineFixedObject, false),
  add("klee_get_obj_size", handleGetObjSize, true),
  add("klee_get_errno", handleGetErrno, true),
#ifndef __APPLE__
  add("__errno_location", handleErrnoLocation, true),
#else
  add("__error", handleErrnoLocation, true),
#endif
  add("klee_is_symbolic", handleIsSymbolic, true),
  add("klee_make_symbolic", handleMakeSymbolic, false),
  add("klee_mark_global", handleMarkGlobal, false),
  add("klee_open_merge", handleOpenMerge, false),
  add("klee_close_merge", handleCloseMerge, false),
  add("klee_prefer_cex", handlePreferCex, false),
  add("klee_posix_prefer_cex", handlePosixPreferCex, false),
  add("klee_print_expr", handlePrintExpr, false),
  add("klee_print_range", handlePrintRange, false),
  add("klee_set_forking", handleSetForking, false),
  add("klee_stack_trace", handleStackTrace, false),
  add("klee_warning", handleWarning, false),
  add("klee_warning_once", handleWarningOnce, false),
  add("klee_alias_function", handleAliasFunction, false),
  add("malloc", handleMalloc, true),
  add("realloc", handleRealloc, true),

  /* SYSREL EXTENSION */
  //add("__kmalloc", handleMalloc, true),
  add("kfree", handleFree, false),
  /* SYSREL EXTENSION */

  // operator delete[](void*)
  add("_ZdaPv", handleDeleteArray, false),
  // operator delete(void*)
  add("_ZdlPv", handleDelete, false),

  // operator new[](unsigned int)
  add("_Znaj", handleNewArray, true),
  // operator new(unsigned int)
  add("_Znwj", handleNew, true),

  // FIXME-64: This is wrong for 64-bit long...

  // operator new[](unsigned long)
  add("_Znam", handleNewArray, true),
  // operator new(unsigned long)
  add("_Znwm", handleNew, true),

  // Run clang with -fsanitize=signed-integer-overflow and/or
  // -fsanitize=unsigned-integer-overflow
  add("__ubsan_handle_add_overflow", handleAddOverflow, false),
  add("__ubsan_handle_sub_overflow", handleSubOverflow, false),
  add("__ubsan_handle_mul_overflow", handleMulOverflow, false),
  add("__ubsan_handle_divrem_overflow", handleDivRemOverflow, false),

#undef addDNR
#undef add
};

SpecialFunctionHandler::const_iterator SpecialFunctionHandler::begin() {
  return SpecialFunctionHandler::const_iterator(handlerInfo);
}

SpecialFunctionHandler::const_iterator SpecialFunctionHandler::end() {
  // NULL pointer is sentinel
  return SpecialFunctionHandler::const_iterator(0);
}

SpecialFunctionHandler::const_iterator& SpecialFunctionHandler::const_iterator::operator++() {
  ++index;
  if ( index >= SpecialFunctionHandler::size())
  {
    // Out of range, return .end()
    base=0; // Sentinel
    index=0;
  }

  return *this;
}

int SpecialFunctionHandler::size() {
	return sizeof(handlerInfo)/sizeof(handlerInfo[0]);
}

SpecialFunctionHandler::SpecialFunctionHandler(Executor &_executor) 
  : executor(_executor) {}


void SpecialFunctionHandler::prepare() {
  unsigned N = size();

  for (unsigned i=0; i<N; ++i) {
    HandlerInfo &hi = handlerInfo[i];
    Function *f = executor.kmodule->module->getFunction(hi.name);
    
    // No need to create if the function doesn't exist, since it cannot
    // be called in that case.
  
    if (f && (!hi.doNotOverride || f->isDeclaration())) {
      // Make sure NoReturn attribute is set, for optimization and
      // coverage counting.
      if (hi.doesNotReturn)
        f->addFnAttr(Attribute::NoReturn);

      // Change to a declaration since we handle internally (simplifies
      // module and allows deleting dead code).
      if (!f->isDeclaration())
        f->deleteBody();
    }
  }
}

void SpecialFunctionHandler::bind() {
  unsigned N = sizeof(handlerInfo)/sizeof(handlerInfo[0]);

  for (unsigned i=0; i<N; ++i) {
    HandlerInfo &hi = handlerInfo[i];
    Function *f = executor.kmodule->module->getFunction(hi.name);
    
    if (f && (!hi.doNotOverride || f->isDeclaration()))
      handlers[f] = std::make_pair(hi.handler, hi.hasReturnValue);
  }
}


bool SpecialFunctionHandler::handle(ExecutionState &state, 
                                    Function *f,
                                    KInstruction *target,
                                    std::vector< ref<Expr> > &arguments) {
  handlers_ty::iterator it = handlers.find(f);
  if (it != handlers.end()) {    
    Handler h = it->second.first;
    bool hasReturnValue = it->second.second;
     // FIXME: Check this... add test?
    if (!hasReturnValue && !target->inst->use_empty()) {
      executor.terminateStateOnExecError(state, 
                                         "expected return value from void special function");
    } else {
      (this->*h)(state, target, arguments);
    }
    return true;
  } else {
    return false;
  }
}

/****/

// reads a concrete string from memory
std::string 
SpecialFunctionHandler::readStringAtAddress(ExecutionState &state, 
                                            ref<Expr> addressExpr) {
  ObjectPair op;
  addressExpr = executor.toUnique(state, addressExpr);
  if (!isa<ConstantExpr>(addressExpr)) {
    executor.terminateStateOnError(
        state, "Symbolic string pointer passed to one of the klee_ functions",
        Executor::TerminateReason::User);
    return "";
  }
  ref<ConstantExpr> address = cast<ConstantExpr>(addressExpr);
  if (!state.addressSpace.resolveOne(address, op)) {
    executor.terminateStateOnError(
        state, "Invalid string pointer passed to one of the klee_ functions",
        Executor::TerminateReason::User);
    return "";
  }
  bool res __attribute__ ((unused));
  assert(executor.solver->mustBeTrue(state, 
                                     EqExpr::create(address, 
                                                    op.first->getBaseExpr()),
                                     res) &&
         res &&
         "XXX interior pointer unhandled");
  const MemoryObject *mo = op.first;
  const ObjectState *os = op.second;

  char *buf = new char[mo->size];

  unsigned i;
  for (i = 0; i < mo->size - 1; i++) {
    ref<Expr> cur = os->read8(i);
    cur = executor.toUnique(state, cur);
    assert(isa<ConstantExpr>(cur) && 
           "hit symbolic char while reading concrete string");
    buf[i] = cast<ConstantExpr>(cur)->getZExtValue(8);
  }
  buf[i] = 0;
  
  std::string result(buf);
  delete[] buf;
  return result;
}

/****/

void SpecialFunctionHandler::handleAbort(ExecutionState &state,
                           KInstruction *target,
                           std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==0 && "invalid number of arguments to abort");
  executor.terminateStateOnError(state, "abort failure", Executor::Abort);
}

void SpecialFunctionHandler::handleExit(ExecutionState &state,
                           KInstruction *target,
                           std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==1 && "invalid number of arguments to exit");
  executor.terminateStateOnExit(state);
}

void SpecialFunctionHandler::handleSilentExit(ExecutionState &state,
                                              KInstruction *target,
                                              std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==1 && "invalid number of arguments to exit");
  executor.terminateState(state);
}

void SpecialFunctionHandler::handleAliasFunction(ExecutionState &state,
						 KInstruction *target,
						 std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==2 && 
         "invalid number of arguments to klee_alias_function");
  std::string old_fn = readStringAtAddress(state, arguments[0]);
  std::string new_fn = readStringAtAddress(state, arguments[1]);
  KLEE_DEBUG_WITH_TYPE("alias_handling", llvm::errs() << "Replacing " << old_fn
                                           << "() with " << new_fn << "()\n");
  if (old_fn == new_fn)
    state.removeFnAlias(old_fn);
  else state.addFnAlias(old_fn, new_fn);
}

void SpecialFunctionHandler::handleAssert(ExecutionState &state,
                                          KInstruction *target,
                                          std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==3 && "invalid number of arguments to _assert");  
  executor.terminateStateOnError(state,
				 "ASSERTION FAIL: " + readStringAtAddress(state, arguments[0]),
				 Executor::Assert);
}

void SpecialFunctionHandler::handleAssertFail(ExecutionState &state,
                                              KInstruction *target,
                                              std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==4 && "invalid number of arguments to __assert_fail");
  executor.terminateStateOnError(state,
				 "ASSERTION FAIL: " + readStringAtAddress(state, arguments[0]),
				 Executor::Assert);
}

void SpecialFunctionHandler::handleReportError(ExecutionState &state,
                                               KInstruction *target,
                                               std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==4 && "invalid number of arguments to klee_report_error");
  
  // arguments[0], arguments[1] are file, line
  executor.terminateStateOnError(state,
				 readStringAtAddress(state, arguments[2]),
				 Executor::ReportError,
				 readStringAtAddress(state, arguments[3]).c_str());
}

void SpecialFunctionHandler::handleOpenMerge(ExecutionState &state,
    KInstruction *target,
    std::vector<ref<Expr> > &arguments) {
  if (!UseMerge) {
    klee_warning_once(0, "klee_open_merge ignored, use '-use-merge'");
    return;
  }

  state.openMergeStack.push_back(
      ref<MergeHandler>(new MergeHandler(&executor, &state)));

  if (DebugLogMerge)
    llvm::errs() << "open merge: " << &state << "\n";
}

void SpecialFunctionHandler::handleCloseMerge(ExecutionState &state,
    KInstruction *target,
    std::vector<ref<Expr> > &arguments) {
  if (!UseMerge) {
    klee_warning_once(0, "klee_close_merge ignored, use '-use-merge'");
    return;
  }
  Instruction *i = target->inst;

  if (DebugLogMerge)
    llvm::errs() << "close merge: " << &state << " at " << i << '\n';

  if (state.openMergeStack.empty()) {
    std::ostringstream warning;
    warning << &state << " ran into a close at " << i << " without a preceding open";
    klee_warning("%s", warning.str().c_str());
  } else {
    executor.inCloseMerge.insert(&state);
    state.openMergeStack.back()->addClosedState(&state, i);
    state.openMergeStack.pop_back();
  }
}

void SpecialFunctionHandler::handleNew(ExecutionState &state,
                         KInstruction *target,
                         std::vector<ref<Expr> > &arguments) {
  // XXX should type check args
  assert(arguments.size()==1 && "invalid number of arguments to new");

  executor.executeAlloc(state, arguments[0], false, target);
}

void SpecialFunctionHandler::handleDelete(ExecutionState &state,
                            KInstruction *target,
                            std::vector<ref<Expr> > &arguments) {
  // FIXME: Should check proper pairing with allocation type (malloc/free,
  // new/delete, new[]/delete[]).

  // XXX should type check args
  assert(arguments.size()==1 && "invalid number of arguments to delete");
  executor.executeFree(state, arguments[0]);
}

void SpecialFunctionHandler::handleNewArray(ExecutionState &state,
                              KInstruction *target,
                              std::vector<ref<Expr> > &arguments) {
  // XXX should type check args
  assert(arguments.size()==1 && "invalid number of arguments to new[]");
  executor.executeAlloc(state, arguments[0], false, target);
}

void SpecialFunctionHandler::handleDeleteArray(ExecutionState &state,
                                 KInstruction *target,
                                 std::vector<ref<Expr> > &arguments) {
  // XXX should type check args
  assert(arguments.size()==1 && "invalid number of arguments to delete[]");
  executor.executeFree(state, arguments[0]);
}

void SpecialFunctionHandler::handleMalloc(ExecutionState &state,
                                  KInstruction *target,
                                  std::vector<ref<Expr> > &arguments) {
  // XXX should type check args
  /* SYSREL EXTENSION */
  // we need this to handle kmalloc and kzalloc via malloc
  assert(arguments.size()>=1 && "invalid number of arguments to malloc");
  //assert(arguments.size()==1 && "invalid number of arguments to malloc");
  llvm::outs() << "argument to malloc: " << arguments[0] << "\n";
  /* SYSREL EXTENSION */
  executor.executeAlloc(state, arguments[0], false, target);
}

void SpecialFunctionHandler::handleAssume(ExecutionState &state,
                            KInstruction *target,
                            std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==1 && "invalid number of arguments to klee_assume");
  
  ref<Expr> e = arguments[0];
  
  if (e->getWidth() != Expr::Bool)
    e = NeExpr::create(e, ConstantExpr::create(0, e->getWidth()));
  
  bool res;
  bool success __attribute__ ((unused)) = executor.solver->mustBeFalse(state, e, res);
  assert(success && "FIXME: Unhandled solver failure");
  if (res) {
    if (SilentKleeAssume) {
      executor.terminateState(state);
    } else {
      executor.terminateStateOnError(state,
                                     "invalid klee_assume call (provably false)",
                                     Executor::User);
    }
  } else {
    executor.addConstraint(state, e);
  }
}

void SpecialFunctionHandler::handleIsSymbolic(ExecutionState &state,
                                KInstruction *target,
                                std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==1 && "invalid number of arguments to klee_is_symbolic");

  executor.bindLocal(target, state, 
                     ConstantExpr::create(!isa<ConstantExpr>(arguments[0]),
                                          Expr::Int32));
}

void SpecialFunctionHandler::handlePreferCex(ExecutionState &state,
                                             KInstruction *target,
                                             std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==2 &&
         "invalid number of arguments to klee_prefex_cex");

  ref<Expr> cond = arguments[1];
  if (cond->getWidth() != Expr::Bool)
    cond = NeExpr::create(cond, ConstantExpr::alloc(0, cond->getWidth()));

  Executor::ExactResolutionList rl;
  executor.resolveExact(state, arguments[0], rl, "prefex_cex");
  
  assert(rl.size() == 1 &&
         "prefer_cex target must resolve to precisely one object");

  rl[0].first.first->cexPreferences.push_back(cond);
}

void SpecialFunctionHandler::handlePosixPreferCex(ExecutionState &state,
                                             KInstruction *target,
                                             std::vector<ref<Expr> > &arguments) {
  if (ReadablePosix)
    return handlePreferCex(state, target, arguments);
}

void SpecialFunctionHandler::handlePrintExpr(ExecutionState &state,
                                  KInstruction *target,
                                  std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==2 &&
         "invalid number of arguments to klee_print_expr");

  std::string msg_str = readStringAtAddress(state, arguments[0]);
  llvm::errs() << msg_str << ":" << arguments[1] << "\n";
}

void SpecialFunctionHandler::handleSetForking(ExecutionState &state,
                                              KInstruction *target,
                                              std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==1 &&
         "invalid number of arguments to klee_set_forking");
  ref<Expr> value = executor.toUnique(state, arguments[0]);
  
  if (ConstantExpr *CE = dyn_cast<ConstantExpr>(value)) {
    state.forkDisabled = CE->isZero();
  } else {
    executor.terminateStateOnError(state, 
                                   "klee_set_forking requires a constant arg",
                                   Executor::User);
  }
}

void SpecialFunctionHandler::handleStackTrace(ExecutionState &state,
                                              KInstruction *target,
                                              std::vector<ref<Expr> > &arguments) {
  state.dumpStack(outs());
}

void SpecialFunctionHandler::handleWarning(ExecutionState &state,
                                           KInstruction *target,
                                           std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==1 && "invalid number of arguments to klee_warning");

  std::string msg_str = readStringAtAddress(state, arguments[0]);
  klee_warning("%s: %s", state.stack.back().kf->function->getName().data(), 
               msg_str.c_str());
}

void SpecialFunctionHandler::handleWarningOnce(ExecutionState &state,
                                               KInstruction *target,
                                               std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==1 &&
         "invalid number of arguments to klee_warning_once");

  std::string msg_str = readStringAtAddress(state, arguments[0]);
  klee_warning_once(0, "%s: %s", state.stack.back().kf->function->getName().data(),
                    msg_str.c_str());
}

void SpecialFunctionHandler::handlePrintRange(ExecutionState &state,
                                  KInstruction *target,
                                  std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==2 &&
         "invalid number of arguments to klee_print_range");

  std::string msg_str = readStringAtAddress(state, arguments[0]);
  llvm::errs() << msg_str << ":" << arguments[1];
  if (!isa<ConstantExpr>(arguments[1])) {
    // FIXME: Pull into a unique value method?
    ref<ConstantExpr> value;
    bool success __attribute__ ((unused)) = executor.solver->getValue(state, arguments[1], value);
    assert(success && "FIXME: Unhandled solver failure");
    bool res;
    success = executor.solver->mustBeTrue(state, 
                                          EqExpr::create(arguments[1], value), 
                                          res);
    assert(success && "FIXME: Unhandled solver failure");
    if (res) {
      llvm::errs() << " == " << value;
    } else { 
      llvm::errs() << " ~= " << value;
      std::pair< ref<Expr>, ref<Expr> > res =
        executor.solver->getRange(state, arguments[1]);
      llvm::errs() << " (in [" << res.first << ", " << res.second <<"])";
    }
  }
  llvm::errs() << "\n";
}

void SpecialFunctionHandler::handleGetObjSize(ExecutionState &state,
                                  KInstruction *target,
                                  std::vector<ref<Expr> > &arguments) {
  // XXX should type check args
  assert(arguments.size()==1 &&
         "invalid number of arguments to klee_get_obj_size");
  Executor::ExactResolutionList rl;
  executor.resolveExact(state, arguments[0], rl, "klee_get_obj_size");
  for (Executor::ExactResolutionList::iterator it = rl.begin(), 
         ie = rl.end(); it != ie; ++it) {
    executor.bindLocal(
        target, *it->second,
        ConstantExpr::create(it->first.first->size,
                             executor.kmodule->targetData->getTypeSizeInBits(
                                 target->inst->getType())));
  }
}

void SpecialFunctionHandler::handleGetErrno(ExecutionState &state,
                                            KInstruction *target,
                                            std::vector<ref<Expr> > &arguments) {
  // XXX should type check args
  assert(arguments.size()==0 &&
         "invalid number of arguments to klee_get_errno");
#ifndef WINDOWS
  int *errno_addr = executor.getErrnoLocation(state);
#else
  int *errno_addr = nullptr;
#endif

  // Retrieve the memory object of the errno variable
  ObjectPair result;
  bool resolved = state.addressSpace.resolveOne(
      ConstantExpr::create((uint64_t)errno_addr, Expr::Int64), result);
  if (!resolved)
    executor.terminateStateOnError(state, "Could not resolve address for errno",
                                   Executor::User);
  executor.bindLocal(target, state, result.second->read(0, Expr::Int32));
}

void SpecialFunctionHandler::handleErrnoLocation(
    ExecutionState &state, KInstruction *target,
    std::vector<ref<Expr> > &arguments) {
  // Returns the address of the errno variable
  assert(arguments.size() == 0 &&
         "invalid number of arguments to __errno_location/__error");

#ifndef WINDOWS
  int *errno_addr = executor.getErrnoLocation(state);
#else
  int *errno_addr = nullptr;
#endif

  executor.bindLocal(
      target, state,
      ConstantExpr::create((uint64_t)errno_addr,
                           executor.kmodule->targetData->getTypeSizeInBits(
                               target->inst->getType())));
}
void SpecialFunctionHandler::handleCalloc(ExecutionState &state,
                            KInstruction *target,
                            std::vector<ref<Expr> > &arguments) {
  // XXX should type check args
  assert(arguments.size()==2 &&
         "invalid number of arguments to calloc");

  ref<Expr> size = MulExpr::create(arguments[0],
                                   arguments[1]);
  executor.executeAlloc(state, size, false, target, true);
}

void SpecialFunctionHandler::handleRealloc(ExecutionState &state,
                            KInstruction *target,
                            std::vector<ref<Expr> > &arguments) {
  // XXX should type check args
  assert(arguments.size()==2 &&
         "invalid number of arguments to realloc");
  ref<Expr> address = arguments[0];
  ref<Expr> size = arguments[1];

  Executor::StatePair zeroSize = executor.fork(state, 
                                               Expr::createIsZero(size), 
                                               true);
  
  if (zeroSize.first) { // size == 0
    executor.executeFree(*zeroSize.first, address, target);   
  }
  if (zeroSize.second) { // size != 0
    Executor::StatePair zeroPointer = executor.fork(*zeroSize.second, 
                                                    Expr::createIsZero(address), 
                                                    true);
    
    if (zeroPointer.first) { // address == 0
      executor.executeAlloc(*zeroPointer.first, size, false, target);
    } 
    if (zeroPointer.second) { // address != 0
      Executor::ExactResolutionList rl;
      executor.resolveExact(*zeroPointer.second, address, rl, "realloc");
      
      for (Executor::ExactResolutionList::iterator it = rl.begin(), 
             ie = rl.end(); it != ie; ++it) {
        executor.executeAlloc(*it->second, size, false, target, false, 
                              it->first.second);
      }
    }
  }
}

void SpecialFunctionHandler::handleFree(ExecutionState &state,
                          KInstruction *target,
                          std::vector<ref<Expr> > &arguments) {
  // XXX should type check args
  assert(arguments.size()==1 &&
         "invalid number of arguments to free");
  executor.executeFree(state, arguments[0]);
}

void SpecialFunctionHandler::handleCheckMemoryAccess(ExecutionState &state,
                                                     KInstruction *target,
                                                     std::vector<ref<Expr> > 
                                                       &arguments) {
  assert(arguments.size()==2 &&
         "invalid number of arguments to klee_check_memory_access");

  ref<Expr> address = executor.toUnique(state, arguments[0]);
  ref<Expr> size = executor.toUnique(state, arguments[1]);
  if (!isa<ConstantExpr>(address) || !isa<ConstantExpr>(size)) {
    executor.terminateStateOnError(state, 
                                   "check_memory_access requires constant args",
				   Executor::User);
  } else {
    ObjectPair op;

    if (!state.addressSpace.resolveOne(cast<ConstantExpr>(address), op)) {
      executor.terminateStateOnError(state,
                                     "check_memory_access: memory error",
				     Executor::Ptr, NULL,
                                     executor.getAddressInfo(state, address));
    } else {
      ref<Expr> chk = 
        op.first->getBoundsCheckPointer(address, 
                                        cast<ConstantExpr>(size)->getZExtValue());
      if (!chk->isTrue()) {
        executor.terminateStateOnError(state,
                                       "check_memory_access: memory error",
				       Executor::Ptr, NULL,
                                       executor.getAddressInfo(state, address));
      }
    }
  }
}

void SpecialFunctionHandler::handleGetValue(ExecutionState &state,
                                            KInstruction *target,
                                            std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==1 &&
         "invalid number of arguments to klee_get_value");

  executor.executeGetValue(state, arguments[0], target);
}

void SpecialFunctionHandler::handleDefineFixedObject(ExecutionState &state,
                                                     KInstruction *target,
                                                     std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==2 &&
         "invalid number of arguments to klee_define_fixed_object");
  assert(isa<ConstantExpr>(arguments[0]) &&
         "expect constant address argument to klee_define_fixed_object");
  assert(isa<ConstantExpr>(arguments[1]) &&
         "expect constant size argument to klee_define_fixed_object");
  
  uint64_t address = cast<ConstantExpr>(arguments[0])->getZExtValue();
  uint64_t size = cast<ConstantExpr>(arguments[1])->getZExtValue();
  MemoryObject *mo = executor.memory->allocateFixed(address, size, state.prevPC->inst);
  executor.bindObjectInState(state, mo, false);
  mo->isUserSpecified = true; // XXX hack;
}

void SpecialFunctionHandler::handleMakeSymbolic(ExecutionState &state,
                                                KInstruction *target,
                                                std::vector<ref<Expr> > &arguments) {
  std::string name;

  // FIXME: For backwards compatibility. We should eventually enforce the
  // correct arguments and types.
  switch (arguments.size()) {
    case 2:
      klee_warning("klee_make_symbolic: deprecated number of arguments (2 instead of 3)");
      break;
    case 3:
      name = readStringAtAddress(state, arguments[2]);
      break;
    default:
      executor.terminateStateOnError(state, "illegal number of arguments to klee_make_symbolic(void*, size_t, char*)", Executor::User);
      return;
  }
  if (name.length() == 0) {
    name = "unnamed";
    klee_warning("klee_make_symbolic: renamed empty name to \"unnamed\"");
  }

  Executor::ExactResolutionList rl;
  executor.resolveExact(state, arguments[0], rl, "make_symbolic");
  
  for (Executor::ExactResolutionList::iterator it = rl.begin(), 
         ie = rl.end(); it != ie; ++it) {
    const MemoryObject *mo = it->first.first;
    mo->setName(name);
    
    const ObjectState *old = it->first.second;
    ExecutionState *s = it->second;
    
    if (old->readOnly) {
      executor.terminateStateOnError(*s, "cannot make readonly object symbolic",
                                     Executor::User);
      return;
    } 

    // FIXME: Type coercion should be done consistently somewhere.
    bool res;
    bool success __attribute__ ((unused)) =
      executor.solver->mustBeTrue(*s, 
                                  EqExpr::create(ZExtExpr::create(arguments[1],
                                                                  Context::get().getPointerWidth()),
                                                 mo->getSizeExpr()),
                                  res);
    assert(success && "FIXME: Unhandled solver failure");
    
    if (res) {
      executor.executeMakeSymbolic(*s, mo, name);
    } else {      
      executor.terminateStateOnError(*s, 
                                     "wrong size given to klee_make_symbolic[_name]", 
                                     Executor::User);
    }
  }
}

void SpecialFunctionHandler::handleMarkGlobal(ExecutionState &state,
                                              KInstruction *target,
                                              std::vector<ref<Expr> > &arguments) {
  assert(arguments.size()==1 &&
         "invalid number of arguments to klee_mark_global");  

  Executor::ExactResolutionList rl;
  executor.resolveExact(state, arguments[0], rl, "mark_global");
  
  for (Executor::ExactResolutionList::iterator it = rl.begin(), 
         ie = rl.end(); it != ie; ++it) {
    const MemoryObject *mo = it->first.first;
    assert(!mo->isLocal);
    mo->isGlobal = true;
  }
}

void SpecialFunctionHandler::handleAddOverflow(ExecutionState &state,
                                               KInstruction *target,
                                               std::vector<ref<Expr> > &arguments) {
  executor.terminateStateOnError(state, "overflow on addition",
                                 Executor::Overflow);
}

void SpecialFunctionHandler::handleSubOverflow(ExecutionState &state,
                                               KInstruction *target,
                                               std::vector<ref<Expr> > &arguments) {
  executor.terminateStateOnError(state, "overflow on subtraction",
                                 Executor::Overflow);
}

void SpecialFunctionHandler::handleMulOverflow(ExecutionState &state,
                                               KInstruction *target,
                                               std::vector<ref<Expr> > &arguments) {
  executor.terminateStateOnError(state, "overflow on multiplication",
                                 Executor::Overflow);
}

void SpecialFunctionHandler::handleDivRemOverflow(ExecutionState &state,
                                               KInstruction *target,
                                               std::vector<ref<Expr> > &arguments) {
  executor.terminateStateOnError(state, "overflow on division or remainder",
                                 Executor::Overflow);
}


/* SYSREL EXTENSION */

/*
MutexAPIHandler::MutexAPIHandler() {}

void MutexAPIHandler::addInitAcqRel(std::string acqf, std::string relf, std::string initf) {
   if (initf != "")
      initSet.insert(initf);
   initMap[acqf] = initf;
   acqRelMap[acqf] = relf;
   releaseSet.insert(relf);
}
     

bool MutexAPIHandler::handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, int tid) {
   return handle(state.threads[tid].stateModel, fname, arguments);
}

bool MutexAPIHandler::handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments) {
   return handle(state.stateModel, fname, arguments);
}

bool MutexAPIHandler::handle(std::map<ref<Expr>, int> &stateModel, std::string fname, std::vector< ref<Expr> > &arguments) {

  if (initSet.find(fname) != initSet.end()) {
     if (stateModel.find(arguments[0]) == stateModel.end())
        stateModel[arguments[0]] = 1;
     else assert(false && "mutex init called more than once \n");
    return true;
  }
  else if (acqRelMap.find(fname) != acqRelMap.end()) {
     if (initMap.find(fname) != initMap.end()) {
        if (stateModel.find(arguments[0]) == stateModel.end())
           assert(false && "mutex acquired without initializing it first\n");
        else if (stateModel[arguments[0]] == 0) 
           assert(false && "Double acquire!\n");
        // to do implement blocking semantics for multithreaded execution cases
     }
     if (stateModel.find(arguments[0]) != stateModel.end()) {
        if (stateModel[arguments[0]] == 0) 
           assert(false && "Double acquire!\n");
     }
     stateModel[arguments[0]] = 0; 
     return true; 
  }  
  else if (releaseSet.find(fname) != releaseSet.end()) {
     if (stateModel.find(arguments[0]) == stateModel.end())
        assert(false && "mutex released without initializing it first\n"); 
     else  if (stateModel[arguments[0]] == 1)
        assert(false && "Release without acquire\n");
     // to do implement signaling semantics for multithreaded execution cases
    stateModel[arguments[0]] = 1;
    return true;
  }
  return false;
}

*/

RefCountAPIHandler::RefCountAPIHandler() : APIHandler() {}

bool RefCountAPIHandler::assignsRetValue() {
  return false;   
}

bool RefCountAPIHandler::interpret(APIAction &action, ExecutionState &state, 
    std::vector< ref<Expr> > &arguments, KInstruction *target, bool &temp, int tid){   

  std::vector<std::string> par = action.getParams();
  std::string api = par[0];
  std::string op = par[1];
  const DataLayout &dl = target->inst->getParent()->getParent()->getParent()->getDataLayout();
  llvm::CallSite cs(target->inst);
  Value *fp = cs.getCalledValue();
  Function *caller = ((Executor*)theInterpreter)->getTargetFunction(fp, state);
  ref<Expr> refobjaddr = eval(state,  dl, caller, arguments, par[2], target, false);
  llvm::errs() << "memory object at base addr " << refobjaddr  << "\n";
  int value = std::stoi(par[3]);
  std::string onzerocb = par[4]; 
 
  if (op == "set") {
     state.setRefCount(refobjaddr, value);
     llvm::errs() << "refcount for " << refobjaddr  << " set to " << value << "\n";
     assert(value >= 0 && "Negative refcount value set!");
  }
  else if (op == "inc") {
     int cv = state.getRefCount(refobjaddr);
     llvm::errs() << "refcount for " << refobjaddr  << " set to " << cv + value << "\n";
     state.setRefCount(refobjaddr, cv + value);
  }
  else if (op == "dec") {
     int cv = state.getRefCount(refobjaddr);
     state.setRefCount(refobjaddr, cv - value);
     llvm::errs() << "refcount for " << refobjaddr  << " set to " << cv - value << "\n";
     assert((cv - value) >= 0 && "Negative refcount encountered in decrement!");
     if ((cv - value) == 0) {
        if (onzerocb != "none") {
           if (onzerocb.find("arg") != std::string::npos) { 
              llvm::outs() << "executing callback " << onzerocb << " on zero refcount\n";
              executeCallback(state, onzerocb, arguments, target, tid); 
           }
           else {
             llvm::outs() << "executing subblock " << onzerocb << " on zero refcount\n";
             APIAction *action = APIHandler::lookup(onzerocb);
             llvm::errs() << "onzerocb=" << onzerocb << "\n";
             assert(action);
             bool term2 = false;
             if (action) {
                action->execute(state, arguments, target, term2, tid);
             }
           }
        }
     }
  }
};

/*
     
RegistrationAPIHandler::RegistrationAPIHandler() : APIHandler() {}

void RegistrationAPIHandler::addRegUnreg(std::string regf, std::string unregf, std::set<std::string> enabledFuncs) {
  regAPIMap[regf] = unregf;  
  unregAPIMap[unregf] = regf;
  this->enabledFuncs[regf] = enabledFuncs;
  
}

bool RegistrationAPIHandler::handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, int tid) {
  return handle(state.threads[tid].stateModel, fname, arguments);
}

bool RegistrationAPIHandler::handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments) {
  return handle(state.stateModel, fname, arguments);
}

bool RegistrationAPIHandler::handle(std::map<ref<Expr>, int> &stateModel, std::string fname, std::vector< ref<Expr> > &arguments) {

  if (regAPIMap.find(fname) != regAPIMap.end()) {
     if (stateModel.find(arguments[0]) != stateModel.end())
        assert(false && "Double registration with \n");
     stateModel[arguments[0]] = 1; 
     return true;
  }
  if (unregAPIMap.find(fname) != unregAPIMap.end()) {
     if (stateModel.find(arguments[0]) == stateModel.end())
        assert(false && "Unregister with without registering it first \n");
     if (stateModel[arguments[0]] == 0)
        assert(false && "Double unregistration \n");
     stateModel[arguments[0]] = 0;
     return true;
  }   
  return false;
};

bool RegistrationAPIHandler::getEnabled(std::string reg, std::set<std::string> &q) {
   if (enabledFuncs.find(reg) != enabledFuncs.end()) {
      q = enabledFuncs[reg];
      return true;
   }
   return false; 
}

bool RegistrationAPIHandler::getDisabled(std::string unreg, std::set<std::string> &q) {
  if (unregAPIMap.find(unreg) != unregAPIMap.end()) {
     q = enabledFuncs[unregAPIMap[unreg]]; 
     return true;
  }
  return false;
}



ResourceAllocReleaseAPIHandler::ResourceAllocReleaseAPIHandler() : APIHandler() {}


bool ResourceAllocReleaseAPIHandler::handle(std::map<ref<Expr>, int> &stateModel, std::string fname, std::vector< ref<Expr> > &arguments) {

   if (resAPIMap.find(fname) != resAPIMap.end()) {
      if (stateModel.find(arguments[0]) != stateModel.end())
         assert(false && "Memory leak due to consecutive resource allocation!\n");
      else stateModel[arguments[0]] = 1;
      return true;
   }
   if (revresAPIMap.find(fname) != revresAPIMap.end()) {
      if (stateModel.find(arguments[0]) == stateModel.end())
         assert(false && "Resource deallocation without an allocation!\n");
      else if (stateModel[arguments[0]] == 0) 
         assert(false && "Double resource deallocation\n");
      else stateModel[arguments[0]] = 0; 
      return true;
   }
   return false;
}

SetGetAPIHandler::SetGetAPIHandler() : APIHandler() {
}

void SetGetAPIHandler::addSetGet(std::string set, std::string get) {
   setgetAPIMap[set] = get;
   getsetAPIMap[get] = set;
}

bool SetGetAPIHandler::handle(ExecutionState &state, int tid, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target) {
   std::map<ref<Expr>, ref<Expr> > *assocModel = NULL;
   llvm::outs() << "checking get/set handler for " << fname << "\n";
   if (tid == -1)
      assocModel = &state.assocModel;
   else if (tid >= 0)
      assocModel = &state.threads[tid].assocModel;
   else assert(false && "Illegal thread id!\n");
   if (setgetAPIMap.find(fname) != setgetAPIMap.end())  {
      llvm::outs() << "setgetAPIHandler (set)" << fname << " tid " << tid << "\n";
      (*assocModel)[arguments[0]] = arguments[1];
      llvm::outs() << "setting " << arguments[0] << " to " << arguments[1] << " with " << fname << "\n";  
      return true;
   }
   if (getsetAPIMap.find(fname) != getsetAPIMap.end()) {
      if (assocModel->find(arguments[0]) != assocModel->end()) {
         llvm::outs() << "setgetAPIHandler (get)" << fname << " tid " << tid << "\n";
         ref<Expr> result = (*assocModel)[arguments[0]];
         llvm::outs() << " assigning " << result << " to " << target->inst << "\n";
         if (tid == -1)
            ((Executor*)(theInterpreter))->bindLocal(target, state, result);
         else 
            ((Executor*)(theInterpreter))->bindLocalThread(target, state, result, tid);
  
      }
      else assert(false && "Get without a previous set API call!");
      return true;
   }
   return false;
}

bool SetGetAPIHandler::handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target) {
   return handle(state, -1, fname, arguments, target);
}

bool SetGetAPIHandler::handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target, int tid) {
   return handle(state, tid, fname, arguments, target);
}
*/

AllocAPIHandler::AllocAPIHandler() : APIHandler() {
}

bool AllocAPIHandler::assignsRetValue(APIAction &action) {
     std::vector<std::string> pars = action.getParams();
     llvm::outs() << "Alloc API assigns to return value? " << pars[4] << "entered\n";
     return (pars[4] == "true");   
}

bool AllocAPIHandler::interpret(APIAction &action, ExecutionState &state,
         std::vector< ref<Expr> > &arguments, KInstruction *target, bool &term, int tid) {
    std::vector<std::string> par = action.getParams();
    std::string fname = par[0];
    llvm::outs() << "Handling alloc api " << fname << " in thread " << tid << "\n";
    llvm::outs() << "Target instruction " << (*target->inst) << "\n";
    bool zeroMemory = (par[2] == "true");
    llvm::CallInst *ci = dyn_cast<CallInst>(target->inst);
    if (!ci)
       assert(false && "Expected a call instruction for allocation API!\n");
    int param = std::stoi(par[1]);
    bool sym = (par[3] == "true");
    if (!sym) {
       if (tid == -1)  
          ((Executor*)(theInterpreter))->executeAlloc(state, arguments[param], false, target, zeroMemory);
       else
          ((Executor*)(theInterpreter))->executeAllocThread(state, arguments[param], false, target, zeroMemory, 0, tid); 

      state.addSymbolDef(par[5], ((Executor*)(theInterpreter))->getDestCell(state, target).value);
      state.addSymbolType(par[5], target->inst->getType());
    }
    else {

    Type *t = ci->getArgOperand(param)->getType();
    if (t->isPointerTy()) {
       t = t->getPointerElementType();
       if (t->isPointerTy()) {
          t = t->getPointerElementType();
       }
       else assert(false && "Alloc API param type must be a double-pointer!\n");
    }
    else assert(false && "Alloc API param type must be a double-pointer!\n");
    const DataLayout &dl = target->inst->getParent()->getParent()->getParent()->getDataLayout();
    size_t allocationAlignment = dl.getPrefTypeAlignment(t);
    size_t allocationSize = dl.getTypeAllocSize(t);
    MemoryObject *mo = ((Executor*)(theInterpreter))->memory->allocate(allocationSize, false, true,
                         ci, allocationAlignment);
    if (!mo) {
       llvm::outs() << "Could not allocate memory object while handling alloc API!\n";
       return false;
    }
    llvm::outs() << "Allocated memory object at " << mo->getBaseExpr() << " to handle alloc API " << fname << "\n"; 

    state.addSymbolDef(par[5], mo->getBaseExpr());
    state.addSymbolType(par[5], t);
     
    mo->name = fname + "_" + std::to_string(param);
    if (tid == -1) {
       ((Executor*)(theInterpreter))->executeMakeSymbolic(state, mo, mo->name);
       ((Executor*)(theInterpreter))->executeMemoryOperation(state, true, arguments[param], mo->getBaseExpr(), 0);
    } 
    else {
       ((Executor*)(theInterpreter))->executeMakeSymbolicThread(state, mo, mo->name, tid); 
       ((Executor*)(theInterpreter))->executeMemoryOperationThread(state, true, arguments[param], mo->getBaseExpr(), 0, tid); 
    }
     }
    return true;
}


FreeAPIHandler::FreeAPIHandler() : APIHandler() {}

bool FreeAPIHandler::assignsRetValue(APIAction &action) {
  return false;
}

       
bool FreeAPIHandler::interpret(APIAction &action, ExecutionState &state, 
          std::vector< ref<Expr> > &arguments, KInstruction *target, bool &term, int tid) {
     std::vector<std::string> pars = action.getParams();
     llvm::outs() << "Handling free api " << pars[0] << "\n";
     int arg =  std::stoi(pars[1]);
     if (tid == -1)  
         ((Executor*)(theInterpreter))->executeFree(state, arguments[arg], target);
     else
         ((Executor*)(theInterpreter))->executeFreeThread(state, arguments[arg], target, tid);     
     return true;
}

/*

ReadWriteAPIHandler::ReadWriteAPIHandler() : APIHandler() {
}

void ReadWriteAPIHandler::addRead(std::string read) {
  readAPI.insert(read);
}
 
// to simulate writes from a source address to a destination address
void ReadWriteAPIHandler::addWrite(std::string write, int source, int dest) {
  llvm::outs() << "recording writeAPI " << write << "," << source << "," << dest << "\n";
  writeAPIMap[write] = std::make_pair(source, dest); 
  writeAPI.insert(write);
} 

// to simulate writes that write a value to a destination
void ReadWriteAPIHandler::addWrite(std::string write) {
  writeAPI.insert(write);
} 

bool ReadWriteAPIHandler::handle(ExecutionState &state, int tid, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target) {
  if (readAPI.find(fname) != readAPI.end()) {
     if (tid == -1)
        ((Executor*)(theInterpreter))->executeMemoryOperation(state, false, arguments[0], 0, target);
     else 
        ((Executor*)(theInterpreter))->executeMemoryOperationThread(state, false, arguments[0], 0, target, tid);
     llvm::outs() << "readAPI " << fname << " reading from " << arguments[0] << "\n"; 
     return true;
  }
  else if (writeAPI.find(fname) != writeAPI.end()) {
     if (writeAPIMap.find(fname) == writeAPIMap.end()) {
        if (tid == -1)
           ((Executor*)(theInterpreter))->executeMemoryOperation(state, true, arguments[0], arguments[1], 0);
        else 
           ((Executor*)(theInterpreter))->executeMemoryOperationThread(state, true, arguments[0], arguments[1], 0, tid);
        llvm::outs() << "writeAPI " << fname << " writing " << arguments[1] << " to " << arguments[0] << "\n"; 
     }
     else  {
        int source = writeAPIMap[fname].first;
        int dest = writeAPIMap[fname].second;
        llvm::outs() << "writeAPI (source,dest) " << fname << " writing " << arguments[source] << " to " << arguments[dest] << "\n"; 
        ObjectPair op;
        bool asuccess;
        ref<Expr> sourceAddr = arguments[source];
        ref<Expr> destAddr = arguments[dest];
        ((Executor*)(theInterpreter))->solver->setTimeout(((Executor*)(theInterpreter))->coreSolverTimeout);
        if (!state.addressSpace.resolveOne(state, ((Executor*)(theInterpreter))->solver, sourceAddr, op, asuccess)) {
           sourceAddr = ((Executor*)(theInterpreter))->toConstant(state, sourceAddr, "resolveOne failure");
           asuccess = state.addressSpace.resolveOne(cast<ConstantExpr>(sourceAddr), op);
        }
        ((Executor*)(theInterpreter))->solver->setTimeout(0);             
        if (asuccess) {
           // find the type and then the width
           llvm::Function *function = moduleHandle->getFunction(fname);
           int ai=0;
           Type *t = NULL;
           for(llvm::Function::arg_iterator agi = function->arg_begin(); agi != function->arg_end(); agi++, ai++) {
              if (ai == source) {
                 t = agi->getType();
                 break;
              }
           }
           assert(t->isPointerTy());
           t = t->getPointerElementType();
           Expr::Width type = ((Executor*)(theInterpreter))->getWidthForLLVMType(t);
           ref<Expr> value = op.second->read(ConstantExpr::alloc(0, Expr::Int64), type);       
           if (tid == -1)
              ((Executor*)(theInterpreter))->executeMemoryOperation(state, true, arguments[dest], value , 0);
           else 
             ((Executor*)(theInterpreter))->executeMemoryOperationThread(state, true, arguments[dest], value, 0, tid);           
        }
     }
     return true;
  }
  return false;
}

bool ReadWriteAPIHandler::handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target) {
   return handle(state, -1, fname, arguments, target);
}

bool ReadWriteAPIHandler::handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target, int tid) {
   return handle(state, tid, fname, arguments, target); 
}
*/

IgnoreAPIHandler::IgnoreAPIHandler() : APIHandler() {
}


bool IgnoreAPIHandler::interpret(APIAction &action, ExecutionState &state, 
           std::vector< ref<Expr> > &arguments, KInstruction *target, bool &term, int tid) {
    std::vector<std::string> pars = action.getParams();
    llvm::outs() << "Handling ignore api " << pars[0] << "\n";
    return true;
}

bool IgnoreAPIHandler::assignsRetValue(APIAction &action) {
  return false;
}

void APIHandler::executeCallback(ExecutionState &state, std::string cbexpr, std::vector< ref<Expr> > &arguments, 
         KInstruction *target, int tid) {
   const Function *f = NULL;
   if (cbexpr.find("arg") == 0) {
      llvm::CallSite cs(target->inst);
      Value *fp = cs.getCalledValue();
      Function *caller = ((Executor*)theInterpreter)->getTargetFunction(fp, state);
      const DataLayout &dl = target->inst->getParent()->getParent()->getParent()->getDataLayout();
      ref<Expr> cbaddr = eval(state,  dl, caller, arguments, cbexpr, target, false) ;
      llvm::errs() << "cbaddr=" << cbaddr << "\n";
      f = ((Executor*)theInterpreter)->getFunctionFromAddress(cbaddr);
   }
   else 
      f = kmoduleExt->module->getFunction(cbexpr);
   llvm::errs() << "callback expr: " << cbexpr << "\n";
   assert(f && "Callback function not defined!\n");
   llvm::outs() << "calling callback " << f->getName() << "\n";
   Function *ncf = target->inst->getParent()->getParent()->getParent()->getFunction(f->getName());
   KFunction *kf = kmoduleExt->functionMap[ncf];      
   state.pushFrame(state.prevPC, kf);
   state.pc = kf->instructions;

   StatsTracker * statsTracker = ((Executor*)theInterpreter)->getStatsTracker();
   if (statsTracker) {
      if (tid == -1)
         statsTracker->framePushed(state, &state.stack[state.stack.size()-2]);
      else
         statsTracker->framePushedThread(state, &state.stack[state.stack.size()-2], tid);
   }
   // allocate symbolic arguments
   ((Executor*)theInterpreter)->initArgsAsSymbolic(state, kf->function);

}  

CallbackAPIHandler::CallbackAPIHandler() : APIHandler() {}

bool CallbackAPIHandler::assignsRetValue(APIAction &action) {
  return false;
}


bool CallbackAPIHandler::interpret(APIAction &action, ExecutionState &state, 
      std::vector< ref<Expr> > &arguments, KInstruction *target, bool &term, int tid) {
   std::vector<std::string> par = action.getParams();
   executeCallback(state, par[1], arguments, target, tid); 
   return true; 
}
 

TerminateAPIHandler::TerminateAPIHandler() : APIHandler() {}

bool TerminateAPIHandler::interpret(APIAction &action, ExecutionState &state, 
                                std::vector< ref<Expr> > &arguments, 
                                KInstruction *target, bool &term, int tid) {
  llvm::outs() << "Handling terminate api\n " ; action.print();
  std::vector<std::string>  par = action.getParams();
  std::string texpr = par[1];
  Function *f = kmoduleExt->module->getFunction(par[0]);
  const DataLayout &dl = target->inst->getParent()->getParent()->getParent()->getDataLayout();
  ref<Expr> res = eval(state, dl, f, arguments, texpr, target, false);
  llvm::errs() << "Time to terminate? " << res << " vs " << Expr::createPointer(0) << "\n";
  term = (res == Expr::createPointer(0));
  return true;
}       

bool TerminateAPIHandler::assignsRetValue(APIAction &action) {
}


SideEffectAPIHandler::SideEffectAPIHandler() : APIHandler() {}

// syntax argi([-> | .]fieldindex)+
// we assume that argi holds base address of the memory object pointed by that pointer type argument
// to do: handle the * operator
ref<Expr> APIHandler::eval(ExecutionState &state,  const DataLayout &dl, Function *f, 
     std::vector< ref<Expr> > &arguments, std::string expr, KInstruction *target, bool address) {

   if (expr == "NULL")
      return Expr::createPointer(0);

   ref<Expr> cur;
   llvm::Type *t = NULL;
   llvm::StructType *st = NULL;
   llvm::Type *ptrtype = NULL;
   unsigned i; 
   auto pos = expr.find("arg");
   if (pos != std::string::npos) {
    for(i = pos + 3; expr[i] != '-' && expr[i] != '.' && i < expr.length() ; i++) ;
    std::string index;
    int argindex; 
    if (i == expr.length()) 
       index = expr.substr(pos+3);
    else {
       index = expr.substr(pos+3, i);
    }     
    argindex = std::stoi(index); 
    // find the type of the argument
    int j = 0;
    for(llvm::Argument & arg : f->args()) {
       if (j == argindex) {
           t = arg.getType();
           break;
       }
       j++;
    }
    if (t->isPointerTy()) {
       ptrtype = t;
       t = t->getPointerElementType(); 
    } 
    st = dyn_cast<StructType>(t);
    cur = arguments[argindex];
    llvm::errs() << "In SE:  arguments[" << argindex << "] is " <<  cur << "\n";
 }
 else { 
       pos = expr.find("alloc");
       if (pos == std::string::npos) {
          llvm::outs() <<  expr << "\n";
          assert(0 && "expression does not refer to an argument or an alloc symbol");
       }
       else {
          for(i = pos + 5; expr[i] != '-' && expr[i] != '.' && i < expr.length() ; i++) ;
          std::string symb;
          if (i == expr.length()) 
             symb = expr.substr(pos);
          else 
             symb = expr.substr(pos, i);
          cur = state.getSymbolDef(symb);
          if (dyn_cast<ConstantExpr>(cur)) {
             const auto CE = dyn_cast<ConstantExpr>(cur);
             if (!CE->getZExtValue()) {
                llvm::errs() << "Symbol " << symb  << " not defined!";
                assert(0 && "Alloc symbol not defined!");
             }
          }
          t = state.getSymbolType(symb);
          if (t->isPointerTy()) {
             ptrtype = t;
             t = t->getPointerElementType(); 
          } 
          st = dyn_cast<StructType>(t);
          llvm::errs() << "In SE: " << symb << " is " << cur << "\n";
       }
  }


    int last = 0;
    std::string field;
    int fieldIndex; 
    ref<Expr> addressRes = cur;
    while (i < expr.length()) {
        llvm::errs() <<"parsing loop \n";
        if ((expr[i] == '-' || expr[i] == '.')) {
           if (!st)
              assert(0 && "Dereferencing into a non-struct type pointer!\n");
           if (expr[i] == '-') {
              assert(expr[i+1] == '>' && "Unexpected dereferencing operator in the update expression!\n");
              i = i + 2;
           }
           else  i = i + 1;
           last = i;
           if (i == expr.length())
              assert(0 && "Incomplete update expression field name does not follow the dereferencing operator!\n");
           else {
              while (i < expr.length() && expr[i] != '-' && expr[i] != '.') {
                i++;
              }  
              field = expr.substr(last, i);
              fieldIndex = std::stoi(field); 
              uint64_t offset = dl.getStructLayout(st)->getElementOffset(fieldIndex);
              llvm::errs() << "using " << cur << "\n"; 
              cur = AddExpr::create(ConstantExpr::create(offset, Expr::Int64), cur);
              llvm::errs() << "computed addr " << cur << "\n";
              t = st->getElementType(fieldIndex);
              if (t->isPointerTy()) {
                 ptrtype = t;
                 t = t->getPointerElementType();
                 // load the pointer object and update cur with the base address
                 bool asuccess;
                 ObjectPair op;
                 ((Executor*)(theInterpreter))->solver->setTimeout(((Executor*)(theInterpreter))->coreSolverTimeout);
                 if (!state.addressSpace.resolveOne(state, ((Executor*)(theInterpreter))->solver, cur, op, asuccess)) {
                    cur = ((Executor*)(theInterpreter))->toConstant(state, cur, "resolveOne failure");
                    asuccess = state.addressSpace.resolveOne(cast<ConstantExpr>(cur), op);
                 }
                 ((Executor*)(theInterpreter))->solver->setTimeout(0);             
                 if (asuccess) {
                    addressRes = cur;
                    ref<Expr> offsetExpr = SubExpr::create(cur, op.first->getBaseExpr()); 
                    cur = op.second->read(offsetExpr,Context::get().getPointerWidth()/*dl.getPointerTypeSizeInBits(ptrtype)*/);
                    llvm::errs() << "after updating with the value, cur=" << cur << "\n";
                    // read the field string           
                 }
                 else {
                   llvm::Type *rallocType;
                   ref<Expr> resaddr; 
                   bool sym;
                   int count = 0;
                   bool singleInstance = false;
                   bool lazyInitTemp = isLazyInit(t, singleInstance, count);
                   const MemoryObject *mo = ((Executor*)(theInterpreter))->memory->allocateLazyForTypeOrEmbedding(state, target->inst, 
                              t, t, singleInstance, count, rallocType, resaddr, sym);
                   mo->name = "sym_" + getTypeName(t); 
                   if (sym) {
                      ((Executor*)(theInterpreter))->executeMakeSymbolic(state, mo, mo->name);  
                   }
                   ((Executor*)(theInterpreter))->executeMemoryOperation(state, true, cur, resaddr, 0);
                   addressRes = cur;
                   cur = resaddr;
                   llvm::outs() << "lazy initializing, new object at " << resaddr << " while evaluating expression\n";
                 }

              }  
              std::string type_str;
              llvm::raw_string_ostream rso(type_str);
              t->print(rso);
              llvm::errs() << "type in SE expr: " << rso.str() << "\n";
              st = dyn_cast<StructType>(t);
           }
        }
        else {
           assert(0 && "Unexpected dereferencing operator in the update expression!\n");
        }
    } 
    if (address)
       return addressRes;
    else
       return cur;   
}

bool SideEffectAPIHandler::assignsRetValue(APIAction &action) {
     return false;
}

bool SideEffectAPIHandler::interpret(APIAction &action, ExecutionState &state, std::vector< ref<Expr> > &arguments, 
      KInstruction *target, bool &term, int tid) {

   std::vector<std::string> pars = action.getParams();
   std::string fname = pars[0];
   llvm::outs() << "Handling side-effect api " << fname << "\n";
   Function *f = kmoduleExt->module->getFunction(fname);
   const DataLayout &dl = target->inst->getParent()->getParent()->getParent()->getDataLayout();
   std::string uexpr = pars[1];
      std::string lhs, rhs; 
      std::istringstream ss(uexpr);
      getline(ss, lhs, '=');
      getline(ss, rhs, '=');
      lhs = ltrim(rtrim(lhs));
      rhs = ltrim(rtrim(rhs));
      llvm::outs() << "lhs=" << lhs << "\n";
      llvm::outs() << "rhs=" << rhs << "\n";
      ref<Expr> addr = eval(state, dl, f, arguments, lhs, target, true);
      ref<Expr> value = eval(state, dl, f, arguments, rhs, target, false);
      llvm::outs() << "addr=" << addr << "\n";
      llvm::outs() << "value=" << value << "\n";
      // write value to addr
      bool asuccess;
      ObjectPair op; 
      ((Executor*)(theInterpreter))->solver->setTimeout(((Executor*)(theInterpreter))->coreSolverTimeout);
      if (!state.addressSpace.resolveOne(state, ((Executor*)(theInterpreter))->solver, addr, op, asuccess)) {
         addr = ((Executor*)(theInterpreter))->toConstant(state, addr, "resolveOne failure");
         asuccess = state.addressSpace.resolveOne(cast<ConstantExpr>(addr), op);
      }
      ((Executor*)(theInterpreter))->solver->setTimeout(0);             
      if (asuccess) {
         ObjectState *wos = state.addressSpace.getWriteable(op.first, op.second);
         ref<Expr> offsetexpr =  SubExpr::create(addr, op.first->getBaseExpr());
         llvm::outs() << "executing side-effect expr: " << uexpr << " and writing " << value << 
                         " to address " << op.first->getBaseExpr() << " at offset " << offsetexpr << "\n";
         wos->write(offsetexpr, value);         
      }      
   return true;
}


APIHandler::APIHandler() {
}

APIAction* APIHandler::lookup(std::string label) {
  if (symbolTable.find(label) != symbolTable.end())
     return symbolTable[label];
  return NULL;
}

bool APIHandler::assignsRetValue(APIAction &action) {
  return false;
}

bool APIHandler::handle(ExecutionState &state, 
                               std::vector<ExecutionState*> &branches,
                               std::string fname, 
                               std::vector< ref<Expr> > &arguments, 
                               KInstruction *target, 
                               int tid) {
  if (apiModel.find(fname) == apiModel.end()) {
     llvm::outs() << "external function " << fname << " not handled\n"; 
     return false;
  }
  Function *function = kmoduleExt->module->getFunction(fname);
  std::set<RetCond> conds;
  bool symbolizeRetValueOK = true;
  bool initArgs = false;
  for(auto action : apiModel[fname]) {
     conds.insert(action->getReturnCond());
     if (action->assignsRetValue())
        symbolizeRetValueOK = false;
     //if (action->setsArgs())
       // initArgs = false;
  }
  bool branching = false;
  for(auto c : conds) {
     if (c != TRUE) {
        branching = true;
        break; 
     }
  }
  bool term = false;
  if (!branching || !symbolizeRetValueOK) {
     if (branching && !symbolizeRetValueOK)
        llvm::outs() << "WARNING: api both branching and requesting not to symbolize the return value!\n";
     for(auto action : apiModel[fname]) {
        llvm::outs() << "executing action: \n";
        action->print();
        action->execute(state, arguments, target, term, tid);
     }
     if (initArgs) {
        llvm::outs() << "symbolizing args in handler for function " << function->getName() << "\n";
        ((Executor*)(theInterpreter))->symbolizeArguments(state, target, function, arguments); 
     }
     if (symbolizeRetValueOK) {
        llvm::outs() << "symbolizing ret value in handler for function " << function->getName() << "\n";
        ((Executor*)(theInterpreter))->symbolizeReturnValue(state, target, function);
     }
  }
  else {
   Function *f = kmoduleExt->module->getFunction(fname);
   assert(f);
   const MemoryObject *srt = ((Executor*)(theInterpreter))->symbolizeReturnValue(state, target, function);
   bool asuccess;
   ObjectPair op;
   if (srt) { 
    ref<Expr> srtbase = srt->getBaseExpr();
    ((Executor*)(theInterpreter))->solver->setTimeout(((Executor*)(theInterpreter))->coreSolverTimeout);
    if (!state.addressSpace.resolveOne(state, ((Executor*)(theInterpreter))->solver, srtbase, op, asuccess)) {
       srtbase = ((Executor*)(theInterpreter))->toConstant(state, srtbase, "resolveOne failure");
       asuccess = state.addressSpace.resolveOne(cast<ConstantExpr>(srtbase), op);
    }
    ((Executor*)(theInterpreter))->solver->setTimeout(0);             
    if (asuccess) {
       // generate all possible branches
       bool ltzero = conds.find(LTZERO) != conds.end();
       bool lteqzero = conds.find(LTEQZERO) != conds.end();
       bool gtzero = conds.find(GTZERO) != conds.end();
       bool gteqzero = conds.find(GTEQZERO) != conds.end();
       bool zero = conds.find(ZERO) != conds.end();
       bool notzero = conds.find(NOTZERO) != conds.end();
       std::map<RetCond, std::set<ExecutionState*> > partition;        
       std::set<ExecutionState*> empty;
       partition[ZERO] = empty;
       partition[LTZERO] = empty;
       partition[LTEQZERO] = empty;
       partition[GTZERO] = empty;
       partition[GTEQZERO] = empty;
       partition[NOTZERO] = empty;  
       Expr::Width retwidth = ((Executor*)(theInterpreter))->getWidthForLLVMType(target->inst->getType());
       ref<Expr> retexpr = op.second->read(0, retwidth);
       if ((ltzero && gtzero) || (lteqzero && gteqzero) || 
           ((zero || notzero) && (ltzero || lteqzero || gtzero || gteqzero))) {
          StatePair p = ((Executor*)(theInterpreter))->fork(state,
                  EqExpr::create(retexpr, ConstantExpr::create(0, retwidth)),
                  false);
          branches.push_back(p.first);
          partition[ZERO].insert(p.first);
          partition[LTEQZERO].insert(p.first);
          partition[GTEQZERO].insert(p.first);
          StatePair p2 = ((Executor*)(theInterpreter))->fork(*p.second, 
                     UltExpr::create(retexpr, ConstantExpr::create(0, retwidth)), 
                 false);           
          branches.push_back(p2.first);
          partition[LTZERO].insert(p2.first);
          partition[NOTZERO].insert(p2.first);
          partition[LTEQZERO].insert(p2.first);
          branches.push_back(p2.second);
          partition[GTZERO].insert(p2.second);
          partition[NOTZERO].insert(p2.second);
          partition[GTEQZERO].insert(p2.second);
          llvm::outs() << "generated =0, <0, and >0 partitions on the ret val\n";
       }
       else if (zero || notzero) { 
          StatePair p = ((Executor*)(theInterpreter))->fork(state,
               EqExpr::create(retexpr, ConstantExpr::create(0, retwidth)),
               false);
          branches.push_back(p.first);
          partition[ZERO].insert(p.first); 
          branches.push_back(p.second);
          partition[NOTZERO].insert(p.second);
          llvm::outs() << "generated =0 and <>0 partitions on the ret val\n";
      }
      else {
          if (lteqzero) {
             StatePair p = ((Executor*)(theInterpreter))->fork(state, 
                     UleExpr::create(retexpr, ConstantExpr::create(0, retwidth)), 
                   false);
             branches.push_back(p.first);
             partition[LTEQZERO].insert(p.first);
             branches.push_back(p.second);           
             partition[GTZERO].insert(p.second);
             llvm::outs() << "generated =<0 and >0 partitions on the ret val\n";
          }
          else if (gteqzero) {
             StatePair p = ((Executor*)(theInterpreter))->fork(state, 
                  UgeExpr::create(retexpr, ConstantExpr::create(0, retwidth)), 
                 false);
             branches.push_back(p.first);
             partition[GTEQZERO].insert(p.first);
             branches.push_back(p.second);           
             partition[LTZERO].insert(p.second);
             llvm::outs() << "generated <0 and >=0 partitions on the ret val\n";
         }        
         else {
           partition[TRUE].insert(&state);
           llvm::outs() << "No partitions generated on the ret val\n";
         }
      } 
      
      for(auto action : apiModel[fname]) {
         llvm::outs() << "executing branches in the partition " << action->getReturnCond() << "\n";
         int p = 0;
         for(auto branch : partition[action->getReturnCond()]) {
            bool term2 = false;
            action->execute(*branch, arguments, target, term2, tid);
            llvm::outs() << "partition " << p++ << "\n";
         }
      }
      if (initArgs)
         ((Executor*)(theInterpreter))->symbolizeArguments(state, target, function, arguments); 
     }
    }
  }
  return true;  
}

void APIHandler::addSubblock(std::string label, APIAction *action) {
   symbolTable[label] = action;
}

void APIHandler::addAction(std::string api, APIAction *action) {
  std::set<APIAction*> set;
  if (apiModel.find(api) != apiModel.end())
      set = apiModel[api];
  set.insert(action);
  apiModel[api] = set;
  llvm::outs() << "Added API action for " << api << "\n";
}

void read(std::istringstream& iss, std::vector<std::string> &par) {
     std::string item = "";
     while(getline(iss, item, ',')) {
       item = ltrim(rtrim(item));
       par.push_back(item);
    }
      
}

void APIHandler::readProgModelSpec(const char *name) {
  allocAPIHandler = new AllocAPIHandler();
  ignoreAPIHandler = new IgnoreAPIHandler();
  callbackAPIHandler = new CallbackAPIHandler();
  refcountAPIHandler = new RefCountAPIHandler();
  freeAPIHandler = new FreeAPIHandler();
  sideEffectAPIHandler = new SideEffectAPIHandler();
  terminateAPIHandler = new TerminateAPIHandler();
  std::fstream cf(name, std::fstream::in);
  if (cf.is_open()) {
     std::string line, desc, data;
     APIBlock *apiblock = NULL;
     std::string mainapi = "";
     std::string subblocklabel = ""; 
     RetCond rcond = TRUE; 
     while(std::getline(cf,line)) { 
       desc = data = "";  
       std::istringstream iss(line);
       getline(iss, desc, ':');
       desc = ltrim(rtrim(desc));
       getline(iss, data);
       llvm::outs() << desc << ":" << data << "\n";
       if (desc == "apiblock") {
          std::string rcondst;
          std::istringstream iss2(data);
          getline(iss2, mainapi, ',');
          getline(iss2, rcondst, ','); 
          mainapi = ltrim(rtrim(mainapi));
          rcondst = ltrim(rtrim(rcondst));           
          if (rcondst == "zero")
             rcond = ZERO;
          else if (rcondst == "ltzero")
             rcond = LTZERO;
          else if (rcondst == "lteqzero")
             rcond = LTEQZERO;
          else if (rcondst == "gtzero")
             rcond = GTZERO;
          else if (rcondst == "gteqzero")
             rcond = GTEQZERO;
          else if (rcondst == "nonzero")
             rcond = NOTZERO;
          apiblock = new APIBlock();
          apiblock->setReturnCond(rcond);
       }
       else if (desc == "endapiblock") {
          addAction(mainapi, apiblock);  
          mainapi = "";        
          apiblock = NULL;
          rcond = TRUE;
       } 
       else if (desc == "apisubblock") {
          std::istringstream iss2(data); 
          getline(iss2, mainapi, ',');
          getline(iss2, subblocklabel, ',');
          mainapi = ltrim(rtrim(mainapi));
          subblocklabel = ltrim(rtrim(subblocklabel));
          apiblock = new APIBlock();
       }
       else if (desc == "endapisubblock") {
          addSubblock(subblocklabel, apiblock);
          mainapi = "";  
          subblocklabel = "";
          apiblock = NULL;
          rcond = TRUE;
       }
       else if (desc == "assembly") {
          std::string afunc;
          afunc = ltrim(rtrim(data));
          assemblyFunctions.insert(afunc);
          llvm::outs() << "assembly " << afunc << "\n"; 
       }
       else {
          std::vector<std::string> par;
          std::istringstream iss2(data);
          read(iss2, par);
          APIAction *a = NULL;
          if (desc == "terminate") {
             if (par.size() == 1)
                par.insert(par.begin(), mainapi);
             a = new APIAction(desc, par, terminateAPIHandler);
             llvm::outs() << "registering terminate expr "; a->print();
             assert(par.size() == 2 && "Incorrect number of params for terminate spec!");
          }
          else if (desc == "side-effect") {
             if (par.size() == 1)
                par.insert(par.begin(), mainapi);
             a = new APIAction(desc,par,sideEffectAPIHandler);
             llvm::outs() << "registering sideeffect expr "; a->print();
             assert(par.size() == 2 && "Incorrect number of params for side-effect spec!");
          }
          else if (desc == "alloc,parameter,initzero,symbolic,return,symname") {
             if (par.size() == 5)
                par.insert(par.begin(), mainapi); 
             a = new APIAction(desc,par,allocAPIHandler);
             llvm::outs() << " alloc,parameter,initzero,sym interface "; a->print();
             assert(par.size() == 6 && "Incorrect number of params for alloc spec!"); 
          }
          else if (desc == "free,parameter") {
             if (par.size() == 1)
                par.insert(par.begin(), mainapi); 
             a = new APIAction(desc, par, freeAPIHandler);
             llvm::outs() << " free,parameter interface "; a->print(); 
             assert(par.size() == 2 && "Incorrect number of params for free spec!");
          }
          else if (desc == "refcount,op,expr,value,onzerocb") {
             if (par.size() == 4)
                par.insert(par.begin(), mainapi); 
             a = new APIAction(desc, par, refcountAPIHandler);
             llvm::outs() << " refcount,op,expr,value,onzerocb interface "; a->print();
             assert(par.size() == 5 && "Incorrect number of params for refcount spec!");
          }
          else if (desc == "callback") {
             if (par.size() == 1)
                par.insert(par.begin(), mainapi); 
             a = new APIAction(desc, par, callbackAPIHandler); 
             llvm::outs() << "callback interface "; a->print();
             assert(par.size() == 2 && "Incorrect number of params for callback spec!");
          }
          else if (desc == "ignore") {
             if (par.size() == 0)
                par.insert(par.begin(), mainapi); 
             a = new APIAction(desc,par,ignoreAPIHandler);
             llvm::outs() << "ignore interface "; a->print();
             assert(par.size() == 1 && "Incorrect number of params for ignore spec!");
          }
          if (apiblock) {
             if (a) {
                apiblock->addNextAction(a, rcond);
                a = NULL;
             }
          }
          else {
             if (a) {
                addAction(par[0],a);
                a = NULL;
             }
          }
       }
      
     }
     llvm::outs() << "Parsed Prog Model Spec \n";     
     for(auto acs : apiModel) {
        llvm::outs() << acs.first << ":\n";
        for(auto ac : acs.second) {
           ac->print();
        }
     } 
     for(auto st : symbolTable) {
       llvm::outs() << "subblock labeled " << st.first << "\n";
       st.second->print();
     }
     cf.close();
  }
}


bool APIHandler::interpret(APIAction &action, ExecutionState &state, std::vector< ref<Expr> > &arguments, 
            KInstruction *target, bool &term, int tid) {
     llvm::outs() << "Superclass interpret should not be executing!\n";
     return true;
}


bool APIHandler::setsArgs() {
   return true;
}


APIAction::APIAction() {
  handler = NULL;
  terminated = false;
}

       
APIAction::APIAction(std::string desc, std::vector<std::string> param, APIHandler *handler, RetCond retCond) {
   this->desc = desc;
   this->param = param;
   this->handler = handler;
   returnCondition = retCond;
   terminated = false;
}

bool APIAction::hasTerminated() {
  return terminated;
}

void APIAction::terminate() {
  terminated = true;
}

std::vector<std::string> APIAction::getParams() {
    return param;
}

RetCond APIAction::getReturnCond() {
   return returnCondition;
}

void APIAction::setReturnCond(RetCond rt) {
  returnCondition = rt;
}
       
bool APIAction::assignsRetValue() {
   return handler->assignsRetValue(*this);
}

bool APIAction::setsArgs() {
   return handler->setsArgs();
}


void APIAction::execute(ExecutionState &state, std::vector< ref<Expr> > &arguments, 
          KInstruction *target, bool &term, int tid) {
  handler->interpret(*this, state, arguments, target, term, tid);
}

void APIAction::print() {
   llvm::outs() << desc << ":" ;
   for(auto p : param) {
      llvm::outs() << p << ",";
   }
   llvm::outs() << "\n";
}

APIBlock::APIBlock() : APIAction() {
  returnCondition = TRUE;
}

void APIBlock::addNextAction(APIAction *action, RetCond rcond) {
   action->setReturnCond(rcond);
   body.push_back(action);
}

bool APIBlock::assignsRetValue() {
   for(auto action : body) {
       if (action->assignsRetValue())
          return true;
   }
   return false;
}

bool APIBlock::setsArgs() {
   for(auto action : body) {
       if (action->setsArgs())
          return true;
   }
   return false;
}

void APIBlock::execute(ExecutionState &state, std::vector< ref<Expr> > &arguments, 
         KInstruction *target, bool &term, int tid) {
     term = false;
     for(auto action : body) {
        if (!term)  {
           bool term2 = false;
           action->execute(state, arguments, target, term2, tid);
           if (term2)
              term = term2;
        }
     }
}

void APIBlock::print() {
     llvm::outs() << "Return condition=" << returnCondition << "\n";
     for(auto action : body) {
        action->print();
     }
}

/* SYSREL EXTENSION */


