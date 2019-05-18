/*!
\file	LRlspline.c
\brief	Piecewise linear spline distribution

Borrows many of the LRpiece.c functions since the two are very close.

The CDF is pinned to zero at the end points.  So you will need to add
at least one point in between with a non-zero value. 

*/
#include <stdlib.h>
#include <math.h>	/* sqrt, sqrtf */
#include "libran.h"
#include <stdio.h>

/*!
@brief	LR_lspl_new(LR_obj *o, int n) - create a new linear spline object

@param	o	LR_obj object
@param	n	largest number of intervals
@return	0 if successful, else non-zero if failed
*/
int LR_lspl_new(LR_obj *o, int n) {
	return LR_pcs_new(o,n);
}

/*!
@brief	LR_lspl_rm(LR_obj *o) - strip down the LR_lspl object part of LR_obj

@param	o	LR_obj object address
@return	0 if successful, else non-zero if failed
*/
int LR_lspl_rm(LR_obj *o) {
	return LR_pcs_rm(o);
}

/*!
@brief	LR_lspl_set(LR_obj *o, double x) - add interval boundary (will order internally).

@param	o	LR_obj object
@param	x	interval boundary to add
@param	p	relative probablity for interval starting at x
@return	0 if successful, else non-zero if failed
*/
int LR_lspl_set(LR_obj *o, double x, double p) {
	return LR_pcs_set(o,x,p);
}

/*!
@brief	LR_lspl_norm(LR_obj *o) - normalize the interval scale factors.

@param	o	LR_obj object
@return	0 if successful, else non-zero if failed
*/
int LR_lspl_norm(LR_obj *o) {
	double zero = 0.0, half = 0.5, one = 1.0, delta = .000001;
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
		aux->sc[i] = half * (aux->c[i1] + aux->c[i])
			* (aux->bdrs[i1] - aux->bdrs[i]);
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
@brief	LRd_lspline_RAN(LR_obj *o) - double random linear spline distribution

@param o	LR_obj object
@return double
*/
double LRd_lspline_RAN(LR_obj *o) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	double x, y, dy, slope, zero = 0.0, two = 2.0;
	int i = 0;

	x = o->ud();
	/* find interval */
	while (x >= aux->sc[i])	i++;

	/* use inverse method to return variate */
	slope = ((aux->c[i] - aux->c[i-1]) * aux->norm) /
		 (aux->bdrs[i] - aux->bdrs[i-1]);

	y = aux->c[i-1] * aux->norm;
	x -= aux->sc[i-1];
	dy = (sqrt(y*y + two*slope*(x)) - y)/slope;

	y = aux->bdrs[i-1] + dy;
/*
	printf(">>>>>% 8.5f % 8.5f % 8.5f\n",
		y, aux->sc[i-1] + x, LRd_CDF(o, y));
*/
	return y;
}

/*!
@brief	LRd_lspline_PDF(LR_obj *o, double x) - double linear spline probablity distribution function

@param o	LR_obj object
@param x	value
@return double PDF at x
*/
double LRd_lspline_PDF(LR_obj *o, double x) {
	LR_pcs *aux = (LR_pcs *) o->aux;

	if (x <= o->a.d || x >= o->b.d) {
		return 0.0;
	} else {
		/* find interval */
		int i = 0;
		double slope, y;
		while (x > aux->bdrs[i])	i++;
		slope = (aux->c[i] - aux->c[i-1]) /
			(aux->bdrs[i] - aux->bdrs[i-1]);
		y =	(aux->c[i-1] + (slope * (x - aux->bdrs[i-1])))
			*aux->norm;
/*
	printf("lspline_PDF>>>>>% 8.5f % 8.5f\n", x, y);
*/
		return y;
	}
}

/*!
@brief	LRd_lspline_CDF(LR_obj *o, double x) - double linear spline cumulative distribution function

@param o	LR_obj object
@param x	value
@return double CDF at x
*/
double LRd_lspline_CDF(LR_obj *o, double x) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	double zero = 0.0, one = 1.0, half = 0.5, y;

	if (x <= o->a.d) {
		return zero;
	} else if (x >= o->b.d) {
		return one;
	} else {
		/* find interval */
		int i = 0;
		double diff, slope;
		while (x > aux->bdrs[i])	i++;
		diff =	(x - aux->bdrs[i-1]);
		slope = (aux->c[i] - aux->c[i-1]) /
			(aux->bdrs[i] - aux->bdrs[i-1]);
		y =	(aux->c[i-1] + (slope * diff));
		y = aux->sc[i-1] + half * (y + aux->c[i-1])*diff*aux->norm;
/*
		printf("lspline_CDF>>>>>% 8.5f % 8.5f\n", x, y);
*/
		return y;
	}
}

/*!
@brief	LRf_lspline_RAN(LR_obj *o) - float random linear spline distribution

@param o	LR_obj object
@return float
*/
float LRf_lspline_RAN(LR_obj *o) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	float x, y, dy, slope, zero = 0.0, two = 2.0;
	int i = 0;

	x = o->uf();
	/* find interval */
	while (x >= aux->sc[i])	i++;

	/* use inverse method to return variate */
	slope = ((aux->c[i] - aux->c[i-1]) * aux->norm) /
		 (aux->bdrs[i] - aux->bdrs[i-1]);

	y = aux->c[i-1] * aux->norm;
	x -= aux->sc[i-1];
	dy = (sqrtf(y*y + two*slope*(x)) - y)/slope;

	y = aux->bdrs[i-1] + dy;
/*
	printf(">>>>>% 8.5f % 8.5f % 8.5f\n",
		y, aux->sc[i-1] + x, LRf_CDF(o, y));
*/
	return y;
}

/*!
@brief	LRf_lspline_PDF(LR_obj *o, float x) - float linear spline probablity distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRf_lspline_PDF(LR_obj *o, float x) {
	LR_pcs *aux = (LR_pcs *) o->aux;

	if (x <= o->a.f || x >= o->b.f) {
		return 0.0;
	} else {
		/* find interval */
		int i = 0;
		float slope, y;
		while (x > aux->bdrs[i])	i++;
		slope = (aux->c[i] - aux->c[i-1]) /
			(aux->bdrs[i] - aux->bdrs[i-1]);
		y =	(aux->c[i-1] + (slope * (x - aux->bdrs[i-1])))
			*aux->norm;
/*
	printf("lspline_PDF>>>>>% 8.5f % 8.5f\n", x, y);
*/
		return y;
	}
}

/*!
@brief	LRf_lspline_CDF(LR_obj *o, float x) - float linear spline cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRf_lspline_CDF(LR_obj *o, float x) {
	LR_pcs *aux = (LR_pcs *) o->aux;
	float zero = 0.0, one = 1.0, half = 0.5, y;

	if (x <= o->a.f) {
		return zero;
	} else if (x >= o->b.f) {
		return one;
	} else {
		/* find interval */
		int i = 0;
		float diff, slope;
		while (x > aux->bdrs[i])	i++;
		diff =	(x - aux->bdrs[i-1]);
		slope = (aux->c[i] - aux->c[i-1]) /
			(aux->bdrs[i] - aux->bdrs[i-1]);
		y =	(aux->c[i-1] + (slope * diff));
		y = aux->sc[i-1] + half * (y + aux->c[i-1])*diff*aux->norm;
/*
		printf("lspline_CDF>>>>>% 8.5f % 8.5f\n", x, y);
*/
		return y;
	}
}

