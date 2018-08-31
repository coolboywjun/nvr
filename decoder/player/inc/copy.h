#ifndef _COPY_HEADER_
#define _COPY_HEADER_
extern volatile cp_handle copy_handle[FLUENCY_CHANNEL_MAX];
extern const u8 copy_msg_type[FLUENCY_CHANNEL_MAX];
//extern u8 *copy_720p_noMB_buf,*copy_360p_noMB_buf;

int copy_thr_delete(u8 param);
int copy_thr_create(u8* param);
int copy_thr_copy_image(u8 ch_id,u8 func,u8 *y_src,u8*uv_src,u8 *dst);
s8 copy_put_msg_to_queue(u8 ch,_thr_msg_data *data,u8 cmd,int flag);
void copy_set_clr_cmd(u8 ch,u8 flag);
void copy_add_cat_num(u8 ch,u8 num);
void copy_set_single_channel(int ch);
void copy_set_play_id(u8 ch, u32 play_id);
u32 copy_get_play_id(u8 ch);
void copy_set_pts(u8 ch, u64 pts);
u64 copy_get_pts(u8 ch);

#endif

