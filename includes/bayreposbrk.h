/*
 * bayreposbrk.h
 *
 *  Created on: Aug 10, 2017
 *      Author: alexey
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
	long size;
	char status;
} dataChunk;

typedef struct __attribute__((__packed__)) __globalDataStorage {
	int allocalg;
	int sign;
	long array_size;
	void *next_elem;
	void **pointers_table;
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

#endif /* INCLUDES_BAYREPOSBRK_H_ */
