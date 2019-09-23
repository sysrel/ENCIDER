Tests a simple info flow summarization with collecting semantics from two paths and multilevel access.

$ ./run.sh test23.bc foo 2>&1 | tee o.txt

$ more run.sh 
/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/bin/klee  -entry-point=$2 -lazy-init=true -prog-model-spec=
pm.txt -infoflow-summary=true $1

$ more pm.txt
more: stat of pm.txt failed: No such file or directory
(Means no programming model provided)

Expected output:

Info Flow Summary for function foo
{0,[0:32]}
	===>
1,[0:64][0:32]

{0,[0:32]}
	===>
3,[0:64][0:32]

{2,[0:32]}
	===>
5,[0:32]

