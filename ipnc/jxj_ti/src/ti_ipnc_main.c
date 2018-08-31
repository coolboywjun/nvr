
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <av_pool.h>
#include <net_api.h>
#include <ipnc_lib.h>
#include <ipnc_lib_server.h>
#include "ti_heads.h"

static volatile int quit = 0;
//static sem_t  test_lock;

//static pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;

static IpncProtocolCbTable cbTable = 
{
	.connectCb = ti_ipnc_connect,
	.disconnectCb = ti_ipnc_disconnect,

	.searchCfgCb = ti_ipnc_search_cfg,
	.startSearchCb = ti_ipnc_start_search,
	.stopSearchCb = ti_ipnc_stop_search,
	.getSearchCb = ti_ipnc_get_search,

	.logonCb = ti_ipnc_logon,
	.logoutCb = ti_ipnc_logout,

	.startGetStreamCb = ti_ipnc_start_get_stream,
	.stopGetStreamCb = ti_ipnc_stop_get_stream,
	.requestIFrameCb = ti_ipnc_request_iframe,

	.getParamCb = ti_ipnc_get_param,
	.setParamCb = ti_ipnc_set_param,

	.ptzControl = ti_ipnc_ptz_ctrl,
	
	.testIpcStatus = ti_ipnc_test_ipc,
};


void sig_usr(int signo)
{
	// print the process information here
	ti_ipnc_mng_print_list();
}

void sig_quit(int signo)
{
	quit = 1;
}

#if 1
int main(int argc, char *argv[])
{	
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, sig_quit);
	signal(SIGINT,  sig_quit);
	signal(SIGUSR1, sig_usr);

	Ti_Net_Search_init();

	if(av_pool_init() < 0)
	{
		printf("open av pool failed.\n");
		exit(1);
	}
	
	if(ipnc_server_init(JXJ_TI_PROTOCOL_NAME, &cbTable) < 0)
	{
		printf("ipnc protocol server init failed.\n");
		exit(1);
	}
	
	while(!quit)	pause();
	
	ipnc_server_uninit();
	av_pool_uninit();	
	Ti_Net_Search_uninit();

	return 0;
}
#else


#define TEST_IP		"192.168.88.254"
//#define TEST_IP		"192.168.9.96"
#define TEST_PORT	"6080"
#define TEST_USER	"admin"
#define TEST_PSWD	"admin"
#define FOR_CNT		 200
#define CLIENT_COUNT_SIZE 25
#define CLIENT_COUNT 20
#define TIME_OUT 	 10
#define PTZ_CNT		 10

//test
void thread_para_set(void *data)
{
	long userInfo = *((long*)data);
	/*云台控制：上下左右*/
	int i = 0;
	int j = 0;
	int pos = 0;
	long *p = &userInfo;
	int socket = (int)*p;

	for(i = 0; i < PTZ_CNT; i++)
	{
		if(Ti_Client_Crtl_Ptz(userInfo, PTZ_CMD_DOWN, 0, 50, 0) < 0)
			printf("socket:%d i:%d Test PTZ control failure--------\n", socket, i);

	}
	i = 0;
	while(i < FOR_CNT)
	{
		//if(!ti_net_logon_status(userInfo))
			//break;
			
		pos = (i++)%5;
		if(1 == pos)
			continue;

		for(j = 0; j < PTZ_CNT; j++)
		{
			if(Ti_Client_Crtl_Ptz(userInfo, pos, 0, 50, 0) < 0)
				printf("socket:%d pos:%d i:%d Test PTZ control failure--------\n", socket, pos, i);
		}
		sleep(3);
	}

	for(i = 0; i < PTZ_CNT; i++)
	{
		if(Ti_Client_Crtl_Ptz(userInfo, PTZ_CMD_DOWN, 0, 50, 0) < 0)
			printf("socket:%d i:%d Test PTZ control failure--------\n", socket, i);
	}
	pthread_detach(pthread_self());
}

void thread_para_get(void *data)
{
	
	int i = 0;
	long userInfo = *((long*)data);
	do
	{	
		//if(!ti_net_logon_status(userInfo))
			//break;
		
		ti_param_get_net_ball(userInfo);
		ti_param_get_net(userInfo);
		sleep(1);
		i++;
	
	}while(i < 200);
	
	pthread_detach(pthread_self());
}

void test_ipnc(void)
{
	long client[CLIENT_COUNT_SIZE] = {0};
	int i = 0;
	char quit = '0';
	int pindex = 0;
	pthread_t arr_pid[CLIENT_COUNT*16] = {0};
	
	/*登录设备*/
	for(i = 0; i < CLIENT_COUNT; i++)
		client[i] = ti_net_logon(TEST_IP, TEST_USER,TEST_PSWD, TEST_PORT, TIME_OUT, NULL);

_GET:
	/*获取参数*/
	pindex = 0;
	for(i = 0; i < CLIENT_COUNT; i++)
	{
		//for(j = 0; j < (CLIENT_COUNT/3+1); j++)
		if(client[i] != (long)NULL)
		{
			if(0 != pthread_create(&arr_pid[pindex++], NULL, (void*)thread_para_get, &client[i]))
			{
				printf("pthread_create failed\n");	
			}
		}
	}
	
	printf("pindex:%d\n", pindex);
_SET:	
	/*ptz操作*/
	printf("ptz...\n");
	for(i = 0; i < CLIENT_COUNT; i++)
	{
		//for(j = 0; j < (CLIENT_COUNT/3+1); j++)
		if(client[i] != (long)NULL)
		{
			if(0 != pthread_create(&arr_pid[pindex++], NULL, (void*)thread_para_set, &client[i]))
			{
				printf("pthread_create failed\n");	
			}
		}
	}
	

	//退出
	printf("c/C get; s/S set; q/Q exit...\n");
	while(1)
	{
		quit = getchar();
		if('C' == quit || 'c' == quit)
			goto _GET;
		if('S' == quit || 's' == quit)
			goto _SET;
		if('Q' == quit || 'q' == quit)
			break;
		
	}
	printf("pindex:%d\n", pindex);
	for(i = 0; i < CLIENT_COUNT; i++)
	{
		if(client[i] != (long)NULL)
		{
			if(TI_SUCCESS == ti_net_logout(client[i]))
				printf("quit success!\n");
		}
	}

	return ;	
}

void test_get_search(long handle, int devNum)
{
	IPNC_NET_SEARCH_RES_S *pDev;
	char IP[32] = {0};
	int i = 0;
	
	pDev = (IPNC_NET_SEARCH_RES_S*)malloc(sizeof(IPNC_NET_SEARCH_RES_S)*(devNum));
		
	for(; i < devNum; i++)
	{
		if(Ti_Net_Get_Search(handle, &pDev[i]))
		{
			net_ip_n2a(pDev[i].u32DevIp, IP, 32);
			printf("get IP:%s\r\n", IP);
		}
	}
	
	free(pDev);
}

int main(int argc, char *argv[])
{	
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, sig_quit);
	signal(SIGINT,  sig_quit);
	signal(SIGUSR1, sig_usr);

	long handle = 1221;
	
	if(av_pool_init() < 0)
	{
		printf("open av pool failed.\n");
		exit(1);
	}
	
#if 1	
	printf("IP:%s, USER:%s, PSWD:%s, PORT:%s\n", TEST_IP, TEST_USER,TEST_PSWD, TEST_PORT);
	//test
	Ti_Net_Search_init();
	//lihanwei start
	char ch = '0';	
	int devNum = 0;
	while(1)
	{
		printf("\n");
		printf("Please choose the test function:\n");
		printf("1:Do Search 2:Get Search Dev\n");
		printf("3:Logon 4:Get stream q/Q:Exit Client\n");
		printf("Input CMD:");
		ch = getchar();
		
		switch(ch)
		{
			case '1':
			{
				//搜索
				Ti_Net_Search_Done(handle);
				Ti_Net_Search_Do(handle);
				sleep(3);
			
				//获得搜索数
				devNum = Ti_Net_Get_Search_DevNum();
				printf("devNum :%d\n", devNum);
				
				break;
			}
			case '2':
			{
				//获得搜索结果
				test_get_search(handle, devNum);
				break;
			}
			case '3':
			{
				//参数命令
				test_ipnc();
				break;
			}
			case '4':
			{
				/*//获得流
				long hd = 0;
				TI_NET_Start_RealPlay(TEST_IP, atoi(TEST_PORT), TEST_USER, TEST_PSWD, 
									  0, 0, test_stream, NULL, &hd);
				//TI_NET_Request_IFrame(hd);
				sleep(100);
				TI_NET_Stop_RealPlay(hd);
				printf("get stream end...\n");*/
				break;
			}	
			case 'q':
			case 'Q':
				break;
				
			default:
				break;
		}
		if('q' == ch || 'Q' == ch)
			break;
	}
	Ti_Net_Search_Done(handle);
	//lihanwei end
#endif	
	printf("--------------------ipnc protocol server init ...--------------------------\n");
	if(ipnc_server_init(JXJ_TI_PROTOCOL_NAME, &cbTable) < 0)
	{
		printf("ipnc protocol server init failed.\n");
		exit(1);
	}
	
	while(!quit)	pause();
	
	ipnc_server_uninit();
	av_pool_uninit();	
	Ti_Net_Search_uninit();
	
	return 0;
}
#endif

