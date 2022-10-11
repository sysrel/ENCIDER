This test (test1.c) is the running example in the paper: 

Tuba Yavuz, Farhaan Fowze, Grant Hernandez, Ken Yihang Bai, Kevin Butler, 
and Dave Jing Tian. ENCIDER: Detecting Timing and Cache Side Channels in 
SGX Enclaves and Cryptographic APIs, in IEEE Transactions on Dependable 
and Secure Computing, doi: 10.1109/TDSC.2022.3160346. 

Assuming the environment variable ENCIDER is set as follows:

$ export ENCIDER=path_to_klee_executable_under_bin_under_build_directory

$ more run.sh 
$ENCIDER  -entry-point=$2 -sensitive-inputs=sensargs.txt $1

where $1 represents the LLVM bitcode to be analyzed and $2 represents the 
function, e.g., main, to be analyzed

$ more sensargs.txt 
low,foo,0
high,foo,1

which states that the first parameter of function foo (arg no 0) is a low 
security sensitive value and the second parameter of function foo (arg no 1) 
is a high security sensitive value


To run:
  ./run.sh test1.bc foo 2>/dev/null 1>/dev/null

ENCIDER generates a variety of files that reposrt specific side channels it 
finds. For this example, secret dependent branches can be viewed as 

$ more secretDependentBranch_time.txt
Time=0.0094291
Instruction:   br i1 %5, label %7, label %21, !dbg !125

Location: /home/tuba/Documents/tools/pmguidedsymex/test/test1/test1.c 20


Time=0.0574851
Instruction:   br i1 %25, label %28, label %35, !dbg !156

Location: /home/tuba/Documents/tools/pmguidedsymex/test/test1/test1.c 36

timing side channels along with resource (number of LLVM instructions per 
path) differences can be found as 

$ more timingSideChannels_time.txt
Time=0.138402
resource ranges: [11,11][14,14]
Instruction:   br i1 %5, label %7, label %21, !dbg !125

Location: /home/tuba/Documents/tools/pmguidedsymex/test/test1/test1.c:20

...
