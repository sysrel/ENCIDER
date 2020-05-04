#include <map>
#include <algorithm>
#include "AnalyzeConstraint.h"
#include "ResourceUsage.h"
#include "klee/Internal/Support/ErrorHandling.h"
#include "TimingSolver.h"
#include "klee/Expr.h"
#include "klee/util/ExprPPrinter.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/CFG.h"
#include <fstream>
#include <iomanip>      
#define ICost 1
#define epsilon 0

using namespace llvm;
using namespace klee;

bool onlycompletedpaths = true;
typedef std::pair<ref<Expr>, ref<Expr> > HCLC;
typedef std::pair<int, int> range;
typedef std::pair<unsigned, unsigned> exhash;
typedef std::map<exhash, range> RU;
typedef std::map<exhash, HCLC> HashExpr;
typedef std::map<exhash, ref<Expr> > HashRet;

// source code locations with timing side channels
std::set<std::pair<std::string, int>> locs;
// source code locations with cache side channels
std::set<std::pair<std::string, int>> cachelocs;
// ignore basic blocks that are farther away than this distance
// when detecting code based cache side channels
//unsigned basicBlockDistance = 2;

cl::opt<unsigned>
BasicBlockDist("bb-dist", cl::desc("The threshold distance for considering basic blocks \ 
reachable from the secret dependent branch for code based cache side channels (default is 2)\n"));

cl::opt<bool>
PeerBasicBlocks("peer-bbs", cl::desc("Use peer basic blocks to check for code based cache side channels \n"));


//Enclave functions
std::vector<std::string> * untrusted = new std::vector<std::string>();
std::vector<ExecutionState*> * haltedStates = new std::vector<ExecutionState*>();
int vc = 0;
std::vector<llvm::Instruction*> *vil = new std::vector<llvm::Instruction*>();
std::map<std::string, std::set<int> > dynamicHighLoc;
std::map<std::string, std::set<int> > dynamicLowLoc;

std::map<std::string, unsigned int> timingModels;

std::map<unsigned, std::string> binaryMetadata;
std::map<std::string, std::map<std::string, std::map<unsigned, 
        std::set<std::pair<unsigned int, unsigned int> >* >* >* > binaryAddresses;
std::set<std::pair<std::string, int>> codecachelocs;

extern unsigned long cacheLineBits;
extern bool cacheLineMode;
extern bool cacheBitmaskMode;
extern unsigned int cacheBitmask;

extern bool lookupAddressRange(std::string file, std::string func, unsigned line, 
            std::set<std::pair<unsigned int, unsigned int> > *&p);

extern bool lookupAddressRangeNoFile(std::string func, unsigned line, 
            std::set<std::pair<unsigned int, unsigned int> > *&p);


bool sameBB(BasicBlock *bb1, BasicBlock *bb2) {
     Instruction &inst1 = (*(bb1->begin())); 
     Instruction &inst2 = (*(bb2->begin()));      
     return inst1.isIdenticalTo(&inst2);
}

void findPeerBBs(BasicBlock *tb, BBset &result) {
     llvm::errs() << "finding peers of " << (*(tb->begin())) << "\n";
     for(BasicBlock *bbi : llvm::predecessors(tb)) {
        for(BasicBlock *pc : llvm::successors(bbi)) {
           if (!sameBB(pc,tb)) {
              llvm::errs() << "different blocks:\n";
              llvm::errs() << "1st inst in bb (" << pc << ") " << (*(pc->begin())) << "\n";
              llvm::errs() << "1st inst in bb (" << tb << ") " << (*(tb->begin())) << "\n";
              result.insert((long)pc);
           }
        }
     }
}

void findPeerBBs(BBset &b, BBset &result) {
    for(auto bi : b) {
       findPeerBBs((BasicBlock*)bi, result);
    }
}

void findBBAtDistance(BasicBlock *bb, unsigned int current, unsigned int dist, 
      BBset &result) {
   if (current > dist) return;
   for(BasicBlock *bbi : llvm::successors(bb)) {
       llvm::errs() << "current=" << current << " including bb of inst (" << bbi << ") " << (*(bbi->begin())) << "\n";
       result.insert((long)bbi);
       findBBAtDistance(bbi, current+1, dist, result); 
   }
     
}

void findBBAtDistance(Instruction *inst, unsigned int dist, BBset &result) {
   findBBAtDistance(inst->getParent(), 1, dist, result); 
}

/* 
   s1 represents the addresses in the difference and 
   s2 represents the addresses in the other path 
*/
bool checkCodeCacheSideChannel(Executor* ex, RD* rd, 
       std::set<std::pair<unsigned int, unsigned int> > &s1, 
         std::set<std::pair<unsigned int, unsigned int> > &s2, 
           std::pair<std::string, int> &p, double &accuracy) {

     if (s1.size() == 0 || s2.size() == 0) {
        llvm::errs() << "diff addresses " << s1.size() << 
                        " sibling addresses " << s2.size() << 
                        " for branch " << (*rd->i) << "\n"; 
        return false;
     }


     std::stringstream ss;

     bool found = false;
     unsigned num_total_regions, num_missing_regions;
     num_total_regions = s1.size()*s2.size();
     num_missing_regions = 0;
     for(auto s1i : s1) {
        std::set<unsigned int> s1C, s2C;        
        for(auto s2i : s2) {
          for(unsigned i=s2i.first; i <= s2i.second; i++) {
             if (cacheLineMode) {
                s2C.insert(i >> cacheLineBits);
                llvm::errs() << "other branch cache line " << (i >> cacheLineBits) << "\n";         

             }
             else if (cacheBitmaskMode) {
                s2C.insert(i & cacheBitmask);    
                llvm::errs() << "other branch cache bitmask res " << (i & cacheBitmask) << "\n";
             }
          }
        

          std::set<unsigned int> codeCacheLines, missingCL;
          for(unsigned i=s1i.first; i <= s1i.second; i++) {              
              if (cacheLineMode) {
                 int j = i >> cacheLineBits;
                 if  (s2C.find(j) == s2C.end()) {
                     codeCacheLines.insert(i); 
                     missingCL.insert(j);
                     llvm::errs() << "missing branch cache line " << j << "\n";    
                 }
              }
              else if (cacheBitmaskMode) {
                 int j = i & cacheBitmask;
                 if (s2C.find(j) == s2C.end())
                    codeCacheLines.insert(i);
                    missingCL.insert(j); 
                    llvm::errs() << "missing branch cache bitmask " << j << "\n";    
              }
          }

          if (codeCacheLines.size() > 0) {
             num_missing_regions++;
             found = true;
             ss << "\n" << "missing addresses";
             for(auto cc : codeCacheLines) {
                ss << "\n" << std::hex << cc;
             }
             ss << "\n" << "missing cache lines";
             for(auto cl : missingCL) {
                ss << "\n" << cl;
             }
             ss << "\n" << "missing CL= " << missingCL.size();          
          }

       }
     }
     double ratio = ((double)100*num_missing_regions)/(num_total_regions);
     ss << "\n" << "missing =" << num_missing_regions << " total=" << num_total_regions << 
           "\n" << "accuracy=%" << ratio ;
     accuracy = ratio;
     p.first = p.first + "\n" + ss.str();

     return found;
}

/*
bool checkCodeCacheSideChannel1(Executor* ex, RD* rd, 
       std::set<std::pair<unsigned int, unsigned int> > &s1, 
         std::set<std::pair<unsigned int, unsigned int> > &s2, 
           std::pair<std::string, int> &p) {

       llvm::errs() << "Virtual address matching begins\n";

       std::set<unsigned int> s1C, s2C;
       unsigned total = 0;

       for(auto s2i: s2) { 

          for(unsigned i=s2i.first; i <= s2i.second; i++) {
             compared++;
             llvm::errs() << "other branch address " << i << "\n";
             if (cacheLineMode) {
                s2C.insert(i >> cacheLineBits);
                llvm::errs() << "other branch cache line " << (i >> cacheLineBits) << "\n";             
             }
             else if (cacheBitmaskMode) {
                s2C.insert(i & cacheBitmask);    
                llvm::errs() << "other branch cache bitmask res " << (i & cacheBitmask) << "\n";
             }
          }
       }

       if (compared == 0) return false;

       std::set<unsigned int> codeCacheLines, missingCL;
       unsigned int total, missing, checked_regions;
       total = missing = checked_regions = 0;
       for(auto s1i : s1) {
          checked_regions++;
          for(unsigned i=s1i.first; i <= s1i.second; i++) {
              total++;
              if (cacheLineMode) {
                 int j = i >> cacheLineBits;
                 if  (s2C.find(j) == s2C.end()) {
                     codeCacheLines.insert(i); 
                     missingCL.insert(j);
                     missing++;
                     llvm::errs() << "missing branch cache line " << j << "\n";    
                 }
              }
              else if (cacheBitmaskMode) {
                 int j = i & cacheBitmask;
                 if (s2C.find(j) == s2C.end())
                    codeCacheLines.insert(i);
                    missingCL.insert(j); 
                    missing++; 
                    llvm::errs() << "missing branch cache bitmask " << j << "\n";    
              }
          }
       }
  
       if (codeCacheLines.size() > 0) {
          std::stringstream ss;
          double ratio = ((double)100*missing)/total;
          ss <<"\n" << "#checked regions=" << checked_regions << 
               "\n" << "#compared regions=" << compared_regions << 
               "\n" << "missing CL= " << missingCL.size() << 
               "\n" << "missing ADDR=" << missing << " total=" << total << 
               "\n" << "size of compared=" << compared << "\n" << ratio ;
          for(auto cc : codeCacheLines) {
             ss << "\n" << std::hex << cc;
          }
          p.first = p.first + "\n" + ss.str();
          return true;
       } 
       return false;
}
*/

void findSourceLocInfos(BasicBlock &bb, 
           std::set<std::pair<unsigned int, unsigned int> > &pset) {
   for(BasicBlock::iterator it = bb.begin(); it != bb.end(); it++) {
      if (MDNode *N = it->getMetadata("dbg")) {
         DILocation *Loc = cast<DILocation>(N);
         std::string file = Loc->getFilename();
         unsigned pos = file.find_last_of("/");
         if (pos != std::string::npos)
            file = file.substr(pos+1);
         unsigned line = Loc->getLine();
         std::string func = bb.getParent()->getName();
         std::set<std::pair<unsigned int, unsigned int> > *p;
         //llvm::errs() << file << "," << func << "," << line << " maps to? ";
         //if (lookupAddressRange(file, func, line, p)) {
         if (lookupAddressRangeNoFile(func, line, p)) {
            for(auto pi : *p) {
               pset.insert(pi);
               //llvm::errs() << pi.first << "-" << pi.second;
            }
         }
         //else llvm::errs() << "no match!";
      }
   }
}

void findVirtualAddresses(BasicBlock *bb,  
         std::set<std::pair<unsigned int, unsigned int> > &aset) {
     findSourceLocInfos(*bb, aset);
}

void findVirtualAddresses(BBset &a,  
         std::set<std::pair<unsigned int, unsigned int> > &aset) {
     for(auto ab : a) {
        BasicBlock *bb = (BasicBlock*)ab;
        findSourceLocInfos(*bb, aset);
     }
}



bool checkCacheLeakage(Executor *ex, RD* rd, BBset diff, BBset b, ref<Expr> f) {

  if (diff.size() > 0) {
     llvm::errs() << "CACHE CODE BB diff: " << diff.size() << "\n";
     for(auto ai : diff) {
        llvm::BasicBlock *bb = (BasicBlock*)ai;
        llvm::errs() << "1st inst in bb (" << bb << ") " << (*(bb->begin())) << "\n";
     }
     llvm::errs() << " other set of BBs:\n";
     for(auto bi : b) {
        llvm::BasicBlock *bb = (BasicBlock*)bi;
        llvm::errs() << "1st inst in bb (" << bb << ") " << &(*(bb->begin())) << " " << (*(bb->begin())) << "\n";
     }     
     const InstructionInfo &ii = ex->kmodule->infos->getInfo(rd->i);
     llvm::errs() << "Instruction:\n";
     llvm::errs() << "rd=" << rd->stateid << " " << (*rd->i) << "\n";
     printInfo(ii);
     std::pair<std::string, int> p;
     p.first = "CODE BASED CACHE SIDE CHANNEL!\n";
     std::set<long> diffLeakage;
     double max_accuracy = 0;
     bool found = false;
     for(auto di : diff) {
        std::set<std::pair<unsigned int, unsigned int> > diffset, aset, bset;
        findVirtualAddresses((BasicBlock*)di, aset);
        BBset filter, temp;
        if (BasicBlockDist || PeerBasicBlocks) {
           if (PeerBasicBlocks) {
              findPeerBBs((BasicBlock*)di, filter);
              //assert(filter.size() > 0 && "Could not find peer BB!\n");
           }
           else 
              findBBAtDistance(rd->i, BasicBlockDist, filter);
           llvm::errs() << "filter set:\n";
           for(auto bi : filter) {
              llvm::BasicBlock *bb = (BasicBlock*)bi;
              llvm::errs() << "1st inst in bb (" << bb << ") " << &(*(bb->begin())) << " " << (*(bb->begin())) << "\n";
           }
           for(auto bt : b) {
              for(auto ft : filter) {
                 if (sameBB((BasicBlock*)bt,(BasicBlock*)ft))
                    temp.insert(bt);
              }
           } 
           llvm::errs() << " other set of BBs after filtering:\n";
           for(auto bi : temp) {
              llvm::BasicBlock *bb = (BasicBlock*)bi;
              llvm::errs() << "1st inst in bb (" << bb << ") " << (*(bb->begin())) << "\n";
           }  
           findVirtualAddresses(temp, bset);       
        }
        else 
           findVirtualAddresses(b, bset);
        double accuracy; 
        if (bset.size() > 0  && checkCodeCacheSideChannel(ex, rd, aset, bset, p, accuracy)) {
           const InstructionInfo &ii = ex->kmodule->infos->getInfo(&(*((BasicBlock*)di)->begin()));
           p.first = p.first + "\n1st inst of bb in file " + ii.file + 
                            " at line " + std::to_string(ii.line) + "\n"; 
           found = true;
           diffLeakage.insert(di);
           if (accuracy > max_accuracy)
              max_accuracy = accuracy;
        }
     }
     if (found) {
        std::string mode = cacheLineMode ?  "CACHE LINE" : "BIT MASK";
        llvm::errs() << "CODE CACHE BASED SIDE CHANNEL: (MODE=" << mode << ")\n";
        rd->i->print(llvm::errs());
        llvm::errs() << "\nHCdiff : " << f << "\n";
        std::stringstream ss;
        ss << "blocks with leakage=" << diffLeakage.size() << "\n";
        const InstructionInfo &ii = ex->kmodule->infos->getInfo(rd->i);
        ss << ii.file + "\n";
        ss << "max accuracy=" << max_accuracy << "\n";
        p.first = p.first + "\n" + ss.str();
        p.second = ii.line;
        codecachelocs.insert(p);
        llvm::errs() << p.first << "\nLine=" << ii.line << "\n";
     }
     return found;
  }
  else {
     llvm::errs() << "No diff for BB!\n";
     return false; 
  }
}

unsigned int getTimingModel(std::string fname) {
  if (timingModels.find(fname) != timingModels.end()) {
     return timingModels[fname];
  }
  else return 1;
}

void printInfo(const InstructionInfo &ii) {
	if (ii.file != "") {
		llvm::errs() << "\n\t>>>> " << ii.file.c_str() << ":" << ii.line << "\n";
		//llvm::errs() << "assembly line : " << ii.assemblyLine << "\n";
	} else {
		klee_message(": (location information missing) ");
	}
}


void printviolations(Executor* ex) {
	std::vector<llvm::Instruction*>::iterator it = vil->begin();
	//std::set<llvm::Instruction*> *sv = new std::set<llvm::Instruction*>();
	for(; it != vil->end(); ++it) {
	//	unsigned int size = sv->size();
	//	sv->insert(*it);
	//	if(sv->size() == size) {
	//		continue;
	//	}
		const InstructionInfo &ii = ex->kmodule->infos->getInfo(*it);
		std::pair<std::string, int> p = std::pair<std::string, int>(ii.file, ii.line);
		unsigned int lz = locs.size();
		locs.insert(p);
		if(locs.size() == lz) {
			continue;
		}
		llvm::errs() << "\n\t=====================\n";
		std::string fname = (*it)->getParent()->getParent()->getName();
		llvm::errs() << "\tFunction : " << fname << "\n";
		//(*it)->getParent()->dump();
		printInfo(ii);
		llvm::errs() << "\t=====================\n";
	}
	//llvm::errs() << "\n>>>> Found violations at " << " : " << sv->size() << " locations.\n";
	llvm::errs() << "\n>>>> Found violations at " << " : " << locs.size() << " locations.\n";
}

// reads functions with security sensitive parameters that become inputs to the system under analysis 
// such parameters are marked as symbolic and security sensitive at the callsites of the function 
// (added to highLoc)
// Note: assumes that in all callsites the specified parameter is security sensitive
void readDynamicHighLoc() {
  std::fstream cf("configSYSRel/dynHighLoc.txt", std::fstream::in);
  if (cf.is_open()) {
     std::string line, function, args;
     std::cout << "Functions with security sensitive parameters:\n";
     while(std::getline(cf,line)) { 
       std::set<int> hargs;
       std::istringstream iss(line);
       getline(iss, function, ',');
       std::cout << "Security sensitive parameters of " << function << ":\n";
       std::string arg;
       while (getline(iss, arg, ',')) {
           int iarg = atoi(arg.c_str());
           hargs.insert(iarg);
       }
       dynamicHighLoc[function] =  hargs;
     }
     cf.close();
  }    
}

// reads functions with security insensitive parameters that become inputs to the system under analysis 
// such parameters are marked as symbolic and security insensitive at the callsites of the function 
// (added to lowLoc)
// Note: assumes that in all callsites the specified parameter is security insensitive
void readDynamicLowLoc() {
  std::fstream cf("configSYSRel/dynLowLoc.txt", std::fstream::in);
  if (cf.is_open()) {
     std::string line, function, args;
     std::cout << "Functions with regular input parameters:\n";
     while(std::getline(cf,line)) { 
       std::set<int> largs;
       std::istringstream iss(line);
       getline(iss, function, ',');
       std::cout << "Regular input parameters of " << function << ":\n";
       std::string arg;
       while (getline(iss, arg, ',')) {
           int iarg = atoi(arg.c_str());
           largs.insert(iarg);
       }
       dynamicLowLoc[function] =  largs;
     }
     cf.close();
  }    
}

void readSCAInput() {
   readConfig();
   readMemLoc();
   readDynamicLowLoc(); 
   readDynamicHighLoc(); 
}

void readConfig() {
	std::ifstream infile("configSYSRel/config.txt");
	std::string line;
	std::string umarker = "untrusted";
	while (std::getline(infile, line)) {
		std::vector<std::string> * words = processline(line, " ");
		if(words->size() > 1) {
			std::vector<std::string>::iterator rit = words->begin();
			if((*rit).compare(umarker)==0) {
				++rit;
				untrusted->push_back(*rit);
			}
		}
	}
	infile.close();
}

bool isUntrusted(std::string fname){
	bool ret = (std::find(untrusted->begin(), untrusted->end(), fname) != untrusted->end());
	return ret;
}

bool isHaltedState(ExecutionState * s) {
	bool ret = (std::find(haltedStates->begin(), haltedStates->end(), s) != haltedStates->end());
	return ret;
}

int RD::numHAncestors = 0;
int RD::numHLMixedConstraints = 0;

//
void printResourceUsage(RD* s);
unsigned rdid = 0;
std::map<int, RD*> * rdmap = new std::map<int, RD*>();
bool start = false;
typedef std::map<std::pair<exhash, exhash>, RD* > source;
source * sourcev = new source();

int updatesource(exhash exhash1, exhash exhash2, RD* rd) {
	std::pair<exhash, exhash> exh1 = std::pair<exhash, exhash>(exhash1, exhash2);
	std::pair<exhash, exhash> exh2 = std::pair<exhash, exhash>(exhash2, exhash1);
	if((sourcev->find(exh1) != sourcev->end()) | (sourcev->find(exh2) != sourcev->end())) {
		llvm::errs() << "\nShould not happen. Source already exists. Double reporting.\n";
		exit(0); // This can be commented out. Does not impact the source locations.
		return -1;
	}
	sourcev->insert(std::pair<std::pair<exhash, exhash>, RD* >(exh1, rd));
	sourcev->insert(std::pair<std::pair<exhash, exhash>, RD* >(exh2, rd));
	return 0;
}

RD* newNode(ExecutionState * s, Instruction *ii) {
    RD *r = newNode(s);
    r->i = ii;
    return r;
}

RD* newNode(ExecutionState * s) {
	RD* nrd = new RD();
	nrd->isHBr = false;
	nrd->HAset = false;
	nrd->HC = klee::ConstantExpr::create(true, 1);
	nrd->LC = klee::ConstantExpr::create(true, 1);
	nrd->ru = 0;
	nrd->finalized = 0;
	nrd->numSucc = 0;
	nrd->succ = new std::set<RD*>();
	nrd->i = s->pc->inst;
	nrd->stateid = rdid;
	rdid++;
	return nrd;
}

RD* newNode(RD* rd) {
	RD* nrd = new RD();
	nrd->isHBr = rd->isHBr;
	nrd->HAset = rd->HAset;
	nrd->HC = rd->HC;
	nrd->LC = rd->LC;
	nrd->ru = rd->ru;
	nrd->finalized = rd->finalized;
	nrd->numSucc = 0;
	nrd->succ = new std::set<RD*>();
	nrd->i = rd->i;
	nrd->stateid = rd->stateid;
	//rdid++;
	nrd->copyRd = true;
        nrd->bbs = rd->bbs;
	return nrd;
}

void printPreOrder(RD* rd, std::string indent) {
  llvm::errs() << indent << rd->stateid;// << "\n";
  if(rd->HAset) {
  	llvm::errs() << " -> HA : " << rd->HA->stateid;
  }
  llvm::errs() << "\n";
  if (rd->succ->size() == 0) {return;}
  std::string ni = indent + "-";
	for(std::set<RD*>::iterator srit = rd->succ->begin();
			srit != rd->succ->end(); ++srit) {
			printPreOrder(*srit, ni);
	}
}

exhash getHash(ref<Expr> e1, ref<Expr> e2) {
	exhash eh = exhash(e1->computeHash(), e2->computeHash());
	return eh;
}

range combineRange(range r1, range r2) {
	int min = std::min(r1.first, r2.first);
	int max = std::max(r1.second, r2.second);
	return range(min, max);
}

bool isTrue(ref<Expr> condition, Executor* ex) {
	ref<Expr> t = klee::ConstantExpr::create(true, 1);
	std::vector<ref<Expr> > v;
	v.push_back(t);
	const std::vector<ref<Expr> > & vr = v;
	ExecutionState * solverState = new ExecutionState(vr);
	bool ret = false;
	bool success = ex->solver->mayBeTrue(*solverState, condition, ret);
	if(!success) {
		llvm::errs() << "ERROR : Cannot solve for : ";
		condition->dump();
		exit(0);
	}
	//llvm::errs() << "Result : " << ret << "\n";
	return ret;
}

void printLeakage(RD* rd, Executor* ex) {
   //llvm::errs() << "\nAt printLeakage: \n";
   //llvm::errs() << "\nPrinting RU : \n";


   std::vector<exhash> ruvec;
   for(RU::iterator rit = rd->Ru->begin(); rit != rd->Ru->end(); ++rit) {
      ruvec.push_back(rit->first);
   }


   for(std::vector<exhash>::iterator rvit = ruvec.begin(); rvit != ruvec.end(); ++rvit) {
      RU::iterator rit = rd->Ru->find(*rvit);
      assert(rd->HE->find(rit->first) != rd->HE->end() && "ERROR : HCLC not found in ");
      unsigned numterm1 = rd->terminated[rit->first];
      HCLC hclc = rd->HE->find(rit->first)->second;
      exhash exhash1 = rd->HE->find(rit->first)->first;
      ref<Expr> h1 = hclc.first;
      ref<Expr> l1 = hclc.second;
      range R1 = rit->second;
      assert(rd->bbm.find(*rvit) != rd->bbm.end() && "basic block cov not recorded\n");
      std::vector<exhash>::iterator rvit2 = rvit;
      ++rvit2;

      for(; rvit2 != ruvec.end(); ++rvit2) {
	 RU::iterator rit2 = rd->Ru->find(*rvit2);
         assert(rd->HE->find(rit2->first) != rd->HE->end() && "ERROR : HCLC not found in ");
         unsigned numterm2 = rd->terminated[rit2->first];
	 HCLC hclc2 = rd->HE->find(rit2->first)->second;
	 exhash exhash2 = rd->HE->find(rit2->first)->first;
	 ref<Expr> h2 = hclc2.first;
	 bool sourcecheck = true;
	 if (sourcecheck) {
	    std::pair<exhash, exhash> exh1 = std::pair<exhash, exhash>(exhash1, exhash2);
	    std::pair<exhash, exhash> exh2 = std::pair<exhash, exhash>(exhash2, exhash1);
	    if (sourcev->find(exh1) != sourcev->end() || sourcev->find(exh2) != sourcev->end()) {
	       continue;
	    }
         }

         // check resource usage diff
         range R2 = rit2->second;
	 range r = combineRange(R1, R2);
	 int diff = r.second - r.first;

	 //if(diff > epsilon) {

         ref<Expr> h1diffh2 = AndExpr::create(h1, Expr::createIsZero(h2));
         ref<Expr> h2diffh1 = AndExpr::create(h2, Expr::createIsZero(h1));
         // check if HC's differ on some assignmen
         if (isTrue(h1diffh2,ex) || isTrue(h2diffh1,ex)) {
	    ref<Expr> l2 = hclc2.second;
	    ref<Expr> eval = AndExpr::create(l1, l2);
            llvm::errs() << "difference in HCs\n";
            // check is LC's agree on some assignment
	    if (isTrue(eval, ex)) {
               assert(rd->bbm.find(*rvit2) != rd->bbm.end() && "basic block cov not recorded\n");
               BBset bs1 = rd->bbm[*rvit];
               BBset bs2 = rd->bbm[*rvit2];
               BBset ds1, ds2;
               set_difference(bs1.begin(), bs1.end(), bs2.begin(), bs2.end(), inserter(ds1, ds1.end()));  
               set_difference(bs2.begin(), bs2.end(), bs1.begin(), bs1.end(), inserter(ds2, ds2.end()));  
               llvm::errs() << "bbset sizes " << bs1.size() << " and " << bs2.size() << "\n";
               llvm::errs() << "path1:\n";
               for(auto bsi : bs1) {
                  BasicBlock *bb = (BasicBlock*)(bsi);
                  llvm::errs() << "1st instr of " << bb << " : " << (*(bb->begin())) << "\n";
               }
               llvm::errs() << "path2:\n";
               for(auto bsi : bs2) {
                  BasicBlock *bb = (BasicBlock*)(bsi);
                  llvm::errs() << "1st instr of " << bb << " : " << (*(bb->begin())) << "\n";
               }
               if (ds1.size() > 0 || ds2.size() > 0) {
                  bool lf1 = checkCacheLeakage(ex, rd, ds1, bs2, h1diffh2);
                  bool lf2 = checkCacheLeakage(ex, rd, ds2, bs1, h2diffh1);
               }
                                   
               if ((!onlycompletedpaths || (numterm1 > 0 && numterm2 > 0)) && diff > epsilon) {
		  vc++;
		  llvm::errs() << "\n===============\nFound Violation at : ";
		  llvm::errs() << "\ndiff : " << diff << "\n";
		  Instruction * inst = rd->i;
		  const InstructionInfo &ii = ex->kmodule->infos->getInfo(inst);
                  llvm::errs() << "Instruction:\n";
                  llvm::errs() << (*inst) << "\n";
		  printInfo(ii);
		  vil->push_back(inst);
		  if (sourcecheck) updatesource(exhash1, exhash2, rd);
		     llvm::errs() << "\n[ "; h1->dump(); l1->dump(); llvm::errs() << " ] ";
		     llvm::errs() << "  ---->  [ " << R1.first << " , " << R1.second << " ]\n";
		     llvm::errs() << "\n[ "; h2->dump(); l2->dump(); llvm::errs() << " ] ";
		     llvm::errs() << "  ---->  [ " << R2.first << " , " << R2.second << " ]\n";

		     std::set<RD*>::iterator ssit = rd->succ->begin();
		     llvm::errs() << "\nSuccessors : \n";
		     for (; ssit!=rd->succ->end(); ++ssit) {
		         llvm::Instruction* inst1 = (*ssit)->i;
			 const InstructionInfo &ii1 = ex->kmodule->infos->getInfo(inst1);
			 printInfo(ii1);
			 inst1->dump();
			 std::string n = inst1->getParent()->getParent()->getName();
			 llvm::errs() << "\n" << n << "\n";
		     }
		     llvm::errs() << "\nPredecessor : \n";
		     bool haspred = rd->HAset;
		     RD* rdi = rd;
		     if (haspred) {
			rdi = rd->HA;
		        haspred = rdi->HAset;
			if(rdi == rd) break;
			const InstructionInfo &ii1 = ex->kmodule->infos->getInfo(rdi->i);
			printInfo(ii1);
		     }
		     llvm::errs() << "\n===============\n";
                  }
	       }
	    }
	  }
	}
}

void checkLeakage(RD* rd, Executor* ex) {
	for(std::set<RD*>::iterator srit = rd->succ->begin();
			srit != rd->succ->end(); ++srit) {
	    checkLeakage(*srit, ex);
	}
        //llvm::errs() << "RU size=" << rd->Ru->size()  << " " << (*rd->i) << "\n";
	if(rd->Ru->size() > 1){
	   printLeakage(rd, ex);
	}
}

unsigned propagate(RD* rd, std::string indent, Executor* ex) {
	std::string ni = indent + "-";
	for(std::set<RD*>::iterator srit = rd->succ->begin();
			srit != rd->succ->end(); ++srit) {
			rd->finalized++; // TDOD: keep a lookout for this if multi-threaded
                        //llvm::errs() << (*srit)->stateid << "->" << rd->stateid << "\n";
                        //llvm::errs() << (*srit)->stateid << "isHBr? " << (*srit)->isHBr << "\n";
                        //BBset bs = (*srit)->bbs;
                        //for(auto bsi : bs) {
                        //   BasicBlock *bb = (BasicBlock*)(bsi);
                        //   llvm::errs() << "1st instr of " << bb << " : " << (*(bb->begin())) << "\n";
                        //}
			propagate(*srit, ni, ex);
	}

	if(rd->succ->size() > 0) {
		return 0; //Only let leaf nodes perform the following operations
	}

        // we will keep propagating and deal with termination in printLeakage
	//if(onlycompletedpaths){
	//	if(!rd->pathterminated){
	//		return 0;
	//	}
	//}


	//llvm::errs() << indent << rd->stateid;// << "\n";
	//if(rd->HAset) {
	while(rd->HAset) {
		//printResourceUsage(rd);
		//llvm::errs() << " -> HA : " << rd->HA->stateid << "\n";
		RD* a = rd->HA;
		exhash eh = getHash(rd->HC, rd->LC);
		range R = range(std::numeric_limits<int>::max(), -std::numeric_limits<int>::max());
		if(a->Ru->find(eh) != a->Ru->end()) {
			R = a->Ru->find(eh)->second;
		}
                BBset bbs;
                if (a->bbm.find(eh) != a->bbm.end())
                   bbs = a->bbm[eh];

		if(!rd->isHBr) {
			range rr = combineRange(R, range(rd->ru, rd->ru));
			a->Ru->insert(std::pair<exhash, range>(eh, rr));
			a->HE->insert(std::pair<exhash, HCLC>(eh, HCLC(rd->HC, rd->LC)));
			if(!rd->isvoid) {
				a->lrets->insert(std::pair<exhash, ref<Expr> >(eh, rd->retval));
			}
			for(HashRet::iterator hit = rd->lrets->begin(); hit != rd->lrets->end(); ++hit) {
				a->lrets->insert(std::pair<exhash, ref<Expr> >(hit->first, hit->second));
			}
                        for(auto bsi : rd->bbs)
                           bbs.insert(bsi);
                        a->bbm[eh] = bbs;
                        if (rd->pathterminated) {
                           unsigned numterm = 0;
                           if (a->terminated.find(eh) != a->terminated.end())
                              numterm = a->terminated[eh];
                           a->terminated[eh] = numterm + 1;
                        }
		}
		else {
			RU::iterator rit = rd->Ru->begin();
			range Rs;
			for(; rit != rd->Ru->end(); ++rit) {
				if(rd->HE->find(rit->first) == rd->HE->end()) {
					llvm::errs() << "ERROR : HCLC not found in " << rd->stateid << "\n";
					assert(0);
					exit(0);
				}
				Rs = rit->second;
				HCLC hl1 = rd->HE->find(rit->first)->second;
				ref<Expr> hc = hl1.first;
				ref<Expr> lc = hl1.second;
				ref<Expr> HC = AndExpr::create(rd->HC, hc);
				ref<Expr> LC = AndExpr::create(rd->LC, lc);
				exhash eh2 = getHash(HC, LC);
				//HC.get()->dump();
				range rr = combineRange(R, Rs);
				a->Ru->insert(std::pair<exhash, range>(eh2, rr));
				a->HE->insert(std::pair<exhash, HCLC>(eh2, HCLC(HC, LC)));
                                unsigned numterm = 0;
                                if (a->terminated.find(eh2) != a->terminated.end())
                                   numterm = a->terminated[eh2];
                                a->terminated[eh2] = numterm + rd->terminated[rit->first];
                                assert(rd->bbm.find(rit->first) != rd->bbm.end() && "Could not find relevant basic block cov info!\n");
                                for(auto bsi : rd->bbm[rit->first])
                                   bbs.insert(bsi);
                                a->bbm.insert(std::pair<exhash, BBset>(eh2,bbs));
				//if(!rd->isvoid) {
				//	a->lrets->insert(std::pair<exhash, ref<Expr> >(eh2, rd->retval));
				//}
				for(HashRet::iterator hit = rd->lrets->begin(); hit != rd->lrets->end(); ++hit) {
					a->lrets->insert(std::pair<exhash, ref<Expr> >(hit->first, hit->second));
				}
				//rd->retval->dump();
			}
		}
		rd = a;

	}
	//}
	//llvm::errs() << indent << rd->stateid << " : ru : " << rd->ru << "\n";
	return rd->ru;
}

void rdmapinsert(int i, RD* rd){
	std::pair<int, RD* > p = std::pair<int, RD* > (i, rd);
	rdmap->insert(p);
}

RD *rdmapreplace(int i, RD* rd){
     RD *rdd = NULL;
     std::map<int, RD*>::iterator it = rdmap->find(i); 
     if (rdmap->find(i) != rdmap->end()) {
        rdd = it->second;
     }
     std::pair<int, RD* > p = std::pair<int, RD* > (i, rd);
     rdmap->insert(p);
     return rdd;
}

RD* getrdmap(ExecutionState * s) {
	if(s->rdid == -1) {
		llvm::errs() << "\nERROR : Unlinked state to RD node.\n";
		llvm::errs() << "\nERROR: Should not happen\n";
		llvm::errs() << "\nrdid : " << s->rdid << "\n";
		//llvm::errs() << "\nstate id : " << s->getID() << "\n";
    	ExprPPrinter::printConstraints(llvm::outs(), s->constraints);
    	klee::ConstraintManager::constraint_iterator sit = s->constraints.begin();
		if(s->constraints.size() > 0) {
			sit = s->constraints.end();
			--sit;
			ref<Expr> r1 = *sit;
			r1->dump();
		}
		exit(0);
	}
	RD* rd;
	std::map<int, RD*>::iterator it = rdmap->find(s->rdid);
        #ifdef VBSC
	llvm::errs() << "\nrdid : " << s->rdid;
       #endif
	//llvm::errs() << "\nstate id : " << s->getID();
	if(it != rdmap->end()) {
		rd = it->second;
		return rd;
	}
	else {
		llvm::errs() << "\nERROR: Should not happen\n";
		//llvm::errs() << "\nrdid : " << s->rdid << "\n";
		//llvm::errs() << "\nstate id : " << s->getID() << "\n";
    ExprPPrinter::printConstraints(llvm::outs(), s->constraints);
    klee::ConstraintManager::constraint_iterator sit = s->constraints.begin();
		if(s->constraints.size() > 0) {
			sit = s->constraints.end();
			--sit;
			ref<Expr> r1 = *sit;
			r1->dump();
		}
		exit(0);
		rd->valid = false;
	}
	return rd;
}

void printResourceUsage(RD * s) {
	llvm::errs() << ">>>> Printing Resour usage for state : " << s->stateid << "\n";
	if(s->HAset) {
		llvm::errs() << "\tHA : " << s->HA->stateid << "\n";
	}
	else{
		llvm::errs() << "\tHA : DOES NOT HAVE HA\n";
	}
	llvm::errs() << "\tisHBR : " << s->isHBr << "\n";
	llvm::errs() << "\tHC : "; s->HC->dump();
	llvm::errs() << "\tLC : "; s->LC->dump();
	llvm::errs() << "\tru : " << s->ru << "\n";
	llvm::errs() << "\tRU : " << s->Ru->size() << "\n";
}


void printRDtree(RD* rd, std::string indent) {
  	llvm::errs() << indent << "\n";
	printResourceUsage(rd);
  	std::string ni = indent + "-";
	for(std::set<RD*>::iterator srit = rd->succ->begin();
			srit != rd->succ->end(); ++srit) {
			printRDtree(*srit, ni);
	}
}


void initStateRes(RD * s) {
	s->isHBr = false;
	s->HAset = false;
	s->HC = klee::ConstantExpr::create(true, 1);
	s->LC = klee::ConstantExpr::create(true, 1);
	s->ru = 0;
}
