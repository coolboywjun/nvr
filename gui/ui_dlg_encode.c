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
#include "nvr_lib_interface.h"

#include "gui_core.h"
#include "ui_dlg_head.h"

#define ENCODEC_DLG_W	600
#define ENCODEC_DLG_H	400

#define ENCODEC_DLG_X	((NVR_SCREEN_WIDTH - ENCODEC_DLG_W)>>1)
#define ENCODEC_DLG_Y	((NVR_SCREEN_HEIGHT - ENCODEC_DLG_H)>>1)
#define ENCODEC_TEXT_W	100
#define ENCODEC_EDIT_W	120
#define ENCODEC_EDIT_H	20

#define ENCODE_TEXT_START_X	50
#define ENCODE_TEXT_START_Y 60
#define ENCODE_TEXT_ENC_Y	(ENCODEC_EDIT_H+12)

typedef enum
{
	IDC_ENCODEC_SAVE = 1900,
	IDC_ENCODEC_CANCEL,
	IDC_ENCODEC_COPY,
	IDL_ENCODE_CURRENT_CH,
	IDC_MAIN_STREAM_FRAMERATE,
	IDC_SUB_STREAM_FRAMERATE,
	IDL_MAIN_STREAM_FLOWTYPE,
	IDL_SUB_STREAM_FLOWTYPE,
	IDC_MAIN_STREAM_GOP,
	IDC_SUB_STREAM_GOP,
	IDC_MAIN_STREAM_BITRATE,
	IDC_SUB_STREAM_BITRATE,
}UI_ENCODE_SETTING_ID;


static CTRLDATA ui_encode_control[] =
{
	 {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		ENCODE_TEXT_START_X, ENCODE_TEXT_START_Y, 80, ENCODEC_EDIT_H,
		IDC_STATIC,
		"当前通道",
		0,
		WS_EX_TRANSPARENT
	},
	{   
        GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
		ENCODE_TEXT_START_X+110, ENCODE_TEXT_START_Y, 120, GUI_COMBOX_H,
        IDL_ENCODE_CURRENT_CH,
        "",
        80,
        WS_EX_USEPARENTFONT
    },
    {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		ENCODE_TEXT_START_X, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y, 90, ENCODEC_EDIT_H,
		IDC_STATIC,
		"主码流",
		0,
		WS_EX_TRANSPARENT
	},
	 {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		340, ENCODE_TEXT_START_Y+ENCODE_TEXT_ENC_Y, 90, ENCODEC_EDIT_H,
		IDC_STATIC,
		"次码流",
		0,
		WS_EX_TRANSPARENT
	},
	{
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		ENCODE_TEXT_START_X, ENCODE_TEXT_START_Y+2*ENCODE_TEXT_ENC_Y, 80, ENCODEC_EDIT_H,
		IDC_STATIC,
		"帧率",
		0,
		WS_EX_TRANSPARENT
	},
	 {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		ENCODE_TEXT_START_X+110, ENCODE_TEXT_START_Y+2*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, ENCODEC_EDIT_H,
        IDC_MAIN_STREAM_FRAMERATE,
        "25",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT	
    },
    {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		340, ENCODE_TEXT_START_Y+2*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, ENCODEC_EDIT_H,
		IDC_STATIC,
		"帧率",
		0,
		WS_EX_TRANSPARENT
	},
	 {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		340+110, ENCODE_TEXT_START_Y+2*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, ENCODEC_EDIT_H,
        IDC_SUB_STREAM_FRAMERATE,
        "25",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT	
    },
    {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		ENCODE_TEXT_START_X, ENCODE_TEXT_START_Y+3*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, ENCODEC_EDIT_H,
		IDC_STATIC,
		"I帧间隔",
		0,
		WS_EX_TRANSPARENT
	},
	 {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		ENCODE_TEXT_START_X+110, ENCODE_TEXT_START_Y+3*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, ENCODEC_EDIT_H,
        IDC_MAIN_STREAM_GOP,
        "25",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
    },
    {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		340, ENCODE_TEXT_START_Y+3*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, ENCODEC_EDIT_H,
		IDC_STATIC,
		"I帧间隔",
		0,
		WS_EX_TRANSPARENT
	},
	 {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		340+110, ENCODE_TEXT_START_Y+3*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, ENCODEC_EDIT_H,
        IDC_SUB_STREAM_GOP,
        "75",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
    },

	{
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		ENCODE_TEXT_START_X, ENCODE_TEXT_START_Y+4*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, ENCODEC_EDIT_H,
		IDC_STATIC,
		"码流(Kbps)",
		0,
		WS_EX_TRANSPARENT
	},
	 {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		ENCODE_TEXT_START_X+110, ENCODE_TEXT_START_Y+4*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, ENCODEC_EDIT_H,
        IDC_MAIN_STREAM_BITRATE,
        "4096",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT
    },
    {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		340, ENCODE_TEXT_START_Y+4*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, ENCODEC_EDIT_H,
		IDC_STATIC,
		"码流(Kbps)",
		0,
		WS_EX_TRANSPARENT
	},
	 {
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP,
		340+110, ENCODE_TEXT_START_Y+4*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, ENCODEC_EDIT_H,
        IDC_SUB_STREAM_BITRATE,
        "512",
        0,        
        WS_EX_USEPARENTFONT|WS_EX_EDIT_NUMINPUT	
    },

	{
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		ENCODE_TEXT_START_X, ENCODE_TEXT_START_Y+5*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, ENCODEC_EDIT_H,
		IDC_STATIC,
		"位率类型",
		0,
		WS_EX_TRANSPARENT
	},
	 {		
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP,
		ENCODE_TEXT_START_X+110, ENCODE_TEXT_START_Y+5*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, GUI_COMBOX_H,
        IDL_MAIN_STREAM_FLOWTYPE,
        "定码率",
        0,
        WS_EX_USEPARENTFONT
    },
    {
		CTRL_STATIC,
		WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		340, ENCODE_TEXT_START_Y+5*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, ENCODEC_EDIT_H,
		IDC_STATIC,
		"位率类型",
		0,
		WS_EX_TRANSPARENT
	},
	 {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP,
		340+110, ENCODE_TEXT_START_Y+5*ENCODE_TEXT_ENC_Y, ENCODEC_EDIT_W, GUI_COMBOX_H,
        IDL_SUB_STREAM_FLOWTYPE,
         "定码率",
        0,
        WS_EX_USEPARENTFONT
     },
	///////////////////////////// GUI_BUTTON ///////////////////////////////////
	CONFIG_THREE_WORDS_BUTTON("复制", IDC_ENCODEC_COPY, ENCODE_TEXT_START_X, ENCODEC_DLG_H - 80),
	CONFIG_THREE_WORDS_BUTTON("确定", IDC_ENCODEC_SAVE, ENCODEC_DLG_W - 175, ENCODEC_DLG_H - 50),
	CONFIG_THREE_WORDS_BUTTON("取消", IDC_ENCODEC_CANCEL, ENCODEC_DLG_W - 100 , ENCODEC_DLG_H - 50),

};


static DLGTEMPLATE EncodeDlg =
{
    WS_NONE,
    WS_EX_NONE,
    ENCODEC_DLG_X, ENCODEC_DLG_Y, ENCODEC_DLG_W, ENCODEC_DLG_H,
    "图像颜色",
    0, 0,
    TABLESIZE(ui_encode_control), NULL,
    0
};

typedef struct _gui_encode_cfg
{
  uint8_t	u8Gop;			//I帧间隔1-200
  uint8_t	u8FlowType;		//码率类型 0--定码流1--变码流
  uint8_t	u8FrameRate;	//编码帧率1-25/30
  uint8_t	u8Res[1];
  uint32_t	u32BitrateRate;	//编码码率 32kbps-16Mkbps
}gui_encode_cfg;

gui_encode_cfg stMainStreamCfg, stSubStreamCfg;

static void UpdateEncodeParam(HWND hDlg, int ch)
{
	char acText[32];
	IPNC_DEV_VENC_CFG_S stMainStreamCfg, stSubStreamCfg;

		//当前通道
	SendDlgItemMessage(hDlg, IDL_ENCODE_CURRENT_CH, CB_SETCURSEL, ch, 0);

	if(0 > nvr_get_ipnc_param(ch,IPNC_CMD_VMAIN_CFG, &stMainStreamCfg, sizeof(IPNC_DEV_VENC_CFG_S)))
	{
		memset(&stMainStreamCfg,0, sizeof(IPNC_DEV_VENC_CFG_S));
	}
	
	if(0 > nvr_get_ipnc_param(ch,IPNC_CMD_VSUB_CFG, &stSubStreamCfg, sizeof(IPNC_DEV_VENC_CFG_S)))
	{
		memset(&stSubStreamCfg,0, sizeof(IPNC_DEV_VENC_CFG_S));
	}
	
	//主码流图像参数
	SendDlgItemMessage(hDlg, IDL_MAIN_STREAM_FLOWTYPE, CB_SETCURSEL, stMainStreamCfg.u8FlowType, 0);
	sprintf(acText, "%d", stMainStreamCfg.u8Gop);
	SetDlgItemText(hDlg, IDC_MAIN_STREAM_GOP, acText);
	sprintf(acText, "%d", stMainStreamCfg.u32BitrateRate);
	SetDlgItemText(hDlg, IDC_MAIN_STREAM_BITRATE, acText);
	sprintf(acText, "%d", stMainStreamCfg.u8FrameRate);
	SetDlgItemText(hDlg, IDC_MAIN_STREAM_FRAMERATE, acText);
	//次码流图像参数
	SendDlgItemMessage(hDlg, IDL_SUB_STREAM_FLOWTYPE, CB_SETCURSEL, stSubStreamCfg.u8FlowType, 0);
	sprintf(acText, "%d", stSubStreamCfg.u8Gop);
	SetDlgItemText(hDlg, IDC_SUB_STREAM_GOP, acText);
	sprintf(acText, "%d", stSubStreamCfg.u32BitrateRate);
	SetDlgItemText(hDlg, IDC_SUB_STREAM_BITRATE, acText);
	sprintf(acText, "%d", stSubStreamCfg.u8FrameRate);
	SetDlgItemText(hDlg, IDC_SUB_STREAM_FRAMERATE, acText);
	
}

static void InitEncodeParam(HWND hDlg, int ch)
{
	int i;
	char acCh[16];	
	SendDlgItemMessage(hDlg,IDL_ENCODE_CURRENT_CH, CB_RESETCONTENT, 0, (LPARAM)0);
	for (i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
	{	
		memset(acCh, 0, sizeof(acCh));
		sprintf(acCh, "%d", i+1);
		printf("acCh=%d\r\n", i);
		SendDlgItemMessage(hDlg, IDL_ENCODE_CURRENT_CH, CB_ADDSTRING, 0, (LPARAM)LTR(acCh));		
	}
	
	SendDlgItemMessage(hDlg,IDL_MAIN_STREAM_FLOWTYPE, CB_RESETCONTENT, 0, (LPARAM)0);
	SendDlgItemMessage(hDlg, IDL_MAIN_STREAM_FLOWTYPE, CB_ADDSTRING, 0, (LPARAM)LTR("定码率"));
	SendDlgItemMessage(hDlg, IDL_MAIN_STREAM_FLOWTYPE, CB_ADDSTRING, 0, (LPARAM)LTR("变码率"));
	
	SendDlgItemMessage(hDlg,IDL_SUB_STREAM_FLOWTYPE, CB_RESETCONTENT, 0, (LPARAM)0);
	SendDlgItemMessage(hDlg, IDL_SUB_STREAM_FLOWTYPE, CB_ADDSTRING, 0, (LPARAM)LTR("定码率"));		
	SendDlgItemMessage(hDlg, IDL_SUB_STREAM_FLOWTYPE, CB_ADDSTRING, 0, (LPARAM)LTR("变码率"));		

	UpdateEncodeParam(hDlg, ch);
	
}

static void GetEncodeDlgParam(HWND hDlg, int ch)
{	
	char acText[8];
	int flowType;

	memset(&stMainStreamCfg, 0, sizeof(gui_encode_cfg));
	memset(&stSubStreamCfg, 0, sizeof(gui_encode_cfg));

	//main vcfg
	GetDlgItemText(hDlg, IDC_MAIN_STREAM_FRAMERATE, acText, sizeof(acText));
	stMainStreamCfg.u8FrameRate = atoi(acText);
	flowType = SendDlgItemMessage (hDlg, IDL_MAIN_STREAM_FLOWTYPE, CB_GETCURSEL, 0, 0);
	if (flowType < 0) return;
	stMainStreamCfg.u8FlowType = flowType;
	GetDlgItemText(hDlg, IDC_MAIN_STREAM_GOP, acText, sizeof(acText));	
	stMainStreamCfg.u8Gop = atoi(acText);
	GetDlgItemText(hDlg, IDC_MAIN_STREAM_BITRATE, acText, sizeof(acText));
	stMainStreamCfg.u32BitrateRate = atoi(acText);

	//sub vcfg
	GetDlgItemText(hDlg, IDC_SUB_STREAM_FRAMERATE, acText, sizeof(acText));
	stSubStreamCfg.u8FrameRate = atoi(acText);
	flowType = SendDlgItemMessage (hDlg, IDL_SUB_STREAM_FLOWTYPE, CB_GETCURSEL, 0, 0);
	if (flowType < 0) return;
	stSubStreamCfg.u8FlowType = flowType;
	GetDlgItemText(hDlg, IDC_SUB_STREAM_GOP, acText, sizeof(acText));	
	stSubStreamCfg.u8Gop = atoi(acText);
	GetDlgItemText(hDlg, IDC_SUB_STREAM_BITRATE, acText, sizeof(acText));
	stSubStreamCfg.u32BitrateRate = atoi(acText);	
	
}

int ExchangeEncodeDlgParam(IPNC_DEV_VENC_CFG_S *MainStreamCfg, IPNC_DEV_VENC_CFG_S *SubStreamCfg)
{
	if(NULL == MainStreamCfg || NULL == SubStreamCfg)
	return 0;

	MainStreamCfg->u8FrameRate = stMainStreamCfg.u8FrameRate;
	MainStreamCfg->u8FlowType = stMainStreamCfg.u8FlowType;
	MainStreamCfg->u8Gop = stMainStreamCfg.u8Gop;
	MainStreamCfg->u32BitrateRate = stMainStreamCfg.u32BitrateRate;
	
	SubStreamCfg->u8FrameRate = stSubStreamCfg.u8FrameRate;
	SubStreamCfg->u8FlowType = stSubStreamCfg.u8FlowType;
	SubStreamCfg->u8Gop = stSubStreamCfg.u8Gop;
	SubStreamCfg->u32BitrateRate = stSubStreamCfg.u32BitrateRate;
	return 1;
}

static void SaveEncodeCfg(HWND hDlg , int ch)
{	
	IPNC_DEV_VENC_CFG_S mainStreamCfg, subStreamCfg;
	int ret;
	
	if(0 > nvr_get_ipnc_param(ch,IPNC_CMD_VMAIN_CFG, &mainStreamCfg, sizeof(IPNC_DEV_VENC_CFG_S)))
		return;
	
	if(0 > nvr_get_ipnc_param(ch,IPNC_CMD_VSUB_CFG, &subStreamCfg, sizeof(IPNC_DEV_VENC_CFG_S)))
		return;

	GetEncodeDlgParam(hDlg, ch);
	ExchangeEncodeDlgParam(&mainStreamCfg, &subStreamCfg);
	//main vcfg
	nvr_set_ipnc_param(ch,IPNC_CMD_VMAIN_CFG, &mainStreamCfg, sizeof(IPNC_DEV_VENC_CFG_S));
	//sub vcfg
	nvr_set_ipnc_param(ch,IPNC_CMD_VSUB_CFG, &subStreamCfg, sizeof(IPNC_DEV_VENC_CFG_S));	
}

static void setEditLength(HWND hWnd)
{
	HWND frameHwnd,iframeHwnd;

	frameHwnd = GetDlgItem(hWnd, IDC_MAIN_STREAM_FRAMERATE);
	SendMessage(frameHwnd, EM_LIMITTEXT, 3, 0L);

	frameHwnd = GetDlgItem(hWnd, IDC_SUB_STREAM_FRAMERATE);
	SendMessage(frameHwnd, EM_LIMITTEXT, 3, 0L);
	int i;
	for(i=0; i<4; i++)
	{
		iframeHwnd = GetDlgItem(hWnd, IDC_MAIN_STREAM_GOP+i);
		SendMessage(iframeHwnd, EM_LIMITTEXT, PORT_TEXT_LIMIT, 0L);
	}

}

static int EncodecWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	static int chn = -1;
	
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		if (lParam >=0 &&lParam < 4)
			chn = lParam;
		setEditLength(hDlg);
		InitEncodeParam(hDlg, chn);
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg,  "编码设置");
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
		Gui_WriteTitleName(hdc, &title, LTR("编码设置"));
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
		
		if (id == IDL_ENCODE_CURRENT_CH)
		{		
			if (code == CBN_SELCHANGE)
			{	
				int nSelch;				
				nSelch = SendDlgItemMessage (hDlg, IDL_ENCODE_CURRENT_CH, CB_GETCURSEL, 0, 0);
				if (nSelch < 0) return 0;
				UpdateEncodeParam(hDlg, nSelch);
				chn = nSelch; 
				return 0;
			}
			break;
		}
		
		switch (wParam) 
		{
			case IDC_ENCODEC_COPY:
			{
				GetEncodeDlgParam(hDlg, chn);
	            CreateCopyDlgWin(hDlg, chn, EN_COPY_ENCODE_PARAM);
				return 0;
			}	
			case IDC_ENCODEC_SAVE:
			{
				SaveEncodeCfg(hDlg, chn);
				//PostMessage(hDlg, MSG_CLOSE, 0, 0);
				SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
				return 0;
			}
			case IDC_ENCODEC_CANCEL:
			{
				PostMessage(hDlg, MSG_CLOSE, 0, 0);
				return 0;
			}
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

void CreateEncodeDlgWin(HWND hParent, int chn)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "EncodeDlg" , 0 , 0);
#endif	
	PCTRLDATA ctrolTmp;
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_encode_control));
	if(NULL == ctrolTmp)
		return ;

	memcpy(ctrolTmp, ui_encode_control, sizeof(ui_encode_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_encode_control)
	
    EncodeDlg.controls = ui_encode_control;
    DialogBoxIndirectParam (&EncodeDlg, hParent, EncodecWinProc, chn);
    memcpy(ui_encode_control, ctrolTmp,sizeof(ui_encode_control));
    free(ctrolTmp);
}

