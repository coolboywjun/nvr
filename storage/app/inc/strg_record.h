
#ifndef __STRG_RECORD_H__
#define __STRG_RECORD_H__

#include <storage_interface.h>

int strg_record_init();

void strg_record_uninit();

int strg_record_start(int chn, int stream, StorageRecordType record_type);

void strg_record_stop(int chn);

int strg_record_set_type(int chn, StorageRecordType type);

int strg_record_set_overwrite(int chn, int overwrite);

int strg_record_set_minutes(int minutes);

int strg_record_query(int chn_mask, int shmid, time_t begin_time, time_t end_time, StorageRecordType record_type);

int strg_record_find(int chn_mask, time_t begin_time, time_t end_time, StorageRecordType record_type);

int strg_record_read_start(int shmid, RecordQueryInfo *query_info, StorageFragmentInfo *sfi, long *handle);
							
int strg_record_read_stop(long handle);

int strg_record_read_set(long handle, time_t t);

int strg_record_read_seek(long handle, int seek_iframe_offset);

int strg_record_read(long handle, StorageReadCmd cmd);

int strg_record_format_disk_start(int disk_idx, int check_bad);

void strg_record_format_disk_stop(int disk_idx);

int strg_record_format_get_percent(int *percent);

int strg_record_get_disk_info(StorageDisksInfo *disks_info);

int strg_record_get_record_info(StorageRecordInfo *record_info);

void strg_record_display_info();

int strg_record_save_jpeg(int chn, int stream, int shmid, int jpeg_len, unsigned long long pts);

int strg_record_save_jpeg_by_path(int chn, int stream, const char *path, unsigned long long pts);

#endif

