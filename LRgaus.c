/*!
\file	LRgs.c
\brief 	The Gaussian (Normal) distributions centered on m and width given by 2s

The pseudo-random numbers are distributed from a Gaussian or Normal
distribution.  The mean is given by m and variance = s.

PDF(z) = 1/(s*sqrt(2pi)) exp(-(z-m)^2 / (2s^2))

The default is m = 0, s = 1.
 
*/
#include <math.h>
#include "libran.h"

/* double */
/*!
@brief	LRd_gausbm_RAN(LR_obj *o) - double random gaussian/normal distribution
using the Box-Muller method.
Default values: mean m = 0, std.deviation s = 1

@param o        LR_obj object
@return double
*/
double LRd_gausbm_RAN(LR_obj *o) {
	double one = 1.0, ntwo = -2.0, twopi = 2.0 * M_PI;
	double c, z1, z2;

	if (isnan(o->x.d)) {
		/* generate new pair - 1-U avoids possible overflow */
		c = sqrt(ntwo * log(one - o->ud()));
		z2 = twopi * o->ud();
		z1 = c * sin(z2);
		o->x.d = z2 = c * cos(z2);
		return o->m.d + o->s.d * z1;
	} else {
		/* return saved variate */
		c = o->x.d;
		o->x.d = NAN;
		return o->m.d + o->s.d * c;
	}
}

/*!
@brief	LRd_gaus_PDF(LR_obj *o, double x) - double gaussian/normal probablity distribution function

@param o	LR_obj object
@param x	value
@return double PDF at x
*/
double LRd_gaus_PDF(LR_obj *o, double x) {
	double	one = 1.0, half = .5,
		is = one / o->s.d,
		cc = half * M_2_SQRTPI * M_SQRT1_2 * is,
		xm = (x - o->m.d) * is,
		xx = xm * xm;

	return cc * exp(-half * xx);
}

/*!
@brief	LRd_gaus_CDF(LR_obj *o, double x) - double gaussian/normal cumulative distribution function

@param o	LR_obj object
@param x	value
@return double CDF at x
*/
double LRd_gaus_CDF(LR_obj *o, double x) {
	double	one = 1.0,
		half = 0.5,
		is = M_SQRT1_2 / o->s.d,
		xm = (x - o->m.d) * is;

	return half * (one + erf(xm));
}

/* float */
/*!
@brief	LRf_gausbm_RAN(LR_obj *o) - float random gaussian/normal distribution
using the Box-Muller method.
Default values: mean m = 0, std.deviation s = 1

@param o	LR_obj object
@return float
*/
float LRf_gausbm_RAN(LR_obj *o) {
	float one = 1.0, ntwo = -2.0, twopi = 2.0 * M_PI;
	float c, z1, z2;

	if (isnan(o->x.f)) {
		/* generate new pair - 1-U avoids possible overflow */
		c = sqrt(ntwo * log(one - o->uf()));
		z2 = twopi * o->uf();
		z1 = c * sinf(z2);
		o->x.f = z2 = c * cosf(z2);
		return o->m.f + o->s.f * z1;
	} else {
		/* return saved variate */
		c = o->x.f;
		o->x.f = NAN;
		return o->m.f + o->s.f * c;
	}
}

/*!
@brief	LRf_gaus_PDF(LR_obj *o, float x) - float gaussian/normal probablity distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRf_gaus_PDF(LR_obj *o, float x) {
	float	one = 1.0, half = .5,
		is = one / o->s.f,
		cc = half * M_2_SQRTPI * M_SQRT1_2 * is,
		xm = (x - o->m.f) * is,
		xx = xm * xm;

	return cc * expf(-half * xx);
}

/*!
@brief	LRf_gaus_CDF(LR_obj *o, float x) - float gaussian/normal cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRf_gaus_CDF(LR_obj *o, float x) {
	float	one = 1.0,
		half = 0.5,
		is = M_SQRT1_2 / o->s.f,
		xm = (x - o->m.f) * is;

	return half * (one + erff(xm));
}

