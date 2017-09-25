# Addons

Addon portability is a big part of why I want to make this SRS. I think Lua, Python, and Ruby are great candidates for scripting.

Lua is going to be the most portable and easy to integrate.

My thought is to write an interface for scripting engines.

I think at least Python should be supported. So many SRS solutions use it, and I think it would be a necessary addition to attract the modding community. Since Lua and Python are so desirable, using [lunatic-python](https://labix.org/lunatic-python) is worth consideration.

## Motivation
One thing that is troublesome to me is that a big part of what scares me away from other SRSs is shoehorning the user into modding with a specific language. In particular, Python, which both Anki and Mnemosyne seem to prefer. Python is a great language. I'd even argue that Ruby is a good candidate for writing an SRS. The problem is they're not as portable as C or Lua. One place where all modern SRS solutions have common ground is HTML/Javascript, but they don't seem to leverage those as much as they ought to. I'd like to ensure most things can be done through that avenue. The vast majority of Anki addons are UI related, and often written to aid in changes to the model. Some change the controller.

Ideally, I'd like to facilitate integration of different scripting engines for extending functionality. If someone wants to use Python or Ruby to generate cards or do their scheduling, I think that's great. All that would be required is a way for Kioku to fire off scripts so the REST API can be accessed natively through whatever avenues that language prefers. The problem, of course, comes in for whether the server framework your client targets has that scripting engine installed with the appropriate Kioku binary support. If it's not, any addons you'd synced that rely on it wouldn't work. This is both a security and portability concern unless there's a clear way to sandbox the language.

## Native Support
The idea is to allow support for multiple scripting engines. To do this, the engine wrapper would need to have some way of detecting support for it, so that the user can be notified of unsupported addons on their platform.

Say a user has an addon written in Python that they installed on Desktop, but the mobile version they use doesn't have a native plugin for interpreting Python scripts. Depending on the nature of the plugin, there could be features missing that make their decks behave strangely, but at the very least they need some way to know that it was synced but isn't supported, and how they might be able to download/install it. There would need to be some kind of addon info file to provide useful information.

## Security
Functionality should probably be limited to a standardized subset.

## UI Support
Another big component of this is UI feature portability. There are many standard conventions a UI might follow to provide functionality across platforms. This is an interface the client would need to implement for the sake of JS/HTML-powered addons.
- Menu items
- Hotkeys
- Popups
- Forms
- Custom HTML Frame

In this way HTML/JS clients could do common things in a platform-agnostic way so long as the client is compliant.
