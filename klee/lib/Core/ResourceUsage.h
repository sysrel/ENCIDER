#include <map>
#include <algorithm>

#define ICost 1
#define epsilon 0

bool onlycompletedpaths = true;
typedef std::pair<ref<Expr>, ref<Expr> > HCLC;
typedef std::pair<int, int> range;
typedef std::pair<unsigned, unsigned> exhash;
typedef std::map<exhash, range> RU;
typedef std::map<exhash, HCLC> HashExpr;
typedef std::map<exhash, ref<Expr> > HashRet;

//Elclave functions
std::vector<std::string> * untrusted = new std::vector<std::string>();
std::vector<ExecutionState*> * haltedStates = new std::vector<ExecutionState*>();
int vc = 0;
std::vector<llvm::Instruction*> *vil = new std::vector<llvm::Instruction*>();

void printInfo(const InstructionInfo &ii) {
	if (ii.file != "") {
		std::cerr << "\n\t>>>> " << ii.file.c_str() << ":" << ii.line << "\n";
		//std::cerr << "assembly line : " << ii.assemblyLine << "\n";
	} else {
		klee_message(": (location information missing) ");
	}
}


void printviolations(Executor* ex) {
	std::vector<llvm::Instruction*>::iterator it = vil->begin();
	//std::set<llvm::Instruction*> *sv = new std::set<llvm::Instruction*>();
	std::set<std::pair<std::string, int>> *locs = new std::set<std::pair<std::string, int>>();
	for(; it != vil->end(); ++it) {
	//	unsigned int size = sv->size();
	//	sv->insert(*it);
	//	if(sv->size() == size) {
	//		continue;
	//	}
		const InstructionInfo &ii = ex->kmodule->infos->getInfo(*it);
		std::pair<std::string, int> p = std::pair<std::string, int>(ii.file, ii.line);
		unsigned int lz = locs->size();
		locs->insert(p);
		if(locs->size() == lz) {
			continue;
		}
		std::cerr << "\n\t=====================\n";
		std::string fname = (*it)->getParent()->getParent()->getName();
		std::cerr << "\tFunction : " << fname << "\n";
		//(*it)->getParent()->dump();
		printInfo(ii);
		std::cerr << "\t=====================\n";
	}
	//std::cerr << "\n>>>> Found violations at " << std::dec << sv->size() << " locations.\n";
	std::cerr << "\n>>>> Found violations at " << std::dec << locs->size() << " locations.\n";
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

class RD {
	public:
		bool isHBr;// = false;
		RD* HA;
		bool HAset;// = false;
		// Always set HAset to true when a state's HA is found
		ref<Expr> HC;// = klee::ConstantExpr::create(true, 1);
		ref<Expr> LC;// = klee::ConstantExpr::create(true, 1);
		unsigned ru = 0;
		unsigned finalized = 0;
		unsigned numSucc;// = 0;
		std::set<RD*> * succ;// = new std::set<RD*>();
		Instruction * i;
		unsigned stateid;// = 0;
		RU* Ru = new RU(); // MAP of (hash of) expressions to range
		bool Ruset = false;
		HashExpr* HE = new HashExpr();  // Map of hash of expression to expression
		bool copyRd = false;
		//InstructionInfo *info;
		bool valid = true;
		ref<Expr> retval; // For leaf nodes
		bool isvoid = true; // For leaf nodes
		// For H-ancestors
		HashRet* lrets = new HashRet();  // Map of hash of expression to expression. Stores the return values of each path. Since returns are public.
		bool pathterminated = false; // For leaf nodes keeps track of path termination info.
};
//
void printResourceUsage(RD* s);
unsigned rdid = 0;
RD* root;
RD* currentRD;
std::map<int, RD*> * rdmap = new std::map<int, RD*>();
bool start = false;
typedef std::map<std::pair<exhash, exhash>, RD* > source;
source * sourcev = new source();

int updatesource(exhash exhash1, exhash exhash2, RD* rd) {
	std::pair<exhash, exhash> exh1 = std::pair<exhash, exhash>(exhash1, exhash2);
	std::pair<exhash, exhash> exh2 = std::pair<exhash, exhash>(exhash2, exhash1);
	if((sourcev->find(exh1) != sourcev->end()) | (sourcev->find(exh2) != sourcev->end())) {
		std::cerr << "\nShould not happen. Source already exists. Double reporting.\n";
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
	rdid++;
	nrd->copyRd = true;
	return nrd;
}

void printPreOrder(RD* rd, std::string indent) {
  std::cerr << indent << rd->stateid;// << "\n";
  if(rd->HAset) {
  	std::cerr << " -> HA : " << rd->HA->stateid;
  }
  std::cerr << "\n";
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
		std::cerr << "ERROR : Cannot solve for : ";
		condition->dump();
		exit(0);
	}
	//std::cerr << "Result : " << ret << "\n";
	return ret;
}

void printLeakage(RD* rd, Executor* ex) {
	//std::cerr << "\nAt printLeakage: \n";
	//std::cerr << "\nPrinting RU : \n";
	std::vector<exhash> ruvec;
	for(RU::iterator rit = rd->Ru->begin(); rit != rd->Ru->end(); ++rit) {
		ruvec.push_back(rit->first);
	}

	//for(RU::iterator rit = rd->Ru->begin(); rit != rd->Ru->end(); ++rit) {
	for(std::vector<exhash>::iterator rvit = ruvec.begin(); rvit != ruvec.end(); ++rvit) {
		RU::iterator rit = rd->Ru->find(*rvit);
		if(rd->HE->find(rit->first) == rd->HE->end()) {
			std::cerr << "ERROR : HCLC not found in " << rd->stateid << "\n";
			assert(0);
			exit(0);
		}
		HCLC hclc = rd->HE->find(rit->first)->second;
		exhash exhash1 = rd->HE->find(rit->first)->first;
		ref<Expr> h1 = hclc.first;
		ref<Expr> l1 = hclc.second;
		range R1 = rit->second;

		/* Return value check */
		bool checkret = (rd->lrets->find(rit->first) != rd->lrets->end());
		ref<Expr> rv1 = klee::ConstantExpr::create(true, 1);
		if(checkret) {
			rv1 = rd->lrets->find(rit->first)->second;
		}
		/* Return value check end */

		//std::cerr << "\n[ "; h1->dump(); l1->dump(); std::cerr << " ] ";
		//std::cerr << "  ---->  [ " << R1.first << " , " << R1.second << " ]\n";
		//std::set<exhash> * done = new std::set<exhash>();
		//for(RU::iterator rit2 = rd->Ru->begin(); rit2 != rd->Ru->end(); ++rit2) {
		std::vector<exhash>::iterator rvit2 = rvit;
		++rvit2;
		for(; rvit2 != ruvec.end(); ++rvit2) {
			RU::iterator rit2 = rd->Ru->find(*rvit2);
			if(rd->HE->find(rit2->first) == rd->HE->end()) {
				std::cerr << "ERROR : HCLC not found in " << rd->stateid << "\n";
				assert(0);
				exit(0);
			}
			HCLC hclc2 = rd->HE->find(rit2->first)->second;
			exhash exhash2 = rd->HE->find(rit2->first)->first;
			ref<Expr> h2 = hclc2.first;

			//std::cerr << "\nh1 : "; h1->dump();
			//std::cerr << "\nh2 : "; (*h2).dump();

			//Source check
			bool sourcecheck = true;
			if(sourcecheck) {
				std::pair<exhash, exhash> exh1 = std::pair<exhash, exhash>(exhash1, exhash2);
				if((sourcev->find(exh1) != sourcev->end())) {
					continue;
				}
			}

			if(h1->compare(*(h2))) {
				/* Return value check */
				bool checkret2 = (rd->lrets->find(rit2->first) != rd->lrets->end());
				//std::cerr << "\ncheckret2 : " << checkret2 << "\n";
				//std::cerr << "lrets size = " << std::dec << rd->lrets->size() << "\n";
				ref<Expr> rv2 = klee::ConstantExpr::create(true, 1);

				if(checkret & checkret2) {
					rv2 = rd->lrets->find(rit2->first)->second;
					//std::cerr << "\nrv1 : "; rv1->dump();
					//std::cerr << "\nrv2 : "; rv2->dump();
					ref<Expr> eval = EqExpr::create(rv1, rv2);
					//std::cerr << "\neval : \n"; eval->dump();
					bool commonsolutionavailable = isTrue(eval, ex);
					//exit(0);
					if(!commonsolutionavailable) {
						continue;
					}
				}/**/
				/* Return value check */

				ref<Expr> l2 = hclc2.second;
				ref<Expr> eval = AndExpr::create(l1, l2);
				//std::cerr << "\neval : "; eval->dump();
				if(isTrue(eval, ex)) {
					range R2 = rit2->second;
					range r = combineRange(R1, R2);
					int diff = r.second - r.first;
					if(diff > epsilon) {
						vc++;
						std::cerr << "\n===============\nFound Violation at : ";
						std::cerr << "\ndiff : " << diff << "\n";
						/*
						std::cerr << "\nh1: \n"; h1->dump();
						std::cerr << "\nval1: \n"; rv1->dump();
						std::cerr << "\nh2: \n"; h2->dump();
						std::cerr << "\nval2: \n"; rv2->dump();
						*/

						//exit(0);
						//rd->i->getParent()->dump();
						//Instruction * inst = (*(rd->succ->begin()))->i;
						Instruction * inst = rd->i;
						const InstructionInfo &ii = ex->kmodule->infos->getInfo(inst);
						//inst->getParent()->dump();
						printInfo(ii);
						vil->push_back(inst);
						if(sourcecheck) updatesource(exhash1, exhash2, rd);
						//std::cerr << "\nIn Function : " << rd->i->getParent()->getParent()->getName();
						//printResourceUsage(rd);
						std::cerr << "\n[ "; h1->dump(); l1->dump(); std::cerr << " ] ";
						std::cerr << "  ---->  [ " << R1.first << " , " << R1.second << " ]\n";
						std::cerr << "\n[ "; h2->dump(); l2->dump(); std::cerr << " ] ";
						std::cerr << "  ---->  [ " << R2.first << " , " << R2.second << " ]\n";

						std::set<RD*>::iterator ssit = rd->succ->begin();
						std::cerr << "\nSuccessors : \n";
						for(; ssit!=rd->succ->end(); ++ssit) {
							llvm::Instruction* inst1 = (*ssit)->i;
							const InstructionInfo &ii1 = ex->kmodule->infos->getInfo(inst1);
							printInfo(ii1);
							inst1->dump();
							std::string n = inst1->getParent()->getParent()->getName();
							std::cerr << "\n" << n << "\n";
						}
						std::cerr << "\nPredecessor : \n";
						bool haspred = rd->HAset;
						RD* rdi = rd;
						if(haspred) {
							rdi = rd->HA;
							haspred = rdi->HAset;
							if(rdi == rd) break;
							const InstructionInfo &ii1 = ex->kmodule->infos->getInfo(rdi->i);
							printInfo(ii1);
						}
						std::cerr << "\n===============\n";
						//exit(0);
					}
				}
			}
		}
	}
	//std::cerr << "\n : Printing RU end : \n";
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
			rd->finalized++; // TDOD: keep a lookput for this if multi-threaded
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
	//std::cerr << indent << rd->stateid;// << "\n";
	//if(rd->HAset) {
	while(rd->HAset) {
		//printResourceUsage(rd);
		//std::cerr << " -> HA : " << rd->HA->stateid << "\n";
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
					std::cerr << "ERROR : HCLC not found in " << rd->stateid << "\n";
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
	//std::cerr << indent << rd->stateid << " : ru : " << rd->ru << "\n";
	return rd->ru;
}

void rdmapinsert(int i, RD* rd){
	std::pair<int, RD* > p = std::pair<int, RD* > (i, rd);
	rdmap->insert(p);
}

RD* getrdmap(ExecutionState * s) {
	if(s->rdid == -1) {
		std::cerr << "\nERROR : Unlinked state to RD node.\n";
		std::cerr << "\nERROR: Should not happen\n";
		std::cerr << "\nrdid : " << s->rdid << "\n";
		//std::cerr << "\nstate id : " << s->getID() << "\n";
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
	std::cerr << "\nrdid : " << s->rdid;
	//std::cerr << "\nstate id : " << s->getID();
	if(it != rdmap->end()) {
		rd = it->second;
		return rd;
	}
	else {
		std::cerr << "\nERROR: Should not happen\n";
		//std::cerr << "\nrdid : " << s->rdid << "\n";
		//std::cerr << "\nstate id : " << s->getID() << "\n";
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
	std::cerr << ">>>> Printing Resour usage for state : " << s->stateid << "\n";
	if(s->HAset) {
		std::cerr << "\tHA : " << s->HA->stateid << "\n";
	}
	else{
		std::cerr << "\tHA : DOES NOT HAVE HA\n";
	}
	std::cerr << "\tisHBR : " << s->isHBr << "\n";
	std::cerr << "\tHC : "; s->HC->dump();
	std::cerr << "\tLC : "; s->LC->dump();
	std::cerr << "\tru : " << s->ru << "\n";
	std::cerr << "\tRU : " << s->Ru->size() << "\n";
}


void printRDtree(RD* rd, std::string indent) {
  	std::cerr << indent << "\n";
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
