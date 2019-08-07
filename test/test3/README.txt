Testing complex data structure where the sensitive values are in the fields of the function args

$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=pm.txt -sens
itive-inputs=sensargs.txt -sensitive-type-region=senstypes.txt $1

$ more sensargs.txt 
low,foo,0
high,foo,1

$ more senstypes.txt 
low,%struct.public_t,0,32
high,%struct.secret_t,0,32

$ ./run.sh test3.bc foo 2>&1 | tee o.txt

Expected output:
$ grep "Found V" o.txt  | wc -l
11

