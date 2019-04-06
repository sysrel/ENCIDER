cmake \
  -DENABLE_SOLVER_STP=ON \
  -DENABLE_POSIX_RUNTIME=ON \
  -DENABLE_KLEE_UCLIBC=ON \
  -DKLEE_UCLIBC_PATH=/home/tuba/Documents/tools/pmguidedsymex/klee-uclibc \
  -DENABLE_SYSTEM_TESTS=OFF \
  -DENABLE_UNIT_TESTS=OFF \
  -DLLVM_CONFIG_BINARY=/usr/lib/llvm-3.8/bin/llvm-config \
  -DLLVMCC=/usr/bin/clang-3.8 \
  -DLLVMCXX=/usr/bin/clang++-3.8 \
  /home/tuba/Documents/tools/pmguidedsymex/klee
