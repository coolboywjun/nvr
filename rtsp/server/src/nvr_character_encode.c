
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "nvr_character_encode.h"

static void MD5Transform(UINT4 [4], const unsigned char [64]);
static void Encode(unsigned char *, UINT4 *, unsigned int);
static void Decode(UINT4 *, const unsigned char *, unsigned int);
//static void MD5_memcpy(unsigned char *output, unsigned char *input, unsigned int len);
//static void MD5_memset(unsigned char *output, int value, unsigned int len);

static const unsigned char PADDING[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
* Expanded DES S-boxes
*/
static const ulong SB1[64] =
{
	0x01010400, 0x00000000, 0x00010000, 0x01010404,
	0x01010004, 0x00010404, 0x00000004, 0x00010000,
	0x00000400, 0x01010400, 0x01010404, 0x00000400,
	0x01000404, 0x01010004, 0x01000000, 0x00000004,
	0x00000404, 0x01000400, 0x01000400, 0x00010400,
	0x00010400, 0x01010000, 0x01010000, 0x01000404,
	0x00010004, 0x01000004, 0x01000004, 0x00010004,
	0x00000000, 0x00000404, 0x00010404, 0x01000000,
	0x00010000, 0x01010404, 0x00000004, 0x01010000,
	0x01010400, 0x01000000, 0x01000000, 0x00000400,
	0x01010004, 0x00010000, 0x00010400, 0x01000004,
	0x00000400, 0x00000004, 0x01000404, 0x00010404,
	0x01010404, 0x00010004, 0x01010000, 0x01000404,
	0x01000004, 0x00000404, 0x00010404, 0x01010400,
	0x00000404, 0x01000400, 0x01000400, 0x00000000,
	0x00010004, 0x00010400, 0x00000000, 0x01010004
};

static const ulong SB2[64] =
{
	0x80108020, 0x80008000, 0x00008000, 0x00108020,
	0x00100000, 0x00000020, 0x80100020, 0x80008020,
	0x80000020, 0x80108020, 0x80108000, 0x80000000,
	0x80008000, 0x00100000, 0x00000020, 0x80100020,
	0x00108000, 0x00100020, 0x80008020, 0x00000000,
	0x80000000, 0x00008000, 0x00108020, 0x80100000,
	0x00100020, 0x80000020, 0x00000000, 0x00108000,
	0x00008020, 0x80108000, 0x80100000, 0x00008020,
	0x00000000, 0x00108020, 0x80100020, 0x00100000,
	0x80008020, 0x80100000, 0x80108000, 0x00008000,
	0x80100000, 0x80008000, 0x00000020, 0x80108020,
	0x00108020, 0x00000020, 0x00008000, 0x80000000,
	0x00008020, 0x80108000, 0x00100000, 0x80000020,
	0x00100020, 0x80008020, 0x80000020, 0x00100020,
	0x00108000, 0x00000000, 0x80008000, 0x00008020,
	0x80000000, 0x80100020, 0x80108020, 0x00108000
};

static const ulong SB3[64] =
{
	0x00000208, 0x08020200, 0x00000000, 0x08020008,
	0x08000200, 0x00000000, 0x00020208, 0x08000200,
	0x00020008, 0x08000008, 0x08000008, 0x00020000,
	0x08020208, 0x00020008, 0x08020000, 0x00000208,
	0x08000000, 0x00000008, 0x08020200, 0x00000200,
	0x00020200, 0x08020000, 0x08020008, 0x00020208,
	0x08000208, 0x00020200, 0x00020000, 0x08000208,
	0x00000008, 0x08020208, 0x00000200, 0x08000000,
	0x08020200, 0x08000000, 0x00020008, 0x00000208,
	0x00020000, 0x08020200, 0x08000200, 0x00000000,
	0x00000200, 0x00020008, 0x08020208, 0x08000200,
	0x08000008, 0x00000200, 0x00000000, 0x08020008,
	0x08000208, 0x00020000, 0x08000000, 0x08020208,
	0x00000008, 0x00020208, 0x00020200, 0x08000008,
	0x08020000, 0x08000208, 0x00000208, 0x08020000,
	0x00020208, 0x00000008, 0x08020008, 0x00020200
};

static const ulong SB4[64] =
{
	0x00802001, 0x00002081, 0x00002081, 0x00000080,
	0x00802080, 0x00800081, 0x00800001, 0x00002001,
	0x00000000, 0x00802000, 0x00802000, 0x00802081,
	0x00000081, 0x00000000, 0x00800080, 0x00800001,
	0x00000001, 0x00002000, 0x00800000, 0x00802001,
	0x00000080, 0x00800000, 0x00002001, 0x00002080,
	0x00800081, 0x00000001, 0x00002080, 0x00800080,
	0x00002000, 0x00802080, 0x00802081, 0x00000081,
	0x00800080, 0x00800001, 0x00802000, 0x00802081,
	0x00000081, 0x00000000, 0x00000000, 0x00802000,
	0x00002080, 0x00800080, 0x00800081, 0x00000001,
	0x00802001, 0x00002081, 0x00002081, 0x00000080,
	0x00802081, 0x00000081, 0x00000001, 0x00002000,
	0x00800001, 0x00002001, 0x00802080, 0x00800081,
	0x00002001, 0x00002080, 0x00800000, 0x00802001,
	0x00000080, 0x00800000, 0x00002000, 0x00802080
};

static const ulong SB5[64] =
{
	0x00000100, 0x02080100, 0x02080000, 0x42000100,
	0x00080000, 0x00000100, 0x40000000, 0x02080000,
	0x40080100, 0x00080000, 0x02000100, 0x40080100,
	0x42000100, 0x42080000, 0x00080100, 0x40000000,
	0x02000000, 0x40080000, 0x40080000, 0x00000000,
	0x40000100, 0x42080100, 0x42080100, 0x02000100,
	0x42080000, 0x40000100, 0x00000000, 0x42000000,
	0x02080100, 0x02000000, 0x42000000, 0x00080100,
	0x00080000, 0x42000100, 0x00000100, 0x02000000,
	0x40000000, 0x02080000, 0x42000100, 0x40080100,
	0x02000100, 0x40000000, 0x42080000, 0x02080100,
	0x40080100, 0x00000100, 0x02000000, 0x42080000,
	0x42080100, 0x00080100, 0x42000000, 0x42080100,
	0x02080000, 0x00000000, 0x40080000, 0x42000000,
	0x00080100, 0x02000100, 0x40000100, 0x00080000,
	0x00000000, 0x40080000, 0x02080100, 0x40000100
};

static const ulong SB6[64] =
{
	0x20000010, 0x20400000, 0x00004000, 0x20404010,
	0x20400000, 0x00000010, 0x20404010, 0x00400000,
	0x20004000, 0x00404010, 0x00400000, 0x20000010,
	0x00400010, 0x20004000, 0x20000000, 0x00004010,
	0x00000000, 0x00400010, 0x20004010, 0x00004000,
	0x00404000, 0x20004010, 0x00000010, 0x20400010,
	0x20400010, 0x00000000, 0x00404010, 0x20404000,
	0x00004010, 0x00404000, 0x20404000, 0x20000000,
	0x20004000, 0x00000010, 0x20400010, 0x00404000,
	0x20404010, 0x00400000, 0x00004010, 0x20000010,
	0x00400000, 0x20004000, 0x20000000, 0x00004010,
	0x20000010, 0x20404010, 0x00404000, 0x20400000,
	0x00404010, 0x20404000, 0x00000000, 0x20400010,
	0x00000010, 0x00004000, 0x20400000, 0x00404010,
	0x00004000, 0x00400010, 0x20004010, 0x00000000,
	0x20404000, 0x20000000, 0x00400010, 0x20004010
};

static const ulong SB7[64] =
{
	0x00200000, 0x04200002, 0x04000802, 0x00000000,
	0x00000800, 0x04000802, 0x00200802, 0x04200800,
	0x04200802, 0x00200000, 0x00000000, 0x04000002,
	0x00000002, 0x04000000, 0x04200002, 0x00000802,
	0x04000800, 0x00200802, 0x00200002, 0x04000800,
	0x04000002, 0x04200000, 0x04200800, 0x00200002,
	0x04200000, 0x00000800, 0x00000802, 0x04200802,
	0x00200800, 0x00000002, 0x04000000, 0x00200800,
	0x04000000, 0x00200800, 0x00200000, 0x04000802,
	0x04000802, 0x04200002, 0x04200002, 0x00000002,
	0x00200002, 0x04000000, 0x04000800, 0x00200000,
	0x04200800, 0x00000802, 0x00200802, 0x04200800,
	0x00000802, 0x04000002, 0x04200802, 0x04200000,
	0x00200800, 0x00000000, 0x00000002, 0x04200802,
	0x00000000, 0x00200802, 0x04200000, 0x00000800,
	0x04000002, 0x04000800, 0x00000800, 0x00200002
};

static const ulong SB8[64] =
{
	0x10001040, 0x00001000, 0x00040000, 0x10041040,
	0x10000000, 0x10001040, 0x00000040, 0x10000000,
	0x00040040, 0x10040000, 0x10041040, 0x00041000,
	0x10041000, 0x00041040, 0x00001000, 0x00000040,
	0x10040000, 0x10000040, 0x10001000, 0x00001040,
	0x00041000, 0x00040040, 0x10040040, 0x10041000,
	0x00001040, 0x00000000, 0x00000000, 0x10040040,
	0x10000040, 0x10001000, 0x00041040, 0x00040000,
	0x00041040, 0x00040000, 0x10041000, 0x00001000,
	0x00000040, 0x10040040, 0x00001000, 0x00041040,
	0x10001000, 0x00000040, 0x10000040, 0x10040000,
	0x10040040, 0x10000000, 0x00040000, 0x10001040,
	0x00000000, 0x10041040, 0x00040040, 0x10000040,
	0x10040000, 0x10001000, 0x10001040, 0x00000000,
	0x10041040, 0x00041000, 0x00041000, 0x00001040,
	0x00001040, 0x00040040, 0x10000000, 0x10041000
};

/*
* PC1: left and right halves bit-swap
*/
static const ulong LHs[16] =
{
	0x00000000, 0x00000001, 0x00000100, 0x00000101,
	0x00010000, 0x00010001, 0x00010100, 0x00010101,
	0x01000000, 0x01000001, 0x01000100, 0x01000101,
	0x01010000, 0x01010001, 0x01010100, 0x01010101
};

static const ulong RHs[16] =
{
	0x00000000, 0x01000000, 0x00010000, 0x01010000,
	0x00000100, 0x01000100, 0x00010100, 0x01010100,
	0x00000001, 0x01000001, 0x00010001, 0x01010001,
	0x00000101, 0x01000101, 0x00010101, 0x01010101,
};

/*----------------------------------------------*
* 宏定义                                       *
*----------------------------------------------*/
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

/* F, G, H and I are basic MD5 functions.
*/
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
*/
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
*/
#define FF(a, b, c, d, x, s, ac) { \
	(a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define GG(a, b, c, d, x, s, ac) { \
	(a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define HH(a, b, c, d, x, s, ac) { \
	(a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define II(a, b, c, d, x, s, ac) { \
	(a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

/*
* 32-bit integer manipulation macros (big endian)
*/
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                    \
{                                               \
    (n) = ( (ulong) (b)[(i)    ] << 24 )        \
	| ( (ulong) (b)[(i) + 1] << 16 )        \
	| ( (ulong) (b)[(i) + 2] <<  8 )        \
	| ( (ulong) (b)[(i) + 3]       );       \
	}
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                    \
{                                               \
    (b)[(i)    ] = (uchar) ( (n) >> 24 );       \
    (b)[(i) + 1] = (uchar) ( (n) >> 16 );       \
    (b)[(i) + 2] = (uchar) ( (n) >>  8 );       \
    (b)[(i) + 3] = (uchar) ( (n)       );       \
	}
#endif
/*
* Initial Permutation macro
*/
#define DES_IP(X,Y)                                             \
{                                                               \
    T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
    T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
    T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
    T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
    Y = ((Y << 1) | (Y >> 31)) & 0xFFFFFFFF;                    \
    T = (X ^ Y) & 0xAAAAAAAA; Y ^= T; X ^= T;                   \
    X = ((X << 1) | (X >> 31)) & 0xFFFFFFFF;                    \
	}

/*
* Final Permutation macro
*/
#define DES_FP(X,Y)                                             \
{                                                               \
    X = ((X << 31) | (X >> 1)) & 0xFFFFFFFF;                    \
    T = (X ^ Y) & 0xAAAAAAAA; X ^= T; Y ^= T;                   \
    Y = ((Y << 31) | (Y >> 1)) & 0xFFFFFFFF;                    \
    T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
    T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
    T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
    T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
}

/*
* DES round macro
*/
#define DES_ROUND(X,Y)                          \
{                                               \
    T = *SK++ ^ X;                              \
    Y ^= SB8[ (T      ) & 0x3F ] ^              \
	SB6[ (T >>  8) & 0x3F ] ^              \
	SB4[ (T >> 16) & 0x3F ] ^              \
	SB2[ (T >> 24) & 0x3F ];               \
	\
    T = *SK++ ^ ((X << 28) | (X >> 4));         \
    Y ^= SB7[ (T      ) & 0x3F ] ^              \
	SB5[ (T >>  8) & 0x3F ] ^              \
	SB3[ (T >> 16) & 0x3F ] ^              \
	SB1[ (T >> 24) & 0x3F ];               \
}

/*----------------------------------------------*
* 函数实现                                     *
*----------------------------------------------*/


/****************************************************************
base64解码函数

  src:源字符串
  src_len:源字符串的长度
  dst:解码后字符串的存放地址（需确保缓存空间大小大于3*src_len/4）
  dst_len:返回解码后字符串的长度
*****************************************************************/
void nvr_base64_decode(unsigned char *src, int src_len, char *dst, int *dst_len)
{
	int i = 0, j = 0;
	char base64_decode_map[256] =
	{
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,  62, 255, 255, 255,  63,  52,  53,  54,  55,  56,  57,  58,  59,
		60,  61, 255, 255, 255,   0, 255, 255, 255,   0,   1,   2,   3,   4,
		5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
		19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255, 255,  26,
		27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
		41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255
	};
	if(src_len % 4 != 0)		//make sure the src_len is
		src_len -= src_len % 4;
	for(; i < src_len; i += 4)
	{
		dst[j++] = base64_decode_map[src[i]] << 2 | base64_decode_map[src[i + 1]] >> 4;
		dst[j++] = base64_decode_map[src[i + 1]] << 4 | base64_decode_map[src[i + 2]] >> 2;
		dst[j++] = base64_decode_map[src[i + 2]] << 6 | base64_decode_map[src[i + 3]];
	}
	dst[j]   = '\0';
	if(dst_len)
		*dst_len = j;
}
/**************************************************************
base64编码函数

  src:源字符串
  src_len:源字符串的长度
  dst:编码后字符串的存放地址（需确保缓存空间大小大于4*src_len/3）
  dst_len:返回编码后字符串的长度
***************************************************************/
void nvr_base64_encode(unsigned char *src, int src_len, char *dst, int *dst_len)
{
	int i = 0, j = 0;
	char base64_map[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefgnvrjklmnopqrstuvwxyz0123456789+/";
	if(src == NULL || dst == NULL)
		return ;
	for(; i < src_len - src_len % 3; i += 3)
	{
		dst[j++] = base64_map[(src[i]  >> 2) & 0x3F];
		dst[j++] = base64_map[((src[i] << 4) & 0x30) + ((src[i + 1] >> 4) & 0xF)];
		dst[j++] = base64_map[((src[i + 1] << 2) & 0x3C) + ((src[i + 2] >> 6) & 0x3)];
		dst[j++] = base64_map[src[i + 2] & 0x3F];
	}
	if(src_len % 3 == 1)
	{
		dst[j++] = base64_map[(src[i] >> 2) & 0x3F];
		dst[j++] = base64_map[(src[i] << 4) & 0x30];
		dst[j++] = '=';
		dst[j++] = '=';
	}
	else if(src_len % 3 == 2)
	{
		dst[j++] = base64_map[(src[i] >> 2) & 0x3F];
		dst[j++] = base64_map[((src[i] << 4) & 0x30) + ((src[i + 1] >> 4) & 0xF)];
		dst[j++] = base64_map[(src[i + 1] << 2) & 0x3C];
		dst[j++] = '=';
	}
	dst[j] = '\0';
	if(dst_len)
		*dst_len = j;
}
/****************************************************************
url解码函数

  s:源字符串(解码后字符串也存在该地址)
  len:源字符串的长度
  返回值：解码后的字符串的长度
***************************************************************/
int nvr_url_decode(char *str, int len)
{
	char *dest = str;
	char *data = str;
	int value;
	int c;
	while(len--)
	{
		if(*data == '+')
		{
			*dest = ' ';
		}
		else if(*data == '%' && len >= 2 && isxdigit((int) * (data + 1))  && isxdigit((int) * (data + 2)))
		{
			c = ((unsigned char *)(data + 1))[0];
			if(isupper(c))
				c = tolower(c);
			value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;
			c = ((unsigned char *)(data + 1))[1];
			if(isupper(c))
				c = tolower(c);
			value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;
			*dest = (char)value ;
			data += 2;
			len -= 2;
		}
		else
		{
			*dest = *data;
		}
		data++;
		dest++;
	}
	*dest = '\0';
	return dest - str;
}
/**********************************************************
url编码函数

  s:源字符串
  len:源字符串的长度
  to:编码后字符存放的空间(空间大小 >= 3*len+1)
  new_length:编码后字符串的长度
  返回值：编码后的字符串（注意不用时需要释放该空间）
***********************************************************/
char *nvr_url_encode(char const *s, int len, unsigned char *to, int *new_length)
{
	unsigned char const *from, *end;
	unsigned char c;
	if(s == NULL || len == 0 || to == NULL)
		return NULL;
	from = (unsigned char *)s;
	end = (unsigned char *)s + len;
	unsigned char *start = NULL;
	unsigned char hexchars[] = "0123456789ABCDEF";
	start = to;
	while(from < end)
	{
		c = *from++;
		if(c == ' ')
		{
			*to++ = '+';
		}
		else if((c < '0' && c != '-' && c != '.') ||
		        (c < 'A' && c > '9') ||
		        (c > 'Z' && c < 'a' && c != '_') ||
		        (c > 'z'))
		{
			to[0] = '%';
			to[1] = hexchars[c >> 4];
			to[2] = hexchars[c & 15];
			to += 3;
		}
		else
		{
			*to++ = c;
		}
	}
	*to = 0;
	if(new_length)
	{
		*new_length = to - start;
	}
	return (char *) start;
}


/*******************************************************
md5加密函数
*******************************************************/
/* MD5 initialization. Begins an MD5 operation, writing a new context.
*/
void MD5_Init(struct md5_ctx *context)
{
	context->count[0] = context->count[1] = 0;
	/* Load magic initialization constants. */
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}

/* MD5 block update operation. Continues an MD5 message-digest
operation, processing another message block, and updating the
context.
*/
void MD5_Update(struct md5_ctx *context,     /* context */
                const unsigned char *input, /* input block */
                unsigned int inputLen)      /* length of input block */
{
	unsigned int i, bufindex, partLen;
	/* Compute number of bytes mod 64 */
	bufindex = (unsigned int)((context->count[0] >> 3) & 0x3F);
	/* Update number of bits */
	if((context->count[0] += ((UINT4)inputLen << 3))
	        < ((UINT4)inputLen << 3))
		context->count[1]++;
	context->count[1] += ((UINT4)inputLen >> 29);
	partLen = 64 - bufindex;
	/* Transform as many times as possible. */
	if(inputLen >= partLen)
	{
		memcpy((void *)&context->buffer[bufindex], (void *)input, partLen);
		MD5Transform(context->state, context->buffer);
		for(i = partLen; i + 63 < inputLen; i += 64)
			MD5Transform(context->state, &input[i]);
		bufindex = 0;
	}
	else
		i = 0;
	/* Buffer remaining input */
	memcpy((void *)&context->buffer[bufindex], (void *)&input[i], inputLen - i);
}

/* MD5 finalization. Ends an MD5 message-digest operation, writing the
the message digest and zeroizing the context.
*/
void MD5_Final(unsigned char digest[16], /* message digest */
               struct md5_ctx *context) /* context */
{
	unsigned char bits[8];
	unsigned int count, padLen;
	/* Save number of bits */
	Encode(bits, context->count, 8);
	/* Pad out to 56 mod 64. */
	count = (unsigned int)((context->count[0] >> 3) & 0x3f);
	padLen = (count < 56) ? (56 - count) : (120 - count);
	MD5_Update(context, PADDING, padLen);
	/* Append length (before padding) */
	MD5_Update(context, bits, 8);
	/* Store state in digest */
	Encode(digest, context->state, 16);
	/* Zeroize sensitive information. */
	memset((void *)context, 0, sizeof(*context));
}

/* MD5 basic transformation. Transforms state based on block. */
static void MD5Transform(UINT4 state[4],
                         const unsigned char block[64])
{
	UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];
	Decode(x, block, 64);
	/* Round 1 */
	FF(a, b, c, d, x[ 0], S11, 0xd76aa478);  /* 1 */
	FF(d, a, b, c, x[ 1], S12, 0xe8c7b756);  /* 2 */
	FF(c, d, a, b, x[ 2], S13, 0x242070db);  /* 3 */
	FF(b, c, d, a, x[ 3], S14, 0xc1bdceee);  /* 4 */
	FF(a, b, c, d, x[ 4], S11, 0xf57c0faf);  /* 5 */
	FF(d, a, b, c, x[ 5], S12, 0x4787c62a);  /* 6 */
	FF(c, d, a, b, x[ 6], S13, 0xa8304613);  /* 7 */
	FF(b, c, d, a, x[ 7], S14, 0xfd469501);  /* 8 */
	FF(a, b, c, d, x[ 8], S11, 0x698098d8);  /* 9 */
	FF(d, a, b, c, x[ 9], S12, 0x8b44f7af);  /* 10 */
	FF(c, d, a, b, x[10], S13, 0xffff5bb1);  /* 11 */
	FF(b, c, d, a, x[11], S14, 0x895cd7be);  /* 12 */
	FF(a, b, c, d, x[12], S11, 0x6b901122);  /* 13 */
	FF(d, a, b, c, x[13], S12, 0xfd987193);  /* 14 */
	FF(c, d, a, b, x[14], S13, 0xa679438e);  /* 15 */
	FF(b, c, d, a, x[15], S14, 0x49b40821);  /* 16 */
	/* Round 2 */
	GG(a, b, c, d, x[ 1], S21, 0xf61e2562);  /* 17 */
	GG(d, a, b, c, x[ 6], S22, 0xc040b340);  /* 18 */
	GG(c, d, a, b, x[11], S23, 0x265e5a51);  /* 19 */
	GG(b, c, d, a, x[ 0], S24, 0xe9b6c7aa);  /* 20 */
	GG(a, b, c, d, x[ 5], S21, 0xd62f105d);  /* 21 */
	GG(d, a, b, c, x[10], S22,  0x2441453);  /* 22 */
	GG(c, d, a, b, x[15], S23, 0xd8a1e681);  /* 23 */
	GG(b, c, d, a, x[ 4], S24, 0xe7d3fbc8);  /* 24 */
	GG(a, b, c, d, x[ 9], S21, 0x21e1cde6);  /* 25 */
	GG(d, a, b, c, x[14], S22, 0xc33707d6);  /* 26 */
	GG(c, d, a, b, x[ 3], S23, 0xf4d50d87);  /* 27 */
	GG(b, c, d, a, x[ 8], S24, 0x455a14ed);  /* 28 */
	GG(a, b, c, d, x[13], S21, 0xa9e3e905);  /* 29 */
	GG(d, a, b, c, x[ 2], S22, 0xfcefa3f8);  /* 30 */
	GG(c, d, a, b, x[ 7], S23, 0x676f02d9);  /* 31 */
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a);  /* 32 */
	/* Round 3 */
	HH(a, b, c, d, x[ 5], S31, 0xfffa3942);  /* 33 */
	HH(d, a, b, c, x[ 8], S32, 0x8771f681);  /* 34 */
	HH(c, d, a, b, x[11], S33, 0x6d9d6122);  /* 35 */
	HH(b, c, d, a, x[14], S34, 0xfde5380c);  /* 36 */
	HH(a, b, c, d, x[ 1], S31, 0xa4beea44);  /* 37 */
	HH(d, a, b, c, x[ 4], S32, 0x4bdecfa9);  /* 38 */
	HH(c, d, a, b, x[ 7], S33, 0xf6bb4b60);  /* 39 */
	HH(b, c, d, a, x[10], S34, 0xbebfbc70);  /* 40 */
	HH(a, b, c, d, x[13], S31, 0x289b7ec6);  /* 41 */
	HH(d, a, b, c, x[ 0], S32, 0xeaa127fa);  /* 42 */
	HH(c, d, a, b, x[ 3], S33, 0xd4ef3085);  /* 43 */
	HH(b, c, d, a, x[ 6], S34,  0x4881d05);  /* 44 */
	HH(a, b, c, d, x[ 9], S31, 0xd9d4d039);  /* 45 */
	HH(d, a, b, c, x[12], S32, 0xe6db99e5);  /* 46 */
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8);  /* 47 */
	HH(b, c, d, a, x[ 2], S34, 0xc4ac5665);  /* 48 */
	/* Round 4 */
	II(a, b, c, d, x[ 0], S41, 0xf4292244);  /* 49 */
	II(d, a, b, c, x[ 7], S42, 0x432aff97);  /* 50 */
	II(c, d, a, b, x[14], S43, 0xab9423a7);  /* 51 */
	II(b, c, d, a, x[ 5], S44, 0xfc93a039);  /* 52 */
	II(a, b, c, d, x[12], S41, 0x655b59c3);  /* 53 */
	II(d, a, b, c, x[ 3], S42, 0x8f0ccc92);  /* 54 */
	II(c, d, a, b, x[10], S43, 0xffeff47d);  /* 55 */
	II(b, c, d, a, x[ 1], S44, 0x85845dd1);  /* 56 */
	II(a, b, c, d, x[ 8], S41, 0x6fa87e4f);  /* 57 */
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0);  /* 58 */
	II(c, d, a, b, x[ 6], S43, 0xa3014314);  /* 59 */
	II(b, c, d, a, x[13], S44, 0x4e0811a1);  /* 60 */
	II(a, b, c, d, x[ 4], S41, 0xf7537e82);  /* 61 */
	II(d, a, b, c, x[11], S42, 0xbd3af235);  /* 62 */
	II(c, d, a, b, x[ 2], S43, 0x2ad7d2bb);  /* 63 */
	II(b, c, d, a, x[ 9], S44, 0xeb86d391);  /* 64 */
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	/* Zeroize sensitive information. */
	memset((void *)x, 0, sizeof(x));
}

/* Encodes input (UINT4) into output (unsigned char). Assumes len is
a multiple of 4.
*/
static void Encode(unsigned char *output,
                   UINT4 *input,
                   unsigned int len)
{
	unsigned int i, j;
	for(i = 0, j = 0; j < len; i++, j += 4)
	{
		output[j] = (unsigned char)(input[i] & 0xff);
		output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
		output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
		output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
	}
}

/* Decodes input (unsigned char) into output (UINT4). Assumes len is
a multiple of 4.
*/
static void Decode(UINT4 *output,
                   const unsigned char *input,
                   unsigned int len)
{
	unsigned int i, j;
	for(i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((UINT4)input[j]) | (((UINT4)input[j + 1]) << 8) |
		            (((UINT4)input[j + 2]) << 16) | (((UINT4)input[j + 3]) << 24);
}

/*****************************************************
outbuffer:加密后的字符串(固定长度为32)
input: 明文
*****************************************************/
void nvr_md5_encrypt(unsigned char *outbuffer, /* 16 bytes */
                     const unsigned char *input)
{
	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, input, (unsigned int)strlen((char *)input));
	MD5_Final(outbuffer, &ctx);
}
#if 0
/* Note: Replace "for loop" with standard memcpy if possible.
*/
static void
MD5_memcpy(unsigned char *output, unsigned char *input, unsigned int len)
{
	unsigned int i;
	for(i = 0; i < len; i++)
		output[i] = input[i];
}
/* Note: Replace "for loop" with standard memset if possible.
*/
static void
MD5_memset(unsigned char *output, int value, unsigned int len)
{
	unsigned int i;
	for(i = 0; i < len; i++)
		((char *) output)[i] = (char) value;
}
#endif

void nvr_md5_auth_cvtHex(HASH bin, HASHHEX hex)
{
	unsigned short i;
	unsigned char j;
	/*lint -save -e* */
	for(i = 0; i < HASHLEN; i++)
	{
		j = (bin[i] >> 4) & 0xf;
		if(j <= 9)
			hex[i * 2] = (j + '0');
		else
			hex[i * 2] = (j + 'a' - 10);
		j = bin[i] & 0xf;
		if(j <= 9)
			hex[i * 2 + 1] = (j + '0');
		else
			hex[i * 2 + 1] = (j + 'a' - 10);
	}
	/*lint -restore */
	hex[HASHHEALEXEN] = '\0';
}

void nvr_md5_auth_digest_CalcHA1(const char *authId, const char *password,
                                 unsigned char alg, const char *realm, const char *nonce,
                                 const char *cnonce, HASHHEX sessionKey)
{
	MD5_CTX md5Ctx;
	HASH ha1;
	MD5_Init(&md5Ctx);
	MD5_Update(&md5Ctx, (unsigned char *)authId, strlen(authId));
	MD5_Update(&md5Ctx, (unsigned char *)":", 1);
	MD5_Update(&md5Ctx, (unsigned char *)realm, strlen(realm));
	MD5_Update(&md5Ctx, (unsigned char *)":", 1);
	MD5_Update(&md5Ctx, (unsigned char *)password, strlen(password));
	MD5_Final((unsigned char *)ha1, &md5Ctx);
	if(EN_MD5AUTH_ALGO_SESS == alg)
	{
		MD5_Init(&md5Ctx);
		MD5_Update(&md5Ctx, (unsigned char *)ha1, HASHLEN);
		MD5_Update(&md5Ctx, (unsigned char *)":", 1);
		MD5_Update(&md5Ctx, (unsigned char *)nonce, strlen(nonce));
		MD5_Update(&md5Ctx, (unsigned char *)":", 1);
		MD5_Update(&md5Ctx, (unsigned char *)cnonce, strlen(cnonce));
		MD5_Final((unsigned char *)ha1, &md5Ctx);
	}
	nvr_md5_auth_cvtHex(ha1, sessionKey);
}

void nvr_md5_auth_digest_CalcResponse(HASHHEX ha1, const char *nonce, const char *nc,
                                      const char *cnonce, const char *qop, const char *method,
                                      const char *uri, HASHHEX entity, HASHHEX response)
{
	MD5_CTX md5Ctx;
	HASH ha2;
	HASH respHash;
	HASHHEX ha2Hex;
	/* calculate H(A2) */
	MD5_Init(&md5Ctx);
	MD5_Update(&md5Ctx, (unsigned char *)method, strlen(method));
	MD5_Update(&md5Ctx, (unsigned char *)":", 1);
	MD5_Update(&md5Ctx, (unsigned char *)uri, strlen(uri));
	if(qop == NULL)
	{
		MD5_Final((unsigned char *)ha2, &md5Ctx);
		nvr_md5_auth_cvtHex(ha2, ha2Hex);
		/* calculate response */
		MD5_Init(&md5Ctx);
		MD5_Update(&md5Ctx, (unsigned char *)ha1, HASHHEALEXEN);
		MD5_Update(&md5Ctx, (unsigned char *)":", 1);
		MD5_Update(&md5Ctx, (unsigned char *)nonce, strlen(nonce));
		MD5_Update(&md5Ctx, (unsigned char *)":", 1);
	}
	else
	{
		if(strcmp(qop, "auth-int") == 0)
		{
			MD5_Update(&md5Ctx, (unsigned char *)":", 1);
			MD5_Update(&md5Ctx, (unsigned char *)entity, HASHHEALEXEN);
		}
		else if(strcmp(qop, "auth") == 0)
		{
			MD5_Final((unsigned char *)ha2, &md5Ctx);
			nvr_md5_auth_cvtHex(ha2, ha2Hex);
		}
		/* calculate response */
		MD5_Init(&md5Ctx);
		MD5_Update(&md5Ctx, (unsigned char *)ha1, HASHHEALEXEN);
		MD5_Update(&md5Ctx, (unsigned char *)":", 1);
		MD5_Update(&md5Ctx, (unsigned char *)nonce, strlen(nonce));
		MD5_Update(&md5Ctx, (unsigned char *)":", 1);
		MD5_Update(&md5Ctx, (unsigned char *)nc, strlen(nc));
		MD5_Update(&md5Ctx, (unsigned char *)":", 1);
		MD5_Update(&md5Ctx, (unsigned char *)cnonce, strlen(cnonce));
		MD5_Update(&md5Ctx, (unsigned char *)":", 1);
		MD5_Update(&md5Ctx, (unsigned char *)qop, strlen(qop));
		MD5_Update(&md5Ctx, (unsigned char *)":", 1);
	}
	MD5_Update(&md5Ctx, (unsigned char *)ha2Hex, HASHHEALEXEN);
	MD5_Final((unsigned char *)respHash, &md5Ctx);
	nvr_md5_auth_cvtHex(respHash, response);
}

int nvr_md5_auth_build_resonse(char *resp, unsigned short maxlen,
                               unsigned char alg, const char *auth, const char *realm,
                               const char *pass, const char *nonce, const char *nc,
                               const char *cnonce, const char *qop,
                               const char *method, const char  *uri)
{
	HASHHEX ha1;
	HASHHEX ha2 = "";
	HASHHEX response;
	if(!resp || 0 == maxlen || EN_MD5AUTH_ALGO_INVALID <= alg || !auth || !realm ||
	        !pass || !nonce || !method || !uri)
	{
		return -1;
	}
	nvr_md5_auth_digest_CalcHA1(auth, pass, alg, realm, nonce, cnonce, ha1);
	nvr_md5_auth_digest_CalcResponse(ha1, nonce, nc, cnonce, qop, method, uri, ha2, response);
	snprintf(resp, maxlen, "%s", response);
	return 0;
}

void des_main_ks(ulong SK[32], uchar key[8])
{
	int i;
	ulong X, Y, T;
	GET_UINT32_BE(X, key, 0);
	GET_UINT32_BE(Y, key, 4);
	/*
	* Permuted Choice 1
	*/
	T = ((Y >>  4) ^ X) & 0x0F0F0F0F;
	X ^= T;
	Y ^= (T <<  4);
	T = ((Y) ^ X) & 0x10101010;
	X ^= T;
	Y ^= (T);
	X = (LHs[(X) & 0xF] << 3) | (LHs[(X >>  8) & 0xF ] << 2)
	    | (LHs[(X >> 16) & 0xF] << 1) | (LHs[(X >> 24) & 0xF ])
	    | (LHs[(X >>  5) & 0xF] << 7) | (LHs[(X >> 13) & 0xF ] << 6)
	    | (LHs[(X >> 21) & 0xF] << 5) | (LHs[(X >> 29) & 0xF ] << 4);
	Y = (RHs[(Y >>  1) & 0xF] << 3) | (RHs[(Y >>  9) & 0xF ] << 2)
	    | (RHs[(Y >> 17) & 0xF] << 1) | (RHs[(Y >> 25) & 0xF ])
	    | (RHs[(Y >>  4) & 0xF] << 7) | (RHs[(Y >> 12) & 0xF ] << 6)
	    | (RHs[(Y >> 20) & 0xF] << 5) | (RHs[(Y >> 28) & 0xF ] << 4);
	X &= 0x0FFFFFFF;
	Y &= 0x0FFFFFFF;
	/*
	* calculate subkeys
	*/
	for(i = 0; i < 16; i++)
	{
		if(i < 2 || i == 8 || i == 15)
		{
			X = ((X <<  1) | (X >> 27)) & 0x0FFFFFFF;
			Y = ((Y <<  1) | (Y >> 27)) & 0x0FFFFFFF;
		}
		else
		{
			X = ((X <<  2) | (X >> 26)) & 0x0FFFFFFF;
			Y = ((Y <<  2) | (Y >> 26)) & 0x0FFFFFFF;
		}
		*SK++ = ((X <<  4) & 0x24000000) | ((X << 28) & 0x10000000)
		        | ((X << 14) & 0x08000000) | ((X << 18) & 0x02080000)
		        | ((X <<  6) & 0x01000000) | ((X <<  9) & 0x00200000)
		        | ((X >>  1) & 0x00100000) | ((X << 10) & 0x00040000)
		        | ((X <<  2) & 0x00020000) | ((X >> 10) & 0x00010000)
		        | ((Y >> 13) & 0x00002000) | ((Y >>  4) & 0x00001000)
		        | ((Y <<  6) & 0x00000800) | ((Y >>  1) & 0x00000400)
		        | ((Y >> 14) & 0x00000200) | ((Y) & 0x00000100)
		        | ((Y >>  5) & 0x00000020) | ((Y >> 10) & 0x00000010)
		        | ((Y >>  3) & 0x00000008) | ((Y >> 18) & 0x00000004)
		        | ((Y >> 26) & 0x00000002) | ((Y >> 24) & 0x00000001);
		*SK++ = ((X << 15) & 0x20000000) | ((X << 17) & 0x10000000)
		        | ((X << 10) & 0x08000000) | ((X << 22) & 0x04000000)
		        | ((X >>  2) & 0x02000000) | ((X <<  1) & 0x01000000)
		        | ((X << 16) & 0x00200000) | ((X << 11) & 0x00100000)
		        | ((X <<  3) & 0x00080000) | ((X >>  6) & 0x00040000)
		        | ((X << 15) & 0x00020000) | ((X >>  4) & 0x00010000)
		        | ((Y >>  2) & 0x00002000) | ((Y <<  8) & 0x00001000)
		        | ((Y >> 14) & 0x00000808) | ((Y >>  9) & 0x00000400)
		        | ((Y) & 0x00000200) | ((Y <<  7) & 0x00000100)
		        | ((Y >>  7) & 0x00000020) | ((Y >>  3) & 0x00000011)
		        | ((Y <<  2) & 0x00000004) | ((Y >> 21) & 0x00000002);
	}
}

/*
* DES key schedule
*/
void nvr_des_set_key(des_context *ctx, uchar key[8])
{
	int i;
	des_main_ks(ctx->esk, key);
	for(i = 0; i < 32; i += 2)
	{
		ctx->dsk[i    ] = ctx->esk[30 - i];
		ctx->dsk[i + 1] = ctx->esk[31 - i];
	}
}

void des_crypt(ulong SK[32], uchar input[8], uchar output[8])
{
	ulong X, Y, T;
	GET_UINT32_BE(X, input, 0);
	GET_UINT32_BE(Y, input, 4);
	DES_IP(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_FP(Y, X);
	PUT_UINT32_BE(Y, output, 0);
	PUT_UINT32_BE(X, output, 4);
}

/*
* DES 64-bit block encryption (ECB)
*/
void nvr_des_encrypt(uchar *key, uchar input[8], uchar output[8])
{
	des_context ctx;
	nvr_des_set_key(&ctx,  key);
	des_crypt(ctx.esk, input, output);
}

/*
* DES 64-bit block decryption (ECB)
*/
void nvr_des_decrypt(uchar *key, uchar input[8], uchar output[8])
{
	des_context ctx;
	nvr_des_set_key(&ctx,  key);
	des_crypt(ctx.dsk, input, output);
}

/*
* DES-CBC encryption
*/
void nvr_des_cbc_encrypt(des_context *ctx, uchar iv[8],
                         uchar *input, uchar *output, uint len)
{
	int i;
	while(len > 0)
	{
		for(i = 0; i < 8; i++)
			output[i] = input[i] ^ iv[i];
		des_crypt(ctx->esk, output, output);
		memcpy(iv, output, 8);
		input  += 8;
		output += 8;
		len    -= 8;
	}
}

/*
* DES-CBC decryption
*/
void nvr_des_cbc_decrypt(des_context *ctx, uchar iv[8],
                         uchar *input, uchar *output, uint len)
{
	int i;
	uchar temp[8];
	while(len > 0)
	{
		memcpy(temp, input, 8);
		des_crypt(ctx->dsk, input, output);
		for(i = 0; i < 8; i++)
			output[i] = output[i] ^ iv[i];
		memcpy(iv, temp, 8);
		input  += 8;
		output += 8;
		len    -= 8;
	}
}

/*
* Triple-DES key schedule (112-bit)
*/
void nvr_des3_set_2keys(des3_context *ctx, uchar key[16])
{
	int i;
	des_main_ks(ctx->esk     , key);
	des_main_ks(ctx->dsk + 32, key + 8);
	for(i = 0; i < 32; i += 2)
	{
		ctx->dsk[i     ] = ctx->esk[30 - i];
		ctx->dsk[i +  1] = ctx->esk[31 - i];
		ctx->esk[i + 32] = ctx->dsk[62 - i];
		ctx->esk[i + 33] = ctx->dsk[63 - i];
		ctx->esk[i + 64] = ctx->esk[     i];
		ctx->esk[i + 65] = ctx->esk[ 1 + i];
		ctx->dsk[i + 64] = ctx->dsk[     i];
		ctx->dsk[i + 65] = ctx->dsk[ 1 + i];
	}
}

/*
* Triple-DES key schedule (168-bit)
*/
void nvr_des3_set_3keys(des3_context *ctx, uchar key[24])
{
	int i;
	des_main_ks(ctx->esk     , key);
	des_main_ks(ctx->dsk + 32, key +  8);
	des_main_ks(ctx->esk + 64, key + 16);
	for(i = 0; i < 32; i += 2)
	{
		ctx->dsk[i     ] = ctx->esk[94 - i];
		ctx->dsk[i +  1] = ctx->esk[95 - i];
		ctx->esk[i + 32] = ctx->dsk[62 - i];
		ctx->esk[i + 33] = ctx->dsk[63 - i];
		ctx->dsk[i + 64] = ctx->esk[30 - i];
		ctx->dsk[i + 65] = ctx->esk[31 - i];
	}
}

void des3_crypt(ulong SK[96], uchar input[8], uchar output[8])
{
	ulong X, Y, T;
	GET_UINT32_BE(X, input, 0);
	GET_UINT32_BE(Y, input, 4);
	DES_IP(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_ROUND(Y, X);
	DES_ROUND(X, Y);
	DES_FP(Y, X);
	PUT_UINT32_BE(Y, output, 0);
	PUT_UINT32_BE(X, output, 4);
}

/*
* Triple-DES 64-bit block encryption (ECB)
*/
void nvr_des3_encrypt(uchar *key, uchar input[8], uchar output[8])
{
	des3_context ctx;
	nvr_des3_set_3keys(&ctx, key);
	des3_crypt(ctx.esk, input, output);
}

/*
* Triple-DES 64-bit block decryption (ECB)
*/
void nvr_des3_decrypt(uchar *key, uchar input[8], uchar output[8])
{
	des3_context ctx;
	nvr_des3_set_3keys(&ctx, key);
	des3_crypt(ctx.dsk, input, output);
}

/*
* 3DES-CBC encryption
*/
void nvr_des3_cbc_encrypt(des3_context *ctx, uchar iv[8],
                          uchar *input, uchar *output, uint len)
{
	int i;
	while(len >= 8)
	{
		for(i = 0; i < 8; i++)
			output[i] = input[i] ^ iv[i];
		des3_crypt(ctx->esk, output, output);
		memcpy(iv, output, 8);
		input  += 8;
		output += 8;
		len    -= 8;
	}
}

/*
* 3DES-CBC decryption
*/
void nvr_des3_cbc_decrypt(des3_context *ctx, uchar iv[8],
                          uchar *input, uchar *output, uint len)
{
	int i;
	uchar temp[8];
	while(len > 0)
	{
		memcpy(temp, input, 8);
		des3_crypt(ctx->dsk, input, output);
		for(i = 0; i < 8; i++)
			output[i] = output[i] ^ iv[i];
		memcpy(iv, temp, 8);
		input  += 8;
		output += 8;
		len    -= 8;
	}
}
/***********************************************************
Hex 转 Dec 函数

  uc:16进制数
  返回值：成功返回16进制数对应的十进制数，失败返回-1
***********************************************************/
int	nvr_Hex_2_Dec(unsigned char uc)
{
	if(uc >= '0' && uc <= '9')
		return (uc - '0');
	else if(uc >= 'A' && uc <= 'F')
		return (uc - 'A' + 10);
	else if(uc >= 'a' && uc <= 'f')
		return (uc - 'a' + 10);
	else
		return -1;
}
/***********************************************************
16进制数转十进制数函数

  pSrc:16进制数
  len :16进制数长度
  pDest:转换后的十进制数

	返回值：成功返回转换后的十进制数的长度，失败返回-1
***********************************************************/
int	nvr_Hex_2_Asc(unsigned char *pSrc, int len, unsigned char *pDest)
{
	int	i = 0;
	char szBuf[4] = {0};
	if(pSrc == NULL || pDest == NULL || len == 0)
		return -1;
	for(i = 0; i < len / 2; i++, pSrc += 2)
	{
		memcpy(&szBuf[0], pSrc, 2);
		if(nvr_Hex_2_Dec(*pSrc) < 0 || nvr_Hex_2_Dec(*(pSrc + 1)) < 0)
			return -1;
		*(pDest + i) = (nvr_Hex_2_Dec(*pSrc) << 4 & 0xf0) | (nvr_Hex_2_Dec(*(pSrc + 1)) & 0xf);
	}
	return (len / 2);
}
/***********************************************************
Dec 转 Hex 函数

  uc:10进制数
  返回值：成功返回10进制数对应的16进制数，失败返回-1
***********************************************************/
unsigned char nvr_Dec_2_Hex(int val)
{
	if((val >= 0) && (val <= 9))
		return ('0' + val);
	else if((val >= 10) && (val <= 15))
		return ('A' + val - 10);
	return -1;
}
/***********************************************************
十进制数转16进制数函数

  pSrc:十进制数
  len :十进制数长度
  pDest:转换后的16进制数

	返回值：成功返回转换后的16进制数的长度，失败返回-1
***********************************************************/
int nvr_Asc_2_Hex(unsigned char *pSrc, int len, unsigned char *pDest)
{
	int	i = 0;
	for(i = 0; i < len ; i++, pSrc++)
	{
		*pDest = nvr_Dec_2_Hex((*pSrc & 0xf0) >> 4); //nvrgh four bits
		pDest++;
		*pDest = nvr_Dec_2_Hex(*pSrc & 0x0f);        //lwo four bits
		pDest++;
	}
	return (len * 2);
}
/***********************************************************
BCD码转Asc码函数

  pData:bcd 码字串

	返回值：成功返回转换后的值，失败返回-1
***********************************************************/
unsigned short nvr_Bcd_2_Asc(unsigned char *pData)
{
	unsigned short ret = 0;
	ret = ((pData[0] & 0xf0) >> 4) * 10 + (pData[0] & 0x0f);
	ret *= 100;
	ret += ((pData[1] & 0xf0) >> 4) * 10 + (pData[1] & 0x0f);
	return ret;
}
/***********************************************************
Asc码转BCD码函数

  pData:Ascii 码字串

	返回值：成功返回转换后的BCD码值，失败返回-1
***********************************************************/
unsigned char nvr_Asc_2_Bcd(unsigned char pData)
{
	if(pData >= 100)
	{
		return -1;
	}
	return ((pData / 10) << 4 | (pData % 10));
}

unsigned char nvr_check_crc(unsigned char *pData, int nLength)
{
	unsigned char cRc_16 = 0x0000;
	while(nLength > 0)
	{
		cRc_16 = cRc_16 ^ (*pData);
		nLength--;
		pData++;
	}
	return cRc_16;
}

