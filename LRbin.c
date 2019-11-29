/*!
\file	LRbin.c
\brief	Set of LibRan binning functions

This set of LibRan functions will set-up or take-down the `LR_bin` object
and the bins for
counting values into bins based on the bin boundaries.
Each of the `LR_bin` objects is independent of any other object such as
the `LR_obj`.

Having such bins are useful for visualizing a random variates distribution.
Given the total number of samples is \e N, then the number of likely
samples falling within the bin will be
\f[
N \int_{x_i}^{x_{i+1}}f(x)dx = N \left[F(x_{i+1}) - F(x_i)\right]
\f]
where the bin interval is \f$ [x_i,x_{i+1}) \f$ and \f$ f(x) \f$
is the probability distribution function (PDF) and \f$ F(x) \f$
is the  cumulative distribution function (CDF).

The bin object also tallies samples below and above the set of bins.
If you define \e n-1 boundaries then there are \e n bins.
The order the boundaries are set is not important.  The method orders
the boundaries internally.

The following is some example code fragments for defining the bins, tallying
the samples, and viewing the results.

\code
#include <stdio.h>
#include "libran.h"
...
int nbin = 10;
LR_bin *b = LR_bin_new(nbin+2);
...
// set the boundaries
for (int i = 0; i <= nbin; i++) {
	LR_bin_set(b, 2.0 + .1 * i);
}
...
// tally samples
for (int i = 0; i < 10000; i++) {
	LR_bin_add(b, LRd_RAN(o));
}
...
// view results
fprint("-inf - %f : %f\n", b->bdrs[0], b->bins[0]);
for (int i = 1; i <= nbin; i++) {
	fprint("%f - %f : %f\n", b->bdrs[i-1], b->bdrs[i], b->bins[i]);
}
fprint("%f - inf : %f\n", b->bdrs[nbin+1], b->bins[nbin+1]);
...
LR_bin_rm(&b);

\endcode

*/
/*
 * Copyright	2019	R.K. Owen, Ph.D.
 * License	see lgpl.md (Gnu Lesser General Public License)
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <math.h>	/* isnan() */
#include "libran.h"

/*!
@brief	LR_bin_new(LR_data_type d, int n) - create new binning object

This routine creates a new `LR_bin` object and allocates the necessary
memory for the boundaries and tally bins.  Must give at least the expected
number of bins or the number of boundaries plus one.

@param	t	data type
@param	n	number of bins
@return	LR_bin object if successful, else NULL
*/
LR_bin *LR_bin_new(int n) {
	LR_bin *ptr = (void *) NULL;

	if (!(ptr = (LR_bin *) malloc(sizeof(LR_bin))))
		return ptr;
	ptr->n  = n;
	ptr->nn = 1;	/* always start with one bin */
	ptr->c  = 0;
	ptr->errno = 0;

	if (!(ptr->bdrs = (double *) calloc(n, sizeof(double))))
		goto bad0;

	if (!(ptr->bins = (long *) calloc(n, sizeof(long))))
		goto bad1;

	return ptr;

bad1:
	free((void *) ptr->bdrs);

bad0:
	free((void *) ptr);
	return (void *) NULL;
}

/*!
@brief	LR_bin_rm(LR_bin **b) - remove binning object

Since the `LR_bin` object allocates memory this method needs to be
called to deallocate the memory to avoid memory leaks.

@param	b	LR_bin object address
@return	0 if successful, else non-zero if failed
*/
int LR_bin_rm(LR_bin **b) {
	if (b && *b) {
		free((void *) (*b)->bins);
		free((void *) (*b)->bdrs);
		free((void *) *b);
		*b = (LR_bin *) NULL;
		return LRerr_OK;
	}
	return LRerr_BinGeneric;
}

/*!
@brief	LR_bin_set(LR_bin *b, double x) - add bin boundary

Include another bin boundary, which will be ordered internally.
However, it will not identify or flag repeated boundaries.

@param	b	LR_bin object
@param	x	bin boundary to add
@return	0 if successful, else non-zero if failed
*/
int LR_bin_set(LR_bin *b, double x) {
	int i = 0;
	double t;
	if (b->n < b->nn + 1) {
		/* too many values given */
		return b->errno = LRerr_TooManyValues;
	}
	t = b->bdrs[0];
	for (int i = 0, i1 = 1; i <= b->nn; i++,i1++) {
		if (b->nn == i1) {		/* got to top entry */
			b->bdrs[i] = x;
			b->nn++;
			return LRerr_OK;
		}
		if (x > t) {
			/* compare to next one, next round */
			t = b->bdrs[i1];
		} else {
			/* insert here, but keep current */
			t = b->bdrs[i];
			b->bdrs[i] = x;
			/* use current for comparison */
			x = t;
			t = b->bdrs[i1];
		}
	}
	b->nn++;

	return LRerr_OK;
}

/*!
@brief	LR_bin_add(LR_bin *b, double x) - collect value to be binned.

This is *the* tallying routine, where the value \e x is compared to the
boundary values and the appropriate bin array element tally is incremented.

@param	b	LR_bin object
@param	x	value to count within the given bin.
@return	0 if successful, else non-zero if failed
*/
int LR_bin_add(LR_bin *b, double x) {
	int i = 0;

	if (isnan(x))			
		return	b->errno = LRerr_InvalidInputValue;

	while (i <= b->nn - 1) {
		if (x < b->bdrs[i]
		||  i == b->nn - 1) {
			b->bins[i]++;
			break;
		}
		i++;
	}
	if (i >= b->nn)
		return	b->errno = LRerr_InvalidRange;
	b->c++;

	return LRerr_OK;
}

#ifdef __cplusplus
}
#endif
