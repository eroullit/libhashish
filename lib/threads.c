/*
** $Id: threads.c 11 2007-08-21 07:52:59Z hgndgtl $
**
** Copyright (C) 2007 - Hagen Paul Pfeifer <hagen@jauu.net>
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

#include "privlibhashish.h"
#include "libhashish.h"

#ifdef THREADSAFE

/** Initialize a mutex and perform no
 * error checking.
 */
int lhi_pthread_init(pthread_mutex_t **mutex,
		const pthread_mutexattr_t *attr)
{
	pthread_mutex_t *lm;

	lm = malloc(sizeof(pthread_mutex_t));
	if (!lm)
		return hi_errno(errno);

	pthread_mutex_init(lm, attr);

	*mutex = lm;

	return 0;
}

/** Destroy a mutex lock
 */
void lhi_pthread_destroy(pthread_mutex_t *mutex)
{
	pthread_mutex_destroy(mutex);
	free(mutex);
}


/** Lock a mutex
 */
void lhi_pthread_lock(pthread_mutex_t *mutex)
{
	pthread_mutex_lock(mutex);
}

/** Unlock a mutex
 */
void lhi_pthread_unlock(pthread_mutex_t *mutex)
{
	pthread_mutex_unlock(mutex);
}

#endif

/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
