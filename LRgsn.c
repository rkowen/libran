/*!
\file	LRgsn.c
\brief 	The gaussian-like distributions based on the Central Limit Theorem

Adding two random variates will produce a new CDF which is the convolution
of the two individual respective PDFs.

Here we sum some number of uniformly distributed random variates
where the uniform random variate has:

\manonly
   PDF	u(x) =	{ 0 if x < 0 || x >= 1
		{ 1 if 0 <= x < 1

   CDF	U(x) =	{ 0 if x < 0
		{ x if 0 <= x < 1
		{ 1 if 1 <= x
\endmanonly

\f{eqnarray*}{
\mbox{PDF } u(x)	&=
	\left\{ \begin{array}{ll}
		0, &	x < 0 \mbox{ or } x \ge 1, \\
		1, &	0 \le x < 1 .
	\end{array} \right.
\\
\mbox{CDF } U(x)	&=
	\left\{ \begin{array}{ll}
		0, &	x < 0 ,			\\
		x, &	0 \le x < 1 ,		\\
		1, &	x \ge 1 .
	\end{array} \right.
\f}

A more detailed derivation can be
found at page 22 of "Non-Uniform Random Variate Generation" by Luc Devroye.

The CDF of \f$ n \f$ uniform random variates summed together is

\manonly
F(x)=1/n! [H(x)^n - (n 1)H(x-1)^n + (n 2)H(x-2)^n - ... ]

where H(x) is the positive part of x,
	H(x) =	{0 if x < 0
		{x if x >= 0

and (n m) = n!/(m!*(n-m)!) (binomial factor)

\endmanonly

\f[
F(x) = \frac{1}{n!} \left[H(x)^n
	- \left(\begin{array}{c} {n} \\ {1} \end{array}\right) H(x-1)^n
	+ \left(\begin{array}{c} {n} \\ {2} \end{array}\right) H(x-2)^n
	- \cdots \right]
\f]
\f[
\mbox{where }
H(x) = \left\{ \begin{array}{ll}
		0, &	x < 0	\\
		x, &	0 \le x .
	\end{array} \right.
\f]
\f[
\mbox{and the combination or binomial factor }
\left(\begin{array}{c} {n} \\ {m} \end{array}\right)
= \frac{n!}{m!(n-m)!}
\f]

The set of distributions here are by default
symmetric and centered on \f$ x = 0 \f$ and
on an interval \f$ -n/2 \le x \le n/2 \f$, where \f$ n \f$ is
the order or number of summed random variates.

The pseudo-random numbers are distributed from
some lower bound "a" upto "b" (on the interval [a,b) ).
The medium m = (a+b)/2, and width s = (b - a)/2

The default depends on the distribution.

\see aux/gsn.c

GSN2
====

The \f$ n=2 \f$ Gaussian-like random variate is just the sum of two
uniformly distributed random variates.  The resulting PDF is also just a
saw tooth in form.

The default values are \f$ a = -1 \f$, and \f$ b = 1 \f$.


\f{eqnarray*}{
\mbox{PDF  gsn2}(x)	&=
	\left\{ \begin{array}{ll}
		0,	&	x < -1 \mbox{ or } x \ge 1 ,	\\
		1 + x,	&	-1 \le x < 0 ,			\\
		1 - x,	&	0 \le x < 1 .
	\end{array} \right.
\\
\mbox{CDF GSN2}(x)	&=
	\left\{ \begin{array}{ll}
		0,			&	x < -1 ,		\\
		\frac{1}{2}(x+1)^2,	&	-1 \le x < 0 ,		\\
		-\frac{1}{2}x^2 + x + \frac{1}{2}, &	0 \le x < 1 ,	\\
		1, &	x \ge 1 .
	\end{array} \right.
\f}
 
GSN4
====

The \f$ n=4 \f$ Gaussian-like random variate is just the sum of four
uniformly distributed random variates.  However, the resulting PDF
exhibits the behavior of a bell curve with each segment comprised of
cubic polynomials and the end points of each segment match the adjacent
segments endpoints
and are continuous through the second derivative ... just like cubic
splines.

The CDF are quartic polynomials where the adjacent segments are continuous
through the third derivative.
 
The default values are \f$ a = -2 \f$, and \f$ b = 2 \f$.

GSN12
=====

The \f$ n=12 \f$ Gaussian-like random variate is of special note.
It's the sum of twelve uniformly distributed random variates.
This distribution closely approximates the Gaussian or Normal
distribution.  The variance of the distribution is equal to 1 just like
the standard normal distribution, but has definite upper and lower bounds.

The PDF are segments of 11th order polynomials.
The CDF are segments of 12th order polynomials.
Each of the distribution functions are continuous upto the highest
derivative.  These distribution functions \e exactly represent the
\e gsn12 random variate distribution and are not approximations.

The default values are \f$ m = 0 \f$  and \f$ s = 1 \f$.
Note that this distribution uses the \e mean and \e deviation to identify
the distribution characteristics.  It is understood that the lower
bound is \f$ m - s \f$ and the upper bound is \f$ m + s \f$.

\see LRgaus.c

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

int gscdfn4[5][5] = {
  {0,0,0,0,1},
  {1,1,1,1,-1},
  {1,2,0,-1,1},
  {23,1,-1,1,-1},
  {1,0,0,0,0}
};

int gscdfd4[5][5] = {
  {1,1,1,1,24},
  {24,6,4,6,8},
  {2,3,1,3,8},
  {24,6,4,6,24},
  {1,1,1,1,1}
};

int gspdfn4[5][5] = {
  {0,0,0,1,0},
  {1,1,1,-1,0},
  {2,0,-1,1,0},
  {1,-1,1,-1,0},
  {0,0,0,0,0}
};

int gspdfd4[5][5] = {
  {1,1,1,6,1},
  {6,2,2,2,1},
  {3,1,1,2,1},
  {6,2,2,6,1},
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
		int i = xx + two;		/* truncate */
		double xt = xx + two - i;	/* translate to [0-1] */
		double r = gspdf4d[i][4];
		/* horners rule to evaluate polynomial */
		for (int j = 3; j >= 0; j--) {
			r = (gspdf4d[i][j] + xt*r);
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
		int i = xx + two;		/* truncate */
		double xt = xx + two - i;	/* translate to [0-1] */
		double r = gscdf4d[i][4];
		/* horners rule to evaluate polynomial */
		for (int j = 3; j >= 0; j--) {
			r = (gscdf4d[i][j] + xt*r);
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
		int i = xx + two;		/* truncate */
		float xt = xx + two - i;	/* translate to [0-1] */
		float r = gspdf4f[i][4];
		/* horners rule to evaluate polynomial */
		for (int j = 3; j >= 0; j--) {
			r = (gspdf4f[i][j] + xt*r);
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
		int i = xx + two;		/* truncate */
		float xt = xx + two - i;	/* translate to [0-1] */
		float r = gscdf4f[i][4];
		/* horners rule to evaluate polynomial */
		for (int j = 3; j >= 0; j--) {
			r = (gscdf4f[i][j] + xt*r);
		}
		return r;
	}
}

/* gsn12 - data  [ */
long gscdfn12[13][13] = {
  {0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,1l},
  {1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,-1l},
  {1021l,509l,253l,25l,61l,29l,13l,1l,1l,-1l,-1l,-1l,1l},
  {397l,50879l,5203l,907l,79l,239l,1l,-1l,-1l,-1l,1l,1l,-1l},
  {29639l,1093l,16973l,31l,29l,-17l,-1l,0l,1l,1l,-1l,-1l,1l},
  {12831419l,1623019l,6787l,-1l,-443l,-43l,167l,11l,-1l,-1l,1l,1l,-1l},
  {1l,655177l,0l,-809l,0l,31l,0l,-23l,0l,1l,0l,-1l,1l},
  {67002181l,1623019l,-6787l,-1l,443l,-43l,-167l,11l,1l,-1l,-1l,1l,-1l},
  {1300921l,1093l,-16973l,31l,-29l,-17l,1l,0l,-1l,1l,1l,-1l,1l},
  {393843l,50879l,-5203l,907l,-79l,239l,-1l,-1l,1l,-1l,-1l,1l,-1l},
  {119749379l,509l,-253l,25l,-61l,29l,-13l,1l,-1l,-1l,1l,-1l,1l},
  {479001599l,1l,-1l,1l,-1l,1l,-1l,1l,-1l,1l,-1l,1l,-1l},
  {1l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l}
};
/* integer type must support max value = 479001599 */

long gscdfd12[13][13] = {
  {1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,479001600l},
  {479001600l,39916800l,7257600l,2177280l,967680l,604800l,518400l,604800l,967680l,2177280l,7257600l,39916800l,43545600l},
  {119750400l,9979200l,1814400l,108864l,241920l,151200l,129600l,30240l,241920l,544320l,907200l,3991680l,8709120l},
  {394240l,13305600l,806400l,145152l,21504l,201600l,19200l,8064l,21504l,725760l,268800l,887040l,2903040l},
  {1330560l,19800l,302400l,1134l,8064l,6300l,800l,1l,8064l,45360l,151200l,332640l,1451520l},
  {79833600l,6652800l,57600l,384l,23040l,14400l,86400l,20160l,7680l,17280l,172800l,190080l,1036800l},
  {2l,1663200l,1l,12960l,1l,3600l,1l,25200l,1l,12960l,1l,158400l,1036800l},
  {79833600l,6652800l,57600l,384l,23040l,14400l,86400l,20160l,7680l,17280l,172800l,190080l,1451520l},
  {1330560l,19800l,302400l,1134l,8064l,6300l,800l,1l,8064l,45360l,151200l,332640l,2903040l},
  {394240l,13305600l,806400l,145152l,21504l,201600l,19200l,8064l,21504l,725760l,268800l,887040l,8709120l},
  {119750400l,9979200l,1814400l,108864l,241920l,151200l,129600l,30240l,241920l,544320l,907200l,3991680l,43545600l},
  {479001600l,39916800l,7257600l,2177280l,967680l,604800l,518400l,604800l,967680l,2177280l,7257600l,39916800l,479001600l},
  {1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,1l}
};
/* integer type must support max value = 479001600 */

long gspdfn12[13][13] = {
  {0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,1l,0l},
  {1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,-1l,0l},
  {509l,253l,25l,61l,29l,13l,1l,1l,-1l,-1l,-1l,1l,0l},
  {50879l,5203l,907l,79l,239l,1l,-1l,-1l,-1l,1l,1l,-1l,0l},
  {1093l,16973l,31l,29l,-17l,-3l,0l,1l,1l,-1l,-1l,1l,0l},
  {1623019l,6787l,-1l,-443l,-43l,167l,11l,-1l,-1l,1l,1l,-1l,0l},
  {655177l,0l,-809l,0l,31l,0l,-23l,0l,1l,0l,-1l,1l,0l},
  {1623019l,-6787l,-1l,443l,-43l,-167l,11l,1l,-1l,-1l,1l,-1l,0l},
  {1093l,-16973l,31l,-29l,-17l,3l,0l,-1l,1l,1l,-1l,1l,0l},
  {50879l,-5203l,907l,-79l,239l,-1l,-1l,1l,-1l,-1l,1l,-1l,0l},
  {509l,-253l,25l,-61l,29l,-13l,1l,-1l,-1l,1l,-1l,1l,0l},
  {1l,-1l,1l,-1l,1l,-1l,1l,-1l,1l,-1l,1l,-1l,0l},
  {0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l}
};
/* integer type must support max value = 1623019 */

long gspdfd12[13][13] = {
  {1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,39916800l,1l},
  {39916800l,3628800l,725760l,241920l,120960l,86400l,86400l,120960l,241920l,725760l,3628800l,3628800l,1l},
  {9979200l,907200l,36288l,60480l,30240l,21600l,4320l,30240l,60480l,90720l,362880l,725760l,1l},
  {13305600l,403200l,48384l,5376l,40320l,3200l,1152l,2688l,80640l,26880l,80640l,241920l,1l},
  {19800l,151200l,378l,2016l,1260l,400l,1l,1008l,5040l,15120l,30240l,120960l,1l},
  {6652800l,28800l,128l,5760l,2880l,14400l,2880l,960l,1920l,17280l,17280l,86400l,1l},
  {1663200l,1l,4320l,1l,720l,1l,3600l,1l,1440l,1l,14400l,86400l,1l},
  {6652800l,28800l,128l,5760l,2880l,14400l,2880l,960l,1920l,17280l,17280l,120960l,1l},
  {19800l,151200l,378l,2016l,1260l,400l,1l,1008l,5040l,15120l,30240l,241920l,1l},
  {13305600l,403200l,48384l,5376l,40320l,3200l,1152l,2688l,80640l,26880l,80640l,725760l,1l},
  {9979200l,907200l,36288l,60480l,30240l,21600l,4320l,30240l,60480l,90720l,362880l,3628800l,1l},
  {39916800l,3628800l,725760l,241920l,120960l,86400l,86400l,120960l,241920l,725760l,3628800l,39916800l,1l},
  {1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,1l,1l}
};
/* integer type must support max value = 39916800 */


double gspdf12d[13][13] = {
	NAN
};

double gscdf12d[13][13] = {
	NAN
};

float gspdf12f[13][13] = {
	NAN
};

float gscdf12f[13][13] = {
	NAN
};

/* ] set up coefs from rational integers */
int gs12initd(void) {
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			gspdf12d[i][j] = ((double) gspdfn12[i][j])
				/ ((double) gspdfd12[i][j]);
			gscdf12d[i][j] = ((double) gscdfn12[i][j])
				/ ((double) gscdfd12[i][j]);
		}
	}
}

int gs12initf(void) {
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			gspdf12f[i][j] = ((float) gspdfn12[i][j])
				/ ((float) gspdfd12[i][j]);
			gscdf12f[i][j] = ((float) gscdfn12[i][j])
				/ ((float) gscdfd12[i][j]);
		}
	}
}

/*!
@brief	LRd_gsn12_RAN(LR_obj *o) - double random g12 gaussian-like
distribution closely matches a Gaussian/Normal distribution
using the sum of 12 uniform random variates.

Default: mean m = 0, std.deviation s = 1

@param o	LR_obj object
@return double
*/
double LRd_gsn12_RAN(LR_obj *o) {
	double six = 6.0;

	/* unroll "loop" */
	return o->m.d + o->s.d * (
		o->ud(o) + o->ud(o) + o->ud(o) + o->ud(o)
	+	o->ud(o) + o->ud(o) + o->ud(o) + o->ud(o)
	+	o->ud(o) + o->ud(o) + o->ud(o) + o->ud(o)
	-	six
	);
}

/*!
@brief	LRd_gsn12_PDF(LR_obj *o, double x) - double gaussian-like
(simple bell curve) probablity distribution function

@param o	LR_obj object
@param x	value
@return double PDF at x
*/
double LRd_gsn12_PDF(LR_obj *o, double x) {
	double	zero = 0.0, one = 1.0, six = 6.0;
	double	sa = one/o->s.d,
		xx = sa * (x - o->m.d);

	if (isnan(gspdf12d[0][0]))	gs12initd();

	if (xx <= -six || xx >= six) {
		return zero;
	} else {
		int i = xx + six;		/* truncate */
		double xt = xx + six - i;	/* translate to [0-1] */
		double r = gspdf12d[i][12];
		/* horners rule to evaluate polynomial */
		for (int j = 11; j >= 0; j--) {
			r = (gspdf12d[i][j] + xt*r);
		}
		return sa * r;
	}
}

/*!
@brief	LRd_gsn12_CDF(LR_obj *o, double x) - double gaussian-like
(simple bell curve) cumulative distribution function

@param o	LR_obj object
@param x	value
@return double CDF at x
*/
double LRd_gsn12_CDF(LR_obj *o, double x) {
	double	zero = 0.0, one = 1.0, six = 6.0;
	double	sa = one/o->s.d,
		xx = sa * (x - o->m.d);

	if (isnan(gscdf12d[0][0]))	gs12initd();

	if (xx <= -six) {
		return zero;
	} else if (xx >= six) {
		return one;
	} else {
		int i = xx + six;		/* truncate */
		double xt = xx + six - i;	/* translate to [0-1] */
		double r = gscdf12d[i][12];
		/* horners rule to evaluate polynomial */
		for (int j = 11; j >= 0; j--) {
			r = (gscdf12d[i][j] + xt*r);
		}
		return r;
	}
}

/*!
@brief	LRf_gsn12_RAN(LR_obj *o) - float random g12 gaussian-like
distribution closely matches a Gaussian/Normal distribution
using the sum of 12 uniform random variates.

Default: mean m = 0, std.deviation s = 1

@param o	LR_obj object
@return float
*/
float LRf_gsn12_RAN(LR_obj *o) {
	float six = 6.0;

	/* unroll "loop" */
	return o->m.f + o->s.f * (
		o->uf(o) + o->uf(o) + o->uf(o) + o->uf(o)
	+	o->uf(o) + o->uf(o) + o->uf(o) + o->uf(o)
	+	o->uf(o) + o->uf(o) + o->uf(o) + o->uf(o)
	-	six
	);
}

/*!
@brief	LRf_gsn12_PDF(LR_obj *o, float x) - float gaussian-like
(simple bell curve) probablity distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRf_gsn12_PDF(LR_obj *o, float x) {
	float	zero = 0.0, one = 1.0, six = 6.0;
	float	sa = one/o->s.f,
		xx = sa * (x - o->m.f);

	if (isnan(gspdf12f[0][0]))	gs12initf();

	if (xx <= -six || xx >= six) {
		return zero;
	} else {
		int i = xx + six;		/* truncate */
		float xt = xx + six - i;	/* translate to [0-1] */
		float r = gspdf12f[i][12];
		/* horners rule to evaluate polynomial */
		for (int j = 11; j >= 0; j--) {
			r = (gspdf12f[i][j] + xt*r);
		}
		return sa * r;
	}
}

/*!
@brief	LRf_gsn12_CDF(LR_obj *o, float x) - float gaussian-like
(simple bell curve) cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRf_gsn12_CDF(LR_obj *o, float x) {
	float	zero = 0.0, one = 1.0, six = 6.0;
	float	sa = one/o->s.f,
		xx = sa * (x - o->m.f);

	if (isnan(gspdf12f[0][0]))	gs12initf();

	if (xx <= -six) {
		return zero;
	} else if (xx >= six) {
		return one;
	} else {
		int i = xx + six;		/* truncate */
		float xt = xx + six - i;	/* translate to [0-1] */
		float r = gscdf12f[i][12];
		/* horners rule to evaluate polynomial */
		for (int j = 11; j >= 0; j--) {
			r = (gscdf12f[i][j] + xt*r);
		}
		return r;
	}
}

#ifdef __cplusplus
}
#endif
