/*
 * generic hash table iterator interface.
 * (c) 2008 Florian Westphal <fw@strlen.de>.
 *
 * This file is in the Public Domain.
 */

#include <stdlib.h>

#include "libhashish.h"
#include "privlibhashish.h"

struct hi_operator {
	hi_handle_t *handle; /* hash table we are iterating */
	size_t bucket;	/* position in the hash table array we are currently looking at */
	void **private; /* array of pointers to the data stored at ->bucket; these pointers are returned by subsequent iterator_getnext() calls. */
	size_t counter; /* position in the **private array we returned last. counter is decremented, if it is 0, get next bucket */
};


static int rbtree_get_next_tree(hi_iterator_t *i)
{
	hi_handle_t *t = i->handle;

	for (;i->bucket < t->table_size ; i->bucket++) {
		int res = lhi_rbtree_bucket_to_array(t, i->bucket, (void **) &i->private, (void *) &i->counter);
		if (res == 0)
			return 0;
		if (res != HI_ERR_NODATA)
			return res;
	}
	return HI_ERR_NODATA;
}

static int list_get_next_bucket(hi_iterator_t *i)
{
	hi_handle_t *t = i->handle;

	for (;i->bucket < t->table_size ; i->bucket++) {
		int res = lhi_list_bucket_to_array(t, i->bucket, (void **) &i->private, (void *) &i->counter);
		if (res == 0)
			return 0;
		if (res != HI_ERR_NODATA)
			return res;
	}
	return HI_ERR_NODATA;
}

static int array_get_next_bucket(hi_iterator_t *i)
{
	hi_handle_t *t = i->handle;

	for (;i->bucket < t->table_size ; i->bucket++) {
		int res = lhi_array_bucket_to_array(t, i->bucket, (void**) &i->private, (void *) &i->counter);
		if (res == 0)
			return 0;
		if (res != HI_ERR_NODATA)
			return res;
	}
	return HI_ERR_NODATA;
}


int hi_iterator_create(hi_handle_t *t, hi_iterator_t **i)
{
	int res = HI_ERR_SYSTEM;
	hi_iterator_t *it = malloc(sizeof(*it));
	if (!it)
		return HI_ERR_SYSTEM;
	it->handle = t;
	it->bucket = 0;
	it->counter = 0;
	switch (t->coll_eng) {
	case COLL_ENG_LIST:
	case COLL_ENG_LIST_HASH:
	case COLL_ENG_LIST_MTF:
	case COLL_ENG_LIST_MTF_HASH:
		res = list_get_next_bucket(it);
		break;
	case COLL_ENG_ARRAY:
	case COLL_ENG_ARRAY_HASH:
	case COLL_ENG_ARRAY_DYN:
	case COLL_ENG_ARRAY_DYN_HASH:
		res = array_get_next_bucket(it);
		break;
	case COLL_ENG_RBTREE:
		res = rbtree_get_next_tree(it);
		break;
	default:
		res = HI_ERR_INTERNAL;
	}
	if (res == 0)
		*i = it;
	else
		free(it);
	return res;
}

int hi_iterator_reset(hi_iterator_t *i)
{
	int res = 0;
	enum coll_eng e = i->handle->coll_eng;
	i->bucket = 0;
	switch (e) {
	case COLL_ENG_RBTREE:
		free(i->private);
		res = rbtree_get_next_tree(i);
		if (res)
			i->private = NULL;
		break;
	case COLL_ENG_ARRAY:
	case COLL_ENG_ARRAY_HASH:
	case COLL_ENG_ARRAY_DYN:
	case COLL_ENG_ARRAY_DYN_HASH:
		free(i->private);
		res = list_get_next_bucket(i);
		break;
	case COLL_ENG_LIST:
	case COLL_ENG_LIST_HASH:
	case COLL_ENG_LIST_MTF:
	case COLL_ENG_LIST_MTF_HASH:
		free(i->private);
		res = list_get_next_bucket(i);
		if (res)
			i->private = NULL;
		break;
	default:
		return HI_ERR_SYSTEM;
	}
	return res;
}


int hi_iterator_getnext(hi_iterator_t *i, void **res)
{
	int ret;
	if (i->counter) {
		i->counter--;
		*res = i->private[i->counter];
		return 0;
	}
	switch (i->handle->coll_eng) {
	case COLL_ENG_RBTREE:
		free(i->private);
		i->bucket++;
		ret = rbtree_get_next_tree(i);
		break;
	case COLL_ENG_LIST:
	case COLL_ENG_LIST_HASH:
	case COLL_ENG_LIST_MTF:
	case COLL_ENG_LIST_MTF_HASH:
		free(i->private);
		i->bucket++;
		ret = list_get_next_bucket(i);
		break;
	case COLL_ENG_ARRAY:
	case COLL_ENG_ARRAY_HASH:
	case COLL_ENG_ARRAY_DYN:
	case COLL_ENG_ARRAY_DYN_HASH:
		free(i->private);
		i->bucket++;
		ret = array_get_next_bucket(i);
		break;
	default:
		return HI_ERR_INTERNAL;
	}
	if (ret) {
		i->private = NULL;
		return ret;
	}
	i->counter--;
	*res = i->private[i->counter];
	return ret;
}


void hi_iterator_fini(hi_iterator_t *i)
{
	switch (i->handle->coll_eng) {
	case COLL_ENG_LIST:
	case COLL_ENG_LIST_HASH:
	case COLL_ENG_LIST_MTF:
	case COLL_ENG_LIST_MTF_HASH:

	case COLL_ENG_ARRAY:
	case COLL_ENG_ARRAY_HASH:
	case COLL_ENG_ARRAY_DYN:
	case COLL_ENG_ARRAY_DYN_HASH:

	case COLL_ENG_RBTREE:
		free(i->private);
	/* fallthrough & fix gcc warning (enumeration value '__COLL_ENG_MAX' not handled in switch) */
	case __COLL_ENG_MAX:
		break;
	}
	free(i);
}
