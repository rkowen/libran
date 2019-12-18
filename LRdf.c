/*!
\file	LRdf.c
\brief 	Call the associated functions for the given random variate distribution.

These routines are the \e generic function calls.  They use the
the distribution and generation functions defined for the given random variate
distribution, so the programmer need not remember the exact function
name when making calls to the distribution functions or the random variate
generater.

Also using these routines instead of calling the associated specific
functions allows one to quickly switch from one random variate
distribution to another without making any programmatic changes.

These routines call the associated PDF, CDF, RAN functions
given in the `LR_obj` object as specified by the `LR_type` and `LR_data_type`.

There are also \e generic auxiliary functions: new, rm, set, and norm.
The auxiliary methods are not dependent on the `LR_data_type`, but only
on the `LR_type` and only if they are necessary for the given
random variate distribution.

Auxiliary Functions
-------------------

The current set of auxiliary functions are needed for the
piecewise uniform (`LR_type` -> \e piece)
and the linear spline distributions (`LR_type` -> \e lspline).
The auxiliary object will be created when the `LR_obj` is created.

Use these functions instead of the
distribution specific versions.
These functions are safe to call from any other distribution, but will return
an error which can be ignored if the value returned is equal to
`LRerr_NoAuxiliaryObject`.

*/
/*
 * Copyright	2019	R.K. Owen, Ph.D.
 * License	see lgpl.md (Gnu Lesser General Public License)
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include "libran.h"

/*!
@brief	LRd_RAN(LR_obj *o) - random variate generator

This method will call the specific random variate generator as determined
by the object's `LR_type` and `LR_data_type`.  Note that using the wrong
data type will return a NAN and raise an error.

@param	o	LR_obj object
@return	double random variate
*/
double LRd_RAN(LR_obj *o) {
	if (o->d != LR_double) {
		o->errno = LRerr_BadDataType;
		return NAN;
	}
	return o->rnd(o);
}

/*!
@brief	LRd_PDF(LR_obj *o, double x) - probability distribution function

This method will call the specific random variate
probability distribution function
as determined
by the object's `LR_type` and `LR_data_type`.  Note that using the wrong
data type will return a NAN and raise an error.

@param	o	LR_obj object
@param	x	value
@return	double PDF at x
*/
double LRd_PDF(LR_obj *o, double x) {
	if (o->d != LR_double) {
		o->errno = LRerr_BadDataType;
		return NAN;
	}
	return o->pdfd(o,x);
}

/*!
@brief	LRd_CDF(LR_obj *o, double x) - cumulative distribution function

This method will call the specific random variate
cumulative distribution function
as determined
by the object's `LR_type` and `LR_data_type`.  Note that using the wrong
data type will return a NAN and raise an error.

@param	o	LR_obj object
@param	x	value
@return double CDF at x
*/
double LRd_CDF(LR_obj *o, double x) {
	if (o->d != LR_double) {
		o->errno = LRerr_BadDataType;
		return NAN;
	}
	return o->cdfd(o,x);
}

/*!
@brief	LRf_RAN(LR_obj *o) - random variate generator

This method will call the specific random variate generator as determined
by the object's `LR_type` and `LR_data_type`.  Note that using the wrong
data type will return a NAN and raise an error.

@param	o	LR_obj object
@return	float random variate
*/
float LRf_RAN(LR_obj *o) {
	if (o->d != LR_float) {
		o->errno = LRerr_BadDataType;
		return NAN;
	}
	return o->rnf(o);
}

/*!
@brief	LRf_PDF(LR_obj *o, float x) - probability distribution function

This method will call the specific random variate
probability distribution function
as determined
by the object's `LR_type` and `LR_data_type`.  Note that using the wrong
data type will return a NAN and raise an error.

@param	o	LR_obj object
@param	x	value
@return float PDF at x
*/
float LRf_PDF(LR_obj *o, float x) {
	if (o->d != LR_float) {
		o->errno = LRerr_BadDataType;
		return NAN;
	}
	return o->pdff(o,x);
}

/*!
@brief	LRf_CDF(LR_obj *o, float x) - cumulative distribution function

This method will call the specific random variate
cumulative distribution function
as determined
by the object's `LR_type` and `LR_data_type`.  Note that using the wrong
data type will return a NAN and raise an error.

@param	o	LR_obj object
@param	x	value
@return float CDF at x
*/
float LRf_CDF(LR_obj *o, float x) {
	if (o->d != LR_float) {
		o->errno = LRerr_BadDataType;
		return NAN;
	}
	return o->cdff(o,x);
}

/*!
@brief	LRi_RAN(LR_obj *o) - random variate generator

This method will call the specific random variate generator as determined
by the object's `LR_type` and `LR_data_type`.  Note that using the wrong
data type will return a NAN and raise an error.

@param	o	LR_obj object
@return	int random variate
*/
int LRi_RAN(LR_obj *o) {
	if (o->d != LR_int) {
		o->errno = LRerr_BadDataType;
		return NAN;
	}
	return o->rni(o);
}

/*!
@brief	LRi_PDF(LR_obj *o, int x) - probability distribution function

This method will call the specific random variate
probability distribution function
as determined
by the object's `LR_type` and `LR_data_type`.  Note that using the wrong
data type will return a NAN and raise an error.

@param	o	LR_obj object
@param	x	value
@return float PDF at x
*/
float LRi_PDF(LR_obj *o, int x) {
	if (o->d != LR_int) {
		o->errno = LRerr_BadDataType;
		return NAN;
	}
	return o->pdfi(o,x);
}

/*!
@brief	LRi_CDF(LR_obj *o, int x) - cumulative distribution function

This method will call the specific random variate
cumulative distribution function
as determined
by the object's `LR_type` and `LR_data_type`.  Note that using the wrong
data type will return a NAN and raise an error.

@param	o	LR_obj object
@param	x	value
@return float CDF at x
*/
float LRi_CDF(LR_obj *o, int x) {
	if (o->d != LR_int) {
		o->errno = LRerr_BadDataType;
		return NAN;
	}
	return o->cdfi(o,x);
}

/*!
@brief	LR_aux_new(LR_obj *o, int n) - initialize a new auxiliary object

The generic method for creating a new auxiliary object for those
random variate distributions
that need it.  Can be called otherwise but raises an ignorable error.


@param	o	LR_obj object
@param	n	maxiumum number of intervals
@return	0	if successful, else non-zero if failed
*/
int LR_aux_new(LR_obj *o, int n) {
	if (!o->aux)
		return o->errno = LRerr_NoAuxiliaryObject;
	if (!(((LR_pcs *) o->aux)->new))
		return o->errno = LRerr_BadAuxSetup;

	return ((LR_pcs *) o->aux)->new(o,n);
}

/*!
@brief	LR_aux_rm(LR_obj *o) - remove the auxiliary object

The generic method for removing or deallocating an auxiliary object for those
random variate distributions
that need it.  Can be called otherwise but raises an ignorable error.

@param	o	LR_obj object
@return	0	if successful, else non-zero if failed
*/
int LR_aux_rm(LR_obj *o) {
	if (!o->aux)
		return o->errno = LRerr_NoAuxiliaryObject;
	if (!(((LR_pcs *) o->aux)->rm))
		return o->errno = LRerr_BadAuxSetup;

	return ((LR_pcs *) o->aux)->rm(o);
}

/*!
@brief	LR_aux_set(LR_obj *o, double x, double p) - add an interval boundary
for the underlying distribution.

The generic method for adding an interval boundary for an auxiliary object
for those random variate distributions
that need it.  Can be called otherwise but raises an ignorable error.

@param	o	LR_obj object
@param	x	interval boundary to add
@param	p	relative probablity for interval greater than x
@return 0	if successful, else non-zero if failed
*/
int LR_aux_set(LR_obj *o, double x, double p) {
	if (!o->aux)
		return o->errno = LRerr_NoAuxiliaryObject;
	if (!(((LR_pcs *) o->aux)->set))
		return o->errno = LRerr_BadAuxSetup;

	return ((LR_pcs *) o->aux)->set(o,x,p);
}

/*!
@brief	LR_aux_norm(LR_obj *o) - process given intervals and normalize values.

The generic method for normalizing the given intervals for the auxiliary object
for those random variate distributions
that need it.  Can be called otherwise but raises an ignorable error.

@param	o	LR_obj object
@return 0	if successful, else non-zero if failed
*/
int LR_aux_norm(LR_obj *o) {
	int ret;
	if (!o->aux)
		return o->errno = LRerr_NoAuxiliaryObject;
	if (!(((LR_pcs *) o->aux)->normalize))
		return o->errno = LRerr_BadAuxSetup;

	return ((LR_pcs *) o->aux)->normalize(o);
}

#ifdef __cplusplus
}
#endif
