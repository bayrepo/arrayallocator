/** 
 * Copyright (c) 2014 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef BRP_MAP_H
#define BRP_MAP_H

#include <string.h>

#define BRP_MAP_VERSION "0.1.0"

struct brp_map_node_t;
typedef struct brp_map_node_t brp_map_node_t;

typedef struct {
  brp_map_node_t **buckets;
  unsigned nbuckets, nnodes;
} brp_map_base_t;

typedef struct {
  unsigned bucketidx;
  brp_map_node_t *node;
} brp_map_iter_t;


#define brp_map_t(T)\
  struct { brp_map_base_t base; T *ref; T tmp; }


#define brp_map_init(m, s)\
  memset(m, 0, sizeof(*(m)))


#define brp_map_deinit(m, s)\
  brp_map_deinit_(&(m)->base, s)


#define brp_map_get(m, key, s)\
  ( (m)->ref = brp_map_get_(&(m)->base, key, s) )


#define brp_map_set(m, key, value, s)\
  ( (m)->tmp = (value),\
    brp_map_set_(&(m)->base, key, &(m)->tmp, sizeof((m)->tmp), s) )


#define brp_map_remove(m, key, s)\
  brp_map_remove_(&(m)->base, key, s)


#define brp_map_iter(m, s)\
  brp_map_iter_(s)


#define brp_map_next(m, iter, s)\
  brp_map_next_(&(m)->base, iter, s)


void brp_map_deinit_(brp_map_base_t *m, void *storage);
void *brp_map_get_(brp_map_base_t *m, const char *key, void *storage);
int brp_map_set_(brp_map_base_t *m, const char *key, void *value, int vsize, void *storage);
void brp_map_remove_(brp_map_base_t *m, const char *key, void *storage);
brp_map_iter_t brp_map_iter_(void *storage);
const char *brp_map_next_(brp_map_base_t *m, brp_map_iter_t *iter, void *storage);


typedef brp_map_t(void*) brp_map_void_t;
typedef brp_map_t(char*) brp_map_str_t;
typedef brp_map_t(int) brp_map_int_t;
typedef brp_map_t(char) brp_map_char_t;
typedef brp_map_t(float) brp_map_float_t;
typedef brp_map_t(double) brp_map_double_t;

#endif
