
#include <strg_header.h>
#include <strg_disk.h>
#include <strg_basket.h>

#include <parted/parted.h>



static gint format_percent = 0;
static gint format_once = 0;
static GThread *format_thread;
static gchar format_dev_path[PATH_LEN];
static gint format_disk_idx;

int strg_format_create_flag_file(const gchar *mount_path)
{
	g_assert(mount_path != NULL);
	
	gchar path[PATH_LEN];
	int fd;
	time_t t;
	struct tm stm, *ptm;
	gchar strtime[NAME_LEN];

	j_message("Create %s/flag file", mount_path);
	g_snprintf(path, sizeof(path), "%s/%s", mount_path, DISK_FORMAT_FLAG_FILE);
	fd = open(path, O_CREAT | O_WRONLY | O_TRUNC | O_SYNC, 0644);
	if(fd < 0)
	{
		j_warning("Open disk flag file failed");
		return -1;
	}
	t = time(NULL);
	ptm = localtime_r(&t, &stm);
	strftime(strtime, sizeof(strtime), "%F %T", ptm);
	write(fd, strtime, strlen(strtime));
	close(fd);
	j_message("Create %s flag successfully!", path);
	
	return 0;
}

int strg_format_detect_flag_file(const gchar *mount_path)
{
	g_assert(mount_path != NULL);
	gchar path[PATH_LEN];
	g_snprintf(path, sizeof(path), "%s/%s", mount_path, DISK_FORMAT_FLAG_FILE);
	if(access(path, F_OK) == 0)
		return 1;
	else
		return 0;
}

// dev_path is /dev/sda, /dev/sdb, not /dev/sda1, /dev/sda2, /dev/sdb1,etc.
static int strg_format_delete_all_partitions(PedDisk	*pDisk)
{
	g_assert(pDisk != NULL);
	
	if(!ped_disk_delete_all(pDisk))
	{
		j_warning("Failed to delete disk.");
		return -1;
	}
	ped_disk_commit_to_os(pDisk);
	ped_disk_commit_to_dev(pDisk);
	return 0;
}

static int strg_format_add_primary_partition(PedDevice *pDevice, PedDisk *pDisk)
{
	g_assert(pDevice != NULL);
	g_assert(pDisk != NULL);
	
	PedPartition *pPartition = NULL;
	PedPartitionType part_type = PED_PARTITION_NORMAL;
	const PedFileSystemType* fs_type = ped_file_system_type_get ("ext2");
	PedConstraint *pConstraint;	

	pPartition = ped_partition_new (pDisk, part_type, fs_type, 1, 2);
	if(pPartition == NULL)
	{
		j_warning("Failed to new partition.");
		return -1;
	}
	
	pConstraint = ped_device_get_constraint(pDevice);
	if(pConstraint == NULL)
	{
		j_warning("Failed to get constraint.");
		return -1;
	}
	if(!ped_disk_add_partition(pDisk, pPartition, pConstraint))
	{
		j_warning("Failed to add partition to disk.");
		return -1;
	}
	if(!ped_disk_maximize_partition(pDisk, pPartition, pConstraint))
	{
		j_warning("Failed to grow partition.");
		ped_constraint_destroy(pConstraint);
		return -1;
	}
	ped_constraint_destroy(pConstraint);
	ped_disk_commit_to_os(pDisk);
	ped_disk_commit_to_dev(pDisk);
	
	return 0;	
}


static int strg_format_create_partition(const gchar *dev_path)
{
	g_assert(dev_path != NULL);
	int i, ret = 0;
	PedDevice *pDevice = NULL;
	PedDisk	*pDisk = NULL;
	gchar partition_path[PATH_LEN];

	pDevice = ped_device_get(dev_path);
	if(pDevice == NULL)
	{
		j_warning("Failed to get device.");
		return -1;
	}
	if(!ped_device_open(pDevice))
	{
		j_warning("Failed to open device.");
		ped_device_destroy(pDevice);
		return -1;
	}
	pDisk = ped_disk_new(pDevice);
	if(pDisk == NULL)
	{
		j_warning("Failed to new disk.");
		ped_device_close(pDevice);
		ped_device_destroy(pDevice);
		return -1;
	}
	//ped_disk_print(pDisk);
	j_message("primary number: %d", ped_disk_get_primary_partition_count(pDisk));
	j_message("partion number: %d", ped_disk_get_last_partition_num(pDisk));
	strg_format_delete_all_partitions(pDisk);
	if(strg_format_add_primary_partition(pDevice, pDisk) < 0)
	{
		j_warning("Add primary partition on %s failed.", dev_path);
		ret = -2;
		goto exit_format;
	}
	
	g_snprintf(partition_path, sizeof(partition_path), "%s1", dev_path);
	for(i = 0; i < 10; i++)
	{
		if(access(partition_path, F_OK) == 0)
			break;
		usleep(100000);
	}
	if( i == 10 )
	{
		j_warning("Failed to find partition, is created OK?");
		ret = -3;
		goto exit_format;
	}
	
exit_format:
	ped_disk_destroy(pDisk);
	ped_device_sync (pDevice);
	ped_device_close(pDevice);
	ped_device_destroy(pDevice);

	return ret;
}

static int strg_format_mkfs_ext4(const gchar *dev_path)
{
	g_assert(dev_path != NULL);

	FILE *fp;
	gchar cmd[256];
	gchar buf[64];
	int total = 0;
	int cur = 0;

	////////////////////////////////////
	// -b 4096 设置块大小为4096
	// ^has_journal 不使用根文件系统
	// sparse_super减少super_block拷贝
	g_snprintf(cmd, sizeof(cmd), "mkfs.ext4 -F -b 4096 -O \"^has_journal sparse_super\" %s", dev_path);
	//system(cmd);
	j_debug("format command: %s", cmd);
	fp = popen(cmd, "r");
	if(fp == NULL)
	{
		j_warning("Failed to execute command %s.", cmd);
		return -1;
	}
	while(fgets(buf, sizeof(buf), fp) != NULL)
	{
		j_debug("mkfs.ext print: %s", buf);
		if(strncmp(buf, "percent:", strlen("percent:")) != 0)
			continue;
		if(sscanf(buf, "percent:%d/%d", &cur, &total) != 2)
			j_warning("Get mkfs.ext4 format percent failed.");
		if(total != 0)
		{
			if(cur * 50 / total > format_percent)
				g_atomic_int_set(&format_percent, cur * 50 / total);
		}
	}
	pclose(fp);
	return 0;
}

static int strg_format_mkfs_ext4_check_bad(const gchar *dev_path)
{
	g_assert(dev_path != NULL);

	FILE *fp;
	gchar cmd[256];
	gchar buf[64];
	gchar value[64];
	int percent;

	////////////////////////////////////
	// -c 检测硬盘坏块，会消耗非常多的时间
	// -b 4096 设置块大小为4096
	// ^has_journal 不使用根文件系统
	// sparse_super减少super_block拷贝
	g_snprintf(cmd, sizeof(cmd), "mkfs.ext4 -c -F -b 4096 -O \"^has_journal sparse_super\" %s", dev_path);
	//system(cmd);
	j_debug("format command: %s", cmd);
	fp = popen(cmd, "r");
	if(fp == NULL)
	{
		j_warning("Failed to execute command %s.", cmd);
		return -1;
	}
	while(fgets(buf, sizeof(buf), fp) != NULL)
	{
		j_message("mkfs.ext print: %s", buf);
		if(strstr(buf, "badblocks percent:") == NULL)
			continue;
		/* 0.42% done, 0:20 elapsed. (0/0/0 errors) */
		if(sscanf(buf, "badblocks percent:%[^.]", value) != 1)
			continue;
		percent = atoi(value);
		if(percent < 0)
			continue;
		if(percent >= 100)
			percent = 100;
		g_atomic_int_set(&format_percent, percent * 50 / 100);
	}
	pclose(fp);
	
	return 0;
}

static int strg_format_create_baskets(const gchar *mount_path)
{
	g_assert(mount_path != NULL);

	int i;
	int basket_num;

	if((basket_num = strg_disk_global_format(mount_path)) < 0)
	{
		j_warning("Disk global format failed");
		return -1;
	}
	g_atomic_int_set(&format_percent, 55);

	for(i = 0; i < basket_num; i++)
	{
		if(strg_basket_format(mount_path, i) < 0)
		{
			j_warning("Basket format failed");
			return -1;
		}
		g_atomic_int_set(&format_percent, 55 + i * 35 / basket_num);
	}
	g_atomic_int_set(&format_percent, 90);
	
	return 0;
}

int strg_format_get_percent(int *percent)
{
	g_assert(percent != NULL);
	if(!g_atomic_int_get(&format_once))
	{
		j_warning("The disk is not formatting!\n");
		return -1;
	}
	//j_debug("format percent: %d", g_atomic_int_get(&format_percent));
	*percent = g_atomic_int_get(&format_percent);
	return 0;
}
#if 0
static gpointer strg_format_disk_func(gpointer data)
{
	int try_cnt;
	const gchar *name;
	gchar *dev_path = format_dev_path;
	gchar device_path[PATH_LEN];
	gchar mount_path[PATH_LEN];
	int is_exist[2];
	DiskMng *dm;
	
	if(dev_path == NULL)
		goto failed;
		
	name = basename(dev_path);
	if(name == NULL)
	{
		j_warning("Can't get base name of dev path.");
		goto failed;
	}
	g_snprintf(device_path, sizeof(device_path), "%s1", dev_path);
	if(access(device_path, F_OK) == 0)
		is_exist[0] = 1;
	else
		is_exist[0] = 0;
	g_snprintf(device_path, sizeof(device_path), "%s2", dev_path);
	if(access(device_path, F_OK) == 0)
		is_exist[1] = 1;
	else
		is_exist[1] = 0;
	if(is_exist[0] == 0 || is_exist[1] == 1)
	{
		j_message("Create a partition on %s.", dev_path);
		if(strg_format_create_partition(dev_path) < 0)
		{
			j_warning("Create a partition on %s failed.", dev_path);
			goto failed;
		}
	}
	g_snprintf(device_path, sizeof(device_path), "%s1", dev_path);
	j_message("Test the %s is exist.", device_path);
	for (try_cnt = 0; try_cnt < 10; try_cnt++)
	{
		if (access(device_path, F_OK) == 0)
		{
			break;
		}
		usleep(100000);
	}
	j_message("Format disk %s to ext4.", device_path); 
	if(strg_format_mkfs_ext4(device_path) < 0)
	{
		j_warning("mkfs partition %s to ext4 failed.", dev_path);
		goto failed;
	}
	g_snprintf(mount_path, sizeof(mount_path), "/media/%s1", name);
	j_message("Mount %s to %s.", device_path, mount_path); 
	if(strg_disk_mount(device_path, mount_path) < 0)
	{
		j_warning("Mount %s to %s failed.", device_path, mount_path);
		goto failed;
	}
	j_message("Create baskets in %s.", mount_path);
	if(strg_format_create_baskets(mount_path) < 0)
	{
		j_warning("Create baskets in %s failed.", mount_path);
		strg_disk_umount(mount_path);
		goto failed;
	}
	j_message("Create flag file in %s.", mount_path);
	strg_format_create_flag_file(mount_path);
	j_message("umount %s.", mount_path);
	strg_disk_umount(mount_path);

	if((dm = strg_disk_open(device_path, format_disk_idx)) < 0)
	{
		j_warning("Open disk %s failed", device_path);
		goto failed;
	}
	
	g_atomic_int_set(&format_percent, 100);
	j_message("Write disk infomation successfully.");

	return dm;

failed:
	g_atomic_int_set(&format_percent, -1);
	return NULL;
}
#else
static gpointer strg_format_disk_func(gpointer data)
{
	const gchar *name;
	gchar *dev_path = format_dev_path;
//	gchar dev_path[PATH_LEN];
	gchar mount_path[PATH_LEN];
	DiskMng *dm;
	int check_bad = GPOINTER_TO_INT(data);
	
	if(dev_path == NULL)
		goto failed;
		
	name = basename(dev_path);
	if(name == NULL)
	{
		j_warning("Can't get base name of dev path.");
		goto failed;
	}
	j_message("Format disk %s to ext4.", dev_path); 
	if(!check_bad)
	{
		if(strg_format_mkfs_ext4(dev_path) < 0)
		{
			j_warning("mkfs partition %s to ext4 failed.", dev_path);
			goto failed;
		}
	}
	else
	{
		if(strg_format_mkfs_ext4_check_bad(dev_path) < 0)
		{
			j_warning("mkfs partition %s to ext4 failed.", dev_path);
			goto failed;
		}
	}
	g_snprintf(mount_path, sizeof(mount_path), "/media/%s", name);
	j_message("Mount %s to %s.", dev_path, mount_path); 
	if(strg_disk_mount(dev_path, mount_path) < 0)
	{
		j_warning("Mount %s to %s failed.", dev_path, mount_path);
		goto failed;
	}
	j_message("Create baskets in %s.", mount_path);
	if(strg_format_create_baskets(mount_path) < 0)
	{
		j_warning("Create baskets in %s failed.", mount_path);
		strg_disk_umount(mount_path);
		goto failed;
	}
	j_message("Create flag file in %s.", mount_path);
	strg_format_create_flag_file(mount_path);
	j_message("umount %s.", mount_path);
	strg_disk_umount(mount_path);
	j_message("Write disk identifier.");
	if(strg_disk_write_identifier(dev_path) < 0)
	{
		j_warning("Can't write disk identifier.");
		goto failed;
	}

	if((dm = strg_disk_open(dev_path, format_disk_idx)) < 0)
	{
		j_warning("Open disk %s failed", dev_path);
		goto failed;
	}
	
	g_atomic_int_set(&format_percent, 100);
	j_message("Write disk infomation successfully.");

	return dm;

failed:
	g_atomic_int_set(&format_percent, -1);
	return NULL;
}
#endif

int strg_format_disk_start(const gchar *dev_path, int disk_idx, int check_bad)
{	
	if(access(dev_path, F_OK))
		return -1;
	if (!g_atomic_int_compare_and_exchange(&format_once, 0, 1))
	{
		j_warning("The disk is formating.");
		return -1;
	}
	log_write(LOG_INFO, RECORD_LOG, "格式化磁盘%s", dev_path);
	g_atomic_int_set(&format_percent, 0);

	g_stpcpy(format_dev_path, dev_path);
	format_disk_idx = disk_idx;
	
	format_thread = g_thread_new("StorageFormat", strg_format_disk_func, GINT_TO_POINTER(check_bad));
	//g_thread_unref(format_thread);
	return 0;
}

DiskMng* strg_format_disk_stop()
{
	DiskMng *dm = g_thread_join(format_thread);
	g_atomic_int_set(&format_percent, 0);
	g_atomic_int_set(&format_once, 0);
	log_write(LOG_INFO, RECORD_LOG, "完成格式化磁盘");
	return dm;
}

int strg_format_is_formatting()
{
	return g_atomic_int_get(&format_once);
}

int strg_format_get_format_disk_idx()
{
	return format_disk_idx;
}

