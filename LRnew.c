#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>	/* malloc,free */
#include <string.h>	/* memcpy */
#include "libran.h"
#include "urand/urand.h"

LR_obj *LR_new(LR_type t, LR_data_type d) {
	LR_obj *ptr = (void *) NULL;
	if (!(ptr = (LR_obj *) malloc(sizeof(LR_obj))))
		return ptr;
	ptr->d = d;
	/* set all the pseudo-uniform random number generator */
	ptr->ui = LR_irand;
	ptr->ul = LR_lrand;
	ptr->uf = LR_frand;
	ptr->ud = LR_drand;

	/* set the default values based on LR_type */
	ptr->t = t;
	switch (t) {
	case unif: if (d == LR_double) {
			ptr->a.d = (double) 0.0;
			ptr->b.d = (double) 1.0;
		} else if (d == LR_float) {
			ptr->a.f = (float) 0.0;
			ptr->b.f = (float) 1.0;
		} else {
			/* error */
		}
	default:
		/* error */
		break;
	}

	return  ptr;
}

int LR_rm(LR_obj **o) {
	/* check if LR_obj */
	if (o && *o) {
		switch ((*o)->d) {
		case LR_int:
		case LR_long:
		case LR_float:
		case LR_double:
			free((void *) *o);
			*o = (LR_obj *) NULL;
			return 0;
		default:
			return 1;
		}
	}
}

