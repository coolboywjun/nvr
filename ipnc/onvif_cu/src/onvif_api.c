#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <mxml.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/route.h>
#include <net/if_arp.h>
#include <netinet/tcp.h>
#include <net_api.h>
#include <time.h>
#include <sys/time.h>
#include <rtsp_client.h>
#include "onvif_api.h"

#include "onvif_msg_body.h"

#define ONVIF_MAX_SCOPE		10
#define ONVIF_MAX_MSG_LEN	40000


typedef struct _onvif_rtsp_user_date
{
	int streamID;  //主码流，次码流
	int chn;		//通道号
}onvif_rtsp_user_date_t;

int httpgetLen(char *contentLine)
{
	int i = 0;
	char acData[32] = {0};
	char *p = contentLine;
	p += strlen("Content-Length");
	while((*p!='\r')&&(*p!='\n'))
	{
		if (*p >= '0' && *p <= '9')
		{
			acData[i++] = *p;
		}
		p++;
	}
	acData[i] = '\0';
	return atoi(acData);
}


static int _http_recv(int nSocket, char *recvBuf, int buf_size)
{
	char *p = NULL;
	int ret = 0;
	int recvLen = 0;
	int contentLen = 0;
	int tryTime = 0;
	
	do
	{
		ret = net_tcp_noblock_recv(nSocket, recvBuf+recvLen, buf_size - recvLen, 0, 3000);
		if(ret < 0)
		{		
			net_close_socket(&nSocket);
			return -1;
		}
		recvLen += ret;
		p = strstr(recvBuf, "Content-Length");
		if (p == NULL) 
			continue;
		contentLen = httpgetLen(p);
		//printf("contentLen = %d\r\n", contentLen);
		p = strstr(recvBuf, "\r\n\r\n");
		if (p == NULL)
			continue;
		p += strlen("\r\n\r\n");
		if (strlen(p) == contentLen)
		{
			//printf("############### recv http complete!!!!!!!\r\n");			
			//printf("recvLen = %d\r\n", recvLen);
			break;
		}
		tryTime++;
		if (tryTime >= 5)
		{
			//printf("############### recv http abnormal!!!!!!!\r\n");			
			break;
		}
		
	}while(1);

	return 0;
}

char *onvif_get_profile_token(onvif_profile_list_t *profile_list, int index)
{
	int i = 0;
	onvif_profile_list_t *pDate = NULL;
	struct list_head *pos = NULL, *n = NULL;
	list_for_each_safe(pos, n, &(profile_list->list))
	{
		if (i == index)
		{
			pDate = list_entry(pos, onvif_profile_list_t, list);			
			if (NULL != pDate)
			{
				return pDate->data.profileToken;
			}
			return NULL;
		}
		i++;
	}

	return NULL;
}

char *onvif_get_video_source_token(onvif_profile_list_t *profile_list, int index)
{
	int i = 0;
	onvif_profile_list_t *pDate = NULL;
	struct list_head *pos = NULL, *n = NULL;
	list_for_each_safe(pos, n, &(profile_list->list))
	{
		if (i == index)
		{
			pDate = list_entry(pos, onvif_profile_list_t, list);			
			if (NULL != pDate)
			{
				return pDate->data.VideoSourceCfg.SourceToken;
			}
			return NULL;
		}
		i++;
	}

	return NULL;
}

char *onvif_get_video_encode_token(onvif_profile_list_t *profile_list, int index)
{
	int i = 0;
	onvif_profile_list_t *pDate = NULL;
	struct list_head *pos = NULL, *n = NULL;
	list_for_each_safe(pos, n, &(profile_list->list))
	{
		if (i == index)
		{
			pDate = list_entry(pos, onvif_profile_list_t, list);			
			if (NULL != pDate)
			{
				return pDate->data.VideoEncodeCfg.token;
			}
			return NULL;
		}
		i++;
	}

	return NULL;
}

char *onvif_get_video_encode_name(onvif_profile_list_t *profile_list, int index)
{
	int i = 0;
	onvif_profile_list_t *pDate = NULL;
	struct list_head *pos = NULL, *n = NULL;
	list_for_each_safe(pos, n, &(profile_list->list))
	{
		if (i == index)
		{
			pDate = list_entry(pos, onvif_profile_list_t, list);			
			if (NULL != pDate)
			{
				return pDate->data.VideoEncodeCfg.Name;
			}
			return NULL;
		}
		i++;
	}

	return NULL;
}


char *onvif_get_preset_token(onvif_ptzpresets_list_t *profile_list, int presetNo)
{
	
	int i = 0;
	onvif_ptzpresets_list_t *pDate = NULL;
	struct list_head *pos = NULL, *n = NULL;
	list_for_each_safe(pos, n, &(profile_list->list))
	{
		if (i == presetNo)
		{
			pDate = list_entry(pos, onvif_ptzpresets_list_t, list);			
			if (NULL != pDate)
			{
				return pDate->data.Presettoken;
			}
			return NULL;
		}
		i++;
	}

		return NULL;
}


#if 0
static OnvifCtrl_t g_stOnvifCtrl;


int onvif_list_init()
{
	memset(&g_stOnvifCtrl.list_device, 0, sizeof(onvif_device_list_t));
	INIT_LIST_HEAD(&g_stOnvifCtrl.list_device.list);
	return 0;
}

int onvif_list_add(discover_info_t *pData)
{
	onvif_device_list_t *p = NULL;
	p = (onvif_device_list_t *)malloc(sizeof(onvif_device_list_t));
	if (p == NULL)
		return -1;
	
	memset(p, 0 , sizeof(onvif_device_list_t));
	memcpy(&p->data , pData, sizeof(discover_info_t));
	list_add_tail(&p->list, &g_stOnvifCtrl.list_device.list);
	return 0;
}

int onvif_list_free()
{
	onvif_device_list_t *pDate = NULL;
	struct list_head *pos = NULL, *n = NULL;
	list_for_each_safe(pos, n, &(g_stOnvifCtrl.list_device.list))
	{
		pDate = list_entry(pos, onvif_device_list_t, list);
		if (NULL != pDate)
		{
			list_del(&pDate->list);
			free(pDate);
			pDate = NULL;
		}
	}

	return 0;
}
#endif


#if 0
void profile_show(profile_data_t *pData)
{
	printf("profileName:%s\r\n", pData->profileName);
	printf(">>>>>>>>>>>>>>>>>>>>>>>>VideoSourceCfg");
	printf("Name:%s\r\n", pData->VideoSourceCfg.Name);	
	printf("token:%s\r\n", pData->VideoSourceCfg.token);	
	printf("SourceToken:%s\r\n", pData->VideoSourceCfg.SourceToken);	
	printf("x = %d y = %d w = %d h = %d\r\n", pData->VideoSourceCfg.Bounds.x,
		pData->VideoSourceCfg.Bounds.y, pData->VideoSourceCfg.Bounds.width, pData->VideoSourceCfg.Bounds.height);
}


int profile_list_clear(onvif_profile_list_t *profile_list)
{
	onvif_profile_list_t *pDate = NULL;
	struct list_head *pos = NULL, *n = NULL;
	list_for_each_safe(pos, n, &(profile_list->list))
	{
		pDate = list_entry(pos, onvif_profile_list_t, list);
		if (NULL != pDate)
		{
			printf("@@@@@@@@@@@@@@@@\r\n");
			list_del(&pDate->list);
			free(pDate);
			pDate = NULL;
		}
	}

	return 0;
}

int profile_list_add(onvif_profile_list_t *profile_list, profile_data_t *pData)
{
	onvif_profile_list_t *p = NULL;
	p = (onvif_profile_list_t *)malloc(sizeof(onvif_profile_list_t));
	if (p == NULL)
		return -1;
	
	memset(p, 0 , sizeof(onvif_profile_list_t));
	memcpy(&p->data , pData, sizeof(profile_data_t));
	list_add_tail(&p->list, &profile_list->list);
	printf("add============\r\n");
	return 0;
}

int profile_list_show(onvif_profile_list_t *profile_list)
{
	onvif_profile_list_t *pDate = NULL;
	struct list_head *pos = NULL, *n = NULL;
	list_for_each_safe(pos, n, &(profile_list->list))
	{
		pDate = list_entry(pos, onvif_profile_list_t, list);
		if (NULL != pDate)
		{			
			printf("show============\r\n");
			profile_show(&pDate->data);
			//printf("%d\r\n", pDate->a);
		}
	}

	return 0;
}
#else

#endif


static int __parseDevTypeFromScope(const char *scopes, char *devTypebuf, int bufsize)
{
	char *pTmp = NULL;
	int i = 0;
	if (scopes == NULL || devTypebuf == NULL || bufsize <= 0)
		return -1;

	pTmp = strstr(scopes, "onvif://www.onvif.org/name/");
	if (pTmp == NULL) 
		return -1;

	pTmp += strlen("onvif://www.onvif.org/name/");
	do
	{
		if (i >= bufsize)
			return -1;
		if ((*pTmp != ' ')&&(*pTmp != '\0'))
			devTypebuf[i++] = *pTmp;
		else
		{
			devTypebuf[i] = '\0';
			break;
		}		
		pTmp++;
	}while(1);

	return 0;
}

static int _onvif_discover_message_parse(int index, const char *mxmlMessage, fSearchCb cbSearchFunc)
{
	const char *pTmp = NULL;
	mxml_node_t *pHead = NULL, *pNode = NULL;
	discover_info_t stData;
	int bShowPort = 0;
	char acServerUrl[128];
	char *p1 = NULL;
	int ret = 0;
	char acDevType[32];

	//printf("@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");	
	//printf("@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
	//printf("@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
	//printf("%s\r\n", mxmlMessage);
	pHead = mxmlLoadString(NULL, mxmlMessage, MXML_NO_CALLBACK);
	if (pHead == NULL )
	{
		printf("[%s %d]parse err\r\n", __FILE__, __LINE__);
		return -1;
	}
	
	if((pNode = mxmlGuessElement(pHead, pHead, "d:Types", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	pTmp = mxmlGetText(pNode, 0);
	strcpy(stData.devType,pTmp);
		
	//http://192.168.1.6:8000/onvif/device_service
	//http://192.168.1.6/onvif/device_service
	if((pNode = mxmlGuessElement(pHead, pHead, "d:XAddrs", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	pTmp = mxmlGetText(pNode, 0);	
	strcpy(stData.devServiceAddr, pTmp);
	//printf("######%s\r\n", pTmp);
	if (strncasecmp(pTmp, "http://", 7)!= 0)
		goto err;
	if (strstr(pTmp + 7 ,":") != NULL)
		bShowPort = 1;
	if (strlen(pTmp) > sizeof(acServerUrl))
		goto err;
	strcpy(acServerUrl, pTmp + 7);
	p1 = strstr(acServerUrl, "/");
	*p1 = '\0';
	if (bShowPort)
	{
		p1 = strstr(acServerUrl, ":");
		*p1 = '\0';	p1++;
		strcpy(stData.devIP,acServerUrl);
		strcpy(stData.port, p1);
		//sscanf(acServerUrl, "%s:%s", stData.devIP, stData.port);
	}
	else
	{
		sscanf(acServerUrl, "%s", stData.devIP);		
		strcpy(stData.port, "80");
	}
	//printf("%s\r\n", stData.devServiceAddr);
	//printf("%s:%s\r\n", stData.devIP, stData.port);

	if((pNode = mxmlGuessElement(pHead, pHead, "a:Address", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	pTmp = mxmlGetText(pNode, 0);
	if (pTmp == NULL)
		goto err;
	strcpy(stData.EPAddr, pTmp);

	if((pNode = mxmlGuessElement(pHead, pHead, "d:Scopes", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	pTmp = mxmlGetText(pNode, 0); 
	if (pTmp == NULL)
		goto err;
	//printf("Scopes: %s\r\n", pTmp);
	ret = __parseDevTypeFromScope(pTmp, acDevType, sizeof(acDevType));
	if (ret < 0) goto err;

	printf("====acDevType = %s=====\r\n", acDevType);
	if (strcasecmp(acDevType, "NVR") == 0)
		return -1;	
	//strcpy(stData.scopes, pTmp);

	if (cbSearchFunc)
		cbSearchFunc(index, &stData);

	mxmlDelete(pHead);
	return 0;
err:
	mxmlDelete(pHead);
	return -1;
}


static int _onvif_parse_profiles(const char *httpMessage, onvif_profile_list_t *list_profile)
{	
	const char *pTmp = NULL;
	const char *mxmlMessage = NULL;
	profile_data_t data;

	//printf("%s\r\n", httpMessage);

	mxml_node_t *pHead = NULL, *pProfileResp = NULL,*pNode = NULL, *pNode1 = NULL, *pNode2 = NULL;

	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	pTmp = strstr(httpMessage, "\r\n\r\n");
	mxmlMessage = pTmp+ strlen("\r\n\r\n");
	//printf("%s\r\n", mxmlMessage);
	pHead = mxmlLoadString(NULL, mxmlMessage, MXML_NO_CALLBACK);
	if (pHead == NULL )
	{
		printf("[%s %d]parse err\r\n", __FILE__, __LINE__);
		return -1;
	}
#if 0	
	profile_list_clear(list_profile);
#else
	_onvif_list_clear(onvif_profile_list_t, list_profile);
#endif
	if((pProfileResp = mxmlGuessElement(pHead, pHead, "trt:GetProfilesResponse", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	
	pNode = pProfileResp;
	do
	{
		if((pNode = mxmlGuessElement(pNode, pProfileResp, "trt:Profiles", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			break;
			goto err;
		}
		pTmp = mxmlElementGetAttr(pNode, "token");
		if (pTmp == NULL) goto err;		
		strcpy(data.profileToken, pTmp);		
		//printf("###########pTmp = %s\r\n", pTmp);
		
		if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:Name", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		
		pTmp = mxmlGetText(pNode1, 0);
		if (pTmp == NULL) goto err;		
		strcpy(data.profileName, pTmp);
		//printf("###########pTmp = %s\r\n", pTmp);

		//VideoSourceConfiguration
		////////////////////////////////////////////////
		if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:VideoSourceConfiguration", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlElementGetAttr(pNode1, "token");
		if (pTmp == NULL) 
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		strcpy(data.VideoSourceCfg.token, pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Name", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;	
		strcpy(data.VideoSourceCfg.Name, pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:UseCount", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;	
		data.VideoSourceCfg.UseCount = atoi(pTmp);

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:SourceToken", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;			
		strcpy(data.VideoSourceCfg.SourceToken, pTmp);

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Bounds", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}	
		pTmp = mxmlElementGetAttr(pNode2, "height");
		if (pTmp == NULL) goto err;
		data.VideoSourceCfg.Bounds.height = atoi(pTmp);
		
		pTmp = mxmlElementGetAttr(pNode2, "width");
		if (pTmp == NULL) goto err;
		data.VideoSourceCfg.Bounds.width = atoi(pTmp);
		
		pTmp = mxmlElementGetAttr(pNode2, "y");
		if (pTmp == NULL) goto err;		
		data.VideoSourceCfg.Bounds.y = atoi(pTmp);
		
		pTmp = mxmlElementGetAttr(pNode2, "x");
		if (pTmp == NULL) goto err;
		data.VideoSourceCfg.Bounds.x = atoi(pTmp);

		//AudioSourceConfiguration
		/////////////////////////////////////////////////////////////////
		if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:AudioSourceConfiguration", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlElementGetAttr(pNode1, "token");
		if (pTmp == NULL) goto err;
		strcpy(data.AudioSourceCfg.token, pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Name", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;	
		strcpy(data.AudioSourceCfg.Name, pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:UseCount", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;	
		data.AudioSourceCfg.UseCount = atoi(pTmp);

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:SourceToken", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;			
		strcpy(data.AudioSourceCfg.SourceToken, pTmp);

		//VideoEncoderConfiguration
		///////////////////////////////////////////////////////////////////////
		if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:VideoEncoderConfiguration", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlElementGetAttr(pNode1, "token");
		if (pTmp == NULL) goto err;
		strcpy(data.VideoEncodeCfg.token, pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Name", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err; 
		strcpy(data.VideoEncodeCfg.Name, pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:UseCount", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err; 
		data.VideoEncodeCfg.UseCount = atoi(pTmp);

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Encoding", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		data.VideoEncodeCfg.Encoding = tt__VideoEncoding__H264;

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:BitrateLimit", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		data.VideoEncodeCfg.Bitrate = atoi(pTmp);
		
		data.VideoEncodeCfg.Resolution.Width = 1280;
		data.VideoEncodeCfg.Resolution.Height = 720;
		data.VideoEncodeCfg.FrameRate = 25;
		data.VideoEncodeCfg.Gop = 75;

		//AudioEncoderConfiguration
		///////////////////////////////////////////////////////////////////////
		if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:AudioEncoderConfiguration", NULL, NULL, MXML_DESCEND)) != NULL)
		{
			pTmp = mxmlElementGetAttr(pNode1, "token");
			if (pTmp == NULL) goto err;
			strcpy(data.AudioEncodeCfg.token, pTmp);
			
			if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Name", NULL, NULL, MXML_DESCEND)) == NULL)
				goto err;
			pTmp = mxmlGetText(pNode2, 0);
			if (pTmp == NULL) goto err; 
			strcpy(data.AudioEncodeCfg.Name, pTmp);
			
			if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:UseCount", NULL, NULL, MXML_DESCEND)) == NULL)
			{
				printf("[%s %d]err\r\n", __FILE__, __LINE__);
				goto err;
			}
			pTmp = mxmlGetText(pNode2, 0);
			if (pTmp == NULL) goto err; 
			data.AudioEncodeCfg.UseCount =  atoi(pTmp);
			
			data.AudioEncodeCfg.Bitrate = 16000;
			data.AudioEncodeCfg.SampleRate = 8000;
			data.AudioEncodeCfg.Encoding = tt__AudioEncoding__G711;
		}
		else
		{
			memset(&data.AudioEncodeCfg, 0, sizeof(data.AudioEncodeCfg));
		}

	#if 0
		profile_list_add(list_profile, &data);
	#else
		_onvif_list_add(onvif_profile_list_t, list_profile, profile_data_t, &data);
	#endif
		///////////////////////////////////////////////////////////
		pNode = mxmlWalkNext(pNode, pProfileResp, MXML_DESCEND);
		if (pNode == NULL)
			break;
	}while(1);

	//printf("end===========\r\n");
	//profile_list_show(list_profile);

	mxmlDelete(pHead);
	return 0;
err:
	mxmlDelete(pHead);
	return -1;
	
	return 0;
}

static int _onvif_parse_profile(const char *httpMessage, profile_data_t *pdata)
{	
	const char *pTmp = NULL;
	const char *mxmlMessage = NULL;
	//profile_data_t data;

	//printf("%s\r\n", httpMessage);
	
	mxml_node_t *pHead = NULL, *pProfileResp = NULL,*pNode = NULL, *pNode1 = NULL, *pNode2 = NULL;
	
	memset(pdata, 0, sizeof(profile_data_t));
	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	pTmp = strstr(httpMessage, "\r\n\r\n");
	mxmlMessage = pTmp+ strlen("\r\n\r\n");
	pHead = mxmlLoadString(NULL, mxmlMessage, MXML_NO_CALLBACK);
	if (pHead == NULL )
	{
		printf("[%s %d]parse err\r\n", __FILE__, __LINE__);
		return -1;
	}
	
	if((pProfileResp = mxmlGuessElement(pHead, pHead, "trt:GetProfileResponse", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	
	pNode = pProfileResp;

	if((pNode = mxmlGuessElement(pNode, pProfileResp, "trt:Profile", NULL, NULL, MXML_DESCEND)) == NULL)
	{
		goto err;
	}
	pTmp = mxmlElementGetAttr(pNode, "token");
	if (pTmp == NULL) goto err;		
	strcpy(pdata->profileToken, pTmp);			
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:Name", NULL, NULL, MXML_DESCEND)) == NULL)
	{
		printf("[%s %d]err\r\n", __FILE__, __LINE__);
		goto err;
	}
	
	pTmp = mxmlGetText(pNode1, 0);
	if (pTmp == NULL) goto err;		
	strcpy(pdata->profileName, pTmp);
	//printf("###########pTmp = %s\r\n", pTmp);

	//VideoSourceConfiguration
	////////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:VideoSourceConfiguration", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		//pTmp = mxmlElementGetAttr(pNode1, "token");
		//if (pTmp == NULL) 
		//{
		//	printf("[%s %d]err\r\n", __FILE__, __LINE__);
		//	goto err;
		//}
		//strcpy(pdata->VideoSourceCfg.token, pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Name", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;	
		strcpy(pdata->VideoSourceCfg.Name, pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:UseCount", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;	
		pdata->VideoSourceCfg.UseCount = atoi(pTmp);

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:SourceToken", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;			
		strcpy(pdata->VideoSourceCfg.SourceToken, pTmp);

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Bounds", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}	
		pTmp = mxmlElementGetAttr(pNode2, "height");
		if (pTmp == NULL) goto err;
		pdata->VideoSourceCfg.Bounds.height = atoi(pTmp);
		
		pTmp = mxmlElementGetAttr(pNode2, "width");
		if (pTmp == NULL) goto err;
		pdata->VideoSourceCfg.Bounds.width = atoi(pTmp);
		
		pTmp = mxmlElementGetAttr(pNode2, "y");
		if (pTmp == NULL) goto err;		
		pdata->VideoSourceCfg.Bounds.y = atoi(pTmp);
		
		pTmp = mxmlElementGetAttr(pNode2, "x");
		if (pTmp == NULL) goto err;
		pdata->VideoSourceCfg.Bounds.x = atoi(pTmp);
	}

	//AudioSourceConfiguration
	/////////////////////////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:AudioSourceConfiguration", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		pTmp = mxmlElementGetAttr(pNode1, "token");
		if (pTmp == NULL) goto err;
		strcpy(pdata->AudioSourceCfg.token, pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Name", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;	
		strcpy(pdata->AudioSourceCfg.Name, pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:UseCount", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;	
		pdata->AudioSourceCfg.UseCount = atoi(pTmp);

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:SourceToken", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;			
		strcpy(pdata->AudioSourceCfg.SourceToken, pTmp);
	}

	//VideoEncoderConfiguration
	///////////////////////////////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:VideoEncoderConfiguration", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		pTmp = mxmlElementGetAttr(pNode1, "token");
		if (pTmp == NULL) goto err;
		strcpy(pdata->VideoEncodeCfg.token, pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Name", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err; 
		strcpy(pdata->VideoEncodeCfg.Name, pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:UseCount", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err; 
		pdata->VideoEncodeCfg.UseCount = atoi(pTmp);

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Encoding", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;
		//if (strcmp(pTmp, "H264") == 0)
		pdata->VideoEncodeCfg.Encoding = tt__VideoEncoding__H264;
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Resolution", NULL, NULL, MXML_DESCEND)) != NULL)
		{
			mxml_node_t *pNodetmp = NULL;
			if((pNodetmp = mxmlGuessElement(pNode2, pNode2, "tt:Width", NULL, NULL, MXML_DESCEND)) == NULL)
			{
				printf("[%s %d]err\r\n", __FILE__, __LINE__);
				goto err;
			}
			pTmp = mxmlGetText(pNodetmp, 0);
			if (pTmp == NULL) goto err; 
			pdata->VideoEncodeCfg.Resolution.Width = atoi(pTmp);

			if((pNodetmp = mxmlGuessElement(pNode2, pNode2, "tt:Height", NULL, NULL, MXML_DESCEND)) == NULL)
			{
				printf("[%s %d]err\r\n", __FILE__, __LINE__);
				goto err;
			}
			pTmp = mxmlGetText(pNodetmp, 0);
			if (pTmp == NULL) goto err; 
			pdata->VideoEncodeCfg.Resolution.Height= atoi(pTmp);
			
		}

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Quality", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err; 
		pdata->VideoEncodeCfg.Quality= atof(pTmp);

		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:RateControl", NULL, NULL, MXML_DESCEND)) != NULL)
		{
			
			mxml_node_t *pNodetmp = NULL;
			if((pNodetmp = mxmlGuessElement(pNode2, pNode2, "tt:FrameRateLimit", NULL, NULL, MXML_DESCEND)) == NULL)
			{
				printf("[%s %d]err\r\n", __FILE__, __LINE__);
				goto err;
			}
			pTmp = mxmlGetText(pNodetmp, 0);
			if (pTmp == NULL) goto err; 
			pdata->VideoEncodeCfg.FrameRate = atoi(pTmp);

			if((pNodetmp = mxmlGuessElement(pNode2, pNode2, "BitrateLimit", NULL, NULL, MXML_DESCEND)) == NULL)
			{
				printf("[%s %d]err\r\n", __FILE__, __LINE__);
				goto err;
			}
			pTmp = mxmlGetText(pNodetmp, 0);
			if (pTmp == NULL) goto err; 
			pdata->VideoEncodeCfg.Bitrate= atoi(pTmp);

			if((pNodetmp = mxmlGuessElement(pNode2, pNode2, "tt:EncodingInterval", NULL, NULL, MXML_DESCEND)) == NULL)
			{
				printf("[%s %d]err\r\n", __FILE__, __LINE__);
				goto err;
			}
			pTmp = mxmlGetText(pNodetmp, 0);
			if (pTmp == NULL) goto err; 			
			pdata->VideoEncodeCfg.Gop = atoi(pTmp);
			//data.VideoEncodeCfg.Bitrate= atoi(pTmp);		
		}

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:H264", NULL, NULL, MXML_DESCEND)) != NULL)
		{
			
			mxml_node_t *pNodetmp = NULL;
			if((pNodetmp = mxmlGuessElement(pNode2, pNode2, "tt:GovLength", NULL, NULL, MXML_DESCEND)) == NULL)
			{
				printf("[%s %d]err\r\n", __FILE__, __LINE__);
				goto err;
			}
			pTmp = mxmlGetText(pNodetmp, 0);
			if (pTmp == NULL) goto err; 

			if((pNodetmp = mxmlGuessElement(pNode2, pNode2, "tt:H264Profile", NULL, NULL, MXML_DESCEND)) == NULL)
			{
				printf("[%s %d]err\r\n", __FILE__, __LINE__);
				goto err;
			}
			pTmp = mxmlGetText(pNodetmp, 0);
			if (pTmp == NULL) goto err; 
			if (strcasecmp(pTmp, "High") == 0)
				pdata->VideoEncodeCfg.Profile = tt__H264Profile__High;
			else if (strcasecmp(pTmp, "Main") == 0)
				pdata->VideoEncodeCfg.Profile = tt__H264Profile__Main;
			else if (strcasecmp(pTmp, "Baseline") == 0)
				pdata->VideoEncodeCfg.Profile = tt__H264Profile__Baseline;
			else 
				pdata->VideoEncodeCfg.Profile = tt__H264Profile__Baseline;
		}
	}

	//AudioEncoderConfiguration
	///////////////////////////////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:AudioEncoderConfiguration", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		pTmp = mxmlElementGetAttr(pNode1, "token");
		if (pTmp == NULL) goto err;
		strcpy(pdata->AudioEncodeCfg.token, pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Name", NULL, NULL, MXML_DESCEND)) == NULL)
			goto err;
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err; 
		strcpy(pdata->AudioEncodeCfg.Name, pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:UseCount", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err; 
		pdata->AudioEncodeCfg.UseCount =  atoi(pTmp);
		
		pdata->AudioEncodeCfg.Bitrate = 16000;
		pdata->AudioEncodeCfg.SampleRate = 8000;
		pdata->AudioEncodeCfg.Encoding = tt__AudioEncoding__G711;
	}

	//printf("end===========\r\n");
	//profile_list_show(list_profile);

	mxmlDelete(pHead);
	return 0;
err:
	mxmlDelete(pHead);
	return -1;
	
	return 0;
}


static int _onvif_parse_scopes(const char *httpMessage, onvif_scopes_list_t *list_scopes)
{
	const char *pTmp = NULL;
	const char *mxmlMessage = NULL;
	mxml_node_t *pHead = NULL, *pNode = NULL, *pNode1 = NULL, *pNodeResp = NULL;
	scope_data_t data;
	
	//printf("%s\r\n", httpMessage);
	if (httpMessage == NULL)
			return -1;
	
	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	pTmp = strstr(httpMessage, "\r\n\r\n");
	mxmlMessage = pTmp+ strlen("\r\n\r\n");
	
	pHead = mxmlLoadString(NULL, mxmlMessage, MXML_NO_CALLBACK);
	if (pHead == NULL )
	{
		printf("[%s %d]parse err\r\n", __FILE__, __LINE__);
		return -1;
	}
	_onvif_list_clear(onvif_scopes_list_t, list_scopes);
	if((pNodeResp = mxmlGuessElement(pHead, pHead, "tds:GetScopesResponse", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	pNode = pNodeResp;
	
	do
	{
		if((pNode = mxmlGuessElement(pNode, pNodeResp, "tds:Scopes", NULL, NULL, MXML_DESCEND)) == NULL)
		{
			break;
			goto err;
		}

		if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:ScopeDef", NULL, NULL, MXML_DESCEND)) == NULL)
			goto err;
		pTmp = mxmlGetText(pNode1, 0);
		if (pTmp == NULL) goto err;
		strcpy(data.scopeDefalt, pTmp);

		if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:ScopeItem", NULL, NULL, MXML_DESCEND)) == NULL)
			goto err;
		pTmp = mxmlGetText(pNode1, 0);
		if (pTmp == NULL) goto err;
		strcpy(data.scopeItem, pTmp);

		//printf("%s\r\n", data.scopeDefalt);		
		//printf("%s\r\n", data.scopeItem);		
		_onvif_list_add(onvif_scopes_list_t, list_scopes, scope_data_t, &data);
		pNode = mxmlWalkNext(pNode, pNodeResp, MXML_DESCEND);
		if (pNode == NULL)
			break;
		
	}while(1);
	
	//profile_list_show(list_scopes);
	
	mxmlDelete(pHead);
	return 0;
err:
	printf("#########err\r\n");
	mxmlDelete(pHead);
	return -1;	
}

static int _onvif_parse_capibility(const char *httpMessage, device_capability_t *capability)
{	
	const char *pTmp = NULL;
	const char *mxmlMessage = NULL;
	mxml_node_t *pHead = NULL, *pNode = NULL, *pNodeResp = NULL, 
				*pStreamCapability = NULL, *pprofileCapability = NULL;

	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	pTmp = strstr(httpMessage, "\r\n\r\n");
	mxmlMessage = pTmp+ strlen("\r\n\r\n");

	pHead = mxmlLoadString(NULL, mxmlMessage, MXML_NO_CALLBACK);
	if (pHead == NULL )
	{
		printf("[%s %d]parse err\r\n", __FILE__, __LINE__);
		return -1;
	}

	if((pNodeResp = mxmlGuessElement(pHead, pHead, "tds:GetCapabilitiesResponse", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;

	if((pStreamCapability = mxmlGuessElement(pNodeResp, pNodeResp, "tt:StreamingCapabilities", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	
	if((pNode = mxmlGuessElement(pStreamCapability, pStreamCapability, "tt:RTPMulticast", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	pTmp = mxmlGetText(pNode, 0);
	if (pTmp == NULL) goto err;
	if (strcasecmp(pTmp, "true") == 0)
		capability->spportRTPMulticast =  1;
	else
		capability->spportRTPMulticast =  0;
	

	if((pNode = mxmlGuessElement(pNodeResp, pNodeResp, "tt:RTP_TCP", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	pTmp = mxmlGetText(pNode, 0);
	if (pTmp == NULL) goto err;
	if (strcasecmp(pTmp, "true") == 0)
		capability->supportRTP_TCP = 1;
	else
		capability->supportRTP_TCP =  0;

	if((pNode = mxmlGuessElement(pNodeResp, pNodeResp, "tt:RTP_RTSP_TCP", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	pTmp = mxmlGetText(pNode, 0);
	if (pTmp == NULL) goto err;
	if (strcasecmp(pTmp, "true") == 0)
		capability->supportRTP_RTSP_TCP = 1;
	else
		capability->supportRTP_RTSP_TCP =  0;

	if((pprofileCapability = mxmlGuessElement(pNodeResp, pNodeResp, "tt:ProfileCapabilities", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;

	if((pNode = mxmlGuessElement(pprofileCapability, pprofileCapability, "tt:MaximumNumberOfProfiles", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	pTmp = mxmlGetText(pNode, 0);
	if (pTmp == NULL) goto err;
	capability->MaximumNumberOfProfiles = atoi(pTmp);
	
	mxmlDelete(pHead);
	return 0;
err:
	mxmlDelete(pHead);
	return -1;
}


static int _onvif_parse_device_info(const char *httpMessage, device_info_t *dev_info)
{
	const char *pTmp = NULL;
	const char *mxmlMessage = NULL;
	mxml_node_t *pHead = NULL, *pNode = NULL, *pNodeResp = NULL;

	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	pTmp = strstr(httpMessage, "\r\n\r\n");
	mxmlMessage = pTmp+ strlen("\r\n\r\n");

	pHead = mxmlLoadString(NULL, mxmlMessage, MXML_NO_CALLBACK);
	if (pHead == NULL )
	{
		printf("[%s %d]parse err\r\n", __FILE__, __LINE__);
		return -1;
	}

	if((pNodeResp = mxmlGuessElement(pHead, pHead, "tds:GetDeviceInformationResponse", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;

	if((pNode = mxmlGuessElement(pNodeResp, pNodeResp, "tds:Manufacturer", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	
	pTmp = mxmlGetText(pNode, 0);
	if (pTmp == NULL) goto err;
	sprintf(dev_info->Manifacture, pTmp);

	if((pNode = mxmlGuessElement(pNodeResp, pNodeResp, "tds:Model", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	pTmp = mxmlGetText(pNode, 0);
	if (pTmp == NULL) goto err;
	sprintf(dev_info->Model, pTmp);

	if((pNode = mxmlGuessElement(pNodeResp, pNodeResp, "tds:FirmwareVersion", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	pTmp = mxmlGetText(pNode, 0);
	if (pTmp == NULL) goto err;
	sprintf(dev_info->FirmwareVersion, pTmp);

	if((pNode = mxmlGuessElement(pNodeResp, pNodeResp, "tds:SerialNumber", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	pTmp = mxmlGetText(pNode, 0);
	if (pTmp == NULL) goto err;
	sprintf(dev_info->SerialNum, pTmp);

	mxmlDelete(pHead);
	return 0;
err:
	mxmlDelete(pHead);
	return -1;
}

static int _onvif_parse_reboot(const char *httpMessage)
{
	const char *pTmp = NULL;
	
	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;
	
	return 0;
}

static int _onvif_parse_image_options(const char *httpMessage, onvif_image_options_t *opt)
{
	const char *pTmp = NULL;
	const char *mxmlMessage = NULL;
	mxml_node_t *pHead = NULL, *pNode = NULL, *pNode1 = NULL,*pNode2 = NULL, *pNodeResp = NULL;

	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	pTmp = strstr(httpMessage, "\r\n\r\n");
	mxmlMessage = pTmp+ strlen("\r\n\r\n");

	//printf("%s\r\n", mxmlMessage);
	pHead = mxmlLoadString(NULL, mxmlMessage, MXML_NO_CALLBACK);
	if (pHead == NULL )
	{
		printf("[%s %d]parse err\r\n", __FILE__, __LINE__);
		return -1;
	}

	if((pNodeResp = mxmlGuessElement(pHead, pHead, "timg:GetOptionsResponse", NULL, NULL, MXML_DESCEND)) == NULL)
	{		
		printf("[%s %d]err\r\n", __FILE__, __LINE__);
		goto err;
	}
	if((pNode = mxmlGuessElement(pNodeResp, pNodeResp, "timg:ImagingOptions", NULL, NULL, MXML_DESCEND)) == NULL)
	{		
		printf("[%s %d]err\r\n", __FILE__, __LINE__);
		goto err;
	}
	
	////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:BacklightCompensation", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Mode", NULL, NULL, MXML_DESCEND)) == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;
		sprintf(opt->backlightCompensation, pTmp);
	}
	//////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:Brightness", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Min", NULL, NULL, MXML_DESCEND)) == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		opt->brightness.Min = atoi(pTmp);
		
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Max", NULL, NULL, MXML_DESCEND)) == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		opt->brightness.Max = atoi(pTmp);
		//printf("1Min = %d, Max = %d\r\n", opt->brightness.Min, opt->brightness.Max);
	}
	//////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:ColorSaturation", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Min", NULL, NULL, MXML_DESCEND)) == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		
		opt->colorSaturation.Min = atoi(pTmp);
		if((pNode1 = mxmlGuessElement(pNode1, pNode1, "tt:Max", NULL, NULL, MXML_DESCEND)) == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;
		opt->colorSaturation.Max = atoi(pTmp);
		
		//printf("2Min = %d, Max = %d\r\n", opt->colorSaturation.Min, opt->colorSaturation.Max);
	}
	
	//////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:Contrast", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Min", NULL, NULL, MXML_DESCEND)) == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		opt->contrast.Min = atoi(pTmp);
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Max", NULL, NULL, MXML_DESCEND)) == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;
		opt->contrast.Max = atoi(pTmp);
		
		//printf("3Min = %d, Max = %d\r\n", opt->contrast.Min, opt->contrast.Max);
	}

	////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:IrCutFilterModes", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		pTmp = mxmlGetText(pNode1, 0);
		if (pTmp == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		sprintf(opt->irCutFilterModes, pTmp);
	}
	
	//////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:Sharpness", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Min", NULL, NULL, MXML_DESCEND)) == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		opt->sharpness.Min = atoi(pTmp);
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Max", NULL, NULL, MXML_DESCEND)) == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL)
		{		
			printf("[%s %d]err\r\n", __FILE__, __LINE__);
			goto err;
		}
		opt->sharpness.Max = atoi(pTmp);
		
		//printf("4Min = %d, Max = %d\r\n", opt->sharpness.Min, opt->sharpness.Max);
	}

	mxmlDelete(pHead);
	return 0;
err:
	mxmlDelete(pHead);
	return -1;
}

static int _onvif_parse_get_image_settings(const char *httpMessage, onvif_image_settings_t *settings)
{
	const char *pTmp = NULL;
	const char *mxmlMessage = NULL;
	mxml_node_t *pHead = NULL, *pNode = NULL, *pNode1 = NULL, *pNode2 = NULL,*pNodeResp = NULL;

	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	pTmp = strstr(httpMessage, "\r\n\r\n");
	mxmlMessage = pTmp+ strlen("\r\n\r\n");

	//printf("%s\r\n", mxmlMessage);
	pHead = mxmlLoadString(NULL, mxmlMessage, MXML_NO_CALLBACK);
	if (pHead == NULL )
	{
		printf("[%s %d]parse err\r\n", __FILE__, __LINE__);
		return -1;
	}

	if((pNodeResp = mxmlGuessElement(pHead, pHead, "timg:GetImagingSettingsResponse", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	if((pNode = mxmlGuessElement(pNodeResp, pNodeResp, "timg:ImagingSettings", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	
	////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:BacklightCompensation", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Mode", NULL, NULL, MXML_DESCEND)) == NULL)
			goto err;
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;
		if (strcasecmp(pTmp, "ON") == 0)
			settings->backlightcompensation.Mode = BacklightCompensationMode__ON;
		else
			settings->backlightcompensation.Mode = BacklightCompensationMode__OFF;

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Level", NULL, NULL, MXML_DESCEND)) == NULL)
			goto err;
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;
		settings->backlightcompensation.Level =  atof(pTmp);
	}
	//////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:Brightness", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		pTmp = mxmlGetText(pNode1, 0);
		if (pTmp == NULL) goto err;
		settings->brightness = atof(pTmp);
	}
	//////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:ColorSaturation", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		pTmp = mxmlGetText(pNode1, 0);
		if (pTmp == NULL) goto err;
		settings->colorsaturation = atof(pTmp);
	}
	
	//////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:Contrast", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		pTmp = mxmlGetText(pNode1, 0);
		if (pTmp == NULL) goto err;
		settings->contrast = atof(pTmp);
	}

	////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:IrCutFilter", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		pTmp = mxmlGetText(pNode1, 0);		
		if (pTmp == NULL) goto err;		
		printf("@@@@@@@@@@IrCutFilterModes:%s\r\n", pTmp);
		if (strcasecmp(pTmp, "ON") == 0)
			settings->IrCutFilter = IrCutFilterMode__ON;
		else if (strcasecmp(pTmp, "OFF") == 0)
			settings->IrCutFilter = IrCutFilterMode__OFF;
		else if (strcasecmp(pTmp, "AUTO") == 0)	
			settings->IrCutFilter = IrCutFilterMode__AUTO;
	}
	
	//////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:Sharpness", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		pTmp = mxmlGetText(pNode1, 0);
		if (pTmp == NULL) goto err;
		settings->sharpness = atof(pTmp);
	}

	////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:WideDynamicRange", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Mode", NULL, NULL, MXML_DESCEND)) == NULL)
			goto err;
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;
		if (strcasecmp(pTmp, "ON") == 0)
			settings->widedynamicrange.Mode = WideDynamicMode__ON;
		else
			settings->widedynamicrange.Mode = WideDynamicMode__OFF;

	}

	////////////////////////////////////////////
	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:WhiteBalance", NULL, NULL, MXML_DESCEND)) != NULL)
	{
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Mode", NULL, NULL, MXML_DESCEND)) == NULL)
			goto err;
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;
		if (strcasecmp(pTmp, "AUTO") == 0)
			settings->whitebalance.Mode = WhiteBalanceMode__AUTO;
		else
			settings->whitebalance.Mode = WhiteBalanceMode__MANUAL;

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:CrGain", NULL, NULL, MXML_DESCEND)) == NULL)
			goto err;
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;
		settings->whitebalance.CrGain =  atof(pTmp);

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:CbGain", NULL, NULL, MXML_DESCEND)) == NULL)
			goto err;
		pTmp = mxmlGetText(pNode2, 0);
		if (pTmp == NULL) goto err;
		settings->whitebalance.CbGain =  atof(pTmp);
	}

	mxmlDelete(pHead);
	return 0;
err:
	mxmlDelete(pHead);
	return -1;
}

static int _onvif_parse_set_image_settings(const char *httpMessage)
{
	const char *pTmp = NULL;

	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	return 0;
}
static int _onvif_parse_venc(const char *httpMessage)
{
	const char *pTmp = NULL;

	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	return 0;
}

static int _onvif_parse_ptz_move(const char *httpMessage)
{
	const char *pTmp = NULL;
	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	return 0;
}

static int _onvif_parse_ptz_stop(const char *httpMessage)
{
	const char *pTmp = NULL;

	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	return 0;
}

static int _onvif_parse_get_venc_configuration_options(const char *httpMessage, onvif_venccfg_options_t *vencOptions)
{
	const char *pTmp = NULL;
	const char *mxmlMessage = NULL;
	mxml_node_t *pHead = NULL, *pNode = NULL, *pNode1 = NULL,
		*pNode2 = NULL, *pNode3 = NULL, *pNodeResp = NULL;

	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	pTmp = strstr(httpMessage, "\r\n\r\n");
	mxmlMessage = pTmp+ strlen("\r\n\r\n");

	pHead = mxmlLoadString(NULL, mxmlMessage, MXML_NO_CALLBACK);
	if (pHead == NULL )
	{
		printf("[%s %d]parse err\r\n", __FILE__, __LINE__);
		return -1;
	}

	if((pNodeResp = mxmlGuessElement(pHead, pHead, "trt:GetVideoEncoderConfigurationOptionsResponse", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;

	if((pNode = mxmlGuessElement(pNodeResp, pNodeResp, "trt:Options", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;

	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:QualityRange", NULL, NULL, MXML_DESCEND)) != NULL)
	{	
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Min", NULL, NULL, MXML_DESCEND)) == NULL)
			goto err;		
		pTmp = mxmlGetText(pNode2, 0);		
		if (pTmp == NULL) goto err;
		vencOptions->QualityRange.Min = atoi(pTmp);

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:Max", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;		
		pTmp = mxmlGetText(pNode2, 0);		
		if (pTmp == NULL) goto err;
		vencOptions->QualityRange.Max = atoi(pTmp);
	}
	else
		goto err;

	if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:H264", NULL, NULL, MXML_DESCEND)) != NULL)
	{	
		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:GovLengthRange", NULL, NULL, MXML_DESCEND)) != NULL)
		{
			if((pNode3 = mxmlGuessElement(pNode2, pNode2, "tt:Min", NULL, NULL, MXML_DESCEND)) == NULL)
				goto err;		
			pTmp = mxmlGetText(pNode2, 0);		
			if (pTmp == NULL) goto err;
			vencOptions->GovLengthRange.Min = atoi(pTmp);

			if((pNode3 = mxmlGuessElement(pNode2, pNode2, "tt:Max", NULL, NULL, MXML_DESCEND)) == NULL)
				goto err;		
			pTmp = mxmlGetText(pNode2, 0);		
			if (pTmp == NULL) goto err;
			vencOptions->GovLengthRange.Max = atoi(pTmp);
		}

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:FrameRateRange", NULL, NULL, MXML_DESCEND)) != NULL)
		{
			if((pNode3 = mxmlGuessElement(pNode2, pNode2, "tt:Min", NULL, NULL, MXML_DESCEND)) == NULL)
				goto err;		
			pTmp = mxmlGetText(pNode2, 0);		
			if (pTmp == NULL) goto err;
			vencOptions->FrameRateRange.Min = atoi(pTmp);

			if((pNode3 = mxmlGuessElement(pNode2, pNode2, "tt:Max", NULL, NULL, MXML_DESCEND)) == NULL)
				goto err;		
			pTmp = mxmlGetText(pNode2, 0);		
			if (pTmp == NULL) goto err;
			vencOptions->FrameRateRange.Max = atoi(pTmp);
		}

		if((pNode2 = mxmlGuessElement(pNode1, pNode1, "tt:EncodingIntervalRange", NULL, NULL, MXML_DESCEND)) != NULL)
		{
			if((pNode3 = mxmlGuessElement(pNode2, pNode2, "tt:Min", NULL, NULL, MXML_DESCEND)) == NULL)
				goto err;		
			pTmp = mxmlGetText(pNode2, 0);		
			if (pTmp == NULL) goto err;
			vencOptions->EncodingIntervalRange.Min = atoi(pTmp);

			if((pNode3 = mxmlGuessElement(pNode2, pNode2, "tt:Max", NULL, NULL, MXML_DESCEND)) == NULL)
				goto err;		
			pTmp = mxmlGetText(pNode2, 0);		
			if (pTmp == NULL) goto err;
			vencOptions->EncodingIntervalRange.Max = atoi(pTmp);
		}
		
	}
	

	mxmlDelete(pHead);
	return 0;
err:
	mxmlDelete(pHead);
	return -1;
}

static int _onvif_parse_get_presets(const char *httpMessage, onvif_ptzpresets_list_t *list_presets)
{
	const char *pTmp = NULL;
	const char *mxmlMessage = NULL;
	mxml_node_t *pHead = NULL, *pNode = NULL, *pNode1 = NULL, *pPresetsResp = NULL;
	Onvif_PTZPreset_t preset;

	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	pTmp = strstr(httpMessage, "\r\n\r\n");
	mxmlMessage = pTmp+ strlen("\r\n\r\n");
	pHead = mxmlLoadString(NULL, mxmlMessage, MXML_NO_CALLBACK);
	if (pHead == NULL )
	{
		printf("[%s %d]parse err\r\n", __FILE__, __LINE__);
		return -1;
	}
	
	_onvif_list_clear(onvif_ptzpresets_list_t, list_presets);
	//printf("==========begin preset=======\r\n");
	if((pPresetsResp = mxmlGuessElement(pHead, pHead, "tptz:GetPresetsResponse", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	
	pNode = pPresetsResp;
	do
	{
		if((pNode = mxmlGuessElement(pNode, pPresetsResp, "tptz:Preset", NULL, NULL, MXML_DESCEND)) != NULL)
		{
			pTmp = mxmlElementGetAttr(pNode, "token");
			if (pTmp == NULL) 
			{
				printf("[%s %d]err\r\n", __FILE__, __LINE__);
				goto err;
			}
			strcpy(preset.Presettoken, pTmp);

			if((pNode1 = mxmlGuessElement(pNode, pNode, "tt:Name", NULL, NULL, MXML_DESCEND)) == NULL)
			goto err;
			pTmp = mxmlGetText(pNode1, 0);
			if (pTmp == NULL) goto err;
			strcpy(preset.PresetName, pTmp);			
			_onvif_list_add(onvif_ptzpresets_list_t, list_presets, Onvif_PTZPreset_t, &preset);
			//printf("============add preset=======\r\n");
		}

		pNode = mxmlWalkNext(pNode, pPresetsResp, MXML_DESCEND);
		if (pNode == NULL)
			break;
		
	}while(1);
	//printf("==========end preset=======\r\n");
	return 0;
	
err:
	mxmlDelete(pHead);
	return -1;
}


static int _onvif_parse_set_presets(const char *httpMessage)
{
	const char *pTmp = NULL;

	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	return 0;
}


static int _onvif_parse_goto_presets(const char *httpMessage)
{
	const char *pTmp = NULL;

	printf("httpMessage = %s\r\n", httpMessage);
	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	return 0;
}

static int _onvif_parse_stream_url(const char *httpMessage, char *fullUrl)
{
	
	const char *pTmp = NULL;
	const char *mxmlMessage = NULL;

	mxml_node_t *pHead = NULL, *pNode = NULL;

	if (httpMessage == NULL || fullUrl == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	pTmp = strstr(httpMessage, "\r\n\r\n");
	mxmlMessage = pTmp+ strlen("\r\n\r\n");
	
	pHead = mxmlLoadString(NULL, mxmlMessage, MXML_NO_CALLBACK);
	if (pHead == NULL )
	{
		printf("[%s %d]parse err\r\n", __FILE__, __LINE__);
		return -1;
	}
	
	if((pNode = mxmlGuessElement(pHead, pHead, "tt:Uri", NULL, NULL, MXML_DESCEND)) == NULL)
		goto err;
	pTmp = mxmlGetText(pNode, 0);
	strcpy(fullUrl, pTmp);
	//printf("tt:Uri %s\r\n", pTmp);	

	mxmlDelete(pHead);
	return 0;
err:
	mxmlDelete(pHead);
	return -1;
		
}

static int _onvif_parse_set_time_settings(const char *httpMessage)
{
	const char *pTmp = NULL;

	if (httpMessage == NULL)
		return -1;

	if (httpMessage[0] != 'H' || httpMessage[1]!='T' || httpMessage[2]!='T' || httpMessage[3]!='P')
		return -1;
		
	pTmp = strstr(httpMessage, "200 OK");
	if (pTmp == NULL) return -1;

	return 0;
}



static int onvif_build_capability_request(char *pBuf, int nBufLen, const char *host)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];
	
	sprintf(acMsgBody, MSG_GET_CAPABILITY);
	bodyLen = strlen(acMsgBody);
	
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;
	len += sprintf(pBuf+len, acMsgBody);
	return 0;
}


static int onvif_build_profiles_request(char *pBuf, int nBufLen, const char *host)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];
	
	sprintf(acMsgBody, MSG_GET_PROFILES);
	bodyLen = strlen(acMsgBody);
	
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;
	len += sprintf(pBuf+len, acMsgBody);
	
	return 0;
	
}

static int onvif_build_profile_request(char *pBuf, int nBufLen, const char *host, const char *profileToken)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];

	sprintf(acMsgBody, MSG_GET_PROFILE, profileToken);
	bodyLen = strlen(acMsgBody);
	
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;
	len += sprintf(pBuf+len, acMsgBody);
	
	return 0;
	
}



static int onvif_build_scopes_request(char *pBuf, int nBufLen, const char *host)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];
	
	sprintf(acMsgBody, MSG_GET_SCOPES);
	bodyLen = strlen(acMsgBody);
	
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;
	len += sprintf(pBuf+len, acMsgBody);
	
	return 0;
	
}


static int onvif_build_device_info_request(char *pBuf, int nBufLen, const char *host)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];
	
	sprintf(acMsgBody, MSG_GET_DEVICE_INFO);
	bodyLen = strlen(acMsgBody);
	
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;	
	len += sprintf(pBuf+len,acMsgBody);
	
	return 0;	
}

static int onvif_build_reboot_request(char *pBuf, int nBufLen, const char *host)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];

	sprintf(acMsgBody, MSG_SYSTEM_REBOOT);
	bodyLen = strlen(acMsgBody);
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;	
	len += sprintf(pBuf+len,acMsgBody);
	
	return 0;	
}

static int onvif_build_image_options_request(char *pBuf, int nBufLen, const char *videosrcToken, const char *host)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];

	sprintf(acMsgBody, MSG_GET_IMAGE_OPTIONS, videosrcToken);
	bodyLen = strlen(acMsgBody);
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;	
	len += sprintf(pBuf+len,acMsgBody);
	
	return 0;	
}

static int onvif_build_get_image_settings_request(char *pBuf, int nBufLen, const char *videosrcToken, const char *host)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];

	sprintf(acMsgBody, MSG_GET_IMAGE_SETTINGS, videosrcToken);
	bodyLen = strlen(acMsgBody);
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;	
	len += sprintf(pBuf+len,acMsgBody);
	
	return 0;	
}

static int onvif_build_set_image_settings_request(char *pBuf, int nBufLen, const char *videosrcToken, onvif_image_settings_t *settings, const char *host)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];
	char *ircfMode[3] = {"ON", "OFF", "AUTO"};
	if (settings->IrCutFilter > IrCutFilterMode__OFF || settings->IrCutFilter < IrCutFilterMode__ON)
		settings->IrCutFilter = IrCutFilterMode__AUTO;
	sprintf(acMsgBody, MSG_SET_IMAGE_SETTINGS, videosrcToken,
			settings->brightness, settings->colorsaturation, 
			settings->contrast, ircfMode[settings->IrCutFilter],settings->sharpness);
	bodyLen = strlen(acMsgBody);
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;	
	len += sprintf(pBuf+len,acMsgBody);

	//printf("%s\r\n",pBuf);
	
	return 0;	
}


static int onvif_build_vconfig_request(char *pBuf, int nBufLen, const char *host, video_encode_cfg_t *cfg)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[4096];
	const char *strProfile[] = {"Baseline", "Main", "Extended", "High"};
	if (cfg->Profile > tt__H264Profile__High)
		cfg->Profile = tt__H264Profile__High;
	sprintf(acMsgBody, MSG_SET_VENC_CONFIGURATIONS, cfg->token, cfg->Name,
		cfg->Resolution.Width, cfg->Resolution.Height, cfg->Quality, cfg->FrameRate,
		cfg->Gop, cfg->Bitrate, 1, strProfile[cfg->Profile]);
	bodyLen = strlen(acMsgBody);
	
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;
	len += sprintf(pBuf+len, acMsgBody);

	//printf("%s\r\n", pBuf);
	return 0;	
}

static int onvif_build_get_venc_configurations_options_request(char *pBuf, int nBufLen, const char *host, const char *vencToken)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];
	
	sprintf(acMsgBody, MSG_GET_VENC_CONFIGURATION_OPTIONS, vencToken);
	bodyLen = strlen(acMsgBody);
	
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;
	len += sprintf(pBuf+len, acMsgBody);
	
	return 0;
}

static int onvif_build_get_ptzpresets_request(char *pBuf, int nBufLen, const char *host, const char *profileToken)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];
	
	sprintf(acMsgBody, MSG_GET_PRESETS, profileToken);
	bodyLen = strlen(acMsgBody);
	
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;
	len += sprintf(pBuf+len, acMsgBody);
	
	return 0;
}

static int onvif_build_set_ptzpresets_request(char *pBuf, int nBufLen, const char *host, const char *profileToken, const char *presetName)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];
	
	sprintf(acMsgBody, MSG_SET_PRESETS, profileToken, presetName);
	bodyLen = strlen(acMsgBody);
	
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;
	len += sprintf(pBuf+len, acMsgBody);
	
	return 0;
}

static int onvif_build_goto_ptzpresets_request(char *pBuf, int nBufLen, const char *host, const char *profileToken, const char *presetToken)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];
	
	sprintf(acMsgBody, MSG_GOTO_PRESETS, profileToken, presetToken);
	bodyLen = strlen(acMsgBody);
	
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;
	len += sprintf(pBuf+len, acMsgBody);
	
	return 0;
}

static int onvif_build_ptz_move_request(char *pBuf, int nBufLen, const char *host, const char *profileToken, float speed_x, float speed_y)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];
	
	sprintf(acMsgBody, MSG_PTZ_CONTINUOUS_MOVE, profileToken, speed_x, speed_y);
	bodyLen = strlen(acMsgBody);
	
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;
	len += sprintf(pBuf+len, acMsgBody);
	
	return 0;
}

static int onvif_build_ptz_stop_request(char *pBuf, int nBufLen, const char *host, const char *profileToken)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];
	
	sprintf(acMsgBody, MSG_PTZ_STOP, profileToken);
	bodyLen = strlen(acMsgBody);
	
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;
	len += sprintf(pBuf+len, acMsgBody);
	
	return 0;
}

static int onvif_build_stream_url_request(char *pBuf, int nBufLen, const char *host, const char *profileToken)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];
	
	sprintf(acMsgBody, MSG_GET_STREAM_URL, "UDP", profileToken);
	bodyLen = strlen(acMsgBody);
	
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;	
	len += sprintf(pBuf+len,acMsgBody);
	
	return 0;	
}

static int onvif_build_set_time_settings_request(char *pBuf, int nBufLen, 
								int zone,
								int year, int mon, int day,
								int hour, int min, int sec,
								const char *host)
{
	int bodyLen = 0;
	int len = 0;
	char acMsgBody[2048];
	sprintf(acMsgBody, MSG_TIME_SET, zone>0?"UTC+":"UTC", zone, hour, min, sec, year, mon, day);
	bodyLen = strlen(acMsgBody);
	len += sprintf(pBuf+len, "POST /onvif/device_service HTTP/1.1\r\n");
	len += sprintf(pBuf+len, "Host: %s\r\n", host);
	len += sprintf(pBuf+len, "Content-Type: application/soap+xml; charset=utf-8\r\n");	
	len += sprintf(pBuf+len, "Content-Length: %d\r\n\r\n", bodyLen);
	if (len + bodyLen >= nBufLen) 
		return -1;	
	len += sprintf(pBuf+len,acMsgBody);

	//printf("%s\r\n",pBuf);
	
	return 0;	
}



int nvr_onvif_get_profiles(const char *host, char *port, onvif_profile_list_t *list_profile)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char *pRecvBuf = NULL;

	if (host == NULL || port == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_profiles_request(acMsg, sizeof(acMsg), host);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	
	net_set_sock_attr(nSocket, 0, 0, 0, 100*1024, 100*1024);
	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	pRecvBuf = (char *)malloc(100*1024);
	memset(pRecvBuf, 0, 100*1024);

	ret = _http_recv(nSocket, pRecvBuf, 100*1024); 
	if (ret < 0)	//socket has closeed
		return -1;

	net_close_socket(&nSocket);
	ret = _onvif_parse_profiles(pRecvBuf, list_profile);
	if (ret < 0) return -1;

	free(pRecvBuf);
	
	return 0;
}

int nvr_onvif_get_profile(const char *host, char *port, const char *profileToken, profile_data_t *data)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char *pRecvBuf = NULL;

	if (host == NULL || port == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_profile_request(acMsg, sizeof(acMsg), host, profileToken);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}
	
	net_set_sock_attr(nSocket, 0, 0, 0, 100*1024, 100*1024);
	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	pRecvBuf = (char *)malloc(100*1024);
	memset(pRecvBuf, 0, 100*1024);

	ret = _http_recv(nSocket, pRecvBuf, 100*1024); 
	if (ret < 0)	//socket has closeed
		return -1;

	net_close_socket(&nSocket);
	ret = _onvif_parse_profile(pRecvBuf, data);
	if (ret < 0) return -1;

	free(pRecvBuf);
	
	return 0;
}


int nvr_onvif_get_scopes(const char *host, char *port, onvif_scopes_list_t *list_scopes)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char acRecvBuf[ONVIF_MAX_MSG_LEN] = {0};

	if (host == NULL || port == NULL)
		return -1;
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_scopes_request(acMsg, sizeof(acMsg), host);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = _http_recv(nSocket, acRecvBuf, sizeof(acRecvBuf)); 
	if (ret < 0)	//socket has closeed
		return -1;
	net_close_socket(&nSocket);
	ret = _onvif_parse_scopes(acRecvBuf, list_scopes);
	if (ret < 0) return -1;
	
	return 0;
}



int onvif_set_video_encoder_configurations(const char *host, char *port, video_encode_cfg_t *cfg)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char acRecvBuf[ONVIF_MAX_MSG_LEN] = {0};

	if (host == NULL || port == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_vconfig_request(acMsg, sizeof(acMsg), host, cfg);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = _http_recv(nSocket, acRecvBuf, sizeof(acRecvBuf)); 
	if (ret < 0)	//socket has closeed
		return -1;
	net_close_socket(&nSocket);
	ret = _onvif_parse_venc(acRecvBuf);
	if (ret < 0) return -1;
	
	return 0;
}

int nvr_onvif_get_video_encode_configuration_options(const char *host , char *port, const char *videosrcToken ,onvif_venccfg_options_t *vencOptions)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char *pRecvBuf = NULL;

	if (host == NULL || port == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_get_venc_configurations_options_request(acMsg, sizeof(acMsg), host, videosrcToken);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	
	net_set_sock_attr(nSocket, 0, 0, 0, 100*1024, 100*1024);
	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	pRecvBuf = (char *)malloc(100*1024);
	memset(pRecvBuf, 0, 100*1024);

	ret = _http_recv(nSocket, pRecvBuf, 100*1024); 
	if (ret < 0)	//socket has closeed
		return -1;

	net_close_socket(&nSocket);
	ret = _onvif_parse_get_venc_configuration_options(pRecvBuf, vencOptions);
	if (ret < 0) return -1;

	free(pRecvBuf);
	
	return 0;
}


int nvr_onvif_get_presets(const char *host, char *port, const char *profileToken, onvif_ptzpresets_list_t *presets)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char *pRecvBuf = NULL;

	if (host == NULL || port == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_get_ptzpresets_request(acMsg, sizeof(acMsg), host, profileToken);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	
	net_set_sock_attr(nSocket, 0, 0, 0, 100*1024, 100*1024);
	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	pRecvBuf = (char *)malloc(100*1024);
	memset(pRecvBuf, 0, 100*1024);

	ret = _http_recv(nSocket, pRecvBuf, 100*1024); 
	if (ret < 0)	//socket has closeed
		return -1;

	net_close_socket(&nSocket);
	ret = _onvif_parse_get_presets(pRecvBuf, presets);
	if (ret < 0) return -1;

	free(pRecvBuf);
	
	return 0;
}

int nvr_onvif_set_presets(const char *host, char *port, const char *profileToken, const char *presetName)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char *pRecvBuf = NULL;

	if (host == NULL || port == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_set_ptzpresets_request(acMsg, sizeof(acMsg), host, profileToken, presetName);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}
	
	net_set_sock_attr(nSocket, 0, 0, 0, 100*1024, 100*1024);
	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	pRecvBuf = (char *)malloc(100*1024);
	memset(pRecvBuf, 0, 100*1024);

	ret = _http_recv(nSocket, pRecvBuf, 100*1024); 
	if (ret < 0)	//socket has closeed
		return -1;

	net_close_socket(&nSocket);
	ret = _onvif_parse_set_presets(pRecvBuf);
	if (ret < 0) return -1;

	free(pRecvBuf);
	
	return 0;
}


int nvr_onvif_goto_presets(const char *host, char *port, const char *profileToken, const char *prosetsToken)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char *pRecvBuf = NULL;

	if (host == NULL || port == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_goto_ptzpresets_request(acMsg, sizeof(acMsg), host, profileToken, prosetsToken);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}
	
	net_set_sock_attr(nSocket, 0, 0, 0, 100*1024, 100*1024);
	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	pRecvBuf = (char *)malloc(100*1024);
	memset(pRecvBuf, 0, 100*1024);

	ret = _http_recv(nSocket, pRecvBuf, 100*1024); 
	if (ret < 0)	//socket has closeed
		return -1;

	net_close_socket(&nSocket);
	ret = _onvif_parse_goto_presets(pRecvBuf);
	if (ret < 0) return -1;

	free(pRecvBuf);
	
	return 0;
}

int nvr_onvif_ptz_continuous_move(const char *host, char *port, const char *profileToken, float speed_x, float speed_y)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char acRecvBuf[ONVIF_MAX_MSG_LEN] = {0};

	if (host == NULL || port == NULL || profileToken == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_ptz_move_request(acMsg, sizeof(acMsg), host, profileToken, speed_x, speed_y);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}
	ret = _http_recv(nSocket, acRecvBuf, sizeof(acRecvBuf)); 
	if (ret < 0)	//socket has closeed
		return -1;
	
	net_close_socket(&nSocket);
	//printf("%s\r\n", acRecvBuf);
	ret = _onvif_parse_ptz_move(acRecvBuf);
	if (ret < 0) return -1;
	
	return 0;
}

int nvr_onvif_ptz_stop(const char *host, char *port, const char *profileToken)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char acRecvBuf[ONVIF_MAX_MSG_LEN] = {0};

	if (host == NULL || port == NULL || profileToken == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_ptz_stop_request(acMsg, sizeof(acMsg), host, profileToken);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}
	ret = _http_recv(nSocket, acRecvBuf, sizeof(acRecvBuf)); 
	if (ret < 0)	//socket has closeed
		return -1;
	
	net_close_socket(&nSocket);
	//printf("%s\r\n", acRecvBuf);
	ret = _onvif_parse_ptz_stop(acRecvBuf);
	if (ret < 0) return -1;
	
	return 0;
}

int nvr_onvif_get_stream_url(const char *host, const char *port, const char *profileToken, char *fullUrl)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char acRecvBuf[ONVIF_MAX_MSG_LEN] = {0};

	if (host == NULL || port == NULL || fullUrl == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_stream_url_request(acMsg, sizeof(acMsg), host, profileToken);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}
	ret = _http_recv(nSocket, acRecvBuf, sizeof(acRecvBuf)); 
	if (ret < 0)	//socket has closeed
		return -1;
	
	net_close_socket(&nSocket);
	//printf("%s\r\n", acRecvBuf);
	ret = _onvif_parse_stream_url(acRecvBuf, fullUrl);
	if (ret < 0) return -1;
	
	return 0;
}



int nvr_onvif_discover(int timeout, fSearchCb cbSearchFunc)
{	
	int ret;
	int nSocket = 0;
	int index = 0;
	int nLen = sizeof(struct sockaddr_in);
	struct sockaddr_in multi_addr;
	char acRecvbuf[ONVIF_MAX_MSG_LEN];
	struct timespec tp;
	time_t curtime;
	
	nSocket = net_create_sock(0);
	memset(&multi_addr, 0, sizeof(struct sockaddr_in));
	multi_addr.sin_family = AF_INET;
	multi_addr.sin_port = htons(3702);
	if (inet_pton(AF_INET, "239.255.255.250", &multi_addr.sin_addr) <= 0)
	{
		net_close_socket(&nSocket);
		return -1;
	}
	
	ret = net_udp_send(nSocket, PROBE_MSG, strlen(PROBE_MSG),(struct sockaddr *)&multi_addr);
	if (ret < 0)
	{
		net_close_socket(&nSocket);
		return -1;
	}
	
	clock_gettime(CLOCK_MONOTONIC, &tp);
	curtime = tp.tv_sec;
	do
	{	
		clock_gettime(CLOCK_MONOTONIC, &tp);
		if (tp.tv_sec - curtime >= timeout)
			break;
		
		if((ret = net_select(&nSocket, 1, 0x1, 1000)) < 0)
			break;
		else if (ret == 0) 
			continue;

		memset(acRecvbuf, 0, sizeof(acRecvbuf));
		ret = net_udp_recv(nSocket, acRecvbuf, sizeof(acRecvbuf), 0, (struct sockaddr *)&multi_addr, &nLen);
		if (ret < 0) break;	
		
		_onvif_discover_message_parse(index, acRecvbuf, cbSearchFunc);		
		index++;
	}while(1);
	net_close_socket(&nSocket);	
	return 0;
}


int nvr_onvif_get_capability(const char *host, char *port, device_capability_t *capability)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char acRecvBuf[ONVIF_MAX_MSG_LEN] = {0};

	if (host == NULL || port == NULL || capability == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_capability_request(acMsg, sizeof(acMsg), host);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = _http_recv(nSocket, acRecvBuf, sizeof(acRecvBuf)); 
	if (ret < 0)	//socket has closeed
		return -1;
	net_close_socket(&nSocket);
	ret = _onvif_parse_capibility(acRecvBuf, capability);
	if (ret < 0) return -1;
	return 0;
	
}

int nvr_onvif_get_device_info(const char *host, char *port, device_info_t *dev_info)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char acRecvBuf[ONVIF_MAX_MSG_LEN] = {0};

	if (host == NULL || port == NULL || dev_info == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_device_info_request(acMsg, sizeof(acMsg), host);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}
	ret = _http_recv(nSocket, acRecvBuf, sizeof(acRecvBuf)); 
	if (ret < 0)	//socket has closeed
		return -1;
	
	net_close_socket(&nSocket);
	ret = _onvif_parse_device_info(acRecvBuf, dev_info);
	if (ret < 0) return -1;
	return 0;
}

int nvr_onvif_get_network_interface(const char *host, char *port, network_interface_t *network)
{
	return -1;
}

int nvr_onvif_system_reboot(const char *host, char *port)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char acRecvBuf[ONVIF_MAX_MSG_LEN] = {0};

	if (host == NULL || port == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_reboot_request(acMsg, sizeof(acMsg), host);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = _http_recv(nSocket, acRecvBuf, sizeof(acRecvBuf)); 
	if (ret < 0)	//socket has closeed
		return -1;
	net_close_socket(&nSocket);
	ret = _onvif_parse_reboot(acRecvBuf);
	if (ret < 0) return -1;
	return 0;
}


int nvr_onvif_get_image_options(const char *host , char *port, const char *videosrcToken ,onvif_image_options_t *options)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char acRecvBuf[ONVIF_MAX_MSG_LEN] = {0};

	if (host == NULL || port == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_image_options_request(acMsg, sizeof(acMsg), videosrcToken, host);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}
	net_set_sock_attr(nSocket, 0, 0, 0, 40000, 40000);
	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = _http_recv(nSocket, acRecvBuf, sizeof(acRecvBuf)); 
	if (ret < 0)	//socket has closeed
		return -1;
	
	net_close_socket(&nSocket);
	ret = _onvif_parse_image_options(acRecvBuf, options);
	if (ret < 0) return -1;
	return 0;
}



int nvr_onvif_get_image_settings(const char *host , char *port, const char *videosrcToken ,onvif_image_settings_t *settings)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char acRecvBuf[ONVIF_MAX_MSG_LEN] = {0};

	if (host == NULL || port == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_get_image_settings_request(acMsg, sizeof(acMsg), videosrcToken, host);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}
	net_set_sock_attr(nSocket, 0, 0, 0, 40000, 40000);
	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = _http_recv(nSocket, acRecvBuf, sizeof(acRecvBuf)); 
	if (ret < 0)	//socket has closeed
		return -1;
	
	net_close_socket(&nSocket);
	ret = _onvif_parse_get_image_settings(acRecvBuf, settings);
	if (ret < 0) return -1;
	return 0;
}

int nvr_onvif_set_image_settings(const char *host , char *port, const char *videosrcToken ,onvif_image_settings_t *settings)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char acRecvBuf[ONVIF_MAX_MSG_LEN] = {0};

	if (host == NULL || port == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_set_image_settings_request(acMsg, sizeof(acMsg), videosrcToken, settings, host);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}
	ret = _http_recv(nSocket, acRecvBuf, sizeof(acRecvBuf)); 
	if (ret < 0)	//socket has closeed
		return -1;
	net_close_socket(&nSocket);
	ret = _onvif_parse_set_image_settings(acRecvBuf);
	if (ret < 0) return -1;
	return 0;
}

int nvr_onvif_set_time_settings(const char *host , char *port ,int zone, int year, int mon, 
										int day, int hour, int min, int sec)
{
	int ret;
	int nSocket = 0;
	char acMsg[4096];
	char acRecvBuf[ONVIF_MAX_MSG_LEN] = {0};
	if (host == NULL || port == NULL)
		return -1;
	
	if((nSocket = net_tcp_noblock_connect(NULL, NULL, host, port, 3000)) <= 0)
		return -1;

	ret = onvif_build_set_time_settings_request(acMsg, sizeof(acMsg), zone, year, mon, day, 
														hour, min, sec, host);
	if (ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}

	ret = net_tcp_noblock_send(nSocket, acMsg, strlen(acMsg), 0, 3000);
	if(ret < 0) 
	{
		net_close_socket(&nSocket);
		return -1;
	}
	ret = _http_recv(nSocket, acRecvBuf, sizeof(acRecvBuf)); 
	if (ret < 0)	//socket has closeed
		return -1;
	net_close_socket(&nSocket);
	ret = _onvif_parse_set_time_settings(acRecvBuf);
	if (ret < 0) return -1;
	return 0;
}


