#ifndef _DISPLAY_HEADER_
#define _DISPLAY_HEADER_

#define DIS_TIME_LIMIT (17)
extern volatile dis_handle display_handle;
extern u8 *dis_back_buf;
int display_thr_delete(void);
int display_thr_create(void *param);
s8 dis_put_msg_to_queue(_thr_msg_data *data,u8 cmd,int flag);
void dis_set_clr_cmd(u8 ch,u8 cmd);
void dis_thr_full_scn_buf_pro(u8 ch,u8*ysrc,u8*uvsrc,u8 black);
s8 dis_thr_get_buf_register(u8 ch);
void dis_thr_get_buf_unregister(u8 ch);
void dis_thr_get_empty_buf(void);
void dis_thr_put_full_buf(u8*ysrc,u8*uvsrc);
void dis_thr_slip_scn_buf_pro(u8 ch,u8*ysrc,u8*uvsrc,u8 black,u8 put_dis_buf);
#endif

