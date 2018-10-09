//===-- ExecutionState.h ----------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_EXECUTIONSTATE_H
#define KLEE_EXECUTIONSTATE_H
/* SYSREL extension */
#include "llvm/IR/Function.h"
#include <queue>

/* SYSREL extension */
#include "klee/Constraints.h"
#include "klee/Expr.h"
#include "klee/Internal/ADT/TreeStream.h"
#include "klee/MergeHandler.h"

// FIXME: We do not want to be exposing these? :(
#include "../../lib/Core/AddressSpace.h"
#include "klee/Internal/Module/KInstIterator.h"

#include <map>
#include <set>
#include <vector>

namespace klee {
class MemoryManager;
class Array;
class CallPathNode;
struct Cell;
struct KFunction;
struct KInstruction;
class MemoryObject;
class PTreeNode;
struct InstructionInfo;

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const MemoryMap &mm);

struct StackFrame {
  KInstIterator caller;
  KFunction *kf;
  CallPathNode *callPathNode;

  std::vector<const MemoryObject *> allocas;
  Cell *locals;

  /// Minimum distance to an uncovered instruction once the function
  /// returns. This is not a good place for this but is used to
  /// quickly compute the context sensitive minimum distance to an
  /// uncovered instruction. This value is updated by the StatsTracker
  /// periodically.
  unsigned minDistToUncoveredOnReturn;

  // For vararg functions: arguments not passed via parameter are
  // stored (packed tightly) in a local (alloca) memory object. This
  // is set up to match the way the front-end generates vaarg code (it
  // does not pass vaarg through as expected). VACopy is lowered inside
  // of intrinsic lowering.
  MemoryObject *varargs;

  StackFrame(KInstIterator caller, KFunction *kf);
  StackFrame(const StackFrame &s);
  ~StackFrame();
};

/* SYSREL extension begin */




class Async {
public:
  Async(ExecutionState *state, llvm::Function *f, int tid, MemoryManager *memory);
  Async(const Async&);
  llvm::Function *function;
  KFunction *kfunction;
  std::vector<StackFrame> stack;
  KInstIterator pc;
  KInstIterator prevPC;
  unsigned incomingBBIndex;
  bool preemptable;
  int tid;
  ExecutionState *context;
  bool terminated;
  // programming model specific state
  std::map<ref<Expr>,int> stateModel;  
  std::map<ref<Expr>, ref<Expr> > assocModel; 
  std::map<std::string, int> returnValueModel;
};

class AsyncInfo { 
public:
  AsyncInfo(llvm::Function *);
  llvm::Function *function;
  //unsigned blockNo;
  unsigned count;
  unsigned numstarted;
};

enum lcmType { sequential, identifier, parallel};

class Sequential {
private:
  std::vector<Sequential*> sequence;
  bool finalized;
protected:
  lcmType type; 
public:
  Sequential(); 
  void addStep(Sequential *seq);
  void printStep(unsigned int s);
  virtual void print();
  void finalize();
  bool isFinalized();
  unsigned int getNumSteps();
  Sequential *getStep(unsigned int);
  lcmType getType() ;
};



class Identifier : public Sequential {
private:
  std::string name;
public:
  Identifier(std::string s);
  virtual void print();
  std::string getValue();
};

class Parallel : public Sequential {
private:
  std::vector<std::string> par;
public:
  Parallel(std::vector<std::string> p);
  int getNumInstance(std::string s);
  bool member(std::string s);
  std::vector<std::string> getComponents();
  virtual void print(); 
};



class LifeCycleModelState {
private:
  unsigned int state;
  bool initialized;
  bool completed;
  std::vector<std::string> componentsCompleted;
public: 
  static Sequential *lcm;
  static void setLifeCycleModel(Sequential *lcm);
  static Sequential *getLifeCycleModel();
  LifeCycleModelState();
  LifeCycleModelState(LifeCycleModelState&);
  bool moveStep();
  int getCurrentStep();
  bool hasCompleted();
  bool isInitialized();
  void setInitialized();
  bool stepMovesWhenReturns(std::string);
  bool isCompleted();
  bool completesWith(std::string);
};
/* SYSREL extension end */


/// @brief ExecutionState representing a path under exploration
class ExecutionState {
public:
  typedef std::vector<StackFrame> stack_ty;
  /* SYSREL extension begin */
  std::map<ref<Expr>,int> stateModel;
  std::map<ref<Expr>, ref<Expr> > assocModel; 
  std::map<std::string, long int> returnValueModel;
  std::map<llvm::Type*, ref<Expr> > typeToAddr;
  bool hasLCM();
  int getLCMState();
  bool lcmCompleted();
  void updateLCMState();
  bool lcmStepMovesWhenReturns(std::string);
  static void setLifeCycleModel(Sequential *lcm);
  void setWaitingForThreadsToTerminate(bool);
  bool getWaitingForThreadsToTerminate(); 
  void terminateThread(int tid);
  bool activeThreads();
  bool threadTerminated(int tid);
  void printPC();
  int getID();
  bool lcmCompletesWith(std::string);
  std::map<llvm::Type *, MemoryObject *> lazyInitSingleInstances;
  /* SYSREL extension begin */
private:
  // unsupported, use copy constructor
  ExecutionState &operator=(const ExecutionState &);

  std::map<std::string, std::string> fnAliases;
  /* SYSREL extension begin */
  static int counter;
  int id;
  bool waitingForThreadsToTerminate;
  LifeCycleModelState *lcmState;
  /* SYSREL extension begin */
public:
  /* SYSREL extension begin */
  std::vector<Async> threads;
  // -1 is used for the main thread
  std::queue<int> threadsQueue;
  // running thread id; -1 denotes the main thread
  int rtid; 
  bool preemptable = false; 
  std::map<std::string,AsyncInfo> initiatedAsync;
  
int initiateAsync(llvm::Function *f);
  int scheduleAsync(MemoryManager *memory);
  int setPreemptable(int tid, bool value); 
  /* SYSREL extension end */
  // Execution - Control Flow specific

  /// @brief Pointer to instruction to be executed after the current
  /// instruction
  KInstIterator pc;

  /// @brief Pointer to instruction which is currently executed
  KInstIterator prevPC;

  /// @brief Stack representing the current instruction stream
  stack_ty stack;

  /// @brief Remember from which Basic Block control flow arrived
  /// (i.e. to select the right phi values)
  unsigned incomingBBIndex;

  // Overall state of the state - Data specific

  /// @brief Address space used by this state (e.g. Global and Heap)
  AddressSpace addressSpace;

  /// @brief Constraints collected so far
  ConstraintManager constraints;

  /// Statistics and information

  /// @brief Costs for all queries issued for this state, in seconds
  mutable double queryCost;

  /// @brief Weight assigned for importance of this state.  Can be
  /// used for searchers to decide what paths to explore
  double weight;

  /// @brief Exploration depth, i.e., number of times KLEE branched for this state
  unsigned depth;

  /// @brief History of complete path: represents branches taken to
  /// reach/create this state (both concrete and symbolic)
  TreeOStream pathOS;

  /// @brief History of symbolic path: represents symbolic branches
  /// taken to reach/create this state
  TreeOStream symPathOS;

  /// @brief Counts how many instructions were executed since the last new
  /// instruction was covered.
  unsigned instsSinceCovNew;

  /// @brief Whether a new instruction was covered in this state
  bool coveredNew;

  /// @brief Disables forking for this state. Set by user code
  bool forkDisabled;

  /// @brief Set containing which lines in which files are covered by this state
  std::map<const std::string *, std::set<unsigned> > coveredLines;

  /// @brief Pointer to the process tree of the current state
  PTreeNode *ptreeNode;

  /// @brief Ordered list of symbolics: used to generate test cases.
  //
  // FIXME: Move to a shared list structure (not critical).
  std::vector<std::pair<const MemoryObject *, const Array *> > symbolics;

  /// @brief Set of used array names for this state.  Used to avoid collisions.
  std::set<std::string> arrayNames;

  std::string getFnAlias(std::string fn);
  void addFnAlias(std::string old_fn, std::string new_fn);
  void removeFnAlias(std::string fn);

  // The objects handling the klee_open_merge calls this state ran through
  std::vector<ref<MergeHandler> > openMergeStack;

  // The numbers of times this state has run through Executor::stepInstruction
  std::uint64_t steppedInstructions;

private:
  ExecutionState() : ptreeNode(0) {}

public:
  ExecutionState(KFunction *kf);

  // XXX total hack, just used to make a state so solver can
  // use on structure
  ExecutionState(const std::vector<ref<Expr> > &assumptions);

  ExecutionState(const ExecutionState &state);

  /* SYSREL EXTENSION */
  // continue a path after termination with a new function executed sequentially
  void extendExecutionWith(KFunction *kf);
  /* SYSREL EXTENSION */
  

  ~ExecutionState();

  ExecutionState *branch();

  void pushFrame(KInstIterator caller, KFunction *kf);
  /* SYSREL extension */
  void pushFrameThread(KInstIterator caller, KFunction *kf, int tid);
  /* SYSREL extension */
  void popFrame();
  /* SYSREL extension */
  void popFrameThread(int tid);
  /* SYSREL extension */

  void addSymbolic(const MemoryObject *mo, const Array *array);
  void addConstraint(ref<Expr> e) { constraints.addConstraint(e); }

  bool merge(const ExecutionState &b);
  void dumpStack(llvm::raw_ostream &out) const;
  /* SYSREL extension */
  void dumpStackThread(llvm::raw_ostream &out) const;
  /* SYSREL extension */
};
}

#endif
