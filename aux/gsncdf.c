/*!
\file	gsn.c
\brief	auxillary program to perform the derivations necessary for computing
the CDF and PDF summing a number of uniform random variates.
upto 12 uniform random variates.

Adding two random variates will produce a new PDF which is the convolution
of the two individual respective PDFs.

The uniform random variate has:

PDF	u(x) =	{ 0 if x < 0 || x >= 1
		{ 1 if 0 <= x < 1

CDF	U(x) =	{ 0 if x < 0
		{ x if 0 <= x < 1
		{ 1 if 1 <= x

Look at page 22 of "Non-Uniform Random Variate Generation" by Luc Devroye.

The CDF of n uniform random variates summed together is
F(x)=1/n! [H(x)^n - (n 1)H(x-1)^n + (n 2)H(x-2)^n - ... + ]

where H(x) is the positive part of x,
	H(x) =	{0 if x < 0
		{x if x >= 0

and (n m) = n!/(m!*(n-m)!) (combination factor)

This code will handle each of the intervals 
and expressly handle the polynomial coefficients for each interval.
The result will be the CDF and when differentiated the new PDF.

Finally, translate the polynomial coefficients to the local interval
to prevent inaccuracies when evaluating a large polynomial.

*/

#include <stdio.h>
#include <limits.h>

#define	NINT	15

typedef	long	LINT;

/* poly order -> ,   interval downward */
/* PDF numerator */
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
/* PDF denominator */
LINT 	pd[NINT][NINT]	= {
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
/* CDF numerator */
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
/* CDF denominator */
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

/* return GCD of a & b */
LINT igcd(LINT a, LINT b) {
	LINT t;
	if (a == 0 || b == 0)	return 0;
	if (a < 0)	a = -a;
	if (b < 0)	b = -b;
	if (a < b) { /* swap */ t = a; a = b; b = t; }
	while ((t = a%b) > 1) { a = b; b = t; }
	if (t)	return t; else	return b;
}

LINT ifac(int n) {
	LINT i = 1, t = 1;
	if (n == 0 || n == 1)	return t;

	do {
		i++;
		t *= i;
	} while (i < n);

	return t;
}

LINT icomb(int n, int k) {
	return ifac(n) / ifac(k) / ifac(n-k);
}

/* return a + b*x as numerator & denominator pairs */
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

/* return a*b*x as numerator & denominator */
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

/* display the DF */
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

/* write out the DF as a C declaration */
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

/* calculate a new CDF */
void newcdf(int nn) {
	extern LINT	pn[NINT][NINT], pd[NINT][NINT],
			cn[NINT][NINT], cd[NINT][NINT];

	LINT ss = 1;
	for (int i = 0; i <= nn; i++) {
	/* do (x-i+n/2)^n for all intervals > i */
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

/* calculate the new PDF by differentiating CDF */
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

/* translate the polynomial coefficients to the local interval */
/* trfn <= nn (order of the array) */
/*
 * Given f(x) = sum^n a_i x^i
 *
 * f(x+c) = sum_m=0 a'_m x^m
 *
 * where a'_m = sum^n_k=m (k k-m) a_k c^(k-m)
 *
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
