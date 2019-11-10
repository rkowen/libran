static const char AUTHOR[]="@(#)urand 06/02/94 1.3 R.K.Owen,PhD";
static const char RCSID[]="$Id: urand.c 106 2002-02-10 08:02:39Z rk $";
/*
 Copyright 2019 by R.K. Owen, Ph.D.
 See LICENSE.LGPL, which must be provided, for details.
*/
/*!
\file	urand.c
\brief	Specialized sequential congruential uniform pseudo-random number generator

	Urand is a specialized sequential congruential uniform
	pseudo-random number generator based on theory and
	suggestions given in D.E. Knuth (1969), vol 2.  and then
	republished by G.E. Forsysth, M.A. Malcolm, and C.B. Moler (1977);
	and converted from Fortran 77 to C with f2c.

	Modified to fit into the LibRan package, and to isolate each
	pseudo-random sequence to each LibRan object.

	This routine is highly optimized by the configuration step to
	provide the fastest set of operations for computing the next
	value in the sequence.  However, this does not lend itself to
	cross compiling, since the configuration step requires testing
	on the actual intended hardware and environment.

The \c LR_*rand routine should generally not be called directly ... call
the \c LR_obj object with the \e unif distribution instead as follows.

\code
#include "libran.h"
...
LR_obj *o = LR_new(unif, LR_double);
// set the seed
LR_lsetseed(19580512L);
...
// generate random number
x = LR_RAN(o);
// get the same random number
y = LR_lgetrand(o);
...
// remove object
LR_rm(&o);
...
\endcode

These routines are specific to this internal LibRan
sequential congruential uniform pseudo-random generator.
If a different pseudo-random generator is used then these are probably
no longer effective.

 -  LR_irand()		- returns int       in range [0,LR_IRAND_IMAX]
 -  LR_lrand()		- returns long      in range [0,LR_IRAND_LMAX]
 -  LR_frand()		- returns float     in range [0,1.0)
 -  LR_drand()		- returns double    in range [0,1.0)
 -  LR_isetseed()	- sets the seed to X
 -  LR_lsetseed()	- sets the seed to X
 -  LR_igetseed()	- gets the current seed
 -  LR_lgetseed()	- gets the current seed
 -  LR_igetrand()	- returns last value returned from irand()
 -  LR_lgetrand()	- returns last value returned from lrand()
 -  LR_fgetrand()	- returns last value returned from frand()
 -  LR_dgetrand()	- returns last value returned from drand()
 -  LR_igetval()	- returns URAND int  configuration values
 -  LR_lgetval()	- returns URAND long configuration values
 -  LR_fgetval()	- returns URAND float  configuration values
 -  LR_dgetval()	- returns URAND double configuration values
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "libran.h"	/* LR_obj, LR_val */
#include <string.h>	/* strcmp */

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

/*!
@brief	LR_irand(LR_obj *) - returns int       in range [0,LR_IRAND_IMAX]

Returns a pseudo-random \e int in the range [0,LR_IRAND_MAX]

@param	o	LR_obj object
@return int	range [0, LR_IRAND_IMAX]
*/
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

/*!
@brief	LR_frand(LR_obj *) - returns float       in range [0.0,1.0)

Returns a pseudo-random \e float in the range [0.0, 1.0)

@param	o	LR_obj object
@return float	range [0.0, 1.0)
*/
float LR_frand(LR_obj *o) {
	/* extern int lr_iy; */
	extern float lr_fscale;

/*  COMPUTE NEXT RANDOM NUMBER */
	LR_irand(o);

	return (float) (o->iy.i) * lr_fscale;
}

/*!
@brief	LR_isetseed(LR_obj *, int X) - set the seed to X

Set the seed for this \c LR_obj .

@param	o	LR_obj object
@param	X	int with new seed
@return void
*/
void LR_isetseed(LR_obj *o, int X) {
	/* extern int lr_iy, lr_iy0; */
	extern int lr_ia;

/*  IF FIRST ENTRY, COMPUTE URAND CONSTANTS */
	if (lr_ia == 0) _set_rand();

	o->iy.i = o->iy0.i = X;
}

/*!
@brief	LR_igetseed(LR_obj *) - get the current seed

Get the seed from this \c LR_obj .

@param	o	LR_obj object
@return int	current object seed
*/
int LR_igetseed(LR_obj *o) {
	/* extern int lr_iy0; */
	return o->iy0.i;
}

/*!
@brief	LR_igetrand(LR_obj *) - get the current pseudo-random number

Get the current pseudo-random number from this \c LR_obj .

@param	o	LR_obj object
@return int	current pseudo-random number
*/
int LR_igetrand(LR_obj *o) {
	/* extern int lr_iy; */
	return o->iy.i;
}

/*!
@brief	LR_fgetrand(LR_obj *) - get the current pseudo-random number

Get the current pseudo-random number from this \c LR_obj .

@param	o	LR_obj object
@return float	current pseudo-random number
*/
float LR_fgetrand(LR_obj *o) {
	/* extern int lr_iy; */
	extern float lr_fscale;
	return (float) (o->iy.i) * lr_fscale;
}

/*!
@brief	LR_lrand(LR_obj *) - returns long       in range [0,LR_IRAND_LMAX]

Returns a pseudo-random \e int in the range [0,LR_IRAND_LMAX]

@param	o	LR_obj object
@return int	range [0, LR_IRAND_LMAX]
*/
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

/*!
@brief	LR_drand(LR_obj *) - returns double       in range [0.0,1.0)

Returns a pseudo-random \e double in the range [0.0, 1.0)

@param	o	LR_obj object
@return double	range [0.0, 1.0)
*/
double LR_drand(LR_obj *o) {
	/* extern long lr_ly; */
	extern double lr_dscale;

/*  COMPUTE NEXT RANDOM NUMBER */
	LR_lrand(o);

	return (double) (o->iy.l) * lr_dscale;
}

/*!
@brief	LR_lsetseed(LR_obj *, long X) - set the seed to X

Set the seed for this \c LR_obj .

@param	o	LR_obj object
@param	X	long with new seed
@return void
*/
void LR_lsetseed(LR_obj *o, long X) {
	/* extern long lr_ly, lr_ly0; */
	extern long lr_la;

/*  IF FIRST ENTRY, COMPUTE URAND CONSTANTS */
	if (lr_la == 0) _set_rand();

	o->iy.l = o->iy0.l = X;
}

/*!
@brief	LR_lgetseed(LR_obj *) - get the current seed

Get the seed from this \c LR_obj .

@param	o	LR_obj object
@return long	current object seed
*/
long LR_lgetseed(LR_obj *o) {
	/* extern long lr_ly0; */
	return o->iy0.l;
}

/*!
@brief	LR_lgetrand(LR_obj *) - get the current pseudo-random number

Get the current pseudo-random number from this \c LR_obj .

@param	o	LR_obj object
@return long	current pseudo-random number
*/
long LR_lgetrand(LR_obj *o) {
	/* extern long lr_ly; */
	return o->iy.l;
}

/*!
@brief	LR_dgetrand(LR_obj *) - get the current pseudo-random number

Get the current pseudo-random number from this \c LR_obj .

@param	o	LR_obj object
@return double	current pseudo-random number
*/
double LR_dgetrand(LR_obj *o) {
	/* extern long lr_ly; */
	extern double lr_dscale;
	return (double) (o->iy.l) * lr_dscale;
}

/*!
@brief	LR_igetval(char *val) - return the configure value

Return the value computed from the library configuration step, such as
\c LR_IRAND_IMAX2 , \c LR_IRAND_IMAX , and \c LR_IRAND_INOT .

\see urand/config.h

@param	val	configuration parameter
@return int	configuration value
*/
int LR_igetval(char *val) {
	if (!strcmp(val, "LR_IRAND_IMAX2"))
		return	LR_IRAND_IMAX2;
	else if (!strcmp(val, "LR_IRAND_IMAX"))
		return	LR_IRAND_IMAX;
	else if (!strcmp(val, "LR_IRAND_INOT"))
		return	LR_IRAND_INOT;
	return 0;
}

/*!
@brief	LR_lgetval(char *val) - return the configure value

Return the value computed from the library configuration step, such as
\c LR_IRAND_LMAX2 , \c LR_IRAND_LMAX , and \c LR_IRAND_LNOT .

\see urand/config.h

@param	val	configuration parameter
@return long	configuration value
*/
long LR_lgetval(char *val) {
	if (!strcmp(val, "LR_IRAND_LMAX2"))
		return	LR_IRAND_LMAX2;
	else if (!strcmp(val, "LR_IRAND_LMAX"))
		return	LR_IRAND_LMAX;
	else if (!strcmp(val, "LR_IRAND_LNOT"))
		return	LR_IRAND_LNOT;
	return 0;
}

/*!
@brief	LR_fgetval(char *val) - return the configure value

Return the value computed from the library configuration step, such as
\c LR_FEPS , and \c LR_FSQEPS .

\see urand/config.h

@param	val	configuration parameter
@return float	configuration value
*/
float LR_fgetval(char *val) {
	if (!strcmp(val, "LR_FEPS"))
		return	LR_FEPS;
	else if (!strcmp(val, "LR_FSQEPS"))
		return	LR_FSQEPS;
	return 0;
}

/*!
@brief	LR_dgetval(char *val) - return the configure value

Return the value computed from the library configuration step, such as
\c LR_DEPS , and \c LR_DSQEPS .

\see urand/config.h

@param	val	configuration parameter
@return double	configuration value
*/
double LR_dgetval(char *val) {
	if (!strcmp(val, "LR_DEPS"))
		return	LR_DEPS;
	else if (!strcmp(val, "LR_DSQEPS"))
		return	LR_DSQEPS;
	return 0;
}

#ifdef __cplusplus
}
#endif
