#ifndef __STRG_BACKET_H__
#define __STRG_BACKET_H__

#include <strg_header.h>

#include <storage_interface.h>

#define BASKET_SIZE	(1ul * 1024 * 1024 * 1024)
//#define BASKET_SIZE				(16ul * 1024 * 1024)

#define BASKET_GLOBAL_MAGIC		0x4f4c4742		/* BGLO */
#define BASKET_FRAGMENT_MAGIC	0x41524642		/* BFRA */
#define BASKET_IFRAME_MAGIC		0x52464942		/* BIFR */
#define BASKET_FRAME_MAGIC		0x4d524642		/* BFRM */

#define BASKET_RESERVE_SIZE	0x400000
#define BASKET_SPACE_MIN		0x800000
#define BASKET_IFRAME_MIN		1000

typedef enum FrameType
{
	FRAME_TYPE_NONE = 0,
	FRAME_TYPE_I = 1,
	FRAME_TYPE_P = 2,
	FRAME_TYPE_B = 3,
	FRAME_TYPE_A = 4,
}FrameType;

typedef enum EncodeType
{
	ENCODE_TYPE_H264 = 0,
	ENCODE_TYPE_G711A = 1,
	ENCODE_TYPE_G711U = 2,
	ENCODE_TYPE_JPEG = 3,
}EncodeType;

typedef enum UseStatus
{
	USE_STATUS_IDLE 		= 0,
	USE_STATUS_WRITING 	= (1<<0),
	USE_STATUS_READING 	= (1<<1),
	USE_STATUS_FULL 		= (1<<2),
	USE_STATUS_ERR		= (1<<3),
}UseStatus;

enum
{
	ERR_SUCCESS 	=  0,
	ERR_SEEK 		= -1,
	ERR_READ 		= -2,
	ERR_WRITE 		= -3,
	ERR_MEMORY 	= -4,
	ERR_MAGIC 		= -5,
	ERR_CHECK	 	= -6,
	ERR_PARAM		= -7,
};

typedef struct BasketGlobal
{
	unsigned long 	magic;
	unsigned int 		use_cnt;
	UseStatus		status;
	int 				chn;
	unsigned int 		fragment_num;
	unsigned char 	res[16];
	unsigned long 	checksum;
}BasketGlobal;
#define BASKET_GLOBAL_LEN	(sizeof(BasketGlobal))
#define BASKET_GLOBAL_OFT	0
#define BASKET_GLOBAL_SIZE	BASKET_GLOBAL_LEN
#define BASKET_GLOBAL_ALLOC()			g_slice_new0(BasketGlobal)
#define BASKET_GLOBAL_FREE(bg_ptr)		g_slice_free(BasketGlobal, bg_ptr)

typedef struct BasketFragment
{
	unsigned long 	magic;
	UseStatus		status;
	unsigned int 		use_cnt;
	time_t 			begin_time;
	time_t 			end_time;
	int 				total_frame_cnt;
	int 				first_iframe_idx;
	int 				iframe_num;
	unsigned int 		begin_frame_pos;
	unsigned int 		fragment_size;
	StorageRecordType	 record_type;
	unsigned char		res[16];
	unsigned long 	checksum;
}BasketFragment;
#define BASKET_FRAGMENT_LEN	(sizeof(BasketFragment))
#define BASKET_FRAGMENT_OFT	BASKET_GLOBAL_SIZE
#define BASKET_FRAGMENT_NUM	1024
#define BASKET_FRAGMENT_SIZE	(BASKET_FRAGMENT_LEN * BASKET_FRAGMENT_NUM)
#define BASKET_FRAGMENT_ALLOC()			g_slice_new0(BasketFragment)
#define BASKET_FRAGMENT_FREE(bf_ptr)		g_slice_free(BasketFragment, bf_ptr)


typedef struct BasketIFrame
{
	unsigned long 	magic;
	unsigned int 		use_cnt;
	int 				frame_no;
	unsigned int 		frame_pos;
	struct timeval		ts;
	unsigned char 	res[16];
	unsigned long 	checksum;
}BasketIFrame;
#define BASKET_IFRAME_LEN	(sizeof(BasketIFrame))
#define BASKET_IFRAME_OFT	(BASKET_FRAGMENT_OFT + BASKET_FRAGMENT_SIZE)
#define BASKET_IFRAME_SIZE	(BASKET_SIZE / 16)
#define BASKET_IFRAME_NUM	(BASKET_IFRAME_SIZE / BASKET_IFRAME_LEN);


typedef struct BasketFrame
{
	unsigned long 	magic;
	int				use_cnt;
	int 				frame_no;
	FrameType		frame_type;
	unsigned int		frame_len;
	struct timeval		ts;
	unsigned long		pos[2];
	unsigned long		data_checksum;
	unsigned char		res[24];
	unsigned long		checksum;
}BasketFrame;
#define BASKET_FRAME_LEN	(sizeof(BasketFrame))
#define BASKET_FRAME_OFT	(BASKET_IFRAME_OFT + BASKET_IFRAME_SIZE)
#define BASKET_FRAME_SIZE	(BASKET_SIZE - BASKET_FRAME_OFT)


typedef struct BasketGlobalInfo
{
	gchar		basket_path[PATH_LEN];
	int 			basket_idx;
	int 			update_flag;
	BasketGlobal 	bg;
	GList		*bf_list;

	int			read_ref;
	
	unsigned int	free_space;
	unsigned int 	free_iframe;

	time_t		begin_time;
	time_t		end_time;
	StorageRecordType	 record_type;
}BasketGlobalInfo;
#define BASKET_GLOBAL_INFO_ALLOC()			g_slice_new0(BasketGlobalInfo)
#define BASKET_GLOBAL_INFO_FREE(bgi_ptr)		g_slice_free(BasketGlobalInfo, bgi_ptr)


typedef struct DiskMng DiskMng;

typedef struct BasketFileInfo
{
	DiskMng	*disk_mng;
	int				fd;		// global fd & fragment fd & iframe fd
	int				ffd;		// frame fd
	
	BasketGlobalInfo	*bgi;
	BasketFragment	*cur_bf;
	int				cur_bf_idx;

	guint64 first_vts;		// ms
	guint64 first_ipc_vts;	// ms
	
	guint64 first_ats;		// ms
	guint64 first_ipc_ats;	// ms
	
}BasketFileInfo;


typedef struct BasketReadInfo
{
	BasketFileInfo *bfi;
	BasketFragment *bf;
	int begin_iframe_idx;	// 需要播放的第一个iframe可能跟bf中的begin_iframe_idx不同，所以记录在这
	int iframe_idx;
	int iframe_num;
	int read_pos;
	int read_cnt;
	int begin_pos;
	int fragment_size;
}BasketReadInfo;
#define BASKET_READ_INFO_ALLOC()		(g_malloc0(sizeof(BasketReadInfo)))
#define BASKET_READ_INFO_FREE(bri_ptr)	(g_free(bri_ptr))

typedef void* ReadHandle;

typedef int strg_query_func(RecordQueryInfo *rqi, gpointer data);

							
int strg_basket_write_start(BasketFileInfo *bfi, int chn, StorageRecordType record_type);

void strg_basket_write_stop(BasketFileInfo *bfi);

int strg_basket_write(BasketFileInfo *bfi, guint64 cur_time,
						gpointer frame_data, int frame_size,
						int frame_no, FrameType frame_type, EncodeType encode_type);

ReadHandle strg_basket_read_start(DiskMng *dm, BasketGlobalInfo *bgi, RecordQueryInfo *query_info, StorageFragmentInfo *sfi);

void strg_basket_read_stop(ReadHandle read_handle);

int strg_basket_read_set(ReadHandle read_handle, time_t t);

int strg_basket_read_seek(ReadHandle read_handle, int seek_iframe_offset);

int strg_basket_read(ReadHandle read_handle, StorageReadCmd cmd, gpointer frame_buff, int buff_len);

int strg_basket_query(DiskMng *dm, BasketGlobalInfo *bgi, BasketFragment *bf, int bf_idx,
						time_t begin_time, time_t end_time, StorageRecordType record_type,
						strg_query_func query_cb, gpointer data);

int strg_basket_reset(BasketFileInfo *bfi, int chn, int use_cnt);

int strg_basket_format(const char *mount_path, int basket_idx);

int strg_basket_scan_fragment(BasketGlobalInfo *bgi, BasketFragment *bf, int bf_idx);

int strg_basket_scan(BasketGlobalInfo *bgi);

BasketFileInfo* strg_basket_open(struct DiskMng *dm, BasketGlobalInfo *bgi);

void strg_basket_close(BasketFileInfo *bfi);

void strg_basket_diskplay_bif(BasketIFrame *bif);

void strg_basket_display_bf(BasketFragment *bf);

void strg_basket_display_bg(BasketGlobal *bg);

void strg_basket_display_bgi(BasketGlobalInfo *bgi);

#endif

