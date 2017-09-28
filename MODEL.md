# Model

This file is a draft of how data is managed in Kioku. I think this will probably drive the high level design.

## Goals


## Filesystem

This is a draft of the partly git-managed filesystem, which acts as a sort of database.

```
.version - Implementation version information for this device
LOG.txt - Logfile for server. Clients can implement their own logging, or log to server.
/.plugins - Where native (compiled/binary) plugins go. Not synced.
  /some-scripting-engine
    .version - Implementation version information. This is checked against device .version file for ABI compatibility.
    .sources - Where to get the binaries from for various platforms, which in turn represents device compatibility.
    -- files
  /lua-scripting-engine
    -- files
  /javascript-scripting-engine
    -- files
  /python-scripting-engine
    -- files
  /ruby-scripting-engine
    -- files
users.(config) - List of users and paths to their directories. By default these are within the Model tree. Users on some devices (such as Desktop) may wish to have their user located elsewhere on the Device. Since the Git repo starts within the User directory, it is appropriate to allow them to store it elsewhere on the Device so long as there's a reference to it here.
/user-name.git - The bare repository. Only present on bare model hosts.
  /extern - Non-versioned data
/user-name
  /.git - This is a git repository
  .gitignore
  .sync
  .version - Implementation version information for this user. This is checked against the device .version file for ABI compatibility.
  searches.txt - list of saved searches
  tags.txt - list of available tags, which should be kept as an aggregate of all note tags.txt files.
  /options
    general.(config) - General user options
    addons.(config) - Addon options
    interface.(config) - User interface options
    schedule.(config) - Scheduling options
    sync.(config) - Syncing options
    /groups - option groups for decks
      default.(config) - The default options.
      -- arbitrary extra option groups.
      some-custom-group.(config)
  /addons - where addon scripts go - this is the root include path for scripting all languages
    .gitignore - CONSIDER - Should the contents of this folder be ignored? Addons may be git versioned separately by advanced users, or installed/updated via archive. In fact, an addon package manager might make for a good meta-addon.
    .gitmodules - CONSIDER - Some addons might be git repositories. The user would need an interface for managing this. If they start their own addon repository here, it'd be nice to provide a user-friendly interface to control how it's kept updated.
    /my-addon-folder
      /.git - CONSIDER - It would be nice if addons could be versioned with git. Forked or otherwise.
      .version - Implementation version for this addon. This is checked against the device .version file for ABI compatibility.
      info.(config) - Contains stuff like plugin dependency, where it came from, permissions, what kind of mod it is, etc.
      manifest.(config) - Data-oriented install/uninstall information.
      options.(config) - Specialized options available to client.
      startup.(script) - This is executed when the SRS server launches.
      -- script files go here
      some-script.(script) - Some arbitrary script file
      LOG.txt - Addon-specific logging.
  /templates
    /note-template-id
      generator.(script) - used to describe how to generate cards from a note
      sides.(config) - used to describe how many sides the note has
      fields.(config) - used to describe the fields available to the note
      /sides
        -- Display templates for card sides as named in the sides file --
        side-name-A.(markup)
        ...
        side-name-Z.(markup)
        -- End sides --
      /media
        -- Extra arbitrary files used by this template
        -- When a foreign note type is imported, resources go here by default to prevent name conflicts
    /media
      -- extra arbitrary files accessible to all templates
  /decks
    /deck-id
      /.git - CONSIDER - What if shared decks were forks? One issue would be that templates would also need to be forked, and that could become complex (unless the template could be embedded in the deck).
      .option-group - soley references option group
      manifest.txt
      .scheduler - Solely references a scheduling strategy
      /notes
        /note-id - Some unique name for the note
          tags.txt - Tags used by this card. Since this is a filesystem database, there will be some redundancy here with the upper tags.txt file.
          .template - solely refers to a note id
          .schedule - solely contains scheduling data for each card - the history of this can be used for anonymous research purposes
          fields.(config) - A special override file for defining all fields in one file. This would be useful for import/export of foreign note types or translating between incompatible ones. This would forego use of the /fields folder. Deleting/replacing that folder would be fine since git could revert it.
          /fields
            -- Data for fields as named in the note template fields file --
            field-data-A.(markup)
            ...
            field-data-Z.(markup)
            -- End fields
          /generated
            .gitignore - This folder is not versioned. This serves as a cache of generated card content.
            /card-id
              -- Generated card sides as named in the note template sides file --
                side-name-A.(display)
              ...
                side-name-Z.(display)
              -- End sides --
      .media - Metadata for media repository syncing.
      /media - Decks have their own separately versioned media folders. If the deck is deleted, the media (and space overhead) can go with it.
        /.git - Having media history managed separately from the deck would mean history could be rewritten to save space in a slightly safer fashion. The references to deleted content will still be present in note history, so clobbering them isn't the worst thing in the world. The user could, of course, mark certain ones as versioned.
        .gitattributes
        -- arbitrary media files for use in this deck
  .media - Metadata for media repository syncing.
  /media
    /.git
    .gitignore
    .gitattributes
    -- arbitrary media files for use in all decks/templates/addons
/.base-user
  -- Same structure as above, but acts as a sort of base class
  -- Content here is available to all users and cannot be modified by users.
  -- Users can, however, override content here by creating duplicates in their directory.
  -- This would include stuff like default note types, default scripts, default tags, etc.
```

## Types / Concepts

### Device
### Plugin (Script Engine)
### User Content
### Script
Any file that uses a script format.
### Addon
A collection of files and scripts to extend the server implementation.
### Addon Types
Scheduler
REST Extension
Import/Export Type
Template Installer 
Media Installer
### Formats
This section describes file formats that could vary.
#### Script Format
Facilitated by Plugins. Must support an HTTP client, and be able to bind to C functions.

Extension Ideas: lua, py, js, rb

Portability: Lua and Python at minimum. Javascript via V7 is another promising option. 
#### Config Format
Nested key-value configuration format. Must be portable out of the box.

Extension Ideas: json, js, lua

Portability: JSON and Lua at minimum. Javascript via V7 is another promising option.
#### Markup Format
Facilitated by Generators; Formats are actually quite arbitrary.
 
Extension Ideas: txt, md/markdown, html

Portability: Generators for markup must be in the portable subset of script formats.
#### Display Format
Up to the templating and/or client implementation. This is what is delivered to the end client via the REST API.

Extension Ideas: html, md/markdown, txt

Portability: HTML and TXT must be available out of the box. Other possibilities include CGI-compatible languages such as PHP, or even custom formats for specialized clients.
#### Import/Export Format
Facilitated by Addons; Available formats for import/export of decks.
Extension Ideas: csv, txt, apkg, kioku
Portability: This is only necessary on Desktop/PC, but since it is facilitated by Addons, it could be ported anywhere.
### Scheduler
Facilitated by Addons; A scripted API for scheduling cards.
It has access to the content and history of `.schedule` files, and is tasked with determining what to set that content to when a User answers a Card.
### Media Sync Strategy
Facilitated by Addons; A scripted API for synchronizing user media between devices.
This must be applicable on a per-file basis, and it must be possible to change it later.
At minimum, Git must be supported.
### Options
These follow a key-value nested config format.
### Option Group
These follow a key-value nested config format, but are used exclusively for configuring decks.
### Deck
A unique Deck within a User.
#### ID
Unique ID of a deck within a User.
#### Name
#### Deck Option Group
#### Notes
### Template
Describes a how Notes of a particular Type are handled when generating Cards.
#### Fields
#### Sides
These describe the textual format for each side of the card using a special markup for placing fields.
#### Card Generator
Facilitated by a portable subset of Scripts; A scripted API for generating cards from a note. These are defined as part of a Note Template. It's important that these are portable to all devices.

These determine how many cards to generate, their IDs, and the content of their fields. Hence, Cloze Deletion cards would have a special generator that defines what to replace text matching `{{c%d::text(::hint)?}}` with for N cards.

Portability Note: It's possible that this may end up similar to addons in that they define their compatibility, and revert to a device-defined default implementation on unsupported devices (concatenating their fields). For instance, say the user uses Ruby to generate their cards natively on Desktop. Or worse, they use Lua to fire off a Ruby script to do the dirty work. A mobile device that has no Ruby plugin or Ruby interpreter installed will bomb out and be forced to use some other default generator. On the flip side, what they ought to do is leave the generator alone (or very lightweight) andutilize the API to do the real work in Javascript clientside.
### Note
A unique Note within a Deck
#### ID
The unique ID of a note within a Deck.
#### Type
An available type for use by Notes, which has an associated Template.
A collection of files that describe a Note Type.
#### Field Data
The textual content stored in each Field as defined by its Template.
#### Cards
The list of cards generated by a Note.
### Card
#### ID
The unique ID of a card within a Note.
#### Side Content
The textual content on a given side of a Card.
#### Schedule
Determines when to next show the Card to the User.

## REST API

Install Plugin (for device)

Uninstall Plugin (for device)

Get Plugins (for device)

Get Users (for device)

Install Addon (for user)

Uninstall Addon (for user)

Get Addons (for user)

Get Deck List (for user)

Get Note List (for deck | for note type | for tag)

Get Card List (for deck | for note type | for note | for tag)

Get Next Scheduled Card (from deck)

Add Tag (for user | to note)

Delete Tag (for user | from note)

Add Note Type (for user)

Delete Note Type (for user)

Edit Note Type (for user)

Add Deck (to user)

Add Note (to deck)

Set Field Content (for note)

Add Resource File (to note type)

Add Media File (to user)

Get Media List (for user | for deck | for note)

Generate Cards (for note)

Format Card Content (for card using note template)

Get Note Type (for note)

Get Note (for card)

Get Card By Child ID (for note)

Get Card By Unique ID (for deck)

Get Siblings (for card)

Give Answer (for card)

Suspend (for card | for note)

Delete (for card | for note)

Reschedule To Date (for card | for note)

Search Using Filter String (for cards | for notes)

Sync (for user)

Set Sync Method (for file | for file extension)

Get File Hash (for file)

Get List of Unstaged Files (for user)

Get Full Revision History (for user)

Restore File to Previous Version (for user)

Get Anonymous Scheduling Data (for user)

Get Options (for user | for addon | for deck)

Set Options (for user | for addon | for deck)

Export Deck (for user)

Import Deck (for user)

Export Addon (for user)

Import Addon (for user)

Write Log (for device)

Get Logfile (for device)

Convert To Note Type (for note)

Rename User (for user)

## Syncing Considerations

Being Git, there's always the possibility of a merge conflict.

In other SRSs you can end up in situations where they say "pick one". My problem with this is if a card was added on one system and a note type was modified on another, one of those changes might be lost. Anki is picky when it comes to modifying fields on note types - requires a total database upload. There's probably a good reason for this, but it's not terribly obvious.

In our case, it should give the user an interface to merge. In most cases, they'll simply need to "pick one" on a per-file basis. At the very least, show version A, version B, say which devices they're from and give them an interface to arbitrate.

One thing I'd like to facilitate is sharing of cards/decks in a merge friendly way. Like, being able to update an RTK deck to newer revisions without it screwing up history.

Even cooler would be anonymous data collection to see where people generally get stuck in a deck.

I very much suspect there will be files introduced that should be synced, but not with Git. One example might certain kinds of content in the media folder. Another would be, say, embedded databases that addons choose to use. It'd be nice to use Rsync, but it's not well supported on Windows. Nearly every syncing solution I've found has serious portability problems. It may be best to roll my own using a portable FTP library and just version the checksums of non-versioned files. It's not very efficient, but it'd get the job done.

Alternatively, there are [scripts like this](https://robinwinslow.uk/2013/06/11/dont-ever-commit-binary-files-to-git/) that purge binary files by rewriting history. You could even preserve commit times by [faking them](https://stackoverflow.com/questions/3895453/how-do-i-make-a-git-commit-in-the-past#3898842). On the other hand, you end up with incompatible histories. This would basically put the user in the same place that Anki does when modifying note types, but for every time media is deleted/modified (or perhaps only when they want to reclaim space). Not so good. This is where there'd need to be a companion sync method (maybe a tiny metadata repository) that assists in getting a re-clone.

This might also be a job for plugins/addons. I think it'd be nice to offer users the option to change out how they sync certain kinds of files.

### Some ideas for syncing media (particularly binary files)
- Version them right along with everything else.  
  - To hell with the user.
  - Who needs disk space anyway?
- Version them with everything else, but provide surgical removal of previous version via history rewrites.  
  - Complex to implement.
  - Requires a full repo re-sync when the user wishes to reclaim space.
  - This would require an extra versioned file that helps determine when this is necessary.
- Version them in their own repository, and do a history rewrite for binary files when the user wants to reclaim space.  
  - Gives user the opportunity to version anything and prune later.
  - Easy to delete a media repository wholesale.
  - Hard to implement history replay.
  - Requires a media repo re-sync in order to fully remove a media file.
  - Requires an extra remote repository.
- Use [Git LFS](https://github.com/git-lfs/git-lfs/blob/master/docs/api/basic-transfers.md)
  - Requires a host with the LFS server installed.
  - Using this locally would offer no particular benefits over directly versioning the files.
- Only version checksums for unversioned files, then use something like SFTP, [Google Drive](https://developers.google.com/drive/v3/reference/), [Dropbox](https://www.dropbox.com/developers/documentation/http/documentation), or something else for actual syncing.  
  - Closest to what Anki does.
  - Easy-ish to implement.
  - Doesn't rewrite history.
  - Consistently avoids bloat.
  - Files managed this way can't be versioned, which might not be desirable in some cases.
  - Requires help from an external protocol and some other media hosting service.
- Version checksums like above, but instead have each media file in its own little repository.  
  - Not terribly hard to implement.
  - Files can be versioned and later be permanently deleted without any complexity.
  - Rewriting history for a single-file repository is much simpler than surgical strikes in a collection.
  - So many repositories, though. This would be quite nasty to self-host.
  
A number of the above strategies suffer from needed extra hosting solutions (multiple git repos, sync services, FTP, etc). The multiple git repository one is particularly bad because it'd be cumbersome for a user to manage without hosting their own automated solution. Granted, a solution like that should probably be available from the get-go just like AnkiWeb. On the other hand, having a built-in self-hosted solution for any of these methods should be available by design. It's an SRS server managing this model, but it also pushes to a locally hosted remote as its first remote. This would mean that, in the FTP case, an FTP server would also be present locally. This is kinda crappy, though, since now you've got twice as much space being taken up per client. The most appropriate solution to this would be to have locally hosted unversioned media be available on-demand, similar to how a remote server would handle it.

I think there is probably some value in the surgical strike for situations where the user has done something ridiculous like commit a gigantic binary early on and later wants to remove it. Having a generic way to delete revisions in a pinch would be nice. This is especially relevant if they download a shared deck that comes with a lot of images/sounds and later wishes to delete it. All revision history relating to the imported files would need to be purged.

There's nothing saying that these couldn't all be mixed and matched, implemented over time. If I did do that, what would be the best order?
1. The easiest "screw the user" approach would be totally appropriate if the "surgical history rewrite" approach gets implemented later. That way it can be introduced as the need begins to arise.
2. The Anki-like checksum versioning scheme would be in line with the precedent set by current SRS solutions.

## Sharing

Sharing decks, note types, notes, and addons is desirable. In what way, though? There are many use-cases that become available with Git, and many ways to go about it.

With Addons, there's a strong analogy to submodules, but some people may prefer to modify them and keep them versioned in their own history.

Decks are the trickiest. 

## Security

A big part of this will be what addons can modify. Some things should be sacred.
