# Principles

This document outlines some guiding principles for development.

## Goto

This codebase makes use of the `goto` statement. This is to promote code clarity and robustness. This may seem counterintuitive to the uninitiated, who have been taught that `goto` is evil. In reality, `goto` is something that good programmers use for good reasons. Philosophically, bad programmers produce bad code regardless of what language features are made available to them. Good programmers produce good code by knowing how to apply the language features available to them, and `goto` is one such feature. Being afraid of it does not mean you're a bad programmer - it simply means you've been indoctrinated by a swath of outdated opinions, likely purveyed by college professors, based on research papers that are no longer applicable to real-world use. Many arguments against it are dependent on contrived examples/anti-patterns that no competent programmer would ever use.

To put this into perspective, both the Linux kernel and libgit2 (upon which this project depends) use `goto` quite liberally. Linus Torvalds even promotes its use. An excellent discussion can be found [here](https://web.archive.org/web/20051128093253/http://kerneltrap.org/node/553/2131), which explains the rationale and positive aspects of `goto` in the kernel much better than I could. There's still contention and disagreement on this, but I've come around to the opinion that it is a wise choice.

Many people go to great lengths to avoid it's use while preserving its utility with convoluted and sometimes wasteful workarounds that ironically *reduce* code clarity and open themselves up to more future maintenance.

Since not everyone is as experienced in the safe use of `goto`, including myself, there are rules for applying it in this codebase.

1. Labels MUST only be declared within top function scope (i.e. outside of control structures such as `for` and `if`, but within the confines of a function).
2. Jumps to labels MUST only go forward in execution - never backward.
3. All symbols referenced *past* the label MUST be declared and initialized *before* the first possible jump to it.

These are some very tight restrictions on its use that help keep the codebase clean and readable. If you follow these rules, you will rarely end up in a situation where `goto` hurts more than helps. As always, clarity is key.

There are however things to watch out for.

### Multi-label functions
Some functions have a pattern of two labels to jump to: One for reverting to a previous state upon failure, and one for things like cleanup/output.

I've made the mistake of not jumping past my revert on success, which effecitively caused a success result with a failure state. Take the following code.
```c
char *DoThing()
{
  char *something = malloc(1000);
  if (something == NULL)
  {
    goto done;
  }
  if (getcwd(something, 1000) != something)
  {
    goto cleanup;
  }
cleanup:
  free(something);
done:
  return something;
}
```
In the above contrived example which is quite similar to my original mistake from a much more complex function, I've neglected to put `goto done;` before `cleanup:`. This means that when `getcwd` succeeds, still end up freeing `something` and returning the pointer. This is very dangerous.

It was my first time applying this strategy, and thankfully unit tests caught my cognitive dissonance. This begs the question as to whether `goto` was the right tool for this. The following might have been clearer for this particular case.
```c
char *DoThing()
{
  char *something = malloc(1000);
  if (something != NULL)
  {
    if (getcwd(something, 1000) != something)
    {
      free(something);
      something = NULL;
    }
  }
  return something;
}
```
As stated before, my real use-case (fixed the same commit where I've added this warning) was far more complex. It may indicate that the function itself is too complex, but at the time of writing I have not judged this to be the case.

One suggestion you could take away from this, which I haven't stated as a rule yet, is to ensure every execution path in such functions ends with a `goto`. This would have disallowed any "fall-through" behaviour and made the bug much more obvious when it was being introduced. For now, just be sure to check the line *above* every label so you know whether it really makes sense. What helped me was to add a debug message at the top of my cleanup label. Another thing that would have helped would be to assert whether my result made sense at the top of my cleanup label.

## User Input

## Support Functions

## Internal Dependency

## Exception Handling

### Failure Classes
There are two kinds of failure class. Critical and non-critical.
1. Critical Failure. This is unrecoverable. It is either the fault of the programmer or due to a force imposed by the runtime environment (such as ENOMEM). Either way, the program ought to bail.
1. Non-Critical Failure. This is recoverable, and often preventable. It is up to the user how to handle this, but the API must clearly specify what manner to detect with.

### Bailing
Bailing is something that is often meant to result in program termination because recovery is infeasible. Bailing must always trigger a helpful log message and output to stderr before bailing.

Bailing should not necessarily abort. Since the API is meant to be used by a server, it's important to give the server a chance to clean up it's resources so relaunching doesn't have resource acquisition problems. There are two options:
1. Give the user a jump point.
1. Give the user a callback setter.

A jump point is preferable as it's more flexible and does not add a new stack frame (as far as I'm aware).

### Handling/Preventing Failure
Functions that can fail in non-critical ways should strive to suggest only one way of recovering from a failure. They must provide some mechanism to help the user prevent failure. This encourages the user to lean toward preventative strategies.

All API functions must provide or have a means for the user to be intelligent about their input quality.

#### Example: Buffer Size

One common scenario is the user not providing enough memory to a buffer that they pass to the API to be filled. They have 3 choices:
1. Respond by retrying with more memory until it succeeds.
1. Use the API to determine how much they need so they can provide perfect input.
1. Provide input that is as perfect as possible to begin with. The library must provided a consistent way of informing what "perfect as possible" is. In this case the unlikely failure takes on a critical nature. This is where a user may choose to bail.

The later two, being prevention-based strategies, are by far preferable. For large values (we'll say greater than a kibibyte) the second option is preferred. Otherwise the third is best. One or both may be provided. An example would be path concatenation. There is a supposed max path size that can be used, but frankly, that can be incorrect and/or dangerous to use (in the case of an unexpectedly large stack allocation).

In general, the top level user will only care about binary outcomes (success/failure). As such, complex failure scenarios (such as filesystem operations) have two options.
1. Provide a mechanism for inspecting failure in greater detail.
1. Log interesting details and move on.

Which is chosen does not much matter.

### Logging
