/*
   Copyright (C) 2007 Bart Massey

   Please see the file LICENSE in this distribution for license terms.
*/
#include <stdio.h>
#include <math.h>
#include "zrandom.h"

#define NV 10000000
double variate[NV];

#define NB 200
int bin[NB];

#define PN 51

int main(void) {
    int i;
    double maxv = 0;
    double binwidth;
    for (i = 0; i < NV; i++)
	 variate[i] = polynomial(PN);
    for (i = 0; i < NV; i++)
	if (variate[i] > maxv)
	    maxv = variate[i];
    binwidth = maxv / NB;
    for (i = 0; i < NV; i++) {
	int j = floor(variate[i] / binwidth);
	bin[j]++;
    }
    for (i = 0; i < NB; i++) {
	 double x = binwidth * (i + 0.5);
	 double x0 = binwidth * i;
	 double x1 = x0 + binwidth;
	 double a = pow(1 - x0, PN + 1) - pow(1-x1, PN + 1);
	 printf("%g %d %g\n", x, bin[i], NV * a);
    }
    return 0;
}
