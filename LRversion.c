/*!
\file	LRversion.c
\brief	LibRan version info

Return the LibRan version info as a string.

The allowed set of input parameters are:

Parameter  | Description
----------- --------------------------
version    | Full string (X.Y.Z)
major      | Major level (X)
minor      | Minor level (Y)
patch      | Patch number (Z)
date       | Version date (YYYYMMDD)

*/
/*
 * Copyright	2019	R.K. Owen, Ph.D.
 * License	see lgpl.md (Gnu Lesser General Public License)
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "libran.h"

/*!
@brief	LRversion(char *str) - return the given version info

See the above table of allowed parameters.  Case is not important.
Anything else will return a NULL string.

@param	str	What info to return
@return	char *	Pointer to an internal string buffer.
*/

#define STRINGIFY(arg)		#arg
#define STRVAL(arg)		STRINGIFY(arg)

char *LRversion(char *str) {
	if (!strncasecmp("version",str,4)) {
		return STRVAL(LR_VERSION);
	} else if (!strncasecmp("major",str,5)) {
		return STRVAL(LR_VER_MAJOR);
	} else if (!strncasecmp("minor",str,5)) {
		return STRVAL(LR_VER_MINOR);
	} else if (!strncasecmp("patch",str,5)) {
		return STRVAL(LR_VER_PATCH);
	} else if (!strncasecmp("date",str,4)) {
		return STRVAL(LR_DATE);
	} else {
		return "";
	}
}

#ifdef __cplusplus
}
#endif

