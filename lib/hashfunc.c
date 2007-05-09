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

#include "libhashish.h"

uint32_t lhi_hash_elf(const uint8_t *key, uint32_t len)
{
	uint32_t hash = 0, x = 0, i = 0;

	for (i = 0; i < len; key++, i++) {
		hash = (hash << 4) + (*key);
		if ((x = hash & 0xF0000000L) != 0)
			hash ^= (x >> 24);

		hash &= ~x;
	}

	return hash;
}

uint32_t lhi_hash_weinb(const uint8_t *key, uint32_t len)
{
	uint32_t hash = 0, test = 0, i = 0;
	const uint32_t bits_unsigned_int = (uint32_t) (sizeof(uint32_t) * 8);
	const uint32_t three_quarters = (uint32_t) ((bits_unsigned_int  * 3) / 4);
	const uint32_t eight_byte = (uint32_t) (bits_unsigned_int / 8);
	const uint32_t high_bits = (uint32_t) (0xFFFFFFFF) << (bits_unsigned_int - eight_byte);

	for (i = 0; i < len; key++, i++) {
		hash = (hash << eight_byte) + (*key);

		if ((test = hash & high_bits) != 0)
			hash = (( hash ^ (test >> three_quarters)) & (~high_bits));
	}

	return hash;
}


/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */

