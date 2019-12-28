/*!
\file	LRbinom.c
\brief 	The Binomial distribution with probability of success \e p > 0
and \e n independent tests.

The pseudo-random numbers are distributed from the Binomial distribution,
which is a discrete distribution describing the number of successful
Bernoulli trails in \e n events.
A Bernoulli trial is one characterized
by either "success" or "failure."  For example, \e n coin flips
where the number of \e heads is shown
is described by the Bimomial distribution.
If \e n is large and \e p not "extreme"
then the distribution will approach the \e Normal distribution, and
these routines may not be as effective.
This approach to a \e Normal distribution is established by
the "central limit theorem."

The attribute \e p is greater than zero and less than one and
represents the probability of a successful trial.
Therefore, \e np is the mean number of successful trials in \e n events
and also may not be an integer value.

\manonly
   PDF(k) = (n k)p^k*(1-p)^{n-k}
   CDF(k) = sum_(i=0)^(k) (n i) p^i *(1-p)^{n-i}
\endmanonly

\f{eqnarray*}{
\mbox{PDF}(k) &=
\left. \begin{array}{ll}
		\left(\begin{array}{c} {n} \\ {k} \end{array}\right)
		p^k (1-p)^{n-k}, &		0 \le k \le n
        \end{array} \right.
\\
\\
\mbox{CDF}(k) &=
\left. \begin{array}{ll}
		\sum_{i=0}^{k}
		\left(\begin{array}{c} {n} \\ {i} \end{array}\right)
		p^i (1-p)^{n-i}, &	0 \le k \le n
        \end{array} \right.
\f}


The default is \f$ p = \frac{1}{2} \f$ and \em q will be
set to \f$ \frac{1}{\log(1-p)} \f$
for calculation efficiency.
Do not set \e q when declaring this distribution.

\image html BinomialDistribution.png
\image latex BinomialDistribution.eps "Binomial Distribution"

The plots show the distribution functions as continuous only to
visually connect adjacent values but are actually discrete.

\see LRgaus.c LRgsn.c LRgeom.c
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
@brief	LRi_binomial_RAN(LR_obj *o) - int Binomial distributed variate. 
Default values: probability of success \e p = 1/2.

The random variate is generated using the waiting time property -
Generate a number of exponential variates until the waited sum
exceeds some value.

@param o        LR_obj object
@return int
*/
int LRi_binomial_RAN(LR_obj *o) {
	float	zero = 0.0, one = 1.0, u, sum = zero, p = one;
	int	i;
	if (isnan(o->q))
		o->q = -logf(one - o->p);

	for (i = 0; i < o->n; i++) {
		do {
			u = o->uf(o);
		} while (u == zero);
		sum += (-logf(u)/(o->n - i));
		if (sum > o->q)
			break;
	}

	return i;
}

/*!
@brief	LRi_binomial_PDF(LR_obj *o, int k) - Binomial 
probablity (or mass) distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRi_binomial_PDF(LR_obj *o, int k) {
	float zero = 0.0, one = 1.0, p = o->p, lp, lq, lt;

	if (isnan(o->q))
		o->q = -logf(one - o->p);

	if (k < 0 || k > o->n)	return zero;

	lp = logf(o->p);
	lq = - o->q;

	lt = k*lp + (o->n - k)*lq;
	lt += (lgammaf(o->n + 1) - lgammaf(k + 1) - lgammaf(o->n - k + 1));

	return expf(lt);
}

/*!
@brief	LRi_binomial_CDF(LR_obj *o, int k) - Binomial distribution
cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRi_binomial_CDF(LR_obj *o, int k) {
	float zero = 0.0, one = 1.0, sum = zero, p = o->p, lp, lq, lt;

	if (isnan(o->q))
		o->q = -logf(one - o->p);

	if (k < 0 || k > o->n)	return zero;

	lp = logf(o->p);
	lq = - o->q;

	for (int i = 0; i <= k; i++) {
		lt = i*lp + (o->n - i)*lq;
		lt += (lgammaf(o->n + 1) - lgammaf(i+1) - lgammaf(o->n - i+1));
		sum += expf(lt);
	}

	return sum;
}

#ifdef __cplusplus
}
#endif
