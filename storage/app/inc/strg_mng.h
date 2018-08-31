
#ifndef __STRG_MNG_H__
#define __STRG_MNG_H__

#include <strg_disk.h>
#include <storage_interface.h>

int strg_mng_init();

void strg_mng_uninit();

int strg_mng_write_start(int chn, StorageRecordType record_type);

void strg_mng_write_stop(int chn);

int strg_mng_write(int chn, guint64 cur_time,
					gpointer frame_data, int frame_size, 
					int frame_no, FrameType frame_type, EncodeType encode_type);

int strg_mng_write_set_type(int chn, StorageRecordType type);

int strg_mng_write_set_overwrite(int chn, int overwrite);

int strg_mng_query(int chn_mask, time_t begin_time, time_t end_time, 
						StorageRecordType record_type, strg_query_func query_cb, gpointer data);

int strg_mng_find(int chn_mask, time_t begin_time, time_t end_time, StorageRecordType record_type);

ReadHandle strg_mng_read_start(RecordQueryInfo *query_info, StorageFragmentInfo *sfi);

void strg_mng_read_stop(ReadHandle read_handle);

int strg_mng_read_set(ReadHandle read_handle, time_t t);

int strg_mng_read_seek(ReadHandle read_handle, int seek_iframe_offset);

int strg_mng_read(ReadHandle read_handle, StorageReadCmd cmd, 
							gpointer frame_buff, int buff_len);

int strg_mng_get_disk_num();

int strg_mng_get_open_num();

int strg_mng_format_disk_start(int disk_idx, int check_bad);

void strg_mng_format_disk_stop(int disk_idx);

int strg_mng_format_get_percent(int *percent);

int strg_mng_get_disk_info(StorageDisksInfo *disks_info);

void strg_mng_display_info();



#endif
