#include "exp_golomb.h"
#include <stdlib.h>
#include <stdio.h>


U32 ExpGolomb::parse_codenum()
{
    S8 leadingZeroBits = -1;
    for(U8 b=0;!b;leadingZeroBits++)
    {
        b=read_1bit();
    }
    U32 codeNum=(1UL<<leadingZeroBits) -1 + read_bits(leadingZeroBits);
    return codeNum;
}

ExpGolomb::ExpGolomb(U8 * s,U8 pb,U32 l):bitvector(s,pb,l)
{
	
}

U32 ExpGolomb::parse_ue()
{
    return parse_codenum();
}

S32 ExpGolomb::parse_se()
{
    S32 ret;
    U32 codenum = parse_codenum();
    ret = (codenum+1)>>1;
    ret = codenum&1?ret:-ret;
    return ret;
}






