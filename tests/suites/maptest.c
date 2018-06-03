/*
 Copyright (c) 2018 Alexey Berezhok
 email: bayrepo.info@gmail.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <CUnit/Basic.h>

#include "utils.h"
#include "bayrepomalloc.h"
#include "bayreposbrk.h"
#include "bayrepostrings.h"
#include "bayrepodump.h"
#include "map.h"

TEST_FUNCT(brp_map_test_init) {
	char testdata[1024] = { 0 };
	CU_ASSERT_EQUAL(brp_malloc_init(testdata, 1024), 0);
	brp_map_int_t *m = brp_malloc(testdata, sizeof(brp_map_int_t));
	CU_ASSERT_PTR_NOT_NULL_FATAL(m);
	brp_map_init(m, testdata);
	brp_map_set(m, "test1", 1, testdata);
	brp_map_set(m, "test2", 10, testdata);
	brp_map_set(m, "test3", 100, testdata);
	int *val = brp_map_get(m, "test2", testdata);
	CU_ASSERT_PTR_NOT_NULL_FATAL(val);
	CU_ASSERT_EQUAL(*val, 10);
	brp_make_pointers_table(testdata, 1);
	brp_set_pointer_to_number(testdata, 0, m);
	int result = brp_dump_area((void *) testdata, "unit2.bin");
	CU_ASSERT_EQUAL(result, 0);
}

TEST_FUNCT(brp_map_test_read) {
	char testdata[1024] = { 0 };
	CU_ASSERT_EQUAL(brp_malloc_init(testdata, 1024), 0);
	char *newarena = brp_restore_dump("unit2.bin");
	CU_ASSERT_PTR_NOT_NULL_FATAL(newarena);
	brp_map_int_t *m = brp_get_pointer_with_number(newarena, 0);
	CU_ASSERT_PTR_NOT_NULL_FATAL(m);
	int *val = brp_map_get(m, "test2", testdata);
	CU_ASSERT_PTR_NOT_NULL_FATAL(val);
	CU_ASSERT_EQUAL(*val, 10);
	val = brp_map_get(m, "test1", testdata);
	CU_ASSERT_PTR_NOT_NULL_FATAL(val);
	CU_ASSERT_EQUAL(*val, 1);
	val = brp_map_get(m, "test3", testdata);
	CU_ASSERT_PTR_NOT_NULL_FATAL(val);
	CU_ASSERT_EQUAL(*val, 100);
	free(newarena);
	unlink("unit2.bin");
}

void runSuite(void) {
	CU_pSuite suite = CUnitCreateSuite("map test");
	if (suite) {
		ADD_SUITE_TEST(suite, brp_map_test_init);
		ADD_SUITE_TEST(suite, brp_map_test_read);
	}
}

