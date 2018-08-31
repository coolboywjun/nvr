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

#define UPNP_DLG_W	490
#define UPNP_DLG_H	350
#define UPNP_DLG_X	((NVR_SCREEN_WIDTH - UPNP_DLG_W)>>1)
#define UPNP_DLG_Y	((NVR_SCREEN_HEIGHT - UPNP_DLG_H)>>1)
#define UPNP_TEXT_W	100
#define UPNP_EDIT_W	160

#define UPNP_LEFT_W		70
#define UPNP_RIGHT_W	60
#define UPNP_STATE_W	50

#define UPNP_EDIT_H	20

#define UPNP_BUTTON_W  100

#define UPNP_TEXT_INC_Y	(UPNP_EDIT_H+10)

#define UPNP_TEXT_START_X	40

#define UPNP_TEXT_START_Y	60


typedef enum
{
		IDC_STATIC_UPNP_TYPE = 1600,
		IDC_UPNP_ENABLE,

		IDC_UPNP_SERVERIP,
        IDC_UPNP_DATASTATE,
        IDC_UPNP_WEBSTATE,
        IDC_UPNP_COMMANDSTATE,
        IDC_UPNP_TALKSTATE,

		IDC_UPNP_REFRESH,
        IDC_UPNP_SURE,
        IDC_UPNP_CANCEL,
        
}UI_UPNP_SETTING_ID;

static CTRLDATA ui_upnpset_control[] =
{
	#if 1
	{
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,//| BS_CHECKED,
		UPNP_TEXT_START_X,UPNP_TEXT_START_Y, UPNP_BUTTON_W, UPNP_EDIT_H,
        IDC_UPNP_ENABLE,
        "启用UPNP",
        0,
        WS_EX_TRANSPARENT
    },

     {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UPNP_TEXT_START_X, UPNP_TEXT_START_Y+1*UPNP_TEXT_INC_Y, UPNP_TEXT_W, UPNP_EDIT_H,
        IDC_STATIC,
        "映射外网IP",
        0,
		WS_EX_TRANSPARENT
    },
     {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UPNP_TEXT_START_X+UPNP_TEXT_W, UPNP_TEXT_START_Y+1*UPNP_TEXT_INC_Y, UPNP_STATE_W, UPNP_EDIT_H,
        IDC_STATIC,
        "状态",
        0,
		WS_EX_TRANSPARENT
    },

    {
        CTRL_IP_ADDRESS,
        WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP|ES_READONLY,
        UPNP_TEXT_START_X+UPNP_TEXT_W+UPNP_STATE_W+5, UPNP_TEXT_START_Y+1*UPNP_TEXT_INC_Y, 160, UPNP_EDIT_H,
        IDC_UPNP_SERVERIP,
        "0.0.0.0",
        0,
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
    },	
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UPNP_TEXT_START_X, UPNP_TEXT_START_Y+2*UPNP_TEXT_INC_Y, UPNP_TEXT_W, UPNP_EDIT_H,
        IDC_STATIC,
        "数据端口",
        0,
		WS_EX_TRANSPARENT
    },

     {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UPNP_TEXT_START_X+UPNP_TEXT_W, UPNP_TEXT_START_Y+2*UPNP_TEXT_INC_Y, UPNP_STATE_W, UPNP_EDIT_H,
        IDC_STATIC,
        "状态",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
        WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP|ES_READONLY,
        UPNP_TEXT_START_X+UPNP_TEXT_W+UPNP_STATE_W+5, UPNP_TEXT_START_Y+2*UPNP_TEXT_INC_Y, UPNP_RIGHT_W, UPNP_EDIT_H,
        IDC_UPNP_DATASTATE,
        "0",
        0,
		WS_EX_EDIT_NUMINPUT| WS_EX_USEPARENTFONT
    },

    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UPNP_TEXT_START_X, UPNP_TEXT_START_Y+3*UPNP_TEXT_INC_Y, UPNP_TEXT_W+20, UPNP_EDIT_H,
        IDC_STATIC,
        "WEB 端口",
        0,
		WS_EX_TRANSPARENT
    },

        {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UPNP_TEXT_START_X+UPNP_TEXT_W, UPNP_TEXT_START_Y+3*UPNP_TEXT_INC_Y, UPNP_STATE_W, UPNP_EDIT_H,
        IDC_STATIC,
        "状态",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
        WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP|ES_READONLY,
        UPNP_TEXT_START_X+UPNP_TEXT_W+UPNP_STATE_W+5, UPNP_TEXT_START_Y+3*UPNP_TEXT_INC_Y, UPNP_RIGHT_W, UPNP_EDIT_H,
        IDC_UPNP_WEBSTATE,
        "0",
        0,
		WS_EX_EDIT_NUMINPUT| WS_EX_USEPARENTFONT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UPNP_TEXT_START_X, UPNP_TEXT_START_Y+4*UPNP_TEXT_INC_Y, UPNP_TEXT_W, UPNP_EDIT_H,
        IDC_STATIC,
        "命令端口",
        0,
		WS_EX_TRANSPARENT
    }, 
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UPNP_TEXT_START_X+UPNP_TEXT_W, UPNP_TEXT_START_Y+4*UPNP_TEXT_INC_Y, UPNP_STATE_W, UPNP_EDIT_H,
        IDC_STATIC,
        "状态",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
        WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP|ES_READONLY,
        UPNP_TEXT_START_X+UPNP_TEXT_W+UPNP_STATE_W+5, UPNP_TEXT_START_Y+4*UPNP_TEXT_INC_Y, UPNP_RIGHT_W, UPNP_EDIT_H,
        IDC_UPNP_COMMANDSTATE,
        "0",
        0,
		WS_EX_EDIT_NUMINPUT| WS_EX_USEPARENTFONT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UPNP_TEXT_START_X, UPNP_TEXT_START_Y+5*UPNP_TEXT_INC_Y, UPNP_TEXT_W, UPNP_EDIT_H,
        IDC_STATIC,
        "对讲端口",
        0,
		WS_EX_TRANSPARENT
    }, 

    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        UPNP_TEXT_START_X+UPNP_TEXT_W, UPNP_TEXT_START_Y+5*UPNP_TEXT_INC_Y, UPNP_STATE_W, UPNP_EDIT_H,
        IDC_STATIC,
        "状态",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
        WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP|ES_READONLY,
        UPNP_TEXT_START_X+UPNP_TEXT_W+UPNP_STATE_W+5, UPNP_TEXT_START_Y+5*UPNP_TEXT_INC_Y, UPNP_RIGHT_W, UPNP_EDIT_H,
        IDC_UPNP_TALKSTATE,
        "0",
        0,
		WS_EX_EDIT_NUMINPUT| WS_EX_USEPARENTFONT
    },

	//////////////////////////// GUI_BUTTON ////////////////////////////////////
	CONFIG_TWO_WORDS_BUTTON("刷新", IDC_UPNP_REFRESH, UPNP_TEXT_START_X, UPNP_DLG_H - 50),
	CONFIG_TWO_WORDS_BUTTON("确定", IDC_UPNP_SURE, UPNP_DLG_W - 160, UPNP_DLG_H - 50),
	CONFIG_TWO_WORDS_BUTTON("取消", IDC_UPNP_CANCEL, UPNP_DLG_W - 80 , UPNP_DLG_H - 50),

	#endif	
};


static DLGTEMPLATE UpnpsetDlg =
{
    WS_NONE,
    WS_EX_NONE,
    UPNP_DLG_X, UPNP_DLG_Y, UPNP_DLG_W, UPNP_DLG_H,
    "录像控制",
    0, 0,
    TABLESIZE(ui_upnpset_control), NULL,
    0
};

static void setEnableUpnpwindow(HWND hDlg, BOOL fEnable)
{
	EnableDlgItem(hDlg, IDC_UPNP_SERVERIP, fEnable);
	EnableDlgItem(hDlg, IDC_UPNP_DATASTATE, fEnable);
	EnableDlgItem(hDlg, IDC_UPNP_WEBSTATE, fEnable);	
	EnableDlgItem(hDlg, IDC_UPNP_COMMANDSTATE, fEnable);	
	EnableDlgItem(hDlg, IDC_UPNP_TALKSTATE, fEnable);
}

static void InitUpnpParam(HWND hDlg)
{
	NVR_UPNP_CFG_S upnpCfg;
	char acPort[8];

	memset(&upnpCfg, 0, sizeof(NVR_UPNP_CFG_S));
	if(nvr_logon_get_upnp_cfg(0, &upnpCfg) < 0)
	{
		printf("[error]--------->>>get upnp cfg error\n");
		return ;
	}

	if(upnpCfg.u8UpnpOpen)
	{
		SendDlgItemMessage(hDlg, IDC_UPNP_ENABLE, BM_SETCHECK, BST_CHECKED, 0);
		setEnableUpnpwindow(hDlg, TRUE);

	}
	else
	{
		SendDlgItemMessage(hDlg, IDC_UPNP_ENABLE, BM_SETCHECK, BST_UNCHECKED, 0);
		setEnableUpnpwindow(hDlg, FALSE);
	}
	
	SetDlgItemText(hDlg, IDC_UPNP_SERVERIP, upnpCfg.szExternalIp);//IP
	
	sprintf(acPort, "%d", upnpCfg.u16UpnpJXJDataPortS);
	SetDlgItemText(hDlg, IDC_UPNP_DATASTATE, acPort);//数据端口
	
	sprintf(acPort, "%d", upnpCfg.u16UpnpWebPortS);
	SetDlgItemText(hDlg, IDC_UPNP_WEBSTATE, acPort);//WEB端口


	sprintf(acPort, "%d", upnpCfg.u16UpnpCmdPortS);
	SetDlgItemText(hDlg, IDC_UPNP_COMMANDSTATE, acPort);//cmd端口
	
	sprintf(acPort, "%d", upnpCfg.u16UpnpTalkPortS);
	SetDlgItemText(hDlg, IDC_UPNP_TALKSTATE, acPort);//talk端口

}

static void SaveUpnpsetParam(HWND hDlg)
{
	NVR_UPNP_CFG_S upnpCfg;
	int checked;

	memset(&upnpCfg, 0, sizeof(NVR_UPNP_CFG_S));
	if(nvr_logon_get_upnp_cfg(0, &upnpCfg) < 0)
	{
		printf("[error]--------->>>get upnp cfg error\n");
		return ;
	}
	
	checked = SendDlgItemMessage(hDlg, IDC_UPNP_ENABLE, BM_GETCHECK, 0, 0);
 	upnpCfg.u8UpnpOpen = checked;

	if(nvr_logon_set_upnp_cfg(0, &upnpCfg) < 0)
	{
		printf("[error]---------->>>set upnp cfg error\n");
	}
	
}

static int UpnpSetWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int bChecked;
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		InitUpnpParam(hDlg);
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
       	hdc = BeginPaint (hDlg);		
       	EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg, "UPNP设置");
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
		Gui_WriteTitleName(hdc, &title, LTR("UPNP设置"));
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
			case IDC_UPNP_ENABLE:
				bChecked = SendDlgItemMessage(hDlg, IDC_UPNP_ENABLE, BM_GETCHECK, 0, 0);
				if(bChecked)
				{
					setEnableUpnpwindow(hDlg, TRUE);
				}
				else
				{
					setEnableUpnpwindow(hDlg, FALSE);
				}

				break;
			case IDC_UPNP_REFRESH:
			{
				InitUpnpParam(hDlg);
				InvalidateRect(hDlg, NULL, TRUE);
				return 0;
			}	
			case IDC_UPNP_SURE:
			{
				SaveUpnpsetParam(hDlg);
				SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
				return 0;
			}
			case IDC_UPNP_CANCEL:
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
void CreateUpnpSetDlgWin(HWND hParent)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "UpnpSetDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_upnpset_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_upnpset_control, sizeof(ui_upnpset_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_upnpset_control)
    UpnpsetDlg.controls = ui_upnpset_control;
    
    DialogBoxIndirectParam (&UpnpsetDlg, hParent, UpnpSetWinProc, 0L);
	memcpy(ui_upnpset_control, ctrolTmp, sizeof(ui_upnpset_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

