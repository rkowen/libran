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
	piece,	/**< piece	- piecewise uniform */
	gaus,	/**< guas	- Gaussian */
	gsn2,	/**< gsn2	- Gaussian like - 2 uni (saw tooth) */
	gsn4,	/**< gsn4	- Gaussian like - 4 uni */
	gsn12,	/**< gsn12	- Gaussian like - 12 uni */
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
/* tag & predefine the fns object */
typedef struct LR_obj LR_obj;

struct LR_obj {
	const char const *	type;	/**< named distribution type */
	LR_type		t;	/**< t - distribution type */
	LR_data_type	d;	/**< d - data type */
	LR_val		a;	/**< a - lower bound */
	LR_val		b;	/**< b - upper bound */
	LR_val		m;	/**< m - middle value of distribution */
	LR_val		s;	/**< s - measure of distribution width */
	LR_val		x;	/**< x - auxiliary value */
	/**< set of uniform random number generators - one for each data type */
	int	(*ui)(void);	/**< ui - int */
	long	(*ul)(void);	/**< ul - long */
	float	(*uf)(void);	/**< uf - float */
	double	(*ud)(void);	/**< ud - double */
	/**< set of Random Fns for this distribution type */
	float	(*rnf)(LR_obj *);		/**< rnf - float */
	double	(*rnd)(LR_obj *);		/**< rnd - double */
	/**< set of PDFs for this distribution type */
	float	(*pdff)(LR_obj *, float);	/**< pdff - float */
	double	(*pdfd)(LR_obj *, double);	/**< pdfd - double */
	/**< set of CDFs for this distribution type  */
	float	(*cdff)(LR_obj *, float);	/**< cdff - float */
	double	(*cdfd)(LR_obj *, double);	/**< cdfd - double */
	/**< generic (void) pointer to some other object */
	void *	aux;				/**< aux - auxiliary object */
};

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

/*!
\struct LR_pcs - the piecewise uniform object
*/
typedef struct {
	int		n;	/**< n - number of bins */
	int		nn;	/**< nn - number of bins declared */
	long		c;	/**< c - count of values */
	double *	bdrs;	/**< bdrs - set of bin boundaries (n - 1) */
	long *		bins;	/**< bins - set of bins (n) */
}	LR_pcs;

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

/* LibRan generic distribution functions */
/* double */
double LRd_RAN(LR_obj *o);
double LRd_PDF(LR_obj *o, double x);
double LRd_CDF(LR_obj *o, double x);
/* float */
float LRf_RAN(LR_obj *o);
float LRf_PDF(LR_obj *o, float x);
float LRf_CDF(LR_obj *o, float x);

/* specific LibRan distribution functions */
/* double unif */
double LRd_unif_RAN(LR_obj *o);
double LRd_unif_PDF(LR_obj *o, double x);
double LRd_unif_CDF(LR_obj *o, double x);
/* float unif */
float LRf_unif_RAN(LR_obj *o);
float LRf_unif_PDF(LR_obj *o, float x);
float LRf_unif_CDF(LR_obj *o, float x);

/* double gsn2 */
double LRd_gsn2_RAN(LR_obj *o);
double LRd_gsn2_PDF(LR_obj *o, double x);
double LRd_gsn2_CDF(LR_obj *o, double x);
/* float gsn2 */
float LRf_gsn2_RAN(LR_obj *o);
float LRf_gsn2_PDF(LR_obj *o, float x);
float LRf_gsn2_CDF(LR_obj *o, float x);

#endif		/* _LIBRAN_H_ */
