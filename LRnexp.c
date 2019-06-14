/*!
\file	LRnexp.c
\brief 	The negative-exponential distribution has mean m and variance m^2.

The pseudo-random numbers are distributed from a negative exponential
distribution.  It's only defined on x>=0 and zero otherwise.

PDF(z) = 1/m * exp(-x/m)
CDF(z) = 1 - exp(-x/m)

The default is m = 1 and s will be set to 1/m for efficiency.
 
*/
#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include "libran.h"

/* double */
/*!
@brief	LRd_nexp_RAN(LR_obj *o) - double random negative exponential
distribution using the inversion method.
Default values: peak m = 1.

@param o        LR_obj object
@return double
*/
double LRd_nexp_RAN(LR_obj *o) {
	double one = 1.0;

	/* avoid overflow with 1-U */
	return - o->m.d*log(one - o->ud(o));
}

/*!
@brief	LRd_nexp_PDF(LR_obj *o, double x) - double negative exponential
probablity distribution function

@param o	LR_obj object
@param x	value
@return double PDF at x
*/
double LRd_nexp_PDF(LR_obj *o, double x) {
	double zero = 0.0, one = 1.0;
	if (isnan(o->s.d))
		o->s.d = one/o->m.d;

	if (x < zero)	return zero;

	return o->s.d * exp(- o->s.d * x);
}

/*!
@brief	LRd_nexp_CDF(LR_obj *o, double x) - double negative exponential
cumulative distribution function

@param o	LR_obj object
@param x	value
@return double CDF at x
*/
double LRd_nexp_CDF(LR_obj *o, double x) {
	double zero = 0.0, one = 1.0;
	if (isnan(o->s.d))
		o->s.d = one/o->m.d;

	if (x < zero)	return zero;

	return one - exp(- o->s.d * x);
}

/* float */
/*!
@brief	LRf_nexp_RAN(LR_obj *o) - float random negative exponential
distribution using the inversion method.
Default values: peak m = 1.

@param o        LR_obj object
@return float
*/
float LRf_nexp_RAN(LR_obj *o) {
	float one = 1.0;

	/* avoid overflow with 1-U */
	return - o->m.f*logf(one - o->uf(o));
}

/*!
@brief	LRf_nexp_PDF(LR_obj *o, float x) - float negative exponential
probablity distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRf_nexp_PDF(LR_obj *o, float x) {
	float zero = 0.0, one = 1.0;
	if (isnan(o->s.f))
		o->s.f = one/o->m.f;

	if (x < zero)	return zero;

	return o->s.f * expf(- o->s.f * x);
}

/*!
@brief	LRf_nexp_CDF(LR_obj *o, float x) - float negative exponential
cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRf_nexp_CDF(LR_obj *o, float x) {
	float zero = 0.0, one = 1.0;
	if (isnan(o->s.f))
		o->s.f = one/o->m.f;

	if (x < zero)	return zero;

	return one - expf(- o->s.f * x);
}

#ifdef __cplusplus
}
#endif
