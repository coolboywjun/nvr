/*
** $Id: hzinput.c,v 1.57 2003/09/29 02:52:46 snig Exp $
**
** hzinput.c: The GB2312 IME window.
**
** Orignally by KANG Xiaoning, some code from cce by He Rui.
**
** Copyright (C) 1998, 1999 He Rui (herui@cs.duke.edu)
** Copyright (C) 1999 Kang Xiaoning.
** Copyright (C) 2000 Zheng Xiang.
** Copyright (C) 2000, 2001, 2002 Wei Yongming.
** Copyright (C) 2003 Feynman Software.
**
** Current maintainer: Wei Yongming
** Create date: 1999/04/01
**
** Used abbreviations:
**  ime: input method
**  punc: punctuate mark
**
*/ 

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
** Modify records:
**
**  Who             When        Where           For What
**-----------------------------------------------------------------------------
**  WEI Yongming    1999/06/14  Tsinghua        Message dispatching
**  GONG Xiaorui    1999/07/01  HongShiFang     Soft keyboard
**  WEI Yongming    1999/07/17  Tsinghua        Add some local functions
**  WEI Yongming    1999/08/24  Tsinghua        Remove soft keyboard.
**  WEI Yongming    1999/08/29  Tsinghua        Handle some special keys.
**  Wei Yongming    1999/09/01  Tsinghua        Translat punc marks.
**  Wei Yongming    1999/09/01  Tsinghua        Translat punc marks.
**  Wei Yongming    2000/11/06  BluePoint       Merge pinyin module by Zheng Xiang.
**
** TODO:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include<sys/prctl.h>

#if 0
#include "common.h" 
#include "minigui.h" 
#include "gdi.h" 
#include "window.h" 
#include "endianrw.h" 
#include "misc.h"
#else
#include <minigui/common.h> 
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/endianrw.h>
#include <minigui/control.h>
//#include <pthread.h>
#endif

//#include "guilib.h"
#include "hzinput.h"
#include "pinyin.h"

#include "hzinput_softkey_define.h"
#include "ctrl_skbkey.h"
#include "t9.h"
#include "t9mb.h"
#include "myedit.h"
#include "ctrl_ipaddress.h"
#include "gui_core.h"

#define SYSBMP_IMECTRLBTN   "IMEctrlbtn"

/************ private functions *******************/
static BOOL hz_input_init(void);
static void hz_input_done(void);
static void unload_input_table(int i);
static BOOL hz_filter(unsigned char key, LPARAM lParam);
static int toggle_input_method(void);
static void toggle_half_full(void);
static void set_active_input_method(int);
static void refresh_input_method_area(HWND hwnd, HDC hDC);
static void DispSelection(HDC hDC);

static void FindMatchKey(void);
static void FillMatchChars(int j);
static void FillAssociateChars(int index);
static void FindAssociateKey(int index);

static hz_input_table* IntCode_Init(void);
static void IntCode_FindMatchKey(void);
static void IntCode_FillMatchChars(int index);
static void intcode_hz_filter(unsigned char key,LPARAM lParam);

static void CreateSoftKey (HWND parent);		//创建软键盘按键
static void OnSkbQuertyCommand(HWND hwnd, int id, int code);		//软键盘按键命令处理
static void ReinitSKB(HWND hwnd, int nShowType);			//重置软键盘的状态
static void ShowInputWnds(HWND hwnd, int nShowType);	//nShowType : 0 鼠标双击 1 面板按键enter

static int OnImeKeydownSkb(HWND hWnd, WPARAM wparam, LPARAM lparam);
static int OnImeKeyupSkb(HWND hWnd, WPARAM wparam, LPARAM lparam);
static int OnImeKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
static int OnImeKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

/***************************************************************************
 *                           variable defines                              *
 ***************************************************************************/

#define VERSION_STRING    "Chinese Input"
static BOOL bTwoLines;
static char ime_tab_path [MAX_PATH + 1];

static const unsigned char	fullchar[]	 =
	"　！”＃＄％＆’（）＊＋，－．／０１２３４５６７８９：；＜＝＞？"
	"＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ〔＼〕＾□"
	"‘ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ｛｜｝～  ";

#define CPUNCTYPE_1TO1      0
#define CPUNCTYPE_1TO2      1
#define CPUNCTYPE_CYCLE     2

#define CTRLBTN_WIDTH       16
#define CTRLBTN_HEIGHT      16

typedef struct PuncMark
{
    unsigned char   epunc;
    int             type;
    int             currone;          // only for cycle
    unsigned char*  cpunc;
}PUNCMARK;

static PUNCMARK puncmark[] = 
{
    {'`', 0, 0, (unsigned char*)"·"},
    {'~', 0, 0, (unsigned char*)"～"},
    {'!', 0, 0, (unsigned char*)"！"},
    {'@', 0, 0, (unsigned char*)"℃"},
    {'#', 0, 0, (unsigned char*)"＃"},
    {'$', 0, 0, (unsigned char*)"￥"},
    {'%', 0, 0, (unsigned char*)"％"},
    {'^', CPUNCTYPE_1TO2, 0, (unsigned char*)"……"},
    {'&', 0, 0, (unsigned char*)"§"},
    {'*', 0, 0, (unsigned char*)"×"},
    {'_', CPUNCTYPE_1TO2, 0, (unsigned char*)"——"},
    {'(', CPUNCTYPE_CYCLE, 0, (unsigned char*)"（）"},
    {')', CPUNCTYPE_CYCLE, 0, (unsigned char*)"〔〕"},
    {'{', CPUNCTYPE_CYCLE, 0, (unsigned char*)"《》"},
    {'}', CPUNCTYPE_CYCLE, 0, (unsigned char*)"〈〉"},
    {'[', CPUNCTYPE_CYCLE, 0, (unsigned char*)"〖〗"},
    {']', CPUNCTYPE_CYCLE, 0, (unsigned char*)"【】"},
    {'\\', 0, 0, (unsigned char*)"÷"},
    {'|', 0, 0, (unsigned char*)"※"},
    {';', 0, 0, (unsigned char*)"；"},
    {':', 0, 0, (unsigned char*)"："},
    {'\'', CPUNCTYPE_CYCLE, 0, (unsigned char*)"‘’"},
    {'\"', CPUNCTYPE_CYCLE, 0, (unsigned char*)"“”"},
    {',', 0, 0, (unsigned char*)"，"},
    {'.', 0, 0, (unsigned char*)"。"},
    {'<', CPUNCTYPE_CYCLE, 0, (unsigned char*)"「」"},
    {'>', CPUNCTYPE_CYCLE, 0, (unsigned char*)"『』"},
    {'?', 0, 0, (unsigned char*)"？"},
    {'/', 0, 0, (unsigned char*)"、"}
};

static int IsImeOpened;        /* 1 for opened, 0 for closed */
static int IsHanziInput;    /* 0 for ascii, 1 for hanzi input */
static int IsFullChar;      /* 0 for half char, 1 for full char */
static int IsFullPunc;      /* 0 for half punctuate marks, 1 for full marks */
 
static int CurIME;
static int nIMENr;          /* effetive IME method number */

static HWND sg_hTargetWnd = 0/*HWND_DESKTOP*/; // target window of ime.

static hz_input_table *input_table[NR_INPUTMETHOD],*cur_table = NULL;
static char seltab[16][MAX_PHRASE_LENGTH];

static int CurSelNum=0;   /* Current Total Selection Number */
static unsigned long InpKey[MAX_INPUT_LENGTH],save_InpKey[MAX_INPUT_LENGTH];
   /* Input key buffer */
static int InputCount,InputMatch, StartKey,EndKey;
static int save_StartKey,save_EndKey, save_MultiPageMode, 
           save_NextPageIndex, save_CurrentPageIndex;
static int NextPageIndex,CurrentPageIndex,MultiPageMode;
/* When enter MultiPageMode:
   StartKey .... CurrentPageIndex .... NextPageIndex .... EndKey 
*/
static unsigned long val1, val2,key1,key2;
static int IsAssociateMode;
static int CharIndex[15];   // starting index of matched char/phrases

static BITMAP sg_bmpIME;	// the bmp of the IME control button

static MSGHOOK	old_hook;

enum SKBTYPE
{
	SKBTYPE_QUERTY = 0,
	SKBTYPE_T9,
};

#define SKBATTR_EN		0x00000001L		//英文
#define SKBATTR_CN		0x00000002L		//中文
#define SKBATTR_NUM		0x00000004L		//全数字
#define SKBATTR_MASK	0x00000007L

enum SKBATTR_ST
{
	SKBATTR_ST_EN = 0,
	SKBATTR_ST_CN,
	SKBATTR_ST_NUM
};

static int sg_nSkbType = 0;		// 0: QWERTY键盘  1 : T9键盘
static DWORD sg_dwSkbAttr = 0;	//标志位 :  0位:英文 1位:中文 2位:纯数字(只在T9键盘中出现)
static int sg_nSkbAttrSt = 0;	

static int UseAssociateMode = 0;

static HWND sg_hImeWnd = 0;

/* 6 bit a key mask */
static const unsigned long mask[]=
{
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
  0x3F000000, 0x3FFC0000, 0x3FFFF000, 0x3FFFFFC0, 0x3FFFFFFF, 0x3FFFFFFF,
  0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF
};

/***************************************************************************
 *								softkeyboard : QWERTY							   *
 ***************************************************************************/
#define MSG_KEYDOWN_SKB		(MSG_USER+500)
#define MSG_KEYUP_SKB		(MSG_USER+501)

#ifndef SKB_QUERTY
#define SKB_QUERTY
#endif
#define WND_BK_COLOR		RGB2Pixel(HDC_SCREEN, 0x1f, 0x1c, 0x31)

#define IME_QUERTY_WND_W	(400+180)
#define IME_T9_WND_W		(134)
#if 1
#define IME_WND_H			(42)
#else
#define IME_WND_H			(0)
#endif
#define SKB_QUERY_WND_W		IME_QUERTY_WND_W
#define SKB_QUERY_WND_H		(168 - 40)

#define SKB_T9_WND_W		IME_T9_WND_W
#define SKB_T9_WND_H		(120)

#define WIDTH_IME		(400)
#define WIDTH_SKB		WIDTH_IME
#define HEIGHT_SKB		(168)
#define HEIGHT_IME		(HEIGHT_SKB+40)

#define STATIC_LEFT 0			//??态????????位??
#define STATIC_H  30//32			//??态???母?
#define STATIC_W  34//24			//??态???目?
#define STATIC_TOP 0			//??态???母叨???始??位??

#define STATIC_ESC_OFFSET 	(STATIC_W>>1)	//10
#define STATIC_BACK_OFFSET	(STATIC_W+(STATIC_W>>1))
#define STATIC_TAB_OFFSET	(STATIC_W>>1)	//15
#define STATIC_DEL_OFFSET	(STATIC_W>>1)
#define STATIC_CAPS_OFFSET  (STATIC_W+(STATIC_W>>2))//25
#define STATIC_ENTER_OFFSET	(STATIC_W+((STATIC_W*3)>>2))
#define STATIC_SHIFT_OFFSET (STATIC_W+(STATIC_W>>2))
#define STATIC_ARROW_OFFSET (STATIC_W>>2)
#define STATIC_CTRL_OFFSET	((STATIC_W<<1) + (STATIC_W>>2))
#define STATIC_SPACE_OFFSET	((STATIC_W<<3) + (STATIC_W>>1))

#define STYLE_SK	(WS_CHILD | WS_VISIBLE | WS_TABSTOP)
#define IDC_COMMON 100
#define IDC_NUM1  IDC_COMMON+1
#define IDC_NUM2  IDC_COMMON+2
#define IDC_NUM3  IDC_COMMON+3
#define IDC_NUM4  IDC_COMMON+4
#define IDC_NUM5  IDC_COMMON+5
#define IDC_NUM6  IDC_COMMON+6
#define IDC_NUM7  IDC_COMMON+7
#define IDC_NUM8  IDC_COMMON+8
#define IDC_NUM9  IDC_COMMON+9
#define IDC_NUM0  IDC_COMMON+10

#define IDC_KEYA  IDC_COMMON+11
#define IDC_KEYB  IDC_COMMON+12
#define IDC_KEYC  IDC_COMMON+13
#define IDC_KEYD  IDC_COMMON+14
#define IDC_KEYE  IDC_COMMON+15
#define IDC_KEYF  IDC_COMMON+16
#define IDC_KEYG  IDC_COMMON+17
#define IDC_KEYH  IDC_COMMON+18
#define IDC_KEYI  IDC_COMMON+19
#define IDC_KEYJ  IDC_COMMON+20
#define IDC_KEYK  IDC_COMMON+21
#define IDC_KEYL  IDC_COMMON+22
#define IDC_KEYM  IDC_COMMON+23
#define IDC_KEYN  IDC_COMMON+24
#define IDC_KEYO  IDC_COMMON+25
#define IDC_KEYP  IDC_COMMON+26
#define IDC_KEYQ  IDC_COMMON+27
#define IDC_KEYR  IDC_COMMON+28
#define IDC_KEYS  IDC_COMMON+29
#define IDC_KEYT  IDC_COMMON+30
#define IDC_KEYU  IDC_COMMON+31
#define IDC_KEYV  IDC_COMMON+32
#define IDC_KEYW  IDC_COMMON+33
#define IDC_KEYX  IDC_COMMON+34
#define IDC_KEYY  IDC_COMMON+35
#define IDC_KEYZ  IDC_COMMON+36


#define IDC_GANTANHAO    IDC_COMMON+37
#define IDC_WENHAO       IDC_COMMON+38
#define IDC_AT     		 IDC_COMMON+39
#define IDC_JINHAO     	 IDC_COMMON+40
#define IDC_DOLLAR       IDC_COMMON+41
#define IDC_PERCENT      IDC_COMMON+42
#define IDC_EQUAL        IDC_COMMON+43
#define IDC_ADD          IDC_COMMON+44
#define IDC_CHEN      	 IDC_COMMON+45
#define IDC_SUB          IDC_COMMON+46
#define IDC_PLINEDOWN    IDC_COMMON+47

#define IDC_CHU          IDC_COMMON+48
#define IDC_MAOHAO       IDC_COMMON+49
#define IDC_DOUHAO	     IDC_COMMON+50
#define IDC_DOT          IDC_COMMON+51
#define IDC_ADDR         IDC_COMMON+52

#define IDC_BACKSPACE    IDC_COMMON+53
#define IDC_ENTER        IDC_COMMON+54
#define IDC_SHIFT        IDC_COMMON+55
#define IDC_SPACE        IDC_COMMON+56



#define IDC_EDIT         IDC_COMMON+37
static char KeyValue[2][37] = {
	{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'a', 'b', 'c', 'd',
	 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
	 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	 0x20},
	{
	 '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'A', 'B', 'C', 'D',
	 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
	 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	 0x20}
};

static char symbolKeyValue[17]={'!','?','@','#','$','%','=','+','*','-','_',
	'/',':',',','.','&',0x20};

HWND hSoftKeyWnd[56];
static T_QUERTYKEYS sg_hSKeys = {0};
static HWND sg_hSkbQuerty = 0;

static HWND CreateSkbQuertyWnd(HWND hHosting);

/***************************************************************************
 *								softkeyboard : T9							   *
 ***************************************************************************/
//MiniSoftKey
#ifndef SKB_T9 
#define SKB_T9
#endif

#define IDC_T9SK1		(IDC_COMMON+101)
#define IDC_T9SK2		(IDC_COMMON+102)
#define IDC_T9SK3		(IDC_COMMON+103)
#define IDC_T9SK4		(IDC_COMMON+104)
#define IDC_T9SK5		(IDC_COMMON+105)
#define IDC_T9SK6		(IDC_COMMON+106)
#define IDC_T9SK7		(IDC_COMMON+107)
#define IDC_T9SK8		(IDC_COMMON+108)
#define IDC_T9SK9		(IDC_COMMON+109)
#define IDC_T9SK0		(IDC_COMMON+110)
#define IDC_T9CTRL		(IDC_COMMON+111)
#define IDC_T9BACKSPACE	(IDC_COMMON+112)

#if 0
#define IDC_T9FN		(IDC_COMMON+113)
#define IDC_T9ENTER		(IDC_COMMON+114)
#define IDC_T9ESC		(IDC_COMMON+115)
#define IDC_T9UP		(IDC_COMMON+116)
#define IDC_T9DOWN		(IDC_COMMON+117)
#define IDC_T9LEFT		(IDC_COMMON+118)
#define IDC_T9RIGHT		(IDC_COMMON+119)
#endif

#define STYLE_T9SK 		(WS_CHILD | WS_VISIBLE | WS_TABSTOP)	
#define STATIC_T9SKB_H	28	
#define STATIC_T9SKB_W	42
#define WIDTH_T9SKB	90
#define HEIGHT_T9SKB	120
#define STATIC_T9_TOP	0			//??态???母叨???始??位??

static HWND sg_hSkbT9 = 0;
//static int sg_nIsT9Skb = 0;
static T_T9SOFTKEYS sg_hT9SKeys = {0};
//static int hzNumber_Pixel[11];
static char hzNumber_Char[10] = 
	{ '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };


/* *********************lyx added 20130719****************** */
/* ****************该部分从开源上移植过来 20130719 ********* */
/* ********************************************************* */
int IMEMode;            //  
const T9PY_IDX* pt9idx;  
  
char en_Buf[MAX_KEY_CHAR];  //当前的字母(用于英文输入方式)  
char py_key[MAX_PY_LENGTH+1];   //中文输入中拼音输入键序列  
char py_Buf[MAX_PY_SAME][MAX_PY_LENGTH+1];              //拼音  
unsigned char hz_Buf[MAX_HZ_COUNT*2];                   //汉字  
unsigned char fh_Buf[50];  
  
int perpagecount;       //每页显示的文字个数  
int cur_status;     //输入状态  0.输入状态; 1.选择状态  
int start_point;        //显示数据的起始位置  
int py_keycounts;       //中文输入键个数(拼音长度)  
int cur_py;         //当前拼音序号  
int py_count;       //拼音个数  
static int is_fh_input = 0;	//是否为符号输入状态
  
char* ime_method[3];  
static const unsigned char fh_table[] = "~!@#$%^&*()_+{}|:\"<>?`-=[]\\;',./，。‘’“”";  

//function define  
static void InputKeyProcess(unsigned char key, LPARAM lParam);  
static void init_ime_parameter(void);  
static void RefreshIMEBox(HWND hWnd, HDC hDC);  
static void ClrInBuf(void);  
static void SetT9KeysText();

static BOOL py_key_filter(char key);                //拼音输入键是否有效  
static void get_py_from_table(void);  
static void get_hz_from_table(char* py);  
static void get_en_from_table(char key);  
static void ime_writemsg(int sindex);  
static void delete_last_py(void);  
static bool is_mouse_in_client(HWND hWnd,int x_pos, int y_pos);
/* ********************************************************* */
/* *********************lyx added ended ******************** */
/* ********************************************************* */


static HWND CreateSkbT9Wnd(HWND hHosting);
/***************************************************************************
 *                          some local functions                           *
 ***************************************************************************/

bool is_mouse_in_client(HWND hWnd,int x_pos, int y_pos)
{
    RECT rect;
	if(!IsWindowEnabled(hWnd))    //非使能状态下过滤掉右键双击的第二次消息  chenwz modify 20130408
	{
		return false;
	}
    SetRect(&rect,
            0,
            0,
            1280,
            720);

    if (PtInRect(&rect, x_pos, y_pos))
    {
        return true;
    }
    return false;
}


static hz_input_table* load_input_method(char *filename)
{
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
  int i;
#endif

  int  nread;
  FILE *fd;
  char phrase_filename[100],assoc_filename[100];
  hz_input_table *table;
  
  table = calloc (1, sizeof(hz_input_table));
  if (table == NULL)
    return NULL;

  fd = fopen(filename, "r");
  if (fd == NULL) {
    free (table);
    return NULL;
  }

  nread = fread(table, sizeof(hz_input_table),1,fd);
  if (nread != 1) {
    goto fail;
  }
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
  table->TotalKey = ArchSwap32(table->TotalKey);
  table->MaxPress = ArchSwap32(table->MaxPress);
  table->MaxDupSel= ArchSwap32(table->MaxDupSel);
  table->TotalChar= ArchSwap32(table->TotalChar);
  table->PhraseNum= ArchSwap32(table->PhraseNum);
  for (i = 0; i < 64; i++) {
    table->KeyIndex [i] = ArchSwap16(table->KeyIndex[i]);
  }
#endif

  /* 设置每次显示的最大条项 */
  table->MaxDupSel = (table->MaxDupSel > 5) ? 5 : table->MaxDupSel;
  
  if( strcmp(MAGIC_NUMBER, table->magic_number) ) {
    goto fail;
  }

  table->item = (ITEM *)malloc (sizeof(ITEM) * table->TotalChar); 
  if ( table->item == NULL ) {
    goto fail;
  }
 
  if (fread (table->item, sizeof(ITEM), table->TotalChar, fd) < table->TotalChar)
    goto fail;

#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
  for (i = 0; i < table->TotalChar; i++) {
      table->item[i].key1 = ArchSwap32(table->item[i].key1);
      table->item[i].key2 = ArchSwap32(table->item[i].key2);
      table->item[i].frequency = ArchSwap16(table->item[i].frequency);
  }
#endif

  fclose (fd);
 
  if (table->PhraseNum > 0) {
     strcpy( phrase_filename, filename );
     strcat( phrase_filename, ".phr" );
     strcpy( assoc_filename, filename );
     strcat( assoc_filename, ".lx");
     
     table->PhraseFile = fopen( phrase_filename, "r" );
     table->AssocFile = fopen( assoc_filename, "r");

     if (table->PhraseFile == NULL || table->AssocFile == NULL) {
          printf("Load Phrase/Assoc File error!\n");
          free(table->item);
          free(table);
          return NULL;
     }
  }
  else {
       table->PhraseFile = table->AssocFile = NULL;
  }

  return table;

fail:
  free (table->item);
  free (table);
  fclose (fd);
  return NULL;
}

static void free_input_method(hz_input_table *table)
{
    free(table->item);
    free(table);
}

static void ClrIn(void)
{
  bzero( InpKey,   sizeof( InpKey   ) );
  bzero( seltab, sizeof( seltab ) );

  MultiPageMode = NextPageIndex = CurrentPageIndex = 0;
  CurSelNum = InputCount = InputMatch = 0;
  IsAssociateMode = 0;   /* lian xiang */
}

static void FindAssociateKey(int index)
{
    FILE *fp = cur_table->AssocFile;
    int ofs[2],offset;
   
    if (index < 0xB0A1) 
    {
       StartKey = EndKey = 0;
       return;  /* no match */
    }  
   
    offset = (index / 256 - 0xB0) * 94 + index % 256 - 0xA1; 
    fseek(fp, offset * sizeof(int), SEEK_SET);
    fread(ofs,sizeof(int),2,fp);
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    ofs[0] = ArchSwap32(ofs[0]);
    ofs[1] = ArchSwap32(ofs[1]);
#endif
    StartKey = 72 * 94 + 1 + ofs[0];
    EndKey = 72 * 94 + 1 + ofs[1];
}

static void load_phrase( int phrno, char *tt )
{
  FILE *fp = cur_table->PhraseFile;
  int ofs[2], len;

  fseek( fp, ( phrno + 1 ) << 2, SEEK_SET );
  fread( ofs, sizeof(int), 2, fp );
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
  ofs[0] = ArchSwap32(ofs[0]);
  ofs[1] = ArchSwap32(ofs[1]);
#endif
  len = ofs[1] - ofs[0];

  if ( len > 128 || len <= 0 ) {
     strcpy( tt, "error" );
     return;
  }

  ofs[0] += ( cur_table->PhraseNum + 1 ) << 2;
   /* Add the index area length */
  fseek( fp, ofs[0], SEEK_SET );
  fread( tt, 1, len, fp );
  tt[len] = 0;
}

static void putstr (unsigned char *p ,LPARAM lParam)
{
    int index,len = strlen((char *)p);
    writemsg_ex(p, len, lParam, TRUE);
    if (InputCount <= InputMatch)  /* All Match */
    {
        index = (int)p[len-2] * 256 + p[len-1]; 
        ClrIn();
        if (UseAssociateMode)
        {
             FindAssociateKey(index); 
             CurrentPageIndex = StartKey;
             MultiPageMode = 0;
             FillAssociateChars(StartKey);  
             if (CurSelNum > 0)
             {
                IsAssociateMode = 1;
             }
        }
    }
    else
    {
      int nCount = InputCount - InputMatch,nMatch = InputMatch,i;
      MultiPageMode = NextPageIndex = CurrentPageIndex = 0;
      InputCount = InputMatch = 0;

      for(i = 0; i < nCount; i++)
        save_InpKey[i] = InpKey[nMatch+i];

      bzero(InpKey, sizeof(InpKey));
      for(i = 1; i <= nCount; i++)  /* feed the additional keys */
      {
         InpKey[InputCount] = save_InpKey[InputCount];
         InputCount++;
         if (InputCount <= InputMatch+1)
         {
             FindMatchKey();
             MultiPageMode = 0;
             CurrentPageIndex = StartKey;
             FillMatchChars(StartKey);
         } 
      }
      if (InputMatch == 0)    /* left key has no match, delete */
      {
         ClrIn(); 
         return;
      }
    }
}

static int epunc2cpunc (unsigned char epunc, unsigned char* cpunc)
{
    int i;
    
    for (i = 0; i < 29; i++) {
        if (puncmark [i].epunc == epunc) {
            switch (puncmark [i].type)
            {
                case CPUNCTYPE_1TO1:
                    memcpy (cpunc, puncmark [i].cpunc, 2);
                    return 2;
                case CPUNCTYPE_1TO2:
                    memcpy (cpunc, puncmark [i].cpunc, 4);
                    return 4;
                case CPUNCTYPE_CYCLE:
                    memcpy (cpunc, puncmark[i].cpunc + puncmark[i].currone, 2);
                    if (puncmark[i].currone == 0)
                        puncmark[i].currone = 2;
                    else
                        puncmark[i].currone = 0;
                    return 2;
                default:
                    return 0;
            }
        }
    }

    return 0;
}

/* After add/delete a char, search the matched char/phrase, update the
   StartKey/EndKey key,  save the related keys at first, if no match
   is found, we may restore its original value
*/
static void FindMatchKey(void)
{
  save_StartKey = StartKey;
  save_EndKey = EndKey;
  save_MultiPageMode = MultiPageMode;
  save_NextPageIndex = NextPageIndex;
  save_CurrentPageIndex = CurrentPageIndex;

  val1 = InpKey[4] | (InpKey[3]<<6) | (InpKey[2]<<12) | (InpKey[1]<<18) | 
        (InpKey[0]<<24);
  val2 = InpKey[9] | (InpKey[8]<<6) | (InpKey[7]<<12) | (InpKey[6]<<18) | 
        (InpKey[5]<<24);

  if (InputCount == 1)
     StartKey = cur_table->KeyIndex[InpKey[0]];
  else
     StartKey = CharIndex[InputCount-1];

  EndKey = cur_table->KeyIndex[InpKey[0]+1];

  for (; StartKey < EndKey; StartKey++)
  {
      key1 = (cur_table->item[StartKey].key1 & mask[InputCount+5]);
      key2 = (cur_table->item[StartKey].key2 & mask[InputCount]);

      if (key1 > val1) break;
      if (key1 < val1) continue;
      if (key2 < val2) continue;
      break;
  }
  CharIndex[InputCount] = StartKey;
}

/*  Find the matched chars/phrases and fill it into SelTab
    The starting checked index is j 
 
    The Selection Line 1xxx 2xxx,  80-20=60 60/2=30 chinese chars only
    0-9 Selection can contain only 30 chinese chars
*/

static void FillAssociateChars(int index)
{
    unsigned char str[25];
    int PhraseNo, CurLen = 0;
   
    CurSelNum = 0; 
    while( CurSelNum < cur_table->MaxDupSel && index < EndKey &&
              CurLen < MAX_SEL_LENGTH)
    {
         fseek( cur_table->AssocFile, index << 2, SEEK_SET );
         fread( &PhraseNo, sizeof(int), 1, cur_table->AssocFile );
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
         PhraseNo = ArchSwap32(PhraseNo);
#endif
         load_phrase( PhraseNo, (char *)str );
         strcpy((char *)seltab[CurSelNum],(char *)str+2);
         CurLen += strlen(seltab[CurSelNum++]);
         index++;
    }

    /* check if more than one page */
    if ( index < EndKey && CurSelNum == cur_table->MaxDupSel )
    {
      /* has another matched key, so enter MultiPageMode, has more pages */
        NextPageIndex = index;
        MultiPageMode = 1;
    }
    else if (MultiPageMode)
    {
        NextPageIndex = StartKey; /* rotate selection */
    }
    else MultiPageMode = 0;
}

static void FillMatchChars(int j)
{
    int SelNum = 0, CurLen = 0;
    //bzero( seltab, sizeof( seltab ) );

    while( ( cur_table->item[j].key1 & mask[InputCount+5] ) == val1 &&
            ( cur_table->item[j].key2 & mask[InputCount] ) == val2 &&
              SelNum < cur_table->MaxDupSel && j < EndKey && 
              CurLen < MAX_SEL_LENGTH)
    {
          unsigned short ch = cur_table->item[j].ch;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
          ch = ArchSwap16 (ch);
#endif

          if (ch < 0xA1A1)
              load_phrase (ch, seltab[SelNum]);
          else
          {
              memcpy (&seltab[SelNum], &(cur_table->item[j].ch), 2);
              seltab [SelNum][2] = '\0';
          }
          CurLen += strlen(seltab[SelNum++]);
          j++;
    }
    
    if (SelNum == 0)  /* some match found */
    {
        StartKey = save_StartKey;
        EndKey = save_EndKey;
        MultiPageMode = save_MultiPageMode;
        NextPageIndex = save_NextPageIndex;
        CurrentPageIndex = save_CurrentPageIndex;
        return;    /* keep the original selection */
    }

    CurSelNum = SelNum;

    for(SelNum = CurSelNum; SelNum < 16; SelNum++)
       seltab[SelNum][0] = '\0';  /* zero out the unused area */
    InputMatch = InputCount; /* until now we have some matches */

    /* check if more than one page */
    if ( j < EndKey && (cur_table->item[j].key1 & mask[InputCount+5]) == val1 &&
         ( cur_table->item[j].key2 & mask[InputCount] ) == val2 &&
          CurSelNum == cur_table->MaxDupSel )
    {
      /* has another matched key, so enter MultiPageMode, has more pages */
        NextPageIndex = j;
        MultiPageMode = 1;
    }  
    else if (MultiPageMode)
    {
        NextPageIndex = StartKey; /* rotate selection */
    }
    else MultiPageMode = 0;
}

/* return value: Has output? */
static BOOL hz_filter(unsigned char key, LPARAM lParam)
{
    int inkey = 0,vv;
    char *is_sel_key = (char*)0;

    if (IsHanziInput) {
        unsigned char cc [2];

        if (IsFullPunc) {
            unsigned char cpunc[4];
            int len;
        
            len = epunc2cpunc (key, cpunc);

            if (len != 0) {
                writemsg_ex(cpunc, len, lParam, TRUE);
                ClrIn();
                return TRUE;
            }
        }
    
        if (IsFullChar && key >= ' ' && key <= 127) {
            key = (key - ' ') << 1;
            cc[0] = (unsigned char)(fullchar[key]);
            cc[1] = (unsigned char)(fullchar[key+1]);
       
            writemsg_ex(cc, 2, lParam, TRUE);
            return FALSE;
        }
    }
    else {
        outchar_ex (key, lParam);
        return FALSE;
    }

    if (CurIME == 0) {
        intcode_hz_filter(key, lParam);
        return TRUE;
    }

#ifdef _IME_GB2312_PINYIN
    if (CurIME == 1) {
        Pinyin_HZFilterEx (&Pinyin_Module_Ex, key, lParam);
        return TRUE;
    }
#endif

    switch ( key ) {
    case '\010':  /* BackSpace Ctrl+H */
    case '\177':  /* BackSpace */
        if ( InputCount > 0 ) 
        {
           InpKey[--InputCount]=0;
           if (InputCount == 0)
           {
               ClrIn();
  
           }
           else if (InputCount < InputMatch)
           {
               FindMatchKey();
               MultiPageMode = 0;
               CurrentPageIndex = StartKey;
               FillMatchChars(StartKey); 
           }
  
        }
        else outchar_ex(key,lParam);
        break;

     case '\033':  /* ESCAPE */
        if (InputCount > 0) 
        {
           ClrIn();
 
        }
        else outchar_ex(key,lParam);
        break;

     case '-':
        if ( MultiPageMode )
        {
           if ( CurrentPageIndex > StartKey) 
                CurrentPageIndex = CurrentPageIndex - cur_table->MaxDupSel;
           else CurrentPageIndex = StartKey;
           if (IsAssociateMode)
                FillAssociateChars(CurrentPageIndex);
           else FillMatchChars(CurrentPageIndex);
 
        }
        else outchar_ex(key,lParam);
        break;

     case '=':
        if ( MultiPageMode )
        {
           CurrentPageIndex = NextPageIndex;
           if (IsAssociateMode)
               FillAssociateChars(CurrentPageIndex);
           else FillMatchChars(CurrentPageIndex);
           
        }
        else outchar_ex(key,lParam);
        break;

     case ' ':
        if ( CurSelNum == 0 )
           outchar_ex(key,lParam);
        if ( seltab[0][0] )
           putstr((unsigned char *)seltab[0],lParam);
        break;

     default:

        inkey   = cur_table->KeyMap[key];
        is_sel_key = strchr( cur_table->selkey, key);
        vv = is_sel_key - cur_table->selkey; 
          /* selkey index, strchr may return NULL */
 
      /* if a key is simultaneously inkey & is_sel_key, then selkey first?*/
        if ( (!inkey && !is_sel_key) || 
             (!inkey && is_sel_key && (CurSelNum == 0 || seltab[vv][0] == 0)) )
        {
           IsAssociateMode = 0;
           ClrIn();
           outchar_ex(key,lParam);
           break;
        }

        if (is_sel_key && CurSelNum > 0 && seltab[vv][0])
        {
            putstr((unsigned char *)seltab[vv],lParam);
            break;
        }
        
        /* now it must be inkey? */
        IsAssociateMode = 0; 
        if ( inkey >= 1 && InputCount < MAX_INPUT_LENGTH )
            InpKey[InputCount++] = inkey;
     
        if (InputCount <= InputMatch+1) 
        {
             FindMatchKey();
             CurrentPageIndex = StartKey;
             MultiPageMode = 0;
             FillMatchChars(StartKey);
             if (InputCount >= cur_table->MaxPress && CurSelNum == 1 &&
                 cur_table->last_full)
             {
                // left only one selection 
                 putstr((unsigned char *)seltab[0],lParam);
             }
        }
        
        break;

  } /* switch */

  return TRUE;
}


static void DispSelection(HDC hDC)
{
  int i, pos = 1;

  char str[100];
  char minibuf[2];
  minibuf[1]=0;
  str[0]=0;
  if (MultiPageMode && CurrentPageIndex != StartKey) 
  {
    strcat(str,"< ");
      pos += 2;
  }

  for( i = 0; i < CurSelNum; i++ )
  {
     if ( !seltab[i][0] ) {
        if (CurIME == 0 && i == 0) 
	        continue; 
	    else
	        break;
     }

     minibuf[0]= input_table[CurIME]->selkey[i];
     strcat(str,minibuf);
     strcat(str,seltab[i]);
     strcat(str," "); 
  }

  if ( MultiPageMode && NextPageIndex != StartKey)  
  {
     strcat (str,"> ");
  }
  
  if (bTwoLines)
    TextOut (hDC, 2, 18, str);
  else
    TextOut (hDC, 150, 2, str);
}

static void unload_input_table(int i)
{
  if (((i >= 0) && (i < 10)) && input_table[i])
  {
    if (i == CurIME)
    {
      if (IsHanziInput)
          toggle_input_method();
      CurIME = 0;
    }

    if (input_table[i]->PhraseFile)
        fclose(input_table[i]->PhraseFile);
    if (input_table[i]->AssocFile)
        fclose(input_table[i]->AssocFile);

    free_input_method(input_table[i]);
    input_table[i] = NULL;
  }
}

static int toggle_input_method(void)
{
	
	BOOL bToggle = FALSE;
	if (SKBTYPE_QUERTY == sg_nSkbType)
	{	  	
		if ((SKBATTR_ST_CN != sg_nSkbAttrSt)
			&& (SKBATTR_CN & sg_dwSkbAttr))
		{
			bToggle = TRUE;
			sg_nSkbAttrSt = SKBATTR_ST_CN;
		}
		else if((SKBATTR_ST_EN != sg_nSkbAttrSt)
				&& (SKBATTR_EN & sg_dwSkbAttr))
		{
			bToggle = TRUE;
			sg_nSkbAttrSt = SKBATTR_ST_EN;
		}			

		if (bToggle
			&& (input_table[CurIME] || CurIME == 1))
	  	{
	  		IsHanziInput ^= 1;
		    cur_table = input_table[CurIME];
		    ClrIn();
	  	}
		
	}
	else /* if(SKBTYPE_T9 == sg_nSkbType) */
	{
		if (SKBATTR_ST_EN == sg_nSkbAttrSt)
		{
			if (SKBATTR_CN & sg_dwSkbAttr)
			{
				bToggle = TRUE;
				sg_nSkbAttrSt = SKBATTR_ST_CN;				
			}
			else if(SKBATTR_NUM & sg_dwSkbAttr)
			{
				bToggle = TRUE;
				sg_nSkbAttrSt = SKBATTR_ST_NUM;
			}
		}
		
		else if(SKBATTR_ST_CN == sg_nSkbAttrSt)
		{
			if (SKBATTR_NUM & sg_dwSkbAttr)
			{
				bToggle = TRUE;
				sg_nSkbAttrSt = SKBATTR_ST_NUM;				
			}
			else if(SKBATTR_EN & sg_dwSkbAttr)
			{
				bToggle = TRUE;
				sg_nSkbAttrSt = SKBATTR_ST_EN;
			}
		}
		else if (SKBATTR_ST_NUM == sg_nSkbAttrSt)
		{
			if (SKBATTR_EN & sg_dwSkbAttr)
			{
				bToggle = TRUE;
				sg_nSkbAttrSt = SKBATTR_ST_EN;				
			}
			else if(SKBATTR_CN & sg_dwSkbAttr)
			{
				bToggle = TRUE;
				sg_nSkbAttrSt = SKBATTR_ST_CN;
			}
		}
			
		if (bToggle)
		{
			ClrInBuf();
			//SetT9KeysText();
		}	
			
	}
  

  return IsHanziInput;
}

static void SetT9KeysText()
{
	if(!IsWindow(sg_hSkbT9))
		return;

	if(SKBATTR_ST_NUM == sg_nSkbAttrSt)
	{
		PostMessage(sg_hT9SKeys.hT9Sk1, MSG_SETTEXT, 0, (LPARAM)"1");
		PostMessage(sg_hT9SKeys.hT9Sk2, MSG_SETTEXT, 0, (LPARAM)"2");
		PostMessage(sg_hT9SKeys.hT9Sk3, MSG_SETTEXT, 0, (LPARAM)"3");
		PostMessage(sg_hT9SKeys.hT9Sk4, MSG_SETTEXT, 0, (LPARAM)"4");
		PostMessage(sg_hT9SKeys.hT9Sk5, MSG_SETTEXT, 0, (LPARAM)"5");
		PostMessage(sg_hT9SKeys.hT9Sk6, MSG_SETTEXT, 0, (LPARAM)"6");
		PostMessage(sg_hT9SKeys.hT9Sk7, MSG_SETTEXT, 0, (LPARAM)"7");
		PostMessage(sg_hT9SKeys.hT9Sk8, MSG_SETTEXT, 0, (LPARAM)"8");
		PostMessage(sg_hT9SKeys.hT9Sk9, MSG_SETTEXT, 0, (LPARAM)"9");
	}
	else
	{
		PostMessage(sg_hT9SKeys.hT9Sk1, MSG_SETTEXT, 0, (LPARAM)"1.,");
		PostMessage(sg_hT9SKeys.hT9Sk2, MSG_SETTEXT, 0, (LPARAM)"2ABC");
		PostMessage(sg_hT9SKeys.hT9Sk3, MSG_SETTEXT, 0, (LPARAM)"3DEF");
		PostMessage(sg_hT9SKeys.hT9Sk4, MSG_SETTEXT, 0, (LPARAM)"4GHI");
		PostMessage(sg_hT9SKeys.hT9Sk5, MSG_SETTEXT, 0, (LPARAM)"5JKL");
		PostMessage(sg_hT9SKeys.hT9Sk6, MSG_SETTEXT, 0, (LPARAM)"6MNO");
		PostMessage(sg_hT9SKeys.hT9Sk7, MSG_SETTEXT, 0, (LPARAM)"7PQRS");
		PostMessage(sg_hT9SKeys.hT9Sk8, MSG_SETTEXT, 0, (LPARAM)"8TUV");
		PostMessage(sg_hT9SKeys.hT9Sk9, MSG_SETTEXT, 0, (LPARAM)"9WXYZ");
	}

	if (SKBATTR_NUM == (sg_dwSkbAttr & SKBATTR_MASK))
	{
		PostMessage(sg_hT9SKeys.hT9SkOk, MSG_SETTEXT, 0, (LPARAM)".");
	}
	else
	{
		PostMessage(sg_hT9SKeys.hT9SkOk, MSG_SETTEXT, 0, (LPARAM)"Shift");
	}
}

static void toggle_half_full(void)
{
   IsFullChar ^= 1;
   ClrIn();
}

static void toggle_punc (void)
{
   IsFullPunc ^= 1;

   ClrIn();
}

static BOOL hz_input_init(void)
{
    char szFileName[MAX_PATH + 1];
    char szKey[10];
    char szValue[MAX_NAME + 1];
    int number;
    int i;

    if( GetMgEtcValue ("imeinfo", "imetabpath",
            ime_tab_path, MAX_PATH) < 0 ) {
        fprintf (stderr, "IME: Get imetab file path error!\n");
        return FALSE;
    }

    IsImeOpened = 0;
    IsHanziInput = 0;	// default is hanzi input 
    IsFullChar = 0;
    IsFullPunc = 0;
    CurIME = 0;
    input_table[0] = IntCode_Init();
    if (input_table[0] == NULL)
        return FALSE;

#ifdef _IME_GB2312_PINYIN
    if (!InitPinyinInputEx (ime_tab_path, &Pinyin_Module_Ex)) {
        fprintf (stderr, "IME: Init Pinyin module failure!\n");
        unload_input_table (0);
        return FALSE;
    }
    nIMENr = 2;
#else
    nIMENr = 1;
#endif

    if( !LoadSystemBitmap (&sg_bmpIME, SYSBMP_IMECTRLBTN) ){
		fprintf (stderr, "IME: Load the control button failure!\n");
		return FALSE;
	}

    if( GetMgEtcValue  ("imeinfo", "imenumber",
            szValue, 10) < 0 )
        goto error;
        
    number = atoi (szValue);
    if(number <= 0)
        goto error;

    number = (number < NR_INPUTMETHOD) ? number : NR_INPUTMETHOD;

    for (i = 0; i < number; i++) {
        sprintf(szKey, "ime%d", i);
        if( GetMgEtcValue ("imeinfo",
            szKey, szValue, MAX_NAME) < 0 )
        goto error;
        
#ifdef _DEBUG
        fprintf (stderr, "IME: Loading Input Method %d: %s\n",i, szValue);
#endif

        strcpy(szFileName, ime_tab_path);
        strcat(szFileName, szValue);
        strcat(szFileName, ".tab");
        input_table [i + nIMENr] = load_input_method(szFileName);

        if (!input_table [i + nIMENr])
            goto error;
    }

    nIMENr += number;

    if (nIMENr > 1)
        CurIME = 1;
    cur_table = input_table [CurIME];

    return TRUE;
    
error:
#ifdef _IME_GB2312_PINYIN
    for (i = 0; i < nIMENr; i++) {
        if (i != 1)
            unload_input_table (i);
    }
    PinyinInputCleanupEx (ime_tab_path, &Pinyin_Module_Ex);
#else
    for (i = 0; i < nIMENr; i++)
        unload_input_table (i);
#endif

    return FALSE;
}

static void hz_input_done(void)
{
  int i;

  for (i = 0; i < NR_INPUTMETHOD; i++)
    if (input_table[i]) {
        if (input_table[i]->PhraseFile)
            fclose(input_table[i]->PhraseFile);
        if (input_table[i]->AssocFile)
            fclose(input_table[i]->AssocFile);

        free_input_method(input_table[i]);
    }

#ifdef _IME_GB2312_PINYIN
    PinyinInputCleanupEx (ime_tab_path, &Pinyin_Module_Ex);
#endif
}

static void set_active_input_method(int active)
{
#if _IME_GB2312_PINYIN
  if (input_table[active] || active == 1)
#else
  if (input_table[active])
#endif
  {
    CurIME = active;
    cur_table = input_table[active];
    ClrIn();
#if _IME_GB2312_PINYIN
    if (CurIME == 1 || CurIME == 0)
#else
    if (CurIME == 0)
#endif
        UseAssociateMode = 0;
    else 
        UseAssociateMode = 1;
  }
}

#define ARROW_H          11
#define MCCLR_DF_ARROW            0xFF4297D2//PIXEL_black
#define MCCLR_DF_ARROWHIBK        PIXEL_lightwhite
#define MCCLR_DF_TITLEBK       RGB2Pixel(HDC_SCREEN, 0x5D, 0x5D, 0x5D) 

// draw page arrow
static void hzDrawPageArrow (HDC hdc, RECT* prcArrow, BOOL bFaceR, BOOL bHilight)
{
    int arrow_w, arrow_h = ARROW_H;
    int line_x, line_h, line_y;
    int stepx;
    int rc_w, rc_h;

	int clr_arrow = MCCLR_DF_ARROW;

	int clr_arrowHibk = MCCLR_DF_ARROWHIBK;
	int clr_titlebk = WND_BK_COLOR;//MCCLR_DF_TITLEBK;

    arrow_w = (arrow_h + 1)/2;
    rc_w = prcArrow->right - prcArrow->left;
    rc_h = prcArrow->bottom - prcArrow->top;
    SetBkMode (hdc, BM_TRANSPARENT);
    SetBkColor(hdc, WND_BK_COLOR);
    SetPenColor (hdc, clr_arrow);
    if (bHilight) {
        SetBrushColor (hdc, clr_arrowHibk);
        FillBox (hdc, prcArrow->left, prcArrow->top, rc_w, rc_h);
    }
    else {
        SetBrushColor (hdc, clr_titlebk);
        FillBox (hdc, prcArrow->left, prcArrow->top, rc_w, rc_h);
    }
    line_y = prcArrow->top + (rc_h-arrow_h)/2;
    line_h = arrow_h;
    if (bFaceR) {
        stepx = 1;
        line_x = prcArrow->left + (rc_w - arrow_w)/2;
    }
    else {
        stepx = -1;
        line_x = prcArrow->right - (rc_w - arrow_w)/2 - 1;
    }
    while (line_h > 0) {
        MoveTo (hdc, line_x, line_y);
        LineTo (hdc, line_x, line_y+line_h);
        line_x += stepx;
        line_y ++;
        line_h -= 2;
    }
}


static void DrawHzPageButton(InputModule *inmd, HWND hwnd, HDC hDC)
{
	int lx = IME_QUERTY_WND_W-60,rx = IME_QUERTY_WND_W-30;
	RECT lButton = {lx, IME_WND_H/2-10, lx+10, IME_WND_H-10};
	RECT rButton = {rx, IME_WND_H/2-10, rx+10, IME_WND_H-10};
	//printf("@@@@@@@@@@@@@@@@ inmd->inbuf: %d\r\n",strlen(inmd->inbuf));
	if(strlen(inmd->inbuf))
	{
		hzDrawPageArrow(hDC, &lButton, FALSE, FALSE);
		hzDrawPageArrow(hDC, &rButton, TRUE, FALSE);
	}
}

static void refresh_input_method_area(HWND hwnd, HDC hDC)
{
    RECT rc;
    char str[100];
    int i;
    char minibuf[2];
	PLOGFONT logfont;
    
    GetClientRect (hwnd, &rc);
	SetBkMode(hDC, BM_TRANSPARENT);
	SetTextColor(hDC, PIXEL_lightwhite);  

	 logfont = GetSystemFont(SYSLOGFONT_FIXED);  
	SelectFont (hDC, logfont);

	if (SKBTYPE_QUERTY == sg_nSkbType)
	{
	    if (!IsHanziInput)
	    {
	    	TextOut(hDC, 2, 2, LTR("【英文】"));
	    }
#ifdef _IME_GB2312_PINYIN
	    else if (CurIME != 1)
#else
	    else
#endif
	    {
	        minibuf[1] = 0;
			strcpy (str, input_table[CurIME]->cname);
	        
	        for( i = 0; i <= MAX_INPUT_LENGTH ; i++)
	        {
	            if (i < InputCount)
	                minibuf[0] = input_table[CurIME]->KeyName[InpKey[i]];
	            else 
	                minibuf[0]=' ';
	                
	            if (i == InputMatch && InputCount > InputMatch && i != 0)
	                strcat (str,"'");
	                
	            strcat (str,minibuf);
	        }
	        
	        TextOut (hDC, 2, 2, str);                                        
	        DispSelection (hDC);
	    }
#ifdef _IME_GB2312_PINYIN
	    else
	    {
	    	RefreshPYInputAreaEx (&Pinyin_Module_Ex, hDC, bTwoLines);
	    	DrawHzPageButton(&Pinyin_Module_Ex,hwnd, hDC);
	    }
#endif
	}
	else if (SKBTYPE_T9 == sg_nSkbType)
	{
		RefreshIMEBox(hwnd, hDC);	
	}
}

/*************************************************************************
 *                       Internal Code Input Method                      *
 *************************************************************************/
static hz_input_table *IntCode_Init(void)
{
  hz_input_table *table;
  int i,index;

  UseAssociateMode = 1;  /* force to no associate */
  table = malloc(sizeof(hz_input_table));
  if (table == NULL)
     {
#ifdef _DEBUG
      fprintf(stderr, "IME: load_input_method");
#endif
      return NULL;
     } 

   /* reset to zero. */
   memset (table, 0, sizeof (hz_input_table));

   strcpy(table->magic_number,MAGIC_NUMBER);
   strcpy(table->ename, "IntCode");
   strcpy(table->cname, "【内码】");
   strcpy(table->selkey, "0123456789abcdef");

   table->last_full = 1;
   for(i = 0; i < 128; i++)
   {
      table->KeyMap[i] = 0;
      if ((i >= '0' && i <= '9') || (i >= 'a' && i <= 'f'))
      {
         if (i >= '0' && i <= '9')
            index = i - '0';
         else index = i -'a' + 10;
         table->KeyMap[i] = index;
         table->KeyName[index] = toupper(i);
      }
   }    
   return table;
}

/* StartKey <= index < EndKey is valid */
static void IntCode_FindMatchKey(void)
{
   unsigned long Key = (InpKey[0] << 12) | (InpKey[1] << 8);
   switch(InputCount)
   {
      case 0:
      case 1:
         StartKey = EndKey = 0;    /* not display selection */
         break;
      case 2:
         StartKey = Key + 0xA1; 
         EndKey = Key + 0xFF;  /* A1-A9,B0-F7 A1-FE */
         break;
      case 3:
         StartKey = Key + (InpKey[2] << 4);
         if (InpKey[2] == 10) StartKey++; /* A1 */
         EndKey = StartKey + 0x10;  
         if (InpKey[2] == 15) EndKey--;  /* FE */ 
         break;
    }
}

/* ABCD  AB then C=a..f, Sel=0-9 begin display selection
        ABC then D=0-f,  Sel=0-f 
*/
static void IntCode_FillMatchChars(int index)
{
    int MaxSel,i;
   
    CurSelNum = 0;
    if (InputCount < 2) return;
    if (InputCount == 2) MaxSel = 10; else MaxSel = 16;
    
    if (index % 256 == 0xA1 && InputCount == 3) 
    {
       seltab[0][0] = '\0';
       CurSelNum++;
    }

    while( CurSelNum < MaxSel && index < EndKey)
    {
         seltab[CurSelNum][0] = index / 256; 
         seltab[CurSelNum][1] = index % 256;
         seltab[CurSelNum][2] = '\0';
         CurSelNum++;
         index++;
    }
    
    for(i = CurSelNum; i < 16; i++)
       seltab[i][0] = '\0';  /* zero out the unused area */

    InputMatch = InputCount;

    /* check if more than one page */
    if ( index < EndKey && CurSelNum == MaxSel && MaxSel == 10)
    {
      /* has another matched key, so enter MultiPageMode, has more pages */
        NextPageIndex = index;
        MultiPageMode = 1;
    }  
    else if (MultiPageMode)
    {
        NextPageIndex = StartKey; /* rotate selection */
    }
    else MultiPageMode = 0;
}

static void intcode_hz_filter(unsigned char key,LPARAM lParam)
{
  int inkey = 0;

  switch ( key )
  {
     case '\010':  /* BackSpace Ctrl+H */
     case '\177':  /* BackSpace */
        if ( InputCount > 0 ) 
        {
           InpKey[--InputCount]=0;
           if (InputCount == 0)
           {
               ClrIn();
           }
           else if (InputCount >= 2)
           {
               IntCode_FindMatchKey();
               MultiPageMode = 0;
               CurrentPageIndex = StartKey;
               IntCode_FillMatchChars(StartKey); 
               
           }
        }
        else outchar_ex(key,lParam);
        break;

   case '\033':  /* ESCAPE */
        if (InputCount > 0) 
        {
           ClrIn();
        }
        else outchar_ex(key,lParam);
        break;

     case '-':
        if ( MultiPageMode )
        {
           if ( CurrentPageIndex > StartKey) 
                CurrentPageIndex = CurrentPageIndex - 10;
           else CurrentPageIndex = StartKey;
           IntCode_FillMatchChars(CurrentPageIndex);
           
        }
        else outchar_ex(key,lParam);
        break;

     case '=':
        if ( MultiPageMode )
        {
           CurrentPageIndex = NextPageIndex;
           IntCode_FillMatchChars(CurrentPageIndex);
           
        }
        else outchar_ex(key,lParam);
        break;

     case ' ':
        if ( CurSelNum == 0 )
           outchar_ex(key,lParam);
        if ( seltab[0][0] )
           putstr((unsigned char *)seltab[0],lParam);
        break;

     default:
        inkey   = cur_table->KeyMap[key];
        switch(InputCount)
        {
           case 0:  /* A-F */
           case 1:  /* 0-F */
           case 2:  /* A-F */
              if (inkey >=0 && !(InputCount != 1 && inkey < 10) &&
                   !(InputCount == 1 && 
                      ( (InpKey[0]==10 && inkey>9) ||       //A1-A9 
                        (InpKey[0]==10 && inkey==0) ||
                        (InpKey[0]==15 && inkey>7)) ) )      //B0-F7  
              {
                  InpKey[InputCount++] = inkey;
                  
                  if (InputCount >=2)
                  {
                      IntCode_FindMatchKey();
                      MultiPageMode = 0;
                      CurrentPageIndex = StartKey;
                      IntCode_FillMatchChars(StartKey);
                  
                  }
                  
              }
              else if (InputCount == 2 && inkey >= 0 && inkey < CurSelNum &&
                       seltab[inkey][0])
              {
                   putstr((unsigned char *)seltab[inkey],lParam);
              }
              else /* not select key or input key */
              {
                  ClrIn();
                  outchar_ex(key,lParam);
                  return;
              }
              break;
           case 3:   /* output char */
              if (inkey >=1 && !(InpKey[2] == 10 && inkey == 0) &&
                  !(InpKey[2] == 15 && inkey == 15))
              {
                  seltab[0][0] = (InpKey[0] << 4) | InpKey[1];
                  seltab[0][1] = (InpKey[2] << 4) | inkey ;
                  seltab[0][2] = '\0';
                  putstr((unsigned char *)seltab[0],lParam);
              }
              else
              {
                  ClrIn();
                  outchar_ex(key,lParam);
                  return;
              }
              break;
        }
  } /* switch */
}

/*************** below are interface to other modules **************/
/*  use these functions in other modules only */

/* set current active input method                   */
/* this function sets the active input method to ime */
/* ime value can be: 0 for internal code             */
/*                   1 for pinyin                    */
/*                   2 for wubi                      */
static void imeSetStatus(int iStatusCode, int Value)
{
    switch (iStatusCode) {
        case IS_ENABLE:
            if (Value && !IsHanziInput)
                toggle_input_method ();
            else if (!Value && IsHanziInput)
                toggle_input_method ();
        break;

        case IS_FULLCHAR:
            if (Value && !IsFullChar)
                toggle_half_full ();
            else if (!Value && IsFullChar)
                toggle_half_full ();
        break;

        case IS_FULLPUNC:
            if (Value && !IsFullPunc)
                toggle_punc ();
            else if (!Value && IsFullPunc)
                toggle_punc ();
        break;

        case IS_METHOD:
            if (Value < 0) {
                Value = (CurIME + 1) % nIMENr;
                set_active_input_method (Value);
            }
            else {
                if (Value >= nIMENr)
                    Value = nIMENr - 1;

                if (Value != CurIME)
                    set_active_input_method (Value);
                else
                    return;
            }
        break;

        default:
        break;
    }
}

static int imeGetStatus(int iStatusCode)
{
    switch (iStatusCode) {
        case IS_ENABLE:
            return IsHanziInput;
        break;

        case IS_FULLCHAR:
            return IsFullChar;
        break;

        case IS_FULLPUNC:
            return IsFullPunc;
        break;

        case IS_METHOD:
            return CurIME;
        break;

        default:
        break;
    }

    return 0;
}

static inline BOOL imeIsSpecialChar (int chr)
{
    if (chr == '\t' || chr == '\n' || chr == '\r')
        return TRUE;

    return FALSE;
}

static inline BOOL imeIsSpecialKey (int scancode)
{
    if (scancode == SCANCODE_TAB
            || scancode == SCANCODE_ENTER
            || scancode > SCANCODE_F1)
        return TRUE;

    return FALSE;
}

static inline BOOL imeIsEffectiveIME (void)
{
    return IsHanziInput || IsFullChar || IsFullPunc;
}

static void HideInputWnds()
{
	ShowWindow(sg_hImeWnd, SW_HIDE);			
	ShowWindow(sg_hSkbQuerty, SW_HIDE);
	ShowWindow(sg_hSkbT9, SW_HIDE );

	//InvalidateRect(GetParent(HWND hWnd), NULL, TRUE);
	RegisterMouseMsgHook (0, old_hook); 
	IsImeOpened = 0;	
}

#if defined(HIDE_CURSER) || defined(LOCK_SCREEN)		
extern unsigned old_tick_count;
extern unsigned is_mouse_down;
#endif
static int ime_msg_hook (void* context, HWND dst_wnd, int msg, WPARAM wParam, LPARAM lparam) 
{ 
	int x = 0, y = 0;	
	int mouse_x, mouse_y;
	RECT	rectKeyborad, rectImeBox;
	mouse_x = LOWORD (lparam);
    mouse_y = HIWORD (lparam);
	HWND hWnd, hKeyBoard;
#if defined(HIDE_CURSER) || defined(LOCK_SCREEN)		
	old_tick_count = GetTickCount ();
	if(msg == MSG_LBUTTONDOWN)
		is_mouse_down = 1;
	else if(msg == MSG_LBUTTONUP)
		is_mouse_down = 0;
#endif
	hWnd = (HWND)context;
	if (sg_nSkbType == SKBTYPE_QUERTY)
		hKeyBoard = sg_hSkbQuerty;
	else if (sg_nSkbType == SKBTYPE_T9)
		hKeyBoard = sg_hSkbT9;
	else
		return HOOK_GOON;

	
	GetWindowRect(sg_hImeWnd, &rectImeBox);	
	WindowToScreen(sg_hImeWnd, &x, &y);
	rectImeBox.right = x + RECTW(rectImeBox);
	rectImeBox.bottom = y + RECTH(rectImeBox);
	rectImeBox.left = x;
	rectImeBox.top = y;
	
	x = 0, y = 0;
	GetWindowRect(hKeyBoard, &rectKeyborad);
	WindowToScreen(hKeyBoard, &x, &y);
	rectKeyborad.right = x + RECTW(rectKeyborad);
	rectKeyborad.bottom = y + RECTH(rectKeyborad);
	rectKeyborad.left = x;
	rectKeyborad.top = y;

	RECT rectCtrl;
	HWND hCtrlWnd = GetFocusChild(sg_hTargetWnd);
	if (!IsControl(hCtrlWnd))
		return HOOK_GOON;

	int x1 = 0, y1 = 0;
	WindowToScreen(hCtrlWnd,&x1, &y1);
	
	GetWindowRect(hCtrlWnd, &rectCtrl);
	rectCtrl.right = x1 + RECTW(rectCtrl)+1;
	rectCtrl.bottom = y1 + RECTH(rectCtrl)+1;
	rectCtrl.left = x1-1;
	rectCtrl.top = y1-1;

	if (msg == MSG_LBUTTONDOWN)
	{	
		if( PtInRect(&rectKeyborad, mouse_x, mouse_y) || 
			PtInRect(&rectImeBox, mouse_x, mouse_y)   || 
			PtInRect(&rectCtrl, mouse_x, mouse_y)
		)
		{
		}
		else
		{			
			PostMessage(hWnd, MSG_IME_CLOSE, 0, 0);
		}
	}
	else if (msg == MSG_RBUTTONDOWN)
	{		
		PostMessage(hWnd, MSG_IME_CLOSE, 0, 0);
	}	
	
	return HOOK_GOON;
}

static void ShowInputWnds(HWND hwnd, int nShowType)
{
	if(!IsWindow(sg_hTargetWnd))
		return;
		
	HWND hCtrlWnd = GetFocusChild(sg_hTargetWnd);
	if (!IsControl(hCtrlWnd))
		return;
	
    BOOL bEdit = SendMessage (hCtrlWnd, MSG_DOESNEEDIME, 0, 0L);
	if (!bEdit && (0 == strcmp(GetClassName(hCtrlWnd), "MYEDIT")))
	{
		HideInputWnds();
		return;
	}
	if ((0 != strcmp(GetClassName(hCtrlWnd), "CTRLIPADDRESS"))
		&& (0 != strcmp(GetClassName(hCtrlWnd), "MYEDIT"))
		&& (0 != strcmp(GetClassName(hCtrlWnd), "CTRLDATE")))
	{
		HideInputWnds();
		return;
	}
	
	//初始化标志位
	ReinitSKB(hwnd, nShowType);

	if ((0 != nShowType)
		 &&(SKBATTR_NUM == (SKBATTR_MASK & sg_dwSkbAttr)))
	{
		HideInputWnds();
		return;
	}

	int new_x = 0, new_y = 0, new_skb_x = 0, new_skb_y = 0;
	int maxx, maxy, w_client, h_client, w_area, h_area;
	RECT rcTargetWnd; //rcIme, rcQuerty, rcT9;
	
	ClrIn();					//QUERY软键盘清空
	ClrInBuf();					//T9软键盘逻辑参数清空
	init_ime_parameter();		//初始化T9键盘输入逻辑参数	
	
	w_client = (SKBTYPE_QUERTY == sg_nSkbType) ? 
		IME_QUERTY_WND_W : IME_T9_WND_W;
	h_client = IME_WND_H;
	w_area = w_client;
	h_area = (SKBTYPE_QUERTY == sg_nSkbType) ?
		(h_client + SKB_QUERY_WND_H) : (h_client + SKB_T9_WND_H);
		
	maxx = GetGDCapability (HDC_SCREEN, GDCAP_HPIXEL);
	maxy = GetGDCapability (HDC_SCREEN, GDCAP_VPIXEL);	
	GetClientRect(hCtrlWnd, &rcTargetWnd);
	ClientToScreen(hCtrlWnd, &rcTargetWnd.left, &rcTargetWnd.top);
	ClientToScreen(hCtrlWnd, &rcTargetWnd.right, &rcTargetWnd.bottom);
	if((maxx - rcTargetWnd.left) < w_area)
		new_x = rcTargetWnd.right - w_area;
	else
		new_x = rcTargetWnd.left;

	if((maxy - rcTargetWnd.bottom) < (h_area +2))
		new_y = rcTargetWnd.top - h_area;
	else 
		new_y = rcTargetWnd.bottom+2;

	if(SKBTYPE_QUERTY == sg_nSkbType)
	{
		int index;
		GetDisplayLanguageIndex(&index);

		if(index == 2 || index == 1)	//英文或繁体显示时，不显示语言切换窗口
			h_client = 0;

		MoveWindow(sg_hImeWnd, new_x, new_y, w_client, h_client, TRUE);
		new_skb_x = new_x;
		new_skb_y = (SKBATTR_ST_EN == sg_nSkbAttrSt) ? 
			(new_y  + h_client) : (new_y + h_client);

		
		MoveWindow (sg_hSkbQuerty, 
				    new_skb_x,
				    new_skb_y, 
				    SKB_QUERY_WND_W, 
				    SKB_QUERY_WND_H, 
				    TRUE);
						    
	}
	else
	{
		if (SKBATTR_ST_NUM == sg_nSkbAttrSt)
			h_client = 0;		
		MoveWindow(sg_hImeWnd, new_x, new_y, w_client, h_client, TRUE);
		new_skb_x = new_x;
		new_skb_y = (SKBATTR_ST_NUM == sg_nSkbAttrSt) ? 
			(new_y + h_client) : (new_y + h_client);	

		
		MoveWindow (sg_hSkbT9, 
				    new_x,
				    new_y + h_client, 
				    SKB_T9_WND_W, 
				    SKB_T9_WND_H, 
				    TRUE);
		  
	}
	ShowWindow(sg_hImeWnd, SW_SHOWNORMAL);	
	ShowWindow(sg_hSkbQuerty, (SKBTYPE_QUERTY == sg_nSkbType) ?
		SW_SHOWNORMAL : SW_HIDE);
#if 1
	//SetT9KeysText();
	ShowWindow(sg_hSkbT9, (SKBTYPE_QUERTY == sg_nSkbType) ?
		SW_HIDE : SW_SHOWNORMAL);			
#else
	ShowWindow(sg_hSkbT9, SW_HIDE);
#endif
	old_hook = RegisterMouseMsgHook ((void *)hwnd, ime_msg_hook); 

	IsImeOpened = 1;
}

static void ReinitSKB(HWND hwnd, int nShowType)
{
	sg_nSkbType = 0;
	sg_dwSkbAttr &= 0;
	sg_dwSkbAttr |= SKBATTR_MASK;
	sg_nSkbAttrSt = SKBATTR_ST_EN;
	
	//ADD CODE HERE : 根据风格设置软键盘类型、属性
    Uint32 unStyle = GetWindowExStyle(GetFocusChild(sg_hTargetWnd));\
    
	// 英文
	if ((unStyle & WS_EX_EDIT_CHARINPUT)
		|| (unStyle&WS_EX_EDIT_ALPINPUT)
		|| (unStyle&WS_EX_EDIT_MINALPINPUT))
	{
		sg_dwSkbAttr |= SKBATTR_EN;
	}
	else
	{
		sg_dwSkbAttr &= ~SKBATTR_EN;
	}

	// 中文
	if (unStyle & WS_EX_EDIT_HZINPUT)
		sg_dwSkbAttr |= SKBATTR_CN;
	else
		sg_dwSkbAttr &= ~SKBATTR_CN;


	// 纯数字
	if ((unStyle & WS_EX_EDIT_NUMINPUT))
	{
		sg_dwSkbAttr |= SKBATTR_NUM;
	}
	else
	{
		sg_dwSkbAttr &= ~SKBATTR_NUM;
	}		

	if (SKBATTR_NUM == (SKBATTR_MASK & sg_dwSkbAttr))  //只有数字风格的情况
	{
		sg_nSkbType = SKBTYPE_T9;
	}
	else if (0 == nShowType)
	{
		sg_nSkbType = SKBTYPE_QUERTY;
		sg_dwSkbAttr &= ~SKBATTR_NUM;		//全键盘按键英文输入状态下包含了数字输入
	}
	else
	{
		sg_nSkbType = SKBTYPE_T9;
	}
	
	// 设置属性状态
	if (SKBATTR_EN & sg_dwSkbAttr)
	{	
		sg_nSkbAttrSt = SKBATTR_ST_EN;
		if (SKBTYPE_QUERTY == sg_nSkbType)
			IsHanziInput = 0;
	}
	else if(SKBATTR_CN & sg_dwSkbAttr)
	{
		sg_nSkbAttrSt = SKBATTR_ST_CN;
		if (SKBTYPE_QUERTY == sg_nSkbType)
			IsHanziInput = 1;
	}
	else if(SKBATTR_NUM & sg_dwSkbAttr)
	{
		sg_nSkbAttrSt = SKBATTR_ST_NUM;
	}
	else 
	{
		//默认情况下为0
		sg_nSkbAttrSt = SKBATTR_ST_EN;		
	}
	
}
static int clean_left_ctrl = 1;


/*************************************************************************
 *                        extern function                                *
 *************************************************************************/
void writemsg_ex (BYTE *buffer, int len, LPARAM lParam, BOOL bDByte)
{
    WORD wDByte;
    int i;

    if (bDByte) {
    	for (i=0; i<len; i+=2) {
            wDByte = MAKEWORD (buffer[i], buffer[i+1]);
            if (sg_hTargetWnd)
	            PostMessage (sg_hTargetWnd, MSG_CHAR, wDByte, 0);
        }
    }
    else {
    	for (i=0; i<len; i++) {
            if (sg_hTargetWnd)
	            PostMessage (sg_hTargetWnd, MSG_CHAR, buffer[i], 0);
        }
    }
}

void outchar_ex (unsigned char c, LPARAM lParam)
{
    int scancode = 0;

    switch (c) {
    case '\010':
    case '\177':  /* BackSpace */
        scancode = SCANCODE_BACKSPACE;
        break;
    case '\033':
        scancode = SCANCODE_ESCAPE;
        break;
    case '\t':
        scancode = SCANCODE_TAB;
        break;
    }
        
    if (scancode) {
	    PostMessage (sg_hTargetWnd, MSG_KEYDOWN, scancode, (LPARAM)KS_IMEPOST);
	    PostMessage (sg_hTargetWnd, MSG_KEYUP, scancode, (LPARAM)KS_IMEPOST);
    }
    else
        writemsg_ex (&c, 1, lParam, FALSE);
}

/* this is the ime window proc */
static int GBIMEWinCustomProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static BOOL fCaptured = FALSE;
    static int oldx, oldy;
    int x, y;
    int maxx, maxy;
    RECT rcWindow;
    RECT *prcExpect, *prcResult;

    switch (message) 
	{
        case MSG_NCCREATE:
		{	
            if (hz_input_init())
                /* Register before show the window. */
                SendMessage (HWND_DESKTOP, MSG_IME_REGISTER, (WPARAM)hWnd, 0);
            else
                return -1;
        }
        break;
		case MSG_IME_LBD_SWITCH:
		{	
			if (IsImeOpened&& (GetFocusChild(sg_hTargetWnd) == (HWND) wParam))
			{	
				
				if (SKBTYPE_T9 == sg_nSkbType 
					&& (SKBATTR_NUM != (SKBATTR_MASK & sg_dwSkbAttr)))
				{
					ShowInputWnds(hWnd, 0);
				}
				else
				{					
					HideInputWnds();						
				}
			}
			else
			{					
				ShowInputWnds(hWnd, 0);
			}
		}
		return 0;
        case MSG_IME_OPEN:
            return 0;

        case MSG_IME_CLOSE:
            if (IsImeOpened) {
                HideInputWnds();
                IsImeOpened = 0;
                //PostMessage(hWnd, MSG_IME_SETTARGET, 0, 0);	//目标窗口归零
            }
        break;
        
        case MSG_IME_SETSTATUS:
            imeSetStatus ((int)wParam, (int)lParam);
        break;

        case MSG_IME_GETSTATUS:
            return imeGetStatus ((int)wParam);
        break;

        case MSG_IME_SETTARGET:
            if (sg_hTargetWnd != hWnd)
                sg_hTargetWnd = (HWND)wParam;
        break;

        case MSG_IME_GETTARGET:
            return (int)sg_hTargetWnd;
        break;

        case MSG_SETFOCUS:
        case MSG_KILLFOCUS:
			break;
            //return 0;

        case MSG_CHAR:
            if (IsImeOpened && !imeIsSpecialChar (wParam)) {
				if(SKBTYPE_QUERTY == sg_nSkbType)
				{
                	if (hz_filter ((BYTE)wParam, lParam))
                		InvalidateRect (hWnd, NULL, TRUE);
                }
                else /* if (SKBTYPE_T9 == sg_nSkbType) */
                {
					if (('0' <= (BYTE)wParam )
						&& ((BYTE)wParam <= '9'))
					{
						InputKeyProcess((BYTE)wParam, lParam);
						InvalidateRect(hWnd, NULL, TRUE);
					}
                }
                
            }
        	return 0;

		case MSG_KEYDOWN_SKB:
			OnImeKeydownSkb(hWnd, wParam, lParam);
        	return 0;
        	
        case MSG_KEYDOWN:
        	OnImeKeyDown(hWnd, wParam, lParam);
			return 0;

		case MSG_KEYUP_SKB:
			OnImeKeyupSkb(hWnd, wParam, lParam);
       		return 0;
       		
        case MSG_KEYUP:
        	OnImeKeyUp(hWnd, wParam, lParam);
			return 0;
        
        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            SetBkColor (hdc, GetWindowElementColor (BKC_CONTROL_DEF));
            refresh_input_method_area (hWnd, hdc);
            EndPaint (hWnd, hdc);
        return 0;

        case MSG_LBUTTONDOWN:
        {
        	//if (SKBTYPE_QUERTY == sg_nSkbType)
        	{
        	
	            RECT rc;
	            int zwidth = GetSysCCharWidth();
	            int zheight = GetSysCharHeight();

	            GetClientRect (hWnd, &rc);

	            oldx = LOWORD (lParam);
	            oldy = HIWORD (lParam);
	            
	            if (/*IsHanziInput && */oldx > 0 && oldx < 4*zwidth && oldy > 2 && oldy < zheight+2){ 
	                //imeSetStatus (IS_METHOD, -1);
	                toggle_input_method();
	                InvalidateRect (hWnd, NULL, TRUE);
	                return 0;
	            }
				int lx = IME_QUERTY_WND_W-60,rx = IME_QUERTY_WND_W-30;
				RECT lButton = {lx, IME_WND_H/2-10, lx+10, IME_WND_H-10};
				RECT rButton = {rx, IME_WND_H/2-10, rx+10, IME_WND_H-10};
			
	            if(IsHanziInput && strlen(Pinyin_Module_Ex.inbuf) && oldx > lButton.left && oldx < lButton.right && oldy > lButton.top && oldy < lButton.bottom)
	            {
	            	//printf("@@@@@@@@@@@@@@@@@@@@@@@@@@%s[%d]\r\n",__FILE__,__LINE__);
					PostMessage(sg_hImeWnd, MSG_CHAR, symbolKeyValue[9], 0);
					return 0;
	            }

	            if(IsHanziInput && strlen(Pinyin_Module_Ex.inbuf) && oldx > rButton.left && oldx < rButton.right && oldy > rButton.top && oldy < rButton.bottom)
	            {
	            	//printf("@@@@@@@@@@@@@@@@@@@@@@@@@@%s[%d]\r\n",__FILE__,__LINE__);
					PostMessage(sg_hImeWnd, MSG_CHAR, symbolKeyValue[6], 0);
					return 0;
	            }


				/*
	            if (oldx > rc.right - 4 - 32 && oldx < rc.right - 4 - 16 && oldy > 2 && oldy < 16+2){
	                toggle_half_full ();
	                InvalidateRect (hWnd, NULL, TRUE);
	                return 0;
	            }
	            if (oldx > rc.right - 4 - 16 && oldx < rc.right - 4 && oldy > 2 && oldy < 16+2){
	                toggle_punc ();
	                InvalidateRect (hWnd, NULL, TRUE);
	                return 0;
	            }
	            */
            }
            //SetCapture (hWnd);
            //ClientToScreen (hWnd, &oldx, &oldy);
            //fCaptured = TRUE;
        }
        break;

        case MSG_LBUTTONUP:
            //ReleaseCapture ();
            //fCaptured = FALSE;
        break;
        
        case MSG_MOUSEMOVE:
            if (fCaptured) {
                GetWindowRect (hWnd, &rcWindow);
                x = LOSWORD (lParam);
                y = HISWORD (lParam);
                OffsetRect (&rcWindow, x - oldx, y - oldy);
                MoveWindow (hWnd, rcWindow.left, rcWindow.top,
                              RECTW (rcWindow), RECTH (rcWindow), FALSE);
                oldx = x;
                oldy = y;
            }
        break;
        
		case MSG_RBUTTONUP:
			{
		        int x_pos = LOSWORD (lParam);
		        int y_pos = HISWORD (lParam);

		        if (is_mouse_in_client(hWnd,x_pos, y_pos))
		        {
					HideInputWnds();
		        }		
			}
			break; 
			
        case MSG_SIZECHANGING:
            prcExpect = (PRECT)wParam;
            prcResult = (PRECT)lParam;

            *prcResult = *prcExpect;
            if (prcExpect->left < 0) {
                prcResult->left = 0;
                prcResult->right = RECTWP (prcExpect);
            }
            if (prcExpect->top < 0) {
                prcResult->top  = 0;
                prcResult->bottom = RECTHP (prcExpect);
            }

            maxx = GetGDCapability (HDC_SCREEN, GDCAP_HPIXEL);
            maxy = GetGDCapability (HDC_SCREEN, GDCAP_VPIXEL);

            if (prcExpect->right > maxx) {
                prcResult->right = maxx;
                prcResult->left = maxx - RECTWP (prcExpect);
            }
            if (prcExpect->bottom > maxy) {
                prcResult->bottom = maxy;
                prcResult->top = maxy - RECTHP (prcExpect);
            }

        return 0;
        
        case MSG_CLOSE:
        	SendMessage (sg_hSkbQuerty, MSG_CLOSE, 0, 0);
        	SendMessage (sg_hSkbT9, MSG_CLOSE, 0, 0);
            SendMessage (HWND_DESKTOP, MSG_IME_UNREGISTER, (WPARAM)hWnd, 0);
            hz_input_done ();
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);    
       		return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

typedef struct ime_info
{
    sem_t wait;
    HWND hwnd;
} IME_INFO;

static void* start_gb_ime (void* data)
{
    MSG Msg;
    MAINWINCREATE createInfo;
    IME_INFO* ime_info = (IME_INFO*) data;

	prctl(PR_SET_NAME, "gui_ime");
	createInfo.dwStyle = WS_ABSSCRPOS | WS_BORDER /*| WS_VISIBLE*/;
	createInfo.dwExStyle = WS_EX_TOPMOST;
	createInfo.spCaption = "The IME Window" ;
	createInfo.hMenu = 0;
	createInfo.hCursor = GetSystemCursor(0);
	createInfo.hIcon = 0;
	createInfo.MainWindowProc = GBIMEWinCustomProc;

	createInfo.lx = 0; 
	createInfo.ty = 0; 
	createInfo.rx = IME_QUERTY_WND_W;
	createInfo.by = IME_WND_H;

	createInfo.iBkColor = WND_BK_COLOR;
	createInfo.dwAddData = 0;
	createInfo.hHosting = 0;

    bTwoLines = TRUE;

    sg_hImeWnd = ime_info->hwnd = CreateMainWindow (&createInfo);

    sem_post (&ime_info->wait);
    if (sg_hImeWnd == HWND_INVALID)
        return NULL;

    sg_hSkbT9 = CreateSkbT9Wnd(sg_hImeWnd);
    sg_hSkbQuerty = CreateSkbQuertyWnd(sg_hImeWnd);
#if 1    
    if (HWND_INVALID == sg_hSkbT9 || HWND_INVALID == sg_hSkbQuerty)
    {
		if (HWND_INVALID != sg_hSkbT9)
			DestroyMainWindow(sg_hSkbT9);
		if (HWND_INVALID != sg_hSkbQuerty)
			DestroyMainWindow(sg_hSkbQuerty);

		DestroyMainWindow(sg_hImeWnd);
		MainWindowThreadCleanup (sg_hImeWnd);
		
		sg_hSkbT9 = HWND_INVALID;
		sg_hSkbQuerty = HWND_INVALID;
		sg_hImeWnd = HWND_INVALID;

		return NULL;
    }
#endif

    while (GetMessage (&Msg, sg_hImeWnd) ) {

        if (Msg.message == MSG_KEYDOWN || Msg.message == MSG_KEYUP)
        {
            if (!IsImeOpened)
            {
                PostMessage (sg_hTargetWnd, 
                    Msg.message, Msg.wParam, Msg.lParam | KS_IMEPOST);
            }
			else if(IsImeOpened && (SKBTYPE_QUERTY == sg_nSkbType))
			{
#if 0			
	            if (IsHanziInput)
	                TranslateMessage (&Msg);
	            else if (sg_hTargetWnd)
	            {
	                PostMessage (sg_hTargetWnd, 
	                    Msg.message, Msg.wParam, Msg.lParam | KS_IMEPOST);
	        	}
#endif	        	
            }            
            
        }

        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (sg_hImeWnd);

    return NULL;
}

static pthread_t imethread;

/* the argument of 'hosting' is ignored. */
HWND GBIMEWindowCustom (HWND hosting)
{
	if(!RegisterSKBKeyControl())
		return HWND_INVALID;
	
    IME_INFO ime_info;
    pthread_attr_t new_attr;

    sem_init (&ime_info.wait, 0, 0);

    pthread_attr_init (&new_attr);
    pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
#ifdef PTHREAD_INFO
	pthread_create (&imethread, "ui_imewindow", &new_attr, start_gb_ime, &ime_info);
#else
    pthread_create (&imethread, &new_attr, start_gb_ime, &ime_info);
#endif

    pthread_attr_destroy (&new_attr);

    sem_wait (&ime_info.wait);
    sem_destroy (&ime_info.wait);

    return ime_info.hwnd;
}

BOOL IsCustomImeOpen()
{
	return IsImeOpened;
}

/* ****************************************************************** *
 *                          softkeybord                               *
 * ****************************************************************** */
#if 1
static void CreateSoftKey (HWND parent)
{

	   /******************Static Caption**************************/
	 sg_hSKeys.hGantanhao
		 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						 "!",
						 STYLE_SK,
						 WS_EX_TRANSPARENT,
						 IDC_GANTANHAO, STATIC_LEFT + 0 * STATIC_W,
						 STATIC_TOP + 0 * STATIC_H,
						 STATIC_W /*+ STATIC_ESC_OFFSET*/, STATIC_H, parent, 0);
	hSoftKeyWnd[0] = sg_hSKeys.hGantanhao;

	sg_hSKeys.hWenhao= CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "?",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_WENHAO,
						  STATIC_LEFT + 1 * STATIC_W + 1/* + STATIC_ESC_OFFSET*/,
						  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
						  0);
	hSoftKeyWnd[1] = sg_hSKeys.hWenhao;

	sg_hSKeys.hAt= CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "@",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_AT,
						  STATIC_LEFT + 2 * STATIC_W + 2/* + STATIC_ESC_OFFSET*/,
						  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
						  0);
	hSoftKeyWnd[2] = sg_hSKeys.hAt;

	sg_hSKeys.hJinhao= CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "#",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_JINHAO,
						  STATIC_LEFT + 3 * STATIC_W + 3/* + STATIC_ESC_OFFSET*/,
						  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
						  0);
	hSoftKeyWnd[3] = sg_hSKeys.hJinhao;

	sg_hSKeys.hDollar = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "$",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_DOLLAR,
						  STATIC_LEFT + 4 * STATIC_W + 4 /*+ STATIC_ESC_OFFSET*/,
						  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
						  0);
	hSoftKeyWnd[4] = sg_hSKeys.hDollar;

	sg_hSKeys.hPercent = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "%",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_PERCENT,
						  STATIC_LEFT + 5 * STATIC_W + 5 /*+ STATIC_ESC_OFFSET*/,
						  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
						  0);
	hSoftKeyWnd[5] = sg_hSKeys.hPercent;

	sg_hSKeys.hEqual = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "=",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_EQUAL,
						  STATIC_LEFT + 6 * STATIC_W + 6 /*+ STATIC_ESC_OFFSET*/,
						  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
						  0);
	hSoftKeyWnd[6] = sg_hSKeys.hEqual;

	sg_hSKeys.hAdd = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "+",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_ADD,
						  STATIC_LEFT + 7 * STATIC_W + 7/* + STATIC_ESC_OFFSET*/,
						  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
						  0);
	hSoftKeyWnd[7] = sg_hSKeys.hAdd;

	sg_hSKeys.hChen = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "*",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_CHEN,
						  STATIC_LEFT + 8 * STATIC_W + 8/* + STATIC_ESC_OFFSET*/,
						  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
						  0);
	hSoftKeyWnd[8] = sg_hSKeys.hChen;

	sg_hSKeys.hSub = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "-",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_SUB,
						  STATIC_LEFT + 9 * STATIC_W + 9/* + STATIC_ESC_OFFSET*/,
						  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
						  0);
	hSoftKeyWnd[9] = sg_hSKeys.hSub;


	sg_hSKeys.hPlinedown = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "_",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_PLINEDOWN,
						  STATIC_LEFT + 10 * STATIC_W + 10/* + STATIC_ESC_OFFSET*/,
						  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
						  0);
	hSoftKeyWnd[10] = sg_hSKeys.hPlinedown;

	sg_hSKeys.hBackspace = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "<--Back",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_BACKSPACE,
						  STATIC_LEFT + 11 * STATIC_W + 11/* + STATIC_ESC_OFFSET*/,
						  STATIC_TOP + 0 * STATIC_H, STATIC_W + STATIC_ESC_OFFSET, STATIC_H, parent,
						  0);
	hSoftKeyWnd[11] = sg_hSKeys.hBackspace;

	sg_hSKeys.hNum1 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "1",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_NUM1,
						  STATIC_LEFT + 13 * STATIC_W + 13 + STATIC_ESC_OFFSET,
						  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
						  0);
	hSoftKeyWnd[12] = sg_hSKeys.hNum1;

	sg_hSKeys.hNum2 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "2",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_NUM2,
						  STATIC_LEFT + 14 * STATIC_W + 14 + STATIC_ESC_OFFSET,
						  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
						  0);
	hSoftKeyWnd[13] = sg_hSKeys.hNum2;

	sg_hSKeys.hNum3 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "3",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_NUM3,
						  STATIC_LEFT + 15 * STATIC_W + 15 + STATIC_ESC_OFFSET,
						  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
						  0);
	hSoftKeyWnd[14] = sg_hSKeys.hNum3;

   /**************************end of the first line****************************************/

   /**************************begin the second line****************************************/

	sg_hSKeys.hKeyq = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						 "q",
						 STYLE_SK,
						 WS_EX_TRANSPARENT,
						 IDC_KEYQ, STATIC_LEFT + 0 * STATIC_W + (STATIC_W>>1),
						 STATIC_TOP + 1 * STATIC_H + 1,
						 STATIC_W/* + STATIC_TAB_OFFSET*/, STATIC_H, parent, 0);
	hSoftKeyWnd[15] = sg_hSKeys.hKeyq;

	sg_hSKeys.hKeyw = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "w",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYW,
						  STATIC_LEFT + 1 * STATIC_W + 1 + (STATIC_W>>1),
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[16] = sg_hSKeys.hKeyw;

	sg_hSKeys.hKeye = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "e",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYE,
						  STATIC_LEFT + 2 * STATIC_W + 2 + (STATIC_W>>1),
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[17] = sg_hSKeys.hKeye;

	sg_hSKeys.hKeyr = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "r",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYR,
						  STATIC_LEFT + 3 * STATIC_W + 3 + (STATIC_W>>1),
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[18] = sg_hSKeys.hKeyr;

	sg_hSKeys.hKeyt = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "t",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYT,
						  STATIC_LEFT + 4 * STATIC_W + 4 + (STATIC_W>>1),
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[19] = sg_hSKeys.hKeyt;

	sg_hSKeys.hKeyy = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "y",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYY,
						  STATIC_LEFT + 5 * STATIC_W + 5 + (STATIC_W>>1),
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[20] = sg_hSKeys.hKeyy;

	sg_hSKeys.hKeyu = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "u",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYU,
						  STATIC_LEFT + 6 * STATIC_W + 6 + (STATIC_W>>1),
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[21] = sg_hSKeys.hKeyu;

	sg_hSKeys.hKeyi = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "i",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYI,
						  STATIC_LEFT + 7 * STATIC_W + 7 + (STATIC_W>>1),
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[22] = sg_hSKeys.hKeyi;

	sg_hSKeys.hKeyo = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "o",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYO,
						  STATIC_LEFT + 8 * STATIC_W + 8 + (STATIC_W>>1),
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[23] = sg_hSKeys.hKeyo;

	sg_hSKeys.hKeyp = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "p",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYP,
						  STATIC_LEFT + 9 * STATIC_W + 9 + (STATIC_W>>1),
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[24] = sg_hSKeys.hKeyp;

	sg_hSKeys.hChu = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "/",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_CHU,
						  STATIC_LEFT + 10 * STATIC_W + 10 + (STATIC_W>>1),
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[25] = sg_hSKeys.hChu;

	sg_hSKeys.hNum4 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "4",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_NUM4,
						  STATIC_LEFT + 13 * STATIC_W + 13 + STATIC_ESC_OFFSET,
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[26] = sg_hSKeys.hNum4;

	sg_hSKeys.hNum5 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "5",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_NUM5,
						  STATIC_LEFT + 14 * STATIC_W + 14 + STATIC_ESC_OFFSET,
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[27] = sg_hSKeys.hNum5;

	sg_hSKeys.hNum6 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "6",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_NUM6,
						  STATIC_LEFT + 15 * STATIC_W + 15 + STATIC_ESC_OFFSET,
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[28] = sg_hSKeys.hNum6;

/**********************************end of the second line*************************************************/

/**********************************begin the third line*************************************************/

	sg_hSKeys.hKeya = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "a",
						  STYLE_SK,// | SKBKS_CHECKED),
						  WS_EX_TRANSPARENT,
						  IDC_KEYA,
						  STATIC_LEFT + 0 * STATIC_W + 0 + STATIC_W,
						  STATIC_TOP + 2 * STATIC_H + 2,
						  STATIC_W , STATIC_H, parent, 0);
	hSoftKeyWnd[29] = sg_hSKeys.hKeya;


	sg_hSKeys.hKeys = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "s",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYS,
						  STATIC_LEFT + 1 * STATIC_W + 1 + STATIC_W,
						  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[30] = sg_hSKeys.hKeys;

	sg_hSKeys.hKeyd = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  "d",
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_KEYD,
					  STATIC_LEFT + 2 * STATIC_W + 2 + STATIC_W,
					  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
					  parent, 0);
	
hSoftKeyWnd[31] = sg_hSKeys.hKeyd;

	
sg_hSKeys.hKeyf = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  "f",
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_KEYF,
					  STATIC_LEFT + 3 * STATIC_W + 3 + STATIC_W,
					  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
					  parent, 0);
hSoftKeyWnd[32] = sg_hSKeys.hKeyf;

	
sg_hSKeys.hKeyg = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  "g",
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_KEYG,
					  STATIC_LEFT + 4 * STATIC_W + 4 + STATIC_W,
					  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
					  parent, 0);
hSoftKeyWnd[33] = sg_hSKeys.hKeyg;

	
sg_hSKeys.hKeyh = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  "h",
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_KEYH,
					  STATIC_LEFT + 5 * STATIC_W + 5 + STATIC_W,
					  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
					  parent, 0);
hSoftKeyWnd[34] = sg_hSKeys.hKeyh;

	
sg_hSKeys.hKeyj = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  "j",
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_KEYJ,
					  STATIC_LEFT + 6 * STATIC_W + 6 + STATIC_W,
					  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
					  parent, 0);
hSoftKeyWnd[35] = sg_hSKeys.hKeyj;

	
sg_hSKeys.hKeyk = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  "k",
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_KEYK,
					  STATIC_LEFT + 7 * STATIC_W + 7 + STATIC_W,
					  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
					  parent, 0);
hSoftKeyWnd[36] = sg_hSKeys.hKeyk;

	
sg_hSKeys.hKeyl = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  "l",
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_KEYL,
					  STATIC_LEFT + 8 * STATIC_W + 8 + STATIC_W,
					  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
					  parent, 0);
hSoftKeyWnd[37] = sg_hSKeys.hKeyl;

	
sg_hSKeys.hMaohao = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  ":",
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_MAOHAO,
					  STATIC_LEFT + 9 * STATIC_W + 9 + STATIC_W,
					  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
					  parent, 0);
	
hSoftKeyWnd[38] = sg_hSKeys.hMaohao;

	
sg_hSKeys.hEnter = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						"Enter",
						STYLE_SK,
						WS_EX_TRANSPARENT,
						IDC_ENTER,
						STATIC_LEFT + 10 * STATIC_W + 10 + STATIC_W,
						STATIC_TOP + 2 * STATIC_H + 2,
						STATIC_W + STATIC_ESC_OFFSET, STATIC_H, parent, 0);
hSoftKeyWnd[39] = sg_hSKeys.hEnter;

sg_hSKeys.hNum7 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						"7",
						STYLE_SK,
						WS_EX_TRANSPARENT,
						IDC_NUM7,
						STATIC_LEFT + 13 * STATIC_W + 13 + STATIC_ESC_OFFSET,
						STATIC_TOP + 2 * STATIC_H + 2,
						STATIC_W, STATIC_H, parent, 0);
hSoftKeyWnd[40] = sg_hSKeys.hNum7;

sg_hSKeys.hNum8 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						"8",
						STYLE_SK,
						WS_EX_TRANSPARENT,
						IDC_NUM8,
						STATIC_LEFT + 14 * STATIC_W + 14 + STATIC_ESC_OFFSET,
						STATIC_TOP + 2 * STATIC_H + 2,
						STATIC_W, STATIC_H, parent, 0);
hSoftKeyWnd[41] = sg_hSKeys.hNum8;

sg_hSKeys.hNum9 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						"9",
						STYLE_SK,
						WS_EX_TRANSPARENT,
						IDC_NUM9,
						STATIC_LEFT + 15 * STATIC_W + 15 + STATIC_ESC_OFFSET,
						STATIC_TOP + 2 * STATIC_H + 2,
	
						STATIC_W, STATIC_H, parent, 0);
	
hSoftKeyWnd[42] = sg_hSKeys.hNum9;

/*************************************end of the third line*****************************************/

/*************************************begin the fourth line*****************************************/

	
sg_hSKeys.hKeyz = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					   "z",
					   STYLE_SK,
					   WS_EX_TRANSPARENT,
					   IDC_KEYZ,
					   STATIC_LEFT + 0 * STATIC_W + 0+ STATIC_W+(STATIC_W>>1),
					   STATIC_TOP + 3 * STATIC_H + 3,
					   STATIC_W , STATIC_H, parent, 0);
hSoftKeyWnd[43] = sg_hSKeys.hKeyz;


	
sg_hSKeys.hKeyx = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  "x",	
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_KEYX,
					  STATIC_LEFT + 1 * STATIC_W + 1 + STATIC_W+(STATIC_W>>1),
					  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
					  parent, 0);
hSoftKeyWnd[44] = sg_hSKeys.hKeyx;

	
sg_hSKeys.hKeyc = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  "c",
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_KEYC,
					  STATIC_LEFT + 2 * STATIC_W + 2 + STATIC_W+(STATIC_W>>1),
					  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
					  parent, 0);
hSoftKeyWnd[45] = sg_hSKeys.hKeyc;

	
sg_hSKeys.hKeyv = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  "v",
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_KEYV,
					  STATIC_LEFT + 3 * STATIC_W + 3 + STATIC_W+(STATIC_W>>1),
					  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
					  parent, 0);
hSoftKeyWnd[46] = sg_hSKeys.hKeyv;

	
sg_hSKeys.hKeyb = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  "b",
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_KEYB,
					  STATIC_LEFT + 4 * STATIC_W + 4 + STATIC_W+(STATIC_W>>1),
					  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
					  parent, 0);
hSoftKeyWnd[47] = sg_hSKeys.hKeyb;

	
sg_hSKeys.hKeyn = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  "n",
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_KEYN,
					  STATIC_LEFT + 5 * STATIC_W + 5 + STATIC_W+(STATIC_W>>1),
					  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
					  parent, 0);
hSoftKeyWnd[48] = sg_hSKeys.hKeyn;


sg_hSKeys.hKeym = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  "m",
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_KEYM,
					  STATIC_LEFT + 6 * STATIC_W + 6 + STATIC_W+(STATIC_W>>1),
					  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
					  parent, 0);
hSoftKeyWnd[49] = sg_hSKeys.hKeym;

	
sg_hSKeys.hDouhao = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					  ",",
					  STYLE_SK,
					  WS_EX_TRANSPARENT,
					  IDC_DOUHAO,
					  STATIC_LEFT + 7 * STATIC_W + 7 + STATIC_W+(STATIC_W>>1),
					  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
					  parent, 0);
hSoftKeyWnd[50] = sg_hSKeys.hDouhao;

	
sg_hSKeys.hDot = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					   ".",
					   STYLE_SK,
					   WS_EX_TRANSPARENT,
					   IDC_DOT,
					   STATIC_LEFT + 8 * STATIC_W + 8 + STATIC_W+(STATIC_W>>1),
					   STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
					   parent, 0);
hSoftKeyWnd[51] = sg_hSKeys.hDot;

	
sg_hSKeys.hShift = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					 "Shift",
					 STYLE_SK,
					 WS_EX_TRANSPARENT,
					 IDC_SHIFT,
					 STATIC_LEFT + 9 * STATIC_W + 9 + STATIC_W+(STATIC_W>>1),
					 STATIC_TOP + 3 * STATIC_H + 3, STATIC_W + STATIC_ESC_OFFSET+(STATIC_W>>1), STATIC_H,
					 parent, 0);
hSoftKeyWnd[52] = sg_hSKeys.hShift;

sg_hSKeys.hSpace = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					 "Space",
					 STYLE_SK,
					 WS_EX_TRANSPARENT,
					 IDC_SPACE,
					 STATIC_LEFT + 13 * STATIC_W + 13 + STATIC_ESC_OFFSET,
					 STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
					 parent, 0);
hSoftKeyWnd[53] = sg_hSKeys.hSpace;

sg_hSKeys.hNum0 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					 "0",
					 STYLE_SK,
					 WS_EX_TRANSPARENT,
					 IDC_NUM0,
					 STATIC_LEFT + 14 * STATIC_W + 14 + STATIC_ESC_OFFSET,
					 STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
					 parent, 0);
hSoftKeyWnd[54] = sg_hSKeys.hNum0;

sg_hSKeys.hAddr = CreateWindowEx  (CTRL_SKBK_CUSTOM,
					 "&",
					 STYLE_SK,
					 WS_EX_TRANSPARENT,
					 IDC_ADDR,
					 STATIC_LEFT + 15 * STATIC_W + 15 + STATIC_ESC_OFFSET,
					 STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
					 parent, 0);
hSoftKeyWnd[55] = sg_hSKeys.hAddr;

/*************************************end of the fourth line***************************************************/

}

#else


static void CreateSoftKey (HWND parent)
{


	
   /******************Static Caption**************************/
	
 sg_hSKeys.hEsc
	
	 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					 "Esc",
	
					 STYLE_SK,
	
					 WS_EX_TRANSPARENT,
	
					 IDC_ESC, STATIC_LEFT + 0 * STATIC_W,
	
					 STATIC_TOP + 0 * STATIC_H,
	
					 STATIC_W + STATIC_ESC_OFFSET, STATIC_H, parent, 0);
	
hSoftKeyWnd[0] = sg_hSKeys.hEsc;

	
sg_hSKeys.hNum1 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "1",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_NUM1,
	
					  STATIC_LEFT + 1 * STATIC_W + 1 + STATIC_ESC_OFFSET,
	
					  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
	
					  0);
	
hSoftKeyWnd[1] = sg_hSKeys.hNum1;

	
sg_hSKeys.hNum2 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "2",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_NUM2,
	
					  STATIC_LEFT + 2 * STATIC_W + 2 + STATIC_ESC_OFFSET,
	
					  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
	
					  0);
	
hSoftKeyWnd[2] = sg_hSKeys.hNum2;

	
sg_hSKeys.hNum3 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "3",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_NUM3,
	
					  STATIC_LEFT + 3 * STATIC_W + 3 + STATIC_ESC_OFFSET,
	
					  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
	
					  0);
	
hSoftKeyWnd[3] = sg_hSKeys.hNum3;

	
sg_hSKeys.hNum4 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "4",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_NUM4,
	
					  STATIC_LEFT + 4 * STATIC_W + 4 + STATIC_ESC_OFFSET,
	
					  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
	
					  0);
	
hSoftKeyWnd[4] = sg_hSKeys.hNum4;

	
sg_hSKeys.hNum5 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "5",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_NUM5,
	
					  STATIC_LEFT + 5 * STATIC_W + 5 + STATIC_ESC_OFFSET,
	
					  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
	
					  0);
	
hSoftKeyWnd[5] = sg_hSKeys.hNum5;

	
sg_hSKeys.hNum6 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "6",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_NUM6,
	
					  STATIC_LEFT + 6 * STATIC_W + 6 + STATIC_ESC_OFFSET,
	
					  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
	
					  0);
	
hSoftKeyWnd[6] = sg_hSKeys.hNum6;

	
sg_hSKeys.hNum7 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "7",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_NUM7,
	
					  STATIC_LEFT + 7 * STATIC_W + 7 + STATIC_ESC_OFFSET,
	
					  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
	
					  0);
	
hSoftKeyWnd[7] = sg_hSKeys.hNum7;

	
sg_hSKeys.hNum8 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "8",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_NUM8,
	
					  STATIC_LEFT + 8 * STATIC_W + 8 + STATIC_ESC_OFFSET,
	
					  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
	
					  0);
	
hSoftKeyWnd[8] = sg_hSKeys.hNum8;

	
sg_hSKeys.hNum9 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "9",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_NUM9,
	
					  STATIC_LEFT + 9 * STATIC_W + 9 + STATIC_ESC_OFFSET,
	
					  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
	
					  0);
	
hSoftKeyWnd[9] = sg_hSKeys.hNum9;


	
sg_hSKeys.hNum0 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "0",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_NUM0,
	
					  STATIC_LEFT + 10 * STATIC_W + 10 + STATIC_ESC_OFFSET,
	
					  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
	
					  0);
	
hSoftKeyWnd[10] = sg_hSKeys.hNum0;

	
sg_hSKeys.hJian = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "-",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_JIAN,
	
					  STATIC_LEFT + 11 * STATIC_W + 11 + STATIC_ESC_OFFSET,
	
					  STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H, parent,
	
					  0);
	
hSoftKeyWnd[11] = sg_hSKeys.hJian;


	
sg_hSKeys.hEqual = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					   "=",
	
					   STYLE_SK,
	
					   WS_EX_TRANSPARENT,
	
					   IDC_EQUAL,
	
					   STATIC_LEFT + 12 * STATIC_W + 12 + STATIC_ESC_OFFSET,
	
					   STATIC_TOP + 0 * STATIC_H, STATIC_W, STATIC_H,
	
					   parent, 0);
	
hSoftKeyWnd[12] = sg_hSKeys.hEqual;


	
sg_hSKeys.hBackSpace = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
						   "<--Back",
	
						   STYLE_SK,
	
						   WS_EX_TRANSPARENT,
	
						   IDC_BACKSPACE,
	
						   STATIC_LEFT + 13 * STATIC_W + 13 +
	
						   STATIC_ESC_OFFSET, STATIC_TOP + 0 * STATIC_H,
	
						   STATIC_W + STATIC_BACK_OFFSET, STATIC_H, parent, 0);
	
						   //STATIC_W + 36, STATIC_H, parent, 0);
	
hSoftKeyWnd[13] = sg_hSKeys.hBackSpace;

 
  /**************************end of the first line****************************************/

 
  /**************************begin the second line****************************************/

	
sg_hSKeys.hTab = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					 "Clear",
	
					 STYLE_SK,
	
					 WS_EX_TRANSPARENT,
	
					 IDC_TAB, STATIC_LEFT + 0 * STATIC_W,
	
					 STATIC_TOP + 1 * STATIC_H + 1,
	
					 STATIC_W + STATIC_TAB_OFFSET, STATIC_H, parent, 0);
	
hSoftKeyWnd[14] = sg_hSKeys.hTab;

	
sg_hSKeys.hKeyq = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "q",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_KEYQ,
	
					  STATIC_LEFT + 1 * STATIC_W + 1 + STATIC_TAB_OFFSET,
	
					  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
	
					  parent, 0);
	
hSoftKeyWnd[15] = sg_hSKeys.hKeyq;

	
sg_hSKeys.hKeyw = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "w",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_KEYW,
	
					  STATIC_LEFT + 2 * STATIC_W + 2 + STATIC_TAB_OFFSET,
	
					  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
	
					  parent, 0);
	
hSoftKeyWnd[16] = sg_hSKeys.hKeyw;

	
sg_hSKeys.hKeye = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "e",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_KEYE,
	
					  STATIC_LEFT + 3 * STATIC_W + 3 + STATIC_TAB_OFFSET,
	
					  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
	
					  parent, 0);
	
hSoftKeyWnd[17] = sg_hSKeys.hKeye;

	
sg_hSKeys.hKeyr = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "r",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_KEYR,
	
					  STATIC_LEFT + 4 * STATIC_W + 4 + STATIC_TAB_OFFSET,
	
					  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
	
					  parent, 0);
	
hSoftKeyWnd[18] = sg_hSKeys.hKeyr;

	
sg_hSKeys.hKeyt = CreateWindowEx  (CTRL_SKBK_CUSTOM,
	
					  "t",
	
					  STYLE_SK,
	
					  WS_EX_TRANSPARENT,
	
					  IDC_KEYT,
	
					  STATIC_LEFT + 5 * STATIC_W + 5 + STATIC_TAB_OFFSET,
	
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[19] = sg_hSKeys.hKeyt;

	sg_hSKeys.hKeyy = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "y",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYY,
						  STATIC_LEFT + 6 * STATIC_W + 6 + STATIC_TAB_OFFSET,
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[20] = sg_hSKeys.hKeyy;

	sg_hSKeys.hKeyu = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "u",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYU,
						  STATIC_LEFT + 7 * STATIC_W + 7 + STATIC_TAB_OFFSET,
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[21] = sg_hSKeys.hKeyu;

	sg_hSKeys.hKeyi = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "i",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYI,
						  STATIC_LEFT + 8 * STATIC_W + 8 + STATIC_TAB_OFFSET,
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[22] = sg_hSKeys.hKeyi;

	sg_hSKeys.hKeyo = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "o",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYO,
						  STATIC_LEFT + 9 * STATIC_W + 9 + STATIC_TAB_OFFSET,
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[23] = sg_hSKeys.hKeyo;

	sg_hSKeys.hKeyp = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "p",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYP,
						  STATIC_LEFT + 10 * STATIC_W + 10 + STATIC_TAB_OFFSET,
						  STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[24] = sg_hSKeys.hKeyp;

	sg_hSKeys.hKuohao1 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
							 "[",
							 STYLE_SK,
							 WS_EX_TRANSPARENT,
							 IDC_KUOHAO1,
							 STATIC_LEFT + 11 * STATIC_W + 11 +
							 STATIC_TAB_OFFSET, STATIC_TOP + 1 * STATIC_H + 1,
							 STATIC_W, STATIC_H, parent, 0);
	hSoftKeyWnd[25] = sg_hSKeys.hKuohao1;

	sg_hSKeys.hKuohao2 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
							 "]",
							 STYLE_SK,
							 WS_EX_TRANSPARENT,
							 IDC_KUOHAO2,
							 STATIC_LEFT + 12 * STATIC_W + 12 +
							 STATIC_TAB_OFFSET, STATIC_TOP + 1 * STATIC_H + 1,
							 STATIC_W, STATIC_H, parent, 0);
	hSoftKeyWnd[26] = sg_hSKeys.hKuohao2;

	sg_hSKeys.hPline = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						   "\\",
						   STYLE_SK,
						   WS_EX_TRANSPARENT,
						   IDC_PLINE,
						   STATIC_LEFT + 13 * STATIC_W + 13 + STATIC_TAB_OFFSET,
						   STATIC_TOP + 1 * STATIC_H + 1, STATIC_W, STATIC_H,
						   parent, 0);
	hSoftKeyWnd[27] = sg_hSKeys.hPline;

	sg_hSKeys.hDel = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						 "Del",
						 STYLE_SK,
						 WS_EX_TRANSPARENT,
						 IDC_DEL,
						 STATIC_LEFT + 14 * STATIC_W + 14 + STATIC_TAB_OFFSET,
						 STATIC_TOP + 1 * STATIC_H + 1, STATIC_W + STATIC_DEL_OFFSET, STATIC_H,
						 //STATIC_TOP + 1 * STATIC_H + 1, STATIC_W + 10, STATIC_H,
						 parent, 0);
	hSoftKeyWnd[28] = sg_hSKeys.hDel;

/**********************************end of the second line*************************************************/

/**********************************begin the third line*************************************************/

	sg_hSKeys.hCaps = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "Caps",
						  (STYLE_SK | SKBKS_CHECKED),
						  WS_EX_TRANSPARENT,
						  IDC_CAPS,
						  STATIC_LEFT + 0 * STATIC_W + 0,
						  STATIC_TOP + 2 * STATIC_H + 2,
						  STATIC_W + STATIC_CAPS_OFFSET, STATIC_H, parent, 0);
	hSoftKeyWnd[29] = sg_hSKeys.hCaps;


	sg_hSKeys.hKeya = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "a",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYA,
						  STATIC_LEFT + 1 * STATIC_W + 1 + STATIC_CAPS_OFFSET,
						  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[30] = sg_hSKeys.hKeya;

	sg_hSKeys.hKeys = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "s",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYS,
						  STATIC_LEFT + 2 * STATIC_W + 2 + STATIC_CAPS_OFFSET,
						  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[31] = sg_hSKeys.hKeys;

	sg_hSKeys.hKeyd = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "d",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYD,
						  STATIC_LEFT + 3 * STATIC_W + 3 + STATIC_CAPS_OFFSET,
						  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[32] = sg_hSKeys.hKeyd;

	sg_hSKeys.hKeyf = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "f",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYF,
						  STATIC_LEFT + 4 * STATIC_W + 4 + STATIC_CAPS_OFFSET,
						  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[33] = sg_hSKeys.hKeyf;

	sg_hSKeys.hKeyg = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "g",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYG,
						  STATIC_LEFT + 5 * STATIC_W + 5 + STATIC_CAPS_OFFSET,
						  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[34] = sg_hSKeys.hKeyg;

	sg_hSKeys.hKeyh = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "h",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYH,
						  STATIC_LEFT + 6 * STATIC_W + 6 + STATIC_CAPS_OFFSET,
						  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[35] = sg_hSKeys.hKeyh;

	sg_hSKeys.hKeyj = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "j",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYJ,
						  STATIC_LEFT + 7 * STATIC_W + 7 + STATIC_CAPS_OFFSET,
						  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[36] = sg_hSKeys.hKeyj;

	sg_hSKeys.hKeyk = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "k",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYK,
						  STATIC_LEFT + 8 * STATIC_W + 8 + STATIC_CAPS_OFFSET,
						  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[37] = sg_hSKeys.hKeyk;

	sg_hSKeys.hKeyl = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "l",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYL,
						  STATIC_LEFT + 9 * STATIC_W + 9 + STATIC_CAPS_OFFSET,
						  STATIC_TOP + 2 * STATIC_H + 2, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[38] = sg_hSKeys.hKeyl;

	sg_hSKeys.hFenhao = CreateWindowEx  (CTRL_SKBK_CUSTOM,
							";",
							STYLE_SK,
							WS_EX_TRANSPARENT,
							IDC_FENHAO,
							STATIC_LEFT + 10 * STATIC_W + 10 +
							STATIC_CAPS_OFFSET, STATIC_TOP + 2 * STATIC_H + 2,
							STATIC_W, STATIC_H, parent, 0);
	hSoftKeyWnd[39] = sg_hSKeys.hFenhao;

	sg_hSKeys.hYinhao = CreateWindowEx  (CTRL_SKBK_CUSTOM,
							"'",
							STYLE_SK,
							WS_EX_TRANSPARENT,
							IDC_YINHAO,
							STATIC_LEFT + 11 * STATIC_W + 11 +
							STATIC_CAPS_OFFSET, STATIC_TOP + 2 * STATIC_H + 2,
							STATIC_W, STATIC_H, parent, 0);
	hSoftKeyWnd[40] = sg_hSKeys.hYinhao;

	sg_hSKeys.hEnter = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						   "Enter",
						   STYLE_SK,
						   WS_EX_TRANSPARENT,
						   IDC_ENTER,
						   STATIC_LEFT + 12 * STATIC_W + 12 +
						   STATIC_CAPS_OFFSET, STATIC_TOP + 2 * STATIC_H + 2,
						   STATIC_W + STATIC_ENTER_OFFSET, STATIC_H, parent, 0);
						   //STATIC_W + 42, STATIC_H, parent, 0);
	hSoftKeyWnd[41] = sg_hSKeys.hEnter;

/*************************************end of the third line*****************************************/

/*************************************begin the fourth line*****************************************/

	sg_hSKeys.hShift = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						   "Shift",
						   (STYLE_SK | SKBKS_CHECKED), 
						   WS_EX_TRANSPARENT,
						   IDC_SHIFT,
						   STATIC_LEFT + 0 * STATIC_W + 0,
						   STATIC_TOP + 3 * STATIC_H + 3,
						   STATIC_W + STATIC_SHIFT_OFFSET, STATIC_H, parent, 0);
	hSoftKeyWnd[42] = sg_hSKeys.hShift;


	sg_hSKeys.hKeyz = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "z",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYZ,
						  STATIC_LEFT + 1 * STATIC_W + 1 + STATIC_SHIFT_OFFSET,
						  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[43] = sg_hSKeys.hKeyz;

	sg_hSKeys.hKeyx = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "x",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYX,
						  STATIC_LEFT + 2 * STATIC_W + 2 + STATIC_SHIFT_OFFSET,
						  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[44] = sg_hSKeys.hKeyx;

	sg_hSKeys.hKeyc = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "c",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYC,
						  STATIC_LEFT + 3 * STATIC_W + 3 + STATIC_SHIFT_OFFSET,
						  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[45] = sg_hSKeys.hKeyc;

	sg_hSKeys.hKeyv = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "v",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYV,
						  STATIC_LEFT + 4 * STATIC_W + 4 + STATIC_SHIFT_OFFSET,
						  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[46] = sg_hSKeys.hKeyv;


	sg_hSKeys.hKeyb = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "b",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYB,
						  STATIC_LEFT + 5 * STATIC_W + 5 + STATIC_SHIFT_OFFSET,
						  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[47] = sg_hSKeys.hKeyb;


	sg_hSKeys.hKeyn = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "n",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYN,
						  STATIC_LEFT + 6 * STATIC_W + 6 + STATIC_SHIFT_OFFSET,
						  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[48] = sg_hSKeys.hKeyn;

	sg_hSKeys.hKeym = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "m",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_KEYM,
						  STATIC_LEFT + 7 * STATIC_W + 7 + STATIC_SHIFT_OFFSET,
						  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[49] = sg_hSKeys.hKeym;

	sg_hSKeys.hDuhao = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						   ",",
						   STYLE_SK,
						   WS_EX_TRANSPARENT,
						   IDC_DUHAO,
						   STATIC_LEFT + 8 * STATIC_W + 8 + STATIC_SHIFT_OFFSET,
						   STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
						   parent, 0);
	hSoftKeyWnd[50] = sg_hSKeys.hDuhao;

	sg_hSKeys.hDot = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						 ".",
						 STYLE_SK,
						 WS_EX_TRANSPARENT,
						 IDC_DOT,
						 STATIC_LEFT + 9 * STATIC_W + 9 + STATIC_SHIFT_OFFSET,
						 STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
						 parent, 0);
	hSoftKeyWnd[51] = sg_hSKeys.hDot;

	sg_hSKeys.hChu = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						 "/",
						 STYLE_SK,
						 WS_EX_TRANSPARENT,
						 IDC_CHU,
						 STATIC_LEFT + 10 * STATIC_W + 10 + STATIC_SHIFT_OFFSET,
						 STATIC_TOP + 3 * STATIC_H + 3, STATIC_W, STATIC_H,
						 parent, 0);
	hSoftKeyWnd[52] = sg_hSKeys.hChu;

	sg_hSKeys.hPageUp = CreateWindowEx  (CTRL_SKBK_CUSTOM,
							"Pu",
							STYLE_SK,
							WS_EX_TRANSPARENT,
							IDC_PAGEUP,
							STATIC_LEFT + 11 * STATIC_W + 11 +
							STATIC_SHIFT_OFFSET, STATIC_TOP + 3 * STATIC_H + 3,
							STATIC_W + STATIC_ARROW_OFFSET, STATIC_H, parent, 0);
							//STATIC_W + 4, STATIC_H, parent, 0);
	hSoftKeyWnd[53] = sg_hSKeys.hPageUp;

	sg_hSKeys.hUp = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						"Up",
						STYLE_SK,
						WS_EX_TRANSPARENT,
						IDC_UP,
						STATIC_LEFT + 12 * STATIC_W + 12 +
						STATIC_SHIFT_OFFSET + STATIC_ARROW_OFFSET,
						STATIC_TOP + 3 * STATIC_H + 3, STATIC_W + STATIC_ARROW_OFFSET, STATIC_H,
						parent, 0/*(DWORD) GetSystemBitmap (SYSBMP_ARROW_UP)*/);

	hSoftKeyWnd[54] = sg_hSKeys.hUp;


	sg_hSKeys.hPageDown = CreateWindowEx  (CTRL_SKBK_CUSTOM,
							  "Pd",
							  STYLE_SK,
							  WS_EX_TRANSPARENT,
							  IDC_PAGEDOWN,
							  STATIC_LEFT + 13 * STATIC_W + 13 +
							  STATIC_SHIFT_OFFSET + STATIC_ARROW_OFFSET*2,
							  STATIC_TOP + 3 * STATIC_H + 3, STATIC_W + STATIC_ARROW_OFFSET,
							  STATIC_H, parent, 0);
	hSoftKeyWnd[55] = sg_hSKeys.hPageDown;

/*************************************end of the fourth line***************************************************/

/*************************************begin the fifth line***************************************************/

	sg_hSKeys.hCtrl = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "Ctrl",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_CTRL, STATIC_LEFT + 0 * STATIC_W + 0,
						  STATIC_TOP + 4 * STATIC_H + 4, STATIC_W + STATIC_CTRL_OFFSET,
						  //STATIC_TOP + 4 * STATIC_H + 4, 3 * STATIC_W + 5,
						  STATIC_H, parent, 0);
	hSoftKeyWnd[56] = sg_hSKeys.hCtrl;


	sg_hSKeys.hSpace = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						   "Space",
						   STYLE_SK,
						   WS_EX_TRANSPARENT,
						   IDC_SPACE, STATIC_LEFT + STATIC_W + STATIC_CTRL_OFFSET,//STATIC_LEFT +  3 * STATIC_W + 6,
						   STATIC_TOP + 4 * STATIC_H + 4, STATIC_W + STATIC_SPACE_OFFSET,
						   //STATIC_TOP + 4 * STATIC_H + 4, 9 * STATIC_W + 18,
						   STATIC_H, parent, 0);
	hSoftKeyWnd[57] = sg_hSKeys.hSpace;

	sg_hSKeys.hLeft = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "Lf",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_LEFT,
						  STATIC_LEFT + STATIC_W*2 + STATIC_CTRL_OFFSET + STATIC_SPACE_OFFSET, //STATIC_LEFT + 12 * STATIC_W + 25,
						  STATIC_TOP + 4 * STATIC_H + 4, STATIC_W + STATIC_ARROW_OFFSET, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[58] = sg_hSKeys.hLeft;

	sg_hSKeys.hDown = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  "Dw",
						  STYLE_SK,
						  WS_EX_TRANSPARENT,
						  IDC_DOWN,
						  STATIC_LEFT + STATIC_W*3 + STATIC_CTRL_OFFSET + STATIC_SPACE_OFFSET + STATIC_ARROW_OFFSET,//STATIC_LEFT + 14 * STATIC_W + 10,
						  STATIC_TOP + 4 * STATIC_H + 4, STATIC_W + STATIC_ARROW_OFFSET, STATIC_H,
						  parent, 0);
	hSoftKeyWnd[59] = sg_hSKeys.hDown;

	sg_hSKeys.hRight = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						   "Rg",
						   STYLE_SK,
						   WS_EX_TRANSPARENT,
						   IDC_RIGHT,
						   STATIC_LEFT + STATIC_W*4 + STATIC_CTRL_OFFSET + STATIC_SPACE_OFFSET + STATIC_ARROW_OFFSET*2,//STATIC_LEFT + 15 * STATIC_W + 15,
						   STATIC_TOP + 4 * STATIC_H + 4, STATIC_W + STATIC_ARROW_OFFSET,
						   STATIC_H, parent,
						   0);
	hSoftKeyWnd[60] = sg_hSKeys.hRight;

/***********************************Create softkey end**********************************************************/
} 
#endif

#if 1
static void OnSkbQuertyCommand(HWND hwnd, int id, int code)
{
	static int s_nPushed = 0;
	
	if ((SKBTYPE_QUERTY == sg_nSkbType)
		&& (SKBKN_PUSHED == code))
	{
		if (id == IDC_SHIFT)
		{
			if (s_nPushed == 0)
			{
				s_nPushed = 1;

				SetWindowText (sg_hSKeys.hKeya, "A");
				SetWindowText (sg_hSKeys.hKeyb, "B");
				SetWindowText (sg_hSKeys.hKeyc, "C");
				SetWindowText (sg_hSKeys.hKeyd, "D");
				SetWindowText (sg_hSKeys.hKeye, "E");
				SetWindowText (sg_hSKeys.hKeyf, "F");
				SetWindowText (sg_hSKeys.hKeyg, "G");
				SetWindowText (sg_hSKeys.hKeyh, "H");
				SetWindowText (sg_hSKeys.hKeyi, "I");
				SetWindowText (sg_hSKeys.hKeyj, "J");
				SetWindowText (sg_hSKeys.hKeyk, "K");
				SetWindowText (sg_hSKeys.hKeyl, "L");
				SetWindowText (sg_hSKeys.hKeym, "M");
				SetWindowText (sg_hSKeys.hKeyn, "N");
				SetWindowText (sg_hSKeys.hKeyo, "O");
				SetWindowText (sg_hSKeys.hKeyp, "P");
				SetWindowText (sg_hSKeys.hKeyq, "Q");
				SetWindowText (sg_hSKeys.hKeyr, "R");
				SetWindowText (sg_hSKeys.hKeys, "S");
				SetWindowText (sg_hSKeys.hKeyt, "T");
				SetWindowText (sg_hSKeys.hKeyu, "U");
				SetWindowText (sg_hSKeys.hKeyv, "V");
				SetWindowText (sg_hSKeys.hKeyw, "W");
				SetWindowText (sg_hSKeys.hKeyx, "X");
				SetWindowText (sg_hSKeys.hKeyy, "Y");
				SetWindowText (sg_hSKeys.hKeyz, "Z");

			}
			else
			{
				s_nPushed = 0;

				SetWindowText (sg_hSKeys.hKeya, "a");
				SetWindowText (sg_hSKeys.hKeyb, "b");
				SetWindowText (sg_hSKeys.hKeyc, "c");
				SetWindowText (sg_hSKeys.hKeyd, "d");
				SetWindowText (sg_hSKeys.hKeye, "e");
				SetWindowText (sg_hSKeys.hKeyf, "f");
				SetWindowText (sg_hSKeys.hKeyg, "g");
				SetWindowText (sg_hSKeys.hKeyh, "h");
				SetWindowText (sg_hSKeys.hKeyi, "i");
				SetWindowText (sg_hSKeys.hKeyj, "j");
				SetWindowText (sg_hSKeys.hKeyk, "k");
				SetWindowText (sg_hSKeys.hKeyl, "l");
				SetWindowText (sg_hSKeys.hKeym, "m");
				SetWindowText (sg_hSKeys.hKeyn, "n");
				SetWindowText (sg_hSKeys.hKeyo, "o");
				SetWindowText (sg_hSKeys.hKeyp, "p");
				SetWindowText (sg_hSKeys.hKeyq, "q");
				SetWindowText (sg_hSKeys.hKeyr, "r");
				SetWindowText (sg_hSKeys.hKeys, "s");
				SetWindowText (sg_hSKeys.hKeyt, "t");
				SetWindowText (sg_hSKeys.hKeyu, "u");
				SetWindowText (sg_hSKeys.hKeyv, "v");
				SetWindowText (sg_hSKeys.hKeyw, "w");
				SetWindowText (sg_hSKeys.hKeyx, "x");
				SetWindowText (sg_hSKeys.hKeyy, "y");
				SetWindowText (sg_hSKeys.hKeyz, "z");

			}
		}


		if (!IsWindow(sg_hImeWnd))
			return;
			
		if ((id - 101) < 36)
		{
			PostMessage (sg_hImeWnd, MSG_CHAR, KeyValue[s_nPushed][id - 101], 0);
		}
		else if((id-137) < 16)
		{
			PostMessage(sg_hImeWnd, MSG_CHAR, symbolKeyValue[id - 137], 0);
		}
		else
		{
			clean_left_ctrl = 1;

			if (id == IDC_BACKSPACE)
			{
				PostMessage(sg_hImeWnd, MSG_CHAR, 127, 0);
				//PostMessage (sg_hImeWnd, MSG_KEYDOWN_SKB, SCANCODE_BACKSPACE, 0);
			}
			else if (id == IDC_ENTER)
			{
				PostMessage (sg_hImeWnd, MSG_IME_CLOSE, 0, 0); 
			}
			else if(id == IDC_SPACE)
			{
				PostMessage(sg_hImeWnd, MSG_CHAR, ' ', 0);
			}
		}
	}	
}
#else

static void OnSkbQuertyCommand(HWND hwnd, int id, int code)
{
	static int s_nPushed = 0;
	
	if ((SKBTYPE_QUERTY == sg_nSkbType)
		&& (SKBKN_PUSHED == code))
	{
		if (id == IDC_SHIFT || id == IDC_CAPS)
		{
			if (s_nPushed == 0)
			{
				s_nPushed = 1;
				SetWindowText (sg_hSKeys.hNum1, "!");
				SetWindowText (sg_hSKeys.hNum2, "@");
				SetWindowText (sg_hSKeys.hNum3, "#");
				SetWindowText (sg_hSKeys.hNum4, "$");
				SetWindowText (sg_hSKeys.hNum5, "%");
				SetWindowText (sg_hSKeys.hNum6, "^");
				SetWindowText (sg_hSKeys.hNum7, "&");
				SetWindowText (sg_hSKeys.hNum8, "*");
				SetWindowText (sg_hSKeys.hNum9, "(");
				SetWindowText (sg_hSKeys.hNum0, ")");

				SetWindowText (sg_hSKeys.hKeya, "A");
				SetWindowText (sg_hSKeys.hKeyb, "B");
				SetWindowText (sg_hSKeys.hKeyc, "C");
				SetWindowText (sg_hSKeys.hKeyd, "D");
				SetWindowText (sg_hSKeys.hKeye, "E");
				SetWindowText (sg_hSKeys.hKeyf, "F");
				SetWindowText (sg_hSKeys.hKeyg, "G");
				SetWindowText (sg_hSKeys.hKeyh, "H");
				SetWindowText (sg_hSKeys.hKeyi, "I");
				SetWindowText (sg_hSKeys.hKeyj, "J");
				SetWindowText (sg_hSKeys.hKeyk, "K");
				SetWindowText (sg_hSKeys.hKeyl, "L");
				SetWindowText (sg_hSKeys.hKeym, "M");
				SetWindowText (sg_hSKeys.hKeyn, "N");
				SetWindowText (sg_hSKeys.hKeyo, "O");
				SetWindowText (sg_hSKeys.hKeyp, "P");
				SetWindowText (sg_hSKeys.hKeyq, "Q");
				SetWindowText (sg_hSKeys.hKeyr, "R");
				SetWindowText (sg_hSKeys.hKeys, "S");
				SetWindowText (sg_hSKeys.hKeyt, "T");
				SetWindowText (sg_hSKeys.hKeyu, "U");
				SetWindowText (sg_hSKeys.hKeyv, "V");
				SetWindowText (sg_hSKeys.hKeyw, "W");
				SetWindowText (sg_hSKeys.hKeyx, "X");
				SetWindowText (sg_hSKeys.hKeyy, "Y");
				SetWindowText (sg_hSKeys.hKeyz, "Z");

				SetWindowText (sg_hSKeys.hJian, "_");
				SetWindowText (sg_hSKeys.hEqual, "+");
				SetWindowText (sg_hSKeys.hKuohao1, "{");
				SetWindowText (sg_hSKeys.hKuohao2, "}");
				SetWindowText (sg_hSKeys.hPline, "|");
				SetWindowText (sg_hSKeys.hFenhao, ":");
				SetWindowText (sg_hSKeys.hYinhao, "\"");
				SetWindowText (sg_hSKeys.hDuhao, "<");
				SetWindowText (sg_hSKeys.hDot, ">");
				SetWindowText (sg_hSKeys.hChu, "?");
			}
			else
			{
				s_nPushed = 0;
				SetWindowText (sg_hSKeys.hNum1, "1");
				SetWindowText (sg_hSKeys.hNum2, "2");
				SetWindowText (sg_hSKeys.hNum3, "3");
				SetWindowText (sg_hSKeys.hNum4, "4");
				SetWindowText (sg_hSKeys.hNum5, "5");
				SetWindowText (sg_hSKeys.hNum6, "6");
				SetWindowText (sg_hSKeys.hNum7, "7");
				SetWindowText (sg_hSKeys.hNum8, "8");
				SetWindowText (sg_hSKeys.hNum9, "9");
				SetWindowText (sg_hSKeys.hNum0, "0");

				SetWindowText (sg_hSKeys.hKeya, "a");
				SetWindowText (sg_hSKeys.hKeyb, "b");
				SetWindowText (sg_hSKeys.hKeyc, "c");
				SetWindowText (sg_hSKeys.hKeyd, "d");
				SetWindowText (sg_hSKeys.hKeye, "e");
				SetWindowText (sg_hSKeys.hKeyf, "f");
				SetWindowText (sg_hSKeys.hKeyg, "g");
				SetWindowText (sg_hSKeys.hKeyh, "h");
				SetWindowText (sg_hSKeys.hKeyi, "i");
				SetWindowText (sg_hSKeys.hKeyj, "j");
				SetWindowText (sg_hSKeys.hKeyk, "k");
				SetWindowText (sg_hSKeys.hKeyl, "l");
				SetWindowText (sg_hSKeys.hKeym, "m");
				SetWindowText (sg_hSKeys.hKeyn, "n");
				SetWindowText (sg_hSKeys.hKeyo, "o");
				SetWindowText (sg_hSKeys.hKeyp, "p");
				SetWindowText (sg_hSKeys.hKeyq, "q");
				SetWindowText (sg_hSKeys.hKeyr, "r");
				SetWindowText (sg_hSKeys.hKeys, "s");
				SetWindowText (sg_hSKeys.hKeyt, "t");
				SetWindowText (sg_hSKeys.hKeyu, "u");
				SetWindowText (sg_hSKeys.hKeyv, "v");
				SetWindowText (sg_hSKeys.hKeyw, "w");
				SetWindowText (sg_hSKeys.hKeyx, "x");
				SetWindowText (sg_hSKeys.hKeyy, "y");
				SetWindowText (sg_hSKeys.hKeyz, "z");

				SetWindowText (sg_hSKeys.hJian, "-");
				SetWindowText (sg_hSKeys.hEqual, "=");
				SetWindowText (sg_hSKeys.hKuohao1, "[");
				SetWindowText (sg_hSKeys.hKuohao2, "]");
				SetWindowText (sg_hSKeys.hPline, "\\");
				SetWindowText (sg_hSKeys.hFenhao, ";");
				SetWindowText (sg_hSKeys.hYinhao, "'");
				SetWindowText (sg_hSKeys.hDuhao, ",");
				SetWindowText (sg_hSKeys.hDot, ".");
				SetWindowText (sg_hSKeys.hChu, "/");
			}
		}


		if (!IsWindow(sg_hImeWnd))
			return;
			
		if ((id - 101) < 47)
		{
			PostMessage (sg_hImeWnd, MSG_CHAR, KeyValue[s_nPushed][id - 101], 0);
		}
		else
		{
			clean_left_ctrl = 1;

			if (id == IDC_BACKSPACE)
			{
				PostMessage(sg_hImeWnd, MSG_CHAR, 127, 0);
				//PostMessage (sg_hImeWnd, MSG_KEYDOWN_SKB, SCANCODE_BACKSPACE, 0);
			}
			else if (id == IDC_ENTER
				|| id == IDC_ESC)
			{
				PostMessage (sg_hImeWnd, MSG_IME_CLOSE, 0, 0); 
			}
			else if (id == IDC_CTRL)
			{
				PostMessage (sg_hImeWnd, MSG_KEYUP_SKB, SCANCODE_LEFTCONTROL, 0);
			}
			else if (id == IDC_DEL)
			{
				if(IsHanziInput)
					PostMessage (sg_hImeWnd, MSG_KEYDOWN_SKB, SCANCODE_REMOVE, 0);
				else
					PostMessage (sg_hTargetWnd, MSG_KEYDOWN, SCANCODE_REMOVE, 0);
			}
			else if (id == IDC_TAB)
			{
				ClrIn();
				InvalidateRect(sg_hImeWnd, NULL, TRUE);
			}
			else if (id == IDC_PAGEUP)
			{
				if(IsHanziInput)
					PostMessage (sg_hImeWnd, MSG_CHAR, '-', 0);
			}
			else if (id == IDC_PAGEDOWN)
			{
				if(IsHanziInput)
					PostMessage (sg_hImeWnd, MSG_CHAR, '=', 0);
			}
			else if (id == IDC_UP)
			{
				//PostMessage (sg_hImeWnd, MSG_KEYDOWN_SKB, 
				//	SCANCODE_CURSORBLOCKUP, 0);
			}
			else if (id == IDC_DOWN)
			{
				//PostMessage (sg_hImeWnd, MSG_KEYDOWN_SKB, 
				//	SCANCODE_CURSORBLOCKDOWN, 0);
			}
			else if (id == IDC_LEFT)
			{
				if(IsHanziInput)
					PostMessage (sg_hImeWnd, MSG_KEYDOWN_SKB, 
						SCANCODE_CURSORBLOCKLEFT, 0);
				else
					PostMessage (sg_hTargetWnd, MSG_KEYDOWN, 
						SCANCODE_CURSORBLOCKLEFT, 0);
			}
			else if (id == IDC_RIGHT)
			{
				if(IsHanziInput)
					PostMessage (sg_hImeWnd, MSG_KEYDOWN_SKB, 
						SCANCODE_CURSORBLOCKRIGHT, 0);
				else
					PostMessage (sg_hTargetWnd, MSG_KEYDOWN, 
						SCANCODE_CURSORBLOCKRIGHT, 0);
			}	
		}
	}	
}
#endif

static int WndQuertySkbProc(HWND hwnd, int message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
	{
		case MSG_NCACTIVATE:
			return 0;
			
		case MSG_CREATE :
			{
				CreateSoftKey(hwnd);
			}
			break;
			
		case MSG_COMMAND:
			{
				OnSkbQuertyCommand(hwnd, LOWORD(wparam), HIWORD(wparam));
			}
			return 0;
			
		case MSG_RBUTTONUP:
		{
	        int x_pos = LOSWORD (lparam);
	        int y_pos = HISWORD (lparam);

	        if (is_mouse_in_client(hwnd,x_pos, y_pos))
	        {
				HideInputWnds();
	        }		
		}
		break; 
		
		default:
			break;
	}
	
	return DefaultMainWinProc(hwnd, message, wparam, lparam);
}

static HWND CreateSkbQuertyWnd(HWND hHosting)
{
	MAINWINCREATE createInfo;
	
	createInfo.dwStyle = WS_ABSSCRPOS | WS_BORDER /*| WS_VISIBLE*/;
	createInfo.dwExStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
	createInfo.spCaption = "The QUERTY SKB" ;
	createInfo.hMenu = 0;
	createInfo.hCursor = GetSystemCursor(0);
	createInfo.hIcon = 0;
	createInfo.MainWindowProc = WndQuertySkbProc;

	createInfo.lx = 0; 
	createInfo.ty = IME_WND_H; 
	createInfo.rx = SKB_QUERY_WND_W;
	createInfo.by = SKB_QUERY_WND_H + IME_WND_H;

	createInfo.iBkColor = WND_BK_COLOR;
	createInfo.dwAddData = 0;
	createInfo.hHosting = hHosting;

	sg_hSkbQuerty = CreateMainWindow(&createInfo);
	if(HWND_INVALID == sg_hSkbQuerty)
		return HWND_INVALID;	

	return sg_hSkbQuerty;
}


/***************************************************************************
 *								softkeyboard : T9							   *
 ***************************************************************************/
static void CreateT9Skeys(HWND parent)
{		
	sg_hT9SKeys.hT9Sk1 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						 "1",
						 STYLE_T9SK,
						 WS_EX_TRANSPARENT,
						 IDC_T9SK1,
						 STATIC_LEFT + 0 * STATIC_T9SKB_W,
						 STATIC_T9_TOP + 0 * STATIC_T9SKB_H,
						 STATIC_T9SKB_W, STATIC_T9SKB_H, parent, 0);

	
	sg_hT9SKeys.hT9Sk2 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						 "2",
						 STYLE_T9SK,
						 WS_EX_TRANSPARENT,
						 IDC_T9SK2,
						 STATIC_LEFT + 1 * STATIC_T9SKB_W + 1,
						 STATIC_T9_TOP + 0 * STATIC_T9SKB_H,
						 STATIC_T9SKB_W, STATIC_T9SKB_H, parent, 0);

	
	sg_hT9SKeys.hT9Sk3 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						 "3",
						 STYLE_T9SK,
						 WS_EX_TRANSPARENT,
						 IDC_T9SK3,
						 STATIC_LEFT + 2 * STATIC_T9SKB_W + 2,
						 STATIC_T9_TOP + 0 * STATIC_T9SKB_H,
						 STATIC_T9SKB_W, STATIC_T9SKB_H, parent, 0);

	
	sg_hT9SKeys.hT9Sk4 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						 "4",
						 STYLE_T9SK,
						 WS_EX_TRANSPARENT,
						 IDC_T9SK4,
						 STATIC_LEFT + 0 * STATIC_T9SKB_W,
						 STATIC_T9_TOP + 1 * STATIC_T9SKB_H + 1,
						 STATIC_T9SKB_W, STATIC_T9SKB_H, parent, 0);
	
	
	sg_hT9SKeys.hT9Sk5 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						 "5",
						 STYLE_T9SK,
						 WS_EX_TRANSPARENT,
						 IDC_T9SK5,
						 STATIC_LEFT + 1 * STATIC_T9SKB_W + 1,
						 STATIC_T9_TOP + 1 * STATIC_T9SKB_H + 1,
						 STATIC_T9SKB_W, STATIC_T9SKB_H, parent, 0);
	
	
	sg_hT9SKeys.hT9Sk6 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						 "6",
						 STYLE_T9SK,
						 WS_EX_TRANSPARENT,
						 IDC_T9SK6,
						 STATIC_LEFT + 2 * STATIC_T9SKB_W + 2,
						 STATIC_T9_TOP + 1 * STATIC_T9SKB_H + 1,
						 STATIC_T9SKB_W, STATIC_T9SKB_H, parent, 0);


	sg_hT9SKeys.hT9Sk7 = CreateWindowEx (CTRL_SKBK_CUSTOM,
							 "7",
							 STYLE_T9SK,
							 WS_EX_TRANSPARENT,
							 IDC_T9SK7,
							 STATIC_LEFT + 0 * STATIC_T9SKB_W,
							 STATIC_T9_TOP + 2 * STATIC_T9SKB_H + 2,
							 STATIC_T9SKB_W,
							 STATIC_T9SKB_H, parent, 0);

	
	sg_hT9SKeys.hT9Sk8 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						 "8",
						 STYLE_T9SK,
						 WS_EX_TRANSPARENT,
						 IDC_T9SK8,
						 STATIC_LEFT + 1 * STATIC_T9SKB_W + 1,
						 STATIC_T9_TOP + 2 * STATIC_T9SKB_H + 2,
						 STATIC_T9SKB_W,
						 STATIC_T9SKB_H, parent, 0);

	
	sg_hT9SKeys.hT9Sk9 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						 "9",
						 STYLE_T9SK,
						 WS_EX_TRANSPARENT,
						 IDC_T9SK9,
						 STATIC_LEFT + 2 * STATIC_T9SKB_W + 2,
						 STATIC_T9_TOP + 2 * STATIC_T9SKB_H + 2,
						 STATIC_T9SKB_W,
						 STATIC_T9SKB_H, parent, 0);

	
	sg_hT9SKeys.hT9Sk0 = CreateWindowEx  (CTRL_SKBK_CUSTOM,
							 "0",
							 STYLE_T9SK,
							 WS_EX_TRANSPARENT,
							 IDC_T9SK0,
							 STATIC_LEFT + 0 * STATIC_T9SKB_W,
							 STATIC_T9_TOP + 3 * STATIC_T9SKB_H+3,
							 STATIC_T9SKB_W, STATIC_T9SKB_H, parent, 0);


	sg_hT9SKeys.hT9SkOk = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						  ".",
						  STYLE_T9SK,
						  WS_EX_TRANSPARENT,
						  IDC_T9CTRL,
						  STATIC_LEFT + 1 * STATIC_T9SKB_W + 1,
						  STATIC_T9_TOP + 3 * STATIC_T9SKB_H+3,
						  STATIC_T9SKB_W, 
						  STATIC_T9SKB_H, parent, 0);	


	sg_hT9SKeys.hT9SkClear = CreateWindowEx  (CTRL_SKBK_CUSTOM,
						 "<--",
						 STYLE_T9SK,
						 WS_EX_TRANSPARENT,
						 IDC_T9BACKSPACE,
						 STATIC_LEFT + 2 * STATIC_T9SKB_W + 2,
						 STATIC_T9_TOP + 3 * STATIC_T9SKB_H+3,
						 STATIC_T9SKB_W, STATIC_T9SKB_H, parent,
						 0);
					  
}

static void OnSkbT9Command(HWND hwnd, int id, int code)
{
	if (SKBTYPE_T9 == sg_nSkbType)
	{
		if (SKBKN_PUSHED == code)
		{
			if (0 <= (id - IDC_T9SK1) && (id - IDC_T9SK1) <= 9)
			{
				PostMessage (sg_hImeWnd, MSG_CHAR, hzNumber_Char[id - IDC_T9SK1], 0);
			}
			else if (IDC_T9BACKSPACE == id)
			{
				if(SKBATTR_ST_NUM != sg_nSkbAttrSt)
					//PostMessage(sg_hImeWnd, MSG_CHAR, 127, 0);
					PostMessage (sg_hImeWnd, MSG_KEYDOWN_SKB,
						SCANCODE_BACKSPACE, 0);					
				else
					PostMessage (sg_hTargetWnd, MSG_KEYDOWN,
						SCANCODE_BACKSPACE, 0);
			}
			else if (IDC_T9CTRL == id)
					 
			{
				if (sg_hTargetWnd && (SKBATTR_NUM == (sg_dwSkbAttr & SKBATTR_MASK)))
					PostMessage (sg_hTargetWnd, MSG_CHAR, '.', 0);
			}
		}
	#if 0	
		else if (SKBKN_UNPUSHED == code)
		{
			if ((IDC_T9CTRL == id) && (SKBATTR_NUM != (sg_dwSkbAttr & SKBATTR_MASK)))
			{	
				PostMessage (sg_hImeWnd, MSG_KEYUP_SKB, SCANCODE_LEFTCONTROL, 0);
			}
		}
	#endif	
	}
}

static int WndT9SkbProc(HWND hwnd, int message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
	{
		case MSG_NCACTIVATE:
			return 0;
			
		case MSG_CREATE :
			{
				CreateT9Skeys(hwnd);
			}
			break;

		case MSG_COMMAND:
			{
				OnSkbT9Command(hwnd, LOWORD(wparam), HIWORD(wparam));
			}
			return 0;
#if 0
		case MSG_RBUTTONUP:
			{
		        int x_pos = LOSWORD (lparam);
		        int y_pos = HISWORD (lparam);

		        if (is_mouse_in_client(hwnd,x_pos, y_pos))
		        {
					HideInputWnds();
		        }		
			}
			break; 
#endif			
		default:
			break;
	}
	
	return DefaultMainWinProc(hwnd, message, wparam, lparam);	
}

static HWND CreateSkbT9Wnd(HWND hHosting)
{
	MAINWINCREATE createInfo;
	
	createInfo.dwStyle = WS_ABSSCRPOS | WS_BORDER /*| WS_VISIBLE*/;
	createInfo.dwExStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
	createInfo.spCaption = "The T9 SKB" ;
	createInfo.hMenu = 0;
	createInfo.hCursor = GetSystemCursor(0);
	createInfo.hIcon = 0;
	createInfo.MainWindowProc = WndT9SkbProc;

	createInfo.lx = 0; 
	createInfo.ty = IME_WND_H; 
	createInfo.rx = SKB_T9_WND_W;
	createInfo.by = SKB_T9_WND_H + IME_WND_H;

	createInfo.iBkColor = WND_BK_COLOR;
	createInfo.dwAddData = 0;
	createInfo.hHosting = hHosting;	
	sg_hSkbT9 = CreateMainWindow(&createInfo);
	if(HWND_INVALID == sg_hSkbT9)
		return HWND_INVALID;

	return sg_hSkbT9;
}

static int OnImeKeydownSkb(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (!IsImeOpened) 
		return 0;
		
	if(SKBTYPE_QUERTY == sg_nSkbType)
	{
	    if (wParam != SCANCODE_LEFTCONTROL)
	        clean_left_ctrl = 0;
	    else
	        clean_left_ctrl = 1;
	        
#if 1
	    if (IsImeOpened) {
	        if (wParam == SCANCODE_SPACE && (lParam & KS_CTRL)) {
	            //toggle_half_full ();
	            //InvalidateRect (hWnd, NULL, TRUE);
	            return 0;
	        }
	        else if (wParam == SCANCODE_PERIOD && (lParam & KS_CTRL)) {
	            //toggle_punc ();
	            //InvalidateRect (hWnd, NULL, TRUE);
	            return 0;
	        }
	        else if ((wParam == SCANCODE_LEFTSHIFT) && (lParam & KS_CTRL)) {
	            //imeSetStatus (IS_METHOD, -1);
	            //InvalidateRect (hWnd, NULL, TRUE);
	            return 0;
	        }

	        if (sg_hTargetWnd && imeIsEffectiveIME () && imeIsSpecialKey (wParam))
	            PostMessage (sg_hTargetWnd, MSG_KEYDOWN, wParam, lParam);
        
	    }
#endif	

	}
    else /* if (SKBTYPE_T9 == sg_nSkbType) */
    {
		if (wParam == SCANCODE_BACKSPACE)
		{
            switch(sg_nSkbAttrSt)  
            {  
                case SKBATTR_ST_CN:
                	if (0 == is_fh_input)
                	{
	                    if(cur_status == 1)  
	                        cur_status = 0;  
	                    else  
	                    {  
	                    	if(strlen(py_key)>0)  
	                        {
	                            delete_last_py();   //删除前一个拼音字母
	                            InvalidateRect(hWnd, NULL, TRUE);
	                        }    
	                        else  
	                        {  
	                            if(sg_hTargetWnd)  
	                                PostMessage(sg_hTargetWnd, MSG_KEYDOWN, wParam, lParam);  
	                        }  
	                    }  
                    }
                    else
                    {
	                    if(cur_status == 1)  
	                    {
	                        ClrInBuf();         //清除符号输入 
	                        InvalidateRect(hWnd, NULL, TRUE);
	                    }    
	                    else  
	                    {  
	                        if(sg_hTargetWnd)   
	                            PostMessage(sg_hTargetWnd, MSG_KEYDOWN, wParam, lParam);  
	                    } 
                    }
                    break;  

                case SKBATTR_ST_EN:  
                    if(cur_status == 1)  
                    {
                        ClrInBuf();         //清除英文输入？  
                        InvalidateRect(hWnd, NULL, TRUE);
                    }    
                    else  
                    {  
                        if(sg_hTargetWnd)   
                            PostMessage(sg_hTargetWnd, MSG_KEYDOWN, wParam, lParam);  
                    }  
                    break;  

                case SKBATTR_NUM:  
                    if(sg_hTargetWnd) 
                    	PostMessage (sg_hTargetWnd, MSG_KEYDOWN, wParam, lParam);  
                    break;  
            }
		}
    }
    
	return 0;
}

static int OnImeKeyupSkb(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (!IsImeOpened) 
		return 0;
		
	if (SKBTYPE_QUERTY == sg_nSkbType)
	{
        if (wParam == SCANCODE_LEFTCONTROL && clean_left_ctrl ) 
        {
            toggle_input_method ();
            InvalidateRect (hWnd, NULL, TRUE);
        }

        //if (sg_hTargetWnd && imeIsEffectiveIME () && imeIsSpecialKey (wParam))
        //    PostMessage (sg_hTargetWnd, MSG_KEYUP, wParam, lParam);
    }
    else /* if (SKBTYPE_T9 == sg_nSkbType) */
    {
		if (SCANCODE_LEFTCONTROL == wParam)
		{
			toggle_input_method();
			InvalidateRect (hWnd, NULL, TRUE);
		}
    }
    
    return 0;
}

static int OnImeKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (!IsImeOpened) 
		return 0;
		
	if (SKBTYPE_T9 == sg_nSkbType)
	{
	    switch(wParam)  
	    { 
			case SCANCODE_1:
			case SCANCODE_2:
			case SCANCODE_3:
			case SCANCODE_4:
			case SCANCODE_5:
			case SCANCODE_6:
			case SCANCODE_7:
			case SCANCODE_8:
			case SCANCODE_9:
			case SCANCODE_0:
				{
					PostMessage(GetDlgItem(sg_hSkbT9, wParam - SCANCODE_1 + IDC_T9SK1),
						SKBKM_PUSHDOWN, 0, 0);
					InvalidateRect(hWnd, NULL, TRUE);	
				}
				break;
			case SCANCODE_F1:
				{
					PostMessage(GetDlgItem(sg_hSkbT9, IDC_T9CTRL), 
						SKBKM_PUSHDOWN, 0, 0);
					InvalidateRect(hWnd, NULL, TRUE);	
				}
				break;

			case SCANCODE_F2:
				{
					PostMessage(GetDlgItem(sg_hSkbT9, IDC_T9BACKSPACE), 
						SKBKM_PUSHDOWN, 0, 0);
					InvalidateRect(hWnd, NULL, TRUE);
				}
				break;

			case SCANCODE_F11:
				{
					if ((1 == cur_status)
						&&(SKBATTR_ST_CN == sg_nSkbAttrSt))
					{
						cur_status = 0;

						InvalidateRect(hWnd, NULL, TRUE);						
					}
				}
				break;

			case SCANCODE_F12:
				{
					if (SKBATTR_ST_CN == sg_nSkbAttrSt)
					{
						if ((0 == cur_status)
							&&(SKBATTR_ST_CN == sg_nSkbAttrSt))
						{
							cur_status = 1;

							InvalidateRect(hWnd, NULL, TRUE);							
						}
					}
				}
				break;
				
	        case SCANCODE_F9: 			//向前翻页
	        	{
	        		  
		            switch(sg_nSkbAttrSt)  
		            {  
			            case SKBATTR_ST_CN: 
			            	if (0 == is_fh_input)
			            	{
				                if(cur_status == 0 && py_count > 0 && cur_py >0)  
				                    cur_py--;  
				                else if(cur_status == 1 && (strlen((char *)hz_Buf)/2) > perpagecount)  
				                {  
				                    if(start_point >= perpagecount)   
				                        start_point -= perpagecount;  
				                }  
				                else if(py_count == 0)  
				                {  
				                    if(sg_hTargetWnd)   
				                        PostMessage(sg_hTargetWnd, MSG_KEYDOWN, SCANCODE_CURSORBLOCKLEFT,lParam);  
				                } 
			                }
			                else 
			                {
				                if(sg_hTargetWnd && cur_status == 0)   
				                    PostMessage(sg_hTargetWnd, MSG_KEYDOWN, SCANCODE_CURSORBLOCKLEFT,lParam);  
				                else if(start_point >= perpagecount)   
				                   	start_point -= perpagecount; 
			                }
			                break;  

			            case SKBATTR_ST_EN:  
			                if(sg_hTargetWnd && cur_status == 0)   
			                    PostMessage(sg_hTargetWnd, MSG_KEYDOWN, SCANCODE_CURSORBLOCKLEFT,lParam);  
			                else if(start_point >= perpagecount)   
			                   	start_point -= perpagecount; 
			                   	
			                break;  
		            }                     

		            InvalidateRect(hWnd, NULL, TRUE);  
	            }
	            break;  

	        case SCANCODE_F10:         //向后翻页  
		        {
		            switch(sg_nSkbAttrSt)  
		            {  
		                case SKBATTR_ST_CN:  
		                	if (0 == is_fh_input)
		                	{
			                    if(cur_status == 0 && py_count >0 && cur_py < py_count-1)  
			                        cur_py++;  
			                    else if(cur_status == 1 && (strlen((char *)hz_Buf)/2) > perpagecount)  
			                    {  
			                        if((start_point + perpagecount) < strlen((char *)hz_Buf)/2)   
			                            start_point += perpagecount;  
			                    }  
			                    else if(py_count == 0)  
			                    {  
			                        if(sg_hTargetWnd)   
			                            PostMessage(sg_hTargetWnd, MSG_KEYDOWN, SCANCODE_CURSORBLOCKRIGHT,lParam);  
			                    }
		                    }
			                else 
			                {
			                    if(sg_hTargetWnd && cur_status == 0)   
			                        PostMessage(sg_hTargetWnd, MSG_KEYDOWN, SCANCODE_CURSORBLOCKRIGHT,lParam);  
		                        else if((start_point + perpagecount) < 38)   
		                            start_point += perpagecount;  	
			                }		                    
		                    break;  
		                  
		                case SKBATTR_ST_EN:  
		                    if(sg_hTargetWnd && cur_status == 0)   
		                        PostMessage(sg_hTargetWnd, MSG_KEYDOWN, SCANCODE_CURSORBLOCKRIGHT,lParam);  
	                        else if(((0 == is_fh_input)
	                           	 && ((start_point + perpagecount) < strlen((char *)en_Buf)))
	                           || ((1 == is_fh_input)
	                           	 && ((start_point + perpagecount) < 38)))
	                            start_point += perpagecount;  		                        
		                    break;  
		            } 
		            
		            InvalidateRect(hWnd, NULL, TRUE);
		        }
	            break;	         
	    }		
	}
	
	return 0;
}

static int OnImeKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (!IsImeOpened
		&& (SCANCODE_F6 != wParam))
	{
		return 0;
	}
	else if (SCANCODE_F6 == wParam)
	{
		ShowInputWnds(hWnd, 1);		//显示
		return 0;
	}
	
	if (SKBTYPE_T9 == sg_nSkbType)
	{
		if(SCANCODE_1 <= wParam && wParam <= SCANCODE_0)
		{
			PostMessage(GetDlgItem(sg_hSkbT9, wParam - SCANCODE_1 + IDC_T9SK1),
				SKBKM_PUSHUP, 0, 0);
		}
		else if(SCANCODE_F1 == wParam)
		{		
			PostMessage(GetDlgItem(sg_hSkbT9, IDC_T9CTRL), 
				SKBKM_PUSHUP, 0, 0);
		}
		else if(SCANCODE_F2 == wParam)
		{
			PostMessage(GetDlgItem(sg_hSkbT9, IDC_T9BACKSPACE), 
				SKBKM_PUSHUP, 0, 0);
		}
	    else if(SCANCODE_F7 == wParam)		//ESC键
	    {
			HideInputWnds();
	    }	

	}
	else
	{
	    if(SCANCODE_F7 == wParam)		//ESC键
	    {
			HideInputWnds();
	    }	
	}

    
	return 0;
}


//funcion process  
static void init_ime_parameter(void)  
{  
    memset(fh_Buf,0,50);  
    memcpy(fh_Buf,fh_table,strlen((char *)fh_table));  
  
    perpagecount = MAX_ROW_COUNTS;  
    cur_py = 0;  
    py_keycounts = 0;  
    py_count = 0;  
    start_point = 0;  
    cur_status = 0;         //初始输入状态   
}  
  
static void ClrInBuf(void)  
{  
	is_fh_input = 0;
    switch(sg_nSkbAttrSt)  
    {  
        case SKBATTR_ST_CN:  
            memset(hz_Buf,0,MAX_HZ_COUNT*2);  
            memset(py_key,0,MAX_PY_LENGTH+1);       //清除拼音输入  
            memset(py_Buf,0,MAX_PY_LENGTH+1); 
            memset(fh_Buf, 0, 50);
            py_keycounts = 0;  
            cur_status = 0;  
            cur_py = 0;  
            start_point = 0;  
            py_count = 0;  
            break;  
        case SKBATTR_ST_EN:  
            memset(en_Buf,0,MAX_KEY_CHAR);  
            memset(fh_Buf, 0, 50);
            cur_status = 0;  
            break;  
        case SKBATTR_ST_NUM:  
            //cur_status = 1;     //符号输入直接进入选择状态  
            break;  
    }  
  
}  
/* *********************lyx added 20130719****************** */
/* ****************该部分从开源上移植过来 20130719 ********* */
/* ********************************************************* */

static void RefreshIMEBox(HWND hWnd, HDC hDC)  
{  
    char str[100];  
    char sstr[100];  
    char tmppy[MAX_PY_LENGTH+1];        //当前拼音  
    char tmpstr[4];             //读取汉字  
  
    PLOGFONT logfont;  
    SIZE size;  
    int i, max_point = 0;  
    int pos_x, cWidth, sWidth;          //绘制选择框  
  
    SetBkColor(hDC,GetWindowElementColor(BKC_CONTROL_DEF));  
    memset(str,0,100);  
    if (1 == is_fh_input && SKBATTR_ST_NUM != sg_nSkbAttrSt)
    {
		//logfont = GetSystemFont(SYSLOGFONT_MENU);  
		//SelectFont (HDC_SCREEN, logfont);		   
    	if (SKBATTR_ST_CN == sg_nSkbAttrSt)
    		TextOut(hDC,2,2,"【CN】"); 
    	else /* if (SKBATTR_ST_EN == sg_nSkbAttrSt) */ 
    		TextOut(hDC,2,2,"【EN】"); 
    		
        GetTextExtent(HDC_SCREEN, str, strlen(str), &size);  
        memset(str,0,4*perpagecount+2);  

        if((start_point >= perpagecount) && (cur_status == 1)) 
        	sprintf(str,"%s","<");  
        else
        	sprintf(str,"%s"," ");  
          
        memset(sstr,0,4*perpagecount);  
        for(i=0;i<perpagecount;i++){  
            if((start_point+i)<32)  
                sprintf(sstr,"%s %d %c",sstr,i,fh_Buf[start_point+i]);  
            else{  
                if((fh_Buf[32+(start_point+i-32)*2] != '\0')||(fh_Buf[32+(start_point+i-32)*2+1] != '\0')){  
                    memset(tmpstr,0,4);  
                    tmpstr[0] = fh_Buf[32+(start_point+i-32)*2];  
                    tmpstr[1] = fh_Buf[32+(start_point+i-32)*2+1];  
                    sprintf(sstr,"%s %d %s",sstr,i,tmpstr);  
                }  
            }  
        }  

        if((start_point+perpagecount) < 38 && (cur_status == 1))  
            sprintf(str,"%s%s %s",str,sstr,">");  
        else  
            sprintf(str,"%s%s %s",str,sstr," ");  

             
        TextOut(hDC, 2, size.cy+2, str);  
    }
    else 
    {
    	switch(sg_nSkbAttrSt)  
	    {  
	        case SKBATTR_ST_CN:    
	            TextOut(hDC,2,2,"【CN】");  
	  
	            GetTextExtent(HDC_SCREEN, str, strlen(str), &size);  
	            cWidth = GetMaxFontWidth(hDC)/2;        //一个字节宽度  
	            pos_x = (strlen(str))*cWidth;  
	  
	            if(py_keycounts>0)  
	            {  
	                get_py_from_table();        //get py from table  
	                //if(cur_status == 0)  
	                {  
	                    for(i=0;i<py_count;i++)  
	                        sprintf(str,"%s%s ",str,py_Buf[i]);  
	                      
	                    for(i=0;i<cur_py;i++)  
	                        pos_x += (strlen(py_Buf[i])+1)*cWidth;  
	  
	                    sWidth = (strlen(py_Buf[cur_py]))*cWidth;  
	  
	                    SetBkMode(hDC,BM_TRANSPARENT);  
	                    SetBrushColor(hDC,COLOR_blue);  
	                    FillBox (hDC, pos_x+45 ,2 ,sWidth, size.cy);  
	                }  
	                //else if(cur_status == 1)  
	                //    sprintf(str,"%s%s",str,py_Buf[cur_py]);  
	  
	                TextOut(hDC, 45, 2, str);    //output first line  
	              
	                memset(tmppy,0,MAX_PY_LENGTH+1);  
	                strncpy(tmppy,py_Buf[cur_py],MAX_PY_LENGTH);  
	                get_hz_from_table(tmppy);   //get hz from table  
	  
	                memset(str,0,4*perpagecount+2);  
	                if((start_point >= perpagecount) && (cur_status == 1))  
	                    sprintf(str,"%s","<");  
	                else  
	                    sprintf(str,"%s"," ");  
	  
	                memset(sstr,0,4*perpagecount);  
	                for(i=0;i<perpagecount;i++)  
	                {  
	                    memset(tmpstr,0,4);  
	                    tmpstr[0] = hz_Buf[(start_point+i)*2];  
	                    tmpstr[1] = hz_Buf[(start_point+i)*2+1];  
	                    tmpstr[2] = '\0';  
	  
	                    if(tmpstr[0]!='\0' || tmpstr[1] != '\0')  
	                    {  
	                        //if(cur_status == 0) sprintf(sstr,"%s  %s",sstr,tmpstr);  
	                        /*else if(cur_status == 1) */sprintf(sstr,"%s%d.%s",sstr,i,tmpstr);  
	                    }  
	                }  
	  
	                if((((start_point+perpagecount)*2) < strlen((char *)hz_Buf)) && (cur_status == 1))  
	                    sprintf(str,"%s%s%s",str,sstr,">");  
	                else  
	                    sprintf(str,"%s%s%s",str,sstr," ");  
	  
	                //logfont = GetSystemFont(SYSLOGFONT_CONTROL);            //设置中文显示字体  
	                //SelectFont (HDC_SCREEN, logfont);  
	                TextOut(hDC, 2, size.cy+2, str);      
	  
	            }         
	            break;  
	  
	        case SKBATTR_ST_EN:         //英文输入显示  
	        	{
					
		            TextOut(hDC, 2, 2, "【EN】");  
		            memset(str,0,60);  
		          	if(en_Buf[0] == '\0')
		          		break;
		          		
		            GetTextExtent (HDC_SCREEN, en_Buf, strlen(en_Buf), &size);  

		            if ((start_point >= perpagecount) && (cur_status == 1))
		                sprintf(str,"%s","<");  
		            else  
		                sprintf(str,"%s"," "); 
		                
	            	max_point = ((start_point + perpagecount) >= strlen(en_Buf)) ? 
	            		(strlen(en_Buf) - start_point) : perpagecount;
		            for(i = 0; i < max_point; i++){  
		                if(cur_status == 0)  
		                    sprintf(str,"%s  %c  ",str,en_Buf[start_point + i]);  
		                else if(cur_status == 1)  
		                    sprintf(str,"%s%d %c  ",str,i,en_Buf[start_point + i]);  
		            } 
		            
					if (((start_point + perpagecount) >= strlen(en_Buf))
						&&(cur_status == 1))
						sprintf(str, "%s%s", str, ">");
					else
						sprintf(str, "%s%s", str, " ");	
						
		            TextOut(hDC,2+2,size.cy+2,str);  
	            	             
	  			}
	  			break; 
	  			
	        case SKBATTR_ST_NUM:         //符号输入  
	            TextOut(hDC, 2, 2, "【123】");            
	            break;            
	    } 
    }
}  
  
static void get_hz_from_table(char* py)  
{  
    int i;  
    BOOL matchflag = FALSE;  
      
    pt9idx = t9PY_ex_index;  
    memset(hz_Buf,0,MAX_HZ_COUNT*2);  
  
    while(pt9idx->PY[0] != '\0'){  
        matchflag = TRUE;  
        if(strlen(pt9idx->PY) == py_keycounts){  
            for(i=0;i<py_keycounts;i++){  
                if(py[i] != pt9idx->PY[i])   
                    matchflag = FALSE;  
            }  
        }  
        else  
            matchflag = FALSE;  
  
        if(matchflag){  
            i = 0;  
            while(pt9idx->MB[i] != '\0'){  
                hz_Buf[i] = pt9idx->MB[i];  
                i++;  
            }  
            break;  
        }  
        pt9idx++;  
    }  
}  
  
static void get_py_from_table(void)  
{  
    int i;  
    BOOL matchflag;  
    pt9idx = t9PY_ex_index;  
    py_count = 0;  
  
    for(i=0;i<MAX_PY_SAME;i++)  
        memset(py_Buf[i],0,MAX_PY_LENGTH+1);  
  
    while(pt9idx->T9[0] != '\0'){  
        matchflag = TRUE;  
        if(strlen(pt9idx->T9) == py_keycounts){  
            for(i=0;i<py_keycounts;i++){  
                if(py_key[i] != pt9idx->T9[i]) matchflag = FALSE;  
            }  
        }  
        else  
            matchflag = FALSE;  
      
        if(matchflag){  
            strcpy(py_Buf[py_count++],pt9idx->PY);  
        }  
  
        pt9idx++;  
    }  
}  
  
static BOOL py_key_filter(char key)  
{  
    char tmpkey[MAX_PY_LENGTH];  
    int i;  
    BOOL matchflag;  
    //BOOL newpyflag;                                         //输入键是否有效  
  
    pt9idx = t9PY_ex_index;  
  
    strcpy(tmpkey,py_key);                                  //记录已输入键序列  
      
    if(py_keycounts < MAX_PY_LENGTH){  
        sprintf(tmpkey,"%s%c",tmpkey,key);              //暂存新输入键  
        //newpyflag = FALSE;  
        while(pt9idx->T9[0] != '\0'){  
            matchflag = TRUE;  
            if(strlen(pt9idx->T9) == py_keycounts +1){  
                for(i=0;i<py_keycounts+1;i++){  
                    if(tmpkey[i] != pt9idx->T9[i]) matchflag = FALSE;  
                }  
            }  
            else  
                matchflag = FALSE;  
  
            if(matchflag) return TRUE;    
            pt9idx++;  
        }  
    }  
    return FALSE;  
  
}  
  
static void get_en_from_table(char key)  
{  
    T9EN_IDX* penidx;  
    penidx = (T9EN_IDX*)t9EN_ex_index;  
    int i = 0;  
  
    for(i=0;i<8;i++){  
        if(key == penidx->key[0]){  
            strcpy(en_Buf,penidx->Letter);  
            cur_status = 1;  
            break;  
        }  
        penidx++;  
    }  
}  
  
static void ime_writemsg(int sindex)  
{     
    WORD wDByte;  
    unsigned char cc [2];  

  	if (SKBATTR_ST_NUM != sg_nSkbAttrSt)
  	{
  		if(0 == is_fh_input)
  		{
  			//输出中文或者英文
		    switch(sg_nSkbAttrSt){  
		        case SKBATTR_ST_CN: /*将选择的汉字写入文本框*/  
		            if(sindex < perpagecount){  
		                cc[0] = hz_Buf[(start_point + sindex)*2];  
		                cc[1] = hz_Buf[(start_point + sindex)*2 + 1];  
		                wDByte = MAKEWORD(cc[0], cc[1]);  
		                if(sg_hTargetWnd && wDByte){  
		                    PostMessage(sg_hTargetWnd,MSG_CHAR,wDByte,0);  
		                    ClrInBuf();  
		                }  
		            }  
		            break;  
		                  
		        case SKBATTR_ST_EN: /*将选择的英文字母写入文本框*/
		        	if(sg_hTargetWnd
		        	   && (sindex < perpagecount)
		        	   && (en_Buf[sindex] != '\0'))
		            {  
		                SendMessage(sg_hTargetWnd,MSG_CHAR,
		                	en_Buf[start_point + sindex],0);  
		                ClrInBuf();  
		            }  
		            break;  
		    }
	    }
	    else 
	    {
			//输出标点符号
            if(sindex < perpagecount){  
                if((start_point + sindex)<32){  
                    if(sg_hTargetWnd)  
                        SendMessage(sg_hTargetWnd,MSG_CHAR,fh_Buf[start_point + sindex],0);  
                }  
                else{  
                    cc[0] = fh_Buf[32+(start_point+sindex-32)*2];  
                    cc[1] = fh_Buf[32+(start_point+sindex-32)*2+1];  
                    wDByte = MAKEWORD(cc[0],cc[1]);  
                    if (sg_hTargetWnd) PostMessage (sg_hTargetWnd, MSG_CHAR, wDByte, 0);  
                }  

                ClrInBuf();
            }  
        }
    }
                          
}  
  
/*删除最近一个拼音字母*/  
static void delete_last_py(void)  
{  
    char tmpkey[MAX_PY_LENGTH];  
    memset(tmpkey,0,MAX_PY_LENGTH);  
    memcpy(tmpkey,py_key,py_keycounts-1);  
    memset(py_key,0,MAX_PY_LENGTH);  
    memcpy(py_key,tmpkey,py_keycounts--);  
  
    cur_py = 0;  
    start_point = 0;  
  
}  
  
static void InputKeyProcess(unsigned char key, LPARAM lParam)  
{  
    const char ckey[] = "0123456789";  
  
    char skey = 0;  
    int sel_index = 0;  
  
    skey = (char)ckey[key - '0'];  
    sel_index = (int)(key - '0');  

  	if (SKBATTR_ST_NUM == sg_nSkbAttrSt)
  	{
		outchar_ex(key, lParam);
  	}
  	else
  	{
	    switch(cur_status){  
	        case 0:     //键盘输入状态(只有中/英文输入才进入此状态)  
	            if(key >= '2')
	            {  
	                switch(sg_nSkbAttrSt){  
	                    //中文输入  
	                    case SKBATTR_ST_CN:  
	                        if(py_key_filter(skey)){  
	                            sprintf(py_key,"%s%c",py_key,skey);     //保存输入键  
	                            cur_py=0;  
	                            start_point=0;  
	                            py_keycounts++;  
	                        }  
	                        break;  
	                    //英文输入  
	                    case SKBATTR_ST_EN:  
	                        get_en_from_table(skey);
	                        break;  
	                }  
	            }  
	          	else if(key == '0')  
	          	{
	              if(sg_hTargetWnd) PostMessage(sg_hTargetWnd,MSG_KEYDOWN,SCANCODE_SPACE,0);  
	  			}
	  			else if(key == '1')
	  			{
				    memset(fh_Buf,0,50);  
				    memcpy(fh_Buf,fh_table,strlen((char *)fh_table));  
	  				is_fh_input = 1;
					cur_status = 1;
	  			}
	            break;  
	        case 1:                 //选择状态  
	            ime_writemsg(sel_index);  
	            break;  

	    }
    }  
} 
/* ********************************************************* */
/* *********************lyx added ended ******************** */
/* ********************************************************* */

