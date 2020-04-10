Tests the loop bound functionality when the loop condition is symbolic. We have 
modified test1.c to call the foo function that has the timing side channel on the 
last iteration (15th) of the loop. Since the loop condition is symbolic, in both cases 
it detects the side channel.

We have tow scripts: run.sh and run15.sh

How to run case 1 (detetcting the time side channel despite the bound):

$ ./run.sh test26.bc bar 2>&1 | tee o.txt

$ more run.sh

/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init
=true -prog-model-spec=pm.txt -sensitive-inputs=sensargs.txt -loop-bound=2  $1

What to expect:

$ grep -r "Found V" o.txt | wc -l
12

Statistics: 

KLEE: done: #Timing Side Channel  Locs = 1
KLEE: done: #Cache Side Channel  Locs = 0
KLEE: done: minInst = 10
KLEE: done: maxInst = 98
KLEE: done: HAncestors = 4
KLEE: done: secretDependentUniqueObser = 6
KLEE: done: HLeaksOnStack = 1
KLEE: done: HLMixedConstraints = 0
KLEE: done: HVars= 0
KLEE: done: LVars= 0
KLEE: done: secretDepStates= 16
KLEE: done: secretIndepStates= 2
KLEE: done: total instructions = 964
KLEE: done: completed paths = 18
KLEE: done: generated tests = 18

How to run case 2 (detecting the time side channel with more paths):

$ ./run15.sh test26.bc bar 2>&1 | tee o15.txt

$ more run15.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init
=true -prog-model-spec=pm.txt -sensitive-inputs=sensargs.txt -loop-bound=15  $1

What to expect: Time side channels found

$ grep -r "Found V" o15.txt | wc -l
165

Satistics:

KLEE: done: #Timing Side Channel  Locs = 2
KLEE: done: #Cache Side Channel  Locs = 0
KLEE: done: minInst = 10
KLEE: done: maxInst = 318
KLEE: done: HAncestors = 30
KLEE: done: secretDependentUniqueObser = 108
KLEE: done: HLeaksOnStack = 1
KLEE: done: HLMixedConstraints = 0
KLEE: done: HVars= 0
KLEE: done: LVars= 0
KLEE: done: secretDepStates= 120
KLEE: done: secretIndepStates= 2
KLEE: done: total instructions = 13632
KLEE: done: completed paths = 122
KLEE: done: generated tests = 122
