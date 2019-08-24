#include <map>
#include <algorithm>
#include "AnalyzeConstraint.h"
#include "ResourceUsage.h"
#include "klee/Internal/Support/ErrorHandling.h"
#include "TimingSolver.h"
#include "klee/util/ExprPPrinter.h"
#include <fstream>
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

//Enclave functions
std::vector<std::string> * untrusted = new std::vector<std::string>();
std::vector<ExecutionState*> * haltedStates = new std::vector<ExecutionState*>();
int vc = 0;
std::vector<llvm::Instruction*> *vil = new std::vector<llvm::Instruction*>();
std::map<std::string, std::set<int> > dynamicHighLoc;
std::map<std::string, std::set<int> > dynamicLowLoc;

std::map<std::string, unsigned int> timingModels;

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

	//for(RU::iterator rit = rd->Ru->begin(); rit != rd->Ru->end(); ++rit) {
	for(std::vector<exhash>::iterator rvit = ruvec.begin(); rvit != ruvec.end(); ++rvit) {
		RU::iterator rit = rd->Ru->find(*rvit);
		if(rd->HE->find(rit->first) == rd->HE->end()) {
			llvm::errs() << "ERROR : HCLC not found in " << rd->stateid << "\n";
			assert(0);
			exit(0);
		}
		HCLC hclc = rd->HE->find(rit->first)->second;
		exhash exhash1 = rd->HE->find(rit->first)->first;
		ref<Expr> h1 = hclc.first;
		ref<Expr> l1 = hclc.second;
		range R1 = rit->second;

		/* Return value check */
                /*
		bool checkret = (rd->lrets->find(rit->first) != rd->lrets->end());
		ref<Expr> rv1 = klee::ConstantExpr::create(true, 1);
		if(checkret) {
			rv1 = rd->lrets->find(rit->first)->second;
		}*/
		/* Return value check end */

		//llvm::errs() << "\n[ "; h1->dump(); l1->dump(); llvm::errs() << " ] ";
		//llvm::errs() << "  ---->  [ " << R1.first << " , " << R1.second << " ]\n";
		//std::set<exhash> * done = new std::set<exhash>();
		//for(RU::iterator rit2 = rd->Ru->begin(); rit2 != rd->Ru->end(); ++rit2) {
		std::vector<exhash>::iterator rvit2 = rvit;
		++rvit2;
		for(; rvit2 != ruvec.end(); ++rvit2) {
			RU::iterator rit2 = rd->Ru->find(*rvit2);
			if(rd->HE->find(rit2->first) == rd->HE->end()) {
				llvm::errs() << "ERROR : HCLC not found in " << rd->stateid << "\n";
				assert(0);
				exit(0);
			}
			HCLC hclc2 = rd->HE->find(rit2->first)->second;
			exhash exhash2 = rd->HE->find(rit2->first)->first;
			ref<Expr> h2 = hclc2.first;

			//llvm::errs() << "\nh1 : "; h1->dump();
			//llvm::errs() << "\nh2 : "; (*h2).dump();

			//Source check
			bool sourcecheck = true;
			if(sourcecheck) {
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
			if(diff > epsilon) {

                           ref<Expr> h1diffh2 = AndExpr::create(h1, Expr::createIsZero(h2));
                           ref<Expr> h2diffh1 = AndExpr::create(h2, Expr::createIsZero(h1));
                           // check if HC's differ on some assignment
			   if(isTrue(h1diffh2,ex) || isTrue(h2diffh1,ex)) {
				/* Return value check */
                                /*
				bool checkret2 = (rd->lrets->find(rit2->first) != rd->lrets->end());
				//llvm::errs() << "\ncheckret2 : " << checkret2 << "\n";
				//llvm::errs() << "lrets size = " << " : " << rd->lrets->size() << "\n";
				ref<Expr> rv2 = klee::ConstantExpr::create(true, 1);

				if(checkret & checkret2) {
					rv2 = rd->lrets->find(rit2->first)->second;
					//llvm::errs() << "\nrv1 : "; rv1->dump();
					//llvm::errs() << "\nrv2 : "; rv2->dump();
					ref<Expr> eval = EqExpr::create(rv1, rv2);
					//llvm::errs() << "\neval : \n"; eval->dump();
					bool commonsolutionavailable = isTrue(eval, ex);
					//exit(0);
					if(!commonsolutionavailable) {
						continue;
					}
				}*/
				/* Return value check */

				ref<Expr> l2 = hclc2.second;
				ref<Expr> eval = AndExpr::create(l1, l2);
				//llvm::errs() << "\neval : "; eval->dump();
                                // check is LC's agree on some assignment
				if(isTrue(eval, ex)) {
						vc++;
						llvm::errs() << "\n===============\nFound Violation at : ";
						llvm::errs() << "\ndiff : " << diff << "\n";
						/*
						llvm::errs() << "\nh1: \n"; h1->dump();
						llvm::errs() << "\nval1: \n"; rv1->dump();
						llvm::errs() << "\nh2: \n"; h2->dump();
						llvm::errs() << "\nval2: \n"; rv2->dump();
						*/

						//exit(0);
						//rd->i->getParent()->dump();
						//Instruction * inst = (*(rd->succ->begin()))->i;
						Instruction * inst = rd->i;
						const InstructionInfo &ii = ex->kmodule->infos->getInfo(inst);
                                                llvm::errs() << "Instruction:\n";
                                                llvm::errs() << (*inst) << "\n";
						//inst->getParent()->dump();
						printInfo(ii);
						vil->push_back(inst);
						if(sourcecheck) updatesource(exhash1, exhash2, rd);
						//llvm::errs() << "\nIn Function : " << rd->i->getParent()->getParent()->getName();
						//printResourceUsage(rd);
						llvm::errs() << "\n[ "; h1->dump(); l1->dump(); llvm::errs() << " ] ";
						llvm::errs() << "  ---->  [ " << R1.first << " , " << R1.second << " ]\n";
						llvm::errs() << "\n[ "; h2->dump(); l2->dump(); llvm::errs() << " ] ";
						llvm::errs() << "  ---->  [ " << R2.first << " , " << R2.second << " ]\n";

						std::set<RD*>::iterator ssit = rd->succ->begin();
						llvm::errs() << "\nSuccessors : \n";
						for(; ssit!=rd->succ->end(); ++ssit) {
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
						if(haspred) {
							rdi = rd->HA;
							haspred = rdi->HAset;
							if(rdi == rd) break;
							const InstructionInfo &ii1 = ex->kmodule->infos->getInfo(rdi->i);
							printInfo(ii1);
						}
						llvm::errs() << "\n===============\n";
						//exit(0);
					}
				}
			}
		}
	}
	//llvm::errs() << "\n : Printing RU end : \n";
}

void checkLeakage(RD* rd, Executor* ex) {
	for(std::set<RD*>::iterator srit = rd->succ->begin();
			srit != rd->succ->end(); ++srit) {
		checkLeakage(*srit, ex);
	}
	if(rd->Ru->size() > 1){
		printLeakage(rd, ex);
	}
}

unsigned propagate(RD* rd, std::string indent, Executor* ex) {
	std::string ni = indent + "-";
	for(std::set<RD*>::iterator srit = rd->succ->begin();
			srit != rd->succ->end(); ++srit) {
			rd->finalized++; // TDOD: keep a lookout for this if multi-threaded
			propagate(*srit, ni, ex);
	}

	if(rd->succ->size() > 0) {
		return 0; //Only let leaf nodes perform the following operations
	}

	if(onlycompletedpaths){
		if(!rd->pathterminated){
			return 0;
		}
	}
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
