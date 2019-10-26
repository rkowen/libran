/*!
\file	gsn.c
\brief	auxillary program to perform the derivations necessary for computing
the CDF and PDF for summing a number of uniform random variates.
Can safely handle upto 12 uniform random variates without overflow.

Adding two random variates will produce a new CDF which is the convolution
of the two individual respective PDFs.

The uniform random variate has:

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


Look at page 22 of "Non-Uniform Random Variate Generation" by Luc Devroye.

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
\mbox{and the combination factor }
\left(\begin{array}{c} {n} \\ {m} \end{array}\right)
= \frac{n!}{m!(n-m)!}
\f]

This code will handle each of the intervals individually
and expressly handle the polynomial coefficients for that interval
as rational values (ratio of integers) and performing basic
mathematic operations \b exactly .
The result will be the new CDF and when differentiated the new PDF.

Finally, translate the polynomial coefficients to the local interval
where \f$ 0 \le x < 1 \f$
to prevent inaccuracies and overflows when evaluating a large polynomial.

Finally the coefficients pairs are output into a format such the text
could be included into C code with minimal editing.
This was how the coefficients were generated for `LRgsn.c`.

The invoking
functions must initialize the floating point coefficients by calculating
the ratio of coefficient pairs.  This only needs to be done once if
the floating point values are stored in an array.

\see{LRgsn.c}

*/

#include <stdio.h>
#include <limits.h>

#define	NINT	15

/*!
\typedef	LINT
\brief		long integer type used for calculations.
*/
typedef	long	LINT;

/* poly order -> ,   interval downward v */

/*!
\var	LINT pn[NINT][NINT]
\brief	PDF numerator
*/
LINT 	pn[NINT][NINT]	= {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

/*!
\var	LINT pd[NINT][NINT]
\brief	PDF denominator
*/
LINT 	pd[NINT][NINT]	= { /*! PDF denominator */
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

/*!
\var	LINT cn[NINT][NINT]
\brief	CDF numerator
*/
LINT 	cn[NINT][NINT]	= {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

/*!
\var	LINT cd[NINT][NINT]
\brief	CDF denominator
*/
LINT 	cd[NINT][NINT]	= {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

/*!
@brief LINT igcd(LINT a, LINT b) return Greatest Common Denominator of a & b
*/
LINT igcd(LINT a, LINT b) {
	LINT t;
	if (a == 0 || b == 0)	return 0;
	if (a < 0)	a = -a;
	if (b < 0)	b = -b;
	if (a < b) { /* swap */ t = a; a = b; b = t; }
	while ((t = a%b) > 1) { a = b; b = t; }
	if (t)	return t; else	return b;
}

/*!
@brief LINT ifac(LINT n) return the factorial of n (n!)
*/
LINT ifac(int n) {
	LINT i = 1, t = 1;
	if (n == 0 || n == 1)	return t;

	do {
		i++;
		t *= i;
	} while (i < n);

	return t;
}

/*!
@brief LINT icomb(LINT n, LINT k) return binomial factor n!/[k!*(n-k)!]
*/
LINT icomb(int n, int k) {
	return ifac(n) / ifac(k) / ifac(n-k);
}

/*!
@brief void iadd(an,ad,bn,bd, x, *on, *od) return a + b*x where a,b
are represented as rationals

@param LINT an	'a' numerator
@param LINT ad	'a' denominator
@param LINT bn	'b' numerator
@param LINT bd	'b' denominator
@param LINT x	 multiplication factor
@param LINT *on	 output numerator
@param LINT *od	 output denominator
*/
void iadd(LINT an, LINT ad, LINT bn, LINT bd, LINT x, LINT *on, LINT *od) {
	LINT xx;

	if (an == 0 || ad == 0)	ad = 1;
	if (bn == 0 || bd == 0)	bd = 1;

	*on = an*bd + bn*ad*x;
	*od = ad*bd;
	xx = igcd(*on, *od);
	if (xx != 0 && xx != 1) {
		*on /= xx;
		*od /= xx;
	}
	/* set denominator to 1 if numerator is 0 */
	if (*on == 0)
		*od = 1;
}

/*!
@brief void imult(an,ad,bn,bd, x, *on, *od) return a*b*x where a,b
are represented as rationals

@param LINT an	'a' numerator
@param LINT ad	'a' denominator
@param LINT bn	'b' numerator
@param LINT bd	'b' denominator
@param LINT x	 multiplication factor
@param LINT *on	 output numerator
@param LINT *od	 output denominator
*/
void imult(LINT an, LINT ad, LINT bn, LINT bd, LINT x, LINT *on, LINT *od) {
	LINT xx;

	if (an == 0 || ad == 0)	ad = 1;
	if (bn == 0 || bd == 0)	bd = 1;

	*on = an*bn*x;
	*od = ad*bd;
	xx = igcd(*on, *od);
	if (xx != 0 && xx != 1) {
		*on /= xx;
		*od /= xx;
	}
	/* set denominator to 1 if numerator is 0 */
	if (*on == 0)
		*od = 1;
}

/*!
@brief void showdf(DFstr, order, xn[][NINT], xd[][NINT]) display the rational values of 'x'.

@param char * DFstr	descriptive string
@param int order	maximum order number
@param LINT xn[][NINT]	output numerator
@param LINT xd[][NINT]	output denominator
*/
void showdf(char *DFstr, int order, LINT xn[][NINT], LINT xd[][NINT]) {
	printf("%s : %d >>> \n", DFstr, order);
	for (int j = 0; j <= order; j++) {
		printf("% 2d = ",j);
		for (int i = 0; i <= order; i++) {
			if (xn[j][i])
				printf("%+ld/%ld*x^%d",xn[j][i],xd[j][i],i);
		}
		printf("\n");
	}
}

/*!
@brief void writedf(DFstr, order, xn[][NINT]) output array as a C declaration

@param char * DFstr	descriptive string
@param int order	maximum order number
@param LINT on[][NINT]	output array
*/
void writedf(char *DFstr, int order, LINT xn[][NINT]) {
	LINT maxcoef = 0;

	printf("long %s%d[%d][%d] = {\n", DFstr, order, order+1, order+1);
	for (int j = 0; j <= order; j++) {
		printf("  {");
		for (int i = 0; i <= order; i++) {
			printf("%ldl",xn[j][i]);
			if (i < order) printf(",");
			LINT cc = (xn[j][i] < 0 ? -xn[j][i] : xn[j][i]);
			if (cc > maxcoef)
				maxcoef = cc;
		}
		printf("}");
		if (j < order) printf(",");
		printf("\n");
	}
	printf("};\n");
	printf("/* integer type must support max value = %ld */\n\n", maxcoef);
}

/*!
@brief void newcdf(nn) calculate the new CDF of order nn

This is the main driver routine that generates the rational coefficients
for the new Gaussian-like culmulative distributions functions of order n

@param int nn	order of gsn.
*/
void newcdf(int nn) {
	extern LINT	pn[NINT][NINT], pd[NINT][NINT],
			cn[NINT][NINT], cd[NINT][NINT];

	LINT ss = 1;
	for (int i = 0; i <= nn; i++) {
	/* do (x-i)^n for all intervals > i */
		LINT coef = ss*icomb((LINT) nn, (LINT) i);
		ss *= -1;
		for (int j = i; j <= nn; j++) {
			/* binomial expansion*/
			cn[j][nn] += coef;
			LINT s = (LINT) (-i);
			for (int k = 1; k <= nn; k++) {
				cn[j][nn-k]+=(s*coef*icomb((LINT) nn,(LINT) k));
				s *= (LINT) (-i);
			}
		}
	}
	/* normalize with 1/n! */
	LINT coef = ifac(nn); 
	LINT cc, maxint = 0, maxcoef = 0;
	for (int j = 0; j <= nn; j++) {
		for (int i = 0; i <= nn; i++) {
			cc = (cn[j][i] < 0 ? -cn[j][i] : cn[j][i]);
			if (cc > maxint)
				maxint = cc;
			imult(cn[j][i],cd[j][i],(LINT) 1,coef,(LINT) 1, 
				&(cn[j][i]),&(cd[j][i])); 
			cc = (cn[j][i] < 0 ? -cn[j][i] : cn[j][i]);
			if (cc > maxcoef)
				maxcoef = cc;
			cc = (cd[j][i] < 0 ? -cd[j][i] : cd[j][i]);
			if (cc > maxcoef)
				maxcoef = cc;
		}
	}
	printf("max int  = %ld\nmax coef = %ld\n"
		"INT_MAX  = %d\nLONG_MAX = %ld\nLLONG_MAX = %lld\n"
		"%d! = %ld\n",
		maxint, maxcoef,
		INT_MAX, LONG_MAX, LLONG_MAX, nn, ifac(nn));
}

/*!
@brief void newpdf(nn) calculate the new PDF of order nn by differentiating
the CDF.

This is the other main driver routine that generates the rational coefficients
for the new Gaussian-like probability distributions functions of order n

@param int nn	order of gsn.
*/
void newpdf(int nn) {
	extern LINT	pn[NINT][NINT], pd[NINT][NINT],
			cn[NINT][NINT], cd[NINT][NINT];

	for (int j = 0; j <= nn; j++) {
		for (int i = 1; i <= nn; i++) {
			imult((LINT) i,(LINT) 1,cn[j][i],cd[j][i],(LINT) 1,
				&(pn[j][i-1]), &(pd[j][i-1]));
		}
	}
}

/*!
@brief void transint(nn, xn[][NINT], xd[]{NINT]) translate each interval to [0,1)

This translates the polynomial coefficients from [i,i+1) ot [0,1)
where the coefficients are represented by rational values.

\manonly
   Given f(x) = sum^n a_i x^i
  
   f(x+c) = sum_m=0 a'_m x^m
  
   where a'_m = sum^n_k=m (k k-m) a_k c^(k-m)
\endmanonly
\f[
\mbox{Given } f(x) = \sum_{i = 0}^{n} a_i x^i
\f]
\f[
f(x+c) = \sum_{m = 0}^{n} a'_m x^m
\f]
\f[
\mbox{Where } a'_m = \sum_{k = m}^{n}
\left(\begin{array}{c} {k} \\ {k-m} \end{array}\right)
a_k c^{k-m}
\f]
  
@param int nn	order of gsn.
@param LINT xn[][NINT]	input/output numerator
@param LINT xd[][NINT]	input/output denominator
 */
void transint(int nn, LINT xn[][NINT], LINT xd[][NINT]) {
	LINT tmpn[NINT], tmpd[NINT], zero = 0, one = 1;
	for (int j = 0; j <= nn; j++) {	/* interval */
		/* zero tmp */
		for (int i = 0; i <= nn; i++) {
			tmpn[i] = zero;
			tmpd[i] = one;
		}

		for (int m = 0; m <= nn; m++) {
			/* use Horners rule and binomial expansion*/
			LINT ct = j;
			LINT cn = icomb(nn,nn-m)*xn[j][nn], cd = xd[j][nn];

			for (int k = nn-1; k >= m; k--) {
/*
	printf("icomb(%d,%d) = %ld\txn = %ld\tcn = %ld\n",
		k,k-m, icomb(k,k-m),xn[j][k], cn);
*/
				iadd(icomb(k,k-m)*xn[j][k], xd[j][k], cn, cd,
					ct, &cn, &cd);
			}
			tmpn[m] = cn;
			tmpd[m] = cd;
		}
		/* transfer results back */
		for (int i = 0; i <= nn; i++) {
			xn[j][i] = tmpn[i];
			xd[j][i] = tmpd[i];
		}
	}
	if (0) {
/* This code bit can be enabled for checking that the coefficient
 * values make sense.  On each local interval sum of the coefs is the
 * same as evaluating the function where x=1 this
 * should give the  a_0 coef on the next interval or the x=0 evaluation.
 */
		for (int j = 0; j <= nn; j++) {	/* interval */
			/* sum coefs (same as x=1 on local interval */
			/* should equal the constant on next interval */
			LINT sn = zero, sd = one;
			for (int i = 0; i <= nn; i++) {
				iadd(sn,sd, xn[j][i], xd[j][i], one, &sn, &sd);
			}
			printf("*** %d %ld/%ld\n", j, sn, sd);
			if (j < nn)
				printf("+++ %d %ld/%ld\n", j+1,
					xn[j+1][0], xd[j+1][0]);
		}
	}
}

int main() {
	int nn = 12;
	newcdf(nn);
	showdf("CDF",nn, cn, cd);
	newpdf(nn);
	showdf("PDF",nn, pn, pd);
if (1) {
	transint(nn, cn, cd);
	showdf("CDF",nn, cn, cd);
	transint(nn, pn, pd);
	showdf("PDF",nn, pn, pd);
}

if (0) {
	writedf("gscdfn", nn, cn);
	writedf("gscdfd", nn, cd);
	writedf("gspdfn", nn, pn);
	writedf("gspdfd", nn, pd);
}
}
