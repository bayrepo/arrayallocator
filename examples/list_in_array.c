/*
 * list_in_array.c
 *
 *  Created on: Aug 14, 2017
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
#include <string.h>
#include <time.h>
#include "bayrepomalloc.h"
#include "utlist.h"

/*
 * Задача по заполнению списка до предела, как только список заполнен и нет больше памяти
 * то удалить первых десять записией отсортированного списка и создать новые и пересортировать
 * и таким образом 5 раз
 */

#define MAX_WORD_LEN 10

typedef struct el {
	char *person_name;
	struct el *next, *prev;
} el;

int namecmp(el *a, el *b) {
	return strcmp(a->person_name, b->person_name);
}

void word_create(char *buffer, int buffer_len) {
	int new_len = 3 + rand() % (buffer_len - 4);
	memset(buffer, 0, buffer_len);
	int index = 0;
	for (index = 0; index < new_len; index++) {
		char c = 'a' + rand() % 20;
		buffer[index] = c;
	}
}

int main() {
	el *head = NULL;
	el *new_elem, *elt, *tmp = NULL;
	char buf[MAX_WORD_LEN];
	char alloc[10000] = { 0 };
	srand(time(NULL));
	char storage[1000] = { 0 };
	if (brp_malloc_init((void * )storage, 1000)) {
		fprintf(stderr, "Something wrong on initialization\n");
		exit(-1);
	}
	int index = 0;
	for (index = 0; index < 10; index++) {
		int counter = 0;
		printf("--------------------Action %d-------------------\n", index);
		do {
			new_elem = brp_malloc((void *) storage, sizeof(el));
			if (new_elem) {
				word_create(buf, MAX_WORD_LEN);
				new_elem->person_name = brp_strdup((void *) storage, buf);
				if (!new_elem->person_name) {
					brp_free((void *) storage, new_elem);
					new_elem = NULL;
				} else {
					DL_APPEND(head, new_elem);
					counter++;
				}
			}
		} while (new_elem);
		brp_return_allocation_picture((void *) storage, alloc, 10000);
		printf("Free %d, use %d: %s\n", brp_get_free((void *) storage),
				brp_get_inuse((void *) storage), alloc);
		printf("Added %d elements---->\n", counter);
		DL_FOREACH(head,elt)
		{
			printf("%s\n", elt->person_name);
		}
		DL_SORT(head, namecmp);
		printf("Sorted array---->\n", counter);
		DL_FOREACH(head, new_elem)
		{
			printf("%s\n", new_elem->person_name);
		}
		counter = 5;
		DL_FOREACH_SAFE(head,elt,tmp)
		{
			brp_free((void *) storage, elt->person_name);
			DL_DELETE(head, elt);
			brp_free((void *) storage, elt);
			counter--;
			if (!counter)
				break;
		}
		printf("Deleted array---->\n", counter);
		DL_FOREACH(head,elt)
		{
			printf("%s\n", elt->person_name);
		}
		brp_return_allocation_picture((void *) storage, alloc, 10000);
		printf("Free %d, use %d: %s\n", brp_get_free((void *) storage),
				brp_get_inuse((void *) storage), alloc);
	}
	DL_FOREACH_SAFE(head,elt,tmp)
	{
		brp_free((void *) storage, elt->person_name);
		DL_DELETE(head, elt);
		brp_free((void *) storage, elt);
	}
}
