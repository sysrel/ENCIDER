This test (test3.c) is a modified version of the running example in the paper: 

Tuba Yavuz, Farhaan Fowze, Grant Hernandez, Ken Yihang Bai, Kevin Butler, 
and Dave Jing Tian. ENCIDER: Detecting Timing and Cache Side Channels in 
SGX Enclaves and Cryptographic APIs, in IEEE Transactions on Dependable 
and Secure Computing, doi: 10.1109/TDSC.2022.3160346. 

Assuming the environment variable ENCIDER is set as follows:

$ export ENCIDER=path_to_klee_executable_under_bin_under_build_directory

$ more run.sh 
$ENCIDER  -entry-point=$2 -sensitive-inputs=sensargs.txt -sensitive-type-region=senstypes.txt $1

where $1 represents the LLVM bitcode to be analyzed and $2 represents the 
function, e.g., main, to be analyzed

$ more sensargs.txt 
low,foo,0
high,foo,1

which states that the first parameter of function foo (arg no 0) is a low 
security sensitive value and the second parameter of function foo (arg no 1) 
is a high security sensitive value

Since the first and second parameters of foo are struct types, we can be specific 
about the fields of those struct types that are low/high security sensitive:

$ more senstypes.txt
low,%struct.public_t,0,32
high,%struct.secret_t,0,32

To run:
  ./run.sh test3.bc foo 2>/dev/null 1>/dev/null

ENCIDER generates a variety of files that reposrt specific side channels it 
finds. For this example, secret dependent branches can be viewed as 

$ more secretDependentBranch_time.txt
Time=0.010828
Instruction:   br i1 %6, label %9, label %24, !dbg !133

Location: /home/tuba/Documents/tools/pmguidedsymex/test/test3/test3.c 28


Time=0.069104
Instruction:   br i1 %29, label %33, label %40, !dbg !164

Location: /home/tuba/Documents/tools/pmguidedsymex/test/test3/test3.c 44

timing side channels along with resource (number of LLVM instructions per 
path) differences can be found as 

$ more timingSideChannels_time.txt
Time=0.136071
resource ranges: [15,15][19,19]
Instruction:   br i1 %6, label %9, label %24, !dbg !133

Location: /home/tuba/Documents/tools/pmguidedsymex/test/test3/test3.c:28

...
