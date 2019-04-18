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

	if (!(ptr->sc = (double *) calloc(n + 1, sizeof(double))))
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
	if (p < 0) {
		return -2;	/* invalid probability */
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
	double zero = 0.0, one = 1.0, delta = .000001;
	double v = zero;
	LR_pcs *aux = (LR_pcs *) o->aux;

	/* move up the set of values */
	for (int i = aux->nn - 1, i1 = aux->nn - 2; i >= 1; i--, i1--) {
		aux->bdrs[i] = aux->bdrs[i1];
		aux->c[i] = aux->c[i1];
	}
	if (o->d == LR_double) {
		aux->bdrs[0] = o->a.d;
		aux->bdrs[aux->nn] = o->b.d;
		aux->c[0] = o->x.d;
	} else if (o->d == LR_float) {
		aux->bdrs[0] = (double) o->a.f;
		aux->bdrs[aux->nn] = (double) o->b.f;
		aux->c[0] = (double) o->x.f;
	}

	/* integrate over each interval */
	aux->norm = zero;
	for (int i = 0, i1 = 1; i < aux->nn; i++, i1++) {
		aux->norm += 
		aux->sc[i] = aux->c[i] * (aux->bdrs[i1] - aux->bdrs[i]);
	}

	/* collect and rescale the CDF */
	aux->norm = one/aux->norm;
	for (int i = 0; i < aux->nn; i++) {
		v += aux->sc[i];
		aux->sc[i] = v * aux->norm;
	}
	/* shift the CDF set up */
	if (aux->sc[aux->nn-1] < one - delta
	||  one + delta < aux->sc[aux->nn-1]) {
		/* the last value should be 1.0 */
		return 1;
	}
	for (int i = aux->nn-1; i > 0; i--) {
		aux->sc[i] = aux->sc[i-1];
	}
	aux->sc[0] = zero;
	aux->sc[aux->nn] = one;

	return 0;
}

/*!
@brief	LRd_piece_RAN(LR_obj *o) - double random piecewise uniform distribution

@param o	LR_obj object
@return double
*/
double LRd_piece_RAN(LR_obj *o) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	double x, dx, zero = 0.0;
	int i = 0;

	x = o->ud();
	/* find interval */
	while (x > aux->sc[i])	i++;

	if (aux->c[i-1] == zero)	return aux->bdrs[i-1];

	dx = (x - aux->sc[i-1]) / (aux->c[i-1] * aux->norm);
	return aux->bdrs[i-1] + dx;
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
	double zero = 0.0, one = 1.0;

	if (x <= o->a.d) {
		return zero;
	} else if (x >= o->b.d) {
		return one;
	} else {
		/* find interval */
		int i = 0;
		double diff = zero; 
		while (x > aux->bdrs[i])	i++;
		diff = (aux->bdrs[i] - aux->bdrs[i-1]);
		return aux->sc[i-1] + aux->c[i-1]*aux->norm*(x-aux->bdrs[i-1]);
	}
}

/*!
@brief	LRf_piece_RAN(LR_obj *o) - float random piecewise uniform distribution

@param o	LR_obj object
@return float
*/
float LRf_piece_RAN(LR_obj *o) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	float x, dx, zero = 0.0;
	int i = 0;

	x = o->ud();
	/* find interval */
	while (x > aux->sc[i])	i++;

	if (aux->c[i-1] == zero)	return aux->bdrs[i-1];

	dx = (x - aux->sc[i-1]) / (aux->c[i-1] * aux->norm);
	return aux->bdrs[i-1] + dx;
}

/*!
@brief	LRf_piece_PDF(LR_obj *o, float x) - float piecewise uniform probablity distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRf_piece_PDF(LR_obj *o, float x) {
	LR_pcs *aux = (LR_pcs *) o->aux;

	if (x <= o->a.f || x > o->b.f) {
		return 0.0;
	} else {
		/* find interval */
		int i = 0;
		while (x > aux->bdrs[i])	i++;
		return aux->c[i-1] * aux->norm;
	}
}

/*!
@brief	LRf_piece_CDF(LR_obj *o, float x) - float piecewise uniform cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRf_piece_CDF(LR_obj *o, float x) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	float zero = 0.0, one = 1.0;

	if (x <= o->a.f) {
		return zero;
	} else if (x >= o->b.f) {
		return one;
	} else {
		/* find interval */
		int i = 0;
		float diff = zero; 
		while (x > aux->bdrs[i])	i++;
		diff = (aux->bdrs[i] - aux->bdrs[i-1]);
		return aux->sc[i-1] + aux->c[i-1]*aux->norm*(x-aux->bdrs[i-1]);
	}
}

