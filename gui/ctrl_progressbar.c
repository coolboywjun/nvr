/*
** $Id: progressbar.c 7329 2007-08-16 02:59:29Z xgwang $
**
** progressbar.c: the Progress Bar Control module.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
**
** Current maintainer: Cui Wei
**
** Create date: 1999/8/29
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/ctrl/ctrlhelper.h>
#include <minigui/cliprect.h>
#include <minigui/internals.h>
#include <minigui/ctrlclass.h>
#include "ctrl_progressbar.h"

//#include "ctrlmisc.h"
//#include "progressbar_impl.h"

#ifdef _FLAT_WINDOW_STYLE
#define  WIDTH_PBAR_BORDER  1
#else
#define  WIDTH_PBAR_BORDER  0
#endif

static BITMAP progressColor;

static int ProgressBarCtrlExProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam);

BOOL RegisterProgressBarControlEx (void)
{
    WNDCLASS WndClass;

	LoadBitmapFromFile(HDC_SCREEN, &progressColor, "nvr_res/progress_color.bmp");
    WndClass.spClassName = CTRL_PROGRESSBAR_EX;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (0);
    WndClass.iBkColor    = 0xff202122;//PIXEL_lightwhite;
    WndClass.WinProc     = ProgressBarCtrlExProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

void ProgressBarControlExCleanup (void)
{
    // do nothing
    UnloadBitmap(&progressColor);
    return;
}

static void pbarOnDraw (HWND hwnd, HDC hdc, PROGRESSDATA_EX* pData, BOOL fVertical)
{
    RECT    rcClient;
    int     x, y, w, h;
    ldiv_t   ndiv_progress;
    unsigned int     nAllPart;
    unsigned int     nNowPart;
    int     whOne, nRem;
    int     ix, iy;
    int     i;
    int     step;
    
    if (pData->nMax == pData->nMin)
        return;
#if 0 
    if ((pData->nMax - pData->nMin) > 5)
        step = 5;
    else
        step = 1;
#endif
	step = pData->nStepInc;

    GetClientRect (hwnd, &rcClient);

    x = rcClient.left + WIDTH_PBAR_BORDER;
    y = rcClient.top + WIDTH_PBAR_BORDER;
    w = RECTW (rcClient) - (WIDTH_PBAR_BORDER << 1);
    h = RECTH (rcClient) - (WIDTH_PBAR_BORDER << 1);

    ndiv_progress = ldiv (pData->nMax - pData->nMin, step);
    nAllPart = ndiv_progress.quot;
    
    ndiv_progress = ldiv (pData->nPos - pData->nMin, step);
    nNowPart = ndiv_progress.quot;
    if (fVertical)
        ndiv_progress = ldiv (h, nAllPart);
    else
        ndiv_progress = ldiv (w, nAllPart);
        
    whOne = ndiv_progress.quot;
    nRem = ndiv_progress.rem;

    SetBrushColor (hdc, 0xFF0773BB/*GetWindowElementColorEx (hwnd, BKC_HILIGHT_LOSTFOCUS)*/);
    if (0) 
	{
        if (fVertical) {
            for (i = 0, iy = y + h - 1; i < nNowPart; ++i) {
                if ((iy - whOne) < y) 
                    whOne = iy - y;

                FillBox (hdc, x + 1, iy - whOne, w - 2, whOne - 1);
                iy -= whOne;
                if(nRem > 0) {
                    iy --;
                    nRem --;
                }
            }
        }
        else {
            for (i = 0, ix = x + 1; i < nNowPart; ++i) {
                if ((ix + whOne) > (x + w)) 
                    whOne = x + w - ix;

                FillBox (hdc, ix, y + 1, whOne - 1, h - 2);
                ix += whOne;
                if(nRem > 0) {
                    ix ++;
                    nRem --;
                }
            }
        }
    }
    else
	{
        // no vertical support
        double d = (nNowPart*1.0)/nAllPart;

        if (fVertical) {
            int prog = (int)(h*d);

            FillBox (hdc, x + 1, rcClient.bottom - WIDTH_PBAR_BORDER - prog, 
                    w - 2, prog);
        }
        else {
            char szText[8];
            SIZE text_ext;
            RECT rc_clip = rcClient;
            int prog = (int)(w*d);

            //FillBox (hdc, x, y, (int)(w*d), h);
            if (prog != 0)
            	FillBoxWithBitmap(hdc, x, y, (int)(w*d), h, &progressColor);

            SetBkMode (hdc, BM_TRANSPARENT);
            sprintf (szText, "%.0f%%", (d*100));
            GetTextExtent (hdc, szText, -1, &text_ext);
            x += ((w - text_ext.cx) >> 1);
            y += ((h - text_ext.cy) >> 1);

            rc_clip.right = prog;
            SelectClipRect (hdc, &rc_clip);
            SetTextColor (hdc, /*GetWindowElementColorEx (hwnd, FGC_HILIGHT_NORMAL)*/PIXEL_lightwhite);
            SetBkColor (hdc, GetWindowElementColorEx (hwnd, BKC_HILIGHT_NORMAL));
            TextOut (hdc, x, y, szText);

            rc_clip.right = rcClient.right;
            rc_clip.left = prog;
            SelectClipRect (hdc, &rc_clip);
            SetTextColor (hdc, PIXEL_lightwhite);//GetWindowElementColorEx (hwnd, FGC_CONTROL_NORMAL));
            SetBkColor (hdc, GetWindowBkColor (hwnd));
            TextOut (hdc, x, y, szText);
        }
    }
}

static void pbarNormalizeParams (const CONTROL* pCtrl, 
                PROGRESSDATA_EX* pData, BOOL fNotify)
{
    if (pData->nPos >= pData->nMax) {
        if (fNotify)
            NotifyParent ((HWND)pCtrl, pCtrl->id, PBN_REACHMAX);
        pData->nPos = pData->nMax;
    }

    if (pData->nPos <= pData->nMin) {
        if (fNotify)
            NotifyParent ((HWND)pCtrl, pCtrl->id, PBN_REACHMIN);
        pData->nPos = pData->nMin;
    }
}

static void pbarOnNewPos (const CONTROL* pCtrl, PROGRESSDATA_EX* pData, unsigned int new_pos)
{
    double d;
    unsigned int old_pos;
    int range = pData->nMax - pData->nMin;
    int old_prog, new_prog, width;
    RECT rc_client;

    if (range == 0 || new_pos == pData->nPos)
        return;

    old_pos = pData->nPos;
    pData->nPos = new_pos;
    pbarNormalizeParams (pCtrl, pData, pCtrl->dwStyle & PBS_NOTIFY);
    if (old_pos == pData->nPos)
        return;

    GetClientRect ((HWND)pCtrl, &rc_client);

    if (pCtrl->dwStyle & PBS_VERTICAL)
        width = RECTH (rc_client) - (WIDTH_PBAR_BORDER << 1);
    else
        width = RECTW (rc_client) - (WIDTH_PBAR_BORDER << 1);

    old_prog = old_pos - pData->nMin;
    d = (old_prog*1.0)/range;
    old_prog = (int) (d * width);

    new_prog = pData->nPos - pData->nMin;
    d = (new_prog*1.0)/range;
    new_prog = (int) (d * width);

    if (pCtrl->dwStyle & PBS_VERTICAL) {
#if 0
        old_prog = rc_client.bottom - WIDTH_PBAR_BORDER - old_prog;
        new_prog = rc_client.bottom - WIDTH_PBAR_BORDER - new_prog;
        rc_client.top = MIN (old_prog, new_prog) - 4;
        rc_client.bottom = MAX (old_prog, new_prog) + 4;
#endif
    }
    else {
        HDC hdc;
        char text [10];
        RECT rc_text;
        SIZE text_ext;

        sprintf (text, "%.0f%%", (d*100));

        hdc = GetClientDC ((HWND)pCtrl);
        GetTextExtent (hdc, text, -1, &text_ext);
        ReleaseDC (hdc);

        rc_text.left = rc_client.left + ((RECTW(rc_client) - text_ext.cx) >> 1);
        rc_text.top = rc_client.top + ((RECTH(rc_client) - text_ext.cy) >> 1);
        rc_text.right = rc_text.left + text_ext.cx;
        rc_text.bottom = rc_text.top + text_ext.cy;
        InvalidateRect ((HWND)pCtrl, &rc_client, TRUE);

        rc_client.left = MIN (old_prog, new_prog);
        rc_client.right = MAX (old_prog, new_prog);
    }

    InvalidateRect ((HWND)pCtrl, &rc_client, TRUE);

}

static int ProgressBarCtrlExProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC           hdc;
    PCONTROL      pCtrl;
    PROGRESSDATA_EX* pData;
    
    pCtrl = Control (hwnd); 
    
    switch (message)
    {
        case MSG_CREATE:
            if (!(pData = malloc (sizeof (PROGRESSDATA_EX)))) {
                fprintf(stderr, "Create progress bar control failure!\n");
                return -1;
            }
            
            pData->nMax     = 100;
            pData->nMin     = 0;
            pData->nPos     = 0;
            pData->nStepInc = 10;
            
            pCtrl->dwAddData2 = (DWORD)pData;
        break;
    
        case MSG_DESTROY:
            free ((void *)(pCtrl->dwAddData2));
        break;

        case MSG_NCPAINT:
            return 0;
        
        case MSG_GETDLGCODE:
            return DLGC_STATIC;

        case MSG_GETTEXTLENGTH:
        case MSG_GETTEXT:
        case MSG_SETTEXT:
            return -1;

        case MSG_PAINT:
        {
            RECT rcClient;
            
            GetClientRect (hwnd, &rcClient);

            hdc = BeginPaint (hwnd);
/*
#ifdef _FLAT_WINDOW_STYLE
            SetPenColor (hdc, GetWindowElementColorEx (hwnd, BKC_CONTROL_DEF));
            Rectangle (hdc, rcClient.left, rcClient.top, 
                             rcClient.right - 1, rcClient.bottom - 1);

            SetPenColor (hdc, GetWindowElementColorEx (hwnd, FGC_CONTROL_NORMAL));
            Rectangle (hdc, rcClient.left + 1, rcClient.top + 1, 
                             rcClient.right - 2, rcClient.bottom - 2);
#else
            Draw3DThickFrameEx (hdc, hwnd, rcClient.left, rcClient.top, 
                             rcClient.right - 1, rcClient.bottom - 1, 
                             DF_3DBOX_PRESSED | DF_3DBOX_NOTFILL, 0);
#endif
*/
            pbarOnDraw (hwnd, hdc, (PROGRESSDATA_EX *)pCtrl->dwAddData2, 
                            pCtrl->dwStyle & PBS_VERTICAL);

            EndPaint (hwnd, hdc);
            return 0;
        }

        case PBM_SETRANGE:
            pData = (PROGRESSDATA_EX *)pCtrl->dwAddData2;
            if (wParam == lParam)
                return PB_ERR;

            pData->nMin = MIN (wParam, lParam);
            pData->nMax = MAX (wParam, lParam);
            if (pData->nPos > pData->nMax)
                pData->nPos = pData->nMax;
            if (pData->nPos < pData->nMin)
                pData->nPos = pData->nMin;

            if (pData->nStepInc > (pData->nMax - pData->nMin))
                pData->nStepInc = pData->nMax - pData->nMin;

            InvalidateRect (hwnd, NULL, TRUE);
            return PB_OKAY;
        
        case PBM_SETSTEP:
            pData = (PROGRESSDATA_EX *)pCtrl->dwAddData2;
            if ((int)wParam > (int)(pData->nMax - pData->nMin))
                return PB_ERR;

            pData->nStepInc = wParam;
            return PB_OKAY;
        
        case PBM_SETPOS:
            pData = (PROGRESSDATA_EX *)pCtrl->dwAddData2;
            
            if (pData->nPos == wParam)
                return PB_OKAY;

            pbarOnNewPos (pCtrl, pData, wParam);
            return PB_OKAY;
        
        case PBM_DELTAPOS:
            pData = (PROGRESSDATA_EX *)pCtrl->dwAddData2;

            if (wParam == 0)
                return PB_OKAY;
            
            pbarOnNewPos (pCtrl, pData, pData->nPos + wParam);
            return PB_OKAY;
        
        case PBM_STEPIT:
            pData = (PROGRESSDATA_EX *)pCtrl->dwAddData2;
            
            if (pData->nStepInc == 0)
                return PB_OKAY;

            pbarOnNewPos (pCtrl, pData, pData->nPos + pData->nStepInc);
            return PB_OKAY;
            
        case MSG_FONTCHANGED:
            InvalidateRect (hwnd, NULL, TRUE);
            break;
    }
    
    return DefaultControlProc (hwnd, message, wParam, lParam);
}


