/*!
\file	LRpiece.c
\brief	Piecewise uniform distribution

The Piecewise uniform distribution is a probability distribution function
that looks like a histogram, where each block is of some width \f$ x_n \f$
and height of \f$ p_n \f$.

Once the \e piecewise distribution object is created the set of \e blocks
can be defined with the \c LR_pcs_*() functions.

\code
#include "libran.h"
...
LR_obj *o = LR_new(piece, LR_double);
...
// set the end points and the probability density height of the first segment
LR_set_all(o,"abx", 0., 8., 4.);

// set the number of pieces
LR_pcs_new(o,6);

// now set the probability density height of the subsequent segments
LR_pcs_set(o, 2.0, 1.0);
LR_pcs_set(o, 3.0, 3.0);
LR_pcs_set(o, 4.0, 0.0);
LR_pcs_set(o, 5.0, 5.0);
LR_pcs_set(o, 7.0, 2.0);

// normalize the segmented probability density (the total integral = 1)
LR_pcs_norm(o);
...
// do your typical processing
...
// remove the segment pieces
LR_pcs_rm(o);
// remove the piecewise LR_obj
LR_rm(&o);
...
\endcode

The probability and cumulative distribution functions as defined by the
above code fragment looks like this:

\image html PiecewiseUniformDistribution.png
\image latex PiecewiseUniformDistribution.eps "Piecewise Uniform Distribution"

*/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <math.h>
#include "libran.h"

/*!
@brief	LR_pcs_new(LR_obj *o, int n) - create a new piecewise uniform object set of segments

This routine must me called after the \c LR_obj object is created and it
allocates memory for the number of expected segments.

@param	o	LR_obj object
@param	n	largest number of intervals
@return	0 if successful, else non-zero if failed
*/
int LR_pcs_new(LR_obj *o, int n) {
	LR_pcs *ptr = (LR_pcs *) o->aux;

	if (o->t != piece
	&&  o->t != lspline)
		return o->errno = LRerr_BadLRType;

	ptr->n  = n;
	ptr->nn = 1;	/* always start with one interval */
	ptr->c  = 0;
	ptr->norm  = 0.;
	ptr->flags  = 0;

	if (!(ptr->bdrs = (double *) calloc(n, sizeof(double))))
		goto bad0;

	if (!(ptr->c = (double *) calloc(n, sizeof(double))))
		goto bad1;

	if (!(ptr->sc = (double *) calloc(n + 1, sizeof(double))))
		goto bad2;

	return LRerr_OK;

bad2:
	free((void *) ptr->sc);

bad1:
	free((void *) ptr->c);

bad0:
	free((void *) ptr->bdrs);

	return o->errno = LRerr_AllocFail;
}

/*!
@brief	LR_pcs_rm(LR_obj *o) - strip out the LR_pcs object part of LR_obj

Removes the allocated memory for the segment pieces.

@param	o	LR_obj object address
@return	0 if successful, else non-zero if failed
*/
int LR_pcs_rm(LR_obj *o) {
	LR_pcs *aux;
	if (o && (o->t == piece || o->t == lspline)
	&&  o->aux) {
		aux = (LR_pcs *) o->aux;
		free((void *) aux->bdrs);
		free((void *) aux->c);
		free((void *) aux->sc);
		return LRerr_OK;
	}
	return o->errno = LRerr_Unspecified;
}

/*!
@brief	LR_pcs_set(LR_obj *o, double x) - add interval boundary (will order internally).

The \c LR_pcs_set function defines the set of segments for the probability
density function.  It adds a new boundary and the value of the probability
density following that boundary.

The first segment (and boundary) is defined through the \c LR_set_all()
function, which also defines the last segment end point.

The set of segments and the probability density is relatively defined
with \c LR_pcs_set().  Once all the segments are defined then the probability
density must be normalized with \c LR_pcs_norm()
such that the total integrated value is equal to one!

@param	o	LR_obj object
@param	x	interval boundary to add
@param	p	relative probablity for interval greater than x
@return	0 if successful, else non-zero if failed
*/
int LR_pcs_set(LR_obj *o, double x, double p) {
	int i = 0;
	double t, tp;
	LR_pcs *aux = (LR_pcs *) o->aux;
	if (aux->n < aux->nn + 1) {
		/* too many values given */
		return o->errno = LRerr_TooManyValues;
	}
	if (p < 0) {
		/* invalid probability */
		return o->errno = LRerr_InvalidInputValue;
	}
	if (o->d == LR_double) {
		if (x < o->a.d || x > o->b.d)
			/* bad range */
			return o->errno = LRerr_InvalidRange;
	} else if (o->d == LR_float) {
		if (x < o->a.f || x > o->b.f)
			/* bad range */
			return o->errno = LRerr_InvalidRange;
	}
	t  = aux->bdrs[0];
	tp = aux->c[0];
	for (int i = 0, i1 = 1; i <= aux->nn; i++,i1++) {
		if (aux->nn == i1) {	/* got to top entry */
			aux->bdrs[i] = x;
			aux->c[i] = p;
			aux->nn++;
			return LRerr_OK;
		}
		if (x > t) {
			/* compare to next one, next round */
			t  = aux->bdrs[i1];
			tp = aux->c[i1];
		} else {
			/* insert here, but keep current */
			t  = aux->bdrs[i];
			tp = aux->c[i];
			aux->bdrs[i] = x;
			/* use current for comparison */
			x = t;
			p = tp;
			t  = aux->bdrs[i1];
			tp = aux->c[i1];
		}
	}
	aux->nn++;

	return LRerr_OK;
}

/*!
@brief	LR_pcs_norm(LR_obj *o) - normalize the interval scale factors.

The \c LR_pcs_norm() function is absolutely required to be called so the
integrated probability density equals one.  If this routine
is not called subsequent calls to the CDF/PDF/RAN functions will raise
an error (returning a NAN).

@param	o	LR_obj object
@return	0 if successful, else non-zero if failed
*/
int LR_pcs_norm(LR_obj *o) {
	double zero = 0.0, one = 1.0, delta = .000001;
	double v= zero;
	LR_pcs *aux = (LR_pcs *) o->aux;

	/* move up the set of values */
	for (int i = aux->nn - 1, i1 = aux->nn - 2; i >= 1; i--, i1--) {
		aux->bdrs[i] = aux->bdrs[i1];
		aux->c[i] = aux->c[i1];
	}
	if (o->d == LR_double) {
		aux->bdrs[0] = o->a.d;
		aux->bdrs[aux->nn] = o->b.d;
		aux->c[0] = o->x.d;
	} else if (o->d == LR_float) {
		aux->bdrs[0] = (double) o->a.f;
		aux->bdrs[aux->nn] = (double) o->b.f;
		aux->c[0] = (double) o->x.f;
	}

	/* integrate over each interval */
	aux->norm = zero;
	for (int i = 0, i1 = 1; i < aux->nn; i++, i1++) {
		aux->norm += 
		aux->sc[i] = aux->c[i] * (aux->bdrs[i1] - aux->bdrs[i]);
	}

	/* collect and rescale the CDF */
	aux->norm = one/aux->norm;
	for (int i = 0; i < aux->nn; i++) {
		v += aux->sc[i];
		aux->sc[i] = v * aux->norm;
	}
	/* shift the CDF set up */
	if (aux->sc[aux->nn-1] < one - delta
	||  one + delta < aux->sc[aux->nn-1]) {
		/* the last value should be 1.0 */
		return o->errno = LRerr_SuspiciousValues;
	}
	for (int i = aux->nn-1; i > 0; i--) {
		aux->sc[i] = aux->sc[i-1];
	}
	aux->sc[0] = zero;
	aux->sc[aux->nn] = one;
	/* norm success */
	((LR_pcs *) o->aux)->flags |= LR_AUX_NORM;

	return LRerr_OK;
}

/*!
@brief	LRd_piece_RAN(LR_obj *o) - double random piecewise uniform distribution

@param o	LR_obj object
@return double if OK, else NaN
*/
double LRd_piece_RAN(LR_obj *o) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	double x, dx, zero = 0.0;
	int i = 0;

	/* must have successfully normalized */
	if (!(aux->flags & LR_AUX_NORM)) {
		o->errno = LRerr_NoAuxNormalizeDone;
		return NAN;
	}

	x = o->ud(o);
	/* find interval */
	while (x > aux->sc[i])	i++;

	if (aux->c[i-1] == zero)	return aux->bdrs[i-1];

	dx = (x - aux->sc[i-1]) / (aux->c[i-1] * aux->norm);
	return aux->bdrs[i-1] + dx;
}

/*!
@brief	LRd_piece_PDF(LR_obj *o, double x) - double piecewise uniform probablity distribution function

@param o	LR_obj object
@param x	value
@return double PDF at x, else a NAN if an error
*/
double LRd_piece_PDF(LR_obj *o, double x) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	double zero = 0.0;

	/* must have successfully normalized */
	if (!(aux->flags & LR_AUX_NORM)) {
		o->errno = LRerr_NoAuxNormalizeDone;
		return NAN;
	}

	if (x <= o->a.d || x > o->b.d) {
		return zero;
	} else {
		/* find interval */
		int i = 0;
		while (x > aux->bdrs[i])	i++;
		return aux->c[i-1] * aux->norm;
	}
}

/*!
@brief	LRd_piece_CDF(LR_obj *o, double x) - double piecewise uniform cumulative distribution function

@param o	LR_obj object
@param x	value
@return double CDF at x, else a NAN if an error
*/
double LRd_piece_CDF(LR_obj *o, double x) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	double zero = 0.0, one = 1.0;

	/* must have successfully normalized */
	if (!(aux->flags & LR_AUX_NORM)) {
		o->errno = LRerr_NoAuxNormalizeDone;
		return NAN;
	}

	if (x <= o->a.d) {
		return zero;
	} else if (x >= o->b.d) {
		return one;
	} else {
		/* find interval */
		int i = 0;
		while (x > aux->bdrs[i])	i++;
		return aux->sc[i-1] + aux->c[i-1]*aux->norm*(x-aux->bdrs[i-1]);
	}
}

/*!
@brief	LRf_piece_RAN(LR_obj *o) - float random piecewise uniform distribution

@param o	LR_obj object
@return float if OK, else NaN
*/
float LRf_piece_RAN(LR_obj *o) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	float x, dx, zero = 0.0;
	int i = 0;

	/* must have successfully normalized */
	if (!(aux->flags & LR_AUX_NORM)) {
		o->errno = LRerr_NoAuxNormalizeDone;
		return NAN;
	}

	x = o->ud(o);
	/* find interval */
	while (x > aux->sc[i])	i++;

	if (aux->c[i-1] == zero)	return aux->bdrs[i-1];

	dx = (x - aux->sc[i-1]) / (aux->c[i-1] * aux->norm);
	return aux->bdrs[i-1] + dx;
}

/*!
@brief	LRf_piece_PDF(LR_obj *o, float x) - float piecewise uniform probablity distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x, else a NAN if an error
*/
float LRf_piece_PDF(LR_obj *o, float x) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	float zero = 0.0;

	/* must have successfully normalized */
	if (!(aux->flags & LR_AUX_NORM)) {
		o->errno = LRerr_NoAuxNormalizeDone;
		return NAN;
	}

	if (x <= o->a.f || x > o->b.f) {
		return zero;
	} else {
		/* find interval */
		int i = 0;
		while (x > aux->bdrs[i])	i++;
		return aux->c[i-1] * aux->norm;
	}
}

/*!
@brief	LRf_piece_CDF(LR_obj *o, float x) - float piecewise uniform cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x, else a NAN if an error
*/
float LRf_piece_CDF(LR_obj *o, float x) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	float zero = 0.0, one = 1.0;

	/* must have successfully normalized */
	if (!(aux->flags & LR_AUX_NORM)) {
		o->errno = LRerr_NoAuxNormalizeDone;
		return NAN;
	}

	if (x <= o->a.f) {
		return zero;
	} else if (x >= o->b.f) {
		return one;
	} else {
		/* find interval */
		int i = 0;
		while (x > aux->bdrs[i])	i++;
		return aux->sc[i-1] + aux->c[i-1]*aux->norm*(x-aux->bdrs[i-1]);
	}
}

#ifdef __cplusplus
}
#endif
