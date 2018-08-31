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
#include "log.h"
#include "nvr_lib_interface.h"
#include "ui_dlg_head.h"
#include "gui_core.h"
#include "ctrl_date.h"
#include "storage_ext_lib.h"



#define PRE_NVR_LOG  "NVR_"

#define LOG_DLG_W	700
#define LOG_DLG_H	500
#define LOG_DLG_X	((NVR_SCREEN_WIDTH - LOG_DLG_W)>>1)
#define LOG_DLG_Y	((NVR_SCREEN_HEIGHT - LOG_DLG_H)>>1)

#define LOG_START_X	 50
#define LOG_START_Y	 60
#define LOG_LINE_INC_Y	(GUI_COMBOX_H + 12)

#define _LOG_LIST_W		(LOG_DLG_W - 2*LOG_START_X)
#define _LOG_LIST_H		270

typedef enum
{
	IDL_LOG_TYPE = 2000, 
	IDC_LOG_INFO_START_TIME,
	IDC_LOG_INFO_END_TIME,
	IDC_LOG_QUERY,
	IDC_LOG_LISTVIEW,
	IDC_LOG_BACKUP,
	IDC_LOG_DELETE,
}UI_LOG_INFO_ID;


static RECT  log_title_rect = {0,0,LOG_DLG_W,50};
//static time_t g_dayLogStartsec;

typedef struct _LogInfo
{
	ctrl_time_t log_time;
	LOG_TYPE    log_type;
	char		*log_message;
}LogInfo_T;

static CTRLDATA ui_log_control[] =
{
	 {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        LOG_START_X,  LOG_START_Y, 80, 25,
        IDC_STATIC,
        "类型",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
       	LOG_START_X+80,LOG_START_Y, 120+10, GUI_COMBOX_H,
        IDL_LOG_TYPE,
        "0",
        80,
        WS_EX_USEPARENTFONT
     },
     {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        LOG_START_X,  LOG_START_Y+LOG_LINE_INC_Y, 70, 25,
        IDC_STATIC,
        "起始时间",
        0,
		WS_EX_TRANSPARENT
     },
     {
        CTRL_DATE,
        WS_TABSTOP | WS_VISIBLE | DE_DATE_TIME|DATE_NOTIFY|WS_TABSTOP,
		LOG_START_X+80,LOG_START_Y+LOG_LINE_INC_Y, 180, 21,
        IDC_LOG_INFO_START_TIME,
        "",
        6,
		WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
    },
	///////////////////////////////////////////////////////////////////
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        LOG_START_X,  LOG_START_Y+2*LOG_LINE_INC_Y, 70, 25,
        IDC_STATIC,
        "结束时间",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_DATE,
        WS_TABSTOP | WS_VISIBLE | DE_DATE_TIME|DATE_NOTIFY|WS_TABSTOP,
		LOG_START_X+80,LOG_START_Y+2*LOG_LINE_INC_Y, 180, 21,
        IDC_LOG_INFO_END_TIME,
        "",
        6,
		WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
    },
 	CONFIG_THREE_WORDS_BUTTON("查询", IDC_LOG_QUERY, LOG_DLG_W - 50 - 65,LOG_START_Y+2*LOG_LINE_INC_Y),
    {
		GUI_LISTVIEW,
        WS_BORDER | WS_CHILD | WS_VISIBLE| WS_VSCROLL | WS_HSCROLL,
        LOG_START_X, LOG_START_Y+3*LOG_LINE_INC_Y + 10, _LOG_LIST_W, _LOG_LIST_H,
        IDC_LOG_LISTVIEW,
        "已添加设备",
        0
    },
	CONFIG_THREE_WORDS_BUTTON("备份", IDC_LOG_BACKUP, LOG_DLG_W - 50 - 65 - 75, LOG_DLG_H - 50),
	CONFIG_THREE_WORDS_BUTTON("清空", IDC_LOG_DELETE, LOG_DLG_W - 50 - 65, LOG_DLG_H - 50),

};


static DLGTEMPLATE LogDlg =
{
    WS_NONE,
    WS_EX_NONE,
    LOG_DLG_X, LOG_DLG_Y, LOG_DLG_W, LOG_DLG_H,
    "日志信息",
    0, 0,
    TABLESIZE(ui_log_control), NULL,
    0
};

static listCaption captionLogInfo[] = 
{
	{3, "序号"},		
	{9, "记录时间"},
	{12,"事件"},
	{4, "播放"}
};

const char *LogType[] = {"所有", "系统操作", "配置操作", "报警事件", "录像操作", "用户管理"};



#ifdef CUSTOM_DRAW
static void my_draw_hdr_bk (HWND hWnd, HLVHDR hlvhdr, HDC hdc, RECT *rcDraw)
{
    SetBrushColor (hdc, LISTVIEW_HEAD_BKC);
    FillBox (hdc, rcDraw->left, rcDraw->top, RECTWP(rcDraw), RECTHP(rcDraw));
}

static void my_draw_hdr_item (HWND hWnd, int idx, HDC hdc, RECT *rcDraw)
{
	SetPenColor(hdc,0xff575859);
    Rectangle (hdc, rcDraw->left, rcDraw->bottom, rcDraw->right, rcDraw->bottom);	
    Rectangle (hdc, rcDraw->right, rcDraw->top, rcDraw->right, rcDraw->bottom);
    SetBkMode (hdc, BM_TRANSPARENT);
	SetTextColor(hdc, PIXEL_lightwhite);
    TextOut (hdc, rcDraw->left+4, rcDraw->top+4, LTR(captionLogInfo[idx].Text));
}
#endif


static int InitLogInfoListHeader(HWND hDlg)
{
	int i;
    LVCOLUMN lvcol;
	int sum_w = 0;
	int col_num = TABLESIZE(captionLogInfo);
	
	for (i = 0; i < col_num; i++)
		sum_w += captionLogInfo[i].col_w;
	
    for (i = 0; i < col_num; i++) 
	{
        lvcol.nCols = i;
        lvcol.pszHeadText = LTR(captionLogInfo[i].Text);
        lvcol.width = captionLogInfo[i].col_w*_LOG_LIST_W/sum_w;
		if(i == 0)
			lvcol.pfnCompare = NULL;//compare_string; //序号项，按照序号的大小排序
		else
        	lvcol.pfnCompare = NULL;//其它选项按照默认的按字符串排序
        lvcol.colFlags = 0;
		SendDlgItemMessage(hDlg, IDC_LOG_LISTVIEW, LVM_ADDCOLUMN, 0, (LPARAM) &lvcol);
    }

#ifdef CUSTOM_DRAW
	HWND hListView;
	hListView = GetDlgItem(hDlg,IDC_LOG_LISTVIEW);
    LVCUSTOMDRAWFUNCS myfuncs = {my_draw_hdr_bk, my_draw_hdr_item};
    SendMessage (hListView, LVM_SETCUSTOMDRAW, 0, (LPARAM) &myfuncs);
#endif

	int nLine;
	SendDlgItemMessage(hDlg,IDL_LOG_TYPE, CB_RESETCONTENT, 0, (LPARAM)0);
	nLine = TABLESIZE(LogType);
	//printf("---------nLine :%d\n",nLine);
	for(i=0; i<nLine; i++)
		SendDlgItemMessage(hDlg, IDL_LOG_TYPE, CB_ADDSTRING, 0, (LPARAM)LTR(LogType[i]));
	SendDlgItemMessage(hDlg, IDL_LOG_TYPE, CB_SETCURSEL, 0, 0);	
	return 0;
}


static void LogDlgInit(HWND hDlg)
{	
	ctrl_time_t stDate;
	struct tm stTime, *pTmTime;
	time_t t;	
	t = time(NULL);
	pTmTime = localtime_r(&t, &stTime);

	stDate.year= pTmTime->tm_year+1900;
	stDate.mon = pTmTime->tm_mon+1;
	stDate.day = pTmTime->tm_mday;
	stDate.hour = 0;
	stDate.min = 0;
	stDate.sec = 0;
	SendDlgItemMessage(hDlg, IDC_LOG_INFO_START_TIME, MSG_SET_DATE, 0, (LPARAM)&stDate);
	stDate.hour = 23;
	stDate.min = 59;
	stDate.sec = 59;
	SendDlgItemMessage(hDlg, IDC_LOG_INFO_END_TIME, MSG_SET_DATE, 0, (LPARAM)&stDate);

	InitLogInfoListHeader(hDlg);
}



static GHANDLE AddLogItemList(HWND hDlg, PLVITEM lvItem,  LogInfo_T *pstLog)
{
	char Text[20];
    LVSUBITEM subdata;
    GHANDLE item = SendDlgItemMessage (hDlg, IDC_LOG_LISTVIEW, LVM_ADDITEM, 0, (LPARAM)lvItem);
  
    subdata.flags = 0;
    subdata.image = 0;
    subdata.nItem = lvItem->nItem;
	subdata.nTextColor = 0;

	//序号
	sprintf(Text, "%d", lvItem->nItem);
	subdata.subItem = 0;
	subdata.pszText = (char *)Text;
    SendDlgItemMessage (hDlg, IDC_LOG_LISTVIEW, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//时间
	char time[64];
	sprintf(time, "%04d-%02d-%02d %02d:%02d:%02d", pstLog->log_time.year, 
		pstLog->log_time.mon, pstLog->log_time.day, pstLog->log_time.hour,
		pstLog->log_time.min, pstLog->log_time.sec);
	subdata.subItem = 1;
	subdata.pszText = time;
    SendDlgItemMessage (hDlg, IDC_LOG_LISTVIEW, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//事件
	subdata.subItem = 2;
	subdata.pszText =  LTR(pstLog->log_message);
    SendDlgItemMessage (hDlg, IDC_LOG_LISTVIEW, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//播放
	subdata.subItem =3;
	subdata.pszText = "---";
    SendDlgItemMessage (hDlg, IDC_LOG_LISTVIEW, LVM_SETSUBITEM, item, (LPARAM) & subdata);

    return item;
}

BOOL LogNeedShow(LogInfo_T *pstLog, ctrl_time_t *start, ctrl_time_t *end,  int logType)
{
	//所有类型
	if ((logType == 0) || ((logType!=0)&&(pstLog->log_type == logType-1)))
	{
		//年份不落在区间
		if (!((pstLog->log_time.year>=start->year)&&(pstLog->log_time.year <= end->year)))
			return FALSE;

		if (!((pstLog->log_time.mon>=start->mon)&&(pstLog->log_time.mon <= end->mon)))
			return FALSE;

		if (!((pstLog->log_time.day>=start->day)&&(pstLog->log_time.day <= end->day)))
			return FALSE;

		if (!((pstLog->log_time.hour>=start->hour)&&(pstLog->log_time.hour <= end->hour)))
			return FALSE;

		if (!((pstLog->log_time.min>=start->min)&&(pstLog->log_time.min <= end->min)))
			return FALSE;

		if (!((pstLog->log_time.sec>=start->sec)&&(pstLog->log_time.sec <= end->sec)))
			return FALSE;
	
		//时间落在区间
		return TRUE;
	}
	else
		return FALSE;
}
	

static void UILogQueryProc(HWND hDlg)
{
	char line[512];
	char LOG_STR[12];
	char message[400];
	FILE *fp = NULL;
	int i = 0;

	ctrl_time_t stStart, stEnd;
		
	//备份录像时间段
	int logType;
	SendDlgItemMessage(hDlg, IDC_LOG_INFO_START_TIME, MSG_GET_DATE, 0, (LPARAM)&stStart);
	SendDlgItemMessage(hDlg, IDC_LOG_INFO_END_TIME, MSG_GET_DATE, 0, (LPARAM)&stEnd);	
	logType = SendDlgItemMessage (hDlg, IDL_LOG_TYPE, CB_GETCURSEL, 0, 0);
	
	fp = log_read_start();
	if (fp == NULL)	return;

	LVITEM item;	
	SendDlgItemMessage (hDlg, IDC_LOG_LISTVIEW, LVM_DELALLITEM, 0, 0);
	SendDlgItemMessage (hDlg, IDC_LOG_LISTVIEW, MSG_FREEZECTRL, TRUE, 0);	
	item.nItemHeight = 20;
	while((log_read(fp, line, 512)) != NULL)
	{	
		LogInfo_T log;		
		item.nItem = i;
		log.log_message = message;
		sscanf(line, "%04d-%02d-%02d %02d:%02d:%02d %8s %d    %s", 
			&log.log_time.year, &log.log_time.mon, &log.log_time.day, 
			&log.log_time.hour, &log.log_time.min, &log.log_time.sec,
			LOG_STR, (int *)&log.log_type, log.log_message);
		if (LogNeedShow(&log, &stStart, &stEnd, (int)logType))
		{
			AddLogItemList(hDlg, &item, &log);
			i++;
		}
	#if 0	
		printf("log.log_time.year = %d\r\n", log.log_time.year);
		printf("log.log_time.mon  = %d\r\n", log.log_time.mon);
		printf("log.log_time.day  = %d\r\n", log.log_time.day);
		printf("log.log_time.hour = %d\r\n", log.log_time.hour);
		printf("log.log_time.min  = %d\r\n", log.log_time.min);
		printf("log.log_time.sec  = %d\r\n", log.log_time.sec);		
		printf("LOG_STR  = %s\r\n", LOG_STR);
		printf("log.log_type  = %d\r\n", log.log_type);		
		printf("message  = %s\r\n", log.log_message);
	#endif	
	}
	SendDlgItemMessage (hDlg, IDC_LOG_LISTVIEW, MSG_FREEZECTRL, FALSE, 0);
	SendDlgItemMessage (hDlg, IDC_LOG_LISTVIEW, LVM_SELECTITEM, 0, (LPARAM)0);
		
	log_read_stop(fp);
}

static int 	checkAvailableUsbDev(void )
{
	char devpath[64] = {0};

	if(storage_ext_get_udisk_device_path(devpath) == 1
		&& storage_ext_mout_device(devpath, STORAGE_USE_MOUNT_PATH) == 0)
	{
		return 0;
	}
	return -1;
}
static FILE * createBakupLogFile(HWND hDlg)
{
	char filePath[256] = {0};	
	ctrl_time_t stStart, stEnd;
	FILE *fp = NULL;
		
	//备份录像时间段
	SendDlgItemMessage(hDlg, IDC_LOG_INFO_START_TIME, MSG_GET_DATE, 0, (LPARAM)&stStart);
	SendDlgItemMessage(hDlg, IDC_LOG_INFO_END_TIME, MSG_GET_DATE, 0, (LPARAM)&stEnd);	

	snprintf(filePath, sizeof(filePath), "%s/%s%d.%d.%d_%d.%d.%d.log", STORAGE_USE_MOUNT_PATH, PRE_NVR_LOG, \
	stStart.year,stStart.mon, stStart.day,stEnd.year,stEnd.mon,stEnd.day);
	
	//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%s[%d]: %s\r\n", __FILE__,__LINE__,filePath);
	fp = fopen(filePath, "w");
	if(NULL == fp)
	{
		printf("open file %s error\r\n", filePath);
		return NULL;
	}
	
	return fp;
}

static int copyValidLogData(HWND hDlg, FILE *fpLog)
{
	ctrl_time_t stStart, stEnd;
	FILE *fp = NULL;	
	char oneLine[512] = {0};
	char message[400] = {0};
	char LOG_STR[12] = {0};

	if(NULL == fpLog)
		return -1;
		
	//备份录像时间段
	int logType;
	SendDlgItemMessage(hDlg, IDC_LOG_INFO_START_TIME, MSG_GET_DATE, 0, (LPARAM)&stStart);
	SendDlgItemMessage(hDlg, IDC_LOG_INFO_END_TIME, MSG_GET_DATE, 0, (LPARAM)&stEnd);	
	logType = SendDlgItemMessage (hDlg, IDL_LOG_TYPE, CB_GETCURSEL, 0, 0);
	
	fp = log_read_start();
	if (fp == NULL)	return -1;

	while((log_read(fp, oneLine, 512)) != NULL)
	{
		LogInfo_T log;		
		log.log_message = message;
		sscanf(oneLine, "%04d-%02d-%02d %02d:%02d:%02d %8s %d    %s", 
			&log.log_time.year, &log.log_time.mon, &log.log_time.day, 
			&log.log_time.hour, &log.log_time.min, &log.log_time.sec,
			LOG_STR, (int *)&log.log_type, log.log_message);
		
		if (LogNeedShow(&log, &stStart, &stEnd, (int)logType))
		{
			sprintf(oneLine, "%04d-%02d-%02d %02d:%02d:%02d %8s %d    %s\n", 
			log.log_time.year, log.log_time.mon, log.log_time.day, 
			log.log_time.hour, log.log_time.min, log.log_time.sec,
			LOG_STR, log.log_type, LTR(log.log_message)); //针对不同的字体编码需要转换

			fputs(oneLine, fpLog);
		}
	}
	
	log_read_stop(fp);
	fclose(fpLog);
	
	return 0;
}

static int LogWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
	{	
		LogDlgInit(hDlg);
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);
		//Gui_WriteTitleName(hdc, &LogTileRect, "日志信息");
        EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg,   "日志信息");
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
		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bk_pic[SIZE_700x500]); 
		Gui_WriteTitleName(hdc, &log_title_rect, LTR("日志信息"));
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
			case IDC_LOG_QUERY:
			UILogQueryProc(hDlg);
			break;
			case IDC_LOG_BACKUP:
			{
				FILE *fpLog = NULL;
				
				//1.检测是否有USB设备
				if(checkAvailableUsbDev() == 0)
				{
					if((fpLog = createBakupLogFile(hDlg)) != NULL)
					{
						if(copyValidLogData(hDlg, fpLog) == 0)
						{													
							storage_ext_umount_device(STORAGE_USE_MOUNT_PATH);
							CreateMessageBoxWin(hDlg, LTR("备份日志已完成!"), LTR("消息"), MB_OK);
							return 0;
						}
					}
					storage_ext_umount_device(STORAGE_USE_MOUNT_PATH);
					CreateMessageBoxWin(hDlg, LTR("备份日志失败!"), LTR("消息"), MB_OK);
				}
				else
				{
					CreateMessageBoxWin(hDlg, LTR("请确认已接入USB设备!"), LTR("消息"), MB_OK);
				}

				return 0;
			}
			break;
			case IDC_LOG_DELETE:
			{
				int ret;
				
#ifdef OWN_MESSAGEBOX_WIN
				ret = CreateMessageBoxWin(hDlg, LTR("确认清空日志信息!"), LTR("消息"), MB_OKCANCEL);
#else
				ret = MessageBox(hDlg, LTR("确认清空日志信息!"), LTR("消息"), MB_OKCANCEL);
#endif
				if(ret == IDOK)
				{
					log_delete();				
					SendDlgItemMessage (hDlg, IDC_LOG_LISTVIEW, LVM_DELALLITEM, 0, 0);
				}
				break;
			}	
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


//日志信息
void CreateLogDlgWin(HWND hParent)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "LogDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_log_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_log_control, sizeof(ui_log_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_log_control)

    LogDlg.controls = ui_log_control;
	
    DialogBoxIndirectParam (&LogDlg, hParent, LogWinProc, 0L);
	memcpy(ui_log_control, ctrolTmp, sizeof(ui_log_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

