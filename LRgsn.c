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

#include <math.h>
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

/* gsn4 - data */

long gscdfn4[5][5] = {
	{2,4,1,1,1},
	{1,2,0,-1,-1},
	{1,2,0,-1,1},
	{1,4,-1,1,-1},
	{1,0,0,0,0}
};

long gscdfd4[5][5] = {
	{3,3,1,3,24},
	{2,3,1,3,8},
	{2,3,1,3,8},
	{3,3,1,3,24},
	{1,1,1,1,1}
};

long gspdfn4[5][5] = {
	{4,2,1,1,0},
	{2,0,-1,-1,0},
	{2,0,-1,1,0},
	{4,-2,1,-1,0},
	{0,0,0,0,0}
};

long gspdfd4[5][5] = {
	{3,1,1,6,1},
	{3,1,1,2,1},
	{3,1,1,2,1},
	{3,1,1,6,1},
	{1,1,1,1,1}
};


double gspdf4d[5][5] = {
	NAN
};

double gscdf4d[5][5] = {
	NAN
};

float gspdf4f[5][5] = {
	NAN
};

float gscdf4f[5][5] = {
	NAN
};

/* set up coefs from rational integers */
int gs4initd(void) {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			gspdf4d[i][j] = ((double) gspdfn4[i][j])
				/ ((double) gspdfd4[i][j]);
			gscdf4d[i][j] = ((double) gscdfn4[i][j])
				/ ((double) gscdfd4[i][j]);
		}
	}
}

int gs4initf(void) {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			gspdf4f[i][j] = ((float) gspdfn4[i][j])
				/ ((float) gspdfd4[i][j]);
			gscdf4f[i][j] = ((float) gscdfn4[i][j])
				/ ((float) gscdfd4[i][j]);
		}
	}
}

/*!
@brief	LRd_gsn4_RAN(LR_obj *o) - double random g4 gaussian-like
(simple bell curve) distribution

Default range: [a,b) = [-2,2)

@param o	LR_obj object
@return double
*/
double LRd_gsn4_RAN(LR_obj *o) {
	double fourth = .25;
	double sa = fourth * (o->b.d - o->a.d);

	return o->a.d + sa * (o->ud(o) + o->ud(o) + o->ud(o) + o->ud(o));
}

/*!
@brief	LRd_gsn4_PDF(LR_obj *o, double x) - double gaussian-like
(simple bell curve) probablity distribution function

@param o	LR_obj object
@param x	value
@return double PDF at x
*/
double LRd_gsn4_PDF(LR_obj *o, double x) {
	double	zero = 0.0, one = 1.0, two = 2.0, four = 4.0;
	double	mm = o->b.d - o->a.d,
		sa = four/mm,
		xx = sa * (x - o->a.d) - two;

	if (isnan(gspdf4d[0][0]))	gs4initd();

	if (xx <= -two || xx >= two) {
		return zero;
	} else {
		int i = xx + two;	/* truncate */
		double r = gspdf4d[i][4];
		/* horners rule to evaluate polynomial */
		for (int j = 3; j >= 0; j--) {
			r = (gspdf4d[i][j] + xx*r);
		}
		return sa * r;
	}
}

/*!
@brief	LRd_gsn4_CDF(LR_obj *o, double x) - double gaussian-like (saw tooth)
cumulative distribution function

@param o	LR_obj object
@param x	value
@return double CDF at x
*/
double LRd_gsn4_CDF(LR_obj *o, double x) {
	double	zero = 0.0, one = 1.0, two = 2.0, four = 4.0;
	double	mm = o->b.d - o->a.d,
		sa = four/mm,
		xx = sa * (x - o->a.d) - two;

	if (isnan(gscdf4d[0][0]))	gs4initd();

	if (xx <= -two) {
		return zero;
	} else if (xx >= two) {
		return one;
	} else {
		int i = xx + two;	/* truncate */
		double r = gscdf4d[i][4];
		/* horners rule to evaluate polynomial */
		for (int j = 3; j >= 0; j--) {
			r = (gscdf4d[i][j] + xx*r);
		}
		return r;
	}
}

/*!
@brief	LRf_gsn4_RAN(LR_obj *o) - float random g4 gaussian-like
(simple bell curve) distribution

Default range: [a,b) = [-2,2)

@param o	LR_obj object
@return float
*/
float LRf_gsn4_RAN(LR_obj *o) {
	float fourth = .25;
	float sa = fourth * (o->b.f - o->a.f);

	return o->a.f + sa * (o->uf(o) + o->uf(o) + o->uf(o) + o->uf(o));
}

/*!
@brief	LRf_gsn4_PDF(LR_obj *o, float x) - float gaussian-like
(simple bell curve) probablity distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRf_gsn4_PDF(LR_obj *o, float x) {
	float	zero = 0.0, one = 1.0, two = 2.0, four = 4.0;
	float	mm = o->b.f - o->a.f,
		sa = four/mm,
		xx = sa * (x - o->a.f) - two;

	if (isnan(gspdf4f[0][0]))	gs4initf();

	if (xx <= -two || xx >= two) {
		return zero;
	} else {
		int i = xx + two;	/* truncate */
		float r = gspdf4f[i][4];
		/* horners rule to evaluate polynomial */
		for (int j = 3; j >= 0; j--) {
			r = (gspdf4f[i][j] + xx*r);
		}
		return sa * r;
	}
}

/*!
@brief	LRf_gsn4_CDF(LR_obj *o, float x) - float gaussian-like (saw tooth)
cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRf_gsn4_CDF(LR_obj *o, float x) {
	float	zero = 0.0, one = 1.0, two = 2.0, four = 4.0;
	float	mm = o->b.f - o->a.f,
		sa = four/mm,
		xx = sa * (x - o->a.f) - two;

	if (isnan(gspdf4f[0][0]))	gs4initf();

	if (xx <= -two) {
		return zero;
	} else if (xx >= two) {
		return one;
	} else {
		int i = xx + two;	/* truncate */
		float r = gscdf4f[i][4];
		/* horners rule to evaluate polynomial */
		for (int j = 3; j >= 0; j--) {
			r = (gscdf4f[i][j] + xx*r);
		}
		return r;
	}
}

#ifdef __cplusplus
}
#endif
