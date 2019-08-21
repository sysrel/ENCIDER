Testing API info flow modeling and tracking with shift operators. Specifically, that by shifting the high region might get lost!

$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=pm.txt -sensitive-inputs=sensargs.txt -info-flow-models=infoFlow
.txt  $1

$ more sensargs.txt 
low,foo,0
high,foo,1

$ more infoFlow.txt 
_mm_fake1,0,32,arg0,0,32
_mm_fake2,0,16,arg0,0,16
_mm_fake2,16,16,arg1,0,16
_mm_fake3,0,32,UNION(arg0,0,32,arg1,0,32)
_mm_fake4,0,32,UNION(arg0,0,16,arg1,0,16)
_mm_fake5,0,32,UNION(arg0,0,32,arg1,0,32)

./run.sh test17.bc foo 2>&1 | tee o.tx


Expected output:

>>>> Listing violations :  

>>>> Found violations at  : 0 locations.
all high sym regions: 
%sym12__mm_fake2_0
offset=0, size=16
foo_arg_1_0
offset=0, size=32
all low sym regions: 
foo_arg_0_0
offset=0, size=32
return_value
offset=0, size=32

KLEE: done: #Timing Side Channel  Locs = 0
KLEE: done: #Cache Side Channel  Locs = 0
KLEE: done: minInst = 88
KLEE: done: maxInst = 172
KLEE: done: HAncestors = 0
KLEE: done: HLMixedConstraints = 0
KLEE: done: HVars= 0
KLEE: done: LVars= 0
KLEE: done: total instructions = 431
KLEE: done: completed paths = 4
KLEE: done: generated tests = 4

