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

extern unsigned long _rand32_state;

/* SHR3 generator */
static inline uint32_t rand32(void) {
    unsigned long state0 = _rand32_state;
    _rand32_state ^= _rand32_state << 13;
    _rand32_state ^= _rand32_state >> 17;
    _rand32_state ^= _rand32_state << 5;
    return state0 + _rand32_state;
}

inline long random(void) {
    assert(RAND_MAX==0x7fffffff);
    return rand32() & RAND_MAX;
}

inline void srandom(unsigned seed) {
    _rand32_state = seed;
}

extern float _rand_normal_w[256];
extern uint32_t _rand_normal_k[256];

extern float _rand_normal(uint32_t r);

static inline float uniform(void) {
      return rand32() / 4294967296.0f;
}

static inline float normal(void) {
      /* 32-bit mantissa */
      const uint32_t r = rand32();
      const uint32_t rabs = r&0x7fffffffUL;
      const int idx = (int)(r&0xFF);
      const float x = ((int32_t)r) * _rand_normal_w[idx];
      if (rabs < _rand_normal_k[idx])
	  return x;   /* 99.3% of the time we return here 1st try */
      return _rand_normal(r);
}

static inline float gaussian(float sigma) {
      return normal() * sigma;
}

extern float _rand_exponential_w[256];
extern uint32_t _rand_exponential_k[256];

extern float _rand_exponential(uint32_t r);

static inline float exponential(void) {
    uint32_t r = rand32();
    const int idx = (int)(r & 0xFF);
    const float x = r * _rand_exponential_w[idx];
    if (r < _rand_exponential_k[idx])
	return x;   /* 98.9% of the time we return here 1st try */
    return _rand_exponential(r);
}

#endif
