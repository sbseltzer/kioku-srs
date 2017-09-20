# Kioku (記憶) SRS

A portable, versioned, distributed, self-hosted SRS inspired by Anki and backed by Git.

## Concept

The idea is to have a decoupled framework that allows the user to sync using free hosting solutions, empower developers to easily implement clients and servers that can be mixed/matched, and empower server admins to deploy their own servers.

### Components

- SRS Server - Self-hosted daemon that implements a RESTful API for managing collections. This would interact with the Collection Repository and facilitate syncing with the Collection Remotes. It would also provide SSH key management.
- SRS Client - The method by which the SRS Server is accessed. This could be via your browser with simple HTML. Perhaps the SRS Server (or some separate server exclusively for serving the client) could provide an HTML interface. A custom UI version (maybe backed by webkit for the lazy) is another option. In fact, an Anki addon could be used to turn it into a Kioku client.
- Collection Repository - The local repository that contains decks and media.
- Collection Remotes - Remotes to use for syncing.

The SRS Server interacts with the Collection Repository, which can sync to Collection Remotes. In fact, the SRS Server is completely decoupled in that you could connect to a remote one, sync to your repository host(s), and pick it up again on a local server. Some SRS Server implementations could serve extra HTML that offer other features like managing/syncing remotes via pull request.

## Dependencies

### Build System

Potential options:

- Make
- CMake
- Autotools
- Premake

To build from source, I'm thinking CMake. This would be for maximum portability.

### Libraries

- [libgit2](https://libgit2.github.com/) - This is wrapped to create a filesystem database thingy. The rationale is for portable synchronization, file management, and deck version history.
- [lua](https://lua.org) - I'm thinking of using this as a portable dynamic loader for backend tasks. This would also make it easier to embed with things like PHP.
- [luagit2](https://github.com/libgit2/luagit2) - Lua bindings for libgit2. Why not?
- [utf8](https://github.com/sheredom/utf8.h) - A portable single-header library for UTF-8 string functions.
- [jsont](https://github.com/rsms/jsont) - A portable JSON library.
- [ims-json](https://github.com/inmotionsoftware/ims-json) - Another portable JSON library I'm considering with a focus on speed and efficiency.
- [php-lua](https://github.com/laruence/php-lua) - A PHP PECL package for embedding a Lua interpreter in PHP.

## Basic Architecture

