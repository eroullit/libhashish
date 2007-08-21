/*
** $Id$
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


#include "libhashish-local.h"
#include "libhashish.h"

#ifdef THREADSAFE

void lhi_pthread_init(pthread_mutex_t *,
		const pthread_mutexattr_t *);
void lhi_pthread_destroy(pthread_mutex_t *);
void lhi_pthread_lock(pthread_mutex_t *);
void lhi_pthread_unlock(pthread_mutex_t *);
# else
static void lhi_pthread_init(pthread_mutex_t *a,
		const pthread_mutexattr_t *b) { }
static void lhi_pthread_destroy(pthread_mutex_t *a) { }
static void lhi_pthread_lock(pthread_mutex_t * a) { }
static void lhi_pthread_unlock(pthread_mutex_t * a) { }

#endif


#endif /* _LHI_THREADS_H */


/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
