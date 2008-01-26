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
#include <time.h>

#include "libhashish.h"
#include "list.h"
#include "tests.h"


#define	RANDOMFILE "/dev/urandom"


unsigned int get_proper_seed(void)
{
	uint32_t rand_seed;
	int ufd = open(RANDOMFILE, O_RDONLY);
	if (ufd == -1) {
		return ((((int)time(NULL)) & ((1 << 30) - 1)) + getpid());
	} else {
		if (sizeof(rand_seed) != read(ufd, &rand_seed, sizeof(rand_seed))) {
			fprintf(stderr, "read(2) error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		close(ufd);
		return rand_seed;
	}
}


int random_string(uint32_t str_len, char **string, struct drand48_data *r_d)
{
	uint32_t i, retval = 0;
	char *newstring;
	long int result;

	newstring = malloc(str_len + 1);
	if (newstring == NULL) {
		fprintf(stderr, "malloc: %s\n", strerror(errno));
		return -1;
	}


	for (i = 0; i <= str_len; i++) {
		lrand48_r(r_d, &result);
		newstring[i] = (((int)result) % (122 - 97 + 1)) + 97;
	}
	newstring[str_len] = '\0';

	*string = newstring;

	return retval;
}



/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
