#!/bin/sh -x
# Initialize variables
start_dir=$PWD
result=0
: ${lib_ext:="a"}
: ${build_conf:="Release"}
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
cmake .. -G"$build_type" -DBUILD_SHARED_LIBS=OFF
cmake --build . --config $build_conf
CTEST_OUTPUT_ON_FAILURE=1 ctest -C $build_conf
result=$?
cd $start_dir
exit $result
