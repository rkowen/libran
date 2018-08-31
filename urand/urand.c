static const char AUTHOR[]="@(#)urand 06/02/94 1.3 R.K.Owen,PhD";
static const char RCSID[]="$Id: urand.c 106 2002-02-10 08:02:39Z rk $";
/*      URAND IS A UNIFORM RANDOM NUMBER GENERATOR BASED  ON  THEORY  AND
 *  SUGGESTIONS  GIVEN  IN  D.E. KNUTH (1969),  VOL  2.   THE INTEGER  IY
 *  SHOULD BE INITIALIZED TO AN ARBITRARY INTEGER PRIOR TO THE FIRST CALL
 *  TO URAND.  THE CALLING PROGRAM SHOULD  NOT  ALTER  THE  VALUE  OF  IY
 *  BETWEEN  SUBSEQUENT CALLS TO URAND.  VALUES OF URAND WILL BE RETURNED
 *  IN THE INTERVAL (0,1).
 *
 *  C version ran through f2c and cleaned up by R.K.Owen,Ph.D. 02/12/93
 *  with many features added.
 */
/*
 *  irand()	- returns INTEGER   in range [0,IRAND_MAX]
 *  urand()	- returns PRECISION in range [0,1.0)
 *  setseed(INTEGER X)	- sets the seed to X
 *  getseed()	- gets the current seed
 *  getirand()	- returns last value returned from irand()
 *  geturand()	- returns last value returned from urand()
 *
 *  extern INTEGER IRAND_MAX	- the maximum value returned by irand
 *				  it's only defined after a call to
 *				  urand(), irand(), or setseed().
 * #define NOINTOVF	- if no integer overflow on addition
 * #define NOADDMULT	- if wordlength for addtion is same as multiplication
 * #define NOOPADD	- if using ^m is faster than -=m2, -=m2
 * #define NOOPSUB	- if using ^m is faster than +=m2, +=m2
 */
/*
 *********************************************************************
 *
 *     This software is copyrighted by R.K.Owen,Ph.D. 1996
 *
 * The author, R.K.Owen, of this software is not liable for any
 * problems WHATSOEVER which may result from use  or  abuse  of
 * this software. The author, R.K.Owen, grants unlimited rights
 * to anyone who uses and modifies this  software  for  private
 * non-commercial  use  as  long  as  this copyright and rights
 * notice remains in this software and is made available to all
 * recipients of this software.
 *
 * last known email: rk@owen.sj.ca.us
 *
 *********************************************************************
 */
/*
 * define TEST to create a stand-alone test program
 */

#if 0
#  define TEST
#endif

#include "urand.h"

#ifdef __cplusplus
extern "C" {
#endif

/* global variables */
INTEGER iy, iy0;
INTEGER m = 1, m2 = 0;
PRECISION s;
PRECISION halfm;
INTEGER ia, ic, mic;
INTEGER IRAND_MAX = 0;

#define SQRT3 1.7320508075688772935
#define PI 3.141592653589793238462643

void _set_rand(void) {
	extern INTEGER m;
	extern INTEGER m2;
	extern PRECISION s;
	extern PRECISION halfm;
	extern INTEGER ia, ic, mic;
	extern INTEGER IRAND_MAX;

/*  COMPUTE MACHINE INTEGER WORD LENGTH */
	do {
		m2 = m;
		m = 2 * m2;
	} while (m > m2);

	halfm = (PRECISION) m2;
	IRAND_MAX = (m2 - 1) + m2;

/*  COMPUTE MULTIPLIER AND INCREMENT FOR LINEAR CONGRUENTIAL METHOD */
	ia = ((INTEGER) (halfm * PI / 32.) << 3) + 5;
	ic = ((INTEGER) (halfm * (.5 - SQRT3 / 6.)) << 1) + 1;
	mic = (m2 - ic) + m2;

/*  S IS THE SCALE FACTOR FOR CONVERTING TO FLOATING POINT */
	s = .5 / halfm;
}

INTEGER irand(void) {
	extern INTEGER iy;
	extern INTEGER m;
	extern INTEGER m2;
	extern PRECISION s;
	extern PRECISION halfm;
	extern INTEGER ia, ic, mic;

/*  IF FIRST ENTRY, COMPUTE URAND CONSTANTS */
	if (m2 == 0) _set_rand();

/*  COMPUTE NEXT RANDOM NUMBER */

	iy *= ia;

/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHICH DO NOT ALLOW */
/*  INTEGER OVERFLOW ON ADDITION */

#ifndef NOINTOVF
	if (iy > mic) {
#  ifndef NOOPSUB
		iy -= m2;
		iy -= m2;
#  else
		iy ^= m;
#  endif
	}
#endif
	iy += ic;

/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHERE THE */
/*  WORD LENGTH FOR ADDITION IS GREATER THAN FOR MULTIPLICATION */

#ifndef NOADDMULT
	if (
#ifndef NODIV
		(iy >> 1)
#else
		(iy/2)
#endif
	> m2) {
#  ifndef NOOPSUB
		iy -= m2;
		iy -= m2;
#  else
		iy ^= m;
#  endif
	}
#endif

/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHERE INTEGER */
/*  OVERFLOW AFFECTS THE SIGN BIT */

	if (iy < 0) {
#ifndef NOOPADD
		iy += m2;
		iy += m2;
#else
		iy ^= m;
#endif
	}

	return iy;
}

PRECISION urand(void) {
#ifndef HACK_INLINE
	extern INTEGER iy;
	extern INTEGER m2;
	extern PRECISION s;


/*  IF FIRST ENTRY, COMPUTE URAND CONSTANTS */
	if (m2 == 0) _set_rand();

/*  COMPUTE NEXT RANDOM NUMBER */

	irand();
#else
#  include "urand.in"
#endif

	return (PRECISION) (iy) * s;
}

void setseed(INTEGER ity) {
	extern INTEGER iy, iy0;
	extern INTEGER m2;

/*  IF FIRST ENTRY, COMPUTE URAND CONSTANTS */
	if (m2 == 0) _set_rand();

	iy = iy0 = ity;
}

INTEGER getseed(void) {
	extern INTEGER iy0;
	return iy0;
}

INTEGER getirand(void) {
	extern INTEGER iy;
	return iy;
}

PRECISION geturand(void) {
	extern INTEGER iy;
	extern PRECISION s;
	return (PRECISION) (iy) * s;
}

#ifdef __cplusplus
}
#endif

#ifdef TEST

#  define PTEST(a,b)	\
	if ((a) == (b)) printf("OK  "); \
	else printf("FAIL"); \
	printf(" : " #a "\n");

#  define NTEST(a,b)	\
	if ((a) != (b)) printf("OK  "); \
	else printf("FAIL"); \
	printf(" : " #a "\n");

#  define ABS(a)	((a) < 0 ? -(a) : (a))

#  include <time.h>
#  include <stdio.h>
#  include <stdlib.h>

#  include "urand_.c"	/* includes common definitions & code too */

#  ifndef MAXSMP
#    define MAXSMP 1000000
#  endif

int main() {
	extern INTEGER m, m2;
	extern INTEGER IRAND_MAX;
	int i, j;
	int arrdim = 10;
	int iarr1[10];
	int uarr1[10];
	int sample = 10000;
	INTEGER myseed = sizeof(INTEGER);
	INTEGER imax = 0;
	double iten;
	INTEGER itst;
	PRECISION utst;
	clocker_t t0;
	double t1[MAXTRY],t2[MAXTRY], t1m, t2m, t1a, t2a;

/* initialize arrays */
	for (i=0; i<arrdim; ++i) {
		iarr1[i] = 0;
		uarr1[i] = 0;
	}
/* initialize seed */
	setseed(myseed);
	iten = 10./((double) IRAND_MAX);
	utst = urand();
	itst = irand();
	PTEST(getirand(), itst );
	NTEST(geturand(), utst );
	itst = irand();
	utst = urand();
	PTEST(geturand(), utst );
	NTEST(getirand(), itst );
	PTEST(getseed(), myseed );

/* sample from irand */
	setseed(myseed);
	for (i = 0; i < sample; ++i) {
		++iarr1[(int) (((double) irand()) * iten)];
		imax = (imax > getirand() ? imax : getirand());
	}
/* display results */
	printf("seed      = %ld %ld\n",(long) myseed, (long) getseed());
	printf("m2        = %ld\n",(long) m2);
	printf("m         = %ld\t\t%lx\n",(long) m,(long) m);
	printf("m2        = %ld\t\t%lx\n",(long) m2,(long) m2);
	printf("IRAND_MAX = %ld\n",(long) IRAND_MAX);
	printf("imax      = %ld\n", (long) imax);
	printf("sampling from irand()\n");
	for (i=0; i<arrdim; ++i) {
		printf(" %5d",iarr1[i]);
	}
	printf("\n");
/* sample from urand */
	printf("sampling from urand()\n");
	setseed(myseed);
	for (i = 0; i < sample; ++i) {
		++uarr1[((int) (10.*urand()))];
	}
	for (i=0; i<arrdim; ++i) {
		printf(" %5d",uarr1[i]);
	}
	printf("\n");
/* the two arrays should be roughly equivalent */
	iten = sample/1000;
	itst = 0;
	for (i=0; i<arrdim; ++i) {
		if (ABS(iarr1[i] - uarr1[i]) > iten) itst++;
	}
	if (itst) printf("FAIL");
	else printf("OK  ");
	printf(" : comparison between irand() and urand() distribution\n");

/* timing results */

	for (j = 0; j < MAXTRY; ++j) {
/* clock irand */
		(void) clocker(&t0, _SET);
		for (i=0; i<MAXSMP; ++i) {
			itst = irand();
		}
/* clock urand */
		t1[j] = clocker(&t0, _RESET);
		for (i=0; i<MAXSMP; ++i) {
			utst = urand();
		}
		t2[j] = clocker(&t0, _RESET);
/* display time */
		printf("irand clocks: %7.3f\t\t", t1[j]);
		printf("urand clocks: %7.3f\n", t2[j]);
	}
/* find median value */
	qsort(t1, MAXTRY, sizeof(double), tcmp);
	qsort(t2, MAXTRY, sizeof(double), tcmp);
	t1m = t1[MAXTRY/2];
	t2m = t2[MAXTRY/2];
	t1a = tave(t1, MAXTRY);
	t2a = tave(t2, MAXTRY);

        printf("Median value --\n");
	printf("irand clocks: %7.3f\t\t", t1m);
	printf("urand clocks: %7.3f\n", t2m);
        printf("Average value --\n");
	printf("irand clocks: %7.3f\t\t", t1a);
	printf("urand clocks: %7.3f\n", t2a);
	return 0;
}

#endif /* TEST */
