#include "gui_core.h"
#include "ui_config.h"
#include <string.h>
#include "ui_storage_interface.h"

extern BOOL RegisterMyEditControl();
extern BOOL RegisterIpAddressControl();
extern BOOL RegisterMyListView();
extern BOOL RegisterRecinfoBarControl ();
extern BOOL RegisterDateControl();
extern BOOL RegisterMyScrollView (void);
extern BOOL RegisterToolbarControlEx(void);
extern BOOL RegisterProgressBarControlEx();


extern void UnregisterMyEditControl();
extern void ButtonCTRLCleanup();


extern BOOL RegisterMyListboxControl (void);
extern void MyListboxControlCleanup (void);

extern BOOL RegisterMyComboBoxControl();
extern void MyComboBoxControlCleanup (void);

extern BOOL RegisterMyButtonCTRL (void);
extern void ButtonMyCTRLCleanup (void);

extern BOOL RegisterMyMonthCalendarControl (void);
extern void MyMonthCalendarControlCleanup (void);

extern BOOL RegisterMyTrackBarControl (void);
extern void MyTrackBarControlCleanup (void);


extern BOOL RegisterMyPropSheetControl (void);
extern void MyPropSheetControlCleanup (void);



//button_pic[0]两个子长度
//button_pic[1]容纳三个汉字
//button_pic[2]容纳四个汉字
//button_pic[3]容纳五个以上的汉字
BITMAP button_pic[4];
void Gui_InitButtonPic()
{
	LoadBitmapFromFile(HDC_SCREEN, &button_pic[0], "nvr_res/two_words_button.bmp");	
	LoadBitmapFromFile(HDC_SCREEN, &button_pic[1], "nvr_res/three_words_button.bmp");
	LoadBitmapFromFile(HDC_SCREEN, &button_pic[2], "nvr_res/four_words_button.bmp");
	LoadBitmapFromFile(HDC_SCREEN, &button_pic[3], "nvr_res/long_words_button.bmp");
}

void Gui_UninitButtonPic()
{
	UnloadBitmap(&button_pic[0]);
	UnloadBitmap(&button_pic[1]);
	UnloadBitmap(&button_pic[2]);
	UnloadBitmap(&button_pic[3]);

}



BITMAP bk_pic[BK_NUM_MAX];



void Gui_DrawRoundRect(HDC hdc ,RECT *pRect,int width,int color,int arc )
{
    gal_pixel old_color  =GetPenColor(hdc);

    SetPenColor(hdc,color);
#if 1
	Rectangle(hdc,pRect->left,pRect->top,pRect->right-1,pRect->bottom-1);
#else	
    int i;
    for(i = 0; i < width; i++)
    {
        RoundRect( hdc, pRect->left+i,pRect->top+i, pRect->right-width+i,pRect->bottom-width+i, arc, arc);
    }
#endif
    SetPenColor(hdc,old_color);

}


BOOL Gui_DrawTriangle ( HDC hdc ,const POINT* pts, int vertices)
{
    return FillPolygon(hdc, pts, vertices);
}

void Gui_WriteTitleName(HDC hdc, RECT* prc,char *text)
{
    PLOGFONT temp_font;
    temp_font = GetCurFont(hdc);
    SetBkMode(hdc, BM_TRANSPARENT);//文本背景模式
    SetTextColor(hdc, PIXEL_lightwhite);
    SelectFont(hdc, GetSystemFont(SYSLOGFONT_CAPTION));//设置字体g_test_logfont
    DrawText(hdc, text, -1, prc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    SelectFont(hdc,temp_font);
}

void Gui_DrawCaption(HWND hwnd, char *text)
{
	BITMAP caption;
	PLOGFONT temp_font;
	RECT winRect, captionRect;

	GetClientRect(hwnd,&winRect);
	HDC hdc = GetClientDC(hwnd);
	
	LoadBitmapFromFile(HDC_SCREEN, &caption , "nvr_res/caption.bmp");
	FillBoxWithBitmap(hdc, 0,  0 , RECTW(winRect),caption.bmHeight, &caption);

	captionRect.left = 0;
	captionRect.top = 0;
	captionRect.right = RECTW(winRect);
	captionRect.bottom = caption.bmHeight;
		
    temp_font = GetCurFont(hdc);
    SetBkMode(hdc, BM_TRANSPARENT);						//文本背景模式
    SetTextColor(hdc, PIXEL_lightwhite);
    SelectFont(hdc, GetSystemFont(SYSLOGFONT_CAPTION));	//设置字体
    DrawText(hdc, text, -1, &captionRect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    SelectFont(hdc,temp_font);
	
	UnloadBitmap(&caption);	
	ReleaseDC (hdc); 
}


//0 fullscreen
const RECT rectReplayScreen[2] = 
{
	{0, 0, NVR_SCREEN_WIDTH, NVR_SCREEN_HEIGHT},
	{4, 5, 128*7+4, 72*8+5 - 20}
};


void GUI_ReplaySetDisplayCreen(int mode)
{
	if (mode < 0|| mode > 1)	return;
		player_set_scn_rsz(rectReplayScreen[mode].left, rectReplayScreen[mode].top, 
		RECTW(rectReplayScreen[mode]), RECTH(rectReplayScreen[mode]));
}


void EnableDlgItem(HWND hDlg, int id, BOOL fEnable)
{
	HWND hWnd = GetDlgItem(hDlg, id);
	EnableWindow(hWnd, fEnable);
}


void ShowAllControls (HWND hDlg, int iCmdShow)
{
	
    PMAINWIN pDlg = (PMAINWIN)hDlg;
    PCONTROL pCtrl = (PCONTROL)(pDlg->hFirstChild);
    PCONTROL pNext;

    while (pCtrl) {
        pNext = pCtrl->next;
        ShowWindow((HWND)pCtrl,iCmdShow);
        pCtrl = pNext;
    }
}



long GUI_GetUptime()
{
	long uptime = 0;
	struct sysinfo s_info;
	if(0==sysinfo(&s_info)){
		uptime = s_info.uptime;
	}
	return uptime;
}

void GUI_InitBKPic()
{
	LoadBitmapFromFile(HDC_SCREEN, &bk_pic[SIZE_1280x720] , "nvr_res/1280x720.png");	
	LoadBitmapFromFile(HDC_SCREEN, &bk_pic[SIZE_700x500] , "nvr_res/700x500.png");	
	LoadBitmapFromFile(HDC_SCREEN, &bk_pic[SIZE_600x400] , "nvr_res/600x400.png");
	LoadBitmapFromFile(HDC_SCREEN, &bk_pic[SIZE_490x350] , "nvr_res/490x350.png");
	LoadBitmapFromFile(HDC_SCREEN, &bk_pic[SIZE_420x180] , "nvr_res/420x180.png");
	LoadBitmapFromFile(HDC_SCREEN, &bk_pic[SIZE_500x150] , "nvr_res/500x150.png");
}

void Gui_UninitBKPic()
{
	int i;
	for (i = 0; i < BK_NUM_MAX; i++)
		UnloadBitmap(&bk_pic[i]);
}

void GUI_InitCtrl()
{
	RegisterMyEditControl();
	RegisterIpAddressControl();
	RegisterMyScrollView ();
	RegisterMyListView();
	RegisterRecinfoBarControl ();
	RegisterDateControl();
	RegisterToolbarControlEx();
	RegisterProgressBarControlEx ();


	RegisterMyListboxControl();
	RegisterMyComboBoxControl();
	RegisterMyButtonCTRL();
	RegisterMyMonthCalendarControl ();

	RegisterMyTrackBarControl ();
	RegisterMyPropSheetControl();

}



void GUI_UninitCtrl()
{
	UnregisterMyEditControl();	
	
	MyComboBoxControlCleanup();
	MyListboxControlCleanup();
	MyButtonCTRLCleanup();
	
	MyMonthCalendarControlCleanup ();	
	MyTrackBarControlCleanup ();

}

/* 位图数组 */
static const char *advance_menu_bmp_name[] = {
"nvr_res/advance/time_setting.bmp",
"nvr_res/advance/abnormal_manage.bmp", 
"nvr_res/advance/alarm_output.bmp",
"nvr_res/advance/record_ctrl.bmp",
"nvr_res/advance/login_manage.bmp",
"nvr_res/advance/auto_maintain.bmp",
"nvr_res/advance/config_bakup.bmp",
"nvr_res/advance/log_info.bmp",
};

BITMAP  advance_menu_pic[10];
static void InitAdvanceMenuPic()
{
	int i, bmp_num = TABLESIZE(advance_menu_bmp_name);
	for(i = 0; i < bmp_num; i++)	
	{
		LoadBitmapFromFile(HDC_SCREEN, &advance_menu_pic[i], advance_menu_bmp_name[i]);
	}

}


static void UninitAdvanceMenuPic()
{
	
	int i, bmp_num = TABLESIZE(advance_menu_bmp_name);
		
	for(i = 0; i < bmp_num; i++)	
	{
		UnloadBitmap(&advance_menu_pic[i]);
	}	
}



static const char *MainMenuPicName[]=
{
	"nvr_res/main/remote_setting.bmp",	
	"nvr_res/main/record_manage.bmp",
	"nvr_res/main/vod_query.bmp",
	"nvr_res/main/network_setting.bmp",
	"nvr_res/main/advance_setting.bmp",
	"nvr_res/main/sys_info.bmp",
	"nvr_res/main/sys_close.bmp",
	"nvr_res/reboot_sys.bmp",
};

BITMAP main_menu_pic[10];
static void InitMainMenuPic()
{
	int i;
	int nNumBmp = TABLESIZE(MainMenuPicName);
	for (i = 0; i < nNumBmp; i++)
	{
		LoadBitmapFromFile(HDC_SCREEN, &main_menu_pic[i], MainMenuPicName[i]);
	}	
}

static void UninitMainMenuPic()
{
	int i;
	int nNumBmp = TABLESIZE(MainMenuPicName);
	for (i = 0; i < nNumBmp; i++)
	{
		UnloadBitmap(&main_menu_pic[i]);
	}
}

static const char * SysQrcodePicName[] = {
	"/tmp/devId.png",	
	"nvr_res/android_vivi.bmp",
	"nvr_res/android_tutk.bmp",
};

BITMAP devidmap, androidmap_vivi, androidmap_tutk;
unsigned int devIdQrCodeWidth,devIdQrCodeHeight;

static void InitQrCodePic()
{
	LoadBitmapFromFile(HDC_SCREEN, &androidmap_vivi, SysQrcodePicName[1]);
	LoadBitmapFromFile(HDC_SCREEN, &androidmap_tutk, SysQrcodePicName[2]);
}

static void Gui_UninitQrCodePic()
{
	UnloadBitmap(&androidmap_vivi);
	UnloadBitmap(&androidmap_tutk);
}

static const char *LoginPicName[]=
{
	"nvr_res/login_chinese.png",
	"nvr_res/login_english.png",
	"nvr_res/login_traditional.png",
};

BITMAP login_pic[3];

static void InitLoginPic()
{
	int i;
	int nNumBmp = TABLESIZE(LoginPicName);
	for (i = 0; i < nNumBmp; i++)
	{
		LoadBitmapFromFile(HDC_SCREEN, &login_pic[i], LoginPicName[i]);
	}	
}

static void UnInitLoginPic()
{
	int i;
	int nNumBmp = TABLESIZE(LoginPicName);
	for (i = 0; i < nNumBmp; i++)
	{
		UnloadBitmap(&login_pic[i]);
	}
}

void GUI_InitGloblePicRes()
{
	GUI_InitBKPic();
	Gui_InitButtonPic();
	InitAdvanceMenuPic();
	InitMainMenuPic();
	InitQrCodePic();
	InitLoginPic();
}

void GUI_UninitGloblePicRes()
{	
	Gui_UninitButtonPic();
	Gui_UninitBKPic();
	UninitAdvanceMenuPic();
	UninitMainMenuPic();
	Gui_UninitQrCodePic();
	UnInitLoginPic();
}




