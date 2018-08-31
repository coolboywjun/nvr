
#include <string.h>
#include <arpa/inet.h>

#include "webs.h"
#include "nvr_lib_interface.h"
#include "nvr_web_proc.h"
#include "nvr_web_server.h"
#include "nvr_xml_cfg.h"

#define WEB_FRM "document.outcfg_frm"
#define WEB_TIME_FRM "outcfg_frm_mu"
#define WEB_NTP_FRM "outcfg_frm_ntp"

static int 	sDevType = 3;//暂定1：ipc，2：dvr，3：nvr

#define JXJ_RTSP_PORT	7554
#define JXJ_SDK_PORT    3321

//页面获取登陆配置接口
int  getLoginInfo(int eid, webs_t wp, int argc, char_t **argv)
{

	//////////////////////////////// 需要修改port
	__DBUG("getLoginInfo...\n");
	__DBUG("JXJ_RTSP_PORT:%d, JXJ_SDK_PORT:%d\n", JXJ_RTSP_PORT, JXJ_SDK_PORT);
	websWrite(wp,(char_t *)"<script>");
	websWrite(wp, "%s.streamPort.value ='%d';\n", WEB_FRM, JXJ_RTSP_PORT);
	websWrite(wp, "%s.dataPort.value ='%d';\n", WEB_FRM, JXJ_SDK_PORT);
	websWrite(wp, "%s.devType.value ='%d';\n", WEB_FRM, sDevType);
	websWrite(wp,(char_t *)"</script>");

	return 0;
}

//页面设置登陆配置接口
void loginInfo(webs_t wp, char_t *path, char_t *query)
{
	__DBUG("loginInfo...\n");

	__DBUG("query: %s\n", query);
	NVR_DEV_USER_CFG_S stUsr;
	
	int i = 0;
	int level = -1;
	
	char szUsrName[NVR_USER_NAME_LEN] = {0};
	char szPsw[NVR_PASS_WORD_LEN] = {0};
	
	NVR_LANGUAGE_TYPE_E eLang = atoi(websGetVar(wp, (char_t *)T("lang"),(char_t *)T("1")));

	memset(&stUsr, 0, sizeof(NVR_DEV_USER_CFG_S));

	strcpy(szUsrName, websGetVar(wp, (char_t *)T("userName"),(char_t *)T("0")));
	strcpy(szPsw, websGetVar(wp, (char_t *)T("password"),(char_t *)T("0")));

	__DBUG("szUsrName:%s %d\n", szUsrName, strlen(szUsrName));
	__DBUG("szPsw:%s %d\n", szPsw, strlen(szPsw));
	
	for (i = 0; i < NVR_MAX_USR_NUM; ++i)
	{
		nvr_logon_get_user_cfg(i, &stUsr);
	
		if (strcmp(stUsr.szUsrName, szUsrName) == 0
			&& strcmp(stUsr.szPsw, szPsw) == 0)
		{ 
			level = i;
			break;
		}
	}
	
	__DBUG("level:%d\n", level);
	if (level != -1)
	{
		websRedirect(wp, (char_t *)T("main.asp"));
		return ;
	}
	
	if(eLang == NVR_LANGUAGE_CHS)
		websRedirect(wp, (char_t *)T("error.asp?lang=1"));
	else 
		websRedirect(wp, (char_t *)T("error.asp?lang=0"));
	
	return ; 
}

int  getMainInfo(int eid, webs_t wp, int argc, char_t **argv)
{
	__DBUG("loginInfo...\n");
	__DBUG("NVR_MAX_CHANNEL_NUM:%d\n", NVR_MAX_CHANNEL_NUM);
	websWrite(wp,(char_t *)"<script>");
	websWrite(wp, "%s.chnCount.value ='%d';\n", WEB_FRM, NVR_MAX_CHANNEL_NUM);
	websWrite(wp, "%s.devType.value ='%d';\n", WEB_FRM, sDevType);
	websWrite(wp,(char_t *)"</script>");

	return 0;
}


