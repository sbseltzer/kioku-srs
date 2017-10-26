#!/bin/sh -x
# Initialize variables
start_dir=$PWD
result=0
: ${lib_ext:="a"}
: ${TRAVIS_BUILD_DIR:=$start_dir}
build_dir=$TRAVIS_BUILD_DIR
build_type="Xcode"

echo "Start dir: $start_dir"
echo "Build dir: $build_dir"

# Delete CMake generated files that could screw up CMake output location
make clean
rm -rf CMakeFiles CMakeCache.txt
# Attempt to create build dir
mkdir build
# Attempt to go to build dir and clear it out if it has anything in it.
cd build && make clean && rm -rf *
# Copy libraries over
cp $build_dir/extern/libssh2/build/src/*.$lib_ext* .
cp $build_dir/extern/libgit2/build/*.$lib_ext* .
# Check whether they were copied
if ! test -f libgit2.$lib_ext ; then
    printf '%s\n' 'Build: Failed to copy libgit2!' >&2
    result=1
elif ! test -f libssh2.$lib_ext ; then
    printf '%s\n' 'Build: Failed to copy libssh2!' >&2
    result=1
else
    cmake .. -G"$build_type"
    cmake --build .
    CTEST_OUTPUT_ON_FAILURE=1 ctest
    result=$?
fi
cd $start_dir
return $result
