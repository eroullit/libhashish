/*
** $Id: xutils.c 13 2007-08-23 06:46:13Z hgndgtl $
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

#define _GNU_SOURCE
#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#include "privlibhashish.h"
#include "libhashish.h"
#include "list.h"


int lhi_errno;
char *lhi_errbuf;


int xalloc_align(void **memptr, size_t alignment, size_t size)
{
#ifdef HAVE_POSIX_MEMALIGN
	return posix_memalign(memptr, alignment, size);
#else
	/* FIXME: here is a workaround necessary - but this needs more
	 * work then my current time window grants (and tomorrow is GPN6 ;-)
	 */
	*memptr = malloc(size);
	return (*memptr == NULL) ? -1 : 0;
#endif
}

int __hi_error(int err, const char *file, unsigned int line, const char *func,
		const char *fmt, ...)
{
	char *user_err;
	va_list args;

	if (lhi_errbuf) {
		free(lhi_errbuf);
		lhi_errbuf = NULL;
	}

	lhi_errno = err;

	if (fmt) {
		va_start(args, fmt);
		vasprintf(&user_err, fmt, args);
		va_end(args);
	}

	asprintf(&lhi_errbuf, "%s:%u:%s: %s (errno = %s)",
			file, line, func, fmt ? user_err : "", strerror(err));

	if (fmt)
		free(user_err);

	return -err;
}

const char *hi_geterror(void)
{
	if (lhi_errbuf)
		return lhi_errbuf;

	if (lhi_errno)
		return strerror(lhi_errno);

	return "Sucess\n";
}

void hi_perror(const char *s)
{
	if (s && *s)
		fprintf(stderr, "%s: %s\n", s, hi_geterror());
	else
		fprintf(stderr, "%s\n", hi_geterror());
}

size_t strlcpy(char *dest, const char *src, size_t size)
{
	size_t ret = strlen(src);

	if (size) {
		size_t len = (ret >= size) ? size - 1 : ret;
		memcpy(dest, src, len);
		dest[len] = '\0';
	}
	return ret;
}



/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
