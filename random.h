#ifndef _RANDOM_H
#define _RANDOM_H

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

extern float _rand_uniform_w[256];
extern uint32_t _rand_uniform_k[256];

extern float _rand_uniform(uint32_t r);

static inline float uniform(void) {
      /* 32-bit mantissa */
      const uint32_t r = rand32();
      const uint32_t rabs = r&0x7fffffffUL;
      const int idx = (int)(r&0xFF);
      const float x = ((int32_t)r) * _rand_uniform_w[idx];
      if (rabs < _rand_uniform_k[idx])
	  return x;   /* 99.3% of the time we return here 1st try */
      return _rand_uniform(r);
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
