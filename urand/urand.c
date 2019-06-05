static const char AUTHOR[]="@(#)urand 06/02/94 1.3 R.K.Owen,PhD";
static const char RCSID[]="$Id: urand.c 106 2002-02-10 08:02:39Z rk $";
/*****
 ******************************************************************************
 LibRan/Urand - 
	Urand is a specialized sequential congruential uniform
	pseudo-random number generator based on theory and
	suggestions given in D.E. Knuth (1969), vol 2.  and then
	republished by G.E. Forsysth, M.A. Malcolm, and C.B. Moler (1977);
	and converted from Fortran 77 to C with f2c.

	Modified to fit into the LibRan package, and to isolate each
	pseudo-random sequence to each LibRan object.

 Copyright 2018 by R.K. Owen, Ph.D.
 See LICENSE.LGPL, which must be provided, for details.

 ******************************************************************************
 *****/
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
 *  LR_igetval()	- returns URAND int  configuration values
 *  LR_lgetval()	- returns URAND long configuration values
 */

#include "config.h"
#include "libran.h"	/* LR_obj, LR_val */
#include <string.h>	/* strcmp */

#ifdef __cplusplus
extern "C" {
#endif

/* global variables */
/* random sequence values */
/*
int	lr_iy = 0, lr_iy0 = 0;
long	lr_ly = 0, lr_ly0 = 0;
*/
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
int LR_irand(LR_obj *o) {
	/* extern int lr_iy, lr_iy0; */
	extern float lr_fscale, lr_fhalfm;
	extern int lr_ia, lr_ic, lr_imc;

/*  IF FIRST ENTRY, COMPUTE URAND CONSTANTS */
	if (lr_ia == 0) _set_rand();

/*  COMPUTE NEXT RANDOM NUMBER */
	o->iy.i *= lr_ia;
/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHICH DO NOT ALLOW */
/*  INTEGER OVERFLOW ON ADDITION */
#ifndef LR_NOINTOVF
	if (o->iy.i > lr_imc) {
#ifdef LR_ISUB
		o->iy.i -= LR_IRAND_IMAX2;
		o->iy.i -= LR_IRAND_IMAX2;
#elif defined LR_NOISUB
		o->iy.i ^= LR_IRAND_INOT;
#else
#  error "Need either LR_ISUB or LR_NOISUB to be defined"
#endif
	}
#endif
	o->iy.i += lr_ic;

/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHERE THE */
/*  WORD LENGTH FOR ADDITION IS GREATER THAN FOR MULTIPLICATION */
#ifndef NOADDMULT
	if (
#  ifdef LR_IDIV
		(o->iy.i/2)
#  elif defined LR_NOIDIV
		(o->iy.i >> 1)
#  else
#    error "Need either LR_IDIV or LR_NOIDIV to be defined"
#  endif
	> LR_IRAND_IMAX2) {
#  ifdef LR_ISUB
		o->iy.i -= LR_IRAND_IMAX2;
		o->iy.i -= LR_IRAND_IMAX2;
#  elif defined LR_NOISUB
		o->iy.i ^= LR_IRAND_INOT;
#  else
#    error "Need either LR_ISUB or LR_NOISUB to be defined"
#  endif
	}
#endif

/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHERE INTEGER */
/*  OVERFLOW AFFECTS THE SIGN BIT */
	if (o->iy.i < 0) {
#ifdef LR_IADD
		o->iy.i += LR_IRAND_IMAX2;
		o->iy.i += LR_IRAND_IMAX2;
#elif defined LR_NOIADD
		o->iy.i ^= LR_IRAND_INOT;
#else
#  error "Need either LR_IADD or LR_NOIADD to be defined"
#endif
	}
	return o->iy.i;
}

float LR_frand(LR_obj *o) {
	/* extern int lr_iy; */
	extern float lr_fscale;

/*  COMPUTE NEXT RANDOM NUMBER */
	LR_irand(o);

	return (float) (o->iy.i) * lr_fscale;
}

void LR_isetseed(LR_obj *o, int ity) {
	/* extern int lr_iy, lr_iy0; */
	extern int lr_ia;

/*  IF FIRST ENTRY, COMPUTE URAND CONSTANTS */
	if (lr_ia == 0) _set_rand();

	o->iy.i = o->iy0.i = ity;
}

int LR_igetseed(LR_obj *o) {
	/* extern int lr_iy0; */
	return o->iy0.i;
}

int LR_igetrand(LR_obj *o) {
	/* extern int lr_iy; */
	return o->iy.i;
}

float LR_fgetrand(LR_obj *o) {
	/* extern int lr_iy; */
	extern float lr_fscale;
	return (float) (o->iy.i) * lr_fscale;
}

/* higher precision long/double */
long LR_lrand(LR_obj *o) {
	/* extern long lr_ly, lr_ly0; */
	extern double lr_dscale, lr_dhalfm;
	extern long lr_la, lr_lc, lr_lmc;

/*  IF FIRST ENTRY, COMPUTE URAND CONSTANTS */
	if (lr_la == 0) _set_rand();

/*  COMPUTE NEXT RANDOM NUMBER */
	o->iy.l *= lr_la;
/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHICH DO NOT ALLOW */
/*  INTEGER OVERFLOW ON ADDITION */
#ifndef LR_NOINTOVF
	if (o->iy.l > lr_lmc) {
#ifdef LR_ISUB
		o->iy.l -= LR_IRAND_LMAX2;
		o->iy.l -= LR_IRAND_LMAX2;
#elif defined LR_NOISUB
		o->iy.l ^= LR_IRAND_LNOT;
#else
#  error "Need either LR_ISUB or LR_NOISUB to be defined"
#endif
	}
#endif
	o->iy.l += lr_lc;

/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHERE THE */
/*  WORD LENGTH FOR ADDITION IS GREATER THAN FOR MULTIPLICATION */
#ifndef NOADDMULT
	if (
#ifdef LR_IDIV
		(o->iy.l/2)
#elif defined LR_NOIDIV
		(o->iy.l >> 1)
#else
#  error "Need either LR_IDIV or LR_NOIDIV to be defined"
#endif
	> LR_IRAND_LMAX2) {
#ifdef LR_ISUB
		o->iy.l -= LR_IRAND_LMAX2;
		o->iy.l -= LR_IRAND_LMAX2;
#elif defined LR_NOISUB
		o->iy.l ^= LR_IRAND_LNOT;
#else
#  error "Need either LR_ISUB or LR_NOISUB to be defined"
#endif
	}
#endif

/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHERE INTEGER */
/*  OVERFLOW AFFECTS THE SIGN BIT */
	if (o->iy.l < 0) {
#ifdef LR_LADD
		o->iy.l += LR_IRAND_LMAX2;
		o->iy.l += LR_IRAND_LMAX2;
#elif defined LR_NOLADD
		o->iy.l ^= LR_IRAND_LNOT;
#else
#  error "Need either LR_LADD or LR_NOIADD to be defined"
#endif
	}
	return o->iy.l;
}

double LR_drand(LR_obj *o) {
	/* extern long lr_ly; */
	extern double lr_dscale;

/*  COMPUTE NEXT RANDOM NUMBER */
	LR_lrand(o);

	return (double) (o->iy.l) * lr_dscale;
}

void LR_lsetseed(LR_obj *o, long lty) {
	/* extern long lr_ly, lr_ly0; */
	extern long lr_la;

/*  IF FIRST ENTRY, COMPUTE URAND CONSTANTS */
	if (lr_la == 0) _set_rand();

	o->iy.l = o->iy0.l = lty;
}

long LR_lgetseed(LR_obj *o) {
	/* extern long lr_ly0; */
	return o->iy0.l;
}

long LR_lgetrand(LR_obj *o) {
	/* extern long lr_ly; */
	return o->iy.l;
}

double LR_dgetrand(LR_obj *o) {
	/* extern long lr_ly; */
	extern double lr_dscale;
	return (double) (o->iy.l) * lr_dscale;
}

int LR_igetval(char *val) {
	if (!strcmp(val, "LR_IRAND_IMAX2"))
		return	LR_IRAND_IMAX2;
	else if (!strcmp(val, "LR_IRAND_IMAX"))
		return	LR_IRAND_IMAX;
	else if (!strcmp(val, "LR_IRAND_INOT"))
		return	LR_IRAND_INOT;
	return 0;
}

long LR_lgetval(char *val) {
	if (!strcmp(val, "LR_IRAND_LMAX2"))
		return	LR_IRAND_LMAX2;
	else if (!strcmp(val, "LR_IRAND_LMAX"))
		return	LR_IRAND_LMAX;
	else if (!strcmp(val, "LR_IRAND_LNOT"))
		return	LR_IRAND_LNOT;
	return 0;
}

#ifdef __cplusplus
}
#endif
