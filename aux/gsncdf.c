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

where H(x) is the Heaviside step function:

	H(x) =	{0 if x < 0
		{1 if x >= 0

and (n m) = n!/(m!*(n-m)!) (combination factor)

This code will handle each of the intervals [-1,0][0-1][1-2]...[11-12][12-13]
and expressly handle the polynomial coefficients for each interval.
It will the CDF and from differentiating that the new PDF.

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

/* return a + b*x as numerator & denominator */
void iadd(LINT an, LINT ad, LINT bn, LINT bd, LINT x, LINT *on, LINT *od) {
	LINT xx;

	if (an == 0)	ad = 1;
	if (bn == 0)	bd = 1;

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

	if (an == 0)	ad = 1;
	if (bn == 0)	bd = 1;

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
		printf("{");
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

/* calculate the new CDF from the current PDF */
void newcdf(int nn) {
	extern LINT	pn[NINT][NINT], pd[NINT][NINT],
			cn[NINT][NINT], cd[NINT][NINT];

	LINT ss = 1;
	/* recenter at n/2 */
	for (int i = 0; i <= nn; i++) {
	/* do (x-i+n/2)^n for all intervals > i */
		LINT coef = ss*icomb((LINT) nn, (LINT) i);
		ss *= -1;
		for (int j = i; j <= nn; j++) {
			/* binomial expansion*/
			cn[j][nn] += coef;
			LINT s = (LINT) (-i + nn/2);
			for (int k = 1; k <= nn; k++) {
				cn[j][nn-k]+=(s*coef*icomb((LINT) nn,(LINT) k));
				s *= (LINT) (-i + nn/2);
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

int main() {
	int nn = 4;
	newcdf(nn);
	showdf("CDF",nn, cn, cd);
	newpdf(nn);
	showdf("PDF",nn, pn, pd);

if (1) {
	writedf("gscdfn", nn, cn);
	writedf("gscdfd", nn, cd);
	writedf("gspdfn", nn, pn);
	writedf("gspdfd", nn, pd);
}
}
