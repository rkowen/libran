#include <stdio.h>

/* only allow 10007 unique "random" numbers */

double fc = 1.0 / 10007.;

int tirand(void) {
	static int t_iy = 1777;
	t_iy *= 1117;
	t_iy += 37;
	t_iy %= 10007;
	return t_iy;
}

long tlrand(void) {
	return (long) tirand();
}

float tfrand(void) {
	return (float) (fc * tirand());
}

double tdrand(void) {
	return (double) (fc * tirand());
}
