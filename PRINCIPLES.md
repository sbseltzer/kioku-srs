# Principles

This document outlines some guiding principles for development.

## Goto

This codebase makes use of the `goto` statement. This is to promote code clarity and robustness. This may seem counterintuitive to the uninitiated, who have been taught that `goto` is evil. In reality, `goto` is something that good programmers use for good reasons. Philosophically, bad programmers produce bad code regardless of what language features are made available to them. Good programmers produce good code by knowing how to apply the language features available to them, and `goto` is one such feature. Being afraid of it does not mean you're a bad programmer - it simply means you've been indoctrinated by a swath of outdated opinions, likely purveyed by college professors, based on research papers that are no longer applicable to real-world use. Many arguments against it are dependent on contrived examples/anti-patterns that no competent programmer would ever use.

To put this into perspective, both the Linux kernel and libgit2 (upon which this project depends) use `goto` quite liberally. Linus Torvalds even promotes its use. An excellent discussion can be found [here](https://web.archive.org/web/20051128093253/http://kerneltrap.org/node/553/2131), which explains the rationale and positive aspects of `goto` in the kernel much better than I could. There's still contention and disagreement on this, but I've come around to the opinion that it is a wise choice.

Many people go to great lengths to avoid it's use while preserving its utility with convoluted and sometimes wasteful workarounds that ironically *reduce* code clarity and open themselves up to more future maintenance.

Since not everyone is as experienced in the safe use of `goto`, including myself, there are three rules for applying it in this codebase.

1. Labels MUST only be declared within top function scope (i.e. outside of control structures such as `for` and `if`, but within the confines of a function).
2. Jumps to labels MUST only go forward in execution - never backward.
3. All symbols referenced *past* the label MUST be declared and initialized *before* the first possible jump to it.

These are some very tight restrictions on its use that help keep the codebase clean and readable. If you follow these rules, you will rarely end up in a situation where `goto` hurts more than helps. As always, clarity is key.

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

All API functions must provide a means for the user to be intelligent about their input quality. This can be achieved by providing a mechanism for determining their specific requirements, or providing a static mechanism that gives a near-guarantee for perfect input.

#### Example: Buffer Size

One common scenario is the user not providing enough memory to a buffer that they pass to the API to be filled. They have 3 choices:
1. Respond by retrying with more memory until it succeeds.
1. Use the API to determine how much they need so they can provide perfect input.
1. Provide input that is as perfect as possible to begin with. The library must provided a static way of informing what "perfect as possible" is. In this case the unlikely failure takes on a critical nature. This is where a user may choose to bail.

The later two, being prevention-based strategies, are by far preferable. For large values (we'll say greater than a kibibyte) the second option is preferred. Otherwise the third is best. One or both may be provided. An example would be path concatenation. There is a definitive max path size that could be used, but frankly, that could be much longer than is necessary (or safe in the likely case of stack allocation).

In general, the top level user will only care about binary outcomes (success/failure). As such, complex failure scenarios (such as filesystem operations) have two options.
1. Provide a mechanism for inspecting failure in greater detail.
1. Log interesting details and move on.

Which is chosen does not much matter.

### Logging
If failures are generalized to 
