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
#include <CUnit/Basic.h>

#include "utils.h"
#include "bayreposbrk.h"
#include "bayrepostrings.h"

TEST_FUNCT(brp_malloc_init_1) {
	char testdata[1000] = { 0 };
	CU_ASSERT_EQUAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	CU_ASSERT_PTR_NULL(brp_get_next_elem_1((void * )testdata));
}

TEST_FUNCT(brp_malloc_init_small_1) {
	char small_test_data[sizeof(globalDataStorage) - 1] = { 0 };
	CU_ASSERT_EQUAL(
			brp_malloc_init_1(small_test_data, (sizeof(globalDataStorage) - 1),
					0), -1);
}

TEST_FUNCT(brp_malloc_1) {
	char testdata[1000] = { 0 };
	CU_ASSERT_EQUAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	CU_ASSERT_PTR_NULL(brp_get_next_elem_1((void * )testdata));
	char *ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_PTR_NOT_NULL(ptr);

}

TEST_FUNCT(brp_malloc_reinit_1) {
	char testdata[1000] = { 0 };
	char alloc_buffer[100] = { 0 };
	CU_ASSERT_EQUAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	CU_ASSERT_PTR_NULL(brp_get_next_elem_1((void * )testdata));
	char *ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_PTR_NOT_NULL(ptr);
	CU_ASSERT_EQUAL(brp_malloc_init_1(testdata, 500, 0), 0);
	ptr = (char *) brp_malloc_1((void *) testdata, 10);
	brp_return_allocation_picture_1((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL_FATAL(alloc_buffer, "UUF");
}

TEST_FUNCT(brp_free_1) {
	char testdata[1000] = { 0 };
	dataChunk *chunk = NULL;
	char alloc_buffer[100] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	CU_ASSERT_PTR_NULL_FATAL(brp_get_next_elem_1(testdata));
	char *ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_PTR_NOT_NULL_FATAL(brp_get_next_elem_1((void * )testdata));
	char *area = (char *) brp_get_next_region_info_1((void *) testdata, NULL,
			(void **) &chunk);
	CU_ASSERT_PTR_NOT_NULL_FATAL(area);
	CU_ASSERT_PTR_NOT_NULL_FATAL(chunk);
	CU_ASSERT_FATAL(
			(chunk->sign[0] == C_SIGN_0) && (chunk->sign[1] == C_SIGN_1));
	brp_return_allocation_picture_1((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL_FATAL(alloc_buffer, "UF");
	strcpy(ptr, "TEST");
	char *ptr_test = (char *) testdata + sizeof(globalDataStorage)
			+ sizeof(dataChunk);
	CU_ASSERT_STRING_EQUAL(ptr_test, "TEST");
	brp_free_1((void *) testdata, ptr);
	brp_return_allocation_picture_1((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL_FATAL(alloc_buffer, "FF");
}

TEST_FUNCT(brp_malloc_size_1) {
	char testdata[1000] = { 0 };
	brp_malloc_init_1(testdata, 1000, 0);
	char *ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_EQUAL(brp_get_free_1((void * )testdata),
			1000 - sizeof(globalDataStorage) - sizeof(dataChunk)
					- (10 * sizeof(char)) - sizeof(dataChunk));
	CU_ASSERT_EQUAL(brp_get_inuse_1((void * )testdata), 10);
	brp_free_1((void *) testdata, ptr);
	CU_ASSERT_EQUAL(brp_get_free_1((void * )testdata),
			1000 - sizeof(globalDataStorage) - sizeof(dataChunk)
					- sizeof(dataChunk));
	CU_ASSERT_EQUAL(brp_get_inuse_1((void * )testdata), 0);
}

TEST_FUNCT(brp_malloc_malloc_free_large_1) {
	char testdata[1000] = { 0 };
	dataChunk *chunk = NULL;
	char alloc_buffer[100] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	CU_ASSERT_PTR_NULL_FATAL(brp_get_next_elem_1((void * )testdata));
	char *ptr = (char *) brp_malloc_1((void *) testdata,
			1000 - sizeof(globalDataStorage) - sizeof(dataChunk));
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	CU_ASSERT_PTR_NOT_NULL_FATAL(brp_get_next_elem_1((void * )testdata));
	char *area = (char *) brp_get_next_region_info_1((void *) testdata, NULL,
			(void **) &chunk);
	CU_ASSERT_PTR_NOT_NULL_FATAL(area);
	CU_ASSERT_PTR_NOT_NULL_FATAL(chunk);
	CU_ASSERT_FATAL(
			(chunk->sign[0] == C_SIGN_0) && (chunk->sign[1] == C_SIGN_1));
	brp_return_allocation_picture_1((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "U");
	strcpy(ptr, "TEST");
	char *ptr_test = (char *) testdata + sizeof(globalDataStorage)
			+ sizeof(dataChunk);
	CU_ASSERT_STRING_EQUAL(ptr_test, "TEST");
	brp_free_1((void *) testdata, ptr);
	brp_return_allocation_picture_1((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "F");
}

TEST_FUNCT(brp_realloc_1) {
	char testdata[1000] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	char *ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	strcpy(ptr, "TEST");
	char *ptr_copy = (char *) brp_realloc_1(testdata, (void *) ptr, 30);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr_copy);
	CU_ASSERT_STRING_EQUAL(ptr_copy, "TEST");
	CU_ASSERT_NOT_EQUAL(ptr, ptr_copy);
	char *ptr_copy_2 = (char *) brp_realloc_1(testdata, (void *) NULL, 30);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr_copy_2);
}

TEST_FUNCT(brp_realloc_big_small_1) {
	char testdata[1000] = { 0 };
	char alloc_buffer[100] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	char *ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	strcpy(ptr, "TESTTEST");
	brp_return_allocation_picture_1((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "UF");
	char *ptr_copy = (char *) brp_realloc_1(testdata, (void *) ptr, 5);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr_copy);
	CU_ASSERT_EQUAL(ptr_copy[0], 'T');
	CU_ASSERT_EQUAL(ptr_copy[1], 'E');
	CU_ASSERT_EQUAL(ptr_copy[2], 'S');
	CU_ASSERT_EQUAL(ptr_copy[3], 'T');
	CU_ASSERT_EQUAL(ptr_copy[4], 'T');
	CU_ASSERT_NOT_EQUAL(ptr, ptr_copy);
	brp_return_allocation_picture_1((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "FUF");

}

TEST_FUNCT(brp_calloc_1) {
	char testdata[1000] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	char *ptr = (char *) brp_calloc_1((void *) testdata, 100, 1);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	long sum = 0;
	int index = 0;
	for (index = 0; index < 100; index++) {
		sum += ptr[index];
	}
	CU_ASSERT_EQUAL(sum, 0);
}

TEST_FUNCT(brp_free_null_1) {
	char testdata[1000] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	char *ptr = (char *) brp_calloc_1((void *) testdata, 100, 1);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	int index = 0;
	for (index = 0; index < 100; index++) {
		ptr[index] = 1;
	}
	brp_free_null_1((void *) testdata, ptr);
	long sum = 0;
	for (index = 0; index < 100; index++) {
		sum += ptr[index];
	}
	CU_ASSERT_EQUAL(sum, 0);
}

TEST_FUNCT(brp_realloc_null_1) {
	char testdata[1000] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	char *ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	strcpy(ptr, "TEST");
	long sum = 0;
	int index = 0;
	for (index = 0; index < 10; index++) {
		sum += ptr[index];
	}
	CU_ASSERT_NOT_EQUAL(sum, 0);
	char *ptr_copy = (char *) brp_realloc_null_1(testdata, (void *) ptr, 30);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr_copy);
	CU_ASSERT_STRING_EQUAL(ptr_copy, "TEST");
	CU_ASSERT_NOT_EQUAL(ptr, ptr_copy);
	sum = 0;
	for (index = 0; index < 10; index++) {
		sum += ptr[index];
	}
	CU_ASSERT_EQUAL(sum, 0);
}

TEST_FUNCT(brp_get_next_region_info_1) {
	char testdata[1000] = { 0 };
	dataChunk *chunk = NULL;
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	char *ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	char *area = (char *) brp_get_next_region_info_1((void *) testdata, NULL,
			(void **) &chunk);
	CU_ASSERT_PTR_NOT_NULL_FATAL(area);
	CU_ASSERT_PTR_NOT_NULL_FATAL(chunk);
	CU_ASSERT_EQUAL(chunk->size, 10);
	char *ptr_new = (char *) brp_malloc_1((void *) testdata, 50);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr_new);
	area = (char *) brp_get_next_region_info_1((void *) testdata, ptr,
			(void **) &chunk);
	CU_ASSERT_PTR_NOT_NULL_FATAL(area);
	CU_ASSERT_PTR_NOT_NULL_FATAL(chunk);
	CU_ASSERT_EQUAL(chunk->size, 50);
}

TEST_FUNCT(brp_get_next_elem_1) {
	char testdata[1000] = { 0 };
	dataChunk *chunk = NULL;
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	CU_ASSERT_PTR_NULL(brp_get_next_elem_1(testdata));
	char *ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_PTR_NOT_NULL(brp_get_next_elem_1((void * )testdata));
	CU_ASSERT_EQUAL(((char * )ptr + 10), brp_get_next_elem_1(testdata));
}

TEST_FUNCT(brp_get_free_1) {
	char testdata[1000] = { 0 };
	dataChunk *chunk = NULL;
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	char *ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_EQUAL(
			(1000 - sizeof(globalDataStorage) - sizeof(dataChunk)
					- 10 * sizeof(char) - sizeof(dataChunk)),
			brp_get_free(testdata));
}

TEST_FUNCT(brp_get_inuse_1) {
	char testdata[1000] = { 0 };
	dataChunk *chunk = NULL;
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	char *ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_EQUAL(10, brp_get_inuse_1(testdata));
}

TEST_FUNCT(brp_return_allocation_picture_1) {
	char testdata[1000] = { 0 };
	char alloc_buffer[4] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	char *ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	brp_return_allocation_picture_1((void *) testdata, alloc_buffer, 4);
	CU_ASSERT_STRING_EQUAL_FATAL(alloc_buffer, "UU");
}

// Проверка ситуации, когда закончилась память в массиве
TEST_FUNCT(brp_check_cannot_allocate_1) {
	char testdata[4 * (sizeof(dataChunk) + 10) + sizeof(globalDataStorage)] = {
			0 };
	CU_ASSERT_EQUAL_FATAL(
			brp_malloc_init_1(testdata,
					4 * (sizeof(dataChunk) + 10) + sizeof(globalDataStorage),
					0), 0);
	char *ptr = NULL;
	char *ptr_2 = NULL;
	char *ptr_last = NULL;
	int index = 0;
	int number = 4;
	for (index = 0; index < (number + 1); index++) {
		ptr = (char *) brp_malloc_1((void *) testdata, 10);
		if (index == 1) {
			ptr_2 = ptr;
		} else if (index == (number - 1)) {
			ptr_last = ptr;
		}
		if (index == number) {
			CU_ASSERT_PTR_NULL_FATAL(ptr);
		} else {
			CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
		}
	}
	brp_free_1((void *) testdata, ptr_last);
	ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	ptr = (char *) brp_malloc_1((void *) testdata, 10);
	CU_ASSERT_PTR_NULL_FATAL(ptr);
	brp_free_1((void *) testdata, ptr_2);
	ptr = (char *) brp_malloc_1((void *) testdata, 5);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	ptr = (char *) brp_malloc_1((void *) testdata, 5);
	CU_ASSERT_PTR_NULL_FATAL(ptr);
}

//Проверка склейки двух маленьких областей в одну болшую
TEST_FUNCT(brp_check_concat_1) {
	char testdata[4 * (sizeof(dataChunk) + 10) + sizeof(globalDataStorage)] = {
			0 };
	char alloc_buffer[100] = { 0 };
	CU_ASSERT_EQUAL_FATAL(
			brp_malloc_init_1(testdata,
					4 * (sizeof(dataChunk) + 10) + sizeof(globalDataStorage),
					0), 0);
	char *ptr = NULL;
	char *ptr_last = NULL;
	char *ptr_pre_last = NULL;
	int index = 0;
	int number = 4;
	for (index = 0; index < number; index++) {
		ptr = (char *) brp_malloc_1((void *) testdata, 10);
		if (index == (number - 1)) {
			ptr_last = ptr;
		} else if (index == (number - 2)) {
			ptr_pre_last = ptr;
		}
		CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	}
	brp_return_allocation_picture_1((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL_FATAL(alloc_buffer, "UUUU");
	brp_free_1((void *) testdata, ptr_last);
	brp_return_allocation_picture_1((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL_FATAL(alloc_buffer, "UUUF");
	brp_free_1((void *) testdata, ptr_pre_last);
	brp_return_allocation_picture_1((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL_FATAL(alloc_buffer, "UUFF");
	ptr = (char *) brp_malloc_1((void *) testdata, 15);
	brp_return_allocation_picture_1((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL_FATAL(alloc_buffer, "UUUF");
	CU_ASSERT_PTR_NOT_NULL_FATAL(ptr);
	ptr = (char *) brp_malloc_1((void *) testdata, 10);
	brp_return_allocation_picture_1((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL_FATAL(alloc_buffer, "UUUF");
	CU_ASSERT_PTR_NULL_FATAL(ptr);

}

TEST_FUNCT(brp_make_pointers_table_1) {
	char testdata[1000] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	CU_ASSERT_EQUAL(brp_make_pointers_table_1((void * )testdata, 20), 0)
}

TEST_FUNCT(brp_pointers_1) {
	char testdata[1000] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	CU_ASSERT_EQUAL_FATAL(brp_make_pointers_table_1((void * )testdata, 20), 0)
	brp_set_pointer_to_number_1((void *) testdata, 0, (void *) 1);
	brp_set_pointer_to_number_1((void *) testdata, 1, (void *) 10);
	brp_set_pointer_to_number_1((void *) testdata, 5, (void *) 100);
	brp_set_pointer_to_number_1((void *) testdata, 12, (void *) 1000);
	CU_ASSERT_EQUAL((long )brp_get_pointer_with_number_1((void * )testdata, 0),
			1)
	CU_ASSERT_EQUAL((long )brp_get_pointer_with_number_1((void * )testdata, 1),
			10)
	CU_ASSERT_EQUAL((long )brp_get_pointer_with_number_1((void * )testdata, 5),
			100)
	CU_ASSERT_EQUAL((long )brp_get_pointer_with_number_1((void * )testdata, 12),
			1000)
}

TEST_FUNCT(brp_recreate_pointers_table_1) {
	char testdata[1000] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	CU_ASSERT_EQUAL_FATAL(brp_make_pointers_table_1((void * )testdata, 20), 0)
	brp_set_pointer_to_number_1((void *) testdata, 0, (void *) 1);
	CU_ASSERT_EQUAL((long )brp_get_pointer_with_number_1((void * )testdata, 0),
			1)
	CU_ASSERT_EQUAL(brp_make_pointers_table_1((void * )testdata, 30), 0)
	CU_ASSERT_EQUAL((long )brp_get_pointer_with_number_1((void * )testdata, 0),
			1)
}

TEST_FUNCT(brp_strdup) {
	char testdata[1000] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	char *test = "TESTTESTTESTTEST";
	char *result = brp_strdup((void *) testdata, test);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_STRING_EQUAL(result, test);
}

TEST_FUNCT(brp_strndup) {
	char testdata[1000] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init_1(testdata, 1000, 0), 0);
	char *test = "TESTTESTTESTTEST";
	char *result = brp_strndup((void *) testdata, test, 100);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_STRING_EQUAL(result, test);
	char *result2 = brp_strndup((void *) testdata, result, 5);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result2);
	CU_ASSERT_STRING_EQUAL(result2, "TESTT");
}

void runSuite(void) {
	CU_pSuite suite = CUnitCreateSuite("mallocsbrk");
	if (suite) {
		ADD_SUITE_TEST(suite, brp_malloc_init_1);
		ADD_SUITE_TEST(suite, brp_malloc_init_small_1);
		ADD_SUITE_TEST(suite, brp_malloc_1);
		ADD_SUITE_TEST(suite, brp_malloc_reinit_1);
		ADD_SUITE_TEST(suite, brp_free_1);
		ADD_SUITE_TEST(suite, brp_malloc_size_1);
		ADD_SUITE_TEST(suite, brp_malloc_malloc_free_large_1);
		ADD_SUITE_TEST(suite, brp_realloc_1);
		ADD_SUITE_TEST(suite, brp_realloc_big_small_1);
		ADD_SUITE_TEST(suite, brp_calloc_1);
		ADD_SUITE_TEST(suite, brp_free_null_1);
		ADD_SUITE_TEST(suite, brp_realloc_null_1);
		ADD_SUITE_TEST(suite, brp_get_next_region_info_1);
		ADD_SUITE_TEST(suite, brp_get_next_elem_1);
		ADD_SUITE_TEST(suite, brp_get_free_1);
		ADD_SUITE_TEST(suite, brp_get_inuse_1);
		ADD_SUITE_TEST(suite, brp_return_allocation_picture_1);
		ADD_SUITE_TEST(suite, brp_check_cannot_allocate_1);
		ADD_SUITE_TEST(suite, brp_check_concat_1);
	}
	CU_pSuite suite2 = CUnitCreateSuite("pointers table");
	if (suite2) {
		ADD_SUITE_TEST(suite2, brp_make_pointers_table_1);
		ADD_SUITE_TEST(suite2, brp_pointers_1);
		ADD_SUITE_TEST(suite2, brp_recreate_pointers_table_1);
	}
	CU_pSuite suite3 = CUnitCreateSuite("strdup");
	if (suite2) {
		ADD_SUITE_TEST(suite3, brp_strdup);
		ADD_SUITE_TEST(suite3, brp_strndup);
	}
}
