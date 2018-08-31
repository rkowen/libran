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
 *
 *  extern INTEGER IRAND_MAX	- the maximum value returned by irand
 *				  it's only defined after a call to
 *				  urand(), irand(), or setseed().
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

#ifndef	_URAND_H_
#  define _URAND_H_

int	LR_irand(void);
float	LR_frand(void);
void	LR_isetseed(int ity);
int	LR_igetseed(void);
int	LR_igetrand(void);
float	LR_fgetrand(void);

#endif	/* _URAND_H_ */
