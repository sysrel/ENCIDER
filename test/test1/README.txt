This is the example in the paper. 

$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=pm.txt -sens
itive-inputs=sensargs.txt $1


$ more sensargs.txt 
low,foo,0
high,foo,1


To run:
 ./run.sh test1.bc foo 2>&1 | tee o.txt

 grep -r "Found V" o.txt | wc -l
Expected result:
 11

Statistics:

KLEE: done: #Timing Side Channel  Locs = 2
KLEE: done: #Cache Side Channel  Locs = 0
KLEE: done: minInst = 69
KLEE: done: maxInst = 153
KLEE: done: HAncestors = 2
KLEE: done: HLMixedConstraints = 0
KLEE: done: HVars= 0
KLEE: done: LVars= 0
KLEE: done: total instructions = 823
KLEE: done: completed paths = 8
KLEE: done: generated tests = 8
