
#include <strg_header.h>
#include <strg_record.h>
#include <strg_format.h>
#include <strg_server.h>
#include <storage_interface.h>

#include <net_api.h>


typedef struct StorageServer
{
	int sock;

	GIOChannel *iochannel;
}StorageServer;
static StorageServer g_storage;


static int strg_server_decode_cmd(int sock, StorageSvrNetHdr *hdr, gpointer ext_data)
{
	g_assert(hdr != NULL);
	guint8 send_buff[4096];
	StorageSvrNetHdr *rsp_hdr = (StorageSvrNetHdr *)send_buff;
	void *send_ext_buff = rsp_hdr + 1;
	
	memcpy(rsp_hdr, hdr, sizeof(StorageSvrNetHdr));

	switch(hdr->cmd)
	{
		case STORAGE_RECORD_START:
		{
			int record_type = hdr->param[0];
			rsp_hdr->result = strg_record_start(hdr->chn, hdr->stream, record_type);
			rsp_hdr->ext_data_len = 0;
			j_debug("Storage ch %d record start!", hdr->chn);
			break;
		}
		case STORAGE_RECORD_STOP:
		{
			strg_record_stop(hdr->chn);
			rsp_hdr->result = 0;
			rsp_hdr->ext_data_len = 0;
			j_debug("Storage ch%d record stop!", hdr->chn);
			break;
		}
		case STORAGE_RECORD_SET_TYPE:
		{
			rsp_hdr->result = strg_record_set_type(hdr->chn, hdr->param[0]);
			rsp_hdr->ext_data_len = 0;
			//j_debug("Storage ch%d record set type: 0x%lx!", hdr->chn, hdr->param[0]);
			break;
		}
		case STORAGE_RECORD_SET_OVERWRITE:
		{
			rsp_hdr->result = strg_record_set_overwrite(hdr->chn, hdr->param[0]);
			rsp_hdr->ext_data_len = 0;
			j_debug("Storage ch%d record set overwrite: %ld!", hdr->chn, hdr->param[0]);
			break;
		}
		case STORAGE_RECORD_SET_MINUTES:
		{
			rsp_hdr->result = strg_record_set_minutes(hdr->param[0]);
			rsp_hdr->ext_data_len = 0;
			j_debug("Storage record set minutes: %ld!", hdr->param[0]);
			break;
		}
		case STORAGE_RECORD_QUERY:
		{
			rsp_hdr->result = strg_record_query(hdr->chn, hdr->param[0], 
												hdr->param[1], hdr->param[2], hdr->param[3]); 
			rsp_hdr->ext_data_len = 0;
			j_debug("Storage ch%d record query!", hdr->chn);
			break;
		}
		case STORAGE_RECORD_FIND:
		{
			rsp_hdr->result = strg_record_find(hdr->chn, hdr->param[0],
												hdr->param[1], hdr->param[2]);
			rsp_hdr->ext_data_len = 0;
			j_debug("Storage ch%d record find!", hdr->chn);
			break;
		}
		case STORAGE_READ_START:
		{
			long handle;
			StorageReadStartCmd *cmd = (StorageReadStartCmd*)ext_data;
			RecordQueryInfo *query_info = &cmd->query_info;
			StorageFragmentInfo *fragment_info = (StorageFragmentInfo*)send_ext_buff;
			g_assert(hdr->ext_data_len == sizeof(StorageReadStartCmd));
			rsp_hdr->result = strg_record_read_start(cmd->shmid, query_info, 
														fragment_info, &handle);
			rsp_hdr->param[0] = handle;
			rsp_hdr->ext_data_len = sizeof(StorageFragmentInfo);
			j_debug("Storage read start!");
			break;
		}
		case STORAGE_READ_STOP:
		{
			long handle = rsp_hdr->param[0];
			rsp_hdr->result = strg_record_read_stop(handle);
			rsp_hdr->ext_data_len = 0; 
			j_debug("Storage read stop!");
			break;
		}
		case STORAGE_READ_SET:
		{
			long handle = rsp_hdr->param[0];
			time_t t = rsp_hdr->param[1];
			rsp_hdr->result = strg_record_read_set(handle, t);
			rsp_hdr->ext_data_len = 0; 
			j_debug("Storage read set!");
			break;
		}
		case STORAGE_READ_SEEK:
		{
			long handle = rsp_hdr->param[0];
			int seek_iframe_offset = rsp_hdr->param[1];
			rsp_hdr->result = strg_record_read_seek(handle, seek_iframe_offset);
			rsp_hdr->ext_data_len = 0; 
			j_debug("Storage read set!");
			break;
		}
		case STORAGE_READ_FRAME:
		{
			long handle = rsp_hdr->param[0];
			StorageReadCmd cmd = rsp_hdr->param[1];
			rsp_hdr->result = strg_record_read(handle, cmd);
			rsp_hdr->ext_data_len = 0;
			//j_debug("Storage read frame!");
			break;
		}
		case STORAGE_FORMAT_DISK_START:
		{
			rsp_hdr->result = strg_record_format_disk_start(hdr->param[0], hdr->param[1]);
			rsp_hdr->ext_data_len = 0;
			j_debug("Storage format disk start!");
			break;
		}
		case STORAGE_FORMAT_DISK_STOP:
		{
			strg_record_format_disk_stop(hdr->param[0]);
			rsp_hdr->result = 0;
			rsp_hdr->ext_data_len = 0;
			j_debug("Storage format disk stop!");
			break;
		}
		case STORAGE_FORMAT_PERCENT:
		{
			rsp_hdr->result = 
				strg_record_format_get_percent((int*)&rsp_hdr->param[0]);
			rsp_hdr->ext_data_len = 0;
			j_debug("Storage get format percent, percent: %ld!", rsp_hdr->param[0]);
			break;
		}
		case STORAGE_GET_DISK_INFO:
		{
			StorageDisksInfo *disks_info = (StorageDisksInfo*)send_ext_buff;
			rsp_hdr->result = strg_record_get_disk_info(disks_info);
			rsp_hdr->ext_data_len = sizeof(StorageDisksInfo);
			j_debug("Storage get disk info!");
			break;
		}
		case STORAGE_GET_RECORD_INFO:
		{
			StorageRecordInfo *record_info = (StorageRecordInfo *)send_ext_buff;
			rsp_hdr->result = strg_record_get_record_info(record_info);
			rsp_hdr->ext_data_len = sizeof(StorageRecordInfo);
			//j_debug("Storage get record info!");
			break;
		}
		case STORAGE_DISPLAY_INFO:
		{
			strg_record_display_info();
			rsp_hdr->result = 0;
			rsp_hdr->ext_data_len = 0;
			j_debug("Storage display infomation");
			break;
		}
		case STORAGE_SAVE_JPEG:
		{
			unsigned long long pts;
			memcpy(&pts, ext_data, sizeof(unsigned long long));
			rsp_hdr->result = strg_record_save_jpeg(hdr->chn, hdr->stream, hdr->param[0], hdr->param[1], pts);
			rsp_hdr->ext_data_len = 0;
			j_debug("Storage save jpeg");
			break;
		}
		case STORAGE_SAVE_JPEG_BY_PATH:
		{
			unsigned long long pts;
			memcpy(&pts, ext_data, sizeof(unsigned long long));
			rsp_hdr->result = strg_record_save_jpeg_by_path(hdr->chn, hdr->stream, 
												(char *)ext_data+sizeof(unsigned long long), pts);
			rsp_hdr->ext_data_len = 0;
			j_debug("Storage save jpeg by path %s", (char *)ext_data+sizeof(unsigned long long));
			break;
		}
		default:
		{
			j_warning("The storage server command %d don't support!!!", hdr->cmd);
			rsp_hdr->result = -1;
			rsp_hdr->ext_data_len = 0;
			break;
		}
	}
	if(net_tcp_noblock_send(sock, send_buff, 
							sizeof(StorageSvrNetHdr)+rsp_hdr->ext_data_len, NULL, 1000) < 0)
	{
		j_warning("Response the storage server command failed!");
		return -1;
	}
	return 0;
}

static int strg_server_read_package(int sock)
{
	StorageSvrNetHdr hdr;
	guint8 buffer[1024];
	gpointer data = NULL;
	
	if(net_tcp_noblock_recv(sock, &hdr, sizeof(hdr), sizeof(hdr), 1000) < 0)
	{
		j_warning("Receive network header failed.");
		return -1;
	}
	if(hdr.magic != STORAGE_SVR_MAGIC)
	{
		j_warning("The magic of storage server network header is wrong.");
		return -1;
	}
	if(hdr.ext_data_len > 0)
	{
		if(hdr.ext_data_len > sizeof(buffer))
			data = g_malloc(hdr.ext_data_len);
		else
			data = &buffer;
		if(net_tcp_noblock_recv(sock, data, hdr.ext_data_len, hdr.ext_data_len, 1000) < 0)
		{
			j_warning("Receive storage package data failed.\n");
			if(data != buffer)
				g_free(data);
			return -1;
		}
	}
	strg_server_decode_cmd(sock, &hdr, data);
	
	if(data != buffer)
		g_free(data);
	return 0;
}

static gboolean strg_server_func (GIOChannel *channel, 
							GIOCondition  condition, gpointer      data)
{
	int nSock = g_io_channel_unix_get_fd (channel);
	
	if(condition | G_IO_IN)
	{
		if(strg_server_read_package(nSock) < 0)
		{
			net_close_socket(&nSock);
			g_io_channel_unref(channel);
			return FALSE;
		}
	}
	else if((condition | G_IO_ERR) || (condition | G_IO_HUP))
	{
		net_close_socket(&nSock);
		g_io_channel_unref(channel);
		return FALSE;
	}
	
	return TRUE;
}

static gboolean strg_server_accept_func (GIOChannel *channel, 
							GIOCondition  condition, gpointer      data)
{
	int nSock;
	int nServerSock = g_io_channel_unix_get_fd (channel);
	struct sockaddr_in addr;
	int nAddrLen = sizeof(addr);
	GIOChannel *iochannel;
	
	if(condition | G_IO_IN)
	{
		/* accept new connect */
		nSock = net_tcp_block_accept(nServerSock, (struct sockaddr *)&addr, &nAddrLen);
		if(nSock < 0)
		{
			j_warning("Accept new jxj talk connection.");
			return FALSE;
		}
		iochannel = g_io_channel_unix_new(nSock);
		g_io_add_watch(iochannel, G_IO_IN | G_IO_ERR | G_IO_HUP,
							strg_server_func, NULL);
	}
	else if((condition | G_IO_ERR) || (condition | G_IO_HUP))
	{
		/* sock error, clear the resource */
		return FALSE;
	}

	return TRUE;
}

int strg_server_init()
{
	memset(&g_storage, 0, sizeof(g_storage));

	g_storage.sock = net_create_local_tcp_server(STORAGE_SERVER_NAME);
	if(g_storage.sock < 0)
	{
		j_warning("Create storage local tcp server failed.");
		return -1;
	}	

	g_storage.iochannel = g_io_channel_unix_new(g_storage.sock);
	g_io_add_watch(g_storage.iochannel, G_IO_IN | G_IO_ERR | G_IO_HUP,
						strg_server_accept_func, NULL);
	j_message("Storage server initialize succesfully!");
	
	return 0;
}

void strg_server_uninit()
{
	g_io_channel_unref(g_storage.iochannel);
	net_close_socket(&g_storage.sock);
	j_message("Stoarge server uninit finish!\n");
}

