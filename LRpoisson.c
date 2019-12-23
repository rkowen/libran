/*!
\file	LRpoisson.c
\brief 	The Poisson distribution with rate \e p > 0.

The pseudo-random numbers are distributed from the Poisson distribution,
which is a discrete distribution describing the number of events
occurring in a fixed interval of time.

The attribute \e p is greater than zero and represents the "rate"
of events per unit interval
(and need not be integer).
Therefore, \e p is the mean number of events per unit interval.

\manonly
   PDF(k) = p^k * exp(-p) / k!
   CDF(k) = exp(-p) * sum_(n=0)^(k) p^n / n!
\endmanonly

\f{eqnarray*}{
\mbox{PDF}(k) &=
\left\{ \begin{array}{ll}
                0, &    k < 0 \\
        	\frac{p^k e^{-p}}{k!}, &    0 \le k .
        \end{array} \right.
\\
\\
\mbox{CDF}(k) &=
\left\{ \begin{array}{ll}
	0, &    k < 0 \\
       	e^{-p} \sum_{n=0}^{k} \frac{p^n}{n!} , & 0 \le k .
        \end{array} \right.
\f}


The default is \f$ p = 1 \f$ and \em q will be set to \f$ e^{-p} \f$
for calculation efficiency.
Do not set \e q when declaring this distribution.

\see LRerlang.c LRnexp.c
 
\image html PoissonDistribution.png
\image latex PoissonDistribution.eps "Poisson Distribution"

The plots show the distribution functions as continuous only to
visually connect adjacent values but are actually discrete.
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

/* int */
/*!
@brief	LRi_poisson_RAN(LR_obj *o) - int Poisson distributed variate. 
Default values: scale m = 1.

@param o        LR_obj object
@return int
*/
int LRi_poisson_RAN(LR_obj *o) {
	float zero = 0.0, one = 1.0, u, p = one;
	int kk = 0;
	if (isnan(o->q))
		o->q = expf(-o->p);

	do {
		do {
			u = o->uf(o);
		} while (u == zero);
		kk++;
		p *= u;
	} while (p > o->q);

	return kk - 1;
}

/*!
@brief	LRi_poisson_PDF(LR_obj *o, int x) - Poisson 
probablity (or mass) distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRi_poisson_PDF(LR_obj *o, int x) {
	float zero = 0.0, one = 1.0, p = one;
	if (isnan(o->q))
		o->q = expf(- o->p);

	if (x < 0)	return zero;

	if (x == 0) {
		return o->q;
	} else {
		for (int nn = 1; nn <= x; nn++) {
			p *= (o->p/nn);
		}
		return p * o->q;
	}
}

/*!
@brief	LRi_poisson_CDF(LR_obj *o, int x) - Poisson distribution
cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRi_poisson_CDF(LR_obj *o, int x) {
	float zero = 0.0, one = 1.0, p = one, s = one;
	if (isnan(o->q))
		o->q = expf(- o->p);

	if (x < 0)	return zero;

	if (x == 0) {
		return o->q;
	} else {
		for (int nn = 1; nn <= x; nn++) {
			p *= (o->p/nn);
			s += p;
		}
		return s * o->q;
	}
}

#ifdef __cplusplus
}
#endif
