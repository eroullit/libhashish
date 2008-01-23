/*
** $Id: libhashish.h 15 2007-08-23 15:17:56Z hgndgtl $
**
** Copyright (C) 2006 - Hagen Paul Pfeifer <hagen@jauu.net>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef _LIB_HASHISH_H
#define	_LIB_HASHISH_H

#include <inttypes.h> /* include stdint.h */
#include <stdio.h>
#include <pthread.h>
#include "list.h"
#include "rbtree.h"
#include "../config.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int lhi_errno;
extern char *lhi_errbuf;


struct hashfunc_map_t
{
	const unsigned int no;
	uint32_t (*hashfunc)(const uint8_t*, uint32_t);
};

enum hash_alg {
	HI_HASH_WEINB = 0,
	__HI_HASH_MAX
};

#define	HI_HASH_MAX (__HI_HASH_MAX - 1)

enum coll_eng {
	COLL_ENG_LIST = 1,
	COLL_ENG_LIST_HASH,
	COLL_ENG_LIST_MTF,
	COLL_ENG_LIST_MTF_HASH,
	COLL_ENG_ARRAY,
	COLL_ENG_ARRAY_HASH,
	COLL_ENG_ARRAY_DYN,
	COLL_ENG_ARRAY_DYN_HASH,
	COLL_ENG_RBTREE,
	__COLL_ENG_MAX
};

#define COLL_ENG_MAX (__COLL_ENG_MAX - 1)

/** This struct is passed to hi_init and determine
 * the hash libaray behaviour
 */
struct hi_init_set {
	uint32_t table_size;
	int self_resizing;
	uint32_t coll_eng_array_size;
	enum coll_eng coll_eng;
	uint32_t (*hash_func)(const uint8_t*, uint32_t);
	uint32_t (*hash2_func)(const uint8_t*, uint32_t);
	int (*key_cmp)(const uint8_t *, const uint8_t *);
};

 typedef struct __hi_bucket_obj {
     uint32_t                 key_len; /* key length in bytes */
     void                    *key;
     void                    *data;
     struct lhi_list_head    *hi_handle;
     struct lhi_list_head     list;
 } hi_bucket_obj_t;


 typedef struct __hi_bucket_hl_obj {
     uint32_t                 key_len; /* key length in bytes */
     void                    *key;
     uint32_t                 key_hash;
     void                    *data;
     struct lhi_list_head    *hi_handle;
     struct lhi_list_head     list;
 } hi_bucket_hl_obj_t;

 /* CHAINING_ARRAY elements */
 typedef struct __hi_bucket_a_obj {
     uint32_t                 key_len; /* key length in bytes */
     void                    *key;
     uint32_t                 key_hash;
     void                    *data;
 } hi_bucket_a_obj_t;

typedef struct __hi_handle {

	/* data from user -> hi_init_set */
	uint32_t table_size;
	enum coll_eng coll_eng;
	int self_resizing;
	uint32_t coll_eng_array_size;

	uint32_t (*hash_func)(const uint8_t*, uint32_t);
	uint32_t (*hash2_func)(const uint8_t*, uint32_t);
	int (*key_cmp)(const uint8_t *, const uint8_t *);
	struct my_stuff * (*rb_search)(struct rb_root *, void *);
	void (*rb_insert)(struct rb_root *, void *);

	/* statistic data */

	/* the current number elements in the particular bucket */
	uint32_t *bucket_size;
	/* the total amount of entries in the whole table */
	uint32_t no_objects;

	/* collision engine specific data */
	union {
		struct {
			struct lhi_list_head *bucket_table;
		} eng_list;
		struct {
			hi_bucket_a_obj_t **bucket_array;
			uint32_t    *bucket_array_slot_size;
			uint32_t    *bucket_array_slot_max;
		} eng_array;
		struct {
			struct rb_root *rb_root;
		} eng_rbtree;
	};

	/* thread locking stuff */
	pthread_mutex_t *mutex_lock;

} hi_handle_t;


/* hi_fini.c */
int hi_fini(hi_handle_t *);

/* libhashish.c */
int hi_create(hi_handle_t **, struct hi_init_set *);

/* hi_set.c */
void hi_set_zero(struct hi_init_set *);
int hi_set_bucket_size(struct hi_init_set *, uint32_t);
int hi_set_hash_alg(struct hi_init_set *, enum hash_alg);
int hi_set_hash_func(struct hi_init_set *, uint32_t (*hash_func)(const uint8_t*, uint32_t));
int hi_set_hash2_alg(struct hi_init_set *, enum hash_alg);
int hi_set_hash2_func(struct hi_init_set *, uint32_t (*hash_func)(const uint8_t*, uint32_t));
int hi_set_coll_eng(struct hi_init_set *, enum coll_eng);
int hi_set_key_cmp_func(struct hi_init_set *, int (*cmp)(const uint8_t *, const uint8_t *));
void hi_set_self_resizing(struct hi_init_set *, int);
int hi_set_coll_eng_array_size(struct hi_init_set *, uint32_t);

/* xutils.c */
void hi_perror(const char *);
const char *hi_geterror(void);

/* cmp_funcs.c */
int hi_cmp_str(const uint8_t *, const uint8_t *);
int hi_cmp_int32(const uint8_t *, const uint8_t *);

/* hi_operations */
int hi_lookup(const hi_handle_t *, void *, uint32_t);
int hi_insert(const hi_handle_t *, void *, uint32_t, void *);
int hi_get(const hi_handle_t *, void *, uint32_t, void **);
int hi_remove(const hi_handle_t *, void *, uint32_t, void **);


/* helper function section */

static inline uint32_t hi_no_objects(const hi_handle_t *h)
{
	return h->no_objects;
}


#ifdef __cplusplus
}
#endif

#endif /* _LIB_HASHISH_H */

/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
