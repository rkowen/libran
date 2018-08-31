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
 *  LR_irand()		- returns int       in range [0,LR_IRAND_IMAX]
 *  LR_lrand()		- returns long      in range [0,LR_IRAND_LMAX]
 *  LR_frand()		- returns float     in range [0,1.0)
 *  LR_drand()		- returns double    in range [0,1.0)
 *  LR_isetseed(int X)	- sets the seed to X
 *  LR_lsetseed(long X)	- sets the seed to X
 *  LR_igetseed()	- gets the current seed
 *  LR_lgetseed()	- gets the current seed
 *  LR_igetrand()	- returns last value returned from irand()
 *  LR_lgetrand()	- returns last value returned from lrand()
 *  LR_fgetrand()	- returns last value returned from frand()
 *  LR_dgetrand()	- returns last value returned from drand()
 */
/*
 *********************************************************************
 *
 *     This software is copyrighted by R.K.Owen,Ph.D. 1996,2018
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
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* global variables */
int	lr_iy = 0, lr_iy0 = 0;
long	lr_ly = 0, lr_ly0 = 0;
float	lr_fscale, lr_fhalfm;
double	lr_dscale, lr_dhalfm;
int	lr_ia = 0, lr_ic = 0, lr_imc = 0;
long	lr_la = 0, lr_lc = 0, lr_lmc = 0;

#define SQRT3 1.7320508075688772935
#define PI 3.141592653589793238462643

void _set_rand(void) {
	extern float lr_fscale, lr_fhalfm;
	extern double lr_dscale, lr_dhalfm;
	extern int lr_ia, lr_ic, lr_imc;
	extern long lr_la, lr_lc, lr_lmc;

	lr_fhalfm = (float) LR_IRAND_IMAX2;
	lr_dhalfm = (double) LR_IRAND_LMAX2;

/*  COMPUTE MULTIPLIER AND INCREMENT FOR LINEAR CONGRUENTIAL METHOD */
	lr_ia = ((int) (lr_fhalfm * PI / 32.) << 3) + 5;
	lr_ic = ((int) (lr_fhalfm * (.5 - SQRT3 / 6.)) << 1) + 1;
	lr_imc = (LR_IRAND_IMAX2 - lr_ic) + LR_IRAND_IMAX2;
	lr_la = ((long) (lr_dhalfm * PI / 32.) << 3) + 5;
	lr_lc = ((long) (lr_dhalfm * (.5 - SQRT3 / 6.)) << 1) + 1;
	lr_lmc = (LR_IRAND_LMAX2 - lr_lc) + LR_IRAND_LMAX2;

/*  SCALE FACTOR FOR CONVERTING TO FLOATING POINT */
	lr_fscale = .5 / lr_fhalfm;
	lr_dscale = .5 / lr_dhalfm;
}

/* lower precision int/float */
int LR_irand(void) {
	extern int lr_iy, lr_iy0;
	extern float lr_fscale, lr_fhalfm;
	extern int lr_ia, lr_ic, lr_imc;

/*  IF FIRST ENTRY, COMPUTE URAND CONSTANTS */
	if (lr_ia == 0) _set_rand();

/*  COMPUTE NEXT RANDOM NUMBER */
	lr_iy *= lr_ia;
/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHICH DO NOT ALLOW */
/*  INTEGER OVERFLOW ON ADDITION */
#ifndef LR_NOINTOVF
	if (lr_iy > lr_imc) {
#ifdef LR_ISUB
		lr_iy -= LR_IRAND_IMAX2;
		lr_iy -= LR_IRAND_IMAX2;
#elif defined LR_NOISUB
		lr_iy ^= LR_IRAND_INOT;
#else
#  error "Need either LR_ISUB or LR_NOISUB to be defined"
#endif
	}
#endif
	lr_iy += lr_ic;

/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHERE THE */
/*  WORD LENGTH FOR ADDITION IS GREATER THAN FOR MULTIPLICATION */
#ifndef NOADDMULT
	if (
#ifdef LR_IDIV
		(lr_iy/2)
#elif defined LR_NOIDIV
		(lr_iy >> 1)
#else
#  error "Need either LR_IDIV or LR_NOIDIV to be defined"
#endif
	> LR_IRAND_IMAX2) {
#ifdef LR_ISUB
		lr_iy -= LR_IRAND_IMAX2;
		lr_iy -= LR_IRAND_IMAX2;
#elif defined LR_NOISUB
		lr_iy ^= LR_IRAND_INOT;
#else
#  error "Need either LR_ISUB or LR_NOISUB to be defined"
#endif
	}
#endif

/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHERE INTEGER */
/*  OVERFLOW AFFECTS THE SIGN BIT */
	if (lr_iy < 0) {
#ifdef LR_IADD
		lr_iy += LR_IRAND_IMAX2;
		lr_iy += LR_IRAND_IMAX2;
#elif defined LR_NOIADD
		lr_iy ^= LR_IRAND_INOT;
#else
#  error "Need either LR_IADD or LR_NOIADD to be defined"
#endif
	}
	return lr_iy;
}

float LR_frand(void) {
	extern int lr_iy;
	extern float lr_fscale;

/*  COMPUTE NEXT RANDOM NUMBER */
	LR_irand();

	return (float) (lr_iy) * lr_fscale;
}

void LR_isetseed(int ity) {
	extern int lr_iy, lr_iy0;
	extern int lr_ia;

/*  IF FIRST ENTRY, COMPUTE URAND CONSTANTS */
	if (lr_ia == 0) _set_rand();

	lr_iy = lr_iy0 = ity;
}

int LR_igetseed(void) {
	extern int lr_iy0;
	return lr_iy0;
}

int LR_igetrand(void) {
	extern int lr_iy;
	return lr_iy;
}

float LR_fgetrand(void) {
	extern int lr_iy;
	extern float lr_fscale;
	return (float) (lr_iy) * lr_fscale;
}

/* higher precision long/double */
long LR_lrand(void) {
	extern long lr_ly, lr_ly0;
	extern double lr_dscale, lr_dhalfm;
	extern long lr_la, lr_lc, lr_lmc;

/*  IF FIRST ENTRY, COMPUTE URAND CONSTANTS */
	if (lr_la == 0) _set_rand();

/*  COMPUTE NEXT RANDOM NUMBER */
	lr_ly *= lr_la;
/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHICH DO NOT ALLOW */
/*  INTEGER OVERFLOW ON ADDITION */
#ifndef LR_NOINTOVF
	if (lr_ly > lr_lmc) {
#ifdef LR_ISUB
		lr_ly -= LR_IRAND_LMAX2;
		lr_ly -= LR_IRAND_LMAX2;
#elif defined LR_NOISUB
		lr_ly ^= LR_IRAND_LNOT;
#else
#  error "Need either LR_ISUB or LR_NOISUB to be defined"
#endif
	}
#endif
	lr_ly += lr_lc;

/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHERE THE */
/*  WORD LENGTH FOR ADDITION IS GREATER THAN FOR MULTIPLICATION */
#ifndef NOADDMULT
	if (
#ifdef LR_IDIV
		(lr_ly/2)
#elif defined LR_NOIDIV
		(lr_ly >> 1)
#else
#  error "Need either LR_IDIV or LR_NOIDIV to be defined"
#endif
	> LR_IRAND_LMAX2) {
#ifdef LR_ISUB
		lr_ly -= LR_IRAND_LMAX2;
		lr_ly -= LR_IRAND_LMAX2;
#elif defined LR_NOISUB
		lr_ly ^= LR_IRAND_LNOT;
#else
#  error "Need either LR_ISUB or LR_NOISUB to be defined"
#endif
	}
#endif

/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHERE INTEGER */
/*  OVERFLOW AFFECTS THE SIGN BIT */
	if (lr_ly < 0) {
#ifdef LR_LADD
		lr_ly += LR_IRAND_LMAX2;
		lr_ly += LR_IRAND_LMAX2;
#elif defined LR_NOLADD
		lr_ly ^= LR_IRAND_LNOT;
#else
#  error "Need either LR_LADD or LR_NOIADD to be defined"
#endif
	}
	return lr_ly;
}

double LR_drand(void) {
	extern long lr_ly;
	extern double lr_dscale;

/*  COMPUTE NEXT RANDOM NUMBER */
	LR_lrand();

	return (double) (lr_ly) * lr_dscale;
}

void LR_lsetseed(long lty) {
	extern long lr_ly, lr_ly0;
	extern long lr_la;

/*  IF FIRST ENTRY, COMPUTE URAND CONSTANTS */
	if (lr_la == 0) _set_rand();

	lr_ly = lr_ly0 = lty;
}

long LR_lgetseed(void) {
	extern long lr_ly0;
	return lr_ly0;
}

long LR_lgetrand(void) {
	extern long lr_ly;
	return lr_ly;
}

double LR_dgetrand(void) {
	extern long lr_ly;
	extern double lr_dscale;
	return (double) (lr_ly) * lr_dscale;
}

#ifdef __cplusplus
}
#endif
