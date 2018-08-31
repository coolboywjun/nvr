#include "bitvector.h"
#define MAX_LENTH 32
#include <stdlib.h>
#include <string.h>

bitvector::bitvector(U8 * s,U8 bp,U32 tb)
{
	bitstr = s;
	bitpos = bp;
	curbitpos = bp&0x7;
	totalbit = tb;
        origin = bitpos;
}
void bitvector::setup(U8 * s,U8 bp, U32 tb)
{
    	bitstr = s;
	bitpos = bp;
	curbitpos = bp&0x7;
	totalbit = tb;
        origin = bitpos;
}

U32 bitvector::read_bits(U8 n)
{
	U8 temp[5] = {0};	
	if(n == 0) 
		return 0;
	if(n > MAX_LENTH)
		n = MAX_LENTH;
	if(bitpos + n > totalbit)
		n = totalbit - bitpos;
	
	U8 * curchar = &bitstr[bitpos>>3];
	U8 nbyte = (curbitpos+n+7)>>3;
	U8 suffixn = 8 -((curbitpos+n)&0x7);


	memcpy(&temp[5-nbyte],curchar,nbyte);
	unsigned long long ret;
	ret = temp[0]<<24;
	ret = ret<<8;
	ret |= (temp[1]<<24)|(temp[2]<<16)|(temp[3]<<8)|temp[4];

	ret = (ret>>suffixn)&((1ULL<<n)-1);
	
	U32 result = ret;
	bitpos+=n;
	curbitpos = bitpos&0x7;
	return result;
}

void bitvector::skip_bits(U8 n)
{
	if(n == 0) 
		return;
	if(n > MAX_LENTH)
		n = MAX_LENTH;
	if(n + bitpos > totalbit)
		n = totalbit - bitpos;

	bitpos += n;
	curbitpos = bitpos&0x7;
	return;
}

U32 bitvector::read_1bit()
{
	U8 * curchar = &bitstr[bitpos>>3];
	
	U8 suffixn = 7-curbitpos;
	bitpos++;
	curbitpos = bitpos&0x7;
	U32 result = ((*curchar)>>suffixn)&1;
	return result;
}

U32 bitvector::diff()
{
    return bitpos-origin;
}



