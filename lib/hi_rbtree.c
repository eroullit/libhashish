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

/* lhi_lookup_rbtree search for a given key and return SUCCESS
 * when found in the hash and FAILURE if not found.
 *
 * @arg hi_handle the hashish handle
 * @arg key the pointer to the key
 * @arg keylen the len of the key in bytes
 * @return SUCCESS if found or FAILURE when not found
 */
int lhi_lookup_rbtree(const hi_handle_t *hi_handle,
		void *key, uint32_t keylen)
{

	(void) hi_handle;
	(void) key;
	(void) keylen;

	return HI_ERR_NOTIMPL;

}

/* lhi_insert_array insert a key/data pair into our hashhandle
 *
 * @arg hi_handle the hashish handle
 * @return SUCCESS or a negativ return values in the case of an error
 */
int lhi_insert_rbtree(hi_handle_t *hi_handle, void *key,
		uint32_t keylen, void *data)
{

	(void) hi_handle;
	(void) key;
	(void) keylen;
	(void) data;

	return HI_ERR_NOTIMPL;
}

int lhi_fini_rbtree(hi_handle_t *hi_handle)
{
	(void) hi_handle;

	return HI_ERR_NOTIMPL;
}

/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
