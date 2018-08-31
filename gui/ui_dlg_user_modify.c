#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>
#include <minigui/skin.h>

#include "nvr_lib_interface.h"
#include "myedit.h"
#include "gui_core.h"
#include "ui_config.h"
#include "ui_dlg_head.h"

#define USER_MODIFY_DLG_W	600
#define USER_MODIFY_DLG_H	400
#define USER_MODIFY_DLG_X	((NVR_SCREEN_WIDTH - USER_MODIFY_DLG_W)>>1)
#define USER_MODIFY_DLG_Y	((NVR_SCREEN_HEIGHT - USER_MODIFY_DLG_H)>>1)
#define USER_M_TEXT_START_X	50
#define USER_M_TEXT_START_Y	60
#define USER_M_TEXT_W		100
#define USER_M_EDIT_START_X	(USER_M_TEXT_START_X + 120)
#define USER_M_EDIT_START_Y	(USER_M_TEXT_START_Y)
#define USER_M_EDIT_H		22
#define USER_M_EDIT_W		180
#define USER_M_EDIT_INC_Y	(USER_M_EDIT_H + 10)

typedef enum
{
	IDC_USER_M_OK = 3500,
	IDC_USER_M_CANCEL,
	IDC_EDIT_USER_NAME,
	IDC_EDIT_OLD_PASS,
	IDC_EDIT_NEW_PASS,
	IDC_EDIT_CONFIRM_PASS,
	IDC_MODIFY_TOOLTIP,
}UI_USER_MODIFY_ID;


//static RECT UserModifyTileRect = {0,0,USER_MODIFY_DLG_W,40};
static int nUserIndex = -1;
static int wFlag = -1;

static CTRLDATA ui_user_modify_control[] =
{
	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        USER_M_TEXT_START_X, USER_M_TEXT_START_Y, USER_M_TEXT_W, USER_M_EDIT_H,
        IDC_STATIC,
        "用户名",
        0,
		WS_EX_TRANSPARENT
    },
    {
	    CTRL_MYEDIT,
        WS_CHILD | ES_LEFT | WS_VISIBLE,
        USER_M_EDIT_START_X, USER_M_EDIT_START_Y, USER_M_EDIT_W, USER_M_EDIT_H,
        IDC_EDIT_USER_NAME,
        "",
        0,
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT| WS_EX_USEPARENTFONT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        USER_M_TEXT_START_X, USER_M_TEXT_START_Y+USER_M_EDIT_INC_Y, USER_M_TEXT_W, USER_M_EDIT_H,
        IDC_STATIC,
        "旧密码",
        0,
		WS_EX_TRANSPARENT
    },
    {
	    CTRL_MYEDIT,
        WS_CHILD | ES_LEFT | WS_VISIBLE|ES_PASSWORD,
        USER_M_EDIT_START_X, USER_M_EDIT_START_Y+USER_M_EDIT_INC_Y, USER_M_EDIT_W, USER_M_EDIT_H,
        IDC_EDIT_OLD_PASS,
        "",
        0,
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT| WS_EX_USEPARENTFONT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        USER_M_TEXT_START_X, USER_M_TEXT_START_Y+USER_M_EDIT_INC_Y*2, USER_M_TEXT_W, USER_M_EDIT_H,
        IDC_STATIC,
        "新密码",
        0,
		WS_EX_TRANSPARENT
    },
    {
	    CTRL_MYEDIT,
        WS_CHILD | ES_LEFT | WS_VISIBLE|ES_PASSWORD,
        USER_M_EDIT_START_X, USER_M_EDIT_START_Y+USER_M_EDIT_INC_Y*2, USER_M_EDIT_W, USER_M_EDIT_H,
        IDC_EDIT_NEW_PASS,
        "",
        0,
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT| WS_EX_USEPARENTFONT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        USER_M_TEXT_START_X, USER_M_TEXT_START_Y+USER_M_EDIT_INC_Y*3, USER_M_TEXT_W+15, USER_M_EDIT_H,
        IDC_STATIC,
        "确认密码",
        0,
		WS_EX_TRANSPARENT
    },
    {
	    CTRL_MYEDIT,
        WS_CHILD | ES_LEFT | WS_VISIBLE|ES_PASSWORD,
        USER_M_EDIT_START_X, USER_M_EDIT_START_Y+USER_M_EDIT_INC_Y*3, USER_M_EDIT_W, USER_M_EDIT_H,
        IDC_EDIT_CONFIRM_PASS,
        "",
        0,
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT| WS_EX_USEPARENTFONT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
		USER_M_TEXT_START_X, USER_M_EDIT_START_Y+USER_M_EDIT_INC_Y*4, USER_M_EDIT_W+100, USER_M_EDIT_H,
        IDC_MODIFY_TOOLTIP,
        "",
        0,
		WS_EX_TRANSPARENT
    },
    CONFIG_TWO_WORDS_BUTTON("确定", IDC_USER_M_OK, USER_MODIFY_DLG_W - 165, USER_MODIFY_DLG_H - 50),
    CONFIG_TWO_WORDS_BUTTON("取消", IDC_USER_M_CANCEL, USER_MODIFY_DLG_W - 100 , USER_MODIFY_DLG_H - 50),

};

static DLGTEMPLATE UserModifyDlg =
{
    WS_NONE,
    WS_EX_NONE,
    USER_MODIFY_DLG_X, USER_MODIFY_DLG_Y, USER_MODIFY_DLG_W, USER_MODIFY_DLG_H,
    "复制",
    0, 0,
    TABLESIZE(ui_user_modify_control), NULL,
    0
};

static void InitUserModifyDlgInfo(HWND hDlg)
{
	NVR_DEV_USER_CFG_S userCfg;
	
	SendDlgItemMessage(hDlg, IDC_EDIT_USER_NAME, EM_LIMITTEXT, 32, 0);	
	SendDlgItemMessage(hDlg, IDC_EDIT_OLD_PASS, EM_LIMITTEXT, 32, 0);
	SendDlgItemMessage(hDlg, IDC_EDIT_NEW_PASS, EM_LIMITTEXT, 32, 0);
	SendDlgItemMessage(hDlg, IDC_EDIT_CONFIRM_PASS, EM_LIMITTEXT, 32, 0);
	
	if ((nUserIndex > NVR_MAX_USR_NUM) || (nUserIndex < 0)) return;
	nvr_logon_get_user_cfg(nUserIndex, &userCfg);
	SetDlgItemText(hDlg,IDC_EDIT_USER_NAME, userCfg.szUsrName);
	if(1 == wFlag)
		EnableDlgItem(hDlg, IDC_EDIT_USER_NAME, FALSE);
	else
		EnableDlgItem(hDlg, IDC_EDIT_OLD_PASS, FALSE);
	
	SetDlgItemText(hDlg,IDC_EDIT_OLD_PASS, "");
	SetDlgItemText(hDlg,IDC_EDIT_NEW_PASS, "");
	SetDlgItemText(hDlg,IDC_EDIT_CONFIRM_PASS, "");
}

static int PasswordModifyProc(HWND hDlg)
{
	char acUser[64], acOldPass[64], acNewPass[64], acConfirmPass[64];
	NVR_DEV_USER_CFG_S userCfg;		
	if ((nUserIndex > NVR_MAX_USR_NUM) || (nUserIndex < 0)) return -1;
	nvr_logon_get_user_cfg(nUserIndex, &userCfg);
	
	GetDlgItemText(hDlg,IDC_EDIT_USER_NAME, acUser, sizeof(acUser) -1);
	GetDlgItemText(hDlg,IDC_EDIT_OLD_PASS, acOldPass, sizeof(acOldPass) -1);
	GetDlgItemText(hDlg,IDC_EDIT_NEW_PASS, acNewPass, sizeof(acNewPass) -1);
	GetDlgItemText(hDlg,IDC_EDIT_CONFIRM_PASS, acConfirmPass, sizeof(acConfirmPass) -1);

	if(1 == wFlag)
	{
		if ((strcmp(userCfg.szPsw, acOldPass) == 0)&&
			(strlen(acUser)!= 0)&&(strlen(acNewPass)!=0)&&
			(strcmp(acNewPass, acConfirmPass) == 0))
		{
			strncpy(userCfg.szPsw, acNewPass, sizeof(userCfg.szPsw)-1);
			strncpy(userCfg.szUsrName, acUser, sizeof(userCfg.szUsrName) -1);		
			nvr_logon_set_user_cfg(nUserIndex, &userCfg);
			return 0;
		}
	}
	else if(0 == wFlag)
	{
		int i;
		for(i = 0; i < NVR_MAX_USR_NUM; i++)
		{
			nvr_logon_get_user_cfg(i, &userCfg);
			if(strcmp(userCfg.szUsrName, acUser) == 0)
			{
#ifdef OWN_MESSAGEBOX_WIN
				CreateMessageBoxWin(hDlg, LTR("系统中已存在此用户名，请更换用户名!"), LTR("消息"), IDOK);
#else
				MessageBox(hDlg, LTR("系统中已存在此用户名，请更换用户名!"), LTR("消息"), IDOK);
#endif
				return -2;
			}
		}
		if((strlen(acUser)!= 0)&&(strlen(acNewPass)!=0)&&
			(strcmp(acNewPass, acConfirmPass) == 0))
		{
			strncpy(userCfg.szPsw, acNewPass, sizeof(userCfg.szPsw)-1);
			strncpy(userCfg.szUsrName, acUser, sizeof(userCfg.szUsrName) -1);		
			nvr_logon_set_user_cfg(nUserIndex, &userCfg);
			return 0;
		}
	}
	return -1;
	
}


static int UserModifyWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		InitUserModifyDlgInfo(hDlg);
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);
		Gui_DrawCaption(hDlg, "用户账号");
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
		Gui_WriteTitleName(hdc, &title, LTR("用户账号"));
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
		case IDC_USER_M_OK:	
		{
			int ret;
			ret = PasswordModifyProc(hDlg);
			if (ret < 0)
			{
				if(-1 == ret)
					SendDlgItemMessage(hDlg, IDC_MODIFY_TOOLTIP, MSG_SETTEXT, 0 ,(LPARAM)LTR("密码修改失败，检查输入"));
				return 0;
			}
			//PostMessage(hDlg, MSG_CLOSE, 0, 0);
			SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
		}
			return 0;
		case IDC_USER_M_CANCEL:
			PostMessage(hDlg, MSG_CLOSE, 0, 0);
			return 0;
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

//远程设备配置
void CreateUserModifyDlgWin(HWND hParent, int nUser, int flag)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "UserModifyDlg" , 0 , 0);
#endif
	nUserIndex = nUser;
	wFlag = flag;
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_user_modify_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_user_modify_control, sizeof(ui_user_modify_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_user_modify_control)
    UserModifyDlg.controls = ui_user_modify_control;
    
    DialogBoxIndirectParam (&UserModifyDlg, hParent, UserModifyWinProc, 0L);
	memcpy(ui_user_modify_control, ctrolTmp, sizeof(ui_user_modify_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

