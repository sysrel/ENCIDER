//===-- SpecialFunctionHandler.h --------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_SPECIALFUNCTIONHANDLER_H
#define KLEE_SPECIALFUNCTIONHANDLER_H

#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <set>
#include "llvm/IR/DataLayout.h"

namespace llvm {
  class Function;
}

namespace klee {
  class Executor;
  class Expr;
  class ExecutionState;
  struct KInstruction;
  template<typename T> class ref;

  /* SYSREL EXTENSION */ 

  class MutexAPIHandler {
    private:
     std::set<std::string> initSet;
     std::map<std::string, std::string> initMap;
     std::map<std::string, std::string> acqRelMap;
     std::set<std::string> releaseSet;     
     bool handle(std::map<ref<Expr>, int> &stateModel, std::string fname, std::vector< ref<Expr> > &arguments);
    public:
     MutexAPIHandler();
     void addInitAcqRel(std::string acqf, std::string relf, std::string initf="");
     bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments);
     bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, int tid);
    
  };

  class RefCountAPIHandler {
    private:
     std::set<std::string> initSet;
     std::map<std::string, std::string> initMap;
     std::map<std::string, std::string> getputMap;
     std::set<std::string> putSet; 
     bool handle(std::map<ref<Expr>, int> &stateModel, std::string fname, std::vector< ref<Expr> > &arguments);
    public:      
     RefCountAPIHandler();
     void addGetPut(std::string getf, std::string putf, std::string initf="");
     bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments);      
     bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, int tid);      

  };
 
  class RegistrationAPIHandler {
    private:
     std::map<std::string, std::string> regAPIMap;
     std::map<std::string, std::string> unregAPIMap;
     std::map<std::string, std::set<std::string> > enabledFuncs;
     bool handle(std::map<ref<Expr>, int> &stateModel, std::string fname, std::vector< ref<Expr> > &arguments); 
    public:
     RegistrationAPIHandler();
     void addRegUnreg(std::string regf, std::string unregf, std::set<std::string> enabled);
     bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments);     
     bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, int tid);     
     bool getEnabled(std::string reg, std::set<std::string> &q);
     bool getDisabled(std::string unreg, std::set<std::string> &q);  
  };

  class ResourceAllocReleaseAPIHandler {
    private:
     std::map<std::string, std::string> resAPIMap;
     std::map<std::string, std::string> revresAPIMap;
     bool handle(std::map<ref<Expr>, int> &stateModel, std::string fname, std::vector< ref<Expr> > &arguments);
   public:
      ResourceAllocReleaseAPIHandler();
      void addResAllocDealloc(std::string regf, std::string unregf);
      bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments); 
      bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, int tid); 
  };

  class SetGetAPIHandler {
   private:
     std::map<std::string, std::string> setgetAPIMap;
     std::map<std::string, std::string> getsetAPIMap;
     bool handle(ExecutionState &state, int tid, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction /* null if set*/ *target);
   public:
     SetGetAPIHandler();
     void addSetGet(std::string set, std::string get);
     bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target);
     bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target, int tid);
  };   


   class AllocAPIHandler {
    private:
      std::map<std::string, int> allocAPIMap;  
      std::map<std::string, bool> allocAPIMapInit;  
      std::map<std::string, bool> allocAPIMapSym;
      std::map<std::string, bool> allocAPIMapRet;
      bool handle(ExecutionState &state, int tid, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target);
    public:
      AllocAPIHandler();
      void addAllocate(std::string alloc, int param, bool zeroMemory, bool sym, bool ret);
      bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target, bool &retstat);
      bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target, bool &retstat, int tid);       
   };

   class FreeAPIHandler {
     private:
       std::map<std::string, int> freeAPI;
       bool handle(ExecutionState &state, int tid, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target);
    public:
      FreeAPIHandler();
      void addFree(std::string fapi, int param);
      bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target);
      bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target, int tid);       
   };

   class ReadWriteAPIHandler {
    private:
     std::set<std::string> readAPI;
     std::set<std::string> writeAPI;
     std::map<std::string, std::pair<int,int> > writeAPIMap;
     bool handle(ExecutionState &state, int tid, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target);
    public:
      ReadWriteAPIHandler();
      void addRead(std::string read); 
      void addWrite(std::string write); 
      // parameter position for the source location and the destination location
      void addWrite(std::string write, int source, int dest); 
      bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target);
      bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target, int tid);
   };


   class IgnoreAPIHandler {
    private:
     std::set<std::string> ignoreAPI;
     bool handle(ExecutionState &state, int tid, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target);
    public:
     IgnoreAPIHandler();
     void addIgnore(std::string ignore);
     bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target);
     bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target, int tid); 
   };

   class CallbackAPIHandler {
    private:
     // for now we handle at most one callback from a given API 
     std::map<std::string, std::string> cbAPI;
     bool handle(ExecutionState &state, int tid, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target);
    public:
     CallbackAPIHandler();
     void addCallback(std::string api, std::string cb);
     bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target);
     bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target, int tid);               
   };

   class SideEffectAPIHandler {
     private:
      std::map<std::string, std::set<std::string> > sideEffectAPI;
      bool handle(ExecutionState &state, int tid, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target);
      ref<Expr> eval(ExecutionState &state,  const llvm::DataLayout &dl, llvm::Function *f, 
                         std::vector< ref<Expr> > &arguments, std::string expr, bool address);
     public:
      SideEffectAPIHandler(); 
      void addAPIUpdateExpr(std::string api, std::string expr);
      bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target);
      bool handle(ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target, int tid);         
   };

  /* SYSREL EXTENSION */ 

  class SpecialFunctionHandler {
  public:
    typedef void (SpecialFunctionHandler::*Handler)(ExecutionState &state,
                                                    KInstruction *target, 
                                                    std::vector<ref<Expr> > 
                                                      &arguments);
    typedef std::map<const llvm::Function*, 
                     std::pair<Handler,bool> > handlers_ty;

    handlers_ty handlers;
    class Executor &executor;

    struct HandlerInfo {
      const char *name;
      SpecialFunctionHandler::Handler handler;
      bool doesNotReturn; /// Intrinsic terminates the process
      bool hasReturnValue; /// Intrinsic has a return value
      bool doNotOverride; /// Intrinsic should not be used if already defined
    };

    // const_iterator to iterate over stored HandlerInfo
    // FIXME: Implement >, >=, <=, < operators
    class const_iterator : public std::iterator<std::random_access_iterator_tag, HandlerInfo>
    {
      private:
        value_type* base;
        int index;
      public:
      const_iterator(value_type* hi) : base(hi), index(0) {};
      const_iterator& operator++();  // pre-fix
      const_iterator operator++(int); // post-fix
      const value_type& operator*() { return base[index];}
      const value_type* operator->() { return &(base[index]);}
      const value_type& operator[](int i) { return base[i];}
      bool operator==(const_iterator& rhs) { return (rhs.base + rhs.index) == (this->base + this->index);}
      bool operator!=(const_iterator& rhs) { return !(*this == rhs);}
    };

    static const_iterator begin();
    static const_iterator end();
    static int size();



  public:
    SpecialFunctionHandler(Executor &_executor);

    /// Perform any modifications on the LLVM module before it is
    /// prepared for execution. At the moment this involves deleting
    /// unused function bodies and marking intrinsics with appropriate
    /// flags for use in optimizations.
    void prepare();

    /// Initialize the internal handler map after the module has been
    /// prepared for execution.
    void bind();

    bool handle(ExecutionState &state, 
                llvm::Function *f,
                KInstruction *target,
                std::vector< ref<Expr> > &arguments);

    /* Convenience routines */

    std::string readStringAtAddress(ExecutionState &state, ref<Expr> address);
    
    /* Handlers */

#define HANDLER(name) void name(ExecutionState &state, \
                                KInstruction *target, \
                                std::vector< ref<Expr> > &arguments)
    HANDLER(handleAbort);
    HANDLER(handleAssert);
    HANDLER(handleAssertFail);
    HANDLER(handleAssume);
    HANDLER(handleCalloc);
    HANDLER(handleCheckMemoryAccess);
    HANDLER(handleDefineFixedObject);
    HANDLER(handleDelete);    
    HANDLER(handleDeleteArray);
    HANDLER(handleExit);
    HANDLER(handleErrnoLocation);
    HANDLER(handleAliasFunction);
    HANDLER(handleFree);
    HANDLER(handleGetErrno);
    HANDLER(handleGetObjSize);
    HANDLER(handleGetValue);
    HANDLER(handleIsSymbolic);
    HANDLER(handleMakeSymbolic);
    HANDLER(handleMalloc);
    HANDLER(handleMarkGlobal);
    HANDLER(handleOpenMerge);
    HANDLER(handleCloseMerge);
    HANDLER(handleNew);
    HANDLER(handleNewArray);
    HANDLER(handlePreferCex);
    HANDLER(handlePosixPreferCex);
    HANDLER(handlePrintExpr);
    HANDLER(handlePrintRange);
    HANDLER(handleRange);
    HANDLER(handleRealloc);
    HANDLER(handleReportError);
    HANDLER(handleRevirtObjects);
    HANDLER(handleSetForking);
    HANDLER(handleSilentExit);
    HANDLER(handleStackTrace);
    HANDLER(handleUnderConstrained);
    HANDLER(handleWarning);
    HANDLER(handleWarningOnce);
    HANDLER(handleAddOverflow);
    HANDLER(handleMulOverflow);
    HANDLER(handleSubOverflow);
    HANDLER(handleDivRemOverflow);
#undef HANDLER
  };
} // End klee namespace

#endif
