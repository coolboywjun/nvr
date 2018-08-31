/*
** $Id: toolbar.c 7329 2007-08-16 02:59:29Z xgwang $
**
** toolbar.c: the toolbar control module.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
** 
** Current maintainer: Yan Xiaowei
**
** Create date: 2000/9/20
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
#include "ctrl_toolbar.h"

static int ToolbarCtrlExProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

BOOL RegisterToolbarControlEx (void)
{
    WNDCLASS WndClass;
    
    WndClass.spClassName = CTRL_TOOLBAR_EX;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = GetWindowElementColor (BKC_CONTROL_DEF);
    WndClass.WinProc     = ToolbarCtrlExProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

static void DrawToolBox (HDC hdc, PTOOLBARCTRL_EX pdata)
{
    TOOLBARITEMDATA_EX*  tmpdata;

    tmpdata = pdata->head;
	
    while (tmpdata != NULL) {		
		int w, h;
		w = (tmpdata->itemBmp->bmWidth>>2);
		h = tmpdata->itemBmp->bmHeight;
		if (tmpdata->enable == FALSE)
		{
			FillBoxWithBitmapPart(hdc, tmpdata->RcTitle.left, tmpdata->RcTitle.top , w, h, 
				0, 0, tmpdata->itemBmp, 3*w, 0);
		}
		else
		{
			FillBoxWithBitmapPart(hdc, tmpdata->RcTitle.left, tmpdata->RcTitle.top , w, h, 
				0, 0, tmpdata->itemBmp, 0, 0);
		}
       tmpdata = tmpdata->next;
    }
}

static TOOLBARITEMDATA_EX* GetCurTag(int posx,int posy, PTOOLBARCTRL_EX pdata)
{
    TOOLBARITEMDATA_EX*  tmpdata;
    tmpdata = pdata->head;
    while (tmpdata != NULL) { 
        if (PtInRect (&tmpdata->RcTitle, posx, posy)) {
            return tmpdata;    
        }
        tmpdata = tmpdata->next;
    }

    return NULL;         
}

static TOOLBARITEMDATA_EX *ToolbarGetCurItem(int id, PTOOLBARCTRL_EX pdata)
{
	TOOLBARITEMDATA_EX*  tmpdata;
    tmpdata = pdata->head;
    while (tmpdata != NULL) { 
        if (tmpdata->id == id) {
            return tmpdata;    
        }
        tmpdata = tmpdata->next;
    }

    return NULL;         
}

static void HilightToolBox (HWND hWnd, TOOLBARITEMDATA_EX* pItemdata)
{
    HDC hdc;

    hdc = GetClientDC (hWnd);
	int w, h;
	w = (pItemdata->itemBmp->bmWidth>>2);
	h = pItemdata->itemBmp->bmHeight;
	
	FillBoxWithBitmapPart(hdc, pItemdata->RcTitle.left, pItemdata->RcTitle.top , w, h, 
				0, 0, pItemdata->itemBmp, w, 0);

    ReleaseDC (hdc);
}

static void UnhilightToolBox (HWND hWnd, TOOLBARITEMDATA_EX* pItemdata)
{
    HDC hdc;

    hdc = GetClientDC (hWnd);

	int w, h;
	w = (pItemdata->itemBmp->bmWidth>>2);
	h = pItemdata->itemBmp->bmHeight;
	
	FillBoxWithBitmapPart(hdc, pItemdata->RcTitle.left, pItemdata->RcTitle.top , w, h, 
				0, 0, pItemdata->itemBmp, w*2, 0);

    ReleaseDC (hdc);
}

static void DisableToolBox(HWND hWnd, TOOLBARITEMDATA_EX* pItemdata)
{
	HDC hdc;

    hdc = GetClientDC (hWnd);
	int w, h;
	w = (pItemdata->itemBmp->bmWidth>>2);
	h = pItemdata->itemBmp->bmHeight;
	
	FillBoxWithBitmapPart(hdc, pItemdata->RcTitle.left, pItemdata->RcTitle.top , w, h, 
				0, 0, pItemdata->itemBmp, 3*w, 0);

    ReleaseDC (hdc);
}


static int ToolbarCtrlExProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC         hdc;
    PCONTROL    pCtrl;
//    DWORD       dwStyle;
    PTOOLBARCTRL_EX TbarData;
    PTOOLBARITEMDATA_EX pTbid;
    
    pCtrl = Control (hWnd);
//    dwStyle = pCtrl->dwStyle;

    switch (message) {   
        case MSG_CREATE:
        {
            DWORD data; 
            data = GetWindowAdditionalData (hWnd);

            TbarData = (TOOLBARCTRL_EX*) calloc (1, sizeof (TOOLBARCTRL_EX));
            TbarData->nCount = 0;
            TbarData->head = TbarData->tail = NULL;
            TbarData->iLBDn = 0;
            TbarData->ItemWidth = HIWORD(data);
            TbarData->ItemHeight = LOWORD(data);
            SetWindowAdditionalData2 (hWnd,(DWORD)TbarData);
        }
        break;

        case MSG_DESTROY:
        { 
            TOOLBARITEMDATA_EX* unloaddata, *tmp;
            TbarData=(PTOOLBARCTRL_EX) GetWindowAdditionalData2(hWnd);
            unloaddata = TbarData->head;

            while (unloaddata != NULL) {
                UnloadBitmap ((PBITMAP)(unloaddata->itemBmp));
                free ((PBITMAP)(unloaddata->itemBmp));

                tmp = unloaddata->next;
                free (unloaddata);
                unloaddata = tmp;
            }

            free (TbarData);
            break;
        }

        case MSG_PAINT:
        {
            TbarData = (PTOOLBARCTRL_EX)GetWindowAdditionalData2(hWnd);
            hdc = BeginPaint (hWnd);
            DrawToolBox (hdc, TbarData);
            EndPaint (hWnd, hdc);
            return 0;
        }

        case TBM_ADDITEM:
        {  
            TOOLBARITEMINFO_EX* TbarInfo = NULL;
            TOOLBARITEMDATA_EX* ptemp;
            RECT rc;
            TbarData=(PTOOLBARCTRL_EX) GetWindowAdditionalData2(hWnd);
            TbarInfo = (TOOLBARITEMINFO_EX*) lParam;
                
            GetClientRect (hWnd, &rc);
                
            ptemp = (TOOLBARITEMDATA_EX*)malloc (sizeof (TOOLBARITEMDATA_EX));
            ptemp->id = TbarInfo->id;

            if (TbarData->tail == NULL) 
                ptemp->RcTitle.left =  0;
            else
                ptemp->RcTitle.left = TbarData->tail->RcTitle.right;

            ptemp->RcTitle.right = ptemp->RcTitle.left + TbarData->ItemWidth;
            ptemp->RcTitle.top = 0;
            ptemp->RcTitle.bottom = ptemp->RcTitle.top + TbarData->ItemHeight;

            ptemp->itemBmp= (BITMAP*)malloc (sizeof (BITMAP));
            LoadBitmap (HDC_SCREEN, ptemp->itemBmp, TbarInfo->itembmpPath);

            ptemp->next = NULL;

            if (TbarData->nCount == 0)
                TbarData->head = TbarData->tail = ptemp;
            else if (TbarData->nCount > 0) { 
                TbarData->tail->next = ptemp;
                TbarData->tail = ptemp;
            }
            ptemp->insPos = TbarData->nCount;
            TbarData->nCount++;
			ptemp->enable = TRUE;

            InvalidateRect (hWnd, NULL, FALSE);
            break;
        }
		
		case TBM_SETIMAGE:
		{
			PTOOLBARITEMDATA_EX pToolbarItem;
			int oldImage = 0;			
			int id = wParam;
			
            TbarData = (PTOOLBARCTRL_EX)GetWindowAdditionalData2(hWnd);
			if (lParam == (LPARAM)NULL)	return oldImage;
			
			if ((pToolbarItem = ToolbarGetCurItem (id ,TbarData)) == NULL)
				return 0;

			oldImage = (int)pToolbarItem->itemBmp;

			if (lParam){
				pToolbarItem->itemBmp = (PBITMAP)lParam;				
				InvalidateRect (hWnd, NULL, TRUE);
			}

			return oldImage;
		}

		case TBM_ITEMENABLE:
		{
			PTOOLBARITEMDATA_EX pToolbarItem;
			int id = wParam;
			
            TbarData = (PTOOLBARCTRL_EX)GetWindowAdditionalData2(hWnd);
			if ((pToolbarItem = ToolbarGetCurItem (id ,TbarData)) == NULL)
				return 0;

			if (lParam)
				pToolbarItem->enable = TRUE;
			else
				pToolbarItem->enable = FALSE;
			InvalidateRect (hWnd, NULL, TRUE);

		}
		break;
		
        case MSG_MOUSEMOVEIN:
        {
            TbarData = (PTOOLBARCTRL_EX) GetWindowAdditionalData2(hWnd);
            if (!wParam) {
                hdc = GetClientDC (hWnd);
                DrawToolBox (hdc, TbarData);
                ReleaseDC (hdc);
            }

            break;
        }

        case MSG_LBUTTONDOWN:
        {
            int posx, posy;
            TbarData=(PTOOLBARCTRL_EX) GetWindowAdditionalData2(hWnd);

            posx = LOSWORD (lParam);
            posy = HISWORD (lParam);

            if (GetCapture () == hWnd)
                break;
            SetCapture (hWnd);
                
            if ((pTbid = GetCurTag (posx,posy,TbarData)) == NULL)
                break; 

			if (pTbid->enable == FALSE)	break;

            TbarData->iLBDn = 1;
            TbarData->iSel = pTbid->insPos;
            UnhilightToolBox (hWnd, GetCurTag (posx, posy, TbarData));
            break;
        }

        case MSG_LBUTTONUP:
        {
            int x, y;
            TbarData=(PTOOLBARCTRL_EX) GetWindowAdditionalData2(hWnd);
            x = LOSWORD(lParam);
            y = HISWORD(lParam);
            
            TbarData->iLBDn = 0;
            
            if (GetCapture() != hWnd)
                break;
            ReleaseCapture ();

            ScreenToClient (hWnd, &x, &y);
            if ((pTbid = GetCurTag(x, y, TbarData)) == NULL) {
                hdc = GetClientDC (hWnd);
                DrawToolBox (hdc, TbarData);
                ReleaseDC (hdc);
                break;
            }
            //else 
            
			if (pTbid->enable == FALSE)	break;
            HilightToolBox (hWnd, GetCurTag (x, y, TbarData));
            
            InvalidateRect (hWnd, NULL, FALSE);
            if (TbarData->iSel == pTbid->insPos)
                NotifyParent (hWnd, pCtrl->id, pTbid->id);

            break;
        }

        case MSG_MOUSEMOVE:
        {
            int x, y;
            TbarData = (PTOOLBARCTRL_EX) GetWindowAdditionalData2(hWnd);
            x = LOSWORD(lParam);
            y = HISWORD(lParam);

            if (TbarData->iLBDn == 1)
                ScreenToClient (hWnd, &x, &y);

            if (( pTbid = GetCurTag (x, y, TbarData)) == NULL) {
                hdc = GetClientDC (hWnd);
                DrawToolBox (hdc, TbarData);
                ReleaseDC (hdc);
                break;
            }
            
            if (TbarData->iMvOver != pTbid->insPos) {
                TbarData->iMvOver = pTbid->insPos;
                    
                hdc = GetClientDC (hWnd);
                DrawToolBox (hdc, TbarData);
                ReleaseDC (hdc);
            }

			PTOOLBARITEMDATA_EX ptmpItem;
			ptmpItem = GetCurTag (x, y, TbarData);
			if (ptmpItem->enable == FALSE)	break;
				
            if (TbarData->iSel == pTbid->insPos && TbarData->iLBDn == 1)
                UnhilightToolBox (hWnd, GetCurTag (x, y, TbarData));
            else if ( TbarData->iLBDn == 0 ) 
                HilightToolBox (hWnd, GetCurTag(x, y, TbarData));
            break;
        }
    }

    return DefaultControlProc (hWnd, message, wParam, lParam);
}


