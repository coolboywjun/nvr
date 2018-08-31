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
#include "gui_core.h"
#include "ctrl_date.h"
#include "nvr_lib_interface.h"


#define NORMAL_PARAM_DLG_W	490
#define NORMAL_PARAM_DLG_H	350
#define NORMAL_PARAM_DLG_X	((NVR_SCREEN_WIDTH - NORMAL_PARAM_DLG_W)>>1)
#define NORMAL_PARAM_DLG_Y	((NVR_SCREEN_HEIGHT - NORMAL_PARAM_DLG_H)>>1)
#define NORMAL_PARAM_TEXT_W	60
#define NORMAL_PARAM_EDIT_W	80
#define NORMAL_PARAM_EDIT_H	20

#define NORMAL_PARAM_INC_Y	(NORMAL_PARAM_EDIT_H+10)


#define NORMAL_PARAM_TEXT_START_X	40
#define NORMAL_PARAM_TEXT_START_Y	60

typedef enum
{
	IDC_NORMAL_PARAM_PREV = 1650,
	IDC_NORMAL_PARAM_NEXT,
	IDC_NORMAL_PARAM_CANCEL,
	IDC_NORMAL_PARAM_CURRENT_TIME,
	IDC_NORMAL_PARAM_TIME_SAVE,
	IDC_NORMAL_PARAM_ZONE,
	IDC_NORMAL_PARAM_LANGUAGE,
}UI_WIZARD_ID;

char *NormaltimeZone[]={
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


static CTRLDATA ui_normal_param_control[] =
{
	 {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        NORMAL_PARAM_TEXT_START_X, NORMAL_PARAM_TEXT_START_Y, NORMAL_PARAM_DLG_W - 2*NORMAL_PARAM_TEXT_START_X, 60,
        IDC_STATIC,
        "系统时间",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_DATE,
        WS_TABSTOP | WS_VISIBLE | DE_DATE_TIME|DATE_NOTIFY|WS_TABSTOP,
        NORMAL_PARAM_TEXT_START_X+NORMAL_PARAM_TEXT_W+40, NORMAL_PARAM_TEXT_START_Y, 180, 20,
        IDC_NORMAL_PARAM_CURRENT_TIME,
        "",
        6,
		WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
    },
	CONFIG_TWO_WORDS_BUTTON("保存", IDC_NORMAL_PARAM_TIME_SAVE, NORMAL_PARAM_DLG_W - 100, NORMAL_PARAM_TEXT_START_Y),
	///////////////////////////////////////////////////////////////////////
	{
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        NORMAL_PARAM_TEXT_START_X, NORMAL_PARAM_TEXT_START_Y+NORMAL_PARAM_INC_Y, NORMAL_PARAM_DLG_W - 2*NORMAL_PARAM_TEXT_START_X, 60,
        IDC_STATIC,
        "时区",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP,
        NORMAL_PARAM_TEXT_START_X+NORMAL_PARAM_TEXT_W+40, NORMAL_PARAM_TEXT_START_Y+NORMAL_PARAM_INC_Y, 180, GUI_COMBOX_H,
        IDC_NORMAL_PARAM_ZONE,
        "",
        (sizeof(NormaltimeZone)/sizeof(NormaltimeZone[0]) >= 4) ? 80 : 0,
		WS_EX_USEPARENTFONT
    },
	////////////////////////////////////////////////////////////////////////
/*	{
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        NORMAL_PARAM_TEXT_START_X, NORMAL_PARAM_TEXT_START_Y+2*NORMAL_PARAM_INC_Y, NORMAL_PARAM_DLG_W - 2*NORMAL_PARAM_TEXT_START_X, 60,
        IDC_STATIC,
        "选择语言",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP,
        NORMAL_PARAM_TEXT_START_X+NORMAL_PARAM_TEXT_W+40, NORMAL_PARAM_TEXT_START_Y+2*NORMAL_PARAM_INC_Y, 180, GUI_COMBOX_H,
        IDC_NORMAL_PARAM_LANGUAGE,
        "简体中文",
        80,
		WS_EX_USEPARENTFONT
    },
 */   ////////////////////////////////////////
	CONFIG_THREE_WORDS_BUTTON("上一步", IDC_NORMAL_PARAM_PREV, NORMAL_PARAM_DLG_W - 260, NORMAL_PARAM_DLG_H - 50),
    CONFIG_THREE_WORDS_BUTTON("下一步", IDC_NORMAL_PARAM_NEXT, NORMAL_PARAM_DLG_W - 180, NORMAL_PARAM_DLG_H - 50),
    CONFIG_TWO_WORDS_BUTTON("取消", IDC_NORMAL_PARAM_CANCEL, NORMAL_PARAM_DLG_W - 100 , NORMAL_PARAM_DLG_H - 50),
};


static DLGTEMPLATE NormalParamDlg =
{
    WS_NONE,
    WS_EX_NONE,
    NORMAL_PARAM_DLG_X, NORMAL_PARAM_DLG_Y, NORMAL_PARAM_DLG_W, NORMAL_PARAM_DLG_H,
    "开机向导",
    0, 0,
    TABLESIZE(ui_normal_param_control), NULL,
    0
};


static void InitNormalParam(HWND hDlg)
{
	NVR_NTP_CFG_S pCfg;
	int i = 0;
	char acText[16];
	memset(acText,0, sizeof(acText));

	memset(&pCfg, 0, sizeof(NVR_NTP_CFG_S));
	nvr_logon_get_ntp_cfg(0, &pCfg);
		
	SendDlgItemMessage(hDlg,IDC_NORMAL_PARAM_ZONE, CB_RESETCONTENT, 0, (LPARAM)0);
	for(i=0; i<sizeof(NormaltimeZone)/sizeof(NormaltimeZone[0]); i++)
		SendDlgItemMessage(hDlg, IDC_NORMAL_PARAM_ZONE, CB_ADDSTRING, 0, (LPARAM)NormaltimeZone[i]);

	if(pCfg.nTimeZone/3600 < 0)
		SendDlgItemMessage(hDlg, IDC_NORMAL_PARAM_ZONE, CB_SETCURSEL, 13+(-1)*pCfg.nTimeZone/3600, 0);	
	else 
		SendDlgItemMessage(hDlg, IDC_NORMAL_PARAM_ZONE, CB_SETCURSEL, pCfg.nTimeZone/3600, 0);

#if 0	
	SendDlgItemMessage(hDlg,IDC_NORMAL_PARAM_LANGUAGE, CB_RESETCONTENT, 0, (LPARAM)0);
	SendDlgItemMessage(hDlg, IDC_NORMAL_PARAM_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)LTR("简体中文"));
	SendDlgItemMessage(hDlg, IDC_NORMAL_PARAM_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)LTR("English"));	
	SendDlgItemMessage(hDlg, IDC_NORMAL_PARAM_LANGUAGE, CB_SETCURSEL, 0, 0);
#endif	
	ctrl_time_t stDate;
	time_t	tTime  = 0;
	struct tm *pTmTime;
	time(&tTime);
	pTmTime = localtime(&tTime);
	stDate.year= pTmTime->tm_year+1900;
	stDate.mon = pTmTime->tm_mon+1;
	stDate.day = pTmTime->tm_mday;
	stDate.hour = pTmTime->tm_hour;
	stDate.min = pTmTime->tm_min;
	stDate.sec = pTmTime->tm_sec;
	SendDlgItemMessage(hDlg, IDC_NORMAL_PARAM_CURRENT_TIME, MSG_SET_DATE, 0, (LPARAM)&stDate);
	
}


static void SaveNormalParam(HWND hDlg)
{
	
	NVR_NTP_CFG_S pCfg;
	int32_t curIndex, u32TimeZone;

	nvr_logon_get_ntp_cfg(0, &pCfg);

	curIndex = SendDlgItemMessage (hDlg, IDC_NORMAL_PARAM_ZONE, CB_GETCURSEL, 0, 0);
	if (curIndex < 0 || curIndex >=sizeof(NormaltimeZone)/sizeof(NormaltimeZone[0])) 
		return ;
	if(curIndex<14 && curIndex>=0)
		u32TimeZone = curIndex*3600;
	else
		u32TimeZone = (13-curIndex)*3600;
	pCfg.nTimeZone = u32TimeZone;	
	
	nvr_logon_set_ntp_cfg(0, &pCfg);
}

static void SaveSysTime(HWND hDlg)
{
	ctrl_time_t stDate;
	SendDlgItemMessage(hDlg, IDC_NORMAL_PARAM_CURRENT_TIME, MSG_GET_DATE, 0, (LPARAM)&stDate);
	NVR_TIME_S stTime;
	stTime.u8Year = stDate.year - 1900;
	stTime.u8Month = stDate.mon;
	stTime.u8Day = stDate.day;
	stTime.u8Hour = stDate.hour;
	stTime.u8Minute = stDate.min;
	stTime.u8Millisec = stDate.sec;
	nvr_logon_set_time_cfg(0, &stTime);
}

static int NormalParamWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
    {
		InitNormalParam(hDlg);
		push_open_dlg(&headOpenDlg, hDlg);
		break;
    }

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
		Gui_WriteTitleName(hdc, &title, LTR("普通参数"));
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
		case IDC_NORMAL_PARAM_PREV:			
			PostMessage(hDlg, MSG_CLOSE, 0, 0);
			break;
		case IDC_NORMAL_PARAM_NEXT:
			SaveNormalParam(hDlg);
			CreateNtpsetDlgWin(hDlg);
			return 0;
		case IDC_NORMAL_PARAM_CANCEL:
			BroadcastMessage(MSG_WIZARD_EXIT, 0 , 0);
			return 0;
		case IDC_NORMAL_PARAM_TIME_SAVE:			
			SaveSysTime(hDlg);
			break;
		}
		break;
    }

	case MSG_WIZARD_EXIT:
	{		
		PostMessage(hDlg, MSG_CLOSE, 0, 0);
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

//远程设备配置
void CreateNormalParamWin(HWND hParent)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "NormalParamDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_normal_param_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_normal_param_control, sizeof(ui_normal_param_control));

	DLG_CAPTION_LANGUAGE_TRANS(ui_normal_param_control)
    NormalParamDlg.controls = ui_normal_param_control;
    
    DialogBoxIndirectParam (&NormalParamDlg, hParent, NormalParamWinProc, 0L);
	memcpy(ui_normal_param_control, ctrolTmp, sizeof(ui_normal_param_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}
