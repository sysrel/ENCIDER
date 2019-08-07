Making the function loop an ocall removes the instruction count balance. 

$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=pm.txt -sens
itive-inputs=sensargs.txt -timing-obs-point=ocalls.txt $1

$ more sensargs.txt 
low,foo,0
high,foo,1

$ more ocalls.txt 
loop

Expected result is no violation to be found.

>>>> Listing violations :  

>>>> Found violations at  : 0 locations.
all high sym regions: 
foo_0_arg_1
all low sym regions: 
foo_0_arg_0
return_value

KLEE: done: #Timing Side Channel  Locs = 0
KLEE: done: #Cache Side Channel  Locs = 0
KLEE: done: minInst = 12
KLEE: done: maxInst = 153
KLEE: done: HAncestors = 2
KLEE: done: HLMixedConstraints = 0
KLEE: done: HVars= 0
KLEE: done: LVars= 0
KLEE: done: total instructions = 823
KLEE: done: completed paths = 16
KLEE: done: generated tests = 16

