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

#define PTZ_DLG_W	490 
#define PTZ_DLG_H	350 
#define PTZ_DLG_X	((NVR_SCREEN_WIDTH - PTZ_DLG_W)>>1)
#define PTZ_DLG_Y	((NVR_SCREEN_HEIGHT - PTZ_DLG_H)>>1)
#define PTZ_TEXT_W	90
#define PTZ_EDIT_W	80
#define PTZ_EDIT_H	22

#define PTZ_PIC_START_X	50
#define PTZ_PIC_START_Y	(150)

#define PTZ_BUTT_W	35
#define PTZ_BUTT_H	35
#define PTZ_INCX	(0+PTZ_BUTT_W)
#define PTZ_INCY	(0+PTZ_BUTT_H)

#define BUTT_INC_W	22
#define BUTT_INC_H	22
#define BUTT_INC_START_X	(200+70)
#define BUTT_INC_START_Y	(100+20)
#define BUTT_INC_INCY	(BUTT_INC_H + 10)

typedef enum
{
	IDC_STATIC_PTZ_CH = 2500,
	IDL_PTZ_CH,
	
	IDC_STATIC_PTZ_SPEED,
	IDL_PTZ_SPEED,
	IDC_STATIC_ZOOM_SPEED,
	IDL_ZOOM_SPEED,
	IDC_PTZ_1,
	IDC_PTZ_2,
	IDC_PTZ_3,
	IDC_PTZ_4,
	IDC_PTZ_5,
	IDC_PTZ_6,
	IDC_PTZ_7,
	IDC_PTZ_8,
	IDC_PTZ_9,
	IDC_RATE_INC,
	IDC_STATIC_RATE,
	IDC_RATE_DEC,
	IDC_FOCUS_INC,
	IDC_STATIC_FOCUS,
	IDC_FOCUS_DEC,
	IDC_IRIS_INC,
	IDC_STATIC_IRIS,
	IDC_IRIS_DEC,
	IDC_PTZ_SETTING,
	IDC_MOUSE_PTZ_CTRL,
	IDC_PTZ_EXIT,
	IDC_STATIC_PERCENT,
	IDC_STA_PTZSET_PREST,
	IDC_STA_PTZSET_USE,
	IDC_STA_PTZSET_SET,
	IDC_COM_PTZSET_PREST
}UI_PTZ_ID;

typedef struct IPC_SPEED_S{
	int ptzSpeed;		//云台速度
	int zoomSpeed;		//变倍速度
}IPC_SPEED;

static IPC_SPEED ipcSpeed[NVR_MAX_CHANNEL_NUM] = {{50,0},{50,0},{50,0},{50,0}};//初始化速度


static BITMAP ptzBmpItem[15];

static const char *ptzPicName[] =
{
	"nvr_res/ptz/leftup.bmp",
	"nvr_res/ptz/up.bmp",
	"nvr_res/ptz/rightup.bmp",
	"nvr_res/ptz/left.bmp",
	"nvr_res/ptz/right.bmp",
	"nvr_res/ptz/leftdown.bmp",
	"nvr_res/ptz/down.bmp",
	"nvr_res/ptz/rightdown.bmp",
	"nvr_res/ptz/middle.bmp",
	"nvr_res/ptz/add.bmp",
	"nvr_res/ptz/sub.bmp"
};

static void PtzInitPic()
{
	int i;
	int bmpNum = TABLESIZE(ptzPicName);

	for(i=0; i<bmpNum; i++)
	{
		LoadBitmapFromFile(HDC_SCREEN, &ptzBmpItem[i], ptzPicName[i]);
	}
}

static void PtzUnInitPic()
{
	int i;
	int bmpNum = TABLESIZE(ptzPicName);

	for(i=0; i<bmpNum; i++)
	{
		UnloadBitmap(&ptzBmpItem[i]);
	}
	
}

static CTRLDATA ui_ptz_control[] =
{
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
         40, 60, PTZ_TEXT_W, PTZ_EDIT_H,
        IDC_STATIC_PTZ_CH,
        "当前通道",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP | CBS_NOTIFY,
        40+PTZ_EDIT_W , 60, PTZ_EDIT_W+20, GUI_COMBOX_H,
        IDL_PTZ_CH,
        "",
        80,
        WS_EX_USEPARENTFONT
    },
    //CONFIG_TWO_WORDS_BUTTON("通道1", IDB_PTZ_CH1, 40+PTZ_EDIT_W*2+10, 60),
    //CONFIG_TWO_WORDS_BUTTON("通道2", IDB_PTZ_CH2, 40+PTZ_EDIT_W*2+75, 60),
	//CONFIG_TWO_WORDS_BUTTON("通道3", IDB_PTZ_CH3, 40+PTZ_EDIT_W*2+140, 60),
	//CONFIG_TWO_WORDS_BUTTON("通道4", IDB_PTZ_CH4, 40+PTZ_EDIT_W*2+205, 60),

	/////////////////////////////////////////////////////////
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
         40, 60+30, PTZ_TEXT_W, PTZ_EDIT_H,
        IDC_STATIC_PTZ_SPEED,
        "云台速度",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
		40 + PTZ_EDIT_W,  60+30, 270, PTZ_EDIT_H,
        IDL_PTZ_SPEED,
        "",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        400, 60+30, 50, PTZ_EDIT_H,
        IDC_STATIC_PERCENT,
        "1",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        40, 60+30*2, PTZ_TEXT_W, PTZ_EDIT_H,
        IDC_STATIC_ZOOM_SPEED,
        "变倍速度",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP | CBS_NOTIFY,
        40+PTZ_EDIT_W , 60+30*2, PTZ_EDIT_W+20, GUI_COMBOX_H,
        IDL_ZOOM_SPEED,
        "",
        0,
        WS_EX_USEPARENTFONT
    },
	//////////////////////////////////////////////////////
	{
        GUI_PIC_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY | BS_BITMAP,
        PTZ_PIC_START_X , PTZ_PIC_START_Y, PTZ_BUTT_W, PTZ_BUTT_H,
        IDC_PTZ_1,
        "",
        (DWORD)&ptzBmpItem[0],
        //WS_EX_TRANSPARENT
    },
    {
        GUI_PIC_BUTTON,
        WS_CHILD | SS_LEFT | WS_VISIBLE | BS_NOTIFY | BS_BITMAP,//WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY | BS_BITMAP,
        PTZ_PIC_START_X + PTZ_INCX , PTZ_PIC_START_Y, PTZ_BUTT_W, PTZ_BUTT_H,
        IDC_PTZ_2,
        "",
        (DWORD)&ptzBmpItem[1],
        //WS_EX_TRANSPARENT
    },
     {
        GUI_PIC_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY | BS_BITMAP,
        PTZ_PIC_START_X + 2*PTZ_INCX , PTZ_PIC_START_Y, PTZ_BUTT_W, PTZ_BUTT_H,
        IDC_PTZ_3,
        "",
        (DWORD)&ptzBmpItem[2],
        //WS_EX_TRANSPARENT
    },
     {
        GUI_PIC_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY | BS_BITMAP,
        PTZ_PIC_START_X , PTZ_PIC_START_Y + PTZ_INCY, PTZ_BUTT_W, PTZ_BUTT_H,
        IDC_PTZ_4,
        "",
        (DWORD)&ptzBmpItem[3],
        //WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE |SS_BITMAP | SS_REALSIZEIMAGE,

        PTZ_PIC_START_X + PTZ_INCX , PTZ_PIC_START_Y + PTZ_INCY, PTZ_BUTT_W, PTZ_BUTT_H,
        IDC_PTZ_5,
        "",
        (DWORD)&ptzBmpItem[8],
        WS_EX_TRANSPARENT
    },
     {
        GUI_PIC_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY | BS_BITMAP,
        PTZ_PIC_START_X + 2*PTZ_INCX ,PTZ_PIC_START_Y + PTZ_INCY, PTZ_BUTT_W, PTZ_BUTT_H,
        IDC_PTZ_6,
        "",
        (DWORD)&ptzBmpItem[4],
        //WS_EX_TRANSPARENT
    },
    {
        GUI_PIC_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY | BS_BITMAP,
        PTZ_PIC_START_X , PTZ_PIC_START_Y + 2*PTZ_INCY, PTZ_BUTT_W, PTZ_BUTT_H,
        IDC_PTZ_7,
        "",
        (DWORD)&ptzBmpItem[5],
        //WS_EX_TRANSPARENT
    },
    {
        GUI_PIC_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY | BS_BITMAP,
		PTZ_PIC_START_X + PTZ_INCX , PTZ_PIC_START_Y + 2*PTZ_INCY, PTZ_BUTT_W, PTZ_BUTT_H,
        IDC_PTZ_8,
        "",
        (DWORD)&ptzBmpItem[6],
        //WS_EX_TRANSPARENT
    },
    {
        GUI_PIC_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY | BS_BITMAP,
		PTZ_PIC_START_X + 2*PTZ_INCX, PTZ_PIC_START_Y + 2*PTZ_INCY, PTZ_BUTT_W, PTZ_BUTT_H,
        IDC_PTZ_9,
        "",
        (DWORD)&ptzBmpItem[7],
        //WS_EX_TRANSPARENT
    },
	////////////////////////////////////////////////////////////////////////////////////
	{
	   GUI_PIC_BUTTON,
	   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY | BS_BITMAP,
	   BUTT_INC_START_X, BUTT_INC_START_Y, BUTT_INC_W, BUTT_INC_H,
	   IDC_RATE_INC,
	   "",
	   (DWORD)&ptzBmpItem[9],
       WS_EX_TRANSPARENT
	 },
	 {
        CTRL_STATIC,
        WS_CHILD | SS_CENTER | WS_VISIBLE,
        BUTT_INC_START_X + BUTT_INC_W+5, BUTT_INC_START_Y - (PTZ_EDIT_H - BUTT_INC_W)/2, PTZ_TEXT_W, PTZ_EDIT_H,
        IDC_STATIC_RATE,
        "变倍",
        0,
		WS_EX_TRANSPARENT
    },
	 {
	   GUI_PIC_BUTTON,
	   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY | BS_BITMAP,
	   BUTT_INC_START_X + BUTT_INC_W + PTZ_TEXT_W+5, BUTT_INC_START_Y, BUTT_INC_W, BUTT_INC_H,
	   IDC_RATE_DEC,
	   "",
	   (DWORD)&ptzBmpItem[10],
       WS_EX_TRANSPARENT
	 },   
	 {
		GUI_PIC_BUTTON,
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY | BS_BITMAP,
		BUTT_INC_START_X, BUTT_INC_START_Y + BUTT_INC_INCY, BUTT_INC_W, BUTT_INC_H,
		IDC_FOCUS_INC,
		"",
		(DWORD)&ptzBmpItem[9],
       	WS_EX_TRANSPARENT
	 },
	 {
        CTRL_STATIC,
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		BUTT_INC_START_X + BUTT_INC_W+5, BUTT_INC_START_Y - (PTZ_EDIT_H - BUTT_INC_W)/2 + BUTT_INC_INCY, PTZ_TEXT_W, PTZ_EDIT_H,
        IDC_STATIC_FOCUS,
        "聚焦",
        0,
		WS_EX_TRANSPARENT
    },
	{
	   GUI_PIC_BUTTON,
	   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY|BS_BITMAP,
	   BUTT_INC_START_X + BUTT_INC_W + PTZ_TEXT_W+5, BUTT_INC_START_Y + BUTT_INC_INCY , BUTT_INC_W, BUTT_INC_H,
	   IDC_FOCUS_DEC,
	   "",
	   (DWORD)&ptzBmpItem[10],
       WS_EX_TRANSPARENT
	},
	{
		GUI_PIC_BUTTON,
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY|BS_BITMAP,
		BUTT_INC_START_X, BUTT_INC_START_Y + 2*BUTT_INC_INCY, BUTT_INC_W, BUTT_INC_H,
		IDC_IRIS_INC,
		"",
		(DWORD)&ptzBmpItem[9],
        WS_EX_TRANSPARENT
	},
	{
		CTRL_STATIC,
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		BUTT_INC_START_X + BUTT_INC_W+5, BUTT_INC_START_Y - (PTZ_EDIT_H - BUTT_INC_W)/2 + 2*BUTT_INC_INCY, PTZ_TEXT_W, PTZ_EDIT_H,
		IDC_STATIC_IRIS,
		"光圈",
		0,
		WS_EX_TRANSPARENT
	},
	{
		GUI_PIC_BUTTON,
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY|BS_BITMAP,
		BUTT_INC_START_X + BUTT_INC_W + PTZ_TEXT_W+5, BUTT_INC_START_Y + 2*BUTT_INC_INCY , BUTT_INC_W, BUTT_INC_H,
		IDC_IRIS_DEC,
		"",
	    (DWORD)&ptzBmpItem[10],
        WS_EX_TRANSPARENT
	},
	////////////////////////////////////////////////////////////////
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        BUTT_INC_START_X, PTZ_DLG_H - 120, 60, BUTT_INC_H,
        IDC_STA_PTZSET_PREST,
        "预置点",
        0,
		WS_EX_TRANSPARENT
    },
    CONFIG_TWO_WORDS_BUTTON("设置", IDC_STA_PTZSET_SET, BUTT_INC_START_X, PTZ_DLG_H - 120+30),
    CONFIG_TWO_WORDS_BUTTON("调用", IDC_STA_PTZSET_USE, BUTT_INC_START_X+85, PTZ_DLG_H - 120+30),
	CONFIG_TWO_WORDS_BUTTON("退出", IDC_PTZ_EXIT, PTZ_PIC_START_X, PTZ_DLG_H - 50),

    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP |CBS_NOTIFY,
        BUTT_INC_START_X+60 , PTZ_DLG_H - 120, 80, GUI_COMBOX_H,
        IDC_COM_PTZSET_PREST,
        "",
        80,
        WS_EX_USEPARENTFONT
    },

};


static DLGTEMPLATE PtzDlg =
{
    WS_NONE,
    WS_EX_NONE,
    PTZ_DLG_X, PTZ_DLG_Y, PTZ_DLG_W, PTZ_DLG_H,
    "云台控制",
    0, 0,
    TABLESIZE(ui_ptz_control), NULL,
    0
};

static MSGHOOK	old_move_hook;


#define GUI_MAX_NAME_LEN 	64
typedef struct PresetPoint
{
	uint8_t  name[GUI_MAX_NAME_LEN];//预置点名称
	uint32_t preset;				//预置点号，
}GuiPresetPoint;

typedef struct PresetConfig
{
	uint32_t action;		        //预置点操作，详情见JPTZAction
	GuiPresetPoint pp;
}GuiPresetCon;

#if defined(HIDE_CURSER) || defined(LOCK_SCREEN)		
extern unsigned old_tick_count;
extern unsigned is_mouse_down;
#endif
/*支持在云台界面菜单栏用鼠标的拖动*/
static int move_probe_hook (void* context, HWND dst_wnd, int msg, WPARAM wParam, LPARAM lparam) 
{
	HWND hWnd;
	RECT ptzRect;
	static int location = HT_UNKNOWN;
	hWnd = (HWND)context;

#if defined(HIDE_CURSER) || defined(LOCK_SCREEN)		
	old_tick_count = GetTickCount ();
	if(msg == MSG_LBUTTONDOWN)
		is_mouse_down = 1;
	else if(msg == MSG_LBUTTONUP)
		is_mouse_down = 0;
#endif
	HWND ActiveWin = GetActiveWindow();
	if (IsDialog(ActiveWin))
	{
		if (msg == MSG_LBUTTONDOWN)
		{
			DRAGINFO drag_info;
			int x_pos = LOSWORD (lparam);
			int y_pos = HISWORD (lparam);

			GetWindowRect(hWnd, &ptzRect);
			ptzRect.bottom = ptzRect.top + 50;
			
			if(PtInRect(&ptzRect, x_pos, y_pos))
			{			
				location = HT_CAPTION;
                drag_info.location = location;
                drag_info.init_x = x_pos;
                drag_info.init_y = y_pos;
                
                SendMessage (HWND_DESKTOP, MSG_STARTDRAGWIN, 
                                (WPARAM)hWnd, (LPARAM)&drag_info);
				return HOOK_STOP;
			}
		}
	}

	return HOOK_GOON;
}


static int getCurposPreset(HWND hDlg)
{
	int curPos = 0;

	curPos = SendDlgItemMessage(hDlg, IDC_COM_PTZSET_PREST, CB_GETCURSEL, 0, 0);
	if(curPos<0 || curPos >=17)
		curPos = 0;

	return curPos;
}

static int operatePresetPoint(HWND hDlg, uint8_t chn, uint8_t index, uint16_t id)
{
	int ret = 0;

	if(id == IDC_STA_PTZSET_SET)
	{
		if(0 > (ret = Nvr_Client_Crtl_Ptz(PTZ_CMD_SET_PRESET, chn, 0, index)))
		{
			printf("[error]>>>>>>>.%s %d, ret = %d\n",__func__,__LINE__,ret);
			return -1;
		}
	}
	else if(id == IDC_STA_PTZSET_USE)
	{
		if(0 > (ret = Nvr_Client_Crtl_Ptz(PTZ_CMD_GOTO_PRESET, chn, 0, index)))
		{
			printf("[error]>>>>>>>.%s %d\n",__func__,__LINE__);
			return -1;
		}
	}
	
	return 0;
}

static void SetPtzZoomSpeed(HWND hDlg, int chn)
{
	uint8_t zoomSpeed = 0;

	zoomSpeed = SendDlgItemMessage(hDlg, IDL_ZOOM_SPEED, CB_GETCURSEL, 0, 0);
	printf("[info]<<<<<<<zoomSpeed = %d, chn = %d\n",zoomSpeed,chn);
	if((zoomSpeed < 0) || (zoomSpeed >= 3))
		zoomSpeed = 0;
	if(Nvr_Client_Crtl_Ptz(PTZ_CMD_ZOOM_SPEED, (uint8_t)chn, zoomSpeed, 0) < 0)
	{
		printf("[error]<<<<<set chan %d zoomspeed failed!\n",chn);
	}
	ipcSpeed[chn].zoomSpeed = zoomSpeed;
}


static void UpdatePtzParam(HWND hDlg, int chan)
{
	SendDlgItemMessage(hDlg, IDL_PTZ_CH, CB_SETCURSEL, chan, 0);
	
	SendDlgItemMessage (hDlg, IDL_PTZ_SPEED, TBM_SETPOS, ipcSpeed[chan].ptzSpeed, 0);	

	SendDlgItemMessage(hDlg, IDL_ZOOM_SPEED, CB_SETCURSEL, ipcSpeed[chan].zoomSpeed, 0);
}

static void InitPtzParam(HWND hWnd , int ch)
{
	int i=0;
	char buffer[8]={0};

	char acCh[16];	
	SendDlgItemMessage(hWnd,IDL_PTZ_CH, CB_RESETCONTENT, 0, (LPARAM)0);
	for (i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
	{	
		memset(acCh, 0, sizeof(acCh));
		sprintf(acCh, "%d", i+1);
		printf("acCh=%d\r\n", i);
		SendDlgItemMessage(hWnd, IDL_PTZ_CH, CB_ADDSTRING, 0, (LPARAM)LTR(acCh));		
	}
	
#if 0
	SendDlgItemMessage(hWnd,IDL_PTZ_SPEED, CB_RESETCONTENT, 0, (LPARAM)0);
	for (ptzspeed = 0; ptzspeed < 6; ptzspeed++)
	{
		sprintf(acSpeed, "%d", ptzspeed+1);
		SendDlgItemMessage(hWnd, IDL_PTZ_SPEED, CB_ADDSTRING, 0, (LPARAM)acSpeed);
	}
#endif 
	SendDlgItemMessage (hWnd, IDL_PTZ_SPEED, TBM_SETRANGE, 1, 100);

	//SendDlgItemMessage(hWnd, IDL_PTZ_SPEED, CB_SETCURSEL, 0, 0);
	SendDlgItemMessage(hWnd,IDL_ZOOM_SPEED, CB_RESETCONTENT, 0, (LPARAM)0);
	SendDlgItemMessage(hWnd, IDL_ZOOM_SPEED, CB_ADDSTRING, 0, (LPARAM)LTR("快速"));
	SendDlgItemMessage(hWnd, IDL_ZOOM_SPEED, CB_ADDSTRING, 0, (LPARAM)LTR("中速"));
	SendDlgItemMessage(hWnd, IDL_ZOOM_SPEED, CB_ADDSTRING, 0, (LPARAM)LTR("低速"));

	SendDlgItemMessage(hWnd,IDC_COM_PTZSET_PREST, CB_RESETCONTENT, 0, (LPARAM)0);
	for(i=1; i<=16; i++)
	{	
		sprintf(buffer,"%d",i);
		SendDlgItemMessage(hWnd, IDC_COM_PTZSET_PREST, CB_ADDSTRING, 0, (LPARAM)buffer);
	}
	SendDlgItemMessage(hWnd, IDC_COM_PTZSET_PREST, CB_SETCURSEL, 0, 0);
	UpdatePtzParam(hWnd, ch);

}

static void SetPtzPresetSpeed(HWND hDlg, WPARAM wParam, int chn, int ptzSpeed)
{
	uint16_t u16PtzCmd;

	int id = LOWORD(wParam);
	int code = HIWORD(wParam);
	
	switch(id)
	{
		case IDC_PTZ_1:	
			u16PtzCmd = PTZ_CMD_LEFT_UP;
			break;
		case IDC_PTZ_2:	
			u16PtzCmd = PTZ_CMD_UP;
			break;
		case IDC_PTZ_3:
			u16PtzCmd = PTZ_CMD_RIGHT_UP;
			break;
		case IDC_PTZ_4:
			u16PtzCmd = PTZ_CMD_LEFT;
			break;

		case IDC_PTZ_6:
			u16PtzCmd = PTZ_CMD_RIGHT;
			break;
		case IDC_PTZ_7:
			u16PtzCmd = PTZ_CMD_LEFT_DOWN;
			break;
		case IDC_PTZ_8:
			u16PtzCmd = PTZ_CMD_DOWN;
			break;
		case IDC_PTZ_9:	
			u16PtzCmd = PTZ_CMD_RIGHT_DOWN;
			break;
		case IDC_PTZ_5:
		default:
			return;  

	}
	
	if(code == BN_PUSHED)
	{
		if(Nvr_Client_Crtl_Ptz(u16PtzCmd, (uint8_t)chn, (uint8_t)ptzSpeed, 0) < 0)
		{
			printf("[error]<<<<<<<channel %d test ptz failed\n",chn);
		}
#if defined(HIDE_CURSER) || defined(LOCK_SCREEN)	
		old_tick_count = GetTickCount ();
		is_mouse_down = 1;
#endif		
	}
	else if(code == BN_UNPUSHED)
	{
		u16PtzCmd = PTZ_CMD_STOP;
		if(Nvr_Client_Crtl_Ptz(u16PtzCmd, (uint8_t)chn, 0, 0) < 0)
		{
			printf("[error]<<<<<<<channel %d test ptz failed\n",chn);
		}
#if defined(HIDE_CURSER) || defined(LOCK_SCREEN)
		old_tick_count = GetTickCount ();	
		is_mouse_down = 0;
#endif		
	}
		
}

static void SetZoomSpeed(HWND hDlg, WPARAM wParam, int chn)
{
	uint16_t u16PtzCmd = 0;

	int id = LOWORD(wParam);
	int code = HIWORD(wParam);
	

	if(id == IDC_RATE_INC)
		u16PtzCmd = PTZ_CMD_ZOOM_TELE;
	else if(id == IDC_RATE_DEC)
		u16PtzCmd = PTZ_CMD_ZOOM_WIDE;

	if(code == BN_PUSHED)
	{	
		if(Nvr_Client_Crtl_Ptz(u16PtzCmd, (uint8_t)chn, 0, 0) < 0)
		{
			printf("[error]<<<<<set chan %d zoomspeed inc failed!\n",chn);
			return;
		}			
	}
	else if(code == BN_UNPUSHED)
	{
		u16PtzCmd = PTZ_CMD_STOP;
		if(Nvr_Client_Crtl_Ptz(u16PtzCmd, (uint8_t)chn, 0, 0) < 0)
		{
			printf("[error]<<<<<<<channel %d test ptz failed\n",chn);
		}
	}
	
}
static void SetFocusSpeed(HWND hDlg, WPARAM wParam, int chn)
{
	uint16_t u16PtzCmd;
	int zoomSpeed;

	int id = LOWORD(wParam);
	int code = HIWORD(wParam);
	
	if(id == IDC_FOCUS_INC)
		u16PtzCmd = PTZ_CMD_FOCUS_NEAR;
	else if(id == IDC_FOCUS_DEC)
		u16PtzCmd = PTZ_CMD_FOCUS_FAR;

	if(code == BN_PUSHED)
	{
		zoomSpeed = SendDlgItemMessage(hDlg, IDL_ZOOM_SPEED, CB_GETCURSEL, 0, 0);
		if(zoomSpeed < 0 || zoomSpeed >= 3)
			zoomSpeed = 0;
		
		if(Nvr_Client_Crtl_Ptz(u16PtzCmd, chn, zoomSpeed, 0) < 0)
		{
			printf("[error]<<<<<set chan %d focus inc failed!\n",chn);
			return;
		}	
	}
	else if(code == BN_UNPUSHED)
	{
		u16PtzCmd = PTZ_CMD_STOP;
		if(Nvr_Client_Crtl_Ptz(u16PtzCmd, (uint8_t)chn, 0, 0) < 0)
		{
			printf("[error]<<<<<<<channel %d test ptz failed\n",chn);
		}
	}
}

static void SetIrisSpeed(HWND hDlg, WPARAM wParam, int chn)
{
	uint16_t u16PtzCmd;
	int zoomSpeed;

	int id = LOWORD(wParam);
	int code = HIWORD(wParam);
	
	if(id == IDC_IRIS_INC)
		u16PtzCmd = PTZ_CMD_IRIS_OPEN;
	else if(id == IDC_IRIS_INC)
		u16PtzCmd = PTZ_CMD_IRIS_CLOSE;

	if(code == BN_PUSHED)
	{
		zoomSpeed = SendDlgItemMessage(hDlg, IDL_ZOOM_SPEED, CB_GETCURSEL, 0, 0);
		if(zoomSpeed < 0 || zoomSpeed >= 3)
			zoomSpeed = 0;
	
		if(Nvr_Client_Crtl_Ptz(u16PtzCmd, chn, zoomSpeed, 0) < 0)
		{
			printf("[error]<<<<<set chan %d focus inc failed!\n",chn);
			return ;
		}
	}
	else if(code == BN_UNPUSHED)
	{
		u16PtzCmd = PTZ_CMD_STOP;
		if(Nvr_Client_Crtl_Ptz(u16PtzCmd, (uint8_t)chn, 0, 0) < 0)
		{
			printf("[error]<<<<<<<channel %d test ptz failed\n",chn);
		}
	}
}

static int TrackbarChanged(HWND hDlg, int id, int code)
{	
	char acCurPosBuffer[8];
	static int nCurrentPos = 1;	
	
	if (code == TBN_CHANGE)
	{
		nCurrentPos = SendDlgItemMessage (hDlg, id, TBM_GETPOS, 0, 0);		
		sprintf(acCurPosBuffer, "%d", nCurrentPos);
		SendDlgItemMessage (hDlg, IDC_STATIC_PERCENT,  MSG_SETTEXT, 0, (LPARAM)acCurPosBuffer);
	}

	return nCurrentPos;
}

static int PtzWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	static int cChan = 0;
	uint8_t curIndex = 0;
    switch (message)
    {

    case MSG_INITDIALOG:
    {
		if (lParam >=0 &&lParam < 4)
			cChan = lParam;
		
		InitPtzParam(hDlg, cChan);
		old_move_hook = RegisterMouseMsgHook ((void *)hDlg, move_probe_hook); 	
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0
	case MSG_PAINT:
	{
		HDC hdc;		
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg, "云台设置");
        break;
	}
#endif
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
		Gui_WriteTitleName(hdc, &title, LTR("云台设置"));
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
		int id = LOWORD(wParam);
		int code = HIWORD(wParam);
		if(id == IDL_PTZ_SPEED)
		{
			ipcSpeed[cChan].ptzSpeed = TrackbarChanged(hDlg, id, code);
			return 0;
		}else if(id == IDL_ZOOM_SPEED)
		{
			if(code == CBN_SELCHANGE)
			{
				SetPtzZoomSpeed(hDlg, cChan);
				return 0;
			}
		}
		else if(id == IDC_PTZ_EXIT)
		{
			PostMessage(hDlg, MSG_CLOSE, 0, 0);
			return 0;
		}
		else if (id == IDL_PTZ_CH)
		{		
			if (code == CBN_SELCHANGE)
			{	
				int nSelch;				
				nSelch = SendDlgItemMessage (hDlg, IDL_PTZ_CH, CB_GETCURSEL, 0, 0);
				if (nSelch < 0) return 0;
				SwitchPlayChan(hDlg, nSelch);
				
				UpdatePtzParam(hDlg, nSelch);
				cChan = nSelch;
				return 0;
			}
			break;
		}

		switch(id)
		{
			case IDC_PTZ_1:	
			case IDC_PTZ_2:	
			case IDC_PTZ_3:
			case IDC_PTZ_4:
			case IDC_PTZ_5:
			case IDC_PTZ_6:
			case IDC_PTZ_7:
			case IDC_PTZ_8:
			case IDC_PTZ_9:
				SetPtzPresetSpeed(hDlg, wParam, cChan, ipcSpeed[cChan].ptzSpeed); 
				break;						
			case IDC_RATE_INC:
			case IDC_RATE_DEC:
				SetZoomSpeed(hDlg, wParam, cChan);
				break;		
			case IDC_FOCUS_INC:
			case IDC_FOCUS_DEC:
				SetFocusSpeed(hDlg, wParam, cChan);
				break;
			case IDC_IRIS_INC:
			case IDC_IRIS_DEC:
				SetIrisSpeed(hDlg, wParam, cChan);
				break;
			//case IDC_PTZ_SETTING:
				//CreatePtzSetDlgWin(hDlg,chn);
				//break;
				
			case IDC_STA_PTZSET_SET:
			case IDC_STA_PTZSET_USE:
				curIndex = getCurposPreset(hDlg);
				operatePresetPoint(hDlg, cChan, curIndex+1, id);
			break;
			default:
			break;

		}
        break;
    }
    case MSG_CLOSE:
    {	
    	RegisterMouseMsgHook(NULL, old_move_hook);	
		HWND hWind;
		pop_open_dlg(&headOpenDlg, &hWind);

        EndDialog (hDlg, 0);
        break;
    }

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

//远程设备配置
void CreatePtzDlgWin(HWND hParent, int chn)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "PtzDlg" , 0 , 0);
#endif
	PtzInitPic();
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_ptz_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_ptz_control, sizeof(ui_ptz_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_ptz_control)
    PtzDlg.controls = ui_ptz_control;
    
    DialogBoxIndirectParam (&PtzDlg, hParent, PtzWinProc, chn);
	PtzUnInitPic();
	memcpy(ui_ptz_control, ctrolTmp, sizeof(ui_ptz_control));
    free(ctrolTmp);
    ctrolTmp = NULL;
}

