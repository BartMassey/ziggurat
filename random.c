/*
   Copyright (C) 2007 Bart Massey

   Please see the file LICENSE in this distribution for license terms.
*/
#include "zrandom.h"

_rand_ctx _rand_ctx_default;

uint32_t _rand_last = 0x63636363;

int _inited = 0;

void zsrandom(unsigned seed) {
    int i;
    for (i = 0; i < _RAND_SIZ; i++)
	_rand_ctx_default.randrsl[i] = seed;
    _rand_isaac_init(&_rand_ctx_default, 1);
    _inited = 1;
}
