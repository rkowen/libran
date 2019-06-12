/*!
\file	LRnew.c
\brief	Create a new LR_obj object and set some parameter values

The LR_obj is created from allocated memory and certain of the parameters
are set to default values dependent on the random variate type given.

A check function is added to check and correct any of the parameter values
possible.  (E.g. the interval boundaries b > a, width s > 0, etc.)

*/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>	/* malloc,free */
#include <string.h>	/* memcpy */
#include <math.h>	/* NAN */
#include "libran.h"
#include "urand/urand.h"

/*!
@brief	LR_new(LR_type t, LR_data_type d) - create the LR object and preset
some default parameter values.

@param	t	LR_type (e.g. LR_gausbm)
@param	d	LR_data_type (e.g. LR_double)
@return	LR_obj *
*/
LR_obj *LR_new(LR_type t, LR_data_type d) {
	LR_obj *ptr = (void *) NULL;
	if (!(ptr = (LR_obj *) malloc(sizeof(LR_obj))))
		return ptr;
	ptr->errno = 0;
	ptr->d = d;
	/* set all the pseudo-uniform random number generators */
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
			ptr->errno = LRerr_BadDataType;
		}
		break;
	case piece:
	{
		ptr->type = "piece";
		if (!(ptr->aux = (void *) malloc(sizeof(LR_pcs))))
			goto objerr;
		LR_pcs *aux = (LR_pcs *) ptr->aux;
		aux->new = LR_pcs_new;
		aux->rm = LR_pcs_rm;
		aux->set = LR_pcs_set;
		aux->normalize = LR_pcs_norm;
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
			ptr->errno = LRerr_BadDataType;
		}
	}
		break;
	case lspline:
	{
		ptr->type = "lspline";
		if (!(ptr->aux = (void *) malloc(sizeof(LR_pcs))))
			goto objerr;
		LR_pcs *aux = (LR_pcs *) ptr->aux;
		aux->new = LR_lspl_new;
		aux->rm = LR_lspl_rm;
		aux->set = LR_lspl_set;
		aux->normalize = LR_lspl_norm;
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
			ptr->errno = LRerr_BadDataType;
		}
	}
		break;
	case nexp:
		ptr->type = "nexp";
		if (d == LR_double) {
			ptr->m.d = (double) 1.0;
			ptr->s.d = NAN;
			ptr->rnd  = LRd_nexp_RAN;
			ptr->pdfd = LRd_nexp_PDF;
			ptr->cdfd = LRd_nexp_CDF;
		} else if (d == LR_float) {
			ptr->m.f = (float) 1.0;
			ptr->s.f = NAN;
			ptr->rnf  = LRf_nexp_RAN;
			ptr->pdff = LRf_nexp_PDF;
			ptr->cdff = LRf_nexp_CDF;
		} else {
			/* error */
			ptr->errno = LRerr_BadDataType;
		}
		break;
	case gausbm:
		ptr->type = "gausbm";
		if (d == LR_double) {
			ptr->m.d = (double) 0.0;
			ptr->s.d = (double) 1.0;
			ptr->x.d = NAN;
			ptr->rnd  = LRd_gausbm_RAN;
			ptr->pdfd = LRd_gaus_PDF;
			ptr->cdfd = LRd_gaus_CDF;
		} else if (d == LR_float) {
			ptr->m.f = (float) 0.0;
			ptr->s.f = (float) 1.0;
			ptr->x.f = NAN;
			ptr->rnf  = LRf_gausbm_RAN;
			ptr->pdff = LRf_gaus_PDF;
			ptr->cdff = LRf_gaus_CDF;
		} else {
			/* error */
			ptr->errno = LRerr_BadDataType;
		}
		break;
	case gausmar:
		ptr->type = "gausmar";
		if (d == LR_double) {
			ptr->m.d = (double) 0.0;
			ptr->s.d = (double) 1.0;
			ptr->x.d = NAN;
			ptr->rnd  = LRd_gausmar_RAN;
			ptr->pdfd = LRd_gaus_PDF;
			ptr->cdfd = LRd_gaus_CDF;
		} else if (d == LR_float) {
			ptr->m.f = (float) 0.0;
			ptr->s.f = (float) 1.0;
			ptr->x.f = NAN;
			ptr->rnf  = LRf_gausmar_RAN;
			ptr->pdff = LRf_gaus_PDF;
			ptr->cdff = LRf_gaus_CDF;
		} else {
			/* error */
			ptr->errno = LRerr_BadDataType;
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
			ptr->errno = LRerr_BadDataType;
		}
		break;
	case cauchy:
		ptr->type = "cauchy";
		if (d == LR_double) {
			ptr->m.d = (double) 0.0;
			ptr->s.d = (double) 1.0;
			ptr->rnd  = LRd_cauchy_RAN;
			ptr->pdfd = LRd_cauchy_PDF;
			ptr->cdfd = LRd_cauchy_CDF;
		} else if (d == LR_float) {
			ptr->m.f = (float) 0.0;
			ptr->s.f = (float) 1.0;
			ptr->rnf  = LRf_cauchy_RAN;
			ptr->pdff = LRf_cauchy_PDF;
			ptr->cdff = LRf_cauchy_CDF;
		} else {
			/* error */
			ptr->errno = LRerr_BadDataType;
		}
		break;
	case cauchymar:
		ptr->type = "cauchymar";
		if (d == LR_double) {
			ptr->m.d = (double) 0.0;
			ptr->s.d = (double) 1.0;
			ptr->rnd  = LRd_cauchymar_RAN;
			ptr->pdfd = LRd_cauchy_PDF;
			ptr->cdfd = LRd_cauchy_CDF;
		} else if (d == LR_float) {
			ptr->m.f = (float) 0.0;
			ptr->s.f = (float) 1.0;
			ptr->rnf  = LRf_cauchymar_RAN;
			ptr->pdff = LRf_cauchy_PDF;
			ptr->cdff = LRf_cauchy_CDF;
		} else {
			/* error */
			ptr->errno = LRerr_BadDataType;
		}
		break;
	default:
		/* error */
		ptr->errno = LRerr_BadLRType;
		break;
	}

	return  ptr;

objerr:
	free((void *) ptr);
	ptr = (void *) NULL;
	return  ptr;
}

/*!
@brief	LR_rm(LR_obj **o) - destroy the LR object and release allocated
memory.

@param	o	Address of LR_obj pointer
@return	0	if successful
*/
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
			return LRerr_OK;
		default:
			return LRerr_BadDataType;
		}
	}
}

/*!
@brief	LR_check(LR_obj *o) - check and fix the LR object parameters
if possible

@param	o	LR_obj pointer
@return	0	if successful, non-zero if an error or not possible to fix
*/
int LR_check(LR_obj *o) {
	double	td, dzero = 0.0;
	float	tf, fzero = 0.0;
	/* check if LR_obj */
	if (o && o->t && o->d) {
		switch (o->t) {
		/* interval types */
		case unif:
		case piece:
		case lspline:
		case gsn2:
			if (o->d == LR_double) {
				if (o->a.d > o->b.d) {
					td = o->a.d;
					o->a.d = o->b.d;
					o->b.d = td;
				} else if (o->a.d == o->b.d) {
					return o->errno = LRerr_InvalidRange;
				}
			} else if (o->d == LR_float) {
				if (o->a.f > o->b.f) {
					tf = o->a.f;
					o->a.f = o->b.f;
					o->b.f = tf;
				} else if (o->a.d == o->b.d) {
					return o->errno = LRerr_InvalidRange;
				}
			} else {
				/* error */
				return o->errno = LRerr_BadDataType;
			}
			return LRerr_OK;

		/* full range */
		case gausbm:
			if (o->d == LR_double) {
				if (o->s.d < dzero) {
					o->s.d = - o->s.d;
				} else if (o->s.d == dzero) {
					return o->errno
						= LRerr_InvalidInputValue;
				}
			} else if (o->d == LR_float) {
				if (o->s.f < fzero) {
					o->s.f = - o->s.f;
				} else if (o->s.f == fzero) {
					return o->errno
						= LRerr_InvalidInputValue;
				}
			} else {
				/* error */
				return o->errno = LRerr_BadDataType;
			}
			return LRerr_OK;

		default:
			return o->errno = LRerr_BadLRType;
		}
	} else {
		return	LRerr_Unspecified;
	}
}

