/*
** $Id$
**
** Copyright (C) 2008 - Hagen Paul Pfeifer <hagen@jauu.net>
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

#include "privlibhashish.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

void hi_bloom_filter_add(hi_bloom_handle_t *bh, uint8_t *key)
{
	uint32_t map_offset, bit_mask, hkey;

	hkey = lhi_hash_elf(key, strlen((char *)key));
	map_offset = hkey % bh->map_bit_size;
	bit_mask = 1 << (hkey & 0x7);
	bh->filter_map[map_offset] |= bit_mask;

	hkey = lhi_hash_torek(key, strlen((char *)key));
	map_offset = hkey % bh->map_bit_size;
	bit_mask = 1 << (hkey & 0x7);
	bh->filter_map[map_offset] |= bit_mask;

	hkey = lhi_hash_weinb(key, strlen((char *)key));
	map_offset = hkey % bh->map_bit_size;
	bit_mask = 1 << (hkey & 0x7);
	bh->filter_map[map_offset] |= bit_mask;

	hkey = lhi_hash_djb2(key, strlen((char *)key));
	map_offset = hkey % bh->map_bit_size;
	bit_mask = 1 << (hkey & 0x7);
	bh->filter_map[map_offset] |= bit_mask;
}

int hi_bloom_filter_check(hi_bloom_handle_t *bh, uint8_t *key)
{
	uint32_t map_offset, bit_mask, hkey;

	hkey = lhi_hash_elf(key, strlen((char *)key));
	map_offset = hkey % bh->map_bit_size;
	bit_mask = 1 << (hkey & 0x7);
	if (!((bh->filter_map[map_offset] & bit_mask) == bit_mask))
		return 0;

	hkey = lhi_hash_torek(key, strlen((char *)key));
	map_offset = hkey % bh->map_bit_size;
	bit_mask = 1 << (hkey & 0x7);
	if (!((bh->filter_map[map_offset] & bit_mask) == bit_mask))
		return 0;

	hkey = lhi_hash_weinb(key, strlen((char *)key));
	map_offset = hkey % bh->map_bit_size;
	bit_mask = 1 << (hkey & 0x7);
	if (!((bh->filter_map[map_offset] & bit_mask) == bit_mask))
		return 0;

	hkey = lhi_hash_djb2(key, strlen((char *)key));
	map_offset = hkey % bh->map_bit_size;
	bit_mask = 1 << (hkey & 0x7);
	if (!((bh->filter_map[map_offset] & bit_mask) == bit_mask))
		return 0;

	/* match! */
	return 1;
}

/**
 * This is the default initialize function for bloom filter.
 * This function must be called to initialize the bloom filter
 *
 * @arg bh	this become out new hashish handle
 * @arg bit_size	hash bucket size
 * @returns negativ error value or zero on success
 */
int hi_init_bloom_filter(hi_bloom_handle_t **bh, uint32_t bits)
{
	int ret;
	hi_bloom_handle_t *nbh;

	if (bits % 8 != 0 && bits > 0) /* bit size must conform to byte boundaries */
		return HI_ERR_NODATA;

	ret = XMALLOC((void **) &nbh, sizeof(hi_bloom_handle_t));
	if (ret != 0) {
		return HI_ERR_SYSTEM;
	}
	memset(nbh, 0, sizeof(hi_bloom_handle_t));

	/* initialize filter_map */
	ret = XMALLOC((void **) &nbh->filter_map, bits / 8);
	if (ret != 0) {
		return HI_ERR_SYSTEM;
	}
	memset(nbh->filter_map, 0, bits / 8);


	nbh->map_bit_size = bits;

	*bh = nbh;

	return SUCCESS;
}

void hi_fini_bloom_filter(hi_bloom_handle_t *bh)
{
	free(bh->filter_map);
	free(bh);
}

/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
