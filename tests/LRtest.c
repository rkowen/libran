#include <stdio.h>
#include <math.h>
#include "libran.h"
#include "urand/urand.h"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>

#define max(a,b) (a > b ? a : b)

#undef LR_DEBUG

#ifdef LR_DEBUG
#  define CX_ASSERT_DOUBLE_EQUAL(val,exp,tol)			\
{	printf("value, expect = % 9.5f,% 9.5f\n",val,exp);	\
	CU_ASSERT_DOUBLE_EQUAL(val,exp,tol); }
#else
#  define CX_ASSERT_DOUBLE_EQUAL(val,exp,tol)			\
	CU_ASSERT_DOUBLE_EQUAL(val,exp,tol);
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
	CU_ASSERT_NOT_EQUAL(LR_set(o, 'z',  9##ptt),0);			\
	CU_ASSERT_EQUAL(o->a.att, -1##ptt);				\
	CU_ASSERT_EQUAL(o->b.att,  3##ptt);				\
	CU_ASSERT_EQUAL(o->m.att,  2##ptt);				\
	CU_ASSERT_EQUAL(o->s.att,  1##ptt);				\
	CU_ASSERT_EQUAL(o->x.att,  5##ptt);				\
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
	  LR_set_all(o,"abmsx",-1##ptt,3##ptt,2##ptt,1##ptt,5##ptt),0);	\
	CU_ASSERT_EQUAL(o->a.att, -1##ptt);				\
	CU_ASSERT_EQUAL(o->b.att,  3##ptt);				\
	CU_ASSERT_EQUAL(o->m.att,  2##ptt);				\
	CU_ASSERT_EQUAL(o->s.att,  1##ptt);				\
	CU_ASSERT_EQUAL(o->x.att,  5##ptt);				\
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
	  LR_set_all(o,"maxsb",2##ptt,-1##ptt,5##ptt,1##ptt,3##ptt),0);	\
	CU_ASSERT_EQUAL(o->a.att, -1##ptt);				\
	CU_ASSERT_EQUAL(o->b.att,  3##ptt);				\
	CU_ASSERT_EQUAL(o->m.att,  2##ptt);				\
	CU_ASSERT_EQUAL(o->s.att,  1##ptt);				\
	CU_ASSERT_EQUAL(o->x.att,  5##ptt);				\
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
	  LR_set_all(o,"maxzsby",2##ptt,-1##ptt,5##ptt,33##ptt,1##ptt,3##ptt,9##ptt),0);	\
	CU_ASSERT_EQUAL(o->a.att, -1##ptt);				\
	CU_ASSERT_EQUAL(o->b.att,  3##ptt);				\
	CU_ASSERT_EQUAL(o->m.att,  2##ptt);				\
	CU_ASSERT_EQUAL(o->s.att,  1##ptt);				\
	CU_ASSERT_EQUAL(o->x.att,  5##ptt);				\
	LR_rm(&o);							\
}

testLRsetall3(int,,i)
testLRsetall3(float,.,f)
testLRsetall3(long,L,l)
testLRsetall3(double,.,d)

/* binning object */
#define testLRbinnew(tt)		void test_bin_new_##tt(void) {	\
	LR_bin *b = LR_bin_new(10);					\
	CU_ASSERT_PTR_NOT_NULL(b);					\
	CU_ASSERT_EQUAL(b->n, 10);					\
	CU_ASSERT_EQUAL(b->nn, 1);					\
	CU_ASSERT_EQUAL(sizeof(b), sizeof(LR_bin *));			\
	CU_ASSERT_EQUAL(sizeof(*b), sizeof(LR_bin));			\
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
 * tol	- absolute tolerance
 * setup- LR object parameter set-up
 */
#define testCdfPdf(nn,tt,ttt,dist,nnt,tol,setup)			\
void test_cdf_pdf_ ## tt ## _ ##dist ## _##nn(void) {			\
	LR_obj *o = LR_new(dist, LR_##ttt);				\
	double	incr;							\
	setup;								\
	incr = (o->b.tt - o->a.tt)/nnt;					\
	CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->a.tt-2*incr),0.,tol)	\
	CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->a.tt-2*incr),0.,tol)	\
	for (int i = 1; i < nnt; i++) {					\
	  compCdfPdf(tt,o,o->a.tt+i*incr,o->a.tt+i*incr+.0001,.001)	\
	}								\
	CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _CDF(o,o->b.tt+2*incr),1.,tol) \
	CU_ASSERT_DOUBLE_EQUAL(LR##tt ## _PDF(o,o->b.tt+2*incr),0.,tol) \
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

/* uniform distribution */
int tirand(void);
long tlrand(void);
float tfrand(void);
double tdrand(void);

testCdfPdf0unif(0,d,double,.001,)
testCdfPdf(1,d,double,unif,20,.001,)
testCdfPdf(2,d,double,unif,50,.001,)
testCdfPdf(3,d,double,unif,90,.001,)

testCdfPdf0unif(4,d,double,.001,LR_set_all(o,"ab",-2.,2.))
testCdfPdf(5,d,double,unif,33,.001,LR_set_all(o,"ab",-1.,3.))

testCdfPdf0unif(0,f,float,.001,)
testCdfPdf(1,f,float,unif,20,.001,)
testCdfPdf(2,f,float,unif,50,.001,)
testCdfPdf(3,f,float,unif,90,.001,)

testCdfPdf0unif(4,f,float,.001,LR_set_all(o,"ab",-2.,2.))
testCdfPdf(5,f,float,unif,33,.001,LR_set_all(o,"ab",-1.,3.))

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

testCdfPdf(0,d,double,piece,75,.001,
	LR_aux_new(o,6);
	LR_set_all(o,"abx", -2., 6., 4.);
	LR_aux_set(o, o->a.d + 2.0, 1.0);
	LR_aux_set(o, o->a.d + 3.0, 3.0);
	LR_aux_set(o, o->a.d + 4.0, 0.0);
	LR_aux_set(o, o->a.d + 5.0, 5.0);
	LR_aux_set(o, o->a.d + 7.0, 2.0);
	LR_aux_norm(o);
)
testCdfPdf(1,d,double,piece,55,.001,
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

testCdfPdf(0,f,float,piece,75,.001,
	LR_aux_new(o,6);
	LR_set_all(o,"abx", -2., 6., 4.);
	LR_aux_set(o, o->a.d + 2.0, 1.0);
	LR_aux_set(o, o->a.d + 3.0, 3.0);
	LR_aux_set(o, o->a.d + 4.0, 0.0);
	LR_aux_set(o, o->a.d + 5.0, 5.0);
	LR_aux_set(o, o->a.d + 7.0, 2.0);
	LR_aux_norm(o);
)
testCdfPdf(1,f,float,piece,55,.001,
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
testCdfPdf(1,d,double,gsn2,20,.001,)
testCdfPdf(2,d,double,gsn2,50,.001,)
testCdfPdf(3,d,double,gsn2,90,.001,)

testCdfPdf0gsn2(4,d,double,.001,LR_set_all(o,"ab",-3.,1.))
testCdfPdf(5,d,double,gsn2,33,.001,LR_set_all(o,"ab",-1.,3.))

testCdfPdf0gsn2(0,f,float,.001,)
testCdfPdf(1,f,float,gsn2,20,.001,)
testCdfPdf(2,f,float,gsn2,50,.001,)
testCdfPdf(3,f,float,gsn2,90,.001,)

testCdfPdf0gsn2(4,f,float,.001,LR_set_all(o,"ab",-3.,1.))
testCdfPdf(5,f,float,gsn2,33,.001,LR_set_all(o,"ab",-1.,3.))

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

testCdfPdf(0,d,double,lspline,75,.001,
	LR_aux_new(o,6);
	LR_set_all(o,"ab", -2., 6.);
	LR_aux_set(o, o->a.d + 2.0, 1.0);
	LR_aux_set(o, o->a.d + 3.0, 3.0);
	LR_aux_set(o, o->a.d + 4.0, 0.0);
	LR_aux_set(o, o->a.d + 5.0, 5.0);
	LR_aux_set(o, o->a.d + 7.0, 2.0);
	LR_aux_norm(o);
)
testCdfPdf(1,d,double,lspline,55,.001,
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

testCdfPdf(0,f,float,lspline,75,.001,
	LR_aux_new(o,6);
	LR_set_all(o,"ab", -2., 6.);
	LR_aux_set(o, o->a.f + 2.0, 1.0);
	LR_aux_set(o, o->a.f + 3.0, 3.0);
	LR_aux_set(o, o->a.f + 4.0, 0.0);
	LR_aux_set(o, o->a.f + 5.0, 5.0);
	LR_aux_set(o, o->a.f + 7.0, 2.0);
	LR_aux_norm(o);
)
testCdfPdf(1,f,float,lspline,55,.001,
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

testCdfPdfFR(3,f,float,gausbm,3,60,.001,)
testCdfPdfFR(4,f,float,gausbm,3,60,.001,
	LR_set_all(o,"ms", -2., .75);
)
testCdfPdfFR(5,f,float,gausbm,4,80,.001,
	LR_set_all(o,"ms", 2., 2.50);
)

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

int main(int argc, char* argv[]) {
	CU_pSuite		pS		= NULL;
	CU_pSuite		pSint		= NULL;
	CU_pSuite		pSfull		= NULL;
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
	pSfull = CU_add_suite("LR_test_suite_full_range",
		init_suite, clean_suite);
	if (pSint == NULL) {
		printf("\nTest Registry Suite Full Range failure.");
		CU_cleanup_registry();
		return CU_get_error();
	}
	/* add tests to the suite */
if ((NULL == CU_add_test(pS,"errors", test_LR_errors))
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
||  (NULL == CU_add_test(pSint,"Gsn2-Ran-f-1", test_gsn2_f_1))
||  (NULL == CU_add_test(pSint,"Gsn2-Ran-f-2", test_gsn2_f_2))
||  (NULL == CU_add_test(pSint,"Gsn2-Ran-f-3", test_gsn2_f_3))
||  (NULL == CU_add_test(pSint,"Gsn2-Ran-f-4", test_gsn2_f_4))
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
) {
		printf("\nTest Suite interval additions failure.");
		CU_cleanup_registry();
		return CU_get_error();
	}

if ((NULL == CU_add_test(pSfull,"Gausbm-P/CDF-d-0", test_cdf_pdf_d_gausbm_0))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-d-1", test_cdf_pdf_d_gausbm_1))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-d-2", test_cdf_pdf_d_gausbm_2))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-d-3", test_cdf_pdf_d_gausbm_3))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-d-3", test_cdf_pdf_d_gausbm_4))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-d-3", test_cdf_pdf_d_gausbm_5))
||  (NULL == CU_add_test(pSfull,"Gausbm-Ran-d-1", test_gausbm_d_1))
||  (NULL == CU_add_test(pSfull,"Gausbm-Ran-d-1", test_gausbm_d_2))
||  (NULL == CU_add_test(pSfull,"Gausbm-Ran-d-1", test_gausbm_d_3))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-f-0", test_cdf_pdf_f_gausbm_0))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-f-1", test_cdf_pdf_f_gausbm_1))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-f-2", test_cdf_pdf_f_gausbm_2))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-f-3", test_cdf_pdf_f_gausbm_3))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-f-3", test_cdf_pdf_f_gausbm_4))
||  (NULL == CU_add_test(pSfull,"Gausbm-P/CDF-f-3", test_cdf_pdf_f_gausbm_5))
||  (NULL == CU_add_test(pSfull,"Gausbm-Ran-f-1", test_gausbm_f_1))
||  (NULL == CU_add_test(pSfull,"Gausbm-Ran-f-1", test_gausbm_f_2))
||  (NULL == CU_add_test(pSfull,"Gausbm-Ran-f-1", test_gausbm_f_3))
) {
		printf("\nTest Suite full range  additions failure.");
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
