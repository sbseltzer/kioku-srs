# Building

This project uses CMake. This is for portability reasons. You will need it in order to build.

I'll be the first to admit that this build system is messy and inelegant, but it works, and that's what's important. Part of this comes from inexperience with leveraging CMake. Another part of this comes from inexperience building such a library with complex dependencies. And finally, the last part comes from my desire to keep things portable and consistent, which sometimes means not having the most optimal solution for each platform in the interest of minimizing maintenance.

If you are experienced in doing this and see it as a kluge, please, for the love of all that is good, tell me what I'm doing wrong and/or submit a PR. A big part of what makes this project special for me is that it's a learning experience.

Anyhow, for the time being, the dependencies such as `libgit2` and `libssh2` are built as static libraries with PIC (where applicable). This is because...  
1. It's easier.
2. They are uncommon on most systems (even Linux), so the costs of *not* making them dynamic libraries is virtually non-existent.
3. This is the most feasible solution for OSX, and consistency is a good thing.

Kioku itself, however, is built as a shared library. This is the reason we build the dependencies as static libraries with PIC. This is because Kioku has a number of unit tests that all link to it, so building those with static linkage would take more build time and disk space.

## Getting started

Start by cloning. This should work on Unix-like systems and Git Bash for Windows. All required dependencies are submodules, and will be included when you clone unless you forget the `--recursive` flag.
```bash
git clone --recursive https://github.com/seltzy/kioku-srs.git
cd kioku-srs
```

## Build Scripts for Linux, OSX, and Windows

Here's how to build in Unix-like environements. This assumes you have `git` and `cmake` installed and in your `PATH`.

The following should work out of the box on Unix-like platforms so long as `git` and `cmake` are installed. You will need CMake installed and added to your `PATH` environment variable (this is mostly pertinent on Windows).

Starting from the root of your cloned repository, run the build scripts. These could take a while to complete.

If you're on Linux, use the convenient building scripts which are also used in the Continuous Integration (CI).
```
./ci/linux/build-deps.sh
./ci/linux/build.sh
```
There is also a counterpart for OSX.
```
./ci/osx/build-deps.sh
./ci/osx/build.sh
```
There is also a counterpart for Windows (assuming you have both Visual Studio 2013 installed and CMake in your `PATH`).
```
./ci/windows/build-deps.bat
./ci/windows/build.bat
```

The results will be in the `build` folder. If you do encounter problems, please report it. These should automagically take care of the entire build process provided you have the required programs installed.

## Dependencies

### Currently Required
All required dependencies are included as submodules in the [extern](extern/) folder. Not all of these are in active use yet, but are expected to be. Most of them are just code that's directly included in Kioku, but a couple are libraries. Thankfully, the build scripts should prevent you from having any linker errors.

- [mongoose](https://github.com/cesanta/mongoose) - Portable embedded webserver and network stack.
- [libgit2](https://libgit2.github.com/) - This will be wrapped to create a filesystem database thingy. The rationale is for portable synchronization, file management, and deck version history.
- [libssh2](https://github.com/libssh2/libssh2.git) - This is a dependency of libgit2.
- [utf8.h](https://github.com/sheredom/utf8.h) - A portable single-header library for UTF-8 string functions.
- [intern](https://github.com/chriso/intern) - A simple string interning library.
- [parson](https://github.com/kgabis/parson) - Simplistic C89 JSON parser.
- [greatest](https://github.com/silentbicycle/greatest) - Header-only unit testing.
- [lua](https://lua.org) - I'm thinking of using this as a portable dynamic loader for backend tasks. This would also make it easier to embed with things like PHP. It could also serve as a nice configuration language, providing similar value to JSON.
- [tinydir](https://github.com/cxong/tinydir) - A portable directory/file reader in a single header.
- [generic-c-hashmap](https://github.com/Kijewski/generic-c-hashmap) - A portable generic easy-to-use hashmap in C. Header only. Very slick API.

## More on Windows

For [archived releases of Visual Studio](https://my.visualstudio.com/downloads/featured) (which you might need for a stable MSBuild) you will need to get a free [Developer Essentials](https://my.visualstudio.com/subscriptions) subscription.

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

# Possible Future Dependencies

Here are other libraries I'm thinking of using in the future. Sorted by use-case. This information isn't really relevant to building, but I can't think of a better place to keep this list right now.

#### Backend
- [curl](https://github.com/curl/curl) - This one should be obvious.
- [mbedtls](https://github.com/ARMmbed/mbedtls) - Appears to be the smallest, most portable SSL implementation, which would allow for HTTPS on mongoose, but libgit2 support is sparse.
- [openssl](https://github.com/openssl/openssl) - More common/accepted. This is currently the only supported libgit2 SSL implementation (though there's a feature in progress).
- [slre](https://github.com/cesanta/slre) - Super Light RegEx library. Portable. Gives a subset of Perl-style regex in C.
- [autoupdate](https://github.com/pmq20/libautoupdate) - A cross-platform in-place application updater in C. Mostly useful for inspiration.
- [sundown](https://github.com/vmg/sundown) - A portable, secure markdown processing library in C.
- [uuid4](https://github.com/rxi/uuid4) - A tiny library for generating random Universally Unique IDs. Necessary for conforming to JSON API.

#### Frontend
- [Cordova](https://cordova.apache.org/) - This is what PhoneGap is based on. This would be good for making the fat clients.
- [cordova-media-capture-plugin](https://github.com/apache/cordova-plugin-media-capture) - Might be best option for bringing media capture to mobile devices.
- [RecordRTC](https://github.com/muaz-khan/RecordRTC) - Media capture for modern browsers.
- [MediaStreamRecorder](https://github.com/streamproc/MediaStreamRecorder) - More media capture for modern browsers.
- [markdown-it](https://github.com/markdown-it/markdown-it) - A Javascript Markdown parser.
- [stackedit](https://github.com/benweet/stackedit) - An in-browser Markdown editor.
- [katex](https://github.com/Khan/KaTeX) - Fast Javascript LaTeX generator.
- [quill](https://quilljs.com/docs/formats/#inline) - Portable in-browser rich text editor. Actually comes with a formula module powered by KaTeX.

#### Scripting
- [v7](https://github.com/cesanta/v7) - Embedded Portable Javascript engine with Lua-like binding API. V7 is recently deprecated, but has a much broader feature set. mJS is so stripped down that it doesn't even look like JS.
- [pypy](http://doc.pypy.org/en/latest/embedding.html) - This seems to be a good sandboxed, portable, embedded python solution.
- [lunatic-python](https://labix.org/lunatic-python) - This looks very promising as a best-of-both-worlds thing.
- [luagit2](https://github.com/libgit2/luagit2) - Lua bindings for libgit2. Why not?
- [luacurl](http://luacurl.luaforge.net/) - Platform independent, apparently. Very simple looking.

#### Misc Server Stuff
- [hotel](https://github.com/typicode/hotel) - This looks promising for developing a client or server.
- [php-src](https://github.com/php/php-src) - The PHP source code for compiling the interpreter and CGI.
- [php-lua](https://github.com/laruence/php-lua) - A PHP PECL package for embedding a Lua interpreter in PHP.

It's worth noting that anything with a viral license is not a problem so long as it is wrapped appropriately. The end-goal is to have some reusable interfaces so that Kioku can be an SRS implementation framework.

### Companion APIs
These would be an immense help in giving users an opportunity to sync their data to private and public sources without needing to know how to create and manage Git repositories.

- [GitLab API](http://doc.gitlab.com/ce/api/)
- [GitHub API](https://developer.github.com/v3/)
- [BitBucket API](https://developer.atlassian.com/bitbucket/api/2/reference/)
