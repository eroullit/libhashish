
/*
** $Id$
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

uint32_t lhi_hash_dump1(const uint8_t *, uint32_t);
uint32_t lhi_hash_phong(const uint8_t *, uint32_t);
uint32_t lhi_hash_torek(const uint8_t *, uint32_t);
uint32_t lhi_hash_weinb(const uint8_t *, uint32_t);
uint32_t lhi_hash_elf(const uint8_t *, uint32_t);
uint32_t lhi_hash_djb2(const uint8_t *, uint32_t);
uint32_t lhi_hash_xor(const uint8_t *, uint32_t);
uint32_t lhi_hash_kr(const uint8_t *, uint32_t);
uint32_t lhi_hash_sdbm(const uint8_t *, uint32_t);

#define	HI_HASH_DEFAULT lhi_hash_weinb
#define	HI_HASH_DEFAULT_HL lhi_hash_elf


#if 0
struct __hashfunc_map_t
{
	const int no;
	uint32_t (*hashfunc)(const uint8_t*, uint32_t);
} hashfunc_map[] =
{
#define	LHI_HASH_WEINB 0
	{LHI_HASH_WEINB, lhi_hash_weinb},
	{ 0, NULL },
};
#define	HI_HASH_DEFAULT LHI_HASH_WEINB /* (hashfunc_map[LHI_HASH_WEINB].hashfunc) */
#endif


/**
 * chaining identifiers
 */
enum chaining_policy {
	CHAINING_LIST,       /**< Normal list chaining */
	CHAINING_LIST_MTF,   /**< List chaining - but *M*ove searched entries up *T*o *F*ront */
	CHAINING_ARRAY,      /**< Array chaining (fixed array length, realloc possible) */
	CHAINING_DYN_ARRAY,  /**< Dynamic Array chaining - automatically resize bucket array */
	CHAINING_HASHLIST,   /**< List chaining but key compare are replaced with 2. hash function */
	CHAINING_HASHARRAY,  /**< Similar like CHAINING_HASHLIST but as array structured (some hacks possible[tm]) */
	CHAINING_RBTREE,     /**< Use red black tree as "chaining" strategie, avoid O(n) */
	__CHAINING_POLICY_MAX,
};
#define CHAINING_POLICY_MAX (__CHAINING_POLICY_MAX - 1)

/* default number of entries in every array (bucket) */
#define	DEFAULT_CHAINING_ARRAY_SIZE 20

typedef struct __hi_bucket_obj {
	uint32_t				 key_len; /* key length in bytes */
	void					*key;
	void					*data;
	struct lhi_list_head	*hi_handle;
	struct lhi_list_head	 list;
} hi_bucket_obj_t;


typedef struct __hi_bucket_hl_obj {
	uint32_t				 key_len; /* key length in bytes */
	void					*key;
	uint32_t				 key_hash;
	void					*data;
	struct lhi_list_head	*hi_handle;
	struct lhi_list_head	 list;
} hi_bucket_hl_obj_t;

/* CHAINING_ARRAY elements */
typedef struct __hi_bucket_a_obj {
	uint32_t				 key_len; /* key length in bytes */
	void					*key;
	uint32_t				 key_hash;
	void					*data;
} hi_bucket_a_obj_t;

/**
 * @immutable if set (after hash initialization) this tag the changeableility of a hi_handle
 */
typedef struct __hi_handle {

	int	immutable;
	enum chaining_policy chaining_policy;

	uint32_t (*hash)(const uint8_t *, uint32_t);
	int (*compare)(const uint8_t *, const void *);

	/* if we use CHAINING_HASHLIST chaining we MUST add a second hashing
	 * function
	 */
	uint32_t (*listhash)(const char *, uint32_t);

	int	size;
	unsigned int buckets;

	unsigned int *bucket_size;

	/* CHAINING_LIST */
	struct lhi_list_head *bucket_table;

	/* CHAINING_ARRAY */
	hi_bucket_a_obj_t **bucket_array;
	unsigned int	   *bucket_array_slot_size;
	unsigned int	   *bucket_array_slot_max;

	/* CHAINING_RBTREE */
	struct rb_root *rb_root;
	struct my_stuff * (*rb_search)(struct rb_root *, void *);
	void (*rb_insert)(struct rb_root *, void *);

	pthread_mutex_t mutex_lock;

} hi_handle_t;



/* public interface (with a leading hi_)*/

int hi_fini(hi_handle_t *);
int hi_create(hi_handle_t **, int,
		int (*compare)(const void *, const void *),
		unsigned int (*hashf)(const void *, unsigned int),
		unsigned int (*hashf2)(const void *, unsigned int),
		struct my_stuff * (*rb_search)(struct rb_root *, void *),
		void (*rb_insert)(struct rb_root *, void *),
		enum chaining_policy);

int hi_insert(hi_handle_t *, void *, uint32_t, void *);
int hi_get(hi_handle_t *, void *, uint32_t, void **);
int hi_remove(hi_handle_t *, void *, uint32_t, void **);

int lhi_bucket_remove(hi_handle_t *, unsigned int);

/* compare functions */
int hi_cmp_str(const uint8_t *key1, const uint8_t *key2);


#define	HI_CMP_STR hi_cmp_str


/* string macros */
#define hi_init_str(hi_hndl, buckets)  hi_create(hi_hndl, buckets, HI_CMP_STR, HI_HASH_DEFAULT, NULL, NULL, NULL, CHAINING_LIST)
#define hi_init_str_hl(hi_hndl, buckets)  hi_create(hi_hndl, buckets, HI_CMP_STR, HI_HASH_DEFAULT, HI_HASH_DEFAULT_HL, NULL, NULL, CHAINING_HASHLIST)
#define hi_init_str_lmtf(hi_hndl, buckets)  hi_create(hi_hndl, buckets, HI_CMP_STR, HI_HASH_DEFAULT, HI_HASH_DEFAULT_HL, NULL, NULL, CHAINING_LIST_MTF)
#define hi_init_str_ar(hi_hndl, buckets)  hi_create(hi_hndl, buckets, HI_CMP_STR, HI_HASH_DEFAULT, NULL, NULL, NULL, CHAINING_ARRAY)

#define	hi_insert_str(hi_handle, key, data) hi_insert(hi_handle, (void *)key, strlen(key), data)
#define	hi_get_str(hi_handle, key, data)  hi_get(hi_handle, (void *)key, strlen(key), data)
#define	hi_remove_str(hi_handle, key, data)  hi_remove(hi_handle, (void *)key, strlen(key), data)

/* miscellaneous helper macros */
#define	hi_size(hi_handle) ((hi_handle)->size)
#define	hi_buckets(hi_handle) ((hi_handle)->buckets)
#define	hi_bucket_size(hi_handle, bucket_index) ((hi_handle->bucket_size[bucket_index]))
#define	hi_is_immutable(hi_handle) ((hi_handle)->immutable)

#define SUCCESS 0
#define	FAILURE -1


/* PRIVATE INTERFACE (with a leading lhi_ - use it if you need some special
 * functionality
 */

int lhi_create_handle(hi_handle_t **);
int lhi_lookup(const hi_handle_t *, void *, uint32_t);
void lhi_bucket_obj_remove(hi_bucket_obj_t *);

#define	lhi_set_immutable(hi_handle) ((hi_handle)->immutable = 1)
#define	lhi_set_mutable(hi_handle) ((hi_handle)->immutable = 0)

#define	lhi_setsize(hi_handle, size) ((hi_handle)->size = size)
#define	lhi_sethashfunc(hi_handle, hashfunc) ((hi_handle)->hash = hashfunc)

#define	hi_setkeytype(hi_handle, keytype) ((hi_handle)->key_type = key_type)

#ifdef __cplusplus
}
#endif

#endif /* _LIB_HASHISH_H */

/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
