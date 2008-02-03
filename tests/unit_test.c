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
#include <assert.h>

#include "libhashish.h"
#include "localhash.h"
#include "list.h"
#include "tests.h"

#define	TESTSTRING "SURVEILLANCE"

#define	TEST_ITER_NO 2048
#define	KEYLEN 50
#define	DATALEN 100

#define	KEY 0
#define	DATA 1

static uint32_t dumb_hash_func(const uint8_t *key,
		uint32_t len)
{
	(void) key;
	(void) len;

	return 1;
}

static void print_error(int ret)
{
	const char *msg;

	if (ret == HI_ERR_SYSTEM)
		msg = strerror(errno);
	else
		msg = hi_strerror(ret);

	fprintf(stderr, "Error: %s\n", msg);
}


static void check_data(const char *data, const char *expected)
{
	if (strcmp(data, expected)) {
		fprintf(stderr, "strcmp failed: got \"%s\", expected \"%s\"\n", data, expected);
		exit(1);
	}
}



static void check_remove(enum coll_eng engine)
{
	int ret;
	hi_handle_t *hi_hndl;
	struct hi_init_set hi_set;
	void *data_ret;

	hi_set_zero(&hi_set);
	ret = hi_set_bucket_size(&hi_set, 100);
	assert(ret == 0);
	ret = hi_set_hash_alg(&hi_set, HI_HASH_WEINB);
	assert(ret == 0);
	ret = hi_set_coll_eng(&hi_set, engine);
	assert(ret == 0);
	ret = hi_set_key_cmp_func(&hi_set, hi_cmp_str);
	assert(ret == 0);

	ret = hi_create(&hi_hndl, &hi_set);
	if (ret != 0)
		print_error(ret);
	assert(ret == 0);
	assert(hi_hndl->no_objects == 0);

	ret = hi_insert(hi_hndl, (void *) "key", strlen("key"), "DATA");
	assert(ret == 0);
	assert(hi_hndl->no_objects == 1);
	ret = hi_insert(hi_hndl, (void *) "key2", strlen("key2"), "DATAX");
	assert(ret == 0);
	assert(hi_hndl->no_objects == 2);
	ret = hi_insert(hi_hndl, (void *) "key3", strlen("key3"), "DATAX");
	assert(ret == 0);

	assert(hi_hndl->no_objects == 3);

	/* key already in data structure -> must return 0 (SUCCESS) */
	ret = hi_remove(hi_hndl, (void *) "key", strlen("key"), &data_ret);
	if (ret != 0)
		print_error(ret);
	assert(ret == 0);
	check_data(data_ret, "DATA");
	assert(hi_hndl->no_objects == 2);
	data_ret = NULL;

	ret = hi_remove(hi_hndl, (void *) "key2", strlen("key2"), &data_ret);
	if (ret != 0)
		print_error(ret);
	assert(ret == 0);
	check_data(data_ret, "DATAX");
	assert(hi_hndl->no_objects == 1);
	data_ret = NULL;

	ret = hi_remove(hi_hndl, (void *) "key3", strlen("key3"), &data_ret);
	if (ret != 0)
		print_error(ret);
	assert(ret == 0);
	check_data(data_ret, "DATAX");
	assert(hi_hndl->no_objects == 0);

	/* must fail */
	ret = hi_remove(hi_hndl, (void *) "key", strlen("key"), &data_ret);
	assert(ret == HI_ERR_NOKEY);

	ret = hi_fini(hi_hndl);
	assert(ret == 0);

}


static void check_get_remove(enum coll_eng engine)
{
	int ret;
	hi_handle_t *hi_hndl;
	struct hi_init_set hi_set;
	void *data_ret;

	hi_set_zero(&hi_set);
	ret = hi_set_bucket_size(&hi_set, 100);
	assert(ret == 0);
	ret = hi_set_hash_alg(&hi_set, HI_HASH_WEINB);
	assert(ret == 0);
	ret = hi_set_coll_eng(&hi_set, engine);
	assert(ret == 0);
	ret = hi_set_key_cmp_func(&hi_set, hi_cmp_str);
	assert(ret == 0);

	ret = hi_create(&hi_hndl, &hi_set);
	if (ret != 0)
		print_error(ret);
	assert(ret == 0);

	assert(hi_hndl->no_objects == 0);
	ret = hi_insert(hi_hndl, (void *) "key", strlen("key"), "DATA");
	assert(ret == 0);
	assert(hi_hndl->no_objects == 1);
	ret = hi_insert(hi_hndl, (void *) "key2", strlen("key2"), "DATAX");
	assert(ret == 0);
	assert(hi_hndl->no_objects == 2);
	ret = hi_insert(hi_hndl, (void *) "key3", strlen("key3"), "DATAX");
	assert(ret == 0);
	assert(hi_hndl->no_objects == 3);

	/* key already in data structure -> must return 0 (SUCCESS) */
	ret = hi_get(hi_hndl, (void *) "key", strlen("key"), &data_ret);
	if (ret != 0)
		print_error(ret);
	check_data(data_ret, "DATA");
	assert(hi_hndl->no_objects == 3);

	ret = hi_get(hi_hndl, (void *) "key3", strlen("key3"), &data_ret);
	if (ret != 0)
		print_error(ret);
	check_data(data_ret, "DATAX");
	assert(hi_hndl->no_objects == 3);
	data_ret = NULL;

	ret = hi_remove(hi_hndl, (void *) "key", strlen("key"), &data_ret);
	assert(ret == 0);
	assert(hi_hndl->no_objects == 2);
	check_data(data_ret, "DATA");
	data_ret = NULL;
	ret = hi_remove(hi_hndl, (void *) "key2", strlen("key2"), &data_ret);
	assert(ret == 0);
	assert(hi_hndl->no_objects == 1);
	ret = hi_remove(hi_hndl, (void *) "key3", strlen("key3"), &data_ret);
	assert(ret == 0);
	assert(hi_hndl->no_objects == 0);
	check_data(data_ret, "DATAX");

	ret = hi_fini(hi_hndl);
	assert(ret == 0);
}


static void check_insert(enum coll_eng engine)
{
	int ret;
	hi_handle_t *hi_hndl;
	struct hi_init_set hi_set;
	void *data_ptr = (void *) 0xdeadbeef;

	hi_set_zero(&hi_set);
	ret = hi_set_bucket_size(&hi_set, 100);
	assert(ret == 0);
	ret = hi_set_hash_alg(&hi_set, HI_HASH_WEINB);
	assert(ret == 0);
	ret = hi_set_coll_eng(&hi_set, engine);
	assert(ret == 0);
	ret = hi_set_key_cmp_func(&hi_set, hi_cmp_str);
	assert(ret == 0);

	ret = hi_create(&hi_hndl, &hi_set);
	if (ret != 0)
		print_error(ret);
	assert(ret == 0);

	ret = hi_insert(hi_hndl, (void *) "key", strlen("key"), NULL);
	assert(ret == 0);

	/* same key -> must fail */
	ret = hi_insert(hi_hndl, (void *) "key", strlen("key"), NULL);
	assert(ret == HI_ERR_DUPKEY);

	/* key already in data structure -> must return 0 (SUCCESS) */
	ret = hi_get(hi_hndl, (void *) "key", strlen("key"), &data_ptr);
	assert(ret == 0);
	assert(data_ptr == NULL);
	ret = hi_remove(hi_hndl, (void *) "key", strlen("key"), &data_ptr);
	assert(ret == 0);
	ret = hi_get(hi_hndl, (void *) "key", strlen("key"), &data_ptr);
	assert(ret == HI_ERR_NOKEY);

	ret = hi_fini(hi_hndl);
	assert(ret == 0);
}


static void check_hi_init_set(void)
{
	int ret;
	struct hi_init_set hi_set;

	fputs(" o struct set initialize tests ...", stdout);

	hi_set_zero(&hi_set);

	/* test if a hash table size of 100
	 * return the right values */
	ret = hi_set_bucket_size(&hi_set, 100);
	assert(ret == 0);

	/* trigger a failure - hash table size of
	 * 0 is invalid */
	ret = hi_set_bucket_size(&hi_set, 0);
	assert(ret == HI_ERR_RANGE);

	/* test for standard hashing algorithm - must pass */
	ret = hi_set_hash_alg(&hi_set, HI_HASH_WEINB);
	assert(ret == 0);

	ret = hi_set_hash_func(&hi_set, dumb_hash_func);
	assert(ret == 0);

	/* hash algorithm not supported test - must fail */
	ret = hi_set_hash_alg(&hi_set, (unsigned int) -1);
	assert(ret == HI_ERR_NOFUNC);

	/* test for standard collision engine - must pass */
	ret = hi_set_coll_eng(&hi_set, COLL_ENG_LIST);
	assert(ret == 0);

	/* collision engine not supported test - must fail */
	ret = hi_set_coll_eng(&hi_set, (unsigned int) -1);
	assert(ret == HI_ERR_NOFUNC);

	/* test compare functions - must pass */
	ret = hi_set_key_cmp_func(&hi_set, hi_cmp_str);
	assert(ret == 0);

	/* test compare functions - must fail */
	ret = hi_set_key_cmp_func(&hi_set, NULL);
	assert(ret == HI_ERR_NODATA);

	puts(" passed");
}


static void check_str_wrapper(void)
{
	int ret;
	hi_handle_t *hi_handle;
	const char *key = "23";
	const char *data = "data element";
	void *data_ptr;

	fputs("o string wrapper functions tests ...", stdout);

	ret = hi_init_str(&hi_handle, 23);
	assert(ret == 0);

	ret = hi_insert_str(hi_handle, key, data);
	assert(ret == 0);

	ret = hi_get_str(hi_handle, key, &data_ptr);
	assert(ret == 0);

	ret = hi_fini(hi_handle);
	assert(ret == 0);

	puts(" passed");
}


static void test_backend(enum coll_eng engine)
{
	puts("check insert");
	check_insert(engine);

	puts("check remove");
	check_remove(engine);

	puts("check get/remove");
	check_get_remove(engine);
}


int
main(void)
{
	puts("Start test sequence");

	check_hi_init_set();

	puts("check COLL_ENG_LIST :");
	test_backend(COLL_ENG_LIST);
	puts("check COLL_ENG_RBTREE");
	test_backend(COLL_ENG_RBTREE);

	check_str_wrapper();

	return 0;
}

/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
