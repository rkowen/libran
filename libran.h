/*!	\file libran.h
	\brief The LibRan common header file
*/

#ifndef 	_LIBRAN_H_
#  define	_LIBRAN_H

/*!
\enum LR_type - an enum of allowed distribution types
*/
typedef	enum {
/**< discrete distributions */
	poiss,	/**< poiss	- Poison */
	binom,	/**< binom	- Binomial */
/**< continuum distributions */
	unif,	/**< unif	- uniform */
	gaus,	/**< guas	- Gaussian */
	g2,	/**< g2		- Gaussian like - 2 uni (saw tooth) */
	g4,	/**< g4		- Gaussian like - 4 uni */
	g12,	/**< g12	- Gaussian like - 12 uni */
	logd,	/**< logd	- logirithmic */
	nexp,	/**< nexp	- negative exponential */
	cauchy	/**< cauchy	- Cauchy */
}	LR_type;

/*!
\union LR_val - spans the set of allowed value types
*/
typedef	union {
	long	l; /**< l	- long type */
	int	i; /**< i	- int type */
	float	f; /**< f	- float type */
	double	d; /**< d	- double type */
}	LR_val;

/*!
\enum LR_data_type - an enum of allowed value types
*/
typedef enum {
	LR_int,
	LR_long,
	LR_float,
	LR_double
}	LR_data_type;

/*!
\struct LR_obj - the fundamental LibRan distribution object
*/
typedef struct {
	LR_type		t;	/**< t - distribution type */
	LR_data_type	d;	/**< d - data type */
	LR_val		a;	/**< a - lower bound */
	LR_val		b;	/**< b - upper bound */
	LR_val		m;	/**< m - middle value of distribution */
	LR_val		s;	/**< s - measure of distribution width */
	/**< set of uniform random number generators - one for each data type */
	int	(*ui)(void);	/**< ui - int */
	long	(*ul)(void);	/**< ul - long */
	float	(*uf)(void);	/**< uf - float */
	double	(*ud)(void);	/**< ud - double */
}	LR_obj;

/* LibRan function declarations */
LR_obj *LR_new(LR_type t, LR_data_type d);
int LR_rm(LR_obj **o);

int LR_vset(LR_obj *o, char *x, va_list ap);
int LR_set(LR_obj *o, char x, ...);
int LR_set_all(LR_obj *o, char *x, ...);

#endif		/* _LIBRAN_H_ */
