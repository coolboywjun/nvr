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
#include "ctrl_date.h"
#include "ctrl_ipaddress.h"
#include "nvr_lib_interface.h"

#include "gui_core.h"
#include "ui_dlg_head.h"

#define SETTIMER_DLG_W			490
#define SETTIMER_DLG_H			350
#define SETTIMER_DLG_X			((NVR_SCREEN_WIDTH - SETTIMER_DLG_W)>>1)
#define SETTIMER_DLG_Y			((NVR_SCREEN_HEIGHT - SETTIMER_DLG_H)>>1)
#define SETTIMER_TEXT_W			80
#define SETTIMER_EDIT_W			180
#define SETTIMER_EDIT_H			20
#define SETTIMER_BUTTON_W  		30
#define SETTIMER_TEXT_INC_Y		(SETTIMER_EDIT_H+10)
#define SETTIMER_TEXT_START_X	40
#define SETTIMER_TEXT_START_Y	60


typedef enum
{
	IDL_TIME_SELECT = 3200,
	IDC_BUTTON_TIME1,
	IDC_BUTTON_TIME2,
	IDC_BUTTON_TIME3,
	IDC_BUTTON_TIME4,
	IDC_CTRDATE_TIME1,
	IDC_CTRDATE_TIME2,
	IDC_CTRDATE_TIME3,
	IDC_CTRDATE_TIME4,
	IDC_BUTTON_SAVE,
	IDC_BUTTON_CANCEL,
}UI_TIME_SETTING_ID;

char *IrDayText[]=
{
	"每天",
	"星期日",
	"星期一",
	"星期二",
	"星期三",
	"星期四",
	"星期五",
	"星期六",
};

static RECT irset_title_rect = {0,0,SETTIMER_DLG_W,50};


static CTRLDATA ui_settimer_control[] =
{
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
       	SETTIMER_TEXT_START_X, SETTIMER_TEXT_START_Y, 100, GUI_COMBOX_H,
        IDL_TIME_SELECT,
        "每天",
        (sizeof(IrDayText)/sizeof(IrDayText[0]) >= 4) ? 80 : 0,
		WS_EX_USEPARENTFONT
     },

    //CtrlDate
     {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX,// | BS_CHECKED,
		SETTIMER_TEXT_START_X, SETTIMER_TEXT_START_Y+SETTIMER_TEXT_INC_Y, 80+40, 20,
        IDC_BUTTON_TIME1,
        "开启时间段1",
        0,
        WS_EX_TRANSPARENT
    },
     {
		CTRL_DATE,
		WS_TABSTOP | WS_VISIBLE | DE_TIME_SEG|DATE_NOTIFY|WS_TABSTOP,
		SETTIMER_TEXT_START_X, SETTIMER_TEXT_START_Y+2*SETTIMER_TEXT_INC_Y,
		150, 24,
		IDC_CTRDATE_TIME1, "12:00-14:00",
		6, 
		WS_EX_USEPARENTFONT | WS_EX_EDIT_NUMINPUT, 
	},
      {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX,// | BS_CHECKED,
		220, SETTIMER_TEXT_START_Y+SETTIMER_TEXT_INC_Y, 80+40, 20,
        IDC_BUTTON_TIME2,
        "开启时间段2",
        0,
        WS_EX_TRANSPARENT
    },
     {
		CTRL_DATE,
		WS_TABSTOP | WS_VISIBLE | DE_TIME_SEG|DATE_NOTIFY|WS_TABSTOP,
		220, SETTIMER_TEXT_START_Y+2*SETTIMER_TEXT_INC_Y, 150, 24,
		IDC_CTRDATE_TIME2, "12:00-14:00",
		6, 
		WS_EX_USEPARENTFONT | WS_EX_EDIT_NUMINPUT, 
	},
     {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX,// | BS_CHECKED,
		SETTIMER_TEXT_START_X, SETTIMER_TEXT_START_Y+3*SETTIMER_TEXT_INC_Y+5, 80+40, 20,
        IDC_BUTTON_TIME3,
        "开启时间段3",
        0,
        WS_EX_TRANSPARENT
    },
     {
		CTRL_DATE,
		WS_TABSTOP | WS_VISIBLE | DE_TIME_SEG|DATE_NOTIFY|WS_TABSTOP,
		SETTIMER_TEXT_START_X, SETTIMER_TEXT_START_Y+4*SETTIMER_TEXT_INC_Y+5,
		150, 24,
		IDC_CTRDATE_TIME3, "12:00-14:00",
		6, 
		WS_EX_USEPARENTFONT | WS_EX_EDIT_NUMINPUT, 
	},
    {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX,// | BS_CHECKED,
		220, SETTIMER_TEXT_START_Y+3*SETTIMER_TEXT_INC_Y+5, 80+40, 20,
        IDC_BUTTON_TIME4,
        "开启时间段4",
        0,
        WS_EX_TRANSPARENT
    },
     {
		CTRL_DATE,
		WS_TABSTOP | WS_VISIBLE | DE_TIME_SEG|DATE_NOTIFY|WS_TABSTOP,
		220, SETTIMER_TEXT_START_Y+4*SETTIMER_TEXT_INC_Y+5, 150, 24,
		IDC_CTRDATE_TIME4, "12:00-14:00",
		6, 
		WS_EX_USEPARENTFONT | WS_EX_EDIT_NUMINPUT, 
	},
	///////////////////////////////////////////////////////////
  {
        GUI_BUTTON,
        WS_CHILD | WS_VISIBLE | ES_PASSWORD | ES_AUTOSELECT|WS_TABSTOP | BS_BITMAP,
		SETTIMER_DLG_W-175, SETTIMER_DLG_H-50, TWO_WORDS_BUTTON_W, ALL_BUTTON_H,
        IDC_BUTTON_SAVE,
        "确定",
        (DWORD)&button_pic[0],
		WS_EX_TRANSPARENT
    },
    CONFIG_TWO_WORDS_BUTTON("取消", IDC_BUTTON_CANCEL, SETTIMER_DLG_W-100, SETTIMER_DLG_H-50),	 
};


//static BITMAP  bmp_bkgnd;
//static RECT SetTimerTileRect = {0,0,SETTIMER_DLG_W,40};

static DLGTEMPLATE SetTimerDlg =
{
    WS_NONE,
    WS_EX_NONE,
    SETTIMER_DLG_X, SETTIMER_DLG_Y, SETTIMER_DLG_W, SETTIMER_DLG_H,
    "图像颜色",
    0, 0,
    TABLESIZE(ui_settimer_control), NULL,
    0
};

static void InitIRTimerCombox(HWND hDlg)
{
	int i;
	int nLine = TABLESIZE(IrDayText);
	
	SendDlgItemMessage(hDlg,IDL_TIME_SELECT, CB_RESETCONTENT, 0, (LPARAM)0);
	for (i = 0; i < nLine; i++)
	{
		SendDlgItemMessage(hDlg, IDL_TIME_SELECT, CB_ADDSTRING, 0, (LPARAM)LTR(IrDayText[i]));
	}	
	SendDlgItemMessage(hDlg, IDL_TIME_SELECT, CB_SETCURSEL, 0, 0);	
}

static void UpdateIRTimer(HWND hDlg, int chan)
{
	int curDay;
	IPNC_DEV_IRCFMODE_CFG_S stIrcfMode;
	int i;
	time_segment stTime;

	memset(&stIrcfMode, 0, sizeof(IPNC_DEV_IRCFMODE_CFG_S));
	if(0 > nvr_get_ipnc_param(chan,  IPNC_CMD_IRCFMODE_CFG, &stIrcfMode, sizeof(IPNC_DEV_IRCFMODE_CFG_S)))
	{
		printf("[error]------>>get ipnc param failed!\n");
		return ;
	}

	curDay = SendDlgItemMessage(hDlg, IDL_TIME_SELECT, CB_GETCURSEL, 0, 0);
	if(curDay<0 || curDay>=TABLESIZE(IrDayText))
		return ;
	
	//printf("[info]----->>>curDay = %d\n",curDay);

	for(i=0; i<NVR_MAX_SEG_NUM; i++)
	{
		stTime.start_hour = stIrcfMode.struSeg[i][curDay].beginSec/3600;
		stTime.start_min = (stIrcfMode.struSeg[i][curDay].beginSec%3600)/60;
		
		stTime.stop_hour = stIrcfMode.struSeg[i][curDay].endSec/3600;
		stTime.stop_min = (stIrcfMode.struSeg[i][curDay].endSec%3600)/60;
		stTime.enable = 1;
		//printf("[info]----->>>stIrcfMode.struSeg[i][curDay].u8Open = %d\n",stIrcfMode.struSeg[i][curDay].u8Open);
		if(stIrcfMode.struSeg[i][curDay].u8Open)
			SendDlgItemMessage(hDlg, IDC_BUTTON_TIME1+i, BM_SETCHECK, BST_CHECKED, 0);	
		else
			SendDlgItemMessage(hDlg, IDC_BUTTON_TIME1+i, BM_SETCHECK, BST_UNCHECKED, 0);

		EnableDlgItem(hDlg, IDC_CTRDATE_TIME1+i, stIrcfMode.struSeg[i][curDay].u8Open);
		SendDlgItemMessage(hDlg, IDC_CTRDATE_TIME1+i, MSG_SET_TIMESEG, 0, (LPARAM)&stTime);		
	}
	
}

static void InitIRTimerParam(HWND hDlg, int chan)
{
	InitIRTimerCombox(hDlg);
	UpdateIRTimer(hDlg, chan);
}

static void checkOpenTimeSeg(HWND hDlg, unsigned int checkID)
{
	int bChecked;
	bChecked = SendDlgItemMessage(hDlg, checkID, BM_GETCHECK, 0, 0);

	if(bChecked)
	{
		EnableDlgItem(hDlg, IDC_CTRDATE_TIME1 + (checkID - IDC_BUTTON_TIME1), TRUE);
	}
	else
	{
		EnableDlgItem(hDlg, IDC_CTRDATE_TIME1 + (checkID - IDC_BUTTON_TIME1), FALSE);
	}
}

static void SaveTimeParam(HWND hDlg, int chan)
{
	int curDay;
	IPNC_DEV_IRCFMODE_CFG_S stIrcfMode;
	int i;
	time_segment stTime;
	int bChecked;

	memset(&stIrcfMode, 0, sizeof(IPNC_DEV_IRCFMODE_CFG_S));
	if(0 > nvr_get_ipnc_param(chan,  IPNC_CMD_IRCFMODE_CFG, &stIrcfMode, sizeof(IPNC_DEV_IRCFMODE_CFG_S)))
	{
		printf("[error]------>>get ipnc param failed!\n");
		return ;
	}

	curDay = SendDlgItemMessage(hDlg, IDL_TIME_SELECT, CB_GETCURSEL, 0, 0);
	if(curDay<0 || curDay>=TABLESIZE(IrDayText))
		return ;
	
	//printf("[info]----->>>curDay = %d\n",curDay);

	for(i=0; i<NVR_MAX_SEG_NUM; i++)
	{
		memset(&stTime, 0, sizeof(LPARAM));
		SendDlgItemMessage(hDlg, IDC_CTRDATE_TIME1+i, MSG_GET_TIMESEG, 0, (LPARAM)&stTime);
		stIrcfMode.struSeg[i][curDay].beginSec = stTime.start_hour*3600+stTime.start_min*60;
		stIrcfMode.struSeg[i][curDay].endSec = stTime.stop_hour*3600+stTime.stop_min*60 + 59;
		bChecked = SendDlgItemMessage(hDlg, IDC_BUTTON_TIME1+i, BM_GETCHECK, 0, 0);
		if (bChecked)
			stIrcfMode.struSeg[i][curDay].u8Open = 1;
		else	
			stIrcfMode.struSeg[i][curDay].u8Open = 0;

		//printf("[info]--------->>>stIrcfMode.struSeg[i][curDay].beginSec:%d\n",stIrcfMode.struSeg[i][curDay].beginSec);

	}
	if(0 > nvr_set_ipnc_param(chan, IPNC_CMD_IRCFMODE_CFG, &stIrcfMode, sizeof(IPNC_DEV_IRCFMODE_CFG_S)))
	{
		printf("[error]--------->>>set ipnc param failed!\n");
	}
	
	
}

static int SetTimerWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	static int chn = -1;
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		if (lParam >=0 &&lParam < 4)
			chn = lParam;
		InitIRTimerParam(hDlg,chn);   
		push_open_dlg(&headOpenDlg, hDlg);
	   	break;
    }
#if 0	
	case MSG_PAINT:
	{
	
		HDC hdc;
        hdc = BeginPaint(hDlg);			
        EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg, "红外开启时段设置");
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
		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bk_pic[SIZE_490x350]); 
		Gui_WriteTitleName(hdc, &irset_title_rect, LTR("红外开启时段设置"));
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
		int id   = LOWORD(wParam);
        int code = HIWORD(wParam);
      
		if (id == IDL_TIME_SELECT)
		{			
			if (code == CBN_SELCHANGE)
			{	
				printf("[info]<<<<<<<<<%s %s %d\n",__FILE__,__func__,__LINE__);
				UpdateIRTimer(hDlg, chn);
			}
			return 0;
		}
				
		switch (wParam) 
		{
			case IDC_BUTTON_TIME1:
			case IDC_BUTTON_TIME2:
			case IDC_BUTTON_TIME3:
			case IDC_BUTTON_TIME4:
				checkOpenTimeSeg(hDlg, wParam);
			return 0;
			
			case IDC_BUTTON_SAVE:
			{
				SaveTimeParam(hDlg, chn);
				//PostMessage(hDlg, MSG_CLOSE, 0, 0);
				SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
				break;
			}
			case IDC_BUTTON_CANCEL:
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

    return DefaultDialogProc (hDlg, message, wParam,lParam);
}


void CreateSetTimer(HWND hParent, int chn)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "SetTimerDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_settimer_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_settimer_control, sizeof(ui_settimer_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_settimer_control)
    SetTimerDlg.controls = ui_settimer_control;
    
    DialogBoxIndirectParam (&SetTimerDlg, hParent, SetTimerWinProc, chn);
	memcpy(ui_settimer_control, ctrolTmp, sizeof(ui_settimer_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}
