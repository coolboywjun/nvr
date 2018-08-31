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
#include "ui_dlg_head.h"
#include "nvr_lib_interface.h"
#include "gui_core.h"
#include "update.h"
#include "syscfg.h"


#define UI_SYSINFO_DLG_W		700
#define UI_SYSINFO_DLG_H		500

#define UI_SYSINFO_DLG_X	((NVR_SCREEN_WIDTH-UI_SYSINFO_DLG_W)/2)	
#define UI_SYSINFO_DLG_Y	((NVR_SCREEN_HEIGHT-UI_SYSINFO_DLG_H)/2)
#define DLG_SYSINFO_TEXT_X	50
#define DLG_SYSINFO_TEXT_Y  60
#define DLG_TEXT_W			(90+70)
#define DLG_TEXT_H			20
#define DLG_EDIT_W			150
#define DLG_ENC_Y		(DLG_TEXT_H + 10)

#define SYSQRCODE_START_X	540
#define SYSQRCODE_PIC_W		100

typedef enum
{
	IDC_STATIC_RECCHANNEL = 3100,
	IDC_STATIC_ALARM_INPUT,
	IDC_STATIC_ALARM_OUTPUT,
	IDC_STATIC_SYSTEM_VER,
	IDC_STATIC_GUI_VER,
	IDC_STATIC_RELEASE_TIME,
	IDC_STATIC_WEB_VER,
	IDC_STATIC_SERIAL_NUM,
	IDC_STATIC_SYSTEM_UPDATE,
	IDC_STATIC_UPDATE_INFO,
	IDC_STATIC_START,
	IDC_STATIC_UPDATE_PATH,
	IDC_STATIC_RECOER_DEF,
	IDC_STATIC_FACTORY_TEST,
	IDC_EDIT_RECCHANNEL,
	IDC_EDIT_ALARM_INPUT,
	IDC_EDIT_ALARM_OUTPUT,
	IDC_EDIT_SYSTEM_VER,
	IDC_EDIT_GUI_VER,
	IDC_EDIT_RELEASE_TIME,
	IDC_EDIT_WEB_VER,
	IDC_EDIT_SERIAL_NUM,
	IDC_EDIT_UPDATE_PATH,
	/*IDC_EDIT_UPTIME,
	ID_UPTIME_REFRESH,*/
	ID_UPTIME_FINDUSB,
	ID_ANDROID_DOWNLOAD,
}UI_SYSINFO_ID;

static RECT system_info_title_rect = {0,0,UI_SYSINFO_DLG_W,50};



static CTRLDATA ui_sysinfo_control[] =
{
	{
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        DLG_SYSINFO_TEXT_X, DLG_SYSINFO_TEXT_Y, DLG_TEXT_W, DLG_TEXT_H,
        IDC_STATIC_RECCHANNEL,
        "录像通道数",
        0,
		WS_EX_TRANSPARENT
    },
     {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE | ES_READONLY,
        DLG_SYSINFO_TEXT_X+DLG_TEXT_W, DLG_SYSINFO_TEXT_Y, DLG_EDIT_W, DLG_TEXT_H,
        IDC_EDIT_RECCHANNEL,
        "",
        0,
		WS_EX_TRANSPARENT
    },
    
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        DLG_SYSINFO_TEXT_X, DLG_SYSINFO_TEXT_Y+DLG_ENC_Y, DLG_TEXT_W, DLG_TEXT_H,
        IDC_STATIC_ALARM_INPUT,
        "报警输入数",
        0,
		WS_EX_TRANSPARENT
    },
     {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE| ES_READONLY,
        DLG_SYSINFO_TEXT_X+DLG_TEXT_W, DLG_SYSINFO_TEXT_Y+DLG_ENC_Y, DLG_EDIT_W, DLG_TEXT_H,
        IDC_EDIT_ALARM_INPUT,
        "",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
         DLG_SYSINFO_TEXT_X, DLG_SYSINFO_TEXT_Y+2*(DLG_ENC_Y), DLG_TEXT_W, DLG_TEXT_H,
        IDC_STATIC_ALARM_OUTPUT,
        "报警输出数",
        0,
		WS_EX_TRANSPARENT
    },
     {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE| ES_READONLY,
        DLG_SYSINFO_TEXT_X+DLG_TEXT_W, DLG_SYSINFO_TEXT_Y+2*DLG_ENC_Y, DLG_EDIT_W, DLG_TEXT_H,
        IDC_EDIT_ALARM_OUTPUT,
        "",
        0,
		WS_EX_TRANSPARENT
    },
    
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
         DLG_SYSINFO_TEXT_X, DLG_SYSINFO_TEXT_Y+3*(DLG_ENC_Y), DLG_TEXT_W, DLG_TEXT_H,
        IDC_STATIC_SYSTEM_VER,
        "系统版本",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE| ES_READONLY,
        DLG_SYSINFO_TEXT_X+DLG_TEXT_W, DLG_SYSINFO_TEXT_Y+3*DLG_ENC_Y, DLG_EDIT_W, DLG_TEXT_H,
        IDC_EDIT_SYSTEM_VER,
        "",
        0,
		WS_EX_TRANSPARENT
    },
    //////////
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
         DLG_SYSINFO_TEXT_X, DLG_SYSINFO_TEXT_Y+4*(DLG_ENC_Y), DLG_TEXT_W, DLG_TEXT_H,
        IDC_STATIC_GUI_VER,
        "GUI版本",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE| ES_READONLY,
        DLG_SYSINFO_TEXT_X+DLG_TEXT_W, DLG_SYSINFO_TEXT_Y+4*DLG_ENC_Y, DLG_EDIT_W, DLG_TEXT_H,
        IDC_EDIT_GUI_VER,
        "",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
         DLG_SYSINFO_TEXT_X, DLG_SYSINFO_TEXT_Y+5*(DLG_ENC_Y), DLG_TEXT_W, DLG_TEXT_H,
        IDC_STATIC_RELEASE_TIME,
        "编译日期",
        0,
		WS_EX_TRANSPARENT
    },
     {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE| ES_READONLY,
        DLG_SYSINFO_TEXT_X+DLG_TEXT_W, DLG_SYSINFO_TEXT_Y+5*DLG_ENC_Y, DLG_EDIT_W + 200, DLG_TEXT_H,
        IDC_EDIT_RELEASE_TIME,
        "",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
         DLG_SYSINFO_TEXT_X, DLG_SYSINFO_TEXT_Y+6*(DLG_ENC_Y), DLG_TEXT_W, DLG_TEXT_H,
        IDC_STATIC_SERIAL_NUM,
        "设备ID",
        0,
		WS_EX_TRANSPARENT
    },
     {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE| ES_READONLY,
        DLG_SYSINFO_TEXT_X+DLG_TEXT_W, DLG_SYSINFO_TEXT_Y+6*DLG_ENC_Y, DLG_EDIT_W, DLG_TEXT_H,
        IDC_EDIT_SERIAL_NUM,
        "",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
         DLG_SYSINFO_TEXT_X + 390, DLG_SYSINFO_TEXT_Y, DLG_TEXT_W-80, DLG_TEXT_H,
        IDC_STATIC,
        "设备ID",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
         DLG_SYSINFO_TEXT_X+390, DLG_SYSINFO_TEXT_Y+5*(DLG_ENC_Y)-10, DLG_TEXT_W-80, DLG_TEXT_H+20,
        ID_ANDROID_DOWNLOAD,
        "",
        0,
		WS_EX_TRANSPARENT
    },
/*	{
	   CTRL_STATIC,
	   WS_CHILD | SS_LEFT | WS_VISIBLE,
		DLG_SYSINFO_TEXT_X, DLG_SYSINFO_TEXT_Y+7*(DLG_ENC_Y), DLG_TEXT_W, DLG_TEXT_H,
	   IDC_STATIC,
	   "运行时间",
	   0,
	   WS_EX_TRANSPARENT
	},
	{
	   CTRL_STATIC,
	   WS_CHILD | SS_LEFT | WS_VISIBLE| ES_READONLY,
	   DLG_SYSINFO_TEXT_X+DLG_TEXT_W, DLG_SYSINFO_TEXT_Y+7*DLG_ENC_Y, DLG_EDIT_W, DLG_TEXT_H,
	   IDC_EDIT_UPTIME,
	   "",
	   0,
	   WS_EX_TRANSPARENT
	},
*/
	//////////////////
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | SS_GROUPBOX | WS_VISIBLE,
        DLG_SYSINFO_TEXT_X, DLG_SYSINFO_TEXT_Y+8*(DLG_ENC_Y), 600, 90,
        IDC_STATIC_SYSTEM_UPDATE,
        "系统升级",
        0,
		WS_EX_TRANSPARENT
    },	
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        DLG_SYSINFO_TEXT_X+10, DLG_SYSINFO_TEXT_Y+9*(DLG_ENC_Y), 590, 60,
        IDC_STATIC_UPDATE_INFO,
        "如果你现在需要通过USB设备对本机进行升级，请插入USB设备，按 \"开始\" 按钮启动升级，在升级的过程中请不要关闭电源。",
        0,
		WS_EX_TRANSPARENT
    },	
     {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        DLG_SYSINFO_TEXT_X, DLG_SYSINFO_TEXT_Y+11*(DLG_ENC_Y)+15, DLG_TEXT_W+20, DLG_TEXT_H,
        IDC_STATIC_UPDATE_PATH,
        "升级文件路径: ",
        0,
		WS_EX_TRANSPARENT
    },	

    ////////////////////////////////////////////////////////////
     {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP,
        DLG_SYSINFO_TEXT_X+DLG_TEXT_W+30, DLG_SYSINFO_TEXT_Y+11*(DLG_ENC_Y)+15, 400, GUI_COMBOX_H,
        IDC_EDIT_UPDATE_PATH,
        "",
        0,
        WS_EX_USEPARENTFONT
    },
    //////////////////// GUI_BUTTON ////////////////////

    CONFIG_TWO_WORDS_BUTTON("开始", IDC_STATIC_START, DLG_SYSINFO_TEXT_X+DLG_TEXT_W+30+400+20, DLG_SYSINFO_TEXT_Y+11*(DLG_ENC_Y)+15),
    CONFIG_THREE_WORDS_BUTTON("NVR测试", IDC_STATIC_FACTORY_TEST, 440, UI_SYSINFO_DLG_H - 50),
    CONFIG_LONG_WORDS_BUTTON("恢复出厂设置", IDC_STATIC_RECOER_DEF, 550, UI_SYSINFO_DLG_H - 50),

};

static BOOL curUsbState = FALSE;
static BOOL preUsbState = FALSE;
static char filePath[128] = {0};
static char fileName[32] = {0};

static DLGTEMPLATE SysemInfoDlg =
{
    WS_NONE,
    WS_EX_NONE,
    UI_SYSINFO_DLG_X, UI_SYSINFO_DLG_Y, UI_SYSINFO_DLG_W, UI_SYSINFO_DLG_H,
    "远程设备",
    0, 0,
    TABLESIZE(ui_sysinfo_control), NULL,
    0
};
#define DEVID_QR_PATH	"/tmp/devId.png"

char	usrName[NVR_USER_NAME_LEN] = {0};	//用户名
char	usrPsw[NVR_PASS_WORD_LEN] = {0};		//密码
extern int GetLoginUserAndPwd(char * name, char * psw);

/*
static void TimeToString(DWORD dwTime, char *strText, int TextBufLen)
{
	int      nTmp;
	memset(strText, 0, TextBufLen);
	if (dwTime == 0)
		return;

	int nDay = dwTime/(3600*24);
	nTmp = dwTime%(3600*24);
	int nHour = nTmp/3600;
	nTmp %= 3600;
	int nMin = nTmp/60;
	int nSec = nTmp%60;
	//if (nDay > 1)
	//	sprintf(strText, "%dDays %02d:%02d:%02d",nDay, nHour, nMin, nSec);
	//else	
	sprintf(strText, "%d天 %02d:%02d:%02d",nDay, nHour, nMin, nSec);
}

static void UpdateUptime(HWND hDlg)
{
	char acUptime[64];
	TimeToString(GUI_GetUptime(), acUptime, sizeof(acUptime)-1);
	SetDlgItemText(hDlg, IDC_EDIT_UPTIME, acUptime);
}
*/
static void UpdateUsbState(HWND hDlg)
{
	char diskpath[128]={0};

	if(nvr_update_udisk_get_file(filePath, fileName) < 0)
	{
		preUsbState = curUsbState;
		curUsbState = FALSE;
	}
	else
	{
		preUsbState = curUsbState;
		curUsbState = TRUE;

	}

	if(curUsbState)
	{
		strcpy(diskpath, filePath);
		EnableDlgItem(hDlg, IDC_EDIT_UPDATE_PATH, TRUE);
	}
	else
	{
		if(preUsbState)
		{
			nvr_update_udisk_release_file(filePath, fileName);
		}
		sprintf(diskpath, LTR("未找到升级文件"));
		EnableDlgItem(hDlg, IDC_EDIT_UPDATE_PATH, FALSE);
	}

	SendDlgItemMessage(hDlg,IDC_EDIT_UPDATE_PATH, CB_RESETCONTENT, 0, (LPARAM)0);
	SendDlgItemMessage(hDlg, IDC_EDIT_UPDATE_PATH, CB_ADDSTRING, 0, (LPARAM)diskpath);
	SendDlgItemMessage(hDlg, IDC_EDIT_UPDATE_PATH, CB_SETCURSEL, 0, 0);
	
}

static void InitSysInfoParam(HWND hDlg)
{
	char port[8];
	char datetime[64];
	HWND chnWnd,alarmInWnd,alarmOutWnd,sysInfoWnd,guiVerWnd,releaseTimeWnd,serialWnd;//fileWnd;
	NVR_DEV_INFO_S devInfo;
	
	memset(&devInfo, 0, sizeof(NVR_DEV_INFO_S));
	if(nvr_logon_get_dev_cfg(0, &devInfo) < 0)
		printf("[error] get dev cfg\n");

	printf("channel %d\n",devInfo.stStableInfo.u8ChnNum);
	sprintf(port, "%d", devInfo.stStableInfo.u8ChnNum);
	chnWnd = GetDlgItem(hDlg, IDC_EDIT_RECCHANNEL);
	SetWindowText(chnWnd, port);

	printf("alarm input num : %d\n",devInfo.stStableInfo.u8AlarmInputNum);
	sprintf(port, "%d", devInfo.stStableInfo.u8AlarmInputNum);
	alarmInWnd = GetDlgItem(hDlg, IDC_EDIT_ALARM_INPUT);
	SetWindowText(alarmInWnd, port);
		
	printf("alarm input num : %d\n",devInfo.stStableInfo.u8AlarmOutputNum);
	sprintf(port, "%d", devInfo.stStableInfo.u8AlarmOutputNum);
	alarmOutWnd = GetDlgItem(hDlg, IDC_EDIT_ALARM_OUTPUT);
	SetWindowText(alarmOutWnd, port);

	printf("soft version : %s\n",devInfo.stStableInfo.szSoftVer);
	sysInfoWnd = GetDlgItem(hDlg, IDC_EDIT_SYSTEM_VER);
	SetWindowText(sysInfoWnd, devInfo.stStableInfo.szSoftVer);

	printf("gui version : %s\n",GUI_VERSION);

	guiVerWnd = GetDlgItem(hDlg, IDC_EDIT_GUI_VER);
	SetWindowText(guiVerWnd, GUI_VERSION);

	releaseTimeWnd = GetDlgItem(hDlg, IDC_EDIT_RELEASE_TIME);
	sprintf(datetime, "%s %s", __DATE__, __TIME__);
	SetWindowText(releaseTimeWnd, datetime);

	printf("serial id : %s\n",devInfo.stStableInfo.szSerialId);

	if(strlen(devInfo.stStableInfo.szSerialId) > 0)
	{
		serialWnd = GetDlgItem(hDlg, IDC_EDIT_SERIAL_NUM);
		SetWindowText(serialWnd, devInfo.stStableInfo.szSerialId);
	}
	printf("netNum %d\n",devInfo.stStableInfo.u8NetNum);
	//UpdateUptime(hDlg);
	UpdateUsbState(hDlg);
	
	return;
}

static int ShowSerialIdQrcode(HWND hDlg, HDC hdc, const char *qrcode_str)
{
	RECT rect;
	unsigned int width,height;
	
	create_qrcode_png(qrcode_str, DEVID_QR_PATH, &width, &height);
	LoadBitmapFromFile(HDC_SCREEN, &devidmap, DEVID_QR_PATH);
	GetClientRect (hDlg, &rect);
	SetBrushColor(hdc, PIXEL_EDIT_BKG_NORMAL);
	FillBox (hdc, SYSQRCODE_START_X, DLG_SYSINFO_TEXT_Y, SYSQRCODE_PIC_W, SYSQRCODE_PIC_W);

#if 0
	FillBoxWithBitmap (hdc, rect.right-width-5-60, DLG_SYSINFO_TEXT_Y+5, width, height, &devidmap);
#else
	HDC hDCMem = CreateCompatibleDCEx(hdc, devidmap.bmWidth, devidmap.bmHeight);
	FillBoxWithBitmap(hDCMem, 0, 0, devidmap.bmWidth, devidmap.bmHeight, &devidmap);
	StretchBlt(hDCMem, 0, 0, devidmap.bmWidth, devidmap.bmHeight, hdc, SYSQRCODE_START_X+5, DLG_SYSINFO_TEXT_Y+5,
	SYSQRCODE_PIC_W-10, SYSQRCODE_PIC_W-10,0);
#endif
	UnloadBitmap(&devidmap);

	return 0;
}
static int ShowDownloadQrcode(HWND hDlg, HDC hdc, BITMAP *bitmap)
{
	HWND androidHwnd;
	
	androidHwnd = GetDlgItem(hDlg, ID_ANDROID_DOWNLOAD);
	SetWindowText(androidHwnd, LTR("ANDROID客户端"));

	FillBox (hdc, SYSQRCODE_START_X, DLG_SYSINFO_TEXT_Y+5*(DLG_ENC_Y)-10, SYSQRCODE_PIC_W, SYSQRCODE_PIC_W);

	HDC hDCMem = CreateCompatibleDCEx(hdc, bitmap->bmWidth, bitmap->bmHeight);
	FillBoxWithBitmap(hDCMem, 0, 0, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	StretchBlt(hDCMem, 0, 0, bitmap->bmWidth, bitmap->bmHeight, hdc, SYSQRCODE_START_X+5, DLG_SYSINFO_TEXT_Y+5*(DLG_ENC_Y)-10+5,
	SYSQRCODE_PIC_W-10, SYSQRCODE_PIC_W-10,0);

	return 0;
}
static int ShowQrcode(HWND hDlg, HDC hdc)
{		
	DEV_SystemCfg *pSystemCfg;
	NVR_DEV_INFO_S devInfo = {0};
	char qrcode_str[512] = {0};

	if(GetLoginUserAndPwd(usrName, usrPsw) < 0)
		return -1;
		
	//printf("@@@@@@@@@@@@@@@@@@%s[%d]: name: %s, psw: %s\r\n",__FILE__,__LINE__, usrName, usrPsw);
		
	nvr_logon_get_dev_cfg(0, &devInfo);
	pSystemCfg = get_system_cfg();	
	if(strstr(devInfo.stStableInfo.szSerialId, "JXJ"))
	{
		sprintf(qrcode_str, "DNNVR,ID%s,CP3321,SP7554,TP3322,TY2,UN%s,PW%s,CC%d",devInfo.stStableInfo.szSerialId,
				usrName, usrPsw, devInfo.stStableInfo.u8ChnNum);
		ShowSerialIdQrcode(hDlg, hdc, qrcode_str);
		ShowDownloadQrcode(hDlg, hdc, &androidmap_vivi);
	}
	else if(strstr(devInfo.stStableInfo.szSerialId, "TCVNVR"))
	{
		ShowSerialIdQrcode(hDlg, hdc, devInfo.stStableInfo.szSerialId);
		ShowDownloadQrcode(hDlg, hdc, &androidmap_tutk);
	}
	else
	{
		if(pSystemCfg->platformServer & PLATFORM_VIVI_SERVER)
		{
			sprintf(qrcode_str, "DNNVR,ID%s,CP3321,SP7554,TP3322,TY2,UN%s,PW%s,CC%d",devInfo.stStableInfo.szSerialId,
					usrName, usrPsw, devInfo.stStableInfo.u8ChnNum);
			ShowSerialIdQrcode(hDlg, hdc, qrcode_str);
			ShowDownloadQrcode(hDlg, hdc, &androidmap_vivi);
		}
		else if(pSystemCfg->platformServer & PLATFORM_TUTK_SERVER)
		{
			ShowSerialIdQrcode(hDlg, hdc, devInfo.stStableInfo.szSerialId);
			ShowSerialIdQrcode(hDlg, hdc, qrcode_str);
			ShowDownloadQrcode(hDlg, hdc, &androidmap_tutk);
		}
		else
		{
			ShowSerialIdQrcode(hDlg, hdc, devInfo.stStableInfo.szSerialId);
		}
	}
	return 0;
}

static int SystemInfoWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int ret = 0;
	
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		InitSysInfoParam(hDlg);
		//if (!IsTimerInstalled(hDlg, ID_UPTIME_REFRESH))
		//	SetTimer(hDlg, ID_UPTIME_REFRESH, 50);
		
		if(!IsTimerInstalled(hDlg, ID_UPTIME_FINDUSB))
			SetTimer(hDlg, ID_UPTIME_FINDUSB, 200);

		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }

	case MSG_PAINT:
	{
       	HDC hdc;
       	hdc = BeginPaint (hDlg);	
		ShowQrcode(hDlg, hdc);
       	EndPaint (hDlg, hdc);		
		break;
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
		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bk_pic[SIZE_700x500]); 
		Gui_WriteTitleName(hdc, &system_info_title_rect, LTR("系统信息"));
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
			case IDC_STATIC_START://升级操作
			{
				if(curUsbState)
				{
					if (IsTimerInstalled(hDlg ,ID_UPTIME_FINDUSB))//升级过程中停止检查usb设备
						KillTimer(hDlg, ID_UPTIME_FINDUSB);
					CreateUpdateDlgWin(hDlg, filePath,fileName);
				}
				else
#ifdef OWN_MESSAGEBOX_WIN
					CreateMessageBoxWin(hDlg, LTR("未找到升级文件!"), LTR("消息"), MB_OK);
#else
					MessageBox(hDlg, LTR("未找到升级文件!"), LTR("消息"), MB_OK);
#endif
				return 0;
			}
			case IDC_STATIC_RECOER_DEF:
			{
#ifdef OWN_MESSAGEBOX_WIN
				ret = CreateMessageBoxWin(hDlg, LTR("恢复出厂设置后，系统将重启!"), LTR("消息"), MB_OKCANCEL);
#else
				ret = MessageBox(hDlg, LTR("恢复出厂设置后，系统将重启!"), LTR("消息"), MB_OKCANCEL);
#endif
				if(ret == IDOK)
				{
					nvr_sw_reset();  //恢复出厂设置
					//PostMessage(hDlg, MSG_CLOSE, 0, 0);
					SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
					return 0;
				}
				else if(ret == IDCANCEL)
				{
					return 0;
				}
				
				break;
			}
			case IDC_STATIC_FACTORY_TEST:
			{
			//	ret = CreateMessageBoxWin(hDlg, LTR("NVR测试完成后，系统需要重启之后才能正常使用!"), LTR("消息"), MB_OKCANCEL);
			//	if(IDOK == ret)
				{
					if (IsTimerInstalled(hDlg ,ID_UPTIME_FINDUSB))//进入子窗口停止检查usb设备
						KillTimer(hDlg, ID_UPTIME_FINDUSB);

					CreateNvrTestDlgWin(hDlg);
					
					if(!IsTimerInstalled(hDlg, ID_UPTIME_FINDUSB))
					SetTimer(hDlg, ID_UPTIME_FINDUSB, 200);
				}
				return 0;
			}
			break;
		}
		break;
    }
	case MSG_TIMER:
	{
		switch(wParam)
		{
		/*	case ID_UPTIME_REFRESH:
				UpdateUptime(hDlg);
				break;
			*/case ID_UPTIME_FINDUSB:
				UpdateUsbState(hDlg);
				break;
			default:
				return 0;
		}
	}
	break;
    case MSG_CLOSE:
    {	
		//if (IsTimerInstalled(hDlg ,ID_UPTIME_REFRESH))
		//	KillTimer(hDlg, ID_UPTIME_REFRESH);
		if (IsTimerInstalled(hDlg ,ID_UPTIME_FINDUSB))
			KillTimer(hDlg, ID_UPTIME_FINDUSB);
		if(curUsbState)
			nvr_update_udisk_release_file(NULL, NULL);

    	HWND hWind;
    	pop_open_dlg(&headOpenDlg, &hWind);
        EndDialog (hDlg, 0);
        break;
    }

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static void AdjustPositionDlgWin()
{
	int index,i;

	GetDisplayLanguageIndex(&index);
	if(0 == index) //汉字
	{
		for(i=0; i<=13; i++)
		{
			if(i%2 == 1)
			{
				ui_sysinfo_control[i].x -= 70;
			}
		}

	}
	else
	{
		ui_sysinfo_control[21].x += 20;
		ui_sysinfo_control[21].w = FOUR_WORDS_BUTTON_W;
		ui_sysinfo_control[21].dwAddData = (DWORD)&button_pic[2];
	}

	ui_sysinfo_control[19].x -= 70;
	ui_sysinfo_control[20].x -= 70;
	
}

void CreateSystemInfoDlgWin(HWND hParent)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "SystemInfoDlg" , 0 , 0);
#endif 
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_sysinfo_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_sysinfo_control, sizeof(ui_sysinfo_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_sysinfo_control)
	AdjustPositionDlgWin();
    SysemInfoDlg.controls = ui_sysinfo_control;
    
    DialogBoxIndirectParam (&SysemInfoDlg, hParent, SystemInfoWinProc, 0L);
	memcpy(ui_sysinfo_control, ctrolTmp, sizeof(ui_sysinfo_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

