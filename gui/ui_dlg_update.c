#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>
#include <minigui/skin.h>

#include "ui_config.h"
#include "ctrl_progressbar.h"

#include "myedit.h"
#include "ctrl_ipaddress.h"
#include "ui_dlg_head.h"
#include "nvr_lib_interface.h"
#include "gui_core.h"
#include "update.h"

#define BUF_SIZE            1024
#define UI_UPDATE_DLG_W		500
#define UI_UPDATE_DLG_H		150
#define UI_UPDATE_DLG_X		((NVR_SCREEN_WIDTH-UI_UPDATE_DLG_W)/2)	
#define UI_UPDATE_DLG_Y		((NVR_SCREEN_HEIGHT-UI_UPDATE_DLG_H)/2)
#define DLG_UPDATE_TEXT_X	50
#define DLG_UPDATE_TEXT_Y  	60
#define DLG_TEXT_W			90
#define DLG_TEXT_H			20
#define DLG_ENC_Y			(DLG_TEXT_H + 10)

typedef enum
{
	IDC_STATIC_PROGRESS = 3300,
	IDC_UPDATE_PROGRESS,
	IDC_STATIC_INFO,
	IDC_STATIC_PERCENT,
	UPDATE_TIMER,
	
}UI_UPDATE_ID;


static HWND upProgressBar = HWND_INVALID;
static char filePath[128] = {0};
static char fileName[128] = {0};
int uPercent = 0;

static CTRLDATA ui_update_control[] =
{
	{
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        DLG_UPDATE_TEXT_X, DLG_UPDATE_TEXT_Y, DLG_TEXT_W+200, DLG_TEXT_H,
        IDC_STATIC_PROGRESS,
        "温馨提示: 系统正在升级，请耐心等待...",
        0,
		WS_EX_TRANSPARENT
    },
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
         DLG_UPDATE_TEXT_X, DLG_UPDATE_TEXT_Y+DLG_ENC_Y, DLG_TEXT_W, DLG_TEXT_H,
        IDC_STATIC_PROGRESS,
        "当前进度",
        0,
		WS_EX_TRANSPARENT
    },

    {
		CTRL_PROGRESSBAR_EX,
		WS_VISIBLE | PBS_NOTIFY,		
        DLG_UPDATE_TEXT_X + DLG_TEXT_W - 20, DLG_UPDATE_TEXT_Y+DLG_ENC_Y,300, DLG_TEXT_H,
        IDC_UPDATE_PROGRESS,
        "0",
        0,
        WS_EX_USEPARENTCURSOR
	},
#if 0	
	{
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        DLG_UPDATE_TEXT_X+ DLG_TEXT_W+300, DLG_UPDATE_TEXT_Y+DLG_ENC_Y, 30, DLG_TEXT_H,
        IDC_STATIC_PERCENT,
        "",
        0,
		WS_EX_TRANSPARENT
    },
#endif
};

static DLGTEMPLATE UpdateDlg =
{
    WS_NONE,
    WS_EX_NONE,
    UI_UPDATE_DLG_X, UI_UPDATE_DLG_Y, UI_UPDATE_DLG_W, UI_UPDATE_DLG_H,
    "远程设备",
    0, 0,
    TABLESIZE(ui_update_control), NULL,
    0
};

void *updateFunc(void *arg)
{
	FILE *fp;
	long nTotalLen = 0; 
	char buffer[1024] = {0};
	int rlen = 0;
	int nPackNo = 0;

	//HWND *hDlg = (HWND *)arg;
	
	fp = fopen(filePath, "rb");
	if(NULL == fp)
	{
		printf("[error]<<<<<printf file error\n");
		return (void *)0;
	}
	fseek(fp, 0, SEEK_END);
	nTotalLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	while((rlen = fread(buffer, 1, BUF_SIZE, fp)) > 0)
	{
		
		if(nvr_update_write(buffer, rlen, nPackNo++, nTotalLen) < 0)
		{
			nvr_update_udisk_release_file(filePath, fileName);
			printf("[error]<<<<<nvr_update_write failed\n");
			return (void *)0;
		}
		uPercent = nvr_update_get_percent();

		memset(buffer, 0, 1024);
	}
	
	nvr_update_write(buffer, 0, nPackNo, nTotalLen);
	nvr_update_udisk_release_file(filePath, fileName);
	fclose(fp);
	
	return (void *)0;

}
static int startUpdate(HWND hDlg)
{
	pthread_t pthid;
	
    pthread_attr_t at;
    pthread_attr_init(&at); //初始化线程属性
    pthread_attr_setdetachstate(&at,PTHREAD_CREATE_DETACHED);


	if(pthread_create(&pthid, &at, updateFunc, NULL) < 0)
	{
		printf("[error]<<<< create update pthread failed\n");
		return (-1);
	}
	return 0;
}

static void UpdateStop(HWND hDlg)
{
	uPercent = 0;
	upProgressBar = HWND_INVALID;
	KillTimer(hDlg, UPDATE_TIMER);
}

static int UpdateWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	//char port[8] = {0};
	//HWND percentWnd;
	
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		GetParent(hDlg);
		upProgressBar = GetDlgItem(hDlg, IDC_UPDATE_PROGRESS);
		SendMessage(upProgressBar, PBM_SETRANGE, 0, 100);	
		SendMessage(upProgressBar, PBM_SETSTEP, 1, 0);	
		startUpdate(hDlg);
		SetTimer(hDlg, UPDATE_TIMER, 50); //创建定时器，每隔0.5s刷新升级进度条
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
       	HDC hdc;
       	hdc = BeginPaint (hDlg);		
       	EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg, "系统升级");
		return 0;
	}
#endif
	case MSG_ERASEBKGND:
	{
		RECT title = {0, 0, 500, 50};
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
		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bk_pic[SIZE_500x150]); 
		Gui_WriteTitleName(hdc, &title, "系统升级");
		if (fGetDC) 
			ReleaseDC (hdc); 
		return 0; 
	}
	break;

	case MSG_TIMER:
	{
		switch(wParam)
		{
			case UPDATE_TIMER:
			{
				int pos = uPercent;
				
				if(pos<0 || pos>100)
					pos = 0;
				SendMessage(upProgressBar,PBM_SETPOS, pos, 0);
				
/*				sprintf(port, "%d%%", pos);
				percentWnd = GetDlgItem(hDlg, IDC_STATIC_PERCENT);
				SetWindowText(percentWnd, port);
*/				if(uPercent >= 100)
				{
					UpdateStop(hDlg);
#ifdef OWN_MESSAGEBOX_WIN
					CreateMessageBoxWin(hDlg, LTR("升级完成，系统将重启!"), LTR("消息"), MB_OK);
#else
					MessageBox(hDlg, LTR("升级完成，系统将重启!"), LTR("消息"), MB_OK);
#endif
					//printf("[info]<<<<GetParent(hDlg) :%d\n",GetParent(hDlg));
					//PostMessage(GetParent(hDlg), MSG_CLOSE, 0, 0);
					//PostMessage(hDlg, MSG_CLOSE, 0, 0);
					BroadcastMessage(MSG_CLOSE, 0, 0);
					//PostMessage (HWND_DESKTOP, MSG_ENDSESSION, 0, 0);
					nvr_restart();//升级完后，重启
				}
			}
			return 0;
		}
	}
	break;
	case MSG_RBUTTONUP:
    {
		//PostMessage(hDlg, MSG_CLOSE, 0, 0);
        break;
    }

    case MSG_COMMAND:
    {
		break;
    }
    case MSG_CLOSE:
    {		
		upProgressBar = HWND_INVALID;
		if (IsTimerInstalled(hDlg ,UPDATE_TIMER))
			KillTimer(hDlg, UPDATE_TIMER);

    	HWND hWind;
    	pop_open_dlg(&headOpenDlg, &hWind);
        EndDialog (hDlg, 0);
        break;
    }

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}


void CreateUpdateDlgWin(HWND hParent, char *filepath, char *filename)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "UpdateDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_update_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_update_control, sizeof(ui_update_control));

	DLG_CAPTION_LANGUAGE_TRANS(ui_update_control)
    UpdateDlg.controls = ui_update_control;
    strcpy(filePath, filepath);
    strcpy(fileName, filename);
	//printf("[info]<<<file name:%s\n",filePath);
    DialogBoxIndirectParam (&UpdateDlg, hParent, UpdateWinProc, 0L);
	memcpy(ui_update_control, ctrolTmp, sizeof(ui_update_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}
