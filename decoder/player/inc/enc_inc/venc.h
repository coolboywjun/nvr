#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef _VENC_H_
#define _VENC_H_

typedef struct RECT_t
{
	int left;
	int top;
	int width;
	int height;
}RECT_t;

typedef struct CROP_INFO
{
	int is_enable;
	RECT_t crop_rect;
}CROP_INFO;

/** VP8 profiles */
typedef enum VENC_VP8_PROFILE_TYPE {
    VENC_VP8ProfileMain = 0x01,
    VENC_VP8ProfileUnknown
} VENC_VP8_PROFILE_TYPE;

/** VP8 levels */
typedef enum VENC_VP8_LEVEL_TYPE {
    VENC_VP8_Level_Version0 = 0x01,
    VENC_VP8_Level_Version1 = 0x02,
    VENC_VP8_Level_Version2 = 0x04,
    VENC_VP8_Level_Version3 = 0x08,
    VENC_VP8_LevelUnknown
} VENC_VP8_LEVEL_TYPE;

typedef enum VENCODER_COMMAND_TYPE{
    VENC_CMD_NOP           =  0, // interface test message
    VENC_CMD_BASE_CONFIG,  // reference: VencBaseConfig
    VENC_CMD_SET_FROFILE_LEVEL,  // reference: VencBaseConfig
    VENC_CMD_SET_QP_RANGE, // reference: VencQPrange
    VENC_CMD_SET_INTRA_REFRESH, // reference: VencIntraRefresh
    VENC_CMD_SET_BITRATE, // reference: VencTargetBitrate
    VENC_CMD_OPEN, // reference: NULL
    VENC_CMD_CLOSE, // reference: NULL
    VENC_CMD_ENCODE, // reference: NULL

    VENC_CMD_ALLOCATE_INPUT_BUFFER, // reference: VencAllocateBufferParam
    VENC_CMD_GET_ALLOCATE_INPUT_BUFFER , // reference: VencInputBuffer
    VENC_CMD_FLUSHCACHE_ALLOCATE_INPUT_BUFFER, // reference: VencInputBuffer
    VENC_CMD_RETURN_ALLOCATE_INPUT_BUFFER, // reference: VencInputBuffer

	VENC_CMD_ENQUENE_INPUT_BUFFER, // reference: VencInputBuffer
	VENC_CMD_DEQUENE_INPUT_BUFFER, // reference: VencInputBuffer

	VENC_CMD_SET_MOTION_PAR_FLAG,
	VENC_CMD_GET_MOTION_FLAG,

	VENC_CMD_GET_BITSTREAM, // reference: VencOutputBuffer
	VENC_CMD_RETURN_BITSTREAM, // reference: VencOutputBuffer
	VENC_CMD_HEADER_DATA, // reference: VencSeqHeader
	
}VENC_IO_COMMAND;

typedef enum VENC_CODEC_TYPE{
	VENC_CODEC_H264,
	VENC_CODEC_VP8,
	VENC_CODEC_JPEG,
}VENC_CODEC_TYPE;

typedef enum VENC_PIXEL_FMT{
    VENC_PIXEL_YUV420,
    VENC_PIXEL_YVU420,
    VENC_PIXEL_ARGB,
    VENC_PIXEL_RGBA,
    VENC_PIXEL_ABGR,
    VENC_PIXEL_BGRA,
    VENC_PIXEL_TILE_32X32,
}VENC_PIXEL_FMT;

typedef struct VencBaseConfig{
	unsigned int	input_width;
	unsigned int	input_height;
	unsigned int	dst_width;
	unsigned int	dst_height;
	unsigned int	framerate;
	unsigned int	targetbitrate;
	int  			inputformat;
	int         	codectype;
	int				maxKeyInterval;
}VencBaseConfig;

typedef struct VencProfileLevel{
	unsigned int	profile;
	unsigned int	level;
}VencProfileLevel;

typedef struct VencQPrange{
	unsigned int	maxqp;
	unsigned int	minqp;
}VencQPrange;

typedef struct VencSeqHeader{
	unsigned char  *bufptr;
	unsigned int	length;
	unsigned int	bufsize;
}VencSeqHeader;

typedef struct VencIntraRefresh{
	unsigned int	irmode;
	unsigned int	mbcount;
}VencIntraRefresh;

typedef struct VencTargetBitrate{
	unsigned int	target_bitrate;
}VencTargetBitrate;

typedef struct VencInputBuffer{
	int			   id;
	unsigned char *addrvirY; //the luma  vir address for yuv format 
	unsigned char *addrvirC; //the color vir address for yuv format
	unsigned char *addrphyY; //the luma  phy address for yuv format 
	unsigned char *addrphyC; //the color phy address for yuv format
	
   	int            bufsize;
	int   		   color_fmt;
	long long  	   pts;      //unit:us
	CROP_INFO      crop_info;
	void		  *addrOverlay;
}VencInputBuffer;

typedef struct VencOutputBuffer{
	unsigned char *ptr0;
	unsigned int  size0;
	
	unsigned char *ptr1;
	unsigned int  size1;
	long long  	   pts;      //unit:us
	int			   id;
}VencOutputBuffer;

typedef struct VencAllocateBufferParam{
    unsigned int              	buffernum;
	unsigned int              	buffersize;  // witdh*heigh*3/2 for yuv420
}VencAllocateBufferParam;

typedef struct CEDARV_ENCODER
{
	void *context;
	int  (*ioctrl)(void *encoder, VENC_IO_COMMAND cmd, void* param);
}cedarv_encoder_t;

cedarv_encoder_t *cedarvEncInit();
void cedarvEncExit(cedarv_encoder_t *encoder);

#endif	//_VENC_H_

#ifdef __cplusplus
}
#endif /* __cplusplus */
