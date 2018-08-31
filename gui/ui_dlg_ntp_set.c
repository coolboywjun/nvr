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
#include "gui_core.h"
#include "ctrl_date.h"

#include "ui_dlg_head.h"

#define NTPSET_DLG_W	600
#define NTPSET_DLG_H	400
#define NTPSET_DLG_X	((NVR_SCREEN_WIDTH - NTPSET_DLG_W)>>1)
#define NTPSET_DLG_Y	((NVR_SCREEN_HEIGHT - NTPSET_DLG_H)>>1)
#define NTPSET_TEXT_W	100
#define NTPSET_EDIT_W	180
#define NTPSET_EDIT_H	20

#define NTPSET_BUTTON_W  30

#define NTPSET_TEXT_INC_Y	(NTPSET_EDIT_H+12)

#define NTPSET_TEXT_START_X	50

#define NTPSET_TEXT_START_Y	60

typedef enum
{
	IDC_STATIC_NTPSET_IP = 2400,
	IDC_STATIC_NTPSET_PORT,
	IDC_STATIC_NTPSET_TIME,
	IDC_STATIC_NTPSET_UPDATE,
	IDC_STATIC_NTPSET_MINUTE,
	IDC_BUTTON_NTPSET_DEFAULT,
	IDC_BUTTON_NTPSET_SURE,
	IDC_BUTTON_NTPSET_CANCEL,
	IDC_BUTTON_NTPSET_MANUAL_UPDATE,
	IDC_EDIT_NTPSET_IP,
	IDC_EDIT_NTPSET_PORT,
	IDC_EDIT_NTPSET_TIME,
	IDC_EDIT_NTPSET_UPDATE,
	IDC_EDIT_NTPSET_MINUTE,
	IDC_EDIT_CURRENT_TIME,
	IDC_TIME_MANUAL_SET,
	IDC_NTP_SET,
	IDC_NTP_SHOWTIME,
	IDC_NTP_PREV,
	IDC_NTP_NEXT,
	IDC_NTP_WIZARD_CANCEL,

}UI_NTP_SETTING_ID;


//static BITMAP  bmp_bkgnd;
//static RECT NtpsetTileRect = {0,0,NTPSET_DLG_W,40};

char *timeZone[]={
	"GMT+00:00",
	"GMT+01:00",
	"GMT+02:00",
	"GMT+03:00",
	"GMT+04:00",
	"GMT+05:00",
	"GMT+06:00",
	"GMT+07:00",
	"GMT+08:00",
	"GMT+09:00",
	"GMT+10:00",
	"GMT+11:00",
	"GMT+12:00",
	"GMT+13:00",
	"GMT-01:00",
	"GMT-02:00",
	"GMT-03:00",
	"GMT-04:00",
	"GMT-05:00",
	"GMT-06:00",
	"GMT-07:00",
	"GMT-08:00",
	"GMT-09:00",
	"GMT-10:00",
	"GMT-11:00",
	"GMT-12:00"
};

static CTRLDATA ui_ntpset_control[] =
{
	
	{
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,
		NTPSET_TEXT_START_X, NTPSET_TEXT_START_Y+0*NTPSET_TEXT_INC_Y, NTPSET_EDIT_W, NTPSET_EDIT_H,
        IDC_NTP_SET,
        "与NTP同步",
        0,
        WS_EX_TRANSPARENT
    },
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        NTPSET_TEXT_START_X, NTPSET_TEXT_START_Y+1*NTPSET_TEXT_INC_Y, NTPSET_TEXT_W, NTPSET_EDIT_H,
        IDC_STATIC_NTPSET_IP,
        "服务器IP",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        NTPSET_TEXT_START_X, NTPSET_TEXT_START_Y+2*NTPSET_TEXT_INC_Y, NTPSET_TEXT_W, NTPSET_EDIT_H,
        IDC_STATIC_NTPSET_PORT,
        "端口",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        NTPSET_TEXT_START_X, NTPSET_TEXT_START_Y+3*NTPSET_TEXT_INC_Y, NTPSET_TEXT_W, NTPSET_EDIT_H,
        IDC_STATIC_NTPSET_TIME,
        "时区",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        NTPSET_TEXT_START_X, NTPSET_TEXT_START_Y+4*NTPSET_TEXT_INC_Y, NTPSET_TEXT_W+20, NTPSET_EDIT_H,
        IDC_STATIC_NTPSET_UPDATE,
        "更新周期",
        0,
		WS_EX_TRANSPARENT
    },     
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        NTPSET_TEXT_START_X+NTPSET_TEXT_W+NTPSET_EDIT_W+10, NTPSET_TEXT_START_Y+4*NTPSET_TEXT_INC_Y, NTPSET_TEXT_W, NTPSET_EDIT_H,
        IDC_STATIC_NTPSET_MINUTE,
        "小时",
        0,
		WS_EX_TRANSPARENT
    },     
    ////////////////////////////////////////////////////////////////

	{
		CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		NTPSET_TEXT_START_X + NTPSET_TEXT_W, NTPSET_TEXT_START_Y+1*NTPSET_TEXT_INC_Y,
		NTPSET_EDIT_W, NTPSET_EDIT_H,
		IDC_EDIT_NTPSET_IP,
		"",
		0,		  
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT| WS_EX_USEPARENTFONT
	},
	{
		CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		NTPSET_TEXT_START_X+NTPSET_TEXT_W, NTPSET_TEXT_START_Y+2*NTPSET_TEXT_INC_Y, 
		NTPSET_EDIT_W, NTPSET_EDIT_H,
		IDC_EDIT_NTPSET_PORT,
		"",
		0,		  
		WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
	},
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP,
        NTPSET_TEXT_START_X+NTPSET_TEXT_W, NTPSET_TEXT_START_Y+3*NTPSET_TEXT_INC_Y, NTPSET_EDIT_W, GUI_COMBOX_H,
        IDC_EDIT_NTPSET_TIME,
        "",
        (sizeof(timeZone)/sizeof(timeZone[0]) >= 4) ? 80 : 0,
		WS_EX_USEPARENTFONT
    },
    {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
        NTPSET_TEXT_START_X+NTPSET_TEXT_W, NTPSET_TEXT_START_Y+4*NTPSET_TEXT_INC_Y, NTPSET_EDIT_W, NTPSET_EDIT_H,
        IDC_EDIT_NTPSET_UPDATE,
        "",
        0,
		WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
    },
    /////////////////////////////////////////////////////////////////

    {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,
		NTPSET_TEXT_START_X, NTPSET_TEXT_START_Y+5*NTPSET_TEXT_INC_Y, NTPSET_EDIT_W, NTPSET_EDIT_H,
        IDC_NTP_SHOWTIME,
        "显示NVR时间",
        0,
        WS_EX_TRANSPARENT
    },
	////////////////////////////////////////////////////////////////////////    
     {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        NTPSET_TEXT_START_X,  NTPSET_TEXT_START_Y+6*NTPSET_TEXT_INC_Y, NTPSET_TEXT_W, NTPSET_EDIT_H,
        IDC_STATIC,
        "当前时间",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_DATE,
        WS_TABSTOP | WS_VISIBLE | DE_DATE_TIME|DATE_NOTIFY|WS_TABSTOP,
        NTPSET_TEXT_START_X+NTPSET_TEXT_W, NTPSET_TEXT_START_Y+6*NTPSET_TEXT_INC_Y, NTPSET_EDIT_W, NTPSET_EDIT_H,
        IDC_EDIT_CURRENT_TIME,
        "",
        6,
		WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
    },

	
	////////////////////////// GUI_BUTTON //////////////////////////////////////
	CONFIG_THREE_WORDS_BUTTON("确定", IDC_BUTTON_NTPSET_SURE, NTPSET_DLG_W - 175, NTPSET_DLG_H - 50),
	CONFIG_THREE_WORDS_BUTTON("取消", IDC_BUTTON_NTPSET_CANCEL, NTPSET_DLG_W - 100, NTPSET_DLG_H- 50),

	CONFIG_THREE_WORDS_BUTTON("上一步", IDC_NTP_PREV, NTPSET_DLG_W - 250 , NTPSET_DLG_H - 50),
	CONFIG_THREE_WORDS_BUTTON("下一步", IDC_NTP_NEXT, NTPSET_DLG_W - 175 , NTPSET_DLG_H - 50),
	CONFIG_THREE_WORDS_BUTTON("取消", IDC_NTP_WIZARD_CANCEL, NTPSET_DLG_W - 100 , NTPSET_DLG_H - 50),

};


static DLGTEMPLATE NtpsetDlg =
{
    WS_NONE,
    WS_EX_NONE,
    NTPSET_DLG_X, NTPSET_DLG_Y, NTPSET_DLG_W, NTPSET_DLG_H,
    "录像控制",
    0, 0,
    TABLESIZE(ui_ntpset_control), NULL,
    0
};

static void setEditLength(HWND hWnd)
{
	HWND httpHwnd,updateHwnd;

	httpHwnd = GetDlgItem(hWnd, IDC_EDIT_NTPSET_PORT);
	SendMessage(httpHwnd, EM_LIMITTEXT, PORT_TEXT_LIMIT, 0L);
	updateHwnd = GetDlgItem(hWnd, IDC_EDIT_NTPSET_UPDATE);
	SendMessage(updateHwnd, EM_LIMITTEXT, 3, 0L);

}

static void switchDlgItem(HWND hDlg, BOOL fEnable)
{
	EnableDlgItem(hDlg, IDC_EDIT_NTPSET_IP, fEnable);
	EnableDlgItem(hDlg, IDC_EDIT_NTPSET_PORT, fEnable);
	EnableDlgItem(hDlg, IDC_EDIT_NTPSET_TIME, fEnable);
	EnableDlgItem(hDlg, IDC_EDIT_NTPSET_UPDATE, fEnable);
}

static void InitNtpsetParam(HWND hDlg)
{
	NVR_NTP_CFG_S pCfg;
	int i = 0;
	char acText[16];
	HWND hCtrl;
	
	if (NeedWizard())
	{
		hCtrl = GetDlgItem(hDlg, IDC_BUTTON_NTPSET_SURE);
		ShowWindow(hCtrl,SW_HIDE);
		hCtrl = GetDlgItem(hDlg, IDC_BUTTON_NTPSET_CANCEL);
		ShowWindow(hCtrl,SW_HIDE);
	}
	else
	{
		hCtrl = GetDlgItem(hDlg, IDC_NTP_PREV);
		ShowWindow(hCtrl,SW_HIDE);
		hCtrl = GetDlgItem(hDlg, IDC_NTP_NEXT);
		ShowWindow(hCtrl,SW_HIDE);
		hCtrl = GetDlgItem(hDlg, IDC_NTP_WIZARD_CANCEL);
		ShowWindow(hCtrl,SW_HIDE);
	}
	
	memset(acText,0, sizeof(acText));

	memset(&pCfg, 0, sizeof(NVR_NTP_CFG_S));
	nvr_logon_get_ntp_cfg(0, &pCfg);
	
	SetDlgItemText(hDlg, IDC_EDIT_NTPSET_IP, pCfg.szNtpServer);

	sprintf(acText, "%d", pCfg.u16NtpPort);
	SetDlgItemText(hDlg, IDC_EDIT_NTPSET_PORT, (char *)acText);
	
	SendDlgItemMessage(hDlg,IDC_EDIT_NTPSET_TIME, CB_RESETCONTENT, 0, (LPARAM)0);
	for(i=0; i<sizeof(timeZone)/sizeof(timeZone[0]); i++)
	SendDlgItemMessage(hDlg, IDC_EDIT_NTPSET_TIME, CB_ADDSTRING, 0, (LPARAM)timeZone[i]);

	if(pCfg.nTimeZone/3600 < 0)
		SendDlgItemMessage(hDlg, IDC_EDIT_NTPSET_TIME, CB_SETCURSEL, 13+(-1)*pCfg.nTimeZone/3600, 0);	
	else 
		SendDlgItemMessage(hDlg, IDC_EDIT_NTPSET_TIME, CB_SETCURSEL, pCfg.nTimeZone/3600, 0);	
	
	sprintf(acText, "%d", pCfg.u8NtpRefTime);
	SetDlgItemText(hDlg, IDC_EDIT_NTPSET_UPDATE, acText);
	
	if (pCfg.u8NtpOpen)
	{
		SendDlgItemMessage(hDlg, IDC_NTP_SET , BM_SETCHECK, BST_CHECKED, 0);
		switchDlgItem(hDlg, TRUE);
	}
	else	
	{
		SendDlgItemMessage(hDlg, IDC_NTP_SET , BM_SETCHECK, BST_UNCHECKED, 0);
		switchDlgItem(hDlg, FALSE);
	}

	NVR_DEV_OSD_CFG_S sOsdCfg;

	memset(&sOsdCfg, 0, sizeof(NVR_DEV_OSD_CFG_S));
	nvr_logon_get_osd_cfg(0, &sOsdCfg);
	//printf("@@@@@@@@@@@@@@sOsdCfg.u8EnableTimeOsd: %d\r\n",sOsdCfg.u8EnableTimeOsd);
	
	if(sOsdCfg.u8EnableTimeOsd == 1)
		SendDlgItemMessage(hDlg, IDC_NTP_SHOWTIME, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendDlgItemMessage(hDlg, IDC_NTP_SHOWTIME, BM_SETCHECK, BST_UNCHECKED, 0);
		
	ctrl_time_t stDate;
	time_t 	tTime  = 0;
	struct tm *pTmTime;
	time(&tTime);
	pTmTime = localtime(&tTime);
	stDate.year= pTmTime->tm_year+1900;
	stDate.mon = pTmTime->tm_mon+1;
	stDate.day = pTmTime->tm_mday;
	stDate.hour = pTmTime->tm_hour;
	stDate.min = pTmTime->tm_min;
	stDate.sec = pTmTime->tm_sec;
	SendDlgItemMessage(hDlg, IDC_EDIT_CURRENT_TIME, MSG_SET_DATE, 0, (LPARAM)&stDate);
}

static void SaveNtpsetParam(HWND hDlg)
{
	NVR_NTP_CFG_S pCfg;
	char acIP[16];
	int32_t curIndex, u32TimeZone;
	uint8_t u8Uptime;
	HWND hostIp,port,updateTime;

	memset(&pCfg, 0, sizeof(NVR_NTP_CFG_S));

	hostIp = GetDlgItem (hDlg, IDC_EDIT_NTPSET_IP);
	GetWindowText(hostIp, acIP, 16);
	printf("[info]severip: %s\n",acIP);
	strcpy(pCfg.szNtpServer, acIP);

	port = GetDlgItem (hDlg, IDC_EDIT_NTPSET_PORT);
	GetWindowText(port, acIP, 16);
	//printf("port %d", acIP);
	pCfg.u16NtpPort = atoi(acIP);

	//printf("[info]time: %d\n",pCfg.nTimeZone);
	//printf("[info]uptime: %d\n",pCfg.u8NtpRefTime);
#if 0
	timezone = GetDlgItem (hDlg, IDC_EDIT_NTPSET_TIME);
	GetWindowText(timezone, acIP, 16);
	printf("<<<<<<<< acIP:%s,u32TimeZone = %d\n",acIP);

	for(i=0; i<sizeof(timeZone)/sizeof(timeZone[0]); i++)
	{
		if(strcmp(timeZone[i], acIP) == 0)
		{
			printf("i = %d\n",i);
			if(i<14 && i>=0)
				u32TimeZone = i*3600;
			else
				u32TimeZone = (13-i)*3600;
				
		}
	}
	printf("<<<<<<<< acIP:%s,u32TimeZone = %d\n",acIP,u32TimeZone);
	pCfg.nTimeZone = u32TimeZone;
#endif
	curIndex = SendDlgItemMessage (hDlg, IDC_EDIT_NTPSET_TIME, CB_GETCURSEL, 0, 0);
	if (curIndex < 0 || curIndex >=sizeof(timeZone)/sizeof(timeZone[0])) 
		return ;
	//printf("<<<<<<<< curIndex = %d\n",curIndex);
	if(curIndex<14 && curIndex>=0)
		u32TimeZone = curIndex*3600;
	else
		u32TimeZone = (13-curIndex)*3600;
	pCfg.nTimeZone = u32TimeZone;
	//printf("<<<<<<<< u32TimeZone = %d\n",u32TimeZone);

	int bChecked;
	bChecked = SendDlgItemMessage(hDlg, IDC_NTP_SET, BM_GETCHECK, 0, 0);
	if (bChecked)
		pCfg.u8NtpOpen = 1;
	else
		pCfg.u8NtpOpen = 0;
	
	
	updateTime = GetDlgItem (hDlg, IDC_EDIT_NTPSET_UPDATE);
	GetWindowText(updateTime, acIP, 16);
	printf("updaetime: %s", acIP);
	u8Uptime = atoi(acIP);
	pCfg.u8NtpRefTime = u8Uptime;

	nvr_logon_set_ntp_cfg(0, &pCfg);

	ctrl_time_t stDate;
	SendDlgItemMessage(hDlg, IDC_EDIT_CURRENT_TIME, MSG_GET_DATE, 0, (LPARAM)&stDate);
	NVR_TIME_S stTime;
	stTime.u8Year = stDate.year - 1900;
	stTime.u8Month = stDate.mon;
	stTime.u8Day = stDate.day;
	stTime.u8Hour = stDate.hour;
	stTime.u8Minute = stDate.min;
	stTime.u8Second = stDate.sec;
	nvr_logon_set_time_cfg(0, &stTime);

}

static int NtpsetWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int bChecked;
	
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		setEditLength(hDlg); //设置端口号编辑框支持的长度5字节
		InitNtpsetParam(hDlg);
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
       	hdc = BeginPaint (hDlg);		
       	EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg, "NTP设置");
        break;
	}
#endif
	case MSG_ERASEBKGND:
	{
		RECT title = {0, 0, 600, 50};
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
		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bk_pic[SIZE_600x400]); 
		Gui_WriteTitleName(hdc, &title, LTR("时间设置"));
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
			case IDC_NTP_SET:
			{
				bChecked = SendDlgItemMessage(hDlg, IDC_NTP_SET, BM_GETCHECK, 0, 0);
				if(bChecked)
					switchDlgItem(hDlg, TRUE);
				else
					switchDlgItem(hDlg, FALSE);
				break;
			}
			case IDC_NTP_SHOWTIME:
			{
				bChecked = SendDlgItemMessage(hDlg, IDC_NTP_SHOWTIME, BM_GETCHECK, 0, 0);
				bChecked = (bChecked != 0) ? 1 : 0;
				NVR_DEV_OSD_CFG_S sOsdCfg;

				memset(&sOsdCfg, 0, sizeof(NVR_DEV_OSD_CFG_S));
				nvr_logon_get_osd_cfg(0, &sOsdCfg);
				sOsdCfg.u8EnableTimeOsd = bChecked;
				//printf("@@@@@@@@@@@@@@sOsdCfg.u8EnableTimeOsd: %d\r\n",sOsdCfg.u8EnableTimeOsd);
				nvr_logon_set_osd_cfg(0, &sOsdCfg);
			}
			break;
			case IDC_BUTTON_NTPSET_SURE:
			{
				SaveNtpsetParam(hDlg);
				//PostMessage(hDlg, MSG_CLOSE, 0, 0);
				SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
			}
			break;
			case IDC_NTP_PREV:
			case IDC_BUTTON_NTPSET_CANCEL:
			{
				PostMessage(hDlg, MSG_CLOSE, 0, 0);
				return 0;
			}
			case IDC_NTP_NEXT:
			{
				SaveNtpsetParam(hDlg);
				CreateNetCfgDlgWin(hDlg);
				return 0;
			}
			case IDC_NTP_WIZARD_CANCEL:
			{
				BroadcastMessage(MSG_WIZARD_EXIT, 0, 0);
				break;
			}

		//	case IDC_BUTTON_NTPSET_MANUAL_UPDATE:
		//		PostMessage(hDlg, MSG_CLOSE, 0, 0);
		//		break;
		}
		
        break;
    }
    case MSG_WIZARD_EXIT:
	{		
		PostMessage(hDlg, MSG_CLOSE, 0, 0);
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
void CreateNtpsetDlgWin(HWND hParent)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "NtpsetDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_ntpset_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_ntpset_control, sizeof(ui_ntpset_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_ntpset_control)
    NtpsetDlg.controls = ui_ntpset_control;
    
    DialogBoxIndirectParam (&NtpsetDlg, hParent, NtpsetWinProc, 0L);
	memcpy(ui_ntpset_control, ctrolTmp, sizeof(ui_ntpset_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

