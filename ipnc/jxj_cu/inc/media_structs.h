#ifndef __MEDIA_STRUCTS_H__
#define __MEDIA_STRUCTS_H__

#include <stdint.h>

/*采样率*/
enum {
     SAMPLE_8K       /*8K Sample rate*/
    ,SAMPLE_90K
};


/*编码类型*/
enum {
   ENC_H264          /*H264*/
  ,ENC_MPEG4         /*MPEG4*/
  ,ENC_G711A         /*G711A*/
  ,ENC_G711U		 /*G711U*/
};


/*帧类型*/
enum {
    FRAME_I  = 0x01 /*视频I*/
   ,FRAME_P  = 0x02 /*视频P*/
   ,FRAME_B  = 0x03 /*视频B*/
   ,FRAME_A  = 0x04 /*音频帧*/
   ,FRAME_EOF= 0x0F /*流结束帧*/
};


/*媒体类型*/
enum {
     MEDIA_VIDEO = 0x01
    ,MEDIA_AUDIO = 0x02
};


/*视频扩展信息*/
typedef struct frame_v_ext_s {
    uint16_t   width;           /* 视频宽度*/
    uint16_t   height;          /* 视频高度*/
    uint32_t   utc_time;
}frame_v_ext_t;


/*音频扩展信息*/
typedef struct frame_a_ext_s {
    uint8_t samples_per_sec;     /*采样率*/         
    uint8_t bits_per_sample;     /*位宽*/
    uint8_t channel_num;         /*音频通道数目, 如 1，2 */
    uint8_t reserved;            /*保留*/
}frame_a_ext_t;


/*视频帧NAL描述*/
typedef struct nal_desc_s {
    uint32_t nal_num;           /*NAL个数*/
    struct nal_s {
        uint32_t nal_off;       /*NAL地址*/
        uint32_t nal_size;      /*NAL大小*/
    }nal[0];
}nal_desc_t;


/*音/视频帧头*/
typedef struct frame_hdr_s {
    uint16_t   no;          /*帧序号*/
    uint16_t   enc_type;    /*编码类型*/
    uint32_t   timestamp;   /*时间卓戳 ms*/
    uint32_t   type;        /*帧类型*/
    union
    {
        frame_v_ext_t v;    /*视频扩展*/
        frame_a_ext_t a;    /*音频扩展*/
    };
    uint32_t   size;        /*帧数据长度*/
}frame_hdr_t;


/*帧描述*/
typedef struct frame_s {
    frame_hdr_t hdr;        /*帧头*/
    uint8_t    *data;       /*帧数据*/
    nal_desc_t *nal_desc;   /*视频帧NAL描述*/
}frame_t;


/*图像参数*/
typedef struct pic_parm_s {
    uint16_t sps_size;      /*sps数据长度*/
    uint16_t pps_size;      /*pps数据长度*/
    uint8_t  sps[128];      /*不含起始码*/
    uint8_t  pps[128];      /*不含起始码*/
}pic_parm_t;


/*视频信息*/
typedef struct media_v_s
{
    uint16_t enc_type;        /*编码类型*/
    uint16_t bitrate;         /*码率*/
    uint16_t samples_per_sec; /*采样率*/
    uint16_t frame_rate;      /*帧率*/
    pic_parm_t pic_parm;      /*图像参数*/
}media_v_t;


/*音频信息*/
typedef struct media_a_s
{
    uint16_t enc_type;        /*编码类型*/
    uint16_t bitrate;         /*码率*/
    uint16_t samples_per_sec; /*采样率*/
    uint16_t bits_per_sample; /*位宽*/         
    uint16_t frame_rate;      /*帧率*/
    uint16_t channel_num;     /*音频通道数目, 如 1，2 */
}media_a_t;


/*媒体信息*/
typedef struct media_info_s {
    uint32_t  mask;           /*媒体类型掩码*/
    uint32_t  begin_time;     /*媒体开始时间*/
    uint32_t  end_time;       /*媒体结束时间*/
    media_v_t video;          /*音频媒体信息*/
    media_a_t audio;          /*视频媒体信息*/
}media_info_t;


#endif  //__MEDIA_STRUCTS_H__
