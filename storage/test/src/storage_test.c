


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h> 
#include <fcntl.h>

#include <storage_lib.h>
#include <av_pool.h>

#define STREAM_NUM	0

static int quit = 0;
static void sig_quit(int signo)
{
	quit = 1;
}

int storage_test_format_disk()
{
	int percent;
	
	printf("Format disk /dev/sda\n");
	if(storage_lib_format_disk_start(0, 0) < 0)
	{
		printf("Failed to format disk /dev/sda\n");
		return -1;
	}
	do
	{
		if(storage_lib_format_get_percent(&percent) < 0)
		{
			printf("Failed to get percent!!!\n");
			break;
		}
		printf("format percent: %d\n", percent);
		if(percent == 100)
			break;
		else if(percent < 0)
		{
			printf("Failed to format disk\n");
			break;
		}
		sleep(1);
	}while(1);
	storage_lib_format_disk_stop(0);
	printf("Format disk /dev/sda successfully!!!\n");

	return 0;
}

int storage_test_start_record()
{
	int chn, stream;

	printf("Please input chn:\n");
	scanf("%d", &chn);
	printf("Please input stream:\n");
	scanf("%d", &stream);
	if(storage_lib_record_start(chn, stream, RECORD_STARTUP_TYPE) < 0)
	{
		printf("Failed to start record chn%d stream%d\n", chn, stream);
		return -1;
	}
	printf("Start record chn%d stream%d succussfully!\n", chn, stream);
	return 0;
}

int storage_test_stop_record()
{
	int chn, stream;

	printf("Please input chn:\n");
	scanf("%d", &chn);
	printf("Please input stream:\n");
	scanf("%d", &stream);
	if(storage_lib_record_stop(chn) < 0)
	{
		printf("Failed to stop record chn%d stream%d\n", chn, stream);
		return -1;
	}
	printf("Stop record chn%d stream%d succussfully!\n", chn, stream);
	return 0;
}

int storage_test_record_query()
{
	int i, ret;
//	int count;
	int chn, stream;
//	int year, mon, day, hour, min, sec;
	struct tm time_str;
	time_t begin_time, end_time;
	RecordQueryChnInfo *chn_info;
	RecordQueryInfo *query_info;
	int shmid;
	
	printf("Please input chn:\n");
	scanf("%d", &chn);
	printf("Please input stream:\n");
	scanf("%d", &stream);
#if 0	
	printf("Please input begin time(1970-5-5 17:00:00):\n");
	scanf("%d-%d-%d %d:%d:%d", &year, &mon, &day, &hour, &min, &sec);
	printf("input time: %d-%d-%d %d:%d:%d\n", year, mon, day, hour, min, sec);
	time_str.tm_year = year - 1900;
	time_str.tm_mon = mon - 1;
	time_str.tm_mday = day;
	time_str.tm_hour = hour;
	time_str.tm_min = min;
	time_str.tm_sec = sec;
	time_str.tm_isdst = -1;
	if(mktime(&time_str) == -1)
	{
		printf("make begin time failed.\n");
		return -1;
	}
	begin_time = mktime(&time_str);
	printf("Please input end time(1970-5-5 17:00:00):\n");
	scanf("%d-%d-%d %d:%d:%d", &year, &mon, &day, &hour, &min, &sec);
	printf("input time: %d-%d-%d %d:%d:%d\n", year, mon, day, hour, min, sec);
	time_str.tm_year = year - 1900;
	time_str.tm_mon = mon - 1;
	time_str.tm_mday = day;
	time_str.tm_hour = hour;
	time_str.tm_min = min;
	time_str.tm_sec = sec;
	time_str.tm_isdst = -1;
	if(mktime(&time_str) == -1)
	{
		printf("make begin time failed.\n");
		return -1;
	}
	end_time = mktime(&time_str);
#else
	// begin time : 1970-1-1 00:00:00
	time_str.tm_year = 1970 - 1900;
	time_str.tm_mon = 1 - 1;
	time_str.tm_mday = 1;
	time_str.tm_hour = 0;
	time_str.tm_min = 0;
	time_str.tm_sec = 0;
	time_str.tm_isdst = -1;
	if(mktime(&time_str) == -1)
	{
		printf("make begin time failed.\n");
		return -1;
	}
	begin_time = mktime(&time_str);

	// end time : 2010-1-1 00:00:00
	time_str.tm_year = 2020 - 1900;
	time_str.tm_mon = 1 - 1;
	time_str.tm_mday = 1;
	time_str.tm_hour = 0;
	time_str.tm_min = 0;
	time_str.tm_sec = 0;
	time_str.tm_isdst = -1;
	if(mktime(&time_str) == -1)
	{
		printf("make begin time failed.\n");
		return -1;
	}
	end_time = mktime(&time_str);
#endif	

	printf("query, chn%d, stream%d\n", chn, stream);
	ret = storage_lib_record_query(1<<chn, 
							 	begin_time, end_time, RECORD_ALL_TYPE,
							 	&shmid, &chn_info, &query_info);
	if(ret < 0)
	{
		printf("Failed to query record\n");
		return -1;
	}
	printf("chn num: %d\n", chn_info->max_chn);
	printf("fragment num: %d\n", ret);
	for(i = 0; i < chn_info->max_chn; i++)
	{
		printf("chn%d fragment num: %d\n", i, chn_info->query_info_num[i]);
	}
	for(i = 0; i < ret; i++)
	{
		char timestr1[32];
		char timestr2[32];
		struct tm *ptm;
		ptm = localtime(&query_info[i].begin_time);
		strftime(timestr1, sizeof(timestr1), "%F %T", ptm);
		ptm = localtime(&query_info[i].end_time);
		strftime(timestr2, sizeof(timestr2), "%F %T", ptm);
		printf("chn%d disk_idx: %d basket_idx: %d bf_idx: %d begin_iframe_idx: %d iframe_num: %d btime: %s etime: %s, size: %dKB, type:%x\n", 
			query_info[i].chn,
			query_info[i].disk_idx, query_info[i].basket_idx,
			query_info[i].bf_idx, query_info[i].begin_iframe_idx,
			query_info[i].iframe_num, timestr1, 
			timestr2, query_info[i].fragment_size>>10,
			query_info[i].record_type);
	}
	storage_lib_record_query_free(shmid, chn_info, query_info);
	return 0;
}

#if 0
int storage_test_record_read()
{
	int i, ret;
	int count;
	int chn, stream;
//	int year, mon, day, hour, min, sec;
	struct tm time_str;
	time_t begin_time, end_time;
	RecordQueryChnInfo *chn_info;
	RecordQueryInfo *query_info;
	RecordQueryInfo query;
	int shmid;
	long handle;
	unsigned char *frame_buff;
	int frame_len;

	// begin to query the record
	printf("Please input chn:\n");
	scanf("%d", &chn);
	printf("Please input stream:\n");
	scanf("%d", &stream);
#if 0	
	printf("Please input begin time(1970-5-5 17:00:00):\n");
	scanf("%d-%d-%d %d:%d:%d", &year, &mon, &day, &hour, &min, &sec);
	printf("input time: %d-%d-%d %d:%d:%d\n", year, mon, day, hour, min, sec);
	time_str.tm_year = year - 1900;
	time_str.tm_mon = mon - 1;
	time_str.tm_mday = day;
	time_str.tm_hour = hour;
	time_str.tm_min = min;
	time_str.tm_sec = sec;
	time_str.tm_isdst = -1;
	if(mktime(&time_str) == -1)
	{
		printf("make begin time failed.\n");
		return -1;
	}
	begin_time = mktime(&time_str);
	printf("Please input end time(1970-5-5 17:00:00):\n");
	scanf("%d-%d-%d %d:%d:%d", &year, &mon, &day, &hour, &min, &sec);
	printf("input time: %d-%d-%d %d:%d:%d\n", year, mon, day, hour, min, sec);
	time_str.tm_year = year - 1900;
	time_str.tm_mon = mon - 1;
	time_str.tm_mday = day;
	time_str.tm_hour = hour;
	time_str.tm_min = min;
	time_str.tm_sec = sec;
	time_str.tm_isdst = -1;
	if(mktime(&time_str) == -1)
	{
		printf("make begin time failed.\n");
		return -1;
	}
	end_time = mktime(&time_str);
#else
	// begin time : 1970-1-1 00:00:00
	time_str.tm_year = 1970 - 1900;
	time_str.tm_mon = 1 - 1;
	time_str.tm_mday = 1;
	time_str.tm_hour = 0;
	time_str.tm_min = 0;
	time_str.tm_sec = 0;
	time_str.tm_isdst = -1;
	if(mktime(&time_str) == -1)
	{
		printf("make begin time failed.\n");
		return -1;
	}
	begin_time = mktime(&time_str);

	// end time : 2010-1-1 00:00:00
	time_str.tm_year = 2010 - 1900;
	time_str.tm_mon = 1 - 1;
	time_str.tm_mday = 1;
	time_str.tm_hour = 0;
	time_str.tm_min = 0;
	time_str.tm_sec = 0;
	time_str.tm_isdst = -1;
	if(mktime(&time_str) == -1)
	{
		printf("make begin time failed.\n");
		return -1;
	}
	end_time = mktime(&time_str);
#endif
	printf("query, chn%d, stream%d\n", chn, stream);
printf("chn_mask: %d\n", 1<<chn);
	ret = storage_lib_record_query(1<<chn, stream, 
							 	begin_time, end_time,
							 	&shmid, &chn_info, &query_info);
	if(ret < 0)
	{
		printf("Failed to query record\n");
		return -1;
	}
	printf("chn num: %d\n", chn_info->max_chn);
	printf("fragment num: %d\n", ret);
	for(i = 0; i < chn_info->max_chn; i++)
	{
		printf("chn%d fragment num: %d\n", i, chn_info->query_info_num[i]);
	}
	if(ret == 0)
		return 0;
	for(i = 0; i < ret; i++)
	{
		printf("chn%d disk_idx: %d basket_idx: %d bf_idx: %d begin_iframe_idx: %d iframe_num: %d btime: %ld etime: %ld\n", 
			query_info[i].chn,
			query_info[i].disk_idx, query_info[i].basket_idx,
			query_info[i].bf_idx, query_info[i].begin_iframe_idx,
			query_info[i].iframe_num, query_info[i].begin_time, 
			query_info[i].end_time);
	}
	memcpy(&query, &query_info[0], sizeof(RecordQueryInfo));
	storage_lib_record_query_free(shmid, chn_info, query_info);

	// begin to read frame from storage server
	if(storage_lib_read_start(query.chn, query.disk_idx, query.basket_idx,
						   	query.bf_idx, query.begin_iframe_idx, query.iframe_num,
						   	&handle, (void**)&frame_buff) < 0)
	{
		printf("Start read failed\n");
		return -1;
	}
	while((frame_len = storage_lib_read_frame(handle, READ_NORMAL)) > 0)
	{
//		printf("get frame size: %d\n", frame_len);
//		for(i = 0; i < 16; i++)
//			printf("%02x ", frame_buff[i]);
//		printf("\n");
		for(i = 0; i < frame_len; i++)
		{
			if(frame_buff[i] != (i & 0xff))
			{
				printf("!!!!!!!!!!!!!!!!!!!!!!!! read frame buffer error!!!!!!\n");
				break;
			}
		}
		if(i == frame_len)
			printf("==========================\n");
	}
	storage_lib_read_stop(handle);
	
	return 0;
}
#else
int storage_test_record_read()
{
	int i, ret;
	int count;
	int chn, stream;
//	int fd;
//	int year, mon, day, hour, min, sec;
	struct tm time_str;
	time_t begin_time, end_time;
	RecordQueryChnInfo *chn_info;
	RecordQueryInfo *query_info;
	RecordQueryInfo query;
	StorageFragmentInfo fragment_info;
	int shmid;
	long handle;
	unsigned char *frame_buff;
	int frame_len;
	int rand;
	int cmd;
	
	// begin to query the record
	printf("Please input chn:\n");
	scanf("%d", &chn);
	printf("Please input stream:\n");
	scanf("%d", &stream);
#if 0	
	printf("Please input begin time(1970-5-5 17:00:00):\n");
	scanf("%d-%d-%d %d:%d:%d", &year, &mon, &day, &hour, &min, &sec);
	printf("input time: %d-%d-%d %d:%d:%d\n", year, mon, day, hour, min, sec);
	time_str.tm_year = year - 1900;
	time_str.tm_mon = mon - 1;
	time_str.tm_mday = day;
	time_str.tm_hour = hour;
	time_str.tm_min = min;
	time_str.tm_sec = sec;
	time_str.tm_isdst = -1;
	if(mktime(&time_str) == -1)
	{
		printf("make begin time failed.\n");
		return -1;
	}
	begin_time = mktime(&time_str);
	printf("Please input end time(1970-5-5 17:00:00):\n");
	scanf("%d-%d-%d %d:%d:%d", &year, &mon, &day, &hour, &min, &sec);
	printf("input time: %d-%d-%d %d:%d:%d\n", year, mon, day, hour, min, sec);
	time_str.tm_year = year - 1900;
	time_str.tm_mon = mon - 1;
	time_str.tm_mday = day;
	time_str.tm_hour = hour;
	time_str.tm_min = min;
	time_str.tm_sec = sec;
	time_str.tm_isdst = -1;
	if(mktime(&time_str) == -1)
	{
		printf("make begin time failed.\n");
		return -1;
	}
	end_time = mktime(&time_str);
#else
	// begin time : 1970-1-1 00:00:00
	time_str.tm_year = 1970 - 1900;
	time_str.tm_mon = 1 - 1;
	time_str.tm_mday = 1;
	time_str.tm_hour = 0;
	time_str.tm_min = 0;
	time_str.tm_sec = 0;
	time_str.tm_isdst = -1;
	if(mktime(&time_str) == -1)
	{
		printf("make begin time failed.\n");
		return -1;
	}
	begin_time = mktime(&time_str);

	// end time : 2010-1-1 00:00:00
	time_str.tm_year = 2020 - 1900;
	time_str.tm_mon = 1 - 1;
	time_str.tm_mday = 1;
	time_str.tm_hour = 0;
	time_str.tm_min = 0;
	time_str.tm_sec = 0;
	time_str.tm_isdst = -1;
	if(mktime(&time_str) == -1)
	{
		printf("make begin time failed.\n");
		return -1;
	}
	end_time = mktime(&time_str);
#endif
	printf("query, chn%d, stream%d\n", chn, stream);
	printf("chn_mask: %d\n", 1<<chn);
	ret = storage_lib_record_query(1<<chn,
							 	begin_time, end_time, RECORD_ALL_TYPE,
							 	&shmid, &chn_info, &query_info);
	if(ret < 0)
	{
		printf("Failed to query record\n");
		return -1;
	}
	printf("chn num: %d\n", chn_info->max_chn);
	printf("fragment num: %d\n", ret);
	for(i = 0; i < chn_info->max_chn; i++)
	{
		printf("chn%d fragment num: %d\n", i, chn_info->query_info_num[i]);
	}
	if(ret == 0)
		return 0;
	for(i = 0; i < ret; i++)
	{
		char timestr1[32];
		char timestr2[32];
		struct tm *ptm;
		ptm = localtime(&query_info[i].begin_time);
		strftime(timestr1, sizeof(timestr1), "%F %T", ptm);
		ptm = localtime(&query_info[i].end_time);
		strftime(timestr2, sizeof(timestr2), "%F %T", ptm);
		printf("chn%d disk_idx: %d basket_idx: %d bf_idx: %d begin_iframe_idx: %d iframe_num: %d btime: %s etime: %s, size: %dKB, type:%x\n", 
			query_info[i].chn,
			query_info[i].disk_idx, query_info[i].basket_idx,
			query_info[i].bf_idx, query_info[i].begin_iframe_idx,
			query_info[i].iframe_num, timestr1, 
			timestr2, query_info[i].fragment_size>>10,
			query_info[i].record_type);
	}
	rand = random() % 3;
//	printf("rand: %d\n", rand);
	//rand = 1;
	memcpy(&query, &query_info[rand], sizeof(RecordQueryInfo));
	storage_lib_record_query_free(shmid, chn_info, query_info);

	// begin to read frame from storage server
	if(storage_lib_read_start(&query, &fragment_info, &handle) < 0)
	{
		printf("Start read failed\n");
		return -1;
	}
#if 0	
	fd = open("/opt/video.h264", O_CREAT | O_TRUNC | O_WRONLY, 0666);
	if(fd < 0)
	{
		printf("Failed to open file.");
		return -1;
	}
#endif	
	count = 0;
	cmd = READ_FORWARD_AUDIO_FRAME;//READ_FORWARD_AUDIO_FRAME;
	while((frame_len = storage_lib_read_frame(handle, cmd, NULL, &frame_buff)) > 0)
	{
		//if(frame_len > 32)
		//	write(fd, frame_buff+32, frame_len-32);
		//else
		//	printf("!!!!!!!!!!!!!!!!!!!!!!!!\n");
		
		AV_FRAME_HEAD_S *pFrameHdr = (AV_FRAME_HEAD_S *)frame_buff;
		printf("frame len: %d, frame no: %lu, frame type: %u\n", frame_len, pFrameHdr->u32FrameNo, pFrameHdr->u8FrameType);
		if(pFrameHdr->u8FrameType == 0xa)
			printf("######################audio frame type: %u\n", pFrameHdr->unAudioInfo.u16EncodeType);
		count++;
		if(count == 1000)
			break;
		//if(count > 3)
		//	cmd = READ_FORWARD_VIDEO_FRAME;
	}
	printf("frame_len : %d\n", frame_len);
//	close(fd);
	storage_lib_read_stop(handle);
	
	return 0;
}

#endif

int storage_test_display_info()
{
	return storage_lib_display_info();
}

int storage_test_save_jpeg_by_path()
{
	char jpeg_path[256];
	struct timeval ctv;
	unsigned long long pts;
	printf("Please input jpeg path: ");
	//fgets(jpeg_path, sizeof(jpeg_path), stdin);
	strcpy(jpeg_path, "/opt/test.jpg");
	printf("Jpeg path %s\n", jpeg_path);
	gettimeofday(&ctv, NULL);
	pts = (unsigned long long)ctv.tv_sec * 1000 + ctv.tv_usec / 1000;
	
	if(storage_lib_save_jpeg_by_path(0, 0, jpeg_path,pts) < 0)
	{
		printf("Save jpeg by path failed.\n");
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	char func;
	
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, sig_quit);
	signal(SIGTERM, sig_quit);

	if(storage_lib_init() < 0)
	{
		printf("Failed to initialize storage lib\n");
		return -1;
	}
	
	while (!quit) 
	{
		printf("Please input the function:\n");
		printf("\t1: format disk\n");
		printf("\t2: record start\n");
		printf("\t3: record stop\n");
		printf("\t4: record query\n");
		printf("\t5: record read\n");
		printf("\t6: display information\n");
		printf("\t7: save jpeg by path\n");
		
		printf("\te: exit\n");
//		scanf("%c\n", &func);
//		printf("======%c\n", func);
		while((func = getchar()) == '\n');
		switch(func)
		{
			case '1':
				storage_test_format_disk();
				break;
			case '2':
				storage_test_start_record();
				break;
			case '3':
				storage_test_stop_record();
				break;
			case '4':
				storage_test_record_query();
				break;
			case '5':
				storage_test_record_read();
				break;
			case '6':
				storage_test_display_info();
				break;
			case '7':
				storage_test_save_jpeg_by_path();
				break;
			case 'e':
				goto exit_main;
				break;
			default:
				printf("input error!!!\n");
				break;
		}
	}
	
exit_main:
	storage_lib_uninit(); 

	return 0;
}
