/*
#include "klee/Expr.h"
#include "klee/util/ExprPPrinter.h"
#include "klee/util/ExprSMTLIBPrinter.h"
#include "klee/util/ExprUtil.h"
#include "klee/ExecutionState.h"
using namespace llvm;
using namespace klee;
*/
#include <iostream>

#define dbp 0

/* Variables based on sensivity */
std::set<std::string> * highLoc = new std::set<std::string>();
std::set<std::string> * lowLoc = new std::set<std::string>();
Function *ff;
bool fset = false;
/* Variables based on sensivity End */

std::set<std::string>* getNameofAddressConstraintSet(klee::ref<Expr> cexpr);
std::vector<std::string> * processline(std::string s, std::string delimiter);
void dumpKids(klee::ref<Expr>& cexpr);
bool replace(std::string& str, const std::string& from, const std::string& to);
void removeBrackets(std::string& str);
ref<Expr> buildProjection(ref<Expr>& cexpr);

void readMemLoc() {
	std::ifstream infile("configSYSRel/highloc.txt");
	std::string line;
	std::string umarker = "high";
	while (std::getline(infile, line)) {
		std::vector<std::string> * words = processline(line, " ");
		if(words->size() > 1) {
			std::vector<std::string>::iterator rit = words->begin();
			if((*rit).compare(umarker)==0) {
				++rit;
				highLoc->insert(*rit);
			}
		}
	}
	infile.close();
	
	std::ifstream infile2("configSYSRel/lowloc.txt");
	std::string line2;
	std::string umarker2 = "low";
	while (std::getline(infile2, line2)) {
		std::vector<std::string> * words = processline(line2, " ");
		if(words->size() > 1) {
			std::vector<std::string>::iterator rit = words->begin();
			if((*rit).compare(umarker2)==0) {
				++rit;
				lowLoc->insert(*rit);
			}
		}
	}
	infile2.close();
}


bool exprHasVar(klee::ref<Expr> expr, std::set<std::string> * varset) {
  std::set<std::string>* names = getNameofAddressConstraintSet(expr);
  std::set<std::string>::iterator it;
  #if dbp
    std::cerr << "\n Variables in expression : \n";
  	for(it = names->begin(); it!=names->end(); it++) {
  		std::cerr << *it << " ";
  	}
  	std::cerr << "\n";
  #endif
	for(std::set<std::string>::iterator sit = varset->begin();
			sit != varset->end(); ++sit) {
		std::string var = *sit;
		it = names->find(var);
	  if(it != names->end()) {
	  	#if dbp
	  		std::cerr << "Found " << var << "\n";
	  	#endif
	  	return true;
	  }
	  #if dbp
			std::cerr << "NotFound " << var << "\n";
		#endif
  }
  return false;
}

std::string getString(ref<Expr> e){
	std::string expr_str;
	llvm::raw_string_ostream rso(expr_str);
	e->print(rso);
	std::string s = rso.str();
	return s;
}

std::set<std::string>* getNameofAddressConstraintSet(klee::ref<Expr> cexpr) {
  std::set<std::string>* names = new std::set<std::string>();
  std::vector< ref<ReadExpr> > results;
  #if dbp
		std::cerr << "\n : findReads : \n";
	#endif
  findReads(cexpr, false, results);
	std::string s;
  for(std::vector<ref<ReadExpr> >::iterator rit = results.begin();
      rit != results.end(); ++rit) {
    ref<Expr> e = *rit;
		std::string s = getString(e);
		removeBrackets(s);
		#if dbp
			std::cout<<"ReadExpr : " << s << "\n";
		#endif
		std::vector<std::string> * words = processline(s, " ");
		if(words->size() > 2) {
		  std::vector<std::string>::iterator rit = words->end();
		  --rit;
		  std::string n = *rit;
			names->insert(n);
		}
  }
  #if dbp
		std::cerr << " : findReads : end\n";
  #endif
  return names;
}

void dumpKids(klee::ref<Expr>& cexpr){
	int kidsn = cexpr->getNumKids();
	std::cerr << "There are " << kidsn << " kids\n";
	if(kidsn > 0) {
		for(int i=0; i<kidsn; ++i) {
		  std::cerr << "kid " << i << " : ";
			cexpr->getKid(i)->dump();
		}
	}
}

void dumpKidsRec(klee::ref<Expr>& cexpr) {
	int kidsn = cexpr->getNumKids();
	std::cerr << "There are " << kidsn << " kids\n";
	if (kidsn > 0) {
		for(int i=0; i<kidsn; ++i) {
		  std::cerr << "kid " << i << " : ";
			cexpr->getKid(i)->dump();
			ref<Expr> r = cexpr->getKid(i);
			int kidsn2 = r->getNumKids();
			if(r->getKind()!= Expr::Read) {
				if(kidsn2 > 0) {
					dumpKidsRec(r);
				}
			}
		}
	}
}

/* 
 * Assumption for getProjection and buildProjection is that
 * cexpr has atleast 1 highloc 
 */
ref<Expr> getProjection(ref<Expr>& cexpr, std::set<std::string> * varset) {
	//std::set<std::string>* names = getNameofAddressConstraintSet(cexpr);
	ref<Expr> e = buildProjection(cexpr);
	return e;	
}

ref<Expr> buildProjection(ref<Expr>& cexpr) {
	//std::cerr << ">>>> buildProjection Gets called\n";
  int size = cexpr->getNumKids();
  ref<Expr> kid[size];
  for(int i=0; i<size; ++i){
      kid[i] = cexpr->getKid(i);
  }
  //cexpr->dump();
  //std::cerr << "Size : " << size << " Kind : " << cexpr->getKind() << "\n";
  switch (cexpr->getKind()) {
    case Expr::Not: return NotExpr::create(cexpr);
    case Expr::ZExt: {
        return ZExtExpr::create(buildProjection(kid[0]), cexpr->getWidth());
    }
    case Expr::SExt: return SExtExpr::create(buildProjection(kid[0]), cexpr->getWidth());
    case Expr::NotOptimized: {
            if(size > 0) { return NotOptimizedExpr::create(buildProjection(kid[0])); }
            else return NotOptimizedExpr::create(cexpr);
        }
    case Expr::Constant: {
        klee::ConstantExpr *CE = dyn_cast<klee::ConstantExpr>(cexpr);
        return klee::ConstantExpr::create(CE->getZExtValue(), CE->getWidth());
    }
    case Expr::Read: {
        //std::cerr << "Size : " << cexpr->getWidth() << "\n";
        //cexpr->dump();
        return cexpr;
        //exit(0);
        break;
    }
    case Expr::Concat: return ConcatExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::Extract: {
        klee::ConstantExpr *CE = dyn_cast<klee::ConstantExpr>(kid[1]);
        //klee::ConstantExpr *DE = dyn_cast<klee::ConstantExpr>(kid[2]);
        return ExtractExpr::create(kid[0], CE->getZExtValue(), cexpr->getWidth());
    }
    case Expr::Select: return SelectExpr::create(buildProjection(kid[0]), buildProjection(kid[1]), 
                                                 buildProjection(kid[2]));
    case Expr::Add: return AddExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::Sub: return SubExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::Mul: return MulExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::UDiv: return UDivExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::SDiv: return SDivExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::URem: return URemExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::SRem: return SRemExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::AShr: return AShrExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::LShr: return LShrExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::Shl: return ShlExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    
    case Expr::And:  
    case Expr::Or:
    case Expr::Xor:
    {
    	bool k0 = exprHasVar(kid[0], highLoc);
    	bool k1 = exprHasVar(kid[1], highLoc);
    	if(k0 & k1) {
    		return AndExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    	}
    	else if(k0) {
    		return buildProjection(kid[0]);
    	}
    	else if(k1){
    		return buildProjection(kid[1]);
    	}
    	else { // Should not happen since there is a varcheck done before calling getProjection
    		std::cerr << "Error : Request to duplicate expression with no H value";
    		cexpr->dump();
    		exit(0);
    	}
    }
    //case Expr::Or:  return OrExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    //case Expr::Xor: return XorExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));

    case Expr::Eq:  return EqExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::Ne:  return NeExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::Ult: return UltExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::Ule: return UleExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::Ugt: return UgtExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::Uge: return UgeExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::Slt: return SltExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::Sle: return SleExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::Sgt: return SgtExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    case Expr::Sge: return SgeExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    
    default:
    std::cerr << ">>>> SYSREL ERROR : Unhandled type of expression in buildProjection: " << cexpr->getKind() << "\n";
    exit(0);
    ref<Expr> res = klee::ConstantExpr::create(0, Expr::Bool);
    return res;    
  }
}

/*String utils*/
bool replace(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return false;
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void removeBrackets(std::string& str){
    std::string end = "End";
    std::string replacement = "";
    std::string brackets[] = {"(", ")", "{", "}", "[", "]", end};
    int i=0;
    while(brackets[i].compare(end)!=0){
        replace(str, brackets[i], replacement);
        ++i;
    }
}

std::vector<std::string> * processline(std::string s, std::string delimiter) {
	size_t pos = 0;
	std::string token;
	std::vector<std::string> * words = new std::vector<std::string>();
	while ((pos = s.find(delimiter)) != std::string::npos) {
		  token = s.substr(0, pos);
		  //std::cout << token << std::endl;
		  words->push_back(token);
		  s.erase(0, pos + delimiter.length());
	}
	//std::cout << s << std::endl;
	words->push_back(s);
	return words;
}
