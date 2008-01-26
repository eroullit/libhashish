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

#include "privlibhashish.h"
#include "libhashish.h"
#include "list.h"

#include "threads.h"

/* lhi_lookup_list search for a given key and return SUCCESS
 * when found in the hash and FAILURE if not found.
 *
 * @arg hi_handle the hashish handle
 * @arg key the pointer to the key
 * @arg keylen the len of the key in bytes
 * @return SUCCESS if found or FAILURE when not found
 */

int lhi_lookup_list(const hi_handle_t *hi_handle,
		void *key, uint32_t keylen)
{
	uint32_t bucket;

	bucket =  hi_handle->hash_func(key, keylen) % hi_handle->table_size;

	switch (hi_handle->coll_eng) {

		case COLL_ENG_LIST:
		case COLL_ENG_LIST_MTF:
			{
			hi_bucket_obj_t *b_obj;
			lhi_list_for_each_entry(b_obj,
					&(hi_handle->eng_list.bucket_table[bucket]), list) {
				if (hi_handle->key_cmp(key, b_obj->key)) {
					fprintf(stderr, "\nDEBUG new: %s old: %s\n\n", b_obj->key, key);
					return SUCCESS;
				}
			}
			return HI_ERR_NOKEY;
			}
			break;

		case COLL_ENG_LIST_HASH:
		case COLL_ENG_LIST_MTF_HASH:
			{
			hi_bucket_hl_obj_t *b_obj;
			uint32_t key_hash = hi_handle->hash2_func(key, keylen);
			lhi_list_for_each_entry(b_obj,
					&(hi_handle->eng_list.bucket_table[bucket]), list) {
				if (key_hash == b_obj->key_hash &&
						hi_handle->key_cmp(key, b_obj->key)) {
					return SUCCESS;
				}
			}
			}
			return HI_ERR_NOKEY;
			break;

		default:
			return HI_ERR_INTERNAL;
	}

	return HI_ERR_NOKEY;
}


/**
 * hi_remove remove a complete dataset completly from the hash set
 *
 * @arg hi_handle the hashish handle
 * @arg key a pointer to the key
 * @arg keylen the length of the key in bytes
 * @data the pointer-pointer for the returned data
 * @returns FAILURE or SUCCESS on success and set data pointer
 */
int lhi_remove_list(hi_handle_t *hi_handle, void *key,
		uint32_t keylen, void **data)
{
	uint32_t bucket;

	bucket =  hi_handle->hash_func(key, keylen) % hi_handle->table_size;

	switch (hi_handle->coll_eng) {

		case COLL_ENG_LIST:
		case COLL_ENG_LIST_MTF:
			{
			hi_bucket_obj_t *b_obj, *p;

			lhi_pthread_lock(hi_handle->mutex_lock);
			lhi_list_for_each_entry_safe(b_obj, p, &(hi_handle->eng_list.bucket_table[bucket]), list) {
				if (hi_handle->key_cmp(key, b_obj->key)) {
					*data = b_obj->data;
					lhi_list_del(&b_obj->list);
					free(b_obj);
					--hi_handle->bucket_size[bucket];
					--hi_handle->no_objects;
					lhi_pthread_unlock(hi_handle->mutex_lock);
					return SUCCESS;
			}
			}
			lhi_pthread_unlock(hi_handle->mutex_lock);
			return HI_ERR_NOKEY;
			}
			break;

		case COLL_ENG_LIST_HASH:
		case COLL_ENG_LIST_MTF_HASH:
			{
			hi_bucket_hl_obj_t *b_obj, *p;
			lhi_pthread_lock(hi_handle->mutex_lock);

			uint32_t key_hash = hi_handle->hash2_func(key, keylen);
			lhi_list_for_each_entry_safe(b_obj, p, &(hi_handle->eng_list.bucket_table[bucket]), list) {

				if (key_hash == b_obj->key_hash &&
						hi_handle->key_cmp(key, b_obj->key)) {

					*data = b_obj->data;
					lhi_list_del(&b_obj->list);
					free(b_obj);
					--hi_handle->bucket_size[bucket];
					--hi_handle->no_objects;
					lhi_pthread_unlock(hi_handle->mutex_lock);
					return SUCCESS;

				}
			}
			lhi_pthread_unlock(hi_handle->mutex_lock);
			return HI_ERR_NOKEY;
			}
			break;

		default:
			return HI_ERR_INTERNAL;
	}


}

/**
 * hi_get_list return for a given key the correspond data entry
 *
 * @arg hi_handle the hashish handle
 * @arg key a pointer to the key
 * @arg keylen the length of the key in bytes
 * @data the pointer-pointer for the returned data
 * @returns FAILURE or SUCCESS on success and set data pointer
 */
int lhi_get_list(hi_handle_t *hi_handle, void *key,
		uint32_t keylen, void **data)
{
	uint32_t bucket;

	bucket =  hi_handle->hash_func(key, keylen) % hi_handle->table_size;

	switch (hi_handle->coll_eng) {

		case COLL_ENG_LIST:
			{
			lhi_pthread_lock(hi_handle->mutex_lock);
			hi_bucket_obj_t *b_obj;
			lhi_list_for_each_entry(b_obj, &(hi_handle->eng_list.bucket_table[bucket]), list) {
				if (hi_handle->key_cmp(key, b_obj->key)) {
					*data = b_obj->data;
					lhi_pthread_unlock(hi_handle->mutex_lock);
					return SUCCESS;
				}
			}
			lhi_pthread_unlock(hi_handle->mutex_lock);
			return HI_ERR_NOKEY;
			}
            /* CHAINING_LIST_MTF is nearly equal to the CHAINING_LIST
             * strategy except to the hi_get routine:
             * This strategy favors often used elements by doing a swapping
             * of elements (key and data) to the beginning of the list.
             * Therefore if the searched elements are underlie no normal
             * distribution this strategy may have an advantage. The
             * disadvantage of the algorithm is the swap routine - of
             * course.
             */
		case COLL_ENG_LIST_MTF:
			{
			/* Key and data are pointers - therefore we store
			 * the pointer to the first set, search the right
			 * set and reorder the set.
			 */
			hi_bucket_obj_t *b_obj, *p;
			lhi_pthread_lock(hi_handle->mutex_lock);
			lhi_list_for_each_entry_safe(b_obj, p, &(hi_handle->eng_list.bucket_table[bucket]), list) {

				if (hi_handle->key_cmp(key, b_obj->key)) {
					lhi_list_del(&b_obj->list);
					lhi_list_add_head(&b_obj->list, &(hi_handle->eng_list.bucket_table[bucket]));
					*data = b_obj->data;
					lhi_pthread_unlock(hi_handle->mutex_lock);
					return SUCCESS;
				}

			}
			lhi_pthread_unlock(hi_handle->mutex_lock);
			return HI_ERR_NOKEY;
			}
			break;

		case COLL_ENG_LIST_HASH:
			{
			hi_bucket_hl_obj_t *b_obj;
			lhi_pthread_lock(hi_handle->mutex_lock);
			uint32_t key_hash = hi_handle->hash2_func(key, keylen);
			lhi_list_for_each_entry(b_obj, &(hi_handle->eng_list.bucket_table[bucket]), list) {

				/* compare hash value (short circuit) and do an
				 * compare, ...yes the second hash function can also
				 * experience collisions ... ;-)
				 */
				if (key_hash == b_obj->key_hash &&
						hi_handle->key_cmp(key, b_obj->key)) {
					*data = b_obj->data;
					lhi_pthread_unlock(hi_handle->mutex_lock);
					return SUCCESS;
				}

			}
			lhi_pthread_unlock(hi_handle->mutex_lock);
			return HI_ERR_NOKEY;
			}

		case COLL_ENG_LIST_MTF_HASH:
			{
			hi_bucket_hl_obj_t *b_obj;
			lhi_pthread_lock(hi_handle->mutex_lock);
			uint32_t key_hash = hi_handle->hash2_func(key, keylen);
			lhi_list_for_each_entry(b_obj, &(hi_handle->eng_list.bucket_table[bucket]), list) {

				/* compare hash value (short circuit) and do an
				 * compare, ...yes the second hash function can also
				 * experience collisions ... ;-)
				 */
				if (key_hash == b_obj->key_hash &&
						hi_handle->key_cmp(key, b_obj->key)) {
					lhi_list_del(&b_obj->list);
					lhi_list_add_head(&b_obj->list, &(hi_handle->eng_list.bucket_table[bucket]));
					*data = b_obj->data;
					lhi_pthread_unlock(hi_handle->mutex_lock);
					return SUCCESS;
				}

			}
			lhi_pthread_unlock(hi_handle->mutex_lock);
			return HI_ERR_NOKEY;
			}
			break;

		default:
			return HI_ERR_INTERNAL;
	}


	return HI_ERR_NOKEY;
}

/* lhi_insert_list insert a key/data pair into our hashhandle
 *
 * @arg hi_handle the hashish handle
 * @return SUCCESS or a negativ return values in the case of an error
 */
int lhi_insert_list(hi_handle_t *hi_handle, void *key,
		uint32_t keylen, void *data)
{
	int ret;
	uint32_t bucket;

	hi_bucket_obj_t *obj;
	bucket = hi_handle->hash_func(key, keylen) % hi_handle->table_size;
	ret = XMALLOC((void **) &obj, sizeof(hi_bucket_obj_t));
	if (ret != 0)
		return HI_ERR_SYSTEM;

	obj->hi_handle = &hi_handle->eng_list.bucket_table[bucket];
	lhi_list_add_tail(&obj->list, &hi_handle->eng_list.bucket_table[bucket]);
	obj->key = key;
	obj->data = data;
	hi_handle->bucket_size[bucket]++;
	hi_handle->no_objects++;

	return SUCCESS;

}


/* lhi_fini_list delete a complete hashish handle. This function is destroy
 * list specific data. The whole funtion is protected by an global
 * lock.
 *
 * @arg hi_handle the hashish handle
 * @return SUCCESS or a negativ return values in the case of an error
 */
int lhi_fini_list(hi_handle_t *hi_handle)
{
	uint32_t i;
	struct lhi_list_head *pos;

	for (i = 0; i < hi_handle->table_size; i++) {
		hi_bucket_obj_t *b_obj, *p;
		pos = &hi_handle->eng_list.bucket_table[i];
		/* iterate over list and remove all entries and free hi_bucket_obj_t */
		lhi_list_for_each_entry_safe(b_obj, p, pos, list) {
			lhi_list_del(&b_obj->list);
			free(b_obj);
		}
	}
	free(hi_handle->eng_list.bucket_table);
	free(hi_handle->bucket_size);

	return SUCCESS;
}

/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
