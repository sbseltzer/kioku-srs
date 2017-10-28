#!/bin/sh -x
# Initialize variables
start_dir=$PWD
result=0
: ${build_conf:="Release"}
: ${clean_first:="yes"}
: ${TRAVIS_BUILD_DIR:=$start_dir}
build_dir=$TRAVIS_BUILD_DIR
build_type="Unix Makefiles"
: ${build_shared:=ON}
if ! "x$build_shared" == "xON"; then
    lib_ext="so"
else
    lib_ext="a"
fi

echo "Start dir: $start_dir"
echo "Build dir: $build_dir"

##### BUILD LIBSSH2
cd $build_dir/extern/libssh2

# Delete CMake generated files that could screw up the build
if "x$clean_first" == "xyes" ; then
    make clean
    rm -rf CMakeFiles CMakeCache.txt build/
fi
mkdir build
cd build

# Build the project
cmake .. -G"$build_type" -DCMAKE_CXXFLAGS="-fPIC" -DCMAKE_C_FLAGS="-fPIC" -DCMAKE_INSTALL_PREFIX:PATH=$build_dir/extern/libssh2/build/src -DBUILD_SHARED_LIBS=$build_shared
cmake --build . --config $build_conf
cmake --build . --target install
ls src

##### BUILD LIBGIT2
cd $build_dir/extern/libgit2

# Delete CMake generated files that could screw up the build
if "x$clean_first" == "xyes" ; then
    make clean
    rm -rf CMakeFiles CMakeCache.txt build/
fi
mkdir build
cd build

# Build the project
PKG_CONFIG_PATH=$build_dir/extern/libssh2/build/src cmake .. -G"$build_type" -DCMAKE_CXXFLAGS="-fPIC" -DCMAKE_C_FLAGS="-fPIC" -DBUILD_CLAR=OFF -DBUILD_SHARED_LIBS=$build_shared
cmake --build . --config $build_conf
ls

# Check whether the libraries were built
if ! test -f $build_dir/extern/libssh2/build/src/libssh2.$lib_ext ; then
    printf '%s\n' 'Build: Failed to build libssh2!' >&2
    result=1
elif ! test -f $build_dir/extern/libgit2/build/libgit2.$lib_ext ; then
    printf '%s\n' 'Build: Failed to build libgit2!' >&2
    result=1
fi
cd $start_dir
exit $result
