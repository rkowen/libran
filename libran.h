/*!	\file libran.h
	\brief The LibRan common header file
*/

#ifndef 	_LIBRAN_H_
#  define	_LIBRAN_H

#  include <stdarg.h>

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

/*!
\struct LR_bin - the binning object
*/
typedef struct {
	int		n;	/**< n - number of bins */
	int		nn;	/**< nn - number of bins declared */
	long		c;	/**< c - count of values */
	double *	bdrs;	/**< bdrs - set of bin boundaries (n - 1) */
	long *		bins;	/**< bins - set of bins (n) */
}	LR_bin;

/* LibRan function declarations */
LR_obj *LR_new(LR_type t, LR_data_type d);
int LR_rm(LR_obj **o);

/* LibRan object parameter setting */
int LR_vset(LR_obj *o, char *x, va_list ap);
int LR_set(LR_obj *o, char x, ...);
int LR_set_all(LR_obj *o, char *x, ...);

/* LibRan binning object */
LR_bin *LR_bin_new(int n);
int LR_bin_rm(LR_bin **b);
int LR_bin_set(LR_bin *b, double x);
int LR_bin_add(LR_bin *b, double x);

/* double unif */
double LRd_unif_RAN(LR_obj *o);
double LRd_unif_PDF(LR_obj *o, double x);
double LRd_unif_CDF(LR_obj *o, double x);
/* float unif */
double LRf_unif_RAN(LR_obj *o);
double LRf_unif_PDF(LR_obj *o, float x);
double LRf_unif_CDF(LR_obj *o, float x);

#endif		/* _LIBRAN_H_ */
