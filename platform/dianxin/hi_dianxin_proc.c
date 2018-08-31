/////////////////////////////////////////////////////////////////////////////////
////////////////////////// hi_dianxin_proc.c ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "hi_dianxin_proc.h"
#include "mxml.h"
#include "hi_dianxin_common.h"
#include "hi_dianxin_turn.h"
#include "hi_rtsp_global.h"
#include "net_api.h"
#include "nvr_lib_interface.h"
#include "storage_lib.h"
#include "storage_interface.h"
#include "hi_ftp_api.h"
#include "hi_ftp.h"


char szDevID[12];

#define TIMEOUT 3000
#define PTZSPEED 50

typedef int (* DIANXIN_FUNC_PROC)(DIANXIN_CTRL *pDianxinCtrl, char *pBuf, int nBufLen);

typedef struct _DIANXIN_FUNC_STRUCT_
{
	DIANXIN_CMD_TYPE eCmdType;
	DIANXIN_FUNC_PROC func_proc;
}DIANXIN_FUNC_STRUCT;



/*
int hi_dianxin_group_connect(char *pGroupUrl)
{
	char szIp[32];
	int nPort;
	struct sockaddr_in stSockAddr;
	hi_dianxin_parse_url(pGroupUrl, szIp, nPort);
	stDianxinCtrl.stGroupCtrl.sock = socket(AF_INET, STREAM, 0);
	if(stDianxinCtrl.stGroupCtrl.sock < 0)
	{
		perror("Can't creat group socket");
		return -1;
	}

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(nPort);
	stSockAddr.sin_addr.s_addr = inet_addr(szIp);

	if(connect(stDianxinCtrl.stGroupCtrl.sock, (struct sockaddr *)&stSockAddr, sizeof(struct sockaddr)) < 0)
	{
		perror("Can't connect the group platform");
		close(stDianxinCtrl.stGroupCtrl.sock);
		return -1;
	}
	
	return 0;
}
*/

static int hi_dianxin_group_register_body(HI_DIANXIN_PLATRORM_CFG *pDianxinPlatform, char *pBodyBuf, int nBodyLen)
{
	char str[8];
	mxml_node_t *pHead, *pRedirect, *pNode;
	NVR_DEV_INFO_S stDevCfg;
	char szStr[64];
	nvr_logon_get_dev_cfg(0, &stDevCfg);

	pHead = mxmlNewXML("1.0");
	pRedirect = mxmlNewElement(pHead, "REDIRECT_REQ");
	pNode = mxmlNewElement(pRedirect, "ManufacturerNo");
	
	strncpy(szStr, pDianxinPlatform->szDevId, 3);
	mxmlNewText(pNode, 0, szStr);
	pNode = mxmlNewElement(pRedirect, "Mode");
	mxmlNewText(pNode, 0, pDianxinPlatform->szMode);
	pNode = mxmlNewElement(pRedirect, "DevType");
	sprintf(str, "%d", EN_TYPE_NVR);
	mxmlNewText(pNode, 0, str);
	pNode = mxmlNewElement(pRedirect, "DevID");
	mxmlNewText(pNode, 0, pDianxinPlatform->szDevId);
	pNode = mxmlNewElement(pRedirect, "DevKey");
	mxmlNewText(pNode, 0, pDianxinPlatform->szKey);		//ʨ±¸ºϷ¨єɏ֤Ϊʨ±¸ID
	mxmlSaveString(pHead, pBodyBuf, nBodyLen, MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	return strlen(pBodyBuf);	
}

static int hi_dianxin_register_PostSubmit(int fd, 
									char * url, char *host, char *port, char *body)
{
	int bodysize;
	char headerbuf[512];
	int headerssize;
	bodysize = (int)strlen(body);

	headerssize = snprintf(headerbuf, sizeof(headerbuf),
                       "POST %s HTTP/1.1\r\n"
	                   "Host: %s:%s\r\n"
					   "User-Agent: IPCM/1.3\r\n"
	                   "Content-Length: %d\r\n"
					   "Content-Type: text/xml;charset=UTF-8\r\n"
					   "Connection: Keep-Alive\r\n"
					   "Cache-Control: no-cache\r\n"	/* ??? */
					   "\r\n",
					   url, host, port, bodysize);
	__D("the group head is %s!\r\n", headerbuf);
	return hi_dianxin_http_write(fd, body, bodysize, headerbuf, headerssize);
}

static int hi_dainxin_group_body_parse(char *pRecvBuf, char *pProvincialUrl)
{
	mxml_node_t *head, *node, *pRedict;
	int nResult;
	char stDevID[16];
	char szLcmsUrl[64];
	char *pBodyMsg;
	
	pBodyMsg = strstr(pRecvBuf, "<?xml");
	head = mxmlLoadString(NULL, pBodyMsg, MXML_TEXT_CALLBACK);
	if((pRedict = mxmlGetFirstChild(head)) == NULL)
	{
		__E("the group invite redict erro!\r\n");
		mxmlDelete(head);
		return -1;
	}
	if((node = dianxin_mxml_node_get(pRedict, "Result")) == NULL)
	{
		__E("the group can not find Result!\r\n");
		mxmlDelete(head);
		return -1;
	}
	nResult = atoi(node->value.text.string);
	__D("the nResult is %d\r\n", nResult);
	if((node = dianxin_mxml_node_get(pRedict, "DevID")) == NULL)
	{
		__E("the group can not find DevID!\r\n");
		mxmlDelete(head);
		return -1;
	}
	strcpy(stDevID, node->value.text.string);
	__D("the stDevID is %s!\r\n", stDevID);
	if((node = dianxin_mxml_node_get(pRedict, "LcmsUrl")) ==  NULL)
	{
		__E("the group can not find LcmsUrl!\r\n");
		mxmlDelete(head);
		return -1;
	}
	strcpy(szLcmsUrl, node->value.text.string);
	mxmlDelete(head);
	__D("the szLcmsUrl is %s!\r\n", szLcmsUrl);
	if(nResult == 0)
	{
		strcpy(pProvincialUrl, szLcmsUrl);
		return 0;
	}
	return -1;
	
}

int hi_dianxin_group_register(DIANXIN_CTRL *pDianxinCtrl)
{
	char szIp[32];
	char szPort[8];
	char szUrl[64];
	int nFd;	
	int ret = -1;
	char szSendBodyBuf[512];
	char szRecvBuf[512];
	HI_DIANXIN_PLATRORM_CFG *pPlatformCfg = &pDianxinCtrl->stPlatformCfg;
	__D("hi_dianxin_group_register!\r\n");
	if(hi_dianxin_parse_url(pPlatformCfg->szGroupUrl, szIp, szPort, szUrl) < 0)
	{
		return -1;
	}
	if((nFd = net_tcp_noblock_connect(NULL, NULL, szIp, szPort, TIMEOUT)) <= 0)
	{
		__D("the group connect err!\r\n");
		return -2;
	}
	memset(szSendBodyBuf, 0, sizeof(szSendBodyBuf));
	hi_dianxin_group_register_body(pPlatformCfg, szSendBodyBuf, sizeof(szSendBodyBuf));
	if(hi_dianxin_register_PostSubmit(nFd, szUrl, szIp, szPort, szSendBodyBuf) < 0)
	{
		__D("the group Post err!\r\n");
		net_close_socket(&nFd);
		return -1;
	}
	if(hi_dianxin_register_recv(nFd, szRecvBuf, sizeof(szRecvBuf)) == 0)
	{
		ret = hi_dainxin_group_body_parse(szRecvBuf, pPlatformCfg->szProvincialUrl);
		net_close_socket(&nFd);
		return ret;
	}
	net_close_socket(&nFd);
	return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

static int hi_dianxin_lcms_register_body(HI_DIANXIN_PLATRORM_CFG *pDianxinPlatform, char *pBodyBuf, int nBodyLen)
{
	char acDevType[8];
	mxml_node_t *pHead, *pRegister, *pNode;
	char szStr[32];

	pHead = mxmlNewXML("1.0");
	pRegister = mxmlNewElement(pHead, "REGISTER_REQ");
	pNode = mxmlNewElement(pRegister, "ManufacturerNo");
	
	strncpy(szStr, pDianxinPlatform->szDevId, 3);
	mxmlNewText(pNode, 0, szStr);
	pNode = mxmlNewElement(pRegister, "Mode");
	mxmlNewText(pNode, 0, pDianxinPlatform->szMode);
	pNode = mxmlNewElement(pRegister, "DevType");
	sprintf(acDevType, "%d", EN_TYPE_NVR);
	mxmlNewText(pNode, 0, acDevType);
	pNode = mxmlNewElement(pRegister, "DevID");	
	mxmlNewText(pNode, 0, pDianxinPlatform->szDevId);
	pNode = mxmlNewElement(pRegister, "DevKey");
	mxmlNewText(pNode, 0, pDianxinPlatform->szKey);
	mxmlSaveString(pHead, pBodyBuf, nBodyLen, MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	//printf(">>>>>>>>>>.the buf is %s!\r\n", pBodyBuf);
	return strlen(pBodyBuf);	
}


int hi_dainxin_lcms_body_parse(char *pRecvBuf, HI_DIANXIN_REGISTER_RES *pDinxinRegisterRes)
{
	mxml_node_t *pHead, *pRegister, *pNode;
	int nResult;
	char *pBodyMsg;

	NVR_FTP_CFG_S stFtpCfg;
	nvr_logon_get_ftp_cfg(0, &stFtpCfg);
	pBodyMsg = strstr(pRecvBuf, "<?xml");
	printf(">>>>>>>######the pBodyMsg is %s\r\n", pBodyMsg);
	pHead = mxmlLoadString(NULL, pBodyMsg, MXML_TEXT_CALLBACK);
	if((pRegister = dianxin_mxml_node_get(pHead, "REGISTER_RES")) == NULL)
	{
		__E("the lmcs invite redict erro!\r\n");
		mxmlDelete(pHead);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pRegister, "Result")) == NULL)
	{
		printf("Can not find result!\r\n");
		mxmlDelete(pHead);
		return -1;
	}
	if(mxmlGetText(pNode, 0))
		nResult = atoi(MXMLGETTEXT(pNode, 0));
	else
		nResult = 255;
	__D(">>>the result is %d\r\n", nResult);
	if(nResult == 1)
	{
		__E("the device is not exist!\r\n");
		mxmlDelete(pHead);
		return -1;
	}else if(nResult == 2)
	{
		__E("the key validation error!\r\n");
		mxmlDelete(pHead);
		return -1;
	}else if(nResult == 3)
	{
		__E("the firmware version not support!\r\n");
		mxmlDelete(pHead);
		return -1;
	}else if(nResult == 255)
	{
		__E("other error!\r\n");
		mxmlDelete(pHead);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pRegister, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pRegister, "NatIP")) == NULL)
	{
		mxmlDelete(pHead);
		return -1;
	}
	//strcpy(pDinxinRegisterRes->szNatIP, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(pDinxinRegisterRes->szNatIP, pNode);
	if((pNode = dianxin_mxml_node_get(pRegister, "NatPort")) == NULL)
	{
		mxmlDelete(pHead);
		return -1;
	}
	//strcpy(pDinxinRegisterRes->szNatPort, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(pDinxinRegisterRes->szNatPort, pNode);
	if((pNode = dianxin_mxml_node_get(pRegister, "BackupNatIP")) == NULL)
	{
		mxmlDelete(pHead);
		return -1;
	}
	//strcpy(pDinxinRegisterRes->szBackupNatIP, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(pDinxinRegisterRes->szBackupNatIP, pNode);
	if((pNode = dianxin_mxml_node_get(pRegister, "BackupNatPort")) == NULL)
	{
		mxmlDelete(pHead);
		return -1;
	}
	//strcpy(pDinxinRegisterRes->szBackupNatPort, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(pDinxinRegisterRes->szBackupNatPort, pNode);
	if((pNode = dianxin_mxml_node_get(pRegister, "TurnIP")) == NULL)
	{
		mxmlDelete(pHead);
		return -1;
	}
	//strcpy(pDinxinRegisterRes->szTurnIP, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(pDinxinRegisterRes->szTurnIP, pNode);
	if((pNode = dianxin_mxml_node_get(pRegister, "TurnPort")) == NULL)
	{
		mxmlDelete(pHead);
		return -1;
	}
	//strcpy(pDinxinRegisterRes->szTurnPort, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(pDinxinRegisterRes->szTurnPort, pNode);
	if((pNode = dianxin_mxml_node_get(pRegister, "BackupTurnIP")) == NULL)
	{
		mxmlDelete(pHead);
		return -1;
	}
	//strcpy(pDinxinRegisterRes->szBackupTurnIP, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(pDinxinRegisterRes->szBackupTurnIP, pNode);
	if((pNode = dianxin_mxml_node_get(pRegister, "BackupTurnPort")) == NULL)
	{
		mxmlDelete(pHead);
		return -1;
	}
	//strcpy(pDinxinRegisterRes->szBackupTurnPort, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(pDinxinRegisterRes->szBackupTurnPort, pNode);
	if((pNode = dianxin_mxml_node_get(pRegister, "FtpIP")) == NULL)
	{
		mxmlDelete(pHead);
		return -1;
	}
	//strcpy(pDinxinRegisterRes->szFtpIP, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(pDinxinRegisterRes->szFtpIP, pNode);
	if((pNode = dianxin_mxml_node_get(pRegister, "FtpPort")) == NULL)
	{
		mxmlDelete(pHead);
		return -1;
	}
	//strcpy(pDinxinRegisterRes->szFtpPort, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(pDinxinRegisterRes->szFtpPort, pNode);
	if((pNode = dianxin_mxml_node_get(pRegister, "FtpAccount")) == NULL)
	{
		mxmlDelete(pHead);
		return -1;
	}
	//strcpy(pDinxinRegisterRes->szFtpAccount, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(pDinxinRegisterRes->szFtpAccount, pNode);
	if((pNode = dianxin_mxml_node_get(pRegister, "FtpPassword")) == NULL)
	{
		mxmlDelete(pHead);
		return -1;
	}
	//strcpy(pDinxinRegisterRes->szFtpPassword, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(pDinxinRegisterRes->szFtpPassword, pNode);
	if((pNode = dianxin_mxml_node_get(pRegister, "Time")) == NULL)
	{
		mxmlDelete(pHead);
		return -1;
	}
	//strcpy(pDinxinRegisterRes->szTime, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(pDinxinRegisterRes->szTime, pNode);
	mxmlDelete(pHead);

	strcpy((char *)stFtpCfg.szFtpSvr, pDinxinRegisterRes->szFtpIP);
	strcpy((char *)stFtpCfg.szFtpUsr, pDinxinRegisterRes->szFtpAccount);
	strcpy((char *)stFtpCfg.szFtpPsw, pDinxinRegisterRes->szFtpPassword);
	stFtpCfg.u16FtpPort = atoi(pDinxinRegisterRes->szFtpPort);

	nvr_logon_set_ftp_cfg(0, &stFtpCfg);
	
	__D("hi_dainxin_lcms_body_parse end!\r\n");
	return 0;
}

int hi_dianxin_lcms_register(DIANXIN_CTRL *pDianxinCtrl)
{
	char szIp[32];
	char szPort[8];
	char szUrl[64];
	int nFd;
	int ret = -1;
	char szSendBodyBuf[512];
	char szRecvBuf[1024];
	HI_DIANXIN_PLATRORM_CFG *pDianxinPlatform = &pDianxinCtrl->stPlatformCfg;
	HI_DIANXIN_REGISTER_RES *pDinxinRegisterRes = &pDianxinCtrl->stRegister;
	__D("hi_dianxin_lcms_register!\r\n");
	if(hi_dianxin_parse_url(pDianxinPlatform->szProvincialUrl, szIp, szPort, szUrl) < 0)
	{
		return -1;
	}
	if((nFd = net_tcp_noblock_connect(NULL, NULL, szIp, szPort, TIMEOUT)) <= 0)
	{
		__D("the lcms connect err!\r\n");
		return -2;
	}
	memset(szSendBodyBuf, 0, sizeof(szSendBodyBuf));
	hi_dianxin_lcms_register_body(pDianxinPlatform, szSendBodyBuf, sizeof(szSendBodyBuf));
	if(hi_dianxin_register_PostSubmit(nFd, szUrl, szIp, szPort, szSendBodyBuf) < 0)
	{
		__D("the lcms post data err!\r\n");
		net_close_socket(&nFd);
		return -1;
	}
	if(hi_dianxin_register_recv(nFd, szRecvBuf, sizeof(szRecvBuf)) == 0)
	{
		ret = hi_dainxin_lcms_body_parse(szRecvBuf, pDinxinRegisterRes);
		net_close_socket(&nFd);
		return ret;
	}
	net_close_socket(&nFd);
	return -1;
}
////////////////////////////////////////////////////////////////////////////////////
int hi_dianxin_login_body_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	char szStr[64];
	char szMon[8];
	int nYear, nDay;
	int i = 0;
	const char *mon[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	mxml_node_t *pHead, *pLogin, *pNode;

	HI_DIANXIN_PLATRORM_CFG *pDianxinPlatform = &pDianxinCtrl->stPlatformCfg;
	NVR_DEV_INFO_S stDevInfo;
	NVR_DEV_NET_CFG_S stNetCfg;
	NVR_UPNP_CFG_S stUpnpCfg;
	char szRelayIp[NVR_IP_ADDR_LEN] = {0};
	int nRelayPort = 0;
	int nNatType;
	
	if(hi_turn_get_relay_info(szRelayIp, sizeof(szRelayIp), &nRelayPort) < 0)
		printf("Get turn relay information failed.\n");
	else
	{
		strcpy(pDianxinCtrl->szRelayIp, szRelayIp);
		pDianxinCtrl->nRelayPort = nRelayPort;
	}

	nvr_logon_get_dev_cfg(0, &stDevInfo);
	nvr_logon_get_net_cfg(0, &stNetCfg);
	nvr_logon_get_upnp_cfg(0, &stUpnpCfg);

	pHead = mxmlNewXML("1.0");
	pLogin = mxmlNewElement(pHead, "LOGIN_REQ");
	
	pNode = mxmlNewElement(pLogin, "DevVer");
	memset(szStr, 0, sizeof(szStr));
	sscanf(__DATE__, "%s %d %d", szMon, &nDay, &nYear);
	for(i = 0; i < 12; i++)
	{
		if(strcmp(mon[i], szMon) == 0)
		{
			break;
		}
	}
	sprintf(szStr, "V1.6_%04d%02d%02d", nYear, i+1, nDay);
	//stDevInfo.stStableInfo.szSoftVer[0]='V';
	//strcpy(szStr, stDevInfo.stStableInfo.szSoftVer);
	//szStr[0]='V';
	mxmlNewText(pNode, 0, szStr);
	
	pNode = mxmlNewElement(pLogin, "DevID");
	mxmlNewText(pNode, 0, pDianxinPlatform->szDevId);
	
	pNode = mxmlNewElement(pLogin, "DevKey");
	mxmlNewText(pNode, 0, pDianxinPlatform->szKey);
	
	pNode = mxmlNewElement(pLogin, "UpnpIP");
	strcpy(szStr, stUpnpCfg.szExternalIp);
	mxmlNewText(pNode, 0, szStr);
	
	pNode = mxmlNewElement(pLogin, "RtspUpnpPort");
	sprintf(szStr, "%d", stUpnpCfg.u16UpnpRtspPortS);
	mxmlNewText(pNode, 0, szStr);
	
	pNode = mxmlNewElement(pLogin, "LocalIP");
	memset(szStr, 0, sizeof(szStr));
	if(stNetCfg.struPppoeCfg.u8PppoeOn && stNetCfg.struPppoeCfg.u8PppoeStatus == 2)
	{
		sprintf(szStr, "%d.%d.%d.%d", 
			(stNetCfg.struPppoeCfg.u32PppoeIp >> 24) & 0xff,
			(stNetCfg.struPppoeCfg.u32PppoeIp >> 16) & 0xff,
			(stNetCfg.struPppoeCfg.u32PppoeIp >> 8) & 0xff,
			(stNetCfg.struPppoeCfg.u32PppoeIp >> 0) & 0xff);
	}else
	{
		sprintf(szStr, "%d.%d.%d.%d", 
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 24) & 0xff,
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 16) & 0xff,
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 8) & 0xff,
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 0) & 0xff);
	}
	mxmlNewText(pNode, 0, szStr);

	pNode = mxmlNewElement(pLogin, "RtspLocalPort");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", pDianxinPlatform->localRtspPort);
	mxmlNewText(pNode, 0, szStr);

	pNode = mxmlNewElement(pLogin, "TurnRelayIP");
	mxmlNewText(pNode, 0, szRelayIp);	
	pNode = mxmlNewElement(pLogin, "RtspTurnRelayPort");
	sprintf(szStr, "%d", nRelayPort);
	mxmlNewText(pNode, 0, szStr);

	pNode = mxmlNewElement(pLogin, "NetType");
	mxmlNewText(pNode, 0, "0");	// ����

	//if(pUpnpCfg->u8UpnpEthNo == 0)
		nNatType = hi_turn_get_nat_type("eth0", pDianxinCtrl->stRegister.szTurnIP, atoi(pDianxinCtrl->stRegister.szTurnPort));
	//else if(pUpnpCfg->u8UpnpEthNo == 1)
	//	nNatType = hi_turn_get_nat_type("ra0", pDianxinCtrl->stRegister.szTurnIP, atoi(pDianxinCtrl->stRegister.szTurnPort));
	//else if(pUpnpCfg->u8UpnpEthNo == 2)
	//	nNatType = hi_turn_get_nat_type("ppp0", pDianxinCtrl->stRegister.szTurnIP, atoi(pDianxinCtrl->stRegister.szTurnPort));
	//else
	//	nNatType = hi_turn_get_nat_type("eth0", pDianxinCtrl->stRegister.szTurnIP, atoi(pDianxinCtrl->stRegister.szTurnPort));

	pNode = mxmlNewElement(pLogin, "NatType");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", nNatType);
	printf(">>>>>>>>>>>>>>>the nNatType is %d\r\n", nNatType);
	if(stNetCfg.struPppoeCfg.u8PppoeOn && stNetCfg.struPppoeCfg.u8PppoeStatus == 2)
	{
		mxmlNewText(pNode, 0, "1");
	}else
	{
		mxmlNewText(pNode, 0, "1");
	}

	mxmlSaveString(pHead, pBodyBuf, nBodyLen, MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	return 0;
}

int hi_dianxin_login_req(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[512];
	if(hi_dianxin_login_body_req(pDianxinCtrl, szSendBuf, sizeof(szSendBuf)) < 0)
		return -1;
	pDianxinCtrl->nSeq++;
	return hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_LOGIN, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf));
}

int hi_dianxin_login_res(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	char *p;
	int nResult = 0;
	mxml_node_t *pHead, *pTree, *pLogin, *pNode;
	HI_DIANXIN_ESTORE_CFG *pEStroeCfg = &pDianxinCtrl->stEstoreCfg;

	int nYear, nMonth, nDay, nHour, nMinute, nSecond;
	NVR_TIME_S stTime;
	nvr_logon_get_time_cfg(0, &stTime);
	
	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';
	pHead = mxmlNewXML("1.0");
	pTree = mxmlLoadString(pHead, p, MXML_NO_CALLBACK);
	pLogin = dianxin_mxml_node_get(pTree, "LOGIN_RES");

	pNode = dianxin_mxml_node_get(pLogin, "Result");
	
	if(mxmlGetText(pNode, 0) == NULL)
	{
		nResult = 255;
	}else
	{
		nResult = atoi(MXMLGETTEXT(pNode, 0));
	}
	if(nResult == 1)
	{
		__E("the device is not exist!\r\n");
		mxmlDelete(pHead);
		free(p);
		pDianxinCtrl->nLoginStatus = DIANXIN_LOGIN_ERR;
		return -1;
	}else if(nResult == 2)
	{
		__E("the key validation error!\r\n");
		mxmlDelete(pHead);
		free(p);
		pDianxinCtrl->nLoginStatus = DIANXIN_LOGIN_ERR;
		return -1;
	}else if(nResult == 3)
	{
		__E("the firmware version not support!\r\n");
		mxmlDelete(pHead);
		free(p);
		pDianxinCtrl->nLoginStatus = DIANXIN_LOGIN_ERR;
		return -1;
	}else if(nResult == 255)
	{
		__E("other error!\r\n");
		mxmlDelete(pHead);
		free(p);
		pDianxinCtrl->nLoginStatus = DIANXIN_LOGIN_ERR;
		return -1;
	}
	pDianxinCtrl->nLoginStatus = DIANXIN_FIRST_LOGIN;
	if((pNode = dianxin_mxml_node_get(pLogin, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pLogin, "EURL")) != NULL)
	{
		//strcpy(pEStroeCfg->szEURL, mxmlGetText(pNode, 0));
		dainxin_mxml_string_copy(pEStroeCfg->szEURL, pNode);
	}
	if((pNode = dianxin_mxml_node_get(pLogin, "EAccount")) != NULL)
	{
		//strcpy(pEStroeCfg->szEAccount, mxmlGetText(pNode, 0));
		dainxin_mxml_string_copy(pEStroeCfg->szEAccount, pNode);
	}
	if((pNode = dianxin_mxml_node_get(pLogin, "EPassword")) != NULL)
	{
		//strcpy(pEStroeCfg->szEPassword, mxmlGetText(pNode, 0));
		dainxin_mxml_string_copy(pEStroeCfg->szEPassword, pNode);
	}
	if((pNode = dianxin_mxml_node_get(pLogin, "EStorgeSpace")) != NULL)
	{
		if(mxmlGetText(pNode, 0) != NULL)
		pEStroeCfg->nEStorgeSpace = atoi(MXMLGETTEXT(pNode, 0));
	}
	if((pNode = dianxin_mxml_node_get(pLogin, "ERecordBucket")) != NULL)
	{
		//strcpy(pEStroeCfg->szERecordBucket, mxmlGetText(pNode, 0));
		dainxin_mxml_string_copy(pEStroeCfg->szERecordBucket, pNode);
	}
	if((pNode = dianxin_mxml_node_get(pLogin, "EPictureBucket")) != NULL)
	{
		//strcpy(pEStroeCfg->szEPictureBucket, mxmlGetText(pNode, 0));
		dainxin_mxml_string_copy(pEStroeCfg->szEPictureBucket, pNode);
	}

	if((pNode = dianxin_mxml_node_get(pLogin, "Time")) != NULL)
	{
		if(mxmlGetText(pNode, 0) != NULL)
		{
			sscanf(mxmlGetText(pNode, 0), "%4d%2d%2d%2d%2d%2d", &nYear, &nMonth, &nDay, &nHour, &nMinute, &nSecond);
			stTime.u8Year = nYear - 1900;
			stTime.u8Month = nMonth;
			stTime.u8Day = nDay;
			stTime.u8Hour = nHour;
			stTime.u8Minute = nMinute;
			stTime.u8Second = nSecond;
			nvr_logon_set_time_cfg(0, &stTime);
		}
	}
	
	mxmlDelete(pHead);
	free(p);
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////
int hi_dianxin_abilityup_body_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	char szStr[64];
	mxml_node_t *pHead, *pAbilityup, *pVideo, *pOnvif, *pNode;
	HI_DIANXIN_PLATRORM_CFG *pDianxinPlatform = &pDianxinCtrl->stPlatformCfg;

	NVR_DEV_HD_INFO_S stHdInfo;
	//nvr_logon_get_hd_cfg(0, &stHdInfo);

	NVR_DEV_INFO_S stDevCfg;
	nvr_logon_get_dev_cfg(0, &stDevCfg);

	NVR_DEV_USER_CFG_S stUser;
	nvr_logon_get_user_cfg(0, &stUser);

	pHead= mxmlNewXML("1.0");
	pAbilityup = mxmlNewElement(pHead, "ABILITYUP_REQ");
	pNode = mxmlNewElement(pAbilityup, "DevID");
	mxmlNewText(pNode, 0, pDianxinPlatform->szDevId);

	pNode = mxmlNewElement(pAbilityup, "ChannelNum");
	mxmlNewText(pNode, 0, "4");

	pNode = mxmlNewElement(pAbilityup, "PTZFlag");
	mxmlNewText(pNode, 0, "0");

	pNode = mxmlNewElement(pAbilityup, "EStoreFlag");
	mxmlNewText(pNode, 0, "0");

	pNode = mxmlNewElement(pAbilityup, "LocalStoreFlag");
	if(stHdInfo.u8DiskNum)
		mxmlNewText(pNode, 0, "1");
	else
		mxmlNewText(pNode, 0, "0");

	pNode = mxmlNewElement(pAbilityup, "MotionDetectionFlag");
	mxmlNewText(pNode, 0, "1");	

	pNode = mxmlNewElement(pAbilityup, "InfraredDetectionFlag");
	mxmlNewText(pNode, 0, "0");	

	pNode = mxmlNewElement(pAbilityup, "ManualAlarmFlag");
	mxmlNewText(pNode, 0, "0");	

	pNode = mxmlNewElement(pAbilityup, "DIDOAlarmFlag");
	mxmlNewText(pNode, 0, "1");	
	pNode = mxmlNewElement(pAbilityup, "DIDONum");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", 0);//stDevCfg.stStableInfo.u8DiNum);
	mxmlNewText(pNode, 0, szStr);	
	
	pVideo = mxmlNewElement(pAbilityup, "Video");
	pNode = mxmlNewElement(pVideo, "StreamType");
	mxmlNewText(pNode, 0, "1");
	pNode = mxmlNewElement(pVideo, "FrameSize");
	mxmlNewText(pNode, 0, "1");

	pVideo = mxmlNewElement(pAbilityup, "Video");
	pNode = mxmlNewElement(pVideo, "StreamType");
	mxmlNewText(pNode, 0, "2");
	pNode = mxmlNewElement(pVideo, "FrameSize");
	mxmlNewText(pNode, 0, "2");

	pVideo = mxmlNewElement(pAbilityup, "Video");
	pNode = mxmlNewElement(pVideo, "StreamType");
	mxmlNewText(pNode, 0, "2");
	pNode = mxmlNewElement(pVideo, "FrameSize");
	mxmlNewText(pNode, 0, "3");

	pOnvif = mxmlNewElement(pAbilityup, "Onvif");	//onvif·ĎːƏ¢
	pNode = mxmlNewElement(pOnvif, "Port");
	mxmlNewText(pNode, 0, "8000");
	pNode = mxmlNewElement(pOnvif, "Account");
	mxmlNewText(pNode, 0, stUser.szUsrName);
	pNode = mxmlNewElement(pOnvif, "Password");
	mxmlNewText(pNode, 0, stUser.szPsw);

	mxmlSaveString(pHead, pBodyBuf, nBodyLen, MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	return 0;
}


int hi_dianxin_abilityup_req(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	hi_dianxin_abilityup_body_req(pDianxinCtrl, szSendBuf, sizeof(szSendBuf));
	pDianxinCtrl->nSeq ++;
	return hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_ABILITYUP, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf));
}

static int hi_dianxin_abilityup_res(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	pDianxinCtrl->nLoginStatus = DIANXIN_LOGIN;
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int hi_dianxin_heart_beat_body_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	char szStr[64];
	mxml_node_t *pHead, *pHeartBeat, *pNode;
	HI_DIANXIN_PLATRORM_CFG *pDianxinPlatform = &pDianxinCtrl->stPlatformCfg;
	NVR_DEV_NET_CFG_S stNetCfg;
	NVR_UPNP_CFG_S stUpnpCfg;
	char szRelayIp[NVR_IP_ADDR_LEN] = {0};
	int nRelayPort = 0;
	
	if(hi_turn_get_relay_info(szRelayIp, sizeof(szRelayIp), &nRelayPort) < 0)
		printf("Get turn relay information failed.\n");
	else
	{
		strcpy(pDianxinCtrl->szRelayIp, szRelayIp);
		pDianxinCtrl->nRelayPort = nRelayPort;
	}

	nvr_logon_get_net_cfg(0, &stNetCfg);
	nvr_logon_get_upnp_cfg(0, &stUpnpCfg);
	
	pHead= mxmlNewXML("1.0");
	pHeartBeat = mxmlNewElement(pHead, "HEARTBEAT_REQ");
	pNode = mxmlNewElement(pHeartBeat, "DevID");
	mxmlNewText(pNode, 0, pDianxinPlatform->szDevId);

	pNode = mxmlNewElement(pHeartBeat, "CurVideoNum");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", rtsp_get_client_cnt());
	sprintf(szStr, "%d", 0);
	printf(">>>>>>>>>>>the CurVideoNum is %s\r\n", szStr);
	mxmlNewText(pNode, 0, szStr);

	pNode = mxmlNewElement(pHeartBeat, "UpnpIP");
	strcpy(szStr, stUpnpCfg.szExternalIp);
	mxmlNewText(pNode, 0, szStr);
	
	pNode = mxmlNewElement(pHeartBeat, "RtspUpnpPort");
	sprintf(szStr, "%d", stUpnpCfg.u16UpnpRtspPortS);
	mxmlNewText(pNode, 0, szStr);

	pNode = mxmlNewElement(pHeartBeat, "LocalIP");
	memset(szStr, 0, sizeof(szStr));
	if(stNetCfg.struPppoeCfg.u8PppoeOn && stNetCfg.struPppoeCfg.u8PppoeStatus == 2)
	{
		sprintf(szStr, "%d.%d.%d.%d", 
			(stNetCfg.struPppoeCfg.u32PppoeIp >> 24) & 0xff,
			(stNetCfg.struPppoeCfg.u32PppoeIp >> 16) & 0xff,
			(stNetCfg.struPppoeCfg.u32PppoeIp >> 8) & 0xff,
			(stNetCfg.struPppoeCfg.u32PppoeIp >> 0) & 0xff);
	}else
	{
		net_ip_n2a(stNetCfg.struEtherCfg[0].u32IpAddr, szStr, sizeof(szStr));
	}
	mxmlNewText(pNode, 0, szStr);

	pNode = mxmlNewElement(pHeartBeat, "RtspLocalPort");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", pDianxinPlatform->localRtspPort);
	mxmlNewText(pNode, 0, szStr);

	//pNode = mxmlNewElement(pHeartBeat, "TurnRelayIP");		//TurnServer·�elayµٖ·
	//pNode = mxmlNewElement(pHeartBeat, "RtspTurnRelayPort");
	pNode = mxmlNewElement(pHeartBeat, "TurnRelayIP");
	mxmlNewText(pNode, 0, szRelayIp);	
	pNode = mxmlNewElement(pHeartBeat, "RtspTurnRelayPort");
	sprintf(szStr, "%d", nRelayPort);
	mxmlNewText(pNode, 0, szStr);

	mxmlSaveString(pHead, pBodyBuf, nBodyLen, MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	return 0;
}

int hi_dianxin_heart_beat_req(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[512];
	hi_dianxin_heart_beat_body_req(pDianxinCtrl, szSendBuf, sizeof(szSendBuf));
	pDianxinCtrl->nSeq ++;
	return hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_HEARTBEAT, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf));
}

static int hi_dianxin_heart_beat_res(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	pDianxinCtrl->u32NoActiveTime = 0;
	char *p;
//	int nYear, nMonth, nDay, nHour, nMinute, nSecond;
	mxml_node_t *pHead, *pHeartBeat, *pDevId, *pTime;
//	HI_TIME_S stTime;

//	hi_platform_get_time_cfg(0, &stTime);
	
	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pHeartBeat = dianxin_mxml_node_get(pHead, "HEARTBEAT_RES");
	if((pDevId = dianxin_mxml_node_get(pHeartBeat, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pTime = dianxin_mxml_node_get(pHeartBeat, "Time")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
/*	sscanf(mxmlGetText(pTime, 0), "%4d%2d%2d%2d%2d%2d", &nYear, &nMonth, &nDay, &nHour, &nMinute, &nSecond);
	stTime.u8Year = nYear - 1900;
	stTime.u8Month = nMonth;
	stTime.u8Day = nDay;
	stTime.u8Hour = nHour;
	stTime.u8Minute = nMinute;
	stTime.u8Second = nSecond;*/
	mxmlDelete(pHead);
	free(p);
//	hi_platform_set_time_cfg(0, &stTime);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int hi_dianxin_exception_notify_body(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	char szStr[64];
	mxml_node_t *pHead, *pEceptionNotify, *pNode;
	HI_DIANXIN_PLATRORM_CFG *pDianxinPlatform = &pDianxinCtrl->stPlatformCfg;

	pHead= mxmlNewXML("1.0");
	pEceptionNotify = mxmlNewElement(pHead, "EXCEPTION_NOTIFY_REQ");
	pNode = mxmlNewElement(pEceptionNotify, "DevID");
	mxmlNewText(pNode, 0, pDianxinPlatform->szDevId);

	pNode = mxmlNewElement(pEceptionNotify, "ChannelNo");
	mxmlNewText(pNode, 0, "0");

	pNode = mxmlNewElement(pEceptionNotify, "ExceptionType");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", pDianxinCtrl->nExceptionType);
	mxmlNewText(pNode, 0, szStr);

	pNode = mxmlNewElement(pEceptionNotify, "ExceptionDesc");

	mxmlSaveString(pHead, pBodyBuf, nBodyLen, MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	return 0;
}

int hi_dianxin_exception_notify_proc(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[512];
	hi_dianxin_exception_notify_body(pDianxinCtrl, szSendBuf, sizeof(szSendBuf));
	pDianxinCtrl->nSeq ++;
	return hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_EXCEPTION_NOTIFY, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf));
}

static int hi_dianxin_exception_notify_res(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	pDianxinCtrl->nExceptionNeedSend = 0;
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int hi_dianxin_video_body_req(DIANXIN_VIDEO_PLAY *pVideoPlay, char *pBodyBuf, int nBodyLen)
{
	mxml_node_t *pHead, *pVidoPlay, *pNode;

	pHead= mxmlNewXML("1.0");
	pVidoPlay = mxmlNewElement(pHead, "VIDEO_PLAY_REQ");
	pNode = mxmlNewElement(pVidoPlay, "DevID");
	mxmlNewText(pNode, 0, pVideoPlay->szDevId);

	pNode = mxmlNewElement(pVidoPlay, "ChannelNo");
	mxmlNewText(pNode, 0, pVideoPlay->szChannelNo);

	pNode = mxmlNewElement(pVidoPlay, "StreamType");
	mxmlNewText(pNode, 0, pVideoPlay->szStreamType);

	pNode = mxmlNewElement(pVidoPlay, "HashToken");
	mxmlNewText(pNode, 0, pVideoPlay->szHashToken);

	mxmlSaveString(pHead, pBodyBuf, nBodyLen, MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	return 0;
}

int hi_dianxin_video_play_req(DIANXIN_VIDEO_PLAY *pVideoPlay)
{
	DIANXIN_CTRL *pDianxinCtrl = hi_dianxin_get_ctrl();
	char szSendBuf[512];
	pDianxinCtrl->nVideoRes = 0;
	hi_dianxin_video_body_req(pVideoPlay, szSendBuf, sizeof(szSendBuf));
	pDianxinCtrl->nSeq ++;
	return hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_VIDEO_PLAY, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf));
	
}

static int hi_dianxin_video_play_res(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	char *p;
	int nResult;
	mxml_node_t *pHead, *pVideo, *pResult;
	
	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pVideo = dianxin_mxml_node_get(pHead, "VIDEO_PLAY_RES");
	if((pResult = dianxin_mxml_node_get(pVideo, "Result")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if(mxmlGetText(pResult, 0) == NULL)
	{
		nResult = 255;
	}else
	{
		nResult = atoi(MXMLGETTEXT(pResult, 0));
	}
	if(nResult == 0)
	{
		pDianxinCtrl->nVideoRes = 1;
	}else 
	{
		pDianxinCtrl->nVideoRes = 2;
	}
	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_notify_picture_res(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
static const unsigned int dianxin_bit_rate_table[][2] = 
{
	{1, 96},
	{2, 128},
	{3, 196},
	{4, 256},
	{5, 384},

	{6, 512},
	{7, 768},
	{8, 1024},
	{9, 1536},
	{10, 2048},

	{11, 2560},
	{12, 3072},
	{13, 4096},
};



static int hi_dianxin_set_channel_proc(char *pBodyBuf, int nBodyLen)
{
	char *p;
	int i;
	int nFrameSize, nStreamType = 0, nBitRate;
	mxml_node_t *pHead, *pChannel, *pNode;

	NVR_DEV_VIDEO_CFG_S	stVideoCfg;
	NVR_DEV_AUDIO_CFG_S	stAudioCfg;
	NVR_DEV_VENC_CFG_S 	*pVencCfg;

	//nvr_logon_get_video_cfg(0, &stVideoCfg);
	//nvr_logon_get_audio_cfg(0, &stAudioCfg);
	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';
	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pChannel = dianxin_mxml_node_get(pHead, "SET_CHANNEL_REQ");
	if((pNode = dianxin_mxml_node_get(pChannel, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pChannel, "ChannelNo")) != NULL)
	{

	}
	if((pNode = dianxin_mxml_node_get(pChannel, "StreamType")) != NULL)
	{
		if(atoi(MXMLGETTEXT(pNode, 0)) == 1)
		{
			pVencCfg = &stVideoCfg.struMainVenc;
			nStreamType = 1;
		}else if(atoi(MXMLGETTEXT(pNode, 0)) == 2)
		{
			pVencCfg = &stVideoCfg.struSubVenc;
			nStreamType = 2;
		}else
		{
			mxmlDelete(pHead);
			free(p);
			return 0;
		}
	}else
	{
		mxmlDelete(pHead);
		free(p);
		return 0;
	}
	
	if((pNode = dianxin_mxml_node_get(pChannel, "FrameSize")) != NULL)
	{
		nFrameSize = atoi(MXMLGETTEXT(pNode, 0));
		if(nStreamType == 1 && nFrameSize == 1)
		{
			pVencCfg->u16Width = 1280;
			pVencCfg->u16Height = 720;
		}else if(nStreamType == 2 && nFrameSize == 2)
		{
			pVencCfg->u16Width = 704;
			pVencCfg->u16Height = 576;
		}else if(nStreamType == 2 && nFrameSize == 3)
		{
			pVencCfg->u16Width = 320;
			pVencCfg->u16Height = 192;
		}
	}

	if((pNode = dianxin_mxml_node_get(pChannel, "FrameRate")) != NULL)
	{
		pVencCfg->u8FrameRate = atoi(MXMLGETTEXT(pNode, 0));
	}

	if((pNode = dianxin_mxml_node_get(pChannel, "RateType")) != NULL)
	{
		pVencCfg->u8FlowType = atoi(MXMLGETTEXT(pNode, 0)) - 1;
	}

	if((pNode = dianxin_mxml_node_get(pChannel, "BitRate")) != NULL)
	{
		nBitRate = atoi(MXMLGETTEXT(pNode, 0));
		for(i = 0; i < sizeof(dianxin_bit_rate_table) / sizeof(dianxin_bit_rate_table[0]); i++)
		{
			if(nBitRate == dianxin_bit_rate_table[i][0])
			{
				pVencCfg->u32BitrateRate = dianxin_bit_rate_table[i][1];
				break;
			}
		}
	}

	if((pNode = dianxin_mxml_node_get(pChannel, "AudioInput")) != NULL)
	{
		stAudioCfg.u8Open = atoi(MXMLGETTEXT(pNode, 0));
	}

	(void)stAudioCfg;
	(void)stVideoCfg;
	//nvr_logon_set_video_cfg(0, &stVideoCfg);
	//nvr_logon_set_audio_cfg(0, &stAudioCfg);
	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_set_channel_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	mxml_node_t *pHead, *pChannel, *pNode;

	pHead = mxmlNewXML("1.0");
	pChannel = mxmlNewElement(pHead, "SET_CHANNEL_RES");

	pNode = mxmlNewElement(pChannel, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pChannel, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_SET_CHANNEL, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}

static int hi_dianxin_set_channel_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_set_channel_proc(pBodyBuf, nBodyLen);
	hi_dianxin_set_channel_res(pDianxinCtrl);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
static int hi_dianxin_get_channel_proc(char *pBodyBuf, int nBodyLen)
{
	char *p;
	int nStreamType = 0;
	mxml_node_t *pHead, *pChannel, *pNode;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pChannel = dianxin_mxml_node_get(pHead, "GET_CHANNEL_REQ");
	if((pNode = dianxin_mxml_node_get(pChannel, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pChannel, "ChannelNo")) != NULL)
	{

	}
	if((pNode = dianxin_mxml_node_get(pChannel, "StreamType")) != NULL)
	{
		nStreamType = atoi(MXMLGETTEXT(pNode, 0));
	}
	
	mxmlDelete(pHead);
	free(p);
	return nStreamType;
}

static int hi_dianxin_get_channel_res(DIANXIN_CTRL *pDianxinCtrl, int nStreamType)
{
	char szSendBuf[1024];
	char szStr[64];
	int  nBitRate;
	int  i;
	mxml_node_t *pHead, *pChannel, *pNode;

	NVR_DEV_VIDEO_CFG_S	stVideoCfg;
	NVR_DEV_AUDIO_CFG_S	stAudioCfg;
	NVR_DEV_VENC_CFG_S 	*pVencCfg;

	//nvr_logon_get_video_cfg(0, &stVideoCfg);
	//nvr_logon_get_audio_cfg(0, &stAudioCfg);

	pHead = mxmlNewXML("1.0");
	pChannel = mxmlNewElement(pHead, "GET_CHANNEL_RES");

	pNode = mxmlNewElement(pChannel, "Result");
	if(nStreamType == 1)
	{
		pVencCfg = &stVideoCfg.struMainVenc;
	}else if(nStreamType == 2)
	{
		pVencCfg = &stVideoCfg.struSubVenc;
	}else
	{
		mxmlNewText(pNode, 0, "1");
		goto end;
	}
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pChannel, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	pNode = mxmlNewElement(pChannel, "FrameSize");
	if(nStreamType == 1)
	{
		mxmlNewText(pNode, 0, "1");
	}else
	{
		if(pVencCfg->u16Width == 320 && pVencCfg->u16Height == 192)
		{
			mxmlNewText(pNode, 0, "3");
		}
		else if(pVencCfg->u16Width == 704 && pVencCfg->u16Height == 576)
		{
			mxmlNewText(pNode, 0, "2");
		}else
		{
			mxmlNewText(pNode, 0, "3");
		}
	}
	pNode = mxmlNewElement(pChannel, "FrameRate");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", pVencCfg->u8FrameRate);
	mxmlNewText(pNode, 0, szStr);

	pNode = mxmlNewElement(pChannel, "RateType");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", pVencCfg->u8FlowType + 1);
	mxmlNewText(pNode, 0, szStr);

	pNode = mxmlNewElement(pChannel, "BitRate");
	nBitRate = pVencCfg->u32BitrateRate;
	for(i = 0; i < sizeof(dianxin_bit_rate_table)/sizeof(dianxin_bit_rate_table[0]); i++)
	{
		if(dianxin_bit_rate_table[i][1] >= nBitRate)
		{
			memset(szStr, 0, sizeof(szStr));
			sprintf(szStr, "%d", dianxin_bit_rate_table[i][0]);
			mxmlNewText(pNode, 0, szStr);
			break;
		}
	}

	pNode = mxmlNewElement(pChannel, "AudioInput");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", stAudioCfg.u8Open);
	mxmlNewText(pNode, 0, szStr);
	
end:
	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_GET_CHANNEL, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}

static int hi_dianxin_get_channel_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	int nStreamType;
	nStreamType = hi_dianxin_get_channel_proc(pBodyBuf, nBodyLen);
	hi_dianxin_get_channel_res(pDianxinCtrl, nStreamType);
	return 0;
}

#if 0
///////////////////////////////////////////////////////////////////////////////////////////////////////////
static int hi_dianxin_get_wifi_res(DIANXIN_CTRL *pDianxinCtrl)
{	
	char szSendBuf[1024];
	char szStr[64];
	int  i;
	mxml_node_t *pHead, *pWifi, *pWifiSSID, *pNode;

	HI_WIFI_ESSID_CFG_S stWifiEssid = {0};
	hi_platform_wifi_scan_cfg(0, &stWifiEssid);

	pHead = mxmlNewXML("1.0");
	pWifi = mxmlNewElement(pHead, "GET_WIFI_RES");

	pNode = mxmlNewElement(pWifi, "Result");
	if(stWifiEssid.u8ScanResult == 0)
	{
		mxmlNewText(pNode, 0, "0");
	}else
	{
		mxmlNewText(pNode, 0, "1");
		goto end;
	}
	
	pNode = mxmlNewElement(pWifi, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	for(i = 0; i < stWifiEssid.u8Count; i++)
	{
		pWifiSSID = mxmlNewElement(pWifi, "WifiSSID");
		pNode = mxmlNewElement(pWifiSSID, "SSID");
		mxmlNewText(pNode, 0, stWifiEssid.Essid[i].szEssid);

		pNode = mxmlNewElement(pWifiSSID, "Signal");
		memset(szStr, 0, sizeof(szStr));
		sprintf(szStr, "%d", stWifiEssid.Essid[i].nSignal);
		mxmlNewText(pNode, 0, szStr);
		
		pNode = mxmlNewElement(pWifiSSID, "Security");
		if(strcmp(stWifiEssid.Essid[i].szAuthMode, "OPEN") == 0)
		{
			mxmlNewText(pNode, 0, "0");
		}else if(strcmp(stWifiEssid.Essid[i].szAuthMode, "WPAPSK") == 0)
		{
			mxmlNewText(pNode, 0, "2");
		}else if(strcmp(stWifiEssid.Essid[i].szAuthMode, "WPA2PSK") == 0)
		{
			mxmlNewText(pNode, 0, "4");
		}else if(strcmp(stWifiEssid.Essid[i].szAuthMode, "WEPAUTO") == 0)
		{
			mxmlNewText(pNode, 0, "1");
		}

		pNode = mxmlNewElement(pWifiSSID, "InUse");
		memset(szStr, 0, sizeof(szStr));
		sprintf(szStr, "%d", stWifiEssid.Essid[i].nStatus);
		mxmlNewText(pNode, 0, szStr);

	}

end:
	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_GET_WIFI, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}
	return 0;
}
#endif

static int hi_dianxin_get_wifi_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	//hi_dianxin_get_wifi_res(pDianxinCtrl);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
static int hi_dianxin_set_wifi_proc(char *pBodyBuf, int nBodyLen)
{
	char *p;
	int nSecurity;
	mxml_node_t *pHead, *pWifi, *pNode;

	HI_WIFI_STR_CFG_S stWifiCfg = {0};
	hi_platform_get_wifi_cfg(0, &stWifiCfg);

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pWifi = dianxin_mxml_node_get(pHead, "SET_WIFI_REQ");
	if((pNode = dianxin_mxml_node_get(pWifi, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pWifi, "SSID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	//strcpy(stWifiCfg.szEssid, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(stWifiCfg.szEssid, pNode);

	if((pNode = dianxin_mxml_node_get(pWifi, "Security")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	nSecurity = atoi(MXMLGETTEXT(pNode, 0));
	switch(nSecurity)
	{
		case 0:
			strcpy(stWifiCfg.szAuthmode, "OPEN");
			break;
		case 1:
			strcpy(stWifiCfg.szAuthmode, "WEPAUTO");
			break;
		case 2:
			strcpy(stWifiCfg.szAuthmode, "WPAPSK");
			break;
		default:
			strcpy(stWifiCfg.szAuthmode, "WPA2PSK");
			break;
	}
	
	if((pNode = dianxin_mxml_node_get(pWifi, "Password")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	//strcpy(stWifiCfg.szPsw, mxmlGetText(pNode, 0));
	dainxin_mxml_string_copy(stWifiCfg.szPsw, pNode);
	stWifiCfg.u8Connect = 1;
	
	hi_platform_set_wifi_cfg(0, &stWifiCfg);
	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_set_wifi_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	mxml_node_t *pHead, *pWifi, *pNode;

	pHead = mxmlNewXML("1.0");
	pWifi = mxmlNewElement(pHead, "SET_WIFI_RES");

	pNode = mxmlNewElement(pWifi, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pWifi, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_SET_WIFI, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}
	return 0;
}
#endif

static int hi_dianxin_set_wifi_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	//hi_dianxin_set_wifi_proc(pBodyBuf, nBodyLen);
	//hi_dianxin_set_wifi_res(pDianxinCtrl);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int hi_dianxin_video_ptz_proc(char *pBodyBuf, int nBodyLen)
{
	char *p;
	int nPtzDirection;
	mxml_node_t *pHead, *pPtz, *pNode;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pPtz = dianxin_mxml_node_get(pHead, "VIDEO_PTZ_REQ");
	if((pNode = dianxin_mxml_node_get(pPtz, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pPtz, "ChannelNo")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	
	if((pNode = dianxin_mxml_node_get(pPtz, "PtzDirection")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	nPtzDirection = atoi(MXMLGETTEXT(pNode, 0));
	switch(nPtzDirection)
	{
		case 0:
			//hi_platform_control_ptz(PTZ_CMD_STOP, 0, 0, 0);
			break;
		case 1:
			//hi_platform_control_ptz(PTZ_CMD_UP, 0, PTZSPEED, 0);
			break;
		case 2:
			//hi_platform_control_ptz(PTZ_CMD_DOWN, 0, PTZSPEED, 0);
			break;
		case 3:
			//hi_platform_control_ptz(PTZ_CMD_LEFT, 0, PTZSPEED, 0);
			break;
		case 4:
			//hi_platform_control_ptz(PTZ_CMD_RIGHT, 0, PTZSPEED, 0);
			break;
	}
	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_video_ptz_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	mxml_node_t *pHead, *pPtz, *pNode;

	pHead = mxmlNewXML("1.0");
	pPtz = mxmlNewElement(pHead, "VIDEO_PTZ_RES");

	pNode = mxmlNewElement(pPtz, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pPtz, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_VIDEO_PTZ, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0 )
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}

static int hi_dianxin_video_ptz_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_video_ptz_proc(pBodyBuf, nBodyLen);
	hi_dianxin_video_ptz_res(pDianxinCtrl);
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////

static int hi_dianxin_set_estore_proc(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	char *p;
	mxml_node_t *pHead, *pEstore, *pNode;
	HI_DIANXIN_ESTORE_CFG *pEstoreCfg = &pDianxinCtrl->stEstoreCfg;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pEstore = dianxin_mxml_node_get(pHead, "SET_ESTORE_REQ");
	if((pNode = dianxin_mxml_node_get(pEstore, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pEstore, "URL")) != NULL)
	{
		//strcpy(pEstoreCfg->szEURL, mxmlGetText(pNode, 0));
		dainxin_mxml_string_copy(pEstoreCfg->szEURL, pNode);
	}

	if((pNode = dianxin_mxml_node_get(pEstore, "Account")) != NULL)
	{
		//strcpy(pEstoreCfg->szEAccount , mxmlGetText(pNode, 0));
		dainxin_mxml_string_copy(pEstoreCfg->szEAccount, pNode);
	}

	if((pNode = dianxin_mxml_node_get(pEstore, "Password")) != NULL)
	{
		//strcpy(pEstoreCfg->szEPassword , mxmlGetText(pNode, 0));
		dainxin_mxml_string_copy(pEstoreCfg->szEPassword, pNode);
	}

	if((pNode = dianxin_mxml_node_get(pEstore, "EStorgeSpace")) != NULL)
	{
		pEstoreCfg->nEStorgeSpace = atoi(MXMLGETTEXT(pNode, 0));
	}
	if((pNode = dianxin_mxml_node_get(pEstore, "ERecordBucket")) != NULL)
	{
		//strcpy(pEstoreCfg->szERecordBucket, mxmlGetText(pNode, 0));
		dainxin_mxml_string_copy(pEstoreCfg->szERecordBucket, pNode);
	}
	if((pNode = dianxin_mxml_node_get(pEstore, "EPictureBucket")) != NULL)
	{
		//strcpy(pEstoreCfg->szEPictureBucket, mxmlGetText(pNode, 0));
		dainxin_mxml_string_copy(pEstoreCfg->szEPictureBucket, pNode);
	}

	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_set_estore_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	mxml_node_t *pHead, *pEStore, *pNode;

	pHead = mxmlNewXML("1.0");
	pEStore = mxmlNewElement(pHead, "SET_ESTORE_RES");

	pNode = mxmlNewElement(pEStore, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pEStore, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_SET_ESTORE, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}

static int hi_dianxin_set_estore_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_set_estore_proc(pDianxinCtrl, pBodyBuf, nBodyLen);
	hi_dianxin_set_estore_res(pDianxinCtrl);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

static int hi_dianxin_get_estore_proc(char *pBodyBuf, int nBodyLen)
{
	return 0;
}

static int hi_dianxin_get_estore_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	char szStr[32];
	mxml_node_t *pHead, *pEStore, *pNode;
	HI_DIANXIN_ESTORE_CFG *pEStoreCfg = &pDianxinCtrl->stEstoreCfg;

	pHead = mxmlNewXML("1.0");
	pEStore = mxmlNewElement(pHead, "GET_ESTORE_RES");

	pNode = mxmlNewElement(pEStore, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pEStore, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	pNode = mxmlNewElement(pEStore, "URL");
	mxmlNewText(pNode, 0, pEStoreCfg->szEURL);
	pNode = mxmlNewElement(pEStore, "Account");
	mxmlNewText(pNode, 0, pEStoreCfg->szEAccount);
	pNode = mxmlNewElement(pEStore, "Password");
	mxmlNewText(pNode, 0, pEStoreCfg->szEPassword);

	pNode = mxmlNewElement(pEStore, "StorgeSpace");
	sprintf(szStr, "%d", pEStoreCfg->nEStorgeSpace);
	mxmlNewText(pNode, 0, szStr);
	pNode = mxmlNewElement(pEStore, "ERecordBucket");
	mxmlNewText(pNode, 0, pEStoreCfg->szERecordBucket);
	pNode = mxmlNewElement(pEStore, "EPictureBucket");
	mxmlNewText(pNode, 0, pEStoreCfg->szEPictureBucket);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_GET_ESTORE, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}


static int hi_dianxin_get_estore_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_get_estore_proc(pBodyBuf, nBodyLen);
	hi_dianxin_get_estore_res(pDianxinCtrl);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hi_dianxin_add_record_policy_proc(char *pBodyBuf, int nBodyLen)
{
	char *p;
	int i;
	int nSegNum = 0, nCh = 0;
	int nRecStream = 0;
	int nStoreType, nWeek;
	int nBHour, nBMinute, nEHour, nEMinute;
	mxml_node_t *pHead, *pRecordPolicy, *pWeekPolicy, *pNode;
	NVR_DEV_REC_STRC_S stRecStac;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pRecordPolicy = dianxin_mxml_node_get(pHead, "ADD_RECORD_POLICY_REQ");
	if((pNode = dianxin_mxml_node_get(pRecordPolicy, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}

	if((pNode = dianxin_mxml_node_get(pRecordPolicy, "ChannelNo")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	
	nCh = atoi(MXMLGETTEXT(pNode, 0));
	if (nCh < 0 || nCh >= 4)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	
	nvr_logon_get_rec_cfg(nCh, &stRecStac);
	pWeekPolicy = pRecordPolicy;
	for(i = 0; i < 7; i++)
	{
		if((pWeekPolicy = mxmlFindElement(pWeekPolicy, pRecordPolicy, "WeekPolicy", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			break;
		}
		if((pNode = dianxin_mxml_node_get(pWeekPolicy, "StoreType")) != NULL)
		{
			nStoreType = atoi(MXMLGETTEXT(pNode, 0));
			(void)nStoreType;
		}
		
		if((pNode = dianxin_mxml_node_get(pWeekPolicy, "StreamType")) != NULL)
		{
			nRecStream = atoi(MXMLGETTEXT(pNode, 0)) - 1;
			(void)nRecStream;
		}
		
		if((pNode = dianxin_mxml_node_get(pWeekPolicy, "Week")) != NULL)
		{
			nWeek = atoi(MXMLGETTEXT(pNode, 0));
		}

		if((pNode = dianxin_mxml_node_get(pWeekPolicy, "StartTime")) != NULL)
		{
			sscanf(mxmlGetText(pNode, 0), "%d:%d", &nBHour, &nBMinute);
		}

		if((pNode = dianxin_mxml_node_get(pWeekPolicy, "EndTime")) != NULL)
		{
			sscanf(mxmlGetText(pNode, 0), "%d:%d", &nEHour, &nEMinute);
		}
	
		if(nWeek == 7)
		{
			for(nSegNum = 0; nSegNum < 4; nSegNum ++)
			{
				if(stRecStac.struTimerSeg[nSegNum][1].u8Open == 0)
				{
					break;
				}
			}
			if(nSegNum >= 4)
				nSegNum = 0;
			
			stRecStac.u8Enable = 1;
			stRecStac.struTimerSeg[nSegNum][1].u8Open = 1;
			stRecStac.struTimerSeg[nSegNum][1].beginSec = nBHour * 3600 + nBMinute * 60;
			stRecStac.struTimerSeg[nSegNum][1].endSec = nEHour * 3600 + nEMinute * 60;
			//stRecStac.recStream = nRecStream;
			
		}else
		{
			for(nSegNum = 0; nSegNum < 4; nSegNum ++)
			{
				if(stRecStac.struTimerSeg[nSegNum][nWeek + 1].u8Open == 0)
				{
					break;
				}
			}
			if(nSegNum >= 4)
				nSegNum = 0;
			
			stRecStac.u8Enable = 1;
			stRecStac.struTimerSeg[nSegNum][nWeek + 1].u8Open = 1;
			stRecStac.struTimerSeg[nSegNum][nWeek + 1].beginSec = nBHour * 3600 + nBMinute * 60;
			stRecStac.struTimerSeg[nSegNum][nWeek + 1].endSec = nEHour * 3600 + nEMinute * 60;
			//stRecStac.recStream = nRecStream;
		}
	}
	printf("nvr_logon_set_rec_cfg++++++++++++\r\n");
	nvr_logon_set_rec_cfg(nCh, &stRecStac);
	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_add_record_policy_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	mxml_node_t *pHead, *pRecordPolicy, *pNode;

	pHead = mxmlNewXML("1.0");
	pRecordPolicy = mxmlNewElement(pHead, "ADD_RECORD_POLICY_RES");

	pNode = mxmlNewElement(pRecordPolicy, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pRecordPolicy, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_ADD_RECORD_POLICY, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}

static int hi_dianxin_add_record_policy_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_add_record_policy_proc(pBodyBuf, nBodyLen);
	hi_dianxin_add_record_policy_res(pDianxinCtrl);
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////

static int hi_dianxin_query_record_policy_proc(char *pBodyBuf, int nBodyLen, int *channel)
{	
	char *p = NULL;
	const char *pDevID = NULL;
	int chn = 0; 
	mxml_node_t *pHead, *pRecordPolicy, *pNode;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pRecordPolicy = dianxin_mxml_node_get(pHead, "QUERY_RECORD_POLICY_REQ");
	if (pRecordPolicy == NULL)
	if((pNode = dianxin_mxml_node_get(pRecordPolicy, "DevID")) == NULL) goto err;
	pDevID = mxmlGetText(pNode, 0);
	
	if((pNode = dianxin_mxml_node_get(pRecordPolicy, "ChannelNo")) == NULL) goto err;
	chn = atoi(MXMLGETTEXT(pNode, 0));
	
	*channel = chn;

	(void)pDevID;
	
	mxmlDelete(pHead);
	free(p);
	return 0;

err:	
	mxmlDelete(pHead);
	free(p);
	return -1;
}

static int hi_dianxin_query_record_policy_res(DIANXIN_CTRL *pDianxinCtrl, int chn)
{
	char szSendBuf[4096];
	char szStr[64];
	int i, k;
	int nHour, nMinute;
	mxml_node_t *pHead, *pRecordPolicy, *pWeekPolicy, *pNode;

	NVR_DEV_REC_STRC_S stRecStac;
	nvr_logon_get_rec_cfg(chn, &stRecStac);

	pHead = mxmlNewXML("1.0");
	pRecordPolicy = mxmlNewElement(pHead, "QUERY_RECORD_POLICY_RES");

	pNode = mxmlNewElement(pRecordPolicy, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pRecordPolicy, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	for(i = 1; i < 8; i++)
	{
		for(k = 0; k < NVR_MAX_SEG_NUM; k++)
		{
			if(stRecStac.struTimerSeg[k][i].u8Open)
			{
				pWeekPolicy = mxmlNewElement(pRecordPolicy, "WeekPolicy");	
				pNode = mxmlNewElement(pWeekPolicy, "PolicyID");
				memset(szStr, 0, sizeof(szStr));
				sprintf(szStr, "%d", k*10 + i);
				mxmlNewText(pNode, 0, szStr);

				pNode = mxmlNewElement(pWeekPolicy, "StoreType");
				mxmlNewText(pNode, 0, "0");

				pNode = mxmlNewElement(pWeekPolicy, "StreamType");
				memset(szStr, 0, sizeof(szStr));
				sprintf(szStr, "%d", 0);//stRecStac.struTimerSeg[k][i].recStream + 1);
				mxmlNewText(pNode, 0, szStr);

				pNode = mxmlNewElement(pWeekPolicy, "Week");
				if(i == 1)
				{
					mxmlNewText(pNode, 0, "7");
				}else
				{
					memset(szStr, 0, sizeof(szStr));
					sprintf(szStr, "%d", i - 1);
					mxmlNewText(pNode, 0, szStr);
				}

				pNode = mxmlNewElement(pWeekPolicy, "StartTime");
				nHour = stRecStac.struTimerSeg[k][i].beginSec / 3600;
				nMinute = stRecStac.struTimerSeg[k][i].beginSec % 3600 / 60;
				sprintf(szStr, "%d:%d", nHour, nMinute);
				mxmlNewText(pNode, 0, szStr);

				pNode = mxmlNewElement(pWeekPolicy, "EndTime");
				nHour = stRecStac.struTimerSeg[k][i].endSec / 3600;
				nMinute = stRecStac.struTimerSeg[k][i].endSec % 3600 / 60;
				sprintf(szStr, "%d:%d", nHour, nMinute);
				mxmlNewText(pNode, 0, szStr);
			}
		}
	}

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_QUERY_RECORD_POLICY, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}

static int hi_dianxin_query_record_policy_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	int chn = 0;
	hi_dianxin_query_record_policy_proc(pBodyBuf, nBodyLen, &chn);
	hi_dianxin_query_record_policy_res(pDianxinCtrl, chn);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static int hi_dianxin_del_record_policy_proc(char *pBodyBuf, int nBodyLen)
{
	char *p;
	int nPolicyID = 0, nCh = 0;
	mxml_node_t *pHead, *pRecordPolicy, *pNode;

	NVR_DEV_REC_STRC_S stRecStac;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pRecordPolicy = dianxin_mxml_node_get(pHead, "DEL_RECORD_POLICY_REQ");
	if((pNode = dianxin_mxml_node_get(pRecordPolicy, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pRecordPolicy, "ChannelNo")) != NULL)
	{
		nCh = atoi(MXMLGETTEXT(pNode, 0));
	}
	if((pNode = dianxin_mxml_node_get(pRecordPolicy, "PolicyID")) != NULL)
	{
		nPolicyID = atoi(MXMLGETTEXT(pNode, 0));
	}
	
	if ((nCh >= 0) && (nCh < 4))
	{
		nvr_logon_get_rec_cfg(nCh, &stRecStac);
		stRecStac.struTimerSeg[nPolicyID/10][nPolicyID%10].u8Open = 0;
		nvr_logon_set_rec_cfg(nCh, &stRecStac);
	}
	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_del_record_policy_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	mxml_node_t *pHead, *pRecordPolicy, *pNode;

	pHead = mxmlNewXML("1.0");
	pRecordPolicy = mxmlNewElement(pHead, "DEL_RECORD_POLICY_RES");

	pNode = mxmlNewElement(pRecordPolicy, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pRecordPolicy, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_DEL_RECORD_POLICY, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}
static int hi_dianxin_del_record_policy_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_del_record_policy_proc(pBodyBuf, nBodyLen);
	hi_dianxin_del_record_policy_res(pDianxinCtrl);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hi_dianxin_reboot_proc(char *pBodyBuf, int nBodyLen)
{
	return 0;
}

static int hi_dianxin_reboot_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	mxml_node_t *pHead, *pReboot, *pNode;

	pHead = mxmlNewXML("1.0");
	pReboot = mxmlNewElement(pHead, "REBOOT_RES");

	pNode = mxmlNewElement(pReboot, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pReboot, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_REBOOT, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	sleep(1);
	nvr_sw_reset();
	//hi_platform_restart_dev();
	return 0;
}

static int hi_dianxin_reboot_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_reboot_proc(pBodyBuf, nBodyLen);
	hi_dianxin_reboot_res(pDianxinCtrl);
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
static int hi_dianxin_upgrade_proc(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	char *p;
	mxml_node_t *pHead, *pUpGrade, *pNode;
	char szFtpDistDir[128];

	NVR_FTP_CFG_S stFtpCfg;
	nvr_logon_get_ftp_cfg(0, &stFtpCfg);
	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pUpGrade = dianxin_mxml_node_get(pHead, "UPGRADE_REQ");
	if((pNode = dianxin_mxml_node_get(pUpGrade, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pUpGrade, "NewDevVer")) != NULL)
	{
		//strcpy(pDianxinCtrl->szNewDevVer, mxmlGetText(pNode, 0));
		dainxin_mxml_string_copy(pDianxinCtrl->szNewDevVer, pNode);
	}

	if((pNode = dianxin_mxml_node_get(pUpGrade, "FtpIP")) != NULL)
	{
		//strcpy(stFtpCfg.szFtpSvr, mxmlGetText(pNode, 0));
		dainxin_mxml_string_copy(stFtpCfg.szFtpSvr, pNode);
	}

	if((pNode = dianxin_mxml_node_get(pUpGrade, "FtpPort")) != NULL)
	{
		stFtpCfg.u16FtpPort = atoi(MXMLGETTEXT(pNode, 0));
	}

	if((pNode = dianxin_mxml_node_get(pUpGrade, "FtpAccount")) != NULL)
	{
		//strcpy(stFtpCfg.szFtpUsr, mxmlGetText(pNode, 0));
		dainxin_mxml_string_copy(stFtpCfg.szFtpUsr, pNode);
	}
	if((pNode = dianxin_mxml_node_get(pUpGrade, "FtpPassword")) != NULL)
	{
		//strcpy(stFtpCfg.szFtpPsw, mxmlGetText(pNode, 0));
		dainxin_mxml_string_copy(stFtpCfg.szFtpPsw, pNode);
	}
	if((pNode = dianxin_mxml_node_get(pUpGrade, "FtpFileName")) != NULL)
	{
		if(mxmlGetText(pNode, 0) != NULL)
		{
			sprintf(szFtpDistDir, "%s%s", stFtpCfg.szFtpPath, mxmlGetText(pNode, 0));
		}
	}
	nvr_logon_set_ftp_cfg(0, &stFtpCfg);
	hi_ftp_send(szFtpDistDir, NULL, 1);
	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_upgrade_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	mxml_node_t *pHead, *pUpGrade, *pNode;

	pHead = mxmlNewXML("1.0");
	pUpGrade = mxmlNewElement(pHead, "UPGRADE_RES");

	pNode = mxmlNewElement(pUpGrade, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pUpGrade, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_UPGRADE, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}

static int hi_dianxin_upgrade_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_upgrade_proc(pDianxinCtrl, pBodyBuf, nBodyLen);
	hi_dianxin_upgrade_res(pDianxinCtrl);
	return 0;
}

static int hi_nvr_remove_camera(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	int chn = 0;
	char szSendBuf[1024];
	NVR_CHANNEL_CFG_S  stChannelCfg;
	mxml_node_t *pHead, *pDelete, *pNode;
	NVR_DEV_DIANXIN_PLATFORM_CFG_S  stDianxinCfg;
	nvr_logon_get_dianxin_platform_cfg(0, &stDianxinCfg);

	///////////
	char *p;
	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pDelete = dianxin_mxml_node_get(pHead, "NVR_DEL_IPCAM_REQ");
	if((pNode = dianxin_mxml_node_get(pDelete, "DevID")) == NULL)
		goto err;
	if((pNode = dianxin_mxml_node_get(pDelete, "ChannelNo")) == NULL)
		goto err;
	chn = atoi(MXMLGETTEXT(pNode, 0));
	if (stDianxinCfg.link[chn].bUsed == 0)
		goto err;
	//stDianxinCfg.link[chn].bUsed = 0;
	memset(&stDianxinCfg.link[chn], 0, sizeof(stDianxinCfg.link[chn]));
	mxmlDelete(pHead);
//////////////

	pHead = mxmlNewXML("1.0");
	pDelete = mxmlNewElement(pHead, "NVR_DEL_IPCAM_RES");

	pNode = mxmlNewElement(pDelete, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pDelete, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	nvr_logon_get_channel_cfg(chn, &stChannelCfg);
	stChannelCfg.u8Enable = 0;	
	nvr_logon_set_channel_cfg(chn, &stChannelCfg);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_NVR_REMOVE_CAMERA, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}
	nvr_logon_set_dianxin_platform_cfg(0, &stDianxinCfg);

	return 0;
err:
	
	mxmlDelete(pHead);
	free(p);
	return -1;
}

static int dianxin_add_bind_ch(int chn, int camIP, char *IpcamDevID, char *User, char *Passwd)
{
	NVR_CHANNEL_CFG_S stChannelCfg;
	if (chn < 0 || chn > NVR_MAX_CHANNEL_NUM)
		return -1;

	if (IpcamDevID == NULL)
		return -1;
	
	memset(&stChannelCfg, 0, sizeof(NVR_CHANNEL_CFG_S));
	stChannelCfg.u32Size = sizeof(NVR_CHANNEL_CFG_INFO_S);
	strcpy(stChannelCfg.szFactoryName, "szjxj");
	strcpy(stChannelCfg.szProtocolName, "jxj_cu");
	strcpy(stChannelCfg.szDevName, IpcamDevID);
	if (User != NULL)
		strcpy(stChannelCfg.szUser, User);
	if (Passwd != NULL)
		strcpy(stChannelCfg.szPasswd, Passwd);
	stChannelCfg.u32IpAddr = camIP;
	stChannelCfg.u16DataPort = 3321;
	stChannelCfg.u16WebPort = 80;
	stChannelCfg.u16RtspPort = 7554;
	stChannelCfg.u8RemoteChn = 1;
	stChannelCfg.u8UseChn = 0;
	stChannelCfg.u8Enable = 1;
	stChannelCfg.u8SearchA = 0;//�?
	nvr_logon_set_channel_cfg(chn, &stChannelCfg);
	
	printf("[%s %d]modify success!!!!\r\n", __FILE__, __LINE__);
	
	return 0;
}

static int hi_nvr_bind_camera(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	int chn = 0;
	char szSendBuf[1024];
	const char *pTemp = NULL;
	int ret = 0;
	mxml_node_t *pHead, *pLink, *pNode;
	NVR_DEV_DIANXIN_PLATFORM_CFG_S  stDianxinCfg;
	nvr_logon_get_dianxin_platform_cfg(0, &stDianxinCfg);

	///////////
	char *p;
	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pLink = dianxin_mxml_node_get(pHead, "NVR_LINK_IPCAM_REQ");
	if((pNode = dianxin_mxml_node_get(pLink, "DevID")) == NULL)
		goto err;
	if((pNode = dianxin_mxml_node_get(pLink, "ChannelNo")) == NULL)
		goto err;
	chn = atoi(MXMLGETTEXT(pNode, 0));
	if (stDianxinCfg.link[chn].bUsed == 1)
		goto err;
	stDianxinCfg.link[chn].bUsed = 1;

	if((pNode = dianxin_mxml_node_get(pLink, "IpcamDevID")) == NULL)
		goto err;
	strcpy(stDianxinCfg.link[chn].szCamId, mxmlGetText(pNode, 0));
	
	if((pNode = dianxin_mxml_node_get(pLink, "IpcamLocalIP")) == NULL)
		goto err;
	
	stDianxinCfg.link[chn].nCamIP = net_ip_a2n(mxmlGetText(pNode, 0));

	if((pNode = dianxin_mxml_node_get(pLink, "OnvifPort")) == NULL)
		goto err;
	pTemp = mxmlGetText(pNode, 0);
	if (pTemp == NULL)
		stDianxinCfg.link[chn].onvifPort = 0;
	else
		stDianxinCfg.link[chn].onvifPort = atoi(pTemp);
		
	if((pNode = dianxin_mxml_node_get(pLink, "OnvifAccount")) == NULL)
		goto err;
	pTemp = mxmlGetText(pNode, 0);
	if (pTemp == NULL)	
		memset(stDianxinCfg.link[chn].szOnvifAccount, 0, sizeof(stDianxinCfg.link[chn].szOnvifAccount));
	else
		strcpy(stDianxinCfg.link[chn].szOnvifAccount, pTemp);
	
	if((pNode = dianxin_mxml_node_get(pLink, "OnvifPassword")) == NULL)
		goto err;
	pTemp = mxmlGetText(pNode, 0);
	if (pTemp == NULL)
		memset(stDianxinCfg.link[chn].szOnvifPassword, 0, sizeof(stDianxinCfg.link[chn].szOnvifPassword));
	else
		strcpy(stDianxinCfg.link[chn].szOnvifPassword, pTemp);
	mxmlDelete(pHead);
	//////////////
	//nvr_logon_set_channel_cfg(int port,NVR_CHANNEL_CFG_S * pCfg)

	pHead = mxmlNewXML("1.0");
	pLink = mxmlNewElement(pHead, "NVR_LINK_IPCAM_RES");

	pNode = mxmlNewElement(pLink, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pLink, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	ret = dianxin_add_bind_ch(chn, stDianxinCfg.link[chn].nCamIP, 
						stDianxinCfg.link[chn].szCamId, 
						stDianxinCfg.link[chn].szOnvifAccount, 
						stDianxinCfg.link[chn].szOnvifPassword);
	if (ret < 0) 
		return -1;
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_NVR_BIND_CAMERA, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}
	nvr_logon_set_dianxin_platform_cfg(0, &stDianxinCfg);

	return 0;
err:
	
	mxmlDelete(pHead);
	free(p);
	return -1;
}

static int hi_nvr_modify_camera(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	int chn = 0;
	char szSendBuf[1024];
	const char *pTemp = NULL;
	int ret = 0;
	mxml_node_t *pHead, *pLink, *pNode;
	NVR_DEV_DIANXIN_PLATFORM_CFG_S  stDianxinCfg;
	///////////
	char *p;
	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pLink = dianxin_mxml_node_get(pHead, "NVR_MODIFY_IPCAM_REQ");
	
	if((pNode = dianxin_mxml_node_get(pLink, "DevID")) == NULL)
		goto err;
	
	if((pNode = dianxin_mxml_node_get(pLink, "IpcamDevID")) == NULL)
		goto err;
	
	pTemp = mxmlGetText(pNode, 0);
	if (pTemp == NULL) 
		goto err;
	
	nvr_logon_get_dianxin_platform_cfg(0, &stDianxinCfg);
	for (chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++)
	{
		if (!stDianxinCfg.link[chn].bUsed) 
			continue;
		if (strcmp(stDianxinCfg.link[chn].szCamId, pTemp) == 0)
			break;
	}
	
	printf("[%s %d]find modify ch %d ###########\r\n", __FILE__, __LINE__, chn);
	//��?ipcamera
	if (chn == NVR_MAX_CHANNEL_NUM)
		goto err;
	
	if((pNode = dianxin_mxml_node_get(pLink, "IpcamLocalIP")) == NULL)
		goto err;
	
	pTemp = mxmlGetText(pNode, 0);
	if (pTemp == NULL)
		goto err;	
	printf("[%s %d]new ip %s ###########\r\n", __FILE__, __LINE__, pTemp);
	stDianxinCfg.link[chn].nCamIP = net_ip_a2n(pTemp);

	if((pNode = dianxin_mxml_node_get(pLink, "OnvifPort")) == NULL)
		goto err;
	pTemp = mxmlGetText(pNode, 0);
	if (pTemp == NULL)
		stDianxinCfg.link[chn].onvifPort = 0;
	else
		stDianxinCfg.link[chn].onvifPort = atoi(pTemp);
	
	if((pNode = dianxin_mxml_node_get(pLink, "OnvifAccount")) == NULL)
		goto err;
	
	pTemp = mxmlGetText(pNode, 0);
	if (pTemp == NULL)	
		memset(stDianxinCfg.link[chn].szOnvifAccount, 0, sizeof(stDianxinCfg.link[chn].szOnvifAccount));
	else
		strcpy(stDianxinCfg.link[chn].szOnvifAccount, pTemp);
	
	if((pNode = dianxin_mxml_node_get(pLink, "OnvifPassword")) == NULL)
		goto err;
	pTemp = mxmlGetText(pNode, 0);
	if (pTemp == NULL)
		memset(stDianxinCfg.link[chn].szOnvifPassword, 0, sizeof(stDianxinCfg.link[chn].szOnvifPassword));
	else
		strcpy(stDianxinCfg.link[chn].szOnvifPassword, pTemp);
	mxmlDelete(pHead);
	pHead = NULL;
	//////////////

	pHead = mxmlNewXML("1.0");
	pLink = mxmlNewElement(pHead, "NVR_MODIFY_IPCAM_RES");

	pNode = mxmlNewElement(pLink, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pLink, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	
	ret = dianxin_add_bind_ch(chn, stDianxinCfg.link[chn].nCamIP, 
						stDianxinCfg.link[chn].szCamId, 
						stDianxinCfg.link[chn].szOnvifAccount, 
						stDianxinCfg.link[chn].szOnvifPassword);
	if (ret < 0) 
		return -1;
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_NVR_MODIFY_CAMERA, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}
	
	printf("[%s %d]end ###########\r\n", __FILE__, __LINE__);
	nvr_logon_set_dianxin_platform_cfg(0, &stDianxinCfg);

	return 0;
	
err:	
	printf("[%s %d]modify failed!!\r\n", __FILE__, __LINE__);
	mxmlDelete(pHead);
	free(p);
	return -1;
}

static int hi_dianxin_query_camera_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	char *p;
	mxml_node_t *pHead, *pQuery, *pNode , *pChanel;
	
	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pQuery = dianxin_mxml_node_get(pHead, "NVR_QUERY_IPCAM_REQ");
	if((pNode = dianxin_mxml_node_get(pQuery, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	
	if ( strcmp(pDianxinCtrl->stPlatformCfg.szDevId, mxmlGetText(pNode, 0))!= 0 )
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}

	mxmlDelete(pHead);
	free(p);


	char szSendBuf[1024];

	pHead = mxmlNewXML("1.0");
	pQuery = mxmlNewElement(pHead, "NVR_QUERY_IPCAM_RES");

	pNode = mxmlNewElement(pQuery, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pQuery, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	int chn = 0;
	for (chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++)
	{
		char szChn[4];		
		NVR_DEV_DIANXIN_PLATFORM_CFG_S stDianxinCfg;
		nvr_logon_get_dianxin_platform_cfg(0, &stDianxinCfg);
		if (stDianxinCfg.link[chn].bUsed == 0) 
			continue;
		
		sprintf(szChn, "%d", chn);
		pChanel = mxmlNewElement(pQuery, "Channel");
		pNode = mxmlNewElement(pChanel, "ChannelNo");	
		mxmlNewText(pNode, 0, szChn);
		pNode = mxmlNewElement(pChanel, "IpcamDevID");	
		//112X1234582
		//if (chn == 1)
		//	mxmlNewText(pNode, 0, "112X1234582");
		//else
		mxmlNewText(pNode, 0, stDianxinCfg.link[chn].szCamId);
	}

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_NVR_QUERY_CAMERA, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
static int hi_dianxin_file_query_proc(char *pBodyBuf, int nBodyLen, DIANXIN_FILE_QUERY *pFileQuery)
{
	char *p;
	struct tm tmTime = {0};
	int nYear, nMonth, nDay, nHour, nMinute;
	mxml_node_t *pHead, *pRecordFile, *pNode;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pRecordFile = dianxin_mxml_node_get(pHead, "FILE_QUERY_REQ");
	if((pNode = dianxin_mxml_node_get(pRecordFile, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pRecordFile, "ChannelNo")) != NULL)
	{
		pFileQuery->chn = atoi(MXMLGETTEXT(pNode, 0));
	}
	if((pNode = dianxin_mxml_node_get(pRecordFile, "StartTime")) != NULL)
	{
		if(mxmlGetText(pNode, 0) != NULL)
		{
			sscanf(mxmlGetText(pNode, 0), "%4d%2d%2d%2d%2d", &nYear, &nMonth, &nDay, &nHour, &nMinute);
			tmTime.tm_year = nYear - 1900;
			tmTime.tm_mon = (nMonth - 1) % 12;
			tmTime.tm_mday= nDay % 32;
			tmTime.tm_hour = nHour % 24; 		
			tmTime.tm_min  = nMinute % 60;		
			pFileQuery->nStartTime = mktime(&tmTime);
		}
	}
	if((pNode = dianxin_mxml_node_get(pRecordFile, "EndTime")) != NULL)
	{
		if(mxmlGetText(pNode, 0) != NULL)
		{
			sscanf(mxmlGetText(pNode, 0), "%4d%2d%2d%2d%2d", &nYear, &nMonth, &nDay, &nHour, &nMinute);
			//printf("@@@@@@@@@@@@@@@end time   ==== >  %d-%d-%d %d:%d\n", 
			//		nYear, nMonth, nDay, nHour, nMinute);
			tmTime.tm_year = nYear - 1900;
			tmTime.tm_mon = (nMonth - 1) % 12;
			tmTime.tm_mday= nDay % 32;
			tmTime.tm_hour = nHour % 24; 		
			tmTime.tm_min  = nMinute % 60;		
			pFileQuery->nEndTime = mktime(&tmTime);
		}
	}

	if((pNode = dianxin_mxml_node_get(pRecordFile, "PageNo")) != NULL)
	{
		pFileQuery->PageNo = atoi(MXMLGETTEXT(pNode, 0));
	}

	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_file_query_res(DIANXIN_CTRL *pDianxinCtrl, DIANXIN_FILE_QUERY *pFileQuery)
{
	char szSendBuf[4096];
	char szStr[64];
	char szTime[64];
	int i, ret, nFragementNum;
	int shmid;
	int nStartTime, nEndTime, nChn;
	RecordQueryChnInfo *chn_info;
	RecordQueryInfo *query_info;
	struct tm stTimeInfo;
	mxml_node_t *pHead, *pRecordFile, *pFile, *pNode;

	int nBCount = (pFileQuery->PageNo - 1) * 10;
	int nECount = (pFileQuery->PageNo - 1) * 10 + 9;

	pHead = mxmlNewXML("1.0");
	pRecordFile = mxmlNewElement(pHead, "FILE_QUERY_RES");

	pNode = mxmlNewElement(pRecordFile, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pRecordFile, "DevID");	
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);	

	nChn = pFileQuery->chn;
	nStartTime = pFileQuery->nStartTime;
	nEndTime = pFileQuery->nEndTime;
	ret = storage_lib_record_query(1<<nChn,
							 	nStartTime, nEndTime, RECORD_ALL_TYPE,
							 	&shmid, &chn_info, &query_info);
	if(ret < 0)
	{
		printf("Failed to query record\n");
		return -1;
	}
	if(ret == 0)
	{	
		storage_lib_record_query_free(shmid, chn_info, query_info);	
		return 0;
	}
	nFragementNum = ret;

	for (i = nFragementNum - 1; i >= 0; i--)
	{
		if(nFragementNum - 1 - i >= nBCount && nFragementNum - 1 - i <= nECount)
		{
			pFile = mxmlNewElement(pRecordFile, "File");
			
			pNode = mxmlNewElement(pFile, "FileName");
			nStartTime = query_info[i].begin_time;
			nEndTime = query_info[i].end_time;
			localtime_r((time_t *)&nStartTime, &stTimeInfo);
			memset(szTime, 0, sizeof(szTime));
			strftime(szTime, sizeof(szTime), "%Y%m%d%H%M%S.mp4", &stTimeInfo);
			memset(szStr, 0, sizeof(szStr));
			sprintf(szStr, "%s_%d_%s", pDianxinCtrl->stPlatformCfg.szDevId, pFileQuery->chn, szTime);
			mxmlNewText(pNode, 0, szStr);	

			pNode = mxmlNewElement(pFile, "FileSize");
			memset(szStr, 0, sizeof(szStr));
			sprintf(szStr, "%d", query_info[i].fragment_size);
			mxmlNewText(pNode, 0, szStr);	

			pNode = mxmlNewElement(pFile, "StartTime");
			memset(szTime, 0, sizeof(szTime));
			localtime_r((time_t *)&nStartTime, &stTimeInfo);
			strftime(szTime, sizeof(szTime), "%Y%m%d%H%M%S", &stTimeInfo);
			mxmlNewText(pNode, 0, szTime);	

			pNode = mxmlNewElement(pFile, "EndTime");
			memset(szTime, 0, sizeof(szTime));
			localtime_r((time_t *)&nEndTime, &stTimeInfo);
			strftime(szTime, sizeof(szTime), "%Y%m%d%H%M%S", &stTimeInfo);
			mxmlNewText(pNode, 0, szTime);	 		
		}
	}
	
	storage_lib_record_query_free(shmid, chn_info, query_info);	
	
	pNode = mxmlNewElement(pRecordFile, "TotalRecordNum");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", nFragementNum);
	mxmlNewText(pNode, 0, szStr);	
	
	
	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_FILE_QUERY, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}
	return 0;
}

static int hi_dianxin_file_query_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	DIANXIN_FILE_QUERY stFileQuery = {0};
	hi_dianxin_file_query_proc(pBodyBuf, nBodyLen, &stFileQuery);
	hi_dianxin_file_query_res(pDianxinCtrl, &stFileQuery);
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
int hi_dianxin_record_play_body_req(DIANXIN_VOD_PLAY *pVodPlay, char *pBodyBuf, int nBodyLen)
{
	mxml_node_t *pHead, *pVidoPlay, *pNode;

	pHead= mxmlNewXML("1.0");
	pVidoPlay = mxmlNewElement(pHead, "RECORD_PLAY_REQ");
	pNode = mxmlNewElement(pVidoPlay, "DevID");
	mxmlNewText(pNode, 0, pVodPlay->szDevId);

	pNode = mxmlNewElement(pVidoPlay, "ChannelNo");
	mxmlNewText(pNode, 0, pVodPlay->szChannelNo);

	pNode = mxmlNewElement(pVidoPlay, "FileName");
	mxmlNewText(pNode, 0, pVodPlay->szFileName);

	pNode = mxmlNewElement(pVidoPlay, "HashToken");
	mxmlNewText(pNode, 0, pVodPlay->szHashToken);

	mxmlSaveString(pHead, pBodyBuf, nBodyLen, MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	return 0;
}

int hi_dianxin_record_play_req(DIANXIN_VOD_PLAY *pVodPlay)
{
	DIANXIN_CTRL *pDianxinCtrl = hi_dianxin_get_ctrl();
	char szSendBuf[512];
	pDianxinCtrl->nVodRes = 0;
	hi_dianxin_record_play_body_req(pVodPlay, szSendBuf, sizeof(szSendBuf));
	pDianxinCtrl->nSeq ++;
	return hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_RECORD_PLAY, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf));
	
}

static int hi_dianxin_record_play_res(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	char *p;
	int nResult;
	mxml_node_t *pHead, *pVod, *pResult;
	
	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';
	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pVod = dianxin_mxml_node_get(pHead, "RECORD_PLAY_RES");
	if((pResult = dianxin_mxml_node_get(pVod, "Result")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if(mxmlGetText(pResult, 0) != NULL)
	nResult = atoi(MXMLGETTEXT(pResult, 0));
	else
		nResult = 255;
	if(nResult == 0)
	{
		pDianxinCtrl->nVodRes = 1;
	}else 
	{
		pDianxinCtrl->nVodRes = 2;
	}
	mxmlDelete(pHead);
	free(p);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
#if 0
static int hi_dianxin_add_picture_policy_proc(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	char *p;
	int i;
	int nShootId, nStoreType, nInterval;
	int nBHour, nBMinute, nEHour, nEMinute;
	mxml_node_t *pHead, *pPicturePolicy, *pDayPolicy, *pNode;
	HI_DIANXIN_SHOOT_FTP_S stDianxinShootCfg;
	hi_platform_get_dianxin_shoot_cfg(0, &stDianxinShootCfg);
	
	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';
	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pPicturePolicy = dianxin_mxml_node_get(pHead, "ADD_PICTURE_POLICY_REQ");
	if((pNode = dianxin_mxml_node_get(pPicturePolicy, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}

	if((pNode = dianxin_mxml_node_get(pPicturePolicy, "ChannelNo")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}

	if((pNode = dianxin_mxml_node_get(pPicturePolicy, "StoreType")) != NULL)
	{
		nStoreType = atoi(MXMLGETTEXT(pNode, 0));
	}
	//printf(">>>>>>>>>>>the nStoreType is %d\r\n", nStoreType);
	if(nStoreType == 0)
	{
		pDayPolicy = pPicturePolicy;
		for(i = 0; i < 3; i++)
		{
			if((pDayPolicy = mxmlFindElement(pDayPolicy, pPicturePolicy, "DayPolicy", NULL, NULL, MXML_DESCEND)) == NULL)
			{
				break;
			}
	//		printf(">>>>>>>>>>>the i is %d\r\n", i);
			if((pNode = dianxin_mxml_node_get(pDayPolicy, "Interval")) != NULL)
			{
				nInterval = atoi(MXMLGETTEXT(pNode, 0));
			}

		if((pNode = dianxin_mxml_node_get(pDayPolicy, "StartTime")) != NULL)
		{
			if(mxmlGetText(pNode, 0) != NULL)
				sscanf(mxmlGetText(pNode, 0), "%d:%d", &nBHour, &nBMinute);
		}

		if((pNode = dianxin_mxml_node_get(pDayPolicy, "EndTime")) != NULL)
		{
				if(mxmlGetText(pNode, 0) != NULL)
				sscanf(mxmlGetText(pNode, 0), "%d:%d", &nEHour, &nEMinute);
			}
			for(nShootId = 0; nShootId < 3; nShootId ++)
			{
				if(stDianxinShootCfg.struSeg[nShootId].nOpen == 0)
				{
					break;
				}
			}
			if(nShootId >= 3)
				nShootId = 0;
			//printf(">>>>>>>>>>>the nShootId is %d\r\n", nShootId);
			stDianxinShootCfg.struSeg[nShootId].nOpen = 1;
			stDianxinShootCfg.struSeg[nShootId].nHour = nBHour;
			stDianxinShootCfg.struSeg[nShootId].nMinute = nBMinute;
		}
	}

	hi_platform_set_dianxin_shoot_cfg(0, &stDianxinShootCfg);
	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_add_picture_policy_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	mxml_node_t *pHead, *pPicturePolicy, *pNode;

	pHead = mxmlNewXML("1.0");
	pPicturePolicy = mxmlNewElement(pHead, "ADD_PICTURE_POLICY_RES");

	pNode = mxmlNewElement(pPicturePolicy, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pPicturePolicy, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_ADD_PICTURE_POLICY, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) <0 )
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}
#endif

static int hi_dianxin_add_picture_policy_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	//hi_dianxin_add_picture_policy_proc(pDianxinCtrl, pBodyBuf, nBodyLen);
	//hi_dianxin_add_picture_policy_res(pDianxinCtrl);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////
static int hi_dianxin_query_picture_policy_proc(char *pBodyBuf, int nBodyLen, int *pStoreType)
{
	char *p;
	mxml_node_t *pHead, *pPicturePolicy, *pNode;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pPicturePolicy = dianxin_mxml_node_get(pHead, "QUERY_PICTURE_POLICY_REQ");
	if((pNode = dianxin_mxml_node_get(pPicturePolicy, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pPicturePolicy, "ChannelNo")) != NULL)
	{

	}
	if((pNode = dianxin_mxml_node_get(pPicturePolicy, "StoreType")) != NULL)
	{
		*pStoreType = atoi(MXMLGETTEXT(pNode, 0));
	}
	
	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_query_picture_policy_res(DIANXIN_CTRL *pDianxinCtrl, int nStoreType)
{	
#if 0		
	char szSendBuf[1024];
	char szStr[64];
	int i;
	int nHour, nMinute;
	mxml_node_t *pHead, *pPicturePolicy, *pDayPolicy, *pNode;

	HI_DIANXIN_SHOOT_FTP_S stDianxinShootCfg;
	hi_platform_get_dianxin_shoot_cfg(0, &stDianxinShootCfg);

	pHead = mxmlNewXML("1.0");
	pPicturePolicy = mxmlNewElement(pHead, "QUERY_PICTURE_POLICY_RES");

	if(nStoreType == 1)
	{
		pNode = mxmlNewElement(pPicturePolicy, "Result");
		mxmlNewText(pNode, 0, "255");
		goto end;
	}else
	{
		pNode = mxmlNewElement(pPicturePolicy, "Result");
		mxmlNewText(pNode, 0, "0");
	}
	pNode = mxmlNewElement(pPicturePolicy, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	for(i = 0; i < 3; i++)
	{
		//printf(">>>>>>>>>>>the i is %d\r\n", i);
		if(stDianxinShootCfg.struSeg[i].nOpen)
		{
			pDayPolicy = mxmlNewElement(pPicturePolicy, "DayPolicy");	
			pNode = mxmlNewElement(pDayPolicy, "PolicyID");
			memset(szStr, 0, sizeof(szStr));
			sprintf(szStr, "%d", i);
			mxmlNewText(pNode, 0, szStr);

			pNode = mxmlNewElement(pDayPolicy, "Interval");
			mxmlNewText(pNode, 0, "0");

			pNode = mxmlNewElement(pDayPolicy, "StartTime");
			nHour = stDianxinShootCfg.struSeg[i].nHour;
			nMinute = stDianxinShootCfg.struSeg[i].nMinute;
			sprintf(szStr, "%d:%d", nHour, nMinute);
			mxmlNewText(pNode, 0, szStr);

			pNode = mxmlNewElement(pDayPolicy, "EndTime");
			nHour = stDianxinShootCfg.struSeg[i].nHour;
			nMinute = stDianxinShootCfg.struSeg[i].nMinute;
			sprintf(szStr, "%d:%d", nHour, nMinute);
			mxmlNewText(pNode, 0, szStr);
		}
	}

end:
	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_QUERY_PICTURE_POLICY, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}
#endif
	return 0;
}


static int hi_dianxin_query_picture_policy_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	int nStoreType = 0;
	hi_dianxin_query_picture_policy_proc(pBodyBuf, nBodyLen, &nStoreType);
	hi_dianxin_query_picture_policy_res(pDianxinCtrl, nStoreType);
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////
#if 0
static int hi_dianxin_del_picture_policy_proc(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	char *p;
	int nPolicyID;
	mxml_node_t *pHead, *pPicturePolicy, *pNode;

	HI_DIANXIN_SHOOT_FTP_S stDianxinShootCfg;
	hi_platform_get_dianxin_shoot_cfg(0, &stDianxinShootCfg);

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pPicturePolicy = dianxin_mxml_node_get(pHead, "DEL_PICTURE_POLICY_REQ");
	if((pNode = dianxin_mxml_node_get(pPicturePolicy, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pPicturePolicy, "ChannelNo")) != NULL)
	{

	}
	if((pNode = dianxin_mxml_node_get(pPicturePolicy, "PolicyID")) != NULL)
	{
		nPolicyID = atoi(MXMLGETTEXT(pNode, 0));
	}
	stDianxinShootCfg.struSeg[nPolicyID].nOpen = 0;

	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_del_picture_policy_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	mxml_node_t *pHead, *pPicturePolicy, *pNode;

	pHead = mxmlNewXML("1.0");
	pPicturePolicy = mxmlNewElement(pHead, "DEL_PICTURE_POLICY_RES");

	pNode = mxmlNewElement(pPicturePolicy, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pPicturePolicy, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_DEL_PICTURE_POLICY, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}
#endif

static int hi_dianxin_del_picture_policy_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	//hi_dianxin_del_picture_policy_proc(pDianxinCtrl, pBodyBuf, nBodyLen);
	//hi_dianxin_del_picture_policy_res(pDianxinCtrl);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
static int hi_dianxin_set_record_full_proc(char *pBodyBuf, int nBodyLen)
{
	char *p;
	int nRecordFull, chn = 0;
	mxml_node_t *pHead, *pRecordFull, *pNode;

	NVR_DEV_REC_STRC_S stRecStrc;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pRecordFull = dianxin_mxml_node_get(pHead, "SET_RECORD_FULL_REQ");
	if((pNode = dianxin_mxml_node_get(pRecordFull, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pRecordFull, "ChannelNo")) != NULL)
	{
		chn = atoi(MXMLGETTEXT(pNode, 0));
		if (chn <0 || chn >= 4)
		{
			mxmlDelete(pHead);
			free(p);
			return -1;
		}
	}
	
	nvr_logon_get_rec_cfg(chn, &stRecStrc);
	if((pNode = dianxin_mxml_node_get(pRecordFull, "RecordFull")) != NULL)
	{
		nRecordFull = atoi(MXMLGETTEXT(pNode, 0));
		if(nRecordFull == 1)
		{
			//stRecStrc.[[0][0].u8Open = 1;
			//stRecStrc.struTimerSeg[0][0].beginSec = 0;
			//stRecStrc.struTimerSeg[0][0].endSec = 24 * 3600 - 1;
			stRecStrc.u8Enable = 1;
			stRecStrc.u8RecType = 0;
		}
		else
		{
			stRecStrc.u8RecType = 1;
			//stRecStrc.struTimerSeg[0][0].u8Open = 0;
		}
	}
	nvr_logon_set_rec_cfg(chn, &stRecStrc);

	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_set_record_full_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	mxml_node_t *pHead, *pRecordFull, *pNode;

	pHead = mxmlNewXML("1.0");
	pRecordFull = mxmlNewElement(pHead, "SET_RECORD_FULL_RES");

	pNode = mxmlNewElement(pRecordFull, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pRecordFull, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_SET_RECORD_FULL, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}

static int hi_dianxin_set_record_full_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_set_record_full_proc(pBodyBuf, nBodyLen);
	hi_dianxin_set_record_full_res(pDianxinCtrl);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
static int hi_dianxin_get_record_full_proc(char *pBodyBuf, int nBodyLen, int *channel)
{
	char *p = NULL;
	const char *pDevID = NULL;
	int chn = 0, storeType = 0; 
	mxml_node_t *pHead, *pPicturePolicy, *pNode;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pPicturePolicy = dianxin_mxml_node_get(pHead, "GET_RECORD_FULL_REQ");
	if((pNode = dianxin_mxml_node_get(pPicturePolicy, "DevID")) == NULL) goto err;
	pDevID = mxmlGetText(pNode, 0);
	(void)pDevID;
	if((pNode = dianxin_mxml_node_get(pPicturePolicy, "ChannelNo")) == NULL) goto err;
	chn = atoi(MXMLGETTEXT(pNode, 0));

	if((pNode = dianxin_mxml_node_get(pPicturePolicy, "StorgeFlag")) == NULL) goto err;
	storeType = atoi(MXMLGETTEXT(pNode, 0));
	(void)storeType;
	*channel = chn;
	
	mxmlDelete(pHead);
	free(p);
	return 0;

err:	
	mxmlDelete(pHead);
	free(p);
	return -1;
}

static int hi_dianxin_get_record_full_res(DIANXIN_CTRL *pDianxinCtrl, int chn)
{
	char szSendBuf[1024];
	char szStr[64];
	int bFull = 0;
	
	mxml_node_t *pHead, *pRecordFull, *pNode;

	NVR_DEV_REC_STRC_S stRecStrc;
	nvr_logon_get_rec_cfg(chn, &stRecStrc);

	pHead = mxmlNewXML("1.0");
	pRecordFull = mxmlNewElement(pHead, "GET_RECORD_FULL_RES");

	pNode = mxmlNewElement(pRecordFull, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pRecordFull, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);
	
	pNode = mxmlNewElement(pRecordFull, "RecordFull");
	memset(szStr, 0, sizeof(szStr));
	
	if(stRecStrc.u8Enable&&stRecStrc.u8RecType == 0)
		bFull = 1;
	sprintf(szStr, "%d", bFull);
	mxmlNewText(pNode, 0, szStr);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_GET_RECORD_FULL, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}

static int hi_dianxin_get_record_full_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	int chn = 0;
	hi_dianxin_get_record_full_proc(pBodyBuf, nBodyLen, &chn);
	hi_dianxin_get_record_full_res(pDianxinCtrl, chn);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hi_dianxin_set_record_loop_proc(char *pBodyBuf, int nBodyLen)
{
	char *p;
	int i = 0;
	int nRecordLoop = 0;
	mxml_node_t *pHead, *pRecordLoop, *pNode;
	
	NVR_DEV_REC_STRC_S stRecStrc;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pRecordLoop = dianxin_mxml_node_get(pHead, "SET_RECORD_LOOP_REQ");
	if((pNode = dianxin_mxml_node_get(pRecordLoop, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pRecordLoop, "RecordLoop")) != NULL)
	{
		nRecordLoop = atoi(MXMLGETTEXT(pNode, 0));
		for (i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
		{
			nvr_logon_get_rec_cfg(i, &stRecStrc);			
			stRecStrc.u8OverWrite = !!nRecordLoop;
			nvr_logon_set_rec_cfg(i, &stRecStrc);
		}
	}

	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_set_record_loop_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	mxml_node_t *pHead, *pRecordLoop, *pNode;

	pHead = mxmlNewXML("1.0");
	pRecordLoop = mxmlNewElement(pHead, "SET_RECORD_LOOP_RES");

	pNode = mxmlNewElement(pRecordLoop, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pRecordLoop, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_SET_RECORD_LOOP, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}

static int hi_dianxin_set_record_loop_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_set_record_loop_proc(pBodyBuf, nBodyLen);
	hi_dianxin_set_record_loop_res(pDianxinCtrl);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hi_dianxin_get_record_loop_proc(char *pBodyBuf, int nBodyLen)
{
	return 0;
}

static int hi_dianxin_get_record_loop_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	char szStr[64];
	mxml_node_t *pHead, *pRecordLoop, *pNode;

	NVR_DEV_REC_STRC_S stRecStrc;
	nvr_logon_get_rec_cfg(0, &stRecStrc);

	pHead = mxmlNewXML("1.0");
	pRecordLoop = mxmlNewElement(pHead, "GET_RECORD_LOOP_RES");

	pNode = mxmlNewElement(pRecordLoop, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pRecordLoop, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);
	
	pNode = mxmlNewElement(pRecordLoop, "RecordLoop");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", stRecStrc.u8OverWrite);
	mxmlNewText(pNode, 0, szStr);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_GET_RECORD_LOOP, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}

static int hi_dianxin_get_record_loop_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_get_record_loop_proc(pBodyBuf, nBodyLen);
	hi_dianxin_get_record_loop_res(pDianxinCtrl);
	return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hi_dianxin_set_alarm_proc(char *pBodyBuf, int nBodyLen)
{
	char *p;
	int nAlarmType = -1, nAlarmFlag = 0;
	int nDiNum = 0;
	mxml_node_t *pHead, *pAlarm, *pNode;
	NVR_DEV_MD_CFG_S stMdCfg;
	NVR_DEV_DI_CFG_S stDiCfg;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';
	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pAlarm = dianxin_mxml_node_get(pHead, "SET_ALARM_REQ");
	if((pNode = dianxin_mxml_node_get(pAlarm, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pAlarm, "ChannelNo")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pAlarm, "AlarmType")) != NULL)
	{
		nAlarmType = atoi(MXMLGETTEXT(pNode, 0));
	}

	if((pNode = dianxin_mxml_node_get(pAlarm, "AlarmFlag")) != NULL)
	{
		nAlarmFlag = atoi(MXMLGETTEXT(pNode, 0));
	}
	if((pNode = dianxin_mxml_node_get(pAlarm, "DIDONo")) != NULL)
	{
		nDiNum = atoi(MXMLGETTEXT(pNode, 0));
		(void)nDiNum;
	}
	nvr_logon_get_di_cfg(0, &stDiCfg);
	//hi_platform_get_vmd_cfg(0, &stMdCfg);
	//hi_platform_get_di_cfg(nDiNum, &stDiCfg);
	if(nAlarmType == ALARM_MD)
	{
		stMdCfg.u8Enable = nAlarmFlag;
	}else if(nAlarmType == ALARM_DI)
	{
		stDiCfg.u8Enable = nAlarmFlag;
	}
	stMdCfg.struLinkAct.u32RecChannel = 1;
	stMdCfg.struLinkAct.u32RecTime = 5*60;
	stMdCfg.struLinkAct.u8RecMode = 0;

	stDiCfg.struLinkAct.u32RecChannel = 1;
	stDiCfg.struLinkAct.u32RecTime = 5*60;
	stDiCfg.struLinkAct.u8RecMode = 0;
	nvr_logon_set_di_cfg(0, &stDiCfg);
	(void)stMdCfg;
	//hi_platform_set_vmd_cfg(0, &stMdCfg);
	//hi_platform_set_di_cfg(nDiNum, &stDiCfg);

	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_set_alarm_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	mxml_node_t *pHead, *pAlarm, *pNode;

	pHead = mxmlNewXML("1.0");
	pAlarm = mxmlNewElement(pHead, "SET_ALARM_RES");

	pNode = mxmlNewElement(pAlarm, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pAlarm, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_SET_ALARM, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}

static int hi_dianxin_set_alarm_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_set_alarm_proc(pBodyBuf, nBodyLen);
	hi_dianxin_set_alarm_res(pDianxinCtrl);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hi_dianxin_get_alarm_proc(char *pBodyBuf, int nBodyLen, int *nAlarmType)
{
	char *p;
	mxml_node_t *pHead, *pAlarm, *pNode;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pAlarm = dianxin_mxml_node_get(pHead, "GET_ALARM_REQ");
	if((pNode = dianxin_mxml_node_get(pAlarm, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	if((pNode = dianxin_mxml_node_get(pAlarm, "ChannelNo")) != NULL)
	{

	}
	if((pNode = dianxin_mxml_node_get(pAlarm, "AlarmType")) != NULL)
	{
		*nAlarmType = atoi(MXMLGETTEXT(pNode, 0));
	}
	
	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_get_alarm_res(DIANXIN_CTRL *pDianxinCtrl, int nAlarmType)
{	
#if 0
	char szSendBuf[1024];
	char szStr[64];
	mxml_node_t *pHead, *pAlarm, *pNode;
	HI_DEV_MD_CFG_S stMdCfg;
	HI_DEV_DI_CFG_S stDiCfg;
	hi_platform_get_vmd_cfg(0, &stMdCfg);
	hi_platform_get_di_cfg(0, &stDiCfg);
	
	pHead = mxmlNewXML("1.0");
	pAlarm = mxmlNewElement(pHead, "GET_ALARM_RES");

	pNode = mxmlNewElement(pAlarm, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pAlarm, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);
	
	pNode = mxmlNewElement(pAlarm, "AlarmFlag");
	memset(szStr, 0, sizeof(szStr));
	if(nAlarmType == ALARM_MD)
	{
		sprintf(szStr, "%d", stMdCfg.u8Enable);
	}else if(nAlarmType == ALARM_DI)
	{
		sprintf(szStr, "%d", stDiCfg.u8Enable);
	}
	mxmlNewText(pNode, 0, szStr);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_GET_RECORD_LOOP, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}
#endif
	return 0;
}

static int hi_dianxin_get_alarm_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	int nAlarmType = 0;
	hi_dianxin_get_alarm_proc(pBodyBuf, nBodyLen, &nAlarmType);
	hi_dianxin_get_alarm_res(pDianxinCtrl, nAlarmType);
	return 0;
}

#if 0
////////////////////////////////////////////////////////////////////////////////////////////
int hi_dianxin_alarm_notify_body_req(DIANXIN_CTRL *pDianxinCtrl, LPNVR_ALARM_INFO_S pAlarmInfo, char *pBodyBuf, int nBodyLen)
{
	char szStr[64];
	mxml_node_t *pHead, *pAlarmNOtify, *pNode;
	HI_DIANXIN_PLATRORM_CFG *pDianxinPlatform = &pDianxinCtrl->stPlatformCfg;

	pHead= mxmlNewXML("1.0");
	pAlarmNOtify = mxmlNewElement(pHead, "ALARM_NOTIFY_REQ");
	pNode = mxmlNewElement(pAlarmNOtify, "DevID");
	mxmlNewText(pNode, 0, pDianxinPlatform->szDevId);

	pNode = mxmlNewElement(pAlarmNOtify, "ChannelNo");
	mxmlNewText(pNode, 0, "0");

	if(pAlarmInfo->u8AlarmType == HI_MD_HAPPEN)
	{
		pNode = mxmlNewElement(pAlarmNOtify, "AlarmType");
		mxmlNewText(pNode, 0, "1");
		pNode = mxmlNewElement(pAlarmNOtify, "AlarmDesc");
		mxmlNewText(pNode, 0, "Motion Detection Alarm Happen");
	}else if(pAlarmInfo->u8AlarmType == HI_MD_DISAPPEAR)
	{
		pNode = mxmlNewElement(pAlarmNOtify, "AlarmType");
		mxmlNewText(pNode, 0, "1");
		pNode = mxmlNewElement(pAlarmNOtify, "AlarmDesc");
		mxmlNewText(pNode, 0, "Motion Detection Alarm Disappear");
	}else if(pAlarmInfo->u8AlarmType == HI_ALARM_IN_HAPPEN)
	{
		pNode = mxmlNewElement(pAlarmNOtify, "AlarmType");
		mxmlNewText(pNode, 0, "4");
		pNode = mxmlNewElement(pAlarmNOtify, "AlarmDesc");
		mxmlNewText(pNode, 0, "DIDO Alarm Disappear");
		pNode = mxmlNewElement(pAlarmNOtify, "DIDONo");
		memset(szStr, 0, sizeof(szStr));
		sprintf(szStr, "%d", pAlarmInfo->u8AlarmInPutCh);
		mxmlNewText(pNode, 0, szStr);
	}

	mxmlSaveString(pHead, pBodyBuf, nBodyLen, MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	return 0;
}
#endif

#if 0
int hi_dianxin_alarm_notify_req(DIANXIN_CTRL *pDianxinCtrl, LPHI_ALARM_INFO_S pAlarmInfo)
{
	char szSendBuf[512];
	hi_dianxin_alarm_notify_body_req(pDianxinCtrl, pAlarmInfo, szSendBuf, sizeof(szSendBuf));
	pDianxinCtrl->nSeq ++;
	if(pDianxinCtrl->nSocketFd <= 0)
	{
		return 0;
	}
	return hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_ALARM_NOTIFY, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf));
}
#endif

static int hi_dianxin_alarm_notify_res(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	printf("the buf is %s\r\n", pBodyBuf);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
static int hi_dianxin_query_storage_proc(char *pBodyBuf, int nBodyLen)
{
	char *p;
	mxml_node_t *pHead, *pStorage, *pNode;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pStorage = dianxin_mxml_node_get(pHead, "STORE_QUERY_REQ");
	if((pNode = dianxin_mxml_node_get(pStorage, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}
	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_query_storage_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	char szStr[64];
	int ret = 0;
	int i = 0;
	int totalSize = 0;
	int freeSize = 0;
	mxml_node_t *pHead, *pStorage, *pNode;

	StorageDisksInfo stDisks_info;
	ret = storage_lib_get_disk_info(&stDisks_info);
	if (ret < 0)	return -1;

	for (i = 0; i < stDisks_info.disk_num; i++)
	{
		totalSize += stDisks_info.disk_info[i].total_size;
		freeSize += stDisks_info.disk_info[i].free_size;
	}

	pHead = mxmlNewXML("1.0");
	pStorage = mxmlNewElement(pHead, "STORE_QUERY_RES");

	pNode = mxmlNewElement(pStorage, "Result");
	if(stDisks_info.disk_num == 0)
	{
		mxmlNewText(pNode, 0, "0");//?��
	}
	else
	{
		mxmlNewText(pNode, 0, "1");
	}

	pNode = mxmlNewElement(pStorage, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	pNode = mxmlNewElement(pStorage, "TotalSize");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d",totalSize);
	mxmlNewText(pNode, 0, szStr);

	pNode = mxmlNewElement(pStorage, "FreeSize");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", freeSize);
	mxmlNewText(pNode, 0, szStr);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_QUERY_STORAGE, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}


static int hi_dianxin_query_storage_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_query_storage_proc(pBodyBuf, nBodyLen);
	hi_dianxin_query_storage_res(pDianxinCtrl);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hi_dianxin_format_storage_proc(char *pBodyBuf, int nBodyLen)
{
	char *p;
	mxml_node_t *pHead, *pStorage, *pNode;

	p = malloc(nBodyLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBodyBuf, nBodyLen);
	p[nBodyLen] = '\0';

	pHead = mxmlLoadString(NULL, p, MXML_NO_CALLBACK);
	pStorage = dianxin_mxml_node_get(pHead, "STORE_FORMAT_REQ");
	if((pNode = dianxin_mxml_node_get(pStorage, "DevID")) == NULL)
	{
		mxmlDelete(pHead);
		free(p);
		return -1;
	}

	mxmlDelete(pHead);
	free(p);
	return 0;
}

static int hi_dianxin_format_storage_res(DIANXIN_CTRL *pDianxinCtrl)
{
	
	char szSendBuf[1024];
	char szStr[64];
	int i, ret= 0;
	mxml_node_t *pHead, *pStorage, *pNode;
	int nPrecent = 0;
	int totalSize = 0,freeSize = 0;
	StorageDisksInfo stDisks_info;
	int nStatus = 0;
	
	printf("format start ######@@@@@@@@@@@@@@\r\n");
	ret = storage_lib_get_disk_info(&stDisks_info);
	if (ret < 0)	
		return -1;

	for (i = 0; i < stDisks_info.disk_num; i++)
	{
		totalSize += stDisks_info.disk_info[i].total_size;
		freeSize += stDisks_info.disk_info[i].free_size;
	}

	pHead = mxmlNewXML("1.0");
	pStorage = mxmlNewElement(pHead, "STORE_FORMAT_RES");
	pNode = mxmlNewElement(pStorage, "Result");
	if(stDisks_info.disk_num == 0)
	{
		mxmlNewText(pNode, 0, "1");
	}else
	{
		mxmlNewText(pNode, 0, "0");
	}

	pNode = mxmlNewElement(pStorage, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	pNode = mxmlNewElement(pStorage, "TotalSize");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", totalSize);
	mxmlNewText(pNode, 0, szStr);

	pNode = mxmlNewElement(pStorage, "FreeSize");
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", freeSize);
	mxmlNewText(pNode, 0, szStr);
	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_STORAGE_FORMAT, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0)
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
		//printf("###########@@@@@@@@@@@@@@@@@@@@\r\n");
	}	
	printf("send resp %s\r\n", szSendBuf);

	for (i = 0; i < stDisks_info.disk_num; i++)
	{
		ret = storage_lib_format_disk_start(i, 0);	
		if (ret < 0)
		{
			nStatus = -1;
			break;
		}
		
		do{			
			ret= storage_lib_format_get_percent(&nPrecent);
			if (nPrecent >= 100)
			{
				nStatus = 0;
				break;
			}

			if (nPrecent < 0 || nPrecent > 100 || ret < 0)
			{
				nStatus = -1;
				goto format_fail;
			}

			usleep(10000);
		}while(1);
		
		ret = storage_lib_format_disk_stop(i);
		if (ret < 0) goto format_fail;
		
		continue;
		
format_fail:	
		storage_lib_format_disk_stop(i);
		nStatus = -1;
		break;
		
	}

	
	printf("format stop @@@@@@@@@@@@@@\r\n");
	
	return 0;
}


static int hi_dianxin_format_storage_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_format_storage_proc(pBodyBuf, nBodyLen);
	hi_dianxin_format_storage_res(pDianxinCtrl);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int hi_dianxin_set_format_proc(char *pBodyBuf, int nBodyLen)
{
	//format disk
	//hi_platform_set_format_cfg();
	return 0;
}

static int hi_dianxin_set_format_res(DIANXIN_CTRL *pDianxinCtrl)
{
	char szSendBuf[1024];
	mxml_node_t *pHead, *pAlarm, *pNode;

	pHead = mxmlNewXML("1.0");
	pAlarm = mxmlNewElement(pHead, "FORMAT_RES");

	pNode = mxmlNewElement(pAlarm, "Result");
	mxmlNewText(pNode, 0, "0");
	pNode = mxmlNewElement(pAlarm, "DevID");
	mxmlNewText(pNode, 0, pDianxinCtrl->stPlatformCfg.szDevId);

	mxmlSaveString(pHead, szSendBuf, sizeof(szSendBuf), MXML_NO_CALLBACK);
	mxmlDelete(pHead);
	if(hi_dianxin_message_send(pDianxinCtrl->nSocketFd, CMD_SET_FORMAT, pDianxinCtrl->nSeq, szSendBuf, strlen(szSendBuf)) < 0 )
	{
		net_close_socket(&pDianxinCtrl->nSocketFd);
		pDianxinCtrl->nLoginStatus = DIANXIN_NO_LOGIN;
	}

	return 0;
}

static int hi_dianxin_set_format_req(DIANXIN_CTRL *pDianxinCtrl, char *pBodyBuf, int nBodyLen)
{
	hi_dianxin_set_format_proc(pBodyBuf, nBodyLen);
	hi_dianxin_set_format_res(pDianxinCtrl);
	return 0;
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////

static const DIANXIN_FUNC_STRUCT dianxin_func_table[] =
{
	{CMD_LOGIN, 			hi_dianxin_login_res},
	{CMD_ABILITYUP, 		hi_dianxin_abilityup_res},
	{CMD_HEARTBEAT, 		hi_dianxin_heart_beat_res},
	{CMD_EXCEPTION_NOTIFY, 	hi_dianxin_exception_notify_res},
	{CMD_VIDEO_PLAY, 		hi_dianxin_video_play_res},
	{CMD_NOTIFY_PICTURE, 	hi_dianxin_notify_picture_res},
	{CMD_ALARM_NOTIFY, 		hi_dianxin_alarm_notify_res},
		
	{CMD_SET_CHANNEL, 		hi_dianxin_set_channel_req},
	{CMD_GET_CHANNEL, 		hi_dianxin_get_channel_req},
		
	{CMD_GET_WIFI, 			hi_dianxin_get_wifi_req},
	{CMD_SET_WIFI, 			hi_dianxin_set_wifi_req},
		
	{CMD_VIDEO_PTZ, 		hi_dianxin_video_ptz_req},
		
	{CMD_SET_ESTORE, 		hi_dianxin_set_estore_req},
	{CMD_GET_ESTORE, 		hi_dianxin_get_estore_req},
		
	{CMD_ADD_RECORD_POLICY, 	hi_dianxin_add_record_policy_req},
	{CMD_QUERY_RECORD_POLICY, 	hi_dianxin_query_record_policy_req},
	{CMD_DEL_RECORD_POLICY, 	hi_dianxin_del_record_policy_req},
		
	{CMD_REBOOT, 		hi_dianxin_reboot_req},
	{CMD_UPGRADE, 		hi_dianxin_upgrade_req},
	{CMD_NVR_BIND_CAMERA, hi_nvr_bind_camera},
	{CMD_NVR_MODIFY_CAMERA, hi_nvr_modify_camera},
	{CMD_NVR_REMOVE_CAMERA, hi_nvr_remove_camera},
	{CMD_NVR_QUERY_CAMERA, hi_dianxin_query_camera_req},
	{CMD_FILE_QUERY,	hi_dianxin_file_query_req},
	{CMD_RECORD_PLAY, 	hi_dianxin_record_play_res},
		
	{CMD_ADD_PICTURE_POLICY, 	hi_dianxin_add_picture_policy_req},
	{CMD_QUERY_PICTURE_POLICY, 	hi_dianxin_query_picture_policy_req},
	{CMD_DEL_PICTURE_POLICY, 	hi_dianxin_del_picture_policy_req},
		
	{CMD_SET_RECORD_FULL, hi_dianxin_set_record_full_req},
	{CMD_GET_RECORD_FULL, hi_dianxin_get_record_full_req},

	{CMD_SET_RECORD_LOOP, hi_dianxin_set_record_loop_req},
	{CMD_GET_RECORD_LOOP, hi_dianxin_get_record_loop_req},

	{CMD_SET_ALARM, hi_dianxin_set_alarm_req},
	{CMD_GET_ALARM, hi_dianxin_get_alarm_req},

	{CMD_QUERY_STORAGE, hi_dianxin_query_storage_req},
	{CMD_STORAGE_FORMAT, hi_dianxin_format_storage_req},

	{CMD_SET_FORMAT, hi_dianxin_set_format_req},
		
};




static int hi_dianxin_recv_proc(DIANXIN_CTRL *pDianxinCtrl, char *pBuf, int nBufLen, unsigned char *pMd5)
{
	int i;
	int nMaxId;
	int nLen;
	int nHeadLen = sizeof(HI_DIANXIN_MSG_HEAD);
	HI_DIANXIN_MSG_HEAD *pMsgHead;
	pMsgHead = (HI_DIANXIN_MSG_HEAD *)pBuf;
	if(pMsgHead->u16Head != htons(0xFAF5))
	{
		__E("the recv msg head %x is err!\r\n", pMsgHead->u16Head);
		return -1;
	}
	pDianxinCtrl->nSeq = htons(pMsgHead->u16Seq);
	nLen = htons(pMsgHead->u16Len);
	//printf(">>>>>>>>>>the nLen is %d\r\n", nLen);
	//printf(">>>>>>>>>>the strlen is %d\r\n", strlen(pBuf + nHeadLen));
#if 1
	char *p;
	p = malloc(nLen + 1);
	if(!p)
	{
		__E("malloc space err!\r\n");
		return -1;
	}
	strncpy(p, pBuf + nHeadLen, nLen);
	p[nLen] = '\0';
	//printf(">>>>>>>>>the recv buf is %s\r\n", p);
	free(p);
#endif
	nMaxId = sizeof(dianxin_func_table) / sizeof(dianxin_func_table[0]);
	printf(">>>>>>>>>the cmd is %x\r\n", pMsgHead->u8Cmd);
	for(i = 0; i < nMaxId; i++)
	{
		if(dianxin_func_table[i].eCmdType == pMsgHead->u8Cmd)
		{
			dianxin_func_table[i].func_proc(pDianxinCtrl, pBuf + nHeadLen, nLen);
			break;
		}
	}	
	return 0;
}







int hi_dianxin_recv(DIANXIN_CTRL *pDianxinCtrl)
{
	char szRecvBuf[4096] = {0};
	int nRet;
	int fd = pDianxinCtrl->nSocketFd;

	if ((nRet = net_select(&fd, 0x1, 0x1, 5000)) < 0)
	{
		__E("the dianxin message recv err!\r\n");
		net_close_socket(&fd);
		pDianxinCtrl->nSocketFd = -1;
		return -1;
	}
	if (nRet == (fd | 0x10000))
	{
		int nLen, nXmlLen;
		unsigned char md5[16];
		HI_DIANXIN_MSG_HEAD *pMsgHead = (HI_DIANXIN_MSG_HEAD*)szRecvBuf;

		if(net_tcp_noblock_recv(pDianxinCtrl->nSocketFd, (char *)pMsgHead, 
					DIANXIN_MSG_HEAD_LEN, DIANXIN_MSG_HEAD_LEN, TIMEOUT) != DIANXIN_MSG_HEAD_LEN)
		{
			__E("the dianxin message recv message head err!\r\n");
			net_close_socket(&fd);
			pDianxinCtrl->nSocketFd = -1;
			return -1;
		}
		if(pMsgHead->u16Head != htons(0xFAF5))
		{
			__E("the recv msg head %x is err!\r\n", pMsgHead->u16Head);
			net_close_socket(&fd);
			pDianxinCtrl->nSocketFd = -1;
			return -1;
		}
		nXmlLen = htons(pMsgHead->u16Len);
		nLen = nXmlLen + 16;	/* md5 length */
		if(net_tcp_noblock_recv(pDianxinCtrl->nSocketFd, szRecvBuf+DIANXIN_MSG_HEAD_LEN, 
							sizeof(szRecvBuf)-DIANXIN_MSG_HEAD_LEN, 
							nLen, TIMEOUT) != nLen)
		{
			__E("the dianxin message recv err!\r\n");
			net_close_socket(&fd);
			pDianxinCtrl->nSocketFd = -1;
			return -1;
		}
		memcpy(md5, &szRecvBuf[DIANXIN_MSG_HEAD_LEN + nXmlLen], 16);
		szRecvBuf[DIANXIN_MSG_HEAD_LEN + nXmlLen] = 0;
		if(hi_dianxin_recv_proc(pDianxinCtrl, szRecvBuf, nLen + DIANXIN_MSG_HEAD_LEN, md5) < 0)
		{
			net_close_socket(&fd);
			pDianxinCtrl->nSocketFd = -1;
			return -1;
		}
	}
	return 0;
}




