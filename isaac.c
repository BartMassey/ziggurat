/*
------------------------------------------------------------------------------
rand.c: By Bob Jenkins.  My random number generator, ISAAC.  Public Domain.
MODIFIED:
  960327: Creation (addition of randinit, really)
  970719: use context, not global variables, for internal state
  980324: added main (ifdef'ed out), also rearranged randinit()
  010626: Note that this is public domain
  071004 Bart Massey <bart@cs.pdx.edu>: cleanups for inclusion in libziggurat
------------------------------------------------------------------------------
*/
#include "zrandom.h"

#if (_RAND_SIZ % 8) != 0
#error ISAAC works in blocks of 8, so _RAND_SIZL must be at least 3
#endif

static uint32_t ind(_rand_ctx *ctx, uint32_t x) {
    return ctx->randmem[(x >> 2) & (_RAND_SIZ-1)];
}

static void rngstep(_rand_ctx *ctx, int i, int j, uint32_t mix) { \
   uint32_t x, y;
   x = ctx->randmem[i];
   ctx->randa = (ctx->randa ^ mix) + ctx->randmem[j];
   ctx->randmem[i] = y = ind(ctx, x) + ctx->randa + ctx->randb;
   ctx->randrsl[i] = ctx->randb = ind(ctx, y >> _RAND_SIZL) + x;
}

void _rand_isaac(_rand_ctx *ctx) {
   int i, j;
   ctx->randc++;
   ctx->randb += ctx->randc;
   for (i = 0, j = _RAND_SIZ / 2; i < _RAND_SIZ / 2; )
   {
     rngstep(ctx, i++, j++, ctx->randa << 13);
     rngstep(ctx, i++, j++, ctx->randa >> 6);
     rngstep(ctx, i++, j++, ctx->randa << 2);
     rngstep(ctx, i++, j++, ctx->randa >> 16);
   }
   for (i = 0, j = _RAND_SIZ / 2; i < _RAND_SIZ / 2; )
   {
     rngstep(ctx, j++, i++, ctx->randa << 13);
     rngstep(ctx, j++, i++, ctx->randa >> 6);
     rngstep(ctx, j++, i++, ctx->randa << 2);
     rngstep(ctx, j++, i++, ctx->randa >> 16);
   }
}

static inline void mix(uint32_t tmp[8]) {
   tmp[0] ^= tmp[1] << 11; tmp[3] += tmp[0]; tmp[1] += tmp[2];
   tmp[1] ^= tmp[2] >> 2;  tmp[4] += tmp[1]; tmp[2] += tmp[3];
   tmp[2] ^= tmp[3] << 8;  tmp[5] += tmp[2]; tmp[3] += tmp[4];
   tmp[3] ^= tmp[4] >> 16; tmp[6] += tmp[3]; tmp[4] += tmp[5];
   tmp[4] ^= tmp[5] << 10; tmp[7] += tmp[4]; tmp[5] += tmp[6];
   tmp[5] ^= tmp[6] >> 4;  tmp[0] += tmp[5]; tmp[6] += tmp[7];
   tmp[6] ^= tmp[7] << 8;  tmp[1] += tmp[6]; tmp[7] += tmp[0];
   tmp[7] ^= tmp[0] >> 9;  tmp[2] += tmp[7]; tmp[0] += tmp[1];
}

/* if (flag==TRUE), then use the contents of randrsl[] to initialize mm[]. */
void _rand_isaac_init(_rand_ctx *ctx, int flag) {
   int i, j;
   uint32_t tmp[8];

   ctx->randa = ctx->randb = ctx->randc = 0;
   tmp[0] = tmp[1] = tmp[2] = tmp[3] = tmp[4] = tmp[5] = tmp[6] = tmp[7] = 0x9e3779b9;  /* the golden ratio */

   for (i = 0; i < 4; i++)          /* scramble it */
     mix(tmp);

   if (flag)
   {
     /* initialize using the contents of r[] as the seed */
     for (i = 0; i < _RAND_SIZ; i += 8)
     {
       for (j = 0; j < 8; ++j)
         tmp[j] += ctx->randrsl[i + j];
       mix(tmp);
       for (j = 0; j < 8; ++j)
         ctx->randmem[i + j] = tmp[j];
     }
     /* do a second pass to make all of the seed affect all of m */
     for (i = 0; i < _RAND_SIZ; i += 8)
     {
       for (j = 0; j < 8; ++j)
         tmp[j] += ctx->randmem[i + j];
       mix(tmp);
       for (j = 0; j < 8; ++j)
         ctx->randmem[i + j] = tmp[j];
     }
   }
   else
   {
     /* fill in m[] with messy stuff */
     for (i = 0; i < _RAND_SIZ; i += 8)
     {
       mix(tmp);
       for (j = 0; j < 8; ++j)
         ctx->randmem[i + j] = tmp[j];
     }
   }

   _rand_isaac(ctx);          /* fill in the first set of results */
   ctx->randcnt = _RAND_SIZ;  /* prepare to use the first set of results */
}


#ifdef TEST_DRIVER
#include <stdio.h>

int main()
{
  uint32_t i, j;
  _rand_ctx ctx;
  ctx.randa = ctx.randb = ctx.randc = 0;
  if (_RAND_SIZL != 8)
    fprintf(stderr, "warning: unexpected _RAND_SIZL %d\n", _RAND_SIZL);
  for (i = 0; i < _RAND_SIZ; i++)
    ctx.randrsl[i] = 0;
  _rand_isaac_init(&ctx, 1);
  for (i=0; i<2; ++i)
  {
    _rand_isaac(&ctx);
    for (j=0; j<_RAND_SIZ; ++j)
    {
      printf("%.8lx",ctx.randrsl[j]);
      if ((j&7)==7) printf("\n");
    }
  }
}
#endif
