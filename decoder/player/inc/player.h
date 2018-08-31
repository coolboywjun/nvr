#ifndef _NVR_PLAYER_
#define _NVR_PLAYER_
#include<pthread.h>
#include<av_pool.h>
#include<sys/msg.h>
#include<dec_inc/libcedarv.h>
#include<string.h> // char *strerror(int errnum);
#include<errno.h> //errno
#include<drv_display.h>
#include<storage_lib.h>
#include<storage_interface.h>
#include<dec_inc/cedarv_osal_linux.h>
//#include<os_adapter.h>
#include<osa_thr.h>
#include<glib.h>
#include<glib-unix.h>
#include<player_interface.h>
#include<audio_play.h>
#include<audio_record.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned int Bool32;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef long long s64;

typedef struct buf_head_{
	u32 time_stamp;
	u32 frame_len;
	u32 average_timestamp;
//	u64 frame_timestamp;
	u32 first_frame;
	u32 single_pic;
	u32 pic_ch;
	u32 play_id;
}buf_head;
#define PLAYER_TEST
#define FRAME_LEN_BYTES (4)
#define DECODE_BUF_SIZE (1280*720*3/2+sizeof(AV_FRAME_HEAD_S)+sizeof(buf_head))
#define DISPLAY_BUF_SIZE (1280*(736+384)+sizeof(buf_head))

#define STREAM_BUF_NUM (1)
#define DECODE_BUF_NUM (5)
#define DISPLAY_BUF_NUM (4)
#define COPY_BUF_NUM (1)

#define PLAYER_TIMEOUT_FOREVER ((u32) -1)
#define PLAYER_TIMEOUT_NONE ((u32) 0)

#define STREAM_CHANNEL_MAX (4)
#define DECODER_MAX (4)
#define FLUENCY_CHANNEL_MAX (4)
#define COPY_CHANNEL_MAX (4)
enum{
	GET_STREAM_NULL=0,
	GET_STREAM_FROME_NET,
	GET_STREAM_FROME_DISK,
};
enum{
	UP_OR_DOWN=READ_BACK_4_IFRAME+1,
	UP_FRAME,
	DOWN_FRAME,
};
typedef struct{
	u32 sleep_ms;
	pthread_mutex_t lock;
	pthread_cond_t cond;
}sleep_ctl;

typedef struct{
//	StorageReadCmd read_cmd;
	__disp_rect_t rect;
	player_status run_status/*[STREAM_CHANNEL_MAX]*/;
	buf_head head_infor;
	file_info replay_infor/*[STREAM_CHANNEL_MAX]*/;
	//u8 cap_num;
	u8 cmd;
	u8 channel;
//	u8 single_en;
	u8 rate;
	u8 record;	
	u8 single_dir;//单帧时候的方向
	u8 black_screen;
	u8 pause;
	u8 discard_num;
	u8 update_now;
//	u8 preview_status;
	u8 audio_ctl;//改
	u8 elec_rsz;//?????
//	u8 alpha;
//	u8 r;
//	u8 g;
//	u8 b;
	u32 play_id;
	u64 pts;
}_thr_msg_data;
typedef struct {
	s32 msg_type;
	_thr_msg_data thr_data;
}_thr_msg;
typedef struct{
	__disp_rect_t rect;
	player_status run_status[STREAM_CHANNEL_MAX];
	buf_head head_infor;
	file_info replay_infor[STREAM_CHANNEL_MAX];
	AV_AUDIO_INFO_S audio_enc_par;
	u8 alarm_file[15];
	u8 alarm_play;
	u8 cap_num;
	u8 cmd;
	u8 channel;
	u8 single_en;
	u8 rate;
	u8 pause;
	u8 update_now;
	u8 preview_status;
	u8 audio_ctl;
	u8 elec_rsz;
	u8 alpha;
	u8 r;
	u8 g;
	u8 b;
	u8 talk_sta;
}_pro_msg_data;
typedef struct {
	s32 msg_type;
	_pro_msg_data pro_data;
}_pro_msg;

typedef struct{
	u8 curRd;
	u8 curWr;
	u8 len;
	u8 count;
	u8 *buf_id_queue;//queue中保存的是正真buf的数组索引
	pthread_mutex_t lock;
	pthread_cond_t	condRd;
	pthread_cond_t	condWr;	
}buf_queue;
typedef struct buf_{
	buf_queue empty_queue;//维护空buf索引的队列
	buf_queue full_queue;//维护满buf索引的队列
	u8 **p_dest;//这两个buf队列所维护的实际buf
}buf_handle;

typedef struct str_{
	u32 buf_size;
	u8 *buf[STREAM_BUF_NUM];
	buf_handle *next_handle_buf;
	buf_handle cur_handle_buf;
	u8 buf_num;
	u8 full_buf_index[STREAM_BUF_NUM];
	u8 empty_buf_index[STREAM_BUF_NUM];	
}str_handle;

typedef struct dec_{
	u32 buf_size;
	u8 *buf[DECODE_BUF_NUM];
	buf_handle *next_handle_buf;
	buf_handle cur_handle_buf;	
	u8 buf_num;
	u8 full_buf_index[DECODE_BUF_NUM];	
	u8 empty_buf_index[DECODE_BUF_NUM];	
}dec_handle;

typedef struct copy_{
	u32 buf_size;
	u8 *buf[COPY_BUF_NUM];
	buf_handle *next_handle_buf;
	buf_handle cur_handle_buf;	
	u8 buf_num;
	u8 full_buf_index[COPY_BUF_NUM];	
	u8 empty_buf_index[COPY_BUF_NUM];	
}cp_handle;

typedef struct dis_{
	u32 buf_size;
	u8 *buf[DISPLAY_BUF_NUM];
	buf_handle *next_handle_buf;
	buf_handle cur_handle_buf;	
	u8 buf_num;
	u8 full_buf_index[DISPLAY_BUF_NUM];	
	u8 empty_buf_index[DISPLAY_BUF_NUM];	
}dis_handle;

#include<stream.h>
#include<display.h>
#include<decode.h>
#include<copy.h>
#include<fluency.h>
#define STREAM_THR_PRI       (OSA_THR_PRI_MAX-2)
#define DISPLAY_THR_PRI       (OSA_THR_PRI_MAX)
#define FLU_THR_PRI       (OSA_THR_PRI_MIN)
#define DECODE_THR_PRI       (OSA_THR_PRI_MAX)
#define COPY_THR_PRI       (OSA_THR_PRI_MAX-1)
#define AUDIO_THR_PRI		(OSA_THR_PRI_MIN)
#define AUDIO_RECORD_THR_PRI		(OSA_THR_PRI_MIN)

#define PLAYER_THR_IPC_KEY_ID (0x12)
#define PLAYER_PRO_IPC_KEY_ID (PLAYER_THR_IPC_KEY_ID+1)

///////定义消息类型
enum
{
	PLAYER_MSG_TYPE_RCV=0X01,
	PLAYER_MSG_TYPE_RCV_PRO,	
	PLAYER_MSG_TYPE_SEND,
	STR1_MSG_TYPE_RCV,
	STR2_MSG_TYPE_RCV,
	STR3_MSG_TYPE_RCV,
	STR4_MSG_TYPE_RCV,
	DEC1_MSG_TYPE_RCV,
	DEC2_MSG_TYPE_RCV,
	DEC3_MSG_TYPE_RCV,
	DEC4_MSG_TYPE_RCV,	
	FLU1_MSG_TYPE_RCV,
	FLU2_MSG_TYPE_RCV,
	FLU3_MSG_TYPE_RCV,
	FLU4_MSG_TYPE_RCV,	
	COPY1_MSG_TYPE_RCV,
	COPY2_MSG_TYPE_RCV,
	COPY3_MSG_TYPE_RCV,
	COPY4_MSG_TYPE_RCV,	
	MIX_MSG_TYPE_RCV,
	DIS_MSG_TYPE_RCV,
};
/////定义消息命令
enum{
/////////// 进程间通信的控制命令
	PLAYER_CMD_RESET=0,
	PLAYER_CMD_PLAY=1,
	PLAYER_CMD_STOP=2,
	PLAYER_CMD_PAUSE=3,
	PLAYER_CMD_ALL_EXIT=4,
	PLAYER_CMD_GET_PLAYER_STATUS=5,
	PLAYER_CMD_SINGAL_DISPLAY=6,
	PLAYER_CMD_SET_RATE=7,
	PLAYER_CMD_REAL_PLAY=8,
	PLAYER_CMD_RECORD_PLAY=9,
	PLAYER_CMD_CHOOSE_FILE=10,
	PLAYER_CMD_DOWN_FRAME=11,
	PLAYER_CMD_UP_FRAME=12,
	PLAYER_CMD_GET_DISPLAY_MODE=13,
	PLAYER_CMD_SET_RECORD_RECT=14,
	PLAYER_CMD_GET_PREVIEW_STATUS=15,
	PLAYER_CMD_SET_AUDIO=16,
	PLAYER_CMD_SET_ELEC_RSZ=17,
	PLAYER_CMD_ALARM_AUDIO=18,
	PLAYER_CMD_CAPTURE=19,
	PLAYER_CMD_SET_FB_ALPHA=20,
	PLAYER_CMD_SET_COLORKEY=21,	
	PLAYER_CMD_START_PHONE_AUDIO=22,	
	PLAYER_CMD_START_AUDIO_RECORD=23,	
	PLAYER_CMD_STOP_AUDIO_RECORD=24,	
	PLAYER_CMD_STOP_PHONE_AUDIO=25,
	PLAYER_CMD_SET_AUDIO_REC_PAR=26,	
	PLAYER_CMD_SET_SRC_RECT=27,
	PLAYER_CMD_SET_SCN_RECT=28,
	STREAM_CMD_STANDBY_ALREADY,//////////////////////////str 线程的控制命令
	STREAM_CMD_WORK_ALREADY,
	STREAM_CMD_SINGLE_DIS_ALREADY,
	STREAM_CMD_PAUSE_ALREADY,
	STREAM_CMD_CHOOSE_FILE_ALREADY,////////////
	STREAM_CMD_CONNECT,
	STREAM_CMD_STANDBY,
	STREAM_CMD_EXIT,
	STREAM_CMD_WORK,
	STREAM_CMD_SINGLE_FRAME,
	STREAM_CMD_PAUSE,	
	STREAM_CMD_CHOOSE_FILE,
	STREAM_CMD_SET_RATE,
	STREAM_CMD_SET_RATE_ALREADY,
	STREAM_CMD_FRAME_SYNC,
	STREAM_CMD_QUERY_PLAY_TIME,
	STREAM_CMD_QUERY_PLAY_TIME_ALREADY,
	STREAM_CMD_SET_AUDIO,
	DECODE_CMD_STANDBY,///////////dec 	线程的控制命令
	DECODE_CMD_NEW_DATA,
	DECODE_CMD_STANDBY_ALREADY,
	DECODE_CMD_WORK_ON_ALREADY,
	DECODE_CMD_WORK_ON,
	DECODE_CMD_EXIT,	
	DECODE_CMD_BLACK,	
	DECODE_CMD_FRAME_SYNC,
	DECODE_CMD_WAIT_CMD,
	DECODE_CMD_WAIT_CMD_ALREADY,	
	FLU_CMD_STANDBY,///////////flu 	线程的控制命令
	FLU_CMD_NEW_DATA,	
	FLU_CMD_WORK_ON,	
	FLU_CMD_BLACK,
	FLU_CMD_STANDBY_ALREADY,
	FLU_CMD_WORK_ON_ALREADY,	
	FLU_CMD_EXIT,
	FLU_CMD_FRAME_SYNC,
	FLU_CMD_WAIT_CMD,
	FLU_CMD_WAIT_CMD_ALREADY,
	COPY_CMD_STANDBY,///////////flu 	线程的控制命令
	COPY_CMD_NEW_DATA,
	COPY_CMD_IGNORE_DATA,
	COPY_CMD_WORK_ON,	
	COPY_CMD_BLACK,
	COPY_CMD_STANDBY_ALREADY,
	COPY_CMD_WORK_ON_ALREADY,	
	COPY_CMD_EXIT,
	COPY_CMD_FRAME_SYNC,
	COPY_CMD_WAIT_CMD,
	COPY_CMD_WAIT_CMD_ALREADY,	
	COPY_CMD_PUT_BACK_BUF,
	DISPLAY_CMD_STANDBY,/////////////////dis 线程控制命令
	DISPLAY_CMD_NEW_DATA,
	DISPLAY_CMD_WORK_ON,
	DISPLAY_CMD_STANDBY_ALREADY,
	DISPLAY_CMD_WORK_ON_ALREADY,
	DISPLAY_CMD_EXIT,
	DISPLAY_CMD_WAIT_CMD,
	DISPLAY_CMD_WAIT_CMD_ALREADY,
	DISPLAY_CMD_SET_ELEC_RSZ,
	DISPLAY_CMD_SET_SRC_RECT,
	DISPLAY_CMD_SET_SCN_RECT,
	DISPLAY_CMD_TEMP_FULLSCREEN,
	DISPLAY_CMD_TEMP_SLIPSCREEN,
	PLAYER_CMD_RET_STATUS,////////////player进程的返回命令
	PLAYER_CMD_RET_DISPLAY_MODE,
	PLAYER_CMD_RET_CHOOSE_FILE_ALREADY,
	PLAYER_CMD_RET_PREVIEW_STATUS,
};

void player_dis_buf_clr(void);
s32 player_get_empty_buf(buf_handle *buf_hdl,u8 *buf_id);
s32 player_get_empty_buf_nowait(buf_handle *buf_hdl,u8 *buf_id);
s32 player_get_full_buf(buf_handle *buf_hdl,u8 * buf_id);
s32 player_put_empty_buf(buf_handle *buf_hdl,u8 buf_id);
s32 player_put_full_buf(buf_handle *buf_hdl,u8 buf_id);
int player_mutex_create(pthread_mutex_t *lock);
int player_mutex_delete(pthread_mutex_t *lock);

void player_waitMsecs(u32 msecs);
void player_waitnsecs(u32 nsecs);
s32 player_cond_create(pthread_cond_t *cond);
s32 player_cond_delete(pthread_cond_t *cond);

s8 player_put_msg_to_queue(_thr_msg_data *data,u8 cmd,int flag);
s32 player_que_put(buf_queue*queue,u8 buf_id, Uint32 timeout);
u8 player_get_empty_buf_num(buf_handle *buf_hdl);

#endif
