# Windows
God I hate compiling/linking on Windows. After much fighting and mangling of CMakeLists, I have the following process that works on my machine.

On Windows in terminal, starting from kioku-srs root directory. I used Git Bash with things like cmake and MSBuild in my PATH.
```
git submodule update --init
mkdir build
mkdir extern/libgit2/build
cd extern/libgit2/build
cmake .. -G "Visual Studio 12 2013 Win64" -DEMBED_SSH_PATH=/full/path/to/kioku/directory/extern/libssh2/
MSBuild.exe *sln
cd ../../../build
cmake -G "Visual Studio 12 2013 Win64" ..
MSBuild.exe *sln
cp ../extern/libgit2/Debug/* Debug/
```
