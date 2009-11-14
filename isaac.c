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

inline static uint32_t ind(uint32_t *mm, uint32_t x) {
    return *(uint32_t *)((uint8_t *)mm + ((x) & ((_RAND_SIZ-1)<<2)));
}

/* XXX requires calling in very specific context below.
   x and y are locals, others are notionally parameters. */
#define RNGSTEP(mix) \
  x = *m; \
  a = (a ^ (mix)) + *m2++; \
  *m++ = y = ind(mm, x) + a + b; \
  *r++ = b = ind(mm, y >> _RAND_SIZL) + x

void _rand_isaac(_rand_ctx *ctx) {
   uint32_t a, b, x, y;
   uint32_t *m, *mm, *m2, *r, *mend;
   mm = ctx->randmem; r = ctx->randrsl;
   a = ctx->randa; b = ctx->randb + (++(ctx->randc));
   for (m = mm, mend = m2 = m + _RAND_SIZ / 2; m < mend; )
   {
     RNGSTEP(a << 13);
     RNGSTEP(a >> 6);
     RNGSTEP(a << 2);
     RNGSTEP(a >> 16);
   }
   for (m2 = mm; m2 < mend; )
   {
     RNGSTEP(a << 13);
     RNGSTEP(a >> 6);
     RNGSTEP(a << 2);
     RNGSTEP(a >> 16);
   }
   ctx->randb = b; ctx->randa = a;
}


/* XXX requires calling in very specific context below. */
#define MIX do { \
   a ^= b << 11; d += a; b += c; \
   b ^= c >> 2;  e += b; c += d; \
   c ^= d << 8;  f += c; d += e; \
   d ^= e >> 16; g += d; e += f; \
   e ^= f << 10; h += e; f += g; \
   f ^= g >> 4;  a += f; g += h; \
   g ^= h << 8;  b += g; h += a; \
   h ^= a >> 9;  c += h; a += b; } while(0)

/* if (flag==TRUE), then use the contents of randrsl[] to initialize mm[]. */
void _rand_isaac_init(_rand_ctx *ctx, int flag) {
   int i;
   uint32_t a, b, c, d, e, f, g, h;
   uint32_t *m, *r;
   ctx->randa = ctx->randb = ctx->randc = 0;
   m = ctx->randmem;
   r = ctx->randrsl;
   a = b = c = d = e = f = g = h = 0x9e3779b9;  /* the golden ratio */

   for (i = 0; i < 4; i++)          /* scramble it */
     MIX;

   if (flag)
   {
     /* initialize using the contents of r[] as the seed */
     for (i = 0; i < _RAND_SIZ; i += 8)
     {
       a+=r[i    ]; b += r[i + 1]; c += r[i + 2]; d += r[i + 3];
       e+=r[i + 4]; f += r[i + 5]; g += r[i + 6]; h += r[i + 7];
       MIX;
       m[i    ] = a; m[i + 1] = b; m[i + 2] = c; m[i + 3] = d;
       m[i + 4] = e; m[i + 5] = f; m[i + 6] = g; m[i + 7] = h;
     }
     /* do a second pass to make all of the seed affect all of m */
     for (i = 0; i < _RAND_SIZ; i += 8)
     {
       a+=m[i    ]; b += m[i + 1]; c += m[i + 2]; d += m[i + 3];
       e+=m[i + 4]; f += m[i + 5]; g += m[i + 6]; h += m[i + 7];
       MIX;
       m[i    ] = a; m[i + 1] = b; m[i + 2] = c; m[i + 3] = d;
       m[i + 4] = e; m[i + 5] = f; m[i + 6] = g; m[i + 7] = h;
     }
   }
   else
   {
     /* fill in m[] with messy stuff */
     for (i = 0; i < _RAND_SIZ; i += 8)
     {
       MIX;
       m[i    ] = a; m[i + 1] = b; m[i + 2] = c; m[i + 3] = d;
       m[i + 4] = e; m[i + 5] = f; m[i + 6] = g; m[i + 7] = h;
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
