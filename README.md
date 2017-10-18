# Kioku (記憶) SRS

A portable, versioned, distributed, self-hosted spaced repetition system (SRS) inspired by Anki and backed by Git.

## Concept

## Why?

This is really born of my frustration with Anki (and other SRS solutions). Anki does its job wonderfully, but it's a nightmare to hack on. Mnemosyne's modding framework is a bit more thought out, but it suffers from a similar class of problems that Anki does.

This section will review the main problems Kioku aims to solve.

### Portability

There are two kinds of portability here. Software Portability, and Addon Portability.

Anki and Mnemosyne are fairly portable to most Desktop and Mobile environments. The desktop apps have some problems that stem from using Python/Qt, and the mobile apps ultimately suffer from the lack of Addons and interface features.

Kioku proposes a decoupled architecture and choice of technology that's designed with full portability in mind, even if it doesn't happen all at once. If everything is done right, someone could trivially modify Kioku to run in embedded Linux terminal.

### Hackability

Anki's modding framework is extremely permissive, but not very clear. Mnemosyne doesn't appear to have a very tight-knit modding community in the way Anki does. Both require use of Python and neither are portable.

Kioku aims to make modding possible in any language that can be bound to C/C++ and support an HTTP client stack.

### Progress is Sacred

No other SRS, so far as I'm aware at the time of writing this, supports full revision history. They support backups and learning history, but arbitrary files cannot be versioned. Worse yet, in Anki it is impossible to implement your own without potentially compromising database integrity. Not good. Individual updates to cards/notes cannot be viewed or rolled back.

With Kioku being backed by Git, every action has a chance to be versioned right off the bat. Every. Single. Action. If your computer crashes in the middle of editing a card, Git's amend feature can ensure there is a low likelihood of losing ANY work.

Being flatfile based, you also stand little chance of ending up in a corrupted state. Database backed solutions are vulnerable to this. They mitigate it by saving compressed backups. A Git repository serves this purpose intrinsically.

### User Freedom

Mnemosyne did something right by providing sync options and self-hosting. Anki not so much, but it has made progress.

Kioku aims to make SRS implementation easy. If DuoLingo decided one day to use it as their internal backbone, they totally can.

If a user wants to use alternative syncing mechanisms, they can implement or download an addon that facilitates that. Same can be said for The Algorithm (though this one may be much more difficult to facilitate).

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
- Support LaTeX, HTML, CSS, Javascript, images, videos, sounds, and so on.

### Properties
1. Decoupled. The APIs for this should be written in such a way that bindings/implementations in other languages are possible, and mixing them is possible.
2. Portable. At minimum, a client/server implementation should be able to run on anything that could support an HTTP server. HTML/Javascript/CSS browsing support would be required if the user plans to utilize such features in their addons and cards.
3. Extensible. Various implementations of clients/servers could be written, and addons could similarly be written in a feature-level portable way. This would mean Javascript for clientside (which could cover anything the REST API touches) and Lua for serverside (extending the REST API). Serverside addons could probably be distributed in a portable way so long as there's a way to configure them to use native modules for multiple platforms. Portability of clientside addons would ultimately depend on the client being used. If they're natively written without a full HTML interface, addon authors would need to be able to check what platform/client their addon is running on and select an implementation accordingly, assuming the client supports it in some way. I think that clientside addons would be best wrapped in Lua with a common interface for loading/injection purposes at the very least.

In an ideal world someone could write a client implementation as a CLI or even in ncurses, and still utilize serverside addons that don't rely on HTML/JS, but clientside display of HTML with addons written in Javascript could not be supported. They could, of course, still rep. The clientside addons would still be synced and usable on supported clients. If the person who made the ncurses client was feeling particularly generous, they might implement their own clientside addon framework which could of course be synced. If the design of the server/model is particularly good, the UI would be described by a model that could be implemented on clients to facilitate maximum addon portability.

One hardmode use-case would be a media capture plugin. This is extremely platform dependent and is difficult to accomplish via browser alone. Many browsers will support [MediaStreamRecorder](https://github.com/streamproc/MediaStreamRecorder) and/or [RecordRTC](https://github.com/muaz-khan/RecordRTC), but for some platforms it may be necessary to implement it using some combination of Lua and native modules. It seems the best bet would be to use [Cordova](https://cordova.apache.org/) with [cordova-media-capture-plugin](https://github.com/apache/cordova-plugin-media-capture) for the quirky mobile platforms. A lot of the quirks in that plugin can be mitigated by the decoupled architecture of this application.

## Dependencies

### Build System

To build from source, I'm thinking CMake. This would be for maximum portability.

### Libraries

Here are some libraries I'm thinking of using.

#### Backend
- [mongoose](https://github.com/cesanta/mongoose) - Portable embedded webserver and network stack.
- [libgit2](https://libgit2.github.com/) - This will be wrapped to create a filesystem database thingy. The rationale is for portable synchronization, file management, and deck version history.
- [utf8.h](https://github.com/sheredom/utf8.h) - A portable single-header library for UTF-8 string functions.
- [curl](https://github.com/curl/curl) - This one should be obvious.
- [libssh2](https://github.com/libssh2/libssh2.git) - This is a dependency of libgit2.
- [mbedtls](https://github.com/ARMmbed/mbedtls) - Appears to be the smallest, most portable SSL implementation, which would allow for HTTPS on mongoose, but libgit2 support is sparse.
- [openssl](https://github.com/openssl/openssl) - More common/accepted. This is currently the only supported libgit2 SSL implementation (though there's a feature in progress).
- [parson](https://github.com/kgabis/parson) - Simplistic C89 JSON parser.
- [slre](https://github.com/cesanta/slre) - Super Light RegEx library. Portable. Gives a subset of Perl-style regex in C.
- [generic-c-hashmap](https://github.com/Kijewski/generic-c-hashmap) - A portable generic easy-to-use hashmap in C. Header only. Very slick API.
- [autoupdate](https://github.com/pmq20/libautoupdate) - A cross-platform in-place application updater in C. Mostly useful for inspiration.
- [tinydir](https://github.com/cxong/tinydir) - A portable directory/file reader in a single header.
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
- [lua](https://lua.org) - I'm thinking of using this as a portable dynamic loader for backend tasks. This would also make it easier to embed with things like PHP. It'd also serve as a nice configuration language, providing similar value to JSON.
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

## Basic Architecture

### Abstract

The idea is to have a decoupled framework that allows the user to sync using free hosting solutions, empower developers to easily implement clients and servers that can be mixed/matched, and empower server admins to deploy their own servers.

### Components

This loosely follows an MVC pattern.

- SRS Server - Self-hosted daemon that implements a RESTful API for managing collections. This would interact with the Collection Repository and facilitate syncing with the Collection Remotes. It would also provide SSH key management. This is the controller.
- SRS Client - The method by which the SRS Server is accessed. This could be via your browser with simple HTML, perhaps even generated using something like Jekyll. Perhaps the SRS Server could provide an HTML interface. A custom UI version (maybe backed by webkit for the lazy) is another option. In fact, an elaborate Anki addon could be used to turn Anki into a Kioku client. This is the view.
- Collection Repository - The local repository that contains decks and media. This is the model.
- Collection Hosts - Remotes to use for syncing.

The SRS Server interacts with the Collection Repository, which can sync to Collection Remotes. In fact, the SRS Server is completely decoupled in that you could connect to a remote one, sync to your repository host(s), and pick it up again on a local server. Some SRS Server implementations could serve extra HTML that offer other features like managing/syncing remotes via pull request.

Using these decoupled components, anyone could write an implementation of any component with the guarantee that it "just works" with arbitrary implementations of any other component. Here are some examples.

- Default case: a local server and local client. The user could hotswap clients and servers if they like. Local repository management and syncing is of course managed by the local server via the local client. They might tell it to sync with something similar to AnkiWeb and then using it via that interface would just automagically work.
- The default case could be replicated on a mobile device by simply creating a mobile webapp and have the server written specially for the device.
- Write both the client and server implementation and host it to get something like AnkiWeb. They could even abstract away user management with something like the [Gitlab API](https://docs.gitlab.com/ce/api/users.html). If the server is hosted in a docker container, addons could safely be synced and used via the remote web interface without admins needing to worry.

### API Layers

The lowest layer is the Plumbing layer.
- Filesystem - One that watches/reads/writes data.
- History - One that versions data via Git.
- Sync - One that syncs data via Git.
- Plugin - Interface for plugins.
- Addons - Interface for addons.

Above that is the SRS layer.
- Model - One that builds and modifies a data model using Filesystem & History modules.
- Stats - One that computes statistics based on the Model module.
- Schedule - One that computes/modifies scheduling using the Model module. Example: [Anki Implementation](https://github.com/dae/anki/blob/master/anki/sched.py)
- Options - One that manages options using the Model module.
- Control - The control interface for SRSing which uses with the Model module.
- Delivery - One that composes content from the Model module for the user.

Above that is the Server layer.
- SRS Interface - The low-level API functions using the whole SRS Layer.
- REST - Uses the SRS Interface Module to implement the REST protocol on an HTTP Server.

Above that is the Client layer.
- Model - The API for interfacing with the SRS data model.
- Sync - The API for syncing Git databases.
- Schedule - The API for scheduling.
- Statistic - The API for computing statistics.
- REST - The API for implementing whatever layers are needed for the REST Server.
