/*!
\file	LRgaus.c
\brief 	The Gaussian (Normal) distributions centered on m and width given by 2s

The pseudo-random numbers are distributed from a Gaussian or Normal
(also Laplace-Gauss) distribution.
The mean is given by \e m and variance = \f$ s^2 \f$.
The normal distribution is a very common continuous probability distribution
and is commonly called the \e bell curve.

\manonly
PDF(z) = 1/(s*sqrt(2pi)) exp(-(z-m)^2 / (2s^2))
CDF(z) = 1/2*[1 + erf((x-m)/(s*sqrt(2))]
\endmanonly

\f{eqnarray*}{
\mbox{PDF}(x)
	&= \frac{1}{\sqrt{2\pi s^2}} \exp{-\frac{(x - m)^2}{2 s^2}}	\\
\mbox{CDF}(x)
	&= \frac{1}{2}
	\left[ 1 + \mbox{erf}\left(\frac{x - m}{s\sqrt{2}}\right) \right]
\f}

The default is m = 0, s = 1; which is called the
<b>standard normal distribution</b>.

The central limit theorem states that the sum of a large number of identically
distributed independent random variates will have an approximately
normal distribution.
 
Gaussian distributed random variates can be generated in a couple of
methods.  The first is
the Box-Muller method which generates a pair of Gaussian distributed
random variates by considering the 2-dimensional form of the Gaussian
distribution.  The polar form can yield itself to using the inverse CDF
for generating the random variates.  However, this requires using
time-intensive math functions such as the sin, cos, log, and sqrt functions.

The second method uses the Marsagalia's polar method with acceptance/rejection
for generating random variates confined to the unit circle centered on
the origin with radius 1.
This gives an acceptance ratio to be \f$\frac{\pi}{4} \approx 78.5\% \f$, but
the method only uses a single sqrt, log, and division per generated pair.
This simplicity may result in a faster generator despite the time to
calculate rejected samples.

*/
#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include "libran.h"

/* double */
/*!
@brief	LRd_gausbm_RAN(LR_obj *o) - double random Gaussian/Normal distribution
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
		c = sqrt(ntwo * log(one - o->ud(o)));
		z2 = twopi * o->ud(o);
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

/* double */
/*!
@brief	LRd_gausmar_RAN(LR_obj *o) - double random Gaussian/Normal distribution
using the Marsaglia method with acceptance-rejection.

Default values: mean m = 0, std.deviation s = 1

@param o        LR_obj object
@return double
*/
double LRd_gausmar_RAN(LR_obj *o) {
	double one = 1.0, two = 2.0;
	double s, z1, z2;

	if (isnan(o->x.d)) {
		do {
			z1 = two*o->ud(o) - one;
			z2 = two*o->ud(o) - one;
			s = z1*z1 + z2*z2;
		} while (s > one);
		s = sqrt(-two*log(s)/s);
		o->x.d = z2 * s;
		return o->m.d + o->s.d * z1 * s;
	} else {
		/* return saved variate */
		s = o->x.d;
		o->x.d = NAN;
		return o->m.d + o->s.d * s;
	}
}

/*!
@brief	LRd_gaus_PDF(LR_obj *o, double x) - double Gaussian/Normal probablity distribution function

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
@brief	LRd_gaus_CDF(LR_obj *o, double x) - double Gaussian/Normal cumulative distribution function

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
@brief	LRf_gausbm_RAN(LR_obj *o) - float random Gaussian/Normal distribution
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
		c = sqrt(ntwo * log(one - o->uf(o)));
		z2 = twopi * o->uf(o);
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
@brief	LRf_gausmar_RAN(LR_obj *o) - float random Gaussian/Normal distribution
using the Marsaglia method with acceptance-rejection.

Default values: mean m = 0, std.deviation s = 1

@param o        LR_obj object
@return float
*/
float LRf_gausmar_RAN(LR_obj *o) {
	float one = 1.0, two = 2.0;
	float s, z1, z2;

	if (isnan(o->x.f)) {
		do {
			z1 = two*o->ud(o) - one;
			z2 = two*o->ud(o) - one;
			s = z1*z1 + z2*z2;
		} while (s > one);
		s = sqrtf(-two*logf(s)/s);
		o->x.f = z2 * s;
		return o->m.f + o->s.f * z1 * s;
	} else {
		/* return saved variate */
		s = o->x.f;
		o->x.f = NAN;
		return o->m.f + o->s.f * s;
	}
}

/*!
@brief	LRf_gaus_PDF(LR_obj *o, float x) - float Gaussian/Normal probablity distribution function

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
@brief	LRf_gaus_CDF(LR_obj *o, float x) - float Gaussian/Normal cumulative distribution function

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

#ifdef __cplusplus
}
#endif
