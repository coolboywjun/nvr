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

#define RECORD_DLG_W	600
#define RECORD_DLG_H	400
#define RECORD_DLG_X	((NVR_SCREEN_WIDTH - RECORD_DLG_W)>>1)
#define RECORD_DLG_Y	((NVR_SCREEN_HEIGHT - RECORD_DLG_H)>>1)
#define RECORD_TEXT_W	(60+10)
#define RECORD_EDIT_W	80
#define RECORD_EDIT_H	30

#define RECORD_BUTTON_W  30

#define RECORD_TEXT_INC_Y	(RECORD_EDIT_H+15)
#define RATION_INC_X		(70)
#define RECORD_TEXT_START_X	50
#define RECORD_TEXT_START_Y	60

typedef enum
{
	IDC_STATIC_RECORD_MODE = 2600,
	IDC_STATIC_RECORD_AUTO,
	IDC_STATIC_RECORD_MANUAL,
	IDC_STATIC_RECORD_ALARM,
	IDC_STATIC_MANUAL_ALARM,
	IDC_STATIC_RECORD_CLOSE,
	IDC_STATIC_RECORD_FULL,
	IDC_STATIC_RECORD_CH1,
	IDC_STATIC_RECORD_CH2,
	IDC_STATIC_RECORD_CH3,
	IDC_STATIC_RECORD_CH4,
	IDC_RECORD_SAVE,
	IDC_RECORD_CANCEL,
	IDC_RADIOBUTTON_AUTO_FULL,
	IDC_RADIOBUTTON_MANUAL_FULL,
	IDC_RADIOBUTTON_ALARM_FULL,
	IDC_RADIOBUTTON_MANUAL_ALARM_FULL,
	IDC_RADIOBUTTON_CLOSE_FULL,
	IDC_RADIOBUTTON_AUTO_CH1,
	IDC_RADIOBUTTON_MANUAL_CH1,
	IDC_RADIOBUTTON_ALARM_CH1,
	IDC_RADIOBUTTON_MANUAL_ALARM_CH1,
	IDC_RADIOBUTTON_CLOSE_CH1,
	IDC_RADIOBUTTON_AUTO_CH2,
	IDC_RADIOBUTTON_MANUAL_CH2,
	IDC_RADIOBUTTON_ALARM_CH2,
	IDC_RADIOBUTTON_MANUAL_ALARM_CH2,
	IDC_RADIOBUTTON_CLOSE_CH2,
	IDC_RADIOBUTTON_AUTO_CH3,
	IDC_RADIOBUTTON_MANUAL_CH3,
	IDC_RADIOBUTTON_ALARM_CH3,
	IDC_RADIOBUTTON_MANUAL_ALARM_CH3,
	IDC_RADIOBUTTON_CLOSE_CH3,
	IDC_RADIOBUTTON_AUTO_CH4,
	IDC_RADIOBUTTON_MANUAL_CH4,
	IDC_RADIOBUTTON_ALARM_CH4,
	IDC_RADIOBUTTON_MANUAL_ALARM_CH4,
	IDC_RADIOBUTTON_CLOSE_CH4
}UI_RECORD_CTRL_ID;

//static BITMAP  bmp_bkgnd;
//static RECT RecordTileRect = {0,0,RECORD_DLG_W,40};


static CTRLDATA ui_record_control[] =
{
	#if 1
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        RECORD_TEXT_START_X, RECORD_TEXT_START_Y, RECORD_TEXT_W+20, RECORD_EDIT_H,
        IDC_STATIC_RECORD_MODE,
        "录像模式",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        RECORD_TEXT_START_X, RECORD_TEXT_START_Y+RECORD_TEXT_INC_Y, RECORD_TEXT_W+30, RECORD_EDIT_H,
        IDC_STATIC_RECORD_AUTO,
        "开机录像",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        RECORD_TEXT_START_X, RECORD_TEXT_START_Y+2*RECORD_TEXT_INC_Y, RECORD_TEXT_W+20, RECORD_EDIT_H,
        IDC_STATIC_RECORD_MANUAL,
        "定时录像",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        RECORD_TEXT_START_X, RECORD_TEXT_START_Y+3*RECORD_TEXT_INC_Y, RECORD_TEXT_W+20, RECORD_EDIT_H,
        IDC_STATIC_RECORD_ALARM,
        "入侵录像",
        0,
		WS_EX_TRANSPARENT
    },
     {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        RECORD_TEXT_START_X, RECORD_TEXT_START_Y+4*RECORD_TEXT_INC_Y, RECORD_TEXT_W+30, RECORD_EDIT_H,
        IDC_STATIC_MANUAL_ALARM,
        "定时&入侵",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        RECORD_TEXT_START_X, RECORD_TEXT_START_Y+5*RECORD_TEXT_INC_Y, RECORD_TEXT_W+20, RECORD_EDIT_H,
        IDC_STATIC_RECORD_CLOSE,
        "关闭",
        0,
		WS_EX_TRANSPARENT
    },
    ///////////////////////////////////////////////
    /*FULL*/
    {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE ,
        RECORD_TEXT_START_X+100+5, RECORD_TEXT_START_Y, RECORD_TEXT_W, RECORD_EDIT_H,
        IDC_STATIC_RECORD_FULL,
        "全",
        0,
        WS_EX_TRANSPARENT
    },
    {
		GUI_BUTTON,
		BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP,
        RECORD_TEXT_START_X+100, RECORD_TEXT_START_Y+RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_AUTO_FULL,
        "",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_BUTTON,
		BS_AUTORADIOBUTTON | WS_VISIBLE,
        RECORD_TEXT_START_X+100, RECORD_TEXT_START_Y+2*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_MANUAL_FULL,
        "",
        0,
		WS_EX_TRANSPARENT
	},
	{
		GUI_BUTTON,
		BS_AUTORADIOBUTTON | WS_VISIBLE,
        RECORD_TEXT_START_X+100, RECORD_TEXT_START_Y+3*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_ALARM_FULL,
        "",
        0,
		WS_EX_TRANSPARENT
	},
	{
		GUI_BUTTON,
		BS_AUTORADIOBUTTON | WS_VISIBLE,
        RECORD_TEXT_START_X+100, RECORD_TEXT_START_Y+4*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_MANUAL_ALARM_FULL,
        "",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_BUTTON,
		BS_AUTORADIOBUTTON | WS_VISIBLE,
        RECORD_TEXT_START_X+100, RECORD_TEXT_START_Y+5*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_CLOSE_FULL,
        "",
        0,
		WS_EX_TRANSPARENT
	},

    ///////////////////////////////////////////
    /*CH1*/
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE ,
        RECORD_TEXT_START_X+100+5+RATION_INC_X, RECORD_TEXT_START_Y, RECORD_TEXT_W, RECORD_EDIT_H,
        IDC_STATIC_RECORD_CH1,
        "1",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_BUTTON,
		BS_AUTORADIOBUTTON  | WS_VISIBLE | WS_GROUP | BS_CHECKED,
        RECORD_TEXT_START_X+100+RATION_INC_X, RECORD_TEXT_START_Y+RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_AUTO_CH1,
        "",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_BUTTON,
		BS_AUTORADIOBUTTON |WS_VISIBLE,
        RECORD_TEXT_START_X+100+RATION_INC_X, RECORD_TEXT_START_Y+2*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_MANUAL_CH1,
        "",
        0,
		WS_EX_TRANSPARENT
	},
	{
		GUI_BUTTON,
		BS_AUTORADIOBUTTON | WS_VISIBLE,
        RECORD_TEXT_START_X+100+RATION_INC_X, RECORD_TEXT_START_Y+3*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_ALARM_CH1,
        "",
        0,
		WS_EX_TRANSPARENT
	},
	{
		GUI_BUTTON,
		BS_AUTORADIOBUTTON | WS_VISIBLE,
        RECORD_TEXT_START_X+100+RATION_INC_X, RECORD_TEXT_START_Y+4*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_MANUAL_ALARM_CH1,
        "",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_BUTTON,
		BS_AUTORADIOBUTTON |WS_VISIBLE,
        RECORD_TEXT_START_X+100+RATION_INC_X, RECORD_TEXT_START_Y+5*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_CLOSE_CH1,
        "",
        0,
		WS_EX_TRANSPARENT
	},
    ////////////////////////////////////
    /*CH2*/
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        RECORD_TEXT_START_X+100+5+2*RATION_INC_X, RECORD_TEXT_START_Y, RECORD_TEXT_W, RECORD_EDIT_H,
        IDC_STATIC_RECORD_CH2,
        "2",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_BUTTON,
		WS_CHILD |  BS_AUTORADIOBUTTON | WS_GROUP | WS_VISIBLE | BS_CHECKED,
        RECORD_TEXT_START_X+100+2*RATION_INC_X, RECORD_TEXT_START_Y+RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_AUTO_CH2,
        "",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_BUTTON,
		BS_AUTORADIOBUTTON |WS_VISIBLE,
        RECORD_TEXT_START_X+100+2*RATION_INC_X, RECORD_TEXT_START_Y+2*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_MANUAL_CH2,
        "",
        0,
		WS_EX_TRANSPARENT
	},
	{
		GUI_BUTTON,
		BS_AUTORADIOBUTTON | WS_VISIBLE,
        RECORD_TEXT_START_X+100+2*RATION_INC_X, RECORD_TEXT_START_Y+3*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_ALARM_CH2,
        "",
        0,
		WS_EX_TRANSPARENT
	},
	{
		GUI_BUTTON,
		BS_AUTORADIOBUTTON | WS_VISIBLE,
        RECORD_TEXT_START_X+100+2*RATION_INC_X, RECORD_TEXT_START_Y+4*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_MANUAL_ALARM_CH2,
        "",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_BUTTON,
		BS_AUTORADIOBUTTON |WS_VISIBLE,
        RECORD_TEXT_START_X+100+2*RATION_INC_X, RECORD_TEXT_START_Y+5*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_CLOSE_CH2,
        "",
        0,
		WS_EX_TRANSPARENT
	},
    ////////////////////////////////////
    /*CH3*/
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        RECORD_TEXT_START_X+100+5+3*RATION_INC_X, RECORD_TEXT_START_Y, RECORD_TEXT_W, RECORD_EDIT_H,
        IDC_STATIC_RECORD_CH3,
        "3",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_BUTTON,
		WS_CHILD |  BS_AUTORADIOBUTTON | WS_GROUP | WS_VISIBLE | BS_CHECKED,
        RECORD_TEXT_START_X+100+3*RATION_INC_X, RECORD_TEXT_START_Y+RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_AUTO_CH3,
        "",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_BUTTON,
		BS_AUTORADIOBUTTON |WS_VISIBLE,
        RECORD_TEXT_START_X+100+3*RATION_INC_X, RECORD_TEXT_START_Y+2*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_MANUAL_CH3,
        "",
        0,
		WS_EX_TRANSPARENT
	},
	{
		GUI_BUTTON,
		BS_AUTORADIOBUTTON | WS_VISIBLE,
        RECORD_TEXT_START_X+100+3*RATION_INC_X, RECORD_TEXT_START_Y+3*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_ALARM_CH3,
        "",
        0,
		WS_EX_TRANSPARENT
	},
	{
		GUI_BUTTON,
		BS_AUTORADIOBUTTON | WS_VISIBLE,
        RECORD_TEXT_START_X+100+3*RATION_INC_X, RECORD_TEXT_START_Y+4*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_MANUAL_ALARM_CH3,
        "",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_BUTTON,
		BS_AUTORADIOBUTTON |WS_VISIBLE,
        RECORD_TEXT_START_X+100+3*RATION_INC_X, RECORD_TEXT_START_Y+5*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_CLOSE_CH3,
        "",
        0,
		WS_EX_TRANSPARENT
	},
    ////////////////////////////////////
    /*CH4*/
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        RECORD_TEXT_START_X+100+5+4*RATION_INC_X, RECORD_TEXT_START_Y, RECORD_TEXT_W, RECORD_EDIT_H,
        IDC_STATIC_RECORD_CH4,
        "4",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_BUTTON,
		WS_CHILD |  BS_AUTORADIOBUTTON | WS_GROUP | WS_VISIBLE | BS_CHECKED,
        RECORD_TEXT_START_X+100+4*RATION_INC_X, RECORD_TEXT_START_Y+RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_AUTO_CH4,
        "",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_BUTTON,
		BS_AUTORADIOBUTTON |WS_VISIBLE,
        RECORD_TEXT_START_X+100+4*RATION_INC_X, RECORD_TEXT_START_Y+2*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_MANUAL_CH4,
        "",
        0,
		WS_EX_TRANSPARENT
	},
	{
		GUI_BUTTON,
		BS_AUTORADIOBUTTON | WS_VISIBLE,
        RECORD_TEXT_START_X+100+4*RATION_INC_X, RECORD_TEXT_START_Y+3*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_ALARM_CH4,
        "",
        0,
		WS_EX_TRANSPARENT
	},
	{
		GUI_BUTTON,
		BS_AUTORADIOBUTTON | WS_VISIBLE,
        RECORD_TEXT_START_X+100+4*RATION_INC_X, RECORD_TEXT_START_Y+4*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_MANUAL_ALARM_CH4,
        "",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_BUTTON,
		BS_AUTORADIOBUTTON |WS_VISIBLE,
        RECORD_TEXT_START_X+100+4*RATION_INC_X, RECORD_TEXT_START_Y+5*RECORD_TEXT_INC_Y, RECORD_BUTTON_W, RECORD_EDIT_H,
        IDC_RADIOBUTTON_CLOSE_CH4,
        "",
        0,
		WS_EX_TRANSPARENT
	},      
	//////////////////////////// GUI_BUTTON ////////////////////////////////////
	CONFIG_TWO_WORDS_BUTTON("确定", IDC_RECORD_SAVE, RECORD_DLG_W - 165, RECORD_DLG_H - 50),
	CONFIG_TWO_WORDS_BUTTON("取消", IDC_RECORD_CANCEL, RECORD_DLG_W - 100 , RECORD_DLG_H- 50),

	#endif	
};


static DLGTEMPLATE RecordDlg =
{
    WS_NONE,
    WS_EX_NONE,
    RECORD_DLG_X, RECORD_DLG_Y, RECORD_DLG_W, RECORD_DLG_H,
    "录像控制",
    0, 0,
    TABLESIZE(ui_record_control), NULL,
    0
};

static void UpdateRecordFullCheck(HWND hDlg)
{
	int i, j;
	int bChecked = 0;
	
	for(i = 0; i < 5; i++)
	{
		for(j = 0; j < 4; j++)
		{
			bChecked = SendDlgItemMessage(hDlg, IDC_RADIOBUTTON_AUTO_CH1+j*5+i, BM_GETCHECK, 0, 0);
			if(!bChecked)
				break;
		}
		if(bChecked)
			SendDlgItemMessage(hDlg, IDC_RADIOBUTTON_AUTO_FULL+i, BM_SETCHECK, BST_CHECKED, 0);
		else 
			SendDlgItemMessage(hDlg, IDC_RADIOBUTTON_AUTO_FULL+i, BM_SETCHECK, BST_UNCHECKED, 0);
	}
}

static void UpdateRecordCheck(HWND hDlg)
{
	int ret;
	int ch;
	NVR_DEV_REC_STRC_S stRec;
	memset(&stRec, 0, sizeof(NVR_DEV_REC_STRC_S));
	for (ch = 0; ch < 4; ch++)
	{
		ret = nvr_logon_get_rec_cfg(ch, &stRec);
		if (ret < 0)
		{
			printf("get rec cfg failed\r\n");
		}

		//关闭该通道录像
		if (stRec.u8Enable == 0)
		{
			SendDlgItemMessage(hDlg, IDC_RADIOBUTTON_CLOSE_CH1 + 5*ch, BM_SETCHECK, BST_CHECKED, 0);
			continue;
		}

		//全天录像
		if (stRec.u8RecType == 0)
		{
			SendDlgItemMessage(hDlg, IDC_RADIOBUTTON_AUTO_CH1 + 5*ch, BM_SETCHECK, BST_CHECKED, 0);
		}
		//定时录像
		else if (stRec.u8RecType == 1)
		{			
			SendDlgItemMessage(hDlg, IDC_RADIOBUTTON_MANUAL_CH1 + 5*ch, BM_SETCHECK, BST_CHECKED, 0);
		}
		//报警录像
		else if (stRec.u8RecType == 2)
		{			
			SendDlgItemMessage(hDlg, IDC_RADIOBUTTON_ALARM_CH1 + 5*ch, BM_SETCHECK, BST_CHECKED, 0);
		}
		//定时&报警
		else if (stRec.u8RecType == 3)
		{			
			SendDlgItemMessage(hDlg, IDC_RADIOBUTTON_MANUAL_ALARM_CH1 + 5*ch, BM_SETCHECK, BST_CHECKED, 0);
		}
	}
	UpdateRecordFullCheck(hDlg);
}


static void CheckCtrlSetRecord(HWND hDlg)
{
	int ch = 0;
	int bChecked;
	int ret;
	NVR_DEV_REC_STRC_S stRec;
	for (ch = 0; ch < 4; ch ++)
	{
		int recType;
		
		ret = nvr_logon_get_rec_cfg(ch, &stRec);
		if (ret < 0)
		{
			printf("get rec cfg failed\r\n");
			return;
		}

		
		bChecked = SendDlgItemMessage(hDlg, IDC_RADIOBUTTON_CLOSE_CH1+ch*5, BM_GETCHECK, 0, 0);
		if (bChecked)
		{
			if (stRec.u8Enable == 0) 
				continue;
			
			stRec.u8Enable = 0;
			nvr_logon_set_rec_cfg(ch, &stRec);
			continue;
		}
		
		for (recType = 0; recType <= 3 ; recType++)
		{
			bChecked = SendDlgItemMessage(hDlg, IDC_RADIOBUTTON_AUTO_CH1+recType+ch*5, BM_GETCHECK, 0, 0);
			if (bChecked)
			{
				if (stRec.u8Enable == 0)
				{
					stRec.u8Enable = 1;
					stRec.u8RecType = recType;
					nvr_logon_set_rec_cfg(ch, &stRec);
					break;
				}
				
				if (stRec.u8RecType == recType) continue;

				stRec.u8RecType = recType;
				nvr_logon_set_rec_cfg(ch, &stRec);
				break;
			}
		}	
	}
}

static void InitRecordParam(HWND hDlg)
{
	UpdateRecordCheck(hDlg);
}


static int RecordWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		InitRecordParam(hDlg);
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{	
		HDC hdc;		
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);
		Gui_DrawCaption(hDlg, "录像控制");
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
		Gui_WriteTitleName(hdc, &title, LTR("录像控制"));
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
			case IDC_RADIOBUTTON_AUTO_FULL:
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH1, IDC_RADIOBUTTON_CLOSE_CH1, IDC_RADIOBUTTON_AUTO_CH1);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH2, IDC_RADIOBUTTON_CLOSE_CH2, IDC_RADIOBUTTON_AUTO_CH2);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH3, IDC_RADIOBUTTON_CLOSE_CH3, IDC_RADIOBUTTON_AUTO_CH3);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH4, IDC_RADIOBUTTON_CLOSE_CH4, IDC_RADIOBUTTON_AUTO_CH4);
				break;
				
			case IDC_RADIOBUTTON_MANUAL_FULL:
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH1, IDC_RADIOBUTTON_CLOSE_CH1, IDC_RADIOBUTTON_MANUAL_CH1);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH2, IDC_RADIOBUTTON_CLOSE_CH2, IDC_RADIOBUTTON_MANUAL_CH2);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH3, IDC_RADIOBUTTON_CLOSE_CH3, IDC_RADIOBUTTON_MANUAL_CH3);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH4, IDC_RADIOBUTTON_CLOSE_CH4, IDC_RADIOBUTTON_MANUAL_CH4);
				break;
				
			case IDC_RADIOBUTTON_ALARM_FULL:
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH1, IDC_RADIOBUTTON_CLOSE_CH1, IDC_RADIOBUTTON_ALARM_CH1);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH2, IDC_RADIOBUTTON_CLOSE_CH2, IDC_RADIOBUTTON_ALARM_CH2);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH3, IDC_RADIOBUTTON_CLOSE_CH3, IDC_RADIOBUTTON_ALARM_CH3);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH4, IDC_RADIOBUTTON_CLOSE_CH4, IDC_RADIOBUTTON_ALARM_CH4);
				break;

			case IDC_RADIOBUTTON_MANUAL_ALARM_FULL:
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH1, IDC_RADIOBUTTON_CLOSE_CH1, IDC_RADIOBUTTON_MANUAL_ALARM_CH1);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH2, IDC_RADIOBUTTON_CLOSE_CH2, IDC_RADIOBUTTON_MANUAL_ALARM_CH2);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH3, IDC_RADIOBUTTON_CLOSE_CH3, IDC_RADIOBUTTON_MANUAL_ALARM_CH3);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH4, IDC_RADIOBUTTON_CLOSE_CH4, IDC_RADIOBUTTON_MANUAL_ALARM_CH4);
				break;
				
			case IDC_RADIOBUTTON_CLOSE_FULL:
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH1, IDC_RADIOBUTTON_CLOSE_CH1, IDC_RADIOBUTTON_CLOSE_CH1);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH2, IDC_RADIOBUTTON_CLOSE_CH2, IDC_RADIOBUTTON_CLOSE_CH2);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH3, IDC_RADIOBUTTON_CLOSE_CH3, IDC_RADIOBUTTON_CLOSE_CH3);
				CheckRadioButton(hDlg, IDC_RADIOBUTTON_AUTO_CH4, IDC_RADIOBUTTON_CLOSE_CH4, IDC_RADIOBUTTON_CLOSE_CH4);
				break;
				
			case IDC_RADIOBUTTON_AUTO_CH1:
			case IDC_RADIOBUTTON_AUTO_CH2:
			case IDC_RADIOBUTTON_AUTO_CH3:
			case IDC_RADIOBUTTON_AUTO_CH4:
			case IDC_RADIOBUTTON_MANUAL_CH1:
			case IDC_RADIOBUTTON_MANUAL_CH2:
			case IDC_RADIOBUTTON_MANUAL_CH3:
			case IDC_RADIOBUTTON_MANUAL_CH4:
			case IDC_RADIOBUTTON_ALARM_CH1:
			case IDC_RADIOBUTTON_ALARM_CH2:
			case IDC_RADIOBUTTON_ALARM_CH3:
			case IDC_RADIOBUTTON_ALARM_CH4:
			case IDC_RADIOBUTTON_MANUAL_ALARM_CH1:
			case IDC_RADIOBUTTON_MANUAL_ALARM_CH2:
			case IDC_RADIOBUTTON_MANUAL_ALARM_CH3:
			case IDC_RADIOBUTTON_MANUAL_ALARM_CH4:
			case IDC_RADIOBUTTON_CLOSE_CH1:
			case IDC_RADIOBUTTON_CLOSE_CH2:
			case IDC_RADIOBUTTON_CLOSE_CH3:
			case IDC_RADIOBUTTON_CLOSE_CH4:
				UpdateRecordFullCheck(hDlg);
				break;
				
			case IDC_RECORD_SAVE:
				CheckCtrlSetRecord(hDlg);
				//PostMessage(hDlg, MSG_CLOSE, 0, 0);
				SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
				return 0;
			case IDC_RECORD_CANCEL:
				PostMessage(hDlg, MSG_CLOSE, 0, 0);
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
void CreateRecordDlgWin(HWND hParent)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "RecordDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_record_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_record_control, sizeof(ui_record_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_record_control)
    RecordDlg.controls = ui_record_control;
    
    DialogBoxIndirectParam (&RecordDlg, hParent, RecordWinProc, 0L);
	memcpy(ui_record_control, ctrolTmp, sizeof(ui_record_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

