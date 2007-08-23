#include <stdio.h>
#include <math.h>
#include "random.h"

#define NV 10000000
float variate[NV];

#define NB 100
int bin[NB];

#define VAR 2.0

int main(void) {
    int i;
    float maxv = 0;
    float binwidth;
    int maxbin;
    for (i = 0; i < NV; i++)
	 variate[i] = gaussian(VAR);
    for (i = 0; i < NV; i++) {
	float v = abs(variate[i]);
	if (v > maxv)
	    maxv = v;
    }
    binwidth = 2 * maxv / (NB - 1);
    for (i = 0; i < NV; i++) {
	int j = floor((variate[i] + maxv) / binwidth);
	bin[j]++;
    }
    maxbin = 0;
    for (i = 0; i < NB; i++)
	if (bin[i] > maxbin)
	    maxbin = bin[i];
    for (i = 0; i < NB; i++) {
	 float x = binwidth * i;
	 printf("%f %d %f\n", x, bin[i],
		maxbin * exp(-x * x * 0.5 / (VAR * VAR)));
    }
    return 0;
}


