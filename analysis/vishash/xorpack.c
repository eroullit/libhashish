#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <gd.h>

static unsigned hamdist(int x, int y)
{
        unsigned int dist = 0, val = x ^ y;
        while (val) {
                ++dist;
                val &= val - 1;
        }
        return dist;
}


static uint32_t xorpack_shift(const void *s, size_t len)
{
	const char *bytes;
	const size_t *mem = s;
	uint32_t hash = len;
	uint32_t last = len << 23;

	while (len >  sizeof(size_t)) {
		hash ^= *mem;
		mem++;
		len -= sizeof(*mem);
		hash = (hash<< 27) | (hash >> 5);
	}

	bytes = (const char *) mem;
	switch (len) {
	case 4:	last |= (bytes[3] << last);
	case 3:	last ^= (bytes[2] << last);
	case 2:	last ^= (bytes[1] << last);
	case 1:	last ^= bytes[0] << last;
	hash ^= last;
	}
	return hash;
}


#if 0
static void swp(uint8_t *a, uint8_t *b) { uint8_t c = *a; *a = *b; *b = c; }
static uint8_t xrnd(void) { return random() & 0xFF; }

int main(void)
{
        uint8_t buf[256];
        int i;
        srandom(time(NULL) ^ getpid());

        for (i=0; i < 256; i++)
                buf[i] = i;

        for (i=0; i < 256; i++)
                swp(&buf[xrnd()], &buf[xrnd()]);

        for (i=0; i < 256; i++)
                printf("%u, ", buf[i]);
}
#endif


uint32_t pearson(const uint8_t *b, size_t len)
{
	uint32_t hash;
	size_t i;
	static uint8_t tab[] = {
		229, 111, 184, 21, 110, 245, 201, 226, 3, 9,
		32, 78, 50, 132, 11, 44, 70, 206, 18, 19, 185,
		124, 22, 23, 24, 1, 39, 27, 43, 85, 121, 113, 73,
		33, 177, 147, 91, 25, 48, 150, 224, 36, 106, 158,
		2, 134, 247, 53, 191, 140, 171, 88, 165, 29, 54, 42,
		131, 243, 8, 59, 186, 61, 143, 240, 179, 55, 66, 67,
		34, 69, 174, 102, 115, 249, 35, 216, 76, 235, 194,
		112, 205, 221, 82, 217, 120, 119, 89, 93, 183, 170,
		197, 83, 162, 64, 56, 157, 214, 255, 135, 99, 100,
		101, 28, 103, 104, 107, 198, 196, 97, 188, 236, 52,
		208, 213, 92, 84, 156, 118, 123, 94, 37, 154, 122,
		139, 212, 228, 51, 127, 195, 16, 40, 77, 161, 176,
		15, 130, 65, 141, 58, 182, 49, 81, 219, 164, 144, 87,
		146, 128, 10, 86, 152, 254, 26, 17, 189, 244, 116, 7,
		207, 159, 30, 193, 60, 232, 169, 153, 138, 108, 238, 222,
		12, 38, 181, 47, 199, 175, 0, 80, 75, 241, 180, 172, 71, 5,
		74, 72, 63, 187, 252, 90, 167, 96, 190, 209, 166, 148, 31, 126,
		210, 6, 14, 137, 253, 203, 204, 168, 151, 105, 4, 160, 79, 211,
		155, 125, 192, 215, 178, 62, 230, 142, 46, 242, 13, 223, 163,
		225, 237, 227, 98, 57, 239, 231, 149, 233, 117, 68, 246, 45,
		234, 133, 218, 136, 109, 20, 202, 114, 129, 220, 248, 200,
		250, 251, 95, 41, 145, 173 };

	for (i=0, hash=0; i<len; ++i)
		hash = tab[hash^b[i]];
	return hash;
}


uint32_t pearson_l(const uint8_t *b, size_t len)
{
	uint32_t hash, hash2 = len;
	size_t i;
	static uint8_t tab[] = {
		229, 111, 184, 21, 110, 245, 201, 226, 3, 9,
		32, 78, 50, 132, 11, 44, 70, 206, 18, 19, 185,
		124, 22, 23, 24, 1, 39, 27, 43, 85, 121, 113, 73,
		33, 177, 147, 91, 25, 48, 150, 224, 36, 106, 158,
		2, 134, 247, 53, 191, 140, 171, 88, 165, 29, 54, 42,
		131, 243, 8, 59, 186, 61, 143, 240, 179, 55, 66, 67,
		34, 69, 174, 102, 115, 249, 35, 216, 76, 235, 194,
		112, 205, 221, 82, 217, 120, 119, 89, 93, 183, 170,
		197, 83, 162, 64, 56, 157, 214, 255, 135, 99, 100,
		101, 28, 103, 104, 107, 198, 196, 97, 188, 236, 52,
		208, 213, 92, 84, 156, 118, 123, 94, 37, 154, 122,
		139, 212, 228, 51, 127, 195, 16, 40, 77, 161, 176,
		15, 130, 65, 141, 58, 182, 49, 81, 219, 164, 144, 87,
		146, 128, 10, 86, 152, 254, 26, 17, 189, 244, 116, 7,
		207, 159, 30, 193, 60, 232, 169, 153, 138, 108, 238, 222,
		12, 38, 181, 47, 199, 175, 0, 80, 75, 241, 180, 172, 71, 5,
		74, 72, 63, 187, 252, 90, 167, 96, 190, 209, 166, 148, 31, 126,
		210, 6, 14, 137, 253, 203, 204, 168, 151, 105, 4, 160, 79, 211,
		155, 125, 192, 215, 178, 62, 230, 142, 46, 242, 13, 223, 163,
		225, 237, 227, 98, 57, 239, 231, 149, 233, 117, 68, 246, 45,
		234, 133, 218, 136, 109, 20, 202, 114, 129, 220, 248, 200,
		250, 251, 95, 41, 145, 173 };

	for (i=0, hash=0; i<len; ++i) {
		//hash2 = (hash2<< 27) | (hash2 >> 5);
		hash2 += tab[hash] << (i & 0xf);
		hash2 ^= b[i] << (tab[hash] & 0xf);
		hash = tab[hash^b[i]];
		hash2 ^= hash;
	}
	return hash2;
}

#if (defined(__GNUC__) && defined(__i386__))
#define get16bits(d) (*((const uint16_t *) (d)))
#else
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

uint32_t HsiehSuperFastHash(const uint8_t *data, size_t len)
{
	uint32_t hash = len, tmp;
	int rem = len & 3;

	len >>= 2;

	for (;len > 0; len--) {
		hash  += get16bits (data);
		tmp    = (get16bits (data+2) << 11) ^ hash;
		hash   = (hash << 16) ^ tmp;
		data  += 2*sizeof (uint16_t);
		hash  += hash >> 11;
	}

	/* Handle end cases */
	switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= data[sizeof (uint16_t)] << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += *data;
                hash ^= hash << 10;
                hash += hash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}
#undef get16bits

#define hashsize(n) ((UINT32)1<<(n))
#define hashmask(n) (hashsize(n)-1)

#define mix(a,b,c) \
{ \
        a -= b; a -= c; a ^= (c>>13); \
        b -= c; b -= a; b ^= (a<<8);  \
        c -= a; c -= b; c ^= (b>>13); \
        a -= b; a -= c; a ^= (c>>12); \
        b -= c; b -= a; b ^= (a<<16); \
        c -= a; c -= b; c ^= (b>>5);  \
        a -= b; a -= c; a ^= (c>>3);  \
        b -= c; b -= a; b ^= (a<<10); \
        c -= a; c -= b; c ^= (b>>15); \
} /* mix */



static uint32_t
jenkins_hash( register uint8_t *k, register uint32_t length )
{
        /* k: the key
         * length: length of the key
         * initval: the previous hash, or an arbitrary value
         */
	uint32_t initval = length;
        register uint32_t a,b,c,len;

        /* Set up the internal state */
        len = length;
        a = b = 0x9e3779b9;     /* the golden ratio; an arbitrary value */
        c = initval;            /* the previous hash value */

        /* handle most of the key */
        while (len >= 12)
        {
                a += (k[0] +((uint32_t)k[1]<<8) +((uint32_t)k[2]<<16) +((uint32_t)k[3]<<24));
                b += (k[4] +((uint32_t)k[5]<<8) +((uint32_t)k[6]<<16) +((uint32_t)k[7]<<24));
                c += (k[8] +((uint32_t)k[9]<<8) +((uint32_t)k[10]<<16)+((uint32_t)k[11]<<24));
                mix(a,b,c);
                k += 12; len -= 12;
        }

        /* handle the last 11 bytes */
        c += length;
        switch( (int)len )      /* all the case statements fall through */
        {
                case 11: c+=((uint32_t)k[10]<<24);
                case 10: c+=((uint32_t)k[9]<<16);
                case 9 : c+=((uint32_t)k[8]<<8);
                /* the first byte of c is reserved for the length */
                case 8 : b+=((uint32_t)k[7]<<24);
                case 7 : b+=((uint32_t)k[6]<<16);
                case 6 : b+=((uint32_t)k[5]<<8);
                case 5 : b+=k[4];
                case 4 : a+=((uint32_t)k[3]<<24);
                case 3 : a+=((uint32_t)k[2]<<16);
                case 2 : a+=((uint32_t)k[1]<<8);
                case 1 : a+=k[0];
                /* case 0: nothing left to add */
        }
        mix(a,b,c);

        /* report the result */
        return c;
} /* jenkins_hash */


static void avalanche(void)
{
	uint32_t tmp, hash = 1, h0 = 0;
	int x = 32;
	uint32_t data[32];
	int i;

	for (i=1; i < sizeof(data); i++) {
		memset(data, 0, i);
		*data = hash;
		printf("hash %x (dist %d)\n", hash, hamdist(hash, h0));
		tmp = xorpack_shift(data, i);
		printf("%d xorpack_shift: %x (dist %d)\n", i, tmp, hamdist(tmp, hash));
		tmp = pearson(data, i);
		printf("%d pearson: %x (dist %d)\n", i, tmp, hamdist(tmp, hash));
		tmp = pearson_l(data, i);
		printf("%d pearson_l: %x (dist %d)\n", i, tmp, hamdist(tmp, hash));
		tmp = HsiehSuperFastHash(data, i);
		printf("%d Hsieh: %x (dist %d)\n", i, tmp, hamdist(tmp, hash));
		h0 = hash;
		hash += hash << 1;
	} while (x--);
}


void mySavePng(char *filename, gdImagePtr im)
{
	FILE *out;
	int size;
	char *data;
	out = fopen(filename, "wb");
	if (!out) {
/* Error */
	}
	data = (char *) gdImagePngPtr(im, &size);
	if (!data) {
/* Error */
	}
	if (fwrite(data, 1, size, out) != size) {
	/* Error */
	}
	if (fclose(out) != 0) {
	/* Error */
	}
	gdFree(data);
}


static void gen_image(unsigned x, unsigned y, uint32_t (*hashfun)(uint8_t *x, size_t l))
{
	unsigned i, j;
	gdImagePtr ptr = gdImageCreateTrueColor(x,y);

	if (!ptr) return;

	for (i = 0; i < x; i++) {
		uint32_t xy[2];
		for (j = 0; j < y; j++) {
			xy[0] = i; xy[1] = j;
			gdImageSetPixel(ptr, i, j, hashfun(xy, sizeof(xy)));
		}
	}
	mySavePng("/tmp/xxx.png", ptr);
}


int main(void)
{
	char buf[256];
	while (fgets(buf, sizeof(buf), stdin)) {
		size_t h;
		char *chr = strchr(buf, '\n');
		if (chr)
			*chr = 0;
		h = HsiehSuperFastHash(buf, strlen(buf));
		printf("%u %s\n", h, buf);
	}

	gen_image(512,512, pearson_l);
return 0;
	avalanche();

	return 0;
}

