#include <stdio.h>
#include "libran.h"	/* LR_obj */

/* only allow 10007 unique "random" numbers */

double fc = 1.0 / 10007.;

int tirand(LR_obj *o) {
	static int t_iy = 1777;
	t_iy *= 1117;
	t_iy += 37;
	t_iy %= 10007;
	return t_iy;
}

long tlrand(LR_obj *o) {
	return (long) tirand(o);
}

float tfrand(LR_obj *o) {
	return (float) (fc * tirand(o));
}

double tdrand(LR_obj *o) {
	return (double) (fc * tirand(o));
}
