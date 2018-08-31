#ifndef __STRG_INTERFACE_H__
#define __STRG_INTERFACE_H__

#include <sys/time.h>


#define MAX_DISK_NUM 4
#define STORAGE_SVR_MAGIC	0xab0000ba

#define STORAGE_SERVER_NAME	"/tmp/storage"

enum
{
	STORAGE_RECORD_START = 1,
	STORAGE_RECORD_STOP,
	STORAGE_RECORD_SET_TYPE,
	STORAGE_RECORD_SET_OVERWRITE,
	STORAGE_RECORD_SET_MINUTES,
	
	STORAGE_RECORD_QUERY,
	STORAGE_RECORD_FIND,
	
	STORAGE_READ_START,
	STORAGE_READ_STOP,
	STORAGE_READ_SET,
	STORAGE_READ_SEEK,
	STORAGE_READ_FRAME,
	STORAGE_FORMAT_DISK_START,
	STORAGE_FORMAT_DISK_STOP,
	STORAGE_FORMAT_PERCENT,
	STORAGE_GET_DISK_INFO,
	STORAGE_GET_RECORD_INFO,
	STORAGE_SAVE_JPEG,
	STORAGE_SAVE_JPEG_BY_PATH,
	
	STORAGE_DISPLAY_INFO,
};

typedef enum StorageReadCmd
{
	READ_NORMAL = 0,
	READ_FORWARD_VIDEO_FRAME,
	READ_FORWARD_AUDIO_FRAME,
	READ_FORWARD_1_IFRAME,
	READ_FORWARD_2_IFRAME,
	READ_FORWARD_3_IFRAME,
	READ_FORWARD_4_IFRAME,
	READ_BACK_1_IFRAME,
	READ_BACK_2_IFRAME,
	READ_BACK_3_IFRAME,
	READ_BACK_4_IFRAME,
}StorageReadCmd;

typedef enum StorageRecordType
{
	RECORD_STARTUP_TYPE = (1<<0),
	RECORD_TIME_TYPE = (1<<1),
	RECORD_ALARM_TYPE = (1<<2),
	RECORD_MANUAL_TYPE = (1<<3),
	RECORD_JPEG_TYPE = (1<<4),
	
	RECORD_ALL_TYPE = 0xff,

	RECORD_AUDIO_G711A = (1<<28),
	RECORD_AUDIO_G711U = (1<<29),
	RECORD_VIDEO		= (1<<30),
}StorageRecordType;

typedef struct StorageSvrNetHdr
{
	int magic;
	int cmd;
	int chn;
	int stream;
	int result;
	union
	{
	//	char data[4];
		long param[4];
	};
	int ext_data_len;
}StorageSvrNetHdr;


typedef struct RecordQueryChnInfo
{
	int max_chn;
	int query_info_num[32];
}RecordQueryChnInfo;

typedef struct RecordQueryInfo
{
	int chn;
	int disk_idx;
	int basket_idx;
	int bf_idx;
	int begin_iframe_idx;
	int iframe_num;
	time_t begin_time;
	time_t end_time;
	int fragment_size;
	StorageRecordType record_type;
}RecordQueryInfo;


typedef struct StorageQueryCmd
{
	int shmid;
	time_t begin_time;
	time_t end_time;
}StorageQueryCmd;


typedef struct StorageReadStartCmd
{
	int shmid;
	RecordQueryInfo query_info;
}StorageReadStartCmd;


typedef struct StorageFragmentInfo
{
	time_t begin_time;
	time_t end_time;
	int total_frame_cnt;
	int fragment_size;
	StorageRecordType record_type;
}StorageFragmentInfo;


typedef enum StorageDiskStatus
{
	DISK_NO_DISK= 0,
	DISK_NO_FORMATED,
	DISK_FORMATTING,
	DISK_MOUNTED,
	DISK_USEING,
	DISK_ERROR,
}StorageDiskStatus;

typedef struct StorageDiskInfo
{
	int disk_idx;
	unsigned int total_size;	// mb
	unsigned int free_size;		// mb
	StorageDiskStatus disk_status;	//  Ó²ÅÌ×´Ì¬
}StorageDiskInfo;

typedef struct StorageDisksInfo
{
	int disk_num;
	StorageDiskInfo disk_info[16];
}StorageDisksInfo;

typedef struct StorageRecordInfo
{
	int record_mask;
	int format_status;
}StorageRecordInfo;

typedef struct StorageFrameHdr
{
	struct timeval		ts;
}StorageFrameHdr;

#endif

