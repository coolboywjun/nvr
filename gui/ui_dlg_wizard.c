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

#define WIZARD_DLG_W	420
#define WIZARD_DLG_H	180
#define WIZARD_DLG_X	((NVR_SCREEN_WIDTH - WIZARD_DLG_W)>>1)
#define WIZARD_DLG_Y	((NVR_SCREEN_HEIGHT - WIZARD_DLG_H)>>1)
#define WIZARD_TEXT_W	60
#define WIZARD_EDIT_W	80
#define WIZARD_EDIT_H	20


#define WIZARD_TEXT_START_X	40
#define WIZARD_TEXT_START_Y	60

typedef enum
{
	IDC_WIZARD_NEXT = 1550,
	IDC_WIZARD_CANCEL,
}UI_WIZARD_ID;


static CTRLDATA ui_wizard_control[] =
{
	 {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        WIZARD_TEXT_START_X, WIZARD_TEXT_START_Y, WIZARD_DLG_W - 2*WIZARD_TEXT_START_X, 60,
        IDC_STATIC,
        "开机向导设置:包括基本设置、网络设置、远程设备、录像计划。",
        0,
		WS_EX_TRANSPARENT
    },
    CONFIG_THREE_WORDS_BUTTON("下一步", IDC_WIZARD_NEXT, WIZARD_DLG_W - 180, WIZARD_DLG_H - 50),
    CONFIG_TWO_WORDS_BUTTON("取消", IDC_WIZARD_CANCEL, WIZARD_DLG_W - 100 , WIZARD_DLG_H - 50),
};


static DLGTEMPLATE WizardDlg =
{
    WS_NONE,
    WS_EX_NONE,
    WIZARD_DLG_X, WIZARD_DLG_Y, WIZARD_DLG_W, WIZARD_DLG_H,
    "开机向导",
    0, 0,
    TABLESIZE(ui_wizard_control), NULL,
    0
};


static int WizardWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
    {
		push_open_dlg(&headOpenDlg, hDlg);
		break;
    }

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
		Gui_WriteTitleName(hdc, &title, LTR("开机向导"));
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
		case IDC_WIZARD_NEXT:
			CreateQrcodeDlgWin(hDlg);
			return 0;
		case IDC_WIZARD_CANCEL:
			BroadcastMessage(MSG_WIZARD_EXIT, 0, 0);
			return 0;
		}
		break;
    }

	case MSG_WIZARD_EXIT:
	{
		printf("0000000000000000\r\n");
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
void CreateWizardDlgWin(HWND hParent)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "WizardDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_wizard_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_wizard_control, sizeof(ui_wizard_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_wizard_control)
    WizardDlg.controls = ui_wizard_control;
    
    DialogBoxIndirectParam (&WizardDlg, hParent, WizardWinProc, 0L);
	memcpy(ui_wizard_control, ctrolTmp, sizeof(ui_wizard_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

