/*
   Copyright (C) 2004, David Bateman
   with modifications
   Copyright (C) 2007 Bart Massey

   Please see the file LICENSE in this distribution for license terms.
*/

#include "zrandom.h"
#include <math.h>
#include "zigconsts.h"

extern double _rand_exponential_f[ZIGGURAT_TABLE_SIZE];

double _rand_exponential (uint32_t r, int idx)
{
  while (1)
    {
      /* XXX we recompute some things here to clean up the inline case */
      const double x = r * _rand_exponential_w[idx];
      if (r < _rand_exponential_k[idx])
	return x;
      else if (idx == 0)
	{
	  /* As stated in Marsaglia and Tsang
	   *
	   * For the exponential tail, the method of Marsaglia[5] provides:
           * x = r - ln(U);
	   */
	  return ZIGGURAT_EXP_R - log(uniform());
	}
      else if ((_rand_exponential_f[idx-1] -
		_rand_exponential_f[idx]) * uniform() +
	       _rand_exponential_f[idx] < exp(-x))
	return x;
      r = rand32();
      idx = (r ^ _rand_last) & 0xFF;
      _rand_last = r;
    }
}
