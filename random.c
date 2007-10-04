#include "random.h"

_rand_ctx _rand_ctx_default;

long random(void) { return rand32(); }

void srandom(unsigned seed) {
    int i;
    for (i = 0; i < _RAND_SIZ; i++)
	_rand_ctx_default.randrsl[i] = seed;
    _rand_isaac_init(&_rand_ctx_default, 1);
}
