/* -*- mode: c++; c-basic-offset: 2; -*- */

//===-- main.cpp ------------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Config/Version.h"
#include "klee/ExecutionState.h"
#include "klee/Expr.h"
#include "klee/Internal/ADT/KTest.h"
#include "klee/Internal/ADT/TreeStream.h"
#include "klee/Internal/Support/Debug.h"
#include "klee/Internal/Support/ErrorHandling.h"
#include "klee/Internal/Support/FileHandling.h"
#include "klee/Internal/Support/ModuleUtil.h"
#include "klee/Internal/Support/PrintVersion.h"
#include "klee/Internal/System/Time.h"
#include "klee/Interpreter.h"
#include "klee/Statistics.h"
#include "klee/util/ExprPPrinter.h"
#include "../../lib/Core/SpecialFunctionHandler.h"
#include "../../lib/Core/AnalyzeConstraint.h"
#include "../../lib/Core/ResourceUsage.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/Errno.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/Signals.h"

#if LLVM_VERSION_CODE < LLVM_VERSION(3, 5)
#include "llvm/Support/system_error.h"
#endif

#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <cerrno>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <climits>


using namespace llvm;
using namespace klee;


/* begin SYSREL extension */

/* Grammar for the life-cycle config file 
sequential := sequential ';' sequential
sequential := '(' parallel ')'
parallel := identifier '|' parallel
parallel := identifier
sequential := identifier
*/
class RD;
int minInstCount = INT_MAX;
int maxInstCount = 0;
const Module * moduleHandle = NULL;
bool asyncMode = false;
std::vector<std::string> asyncFunc;
std::vector<std::string> enabledFunc;
std::string entryFunctionName;
std::string sidechannelentry;
bool lazyInit = false;
bool lazySpec = false;
int numLazyInst = 20;
std::vector<std::string> lazyInits;
std::set<std::string> lazyInitSingles;
std::map<std::string, int> lazyInitNumInstances;
std::map<std::string, std::vector<unsigned int> > lazyInitInitializers;
std::map<std::string, std::map<unsigned int, std::string> > lazyFuncPtrInitializers;
bool progModel = false;
APIHandler *apiHandler = NULL;
std::map<std::string, std::vector<std::string> > inferenceClue;
std::set<std::string> prefixRedact;
extern std::set<std::string> * highLoc, *lowLoc;
extern int maxForkMulRes;
extern int primArraySize;
bool cacheLineMode;
bool cacheBitmaskMode;
unsigned long cacheBitmask = 0xFFFFFFF;
unsigned int cacheLineBits = 6; // cache line size of 2^6 = 64
extern std::map<std::string, unsigned int> timingModels;
extern std::map<std::string, std::map<region, std::set<infoflowsource_t> > > infoFlowRules;
extern std::map<std::string, std::map<unsigned int, std::vector<unsigned int> > > externalFuncsWithSensitiveFlow;
// source code locations with timing side channels
extern std::set<std::pair<std::string, int>> locs;
// source code locations with cache side channels
extern std::set<std::pair<std::string, int>> cachelocs;
std::set<std::string> voidTypeCasts;
// Type hints
extern std::map<std::string, std::map<int, std::string> > funcArgTypeHints;
//  Functions that receive input from the environment
extern std::set<std::string> inputFuncs;
// Functions to arguments that receive input from the environment
extern std::map<std::string, std::set<unsigned int> > inputFuncArgs;
// Maps function arguments to its high security regions
extern std::map<std::string, std::set<int> > highFunctionArgs;
// Maps function arguments to its low security regions
extern std::map<std::string, std::set<int> > lowFunctionArgs;  
// Maps function arguments to its mixed security regions
extern std::map<std::string, std::set<int> > mixedFunctionArgs;  
// Maps function arguments to its high security regions
extern std::map<std::string, std::set<int> > highFunctionArgsRet;
// Maps function arguments to its low security regions
extern std::map<std::string, std::set<int> > lowFunctionArgsRet;  
// Maps function arguments to its mixed security regions
extern std::map<std::string, std::set<int> > mixedFunctionArgsRet;  
// Maps types to its high security regions
extern std::map<std::string, std::vector<region> > highTypeRegions;
// Maps types to its low security regions
extern std::map<std::string, std::vector<region> > lowTypeRegions;
// Maps symbolic regions to its high security regions
// Maps symbolic regions to its high security regions
// unique symbolic names eliminate the need to do the mapping per state
extern std::map<std::string, std::vector<region> > highSymRegions;
// Maps symbolic regions to its low security regions
// unique symbolic names eliminate the need to do the mapping per state
extern std::map<std::string, std::vector<region> > lowSymRegions;
// Stores the functions that when executed can be observed by the timing side channel attackers, e.g., ocalls
extern std::vector<std::string> * untrusted;
extern std::set<std::string> highSecurityLeaksOnStack;
extern std::set<std::string> codeLocHighSecurityLeaksOnStack;
extern std::set<std::string> stackLeakToBeChecked;
extern std::set<std::string> securitySensitiveBranches;
extern std::map<std::string, int> inputFuncsSuccRetValue;
extern bool infoFlowSummaryMode;
extern std::string infoFlowSummarizedFuncName;
extern bool symbolizeInlineAssembly;
/*
RegistrationAPIHandler  *regAPIHandler = NULL;
ResourceAllocReleaseAPIHandler *resADAPIHandler = NULL;
MutexAPIHandler*  mutexAPIHandler = NULL;
RefCountAPIHandler *refcountAPIHandler = NULL;
SetGetAPIHandler *setgetAPIHandler = NULL;
AllocAPIHandler *allocAPIHandler = NULL;
ReadWriteAPIHandler *readWriteAPIHandler = NULL;
IgnoreAPIHandler *ignoreAPIHandler = NULL;
CallbackAPIHandler *callbackAPIHandler = NULL;
FreeAPIHandler *freeAPIHandler = NULL;
SideEffectAPIHandler *sideEffectAPIHandler = NULL;
*/

void addInfoFlowRule(std::string fname, region fr,  std::set<infoflowsource_t> ifss) {
 std::map<region, std::set<infoflowsource_t> > fm;
 if (infoFlowRules.find(fname) != infoFlowRules.end())
    fm = infoFlowRules[fname];
 if (fm.find(fr) != fm.end())
    assert(false && "info flow region already defined !\n"); 
 fm[fr] = ifss;
 infoFlowRules[fname] = fm; 
}


void writeSecuritySenstiveBranches(const char *fname) {
  std::fstream rc(fname, std::fstream::out);
  if (rc.is_open()) {
     for(auto s: securitySensitiveBranches) {
        rc << s << "\n";
     }
     rc.close();
  }
}


void writeCacheSideChannels(const char *fname) {
  std::fstream rc(fname, std::fstream::out);
  if (rc.is_open()) {
     for(auto s: cachelocs) {
        rc << s.first << ":" << s.second << "\n\n";
     }
     rc.close();
  }
}


void writeSensitiveLeaksOnStack(const char *fname) {
  std::fstream rc(fname, std::fstream::out);
  if (rc.is_open()) {
     for(auto s: highSecurityLeaksOnStack) {
        rc << s << "\n";
     }
     rc.close();
  }
}

void writeSensitiveFlows(const char *fname) {
  std::fstream rc(fname, std::fstream::out);
  if (rc.is_open()) {
     for(auto sf: externalFuncsWithSensitiveFlow) {
       for(auto fm : sf.second) {
          rc << sf.first;
          for(auto arg : fm.second)
             rc << "," << arg;
          rc << "\n";
       }
     }  
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

void readTimingModels(const char *fname) {
  std::fstream rc(fname, std::fstream::in);
  if (rc.is_open()) {
     std::string line,fname,value;
     while(std::getline(rc,line)) {
       std::istringstream iss(line);
       getline(iss,  fname, ',');
       fname = ltrim(rtrim(fname));
       getline(iss,  value, ',');
       value = ltrim(rtrim(value));
       unsigned int numinstrs = std::stoi(value);
       if (timingModels.find(fname) == timingModels.end())
          timingModels[fname] = numinstrs;
       else if (timingModels[fname] != numinstrs) {
          llvm::errs() << fname << " conflicting timing values: " << timingModels[fname] << " and " << numinstrs << "\n"; 
          assert(false && "Inconsistent timing model!\n");
       }
     }
     rc.close();
  }
} 

void readInfoFlowModels(const char *fname) {
  std::fstream rc(fname, std::fstream::in);
   if (rc.is_open()) {
      std::string line, fname, token;
      while(std::getline(rc,line)) {
        if (line.find("/") == std::string::npos) {
           llvm::errs() << "processing line: " << line << "\n";
           std::set<infoflowsource_t> ifss;
           region fr;
           std::istringstream iss(line);
           getline(iss,  fname, ',');
           fname = ltrim(rtrim(fname));
           getline(iss,  token, ',');
           token = ltrim(rtrim(token));
           fr.offset = std::stoi(token);
           getline(iss,  token, ',');
           token = ltrim(rtrim(token)); 
           fr.size = std::stoi(token);
           llvm::errs() << "target range: " << fr.offset << "," << fr.size << "\n";
           getline(iss,  token, ',');
           token = ltrim(rtrim(token));
           if (token == "NONE") {
              addInfoFlowRule(fname, fr, ifss);    
           }
           else {
              bool singleSource = true;
              bool dist = false;
              //llvm::errs() << "info flow source token " << token << "\n";
              if (token.find("UNION(") != std::string::npos) {
                 token = token.substr(token.find("UNION(") + 5);
                 singleSource = false;
              }
              else if (token.find("DIST(") != std::string::npos) {
                 dist = true;
              }
              bool done = false;
              while (!done) {
                   if (token.find("arg") != std::string::npos) {
                      infoflowsource_t ifs;
                      ifs.argno = std::stoi(token.substr(token.find("arg") + 3));
                      region r;
                      getline(iss,  token, ',');
                      token = ltrim(rtrim(token));
                      r.offset = std::stoi(token);
                      getline(iss,  token, ',');
                      token = ltrim(rtrim(token));
                      if (token.find(")") != std::string::npos) {
                         token = token.substr(0,token.length()-1); 
                         done = true;
                      }
                      r.size = std::stoi(token);
                      llvm::errs() << "source range: " << r.offset << "," << r.size << "\n";
                      ifs.ifregion = r;
                      ifss.insert(ifs);
                      if (!singleSource) {
                         getline(iss,  token, ',');
                         //llvm::errs() << "next of arg group: " << token << "\n"; 
                      }
                      else {
                         if (dist) {
                            unsigned int  granularity = ifs.ifregion.size; 
                            for(unsigned int k=0; k<fr.size/granularity; k++) {
                               region n;
                               n.offset = fr.offset + k*granularity;
                               n.size = ifs.ifregion.size;
                               llvm::errs() << "distributing to target range: offset="  << n.offset << "," << " size=" << n.size << "\n";
                               addInfoFlowRule(fname, n, ifss);
                            }
                         }
                         break;
                      } 
                   }
                   else assert(false && "was expecting an arg for information flow source!\n");
              }
              if (!dist)
                 addInfoFlowRule(fname, fr, ifss); 
           }
        }
     }
  }
}

void readInferenceClue(const char *fname) {
   std::fstream rc(fname, std::fstream::in);
   if (rc.is_open()) {
      std::string line;
      while(std::getline(rc,line)) {
          std::string  embeddedType, embeddingType;
          std::istringstream iss(line);
          getline(iss, embeddedType, ',');
          getline(iss, embeddingType);
          std::vector<std::string> inf;
          if (inferenceClue.find(embeddedType) != inferenceClue.end())
             inf = inferenceClue[embeddedType];
          inf.push_back(embeddingType);
          inferenceClue[embeddedType] = inf;
      } 
   }
   else llvm::errs() << "Couldn't open file " << fname << "\n";
}

void readVoidTypeCasts(const char *fname) {
    std::fstream rc(fname, std::fstream::in);
    if (rc.is_open()) {
      std::string line;
      while(std::getline(rc,line)) {
         voidTypeCasts.insert(ltrim(rtrim(line)));
      }
    }
    else llvm::errs() << "Couldn't open file " << fname << "\n";
}

/* end SYSREL extension */

namespace {
  cl::opt<std::string>
  InputFile(cl::desc("<input bytecode>"), cl::Positional, cl::init("-"));

  cl::opt<std::string>
  EntryPoint("entry-point",
               cl::desc("Consider the function with the given name as the entrypoint"),
               cl::init("main"));

  /* SYSREL extension */
  cl::opt<int>
  PrimitiveArraySize("prim-array-size", cl::desc("Lazy initialization size for arrays of primitive size (default 32)"));

  cl::opt<int>
  MaxForkMulResolution("max-fork-mul-res", cl::desc("Max number of forked states during memory op with multiple resolution (default is 10)"));

  cl::list<std::string>
  AsyncEntryPoints("async-func",
                   cl::desc("async functions"), cl::CommaSeparated);

  cl::opt<std::string>
  LifeCycleConf("life-cycle-conf", cl::desc("Life-cycle configuration file"));

  cl::opt<bool>
  SymbolizeInlineAssembly("sym-inline-asm", cl::desc("Symbolize inline assembly call return value (default false)"), cl::init(false));

  cl::opt<bool>
  LazyInit("lazy-init", cl::desc("Lazy initialization of input data structures (default false)"), cl::init(false));

  cl::opt<bool>
  InfoFlowSummaryMode("infoflow-summary", cl::desc("Information flow summary mode (default false)"), 
                                          cl::init(false));
  cl::opt<std::string>
  InfoFlowSummaryEntry("infoflow-summary-entry", cl::desc("The name of the function to be summarized"));  

  cl::opt<unsigned>
  LazyInitInst("lazy-init-ins", cl::desc("Number of instances for lazy initialization instances (default is 20)")); 

  cl::opt<std::string>
  LazySingle("lazy-single-ins",cl::desc("Lazily initialized types that should have a single instance"));

  cl::opt<std::string>
  LazyInitializer("lazy-set-at-init", cl::desc("The file that contains lazy init types and their byte offsets that need to be initialized to NULL or to functions for function pointers\n"));

  cl::opt<std::string>
  FrameworkDts("framework-dts", cl::desc("Framework data structure types file \
                                            (turns off automatic inference of lazy types)"));     

  cl::opt<std::string>
  ProgModelSpec("prog-model-spec", cl::desc("Programming Model Spec File, enables related checks and enforcements\n"));

  cl::opt<std::string>
  InferenceClue("infer-clue-spec", cl::desc("Inference Clue Spec File, embedded type and embedding type pairs\n"));

  cl::opt<std::string>
  VoidTypeCasts("void-type-casts", cl::desc("Void Type Cast Spec File\n"));

  cl::opt<std::string>
  FuncArgTypeHints("func-arg-type-hints", cl::desc("Type hints for function arguments of void pointer types\n"));

  cl::opt<std::string>
  SideChannelEntryPoint("side-channel-entry", cl::desc("function name from which time side channel analysis will be initiated \ 
                                            \n \t (default is the symbolic execution entry point) \n"));

  cl::opt<std::string>
  InputFuncs("input-funcs",  cl::desc("Functions that receive inputs from the environment\n"));

  cl::opt<std::string>
  SensitiveInputs("sensitive-inputs",  cl::desc("Functions arguments that are categorized as (high/low) security sensitive\n"));

  cl::opt<std::string>
  SensitiveTypeRegions("sensitive-type-region", cl::desc("Specifies regions (offset,size) of types categorized as (high/low) security sensitive\n"));

  cl::opt<std::string>
  TimingObservationPoints("timing-obs-point", cl::desc("Specifies the functions that mark the points where attackers can take timing measurements, e.g., ocalls for SGX\n"));

  cl::opt<std::string>
  SideChannelMethod("side-channel-method", cl::desc("side channel analysis method. Options are \ 
                                             \n \t (use naive for using all leaf nodes (may not terminate)\n \
                                             \n \t (use decompose for focusing on relevant candidates only\n"));
  cl::opt<unsigned>
  CacheLineBits("cache-line-bits", cl::desc("cache line bits (log of cache line size) (default 6) \n"));

  cl::opt<unsigned>
  CacheBitMask("cache-bit-mask", cl::desc("bit mask to check for secret dependent cache access\n"));    

  cl::opt<std::string>
  InfoFlowModels("info-flow-models", cl::desc("Name of the file that specifies how argument regions may affect security sensitivity of the return value region of API functions\n"));

  cl::opt<std::string>
  TimingModels("timing-models", cl::desc("Name of the file that specifies instruction count of API functions\n"));

  cl::opt<std::string>
  CheckStackLeak("check-stack-leak", cl::desc("Name of the file that specifies the functions for which stack leak check will be performed\n"));
 
  cl::opt<std::string>
  PrefixRedact("prefix-redact", cl::desc("the file that contains the prefixes to be removed \ 
                                                    to map an undefined function to its model function\n"));



                                             
  /* SYSREL extension */


  cl::opt<std::string>
  RunInDir("run-in", cl::desc("Change to the given directory prior to executing"));

  cl::opt<std::string>
  Environ("environ", cl::desc("Parse environ from given file (in \"env\" format)"));

  cl::list<std::string>
  InputArgv(cl::ConsumeAfter,
            cl::desc("<program arguments>..."));

  cl::opt<bool>
  NoOutput("no-output",
           cl::desc("Don't generate test files"));

  cl::opt<bool>
  WarnAllExternals("warn-all-externals",
                   cl::desc("Give initial warning for all externals."));

  cl::opt<bool>
  WriteCVCs("write-cvcs",
            cl::desc("Write .cvc files for each test case"));

  cl::opt<bool>
  WriteKQueries("write-kqueries",
            cl::desc("Write .kquery files for each test case"));

  cl::opt<bool>
  WriteSMT2s("write-smt2s",
            cl::desc("Write .smt2 (SMT-LIBv2) files for each test case"));

  cl::opt<bool>
  WriteCov("write-cov",
           cl::desc("Write coverage information for each test case"));

  cl::opt<bool>
  WriteTestInfo("write-test-info",
                cl::desc("Write additional test case information"));

  cl::opt<bool>
  WritePaths("write-paths",
                cl::desc("Write .path files for each test case"));

  cl::opt<bool>
  WriteSymPaths("write-sym-paths",
                cl::desc("Write .sym.path files for each test case"));

  cl::opt<bool>
  OptExitOnError("exit-on-error",
              cl::desc("Exit if errors occur"));

  /* SYSREL extension */
  cl::opt<bool>
  AsyncMode("async",
            cl::desc("Handle async functions"), cl::init(false));
  /* SYSREL extension */


  enum LibcType {
    NoLibc, KleeLibc, UcLibc
  };

  cl::opt<LibcType>
  Libc("libc",
       cl::desc("Choose libc version (none by default)."),
       cl::values(clEnumValN(NoLibc, "none", "Don't link in a libc"),
                  clEnumValN(KleeLibc, "klee", "Link in klee libc"),
		  clEnumValN(UcLibc, "uclibc", "Link in uclibc (adapted for klee)")
		  KLEE_LLVM_CL_VAL_END),
       cl::init(NoLibc));


  cl::opt<bool>
  WithPOSIXRuntime("posix-runtime",
		cl::desc("Link with POSIX runtime.  Options that can be passed as arguments to the programs are: --sym-arg <max-len>  --sym-args <min-argvs> <max-argvs> <max-len> + file model options"),
		cl::init(false));

  cl::opt<bool>
  OptimizeModule("optimize",
                 cl::desc("Optimize before execution"),
		 cl::init(false));

  cl::opt<bool>
  CheckDivZero("check-div-zero",
               cl::desc("Inject checks for division-by-zero"),
               cl::init(true));

  cl::opt<bool>
  CheckOvershift("check-overshift",
               cl::desc("Inject checks for overshift"),
               cl::init(true));

  cl::opt<std::string>
  OutputDir("output-dir",
            cl::desc("Directory to write results in (defaults to klee-out-N)"),
            cl::init(""));

  cl::opt<bool>
  ReplayKeepSymbolic("replay-keep-symbolic",
                     cl::desc("Replay the test cases only by asserting "
                              "the bytes, not necessarily making them concrete."));

  cl::list<std::string>
      ReplayKTestFile("replay-ktest-file",
                      cl::desc("Specify a ktest file to use for replay"),
                      cl::value_desc("ktest file"));

  cl::list<std::string>
      ReplayKTestDir("replay-ktest-dir",
                   cl::desc("Specify a directory to replay ktest files from"),
                   cl::value_desc("output directory"));

  cl::opt<std::string>
  ReplayPathFile("replay-path",
                 cl::desc("Specify a path file to replay"),
                 cl::value_desc("path file"));

  cl::list<std::string>
  SeedOutFile("seed-out");

  cl::list<std::string>
  SeedOutDir("seed-out-dir");

  cl::list<std::string>
  LinkLibraries("link-llvm-lib",
		cl::desc("Link the given libraries before execution"),
		cl::value_desc("library file"));

  cl::opt<unsigned>
  MakeConcreteSymbolic("make-concrete-symbolic",
                       cl::desc("Probabilistic rate at which to make concrete reads symbolic, "
				"i.e. approximately 1 in n concrete reads will be made symbolic (0=off, 1=all).  "
				"Used for testing."),
                       cl::init(0));

  cl::opt<unsigned>
  StopAfterNTests("stop-after-n-tests",
	     cl::desc("Stop execution after generating the given number of tests.  Extra tests corresponding to partially explored paths will also be dumped."),
	     cl::init(0));

  cl::opt<bool>
  Watchdog("watchdog",
           cl::desc("Use a watchdog process to enforce --max-time."),
           cl::init(0));
}

extern cl::opt<double> MaxTime;

/***/

class KleeHandler : public InterpreterHandler {
private:
  Interpreter *m_interpreter;
  TreeStreamWriter *m_pathWriter, *m_symPathWriter;
  llvm::raw_ostream *m_infoFile;

  SmallString<128> m_outputDirectory;

  unsigned m_numTotalTests;     // Number of tests received from the interpreter
  unsigned m_numGeneratedTests; // Number of tests successfully generated
  unsigned m_pathsExplored; // number of paths explored so far

  // used for writing .ktest files
  int m_argc;
  char **m_argv;

public:
  KleeHandler(int argc, char **argv);
  ~KleeHandler();

  llvm::raw_ostream &getInfoStream() const { return *m_infoFile; }
  /// Returns the number of test cases successfully generated so far
  unsigned getNumTestCases() { return m_numGeneratedTests; }
  unsigned getNumPathsExplored() { return m_pathsExplored; }
  void incPathsExplored() { m_pathsExplored++; }

  void setInterpreter(Interpreter *i);

  void processTestCase(const ExecutionState  &state,
                       const char *errorMessage,
                       const char *errorSuffix);

  std::string getOutputFilename(const std::string &filename);
  llvm::raw_fd_ostream *openOutputFile(const std::string &filename);
  std::string getTestFilename(const std::string &suffix, unsigned id);
  llvm::raw_fd_ostream *openTestFile(const std::string &suffix, unsigned id);

  // load a .path file
  static void loadPathFile(std::string name,
                           std::vector<bool> &buffer);

  static void getKTestFilesInDir(std::string directoryPath,
                                 std::vector<std::string> &results);

  static std::string getRunTimeLibraryPath(const char *argv0);
};

KleeHandler::KleeHandler(int argc, char **argv)
    : m_interpreter(0), m_pathWriter(0), m_symPathWriter(0), m_infoFile(0),
      m_outputDirectory(), m_numTotalTests(0), m_numGeneratedTests(0),
      m_pathsExplored(0), m_argc(argc), m_argv(argv) {

  // create output directory (OutputDir or "klee-out-<i>")
  bool dir_given = OutputDir != "";
  SmallString<128> directory(dir_given ? OutputDir : InputFile);

  if (!dir_given) sys::path::remove_filename(directory);
#if LLVM_VERSION_CODE < LLVM_VERSION(3, 5)
  error_code ec;
  if ((ec = sys::fs::make_absolute(directory)) != errc::success) {
#else
  if (auto ec = sys::fs::make_absolute(directory)) {
#endif
    klee_error("unable to determine absolute path: %s", ec.message().c_str());
  }

  if (dir_given) {
    // OutputDir
    if (mkdir(directory.c_str(), 0775) < 0)
      klee_error("cannot create \"%s\": %s", directory.c_str(), strerror(errno));

    m_outputDirectory = directory;
  } else {
    // "klee-out-<i>"
    int i = 0;
    for (; i <= INT_MAX; ++i) {
      SmallString<128> d(directory);
      llvm::sys::path::append(d, "klee-out-");
      raw_svector_ostream ds(d);
      ds << i;
// SmallString is always up-to-date, no need to flush. See Support/raw_ostream.h
#if LLVM_VERSION_CODE < LLVM_VERSION(3, 8)
      ds.flush();
#endif

      // create directory and try to link klee-last
      if (mkdir(d.c_str(), 0775) == 0) {
        m_outputDirectory = d;

        SmallString<128> klee_last(directory);
        llvm::sys::path::append(klee_last, "klee-last");

        if (((unlink(klee_last.c_str()) < 0) && (errno != ENOENT)) ||
            symlink(m_outputDirectory.c_str(), klee_last.c_str()) < 0) {

          klee_warning("cannot create klee-last symlink: %s", strerror(errno));
        }

        break;
      }

      // otherwise try again or exit on error
      if (errno != EEXIST)
        klee_error("cannot create \"%s\": %s", m_outputDirectory.c_str(), strerror(errno));
    }
    if (i == INT_MAX && m_outputDirectory.str().equals(""))
        klee_error("cannot create output directory: index out of range");
  }

  klee_message("output directory is \"%s\"", m_outputDirectory.c_str());

  // open warnings.txt
  std::string file_path = getOutputFilename("warnings.txt");
  if ((klee_warning_file = fopen(file_path.c_str(), "w")) == NULL)
    klee_error("cannot open file \"%s\": %s", file_path.c_str(), strerror(errno));

  // open messages.txt
  file_path = getOutputFilename("messages.txt");
  if ((klee_message_file = fopen(file_path.c_str(), "w")) == NULL)
    klee_error("cannot open file \"%s\": %s", file_path.c_str(), strerror(errno));

  // open info
  m_infoFile = openOutputFile("info");
}

KleeHandler::~KleeHandler() {
  delete m_pathWriter;
  delete m_symPathWriter;
  fclose(klee_warning_file);
  fclose(klee_message_file);
  delete m_infoFile;
}

void KleeHandler::setInterpreter(Interpreter *i) {
  m_interpreter = i;

  if (WritePaths) {
    m_pathWriter = new TreeStreamWriter(getOutputFilename("paths.ts"));
    assert(m_pathWriter->good());
    m_interpreter->setPathWriter(m_pathWriter);
  }

  if (WriteSymPaths) {
    m_symPathWriter = new TreeStreamWriter(getOutputFilename("symPaths.ts"));
    assert(m_symPathWriter->good());
    m_interpreter->setSymbolicPathWriter(m_symPathWriter);
  }
}

std::string KleeHandler::getOutputFilename(const std::string &filename) {
  SmallString<128> path = m_outputDirectory;
  sys::path::append(path,filename);
  return path.str();
}

llvm::raw_fd_ostream *KleeHandler::openOutputFile(const std::string &filename) {
  llvm::raw_fd_ostream *f;
  std::string Error;
  std::string path = getOutputFilename(filename);
  f = klee_open_output_file(path, Error);
  if (!Error.empty()) {
    klee_warning("error opening file \"%s\".  KLEE may have run out of file "
                 "descriptors: try to increase the maximum number of open file "
                 "descriptors by using ulimit (%s).",
                 path.c_str(), Error.c_str());
    return NULL;
  }
  return f;
}

std::string KleeHandler::getTestFilename(const std::string &suffix, unsigned id) {
  std::stringstream filename;
  filename << "test" << std::setfill('0') << std::setw(6) << id << '.' << suffix;
  return filename.str();
}

llvm::raw_fd_ostream *KleeHandler::openTestFile(const std::string &suffix,
                                                unsigned id) {
  return openOutputFile(getTestFilename(suffix, id));
}


/* Outputs all files (.ktest, .kquery, .cov etc.) describing a test case */
void KleeHandler::processTestCase(const ExecutionState &state,
                                  const char *errorMessage,
                                  const char *errorSuffix) {
  if (!NoOutput) {
    std::vector< std::pair<std::string, std::vector<unsigned char> > > out;
    bool success = m_interpreter->getSymbolicSolution(state, out);

    if (!success)
      klee_warning("unable to get symbolic solution, losing test case");

    double start_time = util::getWallTime();

    unsigned id = ++m_numTotalTests;

    /* SYSREL extxension */
    #ifdef VB
    if (!success) {
       llvm::errs() << "unable to get symbolic solution, losing test case\n";
       ExprPPrinter::printConstraints(llvm::errs(), state.constraints);
    }
    #endif
    /* SYSREL extxension */


    if (success) {
      /* SYSREL extxension */
      #ifdef VB
      llvm::errs() << "\nPath constraint:\n";
      ExprPPrinter::printConstraints(llvm::errs(), state.constraints);
      #endif
      /* SYSREL extxension */

      KTest b;
      b.numArgs = m_argc;
      b.args = m_argv;
      b.symArgvs = 0;
      b.symArgvLen = 0;
      b.numObjects = out.size();
      b.objects = new KTestObject[b.numObjects];
      assert(b.objects);
      for (unsigned i=0; i<b.numObjects; i++) {
        KTestObject *o = &b.objects[i];
        o->name = const_cast<char*>(out[i].first.c_str());
        o->numBytes = out[i].second.size();
        o->bytes = new unsigned char[o->numBytes];
        assert(o->bytes);
        std::copy(out[i].second.begin(), out[i].second.end(), o->bytes);
      }

      if (!kTest_toFile(&b, getOutputFilename(getTestFilename("ktest", id)).c_str())) {
        klee_warning("unable to write output test case, losing it");
        /* SYSREL extxension */
        llvm::errs() << "unable to write output test case, losing it";
        /* SYSREL extxension */
      } else {
        ++m_numGeneratedTests;
      }

      for (unsigned i=0; i<b.numObjects; i++)
        delete[] b.objects[i].bytes;
      delete[] b.objects;
    }

    if (errorMessage) {
      llvm::raw_ostream *f = openTestFile(errorSuffix, id);
      *f << errorMessage;
      delete f;
    }

    if (m_pathWriter) {
      std::vector<unsigned char> concreteBranches;
      m_pathWriter->readStream(m_interpreter->getPathStreamID(state),
                               concreteBranches);
      llvm::raw_fd_ostream *f = openTestFile("path", id);
      for (std::vector<unsigned char>::iterator I = concreteBranches.begin(),
                                                E = concreteBranches.end();
           I != E; ++I) {
        *f << *I << "\n";
      }
      delete f;
    }

    if (errorMessage || WriteKQueries) {
      std::string constraints;
      m_interpreter->getConstraintLog(state, constraints,Interpreter::KQUERY);
      llvm::raw_ostream *f = openTestFile("kquery", id);
      *f << constraints;
      delete f;
    }

    if (WriteCVCs) {
      // FIXME: If using Z3 as the core solver the emitted file is actually
      // SMT-LIBv2 not CVC which is a bit confusing
      std::string constraints;
      m_interpreter->getConstraintLog(state, constraints, Interpreter::STP);
      llvm::raw_ostream *f = openTestFile("cvc", id);
      *f << constraints;
      delete f;
    }

    if(WriteSMT2s) {
      std::string constraints;
        m_interpreter->getConstraintLog(state, constraints, Interpreter::SMTLIB2);
        llvm::raw_ostream *f = openTestFile("smt2", id);
        *f << constraints;
        delete f;
    }

    if (m_symPathWriter) {
      std::vector<unsigned char> symbolicBranches;
      m_symPathWriter->readStream(m_interpreter->getSymbolicPathStreamID(state),
                                  symbolicBranches);
      llvm::raw_fd_ostream *f = openTestFile("sym.path", id);
      for (std::vector<unsigned char>::iterator I = symbolicBranches.begin(), E = symbolicBranches.end(); I!=E; ++I) {
        *f << *I << "\n";
      }
      delete f;
    }

    if (WriteCov) {
      std::map<const std::string*, std::set<unsigned> > cov;
      m_interpreter->getCoveredLines(state, cov);
      llvm::raw_ostream *f = openTestFile("cov", id);
      for (std::map<const std::string*, std::set<unsigned> >::iterator
             it = cov.begin(), ie = cov.end();
           it != ie; ++it) {
        for (std::set<unsigned>::iterator
               it2 = it->second.begin(), ie = it->second.end();
             it2 != ie; ++it2)
          *f << *it->first << ":" << *it2 << "\n";
      }
      delete f;
    }

    if (m_numGeneratedTests == StopAfterNTests)
      m_interpreter->setHaltExecution(true);

    if (WriteTestInfo) {
      double elapsed_time = util::getWallTime() - start_time;
      llvm::raw_ostream *f = openTestFile("info", id);
      *f << "Time to generate test case: "
         << elapsed_time << "s\n";
      delete f;
    }
  }
  
  if (errorMessage && OptExitOnError) {
    m_interpreter->prepareForEarlyExit();
    klee_error("EXITING ON ERROR:\n%s\n", errorMessage);
  }
}

  // load a .path file
void KleeHandler::loadPathFile(std::string name,
                                     std::vector<bool> &buffer) {
  std::ifstream f(name.c_str(), std::ios::in | std::ios::binary);

  if (!f.good())
    assert(0 && "unable to open path file");

  while (f.good()) {
    unsigned value;
    f >> value;
    buffer.push_back(!!value);
    f.get();
  }
}

void KleeHandler::getKTestFilesInDir(std::string directoryPath,
                                     std::vector<std::string> &results) {
#if LLVM_VERSION_CODE < LLVM_VERSION(3, 5)
  error_code ec;
#else
  std::error_code ec;
#endif
  for (llvm::sys::fs::directory_iterator i(directoryPath, ec), e; i != e && !ec;
       i.increment(ec)) {
    std::string f = (*i).path();
    if (f.substr(f.size()-6,f.size()) == ".ktest") {
          results.push_back(f);
    }
  }

  if (ec) {
    llvm::errs() << "ERROR: unable to read output directory: " << directoryPath
                 << ": " << ec.message() << "\n";
    exit(1);
  }
}

std::string KleeHandler::getRunTimeLibraryPath(const char *argv0) {
  // allow specifying the path to the runtime library
  const char *env = getenv("KLEE_RUNTIME_LIBRARY_PATH");
  if (env)
    return std::string(env);

  // Take any function from the execution binary but not main (as not allowed by
  // C++ standard)
  void *MainExecAddr = (void *)(intptr_t)getRunTimeLibraryPath;
  SmallString<128> toolRoot(
      llvm::sys::fs::getMainExecutable(argv0, MainExecAddr)
      );

  // Strip off executable so we have a directory path
  llvm::sys::path::remove_filename(toolRoot);

  SmallString<128> libDir;

  if (strlen( KLEE_INSTALL_BIN_DIR ) != 0 &&
      strlen( KLEE_INSTALL_RUNTIME_DIR ) != 0 &&
      toolRoot.str().endswith( KLEE_INSTALL_BIN_DIR ))
  {
    KLEE_DEBUG_WITH_TYPE("klee_runtime", llvm::dbgs() <<
                         "Using installed KLEE library runtime: ");
    libDir = toolRoot.str().substr(0,
               toolRoot.str().size() - strlen( KLEE_INSTALL_BIN_DIR ));
    llvm::sys::path::append(libDir, KLEE_INSTALL_RUNTIME_DIR);
  }
  else
  {
    KLEE_DEBUG_WITH_TYPE("klee_runtime", llvm::dbgs() <<
                         "Using build directory KLEE library runtime :");
    libDir = KLEE_DIR;
    llvm::sys::path::append(libDir,RUNTIME_CONFIGURATION);
    llvm::sys::path::append(libDir,"lib");
  }

  KLEE_DEBUG_WITH_TYPE("klee_runtime", llvm::dbgs() <<
                       libDir.c_str() << "\n");
  return libDir.str();
}

//===----------------------------------------------------------------------===//
// main Driver function
//
static std::string strip(std::string &in) {
  unsigned len = in.size();
  unsigned lead = 0, trail = len;
  while (lead<len && isspace(in[lead]))
    ++lead;
  while (trail>lead && isspace(in[trail-1]))
    --trail;
  return in.substr(lead, trail-lead);
}

static void parseArguments(int argc, char **argv) {
  cl::SetVersionPrinter(klee::printVersion);
  // This version always reads response files
  cl::ParseCommandLineOptions(argc, argv, " klee\n");
}

static int initEnv(Module *mainModule) {

  /*
    nArgcP = alloc oldArgc->getType()
    nArgvV = alloc oldArgv->getType()
    store oldArgc nArgcP
    store oldArgv nArgvP
    klee_init_environment(nArgcP, nArgvP)
    nArgc = load nArgcP
    nArgv = load nArgvP
    oldArgc->replaceAllUsesWith(nArgc)
    oldArgv->replaceAllUsesWith(nArgv)
  */

  Function *mainFn = mainModule->getFunction(EntryPoint);
  entryFunctionName = mainFn->getName();
  sidechannelentry = (SideChannelEntryPoint.c_str()) ? SideChannelEntryPoint : entryFunctionName ;
  if (!mainFn) {
    klee_error("'%s' function not found in module.", EntryPoint.c_str());
  }

  if (mainFn->arg_size() < 2) {
    klee_error("Cannot handle ""--posix-runtime"" when main() has less than two arguments.\n");
  }

  Instruction *firstInst = &*(mainFn->begin()->begin());

  Value *oldArgc = &*(mainFn->arg_begin());
  Value *oldArgv = &*(++mainFn->arg_begin());

  AllocaInst* argcPtr =
    new AllocaInst(oldArgc->getType(), "argcPtr", firstInst);
  AllocaInst* argvPtr =
    new AllocaInst(oldArgv->getType(), "argvPtr", firstInst);

  /* Insert void klee_init_env(int* argc, char*** argv) */
  std::vector<const Type*> params;
  LLVMContext &ctx = mainModule->getContext();
  params.push_back(Type::getInt32Ty(ctx));
  params.push_back(Type::getInt32Ty(ctx));
  Function* initEnvFn =
    cast<Function>(mainModule->getOrInsertFunction("klee_init_env",
                                                   Type::getVoidTy(ctx),
                                                   argcPtr->getType(),
                                                   argvPtr->getType(),
                                                   NULL));
  assert(initEnvFn);
  std::vector<Value*> args;
  args.push_back(argcPtr);
  args.push_back(argvPtr);
  Instruction* initEnvCall = CallInst::Create(initEnvFn, args,
					      "", firstInst);
  Value *argc = new LoadInst(argcPtr, "newArgc", firstInst);
  Value *argv = new LoadInst(argvPtr, "newArgv", firstInst);

  oldArgc->replaceAllUsesWith(argc);
  oldArgv->replaceAllUsesWith(argv);

  new StoreInst(oldArgc, argcPtr, initEnvCall);
  new StoreInst(oldArgv, argvPtr, initEnvCall);

  return 0;
}


// This is a terrible hack until we get some real modeling of the
// system. All we do is check the undefined symbols and warn about
// any "unrecognized" externals and about any obviously unsafe ones.

// Symbols we explicitly support
static const char *modelledExternals[] = {
  "_ZTVN10__cxxabiv117__class_type_infoE",
  "_ZTVN10__cxxabiv120__si_class_type_infoE",
  "_ZTVN10__cxxabiv121__vmi_class_type_infoE",

  // special functions
  "_assert",
  "__assert_fail",
  "__assert_rtn",
  "__errno_location",
  "__error",
  "calloc",
  "_exit",
  "exit",
  "free",
  "abort",
  "klee_abort",
  "klee_assume",
  "klee_check_memory_access",
  "klee_define_fixed_object",
  "klee_get_errno",
  "klee_get_valuef",
  "klee_get_valued",
  "klee_get_valuel",
  "klee_get_valuell",
  "klee_get_value_i32",
  "klee_get_value_i64",
  "klee_get_obj_size",
  "klee_is_symbolic",
  "klee_make_symbolic",
  "klee_mark_global",
  "klee_open_merge",
  "klee_close_merge",
  "klee_prefer_cex",
  "klee_posix_prefer_cex",
  "klee_print_expr",
  "klee_print_range",
  "klee_report_error",
  "klee_set_forking",
  "klee_silent_exit",
  "klee_warning",
  "klee_warning_once",
  "klee_alias_function",
  "klee_stack_trace",
  "llvm.dbg.declare",
  "llvm.dbg.value",
  "llvm.va_start",
  "llvm.va_end",
  "malloc",
  "realloc",
  "_ZdaPv",
  "_ZdlPv",
  "_Znaj",
  "_Znwj",
  "_Znam",
  "_Znwm",
  "__ubsan_handle_add_overflow",
  "__ubsan_handle_sub_overflow",
  "__ubsan_handle_mul_overflow",
  "__ubsan_handle_divrem_overflow",
};
// Symbols we aren't going to warn about
static const char *dontCareExternals[] = {
#if 0
  // stdio
  "fprintf",
  "fflush",
  "fopen",
  "fclose",
  "fputs_unlocked",
  "putchar_unlocked",
  "vfprintf",
  "fwrite",
  "puts",
  "printf",
  "stdin",
  "stdout",
  "stderr",
  "_stdio_term",
  "__errno_location",
  "fstat",
#endif

  // static information, pretty ok to return
  "getegid",
  "geteuid",
  "getgid",
  "getuid",
  "getpid",
  "gethostname",
  "getpgrp",
  "getppid",
  "getpagesize",
  "getpriority",
  "getgroups",
  "getdtablesize",
  "getrlimit",
  "getrlimit64",
  "getcwd",
  "getwd",
  "gettimeofday",
  "uname",

  // fp stuff we just don't worry about yet
  "frexp",
  "ldexp",
  "__isnan",
  "__signbit",
};
// Extra symbols we aren't going to warn about with klee-libc
static const char *dontCareKlee[] = {
  "__ctype_b_loc",
  "__ctype_get_mb_cur_max",

  // io system calls
  "open",
  "write",
  "read",
  "close",
};
// Extra symbols we aren't going to warn about with uclibc
static const char *dontCareUclibc[] = {
  "__dso_handle",

  // Don't warn about these since we explicitly commented them out of
  // uclibc.
  "printf",
  "vprintf"
};
// Symbols we consider unsafe
static const char *unsafeExternals[] = {
  "fork", // oh lord
  "exec", // heaven help us
  "error", // calls _exit
  "raise", // yeah
  "kill", // mmmhmmm
};
#define NELEMS(array) (sizeof(array)/sizeof(array[0]))
void externalsAndGlobalsCheck(const Module *m) {
  std::map<std::string, bool> externals;
  std::set<std::string> modelled(modelledExternals,
                                 modelledExternals+NELEMS(modelledExternals));
  std::set<std::string> dontCare(dontCareExternals,
                                 dontCareExternals+NELEMS(dontCareExternals));
  std::set<std::string> unsafe(unsafeExternals,
                               unsafeExternals+NELEMS(unsafeExternals));

  switch (Libc) {
  case KleeLibc:
    dontCare.insert(dontCareKlee, dontCareKlee+NELEMS(dontCareKlee));
    break;
  case UcLibc:
    dontCare.insert(dontCareUclibc,
                    dontCareUclibc+NELEMS(dontCareUclibc));
    break;
  case NoLibc: /* silence compiler warning */
    break;
  }

  if (WithPOSIXRuntime)
    dontCare.insert("syscall");

  for (Module::const_iterator fnIt = m->begin(), fn_ie = m->end();
       fnIt != fn_ie; ++fnIt) {
    if (fnIt->isDeclaration() && !fnIt->use_empty())
      externals.insert(std::make_pair(fnIt->getName(), false));
    for (Function::const_iterator bbIt = fnIt->begin(), bb_ie = fnIt->end();
         bbIt != bb_ie; ++bbIt) {
      for (BasicBlock::const_iterator it = bbIt->begin(), ie = bbIt->end();
           it != ie; ++it) {
        if (const CallInst *ci = dyn_cast<CallInst>(it)) {
          if (isa<InlineAsm>(ci->getCalledValue())) {
            klee_warning_once(&*fnIt,
                              "function \"%s\" has inline asm",
                              fnIt->getName().data());
          }
        }
      }
    }
  }
  for (Module::const_global_iterator
         it = m->global_begin(), ie = m->global_end();
       it != ie; ++it)
    if (it->isDeclaration() && !it->use_empty())
      externals.insert(std::make_pair(it->getName(), true));
  // and remove aliases (they define the symbol after global
  // initialization)
  for (Module::const_alias_iterator
         it = m->alias_begin(), ie = m->alias_end();
       it != ie; ++it) {
    std::map<std::string, bool>::iterator it2 =
      externals.find(it->getName());
    if (it2!=externals.end())
      externals.erase(it2);
  }

  std::map<std::string, bool> foundUnsafe;
  for (std::map<std::string, bool>::iterator
         it = externals.begin(), ie = externals.end();
       it != ie; ++it) {
    const std::string &ext = it->first;
    if (!modelled.count(ext) && (WarnAllExternals ||
                                 !dontCare.count(ext))) {
      if (unsafe.count(ext)) {
        foundUnsafe.insert(*it);
      } else {
        klee_warning("undefined reference to %s: %s",
                     it->second ? "variable" : "function",
                     ext.c_str());
      }
    }
  }

  for (std::map<std::string, bool>::iterator
         it = foundUnsafe.begin(), ie = foundUnsafe.end();
       it != ie; ++it) {
    const std::string &ext = it->first;
    klee_warning("undefined reference to %s: %s (UNSAFE)!",
                 it->second ? "variable" : "function",
                 ext.c_str());
  }
}


/* SYSREL static */ Interpreter *theInterpreter = 0;


void readCheckStackLeak(const char *name) {
  std::fstream cf(name, std::fstream::in);
  if (cf.is_open()) {
     std::string  line;
     while(std::getline(cf,line)) {
         stackLeakToBeChecked.insert(ltrim(rtrim(line)));
     }
  }
  cf.close();
}

void readTimingObservationPoints(const char *name) {
  std::fstream cf(name, std::fstream::in);
  if (cf.is_open()) {
     std::string  line;
     while(std::getline(cf,line)) {
        untrusted->push_back(ltrim(rtrim(line)));
     }
  }
  cf.close();
}

void readSensitiveTypeRegions(const char *name) {
  std::fstream cf(name, std::fstream::in);
  if (cf.is_open()) {
     std::string  line;
     while(std::getline(cf,line)) {
       if (line.find("/") != std::string::npos)
          continue;
       std::string marker, tname, offset, size;
       std::istringstream iss(line);
       std::getline(iss, marker,',');
       marker = ltrim(rtrim(marker));
       std::getline(iss,tname,',');
       tname = ltrim(rtrim(tname));
       std::getline(iss,offset,',');
       offset = ltrim(rtrim(offset));
       std::getline(iss, size,',');
       size = ltrim(rtrim(size));
       unsigned o = std::stoi(offset);
       unsigned s = std::stoi(size);
       std::vector<region> rs;
       if (marker == "high") {
          if (highTypeRegions.find(tname) != highTypeRegions.end())
             rs = highTypeRegions[tname];
          region r;
          r.offset = o;
          r.size = s;
          rs.push_back(r);
          highTypeRegions[tname] = rs;
       }   
       else if (marker == "low") {
          if (lowTypeRegions.find(tname) != lowTypeRegions.end())
             rs = lowTypeRegions[tname];
          region r;
          r.offset = o;
          r.size = s;
          rs.push_back(r);
          lowTypeRegions[tname] = rs;
       } 
     }
  }
  cf.close();
}

void readFuncArgTypeHints(const char *name) {
  std::fstream cf(name, std::fstream::in);
  if (cf.is_open()) {
     std::string  line;
     while(std::getline(cf,line)) {
       std::string fname, arg, tname;
       std::istringstream iss(line);
       std::getline(iss, fname,',');
       fname = ltrim(rtrim(fname));
       std::getline(iss, arg,',');
       arg = ltrim(rtrim(arg));
       unsigned int value = std::stoi(arg);
       std::getline(iss, tname,',');
       tname = ltrim(rtrim(tname));
       std::map<int, std::string> tmap;       
       if (funcArgTypeHints.find(fname) == funcArgTypeHints.end()) {
          tmap[value] = tname;
          funcArgTypeHints[fname] = tmap;
       }
       else {
          tmap = funcArgTypeHints[fname];
          if (tmap.find(value) != tmap.end())
             assert(false && "Cannot assign multiple type hints to the same argument!\n");
          tmap[value] = tname;
          funcArgTypeHints[fname] = tmap;  
       } 
     }
  }
  cf.close();
}


void processSensitivitySpec(std::string fname, int value, std::string marker, 
                            std::map<std::string, std::set<int> > &high, 
                            std::map<std::string, std::set<int> > &low, 
                            std::map<std::string, std::set<int> > &mixed) {

   if (marker == "high") {
      llvm::outs() << " high function " << fname << " arg " << value << "\n";
      std::set<int> args;
      if (high.find(fname) != high.end())
         args = high[fname];
      args.insert(value);       
      high[fname] = args;
   } 
   else if (marker == "low") {
      llvm::outs() << " low function " << fname << " arg " << value << "\n";
      std::set<int> args;
      if (low.find(fname) != low.end())
         args = low[fname];
         args.insert(value);       
         low[fname] = args;
   }
   else if (marker == "mixed") {
      llvm::outs() << " mixed function " << fname << " arg " << value << "\n";
      std::set<int> args;
      if (mixed.find(fname) != mixed.end())
         args = mixed[fname];
         args.insert(value);       
         mixed[fname] = args;            
      } 
   else assert(false && "function argument sensitivity other than high/low");
}

void readSensitiveFunctionArgs(const char *name, bool inflow) {
  std::fstream cf(name, std::fstream::in);
  if (cf.is_open()) {
     std::string  line;
     while(std::getline(cf,line)) {
         if (line.find("/") != std::string::npos)
            continue;
         std::string marker, fname, arg;
         std::istringstream iss(line);
         std::getline(iss, marker,',');  
         marker = ltrim(rtrim(marker));
         std::getline(iss, fname,',');
         fname = ltrim(rtrim(fname));
         std::getline(iss, arg,',');
         arg = ltrim(rtrim(arg));
         unsigned int value = std::stoi(arg);
         if (inflow)
            processSensitivitySpec(fname, value, marker, highFunctionArgs, lowFunctionArgs, mixedFunctionArgs);
         else {
            if (fname.find("[") != std::string::npos) {
               size_t pos1 = fname.find("[");
               size_t pos2 = fname.find("]"); 
               std::string rstr = fname.substr(pos1+1,pos2-pos1-1); 
               int rvalue = std::stoi(rstr);
               fname = fname.substr(0,pos1);
               inputFuncsSuccRetValue[fname] = rvalue;
               llvm::errs() << " recording " << rvalue << " as success return value of " << fname << "\n";
            }
            processSensitivitySpec(fname, value, marker, highFunctionArgsRet, lowFunctionArgsRet, mixedFunctionArgsRet);
            inputFuncs.insert(fname);
            std::set<unsigned int> args;
            if (inputFuncArgs.find(fname) != inputFuncArgs.end())
               args = inputFuncArgs[fname];
            args.insert(value);
            inputFuncArgs[fname] = args;
         }
     }
  }
  cf.close();
}

void readInputFuncs(const char *name) {
     readSensitiveFunctionArgs(name, false);
}

/* SYSREL extension */
void readPrefixes(const char *name) {
  std::fstream cf(name, std::fstream::in);
  if (cf.is_open()) {
     std::string  line;
     while(std::getline(cf,line)) { 
        std::string prefix = ltrim(rtrim(line));
        prefixRedact.insert(ltrim(rtrim(line)));
        llvm::outs() << " recording prefix " << prefix << "\n";  
     }
  }
  cf.close();
}

void readFrameworkDts(const char *name) {
  std::fstream cf(name, std::fstream::in);
  if (cf.is_open()) {
     std::string   line, dts, counts;
     int count;
     std::cout << "Framework dts:\n";
     while(std::getline(cf,line)) { 
       std::istringstream iss(line);
       getline(iss, dts, ',');
       //lazyInits.insert(dts);
       lazyInits.push_back(dts);
       getline(iss, counts, ',');
       count = atoi(counts.c_str());
       lazyInitNumInstances[dts] = count;
       std::cout << "data type=" << dts << " num inst=" << count << "\n";
     }
     cf.close();
  }
}

void readLazySingles(const char *name) {
  std::fstream cf(name, std::fstream::in);
  if (cf.is_open()) {
     std::string   line;
     llvm::outs() << "Lazy single instance types:\n";
     while (std::getline(cf,line)) { 
        std::string tline = ltrim(rtrim(line));
        lazyInitSingles.insert(tline);
        llvm::outs() << tline << "\n";
     }
     cf.close();
  }
}

/* 
  Syntax for setting to NULL:
  typename,offset_1,offset_2,...
  Syntax for setting a function pointer to a function
  typename,fptr,offset,functionname
*/
void readLazyInitInitializers(const char * name) {
 
  std::fstream cf(name, std::fstream::in);
  if (cf.is_open()) {
     std::string   line;
     llvm::outs() << "Lazy init initializers:\n";
     while (std::getline(cf,line)) { 
        std::string tname, offset, token, fname;  
        std::string tline = ltrim(rtrim(line));
        llvm::outs() << tline << "\n";
        std::istringstream iss(line);
        getline(iss, tname, ',');
        tname = ltrim(rtrim(tname));
        getline(iss,token, ',');
        token = ltrim(rtrim(token));
        if (token == "fptr") {
           getline(iss, offset, ',');
           offset = ltrim(rtrim(offset));
           unsigned int value = std::stoi(offset);
           getline(iss, fname, ',');
           fname = ltrim(rtrim(fname));
           std::map<unsigned int, std::string> fmap;
           fmap = lazyFuncPtrInitializers[tname]; 
           if (fmap.find(value) != fmap.end()) {
              llvm::errs() << "setting the same functin pointer field twice!\n";
              exit(1); 
           }
           fmap[value] = fname;
           lazyFuncPtrInitializers[tname] = fmap;
        } 
        else { 
         std::vector<unsigned int> offsets;
         llvm::outs() << "lazy init type=" << tname << ", offsets to be set to NULL:\n";
         while (getline(iss, offset, ',')) {
          offset = ltrim(rtrim(offset));
          unsigned int value = std::stoi(offset);
          llvm::outs() << value << " ";
          offsets.push_back(value); 
         }
         llvm::outs() << "\n";
         lazyInitInitializers[tname] = offsets;
       }
     }
     cf.close();
  }
}  

/*

void readProgModelSpec(const char *name) {
  regAPIHandler = new RegistrationAPIHandler();
  resADAPIHandler = new ResourceAllocReleaseAPIHandler();
  mutexAPIHandler = new MutexAPIHandler();
  refcountAPIHandler = new RefCountAPIHandler();
  setgetAPIHandler = new SetGetAPIHandler();
  allocAPIHandler = new AllocAPIHandler();
  readWriteAPIHandler = new ReadWriteAPIHandler();
  ignoreAPIHandler = new IgnoreAPIHandler();
  callbackAPIHandler = new CallbackAPIHandler();
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
          sideEffectAPIHandler->addAPIUpdateExpr(api, expr);
       }
       else if (desc.find("register,deregister[enable]") != std::string::npos) {
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
         resADAPIHandler->addResAllocDealloc(ltrim(rtrim(alloc)), ltrim(rtrim(dealloc)));   
       }
       else if (desc.find("acquire,release,init") != std::string::npos) {
         std::string acq, rel, init;
         std::istringstream iss2(data);
         getline(iss2, acq, ',');
         getline(iss2, rel, ',');
         getline(iss2, init, ',');
         mutexAPIHandler->addInitAcqRel(ltrim(rtrim(acq)), ltrim(rtrim(rel)), ltrim(rtrim(init)));
       } 
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
       }
       else if (desc.find("alloc,parameter,initzero,symbolic,return") != std::string::npos) {
         std::string alloc, param, init, sym, ret;
         std::istringstream iss2(data);
         getline(iss2, alloc, ',');
         getline(iss2, param, ',');
         getline(iss2, init, ',');
         getline(iss2, sym, ',');
         getline(iss2, ret, ',');
         allocAPIHandler->addAllocate(ltrim(rtrim(alloc)), std::stoi(ltrim(rtrim(param))), 
                       ltrim(rtrim(init)) == "true" ? true : false, ltrim(rtrim(sym)) == "true" ? true : false,
                       ltrim(rtrim(ret)) == "true" ? true : false);       
         llvm::outs() << " alloc,parameter,initzero,sym interface " << alloc << " " << param  << " " << init << " " << sym << " " << ret << "\n";
       }
       else if (desc.find("free,parameter") != std::string::npos) {
         std::string freeapi,  param;
         std::istringstream iss2(data);
         getline(iss2, freeapi, ',');
         getline(iss2, param, ',');
         freeAPIHandler->addFree(ltrim(rtrim(freeapi)), std::stoi(ltrim(rtrim(param)))); 
         llvm::outs() << " free,parameter interface " << freeapi << " " << param << "\n"; 
       }
       else if (desc.find("callback") !=  std::string::npos) {
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
       }
       else if (desc.find("ignore") != std::string::npos) {
         ignoreAPIHandler->addIgnore(ltrim(rtrim(data)));
       }
     }
     llvm::outs() << "Parsed Prog Model Spec \n";      
     cf.close();
  }
}
*/

Sequential *readLCMConfig(const char *name) {
  std::fstream cf(name, std::fstream::in);
  if (cf.is_open()) {
    llvm::outs() << "Reading LCM from " << name << "\n";
    Sequential *res = new Sequential();
    std::string   line;
    bool parallel = false;
    std::vector<std::string> group;
    std::string identifier = "";
    while(std::getline(cf,line)) { 
      std::stringstream lineStream(line);
      std::string token;
      while(lineStream >> token) {
        if (token == "(") {
           parallel = true;
        }
        else if (token == ")") {
            Parallel *p = new Parallel(group);
            res->addStep(p);
            parallel = false;
            group.clear();
        }
        else if (token == ";") {
            std::string fname, value;
            int success_retvalue = 0;
            if (identifier.find("[") != std::string::npos) {
               std::istringstream iss(identifier);
               getline(iss, fname, '[');  
               getline(iss, value, ']');
               success_retvalue = std::stoi(value);
               llvm::errs() << fname  << " success value= " << success_retvalue << "\n"; 
            }
            else fname = identifier; 
            Identifier *id = new Identifier(fname);
            id->setSuccessReturnValue(success_retvalue);
            res->addStep(id);
            identifier = "";
        }
        else {
          if (parallel)
            group.push_back(token);
          else identifier = token;
        } 
      }
      if (identifier != "") {
         std::string fname, value;
         int success_retvalue = 0;
         if (identifier.find("[") != std::string::npos) {
            std::istringstream iss(identifier);
            getline(iss, fname, '[');  
            getline(iss, value, ']');
            success_retvalue = std::stoi(value);
         }
         else fname = identifier; 
         Identifier *id = new Identifier(fname);
         id->setSuccessReturnValue(success_retvalue);
         res->addStep(id);
         identifier = "";        
      }
      else {
         Parallel *p = new Parallel(group);
         res->addStep(p);
         parallel = false;
         group.clear();        
      }
    }
    res->finalize();
    llvm::outs() << "Life cycle model: \n";
    res->print();
    return res;
  }
  else { 
    llvm::errs() << "Could not open file " << name << "\n";
    return NULL;
  }
}
/* SYSREL extension */



static bool interrupted = false;

// Pulled out so it can be easily called from a debugger.
extern "C"
void halt_execution() {
  theInterpreter->setHaltExecution(true);
}

extern "C"
void stop_forking() {
  theInterpreter->setInhibitForking(true);
}

static void interrupt_handle() {
  if (!interrupted && theInterpreter) {
    llvm::errs() << "KLEE: ctrl-c detected, requesting interpreter to halt.\n";
    halt_execution();
    sys::SetInterruptFunction(interrupt_handle);
  } else {
    llvm::errs() << "KLEE: ctrl-c detected, exiting.\n";
    exit(1);
  }
  interrupted = true;
}

static void interrupt_handle_watchdog() {
  // just wait for the child to finish
}

// This is a temporary hack. If the running process has access to
// externals then it can disable interrupts, which screws up the
// normal "nice" watchdog termination process. We try to request the
// interpreter to halt using this mechanism as a last resort to save
// the state data before going ahead and killing it.
static void halt_via_gdb(int pid) {
  char buffer[256];
  sprintf(buffer,
          "gdb --batch --eval-command=\"p halt_execution()\" "
          "--eval-command=detach --pid=%d &> /dev/null",
          pid);
  //  fprintf(stderr, "KLEE: WATCHDOG: running: %s\n", buffer);
  if (system(buffer)==-1)
    perror("system");
}

// returns the end of the string put in buf
static char *format_tdiff(char *buf, long seconds)
{
  assert(seconds >= 0);

  long minutes = seconds / 60;  seconds %= 60;
  long hours   = minutes / 60;  minutes %= 60;
  long days    = hours   / 24;  hours   %= 24;

  buf = strrchr(buf, '\0');
  if (days > 0) buf += sprintf(buf, "%ld days, ", days);
  buf += sprintf(buf, "%02ld:%02ld:%02ld", hours, minutes, seconds);
  return buf;
}

#ifndef SUPPORT_KLEE_UCLIBC
static llvm::Module *linkWithUclibc(llvm::Module *mainModule, StringRef libDir) {
  klee_error("invalid libc, no uclibc support!\n");
}
#else
static void replaceOrRenameFunction(llvm::Module *module,
		const char *old_name, const char *new_name)
{
  Function *f, *f2;
  f = module->getFunction(new_name);
  f2 = module->getFunction(old_name);
  if (f2) {
    if (f) {
      f2->replaceAllUsesWith(f);
      f2->eraseFromParent();
    } else {
      f2->setName(new_name);
      assert(f2->getName() == new_name);
    }
  }
}
static llvm::Module *linkWithUclibc(llvm::Module *mainModule, StringRef libDir) {
  LLVMContext &ctx = mainModule->getContext();
  // Ensure that klee-uclibc exists
  SmallString<128> uclibcBCA(libDir);
  llvm::sys::path::append(uclibcBCA, KLEE_UCLIBC_BCA_NAME);

#if LLVM_VERSION_CODE >= LLVM_VERSION(3, 6)
  Twine uclibcBCA_twine(uclibcBCA.c_str());
  if (!llvm::sys::fs::exists(uclibcBCA_twine))
#else
  bool uclibcExists=false;
  llvm::sys::fs::exists(uclibcBCA.c_str(), uclibcExists);
  if (!uclibcExists)
#endif
    klee_error("Cannot find klee-uclibc : %s", uclibcBCA.c_str());

  Function *f;
  // force import of __uClibc_main
  mainModule->getOrInsertFunction(
      "__uClibc_main",
      FunctionType::get(Type::getVoidTy(ctx), std::vector<Type *>(), true));

  // force various imports
  if (WithPOSIXRuntime) {
    llvm::Type *i8Ty = Type::getInt8Ty(ctx);
    mainModule->getOrInsertFunction("realpath",
                                    PointerType::getUnqual(i8Ty),
                                    PointerType::getUnqual(i8Ty),
                                    PointerType::getUnqual(i8Ty),
                                    NULL);
    mainModule->getOrInsertFunction("getutent",
                                    PointerType::getUnqual(i8Ty),
                                    NULL);
    mainModule->getOrInsertFunction("__fgetc_unlocked",
                                    Type::getInt32Ty(ctx),
                                    PointerType::getUnqual(i8Ty),
                                    NULL);
    mainModule->getOrInsertFunction("__fputc_unlocked",
                                    Type::getInt32Ty(ctx),
                                    Type::getInt32Ty(ctx),
                                    PointerType::getUnqual(i8Ty),
                                    NULL);
  }

  f = mainModule->getFunction("__ctype_get_mb_cur_max");
  if (f) f->setName("_stdlib_mb_cur_max");

  // Strip of asm prefixes for 64 bit versions because they are not
  // present in uclibc and we want to make sure stuff will get
  // linked. In the off chance that both prefixed and unprefixed
  // versions are present in the module, make sure we don't create a
  // naming conflict.
  for (Module::iterator fi = mainModule->begin(), fe = mainModule->end();
       fi != fe; ++fi) {
    Function *f = &*fi;
    const std::string &name = f->getName();
    if (name[0]=='\01') {
      unsigned size = name.size();
      if (name[size-2]=='6' && name[size-1]=='4') {
        std::string unprefixed = name.substr(1);

        // See if the unprefixed version exists.
        if (Function *f2 = mainModule->getFunction(unprefixed)) {
          f->replaceAllUsesWith(f2);
          f->eraseFromParent();
        } else {
          f->setName(unprefixed);
        }
      }
    }
  }

  mainModule = klee::linkWithLibrary(mainModule, uclibcBCA.c_str());
  assert(mainModule && "unable to link with uclibc");


  replaceOrRenameFunction(mainModule, "__libc_open", "open");
  replaceOrRenameFunction(mainModule, "__libc_fcntl", "fcntl");

  // XXX we need to rearchitect so this can also be used with
  // programs externally linked with uclibc.

  // We now need to swap things so that __uClibc_main is the entry
  // point, in such a way that the arguments are passed to
  // __uClibc_main correctly. We do this by renaming the user main
  // and generating a stub function to call __uClibc_main. There is
  // also an implicit cooperation in that runFunctionAsMain sets up
  // the environment arguments to what uclibc expects (following
  // argv), since it does not explicitly take an envp argument.
  Function *userMainFn = mainModule->getFunction(EntryPoint);
  entryFunctionName = userMainFn->getName();
  sidechannelentry = (SideChannelEntryPoint.c_str()) ? SideChannelEntryPoint : entryFunctionName ;
  assert(userMainFn && "unable to get user main");
  Function *uclibcMainFn = mainModule->getFunction("__uClibc_main");
  assert(uclibcMainFn && "unable to get uclibc main");
  userMainFn->setName("__user_main");

  const FunctionType *ft = uclibcMainFn->getFunctionType();
  assert(ft->getNumParams() == 7);

  std::vector<Type *> fArgs;
  fArgs.push_back(ft->getParamType(1)); // argc
  fArgs.push_back(ft->getParamType(2)); // argv
  Function *stub = Function::Create(FunctionType::get(Type::getInt32Ty(ctx), fArgs, false),
                                    GlobalVariable::ExternalLinkage,
                                    EntryPoint,
                                    mainModule);
  BasicBlock *bb = BasicBlock::Create(ctx, "entry", stub);

  std::vector<llvm::Value*> args;
  args.push_back(llvm::ConstantExpr::getBitCast(userMainFn,
                                                ft->getParamType(0)));
  args.push_back(&*(stub->arg_begin())); // argc
  args.push_back(&*(++stub->arg_begin())); // argv
  args.push_back(Constant::getNullValue(ft->getParamType(3))); // app_init
  args.push_back(Constant::getNullValue(ft->getParamType(4))); // app_fini
  args.push_back(Constant::getNullValue(ft->getParamType(5))); // rtld_fini
  args.push_back(Constant::getNullValue(ft->getParamType(6))); // stack_end
  CallInst::Create(uclibcMainFn, args, "", bb);

  new UnreachableInst(ctx, bb);

  klee_message("NOTE: Using klee-uclibc : %s", uclibcBCA.c_str());
  return mainModule;
}
#endif

int main(int argc, char **argv, char **envp) {
  atexit(llvm_shutdown);  // Call llvm_shutdown() on exit.

  llvm::InitializeNativeTarget();

  parseArguments(argc, argv);
#if LLVM_VERSION_CODE >= LLVM_VERSION(3, 9)
  sys::PrintStackTraceOnErrorSignal(argv[0]);
#else
  sys::PrintStackTraceOnErrorSignal();
#endif

  if (Watchdog) {
    if (MaxTime==0) {
      klee_error("--watchdog used without --max-time");
    }

    int pid = fork();
    if (pid<0) {
      klee_error("unable to fork watchdog");
    } else if (pid) {
      klee_message("KLEE: WATCHDOG: watching %d\n", pid);
      fflush(stderr);
      sys::SetInterruptFunction(interrupt_handle_watchdog);

      double nextStep = util::getWallTime() + MaxTime*1.1;
      int level = 0;

      // Simple stupid code...
      while (1) {
        sleep(1);

        int status, res = waitpid(pid, &status, WNOHANG);

        if (res < 0) {
          if (errno==ECHILD) { // No child, no need to watch but
                               // return error since we didn't catch
                               // the exit.
            klee_warning("KLEE: watchdog exiting (no child)\n");
            return 1;
          } else if (errno!=EINTR) {
            perror("watchdog waitpid");
            exit(1);
          }
        } else if (res==pid && WIFEXITED(status)) {
          return WEXITSTATUS(status);
        } else {
          double time = util::getWallTime();

          if (time > nextStep) {
            ++level;

            if (level==1) {
              klee_warning(
                  "KLEE: WATCHDOG: time expired, attempting halt via INT\n");
              kill(pid, SIGINT);
            } else if (level==2) {
              klee_warning(
                  "KLEE: WATCHDOG: time expired, attempting halt via gdb\n");
              halt_via_gdb(pid);
            } else {
              klee_warning(
                  "KLEE: WATCHDOG: kill(9)ing child (I tried to be nice)\n");
              kill(pid, SIGKILL);
              return 1; // what more can we do
            }

            // Ideally this triggers a dump, which may take a while,
            // so try and give the process extra time to clean up.
            nextStep = util::getWallTime() + std::max(15., MaxTime*.1);
          }
        }
      }

      return 0;
    }
  }

  sys::SetInterruptFunction(interrupt_handle);

  // Load the bytecode...
  std::string errorMsg;
  LLVMContext ctx;
  Module *mainModule = klee::loadModule(ctx, InputFile, errorMsg);
  if (!mainModule) {
    klee_error("error loading program '%s': %s", InputFile.c_str(),
               errorMsg.c_str());
  }

  if (WithPOSIXRuntime) {
    int r = initEnv(mainModule);
    if (r != 0)
      return r;
  }

  std::string LibraryDir = KleeHandler::getRunTimeLibraryPath(argv[0]);
  Interpreter::ModuleOptions Opts(LibraryDir.c_str(), EntryPoint,
                                  /*Optimize=*/OptimizeModule,
                                  /*CheckDivZero=*/CheckDivZero,
                                  /*CheckOvershift=*/CheckOvershift);

  switch (Libc) {
  case NoLibc: /* silence compiler warning */
    break;

  case KleeLibc: {
    // FIXME: Find a reasonable solution for this.
    SmallString<128> Path(Opts.LibraryDir);
    llvm::sys::path::append(Path, "klee-libc.bc");
    mainModule = klee::linkWithLibrary(mainModule, Path.c_str());
    assert(mainModule && "unable to link with klee-libc");
    break;
  }

  case UcLibc:
    mainModule = linkWithUclibc(mainModule, LibraryDir);
    break;
  }

  if (WithPOSIXRuntime) {
    SmallString<128> Path(Opts.LibraryDir);
    llvm::sys::path::append(Path, "libkleeRuntimePOSIX.bca");
    klee_message("NOTE: Using model: %s", Path.c_str());
    mainModule = klee::linkWithLibrary(mainModule, Path.c_str());
    assert(mainModule && "unable to link with simple model");
  }

  std::vector<std::string>::iterator libs_it;
  std::vector<std::string>::iterator libs_ie;
  for (libs_it = LinkLibraries.begin(), libs_ie = LinkLibraries.end();
          libs_it != libs_ie; ++libs_it) {
    const char * libFilename = libs_it->c_str();
    klee_message("Linking in library: %s.\n", libFilename);
    mainModule = klee::linkWithLibrary(mainModule, libFilename);
  }
  // Get the desired main function.  klee_main initializes uClibc
  // locale and other data and then calls main.
  Function *mainFn = mainModule->getFunction(EntryPoint);
  entryFunctionName = mainFn->getName();
  sidechannelentry = (SideChannelEntryPoint.c_str()) ? SideChannelEntryPoint : entryFunctionName ;
  if (!mainFn) {
    klee_error("'%s' function not found in module.", EntryPoint.c_str());
  }

  // FIXME: Change me to std types.
  int pArgc;
  char **pArgv;
  char **pEnvp;
  if (Environ != "") {
    std::vector<std::string> items;
    std::ifstream f(Environ.c_str());
    if (!f.good())
      klee_error("unable to open --environ file: %s", Environ.c_str());
    while (!f.eof()) {
      std::string line;
      std::getline(f, line);
      line = strip(line);
      if (!line.empty())
        items.push_back(line);
    }
    f.close();
    pEnvp = new char *[items.size()+1];
    unsigned i=0;
    for (; i != items.size(); ++i)
      pEnvp[i] = strdup(items[i].c_str());
    pEnvp[i] = 0;
  } else {
    pEnvp = envp;
  }

  pArgc = InputArgv.size() + 1;
  pArgv = new char *[pArgc];
  for (unsigned i=0; i<InputArgv.size()+1; i++) {
    std::string &arg = (i==0 ? InputFile : InputArgv[i-1]);
    unsigned size = arg.size() + 1;
    char *pArg = new char[size];

    std::copy(arg.begin(), arg.end(), pArg);
    pArg[size - 1] = 0;

    pArgv[i] = pArg;
  }

  std::vector<bool> replayPath;

  if (ReplayPathFile != "") {
    KleeHandler::loadPathFile(ReplayPathFile, replayPath);
  }

  Interpreter::InterpreterOptions IOpts;
  IOpts.MakeConcreteSymbolic = MakeConcreteSymbolic;
  KleeHandler *handler = new KleeHandler(pArgc, pArgv);
  Interpreter *interpreter =
    theInterpreter = Interpreter::create(ctx, IOpts, handler);
  handler->setInterpreter(interpreter);

  for (int i=0; i<argc; i++) {
    handler->getInfoStream() << argv[i] << (i+1<argc ? " ":"\n");
  }
  handler->getInfoStream() << "PID: " << getpid() << "\n";

  const Module *finalModule =
    interpreter->setModule(mainModule, Opts);
  /* begin SYSREL extension */
  if (PrimitiveArraySize)
     primArraySize = PrimitiveArraySize;

  if (MaxForkMulResolution)
     maxForkMulRes = MaxForkMulResolution;

  if (AsyncMode) {
     moduleHandle = finalModule; 
     asyncMode = true;
     for(unsigned int i=0; i < AsyncEntryPoints.size(); i++) {
       llvm::outs() << "Registering async function " << AsyncEntryPoints[i] << "\n";
       asyncFunc.push_back(AsyncEntryPoints[i]);
     }
  }

  if (LazyInit)
     moduleHandle = finalModule; 

  if (LifeCycleConf != "") {
     moduleHandle = finalModule; 
     Sequential *lcm = readLCMConfig(LifeCycleConf.c_str());
     if (lcm) {
        ExecutionState::setLifeCycleModel(lcm);
        llvm::outs() << "Life cycle model:\n";
        lcm->print();
     }
  }

  lazyInit = LazyInit;

  if (FrameworkDts != "") {
     llvm::errs() << "Framework dts=" << FrameworkDts.c_str() << "\n";
     assert(LazyInit);
     lazySpec = true;
     readFrameworkDts(FrameworkDts.c_str());
  }


  if (LazyInit) {
     llvm::outs() << "Lazy initialization is ON \n";
     if (LazyInitInst)
        numLazyInst = LazyInitInst;
     if (!lazySpec && !numLazyInst)
        assert(false && "For automatic lazy instantiation type inference mode, number of instances need to be specified!");
     llvm::outs() << "lazy single spec file? " << LazySingle.c_str() << "\n";
     if (LazySingle != "")
        readLazySingles(LazySingle.c_str()); 
     if (LazyInitializer != "")
        readLazyInitInitializers(LazyInitializer.c_str());     
     if (InferenceClue != "")
        llvm::outs() << "Reading inference clue from " << InferenceClue.c_str() << "\n";
        readInferenceClue(InferenceClue.c_str());
     if (VoidTypeCasts != "")
        readVoidTypeCasts(VoidTypeCasts.c_str());
  }

  if (ProgModelSpec != "") {
     moduleHandle = finalModule; 
     progModel = true;
     APIHandler::readProgModelSpec(ProgModelSpec.c_str());
  }

  if (SymbolizeInlineAssembly)
     symbolizeInlineAssembly = true;

  if (TimingObservationPoints != "")
     readTimingObservationPoints(TimingObservationPoints.c_str());

  if (FuncArgTypeHints != "")
     readFuncArgTypeHints(FuncArgTypeHints.c_str());

  if (InputFuncs != "") {
     readInputFuncs(InputFuncs.c_str());
     for(auto a: inputFuncs) 
        llvm::errs() << "input function: " << a << "\n";
  }

  if (SensitiveInputs != "")
     readSensitiveFunctionArgs(SensitiveInputs.c_str(), true);

  if (SensitiveTypeRegions != "")
     readSensitiveTypeRegions(SensitiveTypeRegions.c_str());

  if (CheckStackLeak != "")
     readCheckStackLeak(CheckStackLeak.c_str());

  if (CacheLineBits && CacheBitMask) {
     llvm::errs() << "Choose either cache line mode (by setting cache line bits) or cache bitmask (by setting cache bitmask)!\n";
     exit(1);
  }
      
  if (CacheLineBits) {
     cacheLineMode = true;
     cacheLineBits = CacheLineBits;
  }

  if (CacheBitMask) {
     cacheBitmaskMode = true;
     cacheBitmask = CacheBitMask;
  }
  else if (!cacheLineMode) {
     llvm::errs() << "Using cache line mode (as default)!\n";
     cacheLineMode = true;    
  }

  if (InfoFlowSummaryMode) {
     infoFlowSummaryMode = true;
  }

  if (InfoFlowSummaryEntry != "") {
     infoFlowSummarizedFuncName = InfoFlowSummaryEntry;
     infoFlowSummaryMode = true;
  }
  else if (InfoFlowSummaryMode) 
     infoFlowSummarizedFuncName = entryFunctionName;     


  if (InfoFlowModels != "") {
     readInfoFlowModels(InfoFlowModels.c_str());
  }
 
  if (TimingModels != "") {
     readTimingModels(TimingModels.c_str());
  }

  if (PrefixRedact != "") {
     readPrefixes(PrefixRedact.c_str());
  }

  /* end SYSREL extension */

  externalsAndGlobalsCheck(finalModule);

  if (ReplayPathFile != "") {
    interpreter->setReplayPath(&replayPath);
  }

  char buf[256];
  time_t t[2];
  t[0] = time(NULL);
  strftime(buf, sizeof(buf), "Started: %Y-%m-%d %H:%M:%S\n", localtime(&t[0]));
  handler->getInfoStream() << buf;
  handler->getInfoStream().flush();

  if (!ReplayKTestDir.empty() || !ReplayKTestFile.empty()) {
    assert(SeedOutFile.empty());
    assert(SeedOutDir.empty());

    std::vector<std::string> kTestFiles = ReplayKTestFile;
    for (std::vector<std::string>::iterator
           it = ReplayKTestDir.begin(), ie = ReplayKTestDir.end();
         it != ie; ++it)
      KleeHandler::getKTestFilesInDir(*it, kTestFiles);
    std::vector<KTest*> kTests;
    for (std::vector<std::string>::iterator
           it = kTestFiles.begin(), ie = kTestFiles.end();
         it != ie; ++it) {
      KTest *out = kTest_fromFile(it->c_str());
      if (out) {
        kTests.push_back(out);
      } else {
        klee_warning("unable to open: %s\n", (*it).c_str());
      }
    }

    if (RunInDir != "") {
      int res = chdir(RunInDir.c_str());
      if (res < 0) {
        klee_error("Unable to change directory to: %s - %s", RunInDir.c_str(),
                   sys::StrError(errno).c_str());
      }
    }

    unsigned i=0;
    for (std::vector<KTest*>::iterator
           it = kTests.begin(), ie = kTests.end();
         it != ie; ++it) {
      KTest *out = *it;
      interpreter->setReplayKTest(out);
      llvm::errs() << "KLEE: replaying: " << *it << " (" << kTest_numBytes(out)
                   << " bytes)"
                   << " (" << ++i << "/" << kTestFiles.size() << ")\n";
      // XXX should put envp in .ktest ?
      interpreter->runFunctionAsMain(mainFn, out->numArgs, out->args, pEnvp);
      if (interrupted) break;
    }
    interpreter->setReplayKTest(0);
    while (!kTests.empty()) {
      kTest_free(kTests.back());
      kTests.pop_back();
    }
  } else {
    std::vector<KTest *> seeds;
    for (std::vector<std::string>::iterator
           it = SeedOutFile.begin(), ie = SeedOutFile.end();
         it != ie; ++it) {
      KTest *out = kTest_fromFile(it->c_str());
      if (!out) {
        klee_error("unable to open: %s\n", (*it).c_str());
      }
      seeds.push_back(out);
    }
    for (std::vector<std::string>::iterator
           it = SeedOutDir.begin(), ie = SeedOutDir.end();
         it != ie; ++it) {
      std::vector<std::string> kTestFiles;
      KleeHandler::getKTestFilesInDir(*it, kTestFiles);
      for (std::vector<std::string>::iterator
             it2 = kTestFiles.begin(), ie = kTestFiles.end();
           it2 != ie; ++it2) {
        KTest *out = kTest_fromFile(it2->c_str());
        if (!out) {
          klee_error("unable to open: %s\n", (*it2).c_str());
        }
        seeds.push_back(out);
      }
      if (kTestFiles.empty()) {
        klee_error("seeds directory is empty: %s\n", (*it).c_str());
      }
    }

    if (!seeds.empty()) {
      klee_message("KLEE: using %lu seeds\n", seeds.size());
      interpreter->useSeeds(&seeds);
    }
    if (RunInDir != "") {
      int res = chdir(RunInDir.c_str());
      if (res < 0) {
        klee_error("Unable to change directory to: %s - %s", RunInDir.c_str(),
                   sys::StrError(errno).c_str());
      }
    }
    interpreter->runFunctionAsMain(mainFn, pArgc, pArgv, pEnvp);

    while (!seeds.empty()) {
      kTest_free(seeds.back());
      seeds.pop_back();
    }
  }

  t[1] = time(NULL);
  strftime(buf, sizeof(buf), "Finished: %Y-%m-%d %H:%M:%S\n", localtime(&t[1]));
  handler->getInfoStream() << buf;

  strcpy(buf, "Elapsed: ");
  strcpy(format_tdiff(buf, t[1] - t[0]), "\n");
  handler->getInfoStream() << buf;

  // Free all the args.
  for (unsigned i=0; i<InputArgv.size()+1; i++)
    delete[] pArgv[i];
  delete[] pArgv;

  delete interpreter;

  uint64_t queries =
    *theStatisticManager->getStatisticByName("Queries");
  uint64_t queriesValid =
    *theStatisticManager->getStatisticByName("QueriesValid");
  uint64_t queriesInvalid =
    *theStatisticManager->getStatisticByName("QueriesInvalid");
  uint64_t queryCounterexamples =
    *theStatisticManager->getStatisticByName("QueriesCEX");
  uint64_t queryConstructs =
    *theStatisticManager->getStatisticByName("QueriesConstructs");
  uint64_t instructions =
    *theStatisticManager->getStatisticByName("Instructions");
  uint64_t forks =
    *theStatisticManager->getStatisticByName("Forks");


  llvm::errs() << "all high sym regions: \n";
  for(auto h : highSymRegions) {
      llvm::errs() << h.first << "\n";
      for(auto r: h.second) {
         llvm::errs() << "offset=" << r.offset << ", size=" << r.size << "\n"; 
      }
  }
  llvm::errs() << "all low sym regions: \n";
  for(auto lw : lowSymRegions) {
      llvm::errs() << lw.first << "\n"; 
           for(auto r: lw.second) {
         llvm::errs() << "offset=" << r.offset << ", size=" << r.size << "\n"; 
      }
  }

  handler->getInfoStream()
    << "KLEE: done: explored paths = " << 1 + forks << "\n";

  // Write some extra information in the info file which users won't
  // necessarily care about or understand.
  if (queries)
    handler->getInfoStream()
      << "KLEE: done: avg. constructs per query = "
                             << queryConstructs / queries << "\n";
  handler->getInfoStream()
    << "KLEE: done: total queries = " << queries << "\n"
    << "KLEE: done: valid queries = " << queriesValid << "\n"
    << "KLEE: done: invalid queries = " << queriesInvalid << "\n"
    << "KLEE: done: query cex = " << queryCounterexamples << "\n";

  std::stringstream stats;
  stats << "\n";
  stats << "KLEE: done: #Timing Side Channel  Locs = " << locs.size() << "\n";
  stats << "KLEE: done: #Cache Side Channel  Locs = " << cachelocs.size() << "\n";
  stats << "KLEE: done: minInst = " << minInstCount << "\n";
  stats << "KLEE: done: maxInst = " << maxInstCount << "\n";
  stats << "KLEE: done: HAncestors = " << RD::numHAncestors << "\n";
  stats << "KLEE: done: HLeaksOnStack = " <<  codeLocHighSecurityLeaksOnStack.size() << "\n";
  stats << "KLEE: done: HLMixedConstraints = " << RD::numHLMixedConstraints << "\n";
  stats << "KLEE: done: HVars= " << highLoc->size() << "\n"; 
  stats << "KLEE: done: LVars= " << lowLoc->size() << "\n"; 
  stats << "KLEE: done: total instructions = "
        << instructions << "\n";
  stats << "KLEE: done: completed paths = "
        << handler->getNumPathsExplored() << "\n";
  stats << "KLEE: done: generated tests = "
        << handler->getNumTestCases() << "\n";

  llvm::errs() << " Code loc with leaks:\n";
  for(auto ss: codeLocHighSecurityLeaksOnStack)
     llvm::errs() << ss << "\n";

  bool useColors = llvm::errs().is_displayed();
  if (useColors)
    llvm::errs().changeColor(llvm::raw_ostream::GREEN,
                             /*bold=*/true,
                             /*bg=*/false);

  llvm::errs() << stats.str();

  if (useColors)
    llvm::errs().resetColor();

  handler->getInfoStream() << stats.str();

  writeSensitiveFlows("sensitiveFlows.txt"); 
  writeSensitiveLeaksOnStack("sensitiveLeaksOnStack.txt");
  writeSecuritySenstiveBranches("securitySensitiveBranches.txt");
  writeCacheSideChannels("cacheSideChannels.txt");

  delete handler;

  return 0;
}
