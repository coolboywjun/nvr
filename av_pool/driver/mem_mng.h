#ifndef __MEM_MNG_H__
#define __MEM_MNG_H__

#include "av_pool_mng.h"
#include "av_pool_module.h"

// Please refrence mem_mng_init()后修改这个值
//#define MEM_BLOCK_NUM	4

typedef struct Tag_MemBlock
{
	unsigned long startPhysAddr;
	unsigned long startVirtAddr;
	unsigned long memBlockLen;
}MemBlock;

extern unsigned long block_start;
extern unsigned long block_end;
extern unsigned long block_len;
extern unsigned long block_virtp;
extern MemBlock mem_block[AV_POOL_MAX_SUPPORT_CHN][AV_POOL_MAX_SUPPORT_STREAM];

int mem_mng_init(void);

void mem_mng_exit(void);


#endif


