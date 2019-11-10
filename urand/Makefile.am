bin_PROGRAMS	= turand
turand_SOURCES	= turand.c urand.c
turand_LDADD	= -lran
turand_LDFLAGS	= -L..
dist_doc_DATA	= README
AM_CPPFLAGS	= -I..

ChangeLog :
	@echo "Do Not Edit!  This is a generated file." > ChangeLog
	@echo "	You should edit the NEWS file instead." >> ChangeLog
	@echo "" >> ChangeLog
	@echo "LibRan/Urand" >> ChangeLog
	@echo "" >> ChangeLog
	git log --all --date=iso --name-status			\
		--pretty="format:%H%n  %cn%n  %cd%n  %s"	\
		>> ChangeLog
