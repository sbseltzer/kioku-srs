# Model
This file is a draft of how data is managed in Kioku.

## Goals


## Filesystem
This is a draft of the git-managed filesystem, which acts as a sort of database.
```
.version - Implementation version information for this device
/.plugins - where native (compiled/binary) plugins go
  /lua-scripting-engine
    -- files
  /python-scripting-engine
    -- files
  /ruby-scripting-engine
    -- files
/user-name
  /.git - This is a git repository
  .version - Implementation version information for this user.
  tags.txt - list of available tags
  /options
    general.[lua|json|xml] - General user options
    addons.[lua|json|xml] - General addon options
    interface.[lua|json|xml] - General interface options
  /addons - where addon scripts go - this is the root include path for scripting all languages
    /my-addon-folder
      info.(lua|json|xml|txt) - Contains stuff like plugin dependency, where it came from, etc.
      install.(lua|rb|py) - How it installs/uninstalls itself.
      options.(lua|rb|py) - Specialized options available to client.
      -- script files go here
      some-script.(lua|rb|py) - Some arbitrary script file
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

## API

Install Plugin (for device)

Get Users (for device)

Install Addon (for user)

Uninstall Addon (for user)

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

Search Using Filter String (for cards | for notes)

Sync (for user)

Get Full Revision History (for user)

Restore File to Previous Version (for user)

Get Anonymous Scheduling Data (for user)

Export Deck (for user)

Import Deck (for user)

## Syncing Considerations

Being Git, there's always the possibility of a merge conflict.

In other SRSs, they just say "pick one". My problem with this is if a card was added on one system and a note type was modified on another, one of those changes will be lost.

Instead, it should give the user an interface to merge. In most cases, they'll simply need to 
