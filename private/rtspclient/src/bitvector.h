#ifndef _BITVECTOR_H_
#define _BITVECTOR_H_
#include "ptype.h"

class bitvector
{
private:
	unsigned char * bitstr;
	unsigned int bitpos;
        unsigned int origin;
	unsigned char curbitpos;
	unsigned int totalbit;
public:

	bitvector(U8 *,U8,U32);
	U32 read_bits(U8);
	void skip_bits(U8);
	U32 read_1bit();
        void setup(U8 *,U8,U32);
        U32 diff();
};


#endif

