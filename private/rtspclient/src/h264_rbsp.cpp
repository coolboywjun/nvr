#include "h264_rbsp.h"

#define NEXT24BIT(p) ((p)[0]<<16|(p)[1]<<8|(p)[2])&0x0fff 
rbsp::rbsp(char * s,U32 sl,char * b,U32 bl)
{
	buf = b;
	src = s;
	srclen = sl;
	buflen = bl;
	
}

int rbsp::torbsp()
{
	ph = (nalh *)src;
	U32 pos = 1;
	U32 NumBytesInRBSP = 0;

	for( pos=1;pos<srclen&&NumBytesInRBSP<buflen-3;pos++)
	{
		if(pos + 2<srclen && ((NEXT24BIT(src+pos))== 0x000003))
		{
			buf[NumBytesInRBSP++]=src[pos];
			buf[NumBytesInRBSP++]=src[pos+1];
			pos += 2;
			/*ignore 0x00 0x00 (0x03) 0x0x*/
			//pos += 1;
		}
		else
		{
			buf[NumBytesInRBSP++]=src[pos];
		}
	}
	return NumBytesInRBSP;
	
}


