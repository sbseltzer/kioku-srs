rem Initialize variables
set build_dir=%CD%
set result=0
set lib_ext=lib
set build_type=Visual Studio 15 2017 Win64
set build_conf=Debug

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
:try_cmake_gen
cmake .. -G"%build_type%"
if not "%errorlevel%"=="0" (
   rm -rf *
   set try_build_type=Visual Studio 12 2013 Win64
   if not "%build_type%"=="%try_build_type%" (
      set build_type=%try_build_type%
      goto try_cmake_gen
   ) else (
      cd %build_dir%
   )
)
cmake --build .
set CTEST_OUTPUT_ON_FAILURE=1
set result=
ctest -C"%build_conf%"
rem fi
cd %build_dir%
exit /b %result%
