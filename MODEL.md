# Model

This file is a draft of how data is managed in Kioku. I think this will probably drive the high level design.

For Types/Concepts and the REST API, see the bottom of this page.

## Goals

- This is not a Git GUI.
- The Model is mostly composed of flatfiles backed by one or more Git Object Databases.
- Metadata has high granularity (split between many files) to simplify history and merging.
- In general, commits aim to be discrete - one commit == one change.
- Content can be shared. Addons, Templates, Decks, and individual Notes.
- Users are empowered to contribute back to shared content in a relatively user-friendly way.
- Multiple sync methods can be supported.
- Multiple sync hosts can be supported.
- User has freedom to work within the tree as they see fit for their own purposes.

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
/user-name.git - The bare repository. Only present on model hosts (which might be the local machine).
  /extern - Non-versioned data
/user-name - If the model host is on the local machine, this can be deleted between sessions (provided there are no unversioned, unsynced files).
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
      default.(config) - The default options. These can, of course, be reset by checking out the original version of it.
      -- arbitrary extra option groups.
      some-custom-group.(config)
  /addons - where addon scripts go - this is the root include path for scripting all languages
    .gitignore - CONSIDER - Should the contents of this folder be ignored? Addons may be git versioned separately by advanced users, or installed/updated via archive. In fact, an addon package manager might make for a good meta-addon.
    .gitmodules - CONSIDER - Some addons might be git repositories. The user would need an interface for managing this. If they start their own addon repository here, it'd be nice to provide a user-friendly interface to control how it's kept updated.
    .addons - CONSIDER - Instead of submodules, a special way of managing addon subprojects that fulfills the needs of Kioku.
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
      /.git - CONSIDER - What if shared templates were forks? One issue would be that decks that use them could run into problems if a template drastically changed its format.
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
      .generated - indicates if this is generated from another deck as a custom study session
      /.research - CONSIDER - This would be a special .git that is handled internally to version just the .schedule files. This might make it simpler to track research data anonymously without needing to post-process history.
      /.git - CONSIDER - What if shared decks were forks? One issue would be that templates would also need to be forked, and that could become complex (unless the template could be embedded in the deck).
      .option-group - soley references option group
      manifest.txt
      .scheduler - Solely references a scheduling strategy
      /notes
        /note-id - Some unique name for the note
          tags.txt - Tags used by this card. Since this is a filesystem database, there will be some redundancy here with the upper tags.txt file.
          .template - solely refers to a note type template - maybe to overcome shared deck dependency, this could refer to a remote so it can be imported.
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

## Repository Usage

I've now got at least 3 sections detailing all the horrific problems with using Git as a backend. Namely because there are so many options. It's looking more and more like the best option is a roll-my-own type solution.

To plot this out in simpler terms, let's look at what some of the problems to deal with are.

1. How to keep overall filesystem state synced.
1. How to share content.
1. How to download shared content.
1. How to hack on shared content.
1. How to contribute your hacks on shared content back to their source.
1. How to manage/sync large content.
1. How to prevent large content from taking up space when you feel permanently done with it.

#### Submodules
I think forcing the use of submodules is a poor choice because they impose a serious burden on any user who wishes to work within the spaces managed by Kioku. It's not a bad idea to make Kioku robust enough to handle cases where users deliberately use submodules, but it should not be a targeted use-case.

#### Subtrees
I'm still thinking through the implications of using subtrees, but the first thing that stands out is that their content is part of the parent repository history. This raises concerns with imported media. If someone feels totally done with a deck, addon, template, or media, they should be able to get rid of it with no strings attached. Subtrees also impose a burden on users in that they shouldn't mix commits with child and parent, and also have somewhat complex push requirements when contributing back upstream. I'm also not sure they can be branched without a fork.

#### Subrepositories
This is more user friendly, I think, but like subtrees it involves getting child content into parent history. It's nice because people can do what they want with that subrepository while staying out of Kioku's way and keep all their stuff synced easily. This is especially great for those who are making changes to stuff and want to still be able to sync without creating a fork.

#### Roll-my-own
I think I might need to take this route.

Some helpful info.
- https://git.wiki.kernel.org/index.php/SubprojectSupport?highlight=%2528subproject%2529
- https://stackoverflow.com/questions/23550689/git-nested-repositories-composer-vs-submodules-vs-subtree-vs
- http://debuggable.com/posts/git-fake-submodules:4b563ee4-f3cc-4061-967e-0e48cbdd56cb
- https://git-scm.com/book/en/v1/Git-Tools-Subtree-Merging
- https://stackoverflow.com/questions/12668711/git-using-subtree-or-submodule-to-manage-external-resources
- https://stackoverflow.com/questions/12078365/how-exactly-does-git-submodule-work/12078816#12078816
- http://blog.nwcadence.com/git-subtrees/
- https://www.atlassian.com/blog/git/alternatives-to-git-submodule-git-subtree
- https://git.wiki.kernel.org/index.php/GitSubmoduleTutorial#Gotchas
- https://stackoverflow.com/questions/996164/is-anyone-really-using-git-super-subprojects

## User Restraint

One point of concern is Users getting too "clever" by manipulating history directly with a CLI. If they seriously tamper with things in dangerous/opaque ways (history rewrites, squashing, etc.), that's on them. The application will interpret that as it will, but it may not do what they think.

To make things fair, all documentation that describes history-reliant functionality (i.e. Git as a database) must be aggregated for advanced Git users to review.

It'd be convenient for application commits to be clear and separate from CLI commits. It'd also be convenient to prevent CLI commits that modify metadata. However, the application should be robust enough to still utilize such history changes by simply analyzing history on a per-file basis. This is why metadata is usually dedicated to individual files.

So far this has assumed a single-branch scenario. Branches could be a very powerful tool, but also seriously complicate matters. All application commits would happen on whatever branch the user is set to. Devices would need to always fetch/push all branches and select the most recent HEAD for application commits.  Before supporting such a feature, there'd need to be some very clearly defined use-cases to flesh out merge strategies.

One possibility is to preinstall hooks that prevent the user from doing anything too dangerous, like committing too many unrelated changes at once.

Now that I think of it, what really bugged me about Anki was the fact that I couldn't manage things with Git IN ADDITION TO the built-in syncing method. Part of why I'm doing this is for user freedom. Imposing these kinds of restrictions and designing it into a corner would sorta defeat the purpose.

One option would be to have Kioku manage things in special workdir paths. Like `.kioku/` or `.deck/` and so on, instead of `.git`. This would mean the user can version any part the collection with Git as usual without interference, and Kioku will just do its thing and automatically version stuff as specified by the REST interface and config files, ignoring the fact that there are subprojects. The only problem with this is it's potentially not very space efficient, especially if media is being versioned. It also could makes user interfaces for sharing less elegant, as it would essentially be working like subtrees from Kioku's point of view.

I'm imagining a few possible workflows. The one that's most familiar to me is the addon developer. I'd want to have full control over my versioning of an addon. Here are some ideas.
- Work on it directly in the worktree, either letting Kioku version it until I'm ready to make a repository, or versioning it myself within Kioku.
- Work on it outside of the worktree pushing to a remote and accepting updates to it inside Kioku, perhaps on a development branch (which would need to be an advanced option in the Kioku interface).
- Download an addon with Kioku as a user, but modify it directly in Kioku like a noob. Kioku might be able to protect the user from this by automatically creating a user branch and switching to it, versioning that. They'd need to know that in order to sync it they'll need their own forked remote to push to. It'd also need to notify them of how to properly contribute by creating a fork (which could be automated via API).

Actually, [subtrees](http://blog.nwcadence.com/git-subtrees/) fulfill a lot of the qualities needed for shared content. It keeps everything in user history and makes it relatively simple to change how things are managed later. One thing it would need to be careful of with stuff like decks is ignoring stuff like user-specific metadata. Someone could whitelist it in a subproject. Subtrees would also split out commits from the parent repository - that is, although they're all part of the same repository.

Still, I'm liking the idea of a shallow clone with a user branch. I think the big benefit of subtrees is that they don't need to fork in order to sync new changes. They can fork if/when they want to contribute upstream. The downside is it's less friendly to users working within the tree.

[This StackExchange Thread](https://stackoverflow.com/questions/23550689/git-nested-repositories-composer-vs-submodules-vs-subtree-vs) has a very comprehensive discussion of submodules and subrepositories. After reading it, I'm thinking subrepositories is the way to go. When working from the parent, it's all versioned. The only problem is that when synced, they won't be subrepositories anymore. This can be fixed with a roll-your-own-git-init method by versioning some kind of remote info file. The thing is, this solution starts to look a lot like submodules, especially when it starts pointing to a particular branch. Reinventing the wheel and all that rubbish. The big drawback with subrepos and subtrees having their history duplicated in the parent is they can't be decoupled later. I remember I had a Kana deck once that came with what seemed like hundreds of poorly compressed images and sounds. When I was done with that deck, I had no desire to keep it around (at least not on my device). It simply took up too much space. This is where a submodule-like approach is powerful. It actually could be considered a compression feature. You could choose to (temporarily) unsync certain things with certain devices in the interest of saving space. The reverse is also true - most git hosts only limit repository size per repository.

Then again, the history rewrite technique is appropriate at the top level when doing things like that. Still very messy.

The following sections are very related to this issue.

## Syncing Considerations

In other SRSs you can end up in situations where they say "pick one". My problem with this is if a card was added on one system and a note type was modified on another, one of those changes might be lost. Anki is picky when it comes to modifying fields on note types - requires a total database upload. There's probably a good reason for this, but it's not terribly obvious.

Being Git, there's always the possibility of a merge conflict. In our case, it should give the user an interface to merge. In most cases, they'll simply need to "pick one" on a per-file basis. At the very least, show version A, version B, say which devices they're from and give them an interface to arbitrate.

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
Sharing parts of a collection is desirable. Note Templates, Addons, Decks, and individual Notes/Cards are great candidates for sharing with others. It'd be very nice if these could be managed by separate Git repos. There are many use-cases that become available with Git, and many ways to go about it.

Let's say we use Git to manage parts of a collection individually. This would allow for sharing. How do we keep these synced?

Well, first it depends on whether the user intends to modify the content. If they cloned a shared *thing*, it now needs to be forked before they can continue to sync it. This means they'd need to create a bare repository somewhere and add it as a remote. The controller can AT LEAST create a local one for them by default, but now they need to know that a remote one needs to be created before it can be available on other devices. In addition, the other devices need to know that there's a subrepository of some kind to clone/pull.

The basic pattern is this:
- The user tree can contain subprojects (not necessarily subtrees or submodules - simply git repositories).
- These subprojects can have the following remotes
  - An origin remote (required) - the upstream original - for example, an addon would have its origin be the authors repository.
  - One or more sync remotes (required) - the place(s) the user syncs to which they have push access to - this might be the same as the origin remote.
- The user has a way of keeping track of subprojects similar to subtrees or submodules so the controller knows what to sync and where.
- When the user syncs, all subprojects must also be synced to keep themselves at their HEAD. This is opaque to the user - it should appear as though they're all part of the same collection.
- If changes are available on the origin and the origin is different than all the sync remotes, it is considered an update, and the user is given the option to update.
- If the user wishes to contribute back to the origin via GUI, they are given a GUI that goes through all possible relevant changes to construct a patch (a la checkboxes) on a separate branch starting from where their last common point is. If it can be submitted as a PR via the appropriate API, that action is performed and they are given the link to the PR so they can work with the maintainer. Otherwise it generates a patch file and they are given the maintainers email address to send it to.

It's important to keep in mind that when using Git to sync anything in a collection, there's potentially a need for a fork. Forking can't be automated if the user wants to use a host they can't control. Hosts with an API (such as GitHub or GitLab) are less of a problem so long as the user configures everything properly. If the user is a novice Git user, it's probably not a good idea to have a complex tree, even if it's mostly abstracted away from them.

Okay, freaking cool concept - host APIs would allow opaque repository management. This means Kioku could be comprised of any repository combination and have it automated for those hosts.

GitHub in particular could act as a public sharing framework for Kioku content. It even has a [search API](https://developer.github.com/v3/search/#search-repositories) so that repositories could indicate that they're for Kioku via something like tags or topics. This way a list of shared content could easily be embedded/sorted into the UI via the Kioku REST API.

### Addons
With Addons, there's a strong analogy to submodules, but some people may prefer to modify them and keep them versioned in their own history.

### Decks
Sharing decks is something any respectable SRS supports. With Git, a unique use-case arises: Open Source SRS Decks. The RTK deck could be updated to stay in sync with Kanji Koohi. In addition, the Template for it may need updates to ensure hotlinks still work (a problem I encountered with a shared Anki deck). Perhaps dozens of people needed to make that change when one of them could have made a PR and solved it for everyone. If a keyword was changed for the better, but the user had already changed it to suit their preferences, a merge conflict would occur. This could technically happen on any field, but the others are less likely to change. The same might happen if the user makes slight adjustments to the hotlink on the Template, or remove fields that are maintained upstream. Additive changes on separate lines would be safest for the user so as not to compromise merge safety, especially the kind that cause clientside postprocessing.

For instance, I often add senses of a keyword in parentheses to the end of ambiguous keywords in my RTK deck. If the keyword was modified upstream to better reflect the sense it was meant in, a merge conflict would occur when pull from the upstream repository to update my fork. This could have been mitigated by adding a new field to the template, and using it in one of two ways.
1. Modify the front side of the template to format the senses in parentheses for me.
2. Modify the front side by adding a script contained in my media folder that postprocesses the card, modifying the keyword to include the value in my senses field.
The latter is the safest way, but it's the more complicated way to go.

If a bad maintainer causes a change to field IDs, or worse - sides, that could cause all sorts of unexpected problems for users, since now all of their notes would need to change.

Decks are dependent on Note Templates, and in very specialized cases Addons.

## Security

A big part of this will be what addons can modify. Some things should be sacred.

## Types / Concepts

### Device
A native device that hosts the controller (serverside) portion of the application.
### Plugin
Native binary for a Device that's loaded at runtime which implements an interface to provide extra functionality on the Device. The most important use-case would be a script engine.
#### Script Engine
Plugin that implements an interface for running scripts, and binding required API functions.
### User
A User has a collection of Addons, Options, Decks, Media, and so on for a particular Device. The concept of a User is Device-local in that it exists as a repository. When using the term "User" in the context of hosting the Model (such as a GitHub user repo as a remote that the User has push access to) or in the context of hosting the Client (such as a web interface like AnkiWeb that connects to an internal Device) it does not quite mean the same thing.
### Script
Any file that uses a script format as made available by a corresponding Script Engine.
### Addon
A collection of files and scripts to extend the server implementation.
#### Addon Type
Addons can be one or more types. The Addon metadata instructs the Scripting Engine what types it can serve as, which affects what functionality is exposed to the scripting environment.
- Scheduler
- Host API Extension - Adding an interface for communication with GitHub, GitLab, BitBucket, etc.
- REST Extension - Adding arbitrary extensions to the REST interface for use by whatever client you choose. This might be used in combination with a script attached to cards to get special information provided by the addon in the client (such as a SQLite database). Very powerful concept.
- Importer/Exporter
- Template Installer 
- Card Extension - Hooks into the generation of cards by doing some kind of pre/post-processing based on something like template or tags. Typical use-case might be specifying extra Javascript to insert at the end.
- Media Installer
- Option Group Extension

### Formats
This section describes abstract file formats that could vary.
#### Script Format
Facilitated by a Script Engine. Must support an HTTP client, and be able to bind to C functions.

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
### Media Directory
### Media File
An arbitrary file of any type.
### Media Path
Works similar to a `PATH` environment variable. It's list of Media Directories that form an aggregate collection of files. They are limited to being within the User directory tree.
### Media Map
Available on a per-device/user basis.
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
A list of field names. These determine the data that a Note using this Template can define. These are also used by Template Sides to specify where Note Field Data is placed. The Template Card Generator uses these to determine how the Note Field Data is processed and through Sides into Cards.

VALIDATION:
- All fields specified by the fields file should appear at least once on one or more sides. The fact that there are unused fields in a template must be available to the client in some way. Anytime this validation status changes from valid to invalid, it must be reported to the client and resolved in one of two ways:  
  1. Ignore it.
  2. Offer to remove the field from the list and delete it's data from all notes that use it.

#### Sides
These describe the textual format for each side of the card using a special markup for placing fields. The Template could define any number of sides, but generally there will be 2. Composed of a side list file and a number of side template files.

These are processed in a manner similar to a CGI script.

VALIDATION:
- Every name in the sides file must have a corresponding side template file. There can be extraneous side template files, but this must be reported to the client. If a side template file is missing, this must be reported to the client and resolved in one of two ways:  
  1. Create a blank file with that name and offer them an interface to edit it.
  2. Offer to remove the specified side from the list.
- Validate Fields.

#### Card Generator
Facilitated by a portable subset of Scripts; A scripted API for generating cards from a note. These are defined as part of a Note Template. It's important that these are portable to all devices.

These determine how many cards to generate, their IDs, and the content of their fields.

Common use cases:
- Reverse cards would be a relatively simple generator.
- Cloze Deletion cards would have a special generator that defines what to replace text matching `{{c%d::text(::hint)?}}` with for N cards.

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

Poll Notifications (for device)

Poll Updates (for addons | for decks | for templates)

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
