/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=pm.txt -sensitive-inputs=sensargs.txt -sensitive-type-region=senstypes.txt -void-type-casts=voidcasts.txt -func-arg-type-hints=funcargtypehints.txt $1