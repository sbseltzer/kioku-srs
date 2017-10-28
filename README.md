# Kioku SRS (記憶SRS)

A portable, versioned, distributed, self-hosted Spaced Repetition System (SRS) inspired by [Anki](https://ankisrs.net) and backed by Git.

## Build Status

This section is for developers. If you wish to build this yourself, see [BUILDING](BUILDING.md).

**Gitlab CI:**&nbsp;&nbsp;Linux  
[![View GitLab CI Build Status](https://gitlab.com/seltzer/kioku-srs/badges/master/build.svg)](https://gitlab.com/seltzer/kioku-srs/commits/master) 

**AppVeyor:**&nbsp;&nbsp;Windows  
[![View AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/kpkdyp27m3fes3jd/branch/master?svg=true)](https://ci.appveyor.com/project/seltzy/kioku-srs/branch/master)

**Travis CI:**&nbsp;&nbsp;OSX  
[![View Travis CI Build Status](https://travis-ci.org/seltzy/kioku-srs.svg?branch=master)](https://travis-ci.org/seltzy/kioku-srs)

## What's an SRS?

SRS stands for Spaced Repetition System. Many people simplify it to "smart flashcards", but frankly, that does not do it justice. SRS is based upon the Spacing Effect, where there is constantly changing optimal interval of time between exposing yourself to information for the purpose of long-term retention. SRS uses an algorithm that finds the optimal spacing based on your ability to recall the information you're testing yourself on. Testing is sort of a bad word for this, as it's really more like exposing and pressing a button based on whether you remembered or not. To utilize an SRS properly, there should be very little thought involved when recalling. You either remember well or you don't. It's not so much a "test" in that sense; it's a "rep" as if you were lifting a small weight.

Examples of SRS applications would be SuperMemo, Anki, Mnemosyne, and perhaps more popularly, DuoLingo (though it's rather young by comparison).

## Why another SRS?

This is really born of my frustration with Anki (and other SRS solutions). I've used Anki for over a year. It does its job wonderfully. There are many user-created decks and addons, but it's a nightmare to actually port addons to multiple devices. Mnemosyne's modding framework is a bit more thought out, but it suffers from a similar class of problems.

This section will review the main problems Kioku aims to solve.

### Portability

When talking about portability in the software world, we mean "how many different kinds of devices can this run on?"

That is, can it work on Windows, OSX/iOS, Android, and various flavours of Linux? Which versions of those can it run on? Which compilers can build the application? Are any of them missing features? These are valuable questions to ask as a programmer if you want a broad user-base. Portable applications are made deliberately from the start. Rarely can an unportable application suddenly become portable without a great deal of effort. Programs written in high-level languages like Java and Python can help with this (which is what Anki and Mnemosyne do), but this can have substantial quirks when it comes to implementing a UI. They're also generally more CPU/Memory intense, which is not good for servers or constrained mobile devices.

There are also two kinds of portability here. Application Portability and Addon Portability. Unlike most other SRS applications, Kioku concerns itself with both.

Anki and Mnemosyne are fairly portable to most Desktop and Mobile environments. The desktop apps have some problems with IMEs (foreign keyboard emulators) that stem from using Python/Qt, and the mobile apps suffer from the lack of addons and interface features. There's also more maintenance overhead with the mobile applications due to the lack of a unified codebase.

Kioku proposes a design with full portability in mind, even if it doesn't happen all at once. Features added to desktop versions should easily port to the mobile versions. If everything is done right, someone could trivially modify Kioku to run in an embedded Linux terminal, that is if they're brave enough to implement that. It's designed more with web browsers in mind, but Kioku aims to facilitate creating other interfaces, even if their feature set is limited by comparison.

### Hackability

This will be a boon for developers and users who wish to extend Kioku's behaviour.

Anki's modding framework is extremely permissive, but not very well thought out. The reliance on Python and Qt makes addons clunky and unintuitive. Mnemosyne doesn't appear to have a very active modding community in the way that Anki does, and there is no convenient way for users to find them. Both require the use of Python and neither are portable to mobile or web platforms.

Kioku aims to make modding possible in any language, so long as it can be executed from C/C++ and support an HTTP client stack. Every supported platform that the Kioku backend can run on will have some kind of scripting available.

### Progress is Sacred

No other SRS, so far as I'm aware at the time of writing this, supports full revision history. That is, every file has a history of changes that can be rolled-back to at any time. It's like having an infinite undo/redo button for everything. Most SRS applications support backups and learning history, but arbitrary files cannot be versioned. Worse yet, in Anki it is impossible to implement your own without potentially compromising database integrity. Not good.

With Kioku being backed by Git, every action has a chance to be versioned right off the bat. Every. Single. Action. If your computer crashes in the middle of editing a card and Git is set up to amend your changes to it as you write (and commit when you save), there's a very low likelihood of losing ANY work.

Being flatfile based, you also stand little chance of ending up in a corrupted state. Applications backed by a database (particularly SQLite) are vulnerable to this. They tend to mitigate it by saving compressed backups. A Git repository serves this purpose intrinsically.

### User Freedom

Mnemosyne did something right by providing sync options and self-hosting. Anki not so much, but it has made progress.

If a user wants to use alternative syncing mechanisms, they can implement or download an addon that facilitates that. Same can be said for *The Algorithm*. More importantly, these kinds of modifications will be portable, unlike the other SRS applications.

Kioku aims to improve usability for power-users. One thing that annoys me in particular is when the data in your SRS (i.e. cards, decks, templates) are either not available on the filesystem because they're packed in a database, or are unsafe to edit directly for integrity reasons.

Kioku also aims to make SRS implementation easy for developers. If DuoLingo decided one day to use it as their internal backbone, they could.

### Collaborative Learning

Something no other SRS has done yet is deck/template collaboration. Being able to fork decks and contribute to them is something that Git can facilitate. Still, this is not a simple problem to solve. Particularly when it comes to users who have forked a deck and want to stay up-to-date with them. This becomes difficult (or perhaps impossible) if the user makes their own modifications to the deck content, which is something that should be encouraged.

On the other hand, there is no precedent for this. Kioku does not have to implement this perfectly. It can simply facilitate it and improve in future iterations.

