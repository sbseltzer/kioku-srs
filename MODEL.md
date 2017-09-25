# Model

This file is a draft of how data is managed in Kioku. I think this will probably drive the high level design.

## Goals


## Filesystem

This is a draft of the partly git-managed filesystem, which acts as a sort of database.
```
.version - Implementation version information for this device
LOG.txt - Logfile for server. Clients can implement their own logging, or log to server.
/.plugins - Where native (compiled/binary) plugins go. Not synced.
  /lua-scripting-engine
    -- files
  /python-scripting-engine
    -- files
  /ruby-scripting-engine
    -- files
/user-name
  /.git - This is a git repository
  .gitignore
  .sync
  .version - Implementation version information for this user. This is checked against the device .version file for ABI compatibility.
  tags.txt - list of available tags, which should be kept as an aggregate of all note tags.txt files.
  /options
    general.[lua|json|xml|ini] - General user options
    addons.[lua|json|xml|ini] - Addon options
    interface.[lua|json|xml|ini] - User interface options
    schedule.[lua|json|xml|ini] - Scheduling options
    sync.[lua|json|xml|ini] - Syncing options
  /addons - where addon scripts go - this is the root include path for scripting all languages
    /my-addon-folder
      info.(lua|json|xml|txt) - Contains stuff like plugin dependency, where it came from, etc.
      install.(lua|rb|py) - How it installs/uninstalls itself.
      options.(lua|rb|py) - Specialized options available to client.
      -- script files go here
      some-script.(lua|rb|py) - Some arbitrary script file
      LOG.txt - Addon-specific logging.
  /templates
    /note-template-id
      generator.(lua|rb|py) - used to describe how it generates cards from a note
      sides.(txt|lua|json|xml|csv) - used to describe how many sides the note has
      fields.(txt|lua|json|xml|csv) - used to describe the fields available to the note
      /sides
        -- Display templates for card sides as named in the sides file --
        side-name-A.(html?|md|markdown|txt)
        ...
        side-name-Z.(html?|md|markdown|txt)
        -- End sides --
      /resource
        -- Extra arbitrary files used by this template
        -- When a foreign note type is imported, resources go here by default to prevent name conflicts
    /resource
      -- extra arbitrary files used by all templates
  /decks
    /deck-id
      manifest.txt
      /note-id - Some unique name for the note
        tags.txt - Tags used by this card. Since this is a filesystem database, there will be some redundancy here with the upper tags.txt file.
        .template - solely refers to a note id
        .schedule - solely contains scheduling data for the card - the history of this can be used for anonymous research purposes fields.(txt|lua|json|xml|html?|csv) - A special override file for defining all fields in one file. This would be useful for import/export of foreign note types or translating between incompatible ones. This would forego use of the /fields folder. Deleting/replacing that folder would be fine since git could revert it.
        /fields
          -- Data for fields as named in the note template fields file --
          field-data-A.(html?|md|markdown|txt)
          ...
          field-data-Z.(html?|md|markdown|txt)
          -- End fields
  /media
    .gitignore
    -- arbitrary media files for use in decks
    filenameA
    filenameB
    ...
    filenameZ
/.base-user
  -- Same structure as above, but acts as a sort of base class
  -- Content here is available to all users and cannot be modified by users.
  -- Users can, however, override content here by creating duplicates in their directory.
  -- This would include stuff like default note types, default scripts, default tags, etc.
```

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

Add Tag (for user)

Delete Tag (for user)

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

Get Note (for card)

Add Tag (to note)

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

In other SRSs, they just say "pick one". My problem with this is if a card was added on one system and a note type was modified on another, one of those changes will be lost.

Instead, it should give the user an interface to merge. In most cases, they'll simply need to 

One thing I'd like to facilitate is sharing of cards/decks in a merge friendly way. Like, being able to update an RTK deck to newer revisions without it screwing up history.

I very much suspect there will be files introduced that should be synced, but not with Git. One example might certain kinds of content in the media folder. Another would be, say, embedded databases that addons choose to use. It'd be nice to use Rsync, but it's not well supported on Windows. Nearly every syncing solution I've found has serious portability problems. It may be best to roll my own using a portable FTP library and just version the checksums of non-versioned files. It's not very efficient, but it'd get the job done.

This might also be a job for plugins/addons.
