/*!
\file	LRdf.c
\brief 	call the associated PDF or CDF for the given distribution function

Use these routines instead of calling the associated PDF or CDF.
These routines will call the PDF or CDF given in the LR_obj object
as specified by the LR_type and LR_data_type.

*/
#include "libran.h"

/*!
@brief	LRd_RAN(LR_obj *o) - random variate generator

@param o	LR_obj object
@return double
*/
double LRd_RAN(LR_obj *o) {
	return o->rnd(o);
}

/*!
@brief	LRd_PDF(LR_obj *o, double x) - probability distribution function

@param o	LR_obj object
@param x	value
@return double PDF at x
*/
double LRd_PDF(LR_obj *o, double x) {
	return o->pdfd(o,x);
}

/*!
@brief	LRd_CDF(LR_obj *o, double x) - cumulative distribution function

@param o	LR_obj object
@param x	value
@return double CDF at x
*/
double LRd_CDF(LR_obj *o, double x) {
	return o->cdfd(o,x);
}

/*!
@brief	LRf_RAN(LR_obj *o) - random variate generator

@param o	LR_obj object
@return float
*/
float LRf_RAN(LR_obj *o) {
	return o->rnf(o);
}

/*!
@brief	LRd_PDF(LR_obj *o, float x) - probability distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRf_PDF(LR_obj *o, float x) {
	return o->pdff(o,x);
}

/*!
@brief	LRd_CDF(LR_obj *o, float x) - cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRf_CDF(LR_obj *o, float x) {
	return o->cdff(o,x);
}

