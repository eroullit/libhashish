/*
** $Id: threads.h 13 2007-08-23 06:46:13Z hgndgtl $
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

#ifndef _LHI_THREADS_H
#define	_LHI_THREADS_H


#include "privlibhashish.h"
#include "libhashish.h"

#ifdef THREADSAFE
#define lhi_pthread_mutex_lock(a)	pthread_mutex_lock((a))
#define lhi_pthread_mutex_unlock(a)	pthread_mutex_unlock((a))
static int __attribute__((unused)) lhi_pthread_mutex_init(pthread_mutex_t **a ,  const  pthread_mutexattr_t *b)
{
	int ret = HI_ERR_SYSTEM;
	pthread_mutex_t *p = malloc(sizeof(*p));
	if (p) {
		ret = pthread_mutex_init(p, b);
		*a = p;
	}
	return ret;
}


static int __attribute__((unused)) lhi_pthread_mutex_destroy(pthread_mutex_t *a)
{
	int r = pthread_mutex_destroy(a);
	if (r == 0)
		free(a);
	return r;
}

#else /* THREADSAFE */

#define lhi_pthread_mutex_destroy(a)	do { } while(0)
#define lhi_pthread_mutex_init(a ,b )	do { } while(0)
#define lhi_pthread_mutex_lock(a)	do { } while(0)
#define lhi_pthread_mutex_unlock(a)	do { } while(0)

#endif


#endif /* _LHI_THREADS_H */


/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
