//////////////////////////////////////////////////////////////////////
////////////////     hi_talk_jxj.h            ////////////////////////
//////////////////////////////////////////////////////////////////////

#ifndef _JXJ_TALK_H_
#define _JXJ_TALK_H_



#define J_TALK_MAGIC   0x33222233
#define J_TALK_PORT    3322


/*
 * 消息功能：对讲数据
 * 消息方向：DEV->CU
 * 		    CU->DEV 
 */
typedef struct _J_TALK_FRMAE_HDR_
{
    unsigned long		u32Magic;     	//0x33222233  
    unsigned long		u32FrameNo;   	//帧号            
    unsigned long		u32Pts;			//时间戳
	unsigned long		u32Len;			//Data 长度
	unsigned char		u8Data[0];		//数据内容
}J_TALK_FRMAE_HDR, *LPJ_TALK_FRMAE_HDR;


typedef struct _J_TALK_AUDIO_ATTR_
{    
	unsigned char		u8AudioSamples;   	//采样率		0--8k 1--16k 2-32k
	unsigned char		u8EncodeType;	   	//音频编码格式 0--pcm 1-g711a 2-g711u 3--g726
	unsigned char		u8AudioChannels;  	//通道数		暂只支持1	
	unsigned char		u8AudioBits;	  	//位数			16bit
}J_TALK_AUDIO_ATTR, *LPJ_TALK_AUDIO_ATTR;


/*
 * 消息功能：请求对讲  
 * 消息方向：CU->DEV 
 */
typedef struct _J_TALK_REQ_
{
    unsigned long    u32Magic;         	//0x33222233 
    char            szUsr[64];		    //admin
    char            szPsw[64];		    //admin
    J_TALK_AUDIO_ATTR  stAttr;
	long               s32Res;       	//响应 0: 成功 ~0: 失败
}J_TALK_REQ, *LPJ_TALK_REQ;



int nvr_jxj_talk_init();
int nvr_jxj_talk_uninit();



#endif
