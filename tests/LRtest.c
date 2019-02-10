#include <stdio.h>
#include "libran.h"
#include "urand/urand.h"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

#define testLRnew(tt)			void test_new_##tt(void) {	\
	LR_obj *o = LR_new(gaus, LR_##tt);				\
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
	LR_obj *o = LR_new(gaus, LR_##tt);				\
	CU_ASSERT_EQUAL(LR_set(o, 'a', -1##ptt),0);			\
	CU_ASSERT_EQUAL(LR_set(o, 'b',  3##ptt),0);			\
	CU_ASSERT_EQUAL(LR_set(o, 'm',  2##ptt),0);			\
	CU_ASSERT_EQUAL(LR_set(o, 's',  1##ptt),0);			\
	CU_ASSERT_NOT_EQUAL(LR_set(o, 'x',  9##ptt),0);			\
	CU_ASSERT_EQUAL(o->a.att, -1##ptt);				\
	CU_ASSERT_EQUAL(o->b.att,  3##ptt);				\
	CU_ASSERT_EQUAL(o->m.att,  2##ptt);				\
	CU_ASSERT_EQUAL(o->s.att,  1##ptt);				\
	LR_rm(&o);							\
}

testLRset(int,,i)
testLRset(float,.,f)
testLRset(long,L,l)
testLRset(double,.,d)

/* test when all the parameters are given */
#define testLRsetall(tt, ptt, att)	void test_set_all_##tt(void) {	\
	LR_obj *o = LR_new(gaus, LR_##tt);				\
	CU_ASSERT_EQUAL(						\
	  LR_set_all(o,"abms",-1##ptt,3##ptt,2##ptt,1##ptt),0);		\
	CU_ASSERT_EQUAL(o->a.att, -1##ptt);				\
	CU_ASSERT_EQUAL(o->b.att,  3##ptt);				\
	CU_ASSERT_EQUAL(o->m.att,  2##ptt);				\
	CU_ASSERT_EQUAL(o->s.att,  1##ptt);				\
	LR_rm(&o);							\
}

testLRsetall(int,,i)
testLRsetall(float,.,f)
testLRsetall(long,L,l)
testLRsetall(double,.,d)

/* test a different order of parameters */
#define testLRsetall2(tt, ptt, att)	void test_set_all2_##tt(void) {	\
	LR_obj *o = LR_new(gaus, LR_##tt);				\
	CU_ASSERT_EQUAL(						\
	  LR_set_all(o,"masb",2##ptt,-1##ptt,1##ptt,3##ptt),0);		\
	CU_ASSERT_EQUAL(o->a.att, -1##ptt);				\
	CU_ASSERT_EQUAL(o->b.att,  3##ptt);				\
	CU_ASSERT_EQUAL(o->m.att,  2##ptt);				\
	CU_ASSERT_EQUAL(o->s.att,  1##ptt);				\
	LR_rm(&o);							\
}

testLRsetall2(int,,i)
testLRsetall2(float,.,f)
testLRsetall2(long,L,l)
testLRsetall2(double,.,d)

/* test when extra parameters are given */
#define testLRsetall3(tt, ptt, att)	void test_set_all3_##tt(void) {	\
	LR_obj *o = LR_new(gaus, LR_##tt);				\
	CU_ASSERT_NOT_EQUAL(						\
	  LR_set_all(o,"maxsby",2##ptt,-1##ptt,33##ptt,1##ptt,3##ptt,9##ptt),0);	\
	CU_ASSERT_EQUAL(o->a.att, -1##ptt);				\
	CU_ASSERT_EQUAL(o->b.att,  3##ptt);				\
	CU_ASSERT_EQUAL(o->m.att,  2##ptt);				\
	CU_ASSERT_EQUAL(o->s.att,  1##ptt);				\
	LR_rm(&o);							\
}

testLRsetall3(int,,i)
testLRsetall3(float,.,f)
testLRsetall3(long,L,l)
testLRsetall3(double,.,d)

/* binning object */
#define testLRbinnew(tt)		void test_bin_new_##tt(void) {	\
	LR_bin *b = LR_bin_new(LR_##tt,10);				\
	CU_ASSERT_PTR_NOT_NULL(b);					\
	CU_ASSERT_EQUAL(b->d, LR_##tt);					\
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

testLRbinnew(int)
testLRbinnew(long)
testLRbinnew(float)
testLRbinnew(double)

#define testLRbinset(nn,aa,bb,cc,dd)	void test_bin_set_##nn(void) {	\
	LR_bin *b = LR_bin_new(LR_double,5);				\
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

int main(int argc, char* argv[]) {
	CU_pSuite		pS		= NULL;
	CU_BasicRunMode		mode		= CU_BRM_VERBOSE;
	CU_ErrorAction		error_action	= CUEA_IGNORE;
	int i;

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
	/* add tests to the suite */
	if ((NULL == CU_add_test(pS,"new - int", test_new_int))
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
	||  (NULL == CU_add_test(pS,"new_bin - int", test_bin_new_int))
	||  (NULL == CU_add_test(pS,"new_bin - float", test_bin_new_float))
	||  (NULL == CU_add_test(pS,"new_bin - long", test_bin_new_long))
	||  (NULL == CU_add_test(pS,"new_bin - double", test_bin_new_double))
	||  (NULL == CU_add_test(pS,"new_bin_set - 1", test_bin_set_1))
	||  (NULL == CU_add_test(pS,"new_bin_set - 2", test_bin_set_2))
	||  (NULL == CU_add_test(pS,"new_bin_set - 3", test_bin_set_3))
	||  (NULL == CU_add_test(pS,"new_bin_set - 4", test_bin_set_4))
	) {
		printf("\nTest Suite additions failure.");
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Run all tests using the basic interface */
	CU_basic_set_mode(mode);
	CU_set_error_action(error_action);
	CU_basic_run_tests();
	printf("\n");

	/* Clean up and return */
	CU_cleanup_registry();
	return CU_get_error();
}
