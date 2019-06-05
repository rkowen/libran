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
 *
 *  extern INTEGER IRAND_MAX	- the maximum value returned by irand
 *				  it's only defined after a call to
 *				  urand(), irand(), or setseed().
 */

#ifndef	_URAND_H_
#  define _URAND_H_

#  include "libran.h"

int	LR_irand(LR_obj *);
float	LR_frand(LR_obj *);
void	LR_isetseed(LR_obj *, int ity);
int	LR_igetseed(LR_obj *);
int	LR_igetrand(LR_obj *);
float	LR_fgetrand(LR_obj *);

long	LR_lrand(LR_obj *);
double	LR_drand(LR_obj *);
void	LR_lsetseed(LR_obj *, long lty);
long	LR_lgetseed(LR_obj *);
long	LR_lgetrand(LR_obj *);
double	LR_dgetrand(LR_obj *);

int	LR_igetval(char *str);
long	LR_lgetval(char *str);
#endif	/* _URAND_H_ */
