/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=pm.txt -sensitive-inputs=sensargs.txt -input-funcs=inputFunctions.txt $1
