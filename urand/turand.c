/*!
\file	turand.c
\brief	basic urand test program

Run a series of tests on the urand routines.  This is more for development
and not part of the test suite.  Just do "make turand" to create the
executable - "turand" .  Run with "./turand" .

*/

#define PTEST(a,b)	\
	if ((a) == (b)) printf("OK  "); \
	else printf("FAIL"); \
	printf(" : " #a "\n");

#define NTEST(a,b)	\
	if ((a) != (b)) printf("OK  "); \
	else printf("FAIL"); \
	printf(" : " #a "\n");

#define ABS(a)	((a) < 0 ? -(a) : (a))

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "libran.h"
#include "urand.h"
#include "config.h"

#ifndef MAXSMP
#  define MAXSMP 1000000
#endif

int main() {
	int i, j;
	int arrdim = 10;
	int iarr1[10];
	int uarr1[10];
	int sample = 100000;
	int imyseed = sizeof(int), imax = 0, itst;
	long lmyseed = sizeof(long), lmax = 0, ltst;
	double dten, dtst;
	float ftst;
	LR_obj *o = LR_new(unif,LR_double);

/* lower precision int/float */
/* initialize arrays */
	for (i=0; i<arrdim; ++i) {
		iarr1[i] = 0;
		uarr1[i] = 0;
	}
/* initialize seed */
	LR_isetseed(o, imyseed);
	dten = 10./((double) LR_IRAND_IMAX);
	ftst = LR_frand(o);
	itst = LR_irand(o);
	PTEST(LR_igetrand(o), itst );
	NTEST(LR_fgetrand(o), ftst );
	itst = LR_irand(o);
	ftst = LR_frand(o);
	PTEST(LR_fgetrand(o), ftst );
	NTEST(LR_igetrand(o), itst );
	PTEST(LR_igetseed(o), imyseed );

/* sample from irand */
	LR_isetseed(o, imyseed);
	for (i = 0; i < sample; ++i) {
		++iarr1[(int) (((double) LR_irand(o)) * dten)];
		imax = (imax > LR_igetrand(o) ? imax : LR_igetrand(o));
	}
/* display results */
	printf("seed          = %ld %ld\n",
		(long) imyseed,(long) LR_igetseed(o));
	printf("IRAND MAX     = %ld\n",(long) LR_IRAND_IMAX);
	printf("LR_IRAND_IMAX = %ld\n",(long) LR_igetval("LR_IRAND_IMAX"));
	printf("LR_IRAND_IMAX2= %ld\n",(long) LR_igetval("LR_IRAND_IMAX2"));
	printf("LR_IRAND_INOT = %ld\n",(long) LR_igetval("LR_IRAND_INOT"));
	printf("imax      = %ld\n", (long) imax);
	printf("sampling from LR_irand()\n");
	for (i=0; i<arrdim; ++i) {
		printf(" %5d",iarr1[i]);
	}
	printf("\n");
/* sample from LR_frand */
	printf("sampling from LR_frand()\n");
	LR_isetseed(o,imyseed);
	for (i = 0; i < sample; ++i) {
		++uarr1[((int) (10.*LR_frand(o)))];
	}
	for (i=0; i<arrdim; ++i) {
		printf(" %5d",uarr1[i]);
	}
	printf("\n");
/* the two arrays should be roughly equivalent */
	dten = sample/1000;
	itst = 0;
	for (i=0; i<arrdim; ++i) {
		if (ABS(iarr1[i] - uarr1[i]) > dten) itst++;
	}
	if (itst) printf("FAIL");
	else printf("OK  ");
	printf(" : compare LR_irand() and LR_frand() distributions\n");

/* higher precision long/double */
/* initialize arrays */
	for (i=0; i<arrdim; ++i) {
		iarr1[i] = 0;
		uarr1[i] = 0;
	}
/* initialize seed */
	LR_lsetseed(o,lmyseed);
	dten = 10./((double) LR_IRAND_LMAX);
	dtst = LR_drand(o);
	ltst = LR_lrand(o);
	PTEST(LR_lgetrand(o), ltst );
	NTEST(LR_dgetrand(o), dtst );
	ltst = LR_lrand(o);
	dtst = LR_drand(o);
	PTEST(LR_dgetrand(o), dtst );
	NTEST(LR_lgetrand(o), ltst );
	PTEST(LR_lgetseed(o), lmyseed );

/* sample from irand */
	LR_lsetseed(o,lmyseed);
	for (i = 0; i < sample; ++i) {
		++iarr1[(int) (((double) LR_lrand(o)) * dten)];
		lmax = (lmax > LR_lgetrand(o) ? lmax : LR_lgetrand(o));
	}
/* display results */
	printf("seed      = %ld %ld\n",(long) lmyseed, (long) LR_lgetseed(o));
	printf("IRAND MAX = %ld\n",(long) LR_IRAND_LMAX);
	printf("LR_IRAND_LMAX = %ld\n",(long) LR_lgetval("LR_IRAND_LMAX"));
	printf("LR_IRAND_LMAX2= %ld\n",(long) LR_lgetval("LR_IRAND_LMAX2"));
	printf("LR_IRAND_LNOT = %ld\n",(long) LR_lgetval("LR_IRAND_LNOT"));
	printf("lmax      = %ld\n", (long) lmax);
	printf("sampling from LR_lrand()\n");
	for (i=0; i<arrdim; ++i) {
		printf(" %5d",iarr1[i]);
	}
	printf("\n");
/* sample from LR_drand */
	printf("sampling from LR_drand()\n");
	LR_lsetseed(o,lmyseed);
	for (i = 0; i < sample; ++i) {
		++uarr1[((int) (10.*LR_drand(o)))];
	}
	for (i=0; i<arrdim; ++i) {
		printf(" %5d",uarr1[i]);
	}
	printf("\n");
/* the two arrays should be roughly equivalent */
	dten = sample/1000;
	ltst = 0;
	for (i=0; i<arrdim; ++i) {
		if (ABS(iarr1[i] - uarr1[i]) > dten) ltst++;
	}
	if (ltst) printf("FAIL");
	else printf("OK  ");
	printf(" : compare LR_lrand() and LR_drand() distributions\n");

	return 0;
}
