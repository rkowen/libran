/*!	\file libran.h
	\brief The LibRan common header file
*/

#ifndef 	_LIBRAN_H_
#  define	_LIBRAN_H_

#  include <stdarg.h>

/*!
Library Version
*/
#  define LR_VERSION	"LibRan_VERSION=0.1.0";
#  define LR_DATE	"LibRan_DATE=20190512";

/*!
LibRan Error Numbers
*/
#  define LRerr_OK					0x00
#  define LRerr_Unspecified				0x01
#  define LRerr_BadDataType				0x03
#  define LRerr_BadLRType				0x05
#  define LRerr_NoAuxiliaryObject			0x09
#  define LRerr_NoAuxNormalizeDone			0x0B
#  define LRerr_BinGeneric				0x11
#  define LRerr_TooManyValues				0x13
#  define LRerr_InvalidInputValue			0x15
#  define LRerr_InvalidRange				0x17
#  define LRerr_UnmetPreconditions			0x19
#  define LRerr_SuspiciousValues			0x1B
#  define LRerr_AllocFail				0x1D

/*!
\enum LR_type - an enum of allowed distribution types
*/
typedef	enum {
/**< discrete distributions */
	poiss,		/**< poiss	- Poison */
	binom,		/**< binom	- Binomial */
/**< continuum distributions */
	unif,		/**< unif	- uniform */
	piece,		/**< piece	- piecewise uniform */
	lspline,	/**< lspline	- linear spline */
	gausbm,		/**< guas	- Gaussian (Normal) - Box Muller method*/
	gsn2,		/**< gsn2	- Gaussian like - 2 uni (saw tooth) */
	gsn4,		/**< gsn4	- Gaussian like - 4 uni */
	gsn12,		/**< gsn12	- Gaussian like - 12 uni */
	logd,		/**< logd	- logirithmic */
	nexp,		/**< nexp	- negative exponential */
	cauchy		/**< cauchy	- Cauchy */
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
	/**< object random values */
	LR_val		iy;	/**< iy  - current sequence value */
	LR_val		iy0;	/**< iy0 - initial sequence value */
	/**< set of uniform random number generators - one for each data type */
	int	(*ui)(LR_obj *);	/**< ui - int */
	long	(*ul)(LR_obj *);	/**< ul - long */
	float	(*uf)(LR_obj *);	/**< uf - float */
	double	(*ud)(LR_obj *);	/**< ud - double */
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
	void *		aux;	/**< aux - auxiliary object */
	int		errno;	/**< errno - last error encountered */
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
	int		errno;	/**< errno - last bin error encountered */
}	LR_bin;

/*!
\struct LR_pcs - the piecewise uniform object (also linear spline)
*/
typedef struct {
	int		n;	/**< n - number of intervals */
	int		nn;	/**< nn - number of intervals declared */
	double *	bdrs;	/**< bdrs - set of interval boundaries (n-1) */
	double *	c;	/**< relative probability for each interval (n) */
	double *	sc;	/**< cumulative probability for each interval (n) */
	double 		norm;	/**< norm - normalization factor for c */
	int		flags;	/**< flags to guarentee certain actions */

/**< special auxiliary methods */
	int (*new)(LR_obj *o, int n);		/**< aux new */
	int (*rm)(LR_obj *o);			/**< aux rm */
	int (*set)(LR_obj *o, double x, double p);	/**< aux set points */
	int (*normalize)(LR_obj *o);		/**< aux normalize points */
}	LR_pcs;

/**< special auxiliary flags */
#  define	LR_AUX_NORM		0x01
#  define	LR_AUX_SET		0x02

/* LibRan function declarations */
LR_obj *LR_new(LR_type t, LR_data_type d);
int LR_rm(LR_obj **o);
int LR_check(LR_obj *o);

/* LibRan object parameter setting */
int LR_vset(LR_obj *o, char *x, va_list ap);
int LR_set(LR_obj *o, char x, ...);
int LR_set_all(LR_obj *o, char *x, ...);

/* LibRan binning object */
LR_bin *LR_bin_new(int n);
int LR_bin_rm(LR_bin **b);
int LR_bin_set(LR_bin *b, double x);
int LR_bin_add(LR_bin *b, double x);

/* LibRan error routines */
char *LRstrerror(int LRerrno);
char *LRstrerrno(int LRerrno);
void LRperror(char *str, int LRerrno);

/* LibRan generic distribution functions */
/* double */
double LRd_RAN(LR_obj *o);
double LRd_PDF(LR_obj *o, double x);
double LRd_CDF(LR_obj *o, double x);
/* float */
float LRf_RAN(LR_obj *o);
float LRf_PDF(LR_obj *o, float x);
float LRf_CDF(LR_obj *o, float x);
/* LibRan generic auxiliary functions */
int LR_aux_new(LR_obj *o, int n);
int LR_aux_rm(LR_obj *o);
int LR_aux_set(LR_obj *o, double x, double p);
int LR_aux_norm(LR_obj *o);

/* specific LibRan distribution functions */
/* double unif */
double LRd_unif_RAN(LR_obj *o);
double LRd_unif_PDF(LR_obj *o, double x);
double LRd_unif_CDF(LR_obj *o, double x);
/* float unif */
float LRf_unif_RAN(LR_obj *o);
float LRf_unif_PDF(LR_obj *o, float x);
float LRf_unif_CDF(LR_obj *o, float x);

/* piecewise uniform */
int LR_pcs_new(LR_obj *o, int n);
int LR_pcs_rm(LR_obj *o);
int LR_pcs_set(LR_obj *o, double x, double p);
int LR_pcs_norm(LR_obj *o);
/* double piece */
double LRd_piece_RAN(LR_obj *o);
double LRd_piece_PDF(LR_obj *o, double x);
double LRd_piece_CDF(LR_obj *o, double x);
/* float piece */
float LRf_piece_RAN(LR_obj *o);
float LRf_piece_PDF(LR_obj *o, float x);
float LRf_piece_CDF(LR_obj *o, float x);

/* linear spline */
int LR_lspl_new(LR_obj *o, int n);
int LR_lspl_rm(LR_obj *o);
int LR_lspl_set(LR_obj *o, double x, double p);
int LR_lspl_norm(LR_obj *o);
/* double linear spline */
double LRd_lspline_RAN(LR_obj *o);
double LRd_lspline_PDF(LR_obj *o, double x);
double LRd_lspline_CDF(LR_obj *o, double x);
/* float linear spline */
float LRf_lspline_RAN(LR_obj *o);
float LRf_lspline_PDF(LR_obj *o, float x);
float LRf_lspline_CDF(LR_obj *o, float x);

/* double gaussian */
double LRd_gausbm_RAN(LR_obj *o);
double LRd_gaus_PDF(LR_obj *o, double x);
double LRd_gaus_CDF(LR_obj *o, double x);
/* float gaussian */
float LRf_gausbm_RAN(LR_obj *o);
float LRf_gaus_PDF(LR_obj *o, float x);
float LRf_gaus_CDF(LR_obj *o, float x);
/* double gsn2 */
double LRd_gsn2_RAN(LR_obj *o);
double LRd_gsn2_PDF(LR_obj *o, double x);
double LRd_gsn2_CDF(LR_obj *o, double x);
/* float gsn2 */
float LRf_gsn2_RAN(LR_obj *o);
float LRf_gsn2_PDF(LR_obj *o, float x);
float LRf_gsn2_CDF(LR_obj *o, float x);

#endif		/* _LIBRAN_H_ */
