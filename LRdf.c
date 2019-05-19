/*!
\file	LRdf.c
\brief 	call the associated PDF or CDF for the given distribution function

Use these routines instead of calling the associated PDF or CDF.
These routines will call the PDF or CDF given in the LR_obj object
as specified by the LR_type and LR_data_type.

You can also do the same for the auxiliary functions: new, rm, set, and norm.
The auxiliary methods are not dependent on the LR_data_type.

*/
#include "libran.h"

/*!
@brief	LRd_RAN(LR_obj *o) - random variate generator

@param	o	LR_obj object
@return	double
*/
double LRd_RAN(LR_obj *o) {
	return o->rnd(o);
}

/*!
@brief	LRd_PDF(LR_obj *o, double x) - probability distribution function

@param	o	LR_obj object
@param	x	value
@return	double PDF at x
*/
double LRd_PDF(LR_obj *o, double x) {
	return o->pdfd(o,x);
}

/*!
@brief	LRd_CDF(LR_obj *o, double x) - cumulative distribution function

@param	o	LR_obj object
@param	x	value
@return double CDF at x
*/
double LRd_CDF(LR_obj *o, double x) {
	return o->cdfd(o,x);
}

/*!
@brief	LRf_RAN(LR_obj *o) - random variate generator

@param	o	LR_obj object
@return float
*/
float LRf_RAN(LR_obj *o) {
	return o->rnf(o);
}

/*!
@brief	LRd_PDF(LR_obj *o, float x) - probability distribution function

@param	o	LR_obj object
@param	x	value
@return float PDF at x
*/
float LRf_PDF(LR_obj *o, float x) {
	return o->pdff(o,x);
}

/*!
@brief	LRd_CDF(LR_obj *o, float x) - cumulative distribution function

@param	o	LR_obj object
@param	x	value
@return float CDF at x
*/
float LRf_CDF(LR_obj *o, float x) {
	return o->cdff(o,x);
}

/*!
The current set of auxiliary functions are needed for the piecewise uniform
and the linear spline distributions.  Use these functions instead of the
distribution specific versions.
These functions are safe to call from any other distribution, but will return
an error.
*/
/*!
@brief	LR_aux_new(LR_obj *o, int n) - initialize a new auxiliary object

@param	o	LR_obj object
@param	n	maxiumum number of intervals
@return	0	if successful, else non-zero if failed
*/
int LR_aux_new(LR_obj *o, int n) {
	if (!o->aux)
		return 9;
	if (!(((LR_pcs *) o->aux)->new))
		return 11;

	return ((LR_pcs *) o->aux)->new(o,n);
}

/*!
@brief	LR_aux_rm(LR_obj *o, int n) - remove the auxiliary object

@param	o	LR_obj object
@return	0	if successful, else non-zero if failed
*/
int LR_aux_rm(LR_obj *o) {
	if (!o->aux)
		return 9;
	if (!(((LR_pcs *) o->aux)->rm))
		return 11;

	return ((LR_pcs *) o->aux)->rm(o);
}

/*!
@brief	LR_aux_set(LR_obj *o, int n) - add an interval boundary for underlying
	distribution.

@param	o	LR_obj object
@param	x	interval boundary to add
@param	p	relative probablity for interval greater than x
@return 0	if successful, else non-zero if failed
*/
int LR_aux_set(LR_obj *o, double x, double p) {
	if (!o->aux)
		return 9;
	if (!(((LR_pcs *) o->aux)->set))
		return 11;

	return ((LR_pcs *) o->aux)->set(o,x,p);
}

/*!
@brief	LR_aux_norm(LR_obj *o) - process given intervals and normalize values.

@param	o	LR_obj object
@return 0	if successful, else non-zero if failed
*/
int LR_aux_norm(LR_obj *o) {
	int ret;
	if (!o->aux)
		return 9;
	if (!(((LR_pcs *) o->aux)->normalize))
		return 11;

	return ((LR_pcs *) o->aux)->normalize(o);
}

