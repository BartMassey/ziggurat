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

/* Please see normal.c in this distribution for more information. */

#include "random.h"
#include <math.h>
#include "zigconsts.h"

extern double _rand_polynomial_f[ZIGGURAT_TABLE_SIZE];

double _rand_polynomial (uint32_t r, int n)
{
  while (1)
    {
      /* XXX we recompute some things here to clean up the inline case */
      const int idx = (int)(r & 0xFF);
      const double x = r * _rand_polynomial_w[idx];
      double y, y1;
      if (r < _rand_polynomial_k[idx])
	  return n * x;
      y1 = _rand_polynomial_f[idx];
      if (idx == 0xFF)
	  y = 0;
      else
	  y = _rand_polynomial_f[idx + 1];
      if ((y1 - y) * uniform() + y < pow(1 - x / n, n))
	  return n * x;
      r = rand32();
    }
}
