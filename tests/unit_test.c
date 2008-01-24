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
#include "list.h"
#include "tests.h"

#undef xassert
/* if 0 -> raise error */
#define	xassert(x)	\
	do {	\
		if(!x) {	\
			fprintf(stderr, "assert failed: %s:%d (function: %s)\n",	\
					__FILE__, __LINE__, __FUNCTION__);		\
			exit(1);	\
		}	\
	} while (0)

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


static void check_list_remove(void)
{
	int ret;
	hi_handle_t *hi_hndl;
	struct hi_init_set hi_set;
	void *data_ret;

	/** COLL_ENG_LIST **/
	fprintf(stderr, " o list remove test (COLL_ENG_LIST) ...");

	hi_set_zero(&hi_set);
	hi_set_bucket_size(&hi_set, 100);
	hi_set_hash_alg(&hi_set, HI_HASH_WEINB);
	hi_set_coll_eng(&hi_set, COLL_ENG_LIST);
	hi_set_key_cmp_func(&hi_set, hi_cmp_str);

	ret = hi_create(&hi_hndl, &hi_set);
	if (ret != 0)
		hi_perror("hi_create");
	xassert(!ret);

	if (hi_hndl->no_objects != 0) {
		fprintf(stderr, "failed! Wront number of objects ...");
		exit(1);
	}

	ret = hi_insert(hi_hndl, (void *) "key", strlen("key"), (void *) "DATA");
	xassert(!ret);
	ret = hi_insert(hi_hndl, (void *) "key2", strlen("key2"), (void *) "DATAX");
	xassert(!ret);
	ret = hi_insert(hi_hndl, (void *) "key3", strlen("key3"), (void *) "DATAX");
	xassert(!ret);

	/* key already in data structure -> must return 0 (SUCCESS) */
	ret = hi_remove(hi_hndl, (void *) "key", strlen("key"), &data_ret);
	if (ret != 0)
		hi_perror("hi_get");
	if (strcmp("DATA", data_ret)) {
		fprintf(stderr, "failed! Can't get key ...");
		exit(1);
	}

	if (hi_hndl->no_objects != 2) {
		fprintf(stderr, "failed! Wront number of objects ...");
		exit(1);
	}

	fprintf(stderr, " passed\n");

	/** COLL_ENG_LIST_MTF **/

	hi_handle_t *hi_hndl2;
	struct hi_init_set hi_set2;
	void *data_ret2;

	fprintf(stderr, " o list remove tests (COLL_ENG_LIST_MTF) ...");

	hi_set_zero(&hi_set2);
	hi_set_bucket_size(&hi_set2, 100);
	hi_set_hash_alg(&hi_set2, HI_HASH_WEINB);
	hi_set_coll_eng(&hi_set2, COLL_ENG_LIST_MTF);
	hi_set_key_cmp_func(&hi_set2, hi_cmp_str);

	ret = hi_create(&hi_hndl2, &hi_set2);
	if (ret != 0)
		hi_perror("hi_create");
	xassert(!ret);

	if (hi_hndl2->no_objects != 0) {
		fprintf(stderr, "failed! Wrong number of objects ...");
		exit(1);
	}

	ret = hi_insert(hi_hndl2, (void *) "key", strlen("key"), (void *) "DATA");
	xassert(!ret);

	ret = hi_insert(hi_hndl2, (void *) "key1", strlen("key1"), (void *) "DATAX");
	xassert(!ret);

	ret = hi_insert(hi_hndl2, (void *) "key2", strlen("key2"), (void *) "DATAX");
	xassert(!ret);

	if (hi_hndl2->no_objects != 3) {
		fprintf(stderr, "failed! Wrong number of objects ...");
		exit(1);
	}

	ret = hi_remove(hi_hndl2, (void *) "key", strlen("key"), &data_ret);
	if (ret != 0)
		hi_perror("hi_get");
	if (strcmp("DATA", data_ret)) {
		fprintf(stderr, "failed! Can't get key ...");
		exit(1);
	}

	if (hi_hndl2->no_objects != 2) {
		fprintf(stderr, "failed! Wrong number of objects ...");
		exit(1);
	}


	fprintf(stderr, " passed\n");
}


static void check_list_get(void)
{
	int ret;
	hi_handle_t *hi_hndl;
	struct hi_init_set hi_set;
	void *data_ret;

	/** COLL_ENG_LIST **/
	fprintf(stderr, " o list get test (COLL_ENG_LIST) ...");

	hi_set_zero(&hi_set);
	hi_set_bucket_size(&hi_set, 100);
	hi_set_hash_alg(&hi_set, HI_HASH_WEINB);
	hi_set_coll_eng(&hi_set, COLL_ENG_LIST);
	hi_set_key_cmp_func(&hi_set, hi_cmp_str);

	ret = hi_create(&hi_hndl, &hi_set);
	if (ret != 0)
		hi_perror("hi_create");
	xassert(!ret);

	ret = hi_insert(hi_hndl, (void *) "key", strlen("key"), (void *) "DATA");
	xassert(!ret);
	ret = hi_insert(hi_hndl, (void *) "key2", strlen("key2"), (void *) "DATAX");
	xassert(!ret);
	ret = hi_insert(hi_hndl, (void *) "key3", strlen("key3"), (void *) "DATAX");
	xassert(!ret);

	/* key already in data structure -> must return 0 (SUCCESS) */
	ret = hi_get(hi_hndl, (void *) "key", strlen("key"), &data_ret);
	if (ret != 0)
		hi_perror("hi_get");
	if (strcmp("DATA", data_ret)) {
		fprintf(stderr, "failed! Can't get key ...");
		exit(1);
	}

	fprintf(stderr, " passed\n");

	/** COLL_ENG_LIST_MTF **/

	hi_handle_t *hi_hndl2;
	struct hi_init_set hi_set2;
	void *data_ret2;

	fprintf(stderr, " o list get tests (COLL_ENG_LIST_MTF) ...");

	hi_set_zero(&hi_set2);
	hi_set_bucket_size(&hi_set2, 100);
	hi_set_hash_alg(&hi_set2, HI_HASH_WEINB);
	hi_set_coll_eng(&hi_set2, COLL_ENG_LIST_MTF);
	hi_set_key_cmp_func(&hi_set2, hi_cmp_str);

	ret = hi_create(&hi_hndl2, &hi_set2);
	if (ret != 0)
		hi_perror("hi_create");
	xassert(!ret);

	ret = hi_insert(hi_hndl2, (void *) "key", strlen("key"), (void *) "DATA");
	xassert(!ret);

	ret = hi_insert(hi_hndl2, (void *) "key1", strlen("key1"), (void *) "DATAX");
	xassert(!ret);

	ret = hi_insert(hi_hndl2, (void *) "key2", strlen("key2"), (void *) "DATAX");
	xassert(!ret);

	ret = hi_get(hi_hndl2, (void *) "key", strlen("key"), &data_ret);
	if (ret != 0)
		hi_perror("hi_get");
	if (strcmp("DATA", data_ret)) {
		fprintf(stderr, "failed! Can't get key ...");
		exit(1);
	}

	fprintf(stderr, " passed\n");


	/** COLL_ENG_LIST_HASH **/
	hi_handle_t *hi_hndl3;
	struct hi_init_set hi_set3;
	void *data_ret3;

	fprintf(stderr, " o list get tests (COLL_ENG_LIST_HASH) ...");

	hi_set_zero(&hi_set3);
	hi_set_bucket_size(&hi_set3, 100);
	hi_set_hash_alg(&hi_set3, HI_HASH_WEINB);
	hi_set_coll_eng(&hi_set3, COLL_ENG_LIST_MTF);
	hi_set_key_cmp_func(&hi_set3, hi_cmp_str);

	ret = hi_create(&hi_hndl3, &hi_set3);
	if (ret != 0)
		hi_perror("hi_create");
	xassert(!ret);

	ret = hi_insert(hi_hndl3, (void *) "key", strlen("key"), (void *) "DATA");
	xassert(!ret);

	ret = hi_insert(hi_hndl3, (void *) "key1", strlen("key1"), (void *) "DATAX");
	xassert(!ret);

	ret = hi_insert(hi_hndl3, (void *) "key2", strlen("key2"), (void *) "DATAX");
	xassert(!ret);

	ret = hi_get(hi_hndl3, (void *) "key", strlen("key"), &data_ret3);
	if (ret != 0)
		hi_perror("hi_get");
	if (strcmp("DATA", data_ret3)) {
		fprintf(stderr, "failed! Can't get key ...");
		exit(1);
	}

	fprintf(stderr, " passed\n");


	/** COLL_ENG_LIST_MTF_HASH: **/
	hi_handle_t *hi_hndl4;
	struct hi_init_set hi_set4;
	void *data_ret4;

	fprintf(stderr, " o list get tests (COLL_ENG_LIST_MTF_HASH) ...");

	hi_set_zero(&hi_set4);
	hi_set_bucket_size(&hi_set4, 100);
	hi_set_hash_alg(&hi_set4, HI_HASH_WEINB);
	hi_set_coll_eng(&hi_set4, COLL_ENG_LIST_MTF);
	hi_set_key_cmp_func(&hi_set4, hi_cmp_str);

	ret = hi_create(&hi_hndl4, &hi_set4);
	if (ret != 0)
		hi_perror("hi_create");
	xassert(!ret);

	ret = hi_insert(hi_hndl4, (void *) "key", strlen("key"), (void *) "DATA");
	xassert(!ret);

	ret = hi_insert(hi_hndl4, (void *) "key1", strlen("key1"), (void *) "DATAX");
	xassert(!ret);

	ret = hi_insert(hi_hndl4, (void *) "key2", strlen("key2"), (void *) "DATAX");
	xassert(!ret);

	ret = hi_get(hi_hndl4, (void *) "key", strlen("key"), &data_ret4);
	if (ret != 0)
		hi_perror("hi_get");
	if (strcmp("DATA", data_ret4)) {
		fprintf(stderr, "failed! Can't get key ...");
		exit(1);
	}

	fprintf(stderr, " passed\n");
}

static void check_list_insert(void)
{
	int ret;
	hi_handle_t *hi_hndl;
	struct hi_init_set hi_set;

	fprintf(stderr, " o list collision engine insertion  tests ...");

	hi_set_zero(&hi_set);
	hi_set_bucket_size(&hi_set, 100);
	hi_set_hash_alg(&hi_set, HI_HASH_WEINB);
	hi_set_coll_eng(&hi_set, COLL_ENG_LIST);
	hi_set_key_cmp_func(&hi_set, hi_cmp_str);

	ret = hi_create(&hi_hndl, &hi_set);
	if (ret != 0)
		hi_perror("hi_create");
	xassert(!ret);

	ret = hi_insert(hi_hndl, (void *) "key", strlen("key"), NULL);
	xassert(!ret);

	/* same key -> must fail */
	ret = hi_insert(hi_hndl, (void *) "key", strlen("key"), NULL);
	xassert(ret);

	/* key already in data structure -> must return 0 (SUCCESS) */
	ret = hi_lookup(hi_hndl, (void *) "key", strlen("key"));
	xassert(!ret);

	fprintf(stderr, " passed\n");
}

static void check_array_insert(void)
{
	int ret;
	hi_handle_t *hi_hndl;
	struct hi_init_set hi_set;

	/** COLL_ENG_ARRAY **/
	fprintf(stderr, " o array collision engine insertion  tests ...");

	hi_set_zero(&hi_set);
	hi_set_bucket_size(&hi_set, 100);
	hi_set_hash_alg(&hi_set, HI_HASH_WEINB);
	hi_set_coll_eng(&hi_set, COLL_ENG_ARRAY);
	hi_set_coll_eng_array_size(&hi_set, 5);
	hi_set_key_cmp_func(&hi_set, hi_cmp_str);

	ret = hi_create(&hi_hndl, &hi_set);
	if (ret != 0)
		hi_perror("hi_create");
	xassert(!ret);

	ret = hi_insert(hi_hndl, (void *) "key", strlen("key"), NULL);
	xassert(!ret);

	/* same key -> must fail */
	ret = hi_insert(hi_hndl, (void *) "key", strlen("key"), NULL);
	xassert(ret);

	/* key already in data structure -> must return 0 (SUCCESS) */
	ret = hi_lookup(hi_hndl, (void *) "key", strlen("key"));
	xassert(!ret);



	fprintf(stderr, " passed\n");
}

static void check_set_init(void)
{
	int ret;
	struct hi_init_set hi_set;

	fprintf(stderr, " o struct set initialize tests ...");

	hi_set_zero(&hi_set);

	/* test if a hash table size of 100
	 * return the right values */
	ret = hi_set_bucket_size(&hi_set, 100);
	xassert(!ret);

	/* trigger a failure - hash table size of
	 * 0 is invalid */
	ret = hi_set_bucket_size(&hi_set, 0);
	xassert(hi_geterror());
	xassert(ret);

	/* test for standard hashing algorithm - must pass */
	ret = hi_set_hash_alg(&hi_set, HI_HASH_WEINB);
	xassert(!ret);

	ret = hi_set_hash_func(&hi_set, dumb_hash_func);
	xassert(!ret);

	/* hash algorithm not supported test - must fail */
	ret = hi_set_hash_alg(&hi_set, (unsigned int) -1);
	xassert(hi_geterror());
	xassert(ret);

	/* test for standard collision engine - must pass */
	ret = hi_set_coll_eng(&hi_set, COLL_ENG_LIST);
	xassert(!ret);

	/* collision engine not supported test - must fail */
	ret = hi_set_coll_eng(&hi_set, (unsigned int) -1);
	xassert(hi_geterror());
	xassert(ret);

	/* test compare functions - must pass */
	ret = hi_set_key_cmp_func(&hi_set, hi_cmp_str);
	xassert(!ret);

	/* test compare functions - must fail */
	ret = hi_set_key_cmp_func(&hi_set, NULL);
	xassert(hi_geterror());
	xassert(!!ret);

	fprintf(stderr, " passed\n");
}


int
main(void)
{
	init_seed();

	fputs("Start test sequence\n", stderr);

	check_set_init();

	check_list_insert();
	check_array_insert();

	check_list_get();
	check_list_remove();

	return 0;
}

/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
