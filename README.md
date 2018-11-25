# Ziggurat: Fast Normal-, Exponential-, and Polynomial-Distributed Pseudo-Random Numbers
Copyright (c) 2007 Bart Massey

In producing my [[BMPF]] implementation, I did some of David
Bateman's BSD-licensed re-implementation of Marsaglia and
Tsang's
[Ziggurat Method](http://www.jstatsoft.org/v05/i08/ziggurat.pdf)
of Gaussian (and exponential) pseudo-random number
generation.  This eventually grew into some fairly custom
stuff.  I replaced the various PRNG's lying around with the
high-quality high-performance <a
href="http://www.burtleburtle.net/bob/rand/isaacafa.html">ISAAC</a>
PRNG.

The Ziggurat method is a vast performance improvement over
the better-known and simpler Box-M&uuml;ller method in
producing normal variates.  The basic cost of producing a
variate is essentially two table lookups, a floating-point
multiply, a floating-point compare, and some amortized
stuff; I can generate more than 15M Gaussian variates per
second on my Intel Core II Duo box.  The tables are
small, about 6KB, and thus fit easily into L0 cache or onto
a microcontroller&mdash;the code could be adapted for
fixed-point with some effort.

My "improvements" include pre-compiled tables, a library
wrapper to make it easier to link against, a header
containing GCC inline functions for all the
performance-critical bits, a fairly generic API, robust
"make check" test code, `random()`/`srandom()` emulation,
etc.

## Build and Install

Should be able to just use `make install` (with appropriate
permissions). This does not currently create a shared
library, just a static one.

## History

This codebase originally came from David Bateman's
BSD-licensed Ziggurat implementation as taken from GNU
Octave.  I've optimized this code by ripping the Mersenne
Twister back out and replacing it with Bob Jenkins's faster
and better ISAAC generator.  I've also split out the
relevant stuff into inline functions in zrandom.h, and fixed
up the interface.  I also ripped out the 64-bit code for
clarity and portability.  The result should be reasonably
fast for portable code and legal for me to distribute.

I originally included a Ziggurat generator for the
distribution *(1-x)<sup>n</sup>* used in the BMPF resampler.
Unfortunately, this generator had a subtle mathematical bug,
and has been replaced by a correct but slower implementation
using `uniform()` and `pow()`. I can't see an easy way to
fix it.  Better to have a slow implementation than a bad
one, I think.

## License

This code is derived from David Bateman's BSD-licensed
Ziggurat implementation.  The bulk of this code is under the
"3-clause BSD License". Please see the file LICENSE in this
distribution for license terms. The file `isaac.c` was
placed "in the Public Domain" by Bob Jenkins.
