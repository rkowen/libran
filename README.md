\image html LRlogo.png
\image latex LRlogo.eps "LibRan library"

LibRan pseudo-random number generator library
=============================================

The LibRan package is a library of various pseudo-random number
generators along with their \e exact probability and cumulative
probability density functions.  The libary contains its own
optimized sequential congruential uniform pseudo-random number
generator on the interval \f$ x \in [0,1) \f$; along with useful
tools such as methods for collecting statistics into \e bins.

Each of the random variate distributions rely on a number of internal
attributes to \e customize the distribution.  The library is written
in an object oriented fashion (in C) such that each object's attributes
and random number generator is separate from every other object.
There are also \e generic routines for calling the various specialized
routines for the given random variate distribution.

Required Library Methods
========================

Each random variate included here has at least three provided methods,
and for two numeric precisions - (`f` = `float` and `d` = `double`).
The data or numeric precision is given by the \e enum `LR_data_type`
in `libran.h`.

Routine     | Generic Fn | Description
----------- | ---------- | -----------
LR?_*_RAN    | LR?_RAN    | Random variate generator
LR?_*_PDF    | LR?_PDF    | Random variate probability distribution function
LR?_*_CDF    | LR?_CDF    | Random variate cumulative distribution function

Where \f$ \mbox{PDF}(x) = \frac{d}{dx} \mbox{CDF}(x) \f$, and
\f$ \mbox{CDF}(x) \f$ is a \e monotonically non-decreasing function such that
\f[ \mbox{CDF}(x_1) \le \mbox{CDF}(x_2) \mbox{ where } x_1 < x_2 \f]
and
\f[ 0 \le \mbox{CDF}(x) \le 1 \mbox{ for all } x \f]

The \e generic functions are found in `LRdf.c`.

Random Variate Distributions
============================

There are a number of built-in random variate distributions and the complete
list of allowed distribution types is given by the \e enum `LR_type` in
`libran.h`.
They fall into a few types as follows:

Full Infinite Range
-------------------

These random variate distributions will generate variates where
\f$ x \in (-\infty,\infty) \f$.  These distributions
are defined with attributes \e m and \e s representing the mean or mode
and the width of the distribution.

Type       | Source           | Distribution Description
---------- | ---------------- | ------------------------
unif       | LRunif.c         | uniformly on given interval
gausbm     | LRgaus.c         | Gaussian or Normal using Box-Muller method
gausmar    | LRgaus.c         | Gaussian or Normal using Marsaglia method
cauchy     | LRcauchy.c       | Cauchy using inverse method
cauchymar  | LRcauchy.c       | Cauchy using Marsaglia method

Semi-Infinite Range
-------------------

These random variate distributions will generate variates where
\f$ x \in (0,\infty) \f$.

Type       | Source            | Distribution Description
---------- | ----------------- | ------------------------
nexp       | LRnexp.c          | negative exponential

Finite Range
------------

These random variate distributions will generate variates on a finite
interval \f$ x \in (a,b) \f$.

Type       | Source           | Distribution Description
---------- | ---------------- | ------------------------
gsn2       | LRgsn.c          | Gaussian-like (sawtooth)
gsn4       | LRgsn.c          | Gaussian-like (simple bell curve)
gsn12      | LRgsn.c          | Gaussian-like (close Gaussian approximation)


User Defined
------------

These distributions have user-defined shapes.

Type       | Source            | Distribution Description
---------- | ----------------- | ------------------------
lspline    | LRlspline.c       | Linear spline PDF on [a,b]
piece      | LRpiece.c         | Histogram-like PDF on [a,b]
inverse    | LRinv.c           | User defined CDF


Example Code
============

Here is an example code showing how to use the `LR_obj` and `LR_bin`
objects and how to set-up the `LR_bin` tally bins.  The expected values
rely on calculating the difference of CDFs at the boundaries.

The median is at \f$ x = 3 \f$ with a width of 2.  This code can be used
\e as-is by just changing the `LR_type` from \e gausbm to either
\e gausmar, \e gsn12, \e cauchy, or \e cauchymar.
The code can be simply changed for those random variate distributions
with definite endpoints \e a and \e b.

\code
/* example LibRan code */

#include <stdio.h>
#include <math.h>
#include "libran.h"

int main() {
	int nsamples = 50000;
	// create LR object and binning object
	LR_obj *o = LR_new(gausbm, LR_double);
	LR_bin *b = LR_bin_new(62);
	if (!o) {
		fprintf(stderr, "Error in creating LR object\n");
		return 1;
	}
	if (!b) {
		fprintf(stderr, "Error in creating bin object\n");
		return 2;
	}
	// set attributes
	if (LR_set_all(o, "ms", 3., 1.) > 0) {
		LRperror("Sample Code", o->errno);
		return 3;
	}
	// check the object
	if (LR_check(o)) {
		LRperror("Sample Code - check", o->errno);
		return 4;
	}
	// set seed
	LR_lsetseed(o, 19580512L);

	// set up bins (use attributes)
	double	start	= o->m.d - 3.0*o->s.d,
		finish	= o->m.d + 3.0*o->s.d,
		incr	= 6.0*o->s.d / 60;
	for (int i = 0; i <= 60; i++) {
		if (LR_bin_set(b, start + i*incr)) {
			LRperror("Sample Code - bin", b->errno);
			return 5;
		}
	}
	// set up expectation histogram
	double	prevcdf = 0.0, thiscdf, ehist[62];
	for (int i = 0; i < 61; i++) {
		if (isnan(thiscdf = LRd_CDF(o, start + i*incr))){
			LRperror("Sample Code - CDF", o->errno);
			return 6;
		}
		ehist[i] = nsamples * (thiscdf - prevcdf);
		prevcdf = thiscdf;
	}
	// last bin
	ehist[61] = nsamples * (1.0 - prevcdf);

	// run samples
	double x;
	for (int i = 0; i < nsamples; i++) {
		if (isnan(x = LRd_RAN(o))) {
			LRperror("Sample Code - RAN", o->errno);
			return 7;
		}
		if (LR_bin_add(b, x)) {
			LRperror("Sample Code - bin add", b->errno);
			return 8;
		}
	}

	// output results (have 99 = inf here)
	double prevbdr = -99., thisbdr;
	printf("\n"
" n >  lo   -   hi   :    sample ,  expected\n"
"--- ------- -------- ----------   ---------\n");
	for (int i = 0; i < 61; i++) {
		thisbdr = b->bdrs[i];
		printf("% 3d>% 6.1f - % 6.1f : % 9ld , % 9.2f\n",
			i, prevbdr, thisbdr, b->bins[i], ehist[i]);
		prevbdr = thisbdr;
	}
	// last bin
	printf("% 3d>% 6.1f - % 6.1f : % 9ld , % 9.2f\n",
		61, prevbdr, 99., b->bins[61], ehist[61]);

	// clean up
	LR_bin_rm(&b);
	LR_rm(&o);

	return 0;
}
\endcode

\include lgpl.md

\include EXPAND.md
