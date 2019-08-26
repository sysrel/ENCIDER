Testing API spec that specifies sensitive flows and a region that is known to be sensitive flowing it.

$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=
pm.txt -sensitive-inputs=sensargs.txt $1

$ more sensargs.txt 
low,foo,0
high,foo,1
high,bar,0

$ ./run.sh test21.bc bar 2>&1 | tee o.txt


Expected output:

High sensitive region created and named after bar and the low one created at callsite of foo and called after foo.

>>>> Found violations at  : 2 locations.
all high sym regions: 
bar_arg_0_0
offset=0, size=640
all low sym regions: 
foo_low_0_0
offset=0, size=32

KLEE: done: #Timing Side Channel  Locs = 2
KLEE: done: #Cache Side Channel  Locs = 0
KLEE: done: minInst = 80
KLEE: done: maxInst = 165
KLEE: done: HAncestors = 2
KLEE: done: HLMixedConstraints = 0
KLEE: done: HVars= 0
KLEE: done: LVars= 0
KLEE: done: total instructions = 842
KLEE: done: completed paths = 8
KLEE: done: generated tests = 8


