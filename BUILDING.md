# Building

This project uses CMake. This is for portability reasons. You will need it in order to build.

## Getting started

Start by cloning. This should work on Unix-like systems and Git Bash for Windows. All required dependencies are submodules, and will be included when you clone unless you forget the `--recursive` flag.
```bash
git clone --recursive https://github.com/seltzy/kioku-srs.git
cd kioku-srs
```

### Dynamic vs Static Libraries

- Windows always uses DLLs moved to the same directory as their executable. I have had a great deal of trouble linking them to Kioku statically.
- Apple platforms always use static libraries. This is the recommended configuration. I'm not too familiar with why that is, but my understanding is it doesn't deal with shared libraries in the same way Windows/Linux does, and for some reason this can make them more complex to deploy.
- Linux can be built as dynamic or static. Hooray!

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

1. As noted before, I have not found a way to link static libraries on Windows. It has something to do with my MSVC builds where standard libraries are not playing nice with static linkage. Something somewhere is linking to them dynamically on static builds, which causes symbol conflicts. I'd like to get this fixed some day.
1. Failing to link at run time in Debug builds for VS2015 due to one Windows DLL not being redistributable. Not sure why this is. This is ultimately why the above explicitly states VS2013.
1. It seems libgit2 has a bug where it segfaults in `srsGit_Add` on Release builds. At the moment I can only get Debug builds to pass that particular unit test.

## More on OSX

I had a lot of problems linking which stemmed from being uninformed - post-10.04 OSX use a proprietary security framework instead of OpenSSL (I think as a knee-jerk reaction to the Heart Bleed exploit) whose symbols I mistook for OpenSSL. For a while I was trying to redetect the libgit2 dependencies and wedge them into my CMakeLists, but this is the wrong way to do things. I think I'm supposed to ignore what those might be and instead utilize pkgconfig to do the heavy lifting. Haven't 100% figured out how to do that yet.

## More on Linux

I rarely have problems compiling for Linux, but I did come across one very wierd problem on one of my Japanese installs of Ubuntu.

When compiling libssh2 on it, I encountered this.
```
[ 44%] Linking C shared library libssh2.so
*** invalid %N$ use detected ***
collect2: fatal error: ld terminated with signal 6 [中止], core dumped
compilation terminated.
/usr/bin/ld: src/CMakeFiles/libssh2.dir/build.make:614: ターゲット 'src/libssh2.so.1.0.1' のレシピで失敗しました
make[2]: *** [src/libssh2.so.1.0.1] エラー 1
make[2]: *** ファイル 'src/libssh2.so.1.0.1'　を削除します
CMakeFiles/Makefile2:85: ターゲット 'src/CMakeFiles/libssh2.dir/all' のレシピで失敗しました
make[1]: *** [src/CMakeFiles/libssh2.dir/all] エラー 2
Makefile:160: ターゲット 'all' のレシピで失敗しました
make: *** [all] エラー 2
```

A number of other Japanese developers had fixed this in unrelated projects with `-fPIC`, and some had varying success with `-U_FORTIFY_SOURCE`/`-D_FORTIFY_SOURCE=0`. None of these worked in my particular case. So I gave up and installed `libssh2-1-dev` via `apt`.

```
citadel:~/kioku-srs$ sudo apt-get install libssh2-1-dev
パッケージリストを読み込んでいます... 完了
依存関係ツリーを作成しています
状態情報を読み取っています... 完了
以下の追加パッケージがインストールされます:
  libgcrypt20-dev libgpg-error-dev libssh2-1
提案パッケージ:
  libgcrypt20-doc
以下のパッケージが新たにインストールされます:
  libgcrypt20-dev libgpg-error-dev libssh2-1 libssh2-1-dev
アップグレード: 0 個、新規インストール: 4 個、削除: 0 個、保留: 0 個。
752 kB のアーカイブを取得する必要があります。
この操作後に追加で 2,888 kB のディスク容量が消費されます。
続行しますか? [Y/n] y
取得:1 http://archive.ubuntu.com/ubuntu xenial/main amd64 libgpg-error-dev amd64 1.21-2ubuntu1 [68.2 kB]
取得:2 http://archive.ubuntu.com/ubuntu xenial-updates/main amd64 libgcrypt20-dev amd64 1.6.5-2ubuntu0.3 [380 kB]
取得:3 http://archive.ubuntu.com/ubuntu xenial-updates/universe amd64 libssh2-1 amd64 1.5.0-2ubuntu0.1 [70.2 kB]
取得:4 http://archive.ubuntu.com/ubuntu xenial-updates/universe amd64 libssh2-1-dev amd64 1.5.0-2ubuntu0.1 [233 kB]
752 kB を 1秒 で取得しました (385 kB/s)
以前に未選択のパッケージ libgpg-error-dev を選択しています。
(データベースを読み込んでいます ... 現在 39621 個のファイルとディレクトリがインストールされています。)
.../libgpg-error-dev_1.21-2ubuntu1_amd64.deb を展開する準備をしています ...
libgpg-error-dev (1.21-2ubuntu1) を展開しています...
以前に未選択のパッケージ libgcrypt20-dev を選択しています。
.../libgcrypt20-dev_1.6.5-2ubuntu0.3_amd64.deb を展開する準備をしています ...
libgcrypt20-dev (1.6.5-2ubuntu0.3) を展開しています...
以前に未選択のパッケージ libssh2-1:amd64 を選択しています。
.../libssh2-1_1.5.0-2ubuntu0.1_amd64.deb を展開する準備をしています ...
libssh2-1:amd64 (1.5.0-2ubuntu0.1) を展開しています...
以前に未選択のパッケージ libssh2-1-dev:amd64 を選択しています。
.../libssh2-1-dev_1.5.0-2ubuntu0.1_amd64.deb を展開する準備をしています ...
libssh2-1-dev:amd64 (1.5.0-2ubuntu0.1) を展開しています...
install-info (6.1.0.dfsg.1-5) のトリガを処理しています ...
man-db (2.7.5-1) のトリガを処理しています ...
libc-bin (2.23-0ubuntu9) のトリガを処理しています ...
libgpg-error-dev (1.21-2ubuntu1) を設定しています ...
libgcrypt20-dev (1.6.5-2ubuntu0.3) を設定しています ...
libssh2-1:amd64 (1.5.0-2ubuntu0.1) を設定しています ...
libssh2-1-dev:amd64 (1.5.0-2ubuntu0.1) を設定しています ...
libc-bin (2.23-0ubuntu9) のトリガを処理しています ...
```

And lo, the next time I compiled `libssh2` from source, it succeeded! The difference? Gcrypt. The ostensibly *optional* dependency fixed my problem. I uninstalled `libssh2-1-dev`, and sure enough, it continued to compile from source.

## Build Strategy

I'll be the first to admit that this build system needs a lot of work. It's easy to use and it works, which is what's truly important. If you look at the source of [CMakeLists.txt](CMakeLists.txt) you'll see it's not pretty. Part of this comes from my inexperience with leveraging CMake. Another part of this comes from my inexperience building a library with such complex dependencies. And finally, the last part comes from my desire to keep things portable and relatively consistent, which sometimes means not having the most optimal solution for each platform.

If you are experienced in doing this and see it as a kluge, please, for the love of all that is good, tell me what I'm doing wrong and how I can improve it. If you're an especially kind person, submit a PR.

# Possible Future Dependencies

Here are other libraries I'm thinking of using in the future. Sorted by use-case. This information isn't really relevant to building, but I can't think of a better place to keep this list right now.

#### Backend
- [curl](https://github.com/curl/curl) - This one should be obvious.
- [openssl](https://github.com/openssl/openssl) - More common/accepted. This is currently the only supported libgit2 SSL implementation (though there's a feature in progress).
- [mbedtls](https://github.com/ARMmbed/mbedtls) - Appears to be the smallest, most portable SSL implementation, which would allow for HTTPS on mongoose, but libgit2 support is sparse.
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
