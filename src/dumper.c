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
#include "bayrepomalloc.h"
#include "bayrepodump.h"

int brp_dump_area(void *storage, char *file_dump) {
	long size_of_storage = brp_get_region_size(storage);
	if (size_of_storage) {
		int fd = open(file_dump, O_RDWR | O_CREAT | O_TRUNC, (mode_t) 0600);

		if (fd == -1) {
			return -1;
		}

		if (lseek(fd, size_of_storage - 1, SEEK_SET) == -1) {
			close(fd);
			return -2;
		}
		if (write(fd, "", 1) == -1) {
			close(fd);
			return -3;
		}

		char *map = mmap(0, size_of_storage, PROT_READ | PROT_WRITE, MAP_SHARED,
				fd, 0);
		if (map == MAP_FAILED) {
			close(fd);
			return -4;
		}
		char *c_storage = (char *) storage;
		size_t i = 0;
		for (i = 0; i < size_of_storage; i++) {
			map[i] = c_storage[i];
		}

		if (msync(map, size_of_storage, MS_SYNC) == -1) {
			perror("Could not sync the file to disk");
		}

		if (munmap(map, size_of_storage) == -1) {
			close(fd);
			return -5;
		}

		close(fd);

		return 0;

	}
	return 0;
}

char *brp_restore_dump(char *file_dump) {
	int fd = open(file_dump, O_RDONLY, (mode_t) 0600);

	if (fd == -1) {
		return NULL;
	}

	struct stat fileInfo = { 0 };

	if (fstat(fd, &fileInfo) == -1) {
		return NULL;
	}

	if (fileInfo.st_size == 0) {
		return NULL;
	}

	char *map = mmap(0, fileInfo.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED) {
		close(fd);
		return NULL;
	}

	char *data = calloc(1, fileInfo.st_size);

	if (data) {
		off_t i = 0;
		for (i = 0; i < fileInfo.st_size; i++) {
			data[i] = map[i];
		}
	}

	if (munmap(map, fileInfo.st_size) == -1) {
		close(fd);
		if (data)
			free(data);
		return NULL;
	}

	close(fd);

	return data;
}
