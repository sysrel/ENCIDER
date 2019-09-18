#ifndef ANALYZECONSTRAINT_H
#define ANALYZECONSTRAINT_H

#include <iostream>
#include "klee/Expr.h"
#include "Memory.h"
#include "MemoryManager.h"

#define dbp 0

using namespace llvm;
using namespace klee;

struct region {
  int offset;
  int size;
};
typedef struct region region;


struct infoflowsource_t {
  unsigned int argno;
  region ifregion;
};
typedef struct infoflowsource_t infoflowsource_t ; 


//InfoFlowSummarization

/*
class Region {
   std::set<region> ifregion;
  public: 
   Region();
   Region(const Region &);
   virtual void addRegion(region);
   virtual ~Region();
};

class MLRegion : Region { 
   std::map<region, Region*> iflow;
   public: 
      MLRegion();
      // adding multi layer
      virtual void addRegion(std::vector<region>);
      virtual ~MLRegion();
};
*/

struct InfoFlowRegion {
  int index;
  std::vector<region> regions;
};
typedef struct InfoFlowRegion InfoFlowRegion_t; 

namespace std {
  bool operator<(const region r1, const region r2);
  bool operator<(const infoflowsource_t i1, const infoflowsource_t i2);
}

std::vector<region> getHighInfoFlowRegions(std::string fname, std::vector<ref<Expr> > & args);
ref<Expr> getProjectionOnRegion(ref<Expr> cexpr, bool high) ;
bool exprHasSymRegion(ref<Expr> cexpr, bool high);
ref<Expr> renameExpr(MemoryManager *memory, ref<Expr> cexpr, bool high);
std::set<std::string>* getNameofAddressConstraintSet(klee::ref<Expr> cexpr);
std::vector<std::string> * processline(std::string s, std::string delimiter);
void dumpKids(klee::ref<Expr>& cexpr);
bool replace(std::string& str, const std::string& from, const std::string& to);
void removeBrackets(std::string& str);
ref<Expr> buildProjection(ref<Expr>& cexpr, bool high);

bool isInfoFlowAPI(std::string);

void readMemLoc();

bool exprHasVar(klee::ref<Expr> expr, std::set<std::string> * varset);

std::string getString(ref<Expr> e);

std::set<std::string>* getNameofAddressConstraintSet(klee::ref<Expr> cexpr);

void dumpKids(klee::ref<Expr>& cexpr);

void dumpKidsRec(klee::ref<Expr>& cexpr);

/*
 * Assumption for getProjection and buildProjection is that
 * cexpr has atleast 1 highloc
 */
ref<Expr> getProjection(ref<Expr>& cexpr, std::set<std::string> * varset);

ref<Expr> buildProjection(ref<Expr>& cexpr, bool high);

/*String utils*/
bool replace(std::string& str, const std::string& from, const std::string& to);

void removeBrackets(std::string& str);

std::vector<std::string> * processline(std::string s, std::string delimiter);

#endif
