
#undef G_LOG_DOMAIN	
#define G_LOG_DOMAIN	"server"


#include <ipnc_lib_client.h>

#include <nvr_server_headers.h>
#include <nvr_net_common.h>
#include <nvr_param_proc.h>
#include <nvr_param_check.h>
#include <nvr_param_sys.h>
#include <nvr_net_search.h>
#include <nvr_smtp.h>
#include <nvr_ftp.h>
#include <nvr_misc.h>
#include <nvr_ptz.h>
#include <nvr_net_search.h>

typedef struct _NVR_NET_SERVER_MNG_S_
{
	int quit;
	int sock;
	int epoll_fd;
	
	GThread *serverTh;	
}NVR_NET_SERVER_MNG_S;


NVR_NET_SERVER_MNG_S gServerMng;

typedef int (*funcGetCmd)(int port, void *pData);
typedef int (*funcSetCmd)(int port, void *pData);
//typedef void (*funcCheckCmd)(int port, void *pData);
typedef void (*funcCheckCmd)(void *pData);

typedef struct _NVR_NET_CMD_PROC_S_
{
	const char 		*pParamName;
	guint32			u32InfoSize;
	gint32			s32MaxSuppPort;	
	funcGetCmd		getCmdProc;
	funcSetCmd		setCmdProc;
	funcCheckCmd		checkCmdProc;
}NVR_NET_CMD_PROC_S;

static const NVR_NET_CMD_PROC_S sCmdProc[] =
{
	{"dev", sizeof(NVR_DEV_INFO_S), -1, (funcGetCmd)nvr_get_dev_cfg, (funcSetCmd)nvr_set_dev_cfg, (funcCheckCmd)nvr_check_dev_cfg},
	{"var", sizeof(NVR_DEV_VAR_INFO_S), -1, (funcGetCmd)nvr_get_dev_cfg, (funcSetCmd)nvr_set_var_cfg, (funcCheckCmd)nvr_check_var_cfg},
	{"stable", sizeof(NVR_DEV_STABLE_INFO_S), -1, (funcGetCmd)nvr_get_stable_cfg, NULL, NULL},
	{"net", sizeof(NVR_DEV_NET_CFG_S), -1, (funcGetCmd)nvr_get_net_cfg, (funcSetCmd)nvr_set_net_cfg, (funcCheckCmd)nvr_check_net_cfg},
	{"ftp", sizeof(NVR_FTP_CFG_S), -1, (funcGetCmd)nvr_get_ftp_cfg, (funcSetCmd)nvr_set_ftp_cfg, (funcCheckCmd)nvr_check_ftp_cfg},
	{"smtp", sizeof(NVR_SMTP_CFG_S), -1, (funcGetCmd)nvr_get_smtp_cfg, (funcSetCmd)nvr_set_smtp_cfg, (funcCheckCmd)nvr_check_smtp_cfg},
	{"ddns", sizeof(NVR_DDNS_CFG_S), -1, (funcGetCmd)nvr_get_ddns_cfg, (funcSetCmd)nvr_set_ddns_cfg, (funcCheckCmd)nvr_check_ddns_cfg},
	{"upnp", sizeof(NVR_UPNP_CFG_S), -1, (funcGetCmd)nvr_get_upnp_cfg, (funcSetCmd)nvr_set_upnp_cfg, (funcCheckCmd)nvr_check_upnp_cfg},
	{"ntp", sizeof(NVR_NTP_CFG_S), -1, (funcGetCmd)nvr_get_ntp_cfg, (funcSetCmd)nvr_set_ntp_cfg, (funcCheckCmd)nvr_check_ntp_cfg},
	//{"time", sizeof(NVR_TIME_CFG_S), -1, (funcGetCmd)nvr_get_time_cfg, (funcSetCmd)nvr_set_time_cfg, (funcCheckCmd)nvr_check_time_cfg},
	{"time", sizeof(NVR_TIME_S), -1, (funcGetCmd)nvr_get_time_cfg, (funcSetCmd)nvr_set_time_cfg, (funcCheckCmd)nvr_check_time_cfg},
	{"user", sizeof(NVR_DEV_USER_CFG_S), -1, (funcGetCmd)nvr_get_user_cfg, (funcSetCmd)nvr_set_user_cfg, (funcCheckCmd)nvr_check_user_cfg},
	{"comm", sizeof(NVR_DEV_COMM_CFG_S), -1, (funcGetCmd)nvr_get_comm_cfg, (funcSetCmd)nvr_set_comm_cfg, (funcCheckCmd)nvr_check_comm_cfg},
	{"channel", sizeof(NVR_CHANNEL_CFG_S), -1, (funcGetCmd)nvr_get_channel_cfg, (funcSetCmd)nvr_set_channel_cfg, (funcCheckCmd)nvr_check_channel_cfg},
	{"auto", sizeof(NVR_AUTO_MAINTAIN_CFG_S), -1, (funcGetCmd)nvr_get_auto_cfg, (funcSetCmd)nvr_set_auto_cfg, (funcCheckCmd)nvr_check_auto_cfg},
	{"jxj_platform", sizeof(NVR_DEV_JXJ_PLATFORM_CFG_S), -1, (funcGetCmd)nvr_get_jxj_platform_cfg, (funcSetCmd)nvr_set_jxj_platform_cfg, (funcCheckCmd)nvr_check_jxj_platform_cfg},
	{"main_venc", sizeof(NVR_DEV_VENC_CFG_S), -1, (funcGetCmd)nvr_get_main_venc_cfg, (funcSetCmd)nvr_set_main_venc_cfg, (funcCheckCmd)nvr_check_main_venc_cfg},	
	{"rec", sizeof(NVR_DEV_REC_STRC_S), -1, (funcGetCmd)nvr_get_rec_cfg, (funcSetCmd)nvr_set_rec_cfg, (funcCheckCmd)nvr_check_rec_cfg},	
	{"di", sizeof(NVR_DEV_DI_CFG_S), -1, (funcGetCmd)nvr_get_di_cfg, (funcSetCmd)nvr_set_di_cfg, (funcCheckCmd)nvr_check_di_cfg},
	{"ipnc_alarm", sizeof(NVR_DEV_IPNC_ALARM_CFG_S), -1, (funcGetCmd)nvr_get_ipnc_alarm_cfg, (funcSetCmd)nvr_set_ipnc_alarm_cfg, (funcCheckCmd)nvr_check_ipnc_alarm_cfg},
	{"dianxin_platform", sizeof(NVR_DEV_DIANXIN_PLATFORM_CFG_S), -1, (funcGetCmd)nvr_get_dianxin_platform_cfg, (funcSetCmd)nvr_set_dianxin_platform_cfg, (funcCheckCmd)nvr_check_dianxin_platform_cfg},
	{"auto_ctrl", sizeof(NVR_DEV_AUTO_CTRL_CFG_S), -1, (funcGetCmd)nvr_get_auto_ctrl_cfg, (funcSetCmd)nvr_set_auto_ctrl_cfg, (funcCheckCmd)nvr_check_auto_ctrl_cfg},
	{"osd", sizeof(NVR_DEV_OSD_CFG_S), -1, (funcGetCmd)nvr_get_osd_cfg, (funcSetCmd)nvr_set_osd_cfg, (funcCheckCmd)nvr_check_osd_cfg},
};

int nvr_server_proc_param(int sock, int nPort, int nCmd, char *pData, int nBufLen, int *pErrNo)
{
	int idx;
	
	if((nCmd - NVR_CMD_GET_DEV_INFO)/2 < 0 
		|| (nCmd - NVR_CMD_GET_DEV_INFO)/2 > sizeof(sCmdProc)/sizeof(sCmdProc[0]))
	{
		j_warning("proc cmd %d is out of range.\n", nCmd);
		*pErrNo = NVR_NET_ERR_PARAMETER;
		return 0;
	}
	
	if ((nCmd - NVR_CMD_GET_DEV_INFO)/2 < 0  
		&& sCmdProc[(nCmd - NVR_CMD_GET_DEV_INFO)/2].s32MaxSuppPort > 0 
		&& nPort >= sCmdProc[(nCmd - NVR_CMD_GET_DEV_INFO)/2].s32MaxSuppPort)
	{
		j_warning("proc cmd err chn %d support chn %d", nPort,
			sCmdProc[(nCmd - NVR_CMD_GET_DEV_INFO)/2].s32MaxSuppPort);
		*pErrNo = NVR_NET_ERR_CHANNEL;
		return 0;
	}

	idx = (nCmd - NVR_CMD_GET_DEV_INFO)/2;
	if (nCmd % 2 != 0)//set cmd
	{
		//j_debug("begin to proc set cmd 0x%x, param name: %s", nCmd, sCmdProc[idx].pParamName);
		if (sCmdProc[idx].checkCmdProc != NULL)
		    sCmdProc[idx].checkCmdProc(pData);
		      
		if (sCmdProc[idx].setCmdProc != NULL)
		    sCmdProc[idx].setCmdProc(nPort, pData);
		//j_debug("end to proc set cmd 0x%x, param name: %s", nCmd, sCmdProc[idx].pParamName);

		*pErrNo = 0;
		return 0;
	}
	else
	{
		if (sCmdProc[idx].u32InfoSize > nBufLen)
		{
			j_warning("proc get cmd buff len err %d", nBufLen);
			*pErrNo = NVR_NET_ERR_DATA_FORMAT;
			return 0;
		}
		
		//j_debug("begin to proc get cmd 0x%x, param name: %s", nCmd, sCmdProc[idx].pParamName);
		if (sCmdProc[idx].getCmdProc != NULL)
		    sCmdProc[idx].getCmdProc(nPort, pData);
		//j_debug("end to proc get cmd 0x%x, param name: %s", nCmd, sCmdProc[idx].pParamName);

		*pErrNo = 0;
		return sCmdProc[idx].u32InfoSize;
	}
	return 0;
}

int nvr_server_proc_cmd(int sock, int nPort, int nCmd, char **pData, int nBufLen, int *pErrNo)
{	
	*pErrNo = 0;
	switch(nCmd)
	{
		case NVR_CMD_RESTART_SERVER://重启
		{
			nvr_set_system_reboot_flag();
			break;
		}
		case NVR_CMD_SHUTDOWN_SERVER://关机
		{
			nvr_set_system_shutdown_flag();
			break;
		}
		case NVR_CMD_SW_RESET_SERVER://复位
		{
			nvr_set_system_default_flag(1);
			break;
		}
		case NVR_CMD_SET_HWGPIO_CHECK:
		{
			nvr_set_hwgpio_check_flag(**pData);
			break;
		}
		case NVR_CMD_CTRL_PTZ://ptz
		{
			j_debug("NVR_CMD_CTRL_PTZ:%x", NVR_CMD_CTRL_PTZ);

			//发送PZT 
			if(0 > nvr_send_ptz_cmd(nPort, *pData))
			{
				*pErrNo = -EFAULT;
				j_warning("Send net pzt cmd packet failed.");
			}
			break;
		}
		case NVR_CMD_GET_PARAM://获取参数
		{
			int recvSize = 0;
			char *recvBuf = *pData;
			
			NVR_NET_REQ_PARAM_S *pParmaP = (NVR_NET_REQ_PARAM_S *)recvBuf;
		
			j_debug("NVR_CMD_GET_PARAM:0x%08x",  pParmaP->paramCmd);
			recvSize = nvr_net_get_param(nPort, pParmaP->paramCmd, *pData, nBufLen);
			if(0 > recvSize)
			{
				*pErrNo = -EFAULT;
				j_warning("nvr get ipnc parameter failed!");
				break;
			}
			else
			{
				return recvSize;
			}
			break;
		}
		case NVR_CMD_SET_PARAM://设置参数
		{
			char *recvBuf = *pData;
			NVR_NET_REQ_PARAM_S *pParmaP = (NVR_NET_REQ_PARAM_S *)recvBuf;

			j_debug("NVR_CMD_SET_PARAM:0x%08x",  pParmaP->paramCmd);
			if(0 > nvr_net_set_param(nPort, pParmaP->paramCmd, pParmaP->paramData, pParmaP->dataSize))
			{
				*pErrNo = -EFAULT;
				j_warning("nvr set ipnc parameter failed!");
			}
			break;
		}
		case NVR_CMD_START_SEARCH://search
		{
			long lHandle = 0;
			
			//j_debug("NVR_CMD_START_SEARCH:%x", NVR_CMD_START_SEARCH);
			//j_debug("pData:%s.", *pData);
			
			//开始搜索
			if(-1 != nvr_search_start(*pData, &lHandle))
			{
				memset(*pData, 0 , nBufLen);
				memcpy(*pData, &lHandle, sizeof(lHandle));
				return sizeof(lHandle);
			}
			else
				j_warning("start search failed.");
			

			break;
		}
		case NVR_CMD_GET_SEARCH://get search
		{
			long lHandle = 0;
			int ret = 0;
			
			j_debug("NVR_CMD_GET_SEARCH:%x", NVR_CMD_GET_SEARCH);
			memcpy(&lHandle, *pData, sizeof(lHandle));
			
			j_debug("lHandle: %#lx", lHandle);
			
			if(lHandle != 0)
			{	
				//获得搜索结果
				ret = nvr_search_get(lHandle, (IPNC_NET_SEARCH_RES_S*)*pData);
				//j_debug("ret: %d", ret);
				if(ret < 0)
				{
					*pErrNo = ret;
					return 0;
				}
				else if(ret == 0)
				{
					return ret;
				}

				//数据大小
				return sizeof(IPNC_NET_SEARCH_RES_S);
			}
			break;
		}
		case NVR_CMD_TEST_IPC_STATUS:
		{
			long lHandle = 0;
			int  ret = 0;
			char ip[32]  = {0};
			char *pTmp = *pData;//lhandle+ipAddr

			j_debug("NVR_CMD_TEST_IPC_STATUS:%x", NVR_CMD_TEST_IPC_STATUS);
			
			memcpy(&lHandle, pTmp, sizeof(lHandle));//获得句柄
			memcpy(ip, pTmp + sizeof(lHandle), sizeof(ip));//获得ip
			j_debug("lHandle: %#lx", lHandle);
			j_debug("ip: %s", ip);

			//获得ipc状态
			memset(*pData, 0, nBufLen);
			ret = nvr_search_test_ipc_status(lHandle, ip, 0, NULL, NULL);
			memcpy(*pData, &ret, sizeof(ret));

			return sizeof(ret);
		}
		case NVR_CMD_SEARCH_CFG:
		{
			long lHandle = 0;
			IPNC_NET_SEARCH_CFG_REQ_S *pSearchCfg = NULL;
			
			j_debug("NVR_CMD_SEARCH_CFG:%x", NVR_CMD_SEARCH_CFG);
			
			memcpy(&lHandle, *pData, sizeof(lHandle));
			pSearchCfg = (IPNC_NET_SEARCH_CFG_REQ_S *)(*pData + sizeof(lHandle));
		/*	
			j_debug("lHandle: %#lx", lHandle);
			j_debug("pSearchCfg: %p", pSearchCfg);
			j_debug("pSearchCfg->szUsrName: %s", pSearchCfg->szUsrName);
			j_debug("pSearchCfg->u16DevPort: %d", pSearchCfg->u16DevPort);
			j_debug("pSearchCfg->u32DevIp: %d.%d.%d.%d", (pSearchCfg->u32DevIp >> 24) & 0xff,
														  (pSearchCfg->u32DevIp >> 16) & 0xff,
														  (pSearchCfg->u32DevIp >>  8) & 0xff,
														  (pSearchCfg->u32DevIp        & 0xff));
		*/
			j_debug("pSearchCfg->u8SerialId: %s", pSearchCfg->u8SerialId);
			//搜索配置
			nvr_search_cfg(lHandle, pSearchCfg);
			
			break;
		}
		case NVR_CMD_STOP_SEARCH:
		{
			long lHandle = 0;

			j_debug("NVR_CMD_STOP_SEARCH:%x", NVR_CMD_STOP_SEARCH);
			memcpy(&lHandle, *pData, sizeof(lHandle));
			j_debug("lHandle: %#lx", lHandle);
			//清空缓存搜索结果
			nvr_search_stop(lHandle);
			
			break;
		}
		case NVR_CMD_GET_PROT_TAB_INFO://获取支持协议表
		{

			j_debug("NVR_CMD_GET_PROT_TAB_INFO:%x", NVR_CMD_GET_PROT_TAB_INFO);

			int len = 0;
			const IPNC_PROTOCOL_S *pProtocol;
			
			pProtocol = nvr_get_protcol_table();
			memset(*pData, 0, nBufLen);
			for(; 0 != strlen(pProtocol->name); pProtocol++)	
			{
				j_debug("*protocol namd:%s", pProtocol->name);
				
				//g_strlcpy(*pData, protocol->name, nBufLen);
				memcpy(*pData + len, pProtocol, sizeof(IPNC_PROTOCOL_S));
				len += sizeof(IPNC_PROTOCOL_S);
			}

			j_debug("len:%d", len);
			return len;
		}
		case NVR_CMD_GET_PROT_TAB_NUM://获取支持协议总数
		{

			j_debug("NVR_CMD_GET_PROT_TAB_NUM:%x", NVR_CMD_GET_PROT_TAB_NUM);

			int num = 0;

			num  = nvr_get_protcol_table_num();
			memset(*pData, 0, nBufLen);
			memcpy(*pData, &num, sizeof(num));
			j_debug("num :%d", num);
			return sizeof(num);
		}
		case NVR_CMD_EXPORT_PARAM:
		{
			*pErrNo = nvr_param_export(*pData);
			break;
		}
		case NVR_CMD_IMPORT_PARAM:
		{
			*pErrNo = nvr_param_import(*pData);
			break;
		}
		case NVR_CMD_START_MANUAL_REC:
		{
			*pErrNo = nvr_rec_manual(nPort, 1);
			break;
		}
		case NVR_CMD_STOP_MANUAL_REC:
		{
			*pErrNo = nvr_rec_manual(nPort, 0);
			break;
		}
		default:
		{
			j_warning("The cmd %x don't support!!!", nCmd);
			*pErrNo = -ERANGE;
			break;
		}
	}
	return 0;
}

int nvr_server_proc_param_cmd(int sock, int nPort, int nCmd, char **pData, int nBufLen, int *pErrNo)
{
	if ((nCmd - NVR_CMD_GET_DEV_INFO)/2 < (sizeof(sCmdProc)/sizeof(NVR_NET_CMD_PROC_S)))
		return nvr_server_proc_param(sock, nPort, nCmd, *pData, nBufLen, pErrNo);//内部参数存储大小不会很大,采用一级指针
	else
		return nvr_server_proc_cmd(sock, nPort, nCmd, pData, nBufLen, pErrNo);//搜索结果可能比较打开,采用二级指针,随时指向新分配空间
}

static int nvr_server_read_package(int sock)
{
	int len;
	NVR_NET_HDR_S netHdr;
	//char buff[NVR_NET_MAX_MSG_LEN];
	char *pBuf = NULL;//考虑到获取搜索的时候结果可能比较大,故不采用数组
	int errNo;
	int ret = -1;

	//默认分配大小
	pBuf = g_malloc0(NVR_NET_MAX_MSG_LEN);
	
	if(nvr_recv_net_packet(sock, &netHdr, pBuf, NVR_NET_MAX_MSG_LEN) < 0)
	{
		g_free(pBuf);
		net_close_socket(&sock);
		return -1;
	}

	len = nvr_server_proc_param_cmd(sock, netHdr.s8Chn, netHdr.u16CmdType, &pBuf, NVR_NET_MAX_MSG_LEN, &errNo);
	netHdr.u32PacketSize = len;
	netHdr.s8ErrNo = errNo;
	
	ret = nvr_send_net_packet(sock, &netHdr, pBuf, len);
	if(NULL != pBuf)
	{
		g_free(pBuf);
		pBuf = NULL;
	}
	
	return ret; 
}

static gpointer nvr_server_func (gpointer data)
{
	int i, nfds, sock;
	struct epoll_event events[16];
	struct epoll_event ev;
	struct sockaddr_in	addr;
	int addr_len;
	
	while(!gServerMng.quit)
	{
		nfds = epoll_wait(gServerMng.epoll_fd, events, 16, 1000);
		if(nfds == 0)
			continue;
		else if(nfds < 0)
		{
			j_warning("epool wait for storage server failed.\n");
			break;
		}
		else
		{
			for(i = 0; i < nfds; i++)
			{
				if(events[i].data.fd == gServerMng.sock)
				{
					addr_len = sizeof(addr);
					sock = net_tcp_block_accept(gServerMng.sock, 
												(struct sockaddr*)&addr, 
												&addr_len);
					if(sock > 0)
					{
						net_set_sock_noblock(sock);
						ev.events = EPOLLIN;
						ev.data.fd = sock;
						if(epoll_ctl(gServerMng.epoll_fd, EPOLL_CTL_ADD, sock, &ev) < 0)
							net_close_socket(&sock);
					}
				}
				else
				{
					if(events[i].events & EPOLLIN)
					{
						if(nvr_server_read_package(events[i].data.fd) < 0)
						{
							sock = events[i].data.fd;
							epoll_ctl(gServerMng.epoll_fd, EPOLL_CTL_DEL, sock, NULL);
							net_close_socket(&sock);
						}
					}
				}
			}
		}
	}
	return NULL;
}


int nvr_server_init()
{
	struct epoll_event ev;
	
	j_debug("Net server init.");
	memset(&gServerMng, 0, sizeof(gServerMng));
	
	gServerMng.sock = net_create_local_tcp_server("/tmp/server");
	if(gServerMng.sock < 0)
	{
		j_warning("Create the server socket failed.\n");
		return -1;
	}
	gServerMng.epoll_fd = epoll_create(1024);
	if(gServerMng.epoll_fd < 0)
	{
		j_warning("Create epoll failed.");
		net_close_socket(&gServerMng.sock);
		return -1;
	}
	memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN;
	ev.data.fd = gServerMng.sock;
	epoll_ctl(gServerMng.epoll_fd, EPOLL_CTL_ADD, gServerMng.sock, &ev);
	
	gServerMng.serverTh = g_thread_new("NetServer", nvr_server_func, NULL);
	
	return 0;
}


void nvr_server_uninit()
{
	j_debug("Net server uninit.");
	gServerMng.quit = 1;
	g_thread_join(gServerMng.serverTh);
	if(gServerMng.epoll_fd > 0)
		net_close_socket(&gServerMng.epoll_fd);
	net_close_socket(&gServerMng.sock);
	j_debug("Net server uninit finish.");
}

