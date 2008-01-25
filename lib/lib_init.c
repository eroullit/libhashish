/*
** $Id: libhashish.c 15 2007-08-23 15:17:56Z hgndgtl $
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

#include "privlibhashish.h"
#include "libhashish.h"
#include "list.h"
#include "threads.h"

struct hashfunc_map_t hashfunc_map[] = {
	{HI_HASH_WEINB, lhi_hash_weinb},
	{ 0, NULL }
};


int lhi_create_vanilla_hdnl(hi_handle_t **hi_hndl)
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

static int lhi_set_sanity_hash2(struct hi_init_set *hi_set)
{
	if (hi_set->hash2_func == NULL)
		return hi_error(EINVAL,
				"COLL_ENG_LIST_HASH  requires a second hash function");

	if (hi_set->hash_func == hi_set->hash2_func)
		return hi_error(EINVAL,
				"COLL_ENG_LIST_HASH requires two distinct hash functions");

	return SUCCESS;
}

static int lhi_set_sanity_array(struct hi_init_set *hi_set)
{
	if (hi_set->coll_eng_array_size == 0)
		return hi_error(EINVAL,
				"coll_eng_array_size must be greater then 0");

	return SUCCESS;
}

static int lhi_set_sanity_check(struct hi_init_set *hi_set)
{
	int ret;

	if (hi_set->table_size == 0)
		return hi_error(EINVAL, "hash table size of 0 isn't supported");

	if (hi_set->key_cmp == NULL)
		return hi_error(EINVAL, "no key compare function set");

	if (hi_set->hash_func == NULL)
		return hi_error(EINVAL, "no valid hash function selected");

	switch (hi_set->coll_eng) {

		case COLL_ENG_LIST:
			/* nothing to check here - all tests are covered
			 * in the above statements
			 */
			break;
		case COLL_ENG_LIST_HASH:
			/* Test if a second hash function is given
			 * and this function isn't the same to the
			 * primary hash function.
			 */
			ret = lhi_set_sanity_hash2(hi_set);
			if (ret != SUCCESS)
				return ret;
			break;
		case COLL_ENG_LIST_MTF:
			break;
		case COLL_ENG_LIST_MTF_HASH:
			/* Test if a second hash function is given
			 * and this function isn't the same to the
			 * primary hash function.
			 */
			ret = lhi_set_sanity_hash2(hi_set);
			if (ret != SUCCESS)
				return ret;
			break;
		case COLL_ENG_ARRAY:
			ret = lhi_set_sanity_array(hi_set);
			if (ret != SUCCESS)
				return ret;
			break;
		case COLL_ENG_ARRAY_HASH:
			break;
		case COLL_ENG_ARRAY_DYN:
			break;
		case COLL_ENG_ARRAY_DYN_HASH:
			/* Test if a second hash function is given
			 * and this function isn't the same to the
			 * primary hash function.
			 */
			ret = lhi_set_sanity_hash2(hi_set);
			if (ret != SUCCESS)
				return ret;
			break;
		case COLL_ENG_RBTREE:
			break;
		default:
			return hi_error(EINVAL,
					"Internal library error - send a bug report! ;-)");
			break;

	}

	return SUCCESS;
}

static int lhi_transform_set_2_hndl(hi_handle_t *hi_hndl,
		struct hi_init_set *hi_set)
{
	int ret;

	/* hi_init_set full sanity checks (all
	 * information is available */
	ret = lhi_set_sanity_check(hi_set);
	if (ret != SUCCESS)
		return ret;

	/* NOTE: it is also possible to integrate the hi_set within
	 * hi_hndl and memcpy the data. BUT maybe in some further revisions
	 * we must/should transform values from user perspective to a internal
	 * representation this additional step is more generic.
	 */

	hi_hndl->table_size          = hi_set->table_size;
	hi_hndl->hash_func           = hi_set->hash_func;
	hi_hndl->hash2_func          = hi_set->hash2_func;
	hi_hndl->key_cmp             = hi_set->key_cmp;
	hi_hndl->coll_eng            = hi_set->coll_eng;
	hi_hndl->self_resizing       = hi_set->self_resizing;
	hi_hndl->coll_eng_array_size = hi_set->coll_eng_array_size;

	return SUCCESS;
}

static int lhi_create_eng_list(hi_handle_t *hi_hndl)
{
	uint32_t i; int ret;

	/* This is the intrinsic table which contains
	 * the pointers to the list-heads.
	 */
	ret = XMALLOC((void **) &hi_hndl->eng_list.bucket_table,
			hi_hndl->table_size * sizeof(struct lhi_list_head));
	if (ret != 0) {
		return hi_errno(errno);
	}

	/* initialize bucket list */
	for (i = 0; i < hi_hndl->table_size; i++) {
		lhi_init_list_head(&(hi_hndl->eng_list.bucket_table[i]));
		hi_hndl->bucket_size[i] = 0;
	}

	return SUCCESS;
}

static int lhi_create_eng_rbtree(hi_handle_t *hi_hndl)
{
	int ret;

	ret =  XMALLOC((void **) &hi_hndl->eng_rbtree.rb_root,
			hi_hndl->table_size * sizeof(struct rb_root));
	if (ret != 0) {
		return hi_errno(errno);
	}


	return SUCCESS;
}

static int lhi_create_eng_array(hi_handle_t *hi_hndl)
{
	int ret; uint32_t i;

	ret = XMALLOC((void **) &hi_hndl->eng_array.bucket_array_slot_size,
			sizeof(unsigned int) * hi_hndl->table_size);
	if (ret != 0)
		return hi_errno(errno);

	ret = XMALLOC((void **) &hi_hndl->eng_array.bucket_array_slot_max,
			sizeof(unsigned int) * hi_hndl->table_size);
	if (ret != 0)
		return hi_errno(errno);

	ret = XMALLOC((void **) &hi_hndl->eng_array.bucket_array,
			(sizeof(hi_bucket_a_obj_t *) * hi_hndl->table_size));
	if (ret != 0)
		return hi_errno(errno);

	for (i = 0; i < hi_hndl->table_size; i++) {
		/* align array on 16 byte boundaries */
		ret = xalloc_align((void **) &hi_hndl->eng_array.bucket_array[i],
				LHI_DEFAULT_MEMORY_ALIGN,
				(sizeof(hi_bucket_a_obj_t) * hi_hndl->coll_eng_array_size));
		if (ret != 0)
			return hi_errno(errno);

		hi_hndl->eng_array.bucket_array_slot_size[i] = 0;
		hi_hndl->eng_array.bucket_array_slot_max[i]  = hi_hndl->coll_eng_array_size;
	}

	return SUCCESS;
}

/**
 * This is the default initialize function. It takes HI_HASH_DEFAULT as the
 * default hash function and set compare function for strings - so use it only
 * for strings
 *
 * @arg hi_hndl	this become out new hashish handle
 * @arg buckets	hash bucket size
 * @returns negativ error value or zero on success
 */
int hi_create(hi_handle_t **hi_hndl, struct hi_init_set *hi_set)
{
	int ret;
	hi_handle_t *hi_handle;

	ret = lhi_create_vanilla_hdnl(&hi_handle);
	if (ret != SUCCESS)
		return ret;

	/* Check values in hi_set and transform user
	 * representation to internal representation */
	ret = lhi_transform_set_2_hndl(hi_handle, hi_set);
	if (ret != SUCCESS)
		return ret;

	/* Allocate memory fot accounting the number of
	 * elements within every bucket in the table.  */
	ret = XMALLOC((void **) &hi_handle->bucket_size,
			hi_handle->table_size * sizeof(hi_handle->bucket_size));
	if (ret != 0) {
		return hi_errno(errno);
	}

	/* 0 objects in the list at start-up */
	hi_handle->no_objects = 0;

	/* Initiate mutex lock if build with thread
	 * support. */
	ret = lhi_pthread_init(&hi_handle->mutex_lock, NULL);
	if (ret != 0) {
		return hi_errno(errno);
	}

	/* Create internal data structure for
	 * list, array or rbtree */
	switch (hi_handle->coll_eng) {

		case COLL_ENG_LIST:
		case COLL_ENG_LIST_HASH:
		case COLL_ENG_LIST_MTF:
		case COLL_ENG_LIST_MTF_HASH:
			ret = lhi_create_eng_list(hi_handle);
			if (ret != SUCCESS)
				return ret;
			break;
		case COLL_ENG_ARRAY:
		case COLL_ENG_ARRAY_HASH:
		case COLL_ENG_ARRAY_DYN:
		case COLL_ENG_ARRAY_DYN_HASH:
			ret = lhi_create_eng_array(hi_handle);
			if (ret != SUCCESS)
				return ret;
			break;
		case COLL_ENG_RBTREE:
			ret = lhi_create_eng_rbtree(hi_handle);
			if (ret != SUCCESS)
				return ret;
			break;
		default:
			return hi_error(EINVAL,
					"Internal library error - send a bug report! ;-)");
			break;
	}

	*hi_hndl = hi_handle;

	return SUCCESS;
}


/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
