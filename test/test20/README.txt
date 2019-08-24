Tests functions that get called with sensitive arguments. Note that this is different than entry point functions
as those automatically get symbolized args. However, for functions that called the parameter possibly could be 
a global variable, a pointer to a global variable, or a local variable. This is especially important for 
enclaves and how secrets may be stored in them.

$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=
pm.txt -sensitive-inputs=sensargs.txt $1

$ more sensargs.txt 
low,foo,0
high,foo,1

$./run.sh test20.bc bar 2>&1 | tee o.txt

Expected output:

KLEE: done: #Timing Side Channel  Locs = 2
KLEE: done: #Cache Side Channel  Locs = 0
KLEE: done: minInst = 74
KLEE: done: maxInst = 159
KLEE: done: HAncestors = 2
KLEE: done: HLMixedConstraints = 0
KLEE: done: HVars= 0
KLEE: done: LVars= 0
KLEE: done: total instructions = 836
KLEE: done: completed paths = 8
KLEE: done: generated tests = 8
