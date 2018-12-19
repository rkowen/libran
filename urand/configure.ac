#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([LibRan],[0.1],[rk@owen.sj.ca.us])
AM_INIT_AUTOMAKE(foreign -Wportability)
AC_CONFIG_SRCDIR([urand.c])
AC_CONFIG_HEADERS([config.h])
AC_AIX
AC_PROG_MAKE_SET

dnl ---------------------------------------------------------------------------
dnl Autoheader "templates" (was acconfig.h)
AH_TOP([
/*****
 ******************************************************************************
 LibRan/Urand - 
	Urand is a specialized sequential congruential uniform
	pseudo-random number generator based on theory and
	suggestions given in D.E. Knuth (1969), vol 2.  and then
	republished by G.E. Forsysth, M.A. Malcolm, and C.B. Moler (1977);
	and converted from Fortran 77 to C with f2c.

 Copyright 2018 by R.K. Owen, Ph.D.
 See LICENSE.LGPL, which must be provided, for details.

 ******************************************************************************
 *****/
])
AH_BOTTOM([
/* add URAND direct compiled values */

/* LibRan IRAND half max long value */
#undef LR_IRAND_LMAX2

/* LibRan IRAND long max */
#undef LR_IRAND_LMAX

/* LibRan IRAND long not max */
#undef LR_IRAND_LNOT

/* LibRan IRAND half max int value */
#undef LR_IRAND_IMAX2

/* LibRan IRAND int max */
#undef LR_IRAND_IMAX

/* LibRan IRAND int not max */
#undef LR_IRAND_INOT

/* +m2 as fast as ^m */
#undef LR_IADD
#undef LR_NOIADD
#undef LR_LADD
#undef LR_NOLADD

/* -m2 as fast as ^m */
#undef LR_ISUB
#undef LR_NOISUB
#undef LR_LSUB
#undef LR_NOLSUB

/* /2 as fast as >>1 */
#undef LR_IDIV
#undef LR_NOIDIV
#undef LR_LDIV
#undef LR_NOLDIV

])

dnl ---------------------------------------------------------------------------
dnl Some standard autoconf stuff
dnl ---------------------------------------------------------------------------
# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AM_PROG_CC_C_O

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])
# this is mostly for the configure time checks
AC_HEADER_TIME

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# time specific features
AC_LANG(C)

# test certain run-time features
AC_MSG_CHECKING([Check for wrapping integer overflow])
AC_RUN_IFELSE([AC_LANG_PROGRAM(
[
],[
int i = 7, ilast, j;
int isize = (int) 8 * sizeof(int);
for (j = 0; j < 2*isize; ++j) {
	ilast = i;
	i += i;
	if (!(j%3)) ++i;
	if ((ilast > i) || (i < 0)) return 0;
}
return 1;
])],[
AC_MSG_RESULT(does wrap-around integer overflow)
],[AC_MSG_RESULT(does not allow integer overflow wrap-around)
AC_DEFINE([LR_NOINTOVF],1,[Does not allow integer overflow wrap-around])
],
[AC_MSG_FAILURE(Can not cross compile to determine integer overflow behavior)]
)
AC_MSG_CHECKING([IRAND half max value])
AC_RUN_IFELSE([AC_LANG_PROGRAM(
[
#include <stdio.h>
],[
	long  mm2 = 0;
	long  mm = 1;
	int  imm2 = 0;
	int  imm = 1;
	FILE *conftest;
	if (!(conftest = fopen("confdefs.h","a")))
		return 1;
/*  COMPUTE MACHINE INTEGER WORD LENGTH */
	do {
		mm2 = mm;
		mm = 2 * mm2;
	} while (mm > mm2);
	do {
		imm2 = imm;
		imm = 2 * imm2;
	} while (imm > imm2);

	fprintf(conftest,
"#define LR_IRAND_LMAX2	%ldL\n"
"#define LR_IRAND_LMAX	%ldL\n"
"#define LR_IRAND_LNOT	0x%lxL\n"
"#define LR_IRAND_IMAX2	%d\n"
"#define LR_IRAND_IMAX	%d\n"
"#define LR_IRAND_INOT	0x%x\n",
		mm2, (mm2 - 1) + mm2, mm,
		imm2, (imm2 - 1) + imm2, imm);
])],[
AC_MSG_RESULT(found values)
],[AC_MSG_FAILURE(Failed to determine LR_IRAND constants)],
[AC_MSG_FAILURE(Can not cross compile LR_IRAND)]
)

AC_MSG_CHECKING([IRAND timing ADD/SUB/DIV operations])
AC_RUN_IFELSE([AC_LANG_SOURCE(
[[
#define MAXTRY 20
#define MAXDIM 5000000
#include <stdio.h>
#include <stdlib.h>		/* random */
#if defined(_AIX)
#  define _POSIX_SOURCE
#endif
#include <time.h>		/* time,CLK_TCK */
#include <sys/times.h>		/* times */
#include <unistd.h>		/* sysconf */
typedef clock_t clocker_t;
typedef enum {_SET = 0, _RESET, _READ, _PER_SEC} clocker_action;

/* clocker - looks at the user time
 *	_SET	= sets the timer returns 0.0 if no error
 *	_RESET	= returns the current time and resets the timer
 *	_READ	= returns the current time and continues on
 *	_PER_SEC= returns the number of ticks per sec
 */
clocker_t clocker_tick(clocker_t *clock_variable, clocker_action what_to_do) {
	struct tms ts;
	clock_t tc = *clock_variable;
	switch(what_to_do) {
	case _SET:
		(void) times(&ts);
		*clock_variable = ts.tms_utime;
		tc = 0;
		break;
	case _RESET:
		(void) times(&ts);
		tc = ts.tms_utime - tc;
		*clock_variable = ts.tms_utime;
		break;
	case _READ:
		(void) times(&ts);
		tc = ts.tms_utime - tc;
		break;
	case _PER_SEC:
		tc = sysconf(_SC_CLK_TCK);
		break;
	}
	return tc;
}

double clocker(clocker_t *clock_variable, clocker_action what_to_do) {
	clock_t t;
	double x;
	t = clocker_tick(clock_variable, what_to_do);
	if (what_to_do == _PER_SEC) {
		x = (double) t;
	} else {
		x = (double) t / (double) sysconf(_SC_CLK_TCK);
	}
	return x;
}
double tave(double t[], size_t maxt) {
	double sum = 0.0;
	size_t i;
	for (i = 0; i < maxt; ++i) {
		sum += t[i];
	}
	sum /= ((double) maxt);
	return sum;
}

int main() {
	int i, j, iflag = 0, lflag = 0;
	long *arr1, *arr2;
	int *iarr1, *iarr2;
	clocker_t t0;
	double t1[MAXTRY],t2[MAXTRY],it1[MAXTRY],it2[MAXTRY];
	double t1a, t2a;
/* allocate memory */
	arr1 = (long *) malloc(sizeof(long int) * MAXDIM);
	arr2 = (long *) malloc(sizeof(long int) * MAXDIM);
	iarr1 = (int *) malloc(sizeof(int) * MAXDIM);
	iarr2 = (int *) malloc(sizeof(int) * MAXDIM);
	FILE *conftest;
	if (!(conftest = fopen("confdefs.h","a")))
		return 1;
/* initialize seed */
	srandom((int) time((time_t *) NULL));

/* time if "+m2" as fast as "^m" */
	for (j = 0; j < MAXTRY; ++j) {
/* initialize arrays */
		for (i=0; i<MAXDIM; ++i) {
			arr1[i] = arr2[i] = random();
		}
		for (i=0; i<MAXDIM; ++i) {
			iarr1[i] = iarr2[i] = (int) LR_IRAND_IMAX & random();
		}
		(void) clocker(&t0, _SET);
/* + */
		for (i=0; i<MAXDIM; ++i) {
			iarr1[i] = (iarr1[i] + LR_IRAND_IMAX2) + LR_IRAND_IMAX2;
		}
		it1[j] = clocker(&t0, _RESET);
		for (i=0; i<MAXDIM; ++i) {
			arr1[i] = (arr1[i] + LR_IRAND_LMAX2) + LR_IRAND_LMAX2;
		}
		t1[j] = clocker(&t0, _RESET);
/* ^ */
		for (i=0; i<MAXDIM; ++i) {
			iarr2[i] = (iarr2[i] ^ LR_IRAND_INOT);
		}
		it2[j] = clocker(&t0, _RESET);
		for (i=0; i<MAXDIM; ++i) {
			arr2[i] = (arr2[i] ^ LR_IRAND_LNOT);
		}
		t2[j] = clocker(&t0, _RESET);
/* flag if different */
		for (i=0; i<MAXDIM; ++i) {
			if (iarr1[i] != iarr2[i])	iflag++;
			if (arr1[i] != arr2[i])		lflag++;
		}
	}
/* find average values */
	t1a = tave(it1, MAXTRY);
	t2a = tave(it2, MAXTRY);
	if (iflag || t2a >= t1a)
		fprintf(conftest,"#define LR_IADD 1\n");
	else
		fprintf(conftest,"#define LR_NOIADD 1\n");
	fprintf(stderr,"i:+m2 = %f, ^m = %f, flag = %d\n", t1a, t2a, iflag);

	t1a = tave(t1, MAXTRY);
	t2a = tave(t2, MAXTRY);
	if (lflag || t2a >= t1a)
		fprintf(conftest,"#define LR_LADD 1\n");
	else
		fprintf(conftest,"#define LR_NOLADD 1\n");
	fprintf(stderr,"l:+m2 = %f, ^m = %f, flag = %d\n", t1a, t2a, lflag);

/* time if "-m2" as fast as "^m" */
	iflag = lflag = 0;
	for (j = 0; j < MAXTRY; ++j) {
/* initialize arrays */
		for (i=0; i<MAXDIM; ++i) {
			arr1[i] = arr2[i] = random();
		}
		for (i=0; i<MAXDIM; ++i) {
			iarr1[i] = iarr2[i] = (int) LR_IRAND_IMAX & random();
		}
		(void) clocker(&t0, _SET);
/* - */
		for (i=0; i<MAXDIM; ++i) {
			iarr1[i] = (iarr1[i] - LR_IRAND_IMAX2) - LR_IRAND_IMAX2;
		}
		it1[j] = clocker(&t0, _RESET);
		for (i=0; i<MAXDIM; ++i) {
			arr1[i] = (arr1[i] - LR_IRAND_LMAX2) - LR_IRAND_LMAX2;
		}
		t1[j] = clocker(&t0, _RESET);
/* ^ */
		for (i=0; i<MAXDIM; ++i) {
			iarr2[i] = (iarr2[i] ^ LR_IRAND_INOT);
		}
		it2[j] = clocker(&t0, _RESET);
		for (i=0; i<MAXDIM; ++i) {
			arr2[i] = (arr2[i] ^ LR_IRAND_LNOT);
		}
		t2[j] = clocker(&t0, _RESET);
/* flag if different */
		for (i=0; i<MAXDIM; ++i) {
			if (iarr1[i] != iarr2[i])	iflag++;
			if (arr1[i] != arr2[i])		lflag++;
		}
	}
/* find average values */
	t1a = tave(it1, MAXTRY);
	t2a = tave(it2, MAXTRY);
	if (iflag || t2a >= t1a)
		fprintf(conftest,"#define LR_ISUB 1\n");
	else
		fprintf(conftest,"#define LR_NOISUB 1\n");
	fprintf(stderr,"i:-m2 = %f, ^m = %f, flag = %d\n", t1a, t2a, iflag);

	t1a = tave(t1, MAXTRY);
	t2a = tave(t2, MAXTRY);
	if (lflag || t2a >= t1a)
		fprintf(conftest,"#define LR_LSUB 1\n");
	else
		fprintf(conftest,"#define LR_NOLSUB 1\n");
	fprintf(stderr,"l:-m2 = %f, ^m = %f, flag = %d\n", t1a, t2a, lflag);

/* time if "/2" as fast as ">>1" */
	iflag = lflag = 0;
	for (j = 0; j < MAXTRY; ++j) {
/* initialize arrays */
		for (i=0; i<MAXDIM; ++i) {
			arr1[i] = arr2[i] = random();
		}
		for (i=0; i<MAXDIM; ++i) {
			iarr1[i] = iarr2[i] = (int) LR_IRAND_IMAX & random();
		}
		(void) clocker(&t0, _SET);
/* /2 */
		for (i=0; i<MAXDIM; ++i) {
			iarr1[i] = (iarr1[i] /2);
		}
		it1[j] = clocker(&t0, _RESET);
		for (i=0; i<MAXDIM; ++i) {
			arr1[i] = (arr1[i] /2);
		}
		t1[j] = clocker(&t0, _RESET);
/* >>1 */
		for (i=0; i<MAXDIM; ++i) {
			iarr2[i] = (iarr2[i] >>1);
		}
		it2[j] = clocker(&t0, _RESET);
		for (i=0; i<MAXDIM; ++i) {
			arr2[i] = (arr2[i] >>1);
		}
		t2[j] = clocker(&t0, _RESET);
/* flag if different */
		for (i=0; i<MAXDIM; ++i) {
			if (iarr1[i] != iarr2[i])	iflag++;
			if (arr1[i] != arr2[i])		lflag++;
		}
	}
/* find average values */
	t1a = tave(it1, MAXTRY);
	t2a = tave(it2, MAXTRY);
	if (iflag || t2a >= t1a)
		fprintf(conftest,"#define LR_IDIV 1\n");
	else
		fprintf(conftest,"#define LR_NOIDIV 1\n");
	fprintf(stderr,"i:/2 = %f, >>1 = %f, flag = %d\n", t1a, t2a, iflag);

	t1a = tave(t1, MAXTRY);
	t2a = tave(t2, MAXTRY);
	if (lflag || t2a >= t1a)
		fprintf(conftest,"#define LR_LDIV 1\n");
	else
		fprintf(conftest,"#define LR_NOLDIV 1\n");
	fprintf(stderr,"l:/2 = %f, >>1 = %f, flag = %d\n", t1a, t2a, lflag);

	return 0;
}
]])],[
AC_MSG_RESULT(timed operations)
],[AC_MSG_FAILURE(Failed to determine LR_IRAND add/sub/div timimgs)],
[AC_MSG_FAILURE(Can not cross compile LR_IRAND add/sub/div timimgs)]
)

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
