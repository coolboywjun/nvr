#ifndef __STORAGE_LIB_H__
#define __STORAGE_LIB_H__

#include <storage_interface.h>

int storage_lib_record_start(int chn, int stream, int record_type);
int storage_lib_record_stop(int chn);
int storage_lib_record_set_type(int chn, StorageRecordType type);
int storage_lib_record_set_overwrite(int chn, int overwrite);
int storage_lib_record_set_minutes(int minutes);
int storage_lib_record_query(int chn_mask, time_t begin_time, time_t end_time, 
							StorageRecordType	 record_type, int *shmid, 
							RecordQueryChnInfo **chn_info, RecordQueryInfo **query_info);
void storage_lib_record_query_free(int shmid, RecordQueryChnInfo *chn_info, RecordQueryInfo *query_info);
int storage_lib_record_find(int chn_mask, int year, int mon, StorageRecordType record_type);
int storage_lib_read_start(RecordQueryInfo *query_info, StorageFragmentInfo *fragment_info, long *handle);
int storage_lib_read_stop(long handle);
int storage_lib_read_set(long handle, time_t t);
int storage_lib_read_frame(long handle, StorageReadCmd cmd, StorageFrameHdr *frameHdr, unsigned char **frame_buff);
int storage_lib_format_disk_start(int disk_idx, int check_bad);
int storage_lib_format_disk_stop(int disk_idx);
int storage_lib_format_get_percent(int *percent);
int storage_lib_get_disk_info(StorageDisksInfo *disks_info);
int storage_lib_get_record_info(StorageRecordInfo *record_info);
int storage_lib_display_info();
// pts: in ms
int storage_lib_save_jpeg(int chn, int stream, const char *jpeg_buff, int jpeg_len, unsigned long long pts);
// pts: in ms
int storage_lib_save_jpeg_by_path(int chn, int stream, const char *jpeg_path, unsigned long long pts);
int storage_lib_init();
int storage_lib_uninit();


#endif
