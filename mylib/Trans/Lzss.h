#pragma once

class Lzss
{
public:
	Lzss(void);
	~Lzss(void);

	size_t encode(const unsigned char* indata, size_t inlen, 
		unsigned char* outdata, size_t outlen);
	size_t decode(const unsigned char* indata, size_t inlen, 
		unsigned char* outdata, size_t outlen);

protected:
	enum {
		N = 4096, 
		F = 18, 
		THRESHOLD = 2, 
		NIL = N,
	};

	void initTree();
	void insertNode(int r);
	void deleteNode(int p);

protected:

	unsigned char	_textBuf[N + F - 1];

	int				_matchPos;
	int				_matchLen;
	int				_lson[N + 1];
	int				_rson[N + 257];
	int				_dad[N + 1];
};
