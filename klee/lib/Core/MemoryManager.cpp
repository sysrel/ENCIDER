//===-- MemoryManager.cpp -------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "CoreStats.h"
#include "Memory.h"
#include "MemoryManager.h"

#include "klee/Expr.h"
#include "klee/Internal/Support/ErrorHandling.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MathExtras.h"

#include <inttypes.h>
#include <sys/mman.h>
#include "klee/Internal/Module/KInstruction.h"
#include "klee/ExecutionState.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DataLayout.h""

using namespace klee;

/* SYSREL EXTENSION */
extern const llvm::Module * moduleHandle;
extern std::set<llvm::Type*> embeddedTypes;
extern std::map<llvm::Type*, std::set<llvm::Type*> >  embeddingTypes;
extern std::string getTypeName(llvm::Type*);
/* SYSREL EXTENSION */

namespace {
llvm::cl::opt<bool> DeterministicAllocation(
    "allocate-determ",
    llvm::cl::desc("Allocate memory deterministically(default=off)"),
    llvm::cl::init(false));

llvm::cl::opt<unsigned> DeterministicAllocationSize(
    "allocate-determ-size",
    llvm::cl::desc(
        "Preallocated memory for deterministic allocation in MB (default=100)"),
    llvm::cl::init(100));

llvm::cl::opt<bool>
    NullOnZeroMalloc("return-null-on-zero-malloc",
                     llvm::cl::desc("Returns NULL in case malloc(size) was "
                                    "called with size 0 (default=off)."),
                     llvm::cl::init(false));

llvm::cl::opt<unsigned> RedZoneSpace(
    "red-zone-space",
    llvm::cl::desc("Set the amount of free space between allocations. This is "
                   "important to detect out-of-bound accesses (default=10)."),
    llvm::cl::init(10));

llvm::cl::opt<unsigned long long> DeterministicStartAddress(
    "allocate-determ-start-address",
    llvm::cl::desc("Start address for deterministic allocation. Has to be page "
                   "aligned (default=0x7ff30000000)."),
    llvm::cl::init(0x7ff30000000));
}

/***/
MemoryManager::MemoryManager(ArrayCache *_arrayCache)
    : arrayCache(_arrayCache), deterministicSpace(0), nextFreeSlot(0),
      spaceSize(DeterministicAllocationSize.getValue() * 1024 * 1024) {
  if (DeterministicAllocation) {
    // Page boundary
    void *expectedAddress = (void *)DeterministicStartAddress.getValue();

    char *newSpace =
        (char *)mmap(expectedAddress, spaceSize, PROT_READ | PROT_WRITE,
                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (newSpace == MAP_FAILED) {
      klee_error("Couldn't mmap() memory for deterministic allocations");
    }
    if (expectedAddress != newSpace && expectedAddress != 0) {
      klee_error("Could not allocate memory deterministically");
    }

    klee_message("Deterministic memory allocation starting from %p", newSpace);
    deterministicSpace = newSpace;
    nextFreeSlot = newSpace;
  }
}

MemoryManager::~MemoryManager() {
  while (!objects.empty()) {
    MemoryObject *mo = *objects.begin();
    if (!mo->isFixed && !DeterministicAllocation)
      free((void *)mo->address);
    objects.erase(mo);
    delete mo;
  }

  if (DeterministicAllocation)
    munmap(deterministicSpace, spaceSize);
}


/* SYSREL extension */

MemoryObject *MemoryManager::getInstanceForType(ExecutionState &state, llvm::Type *t) {
  if (state.lazyInitSingleInstances.find(t) != state.lazyInitSingleInstances.end())
     return state.lazyInitSingleInstances[t];
  return NULL;
}


ref<Expr> MemoryManager::getInstanceAddressForType(ExecutionState &state, llvm::Type *t, bool &result) {
   llvm::errs() << "in getInstanceAddressForType " << getTypeName(t) << "\n";
  if (state.lazyInitSingleInstances.find(t) != state.lazyInitSingleInstances.end()) {
      result = true;
     return state.lazyInitSingleInstances[t]->getBaseExpr();
  }
  else  {
        // Grab the address mapped to this type
        if (state.typeToAddr.find(t) != state.typeToAddr.end()) {
            result = true;
            return state.typeToAddr[t];
        }
        // Grab the address for one of its ancessor embedding type and use the relative offset to compute the address
        //bool found = false;
        //while (!found) {
        if (embeddedTypes.find(t) != embeddedTypes.end()) {
           std::set<llvm::Type*> embset = embeddingTypes[t];
           if (embset.size() > 1) {
               llvm::errs() << "Warning: Single instance embedded type has multiple embedders!\n";
           }
           else { // must be one
               for(auto et : embset) {
                  
                  llvm::StructType *set = dyn_cast<llvm::StructType>(et);
                  assert(set);
                  bool res; 
                  ref<Expr> eaddr = getInstanceAddressForType(state, et, res);
                  if (res) {
                     const llvm::DataLayout &dl = moduleHandle->getDataLayout();
                     const llvm::StructLayout *sl =  dl.getStructLayout(set);
                     for(unsigned i=0; i< set->getNumElements(); i++) {  
                        if (set->getElementType(i) == t)
                            break;
                     assert(i >=0  && i < set->getNumElements());
                     ref<Expr> re = AddExpr::create(ConstantExpr::create(sl->getElementOffset(i), Expr::Int64), eaddr);
                     llvm::outs() << "instance for embedded type found at " << re << "\n";
                     result = true;
                     return re;
                     }
                     
                  }
               }
          // }
 
        }
     }
  }
  result = false;
  return ConstantExpr::alloc(0, Expr::Int64);
}

MemoryObject *MemoryManager::simulateMalloc(ExecutionState &state, ref<Expr> size) {
 size_t allocationAlignment = 8;
 if (ConstantExpr *CE = dyn_cast<ConstantExpr>(size)) {
    MemoryObject *mo = allocate(CE->getZExtValue(), false, /*isGlobal=*/false,
                          state.prevPC->inst, allocationAlignment);
    mo->name = "smalloc" + CE->getZExtValue();
    llvm::outs() << "simulating allocation of size : " << CE->getZExtValue() << " at address " << mo->getBaseExpr() << "\n";
    return mo;  
 } 
 assert(false && "Not simulating symbolic size memory allocations!"); 
}
 

MemoryObject *MemoryManager::allocateForLazyInit(ExecutionState &state, llvm::Instruction *inst, llvm::Type *allocType, bool isSingle, int count) {
 std::string type_str;
 llvm::raw_string_ostream rso(type_str);
 allocType->print(rso); 
 size_t allocationAlignment = 8;
 const llvm::DataLayout & dl = inst->getParent()->getParent()->getParent()->getDataLayout();
 llvm::LoadInst *li = dyn_cast<llvm::LoadInst>(inst);
 if (li) {
    allocationAlignment = li->getAlignment();
 } 
 else {
    allocationAlignment = dl.getPrefTypeAlignment(allocType);
 }
 MemoryObject *mo = NULL; 
 if (isSingle) {
    mo = getInstanceForType(state, allocType);
   if (mo) {
      llvm::outs() << "returning address " << mo->getBaseExpr() << " as single instance of type " << rso.str() << "\n"; 
      return mo;
   }
 }
 llvm::outs() << "allocation size: " << dl.getTypeAllocSize(allocType)*count << "\n"; 
 mo = allocate(dl.getTypeAllocSize(allocType)*count, false, /*true*/false, inst, allocationAlignment);

 llvm::StructType *st = dyn_cast<llvm::StructType>(allocType);
 if (st) {
    std::string tname = getTypeName(allocType); 
    state.typeToAddr[allocType] = mo->getBaseExpr();
    llvm::outs() << "mapping lazy instance of " << tname << " to " << mo->getBaseExpr() << "\n";
 }   


 if (isSingle) {
    state.lazyInitSingleInstances[allocType] = mo;
    llvm::outs() << "storing address " << mo->getBaseExpr() << " as single instance of type " << rso.str() << "\n"; 
 }
 return mo;
}
/* SYSREL extension */

MemoryObject *MemoryManager::allocate(uint64_t size, bool isLocal,
                                      bool isGlobal,
                                      const llvm::Value *allocSite,
                                      size_t alignment) {
  if (size > 10 * 1024 * 1024)
    klee_warning_once(0, "Large alloc: %" PRIu64
                         " bytes.  KLEE may run out of memory.",
                      size);

  // Return NULL if size is zero, this is equal to error during allocation
  if (NullOnZeroMalloc && size == 0)
    return 0;

  if (!llvm::isPowerOf2_64(alignment)) {
    klee_warning("Only alignment of power of two is supported");
    return 0;
  }

  uint64_t address = 0;
  if (DeterministicAllocation) {
#if LLVM_VERSION_CODE >= LLVM_VERSION(3, 9)
    address = llvm::alignTo((uint64_t)nextFreeSlot + alignment - 1, alignment);
#else
    address = llvm::RoundUpToAlignment((uint64_t)nextFreeSlot + alignment - 1,
                                       alignment);
#endif

    // Handle the case of 0-sized allocations as 1-byte allocations.
    // This way, we make sure we have this allocation between its own red zones
    size_t alloc_size = std::max(size, (uint64_t)1);
    if ((char *)address + alloc_size < deterministicSpace + spaceSize) {
      nextFreeSlot = (char *)address + alloc_size + RedZoneSpace;
    } else {
      klee_warning_once(0, "Couldn't allocate %" PRIu64
                           " bytes. Not enough deterministic space left.",
                        size);
      address = 0;
    }
  } else {
    // Use malloc for the standard case
    if (alignment <= 8)
      address = (uint64_t)malloc(size);
    else {
      int res = posix_memalign((void **)&address, alignment, size);
      if (res < 0) {
        klee_warning("Allocating aligned memory failed.");
        address = 0;
      }
    }
  }

  if (!address)
    return 0;

  ++stats::allocations;
  MemoryObject *res = new MemoryObject(address, size, isLocal, isGlobal, false,
                                       allocSite, this);
  objects.insert(res);
  return res;
}

MemoryObject *MemoryManager::allocateFixed(uint64_t address, uint64_t size,
                                           const llvm::Value *allocSite) {
#ifndef NDEBUG
  for (objects_ty::iterator it = objects.begin(), ie = objects.end(); it != ie;
       ++it) {
    MemoryObject *mo = *it;
    if (address + size > mo->address && address < mo->address + mo->size)
      klee_error("Trying to allocate an overlapping object");
  }
#endif

  ++stats::allocations;
  MemoryObject *res =
      new MemoryObject(address, size, false, true, true, allocSite, this);
  objects.insert(res);
  return res;
}

void MemoryManager::deallocate(const MemoryObject *mo) { assert(0); }

void MemoryManager::markFreed(MemoryObject *mo) {
  if (objects.find(mo) != objects.end()) {
    if (!mo->isFixed && !DeterministicAllocation)
      free((void *)mo->address);
    objects.erase(mo);
  }
}

size_t MemoryManager::getUsedDeterministicSize() {
  return nextFreeSlot - deterministicSpace;
}
