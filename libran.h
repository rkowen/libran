/*!	\file libran.h
 *	\brief The LibRan common header file
 */

#ifndef 	_LIBRAN_H_
#  define	_LIBRAN_H_
#  ifdef __cplusplus
extern "C" {
#  endif

#  include <stdarg.h>

/*!	\def	LR_VERSION
 *	\brief	LibRan Library Version (X.Y.Z)
 */
#  define LR_VERSION	"LibRan_VERSION=0.1.0";
/*!	\def	LR_DATE
 *	\brief	LibRan Library date (YYYYMMDD)
 */
#  define LR_DATE	"LibRan_DATE=20190512";

/*!
LibRan Error Numbers
*/
#  define LRerr_OK			0x00 /*!< No error */
#  define LRerr_Unspecified		0x01 /*!< Unknown error */
#  define LRerr_BadDataType		0x03 /*!< Bad data type */
#  define LRerr_BadLRType		0x05 /*!< Bad LibRan variate type */
#  define LRerr_NoAuxiliaryObject	0x09 /*!< No auxiliary object found */
#  define LRerr_NoAuxNormalizeDone	0x0B /*!< Normalization not done */
#  define LRerr_BadAuxSetup		0x0D /*!< Incomplete auxiliary object */
#  define LRerr_BinGeneric		0x11 /*!< Generic binning error */
#  define LRerr_TooManyValues		0x13 /*!< Too many values given */
#  define LRerr_InvalidInputValue	0x15 /*!< Invalid input value given */
#  define LRerr_InvalidRange		0x17 /*!< Invalid range given */
#  define LRerr_UnmetPreconditions	0x19 /*!< Prerequisites not met */
#  define LRerr_SuspiciousValues	0x1B /*!< Something suspicious */
#  define LRerr_AllocFail		0x1D /*!< Memory alloc failure */
#  define LRerr_InvalidCDF		0x21 /*!< CDF is not monotonic, etc. */

/*!
\enum	LR_type
\brief	an enum of allowed random variate distribution types
*/
typedef	enum {
/**< discrete distributions */
	poiss,		/**< Poison */
	binom,		/**< Binomial */
/**< continuum distributions */
	unif,		/**< uniform */
	piece,		/**< piecewise uniform */
	lspline,	/**< linear spline */
	uinvcdf,	/**< User inverse CDF */
	gausbm,		/**< Gaussian (Normal)-Box Muller method*/
	gausmar,	/**< Gaussian (Normal)-Marsaglia method*/
	gsn2,		/**< Gaussian like - 2 uni (saw tooth) */
	gsn4,		/**< Gaussian like - 4 uni */
	gsn12,		/**< Gaussian like - 12 uni */
	logd,		/**< logirithmic */
	nexp,		/**< negative exponential */
	cauchy,		/**< Cauchy */
	cauchymar	/**< Cauchy using polar/Marsaglia method*/
}	LR_type;

/*!
\union	LR_val
\brief	spans the set of allowed value types

This union allows all the various allowed data types to be used for a
given LibRan `LR_obj` object attribute conserving memory.  The length of this
attribute will be the longest of the given types used in the union.

*/
typedef	union {
	long	l; /*!< l - long type */
	int	i; /*!< i - int type */
	float	f; /*!< f - float type */
	double	d; /*!< d - double type */
}	LR_val;

/*!
\enum	LR_data_type
\brief	an enum of allowed value types

This enum value identifies which data type is to be
used within the LibRan object.
Then the associated type should be used from the given `LR_val` attribute.
*/
typedef enum {
	LR_int,		/*!< integer type */
	LR_long,	/*!< long integer type */
	LR_float,	/*!< single precision floating point type */
	LR_double	/*!< double precision floating type */
}	LR_data_type;

/*!
\typedef	LR_obj
\brief		the fundamental LibRan distribution object - the `LR_obj` struct
*/
/* tag & predefine the fns object */
typedef struct LR_obj LR_obj;

/*!
\struct	LR_obj
\brief	the fundamental LibRan random variate distribution object

The `LR_obj` object is the fundemental LibRan random variate distribution
object containing the basic attributes defining the specific distribution
characteristics.

If the built-in pseudo-random number generator is used then the generator
values are also stored within the object to allow each object to act
independently.

Other of the attributes serve the object-oriented aspect of the `LR_obj`
object by having pointers to the random variate distribution's
RAN, PDF, and CDF functions. 

Some random variate distributions (e.g. \e piece and \e lspline ) require
auxiliary methods to define the distribution.

Lastly there is \e errno which contains the last error number encountered.
Use `LR_perror()` to display the error description.
*/
struct LR_obj {
	const char const *	type;	/*!< named distribution type */
	LR_type		t;	/*!< t - distribution type */
	LR_data_type	d;	/*!< d - data type */
	LR_val		a;	/*!< a - lower bound of interval */
	LR_val		b;	/*!< b - upper bound of interval */
	LR_val		m;	/*!< m - middle value of distribution */
	LR_val		s;	/*!< s - measure of distribution width */
	LR_val		x;	/*!< x - auxiliary value */
	/**< object random values */
	LR_val		iy;	/*!< iy  - current random number sequence value */
	LR_val		iy0;	/*!< iy0 - initial random number sequence value */
	/**< set of uniform random number generators - one for each data type */
	int	(*ui)(LR_obj *);	/*!< ui - int random number fn */
	long	(*ul)(LR_obj *);	/*!< ul - long random number fn */
	float	(*uf)(LR_obj *);	/*!< uf - float random number fn */
	double	(*ud)(LR_obj *);	/*!< ud - double random number fn */
	/**< set of Random Fns for this distribution type */
	float	(*rnf)(LR_obj *);		/*!< rnf - float _RAN fn */
	double	(*rnd)(LR_obj *);		/*!< rnd - double _RAN fn */
	/**< set of PDFs for this distribution type */
	float	(*pdff)(LR_obj *, float);	/*!< pdff - float _PDF fn */
	double	(*pdfd)(LR_obj *, double);	/*!< pdfd - double _PDF fn */
	/**< set of CDFs for this distribution type  */
	float	(*cdff)(LR_obj *, float);	/*!< cdff - float _CDF fn */
	double	(*cdfd)(LR_obj *, double);	/*!< cdfd - double _CDF fn */
	/**< generic (void) pointer to some other object */
	void *		aux;	/*!< aux - auxiliary object */
	int		errno;	/*!< errno - last error encountered */
};

/*!
\struct	LR_bin
\brief	the binning object - for tallying results

The `LR_bin` object is a useful tool for tallying and creating a
histogram of samples.  The array of bins and boundaries are allocated
when instantiated the number of which may be less than actually used.

The object also has
\e errno which contains the last error number encountered.
Use `LR_perror()` to display the error description.
*/
typedef struct {
	int		n;	/*!< n - number of bins */
	int		nn;	/*!< nn - number of bins declared */
	long		c;	/*!< c - count of sample values */
	double *	bdrs;	/*!< bdrs - set of bin boundaries (n - 1) */
	long *		bins;	/*!< bins - set of bins (n) */
	int		errno;	/*!< errno - last bin error encountered */
}	LR_bin;

/*!
\struct	LR_pcs
\brief	A special object for defining some of the random variate distributions.

The `LR_pcs` object is not referenced directly by the program and is only
used by the special piecewise uniform and the  linear spline random
variate distributions, and is referenced through the `LR_obj` \e aux
attribute.

In this object are attributes concerning the tallying of samples
and the rest are for \e generic referencing specific methods
given the random variate.
*/
typedef struct {
	int		n;	/*!< n - number of intervals */
	int		nn;	/*!< nn - number of intervals declared */
	double *	bdrs;	/*!< bdrs - set of interval boundaries (n-1) */
	double *	c;	/*!< c - relative probability for each interval (n) */
	double *	sc;	/*!< sc - cumulative probability for each interval (n) */
	double 		norm;	/*!< norm - normalization factor for `c` */
	int		flags;	/*!< flags to guarantee certain actions */

/**< special auxiliary methods */
	int (*new)(LR_obj *o, int n);		/*!< aux new fn */
	int (*rm)(LR_obj *o);			/*!< aux rm fn */
	int (*set)(LR_obj *o, double x, double p); /*!< aux set points fn */
	int (*normalize)(LR_obj *o);		/*!< aux normalize points fn */
}	LR_pcs;

/**< special auxiliary flags for LR_pcs.flags */
#  define	LR_AUX_NORM		0x01 /*!< Performed normalization */
#  define	LR_AUX_SET		0x02 /*!< Performed Aux set */

/*!
\struct	LR_uinvcdf
\brief	A special object for using a user defined CDF

This is another auxiliary object specialized to the `LR_type` = \e uinvcdf.
The user will supply their own defined CDF which will be used via the
inversion method.  The new random variate will be given by
\f$ X \gets \mbox{CDF}^{-1}(U) \f$ where \e U is a uniformly distributed
random variate on the interval [0,1).

The user will define any CDF attributes externally of LibRan.
*/
typedef struct {
	double (*dcdf)(double x);	/*<! double version of CDF */
	float  (*fcdf)(float x);	/*<! float version of CDF */
}	LR_uinvcdf;

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

/* urand routines */
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
float	LR_fgetval(char *str);
double	LR_dgetval(char *str);

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

/* double user inverse CDF */
double LRd_uinvcdf_RAN(LR_obj *o);
double LRd_uinvcdf_PDF(LR_obj *o, double x);
double LRd_uinvcdf_CDF(LR_obj *o, double x);
int LRd_uinvcdf(LR_obj *o, double (*cdf)(double));
/* float user inverse CDF spline */
float LRf_uinvcdf_RAN(LR_obj *o);
float LRf_uinvcdf_PDF(LR_obj *o, float x);
float LRf_uinvcdf_CDF(LR_obj *o, float x);
int LRf_uinvcdf(LR_obj *o, float (*cdf)(float));

/* double negative exponential */
double LRd_nexp_RAN(LR_obj *o);
double LRd_nexp_PDF(LR_obj *o, double x);
double LRd_nexp_CDF(LR_obj *o, double x);
/* float negative exponential */
float LRf_nexp_RAN(LR_obj *o);
float LRf_nexp_PDF(LR_obj *o, float x);
float LRf_nexp_CDF(LR_obj *o, float x);

/* double gaussian */
double LRd_gausbm_RAN(LR_obj *o);
double LRd_gausmar_RAN(LR_obj *o);
double LRd_gaus_PDF(LR_obj *o, double x);
double LRd_gaus_CDF(LR_obj *o, double x);
/* float gaussian */
float LRf_gausbm_RAN(LR_obj *o);
float LRf_gausmar_RAN(LR_obj *o);
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

/* double gsn4 */
double LRd_gsn4_RAN(LR_obj *o);
double LRd_gsn4_PDF(LR_obj *o, double x);
double LRd_gsn4_CDF(LR_obj *o, double x);
/* float gsn4 */
float LRf_gsn4_RAN(LR_obj *o);
float LRf_gsn4_PDF(LR_obj *o, float x);
float LRf_gsn4_CDF(LR_obj *o, float x);

/* double gsn12 */
double LRd_gsn12_RAN(LR_obj *o);
double LRd_gsn12_PDF(LR_obj *o, double x);
double LRd_gsn12_CDF(LR_obj *o, double x);
double LRd_xgsn12_PDF(LR_obj *o, double x);
double LRd_xgsn12_CDF(LR_obj *o, double x);
/* float gsn12 */
float LRf_gsn12_RAN(LR_obj *o);
float LRf_gsn12_PDF(LR_obj *o, float x);
float LRf_gsn12_CDF(LR_obj *o, float x);

/* double cauchy */
double LRd_cauchy_RAN(LR_obj *o);
double LRd_cauchymar_RAN(LR_obj *o);
double LRd_cauchy_PDF(LR_obj *o, double x);
double LRd_cauchy_CDF(LR_obj *o, double x);
/* float cauchy */
float LRf_cauchy_RAN(LR_obj *o);
float LRf_cauchymar_RAN(LR_obj *o);
float LRf_cauchy_PDF(LR_obj *o, float x);
float LRf_cauchy_CDF(LR_obj *o, float x);

#  ifdef __cplusplus
}
#  endif
#endif		/* _LIBRAN_H_ */
