# Windows
God I hate compiling/linking on Windows. After much fighting and mangling of CMakeLists, I have the following process that works on my machine.

On Windows in terminal, starting from kioku-srs root directory. I used Git Bash with things like cmake and MSBuild in my PATH. This is currently only tested on Windows 7.
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

Some problems I ran into:
1. Building kioku and its deps 32 bit, but VC linking against 64 bit Windows DLLs. This is why I explicitly specify a 64 bit build on my 64 bit Windows 7 machine.
1. Failing to link at run time in Debug builds for VS2015 due to one Windows DLL not being redistributable. Not sure why this is. This is ultimately why the above explicitly states VS2013.
1. I had a lot of issues getting libgit2 to find libssh2, so I set the `-DEMBED_SSH_PATH` to compile it directly into the libgit2 binary. This may be an optimal approach since many platforms don't distribute libssh2 by default. I haven't yet linked libssh2 with openssl which is likely desirable, so that may present issues.

