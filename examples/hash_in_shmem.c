/*
 * hash_in_shmem.c
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
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "bayrepomalloc.h"
#include "uthash.h"

#define ARRAY_SIZE 100000

//Структура, хранящаяся в хэш-таблице
typedef struct _data_struct {
	char name[50]; //имя записи
	int counter;           // счетчик обращений
	UT_hash_handle hh;
} data_struct;

void* create_shared_memory(size_t size) {
	int protection = PROT_READ | PROT_WRITE;
	int visibility = MAP_ANONYMOUS | MAP_SHARED;
	return mmap(NULL, size, protection, visibility, 0, 0);
}

int main() {

	data_struct *storage = NULL; //будущий массив структур, обязательно должен быть обнулен
	pthread_mutex_t * pmutex = NULL;
	pthread_mutexattr_t attrmutex;

	void *hash_array = NULL;

	/*
	 * выделяем общую память разером под будущий массив с hash таблицей + область для mutex для
	 * синхронизации
	 */
	void* shmem = create_shared_memory(ARRAY_SIZE + sizeof(pthread_mutex_t));
	if (!shmem) {
		fprintf(stderr, "Error shared memeory allocation\n");
		exit(-1);
	}

	/*
	 * Инициализируем мьютекс и положим его в начало общей памяти
	 */
	pmutex = (pthread_mutex_t *) shmem;

	pthread_mutexattr_init(&attrmutex);
	pthread_mutexattr_setpshared(&attrmutex, PTHREAD_PROCESS_SHARED);

	pthread_mutex_init(pmutex, &attrmutex);

	/*
	 * Массив хэш таблицы будет располагаться сразу после мьютекса
	 */
	hash_array = (void *) ((char *) shmem + sizeof(pthread_mutex_t));
	pthread_mutex_lock(pmutex);
	brp_malloc_init(hash_array, ARRAY_SIZE);

	int pid = fork();

	if (pid == 0) {
		printf("Child: waiting for parent array initialization\n");
		pthread_mutex_lock(pmutex);
		storage = brp_get_pointer_with_number(hash_array, 0);
		if (!storage) {
			fprintf(stderr, "Something wrong");
		} else {
			printf("Child: try to find 3 items TEST10, TEST15, TEST100\n");
			data_struct *item = NULL;
			HASH_FIND_STR(storage, "TEST10", item);
			if (item) {
				printf("Child: TEST10 found. Change counter from %d to %d\n",
						item->counter, item->counter + 1000);
				item->counter += 1000;
			}
			HASH_FIND_STR(storage, "TEST15", item);
			if (item) {
				printf("Child: TEST15 found. Change counter from %d to %d\n",
						item->counter, item->counter + 2000);
				item->counter += 2000;
			}
			HASH_FIND_STR(storage, "TEST100", item);
			if (!item) {
				printf("Child: TEST100 not found. Going to create it\n");
				item = (data_struct*) brp_malloc(hash_array,
						sizeof(data_struct));
				snprintf(item->name, 50, "TEST100");
				item->counter = 10000;
				HASH_ADD_STR(storage, name, item, hash_array);
			}

		}
		pthread_mutex_unlock(pmutex);
		pthread_mutexattr_destroy(&attrmutex);

	} else if (pid == -1) {
		fprintf(stderr, "Fork error\n");
	} else {
		//Родитель инициализирует массив заполняет его и отпускает мьютекс
		int index = 0;
		data_struct *item, *tmp = NULL;
		printf("Parent: add data to hash array\n");
		for (index = 0; index < 20; index++) {
			item = (data_struct*) brp_malloc(hash_array, sizeof(data_struct));
			snprintf(item->name, 50, "TEST%d", index);
			item->counter = index;
			HASH_ADD_STR(storage, name, item, hash_array);
		}
		//Создадим в памяти один указатель на таблицу, чтоб в дочернем процессе знать
		//где hash таблица начигается
		brp_make_pointers_table(hash_array, 1);
		brp_set_pointer_to_number(hash_array, 0, storage);

		pthread_mutex_unlock(pmutex);
		printf("Parent: wait for children work\n");
		sleep(2);
		pthread_mutex_lock(pmutex);
		printf("Parent: read of children activity\n");
		index = 0;
		HASH_ITER(hh, storage, item, tmp)
		{
			printf("Parent: %d) %s=>%d\n", index++, item->name, item->counter);
			HASH_DEL(storage, item);
			brp_free(hash_array, item);
		}
		pthread_mutex_unlock(pmutex);

		//Освободим память
		pthread_mutex_destroy(pmutex);
		pthread_mutexattr_destroy(&attrmutex);
		munmap(shmem, ARRAY_SIZE + sizeof(pthread_mutex_t));
	}

}
