#ifndef _EXP_GOLOMB_H_
#define _EXP_GOLOMB_H_
#include "ptype.h"
#include "bitvector.h"
/*Exp-Golomb parsing function*/
class ExpGolomb:public bitvector
{
    private:
        U32 parse_codenum();
    public:
        ExpGolomb(U8 * s,U8 pb,U32 l);
        U32 parse_ue();
        S32 parse_se();
};

#endif

