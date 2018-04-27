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

#ifndef __BAYREPOMALLOC_H__
#define __BAYREPOMALLOC_H__

#define COUNT_PARMS2(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _, ...) _
#define COUNT_PARMS(...)\
	COUNT_PARMS2(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define CAT(A, B) CAT2(A, B)
#define CAT2(A, B) A ## B

#define brp_malloc_init(...)\
	CAT(brp_malloc_init, COUNT_PARMS(__VA_ARGS__))(__VA_ARGS__)

//Эти функции должны присуствовать во всех алгоритмах
int brp_malloc_init2(void *array_ptr, long array_size);
int brp_malloc_init3(void *array_ptr, long array_size, char *_allocalg);
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
long brp_get_region_size(void *storage);
//Конец обязательных функций

#include "bayrepostrings.h"

#endif
