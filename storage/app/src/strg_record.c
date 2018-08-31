
#include <strg_header.h>
#include <storage_interface.h>
#include <strg_mng.h>
#include <strg_record.h>
#include <strg_common.h>

#include <av_pool.h>
#include <gpio.h>


typedef struct RecordQueryList
{
	GList *list[MAX_CHN];
}RecordQueryList;

typedef struct RecordReadInfo
{
	int shmid;
	int shm_size;		// 共享内存大小
	gpointer shm;		// 共享内存指针

	ReadHandle read_handle;
	void *data;
}RecordReadInfo;

typedef struct RecordWriteInfo
{
	int chn;
	int stream;

	int enable;		// 0,没有开始录像，1，已开始录像
	int av_status;	// 0,avpool没有数据，1,avpool有数据
	int write_status;	// 0,没有写入，1,正在写入
	StorageRecordType record_type;		// 录像类型
	int av_timeout;
	time_t write_uptime;
	unsigned long write_frame_no;
	int write_frame_num;
	int fragment_num;
	int audio_type;
	unsigned long long last_frame_ts;
	
	long lReadId;

	volatile gint quit;
	gchar	th_name[32];
	GThread *thread;
}RecordWriteInfo;


typedef struct RecordMng
{
	GList	*read_list;
	RecordWriteInfo write_info[MAX_CHN];
	GMutex mutex[MAX_CHN];
}RecordMng;

RecordMng g_record_mng;
static gint g_format_status;
static gint g_record_chn;
static gint g_record_chn_bak;
static gint g_record_status;
static long g_record_gpio;
static long g_fan_gpio;
gint g_record_minutes;

static GThread *g_format_thread;

#define RECORD_GPIO	77
#define FAN_GPIO		28

#define FAN_ON	1
#define FAN_OFF (!FAN_ON)

static gboolean strg_record_timeout(gpointer user_data);

int strg_record_init()
{
	int chn;
	
	g_atomic_int_set(&g_format_status, 0);
	g_atomic_int_set(&g_record_chn, 0);
	g_atomic_int_set(&g_record_chn_bak, 0);
	g_atomic_int_set(&g_record_status, 0);
	g_atomic_int_set(&g_record_minutes, 0);
	
	g_record_gpio = gpio_open(RECORD_GPIO);
	gpio_set_direction(g_record_gpio, 0);
	g_fan_gpio = gpio_open(FAN_GPIO);
	gpio_set_direction(g_fan_gpio, 0);
	gpio_set_value(g_fan_gpio, FAN_OFF);
	
	if(av_pool_init() < 0)
	{
		j_warning("Init av pool failed.");
		return -1;
	}
	memset(&g_record_mng, 0, sizeof(g_record_mng));
	for(chn = 0; chn < MAX_CHN; chn++)
		g_mutex_init(&g_record_mng.mutex[chn]);
	g_timeout_add_seconds(1, strg_record_timeout, NULL);
	
	return 0;
}

void strg_record_uninit()
{
	int chn;
	GList *list;
	
	// 关闭所有读取录像
	for(list = g_list_first(g_record_mng.read_list); list != NULL; list = g_list_next(list))
		strg_record_read_stop(GPOINTER_TO_INT(list->data));
	// 关闭所有录像
	for(chn = 0; chn < MAX_CHN; chn++)
		 strg_record_stop(chn);
	g_list_free(g_record_mng.read_list);
	for(chn = 0; chn < MAX_CHN; chn++)
		g_mutex_clear(&g_record_mng.mutex[chn]);
	av_pool_uninit();
	if(g_record_gpio != 0)
	{
		gpio_set_value(g_record_gpio, 0);
		gpio_close(g_record_gpio);
	}
	if(g_fan_gpio != 0)
	{
		gpio_set_value(g_fan_gpio, FAN_OFF);
		gpio_close(g_fan_gpio);
	}
}

static gboolean strg_record_timeout(gpointer user_data)
{
	static int led_status = 0;	
	static int fan_status = 0;	// 0:idle状态，1:运行状态，2:休息状态
	static gint64 fan_set_time = 0;
	gint64 monotonic = g_get_monotonic_time();
	
	if(g_atomic_int_get(&g_record_status))
	{
		led_status = !led_status;
		//j_debug("Set storage led %d.", led_status);
		gpio_set_value(g_record_gpio, led_status);
	}
	else
	{	
		if(strg_mng_get_disk_num() > 0)
		{
			if(led_status == 0)
			{
				led_status = !led_status;
				gpio_set_value(g_record_gpio, led_status);
			}
		}
		else
		{
			if(led_status)
			{
				led_status = 0;
				gpio_set_value(g_record_gpio, led_status);
			}
		}
	}

	if(g_atomic_int_get(&g_record_status))
	{
		if(fan_status == 0)
		{
			gpio_set_value(g_fan_gpio, FAN_ON);
			fan_set_time = monotonic;
			fan_status = 1;
		}
		else if(fan_status == 1 && monotonic - fan_set_time > 3600LL * 1000 * 1000)	// 1 hour
		{
			gpio_set_value(g_fan_gpio, FAN_OFF);
			fan_set_time = monotonic;
			fan_status = 2;
		}
		else if(fan_status == 2 && monotonic - fan_set_time > 120LL * 1000 * 1000)	// 2 min
		{
			gpio_set_value(g_fan_gpio, FAN_ON);
			fan_set_time = monotonic;
			fan_status = 1;
		}
	}
	else
	{
		if(fan_status == 1 && monotonic - fan_set_time > 120LL * 1000 * 1000)	// 2 min
		{
			gpio_set_value(g_fan_gpio, FAN_OFF);
			fan_set_time = monotonic;
			fan_status = 2;
		}
	}
	
	return G_SOURCE_CONTINUE;
}

#if 1
static int strg_record_av_read_start(RecordWriteInfo *write_info)
{
	int chn = write_info->chn;
	int stream = write_info->stream;

	if(av_pool_start_read_frame(chn, stream, 100000, &write_info->lReadId) < 0)
		return -1;
	write_info->av_status = 1;

	return 0;
}

static void strg_record_av_read_stop(RecordWriteInfo *write_info)
{
	av_pool_stop_read_frame(write_info->lReadId);
	write_info->av_status = 0;
}

static void strg_record_check_av_pool(RecordWriteInfo *write_info)
{
	g_assert(write_info != NULL);

	if(write_info->av_status == 0)
	{
		if(av_pool_is_writing(write_info->chn, write_info->stream))
			strg_record_av_read_start(write_info);
	}
	else
	{
		if(!av_pool_is_writing(write_info->chn, write_info->stream))
			strg_record_av_read_stop(write_info);
	}
}

static inline FrameType strg_record_convert_frame_type(AV_FRAME_HEAD_S *hdr)
{
	switch(hdr->u8FrameType)
	{
		case AV_VIDEO_FRAME_I:
			return FRAME_TYPE_I;
		case AV_VIDEO_FRAME_B:
			return FRAME_TYPE_B;
		case AV_AUDIO_FRAME_A:
			return FRAME_TYPE_A;
		case AV_VIDEO_FRAME_P:
			/* break; */
		default:
			return FRAME_TYPE_P;
	}
	return FRAME_TYPE_P;
}

static inline EncodeType strg_record_convert_encode_type(AV_FRAME_HEAD_S *hdr)
{
	if(hdr->u8FrameType == AV_AUDIO_FRAME_A)
	{
		if(hdr->unAudioInfo.u16EncodeType == 1)
			return ENCODE_TYPE_G711A;
		else if(hdr->unAudioInfo.u16EncodeType == 2)
			return ENCODE_TYPE_G711U;
		else
			return ENCODE_TYPE_G711A;
	}
	else
		return ENCODE_TYPE_H264;
}

static inline int strg_record_write_start(RecordWriteInfo *write_info)
{
	if(strg_mng_write_start(write_info->chn, write_info->record_type) < 0)
		return -1;
	if((write_info->record_type & RECORD_ALARM_TYPE) && write_info->fragment_num == 0)
		write_info->av_timeout = -1;
	write_info->write_status = 1;
	write_info->write_frame_no = 0;
	write_info->write_frame_num = 0;
	write_info->fragment_num++;
	write_info->audio_type = -1;
	write_info->last_frame_ts = 0;
	g_atomic_int_or(&g_record_status, (1<<write_info->chn));
	return 0;
}

static inline void strg_record_write_stop(RecordWriteInfo *write_info)
{
	strg_mng_write_stop(write_info->chn);
	write_info->write_status = 0;
	g_atomic_int_and(&g_record_status, ~(1<<write_info->chn));
}

static int strg_record_write(RecordWriteInfo *write_info, guint64 cur_time, char *frame_buf, int frame_len)
{	
	int ret;
	int write_len = 0;
	AV_FRAME_HEAD_S *hdr = (AV_FRAME_HEAD_S*)frame_buf;

	//j_debug("frametype: %d, i: %d", hdr->u8FrameType, i);			
	//j_debug("frame no: %d, frame num: %d", write_info->write_frame_no, write_info->write_frame_num);
	//j_debug("framesize: %d frameno: %d frametype: %d", frame_len, write_info->write_frame_no, hdr->u8FrameType);
	if(write_info->write_frame_num == 0 && hdr->u8FrameType != AV_VIDEO_FRAME_I)
		return 0;
/*	
	//  如果帧不连续，则分成不同的段
	if(write_info->write_frame_num != 0 && write_info->write_frame_no != hdr->u32FrameNo - 1 
			&& hdr->u8FrameType != AV_AUDIO_FRAME_A && hdr->u8FrameType != AV_VIDEO_FRAME_I)
	{
		j_message("Ch %d stream %d write_frame_no = %lu cur_frame_no: %lu", 
				write_info->chn, write_info->stream,
				write_info->write_frame_no, hdr->u32FrameNo);
		j_debug("stop");
		strg_record_write_stop(write_info);
		if(hdr->u8FrameType != AV_VIDEO_FRAME_I)
			return 0;
		j_debug("start");
		if(strg_record_write_start(write_info) < 0)
			return -1;
	}
*/
	if(write_info->write_frame_num != 0 && hdr->u8FrameType == AV_AUDIO_FRAME_A 
		&& write_info->audio_type >= 0 && write_info->audio_type != hdr->unAudioInfo.u16EncodeType)
	{
		j_message("stop");
		strg_record_write_stop(write_info);
		return 0;
	}
	else if(write_info->write_frame_num != 0 && hdr->u8FrameType != AV_AUDIO_FRAME_A
		&& (write_info->last_frame_ts > hdr->u64TimeStamp ||
			hdr->u64TimeStamp - write_info->last_frame_ts > 5000000ULL))
	{
		j_message("stop");
		strg_record_write_stop(write_info);
		if(hdr->u8FrameType != AV_VIDEO_FRAME_I)
			return 0;
		j_message("start");
		if(strg_record_write_start(write_info) < 0)
			return -1;
	}
	
	do
	{
		//j_debug("encode_type: %d", strg_record_convert_encode_type(hdr));
		ret = strg_mng_write(write_info->chn, cur_time,
						frame_buf, frame_len, 
						hdr->u32FrameNo, 
						strg_record_convert_frame_type(hdr),
						strg_record_convert_encode_type(hdr));
		if(ret < 0)
		{
			j_debug("stop");
			strg_record_write_stop(write_info);
			return -1;
		}
		else if(ret == 0)
		{
			j_debug("stop");
			strg_record_write_stop(write_info);
			if(hdr->u8FrameType != AV_VIDEO_FRAME_I)
				return 0;
			j_debug("start");
			if(strg_record_write_start(write_info) < 0)
				return -1;
			continue;
		}
		else
		{
			write_info->av_timeout = 500;
			if(hdr->u8FrameType == AV_VIDEO_FRAME_I ||
				hdr->u8FrameType == AV_VIDEO_FRAME_P ||
				hdr->u8FrameType == AV_VIDEO_FRAME_B)
			{
				write_info->write_frame_no = hdr->u32FrameNo;
				write_info->write_frame_num++;
				write_info->last_frame_ts = hdr->u64TimeStamp;
			}
			else if(hdr->u8FrameType == AV_AUDIO_FRAME_A)
			{	
				write_info->audio_type = hdr->unAudioInfo.u16EncodeType;
			}
			write_len += ret;
			break;
		}
	}while(1);

	return write_len;
}

// use av pool to read frames
static gpointer strg_record_thread_func(gpointer data)
{
	int ret;
	int chn = GPOINTER_TO_INT(data);
	RecordWriteInfo *write_info;
	long uptime;
	struct timespec tp;
	struct timeval tv;
	int pre_time[MAX_CHN];
	int pre_day[MAX_CHN];
	time_t cur_time;
	struct tm stm, *ptm;
	int frame_len, buf_len;
	char *frame_buf;
	
	write_info = (RecordWriteInfo *)&g_record_mng.write_info[chn];
	while(!write_info->quit)
	{
		strg_record_check_av_pool(write_info);
		if(write_info->av_status == 0)
		{
			if(write_info->write_status)
			{
				j_debug("stop");
				strg_record_write_stop(write_info);
			}
			sleep(1);
			continue;
		}
		else
		{
			clock_gettime(CLOCK_MONOTONIC, &tp);
			uptime = tp.tv_sec;
			gettimeofday(&tv, NULL);
			cur_time = tv.tv_sec;
			ptm = localtime_r(&cur_time, &stm);
			
			if(write_info->write_status == 0)
			{
				//j_debug("start");
				if(strg_record_write_start(write_info) < 0)
				{
					sleep(3);
					continue;
				}
			}
			if(write_info->write_status && write_info->write_frame_num > 0
				&& (uptime - write_info->write_uptime > 5 
				|| cur_time - pre_time[write_info->chn] > 5
				|| cur_time - pre_time[write_info->chn] < 0
				|| pre_day[write_info->chn] != ptm->tm_mday))
			{
				j_debug("stop");
				j_debug("write frame_num: %d", write_info->write_frame_num);
				j_debug("uptime: %ld, write_uptime: %ld", uptime, write_info->write_uptime);
				j_debug("cur_time: %ld, pre_time: %d", cur_time, pre_time[write_info->chn]);
				j_debug("cur_mday: %d, pre_mday: %d", ptm->tm_mday, pre_day[write_info->chn]);
				strg_record_write_stop(write_info);
				continue;
			}
			
			// read frame from av pool and write to disk.
			frame_buf =  NULL;
			buf_len = 0;
			frame_len = av_pool_read_frame(write_info->lReadId, 
							&frame_buf, &buf_len, write_info->av_timeout);								
			if(frame_buf != NULL && buf_len > 0 && frame_len > 0)
			{
				guint64 t = (guint64)tv.tv_sec * 1000 + (guint64)tv.tv_usec / 1000;
				ret =strg_record_write(write_info, t, frame_buf, frame_len);
				if(ret < 0)
					j_warning("Record write frame failed.");
				else if(ret > 0)
				{
					write_info->write_uptime = uptime;
					pre_time[write_info->chn] = cur_time;
					pre_day[write_info->chn] = ptm->tm_mday;
				}
			}
			else
			{
				if(write_info->av_timeout <= 0)
					usleep(100000);
			}
		}
	}

	// 线程退出，清理资源
	if(write_info->write_status)
		strg_record_write_stop(write_info);
	if(write_info->av_status)
		strg_record_av_read_stop(write_info);
	
	return NULL;
}



#else

static gpointer strg_record_func(gpointer data)
{
	int chn;
	RecordWriteInfo *write_info;
	long uptime;
	struct timespec tp;
	unsigned char buff[1024];
	int i, ret;
	int frame_no[MAX_CHN];
	int pre_time[MAX_CHN];
	int pre_day[MAX_CHN];
	time_t cur_time;
	struct tm stm, *ptm;
	
	for(i = 0; i < sizeof(buff); i++)
	{
		buff[i] = i & 0xff;
	}
	
	while(!g_atomic_int_get(&g_record_mng.quit))
	{
		g_mutex_lock(&g_record_mng.mutex);
		clock_gettime(CLOCK_MONOTONIC, &tp);
		cur_time = time(NULL);
		ptm = localtime_r(&cur_time, &stm);
		for(chn = 0; chn < MAX_CHN; chn++)
		{
			// read frame from av pool and write to the disk
			write_info = (RecordWriteInfo *)&g_record_mng.write_info[chn];
			if(write_info->status == 0)
			{
				if(write_info->write_status)
				{
					strg_mng_write_stop(write_info->chn);
					write_info->write_status = 0;
				}
				continue;
			}
			
			uptime = tp.tv_sec;
			if(write_info->write_status == 0)
			{
				if(strg_mng_write_start(write_info->chn, write_info->type) < 0)
					continue;
				write_info->write_status = 1;
				write_info->write_uptime = uptime;
				write_info->write_frame_no = 0;
				write_info->write_frame_num = 0;
				frame_no[write_info->chn] = 0;
				pre_time[write_info->chn] = cur_time;
				pre_day[write_info->chn] = ptm->tm_mday;
			}
			if(uptime - write_info->write_uptime > 1 
				|| cur_time - pre_time[write_info->chn] > 1
				|| cur_time - pre_time[write_info->chn] < 0
				|| pre_day[write_info->chn] != ptm->tm_mday)
			{
				strg_mng_write_stop(write_info->chn);
				write_info->write_status = 0;
				continue;
			}
			
			ret = strg_mng_write(write_info->chn, buff, sizeof(buff), 
					frame_no[write_info->chn], FRAME_TYPE_I);
			if(ret < 0)
			{
				strg_mng_write_stop(write_info->chn);
				write_info->write_status = 0;
				continue;
			}
			else if(ret == 0)
			{
				strg_mng_write_stop(write_info->chn);
				write_info->write_status = 0;
				continue;
			}
			write_info->write_uptime = uptime;
			write_info->write_frame_no = frame_no[write_info->chn]++;
			write_info->write_frame_num++;
			pre_time[write_info->chn] = cur_time;
			pre_day[write_info->chn] = ptm->tm_mday;
		}
		g_mutex_unlock(&g_record_mng.mutex);
		usleep(5000);
	}

	g_mutex_lock(&g_record_mng.mutex);
	for(chn = 0; chn < MAX_CHN; chn++)
	{
		write_info = &g_record_mng.write_info[chn];
		if(write_info->write_status)
		{
			strg_mng_write_stop(chn);
			write_info->write_status = 0;
		}
	}
	g_mutex_unlock(&g_record_mng.mutex);

	return NULL;
}
#endif
int strg_record_start(int chn, int stream, StorageRecordType record_type)
{
	g_return_val_if_fail(chn < MAX_CHN, -1);
	g_return_val_if_fail(chn < MAX_CHN, -1);
	g_return_val_if_fail(stream < 2, -1);

	RecordWriteInfo *write_info = &g_record_mng.write_info[chn];

	if (g_atomic_int_get(&g_format_status))
		return -1;
	
	g_mutex_lock(&g_record_mng.mutex[chn]); 
	if(write_info->enable)
	{
		j_message("The chn%d is busy.", chn);
		g_mutex_unlock(&g_record_mng.mutex[chn]);
		return 0;
	}
	memset(write_info, 0, sizeof(RecordWriteInfo));
	write_info->chn = chn;
	write_info->stream = stream;
	write_info->record_type = record_type;
	write_info->enable = 1;
	g_atomic_int_or(&g_record_chn, (1<<chn));

	g_snprintf(write_info->th_name, sizeof(write_info->th_name), "Record%d", chn);
	write_info->thread = g_thread_new(write_info->th_name, strg_record_thread_func, GINT_TO_POINTER(chn));
	g_mutex_unlock(&g_record_mng.mutex[chn]);
	
	return 0;
}

static void __strg_record_stop(int chn)
{
	g_return_if_fail(chn < MAX_CHN);
	
	RecordWriteInfo *write_info = &g_record_mng.write_info[chn];
	
	g_mutex_lock(&g_record_mng.mutex[chn]);
	j_debug("chn%d enable: %d", chn, write_info->enable);
	if(write_info->enable)
	{
		write_info->enable = 0;
		write_info->quit = 1;
		g_thread_join(write_info->thread);
	}
	g_atomic_int_and(&g_record_chn, ~(1<<chn));
	g_mutex_unlock(&g_record_mng.mutex[chn]);
}

void strg_record_stop(int chn)
{
	if (g_atomic_int_get(&g_format_status))
	{
		g_atomic_int_add(&g_record_chn_bak, ~(1<<chn));
		return;
	}

	__strg_record_stop(chn);
}

int strg_record_set_type(int chn, StorageRecordType type)
{
	g_return_val_if_fail(chn < MAX_CHN, -1);
	
	int ret;
	RecordWriteInfo *write_info = &g_record_mng.write_info[chn];

	if (g_atomic_int_get(&g_format_status))
		return -1;

	g_mutex_lock(&g_record_mng.mutex[chn]);
	//j_debug("chn%d enable: %d", chn, write_info->enable);
	if(write_info->enable && write_info->write_status)
		ret = strg_mng_write_set_type(chn, type);
	else
		ret = -1;
	g_mutex_unlock(&g_record_mng.mutex[chn]);

	return ret;
}

int strg_record_set_overwrite(int chn, int overwrite)
{
	g_return_val_if_fail(chn < MAX_CHN, -1);
	return strg_mng_write_set_overwrite(chn, overwrite);
}

int strg_record_set_minutes(int minutes)
{
	g_atomic_int_set(&g_record_minutes, (minutes>0)?minutes:0);
	return 0;
}

static gint __strg_record_query_sort_cb(gconstpointer p1, gconstpointer p2)
{
	RecordQueryInfo *query_info1, *query_info2;
	query_info1 = (RecordQueryInfo *)p1;
	query_info2 = (RecordQueryInfo *)p2;
	if(query_info1->begin_time < query_info2->begin_time)
		return -1;
	else if(query_info1->begin_time > query_info2->begin_time)
		return 1;
	else
		return 0;
}

static int __strg_record_query_cb(RecordQueryInfo *rqi, gpointer data)
{
	g_assert(rqi != NULL);

	RecordQueryList *query_list = (RecordQueryList *)data;
	int chn = rqi->chn;

	rqi = g_slice_dup(RecordQueryInfo, rqi);
	query_list->list[chn] = g_list_insert_sorted(query_list->list[chn], 
						rqi, __strg_record_query_sort_cb);
	
	return 0;
}

static void __strg_record_query_list_free(gpointer data)
{
	g_slice_free(RecordQueryInfo, data);
}

int strg_record_query(int chn_mask, int shmid, time_t begin_time, time_t end_time, StorageRecordType record_type)
{
	GList *list = NULL;
	RecordQueryList *query_list;
	//RecordQueryInfo *rqi;
	struct shmid_ds shmds;
	gpointer shm;
	int shm_size;
	RecordQueryChnInfo *query_read_info;
	RecordQueryInfo	*query_info;
	int chn;
	int rqi_num = 0;
	int full_flag = 0;

	j_debug("query chn_mask: 0x%x", chn_mask);
	g_return_val_if_fail(chn_mask < (1<<MAX_CHN) && chn_mask != 0, -1);
	if (g_atomic_int_get(&g_format_status))
		return -1;

	if (shmctl(shmid, IPC_STAT, &shmds) < 0) 
	{
		j_warning("Get shm status of query command failed!");
		return -1;
	}	
	shm_size = shmds.shm_segsz;
	shm = shmat(shmid, NULL, 0);
	if (shm == NULL)
	{
		j_warning("Get shm address of query command failed!");
		return -1;
	}
	
	query_read_info = (RecordQueryChnInfo *)shm;
	memset(query_read_info, 0, sizeof(RecordQueryChnInfo));
	query_read_info->max_chn = MAX_CHN;
	query_info = (RecordQueryInfo *)(query_read_info + 1);
	
	query_list = g_malloc0(sizeof(RecordQueryList));
	strg_mng_query(chn_mask, begin_time, end_time, record_type,
						__strg_record_query_cb, query_list); 
	for(chn = 0; chn < MAX_CHN; chn++)
	{
		if(((1 << chn) & chn_mask) == 0)
		{
			g_assert(query_list->list[chn] == NULL);
			continue;
		}
		for(list = g_list_first(query_list->list[chn]); list != NULL && full_flag == 0; list = g_list_next(list))
		{
			//rqi = (RecordQueryInfo *)list->data;
			//j_debug("disk:%d basket:%d bf:%d", rqi->disk_idx, rqi->basket_idx, rqi->bf_idx);
		#ifdef STORAGE_DEBUG
			//display_time(rqi->begin_time, "\t");
			//display_time(rqi->end_time, "\t");
		#endif
			if(sizeof(RecordQueryChnInfo) + sizeof(RecordQueryInfo) * (rqi_num + 1) > shm_size)
			{
				full_flag = 1;
				break;
			}
			memcpy(&query_info[rqi_num++], list->data, sizeof(RecordQueryInfo));
			query_read_info->query_info_num[chn]++;
		}
		g_list_free_full(query_list->list[chn], __strg_record_query_list_free);
	}
	g_free(query_list);
	shmdt(shm);
	
	return rqi_num;
}

int strg_record_find(int chn_mask, time_t begin_time, time_t end_time, StorageRecordType record_type)
{
	
	g_return_val_if_fail(chn_mask < (1<<MAX_CHN) && chn_mask != 0, -1);
	if (g_atomic_int_get(&g_format_status))
		return -1;
	return strg_mng_find(chn_mask, begin_time, end_time, record_type);
}

int strg_record_read_start(int shmid, RecordQueryInfo *query_info, StorageFragmentInfo *sfi, long *handle)
{
	gpointer shm;
	struct shmid_ds shmds;
	ReadHandle read_handle;
	RecordReadInfo *read_info;

	j_debug("chn:%d disk_idx:%d basket_idx:%d bf_idx:%d", 
			query_info->chn, query_info->disk_idx, 
			query_info->basket_idx, query_info->bf_idx);
	if(shmctl(shmid, IPC_STAT, &shmds) < 0)
	{
		j_warning("Get shm status failed.");
		return -1;
	}
	shm = shmat(shmid, NULL, 0);
	if(shm == NULL)
	{
		j_warning("shmat for read failed.");
		return -1;
	}
	read_handle = strg_mng_read_start(query_info, sfi);
	if(read_handle == NULL)
	{
		j_warning("Read start failed");
		shmdt(shm);
		return -1;
	}
	
	read_info = g_malloc0(sizeof(RecordReadInfo));
	read_info->shm = shm;
	read_info->shmid = shmid;
	read_info->shm_size = shmds.shm_segsz;
	read_info->read_handle = read_handle;
	read_info->data = NULL;
	g_record_mng.read_list = g_list_append(g_record_mng.read_list, read_info);
	
	*handle = (long)read_info;
	
	return 0;
}

int strg_record_read_stop(long handle)
{
	RecordReadInfo *read_info = (RecordReadInfo *)handle;
	g_return_val_if_fail(read_info != NULL, -1);
	
	strg_mng_read_stop(read_info->read_handle);
	shmdt(read_info->shm);
	g_record_mng.read_list = g_list_remove(g_record_mng.read_list, read_info);
	g_free(read_info);
	
	return 0;
}

int strg_record_read_set(long handle, time_t t)
{
	RecordReadInfo *read_info = (RecordReadInfo *)handle;
	g_return_val_if_fail(read_info != NULL, -1);
	
	return strg_mng_read_set(read_info->read_handle, t);
}

int strg_record_read_seek(long handle, int seek_iframe_offset)
{
	RecordReadInfo *read_info = (RecordReadInfo *)handle;
	g_return_val_if_fail(read_info != NULL, -1);
	
	return strg_mng_read_seek(read_info->read_handle, seek_iframe_offset);
}

int strg_record_read(long handle, StorageReadCmd cmd)
{
	RecordReadInfo *read_info = (RecordReadInfo *)handle;
	g_return_val_if_fail(read_info != NULL, -1);

	if (g_atomic_int_get(&g_format_status))
		return -1;

	return strg_mng_read(read_info->read_handle, cmd, 
							read_info->shm, read_info->shm_size);
}

static gpointer __strg_record_format_disk_func(gpointer data)
{	
	int chn, ret;
	int record_chn;
	int disk_idx = GPOINTER_TO_INT(data)&0xffff;
	int check_bad = GPOINTER_TO_INT(data)>>16;
	RecordWriteInfo *write_info;
	GList *list;
	
	// 关闭所有录像
	record_chn = g_atomic_int_get(&g_record_chn);
	for (chn = 0; chn < MAX_CHN; chn++)
	{
		//if((1 << chn) & record_chn)
			__strg_record_stop(chn);
	}
	// 关闭所有读取录像
	for(list = g_list_first(g_record_mng.read_list); list != NULL; list = g_list_next(list))
	{
		strg_record_read_stop((GPOINTER_TO_INT(list->data)));
	}
	g_list_free(g_record_mng.read_list);
	g_record_mng.read_list = NULL;
	
	ret = strg_mng_format_disk_start(disk_idx, check_bad);
	if(ret < 0)
	{
		g_atomic_int_set(&g_format_status, 0);
		for (chn = 0; chn < MAX_CHN; chn++)
		{
			write_info = &g_record_mng.write_info[chn];
			if((1 << chn) & record_chn)
				strg_record_start(chn, write_info->stream, write_info->record_type);
		}
	}
	
	return GINT_TO_POINTER(ret);
}

int strg_record_format_disk_start(int disk_idx, int check_bad)
{
	if (!g_atomic_int_compare_and_exchange(&g_format_status, 0, 1))
	{
		j_warning("The format is busy!");
		return -1;
	}
	// 创建线程释放硬盘资源，
	// 创建线程的原因主要不想阻塞这个接口时间太长
	g_atomic_int_set(&g_record_chn_bak, g_atomic_int_get(&g_record_chn));
	g_format_thread = g_thread_new("RecordFormat", __strg_record_format_disk_func, 
								GINT_TO_POINTER((disk_idx&0xffff) | (!!check_bad<<16)));
	return 0;
}

void strg_record_format_disk_stop(int disk_idx)
{
	int chn;
	RecordWriteInfo *write_info;

	if (!g_atomic_int_get(&g_format_status))
		return;
		
	if(g_format_thread)
	{
		int ret;
		ret = GPOINTER_TO_INT(g_thread_join(g_format_thread));
		g_format_thread = NULL;
		if(ret < 0)
		{
			j_warning("Record format failed.");
			g_atomic_int_set(&g_format_status, 0);
			return;
		}
	}
	strg_mng_format_disk_stop(disk_idx);
	g_atomic_int_set(&g_format_status, 0);
	for (chn = 0; chn < MAX_CHN; chn++)
	{
		if (g_atomic_int_get(&g_record_chn_bak) & (1<<chn))
		{
			write_info = &g_record_mng.write_info[chn];
			strg_record_start(write_info->chn, write_info->stream, 
							  write_info->record_type);
		}
	}
}

int strg_record_format_get_percent(int *percent)
{
	if (!g_atomic_int_get(&g_format_status))
		return -1;
	if(g_format_thread)
	{
		int ret;
		ret = GPOINTER_TO_INT(g_thread_join(g_format_thread));
		g_format_thread = NULL;
		if(ret < 0)
		{
			j_warning("Record format failed.");
			g_atomic_int_set(&g_format_status, 0);
			return -1;
		}
	}
	return strg_mng_format_get_percent(percent);
}

int strg_record_get_disk_info(StorageDisksInfo *disks_info)
{
	g_return_val_if_fail(disks_info != NULL, -1);
	return strg_mng_get_disk_info(disks_info);
}

int strg_record_get_record_info(StorageRecordInfo *record_info)
{
	g_return_val_if_fail(record_info != NULL, -1);
	
	int chn;
	int mask = 0;
	RecordWriteInfo *write_info = g_record_mng.write_info;
	for(chn = 0; chn < MAX_CHN; chn++)
	{
		if(write_info[chn].write_status)
			mask |= (1<<chn);
	}
	record_info->record_mask = mask;
	record_info->format_status = g_atomic_int_get(&g_format_status);
	return 0;
}

void strg_record_display_info()
{
	int i;
	int record_chn;
	record_chn = g_atomic_int_get(&g_record_chn);
	for(i = 0; i < MAX_CHN; i++)
	{
		if(record_chn & (1<<i))
			printf("chn%d recording\n", i);
		else
			printf("chn%d idle\n", i);
	}
	strg_mng_display_info();
}

static int __strg_record_save_jpeg(int chn, int stream, gpointer jpeg_buff, int jpeg_len, unsigned long long pts)
{
	if(strg_mng_write_start(chn, RECORD_JPEG_TYPE) < 0)
		return -1;
	if(strg_mng_write(chn, pts, jpeg_buff, jpeg_len, 0, FRAME_TYPE_I, ENCODE_TYPE_JPEG) < 0)
	{
		strg_mng_write_stop(chn);
		return -1;
	}
	strg_mng_write_stop(chn);
	return 0;
}

int strg_record_save_jpeg(int chn, int stream, int shmid, int jpeg_len, unsigned long long pts)
{
	gpointer shm;
	gpointer jpeg_buff;
	struct shmid_ds shmds;
	AV_FRAME_HEAD_S hdr = {0};
	
	hdr.u16FrameFlag = 0x3448;
	hdr.u8FrameType = AV_VIDEO_FRAME_I;
	hdr.u32FrameNo = 0;
	hdr.u32Time = pts / 1000;
	hdr.u32FrameSize = jpeg_len;
	hdr.u64TimeStamp = pts * 1000;
	hdr.unVidoeInfo.u16ImageWidth = 0;
	hdr.unVidoeInfo.u16ImageHeight = 0;
	hdr.unVidoeInfo.u8EncodeType = 2;
	hdr.unVidoeInfo.u8FrameRate = 1;
	hdr.unVidoeInfo.u8Pal = 1;
	
	if(shmctl(shmid, IPC_STAT, &shmds) < 0)
	{
		j_warning("Get shm status failed.");
		return -1;
	}
	shm = shmat(shmid, NULL, 0);
	if(shm == NULL)
	{
		j_warning("shmat for read failed.");
		return -1;
	}
	jpeg_buff = g_malloc(jpeg_len + AV_FRAME_HEAD_LEN);
	memcpy(jpeg_buff, &hdr, AV_FRAME_HEAD_LEN);
	memcpy(jpeg_buff+AV_FRAME_HEAD_LEN, shm, jpeg_len);
	if(__strg_record_save_jpeg(chn, stream, jpeg_buff, jpeg_len+AV_FRAME_HEAD_LEN, pts) < 0)
	{
		j_warning("Save jpeg failed.");
		shmdt(shm);
		return -1;
	}
	shmdt(shm);
	g_free(jpeg_buff);
	return 0;
}

int strg_record_save_jpeg_by_path(int chn, int stream, const char *path, unsigned long long pts)
{
	FILE *fp;
	int ret, file_len;
	gpointer jpeg_buff;
	AV_FRAME_HEAD_S hdr = {0};
	
	g_return_val_if_fail(path != NULL, -1);

	
	fp = fopen(path, "rb");
	if(fp == NULL)
	{
		j_warning("Open jpeg path %s failed.\n", path);
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	
	hdr.u16FrameFlag = 0x3448;
	hdr.u8FrameType = AV_VIDEO_FRAME_I;
	hdr.u32FrameNo = 0;
	hdr.u32Time = pts / 1000;
	hdr.u32FrameSize = file_len;
	hdr.u64TimeStamp = pts * 1000;
	hdr.unVidoeInfo.u16ImageWidth = 0;
	hdr.unVidoeInfo.u16ImageHeight = 0;
	hdr.unVidoeInfo.u8EncodeType = 2;
	hdr.unVidoeInfo.u8FrameRate = 1;
	hdr.unVidoeInfo.u8Pal = 1;
	
	jpeg_buff = g_malloc(file_len + AV_FRAME_HEAD_LEN);
	memcpy(jpeg_buff, &hdr, AV_FRAME_HEAD_LEN);
	
	fseek(fp, 0, SEEK_SET);
	if(fread(jpeg_buff+AV_FRAME_HEAD_LEN, file_len, 1, fp) != 1)
	{
		j_warning("Open jpeg file %s failed.\n", path);
		g_free(jpeg_buff);//g_free(fp); 
		fclose(fp);
		return -1;
	}
	fclose(fp);
	
	ret = __strg_record_save_jpeg(chn, stream, jpeg_buff, file_len+AV_FRAME_HEAD_LEN, pts);
	g_free(jpeg_buff);

	return ret;
}

