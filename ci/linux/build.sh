#!/bin/sh
mkdir build
cd build
cp $CI_PROJECT_DIR/extern/libgit2/build/*.so* .
test -f libgit2.so
cmake .. -G"Unix Makefiles"
make
CTEST_OUTPUT_ON_FAILURE=1 make test
return $?
