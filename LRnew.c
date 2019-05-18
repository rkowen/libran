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
	case unif:
		ptr->type = "unif";
		if (d == LR_double) {
			ptr->a.d = (double) 0.0;
			ptr->b.d = (double) 1.0;
			ptr->rnd  = LRd_unif_RAN;
			ptr->pdfd = LRd_unif_PDF;
			ptr->cdfd = LRd_unif_CDF;
		} else if (d == LR_float) {
			ptr->a.f = (float) 0.0;
			ptr->b.f = (float) 1.0;
			ptr->rnf  = LRf_unif_RAN;
			ptr->pdff = LRf_unif_PDF;
			ptr->cdff = LRf_unif_CDF;
		} else {
			/* error */
		}
		break;
	case piece:
		ptr->type = "piece";
		if (!(ptr->aux = (void *) malloc(sizeof(LR_pcs))))
			goto objerr;
		if (d == LR_double) {
			ptr->a.d = (double) -1.0;
			ptr->b.d = (double)  1.0;
			ptr->x.d = (double)  1.0;
			ptr->rnd  = LRd_piece_RAN;
			ptr->pdfd = LRd_piece_PDF;
			ptr->cdfd = LRd_piece_CDF;
		} else if (d == LR_float) {
			ptr->a.f = (float) -1.0;
			ptr->b.f = (float)  1.0;
			ptr->x.f = (float)  1.0;
			ptr->rnf  = LRf_piece_RAN;
			ptr->pdff = LRf_piece_PDF;
			ptr->cdff = LRf_piece_CDF;
		} else {
			/* error */
		}
		break;
	case lspline:
		ptr->type = "lspline";
		if (!(ptr->aux = (void *) malloc(sizeof(LR_pcs))))
			goto objerr;
		if (d == LR_double) {
			ptr->a.d = (double) -1.0;
			ptr->b.d = (double)  1.0;
			ptr->rnd  = LRd_lspline_RAN;
			ptr->pdfd = LRd_lspline_PDF;
			ptr->cdfd = LRd_lspline_CDF;
		} else if (d == LR_float) {
			ptr->a.f = (float) -1.0;
			ptr->b.f = (float)  1.0;
			ptr->rnf  = LRf_lspline_RAN;
			ptr->pdff = LRf_lspline_PDF;
			ptr->cdff = LRf_lspline_CDF;
		} else {
			/* error */
		}
		break;
	case gsn2:
		ptr->type = "gsn2";
		if (d == LR_double) {
			ptr->a.d = (double) -1.0;
			ptr->b.d = (double)  1.0;
			ptr->m.d = (double) 0.0;
			ptr->s.d = (double) 1.0;
			ptr->rnd  = LRd_gsn2_RAN;
			ptr->pdfd = LRd_gsn2_PDF;
			ptr->cdfd = LRd_gsn2_CDF;
		} else if (d == LR_float) {
			ptr->a.f = (float)  -1.0;
			ptr->b.f = (float)   1.0;
			ptr->m.f = (float) 0.0;
			ptr->s.f = (float) 1.0;
			ptr->rnf  = LRf_gsn2_RAN;
			ptr->pdff = LRf_gsn2_PDF;
			ptr->cdff = LRf_gsn2_CDF;
		} else {
			/* error */
		}
		break;
	default:
		/* error */
		break;
	}

	return  ptr;

objerr:
	free((void *) ptr);
	ptr = (void *) NULL;
	return  ptr;
}

int LR_rm(LR_obj **o) {
	/* check if LR_obj */
	if (o && *o) {
		if (((*o)->t == piece)
		||  ((*o)->t == lspline)) {
			free((void *) (*o)->aux);
		}
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

