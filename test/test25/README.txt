Tests the loop bound functionality when the loop condition is concrete. We have 
modified test1.c to call the foo function that has the timing side channel on the 
last iteration (15th) of the loop. So setting the bound < 15 would miss the timing 
side channel.

We have tow scripts: run.sh and run15.sh

How to run case 1 (missing the time side channel):

$ ./run.sh test25.bc bar 2>&1 | tee o.txt

$ more run.sh

/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init
=true -prog-model-spec=pm.txt -sensitive-inputs=sensargs.txt -loop-bound=2  $1

What to expect:

No side channels found

Statistics: 

KLEE: done: #Timing Side Channel  Locs = 0
KLEE: done: #Cache Side Channel  Locs = 0
KLEE: done: minInst = 30
KLEE: done: maxInst = 31
KLEE: done: HAncestors = 0
KLEE: done: secretDependentUniqueObser = 0
KLEE: done: HLeaksOnStack = 0
KLEE: done: HLMixedConstraints = 0
KLEE: done: HVars= 0
KLEE: done: LVars= 0
KLEE: done: secretDepStates= 0
KLEE: done: secretIndepStates= 1
KLEE: done: total instructions = 31
KLEE: done: completed paths = 1
KLEE: done: generated tests = 1

How to run case 2 (detecting the time side channel):

$ ./run15.sh test25.bc bar 2>&1 | tee o15.txt

$ more run15.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init
=true -prog-model-spec=pm.txt -sensitive-inputs=sensargs.txt -loop-bound=15  $1

What to expect: Time side channels found

$ grep -r "Found V" o15.txt | wc -l
11

Satistics:

KLEE: done: #Timing Side Channel  Locs = 2
KLEE: done: #Cache Side Channel  Locs = 0
KLEE: done: minInst = 233
KLEE: done: maxInst = 318
KLEE: done: HAncestors = 2
KLEE: done: secretDependentUniqueObser = 8
KLEE: done: HLeaksOnStack = 1
KLEE: done: HLMixedConstraints = 0
KLEE: done: HVars= 0
KLEE: done: LVars= 0
KLEE: done: secretDepStates= 7
KLEE: done: secretIndepStates= 1
KLEE: done: total instructions = 1051
KLEE: done: completed paths = 8
KLEE: done: generated tests = 8
