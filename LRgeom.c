/*!
\file	LRgeom.c
\brief 	The Geometric distribution with rate \e p > 0.

The pseudo-random numbers are distributed from the Geometric distribution,
which is a discrete distribution describing the number of Bernoulli
trails until a successful event.  A Bernoulli trial is one characterized
by either "success" or "failure."  For example, the number of coin flips
until a \e heads is shown is described by the Geometric distribution.

The attribute \e p is greater than zero and less than one and
represents the probability of a successful trial.
Therefore, \e 1/p is the mean number of trials until success.

\manonly
   PDF(k) = p^k * exp(-p) / k!
   CDF(k) = exp(-p) * sum_(n=0)^(k) p^n / n!
\endmanonly

\f{eqnarray*}{
\mbox{PDF}(k) &=
\left\{ \begin{array}{ll}
                0, &    k \le 0 \\
        	p(1-p)^{k-1}, &    0 < k .
        \end{array} \right.
\\
\\
\mbox{CDF}(k) &=
\left\{ \begin{array}{ll}
                0, &    k \le 0 \\
        	1 - (1-p)^k, &    0 < k .
        \end{array} \right.
\f}


The default is \f$ p = \frac{1}{2} \f$ and \em q will be
set to \f$ \frac{1}{\log(1-p)} \f$
for calculation efficiency.
Do not set \e q when declaring this distribution.

\image html GeometricDistribution.png
\image latex GeometricDistribution.eps "Geometric Distribution"

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
@brief	LRi_geometric_RAN(LR_obj *o) - int Geometric distributed variate. 
Default values: probability of success \e p = 1/2.

The random variate is generated using inversion and truncation.

@param o        LR_obj object
@return int
*/
int LRi_geometric_RAN(LR_obj *o) {
	float zero = 0.0, one = 1.0, u, p = one;
	if (isnan(o->q))
		o->q = one/logf(one - o->p);

	do {
		u = o->uf(o);
	} while (u == zero);

	return ceilf(o->q * logf(u));
}

/*!
@brief	LRi_geometric_PDF(LR_obj *o, int x) - Geometric 
probablity (or mass) distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRi_geometric_PDF(LR_obj *o, int x) {
	float zero = 0.0, one = 1.0, p = o->p;

	if (x <= 0)	return zero;

	if (x == 1) {
		return p;
	} else {
		float q = one - p;
		for (int nn = 2; nn <= x; nn++) {
			p *= q;
		}
		return p;
	}
}

/*!
@brief	LRi_geometric_CDF(LR_obj *o, int x) - Geometric distribution
cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRi_geometric_CDF(LR_obj *o, int x) {
	float zero = 0.0, one = 1.0, q = one - o->p, p = q;

	if (x <= 0)	return zero;

	if (x == 1) {
		return one - p;
	} else {
		for (int nn = 2; nn <= x; nn++) {
			p *= q;
		}
		return one - p;
	}
}

#ifdef __cplusplus
}
#endif
