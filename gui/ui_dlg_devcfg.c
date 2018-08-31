#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>
#include <minigui/skin.h>

#include "ui_config.h"
#include "myedit.h"
#include "ctrl_ipaddress.h"
#include "nvr_lib_interface.h"



#include "gui_core.h"
#include "ui_dlg_head.h"

#define UI_DLG_DEVCFG_W		700
#define UI_DLG_DEVCFG_H		500

#define UI_DLG_DEVCFG_X	((NVR_SCREEN_WIDTH-UI_DLG_DEVCFG_W)/2)	
#define UI_DLG_DEVCFG_Y	((NVR_SCREEN_HEIGHT-UI_DLG_DEVCFG_H)/2)
#define UI_DEVCFG_TEXT_START_X	50
#define UI_DEVCFG_TEXT_START_Y	60
#define UI_DECFG_EDIT_H	20
#define UI_DEVCFG_INC_Y		(UI_DECFG_EDIT_H + 12)
#define UI_DEVCFG_STATIC_W      100


extern char *GetProtoclaAliasName(char *protocalName);
extern char *GetProtoclaName(char *AliasName);

static BITMAP  bmp_bkgnd;
static RECT  dev_config_title_rect = {0,0,UI_DLG_DEVCFG_W,50};



typedef enum
{
	IDC_STATIC_CHNUM = 1700,
	IDL_CHNUM,
	IDC_STATIC_MANUFACTURE,
	IDL_MANUFACTURE,
	IDC_STATIC_DEV_IPADDR,
	IDC_DEV_IPADDR,
	IDC_STATIC_DEV_USERNAME,
	IDC_DEV_USERNAME,
	IDC_STATIC_DEV_DATAPORT,
	IDC_DEV_DATAPORT,
	IDC_STATIC_DEV_PASS,
	IDC_DEV_PASS,
	IDC_STATIC_DEV_HTTPPORT,
	IDC_DEV_HTTPPORT,
	IDC_STATIC_REMOTECH,
	IDC_REMOTECH,
	IDC_STATIC_DEV_RTSPPORT,
	IDC_DEV_RTSPPORT,
	IDC_STATIC_TRANSFER_PROTOCAL,
	IDL_TRANSFER_PROTOCAL,
	IDC_DEVCFG_SAVE,
	IDC_DEVCFG_CANCEL,
	IDC_SEARCH_AAS,
	IDC_ENABLE_CONNECT,
}UI_DEV_SETTING_ID;


static CTRLDATA ui_devcfg_control[] =
{
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
         UI_DEVCFG_TEXT_START_X, UI_DEVCFG_TEXT_START_Y, UI_DEVCFG_STATIC_W, UI_DECFG_EDIT_H,
        IDC_STATIC_CHNUM,
        "通道号",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP | CBS_NOTIFY,
        UI_DEVCFG_TEXT_START_X + UI_DEVCFG_STATIC_W,  UI_DEVCFG_TEXT_START_Y, 160, GUI_COMBOX_H,
        IDL_CHNUM,
        "",
        0,
        WS_EX_USEPARENTFONT
    },
    {
        GUI_BUTTON,
		WS_VISIBLE | BS_AUTOCHECKBOX, 
		UI_DEVCFG_TEXT_START_X + UI_DEVCFG_STATIC_W + 160+80,  UI_DEVCFG_TEXT_START_Y, 110+10, UI_DECFG_EDIT_H,
        IDC_ENABLE_CONNECT,
        "启用连接",
        0,
		WS_EX_TRANSPARENT
    },
    ///////////////////////////////////////////////////////////////////
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		UI_DEVCFG_TEXT_START_X ,  UI_DEVCFG_TEXT_START_Y+UI_DEVCFG_INC_Y*1, UI_DEVCFG_STATIC_W, UI_DECFG_EDIT_H,
        IDC_STATIC_DEV_IPADDR,
        "IP地址",
        0,
		WS_EX_TRANSPARENT
    },
    {
        GUI_BUTTON,
		WS_VISIBLE | BS_AUTOCHECKBOX, 
		UI_DEVCFG_TEXT_START_X + UI_DEVCFG_STATIC_W + 160+80,  UI_DEVCFG_TEXT_START_Y+UI_DEVCFG_INC_Y*1, 110+10, UI_DECFG_EDIT_H,
        IDC_SEARCH_AAS,
        "智能增加",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_IP_ADDRESS,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		UI_DEVCFG_TEXT_START_X + UI_DEVCFG_STATIC_W,  UI_DEVCFG_TEXT_START_Y +UI_DEVCFG_INC_Y*1, 160, UI_DECFG_EDIT_H,
        IDC_DEV_IPADDR,
        "192.168.1.1",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT	
    },
    ///////////////////////////////////////////////////////////////////////
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		UI_DEVCFG_TEXT_START_X ,  UI_DEVCFG_TEXT_START_Y+UI_DEVCFG_INC_Y*2, 
		UI_DEVCFG_STATIC_W, UI_DECFG_EDIT_H,
        IDC_STATIC_DEV_USERNAME,
        "用户名",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		UI_DEVCFG_TEXT_START_X + UI_DEVCFG_STATIC_W ,  UI_DEVCFG_TEXT_START_Y+UI_DEVCFG_INC_Y*2, 
		160, UI_DECFG_EDIT_H,
        IDC_DEV_USERNAME,
        "admin",
        0,        
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT| WS_EX_USEPARENTFONT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		UI_DEVCFG_TEXT_START_X + UI_DEVCFG_STATIC_W + 160+80 , UI_DEVCFG_TEXT_START_Y+UI_DEVCFG_INC_Y*2, 
		UI_DEVCFG_STATIC_W, UI_DECFG_EDIT_H,
        IDC_STATIC_DEV_DATAPORT,
        "数据端口",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		UI_DEVCFG_TEXT_START_X + 2*UI_DEVCFG_STATIC_W + 160+80, UI_DEVCFG_TEXT_START_Y+UI_DEVCFG_INC_Y*2, 
		160, UI_DECFG_EDIT_H,
        IDC_DEV_DATAPORT,
        "6080",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT	
    },
    /////////////////////////////////////////////////////////////
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		UI_DEVCFG_TEXT_START_X ,  UI_DEVCFG_TEXT_START_Y+UI_DEVCFG_INC_Y*3,		
		UI_DEVCFG_STATIC_W, UI_DECFG_EDIT_H,
        IDC_STATIC_DEV_PASS,
        "密码",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP|ES_PASSWORD,
		UI_DEVCFG_TEXT_START_X + UI_DEVCFG_STATIC_W , UI_DEVCFG_TEXT_START_Y+UI_DEVCFG_INC_Y*3, 
		160, UI_DECFG_EDIT_H,
        IDC_DEV_PASS,
        "admin",
        0,        
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT | WS_EX_USEPARENTFONT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		UI_DEVCFG_TEXT_START_X + UI_DEVCFG_STATIC_W + 160+80 , UI_DEVCFG_TEXT_START_Y+UI_DEVCFG_INC_Y*3, 
		UI_DEVCFG_STATIC_W, UI_DECFG_EDIT_H,
        IDC_STATIC_DEV_RTSPPORT,
        "RTSP端口",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		UI_DEVCFG_TEXT_START_X + 2*UI_DEVCFG_STATIC_W + 160+80, UI_DEVCFG_TEXT_START_Y+UI_DEVCFG_INC_Y*3, 
		160, UI_DECFG_EDIT_H,
        IDC_DEV_RTSPPORT,
        "554",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT	
    },
	/////////////////////////////////////////////////////
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		UI_DEVCFG_TEXT_START_X ,  UI_DEVCFG_TEXT_START_Y+UI_DEVCFG_INC_Y*4, UI_DEVCFG_STATIC_W, UI_DECFG_EDIT_H,
        IDC_STATIC_REMOTECH,
        "远程通道号",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		UI_DEVCFG_TEXT_START_X + UI_DEVCFG_STATIC_W,  UI_DEVCFG_TEXT_START_Y +UI_DEVCFG_INC_Y*4, 160, UI_DECFG_EDIT_H,
        IDC_REMOTECH,
        "1",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT	
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		UI_DEVCFG_TEXT_START_X + UI_DEVCFG_STATIC_W + 160+80 , UI_DEVCFG_TEXT_START_Y+UI_DEVCFG_INC_Y*4, 
		UI_DEVCFG_STATIC_W, UI_DECFG_EDIT_H,
        IDC_STATIC_DEV_HTTPPORT,
        "HTTP端口",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		UI_DEVCFG_TEXT_START_X + 2*UI_DEVCFG_STATIC_W + 160+80, UI_DEVCFG_TEXT_START_Y+UI_DEVCFG_INC_Y*4, 
		160, UI_DECFG_EDIT_H,
        IDC_DEV_HTTPPORT,
        "80",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT	
    },
    /////////////////////////////////////////////////////////////////
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		UI_DEVCFG_TEXT_START_X ,  UI_DEVCFG_TEXT_START_Y+UI_DEVCFG_INC_Y*5, UI_DEVCFG_STATIC_W, UI_DECFG_EDIT_H,
        IDC_STATIC_TRANSFER_PROTOCAL,
        "传输协议",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP,
		UI_DEVCFG_TEXT_START_X + UI_DEVCFG_STATIC_W,  UI_DEVCFG_TEXT_START_Y +UI_DEVCFG_INC_Y*5, 160, GUI_COMBOX_H,
        IDL_TRANSFER_PROTOCAL,
        "",
        0,
        WS_EX_USEPARENTFONT
    },
	//////////////////////////// GUI_BUTTON /////////////////////////////////////
	CONFIG_THREE_WORDS_BUTTON("保存", IDC_DEVCFG_SAVE, UI_DLG_DEVCFG_W - 175 , UI_DLG_DEVCFG_H - 50),
	CONFIG_THREE_WORDS_BUTTON("取消", IDC_DEVCFG_CANCEL, UI_DLG_DEVCFG_W - 100 , UI_DLG_DEVCFG_H - 50),

};


static DLGTEMPLATE DevCfgDlg =
{
    WS_NONE,
    WS_EX_NONE,
    UI_DLG_DEVCFG_X, UI_DLG_DEVCFG_Y, UI_DLG_DEVCFG_W, UI_DLG_DEVCFG_H,
    "远程设备配置",
    0, 0,
    TABLESIZE(ui_devcfg_control), NULL,
    0
};

static void CheckSearchProc(HWND hWnd)
{
	int checkstat;
	checkstat = SendDlgItemMessage(hWnd, IDC_SEARCH_AAS, BM_GETCHECK, 0, 0);
	if (checkstat == BST_CHECKED)
	{
		EnableDlgItem(hWnd ,IDC_DEV_IPADDR, FALSE);
		EnableDlgItem(hWnd ,IDC_DEV_USERNAME, FALSE);
		EnableDlgItem(hWnd ,IDC_DEV_PASS, FALSE);
		EnableDlgItem(hWnd ,IDC_DEV_DATAPORT, FALSE);
		EnableDlgItem(hWnd ,IDC_DEV_HTTPPORT, FALSE);
		EnableDlgItem(hWnd ,IDC_DEV_RTSPPORT, FALSE);
		EnableDlgItem(hWnd ,IDC_REMOTECH, FALSE);
		//EnableDlgItem(hWnd ,IDL_CHNUM, FALSE);
	#if 0	
		EnableDlgItem(hWnd ,IDL_MANUFACTURE, FALSE);
	#endif
		EnableDlgItem(hWnd ,IDL_TRANSFER_PROTOCAL, FALSE);
	}
	else
	{
		EnableDlgItem(hWnd ,IDC_DEV_IPADDR, TRUE);
		EnableDlgItem(hWnd ,IDC_DEV_USERNAME, TRUE);
		EnableDlgItem(hWnd ,IDC_DEV_PASS, TRUE);
		EnableDlgItem(hWnd ,IDC_DEV_DATAPORT, TRUE);
		EnableDlgItem(hWnd ,IDC_DEV_HTTPPORT, TRUE);
		EnableDlgItem(hWnd ,IDC_DEV_RTSPPORT, TRUE);
		EnableDlgItem(hWnd ,IDC_REMOTECH, TRUE);
		//EnableDlgItem(hWnd ,IDL_CHNUM, TRUE);
#if 0			
		EnableDlgItem(hWnd ,IDL_MANUFACTURE, TRUE);	
#endif
		EnableDlgItem(hWnd ,IDL_TRANSFER_PROTOCAL, TRUE);
	}
}

static void CheckEnableConnectProc(HWND hWnd)
{
	int checkstat;
	checkstat = SendDlgItemMessage(hWnd, IDC_ENABLE_CONNECT, BM_GETCHECK, 0, 0);
	if (checkstat == BST_CHECKED)
	{
		EnableDlgItem(hWnd ,IDC_SEARCH_AAS, TRUE);
		if(SendDlgItemMessage(hWnd, IDC_SEARCH_AAS, BM_GETCHECK, 0 , 0) == BST_CHECKED)
		{
			EnableDlgItem(hWnd ,IDC_DEV_IPADDR, FALSE);
			EnableDlgItem(hWnd ,IDC_DEV_USERNAME, FALSE);
			EnableDlgItem(hWnd ,IDC_DEV_PASS, FALSE);
			EnableDlgItem(hWnd ,IDC_DEV_DATAPORT, FALSE);
			EnableDlgItem(hWnd ,IDC_DEV_HTTPPORT, FALSE);
			EnableDlgItem(hWnd ,IDC_DEV_RTSPPORT, FALSE);
			EnableDlgItem(hWnd ,IDC_REMOTECH, FALSE);
			//EnableDlgItem(hWnd ,IDL_MANUFACTURE, FALSE);
			EnableDlgItem(hWnd ,IDL_TRANSFER_PROTOCAL, FALSE);
		}
		else
		{
			EnableDlgItem(hWnd ,IDC_DEV_IPADDR, TRUE);
			EnableDlgItem(hWnd ,IDC_DEV_USERNAME, TRUE);
			EnableDlgItem(hWnd ,IDC_DEV_PASS, TRUE);
			EnableDlgItem(hWnd ,IDC_DEV_DATAPORT, TRUE);
			EnableDlgItem(hWnd ,IDC_DEV_HTTPPORT, TRUE);
			EnableDlgItem(hWnd ,IDC_DEV_RTSPPORT, TRUE);
			EnableDlgItem(hWnd ,IDC_REMOTECH, TRUE);
			//EnableDlgItem(hWnd ,IDL_MANUFACTURE, TRUE);
			EnableDlgItem(hWnd ,IDL_TRANSFER_PROTOCAL, TRUE);
		}
	}
	else
	{
		EnableDlgItem(hWnd ,IDC_SEARCH_AAS, FALSE);
		EnableDlgItem(hWnd ,IDC_DEV_IPADDR, FALSE);
		EnableDlgItem(hWnd ,IDC_DEV_USERNAME, FALSE);
		EnableDlgItem(hWnd ,IDC_DEV_PASS, FALSE);
		EnableDlgItem(hWnd ,IDC_DEV_DATAPORT, FALSE);
		EnableDlgItem(hWnd ,IDC_DEV_HTTPPORT, FALSE);
		EnableDlgItem(hWnd ,IDC_DEV_RTSPPORT, FALSE);
		EnableDlgItem(hWnd ,IDC_REMOTECH, FALSE);
#if 0			
		//EnableDlgItem(hWnd ,IDL_MANUFACTURE, TRUE);	
#endif
		EnableDlgItem(hWnd ,IDL_TRANSFER_PROTOCAL, FALSE);
	}
}

static void setEditLength(HWND hWnd)
{
	HWND userHwnd,passwordHwnd,rtspHwnd,httpHwnd,remoteHwnd,dataHwnd;

	userHwnd = GetDlgItem(hWnd, IDC_DEV_USERNAME);
	SendMessage(userHwnd, EM_LIMITTEXT, USERINFO_TEXT_LIMIT, 0L); //usename长度限定16个字节的字符

	passwordHwnd = GetDlgItem(hWnd, IDC_DEV_PASS);
	SendMessage(passwordHwnd, EM_LIMITTEXT, USERINFO_TEXT_LIMIT, 0L); //usename长度限定16个字节的字符

	dataHwnd = GetDlgItem(hWnd, IDC_DEV_DATAPORT);
	SendMessage(dataHwnd, EM_LIMITTEXT, PORT_TEXT_LIMIT, 0L); //DATA端口号长度限定5个字节的字符

	rtspHwnd = GetDlgItem(hWnd, IDC_DEV_RTSPPORT);
	SendMessage(rtspHwnd, EM_LIMITTEXT, PORT_TEXT_LIMIT, 0L); //RTSP端口号长度限定5个字节的字符

	httpHwnd = GetDlgItem(hWnd, IDC_DEV_HTTPPORT);
	SendMessage(httpHwnd, EM_LIMITTEXT, PORT_TEXT_LIMIT, 0L); 

	remoteHwnd = GetDlgItem(hWnd, IDC_REMOTECH);
	SendMessage(remoteHwnd, EM_LIMITTEXT, PORT_TEXT_LIMIT, 0L); 
}

static void UpdateChParam(HWND hWnd , int ch)
{
	char acPort[8];
	char acIpaddr[16];
	int sel = 0;
	int i = 0;
	IPNC_PROTOCOL_S stIpncProtocol[MAX_PROTOCAL_NUM];
	int supportProtocal = 0;
	NVR_CHANNEL_CFG_S chCfg;
	nvr_logon_get_channel_cfg(ch, &chCfg);
	//if (chCfg.u8Enable)
	if(chCfg.u32Size != 0) //通道中是否有数据
	{
		sprintf(acIpaddr, "%d.%d.%d.%d", (chCfg.u32IpAddr>>24)&0xff,
									 	(chCfg.u32IpAddr>>16)&0xff,
									 	(chCfg.u32IpAddr>>8)&0xff,
									 	(chCfg.u32IpAddr>>0)&0xff);
		SetDlgItemText(hWnd, IDC_DEV_IPADDR, acIpaddr);
		SetDlgItemText(hWnd, IDC_DEV_USERNAME, chCfg.szUser);
		SetDlgItemText(hWnd, IDC_DEV_PASS, chCfg.szPasswd);

		sprintf(acPort, "%d", chCfg.u16DataPort);	
		SetDlgItemText(hWnd, IDC_DEV_DATAPORT, acPort);	

		sprintf(acPort, "%d", chCfg.u16WebPort);	
		SetDlgItemText(hWnd, IDC_DEV_HTTPPORT, acPort);	
		sprintf(acPort, "%d", chCfg.u16RtspPort);	
		SetDlgItemText(hWnd, IDC_DEV_RTSPPORT, acPort);	

		//printf("chCfg.u8UseChn = %d,chCfg.u8RemoteChn = %d\n",chCfg.u8UseChn,chCfg.u8RemoteChn);
		sprintf(acPort, "%d", chCfg.u8UseChn);
		SetDlgItemText(hWnd, IDC_REMOTECH, acPort);
		
#if 0
		SendDlgItemMessage(hWnd,IDL_MANUFACTURE, CB_RESETCONTENT, 0, (LPARAM)0);
		SendDlgItemMessage(hWnd, IDL_MANUFACTURE, CB_ADDSTRING, 0, (LPARAM)"jxj");
		SendDlgItemMessage(hWnd, IDL_MANUFACTURE, CB_SETCURSEL, 0, 0);	
#endif
	}
	else
	{
		SetDlgItemText(hWnd, IDC_DEV_IPADDR, "192.168.1.253");
		SetDlgItemText(hWnd, IDC_DEV_USERNAME, "admin");
		SetDlgItemText(hWnd, IDC_DEV_PASS, "admin");
		SetDlgItemText(hWnd, IDC_DEV_DATAPORT, "6080");
		SetDlgItemText(hWnd, IDC_DEV_HTTPPORT, "80");	
		SetDlgItemText(hWnd, IDC_DEV_RTSPPORT, "554");	
		SetDlgItemText(hWnd, IDC_REMOTECH, "0");
		
#if 0
		SendDlgItemMessage(hWnd,IDL_MANUFACTURE, CB_RESETCONTENT, 0, (LPARAM)0);
		SendDlgItemMessage(hWnd, IDL_MANUFACTURE, CB_ADDSTRING, 0, (LPARAM)"jxj");
		SendDlgItemMessage(hWnd, IDL_MANUFACTURE, CB_SETCURSEL, 0, 0);	
#endif
		SendDlgItemMessage(hWnd,IDL_TRANSFER_PROTOCAL, CB_RESETCONTENT, 0, (LPARAM)0);
		SendDlgItemMessage(hWnd, IDL_TRANSFER_PROTOCAL, CB_ADDSTRING, 0, (LPARAM)GetProtoclaAliasName(chCfg.szProtocolName));
		SendDlgItemMessage(hWnd, IDL_TRANSFER_PROTOCAL, CB_SETCURSEL, 0, 0);
	}


	supportProtocal = nvr_lib_get_pro_table(stIpncProtocol, MAX_PROTOCAL_NUM);
	if (supportProtocal <= 0)	supportProtocal = 0;			
	SendDlgItemMessage(hWnd,IDL_TRANSFER_PROTOCAL, CB_RESETCONTENT, 0, (LPARAM)0);
	for(i = 0; i < supportProtocal; i++) 
	{
		if (0 == strcmp(chCfg.szProtocolName, stIpncProtocol[i].name))
			sel = i;
		//printf("g_stSearch.stIpncProtocol[%d].name = %s\r\n", i, stIpncProtocol[i].name);
		SendDlgItemMessage(hWnd, IDL_TRANSFER_PROTOCAL, CB_ADDSTRING, 0, (LPARAM)GetProtoclaAliasName(stIpncProtocol[i].name));
	}
	SendDlgItemMessage(hWnd, IDL_TRANSFER_PROTOCAL, CB_SETCURSEL, sel, 0);	
	
	if (chCfg.u8SearchA == 1)
		SendDlgItemMessage(hWnd, IDC_SEARCH_AAS , BM_SETCHECK, BST_CHECKED, 0);
	else
		SendDlgItemMessage(hWnd, IDC_SEARCH_AAS , BM_SETCHECK, BST_UNCHECKED, 0);

	if (chCfg.u8Enable == 1)
		SendDlgItemMessage(hWnd, IDC_ENABLE_CONNECT, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendDlgItemMessage(hWnd, IDC_ENABLE_CONNECT , BM_SETCHECK, BST_UNCHECKED, 0);


	CheckSearchProc(hWnd);
	
} 

static void InitChParam(HWND hWnd , int ch)
{
	char acPort[8];
	char acIpaddr[16];
	char acCh[8];
	int sel = 0;
	int i = 0;
	IPNC_PROTOCOL_S stIpncProtocol[MAX_PROTOCAL_NUM];
	int supportProtocal = 0;
	NVR_CHANNEL_CFG_S chCfg;

	SendDlgItemMessage(hWnd,IDL_CHNUM, CB_RESETCONTENT, 0, (LPARAM)0);
	for(i=0; i<NVR_MAX_CHANNEL_NUM; i++)
	{
		sprintf(acCh, "%d", i+1);
		SendDlgItemMessage(hWnd, IDL_CHNUM, CB_ADDSTRING, 0, (LPARAM)acCh);
	}
	SendDlgItemMessage(hWnd, IDL_CHNUM, CB_SETCURSEL, ch, 0);
	nvr_logon_get_channel_cfg(ch, &chCfg);
	//if (chCfg.u8Enable)
	if(chCfg.u32Size != 0) //通道中是否有数据
	{
		sprintf(acIpaddr, "%d.%d.%d.%d", (chCfg.u32IpAddr>>24)&0xff,
									 	(chCfg.u32IpAddr>>16)&0xff,
									 	(chCfg.u32IpAddr>>8)&0xff,
									 	(chCfg.u32IpAddr>>0)&0xff);
		SetDlgItemText(hWnd, IDC_DEV_IPADDR, acIpaddr);
		SetDlgItemText(hWnd, IDC_DEV_USERNAME, chCfg.szUser);
		SetDlgItemText(hWnd, IDC_DEV_PASS, chCfg.szPasswd);

		sprintf(acPort, "%d", chCfg.u16DataPort);	
		SetDlgItemText(hWnd, IDC_DEV_DATAPORT, acPort);	

		sprintf(acPort, "%d", chCfg.u16WebPort);	
		SetDlgItemText(hWnd, IDC_DEV_HTTPPORT, acPort);	
		sprintf(acPort, "%d", chCfg.u16RtspPort);	
		SetDlgItemText(hWnd, IDC_DEV_RTSPPORT, acPort);	

		//printf("chCfg.u8UseChn = %d,chCfg.u8RemoteChn = %d\n",chCfg.u8UseChn,chCfg.u8RemoteChn);
		sprintf(acPort, "%d", chCfg.u8UseChn);
		SetDlgItemText(hWnd, IDC_REMOTECH, acPort);
#if 0
		SendDlgItemMessage(hWnd,IDL_MANUFACTURE, CB_RESETCONTENT, 0, (LPARAM)0);
		SendDlgItemMessage(hWnd, IDL_MANUFACTURE, CB_ADDSTRING, 0, (LPARAM)"jxj");
		SendDlgItemMessage(hWnd, IDL_MANUFACTURE, CB_SETCURSEL, 0, 0);	
#endif
		if (chCfg.u8Enable == 1)
			SendDlgItemMessage(hWnd, IDC_ENABLE_CONNECT, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendDlgItemMessage(hWnd, IDC_ENABLE_CONNECT , BM_SETCHECK, BST_UNCHECKED, 0);

	}
	else
	{
		SetDlgItemText(hWnd, IDC_DEV_IPADDR, "192.168.1.253");
		SetDlgItemText(hWnd, IDC_DEV_USERNAME, "admin");
		SetDlgItemText(hWnd, IDC_DEV_PASS, "admin");
		SetDlgItemText(hWnd, IDC_DEV_DATAPORT, "6080");
		SetDlgItemText(hWnd, IDC_DEV_HTTPPORT, "80");	
		SetDlgItemText(hWnd, IDC_DEV_RTSPPORT, "554");	
		SetDlgItemText(hWnd, IDC_REMOTECH, "0");
		
#if 0
		SendDlgItemMessage(hWnd,IDL_MANUFACTURE, CB_RESETCONTENT, 0, (LPARAM)0);
		SendDlgItemMessage(hWnd, IDL_MANUFACTURE, CB_ADDSTRING, 0, (LPARAM)"jxj");
		SendDlgItemMessage(hWnd, IDL_MANUFACTURE, CB_SETCURSEL, 0, 0);	
#endif
		SendDlgItemMessage(hWnd,IDL_TRANSFER_PROTOCAL, CB_RESETCONTENT, 0, (LPARAM)0);
		SendDlgItemMessage(hWnd, IDL_TRANSFER_PROTOCAL, CB_ADDSTRING, 0, (LPARAM)GetProtoclaAliasName(chCfg.szProtocolName));
		SendDlgItemMessage(hWnd, IDL_TRANSFER_PROTOCAL, CB_SETCURSEL, 0, 0);
		SendDlgItemMessage(hWnd, IDC_ENABLE_CONNECT, BM_SETCHECK, BST_CHECKED, 0);

	}


	supportProtocal = nvr_lib_get_pro_table(stIpncProtocol, MAX_PROTOCAL_NUM);
	if (supportProtocal <= 0)	supportProtocal = 0;			
	SendDlgItemMessage(hWnd,IDL_TRANSFER_PROTOCAL, CB_RESETCONTENT, 0, (LPARAM)0);
	for(i = 0; i < supportProtocal; i++) 
	{
		if (0 == strcmp(chCfg.szProtocolName, stIpncProtocol[i].name))
			sel = i;
		//printf("g_stSearch.stIpncProtocol[%d].name = %s\r\n", i, stIpncProtocol[i].name);
		SendDlgItemMessage(hWnd, IDL_TRANSFER_PROTOCAL, CB_ADDSTRING, 0, (LPARAM)GetProtoclaAliasName(stIpncProtocol[i].name));
	}
	SendDlgItemMessage(hWnd, IDL_TRANSFER_PROTOCAL, CB_SETCURSEL, sel, 0);	
	
	if (chCfg.u8SearchA == 1)
		SendDlgItemMessage(hWnd, IDC_SEARCH_AAS , BM_SETCHECK, BST_CHECKED, 0);
	else
		SendDlgItemMessage(hWnd, IDC_SEARCH_AAS , BM_SETCHECK, BST_UNCHECKED, 0);

	CheckSearchProc(hWnd);
	
}


static void SaveChParam(HWND hWnd)
{
	char acIPAddr[16];
	char acPort[8];
	int chn = 0;
	NVR_CHANNEL_CFG_S chCfg;	
	int checkstat, enableStat;
	char AliasName[64];

	//获取通道号
	int index;
	chn = SendDlgItemMessage(hWnd, IDL_CHNUM, CB_GETCURSEL, 0, 0);
	if (chn >= NVR_MAX_CHANNEL_NUM || NVR_MAX_CHANNEL_NUM < 0)
		return;
	
	nvr_logon_get_channel_cfg(chn, &chCfg);
	
	GetDlgItemText(hWnd, IDC_DEV_IPADDR, acIPAddr, sizeof(acIPAddr));
	chCfg.u32IpAddr = htonl(inet_addr(acIPAddr));	
	GetDlgItemText(hWnd, IDC_DEV_USERNAME, chCfg.szUser, sizeof(chCfg.szUser));	
	GetDlgItemText(hWnd, IDC_DEV_PASS, chCfg.szPasswd, sizeof(chCfg.szPasswd));
	
	index = SendDlgItemMessage(hWnd, IDL_TRANSFER_PROTOCAL, CB_GETCURSEL, 0, 0);
	SendDlgItemMessage(hWnd, IDL_TRANSFER_PROTOCAL, CB_GETLBTEXT, index, (LPARAM) AliasName);
	//printf("##########AliasName = %s\r\n", AliasName);
	strcpy(chCfg.szProtocolName, GetProtoclaName(AliasName));
	
	//printf("##########szProtocolName = %s\r\n", chCfg.szProtocolName);
	//GetDlgItemText(hWnd, IDL_TRANSFER_PROTOCAL, chCfg.szProtocolName, sizeof(chCfg.szProtocolName));
	//strcpy(chCfg.szFactoryName, "jxj");
	//strcpy(chCfg.szProtocolName,"jxj_ti");
	//sprintf(chCfg.szDevName,"Ch%d", chn);
	GetDlgItemText(hWnd, IDC_DEV_HTTPPORT, acPort, sizeof(acPort));
	chCfg.u16WebPort = atoi(acPort);
	GetDlgItemText(hWnd, IDC_DEV_DATAPORT, acPort, sizeof(acPort));
	chCfg.u16DataPort = atoi(acPort);	
	GetDlgItemText(hWnd, IDC_DEV_RTSPPORT, acPort, sizeof(acPort));
	chCfg.u16RtspPort = atoi(acPort);

	GetDlgItemText(hWnd, IDC_REMOTECH, acPort, sizeof(acPort));
	
	chCfg.u8UseChn = atoi(acPort);

	
	checkstat = SendDlgItemMessage(hWnd, IDC_SEARCH_AAS, BM_GETCHECK, 0, 0);
	if (checkstat == BST_CHECKED)	
		chCfg.u8SearchA = 1;
	else	
		chCfg.u8SearchA = 0;
	chCfg.u32Size = sizeof(NVR_CHANNEL_CFG_S);
	
	enableStat = SendDlgItemMessage(hWnd, IDC_ENABLE_CONNECT, BM_GETCHECK, 0, 0);
	if (enableStat == BST_CHECKED)	
		chCfg.u8Enable = 1;
	else
		chCfg.u8Enable = 0;
	chCfg.u8RemoteChn = 1;
	//chCfg.u8UseChn= 0;
	if(0 > nvr_logon_set_channel_cfg(chn, &chCfg))
		printf("set channel failed #######################\r\n");
		
}


static int DevCfgWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
    {	int  chn = 0;
		if (lParam >=0 &&lParam < 4)
			chn = lParam;
		setEditLength(hDlg);  //设置编辑框支持的最大长度
		InitChParam(hDlg, chn);		
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);		
		//Gui_DrawCaption(hDlg, "通道设置");
        break;
	}
#endif	
	case MSG_ERASEBKGND:
	{
		HDC hdc = (HDC)wParam; 
		const RECT* clip = (const RECT*) lParam; 
		BOOL fGetDC = FALSE; 
		RECT rcTemp; 
		if (hdc == 0) 
		{ 
			hdc = GetClientDC (hDlg); 
			fGetDC = TRUE; 
		} 
		if (clip) 
		{ 
			rcTemp = *clip; 
			ScreenToClient (hDlg, &rcTemp.left, &rcTemp.top); 
			ScreenToClient (hDlg, &rcTemp.right, &rcTemp.bottom); 
			IncludeClipRect (hdc, &rcTemp); 
		} 
		/* 用图片填充背景*/ 
		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bk_pic[SIZE_700x500]); 
		Gui_WriteTitleName(hdc, &dev_config_title_rect, LTR("通道设置"));
		if (fGetDC) 
			ReleaseDC (hdc); 
		return 0; 
	}
	break;

	case MSG_COMMAND:
	{
		int id = LOWORD(wParam);
		int code = HIWORD(wParam);
		int ch;
		if(id == IDL_CHNUM)
		{
			if(code == CBN_SELCHANGE)
			{
				ch = SendDlgItemMessage (hDlg, IDL_CHNUM, CB_GETCURSEL, 0, 0);
				UpdateChParam(hDlg, ch);
				return 0;
			}
		}
		switch (wParam) 
		{
		case IDC_DEVCFG_SAVE:
			SaveChParam(hDlg);
			SendMessage (GetParent(hDlg), MSG_COMMAND, 
                                 0, 0);
			//PostMessage(hDlg, MSG_CLOSE, 0, 0);
			SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
			return 0;
		case IDC_SEARCH_AAS:
			CheckSearchProc(hDlg);
			return 0;
		case IDC_ENABLE_CONNECT:
			CheckEnableConnectProc(hDlg);
			return 0;
		case IDC_DEVCFG_CANCEL:
			PostMessage(hDlg, MSG_CLOSE, 0, 0);
			return 0;
		}
		break;
	}
	break;
	case MSG_RBUTTONUP:
    {		
		PostMessage(hDlg, MSG_CLOSE, 0, 0);
		
        break;
    }

    case MSG_CLOSE:
    {	
    	HWND hWin;
		UnloadBitmap(&bmp_bkgnd);
		pop_open_dlg(&headOpenDlg, &hWin);
        EndDialog (hDlg, 0);
        return 0;
    }

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

//远程设备配置
void CreateDevCfgDlgWin(HWND hParent, int chn)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "DevCfgDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_devcfg_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_devcfg_control, sizeof(ui_devcfg_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_devcfg_control)
    DevCfgDlg.controls = ui_devcfg_control;
    
    DialogBoxIndirectParam (&DevCfgDlg, hParent, DevCfgWinProc, chn);
	memcpy(ui_devcfg_control, ctrolTmp, sizeof(ui_devcfg_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

