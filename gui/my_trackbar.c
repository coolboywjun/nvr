/*
** $Id: trackbar.c 7329 2007-08-16 02:59:29Z xgwang $
**
** trackbar.c: the TrackBar Control module.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
** 
** Current maintainer: Yan Xiaowei
**
** NOTE: Originally by Zheng Yiran
**
** Create date: 2000/12/02
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/cliprect.h>
#include <minigui/internals.h>
#include <minigui/ctrlclass.h>
#include "my_trackbar.h"
#ifdef _CTRL_TRACKBAR


static BITMAP bmp_hbg, bmp_hslider;
static BITMAP bmp_vbg, bmp_vslider;


#define WIDTH_HORZ_SLIDER       (bmp_hslider.bmWidth)
#define HEIGHT_VERT_SLIDER      (bmp_vslider.bmHeight)


static int MyTrackBarCtrlProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam);

BOOL RegisterMyTrackBarControl (void)
{
    WNDCLASS WndClass;
	LoadBitmapFromFile(HDC_SCREEN, &bmp_hbg, "nvr_res/trackbar-hbg.bmp");
	LoadBitmapFromFile(HDC_SCREEN, &bmp_hslider, "nvr_res/trackbar-hslider.bmp");
	LoadBitmapFromFile(HDC_SCREEN, &bmp_vbg, "nvr_res/trackbar-vbg.bmp");
	LoadBitmapFromFile(HDC_SCREEN, &bmp_vslider, "nvr_res/trackbar-vslider.bmp"); //tupian

    WndClass.spClassName = MY_TRACKBAR;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (0);
    WndClass.iBkColor    = GetWindowElementColor (BKC_CONTROL_DEF);
    WndClass.WinProc     = MyTrackBarCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

void MyTrackBarControlCleanup (void)
{
	UnloadBitmap(&bmp_hbg);
	UnloadBitmap(&bmp_hslider);
	UnloadBitmap(&bmp_vbg);
	UnloadBitmap(&bmp_vslider);
    return;
}


static void GetTrackbarRects (MYTRACKBARDATA* pData, DWORD dwStyle, const RECT* rc_client, RECT* rc_ruler, RECT* rc_slider)
{
    int     l, t, w, h;
    int     pos, min, max;
    int     sliderx, slidery, sliderw, sliderh;

    /* get data of trackbar. */
    w = RECTWP (rc_client);
    h = RECTHP (rc_client);
    pos = pData->nPos;
    max = pData->nMax;
    min = pData->nMin;

    /* get width/height of slider. */
    if (dwStyle & TBS_VERTICAL) {
        sliderw = bmp_vslider.bmWidth;
        sliderh = bmp_vslider.bmHeight;
        h  = h;//-= sliderh;
        l = 0; 
		t = 0;//sliderh>>1;
    }
    else {
        sliderw = bmp_hslider.bmWidth;
        sliderh = bmp_hslider.bmHeight;
        w = w;//-= sliderw;
        l = 0;//sliderw>>1; 
		t = 0;
    }

    /* get the rectangle of the ruler. */
    if (rc_ruler) {
		SetRect(rc_ruler, l, (rc_client->top+(RECTHP (rc_client)>>1) - 1), l + w, (rc_client->top+(RECTHP (rc_client)>>1) + 1));
        //SetRect (rc_ruler, l, t, l + w, t + h);
    }
    
    /* get the rectangle of the slider according to position. */
    if (dwStyle & TBS_VERTICAL) {

        sliderx = (w - sliderw)>>1; 
        slidery = t + (int)((max - pos) * (h - sliderh) / (float)(max - min));// - (sliderh>>1);
    }
    else {
        slidery = (h - sliderh)>>1; 
        sliderx = l + (int)((pos - min) * (w - sliderw) / (float)(max - min)); //- (sliderw>>1);
    }

    SetRect (rc_slider, sliderx, slidery, sliderx + sliderw, slidery + sliderh);
}

static void TrackBarOnDraw (HWND hwnd, HDC hdc, MYTRACKBARDATA* pData, DWORD dwStyle)
{
    RECT rc_client, rc_ruler, rc_slider;

    GetClientRect (hwnd, &rc_client);

    GetTrackbarRects (pData, dwStyle, &rc_client, &rc_ruler, &rc_slider);
#if 1
    /* draw the ruler. */
    if (dwStyle & TBS_VERTICAL)
    {
		bmp_vbg.bmType = BMP_TYPE_COLORKEY;
	  	bmp_vbg.bmColorKey = GetPixelInBitmap(&bmp_vbg, 0, 0);
		FillBoxWithBitmap(hdc,rc_ruler.left, rc_ruler.top, RECTW(rc_ruler),RECTH(rc_ruler), &bmp_vbg);
    }
    else
    {
		bmp_hbg.bmType = BMP_TYPE_COLORKEY;
	  	bmp_hbg.bmColorKey = GetPixelInBitmap(&bmp_hbg, 0, 0);
		FillBoxWithBitmap(hdc,rc_ruler.left, rc_ruler.top, RECTW(rc_ruler),RECTH(rc_ruler), &bmp_hbg);
        //DrawBoxFromBitmap (hdc, &rc_ruler, bmp_hbg, TRUE, FALSE);
    }
#endif

	bmp_vslider.bmType = BMP_TYPE_COLORKEY;
	bmp_vslider.bmColorKey = GetPixelInBitmap(&bmp_vslider, 0, 0);
	bmp_hslider.bmType = BMP_TYPE_COLORKEY;
	bmp_hslider.bmColorKey = GetPixelInBitmap(&bmp_hslider, 0, 0);

    /* draw the slider. */
    FillBoxWithBitmap (hdc, rc_slider.left, rc_slider.top, 0, 0, 
            (dwStyle & TBS_VERTICAL) ? &bmp_vslider : &bmp_hslider);
}

static void TrackBarNormalizeParams (const CONTROL* pCtrl, MYTRACKBARDATA* pData, BOOL fNotify)
{
    if (pData->nPos >= pData->nMax) {
        if (fNotify) {
            NotifyParent ((HWND)pCtrl, pCtrl->id, TBN_CHANGE);
            NotifyParent ((HWND)pCtrl, pCtrl->id, TBN_REACHMAX);
        }
        pData->nPos = pData->nMax;
    }
    else if (pData->nPos <= pData->nMin) {
        if (fNotify) {
            NotifyParent ((HWND)pCtrl, pCtrl->id, TBN_CHANGE);
            NotifyParent ((HWND)pCtrl, pCtrl->id, TBN_REACHMIN);
        }
        pData->nPos = pData->nMin;
    }
    else if (pData->nPos < pData->nMax && pData->nPos > pData->nMin) {
        if (fNotify)
            NotifyParent ((HWND)pCtrl, pCtrl->id, TBN_CHANGE);
    }
}

static void SetSliderPos (const CONTROL* pCtrl, int new_pos)
{
    MYTRACKBARDATA* pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;
    RECT rc_client, old_slider, new_slider;

    if (new_pos > pData->nMax)
        new_pos = pData->nMax; 

    if (new_pos < pData->nMin)
        new_pos = pData->nMin; 

    if (pData->nPos == new_pos) 
        return;

    GetClientRect ((HWND)pCtrl, &rc_client);

    GetTrackbarRects (pData, pCtrl->dwStyle, &rc_client, NULL, &old_slider);

    pData->nPos = new_pos;
    TrackBarNormalizeParams (pCtrl, pData, pCtrl->dwStyle & TBS_NOTIFY);

    GetTrackbarRects (pData, pCtrl->dwStyle, &rc_client, NULL, &new_slider);

    if (pCtrl->dwStyle & TBS_TIP) {
        InvalidateRect ((HWND)pCtrl, NULL, TRUE);
    }
    else if (!EqualRect (&old_slider, &new_slider)) {
        InvalidateRect ((HWND)pCtrl, &old_slider, TRUE);
        InvalidateRect ((HWND)pCtrl, &new_slider, TRUE);
    }

}

static int NormalizeMousePos (HWND hwnd, MYTRACKBARDATA* pData, int mousepos)
{
    RECT    rcClient;
    int     h;
    int     len, pos;

    GetClientRect (hwnd, &rcClient);
    //w = RECTW (rcClient);
    h = RECTH (rcClient);

    if (GetWindowStyle (hwnd) & TBS_VERTICAL) {
        int blank = HEIGHT_VERT_SLIDER>>1;

        len = RECTH (rcClient) - HEIGHT_VERT_SLIDER;
        if (mousepos > rcClient.bottom - blank)
            pos = 0;
        else if (mousepos < blank)
            pos = len;
        else
            pos = h - blank - mousepos;
    } else {
        int blank = WIDTH_HORZ_SLIDER>>1;

        len = RECTW (rcClient) - WIDTH_HORZ_SLIDER;
        if (mousepos < blank)
            pos = 0;
        else if (mousepos > rcClient.right - blank)
            pos = len ;
        else
            pos = mousepos - blank;
    }

    return (int)((pData->nMax - pData->nMin) * pos / (float)len + 0.5) + pData->nMin;
}

static int MyTrackBarCtrlProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    PCONTROL      pCtrl;
    MYTRACKBARDATA* pData;
    pCtrl = Control (hwnd);
    
    switch (message)
    {
        case MSG_CREATE:
            if (!(pData = malloc (sizeof (MYTRACKBARDATA)))) {
                fprintf(stderr, "Create trackbar control failure!\n");
                return -1;
            }
            pData->nMax = 10;
            pData->nMin = 0;
            pData->nPos = 0;
            pData->nLineSize = 1;
            pData->nPageSize = 5;
            strcpy (pData->sStartTip, "Start");
            strcpy (pData->sEndTip, "End");
            pData->nTickFreq = 1;
            pCtrl->dwAddData2 = (DWORD)pData;
        break;
    
        case MSG_DESTROY:
            free((void *)(pCtrl->dwAddData2));
        break;

        case MSG_NCPAINT:
            return 0;
       
        case MSG_GETTEXTLENGTH:
        case MSG_GETTEXT:
        case MSG_SETTEXT:
            return -1;

        case MSG_PAINT:
        {
            HDC hdc;

            hdc = BeginPaint (hwnd);
            TrackBarOnDraw (hwnd, hdc, (MYTRACKBARDATA *)pCtrl->dwAddData2, pCtrl->dwStyle);
            EndPaint (hwnd, hdc);
            return 0;
        }

        case TBM_SETRANGE:
        {
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;

            if (wParam == lParam)
                return -1;

            pData->nMin = MIN ((int)wParam, (int)lParam);
            pData->nMax = MAX ((int)wParam, (int)lParam);

            if (pData->nPos > pData->nMax)
                pData->nPos = pData->nMax;
            if (pData->nPos < pData->nMin)
                pData->nPos = pData->nMin;

            SendMessage (hwnd, TBM_SETPOS, pData->nPos, 0);
            return 0;
        }
        
        case TBM_GETMIN:
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;
            return pData->nMin;
     
        case TBM_GETMAX:    
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;
            return pData->nMax;
    
        case TBM_SETMIN:
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;

            if (wParam == pData->nMin || wParam >= pData->nMax)
                return -1;

            pData->nMin = wParam;
            if (pData->nPos < pData->nMin)
                pData->nPos = pData->nMin;
            SendMessage (hwnd, TBM_SETPOS, pData->nPos, 0);
            return 0;
    
        case TBM_SETMAX:
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;

            if ((int)wParam == pData->nMax || (int)wParam <= pData->nMin)
                return -1;

            pData->nMax = wParam;
            if (pData->nPos > pData->nMax)
                pData->nPos = pData->nMax;
            SendMessage (hwnd, TBM_SETPOS, pData->nPos, 0);
            return 0;
        
        case TBM_SETLINESIZE:
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;
            if (wParam > (pData->nMax - pData->nMin))
                return -1;
            pData->nLineSize = wParam;
            return 0;

        case TBM_GETLINESIZE:
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;
            return pData->nLineSize;
        
        case TBM_SETPAGESIZE:
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;
            if (wParam > (pData->nMax - pData->nMin))
                return -1;
            pData->nPageSize = wParam;
            return 0;
        
        case TBM_GETPAGESIZE:
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;
            return pData->nPageSize;
    
        case TBM_SETPOS:
            SetSliderPos (pCtrl, wParam);
            return 0;
        
        case TBM_GETPOS:
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;
            return pData->nPos;
        
        case TBM_SETTICKFREQ:
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;
            if (wParam > (pData->nMax - pData->nMin))
                wParam = pData->nMax - pData->nMin;
            pData->nTickFreq = wParam > 1 ? wParam : 1;
            InvalidateRect (hwnd, NULL, TRUE);
            return 0;

        case TBM_GETTICKFREQ:
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;
            return pData->nTickFreq;
    
        case TBM_SETTIP:
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;
            if (wParam) 
                strncpy(pData->sStartTip, (char *) wParam, TBLEN_TIP);
            if (lParam)
                strncpy (pData->sEndTip, (char *) lParam, TBLEN_TIP);
            InvalidateRect (hwnd, NULL, TRUE);
            return 0;

        case TBM_GETTIP:
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;
            if (wParam)
                strcpy ((char *) wParam, pData->sStartTip);
            if (lParam)
                strcpy ((char *) lParam, pData->sEndTip);
            return 0;
        
#ifdef _PC3D_WINDOW_STYLE
        case MSG_SETFOCUS:
            if (pCtrl->dwStyle & TBS_FOCUS)
                break;
            pCtrl->dwStyle |= TBS_FOCUS;
            InvalidateRect (hwnd, NULL, TRUE);
            break;
    
        case MSG_KILLFOCUS:
            pCtrl->dwStyle &= ~TBS_FOCUS;
            InvalidateRect (hwnd, NULL, TRUE);
            break;
#endif
    
        case MSG_GETDLGCODE:
            return DLGC_WANTARROWS;

        case MSG_ENABLE:
            if (wParam && (pCtrl->dwStyle & WS_DISABLED))
                pCtrl->dwStyle &= ~WS_DISABLED;
            else if (!wParam && !(pCtrl->dwStyle & WS_DISABLED))
                pCtrl->dwStyle |= WS_DISABLED;
            else
                return 0;
            InvalidateRect (hwnd, NULL, TRUE);

            return 0;

        case MSG_KEYDOWN:
            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;

            if (pCtrl->dwStyle & WS_DISABLED)
                return 0;

            switch (LOWORD (wParam)) {
                case SCANCODE_CURSORBLOCKUP:
                case SCANCODE_CURSORBLOCKRIGHT:
                    SetSliderPos (pCtrl, pData->nPos + pData->nLineSize);
                break;

                case SCANCODE_CURSORBLOCKDOWN:
                case SCANCODE_CURSORBLOCKLEFT:
                    SetSliderPos (pCtrl, pData->nPos - pData->nLineSize);
                break;
            
                case SCANCODE_PAGEDOWN:
                    SetSliderPos (pCtrl, pData->nPos - pData->nPageSize);
                break;
            
                case SCANCODE_PAGEUP:
                    SetSliderPos (pCtrl, pData->nPos + pData->nPageSize);
                break;
            
                case SCANCODE_HOME:
                    pData->nPos = pData->nMin;
                    TrackBarNormalizeParams (pCtrl, pData, pCtrl->dwStyle & TBS_NOTIFY);
                    InvalidateRect (hwnd, NULL, TRUE);
                break;
            
                case SCANCODE_END:
                    pData->nPos = pData->nMax;
                    TrackBarNormalizeParams (pCtrl, pData, pCtrl->dwStyle & TBS_NOTIFY);
                    InvalidateRect (hwnd, NULL, TRUE);
                break;
            }
        break;

        case MSG_LBUTTONDOWN:
            if (GetCapture() != hwnd) {
                int mouseX = LOSWORD(lParam);
                int mouseY = HISWORD(lParam);
        
                SetCapture (hwnd);

                NotifyParent ((HWND)pCtrl, pCtrl->id, TBN_STARTDRAG);

                pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;
                SetSliderPos (pCtrl, NormalizeMousePos (hwnd, pData,
                            (pCtrl->dwStyle & TBS_VERTICAL)?mouseY:mouseX));
            }
            break;
                
        case MSG_MOUSEMOVE:
        {
            int mouseX = LOSWORD(lParam);
            int mouseY = HISWORD(lParam);

            if (GetCapture() == hwnd)
                ScreenToClient (hwnd, &mouseX, &mouseY);
            else
                break;

            pData = (MYTRACKBARDATA *)pCtrl->dwAddData2;
            SetSliderPos (pCtrl, NormalizeMousePos (hwnd, pData,
                            (pCtrl->dwStyle & TBS_VERTICAL)?mouseY:mouseX));
        }
        break;

        case MSG_LBUTTONUP:
            if (GetCapture() == hwnd) {
                ReleaseCapture ();
                NotifyParent ((HWND)pCtrl, pCtrl->id, TBN_STOPDRAG);
            }
            break;
    
        case MSG_FONTCHANGED:
            InvalidateRect (hwnd, NULL, TRUE);
            return 0;

        default:
            break;    
    }
    
    return DefaultControlProc (hwnd, message, wParam, lParam);
}

#endif /* _CTRL_TRACKBAR */

