#ifndef _DECODER_HEADER_
#define _DECODER_HEADER_
extern volatile dec_handle decode_handle[DECODER_MAX];
extern const u8 dec_msg_type[DECODER_MAX];
extern pthread_mutex_t all_decode_lock;
int decode_thr_delete(u8 param);
int decode_thr_create(u8* param);
void decode_thr_release_dis(u8 dec_id,u32 id);
s8 decode_thr_display_request(u8 dec_id,cedarv_picture_t* picture);
s8 decode_put_msg_to_queue(u8 ch,_thr_msg_data *data,u8 cmd,int flag);
void decode_set_clr_cmd(u8 ch,u8 flag);
void decode_set_out_res(u8 ch,int res);
#endif

