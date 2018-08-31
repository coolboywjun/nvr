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
#include "ctrl_progressbar.h"
#include "myedit.h"
#include "ctrl_ipaddress.h"
#include "nvr_lib_interface.h"
#include "ui_storage_interface.h"
#include "ui_dlg_head.h"
#include "ctrl_date.h"
#include "gui_core.h"
#include "storage_ext_lib.h"

#define STORAGE_BACKUP_DLG_W	600
#define STORAGE_BACKUP_DLG_H	400
#define STORAGE_BACKUP_DLG_X	((NVR_SCREEN_WIDTH - STORAGE_BACKUP_DLG_W)>>1)
#define STORAGE_BACKUP_DLG_Y	((NVR_SCREEN_HEIGHT - STORAGE_BACKUP_DLG_H)>>1)

#define STORAGE_TEXT_START_X		50
#define STORAGE_TEXT_START_Y		60
#define STORAGE_TEXT_W				(100+10)
#define STORAGE_TEXT_H				20
#define STORAGE_TEXT_ENC_Y			(STORAGE_TEXT_H+12)

#define STOARGE_EDIT_START_X		(STORAGE_TEXT_START_X + STORAGE_TEXT_W)
#define STORAGE_EDIT_W				300

#define STORAGE_BACKUP_CH_CHECK_W	(80+10)
#define STORAGE_BACKUP_CH_CHECK_H	20
#define STORAGE_BACKUP_CHECK_INC_X	(STORAGE_BACKUP_CH_CHECK_W+10)

static RECT storage_backup_title_rect = {0,0,STORAGE_BACKUP_DLG_W,50};

typedef enum
{
	IDL_STORAGE_BACKUP_CH = 3000,
	IDC_STORAGE_BACKUP_START_TIME,
	IDC_STORAGE_BACKUP_END_TIME,
	IDL_STORAGE_DEV_PATH,
	IDC_STORAGE_BACKUP_PROGRESS,
	IDC_STORAGE_BACKUP_START_CANCEL,
	IDC_STORAGE_BACKUP_FILELEN,
	IDC_STORAGE_BACKUP_CHECK_CH1,
	IDC_STORAGE_BACKUP_CHECK_CH2,
	IDC_STORAGE_BACKUP_CHECK_CH3,
	IDC_STORAGE_BACKUP_CHECK_CH4,
	STORAGE_BACKUP_TIMER,
	CHECK_USB_TIMER,
}UI_BACKUP_ID;

typedef struct tag_StorageBackupMana
{
	int backupStart;
	int findDev;      //是否发现存储设备
	int percent;
}StorageBackupMana_t;

static time_t g_dayStartsec;
static StorageBackupMana_t g_stStorageBackuMan = {0};

static HWND backProgressBar = HWND_INVALID;

static CTRLDATA ui_storage_backup_control[] =
{
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        STORAGE_TEXT_START_X, STORAGE_TEXT_START_Y, 
        STORAGE_TEXT_W, STORAGE_TEXT_H,
        IDC_STATIC,
        "通道选择",
        0,
		WS_EX_TRANSPARENT
    },
    {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKED,
		STOARGE_EDIT_START_X, STORAGE_TEXT_START_Y, 
        STORAGE_BACKUP_CH_CHECK_W, STORAGE_BACKUP_CH_CHECK_H,
        IDC_STORAGE_BACKUP_CHECK_CH1,
        "通道一",
        0,
        WS_EX_TRANSPARENT
    },
    {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKED,
		STOARGE_EDIT_START_X+STORAGE_BACKUP_CHECK_INC_X,  STORAGE_TEXT_START_Y, 
		STORAGE_BACKUP_CH_CHECK_W, STORAGE_BACKUP_CH_CHECK_H,
        IDC_STORAGE_BACKUP_CHECK_CH2,
        "通道二",
        0,
        WS_EX_TRANSPARENT
    },
     {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKED,
		STOARGE_EDIT_START_X+2*STORAGE_BACKUP_CHECK_INC_X,  STORAGE_TEXT_START_Y, 
		STORAGE_BACKUP_CH_CHECK_W, STORAGE_BACKUP_CH_CHECK_H,
        IDC_STORAGE_BACKUP_CHECK_CH3,
        "通道三",
        0,
        WS_EX_TRANSPARENT
    },
    {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKED,
		STOARGE_EDIT_START_X+3*STORAGE_BACKUP_CHECK_INC_X,  STORAGE_TEXT_START_Y, 
		STORAGE_BACKUP_CH_CHECK_W, STORAGE_BACKUP_CH_CHECK_H,
        IDC_STORAGE_BACKUP_CHECK_CH4,
        "通道四",
        0,
        WS_EX_TRANSPARENT
    },
	/////////////////////////////////////////////////
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        STORAGE_TEXT_START_X, STORAGE_TEXT_START_Y+ STORAGE_TEXT_ENC_Y, 
        STORAGE_TEXT_W, STORAGE_TEXT_H,
        IDC_STATIC,
        "起始时间",
        0,
		WS_EX_TRANSPARENT
    },
     {
        CTRL_DATE,
        WS_TABSTOP | WS_VISIBLE | DE_DATE_TIME|DATE_NOTIFY|WS_TABSTOP,
		STOARGE_EDIT_START_X, STORAGE_TEXT_START_Y+ STORAGE_TEXT_ENC_Y, 
		180, STORAGE_TEXT_H,
        IDC_STORAGE_BACKUP_START_TIME,
        "",
        6,
		WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
    },
	///////////////////////////////////////////////////////////////////
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        STORAGE_TEXT_START_X, STORAGE_TEXT_START_Y + 2*STORAGE_TEXT_ENC_Y, 
        STORAGE_TEXT_W, STORAGE_TEXT_H,
        IDC_STATIC,
        "结束时间",
        0,
		WS_EX_TRANSPARENT
    },
     {
        CTRL_DATE,
        WS_TABSTOP | WS_VISIBLE | DE_DATE_TIME|DATE_NOTIFY|WS_TABSTOP,
		STOARGE_EDIT_START_X, STORAGE_TEXT_START_Y+ 2*STORAGE_TEXT_ENC_Y, 
		180, STORAGE_TEXT_H,
        IDC_STORAGE_BACKUP_END_TIME,
        "",
        6,
		WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
    },
    //////////////////////////////////////////////////////////////////
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        STORAGE_TEXT_START_X, STORAGE_TEXT_START_Y + 3*STORAGE_TEXT_ENC_Y, 
        STORAGE_TEXT_W, STORAGE_TEXT_H,
        IDC_STATIC,
        "备份到",
        0,
		WS_EX_TRANSPARENT
    },
    { 
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP,
		STOARGE_EDIT_START_X, STORAGE_TEXT_START_Y + 3*STORAGE_TEXT_ENC_Y,
		STORAGE_EDIT_W, GUI_COMBOX_H,
        IDL_STORAGE_DEV_PATH,
        "",
        0,
        WS_EX_USEPARENTFONT
    },
    ///////////////////////////////////////////////////////////////////
     {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        STORAGE_TEXT_START_X, STORAGE_TEXT_START_Y + 4*STORAGE_TEXT_ENC_Y, 
        STORAGE_TEXT_W, STORAGE_TEXT_H,
        IDC_STATIC,
        "文件容量",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_MYEDIT,
		WS_CHILD | ES_LEFT| WS_VISIBLE | WS_DISABLED,
		STOARGE_EDIT_START_X, STORAGE_TEXT_START_Y + 4*STORAGE_TEXT_ENC_Y, 
		STORAGE_EDIT_W, STORAGE_TEXT_H,
        IDC_STORAGE_BACKUP_FILELEN,
        "0.0M",
        0,        
		WS_EX_USEPARENTFONT
    },
    
    //////////////////////////////////////////////////////////////////////
     {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        STORAGE_TEXT_START_X, STORAGE_TEXT_START_Y + 5*STORAGE_TEXT_ENC_Y, 
        STORAGE_TEXT_W, STORAGE_TEXT_H,
        IDC_STATIC,
        "备份进度",
        0,
		WS_EX_TRANSPARENT
    },
    {
		CTRL_PROGRESSBAR_EX,
		WS_VISIBLE| PBS_NOTIFY,		
        STOARGE_EDIT_START_X, STORAGE_TEXT_START_Y + 5*STORAGE_TEXT_ENC_Y, 
        STORAGE_EDIT_W, STORAGE_TEXT_H,
        IDC_STORAGE_BACKUP_PROGRESS,
        "0",
 		WS_EX_USEPARENTCURSOR
	},
	///////////////////////////////////////////////////////////////////
	CONFIG_TWO_WORDS_BUTTON("备份", IDC_STORAGE_BACKUP_START_CANCEL, STORAGE_BACKUP_DLG_W - 120 + 10, STORAGE_BACKUP_DLG_H - 50),
	
};


static DLGTEMPLATE StorageBkDlg =
{
    WS_NONE,
    WS_EX_NONE,
    STORAGE_BACKUP_DLG_X, STORAGE_BACKUP_DLG_Y, STORAGE_BACKUP_DLG_W, STORAGE_BACKUP_DLG_H,
    "复制",
    0, 0,
    TABLESIZE(ui_storage_backup_control), NULL,
    0
};

static HWND gStorageBackupWin= HWND_INVALID;

void setStorageBackupWin(HWND hWnd)
{
	gStorageBackupWin = hWnd;
}

HWND getStorageBackupWin(void)
{
	return gStorageBackupWin;
}

static void StorageBackupFileSize(HWND hDlg);

static void CheckUsbState(HWND hDlg)
{
	char diskpath[128];
	int ret;
	
	memset(diskpath, 0, sizeof(diskpath));
	ret = storage_ext_get_udisk_device_path(diskpath);
	if (ret != 1)
	{
		EnableDlgItem(hDlg,IDL_STORAGE_DEV_PATH,FALSE);
		sprintf(diskpath, LTR("未发现usb存储设备"));
		g_stStorageBackuMan.findDev = 0;
	}
	else
	{
		EnableDlgItem(hDlg, IDL_STORAGE_DEV_PATH, TRUE);
		sprintf(diskpath, LTR("usb设备"));
		g_stStorageBackuMan.findDev = 1; //找到usb设备
	}
	SendDlgItemMessage(hDlg,IDL_STORAGE_DEV_PATH, CB_RESETCONTENT, 0, (LPARAM)0);
	SendDlgItemMessage(hDlg, IDL_STORAGE_DEV_PATH, CB_ADDSTRING, 0, (LPARAM)diskpath);
	SendDlgItemMessage(hDlg, IDL_STORAGE_DEV_PATH, CB_SETCURSEL, 0, 0);
}

static void InitStorageBackup(HWND hDlg)
{
	ctrl_time_t stDate;
	
	//time_t 	tTime  = 0;
	struct tm *pTmTime;
	//time(&g_dayStartsec);
	pTmTime = localtime(&g_dayStartsec);
	stDate.year= pTmTime->tm_year+1900;
	stDate.mon = pTmTime->tm_mon+1;
	stDate.day = pTmTime->tm_mday;
	stDate.hour = 0;
	stDate.min = 0;
	stDate.sec = 0;
	SendDlgItemMessage(hDlg, IDC_STORAGE_BACKUP_START_TIME, MSG_SET_DATE, 0, (LPARAM)&stDate);
	stDate.hour = 23;
	stDate.min = 59;
	stDate.sec = 59;
	SendDlgItemMessage(hDlg, IDC_STORAGE_BACKUP_END_TIME, MSG_SET_DATE, 0, (LPARAM)&stDate);
/*
	char diskpath[128];
	int ret;
	memset(diskpath, 0, sizeof(diskpath));
	ret = storage_ext_get_udisk_device_path(diskpath);
	if (ret != 1)
	{
		EnableDlgItem(hDlg,IDL_STORAGE_DEV_PATH,FALSE);
		sprintf(diskpath, "未发现usb存储设备");
		g_stStorageBackuMan.findDev = 0;
	}
	else
	{
		EnableDlgItem(hDlg, IDL_STORAGE_DEV_PATH, TRUE);
		sprintf(diskpath, "usb设备");
		g_stStorageBackuMan.findDev = 1; //找到usb设备
	}
	SendDlgItemMessage(hDlg,IDL_STORAGE_DEV_PATH, CB_RESETCONTENT, 0, (LPARAM)0);
	SendDlgItemMessage(hDlg, IDL_STORAGE_DEV_PATH, CB_ADDSTRING, 0, (LPARAM)diskpath);
	SendDlgItemMessage(hDlg, IDL_STORAGE_DEV_PATH, CB_SETCURSEL, 0, 0);
*/
	CheckUsbState(hDlg);
	
	StorageBackupFileSize(hDlg);//获取初始文件容量大小
}

static int StorageBackupGetTimeChn(HWND hDlg, int *chn_mask, time_t *begin_time, time_t *end_time)
{
	ctrl_time_t stDate;
	int chn = 0;		

	if((chn_mask == NULL) || (begin_time == NULL) || (end_time == NULL))
		return (-1);
		
	//备份录像时间段
	SendDlgItemMessage(hDlg, IDC_STORAGE_BACKUP_START_TIME, MSG_GET_DATE, 0, (LPARAM)&stDate);
	*begin_time = ui_param_day_to_time_t(stDate.year, stDate.mon, stDate.day, stDate.hour, stDate.min, stDate.sec);
	SendDlgItemMessage(hDlg, IDC_STORAGE_BACKUP_END_TIME, MSG_GET_DATE, 0, (LPARAM)&stDate);
	*end_time = ui_param_day_to_time_t(stDate.year, stDate.mon, stDate.day, stDate.hour, stDate.min, stDate.sec);

	//备份录像通道
	*chn_mask = 0;
	unsigned int checked;
	for (chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++)
	{
		checked = SendDlgItemMessage(hDlg, IDC_STORAGE_BACKUP_CHECK_CH1+chn, BM_GETCHECK, 0, 0);
		if (checked != BST_CHECKED) continue;
		*chn_mask |= (0x01<<chn);
	}

	return 0;
}
static void StorageBackupStartCancel(HWND hDlg)
{
	if (g_stStorageBackuMan.findDev == 0)
	{
#ifdef OWN_MESSAGEBOX_WIN
		CreateMessageBoxWin(hDlg, LTR("未发现存储设备!"), LTR("消息"), MB_OK);
#else
		MessageBox(hDlg, LTR("未发现存储设备!"), LTR("消息"), MB_OK);
#endif
		return;
	}

	if (g_stStorageBackuMan.backupStart == 0)
	{
		time_t startTime = 0, endTime = 0;
		int chn_mask = 0;
		int ret = 0;
		
		ret = StorageBackupGetTimeChn(hDlg, &chn_mask, &startTime, &endTime);
		if(ret < 0)
		{
			printf("[ERR] get time and channel failed!\n");
			return ;
		}
		//备份录像类型
		StorageRecordType recordType;
		recordType = RECORD_ALL_TYPE&0x0f; //录像
		ret = storage_ext_avi_file_by_detail(chn_mask, startTime, endTime, recordType);
		if (0 > ret)
		{		
			if(ret == STORAGE_ERR_NO_DATA)
#ifdef OWN_MESSAGEBOX_WIN
				CreateMessageBoxWin(hDlg, LTR("设定的时间段内没有录像数据!"), LTR("消息"), MB_OK);
			else
				CreateMessageBoxWin(hDlg, LTR("备份失败!"), LTR("消息"), MB_OK);
#else
				MessageBox(hDlg, LTR("设定的时间段内没有录像数据!"), LTR("消息"), MB_OK);
			else
				MessageBox(hDlg, LTR("备份失败!"), LTR("消息"), MB_OK);
#endif
			return;
		}

		g_stStorageBackuMan.backupStart = 1;
		SetDlgItemText(hDlg, IDC_STORAGE_BACKUP_START_CANCEL , LTR("停止"));
		//启动刷新备份进度定时器
		if (!IsTimerInstalled(hDlg, STORAGE_BACKUP_TIMER))
			SetTimer(hDlg ,STORAGE_BACKUP_TIMER, 50);
		
	}
	else
	{
		//退出备份任务
		storage_ext_avi_finish();
		g_stStorageBackuMan.backupStart = 0;
		//关闭定时器
		if (IsTimerInstalled(hDlg, STORAGE_BACKUP_TIMER))
			KillTimer(hDlg ,STORAGE_BACKUP_TIMER);
		SetDlgItemText(hDlg, IDC_STORAGE_BACKUP_START_CANCEL , LTR("备份"));
	}
}


static void StorageBackupTimerProc(HWND hDlg)
{
	int pos = 0;

	g_stStorageBackuMan.percent = storage_ext_get_percent();
	pos = g_stStorageBackuMan.percent;
	
	//printf("g_stStorageBackuMan.percent = %d\n",ret);
	if(pos < 0)
	{
		if(pos == STORAGE_ERR_UDISK_SPACE)
		{
#ifdef OWN_MESSAGEBOX_WIN
			CreateMessageBoxWin(hDlg, LTR("磁盘空间不足!"), LTR("消息"), MB_OK);
#else
			MessageBox(hDlg, LTR("磁盘空间不足!"), LTR("消息"), MB_OK);
#endif
			SetDlgItemText(hDlg, IDC_STORAGE_BACKUP_START_CANCEL , LTR("备份"));
			if(IsTimerInstalled(hDlg, STORAGE_BACKUP_TIMER))
				KillTimer(hDlg, STORAGE_BACKUP_TIMER);
		}
		g_stStorageBackuMan.backupStart = 0;
	}
	else
	{
		if(pos>100)
			pos = 100;
			
		SendMessage(backProgressBar,PBM_SETPOS, pos, 0);
		if(g_stStorageBackuMan.percent >= 100)
		{
			//storage_ext_avi_finish();
			if(IsTimerInstalled(hDlg, STORAGE_BACKUP_TIMER))
				KillTimer(hDlg, STORAGE_BACKUP_TIMER);
			SetDlgItemText(hDlg, IDC_STORAGE_BACKUP_START_CANCEL , LTR("备份"));
			g_stStorageBackuMan.backupStart = 0;	
		}
	}

}

static void StorageBackupFileSize(HWND hDlg)
{
	time_t startTime = 0, endTime = 0;
	int chn_mask = 0;
	int ret = 0;
	long long frame_size = 0;
	char buffer[16]={0};
	StorageRecordType record_type = RECORD_ALL_TYPE&0x0f;
	
	ret = StorageBackupGetTimeChn(hDlg, &chn_mask, &startTime, &endTime);
	if(ret < 0)
	{
		printf("[ERR] get time and channel failed!\n");
		return ;
	}

	ret = storage_ext_get_avi_file_size(chn_mask, startTime, endTime, record_type, &frame_size);
	if(ret == 0)
	{
		sprintf(buffer, "%.2fM", (float)frame_size/(1024*1024));
		
	}
	else
	{
		sprintf(buffer, "0.0M");
	}
	SetDlgItemText(hDlg, IDC_STORAGE_BACKUP_FILELEN, buffer);
}
static int StorageBackupWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
    	//printf("[int] len = %d,[long] len = %d,[long long] len = %d,\n",sizeof(int),sizeof(long),sizeof(long long));
		backProgressBar = GetDlgItem(hDlg, IDC_STORAGE_BACKUP_PROGRESS);
		SendMessage(backProgressBar, PBM_SETRANGE, 0, 100);	
		SendMessage(backProgressBar, PBM_SETSTEP, 1, 0);	
		InitStorageBackup(hDlg);
		if (!IsTimerInstalled(hDlg, CHECK_USB_TIMER))
			SetTimer(hDlg ,CHECK_USB_TIMER, 100);
		setStorageBackupWin(hDlg);
		//push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg, "录像备份");
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
		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bk_pic[SIZE_600x400]); 
		Gui_WriteTitleName(hdc, &storage_backup_title_rect, LTR("录像备份"));
		if (fGetDC) 
			ReleaseDC (hdc); 
		return 0; 
	}
	break;

	case MSG_RBUTTONUP:
    {
		if (g_stStorageBackuMan.backupStart == 1)
			return 0;
		
		PostMessage(hDlg, MSG_CLOSE, 0, 0);
		
        break;
    }

	case MSG_TIMER:
	{
		switch(wParam)
		{
			case STORAGE_BACKUP_TIMER:
				StorageBackupTimerProc(hDlg);
				return 0;
			case CHECK_USB_TIMER:
				CheckUsbState(hDlg);
				return 0;
		}
		break;
	}

    case MSG_COMMAND:
    { 	
		int id = LOWORD(wParam);
		int code = HIWORD(wParam);
		if((id == IDC_STORAGE_BACKUP_START_TIME) || (id == IDC_STORAGE_BACKUP_END_TIME))
		{
			if(code == DATE_CHANGE) 
			{
				StorageBackupFileSize(hDlg);
			}

			return 0;
		}
		else if((id >= IDC_STORAGE_BACKUP_CHECK_CH1) && (id <= IDC_STORAGE_BACKUP_CHECK_CH4))
		{
			if(code == BN_UNPUSHED)
			{
				StorageBackupFileSize(hDlg);
			}
			return 0;
		}
		switch (wParam) 
		{
			case IDC_STORAGE_BACKUP_START_CANCEL:
				StorageBackupStartCancel(hDlg);
				return 0;
		}
		break;
    }
    case MSG_CLOSE:
    {		
		backProgressBar = HWND_INVALID;
		//g_stStorageBackuMan.percent = 0;
		memset(&g_stStorageBackuMan, 0, sizeof(StorageBackupMana_t));
		if(IsTimerInstalled(hDlg, STORAGE_BACKUP_TIMER))
			KillTimer(hDlg, STORAGE_BACKUP_TIMER);
			
		if(IsTimerInstalled(hDlg, CHECK_USB_TIMER))
			KillTimer(hDlg, CHECK_USB_TIMER);
		setStorageBackupWin(HWND_INVALID);
			
    	//HWND hWind;
    	//pop_open_dlg(&headOpenDlg, &hWind);
        EndDialog (hDlg, 0);
        break;
    }

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

//远程设备配置
void CreateStorageBackupDlgWin(HWND hParent, time_t dayStartsec)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "StorageBkDlg" , 0 , 0);
#endif
	g_dayStartsec = dayStartsec;
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_storage_backup_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_storage_backup_control, sizeof(ui_storage_backup_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_storage_backup_control)
    StorageBkDlg.controls = ui_storage_backup_control;
    
    DialogBoxIndirectParam (&StorageBkDlg, hParent, StorageBackupWinProc, 0L);
	memcpy(ui_storage_backup_control, ctrolTmp, sizeof(ui_storage_backup_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

