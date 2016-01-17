# Calculating Fractals With Integer Operations

This program demonstrates how to use integers instead of floating point
operations using the example of fractal images.

Using integers in that way is specifically interesting if there is no FPU
available. The background is discussed in my article [»Calculating Fractals
With Integer
Operations«](https://www.cypherpunk.at/2015/10/calculating-fractals-with-integer-operations/).

## Implementation Variants

This package contains several implementations:

* `iterated.c` is a strait forward implementation of the iteration loop using `double`s.
* `iteratel.c` is an implementation of the same algorithm using integers of type `long` instead.
* `iterate.S` is an implementation done in Intel x86_64 assembler. It contains a traditional implementation using stack variables (`#define CONSERVATIVE`) and a high performance implementation.

Read my article [»Fractals And Intel x86_64
Assembler«](https://www.cypherpunk.at/2016/01/fractals-and-int…x86_64-assembler/)
for implementation details of the assembler variant.

This package also contains the original variants in C and 68000 assembler
written on Amiga 500 around 1990.

