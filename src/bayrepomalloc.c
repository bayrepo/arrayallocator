/*
 * bayrepomalloc.c
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

#include <stdio.h>
#include <stdlib.h>
#include "bayreposbrk.h"
#include "bayrepomalloc.h"

int brp_malloc_init2(void *array_ptr, long array_size) {
	return brp_malloc_init_1(array_ptr, array_size, 0);
}

int brp_malloc_init3(void *array_ptr, long array_size, char *_allocalg) {
	int allocalg = 0;
	if (!strcmp(_allocalg ? _allocalg : "", "SBRK")
			|| !strcmp(_allocalg ? _allocalg : "", "sbrk")) {
		allocalg = 0;
	}
	switch (allocalg) {
	default:
		return brp_malloc_init_1(array_ptr, array_size, allocalg);
	}
}

void *brp_malloc(void *storage, long size) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		return brp_malloc_1(storage, size);
	default: {
		fprintf(stderr, "malloc was used without init of array\n");
		return NULL;
	}
	}
}

void brp_free(void *storage, void *ptr) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		brp_free_1(storage, ptr);
		return;
	default: {
		fprintf(stderr, "free was used without init of array\n");
		return;
	}
	}
}

void *brp_realloc(void *storage, void *ptr, long new_size) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		return brp_realloc_1(storage, ptr, new_size);
	default: {
		fprintf(stderr, "realloc was used without init of array\n");
		return NULL;
	}
	}
}

void *brp_calloc(void *storage, int elem_numbers, int elem_size) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		return brp_calloc_1(storage, elem_numbers, elem_size);
	default: {
		fprintf(stderr, "calloc was used without init of array\n");
		return NULL;
	}
	}
}

void brp_free_null(void *storage, void *ptr) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		brp_free_null_1(storage, ptr);
		return;
	default: {
		fprintf(stderr, "free_null was used without init of array\n");
		return;
	}
	}
}

void *brp_realloc_null(void *storage, void *ptr, long new_size) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		return brp_realloc_null_1(storage, ptr, new_size);
	default: {
		fprintf(stderr, "realloc_null was used without init of array\n");
		return NULL;
	}
	}
}

void *brp_get_next_region_info(void *storage, void *ptr, void **info) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		return brp_get_next_region_info_1(storage, ptr, info);
	default: {
		fprintf(stderr,
				"brp_get_next_region_info was used without init of array\n");
		return NULL;
	}
	}
}

void brp_return_allocation_picture(void *storage, char *buffer_for_picture,
		long size) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		brp_return_allocation_picture_1(storage, buffer_for_picture, size);
		return;
	default: {
		fprintf(stderr,
				"brp_return_allocation_picture was used without init of array\n");
		return;
	}
	}
}

void *brp_get_next_elem(void *storage) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		return brp_get_next_elem_1(storage);
	default: {
		fprintf(stderr, "brp_get_next_elem was used without init of array\n");
		return NULL;
	}
	}
}

long brp_get_free(void *storage) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		return brp_get_free_1(storage);
	default: {
		fprintf(stderr, "brp_get_free was used without init of array\n");
		return 0;
	}
	}
}

long brp_get_inuse(void *storage) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		return brp_get_inuse_1(storage);
	default: {
		fprintf(stderr, "brp_get_inuse was used without init of array\n");
		return 0;
	}
	}
}

void brp_return_allocation_stdout(void *storage) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		brp_return_allocation_stdout_1(storage);
		return;
	default: {
		fprintf(stderr,
				"brp_return_allocation_stdout was used without init of array\n");
		return;
	}
	}
}

int brp_make_pointers_table(void *storage, int pointers_number) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		return brp_make_pointers_table_1(storage, pointers_number);
	default: {
		fprintf(stderr,
				"brp_make_pointers_table was used without init of array\n");
		return 1;
	}
	}
}

void *brp_get_pointer_with_number(void *storage, int pointer_number) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		return brp_get_pointer_with_number_1(storage, pointer_number);
	default: {
		fprintf(stderr,
				"brp_get_pointer_with_number was used without init of array\n");
		return NULL;
	}
	}
}

void brp_set_pointer_to_number(void *storage, int pointer_number, void *value) {
	switch (brp_get_alloc_alg(storage)) {
	case 0:
		brp_set_pointer_to_number_1(storage, pointer_number, value);
		return;
	default: {
		fprintf(stderr,
				"brp_set_pointer_to_number was used without init of array\n");
	}
	}
}
