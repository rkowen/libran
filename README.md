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

The output may look like this:

\code

 n >  lo   -   hi   :    sample ,  expected
--- ------- -------- ----------   ---------
  0> -99.0 -    0.0 :        60 ,     67.49
  1>   0.0 -    0.1 :        21 ,     25.80
  2>   0.1 -    0.2 :        30 ,     34.47
  3>   0.2 -    0.3 :        53 ,     45.59
  4>   0.3 -    0.4 :        62 ,     59.71
  5>   0.4 -    0.5 :        83 ,     77.42
  6>   0.5 -    0.6 :       103 ,     99.39
  7>   0.6 -    0.7 :       132 ,    126.33
  8>   0.7 -    0.8 :       157 ,    158.97
  9>   0.8 -    0.9 :       177 ,    198.05
 10>   0.9 -    1.0 :       226 ,    244.29
 11>   1.0 -    1.1 :       287 ,    298.32
 12>   1.1 -    1.2 :       377 ,    360.69
 13>   1.2 -    1.3 :       411 ,    431.76
 14>   1.3 -    1.4 :       526 ,    511.69
 15>   1.4 -    1.5 :       609 ,    600.40
 16>   1.5 -    1.6 :       676 ,    697.47
 17>   1.6 -    1.7 :       793 ,    802.19
 18>   1.7 -    1.8 :       920 ,    913.46
 19>   1.8 -    1.9 :      1009 ,   1029.82
 20>   1.9 -    2.0 :      1134 ,   1149.46
 21>   2.0 -    2.1 :      1225 ,   1270.24
 22>   2.1 -    2.2 :      1413 ,   1389.76
 23>   2.2 -    2.3 :      1488 ,   1505.41
 24>   2.3 -    2.4 :      1553 ,   1614.47
 25>   2.4 -    2.5 :      1734 ,   1714.22
 26>   2.5 -    2.6 :      1780 ,   1802.04
 27>   2.6 -    2.7 :      1817 ,   1875.52
 28>   2.7 -    2.8 :      1982 ,   1932.59
 29>   2.8 -    2.9 :      1969 ,   1971.59
 30>   2.9 -    3.0 :      2023 ,   1991.39
 31>   3.0 -    3.1 :      2046 ,   1991.39
 32>   3.1 -    3.2 :      1978 ,   1971.59
 33>   3.2 -    3.3 :      1898 ,   1932.59
 34>   3.3 -    3.4 :      1878 ,   1875.52
 35>   3.4 -    3.5 :      1811 ,   1802.04
 36>   3.5 -    3.6 :      1687 ,   1714.22
 37>   3.6 -    3.7 :      1685 ,   1614.47
 38>   3.7 -    3.8 :      1490 ,   1505.41
 39>   3.8 -    3.9 :      1390 ,   1389.76
 40>   3.9 -    4.0 :      1289 ,   1270.24
 41>   4.0 -    4.1 :      1148 ,   1149.46
 42>   4.1 -    4.2 :      1006 ,   1029.82
 43>   4.2 -    4.3 :       931 ,    913.46
 44>   4.3 -    4.4 :       776 ,    802.19
 45>   4.4 -    4.5 :       718 ,    697.47
 46>   4.5 -    4.6 :       644 ,    600.40
 47>   4.6 -    4.7 :       537 ,    511.69
 48>   4.7 -    4.8 :       419 ,    431.76
 49>   4.8 -    4.9 :       365 ,    360.69
 50>   4.9 -    5.0 :       290 ,    298.32
 51>   5.0 -    5.1 :       249 ,    244.29
 52>   5.1 -    5.2 :       215 ,    198.05
 53>   5.2 -    5.3 :       158 ,    158.97
 54>   5.3 -    5.4 :       140 ,    126.33
 55>   5.4 -    5.5 :       109 ,     99.39
 56>   5.5 -    5.6 :        77 ,     77.42
 57>   5.6 -    5.7 :        58 ,     59.71
 58>   5.7 -    5.8 :        49 ,     45.59
 59>   5.8 -    5.9 :        29 ,     34.47
 60>   5.9 -    6.0 :        23 ,     25.80
 61>   6.0 -   99.0 :        77 ,     67.49

\endcode

LibRan Source Code
==================

The central \e git repository is at SourceForge and can be read-only
cloned with:
\code

git clone git://git.code.sf.net/p/libran/code libran-code

\endcode

The SourceForge site is -
[https://sourceforge.net/projects/libran](
https://sourceforge.net/projects/libran/)

The on-line documentation is at
[http://libran.org/](
http://libran.org/
)

\include lgpl.md

\include EXPAND.md
