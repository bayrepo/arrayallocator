/*
 * bayrepostings.c
 *
 *  Created on: Aug 11, 2017
 *      Author: alexey
 */

#include <stdio.h>
#include <string.h>

#include "bayrepomalloc.h"

char *brp_strdup(void *storage, const char *__s) {
	long size = strlen(__s);
	char *ptr = brp_calloc(storage, size + 1, sizeof(char));
	if (ptr) {
		strcpy(ptr, __s);
	}
	return ptr;
}

char *brp_strndup(void *storage, const char *__string, size_t __n) {
	long size = strlen(__string);
	size = (size > __n) ? __n : size;
	char *ptr = brp_calloc(storage, size + 1, sizeof(char));
	if (ptr) {
		strncpy(ptr, __string, size);
	}
	return ptr;
}
