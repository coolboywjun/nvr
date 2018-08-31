
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <minigui/cliprect.h>
#include <minigui/internals.h>
#include <minigui/ctrlclass.h>


#include "ctrl_ipaddress.h"
#include "ctrl_data.h"
#include "gui_core.h"
#include "ui_config.h"

#ifdef _CTRL_IPADDRESS_H

#define DEFAULT_IP_ITEMS_WIDTH   36
#define DEFAULT_IP_ITEMS_HEIGHT  20

#define IP_DOT_WIDTH  3

typedef struct _tagIPITEM
{
    int x;
    int y;
    int width;
    int height;
    char ip[4];
    int isSel;
} IPITEM, *PIPITEM;

typedef struct _tagDATA
{
    int orgx;
    int orgy;
    int width;           // width
    int height;          // height
    int leftMargin;
    int rightMargin;
    int topMargin;
    int bottomMargin;

    gal_pixel color_bg;        // background color
    gal_pixel color_bg_hilight;
    gal_pixel color_ft;
    gal_pixel color_ft_hilight;
    gal_pixel color_boder;
    gal_pixel color_boder_hilight;

    IPITEM items[4];
    int nItems;       // 4
    int nItemSel;     // -1, 0 - 3

    int dotStartx;
    int dotStarty;
    int dotWidth;
    int dotHeight;
    int dotLength;

    HWND hWnd;
    int dwStyle;
    char *caption;
    int data;
    DWORD status;
} IPADDRESSDATA;

typedef IPADDRESSDATA *PIPADDRESSDATA;



#define IPADDRESS_BE_VALID_ITEM(nPos)    (nPos < pData->nItems && nPos >= 0)

extern BOOL IsCustomImeOpen();
static in_addr_t Items2ip(char *ipbuf, int length, IPITEM *items, int nItems)
{
    char ip[IP_LENGTH_MAX];
    int len;

    len = MIN(length, IP_LENGTH_MAX);

    if( items != NULL && nItems == 4)
    {

        snprintf(ip, IP_LENGTH_MAX, "%s.%s.%s.%s", items[0].ip, items[1].ip,
                 items[2].ip, items[3].ip);
        if(ipbuf)
        {
            memcpy(ipbuf, ip, len);
        }

        return inet_addr(ip);
    }

    return -1;
}

static int ip2Items(IPITEM *items, int nItems, in_addr_t ip, char *ipbuf)
{
    int ret;
    int num0, num1, num2, num3;

    if(items == NULL || nItems != 4)
    {
        return -1;
    }

    num0 = num1 = num2 = num3 = 0;

    if(ipbuf)
    {
        ret = sscanf(ipbuf, "%d.%d.%d.%d", &num0, &num1, &num2, &num3);
        if(ret == -1)
            return -1;

        snprintf(items[0].ip, 4, "%d", num0);
        snprintf(items[1].ip, 4, "%d", num1);
        snprintf(items[2].ip, 4, "%d", num2);
        snprintf(items[3].ip, 4, "%d", num3);
    }
    else
    {
        struct in_addr in;
        char *buf;

        in.s_addr = ip;
        buf = inet_ntoa(in);

        ret = sscanf(buf, "%d.%d.%d.%d", &num0, &num1, &num2, &num3);
        if(ret == -1)
            return -1;

        snprintf(items[0].ip, 4, "%d", num0);
        snprintf(items[1].ip, 4, "%d", num1);
        snprintf(items[2].ip, 4, "%d", num2);
        snprintf(items[3].ip, 4, "%d", num3);
    }

    return 0;
}


static void sDrawText (HDC hdc, int x, int y, int width, int height,
                       const char *psText, UINT format)
{
    RECT rect;

    if (psText != NULL)
    {
        SetRect (&rect, x, y, x + width, y + height);
        DrawText (hdc, psText, -1, &rect, format);
    }
}

static void ipOnDraw(HWND hwnd, HDC hdc)
{
    RECT rect;

    gal_pixel oldBrushColor = GetBrushColor(hdc);
    gal_pixel oldPenColor = GetPenColor(hdc);
    gal_pixel oldTextColor;
    int i, nItem;
  
    PIPITEM pItem;
    UINT text_format;
    int onFocus;

    PIPADDRESSDATA pData;	
	PCONTROL	  pCtrl = Control (hwnd);

    pData = (PIPADDRESSDATA) GetWindowAdditionalData2 (hwnd);

    if(!pData)
        return;

    onFocus = (pData->status & IPST_FOCUSED)? 1 : 0;

    oldTextColor = GetTextColor(hdc);
    SetBkMode (hdc, BM_TRANSPARENT);
	
    text_format = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
    GetClientRect (hwnd, &rect);

	if	(pCtrl->dwStyle & WS_DISABLED)
	{		
		SetBkColor(hdc,PIXEL_EDIT_BKG_DISABLE);
		SetBrushColor(hdc, PIXEL_EDIT_BKG_DISABLE);
	}
	else
	{		
		SetBkColor(hdc,PIXEL_EDIT_BKG_NORMAL);
		SetBrushColor(hdc, PIXEL_EDIT_BKG_NORMAL);
	}
#ifdef _ROUND_RECT_STYLE
	RoundRect (hdc, rect.left , rect.top, RECTW(rect)-1, RECTH(rect) -1, 3, 3);
#else
	//FillBox(hdc,rect.left , rect.top, rect.right ,rect.bottom);
#endif
    for(nItem = 0; nItem < pData->nItems; nItem++)
    {
		if (pCtrl->dwStyle & WS_DISABLED)
		{			
            SetTextColor(hdc, PIXEL_darkgray); // color front;
		}
		else
		{
	        if (nItem == pData->nItemSel)
	        {
	            if(onFocus)
	            {
	                SetTextColor (hdc, pData->color_ft_hilight); // color selected;
	                if (pData->items[nItem].isSel)
	                {
	                    SetBkColor (hdc, pData->color_bg_hilight);
	                    SetBkMode (hdc, BM_OPAQUE);
	                }
	            }
	            else
	            {
	                SetTextColor(hdc, pData->color_ft);
	                SetBkMode (hdc, BM_TRANSPARENT);
	            }
	        }
	        else
	        {
	            SetTextColor(hdc, pData->color_ft); // color front;
	            SetBkMode (hdc, BM_TRANSPARENT);
	        }
		}
        pItem = &pData->items[nItem];
        sDrawText (hdc, pItem->x , pItem->y, pItem->width, pItem->height, pItem->ip, text_format);
    }

	if	(pCtrl->dwStyle & WS_DISABLED)
		SetBrushColor (hdc, PIXEL_darkgray);
	else
    	SetBrushColor (hdc, PIXEL_black);
    for(i = 0; i < 3; i++)
    {
        //画圆点就用这个
        FillCircle (hdc, pData->dotStartx  + pData->dotWidth+(pData->dotLength * i) , pData->dotStarty+pData->dotHeight/2, 1);

    }

    SetBkMode (hdc, BM_OPAQUE);
    SetTextColor (hdc, oldTextColor);
    SetPenColor (hdc, oldPenColor);
    SetBrushColor (hdc, oldBrushColor);
   
    CreateCaret (hwnd, NULL, 1, pCtrl->pLogFont->size);

    return;
}

/*************************  data init  ************************/
static void InitIpAddressData (HWND hwnd, PIPADDRESSDATA pData,int width, int height)
{
    int i;
    int dot_width;
    int  ip_width = width;
    int  ip_height = height;

    /*#define BKC_EDIT_NORMAL         57
      #define FGC_EDIT_NORMAL         58
      #define BKC_EDIT_HILIGHT        59
      #define FGC_EDIT_HILIGHT        60
      #define BKC_EDIT_DISABLED       61
      #define FGC_EDIT_DISABLED       62

      #define WEC_FLAT_BORDER         33
      #define WEC_FLAT_BORDER_HILIGHT 50
    */
    PCONTROL pCtrl;

    pCtrl = Control(hwnd);

    pData->hWnd = hwnd;
    pData->dwStyle = GetWindowStyle(hwnd);//风格
    pData->caption = (char *)GetWindowCaption(hwnd);
    pData->data = GetWindowAdditionalData(hwnd);

    pData->nItems = 4;
    pData->nItemSel = 0;

    if(pData->caption)
    {
        ip2Items(pData->items,  pData->nItems, 0, pData->caption);
    }

    if(pData->data != 0)
    {
        printf("InitIpAddressData  pData->data = %d pCtrl->dwAddData  = %ld\n",pData->data,pCtrl->dwAddData);
        dot_width = pData->data;
    }
    else
        dot_width  = IP_DOT_WIDTH;
#if 0
    pData->color_bg = GetWindowElementColor(BKC_EDIT_NORMAL);//BKC_EDIT_NORMAL);
    pData->color_bg_hilight = GetWindowElementColor(BKC_EDIT_HILIGHT);
    pData->color_ft = GetWindowElementColor(FGC_EDIT_NORMAL);
    pData->color_ft_hilight = GetWindowElementColor(FGC_EDIT_HILIGHT);
    pData->color_boder = GetWindowElementColor(BKC_EDIT_NORMAL);
    pData->color_boder_hilight = GetWindowElementColor(WEC_FLAT_BORDER_HILIGHT);
#else
    //pData->color_bg = PIXEL_green;//BKC_EDIT_NORMAL);
    pData->color_bg =   PIXEL_lightwhite;
    pData->color_bg_hilight = 0xFFBEBEBE;

    pData->color_ft = PIXEL_black;
    pData->color_ft_hilight = PIXEL_black;

    pData->color_boder = BORDER_NORMAL_COLOR;
    pData->color_boder_hilight = BORDER_FOCUS_COLOR;
#endif
    SetWindowBkColor(hwnd, pData->color_bg);
    pData->dotWidth = dot_width;

    {
        int width_need, rect_width,rect_height;
        RECT rect;

        //pData->dotWidth = dot_width;
        width_need = width * 4 + pData->dotWidth * 3;//需要的最小宽度

        GetClientRect (hwnd, &rect);
        rect_width = rect.right - rect.left;//编辑框的实际宽度

        if( width_need  < rect_width )
        {
            ip_width = (rect_width - pData->dotWidth * 3)>>2;  //调整之后的IP宽度
        }

        //RECTH (rcDraw)  可以用这个
        rect_height = rect.bottom - rect.top;
        if( ip_height  < rect_height )
        {
            ip_height   = rect_height;
        }
    }
    pData->dotStartx =  ip_width;
    pData->dotStarty = 0;
    pData->dotHeight = ip_height;
    pData->dotLength = ip_width + pData->dotWidth;

    for(i = 0; i < pData->nItems; i++)
    {
        pData->items[i].x =  i*(ip_width + pData->dotWidth);
        pData->items[i].y = 0;
        pData->items[i].width = ip_width;
        pData->items[i].height = ip_height;
    }

    return;
}

//Destroies all the internal datas
static void IpAddressDataDestory (PIPADDRESSDATA pData)
{
    if(pData)
        free (pData);
}


static int get_nItem_withxy(PIPADDRESSDATA pData, int x, int y)
{
    int i;
    PIPITEM pItem;

    if(pData)
    {
        for(i = 0; i < pData->nItems; i++)
        {
            pItem = &pData->items[i];
            if( x >= pItem->x && x <= pItem->x + pItem->width &&
                    y >= pItem->y && y <= pItem->y + pItem->height)
                return i;
        }
    }

    return -1;
}


static void ipItemSefAdd(PIPADDRESSDATA pData)
{
    int i;
    PIPITEM pItem;
    int nItem;

    nItem = pData->nItemSel;

    pItem = &pData->items[nItem];

    i = atoi(pItem->ip);
    if(i >= 0 && i < (int)255)
    {
        i++;
    }
    else
    {
        i = 0;
    }

    snprintf(pItem->ip, 4, "%d", i);

    return;
}


static void ipItemSefDel(PIPADDRESSDATA pData)
{
    int i;
    PIPITEM pItem;
    int nItem;

    nItem = pData->nItemSel;

    pItem = &pData->items[nItem];

    i = atoi(pItem->ip);
    if(i > 0 && i <= (int)255)
    {
        i--;
    }
    else
    {
        i = 255;
    }

    snprintf(pItem->ip, 4, "%d", i);

    return;
}

static int  ipItemAddChar(PIPADDRESSDATA pData, char c)
{
    PIPITEM pItem;
    int nItem;
    int i, len;//,new_len;
    int ret = -1;
    if(!pData)
        return ret;

    nItem = pData->nItemSel;
    pItem = &pData->items[nItem];

    if (pItem->isSel)
    {
        pItem->isSel = 0;
        len = 0;
    }
    else
        len = strlen(pItem->ip);
#if 1
#if 0
    if(len >= 3)
    {
        pItem->ip[0] = pItem->ip[1];
        pItem->ip[1] = pItem->ip[2];
        pItem->ip[2] = c;
        pItem->ip[3] = '\0';
        //add
        printf("ipItemAddChar  len >= 3 len >= 3  !!!!!!!!!!! \n");
    }
#endif
    if(len >= 0 && len < 3)
    {
        pItem->ip[len++] = c;//执行后加一
        pItem->ip[len] = '\0';
    }
    while(1)
    {
        len = strlen(pItem->ip);
        if(len <= 1 || pItem->ip[0] != '0')
            break;

        for( i = 0; i < len; i++)
            pItem->ip[i] = pItem->ip[i+1];

    }
#endif
    i = atoi(pItem->ip);
    if(i < 0 || i > 255)
    {
        i = 255;
        snprintf(pItem->ip, 4, "%d", i);
        ret = 0;
    }
#if 1
    if(strlen(pItem->ip) >=3)
    {
        pData->nItemSel++;
        if(pData->nItemSel  >= pData->nItems)
            pData->nItemSel =  pData->nItems -1;
        else
            pData->items[pData->nItemSel].isSel = 1;
    }
#endif
    return   ret;
}

static int ipItemDelLastChar(PIPADDRESSDATA pData)
{
    PIPITEM pItem;
    int nItem;
    int len;
    int del_flag = 0;

    nItem = pData->nItemSel;
    pItem = &pData->items[nItem];

    len = strlen(pItem->ip);
    if(len == 1)
    {
        if(pItem->ip[0] != '0')
        {
            pItem->ip[0] = '0';
            pItem->ip[1] = '\0';
            del_flag = 1;
        }
    }

    else if(len > 1)
    {
        pItem->ip[len - 1] = '\0';
        del_flag = 1;
    }

    return del_flag;
}

/**************************************************************************/
/* sets the current caret position in the virtual content window */
static void myIPSetCaretPos (HWND hWnd, PIPADDRESSDATA pData, int x,int y)
{
#if 1
    /*
        if (x >= 0)
           sled->caret_x = x;
        else
           x = sled->caret_x;
    */
//    if (sleContentToVisible(sled, &x) < 0)
    if(0)//这里需要判断插入符是不是已经超出范围
    {
        HideCaret (hWnd);
    }
    else
    {
        // sleVisibleToWindow (sled, &x);
        SetCaretPos (hWnd, x,y);
        if (pData->status & IPST_FOCUSED)
        {
            ActiveCaret (hWnd);
            ShowCaret (hWnd);
        }
    }
#endif
}
/*
 * set caret position according to the new desired x coordinate.光标
 */
static void IPSetCaretPos (HWND hWnd, PIPADDRESSDATA pData, int x, BOOL bIP)
{
#if 1
    //int out_chars;
    int  caret_x = 0,caret_y = 0;
    int  margin_x = 0;//,margin_y = 0;

    //SIZE txtsize;
    PCONTROL pCtrl;
    pCtrl = Control(hWnd);

    PIPITEM pItem;
    int nItem;
    int text_len = 0;

    if(!pData)
        return;

    nItem = pData->nItemSel;
    pItem = &pData->items[nItem];
    text_len = strlen(pItem->ip);
    margin_x = (pItem->width - text_len*pCtrl->pLogFont->size)>>1;
    if(margin_x  < 0)
        margin_x  = 0;
    caret_x =  pItem->x + margin_x+text_len*pCtrl->pLogFont->size;
    if (caret_x > pItem->x +pItem->width  )
        caret_x =  pItem->x +pItem->width -1;
    caret_y = pItem->y  +(pItem->height >> 1) - 8;
    if (!bIP)
    {
        myIPSetCaretPos (hWnd, pData, caret_x,caret_y);
    }
    else
    {

    }

#endif
}


/**************  IpAddress_WinProc  *************/

static int IpAddress_WinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{

    PIPADDRESSDATA pData = NULL;
    DWORD       dwStyle;

    dwStyle  = GetWindowStyle(hWnd);//获取风格

    if (message != MSG_CREATE)
        pData = (PIPADDRESSDATA) GetWindowAdditionalData2 (hWnd);

    switch (message)
    {
    case MSG_CREATE://创建控件
    {		
		SetWindowFont(hWnd, GetSystemFont(SYSLOGFONT_CONTROL));
        if ( !(pData = (PIPADDRESSDATA)calloc(1, sizeof(IPADDRESSDATA))) )
            return -1;

        InitIpAddressData(hWnd, pData,  DEFAULT_IP_ITEMS_WIDTH, DEFAULT_IP_ITEMS_HEIGHT);

        SetWindowAdditionalData2 (hWnd, (DWORD) pData);//作为私有数据

        break;
    }

    case MSG_PAINT:
    {
        HDC hdc;

        hdc = BeginPaint (hWnd);
        ipOnDraw (hWnd, hdc);
        EndPaint (hWnd, hdc);
//		ShowCaret( hWnd);
        return 0;
    }
    break;

	case MSG_ENABLE:
	{
		PCONTROL	  pCtrl   = Control (hWnd);
        if (wParam && (pCtrl->dwStyle & WS_DISABLED))
            pCtrl->dwStyle &= ~WS_DISABLED;
        else if (!wParam && !(pCtrl->dwStyle & WS_DISABLED))
            pCtrl->dwStyle |= WS_DISABLED;
        else
            return 0;
        InvalidateRect (hWnd, NULL, TRUE);

        return 0;
	}
    case MSG_ERASEBKGND:
		EditOnEraseBackground (hWnd, (HDC) wParam, (const RECT*) lParam);
        return 0;

    case MSG_SETFOCUS:
        if(pData->status & IPST_FOCUSED)
            break;

        pData->status |= IPST_FOCUSED;
        pData->nItemSel = 3;
        IPSetCaretPos (hWnd, pData, LOSWORD(pData->items[pData->nItemSel].x), FALSE);
        pData->items[pData->nItemSel].isSel = 1;

        ShowCaret(hWnd);//显示插入符

        InvalidateRect (hWnd, NULL, TRUE);

        if (pData->dwStyle & IPS_NOTIFY)
            NotifyParent (hWnd, GetDlgCtrlID(hWnd), IP_SETFOCUS);
        break;

    case MSG_KILLFOCUS:
        HideCaret (hWnd);//隐藏插入符
        if(pData->status & IPST_FOCUSED)
        {
            pData->status &= (~IPST_FOCUSED);
            InvalidateRect (hWnd, NULL, TRUE);

            if (pData->dwStyle & IPS_NOTIFY)
                NotifyParent (hWnd, GetDlgCtrlID(hWnd), IP_KILLFOCUS);
        }

        break;

	case MSG_DOESNEEDIME:
		   if (dwStyle & ES_READONLY)
			   return FALSE;
		   return TRUE;

    case MSG_GETDLGCODE:
        return (DLGC_WANTCHARS | DLGC_WANTARROWS);

	case MSG_LBUTTONDBLCLK:
    case MSG_LBUTTONDOWN://需要处理插入符
    {
        int x_pos = LOSWORD (lParam);
        int y_pos = HISWORD (lParam);
        int nItem;

        nItem = get_nItem_withxy(pData, x_pos, y_pos);//第几个编辑框
        if(IPADDRESS_BE_VALID_ITEM(nItem))
        {
            pData->nItemSel = nItem;
            InvalidateRect (hWnd, NULL, TRUE);

            IPSetCaretPos (hWnd, pData, LOSWORD(lParam), FALSE);
            ActiveCaret (hWnd);
            ShowCaret(hWnd);

            pData->items[pData->nItemSel].isSel = 1;
        }
#ifdef IMA_CLICK_SHOW
         if (!(dwStyle&ES_READONLY))
        {
            SendNotifyMessage (__mg_ime_wnd, MSG_IME_LBD_SWITCH, hWnd, 0);
            NotifyParent (hWnd,  GetDlgCtrlID(hWnd), EN_DBLCLK);
        }
#endif
    }
    break;

#if 0	
#ifndef IMA_CLICK_SHOW  //显示输入法
    case MSG_LBUTTONDBLCLK:
    {
        if (!(dwStyle&ES_READONLY))
        {
            SendNotifyMessage (__mg_ime_wnd, MSG_IME_LBD_SWITCH, hWnd, 0);
            NotifyParent (hWnd,  GetDlgCtrlID(hWnd), EN_DBLCLK);
        }
        break;
    }
#endif
#endif
	case MSG_NCLBUTTONUP:
	 case MSG_LBUTTONUP:
		  if (GetCapture() == hWnd)
			  ReleaseCapture();
		  NotifyParent (hWnd,  GetDlgCtrlID(hWnd), EN_CLICKED);
	
		  //if (pedit->selStart != pedit->selEnd)
		  //  NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_SELCHANGED);
		  break;
	
	 case MSG_MOUSEMOVEIN:
	 {
		BOOL in_out = (BOOL)wParam;
	
		  if(in_out)
		  {
			  SetFocusChild (hWnd);
		  }
		  else
		  {
		  	if(!IsCustomImeOpen())
			  SetNullFocus (GetParent(hWnd));
		  }
	
		  break;
	
	 }
	  case MSG_MOUSEMOVE:
	  {
		  if (GetCapture () == hWnd)
			  IPSetCaretPos (hWnd, pData, 0, FALSE);
		  return 0;
	  }

    case MSG_KEYDOWN:
    {
        BOOL bChange = FALSE;
        int id = LOWORD (wParam);
        int nItem = -1;
        BOOL need_refresh = 1;
        switch (id)
        {
        case SCANCODE_HOME:
            nItem = 0;
            break;
        case SCANCODE_END:
            nItem = pData->nItems - 1;
            break;
        case SCANCODE_ENTER:
        {
            nItem = pData->nItemSel + 1;
            if(!(IPADDRESS_BE_VALID_ITEM(nItem)))
                nItem = 0;
        }
        break;

        case SCANCODE_BACKSPACE:
            if(ipItemDelLastChar(pData))
                bChange = TRUE;
            break;

        case SCANCODE_SPACE:
            nItem = pData->nItemSel;
            if(strcmp(pData->items[nItem].ip, "0"))
            {
                snprintf(pData->items[nItem].ip, 4, "0");
                bChange = TRUE;
            }
            break;

        case SCANCODE_CURSORBLOCKLEFT:
            nItem = pData->nItemSel - 1;
            break;
        case SCANCODE_CURSORBLOCKRIGHT:
            nItem = pData->nItemSel + 1;
            break;

        case SCANCODE_CURSORBLOCKUP:
            ipItemSefAdd(pData);
            bChange = TRUE;
            break;
        case SCANCODE_CURSORBLOCKDOWN:
            ipItemSefDel(pData);
            bChange = TRUE;
            break;

        default :
            need_refresh = 0;
            break;
        }

        if(need_refresh)
        {
            if(IPADDRESS_BE_VALID_ITEM(nItem))
            {
                pData->nItemSel = nItem;
                pData->items[pData->nItemSel].isSel = 1;
            }

            InvalidateRect (hWnd, NULL, TRUE);

            //add by jason 20110727
            IPSetCaretPos (hWnd, pData, LOSWORD(lParam), FALSE);
            ActiveCaret (hWnd);
            ShowCaret(hWnd);

            if(bChange)
                NotifyParent (hWnd, GetDlgCtrlID(hWnd), IP_CHANGE);
        }
    }
    break;

    case MSG_CHAR:
    {
        char c = LOBYTE(wParam);
        printf(">>>>>>>>>>>> IpAddress_WinProc  current c :%c \n",c);
        if(c >='0' &&  c <= '9' )
        {
            if(0 ==  ipItemAddChar(pData, c))
            {

                NotifyParent (hWnd, GetDlgCtrlID(hWnd), IPM_IP_ERROR);//提示超出范围

            }
            InvalidateRect (hWnd, NULL, TRUE);

            NotifyParent (hWnd, GetDlgCtrlID(hWnd), IP_CHANGE);
        }

        else if(c == '.')
        {
            int nItem = pData->nItemSel + 1;

            if(!(IPADDRESS_BE_VALID_ITEM(nItem)))
                nItem = 0;

            if(IPADDRESS_BE_VALID_ITEM(nItem))
            {
                pData->nItemSel = nItem;
                pData->items[pData->nItemSel].isSel = 1;
                InvalidateRect (hWnd, NULL, TRUE);
            }

        }

        if (wParam == 127) // BS
        {
            pData->items[pData->nItemSel].isSel = 0;
        }
        IPSetCaretPos (hWnd, pData, LOSWORD(lParam), FALSE);
        ActiveCaret (hWnd);
        ShowCaret(hWnd);
    }

    break;

    case MSG_DESTROY:
    {
        IpAddressDataDestory (pData);
        break;
    }

    case MSG_GETTEXTLENGTH:
    {
        char ipbuf[IP_LENGTH_MAX] = "";

        Items2ip(ipbuf, IP_LENGTH_MAX, pData->items, pData->nItems);

        return strlen(ipbuf);
    }
    case MSG_SETTEXT:
    {
        int len;
        char ipbuf[IP_LENGTH_MAX];

        if (lParam == 0)
            return -1;

        len = strlen ((char*)lParam);
        len = MIN (len, IP_LENGTH_MAX - 1);

		memset(ipbuf, 0, IP_LENGTH_MAX);
        memcpy (ipbuf, (char*)lParam, len);
		ipbuf[len + 1] = '\0';
        //ipbuf[IP_LENGTH_MAX - 1] = '\0';
        ip2Items(pData->items, pData->nItems, 0, ipbuf);
        InvalidateRect (hWnd, NULL, TRUE);

        return 0;
    }

    case IPM_SET_IPADDRESS:
    {
        int flag = (int)wParam;
        char *ipbuf = NULL;
        in_addr_t in = 0;

        if(flag == 0)
        {
            ipbuf = (char *)lParam; // pointer to IP buf
        }

        else
        {
            in = (in_addr_t)lParam; // unsigned long IP
        }

        ip2Items(pData->items, pData->nItems, in, ipbuf);

        InvalidateRect (hWnd, NULL, TRUE);
    }

    break;

    case MSG_GETTEXT:
    {
        char *buffer = (char*)lParam;
        int  len;

        //in_addr_t tmp_ip;
        char ipbuf[IP_LENGTH_MAX];

        len = MIN ((int)wParam, IP_LENGTH_MAX - 1);

        /*tmp_ip =*/
		Items2ip(ipbuf, IP_LENGTH_MAX, pData->items, pData->nItems);

        memcpy(buffer, ipbuf, len);
        buffer[len] = '\0';

        return len;
    }

    case IPM_GET_IPADDRESS:
    {
        char ipbuf[IP_LENGTH_MAX];
        char *buffer = NULL;
        int len;
        in_addr_t tmp_ip;
        in_addr_t *to_ip = NULL;

        tmp_ip =Items2ip(ipbuf, IP_LENGTH_MAX, pData->items, pData->nItems);

        if(wParam != 0)
        {
            buffer = (char *)lParam;
            len = MIN((int)wParam, IP_LENGTH_MAX - 1);
            if(buffer)
            {
                memcpy(buffer, ipbuf, len);
                buffer[len] = '\0';
            }

        }

        else
        {
            to_ip = (in_addr_t *)lParam;
            if(to_ip)
                *to_ip = tmp_ip;
        }
    }

    break;

    default :
        break;
    }

    return DefaultControlProc (hWnd, message, wParam, lParam);
}

BOOL RegisterIpAddressControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_IP_ADDRESS;
    WndClass.dwStyle = WS_NONE;
    WndClass.dwExStyle = WS_EX_NONE;
    WndClass.hCursor = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor = PIXEL_lightwhite;
    WndClass.WinProc = IpAddress_WinProc;
    return RegisterWindowClass (&WndClass);
}

void IpAddressControlCleanup (void)
{
    UnregisterWindowClass (CTRL_IP_ADDRESS);
}

#endif /* _UNDEFINE_CTRL_IPADDRESS_ */
