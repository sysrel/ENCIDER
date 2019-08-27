
Testing if setting a region high/low is performed consistently. As a example, an encrypt function may perform in-place encryption. In a case like this 
a buffer may hold high sensitive data before the encrypt call and low sensitive data afterwards.

$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=
pm.txt -sensitive-inputs=sensargs.txt -input-funcs=inputFuncs.txt $1

$ more sensargs.txt 
low,foo,0
high,foo,1

$ more inputFuncs.txt 
low,bar,0

Expected output:

No side channels!

KLEE: done: #Timing Side Channel  Locs = 0
KLEE: done: #Cache Side Channel  Locs = 0
KLEE: done: minInst = 74
KLEE: done: maxInst = 159
KLEE: done: HAncestors = 0
KLEE: done: HLMixedConstraints = 0
KLEE: done: HVars= 0
KLEE: done: LVars= 0
KLEE: done: total instructions = 829
KLEE: done: completed paths = 8
KLEE: done: generated tests = 8
