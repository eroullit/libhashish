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
#include <math.h>

struct {
	uint32_t (*hash)(const uint8_t *key, uint32_t len);
} lhi_hashmap[] = {
	{ lhi_hash_hsieh },
	{ lhi_hash_jenkins },
	{ lhi_hash_goulburn },
	{ lhi_hash_phong },
	{ lhi_hash_torek },
	{ lhi_hash_xor },
	{ lhi_hash_sdbm }
};


double hi_bloom_current_false_positiv_probability(hi_bloom_handle_t *bh)
{
	/* (1-e^(-kn/m))^k */
	return pow((1 - pow(M_E, -((double)bh->k * bh->n / bh->m))), (double)bh->k);
}


double hi_bloom_false_positiv_probability(uint32_t m, uint32_t n, uint32_t k)
{
	/* (1-e^(-kn/m))^k */
	return pow((1 - pow(M_E, -((double)k * n / m))), (double)k);
}


void hi_bloom_filter_add(hi_bloom_handle_t *bh, uint8_t *key, uint32_t len)
{
	uint32_t map_offset, bit, bit_mask, hkey, iter;

	/* track number of elements */
	bh->n++;

	/* TODO: could be unrolled via switch case and fall through ...
	 * but I am to lazy now ... --HGN */
	for (iter = 0; iter < bh->k; ++iter) {
		hkey = lhi_hashmap[iter].hash(key, len);
		bit  = hkey % bh->m;
		map_offset = floor(bit / 8);
		bit_mask = 1 << (bit & 7);
		if (bh->filter_map[map_offset] & bit_mask)
			bh->bit_collision++;
		bh->filter_map[map_offset] |= bit_mask;
	}
}
int hi_bloom_filter_check(hi_bloom_handle_t *bh, uint8_t *key, uint32_t len)
{
	uint32_t map_offset, bit_mask, hkey, iter;

	for (iter = 0; iter < bh->k; ++iter) {

		hkey = lhi_hashmap[iter].hash(key, len);
		map_offset = hkey % (bh->m / 8);
		bit_mask = 1 << (hkey & 0x7);
		if (!((bh->filter_map[map_offset] & bit_mask) == bit_mask))
			return 0; /* bit not set */
	}

	/* match! */
	return 1;
}

void hi_bloom_filter_add_str(hi_bloom_handle_t *a, const char *b)
{
	hi_bloom_filter_add(a, (uint8_t *) b, strlen(b));
}

int hi_bloom_filter_check_str(hi_bloom_handle_t *a, const char *b)
{
	return hi_bloom_filter_check(a, (uint8_t *) b, strlen(b));
}

/**
 * hi_bloom_bit_get returns if an specified bit
 * in the map is set
 *
 * @arg bh	this become out new hashish handle
 * @arg bit	the bit to check (start with bit 0 - of course)
 * @returns negativ error value or zero when not found and one when found
 */
int hi_bloom_bit_get(hi_bloom_handle_t *bh, uint32_t bit)
{
	uint32_t byte_offset, bit_offset;

	if (!bh)
		return HI_ERR_NODATA;

	if (bit > bh->m)
		return HI_ERR_RANGE;

	byte_offset = (uint32_t)(floor((double)bit / 8));
	bit_offset = (uint32_t)(bit % 8);

	return (!!(bh->filter_map[byte_offset] & (1 << bit_offset)));

}

int hi_bloom_print_hex_map(hi_bloom_handle_t *bh)
{
	uint32_t byte_offset = 0;

	if (!bh)
		return HI_ERR_NODATA;

	for (byte_offset = 0; byte_offset < bh->m / 8; ++byte_offset) {
		fprintf(stderr, "0x%X ", bh->filter_map[byte_offset]);
	}


	return SUCCESS;
}

/**
 * This is one initialize function for bloom filter.
 * This function must be called to initialize the bloom filter
 *
 * @arg bh	this become out new hashish handle
 * @arg m	hash bucket size
 * @arg k   number of hash algorithms to use
 * @returns negativ error value or zero on success
 */
int hi_bloom_init_mk(hi_bloom_handle_t **bh, uint32_t m, uint32_t k)
{
	int ret;
	hi_bloom_handle_t *nbh;

	if (m % 8 != 0 && m > 0) /* bit size must conform to byte boundaries */
		return HI_ERR_RANGE;

	if (ARRAY_SIZE(lhi_hashmap) < k)
		return HI_ERR_RANGE;

	ret = XMALLOC((void **) &nbh, sizeof(hi_bloom_handle_t));
	if (ret != 0) {
		return HI_ERR_SYSTEM;
	}
	memset(nbh, 0, sizeof(hi_bloom_handle_t));

	nbh->k = k;

	/* initialize filter_map */
	ret = XMALLOC((void **) &nbh->filter_map, m / 8);
	if (ret != 0) {
		return HI_ERR_SYSTEM;
	}
	memset(nbh->filter_map, 0, m / 8);

	nbh->m = m;

	*bh = nbh;

	return SUCCESS;
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


	nbh->m = bits;

	*bh = nbh;

	return SUCCESS;
}

void hi_fini_bloom_filter(hi_bloom_handle_t *bh)
{
	free(bh->filter_map);
	free(bh);
}

/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
