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

//#define __SET_OSD_PARA  //设置alpha和colorkey

#define COLOR_DLG_W	600
#define COLOR_DLG_H	400

#define COLOR_DLG_X	((NVR_SCREEN_WIDTH - COLOR_DLG_W)>>1)
#define COLOR_DLG_Y	((NVR_SCREEN_HEIGHT - COLOR_DLG_H)>>1)
#define COLOR_TEXT_W	70
#define COLOR_EDIT_W	80
#define COLOR_EDIT_H	20

#define COLOR_TEXT_START_X	50
#define COLOR_TEXT_START_Y	(60+180)
#define COLOR_TEXT_INC_Y	(COLOR_EDIT_H+10)
#define COLOR_TRACKBAR_START_X (COLOR_TEXT_START_X + COLOR_TEXT_W+10)
#define COLOR_TRACKBAR_START_Y  COLOR_TEXT_START_Y//50
#define COLOR_TRACKBAR_W		400
#define COLOR_TRACKBAR_H		21

#define VALUE_TEXT_START_X  (COLOR_TRACKBAR_START_X + COLOR_TRACKBAR_W+20)
#define VALUE_TEXT_START_Y	COLOR_TEXT_START_Y
/////////////////////////////////////////////////////

#define ENCODE_TEXT_START_X	50
#define ENCODE_TEXT_START_Y 60
#define ENCODE_TEXT_ENC_Y	(COLOR_EDIT_H+12)

typedef enum
{
	IDC_STATIC_COLOR_BRIGHT = 1400,
	IDC_STATIC_COLOR_CONTRAST,
	IDC_STATIC_COLOR_SATURATION,
	IDC_STATIC_COLOR_GRAY,
	IDC_STATIC_COLOR_SHARP,
	IDC_COLOR_BRIGHT_TRACKBAR,
	IDC_COLOR_CONTRAST_TRACKBAR,
	IDC_COLOR_SATURATION_TRACKBAR,
	IDC_COLOR_GRAY_TRACKBAR,
	IDC_COLOR_SHARP_TRACKBAR,
	IDC_STATIC_COLOR_BRIGHT_VALUE,
	IDC_STATIC_COLOR_CONTRAST_VALUE,
	IDC_STATIC_COLOR_SATURATION_VALUE,
	IDC_STATIC_COLOR_GRAY_VALUE,
	IDC_STATIC_COLOR_SHARP_VALUE,
	IDC_COLOR_DEFAULT,
	IDC_COLOR_SAVE,
	IDC_COLOR_CANCEL,
	IDC_STATIC_FREQ,
	IDL_POWER_FREQ,
	IDC_SCENCE,
	IDL_SCENCE,
	IDC_STATIC_IRIS_MODE,
	IDL_IRIS_MODE,
	IDC_COLOR_COPY,
	IDC_SET_TIMER,
	IDL_COLOR_CURRENT_CH,
}UI_COLOR_SETTING_ID;

#ifdef __SET_OSD_PARA
#define IDC_SET_GRAPHIC_PARA				740
#endif

static CTRLDATA ui_color_control[] =
{
	 {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		ENCODE_TEXT_START_X, ENCODE_TEXT_START_Y, COLOR_TEXT_W, COLOR_EDIT_H,
		IDC_STATIC,
		"当前通道",
		0,
		WS_EX_TRANSPARENT
	},
	{ 
        GUI_COMBOX,			
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
		ENCODE_TEXT_START_X+80, ENCODE_TEXT_START_Y, 100, GUI_COMBOX_H,
        IDL_COLOR_CURRENT_CH,
        "",
        80,
        WS_EX_USEPARENTFONT
    },	
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        COLOR_TEXT_START_X, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y, COLOR_TEXT_W, COLOR_EDIT_H,
        IDC_STATIC_COLOR_BRIGHT,
        "亮度",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
        COLOR_TRACKBAR_START_X,  ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y, COLOR_TRACKBAR_W, COLOR_TRACKBAR_H,
        IDC_COLOR_BRIGHT_TRACKBAR,
        "",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        VALUE_TEXT_START_X, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y, COLOR_TEXT_W, COLOR_EDIT_H,
        IDC_STATIC_COLOR_BRIGHT_VALUE,
        "0",
        0,
		WS_EX_TRANSPARENT
    },
    //////////////////////////////////////////////////////////////////
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        COLOR_TEXT_START_X, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + COLOR_TEXT_INC_Y, COLOR_TEXT_W, COLOR_EDIT_H,
        IDC_STATIC_COLOR_CONTRAST,
        "对比度",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
		COLOR_TRACKBAR_START_X,  ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + COLOR_TEXT_INC_Y, COLOR_TRACKBAR_W, COLOR_TRACKBAR_H,
        IDC_COLOR_CONTRAST_TRACKBAR,
        "",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        VALUE_TEXT_START_X, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + COLOR_TEXT_INC_Y, COLOR_TEXT_W, COLOR_EDIT_H,
        IDC_STATIC_COLOR_CONTRAST_VALUE,
        "0",
        0,
		WS_EX_TRANSPARENT
    },
    ///////////////////////////////////////////////
	  {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		COLOR_TEXT_START_X, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + 2*COLOR_TEXT_INC_Y, COLOR_TEXT_W, COLOR_EDIT_H,
        IDC_STATIC_COLOR_SATURATION,
        "饱和度",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
		COLOR_TRACKBAR_START_X,  ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + 2*COLOR_TEXT_INC_Y, COLOR_TRACKBAR_W, COLOR_TRACKBAR_H,
        IDC_COLOR_SATURATION_TRACKBAR,
        "",
        0,
		WS_EX_TRANSPARENT
    },
      {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        VALUE_TEXT_START_X, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + 2*COLOR_TEXT_INC_Y, COLOR_TEXT_W, COLOR_EDIT_H,
        IDC_STATIC_COLOR_SATURATION_VALUE,
        "0",
        0,
		WS_EX_TRANSPARENT
    },
    ///////////////////////////////////////////
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		COLOR_TEXT_START_X, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + 3*COLOR_TEXT_INC_Y, COLOR_TEXT_W, COLOR_EDIT_H,
        IDC_STATIC_COLOR_SHARP,
        "锐度",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_TRACKBAR,
		WS_VISIBLE | TBS_NOTIFY,
		COLOR_TRACKBAR_START_X,  ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + 3*COLOR_TEXT_INC_Y, COLOR_TRACKBAR_W, COLOR_TRACKBAR_H,
        IDC_COLOR_SHARP_TRACKBAR,
        "",
        0,
		WS_EX_TRANSPARENT
    },
     {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        VALUE_TEXT_START_X, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + 3*COLOR_TEXT_INC_Y, COLOR_TEXT_W, COLOR_EDIT_H,
        IDC_STATIC_COLOR_SHARP_VALUE,
        "0",
        0,
		WS_EX_TRANSPARENT
    },
    ////////////////////////////////////////////////////////////////
	{
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		COLOR_TEXT_START_X, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + 5*COLOR_TEXT_INC_Y+5, COLOR_TEXT_W, COLOR_EDIT_H,
		IDC_STATIC_FREQ,
		"照明频率",
		0,
		WS_EX_TRANSPARENT
	},
	 {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
		COLOR_TEXT_START_X + 80, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + 5*COLOR_TEXT_INC_Y+5, 100, GUI_COMBOX_H,
        IDL_POWER_FREQ,
        "",
        0,
        WS_EX_USEPARENTFONT
    },
/*  {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		COLOR_TEXT_START_X + 180, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + 5*COLOR_TEXT_INC_Y+5, COLOR_TEXT_W, COLOR_EDIT_H,
		IDC_SCENCE,
		"场景",
		0,
		WS_EX_TRANSPARENT
	},
	 {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
		COLOR_TEXT_START_X + 220, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + 5*COLOR_TEXT_INC_Y+5, 80, GUI_COMBOX_H,
        IDL_SCENCE,//场景
        "",
        0,
        WS_EX_USEPARENTFONT
    },
*/  {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		COLOR_TEXT_START_X + 200, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + 5*COLOR_TEXT_INC_Y+5, COLOR_TEXT_W, COLOR_EDIT_H,
		IDC_STATIC_IRIS_MODE,
		"红外模式",
		0,
		WS_EX_TRANSPARENT
	},
	 {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
		COLOR_TEXT_START_X + 400-130, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + 5*COLOR_TEXT_INC_Y+5, 100, GUI_COMBOX_H,
        IDL_IRIS_MODE,//红外模式
        "",
        80,
        WS_EX_USEPARENTFONT
    },
	///////////////////////// GUI_BUTTON ///////////////////////////////////////
	
	CONFIG_FOUR_WORDS_BUTTON("设置定时", IDC_SET_TIMER, COLOR_TEXT_START_X + 400, 
	ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y + 5*COLOR_TEXT_INC_Y+2),

	CONFIG_THREE_WORDS_BUTTON("复制", IDC_COLOR_COPY, COLOR_TEXT_START_X, COLOR_DLG_H - 80),
	CONFIG_THREE_WORDS_BUTTON("默认", IDC_COLOR_DEFAULT, COLOR_TEXT_START_X + 75, COLOR_DLG_H - 80),

#ifdef __SET_OSD_PARA	
	CONFIG_FOUR_WORDS_BUTTON("OSD参数", IDC_SET_GRAPHIC_PARA, COLOR_TEXT_START_X + 180, COLOR_DLG_H - 80),

#endif
	CONFIG_THREE_WORDS_BUTTON("确定", IDC_COLOR_SAVE, COLOR_DLG_W - 175, COLOR_DLG_H - 50),
	CONFIG_THREE_WORDS_BUTTON("取消", IDC_COLOR_CANCEL, COLOR_DLG_W - 100 , COLOR_DLG_H - 50),

};


static DLGTEMPLATE ColorDlg =
{
    WS_NONE,
    WS_EX_NONE,
    COLOR_DLG_X, COLOR_DLG_Y, COLOR_DLG_W, COLOR_DLG_H,
    "图像颜色",
    0, 0,
    TABLESIZE(ui_color_control), NULL,
    0
};


static void UpdateColorParam(HWND hDlg,int ch)
{
	IPNC_DEV_IMA_CFG_S stImgCfg;
	int nBright = 0, nContrast = 0, nSaturation = 0, nSharpness = 0;
	//int nHue = 0;
	if(0 > nvr_get_ipnc_param(ch,IPNC_CMD_IMA_CFG,&stImgCfg, sizeof(IPNC_DEV_IMA_CFG_S)))
		memset(&stImgCfg, 0, sizeof(IPNC_DEV_IMA_CFG_S));
		
	nBright = stImgCfg.struBrightness.u8Value;
	nContrast = stImgCfg.struContrast.u8Value;
	nSaturation = stImgCfg.struSaturation.u8Value;
	//nHue= stImgCfg.struHue.u8Value;
	nSharpness = stImgCfg.struSharpness.u8Value;
	SendDlgItemMessage (hDlg, IDC_COLOR_BRIGHT_TRACKBAR, TBM_SETPOS, nBright, 0);	
	SendDlgItemMessage (hDlg, IDC_COLOR_CONTRAST_TRACKBAR, TBM_SETPOS, nContrast, 0);
	SendDlgItemMessage (hDlg, IDC_COLOR_SATURATION_TRACKBAR, TBM_SETPOS, nSaturation, 0);
	//SendDlgItemMessage (hDlg, IDC_COLOR_GRAY_TRACKBAR, TBM_SETPOS, nHue, 0);
	SendDlgItemMessage (hDlg, IDC_COLOR_SHARP_TRACKBAR, TBM_SETPOS, nSharpness, 0);

	IPNC_DEV_3A_CFG_S st3ACfg;
	IPNC_DEV_IRCFMODE_CFG_S stIrcfCfg;
	if(0 > nvr_get_ipnc_param(ch, IPNC_CMD_3A_CFG, &st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
		memset(&st3ACfg, 0, sizeof(st3ACfg));
	
	SendDlgItemMessage(hDlg, IDL_POWER_FREQ, CB_SETCURSEL, st3ACfg.struPwdFreq.u8Value, 0);		
	//SendDlgItemMessage(hDlg, IDL_SCENCE, CB_SETCURSEL, st3ACfg.struScene.u8Value, 0);	
	
	if(0 > nvr_get_ipnc_param(ch, IPNC_CMD_IRCFMODE_CFG, &stIrcfCfg, sizeof(IPNC_DEV_IRCFMODE_CFG_S)))
		memset(&stIrcfCfg, 0, sizeof(stIrcfCfg));
	SendDlgItemMessage(hDlg, IDL_IRIS_MODE, CB_SETCURSEL, stIrcfCfg.u8Mode, 0);	
	if(stIrcfCfg.u8Mode == 3) //定时模式
	{
		EnableDlgItem(hDlg, IDC_SET_TIMER, TRUE);
	}
	else
	{
		EnableDlgItem(hDlg, IDC_SET_TIMER, FALSE);
	}
}

static void InitColorParam(HWND hDlg , int ch)
{	
	int n = 0;
	for (n = 0; n < 5; n++)
	{
		SendDlgItemMessage (hDlg, IDC_COLOR_BRIGHT_TRACKBAR+n, TBM_SETRANGE, 0, 255);
	    SendDlgItemMessage (hDlg, IDC_COLOR_BRIGHT_TRACKBAR+n, TBM_SETLINESIZE, 1, 0);
	    SendDlgItemMessage (hDlg, IDC_COLOR_BRIGHT_TRACKBAR+n, TBM_SETPAGESIZE, 10, 0);
	    SendDlgItemMessage (hDlg, IDC_COLOR_BRIGHT_TRACKBAR+n, TBM_SETTICKFREQ, 255, 0);
	}
	
	SendDlgItemMessage(hDlg,IDL_POWER_FREQ, CB_RESETCONTENT, 0, (LPARAM)0);
	SendDlgItemMessage(hDlg, IDL_POWER_FREQ, CB_ADDSTRING, 0, (LPARAM)"50Hz");
	SendDlgItemMessage(hDlg, IDL_POWER_FREQ, CB_ADDSTRING, 0, (LPARAM)"60Hz");
	//SendDlgItemMessage(hDlg, IDL_POWER_FREQ, CB_SETCURSEL, 0, 0);
#if 0
	SendDlgItemMessage(hDlg,IDL_SCENCE, CB_RESETCONTENT, 0, (LPARAM)0);
	SendDlgItemMessage(hDlg, IDL_SCENCE, CB_ADDSTRING, 0, (LPARAM)LTR("自动"));
	SendDlgItemMessage(hDlg, IDL_SCENCE, CB_ADDSTRING, 0, (LPARAM)LTR("室内"));	
	SendDlgItemMessage(hDlg, IDL_SCENCE, CB_ADDSTRING, 0, (LPARAM)LTR("室外"));
	//SendDlgItemMessage(hDlg, IDL_SCENCE, CB_SETCURSEL, 0, 0);	
#endif
	SendDlgItemMessage(hDlg,IDL_IRIS_MODE, CB_RESETCONTENT, 0, (LPARAM)0);
	SendDlgItemMessage(hDlg, IDL_IRIS_MODE, CB_ADDSTRING, 0, (LPARAM)LTR("自动"));
	SendDlgItemMessage(hDlg, IDL_IRIS_MODE, CB_ADDSTRING, 0, (LPARAM)LTR("白天模式"));	
	SendDlgItemMessage(hDlg, IDL_IRIS_MODE, CB_ADDSTRING, 0, (LPARAM)LTR("夜晚模式"));	
	SendDlgItemMessage(hDlg, IDL_IRIS_MODE, CB_ADDSTRING, 0, (LPARAM)LTR("定时模式"));
	//SendDlgItemMessage(hDlg, IDL_IRIS_MODE, CB_SETCURSEL, 0, 0);

	UpdateColorParam(hDlg, ch);
}

static void InitChannelParam(HWND hDlg, int ch)
{
	int i;
	char acCh[8];	
	SendDlgItemMessage(hDlg,IDL_COLOR_CURRENT_CH, CB_RESETCONTENT, 0, (LPARAM)0);
	for (i = 0; i < 4; i++)
	{	
		sprintf(acCh, "%d", i+1);
		SendDlgItemMessage(hDlg, IDL_COLOR_CURRENT_CH, CB_ADDSTRING, 0, (LPARAM)LTR(acCh));		
	}
	SendDlgItemMessage(hDlg, IDL_COLOR_CURRENT_CH, CB_SETCURSEL, ch, 0);
}


static void TrackbarChanged(HWND hDlg, int chnNo,int id, int code, HWND ctrl)
{	
	char acCurPosBuffer[8];
	static int nCurrentPos = 0;	
	IPNC_DEV_IMA_CFG_S stImgCfg;
	
	int nTrackbarIndex = id - IDC_COLOR_BRIGHT_TRACKBAR;
	
	if (code == TBN_CHANGE)
	{
		nCurrentPos = SendDlgItemMessage (hDlg, id, TBM_GETPOS, 0, 0);		
		sprintf(acCurPosBuffer, "%d", nCurrentPos);
		SendDlgItemMessage (hDlg, IDC_STATIC_COLOR_BRIGHT_VALUE+nTrackbarIndex,  MSG_SETTEXT, 0, (LPARAM)acCurPosBuffer);
	}

	if (code == TBN_STOPDRAG)
	{
		if(0 > nvr_get_ipnc_param(chnNo,IPNC_CMD_IMA_CFG,&stImgCfg, sizeof(IPNC_DEV_IMA_CFG_S)))
			return;
		if (id == IDC_COLOR_BRIGHT_TRACKBAR)
			stImgCfg.struBrightness.u8Value = nCurrentPos;
		else if (id == IDC_COLOR_CONTRAST_TRACKBAR)
			stImgCfg.struContrast.u8Value = nCurrentPos;
		else if (id == IDC_COLOR_SATURATION_TRACKBAR)
			stImgCfg.struSaturation.u8Value = nCurrentPos;
	//	else if (id == IDC_COLOR_GRAY_TRACKBAR)	
	//		stImgCfg.struHue.u8Value = nCurrentPos;
		else if (id == IDC_COLOR_SHARP_TRACKBAR)
			stImgCfg.struSharpness.u8Value = nCurrentPos;

		printf("chnNo = %d id = %d value = %d\r\n",chnNo, id , nCurrentPos);
		nvr_set_ipnc_param(chnNo, IPNC_CMD_IMA_CFG, &stImgCfg, sizeof(IPNC_DEV_IMA_CFG_S));
		nCurrentPos = 0;
	}
	
}


static void	ComboxBoxChanged(HWND hDlg, int chnNo,int id, int code, HWND ctrl)
{
	if (code == CBN_SELCHANGE)
	{
		if(id == IDL_POWER_FREQ)
		{
			IPNC_DEV_3A_CFG_S st3ACfg;
			int freq;
			// int scence;
			if(0 > nvr_get_ipnc_param(chnNo,  IPNC_CMD_3A_CFG, &st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
			{
				printf("[ERROR]-------->>>>%s %d\n",__func__,__LINE__);
				return;
			}

			freq = SendDlgItemMessage (hDlg, IDL_POWER_FREQ, CB_GETCURSEL, 0, 0);
			if (freq < 0) return;

			//ence = SendDlgItemMessage (hDlg, IDL_SCENCE, CB_GETCURSEL, 0, 0);
			// (scence < 0) return;
			
			st3ACfg.struPwdFreq.u8Value = freq;
			//st3ACfg.struScene.u8Value = scence;
			if(0>nvr_set_ipnc_param(chnNo, IPNC_CMD_3A_CFG, &st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
			{
				printf("[error]-------->>>>set color param failed!\n");
			}
		}
		else if(id == IDL_IRIS_MODE)
		{
			IPNC_DEV_IRCFMODE_CFG_S stIrcfCfg;
			int irmode;
			if(0 > nvr_get_ipnc_param(chnNo,  IPNC_CMD_IRCFMODE_CFG, &stIrcfCfg, sizeof(IPNC_DEV_IRCFMODE_CFG_S)))
			{
				printf("[ERROR]-------->>>>%s %d\n",__func__,__LINE__);
				return;
			}
			
			irmode = SendDlgItemMessage (hDlg, IDL_IRIS_MODE, CB_GETCURSEL, 0, 0);
			if (irmode < 0) 
				return;
			else if(irmode == 3)
			{
				EnableDlgItem(hDlg, IDC_SET_TIMER, TRUE);
			}
			else
			{
				EnableDlgItem(hDlg, IDC_SET_TIMER, FALSE);
			}
			stIrcfCfg.u8Mode = irmode;
			
			if(0>nvr_set_ipnc_param(chnNo, IPNC_CMD_IRCFMODE_CFG, &stIrcfCfg, sizeof(IPNC_DEV_IRCFMODE_CFG_S)))
			{
				printf("[error]-------->>>>set color param failed!\n");
			}
		}

	}

}

static void SetDefaultParam(HWND hDlg, int ch, 
			IPNC_DEV_IMA_CFG_S *pDefImaCfg, IPNC_DEV_3A_CFG_S *pDef3aCfg,
			IPNC_DEV_IRCFMODE_CFG_S *pIrcfCfg)
{
	int nBright = 0, nContrast = 0, nSaturation = 0, nSharpness = 0;
	//int nHue = 0;
	
	nBright = pDefImaCfg->struBrightness.u8Default;
	nContrast = pDefImaCfg->struContrast.u8Default;
	nSaturation = pDefImaCfg->struSaturation.u8Default;
	//nHue= pDefImaCfg->struHue.u8Default;
	nSharpness = pDefImaCfg->struSharpness.u8Default;
	SendDlgItemMessage (hDlg, IDC_COLOR_BRIGHT_TRACKBAR, TBM_SETPOS, nBright, 0);	
	SendDlgItemMessage (hDlg, IDC_COLOR_CONTRAST_TRACKBAR, TBM_SETPOS, nContrast, 0);
	SendDlgItemMessage (hDlg, IDC_COLOR_SATURATION_TRACKBAR, TBM_SETPOS, nSaturation, 0);
	//SendDlgItemMessage (hDlg, IDC_COLOR_GRAY_TRACKBAR, TBM_SETPOS, nHue, 0);
	SendDlgItemMessage (hDlg, IDC_COLOR_SHARP_TRACKBAR, TBM_SETPOS, nSharpness, 0);

	SendDlgItemMessage(hDlg, IDL_POWER_FREQ, CB_SETCURSEL, pDef3aCfg->struPwdFreq.u8Default, 0);		
	//SendDlgItemMessage(hDlg, IDL_SCENCE, CB_SETCURSEL, pDef3aCfg->struScene.u8Default, 0);	
	SendDlgItemMessage(hDlg, IDL_IRIS_MODE, CB_SETCURSEL, pIrcfCfg->u8Mode, 0);	
	if(pIrcfCfg->u8Mode == 3) //定时模式
	{
		EnableDlgItem(hDlg, IDC_SET_TIMER, TRUE);
	}
	else
	{
		EnableDlgItem(hDlg, IDC_SET_TIMER, FALSE);
	}

	InvalidateRect(hDlg, NULL, FALSE);
}

static void SetDefultColor(HWND hDlg , int ch)
{	
	IPNC_DEV_IMA_CFG_S stDefImaCfg ={0};
	IPNC_DEV_3A_CFG_S stDef3aCfg = {0};
	IPNC_DEV_IRCFMODE_CFG_S stIrcfCfg = {0};
	nvr_set_ipnc_ima_def_param(ch, &stDefImaCfg);
	nvr_set_ipnc_3a_def_param(ch, &stDef3aCfg);
	stIrcfCfg.u32Size = sizeof(stIrcfCfg);
	stIrcfCfg.u8Mode = 0;	// 自动
	nvr_set_ipnc_param(ch, IPNC_CMD_IRCFMODE_CFG, &stIrcfCfg, sizeof(IPNC_DEV_IRCFMODE_CFG_S));
	SetDefaultParam(hDlg, ch, &stDefImaCfg, &stDef3aCfg, &stIrcfCfg);
}

static void SaveColorCfg(HWND hDlg , int ch)
{	
	IPNC_DEV_3A_CFG_S st3ACfg;
	IPNC_DEV_IRCFMODE_CFG_S stIrcfCfg;
	int freq, irmode;
	//int scence;
	freq = SendDlgItemMessage (hDlg, IDL_POWER_FREQ, CB_GETCURSEL, 0, 0);
	if (freq < 0) return;
		
	//scence = SendDlgItemMessage (hDlg, IDL_SCENCE, CB_GETCURSEL, 0, 0);
	//if (scence < 0) return;

	irmode = SendDlgItemMessage (hDlg, IDL_IRIS_MODE, CB_GETCURSEL, 0, 0);
	if (irmode < 0) return;

	if(0 > nvr_get_ipnc_param(ch,  IPNC_CMD_3A_CFG, &st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
		return;
	if(0 > nvr_get_ipnc_param(ch,  IPNC_CMD_IRCFMODE_CFG, &stIrcfCfg, sizeof(IPNC_DEV_IRCFMODE_CFG_S)))
		return;

	st3ACfg.struPwdFreq.u8Value = freq;
	//st3ACfg.struScene.u8Value = scence;
	stIrcfCfg.u8Mode = irmode;

	//printf("freq = %d, scence = %d, irmode = %d\r\n", freq, scence, irmode);
	if(0>nvr_set_ipnc_param(ch, IPNC_CMD_3A_CFG, &st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
	{
		printf("[error]-------->>>>set color param failed!\n");
	}

	if(0>nvr_set_ipnc_param(ch, IPNC_CMD_IRCFMODE_CFG, &stIrcfCfg, sizeof(IPNC_DEV_IRCFMODE_CFG_S)))
	{
		printf("[error]-------->>>>set color IPNC_CMD_IRCFMODE_CFG param failed!\n");
	}

}


static int ColorWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	static int chn = -1;
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		if (lParam >=0 &&lParam < 4)
			chn = lParam;
		printf("chn = %d\n",chn);
		InitColorParam(hDlg, chn);
		InitChannelParam(hDlg, chn);
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
		Gui_WriteTitleName(hdc, &title, LTR("图像设置"));
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
        HWND ctrl = (HWND)(lParam);
		if ((id >= IDC_COLOR_BRIGHT_TRACKBAR)&&(id <= IDC_COLOR_SHARP_TRACKBAR))
		{
            TrackbarChanged(hDlg, chn, id, code, ctrl);
			return 0;
		}else if((id == IDL_IRIS_MODE)/*||(id == IDL_SCENCE)*/||(id == IDL_POWER_FREQ))
		{
			ComboxBoxChanged(hDlg, chn, id, code, ctrl);
			return 0;
		}
		else if (id == IDL_COLOR_CURRENT_CH)
		{
			if (code == CBN_SELCHANGE)
			{
				int nSelch;
				nSelch = SendDlgItemMessage (hDlg, IDL_COLOR_CURRENT_CH, CB_GETCURSEL, 0, 0);
				if (nSelch < 0) return 0;
				UpdateColorParam(hDlg, nSelch);
				chn = nSelch;
			}			
			return 0;
		}
		
		switch (wParam) 
		{
			case IDC_SET_TIMER:
			{
				CreateSetTimer(hDlg,chn);
				return 0;
			}
			case IDC_COLOR_COPY:
			{
	            CreateCopyDlgWin(hDlg, chn, EN_COPY_PIC_PARAM);
				return 0;
			}			
			case IDC_COLOR_DEFAULT:
			{
				SetDefultColor(hDlg, chn);
				break;
			}
			case IDC_COLOR_SAVE:
			{
				SaveColorCfg(hDlg, chn);
				//PostMessage(hDlg, MSG_CLOSE, 0, 0);
				SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
				return 0;
			}
			case IDC_COLOR_CANCEL:
			{
				PostMessage(hDlg, MSG_CLOSE, 0, 0);
				return 0;
			}
#ifdef __SET_OSD_PARA	
			case IDC_SET_GRAPHIC_PARA:
			{
				CreateAlphaColorkeyDlgWin(hDlg);
				return 0;
			}
#endif
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

//远程设备配置
void CreateColorDlgWin(HWND hParent, int chn)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "ColorDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_color_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_color_control, sizeof(ui_color_control));

	DLG_CAPTION_LANGUAGE_TRANS(ui_color_control)
    ColorDlg.controls = ui_color_control;
    
    DialogBoxIndirectParam (&ColorDlg, hParent, ColorWinProc, chn);
	memcpy(ui_color_control, ctrolTmp, sizeof(ui_color_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

