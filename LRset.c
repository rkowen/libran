/*!
\file	LRset.c
\brief	Collection of methods to handle setting the various LR_obj parameters.

The \c LR_obj contains many attributes for the various random variate
distributions.  These routines provide a simple interface for accomplishing
initializing these attributes.

Here is an example of setting the end points for the uniformly
distributed pseudo-random number generator.

\code
#include "libran.h"
...
LR_obj *o = LR_new(unif, LR_double);
// set the end points
if (LR_set_all(o, "ab", 2.0, 5.0) > 0) {
	// a serious error
	LRperror("MyApp", o->errno);
	...
}
// set the seed
LR_lsetseed(19580512L);
...
// generate random number between 2.0 and 5.0
x = LR_RAN(o);
...
// remove object
LR_rm(&o);
...
\endcode

*/
/*
 * Copyright	2019	R.K. Owen, Ph.D.
 * License	see lgpl.md (Gnu Lesser General Public License)
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>	/* va_arg */
#include <string.h>	/* memcpy */
#include "libran.h"

/*!
@brief	LR_vset(LR_obj *o, char *x, va_list ap) - read and parse options

This function is similar to vprintf() where the optional arguments are scanned
according to format *x.  The function  accesses additional arguments by using
the context information designated by `ap`.
The program must execute `va_start()` before calling this function, and call
`va_end()` afterward.

Generally there is no need to call this routine.  Call `LR_set_all()` instead.

However, the list of attributes handled is important -
'k','n', 'p', 'q', 'd', 'a', 'b', 'm', 's', and 'x'.
Any other characters are ignored except causing a negative value
to be returned representing the number of such ignored characters.
A \e real error will result in a positive return value.

All other characters in the \e format string are an ignorable error.

The precision of the input values are governed by the \c LR_obj
\c LR_data_type, except for 'k','n' which are \c int only, and
except for 'p','q' which are \c float only.

@param	o	LR_obj object
@param	x	controlling format string
@param	ap	additional arguments context information
@return	0	if no error else < 0 for non-valid characters in format string
		or an error if > 0
*/
int LR_vset(LR_obj *o, char *x, va_list ap) {
	LR_val	t;
	int	i;
	float	f;
	char	x1;
	int	ret = 0;

	while (x1 = *x++) {
		switch (x1) {
		case	'k':
			i = va_arg(ap, int);
			(void) memcpy(&(o->k), &i, sizeof(int));
			break;
		case	'n':
			i = va_arg(ap, int);
			(void) memcpy(&(o->n), &i, sizeof(int));
			break;
		case	'p':
			f = va_arg(ap, double);
			(void) memcpy(&(o->p), &f, sizeof(float));
			break;
		case	'q':
			f = va_arg(ap, double);
			(void) memcpy(&(o->q), &f, sizeof(float));
			break;
		default: /* rest are dependent on LR_type */
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
		case	'k':
		case	'n':
		case	'p':
		case	'q':
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

This routine is designed to set a single \c LR_obj attribute as directed
by the \e format character.  See \c LR_vset() for allowed list of
format characters.

@param	o	LR_obj object
@param	x	single character format
@param	...	optional argument
@return	negative number of non-valid value in format string or error if > 0
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

Set a collection of \c LR_obj attributes.  This routine has any
number of arguments as defined by the \e format string
(similar to printf() ).

See LR_vset() for list of allowed format string characters.

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
