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
#include "bayrepomalloc.h"

union brp_dump_Data {
   char dt;
   long offset;
   void *addr;
};

FILE *brp_file_open(char *file_name) {
	FILE *fp = fopen(file_name, "wb");
	if (fp) {
		char addr_size = sizeof(void *);
		fwrite(&addr_size, sizeof(char), 1, fp);
	}
	return fp;
}

int brp_write_one_byte(FILE *fp, char data){

}

int brp_dump_area(void *storage, char *file_dump) {
	FILE fp = brp_file_open(file_dump);
	if (!fp) return 0;
	long size_of_storage = brp_get_region_size(storage);
	if (size_of_storage) {
		char *pointer = (char *) storage;
		char *endptr = pointer + size_of_storage;
		int addr_size = sizeof(void *);
		while (pointer < endptr) {
			if ((pointer + addr_size) <= endptr) {
				void *addr = (void *) pointer;
				if ((*addr >= storage) && (*addr <= endptr)) {
					//write offset
					pointer += addr_size;
				} else {
					//Write byte
					pointer++;
				}
			} else {
				//Write byte to file
				pointer++;
			}
		}
	}
}
