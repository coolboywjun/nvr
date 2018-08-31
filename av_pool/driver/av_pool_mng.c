

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <asm/uaccess.h>

#include "av_pool_mng.h"
#include "mem_mng.h"

static JPOOL_MAN	gPoolMan[AV_POOL_MAX_SUPPORT_POOL];

#define JPOOL_MAGIC_FLAG 0x87654321
#define DMA_PARAM_NO_LINK          (0xFFFF)
#define DMA_PARAM_OPT_SYNCDIM_AB   (1<<2)
#define DMA_PARAM_OPT_STATIC       (1<<3)
#define DMA_PARAM_OPT_TCINTEN      (1<<20)
#define DMA_PARAM_OPT_ITCCHEN      (1<<23)

enum
{
	AV_VIDEO_FRAME_I = 0x7,
	AV_VIDEO_FRAME_P = 0x8,
	AV_VIDEO_FRAME_B = 0xB,
	AV_AUDIO_FRAME_A = 0xA, 
};


int av_pool_init(int nTotalCh, int nTotalStreamNo)
{
	int nCh       = 0;
	int nStreamNo = 0;
	int nPoolNo   = 0;
	
	memset(&gPoolMan, 0, sizeof(gPoolMan));
	
	if (nTotalCh > AV_POOL_MAX_SUPPORT_CHN || nTotalStreamNo > AV_POOL_MAX_SUPPORT_STREAM)
		return -EINVAL;
		
	for (nCh = 0;nCh < nTotalCh; ++nCh)
	{
		for (nStreamNo = 0; nStreamNo < nTotalStreamNo; ++nStreamNo)
		{
			nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM+nStreamNo;
			gPoolMan[nPoolNo].pDataBuff = (char *)mem_block[nCh][nStreamNo].startVirtAddr;
			gPoolMan[nPoolNo].u32PhysAddr = mem_block[nCh][nStreamNo].startPhysAddr;
			gPoolMan[nPoolNo].u32BuffSize =  mem_block[nCh][nStreamNo].memBlockLen;
			gPoolMan[nPoolNo].u32MaxSize = 0;

			gPoolMan[nPoolNo].u32OldestFramePos = 0xFFFFFFFF;
			gPoolMan[nPoolNo].u64OldestFramePts = 0xFFFFFFFF;
			
			gPoolMan[nPoolNo].u32OldestIFramePos= 0xFFFFFFFF;
			gPoolMan[nPoolNo].u64OldestIFramePts = 0xFFFFFFFF;
			
			//init_completion(&gPoolMan[nPoolNo].dmaComplete);
			//init_completion(&gPoolMan[nPoolNo].frameComplete);
			init_waitqueue_head(&gPoolMan[nPoolNo].wait);
			init_rwsem(&gPoolMan[nPoolNo].sem);
			
			// stream 0: main video stream
			// stream 1: sub video stream
			// 最后一个通道为主机音频通道
			if (nCh != (nTotalCh - 1))
				gPoolMan[nPoolNo].u8VideoPool = 1;
			else
				gPoolMan[nPoolNo].u8VideoPool = 0;
				
			gPoolMan[nPoolNo].u8PoolNo = nPoolNo;
			gPoolMan[nPoolNo].u8Init = 1;
		}
	}
	
	printk(KERN_INFO "av pool init ok .......[%s %d]\n", __FILE__, __LINE__);
	return JPOOL_SUCCESS;
}

int av_pool_uninit(void)
{
	int nCh       = 0;
	int nStreamNo = 0;
	int nPoolNo   = 0;
	
	for (nCh = 0;nCh < AV_POOL_MAX_SUPPORT_CHN; ++nCh)
	{
		for (nStreamNo = 0; nStreamNo < AV_POOL_MAX_SUPPORT_STREAM; ++nStreamNo)
		{
			nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM+nStreamNo;
			gPoolMan[nPoolNo].u8Init = 0;
		}
	}
	printk(KERN_INFO "av pool uninit ok .......[%s %d]\n", __FILE__, __LINE__);
	return JPOOL_SUCCESS;
}
#if 0
static int av_pool_get_phy_addr(unsigned long virtp)
{
	pgd_t *pgd;
	pmd_t *pmd;
	pte_t *pte;
	struct mm_struct *mm = current->mm;

	pgd = pgd_offset(mm, virtp);
	if (!(pgd_none(*pgd) || pgd_bad(*pgd))) {
		pmd = pmd_offset(pgd, virtp);

		if (!(pmd_none(*pmd) || pmd_bad(*pmd))) {
			pte = pte_offset_kernel(pmd, virtp);

			if (pte_present(*pte)) {
				return __pa(page_address(pte_page(*pte)) +
							(virtp & ~PAGE_MASK));
			}
		}
	}

	return 0;
}
static int av_pool_dma_copy(JPOOL_MAN *poolMan, char *dstVirtAddr, char *srcVirtAddr, unsigned long copyLen, int bWritePool)
{
#if 1

    memcpy(dstVirtAddr, srcVirtAddr, copyLen);
    return copyLen;

#else
	struct paramentry_descriptor dmaParam;
	unsigned int aCnt, bCnt, cCnt;
	int transferSize = 0;
	int remainSize = copyLen;
	unsigned long srcPhysAddr;
	unsigned long dstPhysAddr;

	//memcpy(dstVirtAddr, srcVirtAddr, copyLen);
	//return copyLen;
	
	if(copyLen <= PAGE_SIZE)
	{
		memcpy(dstVirtAddr, srcVirtAddr, copyLen);
		return copyLen;
	}
	if(bWritePool)
	{
		srcPhysAddr = av_pool_get_phy_addr((unsigned long)srcVirtAddr);
		dstPhysAddr = ((unsigned long)dstVirtAddr - block_virtp) + block_start;
	}
	else
	{
		srcPhysAddr = ((unsigned long)srcVirtAddr - block_virtp) + block_start;
		dstPhysAddr = av_pool_get_phy_addr((unsigned long)dstVirtAddr);
	}
	
	aCnt = 32;
	bCnt = 512;
	 if(remainSize > aCnt*bCnt )
	{
		cCnt = remainSize /(aCnt*bCnt);

		dmaParam.src = (uint32_t) srcPhysAddr;
		dmaParam.dst = (uint32_t) dstPhysAddr;
		dmaParam.link_bcntrld = DMA_PARAM_NO_LINK | (bCnt <<16);
		dmaParam.a_b_cnt = (uint32_t)(bCnt<<16)|aCnt;
		dmaParam.ccnt = cCnt;
		dmaParam.opt = DMA_PARAM_OPT_TCINTEN | DMA_PARAM_OPT_SYNCDIM_AB
							|((poolMan->dmaChid & 0x3f)<<12)
							| DMA_PARAM_OPT_ITCCHEN;
		dmaParam.src_dst_cidx = (aCnt*bCnt) | (aCnt*bCnt << 16);
		dmaParam.src_dst_bidx = aCnt | (aCnt<<16);

		davinci_set_dma_params(poolMan->dmaChid, &dmaParam );
		davinci_start_dma(poolMan->dmaChid);

		wait_for_completion_interruptible(&poolMan->dmaComplete);
		INIT_COMPLETION(poolMan->dmaComplete);

		transferSize = aCnt*bCnt*cCnt;
		remainSize = remainSize - transferSize;
	}
	
	if(remainSize>0)
	{
		if(remainSize <= PAGE_SIZE)
		{
			memcpy(dstVirtAddr+transferSize, srcVirtAddr+transferSize, remainSize);
		}
		else
		{
			aCnt = remainSize;
			bCnt = 1;
			cCnt = 1;
			
			dmaParam.src = srcPhysAddr+transferSize;
			dmaParam.dst = dstPhysAddr+transferSize;
			dmaParam.link_bcntrld = DMA_PARAM_NO_LINK | (bCnt <<16);
			dmaParam.a_b_cnt = (uint32_t)(bCnt<<16)|aCnt;
			dmaParam.ccnt = cCnt;
			dmaParam.opt = DMA_PARAM_OPT_TCINTEN | DMA_PARAM_OPT_SYNCDIM_AB 
								|((poolMan->dmaChid&0x3f)<<12);
			dmaParam.src_dst_cidx = (aCnt*bCnt) | (aCnt*bCnt << 16);
			dmaParam.src_dst_bidx = aCnt | (aCnt<<16);

			davinci_set_dma_params(poolMan->dmaChid, &dmaParam );
			davinci_start_dma(poolMan->dmaChid);

			wait_for_completion_interruptible(&poolMan->dmaComplete);
			INIT_COMPLETION(poolMan->dmaComplete);

			transferSize = aCnt*bCnt*cCnt;
			remainSize = remainSize - transferSize;
		}
	}
	return copyLen;
#endif	
}
#endif

static inline int av_pool_copy(JPOOL_MAN *poolMan, char *dstVirtAddr, char *srcVirtAddr, unsigned long copyLen, int bWritePool)
{
	int ret;
	if(bWritePool)
		ret  = copy_from_user(dstVirtAddr, srcVirtAddr, copyLen);
	else
		ret  = copy_to_user(dstVirtAddr, srcVirtAddr, copyLen);
	return ret;
}

int av_pool_get_frame_offset(JPOOL_MAN *pJPool, int nFramePos)
{
	JPOOL_FRAME_HDR *pHdr = NULL; 
	int nOffset = 0;
	
	pHdr = (JPOOL_FRAME_HDR *)(pJPool->pDataBuff+nFramePos);	
	
	nOffset   = ((pHdr->u32DataLen + JPOOL_HDR_LEN - 1) & 0xFFFFFFC0) +
				JPOOL_HDR_LEN;	

	return nOffset;
}

int av_pool_check_is_frame(JPOOL_MAN *pJPool, int nPos)
{
	JPOOL_FRAME_HDR *pVideoHdr = NULL;

	if (pJPool->pDataBuff != NULL && nPos < pJPool->u32BuffSize)
	{
		pVideoHdr = (JPOOL_FRAME_HDR *)(pJPool->pDataBuff+nPos);

		if (pVideoHdr->u32FlagStart == JPOOL_MAGIC_FLAG &&
			pVideoHdr->u32FlagEnd == JPOOL_MAGIC_FLAG)
			return JPOOL_SUCCESS;
	}
	return JPOOL_FAILED;		
}

int av_pool_check_is_iframe(JPOOL_MAN *pJPool, int nPos)
{
	JPOOL_FRAME_HDR *pVideoHdr = NULL;

	if (pJPool->pDataBuff != NULL && nPos < pJPool->u32BuffSize)
	{
		pVideoHdr = (JPOOL_FRAME_HDR *)(pJPool->pDataBuff+nPos);
		
		if (pVideoHdr->u32FlagStart == JPOOL_MAGIC_FLAG && 
		    pVideoHdr->u32FlagEnd	== JPOOL_MAGIC_FLAG && 	
		    pVideoHdr->u8FrameType == AV_VIDEO_FRAME_I)
			return JPOOL_SUCCESS;
	}
	return JPOOL_FAILED;		
}

int av_pool_refresh_oldest_frame_pos(JPOOL_MAN *pJPool, int nWriteLen)
{
	int nOffset	 = 0;
	int nUpdate    = 0; 
	int nWritePos = pJPool->u32WritePos;
	int nOldPos    = pJPool->u32OldestFramePos;
	JPOOL_FRAME_HDR *pVideoHdr = NULL;
	
	if (nWriteLen + nWritePos < pJPool->u32BuffSize)
	{//没有环回
		//最老的帧在写的范围里面
		while (nOldPos >= nWritePos && nOldPos <= (nWriteLen + nWritePos))
		{
			nOffset  = av_pool_get_frame_offset(pJPool, nOldPos);
			nOldPos = (nOffset + nOldPos) % pJPool->u32BuffSize;
			
			if (av_pool_check_is_frame(pJPool,nOldPos) == JPOOL_FAILED)
			{
				pJPool->u32OldestFrameNo  =  0xFFFFFFFF;
				pJPool->u32OldestFramePos =  0x0;
				pJPool->u64OldestFramePts = 0xFFFFFFFF;
				return -1;
			}
			nUpdate = 1;
		}
	}
	else
	{//有环回
		while (nOldPos >= nWritePos || 
			nOldPos <= ((nWritePos + nWriteLen)% pJPool->u32BuffSize) )
		{	
			nOffset  = av_pool_get_frame_offset(pJPool, nOldPos);
			nOldPos = (nOffset + nOldPos) % pJPool->u32BuffSize;
			if (av_pool_check_is_frame(pJPool,nOldPos) == JPOOL_FAILED)
			{
				pJPool->u32OldestFrameNo  =  0xFFFFFFFF;
				pJPool->u32OldestFramePos =  0x0;
				pJPool->u64OldestFramePts = 0xFFFFFFFF;
				return -1;
			}
			nUpdate = 1;
		}
	}

	if (nUpdate == 1)
	{
		pVideoHdr = (JPOOL_FRAME_HDR *)(pJPool->pDataBuff+nOldPos);
		pJPool->u32OldestFramePos = nOldPos;
		pJPool->u64OldestFramePts  = pVideoHdr->u64Pts;
		pJPool->u32OldestFrameNo   = pVideoHdr->u32FrameNo;
	}
	return JPOOL_SUCCESS;
}

int av_pool_refresh_oldest_iframe_pos(JPOOL_MAN *pJPool, int nWriteLen)
{
	int nOffset	 = 0;
	int nOffLen	 = 0;
	int nUpdate    = 0;
	int nWritePos = pJPool->u32WritePos;
	int nOldPos    = pJPool->u32OldestIFramePos;
	JPOOL_FRAME_HDR *pHdr = NULL;
	
	if (nWriteLen + nWritePos < pJPool->u32BuffSize)
	{//没有环回
		//最老的I 帧在写的范围里面
		while (nOldPos >= nWritePos && nOldPos <= (nWriteLen + nWritePos))
		{
			nOffset  = av_pool_get_frame_offset(pJPool, nOldPos);
			nOldPos = (nOffset + nOldPos) % pJPool->u32BuffSize;

			if (av_pool_check_is_frame(pJPool, nOldPos) == JPOOL_FAILED)
			{//下一帧不是合法的帧, 则最老帧记录清除
				pJPool->u32OldestIFrameNo  =  0xFFFFFFFF;
				pJPool->u32OldestIFramePos =  0x0;
				pJPool->u64OldestIFramePts  =  0xFFFFFFFF;
				return JPOOL_SUCCESS;
			}
			nUpdate = 1;
		}
	}
	else
	{//有环回
		while (nOldPos >= nWritePos || nOldPos <= ((nWritePos + nWriteLen)% pJPool->u32BuffSize) )
		{	
			nOffset  = av_pool_get_frame_offset(pJPool, nOldPos);
			nOldPos = (nOffset + nOldPos) % pJPool->u32BuffSize;

			if (av_pool_check_is_frame(pJPool, nOldPos) == JPOOL_FAILED)
			{//下一帧不是合法的帧, 则最老帧记录清除
				pJPool->u32OldestIFrameNo  =  0xFFFFFFFF;
				pJPool->u32OldestIFramePos =  0x0;
				pJPool->u64OldestIFramePts  =  0xFFFFFFFF;
				return JPOOL_SUCCESS;
			}
			nUpdate = 1;
		}
	}

	if (nUpdate == 1)
	{
		do {
                    if (av_pool_check_is_frame(pJPool, nOldPos)  == JPOOL_FAILED)
                        break;
                    
			pHdr = (JPOOL_FRAME_HDR *)(pJPool->pDataBuff + nOldPos);
			if (pHdr->u8FrameType == AV_VIDEO_FRAME_I)
			{
				pJPool->u32OldestIFrameNo  =  pHdr->u32FrameNo;
				pJPool->u32OldestIFramePos =  nOldPos;
				pJPool->u64OldestIFramePts  = pHdr->u64Pts;
				return JPOOL_SUCCESS;
			}
			nOffset  = av_pool_get_frame_offset(pJPool, nOldPos);
			nOldPos = (nOffset + nOldPos) % pJPool->u32BuffSize;
			nOffLen += nOffset;
		}while (nOffLen < pJPool->u32BuffSize); //
		//没有更新的I 帧
		pJPool->u32OldestIFrameNo  =  0xFFFFFFFF;
		pJPool->u32OldestIFramePos =  0x0;
		pJPool->u64OldestIFramePts  =  0xFFFFFFFF;		
	}
		
	return JPOOL_SUCCESS;
}

int av_pool_write_av_data(JPOOL_MAN *pJPool, char *pHdrData, int nHdrLen, 
					int nPics, char **ppPicsData, int *pPicsLen,  int nLen, int nFrameType,
					unsigned long long u64Pts, unsigned long u32NewestFrameNo)
{
	int nNeedSize = 0, nFirstCopyLen = 0, i = 0, nOffset = 0;
	JPOOL_FRAME_HDR *pHdr = (JPOOL_FRAME_HDR *)(pJPool->pDataBuff + pJPool->u32WritePos);

	pJPool->u32NewestFrameNo = u32NewestFrameNo;
	if (nFrameType == AV_VIDEO_FRAME_I) 
	{//更新最新I 帧索引
		pJPool->u32NewestIPos = pJPool->u32WritePos;
		pJPool->u64NewestIPts = u64Pts;
		pJPool->u32NewestIFrameNo = pJPool->u32FrameNo;
	}//更新最新 帧索引
	pJPool->u32NewestFramePos = pJPool->u32WritePos;
	pJPool->u64NewestFramePts = u64Pts;
	//数据帧长度64字节对齐
 	nNeedSize = ((nLen + nHdrLen + JPOOL_HDR_LEN - 1) & 0xFFFFFFC0) +  JPOOL_HDR_LEN; 
	if (pJPool->u32OldestFramePos != 0xFFFFFFFF)
	{
		av_pool_refresh_oldest_frame_pos(pJPool, nNeedSize);
		if (pJPool->u8VideoPool == 1 && pJPool->u32OldestIFramePos != 0xFFFFFFFF)
			av_pool_refresh_oldest_iframe_pos(pJPool, nNeedSize);
	}
	//更新最老帧索引
	if (pJPool->u32OldestFramePos == 0xFFFFFFFF)
	{
		pJPool->u32OldestFramePos = pJPool->u32WritePos;
		pJPool->u64OldestFramePts = u64Pts;
		pJPool->u32OldestFrameNo  = pJPool->u32FrameNo+1;
	}
	//更新最老I 帧索引
	if (nFrameType == AV_VIDEO_FRAME_I && pJPool->u8VideoPool == 1 && pJPool->u32OldestIFramePos == 0xFFFFFFFF)
	{
		pJPool->u32OldestIFramePos = pJPool->u32WritePos;
		pJPool->u64OldestIFramePts = u64Pts;
		pJPool->u32OldestIFrameNo  = pJPool->u32FrameNo+1;		
	}
	
	pHdr->u32FlagStart 	= JPOOL_MAGIC_FLAG;
	pHdr->u32FrameNo	= ++pJPool->u32FrameNo;
	pHdr->u32DataLen		= nLen + nHdrLen;
	pHdr->u64Pts			= u64Pts;
	pHdr->u8FrameType		= nFrameType;
	pHdr->u32FlagEnd		= JPOOL_MAGIC_FLAG;
/*	
	if (pJPool->u8VideoPool == 0)
	{//copy nal info
		;
	}
	if (pJPool->u8PoolNo == 0)
		printf("write data oldest pos %d pts %llu frame no %u oldest new pos %d pts %llu frame no %u data len %d \n", 
		pJPool->u32OldestFramePos, pJPool->u64OldestFramePts, pJPool->u32OldestFrameNo,
		pJPool->u32NewestFramePos, pJPool->u64NewestFramePts, pHdr->u32FrameNo, pHdr->u32DataLen
		);
*/	
	//没有环回可以直接拷贝
	if (nNeedSize + pJPool->u32WritePos <= pJPool->u32BuffSize)
	{
		nOffset = pJPool->u32WritePos+JPOOL_HDR_LEN;		//copy frame hdr  
		if(av_pool_copy(pJPool, pJPool->pDataBuff+nOffset, pHdrData, nHdrLen, 1))
			return -EFAULT;
		nOffset += nHdrLen;
		
		for (i = 0; i < nPics && pPicsLen[i] > 0; ++i) //copy data
		{
			if(av_pool_copy(pJPool, pJPool->pDataBuff+nOffset, ppPicsData[i], pPicsLen[i], 1))
				return -EFAULT;
			nOffset += pPicsLen[i];
		}	
	}
	else	//有环回分段拷贝
	{
		nOffset = pJPool->u32WritePos+JPOOL_HDR_LEN;		//copy frame hdr  
		if (nOffset + nHdrLen > pJPool->u32BuffSize)
		{													//copy frame first
			nFirstCopyLen = pJPool->u32BuffSize - nOffset;
			if (nFirstCopyLen > 0)
				if(av_pool_copy(pJPool, pJPool->pDataBuff+nOffset, pHdrData, nFirstCopyLen, 1))
					return -EFAULT;
			if(av_pool_copy(pJPool, pJPool->pDataBuff, pHdrData+nFirstCopyLen, nHdrLen - nFirstCopyLen, 1))
				return -EFAULT;
			nOffset = nHdrLen - nFirstCopyLen;
		}
		else
		{
			if(av_pool_copy(pJPool, pJPool->pDataBuff+nOffset, pHdrData, nHdrLen, 1))
				return -EFAULT;
			nOffset += nHdrLen;
		}
		
		for (i = 0; i < nPics && pPicsLen[i] > 0; ++i)
		{
			if (nOffset + pPicsLen[i] > pJPool->u32BuffSize)
			{
				nFirstCopyLen = pJPool->u32BuffSize - nOffset;
				if (nFirstCopyLen > 0)
				{
					if(av_pool_copy(pJPool, pJPool->pDataBuff+nOffset, ppPicsData[i], nFirstCopyLen, 1))
						return -EFAULT;
				}
				if(av_pool_copy(pJPool, pJPool->pDataBuff, ppPicsData[i]+nFirstCopyLen, pPicsLen[i] - nFirstCopyLen, 1))
					return -EFAULT;
				nOffset = pPicsLen[i] - nFirstCopyLen;
			}
			else
			{
				if(av_pool_copy(pJPool, pJPool->pDataBuff+nOffset, ppPicsData[i], pPicsLen[i], 1))
					return -EFAULT;
				nOffset += pPicsLen[i];
			}
		}	
	}

	pJPool->u32WritePos = (nNeedSize + pJPool->u32WritePos) % pJPool->u32BuffSize;	
	return  JPOOL_SUCCESS;
	
}

int	 av_pool_write_frame(int nCh, int nStreamNo, char *pHdrData, int nHdrLen, 
					int nPics, char **ppPicsData, int *pPicsLen,  int nLen, int nFrameType,
					unsigned long long u64Pts, unsigned long u32CurFrameNo)
{
	int nPoolNo = 0;
	if (nCh >= AV_POOL_MAX_SUPPORT_CHN || nStreamNo >= AV_POOL_MAX_SUPPORT_STREAM ||
		pHdrData == NULL || *ppPicsData == NULL  || pPicsLen == NULL)
		return -EINVAL;

	nPoolNo =  nCh*AV_POOL_MAX_SUPPORT_STREAM+nStreamNo;
	if(gPoolMan[nPoolNo].u8Init != 1)
		return -EINVAL;
	if (nLen > gPoolMan[nPoolNo].u32BuffSize)
		return -ENOMEM;
	
	down_write(&gPoolMan[nPoolNo].sem);
	av_pool_write_av_data(&gPoolMan[nPoolNo], pHdrData, nHdrLen, nPics, ppPicsData, 
					pPicsLen, nLen, nFrameType, u64Pts, u32CurFrameNo);
	if (nLen > gPoolMan[nPoolNo].u32MaxSize)
		gPoolMan[nPoolNo].u32MaxSize = nLen;
	if(nHdrLen > gPoolMan[nPoolNo].u32MaxHdrLen)
		gPoolMan[nPoolNo].u32MaxHdrLen = nHdrLen;
	gPoolMan[nPoolNo].writeFrameCnt++;
	up_write(&gPoolMan[nPoolNo].sem);

	//complete_all(&gPoolMan[nPoolNo].frameComplete);
	wake_up_all(&gPoolMan[nPoolNo].wait);
	
	//if(u32CurFrameNo % 2000 == 0)
	//	printk(KERN_INFO "Ch %d stream %d write frame...\n", nCh, nStreamNo);
	return  JPOOL_SUCCESS;
}

int av_pool_copy_av_data(JPOOL_MAN *pJPool, char *pBuf, int nBufLen, int nCopyPos, int *pRealLen)
{
	int nFirstCopyLen= 0; 
	int nHdrLen = 0;
	int nDataLen = 0;
	JPOOL_FRAME_HDR *pHdr = (JPOOL_FRAME_HDR *)(pJPool->pDataBuff + nCopyPos);

	nDataLen = pHdr->u32DataLen;
	if (nDataLen > nBufLen)
	{
		*pRealLen = nDataLen;
		return -ENOMEM;
	}
	nHdrLen = JPOOL_HDR_LEN; 
	if (nDataLen + nHdrLen + nCopyPos <= pJPool->u32BuffSize)
	{
		if(av_pool_copy(pJPool, pBuf, pJPool->pDataBuff+nCopyPos+nHdrLen, nDataLen, 0))
			return -EFAULT;
	}
	else
	{
		nFirstCopyLen =  pJPool->u32BuffSize - nCopyPos - nHdrLen;
		if(av_pool_copy(pJPool, pBuf, pJPool->pDataBuff+nCopyPos+nHdrLen, nFirstCopyLen, 0))
			return -EFAULT;
		if(av_pool_copy(pJPool, pBuf+nFirstCopyLen, pJPool->pDataBuff, nDataLen-nFirstCopyLen, 0))
			return -EFAULT;
	}

	return nDataLen;
}

int av_pool_check_frame_in_delay(JPOOL_MAN *pJPool, int nBegPos, unsigned long u32MaxDelay)
{
	JPOOL_FRAME_HDR *pHdr= (JPOOL_FRAME_HDR*)(pJPool->pDataBuff+nBegPos);
	//pts 大于等于最新帧的pts 说明没有更新的帧了
	if (pJPool->u64NewestFramePts >= pHdr->u64Pts) 
	{
		if ((unsigned long)(pJPool->u64NewestFramePts - pHdr->u64Pts) < u32MaxDelay * 1000)
			return 0;
	}
	else//如果最新帧的pts 小于当前帧的pts 则认为最新帧的pts环回了
	{	
		if (pHdr->u64Pts + u32MaxDelay * 1000 > pJPool->u64NewestFramePts)
			return 0;
	}
	return -1;
}

int av_pool_search_data(JPOOL_MAN *pJPool, unsigned long u32FrameNo, int nBegPos, 
							unsigned long long *pU64Pts, unsigned long u32MaxDelay)
{
	JPOOL_FRAME_HDR *pHdr = (JPOOL_FRAME_HDR *)(pJPool->pDataBuff+nBegPos);
	if (nBegPos > pJPool->u32BuffSize)
		return -EINVAL;
	//正常取
	if (pHdr->u32FrameNo == u32FrameNo && 
		av_pool_check_is_frame(pJPool, nBegPos) == JPOOL_SUCCESS)
	{//所取的帧没有超出最大延迟时间
		if (av_pool_check_frame_in_delay(pJPool, nBegPos, u32MaxDelay) == JPOOL_SUCCESS)
			return nBegPos;
	} 
 
	//所要取的帧已经被覆盖掉了或者超出延迟时间
	// 则返回最新一个音频帧或者视频i 帧
	//pHdr = (JPOOL_FRAME_HDR*)(pJPool->pDataBuff+nBegPos);
	//printk(KERN_ERR "need frame no %lu %lu frame hdr %lu frame end %lu frame pst %llu cur pts %llu more than the max delay %lu, pos %d\n",
	//	u32FrameNo, pHdr->u32FrameNo,
	//	pHdr->u32FlagStart, pHdr->u32FlagEnd, pJPool->u64NewestFramePts, pHdr->u64Pts,  u32MaxDelay*1000, nBegPos);

	if (pJPool->u8VideoPool == 0)
		nBegPos = pJPool->u32NewestFramePos;
	else
		nBegPos = pJPool->u32NewestIPos;
	
	if (av_pool_check_is_frame(pJPool, nBegPos) != JPOOL_SUCCESS)
	{
		printk(KERN_INFO "video pool %d check frame err.\n", pJPool->u8VideoPool);
		
		return -EAGAIN;
	}
	
	pHdr = (JPOOL_FRAME_HDR*)(pJPool->pDataBuff+nBegPos);
	if(pJPool->u32FrameNo >= u32FrameNo && pHdr->u32FrameNo < u32FrameNo)
	{
		//帧号比当前要找的帧小说明往前找没有更新的帧了返回失败		if () 
		//printk(KERN_INFO "pool no %u cur frame no %lu need frame no %lu max frame no %lu pos %d.\n", 
		//	pJPool->u8PoolNo, pHdr->u32FrameNo, u32FrameNo, pJPool->u32FrameNo, nBegPos);	
		return -EAGAIN;
	}

	if (pJPool->u8VideoPool == 0 || av_pool_check_is_iframe(pJPool, nBegPos) == JPOOL_SUCCESS)
	{
		return nBegPos;
	}	
	//printk(KERN_INFO "video pool %d search frameerr\n", pJPool->u8VideoPool);	
	return -EAGAIN;
}

int av_pool_read_av_data(JPOOL_MAN *pJPool, char *pBuf, int nBufLen, unsigned long *pU32IFrame, 
		unsigned long u32NeedFrameNo, unsigned long *pU32FrameNo, int *pNextPos, 
		unsigned long long *pU64Pts, unsigned long u32MaxDelay, unsigned long *pU32NewstFrameNo)
{
	JPOOL_FRAME_HDR *pHdr = NULL;
	int nDataLen = 0, nReadPos = -1;
	
	//请求的帧大于缓存里面的帧,且没有环回
	if (((u32NeedFrameNo != 0xFFFFFFFF) && u32NeedFrameNo > pJPool->u32FrameNo &&  
		u32NeedFrameNo - pJPool->u32FrameNo < 30) || 
		pJPool->u32MaxSize == 0)
		return -EAGAIN;
	
	if (u32NeedFrameNo == 0 && *pNextPos == 0) //拷贝最新的帧返回
	{//如果强制取第一帧为I帧则判断第一帧是不是I帧如果不是返回帧不存在
		//if (pJPool->u8VideoPool != 0 && *pU32IFrame != 0 && 
		//	av_pool_check_is_iframe(pJPool, pJPool->u32NewestFramePos) != JPOOL_SUCCESS)
		//	return -EAGAIN;
		if (pJPool->u8VideoPool != 0 && *pU32IFrame != 0 && pJPool->u32FrameNo - pJPool->u32NewestIFrameNo > 5)
			return -EAGAIN;
		
		nReadPos = pJPool->u32NewestIPos;
	}
	else if (u32NeedFrameNo == 0xFFFFFFFF && *pNextPos == 0xFFFFFFFF)
	{
		if (pJPool->u8VideoPool != 0) //视频返回最老I 帧 
			nReadPos = pJPool->u32OldestIFramePos;
		else						//音频防护最老的帧
			nReadPos = pJPool->u32OldestFramePos;
	}
	else if (*pU32IFrame  && pJPool->u8VideoPool != 0)
	{		
		if (av_pool_check_is_iframe(pJPool, pJPool->u32NewestIPos) == JPOOL_SUCCESS)
		{
			pHdr = (JPOOL_FRAME_HDR*)(pJPool->pDataBuff+pJPool->u32NewestIPos);
			if (u32NeedFrameNo > pHdr->u32FrameNo)
				return -EAGAIN;
			nReadPos = pJPool->u32NewestIPos;
		}	
	}
	else if ((nReadPos = av_pool_search_data(pJPool, u32NeedFrameNo, *pNextPos, pU64Pts, u32MaxDelay)) < 0)
	{
		return -EAGAIN;
	}
	
	if (nReadPos >= 0)
	{
		if (av_pool_check_is_frame(pJPool, nReadPos) == JPOOL_FAILED)
			return -EAGAIN;
		if ((nDataLen = av_pool_copy_av_data(pJPool, pBuf, nBufLen, nReadPos, (int *)pU32IFrame)) < 0)
			return nDataLen;

		pHdr = (JPOOL_FRAME_HDR*)(pJPool->pDataBuff+nReadPos);
		*pU32FrameNo 	= pHdr->u32FrameNo;
		*pNextPos         	= (nReadPos +  av_pool_get_frame_offset(pJPool, nReadPos)) % pJPool->u32BuffSize; 
		*pU64Pts		= pHdr->u64Pts;		
		*pU32IFrame	= pHdr->u8FrameType;
		*pU32NewstFrameNo = pJPool->u32NewestFrameNo;
		
		return nDataLen;
	}	
	
	return -EAGAIN;	
}

int av_pool_read_frame(int nCh, int nStreamNo, char *pBuf, int nBufLen, unsigned long *pU32IFrame,
			unsigned long u32NeedFrameNo, unsigned long *pU32FrameNo, unsigned long *pLastPos, unsigned long long *pU64Pts, 
			unsigned long	u32MaxDelay, unsigned long *pU32NewestFrameNo, int timeout)
{
	int nPoolNo = 0;
	int nFrameSize = 0;
	int nTime;
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStreamNo > AV_POOL_MAX_SUPPORT_STREAM ||
		pBuf == NULL ||pU32IFrame == NULL || pU32FrameNo == NULL || pLastPos == NULL 
		|| pU64Pts == NULL || nBufLen < 0 || pU32NewestFrameNo == NULL)
		return -EINVAL;
		
	//printk(KERN_ERR "chn: %d, stream: %d, needIFrame: %lu, needFrameNo: %lu\n",
	//		nCh, nStreamNo, *pU32IFrame, u32NeedFrameNo);
	nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM+nStreamNo;
	if(gPoolMan[nPoolNo].u8Init != 1)
		return -EINVAL;

	nTime = (timeout == 0) ? 0 : ((timeout + 9) / 10);
	do
	{
		down_read(&gPoolMan[nPoolNo].sem);
		nFrameSize = av_pool_read_av_data(&gPoolMan[nPoolNo], pBuf, nBufLen, pU32IFrame, 
				u32NeedFrameNo, pU32FrameNo, (int *)pLastPos, pU64Pts, u32MaxDelay, pU32NewestFrameNo);
		if(nFrameSize > 0)
			gPoolMan[nPoolNo].readFrameCnt++;
		up_read(&gPoolMan[nPoolNo].sem);
		if(nFrameSize == -EAGAIN)
		{
			#if 1
			if(timeout > 0)
			{
				int nRet    = 0;
				//nRet = wait_for_completion_interruptible_timeout(&gPoolMan[nPoolNo].frameComplete, msecs_to_jiffies(timeout));
				//INIT_COMPLETION(gPoolMan[nPoolNo].frameComplete);
				nRet = interruptible_sleep_on_timeout(&gPoolMan[nPoolNo].wait, msecs_to_jiffies(timeout));
				if(nRet > 0)
					continue;
				else if(nRet == 0)
				{
					nFrameSize = -ETIMEDOUT;
					break;
				}
				else
				{
					nFrameSize = -EFAULT;
					break;
				}
			}
			else
			{
				msleep(1);
				break;
			}
			#else
			if(nTime > 0)
			{
				nTime--;
				msleep(10);
				continue;
			}
			else
				break;
			#endif
		}
		else
			break;
	}while(1);
	return  nFrameSize;
}

int av_pool_get_max_frame_size(int nCh, int nStreamNo)
{
	unsigned long u32MaxFrameSize = 0;
	int nPoolNo;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStreamNo > AV_POOL_MAX_SUPPORT_STREAM)
		return -EINVAL;

	nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM+nStreamNo;

	if (gPoolMan[nPoolNo].u8Init != 1)
		return -EINVAL;
	
	down_read(&gPoolMan[nPoolNo].sem);
	u32MaxFrameSize = gPoolMan[nPoolNo].u32MaxSize + gPoolMan[nPoolNo].u32MaxHdrLen;
	up_read(&gPoolMan[nPoolNo].sem);
	
	return u32MaxFrameSize;
}

int av_pool_reset_pool(int nCh, int nStreamNo)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM + nStreamNo;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStreamNo > AV_POOL_MAX_SUPPORT_STREAM)
		return -EINVAL;
	
	down_write(&gPoolMan[nPoolNo].sem);
	
	gPoolMan[nPoolNo].u32FrameNo = 0;
	//gPoolMan[nPoolNo].u32BuffSize = 0;
	gPoolMan[nPoolNo].u32MaxSize = 0;
	gPoolMan[nPoolNo].u32MaxHdrLen = 0;
	gPoolMan[nPoolNo].u32OldestFrameNo = 0;
	gPoolMan[nPoolNo].u32OldestIFrameNo = 0;
	gPoolMan[nPoolNo].u32NewestIPos = 0;
	gPoolMan[nPoolNo].u32NewestIFrameNo = 0;
	gPoolMan[nPoolNo].u32NewestFramePos = 0;
	gPoolMan[nPoolNo].u64NewestIPts = 0;
	gPoolMan[nPoolNo].u64NewestFramePts = 0;
	gPoolMan[nPoolNo].u32WritePos = 0;
	gPoolMan[nPoolNo].u32NewestFrameNo = 0;
	
	gPoolMan[nPoolNo].u32OldestFramePos = 0xFFFFFFFF;
	gPoolMan[nPoolNo].u64OldestFramePts = 0xFFFFFFFF;
	
	gPoolMan[nPoolNo].u32OldestIFramePos= 0xFFFFFFFF;
	gPoolMan[nPoolNo].u64OldestIFramePts = 0xFFFFFFFF;

	// reset pool information
	//memset(gPoolMan[nPoolNo].ipAddr, 0, sizeof(gPoolMan[nPoolNo].ipAddr));
	//memset(gPoolMan[nPoolNo].protocol, 0, sizeof(gPoolMan[nPoolNo].protocol));
	//gPoolMan[nPoolNo].type = 0;
	//gPoolMan[nPoolNo].port = 0;

	//gPoolMan[nPoolNo].ipncHandle = 0;
	//gPoolMan[nPoolNo].streamHandle = 0;

	// don't need to reset read count and write count
	//gPoolMan[nPoolNo].readCnt = 0;
	//gPoolMan[nPoolNo].writeCnt = 0;

	gPoolMan[nPoolNo].readFrameCnt = 0;
	gPoolMan[nPoolNo].writeFrameCnt = 0;

	memset(gPoolMan[nPoolNo].pDataBuff, 0, gPoolMan[nPoolNo].u32BuffSize);
	up_write(&gPoolMan[nPoolNo].sem);
	return 0;
}

int av_pool_set_pool_info(int nCh, 
			const char *ipAddr, const char *protocol, 
			int type, int port)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM;

	if (nCh > AV_POOL_MAX_SUPPORT_CHN)
		return -EINVAL;

	// 只设置第一个,其它的通道中的流就不设置了
	down_write(&gPoolMan[nPoolNo].sem);
	strncpy(gPoolMan[nPoolNo].ipAddr, ipAddr, sizeof(gPoolMan[nPoolNo].ipAddr)-1);
	gPoolMan[nPoolNo].ipAddr[sizeof(gPoolMan[nPoolNo].ipAddr) - 1] = '\0';
	strncpy(gPoolMan[nPoolNo].protocol, protocol, sizeof(gPoolMan[nPoolNo].protocol)-1);
	gPoolMan[nPoolNo].protocol[sizeof(gPoolMan[nPoolNo].protocol) - 1] = '\0';
	gPoolMan[nPoolNo].type = type;
	gPoolMan[nPoolNo].port = port;
	up_write(&gPoolMan[nPoolNo].sem);
	return 0;
}

int av_pool_get_pool_info(int nCh, 
			char *ipAddr, char *protocol, 
			int *type, int *port)
			
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN)
		return -EINVAL;
		
	down_read(&gPoolMan[nPoolNo].sem);
	strcpy(ipAddr, gPoolMan[nPoolNo].ipAddr);
	strcpy(protocol, gPoolMan[nPoolNo].protocol);
	*type = gPoolMan[nPoolNo].type;
	*port = gPoolMan[nPoolNo].port;
	up_read(&gPoolMan[nPoolNo].sem);
	return 0;
}

int av_pool_get_frame_info(int nCh, int nStreamNo, 
			unsigned int *writeFrameCnt, unsigned int *readFrameCnt)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM + nStreamNo;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN)
		return -EINVAL;
		
	down_read(&gPoolMan[nPoolNo].sem);
	*writeFrameCnt = gPoolMan[nPoolNo].writeFrameCnt;
	*readFrameCnt = gPoolMan[nPoolNo].readFrameCnt;
	up_read(&gPoolMan[nPoolNo].sem);
	return 0;
}

int av_pool_set_pool_status(int nCh, int nStreamNo, AvPoolWorkStatus status)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM + nStreamNo;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStreamNo > AV_POOL_MAX_SUPPORT_STREAM)
		return -EINVAL;

	down_write(&gPoolMan[nPoolNo].sem);
	gPoolMan[nPoolNo].status = status;
	up_write(&gPoolMan[nPoolNo].sem);
	
	return 0;
}

int av_pool_get_pool_status(int nCh, int nStreamNo, AvPoolWorkStatus *status)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM + nStreamNo;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStreamNo > AV_POOL_MAX_SUPPORT_STREAM)
		return -EINVAL;

	down_read(&gPoolMan[nPoolNo].sem);
	*status = gPoolMan[nPoolNo].status;
	up_read(&gPoolMan[nPoolNo].sem);
	
	return 0;
}

int av_pool_inc_pool_reading_cnt(int nCh, int nStreamNo)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM + nStreamNo;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStreamNo > AV_POOL_MAX_SUPPORT_STREAM)
		return -EINVAL;
	atomic_inc(&gPoolMan[nPoolNo].readCnt);
	return 0;
}

int av_pool_dec_pool_reading_cnt(int nCh, int nStreamNo)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM + nStreamNo;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStreamNo > AV_POOL_MAX_SUPPORT_STREAM)
		return -EINVAL;
	atomic_dec(&gPoolMan[nPoolNo].readCnt);
	return 0;
}

int av_pool_get_pool_reading_cnt(int nCh, int nStreamNo)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM + nStreamNo;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStreamNo > AV_POOL_MAX_SUPPORT_STREAM)
		return -EINVAL;
	return atomic_read(&gPoolMan[nPoolNo].readCnt);
}

int av_pool_inc_pool_writing_cnt(int nCh, int nStreamNo)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM + nStreamNo;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStreamNo > AV_POOL_MAX_SUPPORT_STREAM)
		return -EINVAL;
	if(atomic_read(&gPoolMan[nPoolNo].writeCnt) > 0)
		return -EBUSY;
	atomic_inc(&gPoolMan[nPoolNo].writeCnt);
	return 0;
}

int av_pool_dec_pool_writing_cnt(int nCh, int nStreamNo)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM + nStreamNo;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStreamNo > AV_POOL_MAX_SUPPORT_STREAM)
		return -EINVAL;
	atomic_dec(&gPoolMan[nPoolNo].writeCnt);
	return 0;
}

int av_pool_get_pool_writing_cnt(int nCh, int nStreamNo)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM + nStreamNo;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStreamNo > AV_POOL_MAX_SUPPORT_STREAM)
		return -EINVAL;
	return atomic_read(&gPoolMan[nPoolNo].writeCnt);
}

int av_pool_set_pool_ipnc_handle(int nCh, long ipncHandle)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN)
		return -EINVAL;
	down_write(&gPoolMan[nPoolNo].sem);
	gPoolMan[nPoolNo].ipncHandle = ipncHandle;
	up_write(&gPoolMan[nPoolNo].sem);
	return 0;
}

int av_pool_get_pool_ipnc_handle(int nCh, long *ipncHandle)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN)
		return -EINVAL;
	down_read(&gPoolMan[nPoolNo].sem);
	*ipncHandle = gPoolMan[nPoolNo].ipncHandle;
	up_read(&gPoolMan[nPoolNo].sem);
	return 0;
}

int av_pool_set_pool_stream_handle(int nCh, int nStreamNo, long streamHandle)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM + nStreamNo;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStreamNo > AV_POOL_MAX_SUPPORT_STREAM)
		return -EINVAL;
	down_write(&gPoolMan[nPoolNo].sem);
	gPoolMan[nPoolNo].streamHandle = streamHandle;
	up_write(&gPoolMan[nPoolNo].sem);
	return 0;
}

int av_pool_get_pool_stream_handle(int nCh, int nStreamNo, long *streamHandle)
{
	int nPoolNo = nCh*AV_POOL_MAX_SUPPORT_STREAM + nStreamNo;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStreamNo > AV_POOL_MAX_SUPPORT_STREAM)
		return -EINVAL;
	down_read(&gPoolMan[nPoolNo].sem);
	*streamHandle = gPoolMan[nPoolNo].streamHandle;
	up_read(&gPoolMan[nPoolNo].sem);
	return 0;
}

