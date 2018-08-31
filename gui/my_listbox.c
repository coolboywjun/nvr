/*
** $Id: listbox.c 8093 2007-11-16 07:37:30Z weiym $
**
** listbox.c: the List Box Control module.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
**
** Current maintainer: Jiao Libo
**
** Create date: 1999/8/31
*/

/*
** TODO:
**  * Multiple columns support.
*/
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
#include "my_listbox.h"

int wndScrollBarSliderStartPos (PMAINWIN pWin, int iSBar);

#define _USE_FIXSTR      1

#define ITEM_BOTTOM(x)  (x->itemTop + x->itemVisibles - 1)

#define LST_WIDTH_CHECKMARK     11
#define LST_HEIGHT_CHECKMARK    11
#define LST_INTER_BMPTEXT       5
#define LST_TEXT_START			6

//static const BITMAP*    bmp_checkmark;
static BITMAP bmp_checkmark;
#define CHECKMARK_BMP   (&bmp_checkmark)

static int MyListboxCtrlProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam);

BOOL RegisterMyListboxControl (void)
{
    WNDCLASS WndClass;

	LoadBitmapFromFile(HDC_SCREEN, &bmp_checkmark, "nvr_res/checkmark.bmp");
    WndClass.spClassName = MY_LISTBOX;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (0);
    WndClass.iBkColor    = PIXEL_black;
    WndClass.WinProc     = MyListboxCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

#if 1
void MyListboxControlCleanup (void)
{
	UnloadBitmap(&bmp_checkmark);
    return;
}
#endif

static BOOL lstInitListBoxData (HWND hwnd, PCONTROL pCtrl, MYLISTBOXDATA* pData, int len)
{
    int i;
    PMYLISTBOXITEM plbi;
    
    pData->itemHeight = pCtrl->pLogFont->size+4;
    pData->itemHilighted = -1;
    pData->selItem = pData->itemHilighted;
    pData->itemCount = 0;

    pData->str_cmp = strncmp;

    /* init item buffer. */
    if (!(pData->buffStart = malloc (len * sizeof (MYLISTBOXITEM))))
        return FALSE;

    pData->buffLen = len;
    pData->buffEnd = pData->buffStart + len;
    pData->freeList = pData->buffStart;

    plbi = pData->freeList;
    for (i = 0; i < len - 1; i++) {
        plbi->next = plbi + 1;
        plbi ++;
    }
    plbi->next = NULL;

    if (GetWindowStyle (hwnd) & LBS_SBALWAYS)
        pData->sbPolicy = SB_POLICY_ALWAYS;
    else
        pData->sbPolicy = SB_POLICY_AUTOMATIC;

    return TRUE;
}

static void lstListBoxCleanUp (MYLISTBOXDATA* pData)
{
    PMYLISTBOXITEM plbi;
    PMYLISTBOXITEM next;

    plbi = pData->head;
    while (plbi) {
#if _USE_FIXSTR
        FreeFixStr (plbi->key);
#else
        free (plbi->key);
#endif
        next = plbi->next;
        if (plbi < pData->buffStart || plbi > pData->buffEnd)
            free (plbi);

        plbi = next;
    }
    
    free (pData->buffStart);
}

static void lstResetListBoxContent (PMYLISTBOXDATA pData)
{
    int i;
    PMYLISTBOXITEM plbi, next;
    
    pData->itemLeft = 0;
    pData->itemWidth = 0;    
    pData->itemMaxWidth = 0;
    
    pData->itemCount = 0;
    pData->itemTop = 0;
    pData->itemHilighted = -1;
    pData->selItem = pData->itemHilighted;

    plbi = pData->head;
    while (plbi) {
#if _USE_FIXSTR
        FreeFixStr (plbi->key);
#else
        free (plbi->key);
#endif
        next = plbi->next;
        if (plbi < pData->buffStart || plbi > pData->buffEnd)
            free (plbi);

        plbi = next;
    }

    pData->head = NULL;
    pData->freeList = pData->buffStart;

    plbi = pData->freeList;
    for (i = 0; i < pData->buffLen - 1; i++) {
        plbi->next = plbi + 1;
        plbi ++;
    }
    plbi->next = NULL;

    return;
}

static PMYLISTBOXITEM lstAllocItem (PMYLISTBOXDATA pData)
{
    PMYLISTBOXITEM plbi;

    if (pData->freeList) {
        plbi = pData->freeList;
        pData->freeList = plbi->next;
    }
    else
        plbi = (PMYLISTBOXITEM) malloc (sizeof (MYLISTBOXITEM));
    
    return plbi;
}

static void lstFreeItem (PMYLISTBOXDATA pData, PMYLISTBOXITEM plbi)
{
    if (plbi < pData->buffStart || plbi > pData->buffEnd)
        free (plbi);
    else {
        plbi->next = pData->freeList;
        pData->freeList = plbi;
    }
}

static int lstGetItemWidth (HWND hwnd, PMYLISTBOXITEM plbi)
{
    PCONTROL    pCtrl;
    DWORD       dwStyle;
    
    int         x = 0;

    pCtrl = Control (hwnd);
    dwStyle = pCtrl->dwStyle;

    x = LST_INTER_BMPTEXT;

    if (dwStyle & LBS_CHECKBOX) 
        x += LST_WIDTH_CHECKMARK + LST_INTER_BMPTEXT;

    if (dwStyle & LBS_USEICON && plbi->dwImage) {
        if (plbi->dwFlags &LBIF_USEBITMAP)
            x += ((PBITMAP)(plbi->dwImage))->bmWidth;
        else {
            int width;
            GetIconSize ((HICON) plbi->dwImage, &width, NULL);
            x += width;
        }
        x += LST_INTER_BMPTEXT;
    }

    if (plbi->key && plbi->key[0] != '\0') {
        SIZE size;
        HDC hdc;
        hdc = GetClientDC (hwnd);
        GetTabbedTextExtent (hdc, plbi->key, -1, &size);
        ReleaseDC (hdc);
        x += size.cx + LST_INTER_BMPTEXT;
    }

    return x;
}

static int lstGetMaxWidth(HWND hwnd, PMYLISTBOXDATA pData)
{
    PMYLISTBOXITEM plbi;
    int max = 0;
    int width;
    
    plbi = pData->head;
    
    while (plbi) {
        width = lstGetItemWidth (hwnd, plbi);
        max = MAX(max, width);
        plbi = plbi->next;
    }

    return max;
}

static int lstAddNewItem (DWORD dwStyle, 
        PMYLISTBOXDATA pData, PMYLISTBOXITEM newItem, int pos)
{
    PMYLISTBOXITEM plbi;
    PMYLISTBOXITEM insPosItem = NULL;
    int insPos = 0;

    newItem->next = NULL;
    if (!pData->head)
        insPosItem = NULL;
    else if (dwStyle & LBS_SORT) {
        plbi = pData->head;

        if (pData->str_cmp (newItem->key, plbi->key, (size_t)-1) < 0) {
            insPosItem = NULL;
            insPos = 0;
        }
        else {
            while (plbi->next) {
                if (pData->str_cmp (newItem->key, plbi->next->key, (size_t)-1) <= 0)
                    break;
            
                plbi = plbi->next;
                insPos ++;
            }
            insPosItem = plbi;
        }
    }
    else {
        plbi = pData->head;

        if (pos < 0) {
            while (plbi->next) {
                plbi = plbi->next;
                insPos ++;
            }
            insPosItem = plbi;
        }
        else if (pos == 0) {
            insPosItem = NULL;
            insPos = 0;
        }
        else {
            int index = 1;

            while (plbi) {
                if (pos == index)
                    break;
                plbi = plbi->next;
                index ++;
                insPos ++;
            }
            insPosItem = plbi;
        }
    }

    if (insPosItem) {
        plbi = insPosItem->next;
        insPosItem->next = newItem;
        newItem->next = plbi;

        insPos ++;
    }
    else {
        plbi = pData->head;
        pData->head = newItem;
        newItem->next = plbi;
    }

    pData->itemCount ++;

    return insPos;
}

static PMYLISTBOXITEM lstRemoveItem (PMYLISTBOXDATA pData, int* pos)
{
    int index = 0;
    PMYLISTBOXITEM plbi, prev;

    if (!pData->head)
        return NULL;

    if (*pos < 0 || *pos >= pData->itemCount) 
        return NULL;    
    
/*    
    if (*pos < 0) {
        prev = pData->head;
        plbi = pData->head;
        while (plbi->next) {
            prev = plbi;
            plbi = plbi->next;
            index ++;
        }

        if (plbi == pData->head) {
            pData->head = pData->head->next;
            *pos = 0;
            return plbi;
        }
        else {
            prev->next = plbi->next;
            *pos = index;
            return plbi;
        }
    }
    else  
*/
    if (*pos == 0) {
        plbi = pData->head;
        pData->head = plbi->next;
        return plbi;
    }
    else {
        index = 0;
        prev = pData->head;
        plbi = pData->head;
        while (plbi->next) {
            if (*pos == index)
                break;

            prev = plbi;
            plbi = plbi->next;
            index ++;
        }

        if (plbi == pData->head) {
            pData->head = pData->head->next;
            *pos = 0;
            return plbi;
        }
        else {
            prev->next = plbi->next;
            *pos = index;
            return plbi;
        }
    }

    return NULL;
}

static void lstGetItemsRect (PMYLISTBOXDATA pData, 
                int start, int end, RECT* prc)
{
    if (start < 0)
        start = 0;

    prc->top = (start - pData->itemTop)*pData->itemHeight;

    if (end >= 0)
        prc->bottom = (end - pData->itemTop + 1)*pData->itemHeight;

}

static void lstInvalidateItem (HWND hwnd, PMYLISTBOXDATA pData, int pos)
{
    RECT rcInv;
    
    if (pos < pData->itemTop || pos > (pData->itemTop + pData->itemVisibles))
        return;
    
    GetClientRect (hwnd, &rcInv);
    rcInv.top = (pos - pData->itemTop)*pData->itemHeight;
    rcInv.bottom = rcInv.top + pData->itemHeight;

    InvalidateRect (hwnd, &rcInv, TRUE);
}

static BOOL 
lstInvalidateUnderMultiItem (HWND hwnd, 
        PMYLISTBOXDATA pData, int start, int end)
{
    RECT rcInv;
    int pos;
    int max;

    if (start < 0) start = 0;
    if (end < 0) end = 0;

    pos = start > end ? end : start;
    max = start > end ? start : end;
    
    if (pos > (pData->itemTop + pData->itemVisibles))
        return FALSE;

    if (max < pData->itemTop)
        return FALSE;

    GetClientRect (hwnd, &rcInv);
    lstGetItemsRect (pData, pos, max, &rcInv);

    if (rcInv.top < rcInv.bottom) {
        InvalidateRect (hwnd, &rcInv, TRUE);
    }

    return TRUE;
}

static BOOL 
lstInvalidateUnderItem (HWND hwnd, PMYLISTBOXDATA pData, int pos)
{
    RECT rcInv;
    
    if (pos > (pData->itemTop + pData->itemVisibles))
        return FALSE;

    if (pos < pData->itemTop)
        return FALSE;

    GetClientRect (hwnd, &rcInv);

    lstGetItemsRect (pData, pos, -1, &rcInv);

    if (rcInv.top < rcInv.bottom)
        InvalidateRect (hwnd, &rcInv, TRUE);

    return TRUE;
}

static PMYLISTBOXITEM lstGetItem (PMYLISTBOXDATA pData, int pos)
{
    int i;
    PMYLISTBOXITEM plbi;

    plbi = pData->head;
    for (i=0; i < pos && plbi; i++)
        plbi = plbi->next;

    return plbi;
}

static int lstFindItem (PMYLISTBOXDATA pData, int start, char* key, BOOL bExact)
{
    PMYLISTBOXITEM plbi;
    int keylen = strlen (key);
  
    if (start >= pData->itemCount)
        start = 0;
    else if (start < 0)
        start = 0;

    plbi = lstGetItem (pData, start);

    while (plbi)
    {
        if (bExact && (keylen != strlen (plbi->key))) {
            plbi = plbi->next;
            start ++;
            continue;
        }

        if (pData->str_cmp (key, plbi->key, keylen) == 0)
            return start;
            
        plbi = plbi->next;
        start ++;
    }

    return LB_ERR;
}

static void lstOnDrawSListBoxItems (HWND hWnd, HDC hdc,
                PMYLISTBOXDATA pData, int width)
{
    DWORD dwStyle = GetWindowStyle (hWnd);
    PMYLISTBOXITEM plbi;
    int i;
    int x = 0, y = 0;
    int offset;
	
	//pData->itemHeight = 24;
    plbi = lstGetItem (pData, pData->itemTop);
    for (i = 0; plbi && i < (pData->itemVisibles + 1); i++) {

        if (plbi->dwFlags & LBIF_SELECTED) {
            (void)SetBkMode (hdc, BM_OPAQUE);            
            (void)SetBkMode (hdc, BM_TRANSPARENT);
            SetBkColor (hdc, GetWindowElementColorEx (hWnd, BKC_HILIGHT_NORMAL));
            //SetTextColor (hdc, GetWindowElementColorEx (hWnd, FGC_HILIGHT_NORMAL));
            //SetBrushColor (hdc, GetWindowElementColorEx (hWnd, BKC_HILIGHT_NORMAL));
            SetTextColor (hdc, PIXEL_lightwhite);
            SetBrushColor (hdc, 0xFF0773BB);
            FillBox (hdc, 0, y+2, width, pData->itemHeight - 4);
        }
        else if (plbi->dwFlags & LBIF_DISABLE) {
            gal_pixel tmp;
            (void)SetBkMode (hdc, BM_TRANSPARENT);
            tmp = GetWindowBkColor (hWnd);
            SetBkColor (hdc, tmp);
            
            SetBkColor (hdc,GetWindowElementColorEx (hWnd, FGC_CONTROL_NORMAL)); 
            SetTextColor (hdc, GetWindowElementColorEx (hWnd, FGC_CONTROL_DISABLED));
        }
        else {
            gal_pixel tmp;
            (void)SetBkMode (hdc, BM_TRANSPARENT);
            tmp = GetWindowBkColor (hWnd);
            SetBkColor (hdc, tmp);
            
            SetBkColor (hdc,GetWindowElementColorEx (hWnd, FGC_CONTROL_NORMAL)); 
           // SetTextColor (hdc, PIXEL_black);//GetWindowElementColorEx (hWnd, FGC_CONTROL_NORMAL));           
		   SetTextColor (hdc, PIXEL_lightwhite);
        }

        if (pData->lst_font != INV_LOGFONT) {
            if (plbi->dwFlags & LBIF_BOLDSTYLE)
                pData->lst_font->style |= FS_WEIGHT_BOLD;
            else
                pData->lst_font->style &= ~FS_WEIGHT_BOLD;
        }
        
        x = LST_INTER_BMPTEXT - pData->itemLeft;

        if (dwStyle & LBS_CHECKBOX) {
            if (plbi->dwFlags & LBIF_CHECKED)
                offset = 0;
            else if (plbi->dwFlags & LBIF_PARTCHECKED)
                offset = LST_WIDTH_CHECKMARK << 1;
            else
                offset = LST_WIDTH_CHECKMARK;
			
            x += LST_TEXT_START;
            FillBoxWithBitmapPart (hdc, 
                x, y + ((pData->itemHeight - LST_HEIGHT_CHECKMARK)>>1),
                LST_WIDTH_CHECKMARK, LST_HEIGHT_CHECKMARK,
                0, 0,
                CHECKMARK_BMP,
                offset, 0);

            x += LST_WIDTH_CHECKMARK + LST_INTER_BMPTEXT;
        }

        if (dwStyle & LBS_USEICON && plbi->dwImage) {
            if (plbi->dwFlags & LBIF_USEBITMAP) {
                FillBoxWithBitmap (hdc, x, y, 0, pData->itemHeight, 
                            (PBITMAP) plbi->dwImage);
                x += ((PBITMAP)(plbi->dwImage))->bmWidth;
            }
            else {
                int width;
                DrawIcon (hdc, x, y, 0, pData->itemHeight, 
                            (HICON) plbi->dwImage);
                GetIconSize ((HICON) plbi->dwImage, &width, NULL);
                x += width;
            }
            x += LST_INTER_BMPTEXT;
        }

        TextOut (hdc, x, y + ((pData->itemHeight - GetCurFont (hdc)->size) >> 1), plbi->key);

        y += pData->itemHeight;
        plbi = plbi->next;
    }
}

static int lstSelectItem (HWND hwnd, PMYLISTBOXDATA pData, int newSel)
{
    PMYLISTBOXITEM plbi, newItem;
    int index;
    
    newItem = lstGetItem (pData, newSel);

    if (newItem->dwFlags & LBIF_DISABLE)
        return newSel;

#ifdef _DEBUG
    if (!newItem)
        fprintf (stderr, "ASSERT failed: return value of lstGetItem"
                         " in lstSelectItem.\n");
#endif

    if (GetWindowStyle (hwnd) & LBS_MULTIPLESEL) {
        newItem->dwFlags ^= LBIF_SELECTED;
        return newSel;
    }

    index = 0;
    plbi = pData->head;
    while (plbi) {
        if (plbi->dwFlags & LBIF_SELECTED) {
            if (index != newSel) {
                plbi->dwFlags &= ~LBIF_SELECTED;
                newItem->dwFlags |= LBIF_SELECTED;
                return index;
            }
            break;
        }

        plbi = plbi->next;
        index ++;
    }

    newItem->dwFlags |= LBIF_SELECTED;
    return pData->itemHilighted;
}

static int lstCancelSelected (HWND hwnd, PMYLISTBOXDATA pData)
{
    PMYLISTBOXITEM plbi;
    int index;

    index = 0;
    plbi = pData->head;
    while (plbi) {
        if (plbi->dwFlags & LBIF_SELECTED) {
            RECT rc;

            plbi->dwFlags &= ~LBIF_SELECTED;
            lstGetItemsRect (pData, index, index, &rc);
            InvalidateRect (hwnd, &rc, TRUE);
            return index;
        }

        plbi = plbi->next;
        index ++;
    }

    return -1;
}

static void lstDrawFocusRect (HWND hwnd, HDC hdc, PMYLISTBOXDATA pData)
{
    RECT rc;

    if (pData->itemHilighted < pData->itemTop
            || pData->itemHilighted > (pData->itemTop + pData->itemVisibles))
        return;

    if (pData->dwFlags & LBF_FOCUS) {
        GetClientRect (hwnd, &rc);
        lstGetItemsRect (pData, 
                         pData->itemHilighted, pData->itemHilighted,
                         &rc);
        InflateRect (&rc, -1, -1);
#ifndef _GRAY_SCREEN
        SetPenColor (hdc, PIXEL_lightwhite);
        FocusRect (hdc, rc.left, rc.top, rc.right, rc.bottom); 
#endif
    }
}

static void lstCalcParams (const RECT* rcClient, PMYLISTBOXDATA pData)
{
    pData->itemVisibles = (RECTHP (rcClient)) / pData->itemHeight;
    pData->itemWidth = RECTWP(rcClient);
}

static void change_scrollbar (HWND hwnd, PMYLISTBOXDATA pData, BOOL bShow, int iSBar)
{
    if (pData->sbPolicy == SB_POLICY_ALWAYS)
        EnableScrollBar (hwnd, iSBar, bShow);
    else if (pData->sbPolicy == SB_POLICY_AUTOMATIC)
        ShowScrollBar (hwnd, iSBar, bShow);
}

static void lstSetVScrollInfo (HWND hwnd, PMYLISTBOXDATA pData, BOOL fRedraw)
{
    SCROLLINFO si;

    if (pData->sbPolicy == SB_POLICY_NEVER) {
        ShowScrollBar (hwnd, SB_VERT, FALSE);
        return;
    }

    if (pData->itemVisibles >= pData->itemCount) {
        SetScrollPos (hwnd, SB_VERT, 0);
        change_scrollbar (hwnd, pData, FALSE, SB_VERT);
        return;
    }
    
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMax = pData->itemCount - 1;
    si.nMin = 0;
#if 0
    si.nPage = MIN (pData->itemVisibles, (pData->itemCount - pData->itemTop));
#else
    si.nPage = MIN (pData->itemVisibles, pData->itemCount);
#endif
    si.nPos = pData->itemTop;
    SetScrollInfo (hwnd, SB_VERT, &si, fRedraw);
    EnableScrollBar (hwnd, SB_VERT, TRUE);
    change_scrollbar (hwnd, pData, TRUE, SB_VERT);
}

static void lstSetHScrollInfo (HWND hwnd, PMYLISTBOXDATA pData, BOOL fRedraw)
{
    SCROLLINFO si;

    if (pData->sbPolicy == SB_POLICY_NEVER) {
        ShowScrollBar (hwnd, SB_HORZ, FALSE);
        return;
    }

    if (pData->itemWidth >= pData->itemMaxWidth) {
        SetScrollPos (hwnd, SB_HORZ, 0);
        change_scrollbar (hwnd, pData, FALSE, SB_HORZ);
        return;
    }
    
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMax = pData->itemMaxWidth;
    si.nMin = 0;
    si.nPage = pData->itemWidth;
    si.nPos = pData->itemLeft;
    
    SetScrollInfo (hwnd, SB_HORZ, &si, fRedraw);
    EnableScrollBar (hwnd, SB_HORZ, TRUE);
    change_scrollbar (hwnd, pData, TRUE, SB_HORZ);
}

static void lstSetScrollInfo (HWND hwnd, PMYLISTBOXDATA pData, BOOL fRedraw)
{
    lstSetVScrollInfo (hwnd, pData, fRedraw);
    lstSetHScrollInfo (hwnd, pData, fRedraw);
}

static int MyListboxCtrlProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC             hdc;
    PCONTROL        pCtrl;
    PMYLISTBOXDATA    pData;
    DWORD           dwStyle;
#if _USE_FIXSTR
    int             len;
#endif
    
    pCtrl   = Control(hwnd);
    dwStyle = pCtrl->dwStyle;

    switch (message)
    {
        case MSG_CREATE:
            pData = (MYLISTBOXDATA*) calloc (1, sizeof(MYLISTBOXDATA));
            if (pData == NULL) 
                return -1;
            
            pData->lst_font = CreateLogFontIndirect(GetSystemFont (SYSLOGFONT_CONTROL));
            
            pCtrl->dwAddData2 = (DWORD)pData;
            if (!lstInitListBoxData (hwnd, pCtrl, pData, DEF_LB_BUFFER_LEN)) {
                free (pData);
                return -1;
            }

            lstSetScrollInfo (hwnd, pData, FALSE);
        break;

        case MSG_SIZECHANGED:
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            lstCalcParams ((const RECT*)lParam, pData);
            break;
            
        case MSG_DESTROY:
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if(pData->lst_font != INV_LOGFONT)
                DestroyLogFont(pData->lst_font);
            lstListBoxCleanUp (pData);
            free (pData);
            break;

        case LB_SETSTRCMPFUNC:
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (pData->itemCount == 0 && lParam) {
                pData->str_cmp = (STRCMP)lParam;
                return LB_OKAY;
            }
            return LB_ERR;

        case LB_RESETCONTENT:
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            lstResetListBoxContent (pData);
            lstSetScrollInfo (hwnd, pData, TRUE);
            InvalidateRect (hwnd, NULL, TRUE);
            return 0;
        
        case LB_ADDSTRING:
        case LB_INSERTSTRING:
        {
            char* string = NULL;
            PLISTBOXITEMINFO plbii = NULL;

            PMYLISTBOXITEM newItem;
            int itemWidth;
            int pos;
           
            if (dwStyle & LBS_CHECKBOX || dwStyle & LBS_USEICON) {
                plbii = (PLISTBOXITEMINFO)lParam;
                if (!plbii)
                    return LB_ERR;

                string = plbii->string;
            }
            else {
                string = (char*)lParam;
                if (string == NULL || string [0] == '\0')
                    return LB_ERR;
            }

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            newItem = lstAllocItem (pData);
            if (!newItem) {
                if (dwStyle & LBS_NOTIFY)
                    NotifyParent (hwnd, pCtrl->id, LBN_ERRSPACE);
                return LB_ERRSPACE;
            }

#if _USE_FIXSTR
            len = strlen (string);
            newItem->key = FixStrAlloc (len);
            if (!newItem->key) {
                lstFreeItem (pData, newItem);
                return LB_ERRSPACE;
            }
            if (len > 0)
                strcpy (newItem->key, string);
#else
            newItem->key = strdup (string);
            if (!newItem->key) {
                lstFreeItem (pData, newItem);
                return LB_ERRSPACE;
            }
#endif
            newItem->dwFlags = LBIF_NORMAL;
            if (plbii) {

                if (plbii->cmFlag & CMFLAG_CHECKED)
                    newItem->dwFlags |= LBIF_CHECKED;
                else if (plbii->cmFlag & CMFLAG_PARTCHECKED)
                    newItem->dwFlags |= LBIF_PARTCHECKED;

                if (plbii->cmFlag & IMGFLAG_BITMAP)
                    newItem->dwFlags |= LBIF_USEBITMAP;
                
                if (dwStyle & LBS_USEICON)
                    newItem->dwImage = (DWORD)plbii->hIcon;
                else
                    newItem->dwImage = 0L;
            }
            newItem->dwAddData = 0L;

            if (message == LB_ADDSTRING)
                pos = lstAddNewItem (dwStyle, pData, newItem, -1);
            else
                pos = lstAddNewItem (dwStyle, pData, newItem, (int)wParam);

            itemWidth = lstGetItemWidth (hwnd, newItem);
            if (pData->itemMaxWidth < itemWidth)
                pData->itemMaxWidth = itemWidth;
            
            lstInvalidateUnderItem (hwnd, pData, pos);

            lstSetScrollInfo (hwnd, pData, TRUE);
            return pos;
        }
        
        case LB_DELETESTRING:
        {
            PMYLISTBOXITEM removed;
            int delete;

            delete = (int)wParam;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            removed = lstRemoveItem (pData, &delete);
            if (removed) {
                int width;
                width = lstGetItemWidth (hwnd, removed);
                if (width >= pData->itemMaxWidth)
                    pData->itemMaxWidth = lstGetMaxWidth(hwnd, pData);
#if _USE_FIXSTR
                FreeFixStr (removed->key);
#else
                free (removed->key);
#endif
                lstFreeItem (pData, removed);
                
                pData->itemCount --;

                if (pData->itemTop != 0 
                        && pData->itemCount <= pData->itemVisibles) {
                    pData->itemTop = 0;
                    InvalidateRect (hwnd, NULL, TRUE);
                }
                else {
                    lstInvalidateUnderItem (hwnd, pData, delete);
                    if (delete <= pData->itemTop) {
                        pData->itemTop --;
                        if (pData->itemTop < 0)
                            pData->itemTop = 0;
                    }
                }

                if (pData->itemHilighted >= pData->itemCount) {
                    pData->itemHilighted = pData->itemCount - 1;
                    if (pData->itemHilighted < 0)
                        pData->itemHilighted = 0;
                }

                if (pData->itemHilighted < pData->itemTop)
                    pData->itemHilighted = pData->itemTop;
                if (pData->itemHilighted > ITEM_BOTTOM (pData))
                    pData->itemHilighted = ITEM_BOTTOM (pData);
             
                pData->selItem = pData->itemHilighted;
                lstSetScrollInfo (hwnd, pData, TRUE);
                return LB_OKAY;
            }

            return LB_ERR;
        }

        case LB_FINDSTRING:
            if( *(char*)lParam == '\0' )
                return LB_ERR;
                
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            return lstFindItem(pData, (int)wParam, (char*)lParam, FALSE);

        case LB_FINDSTRINGEXACT:
            if( *(char*)lParam == '\0' )
                return LB_ERR;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            return lstFindItem(pData, (int)wParam, (char*)lParam, TRUE);
    
        case LB_SETTOPINDEX:
        {
            int newTop = (int) wParam;
            
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;

            if (newTop <0)
                newTop = 0;
            else if (newTop > pData->itemCount - pData->itemVisibles)
                newTop = pData->itemCount - pData->itemVisibles;
                
            if (pData->itemTop != newTop) {
                pData->itemTop = newTop;

                if (pData->itemHilighted < pData->itemTop)
                    pData->itemHilighted = pData->itemTop;
                if (pData->itemHilighted > ITEM_BOTTOM (pData))
                    pData->itemHilighted = ITEM_BOTTOM (pData);

                pData->selItem = pData->itemHilighted;
                lstSetScrollInfo (hwnd, pData, TRUE);
                InvalidateRect (hwnd, NULL, TRUE);
            }

            return 0;
        }
    
        case LB_SETCURSEL:
        case LB_SETCARETINDEX:
        {
            int new = (int)wParam;
            int old, newTop;
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (new < 0 || new > pData->itemCount - 1) {
                if (dwStyle & LBS_MULTIPLESEL)
                    return LB_ERR;
                else 
                    return lstCancelSelected (hwnd, pData);
            }

            old = pData->itemHilighted;
            if (new >= 0 && new != old) {
                if (pData->itemCount - new >= pData->itemVisibles)
                    newTop = new;
                else
                    newTop = MAX (pData->itemCount - pData->itemVisibles, 0);

                pData->itemTop = newTop;
                pData->itemHilighted = new;
                pData->selItem = pData->itemHilighted;
                lstSetScrollInfo (hwnd, pData, TRUE);
            }
            if (!(dwStyle & LBS_MULTIPLESEL))
            {
                PMYLISTBOXITEM newItem = lstGetItem (pData, new);
                if (newItem->dwFlags & LBIF_DISABLE) {
                    lstCancelSelected(hwnd, pData);
                    return old;
                }
                else
                    lstSelectItem (hwnd, pData, new);
            }
            InvalidateRect (hwnd, NULL, TRUE);
            return old;
        }
    
        case LB_GETCOUNT:
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            return pData->itemCount;
    
        case LB_GETCURSEL:
        {
            PMYLISTBOXITEM plbi;
            int index = 0;
            
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (dwStyle & LBS_MULTIPLESEL)
                return pData->itemHilighted;

            plbi = pData->head;
            while (plbi) {
                if (plbi->dwFlags & LBIF_SELECTED)
                    return index;

                index ++;
                plbi = plbi->next;
           }
           
           return LB_ERR;
        }
    
        case LB_GETSELCOUNT:
        {
            int nSel;
            PMYLISTBOXITEM plbi;
            
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;

            nSel = 0;
            plbi = pData->head;
            while (plbi) {
                if (plbi->dwFlags & LBIF_SELECTED)
                    nSel ++;
                plbi = plbi->next;
            }
            
            return nSel;
        }
    
        case LB_GETTOPINDEX:
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            return pData->itemTop;
    
        case LB_GETCARETINDEX:
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            return pData->itemHilighted;

        case LB_GETTEXTLEN:
        {
            PMYLISTBOXITEM plbi;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            plbi = lstGetItem (pData, (int)wParam);
            if (plbi)
                return strlen (plbi->key);

            return LB_ERR;
        }
        
        case LB_GETTEXT:
        {
            PMYLISTBOXITEM plbi;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            plbi = lstGetItem (pData, (int)wParam);
            if (plbi) {
                strcpy ((char*)lParam, plbi->key);
                return LB_OKAY;
            }

            return LB_ERR;
        }

        case LB_SETTEXT:
        {
            PMYLISTBOXITEM plbi;
            char* newStr;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            plbi = lstGetItem (pData, (int)wParam);
            if (plbi) {
#if _USE_FIXSTR
                len = strlen ((char*)lParam);
                newStr = FixStrAlloc (len);
                if (newStr) {
                    FreeFixStr (plbi->key);
                    plbi->key = newStr;
                    if (len > 0)
                        strcpy (plbi->key, (char*)lParam);
#else
                newStr = strdup ((const char*)lParam);
                if (newStr) {
                    free (plbi->key);
                    plbi->key = newStr;
#endif
                    lstInvalidateItem (hwnd, pData, (int)wParam);
                }
                else
                    return LB_ERR;
            }

            return LB_ERR;
        }
    
        case LB_GETITEMDATA:
        {
            PMYLISTBOXITEM plbi;
            PLISTBOXITEMINFO plbii;

            if (!(dwStyle & LBS_CHECKBOX || dwStyle & LBS_USEICON)) {
                return LB_ERR;
            }

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (!(plbi = lstGetItem (pData, (int)wParam)))
                return LB_ERR;

            plbii = (PLISTBOXITEMINFO)lParam;
            if (!plbii)
                return LB_ERR;

            if (plbi->dwFlags & LBIF_CHECKED)
                plbii->cmFlag = CMFLAG_CHECKED;
            else if (plbi->dwFlags & LBIF_PARTCHECKED)
                plbii->cmFlag = CMFLAG_PARTCHECKED;
            else
                plbii->cmFlag = CMFLAG_BLANK;

            if (plbi->dwFlags & LBIF_USEBITMAP)
                plbii->cmFlag |= IMGFLAG_BITMAP;

            plbii->hIcon = (HICON)plbi->dwImage;

            return LB_OKAY;
        }
    
        case LB_SETITEMDATA:
        {
            PMYLISTBOXITEM plbi;
            PLISTBOXITEMINFO plbii;

            if (!(dwStyle & LBS_CHECKBOX || dwStyle & LBS_USEICON)) {
                return LB_ERR;
            }

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (!(plbi = lstGetItem (pData, (int)wParam)))
                return LB_ERR;

            plbii = (PLISTBOXITEMINFO)lParam;
            if (!plbii)
                return LB_ERR;

            plbi->dwFlags &= ~LBIF_CHECKMARKMASK;

            if (plbii->cmFlag & CMFLAG_CHECKED) plbi->dwFlags |= LBIF_CHECKED;
            if (plbii->cmFlag & CMFLAG_PARTCHECKED) plbi->dwFlags |= LBIF_PARTCHECKED;
            if (plbii->cmFlag & IMGFLAG_BITMAP) plbi->dwFlags |= LBIF_USEBITMAP;
/*
            switch (plbii->cmFlag) {
                case CMFLAG_CHECKED:
                    plbi->dwFlags |= LBIF_CHECKED;
                break;
                case CMFLAG_PARTCHECKED:
                    plbi->dwFlags |= LBIF_PARTCHECKED;
                break;
            }
*/
            if (dwStyle & LBS_USEICON)
                plbi->dwImage = (DWORD)plbii->hIcon;
            else
                plbi->dwImage = 0;
           
            if (plbii->string && plbii->string[0] != '\0') {
               int oldlen = strlen (plbi->key);
               int newlen = strlen (plbii->string);

               if (oldlen >= newlen) {
                   memcpy (plbi->key, plbii->string, newlen);
                   plbi->key[newlen] = '\0';
               } 
               else {
#if _USE_FIXSTR
                   FreeFixStr (plbi->key);
                   plbi->key = FixStrAlloc (newlen + 1);
                   pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
                   if (!plbi->key) {
                       lstFreeItem (pData, plbi);
                       return LB_ERRSPACE; 
                   }

                   strcpy (plbi->key, plbii->string);
                   plbi->key[newlen] = '\0';
#else
                   free (plbi->key);
                   plbi->key = strdup (plbii->string);
                   if (!plbi->key) {
                       lstFreeItem (pData, plbi);
                       return LB_ERRSPACE; 
                   }
#endif
               }
            }
            
            lstInvalidateItem (hwnd, pData, (int)wParam);

            return LB_OKAY;
        }
        
        case LB_GETITEMADDDATA:
        {
            PMYLISTBOXITEM plbi;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (!(plbi = lstGetItem (pData, (int)wParam)))
                return LB_ERR;

            return plbi->dwAddData;
        }
    
        case LB_SETITEMADDDATA:
        {
            PMYLISTBOXITEM plbi;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (!(plbi = lstGetItem (pData, (int)wParam)))
                return LB_ERR;

            plbi->dwAddData = (DWORD)lParam;

            return LB_OKAY;
        }
        
        case LB_GETCHECKMARK:
        {
            PMYLISTBOXITEM plbi;

            if (!(dwStyle & LBS_CHECKBOX))
                return LB_ERR;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (!(plbi = lstGetItem (pData, (int)wParam)))
                return LB_ERR;

            if (plbi->dwFlags & LBIF_CHECKED)
                return CMFLAG_CHECKED;

            if (plbi->dwFlags & LBIF_PARTCHECKED)
                return CMFLAG_PARTCHECKED;

            return CMFLAG_BLANK;
        }

        case LB_SETCHECKMARK:
        {
            PMYLISTBOXITEM plbi;

            if (!(dwStyle & LBS_CHECKBOX))
                return LB_ERR;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (!(plbi = lstGetItem (pData, (int)wParam)))
                return LB_ERR;

            plbi->dwFlags &= ~LBIF_CHECKMARKMASK;
            switch (lParam) {
                case CMFLAG_CHECKED:
                    plbi->dwFlags |= LBIF_CHECKED;
                break;
                case CMFLAG_PARTCHECKED:
                    plbi->dwFlags |= LBIF_PARTCHECKED;
                break;
            }
                
            lstInvalidateItem (hwnd, pData, (int)wParam);

            return LB_OKAY;
        }

        case LB_GETSELITEMS:
        {
            int  nItem;
            int  nSel = 0;
            int  index = 0;
            int* pInt;
            PMYLISTBOXITEM plbi;

            nItem = (int)wParam;
            pInt  = (int*)lParam;
            
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            plbi = pData->head;
            while (plbi) {

                if (plbi->dwFlags & LBIF_SELECTED) {
                    if (pInt) {
                        if (nSel < nItem)
                            *(pInt + nSel) = index;
                        else 
                            return nItem;
                    }
                    nSel ++;
                }
                
                plbi = plbi->next;
                index ++;
            }
            
            return nSel;
        }
        break;
    
        case LB_GETSEL:
        {
            PMYLISTBOXITEM plbi;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            plbi = lstGetItem (pData, (int)wParam);
            if (plbi)
                return plbi->dwFlags & LBIF_SELECTED;

            return LB_ERR;
        }

        case LB_SETSEL:
        {
            PMYLISTBOXITEM plbi;

            if (!(dwStyle & LBS_MULTIPLESEL))
                return LB_ERR;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            plbi = lstGetItem (pData, (int)lParam);
            if (plbi) {
                if (plbi->dwFlags & LBIF_DISABLE)
                    return LB_ERR;

                if (wParam == -1)
                    plbi->dwFlags ^= LBIF_SELECTED;
                else if (wParam == 0)
                    plbi->dwFlags &= ~LBIF_SELECTED;
                else
                    plbi->dwFlags |= LBIF_SELECTED;

                lstInvalidateItem (hwnd, pData, (int)lParam);
            }

            return LB_ERR;
        }
    
        case LB_GETITEMHEIGHT:
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            return pData->itemHeight;
    
        case LB_SETITEMHEIGHT:
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (pData->itemHeight != LOWORD (lParam)) {
                RECT rcClient;
                
                pData->itemHeight = LOWORD (lParam);
                if (pData->itemHeight < pCtrl->pLogFont->size)
                    pData->itemHeight = pCtrl->pLogFont->size;

                GetClientRect (hwnd, &rcClient);
                lstCalcParams (&rcClient, pData);
                
                lstSetScrollInfo (hwnd, pData, TRUE);
                InvalidateRect (hwnd, NULL, TRUE);
            }

            return  pData->itemHeight;

        case MSG_SETFOCUS:
        {
            RECT rc;
           
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;

            if (pData->dwFlags & LBF_FOCUS)
                break;
                
            pData->dwFlags |= LBF_FOCUS;

            if (pData->itemHilighted >= 0) {
                GetClientRect (hwnd, &rc);
                lstGetItemsRect (pData, pData->itemHilighted, pData->itemHilighted, &rc);
                InvalidateRect (hwnd, &rc, TRUE);
            }

            if (dwStyle & LBS_NOTIFY)
                NotifyParent (hwnd, pCtrl->id, LBN_SETFOCUS);
            break;
        }
        
        case MSG_KILLFOCUS:
        {
            RECT rc;
            
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            
            if (pData->itemHilighted >= 0) {
                GetClientRect (hwnd, &rc);
                lstGetItemsRect (pData, pData->itemHilighted, pData->itemHilighted, &rc);
                InvalidateRect (hwnd, &rc, TRUE);
            }

            pData->dwFlags &= ~LBF_FOCUS;

            if (dwStyle & LBS_NOTIFY)
            {
                NotifyParent (hwnd, pCtrl->id, LBN_SELCANCEL);
                NotifyParent (hwnd, pCtrl->id, LBN_KILLFOCUS);
            }
            break;
        }

        case MSG_GETDLGCODE:
            return DLGC_WANTARROWS | DLGC_WANTCHARS | DLGC_WANTENTER;

        case MSG_FONTCHANGED:
        {
            RECT rcClient;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (pData->itemHeight < pCtrl->pLogFont->size)
                pData->itemHeight = pCtrl->pLogFont->size;

            if (pData->lst_font != INV_LOGFONT) {
                DestroyLogFont(pData->lst_font);
                pData->lst_font = INV_LOGFONT;
            }
            pData->lst_font = CreateLogFontIndirect(GetWindowFont(hwnd));
            
            GetClientRect (hwnd, &rcClient);
            lstCalcParams (&rcClient, pData);
                
            lstSetScrollInfo (hwnd, pData, TRUE);
            InvalidateRect (hwnd, NULL, TRUE);
            return 0;
        }

        case MSG_GETTEXTLENGTH:
        case MSG_GETTEXT:
        case MSG_SETTEXT:
            return -1;
            
        case MSG_PAINT:
        {
            RECT rc;
            
            hdc = BeginPaint (hwnd);
            GetClientRect (hwnd, &rc);
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (pData->lst_font != INV_LOGFONT) {
                SelectFont(hdc, pData->lst_font);
            }
            lstOnDrawSListBoxItems (hwnd, hdc, pData, RECTW (rc));
            //lstDrawFocusRect (hwnd, hdc, pData);

            EndPaint (hwnd, hdc);
        }
        return 0;
    
        case MSG_LBUTTONDBLCLK:
            if (dwStyle & LBS_NOTIFY)
                NotifyParent (hwnd, pCtrl->id, LBN_DBLCLK);
        break;
    
        case MSG_MOUSEMOVE:
        case MSG_LBUTTONDOWN:
            if ((dwStyle & LBS_MULTIPLESEL) 
                || !(dwStyle & LBS_MOUSEFOLLOW))
                break;

        case MSG_LBUTTONUP:
        {
            int oldSel, mouseX, mouseY, hit;
            RECT rcInv;
            BOOL click_mark = FALSE;
            PMYLISTBOXITEM hitItem;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (pData->itemCount == 0)
                break;

            mouseX = LOSWORD (lParam);
            mouseY = HISWORD (lParam);
            hit = mouseY / pData->itemHeight;
            hit += pData->itemTop;
            
            if (hit >= pData->itemCount)
                break;

            hitItem = lstGetItem (pData, hit);
            if (hitItem->dwFlags & LBIF_DISABLE)
                break;

            oldSel = lstSelectItem (hwnd, pData, hit);

            if (dwStyle & LBS_CHECKBOX) {
                if (mouseX > 0 && mouseX < LST_WIDTH_CHECKMARK+LST_TEXT_START) {
                    click_mark = TRUE;
                    if (message == MSG_LBUTTONUP && dwStyle & LBS_NOTIFY)
                        NotifyParent (hwnd, pCtrl->id, LBN_CLICKCHECKMARK);
                    
                    if (dwStyle & LBS_AUTOCHECK) {
                        if (message == MSG_LBUTTONUP) {
                            PMYLISTBOXITEM plbi;

                            plbi = lstGetItem (pData, hit);

                            switch (plbi->dwFlags & LBIF_CHECKMARKMASK) {
                            case LBIF_CHECKED:
                                plbi->dwFlags &= ~LBIF_CHECKMARKMASK;
                                if (message == MSG_LBUTTONUP && dwStyle & LBS_NOTIFY)
                                    NotifyParent (hwnd, pCtrl->id, LBN_SELCANCEL);
                                break;
                            default:
                                plbi->dwFlags &= ~LBIF_CHECKMARKMASK;
                                plbi->dwFlags |= LBIF_CHECKED;
                                break;
                            }
                        }
                
                        lstInvalidateItem (hwnd, pData, hit);
                    }
                }
            }
            
            GetClientRect (hwnd, &rcInv);
            if (oldSel >= 0) {
                if (oldSel >= pData->itemTop 
                        && (oldSel <= pData->itemTop + pData->itemVisibles)) {
                    lstGetItemsRect (pData, oldSel, oldSel, &rcInv);
                    InvalidateRect (hwnd, &rcInv, TRUE);
                }
            }

            lstGetItemsRect (pData, pData->itemHilighted, pData->itemHilighted, &rcInv);
            InvalidateRect (hwnd, &rcInv, TRUE);
            pData->itemHilighted = hit;

            lstGetItemsRect (pData, hit, hit, &rcInv);
            InvalidateRect (hwnd, &rcInv, TRUE);

            lstSetScrollInfo (hwnd, pData, TRUE);

            if ( message == MSG_LBUTTONUP && (dwStyle & LBS_NOTIFY) ) {
                if (dwStyle & LBS_MOUSEFOLLOW) {
                    oldSel = pData->selItem;
                    pData->selItem = hit;
                }
                if (oldSel != hit)
                    NotifyParent (hwnd, pCtrl->id, LBN_SELCHANGE);
                if (!click_mark)
                    NotifyParent (hwnd, pCtrl->id, LBN_CLICKED);
            }
        }
        return 0;
    
        case LB_GETITEMRECT:
        {
            RECT rcClient, *rcItem;
            int index;
            
            rcItem = (RECT*)lParam;
            index = (int)wParam;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (pData->itemCount == 0 || index > pData->itemCount)
                return LB_ERR;

            GetClientRect ( hwnd, &rcClient );
            rcItem->right = rcClient.right;
            rcItem->left = rcClient.left;
            rcItem->top = (index-pData->itemTop)*pData->itemHeight;
            rcItem->bottom = (index-pData->itemTop+1)*(pData->itemHeight);
            return LB_OKAY;
        }    

        case MSG_KEYDOWN:
        {
            int oldSel, newSel, newTop;
            RECT rcInv;
            PMYLISTBOXITEM pnewItem;
            
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            if (pData->itemCount == 0)
                break;

            newTop = pData->itemTop;
            newSel = pData->itemHilighted;
            
            switch (LOWORD (wParam))
            {
                case SCANCODE_HOME:
                    newSel = 0;
                    newTop = 0;
                    break;
                
                case SCANCODE_END:
                    newSel = pData->itemCount - 1;
                    if (pData->itemCount > pData->itemVisibles)
                        newTop = pData->itemCount - pData->itemVisibles;
                    else
                        newTop = 0;
                    break;
                
                case SCANCODE_CURSORBLOCKDOWN:
                    newSel ++;
                    if (newSel >= pData->itemCount)
                        return 0;
                    if (newSel > ITEM_BOTTOM (pData))
                        newTop ++;
#if 0
                        newTop = newSel-pData->itemVisibles+1; /* sure to newsel is at bottom */
#endif
                    break;
                
                case SCANCODE_CURSORBLOCKUP:
                    newSel --;
                    if (newSel < 0)
                        return 0;
                    if (newSel < pData->itemTop)
                        newTop --;
#if 0
                        newTop = newSel;
#endif
                    break;
                
                case SCANCODE_PAGEDOWN:
                    newSel += pData->itemVisibles;
                    if (newSel >= pData->itemCount)
                        newSel = pData->itemCount - 1;

                    if (pData->itemCount - newSel >= pData->itemVisibles)
                        newTop = newSel;
                    else
                        newTop = MAX (pData->itemCount-pData->itemVisibles, 0);
                    break;

                case SCANCODE_PAGEUP:
                    newSel -= pData->itemVisibles;
                    if (newSel < 0)
                        newSel = 0;
                        
                    newTop -= pData->itemVisibles;
                    if (newTop < 0)
                        newTop = 0;
                    break;
                
                case SCANCODE_KEYPADENTER:
                case SCANCODE_ENTER:
                    if (dwStyle & LBS_NOTIFY)
                        NotifyParent (hwnd, pCtrl->id, LBN_ENTER);
                    return 0;

                default:
                    return 0;
            }
            GetClientRect (hwnd, &rcInv);
            if (pData->itemHilighted != newSel) {
                if (pData->itemTop != newTop) {
                    pData->itemTop = newTop;
                    pData->itemHilighted = newSel;
                    pData->selItem = pData->itemHilighted;
                    if (!(dwStyle & LBS_MULTIPLESEL)) {
                        pnewItem = lstGetItem(pData, newSel);
                        if(pnewItem->dwFlags&LBIF_DISABLE)
                            lstCancelSelected(hwnd, pData);
                        else
                        {
                            oldSel = lstSelectItem (hwnd, pData, newSel);
                            if ((dwStyle & LBS_NOTIFY) && (oldSel != newSel))
                                NotifyParent (hwnd, pCtrl->id, LBN_SELCHANGE);
                        }
                    }
                    InvalidateRect (hwnd, NULL, TRUE);
                }
                else {
                    if (!(dwStyle & LBS_MULTIPLESEL)) {
                        pnewItem = lstGetItem(pData, newSel);
                        if(pnewItem->dwFlags&LBIF_DISABLE) {
                            lstCancelSelected(hwnd, pData);
                            InvalidateRect(hwnd, NULL, TRUE);
                        }
                        else
                        {
                            oldSel = lstSelectItem (hwnd, pData, newSel);
                            if ((dwStyle & LBS_NOTIFY) && (oldSel != newSel))
                                NotifyParent (hwnd, pCtrl->id, LBN_SELCHANGE);
                            if (oldSel >= 0) {
                                if (oldSel >= pData->itemTop 
                                        && oldSel <= (ITEM_BOTTOM (pData) + 1)) {
                                    lstGetItemsRect (pData, oldSel, oldSel, &rcInv);
                                    InvalidateRect (hwnd, &rcInv, TRUE);
                                }
                            }
                        }
                        
                        if (newSel < newTop) {
                            pData->itemHilighted = newSel;
                            pData->selItem = pData->itemHilighted;
                            break;
                        }
                            
                        pData->itemHilighted = newSel;
                        pData->selItem = pData->itemHilighted;
                        lstGetItemsRect (pData, newSel, newSel, &rcInv);
                        InvalidateRect (hwnd, &rcInv, TRUE);
                    }
                    else {
                        pnewItem = lstGetItem(pData, newSel);
                        if(pnewItem->dwFlags&LBIF_DISABLE)
                            break;
                        lstGetItemsRect (pData, pData->itemHilighted, pData->itemHilighted, &rcInv);
                        InvalidateRect (hwnd, &rcInv, TRUE);
                        pData->itemHilighted = newSel;
                        lstGetItemsRect (pData, newSel, newSel, &rcInv);
                        InvalidateRect (hwnd, &rcInv, TRUE);
                    }
                }
                lstSetScrollInfo (hwnd, pData, TRUE);
            }
        }
        break;

		case MSG_MOUSE_SCROLL:
		{		//printf("[DEBUG] ........wParam = %d\n",wParam);
			message = MSG_VSCROLL;//将鼠标滚轮的滚动消息转换为垂直滑动条滑动消息
			SendMessage(hwnd, message, wParam, lParam);
	        return 0;
	    }

        case MSG_CHAR:
        {
            unsigned char head [2];
            int index;
            int newTop;
            PMYLISTBOXITEM plbi;

            if (HIBYTE (wParam))
                break;
           
            head [0] = LOBYTE (wParam);
            head [1] = '\0';

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;

            if (head[0] == ' ') {
                plbi = lstGetItem (pData, pData->itemHilighted);
                if (plbi->dwFlags & LBIF_DISABLE)
                    break;

                if (dwStyle & LBS_MULTIPLESEL) {
                    RECT rcInv;
                    GetClientRect (hwnd, &rcInv);
                    lstSelectItem (hwnd, pData, pData->itemHilighted);
                    lstGetItemsRect (pData, 
                            pData->itemHilighted, 
                            pData->itemHilighted,
                            &rcInv);
                    InvalidateRect (hwnd, &rcInv, TRUE);
                }
                else if (dwStyle & LBS_CHECKBOX) {
                    if (dwStyle & LBS_NOTIFY)
                        NotifyParent (hwnd, pCtrl->id, LBN_CLICKCHECKMARK);
                    
                    if (dwStyle & LBS_AUTOCHECK) {
#if 0
                        PMYLISTBOXITEM plbi;
                        plbi = lstGetItem (pData, pData->itemHilighted);
#endif

                        switch (plbi->dwFlags & LBIF_CHECKMARKMASK) {
                        case LBIF_CHECKED:
                            plbi->dwFlags &= ~LBIF_CHECKMARKMASK;
                            if (dwStyle & LBS_NOTIFY)
                                NotifyParent (hwnd, pCtrl->id, LBN_SELCANCEL);
                            break;
                        default:
                            plbi->dwFlags &= ~LBIF_CHECKMARKMASK;
                            plbi->dwFlags |= LBIF_CHECKED;
                            break;
                        }
                
                        lstInvalidateItem (hwnd, pData, pData->itemHilighted);
                    }
                }
                break;
            }

            index = lstFindItem (pData, pData->itemHilighted + 1, (char*)head, FALSE);
            if (index < 0) {
                index = lstFindItem (pData, 0, (char*)head, FALSE);
            }

            if (index >= 0) {
                if (pData->itemCount - index >= pData->itemVisibles)
                    newTop = index;
                else
                    newTop = MAX (pData->itemCount - pData->itemVisibles, 0);

                pData->itemTop = newTop;
                pData->itemHilighted = index;
                pData->selItem = pData->itemHilighted;
                if (!(dwStyle & LBS_MULTIPLESEL)) {
                    plbi = lstGetItem(pData, index);
                    if(plbi->dwFlags & LBIF_DISABLE)
                        lstCancelSelected(hwnd, pData);
                    else {
                        int oldSel = lstSelectItem (hwnd, pData, index);
                        if ((dwStyle & LBS_NOTIFY) && (oldSel != index))
                            NotifyParent (hwnd, pCtrl->id, LBN_SELCHANGE);
                    }
                }
                InvalidateRect (hwnd, NULL, TRUE);
                lstSetScrollInfo (hwnd, pData, TRUE);
            }
        }
        break;
    
        case MSG_VSCROLL:
        {
            int newTop;
            int scrollHeight = 0;
            
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            newTop = pData->itemTop;
            switch(wParam)
            {
                case SB_LINEDOWN:
                    if (ITEM_BOTTOM (pData) < (pData->itemCount - 1)) {
                        newTop ++;
                        scrollHeight = -pData->itemHeight;
                    }
                break;
                
                case SB_LINEUP:
                    if (pData->itemTop > 0) {
                        newTop --;
                        scrollHeight = pData->itemHeight;
                    }
                break;
                
                case SB_PAGEDOWN:
                    if ((pData->itemTop + (pData->itemVisibles << 1)) <=
                            pData->itemCount)
                        newTop += pData->itemVisibles;
                    else
                        newTop = pData->itemCount - pData->itemVisibles;
                    
                    if (newTop < 0)
                        return 0;

                    scrollHeight = -(newTop - pData->itemTop)
                                    *pData->itemHeight;
                break;

                case SB_PAGEUP:
                    if (pData->itemTop >= pData->itemVisibles)
                        newTop -= pData->itemVisibles;
                    else
                        newTop = 0;

                    scrollHeight = (pData->itemTop - newTop)*pData->itemHeight;
                break;

                case SB_THUMBTRACK:
                    newTop = MIN ((int)lParam, (pData->itemCount - pData->itemVisibles)); 
                    scrollHeight = (pData->itemTop - newTop)*pData->itemHeight;
                break;
            }
            
            if (scrollHeight) {
                pData->itemTop = newTop;
                lstSetScrollInfo (hwnd, pData, TRUE);
                InvalidateRect (hwnd, NULL, TRUE);
                return 0;
            }
        }
        break;

        case MSG_HSCROLL:
        {    
            int newLeft;
            int scrollWidth = 0;
 
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            newLeft = pData->itemLeft;
            switch (wParam)
            {               
                case SB_LINERIGHT:
                    if (pData->itemMaxWidth > (pData->itemWidth + newLeft)) {
                        newLeft++;
                        scrollWidth = 1;
                    }
                    
                break;
                
                case SB_LINELEFT:
                    if (newLeft > 0) {
                        newLeft--;                   
                        scrollWidth = 1;
                    }
                    
                break;
                
                case SB_PAGELEFT:
                    if (newLeft >= pData->itemWidth) {
                        newLeft -= pData->itemWidth;
                        scrollWidth = 1;
                    }
                    else if (newLeft > 0) {
                        newLeft = 0;
                        scrollWidth = 1;
                    }
                            
                break;
                
                case SB_PAGERIGHT:
                    if (newLeft + (pData->itemWidth << 1) <= pData->itemMaxWidth) {
                        newLeft += pData->itemWidth;
                        scrollWidth = 1;
                    }
                    else  {
                        newLeft = pData->itemMaxWidth - pData->itemWidth;
                        scrollWidth = 1;
                    }
                    
                break;

                case SB_THUMBTRACK:
                    newLeft = MIN((int)lParam, (pData->itemMaxWidth - pData->itemWidth));
                    scrollWidth = (pData->itemLeft - newLeft);
                    
                break;              
            }

            if (scrollWidth) {
                pData->itemLeft = newLeft;
                lstSetScrollInfo (hwnd, pData, TRUE);
                InvalidateRect (hwnd, NULL, TRUE);
                return 0;
            }                
        }
        break;

        case LB_SETITEMDISABLE:
        {
            PMYLISTBOXITEM item;
            int index = (int)wParam;
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;

            if (pData->itemCount <= 0 || index < 0 || index > pData->itemCount)
                return LB_ERR;

            item = lstGetItem (pData, index); 
            if (lParam)        
                item->dwFlags |= LBIF_DISABLE;    
            else        
                item->dwFlags &=~LBIF_DISABLE;

            return lstInvalidateUnderItem (hwnd, pData, index);
        }

        case LB_GETITEMDISABLE:
        {
            PMYLISTBOXITEM item;
            int index = (int)wParam;
            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;

            if (pData->itemCount <= 0 || index < 0 || index > pData->itemCount)
                return LB_ERR;
            item = lstGetItem (pData, index); 
            return item->dwFlags&LBIF_DISABLE;
        }
        break;

        case LB_SETITEMBOLD:
        {
            PMYLISTBOXITEM item; 
            int index = (int) wParam;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;

            if(pData->lst_font == INV_LOGFONT)
                return LB_ERR;

            if (pData->itemCount <= 0 || index < 0 || index > pData->itemCount)
                return LB_ERR;

            item = lstGetItem (pData, index); 

            if (lParam)        
                item->dwFlags |= LBIF_BOLDSTYLE;    
            else        
                item->dwFlags &=~LBIF_BOLDSTYLE;

            return lstInvalidateUnderItem (hwnd, pData, index);
        }

        case LB_MULTIADDITEM:
        {
            char* string = NULL;
            PLISTBOXITEMINFO plbii = NULL;
            char** stringarray = NULL;
            
            PMYLISTBOXITEM newItem;
            int itemWidth;
            int start, pos = -1;
            int i;

            pData = (PMYLISTBOXDATA)pCtrl->dwAddData2;
            start = pData->itemCount;
            
            if (dwStyle & LBS_CHECKBOX || dwStyle & LBS_USEICON) {
                plbii = (PLISTBOXITEMINFO)lParam;
                if (!plbii)
                    return LB_ERR;
            }
            else {
                stringarray = (char* *)lParam;
                if (stringarray == NULL)
                    return LB_ERR;
            }
            
            for (i = 0; i < (int)wParam; i++)
            {
                if (dwStyle & LBS_CHECKBOX || dwStyle & LBS_USEICON)
                    string = plbii[i].string;
                else
                    string = stringarray[i];
                
                if(string == NULL || string[0] == '\0')
                    break;
                
                newItem = lstAllocItem (pData);
                if (!newItem) {
                    if (dwStyle & LBS_NOTIFY)
                        NotifyParent (hwnd, pCtrl->id, LBN_ERRSPACE);
                    return LB_ERRSPACE;
                }
#if _USE_FIXSTR
                len = strlen (string);
                newItem->key = FixStrAlloc (len);
                if (!newItem->key) {
                    lstFreeItem (pData, newItem);
                    return LB_ERRSPACE;
                }
                if (len > 0)
                    strcpy (newItem->key, string);
#else
                newItem->key = strdup (string);
                if (!newItem->key) {
                    lstFreeItem (pData, newItem);
                    return LB_ERRSPACE;
                }
#endif
                newItem->dwFlags = LBIF_NORMAL;

                if (dwStyle & LBS_CHECKBOX || dwStyle & LBS_USEICON) {
                    if (plbii[i].cmFlag & CMFLAG_CHECKED)
                        newItem->dwFlags |= LBIF_CHECKED;
                    else if (plbii[i].cmFlag & CMFLAG_PARTCHECKED)
                        newItem->dwFlags |= LBIF_PARTCHECKED;

                    if (plbii[i].cmFlag & IMGFLAG_BITMAP)
                        newItem->dwFlags |= LBIF_USEBITMAP;

                    if (dwStyle & LBS_USEICON)
                        newItem->dwImage = (DWORD)plbii[i].hIcon;
                    else
                        newItem->dwImage = 0L;
               }     
                newItem->dwAddData = 0L;
                pos = lstAddNewItem (dwStyle, pData, newItem, -1);
               
                itemWidth = lstGetItemWidth (hwnd, newItem);
                if (pData->itemMaxWidth < itemWidth)
                    pData->itemMaxWidth = itemWidth;                
            }

            lstInvalidateUnderMultiItem (hwnd, pData, start, pos);
            lstSetScrollInfo (hwnd, pData, TRUE);
            return pos;
        }

        default:
            break;
    }
    
    return DefaultControlProc (hwnd, message, wParam, lParam);
}



