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
#include "storage_interface.h"

#include "ui_dlg_head.h"

#include "gui_core.h"
#include "storage_interface.h"
#include "player_interface.h"
#include "ipnc_lib.h"
#include "nvr_lib_interface.h"
#include "storage_lib.h"


#define DISK_MANAGE_DLG_W	700
#define DISK_MANAGE_DLG_H	500
#define DISK_MANAGE_DLG_X	((NVR_SCREEN_WIDTH - DISK_MANAGE_DLG_W)>>1)
#define DISK_MANAGE_DLG_Y	((NVR_SCREEN_HEIGHT - DISK_MANAGE_DLG_H)>>1)
#define DISK_MANAGE_TEXT_W	60
#define DISK_MANAGE_EDIT_W	80
#define DISK_MANAGE_EDIT_H	20


#define DISK_LIST_X		40
#define DISK_LIST_Y		60
#define DISK_LIST_W		(DISK_MANAGE_DLG_W - 2*DISK_LIST_X)
#define DISK_LIST_H		150

typedef enum
{
	IDC_DISK_INFO_LISTVIEW = 1800,
	IDC_DISK_MANAGE_FORMAT,
	IDC_DISK_MANAGE_REFRESH,
	IDC_DISK_MANAGE_SAVE,
	IDC_DISK_MANAGE_USE,
	IDC_DISK_MANAGE_CANCEL,
	IDL_RECORD_CH,
	IDL_RECORD_STREAM_TYPE,
	IDC_RECORD_OVER_WRITTING,
	IDC_STORAGE_COPY,
	IDC_STORAGE_TIME_SET,
	IDC_DISK_FORMAT_PROGRESS,
	IDC_PRE_RECORD_TIME,
	IDC_REC_MANAGE_REFRESH,
	IDL_RECORD_TYPE,
	ID_FORMAT_TIMER,
	IDC_DISK_MAN_PREV,
	IDC_DISK_MAN_WIZARD_OK,
}UI_DISK_MANAGE_ID;


static RECT diskmanage_title_rect = {0,0,DISK_MANAGE_DLG_W,50};

enum
{
	FORMAT_FREE = 0,
	FORMAT_BUSY = 1,
	FORMAT_ERROR = 2,
	FORMAT_SUCCESS = 3
};

typedef struct tag_FormatMan_t
{
	int diskno;
	int status;    //0 free, 1busy, 2 err, 3success
	int persent;   //0-100
}FormatMan_t;

static FormatMan_t g_stFormatStatus = {0, 0, -1};
static HWND hListView = HWND_INVALID; 
static HWND hProgressBar = HWND_INVALID;

static const char *strRecorType[]=
{
	"开机录像","定时录像","入侵录像","定时&入侵录像","关闭"
};

static CTRLDATA ui_disk_manage_control[] =
{
	{
		GUI_LISTVIEW,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        DISK_LIST_X, DISK_LIST_Y, DISK_LIST_W, DISK_LIST_H,
        IDC_DISK_INFO_LISTVIEW,
        "磁盘信息",
        0
    },
    
	CONFIG_THREE_WORDS_BUTTON("格式化", IDC_DISK_MANAGE_FORMAT, DISK_LIST_X + 5, DISK_LIST_Y+DISK_LIST_H + 5),
	CONFIG_TWO_WORDS_BUTTON("刷新", IDC_DISK_MANAGE_REFRESH, DISK_LIST_X +5+90, DISK_LIST_Y+DISK_LIST_H + 5),

    {
		CTRL_PROGRESSBAR_EX,
		WS_VISIBLE,		
        DISK_LIST_X +5+90+75, DISK_LIST_Y+DISK_LIST_H + 5, DISK_LIST_W - 180, 20,
        IDC_DISK_FORMAT_PROGRESS,
        "0",
        0,
        WS_EX_USEPARENTCURSOR
	},
    /////////////////////////////////////////////////////////////////////
    {
        "static",
        WS_CHILD | SS_LEFT | WS_VISIBLE | SS_GROUPBOX,
        DISK_LIST_X,  DISK_LIST_Y+DISK_LIST_H + 55, DISK_LIST_W, 160,
        IDC_STATIC,
        "录像设置",
        0,
		WS_EX_TRANSPARENT
    },
    {
        "static",
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        DISK_LIST_X+50,  DISK_LIST_Y+DISK_LIST_H + 80, 70, 21,
        IDC_STATIC,
        "选择通道",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
       	DISK_LIST_X+90+30, DISK_LIST_Y+DISK_LIST_H + 80, 120, GUI_COMBOX_H,
        IDL_RECORD_CH,
        "1",
        80,
        WS_EX_USEPARENTFONT
     },
     {
        "static",
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        DISK_LIST_X+350,  DISK_LIST_Y+DISK_LIST_H + 80, 70+70, 21,
        IDC_STATIC,
        "录像策略",
        0,
		WS_EX_TRANSPARENT
     },
     {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
       	DISK_LIST_X+460, DISK_LIST_Y+DISK_LIST_H + 80, 120, GUI_COMBOX_H,
        IDL_RECORD_TYPE,
        "0",
        (sizeof(strRecorType)/sizeof(strRecorType[0]) >= 4) ? 80 : 0,
        WS_EX_USEPARENTFONT
     },
     {
        "static",
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        DISK_LIST_X+50,  DISK_LIST_Y+DISK_LIST_H + 115, 70, 21,
        IDC_STATIC,
        "码流类型",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP,
       	DISK_LIST_X+90+30, DISK_LIST_Y+DISK_LIST_H + 115, 120, GUI_COMBOX_H,
        IDL_RECORD_STREAM_TYPE,
        "主码流",
        0, //使用默认值
        WS_EX_USEPARENTFONT
       },
     {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKED,
		DISK_LIST_X+90+70+85, DISK_LIST_Y+DISK_LIST_H + 115, 100, 21,
        IDC_RECORD_OVER_WRITTING,
        "自动覆盖",
        0,
        WS_EX_TRANSPARENT
    },
    /////////////////////////////// GUI_BUTTON //////////////////////////////////////

    CONFIG_LONG_WORDS_BUTTON("设置时间段", IDC_STORAGE_TIME_SET, DISK_LIST_X+50, DISK_LIST_Y+DISK_LIST_H + 155),
    CONFIG_TWO_WORDS_BUTTON("复制", IDC_STORAGE_COPY, DISK_LIST_X + DISK_LIST_W - 100, DISK_LIST_Y+DISK_LIST_H + 180),

    CONFIG_TWO_WORDS_BUTTON("确定", IDC_DISK_MANAGE_SAVE, DISK_MANAGE_DLG_W-2*TWO_WORDS_BUTTON_W-DISK_LIST_X-10, DISK_MANAGE_DLG_H - 50),
    //CONFIG_TWO_WORDS_BUTTON("应用", IDC_DISK_MANAGE_USE, DISK_LIST_X + 5 + 90, DISK_MANAGE_DLG_H - 50),
    //CONFIG_TWO_WORDS_BUTTON("刷新", IDC_REC_MANAGE_REFRESH, DISK_LIST_X + 5 + 180, DISK_MANAGE_DLG_H - 50),
    CONFIG_TWO_WORDS_BUTTON("取消", IDC_DISK_MANAGE_CANCEL, DISK_MANAGE_DLG_W-TWO_WORDS_BUTTON_W-DISK_LIST_X , DISK_MANAGE_DLG_H - 50),

	CONFIG_THREE_WORDS_BUTTON("上一步", IDC_DISK_MAN_PREV, DISK_MANAGE_DLG_W - 190 , DISK_MANAGE_DLG_H - 50),
	CONFIG_THREE_WORDS_BUTTON("完成", IDC_DISK_MAN_WIZARD_OK, DISK_MANAGE_DLG_W - 115 , DISK_MANAGE_DLG_H - 50),
    
};


static DLGTEMPLATE DiskManageDlg =
{
    WS_NONE,
    WS_EX_NONE,
    DISK_MANAGE_DLG_X, DISK_MANAGE_DLG_Y, DISK_MANAGE_DLG_W, DISK_MANAGE_DLG_H,
    "录像管理",
    0, 0,
    TABLESIZE(ui_disk_manage_control), NULL,
    0
};


static listCaption caption[] = 
{
	{6, "磁盘号"},		
	{9, "总容量"},
	{9, "剩余容量"},
	{8, "备份盘"},
	{8, "状态"},
	{8, "磁盘类型"},
	{8, "文件系统"}
};

typedef struct _gui_diskmanage_cfg
{
	uint8_t		u8OverWrite;	//0 不覆盖1 覆盖
	uint8_t		u8RecStream;	//0 主码流录像1 次码流录像
	uint8_t		u8Enable;		// 是否开启录像功能
	uint8_t		u8RecType;		// 录像类型， 0:开机录像， 1:定时录像，2:报警录像,3:定时&报警
}gui_diskmanage_cfg;

gui_diskmanage_cfg recCfg;


const RECT recordTypeRect={DISK_LIST_X+430, DISK_LIST_Y+DISK_LIST_H + 80, DISK_LIST_X+430+140, DISK_LIST_Y+DISK_LIST_H + 80+GUI_COMBOX_H};

BOOL gui_get_disk_status(void)
{
	return (g_stFormatStatus.status == FORMAT_FREE);
}

static int init_disk_list_header(HWND hListView)
{
	int i;
    LVCOLUMN lvcol;
	int ncol = TABLESIZE(caption);
	int sum_w = 0;

	for (i = 0; i < ncol; i++)
		sum_w+=caption[i].col_w;
	
	for (i = 0; i < ncol; i++) 
	{
        lvcol.nCols = i;
        lvcol.pszHeadText = caption[i].Text;
        lvcol.width = caption[i].col_w*DISK_LIST_W/sum_w;
        lvcol.pfnCompare = NULL;
        lvcol.colFlags = 0;
        SendMessage (hListView, LVM_ADDCOLUMN, 0, (LPARAM) &lvcol);
    }
	
	return 0;
}

static GHANDLE add_disk_info_list(HWND hlist, PLVITEM lvItem, int diskno, StorageDiskInfo *pstDisk)
{
	int col = 0;
	char buff[20];
	//char IP[32];
	//char port[8];
    LVSUBITEM subdata;
    GHANDLE item = SendMessage (hlist, LVM_ADDITEM, 0, (LPARAM)lvItem);
    //int i = lvItem->nItem;


    subdata.flags = 0;
    subdata.image = 0;
    subdata.nItem = lvItem->nItem;
	subdata.nTextColor = 0;

	//磁盘号
	//sprintf(buff, "%d", i);
	sprintf(buff, "%d", pstDisk->disk_idx);
	subdata.subItem = col++;
	subdata.pszText = (char *)buff;
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//pstDisk->total_size;
	//总容量
	sprintf(buff, "%dM", pstDisk->total_size);
	subdata.subItem = col++;
	subdata.pszText = buff;
	SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//剩余容量	
	sprintf(buff, "%dM", pstDisk->free_size);
	subdata.subItem = col++;
	subdata.pszText = buff;//IP;
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//备份盘
	subdata.subItem = col++;
	subdata.pszText = LTR("否");//port;
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//状态
	if(pstDisk->disk_status == DISK_USEING)
		sprintf(buff, LTR("正在使用"));
	else if(pstDisk->disk_status == DISK_NO_FORMATED)
		sprintf(buff, LTR("未格式化"));
	else if (pstDisk->disk_status == DISK_FORMATTING)
		sprintf(buff, LTR("正在格式化"));
	else if (pstDisk->disk_status == DISK_FORMATTING)
		sprintf(buff, "正在格式化");
	else if (pstDisk->disk_status == DISK_MOUNTED)
		sprintf(buff, LTR("已挂载"));
	else if (pstDisk->disk_status == DISK_NO_DISK)
		sprintf(buff, LTR("无硬盘"));
	else 
		sprintf(buff, LTR("硬盘错误"));
		
	subdata.subItem = col++;
	subdata.pszText = buff;//chCfg->szDevName;
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) &subdata);

	//磁盘类型
	subdata.subItem = col++;
	subdata.pszText = "SATA";
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//文件系统	
	subdata.subItem = col++;
	subdata.pszText = "EXT 3";
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);

    return item;
}

static int init_disk_list_info(HWND hListView)
{
	int diskno = 0;
	int i = 0, ret = 0;
	LVITEM item;
	StorageDisksInfo stDisks_info;
	ret = storage_lib_get_disk_info(&stDisks_info);
	if (ret < 0)	return -1;
	
	SendMessage (hListView, LVM_DELALLITEM, 0, 0);
	SendMessage (hListView, MSG_FREEZECTRL, TRUE, 0);
	item.nItemHeight = 20;

	for (diskno = 0; diskno < stDisks_info.disk_num; diskno++)
	{
		item.nItem = i;
		add_disk_info_list(hListView, &item, diskno, &stDisks_info.disk_info[diskno]);
		i++;
	}
	
	SendMessage (hListView, MSG_FREEZECTRL, FALSE, 0);

	return 0;
}


static void disk_list_notify_process (HWND hwnd, int id, int code, DWORD addData)
{
	if (code == LVN_ITEMDBCLK) 
	{   
		HLVITEM hlvi = SendMessage (hwnd, LVM_GETSELECTEDITEM, 0, 0);
        if (hlvi > 0) 
		{
		//	MessageBox(hwnd, "disk list", "message", MB_OK);
        }
	}
}

static void UpdateRecordParam(HWND hDlg, int ch)
{	
	int ret;
	char acRecordTime[4];
	NVR_DEV_REC_STRC_S stRec;	
	SendDlgItemMessage(hDlg, IDL_RECORD_CH, CB_SETCURSEL, ch, 0);
	memset(&stRec, 0, sizeof(NVR_DEV_REC_STRC_S));
	ret = nvr_logon_get_rec_cfg(ch, &stRec);
	if (ret < 0)
	{
		printf("get rec cfg failed\r\n");
	}	
	SendDlgItemMessage(hDlg, IDL_RECORD_STREAM_TYPE, CB_SETCURSEL, 0/*Rec.u8RecStream*/, 0);//暂时只设置主码流
	
	if (stRec.u8Enable && stRec.u8RecType <= 3)
		SendDlgItemMessage(hDlg, IDL_RECORD_TYPE, CB_SETCURSEL, stRec.u8RecType, 0);
	else	
		SendDlgItemMessage(hDlg, IDL_RECORD_TYPE, CB_SETCURSEL, TABLESIZE(strRecorType) - 1, 0);
	//InvalidateRect(hDlg, &recordTypeRect, TRUE);//解决录像策略选项不实时更新显示
	
	sprintf(acRecordTime, "%d", stRec.u8PreRecSecs);
	SetDlgItemText(hDlg, ID_FORMAT_TIMER, acRecordTime);
	if(stRec.u8OverWrite == 1)
		SendDlgItemMessage(hDlg, IDC_RECORD_OVER_WRITTING, BM_SETCHECK, BST_CHECKED, 0);
	else	
		SendDlgItemMessage(hDlg, IDC_RECORD_OVER_WRITTING, BM_SETCHECK, BST_UNCHECKED, 0);
	
}


static void InitDiskCombox(HWND hDlg, int ch)
{
	int i;
	char acBuf[4];
	
	SendDlgItemMessage(hDlg,IDL_RECORD_CH, CB_RESETCONTENT, 0, (LPARAM)0);
	for (i = 0; i < 4; i++)
	{
		sprintf(acBuf, "%d", i+1);
		SendDlgItemMessage(hDlg, IDL_RECORD_CH, CB_ADDSTRING, 0, (LPARAM)acBuf);		
	}
	
	SendDlgItemMessage(hDlg,IDL_RECORD_STREAM_TYPE, CB_RESETCONTENT, 0, (LPARAM)0);	
	SendDlgItemMessage(hDlg, IDL_RECORD_STREAM_TYPE, CB_ADDSTRING, 0, (LPARAM)LTR("主码流"));	
	//SendDlgItemMessage(hDlg, IDL_RECORD_STREAM_TYPE, CB_ADDSTRING, 0, (LPARAM)LTR("次码流"));
	
	SendDlgItemMessage(hDlg,IDL_RECORD_TYPE, CB_RESETCONTENT, 0, (LPARAM)0);	
	for (i = 0; i < TABLESIZE(strRecorType); i++)
		SendDlgItemMessage(hDlg, IDL_RECORD_TYPE, CB_ADDSTRING, 0, (LPARAM)LTR(strRecorType[i]));

	UpdateRecordParam(hDlg, ch);
}

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
    TextOut (hdc, rcDraw->left+4, rcDraw->top+4, LTR(caption[idx].Text));
}
#endif



static void InitDiskManageParam(HWND hDlg)
{	
	HWND hCtrl;
	if (NeedWizard())
	{
		hCtrl = GetDlgItem(hDlg, IDC_DISK_MANAGE_SAVE);
		ShowWindow(hCtrl,SW_HIDE);
		hCtrl = GetDlgItem(hDlg, IDC_DISK_MANAGE_USE);
		ShowWindow(hCtrl,SW_HIDE);
		hCtrl = GetDlgItem(hDlg, IDC_REC_MANAGE_REFRESH);
		ShowWindow(hCtrl,SW_HIDE);
		hCtrl = GetDlgItem(hDlg, IDC_DISK_MANAGE_CANCEL);
		ShowWindow(hCtrl,SW_HIDE);
	}
	else
	{
		hCtrl = GetDlgItem(hDlg, IDC_DISK_MAN_PREV);
		ShowWindow(hCtrl,SW_HIDE);
		hCtrl = GetDlgItem(hDlg, IDC_DISK_MAN_WIZARD_OK);
		ShowWindow(hCtrl,SW_HIDE);
	}
	
	init_disk_list_header(hListView);
	InitDiskCombox(hDlg, 0);
	init_disk_list_info(hListView);	
	SetNotificationCallback (hListView, disk_list_notify_process);

	SendMessage(hProgressBar, PBM_SETRANGE, 0, 100);	
	SendMessage(hProgressBar, PBM_SETSTEP, 1, 0);	
	ShowWindow(hProgressBar, SW_HIDE);

#ifdef CUSTOM_DRAW
    LVCUSTOMDRAWFUNCS myfuncs = {my_draw_hdr_bk, my_draw_hdr_item};
    SendMessage (hListView, LVM_SETCUSTOMDRAW, 0, (LPARAM) &myfuncs);	
#endif
}


static void DiskFormatStart(HWND hDlg)
{
	if (g_stFormatStatus.status != FORMAT_FREE)
		return;

	char acText[64];
	int diskno = 0;
	int rowNum = 0;
	HLVITEM hItem;
	rowNum = SendMessage(hListView, LVM_GETITEMCOUNT, 0, 0);
	if(rowNum <= 0)
		return;
	hItem = SendMessage(hListView, LVM_GETSELECTEDITEM, 0, 0);
	LVSUBITEM lvSubitem;
	lvSubitem.subItem = 0;
	lvSubitem.pszText = acText;
	SendMessage (hListView, LVM_GETSUBITEMTEXT, hItem, (LPARAM)&lvSubitem);		
	diskno = atoi(lvSubitem.pszText);				
	g_stFormatStatus.diskno = diskno;
	g_stFormatStatus.persent = 0;
	storage_lib_format_disk_start(g_stFormatStatus.diskno, 0);	
	g_stFormatStatus.status = FORMAT_BUSY;
	if (!IsTimerInstalled(hDlg, ID_FORMAT_TIMER))
		SetTimer(hDlg, ID_FORMAT_TIMER, 50);
	
	SendMessage(hProgressBar,PBM_SETPOS, 0, 0);
	ShowWindow(hProgressBar, SW_SHOW);
	
}

static int DiskFormatProc(HWND hDlg)
{
	int ret;
	if (g_stFormatStatus.status != FORMAT_BUSY)
	{	
		SendMessage(hDlg, MSG_FORMAT_EXIT, 0 , 0);
		return 0;
	}
		
	ret= storage_lib_format_get_percent(&g_stFormatStatus.persent);
	if (ret < 0 || g_stFormatStatus.persent < 0)
	{	
		storage_lib_format_disk_stop(g_stFormatStatus.diskno);
		g_stFormatStatus.status = FORMAT_ERROR;
		return -1;
	}
	
	if (g_stFormatStatus.persent >= 100)
	{		
		g_stFormatStatus.status = FORMAT_SUCCESS;
	}

	int pos = g_stFormatStatus.persent;
	if ( pos < 0 || pos > 100)
		pos = 0;

	printf("progress status %d\r\n", pos);
	SendMessage(hProgressBar,PBM_SETPOS, pos, 0);

	return 0;
}

static int DiskFormatStop(HWND hDlg)
{
	KillTimer(hDlg, ID_FORMAT_TIMER);
	storage_lib_format_disk_stop(g_stFormatStatus.diskno);
	g_stFormatStatus.diskno = -1;
	g_stFormatStatus.persent = 0;
	ShowWindow(hProgressBar,SW_HIDE);	
	if(g_stFormatStatus.status == FORMAT_SUCCESS)
#ifdef OWN_MESSAGEBOX_WIN		
		CreateMessageBoxWin(hDlg, LTR("格式化完成!"), LTR("消息"), MB_OK);
	else
		CreateMessageBoxWin(hDlg, LTR("格式化失败!"), LTR("消息"), MB_OK);
#else
		MessageBox(hDlg, LTR("格式化完成!"), LTR("消息"), MB_OK);
	else
		MessageBox(hDlg, LTR("格式化失败!"), LTR("消息"), MB_OK);	
#endif
	g_stFormatStatus.status = FORMAT_FREE;


	return 0;
}

static void GetDiskmanageDlgParam(HWND hDlg)
{
	int streamType, recordType;
	int bOverwrite;

	memset(&recCfg, 0, sizeof(gui_diskmanage_cfg));
	recordType = SendDlgItemMessage (hDlg, IDL_RECORD_TYPE, CB_GETCURSEL, 0, 0);
	if (recordType >= 0 && recordType < TABLESIZE(strRecorType) - 1)
	{
		recCfg.u8Enable = 1;
		recCfg.u8RecType = recordType;
	}
	else
		recCfg.u8Enable = 0;

	streamType = SendDlgItemMessage (hDlg, IDL_RECORD_STREAM_TYPE, CB_GETCURSEL, 0, 0);
	if (streamType < 0) return;
	recCfg.u8RecStream = streamType;

	bOverwrite = SendDlgItemMessage(hDlg, IDC_RECORD_OVER_WRITTING, BM_GETCHECK, 0, 0);
	if (bOverwrite)
		recCfg.u8OverWrite = 1;
	else	
		recCfg.u8OverWrite = 0;
/*
	GetDlgItemText(hDlg, IDC_PRE_RECORD_TIME, acText, sizeof(acText) - 1);
	recCfg.u8PreRecSecs = atoi(acText);
	if (recCfg.u8PreRecSecs < 0 || recCfg.u8PreRecSecs > 10)
		recCfg.u8PreRecSecs = 10;
*/	
}

int ExchangeDiskmanageDlgParam(NVR_DEV_REC_STRC_S *stRec)
{
	if(NULL == stRec)
		return 0;

	stRec->u8OverWrite = recCfg.u8OverWrite;
	stRec->u8RecStream = recCfg.u8RecStream;
	stRec->u8Enable = recCfg.u8Enable;
	stRec->u8RecType = recCfg.u8RecType;

	return 1;
}

static void SaveDiskInfo(HWND hDlg)
{
	int ch;
	int ret;
	NVR_DEV_REC_STRC_S stRec;
	
	ch = SendDlgItemMessage (hDlg, IDL_RECORD_CH, CB_GETCURSEL, 0, 0);
	if (ch < 0) return;
	printf("####################ch = %d\r\n",ch);
	memset(&stRec, 0, sizeof(NVR_DEV_REC_STRC_S));
	ret = nvr_logon_get_rec_cfg(ch, &stRec);
	if (ret < 0)
	{
		printf("get rec cfg failed\r\n");
		return;
	}
	GetDiskmanageDlgParam(hDlg);
	ExchangeDiskmanageDlgParam(&stRec);
	nvr_logon_set_rec_cfg(ch, &stRec);
}

static int DiskManageWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		hListView = GetDlgItem (hDlg, IDC_DISK_INFO_LISTVIEW);
		hProgressBar = GetDlgItem(hDlg,IDC_DISK_FORMAT_PROGRESS);
		InitDiskManageParam(hDlg);
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);
        EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg, "录像管理");
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
		Gui_WriteTitleName(hdc, &diskmanage_title_rect, LTR("录像管理"));
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
		int id   = LOWORD(wParam);
        int code = HIWORD(wParam);
		
		if (id == IDL_RECORD_CH)
		{		
			if (code == CBN_SELCHANGE)
			{	
				int ch;
				ch = SendDlgItemMessage (hDlg, IDL_RECORD_CH, CB_GETCURSEL, 0, 0);
				if (ch < 0) return 0;
				UpdateRecordParam(hDlg, ch);
				InvalidateRect(hDlg, NULL, FALSE);
				return 0;
			}
			break;
		}
			
		switch (wParam) 
		{

			case IDC_STORAGE_COPY:
			{
				int ch;
				ch = SendDlgItemMessage (hDlg, IDL_RECORD_CH, CB_GETCURSEL, 0, 0);
				if (ch < 0) return 0;
				GetDiskmanageDlgParam(hDlg);
				CreateCopyDlgWin(hDlg, ch, EN_COPY_STORAGE_STRATEGY);
				return 0;
			}
			case IDC_DISK_MANAGE_SAVE:
			{
				if (g_stFormatStatus.status == FORMAT_BUSY)
				{
					printf("format busy######\r\n");
					return 0;
				}
				SaveDiskInfo(hDlg);				
				//PostMessage(hDlg, MSG_CLOSE, 0 , 0);
				SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
			}
			return 0;
			case IDC_DISK_MANAGE_USE:
			{
				if (g_stFormatStatus.status == FORMAT_BUSY)
				{
					printf("format busy######\r\n");
					return 0;
				}
				SaveDiskInfo(hDlg);	
			}
			return 0;
			case IDC_STORAGE_TIME_SET:
			{				
				int ch;
				ch = SendDlgItemMessage (hDlg, IDL_RECORD_CH, CB_GETCURSEL, 0, 0);
				if (ch < 0) return 0;
				CreateRecordTimeDlgWin(hDlg, ch);
				break;
			}
			case IDC_REC_MANAGE_REFRESH:
			{
				int ch;
				ch = SendDlgItemMessage (hDlg, IDL_RECORD_CH, CB_GETCURSEL, 0, 0);
				if (ch < 0) return 0;
				UpdateRecordParam(hDlg, ch);
			}
			return 0;
			case IDC_DISK_MANAGE_REFRESH:
			{	
				if (g_stFormatStatus.status == FORMAT_BUSY)
				{
					printf("format busy######\r\n");
					return 0;
				}
				init_disk_list_info(hListView);
				ShowWindow(hProgressBar,SW_HIDE);
			
			}
			return 0;
			case IDC_DISK_MAN_PREV:
			case IDC_DISK_MANAGE_CANCEL:
			{
				if (g_stFormatStatus.status == FORMAT_BUSY)
				{
					printf("format busy######\r\n");
					return 0;
				}
				PostMessage(hDlg, MSG_CLOSE, 0 , 0);
			}
			return 0;
			
			case IDC_DISK_MANAGE_FORMAT:
			{
				int ret;
				
#ifdef OWN_MESSAGEBOX_WIN
				ret = CreateMessageBoxWin(hDlg, LTR("确认格式化磁盘空间!"), LTR("消息"), MB_OKCANCEL);
#else
				ret = MessageBox(hDlg, LTR("确认格式化磁盘空间!"), LTR("消息"), MB_OKCANCEL);
#endif
				if(ret == IDOK)
				{
					DiskFormatStart(hDlg);
				}
	
				return 0;
			}

			case IDC_DISK_MAN_WIZARD_OK:
			{
				if (g_stFormatStatus.status == FORMAT_BUSY)
				{
					printf("format busy######\r\n");
					return 0;
				}				
				SaveDiskInfo(hDlg);				
				BroadcastMessage(MSG_WIZARD_EXIT,  0,  0);
			}
			break;
		}
        break;
    }
	case MSG_TIMER:
	{
		switch(wParam)
		{
			case ID_FORMAT_TIMER:
			{
				DiskFormatProc(hDlg);
				return 0;
			}
			break;
		}
	}
	break;
	case MSG_FORMAT_START:
	{			
		printf("format start ############\r\n");
		DiskFormatStart(hDlg);
		return 0;
	}
	case MSG_FORMAT_EXIT:
	{
		DiskFormatStop(hDlg);
	}
	return 0;
	case MSG_WIZARD_EXIT:
	{		
		PostMessage(hDlg, MSG_CLOSE, 0 , 0);
	}
	break;
    case MSG_CLOSE:
    {	
		if (g_stFormatStatus.status != FORMAT_FREE)
			return 0;
		hListView = HWND_INVALID;
		hProgressBar = HWND_INVALID;
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
	int index;

	GetDisplayLanguageIndex(&index);
	if(0 == index) //汉字
	{
		ui_disk_manage_control[8].x -= 40;
	}
}


//远程设备配置
void CreateDiskManageDlgWin(HWND hParent)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "DiskManageDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_disk_manage_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_disk_manage_control, sizeof(ui_disk_manage_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_disk_manage_control)
	AdjustPositionDlgWin();
    DiskManageDlg.controls = ui_disk_manage_control;
    
    DialogBoxIndirectParam (&DiskManageDlg, hParent, DiskManageWinProc, 0L);
	memcpy(ui_disk_manage_control, ctrolTmp, sizeof(ui_disk_manage_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

