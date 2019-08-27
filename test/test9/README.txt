Testing functions that return inputs from the environment via their arguments

$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=pm.txt -sens
itive-inputs=sensargs.txt -input-funcs=inputFunctions.txt $1

$ more sensargs.txt 
$

$ more inputFunctions.txt 
low,bar,0
high,bar,1

./run.sh test9.bc foo 2>&1 | tee o.txt 

Expected output:

$ grep "Found V" o.txt | wc -l
11

