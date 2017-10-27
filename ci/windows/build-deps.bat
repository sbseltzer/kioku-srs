rem Initialize variables
set build_dir=%CD%
set result=0
set lib_ext=lib
set build_type=Visual Studio 15 2017 Win64
set build_conf=Debug

echo "Start Directory: %build_dir%"
echo "Build Type: %build_type%"

rem BUILD LIBSSH2
cd %build_dir%\extern\libssh2
rem Cleanup directory just in case there's something funky left behind
rm -rf CMakeFiles CMakeCache.txt
rem Attempt to create build dir
mkdir build
rem Attempt to go to build dir and clear it out if it has anything in it.
cd build 
rm -rf *
rem Build the project
:try_cmake_gen
cmake .. -G"%build_type%" -DBUILD_SHARED_LIBS=OFF -DBUILD_EXAMPLES=OFF
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
ls src

rem BUILD LIBGIT2
cd %build_dir%\extern\libgit2
rem Delete CMake generated files that could screw up CMake output location
rm -rf CMakeFiles CMakeCache.txt
rem Attempt to create build dir
mkdir build
rem Attempt to go to build dir and clear it out if it has anything in it.
cd build
rm -rf *
rem Build the project
cmake .. -G"%build_type%" -DBUILD_CLAR=OFF -DBUILD_SHARED_LIBS=OFF -DLIBSSH2_FOUND=YES -DLIBSSH2_INCLUDE_DIRS:PATH=%build_dir%\extern\libssh2\include;%build_dir%\extern\libssh2\build\src -DLIBSSH2_LIBRARY_DIRS:PATH=%build_dir%\extern\libssh2\build\src\%build_conf% -DLIBSSH2_LIBRARIES=libssh2.%lib_ext%
cmake --build .
ls

rem Check whether the libraries were built
rem if ! test -f %build_dir%\extern\libssh2\build\src\libssh2.%lib_ext% ; then
rem     printf '%s\n' 'Build: Failed to build libssh2!' >&2
rem     result=1
rem elif ! test -f %build_dir%\extern\libgit2\build\libgit2.%lib_ext% ; then
rem     printf '%s\n' 'Build: Failed to build libgit2!' >&2
rem     result=1
rem fi
cd %build_dir%
exit /b %result%
