# Kioku (記憶) SRS

A portable, versioned, distributed, self-hosted spaced repetition system (SRS) inspired by Anki and backed by Git.

## Build Status

**Travis CI:**
[![Travis CI Build Status](https://travis-ci.org/seltzy/kioku-srs.svg?branch=master)](https://travis-ci.org/seltzy/kioku-srs)

**Gitlab CI:**
[![GitLab CI Build Status](https://gitlab.com/seltzer/kioku-srs/badges/master/build.svg)](https://gitlab.com/seltzer/kioku-srs/commits/master) 

**AppVeyor:**
[![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/kpkdyp27m3fes3jd/branch/master?svg=true)](https://ci.appveyor.com/project/seltzy/kioku-srs/branch/master)

If you wish to build this yourself, see [BUILDING.md](BUILDING.md).

## What's an SRS?

SRS stands for Spaced Repetition System. Many people simplify it to "smart flashcards", but frankly, that does not do it justice. SRS is based upon the Spacing Effect, where there is constantly changing optimal interval of time between exposing yourself to information for the purpose of long-term retention. SRS uses an algorithm that finds the optimal spacing based on your ability to recall the information you're testing yourself on. Testing is sort of a bad word for this, as it's really more like exposing and pressing a button based on whether you remembered or not. To utilize an SRS properly, there should be very little thought involved when recalling. You either remember well or you don't. It's not so much a "test" in that sense; it's a "rep" as if you were lifting a small weight.

Examples of SRS applications would be SuperMemo, Anki, Mnemosyne, and perhaps more popularly, DuoLingo (though it's rather young by comparison).

## Why another SRS?

This is really born of my frustration with Anki (and other SRS solutions). I've used Anki for over a year. It does its job wonderfully. There are many user-created decks and addons, but it's a nightmare to actually port addons to multiple devices. Mnemosyne's modding framework is a bit more thought out, but it suffers from a similar class of problems.

This section will review the main problems Kioku aims to solve.

### Portability

There are two kinds of portability here. Application Portability, and Addon Portability. Kioku concerns itself with both.

Anki and Mnemosyne are fairly portable to most Desktop and Mobile environments. The desktop apps have some problems that stem from using Python/Qt, and the mobile apps ultimately suffer from the lack of Addons and interface features.

Kioku proposes a decoupled architecture and choice of technology that's designed with full portability in mind, even if it doesn't happen all at once. If everything is done right, someone could trivially modify Kioku to run in embedded Linux terminal, if you're brave enough to implement that. It's going to be designed more with web browsers in mind, but Kioku aims to facilitate creating other interfaces, even if their feature set is smaller.

### Hackability

Anki's modding framework is extremely permissive, but not very well thought out. Mnemosyne doesn't appear to have a very active modding community in the way Anki does. Both require use of Python and neither are portable to mobile or web platforms.

Kioku aims to make modding possible in any language, so long as it can be bound to C/C++ and support an HTTP client stack.

### Progress is Sacred

No other SRS, so far as I'm aware at the time of writing this, supports full revision history. That is, every file has a history of changes that can be rolled-back to at any time. It's like having an infinite undo/redo button for everything. Most SRS applications support backups and learning history, but arbitrary files cannot be versioned. Worse yet, in Anki it is impossible to implement your own without potentially compromising database integrity. Not good.

With Kioku being backed by Git, every action has a chance to be versioned right off the bat. Every. Single. Action. If your computer crashes in the middle of editing a card and Git is set up to amend your changes to it as you write (and commit when you save), there's a very low likelihood of losing ANY work.

Being flatfile based, you also stand little chance of ending up in a corrupted state. Applications backed by a database (particularly SQLite) are vulnerable to this. They tend to mitigate it by saving compressed backups. A Git repository serves this purpose intrinsically.

### User Freedom

Mnemosyne did something right by providing sync options and self-hosting. Anki not so much, but it has made progress.

Kioku aims to make SRS implementation easy. If DuoLingo decided one day to use it as their internal backbone, they could.

If a user wants to use alternative syncing mechanisms, they can implement or download an addon that facilitates that. Same can be said for *The Algorithm*.

### Collaborative Learning

Something no other SRS has done yet is deck/template collaboration. Being able to fork decks and contribute to them is something that Git can facilitate. Still, this is not a simple problem to solve. Particularly when it comes to users who have forked a deck and want to stay up-to-date with them. This becomes difficult (or perhaps impossible) if the user makes their own modifications to the deck content, which is something that should be encouraged.

On the other hand, there is no precedent for this. Kioku does not have to implement this perfectly. It can simply facilitate it and improve in future iterations.

