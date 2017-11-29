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
: ${build_server:=ON}
: ${build_lua:=ON}
: ${install_prefix:="$build_dir/extern/install"}
if test "x$build_shared" = "xON"; then
    lib_ext="so"
else
    lib_ext="a"
fi

echo "Start dir: $start_dir"
echo "Build dir: $build_dir"

# Delete CMake generated files that could screw up the build
if test "x$clean_first" = "xyes"; then
    make clean
    rm -rf CMakeFiles CMakeCache.txt build/
fi
mkdir build
cd build

rm *.so*
rm *.a
if test "x$build_shared" = "xON"; then
  # Copy libraries over
  cp $build_dir/extern/install/lib/*.$lib_ext* .
  #Check whether they were copied
  if ! test -f libgit2.$lib_ext ; then
      printf '%s\n' 'Build: Failed to copy libgit2!' >&2
      result=1
  elif ! test -f libssh2.$lib_ext ; then
      printf '%s\n' 'Build: Failed to copy libssh2!' >&2
      result=1
  fi
fi
PKG_CONFIG_PATH=$install_prefix/lib/pkgconfig:$install_prefix/lib64/pkgconfig cmake .. -G"$build_type" -DBUILD_SHARED_LIBS=$build_shared -DCMAKE_BUILD_TYPE=$build_conf -DBUILD_SERVER=$build_server -DBUILD_LUA=$build_lua
cmake --build . --config $build_conf
CTEST_OUTPUT_ON_FAILURE=1 ctest -C $build_conf --timeout 5
result=$?
if "$result" != "0"; then
    cat Testing/Temporary/LastTest.log
fi
cd $start_dir
exit $result
