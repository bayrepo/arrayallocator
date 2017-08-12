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
#include "utarray.h"
#include "utstring.h"
#include "utringbuffer.h"
#include "uthash.h"

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

TEST_FUNCT(brp_utsting_free) {
	char testdata[10000] = { 0 };
	char alloc_buffer[100] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init(testdata, 10000), 0);
	UT_string *s = NULL;
	utstring_new(s, (void * )testdata);
	utstring_printf(s, "Test string %d", 10);
	CU_ASSERT_PTR_NOT_NULL_FATAL(utstring_body(s));
	brp_return_allocation_picture((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "UUF");
	utstring_free(s);
	brp_return_allocation_picture((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "FFF");

}

TEST_FUNCT(brp_utringbuffer) {
	char testdata[10000] = { 0 };
	char result_buffer[200] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init(testdata, 10000), 0);
	UT_ringbuffer *history = NULL;
	utringbuffer_new(history, 7, &ut_int_icd, (void * )testdata);
	CU_ASSERT_PTR_NOT_NULL_FATAL(history);
	char *ptr = (char *) history;
	CU_ASSERT(
			(((char * )&testdata) < ptr)
					&& (((char * )&testdata + 10000) > ptr));
	int i, *p;
	for (i = 0; i < 10; i++)
		utringbuffer_push_back(history, &i);
	for (p = (int*) utringbuffer_front(history); p != NULL; p =
			(int*) utringbuffer_next(history, p)) {
		ptr = (char *) p;
		CU_ASSERT(
				(((char * )&testdata) < ptr)
						&& (((char * )&testdata + 10000) > ptr));
	}
	int data[7] = { 0 };
	for (i = 0; i < 7; i++) {
		int *elem = (int *) utringbuffer_eltptr(history, i);
		data[i] = *elem;
	}
	sprintf(result_buffer, "%d %d %d %d %d %d %d", data[0], data[1], data[2],
			data[3], data[4], data[5], data[6]);
	CU_ASSERT_STRING_EQUAL(result_buffer, "3 4 5 6 7 8 9");
}

TEST_FUNCT(brp_utringbuffer_clear_int) {
	char testdata[10000] = { 0 };
	char alloc_buffer[100] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init(testdata, 10000), 0);
	UT_ringbuffer *history = NULL;
	utringbuffer_new(history, 7, &ut_int_icd, (void * )testdata);
	CU_ASSERT_PTR_NOT_NULL_FATAL(history);
	int i, *p;
	for (i = 0; i < 10; i++)
		utringbuffer_push_back(history, &i);
	brp_return_allocation_picture((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "UUF");
	utringbuffer_clear(history);
	brp_return_allocation_picture((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "UUF");
}

TEST_FUNCT(brp_utringbuffer_clear_str) {
	char testdata[10000] = { 0 };
	char alloc_buffer[100] = { 0 };
	char result_buffer[1000] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init(testdata, 10000), 0);
	UT_ringbuffer *history = NULL;
	utringbuffer_new(history, 7, &ut_str_icd, (void * )testdata);
	CU_ASSERT_PTR_NOT_NULL_FATAL(history);
	int i;
	char **p;
	for (i = 0; i < 10; i++) {
		char s[100] = { 0 };
		sprintf(s, "T%d", i);
		char *pp = s;
		utringbuffer_push_back(history, &pp);
	}
	for (p = (char **) utringbuffer_front(history); p != NULL; p =
			(char **) utringbuffer_next(history, p)) {
		char *ptr = (char *) p;
		CU_ASSERT(
				(((char * )&testdata) < ptr)
						&& (((char * )&testdata + 10000) > ptr));
	}
	char *data[7] = { 0 };
	for (i = 0; i < 7; i++) {
		p = (char **) utringbuffer_eltptr(history, i);
		data[i] = *p;
	}
	sprintf(result_buffer, "%s %s %s %s %s %s %s", data[0], data[1], data[2],
			data[3], data[4], data[5], data[6]);
	CU_ASSERT_STRING_EQUAL(result_buffer, "T3 T4 T5 T6 T7 T8 T9");
	brp_return_allocation_picture((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "UUFFFUUUUUUUF");
	utringbuffer_clear(history);
	brp_return_allocation_picture((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "UUFFFFFFFFFFF");
}

TEST_FUNCT(brp_utringbuffer_free) {
	char testdata[10000] = { 0 };
	char alloc_buffer[100] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init(testdata, 10000), 0);
	UT_ringbuffer *history = NULL;
	utringbuffer_new(history, 7, &ut_int_icd, (void * )testdata);
	CU_ASSERT_PTR_NOT_NULL_FATAL(history);
	int i, *p;
	for (i = 0; i < 10; i++)
		utringbuffer_push_back(history, &i);
	brp_return_allocation_picture((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "UUF");
	utringbuffer_free(history);
	brp_return_allocation_picture((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "FFF");

}

struct my_struct {
	const char *name;
	int id;
	UT_hash_handle hh;
};

TEST_FUNCT(brp_uthash) {
	char testdata[10000] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init(testdata, 10000), 0);
	const char **n, *names[] = { "joe", "bob", "betty", NULL };
	struct my_struct *s, *tmp, *users = NULL;
	int i = 0;

	for (n = names; *n != NULL; n++) {
		s = (struct my_struct*) brp_malloc((void *) testdata,
				sizeof(struct my_struct));
		CU_ASSERT_PTR_NOT_NULL_FATAL(s);
		s->name = *n;
		s->id = i++;
		HASH_ADD_KEYPTR(hh, users, s->name, strlen(s->name), s,
				(void * )testdata);
		CU_ASSERT_PTR_NOT_NULL_FATAL(users);
	}
	char *ptr = (char *) users->hh.tbl;
	CU_ASSERT(
			(((char * )&testdata) < ptr)
					&& (((char * )&testdata + 10000) > ptr));
	ptr = (char *) users->hh.tbl->buckets;
	CU_ASSERT(
			(((char * )&testdata) < ptr)
					&& (((char * )&testdata + 10000) > ptr));

	HASH_FIND_STR(users, "betty", s);
	CU_ASSERT_PTR_NOT_NULL_FATAL(s);
	CU_ASSERT_EQUAL(s->id, 2);

}

TEST_FUNCT(brp_uthash_free) {
	char testdata[10000] = { 0 };
	char alloc_buffer[100] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init(testdata, 10000), 0);
	const char **n, *names[] = { "joe", "bob", "betty", NULL };
	struct my_struct *s, *tmp, *users = NULL;
	int i = 0;

	for (n = names; *n != NULL; n++) {
		s = (struct my_struct*) brp_malloc((void *) testdata,
				sizeof(struct my_struct));
		CU_ASSERT_PTR_NOT_NULL_FATAL(s);
		s->name = *n;
		s->id = i++;
		HASH_ADD_KEYPTR(hh, users, s->name, strlen(s->name), s,
				(void * )testdata);
		CU_ASSERT_PTR_NOT_NULL_FATAL(users);
	}
	brp_return_allocation_picture((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "UUUUUF");

	HASH_ITER(hh, users, s, tmp)
	{
		HASH_DEL(users, s);
		brp_free((void *) testdata, s);
	}
	brp_return_allocation_picture((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "FFFFFF");
}

TEST_FUNCT(brp_utarray) {
	char testdata[10000] = { 0 };
	char result_buffer[200] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init(testdata, 10000), 0);
	UT_array *nums;
	int i, *p;
	utarray_new(nums, &ut_int_icd, (void * )testdata);
	for (i = 0; i < 10; i++)
		utarray_push_back(nums, &i);
	int data[10] = { 0 };
	i = 0;
	for (p = (int*) utarray_front(nums); p != NULL;
			p = (int*) utarray_next(nums, p)) {
		char *ptr = (char *) p;
		CU_ASSERT(
				(((char * )&testdata) < ptr)
						&& (((char * )&testdata + 10000) > ptr));
		data[i++] = *p;
	}
	sprintf(result_buffer, "%d %d %d %d %d %d %d %d %d %d", data[0], data[1],
			data[2], data[3], data[4], data[5], data[6], data[7], data[8],
			data[9]);
	CU_ASSERT_STRING_EQUAL(result_buffer, "0 1 2 3 4 5 6 7 8 9");

}

TEST_FUNCT(brp_utarray_clear) {
	char testdata[10000] = { 0 };
	char alloc_buffer[100] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init(testdata, 10000), 0);
	UT_array *nums;
	int i, *p;
	utarray_new(nums, &ut_int_icd, (void * )testdata);
	for (i = 0; i < 10; i++)
		utarray_push_back(nums, &i);
	brp_return_allocation_picture((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "UFUF");
	utstring_clear(nums);
	brp_return_allocation_picture((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "UFUF");

}

TEST_FUNCT(brp_utarray_free) {
	char testdata[10000] = { 0 };
	char alloc_buffer[100] = { 0 };
	CU_ASSERT_EQUAL_FATAL(brp_malloc_init(testdata, 10000), 0);
	UT_array *nums;
	int i, *p;
	utarray_new(nums, &ut_int_icd, (void * )testdata);
	for (i = 0; i < 10; i++)
		utarray_push_back(nums, &i);
	brp_return_allocation_picture((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "UFUF");
	utstring_free(nums);
	brp_return_allocation_picture((void *) testdata, alloc_buffer, 100);
	CU_ASSERT_STRING_EQUAL(alloc_buffer, "FFFF");

}

void runSuite(void) {
	CU_pSuite suite = CUnitCreateSuite("utstring");
	if (suite) {
		ADD_SUITE_TEST(suite, brp_utsting);
		ADD_SUITE_TEST(suite, brp_utsting_free);
	}
	CU_pSuite suite2 = CUnitCreateSuite("utringbuffer");
	if (suite2) {
		ADD_SUITE_TEST(suite2, brp_utringbuffer);
		ADD_SUITE_TEST(suite2, brp_utringbuffer_clear_int);
		ADD_SUITE_TEST(suite2, brp_utringbuffer_clear_str);
		ADD_SUITE_TEST(suite2, brp_utringbuffer_free);
	}
	CU_pSuite suite3 = CUnitCreateSuite("uthash");
	if (suite3) {
		ADD_SUITE_TEST(suite3, brp_uthash);
		ADD_SUITE_TEST(suite3, brp_uthash_free);
	}
	CU_pSuite suite4 = CUnitCreateSuite("utarray");
	if (suite3) {
		ADD_SUITE_TEST(suite4, brp_utarray);
		ADD_SUITE_TEST(suite4, brp_utarray_clear);
		ADD_SUITE_TEST(suite4, brp_utarray_free);
	}
}
