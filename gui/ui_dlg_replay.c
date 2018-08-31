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
#include "ctrl_recinfobar.h"
#include "my_monthcal.h"
#include "ui_config.h"
#include "ui_dlg_head.h"
#include "nvr_lib_interface.h"
#include "ui_storage_interface.h"
#include "ui_replay_interface.h"
#include <player_interface.h>


#include "gui_core.h"
#include "storage_lib.h"

#define __BUTTON_

#define VOD_DLG_W		NVR_SCREEN_WIDTH
#define VOD_DLG_H		NVR_SCREEN_HEIGHT

#define VOD_PLAYER_WIN_W	VOD_DLG_W*7/10
#define VOD_PLAYER_WIN_H	(VOD_DLG_H*8/10 - 20)
#define VOD_PLAYER_WIN_X	4
#define VOD_PLAYER_WIN_Y	5
#define VOD_PLAYER_WIN_R	(VOD_PLAYER_WIN_X + VOD_PLAYER_WIN_W)
#define VOD_PLAYER_WIN_B	(VOD_PLAYER_WIN_Y + VOD_PLAYER_WIN_H)

#define VOD_PLAYER_CTRL_STATIC_X	(VOD_PLAYER_WIN_R+10)
#define VOD_PLAYER_CTRL_STATIC_Y	VOD_PLAYER_WIN_Y
#define VOD_PLAYER_CTRL_STATIC_R	(VOD_DLG_W - 20)
#define VOD_PLAYER_CTRL_STATIC_B	VOD_PLAYER_WIN_B
#define VOD_PLAYER_CTRL_STATIC_W	(VOD_PLAYER_CTRL_STATIC_R - VOD_PLAYER_CTRL_STATIC_X)
#define VOD_PLAYER_CTRL_STATIC_H	(VOD_PLAYER_CTRL_STATIC_B - VOD_PLAYER_CTRL_STATIC_Y)

#define _DEV_QUERY_LIST_X	(VOD_PLAYER_CTRL_STATIC_X + 10)
#define _DEV_QUERY_LIST_Y	200
#define _DEV_QUERY_LIST_W	(VOD_PLAYER_CTRL_STATIC_W - 20)
#define _DEV_QUERY_LIST_H	325

#define RECINFO_BAR_START_X	 (VOD_PLAYER_WIN_X+25) 
#define RECINFO_BAR_START_Y  (VOD_DLG_H - 100+5)

#define TB_HEIGHT              16
#define TB_WIDTH               22

typedef enum
{
	IDC_STATIC_PLAYER = 2900,
	IDL_READ_VOD_SELECT,
	IDC_VOD_MC,
	IDC_VOD_TIME_SCALE,
	IDC_REPLAY_PAUSE,
	IDC_PLAYBACK,
	IDC_SPEED,
	IDC_FULL_SCREEN,
	IDC_SLOW,
	IDC_CAPTURE,
	IDC_RECORD,
	IDC_PICTURE,
	IDC_CH1,
	IDC_CH2,
	IDC_CH3,
	IDC_CH4,
	IDC_STARTUP_RECORD,
	IDC_TIMER_RECORD,
	IDC_ALARM_RECORD,
	IDC_MANUAL_RECORD,
	//IDC_ALL_RECORD,
	ID_VOD_PLAY,
	IDC_24HOUR,
	IDC_2HOUR,
	IDC_1HOUR,
	IDC_30MIN,
	IDC_VOD_BACKUP,
	ID_REPLAY_STATUS_TIMER,
	ID_DISPLAY_CHAN_TIMER,
	IDC_QUERY_PROPSHEET,
	IDC_QUERY_LISTBOX0,
	IDC_QUERY_LISTBOX1,
	IDC_QUERY_LISTBOX2,
	IDC_QUERY_LISTBOX3,
	IDC_STATIC_REPLAY_STATE,
	ID_CHANGE_DAY,
}UI_REPLAY_ID;

//储存实时流显示模式和当前选择的通道
//供回放退出回复使用
//一天录像信息
static RecordInfo stRecordInfo = {0, 0};
static PlayCtrl_t stPlayCtrl = {0};

static int g_nAxiMaxTick;
static int g_nAxiMinTick;
static int listboxinitok = 0;

static int g_recTypeMask = 0; //

//满屏或缩放时player的窗口大小
//rectReplayScreen[1]满屏, rectReplayScreen[2]缩放
extern RECT rectReplayScreen[2];
extern HWND GetMainWin();

static CTRLDATA VodCtrl[] =
{
	{
        CTRL_STATIC,
        WS_CHILD | /*SS_NOTIFY|*/ WS_VISIBLE,
        VOD_PLAYER_WIN_X, VOD_PLAYER_WIN_Y, VOD_PLAYER_WIN_W, VOD_PLAYER_WIN_H,
        IDC_STATIC_PLAYER,
        "",
        0
    },
    CONFIG_FOUR_WORDS_BUTTON("录像播放", ID_VOD_PLAY, VOD_PLAYER_WIN_X+25, VOD_PLAYER_WIN_B+10),
	{
        GUI_BUTTON,
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP|WS_DISABLED|BS_BITMAP,
        VOD_PLAYER_WIN_X+120, VOD_PLAYER_WIN_B+10, 55, 25,
        IDC_REPLAY_PAUSE,
        "暂停",
		(DWORD)&button_pic[0],
		WS_EX_TRANSPARENT
    },
    CONFIG_TWO_WORDS_BUTTON("全屏", IDC_FULL_SCREEN, VOD_PLAYER_WIN_X+195, VOD_PLAYER_WIN_B+10),
	{
        GUI_BUTTON,
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP|WS_DISABLED|BS_BITMAP,        
        VOD_PLAYER_WIN_R-200-20, VOD_PLAYER_WIN_B+10, 55, 25,
        IDC_SLOW,
        "减速",
		(DWORD)&button_pic[0],
		WS_EX_TRANSPARENT
    },

	{
        GUI_BUTTON,
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP|WS_DISABLED|BS_BITMAP,
        VOD_PLAYER_WIN_R-135-10, VOD_PLAYER_WIN_B+10, 55, 25,
        IDC_SPEED,
        "加速",
		(DWORD)&button_pic[0],
		WS_EX_TRANSPARENT
    },
   
    {
	  	CTRL_STATIC,
        WS_CHILD | WS_VISIBLE,
		VOD_PLAYER_WIN_R-70, VOD_PLAYER_WIN_B+15, 90, 20,
        IDC_STATIC_REPLAY_STATE,
        "停止回放",
        0,        
		WS_EX_TRANSPARENT
    },

#ifdef __BUTTON_
	CONFIG_TWO_WORDS_BUTTON("24h", IDC_24HOUR, VOD_DLG_W-280, VOD_PLAYER_WIN_B+10),
	CONFIG_TWO_WORDS_BUTTON("2h", IDC_2HOUR, VOD_DLG_W - 220, VOD_PLAYER_WIN_B+10),
	CONFIG_TWO_WORDS_BUTTON("1h", IDC_1HOUR, VOD_DLG_W - 160, VOD_PLAYER_WIN_B+10),
	CONFIG_TWO_WORDS_BUTTON("30min", IDC_30MIN, VOD_DLG_W - 100 , VOD_PLAYER_WIN_B+10),

#else
	{
        "toolbar",
        WS_CHILD | WS_VISIBLE,
        VOD_DLG_W - 180 , VOD_DLG_H - 30, 
        HIWORD(MAKELONG (TB_HEIGHT, TB_WIDTH))*TABLESIZE (toolbar_items), 
        LOWORD(MAKELONG (TB_HEIGHT, TB_WIDTH)),
        IDC_VOD_TIME_SCALE,
        "30min",
        MAKELONG (TB_HEIGHT, TB_WIDTH)
    },
#endif 
	{
        MY_MONTHCALENDAR,
        WS_CHILD | WS_VISIBLE | MCS_NOTIFY | MCS_CHN,
        VOD_PLAYER_CTRL_STATIC_X+4, VOD_PLAYER_CTRL_STATIC_Y + 4 , VOD_DLG_W*3/11 - 10, 170,
        IDC_VOD_MC,
        "",
        0,
        0
   },
	{
		CTRL_RECINFOBAR,
		(WS_CHILD | WS_VISIBLE | RIBS_BORDER | RIBS_TIP),
		RECINFO_BAR_START_X, RECINFO_BAR_START_Y, 
		1200, 15,
		IDC_CH1,
		"",
		0, 
		0,
	},
	{
		CTRL_RECINFOBAR,
		WS_CHILD | WS_VISIBLE | RIBS_BORDER,
		RECINFO_BAR_START_X, RECINFO_BAR_START_Y + 15, 
		1200, 15,	
		IDC_CH2,
		"",
		0, 
		0,
	},
	{
		CTRL_RECINFOBAR,
		WS_CHILD | WS_VISIBLE | RIBS_BORDER,
		RECINFO_BAR_START_X, RECINFO_BAR_START_Y + 30, 
		1200, 15,		
		IDC_CH3,
		"",
		0, 
		0,
	},
	{
		CTRL_RECINFOBAR,
		WS_CHILD | WS_VISIBLE | RIBS_BORDER,
		RECINFO_BAR_START_X, RECINFO_BAR_START_Y + 45, 
		1200, 15,		
		IDC_CH4,
		"",
		0, 
		0,
	},
   //////////////////////////////////////////////////////////////////////*/
/*   {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKED,
        VOD_PLAYER_WIN_X+300,  VOD_DLG_H - 30, 90+20, 20,
        IDC_ALL_RECORD,
        "全部录像",
        0,
        WS_EX_TRANSPARENT
    },
*/  ////////////////////////////////////////////////
	{
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX /* | BS_CHECKED*/, 
        VOD_PLAYER_WIN_X+400+20,  VOD_DLG_H - 30, 90+30, 20,
        IDC_STARTUP_RECORD,
        "开机录像",
        0,
        WS_EX_TRANSPARENT
    },
	{
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX /* | BS_CHECKED*/,
        VOD_PLAYER_WIN_X+500+50+20,  VOD_DLG_H - 30, 90+20, 20,
		IDC_TIMER_RECORD,
        "定时录像",
        0,
        WS_EX_TRANSPARENT
    },
	{
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX /* | BS_CHECKED*/, 
        VOD_PLAYER_WIN_X+600+80+20*2,  VOD_DLG_H - 30, 90+30, 20,
        IDC_ALARM_RECORD,
        "入侵录像",
        0,
        WS_EX_TRANSPARENT
    },
    {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX /* | BS_CHECKED*/, 
        VOD_PLAYER_WIN_X+700+110+20*3,  VOD_DLG_H - 30, 90+20, 20,
        IDC_MANUAL_RECORD,
        "手动录像",
        0,
        WS_EX_TRANSPARENT
    },
	//////////////////////
	{
        GUI_PROPSHEET,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | LBS_AUTOCHECKBOX ,
        _DEV_QUERY_LIST_X, _DEV_QUERY_LIST_Y, _DEV_QUERY_LIST_W, _DEV_QUERY_LIST_H,
        IDC_QUERY_PROPSHEET,
        "录像列表",
        0,
        WS_EX_TRANSPARENT
    },
#if 1
	CONFIG_TWO_WORDS_BUTTON("备份", IDC_VOD_BACKUP, _DEV_QUERY_LIST_X, _DEV_QUERY_LIST_Y+ _DEV_QUERY_LIST_H + 10),

#endif
};


static DLGTEMPLATE DlgVod =
{
    WS_NONE,
    WS_EX_NONE,
    0, 0, VOD_DLG_W, VOD_DLG_H,
    "主菜单",
    0, 0,
    TABLESIZE(VodCtrl), NULL,
    0
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
static CTRLDATA Ch0Ctrl[] =
{
	{
        GUI_LISTBOX,
        WS_CHILD | WS_VISIBLE |  WS_VSCROLL | LBS_NOTIFY | LBS_AUTOCHECKBOX,
        2, 5, _DEV_QUERY_LIST_W - 9 , _DEV_QUERY_LIST_H - 35,
        IDC_QUERY_LISTBOX0,
        "",
        0
    }
};

static CTRLDATA Ch1Ctrl[] =
{
	{
        GUI_LISTBOX,
        WS_CHILD | WS_VISIBLE |  WS_VSCROLL | LBS_NOTIFY | LBS_AUTOCHECKBOX,
        2, 5, _DEV_QUERY_LIST_W - 9 , _DEV_QUERY_LIST_H - 35,
        IDC_QUERY_LISTBOX1,
        "",
        0
    }
};
static CTRLDATA Ch2Ctrl[] =
{
	{
        GUI_LISTBOX,
        WS_CHILD | WS_VISIBLE |  WS_VSCROLL | LBS_NOTIFY | LBS_AUTOCHECKBOX,
        2, 5, _DEV_QUERY_LIST_W - 9 , _DEV_QUERY_LIST_H - 35,
        IDC_QUERY_LISTBOX2,
        "",
        0
    }
};
static CTRLDATA Ch3Ctrl[] =
{
	{
        GUI_LISTBOX,
        WS_CHILD | WS_VISIBLE |  WS_VSCROLL | LBS_NOTIFY | LBS_AUTOCHECKBOX,
        2, 5, _DEV_QUERY_LIST_W - 9 , _DEV_QUERY_LIST_H - 35,
        IDC_QUERY_LISTBOX3,
        "",
        0
    }
};

DLGTEMPLATE DlgCh0 = 
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 0, 0,
    "通道1",
    0, 0,
    TABLESIZE(Ch0Ctrl), NULL,
    0
};

DLGTEMPLATE DlgCh1 = 
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, _DEV_QUERY_LIST_W, _DEV_QUERY_LIST_H,
    "通道2",
    0, 0,
    TABLESIZE(Ch1Ctrl), NULL,
    1
 };

DLGTEMPLATE DlgCh2 = 
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, _DEV_QUERY_LIST_W, _DEV_QUERY_LIST_H,
    "通道3",
    0, 0,
    TABLESIZE(Ch2Ctrl), NULL,
    2
};
DLGTEMPLATE DlgCh3 = 
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, _DEV_QUERY_LIST_W, _DEV_QUERY_LIST_H,
    "通道4",
    0, 0,
    TABLESIZE(Ch3Ctrl), NULL,
    3
};

static RECT chTextRect[2][5]=
{
	{{4, (72*8 - 20)/2 - 40+5, (128*7)/2 -1+4, (72*8 - 20)/2+5},/*ch1 显示位置*/
	{(128*7)/2+4, (72*8 - 20)/2 - 40+5, (128*7) -1+4, (72*8 - 20)/2+5},/*ch2 显示位置*/
	{4, (72*8 - 20) - 40+5, (128*7)/2 -1+4, 72*8 - 20+5},/*ch3 显示位置*/
	{(128*7)/2+4, (72*8 - 20) - 40+5, (128*7) -1+4, 72*8 - 20+5},/*ch4 显示位置*/
	{4, 72*8+5-20-40, 128*7+4-1, 72*8+5-20},/*全屏*/},

	{{0, NVR_SCREEN_HEIGHT/2 - 40, NVR_SCREEN_WIDTH/2 -1, NVR_SCREEN_HEIGHT/2},/*ch1 显示位置*/
	{NVR_SCREEN_WIDTH/2, NVR_SCREEN_HEIGHT/2 - 40, NVR_SCREEN_WIDTH -1, NVR_SCREEN_HEIGHT/2},/*ch2 显示位置*/
	{0, NVR_SCREEN_HEIGHT - 40, NVR_SCREEN_WIDTH/2 -1, NVR_SCREEN_HEIGHT},/*ch3 显示位置*/
	{NVR_SCREEN_WIDTH/2, NVR_SCREEN_HEIGHT - 40, NVR_SCREEN_WIDTH -1, NVR_SCREEN_HEIGHT},/*ch4 显示位置*/
	{0, 720 - 40, 1280-1, 720},/*整屏时显示位置*/}
};

typedef struct __UI_COLOR{
	Uint8 u8_red;
	Uint8 u8_green;
	Uint8 u8_blue;
}Ui_Color, *Pui_color;

/*颜色框坐标*/
static RECT colorRect[] = {{VOD_PLAYER_WIN_X+400+20-30,  VOD_DLG_H - 30, VOD_PLAYER_WIN_X+400+20-10, VOD_DLG_H - 10},
					{VOD_PLAYER_WIN_X+500+50+20-30,  VOD_DLG_H - 30, VOD_PLAYER_WIN_X+500+50+20-10, VOD_DLG_H - 10},
					{VOD_PLAYER_WIN_X+600+80+20*2-30,  VOD_DLG_H - 30, VOD_PLAYER_WIN_X+600+80+20*2-10, VOD_DLG_H - 10},
					{VOD_PLAYER_WIN_X+700+110+20*3-30, VOD_DLG_H - 30, VOD_PLAYER_WIN_X+700+110+20*3-10,  VOD_DLG_H - 10}};

static Ui_Color atColor[] =
{
	{0x00, 0x00, 0xff},	//STARTUP 蓝色
	{0x56, 0xaf, 0x48},	//TIMER 绿色
	{0xcf, 0x2d, 0x24},	//ALARM 浅红色
	{0x37, 0x41, 0x88},	//MANUAL 紫颜色
	{0x45, 0x45, 0x45},	//NONE 没有用到
};

extern ZOOM_PLAYERSRC playSrc[2];
extern void InitPlaySrc(int mode);
extern void ZoomPlaySrc(PLAY_MODE mode, int dir, int mouseX, int mouseY);
extern void RecoverBeforePlayState(PLAY_MODE mode);

static void VodStop(HWND hDlg)
{
	HWND hWind;
	pop_open_dlg(&headOpenDlg, &hWind);

	printf("VodStop###########\r\n");
	if (IsTimerInstalled(hDlg, ID_REPLAY_STATUS_TIMER))
	{
		KillTimer(hDlg, ID_REPLAY_STATUS_TIMER);
	}
	SendDlgItemMessage (hDlg, ID_VOD_PLAY, MSG_SETTEXT, 0, (LPARAM)LTR("录像播放"));
	stPlayCtrl.playdir = EN_PLAY_FORWARD;
	stPlayCtrl.speed = EN_NORMAL_RATE;
	player_stop(4);
	//此时调用阻塞到四个通道均退出
	player_status  status;
	player_get_player_status(0,&status);
	stPlayCtrl.status = 0;	
	SetDlgItemText(hDlg, IDC_STATIC_REPLAY_STATE,LTR("回放停止"));	
	EnableDlgItem(hDlg,IDC_REPLAY_PAUSE, FALSE);	
	EnableDlgItem(hDlg,IDC_SPEED, FALSE);
	EnableDlgItem(hDlg,IDC_SLOW, FALSE);
	
}

static void VodPause(HWND hDlg)
{
	player_replay_pause_clt(4, 1);
	EnableDlgItem(hDlg,IDC_SPEED, FALSE);
	EnableDlgItem(hDlg,IDC_SLOW, FALSE);	
	SendDlgItemMessage (hDlg, IDC_REPLAY_PAUSE, MSG_SETTEXT, 0, (LPARAM)LTR("播放"));	
	SetDlgItemText(hDlg, IDC_STATIC_REPLAY_STATE, LTR("暂停回放"));
}

static void VodPlay(HWND hDlg)
{	
	char *str[7]={"慢放三", "慢放二", "慢放一", "正常播放", "快进一", "快进二", "快进三"};
	player_replay_pause_clt(4, 0);
	SendDlgItemMessage (hDlg, IDC_REPLAY_PAUSE, MSG_SETTEXT, 0, (LPARAM)LTR("暂停"));
	if ((stPlayCtrl.speed>=EN_SLOW_RATE8) && (stPlayCtrl.speed<=EN_SPEED_RATE8))
		SetDlgItemText(hDlg, IDC_STATIC_REPLAY_STATE, LTR(str[stPlayCtrl.speed]));
	EnableDlgItem(hDlg,IDC_SPEED, TRUE);
	EnableDlgItem(hDlg,IDC_SLOW, TRUE);
}


static void VodStart(HWND hDlg, int time)
{	
	char *str[7]={"慢放三", "慢放二", "慢放一", "正常播放", "快进一", "快进二", "快进三"};
	
	push_open_dlg(&headOpenDlg, hDlg);

	printf("vod start time = %d\r\n", time);
	//设置普通速度正向播放
	//ui_set_speed_dir(EN_NORMAL_RATE, EN_PLAY_FORWARD);
	ui_set_speed_dir(stPlayCtrl.speed, stPlayCtrl.playdir);
	//stPlayCtrl.playdir = EN_PLAY_FORWARD;
	//stPlayCtrl.speed = EN_NORMAL_RATE;
	stPlayCtrl.status = 1; //播放
	stPlayCtrl.indicatorTime = time;	
	ui_replay_start(time , &stRecordInfo, stPlayCtrl.displaymode, stPlayCtrl.chSelect);
	player_set_dis_mode(stPlayCtrl.displaymode, stPlayCtrl.chSelect);
	//SetDlgItemText(hDlg, IDC_STATIC_REPLAY_STATE,LTR("录像播放"));	
	if ((stPlayCtrl.speed>=EN_SLOW_RATE8) && (stPlayCtrl.speed<=EN_SPEED_RATE8))
		SetDlgItemText(hDlg, IDC_STATIC_REPLAY_STATE, LTR(str[stPlayCtrl.speed]));
	SendDlgItemMessage (hDlg, ID_VOD_PLAY, MSG_SETTEXT, 0, (LPARAM)LTR("回放停止"));	
	SendDlgItemMessage (hDlg, IDC_REPLAY_PAUSE, MSG_SETTEXT, 0, (LPARAM)LTR("暂停"));	
	EnableDlgItem(hDlg,IDC_REPLAY_PAUSE, TRUE);
	EnableDlgItem(hDlg,IDC_SPEED, TRUE);
	EnableDlgItem(hDlg,IDC_SLOW, TRUE);
	if (IsTimerInstalled(hDlg, ID_REPLAY_STATUS_TIMER))
	{
		return;
	}	
	SetTimer (hDlg, ID_REPLAY_STATUS_TIMER, 100);					
}



static void ShowPage(HWND hDlg, int page)
{
	int i = 0;
	int fragementNum = 0;
	RecordQueryInfo *pstFragement = NULL;
	LISTBOXITEMINFO lbii;
	int height = 20;
	char acstart[64], acEnd[64];
	char acMsg[128];
	time_t beginTime, endTime;
	
	RecordInfo *pstRecordInfo = &stRecordInfo;
	
	SendDlgItemMessage(hDlg, IDC_QUERY_LISTBOX0+page , LB_SETITEMHEIGHT, 0, height);
	SendDlgItemMessage(hDlg, IDC_QUERY_LISTBOX0+page , LB_RESETCONTENT, 0, 0);
	fragementNum = pstRecordInfo->stChRecord[page].fragement;
	if (fragementNum <= 0)	
		return;	
	
	for (i = 0; i < fragementNum; i++)
	{
		memset(acMsg, 0, sizeof(acMsg));
		if (fragementNum == 300)
		{				
			break;
		}
		pstFragement = &pstRecordInfo->stChRecord[page].pstRecordHead[i];
		//printf("!!!!!!!!!!!!!!!!!!!%s[%d] type: 0x%x\r\n",__func__,__LINE__, pstFragement->record_type);
		struct tm *start, *end;	
		beginTime = pstFragement->begin_time;
		endTime = pstFragement->end_time;
		start = localtime(&beginTime);
		//sprintf(acstart, "%04d-%02d-%02d %02d:%02d:%02d", 
		//				start->tm_year+1900, start->tm_mon+1, start->tm_mday,
		//				start->tm_hour, start->tm_min, start->tm_sec);
		sprintf(acstart, "%02d:%02d:%02d", 
						start->tm_hour, start->tm_min, start->tm_sec);
		end = localtime(&endTime);
		//sprintf(acEnd, "%04d-%02d-%02d %02d:%02d:%02d", 
		//				end->tm_year+1900, end->tm_mon+1, end->tm_mday,
		//				end->tm_hour, end->tm_min, end->tm_sec);
		sprintf(acEnd, "%02d:%02d:%02d",
						end->tm_hour, end->tm_min, end->tm_sec);
		sprintf(acMsg, "%s ---- %s     %.2fM", acstart, acEnd, (float)pstFragement->fragment_size/(1024*1024));
		lbii.hIcon = 0;
	    lbii.cmFlag = CMFLAG_CHECKED; //CMFLAG_BLANK, CMFLAG_PARTCHECKED
	    lbii.string = (char *)acMsg;
		//printf("page = %d, acMsg = %s\r\n", page, acMsg);
	    SendDlgItemMessage (hDlg, IDC_QUERY_LISTBOX0+page, LB_ADDSTRING, 0, (LPARAM)&lbii);
	}	
}


static void ListBoxCommandProc(HWND hDlg, int id, int code)
{
	int ch, nSel;
	int offsetTime;
	if (code == LBN_DBLCLK)
	{
		RecordQueryInfo *pstFragement;
		nSel = SendDlgItemMessage(hDlg, id, LB_GETCURSEL, 0, 0);
		if (nSel < 0)	return;

		ch = id - IDC_QUERY_LISTBOX0;
		printf("ch = %d, nSel = %d\r\n", ch, nSel);

		int fragementNo;
		fragementNo = stRecordInfo.stChRecord[ch].fragement;
		if (nSel > fragementNo - 1) return;

	
		pstFragement = &stRecordInfo.stChRecord[ch].pstRecordHead[nSel];
		offsetTime = pstFragement->begin_time - stRecordInfo.tm_start;
		stPlayCtrl.chSelect = id - IDC_QUERY_LISTBOX0;
		VodStart(GetParent(GetParent(hDlg)), offsetTime);		
	}	
}

static int PageProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int page = 0;
	
    switch (message) {
    case MSG_INITPAGE:
       	{
		SetWindowBkColor(hDlg,0xFF0773BB);
		page = 	GetWindowAdditionalData(hDlg);
		if (page <0 || page > 4)
		{
			page = 0;
			printf("get page failed#######\r\n");
		}
		ShowPage(hDlg, page);
        } 
        break;

    case MSG_SHOWPAGE:
        return 1;

    case MSG_GETDLGCODE:
        return DLGC_WANTARROWS;

	case MSG_SHEETCMD:
        if (wParam == ID_CHANGE_DAY) 
		{
			page = 	GetWindowAdditionalData(hDlg);
			ShowPage(hDlg, page);
        }
        return 0;

    case MSG_COMMAND:
		{
			int id   = LOWORD(wParam);
	        int code = HIWORD(wParam);		
			if ((id >= IDC_QUERY_LISTBOX0)&&
				(id <= IDC_QUERY_LISTBOX3))
			{
				ListBoxCommandProc(hDlg, id , code);
				return 0;
			}
			else if (id == IDC_VOD_MC)
			{
				if (code == MCN_DATECHANGE || code == MCN_DATECLK)
				{
				}
				return 0;
			}
			
			switch (wParam) {
	        case IDOK:
	        case IDCANCEL:
#ifdef OWN_MESSAGEBOX_WIN
				CreateMessageBoxWin(hDlg, "Button pushed", "OK", MB_OK | MB_ICONINFORMATION | MB_BASEDONPARENT);
#else	        
	            MessageBox (hDlg, "Button pushed", "OK", MB_OK | MB_ICONINFORMATION | MB_BASEDONPARENT);
#endif
	            break;
	        }
	        break;
    	}
		break;
        
    }
    
    return DefaultPageProc (hDlg, message, wParam, lParam);
}


static void InitRecordListBox(HWND hDlg)
{
	DlgCh0.caption = LTR("通道1");
	DlgCh1.caption = LTR("通道2");
	DlgCh2.caption = LTR("通道3");
	DlgCh3.caption = LTR("通道4");

	HWND pshwnd = GetDlgItem (hDlg, IDC_QUERY_PROPSHEET);
    DlgCh0.controls = Ch0Ctrl;
    SendMessage (pshwnd, PSM_ADDPAGE,
                    (WPARAM)&DlgCh0, (LPARAM)PageProc);
    
    DlgCh1.controls = Ch1Ctrl;
    SendMessage ( pshwnd, PSM_ADDPAGE, 
                    (WPARAM)&DlgCh1,(LPARAM) PageProc);
    
    DlgCh2.controls = Ch2Ctrl;
    SendMessage ( pshwnd, PSM_ADDPAGE, 
                    (WPARAM)&DlgCh2,(LPARAM)PageProc);

    DlgCh3.controls = Ch3Ctrl;
    SendMessage ( pshwnd, PSM_ADDPAGE, 
                    (WPARAM)&DlgCh3, (LPARAM) PageProc);
	
	SendMessage(pshwnd, PSM_SETACTIVEINDEX, 0, 0);

}


void ReplayTimerProc(HWND hDlg)
{
	int chn;
	int offSetTime;
	int ret;	
	#if 1
	ret = ui_replay_callback();
	if (ret == -1)
	{	
		stPlayCtrl.status = 0; 
		if (IsTimerInstalled(hDlg, ID_REPLAY_STATUS_TIMER))
			KillTimer(hDlg, ID_REPLAY_STATUS_TIMER);
		return;
	}
	offSetTime = ui_replay_get_time();	
	if (offSetTime < 0 || offSetTime > 24*3600)
		return;
	for (chn = 0; chn < 4; chn++)
	{
		SendDlgItemMessage(hDlg, IDC_CH1+chn, RIBM_SETINDCPOS, 
			(WPARAM)offSetTime, 0);
	}
	stPlayCtrl.indicatorTime = offSetTime;
#endif
}

static void DisplayTimerProc(HWND hDlg)
{
	int i;
	
	if(stPlayCtrl.bfullscreen)
	{
		InvalidateRect(hDlg, NULL, FALSE);
	}
	else
	{
		if(stPlayCtrl.displaymode == 0)
		{
			for(i=0; i<NVR_MAX_CHANNEL_NUM; i++)
			{
				InvalidateRect(hDlg, &chTextRect[0][i], FALSE);
			}
		}
		else 
		{
			InvalidateRect(hDlg, &chTextRect[0][4], FALSE);
		}
	}
}

//将有录像的天使置红色色
static void __SetCalendarRecord(HWND hDlg, int mask)
{
	int i;	
	int monthLen;

	SendDlgItemMessage(hDlg,IDC_VOD_MC, MCM_CLEARDAYCOLOR, 0, 0);
	monthLen = SendDlgItemMessage(hDlg, IDC_VOD_MC, MCM_GETCURMONLEN, 0, 0);
	
	for (i = 0; i<monthLen; i++)
	{		
		SendDlgItemMessage(hDlg,IDC_VOD_MC, MCM_SETDAYCOLOR, i+1, PIXEL_lightwhite);
		if ((mask>>i)&0x01)
		{
			SendDlgItemMessage(hDlg,IDC_VOD_MC, MCM_SETDAYCOLOR, i+1, PIXEL_red);
		}			
	}
}

//获取一个月的录像
static int __GetMonthMask(HWND hDlg)
{
	int mask = 0;
	SYSTEMTIME curdate;
	SendDlgItemMessage (hDlg, IDC_VOD_MC, MCM_GETCURDATE, 0, (LPARAM) &curdate);
	
	mask =  storage_lib_record_find(0xf, curdate.year, curdate.month, g_recTypeMask/*RECORD_ALL_TYPE*/);
	if (mask < 0)
		return -1;

	return mask;
}


static void _CalendarSetRec(HWND hDlg)
{
	int mask = 0;
	mask = __GetMonthMask(hDlg);
	if (mask < 0) mask = 0;
	
	__SetCalendarRecord(hDlg, mask);
	
}

static void _InitplayerWindow(HWND hWnd)
{
	HWND hStatic;
	hStatic = GetDlgItem(hWnd, IDC_STATIC_PLAYER);

	SetWindowBkColor(hStatic, MY_PIXEL_transparent);

}

//绘制录像录像分布信息
static void GetStorageInfoAndDisplay(HWND hDlg)
{
	int fragementNum = 0;
	int ch = 0,i = 0, k = 0;	
	SYSTEMTIME curdate; 
	RecordQueryInfo *pstFragement = NULL;
	RECINTERVAL recinfo;
	
	SendDlgItemMessage (hDlg, IDC_VOD_MC, MCM_GETCURDATE, 0, (LPARAM) &curdate);
	ui_param_get_one_day_record(&stRecordInfo, curdate.year, curdate.month, curdate.day, g_recTypeMask);
	if (stRecordInfo.tm_fistfileTime == (time_t)-1)
		stPlayCtrl.indicatorTime = 0;
	else
		stPlayCtrl.indicatorTime = stRecordInfo.tm_fistfileTime - stRecordInfo.tm_start;
	for (ch = 0; ch < 4; ch++)
	{			
		SendDlgItemMessage(hDlg, IDC_CH1+ch, RIBM_CLEAR, TRUE, 0);
		SendDlgItemMessage(hDlg, IDC_CH1+ch, RIBM_SETINDCPOS, (WPARAM) stPlayCtrl.indicatorTime, 0);
		SendDlgItemMessage(hDlg, IDC_CH1+ch, RIBM_SET_CUR_MINMAX, (WPARAM) g_nAxiMinTick, (LPARAM) g_nAxiMaxTick);
		for (k = REC_MANNUAL; k <= REC_STARTUP; k++)
			SendDlgItemMessage(hDlg, IDC_CH1+ch, RIBM_SET_STAT_TYPE, (WPARAM) k, (LPARAM) TRUE);
		fragementNum = stRecordInfo.stChRecord[ch].fragement;

		if (fragementNum<= 0)	continue;
		for (i = 0; i < fragementNum; i++)
		{
			if (fragementNum == MAX_INTRL_NUM)
				break;
			pstFragement = &stRecordInfo.stChRecord[ch].pstRecordHead[i];
			recinfo.unValid = 1;
			recinfo.nStart = pstFragement->begin_time - stRecordInfo.tm_start;
			recinfo.nEnd = pstFragement->end_time - stRecordInfo.tm_start;
			//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%s[%d] : record_type:0x%x\r\n",__FILE__,__LINE__,pstFragement->record_type);
#if 1	
			if (pstFragement->record_type & RECORD_ALARM_TYPE & g_recTypeMask)
			{
				recinfo.nRecType = REC_ALARM_MASK;
			}
			else if (pstFragement->record_type & RECORD_TIME_TYPE & g_recTypeMask)
			{
				recinfo.nRecType = REC_TIMER_MASK;
			}
			else if (pstFragement->record_type & RECORD_STARTUP_TYPE & g_recTypeMask)
			{
				recinfo.nRecType = REC_STARTUP_MASK;
			}
			else if(pstFragement->record_type & RECORD_MANUAL_TYPE & g_recTypeMask)
			{
				recinfo.nRecType = REC_MANNUAL_MASK;
			}
#endif
			//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%s[%d] recinfo.nRecType: 0x%x,g_recTypeMask: 0x%x\r\n",__FILE__,__LINE__,recinfo.nRecType,g_recTypeMask);
			//recinfo.nRecType = REC_MANNUAL_MASK;	
			SendDlgItemMessage(hDlg, IDC_CH1+ch, RIBM_SETINTRL, (WPARAM) &recinfo, 0L);
		}	

		SendDlgItemMessage(hDlg, IDC_CH1+ch, RIBM_SHOWINDC, 0, 0L);
	}
}

static void* __init_listbox_task(void * argv)
{
	HWND hDlg = (unsigned int)argv;	
	usleep(200000);
	InitRecordListBox(hDlg);	
	listboxinitok = 1;
	return NULL;
}


static void RecordListBoxThread(HWND hDlg)
{
	pthread_t _listshow_thread;
	pthread_attr_t new_attr;	
	pthread_attr_init (&new_attr);
	pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
	pthread_create (&_listshow_thread, &new_attr, __init_listbox_task, (void *)hDlg);	
	pthread_attr_destroy (&new_attr);
}

static void _ui_init_rerplay_ctrl(HWND hDlg)
{
	_InitplayerWindow(hDlg);

	g_nAxiMinTick = 0;	
	g_nAxiMaxTick = 3600*24;
	//显示日历控件录像信息
	
	_CalendarSetRec(hDlg);
	//获取一天的录像信息，并显示时间分布	
	GetStorageInfoAndDisplay(hDlg);
	
	
	//显示一天录像文件列表
	RecordListBoxThread(hDlg);
	
}

static void DrawReplayChRect(HDC hdc, int dis_mode)
{
	if (stPlayCtrl.chSelect < 0)
		return;
	int screen = 0;
	if (stPlayCtrl.bfullscreen)
		screen = 0;
	else
		screen = 1;
	
	SetPenWidth(hdc,3); 
	SetBrushType (hdc, BT_SOLID);
	SetPenColor(hdc, 0xff0072B9);

	LineEx(hdc, rectReplayScreen[screen].left, rectReplayScreen[screen].top, rectReplayScreen[screen].right, rectReplayScreen[screen].top);		
	LineEx(hdc, rectReplayScreen[screen].right, rectReplayScreen[screen].top, rectReplayScreen[screen].right, rectReplayScreen[screen].bottom);		
	LineEx(hdc, rectReplayScreen[screen].left, rectReplayScreen[screen].top, rectReplayScreen[screen].left, rectReplayScreen[screen].bottom);		
	LineEx(hdc, rectReplayScreen[screen].left, rectReplayScreen[screen].bottom, rectReplayScreen[screen].right, rectReplayScreen[screen].bottom);

	if (dis_mode == 0)
	{
		LineEx(hdc,rectReplayScreen[screen].left+(RECTW(rectReplayScreen[screen])>>1), 
				rectReplayScreen[screen].top,
				rectReplayScreen[screen].left+(RECTW(rectReplayScreen[screen])>>1),
				rectReplayScreen[screen].bottom);
		LineEx(hdc,rectReplayScreen[screen].left, 
				rectReplayScreen[screen].top+(RECTH(rectReplayScreen[screen])>>1),
				rectReplayScreen[screen].right,
				rectReplayScreen[screen].top+(RECTH(rectReplayScreen[screen])>>1));

		SetPenColor(hdc, 0xFFD0D1D3);		
		int x = rectReplayScreen[screen].left + (stPlayCtrl.chSelect%2)*RECTW(rectReplayScreen[screen])/2;
		int y = rectReplayScreen[screen].top + (stPlayCtrl.chSelect/2)*RECTH(rectReplayScreen[screen])/2;
		int w = (RECTW(rectReplayScreen[screen])>>1);
		int h = (RECTH(rectReplayScreen[screen])>>1);
		
		Rectangle(hdc,x+1, y+1, x+w-1, y+h-1);
	}
	
}


static void BarIndcChangedProc(HWND hwnd, int id, int code)
{
	int i = 0;
	int nNewPos = 0;
	
	SendDlgItemMessage(hwnd, id, RIBM_GETINDCPOS, 0L, (LPARAM) &nNewPos);
	for (i = 0; i < 4; ++i)
	{
		if (id != IDC_CH1+i)
			SendDlgItemMessage(hwnd, IDC_CH1+i, RIBM_SETINDCPOS, (WPARAM)nNewPos, 0);
	}
}

void BarCurMinMaxChanged(HWND hwnd, int id, int code)
{
	int i = 0;
	for (i = 0; i < 4; ++i)
	{
		if (id != IDC_CH1+i)
			SendDlgItemMessage(hwnd, IDC_CH1+i, RIBM_SET_CUR_MINMAX, 
							  (WPARAM) g_nAxiMinTick, (LPARAM) g_nAxiMaxTick);
	}	
}

static void DrawAxisLabelAndTime(HDC hdc)
{
	int Axi_x;
	int Axi_y;
	int Text_w, Text_h;
	RECT rcTemp;	
			
	int i = 0;
	int step = 12; 
	int nTick = 0;
	char szTimeLabl[32] = {'\0'};
	int dx = 1200/12;
	Axi_x = RECINFO_BAR_START_X - 20;
	Axi_y = RECINFO_BAR_START_Y - 16;
	
	UINT uFormat = DT_TOP;
	SetPenColor(hdc, PIXEL_lightgray);
	SetPenWidth(hdc,3);
	SetTextColor(hdc, PIXEL_lightgray);
    SetBkMode (hdc, BM_TRANSPARENT);

	for(i = 0; i < step + 1; i++) 
	{		
		MoveTo(hdc, RECINFO_BAR_START_X+dx*i, RECINFO_BAR_START_Y);
		LineTo(hdc, RECINFO_BAR_START_X+dx*i, RECINFO_BAR_START_Y - 4);

		
		Text_w = dx - 2;
		Text_h = 20;
		nTick = g_nAxiMinTick + (g_nAxiMaxTick - g_nAxiMinTick)*i/(float)step;
		sprintf(szTimeLabl, "%02d:%02d:%02d", nTick/(60*60), (nTick/60)%60, nTick%60);
		SetRect(&rcTemp, Axi_x+ 1+dx*i, Axi_y, Axi_x+1+dx*i+Text_w, Axi_y+Text_h);	
		SelectFont(hdc,GetSystemFont(SYSLOGFONT_FIXED));
		DrawText(hdc, szTimeLabl, -1, &rcTemp, uFormat);
	}	

	for(i=0; i< TABLESIZE(colorRect); i++)  //绘制不同的颜色框
	{
		SetBrushColor(hdc, RGB2Pixel(HDC_SCREEN, atColor[i].u8_red, atColor[i].u8_green, atColor[i].u8_blue));
		FillBox(hdc, colorRect[i].left, colorRect[i].top, RECTW(colorRect[i]), RECTH(colorRect[i]));
	}
	
}


static void ReplayUpdateSelectCh(int pt_x, int pt_y)
{
	if (stPlayCtrl.displaymode == 0)
	{
		int w, h;
		if (stPlayCtrl.bfullscreen)
		{
			w = RECTW(rectReplayScreen[0])>>1;
			h = RECTH(rectReplayScreen[0])>>1;
		}
		else
		{
			w = RECTW(rectReplayScreen[1])>>1;
			h =  RECTH(rectReplayScreen[1])>>1;
		}
		stPlayCtrl.chSelect = (pt_x/w) + (pt_y/h)*2;

		printf("stPlayCtrl.chSelect = %d\r\n", stPlayCtrl.chSelect);
	}
}
static void __VodStopOtherCh(int chn_sel)
{
	int chn = 0;
	for (chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++)
	{
		if (chn == chn_sel) continue;
		player_stop(chn);
		printf("#############player_stop %d\r\n", chn);
	}
}
static void RepaySwitchDisplayMode(HWND hDlg)
{
	int ret;
	player_status status[NVR_MAX_CHANNEL_NUM];
	if (stPlayCtrl.displaymode == 0)
	{
		ret = player_set_dis_mode(1, stPlayCtrl.chSelect);
		ui_replay_set_mode(1,stPlayCtrl.chSelect,stPlayCtrl.status);
		__VodStopOtherCh(stPlayCtrl.chSelect);
		ui_set_speed_dir(stPlayCtrl.speed, -1);
	}
	else if (stPlayCtrl.displaymode == 1)
	{	
		ret = player_set_dis_mode(0, stPlayCtrl.chSelect);			
		ui_replay_set_mode(0,stPlayCtrl.chSelect,stPlayCtrl.status);
		ui_set_speed_dir(stPlayCtrl.speed, -1);
		if (stPlayCtrl.speed > EN_NORMAL_RATE)
		{
			//ui_set_speed_dir(EN_NORMAL_RATE, EN_PLAY_FORWARD);
			//stPlayCtrl.playdir = EN_PLAY_FORWARD;
			//stPlayCtrl.speed = EN_NORMAL_RATE;
			//ui_set_speed_dir(stPlayCtrl.speed, -1);
			//stPlayCtrl.status = 1; //播放		
			//SetDlgItemText(hDlg, IDC_STATIC_REPLAY_STATE,LTR("录像播放"));	
		}
	}

	if (ret < 0)
		return;
	player_get_player_status(NVR_MAX_CHANNEL_NUM, status);
	stPlayCtrl.displaymode = player_get_display_mode();
	printf("ch %d, switch mode %d\r\n",stPlayCtrl.chSelect, stPlayCtrl.displaymode);
}

static void RefreshFrameClip(HWND hDlg)
{
	if (stPlayCtrl.bfullscreen)
		InvalidateRect(hDlg, &rectReplayScreen[0], TRUE);
	else	
		InvalidateRect(hDlg, &rectReplayScreen[1], FALSE);
}

static void ReplayInitPlayCtrl(HWND hDlg, int dismode)
{
	int displayMode, i;
	if (dismode ==0 || dismode == 1 )
		displayMode = dismode;
	else
		displayMode = player_get_display_mode();
	stPlayCtrl.bfullscreen = 0;
	stPlayCtrl.chSelect = 0;
	stPlayCtrl.displaymode = displayMode;
	stPlayCtrl.speed = EN_NORMAL_RATE;
	stPlayCtrl.playdir = EN_PLAY_FORWARD;
	
	g_recTypeMask = RECORD_ALL_TYPE; //第一次进入默认为全部录像
	for(i=0; i<=(IDC_MANUAL_RECORD-IDC_STARTUP_RECORD); i++)
		SendDlgItemMessage(hDlg, IDC_STARTUP_RECORD + i, BM_SETCHECK, BST_CHECKED, 0);
	
	_ui_init_rerplay_ctrl(hDlg);
}

static void refresh_calendar_display(HWND hDlg)
{
	ui_param_free_record_info(&stRecordInfo);	
	_CalendarSetRec(hDlg);
	GetStorageInfoAndDisplay(hDlg);
	SendDlgItemMessage (hDlg, IDC_QUERY_PROPSHEET, PSM_SHEETCMD, ID_CHANGE_DAY, 0);
}

static void CalendarCtrlProc(HWND hDlg, int id ,int code)
{
	
	if (code == MCN_DATECHANGE || code == MCN_DATECLK)
	{
		refresh_calendar_display(hDlg);
	}
}

static void RecInfoCtrlProc(HWND hDlg, int id, int code)
{
	if(RIBN_LBUTTONDBLCLK == code)
	{					
		char buffer [101];
		int nNewPos;
		SendDlgItemMessage(hDlg, id, RIBM_GETINDCPOS, 0L, (LPARAM) &nNewPos);
		VodStart(hDlg,nNewPos);
		SendDlgItemMessage (hDlg, ID_VOD_PLAY, MSG_GETTEXT, 100, (LPARAM)buffer);
		if (strcmp(buffer, LTR("录像播放")) == 0)
			SendDlgItemMessage (hDlg, ID_VOD_PLAY, MSG_SETTEXT, 0, (LPARAM)LTR("回放停止"));
	}		
	else if (RIBN_INDCHANGE == code)
	{
		BarIndcChangedProc(hDlg, id, code);
	}
	else if (RIBN_CUR_MAX_CHANGE == code)
	{
		
		int nOldMin = g_nAxiMinTick;
		int nOldMax = g_nAxiMaxTick;
		SendDlgItemMessage(hDlg, id, RIBM_GET_CUR_MIN, 0L, (LPARAM) &g_nAxiMinTick);
		SendDlgItemMessage(hDlg, id, RIBM_GET_CUR_MAX, 0L, (LPARAM) &g_nAxiMaxTick);
		BarCurMinMaxChanged(hDlg, id, code);

		if(nOldMin != g_nAxiMinTick || nOldMax != g_nAxiMaxTick)
		{
			
			RECT rectAxiLable = {RECINFO_BAR_START_X - 20, RECINFO_BAR_START_Y - 20, 
				RECINFO_BAR_START_X+1260, RECINFO_BAR_START_Y};
			InvalidateRect(hDlg, &rectAxiLable, 1);
		}	
	}
}


static void ReplayFullScreenProc(HWND hDlg)
{	
	push_open_dlg(&headOpenDlg, hDlg);  //记录需要关闭的窗口
	stPlayCtrl.bfullscreen = 1;	
	GUI_ReplaySetDisplayCreen(0);
	ShowAllControls(hDlg, SW_HIDE);
	stPlayCtrl.dlgBkColor = SetWindowBkColor(hDlg, MY_PIXEL_transparent);	
	InvalidateRect(hDlg, NULL, TRUE);
}

static void RecStartStopProc(HWND hDlg)
{
	char buffer [101];
	SendDlgItemMessage (hDlg, ID_VOD_PLAY, MSG_GETTEXT, 100, (LPARAM)buffer);
	if (strcmp(buffer, LTR("录像播放")) == 0)
	{					
		VodStart(hDlg, 0);				
		if (IsTimerInstalled(hDlg, ID_REPLAY_STATUS_TIMER))
		{
			KillTimer(hDlg, ID_REPLAY_STATUS_TIMER);
		}
		//定时器，1秒钟
		SetTimer (hDlg, ID_REPLAY_STATUS_TIMER, 100);					
	}
	else if (strcmp(buffer, LTR("回放停止")) == 0)
	{	
		if (IsTimerInstalled(hDlg, ID_REPLAY_STATUS_TIMER))
		{
			KillTimer(hDlg, ID_REPLAY_STATUS_TIMER);
		}
		VodStop(hDlg);
	}
	InvalidateRect(hDlg, &rectReplayScreen[1], TRUE);
}

static void RecPauseContinueProc(HWND hDlg)
{
	char buffer [101];
	if (stPlayCtrl.status == 0)
		return; 
	SendDlgItemMessage (hDlg, IDC_REPLAY_PAUSE, MSG_GETTEXT, 100, (LPARAM)buffer);
	if (strcmp(buffer, LTR("暂停")) == 0)
	{	/*****规避 在设置播放为慢速的时候，按暂停，加快播放速度清空显示buffer******/	
		if((stPlayCtrl.speed >= EN_SLOW_RATE8) && (stPlayCtrl.speed <= EN_SLOW_RATE2))
			ui_replay_clear_stream(EN_NORMAL_RATE);
		/**************************************************************************/
		VodPause(hDlg);	
		if (IsTimerInstalled(hDlg, ID_REPLAY_STATUS_TIMER))
		{
			KillTimer(hDlg, ID_REPLAY_STATUS_TIMER);
		}
	}
	else if (strcmp(buffer, LTR("播放")) == 0)
	{
		/*****规避 在设置播放为慢速的时候，按暂停，加快播放速度清空显示buffer******/
		if((stPlayCtrl.speed >= EN_SLOW_RATE8) && (stPlayCtrl.speed <= EN_SLOW_RATE2))
			ui_set_speed_dir(stPlayCtrl.speed, -1);//还原回之前的播放速度	
		/**************************************************************************/
		VodPlay(hDlg);
		if (IsTimerInstalled(hDlg, ID_REPLAY_STATUS_TIMER))
		{
			KillTimer(hDlg, ID_REPLAY_STATUS_TIMER);
		}					
		SetTimer (hDlg, ID_REPLAY_STATUS_TIMER, 100);					
	}
	return ;
}


static void ReplayChangeSpeedProc(HWND hDlg, int wParam)
{
	char *str[7]={"慢放三", "慢放二", "慢放一", "正常播放", "快进一", "快进二", "快进三"};
	if (stPlayCtrl.status == 0)	return;
	if (wParam == IDC_SLOW)
	{
		if (stPlayCtrl.speed > EN_SLOW_RATE8)
			stPlayCtrl.speed--;
		else 
			return;
	}
	else
	{		
	#if 0
		char displayMode = player_get_display_mode();
		if ((stPlayCtrl.speed >= EN_NORMAL_RATE)&&(displayMode == 0))
		{
#ifdef OWN_MESSAGEBOX_WIN
			CreateMessageBoxWin(hDlg, LTR("目前只支持单通道快进!"), LTR("消息"), MB_OK);
#else
			MessageBox(hDlg, LTR("目前只支持单通道快进!"), LTR("消息"), MB_OK);
#endif
			return;
		}
	#endif
		if (stPlayCtrl.speed < EN_SPEED_RATE8)
			stPlayCtrl.speed++;
		else
			return;
	}
	SetDlgItemText(hDlg, IDC_STATIC_REPLAY_STATE,LTR(str[stPlayCtrl.speed]));	
	ui_set_speed_dir(stPlayCtrl.speed, -1);
}

static void RecordInfoZoomProc(HWND hDlg, int ID_CTRL)
{
	int i = 0;
	RECT rectAxiLable = {RECINFO_BAR_START_X - 20, RECINFO_BAR_START_Y - 20, 
		  RECINFO_BAR_START_X+1260, RECINFO_BAR_START_Y};
	int zoomArray[4] = {ZOOM_OUT_X0, ZOOM_OUT_X1_12, ZOOM_OUT_X1_24, ZOOM_OUT_X1_48};
	SendDlgItemMessage(hDlg, IDC_CH1, RIBM_GET_CUR_MIN, 0L, (LPARAM) &g_nAxiMinTick);
	SendDlgItemMessage(hDlg, IDC_CH1, RIBM_GET_CUR_MAX, 0L, (LPARAM) &g_nAxiMaxTick); 	  
	if (ID_CTRL == IDC_24HOUR)
	{   
	  g_nAxiMinTick = 0;
	  g_nAxiMaxTick = (24*3600);
	}
	else if (ID_CTRL == IDC_2HOUR)
	{   
//	  g_nAxiMinTick = (stPlayCtrl.indicatorTime/7200)*7200;
	  g_nAxiMinTick = (stPlayCtrl.indicatorTime/1800)*1800;
	  g_nAxiMaxTick = g_nAxiMinTick+7200;
	}
	else if (ID_CTRL == IDC_1HOUR)
	{   
//	  g_nAxiMinTick = (stPlayCtrl.indicatorTime/3600)*3600;
	  g_nAxiMinTick = (stPlayCtrl.indicatorTime/900)*900;
	  g_nAxiMaxTick = g_nAxiMinTick+3600;
	}
	else if (ID_CTRL == IDC_30MIN)
	{   
//	  g_nAxiMinTick = (stPlayCtrl.indicatorTime/1800)*1800;
	  g_nAxiMinTick = (stPlayCtrl.indicatorTime/450)*450;
	  g_nAxiMaxTick = g_nAxiMinTick+1800;
	} 			  

	for (i = 0 ; i < 4; i++)
	{
	  SendDlgItemMessage(hDlg, IDC_CH1+i, RIBM_SET_ZOOMOUT_PARAM, zoomArray[ID_CTRL - IDC_24HOUR] , 0);			  
	  SendDlgItemMessage(hDlg, IDC_CH1+i, RIBM_SET_CUR_MINMAX, (WPARAM) g_nAxiMinTick, (LPARAM) g_nAxiMaxTick);
	}
	/////////////////////////////////
	InvalidateRect(hDlg, &rectAxiLable, 1);
}



static void ReplayCommandProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int id   = LOWORD(wParam);
    int code = HIWORD(wParam);
	if (id == IDC_VOD_MC)
	{
		CalendarCtrlProc(hDlg, id, code);
		return;
	}
	else if (id >= IDC_CH1 && id <= IDC_CH4)
	{	
		RecInfoCtrlProc(hDlg, id, code);
		return;
	}

	switch (wParam) 
	{
		case IDC_FULL_SCREEN:
		{
			ReplayFullScreenProc(hDlg);
		}
		break;
		
		case ID_VOD_PLAY:
		  	RecStartStopProc(hDlg);
		break;
		
		case IDC_REPLAY_PAUSE:
			RecPauseContinueProc(hDlg);
		break;
		
		case IDC_SLOW:
		case IDC_SPEED:
			ReplayChangeSpeedProc(hDlg, wParam);
		break;
		
		case IDC_VOD_BACKUP:
			CreateStorageBackupDlgWin(hDlg, stRecordInfo.tm_start);
		break;
		  
		case IDC_24HOUR:
		case IDC_2HOUR:
		case IDC_1HOUR:			  
		case IDC_30MIN:
			RecordInfoZoomProc(hDlg, wParam);
		break;
		default:
		  break;
		}
}


static void ReplayLButtonDBclickProc(HWND hDlg, DWORD lParam)
{
	int x_pos = LOSWORD (lParam);
	int y_pos = HISWORD (lParam);

	if (stPlayCtrl.bfullscreen)
	{		
		if (PtInRect(&rectReplayScreen[0],x_pos,y_pos))
		{
			RefreshFrameClip(hDlg);
			ReplayUpdateSelectCh(x_pos, y_pos);
			RepaySwitchDisplayMode(hDlg);
			RefreshFrameClip(hDlg);
			if(stPlayCtrl.displaymode)
			{
				RecoverBeforePlayState(RECORD_PLAY);
			}
		}
	}
	else
	{
		if (PtInRect(&rectReplayScreen[1],x_pos,y_pos))
		{
			RefreshFrameClip(hDlg);
			ReplayUpdateSelectCh(x_pos, y_pos);
			RepaySwitchDisplayMode(hDlg);
			RefreshFrameClip(hDlg);
		}
	}
}


static void ReplayLButtonDownProc(HWND hDlg, DWORD lParam)
{
	int x_pos = LOSWORD (lParam);
	int y_pos = HISWORD (lParam);
	if (stPlayCtrl.bfullscreen)
	{
		if (PtInRect(&rectReplayScreen[0],x_pos,y_pos))
		{
			ReplayUpdateSelectCh(x_pos, y_pos);		
			if (stPlayCtrl.displaymode == 0)
			{
				RefreshFrameClip(hDlg);
			}
		}
	}
	else
	{
		if (PtInRect(&rectReplayScreen[1],x_pos,y_pos))
		{
			ReplayUpdateSelectCh(x_pos, y_pos);		
			if (stPlayCtrl.displaymode == 0)
			{
				RefreshFrameClip(hDlg);
			}
		}
	}
}


static void _RestoreReplay(HWND hDlg)
{	
	HWND hWind;
	pop_open_dlg(&headOpenDlg, &hWind);//记录次窗口需要发送关闭消息的次数

	if(stPlayCtrl.bfullscreen && stPlayCtrl.displaymode)
	{
		RecoverBeforePlayState(RECORD_PLAY);
	}

	stPlayCtrl.bfullscreen = 0;	
	GUI_ReplaySetDisplayCreen(1);
	ShowAllControls(hDlg, SW_SHOW);
	SetWindowBkColor(hDlg, stPlayCtrl.dlgBkColor);	
	_InitplayerWindow(hDlg);	
	InvalidateRect(hDlg, NULL, TRUE);
}

static void DisplayChannelNum(HDC hdc)
{
	PLOGFONT temp_font;
	PLOGFONT caption_font;
	int i;
	const char *strCh[4]={"通道一", "通道二", "通道三", "通道四"};

	SetBkMode(hdc, BM_TRANSPARENT);//文本背景模式
	temp_font = GetCurFont(hdc);
	//caption_font = GetSystemFont(SYSLOGFONT_CONTROL);
	//SelectFont(hdc,caption_font);
	SetTextColor(hdc, PIXEL_lightwhite);	
	
	if(stPlayCtrl.bfullscreen) //整屏
	{
		caption_font = GetSystemFont(SYSLOGFONT_MENU);
		SelectFont(hdc,caption_font);
		
		if(stPlayCtrl.displaymode)//全屏
			DrawText(hdc, LTR(strCh[stPlayCtrl.chSelect]), -1, &chTextRect[stPlayCtrl.bfullscreen][4], DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		else  // 4分屏
		{
			for(i=0; i<NVR_MAX_CHANNEL_NUM; i++)
			{
				DrawText(hdc, LTR(strCh[i]), -1, &chTextRect[stPlayCtrl.bfullscreen][i], DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			}
		}
	}
	else //非整屏
	{
		caption_font = GetSystemFont(SYSLOGFONT_CONTROL);
		SelectFont(hdc,caption_font);
	
		if(stPlayCtrl.displaymode)//全屏
			DrawText(hdc, LTR(strCh[stPlayCtrl.chSelect]), -1, &chTextRect[stPlayCtrl.bfullscreen][4], DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		else  // 4分屏
		{
			for(i=0; i<NVR_MAX_CHANNEL_NUM; i++)
			{
				DrawText(hdc, LTR(strCh[i]), -1, &chTextRect[stPlayCtrl.bfullscreen][i], DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			}
		}
	}
	
	SelectFont(hdc,temp_font);
}

static void ReplayPaintProc(HDC hdc)
{
	if (!stPlayCtrl.bfullscreen)
	{
		DrawAxisLabelAndTime(hdc);
	}			
	DrawReplayChRect(hdc, stPlayCtrl.displaymode);
	DisplayChannelNum(hdc);
}

static int GetRecTypeMask(HWND hDlg, int *mask)
{
	int i;
	unsigned int checked;
	
	if(mask == NULL)
		return -1;

	*mask = 0;
	for (i = 0; i <= (IDC_MANUAL_RECORD-IDC_STARTUP_RECORD); i++)
	{
		checked = SendDlgItemMessage(hDlg, IDC_STARTUP_RECORD+i, BM_GETCHECK, 0, 0);
		if (checked != BST_CHECKED) continue;
		else
		{
			*mask |= (0x01<<i);
		}
	}

	return 0;
}

//stPlayCtrl
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
static int ReplayWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	
    switch (message) 
	{
    case MSG_INITDIALOG:		
		GUI_ReplaySetDisplayCreen(1);
		ReplayInitPlayCtrl(hDlg, lParam);
		if (!IsTimerInstalled(hDlg, ID_DISPLAY_CHAN_TIMER))
			SetTimer (hDlg, ID_DISPLAY_CHAN_TIMER, 200);	
		push_open_dlg(&headOpenDlg, hDlg);
    	return 0;

	case MSG_PAINT:
	{
		hdc = BeginPaint (hDlg);
		ReplayPaintProc(hdc);
        EndPaint (hDlg, hdc);
	}
	return 0;
	case MSG_ERASEBKGND:
	{
		if (stPlayCtrl.bfullscreen)
			break;
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
		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bk_pic[SIZE_1280x720]); 			
		if (fGetDC) 
			ReleaseDC (hdc); 
		return 0; 
	}
    case MSG_COMMAND:
	{
		int id = LOWORD(wParam);
		//int code = HIWORD(wParam);

		if((id >= IDC_STARTUP_RECORD) && (id <= IDC_MANUAL_RECORD))
		{
			GetRecTypeMask(hDlg, &g_recTypeMask);
			refresh_calendar_display(hDlg);
		}
		else
		{
			ReplayCommandProc(hDlg, message, wParam, lParam);
		}
	}
	return 0;
	case MSG_TIMER:
	{
		switch(wParam)
		{
			case ID_REPLAY_STATUS_TIMER:
			{
				ReplayTimerProc(hDlg);
				return 0;
			}
			break;

			case ID_DISPLAY_CHAN_TIMER:
			{
				DisplayTimerProc(hDlg);
			}
			break;
		}
		break;
	}
	case MSG_MOUSE_SCROLL:
	{
		int mouseX = LOSWORD (lParam);
		int mouseY = HISWORD (lParam);
				
		if(stPlayCtrl.bfullscreen && stPlayCtrl.displaymode)
		{
			if(0 == playSrc[RECORD_PLAY].initFlag)
			{
				InitPlaySrc(RECORD_PLAY);
				playSrc[RECORD_PLAY].initFlag = 1;
			}
			ZoomPlaySrc(RECORD_PLAY, (int)wParam, mouseX, mouseY); //支持缩放功能
		}

        break;
	}
	break;
	case MSG_LBUTTONDBLCLK:
	{
		ReplayLButtonDBclickProc(hDlg, lParam);
	}
	break;	
	case MSG_LBUTTONDOWN:
	{
		ReplayLButtonDownProc(hDlg, lParam);
	}
	break;
	case MSG_CLOSE:
	{
    	HWND hWind;
    	
		if(IsTimerInstalled(hDlg, ID_DISPLAY_CHAN_TIMER))
			KillTimer(hDlg, ID_DISPLAY_CHAN_TIMER);
			
		if (listboxinitok == 0) 
			return 0;
		
		if (stPlayCtrl.bfullscreen)
		{		
			_RestoreReplay(hDlg);
			return 0;
		}

		if (stPlayCtrl.status == 0)
		{
			ui_param_free_record_info(&stRecordInfo);
			player_set_dis_mode(0, stPlayCtrl.chSelect);
			player_set_real_play_mode(4);
			GUI_ReplaySetDisplayCreen(0);
			SendNotifyMessage (GetMainWin(), MSG_REPLAY_CLOSE,  0, 0);
			stPlayCtrl.chSelect = -1;
			stPlayCtrl.displaymode = -1;
			listboxinitok = 0;

			pop_open_dlg(&headOpenDlg, &hWind);
			
			EndDialog (hDlg, IDCANCEL);
		}
		else
		{			
			VodStop(hDlg);
		}
	}
	return 0;
	case MSG_DESTROY:
        DestroyAllControls (hDlg);
        return 0;
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static void AdjustPositionDlgWin()
{
	int index,i;

	GetDisplayLanguageIndex(&index);
	if(1 == index)
	{
		VodCtrl[1].w = THREE_WORDS_BUTTON_W;
		VodCtrl[1].dwAddData = (DWORD)&button_pic[1];
		VodCtrl[3].w = LONG_WORDS_BUTTON_W;
		VodCtrl[3].dwAddData = (DWORD)&button_pic[3];

		for(i=4; i<6; i++)
		{
			VodCtrl[i].w = THREE_WORDS_BUTTON_W;
			VodCtrl[i].dwAddData = (DWORD)&button_pic[1];
		}
		VodCtrl[11].dwStyle = (VodCtrl[11].dwStyle & (~MCS_ENG_S))| MCS_ENG_L;
	}
}

void CreateReplayDlgWin(HWND hwnd, int dis_mode, int real_ch)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "VodDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(VodCtrl));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, VodCtrl, sizeof(VodCtrl));

	DLG_CAPTION_LANGUAGE_TRANS(VodCtrl)
	AdjustPositionDlgWin();
    DlgVod.controls = VodCtrl;
    DialogBoxIndirectParam (&DlgVod, hwnd, ReplayWinProc, dis_mode);
	memcpy(VodCtrl, ctrolTmp, sizeof(VodCtrl));
	free(ctrolTmp);
	ctrolTmp = NULL;
}


