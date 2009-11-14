#include <stdio.h>
#include <math.h>
#include "zrandom.h"

#define NV 10000000
float variate[NV];

#define NB 100
int bin[NB];

#define VAR 1.0

int main(void) {
    int i;
    float maxv = 0, minv = 0;
    float binwidth;
    float a;
    const float b = -0.5 / (VAR * VAR);
    for (i = 0; i < NV; i++)
	 variate[i] = gaussian(VAR);
    for (i = 0; i < NV; i++) {
	float v = variate[i];
	if (v < minv)
	    minv = v;
	if (v > maxv)
	    maxv = v;
    }
    binwidth = (maxv - minv) / NB;
    a = NV * erf(binwidth * 0.5 * M_SQRT1_2 / VAR);
    for (i = 0; i < NV; i++) {
	int j = floor((variate[i] - minv) / binwidth);
	bin[j]++;
    }
    for (i = 0; i < NB; i++) {
	 float x = binwidth * (i + 0.5) + minv;
	 printf("%f %d %f\n", x, bin[i], a * exp(x * x * b));
    }
    return 0;
}
