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
#include "syscfg.h"


#define LOGIN_DLG_W	500
#define LOGIN_DLG_H	250
#define LOGIN_DLG_X	((NVR_SCREEN_WIDTH - LOGIN_DLG_W)>>1)
#define LOGIN_DLG_Y	((NVR_SCREEN_HEIGHT - LOGIN_DLG_H)>>1)

#define LOGIN_EDIT_W		180
#define LOGIN_EDIT_H		21
#define LONGIN_INC_Y		30
#define LOGIN_TEXT_W		80
#define LOGIN_TEXT_H	21
#define LOGIN_TEXT_START_X	((LOGIN_DLG_W - LOGIN_EDIT_W - LOGIN_TEXT_W)/2)
#define LOGIN_TEXT_START_Y	90

typedef enum
{
	IDC_LOGIN_OK = 2100,
	IDC_LOGIN_CANCEL,
	IDC_LOGIN_TOK,
	IDC_LOGIN_TCANCEL,
	IDC_LOGIN_EOK,
	IDC_LOGIN_ECANCEL,
	IDL_USERNAME_SELECT,
	IDC_LOGIN_USERPASS,
	IDC_LOGIN_TOOLTIP,
	IDC_LOGIN_LANGUAGE,
}UI_LOGIN_ID;

extern int UISetLogin(char *strUse);

typedef struct tag_LoginInfo
{
	char *user;
	char *pass;
}LoginInfo_t;

static CTRLDATA ui_login_control[] =
{
	{
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE | CBS_DROPDOWNLIST|CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
		LOGIN_TEXT_START_X+40, LOGIN_TEXT_START_Y, LOGIN_EDIT_W, GUI_COMBOX_H,
        IDL_USERNAME_SELECT,
        "",
        0,
        WS_EX_USEPARENTFONT
    },
	{
		CTRL_MYEDIT,
        WS_CHILD| WS_VISIBLE | ES_LEFT | WS_TABSTOP|ES_PASSWORD,
		LOGIN_TEXT_START_X+ 40, LOGIN_TEXT_START_Y+LONGIN_INC_Y+10, LOGIN_EDIT_W, LOGIN_TEXT_H,
        IDC_LOGIN_USERPASS,
        "",
        0,
		WS_EX_EDIT_NUMINPUT|WS_EX_EDIT_CHARINPUT| WS_EX_USEPARENTFONT
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        LOGIN_TEXT_START_X-30, LOGIN_TEXT_START_Y+LONGIN_INC_Y+30, 130, LOGIN_TEXT_H,
        IDC_LOGIN_TOOLTIP,
        "",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
        LOGIN_TEXT_START_X+LOGIN_EDIT_W+93, LOGIN_TEXT_START_Y-40, LOGIN_EDIT_W-100, LOGIN_TEXT_H,
        IDC_LOGIN_LANGUAGE,
        "",
        60,
		WS_EX_USEPARENTFONT
    },

    CONFIG_TWO_WORDS_BUTTON("确定", IDC_LOGIN_OK, (LOGIN_DLG_W)/2 - 30 - TWO_WORDS_BUTTON_W, LOGIN_DLG_H - 60),
    CONFIG_TWO_WORDS_BUTTON("取消", IDC_LOGIN_CANCEL, (LOGIN_DLG_W)/2 + 30, LOGIN_DLG_H - 60),

    CONFIG_TWO_WORDS_BUTTON("確定", IDC_LOGIN_TOK, (LOGIN_DLG_W)/2 - 30 - TWO_WORDS_BUTTON_W, LOGIN_DLG_H - 60),
    CONFIG_TWO_WORDS_BUTTON("取消", IDC_LOGIN_TCANCEL, (LOGIN_DLG_W)/2 + 30, LOGIN_DLG_H - 60),

    CONFIG_TWO_WORDS_BUTTON("Ok", IDC_LOGIN_EOK, (LOGIN_DLG_W)/2 - 30 - TWO_WORDS_BUTTON_W, LOGIN_DLG_H - 60),
    CONFIG_TWO_WORDS_BUTTON("Cancel", IDC_LOGIN_ECANCEL, (LOGIN_DLG_W)/2 + 30, LOGIN_DLG_H - 60),
};


static DLGTEMPLATE LoginDlg =
{
    WS_NONE,
    WS_EX_NONE,
    LOGIN_DLG_X, LOGIN_DLG_Y, LOGIN_DLG_W, LOGIN_DLG_H,
    "复制",
    0, 0,
    TABLESIZE(ui_login_control), NULL,
    0
};

extern HWND GetMainWin();

static char	usrName[NVR_USER_NAME_LEN] = {0};	//用户名
static char	usrPsw[NVR_PASS_WORD_LEN] = {0};		//密码

static const char *gLanguage[] = {
	"简体中文", "English", "繁體中文"
};

static char gComboxLanguage[5][32] = {{0}};

void SetLoginUserAndPwd(char *name, char *psw)
{
	if(NULL == name || NULL == psw)
		return ;

	strncpy(usrName, name, sizeof(usrName));
	strncpy(usrPsw, psw, sizeof(usrPsw));
}

int GetLoginUserAndPwd(char *name, char *psw)
{
	if(NULL == name || NULL == psw)
		return -1;
	strcpy(name, usrName);
	strcpy(psw, usrPsw);

	return 0;
}
static void InitLoginCtrl(HWND hDlg)
{	
	int i, j;
	//char chLanguage[64] = {0};
	
	NVR_DEV_USER_CFG_S userCfg, tmpUserCfg;
	
	SendDlgItemMessage(hDlg, IDC_LOGIN_USERPASS, EM_LIMITTEXT, 32, 0);
	SendDlgItemMessage(hDlg,IDL_USERNAME_SELECT, CB_RESETCONTENT, 0, (LPARAM)0);
	for(i = 0; i < NVR_MAX_USR_NUM; i++)
	{
		nvr_logon_get_user_cfg(i, &userCfg);
		if(strlen(userCfg.szUsrName) == 0)
			continue;
		for(j = i - 1; j>=0; j--)
		{
			nvr_logon_get_user_cfg(j, &tmpUserCfg);
			if(strcmp(userCfg.szUsrName, tmpUserCfg.szUsrName) == 0)
				break;
		}
		if(j < 0)
			SendDlgItemMessage(hDlg, IDL_USERNAME_SELECT, CB_ADDSTRING, 0, (LPARAM)userCfg.szUsrName);
	}
	SendDlgItemMessage(hDlg, IDL_USERNAME_SELECT, CB_SETCURSEL, 0, 0);

	DEV_SystemCfg *pSystemCfg;
	pSystemCfg = get_system_cfg();

	SendDlgItemMessage(hDlg,IDC_LOGIN_LANGUAGE, CB_RESETCONTENT, 0, (LPARAM)0);
	for(i=0, j=0; i<TABLESIZE(gLanguage); i++)
	{
		if(pSystemCfg->supportLang & (0x1<<i))
		{
			SendDlgItemMessage(hDlg, IDC_LOGIN_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)gLanguage[i]);
			strcpy(gComboxLanguage[j++], gLanguage[i]);
		}
	}

	NVR_DEV_INFO_S devInfo;
	int index = 0;
	memset(&devInfo, 0, sizeof(NVR_DEV_INFO_S));
	if(nvr_logon_get_dev_cfg(0, &devInfo) < 0)
		printf("[error] get dev cfg\n");
		
	index = devInfo.stVarInfo.u8Language;
	//printf("####################chLanguage: %d\r\n",devInfo.stVarInfo.u8Language);
	if(pSystemCfg->supportLang & (0x1<<index))
	{
		for(i=0; i<j; i++)
		if(strcmp(gLanguage[index], gComboxLanguage[i]) == 0)
			SendDlgItemMessage(hDlg, IDC_LOGIN_LANGUAGE, CB_SETCURSEL, i, 0);
	}
}


int CheckLoginValid(HWND hDlg)
{		
	int index;
	int i, ret = -1;
	char acUser[64];
	char acPass[64];
	NVR_DEV_USER_CFG_S userCfg;
	index = SendDlgItemMessage(hDlg, IDL_USERNAME_SELECT, CB_GETCURSEL, 0, 0);
	SendDlgItemMessage(hDlg, IDL_USERNAME_SELECT, CB_GETLBTEXT, index, (LPARAM)acUser);
	GetDlgItemText(hDlg, IDC_LOGIN_USERPASS, acPass, sizeof(acPass)-1);

	//printf("user: %s, password: %s\n", acUser, acPass);
	for(i = 0; i < NVR_MAX_USR_NUM; i++)
	{
		nvr_logon_get_user_cfg(i, &userCfg);
		if((strcmp(acUser, userCfg.szUsrName) == 0) &&
			(strcmp(acPass, userCfg.szPsw) == 0))
		{
			UISetLogin(acUser);
			SetLoginUserAndPwd(acUser, acPass);
			ret = 0;
			break;
		}
	}

	return ret;
}

static void UpdateSelectLanguage(HWND hDlg)
{
    int index;
    char chLanguage[64] = {0};
	int i;

	index = SendDlgItemMessage(hDlg, IDC_LOGIN_LANGUAGE, CB_GETCURSEL, 0, 0);
	SendDlgItemMessage(hDlg, IDC_LOGIN_LANGUAGE, CB_GETLBTEXT, index, (LPARAM)chLanguage);
	if(strcmp(chLanguage, gLanguage[0]) == 0)
		SetDisplayLanguage(0);
	else if(strcmp(chLanguage, gLanguage[2]) == 0)
		SetDisplayLanguage(2);
		//strcpy(chLanguage, "chinese.txt");
	else if(strcmp(chLanguage, gLanguage[1]) == 0)
		SetDisplayLanguage(1);

	NVR_DEV_INFO_S devInfo;
	memset(&devInfo, 0, sizeof(NVR_DEV_INFO_S));
	if(nvr_logon_get_dev_cfg(0, &devInfo) < 0)
		printf("[error] get dev cfg\n");

	for(i=0; i<TABLESIZE(gLanguage); i++)
	{
		if(strcmp(gLanguage[i], chLanguage) == 0)
			break;
	}
	devInfo.stVarInfo.u8Language = i;
	nvr_logon_set_dev_cfg(0, &devInfo);

	InvalidateRect(hDlg, NULL, TRUE);
	InvalidateRect(GetMainWin(), NULL, TRUE);
}

static int LoginWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {

    case MSG_INITDIALOG:
	{	
		InitLoginCtrl(hDlg);
		SetWindowBkColor(hDlg, 0XFFA8A9AD);
		push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 1	
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);
		int index;
		HWND cOk,cCancel,tOk,tCancel,eOk,eCancel;

		cOk = GetDlgItem(hDlg, IDC_LOGIN_OK);
		cCancel = GetDlgItem(hDlg, IDC_LOGIN_CANCEL);
		tOk = GetDlgItem(hDlg, IDC_LOGIN_TOK);
		tCancel = GetDlgItem(hDlg, IDC_LOGIN_TCANCEL);
		eOk = GetDlgItem(hDlg, IDC_LOGIN_EOK);
		eCancel = GetDlgItem(hDlg, IDC_LOGIN_ECANCEL);

		GetDisplayLanguageIndex(&index);

		if(0 == index)
		{
			ShowWindow(eOk, SW_HIDE);
			ShowWindow(eCancel, SW_HIDE);
			ShowWindow(tOk, SW_HIDE);
			ShowWindow(tCancel, SW_HIDE);
			ShowWindow(cOk, SW_SHOW);
			ShowWindow(cCancel, SW_SHOW);
		}else if(1 == index)
		{
			ShowWindow(cOk, SW_HIDE);
			ShowWindow(cCancel, SW_HIDE);
			ShowWindow(tOk, SW_HIDE);
			ShowWindow(tCancel, SW_HIDE);
			ShowWindow(eOk, SW_SHOW);
			ShowWindow(eCancel, SW_SHOW);
		}
		else
		{
			ShowWindow(cOk, SW_HIDE);
			ShowWindow(cCancel, SW_HIDE);
			ShowWindow(eOk, SW_HIDE);
			ShowWindow(eCancel, SW_HIDE);
			ShowWindow(tOk, SW_SHOW);
			ShowWindow(tCancel, SW_SHOW);
		}

		EndPaint (hDlg, hdc);		
        break;
	}
#endif
	case MSG_ERASEBKGND:
	{
		//RECT title = {0, 0, 420, 40};
		HDC hdc = (HDC)wParam; 
		const RECT* clip = (const RECT*) lParam; 
		BOOL fGetDC = FALSE; 
		RECT rcTemp; 
		int index, languages;
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
		GetDisplayLanguageIndex(&index);
		languages = getSysSupportLanguages();
		index = (index >= languages) ? 0 : index;

		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &login_pic[index]);
			
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
        
		if(id == IDL_USERNAME_SELECT)
		{			
			if (code == CBN_SELCHANGE)
			{
				SendDlgItemMessage(hDlg, IDC_LOGIN_USERPASS, MSG_SETTEXT,0 ,(LPARAM)"");
				SendDlgItemMessage(hDlg, IDC_LOGIN_TOOLTIP, MSG_SETTEXT,0 ,(LPARAM)"");
			}
			return 0;
		}
		else if(id == IDC_LOGIN_LANGUAGE)
		{
			if(code == CBN_SELCHANGE)
			{	
				SendDlgItemMessage(hDlg, IDC_LOGIN_TOOLTIP, MSG_SETTEXT,0 ,(LPARAM)"");
				UpdateSelectLanguage(hDlg);
			}
			return 0;
		}
		
		switch (wParam) 
		{
		case IDC_LOGIN_EOK:
		case IDC_LOGIN_OK:
		case IDC_LOGIN_TOK:

		{
			int ret;
			ret = CheckLoginValid(hDlg);
			if (ret == 0) 
			{
				SendNotifyMessage(hDlg, MSG_CLOSE, 0, 0);
			}
			else
			{
				SendDlgItemMessage(hDlg, IDC_LOGIN_TOOLTIP, MSG_SETTEXT,0 ,(LPARAM)LTR("密码验证失败"));				
				SendDlgItemMessage(hDlg, IDC_LOGIN_USERPASS, MSG_SETTEXT,0 ,(LPARAM)"");
			}
		}
		return 0;
		case IDC_LOGIN_ECANCEL:
		case IDC_LOGIN_CANCEL:
		case IDC_LOGIN_TCANCEL:
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
void CreateLoginDlgWin(HWND hParent)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "LoginDlg" , 0 , 0);
#endif
	LoginDlg.controls = ui_login_control;

	DialogBoxIndirectParam (&LoginDlg, hParent, LoginWinProc, 0L);
#if 0

	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_login_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_login_control, sizeof(ui_login_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_login_control)
    LoginDlg.controls = ui_login_control;
    
    DialogBoxIndirectParam (&LoginDlg, hParent, LoginWinProc, 0L);
	memcpy(ui_login_control, ctrolTmp, sizeof(ui_login_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
#endif	
}

