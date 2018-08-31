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

#include "gui_core.h"

#include "ui_dlg_head.h"

#define DDNS_DLG_W	490
#define DDNS_DLG_H	350
#define DDNS_DLG_X	((NVR_SCREEN_WIDTH - DDNS_DLG_W)>>1)
#define DDNS_DLG_Y	((NVR_SCREEN_HEIGHT - DDNS_DLG_H)>>1)
#define DDNS_TEXT_W	100
#define DDNS_EDIT_W	150
#define DDNS_EDIT_H	20

#define DDNS_BUTTON_W  100

#define DDNS_TEXT_INC_Y	(DDNS_EDIT_H+10)

#define DDNS_TEXT_START_X	40

#define DDNS_TEXT_START_Y	60


typedef enum
{
	IDC_STATIC_DDNS_TYPE = 1600,
	IDC_STATIC_DDNS_NAME,
	IDC_STATIC_DDNS_PASSWAD,
	IDC_STATIC_DDNS_ZONE,
	IDC_STATIC_DDNS_PORT,
	IDC_STATIC_DDNS_INTERVAL,
	IDC_EDIT_DDNS_RENAME,
	IDC_EDIT_DDNS_NAME,
	IDC_EDIT_DDNS_PASSWAD,
	IDC_EDIT_DDNS_PORT,
	IDC_COMBOX_DDNS_TYPE,
	IDC_COMBOX_DDNS_INTERVAL,
	IDC_BUTTON_DDNS_ENABLE,
	IDC_BUTTON_DDNS_SURE,
	IDC_BUTTON_DDNS_CANCEL,
}UI_DDNS_SETTING_ID;


//static BITMAP  bmp_bkgnd;
//static RECT DdnssetTileRect = {0,0,DDNS_DLG_W,40};

char *serverProvider[]={
	"dyndns",
	"3322"
};

char *updateTime[]={
	"1","2","3","4",
	"5","6","7","8",
	"9","10","11","12",
	"13","14","15","16",
	"17","18","19","20",
	"21","22","23","24",
	"25","26","27","28",
	"29","30","31","32",
	"33","34","35","36",
	"37","38","39","40",
	"41","42","43","44",
	"45","46","47","48"

};

static CTRLDATA ui_ddnsset_control[] =
{
	#if 1
	{
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,//| BS_CHECKED,
		DDNS_TEXT_START_X,DDNS_TEXT_START_Y, DDNS_BUTTON_W, DDNS_EDIT_H,
        IDC_BUTTON_DDNS_ENABLE,
        "启用DDNS",
        0,
        WS_EX_TRANSPARENT
    },
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        DDNS_TEXT_START_X, DDNS_TEXT_START_Y+DDNS_TEXT_INC_Y, DDNS_TEXT_W, DDNS_EDIT_H,
        IDC_STATIC_DDNS_TYPE,
        "服务提供者",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP,
        DDNS_TEXT_START_X+DDNS_TEXT_W, DDNS_TEXT_START_Y+DDNS_TEXT_INC_Y, DDNS_EDIT_W, GUI_COMBOX_H,
        IDC_COMBOX_DDNS_TYPE,
        "dyndns",
        (sizeof(serverProvider)/sizeof(serverProvider[0]) >= 4) ? 80 : 0,
		WS_EX_USEPARENTFONT
    },   
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        DDNS_TEXT_START_X, DDNS_TEXT_START_Y+2*DDNS_TEXT_INC_Y, DDNS_TEXT_W, DDNS_EDIT_H,
        IDC_STATIC_DDNS_PORT,
        "服务器端口",
        0,
		WS_EX_TRANSPARENT
    }, 
     {
        CTRL_MYEDIT,
        WS_CHILD | ES_LEFT | WS_VISIBLE,
        DDNS_TEXT_START_X+DDNS_TEXT_W, DDNS_TEXT_START_Y+2*DDNS_TEXT_INC_Y, DDNS_EDIT_W, DDNS_EDIT_H,
        IDC_EDIT_DDNS_PORT,
        "80",
        0,
		WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        DDNS_TEXT_START_X, DDNS_TEXT_START_Y+3*DDNS_TEXT_INC_Y, DDNS_TEXT_W, DDNS_EDIT_H,
        IDC_STATIC_DDNS_NAME,
        "主机域名",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
        WS_CHILD | ES_LEFT | WS_VISIBLE,
        DDNS_TEXT_START_X+DDNS_TEXT_W, DDNS_TEXT_START_Y+3*DDNS_TEXT_INC_Y, DDNS_EDIT_W, DDNS_EDIT_H,
        IDC_EDIT_DDNS_RENAME,
        "",
        0,
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT| WS_EX_USEPARENTFONT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        DDNS_TEXT_START_X, DDNS_TEXT_START_Y+4*DDNS_TEXT_INC_Y, DDNS_TEXT_W+20, DDNS_EDIT_H,
        IDC_STATIC_DDNS_PASSWAD,
        "用户名",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
        WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
        DDNS_TEXT_START_X+DDNS_TEXT_W, DDNS_TEXT_START_Y+4*DDNS_TEXT_INC_Y, DDNS_EDIT_W, DDNS_EDIT_H,
        IDC_EDIT_DDNS_PASSWAD,
        "",
        0,
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT| WS_EX_USEPARENTFONT
    },

    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        DDNS_TEXT_START_X, DDNS_TEXT_START_Y+5*DDNS_TEXT_INC_Y, DDNS_TEXT_W, DDNS_EDIT_H,
        IDC_STATIC_DDNS_ZONE,
        "用户密码",
        0,
		WS_EX_TRANSPARENT
    },
    
    {
        CTRL_MYEDIT,
        WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP|ES_PASSWORD,
        DDNS_TEXT_START_X+DDNS_TEXT_W, DDNS_TEXT_START_Y+5*DDNS_TEXT_INC_Y, DDNS_EDIT_W, DDNS_EDIT_H,
        IDC_EDIT_DDNS_NAME,
        "",
        0,
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT| WS_EX_USEPARENTFONT
    },

    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        DDNS_TEXT_START_X, DDNS_TEXT_START_Y+6*DDNS_TEXT_INC_Y, DDNS_TEXT_W+50, DDNS_EDIT_H,
        IDC_STATIC_DDNS_INTERVAL,
        "更新间隔",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP,
        DDNS_TEXT_START_X+DDNS_TEXT_W, DDNS_TEXT_START_Y+6*DDNS_TEXT_INC_Y, DDNS_EDIT_W, GUI_COMBOX_H,
        IDC_COMBOX_DDNS_INTERVAL,
        "1小时",
        (sizeof(updateTime)/sizeof(updateTime[0]) >= 4) ? 80 : 0,
		WS_EX_USEPARENTFONT
    },   

	//////////////////////////// GUI_BUTTON ////////////////////////////////////
	CONFIG_TWO_WORDS_BUTTON("确定", IDC_BUTTON_DDNS_SURE, DDNS_DLG_W - 160, DDNS_DLG_H - 50),
	CONFIG_TWO_WORDS_BUTTON("取消", IDC_BUTTON_DDNS_CANCEL, DDNS_DLG_W - 80 , DDNS_DLG_H - 50),

	#endif	
};


static DLGTEMPLATE DdnssetDlg =
{
    WS_NONE,
    WS_EX_NONE,
    DDNS_DLG_X, DDNS_DLG_Y, DDNS_DLG_W, DDNS_DLG_H,
    "录像控制",
    0, 0,
    TABLESIZE(ui_ddnsset_control), NULL,
    0
};

static void setEnableOrdisableswindow(HWND hDlg, BOOL fEnable)
{
	//printf("<<<<<<<<<<<fEnable = %d\n",fEnable);
	EnableDlgItem(hDlg, IDC_COMBOX_DDNS_TYPE, fEnable);
	EnableDlgItem(hDlg, IDC_EDIT_DDNS_RENAME, fEnable);
	EnableDlgItem(hDlg, IDC_EDIT_DDNS_PASSWAD, fEnable);
	EnableDlgItem(hDlg, IDC_EDIT_DDNS_NAME, fEnable);
	EnableDlgItem(hDlg, IDC_EDIT_DDNS_PORT, fEnable);
	EnableDlgItem(hDlg, IDC_COMBOX_DDNS_INTERVAL, fEnable);
}

static void InitDdnssetParam(HWND hDlg)
{
	NVR_DDNS_CFG_S DnnsCfg;
	int i;
	//char buffer[32];
	char acPort[8];

	memset(&DnnsCfg, 0, sizeof(NVR_DDNS_CFG_S));
	if(nvr_logon_get_ddns_cfg(0, &DnnsCfg) < 0)
	{
		printf("[error]--------->>>get ddns cfg error\n");
		return ;
	}

	printf("<<<<<<<<DnnsCfg.u8DDNSOpen = %d,\n",DnnsCfg.u8DDNSOpen);
	if(DnnsCfg.u8DDNSOpen)
	{
		SendDlgItemMessage(hDlg, IDC_BUTTON_DDNS_ENABLE, BM_SETCHECK, BST_CHECKED, 0);
		setEnableOrdisableswindow(hDlg, TRUE);

	}
	else
	{
		SendDlgItemMessage(hDlg, IDC_BUTTON_DDNS_ENABLE, BM_SETCHECK, BST_UNCHECKED, 0);
		setEnableOrdisableswindow(hDlg, FALSE);
	}

	i = DnnsCfg.u8DDNSType;
	if(i >= sizeof(serverProvider)/sizeof(serverProvider[0]))
		i = 1;
	SendDlgItemMessage(hDlg, IDC_COMBOX_DDNS_TYPE, CB_SETCURSEL, i, 0);	
	printf("<<<<<<<i = %d\n",i);

	printf("<<<<<<<<<<DnnsCfg.szDDNSUsr:%s\n",DnnsCfg.szDDNSAccount);
	SetDlgItemText(hDlg, IDC_EDIT_DDNS_RENAME, DnnsCfg.szDDNSAccount);//域名--注册名

	printf("<<<<<<<<<<DnnsCfg.szDDNSAccount:%s\n",DnnsCfg.szDDNSUsr);
	SetDlgItemText(hDlg, IDC_EDIT_DDNS_NAME, DnnsCfg.szDDNSUsr);//用户名称

	printf("<<<<<<<<<<DnnsCfg.szDDNSPsw:%s\n",DnnsCfg.szDDNSPsw);
	SetDlgItemText(hDlg, IDC_EDIT_DDNS_PASSWAD, DnnsCfg.szDDNSPsw);//密码


	sprintf(acPort, "%d", DnnsCfg.u16DDNSPort);
	printf("<<<<<<<<<acPort = %d\n",DnnsCfg.u16DDNSPort);
		
	SetDlgItemText(hDlg, IDC_EDIT_DDNS_PORT, acPort);//域名--注册名

	if(DnnsCfg.u32DDNSTimes>=1 && DnnsCfg.u32DDNSTimes<=sizeof(updateTime)/sizeof(updateTime[0]))
	{
		SendDlgItemMessage(hDlg, IDC_COMBOX_DDNS_INTERVAL, CB_SETCURSEL, DnnsCfg.u32DDNSTimes-1, 0);	
	}
	else
	{
		SendDlgItemMessage(hDlg, IDC_COMBOX_DDNS_INTERVAL, CB_SETCURSEL, sizeof(updateTime)/sizeof(updateTime[0])-1, 0);	
		printf("[error]--------->>>>Don't surport update time\n");
	}

}

static void InitDnnsParam(HWND hDlg)
{
	int i;
	char textBuf[32]={0};
	SendDlgItemMessage(hDlg,IDC_COMBOX_DDNS_TYPE, CB_RESETCONTENT, 0, (LPARAM)0);
	for(i=0; i<sizeof(serverProvider)/sizeof(serverProvider[0]); i++)
		SendDlgItemMessage(hDlg, IDC_COMBOX_DDNS_TYPE, CB_ADDSTRING, 0, (LPARAM)serverProvider[i]);
	SendDlgItemMessage(hDlg, IDC_COMBOX_DDNS_TYPE, CB_SETCURSEL, 0, 0);	

	SendDlgItemMessage(hDlg,IDC_COMBOX_DDNS_INTERVAL, CB_RESETCONTENT, 0, (LPARAM)0);
	for(i=0; i<sizeof(updateTime)/sizeof(updateTime[0]); i++)
	{
		sprintf(textBuf, "%d %s", atoi(updateTime[i]),LTR("小时"));
		//printf("...................%s\n",textBuf);
		SendDlgItemMessage(hDlg, IDC_COMBOX_DDNS_INTERVAL, CB_ADDSTRING, 0, (LPARAM)textBuf);
	}
	SendDlgItemMessage(hDlg, IDC_COMBOX_DDNS_INTERVAL, CB_SETCURSEL, 0, 0);	

	InitDdnssetParam(hDlg);
}


static void SaveDdnssetParam(HWND hDlg)
{
	NVR_DDNS_CFG_S DnnsCfg;
	char buffer[32]={0};
	char acPort[8];
	int checked,curIndex;
	HWND nameHwnd,pswHwnd,zoneHwnd,portHwnd;

	memset(&DnnsCfg, 0, sizeof(NVR_DDNS_CFG_S));
	checked = SendDlgItemMessage(hDlg, IDC_BUTTON_DDNS_ENABLE, BM_GETCHECK, 0, 0);
 	DnnsCfg.u8DDNSOpen = checked;

	curIndex = SendDlgItemMessage (hDlg, IDC_COMBOX_DDNS_TYPE, CB_GETCURSEL, 0, 0);
	//printf("<<<<<<<curIndex = %d\n",curIndex);
	DnnsCfg.u8DDNSType = curIndex; 

	zoneHwnd = GetDlgItem (hDlg, IDC_EDIT_DDNS_RENAME);
	GetWindowText(zoneHwnd, buffer, 32);
	memcpy(DnnsCfg.szDDNSAccount, buffer, 32);

	nameHwnd = GetDlgItem (hDlg, IDC_EDIT_DDNS_NAME);
	GetWindowText(nameHwnd, buffer, 32);
	memcpy(DnnsCfg.szDDNSUsr,buffer, 32);

	pswHwnd = GetDlgItem (hDlg, IDC_EDIT_DDNS_PASSWAD);
	GetWindowText(pswHwnd, buffer, 32);
	memcpy(DnnsCfg.szDDNSPsw, buffer, 32);

	portHwnd = GetDlgItem (hDlg, IDC_EDIT_DDNS_PORT);
	GetWindowText(portHwnd, acPort, 32);
	DnnsCfg.u16DDNSPort = atoi(acPort);

	curIndex = SendDlgItemMessage (hDlg, IDC_COMBOX_DDNS_INTERVAL, CB_GETCURSEL, 0, 0);
	DnnsCfg.u32DDNSTimes = curIndex + 1;
	if(nvr_logon_set_ddns_cfg(0, &DnnsCfg) < 0)
	{
		printf("[error]---------->>>set ddns cfg error\n");
	}
	
}

static void setEditLength(HWND hWnd)
{
	HWND nameHwnd,passwordHwnd,zoneHwnd,devPortHwnd;

	nameHwnd = GetDlgItem(hWnd, IDC_EDIT_DDNS_RENAME);
	SendMessage(nameHwnd, EM_LIMITTEXT, 32, 0L); //usename长度限定16个字节的字符

	passwordHwnd = GetDlgItem(hWnd, IDC_EDIT_DDNS_PASSWAD);
	SendMessage(passwordHwnd, EM_LIMITTEXT, 32, 0L); //password长度限定16个字节的字符

	zoneHwnd = GetDlgItem(hWnd, IDC_EDIT_DDNS_NAME);
	SendMessage(zoneHwnd, EM_LIMITTEXT, 32, 0L); //域名长度限定16个字节的字符

	devPortHwnd = GetDlgItem(hWnd, IDC_EDIT_DDNS_PORT);
	SendMessage(devPortHwnd, EM_LIMITTEXT, 5, 0L);
}


static int DdnsSetWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int bChecked;
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		InitDnnsParam(hDlg);
		setEditLength(hDlg);
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
       	hdc = BeginPaint (hDlg);		
       	EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg, "DDNS设置");
        break;
	}
#endif
	case MSG_ERASEBKGND:
	{
		RECT title = {0, 0, 490, 50};
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
		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bk_pic[SIZE_490x350]); 
		Gui_WriteTitleName(hdc, &title, LTR("DDNS设置"));
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
		switch(wParam)
		{
			case IDC_BUTTON_DDNS_ENABLE:
				bChecked = SendDlgItemMessage(hDlg, IDC_BUTTON_DDNS_ENABLE, BM_GETCHECK, 0, 0);
				if(bChecked)
				{
					setEnableOrdisableswindow(hDlg, TRUE);
				}
				else
				{
					setEnableOrdisableswindow(hDlg, FALSE);
				}

				break;
			case IDC_BUTTON_DDNS_SURE:
			{
				SaveDdnssetParam(hDlg);
				//PostMessage(hDlg, MSG_CLOSE, 0, 0);
				SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
				return 0;
			}
			case IDC_BUTTON_DDNS_CANCEL:
				PostMessage(hDlg, MSG_CLOSE, 0, 0);
				break;
		}
		
        break;
    }
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

//远程设备配置
void CreateDdnsSetDlgWin(HWND hParent)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "DdnsSetDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_ddnsset_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_ddnsset_control, sizeof(ui_ddnsset_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_ddnsset_control)
    DdnssetDlg.controls = ui_ddnsset_control;
    
    DialogBoxIndirectParam (&DdnssetDlg, hParent, DdnsSetWinProc, 0L);
	memcpy(ui_ddnsset_control, ctrolTmp, sizeof(ui_ddnsset_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

