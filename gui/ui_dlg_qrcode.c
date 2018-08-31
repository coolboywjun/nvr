#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>
#include <minigui/skin.h>

#include "ui_config.h"
#include "ui_dlg_head.h"
#include "gui_core.h"
#include "syscfg.h"


#define QRCODE_DLG_W	490
#define QRCODE_DLG_H	350
#define QRCODE_DLG_X	((NVR_SCREEN_WIDTH - QRCODE_DLG_W)>>1)
#define QRCODE_DLG_Y	((NVR_SCREEN_HEIGHT - QRCODE_DLG_H)>>1)
#define QRCODE_PIC_W	110
#define QRCODE_EDIT_W	80
#define QRCODE_EDIT_H	20

#define QRCODE_ENC_Y	(QRCODE_EDIT_H+10)

#define QRCODE_START_X	60
#define QRCODE_START_Y	60

typedef enum
{
	IDC_QRCODE_NEXT = 1650,
	IDC_QRCODE_PREV,
	IDC_QRCODE_CANCEL,
	IDC_ANDROID_DOWNLOAD,
}UI_QRCODE_ID;


static CTRLDATA ui_qrcode_control[] =
{
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        QRCODE_START_X, QRCODE_START_Y, 80, QRCODE_EDIT_H,
        IDC_STATIC,
        "设备ID",
        0,
		WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        QRCODE_START_X+QRCODE_PIC_W+150, QRCODE_START_Y, 100, QRCODE_EDIT_H,
        IDC_ANDROID_DOWNLOAD,
        "",
        0,
		WS_EX_TRANSPARENT
    },
    CONFIG_THREE_WORDS_BUTTON("上一步", IDC_QRCODE_PREV, QRCODE_DLG_W - 260, QRCODE_DLG_H - 50),	
    CONFIG_THREE_WORDS_BUTTON("下一步", IDC_QRCODE_NEXT, QRCODE_DLG_W - 180, QRCODE_DLG_H - 50),
    CONFIG_TWO_WORDS_BUTTON("取消", IDC_QRCODE_CANCEL, QRCODE_DLG_W - 100 , QRCODE_DLG_H - 50),
};


static DLGTEMPLATE QrcodeDlg =
{
    WS_NONE,
    WS_EX_NONE,
    QRCODE_DLG_X, QRCODE_DLG_Y, QRCODE_DLG_W, QRCODE_DLG_H,
    "开机向导",
    0, 0,
    TABLESIZE(ui_qrcode_control), NULL,
    0
};

#define DEVID_QR_PATH	"/tmp/devId.png"

static char	usrName[NVR_USER_NAME_LEN] = {0};	//用户名
static char	usrPsw[NVR_PASS_WORD_LEN] = {0};		//密码
extern int GetLoginUserAndPwd(char * name, char * psw);

static int ShowSerialIdQrcodePic(HWND hDlg, HDC hdc, const char *qrcode_str)
{
	RECT rect;
	unsigned int width,height;
	
	create_qrcode_png(qrcode_str, DEVID_QR_PATH, &width, &height);
	LoadBitmapFromFile(HDC_SCREEN, &devidmap, DEVID_QR_PATH);
	
	GetClientRect (hDlg, &rect);
	SetBrushColor(hdc, PIXEL_EDIT_BKG_NORMAL);	
	FillBox (hdc, QRCODE_START_X, QRCODE_START_Y+QRCODE_ENC_Y, QRCODE_PIC_W, QRCODE_PIC_W);
#if 0
	FillBoxWithBitmap (hdc, rect.right-width-5-60, QRCODE_START_Y+5, width, height, &devidmap);
#else
	HDC hDCMem = CreateCompatibleDCEx(hdc, devidmap.bmWidth, devidmap.bmHeight);
	FillBoxWithBitmap(hDCMem, 0, 0, devidmap.bmWidth, devidmap.bmHeight, &devidmap);
	StretchBlt(hDCMem, 0, 0, devidmap.bmWidth, devidmap.bmHeight, hdc, QRCODE_START_X+5, QRCODE_START_Y+QRCODE_ENC_Y+5,
	QRCODE_PIC_W-10, QRCODE_PIC_W-10,0);
#endif
	
	UnloadBitmap(&devidmap);

	return 0;
}
static int ShowDownloadQrcodePic(HWND hDlg, HDC hdc, BITMAP *bitmap)
{
	HWND androidHwnd;
	
	androidHwnd = GetDlgItem(hDlg, IDC_ANDROID_DOWNLOAD);
	SetWindowText(androidHwnd, LTR("ANDROID客户端"));

	FillBox (hdc, QRCODE_START_X+QRCODE_PIC_W+150, QRCODE_START_Y+QRCODE_ENC_Y, QRCODE_PIC_W,QRCODE_PIC_W);

	HDC hDCMem = CreateCompatibleDCEx(hdc, bitmap->bmWidth, bitmap->bmHeight);
	FillBoxWithBitmap(hDCMem, 0, 0, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	StretchBlt(hDCMem, 0, 0, bitmap->bmWidth, bitmap->bmHeight, hdc, QRCODE_START_X+QRCODE_PIC_W+150+5, QRCODE_START_Y+QRCODE_ENC_Y+5,
	QRCODE_PIC_W-10,QRCODE_PIC_W-10,0);

	return 0;
}
static int ShowQrcodePic(HWND hDlg, HDC hdc)
{		
	DEV_SystemCfg *pSystemCfg;
	NVR_DEV_INFO_S devInfo = {0};
	char qrcode_str[512] = {0};
	
	if(GetLoginUserAndPwd(usrName, usrPsw) < 0)
		return -1;
	
	nvr_logon_get_dev_cfg(0, &devInfo);
	pSystemCfg = get_system_cfg();	
	if(strstr(devInfo.stStableInfo.szSerialId, "JXJ"))
	{
		sprintf(qrcode_str, "DNNVR,ID%s,CP3321,SP7554,TP3322,TY2,UN%s,PW%s,CC%d",devInfo.stStableInfo.szSerialId,
				usrName, usrPsw, devInfo.stStableInfo.u8ChnNum);
		ShowSerialIdQrcodePic(hDlg, hdc, qrcode_str);
		ShowDownloadQrcodePic(hDlg, hdc, &androidmap_vivi);
	}
	else if(strstr(devInfo.stStableInfo.szSerialId, "TCVNVR"))
	{
		ShowSerialIdQrcodePic(hDlg, hdc, devInfo.stStableInfo.szSerialId);
		ShowDownloadQrcodePic(hDlg, hdc, &androidmap_tutk);
	}
	else
	{
		if(pSystemCfg->platformServer & PLATFORM_VIVI_SERVER)
		{
			sprintf(qrcode_str, "DNNVR,ID%s,CP3321,SP7554,TP3322,TY2,UN%s,PW%s,CC%d",devInfo.stStableInfo.szSerialId,
					usrName, usrPsw, devInfo.stStableInfo.u8ChnNum);
			ShowSerialIdQrcodePic(hDlg, hdc, qrcode_str);
			ShowDownloadQrcodePic(hDlg, hdc, &androidmap_vivi);
		}
		else if(pSystemCfg->platformServer & PLATFORM_TUTK_SERVER)
		{
			ShowSerialIdQrcodePic(hDlg, hdc, devInfo.stStableInfo.szSerialId);
			ShowSerialIdQrcodePic(hDlg, hdc, qrcode_str);
			ShowDownloadQrcodePic(hDlg, hdc, &androidmap_tutk);
		}
		else
		{
			ShowSerialIdQrcodePic(hDlg, hdc, devInfo.stStableInfo.szSerialId);
		}
	}
	return 0;
}


static int QrcodeWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
    {
		push_open_dlg(&headOpenDlg, hDlg);
		break;
    }
    case MSG_PAINT:
	{
       	HDC hdc;
       	hdc = BeginPaint (hDlg);	
		ShowQrcodePic(hDlg, hdc);
       	EndPaint (hDlg, hdc);		
		break;
	}

	case MSG_ERASEBKGND:
	{
		RECT title = {0, 0, 490, 50};
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
		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bk_pic[SIZE_490x350]); 
		Gui_WriteTitleName(hdc, &title, LTR("手机监控"));
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
			case IDC_QRCODE_PREV:			
				PostMessage(hDlg, MSG_CLOSE, 0, 0);
				break;
			case IDC_QRCODE_NEXT:
				CreateNormalParamWin(hDlg);			
				return 0;
			case IDC_QRCODE_CANCEL:
				BroadcastMessage(MSG_WIZARD_EXIT, 0, 0);
				return 0;
		}
		break;
    }

	case MSG_WIZARD_EXIT:
	{
		printf("0000000000000000\r\n");
		PostMessage(hDlg, MSG_CLOSE, 0, 0);
	}
	break;
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

//远程设备配置
void CreateQrcodeDlgWin(HWND hParent)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "QrcodeDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_qrcode_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_qrcode_control, sizeof(ui_qrcode_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_qrcode_control)
    QrcodeDlg.controls = ui_qrcode_control;
    
    DialogBoxIndirectParam (&QrcodeDlg, hParent, QrcodeWinProc, 0L);
	memcpy(ui_qrcode_control, ctrolTmp, sizeof(ui_qrcode_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

