/*
** $Id: propsheet.c 9934 2008-03-28 09:35:14Z xwyan $
**
** propsheet.c: the Property Sheet (Tab) control.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming and others.
**
** NOTE: Originally by Wang Jian and Jiang Jun.
**
** Create date: 2001/11/20
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
#include "my_propsheet.h"


    
static int MyPropSheetCtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);


BOOL RegisterMyPropSheetControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = MY_PROPSHEET;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (0);
    WndClass.iBkColor    = GetWindowElementColor (BKC_CONTROL_DEF);
    WndClass.WinProc     = MyPropSheetCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

/**************************internal functions******************************/

#define _ICON_OFFSET 0
#define _GAP_ICON_TEXT 0
#define _MIN_TAB_WIDTH  10

#define LEFT_SCROLL_BTN_CLICKED  1
#define RIGHT_SCROLL_BTN_CLICKED 2
#define NO_SCROLL_BTN_CLICKED 3


static int set_page_title (MYPROPPAGE * page, char *ptr)
{
    int len = 0;
    
    if (page->title) {
        FreeFixStr (page->title);
    }
    if (ptr) {
        len = strlen (ptr);
        if (!(page->title = FixStrAlloc (len))) {
            return PS_ERR;
        }
        strcpy (page->title, ptr);
    } else {
        page->title = NULL;
    }
    return PS_OKAY;
}


static void show_hide_page (PMYPROPPAGE page, int show_cmd)
{
    HWND focus;
    
    ShowWindow (page->hwnd, show_cmd);

    focus = GetNextDlgTabItem (page->hwnd, (HWND)0, 0);
    if (SendMessage (page->hwnd, MSG_SHOWPAGE, focus, show_cmd) && show_cmd == SW_SHOW) {
        if (focus) {
            SetFocus (focus);
        }
    }
}


static PMYPROPPAGE get_left_page (PMYPROPSHEETDATA propsheet, PMYPROPPAGE page)
{
    PMYPROPPAGE left;
    left = propsheet->head;
    
    if (page == left) {
        return NULL;
    }
    while (left->next) {
        if (left->next == page) {
            return left;
        }
        left = left->next;
    }
    return NULL;
}

/* recalculate widths of the tabs after a page added or removed. */
static void recalc_tab_widths (HWND hwnd, PMYPROPSHEETDATA propsheet, DWORD dwStyle)
{
    PMYPROPPAGE page;

    if (propsheet->page_count == 0)
        return;

    if (dwStyle & PSS_COMPACTTAB) {
        
        HDC hdc;
        SIZE ext;
        int total_width = 0;

        hdc = GetClientDC (hwnd);
        page = propsheet->head;
        while (page) {
            page->width = 0;
            if (page->title) {
                GetTextExtent (hdc, page->title, -1, &ext);
                page->width = ext.cx + 8;
            }
            if (page->icon) {
                page->width += GetMainWinMetrics (MWM_ICONX);
            }
            if (page->width < _MIN_TAB_WIDTH) {
                page->width = _MIN_TAB_WIDTH;
            }
            total_width += page->width;
            page = page->next;
        }
        ReleaseDC (hdc);
        if (total_width > propsheet->head_rc.right) {
            int new_width = 0, step = 0;
            page = propsheet->head;
            step = (total_width - propsheet->head_rc.right)/propsheet->page_count;

            if (step == 0)
                step = 1;

            while (page) {
                page->width -= step;
                if (page->width < _MIN_TAB_WIDTH) {
                    page->width = _MIN_TAB_WIDTH;
                }
                if (!(page->next)
                    && (new_width + page->width > propsheet->head_rc.right)) {
                    page->width = propsheet->head_rc.right - new_width;
                }
                else
                    new_width += page->width;

                page = page->next;
            }

            total_width = new_width;
        }
        propsheet->head_width = total_width;
    
    } else {
    
        int width;

        width = propsheet->head_rc.right * 8 / (propsheet->page_count * 10);
        if (width < _MIN_TAB_WIDTH)
            width = propsheet->head_rc.right / propsheet->page_count;

        page = propsheet->head;
        while (page) {
            page->width = width;
            page = page->next;
        }
        propsheet->head_width = width * propsheet->page_count;
    }
}



/* notice!!!
   This is the core function of propsheet normal style !!
   and it is run very slow, because propsheet 
   is designed by single-link-list */
static void update_propsheet (PMYPROPSHEETDATA propsheet)
{
    PMYPROPPAGE temp, page;
    int tab_width = 0;
    
    if (propsheet->first_display_page == NULL) {
        propsheet->first_display_page = propsheet->head;
    }
    page = propsheet->head;
    /*test the tabs are overload ?*/
    propsheet->overload = FALSE;
    while (page) {
        tab_width += page->width;
        if (tab_width > (propsheet->head_rc.right - GetMainWinMetrics (MWM_SB_WIDTH) * 2)) {
            propsheet->overload = TRUE;
            propsheet->head_width = propsheet->head_rc.right - 
              GetMainWinMetrics (MWM_SB_WIDTH) * 2;
            break;
        }
        page = page->next;
    }
 
    if (propsheet->overload == FALSE) {
        propsheet->first_display_page = propsheet->head;
    }
   
    /* test if active is right of first_display_page */

    page = propsheet->active->next;

    while (page) {
        if (page == propsheet->first_display_page) {
            propsheet->first_display_page = propsheet->active;
            break;
        }
        page = page->next;
    }

    /* calc the first-display-page */
    page = propsheet->first_display_page;
    while (page) {
        temp = page;
        tab_width = 0;
        while (temp) {
            tab_width += temp->width;
            if (temp == propsheet->active) {
                if (tab_width <= propsheet->head_width) {
                    return;
                }
            }
            temp = temp->next;
        }
        propsheet->first_display_page = propsheet->first_display_page->next;
        page = page->next;
    }
}



static void scroll_tab_right (HWND hwnd, PCONTROL ctrl, PMYPROPSHEETDATA propsheet)
{
    if (propsheet->active->next == NULL) {
        return;
    }
    show_hide_page (propsheet->active, SW_HIDE);
    propsheet->active = propsheet->active->next;
    NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED); 
    update_propsheet (propsheet);
    show_hide_page (propsheet->active, SW_SHOW);
}





static void scroll_tab_left (HWND hwnd, PCONTROL ctrl, PMYPROPSHEETDATA propsheet)
{
    //PMYPROPPAGE page;
    //page = propsheet->first_display_page;

    if (propsheet->active == propsheet->head) {
        return;
    }
    show_hide_page (propsheet->active, SW_HIDE);
    propsheet->active = get_left_page (propsheet, propsheet->active); 
    if (propsheet->active == NULL) {
        propsheet->active = propsheet->head;
    }
    NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED); 
    update_propsheet (propsheet);
    show_hide_page (propsheet->active, SW_SHOW); 
}



/* destroy a page */
static void destroy_page (MYPROPPAGE * page)
{
    FreeFixStr (page->title);
    DestroyWindow (page->hwnd);
}




/* create a new page */
static BOOL create_page (HWND hwnd, DWORD style, PMYPROPSHEETDATA propsheet, 
                         MYPROPPAGE *page, PDLGTEMPLATE dlgtpl, WNDPROC proc)
{
    int i, len;
    PCTRLDATA ctrl;
    HWND hCtrl;
    RECT rcPage;

    if ((dlgtpl->controlnr > 0) && !dlgtpl->controls)
        return FALSE;
    
    GetClientRect (hwnd, &rcPage);
    
    /* create content page */ 
    if ((style & 0xf0L) == PSS_BOTTOM) {
        page->hwnd = CreateWindowEx (CTRL_STATIC, "", SS_LEFT, WS_EX_NONE, IDC_STATIC,
                                     2, 2, rcPage.right - 4, rcPage.bottom - 4, 
                                     hwnd, dlgtpl->dwAddData);
    } else {
        page->hwnd = CreateWindowEx (CTRL_STATIC, "", SS_LEFT, WS_EX_NONE, IDC_STATIC,
                                     1, propsheet->head_rc.bottom, rcPage.right - 4, 
                                     rcPage.bottom - 4 - propsheet->head_rc.bottom, 
                                     hwnd, dlgtpl->dwAddData);
    }
    if (page->hwnd == HWND_INVALID)
        return FALSE;

    len = strlen (dlgtpl->caption);
    page->title = FixStrAlloc (len);
    if (page->title)
        strcpy (page->title, dlgtpl->caption);

    page->proc = proc;
    page->icon = dlgtpl->hIcon;

    for (i = 0; i < dlgtpl->controlnr; i++) {
        ctrl = dlgtpl->controls + i;
        hCtrl = CreateWindowEx (ctrl->class_name, ctrl->caption,
                    ctrl->dwStyle | WS_CHILD, ctrl->dwExStyle,
                    ctrl->id, ctrl->x, ctrl->y, ctrl->w, ctrl->h,
                    page->hwnd, ctrl->dwAddData);
        
        if (hCtrl == HWND_INVALID) {
            DestroyWindow (page->hwnd);
            if (page->title) {
                FreeFixStr (page->title);
            }
            return FALSE;
        }
    }

    SetWindowCallbackProc (page->hwnd, page->proc);
    SendMessage (page->hwnd, MSG_INITPAGE, 0, dlgtpl->dwAddData);
    return TRUE;
}



/* Given a page, calc it's tab 
   width follow current DC */
static int tab_required_width (HDC hdc, PMYPROPSHEETDATA propsheet, PMYPROPPAGE page)
{
    int width = 0;
    SIZE ext;
    
    if (page->title) {
        GetTextExtent (hdc, page->title, -1, &ext);
        width = ext.cx + 8;
    }
    if (page->icon) {
        width += GetMainWinMetrics (MWM_ICONX);
    }
    if (width < _MIN_TAB_WIDTH) {
        width = _MIN_TAB_WIDTH;
    }
    if (width >= propsheet->head_rc.right - GetMainWinMetrics (MWM_SB_WIDTH) *2) {
        width = propsheet->head_rc.right - GetMainWinMetrics (MWM_SB_WIDTH) *2;
    }
    return width;
}


/* resize children after the sheet resized. */
static void resize_children (PMYPROPSHEETDATA propsheet, const RECT* rcPage, DWORD style)
{
    PMYPROPPAGE page;

    page = propsheet->head;
    while (page) {
        if ((style & 0xf0L) == PSS_BOTTOM) {
            MoveWindow (page->hwnd, 0, 0, 
                        rcPage->right, rcPage->bottom - propsheet->head_rc.bottom,
                        page == propsheet->active);
        } else {
            MoveWindow (page->hwnd, 0, propsheet->head_rc.bottom,
                        rcPage->right, rcPage->bottom,
                        page == propsheet->active);
        }
        page = page->next;
    }
}

/* return the page from index. */
static PMYPROPPAGE get_page (PMYPROPSHEETDATA propsheet, int index)
{
    int i = 0;
    PMYPROPPAGE page;

    page = propsheet->head;
    while (page) {
        if (i == index) {
            return page;
        }
        i++;
        page = page->next;
    }

    return NULL;
}

/* append a new page */
static int append_page (PMYPROPSHEETDATA propsheet, PMYPROPPAGE new_page)
{
    int i = 0;
    PMYPROPPAGE page;

    page = propsheet->head;
    while (page && page->next) {
        i++;
        page = page->next;
    }

    new_page->next = NULL;
    if (page) {
        i++;
        page->next = new_page;
    } else {
        propsheet->head = new_page;
    }

    propsheet->page_count++;
    return i;
}

/* remove a page */
static void remove_page (PMYPROPSHEETDATA propsheet, PMYPROPPAGE rm_page)
{
    PMYPROPPAGE page = propsheet->head, prev = NULL;

    while (page) {
        if (page == rm_page) {
            if (prev) {
                prev->next = rm_page->next;
            } else {
                propsheet->head = rm_page->next;
            }
            propsheet->page_count--; 
            break;
        }
        prev = page;
        page = page->next;
    }
}


static void draw_scroll_button (HWND hWnd, HDC hdc, RECT *title_rc, PMYPROPSHEETDATA propsheet, DWORD style)
{
    RECT btn_rect = {0, 0, 0, 0};
    int l, r, t, b;

    btn_rect.top = ((style & 0xf0L) == PSS_BOTTOM) ? title_rc->top + 1 : title_rc->top;
    btn_rect.right = GetMainWinMetrics (MWM_SB_WIDTH) - 4;
    btn_rect.bottom = ((style & 0xf0L) == PSS_BOTTOM) ? title_rc->bottom + 2: title_rc->bottom - 2;
        
    t = btn_rect.top + 5;
    r = btn_rect.right;
    b = ((style & 0xf0L) == PSS_BOTTOM) ? btn_rect.bottom - 7 : btn_rect.bottom - 4;

    SelectClipRect (hdc, &btn_rect);
    SetBrushColor (hdc, GetWindowBkColor (hWnd));
    FillBox (hdc, btn_rect.left, btn_rect.top, btn_rect.right + 3, btn_rect.bottom); 
    SetPenColor (hdc, GetWindowElementColorEx (hWnd, WEC_3DBOX_DARK));
    for ( ; t <= b; r--, t++, b--) {
        MoveTo (hdc, r, t);
        LineTo (hdc, r, b);
    }
    
    btn_rect.left = propsheet->head_rc.right - GetMainWinMetrics(MWM_SB_WIDTH);
    //btn_rect.top = ((style & 0xf0L) == PSS_BOTTOM) ? btn_rect.top - 1 : btn_rect.top;
    l = ((style & 0xf0L) == PSS_BOTTOM) ? btn_rect.left + 5 : btn_rect.left + 4;
    r = btn_rect.right = propsheet->head_rc.right;
    b = ((style & 0xf0L) == PSS_BOTTOM) ? btn_rect.bottom - 8 : btn_rect.bottom - 5;
    t = btn_rect.top + 6;
    SelectClipRect (hdc, &btn_rect);
    SetBrushColor (hdc, GetWindowBkColor (hWnd));
    FillBox (hdc, btn_rect.left, btn_rect.top, btn_rect.right, btn_rect.bottom);
    SetPenColor (hdc, GetWindowElementColorEx (hWnd, WEC_3DBOX_DARK));
    for ( ; t <= b; l++, t++, b--) {
        MoveTo (hdc, l, t);
        LineTo (hdc, l, b);
    }

}



/* re-draw */
static void draw_propsheet (HWND hwnd, HDC hdc, PCONTROL ctrl, PMYPROPSHEETDATA propsheet, PMYPROPPAGE page)
{
    int x, ty, by, text_extent;
    RECT title_rc = {0, 0, 1, 0};
    
    title_rc.bottom = propsheet->head_rc.bottom;
    if ((ctrl->dwStyle & 0xf0L) == PSS_BOTTOM) {
        title_rc.top = propsheet->head_rc.top;
    } 
/* Draw the content window */
 #if 0  
	 //RECT winRect;
	 //GetClientRect(hwnd, &winRect);
	 //SetBrushColor(hdc,0xFF0773BB);
	 //FillBox(hdc,winRect.left, winRect.top, RECTW(winRect), RECTH(winRect));

    if ((ctrl->dwStyle & 0xf0L) == PSS_BOTTOM) {
        //Draw3DThickFrameEx (hdc, hwnd, 0, 0, propsheet->head_rc.right, 
        //                    ctrl->bottom - ctrl->top - (propsheet->head_rc.bottom - propsheet->head_rc.top) + 2 ,
        //                    DF_3DBOX_NORMAL | DF_3DBOX_NOTFILL, 0);
		//SetPenColor(hdc,0xFF0773BB);
		//Rectangle(hdc,0, 0, propsheet->head_rc.right, 
        //                    ctrl->bottom - ctrl->top - (propsheet->head_rc.bottom - propsheet->head_rc.top) + 2);
    } else {
        //Draw3DThickFrameEx (hdc, hwnd, 0, propsheet->head_rc.bottom - 2, 
        //                    propsheet->head_rc.right, ctrl->bottom - ctrl->top,
        //                    DF_3DBOX_NORMAL | DF_3DBOX_NOTFILL, 0);
		
		//SetPenColor(hdc,0xFF0773BB);
		//Rectangle(hdc,0, propsheet->head_rc.bottom , 
        //                    propsheet->head_rc.right, ctrl->bottom - ctrl->top);
    }

#endif	
#if 0
	SetBrushColor (hdc, GetWindowElementColorEx (hwnd, BKC_DIALOG));
    
    if ((ctrl->dwStyle & 0xf0L) == PSS_BOTTOM) {
        FillBox (hdc, 0, propsheet->head_rc.top + 1, propsheet->head_rc.right, propsheet->head_rc.bottom );
    }else {
        FillBox (hdc, 0, 0, propsheet->head_rc.right, propsheet->head_rc.bottom - 2);
    }
#endif	
   
    /* Just for PSS_SCROLLABLE style 
       if title is overload (too many titles for the propsheet) 
       we should move-right the leftmot title some pixel 
       and we should draw a small icon (left arrow) for the sheet */
    if ((ctrl->dwStyle & 0x0fL) == PSS_SCROLLABLE) {
        title_rc.right = (propsheet->overload == TRUE) ? 
          title_rc.right + GetMainWinMetrics (MWM_SB_WIDTH) : title_rc.right;
    }
    
    while (page) {
        SIZE size;
        int eff_chars, eff_len;

        /* draw some box for title */
        if (title_rc.right == 1)
            title_rc.left = title_rc.right - 1;
        else
            title_rc.left = title_rc.right;
        title_rc.right = title_rc.left + page->width;
        SelectClipRect (hdc, &title_rc);
        x = title_rc.left + _ICON_OFFSET;
        ty = title_rc.top;

        if (page != propsheet->active) {
            /* bottom tab or top tab */
            if ((ctrl->dwStyle & 0xf0L) == PSS_BOTTOM) {
                ty -= 2;
                by = title_rc.bottom - 2;
            } else {
                ty += 2;
                by = title_rc.bottom;
            }
			SetBrushColor(hdc,0xFF1C577F);
            FillBox (hdc, title_rc.left+1, ty, title_rc.right - title_rc.left, by);

        } else {
            by = title_rc.bottom ;			
			SetBrushColor(hdc,0xFF0773BB);
            FillBox (hdc, title_rc.left+1, ty, title_rc.right - title_rc.left, by);
        }
        
        /* draw the ICON */
        ty += _ICON_OFFSET + 2;
        text_extent = RECTW (title_rc) - _ICON_OFFSET * 2;
        if (page->icon) {
            DrawIcon (hdc, x, ty, GetMainWinMetrics (MWM_ICONX),
                      GetMainWinMetrics (MWM_ICONY), page->icon);
            x += GetMainWinMetrics (MWM_ICONX);
            x += _GAP_ICON_TEXT;
            text_extent -= GetMainWinMetrics (MWM_ICONX) + _GAP_ICON_TEXT;
        }
        
        /* draw the TEXT */
/* #if defined(_FLAT_WINDOW_STYLE) && !defined(_GRAY_SCREEN) */

        //SetBkColor (hdc, GetWindowElementColorEx (hwnd, BKC_CONTROL_DEF));		
		SetBkMode(hdc, BM_TRANSPARENT);
		SetTextColor(hdc, PIXEL_lightwhite);
        text_extent -= 4;
        eff_len = GetTextExtentPoint (hdc, page->title, strlen(page->title), 
                      text_extent, &eff_chars, NULL, NULL, &size);
        TextOutLen (hdc, x + 2, ty, page->title, eff_len);
        page = page->next;
    }
    /* draw scroll button , just for PSS_SCROLLABLE style */
    if ((ctrl->dwStyle & 0x0fL) == PSS_SCROLLABLE && propsheet->overload == TRUE) {
        draw_scroll_button (hwnd, hdc, &title_rc, propsheet, ctrl->dwStyle);
    }
}



static int insert_new_page_normal_style (HWND hwnd, PMYPROPSHEETDATA propsheet, PMYPROPPAGE page, DWORD style)
{
    int require_width, index;
    HDC hdc = GetClientDC (hwnd);
    
    require_width = tab_required_width (hdc, propsheet, page);
    ReleaseDC (hdc);
    
    index = 0;
    
    if (propsheet->active) {
        show_hide_page (propsheet->active, SW_HIDE);
    }
    propsheet->active = page;
    
    page->width = require_width;
    index = append_page (propsheet, page);
    update_propsheet (propsheet);

    return index;
}


static int add_new_page_normal_style (HWND hwnd, PCONTROL ctrl, PMYPROPSHEETDATA propsheet, 
                                      DLGTEMPLATE *dlg, WNDPROC proc)
{
    PMYPROPPAGE page;
    int index;
    if (!(page = calloc (1, sizeof (MYPROPPAGE)))) {
        return PS_ERR;
    }
        
    if (!create_page (hwnd, ctrl->dwStyle, propsheet, page, dlg, proc)) {
        free (page);
        return PS_ERR;
    }
    
    index = insert_new_page_normal_style (hwnd, propsheet, 
                                          page, ctrl->dwStyle);
    
    NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
    show_hide_page (page, SW_SHOW);
    
    InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
    
    /* dump_propsheetdata (propsheet); */
    return index;
}


static int test_clicked_scroll_button (int x, int y, int btn_top, 
               int btn_bottom, PMYPROPSHEETDATA propsheet)
{
    RECT btn_rect = {0, 0, 0, 0};
    
    btn_rect.right = GetMainWinMetrics(MWM_SB_WIDTH);
    btn_rect.top = btn_top;
    btn_rect.bottom = btn_bottom;
    
    if (PtInRect (&btn_rect, x, y)) {
        return LEFT_SCROLL_BTN_CLICKED;
    } else {
        btn_rect.left = propsheet->head_rc.right - GetMainWinMetrics(MWM_SB_WIDTH);
        btn_rect.right = propsheet->head_rc.right;
        if (PtInRect (&btn_rect, x, y)) {
            return RIGHT_SCROLL_BTN_CLICKED;
        } else {
            return NO_SCROLL_BTN_CLICKED;
        }
    }

}


static void click_tab_bar (HWND hwnd, PCONTROL ctrl, PMYPROPSHEETDATA propsheet, LPARAM lParam)
{
    int x = LOSWORD(lParam), y = HISWORD(lParam);
    PMYPROPPAGE page;
    RECT title_rc = {0, 0, 0, 0};
        
    title_rc.bottom = propsheet->head_rc.bottom;
    title_rc.top = propsheet->head_rc.top;
    
    if ((ctrl->dwStyle & 0x0fL)!= PSS_SCROLLABLE) {

        page = propsheet->head;
        while (page) {
            title_rc.left = title_rc.right;
            title_rc.right = title_rc.left + page->width;
            if (PtInRect (&title_rc, x, y) && page != propsheet->active) {
                show_hide_page (propsheet->active, SW_HIDE);
                propsheet->active = page;
                NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
                show_hide_page (page, SW_SHOW);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
                return;
            }
            page = page->next;
        }

    } else {
        /* Normal style !!! */
        page = propsheet->first_display_page;
        title_rc.right = (propsheet->overload == TRUE) ? 
          title_rc.right + GetMainWinMetrics(MWM_SB_WIDTH) : title_rc.right;
        
        /* check if left-right-scroll-bar clicked 
           if they are clicked, just change the active title */
        if (propsheet->overload == TRUE) {
            int scroll_clicked;
            scroll_clicked = test_clicked_scroll_button (x, y, 
                                 title_rc.top, title_rc.bottom, propsheet);
            if (scroll_clicked == LEFT_SCROLL_BTN_CLICKED) {
                scroll_tab_left (hwnd, ctrl, propsheet);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
                return;
            } 
            if (scroll_clicked == RIGHT_SCROLL_BTN_CLICKED) {
                scroll_tab_right (hwnd, ctrl, propsheet);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
                return;
            }
        }
        while (page) {
            title_rc.left = title_rc.right;
            title_rc.right = title_rc.left + page->width;
            
            if (PtInRect (&title_rc, x, y) && page != propsheet->active) {
                show_hide_page (propsheet->active, SW_HIDE);
                propsheet->active = page;
                NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
                show_hide_page (page, SW_SHOW);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
                return;
            }
            page = page->next;
        }
    }
}


/* change page title */
static int set_page_title_normal_style (HDC hdc, PMYPROPSHEETDATA propsheet, MYPROPPAGE *page, char *title)
{
    int len = 0;
    
    if (page->title) {
        FreeFixStr (page->title);
    }
    if (title) {
        len = strlen (title);
        if (!(page->title = FixStrAlloc (len))) {
            return PS_ERR;
        }
        strcpy (page->title, title);
        page->width = tab_required_width (hdc, propsheet, page);
        update_propsheet (propsheet);
    } else {
        page->title = NULL;
    }
    return PS_OKAY;
}


static BOOL delete_page (HWND hwnd, PCONTROL ctrl, PMYPROPSHEETDATA propsheet, int index)
{
    PMYPROPPAGE page;
        
    if ((page = get_page (propsheet, index)) == NULL) {
        return PS_ERR;
    }
    /* first_display_page == active */
    if (propsheet->active == page && propsheet->first_display_page == page) {
        if (page->next) {
            propsheet->active = page->next;
            propsheet->first_display_page = page->next;
        } else {
            PMYPROPPAGE left = get_left_page (propsheet, propsheet->active);
            if (left != NULL) {
                propsheet->active = left;
                propsheet->first_display_page = left;
            } else {
                propsheet->active = propsheet->first_display_page = NULL;
            } 
        }
        /* propsheet->active = page->next; */
        /* propsheet->first_display_page = page->next; */
    } else {
        if ((propsheet->active == page) && (propsheet->first_display_page != page)) {
            if (propsheet->active->next == NULL) {
                propsheet->active = get_left_page (propsheet, page);
            } else {
                propsheet->active = propsheet->active->next;
            }
            goto del;
        }
        if ((propsheet->active != page) && (propsheet->first_display_page == page)) {
            if (propsheet->first_display_page->next == NULL) {
                propsheet->first_display_page = get_left_page (propsheet, page);
            } else {
                propsheet->first_display_page = propsheet->first_display_page->next;
            }
            goto del;
        }
    }
 del:
    remove_page (propsheet, page);
    destroy_page (page);
    free (page);
    
    if (propsheet->head) {
        update_propsheet (propsheet);
    }
    if (propsheet->active) {
        show_hide_page (propsheet->active, SW_SHOW);
    }
   
    InvalidateRect (hwnd, &propsheet->head_rc, TRUE);

    /* dump_propsheetdata (propsheet); */
    return PS_OKAY;
}




/* window procedure of the property sheet. */
static int MyPropSheetCtrlProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    PCONTROL      ctrl;
    PMYPROPSHEETDATA propsheet;

    ctrl = Control (hwnd); 
    propsheet = (MYPROPSHEETDATA *) ctrl->dwAddData2;
    
    switch (message) {
        
    case MSG_CREATE: {
        if (!(propsheet = calloc (1, sizeof (MYPROPSHEETDATA)))) {
            return -1;
        }
        ctrl->dwAddData2 = (DWORD)propsheet;
        break;
    }
        
    /* make the client size same as window size */
    case MSG_SIZECHANGED: {
        const RECT* rcWin = (RECT *)wParam;
        RECT* rcClient = (RECT *)lParam;
        
        /* cale the width of content page */
        *rcClient = *rcWin;
        propsheet->head_rc.right = RECTWP (rcClient);

        if ((ctrl->dwStyle & 0xf0L) == PSS_BOTTOM) {
            propsheet->head_rc.top = RECTHP (rcClient) - GetMainWinMetrics (MWM_ICONY) - 4 - _ICON_OFFSET * 2;
            propsheet->head_rc.bottom = RECTHP (rcClient);
        } else {
            propsheet->head_rc.bottom = GetMainWinMetrics (MWM_ICONY) + 4 + _ICON_OFFSET * 2;
        }

        if ((ctrl->dwStyle & 0x0fL)!= PSS_SCROLLABLE) {
            recalc_tab_widths (hwnd, propsheet, ctrl->dwStyle);
        } else {
            propsheet->head_width = propsheet->head_rc.right;
        }
        resize_children (propsheet, rcClient, ctrl->dwStyle);
        InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
        return 1;
    }

    case MSG_DESTROY: {
        PMYPROPPAGE page, temp;
        page = propsheet->head;
        while (page) {
            temp = page->next;
            destroy_page (page);
            free (page);
            page = temp;
        }
        free (propsheet);
        break;
    }

    case MSG_GETDLGCODE: {
        return DLGC_WANTTAB | DLGC_WANTARROWS;
    }
     
    case PSM_SHEETCMD: {
        int index = 0;
        PMYPROPPAGE page = propsheet->head;
        while (page) {
            if (SendMessage (page->hwnd, MSG_SHEETCMD, wParam, lParam))
                /* when encounter an error, return page index plus 1. */
                return index + 1;
            index++;
            page = page->next;
        }
        return 0; /* success */
    }

    case PSM_SETACTIVEINDEX: {
        PMYPROPPAGE page;

        if ((page = get_page (propsheet, wParam)) && page != propsheet->active) {
            show_hide_page (propsheet->active, SW_HIDE);
            propsheet->active = page;
            update_propsheet (propsheet);
            NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
            show_hide_page (page, SW_SHOW);
            
            InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
            return PS_OKAY;
        }
        return PS_ERR;
    }
        
    case PSM_GETACTIVEINDEX: {
        int index = 0;
        PMYPROPPAGE page = propsheet->head;
        while (page) {
            if (page == propsheet->active) {
                return index;
            }
            index ++;
            page = page->next;
        }
        return PS_ERR;
    }

    case PSM_GETACTIVEPAGE: {
        return (propsheet->active) ? 
          propsheet->active->hwnd : HWND_INVALID;
    }

    case PSM_GETPAGE: {
        int index = 0;
        PMYPROPPAGE page = propsheet->head;
        while (page) {
            if (index == wParam) {
                return page->hwnd;
            }
            index ++;
            page = page->next;
        }
        return HWND_INVALID;
    }
        
    case PSM_GETPAGEINDEX: {
        int index = 0;
        PMYPROPPAGE page = propsheet->head;
        while (page) {
            if (page->hwnd == wParam) {
                return index;
            }
            index ++;
            page = page->next;
        }
        return PS_ERR;
    }
        
    case PSM_GETPAGECOUNT: {
        return propsheet->page_count;
    }

    case PSM_GETTITLELENGTH: {
        int len = PS_ERR;
        PMYPROPPAGE page;
        
        if ((page = get_page (propsheet, wParam))) {
            len = strlen (page->title);
        }
        return len;
    }

    case PSM_GETTITLE: {
        char* buffer = (char*)lParam;
        PMYPROPPAGE page;
        
        if ((page = get_page (propsheet, wParam))) {
            strcpy (buffer, page->title);
            return PS_OKAY;
        }
        
        return PS_ERR;
    }

    case PSM_SETTITLE: {
        BOOL rc = PS_ERR;
        char* buffer = (char*)lParam;
        PMYPROPPAGE page;
        HDC hdc;
        if ((ctrl->dwStyle & 0x0fL) != PSS_SCROLLABLE) {
            if ((page = get_page (propsheet, wParam))) {
                rc = set_page_title (page, buffer);
                recalc_tab_widths (hwnd, propsheet, ctrl->dwStyle);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
            }
        } else {
            if ((page = get_page (propsheet, wParam))) {
                hdc = GetClientDC (hwnd);
                rc = set_page_title_normal_style (hdc, propsheet, page, buffer);
                ReleaseDC (hdc);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
            }
        }
        return rc;
    }

    case PSM_ADDPAGE: {
        if ((ctrl->dwStyle & 0x0fL)  != PSS_SCROLLABLE) {
            int index;
            PMYPROPPAGE page;
            if ((propsheet->head_rc.right / (propsheet->page_count + 1)) < _MIN_TAB_WIDTH) {
                return PS_ERR;
            }
            if (!(page = calloc (1, sizeof (MYPROPPAGE)))) {
                return PS_ERR;
            }
            if (!create_page (hwnd, ctrl->dwStyle, propsheet, page, (DLGTEMPLATE *)wParam, (WNDPROC)lParam)) {
                free (page);
                return PS_ERR;
            }
            
            index = append_page (propsheet, page);
            if (propsheet->active) {
                show_hide_page (propsheet->active, SW_HIDE);
            }
            propsheet->active = page;
            NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
            show_hide_page (page, SW_SHOW);
            recalc_tab_widths (hwnd, propsheet, ctrl->dwStyle);
            InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
            return index;
        } else {
            return add_new_page_normal_style (hwnd, ctrl, 
                       propsheet, (DLGTEMPLATE *)wParam, 
                      (WNDPROC)lParam);
        }
    }
        
    case PSM_REMOVEPAGE: {
        if ((ctrl->dwStyle & 0x0fL) != PSS_SCROLLABLE) {
            PMYPROPPAGE page;
            if ((page = get_page (propsheet, wParam))) {
                remove_page (propsheet, page);
                destroy_page (page);
                free (page);
                recalc_tab_widths (hwnd, propsheet, ctrl->dwStyle);
            } else {
                return PS_ERR;
            }
            if (propsheet->active == page) {
                propsheet->active = propsheet->head;
                NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
                if (propsheet->active) {
                    show_hide_page (propsheet->active, SW_SHOW);
                }
            }
            InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
            return PS_OKAY;
        } else {
            return delete_page (hwnd, ctrl, propsheet, wParam);
        }
    }
        
    case MSG_LBUTTONDOWN: {
        click_tab_bar (hwnd, ctrl, propsheet, lParam);
        /* dump_propsheetdata (propsheet); */
        break;
    }

    case MSG_KEYDOWN: {
        PMYPROPPAGE page, new_active = NULL;
        if (!(lParam & KS_CTRL) || (propsheet->head == NULL)) {
            break;
        }
        /* Key borad message for PSS_COMPACTTAB and PSS_SIMPLE */
        switch (LOWORD (wParam)) {
        case SCANCODE_CURSORBLOCKDOWN:
        case SCANCODE_CURSORBLOCKRIGHT:
            if ((ctrl->dwStyle & 0x0fL)!= PSS_SCROLLABLE) {
                new_active = propsheet->active->next;
                if (new_active == NULL)
                    new_active = propsheet->head;
                break;
            } else {
                scroll_tab_right (hwnd, ctrl, propsheet);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
                return 0;
            }
        case SCANCODE_CURSORBLOCKUP:
        case SCANCODE_CURSORBLOCKLEFT:
            if ((ctrl->dwStyle & 0x0fL)!= PSS_SCROLLABLE) {
                page = propsheet->head;
                if (propsheet->head == propsheet->active) {
                    while (page && page->next) {
                        page = page->next;
                    }
                } else {
                    while (page) {
                        if (page->next == propsheet->active)
                            break;
                        page = page->next;
                    }
                }
                new_active = page;
                break;
            } else {
                scroll_tab_left (hwnd, ctrl, propsheet);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
                return 0;
            }
        }  /* switch */
        if (new_active == NULL) {
            break;
        }
        show_hide_page (propsheet->active, SW_HIDE);
        propsheet->active = new_active;
        NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
        show_hide_page (new_active, SW_SHOW);
        InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
        return 0;
    }

    case MSG_NCPAINT:
    case MSG_ERASEBKGND:
        return 0;
        
    case MSG_PAINT: {
        HDC  hdc = BeginPaint (hwnd);
        PMYPROPPAGE page = ((ctrl->dwStyle & 0x0fL) == PSS_SCROLLABLE) ? 
          propsheet->first_display_page : propsheet->head;
        draw_propsheet (hwnd, hdc, ctrl, propsheet, page);
        EndPaint (hwnd, hdc);
        return 0;
    }
        
    default:
        break;
    }
    
    return DefaultControlProc (hwnd, message, wParam, lParam);
}


/* static void dump_propsheetdata (MYPROPSHEETDATA *p) */
/* { */
/*     fprintf (stdout, "----- P R O P S H E E T -------\n"); */
/*     fprintf (stdout, "head_rc            : left = %d, right = %d, top = %d bottom = %d\n", */
/*              p->head_rc.left, p->head_rc.right, p->head_rc.top, p->head_rc.bottom); */
/*     fprintf (stdout, "head_width         : %d\n", p->head_width); */
/*     fprintf (stdout, "page_count         : %d\n", p->page_count); */
       
/*     fprintf (stdout, "active             : %p\n", p->active); */
/*     fprintf (stdout, "first_display_page : %p\n", p->first_display_page); */
/*     fprintf (stdout, "head               : %p\n", p->head); */
/*     fprintf (stdout, "overload           : %s\n", */
/*              p->overload ? "TRUE" : "FALSE"); */
/* } */

/* static void dump_proppage (MYPROPPAGE *p) */
/* { */
/*     fprintf (stdout, "----- P R O P P A G E -------\n"); */
/*     fprintf (stdout, "char *title : %s\n", p->title); */
/*     fprintf (stdout, "HICON icon  : %p\n", p->icon); */
/*     fprintf (stdout, "int width   : %d\n", p->width); */
/*     fprintf (stdout, "HWND hwnd   : %p\n", p->hwnd); */
/*     fprintf (stdout, "WNDPROC proc: %p\n", p->proc); */
/*     fprintf (stdout, "next        : %p\n", p->next); */
/* } */




