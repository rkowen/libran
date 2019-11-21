/*!
\file	LRuinvcdf.c
\brief	A user defined random variate using the inverse method on a given CDF

The user may supply a compliant CDF and these methods will return a
random variate by the inverse method and an approximate PDF using the
calculus definition of a derivative.

The user supplied CDF must have the following properties where
\f$ \mbox{CDF}(x) \f$ is \e monotonically non-decreasing such that
\f[ \mbox{CDF}(x_1) \le \mbox{CDF}(x_2) \mbox{ where } x_1 < x_2 \f]
and
\f[ 0 \le \mbox{CDF}(x) \le 1 \mbox{ for all } x \f].

If any of these requirements are not met and caught the methods will
raise the \c LRerr_InvalidCDF error (amongst other ones).

The supplied CDFs must have the following signature (for \e double
and \e float respectively:
\code
	double UserCDF(double x);

	float UserCDF(float x);
\endcode
where \c UserCDF is some function name and any attributes are tracked
internally or externally (but not passed through the function argument list).

In addition when the user *must* supply some of the following:
   * Zero, one, or two `LR_obj` end point attributes (\e a and \e b).
   * The `LR_obj` \e middle and \e width (\e m and \e s) if either \e a or
\b are not given.

These attributes are for the Zeroin method to start with some reasonable
guesses of where to bracket the solution.  If your CDF has a well defined
interval such that \f$ \mbox{UserCDF}(a) = 0 \f$
and \f$ \mbox{UserCDF}(b) = 1 \f$ then
setting these attributes are likely the best approach.  However, if the CDF is
semi-infinite or infinite in range then setting the middle \e m to either
near the mean, median, or mode will be likely sufficient.  Note that \e m
does not need to be exact, just a good approximation (e.g.
\f$ \mbox{UserCDF}(m) \approx \frac{1}{2} \f$.  The width \e s should be such
that about two thirds of the random variate density lies within about
\f$ m - s \f$ and \f$ m + s \f$.  In other words,
\f$ \mbox{UserCDF}(m + s) - \mbox{UserCDF}(m - s) \approx \frac{2}{3} \f$.
If the function is semi-infinite then give the known endpoint, approximate
\e m and \e s to handle the semi-infinite part.

\code
#include <math.h>
#include "libran.h"

double MyCDF(double x) {
        static double pi4 = NAN;
        double zero = 0.0, one = 1.0, two = 2.0;
        if (isnan(pi4))
                pi4 = atan(one);

        if (x <= zero) {
                return zero;
        } else if (x >= two) {
                return one;
        } else {
                double t = sin(pi4*x);
                return t*t;
        }
}

int main() {
...
LR_obj *o = LR_new(uinvcdf, LR_double);
...
// set the end points for the zeroin search
LR_set_all(o,"ab", 0., 2.);
// Include the User CDF
if (LRd_uinvcdf(o, MyCDF)) {
	LRperror("Sample Code - uinvcdf", o->errno);
}
...
// do your typical processing
...
// remove the LR_obj
LR_rm(&o);
...
}
\endcode

The probability and cumulative distribution functions for the
above user defined CDF is:

\manonly
CDF	U(u) =	{ 0			if x < 0
		{ sin(pi*x/4)^2		if 0 <= x < 2
		{ 1			if 2 <= x
\endmanonly

\f{eqnarray*}{
\mbox{User CDF } U(x)	&=
	\left\{ \begin{array}{ll}
		0, &	x < 0,				\\
		\sin(\frac{\pi}{4}x)^2,	& 0 < x \le 2	\\
		1, &	2 \le x
	\end{array} \right.
\f}

\image html UserDefinedCDF.png
\image latex UserDefinedCDF.eps "User defined CDF"

*/

#ifdef __cplusplus
extern "C" {
#endif

#include "libran.h"
#include <math.h>

#define Abs(a) (((a)<0)?-(a):(a))
#define Signabs(a,b) ((b)<0?-(a):(a))
#define Sign(a,b) ((b)<0?-Abs(a):Abs(a))

/*!
\brief	LRd_uinvcdf() - set the user defined CDF for this variate distribution

@param	o	LR_obj object
@param	cdf	User defined CDF
@return	0 if no error, non-zero otherwise
*/
int LRd_uinvcdf(LR_obj *o, double (*cdf)(double)) {
	LR_uinvcdf *aux = (LR_uinvcdf *) o->aux;
	if (o->d != LR_double) {
		o->errno = LRerr_BadDataType;
		return o->errno;
	}
	aux->dcdf = cdf;

	return LRerr_OK;
}

/*!
\brief	LRd_zeroin() Routine numerically finds solution to f(x)-U = 0
specialized to CDF()s

 * LRd_zeroin() finds the zero of (f(x) - U) within the interval [ax,bx].
 * It is assumed that f(ax) - U and f(bx) - U have opposite signs without
 * checking.
 * LRd_zeroin returns a zero x in the given interval [ax,bx] to within a
 * tolerance 2*eps*abs(x) + tol, where eps is the relative machine precision
 * such that eps is the smallest number + 1 that is different from 1.
 *
 * This is based on the subroutine ZEROIN() in FMMLIB presented in
 * "Introduction to Numerical Analysis" by Forsythe, Malcolm, and Moler,
 * which is a slightly modified translation of the ALGOL 60 procedure ZERO
 * given by Richard Brent, "Algorithms for Minimization without Derivatives",
 * Prentice-Hall, Inc. (1973).
 *
 * by R.K.Owen,Ph.D.  1994/04/18

@param	ax	left  endpoint of the initial interval
@param	bx	right endpoint of the initial interval
@param	U	Uniform random variate of where to find x such that f(x) = U
@param	f	function subprogram which evaluates f(x) for any x in
		  the interval [ax,bx]
@param	tol	desired length of the interval of uncertainty of any
		  final result ( must be >= 0.) (Always set to zero here.)
@return	double 	the value x such that UserCDF(x) - U = 0
 */
double LRd_zeroin(
	double ax, double bx,
	double U, double (*f)(double), double tol) {

	const double zero = 0.0, one = 1.0, two = 2.0, three = 3.0,
		half = 0.5;
	static double eps;
	static char FIRST = (0 == 0);
	double a,b,c,e,d,fa,fb,fc,tol1,xm,p,q,r,s,tmp1,tmp2;

	if (FIRST) {
		eps = LR_dgetval("LR_DEPS");
		FIRST = !FIRST;
	}
/* initialization */
	a = ax;
	b = bx;
	fa = (*f)(a) - U;
	fb = (*f)(b) - U;
/* begin step */
	c = a;
	fc = fa;
	d = b - a;
	e = d;

	while (1) {
/* tighten interval */
		if (Abs(fc) < Abs(fb)) {
			a = b;
			b = c;
			c = a;
			fa = fb;
			fb = fc;
			fc = fa;
		}
/* convergence test */
		tol1 = eps*Abs(b) + half*tol;
		xm = half*(c - b);
		if (Abs(xm) < tol1 || fb == zero) return (b);	/* end */

		if (Abs(e) < tol1 || Abs(fa) <= Abs(fb)) {
/* bisection */
			d = xm;
			e = d;
		} else {
			if (a == c) {
/* linear interpolation */
				s = fb/fa;
				p = two*xm*s;
				q = one - s;
			} else {
/* inverse quadratic interpolation */
				q = fa/fc;
				r = fb/fc;
				s = fb/fa;
				p = s*(two*xm*q*(q - r) - (b - a)*(r - one));
				q = (q - one)*(r - one)*(s - one);
			}
/* adjusts sign */
			if (p > zero) q = -q;
			p = Abs(p);

			tmp1 = tol1*q;
			tmp2 = half*e*q;
			if ((two*p) >= (three*xm*q - Abs(tmp1))
			|| (p >= Abs(tmp2))) {
/* bisection */
				d = xm;
				e = d;
			} else {
/* interpolation is acceptable */
				e = d;
				d = p/q;
			}
		}
/* complete step */
		a = b;
		fa = fb;
		if (Abs(d) > tol1) b += d;
		else b += Signabs(tol1,xm);
		fb = (*f)(b) - U;

		if ((fb*Signabs(one,fc)) > zero) {
/* begin step again */
			c = a;
			fc = fa;
			d = b - a;
			e = d;
		}
	}
}

/*!
@brief LRd_uinvcdf_RAN(LR_obj *o) - double random variate via inverse method
of the UserCDF() fn.

@param	o	LR_obj object
@return	double
*/
double LRd_uinvcdf_RAN(LR_obj *o) {
	double	u, ax,bx, fax, fbx;
	double	zero = 0.0, one = 1.0, two = 2.0;
	if (!o->aux) {
		o->errno = LRerr_NoAuxiliaryObject;
		return NAN;
	}
	if (!(((LR_uinvcdf *) o->aux)->dcdf)) {
		o->errno = LRerr_BadAuxSetup;
		return NAN;
	}

	u = o->ud(o);
	if (!isnan(o->a.d)) {
		ax = o->a.d;
	} else {
		double scale = one;
		ax = o->m.d;
		fax = ((LR_uinvcdf *) o->aux)->dcdf(ax);
		if (fax < zero) {
			o->errno = LRerr_InvalidCDF;
			return NAN;
		}
		while (fax > u) {
			ax = o->m.d - scale*o->s.d;
			fax = ((LR_uinvcdf *) o->aux)->dcdf(ax);
			scale *= two;
		}
	}
	if (!isnan(o->b.d)) {
		bx = o->b.d;
	} else {
		double scale = one;
		bx = o->m.d;
		fbx = ((LR_uinvcdf *) o->aux)->dcdf(bx);
		if (fbx > one) {
			o->errno = LRerr_InvalidCDF;
			return NAN;
		}
		while (fbx < u) {
			bx = o->m.d + scale*o->s.d;
			fbx = ((LR_uinvcdf *) o->aux)->dcdf(bx);
			scale *= two;
		}
	}
	return LRd_zeroin(ax, bx, u, ((LR_uinvcdf *) o->aux)->dcdf, zero);
}

/*!
@brief LRd_uinvcdf_PDF(LR_obj *o, double x) - double probability
distribution function approximated from the user supplied cumulative
distribution function.

Use the definition of a derivative to approximate the PDF from the given
CDF.

@param	o	LR_obj object
@param	x	value
@return	double PDF at x
*/
double LRd_uinvcdf_PDF(LR_obj *o, double x) {
	static double sqeps = NAN, isqeps, nearzero;
	double	half = 0.5, zero = 0.0, one = 1.0, xp, xm, fp, fm, f0, ret;
	LR_uinvcdf *aux = (LR_uinvcdf *) o->aux;

	if (!aux) {
		o->errno = LRerr_NoAuxiliaryObject;
		return NAN;
	}
	if (!(aux->dcdf)) {
		o->errno = LRerr_BadAuxSetup;
		return NAN;
	}

	if (isnan(sqeps)) {
		sqeps = LR_dgetval("LR_DSQEPS");
		isqeps = one/sqeps;
		nearzero = sqeps*sqrt(sqeps);
	}
	
	/* use three values around x */
	if (-nearzero < x && x < nearzero) {
		xp = sqeps;
		xm = -sqeps;
	} else {
		xp = x*(one + sqeps);
		xm = x*(one - sqeps);
	}
	fm = aux->dcdf(xm);
	fp = aux->dcdf(xp);
	
	if (fp == one) {
		f0 = aux->dcdf(x);
		ret = (f0-fm)/(x-xm);
	} else if (fm == zero) {
		f0 = aux->dcdf(x);
		ret = (fp-f0)/(xp-x);
	} else {
		ret = (fp-fm)/(xp-xm);
	}
	if (ret < zero) {
		o->errno = LRerr_InvalidCDF;
		return NAN;
	}
	return ret;
}

/*!
@brief LRd_uinvcdf_CDF(LR_obj *o, double x) - double User supplied cumulative
distribution function.

@param	o	LR_obj object
@param	x	value
@return	double CDF at x
*/
double LRd_uinvcdf_CDF(LR_obj *o, double x) {
	double zero = 0.0, one = 1.0, ret;
	LR_uinvcdf *aux = (LR_uinvcdf *) o->aux;

	if (!aux) {
		o->errno = LRerr_NoAuxiliaryObject;
		return NAN;
	}
	if (!(aux->dcdf)) {
		o->errno = LRerr_BadAuxSetup;
		return NAN;
	}

	ret = aux->dcdf(x);

	if (ret < zero || ret > one) {
		o->errno = LRerr_InvalidCDF;
		return NAN;
	}
	return ret;
}

/*!
\brief	LRf_uinvcdf() - set the user defined CDF for this variate distribution

@param	o	LR_obj object
@param	cdf	User defined CDF
@return	0 if no error, non-zero otherwise
*/
int LRf_uinvcdf(LR_obj *o, float (*cdf)(float)) {
	LR_uinvcdf *aux = (LR_uinvcdf *) o->aux;
	if (o->d != LR_float) {
		o->errno = LRerr_BadDataType;
		return o->errno;
	}
	aux->fcdf = cdf;

	return LRerr_OK;
}

/*!
\brief	LRf_zeroin() Routine numerically finds solution to f(x)-U = 0
specialized to CDF()s

 * LRf_zeroin() finds the zero of (f(x) - U) within the interval [ax,bx].
 * It is assumed that f(ax) - U and f(bx) - U have opposite signs without
 * checking.
 * LRf_zeroin returns a zero x in the given interval [ax,bx] to within a
 * tolerance 2*eps*abs(x) + tol, where eps is the relative machine precision
 * such that eps is the smallest number + 1 that is different from 1.
 *
 * This is based on the subroutine ZEROIN() in FMMLIB presented in
 * "Introduction to Numerical Analysis" by Forsythe, Malcolm, and Moler,
 * which is a slightly modified translation of the ALGOL 60 procedure ZERO
 * given by Richard Brent, "Algorithms for Minimization without Derivatives",
 * Prentice-Hall, Inc. (1973).
 *
 * by R.K.Owen,Ph.D.  1994/04/18

@param	ax	left  endpoint of the initial interval
@param	bx	right endpoint of the initial interval
@param	U	Uniform random variate of where to find x such that f(x) = U
@param	f	function subprogram which evaluates f(x) for any x in
		  the interval [ax,bx]
@param	tol	desired length of the interval of uncertainty of any
		  final result ( must be >= 0.) (Always set to zero here.)
@return	float 	the value x such that UserCDF(x) - U = 0
 */
float LRf_zeroin(
	float ax, float bx,
	float U, float (*f)(float), float tol) {

	const float zero = 0.0, one = 1.0, two = 2.0, three = 3.0,
		half = 0.5;
	static float eps;
	static char FIRST = (0 == 0);
	float a,b,c,e,d,fa,fb,fc,tol1,xm,p,q,r,s,tmp1,tmp2;

	if (FIRST) {
		eps = LR_fgetval("LR_FEPS");
		FIRST = !FIRST;
	}
/* initialization */
	a = ax;
	b = bx;
	fa = (*f)(a) - U;
	fb = (*f)(b) - U;
/* begin step */
	c = a;
	fc = fa;
	d = b - a;
	e = d;

	while (1) {
/* tighten interval */
		if (Abs(fc) < Abs(fb)) {
			a = b;
			b = c;
			c = a;
			fa = fb;
			fb = fc;
			fc = fa;
		}
/* convergence test */
		tol1 = eps*Abs(b) + half*tol;
		xm = half*(c - b);
		if (Abs(xm) < tol1 || fb == zero) return (b);	/* end */

		if (Abs(e) < tol1 || Abs(fa) <= Abs(fb)) {
/* bisection */
			d = xm;
			e = d;
		} else {
			if (a == c) {
/* linear interpolation */
				s = fb/fa;
				p = two*xm*s;
				q = one - s;
			} else {
/* inverse quadratic interpolation */
				q = fa/fc;
				r = fb/fc;
				s = fb/fa;
				p = s*(two*xm*q*(q - r) - (b - a)*(r - one));
				q = (q - one)*(r - one)*(s - one);
			}
/* adjusts sign */
			if (p > zero) q = -q;
			p = Abs(p);

			tmp1 = tol1*q;
			tmp2 = half*e*q;
			if ((two*p) >= (three*xm*q - Abs(tmp1))
			|| (p >= Abs(tmp2))) {
/* bisection */
				d = xm;
				e = d;
			} else {
/* interpolation is acceptable */
				e = d;
				d = p/q;
			}
		}
/* complete step */
		a = b;
		fa = fb;
		if (Abs(d) > tol1) b += d;
		else b += Signabs(tol1,xm);
		fb = (*f)(b) - U;

		if ((fb*Signabs(one,fc)) > zero) {
/* begin step again */
			c = a;
			fc = fa;
			d = b - a;
			e = d;
		}
	}
}

/*!
@brief LRf_uinvcdf_RAN(LR_obj *o) - float random variate via inverse method
of the UserCDF() fn.

@param	o	LR_obj object
@return	float
*/
float LRf_uinvcdf_RAN(LR_obj *o) {
	float	u, ax,bx, fax, fbx;
	float	zero = 0.0, one = 1.0, two = 2.0;
	if (!o->aux) {
		o->errno = LRerr_NoAuxiliaryObject;
		return NAN;
	}
	if (!(((LR_uinvcdf *) o->aux)->fcdf)) {
		o->errno = LRerr_BadAuxSetup;
		return NAN;
	}

	u = o->ud(o);
	if (!isnan(o->a.f)) {
		ax = o->a.f;
	} else {
		float scale = one;
		ax = o->m.f;
		fax = ((LR_uinvcdf *) o->aux)->fcdf(ax);
		if (fax < zero) {
			o->errno = LRerr_InvalidCDF;
			return NAN;
		}
		while (fax > u) {
			ax = o->m.f - scale*o->s.f;
			fax = ((LR_uinvcdf *) o->aux)->fcdf(ax);
			scale *= two;
		}
	}
	if (!isnan(o->b.f)) {
		bx = o->b.f;
	} else {
		float scale = one;
		bx = o->m.f;
		fbx = ((LR_uinvcdf *) o->aux)->fcdf(bx);
		if (fbx > one) {
			o->errno = LRerr_InvalidCDF;
			return NAN;
		}
		while (fbx < u) {
			bx = o->m.f + scale*o->s.f;
			fbx = ((LR_uinvcdf *) o->aux)->fcdf(bx);
			scale *= two;
		}
	}
	return LRf_zeroin(ax, bx, u, ((LR_uinvcdf *) o->aux)->fcdf, zero);
}

/*!
@brief LRf_uinvcdf_PDF(LR_obj *o, float x) - float probability
distribution function approximated from the user supplied cumulative
distribution function.

Use the definition of a derivative to approximate the PDF from the given
CDF.

@param	o	LR_obj object
@param	x	value
@return	float PDF at x
*/
float LRf_uinvcdf_PDF(LR_obj *o, float x) {
	static float sqeps = NAN, isqeps, nearzero;
	float	half = 0.5, zero = 0.0, one = 1.0, xp, xm, fp, fm, f0, ret;
	LR_uinvcdf *aux = (LR_uinvcdf *) o->aux;

	if (!aux) {
		o->errno = LRerr_NoAuxiliaryObject;
		return NAN;
	}
	if (!(aux->fcdf)) {
		o->errno = LRerr_BadAuxSetup;
		return NAN;
	}

	if (isnan(sqeps)) {
		sqeps = LR_fgetval("LR_FSQEPS");
		isqeps = one/sqeps;
		nearzero = sqeps*sqrtf(sqeps);
	}
	
	/* use three values around x */
	if (-nearzero < x && x < nearzero) {
		xp = sqeps;
		xm = -sqeps;
	} else {
		xp = x*(one + sqeps);
		xm = x*(one - sqeps);
	}
	fm = aux->fcdf(xm);
	fp = aux->fcdf(xp);
	
	if (fp == one) {
		f0 = aux->fcdf(x);
		ret = (f0-fm)/(x-xm);
	} else if (fm == zero) {
		f0 = aux->fcdf(x);
		ret = (fp-f0)/(xp-x);
	} else {
		ret = (fp-fm)/(xp-xm);
	}
	if (ret < zero) {
		o->errno = LRerr_InvalidCDF;
		return NAN;
	}
	return ret;
}

/*!
@brief LRf_uinvcdf_CDF(LR_obj *o, float x) - float User supplied cumulative
distribution function.

@param	o	LR_obj object
@param	x	value
@return	float CDF at x
*/
float LRf_uinvcdf_CDF(LR_obj *o, float x) {
	float zero = 0.0, one = 1.0, ret;
	LR_uinvcdf *aux = (LR_uinvcdf *) o->aux;

	if (!aux) {
		o->errno = LRerr_NoAuxiliaryObject;
		return NAN;
	}
	if (!(aux->fcdf)) {
		o->errno = LRerr_BadAuxSetup;
		return NAN;
	}

	ret = aux->fcdf(x);

	if (ret < zero || ret > one) {
		o->errno = LRerr_InvalidCDF;
		return NAN;
	}
	return ret;
}

#ifdef __cplusplus
}
#endif
