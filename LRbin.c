/*!
\file	LRbin.c
\brief	Set of binning functions

This set of functions: set-up, count values into bins, and take-down

*/
#include <stdlib.h>
#include "libran.h"

/*!
@brief	LR_bin_new(LR_data_type d, int n) - create new binning object

@param	t	data type
@param	n	number of bins
@return	LR_bin object
*/
LR_bin *LR_bin_new(int n) {
	LR_bin *ptr = (void *) NULL;

	if (!(ptr = (LR_bin *) malloc(sizeof(LR_bin))))
		return ptr;
	ptr->n  = n;
	ptr->nn = 1;	/* always start with one bin */
	ptr->c  = 0;

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

@param	b	LR_bin object address
@return	0 if successful, else non-zero if failed
*/
int LR_bin_rm(LR_bin **b) {
	if (b && *b) {
		free((void *) (*b)->bins);
		free((void *) (*b)->bdrs);
		free((void *) *b);
		*b = (LR_bin *) NULL;
		return 0;
	}
	return 1;
}

/*!
@brief	LR_bin_set(LR_bin *b, double x) - add bin boundary (will order internally).

@param	b	LR_bin object
@param	x	bin boundary to add
@return	0 if successful, else non-zero if failed
*/
int LR_bin_set(LR_bin *b, double x) {
	int i = 0;
	double t;
	if (b->n < b->nn + 1) {
		return -1;	/* too many values given */
	}
	t = b->bdrs[0];
	for (int i = 0, i1 = 1; i <= b->nn; i++,i1++) {
		if (b->nn == i1) {	/* got to top entry */
			b->bdrs[i] = x;
			b->nn++;
			return 0;
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

	return 0;
}

/*!
@brief	LR_bin_add(LR_bin *b, double x) - collect value to be binned.

@param	b	LR_bin object
@param	x	value to count within the given bin.
@return	0 if successful, else non-zero if failed
*/
int LR_bin_add(LR_bin *b, double x) {
	int i = 0;

	while (i <= b->nn - 1) {
		if (x < b->bdrs[i]
		||  i == b->nn - 1) {
			b->bins[i]++;
			break;
		}
		i++;
	}
	if (i >= b->nn)				return -1;

	b->c++;

	return 0;
}
