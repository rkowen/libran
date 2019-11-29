/*!
\file	LRnew.c
\brief	Create a new LR_obj object and set some parameter values

Use these methods to create the `LR_obj` object by allocating memory,
and set certain of the parameters
to default values dependent on the random variate type given.
Other of the routines set these `LR_obj` object attributes or
remove the object.
Lastly there is a check method to correct any of the parameter values
where possible.  (E.g. the interval boundaries b > a, width s > 0, etc.)

The following code segment sets up a `LR_obj` object to generate
uniform distributed random variates on the interval (a=1, b=3).

\code
#include "libran.h"
...
LR_obj *o = LR_new(unif, LR_double);
// set the parameters from the default
LR_set_all(o, "ab", 1., 3.);
// check parameters
LR_check(o);
// set the seed
LR_lsetseed(19580512L);
...
// generate random number
x = LR_RAN(o);
...
// remove object
LR_rm(&o);
...
\endcode

*/
/*
 * Copyright	2019	R.K. Owen, Ph.D.
 * License	see lgpl.md (Gnu Lesser General Public License)
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>	/* malloc,free */
#include <string.h>	/* memcpy */
#include <math.h>	/* NAN */
#include "libran.h"
#include "urand/urand.h"

/*!
@brief	LR_new(LR_type t, LR_data_type d) - create the LR object
and preset some default parameter values.

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
	ptr->aux = (void *) NULL;
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
	case uinvcdf:
	{
		ptr->type = "uinvcdf";
		if (!(ptr->aux = (void *) malloc(sizeof(LR_uinvcdf))))
			goto objerr;
		LR_uinvcdf *aux = (LR_uinvcdf *) ptr->aux;
		if (d == LR_double) {
			aux->dcdf = NULL;
			ptr->a.d = NAN;
			ptr->b.d = NAN;
			ptr->m.d = NAN;
			ptr->s.d = NAN;
			ptr->rnd  = LRd_uinvcdf_RAN;
			ptr->pdfd = LRd_uinvcdf_PDF;
			ptr->cdfd = LRd_uinvcdf_CDF;
		} else if (d == LR_float) {
			aux->fcdf = NULL;
			ptr->a.f = NAN;
			ptr->b.f = NAN;
			ptr->m.f = NAN;
			ptr->s.f = NAN;
			ptr->rnf  = LRf_uinvcdf_RAN;
			ptr->pdff = LRf_uinvcdf_PDF;
			ptr->cdff = LRf_uinvcdf_CDF;
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
	case erlang:
		ptr->type = "erlang";
		ptr->k = 1;
		if (d == LR_double) {
			ptr->m.d = (double) 1.0;
			ptr->s.d = NAN;
			ptr->rnd  = LRd_erlang_RAN;
			ptr->pdfd = LRd_erlang_PDF;
			ptr->cdfd = LRd_erlang_CDF;
		} else if (d == LR_float) {
			ptr->m.f = (float) 1.0;
			ptr->s.f = NAN;
			ptr->rnf  = LRf_erlang_RAN;
			ptr->pdff = LRf_erlang_PDF;
			ptr->cdff = LRf_erlang_CDF;
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
	case gsn4:
		ptr->type = "gsn4";
		if (d == LR_double) {
			ptr->a.d = (double) -2.0;
			ptr->b.d = (double)  2.0;
			ptr->m.d = (double) 0.0;
			ptr->s.d = (double) 2.0;
			ptr->rnd  = LRd_gsn4_RAN;
			ptr->pdfd = LRd_gsn4_PDF;
			ptr->cdfd = LRd_gsn4_CDF;
		} else if (d == LR_float) {
			ptr->a.f = (float)  -2.0;
			ptr->b.f = (float)   2.0;
			ptr->m.f = (float) 0.0;
			ptr->s.f = (float) 2.0;
			ptr->rnf  = LRf_gsn4_RAN;
			ptr->pdff = LRf_gsn4_PDF;
			ptr->cdff = LRf_gsn4_CDF;
		} else {
			/* error */
			ptr->errno = LRerr_BadDataType;
		}
		break;
	case gsn12:
		ptr->type = "gsn12";
		if (d == LR_double) {
			ptr->a.d = (double) -6.0;
			ptr->b.d = (double)  6.0;
			ptr->m.d = (double) 0.0;
			ptr->s.d = (double) 1.0;
			ptr->rnd  = LRd_gsn12_RAN;
			ptr->pdfd = LRd_gsn12_PDF;
			ptr->cdfd = LRd_gsn12_CDF;
		} else if (d == LR_float) {
			ptr->a.f = (float)  -6.0;
			ptr->b.f = (float)   6.0;
			ptr->m.f = (float) 0.0;
			ptr->s.f = (float) 1.0;
			ptr->rnf  = LRf_gsn12_RAN;
			ptr->pdff = LRf_gsn12_PDF;
			ptr->cdff = LRf_gsn12_CDF;
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
@return	0	if successful, non-zero if an error
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

This method finds and addresses fixable errors.  For example for those
random variate distributions that use the interval end points (a,b) the
check method will switch 'a' and 'b' if 'a' > 'b'.  The other example
is for those distributions that use 's' which generally relates to
the width of the peak.  This value should be positive and the check will
take the absolute value of this value if negative.

However, non-fixable errors will raise an error, such as a=b in the first
case or s=0 for the second case.

@param	o	LR_obj pointer
@return	0	if successful, non-zero if an error or not possible to fix
*/
int LR_check(LR_obj *o) {
	double	td, dzero = 0.0;
	float	tf, fzero = 0.0;
	/* check if LR_obj */
	if (o && o->t && o->d) {
		switch (o->t) {
		/* interval types (a,b) */
		case unif:
		case piece:
		case lspline:
		case gsn2:
		case gsn4:
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

		/* mixed attributes (a,b,m,s) */
		case uinvcdf:
			if (o->d == LR_double) {
			    if ((!isnan(o->a.d)) && (!isnan(o->b.d))) {
				if (o->a.d > o->b.d) {
					td = o->a.d;
					o->a.d = o->b.d;
					o->b.d = td;
				} else if (o->a.d == o->b.d) {
					return o->errno = LRerr_InvalidRange;
				}
			    }
			    if ((!isnan(o->s.d))) {
				if (o->s.d < dzero) {
					o->s.d = - o->s.d;
				} else if (o->s.d == dzero) {
					return o->errno
						= LRerr_InvalidInputValue;
				}
			    }
			    if (isnan(o->a.d) && isnan(o->b.d)
			    &&  isnan(o->m.d) && isnan(o->s.d))
				return o->errno = LRerr_UnmetPreconditions;

			} else if (o->d == LR_float) {
			    if ((!isnan(o->a.f)) && (!isnan(o->b.f))) {
				if (o->a.f > o->b.f) {
					tf = o->a.f;
					o->a.f = o->b.f;
					o->b.f = tf;
				} else if (o->a.f == o->b.f) {
					return o->errno = LRerr_InvalidRange;
				}
			    }
			    if ((!isnan(o->s.f))) {
				if (o->s.f < fzero) {
					o->s.f = - o->s.f;
				} else if (o->s.f == fzero) {
					return o->errno
						= LRerr_InvalidInputValue;
				}
			    }
			    if (isnan(o->a.f) && isnan(o->b.f)
			    &&  isnan(o->m.f) && isnan(o->s.f))
				return o->errno = LRerr_UnmetPreconditions;

			} else {
				/* error */
				return o->errno = LRerr_BadDataType;
			}
			return LRerr_OK;

		/* full range (m,s) */
		case gausbm:
		case gausmar:
		case gsn12:
		case cauchy:
		case cauchymar:
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
		/* semi-infinite (m)*/
		case nexp:
			if (o->d == LR_double) {
				if (o->m.d < dzero) {
					o->m.d = - o->m.d;
				} else if (o->m.d == dzero) {
					return o->errno
						= LRerr_InvalidInputValue;
				}
			} else if (o->d == LR_float) {
				if (o->m.f < fzero) {
					o->m.f = - o->m.f;
				} else if (o->m.f == fzero) {
					return o->errno
						= LRerr_InvalidInputValue;
				}
			} else {
				/* error */
				return o->errno = LRerr_BadDataType;
			}
			return LRerr_OK;

		case erlang:
			if (o->k < 0) {
				o->k = - o->k;
			} else if (o->k == 0) {
				return o->errno = LRerr_InvalidInputValue;
			}
			if (o->d == LR_double) {
				if (o->m.d < dzero) {
					o->m.d = - o->m.d;
				} else if (o->m.d == dzero) {
					return o->errno
						= LRerr_InvalidInputValue;
				}
			} else if (o->d == LR_float) {
				if (o->m.f < fzero) {
					o->m.f = - o->m.f;
				} else if (o->m.f == fzero) {
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

#ifdef __cplusplus
}
#endif
