# Basic Architecture

## Abstract

The idea is to have a decoupled framework that allows the user to sync using free hosting solutions, empower developers to easily implement clients and servers that can be mixed/matched, and empower server admins to deploy their own servers.

## Components

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

## API Layers

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
