#include "mylib.h"
#include "lzss.h"

Lzss::Lzss(void)
{

}

Lzss::~Lzss(void)
{
}

void Lzss::initTree()
{
	int i;
	for (i = N + 1; i <= N + 256; i++) _rson[i] = NIL;
	for (i = 0; i < N; i++) _dad[i] = NIL;
}

void Lzss::insertNode(int r)
{
	int  i, p, cmp;
	unsigned char  *key;

	cmp = 1;  key = &_textBuf[r];  p = N + 1 + key[0];
	_rson[r] = _lson[r] = NIL;  _matchLen = 0;
	for ( ; ; ) {
		if (cmp >= 0) {
			if (_rson[p] != NIL) p = _rson[p];
			else {  _rson[p] = r;  _dad[r] = p;  return;  }
		} else {
			if (_lson[p] != NIL) p = _lson[p];
			else {  _lson[p] = r;  _dad[r] = p;  return;  }
		}
		for (i = 1; i < F; i++)
			if ((cmp = key[i] - _textBuf[p + i]) != 0)  break;
		if (i > _matchLen) {
			_matchPos = p;
			if ((_matchLen = i) >= F)  break;
		}
	}
	_dad[r] = _dad[p];  _lson[r] = _lson[p];  _rson[r] = _rson[p];
	_dad[_lson[p]] = r;  _dad[_rson[p]] = r;
	if (_rson[_dad[p]] == p) _rson[_dad[p]] = r;
	else                   _lson[_dad[p]] = r;
	_dad[p] = NIL;  /* remove p */
}

void Lzss::deleteNode(int p)
{
	int  q;

	if (_dad[p] == NIL) return;  /* not in tree */
	if (_rson[p] == NIL) q = _lson[p];
	else if (_lson[p] == NIL) q = _rson[p];
	else {
		q = _lson[p];
		if (_rson[q] != NIL) {
			do {  q = _rson[q];  } while (_rson[q] != NIL);
			_rson[_dad[q]] = _lson[q];  _dad[_lson[q]] = _dad[q];
			_lson[q] = _lson[p];  _dad[_lson[p]] = q;
		}
		_rson[q] = _rson[p];  _dad[_rson[p]] = q;
	}
	_dad[q] = _dad[p];
	if (_rson[_dad[p]] == p) _rson[_dad[p]] = q;  else _lson[_dad[p]] = q;
	_dad[p] = NIL;
}

size_t Lzss::encode(const unsigned char* indata, size_t inlen, 
		   unsigned char* outdata, size_t outlen)
{

	int  i, c, len, r, s, last_match_length, code_buf_ptr;
	unsigned char  code_buf[17], mask;
	size_t inpos = 0, outpos = 0;

	initTree();
	code_buf[0] = 0;
	code_buf_ptr = mask = 1;
	s = 0;  r = N - F;

	// memset(_textBuf, ' ', r - s);
	for (i = s; i < r; i++) _textBuf[i] = ' ';
											
	for (len = 0; len < F && inpos < inlen; len++) {
		c = indata[inpos ++];
		_textBuf[r + len] = c;  
	}

	if (len == 0) return 0;
	for (i = 1; i <= F; i++) insertNode(r - i);  
	insertNode(r);

	do {
		if (_matchLen > len) _matchLen = len; 

		if (_matchLen <= THRESHOLD) {
			_matchLen = 1;
			code_buf[0] |= mask; 
			code_buf[code_buf_ptr++] = _textBuf[r]; 
		} else {
			code_buf[code_buf_ptr++] = (unsigned char) _matchPos;
			code_buf[code_buf_ptr++] = (unsigned char)
				(((_matchPos >> 4) & 0xf0)
				| (_matchLen - (THRESHOLD + 1))); 

		}
		if ((mask <<= 1) == 0) { 
			for (i = 0; i < code_buf_ptr; i++) 
				outdata[outpos ++] = code_buf[i]; 

			code_buf[0] = 0;  code_buf_ptr = mask = 1;
		}
		last_match_length = _matchLen;
		for (i = 0; i < last_match_length && inpos < inlen; i++) {

				c = indata[inpos ++ ];
				deleteNode(s);
				_textBuf[s] = c;
				if (s < F - 1) _textBuf[s + N] = c;

				s = (s + 1) & (N - 1);  r = (r + 1) & (N - 1);

				insertNode(r);
			}

			while (i++ < last_match_length) {
				deleteNode(s);
				s = (s + 1) & (N - 1);  r = (r + 1) & (N - 1);
				if (-- len) insertNode(r);
			}
	} while (len > 0);

	if (code_buf_ptr > 1) {
		for (i = 0; i < code_buf_ptr; i++) 
			outdata[outpos ++] = code_buf[i];
	}

	return outpos;
}

size_t Lzss::decode(const unsigned char* indata, size_t inlen, 
		   unsigned char* outdata, size_t outlen)
{
	int  i, j, k, r, c;
	unsigned int  flags;
	size_t inpos = 0, outpos = 0;

	for (i = 0; i < N - F; i++) _textBuf[i] = ' ';
	// memset(_textBuf, ' ', N - F);
	r = N - F;  flags = 0;
	for ( ; ; ) {
		if (((flags >>= 1) & 256) == 0) {
			if ((c = indata[inpos ++], inpos > inlen)) break;
			flags = c | 0xff00;
		}
		if (flags & 1) {
			if ((c = indata[inpos ++], inpos > inlen)) break;
			outdata[outpos ++ ] = c;
			_textBuf[r++] = c;  r &= (N - 1);
		} else {
			if ((i = indata[inpos ++], inpos > inlen)) break;
			if ((j = indata[inpos ++], inpos > inlen)) break;
			i |= ((j & 0xf0) << 4);  j = (j & 0x0f) + THRESHOLD;
			for (k = 0; k <= j; k++) {
				c = _textBuf[(i + k) & (N - 1)];
				outdata[outpos ++ ] = c;
				_textBuf[r++] = c;  r &= (N - 1);
			}
		}
	}

	return outpos;
}
