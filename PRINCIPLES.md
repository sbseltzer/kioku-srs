# Principles

This document outlines some guiding principles for development.

## User Input

## Support Functions

## Internal Dependency

## Exception Handling

### Failure Classes
There are two kinds of failure class. Critical and non-critical.
1. Critical Failure. This is unrecoverable. It is either the fault of the programmer or due to a force imposed by the runtime environment (such as ENOMEM). Either way, the program ought to bail.
1. Non-Critical Failure. This is recoverable, and often preventable. It is up to the user how to handle this, but the API must clearly specify what manner to detect with.

### Bailing
Bailing must always trigger a helpful log message and output to stderr before bailing.

Bailing should not necessarily abort. Since the API is meant to be used by a server, it's important to give the server a chance to clean up it's resources so relaunching doesn't have resource acquisition problems. There are two options:
1. Give the user a jump point.
1. Give the user a callback setter.

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
