#ifndef _STREAM_HEADER_
#define _STREAM_HEADER_
extern volatile str_handle stream_handle[STREAM_CHANNEL_MAX];
extern const u8 str_msg_type[STREAM_CHANNEL_MAX];
int stream_thr_delete(u8 param);
int stream_thr_create(u8* param);
u8 stream_preview_status(u8 ch);
s8 stream_put_msg_to_queue(u8 ch,_thr_msg_data *data,u8 cmd,int flag);
void stream_audio_set_ch(u8 ch,u8 cmd);
#endif
