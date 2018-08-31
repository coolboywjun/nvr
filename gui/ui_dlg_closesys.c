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
#include "nvr_headers.h"

typedef enum
{
	IDC_SYS_CLOSE_OK = 1300,
	IDC_SYS_CLOSE_CANCEL,
	IDL_SYS_CLOSE_SELECT,
	IDC_SYS_CLOSE_REBOOT,
	IDC_SYS_CLOSE_HALT,
}UI_CLOSE_SYS_ID;

#define SYS_CLOSE_DLG_W	420
#define SYS_CLOSE_DLG_H	180
#define SYS_CLOSE_DLG_X	((NVR_SCREEN_WIDTH - SYS_CLOSE_DLG_W)>>1)
#define SYS_CLOSE_DLG_Y	((NVR_SCREEN_HEIGHT - SYS_CLOSE_DLG_H)>>1)

#define COBBOX_LIST_W	(SYS_CLOSE_DLG_W*5/6)
#define COBBOX_LIST_H	25
#define COBBOX_LIST_X	((SYS_CLOSE_DLG_W - COBBOX_LIST_W)/2)
#define COBBOX_LIST_Y	50	

#define BUTTON_W        60 
#define BUTTON_H        85

static CTRLDATA ui_sysclose_control[] =
{

	//CONFIG_FOUR_WORDS_BUTTON("重启系统", IDC_SYS_CLOSE_REBOOT, SYS_CLOSE_DLG_W/2 - 70 - 60, SYS_CLOSE_DLG_H/2/* - 25*/),
	//CONFIG_FOUR_WORDS_BUTTON("关闭系统", IDC_SYS_CLOSE_HALT, SYS_CLOSE_DLG_W/2 + 50 , SYS_CLOSE_DLG_H/2/* - 25*/),
	{
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP,
        SYS_CLOSE_DLG_W/2 - 60 - 60, SYS_CLOSE_DLG_H-30-BUTTON_H, BUTTON_W, BUTTON_H,
        IDC_SYS_CLOSE_REBOOT,
        "重启系统",
        (DWORD)&main_menu_pic[7],
        WS_EX_TRANSPARENT
    },
    {
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP,
        SYS_CLOSE_DLG_W/2 + 60 , SYS_CLOSE_DLG_H-30-BUTTON_H, BUTTON_W, BUTTON_H,
        IDC_SYS_CLOSE_HALT,
        "关闭系统",
        (DWORD)&main_menu_pic[6],
        WS_EX_TRANSPARENT
    },
};


static DLGTEMPLATE SysCloseDlg =
{
    WS_NONE,
    WS_EX_NONE,
    SYS_CLOSE_DLG_X, SYS_CLOSE_DLG_Y, SYS_CLOSE_DLG_W, SYS_CLOSE_DLG_H,
    "复制",
    0, 0,
    TABLESIZE(ui_sysclose_control), NULL,
    0
};

static int SysCloseWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
    	push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);
		Gui_DrawCaption(hDlg, "关闭系统");
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
		Gui_WriteTitleName(hdc, &title, LTR("关闭系统"));
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
		case IDC_SYS_CLOSE_OK:
			return 0;
		case IDC_SYS_CLOSE_CANCEL:
			PostMessage(hDlg, MSG_CLOSE, 0, 0);
			return 0;
		case IDC_SYS_CLOSE_REBOOT:
			nvr_restart();
			//PostMessage(hDlg, MSG_CLOSE, 0, 0);
			SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
			return 0;
		case IDC_SYS_CLOSE_HALT:
			nvr_shutdown();
			//PostMessage(hDlg, MSG_CLOSE, 0, 0);
			SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
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
void CreateCloseSysDlgWin(HWND hParent)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "SysCloseDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_sysclose_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_sysclose_control, sizeof(ui_sysclose_control));
	
	DLG_CAPTION_LANGUAGE_TRANS(ui_sysclose_control)
    SysCloseDlg.controls = ui_sysclose_control;
    
    DialogBoxIndirectParam (&SysCloseDlg, hParent, SysCloseWinProc, 0L);
	memcpy(ui_sysclose_control, ctrolTmp, sizeof(ui_sysclose_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

