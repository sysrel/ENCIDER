#ifndef RESOURCEUSAGE_H
#define RESOURCEUSAGE_H

#include <map>
#include <algorithm>
#include "klee/Expr.h"
#include "Executor.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#define ICost 1
#define epsilon 0

using namespace llvm;
using namespace klee;

typedef std::set<long> BBset;
typedef std::pair<ref<Expr>, ref<Expr> > HCLC;
typedef std::pair<int, int> range;
typedef std::pair<unsigned, unsigned> exhash;
typedef std::map<exhash, range> RU;
typedef std::map<exhash, HCLC> HashExpr;
typedef std::map<exhash, ref<Expr> > HashRet;
typedef std::map<exhash, BBset> BS;

// the basic blocks in a do not appear in set b
// reports cache code leakage, if any
bool checkCacheLeakage(BBset a, BBset b);

unsigned int getTimingModel(std::string fname);

void printInfo(const InstructionInfo &ii);

void printviolations(Executor* ex);

void readConfig();

void readSCAInput();

bool isUntrusted(std::string fname);

bool isHaltedState(ExecutionState * s);

void readDynamicHighLoc();

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
                BBset bbs;
                BS bbm;
		//InstructionInfo *info;
		bool valid = true;
		ref<Expr> retval; // For leaf nodes
		bool isvoid = true; // For leaf nodes
		// For H-ancestors
		HashRet* lrets = new HashRet();  // Map of hash of expression to expression. Stores the return values of each path. Since returns are public.
		bool pathterminated = false; // For leaf nodes keeps track of path termination info.
                static int numHAncestors;
                static int numHLMixedConstraints;
};

//

void printResourceUsage(RD* s);

int updatesource(exhash exhash1, exhash exhash2, RD* rd);

RD* newNode(ExecutionState * s, Instruction *ii);

RD* newNode(ExecutionState * s);

RD* newNode(RD* rd);

void printPreOrder(RD* rd, std::string indent);

exhash getHash(ref<Expr> e1, ref<Expr> e2);

range combineRange(range r1, range r2);

bool isTrue(ref<Expr> condition, Executor* ex);

void printLeakage(RD* rd, Executor* ex);

void checkLeakage(RD* rd, Executor* ex);

unsigned propagate(RD* rd, std::string indent, Executor* ex);

void rdmapinsert(int i, RD* rd);

RD *rdmapreplace(int i, RD* rd);

RD* getrdmap(ExecutionState * s);

void printResourceUsage(RD * s);

void printRDtree(RD* rd, std::string indent);

void initStateRes(RD * s);

#endif
