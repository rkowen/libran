/*!
\file	LRunif.c
\brief 	The uniform distribution on the [a,b) interval

The pseudo-random numbers are uniformly distributed from
the lower bound "a" upto "b" (on the interval [a,b) ).

Default:
- a = 0
- b = 1
 
*/
#include "libran.h"

/*!
@brief	LRd_unif_RAN(LR_obj *o) - double random uniform distribution

@param o	LR_obj object
@return double
*/
double LRd_unif_RAN(LR_obj *o) {
	double x, diff = (o->b.d - o->a.d); 
	x = o->ud();

	return o->a.d + x * diff;
}

/*!
@brief	LRd_unif_PDF(LR_obj *o, double x) - double uniform probablity distribution function

@param o	LR_obj object
@param x	value
@return double PDF at x
*/
double LRd_unif_PDF(LR_obj *o, double x) {
	double diff = (o->b.d - o->a.d); 

	if (x < o->a.d || x >= o->b.d) {
		return 0.0;
	} else {
		return 1.0 / diff;
	}
}

/*!
@brief	LRd_unif_CDF(LR_obj *o, double x) - double uniform cumulative distribution function

@param o	LR_obj object
@param x	value
@return double CDF at x
*/
double LRd_unif_CDF(LR_obj *o, double x) {
	double diff = (o->b.d - o->a.d); 

	if (x < o->a.d) {
		return 0.0;
	} else if (x >= o->b.d) {
		return 1.0;
	} else {
		return (x - o->a.d) / diff;
	}
}

