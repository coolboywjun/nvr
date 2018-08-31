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
#include "ui_dlg_head.h"
#include "gui_core.h"

#define COPY_DLG_W	500
#define COPY_DLG_H	150
#define COPY_DLG_X	((NVR_SCREEN_WIDTH - COPY_DLG_W)>>1)
#define COPY_DLG_Y	((NVR_SCREEN_HEIGHT - COPY_DLG_H)>>1)
#define COPY_TEXT_W	60
#define COPY_EDIT_W	80
#define COPY_EDIT_H	20


#define COPY_CHECK_START_X	70
#define COPY_CHECK_START_Y	60
#define COPY_CHECK_W		90
#define COPY_CHECK_H		20
#define COPY_CHECK_INC_X	(COPY_CHECK_W + 10)

typedef enum
{
	IDC_CHECK_CH1 = 1500,
	IDC_CHECK_CH2,
	IDC_CHECK_CH3,
	IDC_CHECK_CH4,
	IDC_COPY_SAVE,
	IDC_COPY_CANCEL,
}UI_COPY_ID;

//static BITMAP  bmp_bkgnd;
//static RECT CopyTileRect = {0,0,COPY_DLG_W,40};
static int g_nCopyCmd = -1;

static CTRLDATA ui_copy_control[] =
{
	{
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,//| BS_CHECKED,
        COPY_CHECK_START_X,  COPY_CHECK_START_Y, COPY_CHECK_W, COPY_CHECK_H,
        IDC_CHECK_CH1,
        "通道一",
        0,
        WS_EX_TRANSPARENT
    },
    {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,//| BS_CHECKED,
		COPY_CHECK_START_X+COPY_CHECK_INC_X,  COPY_CHECK_START_Y, COPY_CHECK_W, COPY_CHECK_H,
        IDC_CHECK_CH2,
        "通道二",
        0,
        WS_EX_TRANSPARENT
    },
     {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,//| BS_CHECKED,
		COPY_CHECK_START_X+2*COPY_CHECK_INC_X,  COPY_CHECK_START_Y, COPY_CHECK_W, COPY_CHECK_H,
        IDC_CHECK_CH3,
        "通道三",
        0,
        WS_EX_TRANSPARENT
    },
    {
        GUI_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX ,//| BS_CHECKED,
		COPY_CHECK_START_X+3*COPY_CHECK_INC_X,	COPY_CHECK_START_Y, COPY_CHECK_W, COPY_CHECK_H,
        IDC_CHECK_CH4,
        "通道四",
        0,
        WS_EX_TRANSPARENT
    },
    //////////////////// GUI_BUTTON /////////////////////////

    CONFIG_TWO_WORDS_BUTTON("确定", IDC_COPY_SAVE, COPY_DLG_W - 160, COPY_DLG_H - 50),
    CONFIG_TWO_WORDS_BUTTON("取消", IDC_COPY_CANCEL, COPY_DLG_W - 80 , COPY_DLG_H - 50),

};


static DLGTEMPLATE CopyDlg =
{
    WS_NONE,
    WS_EX_NONE,
    COPY_DLG_X, COPY_DLG_Y, COPY_DLG_W, COPY_DLG_H,
    "复制",
    0, 0,
    TABLESIZE(ui_copy_control), NULL,
    0
};

extern int ExchangeEncodeDlgParam(IPNC_DEV_VENC_CFG_S *MainStreamCfg, IPNC_DEV_VENC_CFG_S *SubStreamCfg);
extern int ExchangeDiskmanageDlgParam(NVR_DEV_REC_STRC_S *stRec);
extern osdParam *GetIpcOsdParam(void );

static void InitCopyParam(HWND hDlg , int ch)
{
	HWND hWnd;
	hWnd = GetDlgItem(hDlg,IDC_CHECK_CH1+ch);
	SendDlgItemMessage(hDlg, IDC_CHECK_CH1+ch, BM_SETCHECK, BST_CHECKED, 0);
	EnableWindow(hWnd, FALSE);
}

static void SetCopyParam(HWND hDlg , int ch)
{
	int chn = 0;	
	unsigned int checked;
	int ret;


	//拷贝图像参数
	if (g_nCopyCmd == EN_COPY_PIC_PARAM)
	{
		IPNC_DEV_IMA_CFG_S stIMACfg;
		IPNC_DEV_3A_CFG_S st3ACfg;
		
		if(0 > nvr_get_ipnc_param(ch, IPNC_CMD_IMA_CFG, &stIMACfg, sizeof(IPNC_DEV_IMA_CFG_S)))
			return;
		
		if(0 > nvr_get_ipnc_param(ch, IPNC_CMD_3A_CFG, &st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
			return;
		for (chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++)
		{			
			if (chn == ch)	continue;
			
			checked = SendDlgItemMessage(hDlg, IDC_CHECK_CH1+chn, BM_GETCHECK, 0, 0);
			if (checked != BST_CHECKED)	continue;

			printf("copy to ch%d\r\n", chn+1);
			nvr_set_ipnc_param(chn, IPNC_CMD_IMA_CFG, &stIMACfg, sizeof(IPNC_DEV_IMA_CFG_S));	
			nvr_set_ipnc_param(chn, IPNC_CMD_3A_CFG, &st3ACfg, sizeof(IPNC_DEV_3A_CFG_S));
		}
		
	}
	else if(g_nCopyCmd == EN_COPY_ENCODE_PARAM)//拷贝编码参数
	{
		IPNC_DEV_VENC_CFG_S mainStreamCfg, subStreamCfg;
		memset(&mainStreamCfg, 0, sizeof(IPNC_DEV_VENC_CFG_S));
		memset(&subStreamCfg, 0, sizeof(IPNC_DEV_VENC_CFG_S));
		
		for (chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++)
		{			
			if (chn == ch)	continue;
			
			checked = SendDlgItemMessage(hDlg, IDC_CHECK_CH1+chn, BM_GETCHECK, 0, 0);
			if (checked != BST_CHECKED)	continue;

			printf("copy to ch%d\r\n", chn+1);
			nvr_get_ipnc_param(chn,IPNC_CMD_VMAIN_CFG, &mainStreamCfg, sizeof(IPNC_DEV_VENC_CFG_S));
			nvr_get_ipnc_param(chn,IPNC_CMD_VSUB_CFG, &subStreamCfg, sizeof(IPNC_DEV_VENC_CFG_S));
			ExchangeEncodeDlgParam(&mainStreamCfg, &subStreamCfg);
			
			nvr_set_ipnc_param(chn, IPNC_CMD_VMAIN_CFG, &mainStreamCfg, sizeof(IPNC_DEV_VENC_CFG_S));			
			nvr_set_ipnc_param(chn, IPNC_CMD_VSUB_CFG, &subStreamCfg, sizeof(IPNC_DEV_VENC_CFG_S));
		}
	}else if (g_nCopyCmd == EN_COPY_STORAGE_STRATEGY)
	{
		NVR_DEV_REC_STRC_S stRec;	
		memset(&stRec, 0, sizeof(NVR_DEV_REC_STRC_S));

		for (chn =0 ; chn < NVR_MAX_CHANNEL_NUM; chn++)
		{
			if (chn == ch) continue;

			checked = SendDlgItemMessage(hDlg, IDC_CHECK_CH1+chn, BM_GETCHECK, 0, 0);
			if (checked != BST_CHECKED)	continue;
			
			ret = nvr_logon_get_rec_cfg(chn, &stRec);
			ExchangeDiskmanageDlgParam(&stRec);
			nvr_logon_set_rec_cfg(chn, &stRec);
		}
	}
	else if(g_nCopyCmd == EN_COPY_IPC_OSD)
	{
		IPNC_DEV_OSD_CFG_S	stOsdCfg = {0};

		pOsdParam pIpcOsdParam = NULL;
		pIpcOsdParam = GetIpcOsdParam();
		
		for (chn =0 ; chn < NVR_MAX_CHANNEL_NUM; chn++)
		{
			if (chn == ch) continue;

			checked = SendDlgItemMessage(hDlg, IDC_CHECK_CH1+chn, BM_GETCHECK, 0, 0);
			if (checked != BST_CHECKED)	continue;
			printf("!!!!!!!!!!!!!!!!!chn: %d\r\n",chn);
			//获取OSD参数配置	
			nvr_get_ipnc_param(chn,  IPNC_CMD_OSD_CFG, &stOsdCfg, sizeof(IPNC_DEV_OSD_CFG_S));
			stOsdCfg.u8EnableTimeOsd = pIpcOsdParam->u8EnableTimeOsd;
			stOsdCfg.u8EnableBitrateOsd = pIpcOsdParam->u8EnableBitrateOsd;
			stOsdCfg.u32TimeColor = pIpcOsdParam->u32TimeColor;
			stOsdCfg.u32ChnColor = pIpcOsdParam->u32ChnColor;
			strcpy(stOsdCfg.szChnName, pIpcOsdParam->szChnName);

			stOsdCfg.u8ColorValidFlag = 0; //写死

			nvr_set_ipnc_param(chn,  IPNC_CMD_OSD_CFG, &stOsdCfg, sizeof(IPNC_DEV_OSD_CFG_S));

		}	
	}
}


static int CopyWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	static int chn = -1;
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		if (lParam >=0 &&lParam < 4)
			chn = lParam;
		InitCopyParam(hDlg, chn);
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0    
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg, "复制");
        break;
	}
#endif
	case MSG_ERASEBKGND:
	{
		RECT title = {0, 0, 500, 40};
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
		Gui_WriteTitleName(hdc, &title, LTR("复制"));
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
		case IDC_COPY_SAVE:
			SetCopyParam(hDlg, chn);
			//PostMessage(hDlg, MSG_CLOSE, 0, 0);
			SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
			return 0;
		case IDC_COPY_CANCEL:	
			PostMessage(hDlg, MSG_CLOSE, 0, 0);
			return 0;
		}
		break;
    }
    case MSG_CLOSE:
    {		
		chn = -1;
		g_nCopyCmd = -1;
    	HWND hWind;
    	pop_open_dlg(&headOpenDlg, &hWind);
        EndDialog (hDlg, 0);
        break;
    }

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

//远程设备配置
void CreateCopyDlgWin(HWND hParent, int chn, int cmd)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "ColorDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_copy_control));
	if(NULL == ctrolTmp)
		return ;
	g_nCopyCmd = cmd;	
	memcpy(ctrolTmp, ui_copy_control, sizeof(ui_copy_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_copy_control)
    CopyDlg.controls = ui_copy_control;
    
    DialogBoxIndirectParam (&CopyDlg, hParent, CopyWinProc, chn);
	memcpy(ui_copy_control, ctrolTmp, sizeof(ui_copy_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

