#include <iostream>
#include "AnalyzeConstraint.h"
#include "klee/util/ExprUtil.h"
#include "klee/util/ArrayCache.h"
#include "llvm/ADT/StringExtras.h"
#include <fstream>
#define dbp 0



/* Variables based on sensivity */
std::set<std::string> * highLoc = new std::set<std::string>();
std::set<std::string> * lowLoc = new std::set<std::string>();
/* Variables based on sensivity End */

std::set<std::string>* getNameofAddressConstraintSet(klee::ref<Expr> cexpr);
std::vector<std::string> * processline(std::string s, std::string delimiter);
void dumpKids(klee::ref<Expr>& cexpr);
bool replace(std::string& str, const std::string& from, const std::string& to);
void removeBrackets(std::string& str);
ref<Expr> buildProjection(ref<Expr>& cexpr, bool high);

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
                                llvm::errs() << "high " << (*rit) << "\n";
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
                                llvm::errs() << "low " << (*rit) << "\n";
			}
		}
	}
	infile2.close();
}


bool exprHasVar(klee::ref<Expr> expr, std::set<std::string> * varset) {
  std::set<std::string>* names = getNameofAddressConstraintSet(expr);
  std::set<std::string>::iterator it;
  #if VBSC
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
         #ifdef VBSC
          std::cerr << " checking " << var << "\n";
         #endif
	  if(it != names->end()) {
	  	#if VBSC
	  		std::cerr << "Found " << var << "\n";
	  	#endif
	  	return true;
	  }
	  #if VBSC
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
  #if VBSC
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
  #if VBSC
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



const Array *renameArray(MemoryManager *memory, const Array *array, std::set<std::string> * varset, unsigned int id) {
   if (varset->find(array->name) != varset->end()) {
      const Array *nar = memory->getArrayCache()->CreateArray(array->name + "_r_" + llvm::utostr(id), array->getSize());
      return nar;
   }
   else 
       return memory->getArrayCache()->CreateArray(array->name, array->getSize());
}





ref<Expr> rename(MemoryManager *memory, ref<Expr> cexpr, bool high, unsigned id) {
    switch (cexpr->getKind()) {    
      case Expr::Constant: {
        klee::ConstantExpr *CE = dyn_cast<klee::ConstantExpr>(cexpr);
        return klee::ConstantExpr::create(CE->getZExtValue(), CE->getWidth());
      }
      case Expr::Extract: {
        ExtractExpr *EE = dyn_cast<ExtractExpr>(cexpr);
        ref<Expr> eexpr = rename(memory, cexpr->getKid(0), high, id);
        return ExtractExpr::create(eexpr, EE->offset, EE->width);
      }
      case Expr::Read: {
          ReadExpr *rexpr = dyn_cast<ReadExpr>(cexpr);             
          const UpdateList *rul = new UpdateList(renameArray(memory, rexpr->updates.root, high ? highLoc : lowLoc, id), 
                                                 rexpr->updates.head);
          ref<Expr> nrexpr = ReadExpr::alloc(*rul, rexpr->index);     
          //llvm:errs() << "Readexpr: " << cexpr << " renamed as " << nrexpr << "\n";      
          return nrexpr;
      } 
      case Expr::Concat: {
          ref<Expr> rexpr1 = rename(memory, cexpr->getKid(0), high, id);
          ref<Expr> rexpr2 = rename(memory, cexpr->getKid(1), high, id);
          return ConcatExpr::create(rexpr1, rexpr2);
      }
      default:
        std::vector<Expr::CreateArg> args;
        int size = cexpr->getNumKids();
        int i;
        for(i=0; i<size; i++) {
           ref<Expr> rexpr = rename(memory, cexpr->getKid(i), high, id);
           args.push_back(Expr::CreateArg(rexpr));
           //llvm::errs() << "args[" << i << "] " << args[i].expr << "\n";
        }    
        if (cexpr->getKind() == Expr::SExt || cexpr->getKind() == Expr::ZExt) {
           args.push_back(Expr::CreateArg(cexpr->getWidth()));
           //llvm::errs() << "args[" << i << "] " << args[i].width << "\n";
        }
        //llvm::errs() << "cexpr: " << cexpr << " kind: " << cexpr->getKind() << "\n"; 
        return Expr::createFromKind(cexpr->getKind(), args);
   }
}

// uses a static index that gets incremented each time a rename operation is used
ref<Expr> renameExpr(MemoryManager *memory, ref<Expr> cexpr, bool high) {
  static unsigned id = 0;
  return rename(memory, cexpr, high, id++);
}

/*
 * Assumption for getProjection and buildProjection is that
 * cexpr has atleast 1 highloc or 1 lowloc
 */
ref<Expr> getProjection(ref<Expr>& cexpr, std::set<std::string> * varset) {
	//std::set<std::string>* names = getNameofAddressConstraintSet(cexpr);

	ref<Expr> e = buildProjection(cexpr, (varset == highLoc));
	return e;
}

ref<Expr> buildProjection(ref<Expr>& cexpr, bool high) {
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
        return ZExtExpr::create(buildProjection(kid[0],high), cexpr->getWidth());
    }
    case Expr::SExt: return SExtExpr::create(buildProjection(kid[0],high), cexpr->getWidth());
    case Expr::NotOptimized: {
            if(size > 0) { return NotOptimizedExpr::create(buildProjection(kid[0],high)); }
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
    case Expr::Concat: return ConcatExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::Extract: {
				ExtractExpr *EE = dyn_cast<ExtractExpr>(cexpr);
        return ExtractExpr::create(buildProjection(kid[0],high), EE->offset, EE->width);
    }
    case Expr::Select: return SelectExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high),
                                                 buildProjection(kid[2],high));
    case Expr::Add: return AddExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::Sub: return SubExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::Mul: return MulExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::UDiv: return UDivExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::SDiv: return SDivExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::URem: return URemExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::SRem: return SRemExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::AShr: return AShrExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::LShr: return LShrExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::Shl: return ShlExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));

    case Expr::And:
    case Expr::Or:
    case Expr::Xor:
    {
    	bool k0 = high ? exprHasVar(kid[0], highLoc) : exprHasVar(kid[0], lowLoc);
    	bool k1 = high ? exprHasVar(kid[1], highLoc) : exprHasVar(kid[1], lowLoc);
    	if (k0 && k1) {
    		return AndExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    	}
    	else if (k0) {
    		return buildProjection(kid[0],high);
    	}
    	else if (k1){
    		return buildProjection(kid[1],high);
    	}
    	else 
        { // Should not happen since there is a varcheck done before calling getProjection
    		std::cerr << "high=" << high << " k0=" << k0 << " k1=" << k1 << " Error : Request to duplicate expression with no H value";
                llvm::errs() << "\nsubexpr 1:\n"; kid[0]->dump();
    		llvm::errs() << "\nsubexpr 2:\n"; kid[1]->dump();
    		exit(0);
    	}

    }
    //case Expr::Or:  return OrExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));
    //case Expr::Xor: return XorExpr::create(buildProjection(kid[0]), buildProjection(kid[1]));

    case Expr::Eq:  return EqExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::Ne:  return NeExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::Ult: return UltExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::Ule: return UleExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::Ugt: return UgtExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::Uge: return UgeExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::Slt: return SltExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::Sle: return SleExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::Sgt: return SgtExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));
    case Expr::Sge: return SgeExpr::create(buildProjection(kid[0],high), buildProjection(kid[1],high));

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
