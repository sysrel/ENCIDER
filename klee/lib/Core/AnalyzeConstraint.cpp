#include <iostream>
#include "Executor.h"
#include "klee/ExecutionState.h"
#include "AnalyzeConstraint.h"
#include "klee/util/ExprUtil.h"
#include "klee/util/ArrayCache.h"
#include "llvm/ADT/StringExtras.h"
#include <fstream>
#define dbp 0


extern bool rangesIntersect(unsigned min1, unsigned max1, unsigned min2, unsigned max2) ;

std::map<std::string, std::map<region, std::set<infoflowsource_t> > > infoFlowRules;

//bool isInSymRegion(std::string symname, ref<Expr> offset, Expr::Width type, bool high);
bool isASymRegion(std::string symname, bool high);

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


namespace std {

bool operator<(const region r1, const region r2) {
  if (r1.offset < r2.offset)
     return true;
  else if (r1.offset == r2.offset)
    return r1.size < r2.size;
  else  return false;
  /* 
  if (r1.offset == r2.offset)
     return r1.size < r2.size;
  else if (r1.size == r2.size) 
     return r2.offset < r2.offset;
  else r1.offset + r1.size  < r2.offset + r2.size;
  */
}

bool operator<(const infoflowsource_t i1, const infoflowsource_t i2) {
  if (i1.argno == i2.argno)
     return (i1.ifregion < i2.ifregion);
  else return (i1.argno < i2.argno);
}

}

bool isInfoFlowAPI(std::string fname) {
  return (infoFlowRules.find(fname) != infoFlowRules.end());
}

void moveOffset(std::vector<region> &rs, unsigned value) {
  for(unsigned int i=0; i<rs.size(); i++)
     rs[i].offset += value; 
}


bool intersect(region r1, region r2, region &res) {
     unsigned int min1 = r1.offset;
     unsigned int max1 = r1.offset+r1.size-1;
     unsigned int min2 = r2.offset;
     unsigned int max2 = r2.offset+r2.size-1;
     unsigned int maxofmin = min1 > min2 ? min1 : min2;
     unsigned int minofmax = max1 < max2 ? max1 : max2;
     if (maxofmin > minofmax)
        return false;        
     res.offset = maxofmin;
     res.size = minofmax - maxofmin + 1;
     return true;
}

bool intersect(region r1,   std::vector<region> r2, std::vector<region> &rs) {
     bool result = false;
     for(unsigned int i = 0; i<r2.size(); i++) {
        region r;
        if (intersect(r1, r2[i], r)) {
           rs.push_back(r); 
           result = true;
        }
     }
     return result;
} 

// assuming little endian
void shiftLeft(region r1, std::vector<region> &rs, unsigned value, unsigned width) {
  region r;
  r.offset = r1.offset + value;
  if (r1.offset + value + r1.size > width)
     r.size = width - r.offset;
  else r.size = r1.size;
  if (r.size > 0)
     rs.push_back(r);
}

void shiftLeft(std::vector<region> r1, std::vector<region> &rs, unsigned value, unsigned width) {
   for(int i=0; i<r1.size(); i++)
      shiftLeft(r1[i], rs, value, width);
}

// assuming little endian
void shiftRight(region r1, std::vector<region> &rs, unsigned value, unsigned width) {
  region r;
  r.offset = r1.offset - value;
  if (r.offset < 0) {
     r.offset = 0;
     r.size = r1.size + r1.offset - value;
  }
  else 
     r.size = r1.size;
  if (r.size > 0)
     rs.push_back(r);
}

void shiftRight(std::vector<region> r1, std::vector<region> &rs, unsigned value, unsigned width) {
   for(int i=0; i<r1.size(); i++)
      shiftRight(r1[i], rs, value, width);
}


bool canCoalesce(region r1, region r2) {
     //llvm::errs() << "in can coalesce " << r1.offset << "," << r1.size << " and " << r2.offset << "," << r2.size << "\n";
     if (r1.offset + r1.size == r2.offset || r2.offset + r2.size == r1.offset)
        return true;
     return rangesIntersect(r1.offset,r1.offset+r1.size-1,r2.offset,r2.offset+r2.size-1);
}

bool coalasce(region r1, region r2, region &res) {
     bool result = false;
     if (canCoalesce(r1,r2)) {
        unsigned int newoffset = r1.offset < r2.offset ? r1.offset : r2.offset;
        unsigned int newsize = r1.offset+r1.size-1 > r2.offset+r2.size-1 ? (r1.offset+r1.size - newoffset) : (r2.offset+r2.size - newoffset);
        res.offset = newoffset;
        res.size = newsize;
        result = true;
        //llvm::errs() << "coalsced region: " << res.offset << "," << res.size << "\n";
     }
     return result;
}

void mergeAndCoalesce(region r1, std::vector<region> rs2, std::vector<region> &rs3) {
    bool insert = true;
    for(unsigned int i=0; i<rs2.size(); i++) {
       region r;
       bool coal = coalasce(r1, rs2[i], r);
       if (coal) {
          insert = false;
          rs3.push_back(r);
      }
       else rs3.push_back(rs2[i]);
    }
    if (insert)
       rs3.push_back(r1); 
}

void mergeAndCoalesce(std::vector<region> rs1, std::vector<region> rs2, std::vector<region> &rs3) {
    //llvm::errs() << "coalesceAndMerge:\n";
    //llvm::errs() << "rs1:\n"; 
    //for(unsigned int i=0; i<rs1.size(); i++)
      // llvm::errs() << rs1[i].offset << "," << rs1[i].size << "\n";
    //llvm::errs() << "rs2:\n"; 
    //for(unsigned int i=0; i<rs2.size(); i++)
      // llvm::errs() << rs2[i].offset << "," << rs2[i].size << "\n";
    std::vector<region> temp = rs2;
    std::vector<region> temp2;
    for(unsigned int i=0; i<rs1.size(); i++) {
       mergeAndCoalesce(rs1[i], temp, temp2);
       temp = temp2;
       temp2.clear(); 
    } 
    for(unsigned int i=0; i<temp.size(); i++)
       rs3.push_back(temp[i]);
    //llvm::errs() << "rs3:\n"; 
    //for(unsigned int i=0; i<rs3.size(); i++)
      // llvm::errs() << rs3[i].offset << "," << rs3[i].size << "\n";
}

//std::vector<region> extractRegion(ref<Expr> expr, region range, bool high);

std::vector<region> Executor::getHighInfoFlowRegions(ExecutionState &state, std::string fname, 
                                    std::vector<ref<Expr> > & args) {
  std::vector<region> rs;
  if (infoFlowRules.find(fname) != infoFlowRules.end()) {
     std::map<region, std::set<infoflowsource_t> > fm = infoFlowRules[fname];
     for(auto fme : fm) {
        region ur; 
        ur.size = 0; 
        for(auto ifs : fme.second) {
              std::vector<region> temp;
              ref<Expr> &argp = args[ifs.argno];
              if (argp.isNull()) continue;
              int ns = 0;
              std::vector<region> r1 = extractRegion(state, args[ifs.argno],ifs.ifregion,ns,false,true,true);
              if (r1.size() == 0) continue;
              std::vector<region> r2;
              intersect(ifs.ifregion, r1, r2);
              moveOffset(r2, fme.first.offset - ifs.ifregion.offset);
              if (r2.size() > 0) {
                 if (rs.size() > 0) {
                    mergeAndCoalesce(r2,rs,temp);
                    rs = temp;
                    temp.clear();
                 }
                 else rs = r2;
              }
        }
     }     
  }
  return rs;
}

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



const Array *renameArray(MemoryManager *memory, const Array *array, bool high, unsigned int id) {
   if (isASymRegion(array->name, high)) {
      const Array *nar = memory->getArrayCache()->CreateArray(array->name + "_r_" + llvm::utostr(id), array->getSize());
      return nar;
   }
   else 
       return memory->getArrayCache()->CreateArray(array->name, array->getSize());
}


bool Executor::exprHasSymRegion(ExecutionState &state, ref<Expr> cexpr, bool high) {
  region r;
  r.offset = 0; r.size = 64;
  int ns = 0; 
  std::vector<region> r1 = extractRegion(state, cexpr,r,ns,false,true,high);
  if (r1.size() == 0) {
     //llvm::errs() << "empty region set!\n";
     return false;
  }
  //for(unsigned int i=0; i<r1.size(); i++)
    // llvm::errs() << "expr has high=" << high << " " << r1[i].offset << "," << r1[i].size << "\n";
  for(unsigned int i=0; i<r1.size(); i++)
     if (r1[i].size > 0)
        return true;
  //llvm::errs() << "no sized region in the solution set!\n";
  return false;
}

/*
bool exprHasSymRegion(ref<Expr> cexpr, bool high) {
    switch (cexpr->getKind()) {
       case Expr::Read: {
          ReadExpr *rexpr = dyn_cast<ReadExpr>(cexpr);
          //llvm::errs() << "Checking sensitivity of read expression " <<  (*rexpr) << "\n";
          if (isInSymRegion(rexpr->updates.root->name, rexpr->index, rexpr->getWidth(), high))
             return true;
          return false;
       }
       default: {
          bool result = false;
          bool found = false; 
          for(unsigned int i = 0; i<cexpr->getNumKids(); i++)
             if (exprHasSymRegion(cexpr->getKid(i), high)) {
                found = true;
                break;
             }
          return found;
       }
    }
}
*/

std::vector<region> Executor::extractRegion(ExecutionState &state, ref<Expr> cexpr, region range, int &numsym, bool concatSeen, bool top, bool high) {
   std::vector<region> rs;
   //llvm::errs() << "extracting region from " << cexpr << "\n";
   switch (cexpr->getKind()) {
      case Expr::Read: {
          ReadExpr *rexpr = dyn_cast<ReadExpr>(cexpr);             
          if (isInSymRegion(state, rexpr->updates.root->name, rexpr->index, Expr::Int8, rexpr->getWidth(), false, high)) 
             numsym++;
          ConstantExpr *CE = dyn_cast<ConstantExpr>(rexpr->index);
          if (!CE) { // approximating symbolic offset/index based on the possible range!
             rs.push_back(range);
             return rs;
          }
          else  {
             region r;
             if (concatSeen) {
                r.offset = CE->getZExtValue() * rexpr->getWidth();
                r.size = rexpr->getWidth();
             }
             else {
                r.offset = 0;
                r.size = rexpr->getWidth();
             }      
             //llvm::errs() << "read expr range: " << r.offset << "," << r.size << "\n";
             rs.push_back(r);
             return rs;
         }
      } 
      case Expr::Concat: {
          //llvm::errs() << "in concat: \n";
          int numsym1 = 0;
          int numsym2 = 0; 
          std::vector<region> r1 = extractRegion(state, cexpr->getKid(0), range, numsym1, true, false, high);
          std::vector<region> r2 = extractRegion(state, cexpr->getKid(1), range, numsym2, true, false, high);
          numsym += numsym1 + numsym2;
          if (top && numsym == 0) 
             return rs; 
          if  (r1.size() == 0 && r2.size() == 0) {
              return rs;
          }
          else if (r1.size() == 0) {
             return r2; 
          }
          else if (r2.size() == 0) {
             return r1; 
          }
          //llvm::errs() << " concat merging..\n";
          mergeAndCoalesce(r1, r2, rs);           
          return rs;
      }
      case Expr::Extract: {
        ExtractExpr *EE = dyn_cast<ExtractExpr>(cexpr);
        int numsym1 = 0;
        std::vector<region> r1 = extractRegion(state, cexpr->getKid(0), range, numsym1, concatSeen, false, high);
        numsym += numsym1;
        region r2;
        r2.offset = EE->offset;
        r2.size = EE->width;
        std::vector<region> r3; 
        bool inter = intersect(r2,r1,r3);         
        if (top && numsym1 == 0)
           return rs;
        if (inter) 
           return r3;
        return rs;
      }
      case Expr::Shl: 
      case Expr::LShr:
      case Expr::AShr: {
        int numsym1 = 0; 
        std::vector<region> r1 = extractRegion(state, cexpr->getKid(0), range, numsym1, concatSeen, false, high);
        numsym += numsym1;
        if (top && numsym1 == 0)
           return rs;
        if (r1.size() == 0) return rs;
        ConstantExpr * CE = dyn_cast<ConstantExpr>(cexpr->getKid(1));
        if (!CE) { // approximating symbolic offset/index based on the possible range!
             rs.push_back(range);
             return rs;
        }
        else  {
           unsigned int value = CE->getZExtValue();
           if (cexpr->getKind() == Expr::Shl)
             shiftLeft(r1, rs, value, cexpr->getWidth());
           else if (cexpr->getKind() == Expr::LShr || cexpr->getKind() == Expr::AShr)
             shiftRight(r1, rs, value, cexpr->getWidth());
           else assert(false);
           return rs;
        }        
      }
      default: {
         if (cexpr->getKind() == Expr::SExt || cexpr->getKind() == Expr::ZExt)  {
            int numsym1 = 0;
            std::vector<region> rs1 = extractRegion(state, cexpr->getKid(0), range, numsym1, concatSeen, false, high);
            numsym += numsym1;
            if (top && numsym1 == 0)
               return rs;
            return rs1;
         }        
         /*else if (!boolOp && (cexpr->getKind() == Expr::Slt || cexpr->getKind() == Expr::Sle || 
                  cexpr->getKind() == Expr::Sgt || cexpr->getKind() == Expr::Sge || 
                  cexpr->getKind() == Expr::Ult || cexpr->getKind() == Expr::Ule || 
                  cexpr->getKind() == Expr::Ugt || cexpr->getKind() == Expr::Uge || 
                  cexpr->getKind() == Expr::Eq || cexpr->getKind() == Expr::Ne || 
                  (!bitwise && (cexpr->getKind() == Expr::And || 
                               cexpr->getKind() == Expr::Or || cexpr->getKind() == Expr::Not))) {
                 // just merge the regions
                 
         }*/
         else if (cexpr->getNumKids() > 0) {
            //llvm::errs() << "child 0 " << cexpr->getKid(0) << "\n";
            int numsym1 = 0; 
            std::vector<region> rtemp1 = extractRegion(state, cexpr->getKid(0), range, numsym1, concatSeen, false, high);
            numsym += numsym1;
            //llvm::errs() << "child 0 " << cexpr->getKid(0) << " results (high=" << high << "): \n"; 
            //for(unsigned int i=0; i<rtemp1.size(); i++)
              // llvm::errs() << rtemp1[i].offset << "," << rtemp1[i].size << "\n";
            std::vector<region> rtemp3; 
            for(unsigned int i=1; i<cexpr->getNumKids(); i++) {
               //llvm::errs() << "child " << i << " " << cexpr->getKid(i) << "\n";
               int numsym2 = 0;
               std::vector<region> rtemp2 = extractRegion(state, cexpr->getKid(i), range, numsym2, concatSeen, false, high);
               numsym += numsym2;
               //llvm::errs() << "child " << i << " " << cexpr->getKid(i) << " results (high=" << high << "): \n";
               //for(unsigned int j=0; j<rtemp2.size(); j++)
                 // llvm::errs() << rtemp2[j].offset << "," << rtemp2[j].size << "\n";
               mergeAndCoalesce(rtemp1, rtemp2, rtemp3);
               rtemp1 = rtemp3;
               rtemp3.clear();
               //llvm::errs() << "rtemp1.size=" << rtemp1.size() << " numsym= " << numsym << "\n";
            }
            if (top && numsym == 0)
               return rs; 
            return rtemp1;
         }
         else return rs;
      }
   }

}

ref<Expr> Executor::getProjectionOnRegion(ExecutionState &state, ref<Expr> cexpr, bool high, bool maybebitwise) {
    switch (cexpr->getKind()) {    
      case Expr::Constant: {
        klee::ConstantExpr *CE = dyn_cast<klee::ConstantExpr>(cexpr);
        return klee::ConstantExpr::create(CE->getZExtValue(), CE->getWidth());
      }
      case Expr::Extract: {
        ExtractExpr *EE = dyn_cast<ExtractExpr>(cexpr);
        return ExtractExpr::create(getProjectionOnRegion(state, cexpr->getKid(0), high, true), EE->offset, EE->width);
      }
      case Expr::Read: {
          ReadExpr *rexpr = dyn_cast<ReadExpr>(cexpr);             
          ref<Expr> nrexpr = ReadExpr::alloc(rexpr->updates, rexpr->index);     
          //llvm:errs() << "Readexpr: " << cexpr << " copied to " << nrexpr << "\n";      
          return nrexpr;
      } 
      case Expr::Concat: {
          ref<Expr> rexpr1 = getProjectionOnRegion(state, cexpr->getKid(0), high, true);
          ref<Expr> rexpr2 = getProjectionOnRegion(state, cexpr->getKid(1), high, true);
          return ConcatExpr::create(rexpr1, rexpr2);
      }
      case Expr::And:
      case Expr::Or:
      case Expr::Xor: {
          if (!maybebitwise) {
             bool k0 = exprHasSymRegion(state, cexpr->getKid(0), high);
             bool k1 = exprHasSymRegion(state, cexpr->getKid(1), high);
             //llvm::errs() << cexpr->getKid(0) << " has high (" << high << " sym region?: " << k0 << "\n";
             //llvm::errs() << cexpr->getKid(1) << " has high (" << high << " sym region?: " << k1 << "\n";
             if (k0 && k1) {
                if (cexpr->getKind() == Expr::And)
                   return AndExpr::create(getProjectionOnRegion(state, cexpr->getKid(0),high,maybebitwise), 
                                       getProjectionOnRegion(state, cexpr->getKid(1),high,maybebitwise));
                else if (cexpr->getKind() == Expr::Or)
                   return OrExpr::create(getProjectionOnRegion(state, cexpr->getKid(0),high,maybebitwise), 
                                       getProjectionOnRegion(state, cexpr->getKid(1),high,maybebitwise));
                else if (cexpr->getKind() == Expr::Xor)
                   return XorExpr::create(getProjectionOnRegion(state, cexpr->getKid(0),high,maybebitwise), 
                                       getProjectionOnRegion(state, cexpr->getKid(1),high,maybebitwise)); 
                else assert(false);
                  
             }
             else if (k0) {
                return getProjectionOnRegion(state, cexpr->getKid(0),high,maybebitwise);
             }
             else if (k1) {
                return getProjectionOnRegion(state, cexpr->getKid(1),high,maybebitwise);
             }
             else assert(false);
          }
          // otherwise fall through the default case
      }
      default:
        if (!((cexpr->getKind() == Expr::Not) || (cexpr->getKind() == Expr::Eq) ||  
             (cexpr->getKind() == Expr::Ne)))
           maybebitwise = true;         
        std::vector<Expr::CreateArg> args;
        int size = cexpr->getNumKids();
        int i;
        for(i=0; i<size; i++) {
           ref<Expr> rexpr = getProjectionOnRegion(state, cexpr->getKid(i), high, maybebitwise);
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

ref<Expr> Executor::getProjectionOnRegion(ExecutionState &state, ref<Expr> cexpr, bool high) {
  return getProjectionOnRegion(state, cexpr, high, false);
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
          const UpdateList *rul = new UpdateList(renameArray(memory, rexpr->updates.root, high, id), 
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


ref<Expr> renameExpr(MemoryManager *memory, unsigned versionNo, ref<Expr> cexpr, bool high) {
   return rename(memory, cexpr, high, versionNo);
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
    case Expr::Not: return NotExpr::create(buildProjection(cexpr,high));
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
