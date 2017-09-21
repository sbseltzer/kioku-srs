# Kioku (記憶) SRS

A portable, versioned, distributed, self-hosted spaced repetition system (SRS) inspired by Anki and backed by Git.

## Concept

The idea is to have a decoupled framework that allows the user to sync using free hosting solutions, empower developers to easily implement clients and servers that can be mixed/matched, and empower server admins to deploy their own servers.

### Goals

Properties this should fulfill.
1. Portable. At minimum, a client/server implementation should be able to run on anything that could support an HTTP server.
2. Decoupled. The APIs for this should be written in such a way that bindings for other languages are possible, and mixing them is possible.
3. Extensible. Various implementations of clients/servers could be written, and addons could similarly be written in a portable way.

### Components

- SRS API Server - Self-hosted daemon that implements a RESTful API for managing collections. This would interact with the Collection Repository and facilitate syncing with the Collection Remotes. It would also provide SSH key management.
- SRS Client - The method by which the SRS API Server is accessed. This could be via your browser with simple HTML. Perhaps the SRS Server (or some separate server exclusively for serving the client) could provide an HTML interface. A custom UI version (maybe backed by webkit for the lazy) is another option. In fact, an Anki addon could be used to turn it into a Kioku client.
- Collection Repository - The local repository that contains decks and media.
- Collection Remotes - Remotes to use for syncing.

The SRS Server interacts with the Collection Repository, which can sync to Collection Remotes. In fact, the SRS Server is completely decoupled in that you could connect to a remote one, sync to your repository host(s), and pick it up again on a local server. Some SRS Server implementations could serve extra HTML that offer other features like managing/syncing remotes via pull request.

### API Layers

The lowest layer is the Plumbing layer.
- Filesystem - One that reads and manipulates data.
- History - One that versions data via Git.
- Sync - One that syncs data via Git.

Above that is the SRS layer.
- Model - One that builds and modifies a data model using Filesystem & History modules.
- Stats - One that computes statistics based on the Model module.
- Schedule - One that computes/modifies scheduling using the Model module.
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

## Dependencies

### Build System

Potential options:

- Make
- CMake
- Autotools
- Premake

To build from source, I'm thinking CMake. This would be for maximum portability.

### Libraries

- [mongoose](https://github.com/cesanta/mongoose) - Portable embedded webserver.
- [libgit2](https://libgit2.github.com/) - This is wrapped to create a filesystem database thingy. The rationale is for portable synchronization, file management, and deck version history.
- [lua](https://lua.org) - I'm thinking of using this as a portable dynamic loader for backend tasks. This would also make it easier to embed with things like PHP. It'd also serve as a nice configuration language, providing similar value to JSON.
- [luagit2](https://github.com/libgit2/luagit2) - Lua bindings for libgit2. Why not?
- [utf8](https://github.com/sheredom/utf8.h) - A portable single-header library for UTF-8 string functions.
- [jsont](https://github.com/rsms/jsont) or [ims-json](https://github.com/inmotionsoftware/ims-json) - Portable JSON library.
- [hotel](https://github.com/typicode/hotel) - This looks promising for developing a client or server.
- [php-src](https://github.com/php/php-src) - The PHP source code for compiling the interpreter and CGI.
- [php-lua](https://github.com/laruence/php-lua) - A PHP PECL package for embedding a Lua interpreter in PHP.
- [markdown-it](https://github.com/markdown-it/markdown-it) - A Javascript Markdown parser.
- [stackedit](https://github.com/benweet/stackedit) - An in-browser Markdown editor.

## Basic Architecture

