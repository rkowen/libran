/*!
\file	LRlspline.c
\brief	Piecewise linear spline distribution

The piecewise linear spline distribution is similar to the piecewise
uniform distribution except that the probability distribution function
looks like the trapezoidal rule for integration, where each segment
is interpolated by a straight line (an affine function, \e i.e. a
polynomial of degree 1).

The PDF is pinned to zero at the end points.  So
at least one point in between must be added with a non-zero value. 
If this is not done then an error will be raised.

Once the \e linear \e spline distribution object is created the
set of trapezoidal \e blocks can be defined with the
set of \c LR_lspl_*() functions.
(Note: the \c LR_aux_*() 
functions are equivalent and generic procedures and can be used instead.)

This piecewise distribution borrows many of the similar methods
as the piecewise \e uniform distribution (LRpiece.c) auxillary
methods since the two have functional equivalency.

\code
#include "libran.h"
...
LR_obj *o = LR_new(lspline, LR_double);
...
// set the endpoints and this pins the probability density height to zero there
LR_set_all(o,"ab", 0., 8.);

// set the number of segments
LR_aux_new(o,6);
...
// must set at least one intervening value to a non-zero value.
LR_aux_set(o, 2.0, 1.0);
LR_aux_set(o, 3.0, 3.0);
LR_aux_set(o, 4.0, 0.0);
LR_aux_set(o, 5.0, 5.0);
LR_aux_set(o, 7.0, 2.0);

// normalize the segmented probability density (the total integral = 1)
LR_aux_norm(o);
...
// do your typical processing
...
// remove the segment pieces
LR_aux_rm(o);
// remove the piecewise LR_obj
LR_rm(&o);
...
\endcode

The probability and cumulative distribution functions as defined by the
above code fragment looks like this:

\image html LinearSplineDistribution.png
\image latex LinearSplineDistribution.eps "Linear Spline Distribution"

\see LRlspline.c LRdf.c


*/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <math.h>	/* sqrt, sqrtf, NAN */
#include "libran.h"
#include <stdio.h>

/*!
@brief	LR_lspl_new(LR_obj *o, int n) - create a new linear spline object

This routine must be called after the \c LR_obj object is created and it
allocates memory for the number of expected segments.

@param	o	LR_obj object
@param	n	largest number of intervals
@return	0 if successful, else non-zero if failed
*/
int LR_lspl_new(LR_obj *o, int n) {
	return LR_pcs_new(o,n);
}

/*!
@brief	LR_lspl_rm(LR_obj *o) - strip out the LR_lspl object part of LR_obj

Removes the allocated memory for the segment pieces.

@param	o	LR_obj object address
@return	0 if successful, else non-zero if failed
*/
int LR_lspl_rm(LR_obj *o) {
	return LR_pcs_rm(o);
}

/*!
@brief	LR_lspl_set(LR_obj *o, double x) - add interval boundary (will order internally).

The \c LR_lspl_set function defines the set of segments for the probability
density function.  It adds a new boundary and the value of the probability
density at that boundary.

Boundary values can be defined in any order and will be ordered internally
by this method.

The outer boundaries are defined through the \c LR_set_all()
function and the probability density is pinned to zero at these
endpoints.

The set of segments and the probability density is relatively defined
with \c LR_lspl_set().  Once all the segments are defined then the probability
density must be normalized with \c LR_lspl_norm()
such that the total integrated value is equal to one!

@param	o	LR_obj object
@param	x	interval boundary to add
@param	p	relative probablity for interval starting at x
@return	0 if successful, else non-zero if failed
*/
int LR_lspl_set(LR_obj *o, double x, double p) {
	int ret = LR_pcs_set(o,x,p);
	/* must have at least one good value set */
	if ((!ret) && (p > 0))
		((LR_pcs *) o->aux)->flags |= LR_AUX_SET;
	return ret;
}

/*!
@brief	LR_lspl_norm(LR_obj *o) - normalize the interval scale factors.

The \c LR_lspl_norm() function is absolutely required to be called so the
integrated probability density equals one.  If this routine
is not called subsequent calls to the CDF/PDF/RAN functions will raise
an error (returning a NAN).

@param	o	LR_obj object
@return	0 if successful, else non-zero if failed
*/
int LR_lspl_norm(LR_obj *o) {
	double zero = 0.0, half = 0.5, one = 1.0, delta = .000001;
	double v = zero;
	LR_pcs *aux = (LR_pcs *) o->aux;

	/* must have at least one good value set else why bother? */
	if (!(aux->flags & LR_AUX_SET))
		return o->errno = LRerr_UnmetPreconditions;

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
		aux->sc[i] = half * (aux->c[i1] + aux->c[i])
			* (aux->bdrs[i1] - aux->bdrs[i]);
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
@brief	LRd_lspline_RAN(LR_obj *o) - double random linear spline distribution
random variate.

@param o	LR_obj object
@return double if OK else NaN
*/
double LRd_lspline_RAN(LR_obj *o) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	double x, y, dy, slope, zero = 0.0, two = 2.0;
	int i = 0;

	/* must have successfully normalized */
	if (!(aux->flags & LR_AUX_NORM)) {
		o->errno = LRerr_NoAuxNormalizeDone;
		return NAN;
	}

	x = o->ud(o);
	/* find interval */
	while (x >= aux->sc[i])	i++;

	/* use inverse method to return variate */
	slope = ((aux->c[i] - aux->c[i-1]) * aux->norm) /
		 (aux->bdrs[i] - aux->bdrs[i-1]);

	y = aux->c[i-1] * aux->norm;
	x -= aux->sc[i-1];
	dy = (sqrt(y*y + two*slope*(x)) - y)/slope;

	y = aux->bdrs[i-1] + dy;
/*
	printf(">>>>>% 8.5f % 8.5f % 8.5f\n",
		y, aux->sc[i-1] + x, LRd_CDF(o, y));
*/
	return y;
}

/*!
@brief	LRd_lspline_PDF(LR_obj *o, double x) - double linear spline probablity distribution function

@param o	LR_obj object
@param x	value
@return double PDF at x
*/
double LRd_lspline_PDF(LR_obj *o, double x) {
	LR_pcs *aux = (LR_pcs *) o->aux;

	/* must have successfully normalized */
	if (!(aux->flags & LR_AUX_NORM)) {
		o->errno = LRerr_NoAuxNormalizeDone;
		return NAN;
	}

	if (x <= o->a.d || x >= o->b.d) {
		return 0.0;
	} else {
		/* find interval */
		int i = 0;
		double slope, y;
		while (x > aux->bdrs[i])	i++;
		slope = (aux->c[i] - aux->c[i-1]) /
			(aux->bdrs[i] - aux->bdrs[i-1]);
		y =	(aux->c[i-1] + (slope * (x - aux->bdrs[i-1])))
			*aux->norm;
/*
	printf("lspline_PDF>>>>>% 8.5f % 8.5f\n", x, y);
*/
		return y;
	}
}

/*!
@brief	LRd_lspline_CDF(LR_obj *o, double x) - double linear spline cumulative distribution function

@param o	LR_obj object
@param x	value
@return double CDF at x
*/
double LRd_lspline_CDF(LR_obj *o, double x) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	double zero = 0.0, one = 1.0, half = 0.5, y;

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
		double diff, slope;
		while (x > aux->bdrs[i])	i++;
		diff =	(x - aux->bdrs[i-1]);
		slope = (aux->c[i] - aux->c[i-1]) /
			(aux->bdrs[i] - aux->bdrs[i-1]);
		y =	(aux->c[i-1] + (slope * diff));
		y = aux->sc[i-1] + half * (y + aux->c[i-1])*diff*aux->norm;
/*
		printf("lspline_CDF>>>>>% 8.5f % 8.5f\n", x, y);
*/
		return y;
	}
}

/*!
@brief	LRf_lspline_RAN(LR_obj *o) - float random linear spline distribution

@param o	LR_obj object
@return float if OK else NaN
*/
float LRf_lspline_RAN(LR_obj *o) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	float x, y, dy, slope, zero = 0.0, two = 2.0;
	int i = 0;

	/* must have successfully normalized */
	if (!(aux->flags & LR_AUX_NORM)) {
		o->errno = LRerr_NoAuxNormalizeDone;
		return NAN;
	}

	x = o->uf(o);
	/* find interval */
	while (x >= aux->sc[i])	i++;

	/* use inverse method to return variate */
	slope = ((aux->c[i] - aux->c[i-1]) * aux->norm) /
		 (aux->bdrs[i] - aux->bdrs[i-1]);

	y = aux->c[i-1] * aux->norm;
	x -= aux->sc[i-1];
	dy = (sqrtf(y*y + two*slope*(x)) - y)/slope;

	y = aux->bdrs[i-1] + dy;
/*
	printf(">>>>>% 8.5f % 8.5f % 8.5f\n",
		y, aux->sc[i-1] + x, LRf_CDF(o, y));
*/
	return y;
}

/*!
@brief	LRf_lspline_PDF(LR_obj *o, float x) - float linear spline probablity distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRf_lspline_PDF(LR_obj *o, float x) {
	LR_pcs *aux = (LR_pcs *) o->aux;

	/* must have successfully normalized */
	if (!(aux->flags & LR_AUX_NORM)) {
		o->errno = LRerr_NoAuxNormalizeDone;
		return NAN;
	}

	if (x <= o->a.f || x >= o->b.f) {
		return 0.0;
	} else {
		/* find interval */
		int i = 0;
		float slope, y;
		while (x > aux->bdrs[i])	i++;
		slope = (aux->c[i] - aux->c[i-1]) /
			(aux->bdrs[i] - aux->bdrs[i-1]);
		y =	(aux->c[i-1] + (slope * (x - aux->bdrs[i-1])))
			*aux->norm;
/*
	printf("lspline_PDF>>>>>% 8.5f % 8.5f\n", x, y);
*/
		return y;
	}
}

/*!
@brief	LRf_lspline_CDF(LR_obj *o, float x) - float linear spline cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRf_lspline_CDF(LR_obj *o, float x) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	float zero = 0.0, one = 1.0, half = 0.5, y;

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
		float diff, slope;
		while (x > aux->bdrs[i])	i++;
		diff =	(x - aux->bdrs[i-1]);
		slope = (aux->c[i] - aux->c[i-1]) /
			(aux->bdrs[i] - aux->bdrs[i-1]);
		y =	(aux->c[i-1] + (slope * diff));
		y = aux->sc[i-1] + half * (y + aux->c[i-1])*diff*aux->norm;
/*
		printf("lspline_CDF>>>>>% 8.5f % 8.5f\n", x, y);
*/
		return y;
	}
}

#ifdef __cplusplus
}
#endif
