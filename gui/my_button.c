/*
** $Id: button.c 8267 2007-11-27 06:27:34Z weiym $
**
** button.c: the Button Control module.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
**
** Current maintainer: Wang Xiaogang.
**
** Create date: 1999/8/23
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>



#include <minigui/cliprect.h>
#include <minigui/internals.h>
#include <minigui/ctrlclass.h>
#include "my_button.h"
#include <minigui/fixedmath.h>


#define myDrawButton btnDrawFashionButton


#define BTN_WIDTH_BMP       16
#define BTN_HEIGHT_BMP      16
#define BTN_INTER_BMPTEXT   2


#define BUTTON_STATUS(pCtrl)   (((PMYBUTTONDATA)(pCtrl->dwAddData2))->status)
#define BUTTON_DATA(pCtrl)     (((PMYBUTTONDATA)(pCtrl->dwAddData2))->data)

static const BITMAP *bmp_button;
static BITMAP default_check_button;
static BITMAP default_radio_button;

#define BUTTON_BMP              bmp_button

#define BUTTON_OFFSET      1

static int MyButtonCtrlProc (HWND hWnd, int uMsg, WPARAM wParam, LPARAM lParam);

BOOL RegisterMyButtonCTRL (void)
{
	//int ret;
    WNDCLASS WndClass;

    if ((bmp_button = GetStockBitmap (STOCKBMP_BUTTON, 0, 0)) == NULL)
    {
        printf("GetStockBitmap failure! STOCKBMP_BUTTON\n");
        return FALSE;
    }

	LoadBitmapFromFile(HDC_SCREEN, &default_check_button ,"nvr_res/ctrl_check.bmp");	
	LoadBitmapFromFile(HDC_SCREEN, &default_radio_button ,"nvr_res/ctrl_radio.bmp");

    WndClass.spClassName = MY_BUTTONEX;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = GetWindowElementColor (BKC_CONTROL_DEF); 
    WndClass.WinProc     = MyButtonCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

void MyButtonCTRLCleanup (void)
{
	UnloadBitmap(&default_check_button);	
	UnloadBitmap(&default_radio_button);
}


static void draw_fashion_border(HDC hdc, int left, int top, 
                                int right, int bottom, int corner)
{
     SetPenColor(hdc, PIXEL_black);
     MoveTo(hdc, left+corner, top);
     LineTo(hdc, right-corner, top);
     LineTo(hdc, right, top+corner);
     LineTo(hdc, right, bottom-corner);
     LineTo(hdc, right-corner, bottom);
     LineTo(hdc, left+corner, bottom);
     LineTo(hdc, left, bottom-corner);
     LineTo(hdc, left, top+corner);
     LineTo(hdc, left+corner, top);

}

typedef struct _fixed_rgb
{
    fixed r;
    fixed g;
    fixed b;

}FIXED_RGB;

static FIXED_RGB calc_drgb(RGB rgb1, RGB rgb2, int dist)
{
    FIXED_RGB drgb;
    fixed d;

    if (dist == 0){
        drgb.r = rgb2.r << 16;
        drgb.g = rgb2.g << 16;
        drgb.b = rgb2.b << 16;

        return drgb;
    }

    d = abs(dist) << 16;

    drgb.r = fixdiv((rgb1.r - rgb2.r) << 16, d);
    drgb.g = fixdiv((rgb1.g - rgb2.g) << 16, d);
    drgb.b = fixdiv((rgb1.b - rgb2.b) << 16, d);

    return drgb;

}

#define INCRGB(crgb, drgb) \
          crgb.r += drgb.r;\
          crgb.g += drgb.g;\
          crgb.b += drgb.b 

#define INITRGB(rgb, rdata, gdata, bdata)\
                           rgb.r = rdata;\
                           rgb.g = gdata;\
                           rgb.b = bdata

#define FixedRGB2Pixel(hdc, r, g, b)\
          RGB2Pixel(hdc, r>>16, g>>16, b>>16)

static void btnDrawFashionButton(HDC hdc, HWND hwnd, int left, int top ,int right,
                               int bottom, DWORD flags, gal_pixel fillc)                            
{
    int i = 0;
    int l, r, t, b;
    FIXED_RGB drgb, crgb;
    RGB rgb1, rgb2;
    int corner = 2;


    l = left;
    t = top;
    r = right;
    b = bottom;

	int sr, sg, sb;
	int er, eg, eb;

	if ((flags & DF_3DBOX_STATEMASK) == DF_3DBOX_DISABLE)
	{		
		sr = 0x44, sg = 0x44, sb = 0x44;
		er = 0x66, eg = 0x66, eb = 0x66;
	}
    else if ((flags & DF_3DBOX_STATEMASK) == DF_3DBOX_FOCUS)
	{		
		sr = 0x33, sg = 0x33, sb = 0x33;		
		er = 0x66, eg = 0x66, eb = 0x66;
    }
	else if ((flags & DF_3DBOX_STATEMASK) == DF_3DBOX_PRESSED)
	{
		sr = 0x00, sg = 0x00, sb = 0x00;		
		er = 0x11, eg = 0x11, eb = 0x11;
	}
	else// if ((flags & DF_3DBOX_STATEMASK) == DF_3DBOX_NORMAL)
	{
		sr = 0x00, sg = 0x00, sb = 0x00;
		er = 0x33, eg = 0x33, eb = 0x33;
    }

	INITRGB(rgb1, sr, sg, sb);
    INITRGB(rgb2, er, eg, eb);
    INITRGB(crgb, er<<16, eg<<16, eb<<16);

    if ((flags & DF_3DBOX_STATEMASK) == DF_3DBOX_NORMAL)
	{/* push button color */
       
        drgb = calc_drgb(rgb1, rgb2, bottom - top); 
    }
    else
	{/* pushed button color */
        drgb = calc_drgb(rgb1, rgb2, b/2); 
    }

    for (i=b/2; i>=t+corner; i--)
	{
        SetPenColor(hdc, FixedRGB2Pixel(hdc, crgb.r, crgb.g, crgb.b));
        MoveTo(hdc, l, i);
        LineTo(hdc, r, i);

        INCRGB(crgb, drgb);
    }

    SetPenColor(hdc, FixedRGB2Pixel(hdc, crgb.r, crgb.g, crgb.b));
    MoveTo(hdc, l+1, t + 1);
    LineTo(hdc, r-2, t + 1);

    INCRGB(crgb, drgb);
    SetPenColor(hdc, FixedRGB2Pixel(hdc, crgb.r, crgb.g, crgb.b));
    MoveTo(hdc, l+2, t);
    LineTo(hdc, r-3, t);
	
	INITRGB(crgb, er<<16, eg<<16, eb<<16);

    //if ((flags & DF_3DBOX_STATEMASK) == DF_3DBOX_NORMAL){/* push button color */
    //    INITRGB(crgb, 0x33<<16, 0x33<<16, 0x33<<16);
    //}
    //else{/* pushed button color */
    //    INITRGB(crgb, 0x98<<16, 0xb4<<16, 0xff<<16);
    //}

    for (i=b/2+1; i<b-corner; i++)
	{
        SetPenColor(hdc, FixedRGB2Pixel(hdc, crgb.r, crgb.g, crgb.b));
        MoveTo(hdc, l, i);
        LineTo(hdc, r, i);

        INCRGB(crgb, drgb);
    }

    SetPenColor(hdc, FixedRGB2Pixel(hdc, crgb.r, crgb.g, crgb.b));
    MoveTo(hdc, l+1, b-2);
    LineTo(hdc, r-2, b-2);

    INCRGB(crgb, drgb);
    SetPenColor(hdc, FixedRGB2Pixel(hdc, crgb.r, crgb.g, crgb.b));
    MoveTo(hdc, l+2, b-1);
    LineTo(hdc, r-3, b-1);

    draw_fashion_border(hdc, left, top, right-1, bottom-1, corner);
}
static void btnGetRects (HWND hWnd, DWORD dwStyle,
                                    RECT* prcClient, 
                                    RECT* prcText, 
                                    RECT* prcBitmap)
{
    GetClientRect (hWnd, prcClient);
    
#ifndef _PHONE_WINDOW_STYLE
    prcClient->right --;
    prcClient->bottom --;
#endif

    SetRect (prcText, (prcClient->left   + BTN_WIDTH_BORDER),
                      (prcClient->top    + BTN_WIDTH_BORDER),
                      (prcClient->right  - BTN_WIDTH_BORDER),
                      (prcClient->bottom - BTN_WIDTH_BORDER));

    SetRectEmpty (prcBitmap);

    if ( ((dwStyle & BS_TYPEMASK) < BS_CHECKBOX)
        || (dwStyle & BS_PUSHLIKE))
        return;

    if (dwStyle & BS_LEFTTEXT) {
        SetRect (prcText, prcClient->left + 1,
                          prcClient->top + 1,
                          prcClient->right - BTN_WIDTH_BMP - BTN_INTER_BMPTEXT,
                          prcClient->bottom - 1);
        SetRect (prcBitmap, prcClient->right - BTN_WIDTH_BMP,
                            prcClient->top + 1,
                            prcClient->right - 1,
                            prcClient->bottom - 1);
    }
    else {
        SetRect (prcText, prcClient->left + BTN_WIDTH_BMP + BTN_INTER_BMPTEXT,
                          prcClient->top + 1,
                          prcClient->right - 1,
                          prcClient->bottom - 1);
        SetRect (prcBitmap, prcClient->left + 1,
                            prcClient->top + 1,
                            prcClient->left + BTN_WIDTH_BMP,
                            prcClient->bottom - 1);
    }

}


static void btnPaintContent (PCONTROL pCtrl, HDC hdc, DWORD dwStyle, 
                             RECT* prcText)
{
    BOOL pushbutton = FALSE;
    RECT tmp_rc;

    tmp_rc.top = prcText->top - BUTTON_OFFSET;
    tmp_rc.left = prcText->left;
    tmp_rc.right = prcText->right;
    tmp_rc.bottom = prcText->bottom;

    switch (dwStyle & BS_CONTENTMASK)
    {
        case BS_TEXT:
        case BS_LEFTTEXT:
        {
            UINT uFormat;
            
            if (dwStyle & BS_MULTLINE)
                uFormat = DT_WORDBREAK;
            else
                uFormat = DT_SINGLELINE;

            if ((dwStyle & BS_TYPEMASK) == BS_PUSHBUTTON
                    || (dwStyle & BS_TYPEMASK) == BS_DEFPUSHBUTTON
                    || (dwStyle & BS_PUSHLIKE)) {
                pushbutton = TRUE;
                uFormat |= DT_CENTER | DT_VCENTER;
            }
            else 
			{

                if ((dwStyle & BS_ALIGNMASK) == BS_RIGHT)
                    uFormat = DT_WORDBREAK | DT_RIGHT;
                else if ((dwStyle & BS_ALIGNMASK) == BS_CENTER)
                    uFormat = DT_WORDBREAK | DT_CENTER;
                else uFormat = DT_WORDBREAK | DT_LEFT;
            
                if ((dwStyle & BS_ALIGNMASK) == BS_TOP)
                    uFormat |= DT_SINGLELINE | DT_TOP;
                else if ((dwStyle & BS_ALIGNMASK) == BS_BOTTOM)
                    uFormat |= DT_SINGLELINE | DT_BOTTOM;
                else uFormat |= DT_SINGLELINE | DT_VCENTER;
            }
            
            SetBkColor (hdc, GetWindowBkColor ((HWND)pCtrl));
            if (dwStyle & WS_DISABLED) {
                /* RECT rc = *prcText; */
                
                SetBkMode (hdc, BM_TRANSPARENT);
                SetTextColor (hdc, GetWindowElementColorEx ((HWND)pCtrl, FGC_CONTROL_DISABLED));
                DrawText (hdc, pCtrl->spCaption, -1, &tmp_rc/*&rc*/, uFormat);
            }
            else 
			{
                SetBkMode (hdc, BM_TRANSPARENT);
                if (pushbutton && (BUTTON_STATUS(pCtrl) & BST_PUSHED
                            || BUTTON_STATUS(pCtrl) & BST_CHECKED)) {
                    SetTextColor (hdc, GetWindowElementColorEx ((HWND)pCtrl, FGC_BUTTON_PUSHED));				
                    SetTextColor (hdc, PIXEL_lightwhite);
                }
                else {
                    SetTextColor (hdc, GetWindowElementColorEx ((HWND)pCtrl, FGC_BUTTON_NORMAL));					
                    SetTextColor (hdc, PIXEL_lightwhite);
                }
                /*OffsetRect (prcText, 1, 1);*/

				//printf("draw text %s##########\r\n", pCtrl->spCaption);
                DrawText (hdc, pCtrl->spCaption, -1, &tmp_rc/*prcText*/, uFormat);
            }
        }
        break;
    }
}


static void btnDrawPushButton(PCONTROL pCtrl, HDC hdc, DWORD dwStyle,
                RECT* prcClient, RECT* prcText, RECT* prcBitmap)
{
	if (dwStyle & WS_DISABLED) 
	{
		myDrawButton (hdc, (HWND)pCtrl, prcClient->left, prcClient->top,
                                prcClient->right, prcClient->bottom, 
                                DF_3DBOX_DISABLE | DF_3DBOX_FILL, 
                                GetWindowBkColor ((HWND)pCtrl));
		return;
	}
	
	if (BUTTON_STATUS(pCtrl) & BST_PUSHED)
	{ 
		myDrawButton (hdc, (HWND)pCtrl, prcClient->left, prcClient->top,
                                prcClient->right, prcClient->bottom, 
                                DF_3DBOX_PRESSED | DF_3DBOX_FILL, 
                                GetWindowBkColor ((HWND)pCtrl));
	}
	else if (BUTTON_STATUS(pCtrl) & BST_FOCUS)
	{
		 myDrawButton (hdc, (HWND)pCtrl, prcClient->left + 1, prcClient->top + 1,
                                prcClient->right - 1, prcClient->bottom - 1, 
                                DF_3DBOX_FOCUS | DF_3DBOX_FILL, 
                                GetWindowBkColor ((HWND)pCtrl));
	}
	else
	{
		myDrawButton (hdc, (HWND)pCtrl, prcClient->left + 1, prcClient->top + 1,
                                prcClient->right - 1, prcClient->bottom - 1, 
                                DF_3DBOX_NORMAL| DF_3DBOX_FILL, 
                                GetWindowBkColor ((HWND)pCtrl));
	}
	
}


static void btnDrawCheckButton(PCONTROL pCtrl, HDC hdc, DWORD dwStyle,
                RECT* prcClient, RECT* prcText, RECT* prcBitmap)
{
	int bmp_left = prcBitmap->left;
	int bmp_top = prcBitmap->top + (prcBitmap->bottom - BTN_HEIGHT_BMP) / 2;

	if (BUTTON_STATUS(pCtrl) & BST_CHECKED)
	{
		if (dwStyle & WS_DISABLED) 
		{
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										15, 15,
										0, 0,
										&default_check_button,
										7*15, 0);
			return;
		}
		if (BUTTON_STATUS(pCtrl) & BST_PUSHED)
		{
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										15, 15,
										0, 0,
										&default_check_button,
										6*15, 0);
		}
		else if (BUTTON_STATUS(pCtrl) & BST_FOCUS)
		{
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										15, 15,
										0, 0,
										&default_check_button,
										5*15, 0);
		}	
		else
		{	
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										15, 15,
										0, 0,
										&default_check_button,
										4*15, 0);
		}
	}
	else
	{
		if (dwStyle & WS_DISABLED) 
		{
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										15, 15,
										0, 0,
										&default_check_button,
										3*15, 0);
			return;
		}
		
		if (BUTTON_STATUS(pCtrl) & BST_PUSHED)
		{
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										15, 15,
										0, 0,
										&default_check_button,
										2*15, 0);
		}
		else if (BUTTON_STATUS(pCtrl) & BST_FOCUS)
		{
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										15, 15,
										0, 0,
										&default_check_button,
										15, 0);
		}	
		else
		{	
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										15, 15,
										0, 0,
										&default_check_button,
										0, 0);
		}
	}
}


static void btnDrawRadioButton(PCONTROL pCtrl, HDC hdc, DWORD dwStyle,
                RECT* prcClient, RECT* prcText, RECT* prcBitmap)
{
	int bmp_left = prcBitmap->left;
	int bmp_top = prcBitmap->top + (prcBitmap->bottom - 25) / 2;

	default_radio_button.bmType = BMP_TYPE_COLORKEY;
	default_radio_button.bmColorKey = GetPixelInBitmap(&default_radio_button, 0, 0);

	if (BUTTON_STATUS(pCtrl) & BST_CHECKED)
	{
		if (dwStyle & WS_DISABLED) 
		{
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										25, 25,
										0, 0,
										&default_radio_button,
										7*25, 0);
			return;
		}
		if (BUTTON_STATUS(pCtrl) & BST_PUSHED)
		{
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										25, 25,
										0, 0,
										&default_radio_button,
										6*25, 0);
		}
		else if (BUTTON_STATUS(pCtrl) & BST_FOCUS)
		{
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										25, 25,
										0, 0,
										&default_radio_button,
										5*25, 0);
		}	
		else
		{	
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										25, 25,
										0, 0,
										&default_radio_button,
										4*25, 0);
		}
	}
	else
	{
		if (dwStyle & WS_DISABLED) 
		{
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										25, 25,
										0, 0,
										&default_radio_button,
										3*25, 0);
			return;
		}
		
		if (BUTTON_STATUS(pCtrl) & BST_PUSHED)
		{
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										25, 25,
										0, 0,
										&default_radio_button,
										2*25, 0);
		}
		else if (BUTTON_STATUS(pCtrl) & BST_FOCUS)
		{
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										25, 25,
										0, 0,
										&default_radio_button,
										25, 0);
		}	
		else
		{	
			FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
										25, 25,
										0, 0,
										&default_radio_button,
										0, 0);
		}
	}
}

static void btnDrawPicRadioButton(PCONTROL pCtrl, HDC hdc, DWORD dwStyle,
                RECT* prcClient, RECT* prcText, RECT* prcBitmap)
{
	//UINT uFormat = 0;
	PBITMAP bmp = (PBITMAP)(BUTTON_DATA(pCtrl));
	int pic_w = bmp->bmWidth/3;
	int pic_h = bmp->bmHeight;
	int client_w = prcClient->right - prcClient->left;
	RECT textRect, picRect;
	gal_pixel ft_color;
	
	bmp->bmType = BMP_TYPE_COLORKEY;
	bmp->bmColorKey = GetPixelInBitmap(bmp, 0, 0);

    if(pCtrl->spCaption)
    {		
        SetRect(&textRect, prcClient->left,  prcClient->bottom - 20, prcClient->right, prcClient->bottom);
	}
	SetRect(&picRect,prcClient->left + ((client_w -pic_w)>>1), prcClient->top+2 ,
		prcClient->left + ((client_w -pic_w)>>1) + pic_w, prcClient->top+ pic_h + 2);
	
	if ((BUTTON_STATUS(pCtrl) & BST_CHECKED)||(BUTTON_STATUS(pCtrl) & BST_PUSHED))
	{
		FillBoxWithBitmapPart (hdc, picRect.left, picRect.top,
									pic_w, pic_h,
									0, 0,
									bmp,
									pic_w<<1, 0);
		ft_color = 0xFF00AEEF;//按钮选中时高亮
	}
	else if (BUTTON_STATUS(pCtrl) & BST_FOCUS)
	{		
		FillBoxWithBitmapPart (hdc, picRect.left, picRect.top,
									pic_w, pic_h,
									0, 0,
									bmp,
									pic_w, 0);
		ft_color = 0xFF00AEEF;//按钮获得焦点时高亮
	}
	else
	{				
		FillBoxWithBitmapPart (hdc, picRect.left, picRect.top,
									pic_w, pic_h,
									0, 0,
									bmp,
									0, 0);
		ft_color = PIXEL_lightwhite;
	}
	
	SetBkMode (hdc, BM_TRANSPARENT);
    SetTextColor (hdc, ft_color);
    //uFormat |= DT_CENTER|DT_VCENTER|DT_SINGLELINE;
    DrawText (hdc, pCtrl->spCaption, -1, &textRect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

}
	 
static void	 btnDrawPicPushButton(PCONTROL pCtrl, HDC hdc, DWORD dwStyle,
                RECT* prcClient, RECT* prcText, RECT* prcBitmap)
{
	PBITMAP bmp = (PBITMAP)(BUTTON_DATA(pCtrl));
	HDC memDC = hdc;//CreateCompatibleDC(hdc);
	int pic_w = bmp->bmWidth>>2;
	int pic_h = bmp->bmHeight;	
	gal_pixel ft_color;
	
    bmp->bmType = BMP_TYPE_COLORKEY;
	bmp->bmColorKey = 0xff08243d;//GetPixelInBitmap(bmp, 0, 0);//0xff4b4b4b;//
	
	SetBkMode (memDC, BM_TRANSPARENT);
	if (dwStyle & WS_DISABLED) 
	{
		FillBoxWithBitmapPart (memDC, 0, 0,
									pic_w, pic_h,
									0, 0,
									bmp,
									pic_w*3, 0);		
		ft_color = PIXEL_darkgray; //按钮选中时高亮
	}
	else if (BUTTON_STATUS(pCtrl) & BST_PUSHED)
	{
		FillBoxWithBitmapPart (memDC, 0, 0,
									pic_w, pic_h,
									0, 0,
									bmp,
									pic_w<<1, 0);		
		ft_color = PIXEL_lightwhite;//0xFF00AEEF;//按钮选中时高亮
	}
	else if (BUTTON_STATUS(pCtrl) & BST_FOCUS)
	{		
		FillBoxWithBitmapPart (memDC, 0, 0,
									pic_w, pic_h,
									0, 0,
									bmp,
									pic_w, 0);	
		ft_color = PIXEL_lightwhite; //按钮选中时高亮
	}
	else
	{		
		FillBoxWithBitmapPart (memDC, 0, 0,
									pic_w, pic_h,
									0, 0,
									bmp,
									0, 0);
		ft_color = PIXEL_lightwhite;
	}
    SetTextColor (memDC, ft_color);
    DrawText (memDC, pCtrl->spCaption, -1, prcClient/*&rc*/,DT_SINGLELINE|DT_CENTER | DT_VCENTER);
	
	StretchBlt(memDC,0,0,
				pic_w, pic_h,
				hdc, prcClient->left,prcClient->top,
				prcClient->right - prcClient->left, 
				prcClient->bottom - prcClient->top,0);
		
	//DeleteCompatibleDC(memDC);
}


static void btnPaintbutton(PCONTROL pCtrl, HDC hdc, DWORD dwStyle,
                RECT* prcClient, RECT* prcText, RECT* prcBitmap)
{
	 switch (dwStyle & BS_TYPEMASK)
	 {
		case BS_DEFPUSHBUTTON:
		case BS_PUSHBUTTON:	
			if((dwStyle &BS_BITMAP)||(dwStyle & BS_ICON))
				btnDrawPicPushButton(pCtrl, hdc, dwStyle, prcClient,  prcText, prcBitmap);
			else
				btnDrawPushButton(pCtrl, hdc, dwStyle, prcClient,  prcText, prcBitmap);
			break;
		case BS_3STATE:
		case BS_AUTO3STATE:
		case BS_AUTOCHECKBOX:
		case BS_CHECKBOX:
			btnDrawCheckButton(pCtrl, hdc, dwStyle, prcClient,  prcText, prcBitmap);
			break;
		case BS_AUTORADIOBUTTON:
		case BS_RADIOBUTTON:			
			if((dwStyle &BS_BITMAP)||(dwStyle & BS_ICON))
				btnDrawPicRadioButton(pCtrl, hdc, dwStyle, prcClient,	prcText, prcBitmap);
			else
				btnDrawRadioButton(pCtrl, hdc, dwStyle, prcClient,  prcText, prcBitmap);
			break;
	 }
}

static int btnUncheckOthers (PCONTROL pCtrl)
{
    PCONTROL pGroupLeader = pCtrl;
    PCONTROL pOthers;
    DWORD    type = pCtrl->dwStyle & BS_TYPEMASK;

    while (pGroupLeader) {
        if (pGroupLeader->dwStyle & WS_GROUP)
            break;

        pGroupLeader = pGroupLeader->prev;
    }

    pOthers = pGroupLeader;
    while (pOthers) {
        
        if ((pOthers->dwStyle & BS_TYPEMASK) != type)
            break;
        
        //if ((pOthers != pCtrl) && (BUTTON_STATUS (pOthers) | BST_CHECKED)) {
        if ((pOthers != pCtrl) && (BUTTON_STATUS (pOthers))) {
            BUTTON_STATUS (pOthers) &= ~BST_CHECKED;
            InvalidateRect ((HWND)pOthers, NULL, TRUE);
        }

        pOthers = pOthers->next;
    }
    return pCtrl->id;
}

static int MyButtonCtrlProc (HWND hWnd, int uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC         hdc;
    PCONTROL    pCtrl;
    DWORD       dwStyle;
    RECT        rcClient;
    RECT        rcText;
    RECT        rcBitmap;
    PMYBUTTONDATA pData;

    pCtrl   = Control(hWnd);
    dwStyle = pCtrl->dwStyle;

    switch(uMsg)
    {
        case MSG_CREATE:
            pData = (MYBUTTONDATA*) calloc (1, sizeof(MYBUTTONDATA));
            if (pData == NULL) 
                return -1;

            pData->status = 0;
            pData->data = pCtrl->dwAddData;
            pCtrl->dwAddData2 = (DWORD) pData;

            switch (dwStyle & BS_TYPEMASK) {
                case BS_PUSHBUTTON:
                case BS_DEFPUSHBUTTON:
                    break;

                default:
                    if (dwStyle & BS_CHECKED)
                        pData->status |= BST_CHECKED;
                    break;
            }

        break;
       
        case MSG_DESTROY:
            free ((void*) pCtrl->dwAddData2);
        break;
        
		case MSG_SIZECHANGING:
		{
			const RECT* rcExpect = (const RECT*)wParam;
			RECT* rcResult = (RECT*)lParam;

            switch (pCtrl->dwStyle & BS_TYPEMASK)
            {
            case BS_CHECKBOX:
            case BS_RADIOBUTTON:
            case BS_AUTOCHECKBOX:
            case BS_AUTORADIOBUTTON:
            case BS_3STATE:
            case BS_AUTO3STATE:
                if (dwStyle & BS_PUSHLIKE) {
                    rcResult->left = rcExpect->left;
                    rcResult->top = rcExpect->top;
                    rcResult->right = rcExpect->right;
                    rcResult->bottom = rcExpect->bottom;
                    return 0;
                }
            case BS_PUSHBUTTON:
            case BS_DEFPUSHBUTTON:
                rcResult->left = rcExpect->left;
                rcResult->top = rcExpect->top;
                rcResult->right = rcExpect->right;
                rcResult->bottom = rcExpect->bottom; 
                return 0;
            }
            break;
		}

        case MSG_SIZECHANGED:
        {
            RECT* rcWin = (RECT*)wParam;
            RECT* rcClient = (RECT*)lParam;
            switch (pCtrl->dwStyle & BS_TYPEMASK)
            {
                case BS_PUSHBUTTON:
                case BS_DEFPUSHBUTTON:
                    *rcClient = *rcWin;
                    return 1;
            }
            break;
        }

        case BM_GETCHECK:
            switch (dwStyle & BS_TYPEMASK) {
                case BS_AUTOCHECKBOX:
                case BS_AUTORADIOBUTTON:
                case BS_CHECKBOX:
                case BS_RADIOBUTTON:
                    return (BUTTON_STATUS (pCtrl) & BST_CHECKED);
                
                case BS_3STATE:
                case BS_AUTO3STATE:
                    if (BUTTON_STATUS (pCtrl) & BST_INDETERMINATE)
                        return (BST_INDETERMINATE);
                    return (BUTTON_STATUS (pCtrl) & BST_CHECKED);
                
                default:
                    return 0;
            }
        break;
        
        case BM_GETSTATE:
            return (int)(BUTTON_STATUS (pCtrl));

        case BM_GETIMAGE:
        {
            int* image_type = (int*) wParam;

            if (dwStyle & BS_BITMAP) {
                if (image_type) {
                    *image_type = BM_IMAGE_BITMAP;
                }
                return (int)(BUTTON_DATA (pCtrl));
            }
            else if (dwStyle & BS_ICON) {
                if (image_type) {
                    *image_type = BM_IMAGE_ICON;
                }
                return (int)(BUTTON_DATA (pCtrl));
            }
        }
        return 0;
        
        case BM_SETIMAGE:
        {
            int oldImage = (int)BUTTON_DATA (pCtrl);

            if (wParam == BM_IMAGE_BITMAP) {
                pCtrl->dwStyle &= ~BS_ICON;
                pCtrl->dwStyle |= BS_BITMAP;
            }
            else if (wParam == BM_IMAGE_ICON){
                pCtrl->dwStyle &= ~BS_BITMAP;
                pCtrl->dwStyle |= BS_ICON;
            }

            if (lParam) {
                 BUTTON_DATA (pCtrl) = (DWORD)lParam;
                 InvalidateRect (hWnd, NULL, TRUE);
            }

            return oldImage;
        }
        break;

        case BM_CLICK:
        {
            DWORD dwState;
            
            switch (pCtrl->dwStyle & BS_TYPEMASK)
            {
                case BS_AUTORADIOBUTTON:
                    if (!(BUTTON_STATUS (pCtrl) & BST_CHECKED))
                        SendMessage (hWnd, BM_SETCHECK, BST_CHECKED, 0);
                break;
                    
                case BS_AUTOCHECKBOX:
                    if (BUTTON_STATUS (pCtrl) & BST_CHECKED)
                        dwState = BST_UNCHECKED;
                    else
                        dwState = BST_CHECKED;
                                
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)dwState, 0);
                break;
                    
                case BS_AUTO3STATE:
                    dwState = (BUTTON_STATUS (pCtrl) & 0x00000003L);
                    dwState = BST_INDETERMINATE - dwState;
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)dwState, 0);
                break;
    
                case BS_PUSHBUTTON:
                case BS_DEFPUSHBUTTON:
                break;
            }
                
            NotifyParent (hWnd, pCtrl->id, BN_CLICKED);
            InvalidateRect (hWnd, NULL, TRUE);
        }
        break;
        
        case MSG_SETTEXT:
        {
            int len = strlen((char*)lParam);
            FreeFixStr (pCtrl->spCaption);
            pCtrl->spCaption = FixStrAlloc (len);
            if (len > 0)
                strcpy (pCtrl->spCaption, (char*)lParam);
            InvalidateRect (hWnd, NULL, TRUE);
            break;
        }

        case BM_SETCHECK:
        {
            DWORD dwOldState = BUTTON_STATUS (pCtrl);

            switch (dwStyle & BS_TYPEMASK) {
                case BS_CHECKBOX:
                case BS_AUTOCHECKBOX:
                    if (wParam & BST_CHECKED)
                        BUTTON_STATUS(pCtrl) |= BST_CHECKED;
                    else
                        BUTTON_STATUS(pCtrl) &= ~BST_CHECKED;
                break;
            
                case BS_AUTORADIOBUTTON:
                case BS_RADIOBUTTON:
                    if ( ((BUTTON_STATUS(pCtrl) & BST_CHECKED) == 0)
                            && (wParam & BST_CHECKED) ) {
                        BUTTON_STATUS(pCtrl) |= BST_CHECKED;
                        
                        btnUncheckOthers (pCtrl);
                    }
                    else if ( (BUTTON_STATUS(pCtrl) & BST_CHECKED)
                            && (!(wParam & BST_CHECKED)) ) {
                        BUTTON_STATUS(pCtrl) &= ~BST_CHECKED;
                    }
                break;
            
                case BS_3STATE:
                case BS_AUTO3STATE:
                    switch (wParam) { 
                        case BST_INDETERMINATE:
                        case BST_CHECKED:
                        case BST_UNCHECKED:
                            BUTTON_STATUS(pCtrl) &= 0xFFFFFFFC;
                            BUTTON_STATUS(pCtrl) += (wParam & 0x00000003);
                    }
                break;
            }
                
            if (dwOldState != BUTTON_STATUS(pCtrl)) {
                InvalidateRect (hWnd, NULL, TRUE);
            }
            return dwOldState;
        }
        
        case BM_SETSTATE:
        {
            DWORD dwOldState = BUTTON_STATUS(pCtrl) & BST_PUSHED;
            
            if (wParam)
                BUTTON_STATUS(pCtrl) |= BST_PUSHED;
            else
                BUTTON_STATUS(pCtrl) &= ~BST_PUSHED;
            
            if (dwOldState != (BUTTON_STATUS(pCtrl) & BST_PUSHED))
                InvalidateRect (hWnd, NULL, TRUE);

            return dwOldState;
        }
                
        case BM_SETSTYLE:
            pCtrl->dwStyle &= 0xFFFF0000;
            pCtrl->dwStyle |= (DWORD)(wParam & 0x0000FFFF);
            if (LOWORD (lParam))
                InvalidateRect (hWnd, NULL, TRUE);
        break;
        
        case MSG_CHAR:
            if (HIBYTE (wParam) == 0 
                    && ((dwStyle & BS_TYPEMASK) == BS_CHECKBOX
                     || (dwStyle & BS_TYPEMASK) == BS_AUTOCHECKBOX)) {
                DWORD dwOldState = BUTTON_STATUS(pCtrl);
                
                if (LOBYTE(wParam) == '+' || LOBYTE(wParam) == '=')
                    BUTTON_STATUS(pCtrl) |= BST_CHECKED;
                else if (LOBYTE(wParam) == '-')
                    BUTTON_STATUS(pCtrl) &= ~BST_CHECKED;
                    
                if (dwOldState != BUTTON_STATUS(pCtrl))
                    InvalidateRect (hWnd, NULL, FALSE);
            }
        break;
        
        case MSG_ENABLE:
            if (wParam && (dwStyle & WS_DISABLED))
                pCtrl->dwStyle &= ~WS_DISABLED;
            else if (!wParam && !(dwStyle & WS_DISABLED))
                pCtrl->dwStyle |= WS_DISABLED;
            else
                return 0;
            InvalidateRect (hWnd, NULL, FALSE);
        return 0;

        case MSG_GETDLGCODE:
            switch (dwStyle & BS_TYPEMASK) {
                case BS_CHECKBOX:
                case BS_AUTOCHECKBOX:
                return DLGC_WANTCHARS | DLGC_BUTTON;
                
                case BS_RADIOBUTTON:
                case BS_AUTORADIOBUTTON:
                return DLGC_RADIOBUTTON | DLGC_BUTTON;
                
                case BS_DEFPUSHBUTTON:
                return DLGC_DEFPUSHBUTTON;
                
                case BS_PUSHBUTTON:
                return DLGC_PUSHBUTTON;
                
                case BS_3STATE:
                case BS_AUTO3STATE:
                return DLGC_3STATE;

                default:
                return 0;
            }
        break;

        case MSG_NCHITTEST:
        {
            int x, y;
            
            x = (int)wParam;
            y = (int)lParam;
        
            if (PtInRect ((PRECT) &(pCtrl->cl), x, y))
                return HT_CLIENT;
            else  
                return HT_OUT;
        }
        break;
    
        case MSG_KILLFOCUS:
            BUTTON_STATUS(pCtrl) &= (~BST_FOCUS);
            if (GetCapture() == hWnd) {
                ReleaseCapture ();
                BUTTON_STATUS(pCtrl) &= (~BST_PUSHED);
            }

            if (dwStyle & BS_NOTIFY)
                NotifyParent (hWnd, pCtrl->id, BN_KILLFOCUS);

            InvalidateRect (hWnd, NULL, FALSE);
        break;

        case MSG_SETFOCUS:
            BUTTON_STATUS(pCtrl) |= BST_FOCUS;

            if (dwStyle & BS_NOTIFY)
                NotifyParent (hWnd, pCtrl->id, BN_SETFOCUS);

            InvalidateRect (hWnd, NULL, FALSE);
        break;
        
        case MSG_KEYDOWN:
            if (wParam != SCANCODE_SPACE 
                    && wParam != SCANCODE_ENTER
                    && wParam != SCANCODE_KEYPADENTER)
                break;

            if (GetCapture () == hWnd)
                break;
            
            SetCapture (hWnd);

            BUTTON_STATUS(pCtrl) |= BST_PUSHED;
            BUTTON_STATUS(pCtrl) |= BST_FOCUS;

            if (dwStyle & BS_NOTIFY)
                NotifyParent (hWnd, pCtrl->id, BN_PUSHED);

            InvalidateRect (hWnd, NULL, FALSE);
        break;
        
        case MSG_KEYUP:
        {
            DWORD dwState;
            
            if (wParam != SCANCODE_SPACE 
                    && wParam != SCANCODE_ENTER
                    && wParam != SCANCODE_KEYPADENTER)
                break;
                
            if (GetCapture () != hWnd)
                break;

            BUTTON_STATUS(pCtrl) &= ~BST_PUSHED;
            ReleaseCapture ();
            
            InvalidateRect (hWnd, NULL, FALSE);

            switch (pCtrl->dwStyle & BS_TYPEMASK)
            {
                case BS_AUTORADIOBUTTON:
                    if (!(BUTTON_STATUS(pCtrl) & BST_CHECKED))
                        SendMessage (hWnd, BM_SETCHECK, BST_CHECKED, 0);
                break;
                    
                case BS_AUTOCHECKBOX:
                    if (BUTTON_STATUS(pCtrl) & BST_CHECKED)
                        dwState = BST_UNCHECKED;
                    else
                        dwState = BST_CHECKED;
                                
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)dwState, 0);
                break;
                    
                case BS_AUTO3STATE:
                    dwState = (BUTTON_STATUS(pCtrl) & 0x00000003L);
                    dwState = (dwState + 1) % 3;
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)dwState, 0);
                break;
    
                case BS_PUSHBUTTON:
                case BS_DEFPUSHBUTTON:
                break;
            }

            NotifyParent (hWnd, pCtrl->id, BN_CLICKED);
            if (dwStyle & BS_NOTIFY)
                NotifyParent (hWnd, pCtrl->id, BN_UNPUSHED);
        }
        break;
      
        case MSG_LBUTTONDBLCLK:
            if (dwStyle & BS_NOTIFY)
                NotifyParent (hWnd, pCtrl->id, BN_DBLCLK);
        break;
        
        case MSG_LBUTTONDOWN:
            if (GetCapture () == hWnd)
                break;
            SetCapture (hWnd);
                
            BUTTON_STATUS(pCtrl) |= BST_PUSHED;
            BUTTON_STATUS(pCtrl) |= BST_FOCUS;

            if (dwStyle & BS_NOTIFY)
                NotifyParent (hWnd, pCtrl->id, BN_PUSHED);

            InvalidateRect (hWnd, NULL, FALSE);
        break;
    
        case MSG_LBUTTONUP:
        {
            int x, y;
            DWORD dwState;

            if (GetCapture() != hWnd)
                break;

            ReleaseCapture ();

            x = LOSWORD(lParam);
            y = HISWORD(lParam);
            ScreenToClient (GetParent (hWnd), &x, &y);
            
            if (PtInRect ((PRECT) &(pCtrl->cl), x, y))
            {
                switch (pCtrl->dwStyle & BS_TYPEMASK)
                {
                    case BS_AUTORADIOBUTTON:
                        if (!(BUTTON_STATUS(pCtrl) & BST_CHECKED))
                            SendMessage (hWnd, BM_SETCHECK, BST_CHECKED, 0);
                    break;
                    
                    case BS_AUTOCHECKBOX:
                        if (BUTTON_STATUS(pCtrl) & BST_CHECKED)
                            dwState = BST_UNCHECKED;
                        else
                            dwState = BST_CHECKED;
                                
                        SendMessage (hWnd, BM_SETCHECK, (WPARAM)dwState, 0);
                    break;
                    
                    case BS_AUTO3STATE:
                        dwState = (BUTTON_STATUS(pCtrl) & 0x00000003L);
                        dwState = (dwState + 1) % 3;
                        SendMessage (hWnd, BM_SETCHECK, (WPARAM)dwState, 0);
                    break;
    
                    case BS_PUSHBUTTON:
                    case BS_DEFPUSHBUTTON:
                    break;
                }
                
                BUTTON_STATUS(pCtrl) &= ~BST_PUSHED;

                if (dwStyle & BS_NOTIFY)
                    NotifyParent (hWnd, pCtrl->id, BN_UNPUSHED);
                NotifyParent (hWnd, pCtrl->id, BN_CLICKED);

                InvalidateRect (hWnd, NULL, FALSE);
            }
            else if (BUTTON_STATUS(pCtrl) & BST_PUSHED) {
                BUTTON_STATUS(pCtrl) &= ~BST_PUSHED;

                if (dwStyle & BS_NOTIFY)
                    NotifyParent (hWnd, pCtrl->id, BN_UNPUSHED);

                InvalidateRect (hWnd, NULL, FALSE);
            }
        }
        /*return 0;*/
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
	            SetNullFocus (GetParent(hWnd));
	        }

	        break;
	    }
                
        case MSG_MOUSEMOVE:
        {
            int x, y;

            if (GetCapture() != hWnd)
                break;

            x = LOSWORD(lParam);
            y = HISWORD(lParam);
            ScreenToClient (GetParent (hWnd), &x, &y);
            
            if (PtInRect ((PRECT) &(pCtrl->cl), x, y))
            {
                if ( !(BUTTON_STATUS(pCtrl) & BST_PUSHED) ) {
                    BUTTON_STATUS(pCtrl) |= BST_PUSHED;
                    
                    if (dwStyle & BS_NOTIFY)
                        NotifyParent (hWnd, pCtrl->id, BN_PUSHED);
                    InvalidateRect (hWnd, NULL, FALSE);
                }
            }
            else if (BUTTON_STATUS(pCtrl) & BST_PUSHED) {
                BUTTON_STATUS(pCtrl) &= ~BST_PUSHED;

                if (dwStyle & BS_NOTIFY)
                    NotifyParent (hWnd, pCtrl->id, BN_UNPUSHED);
                InvalidateRect (hWnd, NULL, FALSE);
            }
        }
        break;
    
        case MSG_PAINT:
        {
            hdc = BeginPaint (hWnd);
            
            btnGetRects (hWnd, dwStyle, &rcClient, &rcText, &rcBitmap);
			btnPaintbutton(pCtrl, hdc, dwStyle,
                    &rcClient, &rcText, &rcBitmap);
            btnPaintContent (pCtrl, hdc, dwStyle, &rcText);

            EndPaint (hWnd, hdc);
        }
        return 0;

        default:
        break;
    }
    
    return DefaultControlProc (hWnd, uMsg, wParam, lParam);
}


