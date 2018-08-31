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
#include "my_trackbar.h"
#include "ctrl_ipaddress.h"
#include "nvr_lib_interface.h"

#include "gui_core.h"
#include "ui_dlg_head.h"

#define IPCOSD_DLG_W	600
#define IPCOSD_DLG_H	400

#define IPCOSD_DLG_X	((NVR_SCREEN_WIDTH - IPCOSD_DLG_W)>>1)
#define IPCOSD_DLG_Y	((NVR_SCREEN_HEIGHT - IPCOSD_DLG_H)>>1)
#define IPCOSD_TEXT_W	100
#define IPCOSD_EDIT_W	80
#define IPCOSD_EDIT_H	20

#define IPCOSD_TEXT_START_X		50
#define IPCOSD_TEXT_START_Y		60
#define IPCOSD_TEXT_INC_Y	(IPCOSD_EDIT_H+10)
#define IPCOSD_BUTTON_W 	100
#define IPCOSD_COMBOX_W 	100

#define	IPCOSD_TRACK_LEN	200


/////////////////////////////////////////////////////

typedef enum
{
	IDC_IPC_OSD_CHANNEL = 1400,
	IDC_IPC_OSD_SHOWTIME,
	IDC_IPC_OSD_TIMECOLOR,
	IDC_IPC_OSD_SHOWSTREAM,
	IDC_IPC_OSD_TEXTCOLOR,
	IDC_IPC_OSD_SHOWTEXT,
	IDC_IPC_OSD_COPY,
	IDC_IPC_OSD_SAVE,
	IDC_IPC_OSD_CANCEL,

	IDC_IPC_OSD_RED1,
	IDC_IPC_OSD_GREEN1,
	IDC_IPC_OSD_BLUE1,

	IDC_IPC_OSD_RED2,
	IDC_IPC_OSD_GREEN2,
	IDC_IPC_OSD_BLUE2,

	IDC_IPC_OSD_VRED1,
	IDC_IPC_OSD_VGREEN1,
	IDC_IPC_OSD_VBLUE1,
	

	IDC_IPC_OSD_VRED2,
	IDC_IPC_OSD_VGREEN2,
	IDC_IPC_OSD_VBLUE2,

}UI_IPCOSD_SETTING_ID;


static CTRLDATA ui_ipcosd_control[] =
{

{
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		IPCOSD_TEXT_START_X, IPCOSD_TEXT_START_Y, IPCOSD_TEXT_W, IPCOSD_EDIT_H,
		IDC_STATIC,
		"当前通道",
		0,
		WS_EX_TRANSPARENT
	},
	{ 
        GUI_COMBOX,			
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
		IPCOSD_TEXT_START_X+IPCOSD_TEXT_W, IPCOSD_TEXT_START_Y, IPCOSD_COMBOX_W, GUI_COMBOX_H,
        IDC_IPC_OSD_CHANNEL,
        "",
        80,
        WS_EX_USEPARENTFONT
    },	
	{
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,//| BS_CHECKED,
		IPCOSD_TEXT_START_X,IPCOSD_TEXT_START_Y+IPCOSD_TEXT_INC_Y, IPCOSD_BUTTON_W+10, IPCOSD_EDIT_H,
        IDC_IPC_OSD_SHOWTIME,
        "显示时间",
        0,
        WS_EX_TRANSPARENT
    },
    {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		IPCOSD_TEXT_START_X, IPCOSD_TEXT_START_Y+2*IPCOSD_TEXT_INC_Y, IPCOSD_TEXT_W, IPCOSD_EDIT_H,
		IDC_STATIC,
		"时间颜色",
		0,
		WS_EX_TRANSPARENT
	},
/*	
	{
		"static",
        WS_CHILD | SS_LEFT | WS_VISIBLE,
       	IPCOSD_TEXT_START_X+IPCOSD_TEXT_W, IPCOSD_TEXT_START_Y+2*IPCOSD_TEXT_INC_Y, 20, IPCOSD_EDIT_H,
        IDC_STATIC,
        "红",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
        IPCOSD_TEXT_START_X+IPCOSD_TEXT_W+50,  IPCOSD_TEXT_START_Y+2*IPCOSD_TEXT_INC_Y, IPCOSD_TRACK_LEN, 20,
        IDC_IPC_OSD_RED1,
        "",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        IPCOSD_TEXT_START_X+IPCOSD_TEXT_W+50+IPCOSD_TRACK_LEN+10, IPCOSD_TEXT_START_Y+2*IPCOSD_TEXT_INC_Y, 30, 20,
        IDC_IPC_OSD_VRED1,
        "0",
        0,
		WS_EX_TRANSPARENT
    },
     {
		"static",
        WS_CHILD | SS_LEFT | WS_VISIBLE,
       	IPCOSD_TEXT_START_X+IPCOSD_TEXT_W, IPCOSD_TEXT_START_Y+3*IPCOSD_TEXT_INC_Y, 20, IPCOSD_EDIT_H,
        IDC_STATIC,
        "绿",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
        IPCOSD_TEXT_START_X+IPCOSD_TEXT_W+50,  IPCOSD_TEXT_START_Y+3*IPCOSD_TEXT_INC_Y, IPCOSD_TRACK_LEN, 20,
        IDC_IPC_OSD_GREEN1,
        "",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        IPCOSD_TEXT_START_X+IPCOSD_TEXT_W+50+IPCOSD_TRACK_LEN+10, IPCOSD_TEXT_START_Y+3*IPCOSD_TEXT_INC_Y, 30, 20,
        IDC_IPC_OSD_VGREEN1,
        "0",
        0,
		WS_EX_TRANSPARENT
    },
     {
		"static",
        WS_CHILD | SS_LEFT | WS_VISIBLE,
       	IPCOSD_TEXT_START_X+IPCOSD_TEXT_W, IPCOSD_TEXT_START_Y+4*IPCOSD_TEXT_INC_Y, 20, IPCOSD_EDIT_H,
        IDC_STATIC,
        "蓝",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
        IPCOSD_TEXT_START_X+IPCOSD_TEXT_W+50,  IPCOSD_TEXT_START_Y+4*IPCOSD_TEXT_INC_Y, IPCOSD_TRACK_LEN, 20,
        IDC_IPC_OSD_BLUE1,
        "",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        IPCOSD_TEXT_START_X+IPCOSD_TEXT_W+50+IPCOSD_TRACK_LEN+10, IPCOSD_TEXT_START_Y+4*IPCOSD_TEXT_INC_Y, 30, 20,
        IDC_IPC_OSD_VBLUE1,
        "0",
        0,
		WS_EX_TRANSPARENT
    },

*/	
	{ 
        GUI_COMBOX,			
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
		IPCOSD_TEXT_START_X+IPCOSD_TEXT_W, IPCOSD_TEXT_START_Y+2*IPCOSD_TEXT_INC_Y, IPCOSD_COMBOX_W, GUI_COMBOX_H,
        IDC_IPC_OSD_TIMECOLOR,
        "",
        80,
        WS_EX_USEPARENTFONT
    },	

	{
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,//| BS_CHECKED,
		IPCOSD_TEXT_START_X,IPCOSD_TEXT_START_Y+3*IPCOSD_TEXT_INC_Y, IPCOSD_BUTTON_W+80, IPCOSD_EDIT_H,
        IDC_IPC_OSD_SHOWSTREAM,
        "显示码流信息",
        0,
        WS_EX_TRANSPARENT
    },
    //////////////////////////////////////////////////////////////////
    {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		IPCOSD_TEXT_START_X, IPCOSD_TEXT_START_Y+4*IPCOSD_TEXT_INC_Y, IPCOSD_TEXT_W, IPCOSD_EDIT_H,
		IDC_STATIC,
		"文本颜色",
		0,
		WS_EX_TRANSPARENT
	},
/*	
	{
		"static",
        WS_CHILD | SS_LEFT | WS_VISIBLE,
       	IPCOSD_TEXT_START_X+IPCOSD_TEXT_W, IPCOSD_TEXT_START_Y+6*IPCOSD_TEXT_INC_Y, 20, IPCOSD_EDIT_H,
        IDC_STATIC,
        "红",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
        IPCOSD_TEXT_START_X+IPCOSD_TEXT_W+50,  IPCOSD_TEXT_START_Y+6*IPCOSD_TEXT_INC_Y, IPCOSD_TRACK_LEN, 20,
        IDC_IPC_OSD_RED2,
        "",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        IPCOSD_TEXT_START_X+IPCOSD_TEXT_W+50+IPCOSD_TRACK_LEN+10, IPCOSD_TEXT_START_Y+6*IPCOSD_TEXT_INC_Y, 30, 20,
        IDC_IPC_OSD_VRED2,
        "0",
        0,
		WS_EX_TRANSPARENT
    },
     {
		"static",
        WS_CHILD | SS_LEFT | WS_VISIBLE,
       	IPCOSD_TEXT_START_X+IPCOSD_TEXT_W, IPCOSD_TEXT_START_Y+7*IPCOSD_TEXT_INC_Y, 20, IPCOSD_EDIT_H,
        IDC_STATIC,
        "绿",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
        IPCOSD_TEXT_START_X+IPCOSD_TEXT_W+50,  IPCOSD_TEXT_START_Y+7*IPCOSD_TEXT_INC_Y, IPCOSD_TRACK_LEN, 20,
        IDC_IPC_OSD_GREEN2,
        "",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        IPCOSD_TEXT_START_X+IPCOSD_TEXT_W+50+IPCOSD_TRACK_LEN+10, IPCOSD_TEXT_START_Y+7*IPCOSD_TEXT_INC_Y, 30, 20,
        IDC_IPC_OSD_VGREEN2,
        "0",
        0,
		WS_EX_TRANSPARENT
    },
     {
		"static",
        WS_CHILD | SS_LEFT | WS_VISIBLE,
       	IPCOSD_TEXT_START_X+IPCOSD_TEXT_W, IPCOSD_TEXT_START_Y+8*IPCOSD_TEXT_INC_Y, 20, IPCOSD_EDIT_H,
        IDC_STATIC,
        "蓝",
        0,
		WS_EX_TRANSPARENT
	},
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
        IPCOSD_TEXT_START_X+IPCOSD_TEXT_W+50,  IPCOSD_TEXT_START_Y+8*IPCOSD_TEXT_INC_Y, IPCOSD_TRACK_LEN, 20,
        IDC_IPC_OSD_BLUE2,
        "",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        IPCOSD_TEXT_START_X+IPCOSD_TEXT_W+50+IPCOSD_TRACK_LEN+10, IPCOSD_TEXT_START_Y+8*IPCOSD_TEXT_INC_Y, 30, 20,
        IDC_IPC_OSD_VBLUE2,
        "0",
        0,
		WS_EX_TRANSPARENT
    },
*/	
	{ 
        GUI_COMBOX,			
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
		IPCOSD_TEXT_START_X+IPCOSD_TEXT_W, IPCOSD_TEXT_START_Y+4*IPCOSD_TEXT_INC_Y, IPCOSD_COMBOX_W, GUI_COMBOX_H,
        IDC_IPC_OSD_TEXTCOLOR,
        "",
        80,
        WS_EX_USEPARENTFONT
    },

    {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		IPCOSD_TEXT_START_X, IPCOSD_TEXT_START_Y+5*IPCOSD_TEXT_INC_Y, IPCOSD_TEXT_W, IPCOSD_EDIT_H,
		IDC_STATIC,
		"显示文本",
		0,
		WS_EX_TRANSPARENT
	},
    {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		IPCOSD_TEXT_START_X + IPCOSD_TEXT_W ,  IPCOSD_TEXT_START_Y+5*IPCOSD_TEXT_INC_Y, 
		250, IPCOSD_EDIT_H,
        IDC_IPC_OSD_SHOWTEXT,
        "",
        0,        
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT| WS_EX_USEPARENTFONT|WS_EX_EDIT_HZINPUT
    },
	///////////////////////// GUI_BUTTON ///////////////////////////////////////
	CONFIG_THREE_WORDS_BUTTON("复制", IDC_IPC_OSD_COPY, IPCOSD_TEXT_START_X, IPCOSD_DLG_H - 80),
	CONFIG_THREE_WORDS_BUTTON("确定", IDC_IPC_OSD_SAVE, IPCOSD_DLG_W - 175, IPCOSD_DLG_H - 50),
	CONFIG_THREE_WORDS_BUTTON("取消", IDC_IPC_OSD_CANCEL, IPCOSD_DLG_W - 100 , IPCOSD_DLG_H - 50),

};


static DLGTEMPLATE IpcOsdDlg =
{
    WS_NONE,
    WS_EX_NONE,
    IPCOSD_DLG_X, IPCOSD_DLG_Y, IPCOSD_DLG_W, IPCOSD_DLG_H,
    "图像颜色",
    0, 0,
    TABLESIZE(ui_ipcosd_control), NULL,
    0
};

typedef struct _IPC_OSD_PALETTE{
	char color[8];
	Uint32 rgbValue;
}ipcosd_palette, *pipcosd_palette;


static ipcosd_palette osdColor[5]={
	{"黑", 0x000000},
	{"白", 0xffffff},
	{"蓝", 0xff0000},
	{"黄", 0x00ffff},
};


static osdParam ipcOsdParam = {0};

static void SetIpcOsdParam(HWND hDlg)
{
	int index = 0;
	char acText[64];

	ipcOsdParam.u8EnableTimeOsd = SendDlgItemMessage(hDlg, IDC_IPC_OSD_SHOWTIME, BM_GETCHECK, 0, 0);
	ipcOsdParam.u8EnableBitrateOsd = SendDlgItemMessage(hDlg, IDC_IPC_OSD_SHOWSTREAM, BM_GETCHECK, 0, 0);

	index = SendDlgItemMessage(hDlg, IDC_IPC_OSD_TIMECOLOR, CB_GETCURSEL, 0, 0);	
	ipcOsdParam.u32TimeColor = osdColor[index].rgbValue;
	
	index = SendDlgItemMessage(hDlg, IDC_IPC_OSD_TEXTCOLOR, CB_GETCURSEL, 0, 0);	
	ipcOsdParam.u32ChnColor = osdColor[index].rgbValue;

	GetDlgItemText(hDlg, IDC_IPC_OSD_SHOWTEXT, acText, sizeof(acText));	

	strncpy(ipcOsdParam.szChnName, acText, sizeof(ipcOsdParam.szChnName)-1);
	ipcOsdParam.szChnName[sizeof(ipcOsdParam.szChnName)-1] = '\0';
}

osdParam *GetIpcOsdParam(void )
{
	return &ipcOsdParam;
}

static int UpdateIpcOsdParam(HWND hDlg, int chn)
{
	IPNC_DEV_OSD_CFG_S	stOsdCfg = {0};
	int i=0;

	//获取OSD参数配置	
	if(0 > nvr_get_ipnc_param(chn,  IPNC_CMD_OSD_CFG, &stOsdCfg, sizeof(IPNC_DEV_OSD_CFG_S)))
	{
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_SHOWTIME, BM_SETCHECK, BST_UNCHECKED, 0);
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_SHOWSTREAM, BM_SETCHECK, BST_UNCHECKED, 0);
		
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_TIMECOLOR, CB_SETCURSEL, 1, 0);		
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_TEXTCOLOR, CB_SETCURSEL, 1, 0);
		SetDlgItemText(hDlg, IDC_IPC_OSD_SHOWTEXT, "");
		
		return -1;
	}
	//printf("@@@@@@@@@u8EnableTimeOsd: %d,u8EnableBitrateOsd: %d\r\n",stOsdCfg.u8EnableTimeOsd,stOsdCfg.u8EnableBitrateOsd);
	if(1 == stOsdCfg.u8EnableTimeOsd)
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_SHOWTIME, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_SHOWTIME, BM_SETCHECK, BST_UNCHECKED, 0);
		
	if(1 == stOsdCfg.u8EnableBitrateOsd)
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_SHOWSTREAM, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_SHOWSTREAM, BM_SETCHECK, BST_UNCHECKED, 0);
	//printf("!!!!!!!!!!!!!!!!stOsdCfg.u8ColorValidFlag: %d\r\n",stOsdCfg.u8ColorValidFlag);
	if(1/*stOsdCfg.u8ColorValidFlag*/)
	{
		for(i=0; i< TABLESIZE(osdColor); i++)
		{
			if(((stOsdCfg.u32TimeColor)&0x00ffffff) == osdColor[i].rgbValue)
				break;
		}
		if(i == TABLESIZE(osdColor))
			i = 0; 					//default: black
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_TIMECOLOR, CB_SETCURSEL, i, 0);		

		for(i=0; i< TABLESIZE(osdColor); i++)
		{
			if(((stOsdCfg.u32ChnColor)&0x00ffffff) == osdColor[i].rgbValue)
				break;
		}
		if(i == TABLESIZE(osdColor))
			i = 0;				//default: black
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_TEXTCOLOR, CB_SETCURSEL, i, 0);		
		
	}
	else
	{
		if(stOsdCfg.u8OsdColor >= 4)
			stOsdCfg.u8OsdColor = 0;
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_TIMECOLOR, CB_SETCURSEL, stOsdCfg.u8OsdColor, 0);		
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_TEXTCOLOR, CB_SETCURSEL, stOsdCfg.u8OsdColor, 0);		
	}
#if 0
	SendDlgItemMessage (hDlg, IDC_IPC_OSD_RED1, TBM_SETPOS, (u32TimeColor>>0)&0xff, 0);	
	SendDlgItemMessage (hDlg, IDC_IPC_OSD_GREEN1, TBM_SETPOS, (u32TimeColor>>8)&0xff, 0);	
	SendDlgItemMessage (hDlg, IDC_IPC_OSD_BLUE1, TBM_SETPOS, (u32TimeColor>>16)&0xff, 0);	

	SendDlgItemMessage (hDlg, IDC_IPC_OSD_RED2, TBM_SETPOS, (u32TitleColor>>0)&0xff, 0);	
	SendDlgItemMessage (hDlg, IDC_IPC_OSD_GREEN2, TBM_SETPOS, (u32TitleColor>>8)&0xff, 0);	
	SendDlgItemMessage (hDlg, IDC_IPC_OSD_BLUE2, TBM_SETPOS, (u32TitleColor>>16)&0xff, 0);	
#endif
	SetDlgItemText(hDlg, IDC_IPC_OSD_SHOWTEXT, stOsdCfg.szChnName);

	return 0;
}

static void InitIpcOsdParam(HWND hDlg, int chn)
{
	int i;
	char acCh[16];	
	
	SendDlgItemMessage(hDlg,IDC_IPC_OSD_CHANNEL, CB_RESETCONTENT, 0, (LPARAM)0);
	for (i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
	{	
		memset(acCh, 0, sizeof(acCh));
		sprintf(acCh, "%d", i+1);
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_CHANNEL, CB_ADDSTRING, 0, (LPARAM)LTR(acCh));		
	}
	SendDlgItemMessage(hDlg, IDC_IPC_OSD_CHANNEL, CB_SETCURSEL, chn, 0);

	SendDlgItemMessage(hDlg,IDC_IPC_OSD_TIMECOLOR, CB_RESETCONTENT, 0, (LPARAM)0);
	SendDlgItemMessage(hDlg,IDC_IPC_OSD_TEXTCOLOR, CB_RESETCONTENT, 0, (LPARAM)0);

	for (i = 0; i < TABLESIZE(osdColor); i++)
	{	
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_TIMECOLOR, CB_ADDSTRING, 0, (LPARAM)LTR(osdColor[i].color));
		SendDlgItemMessage(hDlg, IDC_IPC_OSD_TEXTCOLOR, CB_ADDSTRING, 0, (LPARAM)LTR(osdColor[i].color));		
	}
	SendDlgItemMessage(hDlg, IDC_IPC_OSD_TIMECOLOR, CB_SETCURSEL, 0, 0);		
	SendDlgItemMessage(hDlg, IDC_IPC_OSD_TEXTCOLOR, CB_SETCURSEL, 0, 0);	
	
	UpdateIpcOsdParam(hDlg, chn);
}

static int SaveIpcOsdParam(HWND hDlg, int chn)
{
	IPNC_DEV_OSD_CFG_S	stOsdCfg = {0};
	int index = 0;
	char acText[64];

	//获取OSD参数配置	
	if(0 > nvr_get_ipnc_param(chn,  IPNC_CMD_OSD_CFG, &stOsdCfg, sizeof(IPNC_DEV_OSD_CFG_S)))
		return -1;

	stOsdCfg.u8EnableTimeOsd = SendDlgItemMessage(hDlg, IDC_IPC_OSD_SHOWTIME, BM_GETCHECK, 0, 0);
	stOsdCfg.u8EnableBitrateOsd = SendDlgItemMessage(hDlg, IDC_IPC_OSD_SHOWSTREAM, BM_GETCHECK, 0, 0);

	index = SendDlgItemMessage(hDlg, IDC_IPC_OSD_TIMECOLOR, CB_GETCURSEL, 0, 0);	
	stOsdCfg.u32TimeColor = osdColor[index].rgbValue;
	
	index = SendDlgItemMessage(hDlg, IDC_IPC_OSD_TEXTCOLOR, CB_GETCURSEL, 0, 0);	
	stOsdCfg.u32ChnColor = osdColor[index].rgbValue;

	stOsdCfg.u8ColorValidFlag = 0; //写死
	
	GetDlgItemText(hDlg, IDC_IPC_OSD_SHOWTEXT, acText, sizeof(acText));	
	//printf("!!!!!!!!!!!!!!!!!!!!!acText: %s", acText);

	strncpy(stOsdCfg.szChnName, acText, sizeof(stOsdCfg.szChnName)-1);
	stOsdCfg.szChnName[sizeof(stOsdCfg.szChnName)-1] = '\0';
	
	if(0 > nvr_set_ipnc_param(chn, IPNC_CMD_OSD_CFG, 
					  		&stOsdCfg, sizeof(IPNC_DEV_OSD_CFG_S)))
		return -1;

	return 0;	
}

static int IpcOsdWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	static int chn = -1;
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		if (lParam >=0 &&lParam < 4)
			chn = lParam;
		InitIpcOsdParam(hDlg, chn);
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg, "图像设置");
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
		Gui_WriteTitleName(hdc, &title, LTR("OSD 设置"));
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
        //HWND ctrl = (HWND)(lParam);

		if (id == IDC_IPC_OSD_CHANNEL && code == CBN_SELCHANGE)
		{
			
			int nSelch;
			nSelch = SendDlgItemMessage (hDlg, IDC_IPC_OSD_CHANNEL, CB_GETCURSEL, 0, 0);
			if (nSelch < 0) return 0;
			UpdateIpcOsdParam(hDlg, nSelch);
			InvalidateRect(hDlg, NULL, TRUE);
			chn = nSelch;
		
			return 0;
		}

		switch (wParam) 
		{
			case IDC_IPC_OSD_COPY:
			{
				SetIpcOsdParam(hDlg);
	            CreateCopyDlgWin(hDlg, chn, EN_COPY_IPC_OSD);
				return 0;
			}				
			case IDC_IPC_OSD_SAVE:
			{
				SaveIpcOsdParam(hDlg, chn);
				SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
				return 0;
			}
			case IDC_IPC_OSD_CANCEL:
				PostMessage(hDlg, MSG_CLOSE, 0, 0);
				break;

		}
        break;
    }
    case MSG_CLOSE:
    {		
		chn = -1;
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
	if(1 == index) 
		ui_ipcosd_control[9].dwExStyle &= ~(WS_EX_EDIT_HZINPUT);
}

//远程设备配置
void CreateIpcOsdDlgWin(HWND hParent, int chn)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "IpcOsdDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_ipcosd_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_ipcosd_control, sizeof(ui_ipcosd_control));

	DLG_CAPTION_LANGUAGE_TRANS(ui_ipcosd_control)
	AdjustPositionDlgWin();
    IpcOsdDlg.controls = ui_ipcosd_control;
    
    DialogBoxIndirectParam (&IpcOsdDlg, hParent, IpcOsdWinProc, chn);
	memcpy(ui_ipcosd_control, ctrolTmp, sizeof(ui_ipcosd_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
	
}

