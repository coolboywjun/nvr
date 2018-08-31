
#include <strg_header.h>

#include <strg_common.h>
#include <strg_basket.h>
#include <strg_disk.h>
#include <strg_format.h>

/////////////////////////////
// 是否需要优化扇区优化
///////////////////////////////

static void strg_disk_log(int fd, int prority, char *fmt, ...);

#define d_message(fmt, args...) 	\
	do{	\
		strg_disk_log(dm->log_fd, LOG_INFO, "[%s %d] "fmt, __func__, __LINE__, ##args);	\
		g_message("[%s %d] "fmt, __func__, __LINE__, ##args);	\
	}while(0)
#define d_error(fmt, args...) 	\
	do{	\
		strg_disk_log(dm->log_fd, LOG_ERR, "[%s %d] "fmt, __func__, __LINE__, ##args);	\
		g_error("[%s %d] "fmt, __func__, __LINE__, ##args);	\
	}while(0)
#define d_warning(fmt, args...) 	\
	do{	\
		strg_disk_log(dm->log_fd, LOG_WARNING, "[%s %d] "fmt, __func__, __LINE__, ##args);	\
		g_warning("[%s %d] "fmt, __func__, __LINE__, ##args);	\
	}while(0)
#define d_critical(fmt, args...) 	\
	do{	\
		strg_disk_log(dm->log_fd, LOG_CRIT, "[%s %d] "fmt, __func__, __LINE__, ##args);	\
		g_critical("[%s %d] "fmt, __func__, __LINE__, ##args);	\
	}while(0)
	
#ifdef STORAGE_DEBUG
#define d_debug(fmt, args...) 	\
	do{	\
		strg_disk_log(dm->log_fd, LOG_DEBUG, "[%s %d] "fmt, __func__, __LINE__, ##args);	\
		g_critical("[%s %d] "fmt, __func__, __LINE__, ##args);	\
	}while(0)
#else
#define d_debug(fmt, args...)
#endif

#define DISK_BG_OFT(basket_idx)		(DISK_GLOBAL_OFT + DISK_GLOBAL_LEN + (BASKET_GLOBAL_LEN + BASKET_FRAGMENT_SIZE) * basket_idx)
#define DISK_BF_OFT(basket_idx, bf_idx)	(DISK_BG_OFT(basket_idx) + BASKET_GLOBAL_LEN + BASKET_FRAGMENT_LEN * bf_idx)

static int strg_disk_check_global(DiskGlobal *dg)
{
	g_return_val_if_fail(dg->magic == DISK_GLOBAL_MAGIC, -1);
	g_return_val_if_fail(calc_checksum(dg, DISK_GLOBAL_LEN - sizeof(dg->checksum)) == dg->checksum, -1);
	return 0;
}

static int strg_disk_write_global(int fd, DiskGlobal *dg)
{
	g_assert(dg != NULL);
	g_assert(dg->magic == DISK_GLOBAL_MAGIC);
	//g_return_val_if_fail(lseek(fd, DISK_GLOBAL_OFT, SEEK_SET) >= 0, -1);
	dg->checksum = calc_checksum((gulong*)dg, DISK_GLOBAL_LEN - sizeof(dg->checksum));
	g_return_val_if_fail(strg_write(fd, dg, DISK_GLOBAL_LEN, DISK_GLOBAL_OFT) == DISK_GLOBAL_LEN, -1);
	return DISK_GLOBAL_LEN;
}

static int strg_disk_read_global(int fd, DiskGlobal *dg)
{
	g_assert(dg != NULL);
	//g_return_val_if_fail(lseek(fd, DISK_GLOBAL_OFT, SEEK_SET) >= 0, -1);
	g_return_val_if_fail(strg_read(fd, dg, DISK_GLOBAL_LEN, DISK_GLOBAL_OFT) == DISK_GLOBAL_LEN, -1);
	g_return_val_if_fail(strg_disk_check_global(dg) == 0, -1);
	return DISK_GLOBAL_LEN;
}

static int strg_disk_write_bg(int fd, int basket_idx, BasketGlobal *bg)
{
	g_assert(bg != NULL);
j_debug("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ bg oft: %d", DISK_BG_OFT(basket_idx));
	//g_return_val_if_fail(lseek(fd, DISK_BG_OFT(basket_idx), SEEK_SET) >= 0, -1);
	bg->checksum = calc_checksum((gulong*)bg, BASKET_GLOBAL_LEN - sizeof(bg->checksum));
	g_return_val_if_fail(strg_write(fd, bg, BASKET_GLOBAL_LEN, DISK_BG_OFT(basket_idx)) == BASKET_GLOBAL_LEN, -1);
	return BASKET_GLOBAL_LEN;
}

static int strg_disk_write_bf(int fd, int basket_idx, int bf_idx, BasketFragment *bf)
{
	g_assert(bf != NULL);
j_debug("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ bf oft: %d", DISK_BF_OFT(basket_idx, bf_idx));
	//g_return_val_if_fail(lseek(fd, DISK_BF_OFT(basket_idx, bf_idx), SEEK_SET) >= 0, -1);
	bf->checksum = calc_checksum((gulong*)bf, BASKET_FRAGMENT_LEN - sizeof(bf->checksum));
	g_return_val_if_fail(strg_write(fd, bf, BASKET_FRAGMENT_LEN, DISK_BF_OFT(basket_idx, bf_idx)) == BASKET_FRAGMENT_LEN, -1);
	return BASKET_FRAGMENT_LEN;
}

static void strg_disk_log(int fd, int prority, char *fmt, ...)
{
	gchar text[1024];
	va_list args; 
	int len;
	time_t t;
	struct tm *ptm, stm;

	if(fd < 0)
		return;
		
	t = time(NULL);
	ptm = localtime_r(&t, &stm);
	len = strftime(text, sizeof(text), "[%F %T]: ", ptm);
	
	va_start(args, fmt);
	len += vsnprintf(&text[len], sizeof(text)-len-1, fmt, args);
	va_end(args);
	if(text[len - 1] != '\n')
		text[len++] = '\n';
	write(fd, text, len);
}

int strg_disk_umount(const gchar *mount_path)
{
	if(umount(mount_path) < 0)
	{
		j_warning("Umount %s failed: %s", mount_path, strerror(errno));
		return -1;
	}
	return 0;
}

int strg_disk_mount(const gchar* dev_path, const gchar *mount_path)
{
	g_assert(dev_path != NULL);
	g_assert(mount_path != NULL);
	// 不修改访问时间,但是还是会修改ctime和mtime
	return mount(dev_path, mount_path, "ext4", MS_NOATIME, NULL);
}

// mb
int strg_disk_get_size(const gchar *mount_path, int *total_mb, int *free_mb)
{
	struct statfs fs;
	g_assert(mount_path != NULL);
	g_assert(total_mb != NULL || free_mb != NULL);

	if(statfs(mount_path, &fs) < 0)
	{
		j_warning("Get disk information failed.");
		return -1;
	}
	if(total_mb != NULL)
		*total_mb = ((fs.f_blocks >> 10) * (fs.f_bsize >> 10));
	if(free_mb != NULL)
		*free_mb = ((fs.f_bfree >> 10) * (fs.f_bsize >> 10));
	return 0;
}

static void  __strg_disk_bgi_list_free(gpointer data)
{
	BASKET_GLOBAL_INFO_FREE(data);
}

static void  __strg_disk_bf_list_free(gpointer data)
{
	BASKET_FRAGMENT_FREE(data);
}

static gint __strg_disk_bgi_insert_sort(gconstpointer  a, gconstpointer  b)
{
	BasketGlobalInfo *bgi1 = (BasketGlobalInfo*)a;
	BasketGlobalInfo *bgi2 = (BasketGlobalInfo*)b;
	if(bgi1->basket_idx < bgi2->basket_idx)
		return -1;
	else if(bgi1->basket_idx > bgi2->basket_idx)
		return 1;
	else
		return 0;
}

#ifdef STORAGE_DEBUG
static void strg_disk_test_global(const gchar *mount_path)
{
	int g_fd;
	int b_fd;
	gchar path[PATH_LEN];
	int basket_idx;
	DiskGlobal dg;
	gpointer g_mem, b_mem;
	int len;
	
	g_snprintf(path, sizeof(path), "%s/%s", mount_path, DISK_GLOBAL_NAME);
	g_fd = open(path, O_RDONLY);
	if(g_fd < 0)
		return;
	if(strg_disk_read_global(g_fd, &dg) < 0)
	{
		close(g_fd);
		return;
	}
	len = BASKET_GLOBAL_LEN + BASKET_FRAGMENT_SIZE;
	g_mem = g_malloc(len);
	b_mem = g_malloc(len);
	for(basket_idx = 0; basket_idx < dg.basket_num; basket_idx++)
	{
		lseek(g_fd, DISK_BG_OFT(basket_idx), SEEK_SET);
		g_snprintf(path, sizeof(path), "%s/basket%d", mount_path, basket_idx);
		b_fd = open(path, O_RDONLY);
		if(b_fd < 0)
			continue;
		if(read(g_fd, g_mem, len) != read(b_fd, b_mem, len))
		{
			close(b_fd);
			continue;
		}
		if(memcmp(g_mem, b_mem, len) != 0)
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		close(b_fd);
	}
	close(g_fd);
	g_free(g_mem);
	g_free(b_mem);
}
#endif

static void strg_disk_global_write(DiskMng *dm)
{
	GList *bgi_list;
	GList *bf_list;
	BasketGlobalInfo *bgi;
	BasketFragment *bf;
	int bf_idx;
	
	for(bgi_list = g_list_first(dm->bgi_list); bgi_list != NULL; bgi_list = g_list_next(bgi_list))
	{
		bgi = (BasketGlobalInfo*)bgi_list->data;
		if(bgi->update_flag)
		{
			bgi->bg.status = USE_STATUS_IDLE;
			strg_disk_write_bg(dm->fd, bgi->basket_idx, &bgi->bg);
			for(bf_idx = 0, bf_list = g_list_first(bgi->bf_list); bf_list != NULL; bf_list = g_list_next(bf_list))
			{
				bf = (BasketFragment *)bf_list->data;
				bf->status = USE_STATUS_IDLE;
				strg_disk_write_bf(dm->fd, bgi->basket_idx, bf_idx++, bf);
			}
			bgi->update_flag = 0;
		}
	}
}

static int strg_disk_global_open(DiskMng *dm)
{
	g_assert(dm != NULL);
	gchar path[PATH_LEN];
	BasketGlobalInfo *bgi;
	BasketFragment *bf;
	int basket_idx;
	int bf_idx;
	
	g_snprintf(path, sizeof(path), "%s/%s", dm->mount_path, DISK_GLOBAL_FILE);

	// disk global 以dsync方式打开，保证数据都写入
	dm->fd = open(path, O_RDWR | O_DSYNC);
	if(dm->fd < 0)
	{
		j_warning("Open disk global file failed.");
		return -1;
	}
	j_message("%s open fd: %d", path, dm->fd);
	if(strg_disk_read_global(dm->fd, &dm->dg) < 0)
	{
		j_warning("Open disk global file failed.");
		close(dm->fd);
		return -1;
	}
	j_message("Storage version: %s", dm->dg.ver);
	
	dm->total_valid_size = dm->dg.basket_num * ((unsigned long long )BASKET_FRAME_SIZE);
	dm->free_valid_size = dm->total_valid_size;
	j_debug("basket num: %d", dm->dg.basket_num);
	j_debug("frame size: %lu", BASKET_FRAME_SIZE);
	j_debug("total valid size: %llu", dm->total_valid_size);
	for(basket_idx = 0; basket_idx < dm->dg.basket_num; basket_idx++)
	{
		bgi = BASKET_GLOBAL_INFO_ALLOC();
		//lseek(dm->fd, DISK_BG_OFT(basket_idx), SEEK_SET);
		if(strg_read(dm->fd, &bgi->bg, sizeof(bgi->bg), DISK_BG_OFT(basket_idx)) < 0)
		{
			j_warning("Read basket global failed");
			BASKET_GLOBAL_INFO_FREE(bgi);
			break;
		}
		if(calc_checksum(&bgi->bg, BASKET_GLOBAL_LEN - sizeof(bgi->bg.checksum)) != bgi->bg.checksum)
		{
			j_warning("Check basket global failed.");
			BASKET_GLOBAL_INFO_FREE(bgi);
			continue;
		}
		g_snprintf(bgi->basket_path, sizeof(bgi->basket_path), "%s/basket%d", dm->mount_path, basket_idx);
		bgi->basket_idx = basket_idx;
		bgi->free_space = BASKET_FRAME_SIZE;
		bgi->free_iframe = BASKET_IFRAME_NUM;
		
		for(bf_idx = 0; bf_idx < bgi->bg.fragment_num; bf_idx++)
		{
			//lseek(dm->fd, DISK_BF_OFT(basket_idx, bf_idx), SEEK_SET);
			bf = BASKET_FRAGMENT_ALLOC();
			if(strg_read(dm->fd, bf, BASKET_FRAGMENT_LEN, DISK_BF_OFT(basket_idx, bf_idx)) < 0)
			{
				j_warning("Read basket%d fragment %d failed", basket_idx, bf_idx);
				BASKET_FRAGMENT_FREE(bf);
				break;
			}
			if(calc_checksum(bf, BASKET_FRAGMENT_LEN - sizeof(bf->checksum)) != bf->checksum)
			{
				j_warning("Check basket%d fragment %d failed", basket_idx, bf_idx);
				BASKET_FRAGMENT_FREE(bf);
				break;
			}
			if(bf->use_cnt != bgi->bg.use_cnt)
			{
				j_warning("Don't reach here, bf use_cnt: %d, bg use_cnt: %d", bf->use_cnt, bgi->bg.use_cnt);
				BASKET_FRAGMENT_FREE(bf);
				break;
			}
			if(bf->status & USE_STATUS_WRITING)
			{
				j_warning("The basket fragment status is wrong!");
				if(strg_basket_scan_fragment(bgi, bf, bf_idx) < 0)
				{
					j_warning("Scan basket failed");
					BASKET_FRAGMENT_FREE(bf);
					break;
				}
				strg_disk_write_bf(dm->fd, basket_idx, bf_idx, bf);
				bgi->bg.status = USE_STATUS_IDLE;
				strg_disk_write_bg(dm->fd, basket_idx, &bgi->bg);
			}
			bf->status = USE_STATUS_IDLE;
			bgi->bf_list = g_list_append(bgi->bf_list, bf);
			bgi->free_space -= bf->fragment_size;
			bgi->free_iframe -= bf->iframe_num;
			if(bgi->begin_time > bf->begin_time || bgi->begin_time == 0)
				bgi->begin_time = bf->begin_time;
			if(bgi->end_time < bf->end_time)
				bgi->end_time = bf->end_time;
			bgi->record_type |= bf->record_type;
		}
		
		dm->bgi_list = g_list_append(dm->bgi_list, bgi);
		if(dm->dg.use_cnt == bgi->bg.use_cnt)
		{
			dm->bgi_new = g_list_append(dm->bgi_new, bgi);
			dm->free_valid_size -= bgi->free_space;
		}
		else
		{
			dm->bgi_old = g_list_append(dm->bgi_old, bgi);
		}
	}

	return 0;
}

static void strg_disk_global_close(DiskMng *dm)
{
	g_assert(dm != NULL);
	GList *bgi_list;
	BasketGlobalInfo *bgi;
	
	strg_disk_global_write(dm);
	for(bgi_list = g_list_first(dm->bgi_list); bgi_list != NULL; bgi_list = g_list_next(bgi_list))
	{
		bgi = (BasketGlobalInfo*)bgi_list->data;
		g_list_free_full(bgi->bf_list, __strg_disk_bf_list_free);
		bgi->bf_list = NULL;
	}
	g_list_free_full(dm->bgi_list, __strg_disk_bgi_list_free);
	dm->bgi_list = NULL; 
	g_list_free(dm->bgi_new);
	g_list_free(dm->bgi_old);
	dm->bgi_new = NULL;
	dm->bgi_old = NULL;
	close(dm->fd);
}

static int strg_disk_global_scan(DiskMng *dm)
{
	g_assert(dm != NULL);
	
	gchar path[PATH_LEN];
	int basket_idx;
	BasketGlobalInfo *bgi;
	
	g_snprintf(path, sizeof(path), "%s/%s", dm->mount_path, DISK_GLOBAL_FILE);

	// disk global 以dsync方式打开，保证数据都写入
	dm->fd = open(path, O_RDWR | O_DSYNC);
	if(dm->fd < 0)
	{
		j_warning("Open disk global file failed.");
		return -1;
	}
	j_message("%s open fd: %d", path, dm->fd);
	if(strg_disk_read_global(dm->fd, &dm->dg) < 0)
	{
		j_warning("Open disk global file failed.");
		close(dm->fd);
		return -1;
	}
	j_message("Storage version: %s", dm->dg.ver);
	
	dm->total_valid_size = dm->dg.basket_num * ((unsigned long long )BASKET_FRAME_SIZE);
	dm->free_valid_size = dm->total_valid_size;
	for(basket_idx = 0; basket_idx < dm->dg.basket_num; basket_idx++)
	{
		bgi = BASKET_GLOBAL_INFO_ALLOC();
		g_snprintf(bgi->basket_path, sizeof(bgi->basket_path), "%s/basket%d", dm->mount_path, basket_idx);
		bgi->basket_idx = basket_idx;
		if(strg_basket_scan(bgi) < 0)
		{
			j_warning("Scan basket failed");
			BASKET_GLOBAL_INFO_FREE(bgi);
			dm->free_valid_size -= BASKET_FRAME_SIZE;
			continue;
		}
		
		dm->bgi_list = g_list_append(dm->bgi_list, bgi);
		if(dm->dg.use_cnt == bgi->bg.use_cnt)
		{
			dm->bgi_new = g_list_append(dm->bgi_new, bgi);
			dm->free_valid_size -= bgi->free_space;
		}
		else
		{
			dm->bgi_old = g_list_append(dm->bgi_old, bgi);
		}
	}
		
	return 0;
}

static int strg_disk_create_flag_file(const gchar *mount_path)
{
	g_assert(mount_path != NULL);
	
	gchar path[PATH_LEN];
	int fd;
	time_t t;
	struct tm stm, *ptm;
	gchar strtime[NAME_LEN];

	j_message("Create %s/%s file", mount_path, DISK_WRITE_FLAG_FILE);
	g_snprintf(path, sizeof(path), "%s/%s", mount_path, DISK_WRITE_FLAG_FILE);
	fd = open(path, O_CREAT | O_WRONLY | O_TRUNC | O_SYNC, 0666);
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

static void strg_disk_delete_flag_file(const gchar *mount_path)
{
	g_assert(mount_path != NULL);
	gchar path[PATH_LEN];
	g_snprintf(path, sizeof(path), "%s/%s", mount_path, DISK_WRITE_FLAG_FILE);
	unlink(path);
}

static int strg_disk_detect_flag_file(const gchar *mount_path)
{
	g_assert(mount_path != NULL);
	gchar path[PATH_LEN];
	g_snprintf(path, sizeof(path), "%s/%s", mount_path, DISK_WRITE_FLAG_FILE);
	if(access(path, F_OK) == 0)
		return 1;
	else
		return 0;
}

DiskMng* strg_disk_open(gchar* dev_path, int disk_idx)
{
	g_return_val_if_fail(dev_path != NULL, NULL);

	DiskMng *dm;
	const char *name;
	//gchar path[PATH_LEN];
	
	name = basename(dev_path);
	if(name == NULL)
	{
		j_warning("Can't get base name of dev path.");
		return NULL;
	}
	dm = g_new0(DiskMng, 1);
	dm->disk_idx = disk_idx;
	g_mutex_init(&dm->mutex);
	g_stpcpy(dm->dev_path, dev_path);
	g_snprintf(dm->mount_path, sizeof(dm->mount_path), "/media/%s", name);
	j_message("Mount %s to %s.", dm->dev_path, dm->mount_path);
	strg_disk_umount(dm->mount_path);
	if(strg_disk_mount(dm->dev_path, dm->mount_path) < 0)
	{
		j_warning("Mount %s to %s failed.", dm->dev_path, dm->mount_path);
		goto failed;
	}
	if(strg_format_detect_flag_file(dm->mount_path) == 0)
	{
		j_warning("The disk don't format!");
		goto failed;
	}
	//g_snprintf(path, sizeof(path), "%s/%s", dm->mount_path, DISK_LOG_NAME);
	//dm->log_fd = open(path, O_CREAT | O_WRONLY | O_APPEND | O_SYNC, 0666);
	if(strg_disk_get_size(dm->mount_path, &dm->total_size_mb, NULL) < 0)
	{
		j_warning("Get total size of %s disk failed.", dm->mount_path);
		goto failed;
	}
#ifdef STORAGE_DEBUG
	//strg_disk_test_global(dm->mount_path);
#endif	
	if(strg_disk_detect_flag_file(dm->mount_path))
	{
		j_message("Detect the disk flag file.");
		if(strg_disk_global_scan(dm) < 0)
		{
			j_warning("Disk global scan failed");
			goto failed;
		}
		strg_disk_global_write(dm);
		strg_disk_delete_flag_file(dm->mount_path);
	}
	else
	{
		if(strg_disk_global_open(dm) < 0)
		{
			j_warning("Open disk mng file info failed.");
			goto failed;
		}
	}
	j_message("Open disk%d %s OK!", disk_idx, dev_path); 
	
	return dm;

failed:
	//if(dm->log_fd > 0)
	//	close(dm->log_fd);
	strg_disk_umount(dm->mount_path);
	g_free(dm);
		
	return NULL;
}

void strg_disk_close(DiskMng *dm)
{
	g_return_if_fail(dm != NULL);

	int chn;
	GList *list;
	ReadHandle read_handle;
	
	for(list = g_list_first(dm->read_list); list != NULL; list = g_list_next(list))
	{
		read_handle = list->data;
		strg_basket_read_stop(read_handle);
	}
	g_list_free(dm->read_list);
	for(chn = 0; chn < MAX_CHN; chn++)
	{
		if(dm->bfi_cur[chn] != NULL)
			strg_basket_close(dm->bfi_cur[chn]);
	}
	j_debug("disk %s close", dm->mount_path);
	strg_disk_global_close(dm);
	strg_disk_delete_flag_file(dm->mount_path);
	//if(dm->log_fd > 0)
	//	close(dm->log_fd);
	strg_disk_umount(dm->mount_path);
	g_mutex_clear(&dm->mutex);
	g_free(dm); 
}

// 复位disk, 但不是删除内部图像
int strg_disk_reset(DiskMng *dm, int overwrite)
{
	g_return_val_if_fail(dm != NULL, -1);

	GList *bgi_list;
	BasketGlobalInfo *bgi;
	int find_flag = 0;
	
	j_debug("use cnt: %d, basket num: %d", dm->dg.use_cnt, dm->dg.basket_num);
	
	DISK_LOCK(dm);
	for(bgi_list = g_list_first(dm->bgi_new); bgi_list != NULL; bgi_list = g_list_next(bgi_list))
	{
		bgi = (BasketGlobalInfo *)bgi_list->data;
		if(bgi->bg.chn != -1 && !(overwrite & (1<<bgi->bg.chn)))
			continue;
		if((bgi->bg.status & USE_STATUS_READING))
			continue;
		if((1<<bgi->bg.chn) & overwrite)
		{
			find_flag = 1;
			break;
		}
	}
	if(!find_flag)
	{
		j_warning("There is not fragment to reset.\n");
		DISK_UNLOCK(dm);
		return -1;
	}
	
	// 修改DiskGlobal,增加use_cnt记数
	dm->dg.use_cnt++;
	dm->free_valid_size = dm->total_valid_size;
	j_message("Disk global use cnt: %d", dm->dg.use_cnt);
	if(strg_disk_write_global(dm->fd, &dm->dg) < 0)
	{
		dm->dg.use_cnt--;
		DISK_UNLOCK(dm);
		return -1;
	}
	
	// 如果bgi_new里面还有数据，移入bgi_old
	dm->bgi_old = g_list_concat(dm->bgi_old, dm->bgi_new);
	dm->bgi_new = NULL;
	j_message("Disk %s reset successfully!", dm->mount_path);
	DISK_UNLOCK(dm);
	
	return 0;
}

static BasketFileInfo *strg_disk_alloc_basket(DiskMng *dm, int chn, int overwrite)
{
	g_assert(dm != NULL);
	g_assert(chn < MAX_CHN);
	BasketFileInfo *bfi;
	BasketGlobalInfo *bgi;
	GList *bgi_list;

	// 从已经开始录像但还有剩余空间的basket中申请
	for(bgi_list = g_list_last(dm->bgi_new); bgi_list != NULL; bgi_list = g_list_previous(bgi_list))
	{
		bgi = (BasketGlobalInfo *)bgi_list->data;
		if(bgi->bg.chn == chn && !(bgi->bg.status & USE_STATUS_READING))
		{
			if(bgi->free_space >= BASKET_SPACE_MIN 
				&& (bgi->bg.status & USE_STATUS_FULL) == 0
				&& bgi->bg.fragment_num < BASKET_FRAGMENT_NUM 
				&& bgi->free_iframe >= BASKET_IFRAME_MIN)
			{
				if((bfi = strg_basket_open(dm, bgi)) == NULL)
				{
					j_warning("Open basket%d failed", bgi->basket_idx);
					continue;
				}
				dm->bgi_new = g_list_delete_link(dm->bgi_new, bgi_list);
				dm->bfi_cur[chn] = bfi;
				
				return bfi;
			}
		}
	}

/////////////////////////////
// 如果支持覆盖，复位disk，重新写入
//	if(g_list_length(dm->bgi_old) == 0)
//		strg_disk_reset(dm);
/////////////////////////////
// 如果支持覆盖，复位disk，重新写入

	// 从bgi_old中申请
	for(bgi_list = g_list_first(dm->bgi_old); bgi_list != NULL; bgi_list = g_list_next(bgi_list))
	{
		bgi = (BasketGlobalInfo *)bgi_list->data;
		if(bgi->bg.chn != -1 && !(overwrite & (1<<bgi->bg.chn)))
			continue;
		if(!(bgi->bg.status & USE_STATUS_READING))
		{
			bgi = (BasketGlobalInfo *)bgi_list->data;
			if((bfi = strg_basket_open(dm, bgi)) == NULL)
			{
				j_warning("Open basket%d failed", bgi->basket_idx);
				continue;
			}
			if(strg_basket_reset(bfi, chn, dm->dg.use_cnt) < 0)
			{
				strg_basket_close(bfi);
				continue;
			}
			j_message("Delete the basket%d data!", bgi->basket_idx);
			dm->bgi_old = g_list_delete_link(dm->bgi_old, bgi_list);
			dm->bfi_cur[chn] = bfi;
			
			return bfi;
		}
	}
	
	return NULL;
}

static void strg_disk_free_basket(DiskMng *dm, int chn)
{
	g_assert(dm != NULL);
	g_assert(chn < MAX_CHN);
	
	BasketFileInfo *bfi;
	
	bfi = dm->bfi_cur[chn];
	if(bfi->bgi->bg.use_cnt == dm->dg.use_cnt)
		dm->bgi_new = g_list_insert_sorted(dm->bgi_new, bfi->bgi, 
											__strg_disk_bgi_insert_sort);
	else
		dm->bgi_old = g_list_insert_sorted(dm->bgi_old, bfi->bgi, 
											__strg_disk_bgi_insert_sort);
	strg_basket_close(bfi);
	dm->bfi_cur[chn] = NULL;
}

int strg_disk_write_start(DiskMng *dm, int chn, StorageRecordType record_type, int overwrite)
{
	g_return_val_if_fail(dm != NULL, -1);
	
	BasketFileInfo *bfi;

	DISK_LOCK(dm);
	while((bfi = strg_disk_alloc_basket(dm, chn, overwrite)))
	{
		if(strg_basket_write_start(bfi, chn, record_type) < 0)
		{
			j_warning("Basket write start failed.");
			strg_disk_free_basket(dm, chn);
			continue;
		}
		j_debug("basket_idx: %d bf_idx: %d", bfi->bgi->basket_idx, bfi->cur_bf_idx);
		if(g_atomic_int_compare_and_exchange(&dm->create_flag_file, 0, 1))
			strg_disk_create_flag_file(dm->mount_path);
		strg_disk_write_bf(dm->fd, bfi->bgi->basket_idx, bfi->cur_bf_idx, bfi->cur_bf);
		strg_disk_write_bg(dm->fd, bfi->bgi->basket_idx, &bfi->bgi->bg);
		j_message("chn%d disk%d basket%d write start", chn, dm->disk_idx, bfi->bgi->basket_idx);
		break;
	}
	DISK_UNLOCK(dm);
	
	if(bfi == NULL)
	{
		j_warning("Don't find the basket to write.");
		return -1;
	}
	return dm->disk_idx;
}

void strg_disk_write_stop(DiskMng *dm, int chn)
{
	g_return_if_fail(dm != NULL);
	g_return_if_fail(chn < MAX_CHN);
	
	BasketFileInfo *bfi;
	BasketGlobalInfo *bgi;
	BasketFragment *cur_bf;
	int cur_bf_idx;
	
	DISK_LOCK(dm);
	if(dm->bfi_cur[chn] == NULL)
	{
		DISK_UNLOCK(dm);
		return;
	}
	if(dm->bfi_cur[chn]->bgi == NULL)
	{
		DISK_UNLOCK(dm);
		return;
	}
	if(dm->bfi_cur[chn]->cur_bf == NULL)
	{
		DISK_UNLOCK(dm);
		return;
	}
	bfi = dm->bfi_cur[chn];
	bgi = bfi->bgi;
	cur_bf = bfi->cur_bf;
	cur_bf_idx = bfi->cur_bf_idx;
	j_message("chn%d disk%d basket%d write stop", chn, dm->disk_idx, bfi->bgi->basket_idx);
	strg_basket_write_stop(bfi);
	strg_disk_write_bf(dm->fd, bgi->basket_idx, cur_bf_idx, cur_bf);
	strg_disk_write_bg(dm->fd, bgi->basket_idx, &bgi->bg);
	bgi->update_flag = 0;
	strg_disk_free_basket(dm, chn);
	DISK_UNLOCK(dm);
}

int strg_disk_write(DiskMng *dm, int chn, guint64 cur_time,
					gpointer frame_data, int frame_size, 
					int frame_no, FrameType frame_type, EncodeType encode_type)
{
	g_return_val_if_fail(dm != NULL, -1);
	g_return_val_if_fail(chn < MAX_CHN, -1);

	int ret;
	BasketFileInfo *bfi = dm->bfi_cur[chn];
	if((ret = strg_basket_write(bfi, cur_time, frame_data, frame_size, 
									frame_no, frame_type, encode_type)) > 0)
		dm->free_valid_size -= frame_size + BASKET_FRAME_LEN;
	return ret;
}

int strg_disk_write_set_type(DiskMng *dm, int chn, StorageRecordType type)
{
	g_return_val_if_fail(dm != NULL, -1);
	g_return_val_if_fail(chn < MAX_CHN, -1);
	g_return_val_if_fail(dm->bfi_cur[chn] != NULL, -1);

	BasketFileInfo *bfi;
	BasketGlobalInfo *bgi;
	BasketFragment *cur_bf;
	int cur_bf_idx;

	DISK_LOCK(dm);
	if(dm->bfi_cur[chn] == NULL)
	{
		DISK_UNLOCK(dm);
		return -1;
	}
	if(dm->bfi_cur[chn]->bgi == NULL)
	{
		DISK_UNLOCK(dm);
		return -1;
	}
	if(dm->bfi_cur[chn]->cur_bf == NULL)
	{
		DISK_UNLOCK(dm);
		return -1;
	}
	bfi = dm->bfi_cur[chn];
	bgi = bfi->bgi;
	cur_bf = bfi->cur_bf;
	cur_bf_idx = bfi->cur_bf_idx;
	j_message("chn%d disk%d basket%d set record type 0x%x", chn, dm->disk_idx, bfi->bgi->basket_idx, type);
	if((cur_bf->record_type & type) == 0)
	{
		cur_bf->record_type |= type;
		strg_disk_write_bf(dm->fd, bgi->basket_idx, cur_bf_idx, cur_bf);
		strg_disk_write_bg(dm->fd, bgi->basket_idx, &bgi->bg);
	}
	DISK_UNLOCK(dm);
	
	return 0;
}

int strg_disk_query(DiskMng *dm, int chn_mask, 
						time_t begin_time, time_t end_time, StorageRecordType record_type,
						strg_query_func query_cb, gpointer data)
{
	g_return_val_if_fail(dm != NULL, -1);
	g_return_val_if_fail(query_cb != NULL, -1);

	int ret;
	int bf_idx;
	GList *bgi_list;
	GList *bf_list;
	BasketGlobalInfo *bgi;
	BasketFragment *bf;
	RecordQueryInfo rqi;
	
	g_return_val_if_fail(end_time >= begin_time, -1);

	DISK_LOCK(dm);
	for(bgi_list = dm->bgi_list; bgi_list != NULL; bgi_list = g_list_next(bgi_list))
	{
		bgi = (BasketGlobalInfo *)bgi_list->data;
		if(!(chn_mask & (1<<bgi->bg.chn)))
			continue;
		// 判断时间是否在范围内
		if(end_time <= bgi->begin_time || begin_time >= bgi->end_time)
			continue;
		if((bgi->record_type & record_type) == 0)
			continue;
		bf_idx = 0;
		for(bf_list = bgi->bf_list; bf_list != NULL; bf_list = g_list_next(bf_list), bf_idx++)
		{
			bf = (BasketFragment *)bf_list->data;
			if(bf->iframe_num == 0 || bf->fragment_size == 0)
				continue;
			// 判断时间是否在范围内
			if(end_time <= bf->begin_time || begin_time >= bf->end_time)
				continue;
			if((bf->record_type & record_type) == 0)
				continue;
			if(begin_time <= bf->begin_time && end_time >= bf->end_time)
			{
				rqi.chn = bgi->bg.chn;
				rqi.disk_idx = dm->disk_idx;
				rqi.basket_idx = bgi->basket_idx;
				rqi.bf_idx = bf_idx;
				rqi.begin_iframe_idx = bf->first_iframe_idx;
				rqi.iframe_num = bf->iframe_num;
				rqi.begin_time = bf->begin_time;
				rqi.end_time = bf->end_time;
				rqi.fragment_size = bf->fragment_size;
				rqi.record_type = bf->record_type;
				g_return_val_if_fail((ret = query_cb(&rqi, data)) >= 0, ret);
			}
			else
				strg_basket_query(dm, bgi, bf, bf_idx, 
							begin_time, end_time, record_type, query_cb, data);
		}
	}
	DISK_UNLOCK(dm);
	
	return ret;
}

int strg_disk_find(DiskMng *dm, int chn_mask, 
						time_t begin_time, time_t end_time, StorageRecordType record_type)
{
	g_return_val_if_fail(dm != NULL, -1);

	int mask = 0;
	int begin_day, end_day;
	GList *bgi_list;
	GList *bf_list;
	BasketGlobalInfo *bgi;
	BasketFragment *bf;
	struct tm time_str = {0};
	struct tm *ptm;
	
	g_return_val_if_fail(end_time >= begin_time, -1);

	DISK_LOCK(dm);
	for(bgi_list = dm->bgi_list; bgi_list != NULL; bgi_list = g_list_next(bgi_list))
	{
		bgi = (BasketGlobalInfo *)bgi_list->data;
		if(!(chn_mask & (1<<bgi->bg.chn)))
			continue;
		// 判断时间是否在范围内
		if(end_time <= bgi->begin_time || begin_time >= bgi->end_time)
			continue;
		if((bgi->record_type & record_type) == 0)
			continue;
		// 判断时间是否在24小时内
		if(bgi->end_time - bgi->begin_time < 24 * 60 * 60)
		{
			ptm = localtime_r(&bgi->begin_time, &time_str);
			begin_day = ptm->tm_mday;
			ptm = localtime_r(&bgi->end_time, &time_str);
			end_day = ptm->tm_mday;
			if(begin_day == end_day)
			{
				mask |= 1<<(begin_day - 1);
				continue;
			}
		}
		for(bf_list = bgi->bf_list; bf_list != NULL; bf_list = g_list_next(bf_list))
		{
			bf = (BasketFragment *)bf_list->data;
			if(bf->iframe_num == 0 || bf->fragment_size == 0)
				continue;
			// 判断时间是否在范围内
			if(end_time <= bf->begin_time || begin_time >= bf->end_time/* || bf->begin_time == bf->end_time*/)
				continue;
			if((bf->record_type & record_type) == 0)
				continue;
			if(begin_time <= bf->begin_time && end_time >= bf->end_time)
			{
				ptm = localtime_r(&bf->begin_time, &time_str);
				mask |= 1<<(ptm->tm_mday - 1);
			}
		}
	}
	DISK_UNLOCK(dm);
	
	return mask;
}


ReadHandle strg_disk_read_start(DiskMng *dm, RecordQueryInfo *query_info, StorageFragmentInfo *sfi)
{
	g_return_val_if_fail(dm != NULL, NULL);
	g_return_val_if_fail(query_info->chn < MAX_CHN, NULL);
	g_return_val_if_fail(query_info->basket_idx < dm->dg.basket_num, NULL);

	BasketGlobalInfo *bgi;
	GList *bgi_list;
	ReadHandle read_handle;

	DISK_LOCK(dm);
	for(bgi_list = g_list_first(dm->bgi_list); bgi_list != NULL; bgi_list = g_list_next(bgi_list))
	{
		bgi = (BasketGlobalInfo *)bgi_list->data;
		if(bgi->basket_idx == query_info->basket_idx && bgi->bg.chn == query_info->chn)
			break;
	}
	if(bgi_list == NULL)
	{
		j_warning("Don't find the fragment");
		DISK_UNLOCK(dm);
		return NULL;
	}
	
	if((read_handle = strg_basket_read_start(dm, bgi, query_info, sfi)) == NULL)
	{
		j_warning("Basket read start failed.");
		DISK_UNLOCK(dm);
		return NULL;
	}
	j_debug("append read handle to list.\n");
	dm->read_list = g_list_append(dm->read_list, read_handle);
	DISK_UNLOCK(dm);
	
	return read_handle;
}

void strg_disk_read_stop(ReadHandle read_handle)
{
	g_return_if_fail(read_handle != NULL);
	BasketReadInfo *bri = (BasketReadInfo*)read_handle;
	g_return_if_fail(bri->bfi != NULL);
	g_return_if_fail(bri->bfi->disk_mng != NULL);
	DiskMng *dm = bri->bfi->disk_mng;

	DISK_LOCK(dm);
	dm->read_list = g_list_remove(dm->read_list, read_handle);
	j_debug("remove read handle from list.");
	strg_basket_read_stop(read_handle);
	DISK_UNLOCK(dm);
}

int strg_disk_read_set(ReadHandle read_handle, time_t t)
{
	g_return_val_if_fail(read_handle != NULL, -1);
	return strg_basket_read_set(read_handle, t);
}

int strg_disk_read_seek(ReadHandle read_handle, int seek_iframe_offset)
{
	g_return_val_if_fail(read_handle != NULL, -1);
	return strg_basket_read_seek(read_handle, seek_iframe_offset);
}

int strg_disk_read(ReadHandle read_handle, StorageReadCmd cmd, 
						 gpointer frame_buff, int buff_len)
{
	return strg_basket_read(read_handle, cmd, frame_buff, buff_len);
}

int strg_disk_read_valid(DiskMng *dm, ReadHandle read_handle)
{
	return (g_list_find(dm->read_list, read_handle) != NULL) ? 1 : 0;
}

int strg_disk_write_identifier(const gchar *dev_path)
{
	FILE *fp;
	DiskIdentifier disk_iden;
	GDate *d;
	
	fp = fopen(dev_path, "wb");
	if(fp == NULL)
	{
		j_warning("Open disk %s failed.", dev_path);
		return -1;
	}
	memset(&disk_iden, 0, sizeof(disk_iden));
	disk_iden.magic = DISK_IDENTIFIER_ID;
	d = g_date_new();
	g_date_set_time_t(d, time(NULL));
	g_date_strftime(disk_iden.time, 100, "%F", d);
	g_date_free(d);
	g_strlcpy(disk_iden.ver, DISK_IDENTIFIER_VER, sizeof(disk_iden.ver));
	disk_iden.checksum = calc_checksum(&disk_iden, sizeof(disk_iden)-sizeof(disk_iden.checksum));
	if(fwrite(&disk_iden, sizeof(disk_iden), 1, fp) != 1)
	{
		j_warning("Read disk identifier failed.");
		fclose(fp);
		return -1;
	}
	fclose(fp);

	return 0;
}

int strg_disk_is_our_format(const gchar *dev_path)
{
	g_return_val_if_fail(dev_path != NULL, -1);
	
	FILE *fp;
	DiskIdentifier disk_iden;
	unsigned long checksum;
	
	fp = fopen(dev_path, "rb");
	if(fp == NULL)
	{
		j_warning("Open disk %s failed.", dev_path);
		return -1;
	}
	if(fread(&disk_iden, sizeof(disk_iden), 1, fp) != 1)
	{
		j_warning("Read disk identifier failed.");
		fclose(fp);
		return -1;
	}
	fclose(fp);

	j_debug("The disk identifier magic is 0x%lx, my magic is 0x%x.", disk_iden.magic, DISK_IDENTIFIER_ID);
	if(disk_iden.magic != DISK_IDENTIFIER_ID)
	{
		j_warning("Disk identifier magic is wrong1111111.\n");
		return -1;
	}
	checksum = calc_checksum(&disk_iden, sizeof(disk_iden)-sizeof(disk_iden.checksum));
	if(checksum != disk_iden.checksum)
	{
		j_warning("The disk identifier checksum is wrong.\n");
		return -1;
	}

	return 0;
}

int strg_disk_global_format(const gchar *mount_path)
{
	g_return_val_if_fail(mount_path != NULL, -1);

	gchar path[PATH_LEN];
	int i;
	int fd;
	int global_len;
	int total_mb, free_mb;
	int basket_num;
	DiskGlobal dg;
	BasketGlobal bg;
	
	if(strg_disk_get_size(mount_path, &total_mb, &free_mb) < 0)
	{
		j_warning("Get disk size failed.");
		return -1;
	}
	j_message("disk total size: %dMB, free size: %dMB", total_mb, free_mb);
	/////////////////////////////////////////////////
	basket_num = free_mb * 98 / 100 / (BASKET_SIZE >> 20);
	//basket_num = 16;
	/////////////////////////////////////////////
	j_message("Create %d baskets", basket_num);
	global_len = DISK_GLOBAL_LEN + (BASKET_GLOBAL_LEN + BASKET_FRAGMENT_SIZE) * basket_num;
	j_message("disk global file size: %d", global_len);
	
	g_snprintf(path, sizeof(path), "%s/%s", mount_path, DISK_GLOBAL_NAME);

	fd = open(path, O_CREAT | O_WRONLY | O_DSYNC, 0666);
	// 预分配文件大小
	if(posix_fallocate(fd, SEEK_SET, global_len) < 0)
	{
		j_warning("fallocation file for disk global file failed.");
		close(fd);
		return -1;
	}

	// begin to write disk global file
	// first we create disk global struct
	memset(&dg, 0, sizeof(dg));
	dg.magic = DISK_GLOBAL_MAGIC;
	dg.use_cnt = 1;
	dg.basket_num = basket_num;
	g_strlcpy(dg.ver, DISK_GLOBAL_VERSION, sizeof(dg.ver));
	strg_disk_write_global(fd, &dg);

	memset(&bg, 0, sizeof(bg));
	bg.magic = BASKET_GLOBAL_MAGIC;
	bg.use_cnt = 0;
	bg.status = USE_STATUS_IDLE;
	bg.chn = -1;
	bg.fragment_num = 0;
	bg.checksum = calc_checksum(&bg, BASKET_GLOBAL_LEN - sizeof(bg.checksum));
	for(i = 0; i < basket_num; i++)
	{
		//lseek(fd, DISK_BG_OFT(i), SEEK_SET);
		strg_write(fd, &bg, sizeof(bg), DISK_BG_OFT(i));
	}
	close(fd);
	
	return basket_num;
}

void strg_disk_display_info(DiskMng *dm)
{
	g_return_if_fail(dm != NULL);
	
	int i;
	GList *bgi_list;
	BasketGlobalInfo *bgi;
	
	DISK_LOCK(dm);
	printf("print time: %ld\n", time(NULL));
	printf("disk index: %d\n",  dm->disk_idx);
	printf("dev_path: %s\n", dm->dev_path);
	printf("mount_path: %s\n", dm->mount_path);
	printf("total size: %d\n", dm->total_size_mb);
	printf("disk global fd: %d\n", dm->fd);
	printf("create_flag_file: %d\n", dm->create_flag_file);
	printf("============== DiskGlobal ==============\n");
	printf("use_cnt: %d\n", dm->dg.use_cnt);
	printf("basket_num: %d\n", dm->dg.basket_num);
	printf("bgi_list num: %d\n", g_list_length(dm->bgi_list));
	printf("bgi_new num: %d\n", g_list_length(dm->bgi_new));
	printf("bgi_old num: %d\n", g_list_length(dm->bgi_old));
	printf("total valid size: %llu\n", dm->total_valid_size);
	printf("free valid size: %llu\n", dm->free_valid_size);
	
	printf("============== bfi_cur ==============\n");
	for(i = 0; i < MAX_CHN; i++)
	{
		printf("chn bfi: %p\n", dm->bfi_cur[i]);
		if(dm->bfi_cur[i])
			strg_basket_display_bgi(dm->bfi_cur[i]->bgi);
	}

	printf("============== bgi_new ==============\n");
	for(bgi_list = g_list_first(dm->bgi_new); bgi_list != NULL; bgi_list = g_list_next(bgi_list))
	{
		bgi = (BasketGlobalInfo *)bgi_list->data;
		strg_basket_display_bgi(bgi);
	}
	printf("============== bgi_old ==============\n");
	for(bgi_list = g_list_first(dm->bgi_old); bgi_list != NULL; bgi_list = g_list_next(bgi_list))
	{
		bgi = (BasketGlobalInfo *)bgi_list->data;
		strg_basket_display_bgi(bgi);
	}
	DISK_UNLOCK(dm);
}


