#!/bin/sh
cd extern/libgit2
mkdir build
cd build
cmake .. -G"Unix Makefiles" -DEMBED_SSH_PATH=$CI_PROJECT_DIR/extern/libssh2/ -DBUILD_CLAR=OFF
make
test -f libgit2.so
return $?
