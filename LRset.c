/*!
\file	LRset.c
\brief	Set of methods to handle setting the various LR_obj parameters.
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>	/* va_arg */
#include <string.h>	/* memcpy */
#include "libran.h"

/*!
@brief	LR_vset(LR_obj *o, char *x, va_list ap) - read and parse options

@param	o	LR_obj object
@param	x	controlling format string
@param	ap	additional arguments context information
@return	negative number of non-valid values in format string or error if > 0

This function is similar to vprintf() where the optional arguments are scanned
according to format *x.  The function  accesses additional arguments by using
the context information designated by ap.
The program must execute va_start() before calling this function, and call
va_end() afterward.

*/
int LR_vset(LR_obj *o, char *x, va_list ap) {
	LR_val t;
	char x1;
	int ret = 0;

	while (x1 = *x++) {
		switch (o->d) {
		case LR_int:
			t.i = va_arg(ap, int);
			break;
		case LR_long:
			t.l = va_arg(ap, long);
			break;
		case LR_float:
			/* floats are promoted to double in ... */
			t.f = va_arg(ap, double);
			break;
		case LR_double:
			t.d = va_arg(ap, double);
			break;
		default:
			/* should not get here unless a new type was added */
			return o->errno = LRerr_BadDataType;
		}

		switch (x1) {
		case	'd':
			(void) memcpy(&(o->d), &t, sizeof(LR_val));
			break;
		case	'a':
			(void) memcpy(&(o->a), &t, sizeof(LR_val));
			break;
		case	'b':
			(void) memcpy(&(o->b), &t, sizeof(LR_val));
			break;
		case	'm':
			(void) memcpy(&(o->m), &t, sizeof(LR_val));
			break;
		case	's':
			(void) memcpy(&(o->s), &t, sizeof(LR_val));
			break;
		case	'x':
			(void) memcpy(&(o->x), &t, sizeof(LR_val));
			break;
		default:
			/* it is an error to include non-object attributes */
			/* but continue on and process remaining */
			ret++;
		}
	}
	return -ret;
}

/*!
@brief	LR_set(LR_obj *o, char x, ...) - set a single LR object parameter

@param	o	LR_obj object
@param	x	single character format
@param	...	optional argument
@return	negative number of non-valid values in format string or error if > 0
*/
int LR_set(LR_obj *o, char x, ...) {
	va_list ap;
	int ret = 0;
	char xx[2] = "0";

	va_start(ap, x);

	xx[0] = x;
	ret = LR_vset(o, xx, ap);

	va_end(ap);

	return ret;
}

/*!
@brief	LR_set_all(LR_obj *o, char *x, ...) - set all given LR object parameters

@param	o	LR_obj object
@param	x	format string
@param	...	optional arguments
@return	negative number of non-valid values in format string or error if > 0
*/
int LR_set_all(LR_obj *o, char *x, ...) {
	va_list ap;
	int ret = 0, val;

	va_start(ap, x);

	val = LR_vset(o, x, ap);
	if (val < 0)
		ret += val;
	va_end(ap);

	return ret;
}

#ifdef __cplusplus
}
#endif
