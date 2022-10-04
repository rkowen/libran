###########################################################################
##
## File:        ./Makefile.am
## Versions:    $Id: Makefile.am 183 2005-08-19 05:39:40Z rk $
## Created:     2019/01/03
##
###########################################################################
##
## run  automake to generate a Makefile.in
##	Makefile.am \
##	configure.ac >  Makefile.in
##	aclocal.m4  / ##

AUTOMAKE_OPTIONS=foreign subdir-objects no-installinfo

#ACLOCAL_AMFLAGS=-I config

AM_LDFLAGS=-lm

if CUNIT
	MAYBE_TESTS=tests
endif
SUBDIRS = . urand $(MAYBE_TESTS)

#AM_CPPFLAGS = -I$(srcdir)/include -I$(srcdir)

LR_srcs	=								\
	LRnew.c LRset.c LRbin.c LRdf.c LRerror.c LRversion.c		\
	LRunif.c LRpiece.c LRlspline.c LRuinvcdf.c			\
	LRnexp.c LRerlang.c						\
	LRgaus.c LRgsn.c LRcauchy.c					\
	LRpoisson.c LRgeom.c LRbinom.c

URAND_srcs = 								\
	urand/urand.c

include_HEADERS =		\
	libran.h

lib_LIBRARIES		= libran.a

libran_a_SOURCES	=	\
	$(LR_srcs) 		\
	$(URAND_srcs)


if HAVE_DOXYGEN
@DX_RULES@

man3_MANS: doxygen-doc

docs: $(DX_CLEAN_PDF) man3_MANS

cleandocs: 
	-@rm -rf $(DX_CLEANFILES)

endif

#	docs/urand.man 

EXTRA_DIST=				\
		ChangeLog		\
		NEWS			\
		$(man3_MANS)		\
		include			\
		urand			\
		tests			\
		config			\
		docs

if CUNIT
test:
	cd tests; $(MAKE) test
endif

#dist-hook:
#	rm -rf `find $(distdir) -name CVS -print`
#	rm -rf `find $(distdir) -name Makefile -print`
#	rm -rf `find $(distdir) -name .deps -print`
#	rm -rf `find $(distdir) -name "*.x" -print`
#	rm -rf `find $(distdir) -name "*.o" -print`

#urand/urand.o : urand/urand.c urand/urand_.c clocker.o
#	cd urand; ./all.csh $(CC)

help:
	@echo ""
	@echo "make		- compiles sources to library"
	@echo "make all		- same as above"
if CUNIT
	@echo "make test	- make & run the test suite (if have CUnit)"
endif
if HAVE_DOXYGEN
	@echo "make docs	- make the docs with Doxygen"
	@echo "make cleandocs	- removes the generated docs"
endif
	@echo "make install	- copy library & man pages to destination"
	@echo "make clean	- cleans out most useless files"
	@echo "make dist-gzip	- makes a .tar.gz tarball"
	@echo "make distclean	- cleans & removes most made files"
	@echo ""
