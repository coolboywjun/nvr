
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <syslog.h>
#include <pthread.h>

#include <storage_interface.h>

#include <net_api.h>

#define PRINT(fmt, args...)	\
do{		\
	printf(fmt, ##args);		\
	syslog(LOG_ERR | LOG_USER, "Storage " fmt, ##args);	\
}while(0);

static int sock = 0;
static pthread_mutex_t mutex;

typedef struct StorageLibReadInfo
{
	long handle;
	int shmid;
	void *shm_addr;
}StorageLibReadInfo;

static int storage_lib_send_and_recv_packet(StorageSvrNetHdr *pHdr, void *sendBuf, void *recvBuf)
{
	pthread_mutex_lock(&mutex);
	if(sock <= 0)
	{
		sock = net_local_tcp_connect(STORAGE_SERVER_NAME);
		if (sock < 0)
		{
			//PRINT("Failed to connect to %s\n", STORAGE_SERVER_NAME);
			pthread_mutex_unlock(&mutex);
			return -1;
		}
	}
	if (net_local_tcp_send(sock, pHdr, sizeof(StorageSvrNetHdr)) < 0)
	{
		net_close_socket(&sock);
		pthread_mutex_unlock(&mutex);
		return -1;
	}
	if(pHdr->ext_data_len > 0 && sendBuf != NULL)
	{
		if (net_local_tcp_send(sock, sendBuf, pHdr->ext_data_len) < 0)
		{
			net_close_socket(&sock);
			pthread_mutex_unlock(&mutex);
			return -1;
		}
	}
	if (net_local_tcp_recv(sock, pHdr, sizeof(StorageSvrNetHdr)) < 0)
	{
		net_close_socket(&sock);
		pthread_mutex_unlock(&mutex);
		return -1;
	}
	if(pHdr->ext_data_len > 0 && recvBuf != NULL)
	{
		if(net_local_tcp_recv(sock, recvBuf, pHdr->ext_data_len) < 0)
		{
			net_close_socket(&sock);
			pthread_mutex_unlock(&mutex);
			return -1;
		}
	}
	pthread_mutex_unlock(&mutex);
	
	return 0;
}

int storage_lib_record_start(int chn, int stream, StorageRecordType record_type)
{
	StorageSvrNetHdr hdr;
	
	memset(&hdr, 0, sizeof(hdr));
	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_RECORD_START;
	hdr.chn = chn;
	hdr.stream = stream;
	hdr.param[0] = record_type;
	hdr.ext_data_len = 0;
	if(storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		//PRINT("Send record start command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the record start response command len error!!!");
	}
	return hdr.result; 
}

int storage_lib_record_stop(int chn)
{
	StorageSvrNetHdr hdr;
	
	memset(&hdr, 0, sizeof(hdr));
	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_RECORD_STOP;
	hdr.chn = chn;
	hdr.stream = 0;
	hdr.ext_data_len = 0;
	if (storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		PRINT("Send record stop command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the record stop response command len error!!!\n");
	}
	return hdr.result;
}

int storage_lib_record_set_type(int chn, StorageRecordType type)
{
	StorageSvrNetHdr hdr;

	memset(&hdr, 0, sizeof(hdr));
	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_RECORD_SET_TYPE;
	hdr.chn = chn;
	hdr.param[0] = type;
	hdr.ext_data_len = 0;
	if (storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		PRINT("Send record set type command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the read set response command len error!!!\n");
	}
	
	return hdr.result;
}

int storage_lib_record_set_overwrite(int chn, int overwrite)
{
	StorageSvrNetHdr hdr;

	memset(&hdr, 0, sizeof(hdr));
	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_RECORD_SET_OVERWRITE;
	hdr.chn = chn;
	hdr.param[0] = overwrite;
	hdr.ext_data_len = 0;
	if (storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		PRINT("Send record set overwrite command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the read set response command len error!!!\n");
	}
	
	return hdr.result;
}

int storage_lib_record_set_minutes(int minutes)
{	
	StorageSvrNetHdr hdr;

	memset(&hdr, 0, sizeof(hdr));
	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_RECORD_SET_MINUTES;
	hdr.chn = 0;	// 无意义
	hdr.param[0] = minutes;
	hdr.ext_data_len = 0;
	if (storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		PRINT("Send record set overwrite command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the read set response command len error!!!\n");
	}
	
	return hdr.result;
}

int storage_lib_record_query(int chn_mask, time_t begin_time, time_t end_time, 
							StorageRecordType	 record_type, int *shmid, 
							RecordQueryChnInfo **chn_info, RecordQueryInfo **query_info)
{
	StorageSvrNetHdr hdr = {0};
	int id;
	void *shm_addr;

	if(chn_mask == 0)
		return -1;
	id = shmget(IPC_PRIVATE, 0x100000, IPC_CREAT|0600);
	if (id < 0)
	{
		PRINT("Create query share memory failed!\n");
		return -1;
	}
	shm_addr = shmat(id, NULL, 0);
	if (shm_addr == NULL)
	{
		PRINT("Map query share memory failed!\n");
		shmctl(id, IPC_RMID, NULL) ;
		return -1;
	}

	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_RECORD_QUERY;
	hdr.chn = chn_mask;
	hdr.stream = 0;
	hdr.param[0] = id;
	hdr.param[1] = begin_time;
	hdr.param[2] = end_time;
	hdr.param[3] = record_type;
	hdr.ext_data_len = 0;

	if (storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		PRINT("Send record query command failed!\n");
		shmctl(id, IPC_RMID, NULL) ;
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the record query response command len error!!!\n");
	}
	*shmid = id;
	*chn_info = shm_addr;
	*query_info = shm_addr + sizeof(RecordQueryChnInfo);
	
	return hdr.result; 
}

void storage_lib_record_query_free(int shmid, RecordQueryChnInfo *chn_info, RecordQueryInfo *query_info)
{
	void *shm_addr = chn_info;
	
	shmdt(shm_addr);
	shmctl(shmid, IPC_RMID, NULL);
}
#if 0
int storage_lib_record_find(int chn_mask, int year, int mon, StorageRecordType record_type)
{
	time_t begin_time, end_time;
	struct tm time_str = {0};
	struct tm *ptm;
	int shmid;
	RecordQueryChnInfo *chn_info;
	RecordQueryInfo *query_info;
	int mask = 0;
	int i, num;
	
	time_str.tm_year = year - 1900;
	time_str.tm_mon = mon - 1;
	time_str.tm_mday = 1;
	begin_time = mktime(&time_str);
	if(mon == 12)
	{
		year += 1;
		mon = 1;
	}
	else
		mon += 1;
	time_str.tm_year = year - 1900;
	time_str.tm_mon = mon - 1;
	time_str.tm_mday = 1;
	end_time = mktime(&time_str) - 1;
	
	if(begin_time < 0 || end_time < 0)
	{
		PRINT("The year %d or the mon %d is wrong.\n", year, mon);
		return -1;
	}
	if((num = storage_lib_record_query(chn_mask, begin_time, end_time, 
						record_type, &shmid, &chn_info, &query_info)) < 0)
	{
		PRINT("Record query failed.\n");
		return -1;
	}
	for(i = 0; i < num; i++)
	{
		ptm = localtime_r(&query_info[i].begin_time, &time_str);
		mask |= 1<<(ptm->tm_mday - 1);
	}
	storage_lib_record_query_free(shmid, chn_info, query_info);
	return mask;
}
#else

int storage_lib_record_find(int chn_mask, int year, int mon, StorageRecordType record_type)
{
	StorageSvrNetHdr hdr = {0};
	time_t begin_time, end_time;
	struct tm time_str = {0};
	
	time_str.tm_year = year - 1900;
	time_str.tm_mon = mon - 1;
	time_str.tm_mday = 1;
	begin_time = mktime(&time_str);
	if(mon == 12)
	{
		year += 1;
		mon = 1;
	}
	else
		mon += 1;
	time_str.tm_year = year - 1900;
	time_str.tm_mon = mon - 1;
	time_str.tm_mday = 1;
	end_time = mktime(&time_str) - 1;
	
	if(begin_time < 0 || end_time < 0)
	{
		PRINT("The year %d or the mon %d is wrong.\n", year, mon);
		return -1;
	}

	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_RECORD_FIND;
	hdr.chn = chn_mask;
	hdr.stream = 0;
	hdr.param[0] = begin_time;
	hdr.param[1] = end_time;
	hdr.param[2] = record_type;
	hdr.param[3] = 0;
	hdr.ext_data_len = 0;

	if (storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		PRINT("Send record query command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the record query response command len error!!!\n");
	}
	
	return hdr.result; 
}

#endif
int storage_lib_read_start(RecordQueryInfo *query_info, StorageFragmentInfo *fragment_info, long *handle)
{
	int shmid;
	void *shm_addr;
	unsigned char buff[1024], recv_buff[256];
	StorageSvrNetHdr *hdr = (StorageSvrNetHdr*)buff;
	StorageReadStartCmd *cmd = (StorageReadStartCmd*)(hdr + 1);
	StorageLibReadInfo *read_info;

	if(query_info == NULL)
		return -1;
	shmid = shmget(IPC_PRIVATE, 0x100000, IPC_CREAT | 0600); 
	if (shmid < 0)
	{
		PRINT("Create query share memory failed!\n");
		return -1;
	}
	shm_addr = shmat(shmid, NULL, 0);
	if (shm_addr == NULL)
	{
		PRINT("Map query share memory failed!\n");
		shmctl(shmid, IPC_RMID, NULL) ;
		return -1;
	}

	memset(hdr, 0, sizeof(StorageSvrNetHdr));
	memset(cmd, 0, sizeof(StorageReadStartCmd));
	hdr->magic = STORAGE_SVR_MAGIC;
	hdr->cmd = STORAGE_READ_START;
	hdr->ext_data_len = sizeof(StorageReadStartCmd);
	cmd->shmid = shmid;
	//cmd->query_info.chn = query_info->chn;
	//cmd->query_info.disk_idx = query_info->disk_idx;
	//cmd->query_info.basket_idx = query_info->basket_idx;
	//cmd->query_info.bf_idx = query_info->bf_idx;
	//cmd->query_info.begin_iframe_idx = query_info->begin_iframe_idx;
	//cmd->query_info.iframe_num = query_info->iframe_num;
	//cmd->query_info.fragment_size = query_info->fragment_size;
	memcpy(&cmd->query_info, query_info, sizeof(cmd->query_info));
	
	if (storage_lib_send_and_recv_packet(hdr, cmd, recv_buff) < 0)
	{
		PRINT("Send read start command failed!\n");
		return -1;
	}
	
	if (hdr->ext_data_len != sizeof(StorageFragmentInfo))
	{
		PRINT("WARNING: the read start response command len error!!!\n");
	}
	
	read_info = malloc(sizeof(StorageLibReadInfo));
	if (read_info == NULL)
	{
		PRINT("Malloc for StorageLibReadInfo failed!\n");
		return -1;
	}
	read_info->handle = hdr->param[0];
	read_info->shmid = shmid;
	read_info->shm_addr = shm_addr;
	*handle = (long)read_info;	
	if(fragment_info)
		memcpy(fragment_info, recv_buff, sizeof(StorageFragmentInfo));
	
	return hdr->result;
}

int storage_lib_read_stop(long handle)
{
	StorageSvrNetHdr hdr;
	StorageLibReadInfo *read_info = (StorageLibReadInfo*)handle;

	memset(&hdr, 0, sizeof(hdr));
	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_READ_STOP;
	hdr.param[0] = read_info->handle;
	hdr.ext_data_len = 0;
	if (storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		PRINT("Send record read stop command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the read stop response command len error!!!\n");
	}
	shmdt(read_info->shm_addr);
	shmctl(read_info->shmid, IPC_RMID, NULL);
	free(read_info);
	
	return hdr.result;
}

int storage_lib_read_set(long handle, time_t t)
{
	StorageSvrNetHdr hdr;
	StorageLibReadInfo *read_info = (StorageLibReadInfo*)handle;

	memset(&hdr, 0, sizeof(hdr));
	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_READ_SET;
	hdr.param[0] = read_info->handle;
	hdr.param[1] = t;
	hdr.ext_data_len = 0;
	if (storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		PRINT("Send record read set command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the read set response command len error!!!\n");
	}
	
	return hdr.result;
}

int storage_lib_read_seek(long handle, int seek_iframe_offset)
{
	StorageSvrNetHdr hdr;
	StorageLibReadInfo *read_info = (StorageLibReadInfo*)handle;

	memset(&hdr, 0, sizeof(hdr));
	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_READ_SEEK;
	hdr.param[0] = read_info->handle;
	hdr.param[1] = seek_iframe_offset;
	hdr.ext_data_len = 0;
	if (storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		PRINT("Send record read set command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the read set response command len error!!!\n");
	}
	
	return hdr.result;
}

int storage_lib_read_frame(long handle, StorageReadCmd cmd, StorageFrameHdr *frameHdr, unsigned char **frame_buff)
{
	StorageSvrNetHdr hdr;
	StorageLibReadInfo *read_info = (StorageLibReadInfo*)handle;

	memset(&hdr, 0, sizeof(hdr));
	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_READ_FRAME;
	hdr.param[0] = read_info->handle;
	hdr.param[1] = cmd;
	hdr.ext_data_len = 0;

	if (storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		PRINT("Send read frame command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the read stopx response command len error!!!\n");
	}
	if(frameHdr != NULL)
		memcpy(frameHdr, (unsigned char *)read_info->shm_addr, sizeof(StorageFrameHdr));
	*frame_buff = (unsigned char *)read_info->shm_addr + sizeof(StorageFrameHdr);
	
	return hdr.result;
}

/*
* dev_path: 如/dev/sda, /dev/sdb,etc
*/
int storage_lib_format_disk_start(int disk_idx, int check_bad)
{
	StorageSvrNetHdr hdr = {0};

	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_FORMAT_DISK_START;
	hdr.param[0] = disk_idx;
	hdr.param[1] = check_bad;
	hdr.ext_data_len = 0;
	
	if (storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		PRINT("Send format disk command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the record query response command len error!!!\n");
	}

	return hdr.result; 
}

int storage_lib_format_disk_stop(int disk_idx)
{
	StorageSvrNetHdr hdr = {0};
	
	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_FORMAT_DISK_STOP;
	hdr.param[0] = disk_idx;
	hdr.ext_data_len = 0;
	
	if (storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		PRINT("Send format end command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the read format disk response command len error!!!\n");
	}
	return hdr.result;
}

int storage_lib_format_get_percent(int *percent)
{
	StorageSvrNetHdr hdr;

	memset(&hdr, 0, sizeof(hdr));
	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_FORMAT_PERCENT;
	hdr.ext_data_len = 0;

	if (storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		PRINT("Send get format percent command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the read get format percent response command len error!!!\n");
	}
	*percent = hdr.param[0];
	return hdr.result;
}

int storage_lib_get_disk_info(StorageDisksInfo *disks_info)
{
	StorageSvrNetHdr hdr;

	if(disks_info == NULL)
		return -1;
	memset(disks_info, 0, sizeof(StorageDiskInfo));
	memset(&hdr, 0, sizeof(hdr));
	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_GET_DISK_INFO;
	hdr.ext_data_len = 0;

	if (storage_lib_send_and_recv_packet(&hdr, NULL, disks_info) < 0)
	{
		//PRINT("Send get disk info command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != sizeof(StorageDisksInfo))
	{
		PRINT("WARNING: the read get disk info command len error!!!\n");
	}
	return hdr.result;
}

int storage_lib_get_record_info(StorageRecordInfo *record_info)
{
	StorageSvrNetHdr hdr;

	if(record_info == NULL)
		return -1;
	memset(record_info, 0, sizeof(StorageRecordInfo));
	memset(&hdr, 0, sizeof(hdr));
	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_GET_RECORD_INFO;
	hdr.ext_data_len = 0;

	if (storage_lib_send_and_recv_packet(&hdr, NULL, record_info) < 0)
	{
		//PRINT("Send get disk info command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != sizeof(StorageRecordInfo))
	{
		PRINT("WARNING: the read get record info command len error!!!\n");
	}
	return hdr.result;
}

int storage_lib_display_info()
{
	StorageSvrNetHdr hdr;

	memset(&hdr, 0, sizeof(hdr));
	hdr.magic = STORAGE_SVR_MAGIC;
	hdr.cmd = STORAGE_DISPLAY_INFO;
	hdr.ext_data_len = 0;

	if (storage_lib_send_and_recv_packet(&hdr, NULL, NULL) < 0)
	{
		PRINT("Send display infomation command failed!\n");
		return -1;
	}
	if (hdr.ext_data_len != 0)
	{
		PRINT("WARNING: the read display infomation response command len error!!!\n");
	}

	return hdr.result;
}

int storage_lib_save_jpeg(int chn, int stream, const char *jpeg_buff, int jpeg_len, unsigned long long  pts)
{
	int shmid;
	void *shm_addr;
	unsigned char buff[1024];
	StorageSvrNetHdr *hdr = (StorageSvrNetHdr*)buff;

	shmid = shmget(IPC_PRIVATE, 0x200000, IPC_CREAT | 0600); 
	if (shmid < 0)
	{
		PRINT("Create query share memory failed!\n");
		return -1;
	}
	shm_addr = shmat(shmid, NULL, 0);
	if (shm_addr == NULL)
	{
		PRINT("Map query share memory failed!\n");
		shmctl(shmid, IPC_RMID, NULL) ;
		return -1;
	}
	memcpy(shm_addr, jpeg_buff, jpeg_len);
	
	memset(&hdr, 0, sizeof(hdr));
	hdr->magic = STORAGE_SVR_MAGIC;
	hdr->cmd = STORAGE_SAVE_JPEG;

	hdr->chn = chn;
	hdr->stream = stream;
	
	hdr->param[0] = shmid;
	hdr->param[1] = jpeg_len;
	hdr->param[2] = 0;
	hdr->param[3] = 0;
	hdr->ext_data_len = sizeof(unsigned long long);
	memcpy(hdr+1, &pts, sizeof(unsigned long long));
	
	if (storage_lib_send_and_recv_packet(hdr, NULL, NULL) < 0)
	{
		PRINT("Send save jpeg command failed!\n");
		shmdt(shm_addr);
		shmctl(shmid, IPC_RMID, NULL);
		return -1;
	}
	if (hdr->ext_data_len != 0)
	{
		PRINT("WARNING: the read save jpeg response command len error!!!\n");
	}
	shmdt(shm_addr);
	shmctl(shmid, IPC_RMID, NULL);

	return hdr->result;
}

int storage_lib_save_jpeg_by_path(int chn, int stream, const char *jpeg_path, unsigned long long pts)
{
	unsigned char buff[1024];
	StorageSvrNetHdr *hdr = (StorageSvrNetHdr*)buff;
	char *ext_data = (char *)(hdr + 1);
	int ext_len = sizeof(unsigned long long) + 256;
	
	memset(hdr, 0, sizeof(StorageSvrNetHdr));
	hdr->magic = STORAGE_SVR_MAGIC;
	hdr->cmd = STORAGE_SAVE_JPEG_BY_PATH;
	
	hdr->chn = chn;
	hdr->stream = stream;

	hdr->ext_data_len = ext_len;
	memcpy(ext_data, &pts, sizeof(unsigned long long));
	strncpy(ext_data+sizeof(unsigned long long), jpeg_path, 256 - 1);
	ext_data[256-1] = '\0';
	
	if (storage_lib_send_and_recv_packet(hdr, ext_data, NULL) < 0)
	{
		PRINT("Send save jpeg by path command failed!\n");
		return -1;
	}
	if (hdr->ext_data_len != 0)
	{
		PRINT("WARNING: the read display infomation response command len error!!!\n");
	}

	return hdr->result;
}

int storage_lib_init()
{
	sock = net_local_tcp_connect(STORAGE_SERVER_NAME);
	pthread_mutex_init(&mutex, NULL);

	return 0;
}

void storage_lib_uninit()
{
	if(sock > 0)
		net_close_socket(&sock);
	pthread_mutex_destroy(&mutex);
}


