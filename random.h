#ifndef _RANDOM_H
#define _RANDOM_H
/*
   Copyright (C) 2004, David Bateman
   All rights reserved.
   with modifications
   Copyright (C) 2007 Bart Massey
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER 
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

/* Call rand32r(r) to retrieve a single 32-bit random value
   from context r. */
static inline uint32_t rand32r(_rand_ctx *r) {
    if (r->randcnt == 0) {
	_rand_isaac(r);
	r->randcnt = _RAND_SIZ;
    }
    return r->randrsl[--r->randcnt];
}

/* Call rand32() to retrieve a single 32-bit random value
   from the "default context". */
static inline uint32_t rand32(void) {
    return rand32r(&_rand_ctx_default);
}

extern long random(void);
extern void srandom(unsigned seed);

extern double _rand_normal_w[256];
extern uint32_t _rand_normal_k[256];

extern double _rand_normal(uint32_t r);

static inline double uniform(void) {
      return rand32() * (1.0 / 4294967294.9);
}

static inline double normal(void) {
      /* 32-bit mantissa */
      const uint32_t r = rand32();
      const uint32_t rabs = r & 0x7fffffffUL;
      const int idx = r & 0xFF;
      /* 99.3% of the time we return here 1st try */
      if (rabs < _rand_normal_k[idx])
	  return (int32_t)r * _rand_normal_w[idx];
      return _rand_normal(r);
}

static inline double gaussian(double sigma) {
      return normal() * sigma;
}

extern double _rand_exponential_w[256];
extern uint32_t _rand_exponential_k[256];

extern double _rand_exponential(uint32_t r);

static inline double exponential(void) {
    uint32_t r = rand32();
    const int idx = (int)(r & 0xFF);
    /* 98.9% of the time we return here 1st try */
    if (r < _rand_exponential_k[idx])
	return r * _rand_exponential_w[idx];
    return _rand_exponential(r);
}

extern double _rand_polynomial_w[256];
extern uint32_t _rand_polynomial_k[256];

extern double _rand_polynomial(uint32_t r, int n);

/* Return a variate with distribution (1 - x)**n */
static inline double polynomial(int n) {
    const int pn = 50;   /* XXX must match PN in zigconsts.h */
    uint32_t r;
    if (n < pn)
	return 1.0 - pow(uniform(), 1.0 / (n + 1));
    r = rand32();
    int idx = (int)(r & 0xFF);
    /* About 95% of the time we return here 1st try. */
    if (r < _rand_polynomial_k[idx])
	return (double)pn * (double)r * _rand_polynomial_w[idx] / n;
    return _rand_polynomial(r, n);
}

#endif
