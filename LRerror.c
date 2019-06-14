/*!
\file	LRerror.c
\brief	LibRan error codes and messages

The LibRan error routines patterned after the Standard C perror()/strerror()
routines and the like.

*/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "libran.h"

/*!
@brief	LRstrerror(int LRerrno)	- return some explanatory text regarding the given LR errno.

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
@brief	LRstrerrno(int LRerrno)	- return the errno code for the given LR errno.

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
@brief	LRperror(char *str, int LRerrno) - print out explanatory error message to stderr.

If the char string is not NULL then prepend the string to the LibRan error
message with a " : " delimiter and sent to stderr.

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
