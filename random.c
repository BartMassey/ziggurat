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

/*
This codebase originally came from David Bateman's code in
GNU Octave.  I've optimized this code by ripping the
Mersenne Twister back out and replacing it with the faster
but not so good SHR3 generator Marsaglia and Tsang used
originally, and changing doubles to floats.  I've also split
out the relevant stuff into inline functions in random.h,
and fixed up the interface.  I also ripped out the 64 bit
code for clarity and portability.  The result should be
reasonably fast for portable code, yet clearly legal for me
to distribute...  --Bart Massey
*/

#include "random.h"
#include <math.h>

unsigned long _rand32_state = 0x23456789UL;

/*
This code is based on the paper Marsaglia and Tsang, "The ziggurat method
for generating random variables", Journ. Statistical Software. Code was
presented in this paper for a Ziggurat of 127 levels and using a 32 bit
integer random number generator. This version of the code, uses the 
Mersenne Twister as the integer generator and uses 256 levels in the 
Ziggurat. This has several advantages.

  1) As Marsaglia and Tsang themselves states, the more levels the few 
     times the expensive tail algorithm must be called
  2) The cycle time of the generator is determined by the integer 
     generator, thus the use of a Mersenne Twister for the core random 
     generator makes this cycle extremely long.
  3) The license on the original code was unclear, thus rewriting the code 
     from the article means we are free of copyright issues.
  4) Compile flag for full 53-bit random mantissa.

It should be stated that the authors made my life easier, by the fact that
the algorithm developed in the text of the article is for a 256 level
ziggurat, even if the code itself isn't...

One modification to the algorithm developed in the article, is that it is
assumed that 0 <= x < Inf, and "unsigned long"s are used, thus resulting in
terms like 2^32 in the code. As the normal distribution is defined between
-Inf < x < Inf, we effectively only have 31 bit integers plus a sign. Thus
in Marsaglia and Tsang, terms like 2^32 become 2^31. We use NMANTISSA for
this term.  The exponential distribution is one sided so we use the 
full 32 bits.  We use EMANTISSA for this term.

It appears that I'm slightly slower than the code in the article, this
is partially due to a better generator of random integers than they
use. But might also be that the case of rapid return was optimized by
inlining the relevant code with a #define. As the basic Mersenne
Twister is only 25% faster than this code I suspect that the main
reason is just the use of the Mersenne Twister and not the inlining,
so I'm not going to try and optimize further.  --David Bateman
*/

#include "zigconsts.h"

extern float _rand_normal_f[ZIGGURAT_TABLE_SIZE];
extern float _rand_exponential_f[ZIGGURAT_TABLE_SIZE];

/*
 * Here is the guts of the algorithm. As Marsaglia and Tsang state the
 * algorithm in their paper
 *
 * 1) Calculate a random signed integer j and let i be the index
 *     provided by the rightmost 8-bits of j
 * 2) Set x = j * w_i. If j < k_i return x
 * 3) If i = 0, then return x from the tail
 * 4) If [f(x_{i-1}) - f(x_i)] * U < f(x) - f(x_i), return x
 * 5) goto step 1
 *
 * Where f is the functional form of the distribution, which for a normal
 * distribution is exp(-0.5*x*x)
 */

float _rand_normal(uint32_t r)
{
  while (1)
    {
      /* XXX we recompute some things here to clean up the inline case */
      const uint32_t rabs = r & 0x7fffffffUL;
      const int idx = r & 0xFF;
      const float x = ((int32_t)r) * _rand_normal_w[idx];
      if (rabs < _rand_normal_k[idx])
	  return x;   /* 99.3% of the time we return here 1st try */
      if (idx == 0)
	{
	  /* As stated in Marsaglia and Tsang
	   * 
	   * For the normal tail, the method of Marsaglia[5] provides:
	   * generate x = -ln(U_1)/r, y = -ln(U_2), until y+y > x*x,
	   * then return r+x. Except that r+x is always in the positive 
	   * tail!!!! Any thing random might be used to determine the
	   * sign, but as we already have r we might as well use it
	   *
	   * [PAK] but not the bottom 8 bits, since they are all 0 here!
	   */
	  float xx, yy;
	  do
	    {
	      xx = - ZIGGURAT_NOR_INV_R * log (rand32());
	      yy = - log (rand32());
	    } 
	  while ( yy+yy <= xx*xx);
	  return (rabs&0x100 ? -ZIGGURAT_NOR_R-xx : ZIGGURAT_NOR_R+xx);
	}
      else if ((_rand_normal_f[idx-1] - _rand_normal_f[idx]) * rand32() +
	       _rand_normal_f[idx] < exp(-0.5*x*x))
	return x;
      r = rand32();
    }
}

float _rand_exponential (uint32_t r)
{
  while (1)
    {
      /* XXX we recompute some things here to clean up the inline case */
      const int idx = (int)(r & 0xFF);
      const float x = r * _rand_exponential_w[idx];
      if (r < _rand_exponential_k[idx])
	return x;
      else if (idx == 0)
	{
	  /* As stated in Marsaglia and Tsang
	   * 
	   * For the exponential tail, the method of Marsaglia[5] provides:
           * x = r - ln(U);
	   */
	  return ZIGGURAT_EXP_R - log(rand32());
	}
      else if ((_rand_exponential_f[idx-1] -
		_rand_exponential_f[idx]) * rand32() +
	       _rand_exponential_f[idx] < exp(-x))
	return x;
      r = rand32();
    }
}
