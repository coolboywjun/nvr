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


#include "storage_interface.h"
#include "player_interface.h"

typedef enum
{
	SIID_VOD_QUERY = 2200,
	SIID_SYS_INFO,
	SIID_SYS_SETTING,
	SIID_REMOTE_SETTING,
	SIID_ADVANCE_SETTING,
	SIID_FILE_BACKUP,
	SIID_SYS_CLOSE,
}UI_MAINMENU_ID;


#define BUTTON_X        60
#define BUTTON_Y        100
#define BUTTON_W        (68+50) 
#define BUTTON_H        (75+ 10)
#define BUTTON_HSPACE   (55-50) 
#define BUTTON_VSPACE   20


#define MAIN_MENU_WIDTH       600
#define MAIN_MENU_HEIGHT      400
#define MAIN_MENU_X ((NVR_SCREEN_WIDTH - MAIN_MENU_WIDTH)/2)
#define MAIN_MENU_Y ((NVR_SCREEN_HEIGHT - MAIN_MENU_HEIGHT)/2)


static RECT  main_title_rect = {0,0,MAIN_MENU_WIDTH, 50};


/* 定义皮肤元素数组 */

static int displaymode;
static int currentCh;

static CTRLDATA menu_control[] =
{
	{
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP |WS_GROUP ,
        BUTTON_X+(BUTTON_W+BUTTON_HSPACE)*0, BUTTON_Y, BUTTON_W, BUTTON_H,
        SIID_REMOTE_SETTING,
        "远程设置",
        (DWORD)&main_menu_pic[0],
        WS_EX_TRANSPARENT
    },
    {
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP,
        BUTTON_X+(BUTTON_W+BUTTON_HSPACE)*1, BUTTON_Y, BUTTON_W, BUTTON_H,
        SIID_FILE_BACKUP,
        "录像管理",
        (DWORD)&main_menu_pic[1],
        WS_EX_TRANSPARENT
    },
    {
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP,
        BUTTON_X+(BUTTON_W+BUTTON_HSPACE)*2, BUTTON_Y, BUTTON_W, BUTTON_H,
        SIID_VOD_QUERY,
        "录像回放",
        (DWORD)&main_menu_pic[2],
        WS_EX_TRANSPARENT
    },
    {
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP ,
        BUTTON_X+(BUTTON_W+BUTTON_HSPACE)*3, BUTTON_Y, BUTTON_W, BUTTON_H,
        SIID_SYS_SETTING,
        "网络设置",
        (DWORD)&main_menu_pic[3],
        WS_EX_TRANSPARENT
    },
     {
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP,
        BUTTON_X+(BUTTON_W+BUTTON_HSPACE)*0, BUTTON_Y+(BUTTON_H+BUTTON_VSPACE), BUTTON_W, BUTTON_H,
        SIID_ADVANCE_SETTING,
        "高级设置",
        (DWORD)&main_menu_pic[4],
        WS_EX_TRANSPARENT
    },
     {
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP,
        BUTTON_X+(BUTTON_W+BUTTON_HSPACE)*1, BUTTON_Y+(BUTTON_H+BUTTON_VSPACE), BUTTON_W, BUTTON_H,
        SIID_SYS_INFO,
        "系统信息",
        (DWORD)&main_menu_pic[5],
        WS_EX_TRANSPARENT
    },
     {
        GUI_PIC_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON|BS_BITMAP,
        BUTTON_X+(BUTTON_W+BUTTON_HSPACE)*2, BUTTON_Y+(BUTTON_H+BUTTON_VSPACE), BUTTON_W, BUTTON_H,
        SIID_SYS_CLOSE,
        "关闭系统",
        (DWORD)&main_menu_pic[6],
        WS_EX_TRANSPARENT
    }
};


static DLGTEMPLATE MainDlg =
{
    WS_NONE,
    WS_EX_NONE,
    MAIN_MENU_X, MAIN_MENU_Y, MAIN_MENU_WIDTH, MAIN_MENU_HEIGHT,
    "远程设备",
    0, 0,
    TABLESIZE(menu_control), NULL,
    0
};

static int MainMenuWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
    
    	push_open_dlg(&headOpenDlg, hDlg);
//    	show_open_dlg(headOpenDlg);
        return 0;
    }
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);
        return 0;
	}
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
		Gui_WriteTitleName(hdc, &main_title_rect, LTR("主菜单窗口"));
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
		case SIID_VOD_QUERY:
		{
			player_set_scn_rsz(4, 5, 128*7+2, 72*8 - 20);
			player_set_replay_mode(4);
			CreateReplayDlgWin(hDlg, displaymode, currentCh);
			break;
		}
		case SIID_SYS_SETTING:
			CreateNetCfgDlgWin(hDlg);
			break;
        case SIID_REMOTE_SETTING:			
			CreateRemoteDeviceDlgWin(hDlg);
			break;
		case SIID_ADVANCE_SETTING:
			CreateAdvanceMainMenuDlgWin(hDlg);
			break;
		case SIID_FILE_BACKUP:			
			CreateDiskManageDlgWin(hDlg);
			break;
		case SIID_SYS_INFO:
			CreateSystemInfoDlgWin(hDlg);
			break;
		case SIID_SYS_CLOSE:
			CreateCloseSysDlgWin(hDlg);
			break;
		default:
			break;
        }
		
        break;
    }
    case MSG_CLOSE:
    {	   
    	HWND hwind;
		pop_open_dlg(&headOpenDlg, &hwind);
        EndDialog (hDlg, 0);
        return 0;
    }
	default:
		break;

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

void CreateMainMenuDlgWin(HWND hParent, int dis_mode, int cur_ch)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "MainDlg" , 0 , 0);
#endif
	displaymode = dis_mode;
	currentCh = cur_ch;
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(menu_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, menu_control, sizeof(menu_control));
	
	DLG_CAPTION_LANGUAGE_TRANS(menu_control)
    MainDlg.controls = menu_control;
    
    DialogBoxIndirectParam (&MainDlg, hParent, MainMenuWinProc, 0L);
	memcpy(menu_control, ctrolTmp, sizeof(menu_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

