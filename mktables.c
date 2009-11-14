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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "zigconsts.h"

# define NMANTISSA 2147483648.0 /* 31 bit mantissa */
# define EMANTISSA 4294967296.0 /* 32 bit mantissa */
# define PMANTISSA 4294967296.0 /* 32 bit mantissa */

static double fi[256], fe[256], fp[256];
static double wi[256], we[256], wp[256];
static uint32_t ki[256], ke[256];
static double kp[256];

static double polynomial_advance(double x0) {
    double dx = 0.5;
    double x = x0;
    while(1) {
	double x1 = x + dx;
	double a = x1 * (exp(-PN * x0) - exp(-PN * x1));
	if (x1 == x)
	    return x;
	if (a <= ZIGGURAT_POL_SECTION_AREA)
	    x = x1;
	dx *= 0.5;
    }
    abort();
}

static void 
create_ziggurat_tables (void)
{
  int i;
  double x, x1;
 
  /* Ziggurat tables for the normal distribution */
  x1 = ZIGGURAT_NOR_R;
  wi[255] = x1 / NMANTISSA;
  fi[255] = exp (-0.5 * x1 * x1);

  /* Index zero is special for tail strip, where Marsaglia and Tsang 
   * defines this as 
   * k_0 = 2^31 * r * f(r) / v, w_0 = 0.5^31 * v / f(r), f_0 = 1,
   * where v is the area of each strip of the ziggurat. 
   */
  ki[0] = (uint32_t)(floor(x1 * fi[255] / NOR_SECTION_AREA * NMANTISSA));
  wi[0] = NOR_SECTION_AREA / fi[255] / NMANTISSA;
  fi[0] = 1.;

  for (i = 254; i > 0; i--)
    {
      /* New x is given by x = f^{-1}(v/x_{i+1} + f(x_{i+1})), thus
       * need inverse operator of y = exp(-0.5*x*x) -> x = sqrt(-2*ln(y))
       */
      x = sqrt(-2. * log(NOR_SECTION_AREA / x1 + fi[i+1]));
      ki[i+1] = floor(x / x1 * NMANTISSA);
      wi[i] = x / NMANTISSA;
      fi[i] = exp (-0.5 * x * x);
      x1 = x;
    }

  ki[1] = 0;

  /* Zigurrat tables for the exponential distribution */
  x1 = ZIGGURAT_EXP_R;
  we[255] = x1 / EMANTISSA;
  fe[255] = exp (-x1);

  /* Index zero is special for tail strip, where Marsaglia and Tsang 
   * defines this as 
   * k_0 = 2^32 * r * f(r) / v, w_0 = 0.5^32 * v / f(r), f_0 = 1,
   * where v is the area of each strip of the ziggurat. 
   */
  ke[0] = (uint32_t)(floor(x1 * fe[255] / EXP_SECTION_AREA * EMANTISSA));
  we[0] = EXP_SECTION_AREA / fe[255] / EMANTISSA;
  fe[0] = 1.;

  for (i = 254; i > 0; i--)
    {
      /* New x is given by x = f^{-1}(v/x_{i+1} + f(x_{i+1})), thus
       * need inverse operator of y = exp(-x) -> x = -ln(y)
       */
      x = - log(EXP_SECTION_AREA / x1 + fe[i+1]);
      ke[i+1] = floor(x / x1 * EMANTISSA);
      we[i] = x / EMANTISSA;
      fe[i] = exp (-x);
      x1 = x;
    }
  ke[1] = 0;

  /* Zigurrat tables for the polynomial distribution. */
  /* XXX Goes forward instead of backward here, just gratuitously. */
  x1 = 0;
  for (i = 0; i < 255; i++)
    {
      x = polynomial_advance(x1);
      kp[i] = 1 - exp(-x1);
      wp[i] = x;
      fp[i] = exp(-PN * x);
      x1 = x;
    }
  kp[255] = 1 - exp(-1);
  wp[255] = 1;
  fp[255] = 0;
}

void write_double_table(FILE *fp, char *name, double *t, int nt) {
    int i;
    fprintf(fp, "double %s[%d] = {\n", name, nt);
    for (i = 0; i < nt - 1; i++)
	fprintf(fp, "  %.16g,\n", t[i]);
    fprintf(fp, "  %.16g };\n\n", t[nt - 1]);
}

void write_uns_table(FILE *fp, char *name, uint32_t *t, int nt) {
    int i;
    fprintf(fp, "uint32_t %s[%d] = {\n", name, nt);
    for (i = 0; i < nt - 1; i++)
	fprintf(fp, "  %uU,\n", t[i]);
    fprintf(fp, "  %uU };\n\n", t[nt - 1]);
}

void write_header(FILE *fp) {
    fprintf(fp, "/* AUTOGENERATED --- do not edit */\n");
    fprintf(fp, "#include \"zrandom.h\"\n\n");
}

int main(void) {
    FILE *f;
    create_ziggurat_tables();

    f = fopen("normal_tab.c", "w");
    assert(f);
    write_header(f);
    write_uns_table(f, "_rand_normal_k", ki, 256);
    write_double_table(f, "_rand_normal_w", wi, 256);
    write_double_table(f, "_rand_normal_f", fi, 256);
    fclose(f);

    f = fopen("exponential_tab.c", "w");
    assert(f);
    write_header(f);
    write_uns_table(f, "_rand_exponential_k", ke, 256);
    write_double_table(f, "_rand_exponential_w", we, 256);
    write_double_table(f, "_rand_exponential_f", fe, 256);
    fclose(f);

    f = fopen("polynomial_tab.c", "w");
    assert(f);
    write_header(f);
    write_double_table(f, "_rand_polynomial_k", kp, 256);
    write_double_table(f, "_rand_polynomial_w", wp, 256);
    write_double_table(f, "_rand_polynomial_f", fp, 256);
    fclose(f);

    return 0;
}
