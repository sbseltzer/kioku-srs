# A place for random thoughts

I've had a lot going on since I last updated this project (Dec 15 '17, appx 7 months ago).
Been thinking recently about where this project went right and wrong.
It was less than 3 months of work, but in short, I did a great job learning and playing but did not make a lot of progress in making an SRS.
On a personal note, it was a lot like what had been going on for me at work up until the person I reported to left the company.

Overall, this project was too principled and too architected. I made 2 major observations today:
1. That maybe making the data model card-centric would be a good idea (inspired by [this article](https://eshapard.github.io/anki/thoughts-on-a-new-algorithm-for-anki.html))
2. That having a portable architecture is more important than portable implementations.
3. Ok one more: that this is going to be most attractive to power-users and that it should be the primary concern.

More on #2 there, it would have been better to prototype with bash scripts cobbled together by C system calls and a light RPC framework. Actually a UI or CLI might have been more important to start with. A CLI probably would have kept things focused, but I'm guessing it could get blown out of proportion. I guess whatever would have been easier at the time... Anyway, things like built-in video players and home-made file system abstraction are nice, but not necessary. And heaven forbid letting generic programming take hold. I was already doing an okay job with that by forcing myself to use C, but SRS implementations widely vary and I wanted to be magic darn it! That being said, this application would benefit from a sprinkle of C++. C calling conventions are going to continue to be my preference, I think. The 1998 standard is plenty portable. C++11 is nice and all, but it has no reason to be in the interface. There are things that are important to have a mind for from the start, such as UTF-8 handling and localization, but a lot of the implementation/portability/scripting/flexibility stuff can wait. Let's start with "is an SRS", "uses Git", "supports browsers", and "does things I care about like cloze-like card generation and media playing". Even if it has to use hugo for generation and such ([which relies on Go and might be more portable than Ruby/Jekyll, by the way](https://stackoverflow.com/questions/33872612/are-golang-binaries-portable#33873000)).

Recently my SRS usage habits have changed. I've shifted to spending more time curating cards than actually repping, and I think open source decks should have more serious attention given to them.
