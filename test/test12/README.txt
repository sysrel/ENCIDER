Testing error reporting when type based sensitivity region is not defined for a data structure of mixed sensitivity

$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=pm.txt -sens
itive-inputs=sensargs.txt  $1

$ more sensargs.txt 
mixed,foo,0


Expected output:

$ ./run.sh test12.bc foo 2>&1 | tee o.txt
KLEE: output directory is "/home/tuba/Documents/tools/pmguidedsymex/test/test12/klee-out-1"
KLEE: Using STP solver backend
Uncov init=821
Couldn't open file 
KLEE: WARNING: undefined reference to function: printf
klee: /home/tuba/Documents/tools/pmguidedsymex/klee/lib/Core/Executor.cpp:5971: void klee::Executor::initArgsAsSymbolic(klee::ExecutionState&, llvm::Function*, bool&, bool): Assertion `false && "Mixed sensitivity arg without type based sensitive regions!\n"' failed.

