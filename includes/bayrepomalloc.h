#ifndef __BAYREPOMALLOC_H__
#define __BAYREPOMALLOC_H__

//Эти функции должны присуствовать во всех алгоритмах
int brp_malloc_init(void *array_ptr, long array_size);
void *brp_malloc(void *storage, long size);
void brp_free(void *storage, void *ptr);
void *brp_realloc(void *storage, void *ptr, long new_size);
void *brp_calloc(void *storage, int elem_numbers, int elem_size);
void brp_free_null(void *storage, void *ptr);
void *brp_realloc_null(void *storage, void *ptr, long new_size);

void *brp_get_next_region_info(void *storage, void *ptr, void **info);
void brp_return_allocation_picture(void *storage, char *buffer_for_picture,
		long size);
void *brp_get_next_elem(void *storage);
long brp_get_free(void *storage);
long brp_get_inuse(void *storage);
void brp_return_allocation_stdout(void *storage);

int brp_make_pointers_table(void *storage, int pointers_number);
void *brp_get_pointer_with_number(void *storage, int pointer_number);
void brp_set_pointer_to_number(void *storage, int pointer_number, void *value);
//Конец обязательных функций

#include "bayrepostrings.h"

#endif
