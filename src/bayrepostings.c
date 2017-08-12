/*
 * bayrepostings.c
 *
 *  Created on: Aug 11, 2017
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
