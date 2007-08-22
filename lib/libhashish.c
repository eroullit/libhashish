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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>

#include "libhashish-local.h"
#include "libhashish.h"
#include "list.h"
#include "threads.h"

/**
 * This is the default initialize function. It takes HI_HASH_DEFAULT as the
 * default hash function and set compare function for strings - so use it only
 * for strings
 *
 * @arg hi_hndl	this become out new hashish handle
 * @arg buckets	hash bucket size
 * @returns negativ error value or zero on success
 */
int hi_init(hi_handle_t **hi_hndl, int buckets)
{
	int i, ret;
	hi_handle_t *hi_handle;

	ret = XMALLOC((void **)&hi_handle, sizeof(hi_handle_t));
	if (ret != 0) {
		return hi_errno(errno);
	}

	memset(hi_handle, 0, sizeof(hi_handle_t));

	ret = XMALLOC((void **) &hi_handle->bucket_table,
			buckets * sizeof(struct lhi_list_head));
	if (ret != 0) {
		return hi_errno(errno);
	}

	ret = XMALLOC((void **) &hi_handle->bucket_size,
			buckets * sizeof(*hi_handle->bucket_size));
	if (ret != 0) {
		return hi_errno(errno);
	}

	/* initialize bucket list */
	for (i = 0; i < buckets; i++) {
		lhi_init_list_head(&(hi_handle->bucket_table[i]));
		hi_handle->bucket_size[i] = 0;
	}

	hi_handle->buckets = buckets;

	/* set default hash algorithm */
	hi_handle->hash = HI_HASH_DEFAULT;

	/* set default compare function - string */
	hi_handle->compare = hi_cmp_str;

	hi_handle->size = 0;

	/* our hash table isn't mutable since here */
	lhi_set_immutable(hi_handle);

	/* last but not least: bend our pointer */
	*hi_hndl = hi_handle;

	return SUCCESS;
}

int lhi_create_handle(hi_handle_t **hi_hndl)
{
	int ret;
	hi_handle_t *hi_handle;

	ret = XMALLOC((void **) &hi_handle, sizeof(hi_handle_t));
	if (ret != 0) {
		return hi_errno(errno);
	}

	memset(hi_handle, 0, sizeof(hi_handle_t));

	*hi_hndl = hi_handle;

	return SUCCESS;
}

int hi_create(hi_handle_t **hi_hndl, int buckets,
		int (*compare)(const void *key1, const void *key2),
		unsigned int (*hashf1)(const void *key, unsigned int len),
		unsigned int (*hashf2)(const void *key, unsigned int len),
		enum chaining_policy chaining_policy)
{
	int i, ret;
	hi_handle_t *hi_handle;

	ret = XMALLOC((void **) &hi_handle, sizeof(hi_handle_t));
	if (ret != 0) {
		return hi_errno(errno);
	}

	memset(hi_handle, 0, sizeof(hi_handle_t));

	ret = XMALLOC((void **) &hi_handle->bucket_size,
			buckets * sizeof(*hi_handle->bucket_size));
	if (ret != 0) {
		return hi_errno(errno);
	}

	switch (chaining_policy) {

		case CHAINING_LIST:
		case CHAINING_LIST_MTF:
			ret = XMALLOC((void **) &hi_handle->bucket_table,
					buckets * sizeof(struct lhi_list_head));
			if (ret != 0) {
				return hi_errno(errno);
			}
			/* initialize bucket list */
			for (i = 0; i < buckets; i++) {
				lhi_init_list_head(&(hi_handle->bucket_table[i]));
				hi_handle->bucket_size[i] = 0;
			}
			break;

		case CHAINING_HASHLIST:
			ret = XMALLOC((void **) &hi_handle->bucket_table,
					buckets * sizeof(struct lhi_list_head));
			if (ret != 0) {
				return hi_errno(errno);
			}
			/* initialize bucket list */
			for (i = 0; i < buckets; i++) {
				lhi_init_list_head(&(hi_handle->bucket_table[i]));
				hi_handle->bucket_size[i] = 0;
			}
			/* table hash function MUST be different from the entry hash function */
			if (hashf1 == hashf2) {
				return hi_error(EINVAL, "hashfunction 2 must be different from thirst hashfunc");
			}
			hi_handle->listhash = hashf2;
			break;

		case CHAINING_ARRAY:

			ret = XMALLOC((void **) &hi_handle->bucket_array_slot_size,
					sizeof(unsigned int) * buckets);
			if (ret != 0)
				return hi_errno(errno);

			ret = XMALLOC((void **) &hi_handle->bucket_array_slot_max,
					sizeof(unsigned int) * buckets);
			if (ret != 0)
				return hi_errno(errno);

			ret = XMALLOC((void **) &hi_handle->bucket_array,
					(sizeof(hi_bucket_a_obj_t *) * buckets));
			if (ret != 0)
				return hi_errno(errno);

			for (i = 0; i < buckets; i++) {
				/* align array on 16 byte boundaries */
				ret = xalloc_align((void **) &hi_handle->bucket_array[i], 16,
						(sizeof(hi_bucket_a_obj_t) * DEFAULT_CHAINING_ARRAY_SIZE));
				if (ret != 0)
					return hi_errno(errno);

				hi_handle->bucket_array_slot_size[i] = 0;
				hi_handle->bucket_array_slot_max[i] = DEFAULT_CHAINING_ARRAY_SIZE;
			}
			break;

		default:
			return hi_error(EINVAL, "chaining strategie not supported (values: %d)",
					chaining_policy);
			break;
	}

	hi_handle->chaining_policy = chaining_policy;

	hi_handle->buckets = buckets;

	hi_handle->hash = hashf1;

	hi_handle->compare = compare;

	hi_handle->size = 0;

	lhi_pthread_init(&hi_handle->mutex_lock, NULL);

	/* our hash table isn't mutable since here */
	lhi_set_immutable(hi_handle);

	/* last but not least: bend our pointer */
	*hi_hndl = hi_handle;


	return SUCCESS;
}

int lhi_lookup(const hi_handle_t *hi_handle, void *key, uint32_t keylen)
{
	int bucket;

	bucket = hi_handle->hash(key, keylen) % hi_handle->buckets;

	switch (hi_handle->chaining_policy) {
		case CHAINING_LIST:
		case CHAINING_LIST_MTF:
			{
			hi_bucket_obj_t *b_obj;
			lhi_pthread_lock(&hi_handle->mutex_lock);
			lhi_list_for_each_entry(b_obj, &(hi_handle->bucket_table[bucket]), list) {
				if (hi_handle->compare(key, b_obj->key)) {
			lhi_pthread_unlock(&hi_handle->mutex_lock);
					return SUCCESS;
				}
			}
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			}
			return FAILURE;
			break;
		case CHAINING_HASHLIST:
			{
			hi_bucket_hl_obj_t *b_obj;
			uint32_t key_hash = hi_handle->listhash(key, keylen);
			lhi_pthread_lock(&hi_handle->mutex_lock);
			lhi_list_for_each_entry(b_obj, &(hi_handle->bucket_table[bucket]), list) {
				if (key_hash == b_obj->key_hash &&
					hi_handle->compare(key, b_obj->key)) {
			lhi_pthread_unlock(&hi_handle->mutex_lock);
					return SUCCESS;
				}
			}
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			}
			return FAILURE;
			break;

		case CHAINING_ARRAY:
			{
			unsigned int i;
			lhi_pthread_lock(&hi_handle->mutex_lock);
			for (i = 0; i < hi_handle->bucket_array_slot_size[bucket]; i++) {
				if (hi_handle->compare(key, hi_handle->bucket_array[bucket][i].key)) {
			lhi_pthread_unlock(&hi_handle->mutex_lock);
					return SUCCESS;
				}
			}
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			}
			return FAILURE;
			break;
		default:
			return hi_error(EINVAL, "chaining strategie not supported (values: %d)",
					hi_handle->chaining_policy);
			break;
	}
	return FAILURE;
}

/**
 * Remove a bucket from the list.
 * This function only do some list handling. It is up to the callee to
 * free this object and do some stat keeping (bucket_size, etc.).
 *
 * @arg hi_bucket_obj the object to remove from bucket list
 */
void lhi_bucket_obj_remove(hi_bucket_obj_t *hi_bucket_obj)
{
	struct lhi_list_head *hi_handle;

	hi_handle = hi_bucket_obj->hi_handle;

	if (hi_handle == NULL) {
		return;
	}

	lhi_list_del(&hi_bucket_obj->list);
	hi_bucket_obj->hi_handle = NULL;
}

/**
 * lhi_bucket_remove cleans a bucket completely. No further
 * free() is needed (and imposible ;).
 *
 * @arg hi_handle the hashish handle
 * @arg bucket_index the index to the bucket
 * @return SUCCESS or negative error value and set lhi_errno (see * hi_geterror())
 */
int lhi_bucket_remove(hi_handle_t *hi_handle, unsigned int bucket_index)
{
	struct lhi_list_head *pos;

	pos = &hi_handle->bucket_table[bucket_index];

	switch (hi_handle->chaining_policy) {
		case CHAINING_LIST:
		case CHAINING_LIST_MTF:
			{
			hi_bucket_obj_t *b_obj, *p;
			/* iterate over list and remove all entries and free hi_bucket_obj_t */
			lhi_pthread_lock(&hi_handle->mutex_lock);
			lhi_list_for_each_entry_safe(b_obj, p, pos, list) {
				lhi_list_del(&b_obj->list);
				free(b_obj);
				--hi_handle->bucket_size[bucket_index];
				--hi_handle->size;
			}
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			return SUCCESS;
			}
			break;
		case CHAINING_HASHLIST:
			{
			hi_bucket_hl_obj_t *b_obj, *p;
			lhi_pthread_lock(&hi_handle->mutex_lock);
			/* iterate over list and remove all entries and free hi_bucket_obj_t */
			lhi_list_for_each_entry_safe(b_obj, p, pos, list) {
				lhi_list_del(&b_obj->list);
				free(b_obj);
				--hi_handle->bucket_size[bucket_index];
				--hi_handle->size;
			}
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			return SUCCESS;
			}
			break;
		default:
			return FAILURE;
			return hi_error(EINVAL, "chaining strategie not supported (value: %d)",
					hi_handle->chaining_policy);
			break;
	};

	return FAILURE; /* should never happen[TM] */
}

/**
 * hi_get return for a given key the correspond data entry
 *
 * @arg hi_handle the hashish handle
 * @arg key a pointer to the key
 * @data the pointer-pointer for the returned data
 */
int hi_get(hi_handle_t *hi_handle, void *key, uint32_t keylen, void **data)
{
	int bucket = hi_handle->hash(key, keylen) % hi_handle->buckets;

	switch (hi_handle->chaining_policy) {
		case CHAINING_LIST:
			{
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			hi_bucket_obj_t *b_obj;
			lhi_list_for_each_entry(b_obj, &(hi_handle->bucket_table[bucket]), list) {
				if (hi_handle->compare(key, b_obj->key)) {
					*data = b_obj->data;
			lhi_pthread_unlock(&hi_handle->mutex_lock);
					return SUCCESS;
				}
			}
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			}
			return FAILURE;
			break;
			/* CHAINING_LIST_MTF is nearly equal to the CHAINING_LIST
			 * strategy except to the hi_get routine:
			 * This strategy favors often used elements by doing a swapping
			 * of elements (key and data) to the beginning of the list.
			 * Therefore if the searched elements are underlie no normal
			 * distribution this strategy may have an advantage. The
			 * disadvantage of the algorithm is the swap routine - of
			 * course.
			 */
		case CHAINING_LIST_MTF:
			{
				/* Key and data are pointers - therefore we store
				 * the pointer to the first set, search the right set and
				 * reorder the set.
				 */
			hi_bucket_obj_t *b_obj, *p;

			lhi_pthread_lock(&hi_handle->mutex_lock);
			lhi_list_for_each_entry_safe(b_obj, p, &(hi_handle->bucket_table[bucket]), list) {
				if (hi_handle->compare(key, b_obj->key)) {

					lhi_list_del(&b_obj->list);
					lhi_list_add_head(&b_obj->list, &(hi_handle->bucket_table[bucket]));
					*data = b_obj->data;
			lhi_pthread_unlock(&hi_handle->mutex_lock);

					return SUCCESS;
				}
			}
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			}
			return FAILURE;
			break;
		case CHAINING_HASHLIST:
			{
			hi_bucket_hl_obj_t *b_obj;
			lhi_pthread_lock(&hi_handle->mutex_lock);
			uint32_t key_hash = hi_handle->listhash(key, keylen);
			lhi_list_for_each_entry(b_obj, &(hi_handle->bucket_table[bucket]), list) {
				if (key_hash == b_obj->key_hash &&
					hi_handle->compare(key, b_obj->key)) {
					*data = b_obj->data;
			lhi_pthread_unlock(&hi_handle->mutex_lock);
					return SUCCESS;
				}
			}
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			}
			return FAILURE;
			break;
		case CHAINING_ARRAY:
			{
			unsigned int i;
			lhi_pthread_lock(&hi_handle->mutex_lock);
			for (i = 0; i < hi_handle->bucket_array_slot_size[bucket]; i++) {
				if (hi_handle->compare(key, hi_handle->bucket_array[bucket][i].key)) {
					*data = hi_handle->bucket_array[bucket][i].data;
					lhi_pthread_unlock(&hi_handle->mutex_lock);
					return SUCCESS;
				}
			}
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			}
			return FAILURE;
			break;
		default:
			return hi_error(EINVAL, "chaining strategie not supported (values: %d)",
					hi_handle->chaining_policy);
			break;
	};

	return FAILURE;
}

/* hi_remove remove a complete dataset completly from the hash set
 *
 * @arg hi_handle the hashish handle
 * @arg key a pointer to the key
 * @arg keylen the length of the key in bytes
 * @data the pointer-pointer for the returned data
 * @returns FAILURE or SUCCESS on success and set data pointer
 */
int hi_remove(hi_handle_t *hi_handle, void *key, uint32_t keylen, void **data)
{
	int bucket = hi_handle->hash(key, keylen) % hi_handle->buckets;

	switch (hi_handle->chaining_policy) {
		case CHAINING_LIST:
		case CHAINING_LIST_MTF:
			{
			hi_bucket_obj_t *b_obj, *p;
			lhi_pthread_lock(&hi_handle->mutex_lock);
			lhi_list_for_each_entry_safe(b_obj, p, &(hi_handle->bucket_table[bucket]), list) {
				if (hi_handle->compare(key, b_obj->key)) {
					*data = b_obj->data;
					lhi_list_del(&b_obj->list);
					free(b_obj);
					--hi_handle->bucket_size[bucket];
					--hi_handle->size;
			lhi_pthread_unlock(&hi_handle->mutex_lock);
					return SUCCESS;
				}
			}
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			}
			return FAILURE;
			break;
		case CHAINING_HASHLIST:
			{
			hi_bucket_hl_obj_t *b_obj, *p;
			lhi_pthread_lock(&hi_handle->mutex_lock);
			uint32_t key_hash = hi_handle->listhash(key, keylen);
			lhi_list_for_each_entry_safe(b_obj, p, &(hi_handle->bucket_table[bucket]), list) {
				if (key_hash == b_obj->key_hash &&
					hi_handle->compare(key, b_obj->key)) {
					*data = b_obj->data;
					lhi_list_del(&b_obj->list);
					free(b_obj);
					--hi_handle->bucket_size[bucket];
					--hi_handle->size;
			lhi_pthread_unlock(&hi_handle->mutex_lock);
					return SUCCESS;
				}
			}
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			}
			return FAILURE;
			break;
		case CHAINING_ARRAY:
			{
			unsigned int i;
			lhi_pthread_lock(&hi_handle->mutex_lock);
			for (i = 0; i < hi_handle->bucket_array_slot_size[bucket]; i++) {
				if (hi_handle->compare(key, hi_handle->bucket_array[bucket][i].key)) {
					*data = hi_handle->bucket_array[bucket][i].data;
					--hi_handle->bucket_array_slot_size[bucket];
					--hi_handle->size;
			lhi_pthread_unlock(&hi_handle->mutex_lock);
					return SUCCESS;
				}
			}
			lhi_pthread_lock(&hi_handle->mutex_lock);
			}
			return FAILURE;
			break;
		default:
			return hi_error(EINVAL, "chaining strategie not supported (values: %d)",
					hi_handle->chaining_policy);
			break;
	};

	return FAILURE;
}

/* hi_insert insert a key/data pair into our hashhandle
 *
 * @arg hi_handle the hashish handle
 * @return SUCCESS or a negativ return values in the case of an error
 */
int hi_insert(hi_handle_t *hi_handle, void *key, uint32_t keylen, void *data)
{
	int bucket, ret;

	if (lhi_lookup(hi_handle, key, keylen) == SUCCESS) { /* already in hash or error */
		return hi_error(EINVAL, "key already in hashtable");
	}

	switch (hi_handle->chaining_policy) {

		case CHAINING_LIST:
		case CHAINING_LIST_MTF:
			{
			hi_bucket_obj_t *obj;
			bucket = hi_handle->hash(key, keylen) % hi_handle->buckets;
			ret = XMALLOC((void **) &obj, sizeof(hi_bucket_obj_t));
			if (ret != 0)
				return hi_errno(errno);

			lhi_pthread_lock(&hi_handle->mutex_lock);
			obj->hi_handle = &hi_handle->bucket_table[bucket];
			lhi_list_add_tail(&obj->list, &hi_handle->bucket_table[bucket]);
			obj->key = key;
			obj->data = data;
			hi_handle->bucket_size[bucket]++;
			hi_handle->size++;
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			return SUCCESS;
			}
			break;
		case CHAINING_HASHLIST:
			{
			hi_bucket_hl_obj_t *obj;

			bucket = hi_handle->hash(key, keylen) % hi_handle->buckets;
			ret = XMALLOC((void **) &obj, sizeof(hi_bucket_hl_obj_t));
			if (ret != 0)
				return hi_errno(errno);

			obj->hi_handle = &hi_handle->bucket_table[bucket];
			lhi_pthread_lock(&hi_handle->mutex_lock);
			lhi_list_add_tail(&obj->list, &hi_handle->bucket_table[bucket]);
			obj->key = key;
			obj->data = data;
			/* build key hashsum */
			obj->key_hash = hi_handle->listhash(key, keylen);
			hi_handle->bucket_size[bucket]++;
			hi_handle->size++;
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			return SUCCESS;
			}
			break;
		case CHAINING_ARRAY:
			bucket = hi_handle->hash(key, keylen) % hi_handle->buckets;
			lhi_pthread_lock(&hi_handle->mutex_lock);
			if (hi_handle->bucket_array_slot_size[bucket] >=
					hi_handle->bucket_array_slot_max[bucket]) {

				/* bucket size exhausted -> double it */
				hi_handle->bucket_array_slot_max[bucket] =
					hi_handle->bucket_array_slot_max[bucket] << 1;

				hi_handle->bucket_array[bucket] = realloc(hi_handle->bucket_array[bucket],
						sizeof(hi_bucket_a_obj_t) * hi_handle->bucket_array_slot_max[bucket]);
				if (hi_handle->bucket_array[bucket] == NULL) {
			lhi_pthread_unlock(&hi_handle->mutex_lock);
					return hi_errno(errno);
				}

			}
			/* add key/data add next free slot */
			hi_handle->bucket_array[bucket][hi_handle->bucket_array_slot_size[bucket]].key = key;
			hi_handle->bucket_array[bucket][hi_handle->bucket_array_slot_size[bucket]].data = data;

			hi_handle->bucket_array_slot_size[bucket]++;
			hi_handle->size++;
			lhi_pthread_unlock(&hi_handle->mutex_lock);

			return SUCCESS;
		default:
			return hi_error(EINVAL, "chaining strategie not supported (values: %d)",
					hi_handle->chaining_policy);
			break;
	}

	return FAILURE;
}

/* hi_fini delete a complete hashish handle. Keys and data _aren't_ free'ed.
 *
 * @arg hi_handle the hashish handle
 * @return SUCCESS or a negativ return values in the case of an error
 */
int hi_fini(hi_handle_t *hi_handle)
{
	uint32_t i;

	switch (hi_handle->chaining_policy) {
		case CHAINING_LIST:
		case CHAINING_LIST_MTF:
			{
			struct lhi_list_head *pos;
			lhi_pthread_lock(&hi_handle->mutex_lock);
			for (i = 0; i < hi_handle->buckets; i++) {
			  hi_bucket_obj_t *b_obj, *p;
			  pos = &hi_handle->bucket_table[i];
			  /* iterate over list and remove all entries and free hi_bucket_obj_t */
			  lhi_list_for_each_entry_safe(b_obj, p, pos, list) {
				lhi_list_del(&b_obj->list);
				free(b_obj);
			  }
			}
			free(hi_handle->bucket_table);
			free(hi_handle->bucket_size);
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			}
			break;

		case CHAINING_HASHLIST:
			{
			struct lhi_list_head *pos;
			lhi_pthread_lock(&hi_handle->mutex_lock);
			for (i = 0; i < hi_handle->buckets; i++) {
			  hi_bucket_hl_obj_t *b_obj, *p;
			  pos = &hi_handle->bucket_table[i];
			  /* iterate over list and remove all entries and free hi_bucket_obj_t */
			  lhi_list_for_each_entry_safe(b_obj, p, pos, list) {
				lhi_list_del(&b_obj->list);
				free(b_obj);
			  }
			}
			free(hi_handle->bucket_table);
			free(hi_handle->bucket_size);
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			}
			break;

		case CHAINING_ARRAY:
			lhi_pthread_lock(&hi_handle->mutex_lock);
			for (i = 0; i < hi_handle->buckets; i++) {
				free(hi_handle->bucket_array[i]);
			}
			free(hi_handle->bucket_array);
			free(hi_handle->bucket_array_slot_size);
			free(hi_handle->bucket_array_slot_max);
			free(hi_handle->bucket_size);
			lhi_pthread_unlock(&hi_handle->mutex_lock);
			break;

		default:
			return hi_error(EINVAL, "chaining strategie not supported (values: %d)",
					hi_handle->chaining_policy);
			break;
	};

	/* cleanup the mutex lock */
	lhi_pthread_destroy(&hi_handle->mutex_lock);

	free(hi_handle);

	hi_handle = NULL;

	return SUCCESS;
}



/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
