/*
 * 版权所有：2011 DVR 有限公司
 * 文 件 名：myedit.c
 * 摘    要：其实是参照魏老板的edit.c 进行小小的修改
 *                     目的就是研究下它的实现过程。
 * 创建日期：2011-07-28
 * 作    者: jason
 * 维护者:
 * 姓    名:
 * 日    期:
 * 修改内容描述:
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <minigui/common.h>

#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

/**********下面这几个头文件都是手动添加的******************/
#include <minigui/cliprect.h>
#include <minigui/internals.h>
#include <minigui/ctrlclass.h>
#include "myedit.h"
#include "ctrl_data.h"
#include "gui_core.h"
#ifdef _MY_EDIT_H

#define PIXEL_EDIT_NORAML			0xFFF3F3F3
#define PIXEL_EDIT_HILIGHT			0xFFFFFFFF
#define PIXEL_EDIT_DISABLE			0xFFBEBEBE

#define PIXEL_FONT_NORAML			0xFF000000
#define PIXEL_FONT_HILIGHT			PIXEL_black
#define PIXEL_FONT_DISABLE			PIXEL_darkblue


extern BOOL IsCustomImeOpen();
static void SetLineWidth (HWND hWnd, PEDITDATA pedit);
static void mySetCaretPos (HWND hWnd, PEDITDATA pedit, int x);
static int EditCtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
static void EditRefreshCaret (HWND hWnd, PEDITDATA pedit, BOOL bInited);

#define check_caret() \
            if(pedit->selStart != pedit->selEnd) \
                HideCaret(hWnd); \
            else \
                ShowCaret(hWnd);


#define shift_one_char_r(pos) \
        { \
            int len = CHLENNEXT((pedit->content.string + pedit->pos), (pedit->content.txtlen - pedit->pos)); \
            pedit->pos += len; \
        }

#define shift_one_char_l(pos) \
        { \
            int len = CHLENPREV((const char* )(pedit->content.string),(const char* )(pedit->content.string + pedit->pos) ); \
            pedit->pos -= len; \
        }


BOOL RegisterMyEditControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_MYEDIT;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_IBEAM);
    WndClass.iBkColor    = PIXEL_EDIT_HILIGHT;//GetWindowElementColor (WEC_3DBOX_NORMAL);
    WndClass.WinProc     = EditCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

void UnregisterMyEditControl (void)
{

    UnregisterWindowClass (CTRL_MYEDIT);
    return;
}

/*********************************************************/
//modify by luo ping 2010年5月31日  /** mark***/
/***************************************************************/
static gal_pixel g_color = 65535;
static gal_pixel color_temp = 65535;

void Set_SLEditControlColor(gal_pixel new_value)
{
    g_color = new_value;
}


void Restore_SLEditControlColor(void)
{
    g_color = color_temp;
}


gal_pixel Get_SLEditControlColor(void)
{
    return g_color;
}
/********************************************************/
/* -------------------------------------------------------------------------- */
//设置颜色之前一定要记住 旧有的颜色，用完之后恢复
static void SetupDC (HWND hWnd, EDITDATA *pedit, HDC hdc, BOOL bSel)
{
    DWORD dwStyle = GetWindowStyle (hWnd);
    int isFocus = -1;
	RECT rectEdit;
    GetClientRect (hWnd, &rectEdit);

    isFocus = (pedit->status&EST_FOCUSED)? 1 : 0;
	if (dwStyle & WS_DISABLED)
	{
		SetTextColor (hdc, GetWindowElementColorEx (hWnd, FGC_CONTROL_DISABLED));
		SetBkColor (hdc, PIXEL_EDIT_DISABLE);
		SetBrushColor(hdc,PIXEL_EDIT_DISABLE);
	#ifdef _ROUND_RECT_STYLE
		RoundRect (hdc, rectEdit.left,rectEdit.top, RECTW(rectEdit) - 1,RECTH(rectEdit) - 1, 3, 3);
	#else
		//FillBox(hdc,rectEdit.left,rectEdit.top, RECTW(rectEdit),RECTH(rectEdit));
	#endif
        SetBkMode (hdc, BM_TRANSPARENT);		
	}
	else
	{
		if (!bSel)
		{
			if(isFocus)
			{				
				
			    SetBkColor (hdc, PIXEL_EDIT_HILIGHT);
			    SetBrushColor(hdc,PIXEL_EDIT_HILIGHT);	
			#ifdef _ROUND_RECT_STYLE
				RoundRect (hdc, rectEdit.left,rectEdit.top, RECTW(rectEdit) - 1,RECTH(rectEdit) - 1, 3, 3);
			#else
				//FillBox(hdc,rectEdit.left,rectEdit.top, RECTW(rectEdit),RECTH(rectEdit));
			#endif
			   SetTextColor (hdc,pedit->color_ft_hilight); 
			}
            else
            {					
			    SetBkColor (hdc, PIXEL_EDIT_NORAML);
				SetBrushColor(hdc,PIXEL_EDIT_NORAML);
			#ifdef _ROUND_RECT_STYLE
				RoundRect (hdc, rectEdit.left,rectEdit.top, RECTW(rectEdit) - 1,RECTH(rectEdit) - 1, 3, 3);
			#else
				//FillBox(hdc,rectEdit.left,rectEdit.top, RECTW(rectEdit),RECTH(rectEdit));
			#endif
                SetTextColor (hdc,pedit->color_ft);
            }
			SetBkMode (hdc, BM_TRANSPARENT);
		}
		else
		{			
			if (dwStyle & WS_DISABLED)
            	SetTextColor (hdc, GetWindowElementColorEx (hWnd, FGC_HILIGHT_DISABLED));
	        else
	            SetTextColor (hdc, GetWindowElementColorEx (hWnd, FGC_HILIGHT_NORMAL));
            SetBkColor (hdc, GetWindowElementColorEx (hWnd, BKC_HILIGHT_LOSTFOCUS));
	        SetBkMode (hdc, BM_OPAQUE);
		}
	}
	
}

/* -------------------------------------------------------------------------- */

static int EditSetText (PEDITDATA pedit, const char *newtext)
{
    int len, txtlen;

    txtlen = strlen (newtext);
    len = (txtlen <= 0) ? pedit->nBlockSize : txtlen;

    if (pedit->hardLimit >= 0 && txtlen > pedit->hardLimit)
    {
        return -1;
    }

    /* free the old text */
    if (pedit->content.string)
        testr_free (&pedit->content);

    if (!testr_alloc (&pedit->content, len, pedit->nBlockSize))
        return -1;

    if (newtext && txtlen > 0)
        testr_setstr (&pedit->content, newtext, txtlen);
    else
        pedit->content.txtlen = 0;

    return 0;
}

static void GetContentWidth (HWND hWnd, PEDITDATA pedit)
{
    GetClientRect (hWnd, &pedit->rcVis);//获取客户区

    pedit->rcVis.left += pedit->leftMargin;
    pedit->rcVis.top += pedit->topMargin;
    pedit->rcVis.right -= pedit->rightMargin;
    pedit->rcVis.bottom -= pedit->bottomMargin;

    pedit->nVisW = pedit->rcVis.right - pedit->rcVis.left;
}
//重新计算编辑区的大小 刷新插入符的位置
static void RecalcSize (HWND hWnd, PEDITDATA pedit, BOOL bInited)
{
    GetContentWidth (hWnd, pedit); //获取可编辑区矩形

    pedit->nContX = 0;
    pedit->nContW = pedit->rcVis.right - pedit->rcVis.left; //字符编辑区的宽度


    SetLineWidth (hWnd, pedit);

    pedit->starty  = pedit->topMargin + ( pedit->rcVis.bottom - pedit->rcVis.top - GetWindowFont (hWnd)->size -1) / 2;

    EditRefreshCaret (hWnd, pedit, bInited);
}

static int EditInit (HWND hWnd, PEDITDATA pedit)
{
    PCONTROL pCtrl;

    if (!pedit)
        return -1;

    pCtrl = Control(hWnd);

    pCtrl->dwAddData2 = (DWORD)pedit;

    pedit->status = 0;
    pedit->editPos = 0;
    pedit->selStart = 0;
    pedit->selEnd = 0;

    pedit->leftMargin     = 5;//MARGIN_EDIT_LEFT;
    pedit->topMargin      = MARGIN_EDIT_TOP;
    pedit->rightMargin    = MARGIN_EDIT_RIGHT;
    pedit->bottomMargin   = MARGIN_EDIT_BOTTOM;



    pedit->color_bg = PIXEL_EDIT_NORAML;
    pedit->color_bg_hilight = PIXEL_EDIT_HILIGHT;

    pedit->color_boder  = PIXEL_lightgray;
    pedit->color_boder_hilight =  PIXEL_lightgray;

    pedit->color_ft = PIXEL_FONT_NORAML;
    pedit->color_ft_hilight = PIXEL_FONT_HILIGHT;
    pedit->passwdChar     = '*';

    pedit->changed        = FALSE;

    pedit->nBlockSize = DEF_LINE_BLOCK_SIZE;
    pedit->hardLimit      = -1;

    if (pCtrl->dwStyle & ES_TIP)
    {
        pedit->tiptext = FixStrAlloc (DEF_TIP_LEN + 1);//魏提示信息申请内存
        pedit->tiptext[0] = 0;
    }
    else
        pedit->tiptext = NULL;

    pedit->content.string = NULL;
    pedit->content.buffsize = 0;
    pedit->content.txtlen = 0;

    EditSetText (pedit, pCtrl->spCaption);//控件标题需要的内存及初始化
    if (pCtrl->dwStyle & ES_PASSWORD)
    {
        pCtrl->pLogFont = GetWindowFont(hWnd);
    }

    CreateCaret (hWnd, NULL, 1, pCtrl->pLogFont->size);//创建插入符
    RecalcSize (hWnd, pedit, TRUE);

    return 0;
}

static void editit_destroy (HWND hWnd, PEDITDATA pedit)
{
    DestroyCaret (hWnd);
    if ( (GetWindowStyle(hWnd) & ES_TIP) && pedit->tiptext)
        FreeFixStr (pedit->tiptext);
    testr_free (&pedit->content);
}

static inline BOOL sleContentToVisible (PEDITDATA pedit, int *x)
{
    if (x)
        *x -= pedit->nContX;
    else
        return -1;

    if (*x < 0 || *x > pedit->nVisW)
        return -1;
    return 0;
}

static inline BOOL sleVisibleToWindow (PEDITDATA pedit, int *x)
{
    if (x)
        *x += pedit->leftMargin;//边框空白
    return TRUE;
}

static inline void sleContentToWindow (PEDITDATA pedit, int *x)
{
    sleContentToVisible (pedit, x);
    sleVisibleToWindow (pedit, x);
}

static inline void sleWindowToContent (PEDITDATA pedit, int *x)
{
    *x -= pedit->leftMargin;
    *x += pedit->nContX;
}

/* sets the current caret position in the virtual content window */
static void mySetCaretPos (HWND hWnd, PEDITDATA pedit, int x)
{
    if (sleContentToVisible(pedit, &x) < 0)
    {
        HideCaret (hWnd);
    }
    else
    {
        sleVisibleToWindow (pedit, &x);
        SetCaretPos (hWnd, x, pedit->starty);
        if (pedit->status & EST_FOCUSED)
        {
            ActiveCaret (hWnd);
            ShowCaret (hWnd);
        }
    }
}

static int get_caretpos_x(HWND hWnd)
{
    PMAINWIN pWin;

    pWin = (PMAINWIN)hWnd;

    if (!pWin->pCaretInfo)
        return FALSE;

    return pWin->pCaretInfo->x;
}

//开始绘制编辑框  刷新的时候也是调用这个

static void EditPaint (HWND hWnd, HDC hdc, PEDITDATA pedit)
{
	char*   dispBuffer, *passwdBuffer = NULL;
    DWORD   dwStyle = GetWindowStyle(hWnd);
    StrBuffer *content = &pedit->content;
    int starty = pedit->starty;
    int outw = 0;
    int startx;
	UINT uFormat = 0;

		
	SetupDC (hWnd, pedit, hdc, FALSE);
	uFormat |= DT_LEFT| DT_VCENTER|DT_SINGLELINE;
    if (dwStyle & ES_TIP && content->txtlen <= 0 &&
            GetFocus(GetParent(hWnd)) != hWnd)
    {

        TextOut (hdc, pedit->leftMargin, starty,
                 pedit->tiptext);
        return;
    }

    if (dwStyle & ES_PASSWORD)
    {
        dispBuffer = FixStrAlloc (content->txtlen);//为密码
        memset (dispBuffer, pedit->passwdChar, content->txtlen);
        passwdBuffer = dispBuffer;
    }
    else
    {

        dispBuffer = (char*)content->string;
    }


	//将设备上下文的可见区域设置为已有区域和给
	//定矩形区域的交集；
    ClipRectIntersect (hdc, &pedit->rcVis);

    if (pedit->selStart != pedit->selEnd)  //select chars 表示有字符被选择
    {
        startx = pedit->startx;
        sleContentToWindow (pedit, &startx);
        /* draw first normal chars  绘制没有被选中的文字*/
        if (pedit->selStart > 0)
        {
            //SetupDC (hWnd, pedit, hdc, FALSE);//设置控件的各种颜色
            outw += TextOutLen (hdc, startx, starty,
                                dispBuffer, pedit->selStart);
            dispBuffer += pedit->selStart;
        }

        /* draw selected chars  绘制选中的文字*/
        SetupDC (hWnd, pedit, hdc, TRUE);
        outw += TextOutLen (hdc, startx + outw, starty, dispBuffer,
                            pedit->selEnd - pedit->selStart);//显示并计算剩余宽度
        dispBuffer += pedit->selEnd - pedit->selStart;

        /* draw others */
        if (pedit->selEnd < content->txtlen)
        {
            //SetupDC (hWnd, pedit, hdc, FALSE);
            outw += TextOutLen (hdc, startx + outw, starty, dispBuffer, content->txtlen - pedit->selEnd);
        }
    }
    else
    {
        //SetupDC (hWnd, pedit, hdc, FALSE);
        startx = pedit->startx;
        sleContentToWindow (pedit, &startx);
        outw += TextOutLen (hdc, startx, starty, dispBuffer, content->txtlen);
    }

    if (dwStyle & ES_PASSWORD)
        FreeFixStr (passwdBuffer);
}


static int sleSetSel (HWND hWnd, PEDITDATA pedit, int sel_start, int sel_end)
{
    if (pedit->content.txtlen <= 0)
        return -1;

    if (sel_start < 0)
        sel_start = 0;
    if (sel_end < 0)
        sel_end = pedit->content.txtlen;
    if (sel_start == sel_end)
        return -1;

    pedit->selStart = sel_start;
    pedit->selEnd = sel_end;
    HideCaret(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);

    return pedit->selEnd - pedit->selStart;
}

/*
 * set caret position according to the new desired x coordinate.
 */
static void set_caret_pos (HWND hWnd, PEDITDATA pedit, int x, BOOL bSel)
{
    int out_chars;
    HDC hdc;
    SIZE txtsize;

    hdc = GetClientDC (hWnd);

    sleWindowToContent (pedit, &x);

    if (x - pedit->startx <= 0)
    {
        out_chars = 0;
        txtsize.cx = 0;
    }
    else
    {
        out_chars = GetTextExtentPoint (hdc, (const char*)pedit->content.string,
                                        pedit->content.txtlen,
                                        x - pedit->startx,
                                        NULL, NULL, NULL, &txtsize);
    }
    if (!bSel)
    {
        pedit->selStart = pedit->selEnd = 0;
        pedit->editPos = out_chars;
        mySetCaretPos (hWnd, pedit, txtsize.cx + pedit->startx);
    }
    else
    {
        if (out_chars > pedit->editPos)
        {
            pedit->selStart = pedit->editPos;
            pedit->selEnd = out_chars;
        }
        else
        {
            pedit->selEnd = pedit->editPos;
            pedit->selStart = out_chars;
        }
    }

    ReleaseDC (hdc);
}

static BOOL make_pos_visible (HWND hWnd, PEDITDATA pedit, int x)
{
    if (x - pedit->nContX > pedit->nVisW)
    {
        pedit->nContX = x - pedit->nVisW;
        return TRUE;
    }
    else if (x < pedit->nContX)
    {
        pedit->nContX = x;
        return TRUE;
    }
    return FALSE;
}

static BOOL make_charpos_visible (HWND hWnd, PEDITDATA pedit, int charPos, int *cx)
{
    SIZE txtsize;
    HDC hdc;

    if (charPos <= 0)
        txtsize.cx = 0;
    else
    {
        hdc = GetClientDC (hWnd);
        GetTextExtent (hdc, (const char*)pedit->content.string, charPos, &txtsize);
        ReleaseDC (hdc);
    }
    if (cx)
        *cx = pedit->startx + txtsize.cx;  //计算光标的位置
    return make_pos_visible (hWnd, pedit, pedit->startx + txtsize.cx);
}

static void calc_content_width(HWND hWnd, PEDITDATA pedit, int charPos, int *cx)
{
    SIZE txtsize;
    HDC hdc;

    if (charPos <= 0)
        txtsize.cx = 0;
    else
    {
        hdc = GetClientDC (hWnd);
        GetTextExtent (hdc, (const char*)pedit->content.string, charPos, &txtsize);
        ReleaseDC (hdc);
    }
    if (cx)
        *cx = txtsize.cx;

}

static void calc_charpos_cx(HWND hWnd, PEDITDATA pedit, int charPos, int *cx)
{
    SIZE txtsize;
    HDC hdc;

    if (charPos <= 0)
        txtsize.cx = 0;
    else
    {
        hdc = GetClientDC (hWnd);
        GetTextExtent (hdc, (const char*)pedit->content.string, charPos, &txtsize);
        ReleaseDC (hdc);
    }
    if (cx)
        *cx = pedit->startx + txtsize.cx;

}

/*
 * set caret position according to the current edit position.
 */
static BOOL edtSetCaretPos (HWND hWnd, PEDITDATA pedit)
{
    BOOL bRefresh;
    int cx;
    bRefresh = make_charpos_visible (hWnd, pedit, pedit->editPos, &cx);
    mySetCaretPos (hWnd, pedit, cx);
    if (bRefresh)
        InvalidateRect (hWnd, NULL, TRUE);

    return bRefresh;
}
static void set_edit_caret_pos(HWND hWnd, PEDITDATA pedit)
{
    int cx;

    make_charpos_visible (hWnd, pedit, pedit->editPos, &cx);
    mySetCaretPos (hWnd, pedit, cx);
}

static int sleMouseMove (HWND hWnd, PEDITDATA pedit, LPARAM lParam)
{
    int mouseX, mouseY;
    RECT rcClient;
    BOOL refresh = TRUE;

    mouseX = LOSWORD(lParam);
    mouseY = HISWORD(lParam);

    ScreenToClient(hWnd, &mouseX, &mouseY);
    GetClientRect(hWnd, &rcClient);

    if(mouseX >= 0 && mouseX < rcClient.right
            && mouseY >= 0 && mouseY < rcClient.bottom)  //in edit window
    {
        set_caret_pos (hWnd, pedit, mouseX, TRUE);
        if (pedit->editPos == pedit->selStart)
            make_charpos_visible (hWnd, pedit, pedit->selEnd, NULL);
        else
            make_charpos_visible (hWnd, pedit, pedit->selStart, NULL);
    }
    else if (mouseY < 0)
    {
        pedit->nContX = 0;
        pedit->selStart = 0;
        pedit->selEnd = pedit->editPos;
    }
    else if(mouseY >= rcClient.bottom)
    {
        pedit->selStart = pedit->editPos;
        pedit->selEnd = pedit->content.txtlen;
        pedit->nContX = pedit->nContW - (pedit->rcVis.right - pedit->rcVis.left);
    }
    else if (mouseX < 0)
    {
        if (pedit->selEnd == pedit->content.txtlen)
        {
            set_caret_pos (hWnd, pedit, 0, TRUE);
            make_charpos_visible (hWnd, pedit, pedit->selStart, NULL);
            goto quit;
        }

        if (pedit->selStart == 0)
            return 0;

        shift_one_char_l (selStart);
        refresh = make_charpos_visible (hWnd, pedit, pedit->selStart, NULL);
    }
    else if (mouseX >= rcClient.right)
    {
        if (pedit->selStart == 0)
        {
            set_caret_pos (hWnd, pedit, rcClient.right - 1, TRUE);
            //make_charpos_visible (hWnd, pedit, pedit->selEnd, NULL);
            //goto quit;
        }

        if (pedit->selEnd == pedit->content.txtlen)
            return 0;

        shift_one_char_r (selEnd);
        refresh = make_charpos_visible (hWnd, pedit, pedit->selEnd, NULL);
    }

quit:
    check_caret();

    if (refresh)
        InvalidateRect(hWnd, NULL, TRUE);

    return 0;
}

static void SetLineWidth (HWND hWnd, PEDITDATA pedit)
{
    SIZE txtsize;
    HDC hdc;
    DWORD dwStyle = GetWindowStyle(hWnd);
    int old_w = pedit->nContW;

    hdc = GetClientDC (hWnd);
    SelectFont (hdc, GetWindowFont(hWnd));

    GetTextExtent(hdc, (const char*)(pedit->content.string), pedit->content.txtlen, &txtsize);//计算文本的大小
    ReleaseDC (hdc);

    if (txtsize.cx > pedit->nVisW)
        pedit->nContW = txtsize.cx;
    else
        pedit->nContW = pedit->nVisW;

    if (dwStyle & ES_RIGHT)//靠右
    {
        pedit->nContX += (pedit->nContW - old_w);
    }
    else if (dwStyle & ES_CENTER)//居中
    {
        pedit->nContX += (pedit->nContW - old_w) >> 1;
    }

    if (pedit->nContX + pedit->nVisW > pedit->nContW)
    {
        pedit->nContX = pedit->nContW - pedit->nVisW;
    }
    else if (pedit->nContX < 0)
        pedit->nContX = 0;

    if (pedit->nContW > pedit->nVisW)
        pedit->startx = 0;
    else if (dwStyle & ES_RIGHT)
    {
        pedit->startx = pedit->nVisW - txtsize.cx;
    }
    else if (dwStyle & ES_CENTER)
    {
        pedit->startx = (pedit->nVisW - txtsize.cx) >> 1;
    }
    else
    {
        pedit->startx = 0;
    }
}

static inline void edtChangeCont (HWND hWnd, PEDITDATA edit)
{
    SetLineWidth (hWnd, edit);
}

/* check input validity for password style */
static void check_valid_passwd (char *newtext)
{
    char *ptmp = newtext;

    while (*ptmp)
    {
        if (!isprint(*ptmp))
            *ptmp = '*';
        ptmp ++;
    }
}

static int  sleInsertText (HWND hWnd, PEDITDATA pedit, char *newtext, int inserting)
{
    int  deleted;
    unsigned char *pIns, *content = pedit->content.string;
    int input_num = -1;
    char buf[8] =  {'\0','\0','\0','\0','\0','\0','\0','\0'};
    if ( (GetWindowStyle(hWnd) & ES_PASSWORD) && newtext && inserting > 0 )
    {
        check_valid_passwd (newtext);
    }
	//delete the seleted
    if(pedit->selStart != pedit->selEnd)  //删除一位
    {
        deleted = pedit->selEnd - pedit->selStart;
        memmove (content + pedit->selStart, content + pedit->selEnd,
                 pedit->content.txtlen - pedit->selEnd);
        pedit->content.txtlen -= deleted;
	pedit->content.string[pedit->content.txtlen] = '\0';
        pedit->editPos = pedit->selStart;
        pedit->selEnd = pedit->selStart;
        ShowCaret(hWnd);

        if (!newtext)
            inserting = 0;
    }

    if ( !(pedit->status & EST_REPLACE) && inserting > 0 && pedit->hardLimit >= 0 &&
            ((pedit->content.txtlen + inserting) > pedit->hardLimit))
    {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
        inserting = pedit->hardLimit - pedit->content.txtlen;
        return 0;
    }
    else if ( (pedit->status & EST_REPLACE) && inserting > 0 && pedit->hardLimit >= 0
              && (pedit->editPos + inserting > pedit->hardLimit) )
    {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
        inserting = pedit->hardLimit - pedit->editPos;

    }

    if (inserting > 0)
    {
        if(GetWindowStyle(hWnd) & ES_INPUT_NUM)
        {
            input_num = atoi((char *)pedit->content.string);
        }
		///pedit->content.txtlen
        if((0 == input_num)&&( pedit->content.txtlen == 1))
        {
            memcpy (pedit->content.string, newtext, inserting);
           //// inserting  -= 1;
           inserting =0;
	     pedit->content.txtlen  = 1;
        }
        else
        {
            content = testr_realloc (&pedit->content, pedit->content.txtlen +
                                     inserting);
            pIns = content + pedit->editPos;
            if ( !(pedit->status & EST_REPLACE) )
                memmove (pIns + inserting, pIns, pedit->content.txtlen+1 - pedit->editPos);
            memcpy (pIns, newtext, inserting);
        }
    }
    else if (inserting < 0)
    {
        pIns = content + pedit->editPos;
        memmove (pIns + inserting, pIns, pedit->content.txtlen+1 - pedit->editPos);
        content = testr_realloc (&pedit->content, pedit->content.txtlen +
                                 inserting);
    }

    if (inserting <= 0 || !(pedit->status & EST_REPLACE))
        pedit->content.txtlen += inserting;
    else
    {
        int add_len = inserting - pedit->content.txtlen + pedit->editPos;
        if (add_len > 0)
        {
            pedit->content.txtlen += add_len;
            pedit->content.string[pedit->content.txtlen] = '\0';
        }
    }

	pedit->editPos += inserting;
	pedit->selStart = pedit->selEnd = pedit->editPos;
	if(GetWindowStyle(hWnd) & ES_INPUT_NUM)
    {
    	  int num = 0, max_num = (int)GetWindowAdditionalData (hWnd);
        input_num = atoi((char *)pedit->content.string);

        if(input_num > max_num)
        {
            sprintf(buf,"%d", max_num);
            pedit->content.txtlen = strlen(buf);
            memcpy(pedit->content.string,buf,pedit->content.txtlen);
            pedit->content.string[pedit->content.txtlen] = '\0';
        }

		while (max_num)
		{
			num++;
			max_num  /= 10;
		}

		if (pedit->selStart > num)
		{
			pedit->editPos -= inserting;
			pedit->selStart = pedit->selEnd = pedit->editPos;
		}
    }
//FIXME
    edtChangeCont (hWnd, pedit);
  /////////  InvalidateRect (hWnd, NULL, TRUE);

    if (!edtSetCaretPos (hWnd, pedit))
    {
        //InvalidateRect (hWnd, NULL, TRUE);
    }
	    InvalidateRect (hWnd, NULL, TRUE);

    pedit->changed = TRUE;
    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);

    return 0;
}


//  插入字符
static int sleInsertText_refresh (HWND hWnd, PEDITDATA pedit, char *newtext, int inserting)
{
    int  deleted;
    unsigned char *pIns, *content = pedit->content.string;

    if ( (GetWindowStyle(hWnd) & ES_PASSWORD) && newtext && inserting > 0 )
    {
        check_valid_passwd (newtext);
    }

    //delete the seleted
    if(pedit->selStart != pedit->selEnd)
    {
        deleted = pedit->selEnd - pedit->selStart;
        memmove (content + pedit->selStart, content + pedit->selEnd,
                 pedit->content.txtlen - pedit->selEnd);
        pedit->content.txtlen -= deleted;

        pedit->editPos = pedit->selStart;
        pedit->selEnd = pedit->selStart;
        ShowCaret(hWnd);
        if (!newtext)  // 空字符情况
            inserting = 0;
    }

    if ( !(pedit->status & EST_REPLACE) && inserting > 0 && pedit->hardLimit >= 0 &&
            ((pedit->content.txtlen + inserting) > pedit->hardLimit))
    {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);//  超出限定长度
        inserting = pedit->hardLimit - pedit->content.txtlen;
        //return 0;
    }
    else if ( (pedit->status & EST_REPLACE) && inserting > 0 && pedit->hardLimit >= 0
              && (pedit->editPos + inserting > pedit->hardLimit) )
    {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
        inserting = pedit->hardLimit - pedit->editPos;
    }

    if (inserting > 0)
    {
        content = testr_realloc (&pedit->content, pedit->content.txtlen +
                                 inserting);
        pIns = content + pedit->editPos;
        if ( !(pedit->status & EST_REPLACE) )
            memmove (pIns + inserting, pIns, pedit->content.txtlen+1 - pedit->editPos);
        memcpy (pIns, newtext, inserting);
    }
    else if (inserting < 0)
    {
        pIns = content + pedit->editPos;
        memmove (pIns + inserting, pIns, pedit->content.txtlen+1 - pedit->editPos);
        content = testr_realloc (&pedit->content, pedit->content.txtlen +
                                 inserting);
    }

    if (inserting <= 0 || !(pedit->status & EST_REPLACE))
        pedit->content.txtlen += inserting;
    else
    {
        int add_len = inserting - pedit->content.txtlen + pedit->editPos;
        if (add_len > 0)
        {
            pedit->content.txtlen += add_len;
            pedit->content.string[pedit->content.txtlen] = '\0';
        }
    }
    pedit->editPos += inserting;
    pedit->selStart = pedit->selEnd = pedit->editPos;

//FIXME
    edtChangeCont (hWnd, pedit);

    return 0;
}

#ifdef _CLIPBOARD_SUPPORT
static int sleInsertCbText (HWND hWnd, PEDITDATA pedit)
{

    int  inserting;
    unsigned char *txtBuffer;

    if (GetWindowStyle(hWnd) & ES_READONLY)
    {
        return 0;
    }

    inserting = GetClipBoardDataLen (CBNAME_TEXT);
    txtBuffer = ALLOCATE_LOCAL (inserting);
    GetClipBoardData (CBNAME_TEXT, txtBuffer, inserting);
    sleInsertText (hWnd, pedit, (char *)txtBuffer, inserting);
    DEALLOCATE_LOCAL(txtBuffer);

    return 0;
}
#endif

static void esright_backspace_refresh(HWND hWnd, PEDITDATA pedit, int del)
{
    RECT scroll_rc, refresh_rc;
    int sel_start_x, sel_end_x;
    int old_caret_x, cur_caret_x;
    int scroll_len;
    int old_peditpos_x, cur_peditpos_x;

    scroll_rc.top = pedit->rcVis.top;
    scroll_rc.bottom = pedit->rcVis.bottom;

    if (pedit->nContW <= pedit->nVisW)
    {
        if (pedit->selStart != pedit->selEnd)
        {

            calc_charpos_cx(hWnd, pedit, pedit->selStart, &sel_start_x);
            calc_charpos_cx(hWnd, pedit, pedit->selEnd, &sel_end_x);
            if (pedit->selStart < pedit->selEnd)
            {
                scroll_rc.right = sel_end_x + 1;
            }
            else
            {
                scroll_rc.right = sel_start_x + 1;
            }

            scroll_rc.left = pedit->rcVis.left;
            sleInsertText_refresh (hWnd, pedit, NULL, del);

            scroll_len = abs(sel_start_x - sel_end_x);
            ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
            edtSetCaretPos (hWnd, pedit);
        }
        else
        {
            calc_charpos_cx(hWnd, pedit, pedit->editPos, &old_peditpos_x);
            scroll_rc.left = pedit->rcVis.left;
            calc_charpos_cx(hWnd, pedit, pedit->editPos + del, &cur_peditpos_x);

            sleInsertText_refresh (hWnd, pedit, NULL, del);

            edtSetCaretPos (hWnd, pedit);
            scroll_rc.right = old_peditpos_x + 1;

            scroll_len = abs(cur_peditpos_x - old_peditpos_x);
            ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

        }

    }
    else
    {

        if (pedit->nContX <= 0) //left scroll window while the head of text is in the pedit->rcVis
        {
            if (pedit->selStart != pedit->selEnd)
            {

                refresh_rc.top = pedit->rcVis.top;
                refresh_rc.bottom = pedit->rcVis.bottom;
                refresh_rc.right = pedit->rcVis.right;

                scroll_rc.right = pedit->rcVis.right;

                calc_charpos_cx(hWnd, pedit, pedit->selStart, &sel_start_x);
                calc_charpos_cx(hWnd, pedit, pedit->selEnd, &sel_end_x);
                if (pedit->selStart < pedit->selEnd)
                {
                    scroll_rc.left = sel_start_x + 1;
                }
                else
                {
                    scroll_rc.left = sel_end_x + 1;
                }

                sleInsertText_refresh (hWnd, pedit, NULL, del);
                edtSetCaretPos (hWnd, pedit);

                scroll_len = abs(sel_start_x - sel_end_x);
                refresh_rc.left = refresh_rc.right - scroll_len;
                ScrollWindow(hWnd, -scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (pedit->nContW <= pedit->nVisW)
                {
                    InvalidateRect(hWnd, NULL, TRUE);
                }

            }
            else
            {
                refresh_rc.top = pedit->rcVis.top;
                refresh_rc.bottom = pedit->rcVis.bottom;
                refresh_rc.right = pedit->rcVis.right;

                scroll_rc.right = pedit->rcVis.right;
                old_caret_x = get_caretpos_x(hWnd);


                sleInsertText_refresh (hWnd, pedit, NULL, del);
                edtSetCaretPos (hWnd, pedit);
                cur_caret_x = get_caretpos_x(hWnd);
                scroll_rc.left = cur_caret_x;

                scroll_len = abs(cur_caret_x - old_caret_x);
                refresh_rc.left = refresh_rc.right - scroll_len;
                ScrollWindow(hWnd, -scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (pedit->nContW <= pedit->nVisW)
                {
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }

        }
        else //right scroll window while pedit->nContX > 0
        {
            if (pedit->selStart != pedit->selEnd)
            {

                refresh_rc.top = pedit->rcVis.top;
                refresh_rc.bottom = pedit->rcVis.bottom;
                refresh_rc.left = pedit->rcVis.left;

                calc_charpos_cx(hWnd, pedit, pedit->selStart, &sel_start_x);
                calc_charpos_cx(hWnd, pedit, pedit->selEnd, &sel_end_x);
                if (pedit->selStart < pedit->selEnd)
                {
                    scroll_rc.right = sel_end_x + 1 - pedit->nContX;
                }
                else
                {
                    scroll_rc.right = sel_start_x + 1 - pedit->nContX;
                }

                scroll_rc.left = pedit->rcVis.left;
                sleInsertText_refresh (hWnd, pedit, NULL, del);

                scroll_len = abs(sel_start_x - sel_end_x);
                refresh_rc.right = refresh_rc.left + scroll_len;
                ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
                edtSetCaretPos (hWnd, pedit);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (pedit->nContW <= pedit->nVisW)
                {
                    InvalidateRect(hWnd, NULL, TRUE);
                }

                if (pedit->nContX <= 0)
                {
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
            else
            {
                int old_ncontw;

                refresh_rc.top = pedit->rcVis.top;
                refresh_rc.bottom = pedit->rcVis.bottom;
                refresh_rc.left = pedit->rcVis.left;

                scroll_rc.left = pedit->rcVis.left;
                old_ncontw = pedit->nContW;
                sleInsertText_refresh (hWnd, pedit, NULL, del);
                edtSetCaretPos (hWnd, pedit);
                cur_caret_x = get_caretpos_x(hWnd);
                scroll_rc.right = cur_caret_x;

                scroll_len = abs(pedit->nContW - old_ncontw);
                refresh_rc.right = refresh_rc.left + abs(scroll_len);
                ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (pedit->nContW <= pedit->nVisW)
                {
                    InvalidateRect(hWnd, NULL, TRUE);
                }

                if (pedit->nContX <= 0)
                {
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }

        }

    }

    pedit->changed = TRUE;
    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
}

static void esright_del_refresh(HWND hWnd, PEDITDATA pedit, int del)
{
    RECT scroll_rc, refresh_rc;
    int sel_start_x, sel_end_x;
    int old_caret_x, cur_caret_x;
    int scroll_len;
    int old_peditpos_x, cur_peditpos_x;

    scroll_rc.top = pedit->rcVis.top;
    scroll_rc.bottom = pedit->rcVis.bottom;

    if (pedit->nContW <= pedit->nVisW)
    {
        if (pedit->selStart != pedit->selEnd)
        {

            calc_charpos_cx(hWnd, pedit, pedit->selStart, &sel_start_x);
            calc_charpos_cx(hWnd, pedit, pedit->selEnd, &sel_end_x);
            if (pedit->selStart < pedit->selEnd)
            {
                scroll_rc.right = sel_end_x + 1;
            }
            else
            {
                scroll_rc.right = sel_start_x + 1;
            }

            scroll_rc.left = pedit->rcVis.left;
            sleInsertText_refresh (hWnd, pedit, NULL, del);

            scroll_len = abs(sel_start_x - sel_end_x);
            ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
            edtSetCaretPos (hWnd, pedit);
        }
        else
        {
            old_caret_x = get_caretpos_x(hWnd);
            scroll_rc.left = pedit->rcVis.left;
            sleInsertText_refresh (hWnd, pedit, NULL, del);
            edtSetCaretPos (hWnd, pedit);
            cur_caret_x = get_caretpos_x(hWnd);
            scroll_rc.right = cur_caret_x;

            scroll_len = cur_caret_x - old_caret_x;
            ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

        }

    }
    else
    {
        if (pedit->nContX <=0) //left scroll window while the head of text is in the pedit->rcVis
        {
            if (pedit->selStart != pedit->selEnd)
            {

                refresh_rc.top = pedit->rcVis.top;
                refresh_rc.bottom = pedit->rcVis.bottom;
                refresh_rc.right = pedit->rcVis.right;

                scroll_rc.right = pedit->rcVis.right;

                calc_charpos_cx(hWnd, pedit, pedit->selStart, &sel_start_x);
                calc_charpos_cx(hWnd, pedit, pedit->selEnd, &sel_end_x);
                if (pedit->selStart < pedit->selEnd)
                {
                    scroll_rc.left = sel_start_x + 1;
                }
                else
                {
                    scroll_rc.left = sel_end_x + 1;
                }

                sleInsertText_refresh (hWnd, pedit, NULL, del);
                edtSetCaretPos (hWnd, pedit);

                scroll_len = abs(sel_start_x - sel_end_x);
                refresh_rc.left = refresh_rc.right - scroll_len;
                ScrollWindow(hWnd, -scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (pedit->nContW <= pedit->nVisW)
                {
                    InvalidateRect(hWnd, NULL, TRUE);
                }

            }
            else
            {
                refresh_rc.top = pedit->rcVis.top;
                refresh_rc.bottom = pedit->rcVis.bottom;
                refresh_rc.right = pedit->rcVis.right;

                scroll_rc.right = pedit->rcVis.right;

                scroll_rc.left = get_caretpos_x(hWnd);
                calc_charpos_cx(hWnd, pedit, pedit->editPos, &old_peditpos_x);

                sleInsertText_refresh (hWnd, pedit, NULL, del);
                calc_charpos_cx(hWnd, pedit, pedit->editPos, &cur_peditpos_x);
                edtSetCaretPos (hWnd, pedit);

                scroll_len = old_peditpos_x - cur_peditpos_x;
                refresh_rc.left = refresh_rc.right - scroll_len;
                ScrollWindow(hWnd, -scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (pedit->nContW <= pedit->nVisW)
                {
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }

        }
        else //right scroll window while pedit->nContX > 0
        {
            if (pedit->selStart != pedit->selEnd)
            {

                refresh_rc.top = pedit->rcVis.top;
                refresh_rc.bottom = pedit->rcVis.bottom;
                refresh_rc.left = pedit->rcVis.left;

                calc_charpos_cx(hWnd, pedit, pedit->selStart, &sel_start_x);
                calc_charpos_cx(hWnd, pedit, pedit->selEnd, &sel_end_x);
                if (pedit->selStart < pedit->selEnd)
                {
                    scroll_rc.right = sel_end_x + 1 - pedit->nContX;
                }
                else
                {
                    scroll_rc.right = sel_start_x + 1 - pedit->nContX;
                }

                scroll_rc.left = pedit->rcVis.left;
                sleInsertText_refresh (hWnd, pedit, NULL, del);

                scroll_len = abs(sel_start_x - sel_end_x);
                refresh_rc.right = refresh_rc.left + scroll_len;
                ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
                edtSetCaretPos (hWnd, pedit);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (scroll_len >= pedit->nVisW)
                {
                    InvalidateRect(hWnd, NULL, TRUE);
                }

                if (pedit->nContW <= pedit->nVisW)
                {
                    InvalidateRect(hWnd, NULL, TRUE);
                }

                if (pedit->nContX <= 0)
                {
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
            else
            {
                refresh_rc.top = pedit->rcVis.top;
                refresh_rc.bottom = pedit->rcVis.bottom;
                refresh_rc.left = pedit->rcVis.left;

                old_caret_x = get_caretpos_x(hWnd);
                scroll_rc.left = pedit->rcVis.left;
                sleInsertText_refresh (hWnd, pedit, NULL, del);
                edtSetCaretPos (hWnd, pedit);
                cur_caret_x = get_caretpos_x(hWnd);
                scroll_rc.right = cur_caret_x;

                scroll_len = cur_caret_x - old_caret_x;
                refresh_rc.right = refresh_rc.left + abs(scroll_len);
                ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (pedit->nContW <= pedit->nVisW)
                {
                    InvalidateRect(hWnd, NULL, TRUE);
                }

                if (pedit->nContX <= 0)
                {
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }

        }

    }
    pedit->changed = TRUE;
    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
}

static void esleft_del_refresh(HWND hWnd, PEDITDATA pedit, int del)
{
    int old_nContX = pedit->nContX;
    RECT scroll_rc, del_rc, refresh_rc;
    int txtlen_x;
    int sel_start_x, sel_end_x;
    int old_caret_x, cur_caret_x;
    int scroll_len;



    scroll_rc.top = pedit->rcVis.top;
    scroll_rc.bottom = pedit->rcVis.bottom;
    del_rc.bottom = pedit->rcVis.bottom;
    del_rc.top = pedit->rcVis.top;
    refresh_rc.top = pedit->rcVis.top;
    refresh_rc.bottom = pedit->rcVis.bottom;

    if (pedit->selStart != pedit->selEnd)
    {

        if (pedit->selStart < pedit->selEnd)
        {
            calc_charpos_cx(hWnd, pedit, pedit->selEnd, &sel_end_x);
            scroll_rc.left = sel_end_x;
        }
        else
        {
            calc_charpos_cx(hWnd, pedit, pedit->selStart, &sel_start_x);
            scroll_rc.left = sel_start_x;
        }

        scroll_rc.right = pedit->rcVis.right;
    }
    else
    {
        calc_charpos_cx(hWnd, pedit, pedit->editPos, &scroll_rc.left);
        scroll_rc.left = scroll_rc.left - old_nContX;
        scroll_rc.right = pedit->rcVis.right;
    }

    old_caret_x = get_caretpos_x(hWnd);
    calc_charpos_cx(hWnd, pedit, pedit->content.txtlen, &txtlen_x);

    sleInsertText_refresh (hWnd, pedit, NULL, del);

    calc_charpos_cx(hWnd, pedit, pedit->editPos, &del_rc.left);
    del_rc.left = del_rc.left - old_nContX;
    del_rc.right = scroll_rc.left;

    scroll_len = del_rc.left - scroll_rc.left;

    if (abs(sel_start_x - sel_end_x) >= pedit->nVisW)
    {
        InvalidateRect(hWnd, NULL, TRUE);
    }
    else
    {
        if ((old_nContX > 0) && ((txtlen_x - old_nContX) <= pedit->rcVis.right))
        {
            cur_caret_x = get_caretpos_x(hWnd);
            if (cur_caret_x != old_caret_x)
            {
                scroll_len = cur_caret_x - old_caret_x;
                scroll_rc.left = pedit->rcVis.left;
                scroll_rc.right = old_caret_x;

                ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

                refresh_rc.left = old_caret_x;
                refresh_rc.right = cur_caret_x;

                InvalidateRect(hWnd, &refresh_rc, TRUE);
            }
            else if (pedit->selStart != pedit->selEnd)
            {
                scroll_len = sel_start_x - sel_end_x;
                scroll_rc.left = pedit->rcVis.left;
                scroll_rc.right = old_caret_x;

                ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

                refresh_rc.left = old_caret_x;
                refresh_rc.right = cur_caret_x;

                InvalidateRect(hWnd, &refresh_rc, TRUE);
            }
            else
            {
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        else
        {
            int rl;

            InvalidateRect(hWnd, &del_rc, TRUE);
            ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
            calc_charpos_cx(hWnd, pedit, pedit->content.txtlen, &rl);

            if (((pedit->nContW - pedit->nContX) == pedit->nVisW) && (old_nContX > 0))
            {
                InvalidateRect(hWnd, NULL, TRUE);
            }
            else if (rl < pedit->nVisW)
            {
                refresh_rc.left = rl;
                refresh_rc.right = pedit->rcVis.right;

                InvalidateRect(hWnd, &refresh_rc, TRUE);

            }
            else
            {
                refresh_rc.left = pedit->rcVis.right - abs(scroll_len);
                refresh_rc.right = pedit->rcVis.right;

                InvalidateRect(hWnd, &refresh_rc, TRUE);
            }
        }
    }

}

static int sleditit_reset_text (HWND hWnd, PEDITDATA pedit, char* str);

static int sleKeyDown (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    BOOL    bChange = FALSE;
    DWORD   dwStyle = GetWindowStyle (hWnd);
    PCONTROL pCtrl   = Control(hWnd);
    PEDITDATA pedit = (PEDITDATA) (pCtrl->dwAddData2);

    switch (LOWORD (wParam))
    {
    case SCANCODE_CURSORBLOCKUP:  //   +
    case SCANCODE_CURSORBLOCKDOWN:  // -
    {
        BOOL 	is_inc  = FALSE;
        int input_num = 0;
        char buf[8] =  {'\0','\0','\0','\0','\0','\0','\0','\0'};
        if((GetWindowStyle(hWnd) & ES_INPUT_NUM) != ES_INPUT_NUM)
            break;
		if(pedit->content.string)
        input_num  = atoi((char *)pedit->content.string);
		else
		  input_num = 0;
        is_inc  = (LOWORD (wParam) ==SCANCODE_CURSORBLOCKUP ) ? TRUE:FALSE;

        if(is_inc  ) //+
        {
            input_num++;
            if(input_num > (int)GetWindowAdditionalData (hWnd))
            {
                /////sprintf(buf,"%d",(int)GetWindowAdditionalData (hWnd));
                /////sprintf(buf,"%d",0);
                input_num = 0;
            }
        }
        else
        {
            input_num--;
            if(input_num < 0)
            {
                /////sprintf(buf,"%d",(int)GetWindowAdditionalData (hWnd));
                input_num = (int)GetWindowAdditionalData (hWnd);


            }
        }
        sprintf(buf,"%d",input_num);
        sleditit_reset_text ( hWnd,  pedit,buf);
        break;
    }
    case SCANCODE_KEYPADENTER:
    case SCANCODE_ENTER:
        NotifyParent (hWnd, pCtrl->id, EN_ENTER);
        return 0;

    case SCANCODE_HOME:
    {
        BOOL refresh = FALSE;

        if(lParam & KS_SHIFT)
        {
            make_pos_visible (hWnd, pedit, pedit->startx);
            pedit->selStart = 0;
            pedit->selEnd = pedit->editPos;
            check_caret();
            //InvalidateRect (hWnd, NULL, TRUE);
            InvalidateRect (hWnd, &pedit->rcVis, TRUE);

            NotifyParent (hWnd, pCtrl->id, EN_SELCHANGED);

            return 0;
        }

        if (pedit->selStart != pedit->selEnd)
        {
            ShowCaret(hWnd);
            refresh = TRUE;
        }

        pedit->editPos  = 0;
        pedit->selStart = pedit->selEnd = 0;

        if (!edtSetCaretPos (hWnd, pedit) && refresh)
            //InvalidateRect (hWnd, NULL, TRUE);
            InvalidateRect (hWnd, &pedit->rcVis, TRUE);


        return 0;
    }

    case SCANCODE_END:
    {
        BOOL refresh = FALSE;

        if(lParam & KS_SHIFT)
        {
            pedit->selStart = pedit->editPos;
            pedit->selEnd = pedit->content.txtlen;
            check_caret();
            make_charpos_visible (hWnd, pedit, pedit->selEnd, NULL);
            InvalidateRect(hWnd, NULL, TRUE);

            NotifyParent (hWnd, pCtrl->id, EN_SELCHANGED);

            return 0;
        }

        if (pedit->selStart != pedit->selEnd)
        {
            ShowCaret(hWnd);
            refresh = TRUE;
        }

        pedit->editPos = pedit->content.txtlen;
        pedit->selStart = pedit->selEnd = pedit->editPos;

        if (!edtSetCaretPos (hWnd, pedit) && refresh)
            InvalidateRect (hWnd, NULL, TRUE);


        return 0;
    }

    case SCANCODE_CURSORBLOCKLEFT:
    {
        BOOL refresh = FALSE;

        if(lParam & KS_SHIFT)
        {
            if (pedit->selStart == pedit->selEnd)
            {
                pedit->selStart = pedit->selEnd = pedit->editPos;
            }

            if (pedit->selStart < pedit->editPos ||
                    pedit->selStart == pedit->selEnd)
            {
                if (pedit->selStart > 0)
                    shift_one_char_l(selStart);
            }
            else
            {
                shift_one_char_l(selEnd);
            }

            check_caret ();
            make_charpos_visible (hWnd, pedit, pedit->selStart, NULL);
            InvalidateRect (hWnd, NULL, TRUE);

            NotifyParent (hWnd, pCtrl->id, EN_SELCHANGED);
            return 0;
        }

		if (0 == pedit->editPos && pedit->selStart == pedit->selEnd)
			return 1;

		shift_one_char_l (editPos);

        if (pedit->selStart != pedit->selEnd)
        {
            ShowCaret (hWnd);
            ActiveCaret (hWnd);
            refresh = TRUE;
        }

        pedit->selStart = pedit->selEnd = pedit->editPos;
        if (!edtSetCaretPos (hWnd, pedit) && refresh)
        {
            InvalidateRect (hWnd, NULL, TRUE);
        }

        return 0;
    }

    case SCANCODE_CURSORBLOCKRIGHT:
    {
        BOOL refresh = FALSE;

        if(lParam & KS_SHIFT)
        {
            if(pedit->selStart == pedit->selEnd)
                pedit->selStart = pedit->selEnd = pedit->editPos;

            if (pedit->selStart == pedit->selEnd ||
                    pedit->selEnd > pedit->editPos)
            {
                if (pedit->selEnd < pedit->content.txtlen)
                    shift_one_char_r (selEnd);
            }
            else
            {
                shift_one_char_r (selStart);
            }

            check_caret ();
            make_charpos_visible (hWnd, pedit, pedit->selEnd, NULL);
            InvalidateRect (hWnd, NULL, TRUE);
            NotifyParent (hWnd, pCtrl->id, EN_SELCHANGED);
            return 0;
        }

		if (pedit->editPos == pedit->content.txtlen
			&& pedit->selStart == pedit->selEnd)
			return 1;

		shift_one_char_r (editPos);

        if (pedit->selStart != pedit->selEnd)
        {
            ShowCaret (hWnd);
            ActiveCaret (hWnd);
            refresh = TRUE;
        }

        pedit->selStart = pedit->selEnd = pedit->editPos;
        if (!edtSetCaretPos (hWnd, pedit) && refresh)
            InvalidateRect (hWnd, NULL, TRUE);

	  return 0;
    }
    case SCANCODE_REMOVE:
    {
        int del;
        int oldpos = pedit->editPos;
          pedit->selStart = pedit->selEnd -1 ;
		  if(pedit->selStart < 0)  pedit->selStart = 0;

        if ((dwStyle & ES_READONLY))
            return 0;
        if (pedit->editPos == pedit->content.txtlen && pedit->selStart == pedit->selEnd)
            return 0;
        if (pedit->selStart == pedit->selEnd &&
                pedit->editPos < pedit->content.txtlen)
        {
            shift_one_char_r(editPos);
        }
        del = oldpos - pedit->editPos;
        if (dwStyle & ES_LEFT)
        {
            esleft_del_refresh(hWnd, pedit, del);

            if (!edtSetCaretPos (hWnd, pedit))
            {
                //InvalidateRect (hWnd, NULL, TRUE);
            }

            pedit->changed = TRUE;
            NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
        }
        else if (dwStyle & ES_RIGHT)
        {
            esright_del_refresh(hWnd, pedit, del);
        }
        else if (dwStyle & ES_CENTER)
        {
            sleInsertText (hWnd, pedit, NULL, del);
            // escenter_del_refresh(hWnd, pedit, del);
        }
        else
        {
            sleInsertText (hWnd, pedit, NULL, del);
        }

    }
    break;
    case SCANCODE_A:
    {
        if (lParam & KS_CTRL)
        {
            sleSetSel (hWnd, pedit, 0, pedit->content.txtlen);
        }
        return 0;
    }

#ifdef _CLIPBOARD_SUPPORT
    case SCANCODE_C:
    case SCANCODE_X:
    {
        if ((lParam & KS_CTRL) && (pedit->selEnd - pedit->selStart > 0))
        {
            SetClipBoardData (CBNAME_TEXT, pedit->content.string + pedit->selStart,
                              pedit->selEnd - pedit->selStart, CBOP_NORMAL);
            if (wParam == SCANCODE_X && !(GetWindowStyle(hWnd) & ES_READONLY))
            {
                sleInsertText (hWnd, pedit, NULL, 0);
            }
        }
        return 0;
    }

    case SCANCODE_V:
    {
        if (!(lParam & KS_CTRL))
            return 0;
        return sleInsertCbText (hWnd, pedit);
    }
#endif /* _CLIPBOARD_SUPPORT */

    case SCANCODE_INSERT:
        pedit->status ^= EST_REPLACE;
        break;
    default:
        break;
    } //end switch

    if (bChange)
    {
        pedit->changed = TRUE;
        NotifyParent (hWnd, pCtrl->id, EN_CHANGE);
    }

    return 0;
}
static void esright_input_char_refresh(HWND hWnd, PEDITDATA pedit, char *charBuffer, int chars)
{
    RECT scroll_rc;
    int scroll_len;
    int old_ncontw, cur_ncontw;
    int cur_caret_x;

    if (pedit->selStart != pedit->selEnd)
    {
        sleInsertText (hWnd, pedit, (char* )charBuffer, chars);
    }
    else //left scroll window
    {
        scroll_rc.top = pedit->rcVis.top;
        scroll_rc.bottom = pedit->rcVis.bottom;
        calc_content_width(hWnd, pedit, pedit->content.txtlen, &old_ncontw);
        sleInsertText_refresh (hWnd, pedit, (char* )charBuffer, chars);
        calc_content_width(hWnd, pedit, pedit->content.txtlen, &cur_ncontw);
        scroll_len = abs(old_ncontw- cur_ncontw);
        cur_caret_x = get_caretpos_x(hWnd);
        scroll_rc.left = pedit->rcVis.left;
        scroll_rc.right = cur_caret_x;

        ScrollWindow(hWnd, -scroll_len, 0, &scroll_rc, NULL);
        UpdateWindow(hWnd, FALSE);
        edtSetCaretPos (hWnd, pedit);

        pedit->changed = TRUE;
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);

    }


}

static void esleft_input_char_refresh(HWND hWnd, PEDITDATA pedit, char *charBuffer, int chars)
{
    int old_caretpos_x;// cur_caretpos_x;
    int old_sel_start, old_sel_end, old_sel_start_x, old_sel_end_x, cur_sel_start_x;
    RECT scroll_rc, refresh_rc;
    int scroll_len;
    int old_pedit_pos_x, cur_pedit_pos_x;
    int old_nContX;//, cur_ncontx;
    int char_size, sel_size;//char_size is width of input chars    sel_size is width of the selected area
    int txtlen_x;

    old_sel_start = pedit->selStart;
    old_sel_end = pedit->selEnd;

    old_caretpos_x = get_caretpos_x(hWnd);
    calc_charpos_cx(hWnd, pedit, pedit->editPos, &old_pedit_pos_x);
    calc_charpos_cx(hWnd, pedit, pedit->content.txtlen, &txtlen_x);
    old_nContX= pedit->nContX;
    calc_charpos_cx(hWnd, pedit, old_sel_start, &old_sel_start_x);
    calc_charpos_cx(hWnd, pedit, old_sel_end, &old_sel_end_x);
    old_sel_start_x = old_sel_start_x - pedit->nContX;
    old_sel_end_x = old_sel_end_x - pedit->nContX;

    sleInsertText_refresh (hWnd, pedit, (char* )charBuffer, chars);

    calc_charpos_cx(hWnd, pedit, pedit->editPos, &cur_pedit_pos_x);
    calc_charpos_cx(hWnd, pedit, old_sel_start, &cur_sel_start_x);
    //cur_ncontx = pedit->nContX;
    //cur_caretpos_x = get_caretpos_x(hWnd);
    char_size = cur_pedit_pos_x - cur_sel_start_x;
    sel_size = abs(old_sel_start_x - old_sel_end_x);

    scroll_rc.top = pedit->rcVis.top;
    scroll_rc.bottom = pedit->rcVis.bottom;
    refresh_rc.top = pedit->rcVis.top;
    refresh_rc.bottom = pedit->rcVis.bottom;

    if (char_size >= pedit->nVisW)
    {
        InvalidateRect(hWnd, NULL, TRUE);
    }
    else
    {
        if (old_sel_start != old_sel_end)
        {

            if (sel_size >= pedit->nVisW)
            {
                InvalidateRect(hWnd, NULL, TRUE);
            }
            else
            {
                if (char_size < sel_size)
                {
                    if ((old_nContX > 0) && ((txtlen_x - old_nContX) <= pedit->rcVis.right)) //right scroll window
                    {
                        scroll_len = sel_size - char_size;
                        scroll_rc.left = pedit->rcVis.left;
                        scroll_rc.right = old_sel_start_x < old_sel_end_x ? old_sel_start_x : old_sel_end_x;

                        ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

                        refresh_rc.left = scroll_rc.right;
                        refresh_rc.right = refresh_rc.left + sel_size + scroll_len;

                        InvalidateRect(hWnd, &refresh_rc, TRUE);
                    }
                    else //left scroll window
                    {
                        scroll_len = -(sel_size - char_size);
                        scroll_rc.left = old_sel_start_x < old_sel_end_x ? old_sel_end_x : old_sel_start_x;
                        scroll_rc.right = pedit->rcVis.right;

                        ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

                        refresh_rc.left = scroll_rc.left - sel_size;
                        refresh_rc.right = scroll_rc.left;

                        InvalidateRect(hWnd, &refresh_rc, TRUE);
                    }
                }
                else //when char_size > sel_size right scroll window
                {
                    scroll_len = char_size - sel_size;
                    scroll_rc.left = old_sel_start_x < old_sel_end_x ? old_sel_end_x : old_sel_start_x;
                    scroll_rc.right = pedit->rcVis.right;

                    ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

                    refresh_rc.left = scroll_rc.left - sel_size;
                    refresh_rc.right = refresh_rc.left + char_size + 1;

                    InvalidateRect(hWnd, &refresh_rc, TRUE);
                }
            }
        }
        else
        {
            int right;
            char_size = cur_pedit_pos_x - old_pedit_pos_x;
            calc_charpos_cx(hWnd, pedit, pedit->content.txtlen, &right);
            if (old_caretpos_x < pedit->rcVis.right) //right scroll window
            {
                scroll_len = cur_pedit_pos_x - old_pedit_pos_x;
                if ((pedit->rcVis.right - old_caretpos_x) < scroll_len) //
                {
                    scroll_len = -(scroll_len - (pedit->rcVis.right- old_caretpos_x));
                    scroll_rc.left = pedit->rcVis.left;
                    scroll_rc.right = old_caretpos_x;
                    ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

                    refresh_rc.left = pedit->rcVis.right - char_size;
                    refresh_rc.right = pedit->rcVis.right;

                    InvalidateRect(hWnd, &refresh_rc, TRUE);
                }
                else
                {
                    scroll_rc.left = old_caretpos_x;
                    scroll_rc.right = (right < pedit->rcVis.right) ? right+1 : pedit->rcVis.right;
                    ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

                    refresh_rc.left = pedit->rcVis.right - char_size;
                    refresh_rc.right = pedit->rcVis.right;

                    InvalidateRect(hWnd, &refresh_rc, TRUE);
                }
            }
            else //left scroll window
            {
                scroll_len = old_pedit_pos_x - cur_pedit_pos_x;
                ScrollWindow(hWnd, scroll_len, 0, &pedit->rcVis, NULL);

                refresh_rc.left = pedit->rcVis.right - char_size;
                refresh_rc.right = pedit->rcVis.right;

                InvalidateRect(hWnd, &refresh_rc, TRUE);
            }

            if (((pedit->nContW - pedit->nContX) == pedit->nVisW) && (old_nContX > 0))
            {
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
    }
}
//刷新插入符的位置
static void EditRefreshCaret (HWND hWnd, PEDITDATA ppedit, BOOL bInited)
{
    DWORD   dwStyle = GetWindowStyle(hWnd);
    SIZE    txtsize;
    int     outchar;
    HDC     hdc;

    if (!ppedit)
        return;

    hdc= GetClientDC (hWnd);
    outchar = GetTextExtentPoint (hdc, (const char*)ppedit->content.string,
                                  ppedit->content.txtlen,
                                  0,
                                  NULL, NULL, NULL, &txtsize);
    ReleaseDC (hdc);

    if (bInited)
    {
        if (dwStyle & ES_RIGHT)
            ppedit->editPos = outchar;
        else if (dwStyle & ES_CENTER)
            ppedit->editPos = outchar/2;
        else
            ///// ppedit->editPos = 0;
            ppedit->editPos = outchar;  // 不然会出问题

    }
    if (!edtSetCaretPos (hWnd, ppedit))
    {
        InvalidateRect (hWnd, NULL, TRUE);
    }
}

static int sleditit_reset_text (HWND hWnd, PEDITDATA pedit, char* str)
{
    DWORD   dwStyle = GetWindowStyle(hWnd);
    char*   buffer  = str;

    if (!pedit || !buffer)
        return -1;

    if (dwStyle & ES_PASSWORD)
    {
        buffer = strdup (buffer);
        check_valid_passwd (buffer);
    }
    /*modify by luoping 2010.9.16*/
    pedit->status &= EST_FOCUSED;
    pedit->selStart = 0;
    pedit->selEnd = 0;

    if (pedit->content.string)
    {
        free (pedit->content.string);
        pedit->content.string = NULL;
        pedit->content.buffsize = 0;
        pedit->content.txtlen = 0;
    }

    EditSetText (pedit, buffer);
    RecalcSize (hWnd, pedit, TRUE);


    if (dwStyle & ES_PASSWORD)
    {
        free (buffer);
    }

    EditRefreshCaret (hWnd, pedit, TRUE);
    pedit->changed = TRUE;
    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_UPDATE);
    return 0;
}

static int peditit_insert_text (HWND hWnd, PEDITDATA pedit, const char* newtext, int len)
{
    int     i, realen = strlen (newtext);
    char*   newbuffer;
    DWORD   dwStyle = GetWindowStyle(hWnd);

    if (len > realen)
        len = realen;

    if (!newtext || len <= 0)
        return -1;

    if (dwStyle & ES_READONLY)
        return 0;

    if (!(newbuffer = calloc (1, realen)))
        return -1;

    for (i = 0; i < len && newtext[i] != '\0'; i ++)
    {
        if (dwStyle & ES_UPPERCASE)
        {
            newbuffer[i] = toupper (newtext[i]);
        }
        else if (dwStyle & ES_LOWERCASE)
        {
            newbuffer[i] = tolower (newtext[i]);
        }
        else
            newbuffer[i] = newtext[i];
    }

    sleInsertText (hWnd, pedit, (char* )newbuffer, len);

    free(newbuffer);
    return 0;
}

static int EditCtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    DWORD       dwStyle;
    HDC         hdc;
    PEDITDATA pedit = NULL;

    dwStyle  = GetWindowStyle(hWnd);
    if (message != MSG_CREATE)
    {
        pedit = (PEDITDATA) GetWindowAdditionalData2 (hWnd);
    }
    switch (message)
    {
    case MSG_CREATE:		
		SetWindowFont(hWnd, GetSystemFont(SYSLOGFONT_CONTROL));
        if ( !(pedit = calloc (1, sizeof (EDITDATA))) )
            return -1;
        if (EditInit (hWnd, pedit) < 0)
            return -1;
        break;

    case MSG_DESTROY:
    {
        editit_destroy (hWnd, pedit);
        free (pedit);
        break;
    }
    case MSG_ERASEBKGND:
    {
        EditOnEraseBackground (hWnd, (HDC)wParam, (const RECT*)lParam);
    }
    return 0;
    case MSG_PAINT:
    {
        hdc = BeginPaint (hWnd);
        EditPaint (hWnd, hdc, pedit);
        EndPaint (hWnd, hdc);

        return 0;
    }
    case MSG_SETSLEDITFOCUS:
    {
        if((BOOL)lParam)
        {
            pedit->status|=EST_FOCUSED;
        }
        else
        {
            pedit->status&=~EST_FOCUSED;
        }
           InvalidateRect (hWnd, NULL, TRUE);//导致重绘
        return 0;
    }

    case MSG_SIZECHANGED:
        RecalcSize (hWnd, pedit, FALSE);
        return 0;

    case MSG_FONTCHANGING:
        if (dwStyle & ES_PASSWORD)
            return -1;
        return 0;

    case MSG_FONTCHANGED:
    {
        //字体改变 后还需销毁插入符 然后再创建
        pedit->starty  = pedit->topMargin + ( pedit->rcVis.bottom - pedit->rcVis.top -
                                              GetWindowFont (hWnd)->size - 1 ) / 2;

        DestroyCaret (hWnd);
        //////CreateCaret (hWnd, NULL, 1, GetWindowFont (hWnd)->size);
        CreateCaret (hWnd, NULL, 1, GetWindowFont (hWnd)->size);

        RecalcSize (hWnd, pedit, FALSE);
        return 0;
    }

    case MSG_SETCURSOR:
        if (dwStyle & WS_DISABLED)
        {
            SetCursor (GetSystemCursor (IDC_ARROW));
            return 0;
        }
        break;
    case MSG_KILLFOCUS:
        if (pedit->status & EST_FOCUSED)
        {
            BOOL refresh = FALSE;

            pedit->status &= ~EST_FOCUSED;
            HideCaret (hWnd);//隐藏插入符
            if ( pedit->selStart != pedit->selEnd || (dwStyle & ES_TIP && pedit->content.txtlen <= 0))
            {
                refresh = TRUE;
            }
            if (pedit->selStart != pedit->selEnd && !(dwStyle &ES_NOHIDESEL))
                pedit->selStart = pedit->selEnd = pedit->editPos;

            if (refresh)
                InvalidateRect (hWnd, NULL, TRUE);//导致重绘

            NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_KILLFOCUS);

            if (pedit->changed)
            {
                pedit->changed = FALSE;
                NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CONTCHANGED);
            }
        }
        InvalidateRect (hWnd, NULL, TRUE);//导致重绘
        break;

    case MSG_SETFOCUS:		
        if (pedit->status & EST_FOCUSED)//已经是焦点的话  就不设置了
            break;
        pedit->status |= EST_FOCUSED;
        ActiveCaret (hWnd);
        ShowCaret(hWnd);//显示插入符

        if (dwStyle & ES_AUTOSELECT)
        {
            if (sleSetSel (hWnd, pedit, 0, pedit->content.txtlen) <= 0 && dwStyle & ES_TIP)
                InvalidateRect (hWnd, NULL, TRUE);
            if (lParam == 1)
                pedit->status |= EST_TMP;
        }
        else if ( (dwStyle & ES_NOHIDESEL && pedit->selStart != pedit->selEnd)
                  || (dwStyle & ES_TIP && pedit->content.txtlen <= 0))
        {
            InvalidateRect (hWnd, NULL, TRUE);
        }
       	InvalidateRect (hWnd, NULL, TRUE);//导致重绘
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_SETFOCUS);
        break;

    case MSG_ENABLE:
        if ( (!(dwStyle & WS_DISABLED) && !wParam)
                || ((dwStyle & WS_DISABLED) && wParam) )
        {
            if (wParam)
                ExcludeWindowStyle(hWnd,WS_DISABLED);
            else
                IncludeWindowStyle(hWnd,WS_DISABLED);

            InvalidateRect (hWnd, NULL, TRUE);
        }
        return 0;

    case MSG_GETTEXTLENGTH:
        return pedit->content.txtlen;

    case MSG_GETTEXT:
    {
        char*   buffer = (char*)lParam;
        int     len;

        len = MIN ((int)wParam, pedit->content.txtlen);
        memcpy (buffer, pedit->content.string, len);
        buffer [len] = '\0';
        return len;
    }

    case MSG_SETTEXT:
    {
        return sleditit_reset_text (hWnd, pedit, (char*)lParam);
    }
    case MSG_KEYDOWN:
    {
        return sleKeyDown (hWnd, wParam, lParam); //这个内部函数要修改
    }

    case MSG_CHAR:
    {
		
        unsigned char charBuffer [3] = {0,0,0};
        int chars;
        //// if (dwStyle & ES_INPUT_NUM)
		// 去掉一些重用键的处理
		// 这里以后要去掉
		if (60 == wParam)		//慢放按钮
			
		{
			break;
		}

        if (wParam == 127) // BS
            wParam = '\b';

        if (dwStyle & ES_READONLY)
            return 0;

        charBuffer [0] = LOBYTE (wParam);
        charBuffer [1] = HIBYTE (wParam);
        charBuffer [2] = (0x00ff0000 & wParam) >> 16;

        if((GetWindowStyle(hWnd) & ES_INPUT_NUM)&&!((charBuffer [0] >=  '0'  &&  charBuffer [0] <= '9')|| ( charBuffer[0] == '\b')))
        {
            return 0;
        }


        if (charBuffer [2])
        {
            chars = 3;
        }
        else if (HIBYTE (wParam))
        {
            chars = 2;
        }
        else
        {
            chars = 1;

            if (dwStyle & ES_UPPERCASE)//只显示大写字母
            {
                charBuffer [0] = toupper (charBuffer[0]);
            }
            else if (dwStyle & ES_LOWERCASE)
            {
                charBuffer [0] = tolower (charBuffer[0]);
            }
        }

        if (chars == 1)
        {
            if (charBuffer [0] < 0x20 && charBuffer[0] != '\b') //FIXM
                return 0;


        }

        if (wParam == '\b')   //backspace
        {
            int del;

            if (pedit->editPos == 0 && pedit->selStart == pedit->selEnd)
                return 0;

            del = -CHLENPREV((const char *)(pedit->content.string), (pedit->content.string + pedit->editPos) );

            if (dwStyle & ES_LEFT)//左对齐处理方式
            {
                esleft_del_refresh(hWnd, pedit, del);
                set_edit_caret_pos(hWnd, pedit);

                pedit->changed = TRUE;
                NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
            }
            else if (dwStyle & ES_RIGHT)
            {
                esright_backspace_refresh(hWnd, pedit, del);
            }
            else if (dwStyle & ES_CENTER)
            {
                sleInsertText (hWnd, pedit, NULL, del);
            }
            else
            {
                sleInsertText (hWnd, pedit, NULL, del);
            }
        }
        else
        {
            if (dwStyle & ES_LEFT)
            {
                esleft_input_char_refresh(hWnd, pedit, (char*)charBuffer, chars);
                set_edit_caret_pos(hWnd, pedit);

                pedit->changed = TRUE;
                NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
            }
            else if (dwStyle & ES_RIGHT)
            {
                esright_input_char_refresh(hWnd, pedit, (char*)charBuffer, chars);
            }
            else if (dwStyle & ES_CENTER)
            {
                sleInsertText (hWnd, pedit, (char* )charBuffer, chars);

            }
            else
            {
                sleInsertText (hWnd, pedit, (char* )charBuffer, chars);
            }
        }
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }
#if 0	
    case MSG_LBUTTONDBLCLK:
    {	
		//printf("dwStyle1 = 0x%x\r\n", dwStyle);
#ifndef IMA_CLICK_SHOW
        if (!(dwStyle&ES_READONLY))
        {
            SendNotifyMessage (__mg_ime_wnd, MSG_IME_LBD_SWITCH, hWnd, 0);
			NotifyParent (hWnd,  GetDlgCtrlID(hWnd), EN_DBLCLK);
        }
#endif	
        break;
    }
#endif	
    case MSG_LBUTTONDBLCLK:
    case MSG_LBUTTONDOWN:
    {
        if ( pedit->status & EST_TMP)
        {
            pedit->status &= ~EST_TMP;
            break;
        }

        if (HISWORD(lParam) < pedit->rcVis.top || HISWORD(lParam) > pedit->rcVis.bottom)
            break;
        set_caret_pos (hWnd, pedit, LOSWORD(lParam), FALSE);
        ActiveCaret (hWnd);
        ShowCaret(hWnd);
        SetCapture(hWnd);
#ifdef IMA_CLICK_SHOW		
		if (!(dwStyle&ES_READONLY))
        {
            SendNotifyMessage (__mg_ime_wnd, MSG_IME_LBD_SWITCH, hWnd, 0);
            NotifyParent (hWnd,  GetDlgCtrlID(hWnd), EN_CLICKED);
        }
#endif
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    }
    case MSG_NCLBUTTONUP:
    case MSG_LBUTTONUP:
        if (GetCapture() == hWnd)
            ReleaseCapture();
        NotifyParent (hWnd,  GetDlgCtrlID(hWnd), EN_CLICKED);

        if (pedit->selStart != pedit->selEnd)
            NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_SELCHANGED);
        break;
#if 1
   case MSG_MOUSEMOVEIN:
   {
	  BOOL in_out = (BOOL)wParam;

        if(in_out)
        {
            SetFocusChild (hWnd);
        }
        else
        {
        // 新输入法接口修改, by lyx 20130711
		if(!IsCustomImeOpen())
            SetNullFocus (GetParent(hWnd));
        }

        break;

   }
#endif   
    case MSG_MOUSEMOVE:
    {
        if (GetCapture () == hWnd)
            sleMouseMove (hWnd, pedit, lParam);
        return 0;
    }

    case MSG_GETDLGCODE:
        return DLGC_WANTCHARS | DLGC_HASSETSEL | DLGC_WANTARROWS | DLGC_WANTENTER;

    case MSG_DOESNEEDIME:
        if (dwStyle & ES_READONLY)
            return FALSE;
        return TRUE;

    case EM_GETCARETPOS:
    case EM_GETSELPOS:
    {
        int nr_chars, pos;
        int* line_pos = (int *)wParam;
        int* char_pos = (int *)lParam;

        if (message == EM_GETSELPOS)
            pos = (pedit->editPos == pedit->selStart) ? pedit->selEnd : pedit->selStart;
        else
            pos = pedit->editPos;
        //////// nr_chars = GetTextMCharInfo (GetWindowFont (hWnd),(const char* )(pedit->content.string), pos, NULL);
        nr_chars = GetTextMCharInfo (GetWindowFont (hWnd),(const char* )(pedit->content.string), pos, NULL);

        if (line_pos) *line_pos = 0;
        if (char_pos) *char_pos = nr_chars;

        return pos;
    }
//给组合框使用
    case EM_SETCARETPOS:
    case EM_SETSEL:
    {
        int char_pos = (int)lParam;
        int nr_chars, pos;
        int *pos_chars;

        if (char_pos < 0)
            return -1;

        pos_chars = ALLOCATE_LOCAL (pedit->content.txtlen * sizeof(int));
        //nr_chars = GetTextMCharInfo (GetWindowFont (hWnd),(const char* )pedit->content.string, pedit->content.txtlen, pos_chars);
        nr_chars = GetTextMCharInfo (GetWindowFont (hWnd),(const char* )pedit->content.string, pedit->content.txtlen, pos_chars);
        if (char_pos > nr_chars)
        {
            DEALLOCATE_LOCAL (pos_chars);
            return -1;
        }

        if (char_pos == nr_chars)
            pos = pedit->content.txtlen;
        else
            pos = pos_chars[char_pos];

        DEALLOCATE_LOCAL (pos_chars);

        if (message == EM_SETCARETPOS)
        {
            pedit->editPos = pos;
            pedit->selStart = pedit->selEnd = 0;
            edtSetCaretPos (hWnd, pedit);
            return pedit->editPos;
        }
        else
        {
            int start, end;
            if (pedit->editPos < pos)
            {
                start = pedit->editPos;
                end = pos;
            }
            else
            {
                start = pos;
                end = pedit->editPos;
            }
            return sleSetSel (hWnd, pedit, start, end);
        }
    }
    case EM_INSERTTEXT:
        return peditit_insert_text (hWnd, pedit, (const char*)lParam, wParam);

    case EM_SETREADONLY:
        if (wParam)
            IncludeWindowStyle (hWnd, ES_READONLY);
        else
            ExcludeWindowStyle (hWnd, ES_READONLY);
        return 0;

    case EM_SETPASSWORDCHAR:
        if (pedit->passwdChar != (int)wParam)
        {
            pedit->passwdChar = (int)wParam;
            if (dwStyle & ES_PASSWORD)
            {
                InvalidateRect (hWnd, NULL, TRUE);
            }
        }
        return 0;

    case EM_GETPASSWORDCHAR:
    {
        if (lParam)
            *((int*)lParam) = pedit->passwdChar;
        return 0;
    }

    case EM_GETMAXLIMIT:
        return pedit->hardLimit;

    case EM_LIMITTEXT:
    {
        int newLimit = (int)wParam;

        if (newLimit >= 0)
        {
            if (pedit->content.txtlen > newLimit)
            {
                Ping ();
                return -1;
            }
            else
            {
                pedit->hardLimit = newLimit;
                return 0;
            }
        }
        return -1;
    }

    case EM_GETTIPTEXT:
    {
        int len, tip_len;
        char *buffer = (char *)lParam;

        if (!(dwStyle & ES_TIP))
            return -1;

        tip_len = strlen (pedit->tiptext);

        if (!buffer)
            return tip_len;

        if (wParam >= 0)
            len = (wParam > DEF_TIP_LEN) ? DEF_TIP_LEN : wParam;
        else
            len = DEF_TIP_LEN;

        strncpy (buffer, pedit->tiptext, len);
        buffer[len] = '\0';

        return tip_len;
    }

    case EM_SETTIPTEXT:
    {
        int len;

        if (!(dwStyle & ES_TIP) || !lParam)
            return -1;

        if (wParam >= 0)
            len = (wParam > DEF_TIP_LEN) ? DEF_TIP_LEN : wParam;
        else
            len = DEF_TIP_LEN;
        strncpy (pedit->tiptext, (char *)lParam, len);
        pedit->tiptext[len] = '\0';

        if (pedit->content.txtlen <= 0)
            InvalidateRect (hWnd, NULL, TRUE);

        return strlen(pedit->tiptext);
    }

    case EM_GETSEL:
    {
        char *buffer = (char *)lParam;
        int len;

        if (!buffer || pedit->selEnd - pedit->selStart <= 0)
            return 0;

        if (wParam < 0)
            len = pedit->selEnd - pedit->selStart;
        else
            len = MIN(pedit->selEnd - pedit->selStart, (int)wParam);

        strncpy (buffer, (const char*)(pedit->content.string + pedit->selStart), len);

        return len;
    }

    case EM_INSERTCBTEXT:
    {
#ifdef _CLIPBOAR_SUPPORT
        return sleInsertCbText (hWnd, pedit);
#endif
    }

    case EM_COPYTOCB:
    case EM_CUTTOCB:
    {
#ifdef _CLIPBOARD_SUPPORT
        if (pedit->selEnd - pedit->selStart > 0)
        {
            SetClipBoardData (CBNAME_TEXT, pedit->content.string + pedit->selStart,
                              pedit->selEnd - pedit->selStart, CBOP_NORMAL);
            if (message == EM_CUTTOCB && !(GetWindowStyle(hWnd) & ES_READONLY))
            {
                sleInsertText (hWnd, pedit, NULL, 0);
            }
            return pedit->selEnd - pedit->selStart;
        }
#endif
        return 0;
    }

    default:
        break;

    } // end switch

    return DefaultControlProc (hWnd, message, wParam, lParam);
}

#endif /* _CTRL_SLEDIT */

