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

#include "privlibhashish.h"
#include "libhashish.h"
#include "list.h"

#include "threads.h"

/**
 * hi_fini delete a complete hashish handle. Keys and data _aren't_ free'ed.
 *
 * @arg hi_handle the hashish handle
 * @return SUCCESS or a negativ return values in the case of an error
 */
int hi_fini(hi_handle_t *hi_handle)
{
	uint32_t ret;

	lhi_pthread_lock(hi_handle->mutex_lock);

	switch (hi_handle->coll_eng) {

		case COLL_ENG_LIST:
		case COLL_ENG_LIST_HASH:
		case COLL_ENG_LIST_MTF:
		case COLL_ENG_LIST_MTF_HASH:
			ret = lhi_fini_list(hi_handle);
			break;

		case COLL_ENG_ARRAY:
		case COLL_ENG_ARRAY_HASH:
		case COLL_ENG_ARRAY_DYN:
		case COLL_ENG_ARRAY_DYN_HASH:
			break;

		case COLL_ENG_RBTREE:
			break;

		default:
			return HI_ERR_INTERNAL;

	}

	lhi_pthread_unlock(hi_handle->mutex_lock);
	lhi_pthread_destroy(hi_handle->mutex_lock);

	/* cleanup the mutex lock */
	free(hi_handle);
	hi_handle = NULL;


	return SUCCESS;
}

/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
