#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([LibRan], [0.1], [rk@owen.sj.ca.us])
AM_INIT_AUTOMAKE(foreign -Wportability)
#
# Doxygen
AC_CHECK_PROGS([DOXYGEN], [doxygen])
AC_CHECK_PROGS([DOT], [dot])
AM_CONDITIONAL([DOXYGEN],[test -z "$DOXYGEN"])
AM_CONDITIONAL([DOT],[test -z "$DOT"])
if test -z "$DOXYGEN"; then
	AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
else
	DX_INIT_DOXYGEN([LibRan],[.],[docs])
	DX_HTML_FEATURE(ON)
	DX_CHM_FEATURE(OFF)
	DX_CHI_FEATURE(OFF)
	DX_MAN_FEATURE(ON)
	DX_RTF_FEATURE(OFF)
	DX_XML_FEATURE(OFF)
	DX_PDF_FEATURE(ON)
	DX_PS_FEATURE(OFF)
fi
if test -n "$DOXYGEN"; then
if test -n "$DOT"; then
	:
	DX_DOT_FEATURE(ON)
else
	AC_MSG_WARN([Dot not found - continuing without Doxygen dot graphics support])
	DX_DOT_FEATURE(OFF)
fi
fi

AC_CONFIG_SRCDIR([LRnew.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	tests/Makefile
])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
dnl ---------------------------------------------------------------------------
dnl Autoheader "templates" (was acconfig.h)
AH_TOP([
/*****
 ******************************************************************************
 LibRan - 
	LibRan is a library of pseudo-random number generators and the
	probability distribution functions (PDFs) and culumative distribution
	functions that represent them.

 Copyright 2019 by R.K. Owen, Ph.D.
 See LICENSE.LGPL, which must be provided, for details.

 ******************************************************************************
 *****/
])
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_SUBDIRS([urand])
AC_OUTPUT
