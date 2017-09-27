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
    general.(lua|json|xml|ini) - General user options
    addons.(lua|json|xml|ini) - Addon options
    interface.(lua|json|xml|ini) - User interface options
    schedule.(lua|json|xml|ini) - Scheduling options
    sync.(lua|json|xml|ini) - Syncing options
    /groups - option groups for decks
      default.(lua|json|xml|ini) - The default options.
      -- arbitrary extra option groups.
      some-custom-group.(lua|json|xml|ini)
  /addons - where addon scripts go - this is the root include path for scripting all languages
    /my-addon-folder
      .version - Implementation version for this addon. This is checked against the device .version file for ABI compatibility.
      info.(lua|json|xml|txt) - Contains stuff like plugin dependency, where it came from, etc.
      manifest.(lua|json|xml|txt) - Data-oriented install/uninstall information.
      options.(lua|json|xml|ini) - Specialized options available to client.
      startup.(lua|rb|py) - This is executed when the SRS server launches.
      -- script files go here
      some-script.(lua|rb|py) - Some arbitrary script file
      LOG.txt - Addon-specific logging.
  /templates
    /note-template-id
      generator.(lua|rb|py) - used to describe how to generate cards from a note
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
      -- extra arbitrary files accessible to all templates
  /decks
    /deck-id
      .option-group - soley references option group
      manifest.txt
      /notes
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
      /media - Decks have their own separately versioned media folders. If the deck is deleted, the media (and space overhead) can go with it.
        -- arbitrary media files for use in this deck
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

Get Note (for card)

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

I very much suspect there will be files introduced that should be synced, but not with Git. One example might certain kinds of content in the media folder. Another would be, say, embedded databases that addons choose to use. It'd be nice to use Rsync, but it's not well supported on Windows. Nearly every syncing solution I've found has serious portability problems. It may be best to roll my own using a portable FTP library and just version the checksums of non-versioned files. It's not very efficient, but it'd get the job done.

Alternatively, there are [scripts like this](https://robinwinslow.uk/2013/06/11/dont-ever-commit-binary-files-to-git/) that purge binary files by rewriting history. You could even preserve commit times by [faking them](https://stackoverflow.com/questions/3895453/how-do-i-make-a-git-commit-in-the-past#3898842). On the other hand, you end up with incompatible histories. This would basically put the user in the same place that Anki does when modifying note types, but for every time media is deleted/modified (or perhaps only when they want to reclaim space). Not so good. This is where there'd need to be a companion sync method (maybe a tiny metadata repository) that assists in getting a re-clone.

This might also be a job for plugins/addons. I think it'd be nice to offer users the option to change out how they sync certain kinds of files.

### Some ideas for syncing media
- Version them right along with everything else.  
  - To hell with the user.
- Version them with everything else, but provide surgical removal of previous version via history rewrites.  
  - Complex to implement.
  - Requires a full repo re-sync when the user wishes to reclaim space.
- Version them in their own repository, and do a history rewrite for binary files when the user wants to reclaim space.  
  - Gives user the opportunity to version anything and prune later.
  - Easy to delete a media repository wholesale.
  - Hard to implement history replay.
  - Requires a media repo re-sync in order to fully remove a media file.
  - Requires an extra remote repository.
- Only version checksums for unversioned files, then use something like SFTP for actual syncing.  
  - Basically what Anki does.
  - Easy to implement.
  - Doesn't rewrite history.
  - Consistently avoids bloat.
  - Files managed this way can't be versioned, which might not be desirable in some cases.
  - Requires help from an external protocol and some other media hosting service.
- Version checksums like above, but instead have each media file in its own little repository.  
  - Not terribly hard to implement.
  - Files can be versioned and later be permanently deleted without any complexity.
  - Rewriting history for a single-file repository is much simpler than surgical strikes in a collection.
  - So many repositories, though. This would be quite nasty to self-host.
  
## Sharing

Sharing decks, note types, notes, and addons is desirable. In what way, though? There are many use-cases that become available with Git, and many ways to go about it.

With Addons, there's a strong analogy to submodules, but some people may prefer to modify them and keep them versioned in their own history.

Decks are the trickiest. 

## Security

A big part of this will be what addons can modify. Some things should be sacred.
