#ifndef _ENCODE_HEADER_
#define _ENCODE_HEADER_
s8 enc_jpeg(u16 width,u16 height,VencInputBuffer *input_buffer,char *file);
void enc_mb2_nonmb(int Width,int Height,u8*ySrc,u8 *cSrc,u8 *dsty,u8*dstuv);
void enc_exit(void);
s8 enc_init(u16 width,u16 height);
s8 enc_get_input_buf(VencInputBuffer *input_buffer);
#endif
