
cmake -DCMAKE_CXX_COMPILER=/usr/bin/afl-g++ -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=<path-to-install-dir> -DDEV_PC=On -DPROFILING=Off -DJARSDIR=<path-to-gwt-compiler-jars> <path-to-C15-git>

make
