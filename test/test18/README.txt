
Testing timing models. test18.c is a minor modification of test16.c to show that timing model of APIs take effect.

$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=pm.txt -sens
itive-inputs=sensargs.txt -info-flow-models=infoFlow.txt -timing-models=timingModels.txt $1

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

$ more timingModels.txt 
_mm_fake1,5
_mm_fake2,10
_mm_fake3,15
_mm_fake4,20
_mm_fake5,25

$./run.sh test18.bc foo 2>&1 | tee o.txt

Expected output: The diff value is 


Found Violation at : 
diff : 57
Instruction:
  %1 = alloca i32, align 4

	>>>> /home/tuba/Documents/tools/pmguidedsymex/test/test18/test18.c:26

[ (And (Eq false
          (Slt 0
               (ReadLSB w32 0 %sym13__mm_fake1_0)))
      (Eq false
          (Slt 0
               (ReadLSB w32 0 %sym21__mm_fake3_0))))
(Eq 1
     (ReadLSB w32 0 return_value))
 ]   ---->  [ 255 , 255 ]

[ (And (Slt 0
           (ReadLSB w32 0 %sym13__mm_fake1_0))
      (Slt 0
           (ReadLSB w32 0 %sym21__mm_fake3_0)))
(Eq 1
     (ReadLSB w32 0 return_value))
 ]   ---->  [ 198 , 198 ]

Successors : 

	>>>> /home/tuba/Documents/tools/pmguidedsymex/test/test18/test18.c:33
  %27 = load i32, i32* %2, align 4, !dbg !151

foo

	>>>> /home/tuba/Documents/tools/pmguidedsymex/test/test18/test18.c:44
  %39 = load i32, i32* %L5, align 4, !dbg !169

foo

