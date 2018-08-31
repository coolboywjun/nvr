#ifndef __STORAGE_EXPORT_LIB_H__
#define __STORAGE_EXPORT_LIB_H__

#include <storage_lib.h>

#define STORAGE_USE_MOUNT_PATH	"/media/udisk"

typedef enum STORAGE_RESULT
{
	STORAGE_ERR_FINISH = 1,
	STORAGE_ERR_OK = 0,
	STORAGE_ERR_PARAM	=	-1,
	STORAGE_ERR_NO_UDISK =	-2,
	STORAGE_ERR_UDISK_SPACE =	-3,
	STORAGE_ERR_BUSY	= -4,
	STORAGE_ERR_FAULT = -5,
	STORAGE_ERR_IDLE = -6,
	STORAGE_ERR_NO_DATA = -7,
}STORAGE_RESULT;

int storage_ext_is_udisk_insert();

int storage_ext_get_udisk_device_path(char *dev_path);

int storage_ext_mout_device(const char *dev_path, const char* mount_path);

void storage_ext_umount_device(const char *mount_path);

STORAGE_RESULT storage_ext_avi_files(RecordQueryInfo *query_info, int query_num);

STORAGE_RESULT storage_ext_avi_file(RecordQueryInfo *query_info);

STORAGE_RESULT storage_ext_avi_file_by_detail(int chn_mask, time_t begin_time, time_t end_time, StorageRecordType record_type);

STORAGE_RESULT storage_ext_get_avi_file_size(int chn_mask, time_t begin_time, time_t end_time, StorageRecordType record_type, long long *frame_size);


// 调完导出完成后再 调用这个函数
int storage_ext_avi_finish();

int storage_ext_get_percent();

int storage_ext_init();

void storage_ext_uninit();

#endif

