
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


#define RECORD_TIME_DLG_W	(490)
#define RECORD_TIME_DLG_H	(350)
#define RECORD_TIME_DLG_X	((NVR_SCREEN_WIDTH - RECORD_TIME_DLG_W)>>1)
#define RECORD_TIME_DLG_Y	((NVR_SCREEN_HEIGHT - RECORD_TIME_DLG_H)>>1)
#define LINE_0_X			(RECORD_TIME_DLG_W/2 - 100)
#define LINE_0_Y			60
#define TIME_RECT_X			((RECORD_TIME_DLG_W - TIME_RECT_W)/2)
#define TIME_RECT_Y			(LINE_0_Y+30)
#define TIME_RECT_W			(RECORD_TIME_DLG_W - 80)
#define TIME_RECT_H			160

#define SEG_START_X			(TIME_RECT_X+20)
#define SEG_START_Y			(TIME_RECT_Y+30)
#define SEG_INC_Y			30

static RECT record_time_title_rect = {0,0,RECORD_TIME_DLG_W,50};

typedef enum
{
	IDL_TIME_SELECT = 2700,
	IDC_ALL_DAY_CHECK,
	IDC_STARTTIME_1,
	IDC_STARTTIME_2,
	IDC_STARTTIME_3,
	IDC_STARTTIME_4,
	IDC_CHECK_STARTTIME_1,
	IDC_CHECK_STARTTIME_2,
	IDC_CHECK_STARTTIME_3,
	IDC_CHECK_STARTTIME_4,
	IDL_COPY_TIME_SELECT,
	IDC_COPY_DAY,
	IDC_SET_REC_SEG,
	IDC_SET_REC_CANCEL,
}UI_RECORD_STRATEGY_ID;


static CTRLDATA ui_record_time_control[] =
{
	 {
        "static",
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        LINE_0_X,  LINE_0_Y, 50, 21,
        IDC_STATIC,
        "星期",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
       	LINE_0_X+50, LINE_0_Y, 80+10, GUI_COMBOX_H,
        IDL_TIME_SELECT,
        "星期日",
        80,
		WS_EX_USEPARENTFONT
     },
     {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX,
		LINE_0_X+150+10, LINE_0_Y, 70+20, 20,
        IDC_ALL_DAY_CHECK,
        "全天",
        0,
        WS_EX_TRANSPARENT
    },
    //////////////////////////////////////////////////////////
	{
        "static",
        WS_CHILD | SS_LEFT | WS_VISIBLE | SS_GROUPBOX,
        TIME_RECT_X,  TIME_RECT_Y, TIME_RECT_W, TIME_RECT_H,
        IDC_STATIC,
        "",
        0,
		WS_EX_TRANSPARENT
    },
    //CtrlDate
     {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKED,
		SEG_START_X, SEG_START_Y, 90, 20,
        IDC_CHECK_STARTTIME_1,
        "时间段1",
        0,
        WS_EX_TRANSPARENT
    },
     {
		CTRL_DATE,
		WS_TABSTOP | WS_VISIBLE | DE_TIME_SEG|DATE_NOTIFY|WS_TABSTOP,
		SEG_START_X + 120, SEG_START_Y,
		150, 24,
		IDC_STARTTIME_1, "12:00-14:00",
		6, 
		WS_EX_USEPARENTFONT | WS_EX_EDIT_NUMINPUT, 
	},
     {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKED,
		SEG_START_X, SEG_START_Y+SEG_INC_Y, 90, 20,
        IDC_CHECK_STARTTIME_2,
        "时间段2",
        0,
        WS_EX_TRANSPARENT
    },
     {
		CTRL_DATE,
		WS_TABSTOP | WS_VISIBLE | DE_TIME_SEG|DATE_NOTIFY|WS_TABSTOP,
		SEG_START_X + 120, SEG_START_Y+SEG_INC_Y,
		150, 24,
		IDC_STARTTIME_2, "12:00-14:00",
		6, 
		WS_EX_USEPARENTFONT | WS_EX_EDIT_NUMINPUT, 
	},
     {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKED,
		SEG_START_X, SEG_START_Y+2*SEG_INC_Y, 90, 20,
        IDC_CHECK_STARTTIME_3,
        "时间段3",
        0,
        WS_EX_TRANSPARENT
    },
     {
		CTRL_DATE,
		WS_TABSTOP | WS_VISIBLE | DE_TIME_SEG|DATE_NOTIFY|WS_TABSTOP,
		SEG_START_X + 120, SEG_START_Y+2*SEG_INC_Y,
		150, 24,
		IDC_STARTTIME_3, "12:00-14:00",
		6, 
		WS_EX_USEPARENTFONT | WS_EX_EDIT_NUMINPUT, 
	},
    {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKED,
		SEG_START_X, SEG_START_Y+3*SEG_INC_Y, 90, 20,
        IDC_CHECK_STARTTIME_4,
        "时间段4",
        0,
        WS_EX_TRANSPARENT
    },
     {
		CTRL_DATE,
		WS_TABSTOP | WS_VISIBLE | DE_TIME_SEG|DATE_NOTIFY|WS_TABSTOP,
		SEG_START_X + 120, SEG_START_Y+3*SEG_INC_Y,
		150, 24,
		IDC_STARTTIME_4, "12:00-14:00",
		6, 
		WS_EX_USEPARENTFONT | WS_EX_EDIT_NUMINPUT, 
	},
    /////////////////////////////////////////////
     {
        "static",
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        LINE_0_X,  RECORD_TIME_DLG_H-80, 50, 20,
        IDC_STATIC,
        "复制到",
        0,
		WS_EX_TRANSPARENT
    },
     {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP,
       	LINE_0_X+50, RECORD_TIME_DLG_H-80, 80+10, GUI_COMBOX_H,
        IDL_COPY_TIME_SELECT,
        "星期日",
        80,
        WS_EX_USEPARENTFONT
     },
     CONFIG_TWO_WORDS_BUTTON("复制", IDC_COPY_DAY, LINE_0_X+150+10, RECORD_TIME_DLG_H-82),

    {
        GUI_BUTTON,
        WS_CHILD | WS_VISIBLE | ES_PASSWORD | ES_AUTOSELECT|WS_TABSTOP | BS_BITMAP,
		TIME_RECT_X, RECORD_TIME_DLG_H-50, TWO_WORDS_BUTTON_W, ALL_BUTTON_H,
        IDC_SET_REC_SEG,
        "确定",
        (DWORD)&button_pic[0],
		WS_EX_TRANSPARENT
    },
    CONFIG_TWO_WORDS_BUTTON("取消", IDC_SET_REC_CANCEL, TIME_RECT_X+TWO_WORDS_BUTTON_W+10, RECORD_TIME_DLG_H-50),
    
};


static DLGTEMPLATE RecordTimeDlg =
{
    WS_NONE,
    WS_EX_NONE,
    RECORD_TIME_DLG_X, RECORD_TIME_DLG_Y, RECORD_TIME_DLG_W, RECORD_TIME_DLG_H,
    "时间设置",
    0, 0,
    TABLESIZE(ui_record_time_control), NULL,
    0
};

char *DayText[]=
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

typedef enum{
	DISABLE_ALL_DAY = 0, //不选中全天combobox
	ENABLE_ALL_DAY,
}allDayFlag;

allDayFlag arrayFlag[8] = {0}; 

static void TimeSettingCheckCmdPorc(HWND hDlg, int checkID)
{
	int bChecked;
	bChecked = SendDlgItemMessage(hDlg, checkID, BM_GETCHECK, 0, 0);
	if (bChecked)
	{
		printf("id = %d checked\r\n", checkID);
		if (checkID == IDC_ALL_DAY_CHECK)
		{
			EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_1, FALSE);
			EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_2, FALSE);
			EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_3, FALSE);
			EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_4, FALSE);
			
			EnableDlgItem(hDlg, IDC_STARTTIME_1, FALSE);
			EnableDlgItem(hDlg, IDC_STARTTIME_2, FALSE);
			EnableDlgItem(hDlg, IDC_STARTTIME_3, FALSE);
			EnableDlgItem(hDlg, IDC_STARTTIME_4, FALSE);
		}
		else
		{			
			EnableDlgItem(hDlg, IDC_STARTTIME_1 + (checkID - IDC_CHECK_STARTTIME_1), TRUE);
		}
	}
	else
	{			
		printf("id = %d uncheck\r\n", checkID);
		if (checkID == IDC_ALL_DAY_CHECK)
		{
			EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_1, TRUE);
			EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_2, TRUE);
			EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_3, TRUE);
			EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_4, TRUE);
			
			EnableDlgItem(hDlg, IDC_STARTTIME_1, TRUE);
			EnableDlgItem(hDlg, IDC_STARTTIME_2, TRUE);
			EnableDlgItem(hDlg, IDC_STARTTIME_3, TRUE);
			EnableDlgItem(hDlg, IDC_STARTTIME_4, TRUE);
		}
		else
		{			
			EnableDlgItem(hDlg, IDC_STARTTIME_1 + (checkID - IDC_CHECK_STARTTIME_1), FALSE);
		}
	}
}


static void InitRecordTimeCombox(HWND hDlg)
{
	int i;
	int nLine = TABLESIZE(DayText);
	
	SendDlgItemMessage(hDlg,IDL_COPY_TIME_SELECT, CB_RESETCONTENT, 0, (LPARAM)0);
	SendDlgItemMessage(hDlg,IDL_TIME_SELECT, CB_RESETCONTENT, 0, (LPARAM)0);
	for (i = 0; i < nLine; i++)
	{
		SendDlgItemMessage(hDlg, IDL_COPY_TIME_SELECT, CB_ADDSTRING, 0, (LPARAM)LTR(DayText[i]));		
		SendDlgItemMessage(hDlg, IDL_TIME_SELECT, CB_ADDSTRING, 0, (LPARAM)LTR(DayText[i]));
	}	
	SendDlgItemMessage(hDlg, IDL_COPY_TIME_SELECT, CB_SETCURSEL, 0, 0);	
	SendDlgItemMessage(hDlg, IDL_TIME_SELECT, CB_SETCURSEL, 0, 0);	
}

static void UpdateDateCtrl(HWND hDlg, NVR_DEV_REC_STRC_S *pstRec)
{
	int i = 0;
	int begin_h, begin_m;
	int end_h, end_m;
	time_segment stTime;
	int bAlldayChecked = 0;
	int curDay;
	
	curDay = SendDlgItemMessage (hDlg, IDL_TIME_SELECT, CB_GETCURSEL, 0, 0);
	if (curDay < 0 || curDay >=8) 
		return ;
	
	for (i = 0; i < NVR_MAX_SEG_NUM; i++)	
	{
		begin_h = pstRec->struTimerSeg[i][curDay].beginSec/3600;
		begin_m = (pstRec->struTimerSeg[i][curDay].beginSec%3600)/60;
		end_h = pstRec->struTimerSeg[i][curDay].endSec/3600;
		end_m = (pstRec->struTimerSeg[i][curDay].endSec%3600)/60;
		stTime.enable = 1;
		stTime.start_hour = begin_h;
		stTime.start_min = begin_m;
		stTime.stop_hour = end_h;
		stTime.stop_min = end_m;
		
		if ((pstRec->struTimerSeg[i][curDay].beginSec == 0)&&
			(pstRec->struTimerSeg[i][curDay].endSec == 24*3600 -1)&&
			(pstRec->struTimerSeg[i][curDay].u8Open == 1))
		{
			bAlldayChecked = 1;
		}
		
		printf("%d:%d---%d:%d\r\n", begin_h, begin_m, end_h, end_m);
		if (pstRec->struTimerSeg[i][curDay].u8Open)
		{			
			SendDlgItemMessage(hDlg, IDC_CHECK_STARTTIME_1+i, BM_SETCHECK, BST_CHECKED, 0);
		}
		else
		{			
			SendDlgItemMessage(hDlg, IDC_CHECK_STARTTIME_1+i, BM_SETCHECK, BST_UNCHECKED, 0);
		}		
		EnableDlgItem(hDlg, IDC_STARTTIME_1+i, pstRec->struTimerSeg[i][curDay].u8Open);
		SendDlgItemMessage(hDlg, IDC_STARTTIME_1+i, MSG_SET_TIMESEG, 0, (LPARAM)&stTime);		
	}
	
	if (bAlldayChecked == 1)
	{		
		SendDlgItemMessage(hDlg, IDC_ALL_DAY_CHECK, BM_SETCHECK, BST_CHECKED, 0);
		arrayFlag[curDay] = ENABLE_ALL_DAY;
		
		EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_1, FALSE);
		EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_2, FALSE);
		EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_3, FALSE);
		EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_4, FALSE);
		
		EnableDlgItem(hDlg, IDC_STARTTIME_1, FALSE);
		EnableDlgItem(hDlg, IDC_STARTTIME_2, FALSE);
		EnableDlgItem(hDlg, IDC_STARTTIME_3, FALSE);
		EnableDlgItem(hDlg, IDC_STARTTIME_4, FALSE);
	}
	else
	{
		SendDlgItemMessage(hDlg, IDC_ALL_DAY_CHECK, BM_SETCHECK, BST_UNCHECKED, 0);
		arrayFlag[curDay] = DISABLE_ALL_DAY;

		EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_1, TRUE);
		EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_2, TRUE);
		EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_3, TRUE);
		EnableDlgItem(hDlg, IDC_CHECK_STARTTIME_4, TRUE);
	}
}



static void UpdateTimeParam(HWND hDlg, int ch)
{	
	int ret;
	printf("ch = %d\r\n", ch);
	NVR_DEV_REC_STRC_S stRec;
	
	memset(&stRec, 0, sizeof(NVR_DEV_REC_STRC_S));
	ret = nvr_logon_get_rec_cfg(ch, &stRec);
	if (ret < 0)
	{
		printf("get rec cfg failed\r\n");
	}	
	UpdateDateCtrl(hDlg, &stRec);
}


static void InitRecordTimeParam(HWND hDlg , int ch)
{
	InitRecordTimeCombox(hDlg);
	UpdateTimeParam(hDlg, ch);
}


static void CopyTimeSetting(HWND hDlg, int ch)
{
	//将srcDay录像设置拷贝到desDay	
	int ret, i;
	int srcDay, desDay;
	NVR_DEV_REC_STRC_S stRec;
	srcDay = SendDlgItemMessage (hDlg, IDL_TIME_SELECT, CB_GETCURSEL, 0, 0);
	if (srcDay < 0 || srcDay >= 8) 
		return;

	desDay = SendDlgItemMessage (hDlg, IDL_COPY_TIME_SELECT, CB_GETCURSEL, 0, 0);
	if (desDay < 0 || desDay >= 8) 
		return;

	memset(&stRec, 0, sizeof(NVR_DEV_REC_STRC_S));
	ret = nvr_logon_get_rec_cfg(ch, &stRec);
	if (ret < 0)
	{
		printf("get rec cfg failed\r\n");
	}

	printf("copy src = %d, copy des = %d\r\n", srcDay, desDay);
	
	for (i = 0; i < NVR_MAX_SEG_NUM; i++)
	{
		memcpy(&stRec.struTimerSeg[i][desDay], &stRec.struTimerSeg[i][srcDay], sizeof(NVR_SEGMENT_S));
	}

	if(0 > nvr_logon_set_rec_cfg(ch, &stRec))
		return;

	
}


static void RecordSegSet(HWND hDlg, int ch)
{
	int i, ret;
	int curDay;
	time_segment stTime;
	NVR_DEV_REC_STRC_S stRec;	
	int bChecked;
	
	memset(&stRec, 0, sizeof(NVR_DEV_REC_STRC_S));
	ret = nvr_logon_get_rec_cfg(ch, &stRec);
	if (ret < 0)
	{
		printf("get rec cfg failed\r\n");
		return;
	}	
	curDay = SendDlgItemMessage (hDlg, IDL_TIME_SELECT, CB_GETCURSEL, 0, 0);
	if (curDay < 0 || curDay >= 8) 
		return;
	for (i = 0; i < NVR_MAX_SEG_NUM; i++)
	{
		SendDlgItemMessage(hDlg, IDC_STARTTIME_1+i, MSG_GET_TIMESEG, 0, (LPARAM)&stTime);
		stRec.struTimerSeg[i][curDay].beginSec = stTime.start_hour*3600+stTime.start_min*60;
		stRec.struTimerSeg[i][curDay].endSec = stTime.stop_hour*3600+stTime.stop_min*60 + 59;
		bChecked = SendDlgItemMessage(hDlg, IDC_CHECK_STARTTIME_1+i, BM_GETCHECK, 0, 0);
		if (bChecked)
			stRec.struTimerSeg[i][curDay].u8Open = 1;
		else	
			stRec.struTimerSeg[i][curDay].u8Open = 0;
	}

	if(0 > nvr_logon_set_rec_cfg(ch, &stRec))
			return;
}

static int RecordTimeWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	static int chn = -1;
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		if (lParam >=0 &&lParam < 4)
			chn = lParam;
		InitRecordTimeParam(hDlg, chn);
		push_open_dlg(&headOpenDlg, hDlg);
     	break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);
        EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg, "时间设置");
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
		Gui_WriteTitleName(hdc, &record_time_title_rect, LTR("时段设置"));
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
				UpdateTimeParam(hDlg, chn);
				return 0;
			}
			return 0;
		}
		
		switch (wParam) 
		{
			case IDC_ALL_DAY_CHECK:
			case IDC_CHECK_STARTTIME_1:
			case IDC_CHECK_STARTTIME_2:
			case IDC_CHECK_STARTTIME_3:
			case IDC_CHECK_STARTTIME_4:
			{
				TimeSettingCheckCmdPorc(hDlg, wParam);
				break;
			}
			return 0;
			case IDC_COPY_DAY:
			{
				CopyTimeSetting(hDlg, chn);
			}
			return 0;
			case IDC_SET_REC_SEG:
			{
				RecordSegSet(hDlg, chn);
			}
			return 0;
			case IDC_SET_REC_CANCEL:
			{
				PostMessage(hDlg, MSG_CLOSE, 0, 0);
			}
			return 0;
			
			default:
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
void CreateRecordTimeDlgWin(HWND hParent, int chn)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "RecordTimeDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_record_time_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_record_time_control, sizeof(ui_record_time_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_record_time_control)
    RecordTimeDlg.controls = ui_record_time_control;
    
    DialogBoxIndirectParam (&RecordTimeDlg, hParent, RecordTimeWinProc, chn);
	memcpy(ui_record_time_control, ctrolTmp, sizeof(ui_record_time_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

