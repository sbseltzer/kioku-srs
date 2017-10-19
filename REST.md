# Short Term
This segment will keep track of the critical path.

## Types / Concepts
## REST API

### GetNextCard
Input:
```
GET <endpoint uri>/card/next
```

Outputs:
```
200 OK
{
  "id" : "<the card id>",
  "front" : "front text",
  "back" : "front text<br>back text which included the front text",
  "buttons" : [{"title" : "Again", "grade" : 1}, {"title" : "Good", "grade" : 3}, {"title" : "Easy", "grade" : 4}]
}

404 Not Found | 500 Internal Server Error
{
  "error" : "error message"
}
```

### GradeCard
Input:
```
POST <endpoint uri>/card/grade
{
  "id" : "<the card id>",
  "grade" : "<grade as specified by buttons>"
}
```

Outputs:
```
200 OK
{}

500 Internal Server Error
{
  "error" : "error message"
}
```

# The Future
This section represents what I hope to acheive some day.

## Types / Concepts

### Device
A native device that hosts the controller (serverside) portion of the application.
### Plugin
Native binary for a Device that's loaded at runtime which implements an interface to provide extra functionality on the Device. The most important use-case would be a script engine.
#### Script Engine
Plugin that implements an interface for running scripts, and binding required API functions.
### User
A User has a collection of Addons, Options, Decks, Media, and so on for a particular Device. This may be referred to as the User Tree. The concept of a User is data-oriented and Device-local in that it exists as a repository. When using the term "User" in the context of hosting the Model (such as a GitHub user repo as a remote that the User has push access to) or in the context of hosting the Client (such as a web interface like AnkiWeb that connects to an internal Device) it does not quite mean the same thing.
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

Issues: One issue I've had with Anki in generated clozes is how scheduling info gets associated with its cloze number. If someone wanted to change the order of clozes later or remove one, there's the possibility of erroneously having one card inherit scheduling information of another. It's really the content being tested on within the card that counts, but scheduling info should not necessarily be reset just because content changes. This is not really a problem Anki aims to solve, but it'd be nice if that could be avoided here.

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
