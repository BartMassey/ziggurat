#ifndef _ISAAC_H
#define _ISAAC_H
/*
------------------------------------------------------------------------------
isaac.h: definitions for a random number generator
By Bob Jenkins, 1996, Public Domain
MODIFIED:
  960327: Creation (addition of randinit, really)
  970719: use context, not global variables, for internal state
  980324: renamed seed to flag
  980605: recommend RANDSIZL=4 for noncryptography.
  010626: note this is public domain
  070927 Bart Massey <bart@cs.pdx.edu>: cleanups for inclusion in libziggurat
------------------------------------------------------------------------------
*/

#include <stdint.h>

#define _RANDSIZL   8   /* I recommend 8 for crypto, 4 for simulations */
#define _RANDSIZ    (1<<_RANDSIZL)

/* context of random number generator */
struct randctx
{
  uint32_t randcnt;
  uint32_t randrsl[_RANDSIZ];
  uint32_t randmem[_RANDSIZ];
  uint32_t randa;
  uint32_t randb;
  uint32_t randc;
};
typedef  struct randctx  randctx;

/* If (flag==TRUE), then use the contents of
   randrsl[0.._RANDSIZ-1] as the seed. */
extern void isaac_init(randctx *r, int flag);
extern void isaac(randctx *r);

extern randctx _default_randctx;

/* Call rand32r(r) to retrieve a single 32-bit random value
   from context r. */
static inline uint32_t rand32r(randctx *r) {
    if (r->randcnt == 0) {
	isaac(r);
	r->randcnt = _RANDSIZ;
    }
    return r->randrsl[--r->randcnt];
}

/* Call rand32() to retrieve a single 32-bit random value
   from the "default context". */
static inline uint32_t rand32(void) {
    return rand32r(&_default_randctx);
}

#endif  /* _ISAAC_H */
