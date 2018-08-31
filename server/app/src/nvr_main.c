
#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN	"main"

#include <av_pool.h>
#include <storage_lib.h>
#include <ipnc_lib.h>
#include <log.h>

#include "nvr_server_headers.h"
#include "nvr_log.h"
#include "nvr_param_sys.h"
#include "nvr_net_server.h"
#include "nvr_timer.h"
#include "nvr_net_search.h"
#include "nvr_param_proc.h"
#include "nvr_channel.h"
#include "nvr_comm.h"
#include "nvr_rec.h"
#include "nvr_misc.h"
#include "nvr_net_man.h"
#include "nvr_ntp.h"
#include "nvr_upnp.h"
#include "nvr_ddns.h"
#include "nvr_ptz.h"
#include "nvr_net_common.h"
#include "nvr_net_search.h"
#include "nvr_alarm.h"
#include "nvr_arp.h"
#include "nvr_smart_search.h"


static gboolean nvr_sig_quit(gpointer user_data)
{
	GMainLoop *mloop = (GMainLoop *)user_data;
	j_debug("Receive quit signal to exit main loop!");
	g_main_loop_quit(mloop);
	return G_SOURCE_REMOVE;
}

static int nvr_check_wdt()
{
	int ret;
	int try_num = 0;
	const char *pCheckCmd  = "/opt/nvr/wdt check";

	while(try_num++ < 10)
	{
		ret = system(pCheckCmd);
		j_message("Check wdt value: %d\n", ret);
		if(ret == 0)
			return 0;
		usleep(100000);
	}
	return -1;		
}

int nvr_init()
{
	log_init();
	av_pool_init();
	nvr_log_init();
	nvr_param_init();
	nvr_arp_init();
	nvr_net_man_init();
	nvr_channel_init();
	nvr_smart_search_init();
	nvr_search_init();
	nvr_ntp_init();
	nvr_upnp_init();
	nvr_ddns_init();
	nvr_alarm_init();
	nvr_rec_init();
	nvr_comm_init();
	nvr_timer_init();
	nvr_server_init();
	
	return 0;
}

void nvr_exit()
{
	nvr_server_uninit();
	nvr_timer_uninit();
	nvr_search_uninit();
	nvr_smart_search_uninit();
	nvr_ntp_uninit();
	nvr_upnp_uninit();
	nvr_ddns_uninit();
	nvr_alarm_uninit();
	nvr_comm_uninit();
	nvr_rec_uninit();
	nvr_channel_uninit();
	nvr_net_man_uninit();
	nvr_arp_uninit();
	nvr_param_uninit();
	nvr_log_uninit();
	av_pool_uninit();
	log_uninit();
}

#if 1
int main(int argc, char *argv[])
{
	GMainLoop *mloop = NULL;

#ifdef ARCH_ARM
	if(nvr_check_wdt() < 0)
		return -1;
#endif

	//if(!g_thread_supported())
	//	g_thread_init(NULL);
	mloop = g_main_loop_new(NULL, FALSE);

	g_unix_signal_add(SIGINT, nvr_sig_quit, mloop);
	g_unix_signal_add(SIGTERM, nvr_sig_quit, mloop);
	signal(SIGPIPE, SIG_IGN);
	
	nvr_init();
	log_write(LOG_INFO, SYSTEM_LOG, "系统开机");
	
	g_main_loop_run(mloop);
	
	nvr_exit();
	g_main_loop_unref(mloop);
	
	return 0;
}

#else

#include <semaphore.h>

typedef struct _NVR_TEST_DATA_S_
{
	int pztQuit;
	int searchQuit;
	int paramQuit;
	
}NVR_TEST_DATA_S;

#define FOR_CNT		 8
#define CLIENT_COUNT_SIZE 25
#define CLIENT_COUNT 8
#define TIME_OUT 	 10
#define PTZ_CNT		 10
static sem_t sem_lock;
static NVR_TEST_DATA_S gTestData;

//===============================================================test start
void thread_set_ptz(void *data)
{
	int  *p = (int *)data;
	/*云台控制：上下左右*/
	int i = 0;
	int j = 0;
	int pos = 0;
	int chnNo = *p;
	sem_post(&sem_lock);//+1
	j_debug("======================================================================");
	do
	{
		while(i < FOR_CNT)
		//while(1)
		{
			i++;
			if(i == (FOR_CNT/2))
			{
				for(j = 0; j < PTZ_CNT; j++)
				{
					if(Nvr_Client_Crtl_Ptz(PTZ_CMD_UP, chnNo, 50, 0) < 0)
					{
						j_debug("chnNo:%d j:%d Test PTZ control failure--------", chnNo, j);
					}
					else
					{
						j_debug("chnNo:%d pos:%d j:%d PTZ Crtl success--------", chnNo,pos, j);
					}
				}
				//j_debug("--------Test PTZ control stop...--------");	
			}

			pos = i%5;
			if(1 == pos)
				continue;

			for(j = 0; j < PTZ_CNT; j++)
			{
				if(Nvr_Client_Crtl_Ptz(pos, chnNo, 50, 0) < 0)
				{
					j_debug("chnNo:%d pos:%d i:%d Test PTZ control failure--------", chnNo, pos, i);
				}
				else
					j_debug("chnNo:%d pos:%d i:%d PTZ Crtl success--------", chnNo,pos, i);
			}
			sleep(3);
		}

		for(j = 0; j < PTZ_CNT; j++)
			Nvr_Client_Crtl_Ptz(PTZ_CMD_UP, chnNo, 50, 0);
	}while(!gTestData.pztQuit);
	pthread_detach(pthread_self());
}

void test_ipnc_connect(int chnNo)
{
	NVR_CHANNEL_CFG_S chanelCfg_0;
	memset(&chanelCfg_0, 0, sizeof(NVR_CHANNEL_CFG_S));
	
	strcpy(chanelCfg_0.szProtocolName, "jxj_ti");
	chanelCfg_0.u32IpAddr = net_ip_a2n("192.168.88.254");
	chanelCfg_0.u16DataPort = 6080;
	chanelCfg_0.u8Enable = 1;
	strcpy(chanelCfg_0.szUser, "admin");
	strcpy(chanelCfg_0.szPasswd, "admin");
	
	nvr_set_channel_cfg(chnNo,&chanelCfg_0);
	nvr_channel_check_all_status();
}

void test_ipnc_ptz(int chnNo)
{
	pthread_t pid = -1;
	pthread_create(&pid, NULL, (void*)thread_set_ptz, &chnNo);
	sem_wait(&sem_lock);//-1
}

static void test_get_ipnc_net_param(int chnNo)
{
	IPNC_NET_CFG_S paramInfo;

	memset(&paramInfo, 0, sizeof(IPNC_NET_CFG_S));

	if(0 > nvr_net_get_param(chnNo, IPNC_CMD_NET_CFG, 
					  		&paramInfo, sizeof(IPNC_NET_CFG_S)))
	{
		j_debug("test_ipnc_param failed!\n");
	}	
	else
	{
		j_debug("     paramInfo.ipAddr:%s", paramInfo.ipAddr);
		j_debug("paramInfo.u16HttpPort:%d", paramInfo.u16HttpPort);
	}	
}
static void test_get_ipnc_user_param(int chnNo)
{
	IPNC_USER_CFG_S paramInfo;

	memset(&paramInfo, 0, sizeof(IPNC_USER_CFG_S));

	if(0 > nvr_net_get_param(chnNo, IPNC_CMD_USR_CFG, 
					  		&paramInfo, sizeof(IPNC_USER_CFG_S)))
	{
		j_debug("test_ipnc_param failed!\n");
	}	
	else
	{
		j_debug("paramInfo.userName:%s", paramInfo.userName);
		j_debug("paramInfo.passWord:%s", paramInfo.passWord);
	}	
}

//测试参数
void test_ipnc_get_param(void* data)
{
	int  *p = (int *)data;
	int chnNo = *p;
	sem_post(&sem_lock);//+1
	do
	{	
		test_get_ipnc_user_param(chnNo);
		test_get_ipnc_net_param(chnNo);
		usleep(5000);
	}while(!gTestData.paramQuit);	
	pthread_detach(pthread_self());
}

void test_param(int chnNo)
{
	pthread_t pid = -1;
	pthread_create(&pid, NULL, (void*)test_ipnc_get_param, &chnNo);
	sem_wait(&sem_lock);//-1
}

//测试搜索
void test_start_search(void)
{
	/*char ch = '0';
	int len = 0;
	int devCnt = 0;
	long lHandle;
	
	do
	{
		len = 0;
		devCnt = 0;
		lHandle = 0;
		nvr_search_start("jxj_ti", &lHandle);
		while('\n' == (ch = getchar()));
		j_debug("ch: %c", ch);

		j_debug("lHandle:%lx", lHandle);

		{
			int i = 0;
			char IP[32] = {0}; 
			char devBuf[RECV_BUFF_SIZE] = {0};
			NVR_NET_SEARCH_RES_S* pDev = (NVR_NET_SEARCH_RES_S*)devBuf;
			
			j_debug("devBuf:%p, sizeof(devBuf):%d", devBuf, sizeof(devBuf));	
			len = nvr_search_get(lHandle, (void*)devBuf, sizeof(devBuf));
			devCnt = len/sizeof(NVR_NET_SEARCH_RES_S);
			j_debug("devCnt:%d", devCnt);
			for(; (i < devCnt) && (0 != pDev[i].u32DevIp); i++)
			{
				net_ip_n2a(pDev[i].u32DevIp, IP, 32);
				j_debug("%d get IP:%s", i, IP);
			}

		}
		nvr_search_stop(lHandle);
	}while(!gTestData.paramQuit);*/
	pthread_detach(pthread_self());
}

void test_search()
{
	pthread_t pid = -1;

	pthread_create(&pid, NULL, (void*)test_start_search, NULL);
	pthread_join(pid,NULL);
	sleep(2);
	pid = -1;	
}
void test_get_pro_tb(void)
{
	/*IpncProtocol **pPtb = NULL;
	pPtb = nvr_get_protcol_table();

	j_debug("NVR_CMD_GET_PROT_TAB:%x", NVR_CMD_GET_PROT_TAB);

	for(;*pPtb != NULL; pPtb++)
	{
		j_debug("*pPtb:%s", *pPtb);
	}*/
	
}

void test_arp_search(void)
{
	int i = 0;	
	NVR_NET_ARP_SEARCH_NODE_S *pTmpResNode = NULL;
	//开始arp搜索
	nvr_net_arp_search_start();

	sleep(2);
	
	j_debug("<<<<<<<<<<>>>>>>>>>>%-2d\n", nvr_net_arp_search_num());
	pTmpResNode = nvr_net_arp_search_get();
	while (NULL != pTmpResNode) 	
	{		
		j_debug("<<<<<<<<<<>>>>>>>>>>%-2d get ip: %d.%d.%d.%d", i++, 				
			(pTmpResNode->arpSearchRes.u32IpAddr >> 24) & 0xff, 				
			(pTmpResNode->arpSearchRes.u32IpAddr >> 16) & 0xff,				
			(pTmpResNode->arpSearchRes.u32IpAddr >> 8 ) & 0xff,  				
			pTmpResNode->arpSearchRes.u32IpAddr & 0xff);

		pTmpResNode = pTmpResNode->next;
	}

	//停止arp搜索
	nvr_net_arp_search_stop();
}
	
void nvr_log_default_handle(const gchar *log_domain, GLogLevelFlags log_level,
				 const gchar *message, gpointer user_data)
{
	char str_buff[1024];
	int levels[] = {LOG_EMERG, LOG_ALERT, LOG_ERR, LOG_CRIT,
						LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG}; 
	int i, level = LOG_INFO;

	for (i = 0; i < 8; i++)
	{
		if (log_level & (1<<i))
		{
			level = levels[i];
			break;
		} 
	}

	snprintf(str_buff, sizeof(str_buff), "%s\n", message);
#ifdef STORAGE_DEBUG
	printf("%s", str_buff);
#endif
	// write to log system, the log message in the /var/log/message file
	syslog(level, str_buff);
}

int main(int argc, char *argv[])
{
	GMainLoop *mloop = NULL;
	
	//if(!g_thread_supported())
	//	g_thread_init(NULL);
	
	//g_log_set_always_fatal(0);
	//g_log_set_fatal_mask(NULL, 0);
	//g_log_set_default_handler(nvr_log_default_handle, NULL);
	mloop = g_main_loop_new(NULL, FALSE);

	g_unix_signal_add(SIGINT, nvr_sig_quit, mloop);
	g_unix_signal_add(SIGTERM, nvr_sig_quit, mloop);
	signal(SIGPIPE, SIG_IGN);
	nvr_init();
#if 1	
	//初始化信号量值0
	sem_init(&sem_lock, 0, 0);	
	//test start
	{
		char ch = '0';
		int chnNo = 0;
		pthread_t pid = -1;
		if(0 != pthread_create(&pid, NULL, (void*)g_main_loop_run, mloop))	
		{		
			j_debug("pthread_create failed\n");		
			return -1;	
		}
		while(1)
		{
			j_debug("\n");
			j_debug("Please choose the test function:");
			j_debug("1:connect    2:Do Ptz");
			j_debug("3:get param  4:set param");
			j_debug("5:Do search");
			j_debug("T:Do test");
			j_debug("p:get protocol table");
			j_debug("a:arp search test");
			j_debug("q/Q:Exit");
			j_debug("Input CMD:");
			memset(&gTestData, 1, sizeof(gTestData));
			
			ch = getchar();
			switch(ch)
			{
				case '1':
					test_ipnc_connect(chnNo);//connect
					break;
				case '2':
					test_ipnc_ptz(chnNo);//ptz
					break;
				case '3':
					test_param(chnNo);//获得参数
					break;
				case '4':
					break;
				case '5':
					test_search();//搜索
					break;
				case 't':
				case 'T'://老化测试
				{
					memset(&gTestData, 0, sizeof(gTestData));
					test_ipnc_connect(chnNo);//connect
					test_ipnc_ptz(chnNo);//ptz
					test_param(chnNo);//获得参数
					test_search();//搜索
					break;
				}
				case 'p':
				case 'P':
				{
					test_get_pro_tb();
					break;
				}
				case 'a':
				case 'A':
				{
					test_arp_search();
					break;
				}
				case 'q':
				case 'Q':
				default:
					break;
			}
			if('q' == ch || 'Q' == ch)
				break;
		}
	}
	//test end
#endif
	//g_main_loop_run(mloop);
	g_main_loop_unref(mloop);
	sem_destroy(&sem_lock);
	nvr_exit();
	
	return 0;
}
#endif

