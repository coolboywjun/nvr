/*
** $Id: listview.c 7367 2007-08-16 05:25:35Z xgwang $
**
** listview.c: the ListView control
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Create date: 2004/04/24
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __MINIGUI_LIB__
#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"
#else
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#endif

#ifdef _EXT_CTRL_LISTVIEW
#include <minigui/ext/listview.h>
#include <minigui/ctrl/text.h>
#include <minigui/ctrl/scrolled.h>
#include <minigui/ctrl/scrollview_impl.h>
#include <minigui/ext/listview_impl.h>


/* -------------------------------------------------------------------------- */
extern int scrollview_get_item_rect (HWND hWnd, HITEM hsvi, RECT *rcItem, BOOL bConv);
static int lvRemoveAllItem (HWND hWnd, PLVDATA plvdata);

/* -------------------------------------------------------------------------- */
/* treeview defines */

static const BITMAP *bmp_fold, *bmp_unfold, *bmp_checkmark;

#define TV_BORDER           2
#define TV_BOXHALFHEIGHT    4
#define TV_BOXGAP           3
#define TV_ICONGAP          3
#define TV_IDENT            5

/* -------------------------------------------------------------------------- */

#define LV_BE_VALID_COL(nPosition)    (nPosition < plvdata->nCols && nPosition >= 0)

#define LV_GET_HDR_RECT(p1, rect)      \
                  rect.left = p1->x + 1;   \
                  rect.right = p1->x + p1->width - 1;  \
                  rect.top = LV_HDR_TOP;     \
                  rect.bottom = plvdata->nHeadHeight;

/* Gets the text rect of a subitem */
#define LV_GET_SUBITEM_TEXTRECT(rect, textrect)  \
                  textrect.left = rect.left + 2;     \
                  textrect.top = rect.top + 2;       \
                  textrect.right = rect.right - 2;   \
                  textrect.bottom = rect.bottom - 2;

#define LV_H_OUTWND(plvdata, rcClient)    \
               ( plvdata->nHeadWidth - 2 > rcClient.right - rcClient.left)

#define LV_GET_ITEM(hsvi)  (PITEMDATA)scrollview_get_item_adddata(hsvi)

#define ISFOLD(pi)   (pi->dwFlags & LVIF_FOLD)
/* -------------------------------------------------------------------------- */
/* header operations */

GET_ENTRY_BY_INDEX(lvGetHdrByCol, LSTHDR, list)

#define GET_SUBITEM_X(nCols)   ((lvGetHdrByCol(&plvdata->hdrqueue, nCols))->x)
#define GET_SUBITEM_W(nCols)   ((lvGetHdrByCol(&plvdata->hdrqueue, nCols))->width)


#define LV_GET_SUBITEM_RECT(rcItem, nCols, rcSub) \
    rcSub.top = rcItem->top; \
    rcSub.bottom = rcItem->bottom; \
    rcSub.left = rcItem->left + GET_SUBITEM_X(nCols); \
    rcSub.right = rcSub.left + GET_SUBITEM_W(nCols);

/*
 * gets the previous nCols items width
 */
static int 
lvGetFrontSubItemsWidth (int nCols, PLVDATA plvdata)
{
    PLSTHDR p1;

    p1 = lvGetHdrByCol(&plvdata->hdrqueue, nCols-1);
    if (p1)
        return p1->x + p1->width;
    return 0;
}

static PITEMDATA lvGetItemByRow (PLVDATA plvdata, int nRows)
{
    HSVITEM hsvi;

    hsvi = scrollview_get_item_by_index (&plvdata->svdata, nRows);
    return LV_GET_ITEM(hsvi);
}

/* -------------------------------------------------------------------------- */

/* creates a new header */
static PLSTHDR 
lvHdrNew (PLVCOLUMN pcol, PLVDATA plvdata, int *col)
{
    PLSTHDR p1;
    int nCols = pcol->nCols;

    if (!LV_BE_VALID_COL(nCols)) {
       nCols = plvdata->nCols;
       *col = nCols;
    }

    if ( !(p1 = (PLSTHDR) malloc(sizeof(LSTHDR))) )
        return NULL;

    p1->sort = NOTSORTED;
    p1->pfnCmp = pcol->pfnCompare;
    p1->Image = pcol->image;
    p1->flags = pcol->colFlags;
    p1->checkFlags = pcol->checkFlags;//huangh add

    if (pcol->pszHeadText != NULL)
    {
        p1->pTitle = (char *) malloc (strlen (pcol->pszHeadText) + 1);
        strcpy (p1->pTitle, pcol->pszHeadText);
    }
    else
        p1->pTitle = NULL;
    
    p1->x = lvGetFrontSubItemsWidth (nCols, plvdata);
    if (pcol->width <= 0)
        p1->width = LV_COLW_DEF;
    else if (pcol->width < COLWIDTHMIN)
        p1->width = COLWIDTHMIN;
    else
        p1->width = pcol->width;

    list_add_by_index (&p1->list, &plvdata->hdrqueue, nCols);

    return p1;
}


//free a header
static void lvHdrDel (PLSTHDR pLstHdr)
{
    if (pLstHdr != NULL)
    {
        list_del (&pLstHdr->list);
        if (pLstHdr->pTitle != NULL)
            free (pLstHdr->pTitle);
        free (pLstHdr);
    }
}

static void lvHdrDestroy (PLVDATA plvdata)
{
    PLSTHDR hdr;

    while (!list_empty(&plvdata->hdrqueue)) {
        hdr = list_entry (plvdata->hdrqueue.next, LSTHDR, list);
        lvHdrDel (hdr);
    }
}

static BOOL
lvBeInHeadBorder (int mouseX, int mouseY, PLSTHDR p1, PLVDATA plvdata)
{
    if ( (mouseX >= (p1->x + p1->width - 1))
         && (mouseX <= (p1->x + p1->width))
         && (mouseY >= 0) && (mouseY <= plvdata->nHeadHeight) )
        return TRUE;
    return FALSE;
}

static int 
lvInWhichHeadBorder (int mouseX, int mouseY, PLSTHDR *pRet, PLVDATA plvdata)
{
    int nPos = 0;
    PLSTHDR p1 = NULL;
    list_t *me;
    BOOL bGet = FALSE;

    scrolled_window_to_visible (plvscr, &mouseX, NULL);
    scrolled_visible_to_content (plvscr, &mouseX, NULL);

    list_for_each (me, &plvdata->hdrqueue) {
        p1 = list_entry (me, LSTHDR, list);
        if (lvBeInHeadBorder(mouseX, mouseY, p1, plvdata)) {
            bGet = TRUE;
            break;
        }
        nPos ++;
    }
    
    if (pRet)
        *pRet = bGet ? p1 : NULL;

    return bGet ? nPos : -1;
}

static int GetColumnPara(int mouseX, int mouseY, PLSTHDR * pRet, PLVDATA plvdata)
{
    PLSTHDR p1 = NULL;
    list_t *me;
    RECT rect;
    int nPosition = 0;

    list_for_each (me, &plvdata->hdrqueue) {
        p1 = list_entry (me, LSTHDR, list);
        LV_GET_HDR_RECT(p1, rect);
    	if (mouseX >= rect.left && mouseX < rect.right)
            break;
        nPosition++;
    }
    
    //not in head
    if (!p1 || (nPosition >= plvdata->nCols) || (nPosition < 0)) {
        if (pRet)
            *pRet = NULL;
        return -1;
    }
    
    if (pRet)
        *pRet = p1;
	
    return nPosition;
}

static int 
isInListViewHead (int mouseX, int mouseY, PLSTHDR * pRet, PLVDATA plvdata)
{
    int nPosition = 0;
    PLSTHDR p1 = NULL;
    list_t *me;
    RECT rect;

    scrolled_window_to_visible (plvscr, &mouseX, NULL);
    scrolled_visible_to_content (plvscr, &mouseX, NULL);

    list_for_each (me, &plvdata->hdrqueue) {
        p1 = list_entry (me, LSTHDR, list);
        LV_GET_HDR_RECT(p1, rect);
        if (PtInRect (&rect, mouseX, mouseY))
            break;
        
        nPosition++;
    }

    //not in head
    if (!p1 || (nPosition >= plvdata->nCols) || (nPosition < 0)) {
        if (pRet)
            *pRet = NULL;
        return -1;
    }

    //in head
    if (pRet)
        *pRet = p1;
    return nPosition;
}

static int isInLVItem (int mouseX, int mouseY, PITEMDATA *pRet, PLVDATA plvdata, POINT *pt)
{
    int ret;
    HSVITEM hsvi = 0;

    scrolled_window_to_visible (plvscr, &mouseX, &mouseY);
    scrolled_visible_to_content (plvscr, &mouseX, &mouseY);

    ret = scrollview_is_in_item (&plvdata->svdata, mouseY, &hsvi, NULL);
    if (ret < 0)
        return ret;

    if (pRet && hsvi) {
        *pRet = LV_GET_ITEM(hsvi);
    }
    
    if (pt) {
        pt->x = mouseX;
        pt->y = mouseY;
    }

    return ret;
}

/* -------------------------------------------------------------------------- */

GET_ENTRY_BY_INDEX(lvGetSubItemByCol, SUBITEMDATA, list)

/* ========================================================================= 
 * Drawing section of the listview control.
   ========================================================================= */

static void sDrawText (HDC hdc, int x, int y, int width, int height, 
                const char *pszText, UINT format)
{
    RECT rect;
    //SIZE size;

    if (pszText != NULL)
    {
        SetRect (&rect, x+2, y+2, x+width, y+height);
        DrawText (hdc, pszText, -1, &rect, format);
    }
}

static void lvDrawFold (HDC hdc, RECT *rc_text, PITEMDATA pItem)
{
    RECT rcFold;
    int bmpx, bmpy;
    int text_format;

    rcFold = *rc_text;
    rcFold.right = rcFold.left + RECTH(rcFold);
    text_format = DT_SINGLELINE | DT_LEFT | DT_VCENTER;

    if (ISFOLD(pItem)) {
        if (bmp_fold) {
            bmpx = rcFold.left;
            bmpy = rcFold.top + (RECTH(rcFold)-bmp_fold->bmHeight) / 2;
            FillBoxWithBitmap (hdc, bmpx, bmpy, 0, 0, bmp_fold);
        }
        else {
            DrawText (hdc, "+", -1, &rcFold, text_format);
        }
    }
    else {
        if (bmp_unfold) {
            bmpx = rcFold.left;
            bmpy = rcFold.top + (RECTH(rcFold)-bmp_unfold->bmHeight) / 2;
            FillBoxWithBitmap (hdc, bmpx, bmpy, 0, 0, bmp_unfold);
        }
        else {
            DrawText (hdc, "--", -1, &rcFold, text_format);
        }
    }
}

#define LSTV_WIDTH_CHECKMARK     11
#define LSTV_HEIGHT_CHECKMARK    11

static void lvDrawSubItem (HWND hWnd, PLVDATA plvdata, HDC hdc, PITEMDATA pItem, RECT *rcItem, int nCols)
{
    RECT rect, rect_text;
    PSUBITEMDATA psub;
    PLSTHDR ph;
    UINT text_format;
    int nItemHeight;

	int offset = 0;
	DWORD dwStyle = GetWindowStyle(hWnd);
   // printf("[DEBUG]~~~~~~~~~~~~dwStyle: 0x%x\r\n",dwStyle);    
    if ( !(psub = lvGetSubItemByCol(&pItem->subqueue, nCols)) )
        return;

    ph = lvGetHdrByCol (&plvdata->hdrqueue, nCols);
    LV_GET_SUBITEM_RECT(rcItem, nCols, rect);

    if (!scrollview_is_item_hilight(hWnd, (HSVITEM)pItem->addData)) {
        SetBkMode (hdc, BM_TRANSPARENT);
        SetBkColor (hdc, GetWindowBkColor (hWnd));
        //SetTextColor (hdc, psub->nTextColor);        
        SetTextColor (hdc, PIXEL_lightwhite);
    }
    else {
        SetBkColor (hdc, plvdata->bkc_selected);
        SetBrushColor (hdc, plvdata->bkc_selected);
        SetTextColor (hdc, PIXEL_lightwhite);
        FillBox (hdc, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
    }

    LV_GET_SUBITEM_TEXTRECT(rect, rect_text);

    if (nCols == 0) {
        rect_text.left += pItem->depth * INDENT;
        if (pItem->child) {
            lvDrawFold (hdc, &rect_text, pItem);
            rect_text.left += INDENT;
        }
    }

    nItemHeight = scrollview_get_item_height((HSVITEM)pItem->addData);
    if (psub->Image) {
       int width, height;
       if (psub->dwFlags & LVIF_BITMAP) {
           PBITMAP bmp = (PBITMAP)psub->Image;
           height = (bmp->bmHeight < nItemHeight)? bmp->bmHeight : nItemHeight;
           rect.top += (nItemHeight - height) / 2;
           rect.left += 2;
           FillBoxWithBitmap (hdc, rect.left, rect.top, 0, height, bmp);
           rect_text.left = rect.left + bmp->bmWidth + 2;
       }
       else if (psub->dwFlags & LVIF_ICON) {
           GetIconSize ((HICON)psub->Image, &width, &height);
           height = (height < nItemHeight) ? height : nItemHeight;
           rect.top += (nItemHeight - height) / 2;
           rect.left += 2;
           DrawIcon (hdc, rect.left, rect.top, 0, height,
                           (HICON)psub->Image);
           rect_text.left = rect.left + width + 2;
       }
    }
	//printf("[DEBUG]~~~~~~~~psub->dwFlags: 0x%x\n",psub->dwFlags);
	if((dwStyle & LVS_CHECKBOX)&&(psub->dwFlags & LVIF_CHECKMASK))
	{
		if(psub->dwFlags & LVIF_CHECKED)
			offset = 0;
		else if(psub->dwFlags & LVIF_PARTCHECKED)
			offset = LSTV_WIDTH_CHECKMARK << 1;
		else if(psub->dwFlags & LVIF_BLANK)
			offset = LSTV_WIDTH_CHECKMARK;
			
		int height;

		height = (LSTV_HEIGHT_CHECKMARK < nItemHeight) ? LSTV_HEIGHT_CHECKMARK : nItemHeight;
		rect.top += (nItemHeight - height)/2;
		rect.left += 2;
        FillBoxWithBitmapPart (hdc, 
            rect.left, rect.top,
            LSTV_WIDTH_CHECKMARK, LSTV_HEIGHT_CHECKMARK,
            0, 0,
            bmp_checkmark,
            offset, 0);
        rect_text.left = rect.left + LSTV_WIDTH_CHECKMARK + 10;
	}

    if (!psub->pszInfo)
        return;

    if (ph->flags & LVCF_RIGHTALIGN)
        text_format = DT_SINGLELINE | DT_RIGHT | DT_VCENTER;
    else if (ph->flags & LVCF_CENTERALIGN)
        text_format = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
    else
        text_format = DT_SINGLELINE | DT_LEFT | DT_VCENTER;
    DrawText (hdc, psub->pszInfo, -1, &rect_text, text_format);
}

static void lvDrawItem (HWND hWnd, HSVITEM hsvi, HDC hdc, RECT *rcDraw)
{
    int i = 0;
    PLVDATA plvdata;
    PITEMDATA pi = NULL;
    list_t *me;

    plvdata = (PLVDATA) GetWindowAdditionalData2 (hWnd);
    pi = LV_GET_ITEM(hsvi);

    list_for_each (me, &pi->subqueue) {
        lvDrawSubItem (hWnd, plvdata, hdc, pi, rcDraw, i);
        i++;
    }
}

static void lvDrawHeaderCustom (HWND hWnd, HDC hdc)
{
    PLVDATA plvdata;
    PLSTHDR p1 = NULL;
    RECT rcClient, rcHdr, rcItem, rect;
    list_t *me;
    int i = 0;
    int offset = 0;
    int nItemHeight = 0;

	DWORD dwStyle = GetWindowStyle(hWnd);

    plvdata = (PLVDATA) GetWindowAdditionalData2 (hWnd);
    GetClientRect (hWnd, &rcClient);
    rcHdr = rcClient;
    rcHdr.top = LV_HDR_TOP;
    rcHdr.bottom = LV_HDR_TOP + LV_HDR_HEIGHT;
    rcItem = rcHdr;
    nItemHeight = plvdata->nHeadHeight;

    /* drawing header background */
    plvdata->pfn_draw_hdr_bk (hWnd, 0, hdc, &rcHdr);

    /* drawing header items */
    list_for_each (me, &plvdata->hdrqueue) {
        p1 = list_entry (me, LSTHDR, list);

		if((dwStyle & LVS_CHECKBOX)&&(p1->checkFlags & LVI_HEADER_CHECKMASK))
				offset = LSTV_WIDTH_CHECKMARK + 10;
		else
				offset = 0;
        //printf("[debug]~~~~~~~~%s offset: %d, nItemHeight: %d\n",__func__,offset, nItemHeight);
        rcItem.left = p1->x - plvscr->nContX-1 + offset;
        rcItem.right = p1->x - plvscr->nContX + p1->width;
        plvdata->pfn_draw_hdr_item (hWnd, i, hdc, &rcItem);
        
		if((dwStyle & LVS_CHECKBOX)&&(p1->checkFlags & LVI_HEADER_CHECKMASK))
		{

			if(p1->checkFlags  & LVI_HEADER_CHECK)
				offset = 0;
			else if(p1->checkFlags  & LVI_HEADER_PARTCHECK)
				offset = LSTV_WIDTH_CHECKMARK << 1;
			else if(p1->checkFlags  & LVI_HEADER_BLANK)
				offset = LSTV_WIDTH_CHECKMARK;

			int height;

			height = (LSTV_HEIGHT_CHECKMARK < nItemHeight) ? LSTV_HEIGHT_CHECKMARK : nItemHeight;
			rect.top = (nItemHeight - height)/2;
			rect.left = p1->x - plvscr->nContX-1+4;
			SetBkMode (hdc, BM_TRANSPARENT);
			FillBoxWithBitmapPart (hdc, 
			    rect.left, rect.top,
			    LSTV_WIDTH_CHECKMARK, LSTV_HEIGHT_CHECKMARK,
			    0, 0,
			    bmp_checkmark,
			    offset, 0);
		}

        i ++;
    }
}

//Draws listview header
static void lvDrawHeader (HWND hWnd, HDC hdc)
{
    PLSTHDR p1 = NULL;
    PLVDATA plvdata;
    RECT rcClient;
    DWORD flags = DF_3DBOX_NORMAL;
    UINT format;
    list_t *me;
    int item_w = 0;

    plvdata = (PLVDATA) GetWindowAdditionalData2 (hWnd);
    GetClientRect (hWnd, &rcClient);

    if (LVSTATUS(hWnd) & LVST_HEADCLICK && LVSTATUS(hWnd) & LVST_INHEAD)
        flags = DF_3DBOX_PRESSED;

    SetBkMode (hdc, BM_TRANSPARENT);
    SetBkColor (hdc, 0xff979797);//GetWindowBkColor (hWnd));
    SetTextColor (hdc, GetWindowElementColorEx(hWnd, FGC_CONTROL_NORMAL));

    list_for_each (me, &plvdata->hdrqueue) {
        p1 = list_entry (me, LSTHDR, list);
		
        DrawFlatControlFrameEx (hdc, hWnd, p1->x - plvscr->nContX-1,
                            LV_HDR_TOP - plvscr->nContY-1,
                            p1->x - plvscr->nContX + p1->width,
                            LV_HDR_TOP + LV_HDR_HEIGHT, 0, 
                            flags | DF_3DBOX_FILL, 0xff979797);
        item_w += p1->width;

        if (p1->flags & LVHF_CENTERALIGN)
            format = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
        else if (p1->flags & LVHF_RIGHTALIGN)
            format = DT_SINGLELINE | DT_RIGHT | DT_VCENTER;
        else
            format = DT_SINGLELINE | DT_LEFT | DT_VCENTER;

        sDrawText (hdc, p1->x - plvscr->nContX + 2, LV_HDR_TOP,
                 p1->width - 4, LV_HDR_HEIGHT, p1->pTitle, format);
    }

    //draws the right most unused header
    DrawFlatControlFrameEx (hdc, hWnd, item_w - plvscr->nContX + 1,
            LV_HDR_TOP-1,
            rcClient.right+2,
            LV_HDR_TOP + LV_HDR_HEIGHT, 0, 
            flags | DF_3DBOX_FILL, GetWindowBkColor (hWnd));
}

static inline void lv_draw_header (HWND hWnd, HDC hdc)
{
    PLVDATA plvdata;
    plvdata = (PLVDATA) GetWindowAdditionalData2 (hWnd);

    if (plvdata->pfn_draw_hdr_bk && plvdata->pfn_draw_hdr_item) {
        lvDrawHeaderCustom (hWnd, hdc);
    }
    else {
        lvDrawHeader (hWnd, hdc);
    }
}

static inline void lvOnDraw (HWND hWnd, HDC hdc)
{
    PLVDATA plvdata;
    plvdata = (PLVDATA) GetWindowAdditionalData2 (hWnd);

    if (plvdata->nHeadHeight != 0) {
        lv_draw_header (hWnd, hdc);
    }
}

/* -------------------------------------------------------------------------- */

static int lvGetItemIndent (PLVDATA plvdata, PITEMDATA pitem)
{
    PITEMDATA pp;
    int indent = -INDENT;

    if (!pitem)
        return -1;
    
    pp = pitem->parent;
    while (pp) {
        indent += INDENT;
        pp = pp->parent;
    }

    return indent;
}

static int lvFoldItem (HWND hWnd, PLVDATA plvdata, PITEMDATA pitem, BOOL bFold)
{
    int i;
    HSVITEM hsvi;
    int newh;

    if (!pitem)
        return -1;

    hsvi = (HSVITEM)pitem->addData;

    if (bFold && !(ISFOLD(pitem))) {
        pitem->dwFlags |= LVIF_FOLD;
    }
    else if (!bFold && (ISFOLD(pitem))) {
        pitem->dwFlags &= ~LVIF_FOLD;
    }
    else
        return -1;

    scrollview_freeze (hWnd, &plvdata->svdata, TRUE);
    for (i = 0; i < pitem->child_nr; i++) {
        PITEMDATA pi;

        hsvi = scrollview_get_next_item (&plvdata->svdata, hsvi);
        pi = LV_GET_ITEM(hsvi);

        newh = bFold ? 0 : pi->itemh;
        scrollview_set_item_height (hWnd, hsvi, newh);

        if (!bFold && ISFOLD(pi) && pi->child_nr > 0) {
            int j;
            i += pi->child_nr;
            for (j=0; j<pi->child_nr; j++) {
                hsvi = scrollview_get_next_item (&plvdata->svdata, hsvi);
            }
        }
    }
    scrollview_freeze (hWnd, &plvdata->svdata, FALSE);

    return 0;
}

/* ---------------------------- subitem operations -------------------------- */

static PSUBITEMDATA subitemNew (const char *pszInfoText)
{
    PSUBITEMDATA p1;
    
    p1 = (PSUBITEMDATA) malloc (sizeof (SUBITEMDATA));
    
    if (pszInfoText)
    {
        p1->pszInfo = (char *) malloc (strlen (pszInfoText) + 1);
        strcpy (p1->pszInfo, pszInfoText);
    }
    else
        p1->pszInfo = NULL;

    p1->nTextColor = PIXEL_lightwhite;
    p1->Image = 0;
    p1->dwFlags = 0;

    return p1;
}

static void subitemFree (PSUBITEMDATA pSubItemData)
{
    if (pSubItemData)
    {
        if (pSubItemData->pszInfo)
            free (pSubItemData->pszInfo);
        free (pSubItemData);
    }
}

static void
lvAddSubitem (PLVDATA plvdata, int nCols)
{
    PSUBITEMDATA pnew;
    PITEMDATA p1;
    HSVITEM hsvi;

    hsvi = scrollview_get_next_item (&plvdata->svdata, 0);
    p1 = LV_GET_ITEM(hsvi);
    while (p1)
    {
        pnew = subitemNew (NULL);
        list_add_by_index (&pnew->list, &p1->subqueue, nCols);
        hsvi = scrollview_get_next_item(&plvdata->svdata, (HSVITEM)p1->addData);
        p1 = LV_GET_ITEM(hsvi);
    }
}

static void
lvDelSubitem (PLVDATA plvdata, int nCols)
{
    HSVITEM hsvi;
    PITEMDATA p1;
    PSUBITEMDATA pdel;

    hsvi = scrollview_get_next_item (&plvdata->svdata, 0);
    p1 = LV_GET_ITEM(hsvi);
    while (p1)
    {
        if ( !(pdel  = lvGetSubItemByCol (&p1->subqueue, nCols)) )
            break;
        list_del (&pdel->list);
        subitemFree(pdel);
        hsvi = scrollview_get_next_item(&plvdata->svdata, (HSVITEM)p1->addData);
        p1 = LV_GET_ITEM(hsvi);
    }
}

/* offset the tail subitems from nCols */
static int sAddOffsetToTailSubItem (int nCols, int offset, PLVDATA plvdata)
{
    PLSTHDR p;
    list_t *me, *prev;

    if ( !(p = lvGetHdrByCol (&plvdata->hdrqueue, nCols)) )
        return -1;

    prev = p->list.prev;

    list_for_each_ex (me, &plvdata->hdrqueue, prev) {
        p = list_entry (me, LSTHDR, list);
        p->x += offset;
    }

    return 0;
}

static int lvSetContWidth (HWND hWnd, PLVDATA plvdata, int new_w)
{
    int old_w, width;

    old_w = plvscr->nContWidth;
    width = scrolled_set_cont_width (hWnd, plvscr, new_w);
    if (width != old_w)
        InvalidateRect (hWnd, NULL, TRUE);

    return width;
}

static BOOL 
lvAddColumnToList (HWND hWnd, PLVDATA plvdata, PLVCOLUMN pcol)
{
    PLSTHDR p1 = NULL;
    int nCols = pcol->nCols;

    if ( !(p1 = lvHdrNew (pcol, plvdata, &nCols)) )
        return FALSE;

    lvAddSubitem (plvdata, nCols);
    sAddOffsetToTailSubItem (nCols+1, p1->width, plvdata);
    plvdata->nHeadWidth += p1->width;
    lvSetContWidth (hWnd, plvdata, plvdata->nHeadWidth);
    plvdata->nCols ++;

    return TRUE;
}

//FIXME
static int sModifyHeadText (int nCols, const char *pszHeadText, PLVDATA plvdata)
{
    PLSTHDR p1 = NULL;

    p1 = lvGetHdrByCol(&plvdata->hdrqueue, nCols);
    if (!p1 || !pszHeadText)
        return -1;
  
    if (p1->pTitle != NULL)
        free (p1->pTitle);
  
    p1->pTitle = (char *) malloc (strlen (pszHeadText) + 1);
    strcpy (p1->pTitle, pszHeadText);
  
    return 0;
}

static PSUBITEMDATA
lvGetSubItemEx (PITEMDATA pitem, int nSubItem, PLVDATA plvdata)
{
    if (!pitem)
        return NULL;
    return lvGetSubItemByCol(&pitem->subqueue, nSubItem);
}

/* Fills the content of a subitem */
static int 
lvFillSubItem (PITEMDATA pi, int subItem, const char *pszText, PLVDATA plvdata)
{
    PSUBITEMDATA p1;

    if (!pi)
        return -1;

    if ( !(p1 = lvGetSubItemEx (pi, subItem, plvdata)) )
        return -1;

    if (pszText == NULL)
        return -1;

    if (p1->pszInfo != NULL)
        free (p1->pszInfo);
    p1->pszInfo = (char *) malloc (strlen (pszText) + 1);
    strcpy (p1->pszInfo, pszText);

    return 0;
}

/* Sets the text color of the subitem */
static int 
lvSetSubItemTextColor (PITEMDATA pitem, int subItem, int color, PLVDATA plvdata)
{
    PSUBITEMDATA p1;

    if (!pitem)
        return -1;

    if ( !(p1 = lvGetSubItemEx (pitem, subItem, plvdata)) )
        return -1;

    p1->nTextColor = color;

    return 0;
}

static int 
lvSetSubItem (PLVDATA plvdata, PITEMDATA pi, PLVSUBITEM pinfo)
{
    PSUBITEMDATA p1;

    if (!pinfo)
        return -1;

    if (!pi)
        pi = lvGetItemByRow (plvdata, pinfo->nItem);
    if (!pi)
        return -1;

    if ( !(p1 = lvGetSubItemEx (pi, pinfo->subItem, plvdata)) )
        return -1;

    if (p1->pszInfo != NULL)
        free (p1->pszInfo);

    if (pinfo->pszText) {
        p1->pszInfo = (char *) malloc (strlen (pinfo->pszText) + 1);
        strcpy (p1->pszInfo, pinfo->pszText);
    }
    else {
        p1->pszInfo = malloc (1);
        p1->pszInfo [0] = 0;
    }

    p1->nTextColor = pinfo->nTextColor;

    p1->dwFlags = LVIF_NORMAL;
    if (pinfo->flags & LVFLAG_BITMAP)
        p1->dwFlags |= LVIF_BITMAP;
    if (pinfo->flags & LVFLAG_ICON)
        p1->dwFlags |= LVIF_ICON;

	if(pinfo->flags & LVFLAG_BLANK)
		p1->dwFlags |= LVIF_BLANK;

	if(pinfo->flags & LVFLAG_CHECKED)
		p1->dwFlags |= LVIF_CHECKED;
	if(pinfo->flags & LVFLAG_PARTCHECKED)
		p1->dwFlags |= LVIF_PARTCHECKED;
     
    p1->Image = pinfo->image;

    scrollview_refresh_item (&plvdata->svdata, (HSVITEM)pi->addData);
    return 0;
}

static int 
lvRemoveColumn (HWND hWnd, PLVDATA plvdata, int nCols)
{
    int offset;
    PLSTHDR pdel;

    if (!LV_BE_VALID_COL(nCols))
        return -1;

    offset = -(GET_SUBITEM_W(nCols));
    sAddOffsetToTailSubItem (nCols+1, offset, plvdata);

    pdel = lvGetHdrByCol(&plvdata->hdrqueue, nCols);
    lvHdrDel (pdel);
    lvDelSubitem (plvdata, nCols);

    plvdata->nCols --;
    plvdata->nHeadWidth += offset;
    lvSetContWidth (hWnd, plvdata, plvdata->nHeadWidth);

    if (plvdata->nCols == 0) {
        lvRemoveAllItem (hWnd, plvdata);
    }

    return 0;
}

/* -------------------------------------------------------------------------- */

static int lvAddTree (PLVDATA plvdata, PITEMDATA pp, PITEMDATA pi)
{
    if (!pp || !pi)
        return -1;

    if (!pp->child)
        pp->child = pi;

    while (pp) {
        pp->child_nr += pi->child_nr + 1;
        pp = pp->parent;
    }

    return 0;
}

static PITEMDATA lvGetPrevSib (PLVDATA plvdata, PITEMDATA pi)
{
    PITEMDATA pp, prev;
    if (!pi)
        return NULL;

    pp = pi->parent;
    prev = pp->child;

    while (prev && prev->next != pi) {
        prev = prev->next;
    }

    return prev;
}

static int lvDelTree (PLVDATA plvdata, PITEMDATA pi)
{
    PITEMDATA pp, prev;

    if (!pi)
        return -1;

    if ( !(pp = pi->parent))
        return -1;

    if ( (prev = lvGetPrevSib(plvdata, pi)) ) {
        prev->next = pi->next;
    }
    else {
        pp->child = pp->next;
    }

    while (pp) {
        pp->child_nr -= pi->child_nr+1;
        pp = pp->parent;
    }

    return 0;
}

static PITEMDATA lvGetLastChild (PLVDATA plvdata, PITEMDATA pitem)
{
    PITEMDATA pi;

    if (!pitem)
        return NULL;

    if ( !(pi = pitem->child) )
        return pitem;

    while (pi->next) {
        pi= pi->next;
    }

    return lvGetLastChild (plvdata, pi);
}

static int lvGetSibItems (PLVDATA plvdata, PITEMDATA parent, int nItem, 
                PITEMDATA *pprev, PITEMDATA *pnext)
{
    PITEMDATA pi = NULL;
    int i = 0;

    if (!parent)
        return -1;

    pi = parent->child;
    while (pi && i < nItem-1) {
        pi = pi->next;
        i++;
    }

    if (pprev) {
        *pprev = pi;
    }
    if (pnext) {
        *pnext = pi ? pi->next : NULL;
    }

    return 0;
}

static void lvInitRootItem (PLVDATA plvdata)
{
    plvroot->dwFlags = 0;
    plvroot->depth = -1;
    plvroot->next = NULL;
    plvroot->child = NULL;
    plvroot->parent = NULL;
    plvroot->child_nr = 0;
    plvroot->itemData = 0;
    plvroot->addData = 0;
}

static PITEMDATA itemNew (int nCols)
{
    PSUBITEMDATA psub = NULL;
    PITEMDATA pi = NULL;
    int i;

    if (nCols <= 0)
        return NULL;

    pi = (PITEMDATA) malloc (sizeof (ITEMDATA));

    INIT_LIST_HEAD (&pi->subqueue);
    for (i = 0; i < nCols; i++)
    {
        psub = subitemNew (NULL);
        list_add_tail (&psub->list, &pi->subqueue);
    }

    return pi;
}

static int itemDelete (HWND hWnd, PLVDATA plvdata, PITEMDATA pItem)
{
    PSUBITEMDATA psub = NULL;
    list_t *me, *n;

    if (!pItem)
        return -1;
    list_for_each_safe (me, n, &pItem->subqueue) {
        psub = list_entry (me, SUBITEMDATA, list);
        subitemFree (psub);
    }
    free (pItem);

    return 0;
}

static int sRemoveItemFromList (HWND hWnd, PLVDATA plvdata, PITEMDATA pi)
{
    if (!pi)
        return -1;
    lvDelTree (plvdata, pi);
    scrollview_del_item (hWnd, &plvdata->svdata, 0, (HSVITEM)pi->addData);
    return 0;
}

static int lvRemoveAllItem (HWND hWnd, PLVDATA plvdata)
{
    int cont_width;

    cont_width = plvscr->nContWidth;

    scrollview_reset_content (hWnd, &plvdata->svdata);
    lvInitRootItem (plvdata);
    lvSetContWidth (hWnd, plvdata, cont_width);
    return 0;
}

void lvDelItem (HWND hWnd, HSVITEM hsvi)
{
    PITEMDATA pi = LV_GET_ITEM(hsvi);
    PLVDATA plvdata = (PLVDATA)GetWindowAdditionalData2(hWnd);
    itemDelete (hWnd, plvdata, pi);
}

static void lvSetColumnWidth (HWND hWnd, PLVDATA plvdata, int nCols, int width)
{
    int offset;
    PLSTHDR ph = lvGetHdrByCol (&plvdata->hdrqueue, nCols);

    if (!ph) return;

    if (width < COLWIDTHMIN)
        width = COLWIDTHMIN;

    offset = width - ph->width;
    ph->width = width;
    sAddOffsetToTailSubItem (nCols+1, offset, plvdata);
    plvdata->nHeadWidth += offset;
}

static void lvBorderDrag (HWND hWnd, int x, int y)
{
    PLVDATA plvdata;
    PLSTHDR pDrag;
    int mouseX = x, mouseY = y;
    RECT rect;
    int offset;
    HDC hdc;

    plvdata = (PLVDATA) GetWindowAdditionalData2 (hWnd);
    pDrag = lvGetHdrByCol (&plvdata->hdrqueue, plvdata->nItemDraged);

    GetClientRect (hWnd, &rect);
    ScreenToClient (hWnd, &mouseX, &mouseY);

    //the column width should not less than the min value
    if ((pDrag->x - plvscr->nContX + COLWIDTHMIN) > mouseX - 1)
        return;

    offset = mouseX - (pDrag->x + pDrag->width - plvscr->nContX);
    lvSetColumnWidth (hWnd, plvdata, plvdata->nItemDraged, pDrag->width+offset);

    rect.bottom = rect.top + plvdata->nHeadHeight+1;
    //FIXME
    //InvalidateRect(hWnd, &rect, TRUE);
    hdc = GetClientDC(hWnd);
    lv_draw_header (hWnd, hdc);
    ReleaseDC (hdc);
}

static void lvToggleSortStatus (PLSTHDR p1)
{
    switch (p1->sort)
    {
    case NOTSORTED:
        p1->sort = LOSORTED;
        break;
    case HISORTED:
        p1->sort = LOSORTED;
        break;
    case LOSORTED:
        p1->sort = HISORTED;
        break;
    }
}

static PITEMDATA lvFindItem (PLVDATA plvdata, PITEMDATA p1, PLVFINDINFO pFindInfo)
{
    HSVITEM hsvi;
    int j = 0, total_nr;
    BOOL by_index;

    if (!pFindInfo)
        return NULL;

    if (!p1) {
        by_index = TRUE;
        p1 = lvGetItemByRow (plvdata, pFindInfo->iStart);
        if (!p1);
            p1 = plvroot->child;
        total_nr = 1; //large than j
    }
    else {
        by_index = FALSE;
        total_nr = p1->child_nr;
    }

    while (p1 && j < total_nr)
    {
        if (pFindInfo->flags & LVFF_ADDDATA) {
            if (pFindInfo->addData == p1->itemData)
                return p1;
        }
        else if (pFindInfo->flags & LVFF_TEXT) {
            PSUBITEMDATA p2;
            list_t *me;
            int i = 0;

            list_for_each (me, &p1->subqueue) {
                p2 = list_entry (me, SUBITEMDATA, list);
                if (plvdata->str_cmp (p2->pszInfo, pFindInfo->pszInfo[i], (size_t)-1) != 0)
                    break;
                if (i >= pFindInfo->nCols)
                    return p1;
                i++;
            }
        }
        
        if (!by_index)
            j++;
        hsvi = scrollview_get_next_item(&plvdata->svdata, (HSVITEM)p1->addData);
        p1 = LV_GET_ITEM(hsvi);
    }

    return NULL;
}

/* -------------------------------------------------------------------------- */

/* The default comparision function for compare two items */
static int 
lvDefCompare (HSVITEM hsvi1, HSVITEM hsvi2, int ncol, PLVDATA plvdata)
{
    PITEMDATA p1, p2;
    PSUBITEMDATA psub1, psub2;

    p1 = LV_GET_ITEM(hsvi1);
    p2 = LV_GET_ITEM(hsvi2);
    psub1 = lvGetSubItemByCol (&p1->subqueue, ncol);
    psub2 = lvGetSubItemByCol (&p2->subqueue, ncol);

    if (psub1 == NULL || psub2 == NULL)
        return -1;

    if(psub1->pszInfo == NULL)
    {
        return (psub2->pszInfo != NULL)?-1:0;
    }
	else if(psub2->pszInfo == NULL)
    {
		return (psub1->pszInfo != NULL)?1:0;
    }

    if (plvdata->str_cmp)
        return plvdata->str_cmp (psub1->pszInfo, psub2->pszInfo, (size_t)-1);
    else
        return strcasecmp (psub1->pszInfo, psub2->pszInfo);
}

static int lv_cmp_fn (MgList *mglst, HSVITEM hsvi1, HSVITEM hsvi2)
{
    HWND hWnd = mglst->hLst;
    PLVDATA plvdata;
    int ret;
    LVSORTDATA sortdata;

    plvdata = (PLVDATA)GetWindowAdditionalData2 (hWnd);
    if (plvdata->pfn_sort) {
        sortdata.ncol = 0;
        sortdata.losorted = 0;
        sortdata.hLV = hWnd;
        ret = plvdata->pfn_sort ( (HLVITEM)LV_GET_ITEM(hsvi1), 
                        (HLVITEM)LV_GET_ITEM(hsvi2), &sortdata );
        if (ret == 0)
            ret = lvDefCompare (hsvi1, hsvi2, 1, plvdata);
    }
    else {
        ret = lvDefCompare (hsvi1, hsvi2, plvdata->cur_col, plvdata);
    }
    
    if ( (plvdata->col_sort == LOSORTED && ret < 0) || 
                    (plvdata->col_sort == HISORTED && ret > 0) )
        return -1;
    return 1;
}

/* sorting items using a comparision function */
static int 
lvSortItem (PFNLVCOMPARE pfn_user, int nCols, SORTTYPE sort, PLVDATA plvdata)
{
    PLSTHDR ph;
    PFNLVCOMPARE pcmp;

    /* If pfn_user is not NULL, use it as comparision function; otherwise, 
     * use the one associated with nCols column.
     */ 
    if (pfn_user)
        pcmp = pfn_user;
    else {
        if ( !(ph = lvGetHdrByCol (&plvdata->hdrqueue, nCols)) )
            return -1;
        pcmp = ph->pfnCmp;
    }

    plvdata->pfn_sort = pcmp;
    plvdata->col_sort = sort;
    plvdata->cur_col = nCols;
    scrollview_sort_items (&plvdata->svdata, (void*)lv_cmp_fn, MG_CMP_TYPE_EX);

    return 0;
}

/* -------------------------------------------------------------------------- */
static void setup_color (HWND hWnd, PLVDATA plvdata)
{
    int color;

    color = (GetFocusChild(GetParent(hWnd)) == hWnd) ? 
                             BKC_HILIGHT_NORMAL : BKC_HILIGHT_LOSTFOCUS;
	if (color == BKC_HILIGHT_NORMAL)
	{
		plvdata->bkc_selected =  0xFF0773BB;
	}
	else
	{
		plvdata->bkc_selected = 0xFF8FA5B2;//GetWindowElementColorEx(hWnd, color);
	}
			
    //plvdata->bkc_selected = GetWindowElementColorEx(hWnd, color);
}

/* -------------------------------------------------------------------------- */

static SVITEMOPS listview_iops =
{
    NULL, lvDelItem, lvDrawItem
};  
    
static int listview_init (HWND hWnd, PLVDATA plvdata)
{
    RECT rc;

    if (!plvdata)
        return -1;

    scrollview_init (hWnd, &plvdata->svdata);
    rc.left = LV_LEFTMARGIN;
    rc.right = LV_RIGHTMARGIN;
    rc.bottom = LV_BOTTOMMARGIN;
    rc.top =  LV_HDRH_DEF(hWnd) + LV_TOPMARGIN;
    scrolled_set_margins (plvscr, &rc);

    plvdata->nHeadHeight = LV_HDRH_DEF(hWnd);
    plvdata->nHeadWidth = 0;
    setup_color (hWnd, plvdata);

    plvdata->nCols = 0;
    //plvdata->nRows = 0;

    plvdata->nColCurSel = -1;

    plvdata->nItemDraged = -1;
    plvdata->pHdrClicked = NULL;
    LVSTATUS(hWnd) = LVST_NORMAL;

    INIT_LIST_HEAD(&plvdata->hdrqueue);
    plvdata->hWnd = hWnd;
    plvdata->str_cmp = strncmp;
    plvdata->pfn_draw_hdr_bk   = NULL;
    plvdata->pfn_draw_hdr_item = NULL;

    scrollview_set_itemops (&plvdata->svdata, (SVITEMOPS*)&listview_iops);

    lvInitRootItem (plvdata);

    SetWindowAdditionalData2 (hWnd, (DWORD) plvdata);
    return 0;
}

//Destroies all the internal datas
static void listview_destroy (HWND hWnd, PLVDATA plvdata)
{
    scrollview_destroy (&plvdata->svdata);
    lvHdrDestroy (plvdata);
}

/* -------------------------------------------------------------------------- */

#define RETURN_DIRECT    return DefaultControlProc(hWnd, message, wParam, lParam)

static int ListViewCtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    PLVDATA plvdata = NULL;

    if (message != MSG_CREATE)
        plvdata = (PLVDATA) GetWindowAdditionalData2 (hWnd);

    switch (message)
    {

    case MSG_CREATE:
    {  
        if (!(plvdata = (PLVDATA) malloc (sizeof (LVDATA))))
            return -1;
        listview_init (hWnd, plvdata);
        RETURN_DIRECT;
    }

    case MSG_DESTROY:
    {
        listview_destroy (hWnd, plvdata);
        free (plvdata);
        RETURN_DIRECT;
    }

    case MSG_SETCURSOR:
    {
        int mouseX = LOSWORD (lParam);
        int mouseY = HISWORD (lParam);

        if ((LVSTATUS(hWnd) & LVST_BDDRAG) || 
               (lvInWhichHeadBorder (mouseX, mouseY, NULL, plvdata) >= 0)) {
            SetCursor (GetSystemCursor (IDC_SPLIT_VERT));
            return 0;
        }
        break;
    }

    case MSG_PAINT:
    {
#if 1
        HDC hdc = BeginPaint (hWnd);
        lvOnDraw (hWnd, hdc);
        EndPaint (hWnd, hdc);
        break;
#else
		#define plstscr  (&plvdata->svdata.scrdata)
        HDC hdc = BeginPaint (hWnd);
        RECT rcDraw, rcVis;

        lvOnDraw (hWnd, hdc);

        scrolled_get_visible_rect (plstscr, &rcVis);
        ClipRectIntersect (hdc, &rcVis);
        scrolled_get_content_rect (plstscr, &rcDraw);
        scrolled_visible_to_window (plstscr, &rcDraw.left, &rcDraw.top);
        scrolled_visible_to_window (plstscr, &rcDraw.right, &rcDraw.bottom);

        scrollview_draw (hWnd, hdc, &plvdata->svdata);

        EndPaint (hWnd, hdc);
        return 0;
        
#endif
    }
    case MSG_SETFOCUS:
    case MSG_KILLFOCUS:
    {
        HSVITEM hsvi;

        setup_color (hWnd, plvdata);
        hsvi = scrollview_get_hilighted_item(&plvdata->svdata);
        if (!hsvi)
            break;
        scrollview_refresh_item (&plvdata->svdata, hsvi);
        break;
    }

    case MSG_GETDLGCODE:
        return DLGC_WANTARROWS | DLGC_WANTCHARS;

    case MSG_LBUTTONDBLCLK:
    {
        int mouseX = LOSWORD (lParam);
        int mouseY = HISWORD (lParam);
        PITEMDATA pi;
        int nCols;
        PLSTHDR p1;

		DWORD  dwStyle;
		dwStyle = GetWindowStyle(hWnd);
        // double clicks on the header
        if ( (nCols = isInListViewHead (mouseX, mouseY, &p1, plvdata)) > 0 )
        {
            break;
        }

        if (isInLVItem (mouseX, mouseY, &pi, plvdata, NULL) >= 0) 
        {
			PLSTHDR pRet;
			int colsIndex;

			colsIndex = GetColumnPara(mouseX, mouseY, &pRet, plvdata);
			if((pRet != NULL) && (colsIndex == 0))	    
        	{
				if ((dwStyle & LVS_CHECKBOX)&&(mouseX > 0 && mouseX < pRet->width))//当鼠标在checkbox内双击时，过滤掉此消息
				{
					printf("[DEBUG]~~~~~~~current mouse is in checkbox!!!!!\n");
					break;
				}
        	}
        	
            NotifyParent (hWnd, GetDlgCtrlID(hWnd), LVN_ITEMDBCLK);
            if (pi->child) {
                BOOL bfold;
                bfold = (ISFOLD(pi)) ? FALSE : TRUE;
                lvFoldItem (hWnd, plvdata, pi, bfold);
                NotifyParentEx (hWnd, GetDlgCtrlID(hWnd), 
                                bfold ? LVN_FOLDED : LVN_UNFOLDED, (DWORD)pi);
            }
        }
        
        break;
    }

    case MSG_MOUSEMOVE:
    {
        int mouseX = LOSWORD (lParam);
        int mouseY = HISWORD (lParam);

        //in head clicked status
        if (LVSTATUS(hWnd) & LVST_HEADCLICK) {
            RECT rc;
            PLSTHDR p1;

            if (GetCapture() != hWnd)
                break;

            ScreenToClient (hWnd, &mouseX, &mouseY);

            p1 = plvdata->pHdrClicked;
            LV_GET_HDR_RECT(p1, rc);

            if (PtInRect(&rc, mouseX, mouseY)) {
                if (!(LVSTATUS(hWnd) & LVST_INHEAD)) {
                    LVSTATUS(hWnd) |= LVST_INHEAD;
                    InvalidateRect (hWnd, &rc, TRUE);
                }
            }
            else if (LVSTATUS(hWnd) & LVST_INHEAD) {
                    rc.left -= 1;
                    rc.right += 1;
                    LVSTATUS(hWnd) &= ~LVST_INHEAD;
                    InvalidateRect (hWnd, &rc, TRUE);
            }
        }
        //in border dragged status
        else if (LVSTATUS(hWnd) & LVST_BDDRAG)
            lvBorderDrag (hWnd, mouseX, mouseY);

        break;
    }

    case MSG_LBUTTONDOWN:
    {
        int mouseX = LOSWORD (lParam);
        int mouseY = HISWORD (lParam);
        int nCols = -1, nRows;
        POINT pt;
        
        RECT rect, rcClient;
        PLSTHDR p1;
        PITEMDATA pi;
		DWORD  dwStyle;
		
		dwStyle = GetWindowStyle(hWnd);
        GetClientRect (hWnd, &rcClient);

        if (GetCapture() == hWnd)
            break;

        SetCapture (hWnd);
        LVSTATUS(hWnd) |= LVST_CAPTURED;

        if (plvdata->nHeadHeight > 0)
            nCols = isInListViewHead (mouseX, mouseY, &p1, plvdata);

        if (nCols >= 0) {  // clicks on the header
			if(mouseX > 0 && mouseX < (LSTV_WIDTH_CHECKMARK+4))
			{
				if(dwStyle & LVS_AUTOCHECK)
				{
	                if (message == MSG_LBUTTONDOWN) 
	                {
						switch(p1->checkFlags & LVI_HEADER_CHECKMASK)
						{
							case LVI_HEADER_CHECK:
								p1->checkFlags &= ~LVI_HEADER_CHECKMASK;
								p1->checkFlags |= LVI_HEADER_BLANK;
                                if ((message == MSG_LBUTTONDOWN) && (dwStyle & LVS_NOTIFY))
                                    NotifyParent (hWnd, GetDlgCtrlID(hWnd), LVN_CHECKBOX_UNSELECTALL);
                            break;
                            case LVI_HEADER_UNCHECK:
                            	return 0;
                            default:
								p1->checkFlags &= ~LVI_HEADER_CHECKMASK;
								p1->checkFlags |= LVI_HEADER_CHECK;
                                if ((message == MSG_LBUTTONDOWN) && (dwStyle & LVS_NOTIFY))
                                    NotifyParent (hWnd, GetDlgCtrlID(hWnd), LVN_CHECKBOX_SELECTALL);
                            break;
						}

	                }

					InvalidateRect (hWnd, NULL, TRUE);
				}
			}
			else
			{
	            LVSTATUS(hWnd) |= LVST_HEADCLICK;
	            LVSTATUS(hWnd) |= LVST_INHEAD;
	            plvdata->pHdrClicked = p1;

	            SetRect (&rect, p1->x - plvscr->nContX, LV_HDR_TOP, 
	                            p1->x - plvscr->nContX+ p1->width, 
	                            LV_HDR_TOP + LV_HDR_HEIGHT);
	            InvalidateRect (hWnd, &rect, TRUE);
			}
        	
        }
        else 
        {
            nCols = lvInWhichHeadBorder (mouseX, mouseY, &p1, plvdata);
            if (plvdata->nHeadHeight > 0 && nCols >= 0) 
            {
                LVSTATUS(hWnd) |= LVST_BDDRAG;
                plvdata->nItemDraged = nCols;
            }
            else if ((nRows = isInLVItem (mouseX, mouseY, &pi, plvdata, &pt)) >= 0) 
            {
                int indent;
                PLSTHDR pRet;
                int colsIndex;
                
			    colsIndex = GetColumnPara(mouseX, mouseY, &pRet, plvdata);
				if((pRet != NULL) && (colsIndex == 0))	    
		        {
		       		 if ((dwStyle & LVS_CHECKBOX)&&(mouseX > 0 && mouseX < pRet->width))
	                {
                   
	                    if (message == MSG_LBUTTONDOWN && dwStyle & LVS_NOTIFY)
	                        NotifyParent (hWnd, GetDlgCtrlID(hWnd), LVN_CLICKCHECKMARK);
                    
	                    if ((dwStyle & LVS_AUTOCHECK) && (message == MSG_LBUTTONDOWN)) 
	                    {
							PSUBITEMDATA psub;
							//printf("[DEBUG]~~~~~~~~~%s: %d,plvdata->nColCurSel: %d,plvdata->nCols: %d\n",__func__,__LINE__,plvdata->nColCurSel,plvdata->nCols);
							if ( !(psub = lvGetSubItemByCol(&pi->subqueue, 0/*plvdata->nColCurSel*/)) )//获取的列数有问题，暂时写死
							return 0;

                            switch (psub->dwFlags & LVIF_CHECKMASK) 
                            {
	                            case LVIF_CHECKED:
	                                psub->dwFlags &= ~LVIF_CHECKMASK;
	                                psub->dwFlags |= LVIF_BLANK;
	                                if ((message == MSG_LBUTTONDOWN) && (dwStyle & LVS_NOTIFY))
	                                    NotifyParentEx(hWnd, GetDlgCtrlID(hWnd), LVN_CHECKBOX_SELCANCEL, nRows);//上报行号
	                                break;
	                            default:
	                                psub->dwFlags &= ~LVIF_CHECKMASK;
	                                psub->dwFlags |= LVIF_CHECKED;
	 								if((message == MSG_LBUTTONDOWN) && (dwStyle & LVS_NOTIFY))
	 									NotifyParentEx(hWnd, GetDlgCtrlID(hWnd), LVN_CHECKBOX_SELECT, nRows);//上报行号
	                               break;
                      		 }
   
 					 		InvalidateRect (hWnd, NULL, TRUE);
                   }
 
               }
 
           }
 
           if ( !(GetWindowStyle(hWnd) & LVS_UPNOTIFY) )
           {
               //NotifyParent (hWnd, GetDlgCtrlID(hWnd), LVN_SELCHANGE);
           }
 
	       else 
	       {
               LVSTATUS(hWnd) |= LVST_ITEMDRAG;		
           }

           indent = lvGetItemIndent (plvdata, pi);

           if (pi->child && pt.x > indent && pt.x < indent + 9
                   /*bmp_fold->bmWidth*/ + 4) 
           {
               BOOL bfold;
               bfold = (ISFOLD(pi)) ? FALSE : TRUE;

               /* select highlight item first.*/

               ScrollViewCtrlProc (hWnd, message, wParam, lParam);
               lvFoldItem (hWnd, plvdata, pi, bfold);
               NotifyParentEx (hWnd, GetDlgCtrlID(hWnd), 
                       bfold ? LVN_FOLDED : LVN_UNFOLDED, (DWORD)pi);
               return 0;
           }
 
           }
 
       }
 
        break;
    }

    case MSG_LBUTTONUP:
    {
        PLSTHDR p1;
        int nCols;
        int mouseX = LOSWORD (lParam);
        int mouseY = HISWORD (lParam);

        if (GetCapture() != hWnd)
            break;

        if (LVSTATUS(hWnd) & LVST_CAPTURED) {
            LVSTATUS(hWnd) &= ~LVST_CAPTURED;
            ReleaseCapture ();
            ScreenToClient (hWnd, &mouseX, &mouseY);
        }
        else
            break;

        if (LVSTATUS(hWnd) & LVST_HEADCLICK)
        {
            LVSTATUS(hWnd) &= ~LVST_HEADCLICK;
            LVSTATUS(hWnd) &= ~LVST_INHEAD;

            nCols = isInListViewHead (mouseX, mouseY, NULL, plvdata);
            if (nCols < 0)
                break;
            p1 = lvGetHdrByCol (&plvdata->hdrqueue, nCols);
            if (!p1 || p1 != plvdata->pHdrClicked)
                break;

            lvToggleSortStatus(p1);
            if ( !(GetWindowStyle(hWnd)&LVS_TREEVIEW) )
                lvSortItem (NULL, nCols, p1->sort, plvdata);
            //FIXME
            InvalidateRect (hWnd, NULL, TRUE);
        }
        else if (LVSTATUS(hWnd) & LVST_BDDRAG)
        {
            LVSTATUS(hWnd) &= ~LVST_BDDRAG;
            lvSetContWidth (hWnd, plvdata, plvdata->nHeadWidth);
        }
        else if (LVSTATUS(hWnd) & LVST_ITEMDRAG && GetWindowStyle(hWnd) & LVS_UPNOTIFY) {
//            HSVITEM hsvi;
            int idx;

            idx = isInLVItem (mouseX, mouseY, NULL, plvdata, NULL);
           /* hsvi = */scrollview_get_item_by_index (&plvdata->svdata, idx);
            NotifyParent (hWnd, GetDlgCtrlID(hWnd), LVN_CLICKED);
/* LVS_UPNOTIFY only affects LVN_CLICKED */
/*
            if (hsvi && hsvi != scrollview_get_hilighted_item (&plvdata->svdata)) {
                if ( GetWindowStyle(hWnd) & LVS_UPNOTIFY ) {
                    NotifyParent (hWnd, GetDlgCtrlID(hWnd), LVN_SELCHANGE);
                }
            }
*/
        }
 
        break;
    }

    case MSG_RBUTTONUP:
    case MSG_RBUTTONDOWN:
    {
        int mouseX = LOSWORD (lParam);
        int mouseY = HISWORD (lParam);
        int nCols, nRows;
        RECT rcClient;
        PLSTHDR p1;
        LVNM_NORMAL lvnm;

        lvnm.wParam = wParam;
        lvnm.lParam = lParam;

        GetClientRect (hWnd, &rcClient);

        // clicks on the header
        if ( (nCols = isInListViewHead (mouseX, mouseY, &p1, plvdata)) > 0 )
        {
            if (message == MSG_RBUTTONDOWN)
                NotifyParentEx (hWnd, GetDlgCtrlID(hWnd), LVN_HEADRDOWN, 
                                (DWORD)&lvnm);
            else
                NotifyParentEx (hWnd, GetDlgCtrlID(hWnd), LVN_HEADRUP, 
                                (DWORD)&lvnm);
        }
        // clicks on an item
        else if ( (nRows = isInLVItem (mouseX, mouseY, NULL, plvdata, NULL)) > 0) {
            if (message == MSG_RBUTTONDOWN) {
                //NotifyParent (hWnd, GetDlgCtrlID(hWnd), LVN_SELCHANGE);
                NotifyParentEx (hWnd, GetDlgCtrlID(hWnd), LVN_ITEMRDOWN, (DWORD)&lvnm);
            }
            else {
                NotifyParentEx (hWnd, GetDlgCtrlID(hWnd), LVN_ITEMRUP, (DWORD)&lvnm);
            }
        }
        break;
    }

    case MSG_KEYDOWN:
    {
        LVNM_NORMAL lvnm;

        lvnm.wParam = wParam;
        lvnm.lParam = lParam;
        NotifyParentEx (hWnd, GetDlgCtrlID(hWnd), LVN_KEYDOWN, (DWORD)&lvnm);
        break;
    }
	case MSG_MOUSE_SCROLL:
	{		//printf("[DEBUG] ........wParam = %d\n",wParam);
		message = MSG_VSCROLL;//将鼠标滚轮的滚动消息转换为垂直滑动条滑动消息

        break;
    }

    case MSG_HSCROLL:
    {
        RECT rc;

        GetClientRect (hWnd, &rc);
        rc.bottom = rc.top + plvdata->nHeadHeight + 1;
        InvalidateRect (hWnd, &rc, TRUE);
        break;
    }

    case LVM_ADDCOLUMN:
    {
        PLVCOLUMN p1;

        if ( !(p1 = (PLVCOLUMN)lParam) )
            return LV_ERR;

        if (!lvAddColumnToList (hWnd, plvdata, p1))
            return LV_ERR;

        return LV_OKAY;
    }

    case LVM_ADDITEM:
    {
        PLVITEM p1;
        SVITEMINFO svii;
        PITEMDATA parent = (PITEMDATA)wParam;
        PITEMDATA pnew, pprev = NULL, pnext = NULL, ptmp;
        HSVITEM hsvi;
        int nItem = 0; //FIXME

        if ( !(p1 = (PLVITEM) lParam) )
            return -1;
        if ( !(pnew = itemNew (plvdata->nCols)) )
            return -1;

        if (!parent)
       	{
            parent = plvroot;
            parent->child = NULL;  //listview:添加一行数据时，序号没有按照大小排序
		}
        else {
            IncludeWindowStyle (hWnd, LVS_TREEVIEW);
        }

        lvGetSibItems (plvdata, parent, p1->nItem, &pprev, &pnext);

        if (p1->nItemHeight > 0)
            pnew->itemh = p1->nItemHeight;
        else
            pnew->itemh = LV_ITEMH_DEF(hWnd);

        pnew->parent = parent;
        pnew->child = NULL;
        pnew->depth = parent->depth + 1;
        pnew->next = pnext;
        pnew->child_nr = 0;
        pnew->dwFlags = p1->dwFlags;
        pnew->itemData = p1->itemData;

        if (pprev)
            pprev->next = pnew;
        lvAddTree (plvdata, parent, pnew);

        if (ISFOLD(parent)) {
            svii.nItemHeight = 0;
        }
        else {
            svii.nItemHeight = pnew->itemh;
        }

        svii.nItem = p1->nItem;
        svii.addData = (DWORD)pnew;

        if (pnext) {
            hsvi = (HSVITEM)pnext->addData;
            pnew->addData = (DWORD) scrollview_add_item_ex (hWnd, &plvdata->svdata, 
                        0, hsvi, &svii, &nItem);
        }
        else if (pprev) {
            ptmp = lvGetLastChild (plvdata, pprev);
            hsvi = ptmp->addData;
            pnew->addData = (DWORD) scrollview_add_item (hWnd, &plvdata->svdata, 
                        hsvi, &svii, &nItem);
        }
        else {
            hsvi = parent->addData;
            pnew->addData = (DWORD) scrollview_add_item (hWnd, &plvdata->svdata, 
                        hsvi, &svii, &nItem);
        }

        return (int)pnew;
    }

    case LVM_GETITEM:
    {
        PLVITEM p1;
        PITEMDATA pitem;
        
        p1 = (PLVITEM) lParam;
        pitem = (PITEMDATA)wParam;

        if (!p1)
            return LV_ERR;

        if (!pitem)
            pitem = lvGetItemByRow (plvdata, p1->nItem);
        if (!pitem)
            return LV_ERR;

        p1->itemData = pitem->itemData;
        p1->nItemHeight = scrollview_get_item_height ((HSVITEM)pitem->addData);
        p1->dwFlags = pitem->dwFlags;

        return LV_OKAY;
    }
    case LVM_GETITEMCOUNT:
    {
        return scrollview_get_item_count(&plvdata->svdata);
    }
    case LVM_GETSELECTEDITEM:
    {
        HSVITEM hsvi;
        PITEMDATA pi;

        hsvi = scrollview_get_hilighted_item (&plvdata->svdata);
        pi = LV_GET_ITEM (hsvi);
        return (int)pi;
    }

    case LVM_GETRELATEDITEM:
    {
        PITEMDATA pi = (PITEMDATA)lParam;

        if (!pi)
            return 0;

        if (wParam == LVIR_PARENT) {
            return (int)pi->parent;
        }
        else if (wParam == LVIR_FIRSTCHILD) {
            return (int)pi->child;
        }
        else if (wParam == LVIR_NEXTSIBLING) {
            return (int)pi->next;
        }
        else if (wParam == LVIR_PREVSIBLING) {
            return (int)lvGetPrevSib(plvdata, pi);
        }
        return 0;
    }

    case LVM_GETCOLUMN:
    {
        int col = (int)wParam;
        PLVCOLUMN pcol = (PLVCOLUMN)lParam;
        PLSTHDR ph = lvGetHdrByCol(&plvdata->hdrqueue, col);

        if (!ph)
            return -1;

        pcol->width = ph->width;
        strncpy (pcol->pszHeadText, ph->pTitle, pcol->nTextMax);
        pcol->pszHeadText[pcol->nTextMax] = 0;
        pcol->image = ph->Image;
        pcol->pfnCompare = ph->pfnCmp;
        pcol->colFlags = ph->flags;

        return 0;
    }
    case LVM_GETCOLUMNWIDTH:
    {
        int col = (int)wParam;
        PLSTHDR ph = lvGetHdrByCol(&plvdata->hdrqueue, col);
        if (ph)
            return ph->width;
        else
            return -1;
    }
    case LVM_GETCOLUMNCOUNT:
    {
        return plvdata->nCols;
    }

    case LVM_FILLSUBITEM:
    case LVM_SETSUBITEM:
    {
        PLVSUBITEM p1;
        PITEMDATA pi = (PITEMDATA)wParam;
        
        if ( !(p1 = (PLVSUBITEM)lParam) )
            return LV_ERR;

        if ( (lvSetSubItem(plvdata, pi, p1)) < 0 )
            return LV_ERR;

        return LV_OKAY;
    }

    case LVM_GETSUBITEMLEN:
    {
        PLVSUBITEM p1;
        PSUBITEMDATA p2;
        PITEMDATA pi = (PITEMDATA)wParam;

        if ( !(p1 = (PLVSUBITEM) lParam) )
            return -1;

        if (!pi)
            pi = lvGetItemByRow (plvdata, p1->nItem);
        if (!pi)
            return -1;
        p2 = lvGetSubItemEx (pi, p1->subItem, plvdata);

        if (!p2 || !p2->pszInfo)
            return LV_ERR;

        return strlen (p2->pszInfo);
    }
    case LVM_GETSUBITEMTEXT:
    {
        PLVSUBITEM p1;
        PSUBITEMDATA p2;
        PITEMDATA pi = (PITEMDATA)wParam;

        if ( !(p1 = (PLVSUBITEM) lParam) )
            return -1;

        if (!pi)
            pi = lvGetItemByRow (plvdata, p1->nItem);
        if (!pi)
            return -1;
        p2 = lvGetSubItemEx (pi, p1->subItem, plvdata);

        if (!p2 || !p2->pszInfo)
            return LV_ERR;

        strcpy (p1->pszText, p2->pszInfo);

        return 0;
    }
    case LVM_MODIFYHEAD:
    {
        PLVCOLUMN p1 = (PLVCOLUMN) lParam;
        if (!p1)
            return LV_ERR;
        if (sModifyHeadText (p1->nCols, p1->pszHeadText, plvdata) >= 0) {
            RECT rc;
            GetClientRect (hWnd, &rc);
            rc.bottom = rc.top + plvdata->nHeadHeight;
            InvalidateRect (hWnd, &rc, TRUE);
            return LV_OKAY;
        }
        return LV_ERR;
    }
    case LVM_SETCOLUMN:
    {
        PLVCOLUMN p1 = (PLVCOLUMN) lParam;
        PLSTHDR ph;

        if (!p1)
            return LV_ERR;
        if ( !(ph = lvGetHdrByCol (&plvdata->hdrqueue, p1->nCols)) )
            return LV_ERR;

        sModifyHeadText (p1->nCols, p1->pszHeadText, plvdata);
        lvSetColumnWidth (hWnd, plvdata, p1->nCols, p1->width);
        lvSetContWidth (hWnd, plvdata, plvdata->nHeadWidth);
        ph->Image = p1->image;
        ph->flags = p1->colFlags;
 
        return LV_OKAY;
    }
    case LVM_FINDITEM:
    {
        return (int)lvFindItem(plvdata, (PITEMDATA)wParam, (PLVFINDINFO)lParam);
    }
    case LVM_DELITEM:
    {
        PITEMDATA pitem = (PITEMDATA)lParam;
        if (!pitem)
            return LV_ERR;
        if (sRemoveItemFromList (hWnd, plvdata, pitem)) 
            return LV_ERR;
        return LV_OKAY;
    }
    case LVM_DELALLITEM:
    {
        lvRemoveAllItem (hWnd, plvdata); 
        return LV_OKAY;
    }

    //TODO
    case LVM_CLEARSUBITEM:
        break;

    case LVM_DELCOLUMN:
    {
        if (lvRemoveColumn (hWnd, plvdata, (int)wParam))
            return LV_ERR;
        return LV_OKAY;
    }

    //TODO, multi select
    case LVM_SELECTITEM:
    {
        PITEMDATA pitem = (PITEMDATA)lParam;
        if (!pitem)
            pitem = lvGetItemByRow (plvdata, wParam);
        if (!pitem)
            return -1;
        scrollview_hilight_item (&plvdata->svdata, (HSVITEM)pitem->addData);
        return 0;
    }
    case LVM_SHOWITEM:
    {
        PITEMDATA pitem = (PITEMDATA)lParam;
        if (!pitem)
            pitem = lvGetItemByRow (plvdata, wParam);
        if (!pitem)
            return -1;
        scrollview_make_item_visible (&plvdata->svdata, (HSVITEM)pitem->addData);
        return 0;
    }
    case LVM_CHOOSEITEM:
    {
        PITEMDATA pitem = (PITEMDATA)lParam;
        if (!pitem)
            pitem = lvGetItemByRow (plvdata, wParam);
        if (!pitem)
            return -1;
        scrollview_hilight_item (&plvdata->svdata, (HSVITEM)pitem->addData);
        scrollview_make_item_visible (&plvdata->svdata, (HSVITEM)pitem->addData);
        return 0;
    }
    case LVM_FOLDITEM:
    {
        PITEMDATA pitem = (PITEMDATA)lParam;
        if (!pitem)
            return -1;
        lvFoldItem (hWnd, plvdata, pitem, wParam);
        return 0;
    }

    case LVM_GETITEMADDDATA:
    {
        PITEMDATA pitem = (PITEMDATA)lParam;

        if (!pitem)
            pitem = lvGetItemByRow (plvdata, (int)wParam);

        if (pitem)
            return pitem->itemData;
        else
            return 0;
    }

    case LVM_SETITEMADDDATA:
    {
        PITEMDATA pitem = (PITEMDATA)wParam;

        if (pitem) {
            pitem->itemData = (DWORD)lParam;
            return LV_OKAY;
        }
        else
            return LV_ERR;
    }

    case LVM_SETSUBITEMCOLOR:
    {
        PLVSUBITEM p1;
        PITEMDATA pitem;
        
        p1 = (PLVSUBITEM) lParam;
        pitem = (PITEMDATA) wParam;

        if (!pitem)
            pitem = lvGetItemByRow(plvdata, p1->nItem);

        lvSetSubItemTextColor(pitem, p1->subItem, p1->nTextColor, plvdata); 
        scrollview_refresh_item (&plvdata->svdata, (HSVITEM)pitem->addData);
        return 0;
    }

    case LVM_SETSUBITEMTEXT:
    {
        PLVSUBITEM p1;
        HSVITEM hsvi;
        PITEMDATA pitem;

        p1 = (PLVSUBITEM) lParam;
        pitem = (PITEMDATA) wParam;
        
        if (!pitem)
            pitem = lvGetItemByRow(plvdata, p1->nItem);
        if (!pitem)
            return LV_ERR;
        if (lvFillSubItem(pitem, p1->subItem, p1->pszText, plvdata) < 0)
            return LV_ERR;

        hsvi = (HSVITEM)pitem->addData;
        //FIXME, TODO, scrollview_refresh_item_rect
        scrollview_refresh_item (&plvdata->svdata, hsvi);
        return LV_OKAY;
    }

    case LVM_SETSTRCMPFUNC:
        if (lParam) {
            plvdata->str_cmp = (STRCMP)lParam;
            return 0;
        }
        return -1;

    case LVM_SETITEMHEIGHT:
    {
        PITEMDATA pi = (PITEMDATA)wParam;
        int new_h = (int)lParam;

        if (!pi)
            return FALSE;

        pi->itemh = new_h;
        scrollview_set_item_height (hWnd, (HSVITEM)pi->addData, new_h);
        return TRUE;
    }

    case LVM_SETHEADHEIGHT:
    {
        int height = (int)wParam; 
        RECT rc;

        if (height < 0)
            height = 0;
        plvdata->nHeadHeight = height;

        rc.left = LV_LEFTMARGIN;
        rc.right = LV_RIGHTMARGIN;
        rc.bottom = LV_BOTTOMMARGIN;
        rc.top =  plvdata->nHeadHeight + LV_TOPMARGIN;
        scrolled_set_margins (plvscr, &rc);

        return TRUE;
    }
        
    case LVM_COLSORT:
    {
        int nCols = (int)wParam;

        if (GetWindowStyle(hWnd) & LVS_TREEVIEW)
            return LV_ERR;

        if (lvSortItem (NULL, nCols, LOSORTED, plvdata) == 0) {
            return LV_OKAY;
        }
        return LV_ERR;
    }

    case LVM_SORTITEMS:
    {
        PFNLVCOMPARE pfn = (PFNLVCOMPARE)lParam;

        if (GetWindowStyle(hWnd) & LVS_TREEVIEW)
            return LV_ERR;

        if (lvSortItem (pfn, 0, LOSORTED, plvdata) == 0) {
            return LV_OKAY;
        }
        return LV_ERR;
    }

    case LVM_SETCUSTOMDRAW:
    {
        LVCUSTOMDRAWFUNCS *pnewFuncs = (LVCUSTOMDRAWFUNCS *)lParam;
        if (pnewFuncs) {
            plvdata->pfn_draw_hdr_bk   = pnewFuncs->pfnDrawHdrBk;
            plvdata->pfn_draw_hdr_item = pnewFuncs->pfnDrawHdrItem;
        }
        return 0;
    }
	case LVM_GETSELECTEDITEMRECT:
		{
	        HSVITEM hsvi;
			RECT *prt = (RECT*) lParam;
			if(prt == NULL)
				return FALSE;
			hsvi = scrollview_get_hilighted_item(&plvdata->svdata);
			if (!hsvi)
				return FALSE;
			return scrollview_get_item_rect(hWnd, (HITEM)hsvi, prt, TRUE);
			/*{
				prt->top += plvdata->nHeadHeight;
				prt->bottom += plvdata->nHeadHeight;
				return TRUE;
			}*/
			return FALSE;
		}
		
#if 1   
	    case LVM_GETCHECKMARK:
	    {
			PLVSUBITEM p1;
			PSUBITEMDATA p2;
			PITEMDATA pi = (PITEMDATA)wParam;

	        if ( !(p1 = (PLVSUBITEM) lParam) )
	            return -1;

	        if (!pi)
	            pi = lvGetItemByRow (plvdata, p1->nItem);
	        if (!pi)
	            return -1;
	        p2 = lvGetSubItemEx (pi, p1->subItem, plvdata);

	        if (!p2 || !p2->pszInfo)
	            return -1;

	        if (p2->dwFlags & LVIF_CHECKED)
	            return LVFLAG_CHECKED;

	        if (p2->dwFlags & LVIF_PARTCHECKED)
	            return LVFLAG_PARTCHECKED;

	        return LVFLAG_BLANK;
	    }
        case LVM_SETCHECKMARK:
        {
			DWORD  dwStyle;
			dwStyle = GetWindowStyle(hWnd);

            if (!(dwStyle & LVS_CHECKBOX))
                return -1;

			PLVSUBITEM p1;
			PSUBITEMDATA p2;
			PITEMDATA pi = (PITEMDATA)wParam;

	        if ( !(p1 = (PLVSUBITEM) lParam) )
	            return -1;

	        if (!pi)
	            pi = lvGetItemByRow (plvdata, p1->nItem);
	        if (!pi)
	            return -1;
	        p2 = lvGetSubItemEx (pi, p1->subItem, plvdata);

	        if (!p2 || !p2->pszInfo)
	            return -1;
			//printf("[debug]~~~~~~~~~~~~~~~%s %d p1->flags: 0x%x\n",__func__,__LINE__,p1->flags);
            p2->dwFlags &= ~LVIF_CHECKMASK;
            switch (p1->flags) {
                case LVFLAG_CHECKED:
                    p2->dwFlags |= LVIF_CHECKED;
                break;
                case LVFLAG_PARTCHECKED:
                    p2->dwFlags |= LVIF_PARTCHECKED;
                break;
                case LVFLAG_BLANK:
                	p2->dwFlags |= LVIF_BLANK;
                break;
            }
                
			InvalidateRect (hWnd, NULL, FALSE);
            return 0;
        }
        case LVM_GETEDITEM:
        {
	        PITEMDATA pi;
	        int nItem = (int)lParam;
			//printf("[debug]~~~~~~~~~~~~~~~~~~~~~nItem=%d\n",nItem);
	        pi = lvGetItemByRow (plvdata, nItem);
	        return (int)pi;
        }

        case LVM_SETHEAD_CHECKMARK:
        {
        	int col = (int) wParam;
        	unsigned long status = lParam;

            PLSTHDR p1;
			p1 = lvGetHdrByCol(&plvdata->hdrqueue, col);

			p1->checkFlags = status;
			//printf("[debug]~~~~~~~~~~~~~~~~~~~~~status = %lu\n",status);
			InvalidateRect(hWnd, NULL, FALSE);

			return 0;
		}
		
        case LVM_GETHEAD_CHECKMARK:
        {
        	int col = (int) wParam;

            PLSTHDR p1;
			p1 = lvGetHdrByCol(&plvdata->hdrqueue, col);

			return (p1->checkFlags);
		}

#endif

    } 
    /* end switch */

    return ScrollViewCtrlProc (hWnd, message, wParam, lParam);
}


BOOL RegisterMyListView (void)
{
    WNDCLASS WndClass;

    if ((bmp_fold = GetStockBitmap (STOCKBMP_LVFOLD, -1, -1)) == NULL)
        fprintf (stderr, "listview: load fold bitmap failure\n");
    if ((bmp_unfold = GetStockBitmap (STOCKBMP_LVUNFOLD, -1, -1)) == NULL)
        fprintf (stderr, "listview: load unfold bitmap failure\n");

	if ((bmp_checkmark = GetStockBitmap (STOCKBMP_CHECKMARK, -1, -1)) == NULL)
		fprintf (stderr, "listview: load checkmark bitmap failure\n");


    WndClass.spClassName    = "listview_ex";
    WndClass.dwStyle        = WS_NONE;
    WndClass.dwExStyle      = WS_EX_NONE;
    WndClass.hCursor        = GetSystemCursor (0);
    WndClass.iBkColor       = PIXEL_black;//0xFFE6E5E4;//GetWindowElementColor (WEC_3DBOX_LIGHT);
    WndClass.WinProc        = ListViewCtrlProc;

    return RegisterWindowClass (&WndClass);
}

void MyListViewCleanup (void)
{
    UnregisterWindowClass ("listview_ex");
}

#endif /* _EXT_CTRL_LISTVIEW */
