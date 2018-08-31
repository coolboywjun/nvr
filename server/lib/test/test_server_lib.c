
#include <nvr_headers.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <ipnc_lib.h>
#include <ipnc_lib_client.h>

#include <nvr_lib_interface.h>

#if 1

int main(int argc, char *argv[])
{
	NVR_DEV_USER_CFG_S userCfg;

	if(0 > nvr_lib_init())
		return -1;
	
	memset(&userCfg, 0, sizeof(userCfg));
	if(nvr_logon_get_user_cfg(0, &userCfg) < 0)
	{
		printf("get user configure error!!!\n");
	}
	printf(">>>>>>>>>>>>>user: %s, passwd: %s\n", userCfg.szUsrName, userCfg.szPsw);

	nvr_lib_uninit();

	return 0;
}

#else//test

typedef struct _LOGON_CHN_INFO_
{
	int chnNo;
}LOGON_CHN_INFO;

#define FOR_CNT		 8
#define CLIENT_COUNT_SIZE 25
#define CLIENT_COUNT 12
#define TIME_OUT 	 10
#define PTZ_CNT		 10

void thread_set_ptz(void *data)
{
	LOGON_CHN_INFO  *pLogonChn = (LOGON_CHN_INFO *)data;

	/*云台控制：上下左右*/
	int i = 0;
	int j = 0;
	int pos = 0;
	printf("======================================================================\n");
	while(i < FOR_CNT)
	{
		i++;
		if(i == (FOR_CNT/2))
		{
			for(j = 0; j < PTZ_CNT; j++)
			{
				if(Nvr_Client_Crtl_Ptz(PTZ_CMD_UP, pLogonChn->chnNo, 50, 0) < 0)
					printf("chnNo:%d j:%d Test PTZ control failure--------\n", pLogonChn->chnNo, j);
				else
					printf("chnNo:%d pos:%d j:%d PTZ Crtl success--------\n", pLogonChn->chnNo,pos, j);
			}
			//printf("--------Test PTZ control stop...--------");	
		}

		pos = i%5;
		if(1 == pos)
			continue;

		for(j = 0; j < PTZ_CNT; j++)
		{
			if(Nvr_Client_Crtl_Ptz(pos, pLogonChn->chnNo, 50, 0) < 0)
				printf("chnNo:%d pos:%d i:%d Test PTZ control failure--------\n",  pLogonChn->chnNo, pos, i);
			else
				printf("chnNo:%d pos:%d i:%d PTZ Crtl success--------\n",  pLogonChn->chnNo,pos, i);
		}
		sleep(3);
	}

	for(j = 0; j < PTZ_CNT; j++)
		Nvr_Client_Crtl_Ptz(PTZ_CMD_UP, pLogonChn->chnNo, 50, 0);

	pthread_detach(pthread_self());
}


int main(int argc, char *argv[])
{
	int ret = 0;
	char quit = '0';
	char ch = '0';
	//int i = 0;

	NVR_DEV_USER_CFG_S userCfg;

	if(0 > nvr_lib_init())
		return -1;

	memset(&userCfg, 0, sizeof(userCfg));
	if(nvr_logon_get_user_cfg(0, &userCfg) < 0)
	{
		printf("get user configure error!!!\n");
	}
	printf(">>>>>>>>>>>>>user: %s, passwd: %s\n", userCfg.szUsrName, userCfg.szPsw);

_TEST:
	printf("\n");
	printf("Please choose the test function:\n");
	printf("1 restart; 2 shutdown;\n");
	printf("w.RESET;\n");

	printf("3.SET NTP;  	4.GET NTP;\n"); 
	printf("5.SET UPNP;  	6.GET UPNP;\n");
	printf("7.PTZ TEST;  	8.SEARCH;\n");
	printf("9.GET CHANNEL; 	0.SET DI;\n");
	printf("b.GET PROT TABLE; 	\n");
	printf("g.GET IPNC ALARM; G.SET IPNC ALARM;\n");
	printf("h.GET WEB PORT; H.SET WEB PORT;\n");
	printf("m.GET SMTP;  	M.SET SMTP;\n");
	printf("p.GET FTP;   	P.SET FTP;\n");
	printf("c.GET COMM;  	C.SET COMM;\n");
	printf("i.GET time;     I.SET time;\n");
	printf("j.GET JXJ_PLF;  J.SET JXJ_PLF;\n");

	printf("n.GET NET PARAM; N.SET NET PARAM;\n");
	printf("d.GET DEV PARAM; D.SET DEV PARAM;\n");
	printf("f.GET FTP PARAM; F.SET FTP PARAM;\n");
	printf("e.GET SMTP PARAM; E.SET SMTP PARAM;\n");
	printf("t.GET NTP PARAM;  T.SET NTP PARAM;\n");
	printf("u.GET UPNP PARAM; U.SET UPNP PARAM;\n");
	printf("s.GET DDNS PARAM; S.SET DDNS PARAM;\n");
	printf("o.GET COMM PARAM; O.SET COMM PARAM;\n");
	printf("y.GET PPPOE PARAM; Y.SET PPPOE PARAM;\n");
	printf("z.GET PTZ PARAM;   Z.SET PTZ PARAM;\n");
	printf("v.GET VIDEO PARAM; V.SET VIDEO PARAM;\n");
	printf("a.GET AUDIO PARAM; A.SET AUDIO PARAM;\n");
	printf("r.GET USER PARAM;  R.SET USER PARAM;\n");
	
	printf("k.GET MD PARAM;  K.SET MD PARAM;\n");
	printf("x.GET DI PARAM;  X.SET DI PARAM;\n");
	
	printf("l.GET 3A PARAM;  L.SET 3A PARAM;\n");

	printf("W. UPGRADE\n");
	printf("q or Q.Exit Logon \n");
	printf("Input CMD:");
	/*for(i = 0; i < 100; i++)
	  {
	  setbuf(stdin, NULL);
	  }*/
_ENTER:	
	ch = getchar();
	switch(ch)
	{
		case '1':
			{
				//客户端发送重启命令
				printf("send restart command....\n");
				if(1 !=	nvr_restart())
					printf("send restart command failed....\n");
				break;
			}
		case '2':
			{
				//客户端发送关机命令
				printf("send shutdown command....\n");
				if(1 != nvr_shutdown())
					printf("send shutdown command failed....\n");
				break;
			}
			/*case '3':
			  {
			//直接重启
			printf("send restart command....\n");
			if(1 != nvr_restart_system())
			printf("send restart command failed....\n");
			break;
			}
			case '4':
			{
			//直接关机
			printf("send shutdown command....\n");
			if(1 != nvr_shutdown_system())
			printf("send shutdown command failed....\n");
			break;
			}*/
		case '3'://设置NTP
			{
				NVR_NTP_CFG_S ntpCfg;
				memset(&ntpCfg, 0, sizeof(ntpCfg));
				ntpCfg.u8NtpOpen = 1;
				ntpCfg.u8NtpRefTime = 1;
				ntpCfg.u16NtpPort = 123;
				strncpy(ntpCfg.szNtpServer, "time.windows.com", sizeof(ntpCfg.szNtpServer));

				ret = nvr_logon_set_ntp_cfg(0, &ntpCfg);
				if(ret < 0)
					printf("set ntpCfg configure error!!!\n");

				break;
			}
		case '4'://获取NTP配置
			{
				NVR_NTP_CFG_S getntpCfg;
				memset(&getntpCfg, 0, sizeof(getntpCfg));
				ret = nvr_logon_get_ntp_cfg(0, &getntpCfg);
				if(ret != -1)
					printf("szNtpServer:%s, port:%d\n", 
							getntpCfg.szNtpServer, getntpCfg.u16NtpPort);
				else
					printf("get ntpCfg configure error:%d!!!\n", ret);

				break;
			}
		case '5'://设置UPNP
			{
				NVR_UPNP_CFG_S upnpCfg;
				memset(&upnpCfg, 0, sizeof(upnpCfg));
				strcpy(upnpCfg.szUpnpSvr, "192.168.33.113");
				upnpCfg.u8UpnpOpen = 1;
				upnpCfg.u8UpnpEthNo = 0;
				upnpCfg.u8UpnpModel = 0;
				upnpCfg.u16UpnpDataPort = 8080;
				upnpCfg.u16UpnpWebPort = 8081;
				upnpCfg.u16UpnpRtspPort = 8082;
				if(nvr_logon_set_upnp_cfg(0, &upnpCfg) < 0)
					printf("set upnpCfg configure error!!!\n");
				break;
			}
		case '6'://获取UPNP配置
			{
				NVR_UPNP_CFG_S getupnpCfg;
				memset(&getupnpCfg, 0, sizeof(getupnpCfg));
				ret = nvr_logon_get_upnp_cfg(0, &getupnpCfg);
				if(ret != -1)
				{
					printf("DataPort:%d, WebPort:%d, RtspPort:%d\n", 
							getupnpCfg.u16UpnpDataPort, getupnpCfg.u16UpnpWebPort, getupnpCfg.u16UpnpRtspPort);
					printf("DataPortS:%d, WebPortS:%d, RtspPortS:%d\n", 
							getupnpCfg.u16UpnpDataPortS, getupnpCfg.u16UpnpWebPortS, getupnpCfg.u16UpnpRtspPortS);

					printf("sdkCmdPort:%d, jxjDataPort:%d, TalkPort:%d\n", 
							getupnpCfg.u16UpnpCmdPort, getupnpCfg.u16UpnpJXJDataPort, getupnpCfg.u16UpnpTalkPort);
					printf("sdkCmdPortS:%d, jxjDataPortS:%d, TalkPortS:%d\n", 
							getupnpCfg.u16UpnpCmdPortS, getupnpCfg.u16UpnpJXJDataPortS, getupnpCfg.u16UpnpTalkPortS);
				}
				else
					printf("get upnpCfg configure error: %d!!!\n", ret);
				break;
			}
		case '7'://PTZ
			{
				LOGON_CHN_INFO logonInfo;
				pthread_t pid = -1;

				logonInfo.chnNo = 0;
				if(0 != pthread_create(&pid, NULL, (void*)thread_set_ptz, &logonInfo))	
				{		
					printf("pthread_create failed\n");		
					return -1;	
				}
				break;
			}
		case '8'://serach
			{
				int ret = 0;
				long lHandle = 0;
				//char recvBuf[RECV_BUFF_SIZE] = {0};
				char ctrl = '0';
				int  cnt = 0;
				
				IPNC_NET_SEARCH_RES_S dev;
				char* devProtocol = "jxj_ti";
_START_SEARCH:
				
				//开始搜索
				lHandle = nvr_lib_search_start(devProtocol);//开始搜索
_GET_SEARCH:				
				printf("<<<<<<<<<<>>>>>>>>>>lHandle: %#lx\n", lHandle);
				if(0 != lHandle)
				{
					//usleep(5000);
					//pDev = (IPNC_NET_SEARCH_REQ_S*)recvBuf;//分配存储设备空间
					int i = 0;
					//获得搜索
					//memset(recvBuf, 0, sizeof(recvBuf));
					ret = nvr_lib_search_get(lHandle, &dev);
					if(0 < ret)//获得搜索
					{
						printf("<<<<<<<<<<>>>>>>>>>>ret: %d\n", ret);
						cnt++;
						//for(; (i < devCnt) && (0 != pDev[i].u32DevIp); i++)
						{
							printf("<<<<<<<<<<>>>>>>>>>>%-2d get ip: %d.%d.%d.%d\n", i, 
															(dev.u32DevIp >> 24) & 0xff, 
															(dev.u32DevIp >> 16) & 0xff,
															(dev.u32DevIp >> 8 ) & 0xff,  
															 dev.u32DevIp & 0xff);
						}
					}
				}
				else
					printf("search failed...\n");
				printf("<<<<<<<<<<>>>>>>>>>>cnt: %d\n", cnt);
				
_GET_IPC_STATUS:
#if 0	
				{
					//测试联通性
					char ip[32] = {0};
					int i   = 0;
					int ret = 0;
					
					for(; (i < sizeof(recvBuf)/sizeof(IPNC_NET_SEARCH_REQ_S))
						  && (0 != pDev[i].u32DevIp); i++)
					{
							net_ip_n2a(pDev[i].u32DevIp, ip, 32);
							ret = nvr_lib_search_get_ipc_status(lHandle, ip, 32);
							printf("<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>i:%-2d, ret: %-2d, %s\n", i, ret, ip);
					}
				}
#endif
_SEARCH_CFG:
#if 0
				{
					//发送搜索配置包更改ip:192.168.88.254的设备
					int i = 0;
					unsigned long dstIp = net_ip_a2n("192.168.88.254");
					IPNC_NET_SEARCH_CFG_REQ_S searchCfg;
					
					memset(&searchCfg, 0, sizeof(IPNC_NET_SEARCH_CFG_REQ_S));

					//指定用户密码
					strncpy(searchCfg.szUsrName, "admin", sizeof(searchCfg.szUsrName));
					strncpy(searchCfg.szPsw, "admin", sizeof(searchCfg.szPsw));
					
					for(; (i < sizeof(recvBuf)/sizeof(IPNC_NET_SEARCH_REQ_S))
							   && (0 != pDev[i].u32DevIp); i++)
					{
						if(dstIp == pDev[i].u32DevIp)
						{
							//更改ip信息
							pDev[i].u32DevIp = net_ip_a2n("192.168.88.253");
							pDev[i].u32DevIpMask = net_ip_a2n("255.0.0.0");
							pDev[i].u16DevPort = 4080;
							pDev[i].u32DevGwAddr = net_ip_a2n("192.168.88.1");
							pDev[i].u32DevDns1 = net_ip_a2n("1.1.1.1");
							pDev[i].u32DevDns2 = net_ip_a2n("2.2.2.2");
							pDev[i].u8MacAddr[0] = 0x11;
							pDev[i].u8MacAddr[1] = 0x22;
							pDev[i].u8MacAddr[2] = 0x33;
							pDev[i].u8MacAddr[3] = 0x44;
							pDev[i].u8MacAddr[4] = 0x55;
							pDev[i].u8MacAddr[5] = 0x66;
							printf("<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>%-2d get ip: %d.%d.%d.%d\n", i, 
																	(pDev[i].u32DevIp >> 24) & 0xff, 
																	(pDev[i].u32DevIp >> 16) & 0xff,
																    (pDev[i].u32DevIp >> 8 ) & 0xff,  
																     pDev[i].u32DevIp & 0xff);

							//拷贝搜索请求包数据到搜索配置包
							nvr_lib_search_req_cpy_cfg(&searchCfg, &pDev[i]);

							printf("<<<<<<<<<<<<<<<>>>>>>>>>>>>>>u8SerialId: %s\n", searchCfg.u8SerialId);
							
							//发送搜索配置包
							nvr_lib_search_cfg(lHandle, &searchCfg, sizeof(IPNC_NET_SEARCH_CFG_REQ_S));

							break;
						}
					}
					
				}
#endif
				while(1)
				{
					printf("s|S start search again !\n");
					printf("c|C continue get search!\n");
					printf("p|P get search ipc status!\n");
					printf("q|Q stop search!\n");
					ctrl = getchar();
					if('c' == ctrl || 'C' == ctrl)
						goto _GET_SEARCH;
					else if('q' == ctrl || 'Q' == ctrl)	
						break;
					else if('s' == ctrl || 'S' == ctrl)	
						goto _START_SEARCH;
					else if('p' == ctrl || 'P' == ctrl)	
						goto _GET_IPC_STATUS;
					else	
						continue;
				}

				//停止搜索
				nvr_lib_search_stop(lHandle);
				
				break;
			}
		case '9'://获取通道信息
		{
			int i = 0;
			NVR_CHANNEL_CFG_S chnCfg;

			for(i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
			{
				memset(&chnCfg, 0, sizeof(NVR_CHANNEL_CFG_S));
				nvr_logon_get_channel_cfg(i, &chnCfg);
				printf("-----------------------------------------------chn%d start\n", i);
				printf("chnCfg.szUser: %s\n", chnCfg.szUser);
				printf("chnCfg.szPasswd: %s\n", chnCfg.szPasswd);
				printf("chnCfg.u8Enable: %d\n", chnCfg.u8Enable);
				printf("chnCfg.u8SearchA: %d\n", chnCfg.u8SearchA);
				printf("chnCfg.u8Enable: %d\n", chnCfg.u16DataPort);
				printf("-----------------------------------------------chn%d end\n", i);
				
			}
			
			break;
		}
		case '0':
		{
				NVR_DEV_DI_CFG_S struDiCfg;
				int i = 0;
				
				for (i = 0; i < 1/*NVR_MAX_SUPPORT_DI*/; i++)
				{
					memset(&struDiCfg, 0, sizeof(NVR_DEV_DI_CFG_S));

					nvr_logon_get_di_cfg(i, &struDiCfg);
					struDiCfg.u8Enable= 1;
					struDiCfg.u8DiType= 1;

					/*//布防时段
					for (j = 0; j < NVR_MAX_SEG_NUM; j++)
					{
						for (k = 0; k < 8; k++)
						{
							pDi->struSeg[j][k].u8Open   = 1;
							pDi->struSeg[j][k].beginSec = 0;
							pDi->struSeg[j][k].endSec   = 24 * 3600 -1;
						}
					}*/

					struDiCfg.u32DetectInterval = 8;
					//联动
					struDiCfg.struLinkAct.u8Email 		  = 1;
					//pDi->struLinkAct.u32ShootChannel  = 0;
					//pDi->struLinkAct.u8ShootMode      = 0;
					//pDi->struLinkAct.u8ShootNum	 	  = 0;
					//pDi->struLinkAct.u32ShootInterval = 100;
					//pDi->struLinkAct.u32RecTime	 	  = 120;
					//pDi->struLinkAct.u32RecChannel	  = 0;
					//pDi->struLinkAct.u8RecMode	      = 0;
					//pDi->struLinkAct.u8RecPreTime	  = 5;
					struDiCfg.struLinkAct.u32OutPut		  = 1;
					//pDi->struLinkAct.u32OutPersist	  = 30;
					//pDi->struLinkAct.u32PCRecChannel  = 0;

					nvr_logon_set_di_cfg(i, &struDiCfg);
				}
			break;
		}
		case 'b':
		{	
			int proNum = 0;
			int len = 0, i = 0;
			char recvBuf[RECV_BUFF_SIZE] = {0};
			IPNC_PROTOCOL_S *pProtocol = (IPNC_PROTOCOL_S *)recvBuf;

			proNum = nvr_lib_get_pro_table_num();//支持的协议总数

			if(0 >= proNum)
				break;

			printf("proNum: %d\n", proNum);
			if(-1 < (len = nvr_lib_get_pro_table(pProtocol, RECV_BUFF_SIZE/sizeof(IPNC_PROTOCOL_S))))
			{
				printf("len: %d\n", len);
				if(0 < len)
				{
					for(; strlen(pProtocol->name) != 0; pProtocol++)
					{
						printf("pProtocol->name: %s\n", pProtocol->name);
						for(i = 0; 0 != strlen(pProtocol->usrPwd[i].usr); i++)
						{
							printf("pProtocol->usrPwd[%d].usr: %s\n", i, pProtocol->usrPwd[i].usr);
							printf("pProtocol->usrPwd[%d].pwd: %s\n", i, pProtocol->usrPwd[i].pwd);
						}
					}
				}
			}
			
			break;
		}
		case 'g':
		{
			int i = 0;
			NVR_DEV_IPNC_ALARM_CFG_S ipncAlarmCfg;
			for(i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
			{
				memset(&ipncAlarmCfg, 0, sizeof(NVR_DEV_IPNC_ALARM_CFG_S));
				nvr_logon_get_ipnc_alarm_cfg(i, &ipncAlarmCfg);
				printf("-----------------------------------------------------------------\n");
				printf("%d ipncAlarmCfg.u8Enable : %d\n", i, ipncAlarmCfg.u8Enable);
				printf("%d ipncAlarmCfg.struLinkAct.u8Email : %d\n", i, ipncAlarmCfg.struLinkAct.u8Email);
				printf("%d ipncAlarmCfg.struLinkAct.u32ShootChannel : %d\n", i, ipncAlarmCfg.struLinkAct.u32ShootChannel);
				printf("%d ipncAlarmCfg.struLinkAct.u8ShootMode : %d\n", i, ipncAlarmCfg.struLinkAct.u8ShootMode);
				printf("%d ipncAlarmCfg.struLinkAct.u8ShootNum : %d\n", i, ipncAlarmCfg.struLinkAct.u8ShootNum);
				printf("%d ipncAlarmCfg.struLinkAct.u32ShootInterval : %d\n", i, ipncAlarmCfg.struLinkAct.u32ShootInterval);
				printf("%d ipncAlarmCfg.struLinkAct.u32RecTime : %d\n", i, ipncAlarmCfg.struLinkAct.u32RecTime);
				printf("%d ipncAlarmCfg.struLinkAct.u32RecChannel : %d\n", i, ipncAlarmCfg.struLinkAct.u32RecChannel);
				printf("%d ipncAlarmCfg.struLinkAct.u8RecMode : %d\n", i, ipncAlarmCfg.struLinkAct.u8RecMode);
				printf("%d ipncAlarmCfg.struLinkAct.u8RecPreTime : %d\n", i, ipncAlarmCfg.struLinkAct.u8RecPreTime);
				printf("%d ipncAlarmCfg.struLinkAct.u32OutPut : %d\n", i, ipncAlarmCfg.struLinkAct.u32OutPut);
				printf("%d ipncAlarmCfg.struLinkAct.u32OutPersist : %d\n", i, ipncAlarmCfg.struLinkAct.u32OutPersist);
				printf("%d ipncAlarmCfg.struLinkAct.u32PCRecChannel : %d\n", i, ipncAlarmCfg.struLinkAct.u32PCRecChannel);
		
			}
			break;
		}
		case 'G':
		{
						int i = 0;
			NVR_DEV_IPNC_ALARM_CFG_S ipncAlarmCfg;
			for(i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
			{
				memset(&ipncAlarmCfg, 0, sizeof(NVR_DEV_IPNC_ALARM_CFG_S));
				nvr_logon_get_ipnc_alarm_cfg(i, &ipncAlarmCfg);
				ipncAlarmCfg.u8Enable= 1;
	
				//联动
				ipncAlarmCfg.struLinkAct.u8Email 		  = 1;
				ipncAlarmCfg.struLinkAct.u32ShootChannel   = 0;
				ipncAlarmCfg.struLinkAct.u8ShootMode       = 0;
				ipncAlarmCfg.struLinkAct.u8ShootNum	 	  = 0;
				ipncAlarmCfg.struLinkAct.u32ShootInterval  = 10;
				ipncAlarmCfg.struLinkAct.u32RecTime	 	  = 10;
				ipncAlarmCfg.struLinkAct.u32RecChannel	  = 0;
				ipncAlarmCfg.struLinkAct.u8RecMode	      = 0;
				ipncAlarmCfg.struLinkAct.u8RecPreTime	  = 5;
				ipncAlarmCfg.struLinkAct.u32OutPut		  = 0;
				ipncAlarmCfg.struLinkAct.u32OutPersist	  = 30;
				ipncAlarmCfg.struLinkAct.u32PCRecChannel   = 0;
				printf("-----------------------------------------------------------------\n");
				printf("%d ipncAlarmCfg.u8Enable : %d\n", i, ipncAlarmCfg.u8Enable);
				printf("%d ipncAlarmCfg.struLinkAct.u8Email : %d\n", i, ipncAlarmCfg.struLinkAct.u8Email);
				printf("%d ipncAlarmCfg.struLinkAct.u32ShootChannel : %d\n", i, ipncAlarmCfg.struLinkAct.u32ShootChannel);
				printf("%d ipncAlarmCfg.struLinkAct.u8ShootMode : %d\n", i, ipncAlarmCfg.struLinkAct.u8ShootMode);
				printf("%d ipncAlarmCfg.struLinkAct.u8ShootNum : %d\n", i, ipncAlarmCfg.struLinkAct.u8ShootNum);
				printf("%d ipncAlarmCfg.struLinkAct.u32ShootInterval : %d\n", i, ipncAlarmCfg.struLinkAct.u32ShootInterval);
				printf("%d ipncAlarmCfg.struLinkAct.u32RecTime : %d\n", i, ipncAlarmCfg.struLinkAct.u32RecTime);
				printf("%d ipncAlarmCfg.struLinkAct.u32RecChannel : %d\n", i, ipncAlarmCfg.struLinkAct.u32RecChannel);
				printf("%d ipncAlarmCfg.struLinkAct.u8RecMode : %d\n", i, ipncAlarmCfg.struLinkAct.u8RecMode);
				printf("%d ipncAlarmCfg.struLinkAct.u8RecPreTime : %d\n", i, ipncAlarmCfg.struLinkAct.u8RecPreTime);
				printf("%d ipncAlarmCfg.struLinkAct.u32OutPut : %d\n", i, ipncAlarmCfg.struLinkAct.u32OutPut);
				printf("%d ipncAlarmCfg.struLinkAct.u32OutPersist : %d\n", i, ipncAlarmCfg.struLinkAct.u32OutPersist);
				printf("%d ipncAlarmCfg.struLinkAct.u32PCRecChannel : %d\n", i, ipncAlarmCfg.struLinkAct.u32PCRecChannel);
				nvr_logon_set_ipnc_alarm_cfg(i, &ipncAlarmCfg);
		
			}
			break;
		}
		case 'h'://获取web配置
			{
				NVR_DEV_NET_CFG_S strNetCfg;
				memset(&strNetCfg, 0, sizeof(NVR_DEV_NET_CFG_S));

				ret = nvr_logon_get_net_cfg(-1, &strNetCfg);
				if(ret != -1)
				{
					printf("strNetCfg.u16HttpPort:%d\n", strNetCfg.u16HttpPort);
				}
				else
					printf("get webserver configure error: %d!!!\n", ret);

				break;
			}
		case 'H'://设置web配置
			{
				NVR_DEV_NET_CFG_S strNetCfg;
				int port = -1;

				printf("Please Input web port:\n");
				scanf("%d", &port);
				if(port > 0)
					strNetCfg.u16HttpPort = port;	
				else
					strNetCfg.u16HttpPort = 80;

				printf("strNetCfg.u16HttpPort:%d\n", strNetCfg.u16HttpPort);
				ret = nvr_logon_set_net_cfg(-1, &strNetCfg);
				if(ret < 0)
					printf("set webserver configure error!!!\n");
				else
					printf("set webserver configure OK!!!\n");

				break;
			}
		case 'm'://获取smtp配置
			{
				NVR_SMTP_CFG_S smtpCfg;
				memset(&smtpCfg, 0, sizeof(NVR_SMTP_CFG_S));

				ret = nvr_logon_get_smtp_cfg(0, &smtpCfg);
				if(ret != -1)
					printf("szMailHub:%s,port:%d, u16MailPort:%s\n", 
							smtpCfg.szMailHub, smtpCfg.u16MailPort,smtpCfg.szMailAddr);
				else
					printf("get smtpCfg configure error:%d!!!\n", ret);


				break;
			}
		case 'M'://设置smtp配置
			{

				NVR_SMTP_CFG_S smtpcfg;
				memset(&smtpcfg, 0, sizeof(NVR_SMTP_CFG_S));

				strcpy(smtpcfg.szMailHub, "smtp.szjxj.net");
				strcpy(smtpcfg.szMailUsr, "lihanwei@szjxj.net");
				strcpy(smtpcfg.szMailPsw, "jxj123");
				//端口号
				smtpcfg.u16MailPort = 25;
				//发送邮箱
				strcpy(smtpcfg.szMailAddr, "lihanwei@szjxj.net");


				//接收邮箱
				strcpy(smtpcfg.szMailRctp1, "114599531@qq.com");
				//抄送邮箱
				strcpy(smtpcfg.szMailRctp2, "545209342@qq.com");
				strcpy(smtpcfg.szMailRctp3, "851140442@qq.com");

				ret = nvr_logon_set_smtp_cfg(0, &smtpcfg);
				if(0 > ret)
					printf("set smtpCfg configure error:%d!!!\n", ret);
				else
				{
					//test send mail
					//char sendBuf[128] = {0};

					//nvr_make_net_hdr((NVR_NET_HDR_S*)sendBuf, 0, NVR_TEST_CMD_SMTP, 0, 0);
					//if(NVR_SUCCESS != nvr_send_cmd(NULL, 0, 
					//							  sendBuf, sizeof(NVR_NET_HDR_S)))
					//	printf("send data packet failed ...\n");
				}

				break;
			}
		case 'p'://获取ftp配置
			{
				NVR_FTP_CFG_S ftpCfg;
				memset(&ftpCfg, 0, sizeof(NVR_FTP_CFG_S));

				ret = nvr_logon_get_ftp_cfg(0, &ftpCfg);
				if(ret != -1)
					printf("szFtpSvr:%s,port:%d, szFtpPath:%s\n", 
							ftpCfg.szFtpSvr, ftpCfg.u16FtpPort,ftpCfg.szFtpPath);
				else
					printf("get ftpCfg configure error:%d!!!\n", ret);

				break;
			}
		case 'P'://设置ftp配置
			{
				NVR_FTP_CFG_S ftpCfg;
				memset(&ftpCfg, 0, sizeof(NVR_FTP_CFG_S));

				strcpy(ftpCfg.szFtpSvr, "192.168.70.119");
				strcpy(ftpCfg.szFtpUsr, "lihanwei");
				strcpy(ftpCfg.szFtpPsw, "080629");
				//端口号
				ftpCfg.u16FtpPort= 21;
				//存储路径
				strcpy(ftpCfg.szFtpPath, "/test/aaa");

				ret = nvr_logon_set_ftp_cfg(0, &ftpCfg);
				if(0 > ret)
					printf("set ftpCfg configure error:%d!!!\n", ret);
				else
				{
					//test ftp
					//char sendBuf[128] = {0};
					//
					//nvr_make_net_hdr((NVR_NET_HDR_S*)sendBuf, 0, NVR_TEST_CMD_FTP, 0, 0);
					//if(NVR_SUCCESS != nvr_send_cmd(NULL, 0, 
					//							   sendBuf, sizeof(NVR_NET_HDR_S)))
					//	printf("send data packet failed ...\n");
				}

				break;
			}
		case 'c'://获取COMM配置
			{
				NVR_DEV_COMM_CFG_S commCfg;
				memset(&commCfg, 0, sizeof(NVR_DEV_COMM_CFG_S));

				ret = nvr_logon_get_comm_cfg(0, &commCfg);
				if(ret != -1)
				{
					printf("commCfg.u32BaudRate: %d\n", commCfg.u32BaudRate);
					printf("commCfg.u8DataBit: %d\n", commCfg.u8DataBit);
					printf("commCfg.u8StopBit: %d\n", commCfg.u8StopBit);
					printf("commCfg.u8Parity: %d\n", commCfg.u8Parity);
					printf("commCfg.u8Flowcontrol: %d\n", commCfg.u8Flowcontrol);
				}
				else
					printf("get commCfg configure error:%d!!!\n", ret);


				break;
			}
		case 'C'://设置COMM配置
			{
				NVR_DEV_COMM_CFG_S commCfg;
				memset(&commCfg, 0, sizeof(NVR_DEV_COMM_CFG_S));

				commCfg.u32BaudRate = 14;
				commCfg.u8DataBit = 3;
				commCfg.u8StopBit = 0;
				commCfg.u8Parity = 1;
				commCfg.u8Flowcontrol = 0;

				ret = nvr_logon_set_comm_cfg(0, &commCfg);
				if(0 > ret)
					printf("set commCfg configure error:%d!!!\n", ret);

				break;
			}
		case 'i'://获取时间
			{
				NVR_TIME_S timeCfg;
				memset(&timeCfg, 0, sizeof(NVR_TIME_S));

				ret = nvr_logon_get_time_cfg(0, &timeCfg);
				if(ret != -1)
				{
					printf("timeCfg.u8Year: %d\n", timeCfg.u8Year);
					printf("timeCfg.u8Month: %d\n", timeCfg.u8Month);
					printf("timeCfg.u8WDay: %d\n", timeCfg.u8WDay);
					printf("timeCfg.u8Day: %d\n", timeCfg.u8Day);
					printf("timeCfg.u8Hour: %d\n", timeCfg.u8Hour);
					printf("timeCfg.u8Minute: %d\n", timeCfg.u8Minute);
					printf("timeCfg.u8Millisec: %d\n", timeCfg.u8Millisec);
					printf("timeCfg.u8Second: %d\n", timeCfg.u8Second);
				}
				else
					printf("get timeCfg configure error:%d!!!\n", ret);

				break;

			}
		
		case 'I'://设置时间
			{
				break;
			}
		case 'j'://获取jxj平台信息
			{
				NVR_DEV_JXJ_PLATFORM_CFG_S jxjPltCfg;
				memset(&jxjPltCfg, 0, sizeof(NVR_DEV_JXJ_PLATFORM_CFG_S));

				ret = nvr_logon_get_jxj_platform_cfg(0, &jxjPltCfg);
				if(ret != -1)
				{
					printf("jxjPltCfg.szPuId: %s\n", jxjPltCfg.szPuId);
					printf("jxjPltCfg.szCmsIp: %s\n", jxjPltCfg.szCmsIp);
					printf("jxjPltCfg.u16CmsPort: %d\n", jxjPltCfg.u16CmsPort);
					printf("jxjPltCfg.u8Enable: %d\n", jxjPltCfg.u8Enable);
					printf("jxjPltCfg.u8ProtocolType: %d\n", jxjPltCfg.u8ProtocolType);
					printf("jxjPltCfg.u8SyncTimeEnable: %d\n", jxjPltCfg.u8SyncTimeEnable);
				}
				else
					printf("get jxjPltCfg configure error:%d!!!\n", ret);
				
				break;
			}
		case 'J'://设置jxj平台信息
			{
				break;
			}
		case 'n'://获取ipnc net参数
			{
				IPNC_NET_CFG_S devNet;
				memset(&devNet, 0, sizeof(IPNC_NET_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_NET_CFG, &devNet, sizeof(IPNC_NET_CFG_S)))
				{
					printf("get ipnc net param failed....\n");
				}
				else
				{
					printf("devNet.u16NetPort:%d\n", devNet.u16NetPort);
					printf("devNet.u16HttpPort:%d\n", devNet.u16HttpPort);
					printf("devNet.ipAddr:%s\n", devNet.ipAddr);
					printf("devNet.subMask:%s\n", devNet.subMask);
					printf("devNet.gateWay:%s\n", devNet.gateWay);
					printf("devNet.macAddr:%s\n", devNet.macAddr);
					printf("devNet.dnsIp1:%s\n", devNet.dnsIp1);
					printf("devNet.dnsIp2:%s\n", devNet.dnsIp2);
				}
				break;
			}
		case 'N'://设置ipnc net参数
			{
				IPNC_NET_CFG_S devNet;

				memset(&devNet, 0, sizeof(IPNC_NET_CFG_S));

				printf("devNet.u16NetPort:%d\n", devNet.u16NetPort);
				printf("devNet.ipAddr:%s\n", devNet.ipAddr);

				strcpy(devNet.ipAddr, "192.168.88.253");//更改ip地址
				strcpy(devNet.subMask, "255.0.0.0");//更改掩码
				strcpy(devNet.gateWay, "192.168.88.1");//更改网关
				strcpy(devNet.dnsIp1, "1.1.1.1");//更改dns1
				strcpy(devNet.dnsIp2, "2.2.2.2");//更改dns2
				strcpy(devNet.macAddr, "010203112233");//更改mac
				devNet.u16HttpPort = 8080;
				devNet.u16NetPort  = 7080;

				if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_NET_CFG, &devNet, sizeof(IPNC_NET_CFG_S)))//设置参数
					printf("set ipnc net param OK....\n");
				else
					printf("set ipnc net param failed....\n");

				break;
			}
		case 'd'://获取ipnc dev参数
			{
				IPNC_DEV_CFG_S devInfo;
				memset(&devInfo, 0, sizeof(IPNC_DEV_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_DEV_CFG, &devInfo, sizeof(IPNC_DEV_CFG_S)))
				{
					printf("get ipnc dev param failed....\n");
				}
				else
				{
					printf("devInfo.serialId:%s\n", devInfo.serialId);
					printf("devInfo.hwVer:%s\n", devInfo.hwVer);
					printf("devInfo.sofrVer:%s\n", devInfo.sofrVer);
					printf("devInfo.codeVer:%s\n", devInfo.codeVer);

				}

				break;
			}
		case 'D'://设置ipnc dev参数
			{
				//系统信息不可更改
#if 0
				   IPNC_DEV_CFG_S devInfo;

				   memset(&devInfo, 0, sizeof(IPNC_DEV_CFG_S));

				   strcpy(devInfo.serialId, "IPNC-lihanwei");
				   strcpy(devInfo.hwVer, "1.111.111.111");
				   strcpy(devInfo.sofrVer, "44.44.55.56");
				   strcpy(devInfo.codeVer, "77.77.88.99");


				   if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_DEV_CFG, &devInfo, sizeof(IPNC_DEV_CFG_S)))//设置参数
				   printf("set ipnc dev param OK....\n");
				   else
				   printf("set ipnc dev param failed....\n");
#endif
				 
				break;
			}
		case 'f'://获取ipnc ftp参数
			{
				IPNC_FTP_CFG_S ftpInfo;

				memset(&ftpInfo, 0, sizeof(IPNC_FTP_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_FTP_CFG, &ftpInfo, sizeof(IPNC_FTP_CFG_S)))
				{
					printf("get ipnc ftp param failed....\n");
				}
				else
				{
					printf("ftpInfo.ftpSvr:%s\n", ftpInfo.ftpSvr);
					printf("ftpInfo.ftpUsr:%s\n", ftpInfo.ftpUsr);
					printf("ftpInfo.ftpPsw:%s\n", ftpInfo.ftpPsw);
					printf("ftpInfo.ftpPath:%s\n", ftpInfo.ftpPath);
					printf("ftpInfo.u16FtpPort:%d\n", ftpInfo.u16FtpPort);

				}

				break;
			}
		case 'F'://设置ipnc ftp参数
			{
				IPNC_FTP_CFG_S ftpInfo;

				memset(&ftpInfo, 0, sizeof(IPNC_FTP_CFG_S));

				ftpInfo.u16FtpPort = 1245;

				strcpy(ftpInfo.ftpSvr, "192.168.70.119");
				strcpy(ftpInfo.ftpUsr, "lihanwei");
				strcpy(ftpInfo.ftpPsw, "080629");
				strcpy(ftpInfo.ftpPath, "/test/aa");


				if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_FTP_CFG, &ftpInfo, sizeof(IPNC_FTP_CFG_S)))//设置参数
					printf("set ipnc ftp param OK....\n");
				else
					printf("set ipnc ftp param failed....\n");

				break;
			}
		case 'e'://获取ipnc smtp参数
			{
				IPNC_SMTP_CFG_S smtpInfo;

				memset(&smtpInfo, 0, sizeof(IPNC_SMTP_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_EMAIL_CFG, &smtpInfo, sizeof(IPNC_SMTP_CFG_S)))
				{
					printf("get ipnc smtp param failed....\n");
				}
				else
				{
					printf("smtpInfo.u16MailPort:%d\n", smtpInfo.u16MailPort);
					printf("smtpInfo.mailHub:%s\n", smtpInfo.mailHub);
					printf("smtpInfo.mailAddr:%s\n", smtpInfo.mailAddr);
					printf("smtpInfo.mailUsr:%s\n", smtpInfo.mailUsr);
					printf("smtpInfo.mailPsw:%s\n", smtpInfo.mailPsw);
					printf("smtpInfo.mailRctp1:%s\n", smtpInfo.mailRctp1);
					printf("smtpInfo.mailRctp2:%s\n", smtpInfo.mailRctp2);
					printf("smtpInfo.mailRctp3:%s\n", smtpInfo.mailRctp3);

				}

				break;
			}
		case 'E'://设置ipnc smtp参数
			{
				IPNC_SMTP_CFG_S smtpInfo;

				memset(&smtpInfo, 0, sizeof(IPNC_SMTP_CFG_S));

				strcpy(smtpInfo.mailHub, "smtp.szjxj.net");
				strcpy(smtpInfo.mailUsr, "lihanwei@szjxj.net");
				strcpy(smtpInfo.mailPsw, "jxj123");
				//端口号
				smtpInfo.u16MailPort = 25;
				//发送邮箱
				strcpy(smtpInfo.mailAddr, "lihanwei@szjxj.net");

				//接收邮箱
				strcpy(smtpInfo.mailRctp1, "114599531@qq.com");
				//抄送邮箱
				strcpy(smtpInfo.mailRctp2, "545209342@qq.com");
				strcpy(smtpInfo.mailRctp3, "851140442@qq.com");

				if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_EMAIL_CFG, &smtpInfo, sizeof(IPNC_SMTP_CFG_S)))//设置参数
					printf("set ipnc smtp param OK....\n");
				else
					printf("set ipnc smtp param failed....\n");
				break;
			}
		case 't'://获取ipnc ntp参数
			{
				IPNC_NTP_CFG_S ntpInfo;

				memset(&ntpInfo, 0, sizeof(IPNC_NTP_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_NTP_CFG, &ntpInfo, sizeof(IPNC_NTP_CFG_S)))
				{
					printf("get ipnc ntp param failed....\n");
				}
				else
				{
					printf("ntpInfo.ntpServer:%s\n", ntpInfo.ntpServer);
					printf("ntpInfo.timeZone:%d\n", ntpInfo.timeZone);
					printf("ntpInfo.euroTime:%d\n", ntpInfo.euroTime);

				}
				break;
			}
		case 'T'://设置ipnc ntp参数
			{
				IPNC_NTP_CFG_S ntpInfo;

				memset(&ntpInfo, 0, sizeof(IPNC_NTP_CFG_S));

				strcpy(ntpInfo.ntpServer, "cn.pool.ntp.org");

				//端口号
				ntpInfo.timeZone = 8*3600;
				ntpInfo.euroTime = 1;

				if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_NTP_CFG, &ntpInfo, sizeof(IPNC_NTP_CFG_S)))//设置参数
					printf("set ipnc ntp param OK....\n");
				else
					printf("set ipnc ntp param failed....\n");

				break;

			}
		case 'u'://获取ipnc upnp参数
			{
				IPNC_UPNP_CFG_S upnpInfo;

				memset(&upnpInfo, 0, sizeof(IPNC_UPNP_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_UPNP_CFG, &upnpInfo, sizeof(IPNC_UPNP_CFG_S)))
				{
					printf("get ipnc ntp param failed....\n");
				}
				else
				{
					printf("upnpInfo.upnpSvr:%s\n", upnpInfo.upnpSvr);
					printf("upnpInfo.u16UpnpWebPort:%d\n", upnpInfo.u16UpnpWebPort);
					printf("upnpInfo.u16UpnpDataPort:%d\n", upnpInfo.u16UpnpDataPort);

				}
				break;
			}
		case 'U'://设置ipnc upnp参数
			{
				IPNC_UPNP_CFG_S upnpInfo;

				memset(&upnpInfo, 0, sizeof(IPNC_UPNP_CFG_S));

				strcpy(upnpInfo.upnpSvr, "192.168.80.119");

				//端口号
				upnpInfo.u16UpnpWebPort = 80;
				upnpInfo.u16UpnpDataPort = 6080;

				if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_UPNP_CFG, &upnpInfo, sizeof(IPNC_UPNP_CFG_S)))//设置参数
					printf("set ipnc upnp param OK....\n");
				else
					printf("set ipnc upnp param failed....\n");

				break;
			}
		case 's'://获取ipnc ddns参数
			{
				IPNC_DDNS_CFG_S ddnsInfo;

				memset(&ddnsInfo, 0, sizeof(IPNC_DDNS_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_DDNS_CFG, &ddnsInfo, sizeof(IPNC_DDNS_CFG_S)))
				{
					printf("get ipnc ddns param failed....\n");
				}
				else
				{
					printf("ddnsInfo.szDDNSAccount:%s\n", ddnsInfo.szDDNSAccount);
					printf("ddnsInfo.szDDNSUsr:%s\n", ddnsInfo.szDDNSUsr);
					printf("ddnsInfo.szDDNSPsw:%s\n", ddnsInfo.szDDNSPsw);
					printf("ddnsInfo.u16DDNSPort:%d\n", ddnsInfo.u16DDNSPort);
					printf("ddnsInfo.u32DDNSTimes:%d\n", ddnsInfo.u32DDNSTimes);

				}

				break;
			}
		case 'S'://设置ipnc ddns参数
			{
				IPNC_DDNS_CFG_S ddnsInfo;

				memset(&ddnsInfo, 0, sizeof(IPNC_DDNS_CFG_S));

				strcpy(ddnsInfo.szDDNSAccount, "www.lhw.com.cn");
				strcpy(ddnsInfo.szDDNSUsr, "lihanwei");
				strcpy(ddnsInfo.szDDNSPsw, "080629");

				//端口号
				ddnsInfo.u16DDNSPort = 8081;
				ddnsInfo.u32DDNSTimes = 4;

				if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_DDNS_CFG, &ddnsInfo, sizeof(IPNC_DDNS_CFG_S)))//设置参数
					printf("set ipnc upnp param OK....\n");
				else
					printf("set ipnc upnp param failed....\n");

				break;
			}
		case 'o'://获取ipnc comm参数
			{
				IPNC_COMM_CFG_S commInfo;

				memset(&commInfo, 0, sizeof(IPNC_COMM_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_COMM_CFG, &commInfo, sizeof(IPNC_COMM_CFG_S)))
				{
					printf("get ipnc comm param failed....\n");
				}
				else
				{
					printf("commInfo.u32BaudRate:%d\n", commInfo.u32BaudRate);
					printf("commInfo.u8DataBit:%d\n", commInfo.u8DataBit);
					printf("commInfo.u8StopBit:%d\n", commInfo.u8StopBit);
					printf("commInfo.u8Parity:%d\n", commInfo.u8Parity);
					printf("commInfo.u8Flowcontrol:%d\n", commInfo.u8Flowcontrol);
				}
				break;
			}
		case 'O'://设置ipnc comm参数
			{
				IPNC_COMM_CFG_S commInfo;

				memset(&commInfo, 0, sizeof(IPNC_COMM_CFG_S));

				//端口号
				//commInfo.u32BaudRate = 9;//9600 不可修改.
				commInfo.u8DataBit   = 3;//8
				commInfo.u8StopBit   = 0;
				commInfo.u8Parity    = 2;
				commInfo.u8Flowcontrol = 1;

				if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_COMM_CFG, &commInfo, sizeof(IPNC_COMM_CFG_S)))//设置参数
					printf("set ipnc comm param OK....\n");
				else
					printf("set ipnc comm param failed....\n");

				break;
			}
		case 'y'://获取ipnc pppoe参数
			{
				IPNC_PPPOE_CFG_S pppoeInfo;

				memset(&pppoeInfo, 0, sizeof(IPNC_PPPOE_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_PPPOE_CFG, &pppoeInfo, sizeof(IPNC_PPPOE_CFG_S)))
				{
					printf("get ipnc pppoe param failed....\n");
				}
				else
				{
					printf("pppoeInfo.pppoeIp:%s\n", pppoeInfo.pppoeIp);
					printf("pppoeInfo.pppoeUsr:%s\n", pppoeInfo.pppoeUsr);
					printf("pppoeInfo.pppoePsw:%s\n", pppoeInfo.pppoePsw);
				}

				break;
			}
		case 'Y'://设置ipnc pppoe参数
			{
				IPNC_PPPOE_CFG_S pppoeInfo;

				memset(&pppoeInfo, 0, sizeof(IPNC_PPPOE_CFG_S));

				strcpy(pppoeInfo.pppoeIp,"202.96.134.133");
				strcpy(pppoeInfo.pppoeUsr,"lihanwei");
				strcpy(pppoeInfo.pppoePsw,"080629");

				if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_PPPOE_CFG, &pppoeInfo, sizeof(IPNC_PPPOE_CFG_S)))//设置参数
					printf("set ipnc pppoe param OK....\n");
				else
					printf("set ipnc pppoe param failed....\n");	

				break;
			}
		case 'z'://获取ipnc ptz参数
			{
				IPNC_PTZ_CFG_S ptzInfo;

				memset(&ptzInfo, 0, sizeof(IPNC_PTZ_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_PTZ_CFG, &ptzInfo, sizeof(IPNC_PTZ_CFG_S)))
				{
					printf("get ipnc ptz param failed....\n");
				}
				else
				{
					printf("ptzInfo.u8ProtocalType:%u\n", ptzInfo.u8ProtocalType);
					printf("ptzInfo.u8PtzAddr:%u\n", ptzInfo.u8PtzAddr);
				}

				break;
			}
		case 'Z'://设置ipnc ptz参数
			{
				IPNC_PTZ_CFG_S ptzInfo;

				memset(&ptzInfo, 0, sizeof(IPNC_PTZ_CFG_S));

				ptzInfo.u8ProtocalType = 1;//0 pelco-d 1 pelco-p
				ptzInfo.u8PtzAddr = 200;

				if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_PTZ_CFG, &ptzInfo, sizeof(IPNC_PTZ_CFG_S)))//设置参数
					printf("set ipnc ptz param OK....\n");
				else
					printf("set ipnc ptz param failed....\n");	


				break;
			}
		case 'v'://获取ipnc video参数
			{
				IPNC_DEV_VIDEO_CFG_S videoInfo;
				int i = 0;
				memset(&videoInfo, 0, sizeof(IPNC_DEV_VIDEO_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_VIDEO_CFG, &videoInfo, sizeof(IPNC_DEV_VIDEO_CFG_S)))
				{
					printf("get ipnc video param failed....\n");
				}
				else
				{
					//osd
					IPNC_DEV_OSD_CFG_S *pOsdInfo = &videoInfo.struOsd;
					printf("osd----------------------------------------------\n");
					printf("pOsdInfo->u8EnableTitleOsd: %d\n",pOsdInfo->u8EnableTitleOsd);
					printf("pOsdInfo->u8EnableTimeOsd: %d\n",pOsdInfo->u8EnableTimeOsd);
					printf("pOsdInfo->u8DispWeek: %d\n",pOsdInfo->u8DispWeek);
					printf("pOsdInfo->u8EnableBitrateOsd: %d\n",pOsdInfo->u8EnableBitrateOsd);

					printf("pOsdInfo->u8TimeOsdType: %d\n",pOsdInfo->u8TimeOsdType);
					printf("pOsdInfo->u8HourOsdType: %d\n",pOsdInfo->u8HourOsdType);
					printf("pOsdInfo->u8OsdColor: %d\n",pOsdInfo->u8OsdColor);
					printf("pOsdInfo->struTimeOrg.s32Y: %d\n",pOsdInfo->struTimeOrg.s32Y);
					printf("pOsdInfo->struTimeOrg.s32X: %d\n",pOsdInfo->struTimeOrg.s32X);
					printf("pOsdInfo->struTitleOrg.s32Y: %d\n",pOsdInfo->struTitleOrg.s32Y);
					printf("pOsdInfo->struTitleOrg.s32X: %d\n",pOsdInfo->struTitleOrg.s32X);
					printf("pOsdInfo->szChnName: %s\n",pOsdInfo->szChnName);

					//vmain
					IPNC_DEV_VENC_CFG_S *pVmainInfo = &videoInfo.struMainVenc;
					printf("vmain----------------------------------------------\n");
					printf("pVmainInfo->u16Width: %d\n",pVmainInfo->u16Width);
					printf("pVmainInfo->u16Height: %d\n",pVmainInfo->u16Height);
					printf("pVmainInfo->u8VideoEncType: %d\n",pVmainInfo->u8VideoEncType);
					printf("pVmainInfo->u8PicQuilty: %d\n",pVmainInfo->u8PicQuilty);
					printf("pVmainInfo->u8Gop: %d\n",pVmainInfo->u8Gop);
					printf("pVmainInfo->u8FlowType: %d\n",pVmainInfo->u8FlowType);
					printf("pVmainInfo->u32BitrateRate: %d\n",pVmainInfo->u32BitrateRate);
					printf("pVmainInfo->u8FrameRate: %d\n",pVmainInfo->u8FrameRate);

					//vsub
					IPNC_DEV_VENC_CFG_S *pVsubInfo = &videoInfo.struSubVenc;
					printf("vsub----------------------------------------------\n");
					printf("pVsubInfo->u16Width: %d\n",pVsubInfo->u16Width);
					printf("pVsubInfo->u16Height: %d\n",pVsubInfo->u16Height);
					printf("pVsubInfo->u8VideoEncType: %d\n",pVsubInfo->u8VideoEncType);
					printf("pVsubInfo->u8PicQuilty: %d\n",pVsubInfo->u8PicQuilty);
					printf("pVsubInfo->u8Gop: %d\n",pVsubInfo->u8Gop);
					printf("pVsubInfo->u8FlowType: %d\n",pVsubInfo->u8FlowType);
					printf("pVsubInfo->u32BitrateRate: %d\n",pVsubInfo->u32BitrateRate);
					printf("pVsubInfo->u8FrameRate: %d\n",pVsubInfo->u8FrameRate);

					//encode level
					printf("elvl----------------------------------------------\n");
					printf("videoInfo.eEncodeLevel:%d\n", videoInfo.eEncodeLevel);

					//vmask
					IPNC_DEV_VMASK_CFG_S   *pVmaskInfo = &videoInfo.struVMask;
					printf("vmask----------------------------------------------\n");
					printf("pVmaskInfo->u8Open:%d\n", pVmaskInfo->u8Open);
					for(i = 0; i < IPNC_VMASK_REG_NUM; i++)
					{
						printf("pVmaskInfo->struArea[%d].s32X: %d\n", i, pVmaskInfo->struArea[i].s32X);
						printf("pVmaskInfo->struArea[%d].s32Y: %d\n", i, pVmaskInfo->struArea[i].s32Y);
						printf("pVmaskInfo->struArea[%d].u32Height: %d\n", i, pVmaskInfo->struArea[i].u32Height);
						printf("pVmaskInfo->struArea[%d].u32Width: %d\n", i, pVmaskInfo->struArea[i].u32Width);
					}

					//vIma
					IPNC_DEV_IMA_CFG_S    *pImaInfo = &videoInfo.struIma;
					printf("vIma----------------------------------------------\n");
					printf("pImaInfo->eSuppMask: %#x\n", pImaInfo->eSuppMask);
					printf("pImaInfo->eSetMask: %#x\n", pImaInfo->eSetMask);

					printf("pImaInfo->struBrightness.u8Default: %d\n", pImaInfo->struBrightness.u8Default);
					printf("pImaInfo->struBrightness.u8Value: %d\n", pImaInfo->struBrightness.u8Value);
					printf("pImaInfo->struBrightness.u8Step: %d\n", pImaInfo->struBrightness.u8Step);

					printf("pImaInfo->struContrast.u8Default: %d\n", pImaInfo->struContrast.u8Default);
					printf("pImaInfo->struContrast.u8Value: %d\n", pImaInfo->struContrast.u8Value);
					printf("pImaInfo->struContrast.u8Step: %d\n", pImaInfo->struContrast.u8Step);

					printf("pImaInfo->struSaturation.u8Default: %d\n", pImaInfo->struSaturation.u8Default);
					printf("pImaInfo->struSaturation.u8Value: %d\n", pImaInfo->struSaturation.u8Value);
					printf("pImaInfo->struSaturation.u8Step: %d\n", pImaInfo->struSaturation.u8Step);

					printf("pImaInfo->struHue.u8Default: %d\n", pImaInfo->struHue.u8Default);
					printf("pImaInfo->struHue.u8Value: %d\n", pImaInfo->struHue.u8Value);
					printf("pImaInfo->struHue.u8Step: %d\n", pImaInfo->struHue.u8Step);

					printf("pImaInfo->struGamma.u8Default: %d\n", pImaInfo->struGamma.u8Default);
					printf("pImaInfo->struGamma.u8Value: %d\n", pImaInfo->struGamma.u8Value);
					printf("pImaInfo->struGamma.u8Step: %d\n", pImaInfo->struGamma.u8Step);

					printf("pImaInfo->struSharpness.u8Default: %d\n", pImaInfo->struSharpness.u8Default);
					printf("pImaInfo->struSharpness.u8Value: %d\n", pImaInfo->struSharpness.u8Value);
					printf("pImaInfo->struSharpness.u8Step: %d\n", pImaInfo->struSharpness.u8Step);

					printf("pImaInfo->struRed.u8Default: %d\n", pImaInfo->struRed.u8Default);
					printf("pImaInfo->struRed.u8Value: %d\n", pImaInfo->struRed.u8Value);
					printf("pImaInfo->struRed.u8Step: %d\n", pImaInfo->struRed.u8Step);

					printf("pImaInfo->struBlue.u8Default: %d\n", pImaInfo->struBlue.u8Default);
					printf("pImaInfo->struBlue.u8Value: %d\n", pImaInfo->struBlue.u8Value);
					printf("pImaInfo->struBlue.u8Step: %d\n", pImaInfo->struBlue.u8Step);
				}

				break;
			}
		case 'V'://设置ipnc video参数
			{
				int i = 0;
				IPNC_DEV_VIDEO_CFG_S videoInfo;
				IPNC_DEV_OSD_CFG_S     *pOsdInfo   = &videoInfo.struOsd;
				IPNC_DEV_VENC_CFG_S    *pVmainInfo = &videoInfo.struMainVenc;
				IPNC_DEV_VENC_CFG_S    *pVsubInfo  = &videoInfo.struSubVenc;
				IPNC_DEV_VMASK_CFG_S   *pVmaskInfo = &videoInfo.struVMask;
				IPNC_DEV_IMA_CFG_S    *pImaInfo   = &videoInfo.struIma;
				
				memset(&videoInfo, 0, sizeof(IPNC_DEV_VIDEO_CFG_S));
				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_VIDEO_CFG, &videoInfo, sizeof(IPNC_DEV_VIDEO_CFG_S)))
				{
					printf("get ipnc video param failed....\n");
					break;
				}
				
				//osd
				pOsdInfo->u8EnableTitleOsd = 1;
				pOsdInfo->u8EnableTimeOsd = 1;
				pOsdInfo->u8DispWeek = 1;
				pOsdInfo->u8EnableBitrateOsd = 1;
				pOsdInfo->u8TimeOsdType = 0;
				pOsdInfo->u8HourOsdType = 1;
				pOsdInfo->u8OsdColor = 2;
				pOsdInfo->struTimeOrg.s32Y = 120;
				pOsdInfo->struTimeOrg.s32X = 33;
				pOsdInfo->struTitleOrg.s32Y = 360;
				pOsdInfo->struTitleOrg.s32X = 99;
				strcpy(pOsdInfo->szChnName, "howaylee");

				//vmain
				//视频编码宽度
				pVmainInfo->u16Width = 800;
				//视频编码高度
				pVmainInfo->u16Height = 600;
				//视频编码格式0--h264 1--MJPEG
				pVmainInfo->u8VideoEncType = 1;
				//编码质量0-最好，1-次好，2-较好，3-一般，4-较差，5-差	
				pVmainInfo->u8PicQuilty = 2;
				//I帧间隔1-200
				pVmainInfo->u8Gop = 30;
				//码率类型 0--定码流1--变码流
				pVmainInfo->u8FlowType = 1;
				//编码码率 32kbps-16Mkbps
				pVmainInfo->u32BitrateRate = 2048;
				//编码帧率1-25/30
				pVmainInfo->u8FrameRate = 30;

				//vsub
				//视频编码宽度
				pVsubInfo->u16Width = 320;
				//视频编码高度
				pVsubInfo->u16Height = 192;
				//视频编码格式0--h264 1--MJPEG
				pVsubInfo->u8VideoEncType = 1;
				//编码质量0-最好，1-次好，2-较好，3-一般，4-较差，5-差	
				pVsubInfo->u8PicQuilty = 4;
				//I帧间隔1-200
				pVsubInfo->u8Gop = 40;
				//码率类型 0--定码流1--变码流
				pVsubInfo->u8FlowType = 1;
				//编码码率 32kbps-16Mkbps
				pVsubInfo->u32BitrateRate = 1024;
				//编码帧率1-25/30
				pVsubInfo->u8FrameRate = 15;

				//encode level
				videoInfo.eEncodeLevel = 2;

				//vmask
				pVmaskInfo->u8Open = 1;
				for(i = 0; i < IPNC_VMASK_REG_NUM; i++)
				{
					pVmaskInfo->struArea[i].s32X = 20*(i+1);
					pVmaskInfo->struArea[i].s32Y = 20*(i+1);
					pVmaskInfo->struArea[i].u32Height = 100*(i+1);
					pVmaskInfo->struArea[i].u32Width = 100*(i+1);
				}

				//vIma
				//亮度
				pImaInfo->struBrightness.u8Value = 180;
				//对比度
				pImaInfo->struContrast.u8Value = 181;
				//饱和度
				pImaInfo->struSaturation.u8Value = 182;
				//色度
				pImaInfo->struHue.u8Value = 183;
				//伽马
				pImaInfo->struGamma.u8Value = 184;
				//锐度
				pImaInfo->struSharpness.u8Value = 185;
				//红
				pImaInfo->struRed.u8Value = 186;
				//蓝
				pImaInfo->struBlue.u8Value = 187;
				if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_VIDEO_CFG, &videoInfo, sizeof(IPNC_DEV_VIDEO_CFG_S)))//设置参数
					printf("set ipnc video param OK....\n");
				else
					printf("set ipnc video param failed....\n");	

				break;
			}
		case 'a'://获取ipnc audio参数
			{
				IPNC_AUDIO_CFG_S audioInfo;

				memset(&audioInfo, 0, sizeof(IPNC_AUDIO_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_AUDIO_CFG, &audioInfo, sizeof(IPNC_AUDIO_CFG_S)))
				{
					printf("get ipnc audio param failed....\n");
				}
				else
				{
					printf("audioInfo.u32SampleRate:%u\n", audioInfo.u8Open);
					printf("audioInfo.u8AEncType:%u\n", audioInfo.u8Type);
					printf("audioInfo.u32SampleRate:%u\n", audioInfo.u32SampleRate);
					printf("audioInfo.u8AEncType:%u\n", audioInfo.u8AEncType);
				}

				break;
			}
		case 'A'://设置ipnc audio参数
			{
				IPNC_AUDIO_CFG_S audioInfo;

				memset(&audioInfo, 0, sizeof(IPNC_AUDIO_CFG_S));

				audioInfo.u8Open = 1;//0 pelco-d 1 pelco-p
				audioInfo.u8Type = 0;//0--mic麦克输入 1--线输入
				audioInfo.u32SampleRate = 9000;
				audioInfo.u8AEncType = 2;//0--PCM(无效) 1--G711A 2-G711U 3--G726(无效)

				if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_AUDIO_CFG, &audioInfo, sizeof(IPNC_AUDIO_CFG_S)))//设置参数
					printf("set ipnc audio param OK....\n");
				else
					printf("set ipnc audio param failed....\n");	

				break;
			}
		case 'r'://获取ipnc user参数
			{
				IPNC_USER_CFG_S userInfo;

				memset(&userInfo, 0, sizeof(IPNC_USER_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_USR_CFG, &userInfo, sizeof(IPNC_USER_CFG_S)))
				{
					printf("get ipnc user param failed....\n");
				}
				else
				{
					printf("userInfo.userName:%s\n", userInfo.userName);
					printf("userInfo.passWord:%s\n", userInfo.passWord);

				}

				break;
			}
		case 'R'://设置ipnc user参数
			{
				IPNC_USER_CFG_S userInfo;

				memset(&userInfo, 0, sizeof(IPNC_USER_CFG_S));

				strcpy(userInfo.userName, "lhw");//可以设置新的名字, 若是只修改密码则先获取用户信息,保留原始用户名

				//设置密码
				strcpy(userInfo.passWord, "080629");
				strcpy(userInfo.cfmPassWord, "080629");//确认密码

				if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_USR_CFG, &userInfo, sizeof(IPNC_USER_CFG_S)))//设置参数
					printf("set ipnc user param OK....\n");
				else
					printf("set ipnc user param failed....\n");

				break;
			}
		case 'k'://获取ipnc md移动侦测参数
			{
				IPNC_DEV_MD_CFG_S mdInfo;
				
				memset(&mdInfo, 0, sizeof(IPNC_DEV_MD_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_VMD_CFG, &mdInfo, sizeof(IPNC_DEV_MD_CFG_S)))
				{
					printf("get ipnc user md failed....\n");
				}
				else
				{
					int i, j;
					printf("----------------------------------------------\n");
					printf("mdInfo.u8Enable: %d\n",mdInfo.u8Enable);
					printf("mdInfo.u8Level: %d\n",mdInfo.u8Level);
					for(i = 0; i < IPNC_VMD_REG_NUM; i++)
					{
						printf("----------------------------------------------\n");
						printf("mdInfo.struRect[%d].s32X: %d\n", i, mdInfo.struRect[i].s32X);
						printf("mdInfo.struRect[%d].s32Y: %d\n", i, mdInfo.struRect[i].s32Y);
						printf("mdInfo.struRect[%d].u32Height: %d\n", i, mdInfo.struRect[i].u32Height);
						printf("mdInfo.struRect[%d].u32Width: %d\n", i, mdInfo.struRect[i].u32Width);
					}
					for(i = 0; i < IPNC_MAX_SEG_NUM; i++)
					{
						printf("----------------------------------------------\n");
						for(j = 0; j < 8; j++)
						{
							printf("mdInfo.struSeg[%d][%d].u8Open: %d\n", i, j, mdInfo.struSeg[i][j].u8Open);
							printf("mdInfo.struSeg[%d][%d].beginSec: %d\n", i, j, mdInfo.struSeg[i][j].beginSec);
							printf("mdInfo.struSeg[%d][%d].endSec: %d\n", i, j, mdInfo.struSeg[i][j].endSec);
							
						}
					}
					printf("----------------------------------------------\n");
					printf("mdInfo.struLinkAct.u32OutPut: %d\n",mdInfo.struLinkAct.u32OutPut);
					printf("mdInfo.struLinkAct.u32OutPersist: %d\n",mdInfo.struLinkAct.u32OutPersist);
					printf("----------------------------------------------\n");
					printf("mdInfo.struLinkAct.u32ShootChannel: %d\n",mdInfo.struLinkAct.u32ShootChannel);
					printf("mdInfo.struLinkAct.u8ShootNum: %d\n",mdInfo.struLinkAct.u8ShootNum);
					printf("mdInfo.struLinkAct.u8ShootMode: %d\n",mdInfo.struLinkAct.u8ShootMode);
					printf("mdInfo.struLinkAct.u32ShootInterval: %d\n",mdInfo.struLinkAct.u32ShootInterval);
					printf("----------------------------------------------\n");
					printf("mdInfo.struLinkAct.u32RecChannel: %d\n",mdInfo.struLinkAct.u32RecChannel);
					printf("mdInfo.struLinkAct.u32RecTime: %d\n",mdInfo.struLinkAct.u32RecTime);
					printf("pmdInfo.struLinkAct.u8RecMode: %d\n",mdInfo.struLinkAct.u8RecMode);
					printf("mdInfo.struLinkAct.u32PCRecChannel: %d\n",mdInfo.struLinkAct.u32PCRecChannel);
					printf("----------------------------------------------\n");
					printf("mdInfo.struLinkAct.u8Email: %d\n",mdInfo.struLinkAct.u8Email);
					printf("mdInfo.u32DetectInterval: %d\n",mdInfo.u32DetectInterval);

				}
				break;
			}
		case 'K'://设置ipnc md移动侦测参数
			{
				IPNC_DEV_MD_CFG_S mdInfo;
				int i, j;
				memset(&mdInfo, 0, sizeof(IPNC_DEV_MD_CFG_S));

				//移动侦测使能 0--关闭 1--开启
				mdInfo.u8Enable = 1;
				//侦测灵敏度 0--很高 1--高 2--中 3--低 4--很低
				mdInfo.u8Level = 2;
				//侦测区域
				for(i = 0; i < IPNC_VMD_REG_NUM; i++)
				{
					mdInfo.struRect[i].s32X = i*i*20;
					mdInfo.struRect[i].s32Y = i*i*20;
					mdInfo.struRect[i].u32Height = 50;
					mdInfo.struRect[i].u32Width = 50;
				}
				//侦测时段
				for(i = 0; i < IPNC_MAX_SEG_NUM; i++)
				{

					for(j = 0; j < 8; j++)
					{
						mdInfo.struSeg[i][j].u8Open   = 1;
						mdInfo.struSeg[i][j].beginSec = 0;
						mdInfo.struSeg[i][j].endSec   = 6*3600;
					}
				}
				//联动输出
				mdInfo.struLinkAct.u32OutPut = 1;
				//联动输出持续时间
				mdInfo.struLinkAct.u32OutPersist = 5;

				//联动抓拍
				mdInfo.struLinkAct.u32ShootChannel = 1;
				//抓拍数
				mdInfo.struLinkAct.u8ShootNum = 5;
				//抓拍存储模式
				mdInfo.struLinkAct.u8ShootMode = 3;
				//抓拍持续时间
				mdInfo.struLinkAct.u32ShootInterval = 5;

				//录像输出
				mdInfo.struLinkAct.u32RecChannel = 1;
				//录像持续时间
				mdInfo.struLinkAct.u32RecTime = 5;
				//录像模式
				mdInfo.struLinkAct.u8RecMode = 1;
				//联动PC录像通道
				mdInfo.struLinkAct.u32PCRecChannel = 1;
				
				//联动email
				mdInfo.struLinkAct.u8Email = 1;

				//检测间隔时间
				mdInfo.u32DetectInterval = 0;

				if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_VMD_CFG, &mdInfo, sizeof(IPNC_DEV_MD_CFG_S)))//设置参数
					printf("set ipnc md param OK....\n");
				else
					printf("set ipnc md param failed....\n");
				break;
			}
		case 'x'://获取ipnc di探头报警参数
			{
				IPNC_DEV_DI_CFG_S diInfo;
				
				memset(&diInfo, 0, sizeof(IPNC_DEV_DI_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_DI_CFG, &diInfo, sizeof(IPNC_DEV_DI_CFG_S)))
				{
					printf("get ipnc user di failed....\n");
				}
				else
				{
					int i, j;
					printf("----------------------------------------------\n");
					printf("diInfo.u8Enable: %d\n",diInfo.u8Enable);
					printf("diInfo.chDiName: %s\n",diInfo.chDiName);
					printf("diInfo.u8DiType: %d\n",diInfo.u8DiType);

					for(i = 0; i < IPNC_MAX_SEG_NUM; i++)
					{
						printf("----------------------------------------------\n");
						for(j = 0; j < 8; j++)
						{
							printf("diInfo.struSeg[%d][%d].u8Open: %d\n", i, j, diInfo.struSeg[i][j].u8Open);
							printf("diInfo.struSeg[%d][%d].beginSec: %d\n", i, j, diInfo.struSeg[i][j].beginSec);
							printf("diInfo.struSeg[%d][%d].endSec: %d\n", i, j, diInfo.struSeg[i][j].endSec);
							
						}
					}
					printf("----------------------------------------------\n");
					printf("diInfo.struLinkAct.u32OutPut: %d\n",diInfo.struLinkAct.u32OutPut);
					printf("diInfo.struLinkAct.u32OutPersist: %d\n",diInfo.struLinkAct.u32OutPersist);
					printf("----------------------------------------------\n");
					printf("diInfo.struLinkAct.u32ShootChannel: %d\n",diInfo.struLinkAct.u32ShootChannel);
					printf("diInfo.struLinkAct.u8ShootNum: %d\n",diInfo.struLinkAct.u8ShootNum);
					printf("diInfo.struLinkAct.u8ShootMode: %d\n",diInfo.struLinkAct.u8ShootMode);
					printf("diInfo.struLinkAct.u32ShootInterval: %d\n",diInfo.struLinkAct.u32ShootInterval);
					printf("----------------------------------------------\n");
					printf("diInfo.struLinkAct.u32RecChannel: %d\n",diInfo.struLinkAct.u32RecChannel);
					printf("diInfo.struLinkAct.u32RecTime: %d\n",diInfo.struLinkAct.u32RecTime);
					printf("pdiInfo.struLinkAct.u8RecMode: %d\n",diInfo.struLinkAct.u8RecMode);
					printf("diInfo.struLinkAct.u32PCRecChannel: %d\n",diInfo.struLinkAct.u32PCRecChannel);
					printf("----------------------------------------------\n");
					printf("diInfo.struLinkAct.u8Email: %d\n",diInfo.struLinkAct.u8Email);
					printf("diInfo.u32DetectInterval: %d\n",diInfo.u32DetectInterval);

				}
				break;
			}
		case 'X'://设置ipnc di探头报警参数
		{
				IPNC_DEV_DI_CFG_S diInfo;
				int i, j;
				memset(&diInfo, 0, sizeof(IPNC_DEV_DI_CFG_S));

				//探头报警使能 0--关闭 1--开启
				diInfo.u8Enable = 1;
				//探头名称
				//strcpy(diInfo.chDiName, "test");
				//探头类型 0--常开探头 1--常闭探头
				diInfo.u8DiType = 1;

				//侦测时段
				for(i = 0; i < IPNC_MAX_SEG_NUM; i++)
				{

					for(j = 0; j < 8; j++)
					{
						diInfo.struSeg[i][j].u8Open   = 1;
						diInfo.struSeg[i][j].beginSec = 0;
						diInfo.struSeg[i][j].endSec   = 6*3600;
					}
				}
				//联动输出
				diInfo.struLinkAct.u32OutPut = 1;
				//联动输出持续时间
				diInfo.struLinkAct.u32OutPersist = 5;

				//联动抓拍
				diInfo.struLinkAct.u32ShootChannel = 1;
				//抓拍数
				diInfo.struLinkAct.u8ShootNum = 5;
				//抓拍存储模式
				diInfo.struLinkAct.u8ShootMode = 3;
				//抓拍持续时间
				diInfo.struLinkAct.u32ShootInterval = 5;

				//录像输出
				diInfo.struLinkAct.u32RecChannel = 1;
				//录像持续时间
				diInfo.struLinkAct.u32RecTime = 5;
				//录像模式
				diInfo.struLinkAct.u8RecMode = 1;
				//联动PC录像通道
				diInfo.struLinkAct.u32PCRecChannel = 1;
				
				//联动email
				diInfo.struLinkAct.u8Email = 1;

				//检测间隔时间
				diInfo.u32DetectInterval = 0;

				if(-1 != nvr_set_ipnc_param(0, IPNC_CMD_DI_CFG, &diInfo, sizeof(IPNC_DEV_DI_CFG_S)))//设置参数
					printf("set ipnc di param OK....\n");
				else
					printf("set ipnc di param failed....\n");
			break;
		}
		case 'l'://获取 ipnc 3A配置
			{
				IPNC_DEV_3A_CFG_S dev3aInfo;

				memset(&dev3aInfo, 0, sizeof(IPNC_DEV_3A_CFG_S));

				if(-1 == nvr_get_ipnc_param(0, IPNC_CMD_3A_CFG, &dev3aInfo, sizeof(IPNC_DEV_3A_CFG_S)))
				{
					printf("get ipnc 3a param failed....\n");
				}
				else
				{
					//翻转图像上下翻转
					printf("dev3aInfo.struFlip.u8Default:%d\n", dev3aInfo.struFlip.u8Default);
					printf("dev3aInfo.struFlip.u8Value:%d\n", dev3aInfo.struFlip.u8Value);
					//镜像图像左右翻转
					printf("dev3aInfo.struMirror.u8Default:%d\n", dev3aInfo.struMirror.u8Default);
					printf("dev3aInfo.struMirror.u8Value:%d\n", dev3aInfo.struMirror.u8Value);
					//电源频率
					printf("dev3aInfo.struPwdFreq.u8Default:%d\n", dev3aInfo.struPwdFreq.u8Default);
					printf("dev3aInfo.struPwdFreq.u8Value:%d\n", dev3aInfo.struPwdFreq.u8Value);
					//场景,0:自动 1:室内2:室外
					printf("dev3aInfo.struScene.u8Default:%d\n", dev3aInfo.struScene.u8Default);
					printf("dev3aInfo.struScene.u8Value:%d\n", dev3aInfo.struScene.u8Value);
					//红外模式，0:自动，1:白天，2:晚上,3:定时模式
					printf("dev3aInfo.struIrcfMode.u8Default:%d\n", dev3aInfo.struIrcfMode.u8Default);
					printf("dev3aInfo.struIrcfMode.u8Value:%d\n", dev3aInfo.struIrcfMode.u8Value);
					//电子快门 0-8  0：自动快门 1：											
					//1/25S， 2：1/50S， 3：1/100S， 4：1/500S， 5：1/1000S， 	
					//6：1/2000S， 7：1/5000S， 8：1/10000S
					printf("dev3aInfo.struShutterSpeed.u8Default:%d\n", dev3aInfo.struShutterSpeed.u8Default);
					printf("dev3aInfo.struShutterSpeed.u8Value:%d\n", dev3aInfo.struShutterSpeed.u8Value);

				}
				break;
			}
		case 'L'://设置 ipnc 3A配置
			{
				
				break;
			}
		
		case 'w'://恢复出厂
			{
				nvr_sw_reset();
				break;
			}


		default:
			break;
	}
	quit = ch;
	while(1)
	{	
		if('\n' == quit)
		{
			goto _ENTER;
		}
		else if('q' != quit && 'Q' != quit)
		{
			ch = '0';
			goto _TEST;
		}
		else
			break;
	}

//	nvr_uninit_server();
	return 0;
}
#endif
