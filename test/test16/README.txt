Testing API information flow models and tracking

$ more run.sh
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=
pm.txt -sensitive-inputs=sensargs.txt -info-flow-models=infoFlow.txt  $1

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

./run.sh test16.bc foo 2>&1 | tee o.txt

Expected result:

>>>> Listing violations :  

	=====================
	Function : foo

	>>>> /home/tuba/Documents/tools/pmguidedsymex/test/test16/test16.c:26
	=====================

>>>> Found violations at  : 1 locations.
all high sym regions: 
%sym13__mm_fake1_0
%sym17__mm_fake2_0
%sym21__mm_fake3_0
%sym25__mm_fake4_0
foo_arg_1_0
all low sym regions: 
foo_arg_0_0
return_value

KLEE: done: #Timing Side Channel  Locs = 1
KLEE: done: #Cache Side Channel  Locs = 0
KLEE: done: minInst = 102
KLEE: done: maxInst = 186
KLEE: done: HAncestors = 1
KLEE: done: HLMixedConstraints = 0
KLEE: done: HVars= 0
KLEE: done: LVars= 0
KLEE: done: total instructions = 445
KLEE: done: completed paths = 4
KLEE: done: generated tests = 4

