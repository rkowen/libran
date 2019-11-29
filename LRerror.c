/*!
\file	LRerror.c
\brief	LibRan error codes and messages

The LibRan error routines are patterned after the Standard C perror()/strerror()
routines and the like.

The object creation routines like `LR_new` and `LR_bin_new` will return
a NULL value if they fail.  No error value is returned otherwise.

Many of the LibRan routines return a random variate or
distribution function value.  When these result in an error these functions
return NAN (Not-A-Number).

The `LR_obj` and `LR_bin` objects have an \e errno attribute which can
be queried.

An example code fragment:

\code
#include <math.h>
#include "libran.h"
...
LR_obj *o =  LR_new(piece, LR_double);
if (!*o) { // handle error }
...
if (isnan(x = LRd_RAN(o)) {
	// handle error
	LRperror("MyApp",o->errno);
}
...
\endcode

However, the other routines, especially those
that return an integer value are returning a LibRan error value if non-zero.

\code
#include "libran.h"
...
LR_bin *b =  LR_bin_new(100);
if (!*b) { // handle error }
...
if (!LR_bin_set(b,2.0)) {
	// handle error
	LRperror("MyApp",b->errno);
}
...
\endcode

Use these routines to identify the specific LibRan error.

*/
/*
 * Copyright	2019	R.K. Owen, Ph.D.
 * License	see lgpl.md (Gnu Lesser General Public License)
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "libran.h"

/*!
@brief	LRstrerror(int LRerrno)	- return some explanatory text regarding
the given LibRan errno.

@param	LRerrno	LibRan error number
@return	char * pointer to static constant string.
*/
char *LRstrerror(int LRerrno) {
	switch (LRerrno) {
	case	LRerr_OK:
		return	"LibRan - No Error";
	case	LRerr_Unspecified:
		return	"LibRan - Unspecified Error";
	case	LRerr_BadDataType:
		return	"LibRan - Bad Data Type given";
	case	LRerr_BadLRType:
		return	"LibRan - Bad Random Variate Type given";
	case	LRerr_NoAuxiliaryObject:
		return	"LibRan - Required Auxiliary Object not found";
	case	LRerr_NoAuxNormalizeDone:
		return	"LibRan - Auxiliary Object requires normalization";
	case	LRerr_BadAuxSetup:
		return	"LibRan - Auxiliary Object incorrectly set-up";
	case	LRerr_BinGeneric:
		return	"LibRan - Binning Object Unspecified Error";
	case	LRerr_TooManyValues:
		return	"LibRan - Too Many Values given";
	case	LRerr_InvalidInputValue:
		return	"LibRan - Invalid Input Value Error";
	case	LRerr_InvalidRange:
		return	"LibRan - Invalid Range Value Error";
	case	LRerr_UnmetPreconditions:
		return	"LibRan - Preconditions Not Performed";
	case	LRerr_SuspiciousValues:
		return	"LibRan - Suspicious Value - Normalization Error?";
	case	LRerr_AllocFail:
		return	"LibRan - Memory Allocation Error";
	}
	return	"LibRan - Invalid errno";
}

/*!
@brief	LRstrerrno(int LRerrno)	- return the errno code string for the
given LibRan errno.

@param	LRerrno	LibRan error number
@return	char * pointer to static constant string.
*/
char *LRstrerrno(int LRerrno) {
	switch (LRerrno) {
	case	LRerr_OK:
		return	"LRerr_OK";
	case	LRerr_Unspecified:
		return	"LRerr_Unspecified";
	case	LRerr_BadDataType:
		return	"LRerr_BadDataType";
	case	LRerr_BadLRType:
		return	"LRerr_BadLRType";
	case	LRerr_NoAuxiliaryObject:
		return	"LRerr_NoAuxiliaryObject";
	case	LRerr_NoAuxNormalizeDone:
		return	"LRerr_NoAuxNormalizeDone";
	case	LRerr_BadAuxSetup:
		return	"LRerr_BadAuxSetup";
	case	LRerr_BinGeneric:
		return	"LRerr_BinGeneric";
	case	LRerr_TooManyValues:
		return	"LRerr_TooManyValues";
	case	LRerr_InvalidInputValue:
		return	"LRerr_InvalidInputValue";
	case	LRerr_InvalidRange:
		return	"LRerr_InvalidRange";
	case	LRerr_UnmetPreconditions:
		return	"LRerr_UnmetPreconditions";
	case	LRerr_SuspiciousValues:
		return	"LRerr_SuspiciousValues";
	case	LRerr_AllocFail:
		return	"LRerr_AllocFail";
	}
	return	"";
}

/*!
@brief	LRperror(char *str, int LRerrno) - print out explanatory error
message to stderr.

If the char `str` string is not NULL then prepend this string to the LibRan
error message with a " : " delimiter and sent to \e stderr .

@param	str	String to prepend to error message
@param	LRerrno	LibRan error number
@return	void - but outputs an error message to stderr
*/
void LRperror(char *str, int LRerrno) {
	if (str) {
		fprintf(stderr,"%s : %s\n", str, LRstrerror(LRerrno));
	} else {
		fprintf(stderr,"%s\n", LRstrerror(LRerrno));
	}
}

#ifdef __cplusplus
}
#endif
