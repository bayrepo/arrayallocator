/** 
 * Copyright (c) 2014 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "bayrepomalloc.h"
#include "bayreposbrk.h"
#include "bayrepostrings.h"
#include "bayrepodump.h"


struct brp_map_node_t {
	unsigned hash;
	void *value;
	brp_map_node_t *next;
/* char key[]; */
/* char value[]; */
};

static unsigned brp_map_hash(const char *str, void *storage) {
	unsigned hash = 5381;
	while (*str) {
		hash = ((hash << 5) + hash) ^ *str++;
	}
	return hash;
}

static brp_map_node_t *brp_map_newnode(const char *key, void *value, int vsize, void *storage) {
	brp_map_node_t *node;
	int ksize = strlen(key) + 1;
	int voffset = ksize + ((sizeof(void*) - ksize) % sizeof(void*));
	node = brp_malloc(storage, sizeof(*node) + voffset + vsize);
	if (!node)
		return NULL;
	memcpy(node + 1, key, ksize);
	node->hash = brp_map_hash(key, storage);
	node->value = ((char*) (node + 1)) + voffset;
	memcpy(node->value, value, vsize);
	return node;
}

static int brp_map_bucketidx(brp_map_base_t *m, unsigned hash, void *storage) {
	/* If the implementation is changed to allow a non-power-of-2 bucket count,
	 * the line below should be changed to use mod instead of AND */
	return hash & (m->nbuckets - 1);
}

static void brp_map_addnode(brp_map_base_t *m, brp_map_node_t *node, void *storage) {
	int n = brp_map_bucketidx(m, node->hash, storage);
	node->next = m->buckets[n];
	m->buckets[n] = node;
}

static int brp_map_resize(brp_map_base_t *m, int nbuckets, void *storage) {
	brp_map_node_t *nodes, *node, *next;
	brp_map_node_t **buckets;
	int i;
	/* Chain all nodes together */
	nodes = NULL;
	i = m->nbuckets;
	while (i--) {
		node = (m->buckets)[i];
		while (node) {
			next = node->next;
			node->next = nodes;
			nodes = node;
			node = next;
		}
	}
	/* Reset buckets */
	buckets = brp_realloc(storage, m->buckets, sizeof(*m->buckets) * nbuckets);
	if (buckets != NULL) {
		m->buckets = buckets;
		m->nbuckets = nbuckets;
	}
	if (m->buckets) {
		memset(m->buckets, 0, sizeof(*m->buckets) * m->nbuckets);
		/* Re-add nodes to buckets */
		node = nodes;
		while (node) {
			next = node->next;
			brp_map_addnode(m, node, storage);
			node = next;
		}
	}
	/* Return error code if realloc() failed */
	return (buckets == NULL) ? -1 : 0;
}

static brp_map_node_t **brp_map_getref(brp_map_base_t *m, const char *key, void *storage) {
	unsigned hash = brp_map_hash(key, storage);
	brp_map_node_t **next;
	if (m->nbuckets > 0) {
		next = &m->buckets[brp_map_bucketidx(m, hash, storage)];
		while (*next) {
			if ((*next)->hash == hash && !strcmp((char*) (*next + 1), key)) {
				return next;
			}
			next = &(*next)->next;
		}
	}
	return NULL;
}

void brp_map_deinit_(brp_map_base_t *m, void *storage) {
	brp_map_node_t *next, *node;
	int i;
	i = m->nbuckets;
	while (i--) {
		node = m->buckets[i];
		while (node) {
			next = node->next;
			brp_free(storage, node);
			node = next;
		}
	}
	brp_free(storage, m->buckets);
}

void *brp_map_get_(brp_map_base_t *m, const char *key, void *storage) {
	brp_map_node_t **next = brp_map_getref(m, key, storage);
	return next ? (*next)->value : NULL;
}

int brp_map_set_(brp_map_base_t *m, const char *key, void *value, int vsize, void *storage) {
	int n, err;
	brp_map_node_t **next, *node;
	/* Find & replace existing node */
	next = brp_map_getref(m, key, storage);
	if (next) {
		memcpy((*next)->value, value, vsize);
		return 0;
	}
	/* Add new node */
	node = brp_map_newnode(key, value, vsize, storage);
	if (node == NULL)
		goto fail;
	if (m->nnodes >= m->nbuckets) {
		n = (m->nbuckets > 0) ? (m->nbuckets << 1) : 1;
		err = brp_map_resize(m, n, storage);
		if (err)
			goto fail;
	}
	brp_map_addnode(m, node, storage);
	m->nnodes++;
	return 0;
	fail: if (node)
		brp_free(storage, node);
	return -1;
}

void brp_map_remove_(brp_map_base_t *m, const char *key, void *storage) {
	brp_map_node_t *node;
	brp_map_node_t **next = map_getref(m, key, storage);
	if (next) {
		node = *next;
		*next = (*next)->next;
		brp_free(storage, node);
		m->nnodes--;
	}
}

brp_map_iter_t brp_map_iter_(void *storage) {
	brp_map_iter_t iter;
	iter.bucketidx = -1;
	iter.node = NULL;
	return iter;
}

const char *brp_map_next_(brp_map_base_t *m, brp_map_iter_t *iter, void *storage) {
	if (iter->node) {
		iter->node = iter->node->next;
		if (iter->node == NULL)
			goto nextBucket;
	} else {
		nextBucket: do {
			if (++iter->bucketidx >= m->nbuckets) {
				return NULL;
			}
			iter->node = m->buckets[iter->bucketidx];
		} while (iter->node == NULL);
	}
	return (char*) (iter->node + 1);
}
