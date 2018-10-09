../klee_build_dir/bin/klee -dump-states-on-halt=false -max-time=100 -entry-point=usbtv_probe -framework-dts=dts_usbtv.txt  $1
