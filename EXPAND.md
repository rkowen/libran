How To Expand LibRan
====================

This document will help the developer to add another random variate
distribution.  First you need to have developed an algorithm to
generate the random variate, the probability distribution function,
and the cumulative distribution function.  Once these are ready then
do the following (suppose we call the distribution \e xxx as an example):

libran.h
--------
* Add an \e enum to `LR_type` to identify this random variate distribution -
such as 'xxx' for example.

* Add the function declarations (assuming `double` and `float`):
  - LRd_xxx_RAN(LR_obj *o)	- random variate generator (double)
  - LRd_xxx_PDF(LR_obj *o, double x) - probability density function (double)
  - LRd_xxx_CDF(LR_obj *o, double x) - cumulative density function (double)
  - LRf_xxx_RAN(LR_obj *o)	- random variate generator (float)
  - LRf_xxx_PDF(LR_obj *o, float x) - probability density function (float)
  - LRf_xxx_CDF(LR_obj *o, float x) - cumulative density function (float)

LRnew.c
-------
* Add the `case xxx` block to 'LR_new()' and set the \e default `LR_obj`
attributes for the random variate distribution.
Look at the other ones for examples, but you'll need to
have code to set the attributes for either `double` and `float` or
`int` and `long`.

* Add the `case xxx` block to `LR_check()` and code to adjust "fixable"
attributes.  (Usually, for example,
setting \e s non-negative and raising an error if given as \e zero.)

LRxxx.c
-------
* Add the source code to git (git add LRxxx.c).

* The code should have internal documentation compliant with Doxygen
guidelines.

Makefile.am
-----------
* Add the source code filename to `LR_srcs` macro.

* Re-generate the autoconf files with "autoreconf -ivf"

* Reconfigure with "configure" and "make help" gives options.

tests/LRtest.c
--------------

* Add unit tests ... this is a must else the new random variate
distribution will not be accepted.  (Need to be familiar with CUnit.)

