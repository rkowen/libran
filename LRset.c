#include <stdarg.h>	/* va_arg */
#include <string.h>	/* memcpy */
#include "libran.h"

int LR_vset(LR_obj *o, char *x, va_list ap) {
	LR_val t;
	char x1;

	while (x1 = *x++) {
		switch (o->d) {
		case LR_int:
			t.i = va_arg(ap, int);
			break;
		case LR_long:
			t.l = va_arg(ap, long);
			break;
		case LR_float:
			/* floats are promoted to double in ... */
			t.f = va_arg(ap, double);
			break;
		case LR_double:
			t.d = va_arg(ap, double);
			break;
		default:
			return 1;
		}

		switch (x1) {
		case	'd':
			(void) memcpy(&(o->d), &t, sizeof(LR_val));
			break;
		case	'a':
			(void) memcpy(&(o->a), &t, sizeof(LR_val));
			break;
		case	'b':
			(void) memcpy(&(o->b), &t, sizeof(LR_val));
			break;
		case	'm':
			(void) memcpy(&(o->m), &t, sizeof(LR_val));
			break;
		case	's':
			(void) memcpy(&(o->s), &t, sizeof(LR_val));
			break;
		default:
			/* it is an error to include non-object attributes */
			return 1;
		}
	}
	return 0;
}

int LR_set(LR_obj *o, char x, ...) {
	va_list ap;
	int ret = 0;
	char xx[2] = "0";

	va_start(ap, x);

	xx[0] = x;
	ret = LR_vset(o, xx, ap);

	va_end(ap);

	return ret;
}

int LR_set_all(LR_obj *o, char *x, ...) {
	va_list ap;
	int ret = 0;

	va_start(ap, x);

	ret += LR_vset(o, x, ap);

	va_end(ap);

	return ret;
}

