###########################################################################
##
## File:        ./Makefile.am
## Versions:    $Id: Makefile.am 183 2005-08-19 05:39:40Z rk $
## Created:     2019/01/03
##
###########################################################################
##

AUTOMAKE_OPTIONS=foreign no-installinfo

bin_PROGRAMS			= LRtest

LRtest_SOURCES			= LRtest.c
LRtest_LDADD			= -lcunit -lran
LRtest_LDFLAGS			= -L..
AM_CPPFLAGS			= -I..
EXTRA_LRtest_DEPENDENCIES	= ../libran.a

test: run

run: LRtest$(EXEEXT)
	./LRtest$(EXEEXT)

help:
	@echo ""
	@echo "make		- compiles tests for library"
	@echo "make all		- same as above"
	@echo "make run		- run the tests for library"
	@echo "make test	- same as above"
	@echo "make clean	- cleans out most useless files"
	@echo "make distclean	- cleans & removes most made files"
	@echo ""
