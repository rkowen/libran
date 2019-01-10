#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([LibRan], [0.1], [rk@owen.sj.ca.us])
AM_INIT_AUTOMAKE(foreign -Wportability)
AC_CONFIG_SRCDIR([LRnew.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	urand/Makefile
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

AC_CONFIG_SUBDIRS([urand tests])
AC_OUTPUT
