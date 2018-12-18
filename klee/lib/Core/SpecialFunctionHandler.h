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
  class RefCountAPIHandler;
  class AllocAPIHandler;
  class FreeAPIHandler;
  class IgnoreAPIHandler;
  class SideEffectAPIHandler;
  class TerminateAPIHandler;
  class CallbackAPIHandler;
  class RefcountAPIHandler;
  class APIAction;
  class APIBlock;

  class APIHandler {
   protected:
    //static RegistrationAPIHandler  *regAPIHandler; 
    //static ResourceAllocReleaseAPIHandler *resADAPIHandler;
    //static MutexAPIHandler*  mutexAPIHandler;
    //static SetGetAPIHandler *setgetAPIHandler;
    //static ReadWriteAPIHandler *readWriteAPIHandler;
    ref<Expr> eval(ExecutionState &state,  const llvm::DataLayout &dl, llvm::Function *f, 
                     std::vector< ref<Expr> > &arguments, std::string expr, KInstruction *target, bool address);
    void executeCallback(ExecutionState &state, std::string cbexpr, std::vector< ref<Expr> > &arguments, 
         KInstruction *target, int tid=-1) ;
    static std::map<std::string, APIAction*> symbolTable;
    public:
     static void addAction(std::string, APIAction *action);
     static void addSubblock(std::string, APIAction *action);
     static APIAction* lookup(std::string);
     static TerminateAPIHandler* terminateAPIHandler;
     static SideEffectAPIHandler *sideEffectAPIHandler;
     static  std::map<std::string, std::set<APIAction*>> apiModel;
     static IgnoreAPIHandler *ignoreAPIHandler;
     static AllocAPIHandler *allocAPIHandler;
     static FreeAPIHandler *freeAPIHandler;
     static RefCountAPIHandler *refcountAPIHandler;
     static CallbackAPIHandler *callbackAPIHandler;
     static void readProgModelSpec(const char *name);
     static bool handle(ExecutionState &state, std::vector<ExecutionState*> &branches, std::string fname, 
                               std::vector< ref<Expr> > &arguments, KInstruction *target, int tid=-1);
     APIHandler();
     virtual bool interpret(APIAction &action, ExecutionState &state, 
        std::vector< ref<Expr> > &arguments, KInstruction *target, bool &term,  int tid);
     virtual bool assignsRetValue(APIAction &action);
     bool setsArgs();
  };

  enum RetCond {ZERO, NOTZERO, LTZERO, LTEQZERO, GTZERO, GTEQZERO, TRUE};


  class APIAction {
    protected: 
       std::string  desc;
       // first element is the api function name
       std::vector<std::string> param;
       APIHandler *handler;
       RetCond returnCondition;
       bool terminated;
    public:
       APIAction();
       APIAction(std::string desc, std::vector<std::string> param, APIHandler *handler, RetCond retCond=TRUE);
       bool hasTerminated();
       void terminate();
       std::vector<std::string> getParams();
       RetCond getReturnCond();
       void setReturnCond(RetCond rt);
       virtual bool assignsRetValue();
       virtual bool setsArgs();
       void addChainAction(APIAction *action);
       virtual void execute(ExecutionState &state, std::vector< ref<Expr> > &arguments, 
             KInstruction *target, bool &term, int tid = -1);
       virtual void print();
  };


  class APIBlock : public APIAction {
    protected:
       std::vector<APIAction*> body;
    public:
       APIBlock();
       void addNextAction(APIAction *action, RetCond rc=TRUE);
       virtual bool assignsRetValue();
       virtual bool setsArgs();       
       virtual void execute(ExecutionState &state, std::vector< ref<Expr> > &arguments, 
             KInstruction *target, bool &term, int tid = -1);  
       virtual void print();
  };


  /*
  class MutexAPIHandler : public APIHandler {
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
  */ 

  class RefCountAPIHandler : public APIHandler {
    public:      
     RefCountAPIHandler();
     virtual bool interpret(APIAction &action,ExecutionState &state,  
          std::vector< ref<Expr> > &arguments, KInstruction *target, bool &term, int tid=-1);      
     virtual bool assignsRetValue();

  };
 
  /*
  
  class RegistrationAPIHandler : public APIHandler {
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

  class ResourceAllocReleaseAPIHandler : public APIHandler {
   public:
      ResourceAllocReleaseAPIHandler();
      bool interpret(APIAction &action,ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, bool &term, int tid=-1); 
  };

  class SetGetAPIHandler : public APIHandler {
   public:
     SetGetAPIHandler();
     bool interpret(APIAction &action,ExecutionState &state, std::string fname, std::vector< ref<Expr> > &arguments, KInstruction *target, bool &term, int tid=-1);
  };   
*/

   class AllocAPIHandler : public APIHandler {
    public:
      AllocAPIHandler();
      virtual bool interpret(APIAction &action, ExecutionState &state, std::vector< ref<Expr> > &arguments, 
                   KInstruction *target, bool &term, int tid=-1);     
      virtual bool assignsRetValue(APIAction &action);
  
   };

   class FreeAPIHandler : public APIHandler {
    public:
      FreeAPIHandler();
      virtual bool interpret(APIAction &action, ExecutionState &state, std::vector< ref<Expr> > &arguments, 
                   KInstruction *target, bool &term, int tid=-1);       
      virtual bool assignsRetValue(APIAction &action);
  
   };

   /*
   class ReadWriteAPIHandler : public APIHandler {
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
  */

   class IgnoreAPIHandler : public APIHandler {
    public:
     IgnoreAPIHandler();
     virtual bool interpret(APIAction &action, ExecutionState &state, std::vector< ref<Expr> > &arguments, 
                              KInstruction *target, bool &term, int tid=-1); 
     virtual bool assignsRetValue(APIAction &action);
   };

   
   class CallbackAPIHandler : public APIHandler {
    public:
     CallbackAPIHandler();
     virtual bool interpret(APIAction &action, ExecutionState &state, std::vector< ref<Expr> > &arguments, 
                             KInstruction *target, bool &term, int tid=-1);              
     virtual bool assignsRetValue(APIAction &action); 
   };
   

   class SideEffectAPIHandler : public APIHandler {
     public:
      SideEffectAPIHandler(); 
      virtual bool interpret(APIAction &action, ExecutionState &state, std::vector< ref<Expr> > &arguments, 
                                KInstruction *target, bool &term, int tid=-1);         
      virtual bool assignsRetValue(APIAction &action);
     
   };

   class TerminateAPIHandler : public APIHandler {
    public:
     TerminateAPIHandler();
     virtual bool interpret(APIAction &action, ExecutionState &state, std::vector< ref<Expr> > &arguments, 
                                KInstruction *target, bool &term, int tid=-1);         
     virtual bool assignsRetValue(APIAction &action);
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
