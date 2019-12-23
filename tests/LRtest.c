#include <stdio.h>
#include <math.h>
#include "libran.h"
#include "urand/urand.h"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>

#define max(a,b) (a > b ? a : b)

#define LR_DEBUG

#ifdef LR_DEBUG
#  define CX_ASSERT_DOUBLE_EQUAL(val,exp,tol)			\
{	printf("value, expect = % 9.5f,% 9.5f\n",val,exp);	\
	CU_ASSERT_DOUBLE_EQUAL(val,exp,tol); }
#  define CX_ASSERT_EQUAL(val, exp);				\
{	printf("value, expect = % ld,% ld\n",(long) val,(long) exp);	\
	CU_ASSERT_EQUAL(val,exp); }
#else
#  define CX_ASSERT_DOUBLE_EQUAL(val,exp,tol)			\
	CU_ASSERT_DOUBLE_EQUAL(val,exp,tol);
#  define CX_ASSERT_EQUAL(val, exp);				\
	CU_ASSERT_EQUAL(val,exp);
#endif

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/* LibRan error routines */
void test_LR_errors(void) {
	FILE *oldstderr = stderr, *newstderr;
	char strbuf[200], *ptr;
	struct errset {
		int err;
		char *msg;
	} es[] = {
0x00,	"LRerr_OK : LibRan - No Error\n",
0x01,	"LRerr_Unspecified : LibRan - Unspecified Error\n",
0x03,	"LRerr_BadDataType : LibRan - Bad Data Type given\n",
0x05,	"LRerr_BadLRType : LibRan - Bad Random Variate Type given\n",
0x09,	"LRerr_NoAuxiliaryObject : LibRan - Required Auxiliary Object not found\n",
0x0B,	"LRerr_NoAuxNormalizeDone : LibRan - Auxiliary Object requires normalization\n",
0x0D,	"LRerr_BadAuxSetup : LibRan - Auxiliary Object incorrectly set-up\n",
0x11,	"LRerr_BinGeneric : LibRan - Binning Object Unspecified Error\n",
0x13,	"LRerr_TooManyValues : LibRan - Too Many Values given\n",
0x15,	"LRerr_InvalidInputValue : LibRan - Invalid Input Value Error\n",
0x17,	"LRerr_InvalidRange : LibRan - Invalid Range Value Error\n",
0x19,	"LRerr_UnmetPreconditions : LibRan - Preconditions Not Performed\n",
0x1B,	"LRerr_SuspiciousValues : LibRan - Suspicious Value - Normalization Error?\n",
0x1D,	"LRerr_AllocFail : LibRan - Memory Allocation Error\n"};

/* capture stderr to file */
	newstderr = stderr = tmpfile();

	for (int i = 0; i < sizeof(es)/sizeof(es[0]); i++) {
		stderr = newstderr;
		(void) fseek(stderr,0,SEEK_SET);
		LRperror(LRstrerrno(es[i].err),es[i].err);
		(void) fseek(stderr,0,SEEK_SET);
		ptr = fgets(strbuf, sizeof(strbuf), stderr);
		stderr = oldstderr;
/*
fprintf(stderr, "<<< %x,%d\n:%s:%s",es[i].err,strcmp(ptr,es[i].msg), ptr, es[i].msg);
*/
		CU_ASSERT_STRING_EQUAL(ptr, es[i].msg);
		(void) sscanf(es[i].msg,"%s : ",strbuf);
		CU_ASSERT_STRING_EQUAL(LRstrerrno(es[i].err), strbuf);
/*
fprintf(stderr, ">>> %x,%d\n:%s:%s\n",es[i].err,
		strcmp(LRstrerrno(es[i].err), strbuf),
		LRstrerrno(es[i].err), strbuf);
*/
	}
/* restore stderr */
	stderr = oldstderr;
}

/* LibRan version */
#define STRINGIFY(arg)		#arg
#define STRVAL(arg)		STRINGIFY(arg)
void test_LR_version(void) {
	CU_ASSERT_STRING_EQUAL(LRversion("vers"), STRVAL(LR_VERSION));
	CU_ASSERT_STRING_EQUAL(LRversion("version"), STRVAL(LR_VERSION));
	CU_ASSERT_STRING_EQUAL(LRversion("major"), STRVAL(LR_VER_MAJOR));
	CU_ASSERT_STRING_EQUAL(LRversion("MAJOR"), STRVAL(LR_VER_MAJOR));
	CU_ASSERT_STRING_EQUAL(LRversion("minor"), STRVAL(LR_VER_MINOR));
	CU_ASSERT_STRING_EQUAL(LRversion("mInOr"), STRVAL(LR_VER_MINOR));
	CU_ASSERT_STRING_EQUAL(LRversion("patch"), STRVAL(LR_VER_PATCH));
	CU_ASSERT_STRING_EQUAL(LRversion("PaTcH"), STRVAL(LR_VER_PATCH));
	CU_ASSERT_STRING_EQUAL(LRversion("date"), STRVAL(LR_DATE));
	CU_ASSERT_STRING_EQUAL(LRversion("Date"), STRVAL(LR_DATE));
/* make sure major.minor.patch are right */
	CU_ASSERT_STRING_EQUAL(
		LRversion("vers"),
		STRVAL(LR_VER_MAJOR) "."
		STRVAL(LR_VER_MINOR) "."
		STRVAL(LR_VER_PATCH));
	CU_ASSERT_STRING_EQUAL(LRversion("foobar"), "");
}

#define testLRnew(tt)			void test_new_##tt(void) {	\
	LR_obj *o = LR_new(gausbm, LR_##tt);				\
	CU_ASSERT_PTR_NOT_NULL(o);					\
	CU_ASSERT_EQUAL(o->d, LR_##tt);					\
	CU_ASSERT_PTR_EQUAL(o->ui, LR_irand);				\
	CU_ASSERT_PTR_EQUAL(o->ul, LR_lrand);				\
	CU_ASSERT_PTR_EQUAL(o->uf, LR_frand);				\
	CU_ASSERT_PTR_EQUAL(o->ud, LR_drand);				\
	CU_ASSERT_EQUAL(sizeof (*o), sizeof (LR_obj));			\
	LR_rm(&o);							\
	CU_ASSERT_PTR_NULL(o);						\
}

testLRnew(int)
testLRnew(float)
testLRnew(long)
testLRnew(double)

/* test when individual parameters are set */
#define testLRset(tt, ptt, att)		void test_set_##tt(void) {	\
	LR_obj *o = LR_new(gausbm, LR_##tt);				\
	CU_ASSERT_EQUAL(LR_set(o, 'a', -1##ptt),0);			\
	CU_ASSERT_EQUAL(LR_set(o, 'b',  3##ptt),0);			\
	CU_ASSERT_EQUAL(LR_set(o, 'm',  2##ptt),0);			\
	CU_ASSERT_EQUAL(LR_set(o, 's',  1##ptt),0);			\
	CU_ASSERT_EQUAL(LR_set(o, 'x',  5##ptt),0);			\
	CU_ASSERT_EQUAL(LR_set(o, 'k',  7),0);				\
	CU_ASSERT_EQUAL(LR_set(o, 'n',  11),0);				\
	CU_ASSERT_NOT_EQUAL(LR_set(o, 'z',  9##ptt),0);			\
	CU_ASSERT_EQUAL(o->a.att, -1##ptt);				\
	CU_ASSERT_EQUAL(o->b.att,  3##ptt);				\
	CU_ASSERT_EQUAL(o->m.att,  2##ptt);				\
	CU_ASSERT_EQUAL(o->s.att,  1##ptt);				\
	CU_ASSERT_EQUAL(o->x.att,  5##ptt);				\
	CU_ASSERT_EQUAL(o->k,  7);					\
	CU_ASSERT_EQUAL(o->n,  11);					\
	LR_rm(&o);							\
}

testLRset(int,,i)
testLRset(float,.,f)
testLRset(long,L,l)
testLRset(double,.,d)

/* test when all the parameters are given */
#define testLRsetall(tt, ptt, att)	void test_set_all_##tt(void) {	\
	LR_obj *o = LR_new(gausbm, LR_##tt);				\
	CU_ASSERT_EQUAL(						\
		LR_set_all(o,"abmsxknpq",				\
		-1##ptt,3##ptt,2##ptt,1##ptt,5##ptt,7,11,4.,16.),0);	\
	CU_ASSERT_EQUAL(o->a.att, -1##ptt);				\
	CU_ASSERT_EQUAL(o->b.att,  3##ptt);				\
	CU_ASSERT_EQUAL(o->m.att,  2##ptt);				\
	CU_ASSERT_EQUAL(o->s.att,  1##ptt);				\
	CU_ASSERT_EQUAL(o->x.att,  5##ptt);				\
	CU_ASSERT_EQUAL(o->k,  7);					\
	CU_ASSERT_EQUAL(o->n,  11);					\
	CU_ASSERT_EQUAL(o->p,  4.);					\
	CU_ASSERT_EQUAL(o->q,  16.);					\
	LR_rm(&o);							\
}

testLRsetall(int,,i)
testLRsetall(float,.,f)
testLRsetall(long,L,l)
testLRsetall(double,.,d)

/* test a different order of parameters */
#define testLRsetall2(tt, ptt, att)	void test_set_all2_##tt(void) {	\
	LR_obj *o = LR_new(gausbm, LR_##tt);				\
	CU_ASSERT_EQUAL(						\
	  LR_set_all(o,"mpnaxksqb",					\
		2##ptt,4.,11,-1##ptt,5##ptt,7,1##ptt,16.,3##ptt),0);	\
	CU_ASSERT_EQUAL(o->a.att, -1##ptt);				\
	CU_ASSERT_EQUAL(o->b.att,  3##ptt);				\
	CU_ASSERT_EQUAL(o->m.att,  2##ptt);				\
	CU_ASSERT_EQUAL(o->s.att,  1##ptt);				\
	CU_ASSERT_EQUAL(o->x.att,  5##ptt);				\
	CU_ASSERT_EQUAL(o->k,  7);					\
	CU_ASSERT_EQUAL(o->n,  11);					\
	CU_ASSERT_EQUAL(o->p,  4.);					\
	CU_ASSERT_EQUAL(o->q,  16.);					\
	LR_rm(&o);							\
}

testLRsetall2(int,,i)
testLRsetall2(float,.,f)
testLRsetall2(long,L,l)
testLRsetall2(double,.,d)

/* test when extra parameters are given */
#define testLRsetall3(tt, ptt, att)	void test_set_all3_##tt(void) {	\
	LR_obj *o = LR_new(gausbm, LR_##tt);				\
	CU_ASSERT_NOT_EQUAL(						\
	  LR_set_all(o,"mpnaxzksqby",					\
  2##ptt,4.,11,-1##ptt,5##ptt,33##ptt,7,1##ptt,16.,3##ptt,9##ptt),0);	\
	CU_ASSERT_EQUAL(o->a.att, -1##ptt);				\
	CU_ASSERT_EQUAL(o->b.att,  3##ptt);				\
	CU_ASSERT_EQUAL(o->m.att,  2##ptt);				\
	CU_ASSERT_EQUAL(o->s.att,  1##ptt);				\
	CU_ASSERT_EQUAL(o->x.att,  5##ptt);				\
	CU_ASSERT_EQUAL(o->k,  7);					\
	CU_ASSERT_EQUAL(o->n,  11);					\
	CU_ASSERT_EQUAL(o->p,  4.);					\
	CU_ASSERT_EQUAL(o->q,  16.);					\
	LR_rm(&o);							\
}

testLRsetall3(int,,i)
testLRsetall3(float,.,f)
testLRsetall3(long,L,l)
testLRsetall3(double,.,d)

/* test the check method */
#define testLRcheck(nn, dist, tt, ttt, setup)				\
void test_check_ ## dist ## _##tt ## _##nn(void) {			\
	LR_obj *o = LR_new(dist, LR_ ##ttt);				\
	setup;								\
	LR_rm(&o);							\
}

void test_check_bad_0(void) {
	LR_obj *o = NULL;
	CU_ASSERT_EQUAL(LR_check(o), LRerr_Unspecified);
}

testLRcheck(0, unif, d, double, 
	LR_set_all(o, "ab", 2.0,-1.5);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_OK);
	CU_ASSERT_EQUAL(o->a.d, -1.5);
	CU_ASSERT_EQUAL(o->b.d,  2.0);
)
testLRcheck(1, unif, d, double, 
	LR_set_all(o, "ab", 2.0, 2.0);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_InvalidRange);
)
testLRcheck(2, gausbm, d, double, 
	LR_set_all(o, "sm",-2.0, 1.5);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_OK);
	CU_ASSERT_EQUAL(o->m.d,  1.5);
	CU_ASSERT_EQUAL(o->s.d,  2.0);
)
testLRcheck(3, gausbm, d, double, 
	LR_set_all(o, "sm", 0., 1.5);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_InvalidInputValue);
)
testLRcheck(4, uinvcdf, d, double, 
	CU_ASSERT_EQUAL(LR_check(o), LRerr_UnmetPreconditions);
)
testLRcheck(5, uinvcdf, d, double, 
	LR_set_all(o, "ab", 2.0,-1.0);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_OK);
)
testLRcheck(6, uinvcdf, d, double, 
	LR_set_all(o, "ams", 2.0,3.0,-1.0);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_OK);
)
testLRcheck(7, uinvcdf, d, double, 
	LR_set_all(o, "ams", 2.0,3.0,0.);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_InvalidInputValue);
)
testLRcheck(8, uinvcdf, d, double, 
	LR_set_all(o, "ab", 2.0,2.0);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_InvalidRange);
)
/* uinvcdf d 9 */

testLRcheck(0, unif, f, float, 
	LR_set_all(o, "ab", 2.0,-1.5);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_OK);
	CU_ASSERT_EQUAL(o->a.f, -1.5);
	CU_ASSERT_EQUAL(o->b.f,  2.0);
)
testLRcheck(1, unif, f, float, 
	LR_set_all(o, "ab", 2.0, 2.0);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_InvalidRange);
)
testLRcheck(2, gausbm, f, float, 
	LR_set_all(o, "sm",-2.0, 1.5);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_OK);
	CU_ASSERT_EQUAL(o->m.f,  1.5);
	CU_ASSERT_EQUAL(o->s.f,  2.0);
)
testLRcheck(3, gausbm, f, float, 
	LR_set_all(o, "sm", 0., 1.5);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_InvalidInputValue);
)
testLRcheck(4, uinvcdf, f, float, 
	CU_ASSERT_EQUAL(LR_check(o), LRerr_UnmetPreconditions);
)
testLRcheck(5, uinvcdf, f, float, 
	LR_set_all(o, "ab", 2.0,-1.0);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_OK);
)
testLRcheck(6, uinvcdf, f, float, 
	LR_set_all(o, "ams", 2.0,3.0,-1.0);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_OK);
)
testLRcheck(7, uinvcdf, f, float, 
	LR_set_all(o, "ams", 2.0,3.0,0.);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_InvalidInputValue);
)
testLRcheck(8, uinvcdf, f, float, 
	LR_set_all(o, "ab", 2.0,2.0);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_InvalidRange);
)

/* independent pseudo-random sequences */
#define testLRindep(dist, nn, num, incr, setup)				\
void test_indep_seq_##nn(void) {					\
	LR_obj *o1 = LR_new(dist, LR_double);				\
	LR_obj *o2 = LR_new(dist, LR_double);				\
	double y1, y2;							\
	setup;								\
	for (int i = 0; i < num; i++) {					\
		for (int j = 0; j < incr; j++) {			\
			y1 = LRd_RAN(o1); y2 = LRd_RAN(o2);		\
		}							\
		CU_ASSERT_DOUBLE_EQUAL(y1, y2,.0000001);		\
	}								\
	LR_rm(&o1); LR_rm(&o2);						\
}

testLRindep(unif, 1, 10, 5000,)
testLRindep(unif, 2, 10, 5000,
	LR_lsetseed(o1, 19580512l);
	LR_lsetseed(o2, 19580512l);
)
testLRindep(gausbm, 3, 10, 5000,)
testLRindep(gausbm, 4, 10, 5000,
	LR_lsetseed(o1, 19580512l);
	LR_lsetseed(o2, 19580512l);
)
testLRindep(gsn2, 5, 10, 5000,)
testLRindep(gsn2, 6, 10, 5000,
	LR_lsetseed(o1, 19580512l);
	LR_lsetseed(o2, 19580512l);
)

/* binning object */
#define testLRbinnew(tt)		void test_bin_new_##tt(void) {	\
	LR_bin *b = LR_bin_new(10);					\
	CU_ASSERT_PTR_NOT_NULL(b);					\
	CU_ASSERT_EQUAL(b->n, 10);					\
	CU_ASSERT_EQUAL(b->nn, 1);					\
	CU_ASSERT_EQUAL(sizeof(b), sizeof(LR_bin *));			\
	CU_ASSERT_EQUAL(sizeof(*b), sizeof(LR_bin));			\
	CU_ASSERT_EQUAL(sizeof(b->bdrs[0]), sizeof(double));		\
	CU_ASSERT_EQUAL(sizeof(b->bins[0]), sizeof(long));		\
	LR_bin_rm(&b);							\
	CU_ASSERT_PTR_NULL(b);						\
}

/*
testLRbinnew(int)
testLRbinnew(long)
testLRbinnew(float)
*/
testLRbinnew(double)

#define testLRbinset(nn,aa,bb,cc,dd)	void test_bin_set_##nn(void) {	\
	LR_bin *b = LR_bin_new(5);					\
	CU_ASSERT_EQUAL(LR_bin_set(b, aa),0);				\
	CU_ASSERT_EQUAL(LR_bin_set(b, bb),0);				\
	CU_ASSERT_EQUAL(LR_bin_set(b, cc),0);				\
	CU_ASSERT_EQUAL(LR_bin_set(b, dd),0);				\
	CU_ASSERT_NOT_EQUAL(LR_bin_set(b, aa+bb),0);			\
	CU_ASSERT_DOUBLE_EQUAL(b->bdrs[0],-1.0,.0001);			\
	CU_ASSERT_DOUBLE_EQUAL(b->bdrs[1], 0.0,.0001);			\
	CU_ASSERT_DOUBLE_EQUAL(b->bdrs[2], 3.3,.0001);			\
	CU_ASSERT_DOUBLE_EQUAL(b->bdrs[3], 4.5,.0001);			\
	CU_ASSERT_DOUBLE_EQUAL(b->bdrs[4], 0.0,.0001);			\
	LR_bin_rm(&b);							\
}

testLRbinset(1,-1.0,0.0,3.3,4.5)
testLRbinset(2,4.5,3.3,0.0,-1.0)
testLRbinset(3,-1.0,4.5,3.3,0.0)
testLRbinset(4,3.3,-1.0,4.5,0.0)

#define testLRbinadd(nn,aa)		void test_bin_add_##nn(void) {	\
	LR_bin *b = LR_bin_new(aa+2);					\
	for (int i = 1; i < aa; i++) {					\
		CU_ASSERT_EQUAL(LR_bin_set(b, i/(double) aa),0);	\
	}								\
	for (int i = 0; i < 10000; i++) {				\
		CU_ASSERT_EQUAL(LR_bin_add(b, 0.0001*i),0);		\
	}								\
	for (int i = 0; i < aa; i++) {					\
		CU_ASSERT_DOUBLE_EQUAL(b->bins[i],10000./(double) aa, 1.);\
	}								\
	LR_bin_rm(&b);							\
}

testLRbinadd(1,10)
testLRbinadd(2,30)
testLRbinadd(3,50)
testLRbinadd(4,100)

/* distributions */
#define compCdfPdf(tt,oo,xx,xxh,tol)				\
CU_ASSERT_DOUBLE_EQUAL(							\
	((LR##tt ## _CDF(oo,xxh))-(LR##tt ## _CDF(oo,xx)))/((xxh)-(xx)),\
	LR##tt ## _PDF(oo,xx),tol)

/* testCdfPdf
 * nn	- test number
 * tt	- LR data type (d or f)
 * ttt	- LR data type (double or float)
 * dist	- LR variate type
 * nnt	- number of test values
 * del	- delta
 * tol	- absolute tolerance
 * setup- LR object parameter set-up
 */
#define testCdfPdf(nn,tt,ttt,dist,nnt,del,tol,setup)			\
void test_cdf_pdf_ ## tt ## _ ##dist ## _##nn(void) {			\
	LR_obj *o = LR_new(dist, LR_##ttt);				\
	double	incr;							\
	setup;								\
	incr = (o->b.tt - o->a.tt)/nnt;					\
	CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt-2*incr),0.,tol)	\
	CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt-2*incr),0.,tol)	\
	for (int i = 1; i < nnt; i++) {					\
	  compCdfPdf(tt,o,o->a.tt+i*incr,o->a.tt+i*incr+del,tol)	\
	}								\
	CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->b.tt+2*incr),1.,tol) \
	CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->b.tt+2*incr),0.,tol) \
}

/* testSymDF - test symetric values
 * nn	- test number
 * tt	- LR data type (d or f)
 * ttt	- LR data type (double or float)
 * dist	- LR variate type
 * x1	- first value
 * x2	- second symmetric value
 * tol	- absolute tolerance
 * setup- LR object parameter set-up
 */
#define testSymDF(nn,tt,ttt,dist,x1,x2,tol,setup)			\
void test_sym_ ## tt ## _ ##dist ## _##nn(void) {			\
	LR_obj *o = LR_new(dist, LR_##ttt);				\
	setup;								\
	ttt c1,p1,c2,p2, one = 1;					\
	c1 = LR##tt ## _CDF(o,x1);					\
	p1 = LR##tt ## _PDF(o,x1);					\
	c2 = LR##tt ## _CDF(o,x2);					\
	p2 = LR##tt ## _PDF(o,x2);					\
	CU_ASSERT_DOUBLE_EQUAL(p1,p2,tol)				\
	CU_ASSERT_DOUBLE_EQUAL(c1+c2,one,tol)				\
}

/* test some well defined points */
#define testCdfPdf0unif(nn,tt,ttt,tol,setup)				\
void test_cdf_pdf_##tt ## _unif ## _##nn(void) {			\
	LR_obj *o = LR_new(unif, LR_##ttt);				\
	double	ival,pval;						\
	setup;								\
	ival = (o->b.tt - o->a.tt);					\
	pval = 1.0/ival;						\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt-.1),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt-.1),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt),pval,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt+.25*ival),.25,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt+.25*ival),pval,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt+.5*ival),.5,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt+.5*ival),pval,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt+.75*ival),.75,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt+.75*ival),pval,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->b.tt),1.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->b.tt),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->b.tt+.1),1.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->b.tt+.1),0.,tol)		\
}

/* random variate generation & binning */
/* the last bin will have one less then rest ... so may be off by 2. */
#define testLRunif(nn,tt,ttt,dist,bn,setup)				\
void test_##dist ## _##tt ## _##nn(void) {\
	LR_obj *o = LR_new(dist, LR_##ttt);				\
	LR_bin *b = LR_bin_new(bn);					\
	double	x, cdf[bn], tot = 10007.;				\
	setup;								\
	double	incr = (o->b.tt - o->a.tt) / bn;			\
	o->uf = tfrand; o->ud = tdrand;					\
	for (int i = 1; i < bn; i++) {					\
		LR_bin_set(b,o->a.tt + i*incr);				\
		cdf[i-1] = tot*(LR##tt ## _CDF(o,o->a.tt+i*incr)	\
			- LR##tt ## _CDF(o,o->a.tt+(i-1)*incr));	\
	}								\
	cdf[bn-1] = tot*(LR##tt ## _CDF(o,o->b.tt)			\
			- LR##tt ## _CDF(o,o->b.tt - incr));		\
	for (int i = 0; i < tot; i++) {					\
		LR_bin_add(b,LR##tt ## _RAN(o));			\
	}								\
	for (int i = 0; i < bn; i++) {					\
		CU_ASSERT_DOUBLE_EQUAL(b->bins[i],cdf[i],2.);		\
	}								\
}

/* more complicated random variates do not have "uniform" coverage */
/* make tolerance adjustable */
/* vty - LR distribution
 * nn  - test #
 * tt  - type letter (f or d)
 * ttt - type (float or double)
 * bn  - bin number
 * vsc - % error tolerance (.1 = 10%)
 * vab - absolute error tolerance
 * setup - set-up code
 */
#define testLRvar(vty,nn,tt,ttt,bn,vsc,vab,setup)			\
void test_##vty ## _##tt ## _##nn(void) {				\
	LR_obj *o = LR_new(vty, LR_##ttt);				\
	LR_bin *b = LR_bin_new(bn);					\
	double	x, cdf[bn], tol, tot = 10007.;				\
	setup;								\
	double	incr = (o->b.tt - o->a.tt) / bn;			\
	o->uf = tfrand; o->ud = tdrand;					\
	for (int i = 1; i < bn; i++) {					\
		LR_bin_set(b,o->a.tt + i*incr);				\
		cdf[i-1] = tot*(LR##tt ## _CDF(o,o->a.tt+i*incr)	\
			- LR##tt ## _CDF(o,o->a.tt+(i-1)*incr));	\
	}								\
	cdf[bn-1] = tot*(LR##tt ## _CDF(o,o->b.tt)			\
			- LR##tt ## _CDF(o,o->b.tt - incr));		\
	for (int i = 0; i < tot; i++) {					\
		LR_bin_add(b,LR##tt ## _RAN(o));			\
	}								\
	for (int i = 0; i < bn; i++) {					\
		tol = max(cdf[i]*vsc,vab);				\
		CU_ASSERT_DOUBLE_EQUAL(b->bins[i],cdf[i],tol);		\
	}								\
}

/* even more complicated random variates do not have "uniform" coverage */
/* also the limited test t*rand functions do not adequately span space */
/* make tolerance adjustable */
/* vty - LR distribution
 * nn  - test #
 * tt  - type letter (f or d)
 * ttt - type (float or double)
 * bn  - bin number
 * xtot	- total number of samples (should be large)
 * vsc - % error tolerance (.1 = 10%)
 * vab - absolute error tolerance
 * setup - set-up code
 */
#define testLRvarx(vty,nn,tt,ttt,bn,xtot,vsc,vab,setup)			\
void test_##vty ## _##tt ## _##nn(void) {				\
	LR_obj *o = LR_new(vty, LR_##ttt);				\
	LR_bin *b = LR_bin_new(bn);					\
	double	x, cdf[bn], tol;					\
	setup;								\
	double	incr = (o->b.tt - o->a.tt) / bn;			\
	/* o->uf = tfrand; o->ud = tdrand; */				\
	for (int i = 1; i < bn; i++) {					\
		LR_bin_set(b,o->a.tt + i*incr);				\
		cdf[i-1] = xtot*(LR##tt ## _CDF(o,o->a.tt+i*incr)	\
			- LR##tt ## _CDF(o,o->a.tt+(i-1)*incr));	\
	}								\
	cdf[bn-1] = xtot*(LR##tt ## _CDF(o,o->b.tt)			\
			- LR##tt ## _CDF(o,o->b.tt - incr));		\
	for (int i = 0; i < xtot; i++) {				\
		LR_bin_add(b,LR##tt ## _RAN(o));			\
	}								\
	for (int i = 0; i < bn; i++) {					\
		tol = max(cdf[i]*vsc,vab);				\
		CU_ASSERT_DOUBLE_EQUAL(b->bins[i],cdf[i],tol);		\
	}								\
}

/* more complicated random variates do not have "uniform" coverage */
/* also the limited test t*rand functions do not adequately span space */
/* make tolerance adjustable */
/* vty	- LR distribution
 * nn	- test #
 * tt	- type letter (f or d)
 * ttt	- type (float or double)
 * end	- the highest value of range
 * bn	- bin number
 * xtot	- total number of samples (should be large)
 * vsc	- % error tolerance (.1 = 10%)
 * vab	- absolute error tolerance
 * setup- set-up code
 */
#define testLRhalf(vty,nn,tt,ttt,end,bn,xtot,vsc,vab,setup)		\
void test_##vty ## _##tt ## _##nn(void) {				\
	LR_obj *o = LR_new(vty, LR_##ttt);				\
	LR_bin *b = LR_bin_new(bn+2);					\
	double	x, cdf[bn+2], tol;					\
	setup;								\
	double	begin = 0.0,						\
		incr = end / bn;					\
	/* o->uf = tfrand; o->ud = tdrand; */				\
	for (int i = 0; i < bn; i++) {					\
		x = begin + i*incr;					\
		LR_bin_set(b,x);					\
		cdf[i] = xtot*(LR##tt ## _CDF(o,x)			\
			- LR##tt ## _CDF(o,x - incr));			\
	}								\
	cdf[bn] = xtot * (1.0 - LR##tt ## _CDF(o,begin+bn*incr));	\
	for (int i = 0; i < xtot; i++) {				\
		LR_bin_add(b,LR##tt ## _RAN(o));			\
	}								\
	for (int i = 0; i <= bn; i++) {					\
		tol = max(cdf[i]*vsc,vab);				\
		CU_ASSERT_DOUBLE_EQUAL(1.0*b->bins[i],cdf[i],tol);	\
	}								\
}

/* more complicated random variates do not have "uniform" coverage */
/* also the limited test t*rand functions do not adequately span space */
/* make tolerance adjustable */
/* vty	- LR distribution
 * nn	- test #
 * tt	- type letter (f or d)
 * ttt	- type (float or double)
 * ww	- number of std.dev on either side of mean.
 * bn	- bin number
 * xtot	- total number of samples (should be large)
 * vsc	- % error tolerance (.1 = 10%)
 * vab	- absolute error tolerance
 * setup- set-up code
 */
#define testLRfull(vty,nn,tt,ttt,ww,bn,xtot,vsc,vab,setup)		\
void test_##vty ## _##tt ## _##nn(void) {				\
	LR_obj *o = LR_new(vty, LR_##ttt);				\
	LR_bin *b = LR_bin_new(bn+2);					\
	double	x, cdf[bn+2], tol;					\
	setup;								\
	double	begin = o->m.tt - ww*o->s.tt,				\
		incr = 2.0*ww*o->s.tt / bn;				\
	/* o->uf = tfrand; o->ud = tdrand; */				\
	cdf[0] = xtot * LR##tt ## _CDF(o,begin);			\
	LR_bin_set(b,begin);						\
	for (int i = 1; i <= bn; i++) {					\
		x = begin + i*incr;					\
		LR_bin_set(b,x);					\
		cdf[i] = xtot*(LR##tt ## _CDF(o,x)			\
			- LR##tt ## _CDF(o,x - incr));			\
	}								\
	cdf[bn+1] = xtot * (1.0 - LR##tt ## _CDF(o,begin+bn*incr));	\
	for (int i = 0; i < xtot; i++) {				\
		LR_bin_add(b,LR##tt ## _RAN(o));			\
	}								\
	for (int i = 0; i <= bn+1; i++) {				\
		tol = max(cdf[i]*vsc,vab);				\
		CU_ASSERT_DOUBLE_EQUAL(1.0*b->bins[i],cdf[i],tol);	\
	}								\
}

/* test bad data type errors */
/* vty	- LR distribution
 * nn	- test #
 * tt	- type letter (f or d)
 * ttt	- type (float or double)
 * btt	- bad type letter (d or f)
 * xx	- value
 * setup- set-up code
 */
#define testBADdt(vty,nn,tt,ttt, btt, xx, setup)			\
void test_bad_##tt ## _dt_ ##nn(void) {					\
	LR_obj *o = LR_new(vty, LR_##ttt);				\
	setup;								\
	CU_ASSERT(isnan(LR##btt ## _RAN(o)));				\
	CU_ASSERT(isnan(LR##btt ## _PDF(o,xx)));			\
	CU_ASSERT(isnan(LR##btt ## _CDF(o,xx)));			\
	CU_ASSERT_EQUAL(LR_aux_new(o,9),LRerr_NoAuxiliaryObject);	\
	CU_ASSERT_EQUAL(LR_aux_rm(o),LRerr_NoAuxiliaryObject);		\
	CU_ASSERT_EQUAL(LR_aux_set(o,1.,1.),LRerr_NoAuxiliaryObject);	\
	CU_ASSERT_EQUAL(LR_aux_norm(o),LRerr_NoAuxiliaryObject);	\
}

/* uniform distribution */
int tirand(LR_obj *);
long tlrand(LR_obj *);
float tfrand(LR_obj *);
double tdrand(LR_obj *);

testCdfPdf0unif(0,d,double,.001,)
testCdfPdf(1,d,double,unif,20,.0001,.001,)
testCdfPdf(2,d,double,unif,50,.0001,.001,)
testCdfPdf(3,d,double,unif,90,.0001,.001,)

testCdfPdf0unif(4,d,double,.001,LR_set_all(o,"ab",-2.,2.))
testCdfPdf(5,d,double,unif,33,.0001,.001,LR_set_all(o,"ab",-1.,3.))

testCdfPdf0unif(0,f,float,.001,)
testCdfPdf(1,f,float,unif,20,.0001,.001,)
testCdfPdf(2,f,float,unif,50,.0001,.001,)
testCdfPdf(3,f,float,unif,90,.0001,.001,)

testCdfPdf0unif(4,f,float,.001,LR_set_all(o,"ab",-2.,2.))
testCdfPdf(5,f,float,unif,33,.0001,.001,LR_set_all(o,"ab",-1.,3.))

testLRunif(1,d,double,unif,10,)
testLRunif(2,d,double,unif,25,)
testLRunif(3,d,double,unif,20,LR_set_all(o,"ab",-2.,2.))
testLRunif(4,d,double,unif,60,LR_set_all(o,"ab",-5.,1.))

testLRunif(1,f,float,unif,10,)
testLRunif(2,f,float,unif,25,)
testLRunif(3,f,float,unif,20,LR_set_all(o,"ab",-2.,2.))
testLRunif(4,f,float,unif,60,LR_set_all(o,"ab",-5.,1.))

/* piece */
/* test some well defined points */

#define testCdfPdf0piece(nn,tt,ttt,tol,lo,hi)				\
void test_cdf_pdf_##tt ## _piece ## _##nn(void) {			\
	LR_obj *o = LR_new(piece, LR_##ttt);				\
	LR_aux_new(o,6);						\
	ttt xlo = lo, xhi = hi, xlen = (xhi - xlo), xun = xlen/8.;	\
	LR_set_all(o,"abx", xlo, xhi, 4.0);				\
CU_ASSERT_EQUAL(LR_aux_set(o,xlo+2.0*xun, 1.0),0)			\
CU_ASSERT_EQUAL(LR_aux_set(o,xlo+2.5*xun,-1.0),LRerr_InvalidInputValue)	\
	LR_aux_set(o, xlo + 3.0*xun, 3.0);				\
	LR_aux_set(o, xlo + 4.0*xun, 0.0);				\
	LR_aux_set(o, xlo + 5.0*xun, 5.0);				\
	LR_aux_set(o, xlo + 7.0*xun, 2.0);				\
	LR_aux_norm(o);							\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo- .1),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo- .1),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+ .5*xun),.0833,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+ .5*xun),.1667/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+1.5*xun),.25,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+1.5*xun),.1667/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+2.5*xun),.3542,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+2.5*xun),.0417/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+3.5*xun),.4375,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+3.5*xun),.125/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+4.5*xun),.5,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+4.5*xun),.0,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+5.5*xun),.6042,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+5.5*xun),.2083/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+6.5*xun),.8125,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+6.5*xun),.2083/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+7.5*xun),.9583,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+7.5*xun),.0833/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xhi+.1),1.,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xhi+.1),0.,tol)			\
}

testCdfPdf(0,d,double,piece,75,.0001,.001,
	LR_aux_new(o,6);
	LR_set_all(o,"abx", -2., 6., 4.);
	LR_aux_set(o, o->a.d + 2.0, 1.0);
	LR_aux_set(o, o->a.d + 3.0, 3.0);
	LR_aux_set(o, o->a.d + 4.0, 0.0);
	LR_aux_set(o, o->a.d + 5.0, 5.0);
	LR_aux_set(o, o->a.d + 7.0, 2.0);
	LR_aux_norm(o);
)
testCdfPdf(1,d,double,piece,55,.0001,.001,
	LR_aux_new(o,6);
	LR_set_all(o,"abx", -2., 6., 4.);
	LR_aux_set(o, o->a.d + 2.0, 1.0);
	LR_aux_set(o, o->a.d + 3.0, 3.0);
	LR_aux_set(o, o->a.d + 4.0, 0.0);
	LR_aux_set(o, o->a.d + 5.0, 5.0);
	LR_aux_set(o, o->a.d + 7.0, 2.0);
	LR_aux_norm(o);
)
testCdfPdf0piece(2,d,double,.001,-2.,6.);
testCdfPdf0piece(3,d,double,.001,0.,8.);

testCdfPdf(0,f,float,piece,75,.0001,.001,
	LR_aux_new(o,6);
	LR_set_all(o,"abx", -2., 6., 4.);
	LR_aux_set(o, o->a.d + 2.0, 1.0);
	LR_aux_set(o, o->a.d + 3.0, 3.0);
	LR_aux_set(o, o->a.d + 4.0, 0.0);
	LR_aux_set(o, o->a.d + 5.0, 5.0);
	LR_aux_set(o, o->a.d + 7.0, 2.0);
	LR_aux_norm(o);
)
testCdfPdf(1,f,float,piece,55,.0001,.001,
	LR_aux_new(o,6);
	LR_set_all(o,"abx", -2., 6., 4.);
	LR_aux_set(o, o->a.d + 2.0, 1.0);
	LR_aux_set(o, o->a.d + 3.0, 3.0);
	LR_aux_set(o, o->a.d + 4.0, 0.0);
	LR_aux_set(o, o->a.d + 5.0, 5.0);
	LR_aux_set(o, o->a.d + 7.0, 2.0);
	LR_aux_norm(o);
)
testCdfPdf0piece(2,f,float,.001,-2.,6.);
testCdfPdf0piece(3,f,float,.001,2.,12.);

testLRunif(1,d,double,piece,32,
	LR_aux_new(o,6);
	LR_set_all(o,"abx", -2., 6., 4.);
	LR_aux_set(o, o->a.d + 2.0, 1.0);
	LR_aux_set(o, o->a.d + 3.0, 3.0);
	LR_aux_set(o, o->a.d + 4.0, 0.0);
	LR_aux_set(o, o->a.d + 5.0, 5.0);
	LR_aux_set(o, o->a.d + 7.0, 2.0);
	LR_aux_norm(o);
)
testLRunif(2,d,double,piece,75,
	LR_aux_new(o,6);
	LR_set_all(o,"abx", -4., 4., 4.);
	LR_aux_set(o, o->a.d + 2.0, 1.0);
	LR_aux_set(o, o->a.d + 3.0, 3.0);
	LR_aux_set(o, o->a.d + 4.0, 0.0);
	LR_aux_set(o, o->a.d + 5.0, 5.0);
	LR_aux_set(o, o->a.d + 7.0, 2.0);
	LR_aux_norm(o);
)
testLRunif(3,d,double,piece,50,
	LR_aux_new(o,6);
	LR_set_all(o,"abx", -1., 4., 3.);
	LR_aux_set(o, o->a.d + 2.0, 1.0);
	LR_aux_set(o, o->a.d + 3.0, 3.0);
	LR_aux_norm(o);
)
testLRunif(4,d,double,piece,60,
	LR_aux_new(o,6);
	LR_set_all(o,"abx", 0., 3., 1.);
	LR_aux_set(o, o->a.d + 1.0, 2.0);
	LR_aux_set(o, o->a.d + 2.0, 3.0);
	LR_aux_norm(o);
)
testLRunif(1,f,float,piece,32,
	LR_aux_new(o,6);
	LR_set_all(o,"abx", -2., 6., 4.);
	LR_aux_set(o, o->a.f + 2.0, 1.0);
	LR_aux_set(o, o->a.f + 3.0, 3.0);
	LR_aux_set(o, o->a.f + 4.0, 0.0);
	LR_aux_set(o, o->a.f + 5.0, 5.0);
	LR_aux_set(o, o->a.f + 7.0, 2.0);
	LR_aux_norm(o);
)
testLRunif(2,f,float,piece,75,
	LR_aux_new(o,6);
	LR_set_all(o,"abx", -4., 4., 4.);
	LR_aux_set(o, o->a.f + 2.0, 1.0);
	LR_aux_set(o, o->a.f + 3.0, 3.0);
	LR_aux_set(o, o->a.f + 4.0, 0.0);
	LR_aux_set(o, o->a.f + 5.0, 5.0);
	LR_aux_set(o, o->a.f + 7.0, 2.0);
	LR_aux_norm(o);
)
testLRunif(3,f,float,piece,50,
	LR_aux_new(o,6);
	LR_set_all(o,"abx", -1., 4., 3.);
	LR_aux_set(o, o->a.f + 2.0, 1.0);
	LR_aux_set(o, o->a.f + 3.0, 3.0);
	LR_aux_norm(o);
)
testLRunif(4,f,float,piece,60,
	LR_aux_new(o,6);
	LR_set_all(o,"abx", 0., 3., 1.);
	LR_aux_set(o, o->a.f + 1.0, 2.0);
	LR_aux_set(o, o->a.f + 2.0, 3.0);
	LR_aux_norm(o);
)

/* test some errors */
#define testBADpiece(nn,tt,ttt,lo,hi,xx,code)				\
void test_bad_##tt ## _piece ## _##nn(void) {				\
	LR_obj *o = LR_new(piece, LR_##ttt);				\
	LR_aux_new(o,6);						\
	LR_set_all(o,"abx", lo, hi, xx);				\
	code;								\
}

testBADpiece(0,d,double,2.,4.,1.,
	CU_ASSERT_TRUE(LR_aux_set(o,0.,2.));
	CU_ASSERT_TRUE(LR_aux_set(o,5.,1.));
	CU_ASSERT_TRUE(LR_aux_set(o,3.,-2.));
	CU_ASSERT(isnan(LRd_RAN(o)));
	CU_ASSERT(isnan(LRd_PDF(o,3.)));
	CU_ASSERT(isnan(LRd_CDF(o,3.)));
)

testBADpiece(0,f,float,2.,4.,1.,
	CU_ASSERT_TRUE(LR_aux_set(o,0.,2.));
	CU_ASSERT_TRUE(LR_aux_set(o,5.,1.));
	CU_ASSERT_TRUE(LR_aux_set(o,3.,-2.));
	CU_ASSERT(isnan(LRf_RAN(o)));
	CU_ASSERT(isnan(LRf_PDF(o,3.)));
	CU_ASSERT(isnan(LRf_CDF(o,3.)));
)

/* gsn2 */
#define testCdfPdf0gsn2(nn,tt,ttt,tol,setup)				\
void test_cdf_pdf_##tt ## _gsn2 ## _##nn(void) {			\
	LR_obj *o = LR_new(gsn2, LR_##ttt);				\
	double	ival,pval;						\
	setup;								\
	ival = (o->b.tt - o->a.tt);					\
	pval = 2.0/ival;						\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt-.1),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt-.1),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt+.25*ival),.125,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt+.25*ival),pval*.5,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt+.5*ival),.5,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt+.5*ival),pval,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt+.75*ival),.875,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt+.75*ival),pval*.5,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->b.tt),1.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->b.tt),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->b.tt+.1),1.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->b.tt+.1),0.,tol)		\
}

testCdfPdf0gsn2(0,d,double,.001,)
testCdfPdf(1,d,double,gsn2,20,.0001,.001,)
testCdfPdf(2,d,double,gsn2,50,.0001,.001,)
testCdfPdf(3,d,double,gsn2,90,.0001,.001,)

testCdfPdf0gsn2(4,d,double,.001,LR_set_all(o,"ab",-3.,1.))
testCdfPdf(5,d,double,gsn2,33,.0001,.001,LR_set_all(o,"ab",-1.,3.))

testSymDF(6,d, double, gsn2, -.9, .9, .0001,);
testSymDF(7,d, double, gsn2, -.6, .6, .0001,);
testSymDF(8,d, double, gsn2, -.3, .3, .0001,);

testCdfPdf0gsn2(0,f,float,.001,)
testCdfPdf(1,f,float,gsn2,20,.0001,.003,)
testCdfPdf(2,f,float,gsn2,50,.0001,.003,)
testCdfPdf(3,f,float,gsn2,90,.0001,.003,)

testCdfPdf0gsn2(4,f,float,.003,LR_set_all(o,"ab",-3.,1.))
testCdfPdf(5,f,float,gsn2,33,.0001,.003,LR_set_all(o,"ab",-1.,3.))

testSymDF(6,f, float, gsn2, -.9, .9, .0001,);
testSymDF(7,f, float, gsn2, -.6, .6, .0001,);
testSymDF(8,f, float, gsn2, -.3, .3, .0001,);

#define testLRgsn2(nn,tt,ttt,bn,setup)					\
	testLRvar(gsn2,nn,tt,ttt,bn,.1,27,setup)

testLRgsn2(1,d,double,10,)
testLRgsn2(2,d,double,25,)
testLRgsn2(3,d,double,20,LR_set_all(o,"ab",-2.,2.))
testLRgsn2(4,d,double,30,LR_set_all(o,"ab",-5.,1.))

testLRgsn2(1,f,float,10,)
testLRgsn2(2,f,float,25,)
testLRgsn2(3,f,float,20,LR_set_all(o,"ab",-2.,2.))
testLRgsn2(4,f,float,30,LR_set_all(o,"ab",-5.,1.))

/* gsn4 */
#define testCdfPdf0gsn4(nn,tt,ttt,tol,setup)				\
void test_cdf_pdf_##tt ## _gsn4 ## _##nn(void) {			\
	LR_obj *o = LR_new(gsn4, LR_##ttt);				\
	double	ival,pval;						\
	setup;								\
	ival = (o->b.tt - o->a.tt);					\
	pval = 4.0/ival;						\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt-.1),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt-.1),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt+.25*ival),.041667,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt+.25*ival),pval*.16666,tol)\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt+.5*ival),.5,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt+.5*ival),pval*.66666,tol)\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt+.75*ival),.95833,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt+.75*ival),pval*.16666,tol)\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->b.tt),1.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->b.tt),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->b.tt+.1),1.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->b.tt+.1),0.,tol)		\
}

testCdfPdf0gsn4(0,d,double,.001,)
testCdfPdf(1,d,double,gsn4,20,.0001,.001,)
testCdfPdf(2,d,double,gsn4,50,.0001,.001,)
testCdfPdf(3,d,double,gsn4,90,.0001,.001,)

testCdfPdf0gsn4(4,d,double,.0001,LR_set_all(o,"ab",-3.,1.))
testCdfPdf(5,d,double,gsn4,33,.0001,.003,LR_set_all(o,"ab", -1.,4.))

testSymDF(6,d, double, gsn4, -1.9, 1.9, .0001,);
testSymDF(7,d, double, gsn4, -1.6, 1.6, .0001,);
testSymDF(8,d, double, gsn4, -1.3, 1.3, .0001,);
testSymDF( 9,d, double, gsn4, -.9, .9, .0001,);
testSymDF(10,d, double, gsn4, -.6, .6, .0001,);
testSymDF(11,d, double, gsn4, -.3, .3, .0001,);

testCdfPdf0gsn4(0,f,float,.001,)
testCdfPdf(1,f,float,gsn4,20,.0001,.003,)
testCdfPdf(2,f,float,gsn4,50,.0001,.003,)
testCdfPdf(3,f,float,gsn4,90,.0001,.003,)

testCdfPdf0gsn4(4,f,float,.001,LR_set_all(o,"ab",-3.,1.))
testCdfPdf(5,f,float,gsn4,33,.0001,.003,LR_set_all(o,"ab", -1.,4.))

testSymDF(6,f, float, gsn4, -1.9, 1.9, .0001,);
testSymDF(7,f, float, gsn4, -1.6, 1.6, .0001,);
testSymDF(8,f, float, gsn4, -1.3, 1.3, .0001,);
testSymDF( 9,f, float, gsn4, -.9, .9, .0001,);
testSymDF(10,f, float, gsn4, -.6, .6, .0001,);
testSymDF(11,f, float, gsn4, -.3, .3, .0001,);


#define testLRgsn4(nn,tt,ttt,bn,setup)					\
	testLRvarx(gsn4,nn,tt,ttt,bn,100*10007,.1,100,setup)

testLRgsn4(1,d,double,10,)
testLRgsn4(2,d,double,25,)
testLRgsn4(3,d,double,20,LR_set_all(o,"ab",-4.,-1.))
testLRgsn4(4,d,double,30,LR_set_all(o,"ab",0.,6.))

testLRgsn4(1,f,float,10,)
testLRgsn4(2,f,float,25,)
testLRgsn4(3,f,float,20,LR_set_all(o,"ab",-4.,-1.))
testLRgsn4(4,f,float,30,LR_set_all(o,"ab",0.,6.))

/* gsn12 */
#define testCdfPdf0gsn12(nn,tt,ttt,tol,setup)				\
void test_cdf_pdf_##tt ## _gsn12 ## _##nn(void) {			\
	LR_obj *o = LR_new(gsn12, LR_##ttt);				\
	setup;								\
	double	m = o->m.tt, s = o->s.tt, iv,pv;			\
	iv = 2.0*s;							\
	pv = 2.0/iv;							\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,m-6.1*s),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,m-6.1*s),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,m-6.0*s),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,m-6.0*s),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,m-3.0*s),pv*50879./13305600.,tol)\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,m-3.0*s),397./394240.,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,m-2.0*s),pv*1093./19800.,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,m-2.0*s),29639./1330560.,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,m-s),pv*1623019./6652800.,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,m-s),12831419./79833600.,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,m),0.5,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,m),pv*655177./1663200.,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,m+s),pv*1623019./6652800.,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,m+s),67002181./79833600.,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,m+2.0*s),pv*1093./19800.,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,m+2.0*s),1300921./1330560.,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,m+3.0*s),pv*50879./13305600.,tol)\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,m+3.0*s),393843./394240.,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,m+6.0*s),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,m+6.0*s),1.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,m+6.1*s),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,m+6.1*s),1.,tol)		\
}

#define testLRgsn12(nn,tt,ttt,bn,setup)					\
	testLRfull(gsn12,nn,tt,ttt,4.,bn,100*10007,.1,20,setup)

testCdfPdf0gsn12(0,d,double,.001,)
testCdfPdf(1,d,double,gsn12,20,.0001,.001,)
testCdfPdf(2,d,double,gsn12,50,.0001,.001,)
testCdfPdf(3,d,double,gsn12,90,.0001,.001,)

testCdfPdf0gsn12(4,d,double,.0001,LR_set_all(o,"ms",-3.,.5))
testCdfPdf(5,d,double,gsn12,33,.0001,.003,LR_set_all(o,"ms", -1.,2.))

testSymDF(6,d, double, gsn12, -4.9, 4.9, .0001,);
testSymDF(7,d, double, gsn12, -3.6, 3.6, .0001,);
testSymDF(8,d, double, gsn12, -2.3, 2.3, .0001,);
testSymDF(9,d, double, gsn12, -1.9, 1.9, .0001,);
testSymDF(10,d, double, gsn12, -1.6, 1.6, .0001,);
testSymDF(11,d, double, gsn12, -1.3, 1.3, .0001,);
testSymDF(12,d, double, gsn12, -.9, .9, .0001,);
testSymDF(13,d, double, gsn12, -.6, .6, .0001,);
testSymDF(14,d, double, gsn12, -.3, .3, .0001,);

testLRgsn12(1,d,double,10,)
testLRgsn12(2,d,double,25,)
testLRgsn12(3,d,double,20,LR_set_all(o,"ms",-2.,2.))
testLRgsn12(4,d,double,30,LR_set_all(o,"ms",1.,.5))

testCdfPdf0gsn12(0,f,float,.002,)
testCdfPdf(1,f,float,gsn12,20,.0001,.002,)
testCdfPdf(2,f,float,gsn12,50,.0001,.002,)
testCdfPdf(3,f,float,gsn12,90,.0001,.002,)

testCdfPdf0gsn12(4,f,float,.0001,LR_set_all(o,"ms",-3.,.5))
testCdfPdf(5,f,float,gsn12,33,.0001,.003,LR_set_all(o,"ms", -1.,2.))

testSymDF(6,f, float, gsn12, -4.9, 4.9, .0001,);
testSymDF(7,f, float, gsn12, -3.6, 3.6, .0001,);
testSymDF(8,f, float, gsn12, -2.3, 2.3, .0001,);
testSymDF(9,f, float, gsn12, -1.9, 1.9, .0001,);
testSymDF(10,f, float, gsn12, -1.6, 1.6, .0001,);
testSymDF(11,f, float, gsn12, -1.3, 1.3, .0001,);
testSymDF(12,f, float, gsn12, -.9, .9, .0001,);
testSymDF(13,f, float, gsn12, -.6, .6, .0001,);
testSymDF(14,f, float, gsn12, -.3, .3, .0001,);

testLRgsn12(1,f,float,10,)
testLRgsn12(2,f,float,25,)
testLRgsn12(3,f,float,20,LR_set_all(o,"ms",-2.,2.))
testLRgsn12(4,f,float,30,LR_set_all(o,"ms",1.,.5))

/* lspline */
/* test some well defined points */
#define testCdfPdf0lspline(nn,tt,ttt,tol,lo,hi)				\
void test_cdf_pdf_##tt ## _lspline ## _##nn(void) {			\
	LR_obj *o = LR_new(lspline, LR_##ttt);				\
	LR_aux_new(o,6);						\
	ttt xlo = lo, xhi = hi, xlen = (xhi - xlo), xun = xlen/8.;	\
	LR_set_all(o,"ab", xlo, xhi);				\
CU_ASSERT_EQUAL(LR_aux_set(o,xlo+2.0*xun, 1.0),0)			\
CU_ASSERT_EQUAL(LR_aux_set(o,xlo+2.5*xun,-1.0),LRerr_InvalidInputValue)	\
	LR_aux_set(o, xlo + 3.0*xun, 3.0);				\
	LR_aux_set(o, xlo + 4.0*xun, 0.0);				\
	LR_aux_set(o, xlo + 5.0*xun, 5.0);				\
	LR_aux_set(o, xlo + 7.0*xun, 2.0);				\
	LR_aux_norm(o);						\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo- .1),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo- .1),0.,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+ .6*xun),.006,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+ .6*xun),.020/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+1.8*xun),.054,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+1.8*xun),.060/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+2.4*xun),.104,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+2.4*xun),.120/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+3.6*xun),.284,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+3.6*xun),.080/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+4.6*xun),.360,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+4.6*xun),.200/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+5.4*xun),.592,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+5.4*xun),.2933/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+6.6*xun),.872,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+6.6*xun),.1733/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+7.4*xun),.976,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+7.4*xun),.080/xun,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xhi+.1),1.,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xhi+.1),0.,tol)			\
}

testCdfPdf(0,d,double,lspline,75,.0001,.001,
	LR_aux_new(o,6);
	LR_set_all(o,"ab", -2., 6.);
	LR_aux_set(o, o->a.d + 2.0, 1.0);
	LR_aux_set(o, o->a.d + 3.0, 3.0);
	LR_aux_set(o, o->a.d + 4.0, 0.0);
	LR_aux_set(o, o->a.d + 5.0, 5.0);
	LR_aux_set(o, o->a.d + 7.0, 2.0);
	LR_aux_norm(o);
)
testCdfPdf(1,d,double,lspline,55,.0001,.001,
	LR_aux_new(o,6);
	LR_set_all(o,"ab", -2., 6.);
	LR_aux_set(o, o->a.d + 2.0, 1.0);
	LR_aux_set(o, o->a.d + 3.0, 3.0);
	LR_aux_set(o, o->a.d + 4.0, 0.0);
	LR_aux_set(o, o->a.d + 5.0, 5.0);
	LR_aux_set(o, o->a.d + 7.0, 2.0);
	LR_aux_norm(o);
)

testCdfPdf0lspline(2,d,double,.001,-2.,6.);
testCdfPdf0lspline(3,d,double,.001,2.,12.);

testCdfPdf(0,f,float,lspline,75,.0001,.001,
	LR_aux_new(o,6);
	LR_set_all(o,"ab", -2., 6.);
	LR_aux_set(o, o->a.f + 2.0, 1.0);
	LR_aux_set(o, o->a.f + 3.0, 3.0);
	LR_aux_set(o, o->a.f + 4.0, 0.0);
	LR_aux_set(o, o->a.f + 5.0, 5.0);
	LR_aux_set(o, o->a.f + 7.0, 2.0);
	LR_aux_norm(o);
)
testCdfPdf(1,f,float,lspline,55,.0001,.001,
	LR_aux_new(o,6);
	LR_set_all(o,"ab", -2., 6.);
	LR_aux_set(o, o->a.f + 2.0, 1.0);
	LR_aux_set(o, o->a.f + 3.0, 3.0);
	LR_aux_set(o, o->a.f + 4.0, 0.0);
	LR_aux_set(o, o->a.f + 5.0, 5.0);
	LR_aux_set(o, o->a.f + 7.0, 2.0);
	LR_aux_norm(o);
)

testCdfPdf0lspline(2,f,float,.001,-2.,6.);
testCdfPdf0lspline(3,f,float,.001,2.,12.);

#define testLRlspl(nn,tt,ttt,bn,setup)					\
	testLRvar(lspline,nn,tt,ttt,bn,.04,2,setup)

testLRlspl(1,d,double,75,
	LR_aux_new(o,6);
	LR_set_all(o,"ab", -2., 6.);
	LR_aux_set(o, o->a.d + 2.0, 1.0);
	LR_aux_set(o, o->a.d + 3.0, 3.0);
	LR_aux_set(o, o->a.d + 4.0, 0.0);
	LR_aux_set(o, o->a.d + 5.0, 5.0);
	LR_aux_set(o, o->a.d + 7.0, 2.0);
	LR_aux_norm(o);
)

testLRlspl(2,d,double,55,
	LR_aux_new(o,6);
	LR_set_all(o,"ab", -4., 4.);
	LR_aux_set(o, o->a.d + 2.0, 1.0);
	LR_aux_set(o, o->a.d + 3.0, 3.0);
	LR_aux_set(o, o->a.d + 4.0, 0.0);
	LR_aux_set(o, o->a.d + 5.0, 5.0);
	LR_aux_set(o, o->a.d + 7.0, 2.0);
	LR_aux_norm(o);
)

/* this should be equivalent to default gsn2, but better tolerances */

testLRlspl(3,d,double,80,
	LR_aux_new(o,2);
	LR_aux_set(o, o->a.d + 1.0, 1.0);
	LR_aux_norm(o);
)

testLRlspl(4,d,double,50,
	LR_aux_new(o,2);
	LR_aux_set(o, o->a.d + 1.0, 1.0);
	LR_aux_norm(o);
)

testLRlspl(1,f,float,75,
	LR_aux_new(o,6);
	LR_set_all(o,"ab", -2., 6.);
	LR_aux_set(o, o->a.f + 2.0, 1.0);
	LR_aux_set(o, o->a.f + 3.0, 3.0);
	LR_aux_set(o, o->a.f + 4.0, 0.0);
	LR_aux_set(o, o->a.f + 5.0, 5.0);
	LR_aux_set(o, o->a.f + 7.0, 2.0);
	LR_aux_norm(o);
)

testLRlspl(2,f,float,55,
	LR_aux_new(o,6);
	LR_set_all(o,"ab", -4., 4.);
	LR_aux_set(o, o->a.f + 2.0, 1.0);
	LR_aux_set(o, o->a.f + 3.0, 3.0);
	LR_aux_set(o, o->a.f + 4.0, 0.0);
	LR_aux_set(o, o->a.f + 5.0, 5.0);
	LR_aux_set(o, o->a.f + 7.0, 2.0);
	LR_aux_norm(o);
)

testLRlspl(3,f,float,80,
	LR_aux_new(o,2);
	LR_aux_set(o, o->a.f + 1.0, 1.0);
	LR_aux_norm(o);
)

testLRlspl(4,f,float,50,
	LR_aux_new(o,2);
	LR_aux_set(o, o->a.f + 1.0, 1.0);
	LR_aux_norm(o);
)

/* test some errors */
#define testBADlspline(nn,tt,ttt,lo,hi,code)				\
void test_bad_##tt ## _lspline ## _##nn(void) {			\
	LR_obj *o = LR_new(lspline, LR_##ttt);				\
	LR_aux_new(o,6);						\
	LR_set_all(o,"ab", lo, hi);					\
	code;								\
}

testBADlspline(0,d,double,2.,4.,
	CU_ASSERT_TRUE(LR_aux_set(o,0.,2.));
	CU_ASSERT_TRUE(LR_aux_set(o,5.,1.));
	CU_ASSERT_TRUE(LR_aux_set(o,3.,-2.));
	CU_ASSERT_TRUE(LR_aux_norm(o));
	CU_ASSERT(isnan(LRd_RAN(o)));
	CU_ASSERT(isnan(LRd_PDF(o,3.)));
	CU_ASSERT(isnan(LRd_CDF(o,3.)));
)

testBADlspline(0,f,float,2.,4.,
	CU_ASSERT_TRUE(LR_aux_set(o,0.,2.));
	CU_ASSERT_TRUE(LR_aux_set(o,5.,1.));
	CU_ASSERT_TRUE(LR_aux_set(o,3.,-2.));
	CU_ASSERT_TRUE(LR_aux_norm(o));
	CU_ASSERT(isnan(LRf_RAN(o)));
	CU_ASSERT(isnan(LRf_PDF(o,3.)));
	CU_ASSERT(isnan(LRf_CDF(o,3.)));
)

/* uinvcdf */
double MyCDF(double x) {
	static double pi4 = NAN;
	double zero = 0.0, one = 1.0, two = 2.0;
	if (isnan(pi4))
		pi4 = atan(one);

	if (x <= zero) {
		return zero;
	} else if (x >= two) {
		return one;
	} else {
		double t = sin(pi4*x);
		return t*t;
	}
}

double MyCDF2(double x) {
	return MyCDF(x + 2.0);
}

double MyCDFc(double x) {
/* Cauchy CDF (moved by 0.5) */
	double half = 0.5;
	return half + M_1_PI *atan(x - half);
}

float MyCDFf(float x) {
	static float pi4 = NAN;
	float zero = 0.0, one = 1.0, two = 2.0;
	if (isnan(pi4))
		pi4 = atan(one);

	if (x <= zero) {
		return zero;
	} else if (x >= two) {
		return one;
	} else {
		float t = sin(pi4*x);
		return t*t;
	}
}

float MyCDF2f(float x) {
	return MyCDF(x + 2.0);
}

float MyCDFcf(float x) {
/* Cauchy CDF (moved by 0.5) */
	float half = 0.5;
	return half + M_1_PI *atan(x - half);
}

#define testCdfPdf0uinvcdf(nn,tt,ttt,cdf,tol,xlo,xhi)			\
void test_cdf_pdf_##tt ## _uinvcdf ## _##nn(void) {			\
	LR_obj *o = LR_new(uinvcdf, LR_##ttt);				\
	ttt xun = (xhi - xlo);						\
	LR_set_all(o,"ab", xlo, xhi);					\
	LR##tt ## _uinvcdf(o,cdf);					\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo-.1),0.,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo-.1),0.,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+.25*xun),.146447,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+.25*xun),.555360,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+.50*xun),.500000,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+.50*xun),.785398,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+.60*xun),.654509,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+.60*xun),.746958,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+.80*xun),.904509,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+.80*xun),.461645,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xlo+.95*xun),.993844,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xlo+.95*xun),.122863,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,xhi+.1),1.,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,xhi+.1),0.,tol)			\
}

testLRcheck(9, uinvcdf, d, double, 
	CU_ASSERT_EQUAL(LRf_uinvcdf(o,MyCDFf), LRerr_BadDataType);
)
testLRcheck(9, uinvcdf, f, float, 
	CU_ASSERT_EQUAL(LRd_uinvcdf(o,MyCDF), LRerr_BadDataType);
)

testCdfPdf0uinvcdf(0,d,double,MyCDF,.0001,0.,2.);
testCdfPdf0uinvcdf(1,d,double,MyCDF2,.0001,-2.,0.);
testSymDF(2,d,double,uinvcdf, .1, 1.9,.0001,
	LR_set_all(o,"ab", 0., 2.);
	LRd_uinvcdf(o,MyCDF);
)
testSymDF(3,d,double,uinvcdf, .25, 1.75,.0001,
	LR_set_all(o,"ab", 0., 2.);
	LRd_uinvcdf(o,MyCDF);
)
testSymDF(4,d,double,uinvcdf, .5, 1.5,.0001,
	LR_set_all(o,"ab", 0., 2.);
	LRd_uinvcdf(o,MyCDF);
)
testSymDF(5,d,double,uinvcdf, .75, 1.25,.0001,
	LR_set_all(o,"ab", 0., 2.);
	LRd_uinvcdf(o,MyCDF);
)
testSymDF(6,d,double,uinvcdf, .9, 1.1,.0001,
	LR_set_all(o,"ab", 0., 2.);
	LRd_uinvcdf(o,MyCDF);
)
testLRvar(uinvcdf,7,d,double, 40, .1, 10,
	LR_set_all(o,"ab", 0., 2.);
	LRd_uinvcdf(o,MyCDF);
)
testLRvar(uinvcdf,8,d,double, 60, .1, 10,
	LR_set_all(o,"ab", -2., 0.);
	LRd_uinvcdf(o,MyCDF2);
)

testCdfPdf0uinvcdf(0,f,float,MyCDFf,.001,0.,2.);
testCdfPdf0uinvcdf(1,f,float,MyCDF2f,.001,-2.,0.);
testSymDF(2,f,float,uinvcdf, .1, 1.9,.001,
	LR_set_all(o,"ab", 0., 2.);
	LRf_uinvcdf(o,MyCDFf);
)
testSymDF(3,f,float,uinvcdf, .25, 1.75,.001,
	LR_set_all(o,"ab", 0., 2.);
	LRf_uinvcdf(o,MyCDFf);
)
testSymDF(4,f,float,uinvcdf, .5, 1.5,.001,
	LR_set_all(o,"ab", 0., 2.);
	LRf_uinvcdf(o,MyCDFf);
)
testSymDF(5,f,float,uinvcdf, .75, 1.25,.001,
	LR_set_all(o,"ab", 0., 2.);
	LRf_uinvcdf(o,MyCDFf);
)
testSymDF(6,f,float,uinvcdf, .9, 1.1,.001,
	LR_set_all(o,"ab", 0., 2.);
	LRf_uinvcdf(o,MyCDFf);
)
testLRvar(uinvcdf,7,f,float, 40, .1, 10,
	LR_set_all(o,"ab", 0., 2.);
	LRf_uinvcdf(o,MyCDFf);
)
testLRvar(uinvcdf,8,f,float, 60, .1, 10,
	LR_set_all(o,"ab", -2., 0.);
	LRf_uinvcdf(o,MyCDF2f);
)

/* CDF/PDF tests for half range */
/* testCdfPdfHR (half range)
 * nn	- test number
 * tt	- LR data type (d or f)
 * ttt	- LR data type (double or float)
 * dist	- LR variate type
 * end	- upper value
 * nnt	- number of test values
 * tol	- absolute tolerance
 * setup- LR object parameter set-up
 */
#define testCdfPdfHR(nn,tt,ttt,dist,end,nnt,tol,setup)			\
void test_cdf_pdf_##tt ## _##dist ## _##nn(void) {			\
	LR_obj *o = LR_new(dist, LR_##ttt);				\
	double	incr = end/nnt, x = 0.0;				\
	setup;								\
	for (int i = 0; i < nnt; i++) {					\
		compCdfPdf(tt,o,x,x+.0001,tol)				\
		x += incr;						\
	}								\
}

/* negative exponential */
/* nn	- test # 
 * tt	- LR data type (d or f)
 * ttt	- data type (double or float)
 * tol	- tolerance
 * mean	- mean value
 */
#define testCdfPdf0nexp(dist,nn,tt,ttt,tol,mean)			\
void test_cdf_pdf_##tt ## _ ## dist ## _##nn(void) {			\
	LR_obj *o = LR_new(dist, LR_##ttt);				\
	LR_set_all(o,"m", mean);					\
	ttt cc = 1.0/M_E;						\
	ttt cc2 = sqrt(cc);						\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mean*0.5),1.0-cc2,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mean*0.5),cc2/mean,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mean),1.0-cc,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mean),cc/mean,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mean*2.0),1.0-cc*cc,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mean*2.0),cc*cc/mean,tol)	\
}

testCdfPdf0nexp(nexp,0,d,double,.0001,1.0)
testCdfPdf0nexp(nexp,1,d,double,.0001,.3)
testCdfPdf0nexp(nexp,2,d,double,.0001,3.0)

testCdfPdfHR(3,d,double,nexp,3.,60,.0001,)
testCdfPdfHR(4,d,double,nexp,3.,60,.0005,
	LR_set_all(o,"m", .5);
)
testCdfPdfHR(5,d,double,nexp,4.,80,.0001,
	LR_set_all(o,"m", 2.);
)

testCdfPdf0nexp(nexp,0,f,float,.001,1.0)
testCdfPdf0nexp(nexp,1,f,float,.001,.3)
testCdfPdf0nexp(nexp,2,f,float,.001,3.0)

testCdfPdfHR(3,f,float,nexp,3.,60,.001,)
testCdfPdfHR(4,f,float,nexp,3.,60,.005,
	LR_set_all(o,"m", .5);
)
testCdfPdfHR(5,f,float,nexp,4.,80,.001,
	LR_set_all(o,"m", 2.);
)

#define testLRnexp(nn,tt,ttt,end,bn,setup)				\
	testLRhalf(nexp,nn,tt,ttt,end,bn,50*10007,.1,100,setup)

testLRnexp(1,d,double,3.0,60, )
testLRnexp(2,d,double,2.0,60,
	LR_set_all(o,"m", .5);
)
testLRnexp(3,d,double,4.0,80,
	LR_set_all(o,"m", 2.0);
)

testLRnexp(1,f,float,3.0,60, )
testLRnexp(2,f,float,2.0,60,
	LR_set_all(o,"m", .5);
)
testLRnexp(3,f,float,4.0,80,
	LR_set_all(o,"m", 2.0);
)

/* erlang */
#define testLRerlang(nn,tt,ttt,end,bn,setup)				\
	testLRhalf(erlang,nn,tt,ttt,end,bn,50*10007,.1,100,setup)

/* default k=1 => nexp */
testCdfPdf0nexp(erlang,0,d,double,.0001,1.0)
testCdfPdf0nexp(erlang,1,d,double,.0001,.3)
testCdfPdf0nexp(erlang,2,d,double,.0001,3.0)

testCdfPdfHR(3,d,double,erlang,3.,60,.0001,)
testCdfPdfHR(4,d,double,erlang,3.,60,.0005,
	LR_set_all(o,"m", .5);
)
testCdfPdfHR(5,d,double,erlang,4.,80,.0001,
	LR_set_all(o,"m", 2.);
)

testLRerlang(1,d,double,3.0,60, )
testLRerlang(2,d,double,2.0,60,
	LR_set_all(o,"m", .5);
)
testLRerlang(3,d,double,4.0,80,
	LR_set_all(o,"m", 2.0);
)

/* k=2 */
#define testCdfPdf0erlang2(nn,tt,ttt,tol,mn)				\
void test_cdf_pdf_##tt ## _ ## erlang2 ## _##nn(void) {			\
	LR_obj *o = LR_new(erlang, LR_##ttt);				\
	LR_set_all(o,"mk", mn, 2);					\
	ttt cc = 1.0/M_E;						\
	ttt cc2 = sqrt(cc);						\
	ttt xm = 1.0/mn;						\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,-.1),0.,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,-.1),0.,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mn*0.5),1.0-cc2*1.5,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mn*0.5),.5*cc2*xm,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mn),1.0-cc*2.0,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mn),cc*xm,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mn*2.0),1.0-cc*cc*3.0,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mn*2.0),2.0*cc*cc*xm,tol)	\
}

testCdfPdf0erlang2(0,d,double,.0001,1.0)
testCdfPdf0erlang2(1,d,double,.0001,.3)
testCdfPdf0erlang2(2,d,double,.0001,3.0)
testLRerlang(4,d,double,3.0,60,
	LR_set_all(o,"k", 2);
)
testLRerlang(5,d,double,2.0,60,
	LR_set_all(o,"km", 2, .5);
)
testLRerlang(6,d,double,4.0,80,
	LR_set_all(o,"km", 2, 2.0);
)

/* k=3 */
#define testCdfPdf0erlang3(nn,tt,ttt,tol,mn)				\
void test_cdf_pdf_##tt ## _ ## erlang3 ## _##nn(void) {			\
	LR_obj *o = LR_new(erlang, LR_##ttt);				\
	LR_set_all(o,"mk", mn, 3);					\
	ttt cc = 1.0/M_E;						\
	ttt cc2 = sqrt(cc);						\
	ttt xm = 1.0/mn;						\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,-.1),0.,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,-.1),0.,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mn*0.5),1.0-cc2*1.625,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mn*0.5),.125*cc2*xm,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mn),1.0-cc*2.5,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mn),.5*cc*xm,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mn*2.0),1.0-cc*cc*5.0,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mn*2.0),2.0*cc*cc*xm,tol)	\
}

testCdfPdf0erlang3(0,d,double,.0001,1.0)
testCdfPdf0erlang3(1,d,double,.0001,.3)
testCdfPdf0erlang3(2,d,double,.0001,3.0)
testLRerlang(7,d,double,3.0,60,
	LR_set_all(o,"k", 3);
)
testLRerlang(8,d,double,2.0,60,
	LR_set_all(o,"km", 3, .5);
)
testLRerlang(9,d,double,4.0,80,
	LR_set_all(o,"km", 3, 2.0);
)

/* k=4 */
#define testCdfPdf0erlang4(nn,tt,ttt,tol,mn)				\
void test_cdf_pdf_##tt ## _ ## erlang4 ## _##nn(void) {			\
	LR_obj *o = LR_new(erlang, LR_##ttt);				\
	LR_set_all(o,"mk", mn, 4);					\
	ttt cc = 1.0/M_E;						\
	ttt cc2 = sqrt(cc);						\
	ttt xm = 1.0/mn;						\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,-.1),0.,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,-.1),0.,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mn*0.5),1.0-cc2*1.645833,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mn*0.5),.02083333*cc2*xm,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mn),1.0-cc*2.666667,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mn),.1666667*cc*xm,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mn*2.0),1.0-cc*cc*6.333333,tol)	\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mn*2.0),1.3333333*cc*cc*xm,tol)	\
}

testCdfPdf0erlang4(0,d,double,.0001,1.0)
testCdfPdf0erlang4(1,d,double,.0001,.3)
testCdfPdf0erlang4(2,d,double,.0001,3.0)
testLRerlang(10,d,double,3.0,60,
	LR_set_all(o,"k", 4);
)
testLRerlang(11,d,double,2.0,60,
	LR_set_all(o,"km", 4, .5);
)
testLRerlang(12,d,double,4.0,80,
	LR_set_all(o,"km", 4, 2.0);
)

/* float erlang */
testCdfPdf0nexp(erlang,0,f,float,.0001,1.0)
testCdfPdf0nexp(erlang,1,f,float,.0001,.3)
testCdfPdf0nexp(erlang,2,f,float,.0001,3.0)

testCdfPdfHR(3,f,float,erlang,3.,60,.001,)
testCdfPdfHR(4,f,float,erlang,3.,60,.005,
	LR_set_all(o,"m", .5);
)
testCdfPdfHR(5,f,float,erlang,4.,80,.001,
	LR_set_all(o,"m", 2.);
)

testLRerlang(1,f,float,3.0,60, )
testLRerlang(2,f,float,2.0,60,
	LR_set_all(o,"m", .5);
)
testLRerlang(3,f,float,4.0,80,
	LR_set_all(o,"m", 2.0);
)

testCdfPdf0erlang2(0,f,float,.001,1.0)
testCdfPdf0erlang2(1,f,float,.001,.3)
testCdfPdf0erlang2(2,f,float,.001,3.0)
testLRerlang(4,f,float,3.0,60,
	LR_set_all(o,"k", 2);
)
testLRerlang(5,f,float,2.0,60,
	LR_set_all(o,"km", 2, .5);
)
testLRerlang(6,f,float,4.0,80,
	LR_set_all(o,"km", 2, 2.0);
)

testCdfPdf0erlang3(0,f,float,.001,1.0)
testCdfPdf0erlang3(1,f,float,.001,.3)
testCdfPdf0erlang3(2,f,float,.001,3.0)
testLRerlang(7,f,float,3.0,60,
	LR_set_all(o,"k", 3);
)
testLRerlang(8,f,float,2.0,60,
	LR_set_all(o,"km", 3, .5);
)
testLRerlang(9,f,float,4.0,80,
	LR_set_all(o,"km", 3, 2.0);
)

testCdfPdf0erlang4(0,f,float,.001,1.0)
testCdfPdf0erlang4(1,f,float,.001,.3)
testCdfPdf0erlang4(2,f,float,.001,3.0)
testLRerlang(10,f,float,3.0,60,
	LR_set_all(o,"k", 4);
)
testLRerlang(11,f,float,2.0,60,
	LR_set_all(o,"km", 4, .5);
)
testLRerlang(12,f,float,4.0,80,
	LR_set_all(o,"km", 4, 2.0);
)

/* CDF/PDF tests for full range */
/* testCdfPdfFR (full range)
 * nn	- test number
 * tt	- LR data type (d or f)
 * ttt	- LR data type (double or float)
 * dist	- LR variate type
 * ww	- number of std.dev on either side of mean.
 * nnt	- number of test values
 * tol	- absolute tolerance
 * setup- LR object parameter set-up
 */
#define testCdfPdfFR(nn,tt,ttt,dist,ww,nnt,tol,setup)			\
void test_cdf_pdf_##tt ## _##dist ## _##nn(void) {			\
	LR_obj *o = LR_new(dist, LR_##ttt);				\
	double	incr, x;						\
	setup;								\
	incr = 2.*ww*o->s.tt/nnt;					\
	x = o->m.tt - o->s.tt*ww;					\
	for (int i = 0; i < nnt; i++) {					\
		compCdfPdf(tt,o,x,x+.0001,tol)				\
		x += incr;						\
	}								\
}

/* gaussian/normal */
/* ggg	- gaussian/normal type
 * nn	- test # 
 * tt	- LR data type (d or f)
 * ttt	- data type (double or float)
 * tol	- tolerance
 * mean	- central value
 * dev	- std.deviation width
 */
#define testCdfPdf0gaus(ggg,nn,tt,ttt,tol,mean,dev)			\
void test_cdf_pdf_##tt ## _##ggg ## _##nn(void) {			\
	LR_obj *o = LR_new(ggg, LR_##ttt);				\
	ttt cc = .5*M_2_SQRTPI*M_SQRT1_2/dev;				\
	ttt ee = exp(-0.5);						\
	ttt efm1 = .5*(1.0 + erf(-M_SQRT1_2));				\
	ttt efp1 = .5*(1.0 + erf(M_SQRT1_2));				\
	LR_set_all(o,"ms", mean, dev);					\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mean-dev),efm1,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mean-dev),cc*ee,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mean),.5,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mean),cc,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,mean+dev),efp1,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,mean+dev),cc*ee,tol)		\
}

testCdfPdf0gaus(gausbm,0,d,double,.0001,0.0, 1.0)
testCdfPdf0gaus(gausbm,1,d,double,.0001,-1.0, 2.0)
testCdfPdf0gaus(gausbm,2,d,double,.0001,1.5, .5)

testCdfPdf0gaus(gausbm,0,f,float,.0001,0.0, 1.0)
testCdfPdf0gaus(gausbm,1,f,float,.0001,-1.0, 2.0)
testCdfPdf0gaus(gausbm,2,f,float,.0001,1.5, .5)

testCdfPdfFR(3,d,double,gausbm,3,60,.0001,)
testCdfPdfFR(4,d,double,gausbm,3,60,.0001,
	LR_set_all(o,"ms", -2., .75);
)
testCdfPdfFR(5,d,double,gausbm,4,80,.0001,
	LR_set_all(o,"ms", 2., 2.50);
)

testSymDF(6,d, double, gausbm, -5.5, 5.5, .0001,);
testSymDF(7,d, double, gausbm, -4.6, 4.6, .0001,);
testSymDF(8,d, double, gausbm, -3.3, 3.3, .0001,);
testSymDF(9,d, double, gausbm, -2.9, 2.9, .0001,);
testSymDF(10,d, double, gausbm,-1.4, 1.4, .0001,);
testSymDF(11,d, double, gausbm, -.7,  .7, .0001,);

testCdfPdfFR(3,f,float,gausbm,3,60,.001,)
testCdfPdfFR(4,f,float,gausbm,3,60,.001,
	LR_set_all(o,"ms", -2., .75);
)
testCdfPdfFR(5,f,float,gausbm,4,80,.001,
	LR_set_all(o,"ms", 2., 2.50);
)
testSymDF(6,f, float, gausbm, -5.5, 5.5, .0001,);
testSymDF(7,f, float, gausbm, -4.6, 4.6, .0001,);
testSymDF(8,f, float, gausbm, -3.3, 3.3, .0001,);
testSymDF(9,f, float, gausbm, -2.9, 2.9, .0001,);
testSymDF(10,f, float, gausbm,-1.4, 1.4, .0001,);
testSymDF(11,f, float, gausbm, -.7,  .7, .0001,);

testCdfPdf0gaus(gausmar,0,d,double,.0001,0.0, 1.0)
testCdfPdf0gaus(gausmar,0,f,float,.001,0.0, 1.0)
testCdfPdfFR(1,d,double,gausmar,3,60,.0001,)
testCdfPdfFR(1,f,float,gausmar,3,60,.001,)

#define testLRgausbm(nn,tt,ttt,ww,bn,setup)				\
	testLRfull(gausbm,nn,tt,ttt,ww,bn,50*10007,.1,100,setup)

testLRgausbm(1,d,double,3.0,60, )
testLRgausbm(2,d,double,3.0,60,
	LR_set_all(o,"ms", -2., .75);
)
testLRgausbm(3,d,double,3.0,60,
	LR_set_all(o,"ms", 2., 2.50);
)

testLRgausbm(1,f,float,3.0,60, )
testLRgausbm(2,f,float,3.0,60,
	LR_set_all(o,"ms", -2., .75);
)
testLRgausbm(3,f,float,3.0,60,
	LR_set_all(o,"ms", 2., 2.50);
)

#define testLRgausmar(nn,tt,ttt,ww,bn,setup)				\
	testLRfull(gausmar,nn,tt,ttt,ww,bn,50*10007,.1,100,setup)

testLRgausmar(1,d,double,3.0,60, )
testLRgausmar(2,d,double,3.0,60,
	LR_set_all(o,"ms", -2., .75);
)
testLRgausmar(3,d,double,3.0,60,
	LR_set_all(o,"ms", 2., 2.50);
)

testLRgausmar(1,f,float,3.0,60, )
testLRgausmar(2,f,float,3.0,60,
	LR_set_all(o,"ms", -2., .75);
)
testLRgausmar(3,f,float,3.0,60,
	LR_set_all(o,"ms", 2., 2.50);
)

/* Cauchy/Lortentz */
/* dist	- LR_type
 * nn	- test # 
 * tt	- LR data type (d or f)
 * ttt	- data type (double or float)
 * tol	- tolerance
 * peak	- central value
 * wid	- peak width
 * setup - added code
 */
#define testCdfPdf0cauchy(dist,nn,tt,ttt,tol,peak,wid,setup)		\
void test_cdf_pdf_##tt ## _## dist ## _##nn(void) {			\
	LR_obj *o = LR_new(dist, LR_##ttt);				\
	setup;								\
	ttt cc = M_1_PI/wid;						\
	LR_set_all(o,"ms", peak, wid);					\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,peak-wid),.25,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,peak-wid),cc*.5,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,peak),.5,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,peak),cc,tol)			\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,peak+wid),.75,tol)		\
CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,peak+wid),cc*.5,tol)		\
}

testCdfPdf0cauchy(cauchy,0,d,double,.0001,0.0, 1.0, )
testCdfPdf0cauchy(cauchy,1,d,double,.0001,-1.0, 2.0, )
testCdfPdf0cauchy(cauchy,2,d,double,.0001,1.5, .5, )

testCdfPdfFR(3,d,double,cauchy,3,60,.0001,)
testCdfPdfFR(4,d,double,cauchy,3,60,.0001,
	LR_set_all(o,"ms", -2., .75);
)
testCdfPdfFR(5,d,double,cauchy,4,80,.0001,
	LR_set_all(o,"ms", 2., 2.50);
)

testSymDF(6,d, double, cauchy, -5.5, 5.5, .0001,);
testSymDF(7,d, double, cauchy, -4.6, 4.6, .0001,);
testSymDF(8,d, double, cauchy, -3.3, 3.3, .0001,);
testSymDF(9,d, double, cauchy, -2.9, 2.9, .0001,);
testSymDF(10,d, double, cauchy,-1.4, 1.4, .0001,);
testSymDF(11,d, double, cauchy, -.7,  .7, .0001,);

testCdfPdf0cauchy(cauchy,0,f,float,.001,0.0, 1.0, )
testCdfPdf0cauchy(cauchy,1,f,float,.001,-1.0, 2.0, )
testCdfPdf0cauchy(cauchy,2,f,float,.001,1.5, .5, )

testCdfPdfFR(3,f,float,cauchy,3,60,.001,)
testCdfPdfFR(4,f,float,cauchy,3,60,.001,
	LR_set_all(o,"ms", -2., .75);
)
testCdfPdfFR(5,f,float,cauchy,4,80,.001,
	LR_set_all(o,"ms", 2., 2.50);
)

testSymDF(6,f, float, cauchy, -5.5, 5.5, .0001,);
testSymDF(7,f, float, cauchy, -4.6, 4.6, .0001,);
testSymDF(8,f, float, cauchy, -3.3, 3.3, .0001,);
testSymDF(9,f, float, cauchy, -2.9, 2.9, .0001,);
testSymDF(10,f, float, cauchy,-1.4, 1.4, .0001,);
testSymDF(11,f, float, cauchy, -.7,  .7, .0001,);

testCdfPdf0cauchy(cauchymar,0,d,double,.0001,0.0, 1.0, )
testCdfPdf0cauchy(cauchymar,1,d,double,.0001,-1.0, 2.0, )
testCdfPdf0cauchy(cauchymar,0,f,float,.001,0.0, 1.0, )
testCdfPdf0cauchy(cauchymar,1,f,float,.001,-1.0, 2.0, )

#define testLRcauchy(nn,tt,ttt,ww,bn,setup)				\
	testLRfull(cauchy,nn,tt,ttt,ww,bn,50*10007,.1,100,setup)

testLRcauchy(1,d,double,3.0,60, )
testLRcauchy(2,d,double,3.0,60,
	LR_set_all(o,"ms", -2., .75);
)
testLRcauchy(3,d,double,3.0,60,
	LR_set_all(o,"ms", 2., 2.50);
)

testLRcauchy(1,f,float,3.0,60, )
testLRcauchy(2,f,float,3.0,60,
	LR_set_all(o,"ms", -2., .75);
)
testLRcauchy(3,f,float,3.0,60,
	LR_set_all(o,"ms", 2., 2.50);
)

#define testLRcauchymar(nn,tt,ttt,ww,bn,setup)				\
	testLRfull(cauchymar,nn,tt,ttt,ww,bn,50*10007,.1,100,setup)

testLRcauchymar(1,d,double,3.0,60, )
testLRcauchymar(2,d,double,3.0,60,
	LR_set_all(o,"ms", -2., .75);
)
testLRcauchymar(3,d,double,3.0,60,
	LR_set_all(o,"ms", 2., 2.50);
)

testLRcauchymar(1,f,float,3.0,60, )
testLRcauchymar(2,f,float,3.0,60,
	LR_set_all(o,"ms", -2., .75);
)
testLRcauchymar(3,f,float,3.0,60,
	LR_set_all(o,"ms", 2., 2.50);
)

/* uinvcdf version of cauchy */
testCdfPdf0cauchy(uinvcdf, 9, d,double,.0001,0.5, 1.0,
	LRd_uinvcdf(o,MyCDFc);
)

testCdfPdfFR(10,d,double,uinvcdf,3,60,.0001,
	LRd_uinvcdf(o,MyCDFc);
	LR_set_all(o,"ms", .5, 1.);
)

testLRfull(uinvcdf,11,d,double,3.0,60,50*10007,.1,100,
	LRd_uinvcdf(o,MyCDFc);
	LR_set_all(o,"ms", .5, 1.);
)

testCdfPdf0cauchy(uinvcdf, 9, f,float,.001,0.5, 1.0,
	LRf_uinvcdf(o,MyCDFcf);
)

testCdfPdfFR(10,f,float,uinvcdf,3,60,.001,
	LRf_uinvcdf(o,MyCDFcf);
	LR_set_all(o,"ms", .5, 1.);
)

testLRfull(uinvcdf,11,f,float,3.0,60,50*10007,.1,100,
	LRf_uinvcdf(o,MyCDFcf);
	LR_set_all(o,"ms", .5, 1.);
)

/* data type errors */
testBADdt(unif,1,d,double, f, .5, )
testBADdt(unif,2,f,float, d, .5, )
testBADdt(cauchy,3,d,double, f, .5, )
testBADdt(cauchy,4,f,float, d, .5, )
testBADdt(gausbm,5,d,double, f, .5, )
testBADdt(gausbm,6,f,float, d, .5, )
testBADdt(nexp,7,d,double, f, .5, )
testBADdt(nexp,8,f,float, d, .5, )
testBADdt(gsn12,9,d,double, f, .5, )
testBADdt(gsn12,10,f,float, d, .5, )
testBADdt(poisson,11,i,int, d, .5, )
testBADdt(geometric,12,i,int, d, .5, )

/* Poisson */
#define tryPdfCdf(kk,pdf,cdf)						\
CU_ASSERT_DOUBLE_EQUAL(LRi_CDF(o,kk),cdf,.000001)			\
CU_ASSERT_DOUBLE_EQUAL(LRi_PDF(o,kk),pdf,.000001)

#define testCdfPdf0pois(nn,xp,setup)					\
void test_cdf_pdf_##xp ## _poisson ## _##nn(void) {			\
	LR_obj *o = LR_new(poisson, LR_int);				\
	LR_set_all(o,"p", xp ## .);					\
	setup;								\
}

testCdfPdf0pois(1,1,
  tryPdfCdf(0,.3678794,.3678794)
  tryPdfCdf(1,.3678794,.7357589)
  tryPdfCdf(2,.1839397,.9196986)
  tryPdfCdf(4,.0153283,.9963402)
  tryPdfCdf(8,.0000091,.9999989)
)
testCdfPdf0pois(2,2,
  tryPdfCdf(0,.1353353,.1353353)
  tryPdfCdf(1,.2706706,.4060058)
  tryPdfCdf(2,.2706706,.6766764)
  tryPdfCdf(4,.0902235,.9473470)
  tryPdfCdf(8,.0008593,.9997626)
)
testCdfPdf0pois(3,4,
  tryPdfCdf(0,.0183156,.0183156)
  tryPdfCdf(1,.0732626,.0915782)
  tryPdfCdf(3,.1953668,.4334701)
  tryPdfCdf(4,.1953668,.6288369)
  tryPdfCdf(8,.0297702,.9786366)
  tryPdfCdf(12,.0006415,.9997263)
)
testCdfPdf0pois(4,8,
  tryPdfCdf(0,.0003355,.0003355)
  tryPdfCdf(1,.0026837,.0030192)
  tryPdfCdf(4,.0572523,.0996324)
  tryPdfCdf(7,.1395865,.4529608)
  tryPdfCdf(8,.1395865,.5925473)
  tryPdfCdf(12,.0481268,.9362028)
  tryPdfCdf(15,.0090260,.9917690)
)

/* more complicated random variates do not have "uniform" coverage */
/* also the limited test t*rand functions do not adequately span space */
/* make tolerance adjustable */
/* vty	- LR distribution
 * nn	- test #
 * bn	- bin number
 * xtot	- total number of samples (should be large)
 * vsc	- % error tolerance (.1 = 10%)
 * vab	- absolute error tolerance
 * setup- set-up code
 */
#define testLRdisc(vty,nn,bn,xtot,vsc,vab,setup)			\
void test_##vty ## _i ## _##nn(void) {					\
	LR_obj *o = LR_new(vty, LR_int);				\
	LR_bin *b = LR_bin_new(bn+1);					\
	double	x, cdf[bn+1], tol;					\
	setup;								\
	double	begin = 0.5,						\
		incr = 1.0;						\
	for (int i = 0; i < bn; i++) {					\
		x = begin + i;						\
		LR_bin_set(b,x);					\
		cdf[i] = xtot*(LRi_PDF(o,x));				\
	}								\
	cdf[bn] = xtot * (1.0 - LRi_CDF(o,bn-1));			\
	for (int i = 0; i < xtot; i++) {				\
		LR_bin_add(b,LRi_RAN(o));				\
	}								\
	for (int i = 0; i <= bn; i++) {					\
		tol = max(cdf[i]*vsc,vab);				\
		CU_ASSERT_DOUBLE_EQUAL(1.0*b->bins[i],cdf[i],tol);	\
	}								\
	LR_rm(&o); LR_bin_rm(&b);					\
}

#define testLRpoisson(nn,bn,setup)					\
	testLRdisc(poisson,nn,bn,50*10007,.1,100,setup)

testLRpoisson(1,20, LR_set_all(o,"p", 1.); )
testLRpoisson(2,20, LR_set_all(o,"p", 2.); )
testLRpoisson(3,20, LR_set_all(o,"p", 4.); )
testLRpoisson(4,20, LR_set_all(o,"p", 8.); )
testLRpoisson(5,20, LR_set_all(o,"p", 5.5); )

testLRcheck(1, poisson, i, int, 
	LR_set_all(o, "p", -1.0);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_OK);
)
testLRcheck(2, poisson, i, int, 
	LR_set_all(o, "p", 0.0);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_InvalidInputValue);
)

/* Geometric */
#define testCdfPdf0geom(nn,xp,setup)					\
void test_cdf_pdf_##xp ## _geometric ## _##nn(void) {			\
	LR_obj *o = LR_new(geometric, LR_int);				\
	LR_set_all(o,"p", . ## xp);					\
	setup;								\
}

testCdfPdf0geom(1,25,
  tryPdfCdf(1,.250000,.250000)
  tryPdfCdf(2,.187500,.437500)
  tryPdfCdf(3,.140625,.578125)
  tryPdfCdf(4,.105469,.683594)
  tryPdfCdf(8,.033371,.899887)
)
testCdfPdf0geom(2,50,
  tryPdfCdf(1,.500000,.500000)
  tryPdfCdf(2,.250000,.750000)
  tryPdfCdf(3,.125000,.875000)
  tryPdfCdf(4,.062500,.937500)
  tryPdfCdf(8,.003906,.996094)
)
testCdfPdf0geom(3,75,
  tryPdfCdf(1,.750000,.750000)
  tryPdfCdf(2,.187500,.937500)
  tryPdfCdf(3,.046875,.984375)
  tryPdfCdf(4,.011719,.996094)
  tryPdfCdf(5,.002930,.999023)
)

#define testLRgeom(nn,bn,setup)					\
	testLRdisc(geometric,nn,bn,50*10007,.1,100,setup)

testLRgeom(1,10, )
testLRgeom(2,10, LR_set_all(o,"p", .5); )
testLRgeom(3,10, LR_set_all(o,"p", .2); )
testLRgeom(4,15, LR_set_all(o,"p", .35); )
testLRgeom(5,20, LR_set_all(o,"p", .65); )
testLRgeom(6,25, LR_set_all(o,"p", .8); )

testLRcheck(1, geometric, i, int, 
	LR_set_all(o, "p", -.5);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_OK);
)
testLRcheck(2, geometric, i, int, 
	LR_set_all(o, "p", 0.0);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_InvalidInputValue);
)
testLRcheck(3, geometric, i, int, 
	LR_set_all(o, "p", -2.0);
	CU_ASSERT_EQUAL(LR_check(o), LRerr_InvalidInputValue);
)

int main(int argc, char* argv[]) {
	CU_pSuite		pS		= NULL;
	CU_pSuite		pSint		= NULL;
	CU_pSuite		pShalf		= NULL;
	CU_pSuite		pSfull		= NULL;
	CU_pSuite		pSdisc		= NULL;
	CU_BasicRunMode		mode		= CU_BRM_VERBOSE;
	CU_ErrorAction		error_action	= CUEA_IGNORE;
	int i;
	int interact		= 0;

	setvbuf(stdout, NULL, _IONBF, 0);

	for (i=1 ; i<argc ; i++) {
		if (!strcmp("-i", argv[i])) {
			error_action = CUEA_IGNORE;
    		} else if (!strcmp("-f", argv[i])) {
			error_action = CUEA_FAIL;
    		} else if (!strcmp("-A", argv[i])) {
			error_action = CUEA_ABORT;
		} else if (!strcmp("-s", argv[i])) {
			mode = CU_BRM_SILENT;
		} else if (!strcmp("-n", argv[i])) {
			mode = CU_BRM_NORMAL;
		} else if (!strcmp("-I", argv[i])) {
			interact = 1;
		} else if (!strcmp("-v", argv[i])) {
			mode = CU_BRM_VERBOSE;
		} else {
			printf(
"\nUsage:  %s [options]\n\n"
"Options:	-i	ignore framework errors [default].\n"
"		-f	fail on framework error.\n"
"		-A	abort on framework error.\n\n"
"		-s	silent mode - no output to screen.\n"
"		-n	normal mode - standard output to screen.\n"
"		-I	interactive mode - uses curses.\n"
"		-v	verbose mode - max output to screen [default].\n\n"
"		-h	print this message and exit.\n\n", argv[0]);
			return 0;
		}
	}

	if (CUE_SUCCESS != CU_initialize_registry()) {
		printf("\nTest Registry initialization failed.");
		return CU_get_error();
	}
	pS = CU_add_suite("LR_test_suite", init_suite, clean_suite);
	if (pS == NULL) {
		printf("\nTest Registry Suite failure.");
		CU_cleanup_registry();
		return CU_get_error();
	}
	pSint = CU_add_suite("LR_test_suite_interval",
		init_suite, clean_suite);
	if (pSint == NULL) {
		printf("\nTest Registry Suite Interval failure.");
		CU_cleanup_registry();
		return CU_get_error();
	}
	pShalf = CU_add_suite("LR_test_suite_half_range",
		init_suite, clean_suite);
	if (pShalf == NULL) {
		printf("\nTest Registry Suite Half Range failure.");
		CU_cleanup_registry();
		return CU_get_error();
	}
	pSfull = CU_add_suite("LR_test_suite_full_range",
		init_suite, clean_suite);
	if (pSfull == NULL) {
		printf("\nTest Registry Suite Full Range failure.");
		CU_cleanup_registry();
		return CU_get_error();
	}
	pSdisc = CU_add_suite("LR_test_suite_discrete",
		init_suite, clean_suite);
	if (pSdisc == NULL) {
		printf("\nTest Registry Suite Discrete failure.");
		CU_cleanup_registry();
		return CU_get_error();
	}
	/* add tests to the suite */
if ((NULL == CU_add_test(pS,"errors", test_LR_errors))
||  (NULL == CU_add_test(pS,"version", test_LR_version))
||  (NULL == CU_add_test(pS,"new - int", test_new_int))
||  (NULL == CU_add_test(pS,"new - float", test_new_float))
||  (NULL == CU_add_test(pS,"new - long", test_new_long))
||  (NULL == CU_add_test(pS,"new - double", test_new_double))
||  (NULL == CU_add_test(pS,"set - int", test_set_int))
||  (NULL == CU_add_test(pS,"set - float", test_set_float))
||  (NULL == CU_add_test(pS,"set - long", test_set_long))
||  (NULL == CU_add_test(pS,"set - double", test_set_double))
||  (NULL == CU_add_test(pS,"set_all - int", test_set_all_int))
||  (NULL == CU_add_test(pS,"set_all - float", test_set_all_float))
||  (NULL == CU_add_test(pS,"set_all - long", test_set_all_long))
||  (NULL == CU_add_test(pS,"set_all - double",test_set_all_double))
||  (NULL == CU_add_test(pS,"set_all2 - int", test_set_all2_int))
||  (NULL == CU_add_test(pS,"set_all2 - float", test_set_all2_float))
||  (NULL == CU_add_test(pS,"set_all2 - long", test_set_all2_long))
||  (NULL == CU_add_test(pS,"set_all2 - double",test_set_all2_double))
||  (NULL == CU_add_test(pS,"set_all3 - int", test_set_all3_int))
||  (NULL == CU_add_test(pS,"set_all3 - float", test_set_all3_float))
||  (NULL == CU_add_test(pS,"set_all3 - long", test_set_all3_long))
||  (NULL == CU_add_test(pS,"set_all3 - double",test_set_all3_double))
||  (NULL == CU_add_test(pS,"check - bad",test_check_bad_0))
||  (NULL == CU_add_test(pS,"check - d - 0",test_check_unif_d_0))
||  (NULL == CU_add_test(pS,"check - d - 1",test_check_unif_d_1))
||  (NULL == CU_add_test(pS,"check - d - 2",test_check_gausbm_d_2))
||  (NULL == CU_add_test(pS,"check - d - 3",test_check_gausbm_d_3))
||  (NULL == CU_add_test(pS,"check - d - 4",test_check_uinvcdf_d_4))
||  (NULL == CU_add_test(pS,"check - d - 5",test_check_uinvcdf_d_5))
||  (NULL == CU_add_test(pS,"check - d - 6",test_check_uinvcdf_d_6))
||  (NULL == CU_add_test(pS,"check - d - 7",test_check_uinvcdf_d_7))
||  (NULL == CU_add_test(pS,"check - d - 8",test_check_uinvcdf_d_8))
||  (NULL == CU_add_test(pS,"check - d - 9",test_check_uinvcdf_d_9))
||  (NULL == CU_add_test(pS,"check - f - 0",test_check_unif_f_0))
||  (NULL == CU_add_test(pS,"check - f - 1",test_check_unif_f_1))
||  (NULL == CU_add_test(pS,"check - f - 2",test_check_gausbm_f_2))
||  (NULL == CU_add_test(pS,"check - f - 3",test_check_gausbm_f_3))
||  (NULL == CU_add_test(pS,"check - f - 4",test_check_uinvcdf_f_4))
||  (NULL == CU_add_test(pS,"check - f - 5",test_check_uinvcdf_f_5))
||  (NULL == CU_add_test(pS,"check - f - 6",test_check_uinvcdf_f_6))
||  (NULL == CU_add_test(pS,"check - f - 7",test_check_uinvcdf_f_7))
||  (NULL == CU_add_test(pS,"check - f - 8",test_check_uinvcdf_f_8))
||  (NULL == CU_add_test(pS,"check - f - 9",test_check_uinvcdf_f_9))
||  (NULL == CU_add_test(pS,"indep seq - 1",test_indep_seq_1))
||  (NULL == CU_add_test(pS,"indep seq - 2",test_indep_seq_2))
||  (NULL == CU_add_test(pS,"indep seq - 3",test_indep_seq_3))
||  (NULL == CU_add_test(pS,"indep seq - 4",test_indep_seq_4))
||  (NULL == CU_add_test(pS,"indep seq - 5",test_indep_seq_5))
||  (NULL == CU_add_test(pS,"indep seq - 6",test_indep_seq_6))
/*
||  (NULL == CU_add_test(pS,"new_bin - int", test_bin_new_int))
||  (NULL == CU_add_test(pS,"new_bin - float", test_bin_new_float))
||  (NULL == CU_add_test(pS,"new_bin - long", test_bin_new_long))
*/
||  (NULL == CU_add_test(pS,"new_bin - double", test_bin_new_double))
||  (NULL == CU_add_test(pS,"new_bin_set - 1", test_bin_set_1))
||  (NULL == CU_add_test(pS,"new_bin_set - 2", test_bin_set_2))
||  (NULL == CU_add_test(pS,"new_bin_set - 3", test_bin_set_3))
||  (NULL == CU_add_test(pS,"new_bin_set - 4", test_bin_set_4))
||  (NULL == CU_add_test(pS,"new_bin_add - 1", test_bin_add_1))
||  (NULL == CU_add_test(pS,"new_bin_add - 2", test_bin_add_2))
||  (NULL == CU_add_test(pS,"new_bin_add - 3", test_bin_add_3))
||  (NULL == CU_add_test(pS,"new_bin_add - 4", test_bin_add_4))
||  (NULL == CU_add_test(pS,"bad data type - 1", test_bad_d_dt_1))
||  (NULL == CU_add_test(pS,"bad data type - 2", test_bad_f_dt_2))
||  (NULL == CU_add_test(pS,"bad data type - 3", test_bad_d_dt_3))
||  (NULL == CU_add_test(pS,"bad data type - 4", test_bad_f_dt_4))
||  (NULL == CU_add_test(pS,"bad data type - 5", test_bad_d_dt_5))
||  (NULL == CU_add_test(pS,"bad data type - 6", test_bad_f_dt_6))
||  (NULL == CU_add_test(pS,"bad data type - 7", test_bad_d_dt_7))
||  (NULL == CU_add_test(pS,"bad data type - 8", test_bad_f_dt_8))
||  (NULL == CU_add_test(pS,"bad data type - 9", test_bad_d_dt_9))
||  (NULL == CU_add_test(pS,"bad data type - 10", test_bad_f_dt_10))
||  (NULL == CU_add_test(pS,"bad data type - 11", test_bad_i_dt_11))
||  (NULL == CU_add_test(pS,"bad data type - 12", test_bad_i_dt_12))
) {
		printf("\nTest Suite additions failure.");
		CU_cleanup_registry();
		return CU_get_error();
	}
if ((NULL == CU_add_test(pSint,"Unif-P/CDF-d-0", test_cdf_pdf_d_unif_0))
||  (NULL == CU_add_test(pSint,"Unif-P/CDF-d-1", test_cdf_pdf_d_unif_1))
||  (NULL == CU_add_test(pSint,"Unif-P/CDF-d-2", test_cdf_pdf_d_unif_2))
||  (NULL == CU_add_test(pSint,"Unif-P/CDF-d-3", test_cdf_pdf_d_unif_3))
||  (NULL == CU_add_test(pSint,"Unif-P/CDF-d-4", test_cdf_pdf_d_unif_4))
||  (NULL == CU_add_test(pSint,"Unif-P/CDF-d-5", test_cdf_pdf_d_unif_5))
||  (NULL == CU_add_test(pSint,"Unif-Ran-d-1", test_unif_d_1))
||  (NULL == CU_add_test(pSint,"Unif-Ran-d-2", test_unif_d_2))
||  (NULL == CU_add_test(pSint,"Unif-Ran-d-3", test_unif_d_3))
||  (NULL == CU_add_test(pSint,"Unif-Ran-d-4", test_unif_d_4))
||  (NULL == CU_add_test(pSint,"Unif-P/CDF-f-0", test_cdf_pdf_f_unif_0))
||  (NULL == CU_add_test(pSint,"Unif-P/CDF-f-1", test_cdf_pdf_f_unif_1))
||  (NULL == CU_add_test(pSint,"Unif-P/CDF-f-2", test_cdf_pdf_f_unif_2))
||  (NULL == CU_add_test(pSint,"Unif-P/CDF-f-3", test_cdf_pdf_f_unif_3))
||  (NULL == CU_add_test(pSint,"Unif-P/CDF-f-4", test_cdf_pdf_f_unif_4))
||  (NULL == CU_add_test(pSint,"Unif-P/CDF-f-5", test_cdf_pdf_f_unif_5))
||  (NULL == CU_add_test(pSint,"Unif-Ran-f-1", test_unif_f_1))
||  (NULL == CU_add_test(pSint,"Unif-Ran-f-2", test_unif_f_2))
||  (NULL == CU_add_test(pSint,"Unif-Ran-f-3", test_unif_f_3))
||  (NULL == CU_add_test(pSint,"Unif-Ran-f-4", test_unif_f_4))
||  (NULL == CU_add_test(pSint,"Piece-P/CDF-d-0",test_cdf_pdf_d_piece_0))
||  (NULL == CU_add_test(pSint,"Piece-P/CDF-d-1",test_cdf_pdf_d_piece_1))
||  (NULL == CU_add_test(pSint,"Piece-P/CDF-d-2",test_cdf_pdf_d_piece_2))
||  (NULL == CU_add_test(pSint,"Piece-P/CDF-d-3",test_cdf_pdf_d_piece_3))
||  (NULL == CU_add_test(pSint,"Piece-Ran-d-1", test_piece_d_1))
||  (NULL == CU_add_test(pSint,"Piece-Ran-d-2", test_piece_d_2))
||  (NULL == CU_add_test(pSint,"Piece-Ran-d-3", test_piece_d_3))
||  (NULL == CU_add_test(pSint,"Piece-Ran-d-4", test_piece_d_4))
||  (NULL == CU_add_test(pSint,"Piece-P/CDF-f-0",test_cdf_pdf_f_piece_0))
||  (NULL == CU_add_test(pSint,"Piece-P/CDF-f-1",test_cdf_pdf_f_piece_1))
||  (NULL == CU_add_test(pSint,"Piece-P/CDF-f-2",test_cdf_pdf_f_piece_2))
||  (NULL == CU_add_test(pSint,"Piece-P/CDF-f-3",test_cdf_pdf_f_piece_3))
||  (NULL == CU_add_test(pSint,"Piece-Ran-f-1", test_piece_f_1))
||  (NULL == CU_add_test(pSint,"Piece-Ran-f-2", test_piece_f_2))
||  (NULL == CU_add_test(pSint,"Piece-Ran-f-3", test_piece_f_3))
||  (NULL == CU_add_test(pSint,"Piece-Ran-f-4", test_piece_f_4))
||  (NULL == CU_add_test(pSint,"Piece-Bad-d-0", test_bad_d_piece_0))
||  (NULL == CU_add_test(pSint,"Piece-Bad-f-0", test_bad_f_piece_0))
||  (NULL == CU_add_test(pSint,"Gsn2-P/CDF-d-0", test_cdf_pdf_d_gsn2_0))
||  (NULL == CU_add_test(pSint,"Gsn2-P/CDF-d-1", test_cdf_pdf_d_gsn2_1))
||  (NULL == CU_add_test(pSint,"Gsn2-P/CDF-d-2", test_cdf_pdf_d_gsn2_2))
||  (NULL == CU_add_test(pSint,"Gsn2-P/CDF-d-3", test_cdf_pdf_d_gsn2_3))
||  (NULL == CU_add_test(pSint,"Gsn2-P/CDF-d-4", test_cdf_pdf_d_gsn2_4))
||  (NULL == CU_add_test(pSint,"Gsn2-P/CDF-d-5", test_cdf_pdf_d_gsn2_5))
||  (NULL == CU_add_test(pSint,"Gsn2-Sym-d-6", test_sym_d_gsn2_6))
||  (NULL == CU_add_test(pSint,"Gsn2-Sym-d-7", test_sym_d_gsn2_7))
||  (NULL == CU_add_test(pSint,"Gsn2-Sym-d-8", test_sym_d_gsn2_8))
||  (NULL == CU_add_test(pSint,"Gsn2-Ran-d-1", test_gsn2_d_1))
||  (NULL == CU_add_test(pSint,"Gsn2-Ran-d-2", test_gsn2_d_2))
||  (NULL == CU_add_test(pSint,"Gsn2-Ran-d-3", test_gsn2_d_3))
||  (NULL == CU_add_test(pSint,"Gsn2-Ran-d-4", test_gsn2_d_4))
||  (NULL == CU_add_test(pSint,"Gsn2-P/CDF-f-0", test_cdf_pdf_f_gsn2_0))
||  (NULL == CU_add_test(pSint,"Gsn2-P/CDF-f-1", test_cdf_pdf_f_gsn2_1))
||  (NULL == CU_add_test(pSint,"Gsn2-P/CDF-f-2", test_cdf_pdf_f_gsn2_2))
||  (NULL == CU_add_test(pSint,"Gsn2-P/CDF-f-3", test_cdf_pdf_f_gsn2_3))
||  (NULL == CU_add_test(pSint,"Gsn2-P/CDF-f-4", test_cdf_pdf_f_gsn2_4))
||  (NULL == CU_add_test(pSint,"Gsn2-P/CDF-f-5", test_cdf_pdf_f_gsn2_5))
||  (NULL == CU_add_test(pSint,"Gsn2-Sym-f-6", test_sym_f_gsn2_6))
||  (NULL == CU_add_test(pSint,"Gsn2-Sym-f-7", test_sym_f_gsn2_7))
||  (NULL == CU_add_test(pSint,"Gsn2-Sym-f-8", test_sym_f_gsn2_8))
||  (NULL == CU_add_test(pSint,"Gsn2-Ran-f-1", test_gsn2_f_1))
||  (NULL == CU_add_test(pSint,"Gsn2-Ran-f-2", test_gsn2_f_2))
||  (NULL == CU_add_test(pSint,"Gsn2-Ran-f-3", test_gsn2_f_3))
||  (NULL == CU_add_test(pSint,"Gsn2-Ran-f-4", test_gsn2_f_4))
||  (NULL == CU_add_test(pSint,"Gsn4-P/CDF-d-0", test_cdf_pdf_d_gsn4_0))
||  (NULL == CU_add_test(pSint,"Gsn4-P/CDF-d-1", test_cdf_pdf_d_gsn4_1))
||  (NULL == CU_add_test(pSint,"Gsn4-P/CDF-d-2", test_cdf_pdf_d_gsn4_2))
||  (NULL == CU_add_test(pSint,"Gsn4-P/CDF-d-3", test_cdf_pdf_d_gsn4_3))
||  (NULL == CU_add_test(pSint,"Gsn4-P/CDF-d-4", test_cdf_pdf_d_gsn4_4))
||  (NULL == CU_add_test(pSint,"Gsn4-P/CDF-d-5", test_cdf_pdf_d_gsn4_5))
||  (NULL == CU_add_test(pSint,"Gsn4-Sym-d-6", test_sym_d_gsn4_6))
||  (NULL == CU_add_test(pSint,"Gsn4-Sym-d-7", test_sym_d_gsn4_7))
||  (NULL == CU_add_test(pSint,"Gsn4-Sym-d-8", test_sym_d_gsn4_8))
||  (NULL == CU_add_test(pSint,"Gsn4-Sym-d-9", test_sym_d_gsn4_9))
||  (NULL == CU_add_test(pSint,"Gsn4-Sym-d-10", test_sym_d_gsn4_10))
||  (NULL == CU_add_test(pSint,"Gsn4-Sym-d-11", test_sym_d_gsn4_11))
||  (NULL == CU_add_test(pSint,"Gsn4-Ran-d-1", test_gsn4_d_1))
||  (NULL == CU_add_test(pSint,"Gsn4-Ran-d-2", test_gsn4_d_2))
||  (NULL == CU_add_test(pSint,"Gsn4-Ran-d-3", test_gsn4_d_3))
||  (NULL == CU_add_test(pSint,"Gsn4-Ran-d-4", test_gsn4_d_4))
||  (NULL == CU_add_test(pSint,"Gsn4-P/CDF-f-0", test_cdf_pdf_f_gsn4_0))
||  (NULL == CU_add_test(pSint,"Gsn4-P/CDF-f-1", test_cdf_pdf_f_gsn4_1))
||  (NULL == CU_add_test(pSint,"Gsn4-P/CDF-f-2", test_cdf_pdf_f_gsn4_2))
||  (NULL == CU_add_test(pSint,"Gsn4-P/CDF-f-3", test_cdf_pdf_f_gsn4_3))
||  (NULL == CU_add_test(pSint,"Gsn4-P/CDF-f-4", test_cdf_pdf_f_gsn4_4))
||  (NULL == CU_add_test(pSint,"Gsn4-P/CDF-f-5", test_cdf_pdf_f_gsn4_5))
||  (NULL == CU_add_test(pSint,"Gsn4-Sym-f-6", test_sym_f_gsn4_6))
||  (NULL == CU_add_test(pSint,"Gsn4-Sym-f-7", test_sym_f_gsn4_7))
||  (NULL == CU_add_test(pSint,"Gsn4-Sym-f-8", test_sym_f_gsn4_8))
||  (NULL == CU_add_test(pSint,"Gsn4-Sym-f-9", test_sym_f_gsn4_9))
||  (NULL == CU_add_test(pSint,"Gsn4-Sym-f-10", test_sym_f_gsn4_10))
||  (NULL == CU_add_test(pSint,"Gsn4-Sym-f-11", test_sym_f_gsn4_11))
||  (NULL == CU_add_test(pSint,"Gsn4-Ran-f-1", test_gsn4_f_1))
||  (NULL == CU_add_test(pSint,"Gsn4-Ran-f-2", test_gsn4_f_2))
||  (NULL == CU_add_test(pSint,"Gsn4-Ran-f-3", test_gsn4_f_3))
||  (NULL == CU_add_test(pSint,"Gsn4-Ran-f-4", test_gsn4_f_4))
||  (NULL == CU_add_test(pSint,"Gsn12-P/CDF-f-0", test_cdf_pdf_f_gsn12_0))
||  (NULL == CU_add_test(pSint,"Gsn12-P/CDF-f-1", test_cdf_pdf_f_gsn12_1))
||  (NULL == CU_add_test(pSint,"Gsn12-P/CDF-f-2", test_cdf_pdf_f_gsn12_2))
||  (NULL == CU_add_test(pSint,"Gsn12-P/CDF-f-3", test_cdf_pdf_f_gsn12_3))
||  (NULL == CU_add_test(pSint,"Gsn12-P/CDF-f-4", test_cdf_pdf_f_gsn12_4))
||  (NULL == CU_add_test(pSint,"Gsn12-P/CDF-f-5", test_cdf_pdf_f_gsn12_5))
||  (NULL == CU_add_test(pSint,"Gsn12-Sym-f-6", test_sym_f_gsn12_6))
||  (NULL == CU_add_test(pSint,"Gsn12-Sym-f-7", test_sym_f_gsn12_7))
||  (NULL == CU_add_test(pSint,"Gsn12-Sym-f-8", test_sym_f_gsn12_8))
||  (NULL == CU_add_test(pSint,"Gsn12-Sym-f-9", test_sym_f_gsn12_9))
||  (NULL == CU_add_test(pSint,"Gsn12-Sym-f-10", test_sym_f_gsn12_10))
||  (NULL == CU_add_test(pSint,"Gsn12-Sym-f-11", test_sym_f_gsn12_11))
||  (NULL == CU_add_test(pSint,"Gsn12-Sym-f-12", test_sym_f_gsn12_12))
||  (NULL == CU_add_test(pSint,"Gsn12-Sym-f-13", test_sym_f_gsn12_13))
||  (NULL == CU_add_test(pSint,"Gsn12-Sym-f-14", test_sym_f_gsn12_14))
||  (NULL == CU_add_test(pSint,"Gsn12-Ran-f-1", test_gsn12_f_1))
||  (NULL == CU_add_test(pSint,"Gsn12-Ran-f-2", test_gsn12_f_2))
||  (NULL == CU_add_test(pSint,"Gsn12-Ran-f-3", test_gsn12_f_3))
||  (NULL == CU_add_test(pSint,"Gsn12-Ran-f-4", test_gsn12_f_4))
||  (NULL == CU_add_test(pSint,"Lspline-P/CDF-d-0",test_cdf_pdf_d_lspline_0))
||  (NULL == CU_add_test(pSint,"Lspline-P/CDF-d-1",test_cdf_pdf_d_lspline_1))
||  (NULL == CU_add_test(pSint,"Lspline-P/CDF-d-2",test_cdf_pdf_d_lspline_2))
||  (NULL == CU_add_test(pSint,"Lspline-P/CDF-d-3",test_cdf_pdf_d_lspline_3))
||  (NULL == CU_add_test(pSint,"Lspline-Ran-d-1", test_lspline_d_1))
||  (NULL == CU_add_test(pSint,"Lspline-Ran-d-2", test_lspline_d_2))
||  (NULL == CU_add_test(pSint,"Lspline-Ran-d-3", test_lspline_d_3))
||  (NULL == CU_add_test(pSint,"Lspline-Ran-d-4", test_lspline_d_4))
||  (NULL == CU_add_test(pSint,"Lspline-P/CDF-f-0",test_cdf_pdf_f_lspline_0))
||  (NULL == CU_add_test(pSint,"Lspline-P/CDF-f-1",test_cdf_pdf_f_lspline_1))
||  (NULL == CU_add_test(pSint,"Lspline-P/CDF-f-2",test_cdf_pdf_f_lspline_2))
||  (NULL == CU_add_test(pSint,"Lspline-P/CDF-f-3",test_cdf_pdf_f_lspline_3))
||  (NULL == CU_add_test(pSint,"Lspline-Ran-f-1", test_lspline_f_1))
||  (NULL == CU_add_test(pSint,"Lspline-Ran-f-2", test_lspline_f_2))
||  (NULL == CU_add_test(pSint,"Lspline-Ran-f-3", test_lspline_f_3))
||  (NULL == CU_add_test(pSint,"Lspline-Ran-f-4", test_lspline_f_4))
||  (NULL == CU_add_test(pSint,"Lspline-Bad-d-0", test_bad_d_lspline_0))
||  (NULL == CU_add_test(pSint,"Lspline-Bad-f-0", test_bad_f_lspline_0))
||  (NULL == CU_add_test(pSint,"Uinvcdf-P/CDF-d-0",test_cdf_pdf_d_uinvcdf_0))
||  (NULL == CU_add_test(pSint,"Uinvcdf-P/CDF-d-1",test_cdf_pdf_d_uinvcdf_1))
||  (NULL == CU_add_test(pSint,"Uinvcdf-Sym-d-2", test_sym_d_uinvcdf_2))
||  (NULL == CU_add_test(pSint,"Uinvcdf-Sym-d-3", test_sym_d_uinvcdf_3))
||  (NULL == CU_add_test(pSint,"Uinvcdf-Sym-d-4", test_sym_d_uinvcdf_4))
||  (NULL == CU_add_test(pSint,"Uinvcdf-Sym-d-5", test_sym_d_uinvcdf_5))
||  (NULL == CU_add_test(pSint,"Uinvcdf-Sym-d-6", test_sym_d_uinvcdf_6))
||  (NULL == CU_add_test(pSint,"Uinvcdf-Ran-d-7", test_uinvcdf_d_7))
||  (NULL == CU_add_test(pSint,"Uinvcdf-Ran-d-8", test_uinvcdf_d_8))
||  (NULL == CU_add_test(pSint,"Uinvcdf-P/CDF-f-0",test_cdf_pdf_f_uinvcdf_0))
||  (NULL == CU_add_test(pSint,"Uinvcdf-P/CDF-f-1",test_cdf_pdf_f_uinvcdf_1))
||  (NULL == CU_add_test(pSint,"Uinvcdf-Sym-f-2", test_sym_f_uinvcdf_2))
||  (NULL == CU_add_test(pSint,"Uinvcdf-Sym-f-3", test_sym_f_uinvcdf_3))
||  (NULL == CU_add_test(pSint,"Uinvcdf-Sym-f-4", test_sym_f_uinvcdf_4))
||  (NULL == CU_add_test(pSint,"Uinvcdf-Sym-f-5", test_sym_f_uinvcdf_5))
||  (NULL == CU_add_test(pSint,"Uinvcdf-Sym-f-6", test_sym_f_uinvcdf_6))
||  (NULL == CU_add_test(pSint,"Uinvcdf-Ran-f-7", test_uinvcdf_f_7))
||  (NULL == CU_add_test(pSint,"Uinvcdf-Ran-f-8", test_uinvcdf_f_8))
) {
		printf("\nTest Suite interval additions failure.");
		CU_cleanup_registry();
		return CU_get_error();
	}

if ((NULL == CU_add_test(pShalf,"Nexp-P/CDF-d-0", test_cdf_pdf_d_nexp_0))
||  (NULL == CU_add_test(pShalf,"Nexp-P/CDF-d-1", test_cdf_pdf_d_nexp_1))
||  (NULL == CU_add_test(pShalf,"Nexp-P/CDF-d-2", test_cdf_pdf_d_nexp_2))
||  (NULL == CU_add_test(pShalf,"Nexp-P/CDF-d-3", test_cdf_pdf_d_nexp_3))
||  (NULL == CU_add_test(pShalf,"Nexp-P/CDF-d-4", test_cdf_pdf_d_nexp_4))
||  (NULL == CU_add_test(pShalf,"Nexp-P/CDF-d-5", test_cdf_pdf_d_nexp_5))
||  (NULL == CU_add_test(pShalf,"Nexp-Ran-d-1", test_nexp_d_1))
||  (NULL == CU_add_test(pShalf,"Nexp-Ran-d-2", test_nexp_d_2))
||  (NULL == CU_add_test(pShalf,"Nexp-Ran-d-3", test_nexp_d_3))
||  (NULL == CU_add_test(pShalf,"Nexp-P/CDF-f-0", test_cdf_pdf_f_nexp_0))
||  (NULL == CU_add_test(pShalf,"Nexp-P/CDF-f-1", test_cdf_pdf_f_nexp_1))
||  (NULL == CU_add_test(pShalf,"Nexp-P/CDF-f-2", test_cdf_pdf_f_nexp_2))
||  (NULL == CU_add_test(pShalf,"Nexp-P/CDF-f-3", test_cdf_pdf_f_nexp_3))
||  (NULL == CU_add_test(pShalf,"Nexp-P/CDF-f-4", test_cdf_pdf_f_nexp_4))
||  (NULL == CU_add_test(pShalf,"Nexp-P/CDF-f-5", test_cdf_pdf_f_nexp_5))
||  (NULL == CU_add_test(pShalf,"Nexp-Ran-f-1", test_nexp_f_1))
||  (NULL == CU_add_test(pShalf,"Nexp-Ran-f-2", test_nexp_f_2))
||  (NULL == CU_add_test(pShalf,"Nexp-Ran-f-3", test_nexp_f_3))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-P/CDF-d-0", test_cdf_pdf_d_erlang_0))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-P/CDF-d-1", test_cdf_pdf_d_erlang_1))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-P/CDF-d-2", test_cdf_pdf_d_erlang_2))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-P/CDF-d-3", test_cdf_pdf_d_erlang_3))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-P/CDF-d-4", test_cdf_pdf_d_erlang_4))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-P/CDF-d-5", test_cdf_pdf_d_erlang_5))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-Ran-d-1", test_erlang_d_1))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-Ran-d-2", test_erlang_d_2))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-Ran-d-3", test_erlang_d_3))
||  (NULL == CU_add_test(pShalf,"Erlang2-P/CDF-d-0", test_cdf_pdf_d_erlang2_0))
||  (NULL == CU_add_test(pShalf,"Erlang2-P/CDF-d-1", test_cdf_pdf_d_erlang2_1))
||  (NULL == CU_add_test(pShalf,"Erlang2-P/CDF-d-2", test_cdf_pdf_d_erlang2_2))
||  (NULL == CU_add_test(pShalf,"Erlang2-Ran-d-4", test_erlang_d_4))
||  (NULL == CU_add_test(pShalf,"Erlang2-Ran-d-5", test_erlang_d_5))
||  (NULL == CU_add_test(pShalf,"Erlang2-Ran-d-6", test_erlang_d_6))
||  (NULL == CU_add_test(pShalf,"Erlang3-P/CDF-d-0", test_cdf_pdf_d_erlang3_0))
||  (NULL == CU_add_test(pShalf,"Erlang3-P/CDF-d-1", test_cdf_pdf_d_erlang3_1))
||  (NULL == CU_add_test(pShalf,"Erlang3-P/CDF-d-2", test_cdf_pdf_d_erlang3_2))
||  (NULL == CU_add_test(pShalf,"Erlang3-Ran-d-7", test_erlang_d_7))
||  (NULL == CU_add_test(pShalf,"Erlang3-Ran-d-8", test_erlang_d_8))
||  (NULL == CU_add_test(pShalf,"Erlang3-Ran-d-9", test_erlang_d_9))
||  (NULL == CU_add_test(pShalf,"Erlang4-P/CDF-d-0", test_cdf_pdf_d_erlang4_0))
||  (NULL == CU_add_test(pShalf,"Erlang4-P/CDF-d-1", test_cdf_pdf_d_erlang4_1))
||  (NULL == CU_add_test(pShalf,"Erlang4-P/CDF-d-2", test_cdf_pdf_d_erlang4_2))
||  (NULL == CU_add_test(pShalf,"Erlang4-Ran-d-10", test_erlang_d_10))
||  (NULL == CU_add_test(pShalf,"Erlang4-Ran-d-11", test_erlang_d_11))
||  (NULL == CU_add_test(pShalf,"Erlang4-Ran-d-12", test_erlang_d_12))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-P/CDF-f-0", test_cdf_pdf_f_erlang_0))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-P/CDF-f-1", test_cdf_pdf_f_erlang_1))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-P/CDF-f-2", test_cdf_pdf_f_erlang_2))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-P/CDF-f-3", test_cdf_pdf_f_erlang_3))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-P/CDF-f-4", test_cdf_pdf_f_erlang_4))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-P/CDF-f-5", test_cdf_pdf_f_erlang_5))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-Ran-f-1", test_erlang_f_1))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-Ran-f-2", test_erlang_f_2))
||  (NULL == CU_add_test(pShalf,"Erlang/Nexp-Ran-f-3", test_erlang_f_3))
||  (NULL == CU_add_test(pShalf,"Erlang2-P/CDF-f-0", test_cdf_pdf_f_erlang2_0))
||  (NULL == CU_add_test(pShalf,"Erlang2-P/CDF-f-1", test_cdf_pdf_f_erlang2_1))
||  (NULL == CU_add_test(pShalf,"Erlang2-P/CDF-f-2", test_cdf_pdf_f_erlang2_2))
||  (NULL == CU_add_test(pShalf,"Erlang2-Ran-f-4", test_erlang_f_4))
||  (NULL == CU_add_test(pShalf,"Erlang2-Ran-f-5", test_erlang_f_5))
||  (NULL == CU_add_test(pShalf,"Erlang2-Ran-f-6", test_erlang_f_6))
||  (NULL == CU_add_test(pShalf,"Erlang3-P/CDF-f-0", test_cdf_pdf_f_erlang3_0))
||  (NULL == CU_add_test(pShalf,"Erlang3-P/CDF-f-1", test_cdf_pdf_f_erlang3_1))
||  (NULL == CU_add_test(pShalf,"Erlang3-P/CDF-f-2", test_cdf_pdf_f_erlang3_2))
||  (NULL == CU_add_test(pShalf,"Erlang3-Ran-f-7", test_erlang_f_7))
||  (NULL == CU_add_test(pShalf,"Erlang3-Ran-f-8", test_erlang_f_8))
||  (NULL == CU_add_test(pShalf,"Erlang3-Ran-f-9", test_erlang_f_9))
||  (NULL == CU_add_test(pShalf,"Erlang4-P/CDF-f-0", test_cdf_pdf_f_erlang4_0))
||  (NULL == CU_add_test(pShalf,"Erlang4-P/CDF-f-1", test_cdf_pdf_f_erlang4_1))
||  (NULL == CU_add_test(pShalf,"Erlang4-P/CDF-f-2", test_cdf_pdf_f_erlang4_2))
||  (NULL == CU_add_test(pShalf,"Erlang4-Ran-f-10", test_erlang_f_10))
||  (NULL == CU_add_test(pShalf,"Erlang4-Ran-f-11", test_erlang_f_11))
||  (NULL == CU_add_test(pShalf,"Erlang4-Ran-f-12", test_erlang_f_12))
) {
		printf("\nTest Suite interval additions failure.");
		CU_cleanup_registry();
		return CU_get_error();
	}

if ((NULL == CU_add_test(pSfull,"Gausbm-P/CDF-d-0", test_cdf_pdf_d_gausbm_0))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-d-1", test_cdf_pdf_d_gausbm_1))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-d-2", test_cdf_pdf_d_gausbm_2))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-d-3", test_cdf_pdf_d_gausbm_3))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-d-4", test_cdf_pdf_d_gausbm_4))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-d-5", test_cdf_pdf_d_gausbm_5))
||  (NULL == CU_add_test(pSfull,"Gausbm-Sym-d-6", test_sym_d_gausbm_6))
||  (NULL == CU_add_test(pSfull,"Gausbm-Sym-d-7", test_sym_d_gausbm_7))
||  (NULL == CU_add_test(pSfull,"Gausbm-Sym-d-8", test_sym_d_gausbm_8))
||  (NULL == CU_add_test(pSfull,"Gausbm-Sym-d-9", test_sym_d_gausbm_9))
||  (NULL == CU_add_test(pSfull,"Gausbm-Sym-d-10", test_sym_d_gausbm_10))
||  (NULL == CU_add_test(pSfull,"Gausbm-Sym-d-11", test_sym_d_gausbm_11))
||  (NULL == CU_add_test(pSfull,"Gausbm-Ran-d-1", test_gausbm_d_1))
||  (NULL == CU_add_test(pSfull,"Gausbm-Ran-d-2", test_gausbm_d_2))
||  (NULL == CU_add_test(pSfull,"Gausbm-Ran-d-3", test_gausbm_d_3))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-f-0", test_cdf_pdf_f_gausbm_0))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-f-1", test_cdf_pdf_f_gausbm_1))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-f-2", test_cdf_pdf_f_gausbm_2))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-f-3", test_cdf_pdf_f_gausbm_3))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-f-4", test_cdf_pdf_f_gausbm_4))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-f-5", test_cdf_pdf_f_gausbm_5))
||  (NULL == CU_add_test(pSfull,"Gausbm-Sym-f-6", test_sym_f_gausbm_6))
||  (NULL == CU_add_test(pSfull,"Gausbm-Sym-f-7", test_sym_f_gausbm_7))
||  (NULL == CU_add_test(pSfull,"Gausbm-Sym-f-8", test_sym_f_gausbm_8))
||  (NULL == CU_add_test(pSfull,"Gausbm-Sym-f-9", test_sym_f_gausbm_9))
||  (NULL == CU_add_test(pSfull,"Gausbm-Sym-f-10", test_sym_f_gausbm_10))
||  (NULL == CU_add_test(pSfull,"Gausbm-Sym-f-11", test_sym_f_gausbm_11))
||  (NULL == CU_add_test(pSfull,"Gausbm-Ran-f-1", test_gausbm_f_1))
||  (NULL == CU_add_test(pSfull,"Gausbm-Ran-f-2", test_gausbm_f_2))
||  (NULL == CU_add_test(pSfull,"Gausbm-Ran-f-3", test_gausbm_f_3))
||  (NULL == CU_add_test(pSfull,"Gausmar-P/CDF-d-0", test_cdf_pdf_d_gausmar_0))
||  (NULL == CU_add_test(pSfull,"Gausmar-P/CDF-d-1", test_cdf_pdf_d_gausmar_1))
||  (NULL == CU_add_test(pSfull,"Gausmar-Ran-d-1", test_gausmar_d_1))
||  (NULL == CU_add_test(pSfull,"Gausmar-Ran-d-2", test_gausmar_d_2))
||  (NULL == CU_add_test(pSfull,"Gausmar-Ran-d-3", test_gausmar_d_3))
||  (NULL == CU_add_test(pSfull,"Gausmar-P/CDF-f-0", test_cdf_pdf_f_gausmar_0))
||  (NULL == CU_add_test(pSfull,"Gausmar-P/CDF-f-1", test_cdf_pdf_f_gausmar_1))
||  (NULL == CU_add_test(pSfull,"Gausmar-Ran-f-1", test_gausmar_f_1))
||  (NULL == CU_add_test(pSfull,"Gausmar-Ran-f-2", test_gausmar_f_2))
||  (NULL == CU_add_test(pSfull,"Gausmar-Ran-f-3", test_gausmar_f_3))
||  (NULL == CU_add_test(pSfull,"Cauchy-P/CDF-d-0", test_cdf_pdf_d_cauchy_0))
||  (NULL == CU_add_test(pSfull,"Cauchy-P/CDF-d-1", test_cdf_pdf_d_cauchy_1))
||  (NULL == CU_add_test(pSfull,"Cauchy-P/CDF-d-2", test_cdf_pdf_d_cauchy_2))
||  (NULL == CU_add_test(pSfull,"Cauchy-P/CDF-d-3", test_cdf_pdf_d_cauchy_3))
||  (NULL == CU_add_test(pSfull,"Cauchy-P/CDF-d-4", test_cdf_pdf_d_cauchy_4))
||  (NULL == CU_add_test(pSfull,"Cauchy-P/CDF-d-5", test_cdf_pdf_d_cauchy_5))
||  (NULL == CU_add_test(pSfull,"Cauchy-Sym-d-6", test_sym_d_cauchy_6))
||  (NULL == CU_add_test(pSfull,"Cauchy-Sym-d-7", test_sym_d_cauchy_7))
||  (NULL == CU_add_test(pSfull,"Cauchy-Sym-d-8", test_sym_d_cauchy_8))
||  (NULL == CU_add_test(pSfull,"Cauchy-Sym-d-9", test_sym_d_cauchy_9))
||  (NULL == CU_add_test(pSfull,"Cauchy-Sym-d-10", test_sym_d_cauchy_10))
||  (NULL == CU_add_test(pSfull,"Cauchy-Sym-d-11", test_sym_d_cauchy_11))
||  (NULL == CU_add_test(pSfull,"Cauchy-Ran-d-1", test_cauchy_d_1))
||  (NULL == CU_add_test(pSfull,"Cauchy-Ran-d-2", test_cauchy_d_2))
||  (NULL == CU_add_test(pSfull,"Cauchy-Ran-d-3", test_cauchy_d_3))
||  (NULL == CU_add_test(pSfull,"Cauchy-P/CDF-f-0", test_cdf_pdf_f_cauchy_0))
||  (NULL == CU_add_test(pSfull,"Cauchy-P/CDF-f-1", test_cdf_pdf_f_cauchy_1))
||  (NULL == CU_add_test(pSfull,"Cauchy-P/CDF-f-2", test_cdf_pdf_f_cauchy_2))
||  (NULL == CU_add_test(pSfull,"Cauchy-P/CDF-f-3", test_cdf_pdf_f_cauchy_3))
||  (NULL == CU_add_test(pSfull,"Cauchy-P/CDF-f-4", test_cdf_pdf_f_cauchy_4))
||  (NULL == CU_add_test(pSfull,"Cauchy-P/CDF-f-5", test_cdf_pdf_f_cauchy_5))
||  (NULL == CU_add_test(pSfull,"Cauchy-Sym-f-6", test_sym_f_cauchy_6))
||  (NULL == CU_add_test(pSfull,"Cauchy-Sym-f-7", test_sym_f_cauchy_7))
||  (NULL == CU_add_test(pSfull,"Cauchy-Sym-f-8", test_sym_f_cauchy_8))
||  (NULL == CU_add_test(pSfull,"Cauchy-Sym-f-9", test_sym_f_cauchy_9))
||  (NULL == CU_add_test(pSfull,"Cauchy-Sym-f-10", test_sym_f_cauchy_10))
||  (NULL == CU_add_test(pSfull,"Cauchy-Sym-f-11", test_sym_f_cauchy_11))
||  (NULL == CU_add_test(pSfull,"Cauchy-Ran-f-1", test_cauchy_f_1))
||  (NULL == CU_add_test(pSfull,"Cauchy-Ran-f-2", test_cauchy_f_2))
||  (NULL == CU_add_test(pSfull,"Cauchy-Ran-f-3", test_cauchy_f_3))
||  (NULL == CU_add_test(pSfull,"Cauchymar-P/CDF-d-0", test_cdf_pdf_d_cauchymar_0))
||  (NULL == CU_add_test(pSfull,"Cauchymar-P/CDF-d-1", test_cdf_pdf_d_cauchymar_1))
||  (NULL == CU_add_test(pSfull,"Cauchymar-Ran-d-1", test_cauchymar_d_1))
||  (NULL == CU_add_test(pSfull,"Cauchymar-Ran-d-2", test_cauchymar_d_2))
||  (NULL == CU_add_test(pSfull,"Cauchymar-Ran-d-3", test_cauchymar_d_3))
||  (NULL == CU_add_test(pSfull,"Cauchymar-P/CDF-f-0", test_cdf_pdf_f_cauchymar_0))
||  (NULL == CU_add_test(pSfull,"Cauchymar-P/CDF-f-1", test_cdf_pdf_f_cauchymar_1))
||  (NULL == CU_add_test(pSfull,"Cauchymar-Ran-f-1", test_cauchymar_f_1))
||  (NULL == CU_add_test(pSfull,"Cauchymar-Ran-f-2", test_cauchymar_f_2))
||  (NULL == CU_add_test(pSfull,"Cauchymar-Ran-f-3", test_cauchymar_f_3))
||  (NULL == CU_add_test(pSfull,"Cauchy/Uinvcdf-P/CDF-d-9",test_cdf_pdf_d_uinvcdf_9))
||  (NULL == CU_add_test(pSfull,"Cauchy/Uinvcdf-P/CDF-d-10",test_cdf_pdf_d_uinvcdf_10))
||  (NULL == CU_add_test(pSfull,"Cauchy/Uinvcdf-Ran-d-11", test_uinvcdf_d_11))
||  (NULL == CU_add_test(pSfull,"Cauchy/Uinvcdf-P/CDF-f-9",test_cdf_pdf_f_uinvcdf_9))
||  (NULL == CU_add_test(pSfull,"Cauchy/Uinvcdf-P/CDF-f-10",test_cdf_pdf_f_uinvcdf_10))
||  (NULL == CU_add_test(pSfull,"Cauchy/Uinvcdf-Ran-f-11", test_uinvcdf_f_11))
) {
		printf("\nTest Suite full range  additions failure.");
		CU_cleanup_registry();
		return CU_get_error();
	}
if ((NULL == CU_add_test(pSdisc,"Poisson-P/CDF-1-1", test_cdf_pdf_1_poisson_1))
||  (NULL == CU_add_test(pSdisc,"Poisson-P/CDF-2-2", test_cdf_pdf_2_poisson_2))
||  (NULL == CU_add_test(pSdisc,"Poisson-P/CDF-4-3", test_cdf_pdf_4_poisson_3))
||  (NULL == CU_add_test(pSdisc,"Poisson-P/CDF-8-4", test_cdf_pdf_8_poisson_4))
||  (NULL == CU_add_test(pSdisc,"Poisson-Ran-1-1", test_poisson_i_1))
||  (NULL == CU_add_test(pSdisc,"Poisson-Ran-2-2", test_poisson_i_2))
||  (NULL == CU_add_test(pSdisc,"Poisson-Ran-4-3", test_poisson_i_3))
||  (NULL == CU_add_test(pSdisc,"Poisson-Ran-8-4", test_poisson_i_4))
||  (NULL == CU_add_test(pSdisc,"Poisson-Ran-55-5", test_poisson_i_5))
||  (NULL == CU_add_test(pSdisc,"check - Poisson - 1",test_check_poisson_i_1))
||  (NULL == CU_add_test(pSdisc,"check - Poisson - 2",test_check_poisson_i_2))
||  (NULL == CU_add_test(pSdisc,"Geometric-P/CDF-25-1", test_cdf_pdf_25_geometric_1))
||  (NULL == CU_add_test(pSdisc,"Geometric-P/CDF-50-2", test_cdf_pdf_50_geometric_2))
||  (NULL == CU_add_test(pSdisc,"Geometric-P/CDF-75-3", test_cdf_pdf_75_geometric_3))
||  (NULL == CU_add_test(pSdisc,"Geometric-Ran-def-1", test_geometric_i_1))
||  (NULL == CU_add_test(pSdisc,"Geometric-Ran-50-2", test_geometric_i_2))
||  (NULL == CU_add_test(pSdisc,"Geometric-Ran-20-3", test_geometric_i_3))
||  (NULL == CU_add_test(pSdisc,"Geometric-Ran-35-4", test_geometric_i_4))
||  (NULL == CU_add_test(pSdisc,"Geometric-Ran-65-5", test_geometric_i_5))
||  (NULL == CU_add_test(pSdisc,"Geometric-Ran-80-6", test_geometric_i_6))
||  (NULL == CU_add_test(pSdisc,"check - Geometric - 1",test_check_geometric_i_1))
||  (NULL == CU_add_test(pSdisc,"check - Geometric - 2",test_check_geometric_i_2))
||  (NULL == CU_add_test(pSdisc,"check - Geometric - 3",test_check_geometric_i_3))
) {
		printf("\nTest Suite discrete  additions failure.");
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Run all tests using the basic interface */
	CU_basic_set_mode(mode);
	CU_set_error_action(error_action);
	if (interact) {
		CU_console_run_tests();
	} else {
		CU_basic_run_tests();
	}
	printf("\n");

	/* Clean up and return */
	CU_cleanup_registry();
	return CU_get_error();
}
