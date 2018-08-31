
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mount.h>
#include <time.h>
#include <sys/vfs.h>
#include <syslog.h>

#include <av_pool.h>
#include <storage_lib.h>

#include <libavformat/avformat.h>


#include <storage_ext_lib.h>


#define PRINT(fmt, args...)	\
do{		\
	printf(fmt, ##args);		\
	syslog(LOG_ERR | LOG_USER, "StorageExt " fmt, ##args);	\
}while(0);



static pthread_mutex_t export_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t export_thid;
static volatile int export_flag = 0;
static volatile int export_status = 0;
static volatile int export_exit = 0;

static volatile int export_num;
static volatile long total_iframe = 0; 
static volatile long write_iframe = 0;

int storage_ext_check_usb_storage(const char *dev_path)
{
	const char *block_dev_path = "/sys/dev/block";
	struct stat statbuf;
	int major, minor;
	char block_path[256];
	char buf[1024];
	
	if(stat(dev_path, &statbuf) < 0)
		return 0;
	major = (statbuf.st_rdev >> 8) & 0xff;
	minor = statbuf.st_rdev & 0xff;
	//major = MAJOR(statbuf.st_rdev);
	//minor = MINOR(statbuf.st_rdev);
	snprintf(block_path, sizeof(block_path), "%s/%d:%d", block_dev_path, major, minor);
	memset(buf, 0, sizeof(buf));
	if(readlink(block_path, buf, sizeof(buf)) < 0)
		return 0;
	if(strstr(buf, "usb"))
		return 1;
	return 0;
}

int storage_ext_get_udisk_device_path(char *dev_path)
{
	const char *usb_storage_path = "/proc/scsi/usb-storage";
	char path[256];
	int main_idx, sub_idx;
	struct stat statbuf;
	
	if(stat(usb_storage_path, &statbuf) < 0)
	{
		PRINT("The %s is not exist.\n", usb_storage_path);
		return -1;
	}
	// 运行到这里，说明已经有u盘插入了
	// 不再判断有几个U盘

	// 接下来判断是否usb device节点
	for(main_idx = 'a'; main_idx <= 'z'; main_idx++)
	{
		for(sub_idx = 0; sub_idx < 16; sub_idx++)
		{
			snprintf(path, sizeof(path), "/dev/sd%c%d", (char)main_idx, sub_idx);
			if(storage_ext_check_usb_storage(path))
			{
				if(dev_path != NULL)
					strcpy(dev_path, path);
				return 1;
			}
		}
		snprintf(path, sizeof(path), "/dev/sd%c", (char)main_idx);
		if(storage_ext_check_usb_storage(path))
		{
			if(dev_path != NULL)
				strcpy(dev_path, path);
			return 1;
		}
	}
	
	return 0;
}

int storage_ext_mout_device(const char *dev_path, const char* mount_path)
{
	if(dev_path == NULL || mount_path == NULL)
		return -1;
	umount(mount_path);
	return mount(dev_path, mount_path, "vfat", 0, NULL);
}

void storage_ext_umount_device(const char *mount_path)
{
	if(mount_path)
		umount(mount_path);
}

static int __storage_ext_avi_file(RecordQueryInfo *query_info, const char *mount_path)
{
	char avi_path[256];
	struct tm *ptm1, tm_str1;
	struct tm*ptm2, tm_str2;
	AVFormatContext *format_cxt;
	AVStream *st;
	AVPacket pkt;
	StorageFragmentInfo frag_info;
	long read_handle;
	AV_FRAME_HEAD_S *frame_hdr;
	unsigned char *frame_buff;
	int frame_len;
	int count = 0;
	
	if(storage_lib_read_start(query_info, &frag_info, &read_handle) < 0)
	{
		PRINT("Storage read start failed.\n");
		return -1;
	}
	
	ptm1 = localtime_r(&query_info->begin_time, &tm_str1);
	ptm2 = localtime_r(&query_info->end_time, &tm_str2);
	snprintf(avi_path, sizeof(avi_path), "%s/%04d%02d%02d[%02d%02d%02d-%02d%02d%02d].avi",
			mount_path,
			ptm1->tm_year + 1900, ptm1->tm_mon + 1, ptm1->tm_mday,
			ptm1->tm_hour, ptm1->tm_min, ptm1->tm_sec,
			ptm2->tm_hour, ptm2->tm_min, ptm2->tm_sec);
	
	// begin to write storage frame to file by ffmpeg.
	if(avformat_alloc_output_context2(&format_cxt, NULL, NULL, avi_path) < 0)
	{
		PRINT("avformat alloc output context failed.\n");
		storage_lib_read_stop(read_handle);
		return -1;
	}
	st = avformat_new_stream(format_cxt, NULL);
	if(st == NULL)
	{
		PRINT("avformat new stream failed.\n");
		avformat_free_context(format_cxt);
		storage_lib_read_stop(read_handle);
		return -1;
	}
	st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
	st->codec->codec_id = AV_CODEC_ID_H264;
	st->codec->codec_tag             = 0;
	st->codec->time_base.num = 1;
	st->codec->time_base.den = 25;
	st->codec->width = 1280;
	st->codec->height = 720;
#ifdef ARCH_ARM	
	st->codec->pix_fmt = AV_PIX_FMT_YUV420P;
#else
	st->codec->pix_fmt = PIX_FMT_YUV420P;
#endif

	av_dump_format(format_cxt, 0, avi_path, 1);
	/* 打开输出文件 */
	if (!(format_cxt->oformat->flags & AVFMT_NOFILE)) 
	{
		if (avio_open(&format_cxt->pb, avi_path, AVIO_FLAG_WRITE) < 0) 
		{
			PRINT("avio open failed.\n");
			avformat_free_context(format_cxt);
			storage_lib_read_stop(read_handle);
			return -1;
		}
	}
	
	/* 写数据的头部信息*/
	avformat_write_header(format_cxt, NULL);
	while((frame_len = storage_lib_read_frame(read_handle, READ_NORMAL, NULL, &frame_buff)) > 0)
	{
		av_init_packet(&pkt);
		frame_hdr = (AV_FRAME_HEAD_S *)frame_buff;
		if(frame_hdr->u8FrameType == AV_AUDIO_FRAME_A)
			continue;
		pkt.stream_index = st->index;
		pkt.data = frame_buff + AV_FRAME_HEAD_LEN;
		pkt.size = frame_len - AV_FRAME_HEAD_LEN;
		if(frame_hdr->u8FrameType == AV_VIDEO_FRAME_I)
		{
			pkt.flags |= AV_PKT_FLAG_KEY;
			pthread_mutex_lock(&export_mutex);
			write_iframe++;
			pthread_mutex_unlock(&export_mutex);
		}
		pkt.pts = count;
		pkt.dts = count;
		av_dup_packet(&pkt);
		if(av_write_frame(format_cxt, &pkt) < 0)
		{
			PRINT("Write frame failed.\n");
			break;
		}
		av_free_packet(&pkt);
		count++;
		if(export_exit)
			break;
	}
	av_write_trailer(format_cxt);
	/*关闭输出文件*/
	if (!(format_cxt->oformat->flags & AVFMT_NOFILE))
		avio_close(format_cxt->pb);

	/* 释放流空间 */
	avformat_free_context(format_cxt);
	
	storage_lib_read_stop(read_handle);
	
	return 0;
}

int storage_ext_is_udisk_insert()
{
	return (storage_ext_get_udisk_device_path(NULL) == 1);
}

static void *__storage_ext_avi_thread_func(void *data)
{
	RecordQueryInfo *query_info = (RecordQueryInfo*)data;
	int i;
	char dev_path[256];
	const char *mount_path = STORAGE_USE_MOUNT_PATH;
	long long total_size = 0;
	struct statfs statbuf;
	STORAGE_RESULT result = STORAGE_ERR_OK;
	
	if(storage_ext_get_udisk_device_path(dev_path) != 1)
	{
		PRINT("Find the udisk failed.\n");
		result = STORAGE_ERR_FAULT;
		goto exit1;
	}
	PRINT("dev path: %s\n", dev_path);
	PRINT("mount path: %s\n", mount_path);
	umount(mount_path);
	if(mount(dev_path, mount_path, "vfat", 0, NULL) < 0)
	{
		PRINT("Mount udisk failed.\n");
		result = STORAGE_ERR_FAULT;
		goto exit1;
	}
	pthread_mutex_lock(&export_mutex);
	for(i = 0; i < export_num; i++)
	{
		PRINT("fragment size: %d\n", query_info[i].fragment_size);
		total_iframe += query_info[i].iframe_num;
		total_size += query_info[i].fragment_size;
	}
	PRINT("total iframe num: %ld\n", total_iframe);
	pthread_mutex_unlock(&export_mutex);
	if(statfs(mount_path, &statbuf) < 0)
	{
		PRINT("Get filesys info failed.\n");
		umount(mount_path);
		result = STORAGE_ERR_FAULT;
		goto exit1;
	}
	PRINT("free space: %lld, total_size: %lld.\n", (long long)statbuf.f_bsize * statbuf.f_bfree * 98 / 100,
		total_size);
	if((long long)statbuf.f_bsize * statbuf.f_bfree * 98 / 100 < total_size)
	{
		PRINT("There is not enough space.\n");
		umount(mount_path);
		result = STORAGE_ERR_UDISK_SPACE;
		goto exit1;
	}
	for(i = 0; i < export_num && export_exit == 0; i++)
	{
		__storage_ext_avi_file(&query_info[i], mount_path);
	}
	umount(mount_path);
	result = STORAGE_ERR_FINISH;
	
exit1:
	free(query_info);
	export_status = result;
	export_flag = 0;
	return NULL;
}

STORAGE_RESULT storage_ext_avi_files(RecordQueryInfo *query_info, int query_num)
{
	RecordQueryInfo *info;
	
	if(query_info == NULL || query_num <= 0)
	{
		PRINT("The record info or the export num %d is wrong.\n", query_num);
		return STORAGE_ERR_FAULT;
	}
	if(!storage_ext_is_udisk_insert())
	{
		PRINT("The udisk is not insert.\n");
		return STORAGE_ERR_NO_UDISK;
	}
	
	pthread_mutex_lock(&export_mutex);
	if(export_flag)
	{
		PRINT("The storage export is busy.\n");
		pthread_mutex_unlock(&export_mutex);
		return STORAGE_ERR_BUSY;
	}
	export_flag = 1;
	export_exit = 0;
	write_iframe = 0;
	total_iframe = 0;
	export_status = 0;
	
	info = malloc(sizeof(RecordQueryInfo) * query_num);
	if(info == NULL)
	{
		PRINT("Failed to malloc record query info.\n");
		export_flag = 0;
		pthread_mutex_unlock(&export_mutex);
		return STORAGE_ERR_FAULT;
	}
	memcpy(info, query_info, sizeof(RecordQueryInfo) * query_num);
	export_num = query_num;
	if(pthread_create(&export_thid, NULL, __storage_ext_avi_thread_func, info) != 0)
	{
		PRINT("Failed to create export thread.\n");
		free(info);
		pthread_mutex_unlock(&export_mutex);
		return STORAGE_ERR_FAULT;
	}
	pthread_mutex_unlock(&export_mutex);
	
	return STORAGE_ERR_OK;
}

STORAGE_RESULT storage_ext_avi_file(RecordQueryInfo *query_info)
{
	return storage_ext_avi_files(query_info, 1);
}

STORAGE_RESULT storage_ext_get_avi_file_size(int chn_mask, time_t begin_time, time_t end_time, StorageRecordType record_type, long long *frame_size)
{
	int shmid;
	RecordQueryChnInfo *chn_info;
	RecordQueryInfo *query_info;
	int i, num;
	long long total_size = 0;

	if(frame_size == NULL)
	{
		return STORAGE_ERR_FAULT;
	}
	*frame_size = total_size;
	
	if((num = storage_lib_record_query(chn_mask, begin_time, end_time, record_type,
							&shmid, &chn_info, &query_info)) < 0)
	{
		PRINT("Query storage file for export avi file failed.\n");
		return STORAGE_ERR_FAULT;
	}
	if(num == 0)
	{
		PRINT("Don't find the record fragment.\n");
		storage_lib_record_query_free(shmid, chn_info, query_info);	
		return STORAGE_ERR_NO_DATA;
	}

	for(i=0; i<num; i++)
	{
		total_size +=  query_info[i].fragment_size;
	}
	*frame_size = total_size;
	
	storage_lib_record_query_free(shmid, chn_info, query_info);	
	
	return STORAGE_ERR_OK;
}

STORAGE_RESULT storage_ext_avi_file_by_detail(int chn_mask, time_t begin_time, time_t end_time, StorageRecordType record_type)
{
	int shmid;
	RecordQueryChnInfo *chn_info;
	RecordQueryInfo *query_info;
	int ret, num;
	
	if((num = storage_lib_record_query(chn_mask, begin_time, end_time, record_type,
							&shmid, &chn_info, &query_info)) < 0)
	{
		PRINT("Query storage file for export avi file failed.\n");
		//storage_lib_record_query_free(shmid, chn_info, query_info);	
		return STORAGE_ERR_FAULT;
	}
	if(num == 0)
	{
		PRINT("Don't find the record fragment.\n");
		storage_lib_record_query_free(shmid, chn_info, query_info);	
		return STORAGE_ERR_NO_DATA;
	}
	if((ret = storage_ext_avi_files(query_info, num)) < 0)
	{
		PRINT("Storage export avi file failed.\n");
		storage_lib_record_query_free(shmid, chn_info, query_info);
		return ret;		
	}
	storage_lib_record_query_free(shmid, chn_info, query_info);	

	return STORAGE_ERR_OK;
}

int storage_ext_avi_finish()
{
	int status;
	
	pthread_mutex_lock(&export_mutex);
	if(export_flag == 0)
	{
		pthread_mutex_unlock(&export_mutex);
		return STORAGE_ERR_IDLE;
	}
	export_exit = 1;
	pthread_join(export_thid, NULL);
	export_flag = 0;
	status = export_status;
	pthread_mutex_unlock(&export_mutex);

	return status;
}

int storage_ext_get_percent()
{
	int percent;
	pthread_mutex_lock(&export_mutex);
	//if(!export_flag)
	//{
	//	pthread_mutex_unlock(&export_mutex);
	//	return -1;
	//}
	if(export_status == STORAGE_ERR_OK)
	{
		if(total_iframe > 0)
		{
			percent = write_iframe * 100 /  (total_iframe + 1);
		}
		else
			percent = 0;
	}
	else if(export_status == STORAGE_ERR_FINISH)
	{
		percent = 100;
	}
	else
		percent = export_status;
	pthread_mutex_unlock(&export_mutex);
		
	return percent;
}

int storage_ext_init()
{
	av_register_all();
	return storage_lib_init();
}

void storage_ext_uninit()
{
	storage_lib_uninit();
}

