/*
 * ut.c
 *
 *  Created on: Aug 11, 2017
 *      Author: alexey
 */

#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>

#include "utils.h"
#include "bayrepomalloc.h"
#include "utstring.h"

TEST_FUNCT(brp_utsting) {
	char testdata[10000] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init(testdata, 10000), 0);
	UT_string *s = NULL;
	utstring_new(s, (void * )testdata);
	utstring_printf(s, "Test string %d", 10);
	CU_ASSERT_PTR_NOT_NULL_FATAL(utstring_body(s));
	CU_ASSERT_STRING_EQUAL(utstring_body(s), "Test string 10");
	char *ptr = utstring_body(s);
	CU_ASSERT(
			(((char * )&testdata) < ptr)
					&& (((char * )&testdata + 10000) > ptr));
	UT_string *s2 = NULL;
	char normal_string[] = " concat";
	utstring_new(s2, (void * )testdata);
	utstring_bincpy(s2, normal_string, strlen(normal_string));
	CU_ASSERT_PTR_NOT_NULL_FATAL(utstring_body(s2));
	CU_ASSERT_STRING_EQUAL(utstring_body(s2), " concat");
	ptr = utstring_body(s2);
	CU_ASSERT(
			(((char * )&testdata) < ptr)
					&& (((char * )&testdata + 10000) > ptr));
	utstring_concat(s, s2);
	CU_ASSERT_PTR_NOT_NULL_FATAL(utstring_body(s));
	ptr = utstring_body(s);
	CU_ASSERT(
			(((char * )&testdata) < ptr)
					&& (((char * )&testdata + 10000) > ptr));
	CU_ASSERT_STRING_EQUAL(utstring_body(s), "Test string 10 concat");
}

void runSuite(void) {
	CU_pSuite suite = CUnitCreateSuite("utstring");
	if (suite) {
		ADD_SUITE_TEST(suite, brp_utsting);
	}
}
