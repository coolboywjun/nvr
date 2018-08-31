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
#include "my_trackbar.h"
#include "ctrl_ipaddress.h"
#include "nvr_lib_interface.h"
#include "gui_core.h"

#include "storage_interface.h"
#include "player_interface.h"

#include "ui_dlg_head.h"

#define ALPHA_COLORKEY_DLG_W	420
#define ALPHA_COLORKEY_DLG_H	300
#define ALPHA_COLORKEY_DLG_X	((NVR_SCREEN_WIDTH - ALPHA_COLORKEY_DLG_W)>>1)
#define ALPHA_COLORKEY_DLG_Y	((NVR_SCREEN_HEIGHT - ALPHA_COLORKEY_DLG_H)>>1)
#define ALPHA_COLORKEY_TEXT_W	60
#define ALPHA_COLORKEY_EDIT_W	150
#define ALPHA_COLORKEY_EDIT_H	20

#define ALPHA_COLORKEY_BUTTON_W  30

#define ALPHA_COLORKEY_TEXT_INC_Y	(ALPHA_COLORKEY_EDIT_H+10)

#define ALPHA_COLORKEY_TEXT_START_X	40

#define ALPHA_COLORKEY_TEXT_START_Y	50


enum UI_ALPHA_COLORKEY_ID{
	IDC_STATIC_ALPHA = 500,
	IDC_STATIC_COLORKEY,
	IDC_STATIC_COLORKEY_R,
	IDC_STATIC_COLORKEY_G,
	IDC_STATIC_COLORKEY_B,
	
	IDC_ALPHA_TRACKBAR,
	IDC_COLORKEY_R_TRACKBAR,
	IDC_COLORKEY_G_TRACKBAR,
	IDC_COLORKEY_B_TRACKBAR,

	IDC_ALPHA_VALUE,
	IDC_COLORKEY_R_VALUE,
	IDC_COLORKEY_G_VALUE,
	IDC_COLORKEY_B_VALUE,

	IDC_EXIT,
};
static CTRLDATA ui_alpha_colorkey_control[] =
{
	{
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
		ALPHA_COLORKEY_TEXT_START_X, ALPHA_COLORKEY_TEXT_START_Y, 50, 20,
        IDC_STATIC_ALPHA,
        "ALPHA",
        0,
        WS_EX_TRANSPARENT
    },
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
        ALPHA_COLORKEY_TEXT_START_X+50,  ALPHA_COLORKEY_TEXT_START_Y, 250, 20,
        IDC_ALPHA_TRACKBAR,
        "",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        ALPHA_COLORKEY_TEXT_START_X+50+250+10, ALPHA_COLORKEY_TEXT_START_Y, 30, 20,
        IDC_ALPHA_VALUE,
        "0",
        0,
		WS_EX_TRANSPARENT
    },
    {
		CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
       	ALPHA_COLORKEY_TEXT_START_X, ALPHA_COLORKEY_TEXT_START_Y+ALPHA_COLORKEY_TEXT_INC_Y, ALPHA_COLORKEY_EDIT_W, ALPHA_COLORKEY_EDIT_H,
        IDC_STATIC_COLORKEY,
        "COLORKEY",
        80,
        WS_EX_TRANSPARENT
     },
	{
		"static",
        WS_CHILD | SS_LEFT | WS_VISIBLE,
       	ALPHA_COLORKEY_TEXT_START_X+10, ALPHA_COLORKEY_TEXT_START_Y+2*ALPHA_COLORKEY_TEXT_INC_Y, 20, ALPHA_COLORKEY_EDIT_H,
        IDC_STATIC_COLORKEY_R,
        "红",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
        ALPHA_COLORKEY_TEXT_START_X+50,  ALPHA_COLORKEY_TEXT_START_Y+2*ALPHA_COLORKEY_TEXT_INC_Y, 250, 20,
        IDC_COLORKEY_R_TRACKBAR,
        "",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        ALPHA_COLORKEY_TEXT_START_X+50+250+10, ALPHA_COLORKEY_TEXT_START_Y+2*ALPHA_COLORKEY_TEXT_INC_Y, 30, 20,
        IDC_COLORKEY_R_VALUE,
        "0",
        0,
		WS_EX_TRANSPARENT
    },
     {
		"static",
        WS_CHILD | SS_LEFT | WS_VISIBLE,
       	ALPHA_COLORKEY_TEXT_START_X+10, ALPHA_COLORKEY_TEXT_START_Y+3*ALPHA_COLORKEY_TEXT_INC_Y, 20, ALPHA_COLORKEY_EDIT_H,
        IDC_STATIC_COLORKEY_G,
        "绿",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
        ALPHA_COLORKEY_TEXT_START_X+50,  ALPHA_COLORKEY_TEXT_START_Y+3*ALPHA_COLORKEY_TEXT_INC_Y, 250, 20,
        IDC_COLORKEY_G_TRACKBAR,
        "",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        ALPHA_COLORKEY_TEXT_START_X+50+250+10, ALPHA_COLORKEY_TEXT_START_Y+3*ALPHA_COLORKEY_TEXT_INC_Y, 30, 20,
        IDC_COLORKEY_G_VALUE,
        "0",
        0,
		WS_EX_TRANSPARENT
    },
     {
		"static",
        WS_CHILD | SS_LEFT | WS_VISIBLE,
       	ALPHA_COLORKEY_TEXT_START_X+10, ALPHA_COLORKEY_TEXT_START_Y+4*ALPHA_COLORKEY_TEXT_INC_Y, 20, ALPHA_COLORKEY_EDIT_H,
        IDC_STATIC_COLORKEY_B,
        "蓝",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
        ALPHA_COLORKEY_TEXT_START_X+50,  ALPHA_COLORKEY_TEXT_START_Y+4*ALPHA_COLORKEY_TEXT_INC_Y, 250, 20,
        IDC_COLORKEY_B_TRACKBAR,
        "",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        ALPHA_COLORKEY_TEXT_START_X+50+250+10, ALPHA_COLORKEY_TEXT_START_Y+4*ALPHA_COLORKEY_TEXT_INC_Y, 30, 20,
        IDC_COLORKEY_B_VALUE,
        "0",
        0,
		WS_EX_TRANSPARENT
    },
    //////////// GUI_BUTTON ////////////////
    CONFIG_TWO_WORDS_BUTTON("退出", IDC_EXIT, ALPHA_COLORKEY_DLG_W-100 , ALPHA_COLORKEY_DLG_H- 40),
    
};


static DLGTEMPLATE AlphaColorkeyDlg =
{
    WS_NONE,
    WS_EX_NONE,
    ALPHA_COLORKEY_DLG_X, ALPHA_COLORKEY_DLG_Y, ALPHA_COLORKEY_DLG_W, ALPHA_COLORKEY_DLG_H,
    "时间设置",
    0, 0,
    TABLESIZE(ui_alpha_colorkey_control), NULL,
    0
};

static void InitTrackBarData(HWND hDlg)
{
	int n=0;
	char InitBuffer[8] = {0};

	sprintf(InitBuffer, "%d", 128);
	for(n=0; n<4; n++)
	{
		SendDlgItemMessage (hDlg, IDC_ALPHA_TRACKBAR+n, TBM_SETRANGE, 0, 255);
	    SendDlgItemMessage (hDlg, IDC_ALPHA_TRACKBAR+n, TBM_SETLINESIZE, 1, 0);
	    SendDlgItemMessage (hDlg, IDC_ALPHA_TRACKBAR+n, TBM_SETPAGESIZE, 10, 0);
	    SendDlgItemMessage (hDlg, IDC_ALPHA_TRACKBAR+n, TBM_SETTICKFREQ, 255, 0);

	   	SendDlgItemMessage (hDlg, IDC_ALPHA_TRACKBAR+n, TBM_SETPOS, 128, 0);	//默认初始值 128
		SendDlgItemMessage (hDlg, IDC_ALPHA_VALUE+n,  MSG_SETTEXT, 0, (LPARAM)InitBuffer);
	}
	
}

static void TrackBarChange(HWND hDlg, int id, int code)
{
	static int nCurrentPos = 0;
	char acCurPosBuffer[8] = {0};
	int nTrackbarIndex = id - IDC_ALPHA_TRACKBAR;
	int rVal, gVal, bVal;
	
	if(code == TBN_CHANGE)
	{
		nCurrentPos = SendDlgItemMessage (hDlg, id, TBM_GETPOS, 0, 0);		
		sprintf(acCurPosBuffer, "%d", nCurrentPos);
		SendDlgItemMessage (hDlg, IDC_ALPHA_VALUE+nTrackbarIndex,  MSG_SETTEXT, 0, (LPARAM)acCurPosBuffer);
	}
	else if(code == TBN_STOPDRAG)
	{

		if(id == IDC_ALPHA_TRACKBAR)
		{
			printf("[DEBUG] alphaVal = %d\n",nCurrentPos);
			if(0 > player_set_fb_alpha((unsigned char) nCurrentPos))
			{
				printf("[ERROR] SET fb alpha failed!\n");
			}
		}
		else if((id == IDC_COLORKEY_R_TRACKBAR) || (id == IDC_COLORKEY_G_TRACKBAR) || (id == IDC_COLORKEY_B_TRACKBAR))
		{
			GetDlgItemText(hDlg, IDC_COLORKEY_R_VALUE, acCurPosBuffer, sizeof(acCurPosBuffer));
			rVal = atoi(acCurPosBuffer);
			GetDlgItemText(hDlg, IDC_COLORKEY_G_VALUE, acCurPosBuffer, sizeof(acCurPosBuffer));
			gVal = atoi(acCurPosBuffer);
			GetDlgItemText(hDlg, IDC_COLORKEY_B_VALUE, acCurPosBuffer, sizeof(acCurPosBuffer));
			bVal = atoi(acCurPosBuffer);
			printf("[DEBUG] rval = %d, gVal = %d, bVal = %d\n", rVal, gVal, bVal);
			if(0 > player_set_colorky((unsigned char) rVal, (unsigned char) gVal, (unsigned char) bVal))
			{
				printf("[ERROR] SET colorkey failed!\n");
			}
		}
	}
}
static int AlphaColorkeyWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
    	InitTrackBarData(hDlg);
    	push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);
		Gui_DrawCaption(hDlg,  "Alpha设置");
        break;
	}
	case MSG_RBUTTONUP:
    {
		PostMessage(hDlg, MSG_CLOSE, 0, 0);
        break;
    }

    case MSG_COMMAND:
    {	
		int id = LOWORD(wParam);
		int code = HIWORD(wParam);

		if((id >= IDC_ALPHA_TRACKBAR) && (id <= IDC_COLORKEY_B_TRACKBAR))
		{
			TrackBarChange(hDlg, id, code);
			return 0;
		}
		switch(wParam)
		{
			case IDC_EXIT:
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
void CreateAlphaColorkeyDlgWin(HWND hParent)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "AlphaColorkeyDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_alpha_colorkey_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_alpha_colorkey_control, sizeof(ui_alpha_colorkey_control));

	DLG_CAPTION_LANGUAGE_TRANS(ui_alpha_colorkey_control)
    AlphaColorkeyDlg.controls = ui_alpha_colorkey_control;
    
    DialogBoxIndirectParam (&AlphaColorkeyDlg, hParent, AlphaColorkeyWinProc, 0L);
	memcpy(ui_alpha_colorkey_control, ctrolTmp, sizeof(ui_alpha_colorkey_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

