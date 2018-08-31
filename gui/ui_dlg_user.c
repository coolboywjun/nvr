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
#include "gui_core.h"
#include "ui_config.h"
#include "ui_dlg_head.h"

#define USER_DLG_W		600
#define USER_DLG_H		400
#define USER_DLG_X		((NVR_SCREEN_WIDTH - USER_DLG_W)>>1)
#define USER_DLG_Y		((NVR_SCREEN_HEIGHT - USER_DLG_H)>>1)
#define USER_LIST_W		((USER_DLG_W*9)/10)
#define USER_LIST_H		((USER_DLG_H*2)/3)
#define USER_LIST_X 	((USER_DLG_W - USER_LIST_W)>>1)
#define USER_LIST_Y 	(60)

typedef enum
{
	IDC_USER_MODIFY = 3400,
	IDC_USER_ADD,
	IDC_USER_DELET,
	IDC_USER_CANCEL,
	IDC_USER_OK,
	IDC_USER_LISTVIEW,
}UI_USER_SETTING_ID;


//static RECT UserTileRect = {0,0,USER_DLG_W,40};

static CTRLDATA ui_user_control[] =
{
	{
        "listview_ex",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        USER_LIST_X, USER_LIST_Y, USER_LIST_W, USER_LIST_H,
        IDC_USER_LISTVIEW,
        "用户列表",
        0
    },
    CONFIG_FOUR_WORDS_BUTTON("添加用户", IDC_USER_ADD, USER_LIST_X+USER_LIST_W - 365, USER_DLG_H - 50),
    CONFIG_FOUR_WORDS_BUTTON("删除用户", IDC_USER_DELET, USER_LIST_X+USER_LIST_W - 265, USER_DLG_H - 50),
    CONFIG_FOUR_WORDS_BUTTON("修改用户", IDC_USER_MODIFY, USER_LIST_X+USER_LIST_W - 165, USER_DLG_H - 50),
    CONFIG_TWO_WORDS_BUTTON("退出", IDC_USER_OK, USER_LIST_X+USER_LIST_W - 65, USER_DLG_H - 50),
	//CONFIG_TWO_WORDS_BUTTON("取消", IDC_USER_CANCEL, USER_DLG_W/2 + 50 , USER_DLG_H - 40),

};

static DLGTEMPLATE UserDlg =
{
    WS_NONE,
    WS_EX_NONE,
    USER_DLG_X, USER_DLG_Y, USER_DLG_W, USER_DLG_H,
    "复制",
    0, 0,
    TABLESIZE(ui_user_control), NULL,
    0
};


static listCaption captionUser[] = 
{
	{5, "序号"},		
	{10, "用户名"},
	{10, "属组"},
	{15, "状态"}
};

extern int UIGetLogin(loginInfo_t * loginData);

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
    TextOut (hdc, rcDraw->left+4, rcDraw->top+4, LTR(captionUser[idx].Text));
}
#endif


static int InitUserListHeader(HWND hDlg)
{
	int i;
    LVCOLUMN lvcol;
	int sum_w = 0;
	int col_num = TABLESIZE(captionUser);
	
	for (i = 0; i < col_num; i++)
		sum_w += captionUser[i].col_w;
	
    for (i = 0; i < col_num; i++) 
	{
        lvcol.nCols = i;
        lvcol.pszHeadText = captionUser[i].Text;
        lvcol.width = captionUser[i].col_w*USER_LIST_W/sum_w;
		if(i == 0)
			lvcol.pfnCompare = NULL;//compare_string; //序号项，按照序号的大小排序
		else
        	lvcol.pfnCompare = NULL;//其它选项按照默认的按字符串排序
        lvcol.colFlags = 0;
		SendDlgItemMessage(hDlg, IDC_USER_LISTVIEW, LVM_ADDCOLUMN, 0, (LPARAM) &lvcol);
    }

#ifdef CUSTOM_DRAW
	HWND hListView;
	hListView = GetDlgItem(hDlg,IDC_USER_LISTVIEW);
    LVCUSTOMDRAWFUNCS myfuncs = {my_draw_hdr_bk, my_draw_hdr_item};
    SendMessage (hListView, LVM_SETCUSTOMDRAW, 0, (LPARAM) &myfuncs);
#endif
	
	return 0;
}


static GHANDLE AddUserItemList(HWND hDlg, PLVITEM lvItem,  NVR_DEV_USER_CFG_S *userCfg, char *curUser)
{
	char Text[20];
    LVSUBITEM subdata;
    GHANDLE item = SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_ADDITEM, 0, (LPARAM)lvItem);
  
    subdata.flags = 0;
    subdata.image = 0;
    subdata.nItem = lvItem->nItem;
	subdata.nTextColor = 0;

	//序号
	sprintf(Text, "%d", lvItem->nItem);
	subdata.subItem = 0;
	subdata.pszText = (char *)Text;
    SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//用户名
	subdata.subItem = 1;
	subdata.pszText = userCfg->szUsrName;
    SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//属组
	if (strcasecmp(userCfg->szUsrName, "admin") == 0)
		sprintf(Text, "admin");
	else
		sprintf(Text, "user");
	subdata.subItem = 2;
	subdata.pszText = Text;
    SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//状态
	subdata.subItem =3;
	if(strcmp(curUser, userCfg->szUsrName) == 0)
		subdata.pszText = LTR("本机登录");
	else
		subdata.pszText = LTR("正常");
    SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_SETSUBITEM, item, (LPARAM) & subdata);

    return item;
}

static int DisplayUserListItem(HWND hDlg)
{
	int i = 0;
	NVR_DEV_USER_CFG_S userCfg;		
	LVITEM item;
	loginInfo_t loginData;

	memset(&loginData, 0, sizeof(loginInfo_t));	
	UIGetLogin(&loginData); //获取当前登录的用户信息

	SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_DELALLITEM, 0, 0);
	SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, MSG_FREEZECTRL, TRUE, 0);	
	item.nItemHeight = 20;
	
	for(i = 0; i < NVR_MAX_USR_NUM; i++)
	{		
		item.nItem = i;
		nvr_logon_get_user_cfg(i, &userCfg);
		
		if(strlen(userCfg.szUsrName) != 0)
			AddUserItemList(hDlg, &item, &userCfg, loginData.acUser);
	}
	SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, MSG_FREEZECTRL, FALSE, 0);

	SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_SELECTITEM, 0, (LPARAM)0);
	
	return 0;
}

static int GetLoginUserIndex(HWND hDlg)
{
	int ret;
	int nItem = 0, nNum = 0;
	HLVITEM hItem; 
	char acText[32];
	loginInfo_t loginData;

	memset(&loginData, 0, sizeof(loginInfo_t));
	nNum = SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_GETITEMCOUNT, 0, 0);
	if (nNum <= 0)	return 0;	
	hItem = SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_GETSELECTEDITEM, 0, 0);
	LVSUBITEM lvSubitem;
	lvSubitem.subItem = 0;
    lvSubitem.pszText = (char *)acText;
	ret = SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_GETSUBITEMTEXT, hItem, (LPARAM)&lvSubitem);
	if (ret == -1)	return -1;
	nItem = atoi(acText);
	if (nItem < 0 || nItem > NVR_MAX_USR_NUM)	return -1;

	UIGetLogin(&loginData); //获取当前登录的用户信息
	
	if((strcmp(loginData.acUser, "admin") != 0)/* || (loginData.login != 1)*/)
	{
		lvSubitem.subItem = 1;
		ret = SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_GETSUBITEMTEXT, hItem, (LPARAM)&lvSubitem);
		if (ret == -1)	return -1;
		//printf("[%s %d]##################acText: %s\r\n",__func__,__LINE__,acText);
		if(strcmp(loginData.acUser, acText) != 0)
		{
			return -2;
		}
	}
	
	return nItem;
}

static int AddUserInfoCheck(HWND hDlg)
{
	int nNum;
	loginInfo_t loginData;

	memset(&loginData, 0, sizeof(loginInfo_t));	
	UIGetLogin(&loginData); //获取当前登录的用户信息
	if(strcmp(loginData.acUser, "admin") != 0)
	return -2;

	nNum = SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_GETITEMCOUNT, 0, 0);
	//printf("[debug]nNum :%d################\n", nNum);
	if (nNum >= NVR_MAX_USR_NUM) return -1;

	return 0;
}

static int DelectUserInfoCheck(HWND hDlg)
{
	int nNum;
	loginInfo_t loginData;

	memset(&loginData, 0, sizeof(loginInfo_t));	
	UIGetLogin(&loginData); //获取当前登录的用户信息
	if(strcmp(loginData.acUser, "admin") != 0)
	return -2;

	nNum = SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_GETITEMCOUNT, 0, 0);
	//printf("[debug]nNum :%d################\n", nNum);
	if (nNum <= 0) return -1;

	return 0;
}

static int GetFreeUserInfoIndex(void )
{
	int i = 0;
	NVR_DEV_USER_CFG_S userCfg;		
	
	for(i = 0; i < NVR_MAX_USR_NUM; i++)
	{		
		nvr_logon_get_user_cfg(i, &userCfg);
		if(strlen(userCfg.szUsrName) == 0)
			break;
	}	
	if(i == NVR_MAX_USR_NUM)
		return -1;
		
	return i;
}

static int DelectUserInfo(HWND hDlg)
{
	int ret;
	HLVITEM hItem; 
	char acText[32];
	int i;
	NVR_DEV_USER_CFG_S userCfg;		

	memset(&userCfg, 0, sizeof(NVR_DEV_USER_CFG_S));
	hItem = SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_GETSELECTEDITEM, 0, 0);
	LVSUBITEM lvSubitem;
	lvSubitem.subItem = 1;
    lvSubitem.pszText = (char *)acText;
	ret = SendDlgItemMessage (hDlg, IDC_USER_LISTVIEW, LVM_GETSUBITEMTEXT, hItem, (LPARAM)&lvSubitem);
	if (ret == -1)	return -1;
	//printf("[DEBUG]################acText: %s\r\n",acText);
	
	if(strcmp(acText, "admin") == 0)
	{
#ifdef OWN_MESSAGEBOX_WIN
		CreateMessageBoxWin(hDlg, LTR("不支持删除管理员用户!"), LTR("消息"), IDOK);
#else
		MessageBox(hDlg, LTR("不支持删除管理员用户!"), LTR("消息"), IDOK);
#endif
		return -1;
	}
	for(i=0; i<NVR_MAX_USR_NUM; i++)
	{
		nvr_logon_get_user_cfg(i, &userCfg);
		if(strcmp(acText, userCfg.szUsrName) == 0)
		{
			memset(&userCfg, 0, sizeof(NVR_DEV_USER_CFG_S));
			nvr_logon_set_user_cfg(i, &userCfg);
			break;
		}
	}
	
	return 0;
}
static int UserWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int nUser;
    switch (message)
    {

    case MSG_INITDIALOG:
    {	
		InitUserListHeader(hDlg);
		DisplayUserListItem(hDlg);
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
		case IDC_USER_OK:	
		case IDC_USER_CANCEL:
			PostMessage(hDlg, MSG_CLOSE, 0, 0);
			return 0;
		case IDC_USER_MODIFY:
		{
			nUser = GetLoginUserIndex(hDlg);
			if (nUser < 0)
			{
				if(nUser == -2)
#ifdef OWN_MESSAGEBOX_WIN
				CreateMessageBoxWin(hDlg, LTR("只有管理员用户才能修改其他用户信息!"), LTR("消息"), IDOK);
#else
				MessageBox(hDlg, LTR("只有管理员用户才能修改其他用户信息!"), LTR("消息"), IDOK);
#endif
				return 0;
			}
			CreateUserModifyDlgWin(hDlg, nUser, 1);
			DisplayUserListItem(hDlg);
		}
		break;
		case IDC_USER_ADD:
			nUser = AddUserInfoCheck(hDlg);
			if(nUser < 0)
			{
				if(nUser == -2)
#ifdef OWN_MESSAGEBOX_WIN
					CreateMessageBoxWin(hDlg, LTR("只有管理员用户才能添加新用户!"), LTR("消息"), IDOK);
#else
					MessageBox(hDlg, LTR("只有管理员用户才能添加新用户!"), LTR("消息"), IDOK);
#endif
				if(nUser == -1)
#ifdef OWN_MESSAGEBOX_WIN
					CreateMessageBoxWin(hDlg, LTR("超过系统支持的用户数!"), LTR("消息"), IDOK);
#else
					MessageBox(hDlg, LTR("超过了系统支持的用户数!"), LTR("消息"), IDOK);
#endif
				return 0;
			}
			nUser = GetFreeUserInfoIndex();
			if(nUser < 0) return 0;
				
			//printf("#############nUser: %d\r\n",nUser);
			CreateUserModifyDlgWin(hDlg, nUser, 0);
			DisplayUserListItem(hDlg);

		return 0;
		case IDC_USER_DELET:
			nUser = DelectUserInfoCheck(hDlg);
			if(nUser < 0)
			{
				if(-2 == nUser)
#ifdef OWN_MESSAGEBOX_WIN
				CreateMessageBoxWin(hDlg, LTR("只有管理员用户才能删除用户!"), LTR("消息"), IDOK);
#else
				MessageBox(hDlg, LTR("只有管理员用户才能删除用户!"), LTR("消息"), IDOK);
#endif
				return 0;	
			}

			DelectUserInfo(hDlg);
			DisplayUserListItem(hDlg);
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

static void AdjustPositionDlgWin()
{
	int index,i;

	GetDisplayLanguageIndex(&index);
	if(1 == index) 
	{
		for(i=1; i<=3; i++)
		{
			ui_user_control[i].x += 10;
			ui_user_control[i].w = TWO_WORDS_BUTTON_W;
			ui_user_control[i].dwAddData = (DWORD)&button_pic[0];
		}
	}	
}


//远程设备配置
void CreateUserDlgWin(HWND hParent)
{
	#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "UserDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_user_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_user_control, sizeof(ui_user_control));
	DLG_CAPTION_LANGUAGE_TRANS(ui_user_control)
	AdjustPositionDlgWin();
    UserDlg.controls = ui_user_control;
    
    DialogBoxIndirectParam (&UserDlg, hParent, UserWinProc, 0L);
	memcpy(ui_user_control, ctrolTmp, sizeof(ui_user_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

