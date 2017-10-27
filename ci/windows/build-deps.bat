rem Initialize variables
set build_dir=%CD%
set result=0
set lib_ext=lib
set build_type=Visual Studio 12 2013 Win32

echo "Start Directory: %build_dir%"
echo "Build Type: %build_type%"

rem BUILD LIBSSH2
cd %build_dir%\\extern\\libssh2
rem Cleanup directory just in case there's something funky left behind
rm -rf CMakeFiles CMakeCache.txt
rem Attempt to create build dir
mkdir build
rem Attempt to go to build dir and clear it out if it has anything in it.
cd build && make clean && rm -rf *
rem Build the project
cmake .. -G"%build_type%" -DCMAKE_INSTALL_PREFIX:PATH=%build_dir%\\extern\\libssh2\\build\\src -DBUILD_SHARED_LIBS=ON
cmake --build .
cmake --build . --target install
ls src

rem BUILD LIBGIT2
cd %build_dir%\\extern\\libgit2
rem Delete CMake generated files that could screw up CMake output location
rm -rf CMakeFiles CMakeCache.txt
rem Attempt to create build dir
mkdir build
rem Attempt to go to build dir and clear it out if it has anything in it.
cd build && rm -rf *
rem Build the project
PKG_CONFIG_PATH=%build_dir%\\extern\\libssh2\\build\\src cmake .. -G"%build_type%" -DBUILD_CLAR=OFF -DBUILD_SHARED_LIBS=ON
cmake --build .
ls

rem Check whether the libraries were built
rem if ! test -f %build_dir%\\extern\\libssh2\\build\\src\\libssh2.%lib_ext% ; then
rem     printf '%s\n' 'Build: Failed to build libssh2!' >&2
rem     result=1
rem elif ! test -f %build_dir%\\extern\\libgit2\\build\\libgit2.%lib_ext% ; then
rem     printf '%s\n' 'Build: Failed to build libgit2!' >&2
rem     result=1
rem fi
cd %build_dir%
exit /b %result%