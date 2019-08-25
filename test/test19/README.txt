Tests sensitive leaks on the stack.

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

$ ./run.sh test19.bc foo 2>&1 | tee o.txt 

Expected output:
 
  report on H3 and H4 staying on the stack! 
  Note that the high secret values flow into these local variables 
  through the _mm_fake3 and _mm_fake4 APIs.


$ more sensitiveLeaksOnStack.txt 
/home/tuba/Documents/tools/pmguidedsymex/test/test19/test19.c 66: (ReadLSB w32 0 %sym21__mm_fake3_0)

/home/tuba/Documents/tools/pmguidedsymex/test/test19/test19.c 66: (ReadLSB w32 0 %sym25__mm_fake4_0)

/home/tuba/Documents/tools/pmguidedsymex/test/test19/test19.c 66: (ReadLSB w32 0 foo_arg_1_0)
