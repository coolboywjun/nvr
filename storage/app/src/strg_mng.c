/*
* 使用说明： 
* 1，在格式化硬盘前，请先关闭录像,查询录像和回放录像，不然会有冲突
*/
#include <strg_header.h>

#include <strg_disk.h>
#include <strg_mng.h>
#include <strg_format.h>
#include <strg_record.h>


typedef struct StorageMng
{
	int disk_num;
	int open_num;
	
	gchar	disk_path[MAX_DISK_NUM][PATH_LEN];
	DiskMng *dm[MAX_DISK_NUM];

	int		chn_write_disk_idx[MAX_CHN];

	gint	overwrite;
}StorageMng;
static StorageMng	g_strg_mng;

G_LOCK_DEFINE_STATIC(mng_lock);
#define MNG_LOCK()		G_LOCK(mng_lock)
#define MNG_UNLOCK()	G_UNLOCK(mng_lock);

static int strg_mng_scan_disk_dev();
static gboolean strg_mng_format_timeout(gpointer user_data);
static int strg_mng_is_default_filesystem();

int strg_mng_init()
{
	int i; 
	
	memset(&g_strg_mng, 0, sizeof(g_strg_mng));
	for(i = 0; i < MAX_CHN; i++)
	{
		g_strg_mng.chn_write_disk_idx[i] = -1;
		g_strg_mng.overwrite |= (1<<i);
	}
	
	if(strg_mng_scan_disk_dev() < 0)
	{
		j_warning("Scan all disk failed.");
		return -1;
	}

	for(i = 0; i < g_strg_mng.disk_num; i++)
	{
		if(strg_disk_is_our_format(g_strg_mng.disk_path[i]) < 0)
		{
			j_warning("Disk %s is not our fotmat, we force to format it.", g_strg_mng.disk_path[i]);
			if(!strg_mng_is_default_filesystem())
				g_timeout_add_seconds(3, strg_mng_format_timeout, GINT_TO_POINTER(i));
		}
		else
		{
			if((g_strg_mng.dm[i] = strg_disk_open(g_strg_mng.disk_path[i], i)) != NULL)
			{
				g_strg_mng.open_num++;
				j_message("Open disk %s OK!", g_strg_mng.disk_path[i]);
			}
			else
			{
				j_warning("Open disk %s failed, the storage date is destroyed!!!", g_strg_mng.disk_path[i]);
				continue;
			}
		}
	}

	j_message("Storage manager is init OK!");

	return 0;
}

void strg_mng_uninit()
{
	int i;
	for (i = 0; i < MAX_DISK_NUM; i++) 
	{
		if(g_strg_mng.dm[i] != NULL)
			strg_disk_close(g_strg_mng.dm[i]);
	}
}

static gboolean strg_mng_format_timeout(gpointer user_data)
{
	static int curStatus = 0;	// 未开始格式化
	int disk_no = GPOINTER_TO_INT(user_data);
	int percent;
	
	switch(curStatus)
	{
		case 0:	// 未开始格式化
			if(strg_record_format_disk_start(disk_no, 0) < 0)
				return G_SOURCE_REMOVE;
			curStatus = 1;
			break;
		case 1:	// 正在格式化中
			if(strg_record_format_get_percent(&percent) < 0)
			{
				curStatus = 0;
				strg_record_format_disk_stop(disk_no);
				return G_SOURCE_REMOVE;
			}
			if(percent  < 0 || percent >= 100)
			{
				curStatus = 0;
				strg_record_format_disk_stop(disk_no);
				return G_SOURCE_REMOVE;
			}
			break;
		default:
			g_assert_not_reached();
			break;
	}
	return G_SOURCE_CONTINUE;
}

static int strg_mng_is_disk_dev(const char *dev_path)
{
	struct stat statbuf;
	int major, minor;
	char buf[512];
	char block_path[256];
	
	if(stat(dev_path, &statbuf) < 0)
		return 0;
	major = (statbuf.st_rdev >> 8) & 0xff;
	minor = statbuf.st_rdev & 0xff;
	//major = MAJOR(statbuf.st_rdev);
	//minor = MINOR(statbuf.st_rdev);
	snprintf(block_path, sizeof(block_path), "/sys/dev/block/%d:%d", major, minor);
	memset(buf, 0, sizeof(buf));
	if(readlink(block_path, buf, sizeof(buf)) < 0)
		return 0;
	if(strstr(buf, "ata"))
		return 1;
	return 0;
}

static int strg_mng_is_default_filesystem()
{
	FILE *fp;
	char cmdline[1024];
	int isDefault = 0;

	fp = fopen("/proc/cmdline", "r");
	if(fp != NULL)
	{
		fgets(cmdline, sizeof(cmdline), fp);
		if(strstr(cmdline, "default"))
			isDefault = 1;
		fclose(fp);
	}
	return isDefault;
}

#if 0
// 挂载sda1, 使用分区
static int strg_mng_scan_disk_dev()
{
	char c;
	gchar dev_name[PATH_LEN];
	gchar dev_name1[PATH_LEN];
	gchar *p_name;

////////////////////////////////////////////////
// 另外一种获取系统硬盘信息的方式是通过读取/proc/partitions
////////////////////////////////////////////////

	g_strg_mng.disk_num = 0;	
	for(c = 'a'; c <= 'z' && g_strg_mng.disk_num < MAX_DISK_NUM; c++)
	{
		g_snprintf(dev_name, PATH_LEN, "/dev/sd%c", c);
		g_snprintf(dev_name1, PATH_LEN, "/dev/sd%c1", c);
		if(access(dev_name1, F_OK) == 0)
			p_name = dev_name1;
		else if(access(dev_name, F_OK) == 0)
			p_name = dev_name;
		else
			continue;
		if(!strg_mng_is_disk_dev(p_name))
			continue;
		g_stpcpy(g_strg_mng.disk_path[g_strg_mng.disk_num], p_name);	
		j_debug("disk[%d] = %s\n", g_strg_mng.disk_num, g_strg_mng.disk_path[g_strg_mng.disk_num]);
	
		g_strg_mng.disk_num++;
	}

	return g_strg_mng.disk_num;
}

#else
// 挂载sda，使用分区
static int strg_mng_scan_disk_dev()
{
	char c;
	gchar dev_name[PATH_LEN];

////////////////////////////////////////////////
// 另外一种获取系统硬盘信息的方式是通过读取/proc/partitions
////////////////////////////////////////////////
#ifdef ARCH_ARM
	g_strg_mng.disk_num = 0;	
	for(c = 'a'; c <= 'z' && g_strg_mng.disk_num < MAX_DISK_NUM; c++)
	{
		g_snprintf(dev_name, PATH_LEN, "/dev/sd%c", c);
		if(access(dev_name, F_OK))
			continue;
		if(!strg_mng_is_disk_dev(dev_name))
			continue;
		g_stpcpy(g_strg_mng.disk_path[g_strg_mng.disk_num], dev_name);	
		j_debug("disk[%d] = %s\n", g_strg_mng.disk_num, g_strg_mng.disk_path[g_strg_mng.disk_num]);
	
		g_strg_mng.disk_num++;
	}
#else
	g_strlcpy(dev_name, "/dev/sdb", sizeof(dev_name));
	if(access(dev_name, F_OK) == 0)
	{
		//if(!strg_mng_is_disk_dev(dev_name))
		//	continue;	
		g_stpcpy(g_strg_mng.disk_path[g_strg_mng.disk_num], dev_name);	
		j_debug("disk[%d] = %s\n", g_strg_mng.disk_num, g_strg_mng.disk_path[g_strg_mng.disk_num]);
		g_strg_mng.disk_num++;
	}
#endif

	return g_strg_mng.disk_num;
}
#endif

int strg_mng_write_start(int chn, StorageRecordType record_type)
{
	int disk_idx;

	MNG_LOCK();
	for(disk_idx = 0; disk_idx < g_strg_mng.disk_num; disk_idx++)
	{
		if(g_strg_mng.dm[disk_idx] == NULL)
			continue;
		if(strg_disk_write_start(g_strg_mng.dm[disk_idx], chn, record_type,
									g_atomic_int_get(&g_strg_mng.overwrite)) >= 0)
		{
			g_strg_mng.chn_write_disk_idx[chn] = disk_idx;
			MNG_UNLOCK();
			return 0;
		}
	}

	// 如果所有通道都不支持覆盖，返回错误
	if(g_atomic_int_get(&g_strg_mng.overwrite) == 0)
	{
		MNG_UNLOCK();
		return -1;
	}
	
	// 如果支持覆盖，复位disk中支持覆盖的chn，重新写入
	for(disk_idx = 0; disk_idx < g_strg_mng.disk_num; disk_idx++)
	{
		if(g_strg_mng.dm[disk_idx] == NULL)
			continue;
		if(strg_disk_reset(g_strg_mng.dm[disk_idx], g_atomic_int_get(&g_strg_mng.overwrite)) < 0)
			continue;
		if(strg_disk_write_start(g_strg_mng.dm[disk_idx], chn, record_type,
									g_atomic_int_get(&g_strg_mng.overwrite)) >= 0)
		{
			g_strg_mng.chn_write_disk_idx[chn] = disk_idx;
			MNG_UNLOCK();
			return 0;
		}
	}
	MNG_UNLOCK();
	
	return -1;
}

void strg_mng_write_stop(int chn)
{
	int disk_idx;
	MNG_LOCK();
	disk_idx = g_strg_mng.chn_write_disk_idx[chn];
	if(disk_idx >= 0)
	{
		j_debug("chn: %d, disk_idx:%d", chn, disk_idx);
		strg_disk_write_stop(g_strg_mng.dm[disk_idx], chn);
		g_strg_mng.chn_write_disk_idx[chn] = -1;
	}
	MNG_UNLOCK();
}

int strg_mng_write(int chn, guint64 cur_time,
					gpointer frame_data, int frame_size, 
					int frame_no, FrameType frame_type, EncodeType encode_type)
{
	int disk_idx = g_strg_mng.chn_write_disk_idx[chn];
	g_return_val_if_fail(disk_idx >= 0, -1);
	return strg_disk_write(g_strg_mng.dm[disk_idx], chn, cur_time,
						frame_data, frame_size,
						frame_no, frame_type, encode_type);
}

int strg_mng_write_set_type(int chn, StorageRecordType type)
{
	int ret, disk_idx;
	MNG_LOCK();
	disk_idx = g_strg_mng.chn_write_disk_idx[chn];
	if(disk_idx < 0)
	{
		MNG_UNLOCK();
		return -1;
	}
	ret = strg_disk_write_set_type(g_strg_mng.dm[disk_idx], chn, type);
	MNG_UNLOCK();
	return ret;
}

int strg_mng_write_set_overwrite(int chn, int overwrite)
{
	if(overwrite)
		g_atomic_int_or(&g_strg_mng.overwrite, 1<<chn);
	else
		g_atomic_int_and(&g_strg_mng.overwrite, ~(1<<chn));
	return 0;
}

int strg_mng_query(int chn_mask, time_t begin_time, time_t end_time, 
						StorageRecordType record_type, strg_query_func query_cb, gpointer data)
{
	g_return_val_if_fail(g_strg_mng.open_num != 0, -1);

	int i;

	for(i = 0; i < g_strg_mng.disk_num; i++)
	{
		if(g_strg_mng.dm[i] == NULL)
			continue;
		strg_disk_query(g_strg_mng.dm[i], chn_mask, 
							begin_time, end_time, record_type,
							query_cb, data);
	}
	return 0;
}

int strg_mng_find(int chn_mask, time_t begin_time, time_t end_time, StorageRecordType record_type)
{
	g_return_val_if_fail(g_strg_mng.open_num != 0, -1);

	int i;
	int mask = 0;

	for(i = 0; i < g_strg_mng.disk_num; i++)
	{
		if(g_strg_mng.dm[i] == NULL)
			continue;
		mask |= strg_disk_find(g_strg_mng.dm[i], chn_mask, 
							begin_time, end_time, record_type);
		if(mask < 0)
			break;
	}
	return mask;
}

static int strg_mng_read_valid(ReadHandle read_handle)
{
	int i;
	for(i = 0; i < g_strg_mng.disk_num; i++)
	{
		if(g_strg_mng.dm[i] == NULL)
			continue;
		if(strg_disk_read_valid(g_strg_mng.dm[i], read_handle))
			return 1;
	}
	return 0;
}

ReadHandle strg_mng_read_start(RecordQueryInfo *query_info, StorageFragmentInfo *sfi)
{
	g_return_val_if_fail(query_info->disk_idx < g_strg_mng.disk_num, NULL);
	return strg_disk_read_start(g_strg_mng.dm[query_info->disk_idx], query_info, sfi);
}

void strg_mng_read_stop(ReadHandle read_handle)
{
	if(!strg_mng_read_valid(read_handle))
		return;
	strg_disk_read_stop(read_handle);
}

int strg_mng_read_set(ReadHandle read_handle, time_t t)
{
	if(!strg_mng_read_valid(read_handle))
		return -1;
	return strg_disk_read_set(read_handle, t);
}

int strg_mng_read_seek(ReadHandle read_handle, int seek_iframe_offset)
{
	if(!strg_mng_read_valid(read_handle))
		return -1;
	return strg_disk_read_seek(read_handle, seek_iframe_offset);
}

int strg_mng_read(ReadHandle read_handle, StorageReadCmd cmd, 
							gpointer frame_buff, int buff_len)
{
	if(!strg_mng_read_valid(read_handle))
		return -1;
	return strg_disk_read(read_handle, cmd,
								frame_buff, buff_len);
}

int strg_mng_get_disk_num()
{
	return g_strg_mng.disk_num;
}

int strg_mng_get_open_num()
{
	return g_strg_mng.open_num;
}

int strg_mng_format_disk_start(int disk_idx, int check_bad)
{	
	gchar path[PATH_LEN];
	
	g_return_val_if_fail(disk_idx < g_strg_mng.disk_num, -1);
	MNG_LOCK();
	if(g_strg_mng.dm[disk_idx] != NULL)
	{
		strg_disk_close(g_strg_mng.dm[disk_idx]);
		g_strg_mng.dm[disk_idx] = NULL;
		g_strg_mng.open_num--;
	}
	strcpy(path, g_strg_mng.disk_path[disk_idx]);
	if(path[strlen(path) - 1] == '1')
		path[strlen(path) - 1] = 0;
	j_message("Begin to format disk %s.", path);
	if (strg_format_disk_start(path, disk_idx, check_bad) < 0)
	{
		j_warning("Format disk %s failed!", g_strg_mng.disk_path[disk_idx]);
		MNG_UNLOCK();
		return -1;
	}
	MNG_UNLOCK();
	
	return 0;
}

void strg_mng_format_disk_stop(int disk_idx)
{
	g_return_if_fail(disk_idx < g_strg_mng.disk_num);
	g_assert(g_strg_mng.dm[disk_idx] == NULL);
	gchar *path;

	path = g_strg_mng.disk_path[disk_idx];
	if(path[strlen(path) - 1] != '1')
		path[strlen(path)] = '1';
	MNG_LOCK();
	g_strg_mng.dm[disk_idx] = strg_format_disk_stop();
	if(g_strg_mng.dm[disk_idx] != NULL)
	{
		g_strg_mng.open_num++;
	}
	MNG_UNLOCK();
}

int strg_mng_format_get_percent(int *percent)
{
	return strg_format_get_percent(percent);
}

int strg_mng_get_disk_info(StorageDisksInfo *disks_info)
{
	g_assert(disks_info != NULL);
	int i, cnt = 0;
	int chn;
	int fd;
	unsigned long long size;
	StorageDiskInfo *disk_info;

	memset(disks_info, 0, sizeof(StorageDisksInfo));
	MNG_LOCK();
	for(i = 0; i < g_strg_mng.disk_num; i++)
	{
		disk_info = &disks_info->disk_info[cnt];
		disk_info->disk_idx = i;
		disk_info->total_size = 0;
		disk_info->free_size = 0;
		if(g_strg_mng.dm[i] != NULL)
		{
			disk_info->total_size = g_strg_mng.dm[i]->total_valid_size >> 20;
			disk_info->free_size = g_strg_mng.dm[i]->free_valid_size >> 20;
			for(chn = 0; chn < MAX_CHN; chn++)
			{
				if(g_strg_mng.chn_write_disk_idx[chn] == i)
					break;
			}
			if(strg_format_is_formatting() && strg_format_get_format_disk_idx() == disk_info->disk_idx)
				disk_info->disk_status = DISK_FORMATTING;
			else if(chn != MAX_CHN)
				disk_info->disk_status = DISK_USEING;
			else
				disk_info->disk_status = DISK_MOUNTED;
			cnt++;
		}
		else
		{
			if(access(g_strg_mng.disk_path[i], F_OK) == 0)
			{
				disk_info->total_size = 0;
				disk_info->free_size = 0;
				if(strg_format_is_formatting() && strg_format_get_format_disk_idx() == disk_info->disk_idx)
					disk_info->disk_status = DISK_FORMATTING;
				else
					disk_info->disk_status = DISK_NO_FORMATED;
				fd = open(g_strg_mng.disk_path[i], O_RDONLY);
				if(fd > 0)
				{
					if(ioctl(fd, BLKGETSIZE64, &size) >= 0)
					{
						disk_info->total_size = size >> 20;
					}
					close(fd);
				}
				cnt++;
			}
			else
				disk_info->disk_status = DISK_NO_DISK;
		}
	}
	disks_info->disk_num = cnt;
	MNG_UNLOCK();
	
	return 0;
}

void strg_mng_display_info()
{
	int i;

	for(i = 0; i < MAX_CHN; i++)
	{
		printf("chn_write_disk_idx[%d]: %d\n", i, g_strg_mng.chn_write_disk_idx[i]);
	}
	for(i = 0; i < g_strg_mng.disk_num; i++)
	{
		if(g_strg_mng.dm[i] == NULL)
			continue;
		strg_disk_display_info(g_strg_mng.dm[i]);
	}
}

