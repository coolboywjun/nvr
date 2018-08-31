#ifndef __STRG_DISK_H__
#define __STRG_DISK_H__

#include <strg_basket.h>

#define DISK_IDENTIFIER_ID		0x4352564e   	/* NVRC */
#define DISK_IDENTIFIER_VER		"v1.0.0"

#define DISK_GLOBAL_ID	 		0x4b534944	/* DISK */
#define DISK_GLOBAL_FILE			"global"
#define DISK_WRITE_FLAG_FILE		"writeflag"
#define DISK_FORMAT_FLAG_FILE		"formatflag"


typedef struct DiskIdentifier
{
	unsigned long magic;
	char time[16];
	char ver[8];
	char res[32];
	unsigned long checksum;
}DiskIdentifier;

typedef struct DiskGlobal
{
	unsigned long magic;
	int 			use_cnt;
	int			basket_num;
	char			ver[8];
	char			res[8];
	unsigned long	checksum;
}DiskGlobal;
#define DISK_GLOBAL_MAGIC	0x4f4c4744			/* DGLO */
#define DISK_GLOBAL_LEN	(sizeof(DiskGlobal))
#define DISK_GLOBAL_OFT	(0)

#define DISK_GLOBAL_VERSION	DISK_IDENTIFIER_VER
#define DISK_GLOBAL_NAME	"global"
#define DISK_LOG_NAME		"log"

struct DiskMng
{
 	int				disk_idx;
	gchar			dev_path[PATH_LEN];
	gchar			mount_path[PATH_LEN];
	int				total_size_mb;
	int				free_size_mb;

	unsigned	long	long	total_valid_size;	// 有效的可以写入帧数据的大小
	unsigned long	long	free_valid_size;	// 有效的还未写入的大小
	
	DiskGlobal		dg;
	
	int				fd;
	int				log_fd;
	
	GList			*bgi_list;
	GList			*bgi_new;
	GList			*bgi_old;
	BasketFileInfo		*bfi_cur[MAX_CHN];

	GList			*read_list;
	
	gint 				create_flag_file;

	GMutex			mutex;
};

#define DISK_LOCK(dm)		(g_mutex_lock(&dm->mutex))
#define DISK_TRYLOCK(dm)	(g_mutex_trylock(&dm->mutex))
#define DISK_UNLOCK(dm)	(g_mutex_unlock(&dm->mutex))

int strg_disk_mount(const gchar* dev_path, const gchar *mount_path);


int strg_disk_umount(const gchar *mount_path);


DiskMng* strg_disk_open(gchar* dev_path, int disk_idx);


void strg_disk_close(DiskMng *dm);


int strg_disk_reset(DiskMng *dm, int overwrite);


int strg_disk_write_start(DiskMng *dm, int chn, StorageRecordType record_type, int overwrite);


void strg_disk_write_stop(DiskMng *dm, int chn);


int strg_disk_write(DiskMng *dm, int chn, guint64 cur_time,
					gpointer frame_data, int frame_size, 
					int frame_no, FrameType frame_type, EncodeType encode_type);


int strg_disk_write_set_type(DiskMng *dm, int chn, StorageRecordType type);


int strg_disk_query(DiskMng *dm, int chn_mask, 
						time_t begin_time, time_t end_time, StorageRecordType record_type,
						strg_query_func query_cb, gpointer data);


int strg_disk_find(DiskMng *dm, int chn_mask, 
						time_t begin_time, time_t end_time, StorageRecordType record_type);
						

ReadHandle strg_disk_read_start(DiskMng *dm, RecordQueryInfo *query_info, StorageFragmentInfo *sfi);


void strg_disk_read_stop(ReadHandle read_handle);


int strg_disk_read_set(ReadHandle read_handle, time_t t);


int strg_disk_read_seek(ReadHandle read_handle, int seek_iframe_offset);


int strg_disk_read(ReadHandle read_handle, StorageReadCmd cmd, 
						 gpointer frame_buff, int buff_len);


int strg_disk_read_valid(DiskMng *dm, ReadHandle read_handle);


int strg_disk_write_identifier(const gchar *dev_path);


int strg_disk_is_our_format(const gchar *dev_path);


int strg_disk_global_format(const gchar *mount_path);


void strg_disk_display_info(DiskMng *dm);



#endif
