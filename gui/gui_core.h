#ifndef __GUI_CORE_H__
#define __HUI_CORE_H__

#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <sys/sysinfo.h>


//#include "gal.h"




	   
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <minigui/cliprect.h>
#include <minigui/internals.h>
#include <minigui/ctrlclass.h>

#include <minigui/vcongui.h>
#include <minigui/mgext.h>
#include <minigui/skin.h>


#include <minigui/ctrl/static.h>
#include <minigui/ctrl/edit.h>
#include <minigui/ctrl/button.h>
#include <minigui/ctrl/combobox.h>
#include <minigui/ctrl/trackbar.h>
#include <minigui/ctrl/ctrlhelper.h>
#include <minigui/ctrl/progressbar.h>
#include <minigui/ctrl/listbox.h>


#include "my_listbox.h"
#include "my_combobox.h"
#include "my_button.h"
#include "my_trackbar.h"
#include "my_propsheet.h"
#include "language.h"
#include "ui_dlg_head.h"



#define IMA_CLICK_SHOW
#define OWN_COMBOBOX_CTRL

#define OWN_MESSAGEBOX_WIN  //开启自定义消息框

#define GUI_COMBOX_H	21

#define	GUI_COMBOX	MY_COMBOBOX

#define GUI_PIC_BUTTON		MY_BUTTONEX
#define GUI_NORMAL_BUTTON	MY_BUTTONEX
#define GUI_BUTTON			MY_BUTTONEX

/*定义不同字数的按钮控件的长和宽*/
#define TWO_WORDS_BUTTON_W   	55
#define THREE_WORDS_BUTTON_W	66
#define FOUR_WORDS_BUTTON_W 	80
#define LONG_WORDS_BUTTON_W		102
#define ALL_BUTTON_H			25

/*	CAPTION_NAME: 标题名
 *	ID : 控件id号
 *  START_X,START_Y: 按钮控件左上角坐标
 */          

#define CONFIG_TWO_WORDS_BUTTON(CAPTION_NAME, ID, START_X, START_Y)	\
{ \
        GUI_BUTTON,											\
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,	\
        (START_X) ,	(START_Y),								\
        TWO_WORDS_BUTTON_W, ALL_BUTTON_H,					\
        (ID),												\
        CAPTION_NAME,										\
        (DWORD)&button_pic[0],								\
		WS_EX_TRANSPARENT									\
 }

#define CONFIG_THREE_WORDS_BUTTON(CAPTION_NAME, ID, START_X, START_Y)	\
{ \
        GUI_BUTTON,											\
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,	\
        (START_X) ,	(START_Y),								\
        THREE_WORDS_BUTTON_W, ALL_BUTTON_H,					\
        (ID),												\
        CAPTION_NAME,										\
        (DWORD)&button_pic[1],								\
		WS_EX_TRANSPARENT									\
 }

#define CONFIG_FOUR_WORDS_BUTTON(CAPTION_NAME, ID, START_X, START_Y)	\
{ \
        GUI_BUTTON,											\
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,	\
        (START_X) ,	(START_Y),								\
        FOUR_WORDS_BUTTON_W, ALL_BUTTON_H,					\
        (ID),												\
        CAPTION_NAME,										\
        (DWORD)&button_pic[2],								\
		WS_EX_TRANSPARENT									\
 }
#define CONFIG_LONG_WORDS_BUTTON(CAPTION_NAME, ID, START_X, START_Y)	\
{ \
        GUI_BUTTON,											\
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,	\
        (START_X) ,	(START_Y),								\
        LONG_WORDS_BUTTON_W, ALL_BUTTON_H,					\
        (ID),												\
        CAPTION_NAME,										\
        (DWORD)&button_pic[3],								\
		WS_EX_TRANSPARENT									\
 }


#define GUI_LISTVIEW "listview_ex"
#define GUI_LISTBOX	 MY_LISTBOX
#define GUI_TRACKBAR MY_TRACKBAR
#define GUI_PROPSHEET MY_PROPSHEET

//ListView自绘
#define CUSTOM_DRAW
#ifdef CUSTOM_DRAW
#define LISTVIEW_HEAD_BKC	0xff979797   //0xff272727
#endif

#define HIDE_CURSER
#define LOCK_SCREEN

//开启宏则编辑框为圆角
//#define _ROUND_RECT_STYLE



void Gui_DrawRoundRect(HDC hdc ,RECT *pRect,int width,int color,int arc);
BOOL Gui_DrawTriangle ( HDC hdc ,const POINT* pts, int vertices);
void Gui_WriteTitleName(HDC hdc, RECT* prc,char *text);
long GUI_GetUptime();

void GUI_InitGloblePicRes();
void GUI_UninitGloblePicRes();

void GUI_InitCtrl();
void GUI_UninitCtrl();
void GUI_ReplaySetDisplayCreen(int mode);

void Gui_DrawCaption(HWND hwnd, char *text);


void EnableDlgItem(HWND hDlg, int id, BOOL fEnable);
void ShowAllControls (HWND hDlg, int iCmdShow);

int  create_qrcode_png(const char *stBuf, const char *pngName, unsigned int *width, unsigned int *height);



#endif


