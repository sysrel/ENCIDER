ENCIDER: A Tool for Side Channel Analysis
===========================================================================================

ENCIDER is a tool for detecting software side channels including timing and cache side 
channels. It supports SGX enclave analysis as well. ENCIDER is built on top of the KLEE 
Symbolic Virtual Machine to perform API model guided symbolic execution at the component 
level. It is built on top of an earlier version of PROMPT.

ENCIDER can be installed according to the instructions for KLEE's LLVM-3.8 build instructions 
(available in INSTALL.TXT). Check out the test directory for example code and scripts that 
utilize various options.


If you use ENCIDER, please use the following paper to cite it:

```
@article{YFH2022,
 title={ENCIDER: Detecting Timing and Cache Side Channels in SGX Enclaves and Cryptographic APIs},
 author={Tuba Yavuz, Farhaan Fowze, Grant Hernandez, Ken Yihang Bai, Kevin Butler, Dave Jing Tian},
 journal={IEEE Transactions on Dependable and Secure Computing},
 year={2022},
 publisher={IEEE}
}

```
