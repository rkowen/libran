/*!
\file	LRpiece.c
\brief	Set of binning functions

This set of functions: set-up, count values into bins, and take-down

*/
#include <stdlib.h>
#include "libran.h"

/*!
@brief	LR_pcs_new(LR_obj *o, int n) - create a new piecewise uniform object

@param	o	LR_obj object
@param	n	largest number of intervals
@return	0 if successful, else non-zero if failed
*/
int LR_pcs_new(LR_obj *o, int n) {
	LR_pcs *ptr = (LR_pcs *) o->aux;

	if (o->t != piece)
		return -2;

	ptr->n  = n;
	ptr->nn = 1;	/* always start with one interval */
	ptr->c  = 0;
	ptr->norm  = 0.;

	if (!(ptr->bdrs = (double *) calloc(n, sizeof(double))))
		goto bad0;

	if (!(ptr->c = (double *) calloc(n, sizeof(double))))
		goto bad1;

	if (!(ptr->sc = (double *) calloc(n, sizeof(double))))
		goto bad2;

	return 0;

bad2:
	free((void *) ptr->sc);

bad1:
	free((void *) ptr->c);

bad0:
	free((void *) ptr->bdrs);

	return -1;
}

/*!
@brief	LR_pcs_rm(LR_obj *o) - strip down the LR_pcs object part of LR_obj

@param	o	LR_obj object address
@return	0 if successful, else non-zero if failed
*/
int LR_pcs_rm(LR_obj *o) {
	LR_pcs *aux;
	if (o && o->t == piece && o->aux) {
		aux = (LR_pcs *) o->aux;
		free((void *) aux->bdrs);
		free((void *) aux->c);
		free((void *) aux->sc);
		return 0;
	}
	return 1;
}

/*!
@brief	LR_pcs_set(LR_obj *o, double x) - add interval boundary (will order internally).

@param	o	LR_obj object
@param	x	interval boundary to add
@param	p	relative probablity for interval greater than x
@return	0 if successful, else non-zero if failed
*/
int LR_pcs_set(LR_obj *o, double x, double p) {
	int i = 0;
	double t, tp;
	LR_pcs *aux = (LR_pcs *) o->aux;
	if (aux->n < aux->nn + 1) {
		return -1;	/* too many values given */
	}
	t  = aux->bdrs[0];
	tp = aux->c[0];
	for (int i = 0, i1 = 1; i <= aux->nn; i++,i1++) {
		if (aux->nn == i1) {	/* got to top entry */
			aux->bdrs[i] = x;
			aux->c[i] = p;
			aux->nn++;
			return 0;
		}
		if (x > t) {
			/* compare to next one, next round */
			t  = aux->bdrs[i1];
			tp = aux->c[i1];
		} else {
			/* insert here, but keep current */
			t  = aux->bdrs[i];
			tp = aux->c[i];
			aux->bdrs[i] = x;
			/* use current for comparison */
			x = t;
			p = tp;
			t  = aux->bdrs[i1];
			tp = aux->c[i1];
		}
	}
	aux->nn++;

	return 0;
}

/*!
@brief	LR_pcs_norm(LR_obj *o) - normalize the interval scale factors.

@param	o	LR_obj object
@return	0 if successful, else non-zero if failed
*/
int LR_pcs_norm(LR_obj *o) {
	double v = 0.0;
	LR_pcs *aux = (LR_pcs *) o->aux;

	/* move up the set of values */
	for (int i = aux->nn - 1, i1 = aux->nn - 2; i >= 1; i--, i1--) {
		aux->bdrs[i] = aux->bdrs[i1];
		aux->c[i] = aux->c[i1];
	}
	if (o->t == LR_double) {
		aux->bdrs[0] = o->a.d;
		aux->bdrs[aux->nn] = o->b.d;
		aux->c[0] = o->x.d;
	} else if (o->t == LR_float) {
		aux->bdrs[0] = (double) o->a.f;
		aux->bdrs[aux->nn] = o->b.f;
		aux->c[0] = (double) o->x.f;
	}

	/* integrate over each interval */
	aux->norm = 0.0;
	for (int i = 0, i1 = 1; i < aux->nn; i++, i1++) {
		aux->norm += 
		aux->sc[i] = aux->c[i] * (aux->bdrs[i1] - aux->bdrs[i]);
	}

	/* collect and rescale the CDF */
	aux->norm = 1.0/aux->norm;
	for (int i = 0; i < aux->nn; i++) {
		v += aux->sc[i];
		aux->sc[i] = v * aux->norm;
	}

	return 0;
}

/*!
@brief	LRd_piece_RAN(LR_obj *o) - double random piecewise uniform distribution

@param o	LR_obj object
@return double
*/
double LRd_piece_RAN(LR_obj *o) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	double x, diff = (o->b.d - o->a.d); 
	x = o->ud();

	return o->a.d + x * diff;
}

/*!
@brief	LRd_piece_PDF(LR_obj *o, double x) - double piecewise uniform probablity distribution function

@param o	LR_obj object
@param x	value
@return double PDF at x
*/
double LRd_piece_PDF(LR_obj *o, double x) {
	LR_pcs *aux = (LR_pcs *) o->aux;

	if (x <= o->a.d || x > o->b.d) {
		return 0.0;
	} else {
		/* find interval */
		int i = 0;
		while (x > aux->bdrs[i])	i++;
		return aux->c[i-1] * aux->norm;
	}
}

/*!
@brief	LRd_piece_CDF(LR_obj *o, double x) - double piecewise uniform cumulative distribution function

@param o	LR_obj object
@param x	value
@return double CDF at x
*/
double LRd_piece_CDF(LR_obj *o, double x) {
	LR_pcs *aux = (LR_pcs *) o->aux;

	if (x <= o->a.d) {
		return 0.0;
	} else if (x >= o->b.d) {
		return 1.0;
	} else {
		/* find interval */
		int i = 0;
		double diff = 0.0; 
		while (x > aux->bdrs[i])	i++;
		diff = (aux->bdrs[i] - aux->bdrs[i-1]);
		return aux->sc[i-1] - aux->c[i-1]*aux->norm*(aux->bdrs[i]-x)*diff;
	}
}

