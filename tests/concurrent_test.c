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

#include <pthread.h>

#include "libhashish.h"
#include "list.h"
#include "tests.h"


#define MAXTHREAD 100

#define	TABLE_SIZE 23
#define	TEST_ITER_NO 2048
#define	KEYLEN  5
#define	DATALEN 100

#define	KEY 0
#define	DATA 1

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

hi_handle_t *hi_hndl;

/* concurrent_test do the following:
 *  o It creates and removes randomly entries
 *    whithin the hash table. At the end the count
 *    must be equal to the expected
 */
static void concurrent_test(int num)
{
	int i, ret;
	void *data;
	char *ptr_bucket[TEST_ITER_NO][2];


	for (i =0; i < TEST_ITER_NO; i++) {

		int sucess;
		char *key_ptr, *data_ptr;

		/* insert at least TEST_ITER_NO data
		 * sets
		 */
		do {
			sucess = 1;

			random_string(KEYLEN, &key_ptr);
			random_string(DATALEN, &data_ptr);

			ptr_bucket[i][KEY] = key_ptr;
			ptr_bucket[i][DATA] = data_ptr;

			ret = hi_insert(hi_hndl, (void *) key_ptr,
					strlen(key_ptr), (void *) data_ptr);
			if (ret != 0) {
				sucess = 0;
			}

		} while (!sucess);
	}

	/* verify storage and cleanup */
	for (i = 0; i < TEST_ITER_NO; i++) {


		ret = hi_get(hi_hndl, ptr_bucket[i][KEY],
				strlen(ptr_bucket[i][KEY]), &data);
		if (ret == 0) {
			if (data != ptr_bucket[i][DATA]) {
				fprintf(stderr, "Failed: should %s - is %s\n",
						ptr_bucket[i][DATA], (char *) data);
			}

			free(ptr_bucket[i][KEY]);
			free(ptr_bucket[i][DATA]);
		} else {
			fprintf(stderr, "# already deleted\n");
		}
	}

	fprintf(stderr, "-%d", num);

	return;
}

static void *thread_main(void *args)
{
	int num = (int) *((int *) args);
	fprintf(stderr, "+%d", num);

	switch(num % 2) {
		case 1:
			concurrent_test(num);
			break;
		case 0:
			concurrent_test(num);
			break;
		default:
			fputs("programmed error\n", stderr);
			exit(23);
			break;
	}

	return NULL;
}

int main(int ac, char **av)
{
	int ret = 0, i;
	pthread_t thread_id[MAXTHREAD];
	struct hi_init_set hi_set;

	(void) ac; (void) av;

	fputs("# concurrent test\n", stderr);

	init_seed();

	/* create one hash */
	hi_set_zero(&hi_set);
	hi_set_bucket_size(&hi_set, 100);
	hi_set_hash_alg(&hi_set, HI_HASH_WEINB);
	hi_set_coll_eng(&hi_set, COLL_ENG_LIST);
	hi_set_key_cmp_func(&hi_set, hi_cmp_str);

	ret = hi_create(&hi_hndl, &hi_set);
	if (ret != 0)
		hi_perror("hi_create");

	for	(i = 0; i < MAXTHREAD; i++) {
		int *num = malloc(sizeof(int *));
		if (!num) {
			perror("malloc");
			exit(1);
		}
		*num = i;
		pthread_create(&thread_id[i], NULL, thread_main, num);
	}

	fputs("# parent waiting for threads ...\n", stderr);
	for(i = 0; i < MAXTHREAD; i++) {
		pthread_join(thread_id[i], NULL);
	}

	hi_fini(hi_hndl);

	fprintf(stderr, " passed\n");

	return ret;
}



/* vim: set tw=78 ts=4 sw=4 sts=4 ff=unix noet: */
