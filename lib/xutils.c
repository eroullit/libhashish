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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#include "libhashish-local.h"
#include "libhashish.h"
#include "list.h"


int lhi_errno;
char *lhi_errbuf;

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



/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
