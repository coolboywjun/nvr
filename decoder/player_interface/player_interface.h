#ifndef PLAYER_INTERFACE_
#define PLAYER_INTERFACE_
#include<av_pool.h>
enum{
	FLU_WORK_NOMAL_RATE=0,//正常播放
	FLU_WORK_F2_RATE,//快进
	FLU_WORK_F4_RATE,	
	FLU_WORK_F8_RATE,
	FLU_WORK_F2_IFRAME_RATE,//跳IFrame快进
	FLU_WORK_F4_IFRAME_RATE,
	FLU_WORK_F8_IFRAME_RATE,
	FLU_WORK_B2_RATE,//快退
	FLU_WORK_B4_RATE,
	FLU_WORK_B8_RATE,
	FLU_WORK_S2_RATE,//慢放
	FLU_WORK_S4_RATE,
	FLU_WORK_S8_RATE,	
};
typedef enum{
	CHANNEL_PALY=0,
	CHANNEL_STANDBY,
	CHANNEL_PAUSE,
}play_status;
typedef struct _query_info
{
	RecordQueryInfo query;
	time_t set_time;
}file_info;

typedef struct _player_status{
	time_t play_time_point;//已经播放的时间ms为单位
	unsigned int query_num;//空闲的query buffer  	正数:可用的buffer数量0:没有buffer可用
	unsigned int replay_end;//所有录像文件已经播放完1:结束0:没结束
	play_status channel_status;//通道的状态
}player_status;
////实时预览模式下，播放某个通道的视频
char player_real_play(unsigned char ch);

//将某个通道设置为实时视频预览模式
char player_set_real_play_mode(unsigned char ch);

///////停止播放某个通道的视频
char player_stop(unsigned char ch);

////获取某个通道的预览状态0:预览停止1:正在预览
char player_get_preview_status(unsigned char ch);//没一个bit对应一个通道

//获取播放器所有通道的状态
char player_get_player_status(unsigned char ch,player_status *status);

//设置显示模式1:全屏0:四分屏
char player_set_dis_mode(unsigned char mode,unsigned char ch);

//获取显示模式1:全屏0:四分屏
char player_get_display_mode(void);

///音频设置1:打开通道的音频0:关闭通道的音频
char player_set_audio_function(unsigned char ch,unsigned char cmd);
//////////////replay命令
/*
query:播放文件的结构体
time:从这个播放文件的某个时间点开始播放，相对于文件的开始时间
flag:播放文件更新的标记，1:立刻更新播放文件，0:排队更新
*/
char player_replay_play(unsigned char ch,file_info *replay_info,unsigned char flag);

//////////回放模式下，暂停某个通道的视频播放
/*
	1:暂停0:恢复
*/
char player_replay_pause_clt(unsigned char ch,unsigned char ctl);
//设置快进、快退、慢放的速度
char player_set_speed(unsigned char ch,unsigned char speed);

//将某个通道设置为回放模式
char player_set_replay_mode(unsigned char ch);

//单帧显示时的下一帧
char player_up_frame(unsigned char ch);

//单帧显示时的上一帧
char player_down_frame(unsigned char ch);
//播放报警声音,file是报警声音的文件
char player_alarm_audio(char *file);
//停止报警音频的播放
char player_stop_alarm_audio(void);
//图像抓拍功能，ch是通道号0-3,num:抓拍参数
char player_set_cap_num(unsigned char ch,unsigned char num);
//设置framebuf的透明度
char player_set_fb_alpha(unsigned char alpha);
//设置colorkey
char player_set_colorky(unsigned char r,unsigned char g,unsigned char b);
//停止播放手机音频流
char player_stop_phone_audio(void);
//播放手机音频流
char player_start_phone_audio(void);
//录音开始
char player_start_audio_record(void);
//录音停止
char player_stop_audio_record(void);
//设置录音的编码参数
char player_set_record_par(AV_AUDIO_INFO_S *par);
//设置图像的显示区域
char player_set_src_rsz(unsigned short x,unsigned short y,unsigned short w,unsigned short h);
//设置屏幕的显示区域
char player_set_scn_rsz(unsigned short x,unsigned short y,unsigned short w,unsigned short h);
//播放器进程正常退出
char player_exit(void);
//播放器进程异常情况下退出
char player_emergency_exit(void);
void player_msg_init(void);
char player_get_msg(unsigned char ch,void *infor);

#endif
