#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>
#include "ui_config.h"
#include "gui_core.h"

#include "ui_dlg_head.h"
#include "hzinput.h"
#include "ctrl_toolbar.h"

#define TI_NVR

//#define MAINMENU_USE_CHN_CTRL

#ifdef TI_NVR
#include "storage_interface.h"
#include "player_interface.h"
#include "ipnc_lib.h"
#include "nvr_lib_interface.h"
#include "storage_lib.h"
#include "av_pool.h"
#include "storage_ext_lib.h"
#include "log.h"
#include "syscfg.h"
#endif
typedef enum
{
	ID_LIST_1_CHANNEL = 1000,
	ID_LIST_4_CHANNEL,
	ID_LIST_PTZ_CTRL,
	ID_LIST_VEDIO_COLOR,
	ID_LIST_IPC_OSD,
	ID_LIST_VOD_QUERY,
	ID_LIST_VOD_CTRL,
	ID_LIST_REMOTE_DEV,
	ID_LIST_WIZARD,
	ID_LIST_ALARM_OUT,
	ID_LIST_MAIN_MENU,
	ID_LIST_LOCK_SCREEN,
	ID_LIST_VEDIO_ENCODE,
	ID_LIST_COLORKEY,
	IDM_4CH1,
	IDM_4CH2,
	IDM_4CH3,
	IDM_4CH4,
	IDM_CH1TO4,
	IDC_MAIN_PLAYSTOP,
	IDC_MAIN_MANNUAL_RECORD,
	IDC_MAIN_CAPUTURE,
	IDC_MAIN_SOUND_OPENCLOSE,
	IDC_MAIN_TAKL_OPENCLOSE,
	IDC_TOOLBAR_CH1,	
	IDC_TOOLBAR_CH2,
	IDC_TOOLBAR_CH3,
	IDC_TOOLBAR_CH4,
	SYS_TIME_TIMER,  //timer id
}
UI_MAIN_ID;

#define TB_BEGIN_X             2
#define TB_BEGIN_Y             2
#define TB_HEIGHT              22
#define TB_WIDTH               22

#define ALARMICON_W				25
#define ALARMICON_H				25
#define RECORDICON_W			25
#define RECORDICON_H			25
#define OFF_SET_BUTTON			4

#define ZOOM_SPEEDX            (NVR_SCREEN_WIDTH/20)  // 1/20倍缩放
#define ZOOM_SPEEDY			   (NVR_SCREEN_HEIGHT/20) // 1/20倍缩放

#define NOSIG_WIDTH            100

static BITMAP bmpAlarm;
static BITMAP bmpRecord;
RECT recordRect[] = 
{
	{40, NVR_SCREEN_HEIGHT/2-(RECORDICON_H+OFF_SET_BUTTON) , 40+RECORDICON_H, NVR_SCREEN_HEIGHT/2-OFF_SET_BUTTON},//ch1 显示位置
	{NVR_SCREEN_WIDTH/2+40, NVR_SCREEN_HEIGHT/2-(RECORDICON_H+OFF_SET_BUTTON), NVR_SCREEN_WIDTH/2+40+RECORDICON_H, NVR_SCREEN_HEIGHT/2-OFF_SET_BUTTON},//ch2 显示位置
	{40, NVR_SCREEN_HEIGHT -(RECORDICON_H+OFF_SET_BUTTON), 40+RECORDICON_H, NVR_SCREEN_HEIGHT -OFF_SET_BUTTON},//ch3 显示位置
	{NVR_SCREEN_WIDTH/2+40, NVR_SCREEN_HEIGHT -(RECORDICON_H+OFF_SET_BUTTON), NVR_SCREEN_WIDTH/2 + 40+RECORDICON_H, NVR_SCREEN_HEIGHT -OFF_SET_BUTTON},//ch4 显示位置
	{40, NVR_SCREEN_HEIGHT -(RECORDICON_H+OFF_SET_BUTTON), 40+RECORDICON_H, NVR_SCREEN_HEIGHT -OFF_SET_BUTTON}//满屏时显示位置
};

RECT playRect[] = 
{
	{NVR_SCREEN_WIDTH/4-NOSIG_WIDTH/2, NVR_SCREEN_HEIGHT/4-NOSIG_WIDTH/2, NVR_SCREEN_WIDTH/4+NOSIG_WIDTH/2, NVR_SCREEN_HEIGHT/4+NOSIG_WIDTH/2},//ch1 显示位置
	{(NVR_SCREEN_WIDTH/4)*3-NOSIG_WIDTH/2, NVR_SCREEN_HEIGHT/4-NOSIG_WIDTH/2, (NVR_SCREEN_WIDTH/4)*3+NOSIG_WIDTH/2,NVR_SCREEN_HEIGHT/4+NOSIG_WIDTH/2},//ch2 显示位置
	{NVR_SCREEN_WIDTH/4-NOSIG_WIDTH/2, (NVR_SCREEN_HEIGHT/4)*3-NOSIG_WIDTH/2, NVR_SCREEN_WIDTH/4+NOSIG_WIDTH/2, (NVR_SCREEN_HEIGHT/4)*3+NOSIG_WIDTH/2},//ch3 显示位置
	{(NVR_SCREEN_WIDTH/4)*3-NOSIG_WIDTH/2, (NVR_SCREEN_HEIGHT/4)*3-NOSIG_WIDTH/2, (NVR_SCREEN_WIDTH/4)*3+NOSIG_WIDTH/2, (NVR_SCREEN_HEIGHT/4)*3+NOSIG_WIDTH/2},//ch4 显示位置
	{NVR_SCREEN_WIDTH/2-NOSIG_WIDTH/2,NVR_SCREEN_HEIGHT/2-NOSIG_WIDTH/2,NVR_SCREEN_WIDTH/2+NOSIG_WIDTH/2,NVR_SCREEN_HEIGHT/2+NOSIG_WIDTH/2},//满屏时显示位置
};

RECT alarmRect[] = 
{
	{65, NVR_SCREEN_HEIGHT/2-(ALARMICON_H+OFF_SET_BUTTON), 65+ALARMICON_H, NVR_SCREEN_HEIGHT/2-OFF_SET_BUTTON},//ch1 显示位置
	{NVR_SCREEN_WIDTH/2+65, NVR_SCREEN_HEIGHT/2-(ALARMICON_H+OFF_SET_BUTTON), NVR_SCREEN_WIDTH/2+65+ALARMICON_H, NVR_SCREEN_HEIGHT/2-OFF_SET_BUTTON},//ch2 显示位置
	{65, NVR_SCREEN_HEIGHT - (ALARMICON_H+OFF_SET_BUTTON), 65+ALARMICON_H, NVR_SCREEN_HEIGHT-OFF_SET_BUTTON},//ch3 显示位置
	{NVR_SCREEN_WIDTH/2+65, NVR_SCREEN_HEIGHT - (ALARMICON_H+OFF_SET_BUTTON), NVR_SCREEN_WIDTH/2+65+ALARMICON_H, NVR_SCREEN_HEIGHT -OFF_SET_BUTTON},//ch4 显示位置
	{65, NVR_SCREEN_HEIGHT - (ALARMICON_H+OFF_SET_BUTTON), 65+ALARMICON_H, NVR_SCREEN_HEIGHT -OFF_SET_BUTTON},//满屏时显示位置
};

RECT textRect[]=
{
	{0, NVR_SCREEN_HEIGHT/2 - 40, NVR_SCREEN_WIDTH/2 -1, NVR_SCREEN_HEIGHT/2},//ch1 显示位置
	{NVR_SCREEN_WIDTH/2, NVR_SCREEN_HEIGHT/2 - 40, NVR_SCREEN_WIDTH -1, NVR_SCREEN_HEIGHT/2},//ch2 显示位置
	{0, NVR_SCREEN_HEIGHT - 40, NVR_SCREEN_WIDTH/2 -1, NVR_SCREEN_HEIGHT},//ch3 显示位置
	{NVR_SCREEN_WIDTH/2, NVR_SCREEN_HEIGHT - 40, NVR_SCREEN_WIDTH -1, NVR_SCREEN_HEIGHT},//ch4 显示位置
	{0, 720 - 40, 1280-1, 720},//满屏时显示位置
};

RECT TimeScreen = {500, 0, 780, 40};
RECT UpdateScreen = {500, 40, 780, 80};
RECT FormatScreen = {500, 720 - 40, 780, 720};


static HWND hMainWnd;
static HWND hIME;
typedef struct __MainListInfo
{
	char *name;
	int	 id;
}MainListInfo_t;

enum
{
	REAL_STATE_STOP = 0,
	REAL_STATE_PLAY,
};

typedef struct tag_RealPlayerCtrl_t
{
	int displaymode; //0四分屏,1全屏
	int chSelect; 	//全屏
	int Realstate[4];
}RealPlayerCtrl_t;

typedef struct tag_WizardCtrl
{
	int needWizard;
	int wizardOk;
}WizardCtrl_t;

extern RECT rectReplayScreen[2];
static MSGHOOK	old_mouse_hook;
static RealPlayerCtrl_t g_stRealCtrl = {-1, -1};
static WizardCtrl_t wizardCtrl = {0, 0};


static int res_init_ok = 0;
static loginInfo_t g_stLogInfo;

#if defined(HIDE_CURSER) || defined(LOCK_SCREEN)		
static int		curShow = 0;
unsigned old_tick_count = 0;
unsigned is_mouse_down = 0;
#endif

MainListInfo_t mainListInfo[] = 
{
{"主菜单",		ID_LIST_MAIN_MENU},
#ifdef MAINMENU_USE_CHN_CTRL
{"单通道",		ID_LIST_1_CHANNEL},
{"四通道",		ID_LIST_4_CHANNEL},
#endif
{"编码参数",    ID_LIST_VEDIO_ENCODE},
{"图像参数",	ID_LIST_VEDIO_COLOR},
{"OSD 参数",	ID_LIST_IPC_OSD},
{"录像回放",	ID_LIST_VOD_QUERY},
{"录像控制",	ID_LIST_VOD_CTRL},
{"云台控制",	ID_LIST_PTZ_CTRL},
{"远程设备",	ID_LIST_REMOTE_DEV},	
{"设置向导",	ID_LIST_WIZARD},
#if 0	
{"报警输出    ",	ID_LIST_ALARM_OUT},
#endif	
{"锁屏",		ID_LIST_LOCK_SCREEN},
#ifdef OPEN_COLOKEY_DLG
{"透明设置",		ID_LIST_COLORKEY}
#endif
};



plink_opendlg headOpenDlg;


ZOOM_PLAYERSRC playSrc[2] = {{0}, {0}};
static HMENU hRightMenu = 0;
extern BOOL gui_get_disk_status(void);
static int SystemIsNfsBoot();

void InitPlaySrc(int mode)
{
	playSrc[mode].x = 0;
	playSrc[mode].y = 0;
	playSrc[mode].w = NVR_SCREEN_WIDTH;
	playSrc[mode].h = NVR_SCREEN_HEIGHT;
	playSrc[mode].zoomWidth = 0;
	playSrc[mode].zoomHeight = 0;
}

void ZoomPlaySrc(PLAY_MODE mode, int dir, int mouseX, int mouseY)
{

	if(1 == dir)//向上滑动放大
	{
		if((playSrc[mode].w >= ZOOM_SPEEDX) && (playSrc[mode].h >= ZOOM_SPEEDY) && (playSrc[mode].x < NVR_SCREEN_WIDTH) && 
		(playSrc[mode].y < NVR_SCREEN_HEIGHT))
		{
			playSrc[mode].x += ZOOM_SPEEDX*mouseX/NVR_SCREEN_WIDTH;
			playSrc[mode].w -= ZOOM_SPEEDX;
			playSrc[mode].y += ZOOM_SPEEDY*mouseY/NVR_SCREEN_HEIGHT;
			playSrc[mode].h -= ZOOM_SPEEDY;

			playSrc[mode].zoomWidth += ZOOM_SPEEDX;
			playSrc[mode].zoomHeight += ZOOM_SPEEDY;
			player_set_src_rsz(playSrc[mode].x, playSrc[mode].y, playSrc[mode].w, playSrc[mode].h);	
		}
	}
	else if(2 == dir)//向下滑动缩小 
	{
		//printf("####################playSrc[mode].x: %d, y: %d,w: %d, h: %d,playSrc[mode].zoomWidth: %d,playSrc[mode].zoomHeight: %d\r\n",
		//playSrc[mode].x,playSrc[mode].y,playSrc[mode].w, playSrc[mode].h, playSrc[mode].zoomWidth,playSrc[mode].zoomHeight);
		if((playSrc[mode].x >= 0) && (playSrc[mode].y >= 0) && (playSrc[mode].w <= NVR_SCREEN_WIDTH) && (playSrc[mode].h <= NVR_SCREEN_HEIGHT)&&
		playSrc[mode].zoomWidth > 0 && playSrc[mode].zoomHeight > 0)
		{
			if((playSrc[mode].x - ZOOM_SPEEDX*playSrc[mode].x/playSrc[mode].zoomWidth >= 0) || 
			(playSrc[mode].y - ZOOM_SPEEDY*playSrc[mode].y/playSrc[mode].zoomHeight >= 0))
			{
				playSrc[mode].x -=  ZOOM_SPEEDX*playSrc[mode].x/playSrc[mode].zoomWidth;
				if(playSrc[mode].x < 0) playSrc[mode].x = 0;
				playSrc[mode].y -=  ZOOM_SPEEDY*playSrc[mode].y/playSrc[mode].zoomHeight;
				if(playSrc[mode].y < 0) playSrc[mode].y = 0;
				
				playSrc[mode].zoomWidth -= ZOOM_SPEEDX;
				playSrc[mode].zoomHeight -= ZOOM_SPEEDY;
				playSrc[mode].w +=  ZOOM_SPEEDX;
				if(playSrc[mode].w >= NVR_SCREEN_WIDTH) playSrc[mode].w = NVR_SCREEN_WIDTH;

				playSrc[mode].h +=  ZOOM_SPEEDY;
				if(playSrc[mode].h >= NVR_SCREEN_HEIGHT) playSrc[mode].h = NVR_SCREEN_HEIGHT;
			}
			else
			{
				InitPlaySrc(mode);
			}
			
			player_set_src_rsz(playSrc[mode].x, playSrc[mode].y, playSrc[mode].w, playSrc[mode].h);
		}
	}

	return ;
}

void RecoverBeforePlayState(PLAY_MODE mode)
{
	playSrc[mode].initFlag = 0; //退出全屏播放电子放大状态，恢复到正常播放状态
	InitPlaySrc(mode);
	player_set_src_rsz(playSrc[mode].x, playSrc[mode].y, playSrc[mode].w, playSrc[mode].h);	
}

HWND GetMainWin()
{
	return hMainWnd;
}

int UIGetLogin(loginInfo_t * loginData)
{
	if(NULL == loginData)
		return -1;

	loginData->login = g_stLogInfo.login;
	strncpy(loginData->acUser, g_stLogInfo.acUser, sizeof(g_stLogInfo.acUser)-1);
	return 0;
}

int UISetLogin(char *strUser)
{
	g_stLogInfo.login = 1;
	strncpy(g_stLogInfo.acUser , strUser, sizeof(g_stLogInfo.acUser) - 1);
	log_write(LOG_INFO, USER_LOG, "用户%s登陆", strUser);
	return 0;
}

int UISetUnLogin()
{
	if(g_stLogInfo.login == 0)
		return 0;
	g_stLogInfo.login = 0;	
	log_write(LOG_INFO, USER_LOG, "用户%s退出登陆", g_stLogInfo.acUser);
	return 0;
}

int UIBeLogin()
{
	return g_stLogInfo.login;
}

int NeedWizard()
{
	return wizardCtrl.needWizard;
}

//TOOLBAR
/////////////////////////////////////////////////////////////////////////////////////////
enum
{
	EN_TOOLBAR_PLAY = 0,
	EN_TOOLBAR_MANUAL_RECORD,
	EN_TOOLBAR_CAPTURE,
	EN_TOOLBAR_SOUND,
	EN_TOOLBAR_TALK,
	EN_TOOLBAR_ITEM_NUM
};

static struct toolbar_items
{
    int   id;
	int   init_opt;
    char *play_name;
	char *stop_name;
} toolbar_items [] =
{
	{IDC_MAIN_PLAYSTOP, 		0, "nvr_res/realplay_play.bmp", "nvr_res/realplay_stop.bmp"},
	{IDC_MAIN_MANNUAL_RECORD, 	1, "nvr_res/manual_record_open.bmp", "nvr_res/manual_record_close.bmp"},
	{IDC_MAIN_CAPUTURE, 		0, "nvr_res/capture.bmp", NULL},
	{IDC_MAIN_SOUND_OPENCLOSE, 	1, "nvr_res/sound_open.bmp", "nvr_res/sound_close.bmp"},
	{IDC_MAIN_TAKL_OPENCLOSE, 	1, "nvr_res/talk_open.bmp", "nvr_res/talk_close.bmp"},
};

typedef struct __item_bmp
{
	BITMAP	*open_bmp;
	BITMAP	*close_bmp;
	BITMAP	*origina_bmp; //用于保存原item的bmp数据
}item_bmp_t;



static item_bmp_t item_bmp[EN_TOOLBAR_ITEM_NUM] = {{NULL, NULL, NULL}};
static HWND hToolbarMain[NVR_MAX_CHANNEL_NUM] = {HWND_INVALID};


void init_open_dlg(plink_opendlg *head)
{
	*head = NULL;
}

BOOL empty_open_dlg(plink_opendlg head)
{
	return (NULL == head) ? TRUE: FALSE;
}

BOOL push_open_dlg(plink_opendlg *head, HWND hDlg)
{
	plink_opendlg pTemp = (plink_opendlg)malloc(sizeof(link_opendlg));
	if(NULL == pTemp)
		return FALSE;

	pTemp->openWind = hDlg;
	pTemp->next = *head;
	*head = pTemp;

	return TRUE;
}
BOOL pop_open_dlg(plink_opendlg *head, HWND *hDlg)
{
	plink_opendlg pTemp; 
	
	if(*head == NULL)
		return FALSE;

	pTemp = *head;
	*head = pTemp->next;
	*hDlg = pTemp->openWind;
	free(pTemp);
	pTemp = NULL;
	
	return TRUE;
}

void uninit_open_dlg(plink_opendlg *head)
{
	HWND hDlg;
	while(!empty_open_dlg(*head))
	{
		pop_open_dlg(head, &hDlg);
	}

	return;
}

void show_open_dlg(plink_opendlg head)
{
	int count = 0;
	while(head)
	{
		printf("################dlg%d: %u\r\n",count++, head->openWind);
		head = head->next;
	}
}

static int _init_toolbar_res()
{
	int item = 0;
	for (item = 0; item < EN_TOOLBAR_ITEM_NUM; item++)
	{
		item_bmp[item].origina_bmp = NULL;
		
		if (toolbar_items[item].play_name != NULL)
			item_bmp[item].open_bmp = (BITMAP *)malloc(sizeof(BITMAP));
		if (toolbar_items[item].stop_name != NULL)
			item_bmp[item].close_bmp = (BITMAP *)malloc(sizeof(BITMAP));
		
		if (item_bmp[item].open_bmp != NULL)
			LoadBitmapFromFile(HDC_SCREEN , item_bmp[item].open_bmp, toolbar_items[item].play_name);
		if (item_bmp[item].close_bmp != NULL)
			LoadBitmapFromFile(HDC_SCREEN , item_bmp[item].close_bmp, toolbar_items[item].stop_name);		
	}
	return 0;
}

static void _uninit_toolbar_res()
{
	int item = 0;
	for (item = 0; item < EN_TOOLBAR_ITEM_NUM; item++)
	{
		if (item_bmp[item].open_bmp != NULL)
		{
			free(item_bmp[item].open_bmp);
			item_bmp[item].open_bmp = NULL;
		}
		
		if (item_bmp[item].close_bmp != NULL)
		{
			free(item_bmp[item].close_bmp);
			item_bmp[item].close_bmp = NULL;
		}

		
		if (item_bmp[item].origina_bmp != NULL)
		{
			free(item_bmp[item].origina_bmp);
			item_bmp[item].origina_bmp = NULL;
		}
		
	}
}


static void InitToolbarItem (HWND toolbar)
{
    int i;
    TOOLBARITEMINFO pData;
	
    for (i = 0; i < TABLESIZE (toolbar_items); i++) {
        pData.id = toolbar_items [i].id;
        pData.insPos = i + 1;
		if (toolbar_items [i].init_opt == 0)
			sprintf (pData.NBmpPath, "%s", toolbar_items[i].play_name);
		else	
			sprintf (pData.NBmpPath, "%s", toolbar_items[i].stop_name);
        SendMessage (toolbar, TBM_ADDITEM, 0, (LPARAM)&pData);
    }	
}


static void InitToolbar(HWND hWnd)
{
	int ret = 0;
	int chn = 0;
	DWORD WdHt = MAKELONG (TB_HEIGHT, TB_WIDTH);
	
	ret = _init_toolbar_res();
	if (ret == -1)
		return;

	for (chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++)
	{
		hToolbarMain[chn] = CreateWindow (CTRL_TOOLBAR_EX, " ",
							WS_CHILD /*| WS_VISIBLE*/ , IDC_TOOLBAR_CH1+chn,
							TB_BEGIN_X, TB_BEGIN_Y, HIWORD(WdHt)*TABLESIZE (toolbar_items),
							LOWORD(WdHt), hWnd, WdHt);		
		InitToolbarItem (hToolbarMain[chn]);				
		SendMessage(hToolbarMain[chn], TBM_ITEMENABLE, IDC_MAIN_PLAYSTOP, FALSE);
		//SendMessage(hToolbarMain[chn], TBM_ITEMENABLE, IDC_MAIN_MANNUAL_RECORD, FALSE);
		//SendMessage(hToolbarMain[chn], TBM_ITEMENABLE, IDC_MAIN_CAPUTURE, FALSE);
		SendMessage(hToolbarMain[chn], TBM_ITEMENABLE, IDC_MAIN_TAKL_OPENCLOSE, FALSE);
	}
	return;
	
}

static void UninitToolbar(HWND hWnd)
{
	_uninit_toolbar_res();
}


static void ToolBarCommandProc(HWND hWnd, int id)
{
	int oldbmp = 0;
	static int audioOpen[4] = {0};
	static int manualRec[4] = {0};
	int chn = g_stRealCtrl.chSelect;
	
	if (hToolbarMain[chn] == HWND_INVALID) return;
	
	if (id == IDC_MAIN_PLAYSTOP)
	{
		//开启关闭实时流		
		//int ret, open;
		//open = !playOpen[chn];
		//if (open)
		//	ret = player_real_play(chn);
		//else
		//	ret = player_stop(chn);
		//if (ret == -1 ) return;
		
		//playOpen[chn] = open;
		//oldbmp = SendMessage(hToolbarMain[chn], TBM_SETIMAGE, (WPARAM)id, (LPARAM)bmpPlay[chn] );
		//if (oldbmp == 0) return;
		
		//bmpPlay[chn]  = (BITMAP *)oldbmp;
		//SendMessage(hToolbarMain[chn], TBM_ITEMENABLE, IDC_MAIN_PLAYSTOP, enable);
	}
	else if (id == IDC_MAIN_TAKL_OPENCLOSE)
	{
		//对讲
		//oldbmp = SendMessage(hToolbarMain[chn],TBM_SETIMAGE, (WPARAM)id, (LPARAM)bmpTalk[chn]);
		//if (oldbmp == 0) return;
		
		//bmpTalk[chn] = (BITMAP *)oldbmp;
		//SendMessage(hToolbarMain[chn], TBM_ITEMENABLE, IDC_MAIN_TAKL_OPENCLOSE, enable);
	}
	else if (id == IDC_MAIN_MANNUAL_RECORD)
	{		
		static  int orignalbmp_saved = 0;
		//手动录像
		int openRec;
		openRec = !manualRec[chn];

		if(openRec)
		{
			nvr_lib_start_manual_record(chn);
			oldbmp = SendMessage(hToolbarMain[chn],TBM_SETIMAGE, (WPARAM)id, (LPARAM)item_bmp[EN_TOOLBAR_MANUAL_RECORD].open_bmp);
		}
		else
		{
			nvr_lib_stop_manual_record(chn);
			oldbmp = SendMessage(hToolbarMain[chn],TBM_SETIMAGE, (WPARAM)id, (LPARAM)item_bmp[EN_TOOLBAR_MANUAL_RECORD].close_bmp);
		}
		if(orignalbmp_saved == 0)
		{
			item_bmp[EN_TOOLBAR_MANUAL_RECORD].origina_bmp = (BITMAP *)oldbmp;
			orignalbmp_saved = 1;
		}
		manualRec[chn] = openRec;
	}
	else if (id == IDC_MAIN_SOUND_OPENCLOSE)
	{
		//声音
		int ret, open, i = 0;
		static  int orignalbmp_saved = 0;
		open = !audioOpen[chn];
		ret = player_set_audio_function(chn, open);
		if (ret == -1 ) return;

		for (i = 0; i < 4; i++)
		{	
			if (chn == i) 	audioOpen[chn] = open;
			else	audioOpen[i] = 0;
			
			if (audioOpen[i])
				oldbmp = SendMessage(hToolbarMain[i],TBM_SETIMAGE, (WPARAM)id, (LPARAM)item_bmp[EN_TOOLBAR_SOUND].open_bmp);
			else
				oldbmp = SendMessage(hToolbarMain[i],TBM_SETIMAGE, (WPARAM)id, (LPARAM)item_bmp[EN_TOOLBAR_SOUND].close_bmp);
			
			if (orignalbmp_saved == 0)
			{
				item_bmp[EN_TOOLBAR_SOUND].origina_bmp = (BITMAP *)oldbmp;
				orignalbmp_saved = 1;
			}
		}
	}
	else if (id == IDC_MAIN_CAPUTURE)
	{
		//printf("@@@@@@@@@@@@@@@@@@@@@%s[%d] chn: %d\r\n",__FILE__,__LINE__,chn);
		//抓拍
		ShowWindow(hToolbarMain[chn], SW_HIDE);			//需要先隐藏toolbar
		SendMessage(hWnd, MSG_COMMAND, ID_LIST_PTZ_CTRL, 0); //打开云台控制界面
	}
}



#ifdef MAINMENU_USE_CHN_CTRL
static HMENU sigle_createpmenufile (void)
{
    HMENU hmnu;
	HMENU hMenuFloat;
    MENUITEMINFO mii;
    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;	
    mii.id          = 0;
    mii.typedata    = (DWORD)"CH   ";
    hmnu = CreatePopupMenu (&mii);

	memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_4CH1;
    mii.typedata    = (DWORD)"通道一   ";
    InsertMenuItem(hmnu, 0, TRUE, &mii);
    
    
    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_4CH2;
    mii.typedata    = (DWORD)"通道二   ";
    InsertMenuItem(hmnu, 1, TRUE, &mii);
    
    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_4CH3;
    mii.typedata    = (DWORD)"通道三   ";
    InsertMenuItem(hmnu, 2, TRUE, &mii);
    
    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_4CH4;
    mii.typedata    = (DWORD)"通道四   ";
    InsertMenuItem(hmnu, 3, TRUE, &mii);

	hMenuFloat= StripPopupHead(hmnu); 

    return hMenuFloat;
}

static HMENU multi_createpmenufile (void)
{
    HMENU hmnu;	
	HMENU hMenuFloat;
    MENUITEMINFO mii;
    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;	
    mii.id          = 0;
    mii.typedata    = (DWORD)"CH   ";
    hmnu = CreatePopupMenu (&mii);

	memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_CH1TO4;
    mii.typedata    = (DWORD)"通道一-四   ";
    InsertMenuItem(hmnu, 0, TRUE, &mii);

	hMenuFloat= StripPopupHead(hmnu); 
    return hMenuFloat;
}
#endif

static HMENU create_rightbutton_menu (HWND hWnd, int px, int py) 
{ 
	int i, nListLen; 
	HMENU hMenuFloat; 
	MENUITEMINFO mii; 
	HMENU hMenu; 
	
	nListLen = TABLESIZE(mainListInfo);
	memset (&mii, 0, sizeof(MENUITEMINFO)); 
	mii.type = MFT_STRING; 
	mii.id = 0; 
	mii.typedata = (DWORD)"File"; 
	char szName[64];
	hMenu = CreatePopupMenu (&mii); 	
	for ( i = 0; i < nListLen; i ++ ) 
	{ 
		memset (&mii, 0, sizeof (MENUITEMINFO) ); 		
		mii.type = MFT_STRING; 
		mii.id = mainListInfo[i].id; 
		mii.state = 0;
#ifdef MAINMENU_USE_CHN_CTRL		
		if (i == 0)
			mii.hsubmenu = sigle_createpmenufile();
		else if (i == 1)
			mii.hsubmenu = multi_createpmenufile();
#endif	
		sprintf(szName, "%s    ", LTR(mainListInfo[i].name));
		mii.typedata= (DWORD)szName; 
		InsertMenuItem (hMenu, i, TRUE, &mii ); 
	} 
	hMenuFloat= StripPopupHead(hMenu); 
	TrackPopupMenu (hMenuFloat, TPM_LEFTALIGN | TPM_TOPALIGN , px, py, hWnd); 
	return hMenu;
}

static void RealplayDblClkProc(HWND hWnd)
{
	int ret; 
	
	if (g_stRealCtrl.chSelect < 0)	
		return;
	if (g_stRealCtrl.displaymode == 0)
	{	
		//四分屏变为全屏
		ret = player_set_dis_mode(1, g_stRealCtrl.chSelect);
	}
	else if (g_stRealCtrl.displaymode == 1)
	{				
		//全屏变四分频
		ret = player_set_dis_mode(0, g_stRealCtrl.chSelect);				
	}
	g_stRealCtrl.displaymode = player_get_display_mode();
	
	if (ret == 0);
	{
		g_stRealCtrl.Realstate[0] = REAL_STATE_STOP;
		g_stRealCtrl.Realstate[1] = REAL_STATE_STOP;
		g_stRealCtrl.Realstate[2] = REAL_STATE_STOP;
		g_stRealCtrl.Realstate[3] = REAL_STATE_STOP;
	}
}

static void ui_replay_set()
{
	player_status status;
	player_get_player_status( 0 , &status);
	player_set_replay_mode(4);
}

static void StartSysTimer(HWND hWnd)
{
	if (IsTimerInstalled(hWnd, SYS_TIME_TIMER))
		return;
	SetTimer(hWnd, SYS_TIME_TIMER, 50);
}

static void EndSysTime(HWND hWnd)
{
	if (IsTimerInstalled(hWnd, SYS_TIME_TIMER))
		KillTimer(hWnd,SYS_TIME_TIMER);
}

static int IsPreviewAlive(int ch)
{	
	int ret; 
	if ((ch > NVR_MAX_CHANNEL_NUM)||(ch < 0))
		return 0;
	ret = player_get_preview_status(ch);
	if ((ret>>ch)&0x01) return 1;
	else	return 0;
}
static int IsDefaultFilesystem()
{
	FILE *fp;
	char cmdline[1024];
	static int isDefault = -1;

	if(isDefault == -1)
	{
		isDefault = 0;
		fp = fopen("/proc/cmdline", "r");
		if(fp != NULL)
		{
			fgets(cmdline, sizeof(cmdline), fp);
			if(strstr(cmdline, "default"))
				isDefault = 1;
			fclose(fp);
		}
	}
	return isDefault;
}
static void DesktopTimerDrawProc(HDC hdc)
{
	int recordMask = 0;
	int status = 0, ch = 0;
	PLOGFONT temp_font;
	PLOGFONT caption_font;
	StorageRecordInfo stRecInfo;
	const char *strCh[4]={"通道一", "通道二", "通道三", "通道四"};

	memset(&stRecInfo, 0, sizeof(StorageRecordInfo));
	storage_lib_get_record_info(&stRecInfo);
	recordMask = stRecInfo.record_mask;
	
	SetBkMode(hdc, BM_TRANSPARENT);//文本背景模式
	temp_font = GetCurFont(hdc);
	caption_font = GetSystemFont(SYSLOGFONT_MENU);
	SelectFont(hdc,caption_font);
	if(g_stRealCtrl.displaymode == 0)
	{
		for (ch = 0; ch < 4; ch++)
		{			
			SetTextColor(hdc, PIXEL_red);	
			av_pool_get_alarm_status(ch, &status);
			if(status)
			{
				FillBoxWithBitmap(hdc, alarmRect[ch].left, alarmRect[ch].top, 
					RECTW(alarmRect[ch]),RECTH(alarmRect[ch]), &bmpAlarm);
				//DrawText(hdc, "A", -1, &alarmRect[ch], DT_VCENTER|DT_SINGLELINE);
			}
			
			SetTextColor(hdc, PIXEL_green); 
			if (recordMask&(0x01<<ch))
			{
				FillBoxWithBitmap(hdc, recordRect[ch].left, recordRect[ch].top, 
					RECTW(recordRect[ch]),RECTH(recordRect[ch]), &bmpRecord);
				//DrawText(hdc, "R", -1, &recordRect[ch], DT_VCENTER|DT_SINGLELINE);
			}
			
			SetTextColor(hdc, PIXEL_lightwhite);	
			DrawText(hdc, LTR(strCh[ch]), -1, &textRect[ch], DT_CENTER|DT_VCENTER|DT_SINGLELINE);
/*
			status = IsPreviewAlive(ch);
			if((status == 0))
			{
				DrawText(hdc, LTR("视频丢失"), -1, &playRect[ch], DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			}
*/
		}
	}
	else
	{			
		SetTextColor(hdc, PIXEL_red);
		
		if (g_stRealCtrl.chSelect > 3 || g_stRealCtrl.chSelect < 0)
			return;
		
		av_pool_get_alarm_status(g_stRealCtrl.chSelect, &status);
		if(status)
		{
			FillBoxWithBitmap(hdc, alarmRect[4].left, alarmRect[4].top, 
					RECTW(alarmRect[4]),RECTH(alarmRect[4]), &bmpAlarm);
			//DrawText(hdc, "A", -1, &alarmRect[4], DT_VCENTER|DT_SINGLELINE);
		}
		
		SetTextColor(hdc, PIXEL_green); 
		if (recordMask&(0x01<<g_stRealCtrl.chSelect))
		{
			FillBoxWithBitmap(hdc, recordRect[4].left, recordRect[4].top, 
					RECTW(recordRect[4]),RECTH(recordRect[4]), &bmpRecord);
			//DrawText(hdc, "R", -1, &recordRect[4], DT_VCENTER|DT_SINGLELINE);
		}

		SetTextColor(hdc, PIXEL_lightwhite);	
		DrawText(hdc, LTR(strCh[g_stRealCtrl.chSelect]), -1, &textRect[4], DT_CENTER|DT_VCENTER|DT_SINGLELINE);
/*
		status = IsPreviewAlive(g_stRealCtrl.chSelect);
		if((status == 0))
		{
			DrawText(hdc, LTR("视频丢失"), -1, &playRect[4], DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
*/
	}
	NVR_DEV_OSD_CFG_S sOsdCfg;

	memset(&sOsdCfg, 0, sizeof(NVR_DEV_OSD_CFG_S));
	nvr_logon_get_osd_cfg(0, &sOsdCfg);

	if(sOsdCfg.u8EnableTimeOsd == 1)
	{
		struct tm stTime, *ptm;
		time_t t;	
		char timeText[32];
		t = time(NULL);
		ptm = localtime_r(&t, &stTime);
		
		sprintf(timeText, "%04d-%02d-%02d %02d:%02d:%02d", 
				ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
				ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
		SetTextColor(hdc, PIXEL_lightwhite);	
		DrawText(hdc, timeText, -1, &TimeScreen, DT_CENTER|DT_VCENTER|DT_SINGLELINE);	
	}
	
	if(IsDefaultFilesystem())
	{
		SetTextColor(hdc, PIXEL_red);	
		DrawText(hdc, LTR("请升级设备"), -1, &UpdateScreen, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	}
	if (stRecInfo.format_status)
	{
		SetTextColor(hdc, PIXEL_green);	
		DrawText(hdc, LTR("正在格式化硬盘..."), -1, &FormatScreen, DT_CENTER|DT_VCENTER|DT_SINGLELINE);	
	}
	SelectFont(hdc,temp_font);
}
static void DesktopTimeProc(HWND hWnd)
{
	int chn;
	if(g_stRealCtrl.displaymode == 0)
	{
		for (chn = 0; chn < 4; chn++)
		{				
			InvalidateRect(hWnd, &alarmRect[chn], TRUE);
			InvalidateRect(hWnd, &recordRect[chn], TRUE);		
			InvalidateRect(hWnd, &playRect[chn], TRUE);				
		}
	}
	else
	{			
		InvalidateRect(hWnd, &alarmRect[4], TRUE);
		InvalidateRect(hWnd, &recordRect[4], TRUE);		
		InvalidateRect(hWnd, &playRect[4], TRUE);				
	}
	InvalidateRect(hWnd, &TimeScreen, TRUE);	
	InvalidateRect(hWnd, &FormatScreen, TRUE);
	UpdateWindow(hWnd, FALSE);
	
	static int testDlgCreate = 0;

	if(res_init_ok && testDlgCreate == 0)
	{				
		PostMessage(hWnd,MSG_FACTORY_TEST, 0 ,0);
		testDlgCreate = 1;
	}
}

static void SwitchFullScreen(HWND hWnd)
{
	//若是四分屏，切换到全屏
	player_status status;
	int dismode = g_stRealCtrl.displaymode;
	if (dismode == 0)
	{
		player_set_dis_mode(1, g_stRealCtrl.chSelect);	
		g_stRealCtrl.Realstate[0] = REAL_STATE_STOP;
		g_stRealCtrl.Realstate[1] = REAL_STATE_STOP;
		g_stRealCtrl.Realstate[2] = REAL_STATE_STOP;
		g_stRealCtrl.Realstate[3] = REAL_STATE_STOP;
		g_stRealCtrl.displaymode = player_get_display_mode();
		//该处只是为了阻塞
		player_get_player_status(g_stRealCtrl.chSelect , &status);
	}					
	InvalidateRect(hWnd, NULL, 1);
}


static void MainMouseMoveProc(HWND hWnd, int pos_x, int pos_y)
{	
	int TB_W, TB_H;	
	RECT winposition;
	//int status;
	//HDC hdc;

	//PLOGFONT temp_font;
	//PLOGFONT caption_font;

	int chn = g_stRealCtrl.chSelect;
	GetWindowRect(hToolbarMain[chn], &winposition);
	TB_W =RECTW(winposition);
	TB_H = RECTH(winposition);
	
	RECT ToolBarRect[5] = 
	{
		{(NVR_SCREEN_WIDTH/2 - TB_W)/2,  0+4, 					NVR_SCREEN_WIDTH/4+TB_W/2, 		TB_H},
		{ NVR_SCREEN_WIDTH*3/4 - TB_W/2, 0+4, 					NVR_SCREEN_WIDTH*3/4+TB_W/2, 	TB_H},
		{(NVR_SCREEN_WIDTH/2 - TB_W)/2,  NVR_SCREEN_HEIGHT/2+4, NVR_SCREEN_WIDTH/4+TB_W/2, 		NVR_SCREEN_HEIGHT/2+TB_H},
		{ NVR_SCREEN_WIDTH*3/4 - TB_W/2, NVR_SCREEN_HEIGHT/2+4, NVR_SCREEN_WIDTH*3/4+TB_W/2, 	NVR_SCREEN_HEIGHT/2+TB_H},
		{(NVR_SCREEN_WIDTH - TB_W)/2,    0+4, 					(NVR_SCREEN_WIDTH+TB_W)/2, 		TB_H}
	};
#if 0
	RECT showRect[5] = 
	{
		{0+4,  0+4, NVR_SCREEN_WIDTH/2-4, NVR_SCREEN_HEIGHT/2-4},
		{ NVR_SCREEN_WIDTH/2+4, 0+4, NVR_SCREEN_WIDTH-4,NVR_SCREEN_HEIGHT/2-4},
		{0+4 ,  NVR_SCREEN_HEIGHT/2+4,  NVR_SCREEN_WIDTH/2-4, NVR_SCREEN_HEIGHT-4},
		{ NVR_SCREEN_WIDTH/2+4, NVR_SCREEN_HEIGHT/2+4, NVR_SCREEN_WIDTH-4, 	NVR_SCREEN_HEIGHT-4},
		{0+4,    0+4, 	NVR_SCREEN_WIDTH-4, NVR_SCREEN_HEIGHT-4}
	};
#endif
	if (g_stRealCtrl.displaymode == 1) //1x1
	{		
		if (PtInRect(&ToolBarRect[4], pos_x, pos_y))
		{
/*			if (UIBeLogin() == 0) //锁屏后不显示
			{	
				return;
			}	
*/			MoveWindow(hToolbarMain[chn], ToolBarRect[4].left, ToolBarRect[4].top, TB_W, TB_H, FALSE);
			ShowWindow(hToolbarMain[chn], SW_SHOW);
		}
		else
		{
			ShowWindow(hToolbarMain[chn], SW_HIDE);
		}
#if 0
		status = IsPreviewAlive(chn);
		if(PtInRect(&showRect[4], pos_x, pos_y) && (status == 0))
		{
			
			hdc = GetDC(hWnd);

			SetBkMode(hdc, BM_TRANSPARENT);//文本背景模式
			temp_font = GetCurFont(hdc);
			caption_font = GetSystemFont(SYSLOGFONT_MENU);
			SelectFont(hdc,caption_font);
			
			SetTextColor(hdc, PIXEL_lightwhite);	
			DrawText(hdc, "视频丢失", -1, &playRect[4], DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			SelectFont(hdc,temp_font);
			//SetTextColor(hdc,PIXEL_red);
			//FillBoxWithBitmap(hdc, playRect[4].left, playRect[4].top, 
			//	RECTW(playRect[4]),RECTH(playRect[4]), &bmpRecord);
			ReleaseDC(hdc);
		}
		else
		{
			InvalidateRect(hWnd, &playRect[4], TRUE);
		}
#endif		
	}
	else
	{
		int i = 0;
		for (i = 0; i < 4; i++)
		{			
			if (PtInRect(&ToolBarRect[i], pos_x, pos_y))
			{
/*				if (UIBeLogin() == 0)
				{	
					return;
				}
*/				MoveWindow(hToolbarMain[i], ToolBarRect[i].left, ToolBarRect[i].top, TB_W, TB_H, FALSE);
				ShowWindow(hToolbarMain[i], SW_SHOW);
			}
			else
			{				
				ShowWindow(hToolbarMain[i], SW_HIDE);
			}
#if 0			
			status = IsPreviewAlive(i);
			if(PtInRect(&showRect[i], pos_x, pos_y) && (status == 0))
			{
				
				hdc = GetDC(hWnd);

				SetBkMode(hdc, BM_TRANSPARENT);//文本背景模式
				temp_font = GetCurFont(hdc);
				caption_font = GetSystemFont(SYSLOGFONT_MENU);
				SelectFont(hdc,caption_font);
	
				SetTextColor(hdc, PIXEL_lightwhite);	
				DrawText(hdc, "视频丢失", -1, &playRect[i], DT_CENTER|DT_VCENTER|DT_SINGLELINE);
				SelectFont(hdc,temp_font);
				//SetTextColor(hdc,PIXEL_red);
				//FillBoxWithBitmap(hdc, playRect[i].left, playRect[i].top, 
				//	RECTW(playRect[i]),RECTH(playRect[i]), &bmpRecord);
				ReleaseDC(hdc);
			}
			else
			{
				InvalidateRect(hWnd, &playRect[i], TRUE);
			}
#endif
		}
	}
}

static void DrawRealplayChInfo(HDC hdc)
{
	SetPenWidth(hdc,3); 
	SetBrushType (hdc, BT_SOLID);
	SetPenColor(hdc, 0xff0072B9);
	
	LineEx(hdc, 1, 1, rectReplayScreen[0].right - 1, 1);		
	LineEx(hdc, rectReplayScreen[0].right - 1, 1, rectReplayScreen[0].right - 1, rectReplayScreen[0].bottom - 1);		
	LineEx(hdc, 1, 1, 1, rectReplayScreen[0].bottom - 1);		
	LineEx(hdc, 1, rectReplayScreen[0].bottom - 1, rectReplayScreen[0].right - 1, rectReplayScreen[0].bottom - 1);

	if (g_stRealCtrl.displaymode == 0)
	{
		LineEx(hdc,(rectReplayScreen[0].right>>1)-1, rectReplayScreen[0].top,
			(rectReplayScreen[0].right>>1)-1,rectReplayScreen[0].bottom);
		LineEx(hdc,rectReplayScreen[0].left, (rectReplayScreen[0].bottom>>1)-1,
			rectReplayScreen[0].right, (rectReplayScreen[0].bottom>>1)-1);
		

		SetPenColor(hdc, 0xFFD0D1D3);		
		int x = rectReplayScreen[0].left + (g_stRealCtrl.chSelect%2)*RECTW(rectReplayScreen[0])/2;
		int y = rectReplayScreen[0].top + (g_stRealCtrl.chSelect/2)*RECTH(rectReplayScreen[0])/2;
		int w = (RECTW(rectReplayScreen[0])>>1);
		int h = (RECTH(rectReplayScreen[0])>>1);		
		if (g_stRealCtrl.chSelect == 0)
			Rectangle(hdc,x+2, y+2, x+w-2, y+h-2);
		else if (g_stRealCtrl.chSelect == 1)
			Rectangle(hdc,x, y+2, x+w-2, y+h-2);
		else if (g_stRealCtrl.chSelect == 2)
			Rectangle(hdc,x+2, y, x+w-2, y+h-2);
		else
			Rectangle(hdc,x, y, x+w-2, y+h-2);	
	}
	else
	{		
		SetPenColor(hdc, 0xFFD0D1D3);		
		int x = rectReplayScreen[0].left;
		int y = rectReplayScreen[0].top;
		int w = RECTW(rectReplayScreen[0]);
		int h = RECTH(rectReplayScreen[0]);	
		Rectangle(hdc,x+2, y+2, x+w-2, y+h-2);	
	}
		
}

static void DesktopPaintProc(HDC hdc)
{
	DrawRealplayChInfo(hdc);
	DesktopTimerDrawProc(hdc);
}


static void _UpdateChInfo(HWND hWnd, int pos_x, int pos_y)
{	
	if (g_stRealCtrl.displaymode == 0)
	{
		int x = pos_x/(RECTW(rectReplayScreen[0])>>1);
		int y = pos_y/(RECTH(rectReplayScreen[0])>>1);
		int ch = x+y*2;
		if (ch < 0) ch = 0;
		else if (ch > 3) ch = 3;
		if (g_stRealCtrl.chSelect != ch)
		{
			g_stRealCtrl.chSelect = ch;			
			InvalidateRect(hWnd, NULL, 1);	
		}
	}	
}
static void DesktopRightButtonProc(HWND hWnd, LPARAM lParam)
{
	int x_pos = LOSWORD (lParam);
	int y_pos = HISWORD (lParam);
	int i;
	
	if (UIBeLogin() == 0)
	{	
		for(i=0; i<NVR_MAX_CHANNEL_NUM; i++)
			ShowWindow(hToolbarMain[i], SW_HIDE);	//在锁屏时，右单击toolbar不响应，隐藏toolbar
		PostMessage(GetMainWin(),MSG_LOGIN, 0 ,0);
		return;
	}
	if (wizardCtrl.needWizard == 1)
	{
		PostMessage(GetMainWin(),MSG_WIZARD, 0 ,0);
		return;
	}
	
	_UpdateChInfo(hWnd, x_pos, y_pos);
	if (hRightMenu != 0)
	{
		DestroyMenu (hRightMenu);
		hRightMenu = 0;
	}
	hRightMenu = create_rightbutton_menu(hWnd, x_pos, y_pos);
}

static void DesktopLeftButtonProc(HWND hWnd, LPARAM lParam)
{
	int x_pos = LOSWORD (lParam);
	int y_pos = HISWORD (lParam);
	int i;
	if (UIBeLogin() == 0)
	{	
		for(i=0; i<NVR_MAX_CHANNEL_NUM; i++)
			ShowWindow(hToolbarMain[i], SW_HIDE);	//在锁屏时，左单击toolbar不响应，隐藏toolbar
		PostMessage(GetMainWin(),MSG_LOGIN, 0 ,0);
		return;
	}
	
	if (wizardCtrl.needWizard == 1)
	{
		PostMessage(GetMainWin(),MSG_WIZARD, 0 ,0);
		return;
	}
	
	_UpdateChInfo(hWnd,x_pos,y_pos);
}

static void CheckCursorInToolbar(HWND hWnd, int pos_x, int pos_y)
{
	int TB_W, TB_H, i;	
	RECT winposition;

	int chn = g_stRealCtrl.chSelect;
	GetWindowRect(hToolbarMain[chn], &winposition);
	TB_W =RECTW(winposition);
	TB_H = RECTH(winposition);
	
	RECT ToolBarRect[5] = 
	{
		{(NVR_SCREEN_WIDTH/2 - TB_W)/2,  0+4, 					NVR_SCREEN_WIDTH/4+TB_W/2, 		TB_H},
		{ NVR_SCREEN_WIDTH*3/4 - TB_W/2, 0+4, 					NVR_SCREEN_WIDTH*3/4+TB_W/2, 	TB_H},
		{(NVR_SCREEN_WIDTH/2 - TB_W)/2,  NVR_SCREEN_HEIGHT/2+4, NVR_SCREEN_WIDTH/4+TB_W/2, 		NVR_SCREEN_HEIGHT/2+TB_H},
		{ NVR_SCREEN_WIDTH*3/4 - TB_W/2, NVR_SCREEN_HEIGHT/2+4, NVR_SCREEN_WIDTH*3/4+TB_W/2, 	NVR_SCREEN_HEIGHT/2+TB_H},
		{(NVR_SCREEN_WIDTH - TB_W)/2,    0+4, 					(NVR_SCREEN_WIDTH+TB_W)/2, 		TB_H}
	};

	if(g_stRealCtrl.displaymode == 1)
	{
		if(PtInRect(&ToolBarRect[4], pos_x, pos_y))
		{
			ShowWindow(hToolbarMain[chn], SW_HIDE);
		}
	}
	else
	{
		for(i=0; i<4; i++)
		{
			if(PtInRect(&ToolBarRect[i], pos_x, pos_y))
			{
				ShowWindow(hToolbarMain[i], SW_HIDE);
			}
		}
	}
}

static void DesktopLeftButtonDbclkProc(HWND hWnd, LPARAM lParam)
{
	int x_pos = LOSWORD (lParam);
	int y_pos = HISWORD (lParam);
	if (UIBeLogin() == 0)
	{	
		PostMessage(GetMainWin(),MSG_LOGIN, 0 ,0);
		return;
	}
	if (wizardCtrl.needWizard == 1)
	{
		PostMessage(GetMainWin(),MSG_WIZARD, 0 ,0);
		return;
	}
	CheckCursorInToolbar(hWnd, x_pos, y_pos);//光标在Toolbar上双击时，隐藏Toolbar
	_UpdateChInfo(hWnd,x_pos,y_pos);
	RealplayDblClkProc(hWnd);			
	InvalidateRect(hWnd, NULL, 1);
	RecoverBeforePlayState(REALTIME_PLAY); 
}

static void ui_realplay_start()
{
	int chn = 0;
	for (chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++){
		g_stRealCtrl.Realstate[chn] = REAL_STATE_PLAY;
			player_real_play(chn);
	}
}

static void ui_realplay_stop()
{
	int chn = 0;
	for (chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++){
		g_stRealCtrl.Realstate[chn] = REAL_STATE_STOP;
		player_stop(chn);
	}
}

static int desktopWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{

	switch (message) 
	 {
        case MSG_CREATE:
		{	
			LoadBitmapFromFile(HDC_SCREEN, &bmpAlarm, "nvr_res/alarm.bmp");			
			LoadBitmapFromFile(HDC_SCREEN, &bmpRecord, "nvr_res/record.bmp");			
			g_stRealCtrl.chSelect = 0;
			g_stRealCtrl.displaymode = player_get_display_mode();
			ui_realplay_start();
			StartSysTimer(hWnd);
			InitToolbar(hWnd);
			init_open_dlg(&headOpenDlg);
		}
		break;
        case MSG_PAINT:
		{
        	HDC hdc = BeginPaint (hWnd);
			DesktopPaintProc(hdc);
       		EndPaint (hWnd, hdc);
		}
		break;
		
		case MSG_MOUSEMOVE:	
		{	
			int x_pos = LOSWORD (lParam);
			int y_pos = HISWORD (lParam);
			MainMouseMoveProc(hWnd, x_pos, y_pos);
			break;
		}
		break;
		case MSG_FACTORY_TEST:
		{
			if (res_init_ok == 0) break;
			
			DEV_SystemCfg *pSysCfg;
			pSysCfg = get_system_cfg();
			if(pSysCfg->u8FactoryChecked == 0)
			{
				CreateNvrTestDlgWin(hWnd);
			}
		}
		break;
		case MSG_LOGIN:
		{		
			//图片资源初始化完成后才能进行对话框创建操作
			if (res_init_ok == 0) break;
			CreateLoginDlgWin(GetMainWin());
			break;
		}
		break;
		case MSG_WIZARD:
		{
			if (res_init_ok == 0)
				break;
			CreateWizardDlgWin(hWnd);
			wizardCtrl.needWizard = 0;
		}
		break;
		case MSG_REPLAY_CLOSE:			
		{	
			player_set_dis_mode(g_stRealCtrl.displaymode, g_stRealCtrl.chSelect);
			ui_realplay_start();
			InvalidateRect(hWnd, NULL, 1);
			break;
		}
		case MSG_COMMAND:
		{			
			//图片资源初始化完成后才能进行对话框创建操作
			if (res_init_ok == 0)
				break;

			int id   = HIWORD(wParam);
			if ((id >= IDC_MAIN_PLAYSTOP)&&(id <IDC_TOOLBAR_CH1))
			{
				ToolBarCommandProc(hWnd, id);
				return 0;
			}
			switch (wParam) 
			{
				case ID_LIST_PTZ_CTRL:
					SwitchFullScreen(hWnd);					
					CreatePtzDlgWin(hWnd, g_stRealCtrl.chSelect);
					return 0;
				case ID_LIST_VEDIO_COLOR:
					CreateColorDlgWin(hWnd, g_stRealCtrl.chSelect);
					return 0;
				case ID_LIST_IPC_OSD:
					CreateIpcOsdDlgWin(hWnd, g_stRealCtrl.chSelect);
					return 0;
				case ID_LIST_VEDIO_ENCODE:
					CreateEncodeDlgWin(hWnd, g_stRealCtrl.chSelect);
					return 0;
				case ID_LIST_VOD_QUERY:					
					ui_realplay_stop();
					ui_replay_set();
					CreateReplayDlgWin(hWnd, g_stRealCtrl.displaymode, g_stRealCtrl.chSelect);
				return 0;
				case ID_LIST_WIZARD:
					wizardCtrl.needWizard = 1;					
					PostMessage(GetMainWin(),MSG_WIZARD, 0 ,0);
					break;
				case ID_LIST_VOD_CTRL:
					CreateRecordDlgWin(hWnd);
					return 0;
				case ID_LIST_REMOTE_DEV:
					CreateRemoteDeviceDlgWin(hWnd);
					return 0;
				case ID_LIST_ALARM_OUT:
					return 0;
				case ID_LIST_MAIN_MENU:
					CreateMainMenuDlgWin(hWnd, g_stRealCtrl.displaymode, g_stRealCtrl.chSelect);
					return 0;
				case ID_LIST_LOCK_SCREEN:					
					UISetUnLogin();
					return 0;
#ifdef OPEN_COLOKEY_DLG					
				case ID_LIST_COLORKEY:
					CreateAlphaColorkeyDlgWin(hWnd);
					return 0;
#endif
				default:
					break;
				}
		}
		break;
		case MSG_TIMER:
		{
			switch(wParam)
			{
				case SYS_TIME_TIMER:
					DesktopTimeProc(hWnd);
					return 0;
			}
			return 0;
		}
		break;

		case MSG_RBUTTONUP:
			DesktopRightButtonProc(hWnd, lParam);
		break;
		
		case MSG_LBUTTONDOWN:
			DesktopLeftButtonProc(hWnd, lParam);
		break;
		
		case MSG_LBUTTONDBLCLK:
			DesktopLeftButtonDbclkProc(hWnd, lParam);
		break;

		case MSG_MOUSE_SCROLL:
		{
			int mouseX = LOSWORD (lParam);
			int mouseY = HISWORD (lParam);
					
			if(1 == g_stRealCtrl.displaymode)
			{
				if(0 == playSrc[REALTIME_PLAY].initFlag)
				{
					InitPlaySrc(REALTIME_PLAY);
					playSrc[REALTIME_PLAY].initFlag = 1;
				}
				ZoomPlaySrc(REALTIME_PLAY, (int)wParam, mouseX, mouseY);
			}

	        break;
	   }
		case MSG_DESTROY:	
			DestroyAllControls (hWnd);			
			return 0;

        case MSG_CLOSE:
			EndSysTime(hWnd);
			ui_realplay_stop();
			UninitToolbar(hWnd);
			UnloadBitmap(&bmpAlarm);
			UnloadBitmap(&bmpRecord);
			PostMessage(hIME, MSG_CLOSE, 0, 0);
			uninit_open_dlg(&headOpenDlg);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
			return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);

}



HWND desktop_win_create(HWND parent)
{
	MAINWINCREATE  window_info;

    window_info.dwStyle = WS_VISIBLE/*|WS_BORDER|WS_CAPTION*/; 
    window_info.dwExStyle = WS_EX_NONE;         
    window_info.spCaption = "mainWin";          
    window_info.hMenu = 0;                      
    window_info.hCursor = GetSystemCursor(0);   
    window_info.hIcon = 0;                      
    window_info.MainWindowProc = desktopWinProc;       
    window_info.lx = 0;                
    window_info.ty = 0;
    window_info.rx = NVR_SCREEN_WIDTH;
    window_info.by = NVR_SCREEN_HEIGHT;
    window_info.iBkColor = MY_PIXEL_transparent;   
    window_info.dwAddData = 0;
    window_info.hHosting = parent;        
    
    return CreateMainWindow(&window_info);
}

static int mouse_probe_hook (void* context, HWND dst_wnd, int msg, WPARAM wParam, LPARAM lparam) 
{	
#if defined(HIDE_CURSER) || defined(LOCK_SCREEN)		
	old_tick_count = GetTickCount ();
	if(msg == MSG_LBUTTONDOWN)
		is_mouse_down = 1;
	else if(msg == MSG_LBUTTONUP)
		is_mouse_down = 0;
#endif
	if (msg == MSG_RBUTTONDOWN)
	{
		HWND ActiveWin = GetActiveWindow();
		if (IsDialog(ActiveWin))
		{
			PostMessage(ActiveWin, MSG_CLOSE, 0, 0);
		}
	}
	if (msg == MSG_LBUTTONDBLCLK)
	{		
		HWND ActiveWin = GetActiveWindow();
		if (ActiveWin != dst_wnd)
		{
			return HOOK_STOP;
		}
	}
	return HOOK_GOON;
}
extern HWND getNvrTestWin(void);
extern HWND getStorageBackupWin(void);


#if defined(HIDE_CURSER) || defined(LOCK_SCREEN)		
void* mouse_show_ctrl(void * argv)
{
	HWND hDlg;
	while(1)
	{	
		//十秒钟鼠标没有动自动隐藏鼠标
#ifdef HIDE_CURSER
		if ((GetTickCount () > old_tick_count + 1000) && (is_mouse_down == 0))
		{
			if (curShow)
			{				
				ShowCursor (FALSE);
				curShow = 0;
			}
		}
		else
		{			
			if (!curShow)
			{
				ShowCursor (TRUE);
				curShow = 1;
			}
		}
#endif
		if((GetTickCount() > old_tick_count + 12*1000)&&(HWND_INVALID == getNvrTestWin())&&(HWND_INVALID == getStorageBackupWin()))//2分钟自动锁屏
		{
			if(!gui_get_disk_status())  //若正在格式化硬盘，等格式化完之后再响应锁屏
			{
				//printf("!!!!!!!!!!!!!!!!!!!!!wait lock screen\r\n");
			}
			else if(SystemIsNfsBoot())
			{
				// none
			}
			else
			{
				if (hRightMenu != 0)  //右键快捷菜单
				{
					//printf("#####################%s %d,%x\r\n",__func__,__LINE__,hRightMenu);
					SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 0);
					hRightMenu = 0;
				}
				
				while(pop_open_dlg(&headOpenDlg, &hDlg))//关闭打开的dialog
				{
					PostMessage(hDlg, MSG_CLOSE, 0, 0);
				}
			
				UISetUnLogin();
			}

		}
		usleep(100000);
	}

	return NULL;
}

static void MouseShowHideCtrlThread()
{	
	pthread_t mousethread;
	pthread_attr_t new_attr;	
	pthread_attr_init (&new_attr);
	pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
	pthread_create (&mousethread, &new_attr, mouse_show_ctrl, NULL);	
	pthread_attr_destroy (&new_attr);
}
#endif

static int SystemIsNfsBoot()
{
	FILE *fp;
	char cmdline[1024];
	static int nfs = 0;
	static int init = 0;
	if(init == 0)
	{
		fp = fopen("/proc/cmdline", "r");
		if(fp != NULL)
		{
			fgets(cmdline, sizeof(cmdline), fp);
			if(strstr(cmdline, "root=/dev/nfs"))
				nfs = 1;
			fclose(fp);
		}
		init = 1;
	}
	return nfs;
}


void* res_init_task(void * argv)
{
	GUI_InitGloblePicRes(); 
	res_init_ok = 1;	
	return NULL;
}


static void GUI_InitGobleResTask()
{
	pthread_t mousethread;
	pthread_attr_t new_attr;	
	pthread_attr_init (&new_attr);
	pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
	pthread_create (&mousethread, &new_attr, res_init_task, NULL);	
	pthread_attr_destroy (&new_attr);
}


void SwitchPlayChan(HWND hWnd, int ch)
{
	player_status status;

	if (ch < 0) ch = 0;
	else if (ch > 3) ch = 3;

	if (g_stRealCtrl.chSelect != ch)
	{
		//player_set_dis_mode(0, g_stRealCtrl.chSelect);		
		g_stRealCtrl.chSelect = ch;
		player_set_dis_mode(1, g_stRealCtrl.chSelect);

		player_get_player_status(g_stRealCtrl.chSelect , &status);
		playSrc[REALTIME_PLAY].initFlag = 0;
	}
	InvalidateRect(GetParent(hWnd), NULL, 1);	
}

void SetSysLanguage(void)
{
	NVR_DEV_INFO_S devInfo;
	memset(&devInfo, 0, sizeof(NVR_DEV_INFO_S));
	if(nvr_logon_get_dev_cfg(0, &devInfo) < 0)
	{
		printf("[error] get dev cfg\n");
		SetDisplayLanguage(1);//获取系统参数失败，默认英文
		return;
	}
	if(1 == devInfo.stVarInfo.u8Language)
	{
		
		SetDisplayLanguage(1);//英文
	}
	else if(2 == devInfo.stVarInfo.u8Language)
	{
		SetDisplayLanguage(2);//繁体中文
	}else
	{
		SetDisplayLanguage(0); //简体中文
	}

	return;
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
	
	signal(SIGPIPE, SIG_IGN);
	InitLanguage();
	SetInputLanguage();

	if(SystemIsNfsBoot())
		UISetLogin("admin");
#ifdef _LITE_VERSION
    SetDesktopRect(0, 0, 1024, 768);
#endif
	GUI_InitGobleResTask();

#ifdef TI_NVR
	storage_ext_init();
	av_pool_init();
	storage_lib_init();
	nvr_lib_init();
	player_msg_init();
	log_init();
#endif
	SetSysLanguage();
	GUI_InitCtrl();
	if (!InitMiniGUIExt()) 
		return -1;
	hMainWnd = desktop_win_create(HWND_DESKTOP);
	if (hMainWnd == HWND_INVALID)
	{
		printf("create main windows failed!\n");	  
		return -1;
	}
	old_mouse_hook = RegisterMouseMsgHook (NULL, mouse_probe_hook); 	
#if defined(HIDE_CURSER) || defined(LOCK_SCREEN)		
	old_tick_count = GetTickCount();
#ifdef HIDE_CURSER
	ShowCursor (FALSE);
#endif
	MouseShowHideCtrlThread();
#endif
	hIME = GBIMEWindowCustom(hMainWnd);
    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    while (GetMessage(&Msg, hMainWnd)) 
	{
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
	}
	
	RegisterMouseMsgHook(NULL, old_mouse_hook);	
	GUI_UninitCtrl();
	MiniGUIExtCleanUp ();
    MainWindowThreadCleanup (hMainWnd);

#ifdef TI_NVR
	log_uninit();
	nvr_lib_uninit();
	storage_lib_uninit();
	av_pool_uninit();
	storage_ext_uninit();	
#endif
	UninitLanguage();
	GUI_UninitGloblePicRes(); 
	PostQuitMessage (HWND_DESKTOP);
    return 0;
}


#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif


