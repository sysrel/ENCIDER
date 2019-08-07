
Testing different public output (return values)
$ more run.sh
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=pm.txt -sens
itive-inputs=sensargs.txt $1

$ more sensargs.txt 
low,foo,0
high,foo,1

$ ./run.sh test2.bc foo 2>&1 | tee o.txt

Expected output:

$ grep "Found V" o.txt  | wc -l
6

