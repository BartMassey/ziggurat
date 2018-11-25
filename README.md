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

The Ziggurat-based polynomial() generator has been removed
from this library, and replaced with a method that uses a
call to uniform() and a call to pow().  The Ziggurat was
subtly borked by bad math, and I can't see an easy way to
fix it.  Better to have a slow implementation than a bad
one, I think.

## License

This code is derived from David Bateman's BSD-licensed
Ziggurat implementation.  The bulk of this code is under the
"3-clause BSD License". Please see the file LICENSE in this
distribution for license terms. The file `isaac.c` was
placed "in the Public Domain" by Bob Jenkins.
