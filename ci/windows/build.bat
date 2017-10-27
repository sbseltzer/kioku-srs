rem Initialize variables
set build_dir=%CD%
set result=0
set lib_ext=lib
set build_type=Visual Studio 12 2013 Win32

echo "Start Directory: %build_dir%"
echo "Build Type: %build_type%"

rem Delete CMake generated files that could screw up CMake output location
rm -rf CMakeFiles CMakeCache.txt
rem Attempt to create build dir
mkdir build
rem Attempt to go to build dir and clear it out if it has anything in it.
cd build
rm -rf *
rem Copy libraries over (uncomment if/when we enable building of shared libraries)
rem copy /V /B /Y /Z %build_dir%\\extern\\libssh2\\build\\src\\*.%lib_ext%* .
rem copy /V /B /Y /Z %build_dir%\\extern\\libgit2\\build\\*.%lib_ext%* .
rem Check whether they were copied
rem if ! test -f libgit2.%lib_ext% ; then
rem     printf '%s\n' 'Build: Failed to copy libgit2!' >&2
rem     result=1
rem elif ! test -f libssh2.%lib_ext% ; then
rem     printf '%s\n' 'Build: Failed to copy libssh2!' >&2
rem     result=1
rem else
cmake .. -G"$build_type"
cmake --build .
set CTEST_OUTPUT_ON_FAILURE=1
set result=
ctest -C "Debug"
rem fi
cd %build_dir%
exit /b %result%