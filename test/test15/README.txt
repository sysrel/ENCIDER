Tests void type pointer arg with mixed sensitivity.


$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=pm.txt -sensitive-inputs=sensargs.txt -sensitive-type-region=sen
stypes.txt -void-type-casts=voidcasts.txt -func-arg-type-hints=funcargtypehints.txt $1

$ more sensargs.txt 
mixed,foo,0

$ more senstypes.txt 
low,%struct.mytest,32,32
high,%struct.mytest,64,32

$ more voidcasts.txt 
%struct.mytest

$ more funcargtypehints.txt 
foo,0,%struct.mytest

$ ./run.sh test15.bc foo 2>&1 | tee o.txt

Expected output:

$ grep "Found V" o.txt | wc -l
11


