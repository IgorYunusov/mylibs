#pragma once

class Tea
{
public:
	Tea(const unsigned long * const key);
	~Tea(void);

	size_t encode(const unsigned char* indata, size_t inlen, 
		unsigned char* outdata, size_t outlen);

	size_t decode(const unsigned char* indata, size_t inlen, 
		unsigned char* outdata, size_t outlen);

protected:
	void encipher( unsigned long * const v, const unsigned long * const k );
	void Tea::decipher( unsigned long * const v, const unsigned long * const k );

protected:
	unsigned long		_key[4]; // 128 bits
};
