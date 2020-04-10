//===-- Executor.cpp ------------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Executor.h"
#include "Context.h"
#include "CoreStats.h"
#include "ExternalDispatcher.h"
#include "ImpliedValue.h"
#include "Memory.h"
#include "MemoryManager.h"
#include "PTree.h"
#include "Searcher.h"
#include "SeedInfo.h"
#include "SpecialFunctionHandler.h"
#include "StatsTracker.h"
#include "TimingSolver.h"
#include "UserSearcher.h"
#include "ExecutorTimerInfo.h"

#include "../Solver/Z3Solver.h"

#include "klee/ExecutionState.h"
#include "klee/Expr.h"
#include "klee/Interpreter.h"
#include "klee/TimerStatIncrementer.h"
#include "klee/CommandLine.h"
#include "klee/Common.h"
#include "klee/util/Assignment.h"
#include "klee/util/ExprPPrinter.h"
#include "klee/util/ExprSMTLIBPrinter.h"
#include "klee/util/ExprUtil.h"
#include "klee/util/GetElementPtrTypeIterator.h"
#include "klee/Config/Version.h"
#include "klee/Internal/ADT/KTest.h"
#include "klee/Internal/ADT/RNG.h"
#include "klee/Internal/Module/Cell.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/KModule.h"
#include "klee/Internal/Support/ErrorHandling.h"
#include "klee/Internal/Support/FloatEvaluation.h"
#include "klee/Internal/Support/ModuleUtil.h"
#include "klee/Internal/System/Time.h"
#include "klee/Internal/System/MemoryUsage.h"
#include "klee/SolverStats.h"
#include "klee/sysrel.h"
#include "klee/SolverImpl.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/TypeFinder.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DerivedTypes.h"

#if LLVM_VERSION_CODE < LLVM_VERSION(3, 5)
#include "llvm/Support/CallSite.h"
#else
#include "llvm/IR/CallSite.h"
#endif

#ifdef HAVE_ZLIB_H
#include "klee/Internal/Support/CompressionStream.h"
#endif

#include <cassert>
#include <algorithm>
#include <iomanip>
#include <iosfwd>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <sys/mman.h>

#include <errno.h>
#include <cxxabi.h>

#include "llvm/IR/InlineAsm.h"
#include "ResourceUsage.h"
#include "AnalyzeConstraint.h"

using namespace llvm;
using namespace klee;

/* SYSREL extension */

bool symbolizeInlineAssembly = false;

bool skipOptimizingSensitiveRegionCheck = false;

//#define INFOFLOW 1

extern std::set<std::string> voidTypeCasts;
size_t maxVoidTypeCastSize = 8;
std::set<std::string> reached;

/* Side channel begin */
// Type hints for void function args
std::map<std::string, std::map<unsigned int, std::vector<unsigned int> > > externalFuncsWithSensitiveFlow;
std::map<std::string, unsigned int> externalFuncs;
std::map<std::string, std::set<unsigned int> > externalFuncsSig;
std::map<std::string, std::map<int, std::string> > funcArgTypeHints;
extern std::set<std::pair<std::string, int>> cachelocs;
extern unsigned long cacheLineBits;
extern bool cacheLineMode;
extern bool cacheBitmaskMode;
extern unsigned int cacheBitmask;
//  Functions that receive input from the environment
std::set<std::string> inputFuncs;
std::map<std::string, std::set<unsigned int> > inputFuncArgs;
// Maps function arguments to its high security regions
std::map<std::string, std::set<int> > highFunctionArgs;
// Maps function arguments to its low security regions
std::map<std::string, std::set<int> > lowFunctionArgs;  
// Maps function arguments to its mixed security regions
std::map<std::string, std::set<int> > mixedFunctionArgs;  
// Maps function arguments to its high security regions
std::map<std::string, std::set<int> > highFunctionArgsRet;
// Maps function arguments to its low security regions
std::map<std::string, std::set<int> > lowFunctionArgsRet;  
// Maps function arguments to its mixed security regions
std::map<std::string, std::set<int> > mixedFunctionArgsRet; 
// Maps types to its high security regions
std::map<std::string, std::vector<region> > highTypeRegions;
// Maps types to its low security regions
std::map<std::string, std::vector<region> > lowTypeRegions;
// Maps symbolic regions to its high security regions
// unique symbolic names eliminate the need to do the mapping per state
std::map<std::string, std::vector<region> > highSymRegions;
// Maps symbolic regions to its low security regions
// unique symbolic names eliminate the need to do the mapping per state
std::map<std::string, std::vector<region> > lowSymRegions;
// Maps addresses of ExecutionState's to addresses of high security sensitive regions
// regions provide precise tracking of subregions
std::map<long, std::map<ref<Expr>, std::vector<region> > > highMemoryRegions;
// Maps addresses of ExecutionState's to addresses of low security sensitive regions
// regions provide precise tracking of subregions
std::map<long, std::map<ref<Expr>, std::vector<region> > > lowMemoryRegions;
std::map<long, klee::ref<Expr> > branchConditionMap;
std::map<long, llvm::Instruction*> branchInstrMap;
std::set<klee::ExecutionState*> * successorsPaths = new std::set<klee::ExecutionState*>();
extern std::string entryFunctionName;
//extern std::map<std::string, std::set<int> > dynamicHighLoc, dynamicLowLoc;
extern std::map<std::string, std::map<int, std::vector<region> > > dynamicHighRegion, dynamicLowRegion;
std::string publicOutputVarName = "return_value";
std::string publicOutputReturningFName;
ref<Expr> publicOutputVar;
extern std::string sidechannelentry;
bool sidechannelstarted = false;
extern std::set<std::string> * highLoc;
extern std::set<std::string> * lowLoc;
extern std::map<int, RD*> * rdmap;
extern std::vector<std::string> * untrusted;
extern std::set<std::string> prefixRedact;
std::set<RD*> resourceTreeRootList;
RD *root;
RD *currentRD;
std::vector<ExecutionState *> addedStates_copy;
std::vector<ExecutionState *> removedStates_copy;
bool fset = false;
Function *ff;
ArrayCache *acache = NULL;
//int maxForkMulRes = 10;
int maxForkMulRes = 2147483647;
int primArraySize = 32;
int leakCheckOffset = 64;
std::map<std::string, int> uniqueSym;
std::set<std::string> highSecurityLeaksOnStack;
std::set<std::string> codeLocHighSecurityLeaksOnStack;
std::set<std::string> stackLeakToBeChecked;
bool checkLeaksPreciseAndFocused = false;
std::set<std::string> securitySensitiveBranches;
std::map<std::string, int> inputFuncsSuccRetValue;
std::set<long> updatedWithSymbolic;
std::set<unsigned> secretDependentRUSet;
std::map<unsigned, unsigned> secretDependentRUMap;
std::map<unsigned, std::vector<ConstraintManager> > secretDependentRUConstMap;
std::string infoFlowSummarizedFuncName;
Function *infoFlowSummarizedFunc = NULL;
bool leakageWMaxSat = false;
unsigned leakageMaxSat = 0;
unsigned numSecretDependent=0;
unsigned numSecretIndependent=0;
bool pauseSecretIndependent = false;
std::set<long> statesWithCorruptedPC;
extern void recordMemObj(ExecutionState &state, const MemoryObject *mo);
std::map<std::string, std::set<int> > ocallFuncPtrMap;
std::set<std::string> timingObservationPointCaller;
std::map<std::string, int> reachabilityTimingObservationPoints;

std::string getTypeName(Type *t);

bool isATimingObservationFuncPtr(std::string tname, int value) {
   if (tname[0] == '%')
      tname = tname.substr(1);
   if (ocallFuncPtrMap.find(tname) == ocallFuncPtrMap.end())
      return false;
   if (ocallFuncPtrMap[tname].find(value) == ocallFuncPtrMap[tname].end())
      return false;
   return true;
}

void checkTimingObservationFuncPtr(Value *value) {
   if (!isa<Instruction>(value)) return;
   Instruction *inst = (Instruction*)value;
   switch (inst->getOpcode()) {
     case Instruction::BitCast:
        checkTimingObservationFuncPtr(inst->getOperand(0));     
     case Instruction::GetElementPtr: {
        GetElementPtrInst *gep = static_cast<GetElementPtrInst*>(inst);
        if (gep) {
           //llvm::errs() << "gep: " << (*gep) << "\n";
           int index = 0;
           for(User::op_iterator oi = gep->idx_begin(), oe = gep->idx_end(); oi != oe; oi++, index++) {
              if (index > 1) break;
              else if (index == 0) continue; 
              Value *v = oi->get();
              if (llvm::ConstantInt* CI = dyn_cast<llvm::ConstantInt>(v)) {
                 Type *t = gep->getSourceElementType();
                 if (t) {
                    if (t->isPointerTy()) 
                       t = t->getPointerElementType();
                    std::string tname = getTypeName(t);
                    if (CI->getSExtValue() >= 0 && isATimingObservationFuncPtr(tname, CI->getSExtValue())) {
                       llvm::errs() << "function " << inst->getParent()->getParent()->getName() << " calls timing observation point " << tname << " field " <<  CI->getSExtValue() << "\n";
                       timingObservationPointCaller.insert(inst->getParent()->getParent()->getName());
                       
                    } 
                 }
              }
           }
        }     
        break;
     }
     case Instruction::Load: {
        LoadInst *linst = static_cast<LoadInst*>(inst);
        checkTimingObservationFuncPtr(linst->getPointerOperand());
     }
     default:       
        return;
   }
}


bool isIndirectCall(CallSite *cs) {
   const Value *V = cs->getCalledValue();
   if (isa<Function>(V) || isa<Constant>(V))
     return false;
   if (const CallInst *CI = dyn_cast<CallInst>(V))
     if (CI->isInlineAsm())
       return false;
  return true;
}


void extractFunctionsCallingTimingObservationPoints(KModule *kmodule) {
    
   for (Module::iterator F = kmodule->module->begin(); F != kmodule->module->end(); F++) {
       for (Function::iterator bb = F->begin(), e = F->end(); bb != e; ++bb) {
           for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {
               Instruction &ii = *i;
               switch (ii.getOpcode()) {
                 case Instruction::Invoke:
                 case Instruction::Call: {
                    CallSite cs(&ii);
                    if (isIndirectCall(&cs)) {
                       checkTimingObservationFuncPtr(cs.getCalledValue());
                    }
                    break;
                 }
                 default: continue;
               }
           }
       }
   }
}

void Executor::recordMostRecentBranchInfo(ExecutionState &state, llvm::Instruction *brinst) {
  long sid = (long)&state;
  branchInstrMap[sid] = brinst;     
  //llvm::errs() << "Recorded most recent branch as " << (*brinst) << "\n";
}

void Executor::getMostRecentBranchInfo(ExecutionState &state, llvm::Instruction *&brinst, bool &found) {
  long sid = (long)&state;
  if (branchInstrMap.find(sid) == branchInstrMap.end()) {
     found = false;
     return;
  }
  brinst = branchInstrMap[sid];
  found = true;
  //llvm::errs() << "Most recent branch: " << (*brinst) << "\n"; 
}

//#ifdef INFOFLOW
// InfoFlowSummarization
struct infoflowdata {
  std::map<unsigned int, const MemoryObject*> *regMap;
  std::map<unsigned int, std::map<region, std::set<InfoFlowRegion_t> > > *localIFlow;
};
typedef struct infoflowdata infoflowdata_t;
std::map<ExecutionState*, std::vector<infoflowdata_t>* > infoflowstack;
infoflowdata_t currentInfoFlowContext;
std::map<ExecutionState*, 
         std::map<const MemoryObject*, 
                  std::map<region, 
                           std::set<InfoFlowRegion_t> > > > memoryInfoFlow;
// Maps info flow relevant memory objects to ids that are meaningful to the client, e.g., argument indices of a summarized function
std::map<int, int> argRegsInfoFlow;
// dest index to dest memory region to source memory regions 
std::map<int, std::map<InfoFlowRegion_t, std::set<InfoFlowRegion_t> > >  infoFlowSummary;
std::map<ExecutionState*, std::map<int, std::map<InfoFlowRegion_t, std::set<InfoFlowRegion_t> > > > infoFlowSummaryPerState;

/*
bool operator<(const region r1, const region r2) {
  if (r1.offset == r2.offset)
     return r1.size < r2.size;
  else if (r1.size == r2.size) 
     return r2.offset < r2.offset;
  else r1.offset + r1.size  < r2.offset + r2.size;
}*/

namespace std {
bool operator<(const InfoFlowRegion_t ifr1, const InfoFlowRegion_t ifr2) {
   if (ifr1.index < ifr2.index)
      return true;
   else if (ifr1.index == ifr2.index) {
      if (ifr1.regions.size() < ifr2.regions.size())
         return true;
      else if (ifr1.regions.size() > ifr2.regions.size())
         return false;
      else {
         for(unsigned int i=0; i<ifr1.regions.size(); i++)
            if (ifr1.regions[i] < ifr2.regions[i])
               return true;
         return false; 
      }
   } 
   else return false;
}
}

//#endif

/* Side channel end */

//#define VBSC

#define ASYNC_STR "async_initiate"
#define ENABLE_STR "enable_entry"
//#define PRIM_LAZY_INIT_SIZE  64
//#define PRIM_LAZY_INIT_SIZE  32
#define STRUCT_LAZY_INIT_INS 10
// this is to avoid assertion failure
// represents size in bits so SIZE_FOR_UNTYPED (2000) * 8
#define SIZE_FOR_UNKNOWN_TYPES 8*8
int MAX_ALLOC_SIZE=10240;

cl::opt<bool>
InitFuncPtrs("init-funcptr-fields" , cl::desc("Set function pointer fields to null when lazy initializing struct type objects"),
             cl::init(false));


cl::opt<bool>
BreakSelectStmt("fork-for-select", cl::desc("Fork states to simulate the select statement\n"),
           cl::init(false));

cl::opt<bool>
PreferredResolution("use-one-for-resol", cl::desc("Use the candidate memory object stored for the relevant symbolic \ 
                                  address expression for symbolic address resolution!\n"), cl::init(false));
std::map<long, std::map<ref<Expr>, const MemoryObject *> > addressToMemObj;
std::map<long, std::map<ref<Expr>, ref<Expr> > > symIndexToMemBase;
extern std::set<std::string> assemblyFunctions;



KModule *kmoduleExt;
llvm::Function *entryFunc;
extern const Module * moduleHandle;
extern bool asyncMode;
extern std::vector<std::string> asyncFunc;
extern std::vector<std::string> enabledFunc;
extern APIHandler *apiHandler;
extern bool progModel;
extern std::map<std::string, std::map<unsigned int, int> > lazyInitInitializersWValues;
extern std::map<std::string, std::vector<unsigned int> > lazyInitInitializers;
extern std::map<std::string, std::map<unsigned int, std::string> > lazyFuncPtrInitializers;

std::string getAsyncFunction(std::string fn) {

  return fn.substr(fn.find(ASYNC_STR) + strlen(ASYNC_STR) + 1, fn.size());
}

std::string getEnableFunction(std::string fn) {

  return fn.substr(fn.find(ENABLE_STR) + strlen(ENABLE_STR) + 1, fn.size());
}

bool isAssemblyFunc(std::string name) {
  for(auto af : assemblyFunctions)
     if (af == name)
        return true;
  return false;
}

extern int minInstCount;
extern int maxInstCount;

extern bool lazyInit;
extern bool lazySpec;
extern int numLazyInst;
extern std::vector<std::string> lazyInits;
extern std::set<std::string> lazyInitSingles;
extern std::map<std::string, int> lazyInitNumInstances;
//std::set<std::string> embeddedTypes;
std::set<Type*> embeddedTypes;
//std::map<std::string, std::set<std::string> >  embeddings;
std::map<Type*, std::set<Type*> >  embeddingTypes;
bool singleSuccessor = true;


bool infoFlowSummaryMode = false;


//#ifdef INFOFLOW
/* InfoFlowSummarization */

region adjustment(region r1, region r2);

std::set<InfoFlowRegion_t> adjust(std::set<InfoFlowRegion_t> ifrs, region adj);

std::vector<region> diffPartitionRegion(region r1, std::vector<region> rgs);

std::map<region, std::set<InfoFlowRegion_t> > slice(std::map<region, std::set<InfoFlowRegion_t> > ifrmap, region rt);

std::set<InfoFlowRegion_t> setIF(InfoFlowRegion_t &ifr) {
  std::set<InfoFlowRegion_t> res;
  res.insert(ifr);
  return res;
}

void print(const InfoFlowRegion_t &ifr) {
  llvm::errs() << ifr.index << ",";
  for(unsigned int i=0; i<ifr.regions.size(); i++)
     llvm::errs() << "[" << ifr.regions[i].offset << ":" << ifr.regions[i].size << "]";
}

void print(std::fstream &fstr, const InfoFlowRegion_t &ifr) {
  fstr << ifr.index << ",";
  for(unsigned int i=0; i<ifr.regions.size(); i++)
     fstr << "[" << ifr.regions[i].offset << ":" << ifr.regions[i].size << "]";
}

void print(std::set<InfoFlowRegion_t> ifrs) {
  llvm::errs() << "{";
  unsigned int i=0;
  for(auto ifr : ifrs) {
     print(ifr);
     if (i < ifrs.size() - 1)
        llvm::errs() << "\n";
  }
  llvm::errs() << "}\n";
  
}

void print(std::fstream &fstr, std::set<InfoFlowRegion_t> ifrs) {
  fstr << "{";
  unsigned int i=0;
  for(auto ifr : ifrs) {
     print(fstr, ifr);
     if (i < ifrs.size() - 1)
        fstr << "\n";
  }
  fstr << "}\n";
  
}



void dumpInfoFlow() {
  llvm::errs() << "Info Flow Summary for function " << infoFlowSummarizedFuncName << "\n"; 
  for(auto ifse : infoFlowSummary) {
     for(auto ifse2 : ifse.second) {
           print(ifse2.second);
           llvm::errs() << "\t===>\n";
           print(ifse2.first);
           llvm::errs() << "\n\n";
     }
  }
}

void writeInfoFlowSummary(const char *fname) {
   std::fstream rc(fname, std::fstream::out);
   if (rc.is_open()) {
      for(auto ifse : infoFlowSummary) {
         for(auto ifse2 : ifse.second) {
            print(rc, ifse2.second);
            rc << "=>\n";
            print(rc, ifse2.first);
            rc << "\n"; 
         }
      }
      rc.close();
   }
}

void readInfoFlowSummary(char *fname){
}


bool isLocal(int reg) {
   return (currentInfoFlowContext.regMap->find(reg) != currentInfoFlowContext.regMap->end());
}

void Executor::recordArgRegisters(Function *f) {
   KFunction *kf = kmodule->functionMap[f];
   unsigned int argi = 0;
   for(llvm::Function::arg_iterator ai = f->arg_begin(); ai != f->arg_end(); ai++, argi++) {
      argRegsInfoFlow[kf->getArgRegister(argi)] = argi;
      llvm::errs() << "recording arg " << argi << " and the register " << kf->getArgRegister(argi) << " as info flow relevant in " << f->getName() << "\n"; 
   }     
}

int isInfoFlowRelevant(KInstruction *ki, int regIndex) {
   Function *f = ki->inst->getParent()->getParent();
   if (f == infoFlowSummarizedFunc) {
      if (argRegsInfoFlow.find(regIndex) != argRegsInfoFlow.end())
         return argRegsInfoFlow[regIndex];
      else return -1; 
   }
   else return -1;
}

int getLocal(ExecutionState &state, KInstruction *ki, int index) {
  assert(index >=0 && index < ki->inst->getNumOperands());
  int vnumber = ki->operands[index];
  return vnumber;
}

void initInfoFlowContext(ExecutionState &state) {
   if (!infoFlowSummaryMode) return;
   infoflowdata_t initvalue;
   initvalue.regMap = new std::map<unsigned int, const MemoryObject*>();
   initvalue.localIFlow = new std::map<unsigned int, std::map<region, std::set<InfoFlowRegion_t> > >();
   std::vector<infoflowdata_t> *ctxstack = new std::vector<infoflowdata_t>();
   currentInfoFlowContext = initvalue;
   ctxstack->push_back(initvalue);
   infoflowstack[&state] = ctxstack;
   llvm::errs() << "init info flow context for " << &state << "\n"; 
}

void cloneInfoFlowContext(ExecutionState &state1, ExecutionState &state2) {
    if (!infoFlowSummaryMode) return;
    std::vector<infoflowdata_t>  *cpstack = new std::vector<infoflowdata_t>();
    for(unsigned int i=0; i< infoflowstack[&state1]->size(); i++)  
       cpstack->push_back((*infoflowstack[&state1])[i]);
    infoflowstack[&state2] = cpstack;
    memoryInfoFlow[&state2] = memoryInfoFlow[&state1];
    llvm::errs() << "cloning info flow context from " << &state1 << " to " << &state2 << "\n";
}

void updateCurrentInfoFlowContext(ExecutionState &state) {
  if (!infoFlowSummaryMode) return;
  int last = infoflowstack[&state]->size() - 1; 
  currentInfoFlowContext = (*infoflowstack[&state])[last];
  llvm::errs() << "switching to state " << &state << "\n";
}

void pushInfoFlowContext(ExecutionState &state) {
   if (!infoFlowSummaryMode) return;
   int last = infoflowstack[&state]->size() - 1;
   (*infoflowstack[&state])[last] = currentInfoFlowContext;
   infoflowdata_t initvalue;
   initvalue.regMap = new std::map<unsigned int, const MemoryObject*>();
   initvalue.localIFlow = new std::map<unsigned int, std::map<region, std::set<InfoFlowRegion_t> > >();
   infoflowstack[&state]->push_back(initvalue);
   currentInfoFlowContext = initvalue;
   llvm::errs() << "pushing info flow context for " << &state << "\n";
}

void popInfoFlowContext(ExecutionState &state) {
   if (!infoFlowSummaryMode) return; 
   infoflowstack[&state]->pop_back();
   currentInfoFlowContext =  infoflowstack[&state]->back();
   llvm::errs() << "poping info flow context for " << &state << "\n";
}

void saveInfoFlowContext(ExecutionState &state) {
   if (!infoFlowSummaryMode) return;
   int last = infoflowstack[&state]->size() - 1;
   (*infoflowstack[&state])[last] = currentInfoFlowContext;       
   llvm::errs() << "saving info flow context for " << &state << "\n"; 
}

bool operator==(region r1, region r2) {
  return ((r1.offset == r2.offset) && (r1.size == r2.size));
}

int getRegIndexFor(const MemoryObject *mo) {
  for(auto me : (*currentInfoFlowContext.regMap)) {
     if (me.second == mo)
        return me.first;
  }  
  return -1;
}

InfoFlowRegion_t createInfoFlowRegion(int source, unsigned int offset, unsigned int size) { 
    InfoFlowRegion_t ifr;
    ifr.index = source;
    region r;
    r.offset = offset;
    r.size = size;
    ifr.regions.push_back(r);
    return ifr;
}

void addLevel(InfoFlowRegion_t &dc, unsigned int offset, unsigned int size) {
   llvm::errs() << "adding a point of indirection to ";
   print(dc);
   llvm::errs() << "\n";
   region r;
   r.offset = offset;
   r.size = size;
   dc.regions.push_back(r);
}

std::set<InfoFlowRegion_t> addLevel(std::set<InfoFlowRegion_t> dcs, unsigned int offset, unsigned int size) {
    std::set<InfoFlowRegion_t> res;
    for(auto dc : dcs) {
       InfoFlowRegion_t dci;
       dci.index = dc.index;
       for(unsigned int i=0; i<dc.regions.size(); i++)
          dci.regions.push_back(dc.regions[i]);
       addLevel(dci, offset, size);
       res.insert(dci);
    } 
    print(res);
    return res;
}


void partitionAndUpdateIFlow(std::map<region, std::set<InfoFlowRegion_t> > &ifrmap, 
                                          region rt, std::set<InfoFlowRegion_t> ifrc, bool replace);


void recordInfoFlow(InfoFlowRegion_t dest, std::set<InfoFlowRegion_t> sourceif) {
  int desti  = dest.index;
  std::map<InfoFlowRegion_t, std::set<InfoFlowRegion_t> > ifsmap;
  std::set<InfoFlowRegion_t>sources;  
  if (infoFlowSummary.find(desti) != infoFlowSummary.end())
      ifsmap = infoFlowSummary[desti];
  if (ifsmap.find(dest) != ifsmap.end())
     sources = ifsmap[dest];
  for(auto se : sourceif)
     sources.insert(se);
  ifsmap[dest] = sources;
  infoFlowSummary[desti] = ifsmap;
}  

void collectInfoFlow() {
  for(auto ifse : infoFlowSummaryPerState) 
     for(auto ifse2 : ifse.second) 
        for(auto ifse3 : ifse2.second) 
           recordInfoFlow(ifse3.first, ifse3.second);
  
}

void clearInfoFlowSummaryPerState(ExecutionState &state, InfoFlowRegion_t dest) {
  int desti  = dest.index;
  std::map<int, std::map<InfoFlowRegion_t, std::set<InfoFlowRegion_t> > > ifsmapt; 
  std::map<InfoFlowRegion_t, std::set<InfoFlowRegion_t> > ifsmap;
  std::set<InfoFlowRegion_t>sources;  
  if (infoFlowSummaryPerState.find(&state) != infoFlowSummaryPerState.end())
      ifsmapt = infoFlowSummaryPerState[&state];
  if (ifsmapt.find(desti) != ifsmapt.end()) {
      ifsmap = ifsmapt[desti];
  }
  if (ifsmap.find(dest) == ifsmap.end())
     return;
  ifsmap.erase(dest);
  ifsmapt[desti] = ifsmap; 
  infoFlowSummaryPerState[&state] = ifsmapt;
}

void recordInfoFlow(ExecutionState &state, InfoFlowRegion_t dest, InfoFlowRegion_t sourceif) {
  int desti  = dest.index;
  std::map<int, std::map<InfoFlowRegion_t, std::set<InfoFlowRegion_t> > > ifsmapt; 
  std::map<InfoFlowRegion_t, std::set<InfoFlowRegion_t> > ifsmap;
  std::set<InfoFlowRegion_t>sources;  
  if (infoFlowSummaryPerState.find(&state) != infoFlowSummaryPerState.end())
      ifsmapt = infoFlowSummaryPerState[&state];
  if (ifsmapt.find(desti) != ifsmapt.end()) {
      ifsmap = ifsmapt[desti];
  }
  //if (ifsmap.find(srci) != ifsmap.end())
  //    ifsmaprg = ifsmap[srci];
  if (ifsmap.find(dest) != ifsmap.end())
     sources = ifsmap[dest];
  sources.insert(sourceif);
  ifsmap[dest] = sources;
  ifsmapt[desti] = ifsmap; 
  infoFlowSummaryPerState[&state] = ifsmapt;
  llvm::errs() << "RECORDING info flow to destination=" << desti << "\n";
  print(sources);
  llvm::errs() << "===>\n";
  print(dest);
  llvm::errs() << "\n";
}

/*void checkAndMarkPointerRegForLoad(ExecutionState &state, KInstruction *target, int destregIndex) {
   bool doublepointer = false;
   llvm::LoadInst *li = dyn_cast<llvm::LoadInst>(target->inst);
   assert(li);
   Type *t = li->getPointerOperand()->getType();
   if (t->isPointerTy()) {
      t = t->getPointerElementType();
      if (t->isPointerTy())
            doublepointer = true;
   }
   if (doublepointer) {
      std::set<unsigned int> prs = currentInfoFlowContext.pointerRegs;
      prs.insert(destregIndex);
      currentInfoFlowContext.pointerRegs = prs; 
   }
}

void checkAndMarkPointerRegForStore(ExecutionState &state, KInstruction *target, int destregIndex) {
   bool doublepointer = false;
   llvm::StoreInst *li = dyn_cast<llvm::StoreInst>(target->inst);
   if (li) {
      Type *t = li->getPointerOperand()->getType();
      if (t->isPointerTy()) {
         t = t->getPointerElementType();
         if (t->isPointerTy())
            doublepointer = true;
      }
      if (doublepointer) {
         std::set<unsigned int> prs = currentInfoFlowContext.pointerRegs;
         prs.insert(destregIndex);
         currentInfoFlowContext.pointerRegs = prs;
      }
  }
}*/


/* Information may flow the memory location that is read or 
   from the information source represented by the address stored on the register that holds the address*/
void Executor::updateInfoFlowForLoad(ExecutionState &state, 
                                    int regIndex, 
                                int destregIndex, 
                                const MemoryObject *mo, 
                            KInstruction *target, 
                            ref<Expr> offsetexpr, 
                                   unsigned size) {
   if (!infoFlowSummaryMode) return;
   ConstantExpr *CE = dyn_cast<ConstantExpr>(offsetexpr);
   unsigned offset;
   if (!CE) { 
      // conservative approximation when offset is symbolic
      offset = 0;
      size = mo->size;
      llvm::errs() << "handling symbolic offset by conservatively using 0 and " << mo->size << "for size\n"; 
   }
   else offset = CE->getZExtValue();
   int sourceIndex = getRegIndexFor(mo);
   region rt;
   rt.offset = 0;
   rt.size = size;
   region rs;
   rs.offset = offset;
   rs.size = size;

   bool doublepointer = false;
   llvm::LoadInst *li = dyn_cast<llvm::LoadInst>(target->inst);
   assert(li);
   Type *t = li->getPointerOperand()->getType();
   if (t->isPointerTy()) {
      t = t->getPointerElementType();
      if (t->isPointerTy())
            doublepointer = true;
   }
   llvm::errs() << "Info Flow For LOAD register=" << regIndex << " offset=" << offset << " size=" << size << "\n";
   std::map<const MemoryObject*, 
                  std::map<region, 
                           std::set<InfoFlowRegion_t> > > memoryIFlow = memoryInfoFlow[&state];
   bool update = false;
   std::vector<region> updated;
   if (memoryIFlow.find(mo) != memoryIFlow.end()) {
         llvm::errs() << "Transferring info in LOAD: " << target->inst << "\n";
         llvm::errs() << "source register=" << regIndex << "\n"; 
         std::map<region, std::set<InfoFlowRegion_t> > ifrmaps =  slice(memoryIFlow[mo], rs);  
         std::map<region, std::set<InfoFlowRegion_t> > ifrmap = (*currentInfoFlowContext.localIFlow)[target->dest];         
         for(auto me : ifrmaps) {
            std::set<InfoFlowRegion_t> dc = me.second;
            if (!isLocal(regIndex))
               dc = addLevel(dc, offset, size);
            partitionAndUpdateIFlow(ifrmap, me.first, dc, true);
            updated.push_back(me.first);
         }
         (*currentInfoFlowContext.localIFlow)[target->dest] = ifrmap;
   }
   std::vector<region> notupdated = diffPartitionRegion(rt, updated);
   if (notupdated.size() > 0) {
      llvm::errs() << "Trying to fill uncovered " << notupdated.size() << " regions in LOAD after handling flow from memory\n";
      int source = isInfoFlowRelevant(target, regIndex);    
      if (source >= 0) {
         // info flow tracking relevant
         InfoFlowRegion_t ifr;
         ifr = createInfoFlowRegion(source, offset, size);   
         if (!isLocal(regIndex)) 
            addLevel(ifr, offset, size); 
         llvm::errs() << "Adding info flow source in LOAD: " << target->inst << "\n";
         print(ifr);
         std::set<InfoFlowRegion_t> ifrs;
         ifrs.insert(ifr);
         std::map<region, std::set<InfoFlowRegion_t> > ifrmap  = (*currentInfoFlowContext.localIFlow)[target->dest];
         for(auto rtp : notupdated) 
            partitionAndUpdateIFlow(ifrmap, rtp, adjust(ifrs, adjustment(rt, rtp)), true);
         (*currentInfoFlowContext.localIFlow)[target->dest] = ifrmap;
      }
      else if (currentInfoFlowContext.localIFlow->find(regIndex) != currentInfoFlowContext.localIFlow->end()) {
         // not updated yet with any infoflow so let's see if this refers to an inforelevant address
         std::map<region, std::set<InfoFlowRegion_t> > ifrmap  = (*currentInfoFlowContext.localIFlow)[target->dest];
         for(auto rtp : notupdated) {
            llvm::errs() << "region " << rtp.offset << ":" << rtp.size << "\n";
            //std::map<region, std::set<InfoFlowRegion_t> > ifrmaps2 = slice(currentInfoFlowContext.localIFlow[regIndex], rtp);
            std::map<region, std::set<InfoFlowRegion_t> > ifrmaps2 = (*currentInfoFlowContext.localIFlow)[regIndex];
            std::map<region, std::set<InfoFlowRegion_t> > ifrmaps;
            if (!isLocal(regIndex)) {
               for(auto me : ifrmaps2) {
                  ifrmaps[rtp] = addLevel(me.second, rtp.offset, rtp.size); 
               }
            }
            else ifrmaps = ifrmaps2;
            for(auto me : ifrmaps)  {
               partitionAndUpdateIFlow(ifrmap, rtp, adjust(me.second, adjustment(me.first, rtp)), true);
            }
         }
         (*currentInfoFlowContext.localIFlow)[target->dest] = ifrmap;
      }
   }
}

bool regionsIntersect(region r1, region r2) {
   if (r1.offset <= r2.offset && r1.offset + r1.size - 1 <= r2.offset + r2.size - 1)
      return true;
   if (r2.offset <= r1.offset && r2.offset + r2.size - 1 <= r1.offset + r1.size - 1)
      return true;
   return false;
}

region intersection(region r1, region r2) {
  region r;
  r.size = r.offset = 0;
  if (r1.offset <= r2.offset && r1.offset + r1.size - 1 <= r2.offset + r2.size - 1) {
     r.offset = r2.offset;
     r.size = r1.offset + r1.size - r2.offset;
  }
  if (r2.offset <= r1.offset && r2.offset + r2.size - 1 <= r1.offset + r1.size - 1) {
     r.offset = r1.offset;
     r.size = r2.offset + r2.size - r1.offset;
  }
  return r;
}

region diffRegion(region r1, region r2) {
  region r = r1;
  if (r1.offset == r2.offset) {
     if (r1.size <= r2.size) 
        r.size = 0;
     else {
        r.offset = r1.size + r2.size;
        r.size = r1.size - r2.size;
     } 
  }
  else if (r1.offset < r2.offset && r1.offset + r1.size - 1 <= r2.offset + r2.size - 1) {
     r.offset = r1.offset;
     r.size = r2.offset - r1.offset;
  }
  else if (r2.offset < r1.offset && r2.offset + r2.size - 1 <= r1.offset + r1.size - 1) {
     r.offset = r2.offset + r2.size;
     r.size = r1.offset + r1.size - (r2.offset + r2.size);
  }
  return r;       
}

// pre: all regions in rgs intersect with r1 and they are sorted w.r.t. offset
std::vector<region> diffPartitionRegion(region r1, std::vector<region> rgs) {
  std::vector<region> res;
  region uncovered = r1;
  for(unsigned int i=0; i<rgs.size(); i++) {
     if (uncovered.size <= 0) break;
     region r;
     if (rgs[i].offset == uncovered.offset) {
        uncovered.size -= rgs[i].size;
        uncovered.offset += rgs[i].size;
     }
     else if (rgs[i].offset < uncovered.offset) {
        uncovered.size -=  (rgs[i].offset + rgs[i].size - uncovered.offset);
        uncovered.offset = rgs[i].offset + rgs[i].size;
     }
     else {
        r.offset = uncovered.offset;
        r.size = rgs[i].offset - uncovered.offset;
        res.push_back(r);
        uncovered.offset = rgs[i].offset + rgs[i].size;
        uncovered.size -= (r.size + rgs[i].size);
     }
  }
  if (uncovered.size > 0)
     res.push_back(uncovered);
  return res;
}

region adjustment(region r1, region r2) {
  region r;
  r.offset = r2.offset - r1.offset;  
  r.size = r2.size - r1.size;
  return r;
}

region adjust(region r, region adj) {
   region res;
   res.offset = r.offset + adj.offset;
   res.size = r.size + adj.size;
   return res;
}

std::vector<region> adjust(std::vector<region> r, region adj) {
  std::vector<region> res;
  for(unsigned int i=0; i<r.size(); i++) {
     res.push_back(adjust(r[i], adj));
  }
  return res; 
}

InfoFlowRegion_t adjust(InfoFlowRegion_t ifr, region adj) {
  llvm::errs() << "adj = " << adj.offset << "," << adj.size << "\n";
  InfoFlowRegion_t res;
  res.index = ifr.index;
  for(unsigned int i=0; i<ifr.regions.size(); i++)
     res.regions.push_back(adjust(ifr.regions[i], adj));
  return res;
}

std::set<InfoFlowRegion_t> adjust(std::set<InfoFlowRegion_t> ifrs, region adj) {
  std::set<InfoFlowRegion_t> res;
  for(auto ifr: ifrs) { 
     InfoFlowRegion_t ifr2 = ifr;
     res.insert(adjust(ifr2,adj));
  }
  return res;
}

std::map<region, std::set<InfoFlowRegion_t> > slice(std::map<region, std::set<InfoFlowRegion_t> > ifrmap, region rt) {
  std::map<region, std::set<InfoFlowRegion_t> > res;
  if (rt.size == 0)
     return res;
  region rd;
  std::vector<region> tobesliced;
  std::vector<std::set<InfoFlowRegion_t> > tobeslicedFlow;
  for(auto me : ifrmap) {
     rd = diffRegion(me.first, rt);
     region ri = intersection(me.first, rt);
     if (rd.size == 0) {
        res[me.first] = ifrmap[me.first];
     }
     else if (ri.size > 0) {
        tobesliced.push_back(me.first);
        tobeslicedFlow.push_back(me.second); 
     }
     // otherwise drop it completely
  }
  for(unsigned int i=0; i<tobesliced.size(); i++) {
     region ri = intersection(tobesliced[i], rt);
     region rd1 = diffRegion(tobesliced[i], rt);
     region adj = adjustment(tobesliced[i], rd1);
     std::set<InfoFlowRegion_t> ifrus;
     for(auto tbsifr : tobeslicedFlow[i]) {
        InfoFlowRegion_t ifru;
        ifru.index = tbsifr.index;
        for(unsigned int j=0; j<tbsifr.regions.size(); j++)
           ifru.regions.push_back(adjust(tbsifr.regions[j], adj));
        ifrus.insert(ifru);
     }
     res[ri] = ifrus;
  }
  return res;
} 

void partitionAndUpdateIFlow(std::map<region, std::set<InfoFlowRegion_t> > &ifrmap, 
                                          region rt, std::set<InfoFlowRegion_t> ifrcs, bool replace) {

  bool exactMatch = false;
  std::set<InfoFlowRegion_t> rgs;
  for(auto me : ifrmap) {
     if (me.first == rt) {
        exactMatch = true;
        rgs = me.second;
        break;
     }
  }
  if (exactMatch) {
     if (replace)  {
        ifrmap[rt] = ifrcs;
        llvm::errs() << "exact match, setting region " << rt.offset << ":" << rt.size << "to:\n";
        print(ifrcs);
     }
     else {
        llvm::errs() << "exact match, expanding info flow:\n";
        print(ifrcs);
        for(auto rgse : rgs) {
           ifrcs.insert(rgse);
        } 
        llvm::errs() << "into:\n";
        print(ifrcs);
        ifrmap[rt] = ifrcs;
     }
  }
  else {
     std::vector<region> covered;
     std::vector<std::set<InfoFlowRegion_t> > infr;
     for(auto me : ifrmap) {
        if (regionsIntersect(me.first, rt)) { 
           covered.push_back(me.first);
           infr.push_back(me.second);
        }  
     }
     std::vector<region> toberemoved;
     for(unsigned int i=0; i<covered.size(); i++) {
        ifrmap.erase(covered[i]);
        region rd1 = diffRegion(covered[i], rt);
        if (rd1.size > 0)  
           ifrmap[rd1] = adjust(infr[i], adjustment(covered[i], rd1));
        //region rd2 = diffRegion(rt, covered[i]);
        //if (rd2.size > 0)    
        //   ifrmap[rd2] = adjust(ifrcs, adjustment(rt, covered[i]));
        region rd3 = intersection(covered[i], rt);
        if (rd3.size > 0) {
           toberemoved.push_back(rd3);
           if (replace) {
              ifrmap[rd3] = adjust(ifrcs, adjustment(rt, rd3));
              llvm::errs() << "setting common subregion " << rd3.offset << ":" << rd3.size << "to:\n";
              print(ifrmap[rd3]);
           }
           else {
              llvm::errs() << "expanding info flow:\n";
              std::set<InfoFlowRegion_t> aifrcs = adjust(ifrcs, adjustment(rt, rd3));
              print(aifrcs);              
              for(auto infre: infr[i]) 
                 aifrcs.insert(adjust(infre, adjustment(covered[i], rd3)));
              ifrmap[rd3] = aifrcs;          
              llvm::errs() << "into:\n";
              print(aifrcs);               
           }
        }
     }
     if (covered.size() > 0) {
        std::vector<region> rts = diffPartitionRegion(rt, toberemoved);
        for(unsigned int k=0; k<rts.size(); k++) {
           ifrmap[rts[k]] = adjust(ifrcs, adjustment(rt, rts[k]));
           llvm::errs() << "setting partitioned region " << rts[k].offset << "," << rts[k].size << " :\n";
           print(ifrmap[rts[k]]);
        }     
     }
     else { //if (covered.size() == 0) {
        ifrmap[rt] = ifrcs;
        llvm::errs() << "setting target region " << rt.offset << ":" << rt.size << "to:\n";
        print(ifrcs);
     }
  }
}


void UpdateIFlow(std::map<region, std::set<InfoFlowRegion_t> > &ifrmap, 
                     std::map<region, std::set<InfoFlowRegion_t> > ifrmaps, int offsetAdj, bool replace) {
    for(auto me : ifrmaps) {
        region rt;
        rt.offset = me.first.offset + offsetAdj;
        rt.size = me.first.size;
        partitionAndUpdateIFlow(ifrmap, rt, me.second, replace);
    }
}

void Executor::updateInfoFlowForGeptr(ExecutionState &state,
                                              int regIndex, 
                                      KInstruction *target,
                                             int offset) {
   if (!infoFlowSummaryMode) return;
   if (regIndex < 0) return; 
   updateInfoFlowDirectCopy(state, regIndex, target);
   /*
   if (currentInfoFlowContext.localIFlow->find(regIndex) != currentInfoFlowContext.localIFlow->end()) {
      std::set<InfoFlowRegion_t> ifrs, ifrs2;
      for(auto me : (*currentInfoFlowContext.localIFlow)[regIndex]) {
         ifrs = me.second;
         break;
      }
      int size = getWidthForLLVMType(target->inst->getType());
      ifrs2 = addLevel(ifrs, offset, size);
      std::map<region, std::set<InfoFlowRegion_t> > ifrmap;
      region rt; rt.offset = 0; rt.size = size;
      ifrmap[rt] = ifrs2;
      (*currentInfoFlowContext.localIFlow)[target->dest] = ifrmap;
   }*/
}
 
void Executor::updateInfoFlowForStore(ExecutionState &state, 
                                     int regIndex, 
                                 int destregIndex, 
                                 const MemoryObject *mo, 
                             KInstruction *target, 
                             ref<Expr> offsetexpr, 
                                    unsigned size) {
   if (!infoFlowSummaryMode) return;
   //if (regIndex < 0) return; 
   ConstantExpr *CE = dyn_cast<ConstantExpr>(offsetexpr);
   unsigned offset;
   if (!CE)
      offset = 0;
   else offset = CE->getZExtValue();
   region rs;
   rs.offset = 0;
   rs.size = size;
   region rt;
   rt.offset = offset;
   rt.size = size;
   InfoFlowRegion_t destif, sourceif;
   std::set<InfoFlowRegion_t> destifs, sourceifs;
   bool record = false;
   bool doublepointer = false;
   llvm::StoreInst *li = dyn_cast<llvm::StoreInst>(target->inst);
   if (!li) return;
   Type *t = li->getPointerOperand()->getType();
   if (t->isPointerTy()) {
      t = t->getPointerElementType();
      if (t->isPointerTy())
            doublepointer = true;
   }
   llvm::errs() << "Info Flow For STORE register=" << regIndex << "\n";
   int dest = isInfoFlowRelevant(target, destregIndex); 
   if (dest >= 0) {
      // info flow tracking relevant
      destif = createInfoFlowRegion(dest, offset, size);  
      llvm::errs() << "Adding info flow relevant destination in STORE: " << target->inst << "\n";
      destifs = setIF(destif);
      print(destifs);
      record = true;
   }
   else if (currentInfoFlowContext.localIFlow->find(destregIndex) != currentInfoFlowContext.localIFlow->end()) {
      record = true;
      for(auto de : (*currentInfoFlowContext.localIFlow)[destregIndex]) {
         destifs = de.second;
         if (!isLocal(destregIndex))
            destifs = addLevel(destifs, offset, size);
         record = true;
         llvm::errs() << "Adding info flow relevant destination from register in STORE: " << target->inst << "\n";
         print(destifs);
         break;
      }
   } 
   int source = isInfoFlowRelevant(target, regIndex); 
   if (source >= 0  || currentInfoFlowContext.localIFlow->find(regIndex) != currentInfoFlowContext.localIFlow->end()) {
      std::map<region, std::set<InfoFlowRegion_t> > ifrmap, ifrmaps;
      if (source >= 0) {
         sourceif = createInfoFlowRegion(source, offset, size);   
         llvm::errs() << "Adding info flow relevant source in STORE: " << target->inst << "\n";
         sourceifs = setIF(sourceif);
         print(sourceifs);
         ifrmaps[rs] = sourceifs;
      }
      else {
         ifrmaps = slice((*currentInfoFlowContext.localIFlow)[regIndex], rs);
      }
      std::map<const MemoryObject*, 
                  std::map<region, 
                           std::set<InfoFlowRegion_t> > > memoryIFlow = memoryInfoFlow[&state];
      if (memoryIFlow.find(mo) != memoryIFlow.end())
         ifrmap = memoryIFlow[mo];  
      UpdateIFlow(ifrmap, ifrmaps, rt.offset - rs.offset, true);      
      memoryIFlow[mo] = ifrmap;
      memoryInfoFlow[&state] = memoryIFlow; 
      if (record) {
         for(auto dc : destifs) {
            for(auto se : ifrmaps) {
               for(auto see : se.second) {
                  recordInfoFlow(state, dc, see); 
               }
            }
         }
      }
   }
   else if (record) {
      llvm::errs() << "CLEARING INFO FLOW\n";
      // clear the info flow to the destif
      for(auto destif : destifs)
         clearInfoFlowSummaryPerState(state, destif);
   }
}

int getReturnIndex(Function *f) {
   unsigned int argi = 0;
   for(llvm::Function::arg_iterator ai = f->arg_begin(); ai != f->arg_end(); ai++, argi++);
   return argi;
}

void Executor::updateInfoFlowForReturn(ExecutionState &state, int regIndex, KInstruction *ki) {
   llvm::errs() << "Info Flow for RETURN\n";
   if (!infoFlowSummaryMode) return;
   if (regIndex < 0) return; 
   if (ki->inst->getNumOperands() > 0) {
      int source = isInfoFlowRelevant(ki, regIndex);
      if (source >=0 || currentInfoFlowContext.localIFlow->find(regIndex) != currentInfoFlowContext.localIFlow->end()) {
         Type *rt = ki->inst->getOperand(0)->getType(); 
         if (source >=0) {
             InfoFlowRegion_t  destif = createInfoFlowRegion(getReturnIndex(infoFlowSummarizedFunc), 
                                                       0, getWidthForLLVMType(rt));
             InfoFlowRegion_t sourceif = createInfoFlowRegion(source, 0, getWidthForLLVMType(rt));  
             recordInfoFlow(state, destif, sourceif);
         }
         else {
            std::map<region, std::set<InfoFlowRegion_t> > ifrmap = (*currentInfoFlowContext.localIFlow)[regIndex];
            for(auto me : ifrmap) {
               InfoFlowRegion_t destif = createInfoFlowRegion(getReturnIndex(infoFlowSummarizedFunc), 
                                                       me.first.offset, me.first.size);
               for(auto mee : me.second) {
                  recordInfoFlow(state, destif, mee);
               }
            }     
        } 
      }  
   }
}


// Assume that regIndex refers to a valid local variable and ki has a valid dest
void Executor::updateInfoFlowDirectCopy(ExecutionState &state, int regIndex, KInstruction *ki) {
   Type *t = ki->inst->getType(); 
   region rt;
   rt.offset = 0;
   rt.size = getWidthForLLVMType(t);
   int destIndex = ki->dest; 
   if (currentInfoFlowContext.localIFlow->find(regIndex) != currentInfoFlowContext.localIFlow->end()) {
      std::map<region, std::set<InfoFlowRegion_t> > ifrmap, ifrmaps;
      // should not happen due to SSA naming..
      if (currentInfoFlowContext.localIFlow->find(destIndex) != currentInfoFlowContext.localIFlow->end())
         ifrmap = (*currentInfoFlowContext.localIFlow)[destIndex];
      ifrmaps = slice((*currentInfoFlowContext.localIFlow)[regIndex], rt);
      UpdateIFlow(ifrmap, ifrmaps, 0, true);
      (*currentInfoFlowContext.localIFlow)[destIndex] = ifrmap;
   }
}


void Executor::updateInfoFlowForPhi(ExecutionState &state, int regIndex, KInstruction *ki) {
   if (!infoFlowSummaryMode) return;
   if (regIndex < 0) return; 
   updateInfoFlowDirectCopy(state, regIndex, ki);  
}

void Executor::updateInfoFlowForBinary(ExecutionState &state, int sourceIndex1, int sourceIndex2, KInstruction *ki) {
   if (!infoFlowSummaryMode) return;
   if (sourceIndex1 < 0 && sourceIndex2 < 0) return; 
   Type *t = ki->inst->getType(); 
   region rs;
   rs.offset = 0;
   rs.size = getWidthForLLVMType(t);
   region rt;
   rt.offset = 0;
   rt.size = rs.size;
   int destIndex = ki->dest; 
   std::map<region, std::set<InfoFlowRegion_t> > ifrmap, ifrmaps;
   if (sourceIndex1 >= 0 && currentInfoFlowContext.localIFlow->find(sourceIndex1) != currentInfoFlowContext.localIFlow->end()) {
      if (currentInfoFlowContext.localIFlow->find(destIndex) != currentInfoFlowContext.localIFlow->end())
         ifrmap = (*currentInfoFlowContext.localIFlow)[destIndex];
      ifrmaps = slice((*currentInfoFlowContext.localIFlow)[sourceIndex1], rs);
      UpdateIFlow(ifrmap, ifrmaps, 0, true);
      (*currentInfoFlowContext.localIFlow)[destIndex] = ifrmap; 
   }
   if (sourceIndex2 >= 0 && currentInfoFlowContext.localIFlow->find(sourceIndex2) != 
                                                    currentInfoFlowContext.localIFlow->end()) {
      if (currentInfoFlowContext.localIFlow->find(destIndex) != currentInfoFlowContext.localIFlow->end())
         ifrmap = (*currentInfoFlowContext.localIFlow)[destIndex];
      // we are over-approximating as we assume the cond holds both ways; so we force union via the false flag
      ifrmaps = slice((*currentInfoFlowContext.localIFlow)[sourceIndex2], rs);
      UpdateIFlow(ifrmap, ifrmaps, 0, false);
      (*currentInfoFlowContext.localIFlow)[destIndex] = ifrmap;
   }
}

void Executor::updateInfoFlowForCmp(ExecutionState &state, int sourceIndex1, int sourceIndex2, KInstruction *ki) {
   if (!infoFlowSummaryMode) return;
   if (sourceIndex1 < 0 && sourceIndex2 < 0) return; 
   // we may want to record these as branch relevant info flow..
}

void Executor::updateInfoFlowForExtract(ExecutionState &state, int regIndex, KInstruction *ki, int offset, int size) {
   if (!infoFlowSummaryMode) return;
   if (regIndex < 0) return; 
   Type *t = ki->inst->getType(); 
   region rs;
   rs.offset = 0;
   rs.size = getWidthForLLVMType(t);
   region rt;
   rt.offset = 0;
   rt.size = rs.size;
   int destIndex = ki->dest; 
   if (currentInfoFlowContext.localIFlow->find(regIndex) != currentInfoFlowContext.localIFlow->end()) {
      std::map<region, std::set<InfoFlowRegion_t> > ifrmap, ifrmaps;
      region radj; radj.offset=offset; radj.size = size;
      ifrmaps = slice((*currentInfoFlowContext.localIFlow)[regIndex], radj);  
      // should not happen due to SSA naming..
      if (currentInfoFlowContext.localIFlow->find(destIndex) != currentInfoFlowContext.localIFlow->end())
         ifrmap = (*currentInfoFlowContext.localIFlow)[destIndex];
      UpdateIFlow(ifrmap, ifrmaps, 0, true);
      (*currentInfoFlowContext.localIFlow)[destIndex] = ifrmap;
   }
   
}

void Executor::updateInfoFlowForExt(ExecutionState &state, int regIndex, KInstruction *ki) {
   if (!infoFlowSummaryMode) return;
   if (regIndex < 0) return; 
   updateInfoFlowDirectCopy(state, regIndex, ki);  
}


//#endif

bool rangesIntersect(unsigned min1, unsigned max1, unsigned min2, unsigned max2);

std::string removeDotSuffix(std::string name) {
  size_t pos = name.find(".");
  if (pos != std::string::npos)  {
     if (name.length() > pos) {
        // we want to get the original name when functions copies are made based on numbering
        // and keep other suffixes, e.g., those used in intrinsics
        if (isdigit(name[pos+1]))
           return name.substr(0,name.find("."));
     }
     // unlikely but better to get rid of the single dot!
     else return name.substr(0,name.find("."));
  }
  return name;
}

bool isInputFunction(Function *function) {
    std::string fname = removeDotSuffix(function->getName());    
    llvm::errs() << "checking " << fname << " to see if an input function\n";
    return (inputFuncs.find(fname) != inputFuncs.end());
}


void timeSideChannelAnalysis(Executor *executor) {
 for(rroot : resourceTreeRootList) {
    propagate(rroot, "", executor);
    checkLeakage(rroot, executor);
 }
 std::cerr << "\n\n\n\n>>>> Listing violations :  \n";
 printviolations(executor);
}

Type *getTypeFromName(llvm::Module *module, std::string tname) {
   llvm::TypeFinder StructTypes;
   StructTypes.run(*(module), true);
   for (auto *STy : StructTypes) { 
    llvm::errs() << getTypeName(STy) << " vs " << tname << "\n";
    if (getTypeName(STy) == tname)
       return STy;
   }
   return NULL;
}

bool exprHasSymMemory(ref<Expr> expr, bool high, std::string &msg) {
   std::string expr_str;
   llvm::raw_string_ostream rso(expr_str);
   expr->print(rso);
   std::string exprs = rso.str();
   if (high) {
      for(auto stm : highMemoryRegions)
         for(auto mr : stm.second) {
            std::string estr;
            llvm::raw_string_ostream rsot(estr);
            mr.first->print(rsot);
            std::string es = rsot.str();
            if (exprs.find(es) != std::string::npos)
               return true;
            klee::ConstantExpr *CE = dyn_cast<klee::ConstantExpr>(mr.first);
            if (CE) {
               for(unsigned int o = 1; o <= leakCheckOffset; o++) {
                  //llvm::errs() << "checking  sensitive high mem " << 
                  // CE->getZExtValue() << " by offset " << o << " in " << exprs << "\n"; 
                  if (exprs.find(std::to_string(CE->getZExtValue() + o)) 
                             != std::string::npos) {
                     msg = "off by " + std::to_string(o) +
                           " bytes from high sensitive address " 
                           + std::to_string(CE->getZExtValue()) + "\n";
                     return true;  
                  }
                  //llvm::errs() << "checking  sensitive high mem " << 
                  // CE->getZExtValue() << " by offset " << o << " in " << exprs << "\n"; 
                  if (exprs.find(std::to_string(CE->getZExtValue() + o)) 
                             != std::string::npos) {
                     msg = "off by " + std::to_string(o) +
                           " bytes from high sensitive address " 
                           + std::to_string(CE->getZExtValue()) + "\n";
                     return true;  
                  }
                  //llvm::errs() << "not in " << exprs << "\n";
               }
            }
         }
   }
   return false;
}

bool exprHasSymRegionFast(ref<Expr> expr, bool high, std::string &msg) {
   std::string expr_str;
   llvm::raw_string_ostream rso(expr_str);
   expr->print(rso);
   std::string exprs = rso.str();
   if (high) {
      for(auto sr : highSymRegions) {
         if (exprs.find(sr.first) != std::string::npos)
            return true;
      }
      return exprHasSymMemory(expr, high, msg);  
   }
   else {
      for(auto sr : lowSymRegions) {
         if (exprs.find(sr.first) != std::string::npos)
            return true;
      }
      for(auto stm : lowMemoryRegions)
         for(auto mr : stm.second) {
            std::string estr;
            llvm::raw_string_ostream rsot(estr);
            mr.first->print(rsot);
            std::string es = rsot.str(); 
            if (exprs.find(es) != std::string::npos)
               return true;
      }
   }
   return false;
}


void Executor::checkHighSensitiveLocals(ExecutionState &state, Instruction *ii) {
  StackFrame &sf = state.stack.back();
  std::string fname = sf.kf->function->getName();
  //if (stackLeakToBeChecked.find(fname) == stackLeakToBeChecked.end())
  //   return;
  const InstructionInfo &ii_info = kmodule->infos->getInfo(ii);
  llvm::errs() << "checking leak in " << sf.kf->function->getName() << " with " << sf.allocas.size() << "locals \n";
  unsigned int localVar = 0; 
  for (std::vector<const MemoryObject*>::iterator it = sf.allocas.begin(), 
         ie = sf.allocas.end(); it != ie; ++it, localVar++) {
      if ((sf.kf->function->getReturnType()->isVoidTy() && 
          localVar < sf.kf->function->arg_size()) ||
          (!sf.kf->function->getReturnType()->isVoidTy() && 
          localVar <= sf.kf->function->arg_size())) 
          continue;
      //llvm::errs() << "base of alloca: " << (*it)->name << " " << (*it)->getBaseExpr() << " num bytes=" << (*it)->size << "\n";
      const ObjectState *os = state.addressSpace.findObject((*it));
      ref<Expr> value = os->read(ConstantExpr::alloc(0, Expr::Int64),(*it)->size*8);
      ref<Expr> &test = value;
      if (test.isNull()) continue;
      std::string info = "";
      //llvm::errs() << "checking leak in " << value << " for local variable " << localVar << "\n";
      if (checkLeaksPreciseAndFocused) {
         std::string cfname = removeDotSuffix(fname);
         if (stackLeakToBeChecked.find(cfname) != stackLeakToBeChecked.end() && 
             (exprHasSymRegion(state, value, true) || 
              exprHasSymMemory(value, true, info))) {
            std::stringstream ss;
            ss << ii_info.file.c_str() << " " << ii_info.line << ": \n\n";
            codeLocHighSecurityLeaksOnStack.insert(ss.str());
            std::stringstream ss2;
            ss2 << ii_info.file.c_str() << " " << ii_info.line << ": \n\n";
            ss2 << value << "\n\n";
            highSecurityLeaksOnStack.insert(ss2.str());
            llvm::errs() << "Sensitive leak via local variable " << localVar << " in function " << fname << " at address " << (*it)->getBaseExpr() << "\n"; 
         }
      }
      else if (exprHasSymRegionFast(value, true, info)) {
            std::stringstream ss;
            ss << ii_info.file.c_str() << " " << ii_info.line << ": \n\n";
            codeLocHighSecurityLeaksOnStack.insert(ss.str());
            std::stringstream ss2;
            ss2 << ii_info.file.c_str() << " " << ii_info.line << ": \n\n";
            ss2 << value << " " << info << "\n\n";
            highSecurityLeaksOnStack.insert(ss2.str());
            llvm::errs() << "Sensitive leak via local variable " << localVar << " in function " << fname << " at address " << (*it)->getBaseExpr() << "\n"; 
      } 
   }
   /*Cell *cells = state.stack.back().locals; 
   KFunction *kf = state.stack.back().kf;
   const InstructionInfo &ii_info = kmodule->infos->getInfo(ii);
   for(unsigned int i=0; i<kf->numRegisters; i++) {
      ref<Expr> &c = cells[i].value;
      if (!c.isNull()) {
         if (exprHasSymRegion(c, true)) {
            std::stringstream ss;
            ss << ii_info.file.c_str() << " " << ii_info.line << ": ";
            ss << c << "\n";
            highSecurityLeaksOnStack.insert(ss.str());
         }
     }
   }*/
   llvm::errs() << "checking done\n";
}

unsigned int Executor::getTimingCost(ExecutionState &state, Instruction *inst) {
  
  if (inst->getOpcode() == Instruction::Call || inst->getOpcode() == Instruction::Invoke) {
     CallSite cs(inst);
     Value *fp = cs.getCalledValue();
     Function *f = getTargetFunction(fp, state);
     if (f) {
        std::string fname = f->getName();
        std::string cname = removeDotSuffix(fname);
        return getTimingModel(cname); 
     }
     else return ICost; 
  } 
  else return ICost;
}

void Executor::checkAndRecordSensitiveFlow(ExecutionState &state, Function *function, 
                                                   std::vector<ref<Expr> > & args) {
     std::string fname = removeDotSuffix(function->getName().str()); 
     bool found = false;
     for(unsigned int i=0; i<untrusted->size(); i++) 
        if ((*untrusted)[i] == fname) {
           found=true;
           break;
        }
     if (!found) return;
     std::vector<unsigned int> highIndices;
     unsigned int sig = 0;
     unsigned int a = 0;
     for(llvm::Function::arg_iterator agi = function->arg_begin(); agi != function->arg_end(); agi++, a++) {
        Type *at = agi->getType();
        if (at->isPointerTy()) {
           Type *bt = at->getPointerElementType();
           if (funcArgTypeHints.find(fname) != funcArgTypeHints.end())
              if (funcArgTypeHints[fname].find(a) != funcArgTypeHints[fname].end()) {
                 std::string btname= funcArgTypeHints[fname][a];
                 bt = getTypeFromName(kmodule->module, btname);
              }
           ObjectPair op;
           bool asuccess; 
           ref<Expr> base = args[a];
           solver->setTimeout(coreSolverTimeout);
           if (!state.addressSpace.resolveOne(state, solver, base, op, asuccess)) {
              base = toConstant(state, base, "resolveOne failure");
              asuccess = state.addressSpace.resolveOne(cast<ConstantExpr>(base), op);
           }
           solver->setTimeout(0);
           if (asuccess) {
              ref<Expr> result = op.second->read(ConstantExpr::alloc(0, Expr::Int32), getWidthForLLVMType(bt));
              llvm::errs() << "checking arg " << a << " of " <<  fname << " : " << result << " for sensitive flow\n"; 
              if (exprHasSymRegion(state, result, true)) {
                 llvm::errs() << " high region flows into " << fname << "\n";
                 highIndices.push_back(a);
                 sig += pow(2,a);
              }
           }
        }
        else {
           if (exprHasSymRegion(state, args[a], true)) {
              llvm::errs() << " high region flows into " << fname << "\n";
              highIndices.push_back(a);
              sig += pow(2,a);
           }
        }
     }
     if (highIndices.size() > 0) {          
        unsigned int index = 0;
        std::set<unsigned int> sigs;
        std::map<unsigned int, std::vector<unsigned int> > fmap; 
        if (externalFuncsWithSensitiveFlow.find(fname) != externalFuncsWithSensitiveFlow.end()) {
           sigs = externalFuncsSig[fname];
           if (sigs.find(sig) == sigs.end()) { 
              fmap = externalFuncsWithSensitiveFlow[fname];  
              index = externalFuncs[fname];
              externalFuncs[fname] = index + 1; 
           }
           else return;
        }
        else externalFuncs[fname] = 1;       
        sigs.insert(sig);
        externalFuncsSig[fname] = sigs;
        fmap[index] = highIndices; 
        externalFuncsWithSensitiveFlow[fname] = fmap;
     }
}

std::string getUniqueSymRegion(std::string name) {
  unsigned id;
  std::string result;
  if (uniqueSym.find(name) == uniqueSym.end()) 
     id = 0;
  else id = uniqueSym[name];
  result = name + "_" + llvm::utostr(id++);
  uniqueSym[name] = id;
  return result;   
}

void clearRegion(region r, std::vector<region> source, std::vector<region> &target) {
   for(unsigned int i=0; i<source.size(); i++) {
      if (r.offset == source[i].offset && r.size == source[i].size) {
         // clear as it is!
         continue;
      }
      else {
        unsigned int min1 = r.offset;
        unsigned int max1 = r.offset + r.size - 1;
        unsigned int size1 = r.size;
        unsigned int min2 = source[i].offset;
        unsigned int max2 = source[i].offset + source[i].size - 1;
        unsigned int size2 = source[i].size;
        if (rangesIntersect(min1,max1,min2,max2)) {
           if (min1 + size1 == min2 + size2) {
              region r1;
              r1.offset = min1 < min2 ? min1 : min2;
              r1.size = min1 < min2 ? size1 - size2 : size2 - size1;  
              target.push_back(r1);
           }
           else if (min1 == min2) {
              region r1;
              r1.offset = size1 > size2 ? min2 + size2 : min1 + size1;
              r1.size = size1 > size2 ? size1 - size2 : size2 - size1; 
              target.push_back(r1);
           } 
           else {
              region r1, r2;
              r1.offset = min1 < min2 ? min1 : min2;
              r2.offset = max1 < max2 ? max1 + 1 : max2 + 1;
              r1.size = min1 > min2 ? min1 - min2 : min2 - min1;
              r2.size = max1 > max2 ? max1 - max2 : max2 - max1;
              target.push_back(r1);
              target.push_back(r2);        
           }
        }
        else {
           // preserve as it is 
           target.push_back(source[i]);
        }
      }
   } 
}

void clearRegion(std::vector<region> rs, std::vector<region> source, std::vector<region> &target) {
   std::vector<region> temp = source;
   for(unsigned int i=0; i<rs.size(); i++) { 
      std::vector<region>  result;
      clearRegion(rs[i], temp, result);
      temp = result;
      if (result.size() == 0) {
         break;
      }
   }
   target = temp;
}

void clearMemoryRegion(ExecutionState &state, ref<Expr> a, std::vector<region> rs, bool high) {
   long saddr = (long)&state;
   std::map<ref<Expr>, std::vector<region> > m;
   if (high && highMemoryRegions.find(saddr) != highMemoryRegions.end()) {
      m = highMemoryRegions[saddr];
   }
   else if (!high && lowMemoryRegions.find(saddr) != lowMemoryRegions.end()) {
     m = lowMemoryRegions[saddr];
   }
   else return;
   if (m.find(a) != m.end()) {
      std::vector<region> temp;
      clearRegion(rs, m[a], temp);
      if (temp.size() == 0) {
         m.erase(a);
         llvm::errs() << "clearing high=" << high << " status of " << a << "\n";
      }
      else m[a] = temp;
      if (high) 
         highMemoryRegions[saddr] = m;
     else lowMemoryRegions[saddr] = m;
  }
}

void clearSymRegion(std::string name, std::vector<region> rs, bool high) {
  if (high && highSymRegions.find(name) != highSymRegions.end()) {
     std::vector<region> cur = highSymRegions[name];
     std::vector<region> temp;
     clearRegion(rs, cur, temp);
      if (temp.size() == 0) {
         highSymRegions.erase(name);
         llvm::errs() << "clearing high=" << high << " status of " << name << "\n";
      }
      highSymRegions[name] = temp; 
  }
  else if (!high && lowSymRegions.find(name) != lowSymRegions.end()) {
     std::vector<region> cur = lowSymRegions[name];
     std::vector<region> temp;
     clearRegion(rs, cur, temp);
      if (temp.size() == 0) {
         lowSymRegions.erase(name);
         llvm::errs() << "clearing high=" << high << " status of " << name << "\n";
      }
      lowSymRegions[name] = temp; 
  }
}


void setHighMemoryRegion(ExecutionState &state, ref<Expr> a, std::vector<region> rs) {
   long saddr = (long)&state;
   std::map<ref<Expr>, std::vector<region> > m;
   if (highMemoryRegions.find(saddr) != highMemoryRegions.end())
      m = highMemoryRegions[saddr];
   //if (m.find(a) != m.end())
     // assert(false && "address already has high security memory region set!\n");
   m[a] = rs;
   highMemoryRegions[saddr] = m; 
   clearMemoryRegion(state, a, rs, false);
}

void setLowMemoryRegion(ExecutionState &state, ref<Expr> a, std::vector<region> rs) {
   long saddr = (long)&state;
   std::map<ref<Expr>, std::vector<region> > m;
   if (lowMemoryRegions.find(saddr) != lowMemoryRegions.end())
      m = lowMemoryRegions[saddr];
   //if (m.find(a) != m.end())
     // assert(false && "address already has low security memory region set!\n");
   m[a] = rs;
   lowMemoryRegions[saddr] = m; 
   clearMemoryRegion(state, a, rs, true);
}

void addHighMemoryRegion(ExecutionState &state, ref<Expr> a, region r) {
   long saddr = (long)&state;
   std::map<ref<Expr>, std::vector<region> > m;
   if (highMemoryRegions.find(saddr) != highMemoryRegions.end())
      m = highMemoryRegions[saddr];
   std::vector<region> rs;
   if (m.find(a)!= m.end())
      rs = m[a];
   rs.push_back(r);
   m[a] = rs;     
   highMemoryRegions[saddr] = m;
}

void addLowMemoryRegion(ExecutionState &state, ref<Expr> a, region r) {
   long saddr = (long)&state;
   std::map<ref<Expr>, std::vector<region> > m;
   if (lowMemoryRegions.find(saddr) != lowMemoryRegions.end())
      m = lowMemoryRegions[saddr];
   std::vector<region> rs;
   if (m.find(a)!= m.end())
      rs = m[a];
   rs.push_back(r);
   m[a] = rs;     
   lowMemoryRegions[saddr] = m;
}

void setHighSymRegion(std::string name, std::vector<region> rs) {
  /*if (highSymRegions.find(name) != highSymRegions.end()) {
     for(unsigned int i=0; i < rs.size(); i++) {
        llvm::errs() << "[" << rs[i].offset << "," << rs[i].size << "]\n"; 
     }
     llvm::errs() << "setting high sym region for " << name << "failed!\n";
     std::vector<region> ers = highSymRegions[name];
     for(unsigned int i=0; i < ers.size(); i++) {
        llvm::errs() << "[" << ers[i].offset << "," << ers[i].size << "]\n"; 
     }
     assert(false && "duplicate entry for highsymregions");
  }*/
  highSymRegions[name] = rs;
  clearSymRegion(name, rs, false);
  llvm::errs() << "setting symbolic region high: " << name << "\n";
}

void setLowSymRegion(std::string name, std::vector<region> rs) {
  /*if (lowSymRegions.find(name) != lowSymRegions.end()) {
     llvm::errs() << "setting low sym region for " << name << "failed!\n";
     assert(false && "duplicate entry for lowsymregions");
  }*/
  lowSymRegions[name] = rs; 
  llvm::errs() << "clearing " << name << " from high\n";
  clearSymRegion(name, rs, true);
}

void initHighLowRegions(ExecutionState &state) {
   long saddr = (long)&state;
   std::map<ref<Expr>, std::vector<region> > empty;
   highMemoryRegions[saddr] = empty;
   lowMemoryRegions[saddr] = empty;
}

// Check intersection of [min1, max1] and [min2, max2]
bool rangesIntersect(unsigned min1, unsigned max1, unsigned min2, unsigned max2) {
  if ((min1 > max2) || (min2 > max1))
     return false;
  return true; 
}

bool Executor::checkSymInRegion(ExecutionState &state, region r, ref<Expr> offset, Expr::Width offsetType, Expr::Width type, bool computed)  {
    ref<Expr> rbase = ConstantExpr::alloc(r.offset, offset->getWidth());
    ref<Expr> rend =  ConstantExpr::alloc(r.offset + r.size - 1, offset->getWidth());
    ref<Expr> o1 = offset;
    if (!computed)
       o1 = MulExpr::create(offset, ConstantExpr::alloc(offsetType, offset->getWidth()));
    else o1 = MulExpr::create(offset, ConstantExpr::alloc(8, offset->getWidth()));
    ref<Expr> end = AddExpr::create(o1, ConstantExpr::alloc(type - 1, offset->getWidth())); 
    ref<Expr> case1 = UgtExpr::create(rbase, end);
    ref<Expr> case2 = UgtExpr::create(o1, rend);

    bool res = false;    
    double timeout = coreSolverTimeout;
    solver->setTimeout(timeout);
    bool success = solver->mayBeTrue(state, case1, res);
    solver->setTimeout(0);
    if (success && res) {
       //llvm::errs() << "case 1 of not intersecting holds: " << case1  << "\n";
       return false;
    }
    res = false;
    solver->setTimeout(timeout);
    success = solver->mayBeTrue(state, case2, res);
    solver->setTimeout(0);
    if (success && res) {
       //llvm::errs() << "case 2 of not intersecting holds: " << case2  << "\n";
       return false;
    }
    //llvm::errs() << "symbolic intersect holds!\n";
    return true;
}

// offset must already be computed as the byte offset
bool Executor::isInRegion(ExecutionState &state, std::vector<region> rs, ref<Expr> offset, Expr::Width offsetType, Expr::Width type, bool computed) {
  if (klee::ConstantExpr *CE = dyn_cast<klee::ConstantExpr>(offset)) {
     uint64_t value = CE->getZExtValue();     
     uint64_t base;
     if (computed) 
        base = value*8;
     else base = value*offsetType;
     for(unsigned int i=0; i < rs.size(); i++) {
        
        //llvm::errs() << "do ranges intersect: " << rs[i].offset << "," << rs[i].offset + rs[i].size-1 << " AND " << 
          //              base << "," << base + type-1 << "\n";        
        if (rangesIntersect(rs[i].offset, rs[i].offset + rs[i].size-1, base, base + type-1)) {
           //llvm::errs() << "ranges intersect: " << rs[i].offset << "," << rs[i].offset + rs[i].size-1 << " AND " << 
             //            base << "," << base + type-1 << "\n";
           return true; 
        }
     }
  }
  else {
     for(unsigned int i=0; i < rs.size(); i++) {
        if (checkSymInRegion(state, rs[i], offset, offsetType, type, computed)) {
           return true;
        } 
     }
    llvm::errs() << "Symbolic offset do not fall into the security sensitive region!\n";
  }
  return false;
}

bool isASymRegion(std::string symname, bool high) {
   if (high)
      return (highSymRegions.find(symname) != highSymRegions.end()); 
   else 
      return (lowSymRegions.find(symname) != lowSymRegions.end()); 
}

bool Executor::isInSymRegion(ExecutionState &state, std::string symname, ref<Expr> offset, Expr::Width offsetType, Expr::Width type, bool computed,  
      bool high) {
  //llvm::errs() << "Checking if symbolic region " << symname << " offset=" << offset << " size=" << type << " is (high?) " << high << "\n";  
  if (high) {
     if (highSymRegions.find(symname) == highSymRegions.end()) 
        return false;
     std::vector<region> rs = highSymRegions[symname];    
     return isInRegion(state, rs, offset, offsetType, type, computed);
  }
  else {
     if (lowSymRegions.find(symname) == lowSymRegions.end()) 
        return false;
     std::vector<region> rs = lowSymRegions[symname];
     return isInRegion(state, rs, offset, offsetType, type, computed);
  }
}

bool Executor::isInHighMemoryRegion(ExecutionState &state, ref<Expr> baseAddress, ref<Expr> offset, Expr::Width offsetType, Expr::Width type, bool computed) {
  long sa = (long)&state;
  if (highMemoryRegions.find(sa) == highMemoryRegions.end()) {
     assert(false && "State does not have high regions!!!");
  }
  std::map<ref<Expr>, std::vector<region> > m = highMemoryRegions[sa];
  if (m.find(baseAddress) == m.end()) {
     llvm::errs() << "base address not marked as security sensitive\n";
     return false;
  }
  else {
     std::vector<region> rs = m[baseAddress];
     llvm::errs() << "checking if is in high mem region: \n";
     return isInRegion(state, rs, offset, offsetType, type, computed);
  }
}

void printSymRegions() {
  llvm::errs() << "High symbolic regions:\n";
  for(auto  k : highSymRegions)
     llvm::errs() << k.first << "\n";
  llvm::errs() << "Low symbolic regions:\n";    
  for(auto  k : lowSymRegions)
     llvm::errs() << k.first << "\n";
}

bool Executor::isInLowMemoryRegion(ExecutionState &state, ref<Expr> baseAddress, ref<Expr> offset, Expr::Width offsetType, Expr::Width type, bool computed) {
  long sa = (long)&state;
  if (lowMemoryRegions.find(sa) == lowMemoryRegions.end()) {
     assert(false && "State does not have low regions!!!");
  }
  std::map<ref<Expr>, std::vector<region> > m = lowMemoryRegions[sa];
  if (m.find(baseAddress) == m.end())
     return false;
  else {
     std::vector<region> rs = m[baseAddress];
     return isInRegion(state, rs, offset, offsetType, type, computed);
  }
}

void cloneHighMemoryRegions(const ExecutionState &from, ExecutionState &to) {
   long faddr = (long)&from;
   long taddr = (long)&to;
   if (highMemoryRegions.find(faddr) == highMemoryRegions.end())
      assert(0 && "highRegion set not initialized for state!");
   highMemoryRegions[taddr] = highMemoryRegions[faddr];
}

void cloneLowMemoryRegions(const ExecutionState &from, ExecutionState &to) {
   long faddr = (long)&from;
   long taddr = (long)&to;
   if (lowMemoryRegions.find(faddr) == lowMemoryRegions.end())
      assert(0 && "lowAddress set not initialized for state!");
   lowMemoryRegions[taddr] = lowMemoryRegions[faddr];
}

void Executor::checkAndUpdateInfoFlow(ExecutionState &state, Function *f, std::vector<ref<Expr> > & args, 
          const MemoryObject *mo) {
   std::string fname = removeDotSuffix(f->getName()); 
   std::vector<ref<Expr> > argsValues;
   getArgValue(state, f, args, argsValues);
   if (argsValues.size() == 0) {
      llvm::errs() << "no relevant args to check for info flow for " << fname << "\n";   
      return;
   }
   std::vector<region> rs = getHighInfoFlowRegions(state, fname, argsValues);
   //llvm::errs() << "will info flow into " << mo->name << "\n";
   if (rs.size() > 0) {
      //llvm::errs() << "yes!\n";
      for(unsigned int i=0; i<rs.size(); i++)
         llvm::errs() << rs[i].offset << "," << rs[i].size << "\n";
      setHighMemoryRegion(state, mo->getBaseExpr(), rs);
      llvm::errs() << "calling setHighSym from checkAndUpdateInfoFlow for " << fname << "\n"; 
      setHighSymRegion(mo->name, rs);
   }
}


// trim from left
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

std::string getTypeName(Type *t) {
   std::string type_str;
   llvm::raw_string_ostream rso(type_str);
   t->print(rso);
   std::istringstream iss(rso.str());
   std::string typestr;
   getline(iss, typestr, ' ');
   return typestr;
}

void computeMaxVoidTypeCastSize() {
  maxVoidTypeCastSize = 8;
  DataLayout *targetData = kmoduleExt->targetData;
  llvm::TypeFinder StructTypes;
  StructTypes.run(*(kmoduleExt->module), true);
  for(auto vct : voidTypeCasts) {
     for (auto *STy : StructTypes) {
        std::string tname = getTypeName(STy);
        if (tname.find(vct) != std::string::npos) 
           if (targetData->getTypeStoreSize(STy) > maxVoidTypeCastSize) {
              llvm::errs() << "Storing size of " << tname << " for max void cast size: " << targetData->getTypeStoreSize(STy) << " \n";
              maxVoidTypeCastSize = targetData->getTypeStoreSize(STy);   
           } 
     }
  } 
}

void initializeLazyInit(const MemoryObject *mo, ObjectState *obj, Type *t) {
  if (!t) return;
  StructType *st = dyn_cast<StructType>(t);
  if (st) {
     std::string tname = getTypeName(t);
     std::string type_str;
     llvm::raw_string_ostream rso(type_str);
     t->print(rso);
     int pos = tname.find("struct");
     if (pos == std::string::npos) {
        //llvm::errs() << "skipping lazy initializing fields of full type name =" << rso.str() << " due to non-standard type name!!!\n";
        return;
     }
     tname = tname.substr(pos);       
     //llvm::errs() << "checking initializers for type " << tname << "!\n";
     if (lazyInitInitializersWValues.find(tname) != lazyInitInitializersWValues.end()) {
       //llvm::errs() << "initializer found!\n"; 
        std::map<unsigned int, int> offsets = lazyInitInitializersWValues[tname];
        for(auto offset : offsets) {
           ref<Expr> ve = klee::ConstantExpr::alloc(offset.second, Expr::Int32); 
           llvm::errs() << "Initializing object of lazy init type " << tname << " at offset " << offset.first << " to value " << ve << " at base address " << mo->getBaseExpr() << "\n";
           obj->write(offset.first, ve); 
        }
     }
     if (lazyInitInitializers.find(tname) != lazyInitInitializers.end()) {
       //llvm::errs() << "initializer found!\n"; 
        std::vector<unsigned int> offsets = lazyInitInitializers[tname];
        for(auto offset : offsets) {
           //llvm::errs() << "Initializing object of lazy init type " << tname << " at offset " << offset << " to a NULL pointer\n";
           obj->write(offset, Expr::createPointer(0));
        }
     }
     // initialize function pointers, if any specified
     if (lazyFuncPtrInitializers.find(tname) != lazyFuncPtrInitializers.end()) {
        std::map<unsigned int, std::string> fmap = lazyFuncPtrInitializers[tname];
        for(auto e : fmap) {
           // lookup the function 
           Function *f = moduleHandle->getFunction(e.second);
           if (!f) {
              llvm::errs() << "Unable to find function " << e.second << " in the module to init func pointer!!!\n";
              exit(1);
           }
           // assign the function address to the offset
           obj->write(e.first, Expr::createPointer((uint64_t)f));
           llvm::errs() << "assigning function " << f->getName() << "to offset " << e.first << " of base address " << mo->getBaseExpr() << "\n";
        }
     }
  }
}

bool isEmbeddedType(Type *t) {
  if (!dyn_cast<StructType>(t))
      return false;
  /*std::string type_str;
  llvm::raw_string_ostream rso(type_str);
  t->print(rso);
  std::istringstream iss(rso.str());
  std::string typestr;
  getline(iss, typestr, ' ');
  for(auto et : embeddedTypes) {
      if (et == typestr)
          return true;
  }
  return false;
   */
  return embeddedTypes.find(t) != embeddedTypes.end();
}

bool isLazySingle(Type *t, std::string pointertype="**") {
  std::string type_str;
  llvm::raw_string_ostream rso(type_str);
  t->print(rso);
  std::istringstream iss(rso.str());
  std::string typestr;
  getline(iss, typestr, ' ');
  for(auto el : lazyInitSingles)
    if (typestr.find(el) != std::string::npos) {
       return true;
    }
  return false;
}

bool isLazySingle(std::string typestr, std::string pointertype="**") {
  for(auto el : lazyInitSingles)
    if (typestr.find(el) != std::string::npos) {
       return true;
    }
  return false;
}


bool isLazyInit(Type *t, bool &single, int &count) {
  int origvalue = count;
  std::string pointertype = "**";
  count = 0;
  std::string type_str;
  llvm::raw_string_ostream rso(type_str);
  t->print(rso);
  std::istringstream iss(rso.str());
  std::string typestr;
  getline(iss, typestr, ' ');
  /*for(auto el : lazyInits)
     if (typestr.find(el+pointertype) != std::string::npos) {
        count = lazyInitNumInstances[el];
        single = isLazySingle(typestr);
        return true;
     }
  */
  for(int i=0; i<lazyInits.size(); i++)
     if (typestr.find(lazyInits[i]+pointertype) != std::string::npos) {
        count = lazyInitNumInstances[lazyInits[i]];
        single = isLazySingle(typestr);
        return true;
     }
  // Support lazy init for double pointer to a primitive type
  if (t->isPointerTy()) {
     Type *el = t->getPointerElementType();
     if (el->isPointerTy()) {
        el = el->getPointerElementType();             
        StructType *st = dyn_cast<StructType>(el);
        if (st) {
           count = STRUCT_LAZY_INIT_INS;
           single = false;
           return true;
        }
        else { // if (el->getPrimitiveSizeInBits()) {
           count = primArraySize; 
           single = false;
           return true;
        }
     }
  }
  count = origvalue;
  single = false;
  return false;
}

bool isAllocTypeLazyInit(Type *t, bool &single, int &count) {
  //std::string pointertype = "*";
  int origvalue = count;
  count = 0;
  std::string type_str;
  llvm::raw_string_ostream rso(type_str);
  t->print(rso);
  std::istringstream iss(rso.str());
  std::string typestr;
  getline(iss, typestr, ' ');
  /*for(auto el : lazyInits) {
     llvm::outs() << typestr << " vs " << el <<"\n";
     if (typestr.find(el) != std::string::npos) {
        count = lazyInitNumInstances[el];
        single = isLazySingle(typestr);
        return true;
     }
  }*/
  for(int i=0; i<lazyInits.size(); i++) {
     if (typestr.find(lazyInits[i]) != std::string::npos) {
        count = lazyInitNumInstances[lazyInits[i]];
        single = isLazySingle(typestr);
        return true;
     }
  }
  if (t->getPrimitiveSizeInBits()) {
      count = primArraySize; 
      single = false;
      return true;
  }
  else {
     count = origvalue;
     single = false;
  }
  return false;
}

void collectEmbeddedPointerTypes(Type *t, std::vector<std::string> &lazyTypes, std::set<std::string> &visited) {

     bool pointerType = false;
     if (t->isPointerTy()) {
        pointerType = true;
        while (t->isPointerTy()) {
           t = t->getPointerElementType();
        }
     }
     std::string type_str;
     llvm::raw_string_ostream rso(type_str);
     t->print(rso);
     StructType *st = dyn_cast<StructType>(t);
     if (st) {
        std::istringstream ist(rso.str());
        std::string temp;
        getline(ist, temp, '=');
        std::string ltypename = ltrim(rtrim(temp));
        if (visited.find(ltypename) == visited.end()) {
           visited.insert(ltypename);
           if (pointerType) {
              //lazyTypes.insert(ltypename);
              lazyTypes.push_back(ltypename);
              lazyInitNumInstances[ltypename] = numLazyInst;
              //llvm::errs() << "lazy type: " << ltypename << " extracted\n";
           }
           for(unsigned i=0; i < st->getNumElements(); i++) {
              /*Type *temp = st->getElementType(i);
              if (temp->isPointerTy()) {
                 temp = temp->getPointerElementType();
                 if (temp->isFunctionTy())
                    llvm::errs() << "Function pointer field " << i << " of " << getTypeName(st) << "\n";
              }*/
              collectEmbeddedPointerTypes(st->getElementType(i), lazyTypes, visited);
           }
        }
     }
     else if (t->getPrimitiveSizeInBits()) {
        if (pointerType) {
           //llvm::errs() << "lazy primitive type: " << rso.str() << " extracted\n";
           //lazyTypes.insert(rso.str());
           lazyTypes.push_back(rso.str());
           lazyInitNumInstances[rso.str()] = primArraySize ;//numLazyInst;
        }
     }

}

/* end SYSREL extension */


/* SYSREL extension */
//namespace {
/* SYSREL extension */
  cl::opt<bool>
  DumpStatesOnHalt("dump-states-on-halt",
                   cl::init(true),
		   cl::desc("Dump test cases for all active states on exit (default=on)"));

  cl::opt<bool>
  AllowExternalSymCalls("allow-external-sym-calls",
                        cl::init(false),
			cl::desc("Allow calls with symbolic arguments to external functions.  This concretizes the symbolic arguments.  (default=off)"));

  /// The different query logging solvers that can switched on/off
  enum PrintDebugInstructionsType {
    STDERR_ALL, ///
    STDERR_SRC,
    STDERR_COMPACT,
    FILE_ALL,    ///
    FILE_SRC,    ///
    FILE_COMPACT ///
  };

  llvm::cl::bits<PrintDebugInstructionsType> DebugPrintInstructions(
      "debug-print-instructions",
      llvm::cl::desc("Log instructions during execution."),
      llvm::cl::values(
          clEnumValN(STDERR_ALL, "all:stderr", "Log all instructions to stderr "
                                               "in format [src, inst_id, "
                                               "llvm_inst]"),
          clEnumValN(STDERR_SRC, "src:stderr",
                     "Log all instructions to stderr in format [src, inst_id]"),
          clEnumValN(STDERR_COMPACT, "compact:stderr",
                     "Log all instructions to stderr in format [inst_id]"),
          clEnumValN(FILE_ALL, "all:file", "Log all instructions to file "
                                           "instructions.txt in format [src, "
                                           "inst_id, llvm_inst]"),
          clEnumValN(FILE_SRC, "src:file", "Log all instructions to file "
                                           "instructions.txt in format [src, "
                                           "inst_id]"),
          clEnumValN(FILE_COMPACT, "compact:file",
                     "Log all instructions to file instructions.txt in format "
                     "[inst_id]")
          KLEE_LLVM_CL_VAL_END),
      llvm::cl::CommaSeparated);
#ifdef HAVE_ZLIB_H
  cl::opt<bool> DebugCompressInstructions(
      "debug-compress-instructions", cl::init(false),
      cl::desc("Compress the logged instructions in gzip format."));
#endif

  cl::opt<bool>
  DebugCheckForImpliedValues("debug-check-for-implied-values");


  cl::opt<bool>
  SimplifySymIndices("simplify-sym-indices",
                     cl::init(false),
		     cl::desc("Simplify symbolic accesses using equalities from other constraints (default=off)"));

  cl::opt<bool>
  EqualitySubstitution("equality-substitution",
		       cl::init(true),
		       cl::desc("Simplify equality expressions before querying the solver (default=on)."));

  cl::opt<unsigned>
  MaxSymArraySize("max-sym-array-size",
                  cl::init(0));

  cl::opt<bool>
  SuppressExternalWarnings("suppress-external-warnings",
			   cl::init(false),
			   cl::desc("Supress warnings about calling external functions."));

  cl::opt<bool>
  AllExternalWarnings("all-external-warnings",
		      cl::init(false),
		      cl::desc("Issue an warning everytime an external call is made,"
			       "as opposed to once per function (default=off)"));

  cl::opt<bool>
  OnlyOutputStatesCoveringNew("only-output-states-covering-new",
                              cl::init(false),
			      cl::desc("Only output test cases covering new code (default=off)."));

  cl::opt<bool>
  EmitAllErrors("emit-all-errors",
                cl::init(false),
                cl::desc("Generate tests cases for all errors "
                         "(default=off, i.e. one per (error,instruction) pair)"));

  cl::opt<bool>
  NoExternals("no-externals",
           cl::desc("Do not allow external function calls (default=off)"));

  cl::opt<bool>
  AlwaysOutputSeeds("always-output-seeds",
		    cl::init(true));

  cl::opt<bool>
  OnlyReplaySeeds("only-replay-seeds",
		  cl::init(false),
                  cl::desc("Discard states that do not have a seed (default=off)."));

  cl::opt<bool>
  OnlySeed("only-seed",
	   cl::init(false),
           cl::desc("Stop execution after seeding is done without doing regular search (default=off)."));

  cl::opt<bool>
  AllowSeedExtension("allow-seed-extension",
		     cl::init(false),
                     cl::desc("Allow extra (unbound) values to become symbolic during seeding (default=false)."));

  cl::opt<bool>
  ZeroSeedExtension("zero-seed-extension",
		    cl::init(false),
		    cl::desc("(default=off)"));

  cl::opt<bool>
  AllowSeedTruncation("allow-seed-truncation",
		      cl::init(false),
                      cl::desc("Allow smaller buffers than in seeds (default=off)."));

  cl::opt<bool>
  NamedSeedMatching("named-seed-matching",
		    cl::init(false),
                    cl::desc("Use names to match symbolic objects to inputs (default=off)."));

  cl::opt<double>
  MaxStaticForkPct("max-static-fork-pct",
		   cl::init(1.),
		   cl::desc("(default=1.0)"));

  cl::opt<double>
  MaxStaticSolvePct("max-static-solve-pct",
		    cl::init(1.),
		    cl::desc("(default=1.0)"));

  cl::opt<double>
  MaxStaticCPForkPct("max-static-cpfork-pct",
		     cl::init(1.),
		     cl::desc("(default=1.0)"));

  cl::opt<double>
  MaxStaticCPSolvePct("max-static-cpsolve-pct",
		      cl::init(1.),
		      cl::desc("(default=1.0)"));

  cl::opt<double>
  MaxInstructionTime("max-instruction-time",
                     cl::desc("Only allow a single instruction to take this much time (default=0s (off)). Enables --use-forked-solver"),
                     cl::init(0));

  cl::opt<double>
  SeedTime("seed-time",
           cl::desc("Amount of time to dedicate to seeds, before normal search (default=0 (off))"),
           cl::init(0));

  cl::list<Executor::TerminateReason>
  ExitOnErrorType("exit-on-error-type",
		  cl::desc("Stop execution after reaching a specified condition.  (default=off)"),
		  cl::values(
		    clEnumValN(Executor::Abort, "Abort", "The program crashed"),
		    clEnumValN(Executor::Assert, "Assert", "An assertion was hit"),
		    clEnumValN(Executor::BadVectorAccess, "BadVectorAccess", "Vector accessed out of bounds"),
		    clEnumValN(Executor::Exec, "Exec", "Trying to execute an unexpected instruction"),
		    clEnumValN(Executor::External, "External", "External objects referenced"),
		    clEnumValN(Executor::Free, "Free", "Freeing invalid memory"),
		    clEnumValN(Executor::Model, "Model", "Memory model limit hit"),
		    clEnumValN(Executor::Overflow, "Overflow", "An overflow occurred"),
		    clEnumValN(Executor::Ptr, "Ptr", "Pointer error"),
		    clEnumValN(Executor::ReadOnly, "ReadOnly", "Write to read-only memory"),
		    clEnumValN(Executor::ReportError, "ReportError", "klee_report_error called"),
		    clEnumValN(Executor::User, "User", "Wrong klee_* functions invocation"),
		    clEnumValN(Executor::Unhandled, "Unhandled", "Unhandled instruction hit")
		    KLEE_LLVM_CL_VAL_END),
		  cl::ZeroOrMore);

  cl::opt<unsigned long long>
  StopAfterNInstructions("stop-after-n-instructions",
                         cl::desc("Stop execution after specified number of instructions (default=0 (off))"),
                         cl::init(0));

  cl::opt<unsigned>
  MaxForks("max-forks",
           cl::desc("Only fork this many times (default=-1 (off))"),
           cl::init(~0u));

  cl::opt<unsigned>
  MaxDepth("max-depth",
           cl::desc("Only allow this many symbolic branches (default=0 (off))"),
           cl::init(0));

  cl::opt<unsigned>
  MaxMemory("max-memory",
            cl::desc("Refuse to fork when above this amount of memory (in MB, default=2000)"),
            cl::init(2000));

  cl::opt<bool>
  MaxMemoryInhibit("max-memory-inhibit",
            cl::desc("Inhibit forking at memory cap (vs. random terminate) (default=on)"),
            cl::init(true));
/* SYSREL extension */
//}
/* SYSREL extension */

namespace klee {
  RNG theRNG;
}

const char *Executor::TerminateReasonNames[] = {
  [ Abort ] = "abort",
  [ Assert ] = "assert",
  [ BadVectorAccess ] = "bad_vector_access",
  [ Exec ] = "exec",
  [ External ] = "external",
  [ Free ] = "free",
  [ Model ] = "model",
  [ Overflow ] = "overflow",
  [ Ptr ] = "ptr",
  [ ReadOnly ] = "readonly",
  [ ReportError ] = "reporterror",
  [ User ] = "user",
  [ Unhandled ] = "xxx",
  [ NegativeRefcount ] = "negrefcount",
};

/* SYSREL extension */

/*
void initHighLowAddress(ExecutionState &state) {
   long saddr = (long)&state;
   std::set<ref<Expr> > empty;
   highAddresses[saddr] = empty;
   lowAddresses[saddr] = empty;
}


bool isAHighAddress(ExecutionState &state, ref<Expr> addr) {
   long saddr = (long)&state;
   if (highAddresses.find(saddr) == highAddresses.end())
      assert(0 && "highAddress set not initialized for state!");
   std::set<ref<Expr> > addrs = highAddresses[saddr];
   return addrs.find(addr) !=  addrs.end();
}
  
bool isALowAddress(ExecutionState &state, ref<Expr> addr) {
   long laddr = (long)&state; 
   if (lowAddresses.find(laddr) == lowAddresses.end())
      assert(0 && "lowAddress set not initialized for state!");
   std::set<ref<Expr> > addrs = lowAddresses[laddr];
   return addrs.find(addr) !=  addrs.end();
}

void addHighAddress(ExecutionState &state, ref<Expr> addr) {
   long haddr = (long)&state;
   if (highAddresses.find(haddr) == highAddresses.end())
      assert(0 && "highAddress set not initialized for state!");
   std::set<ref<Expr> > addrs = highAddresses[haddr];
   addrs.insert(addr);
   highAddresses[haddr] = addrs;
}

void addLowAddress(ExecutionState &state, ref<Expr> addr) {
   long laddr = (long)&state;
   if (lowAddresses.find(laddr) == lowAddresses.end())
      assert(0 && "lowAddress set not initialized for state!");
   std::set<ref<Expr> > addrs = lowAddresses[laddr];
   addrs.insert(addr);
   lowAddresses[laddr] = addrs;
}

void cloneHighAddresses(const ExecutionState &from, ExecutionState &to) {
   long faddr = (long)&from;
   long taddr = (long)&to;
   if (highAddresses.find(faddr) == highAddresses.end())
      assert(0 && "highAddress set not initialized for state!");
   highAddresses[taddr] = highAddresses[faddr];
}

void cloneLowAddresses(const ExecutionState &from, ExecutionState &to) {
   long faddr = (long)&from;
   long taddr = (long)&to;
   if (lowAddresses.find(faddr) == lowAddresses.end())
      assert(0 && "lowAddress set not initialized for state!");
   lowAddresses[taddr] = lowAddresses[faddr];
}

*/

bool mayBeUnitializedSymbolic(const ObjectState *obj, unsigned width) {
   for (int b = 0; b<width/8; b++) {
       if (!obj->isByteConcrete(b) && !obj->isByteKnownSymbolic(b)
                   && !obj->isByteFlushed(b))
          return true;
   }
   return false;
}

// for pointer type args use the expressions pointed by them
void Executor::getArgValue(ExecutionState &state, Function *function, 
                 std::vector<ref<Expr> > &arguments, 
                 std::vector<ref<Expr> > &argsRes) {
   unsigned int ai = 0;
   for(llvm::Function::arg_iterator agi = function->arg_begin(); agi != function->arg_end(); agi++, ai++) {
      Type *at = agi->getType();
      if (at->isPointerTy()) {
         Type *bt = at->getPointerElementType();
         DataLayout *TD = kmodule->targetData;
         ObjectPair op;
         bool asuccess;
         ref<Expr> base = arguments[ai];
         if (SimplifySymIndices) {
            if (!isa<ConstantExpr>(base))
               base = state.constraints.simplifyExpr(base);
         }
         ConstantExpr *cexpr = dyn_cast<ConstantExpr>(base);
         if (cexpr) {
            solver->setTimeout(coreSolverTimeout);
            if (!state.addressSpace.resolveOne(state, solver, base, op, asuccess)) {
               base = toConstant(state, base, "resolveOne failure");
               asuccess = state.addressSpace.resolveOne(cast<ConstantExpr>(base), op);
            }
            solver->setTimeout(0);
            if (asuccess) {
               if (mayBeUnitializedSymbolic(op.second,getWidthForLLVMType(bt))) { 
                  // symbolic content without any initialization..
                  ref<Expr> result = ConstantExpr::alloc(0, Expr::Int64);
                  argsRes.push_back(result);
               }
               else {
                  llvm::errs() << op.second->isByteConcrete(0) << " " 
                               << op.second->isByteKnownSymbolic(0) << " "
                               << op.second->isByteFlushed(0) << "\n";
                  llvm::errs() << "getArgValue " << ai  << "\n";
                  ref<Expr> result = op.second->read(ConstantExpr::alloc(0, Expr::Int64), getWidthForLLVMType(bt));
                  argsRes.push_back(result);
               }
            }
        }
     }
     else {
        argsRes.push_back(arguments[ai]);
     }
  }
}


bool isAsyncInitiate(std::string name) {
  return name.find(ASYNC_STR) == 0;
}

bool isEnableEntry(std::string name) {
  return name.find(ENABLE_STR) == 0;
}

bool isAsync(std::string name) {
  for(unsigned int i=0; i < asyncFunc.size(); i++)
    if (asyncFunc[i] == name)
      return true;
  return false;
}

bool isEnabled(std::string name) {
  for(unsigned int i=0; i < enabledFunc.size(); i++)
    if (enabledFunc[i] == name)
      return true;
  return false;
}
/* SYSREL extension */


Executor::Executor(LLVMContext &ctx, const InterpreterOptions &opts,
    InterpreterHandler *ih)
    : Interpreter(opts), kmodule(0), interpreterHandler(ih), searcher(0),
      externalDispatcher(new ExternalDispatcher(ctx)), statsTracker(0),
      pathWriter(0), symPathWriter(0), specialFunctionHandler(0),
      processTree(0), replayKTest(0), replayPath(0), usingSeeds(0),
      atMemoryLimit(false), inhibitForking(false), haltExecution(false),
      ivcEnabled(false),
      coreSolverTimeout(MaxCoreSolverTime != 0 && MaxInstructionTime != 0
                            ? std::min(MaxCoreSolverTime, MaxInstructionTime)
                            : std::max(MaxCoreSolverTime, MaxInstructionTime)),
      debugInstFile(0), debugLogBuffer(debugBufferString) {

  if (coreSolverTimeout) UseForkedCoreSolver = true;
  Solver *coreSolver = klee::createCoreSolver(CoreSolverToUse);
  if (!coreSolver) {
    klee_error("Failed to create core solver\n");
  }

  Solver *solver = constructSolverChain(
      coreSolver,
      interpreterHandler->getOutputFilename(ALL_QUERIES_SMT2_FILE_NAME),
      interpreterHandler->getOutputFilename(SOLVER_QUERIES_SMT2_FILE_NAME),
      interpreterHandler->getOutputFilename(ALL_QUERIES_KQUERY_FILE_NAME),
      interpreterHandler->getOutputFilename(SOLVER_QUERIES_KQUERY_FILE_NAME));

  this->solver = new TimingSolver(solver, EqualitySubstitution);
  memory = new MemoryManager(&arrayCache);

  initializeSearchOptions();

  if (OnlyOutputStatesCoveringNew && !StatsTracker::useIStats())
    klee_error("To use --only-output-states-covering-new, you need to enable --output-istats.");

  if (DebugPrintInstructions.isSet(FILE_ALL) ||
      DebugPrintInstructions.isSet(FILE_COMPACT) ||
      DebugPrintInstructions.isSet(FILE_SRC)) {
    std::string debug_file_name =
        interpreterHandler->getOutputFilename("instructions.txt");
    std::string ErrorInfo;
#ifdef HAVE_ZLIB_H
    if (!DebugCompressInstructions) {
#endif

#if LLVM_VERSION_CODE >= LLVM_VERSION(3, 6)
    std::error_code ec;
    debugInstFile = new llvm::raw_fd_ostream(debug_file_name.c_str(), ec,
                                             llvm::sys::fs::OpenFlags::F_Text);
    if (ec)
	    ErrorInfo = ec.message();
#elif LLVM_VERSION_CODE >= LLVM_VERSION(3, 5)
    debugInstFile = new llvm::raw_fd_ostream(debug_file_name.c_str(), ErrorInfo,
                                             llvm::sys::fs::OpenFlags::F_Text);
#else
    debugInstFile =
        new llvm::raw_fd_ostream(debug_file_name.c_str(), ErrorInfo);
#endif
#ifdef HAVE_ZLIB_H
    } else {
      debugInstFile = new compressed_fd_ostream(
          (debug_file_name + ".gz").c_str(), ErrorInfo);
    }
#endif
    if (ErrorInfo != "") {
      klee_error("Could not open file %s : %s", debug_file_name.c_str(),
                 ErrorInfo.c_str());
    }
  }
}

const Function *Executor::getFunctionFromAddress(ref<Expr> addr) {
   if (klee::ConstantExpr *CE = dyn_cast<klee::ConstantExpr>(addr)) {
      if (globalAddressesRev.find(CE) != globalAddressesRev.end()) {
         const Function *f = dyn_cast<Function>(globalAddressesRev.find(CE)->second);
         return f;
      }
   }
   return NULL;
}

const Module *Executor::setModule(llvm::Module *module,
                                  const ModuleOptions &opts) {
  assert(!kmodule && module && "can only register one module"); // XXX gross

  kmodule = new KModule(module);
  /* SYSREL EXTENSION */
  kmoduleExt = kmodule;
  /* SYSREL EXTENSION */


  // Initialize the context.
  DataLayout *TD = kmodule->targetData;
  Context::initialize(TD->isLittleEndian(),
                      (Expr::Width) TD->getPointerSizeInBits());

  specialFunctionHandler = new SpecialFunctionHandler(*this);

  specialFunctionHandler->prepare();
  kmodule->prepare(opts, interpreterHandler);
  specialFunctionHandler->bind();

  if (StatsTracker::useStatistics() || userSearcherRequiresMD2U()) {
    statsTracker =
      new StatsTracker(*this,
                       interpreterHandler->getOutputFilename("assembly.ll"),
                       userSearcherRequiresMD2U());
  }

  return module;
}

Executor::~Executor() {
  delete memory;
  delete externalDispatcher;
  delete processTree;
  delete specialFunctionHandler;
  delete statsTracker;
  delete solver;
  delete kmodule;
  while(!timers.empty()) {
    delete timers.back();
    timers.pop_back();
  }
  delete debugInstFile;
}

/***/

void Executor::initializeGlobalObject(ExecutionState &state, ObjectState *os,
                                      const Constant *c,
                                      unsigned offset) {
  DataLayout *targetData = kmodule->targetData;
  if (const ConstantVector *cp = dyn_cast<ConstantVector>(c)) {
    unsigned elementSize =
      targetData->getTypeStoreSize(cp->getType()->getElementType());
    for (unsigned i=0, e=cp->getNumOperands(); i != e; ++i)
      initializeGlobalObject(state, os, cp->getOperand(i),
			     offset + i*elementSize);
  } else if (isa<ConstantAggregateZero>(c)) {
    unsigned i, size = targetData->getTypeStoreSize(c->getType());
    for (i=0; i<size; i++)
      os->write8(offset+i, (uint8_t) 0);
  } else if (const ConstantArray *ca = dyn_cast<ConstantArray>(c)) {
    unsigned elementSize =
      targetData->getTypeStoreSize(ca->getType()->getElementType());
    for (unsigned i=0, e=ca->getNumOperands(); i != e; ++i)
      initializeGlobalObject(state, os, ca->getOperand(i),
			     offset + i*elementSize);
  } else if (const ConstantStruct *cs = dyn_cast<ConstantStruct>(c)) {
    const StructLayout *sl =
      targetData->getStructLayout(cast<StructType>(cs->getType()));
    for (unsigned i=0, e=cs->getNumOperands(); i != e; ++i)
      initializeGlobalObject(state, os, cs->getOperand(i),
			     offset + sl->getElementOffset(i));
  } else if (const ConstantDataSequential *cds =
               dyn_cast<ConstantDataSequential>(c)) {
    unsigned elementSize =
      targetData->getTypeStoreSize(cds->getElementType());
    for (unsigned i=0, e=cds->getNumElements(); i != e; ++i)
      initializeGlobalObject(state, os, cds->getElementAsConstant(i),
                             offset + i*elementSize);
  } else if (!isa<UndefValue>(c)) {
    unsigned StoreBits = targetData->getTypeStoreSizeInBits(c->getType());
    ref<ConstantExpr> C = evalConstant(c);

    // Extend the constant if necessary;
   // assert(StoreBits >= C->getWidth() && "Invalid store size!");
    if (StoreBits > C->getWidth()) {
      llvm::errs() << "WARNING: StoreBits " << StoreBits << " greater than width " << C->getWidth() << "\n";
      C = C->ZExt(StoreBits);
    }
    os->write(offset, C);
 
  }
}

MemoryObject * Executor::addExternalObject(ExecutionState &state,
                                           void *addr, unsigned size,
                                           bool isReadOnly) {
  auto mo = memory->allocateFixed(reinterpret_cast<std::uint64_t>(addr),
                                  size, nullptr);
  ObjectState *os = bindObjectInState(state, mo, false);
  for(unsigned i = 0; i < size; i++)
    os->write8(i, ((uint8_t*)addr)[i]);
  if(isReadOnly)
    os->setReadOnly(true);
  return mo;
}


extern void *__dso_handle __attribute__ ((__weak__));

void Executor::initializeGlobals(ExecutionState &state) {
  Module *m = kmodule->module;

  if (m->getModuleInlineAsm() != "")
    klee_warning("executable has module level assembly (ignoring)");
  // represent function globals using the address of the actual llvm function
  // object. given that we use malloc to allocate memory in states this also
  // ensures that we won't conflict. we don't need to allocate a memory object
  // since reading/writing via a function pointer is unsupported anyway.
  for (Module::iterator i = m->begin(), ie = m->end(); i != ie; ++i) {
    Function *f = &*i;
    ref<ConstantExpr> addr(0);

    // If the symbol has external weak linkage then it is implicitly
    // not defined in this module; if it isn't resolvable then it
    // should be null.
    if (f->hasExternalWeakLinkage() &&
        !externalDispatcher->resolveSymbol(f->getName())) {
      addr = Expr::createPointer(0);
    } else {
      addr = Expr::createPointer(reinterpret_cast<std::uint64_t>(f));
      legalFunctions.insert(reinterpret_cast<std::uint64_t>(f));
    }

    globalAddresses.insert(std::make_pair(f, addr));

    /* SYSREL extension */
    //   globalAddressesRev.insert(std::make_pair(addr,f));
    /* SYSREL extension */
  }

#ifndef WINDOWS
  int *errno_addr = getErrnoLocation(state);
  MemoryObject *errnoObj =
      addExternalObject(state, (void *)errno_addr, sizeof *errno_addr, false);
  // Copy values from and to program space explicitly
  errnoObj->isUserSpecified = true;
#endif

  // Disabled, we don't want to promote use of live externals.
#ifdef HAVE_CTYPE_EXTERNALS
#ifndef WINDOWS
#ifndef DARWIN
  /* from /usr/include/ctype.h:
       These point into arrays of 384, so they can be indexed by any `unsigned
       char' value [0,255]; by EOF (-1); or by any `signed char' value
       [-128,-1).  ISO C requires that the ctype functions work for `unsigned */
  const uint16_t **addr = __ctype_b_loc();
  addExternalObject(state, const_cast<uint16_t*>(*addr-128),
                    384 * sizeof **addr, true);
  addExternalObject(state, addr, sizeof(*addr), true);

  const int32_t **lower_addr = __ctype_tolower_loc();
  addExternalObject(state, const_cast<int32_t*>(*lower_addr-128),
                    384 * sizeof **lower_addr, true);
  addExternalObject(state, lower_addr, sizeof(*lower_addr), true);

  const int32_t **upper_addr = __ctype_toupper_loc();
  addExternalObject(state, const_cast<int32_t*>(*upper_addr-128),
                    384 * sizeof **upper_addr, true);
  addExternalObject(state, upper_addr, sizeof(*upper_addr), true);
#endif
#endif
#endif

  // allocate and initialize globals, done in two passes since we may
  // need address of a global in order to initialize some other one.

  // allocate memory objects for all globals
  for (Module::const_global_iterator i = m->global_begin(),
         e = m->global_end();
       i != e; ++i) {
    const GlobalVariable *v = &*i;
    size_t globalObjectAlignment = getAllocationAlignment(v);
    if (i->isDeclaration()) {
      // FIXME: We have no general way of handling unknown external
      // symbols. If we really cared about making external stuff work
      // better we could support user definition, or use the EXE style
      // hack where we check the object file information.

      Type *ty = i->getType()->getElementType();
      uint64_t size = 0;
      if (ty->isSized()) {
	size = kmodule->targetData->getTypeStoreSize(ty);
      } else {
        klee_warning("Type for %.*s is not sized", (int)i->getName().size(),
			i->getName().data());
      }

      // XXX - DWD - hardcode some things until we decide how to fix.
#ifndef WINDOWS
      if (i->getName() == "_ZTVN10__cxxabiv117__class_type_infoE") {
        size = 0x2C;
      } else if (i->getName() == "_ZTVN10__cxxabiv120__si_class_type_infoE") {
        size = 0x2C;
      } else if (i->getName() == "_ZTVN10__cxxabiv121__vmi_class_type_infoE") {
        size = 0x2C;
      }
#endif

      if (size == 0) {
        klee_warning("Unable to find size for global variable: %.*s (use will result in out of bounds access)",
			(int)i->getName().size(), i->getName().data());
      }

      MemoryObject *mo = memory->allocate(size, /*isLocal=*/false,
                                          /*isGlobal=*/true, /*allocSite=*/v,
                                          /*alignment=*/globalObjectAlignment);
      recordMemObj(state, mo);
      ObjectState *os = bindObjectInState(state, mo, false);
      globalObjects.insert(std::make_pair(v, mo));
      globalAddresses.insert(std::make_pair(v, mo->getBaseExpr()));

      /* SYSREL extension */
      //globalAddressesRev.insert(std::make_pair(mo->getBaseExpr(), v));
      /* SYSREL extension */

      // Program already running = object already initialized.  Read
      // concrete value and write it to our copy.
      if (size) {
        void *addr;
        if (i->getName() == "__dso_handle") {
          addr = &__dso_handle; // wtf ?
        } else {
          addr = externalDispatcher->resolveSymbol(i->getName());
        }
        if (!addr) {
          /* SYSREL EXTENSION */
          if (!lazyInit)
             klee_error("unable to load symbol(%s) while initializing globals.",
                     i->getName().data());
          else
             executeMakeSymbolic(state, mo, i->getName());
        }
        else {
           /* SYSREL EXTENSION */
        for (unsigned offset=0; offset<mo->size; offset++)
          os->write8(offset, ((unsigned char*)addr)[offset]);
        }
      }
    } else {
      Type *ty = i->getType()->getElementType();
      uint64_t size = kmodule->targetData->getTypeStoreSize(ty);
      MemoryObject *mo = memory->allocate(size, /*isLocal=*/false,
                                          /*isGlobal=*/true, /*allocSite=*/v,
                                          /*alignment=*/globalObjectAlignment);
      if (!mo)
        llvm::report_fatal_error("out of memory");
      recordMemObj(state, mo);
      ObjectState *os = bindObjectInState(state, mo, false);
      globalObjects.insert(std::make_pair(v, mo));
      globalAddresses.insert(std::make_pair(v, mo->getBaseExpr()));

      /* SYSREL extnesion */
      //globalAddressesRev.insert(std::make_pair(mo->getBaseExpr(), v));
      /* SYSREL extnesion */


      if (!i->hasInitializer())
          os->initializeToRandom();
    }
  }

  // link aliases to their definitions (if bound)
  for (Module::alias_iterator i = m->alias_begin(), ie = m->alias_end();
       i != ie; ++i) {
    // Map the alias to its aliasee's address. This works because we have
    // addresses for everything, even undefined functions.
    globalAddresses.insert(std::make_pair(&*i, evalConstant(i->getAliasee())));
    /* SYSREL extension */
     //  globalAddressesRev.insert(std::make_pair(evalConstant(i->getAliasee()), &*i));
    /* SYSREL extension */

  }

  // once all objects are allocated, do the actual initialization
  for (Module::const_global_iterator i = m->global_begin(),
         e = m->global_end();
       i != e; ++i) {
    if (i->hasInitializer()) {
      const GlobalVariable *v = &*i;
      MemoryObject *mo = globalObjects.find(v)->second;
      const ObjectState *os = state.addressSpace.findObject(mo);
      assert(os);
      ObjectState *wos = state.addressSpace.getWriteable(mo, os);

      initializeGlobalObject(state, wos, i->getInitializer(), 0);
      // if(i->isConstant()) os->setReadOnly(true);
    }
  }
}

void Executor::branch(ExecutionState &state,
                      const std::vector< ref<Expr> > &conditions,
                      std::vector<ExecutionState*> &result, bool forBranch) {
  TimerStatIncrementer timer(stats::forkTime);
  unsigned N = conditions.size();
  assert(N);

  if (MaxForks!=~0u && stats::forks >= MaxForks) {
    unsigned next = theRNG.getInt32() % N;
    for (unsigned i=0; i<N; ++i) {
      if (i == next) {
        result.push_back(&state);
      } else {
        result.push_back(NULL);
      }
    }
  } else {
    stats::forks += N-1;

    // XXX do proper balance or keep random?
    result.push_back(&state);
    for (unsigned i=1; i<N; ++i) {
      ExecutionState *es = result[theRNG.getInt32() % i];
      ExecutionState *ns = es->branch();
      addedStates.push_back(ns);
      result.push_back(ns);
      es->ptreeNode->data = 0;
      std::pair<PTree::Node*,PTree::Node*> res =
        processTree->split(es->ptreeNode, ns, es);
      ns->ptreeNode = res.first;
      es->ptreeNode = res.second;
    }
  }

  // If necessary redistribute seeds to match conditions, killing
  // states if necessary due to OnlyReplaySeeds (inefficient but
  // simple).

  std::map< ExecutionState*, std::vector<SeedInfo> >::iterator it =
    seedMap.find(&state);
  if (it != seedMap.end()) {
    std::vector<SeedInfo> seeds = it->second;
    seedMap.erase(it);

    // Assume each seed only satisfies one condition (necessarily true
    // when conditions are mutually exclusive and their conjunction is
    // a tautology).
    for (std::vector<SeedInfo>::iterator siit = seeds.begin(),
           siie = seeds.end(); siit != siie; ++siit) {
      unsigned i;
      for (i=0; i<N; ++i) {
        ref<ConstantExpr> res;
        bool success =
          solver->getValue(state, siit->assignment.evaluate(conditions[i]),
                           res);
        assert(success && "FIXME: Unhandled solver failure");
        (void) success;
        if (res->isTrue())
          break;
      }

      // If we didn't find a satisfying condition randomly pick one
      // (the seed will be patched).
      if (i==N)
        i = theRNG.getInt32() % N;

      // Extra check in case we're replaying seeds with a max-fork
      if (result[i])
        seedMap[result[i]].push_back(*siit);
    }

    if (OnlyReplaySeeds) {
      for (unsigned i=0; i<N; ++i) {
        if (result[i] && !seedMap.count(result[i])) {
          terminateState(*result[i]);
          result[i] = NULL;
        }
      }
    }
  }

  for (unsigned i=0; i<N; ++i)
    if (result[i])
      addConstraint(*result[i], conditions[i], forBranch);
}

Executor::StatePair
Executor::fork(ExecutionState &current, ref<Expr> condition, bool isInternal, bool forBranch) {


  Solver::Validity res;
  std::map< ExecutionState*, std::vector<SeedInfo> >::iterator it =
    seedMap.find(&current);
  bool isSeeding = it != seedMap.end();

  if (!isSeeding && !isa<ConstantExpr>(condition) &&
      (MaxStaticForkPct!=1. || MaxStaticSolvePct != 1. ||
       MaxStaticCPForkPct!=1. || MaxStaticCPSolvePct != 1.) &&
      statsTracker->elapsed() > 60.) {
    StatisticManager &sm = *theStatisticManager;
    CallPathNode *cpn = current.stack.back().callPathNode;
    if ((MaxStaticForkPct<1. &&
         sm.getIndexedValue(stats::forks, sm.getIndex()) >
         stats::forks*MaxStaticForkPct) ||
        (MaxStaticCPForkPct<1. &&
         cpn && (cpn->statistics.getValue(stats::forks) >
                 stats::forks*MaxStaticCPForkPct)) ||
        (MaxStaticSolvePct<1 &&
         sm.getIndexedValue(stats::solverTime, sm.getIndex()) >
         stats::solverTime*MaxStaticSolvePct) ||
        (MaxStaticCPForkPct<1. &&
         cpn && (cpn->statistics.getValue(stats::solverTime) >
                 stats::solverTime*MaxStaticCPSolvePct))) {
      ref<ConstantExpr> value;
      bool success = solver->getValue(current, condition, value);
      assert(success && "FIXME: Unhandled solver failure");
      (void) success;
      addConstraint(current, EqExpr::create(value, condition), forBranch);
      condition = value;
    }
  }

  double timeout = coreSolverTimeout;
  if (isSeeding)
    timeout *= it->second.size();
  solver->setTimeout(timeout);
  bool success = solver->evaluate(current, condition, res);
  solver->setTimeout(0);
  if (!success) {
    current.pc = current.prevPC;
    terminateStateEarly(current, "Query timed out (fork).");
    return StatePair(0, 0);
  }

  if (!isSeeding) {
    if (replayPath && !isInternal) {
      assert(replayPosition<replayPath->size() &&
             "ran out of branches in replay path mode");
      bool branch = (*replayPath)[replayPosition++];

      if (res==Solver::True) {
        assert(branch && "hit invalid branch in replay path mode");
      } else if (res==Solver::False) {
        assert(!branch && "hit invalid branch in replay path mode");
      } else {
        // add constraints
        if(branch) {
          res = Solver::True;
          addConstraint(current, condition, forBranch);
        } else  {
          res = Solver::False;
          addConstraint(current, Expr::createIsZero(condition), forBranch);
        }
      }
    } else if (res==Solver::Unknown) {
      assert(!replayKTest && "in replay mode, only one branch can be true.");

      if ((MaxMemoryInhibit && atMemoryLimit) ||
          current.forkDisabled ||
          inhibitForking ||
          (MaxForks!=~0u && stats::forks >= MaxForks)) {

	if (MaxMemoryInhibit && atMemoryLimit)
	  klee_warning_once(0, "skipping fork (memory cap exceeded)");
	else if (current.forkDisabled)
	  klee_warning_once(0, "skipping fork (fork disabled on current path)");
	else if (inhibitForking)
	  klee_warning_once(0, "skipping fork (fork disabled globally)");
	else
	  klee_warning_once(0, "skipping fork (max-forks reached)");

        TimerStatIncrementer timer(stats::forkTime);
        if (theRNG.getBool()) {
          addConstraint(current, condition, forBranch);
          res = Solver::True;
        } else {
          addConstraint(current, Expr::createIsZero(condition), forBranch);
          res = Solver::False;
        }
      }
    }
  }

  // Fix branch in only-replay-seed mode, if we don't have both true
  // and false seeds.
  if (isSeeding &&
      (current.forkDisabled || OnlyReplaySeeds) &&
      res == Solver::Unknown) {
    bool trueSeed=false, falseSeed=false;
    // Is seed extension still ok here?
    for (std::vector<SeedInfo>::iterator siit = it->second.begin(),
           siie = it->second.end(); siit != siie; ++siit) {
      ref<ConstantExpr> res;
      bool success =
        solver->getValue(current, siit->assignment.evaluate(condition), res);
      assert(success && "FIXME: Unhandled solver failure");
      (void) success;
      if (res->isTrue()) {
        trueSeed = true;
      } else {
        falseSeed = true;
      }
      if (trueSeed && falseSeed)
        break;
    }
    if (!(trueSeed && falseSeed)) {
      assert(trueSeed || falseSeed);

      res = trueSeed ? Solver::True : Solver::False;
      addConstraint(current, trueSeed ? condition : Expr::createIsZero(condition), forBranch);
    }
  }


  // XXX - even if the constraint is provable one way or the other we
  // can probably benefit by adding this constraint and allowing it to
  // reduce the other constraints. For example, if we do a binary
  // search on a particular value, and then see a comparison against
  // the value it has been fixed at, we should take this as a nice
  // hint to just use the single constraint instead of all the binary
  // search ones. If that makes sense.
  if (res==Solver::True) {
    if (!isInternal) {
      if (pathWriter) {
        current.pathOS << "1";
      }
    }

    return StatePair(&current, 0);
  } else if (res==Solver::False) {
    if (!isInternal) {
      if (pathWriter) {
        current.pathOS << "0";
      }
    }

    return StatePair(0, &current);
  } else {
    TimerStatIncrementer timer(stats::forkTime);
    ExecutionState *falseState, *trueState = &current;

    ++stats::forks;

    falseState = trueState->branch();
    addedStates.push_back(falseState);

    if (it != seedMap.end()) {
      std::vector<SeedInfo> seeds = it->second;
      it->second.clear();
      std::vector<SeedInfo> &trueSeeds = seedMap[trueState];
      std::vector<SeedInfo> &falseSeeds = seedMap[falseState];
      for (std::vector<SeedInfo>::iterator siit = seeds.begin(),
             siie = seeds.end(); siit != siie; ++siit) {
        ref<ConstantExpr> res;
        bool success =
          solver->getValue(current, siit->assignment.evaluate(condition), res);
        assert(success && "FIXME: Unhandled solver failure");
        (void) success;
        if (res->isTrue()) {
          trueSeeds.push_back(*siit);
        } else {
          falseSeeds.push_back(*siit);
        }
      }

      bool swapInfo = false;
      if (trueSeeds.empty()) {
        if (&current == trueState) swapInfo = true;
        seedMap.erase(trueState);
      }
      if (falseSeeds.empty()) {
        if (&current == falseState) swapInfo = true;
        seedMap.erase(falseState);
      }
      if (swapInfo) {
        std::swap(trueState->coveredNew, falseState->coveredNew);
        std::swap(trueState->coveredLines, falseState->coveredLines);
      }
    }

    current.ptreeNode->data = 0;
    std::pair<PTree::Node*, PTree::Node*> res =
      processTree->split(current.ptreeNode, falseState, trueState);
    falseState->ptreeNode = res.first;
    trueState->ptreeNode = res.second;

    if (pathWriter) {
      // Need to update the pathOS.id field of falseState, otherwise the same id
      // is used for both falseState and trueState.
      falseState->pathOS = pathWriter->open(current.pathOS);
      if (!isInternal) {
        trueState->pathOS << "1";
        falseState->pathOS << "0";
      }
    }
    if (symPathWriter) {
      falseState->symPathOS = symPathWriter->open(current.symPathOS);
      if (!isInternal) {
        trueState->symPathOS << "1";
        falseState->symPathOS << "0";
      }
    }

    addConstraint(*trueState, condition, forBranch);
    addConstraint(*falseState, Expr::createIsZero(condition), forBranch);

    // Kinda gross, do we even really still want this option?
    if (MaxDepth && MaxDepth<=trueState->depth) {
      terminateStateEarly(*trueState, "max-depth exceeded.");
      terminateStateEarly(*falseState, "max-depth exceeded.");
      return StatePair(0, 0);
    }


   // clone state specific data here!!!

   // info flow make a copy of the infoflowcontext!!! 
   if (&current == trueState)
      cloneInfoFlowContext(current, *falseState);
   else 
      cloneInfoFlowContext(current, *trueState);

    return StatePair(trueState, falseState);
  }
}

void Executor::addConstraint(ExecutionState &state, ref<Expr> condition, bool forBranch) {
  if (ConstantExpr *CE = dyn_cast<ConstantExpr>(condition)) {
    if (!CE->isTrue())
      llvm::report_fatal_error("attempt to add invalid constraint");
    return;
  }

  // Check to see if this constraint violates seeds.
  std::map< ExecutionState*, std::vector<SeedInfo> >::iterator it =
    seedMap.find(&state);
  if (it != seedMap.end()) {
    bool warn = false;
    for (std::vector<SeedInfo>::iterator siit = it->second.begin(),
           siie = it->second.end(); siit != siie; ++siit) {
      bool res;
      bool success =
        solver->mustBeFalse(state, siit->assignment.evaluate(condition), res);
      assert(success && "FIXME: Unhandled solver failure");
      (void) success;
      if (res) {
        siit->patchSeed(state, condition, solver);
        warn = true;
      }
    }
    if (warn)
      klee_warning("seeds patched for violating constraint");
  }
 
  if (forBranch)
     state.addConstraintForBranch(condition);
  else
     state.addConstraint(condition);
 

  if (ivcEnabled)
    doImpliedValueConcretization(state, condition,
                                 ConstantExpr::alloc(1, Expr::Bool));
}




const Cell& Executor::eval(KInstruction *ki, unsigned index,
                           ExecutionState &state) const {
  assert(index < ki->inst->getNumOperands());
  int vnumber = ki->operands[index];

  assert(vnumber != -1 &&
         "Invalid operand to eval(), not a value or constant!");

  // Determine if this is a constant or not.
  if (vnumber < 0) {
    unsigned index = -vnumber - 2;
    return kmodule->constantTable[index];
  } else {
    unsigned index = vnumber;
    StackFrame &sf = state.stack.back();
    return sf.locals[index];
  }
}

void Executor::bindLocal(KInstruction *target, ExecutionState &state,
                         ref<Expr> value) {

  getDestCell(state, target).value = value;
}

void Executor::bindArgument(KFunction *kf, unsigned index,
                            ExecutionState &state, ref<Expr> value) {
  getArgumentCell(state, kf, index).value = value;
}

ref<Expr> Executor::toUnique(const ExecutionState &state,
                             ref<Expr> &e) {
  ref<Expr> result = e;

  if (!isa<ConstantExpr>(e)) {
    ref<ConstantExpr> value;
    bool isTrue = false;

    solver->setTimeout(coreSolverTimeout);
    if (solver->getValue(state, e, value) &&
        solver->mustBeTrue(state, EqExpr::create(e, value), isTrue) &&
        isTrue)
      result = value;
    solver->setTimeout(0);
  }

  return result;
}


/* Concretize the given expression, and return a possible constant value.
   'reason' is just a documentation string stating the reason for concretization. */
ref<klee::ConstantExpr>
Executor::toConstant(ExecutionState &state,
                     ref<Expr> e,
                     const char *reason) {
  e = state.constraints.simplifyExpr(e);
  if (ConstantExpr *CE = dyn_cast<ConstantExpr>(e))
    return CE;

  ref<ConstantExpr> value;
  bool success = solver->getValue(state, e, value);
  assert(success && "FIXME: Unhandled solver failure");
  (void) success;

  std::string str;
  llvm::raw_string_ostream os(str);
  os << "silently concretizing (reason: " << reason << ") expression " << e
     << " to value " << value << " (" << (*(state.pc)).info->file << ":"
     << (*(state.pc)).info->line << ")";

  if (AllExternalWarnings)
    klee_warning(reason, os.str().c_str());
  else
    klee_warning_once(reason, "%s", os.str().c_str());

  addConstraint(state, EqExpr::create(e, value));

  return value;
}

void Executor::executeGetValue(ExecutionState &state,
                               ref<Expr> e,
                               KInstruction *target) {
  e = state.constraints.simplifyExpr(e);
  std::map< ExecutionState*, std::vector<SeedInfo> >::iterator it =
    seedMap.find(&state);
  if (it==seedMap.end() || isa<ConstantExpr>(e)) {
    ref<ConstantExpr> value;
    bool success = solver->getValue(state, e, value);
    assert(success && "FIXME: Unhandled solver failure");
    (void) success;
    bindLocal(target, state, value);
  } else {
    std::set< ref<Expr> > values;
    for (std::vector<SeedInfo>::iterator siit = it->second.begin(),
           siie = it->second.end(); siit != siie; ++siit) {
      ref<ConstantExpr> value;
      bool success =
        solver->getValue(state, siit->assignment.evaluate(e), value);
      assert(success && "FIXME: Unhandled solver failure");
      (void) success;
      values.insert(value);
    }

    std::vector< ref<Expr> > conditions;
    for (std::set< ref<Expr> >::iterator vit = values.begin(),
           vie = values.end(); vit != vie; ++vit)
      conditions.push_back(EqExpr::create(e, *vit));

    std::vector<ExecutionState*> branches;
    branch(state, conditions, branches);

    std::vector<ExecutionState*>::iterator bit = branches.begin();
    for (std::set< ref<Expr> >::iterator vit = values.begin(),
           vie = values.end(); vit != vie; ++vit) {
      ExecutionState *es = *bit;
      if (es)
        bindLocal(target, *es, *vit);
      ++bit;
    }
  }
}

void Executor::printDebugInstructions(ExecutionState &state) {
  // check do not print
  if (DebugPrintInstructions.getBits() == 0)
	  return;

  llvm::raw_ostream *stream = 0;
  if (DebugPrintInstructions.isSet(STDERR_ALL) ||
      DebugPrintInstructions.isSet(STDERR_SRC) ||
      DebugPrintInstructions.isSet(STDERR_COMPACT))
    stream = &llvm::errs();
  else
    stream = &debugLogBuffer;

  if (!DebugPrintInstructions.isSet(STDERR_COMPACT) &&
      !DebugPrintInstructions.isSet(FILE_COMPACT)) {
    (*stream) << "     " << state.pc->getSourceLocation() << ":";
  }

  (*stream) << state.pc->info->assemblyLine;

  if (DebugPrintInstructions.isSet(STDERR_ALL) ||
      DebugPrintInstructions.isSet(FILE_ALL))
    (*stream) << ":" << *(state.pc->inst);
  (*stream) << "\n";

  if (DebugPrintInstructions.isSet(FILE_ALL) ||
      DebugPrintInstructions.isSet(FILE_COMPACT) ||
      DebugPrintInstructions.isSet(FILE_SRC)) {
    debugLogBuffer.flush();
    (*debugInstFile) << debugLogBuffer.str();
    debugBufferString = "";
  }
}

void Executor::stepInstruction(ExecutionState &state) {
  state.instCount++;
  printDebugInstructions(state);
  if (statsTracker)
    statsTracker->stepInstruction(state);

  ++stats::instructions;
  ++state.steppedInstructions;
  state.prevPC = state.pc;
  ++state.pc;

  if (stats::instructions==StopAfterNInstructions)
    haltExecution = true;
}

void Executor::executeCall(ExecutionState &state,
                           KInstruction *ki,
                           Function *f,
                           std::vector< ref<Expr> > &arguments) {
  Instruction *i = ki->inst;
  if (f && f->isDeclaration()) {
    switch(f->getIntrinsicID()) {
    case Intrinsic::not_intrinsic:
      // state may be destroyed by this call, cannot touch
      callExternalFunction(state, ki, f, arguments);
      break;

      // va_arg is handled by caller and intrinsic lowering, see comment for
      // ExecutionState::varargs
    case Intrinsic::vastart:  {
      StackFrame &sf = state.stack.back();

      // varargs can be zero if no varargs were provided
      if (!sf.varargs)
        return;

      // FIXME: This is really specific to the architecture, not the pointer
      // size. This happens to work for x86-32 and x86-64, however.
      Expr::Width WordSize = Context::get().getPointerWidth();
      if (WordSize == Expr::Int32) {
        executeMemoryOperation(state, -2, -2, true, arguments[0],
                               sf.varargs->getBaseExpr(), 0);
      } else {
        assert(WordSize == Expr::Int64 && "Unknown word size!");

        // x86-64 has quite complicated calling convention. However,
        // instead of implementing it, we can do a simple hack: just
        // make a function believe that all varargs are on stack.
        executeMemoryOperation(state, -2, -2, true, arguments[0],
                               ConstantExpr::create(48, 32), 0); // gp_offset
        executeMemoryOperation(state, -2, -2, true,
                               AddExpr::create(arguments[0],
                                               ConstantExpr::create(4, 64)),
                               ConstantExpr::create(304, 32), 0); // fp_offset
        executeMemoryOperation(state, -2, -2, true,
                               AddExpr::create(arguments[0],
                                               ConstantExpr::create(8, 64)),
                               sf.varargs->getBaseExpr(), 0); // overflow_arg_area
        executeMemoryOperation(state, -2, -2, true,
                               AddExpr::create(arguments[0],
                                               ConstantExpr::create(16, 64)),
                               ConstantExpr::create(0, 64), 0); // reg_save_area
      }
      break;
    }
    case Intrinsic::vaend:
      // va_end is a noop for the interpreter.
      //
      // FIXME: We should validate that the target didn't do something bad
      // with va_end, however (like call it twice).
      break;

    case Intrinsic::vacopy:
      // va_copy should have been lowered.
      //
      // FIXME: It would be nice to check for errors in the usage of this as
      // well.
    default:
      klee_error("unknown intrinsic: %s", f->getName().data());
    }

    if (InvokeInst *ii = dyn_cast<InvokeInst>(i))
      transferToBasicBlock(ii->getNormalDest(), i->getParent(), state);
  } else {


       /* SYSREL extension */

      // check if PROSE version of the function exists, if so use that one
      std::string prosename = f->getName().str() + "_PROSE"; 
      Function *proseFn = moduleHandle->getFunction(prosename);
      if (proseFn) {
         llvm::errs() << "WARNING: Using " << prosename << " for " << f->getName() << "\n";
         ((CallInst*)ki->inst)->setCalledFunction(proseFn);
         executeCall(state, ki, proseFn, arguments);
         return;
      }


       //llvm::errs() << "checking regular function call for high/low flows\n";
       //checkHighArgumentFlow(state, ki, f, arguments);
       // Handle certain functions in a special way, e.g., those that have inline assembly
       if (lazyInit && (APIHandler::handles(removeDotSuffix(f->getName())) || isInfoFlowAPI(removeDotSuffix(f->getName())))) {
          callExternalFunction(state, ki, f, arguments);
          return;
       }
       /* SYSREL extension */



    // FIXME: I'm not really happy about this reliance on prevPC but it is ok, I
    // guess. This just done to avoid having to pass KInstIterator everywhere
    // instead of the actual instruction, since we can't make a KInstIterator
    // from just an instruction (unlike LLVM).
    KFunction *kf = kmodule->functionMap[f];
    state.pushFrame(state.prevPC, kf);
    state.pc = kf->instructions;
    #ifdef INFOFLOW
    pushInfoFlowContext(state);
    #endif
    if (statsTracker)
      statsTracker->framePushed(state, &state.stack[state.stack.size()-2]);

     // TODO: support "byval" parameter attribute
     // TODO: support zeroext, signext, sret attributes

    unsigned callingArgs = arguments.size();
    unsigned funcArgs = f->arg_size();
    if (!f->isVarArg()) {
      if (callingArgs > funcArgs) {
        klee_warning_once(f, "calling %s with extra arguments.",
                          f->getName().data());
      } else if (callingArgs < funcArgs) {
        terminateStateOnError(state, "calling function with too few arguments",
                              User);
        return;
      }
    } else {
      Expr::Width WordSize = Context::get().getPointerWidth();

      if (callingArgs < funcArgs) {
        terminateStateOnError(state, "calling function with too few arguments",
                              User);
        return;
      }

      StackFrame &sf = state.stack.back();
      unsigned size = 0;
      bool requires16ByteAlignment = false;
      for (unsigned i = funcArgs; i < callingArgs; i++) {
        // FIXME: This is really specific to the architecture, not the pointer
        // size. This happens to work for x86-32 and x86-64, however.
        if (WordSize == Expr::Int32) {
          size += Expr::getMinBytesForWidth(arguments[i]->getWidth());
        } else {
          Expr::Width argWidth = arguments[i]->getWidth();
          // AMD64-ABI 3.5.7p5: Step 7. Align l->overflow_arg_area upwards to a
          // 16 byte boundary if alignment needed by type exceeds 8 byte
          // boundary.
          //
          // Alignment requirements for scalar types is the same as their size
          if (argWidth > Expr::Int64) {
#if LLVM_VERSION_CODE >= LLVM_VERSION(3, 9)
             size = llvm::alignTo(size, 16);
#else
             size = llvm::RoundUpToAlignment(size, 16);
#endif
             requires16ByteAlignment = true;
          }
#if LLVM_VERSION_CODE >= LLVM_VERSION(3, 9)
          size += llvm::alignTo(argWidth, WordSize) / 8;
#else
          size += llvm::RoundUpToAlignment(argWidth, WordSize) / 8;
#endif
        }
      }

      MemoryObject *mo = sf.varargs =
          memory->allocate(size, true, false, state.prevPC->inst,
                           (requires16ByteAlignment ? 16 : 8));
      if (!mo && size) {
        terminateStateOnExecError(state, "out of memory (varargs)");
        return;
      }
   
      if (mo) {
        recordMemObj(state, mo);
        if ((WordSize == Expr::Int64) && (mo->address & 15) &&
            requires16ByteAlignment) {
          // Both 64bit Linux/Glibc and 64bit MacOSX should align to 16 bytes.
          klee_warning_once(
              0, "While allocating varargs: malloc did not align to 16 bytes.");
        }

        ObjectState *os = bindObjectInState(state, mo, true);
        unsigned offset = 0;
        for (unsigned i = funcArgs; i < callingArgs; i++) {
          // FIXME: This is really specific to the architecture, not the pointer
          // size. This happens to work for x86-32 and x86-64, however.
          if (WordSize == Expr::Int32) {
            os->write(offset, arguments[i]);
            offset += Expr::getMinBytesForWidth(arguments[i]->getWidth());
          } else {
            assert(WordSize == Expr::Int64 && "Unknown word size!");

            Expr::Width argWidth = arguments[i]->getWidth();
            if (argWidth > Expr::Int64) {
#if LLVM_VERSION_CODE >= LLVM_VERSION(3, 9)
              offset = llvm::alignTo(offset, 16);
#else
              offset = llvm::RoundUpToAlignment(offset, 16);
#endif
            }
            os->write(offset, arguments[i]);
#if LLVM_VERSION_CODE >= LLVM_VERSION(3, 9)
            offset += llvm::alignTo(argWidth, WordSize) / 8;
#else
            offset += llvm::RoundUpToAlignment(argWidth, WordSize) / 8;
#endif
          }
        }
      }
    }

    unsigned numFormals = f->arg_size();
    for (unsigned i=0; i<numFormals; ++i)
      bindArgument(kf, i, state, arguments[i]);
    llvm::errs() << "before check 2\n";
    symbolizeAndMarkSensitiveArgumentsOnCall(state,ki,f,arguments,false);
    llvm::errs() << "after check 2\n";
  }
}

void Executor::transferToBasicBlock(BasicBlock *dst, BasicBlock *src,
                                    ExecutionState &state) {
  // Note that in general phi nodes can reuse phi values from the same
  // block but the incoming value is the eval() result *before* the
  // execution of any phi nodes. this is pathological and doesn't
  // really seem to occur, but just in case we run the PhiCleanerPass
  // which makes sure this cannot happen and so it is safe to just
  // eval things in order. The PhiCleanerPass also makes sure that all
  // incoming blocks have the same order for each PHINode so we only
  // have to compute the index once.
  //
  // With that done we simply set an index in the state so that PHI
  // instructions know which argument to eval, set the pc, and continue.

  // XXX this lookup has to go ?
  KFunction *kf = state.stack.back().kf;
  unsigned entry = kf->basicBlockEntry[dst];
  state.pc = &kf->instructions[entry];
  if (state.pc->inst->getOpcode() == Instruction::PHI) {
    PHINode *first = static_cast<PHINode*>(state.pc->inst);
    state.incomingBBIndex = first->getBasicBlockIndex(src);
  }


  /* SYSREL */
  state.setPreemptable(-1, true);
  /* SYSREL */

}

/// Compute the true target of a function call, resolving LLVM and KLEE aliases
/// and bitcasts.
Function* Executor::getTargetFunction(Value *calledVal, ExecutionState &state) {
  SmallPtrSet<const GlobalValue*, 3> Visited;

  Constant *c = dyn_cast<Constant>(calledVal);
  if (!c)
    return 0;

  while (true) {
    if (GlobalValue *gv = dyn_cast<GlobalValue>(c)) {
#if LLVM_VERSION_CODE >= LLVM_VERSION(3, 6)
      if (!Visited.insert(gv).second)
        return 0;
#else
      if (!Visited.insert(gv))
        return 0;
#endif
      std::string alias = state.getFnAlias(gv->getName());
      if (alias != "") {
        llvm::Module* currModule = kmodule->module;
        GlobalValue *old_gv = gv;
        gv = currModule->getNamedValue(alias);
        if (!gv) {
          klee_error("Function %s(), alias for %s not found!\n", alias.c_str(),
                     old_gv->getName().str().c_str());
        }
      }

      if (Function *f = dyn_cast<Function>(gv))
        return f;
      else if (GlobalAlias *ga = dyn_cast<GlobalAlias>(gv))
        c = ga->getAliasee();
      else
        return 0;
    } else if (llvm::ConstantExpr *ce = dyn_cast<llvm::ConstantExpr>(c)) {
      if (ce->getOpcode()==Instruction::BitCast)
        c = ce->getOperand(0);
      else
        return 0;
    } else
      return 0;
  }
}

/// TODO remove?
/*static*/ bool isDebugIntrinsic(const Function *f, KModule *KM) {
  return false;
}

static inline const llvm::fltSemantics * fpWidthToSemantics(unsigned width) {
  switch(width) {
  case Expr::Int32:
    return &llvm::APFloat::IEEEsingle;
  case Expr::Int64:
    return &llvm::APFloat::IEEEdouble;
  case Expr::Fl80:
    return &llvm::APFloat::x87DoubleExtended;
  default:
    return 0;
  }
}

void Executor::executeInstruction(ExecutionState &state, KInstruction *ki) {

  Instruction *i = ki->inst;

  llvm::errs() << "next instruction: " << (*i) << "\n";

  /* SYSREL extension */
  /* side channel begin */
  if (!state.inEnclave && i->getParent()->getParent() && 
         ((Function*)i->getParent()->getParent())->getName() == state.getLastEnclaveFunction()) {
     llvm::errs() << "returned back to the enclave inside " << state.getLastEnclaveFunction() << "\n";
     state.inEnclave = true;
  }
  /* side channel end */  
  /* SYSREL extension */
 

  switch (i->getOpcode()) {
    // Control flow
  case Instruction::Ret: {
    bool terminating = false;
    ReturnInst *ri = cast<ReturnInst>(i);
    Function *rf = ri->getParent()->getParent();
    KInstIterator kcaller = state.stack.back().caller;
    Instruction *caller = kcaller ? kcaller->inst : 0;
    bool isVoidReturn = (ri->getNumOperands() == 0);
    ref<Expr> result = ConstantExpr::alloc(0, Expr::Bool);

    /* SYSREL EXTENSION */
    state.checkAndSetPMCallbackCompleted(ri->getParent()->getParent()->getName());
    /* SYSREL EXTENSION */
    #ifdef VB 
    llvm::outs() << "handling return for function " << ri->getParent()->getParent()->getName()  << "\n";
    #endif

    if (!isVoidReturn) {
      result = eval(ki, 0, state).value;
      /* SYSREL EXTENSION */
      /* side channel */
      if (ri->getParent()->getParent()->getName() == publicOutputReturningFName) {    
         RD *rd =  getrdmap(&state);  
         ref<Expr> eq = EqExpr::create(publicOutputVar,result); 
         rd->LC =  AndExpr::create(rd->LC, eq);
      }
      // reconstruct the arguments
      Function *f = ri->getParent()->getParent();
      KFunction *kf = kmodule->functionMap[f];
      std::vector<ref<Expr> > args;
      unsigned numFormals = f->arg_size();
      for (unsigned ind=0; ind<numFormals; ++ind) { 
          args.push_back(getArgumentCell(state, kf, ind).value);
      }
      symbolizeAndMarkArgumentsOnReturn(state, ki, ri->getParent()->getParent(), args, result);
      /* side channel */   
      if (state.hasLCM()) {
         #ifdef VB
         llvm::outs() << "function " << ri->getParent()->getParent()->getName() << " return expression " << result << "\n";
         #endif
         ConstantExpr *re = dyn_cast<ConstantExpr>(result);
         if (re) {
            //#ifdef VB
            llvm::errs() << "Recording return value for an expression of width " << re->getWidth() << ":\n";
            llvm::errs() << re->getZExtValue(re->getWidth()) << " for " << ri->getParent()->getParent()->getName() << "\n";
            //#endif
            state.returnValueModel[ri->getParent()->getParent()->getName()] = re->getZExtValue(re->getWidth());
         }
         else if (state.lcmStepMovesWhenReturns(rf->getName().str())) { //  consider possibility of a success case
            bool res;
            //ref<Expr> reszero = Expr::createIsZero(result);
            ref<Expr> ressucc = EqExpr::create(result,ConstantExpr::alloc(state.getCurrentSuccessReturnValue(), result->getWidth()));
            double timeout = coreSolverTimeout;
            solver->setTimeout(timeout);
            bool success = solver->mayBeTrue(state, ressucc, res);
            solver->setTimeout(0);
            if (success && res) {
               state.returnValueModel[ri->getParent()->getParent()->getName()] = state.getCurrentSuccessReturnValue();
            }
         }
      }
      #ifdef VB
      llvm::outs() << "return value " << result << " for " << ri->getParent()->getParent()->getName() << "\n";
      #endif
      /* SYSREL EXTENSION */
    }

    if (state.stack.size() <= 1) {
      assert(!caller && "caller set on initial stack frame");
      /* SYSREL extension */


      #ifdef VB     

      if (state.hasLCM()) {
         llvm::outs() << "returning from function " << rf->getName() << "\n";
         llvm::outs() << "lcm completed? " <<  state.lcmCompleted() << "\n";
         llvm::outs() << "return instruction:\n";
         ri->print(llvm::outs());
         llvm::outs() << "state id: " << state.getID() << "\n";
         ExprPPrinter::printConstraints(llvm::outs(), state.constraints);
      }
      #endif

      if (!state.activeThreads() && (!state.hasLCM() || state.lcmCompletesWith(rf->getName().str()))) {
         //#ifdef VB
         llvm::errs() << "terminating state with " << rf->getName() << "\n";
         //#endif

         /* Side channel begin */
         RD* rdd = getrdmap(&state);
         rdd->pathterminated = true;
         if (!isVoidReturn) {
           rdd->isvoid = false;
           rdd->retval = ref<Expr>(result);
         }
         checkHighSensitiveLocals(state,i);
         /* Side channel end */
         terminating = true;
         terminateStateOnExit(state);
      }
      else {
        if (state.activeThreads())
           state.setWaitingForThreadsToTerminate(true);
        if (state.hasLCM()) {
           if (state.lcmStepMovesWhenReturns(rf->getName())) {
              if (state.returnValueModel.find(rf->getName()) != state.returnValueModel.end() &&
                            state.returnValueModel[rf->getName()] == state.getCurrentSuccessReturnValue()) {
                 #ifdef VB
                 llvm::outs() << "lcm continues with the next sequential step\n";
                 #endif
                 state.updateLCMState();
                 if (LifeCycleModelState::lcm->getStep(state.getLCMState())->getValue() == sidechannelentry)
                    sidechannelstarted = true; 
              }
              else {
                 #ifdef VB
                 llvm::outs() << "terminating state and life cycle early due to error return value!\n";
                 #endif
                 checkHighSensitiveLocals(state,i);
                 terminating = true;
                 terminateStateOnExit(state);
              }
           }
           #ifdef VB
           else {
               llvm::outs() << "no, step not moving at..\n";
               llvm::outs() << state.prevPC->inst << "\n";
           }
           #endif
        }
      }
      /* SYSREL extension */
      #ifdef INFOFLOW
      if (!isVoidReturn)
         updateInfoFlowForReturn(state, getLocal(state, ki, 0), ki);
      #endif
    } else {
      /* SYSREL extension */
      if (!terminating) {
         checkHighSensitiveLocals(state,i);
      }
      #ifdef INFOFLOW
      if (!isVoidReturn)
         updateInfoFlowForReturn(state, getLocal(state, ki, 0), ki);
      #endif
      /* SYSREL extension */
      state.popFrame();
      #ifdef INFOFLOW
      popInfoFlowContext(state);
      #endif
      if (statsTracker)
        statsTracker->framePopped(state);

      if (InvokeInst *ii = dyn_cast<InvokeInst>(caller)) {
        #ifdef VB
        llvm::outs() << "returning to basic block of " << (*caller) << "\n";
        #endif
        transferToBasicBlock(ii->getNormalDest(), caller->getParent(), state);
      } else {
        state.pc = kcaller;
        ++state.pc;
        #ifdef VB
        llvm::outs() << "returning to " <<  (*state.pc->inst) << "\n";
        #endif
      }

      if (!isVoidReturn) {
        Type *t = caller->getType();
        if (t != Type::getVoidTy(i->getContext())) {
          // may need to do coercion due to bitcasts
          Expr::Width from = result->getWidth();
          Expr::Width to = getWidthForLLVMType(t);

          if (from != to) {
            CallSite cs = (isa<InvokeInst>(caller) ? CallSite(cast<InvokeInst>(caller)) :
                           CallSite(cast<CallInst>(caller)));

            // XXX need to check other param attrs ?
      bool isSExt = cs.paramHasAttr(0, llvm::Attribute::SExt);
            if (isSExt) {
              result = SExtExpr::create(result, to);
            } else {
              result = ZExtExpr::create(result, to);
            }
          }

          bindLocal(kcaller, state, result);
        }
      } else {
        // We check that the return value has no users instead of
        // checking the type, since C defaults to returning int for
        // undeclared functions.

        /* SYSREL extension */
        //if (!caller->use_empty()) {
          //terminateStateOnExecError(state, "return void when caller expected a result");
        //}
        #ifdef VB
        llvm::errs() << "Warning: return void when caller expected a result\n";
        #endif
        /* SYSREL extension */
      }
    }
    break;
  }
  case Instruction::Br: {
    BranchInst *bi = cast<BranchInst>(i);
    if (bi->isUnconditional()) {
      transferToBasicBlock(bi->getSuccessor(0), bi->getParent(), state);
    } else {
      // FIXME: Find a way that we don't have this hidden dependency.
      assert(bi->getCondition() == bi->getOperand(0) &&
             "Wrong operand index!");
      ref<Expr> cond = eval(ki, 0, state).value;
      /* SYSREL side channel begin */
      recordMostRecentBranchInfo(state, i);
      /* SYSREL side channel end */
      //llvm::errs() << "PC: \n"; 
      //ExprPPrinter::printConstraints(llvm::errs(), state.constraints);
      Executor::StatePair branches = fork(state, cond, false, true);

      // NOTE: There is a hidden dependency here, markBranchVisited
      // requires that we still be in the context of the branch
      // instruction (it reuses its statistic id). Should be cleaned
      // up with convenient instruction specific data.
      if (statsTracker && state.stack.back().kf->trackCoverage)
        statsTracker->markBranchVisited(branches.first, branches.second);

      if (branches.first) {
        transferToBasicBlock(bi->getSuccessor(0), bi->getParent(), *branches.first);
        /* SYSREL side channel begin */
        successorsPaths->insert(branches.first); // Keeping track of the new paths
        /* SYSREL side channel end */
      }
      if (branches.second) {
        transferToBasicBlock(bi->getSuccessor(1), bi->getParent(), *branches.second);
        /* SYSREL side channel begin */
        successorsPaths->insert(branches.second); // Keeping track of the new paths
        /* SYSREL side channel end */
      }

      /* SYSREL extension */
      if (branches.first && branches.second) {
         singleSuccessor = false;
         #ifdef VB
         llvm::errs() << "executingPM on successor first\n";
         #endif
         bool abort = false;
         branches.first->executePM(abort);
         if (abort) {
            terminateStateOnError(state, "Memory error", Ptr, NULL, "Possible use-after-free");
         }
         #ifdef VB
         llvm::errs() << "executingPM on successor second\n";
         #endif
         branches.second->executePM(abort);
         if (abort) {
            terminateStateOnError(state, "Memory error", Ptr, NULL, "Possible use-after-free");
         }
      }
      /* SYSREL extension */

    }
    break;
  }
  case Instruction::IndirectBr: {
    // implements indirect branch to a label within the current function
    const auto bi = cast<IndirectBrInst>(i);
    auto address = eval(ki, 0, state).value;
    address = toUnique(state, address);

    // concrete address
    if (const auto CE = dyn_cast<ConstantExpr>(address.get())) {
      const auto bb_address = (BasicBlock *) CE->getZExtValue(Context::get().getPointerWidth());
      transferToBasicBlock(bb_address, bi->getParent(), state);
      break;
    }

    // symbolic address
    const auto numDestinations = bi->getNumDestinations();
    std::vector<BasicBlock *> targets;
    targets.reserve(numDestinations);
    std::vector<ref<Expr>> expressions;
    expressions.reserve(numDestinations);

    ref<Expr> errorCase = ConstantExpr::alloc(1, Expr::Bool);
    SmallPtrSet<BasicBlock *, 5> destinations;
    // collect and check destinations from label list
    for (unsigned k = 0; k < numDestinations; ++k) {
      // filter duplicates
      const auto d = bi->getDestination(k);
      if (destinations.count(d)) continue;
      destinations.insert(d);

      // create address expression
      const auto PE = Expr::createPointer(reinterpret_cast<std::uint64_t>(d));
      ref<Expr> e = EqExpr::create(address, PE);

      // exclude address from errorCase
      errorCase = AndExpr::create(errorCase, Expr::createIsZero(e));

      // check feasibility
      bool result;
      bool success __attribute__ ((unused)) = solver->mayBeTrue(state, e, result);
      assert(success && "FIXME: Unhandled solver failure");
      if (result) {
        targets.push_back(d);
        expressions.push_back(e);
      }
    }
    // check errorCase feasibility
    bool result;
    bool success __attribute__ ((unused)) = solver->mayBeTrue(state, errorCase, result);
    assert(success && "FIXME: Unhandled solver failure");
    if (result) {
      expressions.push_back(errorCase);
    }

    // fork states
    std::vector<ExecutionState *> branches;
    branch(state, expressions, branches, true);

    // terminate error state
    if (result) {
      terminateStateOnExecError(*branches.back(), "indirectbr: illegal label address");
      branches.pop_back();
    }

    // branch states to resp. target blocks
    assert(targets.size() == branches.size());
    for (std::vector<ExecutionState *>::size_type k = 0; k < branches.size(); ++k) {
      if (branches[k]) {
        transferToBasicBlock(targets[k], bi->getParent(), *branches[k]);
      }
    }

    break;
  }
  case Instruction::Switch: {
    SwitchInst *si = cast<SwitchInst>(i);
    ref<Expr> cond = eval(ki, 0, state).value;
    /* SYSREL side channel begin */
    recordMostRecentBranchInfo(state, i);
    /* SYSREL side channel  end */
    BasicBlock *bb = si->getParent();

    cond = toUnique(state, cond);
    if (ConstantExpr *CE = dyn_cast<ConstantExpr>(cond)) {
      // Somewhat gross to create these all the time, but fine till we
      // switch to an internal rep.
      llvm::IntegerType *Ty = cast<IntegerType>(si->getCondition()->getType());
      ConstantInt *ci = ConstantInt::get(Ty, CE->getZExtValue());
      unsigned index = si->findCaseValue(ci).getSuccessorIndex();
      transferToBasicBlock(si->getSuccessor(index), si->getParent(), state);
    } else {
      // Handle possible different branch targets

      // We have the following assumptions:
      // - each case value is mutual exclusive to all other values including the
      //   default value
      // - order of case branches is based on the order of the expressions of
      //   the scase values, still default is handled last
      std::vector<BasicBlock *> bbOrder;
      std::map<BasicBlock *, ref<Expr> > branchTargets;

      std::map<ref<Expr>, BasicBlock *> expressionOrder;

      // Iterate through all non-default cases and order them by expressions
#if LLVM_VERSION_CODE > LLVM_VERSION(3, 4)
      for (auto i : si->cases()) {
#else
      for (SwitchInst::CaseIt i = si->case_begin(), e = si->case_end(); i != e;
           ++i) {
#endif
        ref<Expr> value = evalConstant(i.getCaseValue());

        BasicBlock *caseSuccessor = i.getCaseSuccessor();
        expressionOrder.insert(std::make_pair(value, caseSuccessor));
      }

      // Track default branch values
      ref<Expr> defaultValue = ConstantExpr::alloc(1, Expr::Bool);

      // iterate through all non-default cases but in order of the expressions
      for (std::map<ref<Expr>, BasicBlock *>::iterator
               it = expressionOrder.begin(),
               itE = expressionOrder.end();
           it != itE; ++it) {
        ref<Expr> match = EqExpr::create(cond, it->first);

        // Make sure that the default value does not contain this target's value
        defaultValue = AndExpr::create(defaultValue, Expr::createIsZero(match));

        // Check if control flow could take this case
        bool result;
        bool success = solver->mayBeTrue(state, match, result);
        assert(success && "FIXME: Unhandled solver failure");
        (void) success;
        if (result) {
          BasicBlock *caseSuccessor = it->second;

          // Handle the case that a basic block might be the target of multiple
          // switch cases.
          // Currently we generate an expression containing all switch-case
          // values for the same target basic block. We spare us forking too
          // many times but we generate more complex condition expressions
          // TODO Add option to allow to choose between those behaviors
          std::pair<std::map<BasicBlock *, ref<Expr> >::iterator, bool> res =
              branchTargets.insert(std::make_pair(
                  caseSuccessor, ConstantExpr::alloc(0, Expr::Bool)));

          res.first->second = OrExpr::create(match, res.first->second);

          // Only add basic blocks which have not been target of a branch yet
          if (res.second) {
            bbOrder.push_back(caseSuccessor);
          }
        }
      }

      // Check if control could take the default case
      bool res;
      bool success = solver->mayBeTrue(state, defaultValue, res);
      assert(success && "FIXME: Unhandled solver failure");
      (void) success;
      if (res) {
        std::pair<std::map<BasicBlock *, ref<Expr> >::iterator, bool> ret =
            branchTargets.insert(
                std::make_pair(si->getDefaultDest(), defaultValue));
        if (ret.second) {
          bbOrder.push_back(si->getDefaultDest());
        }
      }

      // Fork the current state with each state having one of the possible
      // successors of this switch
      std::vector< ref<Expr> > conditions;
      for (std::vector<BasicBlock *>::iterator it = bbOrder.begin(),
                                               ie = bbOrder.end();
           it != ie; ++it) {
        conditions.push_back(branchTargets[*it]);
      }
      std::vector<ExecutionState*> branches;
      branch(state, conditions, branches, true);

      std::vector<ExecutionState*>::iterator bit = branches.begin();
      for (std::vector<BasicBlock *>::iterator it = bbOrder.begin(),
                                               ie = bbOrder.end();
           it != ie; ++it) {
        ExecutionState *es = *bit;
        if (es) {
          transferToBasicBlock(*it, bb, *es);
          /* SYSREL side channel begin */
          successorsPaths->insert(es);
          /* SYSREL side channel end */
        }
        ++bit;
      }
    }
    break;
  }
  case Instruction::Unreachable:
    // Note that this is not necessarily an internal bug, llvm will
    // generate unreachable instructions in cases where it knows the
    // program will crash. So it is effectively a SEGV or internal
    // error.
    terminateStateOnExecError(state, "reached \"unreachable\" instruction");
    break;

  case Instruction::Invoke:
  case Instruction::Call: {
    CallSite cs(i);

    unsigned numArgs = cs.arg_size();
    Value *fp = cs.getCalledValue();
    Function *f = getTargetFunction(fp, state);

    /* begin SYSREL extension */
    /* side channel begin */
    // making an 
    if (state.inEnclave && f && std::find(untrusted->begin(), untrusted->end(), f->getName()) != untrusted->end()) {
       // First create a branch to be terminated to simulate the fact the that attacker can detect ocalls and measure the elapsed time
       ExecutionState *tobeterm = state.branch();
       addedStates.push_back(tobeterm);
       state.ptreeNode->data = 0;
       std::pair<PTree::Node*, PTree::Node*> res = processTree->split(state.ptreeNode, tobeterm, &state);
       tobeterm->ptreeNode = res.first;
       state.ptreeNode = res.second;
       llvm::errs() << "copy of state " << &state << " to be terminated: " << tobeterm << "\n"; 
       RD *rdd = getrdmap(&state);
       RD *rddterm = newNode(tobeterm);
       tobeterm->rdid = rddterm->stateid;
       rdmapinsert(tobeterm->rdid, rddterm);
       RD *rddreset =  newNode(&state);
       state.rdid = rddreset->stateid;
       rdmapinsert(state.rdid, rddreset);
       // This ensures that time side channel analysis can be started from this node as well
       resourceTreeRootList.insert(rddreset);
       rdd->succ->insert(rddterm);
       rdd->succ->insert(rddreset);
       terminateStateOnExit(*tobeterm);
       llvm::errs() << "making ocall " << f->getName() << " from " << ((Function*)i->getParent()->getParent())->getName() << "\n";
       state.setLastEnclaveFunction(((Function*)i->getParent()->getParent())->getName());
       state.inEnclave = false;
    }
    /* side channel end */
    if (asyncMode) {
      if (isAsyncInitiate(f->getName())) {
         std::string asyncName = getAsyncFunction(f->getName());
         #ifdef VB
         llvm::outs() << "checking for async function name " << asyncName << "\n";
         #endif
         if (isAsync(asyncName)) {
            Function *asyncF =  moduleHandle->getFunction(asyncName);
            int c = state.initiateAsync(asyncF);
            #ifdef VB
            llvm::outs() << "async function " << asyncName << " initiated, count=" << c << "\n";
            #endif
            break;
         }
      }
      else if (isEnableEntry(f->getName())) {
         std::string asyncName = getEnableFunction(f->getName());
         #ifdef VB
         llvm::outs() << "checking for enabled function name " << asyncName << "\n";
         #endif
         if (isEnabled(asyncName)) {
            Function *asyncF =  moduleHandle->getFunction(asyncName);
            int c = state.initiateAsync(asyncF);
            #ifdef VB
            llvm::outs() << "enabled function " << asyncName << " initiated, count=" << c << "\n";
            #endif
            break;
         }
      }
    }
    else if (!asyncMode && f && isAsyncInitiate(f->getName())) {
     #ifdef VB
      llvm::outs() << "Ignoring async_initiate in single threaded mode\n";
     #endif
      break;
    }
    /* end SYSREL extension */

    // Skip debug intrinsics, we can't evaluate their metadata arguments.
    if (f && isDebugIntrinsic(f, kmodule))
      break;

    if (isa<InlineAsm>(fp)) {
       llvm::InlineAsm *iasm = dyn_cast<llvm::InlineAsm>(fp);
       if (symbolizeInlineAssembly) {
          //llvm::errs() << "handling inline assembly by symbolizing the return value " << iasm->getAsmString() << "\n";
          bool abort = false;
          symbolizeReturnValueForAsmInstruction(state, ki, iasm, abort);
          if (abort)
             terminateStateOnExecError(state, "failure to symbolize return value of inline assembly");
          break;
       }
       else {
          llvm::errs() << "inline assembly is unsupported: " << iasm->getAsmString() << "\n";
          terminateStateOnExecError(state, "inline assembly is unsupported");
          break;
       }
    }
    // evaluate arguments
    std::vector< ref<Expr> > arguments;
    arguments.reserve(numArgs);

    for (unsigned j=0; j<numArgs; ++j)
      arguments.push_back(eval(ki, j+1, state).value);

    if (f) {

       /* SYSREL extension */
       #ifdef VB
       llvm::outs() << "calling function " << f->getName() << "\n";
       #endif
       if (isAssemblyFunc(f->getName())) {
          #ifdef VB 
          llvm::outs() << "handling assembly function " << f->getName() << "\n";
          #endif
          callExternalFunction(state, ki, f, arguments);
          return;
       }
       /* SYSREL extension */

      const FunctionType *fType =
        dyn_cast<FunctionType>(cast<PointerType>(f->getType())->getElementType());
      const FunctionType *fpType =
        dyn_cast<FunctionType>(cast<PointerType>(fp->getType())->getElementType());

      // special case the call with a bitcast case
      if (fType != fpType) {
        assert(fType && fpType && "unable to get function type");

        // XXX check result coercion

        // XXX this really needs thought and validation
        unsigned i=0;
        for (std::vector< ref<Expr> >::iterator
               ai = arguments.begin(), ie = arguments.end();
             ai != ie; ++ai) {
          Expr::Width to, from = (*ai)->getWidth();

          if (i<fType->getNumParams()) {
            to = getWidthForLLVMType(fType->getParamType(i));

            if (from != to) {
              // XXX need to check other param attrs ?
              bool isSExt = cs.paramHasAttr(i+1, llvm::Attribute::SExt);
              if (isSExt) {
                arguments[i] = SExtExpr::create(arguments[i], to);
              } else {
                arguments[i] = ZExtExpr::create(arguments[i], to);
              }
            }
          }

          i++;
        }
      }

      executeCall(state, ki, f, arguments);
    } else {
      ref<Expr> v = eval(ki, 0, state).value;

      ExecutionState *free = &state;
      bool hasInvalid = false, first = true;

      /* XXX This is wasteful, no need to do a full evaluate since we
         have already got a value. But in the end the caches should
         handle it for us, albeit with some overhead. */
      do {
        ref<ConstantExpr> value;
        bool success = solver->getValue(*free, v, value);
        assert(success && "FIXME: Unhandled solver failure");
        (void) success;
        StatePair res = fork(*free, EqExpr::create(v, value), true);
        if (res.first) {
          uint64_t addr = value->getZExtValue();
          if (legalFunctions.count(addr)) {
            f = (Function*) addr;

            // Don't give warning on unique resolution
            if (res.second || !first)
              klee_warning_once(reinterpret_cast<void*>(addr),
                                "resolved symbolic function pointer to: %s",
                                f->getName().data());

            executeCall(*res.first, ki, f, arguments);
          } else {
            if (!hasInvalid) {
              terminateStateOnExecError(state, "invalid function pointer");
              hasInvalid = true;
            }
          }
        }

        first = false;
        free = res.second;
      } while (free);
    }
    break;
  }
  case Instruction::PHI: {
    ref<Expr> result = eval(ki, state.incomingBBIndex, state).value;
    bindLocal(ki, state, result);
    #ifdef INFOFLOW
    updateInfoFlowForPhi(state, getLocal(state,ki,state.incomingBBIndex), ki);
    #endif
    break;
  }

    // Special instructions
  case Instruction::Select: {
    // NOTE: It is not required that operands 1 and 2 be of scalar type.
    if (BreakSelectStmt) {
       ref<Expr> cond = eval(ki, 0, state).value;
       Executor::StatePair branches = fork(state, cond, false, true);
       recordMostRecentBranchInfo(state, i);
       if (branches.first) {
          ref<Expr> tExpr = eval(ki, 1, state).value;
          bindLocal(ki, *branches.first, tExpr);
          /* SYSREL side channel begin */
          successorsPaths->insert(branches.first); // Keeping track of the new paths
         /* SYSREL side channel end */
          llvm::errs() << "True branch of select: " << (*ki->inst) << " condition " << cond << "\n";
       }
       if (branches.second) {
          ref<Expr> fExpr = eval(ki, 2, state).value;
          bindLocal(ki, *branches.second, fExpr);
          /* SYSREL side channel begin */
          successorsPaths->insert(branches.second); // Keeping track of the new paths
          /* SYSREL side channel end */
          llvm::errs() << "False branch of select: " << (*ki->inst) << " condition !" << cond << "\n";
       }
    }
    else {
       ref<Expr> cond = eval(ki, 0, state).value;
       ref<Expr> tExpr = eval(ki, 1, state).value;
       ref<Expr> fExpr = eval(ki, 2, state).value;
       ref<Expr> result = SelectExpr::create(cond, tExpr, fExpr);
       bindLocal(ki, state, result);
       #ifdef INFOFLOW
       updateInfoFlowForBinary(state, getLocal(state, ki, 1), getLocal(state, ki, 2), ki);
       #endif
    }
    break;
  }

  case Instruction::VAArg:
    terminateStateOnExecError(state, "unexpected VAArg instruction");
    break;

    // Arithmetic / logical

  case Instruction::Add: {
    ref<Expr> left = eval(ki, 0, state).value;
    ref<Expr> right = eval(ki, 1, state).value;
    bindLocal(ki, state, AddExpr::create(left, right));
    #ifdef INFOFLOW 
    updateInfoFlowForBinary(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
    #endif
    break;
  }

  case Instruction::Sub: {
    ref<Expr> left = eval(ki, 0, state).value;
    ref<Expr> right = eval(ki, 1, state).value;
    bindLocal(ki, state, SubExpr::create(left, right));
    #ifdef INFOFLOW
    updateInfoFlowForBinary(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
    #endif
    break;
  }

  case Instruction::Mul: {
    ref<Expr> left = eval(ki, 0, state).value;
    ref<Expr> right = eval(ki, 1, state).value;
    bindLocal(ki, state, MulExpr::create(left, right));
    #ifdef INFOFLOW
    updateInfoFlowForBinary(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
    #endif
    break;
  }

  case Instruction::UDiv: {
    ref<Expr> left = eval(ki, 0, state).value;
    ref<Expr> right = eval(ki, 1, state).value;
    ref<Expr> result = UDivExpr::create(left, right);
    bindLocal(ki, state, result);
    #ifdef INFOFLOW
    updateInfoFlowForBinary(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
    #endif
    break;
  }

  case Instruction::SDiv: {
    ref<Expr> left = eval(ki, 0, state).value;
    ref<Expr> right = eval(ki, 1, state).value;
    ref<Expr> result = SDivExpr::create(left, right);
    bindLocal(ki, state, result);
    #ifdef INFOFLOW
    updateInfoFlowForBinary(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
    #endif
    break;
  }

  case Instruction::URem: {
    ref<Expr> left = eval(ki, 0, state).value;
    ref<Expr> right = eval(ki, 1, state).value;
    ref<Expr> result = URemExpr::create(left, right);
    bindLocal(ki, state, result);
    #ifdef INFOFLOW
    updateInfoFlowForBinary(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
    #endif
    break;
  }

  case Instruction::SRem: {
    ref<Expr> left = eval(ki, 0, state).value;
    ref<Expr> right = eval(ki, 1, state).value;
    ref<Expr> result = SRemExpr::create(left, right);
    bindLocal(ki, state, result);
    #ifdef INFOFLOW
    updateInfoFlowForBinary(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
    #endif
    break;
  }

  case Instruction::And: {
    ref<Expr> left = eval(ki, 0, state).value;
    ref<Expr> right = eval(ki, 1, state).value;
    ref<Expr> result = AndExpr::create(left, right);
    bindLocal(ki, state, result);
    #ifdef INFOFLOW 
    updateInfoFlowForBinary(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
    #endif
    break;
  }

  case Instruction::Or: {
    ref<Expr> left = eval(ki, 0, state).value;
    ref<Expr> right = eval(ki, 1, state).value;
    ref<Expr> result = OrExpr::create(left, right);
    bindLocal(ki, state, result);
    #ifdef INFOFLOW
    updateInfoFlowForBinary(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
    #endif
    break;
  }

  case Instruction::Xor: {
    ref<Expr> left = eval(ki, 0, state).value;
    ref<Expr> right = eval(ki, 1, state).value;
    ref<Expr> result = XorExpr::create(left, right);
    bindLocal(ki, state, result);
    #ifdef INFOFLOW
    updateInfoFlowForBinary(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
    #endif
    break;
  }

  case Instruction::Shl: {
    ref<Expr> left = eval(ki, 0, state).value;
    ref<Expr> right = eval(ki, 1, state).value;
    ref<Expr> result = ShlExpr::create(left, right);
    bindLocal(ki, state, result);
    #ifdef INFOFLOW
    updateInfoFlowForBinary(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
    #endif
    break;
  }

  case Instruction::LShr: {
    ref<Expr> left = eval(ki, 0, state).value;
    ref<Expr> right = eval(ki, 1, state).value;
    ref<Expr> result = LShrExpr::create(left, right);
    bindLocal(ki, state, result);
    #ifdef INFOFLOW
    updateInfoFlowForBinary(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
    #endif
    break;
  }

  case Instruction::AShr: {
    ref<Expr> left = eval(ki, 0, state).value;
    ref<Expr> right = eval(ki, 1, state).value;
    ref<Expr> result = AShrExpr::create(left, right);
    bindLocal(ki, state, result);
    #ifdef INFOFLOW 
    updateInfoFlowForBinary(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
    #endif
    break;
  }

    // Compare

  case Instruction::ICmp: {
    CmpInst *ci = cast<CmpInst>(i);
    ICmpInst *ii = cast<ICmpInst>(ci);

    switch(ii->getPredicate()) {
    case ICmpInst::ICMP_EQ: {
      ref<Expr> left = eval(ki, 0, state).value;
      ref<Expr> right = eval(ki, 1, state).value;
      ref<Expr> result = EqExpr::create(left, right);
      bindLocal(ki, state, result);
      #ifdef INFOFLOW
      updateInfoFlowForCmp(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
      #endif
      break;
    }

    case ICmpInst::ICMP_NE: {
      ref<Expr> left = eval(ki, 0, state).value;
      ref<Expr> right = eval(ki, 1, state).value;
      ref<Expr> result = NeExpr::create(left, right);
      bindLocal(ki, state, result);
      #ifdef INFOFLOW
      updateInfoFlowForCmp(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
      #endif
      break;
    }

    case ICmpInst::ICMP_UGT: {
      ref<Expr> left = eval(ki, 0, state).value;
      ref<Expr> right = eval(ki, 1, state).value;
      ref<Expr> result = UgtExpr::create(left, right);
      bindLocal(ki, state,result);
      #ifdef INFOFLOW
      updateInfoFlowForCmp(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
      #endif
      break;
    }

    case ICmpInst::ICMP_UGE: {
      ref<Expr> left = eval(ki, 0, state).value;
      ref<Expr> right = eval(ki, 1, state).value;
      ref<Expr> result = UgeExpr::create(left, right);
      bindLocal(ki, state, result);
      #ifdef INFOFLOW
      updateInfoFlowForCmp(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
      #endif
      break;
    }

    case ICmpInst::ICMP_ULT: {
      ref<Expr> left = eval(ki, 0, state).value;
      ref<Expr> right = eval(ki, 1, state).value;
      ref<Expr> result = UltExpr::create(left, right);
      bindLocal(ki, state, result);
      #ifdef INFOFLOW
      updateInfoFlowForCmp(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
      #endif
      break;
    }

    case ICmpInst::ICMP_ULE: {
      ref<Expr> left = eval(ki, 0, state).value;
      ref<Expr> right = eval(ki, 1, state).value;
      ref<Expr> result = UleExpr::create(left, right);
      bindLocal(ki, state, result);
      #ifdef INFOFLOW
      updateInfoFlowForCmp(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
      #endif
      break;
    }

    case ICmpInst::ICMP_SGT: {
      ref<Expr> left = eval(ki, 0, state).value;
      ref<Expr> right = eval(ki, 1, state).value;
      ref<Expr> result = SgtExpr::create(left, right);
      bindLocal(ki, state, result);
      #ifdef INFOFLOW
      updateInfoFlowForCmp(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
      #endif
      break;
    }

    case ICmpInst::ICMP_SGE: {
      ref<Expr> left = eval(ki, 0, state).value;
      ref<Expr> right = eval(ki, 1, state).value;
      ref<Expr> result = SgeExpr::create(left, right);
      bindLocal(ki, state, result);
      #ifdef INFOFLOW
      updateInfoFlowForCmp(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
      #endif
      break;
    }

    case ICmpInst::ICMP_SLT: {
      ref<Expr> left = eval(ki, 0, state).value;
      ref<Expr> right = eval(ki, 1, state).value;
      ref<Expr> result = SltExpr::create(left, right);
      bindLocal(ki, state, result);
      #ifdef INFOFLOW
      updateInfoFlowForCmp(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
      #endif
      break;
    }

    case ICmpInst::ICMP_SLE: {
      ref<Expr> left = eval(ki, 0, state).value;
      ref<Expr> right = eval(ki, 1, state).value;
      ref<Expr> result = SleExpr::create(left, right);
      bindLocal(ki, state, result);
      #ifdef INFOFLOW
      updateInfoFlowForCmp(state, getLocal(state, ki, 0), getLocal(state, ki, 1), ki);
      #endif
      break;
    }

    default:
      terminateStateOnExecError(state, "invalid ICmp predicate");
    }
    break;
  }

    // Memory instructions...
  case Instruction::Alloca: {
    AllocaInst *ai = cast<AllocaInst>(i);
    unsigned elementSize =
      kmodule->targetData->getTypeStoreSize(ai->getAllocatedType());
    ref<Expr> size = Expr::createPointer(elementSize);
    /* SYSREL EXTENSION */
    /*int count = 0;
    bool singleInstance;
    ai->print(llvm::outs());
    std::string type_str;
    llvm::raw_string_ostream rso(type_str);
    ai->getAllocatedType()->print(rso);
    llvm::outs() << "Allocation type: " << rso.str() << "\n";
    bool lazyInit = isAllocTypeLazyInit(ai->getAllocatedType(), singleInstance, count);
    if (lazyInit) {
       llvm::outs() << "Allocating a lazy init object\n";
       size_t allocationAlignment = getAllocationAlignment(ai);
       MemoryObject *mo =
           memory->allocate(dyn_cast<ConstantExpr>(size)->getZExtValue(), true, false,
                         state.prevPC->inst, allocationAlignment);
      if (!mo) {
         bindLocal(ki, state,
                ConstantExpr::alloc(0, Context::get().getPointerWidth()));
      }
      else {
         llvm::outs() << "\n making the allocated object with base " << mo->getBaseExpr() << " symbolic\n";
         executeMakeSymbolic(state, mo, (ai->getParent()->getParent()->getName() + std::string("_") + rso.str()).str());
         ObjectState *os = bindObjectInState(state, mo, true);
         os->initializeToRandom();
         bindLocal(ki, state, mo->getBaseExpr());
      }
      break;
    }
    */
    /* SYSREL EXTENSION */
    if (ai->isArrayAllocation()) {
      ref<Expr> count = eval(ki, 0, state).value;
      count = Expr::createZExtToPointerWidth(count);
      size = MulExpr::create(size, count);
    }
    executeAlloc(state, size, true, ki);
    break;
  }

  case Instruction::Load: {
    ref<Expr> base = eval(ki, 0, state).value;
    executeMemoryOperation(state, getLocal(state, ki, 0), ki->dest, false, base, 0, ki);
    break;
  }
  case Instruction::Store: {
    ref<Expr> base = eval(ki, 1, state).value;
    ref<Expr> value = eval(ki, 0, state).value; 
    executeMemoryOperation(state, getLocal(state, ki, 0), getLocal(state, ki, 1), true, base, value, ki);
    break;
  }

  case Instruction::GetElementPtr: {
    KGEPInstruction *kgepi = static_cast<KGEPInstruction*>(ki);
    ref<Expr> base = eval(ki, 0, state).value;
    ref<Expr> origbase = base;
    /* side channel */
    //bool high = false;
    //bool low = false;
    //if (isAHighAddress(state,base))
    //   high = true;  
    //if (isALowAddress(state,base)) 
    //   low = true;    
    /* side channel */
    #ifdef VB 
    //llvm::outs() << "GetElementPtr info:\n";
    //ki->inst->print(llvm::outs());
    //llvm::outs() << "\n initial base: " << base << "\n";
    #endif
    for (std::vector< std::pair<unsigned, uint64_t> >::iterator
           it = kgepi->indices.begin(), ie = kgepi->indices.end();
         it != ie; ++it) {
      uint64_t elementSize = it->second;
      ref<Expr> index = eval(ki, it->first, state).value;

      #ifdef VB
      llvm::errs() << "gep index: " << index << "\n";
      llvm::errs() << "gep pointer: " << Expr::createPointer(elementSize) << "\n";
      #endif

      /* SYSREL extension */ 
      /* side channel begin */
      if (state.inEnclave && exprHasSymRegion(state, index, true)) {
          llvm::errs() << "CRITICAL: Security sensitive index in GetElementPtr:\n";
          ki->inst->print(llvm::errs());
          llvm::errs() << index << "\n"; 
      }
      /* side channel end */
      /* SYSREL extension */ 

      
      base = AddExpr::create(base,
                             MulExpr::create(Expr::createSExtToPointerWidth(index),
                                             Expr::createPointer(elementSize)));

      #ifdef VB
      llvm::errs() << "gep base: " << base << "\n";
      #endif
    }
    if (kgepi->offset) {
      /* SYSREL extension */ 
      /* side channel begin */
      if (state.inEnclave && exprHasSymRegion(state, Expr::createPointer(kgepi->offset), true)) {
          llvm::errs() << "CRITICAL: Security sensitive offset in GetElementPtr:\n";
          ki->inst->print(llvm::errs());
          llvm::errs() << kgepi->offset << "\n";
      } 
      /* side channel end */
      /* SYSREL extension */ 
 
      base = AddExpr::create(base,
                             Expr::createPointer(kgepi->offset));
      #ifdef VB
      ki->inst->print(llvm::errs());  
      llvm::errs() << "geptr offset: " << kgepi->offset << "\n";
      llvm::errs() << "geptr base: " << base << "\n";
      #endif
    }
    #ifdef VB
    llvm::errs() << "geptr final base: " << base << "\n";
    #endif
    bindLocal(ki, state, base);

    if (PreferredResolution) {
       if (symIndexToMemBase.find((long)&state) != symIndexToMemBase.end()) {
             std::map<ref<Expr>, ref<Expr> > m = symIndexToMemBase[(long)&state];
           if (base != origbase || m.find(base) == m.end()) { 
              m[base] = origbase;
              symIndexToMemBase[(long)&state] = m;
              llvm::errs() << "recording mapping from geptr index " << base << " to " << origbase << " in state " << &state << "\n";
          }
       }
    }

    /* side channel */
    if (state.inEnclave && exprHasSymRegion(state, base, true)) {
       // Check for the possibility of the address base mapping to different cache lines for different security sensitive values
       // Let cache line size denoted by 2^L and memory address size denoted by N
       // Since N-L bits are used to decide the cache line that gets accessed, we will check the following formula
       // \exists high'. base >> L<> base[high'/high] >> L
       //llvm::errs() << "Checking for cache based side channel:\n";
       ref<Expr> rexpr = renameExpr(memory, base, true); 
       //llvm::errs() << "Renaming of " << base << ":\n" << rexpr << "\n";
       ref<Expr> diffcachelines;
       if (cacheLineMode) {
          //llvm::errs() << "address: " << base << " cache line size: " << pow(2,cacheLineBits) << "\n";
          ref<Expr> clexpr1 = LShrExpr::alloc(base, ConstantExpr::alloc(cacheLineBits, 64));
          ref<Expr> clexpr2 = LShrExpr::alloc(rexpr, ConstantExpr::alloc(cacheLineBits, 64));
          ref<Expr> cleqexpr = EqExpr::create(clexpr1, clexpr2);
          diffcachelines = NotExpr::create(cleqexpr);
       }
       else if (cacheBitmaskMode) {
          ref<Expr>  aexpr1 = AndExpr::create(base, ConstantExpr::alloc(cacheBitmask, base->getWidth()));
          ref<Expr>  aexpr2 = AndExpr::create(rexpr, ConstantExpr::alloc(cacheBitmask, base->getWidth()));
          ref<Expr> cbmeqexpr = EqExpr::create(aexpr1, aexpr2);
          diffcachelines = NotExpr::create(cbmeqexpr); 
       }
       else assert(false && "Need a model (cache line or bitmask) for checking cache side channels!\n");
       bool result;
       bool success = solver->mayBeTrue(state, diffcachelines, result);
       assert(success && "FIXME: Unhandled solver failure while checking feasibility of the projection");
       if (result) {
          std::string mode = cacheLineMode ?  "CACHE LINE" : "BIT MASK";
          llvm::errs() << "CACHE BASED SIDE CHANNEL: (MODE=" << mode << ")\n";
          ki->inst->print(llvm::errs());
          const InstructionInfo &ii = kmodule->infos->getInfo(ki->inst);
          std::pair<std::string, int> p = std::pair<std::string, int>(ii.file, ii.line);
          cachelocs.insert(p);
       }
    }
    #ifdef INFOFLOW
    ref<Expr> sizeExpr = SubExpr::create(base, origbase);
    ConstantExpr *cs = dyn_cast<ConstantExpr>(sizeExpr);
    if (cs)
      updateInfoFlowForGeptr(state, getLocal(state, ki, 0), ki, kgepi->offset + cs->getZExtValue());
    else // under-approx
      updateInfoFlowForGeptr(state, getLocal(state, ki, 0), ki, kgepi->offset);
    #endif
    /*if (high) {
       addHighAddress(state,base);
       #ifdef VB 
       llvm::errs() << "recording address " << base << " as transitive security sensitive \n";
       #endif
    }
    if (low) {
       addLowAddress(state,base);
       #ifdef VB 
       llvm::errs() << "recording address " << base << " as transitive security insensitive \n";
       #endif
    }*/
    /* side channel */
    break;
  }

    // Conversion
  case Instruction::Trunc: {
    CastInst *ci = cast<CastInst>(i);
    ref<Expr> result = ExtractExpr::create(eval(ki, 0, state).value,
                                           0,
                                           getWidthForLLVMType(ci->getType()));
    bindLocal(ki, state, result);
    #ifdef INFOFLOW
    updateInfoFlowForExtract(state, getLocal(state, ki, 0), ki, 0, getWidthForLLVMType(ci->getType()));
    #endif
    break;
  }
  case Instruction::ZExt: {
    CastInst *ci = cast<CastInst>(i);
    ref<Expr> result = ZExtExpr::create(eval(ki, 0, state).value,
                                        getWidthForLLVMType(ci->getType()));
    bindLocal(ki, state, result);
    #ifdef INFOFLOW
    updateInfoFlowForExt(state, getLocal(state, ki, 0), ki);
    #endif
    break;
  }
  case Instruction::SExt: {
    CastInst *ci = cast<CastInst>(i);
    ref<Expr> result = SExtExpr::create(eval(ki, 0, state).value,
                                        getWidthForLLVMType(ci->getType()));
    bindLocal(ki, state, result);
    #ifdef INFOFLOW
    updateInfoFlowForExt(state, getLocal(state, ki, 0), ki);
    #endif
    break;
  }

  case Instruction::IntToPtr: {
    CastInst *ci = cast<CastInst>(i);
    Expr::Width pType = getWidthForLLVMType(ci->getType());
    ref<Expr> arg = eval(ki, 0, state).value;
    bindLocal(ki, state, ZExtExpr::create(arg, pType));
    #ifdef INFOFLOW
    updateInfoFlowForExt(state, getLocal(state, ki, 0), ki);
    #endif
    break;
  }
  case Instruction::PtrToInt: {
    CastInst *ci = cast<CastInst>(i);
    Expr::Width iType = getWidthForLLVMType(ci->getType());
    ref<Expr> arg = eval(ki, 0, state).value;
    bindLocal(ki, state, ZExtExpr::create(arg, iType));
    #ifdef INFOFLOW
    updateInfoFlowForExt(state, getLocal(state, ki, 0), ki);
    #endif
    break;
  }

  case Instruction::BitCast: {
    DataLayout *TD = kmodule->targetData;
    //llvm::errs() << (*ki->inst) << "\n"; 
    //llvm::errs() << "bitcast to " << getTypeName(ki->inst->getType()) << " size: " << TD->getTypeAllocSize(ki->inst->getType()) << "\n";
    if (((llvm::CastInst*)ki->inst)->getSrcTy()->isPointerTy()) {
       Type *st = ((llvm::CastInst*)ki->inst)->getSrcTy()->getPointerElementType();
       if (st->isPointerTy())
          st = st->getPointerElementType();
       //llvm::errs() << "source type base element size: " << TD->getTypeAllocSize(st) << "\n";
    }
    ref<Expr> result = eval(ki, 0, state).value;
    bindLocal(ki, state, result);
    #ifdef INFOFLOW
    updateInfoFlowForExt(state, getLocal(state, ki, 0), ki);
    #endif
    /* SYSREL EXTENSION */
   if (lazyInit) {
    #ifdef VB
    llvm::outs() <<  (*(ki->inst)) << "\n";
    llvm::outs() << "num ops " << getTypeName(ki->inst->getType()) << "\n";
    #endif
    Type *t = ki->inst->getType();
    if (t->isPointerTy()) {
       t = t->getPointerElementType();
       if (!t->isPointerTy()) {
          StructType *st = dyn_cast<StructType>(t);
          if (st) {
             std::string tname = getTypeName(t);
             state.typeToAddr[t] = result;

             #ifdef VB
             llvm::errs() << "mapping bitcast instance of " << tname << " to " << result << " type pointer=" << t << " in " << ki->inst << "\n";
             #endif
          }
       }
    }
   }
    /* SYSREL EXTENSION */

    break;
  }

    // Floating point instructions

  case Instruction::FAdd: {
    ref<ConstantExpr> left = toConstant(state, eval(ki, 0, state).value,
                                        "floating point");
    ref<ConstantExpr> right = toConstant(state, eval(ki, 1, state).value,
                                         "floating point");
    if (!fpWidthToSemantics(left->getWidth()) ||
        !fpWidthToSemantics(right->getWidth()))
      return terminateStateOnExecError(state, "Unsupported FAdd operation");

    llvm::APFloat Res(*fpWidthToSemantics(left->getWidth()), left->getAPValue());
    Res.add(APFloat(*fpWidthToSemantics(right->getWidth()),right->getAPValue()), APFloat::rmNearestTiesToEven);
    bindLocal(ki, state, ConstantExpr::alloc(Res.bitcastToAPInt()));
    break;
  }

  case Instruction::FSub: {
    ref<ConstantExpr> left = toConstant(state, eval(ki, 0, state).value,
                                        "floating point");
    ref<ConstantExpr> right = toConstant(state, eval(ki, 1, state).value,
                                         "floating point");
    if (!fpWidthToSemantics(left->getWidth()) ||
        !fpWidthToSemantics(right->getWidth()))
      return terminateStateOnExecError(state, "Unsupported FSub operation");
    llvm::APFloat Res(*fpWidthToSemantics(left->getWidth()), left->getAPValue());
    Res.subtract(APFloat(*fpWidthToSemantics(right->getWidth()), right->getAPValue()), APFloat::rmNearestTiesToEven);
    bindLocal(ki, state, ConstantExpr::alloc(Res.bitcastToAPInt()));
    break;
  }

  case Instruction::FMul: {
    ref<ConstantExpr> left = toConstant(state, eval(ki, 0, state).value,
                                        "floating point");
    ref<ConstantExpr> right = toConstant(state, eval(ki, 1, state).value,
                                         "floating point");
    if (!fpWidthToSemantics(left->getWidth()) ||
        !fpWidthToSemantics(right->getWidth()))
      return terminateStateOnExecError(state, "Unsupported FMul operation");

    llvm::APFloat Res(*fpWidthToSemantics(left->getWidth()), left->getAPValue());
    Res.multiply(APFloat(*fpWidthToSemantics(right->getWidth()), right->getAPValue()), APFloat::rmNearestTiesToEven);
    bindLocal(ki, state, ConstantExpr::alloc(Res.bitcastToAPInt()));
    break;
  }

  case Instruction::FDiv: {
    ref<ConstantExpr> left = toConstant(state, eval(ki, 0, state).value,
                                        "floating point");
    ref<ConstantExpr> right = toConstant(state, eval(ki, 1, state).value,
                                         "floating point");
    if (!fpWidthToSemantics(left->getWidth()) ||
        !fpWidthToSemantics(right->getWidth()))
      return terminateStateOnExecError(state, "Unsupported FDiv operation");

    llvm::APFloat Res(*fpWidthToSemantics(left->getWidth()), left->getAPValue());
    Res.divide(APFloat(*fpWidthToSemantics(right->getWidth()), right->getAPValue()), APFloat::rmNearestTiesToEven);
    bindLocal(ki, state, ConstantExpr::alloc(Res.bitcastToAPInt()));
    break;
  }

  case Instruction::FRem: {
    ref<ConstantExpr> left = toConstant(state, eval(ki, 0, state).value,
                                        "floating point");
    ref<ConstantExpr> right = toConstant(state, eval(ki, 1, state).value,
                                         "floating point");
    if (!fpWidthToSemantics(left->getWidth()) ||
        !fpWidthToSemantics(right->getWidth()))
      return terminateStateOnExecError(state, "Unsupported FRem operation");
    llvm::APFloat Res(*fpWidthToSemantics(left->getWidth()), left->getAPValue());
#if LLVM_VERSION_CODE >= LLVM_VERSION(3, 8)
    Res.mod(
        APFloat(*fpWidthToSemantics(right->getWidth()), right->getAPValue()));
#else
    Res.mod(APFloat(*fpWidthToSemantics(right->getWidth()),right->getAPValue()),
            APFloat::rmNearestTiesToEven);
#endif
    bindLocal(ki, state, ConstantExpr::alloc(Res.bitcastToAPInt()));
    break;
  }

  case Instruction::FPTrunc: {
    FPTruncInst *fi = cast<FPTruncInst>(i);
    Expr::Width resultType = getWidthForLLVMType(fi->getType());
    ref<ConstantExpr> arg = toConstant(state, eval(ki, 0, state).value,
                                       "floating point");
    if (!fpWidthToSemantics(arg->getWidth()) || resultType > arg->getWidth())
      return terminateStateOnExecError(state, "Unsupported FPTrunc operation");

    llvm::APFloat Res(*fpWidthToSemantics(arg->getWidth()), arg->getAPValue());
    bool losesInfo = false;
    Res.convert(*fpWidthToSemantics(resultType),
                llvm::APFloat::rmNearestTiesToEven,
                &losesInfo);
    bindLocal(ki, state, ConstantExpr::alloc(Res));
    break;
  }

  case Instruction::FPExt: {
    FPExtInst *fi = cast<FPExtInst>(i);
    Expr::Width resultType = getWidthForLLVMType(fi->getType());
    ref<ConstantExpr> arg = toConstant(state, eval(ki, 0, state).value,
                                        "floating point");
    if (!fpWidthToSemantics(arg->getWidth()) || arg->getWidth() > resultType)
      return terminateStateOnExecError(state, "Unsupported FPExt operation");
    llvm::APFloat Res(*fpWidthToSemantics(arg->getWidth()), arg->getAPValue());
    bool losesInfo = false;
    Res.convert(*fpWidthToSemantics(resultType),
                llvm::APFloat::rmNearestTiesToEven,
                &losesInfo);
    bindLocal(ki, state, ConstantExpr::alloc(Res));
    break;
  }

  case Instruction::FPToUI: {
    FPToUIInst *fi = cast<FPToUIInst>(i);
    Expr::Width resultType = getWidthForLLVMType(fi->getType());
    ref<ConstantExpr> arg = toConstant(state, eval(ki, 0, state).value,
                                       "floating point");
    if (!fpWidthToSemantics(arg->getWidth()) || resultType > 64)
      return terminateStateOnExecError(state, "Unsupported FPToUI operation");

    llvm::APFloat Arg(*fpWidthToSemantics(arg->getWidth()), arg->getAPValue());
    uint64_t value = 0;
    bool isExact = true;
    Arg.convertToInteger(&value, resultType, false,
                         llvm::APFloat::rmTowardZero, &isExact);
    bindLocal(ki, state, ConstantExpr::alloc(value, resultType));
    break;
  }

  case Instruction::FPToSI: {
    FPToSIInst *fi = cast<FPToSIInst>(i);
    Expr::Width resultType = getWidthForLLVMType(fi->getType());
    ref<ConstantExpr> arg = toConstant(state, eval(ki, 0, state).value,
                                       "floating point");
    if (!fpWidthToSemantics(arg->getWidth()) || resultType > 64)
      return terminateStateOnExecError(state, "Unsupported FPToSI operation");
    llvm::APFloat Arg(*fpWidthToSemantics(arg->getWidth()), arg->getAPValue());

    uint64_t value = 0;
    bool isExact = true;
    Arg.convertToInteger(&value, resultType, true,
                         llvm::APFloat::rmTowardZero, &isExact);
    bindLocal(ki, state, ConstantExpr::alloc(value, resultType));
    break;
  }

  case Instruction::UIToFP: {
    UIToFPInst *fi = cast<UIToFPInst>(i);
    Expr::Width resultType = getWidthForLLVMType(fi->getType());
    ref<ConstantExpr> arg = toConstant(state, eval(ki, 0, state).value,
                                       "floating point");
    const llvm::fltSemantics *semantics = fpWidthToSemantics(resultType);
    if (!semantics)
      return terminateStateOnExecError(state, "Unsupported UIToFP operation");
    llvm::APFloat f(*semantics, 0);
    f.convertFromAPInt(arg->getAPValue(), false,
                       llvm::APFloat::rmNearestTiesToEven);

    bindLocal(ki, state, ConstantExpr::alloc(f));
    break;
  }

  case Instruction::SIToFP: {
    SIToFPInst *fi = cast<SIToFPInst>(i);
    Expr::Width resultType = getWidthForLLVMType(fi->getType());
    ref<ConstantExpr> arg = toConstant(state, eval(ki, 0, state).value,
                                       "floating point");
    const llvm::fltSemantics *semantics = fpWidthToSemantics(resultType);
    if (!semantics)
      return terminateStateOnExecError(state, "Unsupported SIToFP operation");
    llvm::APFloat f(*semantics, 0);
    f.convertFromAPInt(arg->getAPValue(), true,
                       llvm::APFloat::rmNearestTiesToEven);

    bindLocal(ki, state, ConstantExpr::alloc(f));
    break;
  }

  case Instruction::FCmp: {
    FCmpInst *fi = cast<FCmpInst>(i);
    ref<ConstantExpr> left = toConstant(state, eval(ki, 0, state).value,
                                        "floating point");
    ref<ConstantExpr> right = toConstant(state, eval(ki, 1, state).value,
                                         "floating point");
    if (!fpWidthToSemantics(left->getWidth()) ||
        !fpWidthToSemantics(right->getWidth()))
      return terminateStateOnExecError(state, "Unsupported FCmp operation");

    APFloat LHS(*fpWidthToSemantics(left->getWidth()),left->getAPValue());
    APFloat RHS(*fpWidthToSemantics(right->getWidth()),right->getAPValue());
    APFloat::cmpResult CmpRes = LHS.compare(RHS);

    bool Result = false;
    switch( fi->getPredicate() ) {
      // Predicates which only care about whether or not the operands are NaNs.
    case FCmpInst::FCMP_ORD:
      Result = (CmpRes != APFloat::cmpUnordered);
      break;

    case FCmpInst::FCMP_UNO:
      Result = (CmpRes == APFloat::cmpUnordered);
      break;

      // Ordered comparisons return false if either operand is NaN.  Unordered
      // comparisons return true if either operand is NaN.
    case FCmpInst::FCMP_UEQ:
      Result = (CmpRes == APFloat::cmpUnordered || CmpRes == APFloat::cmpEqual);
      break;
    case FCmpInst::FCMP_OEQ:
      Result = (CmpRes != APFloat::cmpUnordered && CmpRes == APFloat::cmpEqual);
      break;

    case FCmpInst::FCMP_UGT:
      Result = (CmpRes == APFloat::cmpUnordered || CmpRes == APFloat::cmpGreaterThan);
      break;
    case FCmpInst::FCMP_OGT:
      Result = (CmpRes != APFloat::cmpUnordered && CmpRes == APFloat::cmpGreaterThan);
      break;

    case FCmpInst::FCMP_UGE:
      Result = (CmpRes == APFloat::cmpUnordered || (CmpRes == APFloat::cmpGreaterThan || CmpRes == APFloat::cmpEqual));
      break;
    case FCmpInst::FCMP_OGE:
      Result = (CmpRes != APFloat::cmpUnordered && (CmpRes == APFloat::cmpGreaterThan || CmpRes == APFloat::cmpEqual));
      break;

    case FCmpInst::FCMP_ULT:
      Result = (CmpRes == APFloat::cmpUnordered || CmpRes == APFloat::cmpLessThan);
      break;
    case FCmpInst::FCMP_OLT:
      Result = (CmpRes != APFloat::cmpUnordered && CmpRes == APFloat::cmpLessThan);
      break;

    case FCmpInst::FCMP_ULE:
      Result = (CmpRes == APFloat::cmpUnordered || (CmpRes == APFloat::cmpLessThan || CmpRes == APFloat::cmpEqual));
      break;
    case FCmpInst::FCMP_OLE:
      Result = (CmpRes != APFloat::cmpUnordered && (CmpRes == APFloat::cmpLessThan || CmpRes == APFloat::cmpEqual));
      break;

    case FCmpInst::FCMP_UNE:
      Result = (CmpRes == APFloat::cmpUnordered || CmpRes != APFloat::cmpEqual);
      break;
    case FCmpInst::FCMP_ONE:
      Result = (CmpRes != APFloat::cmpUnordered && CmpRes != APFloat::cmpEqual);
      break;

    default:
      assert(0 && "Invalid FCMP predicate!");
      break;
    case FCmpInst::FCMP_FALSE:
      Result = false;
      break;
    case FCmpInst::FCMP_TRUE:
      Result = true;
      break;
    }

    bindLocal(ki, state, ConstantExpr::alloc(Result, Expr::Bool));
    break;
  }
  case Instruction::InsertValue: {
    KGEPInstruction *kgepi = static_cast<KGEPInstruction*>(ki);

    ref<Expr> agg = eval(ki, 0, state).value;
    ref<Expr> val = eval(ki, 1, state).value;

    ref<Expr> l = NULL, r = NULL;
    unsigned lOffset = kgepi->offset*8, rOffset = kgepi->offset*8 + val->getWidth();

    if (lOffset > 0)
      l = ExtractExpr::create(agg, 0, lOffset);
    if (rOffset < agg->getWidth())
      r = ExtractExpr::create(agg, rOffset, agg->getWidth() - rOffset);

    ref<Expr> result;
    if (!l.isNull() && !r.isNull())
      result = ConcatExpr::create(r, ConcatExpr::create(val, l));
    else if (!l.isNull())
      result = ConcatExpr::create(val, l);
    else if (!r.isNull())
      result = ConcatExpr::create(r, val);
    else
      result = val;

    bindLocal(ki, state, result);
    break;
  }
  case Instruction::ExtractValue: {
    KGEPInstruction *kgepi = static_cast<KGEPInstruction*>(ki);

    ref<Expr> agg = eval(ki, 0, state).value;

    ref<Expr> result = ExtractExpr::create(agg, kgepi->offset*8, getWidthForLLVMType(i->getType()));

    bindLocal(ki, state, result);
    #ifdef INFOFLOW
    updateInfoFlowForExtract(state, getLocal(state, ki, 0), ki, kgepi->offset*8, getWidthForLLVMType(i->getType()));
    #endif
    break;
  }
  case Instruction::Fence: {
    // Ignore for now
    break;
  }
  case Instruction::InsertElement: {
    InsertElementInst *iei = cast<InsertElementInst>(i);
    ref<Expr> vec = eval(ki, 0, state).value;
    ref<Expr> newElt = eval(ki, 1, state).value;
    ref<Expr> idx = eval(ki, 2, state).value;

    ConstantExpr *cIdx = dyn_cast<ConstantExpr>(idx);
    if (cIdx == NULL) {
      terminateStateOnError(
          state, "InsertElement, support for symbolic index not implemented",
          Unhandled);
      return;
    }
    uint64_t iIdx = cIdx->getZExtValue();
    const llvm::VectorType *vt = iei->getType();
    unsigned EltBits = getWidthForLLVMType(vt->getElementType());

    if (iIdx >= vt->getNumElements()) {
      // Out of bounds write
      terminateStateOnError(state, "Out of bounds write when inserting element",
                            BadVectorAccess);
      return;
    }

    const unsigned elementCount = vt->getNumElements();
    llvm::SmallVector<ref<Expr>, 8> elems;
    elems.reserve(elementCount);
    for (unsigned i = elementCount; i != 0; --i) {
      auto of = i - 1;
      unsigned bitOffset = EltBits * of;
      elems.push_back(
          of == iIdx ? newElt : ExtractExpr::create(vec, bitOffset, EltBits));
    }

    assert(Context::get().isLittleEndian() && "FIXME:Broken for big endian");
    ref<Expr> Result = ConcatExpr::createN(elementCount, elems.data());
    bindLocal(ki, state, Result);
    break;
  }
  case Instruction::ExtractElement: {
    ExtractElementInst *eei = cast<ExtractElementInst>(i);
    ref<Expr> vec = eval(ki, 0, state).value;
    ref<Expr> idx = eval(ki, 1, state).value;

    ConstantExpr *cIdx = dyn_cast<ConstantExpr>(idx);
    if (cIdx == NULL) {
      terminateStateOnError(
          state, "ExtractElement, support for symbolic index not implemented",
          Unhandled);
      return;
    }
    uint64_t iIdx = cIdx->getZExtValue();
    const llvm::VectorType *vt = eei->getVectorOperandType();
    unsigned EltBits = getWidthForLLVMType(vt->getElementType());

    if (iIdx >= vt->getNumElements()) {
      // Out of bounds read
      terminateStateOnError(state, "Out of bounds read when extracting element",
                            BadVectorAccess);
      return;
    }

    unsigned bitOffset = EltBits * iIdx;
    ref<Expr> Result = ExtractExpr::create(vec, bitOffset, EltBits);
    bindLocal(ki, state, Result);
    #ifdef INFOFLOW
    updateInfoFlowForExtract(state, getLocal(state, ki, 0), ki, bitOffset, EltBits);
    #endif
    break;
  }
  case Instruction::ShuffleVector:
    // Should never happen due to Scalarizer pass removing ShuffleVector
    // instructions.
    terminateStateOnExecError(state, "Unexpected ShuffleVector instruction");
    break;
  // Other instructions...
  // Unhandled
  default:
    terminateStateOnExecError(state, "illegal instruction");
    break;
  }
}

void Executor::updateStates(ExecutionState *current) {

 
  // SYSREL extension
  // since hasHLoc is evaluated later, we will refer to these lists to update the statistics 
  if (pauseSecretIndependent) {
     if (stats::sensitiveDesc.getValue() > 0) {
         for (std::vector<ExecutionState *>::iterator it = addedStates.begin(),
                                                  ie = addedStates.end();
             it != ie; ++it) {
             if (!(*it)->isSecretDescendant()) {
                pausedStates.push_back(*it);            
             }
         }     
         for (std::vector<ExecutionState *>::iterator it = pausedStates.begin(),
                                                  ie = pausedStates.end(); 
             it != ie; ++it) {
             std::vector<ExecutionState *>::iterator temp = std::find(addedStates.begin(),addedStates.end(),*it);
             if (temp != addedStates.end())
                addedStates.erase(temp);
         }
     }
     else {
         for (std::vector<ExecutionState *>::iterator it = pausedStates.begin(),
                                                  ie = pausedStates.end(); 
             it != ie; ++it) {
             continuedStates.push_back(*it);
         }
         pausedStates.clear();
     }
  }
  addedStates_copy = addedStates;
  removedStates_copy = removedStates;
  // SYSREL extension  


  if (searcher) {
    searcher->update(current, addedStates, removedStates);
    searcher->update(nullptr, continuedStates, pausedStates);
    //pausedStates.clear();
    //continuedStates.clear();
  }

  states.insert(addedStates.begin(), addedStates.end());
  addedStates.clear();

  for (std::vector<ExecutionState *>::iterator it = removedStates.begin(),
                                               ie = removedStates.end();
       it != ie; ++it) {
    ExecutionState *es = *it;
    std::set<ExecutionState*>::iterator it2 = states.find(es);
    //assert(it2!=states.end());
    // SYSREL extension 
    if (it2!=states.end()) {
    states.erase(it2);
    std::map<ExecutionState*, std::vector<SeedInfo> >::iterator it3 =
      seedMap.find(es);
    if (it3 != seedMap.end())
      seedMap.erase(it3);
    processTree->remove(es->ptreeNode);
    delete es;
    } // SYSREL extension 
  }
  removedStates.clear();
}

template <typename TypeIt>
void Executor::computeOffsets(KGEPInstruction *kgepi, TypeIt ib, TypeIt ie) {
  ref<ConstantExpr> constantOffset =
    ConstantExpr::alloc(0, Context::get().getPointerWidth());
  uint64_t index = 1;
  for (TypeIt ii = ib; ii != ie; ++ii) {
    if (StructType *st = dyn_cast<StructType>(*ii)) {
      const StructLayout *sl = kmodule->targetData->getStructLayout(st);
      const ConstantInt *ci = cast<ConstantInt>(ii.getOperand());
      uint64_t addend = sl->getElementOffset((unsigned) ci->getZExtValue());
      constantOffset = constantOffset->Add(ConstantExpr::alloc(addend,
                                                               Context::get().getPointerWidth()));
    } else {
      const SequentialType *set = cast<SequentialType>(*ii);
      uint64_t elementSize =
        kmodule->targetData->getTypeStoreSize(set->getElementType());
      Value *operand = ii.getOperand();
      if (Constant *c = dyn_cast<Constant>(operand)) {
        ref<ConstantExpr> index =
          evalConstant(c)->SExt(Context::get().getPointerWidth());
        ref<ConstantExpr> addend =
          index->Mul(ConstantExpr::alloc(elementSize,
                                         Context::get().getPointerWidth()));
        constantOffset = constantOffset->Add(addend);
      } else {
        kgepi->indices.push_back(std::make_pair(index, elementSize));
      }
    }
    index++;
  }
  kgepi->offset = constantOffset->getZExtValue();
}

void Executor::bindInstructionConstants(KInstruction *KI) {
  KGEPInstruction *kgepi = static_cast<KGEPInstruction*>(KI);

  if (GetElementPtrInst *gepi = dyn_cast<GetElementPtrInst>(KI->inst)) {
    computeOffsets(kgepi, gep_type_begin(gepi), gep_type_end(gepi));
  } else if (InsertValueInst *ivi = dyn_cast<InsertValueInst>(KI->inst)) {
    computeOffsets(kgepi, iv_type_begin(ivi), iv_type_end(ivi));
    assert(kgepi->indices.empty() && "InsertValue constant offset expected");
  } else if (ExtractValueInst *evi = dyn_cast<ExtractValueInst>(KI->inst)) {
    computeOffsets(kgepi, ev_type_begin(evi), ev_type_end(evi));
    assert(kgepi->indices.empty() && "ExtractValue constant offset expected");
  }
}

void Executor::bindModuleConstants() {
  for (std::vector<KFunction*>::iterator it = kmodule->functions.begin(),
         ie = kmodule->functions.end(); it != ie; ++it) {
    KFunction *kf = *it;
    for (unsigned i=0; i<kf->numInstructions; ++i)
      bindInstructionConstants(kf->instructions[i]);
  }

  kmodule->constantTable = new Cell[kmodule->constants.size()];
  for (unsigned i=0; i<kmodule->constants.size(); ++i) {
    Cell &c = kmodule->constantTable[i];
    //llvm::errs() << "module constant " << kmodule->constants[i] << "\n";
    c.value = evalConstant(kmodule->constants[i]);
  }
}

void Executor::checkMemoryUsage() {
  if (!MaxMemory)
    return;
  if ((stats::instructions & 0xFFFF) == 0) {
    // We need to avoid calling GetTotalMallocUsage() often because it
    // is O(elts on freelist). This is really bad since we start
    // to pummel the freelist once we hit the memory cap.
    unsigned mbs = (util::GetTotalMallocUsage() >> 20) +
                   (memory->getUsedDeterministicSize() >> 20);

    if (mbs > MaxMemory) {
      if (mbs > MaxMemory + 100) {
        // just guess at how many to kill
        unsigned numStates = states.size();
        unsigned toKill = std::max(1U, numStates - numStates * MaxMemory / mbs);
        klee_warning("killing %d states (over memory cap)", toKill);
        std::vector<ExecutionState *> arr(states.begin(), states.end());
        for (unsigned i = 0, N = arr.size(); N && i < toKill; ++i, --N) {
          unsigned idx = rand() % N;
          // Make two pulls to try and not hit a state that
          // covered new code.
          if (arr[idx]->coveredNew)
            idx = rand() % N;

          std::swap(arr[idx], arr[N - 1]);
          terminateStateEarly(*arr[N - 1], "Memory limit exceeded.");
        }
      }
      atMemoryLimit = true;
    } else {
      atMemoryLimit = false;
    }
  }
}

void Executor::doDumpStates() {
  if (!DumpStatesOnHalt || states.empty())
    return;

  klee_message("halting execution, dumping remaining states");
  for (const auto &state : states)
    terminateStateEarly(*state, "Execution halting.");
  updateStates(nullptr);
}

void Executor::run(ExecutionState &initialState) {
  bindModuleConstants();

  // Delay init till now so that ticks don't accrue during
  // optimization and such.
  initTimers();

  states.insert(&initialState);
  /* SYSREL side channel begin */
  if (initialState.hasLCM()) {
     Sequential *lcmModel = LifeCycleModelState::lcm;
     publicOutputReturningFName = lcmModel->getStep(lcmModel->getNumSteps()-1)->getValue();
     sidechannelstarted = true;
  }
  else {
     publicOutputReturningFName = entryFunctionName;
     sidechannelstarted = true;
  }
  llvm::errs() << "Registering publicOutputReturningFName as " << publicOutputReturningFName << "\n";
  bool validOutputExp = false;
  MemoryObject  *rmo = NULL;
  publicOutputVar = initialState.addSymbolicReturnAsPublicOutput(publicOutputReturningFName, publicOutputVarName, memory, arrayCache, rmo, validOutputExp);
  if (validOutputExp) {
     region r;
     r.offset = 0;
     r.size = rmo->size * 8;
     std::vector<region> rs;
     rs.push_back(r);
     setLowMemoryRegion(initialState, rmo->getBaseExpr(), rs); 
     setLowSymRegion(publicOutputVarName, rs);
  }
  root = newNode(&initialState);
  initialState.rdid = root->stateid;
  rdmapinsert(initialState.rdid, root);
  #ifdef VBSC
    printResourceUsage(root);
  #endif
  currentRD = root;
  resourceTreeRootList.insert(root);
  /* SYSREL side channel end */
  if (usingSeeds) {
    std::vector<SeedInfo> &v = seedMap[&initialState];

    for (std::vector<KTest*>::const_iterator it = usingSeeds->begin(),
           ie = usingSeeds->end(); it != ie; ++it)
      v.push_back(SeedInfo(*it));

    int lastNumSeeds = usingSeeds->size()+10;
    double lastTime, startTime = lastTime = util::getWallTime();
    ExecutionState *lastState = 0;
    while (!seedMap.empty()) {
      if (haltExecution) {
        doDumpStates();
        llvm::errs() << "Halting execution!!!\n";
        return;
      }

      std::map<ExecutionState*, std::vector<SeedInfo> >::iterator it =
        seedMap.upper_bound(lastState);
      if (it == seedMap.end())
        it = seedMap.begin();
      lastState = it->first;
      unsigned numSeeds = it->second.size();
      ExecutionState &state = *lastState;
      #ifdef INFOFLOW
      updateCurrentInfoFlowContext(state);
      #endif
      KInstruction *ki = state.pc;
      stepInstruction(state);
      /* SYSREL extension */
      singleSuccessor = true;
      /* SYSREL extension */
      executeInstruction(state, ki);
      /* SYSREL extension */
      // First check if any programming model related action to be executed
      if (singleSuccessor) {
         bool abort = false;
         state.executePM(abort);
         if (abort) {
            terminateStateOnError(state, "Memory error", Ptr, NULL, "Possible use-after-free");
         }
      }
      /* SYSREL extension */
      processTimers(&state, MaxInstructionTime * numSeeds);
      updateStates(&state);

      if ((stats::instructions % 1000) == 0) {
        int numSeeds = 0, numStates = 0;
        for (std::map<ExecutionState*, std::vector<SeedInfo> >::iterator
               it = seedMap.begin(), ie = seedMap.end();
             it != ie; ++it) {
          numSeeds += it->second.size();
          numStates++;
        }
        double time = util::getWallTime();
        if (SeedTime>0. && time > startTime + SeedTime) {
          klee_warning("seed time expired, %d seeds remain over %d states",
                       numSeeds, numStates);
          break;
        } else if (numSeeds<=lastNumSeeds-10 ||
                   time >= lastTime+10) {
          lastTime = time;
          lastNumSeeds = numSeeds;
          klee_message("%d seeds remaining over: %d states",
                       numSeeds, numStates);
        }
      }
    }

    klee_message("seeding done (%d states remain)", (int) states.size());

    // XXX total hack, just because I like non uniform better but want
    // seed results to be equally weighted.
    for (std::set<ExecutionState*>::iterator
           it = states.begin(), ie = states.end();
         it != ie; ++it) {
      (*it)->weight = 1.;
    }

    if (OnlySeed) {
      doDumpStates();
      return;
    }
  }

  searcher = constructUserSearcher(*this);
  llvm::errs() << "Using searcher "; 
  searcher->printName(llvm::errs());
  llvm::errs() << "\n";

  std::vector<ExecutionState *> newStates(states.begin(), states.end());
  searcher->update(0, newStates, std::vector<ExecutionState *>());
  /* SYSREL side channel begin */
  bool isbranch = false;
   ExecutionState *lastState = NULL;
  //extractFunctionsCallingTimingObservationPoints(kmodule);
  //extractReachabilityOfTimingObservationPoints(kmodule,entryFunc);
  /* SYSREL side channel end */
  while (!states.empty() && !haltExecution) {
    ExecutionState &state = searcher->selectState();
    if (lastState && lastState != &state) {
       saveInfoFlowContext(*lastState);
       updateCurrentInfoFlowContext(state);
    }
    lastState = &state;
     /* Side channel begin */
     #ifdef VBSC
      std::cerr << "Number of states=" << states.size() << "\n";
     #endif
      Instruction* ii = state.pc->inst;
      #ifdef VBSC
      ii->dump();
      #endif
      const InstructionInfo &ii_info = kmodule->infos->getInfo(ii);
      #ifdef VBSC
      printInfo(ii_info);
      std::cerr << "\nGetting currentRD.\n";
      #endif
      /* SYSREL - branch tracking */
      if ((ii->getOpcode()==Instruction::Br) || (ii->getOpcode()==Instruction::Switch) 
          || (ii->getOpcode()==Instruction::Select && BreakSelectStmt)) {
         isbranch = true;
         successorsPaths->erase(successorsPaths->begin(), successorsPaths->end());
         #ifdef VBSC
         std::cerr << "\nIs a branch instruction\n";
         #endif
      }/* SYSREL - branch tracking */
      /* Side channel end */

    /* SYSREL */
    // Schedule async function if applicable
    #ifdef VB
    llvm::outs() << "Number of states=" << states.size() << "\n";
    #endif
    if (asyncMode) {
    int tid = -1;
    if ((tid = state.scheduleAsync(memory)) >= 0) {
       #ifdef VB
       llvm::outs() << "scheduling thread " << tid << "\n";
       #endif
       KInstruction *kia = state.threads[tid].pc;
       stepInstructionThread(state, tid);
       #ifdef VB
       llvm::outs() << "after step instruction for thread " << tid << "..\n";
       kia->inst->dump();
       #endif 
       executeInstructionThread(state, kia, tid);
       #ifdef VB
       llvm::outs() << "after execute instruction for thread " << tid << "..\n";
       #endif
       processTimers(&state, MaxInstructionTime);
       checkMemoryUsage();
       updateStates(&state);
    }
    else {
      if (!state.getWaitingForThreadsToTerminate()) {
       #ifdef VB
       llvm::outs() << "scheduling main thread\n";
       #endif
       KInstruction *ki = state.pc;
       stepInstruction(state);
       #ifdef VB
       llvm::outs() << "after step instruction for the main thread\n";
       ki->inst->dump();
       #endif
       /* SYSREL extension */
       singleSuccessor = true;
       /* SYSREL extension */
       executeInstruction(state, ki);
       /* SYSREL extension */
       // First check if any programming model related action to be executed
       if (singleSuccessor) {
           bool abort = false;
           state.executePM(abort);
           if (abort) {
              terminateStateOnError(state, "Memory error", Ptr, NULL, "Possible use-after-free");
          }
       }
       /* SYSREL extension */
       #ifdef VB
       llvm::outs() << "after execute instruction for the main thread\n";
       #endif
       processTimers(&state, MaxInstructionTime);

       checkMemoryUsage();
       updateStates(&state);
     }
     else {
        #ifdef VB
        llvm::outs() << "terminating main thread\n";
        #endif
        terminateStateOnExit(state);
        updateStates(&state);
     }
    }
    }
    else {
    /* SYSREL */
      KInstruction *ki = state.pc;
      stepInstruction(state);
      singleSuccessor = true;
      executeInstruction(state, ki);
      /* SYSREL extension */
      // First check if any programming model related action to be executed
      if (singleSuccessor) {
         bool abort = false;
         state.executePM(abort);
         if (abort) {
            terminateStateOnError(state, "Memory error", Ptr, NULL, "Possible use-after-free");
         }
      }
      /* SYSREL extension */
      processTimers(&state, MaxInstructionTime);
      checkMemoryUsage();
      updateStates(&state);
   }

   /* SYSREL Side Channel */
  if (sidechannelstarted) {
   RD* rdd = getrdmap(&state);
   currentRD = rdd;
   currentRD->ru += getTimingCost(state, ii);

   if (isbranch) {
      isbranch = false;
      if (successorsPaths->size() > 1) {
      	 bool printsucc = false;
    	 std::set<klee::ExecutionState*>::iterator it = successorsPaths->begin();
    	 bool done = false;
	 for(; it!=successorsPaths->end(); ++it) {
            ExecutionState *s = *it;
 	    ref<Expr> r1 = s->lastCondition;
            llvm::Instruction *brInst = NULL;
            bool mrbfound = false;
            getMostRecentBranchInfo(state,brInst, mrbfound);
            assert(mrbfound && "Most recent branch of the state has not been recorded\n");
             bool hasHloc = exprHasSymRegion(state, r1, true);
             bool hasLloc = exprHasSymRegion(state, r1, false);
             #ifdef VBSC
	            std::cerr << "\n>>>> Branch Condition : \n";
		    r1->dump();
                    const InstructionInfo &iit = kmodule->infos->getInfo(state.prevPC->inst);
                    state.prevPC->inst->print(llvm::errs());
                    printInfo(iit);                    
                    llvm::errs() << "has high? " << hasHloc << "\n";
                    llvm::errs() << "has low? " << hasLloc << "\n";
             #endif
             if (hasHloc)
                s->setSecretDescendant(true);

             RD* srd = newNode(s);
	     s->rdid = srd->stateid;
	     rdmapinsert(s->rdid, srd);
             // initialize srd in case it does not satisfy any special case
             srd->HA = currentRD->HA;
             srd->HAset = currentRD->HAset;
             srd->HC = currentRD->HC;
             srd->LC = currentRD->LC;
             srd->ru = currentRD->ru; 
	     if (hasHloc && !hasLloc) {
	     	ref<Expr> proj = getProjectionOnRegion(state, r1, true);
                //llvm::errs() << " high projection on " << r1 << " : " << proj << "\n";
                // check if the projection is consistent with the path condition
                bool result;
                bool success = solver->mayBeTrue(*s, proj, result);
                assert(success && "FIXME: Unhandled solver failure while checking feasibility of the projection");
                if (result) {
                   #ifdef VBSC
                          std::cerr << "Projection on high\n";
    	                  std::cerr << "\n>>>> Projection : ";
	                  proj->dump();
                          const InstructionInfo &ii = kmodule->infos->getInfo(state.prevPC->inst);
                          state.prevPC->inst->print(llvm::errs());
                          printInfo(ii);
                   #endif
                   //Update currentstate
		   if (!done) {
		      rdd->isHBr = true;
		      rdd->finalized = 0;
		      rdd->numSucc = successorsPaths->size();
		      done = true;
	              printsucc = true;
                      RD::numHAncestors++;
                      std::stringstream ss;
                      ss << ii_info.file.c_str() << " " << ii_info.line << ":\n ";
                      std::string tstr;
                      llvm::raw_string_ostream ros(tstr);
                      if (brInst != NULL)
                         ros << (*brInst) << "\n";
                      ss << ros.str() ;
                      ss << proj << "\n";
                      securitySensitiveBranches.insert(ss.str());
	           }
	   	   srd->HA = currentRD;
		   srd->HAset = true;
		   srd->HC = proj;
		   srd->LC = klee::ConstantExpr::create(true, 1);
	        }                
                else {
                   #ifdef VBSC
                          std::cerr << "Infeasible Projection on high\n";
                          std::cerr << "\n>>>> Projection : ";
                          proj->dump();
                          std::cerr << "PC:\n";
                          ExprPPrinter::printConstraints(llvm::errs(), s->constraints);
                   #endif

                }                                                 
             }   
	     else {
                if (hasHloc && hasLloc) {
                   RD::numHLMixedConstraints++;
                   #ifdef VBSC
                   llvm::errs() << "Mixed Branch Condition:\n";
                   // Note: may increase runtime
                   //r1->dump();
                   #endif
                }

                if (hasLloc) {
   	           ref<Expr> proj = getProjectionOnRegion(state, r1, false);
                   //llvm::errs() << " low projection on " << r1 << " : " << proj << "\n";

                   // check if the projection is consistent with the path condition
                   bool result;
                   bool success = solver->mayBeTrue(*s, proj, result);
                   assert(success && "FIXME: Unhandled solver failure while checking feasibility of the projection!");
                   if (result) {
                      #ifdef VBSC 
                             std::cerr << "Projection on low\n";
                             std::cerr << "\n>>>> Projection : ";
	                     proj->dump();
                      #endif
	              srd->LC = AndExpr::create(currentRD->LC, proj);
                   }
                   else {
                      #ifdef VBSC
                          std::cerr << "Infeasible Projection on low\n";
                          std::cerr << "\n>>>> Projection : ";
                          proj->dump();
                          std::cerr << "PC:\n";
                          ExprPPrinter::printConstraints(llvm::errs(), s->constraints);
                      #endif

                   }                     
               }
	     }
	     currentRD->succ->insert(srd);
             #ifdef VBSC
		    printResourceUsage(srd);
             #endif
          }
          #ifdef VBSC
		 if (printsucc) {
		    std::cerr << "\n>>>> HA : ";
		    for(std::set<RD*>::iterator srit = currentRD->succ->begin();
			srit != currentRD->succ->end(); ++srit){
			printResourceUsage(*srit);
		    }
		    printResourceUsage(currentRD);
		 }
          #endif
      }
    }
   }

   if (statsTracker) {
      for(unsigned aid = 0; aid < addedStates_copy.size(); aid++)
         if (addedStates_copy[aid]->isSecretDescendant())
            ++stats::sensitiveDesc;
      for(unsigned rid = 0; rid < removedStates_copy.size(); rid++)
         if (removedStates_copy[rid]->isSecretDescendant()) {
            --stats::sensitiveDesc;
            numSecretDependent++;
         }
         else numSecretIndependent++;
   }
   addedStates_copy.clear();
   removedStates_copy.clear();
   /* SYSREL Side Channel End */

  }

  delete searcher;
  searcher = 0;
  // SYSREL side channel
  std::cerr << "Size of rdmap : " << rdmap->size() << "\n";
  if (!SolverImpl::interrupted || !SolverImpl::forceOutput)
    doDumpStates();
  /* SYSREL side channel begin */
  timeSideChannelAnalysis(this);
  if (leakageWMaxSat)
     computeMaxSMT();
  collectInfoFlow();
  dumpInfoFlow(); 
  writeInfoFlowSummary((infoFlowSummarizedFuncName + "_summary.txt").c_str());
  /* SYSREL side channel end */

}


std::string Executor::getAddressInfo(ExecutionState &state,
                                     ref<Expr> address) const{
  std::string Str;
  llvm::raw_string_ostream info(Str);
  info << "\taddress: " << address << "\n";
  uint64_t example;
  if (ConstantExpr *CE = dyn_cast<ConstantExpr>(address)) {
    example = CE->getZExtValue();
  } else {
    ref<ConstantExpr> value;
    bool success = solver->getValue(state, address, value);
    assert(success && "FIXME: Unhandled solver failure");
    (void) success;
    example = value->getZExtValue();
    info << "\texample: " << example << "\n";
    std::pair< ref<Expr>, ref<Expr> > res = solver->getRange(state, address);
    info << "\trange: [" << res.first << ", " << res.second <<"]\n";
  }

  MemoryObject hack((unsigned) example);
  MemoryMap::iterator lower = state.addressSpace.objects.upper_bound(&hack);
  info << "\tnext: ";
  if (lower==state.addressSpace.objects.end()) {
    info << "none\n";
  } else {
    const MemoryObject *mo = lower->first;
    std::string alloc_info;
    mo->getAllocInfo(alloc_info);
    info << "object at " << mo->address
         << " of size " << mo->size << "\n"
         << "\t\t" << alloc_info << "\n";
  }
  if (lower!=state.addressSpace.objects.begin()) {
    --lower;
    info << "\tprev: ";
    if (lower==state.addressSpace.objects.end()) {
      info << "none\n";
    } else {
      const MemoryObject *mo = lower->first;
      std::string alloc_info;
      mo->getAllocInfo(alloc_info);
      info << "object at " << mo->address
           << " of size " << mo->size << "\n"
           << "\t\t" << alloc_info << "\n";
    }
  }

  return info.str();
}

void Executor::pauseState(ExecutionState &state){
  auto it = std::find(continuedStates.begin(), continuedStates.end(), &state);
  // If the state was to be continued, but now gets paused again
  if (it != continuedStates.end()){
    // ...just don't continue it
    std::swap(*it, continuedStates.back());
    continuedStates.pop_back();
  } else {
    pausedStates.push_back(&state);
  }
}

void Executor::continueState(ExecutionState &state){
  auto it = std::find(pausedStates.begin(), pausedStates.end(), &state);
  // If the state was to be paused, but now gets continued again
  if (it != pausedStates.end()){
    // ...don't pause it
    std::swap(*it, pausedStates.back());
    pausedStates.pop_back();
  } else {
    continuedStates.push_back(&state);
  }
}

void Executor::computeMaxSMT() {
      unsigned versionNo = 0;
      std::vector<ref<Expr> > softConstraints;
      for(auto cms : secretDependentRUConstMap) {
         versionNo++;
         ref<Expr> orExpr;
         for(unsigned j=0; j<cms.second.size(); j++) {
            ref<Expr> andExpr;
            for (ConstraintManager::const_iterator i = cms.second[j].begin();
                          i != cms.second[j].end(); i++) {
                if (andExpr.isNull())
                   andExpr = *i;
                else {
                   ref<Expr> temp = andExpr;
                   andExpr = AndExpr::create(temp, *i);
                }
            }
            if (orExpr.isNull())
               orExpr = andExpr;
            else {
               ref<Expr> temp = orExpr; 
               orExpr = OrExpr::create(temp, andExpr);
            }
        }
        ref<Expr> rn = renameExpr(memory, versionNo, orExpr, true);
        softConstraints.push_back(rn);
        std::fstream fsmt("maxsmt.txt", std::fstream::out);
        llvm::errs() << "maxsmt disjunct: " << rn << "\n";
      }
      Z3Solver  *z3 = new Z3Solver();
      leakageMaxSat = z3->maxSat(softConstraints);
      llvm::errs() << "leakage based on max smt " << leakageMaxSat << "\n";
    
}

void Executor::terminateState(ExecutionState &state) {
  /* SYSREL extension */
  if (state.instCount < minInstCount && state.instCount != 0)
     minInstCount = state.instCount;
  if (state.instCount > maxInstCount)
     maxInstCount = state.instCount;
  RD* rdd = getrdmap(&state);
  if (rdd) {
     if (rdd->ru < minInstCount && rdd->ru != 0)
        minInstCount = rdd->ru; 
     if (rdd->ru > maxInstCount)
        maxInstCount = rdd->ru; 
     // record statistics on the resource usage
     if (rdd->HA) {
        // secret dependent path
        secretDependentRUSet.insert(rdd->ru);
        unsigned c = 0;
        if (secretDependentRUMap.find(rdd->ru) != secretDependentRUMap.end()) {
           c = secretDependentRUMap[rdd->ru];
        }
        secretDependentRUMap[rdd->ru] = c+1;
        // store the path condition
        // printing should be done at the very end..
        // ExprSMTLIBPrinter *smtpr = new ExprSMTLIBPrinter();
        std::vector<ConstraintManager> cs;
        if (secretDependentRUConstMap.find(rdd->ru) !=  secretDependentRUConstMap.end()) {
           cs = secretDependentRUConstMap[rdd->ru];
        }            
        cs.push_back(state.constraints);
        secretDependentRUConstMap[rdd->ru] = cs;
        #ifdef VB
        llvm::errs() <<"PATH COND for secret dependent state:\n";
        ExprPPrinter::printConstraints(llvm::errs(), state.constraints);
        #endif
     }
  }
  #ifdef VB
  llvm::errs() << "Checking the state at the end of path\n";
  state.check();
  #endif
  /* SYSREL extension */


  if (replayKTest && replayPosition!=replayKTest->numObjects) {
    klee_warning_once(replayKTest,
                      "replay did not consume all objects in test input.");
  }

  interpreterHandler->incPathsExplored();

  std::vector<ExecutionState *>::iterator it =
      std::find(addedStates.begin(), addedStates.end(), &state);
  if (it==addedStates.end()) {
    state.pc = state.prevPC;

    removedStates.push_back(&state);
  } else {
    // never reached searcher, just delete immediately

    /* SYSREL extension */
    if (state.isSecretDescendant()) {
       numSecretDependent++;
    }
    else numSecretIndependent++;
    /* SYSREL extension */
  

    std::map< ExecutionState*, std::vector<SeedInfo> >::iterator it3 =
      seedMap.find(&state);
    if (it3 != seedMap.end())
      seedMap.erase(it3);
    addedStates.erase(it);
    processTree->remove(state.ptreeNode);
    delete &state;
  }
}

void Executor::terminateStateEarly(ExecutionState &state,
                                   const Twine &message) {
  if ((!SolverImpl::interrupted || !SolverImpl::forceOutput) && !OnlyOutputStatesCoveringNew || state.coveredNew ||
      (AlwaysOutputSeeds && seedMap.count(&state)))
    interpreterHandler->processTestCase(state, (message + "\n").str().c_str(),
                                        "early");
  terminateState(state);
}

void Executor::terminateStateOnExit(ExecutionState &state) {
  if (!OnlyOutputStatesCoveringNew || state.coveredNew ||
      (AlwaysOutputSeeds && seedMap.count(&state)))
    interpreterHandler->processTestCase(state, 0, 0);
  terminateState(state);
}

const InstructionInfo & Executor::getLastNonKleeInternalInstruction(const ExecutionState &state,
    Instruction ** lastInstruction) {
  // unroll the stack of the applications state and find
  // the last instruction which is not inside a KLEE internal function
  ExecutionState::stack_ty::const_reverse_iterator it = state.stack.rbegin(),
      itE = state.stack.rend();

  // don't check beyond the outermost function (i.e. main())
  itE--;

  const InstructionInfo * ii = 0;
  if (kmodule->internalFunctions.count(it->kf->function) == 0){
    ii =  state.prevPC->info;
    *lastInstruction = state.prevPC->inst;
    //  Cannot return yet because even though
    //  it->function is not an internal function it might of
    //  been called from an internal function.
  }

  // Wind up the stack and check if we are in a KLEE internal function.
  // We visit the entire stack because we want to return a CallInstruction
  // that was not reached via any KLEE internal functions.
  for (;it != itE; ++it) {
    // check calling instruction and if it is contained in a KLEE internal function
    const Function * f = (*it->caller).inst->getParent()->getParent();
    if (kmodule->internalFunctions.count(f)){
      ii = 0;
      continue;
    }
    if (!ii){
      ii = (*it->caller).info;
      *lastInstruction = (*it->caller).inst;
    }
  }

  if (!ii) {
    // something went wrong, play safe and return the current instruction info
    *lastInstruction = state.prevPC->inst;
    return *state.prevPC->info;
  }
  return *ii;
}

bool Executor::shouldExitOn(enum TerminateReason termReason) {
  std::vector<TerminateReason>::iterator s = ExitOnErrorType.begin();
  std::vector<TerminateReason>::iterator e = ExitOnErrorType.end();

  for (; s != e; ++s)
    if (termReason == *s)
      return true;

  return false;
}

void Executor::terminateStateOnError(ExecutionState &state,
                                     const llvm::Twine &messaget,
                                     enum TerminateReason termReason,
                                     const char *suffix,
                                     const llvm::Twine &info) {
  std::string message = messaget.str();
  static std::set< std::pair<Instruction*, std::string> > emittedErrors;
  static std::set< std::tuple<Instruction*, std::string, int> > emittedErrorsAlt;
  Instruction * lastInst;
  const InstructionInfo &ii = getLastNonKleeInternalInstruction(state, &lastInst);

  /* SYSREL extension */
  #ifdef VB
  llvm::errs() << "Error instruction " << lastInst << " content " << (*lastInst) << "\n";
  #endif
  /* SYSREL extension */


  // DEBUG
  #ifdef VB
  if (info.str() != "")
     llvm::outs() << "Info: \n" << info.str() << "\n";
  else llvm::outs() << "No info\n";
  #endif
  // END DEBUG


  if (EmitAllErrors || (state.hasLCM() ? emittedErrorsAlt.insert(std::make_tuple(lastInst, message, state.getLCMState())).second :
      emittedErrors.insert(std::make_pair(lastInst, message)).second)) {
    if (ii.file != "") {
      klee_message("ERROR: %s:%d: %s", ii.file.c_str(), ii.line, message.c_str());
    } else {
      klee_message("ERROR: (location information missing) %s", message.c_str());
    }
    if (!EmitAllErrors)
      klee_message("NOTE: now ignoring this error at this location");

    std::string MsgString;
    llvm::raw_string_ostream msg(MsgString);
    msg << "Error: " << message << "\n";
    if (ii.file != "") {
      msg << "File: " << ii.file << "\n";
      msg << "Line: " << ii.line << "\n";
      msg << "assembly.ll line: " << ii.assemblyLine << "\n";
    }
    msg << "Stack: \n";
    state.dumpStack(msg);

    std::string info_str = info.str();
    if (info_str != "")
      msg << "Info: \n" << info_str;

    std::string suffix_buf;
    if (!suffix) {
      suffix_buf = TerminateReasonNames[termReason];
      suffix_buf += ".err";
      suffix = suffix_buf.c_str();
    }

    /* SYSREL extension */
    #ifdef VB
    llvm::errs() << "processing test case in error state termination\n";
    #endif
    /* SYSREL extension */

    interpreterHandler->processTestCase(state, msg.str().c_str(), suffix);
  }
  else {
        /* SYSREL extension */
    #ifdef VB
    llvm::errs() << "error seen before\n";
    #endif
    /* SYSREL extension */
  }

  terminateState(state);

  if (shouldExitOn(termReason))
    haltExecution = true;

   #ifdef VB
  llvm::outs() << "Execution halting? " << haltExecution << "\n";
   #endif
}

// XXX shoot me
static const char *okExternalsList[] = { "printf",
                                         "fprintf",
                                         "puts",
                                         "getpid" };
static std::set<std::string> okExternals(okExternalsList,
                                         okExternalsList +
                                         (sizeof(okExternalsList)/sizeof(okExternalsList[0])));

void Executor::callExternalFunction(ExecutionState &state,
                                    KInstruction *target,
                                    Function *function,
                                    std::vector< ref<Expr> > &arguments) {
  // check if specialFunctionHandler wants it
  if (specialFunctionHandler->handle(state, function, target, arguments))
    return;

  if (NoExternals && !okExternals.count(function->getName())) {
    klee_warning("Disallowed call to external function: %s\n",
               function->getName().str().c_str());
    terminateStateOnError(state, "externals disallowed", User);
    return;
  }

  // normal external function handling path
  // allocate 128 bits for each argument (+return value) to support fp80's;
  // we could iterate through all the arguments first and determine the exact
  // size we need, but this is faster, and the memory usage isn't significant.

  uint64_t *args = (uint64_t*) alloca(2*sizeof(*args) * (arguments.size() + 1));
  memset(args, 0, 2 * sizeof(*args) * (arguments.size() + 1));
  unsigned wordIndex = 2;
  ObjectPair result;
 if (!APIHandler::handles(function->getName()) && !isInfoFlowAPI(removeDotSuffix(function->getName()))) { // this check is a SYSREL extension to avoid issues with pointers to 128 bit 
  llvm::errs() << "treating as external function: " << function->getName() << "\n";
  for (std::vector<ref<Expr> >::iterator ai = arguments.begin(),
       ae = arguments.end(); ai!=ae; ++ai) {
    if (AllowExternalSymCalls) { // don't bother checking uniqueness
      ref<ConstantExpr> ce;
      bool success = solver->getValue(state, *ai, ce);
      assert(success && "FIXME: Unhandled solver failure");
      (void) success;
      ce->toMemory(&args[wordIndex]);
      ObjectPair op;
      // Checking to see if the argument is a pointer to something
      if (ce->getWidth() == Context::get().getPointerWidth() &&
          state.addressSpace.resolveOne(ce, op)) {
        op.second->flushToConcreteStore(solver, state);
      }
      wordIndex += (ce->getWidth()+63)/64;
    } else {
      ref<Expr> arg = toUnique(state, *ai);
      if (ConstantExpr *ce = dyn_cast<ConstantExpr>(arg)) {
        // XXX kick toMemory functions from here
        ce->toMemory(&args[wordIndex]);
        wordIndex += (ce->getWidth()+63)/64;
      } else {
        /* SYSREL EXTENSION */
        if (progModel || lazyInit)
           break;
        /* SYSREL EXTENSION */
        terminateStateOnExecError(state,
                                  "external call with symbolic argument: " +
                                  function->getName());

        return;
      }
    }
  }

  // Prepare external memory for invoking the function
  state.addressSpace.copyOutConcretes();
#ifndef WINDOWS
  // Update external errno state with local state value
  int *errno_addr = getErrnoLocation(state);
  bool resolved = state.addressSpace.resolveOne(
      ConstantExpr::create((uint64_t)errno_addr, Expr::Int64), result);
  if (!resolved)
    klee_error("Could not resolve memory object for errno");
  ref<Expr> errValueExpr = result.second->read(0, sizeof(*errno_addr) * 8);
  ConstantExpr *errnoValue = dyn_cast<ConstantExpr>(errValueExpr);
  if (!errnoValue) {
    terminateStateOnExecError(state,
                              "external call with errno value symbolic: " +
                                  function->getName());
    return;
  }

  externalDispatcher->setLastErrno(
      errnoValue->getZExtValue(sizeof(*errno_addr) * 8));
#endif

  if (!SuppressExternalWarnings) {

    std::string TmpStr;
    llvm::raw_string_ostream os(TmpStr);
    os << "calling external: " << function->getName().str() << "(";
    for (unsigned i=0; i<arguments.size(); i++) {
      os << arguments[i];
      if (i != arguments.size()-1)
        os << ", ";
    }
    os << ") at " << state.pc->getSourceLocation();

    if (AllExternalWarnings)
      klee_warning("%s", os.str().c_str());
    else
      klee_warning_once(function, "%s", os.str().c_str());
  }

  }// end if APIHandler::handles

   /* SYSREL EXTENSION */
  bool success = false;
  if (!APIHandler::handles(function->getName()) && !isInfoFlowAPI(removeDotSuffix(function->getName()))) {
     success = externalDispatcher->executeCall(function, target->inst, args);

  #ifdef VB
  llvm::outs() << "external dispatcher result for " << function->getName() << " " << success << "\n";
   #endif
 }
 /* SYSREL EXTENSION */

  if (!success) {

    /* SYSREL EXTENSION */
   

    if (progModel) {
      checkAndRecordSensitiveFlow(state,function, arguments);
      llvm::errs() << "before check 1\n";
      symbolizeAndMarkSensitiveArgumentsOnCall(state, target, function, arguments, true);
      llvm::errs() << "after check 1\n";
      // check if PROSE version of the function exists, if so use that one
      std::string prosename = function->getName().str() + "_PROSE"; 
      Function *proseFn = moduleHandle->getFunction(prosename);
      if (proseFn) {
         llvm::errs() << "WARNING: Using " << prosename << " for " << function->getName() << "\n";
         ((CallInst*)target->inst)->setCalledFunction(proseFn);
         executeCall(state, target, proseFn, arguments);
         return;
      }
       // let the generic API handler handle the arg and return value symbolization
       std::vector<ExecutionState*> successors;
       #ifdef VB 
       llvm::errs() << "state=" << &state << " are we handling external function " << function->getName() << "\n";
       for(int a=0; a<arguments.size(); a++)
          llvm::errs() << "arg" << a << "=" << arguments[a] << "\n";
       #endif
       bool abort = false;
       bool resHandled =  APIHandler::handle(state, successors, function->getName(), arguments, target, abort);
       if (abort) {
          terminateStateOnError(state, "Memory error", Ptr, NULL, "Possible use-after-free");
          return;
       }
       if (!resHandled) {
          // check if prefix redaction works
          for(auto pr: prefixRedact) {
             if (function->getName().find(pr) == 0) {
                std::string fname = function->getName();
                std::string modelName =  fname.substr(pr.length());
                Function *modelFn = moduleHandle->getFunction(modelName);
                llvm::errs() << "Checking model function " << modelName << " using redacted prefix " << pr << "\n";
                if (modelFn) {
                   llvm::errs() << "WARNING: Using " << modelName << " for " << function->getName() << "\n";
                   ((CallInst*)target->inst)->setCalledFunction(modelFn);
                   executeCall(state, target, modelFn, arguments);
                   return;
                }
             }
          }          
          /* side channel */
          //checkHighArgumentFlow(state, target, function, arguments);
          /* side channel */
          #ifdef VB
          llvm::outs() << "symbolizing args and ret  value for function " << function->getName() << "\n";
          #endif
          //if (!symbolizeAndMarkArgumentsOnReturn(state, target, function, arguments)) {
          if (!isInputFunction(function)) {
           bool term = false;
           if (!isInfoFlowAPI(removeDotSuffix(function->getName())))
              symbolizeArguments(state, target, function, arguments, term);
           if (!term) {
             symbolizeReturnValue(state, arguments, target, function, abort);
             if (abort) {
                terminateStateOnError(state, "Memory error", Ptr, NULL, "Possible use-after-free");
                return;
             }
             ref<Expr> ret_value = getDestCell(state, target).value;
             symbolizeAndMarkArgumentsOnReturn(state, target, function, arguments, ret_value);
           }
          }
          else { // if input function, we also want to symbolize return value
             symbolizeReturnValue(state, arguments, target, function, abort);
             if (abort) {
                terminateStateOnError(state, "Memory error", Ptr, NULL, "Possible use-after-free");
                return;
             }
             ref<Expr> ret_value = getDestCell(state, target).value;
             if (!ret_value.isNull())
                symbolizeAndMarkArgumentsOnReturn(state, target, function, arguments, ret_value);
          }
       } // even if a model is handled via modeling it may be designated as an input function!
       // to be handled while model is simulated!
       // else symbolizeAndMarkArgumentsOnReturn(state, target, function, arguments);
       return;
       /*
       bool resHandled = false;
       bool regAPI = regAPIHandler->handle(state, function->getName(), arguments);
       bool resAPI = resADAPIHandler->handle(state, function->getName(), arguments);
       bool mutAPI = mutexAPIHandler->handle(state, function->getName(), arguments);
       bool refAPI = refcountAPIHandler->handle(state, function->getName(), arguments);
       bool allAPI = allocAPIHandler->handle(state, function->getName(), arguments, target, resHandled);
       bool freAPI = freeAPIHandler->handle(state, function->getName(), arguments, target);
       bool rwAPI = readWriteAPIHandler->handle(state, function->getName(), arguments, target);
       bool sgAPI = setgetAPIHandler->handle(state, function->getName(), arguments, target);
       bool igAPI = ignoreAPIHandler->handle(state, function->getName(), arguments, target);
       bool cbAPI = callbackAPIHandler->handle(state, function->getName(), arguments, target);
       bool seAPI = sideEffectAPIHandler->handle(state, function->getName(), arguments, target);
       if (regAPI || resAPI || mutAPI || refAPI || allAPI || freAPI || rwAPI || sgAPI || igAPI || cbAPI || seAPI) {
          llvm::outs() << "Handled API function " << function->getName() << "\n";
          llvm::outs() << "regApi=" << regAPI << "\n";
          llvm::outs() << "resApi=" << resAPI << "\n";
          llvm::outs() << "mutApi=" << mutAPI << "\n";
          llvm::outs() << "refApi=" << refAPI << "\n";
          llvm::outs() << "allApi=" << allAPI << "\n";
          llvm::outs() << "freApi=" << freAPI << "\n";
          llvm::outs() << "rwApi=" << rwAPI << "\n";
          llvm::outs() << "sgApi=" << sgAPI << "\n";
          llvm::outs() << "igApi=" << igAPI << "\n";
          llvm::outs() << "cbApi=" << cbAPI << "\n";
          llvm::outs() << "seApi=" << seAPI << "\n";
          if (sgAPI || (allAPI && resHandled))
             return;
          argInitSkip = true;
       }
    }
       */


     /*

   if (lazyInit) {
    // Instead of terminating the path, we would like to model such calls as functions that

    if (!argInitSkip) {
    // 1) arbitrarily update data reachable from pointer or call by reference arguments and
    // We will consider primitive types for now
    // Since we were not able to do makeSymbolic on an existing ObjectState, we will create shadow sym values
    // and copy them over the memory pointed by arg's; effectively making that memory symbolic
    // To find whether an argument is a pointer type, we need to find its use first!
    unsigned int ai = 0;
    for(llvm::Function::arg_iterator agi = function->arg_begin(); agi != function->arg_end(); agi++, ai++) {
       llvm::outs() << "ext function operand " << ai+1 << " " << target->operands[ai+1] << "\n";
       if (target->operands[ai+1] >= 0) { // a local var
          Type *at = agi->getType();
          if (at->isPointerTy()) {
             Type *bt = at->getPointerElementType();
                 std::string type_str;
                 llvm::raw_string_ostream rso(type_str);
                 at->print(rso);
             //if (bt->getPrimitiveSizeInBits()) {
                llvm::outs() << "Type of parameter " << ai << " is " << rso.str() << "\n";
                DataLayout *TD = kmodule->targetData;
                // find the MemoryObject for this value
                ObjectPair op;
                bool asuccess;
                ref<Expr> base = eval(target, ai+1, state).value;
                if (SimplifySymIndices) {
                   if (!isa<ConstantExpr>(base))
                      base = state.constraints.simplifyExpr(base);
                }
                solver->setTimeout(coreSolverTimeout);
                if (!state.addressSpace.resolveOne(state, solver, base, op, asuccess)) {
                   base = toConstant(state, base, "resolveOne failure");
                   asuccess = state.addressSpace.resolveOne(cast<ConstantExpr>(base), op);
                }
                solver->setTimeout(0);
                if (asuccess) {
                   MemoryObject *sm = memory->allocate(TD->getTypeAllocSize(bt), op.first->isLocal,
                           op.first->isGlobal, op.first->allocSite, TD->getPrefTypeAlignment(bt));
                   unsigned id = 0;
                   std::string name = "shadow";
                   std::string uniqueName = name;
                   while (!state.arrayNames.insert(uniqueName).second) {
                       uniqueName = name + "_" + llvm::utostr(++id);
                   }
                   // we're mimicking what executeMemoryOperation do without a relevant load or store instruction
                   const Array *array = arrayCache.CreateArray(uniqueName, sm->size);
                   ObjectState *sos = bindObjectInState(state, sm, true, array);
                   ref<Expr> result = sos->read(ConstantExpr::alloc(0, Expr::Int64), getWidthForLLVMType(bt));
                   ObjectState *wos = state.addressSpace.getWriteable(op.first, op.second);
                   wos->write(ConstantExpr::alloc(0, Expr::Int64), result);
                   llvm::outs() << "Wrote " << result << " to lazy init arg address " << base << " for function " << function->getName() << "\n";
               }
               else llvm::outs() << "Couldn't resolve address during lazy init arg: " << base << " for function " << function->getName() << "\n";
             //}

         }
       }
      }
    }

    // 2) return an arbitrary result as follows:
    // Alloc the data of return type, make it symbolic, and load the value of the symbolic mem to target->dest
    // TODO: check if there is a return value!
    if (function->getReturnType()->isVoidTy())
       return;
    std::string type_str;
    llvm::raw_string_ostream rso(type_str);
    function->getReturnType()->print(rso);
    llvm::outs() << "return type of external function " << function->getName() << ": " << rso.str() << "\n";
    const MemoryObject *mo;
    ref<Expr> laddr;
    llvm::Type *rType;
    bool mksym;
    mo = memory->allocateLazyForTypeOrEmbedding(state, state.prevPC->inst, function->getReturnType(), function->getReturnType(),
          isLazySingle(function->getReturnType(), "*"), 1, rType, laddr, mksym);
    mo->name = "%sym" + std::to_string(target->dest) + "_" + function->getName().str();
    llvm::outs() << "base address of symbolic memory to be copied from " << mo->getBaseExpr() << " and real target address " << laddr << "\n";
    if (mksym)
       executeMakeSymbolic(state, mo, mo->name);
    executeMemoryOperation(state, false, laddr, 0, target);
    return;
    */
    }
    else {
    /* SYSREL EXTENSION */

    terminateStateOnError(state, "failed external call: " + function->getName(),
                          External);
    return;
    }
  }

  if (!state.addressSpace.copyInConcretes()) {
    terminateStateOnError(state, "external modified read-only object",
                          External);
    return;
  }

#ifndef WINDOWS
  // Update errno memory object with the errno value from the call
  int error = externalDispatcher->getLastErrno();
  state.addressSpace.copyInConcrete(result.first, result.second,
                                    (uint64_t)&error);
#endif

  Type *resultType = target->inst->getType();
  if (resultType != Type::getVoidTy(function->getContext())) {
    ref<Expr> e = ConstantExpr::fromMemory((void*) args,
                                           getWidthForLLVMType(resultType));
    bindLocal(target, state, e);
  }
}

/* SYSREL extension */




bool Executor::checkHighArgumentFlow(ExecutionState &state,
                                  KInstruction *target,
                                  Function *function,
                                  std::vector< ref<Expr> > &arguments) {
 
    bool flows = false;
    unsigned int ai = 0;
    for(llvm::Function::arg_iterator agi = function->arg_begin(); agi != function->arg_end(); agi++, ai++) {
       #ifdef VB
       llvm::outs() << "ext function operand " << ai+1 << " " << target->operands[ai+1] << "\n";
       #endif
       if (target->operands[ai+1] >= 0) { // a local var
          Type *at = agi->getType();
          if (at->isPointerTy()) {
             Type *bt = at->getPointerElementType();
                 std::string type_str;
                 llvm::raw_string_ostream rso(type_str);
                 at->print(rso);
             //if (bt->getPrimitiveSizeInBits()) {
                #ifdef VB
                llvm::outs() << "Type of parameter " << ai << " is " << rso.str() << "\n";
                #endif
                DataLayout *TD = kmodule->targetData;
                // find the MemoryObject for this value
                ObjectPair op;
                bool asuccess;
                ref<Expr> base = eval(target, ai+1, state).value;
                if (SimplifySymIndices) {
                   if (!isa<ConstantExpr>(base))
                      base = state.constraints.simplifyExpr(base);
                }
                solver->setTimeout(coreSolverTimeout);
                if (!state.addressSpace.resolveOne(state, solver, base, op, asuccess)) {
                   base = toConstant(state, base, "resolveOne failure");
                   asuccess = state.addressSpace.resolveOne(cast<ConstantExpr>(base), op);
                }
                solver->setTimeout(0);
                if (asuccess) {
                   if (op.second->isHigh()) {
                      #ifdef VBSC
                      llvm::errs() << "High security argument " << ai << " flows into function " << function->getName() << "\n";
                      #endif
                      flows = true;
                   }
                   if (op.second->isLow()) {
                      #ifdef VBSC
                      llvm::errs() << "Low security argument " << ai << " flows into function " << function->getName() << "\n";
                      #endif
                      flows = true;
                   }
                }
          }
       }
     }
     return flows;
}

// depending on the called context, the stack frame of the function may or may not be created
// In the latter case the operands should be used to store the symbolized expressions
bool Executor::symbolizeAndMarkSensitiveArgumentsOnCall(ExecutionState &state,
                                  KInstruction *target,
                                  Function *function,
                                  std::vector< ref<Expr> > &arguments, bool useCallInstOperands) {

    std::string fname = function->getName();
    if (highFunctionArgs.find(fname) == highFunctionArgs.end() && 
        lowFunctionArgs.find(fname) == lowFunctionArgs.end() && 
        mixedFunctionArgs.find(fname) == mixedFunctionArgs.end()) { 
       return false; 
    }
    KFunction *kf = kmodule->functionMap[function];
    std::set<int> argsH, argsL, argsM; 
    if (highFunctionArgs.find(fname) != highFunctionArgs.end())
       argsH = highFunctionArgs[fname];
    if (lowFunctionArgs.find(fname) != lowFunctionArgs.end())
       argsL = lowFunctionArgs[fname];
    if (mixedFunctionArgs.find(fname) != mixedFunctionArgs.end())
       argsM = mixedFunctionArgs[fname];     
    llvm::errs() << "Handling functions with sensitive args " << function->getName() << "\n"; 
    unsigned int ai = 0;
    for(llvm::Function::arg_iterator agi = function->arg_begin(); agi != function->arg_end(); agi++, ai++) {
       if (argsH.find(ai) != argsH.end() || argsL.find(ai) != argsL.end() || argsM.find(ai) != argsM.end()) {
          llvm::errs() << "handling arg " << ai << " " << arguments[ai] << "\n"; 
          // a local var
          Type *at = agi->getType();
          bool pointerType = false;
          if (at->isPointerTy()) {
                pointerType = true;
                Type *bt = at->getPointerElementType();
                std::string btname = getTypeName(bt);
                std::string type_str;
                llvm::raw_string_ostream rso(type_str);
                at->print(rso);
                DataLayout *TD = kmodule->targetData;
                // find the MemoryObject for this value
                ObjectPair op;
                bool asuccess;
                //ref<Expr> base = eval(target, ai+1, state).value;
                ref<Expr> base = arguments[ai];
                if (SimplifySymIndices) {
                   if (!isa<ConstantExpr>(base))
                      base = state.constraints.simplifyExpr(base);
                }
                ConstantExpr *cexpr = dyn_cast<ConstantExpr>(base);
                if (cexpr) {
                   solver->setTimeout(coreSolverTimeout);
                   if (!state.addressSpace.resolveOne(state, solver, base, op, asuccess)) {
                      base = toConstant(state, base, "resolveOne failure");
                      asuccess = state.addressSpace.resolveOne(cast<ConstantExpr>(base), op);
                  }
                  solver->setTimeout(0);
                  if (asuccess) {
                   if (state.isSymbolic(op.first) || updatedWithSymbolic.find((long)op.first) != updatedWithSymbolic.end()) {
                      llvm::errs() << "yes!\n";
                      if (!skipOptimizingSensitiveRegionCheck) {
                      // check if it is already known to have a sensitive region
                      ref<Expr> cur = op.second->read(ConstantExpr::alloc(0, Expr::Int64),op.first->size*8);
                      ConstantExpr *CE = dyn_cast<ConstantExpr>(cur);
                      if (!CE) {
                        llvm::errs() << "going to check sym region sensitive\n";
                        if (isSymRegionSensitive(state, cur, fname, ai, true)) {
                           llvm::errs() << cur << "already known to be sensitive \n";
                           continue;
                        }
                        llvm::errs() << "done checking sym region sensitive\n";
                     } 
                     }
                     // set high/low
                     ref<Expr> mobase = op.first->getBaseExpr(); 
                     ConstantExpr *mob = dyn_cast<ConstantExpr>(mobase);
                     unsigned long diff = 0;
                     if (mob) 
                        diff = cexpr->getZExtValue() - mob->getZExtValue(); 
                     llvm::errs() << &state << " calling setSymRegionSensitive from OnCall " << fname  << " for arg " << ai << "\n";
                     setSymRegionSensitive(state,op.first,fname,bt,ai,pointerType,true,diff); 
                   }
                   else {
                      // mark the memory object symbolic and the region sensitive
                   std::string name = function->getName();
                   std::string uniqueName = name;
                   if (argsH.find(ai) != argsH.end())
                      uniqueName = name + "_high_" + std::to_string(ai);
                   if (argsL.find(ai) != argsL.end())
                      uniqueName = name + "_low_" + std::to_string(ai);
                   uniqueName = getUniqueSymRegion(uniqueName);
                   executeMakeSymbolic(state, op.first, uniqueName);
                   op.first->name = uniqueName;
                   ref<Expr> mobase = op.first->getBaseExpr(); 
                   ConstantExpr *mob = dyn_cast<ConstantExpr>(mobase);
                   unsigned long diff = 0;
                   if (mob) {
                      llvm::errs() << "cexpr: " <<  cexpr << "\n"; 
                      llvm::errs() << "width: " << cexpr->getWidth() << "\n";
                      llvm::errs() << "mob: " <<  mob << "\n";  
                      llvm::errs() << "width: " << mob->getWidth() << "\n";
                      diff = cexpr->getZExtValue() - mob->getZExtValue();                    
                   }
                   llvm::errs() << &state << " calling setSymRegionSensitive from OnCall 2" << fname  << " for arg " << ai << "\n";
                   setSymRegionSensitive(state, op.first, fname, bt, ai, pointerType, true, diff);
                  }
                 }
              }
          }
          else { // pass by value args
           if (!useCallInstOperands) {
            // check if it is already known to have a sensitive region
            ref<Expr> &cur = getArgumentCell(state, kf, ai).value;
            if (cur.isNull()) continue;
            ConstantExpr *CE = dyn_cast<ConstantExpr>(cur); 
            if (!CE) {
               if (isSymRegionSensitive(state, cur, fname, ai, true)) {
                  llvm::errs() << cur << "already known to be sensitive \n";
                  continue;
               }
            } 
            size_t allocationAlignment = 8;
            Instruction *inst = &*(function->begin()->begin());
            const llvm::DataLayout & dl = inst->getParent()->getParent()->getParent()->getDataLayout();
            MemoryObject *mo =  memory->allocate(dl.getTypeAllocSize(at), false, /*true*/false, inst, allocationAlignment);
            recordMemObj(state, mo);
            std::string name = function->getName();
            std::string uniqueName = name;
            if (argsH.find(ai) != argsH.end())
               uniqueName = name + "_high_" + std::to_string(ai);
            if (argsL.find(ai) != argsL.end())
               uniqueName = name + "_low_" + std::to_string(ai);
            uniqueName = getUniqueSymRegion(uniqueName);
            mo->name = uniqueName;
            // make local variable (arg registers) by writing a symbolic value
            // we're mimicking what executeMemoryOperation do without a relevant load or store instruction
            const Array *array = arrayCache.CreateArray(mo->name, mo->size);
            ObjectState *sos = bindObjectInState(state, mo, true, array);
            state.addSymbolic(mo, array);
            ref<Expr> result = sos->read(ConstantExpr::alloc(0, Expr::Int64), getWidthForLLVMType(at));
            bindArgument(kf, ai, state, result);
            std::string atname = getTypeName(at);
            llvm::errs() << &state << " calling setSymRegionSensitive from OnCall 3" << fname  << " for arg " << ai << "\n";
            setSymRegionSensitive(state,mo, fname, at, ai, pointerType, true);
          } 
          else { // useCallInstOperands
             // It means we have been called without a stack frame for the called function!
             llvm::errs() << "Skipping marking sensitivity for arg " << ai << " with value " << arguments[ai] << " in function " << function->getName() << " to avoid data-flow analysis\n";
          }
         }
      }
    }
    return true;
}

bool Executor::isSymRegionSensitive(ExecutionState &state, ref<Expr> CE, std::string fname, unsigned int ai, bool oncall) {
   std::set<int> argsH, argsL, argsM; 
   if (oncall && highFunctionArgs.find(fname) != highFunctionArgs.end())
      argsH = highFunctionArgs[fname];
   else if (!oncall && highFunctionArgsRet.find(fname) != highFunctionArgsRet.end())
     argsH = highFunctionArgsRet[fname];
   if (oncall && lowFunctionArgs.find(fname) != lowFunctionArgs.end())
      argsL = lowFunctionArgs[fname];
   else if (!oncall && lowFunctionArgsRet.find(fname) != lowFunctionArgsRet.end())
      argsL = lowFunctionArgsRet[fname];
   if (oncall && mixedFunctionArgs.find(fname) != mixedFunctionArgs.end())
      argsM = mixedFunctionArgs[fname];    
   else if (!oncall && mixedFunctionArgsRet.find(fname) != mixedFunctionArgsRet.end())
      argsM = mixedFunctionArgsRet[fname];  
   if (argsH.find(ai) != argsH.end()) 
      return exprHasSymRegion(state, CE, true);
   else if (argsL.find(ai) != argsL.end()) 
      return exprHasSymRegion(state, CE, false);
   else if (argsM.find(ai) != argsM.end()) { 
     // this is expensive to check; so lets be conservative and force to set it sensitive even if it is known to be sens
     return false;
     //assert(false && "found a case of mixed sensitivity of arg at a callsite!!!");
   }
   return false;
}

void Executor::setSymRegionSensitive(ExecutionState &state,
                                     const MemoryObject *sm, 
                                     std::string fname, 
                                     Type *argtype, 
                                     unsigned int ai, 
                                     bool pointerType,
                                     bool oncall,
                                     unsigned int offset)  {
   DataLayout *TD = kmodule->targetData;
   std::string btname = getTypeName(argtype); 
   bool hintFound = false;
   if (funcArgTypeHints.find(fname) != funcArgTypeHints.end())
      if (funcArgTypeHints[fname].find(ai) != funcArgTypeHints[fname].end()) {
         hintFound = true;
         btname = funcArgTypeHints[fname][ai];
         argtype = getTypeFromName(kmodule->module, btname);
      }
   size_t argsize = TD->getTypeAllocSize(argtype);
   std::set<int> argsH, argsL, argsM; 
   if (oncall && highFunctionArgs.find(fname) != highFunctionArgs.end())
      argsH = highFunctionArgs[fname];
   else if (!oncall && highFunctionArgsRet.find(fname) != highFunctionArgsRet.end())
     argsH = highFunctionArgsRet[fname];
   if (oncall && lowFunctionArgs.find(fname) != lowFunctionArgs.end())
      argsL = lowFunctionArgs[fname];
   else if (!oncall && lowFunctionArgsRet.find(fname) != lowFunctionArgsRet.end())
      argsL = lowFunctionArgsRet[fname];
   if (oncall && mixedFunctionArgs.find(fname) != mixedFunctionArgs.end())
      argsM = mixedFunctionArgs[fname];    
    else if (!oncall &&  mixedFunctionArgsRet.find(fname) != mixedFunctionArgsRet.end())
      argsM = mixedFunctionArgsRet[fname];
   if (argsH.find(ai) != argsH.end()) {
      std::vector<region> rs; 
      if (highTypeRegions.find(btname) != highTypeRegions.end()) {
         rs = highTypeRegions[btname];
      }
      else { // if not specified assume the whole region is high security sensitive
         region r;
         r.offset = offset;
         //r.size = sm->size * 8;
         llvm::errs() << "setting region size of " << sm->name << " to " << argsize * 8 << " while memobj size is " << sm->size * 8 << "\n";
         if (pointerType)
            r.size = sm->size * 8;
         else 
            r.size = argsize * 8;
         rs.push_back(r); 
      }
      setHighMemoryRegion(state, sm->getBaseExpr(), rs);
      llvm::errs() << &state << " calling setHighSym from setSymRegionSensitive for " << fname  << " for arg " << ai << "\n";
      setHighSymRegion(sm->name, rs);                    
   }
   else if (argsL.find(ai) != argsL.end()) {
      std::vector<region> rs;
      if (lowTypeRegions.find(btname) != lowTypeRegions.end()) {
         rs = lowTypeRegions[btname];
      }
      else { // if not specified assume the whole region is  low security sensitive
         region r;
         r.offset = offset;
         if (pointerType)
            r.size = sm->size * 8;
         else 
            r.size = argsize * 8;
         rs.push_back(r);
      }
      setLowMemoryRegion(state, sm->getBaseExpr(), rs);
      setLowSymRegion(sm->name, rs); 
   }  
   else if (argsM.find(ai) != argsM.end()) {
      if (!hintFound && lowTypeRegions.find(btname) == lowTypeRegions.end() && 
           highTypeRegions.find(btname) == highTypeRegions.end()) {
         llvm::errs() << "type=" << btname << "\n"; 
         assert(false && "Mixed sensitivity arg without type based sensitive regions!\n");
      }
      if (lowTypeRegions.find(btname) != lowTypeRegions.end()) {
         std::vector<region> rs = lowTypeRegions[btname]; 
         setLowMemoryRegion(state, sm->getBaseExpr(), rs);
         setLowSymRegion(sm->name, rs); 
       }
       if (highTypeRegions.find(btname) != highTypeRegions.end()) {
          std::vector<region> rs = highTypeRegions[btname];  
          setHighMemoryRegion(state, sm->getBaseExpr(), rs);
          llvm::errs() << "calling setHighSym from setSymRegionSensitive for " << fname  << "\n";
          setHighSymRegion(sm->name, rs); 
       }
   }
}


bool Executor::symbolizeAndMarkArgumentsOnReturn(ExecutionState &state,
                                  KInstruction *target,
                                  Function *function,
                                  std::vector< ref<Expr> > &arguments,
                                  ref<Expr> ret_value) {
    std::string fname = removeDotSuffix(function->getName());    
    llvm::errs() << "state :" << &state << " checking " << fname << " to see if an input function\n";
    //llvm::errs() << "instruction: " << (*target->inst) << "\n";
    //llvm::errs() << "return value: " << ret_value << "\n";
    if (inputFuncs.find(fname) == inputFuncs.end()) { 
       return false; 
    }
    ExecutionState *tstate = NULL;
    int success_value = 0;
    if (!function->getReturnType()->isVoidTy()) {
       if (inputFuncsSuccRetValue.find(fname) != inputFuncsSuccRetValue.end())
          success_value = inputFuncsSuccRetValue[fname];
       ConstantExpr *CE = dyn_cast<ConstantExpr>(ret_value);
       if (CE) {
          if (CE->getZExtValue() != success_value) {
             llvm::errs() << "skipping input function " << fname << " handling due to error path\n";
             return true;
          }
          tstate = &state;
       }
       else {
          ref<Expr> cond = EqExpr::create(ret_value, ConstantExpr::alloc(success_value,ret_value->getWidth()));
          Executor::StatePair branches = fork(state, cond, false);
          if (branches.first) {
             tstate = branches.first;
          }
          else {
             llvm::errs() << "skipping input function " << fname << " handling due to not satisfying success case:\n";
             return true;
          }
       }
    }
    else tstate = &state;

    std::set<unsigned int> argset = inputFuncArgs[fname];
    std::set<int> argsH, argsL, argsM; 
    if (highFunctionArgsRet.find(fname) != highFunctionArgsRet.end())
       argsH = highFunctionArgsRet[fname];
    if (lowFunctionArgsRet.find(fname) != lowFunctionArgsRet.end())
       argsL = lowFunctionArgsRet[fname];
    if (mixedFunctionArgsRet.find(fname) != mixedFunctionArgsRet.end())
       argsM = mixedFunctionArgsRet[fname];     
    llvm::errs() << "state: " << tstate << " Handling input function " << function->getName() << "\n"; 
    unsigned int ai = 0;
    for(llvm::Function::arg_iterator agi = function->arg_begin(); agi != function->arg_end(); agi++, ai++) {
       if (argsH.find(ai) != argsH.end() || argsL.find(ai) != argsL.end() || argsM.find(ai) != argsM.end()) { 
          // a local var
          Type *at = agi->getType();
          bool pointerType = false;
          if (at->isPointerTy()) {
                pointerType = true;
                Type *bt = at->getPointerElementType();
                std::string btname = getTypeName(bt);
                /*bool hintFound = false;
                if (funcArgTypeHints.find(fname) != funcArgTypeHints.end())
                   if (funcArgTypeHints[fname].find(ai) != funcArgTypeHints[fname].end()) {
                       hintFound = true;
                       btname = funcArgTypeHints[fname][ai];
                   }*/
                std::string type_str;
                llvm::raw_string_ostream rso(type_str);
                at->print(rso);
                DataLayout *TD = kmodule->targetData;
                // find the MemoryObject for this value
                ObjectPair op;
                bool asuccess;
                //ref<Expr> base = eval(target, ai+1, state).value;
                ref<Expr> base = arguments[ai];
                if (SimplifySymIndices) {
                   if (!isa<ConstantExpr>(base))
                      base = tstate->constraints.simplifyExpr(base);
                }
                solver->setTimeout(coreSolverTimeout);
                if (!tstate->addressSpace.resolveOne((*tstate), solver, base, op, asuccess)) {
                   base = toConstant((*tstate), base, "resolveOne failure");
                   asuccess = tstate->addressSpace.resolveOne(cast<ConstantExpr>(base), op);
                }
                solver->setTimeout(0);
                if (asuccess) {
                  if (tstate->isSymbolic(op.first) || updatedWithSymbolic.find((long)op.first) != updatedWithSymbolic.end()) {
                     // set high/low
                     unsigned int diff = 0;
                     ConstantExpr *cexpr = dyn_cast<ConstantExpr>(base);
                     ref<Expr> mobase = op.first->getBaseExpr();
                     ConstantExpr *mob = dyn_cast<ConstantExpr>(mobase);  
                     if (cexpr && mob) 
                        diff = cexpr->getZExtValue() - mob->getZExtValue(); 
                     llvm::errs() << "calling setSymRegionSensitive from OnReturn " << fname  << " btname= " << btname << "\n";
                     setSymRegionSensitive((*tstate),op.first,fname,bt,ai,pointerType,false,diff); 
                  }
                  else {
                   MemoryObject *sm = memory->allocate(TD->getTypeAllocSize(bt), op.first->isLocal,
                           op.first->isGlobal, op.first->allocSite, TD->getPrefTypeAlignment(bt));
                   recordMemObj(state, sm);
                   //#ifdef VB
                   llvm::errs() << "Symbolizing and marking argument " << ai << " of function " << function->getName() 
                                << ", address=" << sm->getBaseExpr() << "\n";
                   llvm::errs() << "base addres=" << base << "\n";
                   //#endif
                   std::string name = function->getName();
                   std::string uniqueName = name;
                   if (argsH.find(ai) != argsH.end())
                      uniqueName = name + "_high_" + std::to_string(ai);
                   if (argsL.find(ai) != argsL.end())
                      uniqueName = name + "_low_" + std::to_string(ai);
                   sm->name = getUniqueSymRegion(uniqueName);
                   unsigned int diff = 0;
                   ConstantExpr *cexpr = dyn_cast<ConstantExpr>(base);
                   ref<Expr> mobase = op.first->getBaseExpr();
                   ConstantExpr *mob = dyn_cast<ConstantExpr>(mobase);  
                   if (cexpr && mob) 
                      diff = cexpr->getZExtValue() - mob->getZExtValue(); 
                   llvm::errs() << "calling setSymRegionSensitive from OnReturn " << fname  << " btname= " << btname << "\n";
                   setSymRegionSensitive((*tstate),sm,fname,bt,ai,pointerType,false,diff); 
                   // we're mimicking what executeMemoryOperation do without a relevant load or store instruction
                   const Array *array = arrayCache.CreateArray(sm->name, sm->size);
                   ObjectState *sos = bindObjectInState((*tstate), sm, false, array);
                   state.addSymbolic(sm, array); 
                   ref<Expr> result = sos->read(ConstantExpr::alloc(0, Expr::Int64), getWidthForLLVMType(bt));
                   ObjectState *wos = (*tstate).addressSpace.getWriteable(op.first, op.second);
                   // compute offset: base - op.first->getBaseExpr()
                   ref<Expr> offsetexpr = SubExpr::create(base, op.first->getBaseExpr());
                   // check sanity
                   const ConstantExpr *co = dyn_cast<ConstantExpr>(offsetexpr);
                   if (co) {
                      if ((op.first->size - co->getZExtValue()) < sm->size) {
                         //#ifdef VB
                         llvm::errs() << "Lazy init of a void pointer mismatches real type, "
                                      << op.first->size << " " << co->getZExtValue() << " " << (op.first->size - co->getZExtValue()) << " vs " << sm->size << "\n";
                         //#endif
                         terminateStateOnError((*tstate), "memory error: cast due a void pointer", Ptr,
                            NULL, getAddressInfo((*tstate), op.first->getBaseExpr()));
                         return false;
                      }
                   }
                   wos->write(offsetexpr, result);
                   op.first->name = sm->name;
                   updatedWithSymbolic.insert((long)op.first);
               }
             }
         }
       }
      }
    return true;
}

void Executor::symbolizeArguments(ExecutionState &state,
                                  KInstruction *target,
                                  Function *function,
                                  std::vector< ref<Expr> > &arguments, bool &term) {

    unsigned int ai = 0;
    for(llvm::Function::arg_iterator agi = function->arg_begin(); agi != function->arg_end(); agi++, ai++) {
       #ifdef VB
       llvm::outs() << "ext function operand " << ai+1 << " " << target->operands[ai+1] << "\n";
       #endif
       if (target->operands[ai+1] >= 0) { // a local var
          Type *at = agi->getType();
          if (at->isPointerTy()) {
             Type *bt = at->getPointerElementType();
                 std::string type_str;
                 llvm::raw_string_ostream rso(type_str);
                 at->print(rso);
             //if (bt->getPrimitiveSizeInBits()) {
                #ifdef VB
                llvm::outs() << "Type of parameter " << ai << " is " << rso.str() << "\n";
                #endif
                DataLayout *TD = kmodule->targetData;
                // find the MemoryObject for this value
                ObjectPair op;
                bool asuccess;
                ref<Expr> base = eval(target, ai+1, state).value;
                if (SimplifySymIndices) {
                   if (!isa<ConstantExpr>(base))
                      base = state.constraints.simplifyExpr(base);
                }
                solver->setTimeout(coreSolverTimeout);
                if (!state.addressSpace.resolveOne(state, solver, base, op, asuccess)) {
                   base = toConstant(state, base, "resolveOne failure");
                   asuccess = state.addressSpace.resolveOne(cast<ConstantExpr>(base), op);
                }
                solver->setTimeout(0);
                if (asuccess) {
                   MemoryObject *sm = memory->allocate(TD->getTypeAllocSize(bt), op.first->isLocal,
                           op.first->isGlobal, op.first->allocSite, TD->getPrefTypeAlignment(bt));
                   recordMemObj(state, sm);
                   #ifdef VB
                   llvm::errs() << "Symbolizing argument of function " << function->getName() << ", address=" << sm->getBaseExpr() << "\n";
                   llvm::errs() << "base addres=" << base << "\n";
                   #endif
                   unsigned id = 0;
                   std::string name = function->getName();
                   name = name + "_shadow_arg_";
                   name = name + std::to_string(ai);
                   std::string uniqueName = getUniqueSymRegion(name);
                   /*while (!state.arrayNames.insert(uniqueName).second) {
                       uniqueName = name + "_" + llvm::utostr(++id);
                   }*/
                   // we're mimicking what executeMemoryOperation do without a relevant load or store instruction
                   const Array *array = arrayCache.CreateArray(uniqueName, sm->size);
                   ObjectState *sos = bindObjectInState(state, sm, true, array);
                   state.addSymbolic(sm, array);
                   ref<Expr> result = sos->read(ConstantExpr::alloc(0, Expr::Int64), getWidthForLLVMType(bt));
                   ObjectState *wos = state.addressSpace.getWriteable(op.first, op.second);
                   // compute offset: base - op.first->getBaseExpr()
                   ref<Expr> offsetexpr = SubExpr::create(base, op.first->getBaseExpr());
                   #ifdef VB
                   llvm::errs() << TD->getTypeAllocSize(bt) << " vs width=" << getWidthForLLVMType(bt) << "offsetexpr=" << offsetexpr << "result=" << result << " width=" << getWidthForLLVMType(bt) << " sm->size=" << sm->size << "targetbase=" << op.first->getBaseExpr() << " targetsize=" << op.first->size << "\n";
                   #endif
                   // check sanity
                   const ConstantExpr *co = dyn_cast<ConstantExpr>(offsetexpr);
                   if (co) {
                      if ((op.first->size - co->getZExtValue()) < sm->size) {
                         //#ifdef VB
                         llvm::errs() << "Lazy init of a void pointer mismatches real type, "
                                      << op.first->size << " " <<  co->getZExtValue() << " " << (op.first->size - co->getZExtValue()) << " vs " << sm->size << "\n";
                         //#endif
                         terminateStateOnError(state, "memory error: cast due a void pointer", Ptr,
                            NULL, getAddressInfo(state, op.first->getBaseExpr()));
                         term = true;
                         return;
                      }
                   }
                   wos->write(offsetexpr, result);
                   op.first->name = sm->name;
                   updatedWithSymbolic.insert((long)op.first);
                   #ifdef VB
                   llvm::outs() << "Wrote " << result << " to lazy init arg address " << base << " for function " << function->getName() << "\n";
                   #endif
               }
               #ifdef VB
               else llvm::outs() << "Couldn't resolve address during lazy init arg: " << base << " for function " << function->getName() << "\n";
               #endif
             //}

         }
       }
      }

}


const MemoryObject *Executor::symbolizeReturnValueForAsmInstruction(ExecutionState &state, 
                                  KInstruction *target, llvm::Value *inst,
                                  bool &abort) {
    std::string type_str;
    llvm::raw_string_ostream rso(type_str);
    llvm::InlineAsm *iasm = dyn_cast<llvm::InlineAsm>(inst);
    if (!iasm) return NULL;
    Type *retType = iasm->getFunctionType()->getReturnType();
    retType->print(rso);
    const MemoryObject *mo;
    ref<Expr> laddr;
    llvm::Type *rType;
    bool mksym;
    Type *allocType = retType;
    if (retType->isPointerTy())
       allocType = retType->getPointerElementType();
    mo = memory->allocateLazyForTypeOrEmbedding(state, state.prevPC->inst, allocType, allocType,
          isLazySingle(retType, "*"), 1, rType, laddr, mksym, abort);
    if (!mo) {
       return NULL;
    }
    mo->name = "%sym" + std::to_string(target->dest) + "_" + getUniqueSymRegion("asm"); 
    #ifdef VB
    llvm::errs() << "symbolizing return value of inline assembly\n";
    #endif
    executeMakeSymbolic(state, mo, mo->name, allocType, true);
    if (allocType == retType) 
       executeMemoryOperation(state, -2, -2, false, laddr, 0, target);
    else
       bindLocal(target, state, laddr);

   return mo;
}

const MemoryObject *Executor::symbolizeReturnValueSimple(ExecutionState &state, 
                                  KInstruction *target,
                                  Function *function, bool &abort) {

    if (function->getReturnType()->isVoidTy())
       return NULL;
    std::string type_str;
    llvm::raw_string_ostream rso(type_str);
    function->getReturnType()->print(rso);
    #ifdef VB
    llvm::outs() << "return type of external function " << function->getName() << ": " << rso.str() << "\n";
    #endif
    const MemoryObject *mo;
    ref<Expr> laddr;
    llvm::Type *rType;
    bool mksym;
    llvm::Type *retType = function->getReturnType();
    llvm::Type *allocType = retType;
    if (retType->isPointerTy())
       allocType = retType->getPointerElementType();
    mo = memory->allocateLazyForTypeOrEmbedding(state, state.prevPC->inst, allocType, allocType,
          isLazySingle(function->getReturnType(), "*"), 1, rType, laddr, mksym, abort);
    if (abort) {
       return NULL;
    }
    mo->name = "%sym" + std::to_string(target->dest) + "_" + getUniqueSymRegion(function->getName().str());
    #ifdef VB
    llvm::outs() << "mo=" << mo << "\n";
    llvm::outs() << "base address of symbolic memory to be copied from " << mo->getBaseExpr() << " and real target$
    #endif
    if (mksym) {
       #ifdef VB
       llvm::outs() << "symbolizing return value \n";
       #endif
       executeMakeSymbolic(state, mo, mo->name, allocType, true);
    }
    if (allocType == retType)
       executeMemoryOperation(state, -2, -2, false, laddr, 0, target);
    else
       bindLocal(target, state, laddr);

    return mo;
}


const MemoryObject *Executor::symbolizeReturnValue(ExecutionState &state, 
                                  std::vector<ref<Expr> > & args,
                                  KInstruction *target,
                                  Function *function, bool &abort) {

    if (function->getReturnType()->isVoidTy())
       return NULL;
    std::string type_str;
    llvm::raw_string_ostream rso(type_str);
    function->getReturnType()->print(rso);
    #ifdef VB
    llvm::outs() << "return type of external function " << function->getName() << ": " << rso.str() << "\n";
    #endif
    const MemoryObject *mo;
    ref<Expr> laddr;
    llvm::Type *rType;
    bool mksym;
    llvm::Type *retType = function->getReturnType();
    llvm::Type *allocType = retType;
    if (retType->isPointerTy())
       allocType = retType->getPointerElementType();
    mo = memory->allocateLazyForTypeOrEmbedding(state, state.prevPC->inst, allocType, allocType,
          isLazySingle(function->getReturnType(), "*"), 1, rType, laddr, mksym, abort);
    if (abort) {
       return NULL;
    }
    mo->name = "%sym" + std::to_string(target->dest) + "_" + getUniqueSymRegion(function->getName().str());
    #ifdef VB
    llvm::outs() << "mo=" << mo << "\n";
    llvm::outs() << "base address of symbolic memory to be copied from " << mo->getBaseExpr() << " and real target address " << laddr << "\n";
    #endif
    if (mksym) {
       #ifdef VB
       llvm::outs() << "symbolizing return value \n";
       #endif
       executeMakeSymbolic(state, mo, mo->name, allocType, true);
    }
    if (allocType == retType)
       executeMemoryOperation(state, -2, -2, false, laddr, 0, target);
    else  { // return type is a pointer type
       if (!nullReturnValue) {
          bindLocal(target, state, laddr);
       }
       else { 

       // first create a symbolic pointer
       const MemoryObject *moptr = memory->allocate(kmodule->targetData->getTypeAllocSize(retType), false,
                                          true, state.prevPC->inst,
                                          kmodule->targetData->getPrefTypeAlignment(retType));
       recordMemObj(state, moptr);
       moptr->name = "%sym_retpointer_" + std::to_string(target->dest) + "_" + state.getUnique(function->getName().str());
       // we're mimicking what executeMemoryOperation do without a relevant load or store instruction
       const Array *array = arrayCache.CreateArray(moptr->name, moptr->size);
       ObjectState *sos = bindObjectInState(state, moptr, true, array);
       ref<Expr> result = sos->read(ConstantExpr::alloc(0, Expr::Int64), getWidthForLLVMType(retType)); 
       //llvm::errs() << "PC before symbolize return value for state " << &state << "\n";
       //ExprPPrinter::printConstraints(llvm::errs(), state.constraints);
       //llvm::errs() << "first assigning " << result << " to the return value\n";      
       bindLocal(target,  state, result);
       //ref<Expr> nullret = EqExpr::create(target, Expr::createPointer(0));
       //llvm::errs() << "forking on null return value: " << nullret << "\n";
       Executor::StatePair pair = fork(state, Expr::createIsZero(result), true);
       //llvm::errs() << "result: " << result << " first: " << pair.first << " second: " << pair.second << "\n";
       //llvm::errs() << "symbolize return value for : " << (*target->inst) << "\n";
       //llvm::errs() << "PC for symbolize return value\n";
       //ExprPPrinter::printConstraints(llvm::errs(), state.constraints);
       if (pair.first && pair.second) {
          // we need to enforce null pointer here or lazy initialization will assign a valid address upon memory access!
          bindLocal(target, *pair.first, Expr::createPointer(0)); 
          // make the not null case point to the symbolic memory of the base type
          //llvm::errs() << "assigning " << laddr << " in symbolizereturn in state " << pair.second << " orig state=" << &state << "\n"; 
          bindLocal(target, *pair.second, laddr);
       }
       else {
          assert(pair.first == &state || pair.second == &state);
          bool res;
          solver->setTimeout(coreSolverTimeout);
          bool success = solver->mustBeTrue(state, 
                                      Expr::createIsZero(result),
                                      res);
          solver->setTimeout(0);
          if (success) {
             if (res) {
                bindLocal(target, (pair.first ? *pair.first : *pair.second), Expr::createPointer(0));
                //llvm::errs() << "assigning in single successor NULL for return value\n";
             }
             else {
                bindLocal(target, (pair.first ? *pair.first : *pair.second) , laddr); 
                //llvm::errs() << "assigning in single successor " << laddr << "for return value\n";
             }
          }
          //else llvm::errs() << "keeping return value symbolic!!!\n"; 

       }
     }
    }

    checkAndUpdateInfoFlow(state, function, args, mo);
    return mo;
}

/* SYSREL extension */


/***/

ref<Expr> Executor::replaceReadWithSymbolic(ExecutionState &state,
                                            ref<Expr> e) {
  unsigned n = interpreterOpts.MakeConcreteSymbolic;
  if (!n || replayKTest || replayPath)
    return e;

  // right now, we don't replace symbolics (is there any reason to?)
  if (!isa<ConstantExpr>(e))
    return e;

  if (n != 1 && random() % n)
    return e;

  // create a new fresh location, assert it is equal to concrete value in e
  // and return it.

  static unsigned id;
  const Array *array =
      arrayCache.CreateArray("rrws_arr" + llvm::utostr(++id),
                             Expr::getMinBytesForWidth(e->getWidth()));
  ref<Expr> res = Expr::createTempRead(array, e->getWidth());
  ref<Expr> eq = NotOptimizedExpr::create(EqExpr::create(e, res));
  #ifdef VB
  llvm::errs() << "Making symbolic: " << eq << "\n";
  #endif
  state.addConstraint(eq);
  return res;
}

ObjectState *Executor::bindObjectInState(ExecutionState &state,
                                         const MemoryObject *mo,
                                         bool isLocal,
                                         const Array *array) {
  //llvm::errs() << "binding a new object to mo " << mo->getBaseExpr() << ":\n"; 

  ObjectState *os = array ? new ObjectState(mo, array) : new ObjectState(mo);
  state.addressSpace.bindObject(mo, os);

  // Its possible that multiple bindings of the same mo in the state
  // will put multiple copies on this list, but it doesn't really
  // matter because all we use this list for is to unbind the object
  // on function return.
  if (isLocal) {
    //llvm::errs() << "alloca at " << mo->getBaseExpr() << "\n";
    state.stack.back().allocas.push_back(mo);
  }

  return os;
}

void Executor::executeAlloc(ExecutionState &state,
                            ref<Expr> size,
                            bool isLocal,
                            KInstruction *target,
                            bool zeroMemory,
                            bool record,
                            const ObjectState *reallocFrom) {
  #ifdef VB 
  llvm::errs() << "Alloc'ing...\n";
  #endif

  size = toUnique(state, size);
  if (ConstantExpr *CE = dyn_cast<ConstantExpr>(size)) {
    const llvm::Value *allocSite = state.prevPC->inst;
    size_t allocationAlignment = getAllocationAlignment(allocSite);
    int allocsize = CE->getZExtValue();
    #ifdef VB
    llvm::errs() << "Alloc size: " << CE->getZExtValue() << " or " << allocsize << " vs MAX: " << MAX_ALLOC_SIZE << " " << (CE->getZExtValue() > MAX_ALLOC_SIZE) << "\n";
    #endif
    if (allocsize <= 0 || allocsize > MAX_ALLOC_SIZE) {
       //llvm::errs() << "Replacing " << allocsize << " alloc size with " << primArraySize << "\n";
       allocsize = primArraySize;
    }
    //llvm::errs() << "adjusted alloc size: " << allocsize << "\n"; 
    MemoryObject *mo =
        memory->allocate(allocsize, isLocal, /*isGlobal=*/false,
                         allocSite, allocationAlignment);
    recordMemObj(state, mo);
    //#ifdef VB
    if (isLocal)
       llvm::errs() << "Local alloc: " << (*target->inst) << " at address " << mo->getBaseExpr() << "\n"; 
    //#endif


   
    /* SYSREL extension */
 
   if (infoFlowSummaryMode && isLocal && target->dest >= 0)  {
      llvm::errs() << "recording register " << target->dest << " as a local variable\n"; 
      (*currentInfoFlowContext.regMap)[target->dest] = mo;
   }

   if (record)
      state.recordAlloc(mo->getBaseExpr());

   if (lazyInit) {
    Type *t = target->inst->getType();
    if (t->isPointerTy()) {
        t = t->getPointerElementType();
        if (!t->isPointerTy()) {
            StructType *st = dyn_cast<StructType>(t);
            if (st) {
               std::string tname = getTypeName(t);
               state.typeToAddr[t] = mo->getBaseExpr();
               #ifdef VB
               llvm::outs() << "mapping alloced " << tname << " to " << mo->getBaseExpr() << "\n";
              #endif
            }
        }
    }
   }
    /* SYSREL extension */

    if (!mo) {
      bindLocal(target, state,
                ConstantExpr::alloc(0, Context::get().getPointerWidth()));
    } else {
      ObjectState *os = bindObjectInState(state, mo, isLocal);
      if (zeroMemory) {
        os->initializeToZero();
      } else {
        os->initializeToRandom();
      }
      #ifdef VB
      llvm::errs() << "mo=" << mo << "\n";
      llvm::errs() << "binding " << mo->getBaseExpr() << " to " << (*target->inst) << "\n";
      if (getDestCell(state, target).value.get())
         llvm::errs() << "address to be dest: " << getDestCell(state, target).value << "\n";
      #endif


      bindLocal(target, state, mo->getBaseExpr());

      if (reallocFrom) {
        unsigned count = std::min(reallocFrom->size, os->size);
        for (unsigned i=0; i<count; i++)
          os->write(i, reallocFrom->read8(i));
        state.addressSpace.unbindObject(reallocFrom->getObject());
      }
    }
  } else {
    // XXX For now we just pick a size. Ideally we would support
    // symbolic sizes fully but even if we don't it would be better to
    // "smartly" pick a value, for example we could fork and pick the
    // min and max values and perhaps some intermediate (reasonable
    // value).
    //
    // It would also be nice to recognize the case when size has
    // exactly two values and just fork (but we need to get rid of
    // return argument first). This shows up in pcre when llvm
    // collapses the size expression with a select.

    ref<ConstantExpr> example;
    bool success = solver->getValue(state, size, example);
    assert(success && "FIXME: Unhandled solver failure");
    (void) success;

    // Try and start with a small example.
    Expr::Width W = example->getWidth();
    while (example->Ugt(ConstantExpr::alloc(128, W))->isTrue()) {
      ref<ConstantExpr> tmp = example->LShr(ConstantExpr::alloc(1, W));
      bool res;
      bool success = solver->mayBeTrue(state, EqExpr::create(tmp, size), res);
      assert(success && "FIXME: Unhandled solver failure");
      (void) success;
      if (!res)
        break;
      example = tmp;
    }

    llvm::errs() << "Inferred alloc size " << example << "\n";

    StatePair fixedSize = fork(state, EqExpr::create(example, size), true);

    if (fixedSize.second) {
      // Check for exactly two values
      ref<ConstantExpr> tmp;
      bool success = solver->getValue(*fixedSize.second, size, tmp);
      assert(success && "FIXME: Unhandled solver failure");
      (void) success;
      bool res;
      success = solver->mustBeTrue(*fixedSize.second,
                                   EqExpr::create(tmp, size),
                                   res);
      assert(success && "FIXME: Unhandled solver failure");
      (void) success;
      if (res) {
        if (lazyInit) {
           ref<Expr> ps = ConstantExpr::alloc(primArraySize, 32);
           executeAlloc(*fixedSize.second, ps, isLocal,
                     target, zeroMemory, record, reallocFrom);
        }
        else 
           executeAlloc(*fixedSize.second, tmp, isLocal,
                     target, zeroMemory, record, reallocFrom);
      } else {
        // See if a *really* big value is possible. If so assume
        // malloc will fail for it, so lets fork and return 0.
        StatePair hugeSize =
          fork(*fixedSize.second,
               UltExpr::create(ConstantExpr::alloc(1U<<31, W), size),
               true);
        if (hugeSize.first) {
          klee_message("NOTE: found huge malloc, returning 0");
          bindLocal(target, *hugeSize.first,
                    ConstantExpr::alloc(0, Context::get().getPointerWidth()));
        }

        if (hugeSize.second) {

          /* SYSREL extension */
          if (lazyInit) {
             llvm::errs() << "concretized symbolic size as " << primArraySize << "\n";
              executeAlloc(*hugeSize.second, ConstantExpr::create(primArraySize,32), isLocal,
                   target, zeroMemory, record, reallocFrom);

         }
         else {
         /* SYSREL extension */
          std::string Str;
          llvm::raw_string_ostream info(Str);
          ExprPPrinter::printOne(info, "  size expr", size);
          info << "  concretization : " << example << "\n";
          info << "  unbound example: " << tmp << "\n";
          terminateStateOnError(*hugeSize.second, "concretized symbolic size",
                                Model, NULL, info.str());
         }
        }
      }
    }

    if (fixedSize.first) { // can be zero when fork fails
       // SYSREL extension 
       //llvm::errs() << "Encountered a large alloc; symbolizing the return value instead!\n";
       //bool abort = false;
       //symbolizeReturnValueSimple(state, target, target->inst->getParent()->getParent(), abort); 
       llvm::errs() << "Encountered a large alloc; using the primitive array size instead!\n";
       ref<Expr> ps = ConstantExpr::alloc(primArraySize, 32);
       executeAlloc(*fixedSize.first, ps, isLocal,
                   target, zeroMemory, record, reallocFrom);
       //executeAlloc(*fixedSize.first, example, isLocal,
                   //target, zeroMemory, record, reallocFrom);
       // SYSREL extension 
    }
  }
}

void Executor::executeFree(ExecutionState &state,
                           ref<Expr> address,
                           KInstruction *target) {
  #ifdef VB
  llvm::errs() << "Executing free " << address << " from function " << state.prevPC->inst->getParent()->getParent()->getName() << "\n";
  #endif
  state.recordFree(address);
  StatePair zeroPointer = fork(state, Expr::createIsZero(address), true);
  if (zeroPointer.first) {
     #ifdef VB
     llvm::outs() << "A zero pointer possibility in free " << address << "\n";
    #endif
    if (target)
      bindLocal(target, *zeroPointer.first, Expr::createPointer(0));
  }
  if (zeroPointer.second) { // address != 0
    ExactResolutionList rl;
    #ifdef VB 
    llvm::outs() << "Resolving exact for free!";
    #endif
    resolveExact(*zeroPointer.second, address, rl, "free");
    #ifdef VB
    llvm::outs() << "Resolved exact for free!";
    #endif
    for (Executor::ExactResolutionList::iterator it = rl.begin(),
           ie = rl.end(); it != ie; ++it) {
      const MemoryObject *mo = it->first.first;
      if (mo->isLocal) {
        terminateStateOnError(*it->second, "free of alloca", Free, NULL,
                              getAddressInfo(*it->second, address));
      } else if (mo->isGlobal) {
        terminateStateOnError(*it->second, "free of global", Free, NULL,
                              getAddressInfo(*it->second, address));
      } else {
         #ifdef VB
         llvm::outs() << "In free, unbinding obj at address "  << mo->getBaseExpr() << "\n";
         #endif

         /* SYSREL extension */
         state.recordFree(mo->getBaseExpr());
         /* SYSREL extension */

        it->second->addressSpace.unbindObject(mo);
        if (target)
          bindLocal(target, *it->second, Expr::createPointer(0));
      }
    }
  }
}

void Executor::resolveExact(ExecutionState &state,
                            ref<Expr> p,
                            ExactResolutionList &results,
                            const std::string &name) {
  // XXX we may want to be capping this?
  ResolutionList rl;
  state.addressSpace.resolve(state, solver, p, rl);

  ExecutionState *unbound = &state;
  for (ResolutionList::iterator it = rl.begin(), ie = rl.end();
       it != ie; ++it) {
    ref<Expr> inBounds = EqExpr::create(p, it->first->getBaseExpr());

    StatePair branches = fork(*unbound, inBounds, true);

    if (branches.first)
      results.push_back(std::make_pair(*it, branches.first));

    unbound = branches.second;
    if (!unbound) // Fork failure
      break;
  }

  if (unbound) {
    terminateStateOnError(*unbound, "memory error: invalid pointer: " + name,
                          Ptr, NULL, getAddressInfo(*unbound, p));
  }
}

/* SYSREL EXTENSION */

int getLocalIndex(ExecutionState &state, KInstruction *ki, int index) {

  int vnumber = ki->operands[index];

  assert(vnumber != -1 &&
         "Invalid operand to eval(), not a value or constant!");

  // Determine if this is a constant or not.
  if (vnumber < 0) {
    return -1;
  } else {
    return vnumber;
  }
}

// return true if abort
bool Executor::executeMemoryOperation(ExecutionState &state,
                                      int regIndex,
                                      int destregIndex,
                                      bool isWrite,
                                      ref<Expr> address,
                                      ref<Expr> value /* undef if read */,
                                      KInstruction *target /* undef if write */) {


  /* SYSREL EXTENSION */
  static bool forcedOffset = false;
  /* SYSREL EXTENSION */

  if (reached.find(state.prevPC->inst->getParent()->getParent()->getName()) == reached.end()) { 
     llvm::errs() << "reached " << state.prevPC->inst->getParent()->getParent()->getName() << "\n";
     reached.insert(state.prevPC->inst->getParent()->getParent()->getName());
  }

  //#ifdef VB
  llvm::errs() << "state=" << &state << " memory operation (inside " << state.prevPC->inst->getParent()->getParent()->getName() << ") \n";
  state.prevPC->inst->print(llvm::errs());
  llvm::errs() << "\n address: " << address << "\n";
  llvm::errs() << "executeMemoryOperation isWrite? " << isWrite  << "\n";
  if (isWrite)
     llvm::errs() << "storing value " << value << "\n";
  //#endif


  Expr::Width type = (isWrite ? value->getWidth() :
                     getWidthForLLVMType(target->inst->getType()));

  unsigned bytes = Expr::getMinBytesForWidth(type);



  if (SimplifySymIndices) {
    if (!isa<ConstantExpr>(address))
      address = state.constraints.simplifyExpr(address);
    if (isWrite && !isa<ConstantExpr>(value))
      value = state.constraints.simplifyExpr(value);
  }

  llvm::errs() << "address width " << address->getWidth() << "\n";
  if (address->getWidth() < Expr::Int64)
     address = ZExtExpr::alloc(address, Expr::Int64); 

  // fast path: single in-bounds resolution
  ObjectPair op;
  bool success;
  solver->setTimeout(coreSolverTimeout);
  if (!state.addressSpace.resolveOne(state, solver, address, op, success)) {
    address = toConstant(state, address, "resolveOne failure");
    llvm::errs() << "address2 width " << address->getWidth() << "\n"; 
    success = state.addressSpace.resolveOne(cast<ConstantExpr>(address), op);
  }
  solver->setTimeout(0);

  if (success) {
    const MemoryObject *mo = op.first;
    ref<Expr> baseAddress = mo->getBaseExpr();

    if (MaxSymArraySize && mo->size>=MaxSymArraySize) {
      address = toConstant(state, address, "max-sym-array-size");
    }

    ref<Expr> offset = mo->getOffsetExpr(address);
    //#ifdef VB
    llvm::errs() << "address for memop " << address << "\n";
    llvm::errs() << "default offset for target address " << offset << "\n";
    llvm::errs() << "base memory address " << mo->getBaseExpr() << "\n";
    //#endif
    /* SYSREL EXTENSION */
    //if (forcedOffset) {
       //offset = ConstantExpr::alloc(0, Expr::Int64);
       //#ifdef VB
       llvm::outs() << "(forced) offset for target address " << offset << "\n";
      //#endif
    //}
    /* SYSREL EXTENSION */



    bool inBounds;
    solver->setTimeout(coreSolverTimeout);
    bool success = solver->mustBeTrue(state,
                                      mo->getBoundsCheckOffset(offset, bytes),
                                      inBounds);
    solver->setTimeout(0);
    if (!success) {
      state.pc = state.prevPC;
      terminateStateEarly(state, "Query timed out (bounds check).");
      return false;
    }
    //#ifdef VB
    llvm::outs() << "bounds check expression " << mo->getBoundsCheckOffset(offset, bytes) << "\n";
    llvm::outs() << "in bounds? " << inBounds << "\n";
    //#endif

    if (inBounds) {
      const ObjectState *os = op.second;
      if (isWrite) {
        if (os->readOnly) {
          terminateStateOnError(state, "memory error: object read only",
                                ReadOnly);
        } else {
          ObjectState *wos = state.addressSpace.getWriteable(mo, os);
          /* SYSREL EXTENSION */
          if (forcedOffset)
             wos->write(offset/*ConstantExpr::alloc(0, Expr::Int64)*/, value);
          else
          /* SYSREL EXTENSION */
             // update info flow
             /*ConstantExpr *oexpr = dyn_cast<ConstantExpr>(offset);
             ConstantExpr *cexpr = dyn_cast<ConstantExpr>(value);
             if (cexpr) {
                region r;
                if (oexpr) {  
                   r.offset = oexpr->getZExtValue();
                   r.size = bytes; 
                }
                else { // conservatively considering the whole range of the target
                   r.offset = 0;
                   r.size = op.first->size;
                }
                clearHighMemoryRegion(state, address, r);
                if (state.isSymbolic(mo))                
                   clearHighSymRegion(mo,r);
             }
             else {
                std::vector<region> rs;
                if (exprHasSymRegion(cexpr, true)) {
                   region source;
                   source.offset = 0;
                   source.size = cexpr->width * 8;
                   rs = extractRegion(cexpr, source, true);
                   if (rs.size() > 0) {
                      unsigned int diff = 0;
                      if (oexpr) 
                         diff = oexpr->getZExtValue();
                      moveOffset(rs, diff);
                      setHighMemoryRegion(state, address, rs);
                      if (state.isSymbolic(mo))
                         setHighSymRegion(m->name, rs);
                   }
                }
             }*/
             wos->write(offset, value);

            #ifdef INFOFLOW
            // info flow write case 1
            llvm::errs() << "case 1: " << target << "\n";
            updateInfoFlowForStore(state, regIndex, destregIndex, mo, target, offset, type);
            #endif

          #ifdef VB
          llvm::outs() << "just wrote:\n";
          #endif
        }
      } else {
        ref<Expr> result = os->read(offset, type);

        #ifdef INFOFLOW
        // info flow load case 1
        updateInfoFlowForLoad(state, regIndex, destregIndex, mo, target, offset, type);  
        #endif


        if (interpreterOpts.MakeConcreteSymbolic)
          result = replaceReadWithSymbolic(state, result);


        /* SYSREL EXTENSION */
       if (lazyInit) {
        if (!dyn_cast<ConstantExpr>(result)) {
            #ifdef VB
            llvm::errs() << "load orig result: " << result << "\n";
            #endif
            bool lazyInitTemp = false, singleInstance = false;
            llvm::Instruction *inst = state.prevPC->inst;
            llvm::LoadInst *li = dyn_cast<llvm::LoadInst>(inst);
            if (li) {
               Type *t = li->getPointerOperand()->getType();
               int count = 0;
               lazyInitTemp = isLazyInit(t, singleInstance, count);
               //std::string type_str;
               //llvm::raw_string_ostream rso(type_str);
               //t->print(rso);
               std::string rsostr = getTypeName(t);
               //#ifdef VB 
               llvm::errs() << "Is " << rsostr << " (count=" << count << ") to be lazy init?\n";
               inst->dump();
               //#endif
               if (lazyInitTemp) {
                  if (t->isPointerTy()) {
                     t = t->getPointerElementType();
                     if (t->isPointerTy())
                        t = t->getPointerElementType();
                     else
                        assert(false && "Expected a pointer type for lazy init");
                  }
                  else
                     assert(false && "Expected a pointer type for lazy init");
                 //#ifdef VB
                  llvm::errs() << "Yes!\n";
                  llvm::errs() << "original load result: " << result << " in state " <<&state << "\n";
                 //#endif
                  //std::string type_str2;
                  //llvm::raw_string_ostream rso2(type_str2);
                  //t->print(rso2);
                  std::string rso2str = getTypeName(t);
                  //#ifdef VB
                  llvm::errs() << "Allocating memory for type " << rso2str << " of size " << "\n";
                  //#endif
                  ref<Expr> laddr;
                  llvm::Type *rType;
                  bool mksym;
                  bool abort = false;
                  const MemoryObject *mo = memory->allocateLazyForTypeOrEmbedding(state,
                              state.prevPC->inst, t, t, singleInstance, count, rType, laddr, mksym, abort);
                  if (abort) {
                     return true;
                  }
                  

                  //#ifdef VB
                  llvm::errs() << "lazy init mem obj addr=" << laddr << " in state " << &state << " count=" << count << "\n";
                  //#endif
                  mo->name = getUniqueSymRegion(rso2str);
                  if (mksym) {
                     executeMakeSymbolic(state, mo, mo->name, t, true);
                     //llvm::errs() <<  "Making lazy init object at " << laddr << " symbolic \n";
                  } 
                  //#ifdef VB
                  llvm::errs() << "lazy initializing writing " << laddr << "( inside " << mo->getBaseExpr() << ") to " << address << " in state " << &state << "\n";
                  //#endif
                  forcedOffset = true;
                  executeMemoryOperation(state, -2, -2, true, address, laddr, target);
                  forcedOffset = false;
                  result = laddr;
                  /* side channel */
                  // check if initializing a pointer field of a high security memory object
                  if (isInHighMemoryRegion(state, baseAddress, offset, Expr::Int8, type, true)) {
                     std::vector<region> rs; 
                     if (highTypeRegions.find(rso2str) != highTypeRegions.end()) {
                        rs = highTypeRegions[rso2str];
                     }
                     else { // if not specified assume the whole region is high security sensitive
                        region r;
                        r.offset = 0;
                        r.size = mo->size * 8;
                        rs.push_back(r);
                     }
                     setHighMemoryRegion(state, mo->getBaseExpr(), rs);
                     setHighSymRegion(mo->name, rs);
                     mo->ishigh = true;
                     #ifdef VB
                     llvm::errs() << "lazy initialized memory object " << mo->name << " at " <<  laddr 
                                  << " and container base " << mo->getBaseExpr() << " marked security sensitive\n";
                     #endif
                  }
                  if (isInLowMemoryRegion(state, baseAddress, offset, Expr::Int8, type, true)) {
                     std::vector<region> rs; 
                     if (lowTypeRegions.find(rso2str) != lowTypeRegions.end()) {
                        rs = lowTypeRegions[rso2str];
                     }
                     else { // if not specified assume the whole region is  low security sensitive
                        region r;
                        r.offset = 0;
                        r.size = mo->size * 8;
                        rs.push_back(r);
                     }
                     setLowMemoryRegion(state, mo->getBaseExpr(), rs);
                     setLowSymRegion(mo->name, rs);
                     mo->islow = true;
                     #ifdef VB
                     llvm::errs() << "lazy initialized memory object " << mo->name << " at " <<  laddr 
                                  << " and container base " << mo->getBaseExpr() << " marked security insensitive\n";
                     #endif
                  }


                  /* side channel */
              }
            }
        }
       }
        /* SYSREL EXTENSION */

        //#ifdef VB
        if (getDestCell(state, target).value.get())
         llvm::errs() << "address to be dest: " << getDestCell(state, target).value << "\n";
        //#endif

        bindLocal(target, state, result);


       #ifdef VB
       llvm::errs() << " load result: " << result << "\n";
       #endif

       if (lazyInit) {
        if (!forcedOffset) {
          Type *t;
          if (isWrite) {
             t = ((StoreInst*)target->inst)->getPointerOperand()->getType();
          }
          else  t = ((LoadInst*)target->inst)->getPointerOperand()->getType();
          if (t->isPointerTy()) {
              t = t->getPointerElementType();
              if (!t->isPointerTy()) { // not interested in the pointer's address
                 std::string tname = getTypeName(t);
                 state.typeToAddr[t] =  address;
                 #ifdef VB
                 llvm::outs() <<  "recorded memory to type mapping (1) as " <<  address << "->" << tname <<"\n";
                 #endif
              }
          }
        }
       }

       }

      return false;
    }
  }

  // we are on an error path (no resolution, multiple resolution, one
  // resolution with out of bounds)

  ResolutionList rl;
  bool incomplete = false;

  bool craftedResList = false;
  if (PreferredResolution) { // use the base address stored in getelementptr to use the MemoryObject for resolution
     llvm::errs() << "Checking if we can identify the single resolution candidate with PreferredResolution option!\n"; 
     if (symIndexToMemBase.find((long)&state) != symIndexToMemBase.end()) {
          std::map<ref<Expr>, ref<Expr> > m = symIndexToMemBase[(long)&state];
          ref<Expr> addtocheck = address;
          while (m.find(addtocheck) != m.end()) {
             llvm::errs() << " checking mapping for " << addtocheck << "\n";
             ref<Expr> mobase = m[addtocheck];
             if (!isa<ConstantExpr>(mobase)) {
                // non-constant; keep checking the address it gets mapped 
                addtocheck = mobase;
                continue;
             }
             if (addressToMemObj.find((long)&state) != addressToMemObj.end()) {
                std::map<ref<Expr>, const MemoryObject *> m2 = addressToMemObj[(long)&state];
                if (m2.find(mobase) != m2.end()) {
                   const MemoryObject *mres = m2[mobase];
                   const ObjectState *ores = state.addressSpace.findObject(mres);
                   if (mres && ores) {
                      ObjectPair opres;
                      opres.first = mres;
                      opres.second = ores;
                      rl.push_back(opres);
                      craftedResList = true; 
                      llvm::errs() << "Using mem obj with base " << mobase << " to resolve " << address << "\n";
                      break;
                   } 
                   else break;
                }
                else { 
                   if (addtocheck == mobase) break;
                   llvm::errs() << "memobj for " << mobase << " in state " << &state << " not found \n"; 
                   addtocheck = mobase;                   
                }
             }
             else break;
          }
       }
  }

  if (!craftedResList) {
    solver->setTimeout(coreSolverTimeout);
    incomplete = state.addressSpace.resolve(state, solver, address, rl,
                                               0, coreSolverTimeout);
    solver->setTimeout(0);
  }

  // XXX there is some query wasteage here. who cares?
  ExecutionState *unbound = &state;

  int index = 0;
  const MemoryObject *mo; 
  for (ResolutionList::iterator i = rl.begin(), ie = rl.end(); i != ie; ++i) {
    mo = i->first;
    const ObjectState *os = i->second;
    ref<Expr> inBounds = mo->getBoundsCheckPointer(address, bytes);

    StatePair branches = fork(*unbound, inBounds, true);
    ExecutionState *bound = branches.first;

    // bound can be 0 on failure or overlapped
    if (bound) {
      
      llvm::errs() << "inbound case MemObj base=" << mo->getBaseExpr() << " mo size=" << mo->size << " PC:\n";
      ExprPPrinter::printConstraints(llvm::errs(), bound->constraints);

      if (isWrite) {
        if (os->readOnly) {
          terminateStateOnError(*bound, "memory error: object read only",
                                ReadOnly);
        } else {
          ObjectState *wos = bound->addressSpace.getWriteable(mo, os);
          if (forcedOffset)
             wos->write(mo->getOffsetExpr(address)/*ConstantExpr::alloc(0, Expr::Int64)*/, value);
          else {
             wos->write(mo->getOffsetExpr(address), value);
             #ifdef INFOFLOW
             // info flow write case 2
             ref<Expr> oe = mo->getOffsetExpr(address);
             llvm::errs() << "case 2: " << target << "\n";
             updateInfoFlowForStore(*bound, regIndex, destregIndex, mo, target, oe, type);
             #endif
          }
          #ifdef VB
          llvm::outs() << "just wrote:\n";
          #endif
        }
      } else {
        ref<Expr> result;
        //llvm::errs() << "on an error path in executeMem, going to read!\n";
        if (forcedOffset)
           result = os->read(mo->getOffsetExpr(address)/*ConstantExpr::alloc(0, Expr::Int64)*/, type);
        else {
           result = os->read(mo->getOffsetExpr(address), type);
           #ifdef INFOFLOW
           // info flow load case 2
           ref<Expr> oe = mo->getOffsetExpr(address);
           updateInfoFlowForLoad(*bound, regIndex, destregIndex, mo, target, oe, type); 
           #endif 
        }
        bindLocal(target, *bound, result);
        #ifdef VB
        llvm::outs() << " load result: " << result << "\n";
        #endif 
      }
      
    }

    unbound = branches.second;
    if (!unbound)
      break;

    /* SYSREL extension */
    // Bounding path forks due to multiple resolutions
    index++;
    if (index > maxForkMulRes)
       break; 
    /* SYSREL extension */ 
  }

  // XXX should we distinguish out of bounds and overlapped cases?
  if (unbound) {
    if (incomplete) {
      terminateStateEarly(*unbound, "Query timed out (resolve).");
    } else {
      //#ifdef VB
      llvm::outs() << "Offending address for unbound" << address << "\n";
      ExprPPrinter::printConstraints(llvm::errs(), unbound->constraints);
      //#endif

      std::string s;
      llvm::raw_string_ostream ors(s);
      ors << (*target->inst) ;
      //if (mo)
      //   ors << "memory error: out of bound pointer, mem base=" << mo->getBaseExpr() << " offsending address = " << address << " mem size= " << mo->size << "\n";
      state.dumpStack(ors);
      #ifdef VB 
      llvm::outs() << ors.str() ;
      ExprPPrinter::printConstraints(llvm::outs(), state.constraints);
      #endif
      terminateStateOnError(*unbound, ors.str()/* "memory error: out of bound pointer"*/, Ptr,
                            NULL, getAddressInfo(*unbound, address));
    }
  }
  return false;
}

void Executor::executeMakeSymbolic(ExecutionState &state,
                                   const MemoryObject *mo,
                                   const std::string &name,
                                   Type *t,
                                   bool clrFncPtr) {
  // Create a new object state for the memory object (instead of a copy).
  if (!replayKTest) {
    // Find a unique name for this array.  First try the original name,
    // or if that fails try adding a unique identifier.
    unsigned id = 0;
    std::string uniqueName = name;
    while (!state.arrayNames.insert(uniqueName).second) {
      uniqueName = name + "_" + llvm::utostr(++id);
    }
    const Array *array = arrayCache.CreateArray(uniqueName, mo->size);
    ObjectState *os = bindObjectInState(state, mo, false, array);
    state.addSymbolic(mo, array);
    /* SYSREL extension */
    if (clrFncPtr)
       clearFunctionPointers(mo, os, t);
    initializeLazyInit(mo, os, t);
    /* SYSREL extension */

    std::map< ExecutionState*, std::vector<SeedInfo> >::iterator it =
      seedMap.find(&state);
    if (it!=seedMap.end()) { // In seed mode we need to add this as a
                             // binding.
      for (std::vector<SeedInfo>::iterator siit = it->second.begin(),
             siie = it->second.end(); siit != siie; ++siit) {
        SeedInfo &si = *siit;
        KTestObject *obj = si.getNextInput(mo, NamedSeedMatching);

        if (!obj) {
          if (ZeroSeedExtension) {
            std::vector<unsigned char> &values = si.assignment.bindings[array];
            values = std::vector<unsigned char>(mo->size, '\0');
          } else if (!AllowSeedExtension) {
            terminateStateOnError(state, "ran out of inputs during seeding",
                                  User);
            break;
          }
        } else {
          if (obj->numBytes != mo->size &&
              ((!(AllowSeedExtension || ZeroSeedExtension)
                && obj->numBytes < mo->size) ||
               (!AllowSeedTruncation && obj->numBytes > mo->size))) {
	    std::stringstream msg;
	    msg << "replace size mismatch: "
		<< mo->name << "[" << mo->size << "]"
		<< " vs " << obj->name << "[" << obj->numBytes << "]"
		<< " in test\n";

            terminateStateOnError(state, msg.str(), User);
            break;
          } else {
            std::vector<unsigned char> &values = si.assignment.bindings[array];
            values.insert(values.begin(), obj->bytes,
                          obj->bytes + std::min(obj->numBytes, mo->size));
            if (ZeroSeedExtension) {
              for (unsigned i=obj->numBytes; i<mo->size; ++i)
                values.push_back('\0');
            }
          }
        }
      }
    }
  } else {
    ObjectState *os = bindObjectInState(state, mo, false);
    if (replayPosition >= replayKTest->numObjects) {
      terminateStateOnError(state, "replay count mismatch", User);
    } else {
      KTestObject *obj = &replayKTest->objects[replayPosition++];
      if (obj->numBytes != mo->size) {
        terminateStateOnError(state, "replay size mismatch", User);
      } else {
        for (unsigned i=0; i<mo->size; i++)
          os->write8(i, obj->bytes[i]);
      }
    }
  }
}

/***/

/* SYSREL EXTENSION */

void Executor::clearFunctionPointers(const MemoryObject *mo, ObjectState *os, Type *at) {
    if (!InitFuncPtrs)
       return;
    llvm::StructType *st = dyn_cast<llvm::StructType>(at);
    if (st) {
       #ifdef VB
       llvm::errs() << "Struct type=" << getTypeName(st) << "\n";
       #endif
       const llvm::DataLayout &dl = kmodule->module->getDataLayout();
       const llvm::StructLayout *sl =  dl.getStructLayout(st);
       for(unsigned i=0; i < st->getNumElements(); i++) {
          Type *temp = st->getElementType(i);
          #ifdef VB
          llvm::errs() << "field " << i << " offset= " << sl->getElementOffset(i) << " type=" << getTypeName(temp) << " typeid=" << temp->getTypeID() << " vs " << llvm::Type::FunctionTyID << "\n";
          #endif
          if (temp->isPointerTy()) {
             temp = temp->getPointerElementType();
             #ifdef VB 
             llvm::errs() << "pointer element of field" << i << " offset= " << sl->getElementOffset(i) << " type=" << getTypeName(temp) << " typeid=" << temp->getTypeID() << " vs " << llvm::Type::FunctionTyID << "\n";
             #endif
             // The second condition is to deal with a bug in the clang compiler..
             // Otherwise, some function pointers are not recognized. This may have a side effect of concretizing
             // some of the symbolic pointers and hence preventing lazy initialization of such pointer fields..
             if (temp->isFunctionTy() || getTypeName(temp) == "{}") {
                #ifdef VB
                llvm::errs() << "Setting function pointer field " << i << " of (out of " << st->getNumElements() << ")" << getTypeName(st) << "\n";
                llvm::errs() << "base addr=" << mo->getBaseExpr() << " offset " << sl->getElementOffset(i) << " type " << getTypeName(temp) << "\n";
                #endif 
                os->write(ConstantExpr::create(sl->getElementOffset(i), 64), Expr::createPointer(0));
             }
          }
       }
    }
}


void Executor::initArgsAsSymbolic(ExecutionState &state, Function *entryFunc, bool &abort, bool nosym) {
   #ifdef INFOFLOW
   if (entryFunc == infoFlowSummarizedFunc)
      recordArgRegisters(entryFunc);
   #endif
   std::string fname = entryFunc->getName();
   std::set<int> argH, argL, argM;
   if (highFunctionArgs.find(fname) != highFunctionArgs.end())
      argH = highFunctionArgs[fname];
   if (lowFunctionArgs.find(fname) != lowFunctionArgs.end())
      argL = lowFunctionArgs[fname];
   if (mixedFunctionArgs.find(fname) != mixedFunctionArgs.end())
      argM = mixedFunctionArgs[fname];   
   std::string uniquefname = getUniqueSymRegion(fname);
   KFunction *kf = kmodule->functionMap[entryFunc];
   unsigned int ind = 0;
   unsigned int argi = 0;
   for(llvm::Function::arg_iterator ai = entryFunc->arg_begin(); ai != entryFunc->arg_end(); ai++, argi++) {
     Type *at = ai->getType();
     std::string type_str;
     llvm::raw_string_ostream rso(type_str);
     at->print(rso);
     llvm::errs() << "base type pointer=" << at << "\n";
     if (at->isPointerTy()) {
        at = at->getPointerElementType();
        bool singleInstance = false;
        int count = 1; // force symbolic init of pointers to array types
        bool lazyInitT = isAllocTypeLazyInit(at, singleInstance, count);
        //#ifdef VB
        llvm::errs() << "arg " << ind << " type " << getTypeName(at) << " pointer= " << at << " count=" << count << "\n";
        //#endif
        //if (lazyInitT) {
          /*
           // if single instance and already init use that one
           MemoryObject *mo = memory->getInstanceForType(state, at);
           if (singleInstance && (mo || isEmbeddedType(at))) {
              if (mo) {
                 llvm::outs() << "reusing single instance at address " << mo->getBaseExpr() << "\n";
                 bindArgument(kf, ind, state, mo->getBaseExpr());
              }
              else { //if (isEmbeddedType(at)) {
                  // check type address mappings
                  bool rs = false;
                  ref<Expr> eta = memory->getInstanceAddressForType(state, at, rs);
                  std::string tname = getTypeName(at);
                  if (rs) {
                     bindArgument(kf, ind, state, eta);
                     llvm::outs() << "reusing single instance for embedded type " << tname << " at address " << eta << "\n";
                  }
                  else {
                     llvm::errs() << "embedded type: " <<tname << "\n";
                     assert(0 && "Single instance for embedded type could not be found!\n");
                  }
              }
           }
           else {// otherwise
           */
              // We now handle all these individual cases in a uniform way!!!
              ref<Expr> laddr;
              llvm::Type *rType;
              bool mksym;
              const MemoryObject *mo = memory->allocateLazyForTypeOrEmbedding(state, state.prevPC->inst, at, at, singleInstance,
                              count, rType, laddr, mksym, abort); 
              if (abort) return;
              //#ifdef VB
              llvm::errs() << "Symbolizing arg " << argi << " of " << entryFunc->getName() << ", address " << mo->getBaseExpr() << " size=" << mo->size << "\n";
              //mo = memory->allocateForLazyInit(state, state.prevPC->inst, at, singleInstance, count, laddr);
              llvm::errs() << "is arg " << ind <<  " type " << rso.str() << " single instance? " << singleInstance << "\n";
              llvm::errs() << "to be made symbolic? " << mksym << "\n";
              //#endif
              mo->name = getUniqueSymRegion(fname + std::string("_arg_") + std::to_string(ind));
              #ifdef VB
              llvm::errs() << "lazy init arg " << mo->name << "\n";
              #endif 
              std::string atname = getTypeName(at);
              // Use the input arg type hint, if any!
              bool hintFound = false;
              if (funcArgTypeHints.find(fname) != funcArgTypeHints.end())
                 if (funcArgTypeHints[fname].find(argi) != funcArgTypeHints[fname].end()) {
                    hintFound = true; 
                    atname = funcArgTypeHints[fname][argi];
                 }
              // check if it is a security sensitive arg, if so mark the memory object ishigh
              if (argH.find(argi) != argH.end()) {
                 std::vector<region> rs; 
                 if (highTypeRegions.find(atname) != highTypeRegions.end()) {
                    rs = highTypeRegions[atname];
                    llvm::errs() << "high security regions from type def\n";
                    for(unsigned int hr=0; hr<rs.size(); hr++)
                       llvm::errs() << rs[hr].offset << "," << rs[hr].size << "\n"; 
                 }
                 else { // if not specified assume the whole region is high security sensitive
                    region r;
                    r.offset = 0;
                    r.size = mo->size * 8;
                    rs.push_back(r); 
                    llvm::errs() << " setting high security region offset= " << r.offset << " size= " << r.size << "\n";
                 }
                 mo->ishigh = true;
                 setHighMemoryRegion(state, mo->getBaseExpr(), rs);
                 setHighSymRegion(mo->name, rs);
                 //#ifdef VB
                 llvm::errs() << "recording address of " << mo->name << " " << mo->getBaseExpr() << " as high security sensitive\n";
                 //#endif 
              }
              else if (argL.find(argi) != argL.end()) {
                 std::vector<region> rs;
                 if (lowTypeRegions.find(atname) != lowTypeRegions.end()) {
                    rs = lowTypeRegions[atname];
                 }
                 else { // if not specified assume the whole region is  low security sensitive
                    region r;
                    r.offset = 0;
                    r.size = mo->size * 8;
                    rs.push_back(r);
                    llvm::errs() << " setting low security region offset= " << r.offset << " size= " << r.size << "\n";
                 }
                 mo->islow = true;
                 setLowMemoryRegion(state, mo->getBaseExpr(), rs);
                 setLowSymRegion(mo->name, rs);
                 //#ifdef VB
                 llvm::errs() << "recording address of " << mo->name << " " << mo->getBaseExpr() << " as low security insensitive\n";
                 //#endif 
              }
              else if (argM.find(argi) != argM.end()) {
                 if (!hintFound && lowTypeRegions.find(atname) == lowTypeRegions.end() && 
                       highTypeRegions.find(atname) == highTypeRegions.end())
                    assert(false && "Mixed sensitivity arg without type based sensitive regions!\n");
                 if (lowTypeRegions.find(atname) != lowTypeRegions.end()) {
                    std::vector<region> rs = lowTypeRegions[atname]; 
                    setLowMemoryRegion(state, mo->getBaseExpr(), rs);
                    setLowSymRegion(mo->name, rs); 
                 }
                 if (highTypeRegions.find(atname) != highTypeRegions.end()) {
                    std::vector<region> rs = highTypeRegions[atname];  
                    llvm::errs() << "high security regions from type def\n";
                    for(unsigned int hr=0; hr<rs.size(); hr++)
                       llvm::errs() << rs[hr].offset << "," << rs[hr].size << "\n"; 
                    setHighMemoryRegion(state, mo->getBaseExpr(), rs);
                    setHighSymRegion(mo->name, rs); 
                    //#ifdef VB
                    llvm::errs() << "recording address of " << mo->name << " " << mo->getBaseExpr() << " as high security insensitive\n";
                    //#endif            
                 }
              }
              if (!nosym/* && mksym*/) {
                 executeMakeSymbolic(state, mo,
                                     mo->name,
                                     at, true);
              }
              bindArgument(kf, ind, state, laddr);
              #ifdef VB
              llvm::outs() << "binding arg " << ind << " of type " << rso.str() << " to address " << laddr <<
                                                                " (in " << mo->getBaseExpr() << ")\n";
              #endif
           //}
        //}
       // else llvm::outs() << "skipping lazy init for " << rso.str() << "\n";
     }
     else {
        size_t allocationAlignment = 8;
        Instruction *inst = &*(entryFunc->begin()->begin());
        const llvm::DataLayout & dl = inst->getParent()->getParent()->getParent()->getDataLayout();
        MemoryObject *mo =  memory->allocate(dl.getTypeAllocSize(at), false, /*true*/false, inst, allocationAlignment);
        recordMemObj(state, mo);
        mo->name = getUniqueSymRegion(fname  + std::string("_arg_") + std::to_string(ind));
        llvm::errs() << "lazy init arg " << mo->name << "\n";
        // we're mimicking what executeMemoryOperation do without a relevant load or store instruction
        const Array *array = arrayCache.CreateArray(mo->name, mo->size);
        ObjectState *sos = bindObjectInState(state, mo, true, array);
        state.addSymbolic(mo, array);
        ref<Expr> result = sos->read(ConstantExpr::alloc(0, Expr::Int64), getWidthForLLVMType(at));
        bindArgument(kf, ind, state, result);
        #ifdef VB
        llvm::outs() << "binding nonpointer arg " << ind << " of type " << rso.str() << " to value " << result << "\n";
        #endif
        std::string atname = getTypeName(at);
        // check if it is a security sensitive arg, if so mark the memory object ishigh
        if (argH.find(argi) != argH.end()) {
           std::vector<region> rs; 
           if (highTypeRegions.find(atname) != highTypeRegions.end()) {
              rs = highTypeRegions[atname];
           }
           else { // if not specified assume the whole region is high security sensitive
              region r;
              r.offset = 0;
              r.size = mo->size * 8;
              rs.push_back(r); 
              llvm::errs() << " setting high security region offset= " << r.offset << " size= " << r.size << "\n";
           }
           mo->ishigh = true;
           setHighMemoryRegion(state, mo->getBaseExpr(), rs);
           setHighSymRegion(mo->name, rs);
           //#ifdef VB
            llvm::errs() << "recording address of " << mo->name << " " << mo->getBaseExpr() << " as security sensitive \n";
           //#endif 
        }
        else if (argL.find(argi) != argL.end()) {
           std::vector<region> rs;
           if (lowTypeRegions.find(atname) != lowTypeRegions.end()) {
              rs = lowTypeRegions[atname];
           }
           else { // if not specified assume the whole region is  low security sensitive
              region r;
              r.offset = 0;
              r.size = mo->size * 8;
              rs.push_back(r);
              llvm::errs() << " setting low security region offset= " << r.offset << " size= " << r.size << "\n";
           }
           mo->islow = true;
           setLowMemoryRegion(state, mo->getBaseExpr(), rs);
           setLowSymRegion(mo->name, rs);
           //#ifdef VB
            llvm::errs() << "recording address of " << mo->name << " " << mo->getBaseExpr() << " as not security sensitive\n"; 
           //#endif 
        }        
        else if (argM.find(argi) != argM.end()) {
           if (lowTypeRegions.find(atname) == lowTypeRegions.end() && 
              highTypeRegions.find(atname) == highTypeRegions.end())
              assert(false && "Mixed sensitivity arg without type based sensitive regions!\n");
           if (lowTypeRegions.find(atname) != lowTypeRegions.end()) {
              std::vector<region> rs = lowTypeRegions[atname]; 
              setLowMemoryRegion(state, mo->getBaseExpr(), rs);
              setLowSymRegion(mo->name, rs); 
           }
           if (highTypeRegions.find(atname) != highTypeRegions.end()) {
              std::vector<region> rs = highTypeRegions[atname];  
              setHighMemoryRegion(state, mo->getBaseExpr(), rs);
              setHighSymRegion(mo->name, rs); 
           }
        }

     }
     ind++;
   }

}

/* SYSREL EXTENSION */

/***/

void Executor::runFunctionAsMain(Function *f,
				 int argc,
				 char **argv,
				 char **envp) {

  /* SYSREL EXTENSION */
  entryFunc = f;
  /* SYSREL EXTENSION */

  std::vector<ref<Expr> > arguments;

  // force deterministic initialization of memory objects
  srand(1);
  srandom(1);

  MemoryObject *argvMO = 0;

  // In order to make uclibc happy and be closer to what the system is
  // doing we lay out the environments at the end of the argv array
  // (both are terminated by a null). There is also a final terminating
  // null that uclibc seems to expect, possibly the ELF header?

  int envc;
  for (envc=0; envp[envc]; ++envc) ;

  #ifdef VB
  llvm::outs() << "num args " << argc << " num envc " << envc << "\n";
  #endif 

  unsigned NumPtrBytes = Context::get().getPointerWidth() / 8;
  KFunction *kf = kmodule->functionMap[f];
  /* SYSREL side channel begin */
  if (!fset){
        ff=f;
        fset = true;
  }
  #ifdef INFOFLOW
    infoFlowSummarizedFunc = moduleHandle->getFunction(infoFlowSummarizedFuncName);
    if (infoFlowSummaryMode && !infoFlowSummarizedFunc) 
       assert(false && "info flow summary cannot be computed for an unexisting function!");
  #endif
  /* SYSREL side channel end */
  assert(kf);
  Function::arg_iterator ai = f->arg_begin(), ae = f->arg_end();

  if (!lazyInit) {
  if (ai!=ae) {
    arguments.push_back(ConstantExpr::alloc(argc, Expr::Int32));
    if (++ai!=ae) {
      Instruction *first = &*(f->begin()->begin());
      argvMO =
          memory->allocate((argc + 1 + envc + 1 + 1) * NumPtrBytes,
                           /*isLocal=*/false, /*isGlobal=*/true,
                           /*allocSite=*/first, /*alignment=*/8);

      if (!argvMO)
        klee_error("Could not allocate memory for function arguments");

      arguments.push_back(argvMO->getBaseExpr());

      if (++ai!=ae) {
        uint64_t envp_start = argvMO->address + (argc+1)*NumPtrBytes;
        arguments.push_back(Expr::createPointer(envp_start));

        if (++ai!=ae)
          klee_error("invalid main function (expect 0-3 arguments)");
      }
    }
  }
 }

  ExecutionState *state = new ExecutionState(kmodule->functionMap[f]);
  recordMemObj(*state, argvMO);

  #ifdef INFOFLOW
  initInfoFlowContext(*state);
  #endif
  if (pathWriter)
    state->pathOS = pathWriter->open();
  if (symPathWriter)
    state->symPathOS = symPathWriter->open();


  if (statsTracker)
    statsTracker->framePushed(*state, 0);

  if (!lazyInit)
     assert(arguments.size() == f->arg_size() && "wrong number of arguments");

  /* SYSREL EXTENSION */
  if (lazyInit) {
     // to avoid memory out of bound errors due to lazy init of void pointers
     // currently relies on the input model
     // TODO: scan the code to find bitcasts and maximum possible target type size
     computeMaxVoidTypeCastSize();     
     /// visit all struct types in this module
     llvm::TypeFinder StructTypes;
     StructTypes.run(*(kmodule->module), true);
     for (auto *STy : StructTypes) {
         std::set<std::string> temp;
         collectEmbeddedPointerTypes(STy, lazyInits, temp);
     }

     // visit parameter types of all functions in this module
     for(llvm::Module::iterator fc = kmodule->module->begin(); fc != kmodule->module->end(); fc++) {
        for(llvm::Function::arg_iterator ai = fc->arg_begin(); ai != fc->arg_end(); ai++) {
           if (lazyInit) {
              std::set<std::string> temp;
              collectEmbeddedPointerTypes(ai->getType(), lazyInits, temp);
           }
        }
     }

     for (auto *STy : StructTypes) {
       for(unsigned i=0; i < STy->getNumElements(); i++) {
           Type *et = STy->getElementType(i);
           if (!et->isPointerTy()) {
              StructType *est = dyn_cast<StructType>(et);
              if (est) {
                 /*std::string type_str;
                 llvm::raw_string_ostream rso(type_str);
                 et->print(rso);
                 std::istringstream ist(rso.str());
                 std::string temp;
                 getline(ist, temp, '=');
                 std::string etypename = ltrim(rtrim(temp));
                 embeddedTypes.insert(etypename); */
                 embeddedTypes.insert(et);
                 /*std::set<std::string> embeddings;
                 std::string etname = getTypeName(STy);
                 if (embeddingTypes.find(etypename) != embedingTypes.end())
                     embeddings = embeddingTypes[etypename];
                 embeddings.insert(etname);
                 embeddingTypes[etypename] = embeddings;*/
                 std::set<Type*> embeddings;
                 if (embeddingTypes.find(et) != embeddingTypes.end())
                     embeddings = embeddingTypes[et];
                 embeddings.insert(STy);
                 embeddingTypes[et] = embeddings;
                 #ifdef VB
                 llvm::outs() << getTypeName(et) << " is an embedded type, embedded by " << getTypeName(STy) << "\n";
                 #endif
              }
           }
       }
     }

  }
  /* SYSREL EXTENSION */


  if (!lazyInit) {
  for (unsigned i = 0, e = f->arg_size(); i != e; ++i) {
    bindArgument(kf, i, *state, arguments[i]);
  }
}

  if (argvMO) {
    ObjectState *argvOS = bindObjectInState(*state, argvMO, false);

    for (int i=0; i<argc+1+envc+1+1; i++) {
      if (i==argc || i>=argc+1+envc) {
        // Write NULL pointer
        argvOS->write(i * NumPtrBytes, Expr::createPointer(0));
      } else {
        char *s = i<argc ? argv[i] : envp[i-(argc+1)];
        int j, len = strlen(s);

        MemoryObject *arg =
            memory->allocate(len + 1, /*isLocal=*/false, /*isGlobal=*/true,
                             /*allocSite=*/state->pc->inst, /*alignment=*/8);
        if (!arg)
          klee_error("Could not allocate memory for function arguments");
        recordMemObj(*state, arg);

        ObjectState *os = bindObjectInState(*state, arg, false);
        for (j=0; j<len+1; j++)
          os->write8(j, s[j]);

        // Write pointer to newly allocated and initialised argv/envp c-string
        argvOS->write(i * NumPtrBytes, arg->getBaseExpr());
      }
    }
  }

  initializeGlobals(*state);
  /* SYSREL extension */
  // side channel
  //initHighLowAddress(*state);
  initHighLowRegions(*state);
  /* SYSREL extension */

  processTree = new PTree(state);
  state->ptreeNode = processTree->root;
  /* SYSREL extension */
  // the very first state should be constructed according to the first step of life-cycle model, in other words initialize
  if (state->hasLCM()) {
     state->updateLCMState();
     if (LifeCycleModelState::lcm->getStep(state->getLCMState())->getValue() == sidechannelentry)
        sidechannelstarted = true; 
  }
  if (lazyInit) {
   // Lazy init args of the entryFunc
   bool abort = false;
   initArgsAsSymbolic(*state, entryFunc, abort);
   printSymRegions();
   assert(!abort);
   /* To be replaced with initArgsAsSymbolic
   unsigned int ind = 0;
   for(llvm::Function::arg_iterator ai = entryFunc->arg_begin(); ai != entryFunc->arg_end(); ai++) {
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
           MemoryObject *mo = memory->allocateForLazyInit(state->prevPC->inst, at, singleInstance, count);
           mo->name = std::string("arg_") + std::to_string(ind);
           executeMakeSymbolic(*state, mo, std::string("arg_") + std::to_string(ind));
           bindArgument(kf, ind, *state, mo->getBaseExpr());
           llvm::outs() << "binding arg " << ind << " of type " << rso.str() << " to address " << mo->getBaseExpr() << "\n";
        }
     }
     ind++;
   }
   */
  }
  /* SYSREL extension */
  run(*state);
  delete processTree;
  processTree = 0;

  // hack to clear memory objects
  delete memory;
  memory = new MemoryManager(NULL);

  globalObjects.clear();
  globalAddresses.clear();
  /* SYSREL extension */
  //globalAddressesRev.clear();
  /* SYSREL extension */


  if (statsTracker)
    statsTracker->done();
}

unsigned Executor::getPathStreamID(const ExecutionState &state) {
  assert(pathWriter);
  return state.pathOS.getID();
}

unsigned Executor::getSymbolicPathStreamID(const ExecutionState &state) {
  assert(symPathWriter);
  return state.symPathOS.getID();
}

void Executor::getConstraintLog(const ExecutionState &state, std::string &res,
                                Interpreter::LogType logFormat) {

  switch (logFormat) {
  case STP: {
    Query query(state.constraints, ConstantExpr::alloc(0, Expr::Bool));
    char *log = solver->getConstraintLog(query);
    res = std::string(log);
    free(log);
  } break;

  case KQUERY: {
    std::string Str;
    llvm::raw_string_ostream info(Str);
    ExprPPrinter::printConstraints(info, state.constraints);
    res = info.str();
  } break;

  case SMTLIB2: {
    std::string Str;
    llvm::raw_string_ostream info(Str);
    ExprSMTLIBPrinter printer;
    printer.setOutput(info);
    Query query(state.constraints, ConstantExpr::alloc(0, Expr::Bool));
    printer.setQuery(query);
    printer.generateOutput();
    res = info.str();
  } break;

  default:
    klee_warning("Executor::getConstraintLog() : Log format not supported!");
  }
}

bool Executor::getSymbolicSolution(const ExecutionState &state,
                                   std::vector<
                                   std::pair<std::string,
                                   std::vector<unsigned char> > >
                                   &res) {
  solver->setTimeout(coreSolverTimeout);

  ExecutionState tmp(state);

  // Go through each byte in every test case and attempt to restrict
  // it to the constraints contained in cexPreferences.  (Note:
  // usually this means trying to make it an ASCII character (0-127)
  // and therefore human readable. It is also possible to customize
  // the preferred constraints.  See test/Features/PreferCex.c for
  // an example) While this process can be very expensive, it can
  // also make understanding individual test cases much easier.
  for (unsigned i = 0; i != state.symbolics.size(); ++i) {
    const MemoryObject *mo = state.symbolics[i].first;
    std::vector< ref<Expr> >::const_iterator pi =
      mo->cexPreferences.begin(), pie = mo->cexPreferences.end();
    for (; pi != pie; ++pi) {
      bool mustBeTrue;
      // Attempt to bound byte to constraints held in cexPreferences
      bool success = solver->mustBeTrue(tmp, Expr::createIsZero(*pi),
					mustBeTrue);
      // If it isn't possible to constrain this particular byte in the desired
      // way (normally this would mean that the byte can't be constrained to
      // be between 0 and 127 without making the entire constraint list UNSAT)
      // then just continue on to the next byte.
      if (!success) break;
      // If the particular constraint operated on in this iteration through
      // the loop isn't implied then add it to the list of constraints.
      if (!mustBeTrue) tmp.addConstraint(*pi);
    }
    if (pi!=pie) break;
  }

  std::vector< std::vector<unsigned char> > values;
  std::vector<const Array*> objects;
  for (unsigned i = 0; i != state.symbolics.size(); ++i)
    objects.push_back(state.symbolics[i].second);
  bool success = solver->getInitialValues(tmp, objects, values);
  solver->setTimeout(0);
  if (!success) {
    klee_warning("unable to compute initial values (invalid constraints?)!");
    if (!SolverImpl::interrupted || !SolverImpl::forceOutput) 
       ExprPPrinter::printQuery(llvm::errs(), state.constraints,
                             ConstantExpr::alloc(0, Expr::Bool));
    else statesWithCorruptedPC.insert((long)&state);;
    return false;
  }

  for (unsigned i = 0; i != state.symbolics.size(); ++i)
    res.push_back(std::make_pair(state.symbolics[i].first->name, values[i]));
  return true;
}

void Executor::getCoveredLines(const ExecutionState &state,
                               std::map<const std::string*, std::set<unsigned> > &res) {
  res = state.coveredLines;
}

void Executor::doImpliedValueConcretization(ExecutionState &state,
                                            ref<Expr> e,
                                            ref<ConstantExpr> value) {
  abort(); // FIXME: Broken until we sort out how to do the write back.

  if (DebugCheckForImpliedValues)
    ImpliedValue::checkForImpliedValues(solver->solver, e, value);

  ImpliedValueList results;
  ImpliedValue::getImpliedValues(e, value, results);
  for (ImpliedValueList::iterator it = results.begin(), ie = results.end();
       it != ie; ++it) {
    ReadExpr *re = it->first.get();

    if (ConstantExpr *CE = dyn_cast<ConstantExpr>(re->index)) {
      // FIXME: This is the sole remaining usage of the Array object
      // variable. Kill me.
      const MemoryObject *mo = 0; //re->updates.root->object;
      const ObjectState *os = state.addressSpace.findObject(mo);

      if (!os) {
        // object has been free'd, no need to concretize (although as
        // in other cases we would like to concretize the outstanding
        // reads, but we have no facility for that yet)
      } else {
        assert(!os->readOnly &&
               "not possible? read only object with static read?");
        ObjectState *wos = state.addressSpace.getWriteable(mo, os);
        wos->write(CE, it->second);
      }
    }
  }
}

Expr::Width Executor::getWidthForLLVMType(llvm::Type *type) const {
  /* SYSREL extension */
  //if (!type->isSized())
    //  return SIZE_FOR_UNKNOWN_TYPES;
  /* SYSREL extension */ 
  Expr::Width width = kmodule->targetData->getTypeSizeInBits(type);
  if (width == 0)
     width = 64;
  // hack
  return width; 
}


size_t Executor::getAllocationAlignment(const llvm::Value *allocSite) const {
  // FIXME: 8 was the previous default. We shouldn't hard code this
  // and should fetch the default from elsewhere.
  const size_t forcedAlignment = 8;
  size_t alignment = 0;
  llvm::Type *type = NULL;
  std::string allocationSiteName(allocSite->getName().str());
  if (const GlobalValue *GV = dyn_cast<GlobalValue>(allocSite)) {
    alignment = GV->getAlignment();
    if (const GlobalVariable *globalVar = dyn_cast<GlobalVariable>(GV)) {
      // All GlobalVariables's have pointer type
      llvm::PointerType *ptrType =
          dyn_cast<llvm::PointerType>(globalVar->getType());
      assert(ptrType && "globalVar's type is not a pointer");
      type = ptrType->getElementType();
    } else {
      type = GV->getType();
    }
  } else if (const AllocaInst *AI = dyn_cast<AllocaInst>(allocSite)) {
    alignment = AI->getAlignment();
    type = AI->getAllocatedType();
  } else if (isa<InvokeInst>(allocSite) || isa<CallInst>(allocSite)) {
    // FIXME: Model the semantics of the call to use the right alignment
    llvm::Value *allocSiteNonConst = const_cast<llvm::Value *>(allocSite);
    const CallSite cs = (isa<InvokeInst>(allocSiteNonConst)
                             ? CallSite(cast<InvokeInst>(allocSiteNonConst))
                             : CallSite(cast<CallInst>(allocSiteNonConst)));
    llvm::Function *fn =
        klee::getDirectCallTarget(cs, /*moduleIsFullyLinked=*/true);
    if (fn)
      allocationSiteName = fn->getName().str();

    
    llvm::errs() << (*allocSite) << "\n";
 
    klee_warning_once(fn != NULL ? fn : allocSite,
                      "Alignment of memory from call \"%s\" is not "
                      "modelled. Using alignment of %zu.",
                      allocationSiteName.c_str(), forcedAlignment);
    alignment = forcedAlignment;
  } else {
    llvm_unreachable("Unhandled allocation site");
  }

  if (alignment == 0) {
    assert(type != NULL);
    // No specified alignment. Get the alignment for the type.
    if (type->isSized()) {
      alignment = kmodule->targetData->getPrefTypeAlignment(type);
    } else {
      klee_warning_once(allocSite, "Cannot determine memory alignment for "
                                   "\"%s\". Using alignment of %zu.",
                        allocationSiteName.c_str(), forcedAlignment);
      alignment = forcedAlignment;
    }
  }

  // Currently we require alignment be a power of 2
  if (!bits64::isPowerOfTwo(alignment)) {
    klee_warning_once(allocSite, "Alignment of %zu requested for %s but this "
                                 "not supported. Using alignment of %zu",
                      alignment, allocSite->getName().str().c_str(),
                      forcedAlignment);
    alignment = forcedAlignment;
  }
  assert(bits64::isPowerOfTwo(alignment) &&
         "Returned alignment must be a power of two");
  return alignment;
}

void Executor::prepareForEarlyExit() {
  if (statsTracker) {
    // Make sure stats get flushed out
    statsTracker->done();
  }
}

/// Returns the errno location in memory
int *Executor::getErrnoLocation(const ExecutionState &state) const {
#ifndef __APPLE__
  /* From /usr/include/errno.h: it [errno] is a per-thread variable. */
  return __errno_location();
#else
  return __error();
#endif
}

///

Interpreter *Interpreter::create(LLVMContext &ctx, const InterpreterOptions &opts,
                                 InterpreterHandler *ih) {
  return new Executor(ctx, opts, ih);
}
