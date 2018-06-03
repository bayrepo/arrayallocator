/*
 * dumper.c
 *
 *  Created on: Apr 11, 2018
 *      Author: alexey
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>
#include "bayrepomalloc.h"
#include "bayrepodump.h"

typedef union brp_addr_ {
	char raw[8];
	uint64_t fulladdr;
} brp_addr;

typedef struct offsets_ {
	long pos;
	long offset;
} offsets;

static int is_little_endian() {
	unsigned int i = 1;
	char *c = (char*) &i;
	if (*c)
		return 1;
	else {
		return 0;
	}
}

static long get_offset(void *ptr, void *begin, void *end, int size) {
	brp_addr data = { 0 };
	if (size > 8)
		return -1;
	if ((ptr + size) >= end)
		return -1;
	if (ptr < begin)
		return -1;
	data.fulladdr = 0;
	int order = is_little_endian();
	if (order) {
		int i = 0;
		for (i = 0; i < size; i++) {
			char *p = (char *) ptr;
			data.raw[i] = *(p + i);
		}
	} else {
		int i = 0;
		for (i = 8 - size; i < 8; i++) {
			char *p = (char *) ptr;
			data.raw[i] = *(p + i);
		}
	}
	void *tmp = (void *) data.fulladdr;
	if ((tmp >= begin) && (tmp < end)) {
		return tmp - begin;
	}
	return -1;
}

int brp_dump_area(void *storage, char *file_dump) {
	long size_of_storage = brp_get_region_size(storage);
	if (size_of_storage) {
		FILE *fp = fopen(file_dump, "wb");
		if (!fp)
			return -1;
		fwrite(&size_of_storage, sizeof(long), 1, fp);
		char *c_storage = (char *) storage;
		size_t i = 0;
		int sz_of_addr = sizeof(void *);
		long offsets_counter = 0;
		for (i = 0; i < size_of_storage; i++) {
			void *addr = (void *) (c_storage + i);
			long offset = get_offset(addr, storage,
					(void *) (c_storage + size_of_storage), sz_of_addr);
			if (offset >= 0)
				offsets_counter++;
		}
		fwrite(&offsets_counter, sizeof(long), 1, fp);
		for (i = 0; i < size_of_storage; i++) {
			void *addr = (void *) (c_storage + i);
			long offset = get_offset(addr, storage,
					(void *) (c_storage + size_of_storage), sz_of_addr);
			if (offset >= 0) {
				offsets of;
				of.offset = offset;
				of.pos = i;
				fwrite(&of, sizeof(of), 1, fp);
			}
		}
		for (i = 0; i < size_of_storage; i++) {
			fwrite((void *) &c_storage[i], sizeof(char), 1, fp);
		}

		fclose(fp);
		return 0;

	}
	return 0;
}

char *brp_restore_dump(char *file_dump) {
	FILE *fp = fopen(file_dump, "rb");
	if (!fp)
		return NULL;
	long size_of_storage = 0;
	fread(&size_of_storage, sizeof(long), 1, fp);
	if (!size_of_storage) {
		fclose(fp);
		return NULL;
	}
	long numb_offs = 0;
	fread(&numb_offs, sizeof(long), 1, fp);
	fseek(fp, numb_offs * sizeof(offsets), SEEK_CUR);

	char *data = calloc(1, size_of_storage);

	fread(data, sizeof(char), size_of_storage, fp);

	fseek(fp, 2 * sizeof(long), SEEK_SET);

	int i = 0;
	for (i = 0; i < numb_offs; i++) {
		offsets of;
		fread(&of, sizeof(offsets), 1, fp);
		brp_addr addr;
		addr.fulladdr = data + of.offset;
		if (is_little_endian()) {
			int j = 0;
			for (j = 0; j < sizeof(void *); j++) {
				*(data + of.pos + j) = addr.raw[j];
			}
		} else {
			int j = 0;
			for (j = 8 - sizeof(void *); j < sizeof(void *); j++) {
				*(data + of.pos + j - (8 - sizeof(void *))) = addr.raw[j];
			}
		}
	}

	fclose(fp);

	return data;
}
