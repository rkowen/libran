
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

/* lower precision int/float */
/* initialize arrays */
	for (i=0; i<arrdim; ++i) {
		iarr1[i] = 0;
		uarr1[i] = 0;
	}
/* initialize seed */
	LR_isetseed(imyseed);
	dten = 10./((double) LR_IRAND_IMAX);
	ftst = LR_frand();
	itst = LR_irand();
	PTEST(LR_igetrand(), itst );
	NTEST(LR_fgetrand(), ftst );
	itst = LR_irand();
	ftst = LR_frand();
	PTEST(LR_fgetrand(), ftst );
	NTEST(LR_igetrand(), itst );
	PTEST(LR_igetseed(), imyseed );

/* sample from irand */
	LR_isetseed(imyseed);
	for (i = 0; i < sample; ++i) {
		++iarr1[(int) (((double) LR_irand()) * dten)];
		imax = (imax > LR_igetrand() ? imax : LR_igetrand());
	}
/* display results */
	printf("seed      = %ld %ld\n",(long) imyseed, (long) LR_igetseed());
	printf("IRAND MAX = %ld\n",(long) LR_IRAND_IMAX);
	printf("imax      = %ld\n", (long) imax);
	printf("sampling from LR_irand()\n");
	for (i=0; i<arrdim; ++i) {
		printf(" %5d",iarr1[i]);
	}
	printf("\n");
/* sample from LR_frand */
	printf("sampling from LR_frand()\n");
	LR_isetseed(imyseed);
	for (i = 0; i < sample; ++i) {
		++uarr1[((int) (10.*LR_frand()))];
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
	LR_lsetseed(lmyseed);
	dten = 10./((double) LR_IRAND_LMAX);
	dtst = LR_drand();
	ltst = LR_lrand();
	PTEST(LR_lgetrand(), ltst );
	NTEST(LR_dgetrand(), dtst );
	ltst = LR_lrand();
	dtst = LR_drand();
	PTEST(LR_dgetrand(), dtst );
	NTEST(LR_lgetrand(), ltst );
	PTEST(LR_lgetseed(), lmyseed );

/* sample from irand */
	LR_lsetseed(lmyseed);
	for (i = 0; i < sample; ++i) {
		++iarr1[(int) (((double) LR_lrand()) * dten)];
		lmax = (lmax > LR_lgetrand() ? lmax : LR_lgetrand());
	}
/* display results */
	printf("seed      = %ld %ld\n",(long) lmyseed, (long) LR_lgetseed());
	printf("IRAND MAX = %ld\n",(long) LR_IRAND_LMAX);
	printf("lmax      = %ld\n", (long) lmax);
	printf("sampling from LR_lrand()\n");
	for (i=0; i<arrdim; ++i) {
		printf(" %5d",iarr1[i]);
	}
	printf("\n");
/* sample from LR_drand */
	printf("sampling from LR_drand()\n");
	LR_lsetseed(lmyseed);
	for (i = 0; i < sample; ++i) {
		++uarr1[((int) (10.*LR_drand()))];
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
