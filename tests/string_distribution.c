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

#include "libhashish.h"
#include "libhashish-local.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#ifdef HAVE_LIBGD
# include <gd.h>
#endif

#define	MAX_STRING_LEN 30
#define	MIN_STRING_LEN 5

#define	DEFAULT_ENTRIES 5000
#define	DEFAULT_HASHTABLE_SIZE 100

#define	RANDOMFILE "/dev/urandom"


static int random_string( uint32_t str_len, char **string)
{
	uint32_t i, retval = 0;
	char *newstring;

	newstring = malloc(str_len + 1);
	if (newstring == NULL) {
		fprintf(stderr, "malloc: %s\n", strerror(errno));
		return -1;
	}

	for (i = 0; i <= str_len; i++) {
		newstring[i] = (rand() % (122 - 97 + 1)) + 97;
	}
	newstring[str_len] = '\0';

	*string = newstring;

	return retval;
}



int main(int argc, char **argv)
{

	int ufd, ret, verbose = 0; unsigned int i;
	uint32_t rand_seed;
	char *affix = NULL;
	char *prefix = NULL;
	int opt = 0;
	int len = 0;
	char *png_filename = NULL;
	unsigned int entries = DEFAULT_ENTRIES;
	unsigned int table_size = DEFAULT_HASHTABLE_SIZE;
	hi_handle_t *hi_handle;

	while ((opt = getopt(argc, argv, "qn:l:p:t:a:g:v")) != -1) {
		switch (opt) {
		case 'q':
			{
			int max_fd, std_fd, j;
			if ((max_fd = (int) sysconf(_SC_OPEN_MAX)) < 0) {
				max_fd = 256;
			}
			for (j = max_fd - 1; j >= 0; --j) {
				close(j);
			}
			std_fd = open("/dev/null", O_RDWR);
			dup(std_fd);
			dup(std_fd);
			}
			break;
		case 'n':
			entries = atoi(optarg);
			break;
		case 'l':
			len = atoi(optarg);
			break;
		case 'p':
			prefix = strdup(optarg);
			break;
		case 't':
			table_size = atoi(optarg);
			break;
		case 'v':
			verbose++;
			break;
		case 'a':
			affix = strdup(optarg);
			break;
		case 'g':
#ifdef HAVE_LIBGD
			png_filename = strdup(optarg);
# else
			fprintf(stderr, "sorry - you build without gd library support\n");
			exit(1);
#endif
			break;
		case '?':
			fprintf(stderr, "No such option: `%c'\n\n", optopt);
			exit(1);
			break;
		}
	}

	fputs("# String Distribution Hash Test\n", stderr);

	ufd = open(RANDOMFILE, O_RDONLY);
	if (ufd == -1) {
		srand((((int)time(NULL)) & ((1 << 30) - 1)) + getpid());
	} else {
		if (sizeof(rand_seed) != read(ufd, &rand_seed, sizeof(rand_seed))) {
			fprintf(stderr, "read(2) error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		close(ufd);
		rand_seed = abs(rand_seed) + getpid();
		srand(rand_seed);
	}

	/* initialize hash table */
	ret = hi_init_str(&hi_handle, table_size);

	/* fill hash table */
	for(i = 0; i < entries; i++) {

		char *key = NULL, *tmp_key;
		size_t key_len = 0;

		/* compound key */
		if (len == 0) {
			len = (rand() % (MAX_STRING_LEN - MIN_STRING_LEN + 1)) + MIN_STRING_LEN;
		}
		if (random_string(len, &tmp_key) < 0)
			exit(EXIT_FAILURE);

		if (prefix)
			key_len += strlen(prefix);
		key_len += strlen(tmp_key);
		if (affix)
			key_len += strlen(affix);


		key = malloc(key_len + 1);
		if (key == NULL) {
			fprintf(stderr, "malloc %s\n", strerror(errno));
			exit(1);
		}

		if (prefix != NULL) {
			sprintf(&key[0], "%s%s", prefix, tmp_key);
		} else {
			sprintf(key, "%s", tmp_key);
		}


		free(tmp_key);
		tmp_key = NULL;

		if (affix)
			strcat(key, affix);

		ret = hi_insert_str(hi_handle, (void *) key, NULL);
		if (ret < 0)
			fprintf(stderr, "# WARNING: Can't insert key (maybe a duplicated key: %s)!\n", key);
	}

	/* print statistic */
	ret = hi_size(hi_handle);
	fprintf(stderr, "# hash table entries: %d\n", ret);

	if (png_filename) { /* grapical output */

#ifdef HAVE_LIBGD
# define RECT_SIZE 5
# define RECT_BODER_SIZE 1

		int j;
		uint32_t x, y;
		gdImagePtr im;
		FILE *pngout;
		int black, white, red, max_list_len = 0;


		/* calculate maximum listsize */
		for(i = 0; i < table_size; i++) {
			int tmp_bucket_size = hi_bucket_size(hi_handle, i);
			max_list_len = max(max_list_len, tmp_bucket_size);
		}


		/* create a image with max_list_len X table_size */
		im = gdImageCreate((max_list_len * (RECT_SIZE + RECT_BODER_SIZE * 2)) + 2,
				(table_size * ((RECT_SIZE + RECT_BODER_SIZE * 2)) + 2));

		black = gdImageColorAllocate(im, 255, 231, 186);
		white = gdImageColorAllocate(im, 255, 165, 79);
		red   = gdImageColorAllocate(im, 205, 102, 29);

		x = 1;
		y = 1;

		for (i = 0; i < table_size; i++) {
			int bucket_size =  hi_bucket_size(hi_handle, i);
			for (j = 0; j < bucket_size; j++) {
				gdImageFilledRectangle(im, x + 1, y + 1, x + RECT_SIZE, y + RECT_SIZE, white);
				gdImageRectangle(im, x, y, x + RECT_SIZE + RECT_BODER_SIZE * 2, y + RECT_SIZE + RECT_BODER_SIZE * 2, red);
				x += RECT_SIZE + RECT_BODER_SIZE * 2;
			}
			x = 1;
			y += RECT_SIZE + RECT_BODER_SIZE * 2;
		}

		pngout = fopen(png_filename, "wb");
		gdImagePng(im, pngout);
		fclose(pngout);
		gdImageDestroy(im);

# undef RECT_SIZE
# undef RECT_BODER_SIZE
#endif /* HAVE_LIBGD */

	}

	if (verbose >= 1) {  /* terminal output */
		for(i = 0; i < table_size; i++) {
			fprintf(stderr, "bucket no: %d bucket size: %d\n",
					i, hi_bucket_size(hi_handle, i));
		}

	}

	/* delete table */
	hi_fini(hi_handle);

	return 0;
}


/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
