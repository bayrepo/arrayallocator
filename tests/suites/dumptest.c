/*
 Copyright (c) 2017 Alexey Berezhok
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

TEST_FUNCT(brp_dump_test_1) {
	char testdata[200] = { 0 };
	CU_ASSERT_EQUAL(brp_malloc_init(testdata, 200), 0);
	brp_make_pointers_table(testdata, 2);
	char *data = brp_malloc(testdata, 10);
	CU_ASSERT_PTR_NOT_NULL_FATAL(data);
	memset(data, 0, 10);
	strcpy(data, "test1");
	brp_set_pointer_to_number(testdata, 0, data);
	data = brp_malloc(testdata, 10);
	CU_ASSERT_PTR_NOT_NULL_FATAL(data);
	memset(data, 0, 10);
	strcpy(data, "test2");
	brp_set_pointer_to_number(testdata, 1, data);
	int result = brp_dump_area((void *) testdata, "unit1.bin");
	CU_ASSERT_EQUAL(result, 0);
	char *newarena = brp_restore_dump("unit1.bin");
	CU_ASSERT_PTR_NOT_NULL_FATAL(newarena);
	char *newd = brp_get_pointer_with_number(newarena, 0);
	CU_ASSERT_PTR_NOT_NULL_FATAL(newd);
	CU_ASSERT_STRING_EQUAL_FATAL(newd, "test1");
	newd = brp_get_pointer_with_number(newarena, 1);
	CU_ASSERT_PTR_NOT_NULL_FATAL(newd);
	CU_ASSERT_STRING_EQUAL_FATAL(newd, "test2");
	free(newarena);
	unlink("unit1.bin");
}

void runSuite(void) {
	CU_pSuite suite = CUnitCreateSuite("dump");
	if (suite) {
		ADD_SUITE_TEST(suite, brp_dump_test_1);
	}
}
