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

#include "ui_dlg_head.h"
#include "myedit.h"
#include "ctrl_ipaddress.h"
#include "nvr_lib_interface.h"
#include "net_api.h"
#include "gui_core.h"

#define UI_NET_DLG_W		700
#define UI_NET_DLG_H		500

#define UI_NET_DLG_X	((NVR_SCREEN_WIDTH-UI_NET_DLG_W)/2)	
#define UI_NET_DLG_Y	((NVR_SCREEN_HEIGHT-UI_NET_DLG_H)/2)

typedef enum
{
	IDC_STATIC_MACADDR = 2300,
	IDC_STATIC_IPADDR,
	IDC_STATIC_IPMASK,
	IDC_STATIC_GATEWAY,
	IDC_STATIC_TCPPORT,
	IDC_STATIC_HTTPPORT,
	IDC_STATIC_AUTODHCP,
	IDC_STATIC_AUTODNS,
	IDC_MAC_ADDR,
	IDC_IPADDR,
	IDC_IPMASK,
	IDC_GATEWAY,
	IDC_TCPPORT,
	IDC_HTTPPORT,
	IDC_NET_SERVER_CFG,
	IDC_NET_DEFAULT,
	IDC_NET_SAVE,
	IDC_NET_CANCEL,
	IDC_DNS1,
	IDC_DNS2,
	IDC_RTSPPORT,
	IDC_DDNS_CONFIG,
	IDC_UPNP_CONFIG,
	IDC_PPPOE_CONFIG,
	IDC_NET_PREV,
	IDC_NET_NEXT,
	IDC_NET_WIZARD_CANCEL,
}UI_NET_SETTING_ID;

#define DLG_NET_TEXT_X	50
#define DLG_NET_TEXT_Y  60

#define IP_EDIT_W	160
#define PORT_EDIT_W	110
#define DLG_NET_EDIT_H	20
#define DLG_TEXT_W		90 //150
#define DLG_ENC_Y		(DLG_NET_EDIT_H + 12)

static RECT net_config_title_rect = {0,0,UI_NET_DLG_W,50};


static CTRLDATA ui_net_control[] =
{
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
         DLG_NET_TEXT_X, DLG_NET_TEXT_Y, DLG_TEXT_W, DLG_NET_EDIT_H,
        IDC_STATIC_MACADDR,
        "MAC地址",
        0,
		WS_EX_TRANSPARENT
    },
    {
		CTRL_MYEDIT,
        WS_CHILD| WS_VISIBLE | ES_LEFT | WS_TABSTOP,
        DLG_NET_TEXT_X + DLG_TEXT_W,  DLG_NET_TEXT_Y, IP_EDIT_W, DLG_NET_EDIT_H,
        IDC_MAC_ADDR,
        "E0:3C:5B:00:D2:69",
        0,        
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT| WS_EX_USEPARENTFONT
    },
    {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,//| BS_CHECKED,
        DLG_NET_TEXT_X , DLG_NET_TEXT_Y+DLG_ENC_Y*1, DLG_TEXT_W+10, DLG_NET_EDIT_H,
        IDC_STATIC_AUTODHCP,
        "启用DHCP",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        DLG_NET_TEXT_X , DLG_NET_TEXT_Y+DLG_ENC_Y*2, DLG_TEXT_W, DLG_NET_EDIT_H,
        IDC_STATIC_IPADDR,
        "IP地址",
        0,
		WS_EX_TRANSPARENT
    },
     {
        CTRL_IP_ADDRESS,
        WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
        DLG_NET_TEXT_X + DLG_TEXT_W, DLG_NET_TEXT_Y+DLG_ENC_Y*2, IP_EDIT_W, DLG_NET_EDIT_H,
        IDC_IPADDR,
        "192.168.25.100",
        0,
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
     },
     {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		DLG_NET_TEXT_X ,  DLG_NET_TEXT_Y + DLG_ENC_Y*3, DLG_TEXT_W, DLG_NET_EDIT_H,
        IDC_STATIC_IPMASK,
        "子网掩码",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_IP_ADDRESS,
        WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
        DLG_NET_TEXT_X + DLG_TEXT_W, DLG_NET_TEXT_Y+DLG_ENC_Y*3, IP_EDIT_W, DLG_NET_EDIT_H,
        IDC_IPMASK,
        "255.255.255.0",
        0,
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
     },
     {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		DLG_NET_TEXT_X ,  DLG_NET_TEXT_Y + DLG_ENC_Y*4, DLG_TEXT_W+15, DLG_NET_EDIT_H,
        IDC_STATIC_GATEWAY,
        "默认网关",
        0,
		WS_EX_TRANSPARENT
    },
     {
        CTRL_IP_ADDRESS,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
        DLG_NET_TEXT_X + DLG_TEXT_W,  DLG_NET_TEXT_Y + DLG_ENC_Y*4, IP_EDIT_W, DLG_NET_EDIT_H,
        IDC_GATEWAY,
        "192.168.4.1",
        0,
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT	
      },
	{
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,//| BS_CHECKED,
		DLG_NET_TEXT_X + 350,  DLG_NET_TEXT_Y + DLG_ENC_Y*1, DLG_TEXT_W+110, DLG_NET_EDIT_H,
        IDC_STATIC_AUTODNS,
        "启用自动DNS",
        0,
        WS_EX_TRANSPARENT
    },
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		DLG_NET_TEXT_X + 350,  DLG_NET_TEXT_Y + DLG_ENC_Y*2, DLG_TEXT_W, DLG_NET_EDIT_H,
        IDC_STATIC_GATEWAY,
        "首选DNS",
        0,
		WS_EX_TRANSPARENT
    },
     {
        CTRL_IP_ADDRESS,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
        DLG_NET_TEXT_X + DLG_TEXT_W+ 350,  DLG_NET_TEXT_Y + DLG_ENC_Y*2, IP_EDIT_W, DLG_NET_EDIT_H,
        IDC_DNS1,
        "8.8.8.8",
        0,
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
      },
     {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		DLG_NET_TEXT_X + 350,  DLG_NET_TEXT_Y + DLG_ENC_Y*3, DLG_TEXT_W, DLG_NET_EDIT_H,
        IDC_STATIC_GATEWAY,
        "备用DNS",
        0,
		WS_EX_TRANSPARENT
    },
     {
        CTRL_IP_ADDRESS,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
        DLG_NET_TEXT_X + DLG_TEXT_W+ 350,  DLG_NET_TEXT_Y + DLG_ENC_Y*3, IP_EDIT_W, DLG_NET_EDIT_H,
        IDC_DNS2,
        "8.8.4.4",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT	
      },
     {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		DLG_NET_TEXT_X+ 350,  DLG_NET_TEXT_Y + DLG_ENC_Y*4, DLG_TEXT_W, DLG_NET_EDIT_H,
        IDC_STATIC_HTTPPORT,
        "HTTP端口",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
        DLG_NET_TEXT_X + DLG_TEXT_W+ 350, DLG_NET_TEXT_Y + DLG_ENC_Y*4, 160, DLG_NET_EDIT_H, 
        IDC_HTTPPORT,
        "80",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
     },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		DLG_NET_TEXT_X+ 350,  DLG_NET_TEXT_Y + DLG_ENC_Y*5, DLG_TEXT_W, DLG_NET_EDIT_H,
        IDC_STATIC_HTTPPORT,
        "数据端口",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
        DLG_NET_TEXT_X + DLG_TEXT_W+ 350, DLG_NET_TEXT_Y + DLG_ENC_Y*5, 160, DLG_NET_EDIT_H, 
        IDC_RTSPPORT,
        "554",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
     },

    /////////////////////////////////////////////////////////////////
	{
        CTRL_STATIC,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        DLG_NET_TEXT_X , DLG_NET_TEXT_Y + DLG_ENC_Y*7, 100+100, 20,
        IDC_NET_SERVER_CFG,
        "网络服务设置",
        0,
		WS_EX_TRANSPARENT
    },
    //////////////////////////// GUI_BUTTON /////////////////////////////////////
    CONFIG_FOUR_WORDS_BUTTON("DDNS设置", IDC_DDNS_CONFIG, DLG_NET_TEXT_X , DLG_NET_TEXT_Y + DLG_ENC_Y*8),
    CONFIG_FOUR_WORDS_BUTTON("UPNP设置", IDC_UPNP_CONFIG, DLG_NET_TEXT_X + 80 + 20 , DLG_NET_TEXT_Y + DLG_ENC_Y*8),
    CONFIG_FOUR_WORDS_BUTTON("PPPOE设置", IDC_PPPOE_CONFIG, DLG_NET_TEXT_X + 80*2+20*2 , DLG_NET_TEXT_Y + DLG_ENC_Y*8),

	CONFIG_THREE_WORDS_BUTTON("默认", IDC_NET_DEFAULT, DLG_NET_TEXT_X , UI_NET_DLG_H - 50),
	CONFIG_THREE_WORDS_BUTTON("确认", IDC_NET_SAVE, UI_NET_DLG_W - 175 , UI_NET_DLG_H - 50),
	CONFIG_THREE_WORDS_BUTTON("取消", IDC_NET_CANCEL, UI_NET_DLG_W - 100 , UI_NET_DLG_H - 50),

	
	CONFIG_THREE_WORDS_BUTTON("上一步", IDC_NET_PREV, UI_NET_DLG_W - 250 , UI_NET_DLG_H - 50),
	CONFIG_THREE_WORDS_BUTTON("下一步", IDC_NET_NEXT, UI_NET_DLG_W - 175 , UI_NET_DLG_H - 50),
	CONFIG_THREE_WORDS_BUTTON("取消", IDC_NET_WIZARD_CANCEL, UI_NET_DLG_W - 100 , UI_NET_DLG_H - 50),

};


static DLGTEMPLATE NetDlg =
{
    WS_NONE,
    WS_EX_NONE,
    UI_NET_DLG_X, UI_NET_DLG_Y, UI_NET_DLG_W, UI_NET_DLG_H,
    "远程设备",
    0, 0,
    TABLESIZE(ui_net_control), NULL,
    0
};

static void setEditLength(HWND hWnd)
{
	HWND httpHwnd,rtspHwnd;

	httpHwnd = GetDlgItem(hWnd, IDC_HTTPPORT);
	SendMessage(httpHwnd, EM_LIMITTEXT, PORT_TEXT_LIMIT, 0L);

	rtspHwnd = GetDlgItem(hWnd, IDC_RTSPPORT);
	SendMessage(rtspHwnd, EM_LIMITTEXT, PORT_TEXT_LIMIT, 0L);

}

static void InitNetParam(HWND hWnd)
{
	HWND hMac,hIP, hMask, hGateway, hDNS1, hDNS2;
	HWND hHttp, hRtsp;
	char acIp[32];
	char acTmp[8];
	int ret = 0;
	int tryTime = 2; 	
	HWND hCtrl;

	if (NeedWizard())
	{
		hCtrl = GetDlgItem(hWnd, IDC_NET_SAVE);
		ShowWindow(hCtrl,SW_HIDE);
		hCtrl = GetDlgItem(hWnd, IDC_NET_CANCEL);
		ShowWindow(hCtrl,SW_HIDE);
	}
	else
	{
		hCtrl = GetDlgItem(hWnd, IDC_NET_PREV);
		ShowWindow(hCtrl,SW_HIDE);
		hCtrl = GetDlgItem(hWnd, IDC_NET_NEXT);
		ShowWindow(hCtrl,SW_HIDE);
		hCtrl = GetDlgItem(hWnd, IDC_NET_WIZARD_CANCEL);
		ShowWindow(hCtrl,SW_HIDE);
	}

	NVR_DEV_NET_CFG_S stNetCfg;	
	memset(&stNetCfg, 0, sizeof(NVR_DEV_NET_CFG_S));

	do{
		ret = nvr_logon_get_net_cfg(0, &stNetCfg);
		tryTime--;
	}while((ret < 0) && (tryTime > 0));

	if((ret > 0) || (tryTime == 0))
	{
		printf("[ERROR].............nvr_logon_get_net_cfg fail!!!!!!1\n");
		return;
	}
	
	//mac addr
	
	sprintf(acIp, "%02X:%02X:%02X:%02X:%02X:%02X", stNetCfg.struEtherCfg[0].u8MacAddr[0],
								 stNetCfg.struEtherCfg[0].u8MacAddr[1],
								 stNetCfg.struEtherCfg[0].u8MacAddr[2],
								 stNetCfg.struEtherCfg[0].u8MacAddr[3],
								 stNetCfg.struEtherCfg[0].u8MacAddr[4],
								 stNetCfg.struEtherCfg[0].u8MacAddr[5]);
	hMac = GetDlgItem (hWnd, IDC_MAC_ADDR);

	SetWindowText(hMac, acIp);

	//local ipaddr
	sprintf(acIp, "%d.%d.%d.%d", (stNetCfg.struEtherCfg[0].u32IpAddr>>24)&0xff,
								 (stNetCfg.struEtherCfg[0].u32IpAddr>>16)&0xff,
								 (stNetCfg.struEtherCfg[0].u32IpAddr>>8)&0xff,
								 (stNetCfg.struEtherCfg[0].u32IpAddr>>0)&0xff);
	hIP = GetDlgItem (hWnd, IDC_IPADDR);
	SetWindowText(hIP, acIp);

	//netmask
	sprintf(acIp, "%d.%d.%d.%d", (stNetCfg.struEtherCfg[0].u32SubMask>>24)&0xff,
								 (stNetCfg.struEtherCfg[0].u32SubMask>>16)&0xff,
								 (stNetCfg.struEtherCfg[0].u32SubMask>>8)&0xff,
								 (stNetCfg.struEtherCfg[0].u32SubMask>>0)&0xff);
	hMask = GetDlgItem (hWnd, IDC_IPMASK);
	SetWindowText(hMask, acIp);
	
	//gateway
	sprintf(acIp, "%d.%d.%d.%d", (stNetCfg.struEtherCfg[0].u32GateWay>>24)&0xff,
								 (stNetCfg.struEtherCfg[0].u32GateWay>>16)&0xff,
								 (stNetCfg.struEtherCfg[0].u32GateWay>>8)&0xff,
								 (stNetCfg.struEtherCfg[0].u32GateWay>>0)&0xff);
	hGateway = GetDlgItem (hWnd, IDC_GATEWAY);
	SetWindowText(hGateway, acIp);


	if(stNetCfg.struEtherCfg[0].u8DhcpOn != 0)
	{
		SendMessage(GetDlgItem(hWnd, IDC_STATIC_AUTODHCP), BM_SETCHECK, BST_CHECKED, 0);
		EnableDlgItem(hWnd, IDC_IPADDR, FALSE);
		EnableDlgItem(hWnd, IDC_IPMASK, FALSE);
		EnableDlgItem(hWnd, IDC_GATEWAY, FALSE);
		
		EnableDlgItem(hWnd, IDC_STATIC_AUTODNS, TRUE);
		if(stNetCfg.u8DnsDhcp != 0)
		{
			SendMessage(GetDlgItem(hWnd, IDC_STATIC_AUTODNS), BM_SETCHECK, BST_CHECKED, 0);
			EnableDlgItem(hWnd, IDC_DNS1, FALSE);
			EnableDlgItem(hWnd, IDC_DNS2, FALSE);
		}
		else
		{
			EnableDlgItem(hWnd, IDC_DNS1, TRUE);
			EnableDlgItem(hWnd, IDC_DNS2, TRUE);
		}
	}
	else
	{
		EnableDlgItem(hWnd, IDC_IPADDR, TRUE);
		EnableDlgItem(hWnd, IDC_IPMASK, TRUE);
		EnableDlgItem(hWnd, IDC_GATEWAY, TRUE);

		EnableDlgItem(hWnd, IDC_STATIC_AUTODNS, FALSE);
		EnableDlgItem(hWnd, IDC_DNS1, TRUE);
		EnableDlgItem(hWnd, IDC_DNS2, TRUE);
	}
	
	//dns1
	sprintf(acIp, "%d.%d.%d.%d", (stNetCfg.u32DnsIp1>>24)&0xff,
								 (stNetCfg.u32DnsIp1>>16)&0xff,
								 (stNetCfg.u32DnsIp1>>8)&0xff,
								 (stNetCfg.u32DnsIp1>>0)&0xff);
	hDNS1 = GetDlgItem (hWnd, IDC_DNS1);
	SetWindowText(hDNS1, acIp);

	//dns2
	sprintf(acIp, "%d.%d.%d.%d", (stNetCfg.u32DnsIp2>>24)&0xff,
								 (stNetCfg.u32DnsIp2>>16)&0xff,
								 (stNetCfg.u32DnsIp2>>8)&0xff,
								 (stNetCfg.u32DnsIp2>>0)&0xff);
	hDNS2 = GetDlgItem (hWnd, IDC_DNS2);
	SetWindowText(hDNS2, acIp);

	//http port
	sprintf(acTmp, "%d", stNetCfg.u16HttpPort);
	hHttp = GetDlgItem (hWnd, IDC_HTTPPORT);	
	SetWindowText(hHttp, acTmp);

	//data port
	sprintf(acTmp, "%d", stNetCfg.u16NetPort);
	hRtsp = GetDlgItem (hWnd, IDC_RTSPPORT);	
	SetWindowText(hRtsp, acTmp);
	
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

static void SaveNetParam(HWND hWnd)
{
	HWND hMac, hIP, hMask, hGateway, hDNS1, hDNS2;
	HWND hHttp, hRtsp;
	char acIp[32];
	char acTmp[8];
	uint8_t macAddr[6]; 
	NVR_DEV_NET_CFG_S pstNetCfg;

	memset(&pstNetCfg, 0, sizeof(NVR_DEV_NET_CFG_S));
	if(nvr_logon_get_net_cfg(0, &pstNetCfg) < 0)
	{
		printf("[error]<<<<<<<< %s %d",__func__,__LINE__);
		return ;
	}

	/*MAC地址*/
	hMac = GetDlgItem (hWnd, IDC_MAC_ADDR);
	GetWindowText(hMac, acIp, 32);
	mac_str_to_bin((char *)acIp, (char *)macAddr);
	memcpy((char *)pstNetCfg.struEtherCfg[0].u8MacAddr, (char *)macAddr, 6);
	
	int bChecked;
	
	bChecked = SendDlgItemMessage(hWnd, IDC_STATIC_AUTODHCP, BM_GETCHECK, 0, 0);
	if(bChecked)
		pstNetCfg.struEtherCfg[0].u8DhcpOn = 1;
	else 
		pstNetCfg.struEtherCfg[0].u8DhcpOn = 0;

	memset(acIp, 0, 32);
	hIP = GetDlgItem (hWnd, IDC_IPADDR);
	GetWindowText(hIP, acIp, 32);
	pstNetCfg.struEtherCfg[0].u32IpAddr = net_ip_a2n(acIp);
	
	memset(acIp, 0, 32);
	hMask= GetDlgItem (hWnd, IDC_IPMASK);
	GetWindowText(hMask, acIp, 32);
	pstNetCfg.struEtherCfg[0].u32SubMask = net_ip_a2n(acIp);


	memset(acIp, 0, 32);
	hGateway = GetDlgItem (hWnd, IDC_GATEWAY);
	GetWindowText(hGateway, acIp, 32);
	pstNetCfg.struEtherCfg[0].u32GateWay = net_ip_a2n(acIp);


	bChecked = SendDlgItemMessage(hWnd, IDC_STATIC_AUTODNS, BM_GETCHECK, 0, 0);
	if(bChecked)
		pstNetCfg.u8DnsDhcp = 1;
	else 
		pstNetCfg.u8DnsDhcp = 0;


	memset(acIp, 0, 32);
	hDNS1 = GetDlgItem (hWnd, IDC_DNS1);
	GetWindowText(hDNS1, acIp, 32);
	pstNetCfg.u32DnsIp1 = net_ip_a2n(acIp);

	memset(acIp, 0, 32);
	hDNS2 = GetDlgItem (hWnd, IDC_DNS2);
	GetWindowText(hDNS2, acIp, 32);
	pstNetCfg.u32DnsIp2 = net_ip_a2n(acIp);

    //http port
    memset(acTmp, 0, 8);
	hHttp = GetDlgItem (hWnd, IDC_HTTPPORT);	
	GetWindowText(hHttp, acTmp, 8);
	pstNetCfg.u16HttpPort = atoi(acTmp);

	//rtsp port
    memset(acTmp, 0, 8);
	hRtsp = GetDlgItem (hWnd, IDC_RTSPPORT);	
	GetWindowText(hRtsp, acTmp, 8);
	pstNetCfg.u16RtspPort = atoi(acTmp);

	if(nvr_logon_set_net_cfg(0, &pstNetCfg) < 0)
	{
		printf("[error]<<<<<<<< %s %d",__func__,__LINE__);
		return ;
	}
	
}

static void NetSettingCheckCmdPorc(HWND hDlg, int checkID)
{
	int bChecked;
	bChecked = SendDlgItemMessage(hDlg, checkID, BM_GETCHECK, 0, 0);
	if (bChecked)
	{
		//printf("id = %d checked\r\n", checkID);
		if(checkID == IDC_STATIC_AUTODHCP)
		{
			EnableDlgItem(hDlg, IDC_IPADDR, FALSE);
			EnableDlgItem(hDlg, IDC_IPMASK, FALSE);
			EnableDlgItem(hDlg, IDC_GATEWAY, FALSE);
			EnableDlgItem(hDlg, IDC_STATIC_AUTODNS, TRUE);
		}
		else if(checkID == IDC_STATIC_AUTODNS)
		{			
			EnableDlgItem(hDlg, IDC_DNS1, FALSE);
			EnableDlgItem(hDlg, IDC_DNS2, FALSE);
		}
	}
	else
	{			
		printf("id = %d uncheck\r\n", checkID);
		if (checkID == IDC_STATIC_AUTODHCP)
		{
			EnableDlgItem(hDlg, IDC_IPADDR, TRUE);
			EnableDlgItem(hDlg, IDC_IPMASK, TRUE);
			EnableDlgItem(hDlg, IDC_GATEWAY, TRUE);
			EnableDlgItem(hDlg, IDC_STATIC_AUTODNS, FALSE);
		}
		else if(checkID == IDC_STATIC_AUTODNS)
		{			
			EnableDlgItem(hDlg, IDC_DNS1, TRUE);
			EnableDlgItem(hDlg, IDC_DNS2, TRUE);
		}
	}
	
}

static void SetDefaultNetCfg(HWND hDlg)
{
	NVR_DEV_INFO_S stDevInfo;
	NVR_DEV_NET_CFG_S stNetCfg;	
	char acIp[32];
	char acTmp[8];

	memset(&stNetCfg, 0, sizeof(NVR_DEV_NET_CFG_S));
	if(nvr_logon_get_net_cfg(0, &stNetCfg) < 0)
	{
		printf("[error]-------->>>get default param faild\n");
		return;
	}

	memset(&stDevInfo, 0, sizeof(NVR_DEV_INFO_S));
	if(nvr_logon_get_dev_cfg(0, &stDevInfo) < 0)
	{
		printf("[error]-------->>>get default param faild\n");
		return;
	}

	//mac addr
	sprintf(acIp, "%02X:%02X:%02X:%02X:%02X:%02X", stDevInfo.stStableInfo.u8MacAddr[0][0],
								 stDevInfo.stStableInfo.u8MacAddr[0][1],
								 stDevInfo.stStableInfo.u8MacAddr[0][2],
								 stDevInfo.stStableInfo.u8MacAddr[0][3],
								 stDevInfo.stStableInfo.u8MacAddr[0][4],
								 stDevInfo.stStableInfo.u8MacAddr[0][5]);
	SetDlgItemText(hDlg, IDC_MAC_ADDR, acIp);

	//local ipaddr
	sprintf(acIp, "%d.%d.%d.%d", (stDevInfo.stStableInfo.u32IPAddr[0]>>24)&0xff,
								 (stDevInfo.stStableInfo.u32IPAddr[0]>>16)&0xff,
								 (stDevInfo.stStableInfo.u32IPAddr[0]>>8)&0xff,
								 (stDevInfo.stStableInfo.u32IPAddr[0]>>0)&0xff);
	SetDlgItemText(hDlg, IDC_IPADDR, acIp);

	//netmask
	sprintf(acIp, "%d.%d.%d.%d", (stDevInfo.stStableInfo.u32NetMask[0]>>24)&0xff,
								 (stDevInfo.stStableInfo.u32NetMask[0]>>16)&0xff,
								 (stDevInfo.stStableInfo.u32NetMask[0]>>8)&0xff,
								 (stDevInfo.stStableInfo.u32NetMask[0]>>0)&0xff);
	SetDlgItemText(hDlg, IDC_IPMASK, acIp);
	//printf("InitNetParam Mask = %s\r\n", acIp);
	
	//gateway
	sprintf(acIp, "%d.%d.%d.%d", (stDevInfo.stStableInfo.u32GateWay[0]>>24)&0xff,
								 (stDevInfo.stStableInfo.u32GateWay[0]>>16)&0xff,
								 (stDevInfo.stStableInfo.u32GateWay[0]>>8)&0xff,
								 (stDevInfo.stStableInfo.u32GateWay[0]>>0)&0xff);
	//printf("InitNetParam Gateway = %s\r\n", acIp);
	SetDlgItemText(hDlg, IDC_GATEWAY, acIp);

	//printf("u8DHCP:%d\n", stNetCfg.struEtherCfg[0].u8DhcpOn);
	if(stNetCfg.struEtherCfg[0].u8DhcpOn != 0)
	{
		printf("[info]<<<<<<<<< check dhcp = 1\n");
		SendMessage(GetDlgItem(hDlg, IDC_STATIC_AUTODHCP), BM_SETCHECK, BST_CHECKED, 0);
		EnableDlgItem(hDlg, IDC_IPADDR, FALSE);
		EnableDlgItem(hDlg, IDC_IPMASK, FALSE);
		EnableDlgItem(hDlg, IDC_GATEWAY, FALSE);
		
		EnableDlgItem(hDlg, IDC_STATIC_AUTODNS, TRUE);
		if(stNetCfg.u8DnsDhcp != 0)
		{
			printf("[info]<<<<<<<<< check u8DnsDhcp = 1\n");
			SendMessage(GetDlgItem(hDlg, IDC_STATIC_AUTODNS), BM_SETCHECK, BST_CHECKED, 0);
			EnableDlgItem(hDlg, IDC_DNS1, FALSE);
			EnableDlgItem(hDlg, IDC_DNS2, FALSE);
		}
		else
		{
			printf("[info]<<<<<<<<< check u8DnsDhcp = 0\n");
			EnableDlgItem(hDlg, IDC_DNS1, TRUE);
			EnableDlgItem(hDlg, IDC_DNS2, TRUE);
		}
	}
	else
	{
		printf("[info]<<<<<<<<< check dhcp=0\n");
		EnableDlgItem(hDlg, IDC_IPADDR, TRUE);
		EnableDlgItem(hDlg, IDC_IPMASK, TRUE);
		EnableDlgItem(hDlg, IDC_GATEWAY, TRUE);

		EnableDlgItem(hDlg, IDC_STATIC_AUTODNS, FALSE);
		EnableDlgItem(hDlg, IDC_DNS1, TRUE);
		EnableDlgItem(hDlg, IDC_DNS2, TRUE);
	}
	
	//dns1
	sprintf(acIp, "%d.%d.%d.%d", (stNetCfg.u32DnsIp1>>24)&0xff,
								 (stNetCfg.u32DnsIp1>>16)&0xff,
								 (stNetCfg.u32DnsIp1>>8)&0xff,
								 (stNetCfg.u32DnsIp1>>0)&0xff);
	//printf("hDNS1 = %s\r\n", acIp);
	SetDlgItemText(hDlg, IDC_DNS1, acIp);

	//dns2
	sprintf(acIp, "%d.%d.%d.%d", (stNetCfg.u32DnsIp2>>24)&0xff,
								 (stNetCfg.u32DnsIp2>>16)&0xff,
								 (stNetCfg.u32DnsIp2>>8)&0xff,
								 (stNetCfg.u32DnsIp2>>0)&0xff);
	//printf("hDNS2 = %s\r\n", acIp);
	SetDlgItemText(hDlg, IDC_DNS2, acIp);

	//http port
	sprintf(acTmp, "%d", stNetCfg.u16HttpPort);
	SetDlgItemText(hDlg, IDC_HTTPPORT, acTmp);

	//rtsp port
	sprintf(acTmp, "%d", stNetCfg.u16RtspPort);
	SetDlgItemText(hDlg, IDC_RTSPPORT, acTmp);
}

static int NetConfigWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    
    case MSG_INITDIALOG:
    {	
		setEditLength(hDlg); //限定端口号编辑框输入的字节数
		InitNetParam(hDlg);
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
       	HDC hdc;
       	hdc = BeginPaint (hDlg);		
       	EndPaint (hDlg, hdc);	
		Gui_DrawCaption(hDlg, "网络设置");
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
		Gui_WriteTitleName(hdc, &net_config_title_rect, LTR("网络设置"));
		if (fGetDC) 
			ReleaseDC (hdc); 
		return 0; 
	}
	break;

	case MSG_RBUTTONUP:
    {
		PostMessage(hDlg, MSG_CLOSE, 0, 0);
		
        break;
    }

    case MSG_COMMAND:
    {
		switch (wParam) 
		{
			case IDC_STATIC_AUTODHCP:
			case IDC_STATIC_AUTODNS:
			{
				NetSettingCheckCmdPorc(hDlg, wParam);
				break;
			}
			case IDC_NET_SAVE:
			{
				SaveNetParam(hDlg);
				//PostMessage(hDlg, MSG_CLOSE, 0, 0);
				SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
				return 0;
			}
			
			case IDC_NET_PREV:
			case IDC_NET_CANCEL:
			{
				PostMessage(hDlg, MSG_CLOSE, 0, 0);
				return 0;
			}
			//case IDC_NET_SERVER_CFG:
			{
			//	CreateNetServeDlgWin(hDlg);
			//	break;
			} 
			case IDC_NET_DEFAULT:
			{
				SetDefaultNetCfg(hDlg);
				return 0;
			}
			case IDC_DDNS_CONFIG:
			{
				CreateDdnsSetDlgWin(hDlg);
				return 0;
			}
			case IDC_UPNP_CONFIG:
			{
				CreateUpnpSetDlgWin(hDlg);
				return 0;
			}
			case IDC_PPPOE_CONFIG:
			{
				CreatePppoeSetDlgWin(hDlg);
				return 0;
			}

			case IDC_NET_NEXT:
			{				
				SaveNetParam(hDlg);
				CreateRemoteDeviceDlgWin(hDlg);
			}
			break;

			case IDC_NET_WIZARD_CANCEL:
			{
				BroadcastMessage(MSG_WIZARD_EXIT, 0, 0);
			}
			break;
		}
		break;
    }
   	case MSG_WIZARD_EXIT:
	{		
		PostMessage(hDlg, MSG_CLOSE, 0 , 0);
	}
	break;
    case MSG_CLOSE:
    {		
    	HWND hWind;
    	pop_open_dlg(&headOpenDlg, &hWind);
        EndDialog (hDlg, 0);
        break;
    }

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static void AdjustPositionDlgWin()
{
	int index,i;

	GetDisplayLanguageIndex(&index);
	if(1 == index) 
	{
		ui_net_control[1].x += 20; 
		ui_net_control[4].x += 20; 
		ui_net_control[6].x += 20;
		ui_net_control[8].x += 20; 

    	for(i=19; i<=21; i++)
    	{
			ui_net_control[i].w = THREE_WORDS_BUTTON_W;
			ui_net_control[i].dwAddData = (DWORD)&button_pic[1];
    	}
	}

}

void CreateNetCfgDlgWin(HWND hParent)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "NetDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_net_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_net_control, sizeof(ui_net_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_net_control)
	AdjustPositionDlgWin();
    NetDlg.controls = ui_net_control;
    
    DialogBoxIndirectParam (&NetDlg, hParent, NetConfigWinProc, 0L);
	memcpy(ui_net_control, ctrolTmp, sizeof(ui_net_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

