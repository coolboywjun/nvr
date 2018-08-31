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
#include "ctrl_date.h"
#include "myedit.h"
#include "ctrl_ipaddress.h"
#include "nvr_lib_interface.h"
#include "gui_core.h"

#include "ui_dlg_head.h"

typedef enum
{
	IDC_AUTO_RESTART_SYS = 1200,
	IDL_DATE_SELECT,
	IDC_AT,
	IDL_TIME_SELECT,
	IDC_AUTO_DELETE_FILE,
	IDL_DELETE_SELECT,
	IDC_SURE,
	IDC_BUTTON_CANCEL,
	IDC_EDIT_BEFORE_DAY,
	IDC_BEFORE_DAY,
	IDC_ENABLE_MAINTAIN,
}UI_AUTO_MAINTAIN_ID;

#define AUTO_MAINTAIN_DLG_W	420
#define AUTO_MAINTAIN_DLG_H	180
#define AUTO_MAINTAIN_DLG_X	((NVR_SCREEN_WIDTH - AUTO_MAINTAIN_DLG_W)>>1)
#define AUTO_MAINTAIN_DLG_Y	((NVR_SCREEN_HEIGHT - AUTO_MAINTAIN_DLG_H)>>1)
#define AUTO_MAINTAIN_TEXT_W	60
#define AUTO_MAINTAIN_EDIT_W	150
#define AUTO_MAINTAIN_EDIT_H	20

#define AUTO_MAINTAIN_BUTTON_W  30

#define AUTO_MAINTAIN_TEXT_INC_Y	(AUTO_MAINTAIN_EDIT_H+15)

#define AUTO_MAINTAIN_TEXT_START_X	40

#define AUTO_MAINTAIN_TEXT_START_Y	60


//static BITMAP  bmp_bkgnd;
//static RECT AutoMainTileRect = {0,0,AUTO_MAINTAIN_DLG_W,40};

char *PerDayText[]=
{
	"每天",
	"每星期日",
	"每星期一",
	"每星期二",
	"每星期三",
	"每星期四",
	"每星期五",
	"每星期六",
};
char *TimeText[]=
{
	"00 : 00",
	"01 : 00",
	"02 : 00",
	"03 : 00",
	"04 : 00",
	"05 : 00",
	"06 : 00",
	"07 : 00",
	"08 : 00",
	"09 : 00",
	"10 : 00",
	"11 : 00",
	"12 : 00",
	"13 : 00",
	"14 : 00",
	"15 : 00",
	"16 : 00",
	"17 : 00",
	"18 : 00",
	"19 : 00",
	"20 : 00",
	"21 : 00",
	"22 : 00",
	"23 : 00"
};

static CTRLDATA ui_auto_maintain_control[] =
{
	{
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,
		AUTO_MAINTAIN_TEXT_START_X, AUTO_MAINTAIN_TEXT_START_Y, 70, 20,
        IDC_ENABLE_MAINTAIN,
        "启用",
        0,
        WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP,
       	AUTO_MAINTAIN_TEXT_START_X, AUTO_MAINTAIN_TEXT_START_Y+AUTO_MAINTAIN_TEXT_INC_Y, AUTO_MAINTAIN_EDIT_W+5, GUI_COMBOX_H,
        IDL_DATE_SELECT,
        "",
        (sizeof(PerDayText)/sizeof(PerDayText[0]) >= 4) ? 80 : 0,
        WS_EX_USEPARENTFONT
     },
	{
		"static",
        WS_CHILD | SS_LEFT | WS_VISIBLE,
       	AUTO_MAINTAIN_TEXT_START_X+AUTO_MAINTAIN_EDIT_W+10+4, AUTO_MAINTAIN_TEXT_START_Y+AUTO_MAINTAIN_TEXT_INC_Y+3, 20, AUTO_MAINTAIN_EDIT_H,
        IDC_AT,
        "在",
        0,
		WS_EX_TRANSPARENT
	},
	{
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP,
       	AUTO_MAINTAIN_TEXT_START_X+AUTO_MAINTAIN_EDIT_W+40, AUTO_MAINTAIN_TEXT_START_Y+AUTO_MAINTAIN_TEXT_INC_Y, AUTO_MAINTAIN_EDIT_W+5, GUI_COMBOX_H,
        IDL_TIME_SELECT,
        "00 : 00",
        (sizeof(TimeText)/sizeof(TimeText[0]) >= 4) ? 80 : 0,
		WS_EX_USEPARENTFONT
     },
    ///////////////////// GUI_BUTTON ////////////////////////

    CONFIG_TWO_WORDS_BUTTON("确定", IDC_SURE, AUTO_MAINTAIN_DLG_W/2 - 70 - 20, AUTO_MAINTAIN_DLG_H - 50),
    CONFIG_TWO_WORDS_BUTTON("取消", IDC_BUTTON_CANCEL, AUTO_MAINTAIN_DLG_W/2+20+15 , AUTO_MAINTAIN_DLG_H- 50),

};


static DLGTEMPLATE AutoMainDlg =
{
    WS_NONE,
    WS_EX_NONE,
    AUTO_MAINTAIN_DLG_X, AUTO_MAINTAIN_DLG_Y, AUTO_MAINTAIN_DLG_W, AUTO_MAINTAIN_DLG_H,
    "时间设置",
    0, 0,
    TABLESIZE(ui_auto_maintain_control), NULL,
    0
};

static void InitAutoMainParam(HWND hDlg)
{
	int i;
	int nLine = TABLESIZE(PerDayText);
	NVR_AUTO_MAINTAIN_CFG_S pCfg;

	memset(&pCfg, 0, sizeof(NVR_AUTO_MAINTAIN_CFG_S));
	
	if(nvr_logon_get_auto_cfg(0, &pCfg) < 0)
	{
		printf("[error]<<<<<<<< %s %d",__func__,__LINE__);
		return ;
	}
	
	//printf("<<<<<<<<<pCfg.u8WeekDay :%d\n",pCfg.u8WeekDay);
	//printf("<<<<<<<<<pCfg.u8Hour :%d\n",pCfg.u8Hour);
	//printf("<<<<<<<<<pCfg.u8Min :%d\n",pCfg.u8Min);

	if (pCfg.u8Enable)
	{		
		SendDlgItemMessage(hDlg, IDC_ENABLE_MAINTAIN, BM_SETCHECK, BST_CHECKED, 0);
		EnableDlgItem(hDlg,IDL_DATE_SELECT ,TRUE);
		EnableDlgItem(hDlg,IDL_TIME_SELECT ,TRUE);
	}
	else
	{		
		SendDlgItemMessage(hDlg, IDC_ENABLE_MAINTAIN, BM_SETCHECK, BST_UNCHECKED, 0);
		EnableDlgItem(hDlg,IDL_DATE_SELECT ,FALSE);
		EnableDlgItem(hDlg,IDL_TIME_SELECT ,FALSE);
	}

	
	SendDlgItemMessage(hDlg,IDL_DATE_SELECT, CB_RESETCONTENT, 0, (LPARAM)0);
	for (i = 0; i < nLine; i++)
	{
		SendDlgItemMessage(hDlg, IDL_DATE_SELECT, CB_ADDSTRING, 0, (LPARAM)LTR(PerDayText[i]));		
	}	
	SendDlgItemMessage(hDlg, IDL_DATE_SELECT, CB_SETCURSEL, pCfg.u8WeekDay, 0);	
	
	SendDlgItemMessage(hDlg,IDL_TIME_SELECT, CB_RESETCONTENT, 0, (LPARAM)0);
	nLine = TABLESIZE(TimeText);
	//printf("---------nLine :%d\n",nLine);
	for(i=0; i<nLine; i++)
		SendDlgItemMessage(hDlg, IDL_TIME_SELECT, CB_ADDSTRING, 0, (LPARAM)TimeText[i]);
	SendDlgItemMessage(hDlg, IDL_TIME_SELECT, CB_SETCURSEL, pCfg.u8Hour, 0);	
	SendDlgItemMessage(hDlg,IDL_DELETE_SELECT, CB_RESETCONTENT, 0, (LPARAM)0);
}

static void SaveAutoMaintainParam(HWND hDlg)
{
	int curDay;
	int curTime;	
	int bChecked;
	NVR_AUTO_MAINTAIN_CFG_S pCfg;
	
	curDay = SendDlgItemMessage (hDlg, IDL_DATE_SELECT, CB_GETCURSEL, 0, 0);
	if (curDay < 0 || curDay >=8) 
		return ;
	memset(&pCfg, 0, sizeof(NVR_AUTO_MAINTAIN_CFG_S));

	if(nvr_logon_get_auto_cfg(0, &pCfg) < 0)
	{
		printf("[error]<<<<<<<< %s %d",__func__,__LINE__);
		return ;
	}

	bChecked = SendDlgItemMessage(hDlg, IDC_ENABLE_MAINTAIN, BM_GETCHECK, 0, 0);
	if (bChecked == 1)
		pCfg.u8Enable = 1;
	else
		pCfg.u8Enable = 0;
	
	curTime = SendDlgItemMessage (hDlg, IDL_TIME_SELECT, CB_GETCURSEL, 0, 0);
	if (curTime < 0 || curTime >= 24) 
		return ;
	pCfg.u8WeekDay = curDay;
	pCfg.u8Hour = curTime;
	pCfg.u8Min = 0;

	if(nvr_logon_set_auto_cfg(0, &pCfg) < 0)
	{
		printf("[error]<<<<<<<< %s %d",__func__,__LINE__);
		return ;
	}
	
}

static int AutoMainWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		InitAutoMainParam(hDlg);	
    	push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);			
		Gui_DrawCaption(hDlg,  "自动维护");
        break;
	}
#endif
	case MSG_ERASEBKGND:
	{
		RECT title = {0, 0, 420, 50};
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
		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bk_pic[SIZE_420x180]); 
		Gui_WriteTitleName(hdc, &title, LTR("自动维护"));
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
			case IDC_SURE:
			{
				SaveAutoMaintainParam(hDlg);
				//PostMessage(hDlg, MSG_CLOSE, 0, 0);
				SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
				return 0;
			}
			case IDC_BUTTON_CANCEL:
			{
				PostMessage(hDlg, MSG_CLOSE, 0, 0);
				return 0;
			}
			case IDC_ENABLE_MAINTAIN:
			{
				int bChecked;
				bChecked = SendDlgItemMessage(hDlg, IDC_ENABLE_MAINTAIN, BM_GETCHECK, 0, 0);
				if (bChecked)
				{
					EnableDlgItem(hDlg,IDL_DATE_SELECT ,TRUE);
					EnableDlgItem(hDlg,IDL_TIME_SELECT ,TRUE);
				}
				else
				{
					EnableDlgItem(hDlg,IDL_DATE_SELECT ,FALSE);
					EnableDlgItem(hDlg,IDL_TIME_SELECT ,FALSE);
				}
			}
			return 0;
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
void CreateAutoMaintainDlgWin(HWND hParent)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "AutoMaintainDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_auto_maintain_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_auto_maintain_control, sizeof(ui_auto_maintain_control));

	DLG_CAPTION_LANGUAGE_TRANS(ui_auto_maintain_control);
    AutoMainDlg.controls = ui_auto_maintain_control;
    
    DialogBoxIndirectParam (&AutoMainDlg, hParent, AutoMainWinProc, 0L);
	memcpy(ui_auto_maintain_control, ctrolTmp, sizeof(ui_auto_maintain_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

