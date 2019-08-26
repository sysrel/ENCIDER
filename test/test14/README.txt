Testing getting sensitive input from the environment through an input function and the parameter type being a struct type

$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=pm.txt -sens
itive-inputs=sensargs.txt -sensitive-type-region=senstypes.txt -input-funcs=inputFunctions.txt $1

$ more sensargs.txt 
mixed,bar,0

$ more senstypes.txt 
low,%struct.mytest,32,32
high,%struct.mytest,64,32

$ more inputFunctions.txt 
bar,0


$ ./run.sh test14.bc foo 2>&1 | tee o.txt 

Expected output:

$ grep "Found V" o.txt | wc -l
11





