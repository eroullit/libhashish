#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/time.h>
#include <gd.h>

#include "libhashish.h"
#include "analysis_common.h"


static FILE *words_file;

static uint32_t xorpack(const uint8_t *s, size_t len)
{
	const uint32_t *mem;
	uint32_t last = 0xB4dF00d;
	uint32_t hash = 0x4551;
	uint32_t align = ((unsigned) s) & 3;
	uint32_t rem = len & 3;

	if (rem)
		memcpy(&last, &s[len - rem], rem);

	if (align) {
		memcpy(&hash, s, align);
		align = sizeof(uint32_t) - align;
		s += align;
		len -= align;
	}
	mem = (uint32_t *) s;
	hash ^= len;
	len >>= 2;
	while (len--) {
		hash ^= *mem;
		mem++;
	}
	return hash ^ last;
}


static void save_png(const char *filename, gdImagePtr im)
{
	FILE *out;
	int size;
	char *data;
	out = fopen(filename, "wb");
	if (!out) {
		perror("fopen");
		return;
	}
	data = (char *) gdImagePngPtr(im, &size);
	if (!data) {
		perror("gdImagePngPtr");
		fclose(out);
		return;
	}
	if (fwrite(data, 1, size, out) != (size_t) size)
		perror("fwrite");
	fclose(out);
	gdFree(data);
}


#define GEN_IMAGE(x, y, hashfun, name, hashdata) \
        gen_image((x), (y), hashfun, hashdata, name, #hashdata)

static void gen_image(unsigned x, unsigned y,
	uint32_t (*hashfun) (const uint8_t *, uint32_t),
	uint8_t* (*hashdata) (unsigned x, unsigned y, size_t *len),
	const char *hashfun_name,
	const char *hashdata_name)
{
	unsigned i, j;
	char filename[512];
	gdImagePtr ptr = gdImageCreateTrueColor(x,y);
	if (!ptr) return;

	for (i = 0; i < x; i++) {
		for (j = 0; j < y; j++) {
			size_t hash_len;
			uint8_t *data = hashdata(i, j, &hash_len);
			gdImageSetPixel(ptr, i, j, hashfun(data, hash_len));
		}
	}
	snprintf(filename, sizeof(filename), "%s-%s.png", hashfun_name, hashdata_name);
	save_png(filename, ptr);
	free(ptr);
}


static uint8_t *xy_concat(unsigned x, unsigned y, size_t *len)
{
	static unsigned xy[2];
	xy[0] = x;
	xy[1] = y;
	*len = sizeof(xy);
	return (uint8_t*) xy;
}


static uint8_t *counter(unsigned x, unsigned y, size_t *len)
{
	static unsigned long z;
	(void)x; (void)y;
	z++;
	*len = sizeof(z);
	return (uint8_t*) &z;
}


static uint8_t *getstring(unsigned x, unsigned y, size_t *len)
{
	static char buf[256];

	if (fgets(buf, sizeof(buf), words_file)) {
		char *chr = strchr(buf, '\n');
		if (chr)
			*chr = 0;
		*len = strlen(buf);
	} else {
		*len = 1;
	}
	(void) x;
	(void) y;
	return (uint8_t *)buf;
}


int main(int argc, char *argv[])
{
	unsigned int res_x = 448;
	unsigned int res_y = 448;
	hash_function_t fun;

	if (argc > 2) {
		words_file = fopen(argv[2], "r");
		if (!words_file)
			perror("fopen");
	}

	if (argc < 2)
		die_list();

	fun = get_hashfunc_by_name(argv[1]);
	if (!fun) {
		if (strcmp(argv[1], "xorpack"))
			die_list();
		fun = xorpack;
	}
	if (words_file) {
		GEN_IMAGE(480, 480, fun, argv[1], getstring);
	} else {
		GEN_IMAGE(res_x, res_y, fun, argv[1], xy_concat);
		GEN_IMAGE(res_x, res_y, fun, argv[1], counter);
	}
	return 0;
}
