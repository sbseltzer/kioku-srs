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
: ${install_prefix:="$build_dir/extern/install"}
rm -rf $install_prefix
mkdir $install_prefix
fpic_flags="-DCMAKE_CXXFLAGS=-fPIC -DCMAKE_C_FLAGS=-fPIC"

if test "x$build_shared" = "xON"; then
    lib_ext="so"
else
    lib_ext="a"
fi

echo "Start dir: $start_dir"
echo "Build dir: $build_dir"

##### BUILD LIBSSH2
cd $build_dir/extern/libssh2

# Delete CMake generated files that could screw up the build
if test "x$clean_first" = "xyes"; then
    make clean
    rm -rf CMakeFiles CMakeCache.txt build/
fi
mkdir build
cd build

# Build the project
cmake .. -G"$build_type" $fpic_flags -DCMAKE_INSTALL_PREFIX:PATH=$install_prefix -DBUILD_SHARED_LIBS=$build_shared  -DCMAKE_BUILD_TYPE=$build_conf
cmake --build . --config $build_conf
cmake --build . --target install
ls $install_prefix

##### BUILD LIBGIT2
cd $build_dir/extern/libgit2

# Delete CMake generated files that could screw up the build
if test "x$clean_first" = "xyes"; then
    make clean
    rm -rf CMakeFiles CMakeCache.txt build/
fi
mkdir build
cd build

# Build the project
PKG_CONFIG_PATH=$install_prefix/lib/pkgconfig cmake .. -G"$build_type" $fpic_flags -DCMAKE_INSTALL_PREFIX:PATH=$install_prefix -DBUILD_SHARED_LIBS=$build_shared -DBUILD_CLAR=OFF -DCMAKE_BUILD_TYPE=$build_conf
cmake --build . --config $build_conf
cmake --build . --target install
ls $install_prefix

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
