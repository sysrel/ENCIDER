Testing data structures of mixed sensitivity and with high security region that is not accessed in the code.
So should not find any violations.

$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=pm.txt -sens
itive-inputs=sensargs.txt -sensitive-type-region=senstypes.txt $1

$ more sensargs.txt 
mixed,foo,0

$ more senstypes.txt 
low,%struct.mytest,32,32
high,%struct.mytest,0,32

$ ./run.sh test13.bc foo 2>&1 | tee o.txt 

Expected output:

KLEE: done: #Timing Side Channel  Locs = 0
KLEE: done: #Cache Side Channel  Locs = 0
KLEE: done: minInst = 71
KLEE: done: maxInst = 155
KLEE: done: HAncestors = 0
KLEE: done: HLMixedConstraints = 0
KLEE: done: HVars= 0
KLEE: done: LVars= 0
KLEE: done: total instructions = 831
KLEE: done: completed paths = 8
KLEE: done: generated tests = 8



