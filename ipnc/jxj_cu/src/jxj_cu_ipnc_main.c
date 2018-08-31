

#include <jxj_cu_heads.h>


#include "av_pool.h"
#include "j_sdk.h"
#include "mb_api.h"
#include "ipnc_lib.h"
#include "ipnc_lib_server.h"
#include "rtsp_client.h"

#include "jxj_cu_param.h"
#include "jxj_cu_logon.h"
#include "jxj_cu_preview.h"
#include "jxj_cu_search.h"
#include "jxj_cu_ptz.h"

#include "jxj_cu_ipnc_realplay.h"
#include "jxj_cu_ipnc_mng.h"
#include "jxj_cu_ipnc_logon.h"


static IpncProtocolCbTable cbTable = 
{
	.connectCb = jxj_cu_ipnc_connect,
	.disconnectCb = jxj_cu_ipnc_disconnect,

	.logonCb = jxj_cu_ipnc_logon,
	.logoutCb = jxj_cu_ipnc_logout,

	.getParamCb = jxj_cu_ipnc_get_param,
	.setParamCb = jxj_cu_ipnc_set_param,

	.ptzControl = jxj_cu_ipnc_ptz_ctrl,
	
	.searchCfgCb = jxj_cu_ipnc_set_search,
	.startSearchCb = jxj_cu_ipnc_start_search,
	.stopSearchCb = jxj_cu_ipnc_stop_search,
	.getSearchCb = jxj_cu_ipnc_get_search,

	.startGetStreamCb = jxj_cu_ipnc_start_get_stream,
	.stopGetStreamCb = jxj_cu_ipnc_stop_get_stream,
	.requestIFrameCb = jxj_cu_ipnc_request_iframe,
	
	.testIpcStatus = jxj_cu_ipnc_test_logon,
};


static volatile int quit = 0;
void sig_quit(int signo)
{
	quit = 1;
}


int jxj_cu_init()
{

	mb_cu_init(GROUP_IP, GROUP_PORT, 40087); 
	av_pool_init();
	jxj_cu_search_init();
	rtspClientInit();
	
	return 0;
}

void jxj_cu_uninit()
{
	av_pool_uninit();
	mb_cu_uinit(); 
	jxj_cu_search_uninit();
	rtspClientUninit();
}

#if 1

int main()
{	
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, sig_quit);
	signal(SIGINT,  sig_quit);
	//signal(SIGUSR1, sig_usr);

	jxj_cu_init();

	if(ipnc_server_init(JXJ_CU_PROTOCOL_NAME, &cbTable) < 0)
	{
		printf("ipnc protocol server init failed.\n");
		exit(1);
	}
	
	while(!quit) pause();
	
	jxj_cu_uninit();
	ipnc_server_uninit();
	return 0;
}


#else
static long handle;

void usage_main(void)
{
	printf("**************************************************\n");
	printf("1. connect server.\n");
	printf("2. param get and set.\n");
	printf("3. search get and set.\n");
	printf("4. ptz control.\n");
	printf("5. rtsp control.\n");
	printf("6. test logon.\n");
	printf("0. exit.\n");
	printf("**************************************************\n");
	printf("please test val: \n");
}

static void test_ntp_param(long handle)
{
	int ret = 0;
	int testVal = 0;
	int quit = 0;
	JDeviceNTPInfo ntpCfg;
	if(0 == handle)
	{
		printf("logon failed.\n");
		return;
	}
	while(!quit)
	{
		memset(&ntpCfg, 0, sizeof(ntpCfg));
		printf("**************************************************\n");
		printf("1. get ntp param.\n");
		printf("2. set ntp param.\n");
		printf("0. exit.\n");
		printf("**************************************************\n");
		printf("please test val: \n");
		scanf("%d", &testVal);

		switch(testVal)
		{
			case 1:
			{
				ret = jxj_cu_ntp_get(handle, &ntpCfg);
				printf("ret :%d\n", ret);
				printf("ntpCfg.ntp_server_ip: %s\n", ntpCfg.ntp_server_ip);
				printf("ntpCfg.time_zone: %d\n", ntpCfg.time_zone);
				printf("ntpCfg.time_interval: %d\n", ntpCfg.time_interval);	// 对时间隔
				printf("ntpCfg.ntp_enable: %d\n", ntpCfg.ntp_enable);
				printf("ntpCfg.dst_enable: %d\n", ntpCfg.dst_enable);
				break;
			}
			case 2:
			{
				ntpCfg.ntp_enable = 0;
				ntpCfg.dst_enable = 0;
				ntpCfg.time_interval = 8;
				strcpy((char*)ntpCfg.ntp_server_ip,"aa.aaa.aa");
				ntpCfg.time_zone = 6;
				ret = jxj_cu_ntp_set(handle, &ntpCfg);
				printf("ret :%d\n", ret);
				break;
			}
			case 0:
			{ 
				quit = 1;
				break;
			}
			default:
				break;
		}
	}
}
static void test_encode_param(long handle)
{
	int ret = 0;
	int testVal = 0;
	int quit = 0;
	JEncodeParameter cfg;
	if(0 == handle)
	{
		printf("logon failed.\n");
		return;
	}
	while(!quit)
	{
		memset(&cfg, 0, sizeof(cfg));
		printf("**************************************************\n");
		printf("1. get encode param.\n");
		printf("2. set encode param.\n");
		printf("0. exit.\n");
		printf("**************************************************\n");
		printf("please test val: \n");
		scanf("%d", &testVal);

		switch(testVal)
		{
			case 1:
			{
				ret = jxj_cu_encode_get(handle, &cfg);
				printf("ret :%d\n", ret);
				printf("cfg.audio_enble: %d\n", cfg.audio_enble);
				printf("cfg.audio_type: %d\n", cfg.audio_type);
				printf("cfg.au_in_mode: %d\n", cfg.au_in_mode);	
				printf("cfg.bit_rate: %d\n", cfg.bit_rate);	
				printf("cfg.code_rate: %d\n", cfg.code_rate);
				printf("cfg.encode_level: %d\n", cfg.encode_level);
				printf("cfg.format: %d\n", cfg.format);

				printf("cfg.frame_priority: %d\n", cfg.frame_priority);
				printf("cfg.frame_rate: %d\n", cfg.frame_rate);
				printf("cfg.i_frame_interval: %d\n", cfg.i_frame_interval);
				printf("cfg.level: %d\n", cfg.level);
				printf("cfg.qp_value: %d\n", cfg.qp_value);
				printf("cfg.resolution: %d\n", cfg.resolution);
				printf("cfg.video_type: %d\n", cfg.video_type);
				break;
			}
			case 2:
			{
				ret = jxj_cu_encode_set(handle, &cfg);
				printf("ret :%d\n", ret);
				break;
			}
			case 0:
			{ 
				quit = 1;
				break;
			}
			default:
				break;
		}
	}
}


int test_param(long handle)
{
#if 1
	int quit = 0, testVal = 0;

	while(!quit)
	{
		printf("**************************************************\n");
		printf("1. get and set ntp param.\n");
		printf("2. get and set encode param.\n");
		printf("0. exit.\n");
		printf("**************************************************\n");
		printf("please test val: \n");
		scanf("%d", &testVal);

		switch(testVal)
		{
			case 1:
			{
				test_ntp_param(handle);
				break;
			}
			case 2:
			{
				test_encode_param(handle);
				break;
			}	
			case 0:
			{
				quit = 1;
				break;
			}
			default:
				break;
		}
		
	}

	/*if(0 == (ret = jxj_cu_media_get(handle, 0, &mediaCfg)))
	{
		__DBUG("mediaCfg.level: %d\n", mediaCfg.level);
		__DBUG("mediaCfg.media_type: %d\n", mediaCfg.media_type);
		__DBUG("mediaCfg.ip: %s\n", mediaCfg.ip);
		__DBUG("mediaCfg.url: %s\n", mediaCfg.url);
		__DBUG("mediaCfg.cu_ip: %s\n", mediaCfg.cu_ip);
	}*/

#endif

	return 0;
}
int test_search(void)
{
	int quit = 0;
	int i = 0;
	int val = 0;
	int devCnt = 0;
	long userDefId = 0;

	IPNC_NET_SEARCH_RES_S *pDevTmp = NULL;
	IPNC_NET_SEARCH_RES_S *pDevInfo = NULL;
	while(!quit)
	{
		printf("**************************************************\n");
		printf("1. start search.\n");
		printf("2. get search.\n");
		printf("3. set search.\n");
		printf("0. exit.\n");
		printf("**************************************************\n");
		printf("please test val: \n");
		
		scanf("%d", &val);
		switch(val)
		{
			case 1://start
			{
				userDefId = 1221; //每次搜索都要用户自己传递一个标识本次搜索的标识值
				jxj_cu_search_start(userDefId);
				break;
			}
			case 2://get 可定时获取, 每次获取只返回一个设备信息.
			{
				if(0 == userDefId)
				{
					printf("start failed.\n");
					break;
				}
				devCnt = jxj_cu_search_get_dev_cnt();
				printf("devCnt: %d\n", devCnt);
				if(0 < devCnt)
				{
					//memset(&devInfo, 0, sizeof(devInfo));
					pDevInfo = malloc(devCnt*sizeof(IPNC_NET_SEARCH_RES_S));
					pDevTmp = pDevInfo;
					for(i = 0; i < devCnt; i++)
					{
						memset(pDevTmp, 0, sizeof(IPNC_NET_SEARCH_RES_S));
						if(jxj_cu_search_get(userDefId, pDevTmp))
						{
							printf("%-20s --- %-2d.%-2d.%-2d.%-2d ---- %-4d -- %d.%d.%d.%d ---- %02x:%02x:%02x:%02x:%02x:%02x\n", pDevTmp->u8SerialId,
													pDevTmp->u32SoftVer >> 24 & 0xff, pDevTmp->u32SoftVer >> 16 & 0xff,
												  	pDevTmp->u32SoftVer >> 8  & 0xff, pDevTmp->u32SoftVer & 0xff,
												  	pDevTmp->u16DevPort,
													pDevTmp->u32DevIp>>24 & 0xff, pDevTmp->u32DevIp>>16 & 0xff,
													pDevTmp->u32DevIp>>8  & 0xff, pDevTmp->u32DevIp & 0xff,
													pDevTmp->u8MacAddr[0], pDevTmp->u8MacAddr[1], pDevTmp->u8MacAddr[2],
													pDevTmp->u8MacAddr[3], pDevTmp->u8MacAddr[4], pDevTmp->u8MacAddr[5]);
							pDevTmp++;
						}
					}
				}

				break;
			}
			case 3://set
			{
				IPNC_NET_SEARCH_CFG_REQ_S searchCfgPkg = {0};
				char ip[32];

				if(NULL == pDevInfo)
				{
					printf("first please get search.\n");
					break;
				}

				strcpy(ip, "192.168.88.253");

				pDevTmp = pDevInfo;
				for(i = 0; i < devCnt; i++)
				{
					if(net_ip_a2n(ip) == pDevTmp->u32DevIp)
					{
						printf(">>>>>>>>>jxj cu set dec cfg.\n");
						
						//获取响应包信息
						jxj_cu_search_cpy_res2req(&searchCfgPkg, pDevTmp);

						//更改信息
						searchCfgPkg.u16DevPort = 6080;
						searchCfgPkg.u32DevIp = net_ip_a2n("192.168.88.254");
						searchCfgPkg.u32DevIpMask = net_ip_a2n("255.255.0.0"); 
						searchCfgPkg.u32DevGwAddr = net_ip_a2n("192.168.80.1");
						searchCfgPkg.u32DevDns1 = net_ip_a2n("202.96.134.133");
						searchCfgPkg.u32DevDns2 = net_ip_a2n("5.5.5.5");
						searchCfgPkg.u8MacAddr[0] = 0x01;
						searchCfgPkg.u8MacAddr[1] = 0x02;
						searchCfgPkg.u8MacAddr[2] = 0x33;
						searchCfgPkg.u8MacAddr[3] = 0x04;
						searchCfgPkg.u8MacAddr[4] = 0x50;
						searchCfgPkg.u8MacAddr[5] = 0x66;
						
						strcpy(searchCfgPkg.szUsrName, "admin");
						strcpy(searchCfgPkg.szPsw, "admin");

						jxj_cu_search_set(&searchCfgPkg);

						break;
					}
					else
						pDevTmp++;
					
				}
				
				break;
			}
			case 0:
			{
				quit = 1;
				if(NULL != pDevInfo)
					free(pDevInfo);
				jxj_cu_search_stop(userDefId);
				break;
			}
			default:
				break;
		}
	}

	return 0;
}

int test_ptz(void)
{
	int i = 0, j = 0;
	JPTZAction ptzCmd;
	if(0 == handle)
		return -1;

	while(i < 4)
	{
		ptzCmd = PTZ_LEFT + i;
		for(j = 0; j < 10 ;j++)
		{
			if(0 > jxj_cu_client_crtl_ptz(handle, ptzCmd, 50))
				printf("ptz control failed.\n");
		}
		i++;
		sleep(2);
	}
	
	i = 0;
	while(i < 4)
	{
		ptzCmd =  PTZ_DOWN - i;
		for(j = 0; j < 10 ;j++)
		{
			if(0 > jxj_cu_client_crtl_ptz(handle, ptzCmd, 50))
				printf("ptz control failed.\n");
		}
		i++;
		sleep(2);
	}

	for(j = 0; j < 5 ;j++)
	{
		if(0 > jxj_cu_client_crtl_ptz(handle, PTZ_DOWN, 50))
			printf("ptz control failed.\n");
	}
	
	return 0;
}

int test_rtsp(long ipncHandle)
{
	//int chn = 0;
	int quit = 0;
	//char ip[32] = {0};
	//char url[128] = {0};
	//char *url = "rtsp://192.168.4.117:7554/dev=JXJ-NVR-00001234/media=0/channel=1&level=0";
	long rtspCtrlHdl = {0};
	
	//printf("please ip address:\n");
	//fflush(stdin);
	//scanf("%s", ip);
	//printf("\n");
	//printf("please channel number:\n");
	//fflush(stdin);
	//scanf("%d", &chn);
	//printf("\n");
	//fflush(stdin);
	///sprintf(url, "rtsp://%s:7554/dev=JXJ-NVR-00001234/media=0/channel=%d&level=0", ip, chn);

	jxj_cu_start_write_frame(ipncHandle, 0, 0, &rtspCtrlHdl);
	__DBUG("rtspCtrlHdl: %lx\n", rtspCtrlHdl);
	//while(1);
	while(!quit)
	{
		printf("exit, please input 1\n");
		scanf("%d", &quit);
		
	}

	jxj_cu_stop_write_frame(ipncHandle, rtspCtrlHdl);
	
	return 0;
}

void test_logon()
{
	char ip[32] = {0};
	char port[8] = {0};
	char usr[32] = {0};
	char pwd[32] = {0};
	
	printf("please server ip:\n");
	fflush(stdin);
	scanf("%s", ip);
	printf("\n");
	
	printf("please port:\n");
	fflush(stdin);
	scanf("%s", port);
	printf("\n");

	printf("please user name:\n");
	fflush(stdin);
	scanf("%s", usr);
	printf("\n");

	printf("please password:\n");
	fflush(stdin);
	scanf("%s", pwd);
	printf("\n");
	
	if(0 == jxj_cu_logon_test(ip, usr, pwd, port))
	{
		printf("test logon success ...\n");
	}
	else
		printf("test logon failed!!!\n");
}

int main()
{	
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, sig_quit);
	signal(SIGINT,  sig_quit);
	//signal(SIGUSR1, sig_usr);
	signal(SIGPIPE, SIG_IGN);
	
	int testVal = 0;
	int chn = 0;
	char port[8] = {0};
	jxj_cu_init();

	if(ipnc_server_init(JXJ_CU_PROTOCOL_NAME, &cbTable) < 0)
	{
		printf("ipnc protocol server init failed.\n");
		exit(1);
	}
	
	while(!quit)
	{
		usage_main();
		scanf("%d", &testVal);
		switch(testVal)
		{
			case 1://connect server
			{
				
				char ip[32] = {0};
				printf("please server ip:\n");
				fflush(stdin);
				scanf("%s", ip);
				printf("\n");
				printf("please channel number:\n");
				fflush(stdin);
				scanf("%d", &chn);
				printf("\n");
				printf("please port:\n");
				fflush(stdin);
				scanf("%s", port);
				printf("\n");
				handle = jxj_cu_logon(ip,
							  "admin",
							  "admin",
							  port, NULL, &chn);
				if(0 == handle)
				{
					printf("logon failed.\n");
					return -1;
				}
				break;
			}
			case 2://param
			{
				test_param(handle);
				break;
			}
			case 3://search
			{
				test_search();
				break;
			}
			case 4://ptz
			{
				test_ptz();
				break;
			}
			case 5://rtsp
			{
				//test_rtsp();
				break;
			}
			case 6://test logon
			{
				test_logon();
				break;
			}
			case 0:
			{
				quit = 1;
				break;
			}
			default:
				break;
		}
	}

	jxj_cu_uninit();
	ipnc_server_uninit();
	return 0;
}

#endif

