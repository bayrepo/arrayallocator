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
#ifdef MTHREAD
#include <pthread.h>
#endif

#include "bayreposbrk.h"

// мьютекс, ограничивающий доступ, при переразделении памяти
#ifdef MTHREAD
static pthread_mutex_t access_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

static inline void lock() {
#ifdef MTHREAD
	pthread_mutex_lock(&access_lock);
#endif
}

static inline void unlock() {
#ifdef MTHREAD
	pthread_mutex_unlock(&access_lock);
#endif
}

static inline globalDataStorage *brp_get_storage_ptr(void *storage) {
	globalDataStorage *ptr = (globalDataStorage *) storage;
	return ptr;
}

// обнуление указанного участка памяти
static inline void brp_zero_mem(void *ptr, long size) {
	long index = size / sizeof(long);
	long index_last = size % sizeof(long);
	long *main_ptr = (long *) ptr;
	while (index) {
		*main_ptr = 0;
		index--;
		main_ptr++;
	}
	char *ptr_last = (char *) ((long *) ptr + size / sizeof(long));
	while (index_last) {
		*ptr_last = 0;
		index_last--;
		ptr_last++;
	}
}

static inline void brp_set_sign(dataChunk *ptr) {
	ptr->sign[0] = C_SIGN_0;
	ptr->sign[1] = C_SIGN_1;
}

static inline void brp_set_used(dataChunk *ptr) {
	ptr->status = C_USED;
}

static inline void brp_set_free(dataChunk *ptr) {
	ptr->status = C_FREE;
}

static inline int brp_set_is_sign(dataChunk *ptr) {
	return (ptr->sign[0] == C_SIGN_0 && ptr->sign[1] == C_SIGN_1);
}

static inline int brp_is_end_chank(globalDataStorage *storage, dataChunk *ptr) {
	char *end_of_array = (char *) storage + storage->array_size;
	if ((char *) ptr == end_of_array) {
		return 1;
	}
	char *next_elem_propose = (char *) ptr + sizeof(dataChunk) + ptr->size;
	if (next_elem_propose >= end_of_array
			|| next_elem_propose < (char *) storage) {
		return 2;
	} else if ((end_of_array - next_elem_propose) < (sizeof(dataChunk) + 1)) {
		return 1;
	} else if (brp_set_is_sign((dataChunk *) next_elem_propose)) {
		return 0;
	} else {
		return 1;
	}
}

static inline void *brp_find_next_elem(globalDataStorage *storage,
		dataChunk *ptr) {
	char *next_elem_propose = (char *) ptr + sizeof(dataChunk) + ptr->size;
	if (!brp_is_end_chank(storage, ptr)) {
		return (void *) next_elem_propose;
	} else {
		return (void *) ((char *) storage + sizeof(globalDataStorage));
	}
}

static inline void brp_try_concat_next(globalDataStorage *storage,
		dataChunk *ptr) {
	dataChunk *ptr_old = ptr;
	if (ptr->status == C_FREE) {
		while (1) {
			int end_chunk = brp_is_end_chank(storage, ptr);
			if (end_chunk) {
				if ((ptr != ptr_old) && (ptr->status == C_FREE)
						&& (end_chunk != 2)) {
					ptr_old->size += ptr->size + sizeof(dataChunk);
				}
				break;
			}

			ptr = brp_find_next_elem(storage, ptr);
			if (ptr->status == C_FREE) {
				ptr_old->size += ptr->size + sizeof(dataChunk);
			} else {
				break;
			}
		}

	}
}

static inline void brp_mem_cpy(char *src, char *dst, long size) {
	long index = size / sizeof(long);
	long index_last = size % sizeof(long);
	long *main_ptr = (long *) src;
	long *dst_ptr = (long *) dst;
	while (index) {
		*dst_ptr = *main_ptr;
		index--;
		main_ptr++;
		dst_ptr++;
	}
	char *ptr_last = (char *) ((long *) src + size / sizeof(long));
	char *dst_last = (char *) ((long *) dst + size / sizeof(long));
	while (index_last) {
		*dst_last = *ptr_last;
		index_last--;
		ptr_last++;
		dst_last++;
	}
}

int brp_get_alloc_alg(void *storage) {
	globalDataStorage *ds = brp_get_storage_ptr(storage);
	if (ds->sign != S_SIGN)
		return -1;
	else
		return ds->allocalg;
}

// функция для инициализации нового массива
// array_ptr - указатель на массив
// array_size -размер массива
// return: 0 - инициализация прошла успешно, -1 - инициализация провалена
int brp_malloc_init_1(void *array_ptr, long array_size, int allocalg) {
	lock();
	if (array_size < (sizeof(globalDataStorage) + sizeof(dataChunk) + 1)) {
		unlock();
		return -1;
	}
	globalDataStorage *storage = (globalDataStorage *) array_ptr;
	if (storage->sign != S_SIGN) {
		brp_zero_mem(array_ptr, array_size);
		storage->array_size = array_size;
		storage->next_elem = NULL;
		storage->allocalg = allocalg;
		storage->sign = S_SIGN;
		storage->pointers_table = NULL;
		storage->max_number_of_pointers = 0;
	}
	unlock();
	return 0;
}

// функция выделения памяти
// storage указатель на хранилище
// size - размер выделяемого куска
// return: NULL - при неудаче и указатель в случае успеха
static void *brp_malloc_internal(void *storage, long size, int need_lock) {
	if (need_lock) {
		lock();
	}
	globalDataStorage *ds = brp_get_storage_ptr(storage);
	if (ds->next_elem) {
		dataChunk *ch_ptr = (dataChunk *) ds->next_elem;
		int correct_elem_found = 0;
		if (brp_set_is_sign(ch_ptr)) {
			correct_elem_found = 1;
		} else {
			ch_ptr =
					(dataChunk *) ((char *) storage + sizeof(globalDataStorage));
			if (brp_set_is_sign(ch_ptr)) {
				correct_elem_found = 1;
			}
		}
		if (correct_elem_found) {
			int found = 0;
			void *ptr = NULL;
			dataChunk *old_ch_ptr = ch_ptr;
			int retry = 0;
			while (!found) {
				if (ch_ptr->status == C_FREE) {
					if (ch_ptr->size == size) {
						ptr = (void *) ((char *) ch_ptr + sizeof(dataChunk));
						ds->next_elem = brp_find_next_elem(ds, ch_ptr);
						brp_set_used(ch_ptr);
						found = 1;
					} else if (ch_ptr->size > size) {
						long lv_size = ch_ptr->size - size;
						if (lv_size >= (sizeof(dataChunk) + 1)) {
							dataChunk *new_ch_ptr =
									(dataChunk *) ((char *) ch_ptr
											+ sizeof(dataChunk) + size);
							brp_set_sign(new_ch_ptr);
							brp_set_free(new_ch_ptr);
							new_ch_ptr->size = lv_size - sizeof(dataChunk);
							ch_ptr->size = size;
						}
						ptr = (void *) ((char *) ch_ptr + sizeof(dataChunk));
						brp_set_used(ch_ptr);
						ds->next_elem = brp_find_next_elem(ds, ch_ptr);
						found = 1;
					} else {
						brp_try_concat_next(ds, ch_ptr);
						if (ch_ptr->size >= size) {
							retry = 1;
						}
					}
				}
				if (!retry) {
					ch_ptr = brp_find_next_elem(ds, ch_ptr);
					if (!found && (old_ch_ptr == ch_ptr)) {
						found = 1;
					}
				}
				retry = 0;
			}
			if (need_lock) {
				unlock();
			}
			return ptr;
		} else {
			if (need_lock) {
				unlock();
			}
			return NULL;
		}
	} else {
		if (size
				== (ds->array_size - sizeof(globalDataStorage)
						- sizeof(dataChunk))) {
			char *ptr = ((char *) storage + sizeof(globalDataStorage));
			dataChunk *ch_ptr = (dataChunk *) ptr;
			brp_set_sign(ch_ptr);
			brp_set_used(ch_ptr);
			ch_ptr->size = size;
			ptr = ptr + sizeof(dataChunk);
			ds->next_elem = (void *) ch_ptr;
			if (need_lock) {
				unlock();
			}
			return ptr;
		} else if (size
				> (ds->array_size - sizeof(globalDataStorage)
						- sizeof(dataChunk))) {
			if (need_lock) {
				unlock();
			}
			return NULL;
		} else {
			long lv_size = ds->array_size - sizeof(globalDataStorage)
					- sizeof(dataChunk) - size;
			char *ptr = ((char *) storage + sizeof(globalDataStorage));
			dataChunk *ch_ptr = (dataChunk *) ptr;
			brp_set_sign(ch_ptr);
			brp_set_used(ch_ptr);
			ch_ptr->size = size;
			ptr = ptr + sizeof(dataChunk);
			if (lv_size >= (sizeof(dataChunk) + 1)) {
				//создадим второй кусок, который будет свободным
				dataChunk *new_ch_ptr = (dataChunk *) ((char *) ch_ptr
						+ sizeof(dataChunk) + size);
				brp_set_sign(new_ch_ptr);
				brp_set_free(new_ch_ptr);
				new_ch_ptr->size = lv_size - sizeof(dataChunk);
				ds->next_elem = new_ch_ptr;
				if (need_lock) {
					unlock();
				}
				return ptr;
			} else {
				ds->next_elem = (void *) ch_ptr;
			}
			if (need_lock) {
				unlock();
			}
			return ptr;
		}
	}
	if (need_lock) {
		unlock();
	}
	return NULL;
}

void *brp_malloc_1(void *storage, long size) {
	return brp_malloc_internal(storage, size, 1);
}

// функция освобождения блока памяти
// ptr - указатель на блок
// storage указатель на хранилище
static void brp_free_internal(void *storage, void *ptr, int need_lock) {
	if (!ptr)
		return;
	if (need_lock) {
		lock();
	}
	char *try_to_check_info = ptr - sizeof(dataChunk);
	if (try_to_check_info < ((char *) storage + sizeof(globalDataStorage))) {
		fprintf(stderr, "%p corrupted, out of storage\n", ptr);
	}
	dataChunk *ch_ptr = (dataChunk *) try_to_check_info;
	if (brp_set_is_sign(ch_ptr)) {
		if (ch_ptr->status == C_FREE) {
			fprintf(stderr, "%p is freed earlier\n", ptr);
		} else {
			brp_set_free(ch_ptr);
		}
	} else {
		fprintf(stderr, "%p is garbage\n", ptr);
	}
	if (need_lock) {
		unlock();
	}
}

void brp_free_1(void *storage, void *ptr) {
	brp_free_internal(storage, ptr, 1);

}

// функция выделения нового участка памяти
// старый участок удаляется, а в новый копируется его содержимое
// если новый размер меньше прежнего, то данные из старого урезаются
// storage указатель на хранилище
void *brp_realloc_1(void *storage, void *ptr, long new_size) {
	lock();
	if (!ptr) {
		void *new_ptr = brp_malloc_internal(storage, new_size, 0);
		unlock();
		return new_ptr;
	} else {
		char *try_to_check_info = ptr - sizeof(dataChunk);
		if (try_to_check_info
				< ((char *) storage + sizeof(globalDataStorage))) {
			fprintf(stderr, "%p corrupted, out of storage\n", ptr);
		}
		dataChunk *ch_ptr = (dataChunk *) try_to_check_info;
		if (brp_set_is_sign(ch_ptr)) {
			if (ch_ptr->status == C_FREE) {
				fprintf(stderr, "%p is freed earlier\n", ptr);
			} else {
				void *new_ptr = brp_malloc_internal(storage, new_size, 0);
				if (new_ptr) {
					long copy_size =
							(ch_ptr->size < new_size) ? ch_ptr->size : new_size;
					brp_mem_cpy((char *) ptr, (char *) new_ptr, copy_size);
					brp_free_internal(storage, ptr, 0);
					unlock();
					return new_ptr;
				}
			}
		} else {
			fprintf(stderr, "%p is garbage\n", ptr);
		}
	}
	unlock();
	return NULL;
}

// выделение участка памяти в elem_numbers * elem_size
// и обнуление выделенного участка
// storage указатель на хранилище
static void *brp_calloc_internal(void *storage, int elem_numbers, int elem_size,
		int need_lock) {
	if (need_lock) {
		lock();
	}
	void *new_ptr = brp_malloc_internal(storage, elem_numbers * elem_size, 0);
	if (new_ptr) {
		brp_zero_mem(new_ptr, elem_numbers * elem_size);
	}
	if (need_lock) {
		unlock();
	}
	return new_ptr;
}

void *brp_calloc_1(void *storage, int elem_numbers, int elem_size) {
	return brp_calloc_internal(storage, elem_numbers, elem_size, 1);
}

// функция аналогична free, но с обнулением освободившегося участка
static void brp_free_null_internal(void *storage, void *ptr, int need_lock) {
	if (!ptr)
		return;
	if (need_lock) {
		lock();
	}
	char *try_to_check_info = ptr - sizeof(dataChunk);
	if (try_to_check_info < ((char *) storage + sizeof(globalDataStorage))) {
		fprintf(stderr, "%p corrupted, out of storage\n", ptr);
		abort();
	}
	dataChunk *ch_ptr = (dataChunk *) try_to_check_info;
	if (brp_set_is_sign(ch_ptr)) {
		if (ch_ptr->status == C_FREE) {
			fprintf(stderr, "%p is freed earlier\n", ptr);
			abort();
		} else {
			brp_zero_mem(ptr, ch_ptr->size);
			brp_set_free(ch_ptr);
		}
	} else {
		fprintf(stderr, "%p is garbage\n", ptr);
		abort();
	}
	if (need_lock) {
		unlock();
	}
	return;
}

void brp_free_null_1(void *storage, void *ptr) {
	brp_free_null_internal(storage, ptr, 1);
}

// функция подобна realloc, но с обнулением освобожденного участка
// и с обнулением нового перед копированием
void *brp_realloc_null_1(void *storage, void *ptr, long new_size) {
	lock();
	if (!ptr) {
		void *new_ptr = brp_calloc_internal(storage, 1, new_size, 0);
		unlock();
		return new_ptr;
	} else {
		char *try_to_check_info = ptr - sizeof(dataChunk);
		if (try_to_check_info
				< ((char *) storage + sizeof(globalDataStorage))) {
			fprintf(stderr, "%p corrupted, out of storage\n", ptr);
		}
		dataChunk *ch_ptr = (dataChunk *) try_to_check_info;
		if (brp_set_is_sign(ch_ptr)) {
			if (ch_ptr->status == C_FREE) {
				fprintf(stderr, "%p is freed earlier\n", ptr);
			} else {
				void *new_ptr = brp_calloc_internal(storage, 1, new_size, 0);
				if (new_ptr) {
					long copy_size =
							(ch_ptr->size < new_size) ? ch_ptr->size : new_size;
					brp_mem_cpy((char *) ptr, (char *) new_ptr, copy_size);
					brp_free_null_internal(storage, ptr, 0);
					unlock();
					return new_ptr;
				}
			}
		} else {
			fprintf(stderr, "%p is garbage\n", ptr);
		}
	}
	unlock();
	return NULL;
}

// функция ищет следующий указатель на выделенную область за переданным
// в переменной info передается указатель на информацию о куске
// если *info = 1 и return = NULL, то ptr - это неверный указатель
// если *info = NULL и return = NULL, то закончились участки
static void *brp_get_next_region_info_internal(void *storage, void *ptr,
		void **info, int need_lock) {
	if (need_lock) {
		lock();
	}
	globalDataStorage *ds = brp_get_storage_ptr(storage);
	if (ptr == NULL) {
		dataChunk *ch_ptr = (dataChunk *) ((char *) storage
				+ sizeof(globalDataStorage));
		if (!brp_set_is_sign(ch_ptr)) {
			*info = (void *) 1;
			if (need_lock) {
				unlock();
			}
			return NULL;
		}
		*info = (void *) ch_ptr;
		if (need_lock) {
			unlock();
		}
		return (void *) ((char *) ch_ptr + sizeof(dataChunk));
	} else {
		char *try_to_check_info = ptr - sizeof(dataChunk);
		if (try_to_check_info
				< ((char *) storage + sizeof(globalDataStorage))) {
			*info = (void *) 1;
			if (need_lock) {
				unlock();
			}
			return NULL;
		}
		dataChunk *ch_ptr = (dataChunk *) try_to_check_info;
		if (brp_set_is_sign(ch_ptr)) {
			ch_ptr = brp_find_next_elem(ds, ch_ptr);
			*info = (void *) ch_ptr;
			if (need_lock) {
				unlock();
			}
			return (void *) ((char *) ch_ptr + sizeof(dataChunk));
		} else {
			*info = (void *) 1;
			if (need_lock) {
				unlock();
			}
			return NULL;
		}
	}

	if (need_lock) {
		unlock();
	}
	return NULL;
}

void *brp_get_next_region_info_1(void *storage, void *ptr, void **info) {
	return brp_get_next_region_info_internal(storage, ptr, info, 1);
}

// функция возвращает рисунок областей аллокаций
// размер буффера должен быть достаточным для сохранения рисунка
// иначе, рисунок не будет содержать верной картины
// F - свободный участок, U - занятый участок
void brp_return_allocation_picture_1(void *storage, char *buffer_for_picture,
		long size) {
	lock();
	int index = 0;
	globalDataStorage *ds = brp_get_storage_ptr(storage);
	dataChunk *first_ptr = NULL;
	dataChunk *next_ptr = NULL;
	brp_get_next_region_info_internal(storage, NULL, (void **) &first_ptr, 0);
	while (next_ptr != first_ptr) {
		if (!next_ptr) {
			next_ptr = first_ptr;
		}
		buffer_for_picture[index] = next_ptr->status;
		brp_get_next_region_info_internal(storage,
				(void *) ((char *) next_ptr + sizeof(dataChunk)),
				(void **) &next_ptr, 0);
		index++;
		if (index >= (size - 2)) {
			break;
		}
	}
	buffer_for_picture[index] = 0;
	unlock();
	return;
}

// функция аналогична предыдущей, но информация о кусках
// сразу выводится на экран. Для отладочных целей
void brp_return_allocation_stdout_1(void *storage) {
	lock();
	int index = 0;
	globalDataStorage *ds = brp_get_storage_ptr(storage);
	dataChunk *first_ptr = NULL;
	dataChunk *next_ptr = NULL;
	brp_get_next_region_info_internal(storage, NULL, (void *) &first_ptr, 0);
	long sum_size = 0;
	long chunk_size = 0;
	while (next_ptr != first_ptr) {
		if (!next_ptr) {
			next_ptr = first_ptr;
		}
		printf("%d) STATUS %c SIZE %d ST %d DC %d\n", index + 1,
				next_ptr->status, next_ptr->size, sizeof(globalDataStorage),
				sizeof(dataChunk));
		sum_size += next_ptr->size;
		chunk_size += next_ptr->size + sizeof(dataChunk);
		brp_get_next_region_info_internal(storage,
				(void *) ((char *) next_ptr + sizeof(dataChunk)),
				(void *) &next_ptr, 0);
		index++;
	}
	printf("----> SIZE %d DIRTY %d\n", sum_size,
			sizeof(globalDataStorage) + chunk_size);
	unlock();
	return;
}

// получить значение поля next_elem
// если он = NULL, значит в массиве ни разу не выделяли память
void *brp_get_next_elem_1(void *storage) {
	void *ptr = NULL;
	lock();
	ptr = brp_get_storage_ptr(storage)->next_elem;
	unlock();
	return ptr;
}

long brp_get_free_1(void *storage) {
	long ptr = 0;
	lock();
	char *pointer = (char *) brp_get_storage_ptr(storage);
	dataChunk *ch_ptr = (dataChunk *) ((char *) pointer
			+ sizeof(globalDataStorage));
	if (ch_ptr->status == C_FREE) {
		ptr += ch_ptr->size;
	}
	while (!brp_is_end_chank(brp_get_storage_ptr(storage), ch_ptr)) {
		ch_ptr = brp_find_next_elem(brp_get_storage_ptr(storage), ch_ptr);
		if (ch_ptr->status == C_FREE) {
			ptr += ch_ptr->size;
		}
	}
	unlock();
	return ptr;
}

long brp_get_inuse_1(void *storage) {
	long ptr = 0;
	lock();
	char *pointer = (char *) brp_get_storage_ptr(storage);
	dataChunk *ch_ptr = (dataChunk *) ((char *) pointer
			+ sizeof(globalDataStorage));
	if (ch_ptr->status == C_USED) {
		ptr += ch_ptr->size;
	}
	while (!brp_is_end_chank(brp_get_storage_ptr(storage), ch_ptr)) {
		ch_ptr = brp_find_next_elem(brp_get_storage_ptr(storage), ch_ptr);
		if (ch_ptr->status == C_USED) {
			ptr += ch_ptr->size;
		}
	}
	unlock();
	return ptr;
}

//Создание таблицы с указателями, для передаче участка памяти в другой процесс
// или в другой обработчик массива
// 0 - таблица создана, 1 - не создана
int brp_make_pointers_table_1(void *storage, int pointers_number) {
	lock();
	globalDataStorage *ds = brp_get_storage_ptr(storage);
	if (!ds->pointers_table) {
		ds->pointers_table = (void *) brp_calloc_internal(storage,
				pointers_number, sizeof(void *), 0);
		if (!ds->pointers_table) {
			unlock();
			return 1;
		}
		ds->max_number_of_pointers = pointers_number;
	}
	unlock();
	return 0;
}

void *brp_get_pointer_with_number_1(void *storage, int pointer_number) {
	lock();
	globalDataStorage *ds = brp_get_storage_ptr(storage);
	if (ds->pointers_table && (pointer_number < ds->max_number_of_pointers)) {
		void **ptr = (void **) ds->pointers_table + pointer_number;
		unlock();
		return *ptr;
	}
	unlock();
	return NULL;
}

void brp_set_pointer_to_number_1(void *storage, int pointer_number, void *value) {
	lock();
	globalDataStorage *ds = brp_get_storage_ptr(storage);
	if (ds->pointers_table && (pointer_number < ds->max_number_of_pointers)) {
		void **ptr = (void **) ds->pointers_table + pointer_number;
		*ptr = value;
	}
	unlock();
}

long brp_get_region_size_1(void *array_ptr){
	globalDataStorage *storage = (globalDataStorage *) array_ptr;
	if (storage->sign != S_SIGN) {
		return storage->array_size;
	}
	return 0;
}

