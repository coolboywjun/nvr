#ifndef _AUDIO_HEAD_
#include<audio_params.h>
#define _AUDIO_HEAD_
#define DATA_SIZE (160)
enum{
	AUDIO_STOP=0,
	AUDIO_PLAY,
};
enum{
	AUDIO_TYPE_PHONE=0,//从0开始，方便移位操作
	AUDIO_TYPE_ALARM,
	AUDIO_TYPE_CHANNEL,
};
extern pthread_mutex_t audio_lock;
int audio_thr_create(void *param);
int audio_thr_delete(void);
s8 audio_play(u8 audio_type);
s8 audio_stop(u8 audio_type);
s8 audio_thr_exit(void);
void audio_set_alarm_file(char*file,char flag);
void audio_set_alarm_file(char*file,char flag);
void audio_set_cmd(u32 cmd);
void audio_put_data_init(void);
void audio_put_channel_data(char*in_buf);

#endif
