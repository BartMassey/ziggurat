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
    double a0 = 0;
    double x;
    int i;
    while(1) {
	double a1 = a - da;
	x = 0;
	da *= 0.5;
	if (a1 == a)
	    break;
	for (i = 0; i < 256; i++) {
	    x = polynomial_advance(a1, x);
	    if (x > 1) {
		a = a1;
		break;
	    }
	}
    }
    printf("%.14g\n", a);
    x = 0;
    for (i = 0; i < 256; i++) {
	double x1 = polynomial_advance(a, x);
	double a1 = (x1 - x) * (exp(-PN * x) - exp(-PN * x1));
	a0 += a1;
	x = x1;
    }
    printf("%.14g %.14g %.14g\n", a * 256, a0, x);
    exit(0);
}
