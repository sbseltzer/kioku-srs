rem Initialize variables
set build_dir=%CD%
set result=0
IF "%build_type%" == "" (
  set build_type=Visual Studio 15 2017 Win64
)
IF "%build_conf%" == "" (
  set build_conf=Release
)
IF "%shared%" == "" (
  set shared=ON
)
IF "%shared%" == "ON" (
  set lib_ext=dll
) ELSE (
  set lib_exit=lib
)

echo "Start Directory: %build_dir%"
echo "Build Type: %build_type%"

rem BUILD LIBSSH2
cd %build_dir%\extern\libssh2
rem Cleanup directory just in case there's something funky left behind
mv build build.old
mv CMakeFiles CMakeFiles.old
call "cmd /c start rm -rf build.old CMakeFiles.old CMakeCache.txt"
rem Attempt to create build dir
mkdir build
rem Attempt to go to build dir and clear it out if it has anything in it.
cd build 
rem Build the project
:try_cmake_gen
cmake .. -G"%build_type%" -DBUILD_SHARED_LIBS=%shared% -DBUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=%build_dir%\extern\install
if not "%errorlevel%"=="0" (
   rm -rf *
   set try_build_type=Visual Studio 12 2013 Win64
   if not "%build_type%"=="%try_build_type%" (
      set build_type=%try_build_type%
      goto try_cmake_gen
   ) else (
      goto end
   )
)
cmake --build . --config %build_conf%
rem Install it
cmake --build . --config %build_conf% --target install
ls src
ls %build_dir%\extern\install

rem Check whether the libraries were built
if NOT EXIST %build_dir%\extern\libssh2\build\src\%build_conf%\libssh2.%lib_ext% (
   echo Build: Failed to build libssh2!
   set result=1
   goto end
)

rem BUILD LIBGIT2
cd %build_dir%\extern\libgit2
rem Delete CMake generated files that could screw up CMake output location
mv build build.old
mv CMakeFiles CMakeFiles.old
call "cmd /c start rm -rf build.old CMakeFiles.old CMakeCache.txt"
rem Attempt to create build dir
mkdir build
rem Attempt to go to build dir and clear it out if it has anything in it.
cd build
rem Build the project
cmake .. -G"%build_type%" -DBUILD_CLAR=OFF -DBUILD_SHARED_LIBS=%shared% -DLIBSSH2_FOUND=YES -DLIBSSH2_INCLUDE_DIRS:PATH=%build_dir%\extern\install\include;%build_dir%\extern\libssh2\build\src -DLIBSSH2_LIBRARY_DIRS:PATH=%build_dir%\extern\install\lib\;%build_dir%\extern\libssh2\build\src\%build_conf% -DLIBSSH2_LIBRARIES=libssh2.%lib_ext%  -DCMAKE_INSTALL_PREFIX=%build_dir%\extern\install
cmake --build . --config %build_conf%
rem Install it
cmake --build . --config %build_conf% --target install
ls
ls %build_dir%\extern\install

rem Check whether the libraries were built
if NOT EXIST %build_dir%\extern\libgit2\build\%build_conf%\git2.%lib_ext% (
   echo Build: Failed to build libgit2!
   set result=1
   goto end
)

:end
cd %build_dir%
exit /b %result%
