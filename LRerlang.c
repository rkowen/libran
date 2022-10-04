/*!
\file	LRerlang.c
\brief 	The Erlang distribution with shape \e k > 0 and scale \e m > 0.

The pseudo-random numbers are distributed from the Erlang
distribution.  It's only defined on interval \f$ x \ge 0 \f$ and zero otherwise.
This distribution typically represents the waiting time between \e k
occurrences of the events.
The number of events in a given amount time is described by the Poisson
distribution.
The attributes \e k is an integer greater than zero, and the scale \e m 
is greater than zero.

\manonly
   PDF(x) = x^(k-1) * exp(-x/m) / (m^k * (k-1)!)
   CDF(x) = 1 - exp(-x/m) * sum_(n=0)^(k-1) (x/m)^n
\endmanonly

\f{eqnarray*}{
\mbox{PDF}(x) &=
\left\{ \begin{array}{ll}
                0, &    x < 0 \\
        	\frac{x^{k-1} e^{-\frac{x}{m}}}{m^k (k-1)!}, &    0 \le x .
        \end{array} \right.
\\
\\
\mbox{CDF}(x) &=
\left\{ \begin{array}{ll}
	0, &    x < 0 \\
       	1 - e^{-\frac{x}{m}}\sum_{n=0}^{k-1} \frac{(\frac{x}{m})^n}{n!} , &
			0 \le x .
        \end{array} \right.
\f}


The default is \f$ m = 1 \f$ and \em s will be set to \f$ 1/m \f$
for calculation efficiency.
Do not set \e s when declaring this distribution.
The default for \e k = 1 , which is also the \e nexp distribution.

\see LRnexp.c
 
\image html ErlangDistribution.png
\image latex ErlangDistribution.eps "Erlang Distribution"
*/
/*
 * Copyright	2019	R.K. Owen, Ph.D.
 * License	see lgpl.md (Gnu Lesser General Public License)
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include "libran.h"

/* double */
/*!
@brief	LRd_erlang_RAN(LR_obj *o) - double random negative exponential
distribution using the inversion method.
Default values: scale m = 1.

@param o        LR_obj object
@return double
*/
double LRd_erlang_RAN(LR_obj *o) {
	double zero = 0.0, one = 1.0, u, p = one;
	int kk = o->k;

	while (kk--) {
		do {
			u = o->ud(o);
		} while (u == zero);
		p *= u;
	}
	return - o->m.d*log(p);
}

/*!
@brief	LRd_erlang_PDF(LR_obj *o, double x) - Erlang distribution
probablity distribution function

@param o	LR_obj object
@param x	value
@return double PDF at x
*/
double LRd_erlang_PDF(LR_obj *o, double x) {
	double zero = 0.0, one = 1.0, p = one, xx = x;
	if (isnan(o->s.d))
		o->s.d = one/o->m.d;

	if (x < zero)	return zero;

	if (x == zero) {
		if (o->k == 1) {
			return o->s.d;
		} else {
			return zero;
		}
	} else {
		p *= o->s.d;
		xx *= p;
		for (int nn = 1; nn < o->k; nn++) {
			p *= (xx/nn);
		}
		return p * exp(- o->s.d * x);
	}
}

/*!
@brief	LRd_erlang_CDF(LR_obj *o, double x) - Erlang distribution
cumulative distribution function

@param o	LR_obj object
@param x	value
@return double CDF at x
*/
double LRd_erlang_CDF(LR_obj *o, double x) {
	double zero = 0.0, one = 1.0, xx = x, p = one, s = one;
	if (isnan(o->s.d))
		o->s.d = one/o->m.d;

	if (x < zero)	return zero;

	xx *= o->s.d;
	for (int nn = 1; nn < o->k; nn++) {
		p *= (xx / nn);
		s += p;
	}
	return one - s * exp(- o->s.d * x);
}

/* float */
/*!
@brief	LRf_erlang_RAN(LR_obj *o) - float random negative exponential
distribution using the inversion method.
Default values: scale m = 1.

@param o        LR_obj object
@return float
*/
float LRf_erlang_RAN(LR_obj *o) {
	float zero = 0.0, one = 1.0, u, p = one;
	int kk = o->k;

	while (kk--) {
		do {
			u = o->uf(o);
		} while (u == zero);
		p *= u;
	}
	return - o->m.f*logf(p);
}

/*!
@brief	LRf_erlang_PDF(LR_obj *o, float x) - Erlang distribution
probablity distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRf_erlang_PDF(LR_obj *o, float x) {
	float zero = 0.0, one = 1.0, p = one, xx = x;
	if (isnan(o->s.f))
		o->s.f = one/o->m.f;

	if (x < zero)	return zero;

	if (x == zero) {
		if (o->k == 1) {
			return o->s.f;
		} else {
			return zero;
		}
	} else {
		p *= o->s.f;
		xx *= p;
		for (int nn = 1; nn < o->k; nn++) {
			p *= (xx/nn);
		}
		return p * expf(- o->s.f * x);
	}
}

/*!
@brief	LRf_erlang_CDF(LR_obj *o, float x) - Erlang distribution
cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRf_erlang_CDF(LR_obj *o, float x) {
	float zero = 0.0, one = 1.0, xx = x, p = one, s = one;
	if (isnan(o->s.f))
		o->s.f = one/o->m.f;

	if (x < zero)	return zero;

	xx *= o->s.f;
	for (int nn = 1; nn < o->k; nn++) {
		p *= (xx / nn);
		s += p;
	}
	return one - s * expf(- o->s.f * x);
}


#ifdef __cplusplus
}
#endif
