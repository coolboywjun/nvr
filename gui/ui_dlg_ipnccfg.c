#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#include "ui_dlg_head.h"
#include "net_api.h"
#include "gui_core.h"

typedef enum
{
	IDC_STATIC_DEVID = 2000,
	IDC_STATIC_SOFTVER,
	IDC_STATIC_DEVTYPE,
	IDC_STATIC_DEVPORT,
	IDC_STATIC_MACADDR,
	IDC_STATIC_USERNAME,
	IDC_STATIC_PASSWD,
	IDC_STATIC_AUTODHCP,
	IDC_STATIC_DEVIP,
	IDC_STATIC_MASK,
	IDC_STATIC_GATEWAY,
	IDC_STATIC_AUTODNS,
	IDC_STATIC_MAINDNS,
	IDC_STATIC_NEXTDNS,
	IDC_IPNCCFG_SAVE,
	IDC_IPNCCFG_CANCEL,
	IDC_IPNC_EDIT_DEVID,
	IDC_IPNC_EDIT_SOFTVER,
	IDC_IPNC_EDIT_DEVTYPE,
	IDC_IPNC_EDIT_DEVPORT,
	IDC_IPNC_EDIT_MACADDR,
	IDC_IPNC_EDIT_USERNAME,
	IDC_IPNC_EDIT_PASSWD,
	IDC_IPNC_EDIT_AUTODHCP,
	IDC_IPNC_EDIT_DEVIP,
	IDC_IPNC_EDIT_MASK,
	IDC_IPNC_EDIT_GATEWAY,
	IDC_IPNC_EDIT_AUTODNS,
	IDC_IPNC_EDIT_DNS1,
	IDC_IPNC_EDIT_DNS2,
	
}UI_IPNC_SETTING_ID;

#define UI_DLG_IPNCCFG_W		700
#define UI_DLG_IPNCCFG_H		500

#define UI_DLG_IPNCCFG_X	((NVR_SCREEN_WIDTH-UI_DLG_IPNCCFG_W)/2)	
#define UI_DLG_IPNCCFG_Y	((NVR_SCREEN_HEIGHT-UI_DLG_IPNCCFG_H)/2)
#define UI_IPNCCFG_TEXT_START_X	50
#define UI_IPNCCFG_TEXT_START_Y	60
#define UI_IPNCCFG_EDIT_H	20
#define UI_IPNCCFG_INC_Y		(UI_IPNCCFG_EDIT_H + 12)
#define UI_IPNCCFG_IP_W     160

#define UI_SECONDLIST_START_X 300
#define BUTTON_CHECK_W		90
#define BUTTON_CHECK_H		20

static RECT  ipnc_set_title_rect = {0,0,UI_DLG_IPNCCFG_W, 50};
extern IPNC_NET_SEARCH_RES_S *GetUserByIndex(int index);
extern IPNC_PROTOCOL_S *GetProtocalByIndex(int index);

static CTRLDATA ui_ipnccfg_control[] =
{
	{
		CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UI_IPNCCFG_TEXT_START_X, UI_IPNCCFG_TEXT_START_Y, 100, UI_IPNCCFG_EDIT_H,
        IDC_STATIC_DEVID,
        "设备ID",
        0,
		WS_EX_TRANSPARENT
	},
	{
		CTRL_STATIC,
		WS_CHILD | WS_VISIBLE |ES_LEFT ,
		UI_IPNCCFG_TEXT_START_X + 115, UI_IPNCCFG_TEXT_START_Y, UI_IPNCCFG_IP_W, UI_IPNCCFG_EDIT_H,
        IDC_IPNC_EDIT_DEVID,
        "IPNC1316Q101845",
        0,        
        WS_EX_TRANSPARENT
	},
	
	{
		CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UI_IPNCCFG_TEXT_START_X, UI_IPNCCFG_TEXT_START_Y+UI_IPNCCFG_INC_Y, 115, UI_IPNCCFG_EDIT_H,
        IDC_STATIC_SOFTVER,
        "软件版本",
        0,
		WS_EX_TRANSPARENT
	},
	{
		CTRL_STATIC,
		WS_CHILD | WS_VISIBLE |SS_SIMPLE|WS_TABSTOP ,
		UI_IPNCCFG_TEXT_START_X + 115, UI_IPNCCFG_TEXT_START_Y+UI_IPNCCFG_INC_Y, UI_IPNCCFG_IP_W, UI_IPNCCFG_EDIT_H,
        IDC_IPNC_EDIT_SOFTVER,
        "02.2E.0A.13",
        0,        
        WS_EX_TRANSPARENT	
	},

	{
		CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UI_IPNCCFG_TEXT_START_X, UI_IPNCCFG_TEXT_START_Y+2*UI_IPNCCFG_INC_Y, 100, UI_IPNCCFG_EDIT_H,
        IDC_STATIC_DEVTYPE,
        "设备类型",
        0,
		WS_EX_TRANSPARENT
	},
	{
		CTRL_STATIC,
		WS_CHILD | WS_VISIBLE |SS_SIMPLE|WS_TABSTOP,
		UI_IPNCCFG_TEXT_START_X + 115, UI_IPNCCFG_TEXT_START_Y+2*UI_IPNCCFG_INC_Y, UI_IPNCCFG_IP_W, UI_IPNCCFG_EDIT_H,
        IDC_IPNC_EDIT_DEVTYPE,
        "16384",
        0,        
        WS_EX_TRANSPARENT	
	},
	
	{
		CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UI_IPNCCFG_TEXT_START_X, UI_IPNCCFG_TEXT_START_Y+3*UI_IPNCCFG_INC_Y, 100, UI_IPNCCFG_EDIT_H,
        IDC_STATIC_DEVPORT,
        "设备端口",
        0,
		WS_EX_TRANSPARENT
	},
	{
		CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		UI_IPNCCFG_TEXT_START_X + 115, UI_IPNCCFG_TEXT_START_Y+3*UI_IPNCCFG_INC_Y, UI_IPNCCFG_IP_W, UI_IPNCCFG_EDIT_H,
        IDC_IPNC_EDIT_DEVPORT,
        "6080",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT	
	},
	{
		CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UI_IPNCCFG_TEXT_START_X, UI_IPNCCFG_TEXT_START_Y+4*UI_IPNCCFG_INC_Y, 100, UI_IPNCCFG_EDIT_H,
        IDC_STATIC_MACADDR,
        "MAC地址",
        0,
		WS_EX_TRANSPARENT
	},
	{
		CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		UI_IPNCCFG_TEXT_START_X + 115, UI_IPNCCFG_TEXT_START_Y+4*UI_IPNCCFG_INC_Y, UI_IPNCCFG_IP_W, UI_IPNCCFG_EDIT_H,
        IDC_IPNC_EDIT_MACADDR,
        "E0-3C-5B-00-D2-69",
        0,        
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT | WS_EX_USEPARENTFONT
	},
	{
		CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UI_IPNCCFG_TEXT_START_X, UI_IPNCCFG_TEXT_START_Y+5*UI_IPNCCFG_INC_Y, 100, UI_IPNCCFG_EDIT_H,
        IDC_STATIC_USERNAME,
        "用户名",
        0,
		WS_EX_TRANSPARENT
	},
	{
		CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP/* | WS_DISABLED*/,
		UI_IPNCCFG_TEXT_START_X + 115, UI_IPNCCFG_TEXT_START_Y+5*UI_IPNCCFG_INC_Y, UI_IPNCCFG_IP_W, UI_IPNCCFG_EDIT_H,
        IDC_IPNC_EDIT_USERNAME,
        "admin",
        0,        
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT| WS_EX_USEPARENTFONT
	},
	{
		CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UI_IPNCCFG_TEXT_START_X, UI_IPNCCFG_TEXT_START_Y+6*UI_IPNCCFG_INC_Y, 100, UI_IPNCCFG_EDIT_H,
        IDC_STATIC_PASSWD,
        "密码",
        0,
		WS_EX_TRANSPARENT
	},
	{
		CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP|ES_PASSWORD/* | WS_DISABLED*/,
		UI_IPNCCFG_TEXT_START_X + 115, UI_IPNCCFG_TEXT_START_Y+6*UI_IPNCCFG_INC_Y, UI_IPNCCFG_IP_W, UI_IPNCCFG_EDIT_H,
        IDC_IPNC_EDIT_PASSWD,
        "admin",
        0,        
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT| WS_EX_USEPARENTFONT
	},
#if 1
	{
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,//| BS_CHECKED,
		UI_SECONDLIST_START_X+100,UI_IPNCCFG_TEXT_START_Y, BUTTON_CHECK_W+10, BUTTON_CHECK_H,
        IDC_STATIC_AUTODHCP,
        "启用DHCP",
        0,
        WS_EX_TRANSPARENT
    },
    {
		CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UI_SECONDLIST_START_X+100, UI_IPNCCFG_TEXT_START_Y+UI_IPNCCFG_INC_Y, 100, UI_IPNCCFG_EDIT_H,
        IDC_STATIC_DEVIP,
        "设备IP",
        0,
		WS_EX_TRANSPARENT
	},
	{
		CTRL_IP_ADDRESS,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		UI_SECONDLIST_START_X+100 + 100, UI_IPNCCFG_TEXT_START_Y+UI_IPNCCFG_INC_Y, UI_IPNCCFG_IP_W, UI_IPNCCFG_EDIT_H,
        IDC_IPNC_EDIT_DEVIP,
        "192.168.2.145",
        0,        
		WS_EX_EDIT_NUMINPUT|WS_EX_USEPARENTFONT
	},
	{
		CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UI_SECONDLIST_START_X+100, UI_IPNCCFG_TEXT_START_Y+2*UI_IPNCCFG_INC_Y, 100, UI_IPNCCFG_EDIT_H,
        IDC_STATIC_MASK,
        "掩码",
        0,
		WS_EX_TRANSPARENT
	},
	{
		CTRL_IP_ADDRESS,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		UI_SECONDLIST_START_X+100 + 100, UI_IPNCCFG_TEXT_START_Y+2*UI_IPNCCFG_INC_Y, UI_IPNCCFG_IP_W, UI_IPNCCFG_EDIT_H,
        IDC_IPNC_EDIT_MASK,
        "255.255.255.0",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
	},
	{
		CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UI_SECONDLIST_START_X+100, UI_IPNCCFG_TEXT_START_Y+3*UI_IPNCCFG_INC_Y, 100, UI_IPNCCFG_EDIT_H,
        IDC_STATIC_GATEWAY,
        "网关",
        0,
		WS_EX_TRANSPARENT
	},
	{
		CTRL_IP_ADDRESS,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		UI_SECONDLIST_START_X + 100+100, UI_IPNCCFG_TEXT_START_Y+3*UI_IPNCCFG_INC_Y, UI_IPNCCFG_IP_W, UI_IPNCCFG_EDIT_H,
        IDC_IPNC_EDIT_GATEWAY,
        "192.168.2.1",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
	},
	{
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,//| BS_CHECKED,
		UI_SECONDLIST_START_X+100,UI_IPNCCFG_TEXT_START_Y+4*UI_IPNCCFG_INC_Y, BUTTON_CHECK_W+110, BUTTON_CHECK_H,
        IDC_STATIC_AUTODNS,
        "启用自动DNS",
        0,
        WS_EX_TRANSPARENT
    },
    {
		CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UI_SECONDLIST_START_X+100, UI_IPNCCFG_TEXT_START_Y+5*UI_IPNCCFG_INC_Y, 100, UI_IPNCCFG_EDIT_H,
        IDC_STATIC_MAINDNS,
        "主DNS",
        0,
		WS_EX_TRANSPARENT
	},
	{
		CTRL_IP_ADDRESS,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		UI_SECONDLIST_START_X + 100+100, UI_IPNCCFG_TEXT_START_Y+5*UI_IPNCCFG_INC_Y, UI_IPNCCFG_IP_W, UI_IPNCCFG_EDIT_H,
        IDC_IPNC_EDIT_DNS1,
        "202.96.128.166",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
	},
	{
		CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UI_SECONDLIST_START_X+100, UI_IPNCCFG_TEXT_START_Y+6*UI_IPNCCFG_INC_Y, 100, UI_IPNCCFG_EDIT_H,
        IDC_STATIC_NEXTDNS,
        "次DNS",
        0,
		WS_EX_TRANSPARENT
	},
	{
		CTRL_IP_ADDRESS,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		UI_SECONDLIST_START_X + 100+100, UI_IPNCCFG_TEXT_START_Y+6*UI_IPNCCFG_INC_Y, UI_IPNCCFG_IP_W, UI_IPNCCFG_EDIT_H,
        IDC_IPNC_EDIT_DNS2,
        "202.96.134.133",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
	},
#endif
	////////////////////// GUI_BUTTON //////////////////////////////
	CONFIG_THREE_WORDS_BUTTON("保存", IDC_IPNCCFG_SAVE, UI_DLG_IPNCCFG_W - 175 , UI_DLG_IPNCCFG_H - 50),
	CONFIG_THREE_WORDS_BUTTON("取消", IDC_IPNCCFG_CANCEL, UI_DLG_IPNCCFG_W - 100 , UI_DLG_IPNCCFG_H - 50),

};


static DLGTEMPLATE IpncCfgDlg =
{
    WS_NONE,
    WS_EX_NONE,
    UI_DLG_IPNCCFG_X, UI_DLG_IPNCCFG_Y, UI_DLG_IPNCCFG_W, UI_DLG_IPNCCFG_H,
    "远程设备配置",
    0, 0,
    TABLESIZE(ui_ipnccfg_control), NULL,
    0
};

static void setEditLength(HWND hWnd)
{
	HWND userHwnd,passwordHwnd,devPortHwnd;

	userHwnd = GetDlgItem(hWnd, IDC_IPNC_EDIT_USERNAME);
	SendMessage(userHwnd, EM_LIMITTEXT, USERINFO_TEXT_LIMIT, 0L); //usename长度限定16个字节的字符

	passwordHwnd = GetDlgItem(hWnd, IDC_IPNC_EDIT_PASSWD);
	SendMessage(passwordHwnd, EM_LIMITTEXT, USERINFO_TEXT_LIMIT, 0L); //password长度限定16个字节的字符

	devPortHwnd = GetDlgItem(hWnd, IDC_IPNC_EDIT_DEVPORT);
	SendMessage(devPortHwnd, EM_LIMITTEXT, PORT_TEXT_LIMIT, 0L);
}

static void InitIpncParam(HWND hDlg, int index)
{
	char acIpaddr[32];
	char acCh[8];
	IPNC_NET_SEARCH_RES_S *ipncNetRes = NULL;
	
	ipncNetRes = GetUserByIndex(index);
	if(NULL == ipncNetRes)
	{
		printf("[error]Can not find %d colum\n",index);
		return;
	}
	IPNC_PROTOCOL_S *ipncProto = NULL;

	ipncProto = GetProtocalByIndex(index);
	if(NULL == ipncProto)
	{
		printf("[error]Can not find %d colum\n",index);
		return;
	}

	SetDlgItemText(hDlg, IDC_IPNC_EDIT_DEVID, (char *)ipncNetRes->u8SerialId);
		
	
	sprintf(acIpaddr,"%02X.%02X.%02X.%02X",(ipncNetRes->u32SoftVer>>24)&0xff,
										(ipncNetRes->u32SoftVer>>16)&0xff,
										(ipncNetRes->u32SoftVer>>8)&0xff,
										(ipncNetRes->u32SoftVer>>0)&0xff);
	SetDlgItemText(hDlg, IDC_IPNC_EDIT_SOFTVER, acIpaddr);
	
	sprintf(acCh, "%d",ipncNetRes->u16DevType);
	SetDlgItemText(hDlg, IDC_IPNC_EDIT_DEVTYPE, acCh);
	
	sprintf(acCh, "%d",ipncNetRes->u16DevPort);
	SetDlgItemText(hDlg, IDC_IPNC_EDIT_DEVPORT, acCh);
	
	sprintf(acIpaddr,"%d.%d.%d.%d", (ipncNetRes->u32DevIp>>24)&0xff,
									(ipncNetRes->u32DevIp>>16)&0xff,
									(ipncNetRes->u32DevIp>>8)&0xff,
									(ipncNetRes->u32DevIp>>0)&0xff);
	SetDlgItemText(hDlg, IDC_IPNC_EDIT_DEVIP, acIpaddr);
	
	sprintf(acIpaddr,"%02X:%02X:%02X:%02X:%02X:%02X", 
										ipncNetRes->u8MacAddr[0],
										ipncNetRes->u8MacAddr[1],
										ipncNetRes->u8MacAddr[2],
										ipncNetRes->u8MacAddr[3],
										ipncNetRes->u8MacAddr[4],
										ipncNetRes->u8MacAddr[5]);
	SetDlgItemText(hDlg, IDC_IPNC_EDIT_MACADDR, acIpaddr);
	
	if(ipncNetRes->u8DHCP != 0)
	{
		SendMessage(GetDlgItem(hDlg, IDC_STATIC_AUTODHCP), BM_SETCHECK, BST_CHECKED, 0);
		EnableDlgItem(hDlg, IDC_IPNC_EDIT_DEVIP, FALSE);
		EnableDlgItem(hDlg, IDC_IPNC_EDIT_MASK, FALSE);
		EnableDlgItem(hDlg, IDC_IPNC_EDIT_GATEWAY, FALSE);
		
		EnableDlgItem(hDlg, IDC_STATIC_AUTODNS, TRUE);
		if(ipncNetRes->u8Dns != 0)
		{
			SendMessage(GetDlgItem(hDlg, IDC_STATIC_AUTODNS), BM_SETCHECK, BST_CHECKED, 0);
			EnableDlgItem(hDlg, IDC_IPNC_EDIT_DNS1, FALSE);
			EnableDlgItem(hDlg, IDC_IPNC_EDIT_DNS2, FALSE);
		}
		else
		{
			EnableDlgItem(hDlg, IDC_IPNC_EDIT_DNS1, TRUE);
			EnableDlgItem(hDlg, IDC_IPNC_EDIT_DNS2, TRUE);
		}
	}
	else
	{
		EnableDlgItem(hDlg, IDC_IPNC_EDIT_DEVIP, TRUE);
		EnableDlgItem(hDlg, IDC_IPNC_EDIT_MASK, TRUE);
		EnableDlgItem(hDlg, IDC_IPNC_EDIT_GATEWAY, TRUE);

		EnableDlgItem(hDlg, IDC_STATIC_AUTODNS, FALSE);
		EnableDlgItem(hDlg, IDC_IPNC_EDIT_DNS1, TRUE);
		EnableDlgItem(hDlg, IDC_IPNC_EDIT_DNS2, TRUE);
	}

	sprintf(acIpaddr,"%d.%d.%d.%d", (ipncNetRes->u32DevIpMask>>24)&0xff,
										(ipncNetRes->u32DevIpMask>>16)&0xff,
										(ipncNetRes->u32DevIpMask>>8)&0xff,
										(ipncNetRes->u32DevIpMask>>0)&0xff);
	SetDlgItemText(hDlg, IDC_IPNC_EDIT_MASK, acIpaddr);
	
	sprintf(acIpaddr,"%d.%d.%d.%d", (ipncNetRes->u32DevGwAddr>>24)&0xff,
										(ipncNetRes->u32DevGwAddr>>16)&0xff,
										(ipncNetRes->u32DevGwAddr>>8)&0xff,
										(ipncNetRes->u32DevGwAddr>>0)&0xff);
	SetDlgItemText(hDlg, IDC_IPNC_EDIT_GATEWAY, acIpaddr);
	
	sprintf(acIpaddr,"%d.%d.%d.%d",(ipncNetRes->u32DevDns1>>24)&0xff,
										(ipncNetRes->u32DevDns1>>16)&0xff,
										(ipncNetRes->u32DevDns1>>8)&0xff,
										(ipncNetRes->u32DevDns1>>0)&0xff);
	SetDlgItemText(hDlg, IDC_IPNC_EDIT_DNS1, acIpaddr);
	
	sprintf(acIpaddr,"%d.%d.%d.%d",(ipncNetRes->u32DevDns2>>24)&0xff,
										(ipncNetRes->u32DevDns2>>16)&0xff,
										(ipncNetRes->u32DevDns2>>8)&0xff,
										(ipncNetRes->u32DevDns2>>0)&0xff);
	SetDlgItemText(hDlg, IDC_IPNC_EDIT_DNS2, acIpaddr);

	//printf(".................ipncProto.usrPwd[0].usr: %s\n",ipncProto->usrPwd[0].usr);
	//printf(".................ipncProto.usrPwd[0].pwd: %s\n",ipncProto->usrPwd[0].pwd);

	SetDlgItemText(hDlg, IDC_IPNC_EDIT_USERNAME, ipncProto->usrPwd[0].usr);
	SetDlgItemText(hDlg, IDC_IPNC_EDIT_PASSWD, ipncProto->usrPwd[0].pwd);

}

static int mac_str_to_bin( char *str, char *mac)
{
    int i;
    char *s, *e;

    if ((mac == NULL) || (str == NULL))
    {
        return -1;
    }

    s = (char *) str;
    for (i = 0; i < 6; ++i)
    {
        mac[i] = s ? strtoul (s, &e, 16) : 0;
        if (s)
           s = (*e) ? e + 1 : e;
    }
    return 0;
}

static void SaveChParam(HWND hDlg, int index)
{
	char acPort[8];
	char acIpaddr[32] = {0};//,acIpaddr2[32];
	char acText[32] = {0};
	//char acCh[8];
	uint8_t	macAddr[6];
	HWND devSoftVer,devType,devPort,devMac,devIP,devMask,devGateway;
	uint32_t softVer = 0,devIpValue,devMaskValue,devGateValue,devDns1Value,devDns2Value;
	HWND devDNS1,devDNS2,userName,password;
	unsigned int checked = 0;
	
	IPNC_NET_SEARCH_RES_S *ipncNetRes = NULL;
	
	ipncNetRes = GetUserByIndex(index);
	if(NULL == ipncNetRes)
	{
		printf("[error]Can not find %d colum\n",index);
		return;
	}
	
	IPNC_PROTOCOL_S *ipncProto = NULL;

	ipncProto = GetProtocalByIndex(index);
	if(NULL == ipncProto)
	{
		printf("[error]Can not find %d colum\n",index);
		return;
	}
	
	/*软件版本*/
	memset(acIpaddr, 0, 32);
	devSoftVer = GetDlgItem (hDlg, IDC_IPNC_EDIT_SOFTVER);
	GetWindowText(devSoftVer, acIpaddr, 32);
	
	softVer = net_ip_a2n(acIpaddr);
	if(softVer != ipncNetRes->u32SoftVer)
	{
		ipncNetRes->u32SoftVer = softVer;
	}
	/*设备类型*/
	devType = GetDlgItem (hDlg, IDC_IPNC_EDIT_DEVTYPE);
	GetWindowText(devType, acIpaddr, 32);
	if(atol(acIpaddr) != ipncNetRes->u16DevType)
		ipncNetRes->u16DevType = atol(acIpaddr);
	
	/*设备端口*/
	devPort = GetDlgItem (hDlg, IDC_IPNC_EDIT_DEVPORT);
	GetWindowText(devPort, acPort, 8);
	printf("u16DevPort: %ld\n",atol(acPort));
	if(atol(acPort) != ipncNetRes->u16DevPort)
		ipncNetRes->u16DevPort = atol(acPort);

	/*MAC地址*/
	devMac = GetDlgItem (hDlg, IDC_IPNC_EDIT_MACADDR);
	GetWindowText(devMac, acIpaddr, 32);
	mac_str_to_bin((char *)acIpaddr, (char *)macAddr);

	if(strncmp((char *)macAddr, (char *)ipncNetRes->u8MacAddr,6) != 0)
	{
		printf("mac addr different\n");
		strcpy((char *)ipncNetRes->u8MacAddr, (char *)macAddr);
	}

	/*自动DHCP*/
	checked = SendDlgItemMessage(hDlg, IDC_STATIC_AUTODHCP, BM_GETCHECK, 0, 0);
	ipncNetRes->u8DHCP = checked;

	/*设备IP*/
	devIP= GetDlgItem (hDlg, IDC_IPNC_EDIT_DEVIP);
	GetWindowText(devIP, acIpaddr, 32);
	devIpValue = net_ip_a2n(acIpaddr);
	if(devIpValue != ipncNetRes->u32DevIp)
		ipncNetRes->u32DevIp = devIpValue;
	
	/*掩码*/
	devMask = GetDlgItem (hDlg, IDC_IPNC_EDIT_MASK);
	GetWindowText(devMask, acIpaddr, 32);
	devMaskValue = net_ip_a2n(acIpaddr);
	if(devMaskValue != ipncNetRes->u32DevIpMask)
		ipncNetRes->u32DevIpMask = devMaskValue;
	
	/*网关*/
	devGateway = GetDlgItem (hDlg, IDC_IPNC_EDIT_GATEWAY);
	GetWindowText(devGateway, acIpaddr, 32);
	devGateValue = net_ip_a2n(acIpaddr);
	if(ipncNetRes->u32DevGwAddr != devGateValue)
		ipncNetRes->u32DevGwAddr = devGateValue;

	/*自动DNS*/
	checked = SendDlgItemMessage(hDlg, IDC_STATIC_AUTODNS, BM_GETCHECK, 0, 0);
	ipncNetRes->u8Dns = checked;

	/*主DNS*/
	devDNS1 = GetDlgItem (hDlg, IDC_IPNC_EDIT_DNS1);
	GetWindowText(devDNS1, acIpaddr, 32);
	
	devDns1Value = net_ip_a2n(acIpaddr);
	if(ipncNetRes->u32DevDns1 != devDns1Value)
		ipncNetRes->u32DevDns1 = devDns1Value;
	
	/*次DNS*/
	devDNS2 = GetDlgItem (hDlg, IDC_IPNC_EDIT_DNS2);
	GetWindowText(devDNS2, acIpaddr, 32);
	
	devDns2Value = net_ip_a2n(acIpaddr);
	if(ipncNetRes->u32DevDns2 != devDns2Value)
		ipncNetRes->u32DevDns2 = devDns2Value;

	/*发送配置信息到ipnc端*/
	long lHandle;
	IPNC_NET_SEARCH_CFG_REQ_S searchCfg;
	
	lHandle = nvr_lib_search_start(ipncProto->name);
	if(lHandle == 0)
		return ;
	memset(&searchCfg, 0, sizeof(IPNC_NET_SEARCH_CFG_REQ_S));
	if(nvr_lib_search_req_cpy_cfg(&searchCfg, ipncNetRes) < 0)
	{
		printf("[error] nvr_lib_search_req_cpy_cfg\n");
		return;
	}
	/****暂时指定用户密码****/
	userName = GetDlgItem (hDlg, IDC_IPNC_EDIT_USERNAME);
	GetWindowText(userName, acText, 32);
	strncpy(searchCfg.szUsrName, acText, sizeof(searchCfg.szUsrName));
	//printf(".................searchCfg.szUsrName: %s\n",acText);
	
	password = GetDlgItem (hDlg, IDC_IPNC_EDIT_PASSWD);
	GetWindowText(password, acText, 32);	
	strncpy(searchCfg.szPsw, acText, sizeof(searchCfg.szPsw));
	//printf(".................searchCfg.szPsw: %s\n",acText);
	if(nvr_lib_search_cfg(lHandle, &searchCfg, sizeof(IPNC_NET_SEARCH_CFG_REQ_S)) < 0)
	{
		printf("[error] nvr_lib_search_cfg\n");
		return;
	}
	if(nvr_lib_search_stop(lHandle) < 0)
	{
		printf("[error] nvr_lib_search_stop\n");
		return;
	}

	
}

static void IpncSettingCheckCmdPorc(HWND hDlg, int checkID)
{
	int bChecked;
	bChecked = SendDlgItemMessage(hDlg, checkID, BM_GETCHECK, 0, 0);
	if (bChecked)
	{
		if(checkID == IDC_STATIC_AUTODHCP)
		{
			EnableDlgItem(hDlg, IDC_IPNC_EDIT_DEVIP, FALSE);
			EnableDlgItem(hDlg, IDC_IPNC_EDIT_MASK, FALSE);
			EnableDlgItem(hDlg, IDC_IPNC_EDIT_GATEWAY, FALSE);
			EnableDlgItem(hDlg, IDC_STATIC_AUTODNS, TRUE);
		}
		else if(checkID == IDC_STATIC_AUTODNS)
		{			
			EnableDlgItem(hDlg, IDC_IPNC_EDIT_DNS1, FALSE);
			EnableDlgItem(hDlg, IDC_IPNC_EDIT_DNS2, FALSE);
		}
	}
	else
	{			
		if (checkID == IDC_STATIC_AUTODHCP)
		{
			EnableDlgItem(hDlg, IDC_IPNC_EDIT_DEVIP, TRUE);
			EnableDlgItem(hDlg, IDC_IPNC_EDIT_MASK, TRUE);
			EnableDlgItem(hDlg, IDC_IPNC_EDIT_GATEWAY, TRUE);
			EnableDlgItem(hDlg, IDC_STATIC_AUTODNS, FALSE);
		}
		else if(checkID == IDC_STATIC_AUTODNS)
		{			
			EnableDlgItem(hDlg, IDC_IPNC_EDIT_DNS1, TRUE);
			EnableDlgItem(hDlg, IDC_IPNC_EDIT_DNS2, TRUE);
		}
	}
	
}

static int IpncCfgWinProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	static int index =0;
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		index = lParam;
		//printf("index = %d\n",index);
		setEditLength(hDlg);
		InitIpncParam(hDlg, index);
		push_open_dlg(&headOpenDlg, hDlg);
//    	show_open_dlg(headOpenDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg, "IPNC设置");
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
		Gui_WriteTitleName(hdc, &ipnc_set_title_rect, LTR("IPNC设置"));
		if (fGetDC) 
			ReleaseDC (hdc); 
		return 0; 
	}
	break;

	case MSG_COMMAND:
	{
		switch (wParam) 
		{
		case IDC_STATIC_AUTODHCP:
		case IDC_STATIC_AUTODNS:
			{
				IpncSettingCheckCmdPorc(hDlg, wParam);
				break;
			}
		case IDC_IPNCCFG_SAVE:
			SaveChParam(hDlg,index);
			//SendMessage (GetParent(hDlg), MSG_COMMAND, 
            //                     0, 0);
			//PostMessage(hDlg, MSG_CLOSE, 0, 0);
			SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
			return 0;
		case IDC_IPNCCFG_CANCEL:
			//SendMessage(GetParent(hDlg), MSG_COMMAND, 0, 0);
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
//		pop_open_dlg(&headOpenDlg, &hWin);
        EndDialog (hDlg, 0);
        return 0;
    }

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}
void CreateIpncCfgDlgWin(HWND hParent, int index)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "IpncCfgDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_ipnccfg_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_ipnccfg_control, sizeof(ui_ipnccfg_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_ipnccfg_control)
    IpncCfgDlg.controls = ui_ipnccfg_control;    
    DialogBoxIndirectParam (&IpncCfgDlg, hParent, IpncCfgWinProc, index);
	memcpy(ui_ipnccfg_control, ctrolTmp, sizeof(ui_ipnccfg_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

