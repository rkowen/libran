/*!
\file	LRgsn.c
\brief 	The gaussian-like distributions on the [a,b) interval

The pseudo-random numbers are distributed from
some lower bound "a" upto "b" (on the interval [a,b) ).
The medium m = (a+b)/2, and width s = 1

The default depends on the distribution.
 
*/
#ifdef __cplusplus
extern "C" {
#endif

#include "libran.h"

/*!
@brief	LRd_gsn2_RAN(LR_obj *o) - double random g2 gaussian-like (saw tooth) distribution

Default range: [a,b) = [-1,1)

@param o	LR_obj object
@return double
*/
double LRd_gsn2_RAN(LR_obj *o) {
	double sa = .5 * (o->b.d - o->a.d);
	return o->a.d + sa * (o->ud(o) + o->ud(o));
}

/*!
@brief	LRd_gsn2_PDF(LR_obj *o, double x) - double gaussian-like (saw tooth)  probablity distribution function

@param o	LR_obj object
@param x	value
@return double PDF at x
*/
double LRd_gsn2_PDF(LR_obj *o, double x) {
	double	mm = (o->b.d - o->a.d)/2.0,
		sa = 1.0/mm,
		xx = sa * (x - o->a.d);

	if (xx <= 0.0 || xx >= 2.0) {
		return 0.0;
	} else if (xx <= 1.0) {
		return sa * xx;
	} else {
		return sa * (2.0 - xx);
	}
}

/*!
@brief	LRd_gsn2_CDF(LR_obj *o, double x) - double gaussian-like (saw tooth)  cumulative distribution function

@param o	LR_obj object
@param x	value
@return double CDF at x
*/
double LRd_gsn2_CDF(LR_obj *o, double x) {
	double	mm = (o->b.d - o->a.d)/2.0,
		sa = 1.0/mm,
		xx = sa * (x - o->a.d);

	if (xx <= 0.0) {
		return 0.0;
	} else if (xx >= 2.0) {
		return 1.0;
	} else {
		if (xx < 1.0) {
			return 0.5 * xx * xx;
		} else {
			return xx * (2.0 - 0.5 * xx) - 1.0;
		}
	}
}

/*!
@brief	LRf_gsn2_RAN(LR_obj *o) - float random g2 gaussian-like (saw tooth) distribution

Default range: [a,b) = [-1,1)

@param o	LR_obj object
@return float
*/
float LRf_gsn2_RAN(LR_obj *o) {
	float sa = .5 * (o->b.f - o->a.f);
	return o->a.f + sa * (o->ud(o) + o->ud(o));
}

/*!
@brief	LRf_gsn2_PDF(LR_obj *o, float x) - float gaussian-like (saw tooth)  probablity distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRf_gsn2_PDF(LR_obj *o, float x) {
	float	mm = (o->b.f - o->a.f)/2.0,
		sa = 1.0/mm,
		xx = sa * (x - o->a.f);

	if (xx <= 0.0 || xx >= 2.0) {
		return 0.0;
	} else if (xx <= 1.0) {
		return sa * xx;
	} else {
		return sa * (2.0 - xx);
	}
}

/*!
@brief	LRf_gsn2_CDF(LR_obj *o, float x) - float gaussian-like (saw tooth)  cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRf_gsn2_CDF(LR_obj *o, float x) {
	float	mm = (o->b.f - o->a.f)/2.0,
		sa = 1.0/mm,
		xx = sa * (x - o->a.f);

	if (xx <= 0.0) {
		return 0.0;
	} else if (xx >= 2.0) {
		return 1.0;
	} else {
		if (xx < 1.0) {
			return 0.5 * xx * xx;
		} else {
			return xx * (2.0 - 0.5 * xx) - 1.0;
		}
	}
}

#ifdef __cplusplus
}
#endif
