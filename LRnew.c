#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>	/* malloc,free */
#include <string.h>	/* memcpy */
#include "libran.h"

LR_obj *LR_new(LR_type t, LR_data_type d) {
	LR_obj *ptr = (void *) NULL;
	/* ignore LR_type for now */
	if (!(ptr = (LR_obj *) malloc(sizeof(LR_obj))))
		return ptr;
	ptr->d = d;
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
