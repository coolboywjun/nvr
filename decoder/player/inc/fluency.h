#ifndef _FLUENCY_HEADER_
#define _FLUENCY_HEADER_
extern const u8 flu_msg_type[FLUENCY_CHANNEL_MAX];
int fluency_thr_create(u8 *param);
int fluency_thr_delete(u8 param);
s8 fluency_put_msg_to_queue(u8 ch,_thr_msg_data *data,u8 cmd,int flag);
void fluency_set_clr_cmd(u8 ch,u8 flag);
void fluency_set_rate(u8 ch,u8 rate);
void fluency_set_play_id(u8 ch, u32 play_id);
u32 fluency_get_play_id(u8 ch);
#endif

