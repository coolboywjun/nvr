#ifndef _HI_CHARACTER_ENCODE_H
#define _HI_CHARACTER_ENCODE_H

/*
* Copyright (c) 2010
* All rights reserved.
* 
* filename:hi_character_encode.h
* 
* func:本文件实现了base64 url 3des 3des crc 的编解码
*      md5加密 和 各进制数字的转换
* 
* author : hi
* createdate: 2010-10-10
*
* modify list: 
* 
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int UINT4;

/* MD5 context. */
struct md5_ctx 
{
	UINT4 state[4];					/* state (ABCD) */
	UINT4 count[2];		                     /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];			 /* input buffer */
};

typedef struct md5_ctx MD5_CTX;

#define uchar   unsigned char
#define uint    unsigned int
#define ulong   unsigned long int

typedef struct
{
    ulong esk[32];     /* DES encryption subkeys */
    ulong dsk[32];     /* DES decryption subkeys */
}des_context;

typedef struct
{
    ulong esk[96];     /* Triple-DES encryption subkeys */
    ulong dsk[96];     /* Triple-DES decryption subkeys */
}des3_context;

/*****************************************************
md5 加密函数

outbuffer:加密后的字符串(固定长度为32)
input: 明文
*****************************************************/

/* md5 auth algo type */
typedef enum
{
    EN_MD5AUTH_ALGO_MD5,
    EN_MD5AUTH_ALGO_SESS,
    EN_MD5AUTH_ALGO_INVALID
}EN_MD5AUTH_ALGO;

void hi_md5_encrypt(unsigned char *outbuffer, /* 16 bytes */
                const unsigned char *input);
void hi_md5_encrypt_bin(unsigned char *outbuffer, /* 16 bytes */
                const unsigned char *input, int inputlen);

#define HASHLEN         16
#define HASHHEALEXEN      32

typedef char HASH[HASHLEN];
typedef char HASHHEX[HASHHEALEXEN + 1];

void hi_md5_auth_cvt_hex( HASH bin, HASHHEX hex );

void hi_md5_auth_digest_calc_hA1(const char* authId, const char* password,
                             unsigned char alg, const char* realm, const char* nonce,
                             const char* cnonce, HASHHEX sessionKey );

void hi_md5_auth_digest_calc_response( HASHHEX ha1, const char* nonce, const char* nc,
                                 const char* cnonce, const char* qop, const char* method,
                                 const char* uri, HASHHEX entity, HASHHEX response );

int hi_md5_auth_build_resonse( char* resp, unsigned short maxlen,
                                     unsigned char alg, const char* auth, const char* realm,
                                     const char* pass, const char* nonce, const char* nc,
                                     const char* cnonce, const char* qop,
                                     const char* method,const char*  uri);

/*
 * DES-CBC decryption
 */
void hi_des_cbc_decrypt( des_context *ctx, uchar iv[8],
                      uchar *input, uchar *output, uint len );

/*
 * DES-CBC encryption
 */
void hi_des_cbc_encrypt( des_context *ctx, uchar iv[8],
                      uchar *input, uchar *output, uint len );

/*
 * Triple-DES 64-bit block encryption (ECB)
 */
void hi_des3_encrypt(uchar *key, uchar input[8], uchar output[8] );

/*
 * Triple-DES 64-bit block decryption (ECB)
 */
void hi_des3_decrypt(uchar *key, uchar input[8], uchar output[8]);

/*
 * 3DES-CBC encryption
 */
void hi_des3_cbc_encrypt( des3_context *ctx, uchar iv[8],
                       uchar *input, uchar *output, uint len );

/*
 * 3DES-CBC decryption
 */
void hi_des3_cbc_decrypt( des3_context *ctx, uchar iv[8],
                       uchar *input, uchar *output, uint len );

/****************************************************************
base64解码函数

src:源字符串
src_len:源字符串的长度
dst:解码后字符串的存放地址（需确保缓存空间大小大于3*src_len/4）
dst_len:返回解码后字符串的长度
*****************************************************************/		
void hi_base64_decode(unsigned char *src, int src_len, char *dst, int *dst_len);

/**************************************************************
base64编码函数

src:源字符串
src_len:源字符串的长度
dst:编码后字符串的存放地址（需确保缓存空间大小大于4*src_len/3）
dst_len:返回编码后字符串的长度
***************************************************************/
void hi_base64_encode(unsigned char *src, int src_len, char *dst, int *dst_len);

/****************************************************************
url解码函数

s:源字符串(解码后字符串也存在该地址)
len:源字符串的长度
返回值：解码后的字符串的长度
***************************************************************/
int hi_url_decode(char *str, int len);

/****************************************************************
url解码函数

s:源字符串(解码后字符串也存在该地址)
len:源字符串的长度
返回值：解码后的字符串的长度
***************************************************************/
int hi_url_decode(char *str, int len);

/***********************************************************
Hex 转 Dec 函数

uc:16进制数		
返回值：成功返回16进制数对应的十进制数，失败返回-1
***********************************************************/
int	hi_hex_2_dec(unsigned char uc);

/***********************************************************
16进制数转ASCII码函数

pSrc:16进制数
len :16进制数长度
pDest:转换后的十进制数

返回值：成功返回转换后的十进制数的长度，失败返回-1
***********************************************************/
int	hi_hex_2_asc(unsigned char *pSrc,int len,unsigned char *pDest);

/***********************************************************
Dec 转 Hex 函数

uc:10进制数		
返回值：成功返回10进制数对应的16进制数，失败返回-1
***********************************************************/
unsigned char hi_dec_2_hex(int val);

/***********************************************************
ASCII码转16进制数函数

pSrc:十进制数
len :十进制数长度
pDest:转换后的16进制数

返回值：成功返回转换后的16进制数的长度，失败返回-1
***********************************************************/
int hi_asc_2_hex(unsigned char *pSrc,int len,unsigned char *pDest);

/***********************************************************
BCD码转Asc码函数

pData:bcd 码字串

返回值：成功返回转换后的值，失败返回-1
***********************************************************/
unsigned short hi_bcd_2_asc(unsigned char *pData);

/***********************************************************
Asc码转BCD码函数

pData:Ascii 码字串

返回值：成功返回转换后的BCD码值，失败返回-1
***********************************************************/
unsigned char hi_asc_2_bcd(unsigned char pData);

/***********************************************************
获取CRC校验码

pData:校验数据
nLength:校验数据长度

返回值：CRC校验码
***********************************************************/
unsigned char hi_check_crc(unsigned char * pData, int nLength);
#ifdef __cplusplus
}
#endif

#endif

