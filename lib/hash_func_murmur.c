/*
 * MurmurHashAligned, By Austin Appleby (aappleby (AT) gmail).
 * Source code is released to the public domain.
 * taken from http://murmurhash.googlepages.com/
 *
 * Same algorithm as MurmurHash, but only does aligned reads - should be safer
 * on certain platforms.
 * And it has a few limitations -
 * 1. It will not work incrementally.
 * 2. It will not produce the same results on little-endian and big-endian
 */

#include "libhashish.h"


static uint32_t murmur_hash(const void * key, uint32_t len, uint32_t seed)
{
	static const uint32_t m = 0x7fd652ad;
	static const int r = 16;
	const unsigned char * data = (const unsigned char *)key;
	unsigned int h = seed ^ (len * m);
	unsigned int align = (unsigned)data & 3;

	if (align) {
		uint32_t t = 0, d = 0;

		switch(align) {
		case 1: t = data[2] << 16;
		case 2: t |= data[1] << 8;
		case 3: t |= data[0];
		}

		t <<= (8 * align);

		data += 4-align;
	        if ((4-align) > len)
		        len -= 4-align;
		else
			len = 0;

		int sl = 8 * (4-align);
		int sr = 8 * align;

		while(len >= 4) {
			d = *(unsigned int *)data;
			t = (t >> sr) | (d << sl);
			h += t;
			h *= m;
			h ^= h >> r;
			t = d;

			data += 4;
			len -= 4;
		}

		/* Handle leftover data in temp registers */

		int pack = len < align ? len : align;

		d = 0;
		switch(pack) {
		case 3: d |= data[2] << 16;
		case 2: d |= data[1] << 8;
		case 1: d |= data[0];
		case 0:
			h += (t >> sr) | (d << sl);
			h *= m;
			h ^= h >> r;
		}

		data += pack;
		len -= pack;
	} else {
		while(len >= 4) {
			h += *(unsigned int *)data;
			h *= m;
			h ^= h >> r;

			data += 4;
			len -= 4;
		}
	}

	//----------
	// Handle tail bytes

	switch (len) {
	case 3: h += data[2] << 16;
	case 2: h += data[1] << 8;
	case 1: h += data[0];
	h *= m;
	h ^= h >> r;
	};

	h *= m;
	h ^= h >> 10;
	h *= m;
	h ^= h >> 17;

	return h;
}

uint32_t lhi_hash_murmur(const uint8_t *key, uint32_t len)
{
	return murmur_hash(key,len, 0);
}
