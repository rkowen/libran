/*!
\file	LRgsn.c
\brief 	The gaussian-like distributions on the [a,b) interval

The pseudo-random numbers are distributed from
some lower bound "a" upto "b" (on the interval [a,b) ).
The medium m = (a+b)/2, and width s = (b - a)/2

The default depends on the distribution.
 
*/
#ifdef __cplusplus
extern "C" {
#endif

#include "libran.h"

/*!
@brief	LRd_gsn2_RAN(LR_obj *o) - double random g2 gaussian-like (saw tooth)
distribution

Default range: [a,b) = [-1,1)

@param o	LR_obj object
@return double
*/
double LRd_gsn2_RAN(LR_obj *o) {
	double half = .5;
	double sa = half * (o->b.d - o->a.d);
	return o->a.d + sa * (o->ud(o) + o->ud(o));
}

/*!
@brief	LRd_gsn2_PDF(LR_obj *o, double x) - double gaussian-like (saw tooth)
probablity distribution function

@param o	LR_obj object
@param x	value
@return double PDF at x
*/
double LRd_gsn2_PDF(LR_obj *o, double x) {
	double	zero = 0.0, one = 1.0, two = 2.0, half = .5;
	double	mm = half*(o->b.d - o->a.d),
		sa = one/mm,
		xx = sa * (x - o->a.d);

	if (xx <= zero || xx >= two) {
		return zero;
	} else if (xx <= one) {
		return sa * xx;
	} else {
		return sa * (two - xx);
	}
}

/*!
@brief	LRd_gsn2_CDF(LR_obj *o, double x) - double gaussian-like (saw tooth)
cumulative distribution function

@param o	LR_obj object
@param x	value
@return double CDF at x
*/
double LRd_gsn2_CDF(LR_obj *o, double x) {
	double	zero = 0.0, one = 1.0, two = 2.0, half = .5;
	double	mm = half*(o->b.d - o->a.d),
		sa = one/mm,
		xx = sa * (x - o->a.d);

	if (xx <= zero) {
		return zero;
	} else if (xx >= two) {
		return one;
	} else {
		if (xx < one) {
			return half * xx * xx;
		} else {
			return xx * (two - half * xx) - one;
		}
	}
}

/*!
@brief	LRf_gsn2_RAN(LR_obj *o) - float random g2 gaussian-like (saw tooth)
distribution

Default range: [a,b) = [-1,1)

@param o	LR_obj object
@return float
*/
float LRf_gsn2_RAN(LR_obj *o) {
	float half = .5;
	float sa = half * (o->b.f - o->a.f);
	return o->a.f + sa * (o->uf(o) + o->uf(o));
}

/*!
@brief	LRf_gsn2_PDF(LR_obj *o, float x) - float gaussian-like (saw tooth)
probablity distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRf_gsn2_PDF(LR_obj *o, float x) {
	float	zero = 0.0, one = 1.0, two = 2.0, half = .5;
	float	mm = half*(o->b.f - o->a.f),
		sa = one/mm,
		xx = sa * (x - o->a.f);

	if (xx <= zero || xx >= two) {
		return zero;
	} else if (xx <= one) {
		return sa * xx;
	} else {
		return sa * (two - xx);
	}
}

/*!
@brief	LRf_gsn2_CDF(LR_obj *o, float x) - float gaussian-like (saw tooth)
cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRf_gsn2_CDF(LR_obj *o, float x) {
	float	zero = 0.0, one = 1.0, two = 2.0, half = .5;
	float	mm = half*(o->b.f - o->a.f),
		sa = one/mm,
		xx = sa * (x - o->a.f);

	if (xx <= zero) {
		return zero;
	} else if (xx >= two) {
		return one;
	} else {
		if (xx < one) {
			return half * xx * xx;
		} else {
			return xx * (two - half * xx) - one;
		}
	}
}

#ifdef __cplusplus
}
#endif
