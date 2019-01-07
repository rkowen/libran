/*
 * libran common header file
 */

#ifndef 	_LIBRAN_H_
#  define	_LIBRAN_H

#  include	<stdio.h>
#  include	<stdarg.h>

typedef	enum {
/* discrete */
	poiss,	/* Poison */
	binom,	/* Binomial */
/* continuum */
	unif,	/* uniform */
	gaus,	/* Gaussian */
	g2,	/* Gaussian like - 2 uni (saw tooth) */
	g4,	/* Gaussian like - 4 uni */
	g12,	/* Gaussian like - 12 uni */
	log,	/* logirithmic */
	nexp,	/* negative exponential */
	cauchy	/* Cauchy */
}	LR_type;

typedef	union {
	long	l;
	int	i;
	float	f;
	double	d;
}	LR_val;

typedef enum {
	LR_int,
	LR_long,
	LR_float,
	LR_double
}	LR_data_type;

typedef struct {
	LR_type		t;
	LR_data_type	d;
	LR_val		a;	/* lower bound */
	LR_val		b;	/* upper bound */
	LR_val		m;	/* middle value of distribution */
	LR_val		s;	/* measure of distribution width */
}	LR_obj;

#endif		/* _LIBRAN_H_ */
