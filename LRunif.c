/*!
\file	LRunif.c
\brief 	The uniform distribution on the [a,b) interval

The pseudo-random numbers are uniformly distributed on an interval
from the lower bound "a" upto "b" - the interval [a,b).

\manonly
   PDF	u(x) =	{ 0		if x < a || x >= b
		{ 1/(b-a)	if a <= x < b

   CDF	U(x) =	{ 0		if x < a
		{ (x-a)/(b-a)	if a <= x < b
		{ 1		if b <= x
\endmanonly

\f{eqnarray*}{
\mbox{PDF } u(x)	&=
	\left\{ \begin{array}{ll}
		0,		& x < a \mbox{ or } x \ge b,	\\
		\frac{1}{b-a},	& a \le x < b ,			\\
	\end{array} \right.
\\
\mbox{CDF } U(x)	&=
	\left\{ \begin{array}{ll}
		0,			& x < a ,		\\
		\frac{x-a}{b-a},	& a \le x < b ,		\\
		1,			& x \ge b .
	\end{array} \right.
\f}

Default:
- \e a = 0
- \e b = 1

\see urand/urand.c
 
*/
#ifdef __cplusplus
extern "C" {
#endif

#include "libran.h"

/*!
@brief	LRd_unif_RAN(LR_obj *o) - double random uniform distribution

@param o	LR_obj object
@return double
*/
double LRd_unif_RAN(LR_obj *o) {
	double x, diff = (o->b.d - o->a.d); 
	x = o->ud(o);

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

/*!
@brief	LRf_unif_RAN(LR_obj *o) - float random uniform distribution

@param o	LR_obj object
@return float
*/
float LRf_unif_RAN(LR_obj *o) {
	float x, diff = (o->b.f - o->a.f); 
	x = o->uf(o);

	return o->a.f + x * diff;
}

/*!
@brief	LRf_unif_PDF(LR_obj *o, float x) - float uniform probablity distribution function

@param o	LR_obj object
@param x	value
@return float PDF at x
*/
float LRf_unif_PDF(LR_obj *o, float x) {
	float diff = (o->b.f - o->a.f); 

	if (x < o->a.f || x >= o->b.f) {
		return 0.0;
	} else {
		return 1.0 / diff;
	}
}

/*!
@brief	LRf_unif_CDF(LR_obj *o, float x) - float uniform cumulative distribution function

@param o	LR_obj object
@param x	value
@return float CDF at x
*/
float LRf_unif_CDF(LR_obj *o, float x) {
	float diff = (o->b.f - o->a.f); 

	if (x < o->a.f) {
		return 0.0;
	} else if (x >= o->b.f) {
		return 1.0;
	} else {
		return (x - o->a.f) / diff;
	}
}

#ifdef __cplusplus
}
#endif
