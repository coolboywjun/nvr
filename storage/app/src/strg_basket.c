
#include <strg_header.h>

#include <strg_common.h>
#include <strg_basket.h>
#include <strg_disk.h>
#include <av_pool.h>


typedef struct BasketReadList
{
	BasketReadInfo *bri;
	time_t			start_time;
}BasketReadList;

extern gint g_record_minutes;


static int strg_basket_check_global(BasketGlobal *bg)
{
	g_return_val_if_fail(bg->magic == BASKET_GLOBAL_MAGIC, ERR_MAGIC);
	g_return_val_if_fail(calc_checksum(bg, BASKET_GLOBAL_LEN - sizeof(bg->checksum)) == bg->checksum, ERR_CHECK);
	return ERR_SUCCESS;
}

static int strg_basket_write_global(int fd, BasketGlobal *bg)
{
	g_assert(bg != NULL);
	g_assert(bg->magic == BASKET_GLOBAL_MAGIC);
	//g_return_val_if_fail(lseek(fd, BASKET_GLOBAL_OFT, SEEK_SET) >= 0, ERR_SEEK);
	bg->checksum = calc_checksum(bg, BASKET_GLOBAL_LEN - sizeof(bg->checksum));
	g_return_val_if_fail(strg_write(fd, bg, BASKET_GLOBAL_LEN, BASKET_GLOBAL_OFT) == BASKET_GLOBAL_LEN, ERR_WRITE);
	return BASKET_GLOBAL_LEN;
}

static int strg_basket_read_global(int fd, BasketGlobal *bg)
{
	g_assert(bg != NULL);
	//g_return_val_if_fail(lseek(fd, BASKET_GLOBAL_OFT, SEEK_SET) >= 0, ERR_SEEK);
	g_return_val_if_fail(strg_read(fd, bg, BASKET_GLOBAL_LEN, BASKET_GLOBAL_OFT) == BASKET_GLOBAL_LEN, ERR_READ);
	g_return_val_if_fail(strg_basket_check_global(bg) == 0, ERR_CHECK);
	return BASKET_GLOBAL_LEN;
}

static int strg_basket_check_fragment(BasketFragment *bf)
{
	g_return_val_if_fail(bf->magic == BASKET_FRAGMENT_MAGIC, ERR_MAGIC);
	g_return_val_if_fail(calc_checksum(bf, BASKET_FRAGMENT_LEN- sizeof(bf->checksum)) == bf->checksum, ERR_CHECK);
	return ERR_SUCCESS;
}

static int strg_basket_write_fragment(int fd, BasketFragment *bf, int idx)
{
	g_assert(bf != NULL);
	g_assert(bf->magic == BASKET_FRAGMENT_MAGIC);
	//g_return_val_if_fail(lseek(fd, BASKET_FRAGMENT_OFT + BASKET_FRAGMENT_LEN * idx, SEEK_SET) >= 0, ERR_SEEK);
	bf->checksum = calc_checksum(bf, BASKET_FRAGMENT_LEN - sizeof(bf->checksum));
	g_return_val_if_fail(strg_write(fd, bf, BASKET_FRAGMENT_LEN, BASKET_FRAGMENT_OFT + BASKET_FRAGMENT_LEN * idx) == BASKET_FRAGMENT_LEN, ERR_WRITE);
	return BASKET_FRAGMENT_LEN;
}

static int strg_basket_read_fragment(int fd, BasketFragment *bf, int idx)
{
	g_assert(bf != NULL);
	//g_return_val_if_fail(lseek(fd, BASKET_FRAGMENT_OFT + BASKET_FRAGMENT_LEN * idx, SEEK_SET) >= 0, ERR_SEEK);
	g_return_val_if_fail(strg_read(fd, bf, BASKET_FRAGMENT_LEN, BASKET_FRAGMENT_OFT + BASKET_FRAGMENT_LEN * idx) == BASKET_FRAGMENT_LEN, ERR_READ);
	g_return_val_if_fail(strg_basket_check_fragment(bf) == 0, ERR_CHECK);
	return BASKET_GLOBAL_LEN;
}

static int strg_basket_check_iframe(BasketIFrame *bif)
{
	g_return_val_if_fail(bif->magic == BASKET_IFRAME_MAGIC, ERR_MAGIC);
	g_return_val_if_fail(calc_checksum(bif, BASKET_IFRAME_LEN - sizeof(unsigned int)) == bif->checksum, ERR_CHECK);
	return ERR_SUCCESS;
}

static int strg_basket_write_iframe(int fd, BasketIFrame *bif, int idx)
{
	g_assert(bif != NULL);
	g_assert(bif->magic == BASKET_IFRAME_MAGIC);
	//g_return_val_if_fail(lseek(fd, BASKET_IFRAME_OFT + BASKET_IFRAME_LEN * idx, SEEK_SET) >= 0, ERR_SEEK);
	bif->checksum = calc_checksum(bif, BASKET_IFRAME_LEN - sizeof(bif->checksum));
	g_return_val_if_fail(strg_write(fd, bif, BASKET_IFRAME_LEN, BASKET_IFRAME_OFT + BASKET_IFRAME_LEN * idx) == BASKET_IFRAME_LEN, ERR_WRITE);
	return BASKET_IFRAME_LEN;
}

static int strg_basket_read_iframe(int fd, BasketIFrame *bif, int idx)
{
	g_assert(bif != NULL);
	//g_return_val_if_fail(lseek(fd, BASKET_IFRAME_OFT + BASKET_IFRAME_LEN * idx, SEEK_SET) >= 0, ERR_SEEK);
	g_return_val_if_fail(strg_read(fd, bif, BASKET_IFRAME_LEN, BASKET_IFRAME_OFT + BASKET_IFRAME_LEN * idx) == BASKET_IFRAME_LEN, ERR_READ);
	g_return_val_if_fail(strg_basket_check_iframe(bif) == 0, ERR_CHECK);
	return BASKET_IFRAME_LEN;
}

static int strg_basket_check_frame(BasketFrame *bframe)
{
	g_return_val_if_fail(bframe->magic, BASKET_FRAME_MAGIC);
	g_return_val_if_fail(calc_checksum(bframe, BASKET_FRAME_LEN - sizeof(unsigned int)) == bframe->checksum, ERR_CHECK);
	return ERR_SUCCESS;
}

static void strg_basket_calc_frame_data_checksum(BasketFrame *bframe, void *frame_data, int frame_len)
{
	int i;
	unsigned long data;
	
	bframe->pos[0] = ((unsigned long)random()) % (frame_len - sizeof(unsigned long));
	bframe->pos[1] = ((unsigned long)random()) % (frame_len - sizeof(unsigned long));

	if(frame_len < 8)
		return;
	memcpy(&data, frame_data + 4, sizeof(data));
	bframe->data_checksum = data;
	memcpy(&data, frame_data + frame_len - sizeof(data), sizeof(data));
	bframe->data_checksum += data;
	for(i = 0; i < 2; i++)
	{
		memcpy(&data, frame_data + bframe->pos[i], sizeof(data));
		bframe->data_checksum += data;
	}
}

static int strg_basket_check_frame_data(BasketFrame *bframe, void *frame_data, int frame_len)
{
	int i;
	unsigned long data;
	unsigned long checksum = 0; 
	
	memcpy(&data, frame_data + 4, sizeof(data));
	checksum = data;
	memcpy(&data, frame_data + frame_len - sizeof(data), sizeof(data));
	checksum += data;
	for(i = 0 ; i < 2; i++)
	{
		if(bframe->pos[i] >= frame_len - sizeof(data))
			return 0;
		memcpy(&data, frame_data + bframe->pos[i], sizeof(data));
		checksum += data;
	}
	if(checksum == bframe->data_checksum)
		return 1;
	else
		return 0;
}

static int strg_basket_write_frame(int fd, int pos, BasketFrame *bframe, void *frame_data, int frame_len)
{
	g_assert(bframe != NULL);
	g_assert(frame_data != NULL);
	g_assert(frame_len > 0);
	g_assert(bframe->magic == BASKET_FRAME_MAGIC);
	strg_basket_calc_frame_data_checksum(bframe, frame_data, frame_len);
	//g_return_val_if_fail(lseek(fd, pos, SEEK_SET) >= 0, ERR_SEEK);
	bframe->checksum = calc_checksum(bframe, BASKET_FRAME_LEN - sizeof(bframe->checksum));
	g_return_val_if_fail(strg_write(fd, bframe, BASKET_FRAME_LEN, pos) == BASKET_FRAME_LEN, ERR_WRITE);
	g_return_val_if_fail(strg_write(fd, frame_data, frame_len, pos + BASKET_FRAME_LEN) == frame_len, ERR_WRITE);
	return BASKET_FRAME_LEN + frame_len;
}

static int strg_basket_read_frame(int fd, int pos, BasketFrame *bframe, void *frame_buff, int buff_len)
{
	g_assert(bframe != NULL);
	g_assert(frame_buff != NULL);
	//g_return_val_if_fail(lseek(fd, pos, SEEK_SET) >= 0, ERR_SEEK);
	g_return_val_if_fail(strg_read(fd, bframe, BASKET_FRAME_LEN, pos) == BASKET_FRAME_LEN, ERR_READ);
	g_return_val_if_fail(strg_basket_check_frame(bframe) == 0, ERR_CHECK);
	g_return_val_if_fail(bframe->frame_len <= buff_len, ERR_PARAM);
	g_return_val_if_fail(strg_read(fd, frame_buff, bframe->frame_len, pos + BASKET_FRAME_LEN) == bframe->frame_len, ERR_READ);
	g_return_val_if_fail(strg_basket_check_frame_data(bframe, frame_buff, bframe->frame_len), ERR_CHECK);
	return BASKET_FRAME_LEN + bframe->frame_len;
}

BasketGlobalInfo *strg_basket_new_global_info(const gchar *mount_path, int basket_idx, int chn, int use_cnt)
{
	BasketGlobalInfo *bgi;

	bgi = BASKET_GLOBAL_INFO_ALLOC();
	g_snprintf(bgi->basket_path, sizeof(bgi->basket_path), "%s/basket%d", mount_path, basket_idx);
	bgi->basket_idx = basket_idx;
	bgi->update_flag = 0;
	bgi->bg.magic = BASKET_GLOBAL_MAGIC;
	bgi->bg.use_cnt= use_cnt;
	bgi->bg.status = USE_STATUS_IDLE;
	bgi->bg.chn = chn;
	bgi->bg.fragment_num = 0;
	bgi->free_space = BASKET_FRAME_SIZE;
	bgi->free_iframe = BASKET_IFRAME_NUM;

	return bgi;
}

static void  __strg_basket_bf_list_free(gpointer data)
{
	BASKET_FRAGMENT_FREE(data);
}

static BasketFragment *strg_basket_new_fragment(BasketGlobalInfo *bgi, int first_iframe_idx, int begin_frame_pos, StorageRecordType record_type)
{
	g_assert(bgi != NULL);
	
	BasketFragment *bf;

	bf = BASKET_FRAGMENT_ALLOC();
	bf->magic = BASKET_FRAGMENT_MAGIC;
	bf->status = USE_STATUS_IDLE;
	bf->use_cnt = bgi->bg.use_cnt;
	bf->begin_time = time(NULL);
	bf->end_time = bf->begin_time;
	bf->total_frame_cnt = 0;
	bf->first_iframe_idx = first_iframe_idx;
	bf->begin_frame_pos = begin_frame_pos;
	bf->fragment_size = 0;
	bf->record_type = record_type | RECORD_VIDEO;
	
	return bf;
}

static BasketFragment *strg_basket_create_fragment(BasketGlobalInfo *bgi, StorageRecordType record_type)
{
	g_assert(bgi != NULL);
	BasketFragment *bf, *bf_new;
	GList *list;
	
	if(bgi->free_space < BASKET_SPACE_MIN || bgi->bg.fragment_num == BASKET_FRAGMENT_NUM || bgi->free_iframe < BASKET_IFRAME_MIN)
	{
		j_warning("The basket%d don't have enough space to write.", bgi->basket_idx);
		return NULL;
	}
	if(bgi->bg.fragment_num == BASKET_FRAGMENT_NUM)
	{
		j_warning("Basket don't have enough fragment.");
		return NULL;
	}
	
	list = g_list_last(bgi->bf_list);
	if(list && list->data)
	{
		bf = (BasketFragment *)list->data;
		bf_new = strg_basket_new_fragment(bgi, 
					bf->first_iframe_idx + bf->iframe_num,
					bf->begin_frame_pos + bf->fragment_size, record_type);
	}
	else
	{
		bf_new = strg_basket_new_fragment(bgi, 0, BASKET_FRAME_OFT, record_type);
	}
	
	return bf_new;
}

static inline void strg_basket_free_fragment(BasketFragment *bf)
{
	BASKET_FRAGMENT_FREE(bf);
}

static void strg_basket_free_all_fragment(BasketGlobalInfo *bgi)
{
	g_assert(bgi != NULL);

	g_list_free_full(bgi->bf_list, __strg_basket_bf_list_free);
	bgi->bf_list = NULL;
}

static void strg_basket_update_fragment(BasketFileInfo *bfi, time_t cur_time, 
										int frame_size, FrameType frame_type, 
										EncodeType encode_type)
{
	g_assert(bfi != NULL);

	BasketGlobalInfo *bgi = bfi->bgi;
	
/*	
	if(cur_time - bfi->cur_bf->end_time == 0 || cur_time - bfi->cur_bf->end_time == 1)
		bfi->cur_bf->end_time = cur_time;
	else if(cur_time - bfi->cur_bf->end_time > 1)
		bfi->cur_bf->end_time = bfi->cur_bf->begin_time;
*/
	if(!bfi->cur_bf->total_frame_cnt)
		bfi->cur_bf->begin_time = cur_time;
	if(cur_time - bfi->cur_bf->end_time > 0)
		bfi->cur_bf->end_time = cur_time;
	bfi->cur_bf->total_frame_cnt++;
	if(frame_type == FRAME_TYPE_I)
		bfi->cur_bf->iframe_num++;
	else if(frame_type == FRAME_TYPE_A)
	{
		if(encode_type == ENCODE_TYPE_G711A)
			bfi->cur_bf->record_type |= RECORD_AUDIO_G711A;
		else
			bfi->cur_bf->record_type |= RECORD_AUDIO_G711U;
	}
	bfi->cur_bf->fragment_size += BASKET_FRAME_LEN + frame_size;

	
	if(bgi->begin_time > cur_time || bgi->begin_time == 0)
		bgi->begin_time = cur_time;
	if(bgi->end_time < cur_time)
		bgi->end_time = cur_time;
}

int strg_basket_write_start(BasketFileInfo *bfi, int chn, StorageRecordType record_type)
{
	g_return_val_if_fail(bfi != NULL, -1);
	g_return_val_if_fail(bfi->bgi != NULL, -1);
	
	BasketGlobalInfo *bgi;
	BasketFragment	*bf;
	int bf_idx;
	
	bgi = bfi->bgi;
	if(bgi->bg.chn != chn)
	{
		j_warning("The chn is not equal the previous chn");
		return -1;
	}
	
	bf_idx = g_list_length(bgi->bf_list);
	if((bf = strg_basket_create_fragment(bgi, record_type)) == NULL)
	{
		j_warning("Create fragment failed.");
		return -1;
	}
	bf->status |= USE_STATUS_WRITING;
	if(strg_basket_write_fragment(bfi->fd, bf, bf_idx) < 0)
	{
		j_warning("Write fragment failed");
		strg_basket_free_fragment(bf);
		return -1;
	}
	bgi->bg.status |= USE_STATUS_WRITING;
	bgi->bg.fragment_num++;
	if(strg_basket_write_global(bfi->fd, &bgi->bg) < 0)
	{
		j_warning("Write global failed");
		strg_basket_free_fragment(bf);
		bgi->bg.status &= (~USE_STATUS_WRITING);
		bgi->bg.fragment_num--;
		return -1;
	}
	fdatasync(bfi->fd);
	//fdatasync(bfi->ffd);
	bgi->record_type |= record_type;
	bgi->bf_list = g_list_append(bgi->bf_list, bf);
	bfi->cur_bf = bf;
	bfi->cur_bf_idx = bf_idx;

	return 0;
}

void strg_basket_write_stop(BasketFileInfo *bfi)
{
	g_return_if_fail(bfi->cur_bf != NULL);

	bfi->cur_bf->status &= (~USE_STATUS_WRITING);
	strg_basket_write_fragment(bfi->fd, bfi->cur_bf, bfi->cur_bf_idx);
	bfi->bgi->bg.status &= (~USE_STATUS_WRITING);
	strg_basket_write_global(bfi->fd, &bfi->bgi->bg);
	fdatasync(bfi->fd);
	fdatasync(bfi->ffd);
	bfi->cur_bf = NULL;
	bfi->cur_bf_idx = 0;
}

int strg_basket_write(BasketFileInfo *bfi, guint64 cur_time,
						gpointer frame_data, int frame_size,
						int frame_no, FrameType frame_type, EncodeType encode_type)
{
	g_return_val_if_fail(bfi != NULL, -1);
	g_return_val_if_fail(bfi->cur_bf != NULL, -1);
	g_return_val_if_fail(frame_data != NULL, -1);
	g_return_val_if_fail(frame_size != 0, -1);
	
	BasketFragment *bf;
	BasketIFrame	bif;
	BasketFrame	bframe;
	AV_FRAME_HEAD_S *hdr = (AV_FRAME_HEAD_S *)frame_data;
	
	bf = bfi->cur_bf;
	// 如果 是I帧，判断是否还有BASKET_IFRAME_LEN的剩余空间可以写入
	if(frame_type == FRAME_TYPE_I && 
		(bfi->bgi->free_iframe == 0 || bfi->bgi->free_space < BASKET_RESERVE_SIZE))
	{
		if(bfi->bgi->free_iframe == 0)
			j_warning("Don't have enough iframe struct space!!!");
		if(bfi->bgi->free_space < BASKET_RESERVE_SIZE)
			j_warning("Don't have enough frame space!!!");
		bfi->bgi->bg.status |= USE_STATUS_FULL;
		return 0;
	}
	if(bfi->bgi->free_space < BASKET_FRAME_LEN + frame_size)
	{
		j_warning("Don't have enough frame space!!!");
		bfi->bgi->bg.status |= USE_STATUS_FULL;
		return 0;
	}
	if(g_atomic_int_get(&g_record_minutes) > 0)
	{
		if(bf->end_time - bf->begin_time >= g_atomic_int_get(&g_record_minutes) * 60)
		{
			j_warning("The fragment have 15 minutes!, begin:%ld, end:%ld, minutes:%d", 
				bf->begin_time, bf->end_time, g_record_minutes);
			return 0;
		}
	}
	
	bframe.magic = BASKET_FRAME_MAGIC;
	bframe.use_cnt = bfi->bgi->bg.use_cnt;
	bframe.frame_no = frame_no;
	bframe.frame_type = frame_type;
	bframe.frame_len = frame_size;
	if(frame_type == FRAME_TYPE_A)
	{
		if(bfi->first_ats == 0)
		{
			//gettimeofday(&bframe.ts, NULL);
			//bfi->first_ats = (guint64)(bframe.ts.tv_sec) * 1000 + (guint64)(bframe.ts.tv_usec) / 1000;
			//bfi->first_ipc_ats = hdr->u64TimeStamp / 1000;
			bfi->first_ats = cur_time;
			bfi->first_ipc_ats = hdr->u64TimeStamp / 1000;
			bframe.ts.tv_sec = cur_time / 1000;
			bframe.ts.tv_usec = (cur_time % 1000) * 1000;
		}
		else
		{
			guint64 t = bfi->first_ats + (hdr->u64TimeStamp / 1000 - bfi->first_ipc_ats);
			bframe.ts.tv_sec = t / 1000;
			bframe.ts.tv_usec = (t % 1000) * 1000;
		}
	}
	else
	{
		if(bfi->first_vts == 0)
		{
			//gettimeofday(&bframe.ts, NULL);
			//bfi->first_vts = (guint64)(bframe.ts.tv_sec) * 1000 + (guint64)(bframe.ts.tv_usec) / 1000;
			//bfi->first_ipc_vts = hdr->u64TimeStamp / 1000;
			bfi->first_vts = cur_time;
			bfi->first_ipc_vts = hdr->u64TimeStamp / 1000;
			bframe.ts.tv_sec = cur_time / 1000;
			bframe.ts.tv_usec = (cur_time % 1000) * 1000;
		}
		else
		{
			guint64 t = bfi->first_vts + (hdr->u64TimeStamp / 1000 - bfi->first_ipc_vts);
			bframe.ts.tv_sec = t / 1000;
			bframe.ts.tv_usec = (t % 1000) * 1000;
			//j_debug("video sec: u64TimeStamp: %llu, t: %llu, %ld, usec: %ld", hdr->u64TimeStamp, t, bframe.ts.tv_sec, bframe.ts.tv_usec);
		}
	}
	if(strg_basket_write_frame(bfi->ffd, bf->begin_frame_pos + bf->fragment_size, 
								&bframe, frame_data, frame_size) < 0)
	{
		j_warning("Write frame failed");
		return -1;
	}
	
	if(frame_type == FRAME_TYPE_I)
	{
		bif.magic = BASKET_IFRAME_MAGIC;
		bif.use_cnt = bfi->bgi->bg.use_cnt;
		bif.frame_no = frame_no;
		bif.frame_pos = bf->begin_frame_pos + bf->fragment_size;
		bif.ts = bframe.ts;
		//memcpy(&bif->ts, &bframe.ts, sizeof(bif->ts));
		if(strg_basket_write_iframe(bfi->fd, &bif, bf->first_iframe_idx + bf->iframe_num) < 0)
		{
			j_warning("Write iframe failed");
			return -1;
		}
	}
	strg_basket_update_fragment(bfi, cur_time/1000, frame_size, frame_type, encode_type);
	bfi->bgi->free_space -= BASKET_FRAME_LEN + frame_size;
	if(frame_type == FRAME_TYPE_I)
		bfi->bgi->free_iframe--;
	bfi->bgi->update_flag = 1;
	
	return frame_size;
}

ReadHandle strg_basket_read_start(DiskMng *dm, BasketGlobalInfo *bgi, RecordQueryInfo *query_info, StorageFragmentInfo *sfi)
{
	g_return_val_if_fail(dm != NULL, NULL);
	g_return_val_if_fail(bgi != NULL, NULL);
	g_return_val_if_fail(sfi != NULL, NULL);
	g_return_val_if_fail(query_info->bf_idx < bgi->bg.fragment_num, NULL);

	BasketFileInfo *bfi;
	BasketFragment *bf;
	BasketIFrame bif;
	BasketReadInfo *bri;
	GList *bf_list;
	
	bf_list = g_list_nth(bgi->bf_list, query_info->bf_idx);
	g_return_val_if_fail(bf_list != NULL, NULL);
	bf = (BasketFragment *)bf_list->data;
//	g_return_val_if_fail(bf->first_iframe_idx == begin_iframe_idx, NULL);
	g_return_val_if_fail((bfi = strg_basket_open(dm, bgi)) != NULL, NULL);
	if(strg_basket_read_iframe(bfi->fd, &bif, query_info->begin_iframe_idx) != BASKET_IFRAME_LEN)
	{
		j_warning("Read iframe failed");
		strg_basket_close(bfi);
		return NULL;
	}
	bri = BASKET_READ_INFO_ALLOC();
	bri->bfi = bfi;
	bri->bf = bf;
	bri->begin_iframe_idx = query_info->begin_iframe_idx;
	bri->iframe_idx = query_info->begin_iframe_idx;
	bri->iframe_num = query_info->iframe_num;
	bri->read_pos = bif.frame_pos;
	bri->read_cnt = 0;
	bri->begin_pos = bif.frame_pos;
	bri->fragment_size = query_info->fragment_size;

	sfi->begin_time = bf->begin_time;
	sfi->end_time = bf->end_time;
	sfi->total_frame_cnt = bf->total_frame_cnt;
	sfi->fragment_size = bf->fragment_size;
	sfi->record_type = bf->record_type;	
	
	bgi->bg.status |= USE_STATUS_READING;
	g_atomic_int_inc(&bgi->read_ref);
	
	return (ReadHandle)bri;
}

void strg_basket_read_stop(ReadHandle read_handle)
{
	g_return_if_fail(read_handle != 0);

	BasketReadInfo *bri = (BasketReadInfo *)read_handle;
	
	if(g_atomic_int_dec_and_test(&bri->bfi->bgi->read_ref))
	{
		bri->bfi->bgi->bg.status &= ~USE_STATUS_READING;
	}
	strg_basket_close(bri->bfi);
	BASKET_READ_INFO_FREE(bri);
}

int strg_basket_read_set(ReadHandle read_handle, time_t t)
{
	g_return_val_if_fail(read_handle != NULL, -1);

	BasketReadInfo *bri = (BasketReadInfo *)read_handle;
	BasketFileInfo *bfi = bri->bfi;
	BasketIFrame bif, bif1, bif2;
	int iframe_idx, idx;
	time_t begin_time, end_time;
	
	if(t <= bri->bf->begin_time)
	{
		if(strg_basket_read_iframe(bfi->fd, &bif, bri->begin_iframe_idx) != BASKET_IFRAME_LEN)
		{
			j_warning("Read iframe failed");
			return -1;
		}
		bri->iframe_idx = bri->begin_iframe_idx;
		bri->read_pos = bif.frame_pos;
		return 0;
	}
	else if(t >= bri->bf->end_time)
	{
		iframe_idx = bri->begin_iframe_idx + bri->iframe_num - 1;
		if(strg_basket_read_iframe(bfi->fd, &bif, iframe_idx) != BASKET_IFRAME_LEN)
		{
			j_warning("Read iframe failed");
			return -1;
		}
		bri->iframe_idx = iframe_idx;
		bri->read_pos = bif.frame_pos;
		return 0;
	}
	
	if(strg_basket_read_iframe(bfi->fd, &bif, bri->begin_iframe_idx) != BASKET_IFRAME_LEN)
	{
		j_warning("Read iframe failed");
		return -1;
	}
	begin_time = bif.ts.tv_sec;
	if(t < begin_time)
		t = begin_time;
		
	if(strg_basket_read_iframe(bfi->fd, &bif, bri->begin_iframe_idx + bri->iframe_num - 1) != BASKET_IFRAME_LEN)
	{
		j_warning("Read iframe failed");
		return -1;
	}
	end_time = bif.ts.tv_sec;
	if(t > end_time)
		t = end_time;
	// 计算设置时间的iframe位置
	if(end_time - begin_time > 0)
		iframe_idx = (t - begin_time) * (bri->iframe_num - 1) / (end_time - begin_time) + bri->begin_iframe_idx;
	else
		iframe_idx = bri->begin_iframe_idx;
	if(strg_basket_read_iframe(bfi->fd, &bif, iframe_idx) != BASKET_IFRAME_LEN)
	{
		j_warning("Read iframe failed");
		return -1;
	}
	
	if(bif.ts.tv_sec == t)
	{
		bri->iframe_idx = iframe_idx;
		bri->read_pos = bif.frame_pos;
		return 0;
	}
	else if(bif.ts.tv_sec < t)
	{
		memcpy(&bif1, &bif, BASKET_IFRAME_LEN);
		for(idx = iframe_idx + 1; idx < bri->begin_iframe_idx + bri->iframe_num; idx++)
		{
			if(strg_basket_read_iframe(bfi->fd, &bif2, idx) != BASKET_IFRAME_LEN)
			{
				j_warning("Read iframe failed");
				return -1;
			}
			//j_debug("bif ts: %ld, set time: %ld", bif2.ts.tv_sec, t);
			if(bif2.ts.tv_sec > t)
			{
				bri->iframe_idx = idx - 1;
				bri->read_pos = bif1.frame_pos;
				break;
			}
			else if(bif2.ts.tv_sec == t)
			{
				bri->iframe_idx = idx;
				bri->read_pos = bif2.frame_pos;
				break;
			}
			else
			{
				memcpy(&bif1, &bif2, BASKET_IFRAME_LEN);
			}
		}
		if(idx == bri->begin_iframe_idx + bri->iframe_num)
		{
			bri->iframe_idx = bri->begin_iframe_idx + bri->iframe_num - 1;
			bri->read_pos = bif1.frame_pos;
		}
		return 0;
	}
	else /* if(bif.ts.tv_sec > t) */
	{
		memcpy(&bif1, &bif, BASKET_IFRAME_LEN);
		for(idx = iframe_idx - 1; idx >= bri->begin_iframe_idx; idx--)
		{
			if(strg_basket_read_iframe(bfi->fd, &bif2, idx) != BASKET_IFRAME_LEN)
			{
				j_warning("Read iframe failed");
				return -1;
			}
//			j_debug("bif ts: %ld, set time: %ld", bif2.ts.tv_sec, t);
			if(bif2.ts.tv_sec <= t)
			{
				bri->iframe_idx = idx;
				bri->read_pos = bif2.frame_pos;
				break;
			}
			else
			{
				memcpy(&bif1, &bif2, BASKET_IFRAME_LEN);
			}
		}
		if(idx == bri->begin_iframe_idx - 1)
		{
			bri->iframe_idx = bri->begin_iframe_idx;
			bri->read_pos = bif1.frame_pos;
		}
		return 0;
	}
	
	return 0;
}

int strg_basket_read_seek(ReadHandle read_handle, int seek_iframe_offset)
{
	g_return_val_if_fail(read_handle != NULL, -1);

	BasketReadInfo *bri = (BasketReadInfo *)read_handle;
	BasketFileInfo *bfi = bri->bfi;
	BasketIFrame bif;
	int iframe_idx;

	if(bri->iframe_idx + seek_iframe_offset >= bri->iframe_idx + bri->iframe_num)
		iframe_idx = bri->iframe_idx + bri->iframe_num - 1;
	else if(bri->iframe_idx + seek_iframe_offset < bri->begin_iframe_idx)
		iframe_idx = bri->begin_iframe_idx;
	else
		iframe_idx = bri->iframe_idx + seek_iframe_offset;
	if(strg_basket_read_iframe(bfi->fd, &bif, iframe_idx) != BASKET_IFRAME_LEN)
	{
		j_warning("Read iframe failed");
		return -1;
	}
	bri->iframe_idx = iframe_idx;
	bri->read_pos = bif.frame_pos;
	
	return 0;
}


int strg_basket_read(ReadHandle read_handle, StorageReadCmd cmd, gpointer frame_buff, int buff_len)
{
	g_return_val_if_fail(read_handle != 0, -1);
	g_return_val_if_fail(frame_buff != NULL, -1);
	g_return_val_if_fail(buff_len > 0, -1);

	int len;
	int iframe_oft;
	BasketFrame bframe;
	BasketIFrame bif;
	BasketReadInfo *bri = (BasketReadInfo *)read_handle;		
	StorageFrameHdr *frame_hdr = (StorageFrameHdr*)frame_buff;
	int frame_hdr_len = sizeof(StorageFrameHdr);
	
	if(cmd == READ_NORMAL)
	{
		if(bri->read_pos >= bri->begin_pos + bri->fragment_size)
			return 0;
		g_return_val_if_fail((len = strg_basket_read_frame(bri->bfi->fd, bri->read_pos, &bframe,
							frame_buff+frame_hdr_len, buff_len-frame_hdr_len)) > 0, -1);
		frame_hdr->ts = bframe.ts;
		if(bframe.frame_type == FRAME_TYPE_I)
			bri->iframe_idx++;
		bri->read_pos += len;
		bri->read_cnt++;
		return len - BASKET_FRAME_LEN;
	}
	else if(cmd == READ_FORWARD_VIDEO_FRAME || cmd == READ_FORWARD_AUDIO_FRAME)
	{
		if(cmd == READ_FORWARD_AUDIO_FRAME)
		{
			if((bri->bf->record_type & RECORD_AUDIO_G711A) == 0
				&& (bri->bf->record_type & RECORD_AUDIO_G711U) == 0)
			{
				j_warning("The storage fragment don't have audio frame.\n");
				return -1;
			}
		}
		do
		{
			if(bri->read_pos >= bri->begin_pos + bri->fragment_size)
				return 0;
			g_return_val_if_fail((len = strg_basket_read_frame(bri->bfi->fd, bri->read_pos, &bframe,
								frame_buff+frame_hdr_len, buff_len-frame_hdr_len)) > 0, -1);
			frame_hdr->ts = bframe.ts;
			if(bframe.frame_type == FRAME_TYPE_I)
				bri->iframe_idx++;
			bri->read_pos += len;
			bri->read_cnt++;
			if((cmd == READ_FORWARD_VIDEO_FRAME)
				 && (bframe.frame_type == FRAME_TYPE_I
				|| bframe.frame_type == FRAME_TYPE_P
				|| bframe.frame_type == FRAME_TYPE_B))
				break;
			else if(cmd == READ_FORWARD_AUDIO_FRAME 
				&& bframe.frame_type == FRAME_TYPE_A)
				break;
		}while(1);
		return len - BASKET_FRAME_LEN;
	}
	
	if(bri->read_cnt == 0)
	{
		iframe_oft = 0;
	}
	else
	{
		switch(cmd)
		{
			case READ_FORWARD_1_IFRAME:
				iframe_oft = 1;
				break;
			case READ_FORWARD_2_IFRAME:
				iframe_oft = 2;
				break;
			case READ_FORWARD_3_IFRAME:
				iframe_oft = 3;
				break;
			case READ_FORWARD_4_IFRAME:
				iframe_oft = 4;
				break;
			case READ_BACK_1_IFRAME:
				iframe_oft = -1;
				break;
			case READ_BACK_2_IFRAME:
				iframe_oft = -2;
				break;
			case READ_BACK_3_IFRAME:
				iframe_oft = -3;
				break;
			case READ_BACK_4_IFRAME:
				iframe_oft = -4;
				break;
			default:
				return -1;
				/* break; */
		}
	}
	
	if(bri->iframe_idx + iframe_oft < 0 || 
		bri->iframe_idx + iframe_oft >= bri->bf->first_iframe_idx + bri->iframe_num)
		return 0;
	g_return_val_if_fail(strg_basket_read_iframe(bri->bfi->fd, &bif, bri->iframe_idx + iframe_oft) == BASKET_IFRAME_LEN, -1);
	g_return_val_if_fail((len = strg_basket_read_frame(bri->bfi->fd, bif.frame_pos, &bframe,
						frame_buff+frame_hdr_len, buff_len-frame_hdr_len)) > 0, -1);
	frame_hdr->ts = bframe.ts;
	bri->iframe_idx += iframe_oft;
	bri->read_pos = bif.frame_pos + len;
	bri->read_cnt++;
	
	return len - BASKET_FRAME_LEN;
}

int strg_basket_query(DiskMng *dm, BasketGlobalInfo *bgi, BasketFragment *bf, int bf_idx,
						time_t begin_time, time_t end_time, StorageRecordType record_type,
						strg_query_func query_cb, gpointer data)
{
	g_return_val_if_fail(dm != NULL, -1);
	g_return_val_if_fail(bgi != NULL, -1);
	g_return_val_if_fail(bf != NULL, -1);

	int i;
	int ret;
	int begin_iframe_idx = bf->first_iframe_idx;
	int end_iframe_idx = bf->first_iframe_idx + bf->iframe_num;
	time_t bf_begin_time = bf->begin_time;
	time_t bf_end_time = bf->end_time;
	int begin_frame_pos = bf->begin_frame_pos;
	int end_frame_pos = bf->begin_frame_pos + bf->fragment_size;
	BasketFileInfo *bfi;
	BasketIFrame bif;
	RecordQueryInfo rqi;

	g_return_val_if_fail((bfi = strg_basket_open(dm, bgi)) != NULL, -1);
	
	if(begin_time > bf->begin_time && begin_time < bf->end_time)
	{
		// 如果需要查找的Iframe在fragment的最后面，这些查找方法
		// 会很慢，后面可以想办法进行改进
		for(i = 0; i < bf->iframe_num; i++)
		{
			if(strg_basket_read_iframe(bfi->fd, &bif, bf->first_iframe_idx + i) != BASKET_IFRAME_LEN)
			{
				j_warning("read iframe failed.\n");
				strg_basket_close(bfi);
				return -1;
			}
			if(bif.ts.tv_sec > begin_time)
			{
				begin_iframe_idx = ((i > 0) ? i - 1 : 0);
				bf_begin_time = bif.ts.tv_sec;
				begin_frame_pos = bif.frame_pos;
				break;
			}
		}
	}

	if(end_time > bf->begin_time && end_time < bf->end_time)
	{
		// 如果需要查找的Iframe在fragment的最前面，这些查找方法
		// 会很慢，后面可以想办法进行改进
		for(i = bf->iframe_num - 1; i >= 0; i++)
		{
			if(strg_basket_read_iframe(bfi->fd, &bif, bf->first_iframe_idx + i) != BASKET_IFRAME_LEN)
			{
				j_warning("read iframe failed.\n");
				strg_basket_close(bfi);
				return -1;
			}
			if(bif.ts.tv_sec > end_time)
			{
				end_iframe_idx = bf->first_iframe_idx + i;
				bf_end_time = bif.ts.tv_sec;
				end_frame_pos = bif.frame_pos;
				break;
			}
		}
	}
	strg_basket_close(bfi);
	
	rqi.chn = bgi->bg.chn;
	rqi.disk_idx = dm->disk_idx;
	rqi.basket_idx = bgi->basket_idx;
	rqi.bf_idx = bf_idx;
	rqi.begin_iframe_idx = begin_iframe_idx;
	rqi.iframe_num = end_iframe_idx - begin_iframe_idx;
	rqi.begin_time = bf_begin_time;
	rqi.end_time = bf_end_time;
	// 因为只取fragment里部分帧，实际大小应该比bf->fragment_size小
	// 这里算出这个大小，麻烦且消耗运算时间，
	// 所以直接使用fragment的大小，影响不大
	rqi.fragment_size = end_frame_pos - begin_frame_pos;
	rqi.record_type = bf->record_type;
	g_return_val_if_fail((ret = query_cb(&rqi, data)) >= 0, ret);

	return 0;
}

int strg_basket_reset(BasketFileInfo *bfi, int chn, int use_cnt)
{
	g_return_val_if_fail(bfi != NULL, -1);

	BasketGlobalInfo *bgi = bfi->bgi;
	
	// basket_path 和basket_idx不需要修改

	// 释放所有fragment所占用的资源
	strg_basket_free_all_fragment(bgi);
	bgi->free_space = BASKET_FRAME_SIZE;
	bgi->free_iframe = BASKET_IFRAME_NUM;

	bgi->bg.magic = BASKET_GLOBAL_MAGIC;
	bgi->bg.use_cnt = use_cnt;
	bgi->bg.chn = chn;
	bgi->bg.status = USE_STATUS_IDLE;
	bgi->bg.fragment_num = 0;

	if(strg_basket_write_global(bfi->fd, &bgi->bg) < 0)
	{
		j_warning("Write basket global failed");
		return -1;
	}
	
	return 0;
}

int strg_basket_format(const char *mount_path, int basket_idx)
{
	gchar path[PATH_LEN];
	int fd;
	BasketGlobal bg;
	
	g_snprintf(path, sizeof(path), "%s/basket%d", mount_path, basket_idx);
	fd = open(path, O_CREAT | O_WRONLY, 0666);
	if(fd < 0)
	{
		j_warning("Open %s failed", path);
		return -1;
	}
	// 预分配文件大小
	if(posix_fallocate(fd, SEEK_SET, BASKET_SIZE) < 0)
	{
		j_warning("fallocation file for basket file failed.");
		close(fd);
		return -1;
	}

	memset(&bg, 0, sizeof(bg));
	bg.magic = BASKET_GLOBAL_MAGIC;
	bg.use_cnt = 0;
	bg.status = USE_STATUS_IDLE;
	bg.chn = -1;
	bg.fragment_num = 0;
	if(strg_basket_write_global(fd, &bg) < 0)
	{
		j_warning("basket write global failed");
		close(fd);
		return -1;
	}
	close(fd);
	
	return 0;
}

static int strg_basket_scan_iframe(int fd, BasketFragment *bf)
{
	g_assert(bf != NULL);
	
	int bif_num;
	BasketIFrame bif1, bif2, bif3;
	int iframe_num;

	if((bf->status & USE_STATUS_WRITING) == 0)
		return 0;
	if(strg_basket_read_iframe(fd, &bif1, bf->first_iframe_idx) < 0)
		return -1;
#ifdef STORAGE_DEBUG		
	strg_basket_diskplay_bif(&bif1);
#endif	
	if(bif1.use_cnt != bf->use_cnt)
		return -1;
	if(bif1.ts.tv_sec - bf->begin_time < 0 || bif1.ts.tv_sec - bf->begin_time > 10)
		return -1;
	// 后面的代码扫描iframe结构体,会抛弃最后成功读取的iframe,
	// 因为分析最后一个iframe后面的frame比较麻烦，所以暂时以
	// 最后的iframe来计数填充fragment, 只是有可能抛弃了最后一个gop 数据
	// 以后有必要再加入吧
	iframe_num = 0;
	bif_num = 1;
	while(1)
	{
		if(strg_basket_read_iframe(fd, &bif3, bf->first_iframe_idx + bif_num) < 0)
			break;
	#ifdef STORAGE_DEBUG	
		strg_basket_diskplay_bif(&bif3);
	#endif
		if(bif3.use_cnt != bf->use_cnt)
			break;
		if(bif3.ts.tv_sec - bif1.ts.tv_sec < 0 || bif3.ts.tv_sec - bif1.ts.tv_sec > 10)
			break;
		if(bif3.frame_no <= bif1.frame_no || bif3.frame_no - bif1.frame_no > 200)
			break;
		memcpy(&bif2, &bif3, sizeof(bif2));
		iframe_num++;
		bif_num++;
	}
	j_debug("iframe_num = %d", iframe_num);
	if(iframe_num == 0)
		return -1;
	bf->status = USE_STATUS_IDLE;
	bf->end_time = bif2.ts.tv_sec;
	bf->total_frame_cnt = bif2.frame_no - bif1.frame_no;
	bf->iframe_num = iframe_num;
	bf->fragment_size = bif2.frame_pos - bif1.frame_pos;
	
	return  0;
}

int strg_basket_scan_fragment(BasketGlobalInfo *bgi, BasketFragment *bf, int bf_idx)
{
	g_return_val_if_fail(bgi != NULL, -1);
	g_return_val_if_fail(bf != NULL, -1);
	
	int fd;
	BasketGlobal bg;
	int ret;
	
	fd = open(bgi->basket_path, O_RDWR | O_DSYNC);
	if(fd < 0)
	{
		j_warning("Open %s failed", bgi->basket_path);
		return -1;
	}
	if(strg_basket_read_global(fd, &bg) < 0)
	{
		j_warning("Read basket global failed.");
		goto failed;
	}
#ifdef STORAGE_DEBUG
//	strg_basket_display_bg(&bg);
#endif	
	g_return_val_if_fail(bf_idx < bg.fragment_num, -1);
	if(strg_basket_read_fragment(fd, bf, bf_idx) < 0)
	{
		j_warning("Read fragment failed");
		goto failed;
	}
	strg_basket_display_bf(bf);
	
	if(strg_basket_scan_iframe(fd, bf) < 0)
	{
		j_warning("Scan iframe failed");
		goto failed;
	}
	
	ret = strg_basket_write_fragment(fd, bf, bf_idx);
	if(bg.status & USE_STATUS_WRITING)
	{
		j_message("The basket%d didn't close normally!!!", bgi->basket_idx);
		bg.status = USE_STATUS_IDLE;
		ret |= strg_basket_write_global(fd, &bg);
		memcpy(&bgi->bg, &bg, sizeof(bg));
	}
	close(fd);
	
	return ret;

failed:
	close(fd);
	return -1;
}

int strg_basket_scan(BasketGlobalInfo *bgi)
{
	g_return_val_if_fail(bgi != NULL, -1);

	int bf_idx;
	int fd, bf_num = 0;
	BasketFragment *bf;
	
	fd = open(bgi->basket_path, O_RDWR | O_DSYNC);
	if(fd < 0)
	{
		j_warning("Open %s failed", bgi->basket_path);
		return -1;
	}
	if(strg_basket_read_global(fd, &bgi->bg) < 0)
	{
		j_warning("Read basket global failed.");
		bgi->bg.magic = BASKET_GLOBAL_MAGIC;
		bgi->bg.use_cnt = 0;
		bgi->bg.status = USE_STATUS_IDLE;
		bgi->bg.chn = -1;
		bgi->bg.fragment_num = 0;
		if(strg_basket_write_global(fd, &bgi->bg) < 0)
		{
			j_warning("Write global failed");
			close(fd);
			return -1;
		}
		bgi->free_space = BASKET_FRAME_SIZE;
		bgi->free_iframe = BASKET_IFRAME_NUM;
		close(fd);
		return 0;
	}
	if(bgi->bg.status & USE_STATUS_WRITING)
	{
		j_message("The basket%d didn't close normally!!!", bgi->basket_idx);
	}
#ifdef STORAGE_DEBUG
//	strg_basket_display_bg(&bgi->bg);
#endif	
	bgi->free_space = BASKET_FRAME_SIZE;
	bgi->free_iframe = BASKET_IFRAME_NUM;
	for(bf_idx = 0; bf_idx < bgi->bg.fragment_num; bf_idx++)
	{
		bf = BASKET_FRAGMENT_ALLOC();
		if(strg_basket_read_fragment(fd, bf, bf_idx) < 0)
		{
			j_warning("read fragment failed.");
			BASKET_FRAGMENT_FREE(bf);
			break;
		}
		if(bf->status & USE_STATUS_WRITING)
		{
			j_warning("The basket fragment status is wrong!");
			if(strg_basket_scan_iframe(fd, bf) < 0)
			{
				j_warning("Scan basket failed");
				BASKET_FRAGMENT_FREE(bf);
				break;
			}
			strg_basket_write_fragment(fd, bf, bf_idx);
			bgi->update_flag = 1;
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
		bf_num++;
	}
	if(bgi->bg.status != USE_STATUS_IDLE || bgi->bg.fragment_num != bf_num)
	{
		bgi->bg.status = USE_STATUS_IDLE;
		bgi->bg.fragment_num = bf_num;
		strg_basket_write_global(fd, &bgi->bg);
		bgi->update_flag = 1;
	}
	close(fd);
	
	return 0;
}


BasketFileInfo* strg_basket_open(struct DiskMng *dm, BasketGlobalInfo *bgi)
{
	g_return_val_if_fail(dm != NULL, NULL);

	BasketFileInfo *bfi;
	
	bfi = g_malloc0(sizeof(BasketFileInfo));
	bfi->disk_mng = dm;
	bfi->bgi = bgi;
	j_debug("open %s file", bgi->basket_path);
	bfi->fd = open(bgi->basket_path, O_RDWR);
	bfi->ffd = open(bgi->basket_path, O_RDWR);
	if(bfi->fd < 0 || bfi->ffd < 0)
	{
		if(bfi->fd > 0)
			close(bfi->fd);
		if(bfi->ffd > 0)
			close(bfi->ffd);
		j_warning("Open %s file failed.", bgi->basket_path);
		g_free(bfi);
		return NULL;
	}
	
	return bfi;
}

void strg_basket_close(BasketFileInfo *bfi)
{
	g_return_if_fail(bfi != NULL);
	
	j_debug("%s close", bfi->bgi->basket_path);
	close(bfi->fd);
	close(bfi->ffd);
	g_free(bfi);
}

void strg_basket_diskplay_bif(BasketIFrame *bif)
{
	g_return_if_fail(bif != NULL);
	
	printf("==> basket iframe\n");
	printf("use_cnt: %d\n", bif->use_cnt);
	printf("frame_no: %d\n", bif->frame_no);
	printf("frame_pos: %d\n", bif->frame_pos);
	printf("timestamp: %ld\n", bif->ts.tv_sec);
	printf("\n");
}

void strg_basket_display_bf(BasketFragment *bf)
{
	g_return_if_fail(bf != NULL);

	printf("==> basket fragment\n");
	printf("status: %d\n", bf->status);
	printf("use_cnt: %d\n", bf->use_cnt);
	printf("begin_time: %ld\n", bf->begin_time);
	printf("end_time: %ld\n", bf->end_time);
	printf("total frame cnt: %d\n", bf->total_frame_cnt);
	printf("first iframe idx: %d\n", bf->first_iframe_idx);
	printf("iframe num: %d\n", bf->iframe_num);
	printf("begin frame pos: %d\n", bf->begin_frame_pos);
	printf("fragment size: %d\n", bf->fragment_size);
	printf("\n");
}

void strg_basket_display_bg(BasketGlobal *bg)
{
	g_return_if_fail(bg != NULL);

	printf("bg use_cnt: %d\n", bg->use_cnt);
	printf("bg status: %d\n", bg->status);
	printf("bg chn: %d\n", bg->chn);
	printf("bg fragment_num: %d\n", bg->fragment_num);
}

void strg_basket_display_bgi(BasketGlobalInfo *bgi)
{
	g_return_if_fail(bgi != NULL);

	GList *bf_list;
	BasketFragment *bf;

	printf("bgi basket path: %s\n", bgi->basket_path);
	printf("bgi basket%d\n", bgi->basket_idx);
	printf("bgi update flag: %d\n", bgi->update_flag);
	printf("bgi free space: %u\n", bgi->free_space);
	printf("bgi free iframe num: %u\n", bgi->free_iframe);
	strg_basket_display_bg(&bgi->bg);
	for(bf_list = g_list_first(bgi->bf_list); bf_list != NULL; bf_list = g_list_next(bf_list))
	{
		bf = (BasketFragment *)bf_list->data;
		printf("status: %d\n", bf->status);
		printf("use_cnt: %d\n", bf->use_cnt);
		printf("begin_time: %ld\n", bf->begin_time);
		printf("end_time: %ld\n", bf->end_time);
		printf("total frame cnt: %d\n", bf->total_frame_cnt);
		printf("first iframe idx: %d\n", bf->first_iframe_idx);
		printf("iframe num: %d\n", bf->iframe_num);
		printf("begin frame pos: %u\n", bf->begin_frame_pos);
		printf("fragment_size: %d\n", bf->fragment_size);
		printf("\n");
	}
	printf("\n\n");
}


