/*
*file:	h264_rbsp.h
*description:
*		H264 buffer to raw byte sequence payload (rbsp)
*		reserve NAL struct, not used now.
*in: 	src=h264.data
*out:	buf=rbsp
*author:pxh
*creat on:
*		2012-10-24
*t.b.d.:
*/


#ifndef _H264_RBSP_H_
#define _H264_RBSP_H_

#include "ptype.h"
class rbsp
{
typedef struct NALU_HEAD
{
	U8 nal_unit_type;//  All u(5)
	U8 nal_ref_idc;//  All u(2) 
	U8 forbidden_zero_bit;//  All f(1)  	
}nalh;

private:

	nalh * ph;
	char * buf;
	char * src;
	unsigned int srclen;
	unsigned int buflen;
public:
	rbsp(char *,U32,char *,U32);
	int torbsp();
};


#endif
