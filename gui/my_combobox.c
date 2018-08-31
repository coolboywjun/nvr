/*
** $Id: combobox.c 8093 2007-11-16 07:37:30Z weiym $
**
** cobmobox.c: the cobmobox control.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
**
** NOTE: Originally by Jiao Libo
**
** Create date: 2001/08/06
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


#include <minigui/cliprect.h>
#include <minigui/internals.h>
#include <minigui/ctrlclass.h>
#include "my_combobox.h"
#include "my_listbox.h"
#include "my_button.h"
#include "gui_core.h"
#include "ui_config.h"


//#include "ctrlmisc.h"

#define  LEN_SPINVALUE          50

#define  IDC_CEDIT              100
#define  IDC_CLISTBOX           101   
#define  IDC_SPIN_BUTTON		102

#define  INTER_EDIT_BUTTON      0 
#define  INTER_EDIT_LISTBOX     4 
#define  DEF_LISTHEIGHT         60

#define  COMBOX_BITMAP_SIZE			21

static const BITMAP*        bmp_downarrow;
static const BITMAP*        bmp_updownarrow;
static const BITMAP*        bmp_leftrightarrow;

#define ARROWBMP_DOWN       bmp_downarrow
#define ARROWBMP_UPDOWN     bmp_updownarrow
#define ARROWBMP_LEFTRIGHT  bmp_leftrightarrow


static BITMAP bmp_down;

static int MyComboBoxCtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

BOOL RegisterMyComboBoxControl (void)
{
    WNDCLASS WndClass;

#ifdef _PHONE_WINDOW_STYLE
    if ((bmp_downarrow = GetStockBitmap (STOCKBMP_DOWNARROW, 0, 0)) == NULL)
        return FALSE;

    if ((bmp_updownarrow = GetStockBitmap (STOCKBMP_UPDOWNARROW, 0, 0)) == NULL)
        return FALSE;

    if ((bmp_leftrightarrow = GetStockBitmap (STOCKBMP_LEFTRIGHTARROW, 0, 0)) == NULL)
        return FALSE;
#else
    if ((bmp_downarrow = GetStockBitmap (STOCKBMP_DOWNARROW, -1, -1)) == NULL)
        return FALSE;

    if ((bmp_updownarrow = GetStockBitmap (STOCKBMP_UPDOWNARROW, -1, -1)) == NULL)
        return FALSE;

    if ((bmp_leftrightarrow = GetStockBitmap (STOCKBMP_LEFTRIGHTARROW, -1, -1)) == NULL)
        return FALSE;
#endif

	LoadBitmapFromFile(HDC_SCREEN, &bmp_down, "nvr_res/combox_down.bmp");


    WndClass.spClassName = MY_COMBOBOX;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (0);
    WndClass.iBkColor    = PIXEL_lightwhite;//GetWindowElementColor (BKC_CONTROL_DEF);
    WndClass.WinProc     = MyComboBoxCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

void MyComboBoxControlCleanup (void)
{
	UnloadBitmap(&bmp_down);
}

static int DefCBProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);


static void ComboBoxDrawSpinButton (HWND hwnd, HDC hdc)
{	
    UINT uFormat = 0;
	PMYCOMBOBOXDATA pData;
    PCONTROL pCtrl;
 	RECT textRect;
	const char* edit_text = GetWindowCaption (hwnd);
    pCtrl = Control (hwnd); 

	DWORD dwStyle;
	dwStyle = pCtrl->dwStyle;	
	pData = (PMYCOMBOBOXDATA)pCtrl->dwAddData2;
	uFormat |= DT_LEFT| DT_VCENTER|DT_SINGLELINE;
	CopyRect(&textRect, &pData->EditRect);
	textRect.left += 4;
	
	if (dwStyle & WS_DISABLED)
	{			
		//printf("disable\r\n");
		SetBkMode (hdc, BM_TRANSPARENT);
	    SetTextColor (hdc, PIXEL_darkgray);			
		SetBkColor(hdc,PIXEL_EDIT_BKG_DISABLE);
		SetBrushColor(hdc, PIXEL_EDIT_BKG_DISABLE);
	}
	else
	{			
		//printf("enable\r\n");
		SetBkMode (hdc, BM_TRANSPARENT);
	    SetTextColor (hdc, PIXEL_black);			
		SetBkColor(hdc,PIXEL_EDIT_BKG_HILIGHT);
		SetBrushColor(hdc, PIXEL_EDIT_BKG_HILIGHT);		
	}
#ifdef _ROUND_RECT_STYLE
	RoundRect (hdc, pData->EditRect.left,pData->EditRect.top,
                RECTW(pData->EditRect)+3, RECTH(pData->EditRect)- 1, 3, 3);
#else
    FillBox(hdc,pData->EditRect.left,pData->EditRect.top,
                RECTW(pData->EditRect),RECTH(pData->EditRect));
#endif
    DrawText (hdc, edit_text, -1, &textRect, uFormat);
	
}

static void GetSpinBox (PMYCOMBOBOXDATA pData, DWORD dwStyle, const RECT* rect, RECT* edit_box)
{
    if (dwStyle & CBS_SPINARROW_TOPBOTTOM) {

        pData->SpinBmp = ARROWBMP_UPDOWN;

        edit_box->left = rect->left;
        edit_box->top = rect->top + bmp_updownarrow->bmHeight/2 + INTER_EDIT_BUTTON;
        edit_box->right = rect->right;
        edit_box->bottom = rect->bottom - bmp_updownarrow->bmHeight/2 - INTER_EDIT_BUTTON;

        pData->IncSpinBox.left = (rect->right - bmp_updownarrow->bmWidth)/2;
        pData->IncSpinBox.right = pData->IncSpinBox.left + bmp_updownarrow->bmWidth;
        pData->IncSpinBox.top = rect->top;
        pData->IncSpinBox.bottom = rect->top + bmp_updownarrow->bmHeight/2;

        pData->DecSpinBox.left = pData->IncSpinBox.left;
        pData->DecSpinBox.right = pData->IncSpinBox.right;
        pData->DecSpinBox.top = rect->bottom - bmp_updownarrow->bmHeight/2;
        pData->DecSpinBox.bottom = rect->bottom;
    }
    else if (dwStyle & CBS_SPINARROW_LEFTRIGHT) {
        pData->SpinBmp = ARROWBMP_LEFTRIGHT;

        edit_box->left = rect->left + bmp_updownarrow->bmWidth/2 + INTER_EDIT_BUTTON*2;
        edit_box->top = rect->top;
        edit_box->bottom = rect->bottom;
        edit_box->right = rect->right - bmp_updownarrow->bmWidth/2 - INTER_EDIT_BUTTON*2;

        pData->DecSpinBox.left = rect->left;
        pData->DecSpinBox.top = (rect->bottom - bmp_leftrightarrow->bmHeight)/2;
        pData->DecSpinBox.right = rect->left + bmp_leftrightarrow->bmWidth/2;
        pData->DecSpinBox.bottom = pData->DecSpinBox.top + bmp_updownarrow->bmHeight;

        pData->IncSpinBox.left = rect->right - bmp_updownarrow->bmWidth/2 - INTER_EDIT_BUTTON;
        pData->IncSpinBox.top = pData->DecSpinBox.top;
        pData->IncSpinBox.right = rect->right - INTER_EDIT_BUTTON;
        pData->IncSpinBox.bottom = pData->DecSpinBox.bottom;
    }
    else {
		
        int vcenter = (rect->bottom - rect->top)/2;

        pData->SpinBmp = ARROWBMP_UPDOWN;

        edit_box->left = rect->left;
        edit_box->top = rect->top;
        edit_box->bottom = rect->bottom;
        edit_box->right = rect->right - bmp_updownarrow->bmWidth - INTER_EDIT_BUTTON;

        pData->IncSpinBox.left = rect->right - bmp_updownarrow->bmWidth;
        pData->IncSpinBox.top = vcenter - bmp_updownarrow->bmHeight/2 - 1;
        pData->IncSpinBox.right = rect->right;
        pData->IncSpinBox.bottom = pData->IncSpinBox.top + bmp_updownarrow->bmHeight/2;

        pData->DecSpinBox.left = pData->IncSpinBox.left;
        pData->DecSpinBox.top = vcenter + 1;
        pData->DecSpinBox.right =  pData->IncSpinBox.right;
        pData->DecSpinBox.bottom = pData->DecSpinBox.top + bmp_updownarrow->bmHeight/2;
    }
}

static void OnSizeChanged (PCONTROL pCtrl, DWORD dwStyle, const RECT* rcClient)
{
    PMYCOMBOBOXDATA pData;
    int  tmpx, tmpy;
    int  height, width;
    RECT edit_box, rect;

    pData = (PMYCOMBOBOXDATA)pCtrl->dwAddData2;

    SetRect (&rect, 0, 0, RECTWP (rcClient), RECTHP (rcClient));
    width = rect.right - rect.left;

    switch (dwStyle & CBS_TYPEMASK) {
        case CBS_SIMPLE:
			
            height = GetSysFontHeight (SYSLOGFONT_CONTROL)
                    + MARGIN_EDIT_TOP + MARGIN_EDIT_BOTTOM + (WIDTH_EDIT_BORDER<<1);
            edit_box.left = rect.left;
            edit_box.top = rect.top;
            edit_box.right = rect.left + width;
            edit_box.bottom = rect.top + height;
            width -= GetMainWinMetrics (MWM_ICONX);
            pData->ListBoxRect.left = rect.left + GetMainWinMetrics (MWM_ICONX);
            pData->ListBoxRect.right = rect.right - MARGIN_EDIT_RIGHT;
            pData->ListBoxRect.top = rect.top + height + INTER_EDIT_LISTBOX;
            pData->ListBoxRect.bottom = pData->ListBoxRect.top + pData->list_height;
            break;

        case CBS_SPINLIST:
			
            GetSpinBox (pData, dwStyle, &rect, &edit_box);
            pData->spin_pace = 1;
            pData->fastspin_pace = 5;
            break;

        case CBS_AUTOSPIN:
            GetSpinBox (pData, dwStyle, &rect, &edit_box);
            pData->spin_min = 0;
            pData->spin_max = 100;
            pData->spin_pace = 1;
            pData->fastspin_pace = 5;
            break;

        default:
		
		 	pData->EditRect.left = rect.left;
            pData->EditRect.top = rect.top;
            pData->EditRect.right = rect.right - (int)(bmp_down.bmWidth>>2) - INTER_EDIT_BUTTON;
            pData->EditRect.bottom = rect.bottom;
			
            pData->DecSpinBox.left = rect.right - (int)(bmp_down.bmWidth>>2);
            pData->DecSpinBox.top = rect.top+(RECTH(rect) - bmp_down.bmHeight)/2;
            pData->DecSpinBox.right = rect.right;
            pData->DecSpinBox.bottom = pData->DecSpinBox.top + (int)(bmp_down.bmHeight);

            pData->IncSpinBox.left = pData->DecSpinBox.left;
            pData->IncSpinBox.top = pData->DecSpinBox.top;
            pData->IncSpinBox.right = pData->DecSpinBox.right;
            pData->IncSpinBox.bottom = pData->DecSpinBox.bottom;

            pData->ListBoxRect.left = rect.left;
            pData->ListBoxRect.right = rect.right;
            pData->ListBoxRect.top = rect.bottom;
            pData->ListBoxRect.bottom = rect.bottom + pData->list_height;
            break;
    }

    tmpx = pData->ListBoxRect.right;
    tmpy = pData->ListBoxRect.bottom;
    ClientToScreen ((HWND)pCtrl, &tmpx, &tmpy);
    if (tmpy > g_rcScr.bottom) {
        pData->ListBoxRect.left = rect.left;
        pData->ListBoxRect.right = rect.right;
        pData->ListBoxRect.top = rect.top - pData->list_height;
        pData->ListBoxRect.bottom = rect.top;
    }
	
    if (pData->ListBoxControl) {
        MoveWindow (pData->ListBoxControl, 
                        pData->ListBoxRect.left, pData->ListBoxRect.top,
                        width, pData->list_height, TRUE);
    }
#if 1	
	if (pData->hButton)
	{
		 MoveWindow (pData->hButton, pData->IncSpinBox.left, pData->IncSpinBox.top, 
                            RECTW(pData->IncSpinBox), RECTH(pData->IncSpinBox), TRUE);
	}
#endif	
}

static int MyComboBoxCtrlProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    PCONTROL      pCtrl   = Control (hwnd);
    PMYCOMBOBOXDATA pData   = (PMYCOMBOBOXDATA)pCtrl->dwAddData2;
    DWORD         dwStyle = pCtrl->dwStyle;
    
    switch (message) {
        case MSG_CREATE:
        {
            //const char* edit_text = GetWindowCaption (hwnd);
            DWORD list_style;

            if (!(pData = calloc (1, sizeof (MYCOMBOBOXDATA)))) {
                fprintf (stderr, "Create ComboBox control failure!\n");
                return -1;
            }
			
			SetWindowFont(hwnd, GetSystemFont(SYSLOGFONT_CONTROL));
            pData->wStateFlags = 0;
            pData->nListItems = 0;
            pCtrl->dwAddData2 = (DWORD)pData;
#if defined (_FLAT_WINDOW_STYLE) || defined (_PHONE_WINDOW_STYLE)
            list_style = WS_VSCROLL | LBS_NOTIFY | WS_THINFRAME;
#else
            list_style = WS_VSCROLL | LBS_NOTIFY | WS_THINFRAME | WS_BORDER;
#endif 
            if ((dwStyle & CBS_TYPEMASK) == CBS_DROPDOWNLIST)
            list_style |= LBS_MOUSEFOLLOW;

            if (dwStyle & CBS_SORT) list_style |= LBS_SORT;
			
            switch (dwStyle & CBS_TYPEMASK) {
            case CBS_SIMPLE:
                list_style |= WS_VISIBLE | WS_BORDER;
                pData->list_height = (pCtrl->dwAddData > 0) ? pCtrl->dwAddData : DEF_LISTHEIGHT;
                pData->SpinBmp = 0;
                break;

            case CBS_SPINLIST:
                pData->list_height = 0;
                pData->spin_pace = 1;
                pData->fastspin_pace = 5;
                break;

            case CBS_AUTOSPIN:
                pData->spin_min = 0;
                pData->spin_max = 100;
                pData->spin_pace = 1;
                pData->fastspin_pace = 5;
                pData->list_height = -1;
                break;

            default:
                pData->SpinBmp = ARROWBMP_DOWN;

                pData->list_height = (pCtrl->dwAddData > 0) ? pCtrl->dwAddData : DEF_LISTHEIGHT;
                break;
            }

            if (pData->list_height >= 0) {
                DWORD ex_style = WS_EX_NONE;
                if ((dwStyle & CBS_TYPEMASK) != CBS_SIMPLE) {
                    ex_style |= WS_EX_CTRLASMAINWIN;
                }
                
                pData->ListBoxControl = CreateWindowEx (MY_LISTBOX,
                            "my_listbox", list_style, ex_style,
                            IDC_CLISTBOX, 0, 0, 10, 10, hwnd, 0);
            }
            else {
                pData->ListBoxControl = 0;
            }
#if 1
			pData->hButton = CreateWindow (MY_BUTTONEX, 
                            "", 
                            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP, IDC_SPIN_BUTTON,
                            0, 0, COMBOX_BITMAP_SIZE, COMBOX_BITMAP_SIZE,
                            hwnd, (DWORD)&bmp_down);
#endif
            break;
        }

        case MSG_DESTROY:
            DestroyAllControls (hwnd);
            free (pData->str_format);
            free (pData);
        return 0;

        case MSG_SIZECHANGING:
        {
            const RECT* rcExpect = (const RECT*)wParam;
            RECT* rcResult = (RECT*)lParam;
            if ((dwStyle & CBS_TYPEMASK) == CBS_SIMPLE) {
                int height = GetSysFontHeight (SYSLOGFONT_CONTROL)
                        + MARGIN_EDIT_TOP + MARGIN_EDIT_BOTTOM + (WIDTH_EDIT_BORDER<<1);

                rcResult->left = rcExpect->left;
                rcResult->top = rcExpect->top;
                rcResult->right = rcExpect->right;
                rcResult->bottom = rcExpect->top + height;
                rcResult->bottom += pData->list_height + INTER_EDIT_LISTBOX*2;
                return 0;
            }
            break;
        }

        case MSG_SIZECHANGED:
            OnSizeChanged (pCtrl, dwStyle, (const RECT*)lParam);
            break;

        case MSG_CHILDHIDDEN: 
        { 
            int reason = wParam; 
            int x = LOSWORD(lParam); 
            int y = HISWORD(lParam); 

            if (reason == RCTM_CLICK)
                ScreenToClient (hwnd, &x, &y); 

            if (IS_SET (pData, CSF_HASDROPPED)) {
                CLEAR_STATE (pData, CSF_HASDROPPED);
                NotifyParent (hwnd, pCtrl->id, CBN_SELECTCANCEL);
                NotifyParent (hwnd, pCtrl->id, CBN_CLOSEUP);
            }

            if (((dwStyle & CBS_TYPEMASK) > CBS_AUTOSPIN) && PtInRect (&pData->IncSpinBox, x, y))
                SET_STATE (pData, CSF_HASDROPPED); 
            break; 
        } 
	  case MSG_SETFOCUS:
		  if (dwStyle & CBS_AUTOFOCUS || dwStyle & WS_TABSTOP)
			  SetFocusChild (pData->hButton);
			  //SetFocusChild (pData->EditControl);
		  if (dwStyle & CBS_NOTIFY)
			  NotifyParent (hwnd, pCtrl->id, CBN_SETFOCUS);
		  break;

	  case MSG_KILLFOCUS:
		  if (((dwStyle & CBS_TYPEMASK) == CBS_DROPDOWNLIST) && 
				  IS_SET (pData, CSF_HASDROPPED)) { 
			  ShowWindow (pData->ListBoxControl, SW_HIDE); 
			  CLEAR_STATE (pData, CSF_HASDROPPED); 
			  if (dwStyle & CBS_NOTIFY)
			  {
				  NotifyParent (hwnd, pCtrl->id, CBN_SELECTCANCEL);
				  NotifyParent (hwnd, pCtrl->id, CBN_CLOSEUP);
			  }
		  } 
		  if (dwStyle & CBS_NOTIFY)
			  NotifyParent (hwnd, pCtrl->id, CBN_KILLFOCUS);
		  break;

	  case MSG_LBUTTONDOWN: 
	  { 
		  int x = LOSWORD(lParam); 
		  int y = HISWORD(lParam); 
		  BOOL inc = FALSE;

		  if (PtInRect (&pData->IncSpinBox, x, y))
			  inc = TRUE;
		  else if (PtInRect (&pData->DecSpinBox, x, y))
			  inc = FALSE;
		  else
			  break;
		  
		  (void)inc;
		  switch (dwStyle & CBS_TYPEMASK) { 
			  case CBS_DROPDOWNLIST:
				  if (IS_SET (pData, CSF_HASDROPPED)) { 
					  CLEAR_STATE (pData, CSF_HASDROPPED); 
				  } 
				  else { 
					  ShowWindow (pData->ListBoxControl, SW_SHOW);
					  SET_STATE (pData, CSF_HASDROPPED); 
					  if (dwStyle & CBS_NOTIFY)
						  NotifyParent (hwnd, pCtrl->id, CBN_DROPDOWN);
				  } 
				  break;
		  } 

		  break; 
	  } 

		case MSG_LBUTTONUP: 
		{
		  switch (dwStyle & CBS_TYPEMASK) { 
		  case CBS_SPINLIST:
		  case CBS_AUTOSPIN:
			  /* cancel repeat message */
			  SetAutoRepeatMessage (0, 0, 0, 0);
			  ReleaseCapture ();
		  }
		  NotifyParent (hwnd, pCtrl->id, CBN_CLICKED);
		  break;
		}

        case MSG_COMMAND:
        {
            int len;
            char *buffer;
            int sel;
            int id = LOWORD(wParam);
            int code = HIWORD(wParam);

            switch (id) 
			{
            case IDC_CLISTBOX:
            {
                if ((dwStyle & CBS_TYPEMASK) == CBS_DROPDOWNLIST && 
                                (code == LBN_CLICKED))
                {
					if (code == LBN_KILLFOCUS)
					{
						ShowWindow (pData->ListBoxControl, SW_HIDE);
                        CLEAR_STATE (pData, CSF_HASDROPPED);
						if (dwStyle & CBS_NOTIFY) 
						{
	                        NotifyParent (hwnd, pCtrl->id, CBN_SELECTOK);
	                        NotifyParent (hwnd, pCtrl->id, CBN_CLOSEUP);
	                    }
					}
					else
					{
						if (IS_SET (pData, CSF_HASDROPPED)) 
						{						
	                        ShowWindow (pData->ListBoxControl, SW_HIDE);
	                        CLEAR_STATE (pData, CSF_HASDROPPED);
							if (dwStyle & CBS_NOTIFY) 
							{
		                        NotifyParent (hwnd, pCtrl->id, CBN_SELECTOK);
		                        NotifyParent (hwnd, pCtrl->id, CBN_CLOSEUP);
		                    }
	                    }
						else
						{	
							
	                        SET_STATE (pData, CSF_HASDROPPED);
							if (dwStyle & CBS_NOTIFY) 
							{
		                        NotifyParent (hwnd, pCtrl->id, CBN_SELECTOK);
		                        NotifyParent (hwnd, pCtrl->id, CBN_DROPDOWN);
		                    }
						}
					}
                    break;
                }

                if (code == LBN_DBLCLK && (dwStyle & CBS_NOTIFY)) 
				{
                    NotifyParent (hwnd, pCtrl->id, CBN_DBLCLK);
                    break;
                }

                if (code != LBN_SELCHANGE && dwStyle & CBS_READONLY)
                    break;

                if ((sel = SendMessage (pData->ListBoxControl, LB_GETCURSEL, 0, 0)) >= 0) 
				{
                    len = SendMessage (pData->ListBoxControl, LB_GETTEXTLEN, sel, 0);
                    buffer = ALLOCATE_LOCAL (len + 1);
                    SendMessage (pData->ListBoxControl, LB_GETTEXT, sel, (LPARAM)buffer);
                    SetWindowCaption(hwnd, buffer);
                    DEALLOCATE_LOCAL(buffer);
					InvalidateRect(hwnd,&pData->EditRect,TRUE);
                }

                if (dwStyle & CBS_NOTIFY)
                    NotifyParent (hwnd, pCtrl->id, CBN_SELCHANGE);
                break;
				}
            }
        }
        case MSG_GETDLGCODE:
            return DLGC_WANTARROWS | DLGC_WANTENTER;

        case MSG_FONTCHANGED:
            if (pData->ListBoxControl)
                SetWindowFont (pData->ListBoxControl, GetWindowFont (hwnd));
            return 0;
		case MSG_MOUSEMOVEIN:
		{

			BOOL in_out = (BOOL)wParam;
			if(in_out)
			    SetFocusChild (hwnd);
			else
				SetNullFocus (GetParent(hwnd));
			break;
		}
		case MSG_ENABLE:
            if (wParam && (pCtrl->dwStyle & WS_DISABLED))
                pCtrl->dwStyle &= ~WS_DISABLED;
            else if (!wParam && !(pCtrl->dwStyle & WS_DISABLED))
                pCtrl->dwStyle |= WS_DISABLED;
            else
                return 0;
			if (pCtrl->dwStyle & WS_DISABLED)
				EnableWindow(pData->hButton, FALSE);
			else	
				EnableWindow(pData->hButton, TRUE);
            InvalidateRect (hwnd, NULL, TRUE);

            return 0;
        case MSG_PAINT:
        {
            HDC hdc;
            hdc = BeginPaint (hwnd);
            ComboBoxDrawSpinButton (hwnd, hdc);    
            EndPaint (hwnd, hdc);
            return 0;
        }
    }
    return DefCBProc (hwnd, message, wParam, lParam);
}
//MSG_GETTEXT

static int DefCBProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    PMYCOMBOBOXDATA pData;
    int len, index;
    char *selection;
    int  rc;

    pData = (MYCOMBOBOXDATA *) Control(hWnd)->dwAddData2;
    switch (message) {
        /* messages specific to the listbox control */
        case CB_SETSTRCMPFUNC:
            return SendMessage (pData->ListBoxControl, LB_SETSTRCMPFUNC, wParam, lParam);

        case CB_ADDSTRING:
            rc = SendMessage (pData->ListBoxControl, LB_ADDSTRING, wParam, lParam);
            if (rc >= 0)
                pData->nListItems++; 
            return rc;

        case CB_DELETESTRING:
            rc = SendMessage (pData->ListBoxControl, LB_DELETESTRING, wParam, lParam);
            if (rc == 0) {
                int idx;
                pData->nListItems--;
                if (pData->nListItems <= wParam)
                    idx = pData->nListItems - 1;
                else
                    idx = wParam;

                SendMessage (hWnd, CB_SETCURSEL, idx, 0);
            }
            return rc;

        case CB_DIR:
            return SendMessage (pData->ListBoxControl, LB_DIR, wParam, lParam);
            
        case CB_FINDSTRING:
            return SendMessage (pData->ListBoxControl, LB_FINDSTRING, wParam, lParam);
            
        case CB_FINDSTRINGEXACT:
               return SendMessage (pData->ListBoxControl, LB_FINDSTRINGEXACT, wParam, lParam);
            
        case CB_GETCOUNT:
            return SendMessage (pData->ListBoxControl, LB_GETCOUNT, wParam, lParam);
            
        case CB_GETITEMADDDATA:
            return SendMessage (pData->ListBoxControl, LB_GETITEMADDDATA, wParam, lParam);
            
        case CB_SETITEMADDDATA:
            return SendMessage (pData->ListBoxControl, LB_SETITEMADDDATA, wParam, lParam);
            
        case CB_GETITEMHEIGHT:
            return SendMessage (pData->ListBoxControl, LB_GETITEMHEIGHT, wParam, lParam);
            
        case CB_SETITEMHEIGHT:
            return SendMessage (pData->ListBoxControl, LB_SETITEMHEIGHT, wParam, lParam);
            
        case CB_GETLBTEXT:
			{
            int ret = SendMessage (pData->ListBoxControl, LB_GETTEXT, wParam, lParam);			
			//printf("wParam = %d, lParam = %s ret = %d\r\n", wParam, (char *)lParam, ret);
            return ret;
        	}
        case CB_GETLBTEXTLEN:
            return SendMessage (pData->ListBoxControl, LB_GETTEXTLEN, wParam, lParam);
            
        case CB_INSERTSTRING:
            rc = SendMessage (pData->ListBoxControl, LB_INSERTSTRING, wParam, lParam);
            if (rc >= 0)
                pData->nListItems++; 
            return rc;
                
        case CB_GETCURSEL:
			//printf("current sel = %d\r\n", SendMessage (pData->ListBoxControl, LB_GETCURSEL, wParam, lParam));
            return SendMessage (pData->ListBoxControl, LB_GETCURSEL, wParam, lParam);

        case CB_SETCURSEL:
            if (SendMessage (pData->ListBoxControl, LB_SETCURSEL, wParam, lParam) == LB_ERR) {
                SetWindowCaption (hWnd, "");
                return CB_ERR;
            }

            index = SendMessage (pData->ListBoxControl, LB_GETCURSEL, wParam, lParam);
            len = SendMessage (pData->ListBoxControl, LB_GETTEXTLEN, index, 0);
            if (len <= 0) {
                SetWindowCaption (hWnd, "");
                return CB_ERR;
            }

            selection = FixStrAlloc (len + 1);  
            SendMessage (pData->ListBoxControl, LB_GETTEXT, (WPARAM)index, (LPARAM)selection);
            SetWindowCaption (hWnd, selection);
            FreeFixStr (selection);
            break;

        /* messages specific to the edit control */
        case CB_GETEDITSEL:
            return SendMessage (pData->EditControl, EM_GETSEL, wParam, lParam);

        case CB_LIMITTEXT:
            return SendMessage (pData->EditControl, EM_LIMITTEXT, wParam, lParam);

        case CB_SETEDITSEL:
            return SendMessage (pData->EditControl, EM_SETSEL, wParam, lParam);

        case MSG_GETTEXTLENGTH:
            return SendMessage (pData->EditControl, MSG_GETTEXTLENGTH, wParam, lParam);

        case MSG_GETTEXT:
			{
				char *buf = (char *)lParam;
				int ilen = wParam;

				//printf("wParam = %d lParam = %d",wParam,  lParam);
				if (ilen <= 0) return 0;
				const char *p = GetWindowCaption (hWnd);
				if (p == NULL)
				{
					buf[0] = 0;
					return 0;
				}
				
				int lenText = strlen(p);
				if (ilen < lenText)
				{
					memcpy(buf, p, ilen-1);
					buf[ilen -1] = 0;
					return ilen - 1;
				}

				memcpy(buf, p, lenText);
				return lenText;
			}

        case MSG_SETTEXT:
			//printf("set text %s\r\n", (const char* )lParam);
			return SetWindowCaption(hWnd, (const char* )lParam);
            //return SendMessage (pData->EditControl, MSG_SETTEXT, wParam, lParam);

        /* messages handled by the combobox itself */
        case CB_GETDROPPEDCONTROLRECT:
            if (pData->ListBoxControl) {
                CopyRect ((PRECT)lParam, &pData->ListBoxRect);
                return 0;
            }
            return CB_ERR;

        case CB_GETDROPPEDSTATE:
            return IS_SET (pData, CSF_CAPTUREACTIVE);

        case CB_RESETCONTENT:
            SendMessage (pData->ListBoxControl, LB_RESETCONTENT, 0, 0);
            SetWindowCaption (hWnd, "");
            return 0;

        case CB_SELECTSTRING:
            index = SendMessage (pData->ListBoxControl, LB_SELECTSTRING, wParam, lParam);
            if (index == LB_ERR)
               return CB_ERR;

            len = SendMessage (pData->ListBoxControl, LB_GETTEXTLEN, index, 0);
            if (len <= 0)
               return CB_ERR;

            selection = FixStrAlloc (len + 1);  
            rc = SendMessage (pData->ListBoxControl, LB_GETTEXT, (WPARAM)index, (LPARAM)selection);
            SetWindowCaption (hWnd, selection);
            FreeFixStr (selection);
            break;
    }

    return DefaultControlProc (hWnd, message, wParam, lParam);
}


