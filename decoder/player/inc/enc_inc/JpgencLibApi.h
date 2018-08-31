/*
********************************************************************************
*                                    eMOD
*                   the Easy Portable/Player Develop Kits
*                               mod_herb sub-system
*                            mpeg4 video encode module
*
*          (c) Copyright 2009-2010, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : Mp4encLibApi.h
* Version: V1.0
* By     : Eric_wang
* Date   : 2009-12-25
* Description: 这是mp4encodeLib提供的给外部模块引用的头文件，可以视为API文件
********************************************************************************
*/
#ifndef _JPGENCLIBAPI_H_
#define _JPGENCLIBAPI_H_

#include "encoder_type.h"

enum VENC_VIDEOINFO_TYPE
{
    VENC_SET_CSI_INFO_CMD = 1,
	VENC_SET_ENC_INFO_CMD,
	//VENC_SET_QUALITY_CMD,	//set picture quality, used by JPEG only, in this case uPara is the quality percent, for example, target quality is 80%, uPara is 80.
	//VENC_SET_FRAMERATE_CMD,	//set picture frame rate, uPara is the actual frame rate multiplied by 1000.
	//VENC_SET_BITRATE_CMD,	//set picture target bit rate,uPara is the target bit rate.
	//VENC_SET_KEYITL_CMD,	//set keyframe interval, after force keyframe flag set, it recounter again. default value is 25,and 0 is forbiden , 两个I帧之间的P帧数量。
	//VENC_SET_COLORINFO_CMD,	//set color format and color space,
							//lowest 16LSBs are color space 0--yuv 4:2:0, 1:3--reserved, 4--bayer(BG/GR) , 5--bayer(RG/GB) , 6--bayer(GB/RG) , 7--bayer(GR/BG)
							//highest MSBs are color space 0--BT601, 1--BT709 but for jpeg always YCC.
	//VENC_SET_QPRANGE_CMD,	//set quant parameter range (lowest 16LSBs are min qp, highest MSBs are max qp)
	//VENC_SET_PICSIZE_CMD,	//set picture size, uPara is the actual size (lowest 16LSBs are width, highest MSBs are height)
	VENC_REGCB_CMD,			//register one call back function, the input parameter is the pointer to the parameter structure VEnc_IOCtrl_CB described below.
	VENC_INIT_CMD,
	VENC_SET_JPG_INFO_CMD,
	VENC_CROP_IMAGE_CMD,
};

enum VEncCBType {
	VENC_CB_GET_FRMBUF, //register the GetFrmBufCB() function
	VENC_CB_WAIT_FINISH //register the WaitFinishCB() function
};

typedef __s32 (*VEncCBFunc)(__s32 uParam1,void *vParam2);

struct VEnc_IOCtrl_CB
{
	enum VEncCBType eCBType; //call back function type, one of element of ENUm VEncCBType defined below.
	VEncCBFunc pCB; //the call back function pointer defined as: typedef __s32 (*VEncCBFunc)(__s32 uParam1,void *vParam2);
};

typedef enum __PIC_ENC_RESULT
{
	PIC_ENC_ERR_NO_CODED_FRAME = -4,
	PIC_ENC_ERR_NO_MEM_SPACE = -3,
	PIC_ENC_ERR_VE_OVERTIME = -2, // video engine is busy, no picture is encoded
	PIC_ENC_ERR_VBS_UNOVERDERFLOW = -1, // video bitstream overderflow,即没有vbv buffer装填coded frame.
	PIC_ENC_ERR_NONE = 0, // encode one picture successed
	PIC_ENC_ERR_
} __pic_enc_result;

typedef struct VEnc_FrmBuf_Info
{
    __u8 *addrY;		//the luma component address for yuv format, or the address for bayer pattern format
	__u8 *addrCb;		//the Cb component address for yuv format only.
	__u8 *addrCr;		//the Cr component address for yuv format only
	SIZE size;          //the size of the picture
    __u32 color_fmt;
    __u32 color_space;
	__s32 ForceKeyFrame;   //0----current frame may be encoded as non-key frame, 1----current frame must be encoded as key frame
	__s64 pts;        //unit:ms
    __s32 pts_valid;
	__u8 *bayer_y;
    __u8 *bayer_cb;
    __u8 *bayer_cr;
    __u32 *Block_Header;   //block header for olayer
    __u8 *Block_Data;     //block data for olayer
    __u32 *Palette_Data;   //palette data for olayer
    __u8 scale_mode;  //the THUMB scale_down coefficient
    void *pover_overlay;
}VEnc_FrmBuf_Info;

typedef struct VBV_DATA_CTRL_INFO
{
    __s32       idx;    //该元素在数组中的下标号,用于归还.
    __u8        *pData0;
    __s32       uSize0;
    __u8        *pData1;
    __s32       uSize1;
    __s64       pts;
    int   		pts_valid;
	__u8		*privateData;
	__s32		privateDataLen;
} __vbv_data_ctrl_info_t;

// typedef struct vdec_vbv_info
// {
//     __u8        *vbv_buf;
//     __u8        *vbv_buf_end;
//     __u8        *read_addr;
//     __u8        *write_addr;
//     __bool      hasPicBorder;
//     __bool      hasSliceStructure;
//     __vbv_data_ctrl_info_t vbv_data_info[MAX_PIC_NUM_IN_VBV];
//     __u16       PicReadIdx;
//     __u16       PicWriteIdx;
//     __u16       PicIdxSize;
//     __u32       valid_size;
//
// } __vdec_vbv_info_t;

typedef struct VENC_DEVICE
{
	void *priv_data;    //Mp4EncCtx
	void *pIsp;
	//struct vdec_vbv_info        vbvInfo;//存放编码完成的帧的数据结构
	__s16 (*open)(struct VENC_DEVICE *p);
	__s16 (*close)(struct VENC_DEVICE *p);
	__s16 (*encode)(struct VENC_DEVICE *p, void *pBuffer);
	__s16 (*IoCtrl)(struct VENC_DEVICE *p, __u32, __u32);
    __s16 (*GetBitStreamInfo)(struct VENC_DEVICE *pDev, __vbv_data_ctrl_info_t *pdatainfo);
    __s16 (*ReleaseBitStreamInfo)(struct VENC_DEVICE *pDev, __s32 node_id);
	__s32 (*GetFrmBufCB)(__s32 uParam1,  void *pFrmBufInfo);
	__s32 (*WaitFinishCB)(__s32 uParam1, void *pMsg);

    //struct VEnc_FrmBuf_Info FrmBufInfo;
}VENC_DEVICE;

extern struct VENC_DEVICE *JPGVENC_init(int *ret);
extern __s16 JPGVENC_exit(struct VENC_DEVICE *pDev);

//由上层代码提供
extern __s32 SysSendVEReadyFlag(void);


#endif  /* _JPGENCLIBAPI_ */
