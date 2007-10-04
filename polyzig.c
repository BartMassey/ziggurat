#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "zigconsts.h"

static double polynomial_advance(double A, double x0) {
    double dx = 0.5;
    double x = x0;
    while(1) {
	double x1 = x + dx;
	double a = (x1 - x0) * (exp(-PN * x0) - exp(-PN * x1));
	if (x1 == x)
	    return x;
	if (a <= A)
	    x = x1;
	dx *= 0.5;
    }
    abort();
}

int main(void) {
    double da = 0.5;
    double a = 1.0;
    while(1) {
	double a1 = a - da;
	double x = 0;
	int i;
	da *= 0.5;
	if (a1 == a) {
	    printf("%.23g\n", a);
	    exit(0);
	}
	for (i = 0; i < 256; i++) {
	    x = polynomial_advance(a1, x);
	    if (x > 1) {
		a = a1;
		break;
	    }
	}
    }
    abort();
}
