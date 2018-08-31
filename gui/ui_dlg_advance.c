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

#include "gui_core.h"
#include "ui_dlg_head.h"


typedef enum
{
	SIID_TIME_SETTING = 1100,
	SIID_ABNORMAL_MANAGE,
	SIID_ALARM_OUTPUT,
	SIID_RECORD_CTRL,
	SIID_LOGIN_MANAGE,
	SIID_AUTO_MAINTAIN,
	SIID_CONFIG_BAKUP,
	SIID_LOG_INFO,
}UI_ADVANCE_SETTING_ID;

#define ADVANCE_BUTTON_X        60  //button x
#define ADVANCE_BUTTON_Y        100 //button y
#define ADVANCE_BUTTON_W        (68+50)  //button width
#define ADVANCE_BUTTON_H        (75+10) //button height
#define ADVANCE_BUTTON_HSPACE   (55-50)  //button horizontal space
#define ADVANCE_BUTTON_VSPACE   20  //button vertical space

#define ADVANCE_MAIN_MENU_WIDTH       600
#define ADVANCE_MAIN_MENU_HEIGHT      400
#define ADVANCE_MAIN_MENU_X ((NVR_SCREEN_WIDTH - ADVANCE_MAIN_MENU_WIDTH)/2)
#define ADVANCE_MAIN_MENU_Y ((NVR_SCREEN_HEIGHT - ADVANCE_MAIN_MENU_HEIGHT)/2)



static RECT  advance_title_rect = {0,0,ADVANCE_MAIN_MENU_WIDTH, 50};


/* 定义皮肤元素数组 */

static CTRLDATA advance_menu_control[] =
{
	{
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP |WS_GROUP ,
        ADVANCE_BUTTON_X+(ADVANCE_BUTTON_W+ADVANCE_BUTTON_HSPACE)*0, 
        ADVANCE_BUTTON_Y, ADVANCE_BUTTON_W, ADVANCE_BUTTON_H,
        SIID_TIME_SETTING,
        "时间设置",
        (DWORD)&advance_menu_pic[0],
        WS_EX_TRANSPARENT
    },
/*    {
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP,
        ADVANCE_BUTTON_X+(ADVANCE_BUTTON_W+ADVANCE_BUTTON_HSPACE)*1, 
        ADVANCE_BUTTON_Y, ADVANCE_BUTTON_W, ADVANCE_BUTTON_H,
        SIID_ABNORMAL_MANAGE,
        "异常处理",
        (DWORD)&advance_menu_pic[1],
        WS_EX_TRANSPARENT
    },
    {
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP,
        ADVANCE_BUTTON_X+(ADVANCE_BUTTON_W+ADVANCE_BUTTON_HSPACE)*2, 
        ADVANCE_BUTTON_Y, ADVANCE_BUTTON_W, ADVANCE_BUTTON_H,
        SIID_ALARM_OUTPUT,
        "报警输出",
        (DWORD)&advance_menu_pic[2],
        WS_EX_TRANSPARENT
    },
*/    {
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP ,
        ADVANCE_BUTTON_X+(ADVANCE_BUTTON_W+ADVANCE_BUTTON_HSPACE)*1, 
        ADVANCE_BUTTON_Y, ADVANCE_BUTTON_W, ADVANCE_BUTTON_H,
        SIID_RECORD_CTRL,
        "录像控制",
        (DWORD)&advance_menu_pic[3],
        WS_EX_TRANSPARENT
    },
     {
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP,
        ADVANCE_BUTTON_X+(ADVANCE_BUTTON_W+ADVANCE_BUTTON_HSPACE)*2, 
        ADVANCE_BUTTON_Y, ADVANCE_BUTTON_W, ADVANCE_BUTTON_H,
        SIID_LOGIN_MANAGE,
        "用户账户",
        (DWORD)&advance_menu_pic[4],
        WS_EX_TRANSPARENT
    },
     {
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP,
        ADVANCE_BUTTON_X+(ADVANCE_BUTTON_W+ADVANCE_BUTTON_HSPACE)*3, 
        ADVANCE_BUTTON_Y, ADVANCE_BUTTON_W, ADVANCE_BUTTON_H,
        SIID_AUTO_MAINTAIN,
        "自动维护",
        (DWORD)&advance_menu_pic[5],
        WS_EX_TRANSPARENT
    },
     {
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP,
        ADVANCE_BUTTON_X+(ADVANCE_BUTTON_W+ADVANCE_BUTTON_HSPACE)*1, 
        ADVANCE_BUTTON_Y+(ADVANCE_BUTTON_H+ADVANCE_BUTTON_VSPACE), ADVANCE_BUTTON_W, ADVANCE_BUTTON_H,
        SIID_CONFIG_BAKUP,
        "配置备份",
        (DWORD)&advance_menu_pic[6],
        WS_EX_TRANSPARENT
    }
	, 
	{
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP,
        ADVANCE_BUTTON_X+(ADVANCE_BUTTON_W+ADVANCE_BUTTON_HSPACE)*0,
        ADVANCE_BUTTON_Y+(ADVANCE_BUTTON_H+ADVANCE_BUTTON_VSPACE), ADVANCE_BUTTON_W, ADVANCE_BUTTON_H,
        SIID_LOG_INFO,
        "日志信息",
        (DWORD)&advance_menu_pic[7],
        WS_EX_TRANSPARENT
    }
};

static DLGTEMPLATE AdvanceMainDlg =
{
    WS_NONE,
    WS_EX_NONE,
    ADVANCE_MAIN_MENU_X, ADVANCE_MAIN_MENU_Y, 
    ADVANCE_MAIN_MENU_WIDTH, ADVANCE_MAIN_MENU_HEIGHT,
    "远程设备",
    0, 0,
    TABLESIZE(advance_menu_control), NULL,
    0
};


static int AdvanceMainMenuWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
    {
    	push_open_dlg(&headOpenDlg, hDlg);
        return 0;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);
		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bmp_bkgnd); 		
		Gui_WriteTitleName(hdc, &advance_title_rect, "高级选项");
        EndPaint (hDlg, hdc);
        return 0;
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
		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bk_pic[SIZE_600x400]); 
		Gui_WriteTitleName(hdc, &advance_title_rect, LTR("高级选项"));
		if (fGetDC) 
			ReleaseDC (hdc); 
		return 0; 
	}
	break;
	case MSG_RBUTTONUP:
    {
		PostMessage(hDlg, MSG_CLOSE, 0, 0);	
        return 0;
    }

    case MSG_COMMAND:
    {		
		switch (wParam) 
		{
		case SIID_TIME_SETTING:
			CreateNtpsetDlgWin(hDlg);
			break;
        case SIID_ABNORMAL_MANAGE:	
			return 0;
		case SIID_AUTO_MAINTAIN:
			CreateAutoMaintainDlgWin(hDlg);
			break;
		case SIID_LOGIN_MANAGE:
			CreateUserDlgWin(hDlg);
			break;
		case SIID_RECORD_CTRL:
			CreateRecordDlgWin(hDlg);
			break;
		case SIID_CONFIG_BAKUP:
			CreateConfigBakupWin(hDlg);
			break;
		case SIID_LOG_INFO:
			CreateLogDlgWin(hDlg);
			break;
		default:
			break;
        }
		
        break;
    }
    case MSG_CLOSE:
    {	HWND hWnd;
		pop_open_dlg(&headOpenDlg, &hWnd);
        EndDialog (hDlg, 0);
        return 0;
    }
	default:
		break;

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

void CreateAdvanceMainMenuDlgWin(HWND hParent)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "AdvanceMainDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(advance_menu_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, advance_menu_control, sizeof(advance_menu_control));

	DLG_CAPTION_LANGUAGE_TRANS(advance_menu_control)
    AdvanceMainDlg.controls = advance_menu_control; 

    DialogBoxIndirectParam (&AdvanceMainDlg, hParent, AdvanceMainMenuWinProc, 0L);	
	memcpy(advance_menu_control, ctrolTmp, sizeof(advance_menu_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

