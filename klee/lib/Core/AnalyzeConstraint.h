#ifndef ANALYZECONSTRAINT_H
#define ANALYZECONSTRAINT_H

#include <iostream>
#include "klee/Expr.h"
#include "Memory.h"
#include "MemoryManager.h"

#define dbp 0

using namespace llvm;
using namespace klee;

ref<Expr> renameExpr(MemoryManager *memory, ref<Expr> cexpr, bool high);
std::set<std::string>* getNameofAddressConstraintSet(klee::ref<Expr> cexpr);
std::vector<std::string> * processline(std::string s, std::string delimiter);
void dumpKids(klee::ref<Expr>& cexpr);
bool replace(std::string& str, const std::string& from, const std::string& to);
void removeBrackets(std::string& str);
ref<Expr> buildProjection(ref<Expr>& cexpr, bool high);

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
