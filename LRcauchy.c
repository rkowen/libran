/*!
\file	LRcauchy.c
\brief 	The Cauchy (or Lorentz) distribution centered on m and width given by 2s

The pseudo-random numbers are distributed from a Cauchy or Lorentz
distribution.  It's a pathological distribution where the mean and variance
is undefined, but the distribution has a clear peak and width (given by m
and 2s respectively).

PDF(z) = 1/[s*pi * {1 + (z-m)^2 / s)}]
CDF(z) = 1/pi*arctan((x-m)/s) + 1/2

The default is m = 0, s = 1.
 
*/
#include <math.h>
#include "libran.h"

/* double */
/*!
@brief	LRd_cauchy_RAN(LR_obj *o) - double random Cauchy/Lorentz distribution
using the inversion method.
Default values: peak m = 0, half width s = 1

@param o        LR_obj object
@return double
*/
double LRd_cauchy_RAN(LR_obj *o) {
	return o->m.d + o->s.d*tan(M_PI * o->ud(o));
}

/*!
@brief	LRd_cauchymar_RAN(LR_obj *o) - double random Cauchy/Lorentz distribution
using the polar method and the Marsagalia method and acceptance/rejection for generating a
Gaussian/Normal variate.
Default values: peak m = 0, half width s = 1

@param o        LR_obj object
@return double
*/
double LRd_cauchymar_RAN(LR_obj *o) {
	double zero = 0.0, one = 1.0, two = 2.0;
	double s, z1, z2;
	do {
		z1 = two*o->ud(o) - one;
		z2 = two*o->ud(o) - one;
		s = z1*z1 + z2*z2;
	} while (s > one || z2 == zero);

	return o->m.d + o->s.d*z1/z2;
}

/*!
@brief	LRd_cauchy_PDF(LR_obj *o, double x) - double Cauchy/Lorentz probablity distribution function

@param o	LR_obj object
@param x	value
@return double PDF at x
*/
double LRd_cauchy_PDF(LR_obj *o, double x) {

	x -= o->m.d;

	return o->s.d * M_1_PI / (x*x + o->s.d * o->s.d);
}

/*!
@brief	LRd_cauchy_CDF(LR_obj *o, double x) - double Cauchy/Lorentz cumulative distribution function

@param o	LR_obj object
@param x	value
@return double CDF at x
*/
double LRd_cauchy_CDF(LR_obj *o, double x) {
	double	half = 0.5;

	x -= o->m.d;

	return half + M_1_PI * atan(x/o->s.d);
}

/* float */
/*!
@brief	LRf_cauchy_RAN(LR_obj *o) - float random Cauchy/Lorentz distribution
using the inversion method.
Default values: peak m = 0, half width s = 1

@param o        LR_obj object
@return float
*/
float LRf_cauchy_RAN(LR_obj *o) {
	return o->m.f + o->s.f*tanf(M_PI * o->uf(o));
}

/*!
@brief	LRf_cauchymar_RAN(LR_obj *o) - float random Cauchy/Lorentz distribution
using the polar method and the Marsagalia method and acceptance/rejection for generating a
Gaussian/Normal variate.
Default values: peak m = 0, half width s = 1

@param o        LR_obj object
@return float
*/
float LRf_cauchymar_RAN(LR_obj *o) {
	float zero = 0.0, one = 1.0, two = 2.0;
	float s, z1, z2;
	do {
		z1 = two*o->uf(o) - one;
		z2 = two*o->uf(o) - one;
		s = z1*z1 + z2*z2;
	} while (s > one || z2 == zero);

	return o->m.f + o->s.f*z1/z2;
}

/*!
@brief	LRf_cauchy_PDF(LR_obj *o, float x) - float Cauchy/Lorentz probablity distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRf_cauchy_PDF(LR_obj *o, float x) {

	x -= o->m.f;

	return o->s.f * M_1_PI / (x*x + o->s.f * o->s.f);
}

/*!
@brief	LRf_cauchy_CDF(LR_obj *o, float x) - float Cauchy/Lorentz cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRf_cauchy_CDF(LR_obj *o, float x) {
	float	half = 0.5;

	x -= o->m.f;

	return half + M_1_PI * atanf(x/o->s.f);
}

