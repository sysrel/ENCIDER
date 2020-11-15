//===-- ExecutionState.cpp ------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/ExecutionState.h"

#include "klee/Internal/Module/Cell.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/KModule.h"

#include "klee/Expr.h"
#include "klee/sysrel.h"

#include "Memory.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

#include <iomanip>
#include <sstream>
#include <cassert>
#include <map>
#include <set>
#include <stdarg.h>

/* SYSREL extension */
#include "Executor.h"
#include "StatsTracker.h"
#include "llvm/IR/Module.h"
#include "MemoryManager.h"
/* SYSREL extension end */

using namespace llvm;
using namespace klee;

namespace { 
  cl::opt<bool>
  DebugLogStateMerge("debug-log-state-merge");
}

/* SYSREL */
void recordMemObj(ExecutionState &state, const MemoryObject *mo);
extern KModule *kmoduleExt;
extern Interpreter *theInterpreter;
int ExecutionState::counter=0;
Sequential *LifeCycleModelState::lcm = NULL;
extern std::string entryFunctionName;
extern const Module * moduleHandle;

extern bool lazyInit;
extern bool lazySpec;
extern int numLazyInst;
extern std::set<std::string> lazyInits;
extern std::set<std::string> lazyInitSingles;
extern std::map<std::string, int> lazyInitNumInstances;

extern bool isLazyInit(Type *t, bool &single, int &count);
extern bool isAllocTypeLazyInit(Type *t, bool &single, int &count);
extern void cloneHighMemoryRegions(const ExecutionState &from, ExecutionState &to);
extern void cloneLowMemoryRegions(const ExecutionState &from, ExecutionState &to);
extern std::map<long, std::map<ref<Expr>, const MemoryObject *> > addressToMemObj;
extern std::map<long, std::map<ref<Expr>, ref<Expr> > > symIndexToMemBase;
extern std::map<long, std::map<std::string, unsigned> > forkFreqMapTrue;
extern std::map<long, std::map<std::string, unsigned> > forkFreqMapFalse;
/* SYSREL */

/***/

StackFrame::StackFrame(KInstIterator _caller, KFunction *_kf)
  : caller(_caller), kf(_kf), callPathNode(0), 
    minDistToUncoveredOnReturn(0), varargs(0) {
  locals = new Cell[kf->numRegisters];
}

StackFrame::StackFrame(const StackFrame &s) 
  : caller(s.caller),
    kf(s.kf),
    callPathNode(s.callPathNode),
    allocas(s.allocas),
    minDistToUncoveredOnReturn(s.minDistToUncoveredOnReturn),
    varargs(s.varargs) {
  locals = new Cell[s.kf->numRegisters];
  for (unsigned i=0; i<s.kf->numRegisters; i++)
    locals[i] = s.locals[i];
}

StackFrame::~StackFrame() { 
  delete[] locals; 
}

/***/

ExecutionState::ExecutionState(KFunction *kf) :
    instCount(0),
    pc(kf->instructions),
    prevPC(pc),

    queryCost(0.), 
    weight(1),
    depth(0),

    instsSinceCovNew(0),
    coveredNew(false),
    forkDisabled(false),
    ptreeNode(0),
    steppedInstructions(0) {
   /* SYSREL */
  pushFrame(0, kf);
  /* SYSREL */
  threadsQueue.push(-1);
  rtid = -1;
  waitingForThreadsToTerminate = false;
  id = counter++;
  if (LifeCycleModelState::lcm)
    lcmState = new LifeCycleModelState();
  else lcmState = NULL;
  pmstack.clear();
  inEnclave = true;
  lastEnclaveFunction = "";
  secretDescendant = false;
  std::map<ref<Expr>, ref<Expr> > em1;
  symIndexToMemBase[(long)this] = em1;
  std::map<ref<Expr>, const MemoryObject*> em2;
  addressToMemObj[(long)this] = em2;
  std::map<std::string, unsigned> fm;
  forkFreqMapTrue[(long)this] = fm;
  forkFreqMapFalse[(long)this] = fm;
  timingObservationPoint = false;
  /* SYSREL */  
}

ExecutionState::ExecutionState(const std::vector<ref<Expr> > &assumptions)
    : instCount(0), constraints(assumptions), queryCost(0.), ptreeNode(0) {
   /* SYSREL */
  threadsQueue.push(-1);
  rtid = -1;
  waitingForThreadsToTerminate = false;
  id = counter++;
  if (LifeCycleModelState::lcm)
    lcmState = new LifeCycleModelState();
  pmstack.clear();
  inEnclave = true;
  lastEnclaveFunction = "";
  secretDescendant = false;
  std::map<ref<Expr>, ref<Expr> > em1;
  symIndexToMemBase[(long)this] = em1;
  std::map<ref<Expr>, const MemoryObject*> em2;
  addressToMemObj[(long)this] = em2;
  std::map<std::string, unsigned> fm;
  forkFreqMapTrue[(long)this] = fm;
  forkFreqMapFalse[(long)this] = fm;
  timingObservationPoint = false;
  /* SYSREL */  
}

/* SYSREL */
void ExecutionState::extendExecutionWith(KFunction *kf) {
  KInstIterator temp(kf->instructions);
  pc = temp;
  prevPC = pc;
  CallPathNode *prev = stack.back().callPathNode;
  popFrame(); // removes the locals of the previous function
  pushFrame(0, kf);
  stack.back().callPathNode = prev; 
  // allocate symbolic arguments
  bool abort = false;
  ((Executor*)theInterpreter)->initArgsAsSymbolic(*this, kf->function, abort);
  assert(!abort);
  #ifdef VB
  llvm::outs() << "extending execution with the first instruction \n";
  pc->inst->print(llvm::outs());
  #endif
}
/* SYSREL */

ExecutionState::~ExecutionState() {
  for (unsigned int i=0; i<symbolics.size(); i++)
  {
    const MemoryObject *mo = symbolics[i].first;
    assert(mo->refCount > 0);
    mo->refCount--;
    if (mo->refCount == 0)
      delete mo;
  }

  for (auto cur_mergehandler: openMergeStack){
    cur_mergehandler->removeOpenState(this);
  }


  while (!stack.empty()) popFrame();
}

ExecutionState::ExecutionState(const ExecutionState& state):
		rdid(state.rdid),
    instCount(state.instCount),
    fnAliases(state.fnAliases),
    pc(state.pc),
    prevPC(state.prevPC),
    stack(state.stack),
    incomingBBIndex(state.incomingBBIndex),

    addressSpace(state.addressSpace),
    constraints(state.constraints),

    queryCost(state.queryCost),
    weight(state.weight),
    depth(state.depth),

    pathOS(state.pathOS),
    symPathOS(state.symPathOS),

    instsSinceCovNew(state.instsSinceCovNew),
    coveredNew(state.coveredNew),
    forkDisabled(state.forkDisabled),
    coveredLines(state.coveredLines),
    ptreeNode(state.ptreeNode),
    symbolics(state.symbolics),
    arrayNames(state.arrayNames),
    openMergeStack(state.openMergeStack),
    steppedInstructions(state.steppedInstructions)
{
  for (unsigned int i=0; i<symbolics.size(); i++)
    symbolics[i].first->refCount++;

  for (auto cur_mergehandler: openMergeStack)
    cur_mergehandler->addOpenState(this);

  /* SYSREL */
  lastCondition = state.lastCondition;
  threadsQueue = state.threadsQueue; 
  rtid = state.rtid;
  waitingForThreadsToTerminate = state.waitingForThreadsToTerminate;
  for(unsigned int i=0; i < state.threads.size(); i++) {
     threads.push_back(state.threads[i]);
     threads[i].context = this;
  }
  id = counter++;
  if (LifeCycleModelState::lcm)
    lcmState = new LifeCycleModelState(*state.lcmState);
  typeToAddr = state.typeToAddr;
  refCountModel = state.refCountModel;
  refCountStack = state.refCountStack;
  symbolDefs = state.symbolDefs;
  symbolTypes = state.symbolTypes;
  symIdCounters = state.symIdCounters;
  lazyInitSingleInstances = state.lazyInitSingleInstances;
  for(auto pmf : pmstack) 
     pmstack.push_back(new PMFrame(*pmf));
  inEnclave = state.inEnclave;
  lastEnclaveFunction = state.lastEnclaveFunction;
  secretDescendant = state.secretDescendant;
  if (symIndexToMemBase.find((long)&state) != symIndexToMemBase.end())
    symIndexToMemBase[(long)this] = symIndexToMemBase[(long)&state];
  if (addressToMemObj.find((long)&state) != addressToMemObj.end())
    addressToMemObj[(long)this] = addressToMemObj[(long)&state];
  if (forkFreqMapTrue.find((long)&state) != forkFreqMapTrue.end())
     forkFreqMapTrue[(long)this] = forkFreqMapTrue[(long)&state];
  if (forkFreqMapFalse.find((long)&state) != forkFreqMapFalse.end())
     forkFreqMapFalse[(long)this] = forkFreqMapFalse[(long)&state];
  reachedFunctions = state.reachedFunctions;
  timingObservationPoint = state.timingObservationPoint;
  //llvm::errs() << "cloning state " << &state << " to " << this << "\n";
  /* SYSREL */ 
}

ExecutionState *ExecutionState::branch() {
  depth++;

  ExecutionState *falseState = new ExecutionState(*this);
  falseState->coveredNew = false;
  falseState->coveredLines.clear();

  weight *= .5;
  falseState->weight -= weight;

  /* SYSREL extension */
  // side channel
  //cloneHighAddresses(*this, *falseState);
  //cloneLowAddresses(*this, *falseState);
  cloneHighMemoryRegions(*this, *falseState);
  cloneLowMemoryRegions(*this, *falseState);
  /* SYSREL extension */

  return falseState;
}

void ExecutionState::pushFrame(KInstIterator caller, KFunction *kf) {
  stack.push_back(StackFrame(caller,kf));
}

/* SYSREL extension */
void ExecutionState::pushFrameThread(KInstIterator caller, KFunction *kf, int tid) {
  threads[tid].stack.push_back(StackFrame(caller,kf));
}
/* SYSREL extension */

void ExecutionState::popFrame() {
  StackFrame &sf = stack.back();
  for (std::vector<const MemoryObject*>::iterator it = sf.allocas.begin(), 
         ie = sf.allocas.end(); it != ie; ++it)
    addressSpace.unbindObject(*it);
  stack.pop_back();
}

/* SYSREL extension */
void ExecutionState::popFrameThread(int tid) {
  StackFrame &sf = threads[tid].stack.back();
  for (std::vector<const MemoryObject*>::iterator it = sf.allocas.begin(), 
         ie = sf.allocas.end(); it != ie; ++it)
    addressSpace.unbindObject(*it);
  threads[tid].stack.pop_back();
}

bool ExecutionState::isSymbolic(const MemoryObject *mo) {
  for(unsigned int i=0; i<symbolics.size(); i++) {
     if (symbolics[i].first == mo)
        return true;
  }
  return false;
}

/* SYSREL extension */

void ExecutionState::addSymbolic(const MemoryObject *mo, const Array *array) { 
  mo->refCount++;
  symbolics.push_back(std::make_pair(mo, array));
}
///

std::string ExecutionState::getFnAlias(std::string fn) {
  std::map < std::string, std::string >::iterator it = fnAliases.find(fn);
  if (it != fnAliases.end())
    return it->second;
  else return "";
}

void ExecutionState::addFnAlias(std::string old_fn, std::string new_fn) {
  fnAliases[old_fn] = new_fn;
}

void ExecutionState::removeFnAlias(std::string fn) {
  fnAliases.erase(fn);
}

/**/

llvm::raw_ostream &klee::operator<<(llvm::raw_ostream &os, const MemoryMap &mm) {
  os << "{";
  MemoryMap::iterator it = mm.begin();
  MemoryMap::iterator ie = mm.end();
  if (it!=ie) {
    os << "MO" << it->first->id << ":" << it->second;
    for (++it; it!=ie; ++it)
      os << ", MO" << it->first->id << ":" << it->second;
  }
  os << "}";
  return os;
}

bool ExecutionState::merge(const ExecutionState &b) {
  if (DebugLogStateMerge)
    llvm::errs() << "-- attempting merge of A:" << this << " with B:" << &b
                 << "--\n";
  if (pc != b.pc)
    return false;

  // XXX is it even possible for these to differ? does it matter? probably
  // implies difference in object states?
  if (symbolics!=b.symbolics)
    return false;

  {
    std::vector<StackFrame>::const_iterator itA = stack.begin();
    std::vector<StackFrame>::const_iterator itB = b.stack.begin();
    while (itA!=stack.end() && itB!=b.stack.end()) {
      // XXX vaargs?
      if (itA->caller!=itB->caller || itA->kf!=itB->kf)
        return false;
      ++itA;
      ++itB;
    }
    if (itA!=stack.end() || itB!=b.stack.end())
      return false;
  }

  std::set< ref<Expr> > aConstraints(constraints.begin(), constraints.end());
  std::set< ref<Expr> > bConstraints(b.constraints.begin(), 
                                     b.constraints.end());
  std::set< ref<Expr> > commonConstraints, aSuffix, bSuffix;
  std::set_intersection(aConstraints.begin(), aConstraints.end(),
                        bConstraints.begin(), bConstraints.end(),
                        std::inserter(commonConstraints, commonConstraints.begin()));
  std::set_difference(aConstraints.begin(), aConstraints.end(),
                      commonConstraints.begin(), commonConstraints.end(),
                      std::inserter(aSuffix, aSuffix.end()));
  std::set_difference(bConstraints.begin(), bConstraints.end(),
                      commonConstraints.begin(), commonConstraints.end(),
                      std::inserter(bSuffix, bSuffix.end()));
  if (DebugLogStateMerge) {
    llvm::errs() << "\tconstraint prefix: [";
    for (std::set<ref<Expr> >::iterator it = commonConstraints.begin(),
                                        ie = commonConstraints.end();
         it != ie; ++it)
      llvm::errs() << *it << ", ";
    llvm::errs() << "]\n";
    llvm::errs() << "\tA suffix: [";
    for (std::set<ref<Expr> >::iterator it = aSuffix.begin(),
                                        ie = aSuffix.end();
         it != ie; ++it)
      llvm::errs() << *it << ", ";
    llvm::errs() << "]\n";
    llvm::errs() << "\tB suffix: [";
    for (std::set<ref<Expr> >::iterator it = bSuffix.begin(),
                                        ie = bSuffix.end();
         it != ie; ++it)
      llvm::errs() << *it << ", ";
    llvm::errs() << "]\n";
  }

  // We cannot merge if addresses would resolve differently in the
  // states. This means:
  // 
  // 1. Any objects created since the branch in either object must
  // have been free'd.
  //
  // 2. We cannot have free'd any pre-existing object in one state
  // and not the other

  if (DebugLogStateMerge) {
    llvm::errs() << "\tchecking object states\n";
    llvm::errs() << "A: " << addressSpace.objects << "\n";
    llvm::errs() << "B: " << b.addressSpace.objects << "\n";
  }
    
  std::set<const MemoryObject*> mutated;
  MemoryMap::iterator ai = addressSpace.objects.begin();
  MemoryMap::iterator bi = b.addressSpace.objects.begin();
  MemoryMap::iterator ae = addressSpace.objects.end();
  MemoryMap::iterator be = b.addressSpace.objects.end();
  for (; ai!=ae && bi!=be; ++ai, ++bi) {
    if (ai->first != bi->first) {
      if (DebugLogStateMerge) {
        if (ai->first < bi->first) {
          llvm::errs() << "\t\tB misses binding for: " << ai->first->id << "\n";
        } else {
          llvm::errs() << "\t\tA misses binding for: " << bi->first->id << "\n";
        }
      }
      return false;
    }
    if (ai->second != bi->second) {
      if (DebugLogStateMerge)
        llvm::errs() << "\t\tmutated: " << ai->first->id << "\n";
      mutated.insert(ai->first);
    }
  }
  if (ai!=ae || bi!=be) {
    if (DebugLogStateMerge)
      llvm::errs() << "\t\tmappings differ\n";
    return false;
  }
  
  // merge stack

  ref<Expr> inA = ConstantExpr::alloc(1, Expr::Bool);
  ref<Expr> inB = ConstantExpr::alloc(1, Expr::Bool);
  for (std::set< ref<Expr> >::iterator it = aSuffix.begin(), 
         ie = aSuffix.end(); it != ie; ++it)
    inA = AndExpr::create(inA, *it);
  for (std::set< ref<Expr> >::iterator it = bSuffix.begin(), 
         ie = bSuffix.end(); it != ie; ++it)
    inB = AndExpr::create(inB, *it);

  // XXX should we have a preference as to which predicate to use?
  // it seems like it can make a difference, even though logically
  // they must contradict each other and so inA => !inB

  std::vector<StackFrame>::iterator itA = stack.begin();
  std::vector<StackFrame>::const_iterator itB = b.stack.begin();
  for (; itA!=stack.end(); ++itA, ++itB) {
    StackFrame &af = *itA;
    const StackFrame &bf = *itB;
    for (unsigned i=0; i<af.kf->numRegisters; i++) {
      ref<Expr> &av = af.locals[i].value;
      const ref<Expr> &bv = bf.locals[i].value;
      if (av.isNull() || bv.isNull()) {
        // if one is null then by implication (we are at same pc)
        // we cannot reuse this local, so just ignore
      } else {
        av = SelectExpr::create(inA, av, bv);
      }
    }
  }

  for (std::set<const MemoryObject*>::iterator it = mutated.begin(), 
         ie = mutated.end(); it != ie; ++it) {
    const MemoryObject *mo = *it;
    const ObjectState *os = addressSpace.findObject(mo);
    const ObjectState *otherOS = b.addressSpace.findObject(mo);
    assert(os && !os->readOnly && 
           "objects mutated but not writable in merging state");
    assert(otherOS);

    ObjectState *wos = addressSpace.getWriteable(mo, os);
    for (unsigned i=0; i<mo->size; i++) {
      ref<Expr> av = wos->read8(i);
      ref<Expr> bv = otherOS->read8(i);
      wos->write(i, SelectExpr::create(inA, av, bv));
    }
  }

  constraints = ConstraintManager();
  for (std::set< ref<Expr> >::iterator it = commonConstraints.begin(), 
         ie = commonConstraints.end(); it != ie; ++it)
    constraints.addConstraint(*it);
  constraints.addConstraint(OrExpr::create(inA, inB));

  return true;
}

void ExecutionState::dumpStack(llvm::raw_ostream &out) const {
  unsigned idx = 0;
  const KInstruction *target = prevPC;
  for (ExecutionState::stack_ty::const_reverse_iterator
         it = stack.rbegin(), ie = stack.rend();
       it != ie; ++it) {
    const StackFrame &sf = *it;
    Function *f = sf.kf->function;
    const InstructionInfo &ii = *target->info;
    out << "\t#" << idx++;
    std::stringstream AssStream;
    AssStream << std::setw(8) << std::setfill('0') << ii.assemblyLine;
    out << AssStream.str();
    out << " in " << f->getName().str() << " (";
    // Yawn, we could go up and print varargs if we wanted to.
    unsigned index = 0;
    for (Function::arg_iterator ai = f->arg_begin(), ae = f->arg_end();
         ai != ae; ++ai) {
      if (ai!=f->arg_begin()) out << ", ";

      out << ai->getName().str();
      // XXX should go through function
      ref<Expr> value = sf.locals[sf.kf->getArgRegister(index++)].value;
      if (value.get() && isa<ConstantExpr>(value))
        out << "=" << value;
    }
    out << ")";
    if (ii.file != "")
      out << " at " << ii.file << ":" << ii.line;
    out << "\n";
    target = sf.caller;
  }

  /* SYSREL extension */
  for(auto refrec : refCountStack) {
     out << "refcount history of " << refrec.first << "\n";
     for(int i=0; i<refrec.second.size(); i++)
        out << refrec.second[i] << "\n";
  }
  /* SYSREL extension */
}


/* SYSREL */

void ExecutionState::dumpStackThread(llvm::raw_ostream &out) const {
  unsigned idx = 0;
  const KInstruction *target = prevPC;
  for (ExecutionState::stack_ty::const_reverse_iterator
         it = stack.rbegin(), ie = stack.rend();
       it != ie; ++it) {
    const StackFrame &sf = *it;
    Function *f = sf.kf->function;
    const InstructionInfo &ii = *target->info;
    out << "\t#" << idx++;
    std::stringstream AssStream;
    AssStream << std::setw(8) << std::setfill('0') << ii.assemblyLine;
    out << AssStream.str();
    out << " in " << f->getName().str() << " (";
    // Yawn, we could go up and print varargs if we wanted to.
    unsigned index = 0;
    for (Function::arg_iterator ai = f->arg_begin(), ae = f->arg_end();
         ai != ae; ++ai) {
      if (ai!=f->arg_begin()) out << ", ";

      out << ai->getName().str();
      // XXX should go through function
      ref<Expr> value = sf.locals[sf.kf->getArgRegister(index++)].value;
      if (value.get() && isa<ConstantExpr>(value))
        out << "=" << value;
    }
    out << ")";
    if (ii.file != "")
      out << " at " << ii.file << ":" << ii.line;
    out << "\n";
    target = sf.caller;
  }
  for(unsigned int tid=0; tid<threads.size(); tid++) {
   if (!threads[tid].terminated) {
   out<< "Stack for thread " << tid << "\n";
  idx = 0;
  target = threads[tid].prevPC;
  for (ExecutionState::stack_ty::const_reverse_iterator
         it = threads[tid].stack.rbegin(), ie = threads[tid].stack.rend();
       it != ie; ++it) {
    const StackFrame &sf = *it;
    Function *f = sf.kf->function;
    const InstructionInfo &ii = *target->info;
    out << "\t#" << idx++;
    std::stringstream AssStream;
    AssStream << std::setw(8) << std::setfill('0') << ii.assemblyLine;
    out << AssStream.str();
    out << " in " << f->getName().str() << " (";
    // Yawn, we could go up and print varargs if we wanted to.
    unsigned index = 0;
    for (Function::arg_iterator ai = f->arg_begin(), ae = f->arg_end();
         ai != ae; ++ai) {
      if (ai!=f->arg_begin()) out << ", ";

      out << ai->getName().str();
      // XXX should go through function
      ref<Expr> value = sf.locals[sf.kf->getArgRegister(index++)].value;
      if (value.get() && isa<ConstantExpr>(value))
        out << "=" << value;
    }
    out << ")";
    if (ii.file != "")
      out << " at " << ii.file << ":" << ii.line;
    out << "\n";
    target = sf.caller;
  }
 }
 }
}


void ExecutionState::printPC() {
  llvm::outs() << "rtid=" << rtid << "\n";
  if (rtid < 0)
     pc->inst->dump();
  else 
     threads[rtid].pc->inst->dump();
}

void ExecutionState::terminateThread(int tid) {
  threads[tid].terminated = true;
  threads[tid].preemptable = true;
  std::queue<int> temp;
  while (!threadsQueue.empty()) {  
    int next = threadsQueue.front(); 
    threadsQueue.pop();
    if (next != tid)
       temp.push(next);  
  }
  threadsQueue = temp;   
}


bool ExecutionState::activeThreads() {
   for(unsigned int i=0; i<threads.size(); i++)
      if (!threads[i].terminated)
         return true;
   return false;
}

bool ExecutionState::threadTerminated(int tid) {
   return threads[tid].terminated;
}

void ExecutionState::setWaitingForThreadsToTerminate(bool value) {
  waitingForThreadsToTerminate = value;
  if (value) {
  // remove the main thread from the scheduler's queue
  llvm::outs() << "queue updated:\n";
  std::queue<int> temp;
  while (!threadsQueue.empty()) {  
    int next = threadsQueue.front(); 
    threadsQueue.pop();
    if (next != -1) {
       temp.push(next);
       llvm::outs() << next << " ";
    }  
  }
  llvm::outs() << "\n";       
  threadsQueue = temp;
 }
}

bool ExecutionState::getWaitingForThreadsToTerminate() {
  return waitingForThreadsToTerminate;
}

int ExecutionState::getID() {
  return id;
}

void ExecutionState::setLifeCycleModel(Sequential *lcm) {
  LifeCycleModelState::setLifeCycleModel(lcm);
}

bool ExecutionState::hasLCM() {
  return (LifeCycleModelState::lcm != NULL);
}

bool ExecutionState::lcmCompleted() {
  if (hasLCM()) {
    return lcmState->hasCompleted();     
  }
  else return true;
}

int ExecutionState::getLCMState() {
  if (hasLCM()) 
    return lcmState->getCurrentStep();
  return -1;
}

bool ExecutionState::lcmCompletesWith(std::string fn) {
  if (hasLCM()) {
     return lcmState->completesWith(fn);
  }
  else return true;
}

int ExecutionState::getCurrentSuccessReturnValue() {
  if (hasLCM()) 
    return lcmState->getCurrentSuccessValue();
  return 0;
}

// set up the state according to the next step in the life cycle model
// if move == true then fn is the one that has just returned and 
// completing the current step
// if move == false then fn is the entry point and the current step 
// should be prepared to initialize other components, if any, in the current step
void ExecutionState::updateLCMState() {
  if (lcmState && !lcmState->hasCompleted()) {
     int curr = lcmState->getCurrentStep();
     Sequential *seq = NULL;
     if (curr != 0 || lcmState->isInitialized()) {
        curr++;
        lcmState->moveStep();
     }
     seq  = lcmState->lcm->getStep(curr);            
     if (seq->getType() == parallel) {
           std::vector<std::string> comps = ((Parallel*)seq)->getComponents();
           // Place one as the main thread
           unsigned int entryIndex = comps.size();
           if (curr == 0) {
              // make sure entry function is one of them
             for(unsigned int i=0; i<comps.size(); i++) {
               if (comps[i] == entryFunctionName) {
                  entryIndex = i;
                  break;
               }              
             }
           }
           if (curr == 0 && entryIndex == comps.size()) {
                llvm::outs() << "Invalid life-cycle model: entry function is not part of the first step!\n";
                assert(0);
           }
           else {
             for(unsigned int i=0; i<comps.size(); i++) {
                // create a separate thread for each component
               if (curr != 0 || i != entryIndex) {  
                  Function *func = moduleHandle->getFunction(comps[i]);
                  initiateAsync(func);
               }
             }   
           }
     }
     else if (seq->getType() == identifier) {
       if (curr != 0 ) {
        Identifier *id = dynamic_cast<Identifier*>(seq);
        assert(id);
        llvm::errs() << " next step " << id->getValue() << " to execute " << "\n";
        Function *func = moduleHandle->getFunction(id->getValue());
        //initiateAsync(func); 
        llvm::errs() << "extending current state " << this << " with " << func->getName() << " to simulate sequential composition\n";
        extendExecutionWith(kmoduleExt->functionMap[func]);
       }
     }
     else assert(0); 
     lcmState->setInitialized();
  }
  #ifdef VB
  llvm::outs() << "LCM state after update: " << lcmState->getCurrentStep() << "\n";
  #endif
}

bool ExecutionState::lcmStepMovesWhenReturns(std::string fname) {
  return lcmState->stepMovesWhenReturns(fname);
}



Identifier::Identifier(std::string s) {
  name = s;
  type = identifier;
  successretval = 0;
}

void Identifier::setSuccessReturnValue(int value) {
  successretval = value;
}

int Identifier::getSuccessReturnValue() {
  return successretval;
}

void Identifier::print() {
  llvm::outs() << name << " [" << successretval << "]";
}

std::string Identifier::getValue() { return name; }

Parallel::Parallel(std::vector<std::string> p) {
  par = p;
  type = parallel;
}

int Parallel::getNumInstance(std::string s) {
  int found =0;
  for(unsigned int i=0; i<par.size(); i++)
    if (par[i] == s)
      found++;
  return found;
}

bool Parallel::member(std::string s) {
   for(unsigned int i=0; i<par.size(); i++)
    if (par[i] == s)
      return true;
   return false;
}

std::vector<std::string> Parallel::getComponents() {
  return par;
}

void Parallel::print() {
  llvm::outs() << "( ";
  for(unsigned int i=0; i<par.size(); i++) {
    llvm::outs() << par[i] << " ";
    if (i != par.size() - 1)
      llvm::outs() << "| ";  
  } 
  llvm::outs() << ")";
}


Sequential::Sequential() {
  finalized = false;
  type = sequential;
}

unsigned int Sequential::getNumSteps() {
  return sequence.size();
}

lcmType Sequential::getType() {
  return  type;
}

void Sequential::addStep(Sequential *seq) {
  if (!finalized)
    sequence.push_back(seq);
  else llvm::errs() << "Cannot add step to sequence after finalization!\n";
}

void Sequential::printStep(unsigned int s) {
  if (s < sequence.size())
    sequence[s]->print();
}

void Sequential::print() {
  for(unsigned int i=0; i<sequence.size(); i++) {
    sequence[i]->print();
    if (i != sequence.size() - 1)
       llvm::outs() << "; ";
  }
}

void Sequential::finalize() {
  finalized = true;
}

bool Sequential::isFinalized() {
  return finalized;
}

Sequential *Sequential::getStep(unsigned int i) {
  if (i < sequence.size())
     return sequence[i];
  else return NULL;
}


void LifeCycleModelState::setLifeCycleModel(Sequential *lcmC) {
  lcm = lcmC;
}

Sequential *LifeCycleModelState::getLifeCycleModel() {
  return lcm;
}

LifeCycleModelState::LifeCycleModelState() {
  state = 0;
  initialized = false;
  completed = false;
}

LifeCycleModelState::LifeCycleModelState(LifeCycleModelState &other) {
  state = other.state;
  initialized = other.initialized;
  completed = other.completed;
  componentsCompleted = other.componentsCompleted;
}


bool LifeCycleModelState::moveStep() {
  state++;
  componentsCompleted.clear();
  return (state < lcm->getNumSteps());
}

int LifeCycleModelState::getCurrentStep() {
  return state;
}

int LifeCycleModelState::getCurrentSuccessValue() {
  Sequential *seq = lcm->getStep(state);
  if (seq->getType() == identifier) {
     return ((Identifier*)seq)->getSuccessReturnValue();
  }
  assert(false && "Not sure what to return for concurrent\n!");   
}

bool LifeCycleModelState::hasCompleted() {
  return (state >= lcm->getNumSteps());
}

bool LifeCycleModelState::stepMovesWhenReturns(std::string fname) {
 #ifdef VB
  llvm::outs() << "step moves when " << fname << " returns?\n";
 #endif
  Sequential *seq = lcm->getStep(state);
  if (seq->getType() == identifier) {
    #ifdef VB
    llvm::outs() << "current state: " << state << "\n";
    llvm::outs() << "current step: " << ((Identifier*)seq)->getValue() << "\n"; 
    #endif
    return ((Identifier*)seq)->getValue() == fname;
  }
  assert(seq->getType() == parallel);
  std::vector<std::string> comp = ((Parallel*)seq)->getComponents();
  int count = 0;
  for(unsigned int i=0; i<comp.size(); i++) {
    if (comp[i] == fname)
      count++; 
  }
  if (count == 0) return false;
  int countComp = 0;
  for(unsigned int i=0; i<componentsCompleted.size(); i++)
    if (componentsCompleted[i] == fname)
       countComp++;
  return (componentsCompleted.size() == comp.size() - 1) && (countComp == count - 1);
}

bool LifeCycleModelState::isInitialized() {
  return initialized;
}

void LifeCycleModelState::setInitialized() {
  initialized = true;
}

bool LifeCycleModelState::completesWith(std::string fn) {
  #ifdef VB
   llvm::outs() << "checking completes with for " << fn << " state " << state  << "numsteps " << lcm->getNumSteps() << "\n";
  #endif
   return (state == lcm->getNumSteps() - 1 && stepMovesWhenReturns(fn));
}

bool LifeCycleModelState::isLastStep(std::string fn) {
  Sequential *seq = lcm->getStep(lcm->getNumSteps() - 1);
  assert(seq->getType() == identifier);
  #ifdef VB
    llvm::outs() << "current state: " << state << "\n";
    llvm::outs() << "current step: " << ((Identifier*)seq)->getValue() << "\n"; 
  #endif
  return ((Identifier*)seq)->getValue() == fn;
}

AsyncInfo::AsyncInfo(Function *f) {
    function = f;
    count = 1;
    numstarted = 0;  
}

Async::Async(ExecutionState *state, Function *f, int tid,  MemoryManager *memory) {
  function = f;
  KFunction *kf = kmoduleExt->functionMap[f];
  kfunction = kf;  
  pc = kf->instructions;
  prevPC = pc;
  incomingBBIndex = -1;
  preemptable = false; 
  this->tid = tid;
  context = state;
  stack.push_back(StackFrame(0,kf));
  terminated = false;

  /* SYSREL extension */
  if (lazyInit) {
   // Lazy init args of the entryFunc
   unsigned int ind = 0;
   for(llvm::Function::arg_iterator ai = f->arg_begin(); ai != f->arg_end(); ai++) {
     Type *at = ai->getType();
     std::string type_str;
     llvm::raw_string_ostream rso(type_str);
     at->print(rso);
     if (at->isPointerTy()) {
        llvm::outs() << "arg " << ind << " type " << rso.str() << "\n";
        at = at->getPointerElementType();
        bool singleInstance = false;
        int count = 0;
        bool lazyInitT = isAllocTypeLazyInit(at, singleInstance, count);
        if (lazyInitT) {
           ref<Expr> laddr;
           llvm::Type *rType;
           bool mksym;
           bool abort = false;
           const MemoryObject *mo = memory->allocateLazyForTypeOrEmbedding(*state, prevPC->inst, at, at, singleInstance, count, rType, laddr, mksym, abort);
           assert(!abort);
           //MemoryObject *mo = memory->allocateForLazyInit(*state, prevPC->inst, at, singleInstance, count, laddr);
           mo->name = state->getUnique(f->getName()) + std::string("arg_") + std::to_string(ind);
           Executor *exe = (Executor*)theInterpreter;
           if (mksym)
              exe->executeMakeSymbolicThread(*state, mo, std::string("arg_") + std::to_string(ind), tid); 
           exe->bindArgumentThread(kf, ind, *state, laddr, tid);
           llvm::outs() << "binding arg " << ind << " of type " << rso.str() << " to address " << laddr << " (in " << mo->getBaseExpr() << ")\n";
        }
     }    
     ind++; 
   }
  }
  /* SYSREL extension */


}


// the right context (ExecutionState) must be set separately!
// An example is creating a new ExecutionState through branching
// The same goes for the pc of the thread that is branching
Async::Async(const Async& a) {
  function = a.function;
  kfunction = a.kfunction;  
  pc = a.pc;
  prevPC = a.prevPC;
  incomingBBIndex = a.incomingBBIndex;
  preemptable = a.preemptable; 
  this->tid = a.tid;
  context = a.context;
  stack = a.stack;
  terminated = a.terminated;
}

int ExecutionState::initiateAsync(Function *f) { 

  std::string fn = f->getName();
  std::map<std::string, AsyncInfo>::iterator it;
  if ((it = initiatedAsync.find(fn)) != initiatedAsync.end()) {
    // initiated before
    AsyncInfo &aif =  it->second;
    aif.count++;
    return aif.count;
  }
  else {
    AsyncInfo aif(f); 
    initiatedAsync.insert(std::pair<std::string, AsyncInfo>(fn, aif));
    return aif.count;
  }

}

// return the id of the next thread (main or one of the auxillary threads)
int ExecutionState::scheduleAsync(MemoryManager *memory) {
  if (!waitingForThreadsToTerminate && rtid == -1 && !preemptable) /* main thread currently running and not preemptable */ {
    llvm::outs() << "scheduling main again\n";
    return -1;
  }
  if (rtid >= 0 && !threads[rtid].preemptable) /* the current thread not preemptable */ {
    llvm::outs() << "scheduling thread " << rtid << " again\n";
    return rtid;
 }
  // Any async to start?
  for(std::map<std::string, AsyncInfo>::iterator it =  initiatedAsync.begin(); 
      it !=  initiatedAsync.end(); it++) {
    if (it->second.numstarted < it->second.count) {
         llvm::outs() << "Starting a new instance for async " << it->second.function->getName() << "\n";
         Async a(this, it->second.function, threads.size(), memory);
         threads.push_back(a); 
         if (threadsQueue.empty())
            threadsQueue.push(-1);
         threadsQueue.push(a.tid);
         Executor *exe = (Executor*)theInterpreter;
         exe->getStatsTracker()->framePushedThread(*this, 0, a.tid);
         it->second.numstarted++;
         rtid = a.tid;
         return a.tid;
     }
  }
  // choose one from the queue !
  if (threadsQueue.empty()) {
     llvm::outs() << "thread queue empty!\n";
     return -1;  // so that main can go ahead and terminate!
  }
  int next = threadsQueue.front(); 
  threadsQueue.pop();
  if (!waitingForThreadsToTerminate || next >= 0)
     threadsQueue.push(next);  
  if (next == -1) {
     llvm::outs() << "scheduling main\n";
     assert(!waitingForThreadsToTerminate);
     preemptable = false;
     rtid = -1;
  }
  else if (next >= 0) {
     llvm::outs() << "scheduling thread " << next << "\n";
     threads[next].preemptable = false;
     rtid = next;
  }
  return next; 
}

int ExecutionState::setPreemptable(int tid, bool value) {
  if (tid == -1) {
     preemptable = value;
     return 0;
  }  
  else if (tid >= 0 && (unsigned int)tid < threads.size()) {
    threads[tid].preemptable = value;
    return 0;
  }
  else return -1;
} 

void ExecutionState::setRefCount(ref<Expr> addr,int value) {
   refCountModel[addr] = value; 
}

int ExecutionState::getRefCount(ref<Expr> addr) {
   if (refCountModel.find(addr) == refCountModel.end())
      refCountModel[addr] = 0;
   return refCountModel[addr];
}

void ExecutionState::addSymbolDef(std::string sym, ref<Expr> value) {
  symbolDefs[sym] = value;
}
 
ref<Expr> ExecutionState::getSymbolDef(std::string sym) {
  if (symbolDefs.find(sym) != symbolDefs.end())
     return symbolDefs[sym];
  return NULL;
}

void ExecutionState::addSymbolType(std::string sym, llvm::Type *t) {
  symbolTypes[sym] = t;
}
 
llvm::Type *ExecutionState::getSymbolType(std::string sym) {
  if (symbolTypes.find(sym) != symbolTypes.end())
     return symbolTypes[sym];
  return NULL;
}

void ExecutionState::recordAlloc(ref<Expr> address) {
  #ifdef VB
  llvm::errs() << "Recording alloc " << address << "\n";
  #endif
  alloced.insert(address);
}

void ExecutionState::recordFree(ref<Expr> address) {
  #ifdef VB
  llvm::errs() << "Recording free " << address << "\n";
  #endif
  freed.insert(address);
}

bool ExecutionState::isFreed(ref<Expr> address) {
  return (freed.find(address) != freed.end());
}

void ExecutionState::check() {
   for(auto rfm : refCountModel) {
      if (rfm.second > 0)
         llvm::errs() << "Positive refcount for " << rfm.first << ":" << rfm.second << "\n";
      else if (rfm.second < 0)
         llvm::errs() << "Negative refcount for " << rfm.first << ":" << rfm.second << "\n";
      else 
         llvm::errs() << "Zero refcount for " << rfm.first << ":" << rfm.second << "\n";
   }
 
   llvm::errs() << "Memory leaks:\n";
   for(auto al : alloced) {
      if (freed.find(al) == freed.end())
         llvm::errs() << al << "\n";
   } 
   llvm::errs() << "Memory leaks end:\n";
  
}

void ExecutionState::recordRefcountOp(ref<Expr> addr, std::string record) {
  std::vector<std::string> stack;
  if (refCountStack.find(addr) != refCountStack.end())
     stack = refCountStack[addr];
  stack.push_back(record);
  refCountStack[addr] = stack;
}

std::string ExecutionState::getUnique(std::string n) {
  if (symIdCounters.find(n) == symIdCounters.end()) {
     symIdCounters[n] = 0;
     return n + "_0";
  }
  long int v = symIdCounters[n];
  symIdCounters[n] = ++v;
  return n + "_" + std::to_string(symIdCounters[n]);    
}


bool ExecutionState::isSecretDescendant() {
  return secretDescendant;
}

void ExecutionState::setSecretDescendant(bool val) {
  secretDescendant = val;  
}

PMFrame::PMFrame() {
   action = NULL;
   currentAction = -1;
   target = NULL;
   this->tid = -1;  
   callback = "";
   args.clear();
}

PMFrame::PMFrame(APIAction *a, std::vector< ref<Expr> > &arguments, 
          KInstruction *target, int tid) {
  action = a;
  currentAction = 0;
  this->target = target; 
  for(auto a: arguments)
    args.push_back(a);
  this->tid = tid; 
  callback = "";
}

PMFrame::PMFrame(const PMFrame &pf) {
  action = pf.action;
  currentAction = pf.currentAction;
  target = pf.target; 
  for(auto a: pf.args)
    args.push_back(a);
  tid = pf.tid; 
  callback = pf.callback;
}

void PMFrame::execute(ExecutionState &state, bool &term, bool &comp, bool &abort) {
  #ifdef VB
  llvm::errs() << "PMFrame's action:\n "; action->print();
  #endif
  action->execute(*this, state, args, target, term, comp, abort, tid);
  
}

void PMFrame::setCallback(std::string cb) {
  callback = cb;
}

void PMFrame::setPMAction(int cn) {
  currentAction = cn;
}

void ExecutionState::pushPMFrame(APIAction *a, std::vector< ref<Expr> > arguments, 
          KInstruction *target, int tid) {
   PMFrame *pf = new PMFrame(a, arguments, target, tid);
   #ifdef VB
   llvm::errs() << "pushing to PM stack of size=" << pmstack.size() << "\n";
   a->print();
   for(int i=0; i<arguments.size(); i++)
      llvm::errs() << "arg " << i << "=" << arguments[i] << "\n";
  #endif
   pmstack.push_back(pf);
}

void ExecutionState::popPMFrame() {
   int i;
   if ((i=pmstack.size()) > 0) {
      pmstack.pop_back();
   }
}

// post: either the stack is emptied or the top frame is waiting for a callback to finish
bool ExecutionState::isPMStackEmpty() {
   return (pmstack.size() == 0);
}

void ExecutionState::executePM(bool &abort) {
  if (isPMStackEmpty())
     return;
  int top = pmstack.size() - 1;
  if (pmstack[top]->callback != "") {
     #ifdef VB
     llvm::errs() << "skipping rest of the APIAction to wait for the callback to finish!\n";
    #endif
     return;
  } 
  #ifdef VB
  llvm::errs() << "Executing PMFrame, callback=" << pmstack[top]->callback << "\n";
  #endif
  bool term;
  bool comp;
  pmstack[top]->execute(*this, term, comp, abort);
  if (abort) return;
  if (term || comp) {
     #ifdef VB
     llvm::errs() << "APIAction completed\n"; pmstack[top]->action->print();
     #endif
     popPMFrame(); 
  }   
  // in case an apisubblock frame is on the stack
  executePM(abort);
}

void ExecutionState::setPMCallback(std::string cbn) {
  if (isPMStackEmpty())
     assert(false);
  int top = pmstack.size() - 1;
  if (cbn != "" && pmstack[top]->callback != "") {
     llvm::errs() << "callback is already set to " << pmstack[top]->callback << "\n";
     assert(false);
  }
  pmstack[top]->setCallback(cbn);
}

std::string ExecutionState::getPMCallback() {
  if (isPMStackEmpty())
     return "";
  int top = pmstack.size() - 1;
  return pmstack[top]->callback;  
}

void ExecutionState::checkAndSetPMCallbackCompleted(std::string cbn) {
   if (getPMCallback() == cbn) {
      #ifdef VB
      llvm::errs() << "API callback " << cbn << " completed!\n";
     #endif
      setPMCallback("");
      // Has the APIAction completed?
      if (getPMAction() >= getPMNumActions())
         popPMFrame(); 
   }
}

int ExecutionState::getPMAction() {
    if (isPMStackEmpty())
       return -1;
    int top = pmstack.size() - 1;
    return pmstack[top]->currentAction;
}

int ExecutionState::getPMNumActions() {
    if (isPMStackEmpty())
       return 0;
    int top = pmstack.size() - 1;
    return pmstack[top]->action->getNumActions();
}


ref<Expr> ExecutionState::addSymbolicReturnAsPublicOutput(std::string entry, std::string name, MemoryManager *memory, ArrayCache &arrayCache, MemoryObject* &mo, bool &valid) {
   Function *f = moduleHandle->getFunction(entry);
   Type *t = f->getReturnType();
   valid = !t->isVoidTy();
   if (!valid) return NULL;
   const llvm::DataLayout & dl = f->getParent()->getDataLayout();
   size_t allocationAlignment = 8;
   mo =  memory->allocate(dl.getTypeAllocSize(t), false, /*true*/false, NULL, allocationAlignment);
   recordMemObj(*this, mo);
   mo->name = name;
   const Array *array = arrayCache.CreateArray(mo->name, mo->size);
   ObjectState *sos = ((Executor*)theInterpreter)->bindObjectInState(*this, mo, true, array);
   ref<Expr> result = sos->read(ConstantExpr::alloc(0, Expr::Int64), ((Executor*)theInterpreter)->getWidthForLLVMType(t));
   return result; 
}

void ExecutionState::setLastEnclaveFunction(std::string last) {
  lastEnclaveFunction = last;
}

std::string ExecutionState::getLastEnclaveFunction() {
   return lastEnclaveFunction;
}

/* SYSREL */

