#ifndef _RANDOM_H
#define _RANDOM_H
/*
   Copyright (C) 2004, David Bateman
   with modifications
   Copyright (C) 2007 Bart Massey

   Please see the file LICENSE in this distribution for license terms.
*/

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

/* ISAAC definitions */

#define _RAND_SIZL   8   /* I recommend 8 for crypto, 4 for simulations */
#define _RAND_SIZ    (1<<_RAND_SIZL)

/* context of random number generator */
struct _rand_ctx
{
  uint32_t randcnt;
  uint32_t randrsl[_RAND_SIZ];
  uint32_t randmem[_RAND_SIZ];
  uint32_t randa;
  uint32_t randb;
  uint32_t randc;
};
typedef  struct _rand_ctx  _rand_ctx;

/* If (flag==TRUE), then use the contents of
   randrsl[0.._RAND_SIZ-1] as the seed. */
extern void _rand_isaac_init(_rand_ctx *r, int flag);
extern void _rand_isaac(_rand_ctx *r);

extern _rand_ctx _rand_ctx_default;

extern int _inited;

extern void zsrandom(unsigned seed);

/* Call rand32r(r) to retrieve a single 32-bit random value
   from context r. */
static inline uint32_t rand32r(_rand_ctx *r) {
    if (r->randcnt == 0) {
	if (!_inited)
	    zsrandom(17);
	_rand_isaac(r);
    }
    return r->randrsl[--r->randcnt];
}

/* Call rand32() to retrieve a single 32-bit random value
   from the "default context". */
static inline uint32_t rand32(void) {
    return rand32r(&_rand_ctx_default);
}

static inline long zrandom(void) {
    return (long) (rand32() & 0x7fffffff);
}

extern double _rand_normal_w[256];
extern uint32_t _rand_normal_k[256];

extern double _rand_normal(uint32_t r, int idx);

static inline double uniform(void) {
    const double scale = 5.42101086242752e-20;
    return (4294967296.0 * rand32() + rand32()) * scale;
}

extern uint32_t _rand_last;

static inline double normal(void) {
    /* 32-bit mantissa */
    const uint32_t r = rand32();
    const uint32_t rabs = r & 0x7fffffffUL;
    const int idx = (r ^ _rand_last) & 0xFF;
    _rand_last = r;
    /* 99.3% of the time we return here 1st try */
    if (rabs < _rand_normal_k[idx])
	return (int32_t)r * _rand_normal_w[idx];
    return _rand_normal(r, idx);
}

static inline double gaussian(double sigma) {
      return normal() * sigma;
}

extern double _rand_exponential_w[256];
extern uint32_t _rand_exponential_k[256];

extern double _rand_exponential(uint32_t r, int idx);

static inline double exponential(void) {
    uint32_t r = rand32();
    int idx = (r ^ _rand_last) & 0xFF;
    _rand_last = r;
    /* 98.9% of the time we return here 1st try */
    if (r < _rand_exponential_k[idx])
	return r * _rand_exponential_w[idx];
    return _rand_exponential(r, idx);
}

/* Return a variate with distribution (1 - x)**n */
static inline double polynomial(int n) {
    return 1.0 - pow(uniform(), 1.0 / (n + 1.0));
}

#endif
