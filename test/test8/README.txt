Testing cache based side channels


$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=pm.txt -sens
itive-inputs=sensargs.txt $1

$ more sensargs.txt 
low,foo,0
high,foo,1

$ ./run.sh test8.bc foo 2>&1 | tee o.txt

Expected output:

  cache side channel due to using expressions on sensitive input as an index for table access

  timing side channel due to an imbalance in the two branches on security sensitive input


KLEE: done: #Timing Side Channel  Locs = 1
KLEE: done: #Cache Side Channel  Locs = 1
KLEE: done: minInst = 20
KLEE: done: maxInst = 1689
KLEE: done: HAncestors = 1
KLEE: done: HLMixedConstraints = 0
KLEE: done: HVars= 0
KLEE: done: LVars= 0
KLEE: done: total instructions = 1774
KLEE: done: completed paths = 20
KLEE: done: generated tests = 20
