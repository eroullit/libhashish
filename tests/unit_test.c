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

static void const_check(void)
{
	int i;
	int ret;
	void *data;
	char xstrting[] = TESTSTRING;
	hi_handle_t *hi_handle;
	char *ptr_bucket[TEST_ITER_NO][2];

	init_seed();

	fprintf(stderr, " o trivial CHAINING_LIST tests ...");

#define	TABLE_SIZE 2

	ret = hi_init_str(&hi_handle, TABLE_SIZE);
	xassert(!ret);

	ret = hi_insert_str(hi_handle, "test", NULL);
	xassert(!ret);

	/* same string, set associative -> MUST fail ;-) */
	ret = hi_insert_str(hi_handle, "test", NULL);
	xassert(ret);

	ret = hi_insert_str(hi_handle, "test1", NULL);
	ret = hi_insert_str(hi_handle, "test2", NULL);
	ret = hi_insert_str(hi_handle, "test3", NULL);
	ret = hi_insert_str(hi_handle, "test4", xstrting);

	hi_get_str(hi_handle, "test4", &data);
	/* No! Don't touch this if you don't know why double negate ;) */
	xassert(!!(data == xstrting));

	ret = hi_size(hi_handle);
	xassert(!(ret - 5));

	for (i = 0; i < TABLE_SIZE; i++) {
		lhi_bucket_remove(hi_handle, i);
	}
	ret = hi_size(hi_handle);
	xassert(!(ret));

	/* some trivial memory leak tests ... */
	for (i = 0; i < TEST_ITER_NO; i++) {
		char *key_ptr, *data_ptr;

		random_string(KEYLEN, &key_ptr);
		random_string(DATALEN, &data_ptr);

		ptr_bucket[i][KEY] = key_ptr;
		ptr_bucket[i][DATA] = data_ptr;

		hi_insert_str(hi_handle, key_ptr, data_ptr);
	}

	/* verify storage and cleanup */
	for (i = 0; i < TEST_ITER_NO; i++) {

		hi_get_str(hi_handle, ptr_bucket[i][KEY], &data);
		xassert(!!(data == ptr_bucket[i][DATA]));

		free(ptr_bucket[i][KEY]);
		free(ptr_bucket[i][DATA]);
	}

	hi_fini(hi_handle);

	fprintf(stderr, " passed\n");

#ifdef TABLE_SIZE
# undef TABLE_SIZE
# define TABLE_SIZE 8
#endif

	fprintf(stderr, " o trivial CHAINING_HASHLIST tests ...");

	ret = hi_init_str_hl(&hi_handle, TABLE_SIZE);
	xassert(!ret);

	ret = hi_insert_str(hi_handle, "test", NULL);
	xassert(!ret);

	/* same string, set associative -> MUST fail ;-) */
	ret = hi_insert_str(hi_handle, "test", NULL);
	xassert(ret);

	ret = hi_insert_str(hi_handle, "test1", NULL);
	ret = hi_insert_str(hi_handle, "test2", NULL);
	ret = hi_insert_str(hi_handle, "test3", NULL);
	ret = hi_insert_str(hi_handle, "test4", xstrting);

	hi_get_str(hi_handle, "test4", &data);
	/* No! Don't touch this if you don't know why double negate ;) */
	xassert(!!(data == xstrting));

	ret = hi_size(hi_handle);
	xassert(!(ret - 5));


	data = NULL;
	ret = hi_remove_str(hi_handle, "test4", &data);
	xassert(ret == SUCCESS);
	xassert(!!(data == xstrting));

	ret = hi_size(hi_handle);
	xassert(!(ret - 4));


	for (i = 0; i < TABLE_SIZE; i++) {
		lhi_bucket_remove(hi_handle, i);
	}
	ret = hi_size(hi_handle);
	xassert(!(ret));

	/* some trivial memory leak tests ... */
	for (i = 0; i < TEST_ITER_NO; i++) {
		char *key_ptr, *data_ptr;

		random_string(KEYLEN, &key_ptr);
		random_string(DATALEN, &data_ptr);

		ptr_bucket[i][KEY] = key_ptr;
		ptr_bucket[i][DATA] = data_ptr;

		hi_insert_str(hi_handle, key_ptr, data_ptr);
	}

	/* verify storage and cleanup */
	for (i = 0; i < TEST_ITER_NO; i++) {

		hi_get_str(hi_handle, ptr_bucket[i][KEY], &data);
		xassert(!!(data == ptr_bucket[i][DATA]));

		free(ptr_bucket[i][KEY]);
		free(ptr_bucket[i][DATA]);
	}

	hi_fini(hi_handle);

	fprintf(stderr, " passed\n");


}

#undef TABLE_SIZE

int
main(void)
{
	fputs("Start test sequence\n", stderr);
	const_check();

	return 0;
}

/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
