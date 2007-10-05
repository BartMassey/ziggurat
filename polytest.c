#include <stdio.h>
#include <math.h>
#include "random.h"

#define NV 10000000
float variate[NV];

#define NB 100
int bin[NB];

#define PN 50

int main(void) {
    int i;
    float maxv = 0;
    float binwidth;
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
	 float x = binwidth * (i + 0.5);
	 printf("%f %d %f\n", x, bin[i],
		(double)NV * (PN + 1) * pow(1 - binwidth * i, PN) * binwidth);
    }
    return 0;
}
