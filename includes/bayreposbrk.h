/*
 * bayreposbrk.h
 *
 *  Created on: Aug 10, 2017
 *
 */

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

#ifndef INCLUDES_BAYREPOSBRK_H_
#define INCLUDES_BAYREPOSBRK_H_

#define S_SIGN 12345
#define C_SIGN_0 'B'
#define C_SIGN_1 'R'
#define C_FREE 'F'
#define C_USED 'U'

typedef struct __attribute__((__packed__)) __dataChunk {
	char sign[2];  //'BR'
	unsigned long size;
	char status;
} dataChunk;

typedef struct __attribute__((__packed__)) __globalDataStorage {
	int allocalg;
	int sign;
	long array_size;
	void *next_elem;
	void **pointers_table;
	long allocated_size;
	long allocated_times;
	long max_number_of_pointers;
} globalDataStorage;

int brp_malloc_init_1(void *array_ptr, long array_size, int allocalg);
void *brp_malloc_1(void *storage, long size);
void brp_free_1(void *storage, void *ptr);
void *brp_realloc_1(void *storage, void *ptr, long new_size);
void *brp_calloc_1(void *storage, int elem_numbers, int elem_size);
void brp_free_null_1(void *storage, void *ptr);
void *brp_realloc_null_1(void *storage, void *ptr, long new_size);

void *brp_get_next_region_info_1(void *storage, void *ptr, void **info);
void brp_return_allocation_picture_1(void *storage, char *buffer_for_picture,
		long size);
void *brp_get_next_elem_1(void *storage);
long brp_get_free_1(void *storage);
long brp_get_inuse_1(void *storage);
void brp_return_allocation_stdout_1(void *storage);

int brp_get_alloc_alg(void *storage);

int brp_make_pointers_table_1(void *storage, int pointers_number);
void *brp_get_pointer_with_number_1(void *storage, int pointer_number);
void brp_set_pointer_to_number_1(void *storage, int pointer_number, void *value);

long brp_get_region_size_1(void *array_ptr);
long brp_summary_allocation_size(void *storage);

#endif /* INCLUDES_BAYREPOSBRK_H_ */
