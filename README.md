# Kioku (記憶) SRS

A portable, versioned, distributed, self-hosted spaced repetition system (SRS) inspired by Anki and backed by Git.

Developers and people who just want to build this should just skip to the build section at the end of the readme.

## What's an SRS?

SRS stands for Spaced Repetition System. Many people simplify it to "smart flashcards", but frankly, that does not do it justice. SRS is based upon the Spacing Effect, where there is constantly changing optimal interval of time between exposing yourself to information for the purpose of long-term retention. SRS uses an algorithm that finds the optimal spacing based on your ability to recall the information you're testing yourself on. Testing is sort of a bad word for this, as it's really more like exposing and pressing a button based on whether you remembered or not. To utilize an SRS properly, there should be very little thought involved when recalling. You either remember well or you don't. It's not so much a "test" in that sense; it's a "rep" as if you were lifting a small weight.

Examples of SRS applications would be SuperMemo, Anki, Mnemosyne, and perhaps more popularly, DuoLingo (though it's rather young by comparison).

## Why another SRS?

This is really born of my frustration with Anki (and other SRS solutions). I've used Anki for over a year. It does its job wonderfully. There are many user-created decks and addons, but it's a nightmare to actually port addons to multiple devices. Mnemosyne's modding framework is a bit more thought out, but it suffers from a similar class of problems.

This section will review the main problems Kioku aims to solve.

### Portability

There are two kinds of portability here. Application Portability, and Addon Portability. Kioku concerns itself with both.

Anki and Mnemosyne are fairly portable to most Desktop and Mobile environments. The desktop apps have some problems that stem from using Python/Qt, and the mobile apps ultimately suffer from the lack of Addons and interface features.

Kioku proposes a decoupled architecture and choice of technology that's designed with full portability in mind, even if it doesn't happen all at once. If everything is done right, someone could trivially modify Kioku to run in embedded Linux terminal, if you're brave enough to implement that. It's going to be designed more with web browsers in mind, but Kioku aims to facilitate creating other interfaces, even if their feature set is smaller.

### Hackability

Anki's modding framework is extremely permissive, but not very well thought out. Mnemosyne doesn't appear to have a very active modding community in the way Anki does. Both require use of Python and neither are portable to mobile or web platforms.

Kioku aims to make modding possible in any language, so long as it can be bound to C/C++ and support an HTTP client stack.

### Progress is Sacred

No other SRS, so far as I'm aware at the time of writing this, supports full revision history. That is, every file has a history of changes that can be rolled-back to at any time. It's like having an infinite undo/redo button for everything. Most SRS applications support backups and learning history, but arbitrary files cannot be versioned. Worse yet, in Anki it is impossible to implement your own without potentially compromising database integrity. Not good.

With Kioku being backed by Git, every action has a chance to be versioned right off the bat. Every. Single. Action. If your computer crashes in the middle of editing a card and Git is set up to ammend your changes to it as you write (and commit when you save), there's a very likelihood of losing ANY work.

Being flatfile based, you also stand little chance of ending up in a corrupted state. Database (particularly SQLite) backed solutions are vulnerable to this. They mitigate it by saving compressed backups. A Git repository serves this purpose intrinsically.

### User Freedom

Mnemosyne did something right by providing sync options and self-hosting. Anki not so much, but it has made progress.

Kioku aims to make SRS implementation easy. If DuoLingo decided one day to use it as their internal backbone, they could.

If a user wants to use alternative syncing mechanisms, they can implement or download an addon that facilitates that. Same can be said for *The Algorithm*.

### Collaborative Learning

Something no other SRS has done yet is deck/template collaboration. Being able to fork decks and contribute to them is something that Git can facilitate. Still, this is not a simple problem to solve. Particularly when it comes to users who have forked a deck and want to stay up-to-date with them. This becomes difficult (or perhaps impossible) if the user makes their own modifications to the deck content, which is something that should be encouraged.

On the other hand, there is no precedent for this. Kioku does not have to implement this perfectly. It can simply facilitate it and improve in future iterations.

## Goals

### Features
- Program is portable.
- Addons are portable.
- Addons can extend/modify/reimplement nearly all functionality.
- Can be self-hosted.
- All user data can be versioned.
- Certain kinds of user data can be unversioned and synced via different mechanisms.
- Can import from and export to major SRS solutions such as Anki and Mnemosyne.
- Can be installed or run self-contained (e.g. flash drive).
- Can be used for research purposes.
- Unicode support
- [Localization support](https://msdn.microsoft.com/en-us/library/aa241727(v=vs.60).aspx)
- Support LaTeX, HTML, CSS, Javascript, images, videos, sounds, and so on.

### Properties
1. Decoupled. The APIs for this should be written in such a way that bindings/implementations in other languages are possible, and mixing them is possible.
2. Portable. At minimum, a client/server implementation should be able to run on anything that could support an HTTP server. HTML/Javascript/CSS browsing support would be required if the user plans to utilize such features in their addons and cards.
3. Extensible. Various implementations of clients/servers could be written, and addons could similarly be written in a feature-level portable way. This would mean Javascript for clientside (which could cover anything the REST API touches) and Lua for serverside (extending the REST API). Serverside addons could probably be distributed in a portable way so long as there's a way to configure them to use native modules for multiple platforms. Portability of clientside addons would ultimately depend on the client being used. If they're natively written without a full HTML interface, addon authors would need to be able to check what platform/client their addon is running on and select an implementation accordingly, assuming the client supports it in some way. I think that clientside addons would be best wrapped in Lua with a common interface for loading/injection purposes at the very least.

In an ideal world someone could write a client implementation as a CLI or even in ncurses, and still utilize serverside addons that don't rely on HTML/JS, but clientside display of HTML with addons written in Javascript could not be supported. They could, of course, still rep. The clientside addons would still be synced and usable on supported clients. If the person who made the ncurses client was feeling particularly generous, they might implement their own clientside addon framework which could of course be synced. If the design of the server/model is particularly good, the UI would be described by a model that could be implemented on clients to facilitate maximum addon portability.

One hardmode use-case would be a media capture plugin. This is extremely platform dependent and is difficult to accomplish via browser alone. Many browsers will support [MediaStreamRecorder](https://github.com/streamproc/MediaStreamRecorder) and/or [RecordRTC](https://github.com/muaz-khan/RecordRTC), but for some platforms it may be necessary to implement it using some combination of Lua and native modules. It seems the best bet would be to use [Cordova](https://cordova.apache.org/) with [cordova-media-capture-plugin](https://github.com/apache/cordova-plugin-media-capture) for the quirky mobile platforms. A lot of the quirks in that plugin can be mitigated by the decoupled architecture of this application.

## Building

This project uses CMake. This is for portability reasons. You will need it in order to build.

Start by cloning. This should work on Unix-like systems and Git Bash for Windows.
```bash
git clone --recursive https://git@github.com/seltzer/kioku-srs.git
cd kioku-srs
```
Here's how to build on Linux. This assumes you have git and cmake installed.
The following should work out of the box on Unix-like platforms so long as git and cmake are installed. On Windows, I've only tested with Visual Studio 2013 installed and from a Git Bash terminal. You will need CMake installed and added to your `PATH` environment variable.

Starting from the cloned repository...
```bash
mkdir build
cd build
cmake ..
cmake --build .
ctest
```

## Libraries

### Currently Required
All required libraries are included as submodules in the [extern](extern/) folder. Not all of these are in active use yet, but are expected to be.
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

### Possible Future Dependencies

Here are other libraries I'm thinking of using in the future. Sorted by use-case.

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

