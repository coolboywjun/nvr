/*
********************************************************************************
*                                    eMOD
*                   the Easy Portable/Player Develop Kits
*                               mod_herb sub-system
*                            h264 video encode module
*
*          (c) Copyright 2009-2010, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : H264encLibApi.h
* Version: V1.0
* By     : Eric_wang
* Date   : 2009-12-25
* Description:
********************************************************************************
*/
#ifndef _H264ENCLIBAPI_H___
#define _H264ENCLIBAPI_H___

#ifdef __cplusplus
extern "C" {
#endif

#include "encoder_type.h"

typedef struct Data_Container
{
	char *data;
	int length;
}Data_Container;

typedef struct motion_par
{
	int motion_detect_enable;
	int motion_detect_ratio;   //for 0 to 12,the 0 is the best sensitive
}motion_par;

enum VENC_VIDEOINFO_TYPE
{
    VENC_SET_CSI_INFO_CMD = 1,
	VENC_SET_ENC_INFO_CMD,
	VENC_REGCB_CMD,
	VENC_INIT_CMD,
	VENC_GET_MOTION_FLAG,
	VENC_CROP_IMAGE_CMD,
	
	// star add 
	VENC_REQUEST_KEY_FRAME_CMD,
	VENC_SET_BITRATE_CMD,
	VENC_SET_FRAMERATE_CMD,
	VENC_SET_ENCODE_MODE_CMD,
	VENC_SET_MAX_KEY_FRAME_INTERVAL,

	VENC_GET_SPS_PPS_DATA,
	VENC_QUERY_BSINFO,

	VENC_SET_RC_MODE,
	VENC_SET_MOTION_PAR_FLAG,
	VENC_SET_OUTSTREAM_BUF_LENGTH
};


typedef enum __PIC_ENC_RESULT
{
	PIC_ENC_ERR_NO_CODED_FRAME = -4,
	PIC_ENC_ERR_NO_MEM_SPACE = -3,
	PIC_ENC_ERR_VE_OVERTIME = -2, // video engine is busy, no picture is encoded
	PIC_ENC_ERR_VBS_UNOVERDERFLOW = -1, // video bitstream overderflow,
	PIC_ENC_ERR_NONE = 0, // encode one picture successed
	PIC_ENC_ERR_
} __pic_enc_result;

typedef struct VEnc_FrmBuf_Info
{
	unsigned char *addrY;		//the luma component address for yuv format, or the address for bayer pattern format
	unsigned char *addrCb;		//the Cb component address for yuv format only.
	unsigned char *addrCr;		//the Cr component address for yuv format only
    unsigned int color_fmt;
    unsigned int color_space;
	long long pts;        //unit:ms
    int 	  pts_valid;
	unsigned char *bayer_y;
    unsigned char *bayer_cb;
    unsigned char *bayer_cr;
    unsigned int *Block_Header;   //block header for olayer
    unsigned char *Block_Data;     //block data for olayer
    unsigned int *Palette_Data;   //palette data for olayer
    unsigned char scale_mode;  //the THUMB scale_down coefficient 
    void *pover_overlay;
}VEnc_FrmBuf_Info;

typedef struct VBV_DATA_CTRL_INFO
{
    int       			idx;
    unsigned char 		*pData0;
    int       			uSize0;
    unsigned char 		*pData1;
    int       			uSize1;
    long long       	pts;
    int       			pts_valid;
	unsigned char		*privateData;
	int					privateDataLen;
	unsigned char		keyFrameFlag;
} __vbv_data_ctrl_info_t;

typedef struct CDX_RECORDER_BUFFERSTATE{
  int nValidSizePercent;
  int nValidSize;
  int nElementCounter;
}CDX_RECORDER_BUFFERSTATE;

typedef struct VENC_DEVICE
{
	void *priv_data;
	void *pIsp;
	int (*open)(struct VENC_DEVICE *p);
	int (*close)(struct VENC_DEVICE *p);
	int (*RequestBuffer)(struct VENC_DEVICE *pDev, unsigned char **pBuffery,unsigned char **pBufferC, unsigned int *phy_y, unsigned int *phy_c);
	int (*UpdataBuffer)(struct VENC_DEVICE *pDev);
	int (*hasOutputStream)(struct VENC_DEVICE *pDev);
	int (*encode)(struct VENC_DEVICE *pDev, void *pBuffer);
	int (*IoCtrl)(struct VENC_DEVICE *p, unsigned int, unsigned int);
    int (*GetBitStreamInfo)(struct VENC_DEVICE *pDev, __vbv_data_ctrl_info_t *pdatainfo);
    int (*ReleaseBitStreamInfo)(struct VENC_DEVICE *pDev, int node_id);
	int (*GetFrmBufCB)(int uParam1,  void *pFrmBufInfo);
	int (*WaitFinishCB)(int uParam1, void *pMsg);
}VENC_DEVICE;

extern struct VENC_DEVICE *H264EncInit(int *ret);
extern int H264EncExit(struct VENC_DEVICE *pDev);


#ifdef __cplusplus
}
#endif

#endif  /* _H264ENCLIBAPI_H_ */

