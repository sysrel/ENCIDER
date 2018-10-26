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
std::map<std::string, std::set<APIAction*>> APIHandler::apiModel;
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


RefCountAPIHandler::RefCountAPIHandler() {}



bool RefCountAPIHandler::handle(std::map<ref<Expr>, int> &stateModel, std::string fname, std::vector< ref<Expr> > &arguments){   

   if (initSet.find(fname) != initSet.end()) {
      ConstantExpr *ce = dyn_cast<ConstantExpr>(arguments[1]);
      if (ce) {   
         stateModel[arguments[0]] = ce->getZExtValue();
      }
      else assert(false && "Unexpected symbolic value to ref count init function\n");
      return true;
   }
   else if (getputMap.find(fname) != getputMap.end()) {
      if (initMap.find(fname) == initMap.end())
         stateModel[arguments[0]] = 1;
      else
         stateModel[arguments[0]] = stateModel[arguments[0]] + 1;
     return true;
   }
   else if (putSet.find(fname) != putSet.end()) {
     if (stateModel.find(arguments[0]) == stateModel.end())
        assert(false && "Refcount decrement without an increment\n");
     stateModel[arguments[0]] = stateModel[arguments[0]] - 1;
     if (stateModel[arguments[0]] < 0)
        assert(false && "refcount became negative \n");
     return true;
   }
   return false;
};


     
RegistrationAPIHandler::RegistrationAPIHandler() {}

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



ResourceAllocReleaseAPIHandler::ResourceAllocReleaseAPIHandler() {}


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

SetGetAPIHandler::SetGetAPIHandler() {
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

AllocAPIHandler::AllocAPIHandler() {
}

bool AllocAPIHandler::assignsRetValue(APIAction &action) {
     std::vector<std::string> pars = action.getParams();
     llvm::outs() << "Alloc API assigns to return value? " << pars[4] << "entered\n";
     return (pars[4] == "true");   
}

bool AllocAPIHandler::interpret(APIAction &action, ExecutionState &state,
         std::vector< ref<Expr> > &arguments, KInstruction *target, int tid) {
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


FreeAPIHandler::FreeAPIHandler() {}

bool FreeAPIHandler::assignsRetValue(APIAction &action) {
  return false;
}

       
bool FreeAPIHandler::interpret(APIAction &action, ExecutionState &state, 
          std::vector< ref<Expr> > &arguments, KInstruction *target, int tid) {
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

ReadWriteAPIHandler::ReadWriteAPIHandler() {
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

IgnoreAPIHandler::IgnoreAPIHandler() {
}


bool IgnoreAPIHandler::interpret(APIAction &action, ExecutionState &state, std::vector< ref<Expr> > &arguments, KInstruction *target, int tid) {
    std::vector<std::string> pars = action.getParams();
    llvm::outs() << "Handling ignore api " << pars[0] << "\n";
    return true;
}

bool IgnoreAPIHandler::assignsRetValue(APIAction &action) {
  return false;
}

/*

CallbackAPIHandler::CallbackAPIHandler() {}

bool CallbackAPIHandler::handle(ExecutionState &state, int tid, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target) {
   if (cbAPI.find(fname) == cbAPI.end())
      return false;
   llvm::outs() << "calling callback " << cbAPI[fname] << "\n";
   Function *f = kmoduleExt->module->getFunction(cbAPI[fname]);
   assert(f && "Callback function not defined!\n");
   llvm::outs() << "simulating callback " << cbAPI[fname] << " with return instr " << (*state.prevPC->inst) << "\n";   
   KFunction *kf = kmoduleExt->functionMap[f];      
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
   return true; 
}
 
void CallbackAPIHandler::addCallback(std::string api, std::string cb) {
   if (cbAPI.find(api) != cbAPI.end()) {
       llvm::errs() << "WARNING:  " << api << "->" << cbAPI[api] << " (API->callback) overwritten!\n";
   }
   cbAPI[api] = cb;
}

bool CallbackAPIHandler::handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target) {
   return handle(state, -1, fname, arguments, target);
}
     
bool CallbackAPIHandler::handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target, int tid) {              
   return handle(state, tid, fname, arguments, target);
}
*/

SideEffectAPIHandler::SideEffectAPIHandler() {}

// syntax argi([-> | .]fieldindex)+
// to do: handle the * operator
ref<Expr> APIHandler::eval(ExecutionState &state,  const DataLayout &dl, Function *f, 
     std::vector< ref<Expr> > &arguments, std::string expr, bool address) {
// try {
    auto pos = expr.find("arg");
    if (pos == std::string::npos) {
       llvm::outs() <<  expr << "\n";
       assert(0 && "expression does not refer to an argument");
    }
    unsigned i; 
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
    llvm::Type *t = NULL;
    llvm::StructType *st = NULL;
    int j = 0;
    for(llvm::Argument & arg : f->args()) {
       if (j == argindex) {
           t = arg.getType();
           break;
       }
       j++;
    }
    if (t->isPointerTy()) {
       t = t->getPointerElementType(); 
    } 
    st = dyn_cast<StructType>(t);
    ref<Expr> cur = arguments[argindex];
    llvm::errs() << "In SE:  arguments[" << argindex << "] is " <<  cur << "\n";
    int last = 0;
    std::string field;
    int fieldIndex; 
    while (i < expr.length()) {
        llvm::errs() <<"parsing loop \n";
        if ((expr[i] == '-' || expr[i] == '.')) {
           if (!st)
              assert(0 && "Dereferencing into a non-struct type pointer!\n");
           if (expr[i] == '-') {
              assert(expr[i+1] == '>' && "Unexpected dereferencing operator in the update expression!\n");
              /*bool asuccess;
              ObjectPair op;
              ((Executor*)(theInterpreter))->solver->setTimeout(((Executor*)(theInterpreter))->coreSolverTimeout);
              if (!state.addressSpace.resolveOne(state, ((Executor*)(theInterpreter))->solver, cur, op, asuccess)) {
                 cur = ((Executor*)(theInterpreter))->toConstant(state, cur, "resolveOne failure");
                 asuccess = state.addressSpace.resolveOne(cast<ConstantExpr>(cur), op);
              }
              ((Executor*)(theInterpreter))->solver->setTimeout(0);             
              if (asuccess) {
                 cur = op.second->read(0,op.first->size);
                // read the field string           
              }
              else assert(0 && "Could not find memory object for update expression!");*/
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
              llvm::outs() << "using " << cur << "\n"; 
              cur = AddExpr::create(ConstantExpr::create(offset, Expr::Int64), cur);
              llvm::outs() << "computed addr " << cur << "\n";
              t = st->getElementType(fieldIndex);
              if (t->isPointerTy())
                 t = t->getPointerElementType();  
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
    return cur;   
 //}
 //catch (std::invalid_argument e) {
   //    assert(0 && "argument index not a number in the update expression!\n");
 //}     
}

bool SideEffectAPIHandler::assignsRetValue(APIAction &action) {
     return false;
}

bool SideEffectAPIHandler::interpret(APIAction &action, ExecutionState &state, std::vector< ref<Expr> > &arguments, KInstruction *target, int tid) {
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
      ref<Expr> addr = eval(state, dl, f, arguments, lhs, true);
      ref<Expr> value = eval(state, dl, f, arguments, rhs, false);
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


APIHandler::APIHandler() {}

bool APIHandler::assignsRetValue(APIAction &action) {
  return false;
}

bool APIHandler::handle(ExecutionState &state, 
                               std::vector<ExecutionState*> &branches,
                               std::string fname, 
                               std::vector< ref<Expr> > &arguments, 
                               KInstruction *target, 
                               int tid) {
  if (apiModel.find(fname) == apiModel.end())
     return false;
  Function *function = kmoduleExt->module->getFunction(fname);
  std::set<RetCond> conds;
  bool symbolizeRetValueOK = true;
  bool initArgs = true;
  for(auto action : apiModel[fname]) {
     conds.insert(action->getReturnCond());
     if (action->assignsRetValue())
        symbolizeRetValueOK = false;
     if (action->setsArgs())
        initArgs = false;
  }
  bool branching = false;
  for(auto c : conds) {
     if (c != TRUE) {
        branching = true;
        break; 
     }
  }
  if (!branching || !symbolizeRetValueOK) {
     if (branching && !symbolizeRetValueOK)
        llvm::outs() << "WARNING: api both branching and requesting not to symbolize the return value!\n";
     for(auto action : apiModel[fname]) {
        llvm::outs() << "executing action: \n";
        action->print();
        action->execute(state, arguments, target, tid);
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
    if (srt) {
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
       if ((ltzero && gtzero) || (lteqzero && gteqzero) || 
           ((zero || notzero) && (ltzero || lteqzero || gtzero || gteqzero))) {
          ref<Expr> brexpr = EqExpr::create(srt->getBaseExpr(), 0);
          StatePair p = ((Executor*)(theInterpreter))->fork(state,brexpr,false);
          branches.push_back(p.first);
          partition[ZERO].insert(p.first);
          partition[LTEQZERO].insert(p.first);
          partition[GTEQZERO].insert(p.first);
          brexpr = UltExpr::create(srt->getBaseExpr(), 0);
          StatePair p2 = ((Executor*)(theInterpreter))->fork(*p.second, brexpr, false);           
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
          ref<Expr> brexpr = EqExpr::create(srt->getBaseExpr(), 0);
          StatePair p = ((Executor*)(theInterpreter))->fork(state,brexpr,false);
          branches.push_back(p.first);
          partition[ZERO].insert(p.first); 
          branches.push_back(p.second);
          partition[NOTZERO].insert(p.second);
          llvm::outs() << "generated =0 and <>0 partitions on the ret val\n";
      }
      else {
          if (lteqzero) {
             ref<Expr> brexpr = UleExpr::create(srt->getBaseExpr(), 0);
             StatePair p = ((Executor*)(theInterpreter))->fork(*p.second, brexpr, false);
             branches.push_back(p.first);
             partition[LTEQZERO].insert(p.first);
             branches.push_back(p.second);           
             partition[GTZERO].insert(p.second);
             llvm::outs() << "generated =<0 and >0 partitions on the ret val\n";
          }
          else if (gteqzero) {
             ref<Expr> brexpr = UgeExpr::create(srt->getBaseExpr(), 0);
             StatePair p = ((Executor*)(theInterpreter))->fork(*p.second, brexpr, false);
             branches.push_back(p.first);
             partition[GTEQZERO].insert(p.first);
             branches.push_back(p.second);           
             partition[LTZERO].insert(p.second);
             llvm::outs() << "generated <0 and >=0 partitions on the ret val\n";
         }        
         else assert(0 && "No partitions generated on the ret val\n");
      } 
      
      for(auto action : apiModel[fname]) {
         llvm::outs() << "executing branches in the partition " << action->getReturnCond() << "\n";
         int p = 0;
         for(auto branch : partition[action->getReturnCond()]) {
            action->execute(*branch, arguments, target, tid);
            llvm::outs() << "partition " << p++ << "\n";
         }
      }
      if (initArgs)
         ((Executor*)(theInterpreter))->symbolizeArguments(state, target, function, arguments); 
    }
  }
  return true;  
}

void APIHandler::addAction(std::string api, APIAction *action) {
  std::set<APIAction*> set;
  if (apiModel.find(api) != apiModel.end())
      set = apiModel[api];
  set.insert(action);
  apiModel[api] = set;
}

void APIHandler::readProgModelSpec(const char *name) {
  //regAPIHandler = new RegistrationAPIHandler();
  //resADAPIHandler = new ResourceAllocReleaseAPIHandler();
  //mutexAPIHandler = new MutexAPIHandler();
  //refcountAPIHandler = new RefCountAPIHandler();
  //setgetAPIHandler = new SetGetAPIHandler();
  allocAPIHandler = new AllocAPIHandler();
  //readWriteAPIHandler = new ReadWriteAPIHandler();
  ignoreAPIHandler = new IgnoreAPIHandler();
  //callbackAPIHandler = new CallbackAPIHandler();
  freeAPIHandler = new FreeAPIHandler();
  sideEffectAPIHandler = new SideEffectAPIHandler();
  std::fstream cf(name, std::fstream::in);
  if (cf.is_open()) {
     std::string line, desc, data;
     while(std::getline(cf,line)) { 
       std::istringstream iss(line);
       getline(iss, desc, ':');
       getline(iss, data);
       llvm::outs() << desc << ":" << data << "\n";
       if (desc.find("assembly") != std::string::npos) {
          std::string afunc;
          afunc = ltrim(rtrim(data));
          assemblyFunctions.insert(afunc);
          llvm::outs() << "assembly " << afunc << "\n"; 
       }
       else if (desc.find("side-effect") != std::string::npos) {
          std::string api, expr;
          std::istringstream iss2(data);
          getline(iss2, api, ',');
          getline(iss2, expr, ',');
          api = ltrim(rtrim(api));
          expr = ltrim(rtrim(expr));
          llvm::outs() << "registering sideeffect expr " << expr << " for " << api << "\n";
          std::vector<std::string> par;
          par.push_back(api);
          par.push_back(expr);
          APIAction *a = new APIAction(desc,par,sideEffectAPIHandler);
          addAction(api,a);
       }
       /*else if (desc.find("register,deregister[enable]") != std::string::npos) {
          std::string reg, unreg, el, enabled; 
          std::istringstream iss2(data);
          getline(iss2, reg, ',');
          getline(iss2, unreg, ',');
          getline(iss2, el, ',');
          std::set<std::string> enabledSet;
          if (el.find("[]") == std::string::npos) {
             std::istringstream iss3(el);
             while (getline(iss3, enabled, ',')) {
               enabledSet.insert(enabled);
             }
          } 
          regAPIHandler->addRegUnreg(ltrim(rtrim(reg)), ltrim(rtrim(unreg)), enabledSet);          
       }
       else if (desc.find("alloc,dealloc") != std::string::npos) {
         std::string alloc, dealloc;
         std::istringstream iss2(data);
         getline(iss2, alloc, ',');
         getline(iss2, dealloc, ',');
         std::vector<std::string> param;
          param.push_back(a
         resADAPIHandler->addResAllocDealloc(ltrim(rtrim(alloc)), ltrim(rtrim(dealloc)));   
       }
       else if (desc.find("acquire,release,init") != std::string::npos) {
         std::string acq, rel, init;
         std::istringstream iss2(data);
         getline(iss2, acq, ',');
         getline(iss2, rel, ',');
         getline(iss2, init, ',');
         mutexAPIHandler->addInitAcqRel(ltrim(rtrim(acq)), ltrim(rtrim(rel)), ltrim(rtrim(init)));
       / 
       else if (desc.find("get,put,init") != std::string::npos) {
         std::string get, put, init;
         std::istringstream iss2(data);
         getline(iss2, get, ',');
         getline(iss2, put, ',');
         getline(iss2, init, ',');
         refcountAPIHandler->addGetPut(ltrim(rtrim(get)), ltrim(rtrim(put)), ltrim(rtrim(init)));
       }
       else if (desc.find("set,get") != std::string::npos) {
         std::string get, set, init;
         std::istringstream iss2(data);
         getline(iss2, set, ',');
         getline(iss2, get, ',');
         set = ltrim(rtrim(set));
         get = ltrim(rtrim(get));
         setgetAPIHandler->addSetGet(set, get);        
         llvm::outs() << " set/get interface: " << set << " " << get << "\n";
       }*/
       else if (desc.find("alloc,parameter,initzero,symbolic,return") != std::string::npos) {
         std::string alloc, param, init, sym, ret;
         std::istringstream iss2(data);
         getline(iss2, alloc, ',');
         getline(iss2, param, ',');
         getline(iss2, init, ',');
         getline(iss2, sym, ',');
         getline(iss2, ret, ',');
         alloc = ltrim(rtrim(alloc));
         param = ltrim(rtrim(param));
         init = ltrim(rtrim(init)) ;
         sym =  ltrim(rtrim(sym));
         ret = ltrim(rtrim(ret));
         std::vector<std::string> par;
         par.push_back(alloc);
         par.push_back(param);
         par.push_back(init);
         par.push_back(sym);
         par.push_back(ret); 
         APIAction *a = new APIAction(desc,par,allocAPIHandler);
         addAction(alloc,a);
         llvm::outs() << " alloc,parameter,initzero,sym interface " << alloc << " " << param  << " " << init << " " << sym << " " << ret << "\n";
       }
       else if (desc.find("free,parameter") != std::string::npos) {
         std::string freeapi,  param;
         std::istringstream iss2(data);
         getline(iss2, freeapi, ',');
         getline(iss2, param, ',');
         freeapi = ltrim(rtrim(freeapi));
         param = ltrim(rtrim(param)); 
         std::vector<std::string> par;
         par.push_back(freeapi);
         par.push_back(param);
         APIAction *a = new APIAction(desc, par, freeAPIHandler);
         addAction(freeapi,a);
         llvm::outs() << " free,parameter interface " << freeapi << " " << param << "\n"; 
       }
       /*else if (desc.find("callback") !=  std::string::npos) {
         std::string api, cb;
         std::istringstream iss2(data);
         getline(iss2, api, ',');
         getline(iss2, cb, ',');
         callbackAPIHandler->addCallback(ltrim(rtrim(api)), ltrim(rtrim(cb))); 
         llvm::outs() << "callback interface " << api << " " << cb << "\n"; 
       }
       else if (desc.find("write") != std::string::npos) {
          if (desc.find("write,source,dest") != std::string::npos) {
             std::string writer, source, dest;
             std::istringstream iss2(data);
             getline(iss2, writer, ',');
             getline(iss2, source, ',');
             getline(iss2, dest, ',');
             readWriteAPIHandler->addWrite(ltrim(rtrim(writer)), std::stoi(ltrim(rtrim(source))),
                      std::stoi(ltrim(rtrim(dest))));
          }
          else readWriteAPIHandler->addWrite(ltrim(rtrim(data)));
       }
       else if (desc.find("read") != std::string::npos) {
         readWriteAPIHandler->addRead(ltrim(rtrim(data)));
       }*/
       else if (desc.find("ignore") != std::string::npos) {
         data = ltrim(rtrim(data));
         std::vector<std::string> par;
         par.push_back(data);
         APIAction *a = new APIAction(desc,par,ignoreAPIHandler);
         addAction(data,a);
       }
     }
     llvm::outs() << "Parsed Prog Model Spec \n";      
     cf.close();
  }
}


bool APIHandler::interpret(APIAction &action, ExecutionState &state, std::vector< ref<Expr> > &arguments, 
            KInstruction *target, int tid) {
     llvm::outs() << "Superclass interpret should not be executing!\n";
     return true;
}


bool APIHandler::setsArgs() {
   return true;
}


APIAction::APIAction() {
  handler = NULL;
}

       
APIAction::APIAction(std::string desc, std::vector<std::string> param, APIHandler *handler, RetCond retCond) {
   this->desc = desc;
   this->param = param;
   this->handler = handler;
   returnCondition = retCond;
   condition = NONE;
}

APIAction::APIAction(std::string desc, std::vector<std::string> param, Cond condition, std::vector<std::string> conditionArg,
                APIHandler *handler, RetCond retCond) {
   this->desc = desc;
   this->param = param;
   this->handler = handler;
   this->condition = condition;
   this->conditionArg = conditionArg;
   returnCondition = retCond;
}

std::vector<std::string> APIAction::getParams() {
    return param;
}

RetCond APIAction::getReturnCond() {
   return returnCondition;
}
       
bool APIAction::assignsRetValue() {
   return handler->assignsRetValue(*this);
}

bool APIAction::setsArgs() {
   return handler->setsArgs();
}

void  APIAction::addChainAction(APIAction *action) {
   chain.push_back(action);
}

void APIAction::execute(ExecutionState &state, std::vector< ref<Expr> > &arguments, KInstruction *target, int tid) {
  handler->interpret(*this, state, arguments, target, tid);
}

void APIAction::print() {
   llvm::outs() << desc << ":" ;
   for(auto p : param) {
      llvm::outs() << p << ",";
   }
   llvm::outs() << "\n";
}

APIBlock::APIBlock() : APIAction() {
}

void APIBlock::addNextAction(APIAction *action) {
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

void APIBlock::execute(ExecutionState &state, std::vector< ref<Expr> > &arguments, KInstruction *target, int tid) {
     for(auto action : body) {
        action->execute(state, arguments, target, tid);
     }
}

void APIBlock::print() {
     for(auto action : body) {
        action->print();
     }
}

/* SYSREL EXTENSION */



