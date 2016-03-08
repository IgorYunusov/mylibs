#include "mylib.h"
#include "tea.h"
#include <memory.h>

#define CRYPTO_GROUP_SIZE		8

Tea::Tea(const unsigned long * const key)
{
	_key[0] = key[0];
	_key[1] = key[1];
	_key[2] = key[2];
	_key[3] = key[3];
}

Tea::~Tea(void)
{
}

inline void Tea::encipher( unsigned long * const v, const unsigned long * const k )
{
	register unsigned long y = v[0], z = v[1], sum = 0, delta = 0x9E3779B9,
		a = k[0], b = k[1], c = k[2], d = k[3], n = 32;
	while ( n-- > 0 )
	{
		sum += delta;
		y += ( z << 4 ) + a ^ z + sum ^ ( z >> 5 ) + b;
		z += ( y << 4 ) + c ^ y + sum ^ ( y >> 5 ) + d;
	}
	v[0] = y;
	v[1] = z;
}

inline void Tea::decipher( unsigned long * const v, const unsigned long * const k )
{
	register unsigned long y = v[0], z = v[1], sum = 0xC6EF3720, delta = 0x9E3779B9,
		a = k[0], b = k[1], c = k[2], d = k[3], n = 32;
	// sum = delta << 5, in general sum = delta * n
	while ( n-- > 0 )
	{
		z -= ( y << 4 ) + c ^ y + sum ^ ( y >> 5 ) + d;
		y -= ( z << 4 ) + a ^ z + sum ^ ( z >> 5 ) + b;
		sum -= delta;
	}
	v[0] = y;
	v[1] = z;
}

size_t Tea::encode(const unsigned char* indata, size_t inlen, unsigned char* outdata, size_t outlen)
{
	size_t groups = (inlen + (CRYPTO_GROUP_SIZE - 1)) / CRYPTO_GROUP_SIZE;
	size_t needlen = groups * CRYPTO_GROUP_SIZE + 2;
	if (needlen > outlen)
		return 0;

	const unsigned long * const key = _key;

	outdata[0] = (unsigned char )(inlen >> 8);
	outdata[1] = (unsigned char )inlen;
	memcpy(outdata + 2, indata, inlen);
	for (size_t i = inlen + 2; i < needlen; i ++)
		outdata[i] = 0;

	unsigned char* p = outdata + 2;
	for (size_t i = 0; i < groups; i ++) {
        encipher((unsigned long* )p, key);
		p += CRYPTO_GROUP_SIZE;
	}

	return needlen;
}

size_t Tea::decode(const unsigned char* indata, size_t inlen, unsigned char* outdata, size_t outlen)
{
    size_t needlen = (((unsigned short )indata[0]) << 8) | indata[1];

	if (needlen > outlen)
		return 0;

	if ((inlen - 2) % CRYPTO_GROUP_SIZE) {
		return 0; // 非法的数据
	}

	const unsigned long * const key = _key;
	memcpy(outdata, indata + 2, inlen - 2);
	size_t groups = (inlen - 2) / CRYPTO_GROUP_SIZE;
	unsigned char* p = outdata;
	for (size_t i = 0; i < groups; i ++) {
		decipher((unsigned long* )p, key);
		p += CRYPTO_GROUP_SIZE;
	}

	return needlen;
}
