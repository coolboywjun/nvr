#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <dirent.h>
#include <sys/mount.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>
#include <minigui/skin.h>

#include "ui_config.h"
#include "myedit.h"
#include "my_trackbar.h"
#include "ctrl_ipaddress.h"
#include "nvr_lib_interface.h"
#include "storage_ext_lib.h"

#include "gui_core.h"
#include "ui_dlg_head.h"
#include "ctrl_date.h"


#define CONFIG_BAKUP_DLG_W	600
#define CONFIG_BAKUP_DLG_H	400
#define CONFIG_BAKUP_DLG_X	((NVR_SCREEN_WIDTH - CONFIG_BAKUP_DLG_W)>>1)
#define CONFIG_BAKUP_DLG_Y	((NVR_SCREEN_HEIGHT - CONFIG_BAKUP_DLG_H)>>1)
#define CONFIG_BAKUP_TEXT_W	120
#define CONFIG_BAKUP_EDIT_W	315
#define CONFIG_BAKUP_EDIT_H	21

#define CONFIG_BAKUP_BUTTON_W  30

#define CONFIG_BAKUP_INC_Y	(CONFIG_BAKUP_EDIT_H+15)

#define CONFIG_BAKUP_START_X	50

#define CONFIG_BAKUP_START_Y	60

typedef enum
{
	IDC_CONFIG_BAKUP = 1110,
	IDC_EXPORT_FILE_PATH,
	IDC_IMPORT_FILE_PATH,
	IDC_START_EXPORT,
	IDC_START_IMPORT,
	IDC_EXIT,
	IDC_CONFIG_BAKUP_TIMER,
	
}UI_CONFIG_BAKUP_ID;

static CTRLDATA ui_config_bakup_control[] =
{
	{
		CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
       	CONFIG_BAKUP_START_X, CONFIG_BAKUP_START_Y, CONFIG_BAKUP_TEXT_W, CONFIG_BAKUP_EDIT_H,
        IDC_STATIC,
        "导出系统配置文件",
        0,
		WS_EX_TRANSPARENT
	},	
    {
        CTRL_MYEDIT,
		WS_CHILD | ES_LEFT| WS_VISIBLE | WS_DISABLED,
        CONFIG_BAKUP_START_X+CONFIG_BAKUP_TEXT_W, CONFIG_BAKUP_START_Y, CONFIG_BAKUP_EDIT_W, CONFIG_BAKUP_EDIT_H,
        IDC_EXPORT_FILE_PATH,
        "",
        0,
        WS_EX_USEPARENTFONT
    },	
	{
		CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
       	CONFIG_BAKUP_START_X, CONFIG_BAKUP_START_Y+CONFIG_BAKUP_INC_Y, CONFIG_BAKUP_TEXT_W, CONFIG_BAKUP_EDIT_H,
        IDC_STATIC,
        "导入系统配置文件",
        0,
		WS_EX_TRANSPARENT
	},	
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP/*|CBS_NOTIFY*/,
        CONFIG_BAKUP_START_X+CONFIG_BAKUP_TEXT_W, CONFIG_BAKUP_START_Y+CONFIG_BAKUP_INC_Y, CONFIG_BAKUP_EDIT_W, CONFIG_BAKUP_EDIT_H,
        IDC_IMPORT_FILE_PATH,
        "",
        0,
        WS_EX_USEPARENTFONT
    },	
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | SS_GROUPBOX | WS_VISIBLE,
        CONFIG_BAKUP_START_X, CONFIG_BAKUP_START_Y+CONFIG_BAKUP_INC_Y*2, 500, 70,
        IDC_STATIC,
        "提示",
        0,
		WS_EX_TRANSPARENT
    },	
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        CONFIG_BAKUP_START_X+10, CONFIG_BAKUP_START_Y+CONFIG_BAKUP_INC_Y*2+30, 500, 50,
        IDC_STATIC,
        "导入或导出系统配置文件，需要先接入USB设备，才能支持相应的操作!",
        0,
		WS_EX_TRANSPARENT
    },	

    ///////////////////// GUI_BUTTON ////////////////////////
    CONFIG_TWO_WORDS_BUTTON("开始", IDC_START_EXPORT, CONFIG_BAKUP_START_X+CONFIG_BAKUP_TEXT_W+CONFIG_BAKUP_EDIT_W+10, CONFIG_BAKUP_START_Y),
    CONFIG_TWO_WORDS_BUTTON("开始", IDC_START_IMPORT, CONFIG_BAKUP_START_X+CONFIG_BAKUP_TEXT_W+CONFIG_BAKUP_EDIT_W+10, CONFIG_BAKUP_START_Y+CONFIG_BAKUP_INC_Y),
    
    CONFIG_TWO_WORDS_BUTTON("退出", IDC_EXIT, CONFIG_BAKUP_DLG_W - 95, CONFIG_BAKUP_DLG_H - 50),

};


static DLGTEMPLATE ConfigBakupDlg =
{
    WS_NONE,
    WS_EX_NONE,
    CONFIG_BAKUP_DLG_X, CONFIG_BAKUP_DLG_Y, CONFIG_BAKUP_DLG_W, CONFIG_BAKUP_DLG_H,
    "时间设置",
    0, 0,
    TABLESIZE(ui_config_bakup_control), NULL,
    0
};

#define PRE_SYS_PARAM_FILE "NVRDevCfg"

#define MAX_SYSFILE  5
#define SYSFILE_LENG 256

typedef struct _USB_STATUS{
BOOL preStatus;
BOOL curStatus;
char *sysFilePath[MAX_SYSFILE];
int  fileCount;
}usb_status, *pusb_status;


static int ui_export_sys_param(char *filePath, int len, char *mountPath)
{
	const char *preFileName = PRE_SYS_PARAM_FILE;
	int ret;
	
	if(NULL == filePath || NULL == mountPath)
	return -1;
	
	ctrl_time_t stDate;
	time_t 	tTime  = 0;
	struct tm *pTmTime;
	time(&tTime);
	pTmTime = localtime(&tTime);
	stDate.year= pTmTime->tm_year+1900;
	stDate.mon = pTmTime->tm_mon+1;
	stDate.day = pTmTime->tm_mday;
	//stDate.hour = pTmTime->tm_hour;
	//stDate.min = pTmTime->tm_min;
	//stDate.sec = pTmTime->tm_sec;
	
	snprintf(filePath, len, "%s/%s%d%02d%02d.bin", mountPath, preFileName,stDate.year,stDate.mon,stDate.day);
	//printf("!!!!!!!!!!!!!!!!!!!!!!!file path: %s \r\n", filePath);
	
	ret = nvr_export_param(filePath);
	if(ret < 0)
	{
		printf("!!!!!!!!!!!!!!!!export param failed\r\n");
		return -1;
	}

	return 0;
}

static int ui_get_sys_file(char *mountPath, char **filePath, int *count)
{
	DIR *mount_dir;
	struct dirent *result,entry;
	int i = 0;
	
	if(NULL == mountPath || NULL == filePath || NULL == count)
		return -1;

	*count = 0;
	
	mount_dir = opendir(mountPath);
	if(NULL == mount_dir)
	{
		printf("!!!!!!!!!!!!open %s error\r\n", mountPath);
		umount(mountPath);
		return -1;
	}
	
	while(readdir_r(mount_dir, &entry, &result) == 0)
	{
		if(NULL == result)
			break;
		
		if(result->d_type != DT_REG)
			continue;
		if(strncmp(result->d_name, PRE_SYS_PARAM_FILE, strlen(PRE_SYS_PARAM_FILE)) != 0)
			continue;

		if(i >= MAX_SYSFILE) //显示5条信息
			break;
			
		snprintf(filePath[i], SYSFILE_LENG, "%s/%s", mountPath, result->d_name);
		filePath[i][SYSFILE_LENG-1] = '\0';
		i++;
	
	}
	closedir(mount_dir);
	*count = i;
	
	return 0;
}


usb_status gUsbStatus;

static void initUdiskData(void)
{
	int i;
	gUsbStatus.preStatus = FALSE;
	gUsbStatus.curStatus = FALSE;
	gUsbStatus.fileCount = 0;
	for(i=0; i<MAX_SYSFILE; i++)
		gUsbStatus.sysFilePath[i] = (char *)malloc(SYSFILE_LENG);
}

static void uninitUdiskData(void)
{
	int i;
	for(i=0; i<MAX_SYSFILE; i++)
		free(gUsbStatus.sysFilePath[i]);
}

static void updateUdiskState(HWND hDlg)
{
	int ret = 0, i = 0;
	char diskpath[64] = {0};
	char devpath[64] = {0};
	
	gUsbStatus.preStatus = gUsbStatus.curStatus;
	gUsbStatus.curStatus = FALSE;
	if(storage_ext_get_udisk_device_path(devpath) == 1
		&& storage_ext_mout_device(devpath, STORAGE_USE_MOUNT_PATH) == 0)
	{
		gUsbStatus.preStatus = gUsbStatus.curStatus;
		gUsbStatus.curStatus = TRUE;
	}

	if(gUsbStatus.curStatus)
	{
		
		ret = ui_get_sys_file(STORAGE_USE_MOUNT_PATH, gUsbStatus.sysFilePath, &gUsbStatus.fileCount);
		if(ret == 0 && gUsbStatus.fileCount != 0)
		{
			SendDlgItemMessage(hDlg,IDC_IMPORT_FILE_PATH, CB_RESETCONTENT, 0, (LPARAM)0);
			//printf("@@@@@@@@@@@@@@@@@@@@fileCount: %d\r\n", gUsbStatus.fileCount);
			
			for (i = 0; i < gUsbStatus.fileCount; i++)
			{	
				SendDlgItemMessage(hDlg, IDC_IMPORT_FILE_PATH, CB_ADDSTRING, 0, (LPARAM)gUsbStatus.sysFilePath[i]);		
			}
			SendDlgItemMessage(hDlg, IDC_IMPORT_FILE_PATH, CB_SETCURSEL, 0, 0);	
			EnableDlgItem(hDlg, IDC_IMPORT_FILE_PATH, TRUE);

			return;
		}
		
	}
	else
	{
		if(gUsbStatus.preStatus)
		{
			storage_ext_umount_device(STORAGE_USE_MOUNT_PATH);
		}
	}
	sprintf(diskpath, LTR("USB设备中无系统配置文件"));

	SendDlgItemMessage(hDlg,IDC_IMPORT_FILE_PATH, CB_RESETCONTENT, 0, (LPARAM)0);
	SendDlgItemMessage(hDlg, IDC_IMPORT_FILE_PATH, CB_ADDSTRING, 0, (LPARAM)diskpath);
	SendDlgItemMessage(hDlg, IDC_IMPORT_FILE_PATH, CB_SETCURSEL, 0, 0);	
	EnableDlgItem(hDlg, IDC_IMPORT_FILE_PATH, FALSE);
	
}

static void initDlgParam(HWND hDlg)
{
	initUdiskData();
	updateUdiskState(hDlg);
}

static int ConfigBakupWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
    	initDlgParam(hDlg);
		if(!IsTimerInstalled(hDlg, IDC_CONFIG_BAKUP_TIMER))
			SetTimer(hDlg, IDC_CONFIG_BAKUP_TIMER, 200);
			
    	push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);			
		Gui_DrawCaption(hDlg,  "自动维护");
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
		Gui_WriteTitleName(hdc, &title, LTR("配置备份"));
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
			case IDC_START_EXPORT:
			{
				char filePath[256] = {0};
				if(gUsbStatus.curStatus)//usb设备已挂载成功
				{
					if(ui_export_sys_param(filePath, 256, STORAGE_USE_MOUNT_PATH) < 0)
					{
						CreateMessageBoxWin(hDlg, LTR("导出系统数据失败!"), LTR("消息"), MB_OK);
					}
					else
					{
						CreateMessageBoxWin(hDlg, LTR("数据已导出!"), LTR("消息"), MB_OK);
					}
					
					SetDlgItemText(hDlg, IDC_EXPORT_FILE_PATH, filePath);
					InvalidateRect(hDlg, NULL, TRUE);
					return 0;
				}
				CreateMessageBoxWin(hDlg, LTR("请确认已接入USB设备!"), LTR("消息"), MB_OK);
				return 0;
			}
			break;
			case IDC_START_IMPORT:
			{
				int nSel, ret;	
				
				if(gUsbStatus.curStatus)
				{
					nSel = SendDlgItemMessage (hDlg, IDC_IMPORT_FILE_PATH, CB_GETCURSEL, 0, 0);
					if (nSel < 0 || nSel >= MAX_SYSFILE)
						 return 0;
						 
				//	printf("@@@@@@@@@@@@@@@@@@nSel: %d\r\n",nSel);
					if(nvr_import_param(gUsbStatus.sysFilePath[nSel]) == 0)
					{
						printf("!!!!!!!!!!!!!!!import nvr sys param success\r\n");
						ret = CreateMessageBoxWin(hDlg, LTR("数据已经导入，系统需要重启才能正常工作!"), LTR("消息"), MB_OKCANCEL);
						if(ret == IDOK)
						{
							nvr_restart();  //重启设备
							SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
						}
					}
					else
					{
						CreateMessageBoxWin(hDlg, LTR("数据导入失败!"), LTR("消息"), MB_OK);
					}
					
					return 0;
				}
				CreateMessageBoxWin(hDlg, LTR("请确认已接入USB设备!"), LTR("消息"), MB_OK);
				return 0;
			}
			break;
			case IDC_EXIT:
				PostMessage(hDlg, MSG_CLOSE, 0, 0);
			break;
		}

		break;
	}
	case MSG_TIMER:
	{
		switch(wParam)
		{
			case IDC_CONFIG_BAKUP_TIMER:
				updateUdiskState(hDlg);
				break;
			default:
				return 0;
		}
	}
	break;
    case MSG_CLOSE:
    {		
		if(IsTimerInstalled(hDlg, IDC_CONFIG_BAKUP_TIMER))
			KillTimer(hDlg, IDC_CONFIG_BAKUP_TIMER);
			
    	if(gUsbStatus.curStatus)
    		storage_ext_umount_device(STORAGE_USE_MOUNT_PATH);
    		
    	uninitUdiskData();
    	HWND hWind;
    	pop_open_dlg(&headOpenDlg, &hWind);
        EndDialog (hDlg, 0);
        break;
    }

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

//远程设备配置
void CreateConfigBakupWin(HWND hParent)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "ConfigBakupDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_config_bakup_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_config_bakup_control, sizeof(ui_config_bakup_control));

	DLG_CAPTION_LANGUAGE_TRANS(ui_config_bakup_control);
    ConfigBakupDlg.controls = ui_config_bakup_control;
    
    DialogBoxIndirectParam (&ConfigBakupDlg, hParent, ConfigBakupWinProc, 0L);
	memcpy(ui_config_bakup_control, ctrolTmp, sizeof(ui_config_bakup_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

