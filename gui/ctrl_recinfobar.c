#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/cliprect.h>
#include <minigui/internals.h>
#include <minigui/ctrlclass.h>

#ifndef	_CUSTOM_CTRL_RECINFOBAR 
#define _CUSTOM_CTRL_RECINFOBAR 
#endif

#ifdef _CUSTOM_CTRL_RECINFOBAR

#include "ctrl_recinfobar.h"
//#include "recinfobar_impl.h"

#define MAX_COLOR_NUM 	(32)
#define INTRL_INVALID 	(unsigned char)0
#define INTRL_VALID		(unsigned char)1
#define INTRL_MIN 		(int)0
#define INTRL_MAX		(int)86400  /* (int)24 */		/* 一天的秒数 */
#define RIB_BORDER 		1

typedef struct _T_COLOR
{
	Uint8 u8_red;
	Uint8 u8_green;
	Uint8 u8_blue;
	Uint8 u8_alpha;
}T_COLOR;


typedef struct tagINTERVAL
{
	unsigned char valid; 	/*  是否可用  0 不可用， 1 为可用*/
	int nType;		/*  录像类型 */
	int nStart;	/*  区间的起始位置 */
	int nEnd;		/*  区间的结束位置 */
}INTERVAL;

typedef struct INTERVAL *PINTERVAL;

typedef struct tagRECINFOBARDATA
{
	int nMin;		/* 显示范围最小值 */
	int nMax;		/* 显示范围最大值 */
	int nCurMin;	/* 当前显示的最小值 */	
	int nCurMax;	/* 当前显示的最大值 */
	int nPos;		/* indicator指示的逻辑位置 */
	int nMousePos;	/* 鼠标位置 */

	unsigned char ucZoomout;	/* 当前放大的倍数 */
	
	INTERVAL interval[MAX_INTRL_NUM+1]; /* 区间 */

	int nColorBorder;				/* 边框的颜色 */
	int nColorBkgnd;				/* 背景颜色 */
	int nColorInd;					/* 指示游标颜色 */
	T_COLOR atColorRec[MAX_COLOR_NUM];	/* 各种录像颜色 */

	DWORD dwStatus;	
}RECINFOBARDATA;

typedef RECINFOBARDATA *PRECINFOBARDATA;


static int RecinfoBarProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam);

BOOL RegisterRecinfoBarControl (void)
{
    WNDCLASS WndClass;
	
    WndClass.spClassName = CTRL_RECINFOBAR;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (0);
    WndClass.iBkColor    = GetWindowElementColor (BKC_CONTROL_DEF);
    WndClass.WinProc     = RecinfoBarProc;

    return RegisterWindowClass (&WndClass);
}

void RecinfoBarControlCleanup (void)
{
    UnregisterWindowClass (CTRL_RECINFOBAR);
}

static void RecinfoBarSetZoomout(HWND hwnd, RECINFOBARDATA * pData, unsigned char ucZoomout)
{
	if(ucZoomout < ZOOM_OUT_X0 || ucZoomout > ZOOM_OUT_X1_48)
		return;
		
	if(pData->ucZoomout != ucZoomout)
	{
		pData->ucZoomout = ucZoomout;

		switch(pData->ucZoomout)
		{
			case ZOOM_OUT_X0:
			{
				pData->nCurMin = pData->nMin;
				pData->nCurMax = pData->nMax;
				
				break;
			}

			case ZOOM_OUT_X1_3:
			{
				pData->nCurMin = pData->nMin + (pData->nPos - pData->nMin)*2.0 / 3.0;
				pData->nCurMax = pData->nPos + (pData->nMax - pData->nPos)*1.0 / 3.0;

				break;
			}

			
			case ZOOM_OUT_X1_6:
			{
				pData->nCurMin = pData->nMin + (pData->nPos - pData->nMin)*5.0 / 6.0;
				pData->nCurMax = pData->nPos + (pData->nMax - pData->nPos)*1.0 / 6.0;

				break;
			}

			case ZOOM_OUT_X1_12:
			{
				pData->nCurMin = pData->nMin + (pData->nPos - pData->nMin)*11.0 / 12.0;
				pData->nCurMax = pData->nPos + (pData->nMax - pData->nPos)*1.0 / 12.0;

				break;
			}

			case ZOOM_OUT_X1_24:
			{
				pData->nCurMin = pData->nMin + (pData->nPos - pData->nMin)*23.0 / 24.0;
				pData->nCurMax = pData->nPos + (pData->nMax - pData->nPos)*1.0 / 24.0;

				break;
			}

			case ZOOM_OUT_X1_48:
			{
				pData->nCurMin = pData->nMin + (pData->nPos - pData->nMin)*47.0 / 48.0;
				pData->nCurMax = pData->nPos + (pData->nMax - pData->nPos)*1.0 / 48.0;

				break;
			}

			default:
				break;
		}
	}
}

static BOOL RecinfoBarCheckType(HWND hwnd, int nType, RECINFOBARDATA * pData)
{
	int nFilterType = 
		((nType & REC_TYPE_MASK) & (pData->dwStatus & REC_TYPE_MASK));
	if(nFilterType < 1 || nFilterType > REC_TYPE_MASK)
		return FALSE;

		return TRUE;
}
static int RetriveRecTypeColor(int nRecType, int nFilter)
{
	int nType = (nRecType & REC_TYPE_MASK);
	nType &= (nFilter & REC_TYPE_MASK);
	if(nType & REC_ALARM_MASK)
		return REC_ALARM_COLOR;
	else if(nType & REC_MOTION_MASK)
		return REC_MOTION_COLOR;
	else if(nType & REC_TIMER_MASK)
		return REC_TIMER_COLOR;
	else if(nType & REC_MANNUAL_MASK)
		return REC_MANNUAL_COLOR;
	else if(nType & REC_STARTUP_MASK)
		return REC_STARTUP_COLOR;
	else
		return REC_COLOR_NONE;
}

static void RecinfoBarOnDraw(HWND hwnd, HDC hdc, RECINFOBARDATA * pData, DWORD dwStyle)
{
	//int nColorIndex = 0;
	T_COLOR *pColor = NULL;
	RECT rc_client, rc_area, rc_text;
	int  i, x, y, w, h;
	int nFixStart = 0, nFixEnd = 0;
	int nPosX;
	INTERVAL *pIntval = NULL;
	char szTime[32] = {'\0'};
	
	GetClientRect (hwnd, &rc_client);
	SetBrushColor(hdc, RGB2Pixel(HDC_SCREEN, 0x05, 0x05, 0x05));
	FillBox(hdc, rc_client.left, rc_client.top, RECTW(rc_client), RECTH(rc_client));
	
	/* draw the border of this control by the style */
	if(dwStyle & RIBS_BORDER)
	{
		SetPenColor(hdc, pData->nColorBorder);
		SetPenWidth(hdc, RIB_BORDER);
		MoveTo(hdc, rc_client.left, rc_client.top);
		LineTo(hdc, rc_client.right, rc_client.top);
		LineTo(hdc, rc_client.right, rc_client.bottom);
		LineTo(hdc, rc_client.left, rc_client.bottom);
		LineTo(hdc, rc_client.left, rc_client.top);	
	}
	
	/* 根据录像类型颜色、区间画出录像片段 */	
//	if(dwStyle & RIBS_BORDER)
	{
		x = rc_client.left+RIB_BORDER;
		y = rc_client.top+RIB_BORDER;
		w = RECTW(rc_client)-(RIB_BORDER<<1);
		h = RECTH(rc_client)- (RIB_BORDER<<1);	
	}
/*	
	else
	{
		x = rc_client.left;
		y = rc_client.top;
		w = RECTW(rc_client);
		h = RECTH(rc_client);	
	}
*/

	for (i = 0; i < MAX_INTRL_NUM; ++i)
	{
		pIntval = & (pData->interval[i]);
		if(pIntval->valid 
		   && RecinfoBarCheckType(hwnd, pIntval->nType, pData))
		{
			if(pIntval->nEnd <= pData->nCurMin 
			   || pIntval->nStart >= pData->nCurMax)
			{
				continue;
			}
			
			nFixStart 
				= (pIntval->nStart > pData->nCurMin) ? pIntval->nStart : pData->nCurMin;
			nFixEnd 
				= (pIntval->nEnd < pData->nCurMax) ? pIntval->nEnd : pData->nCurMax;
			rc_area.left = x + (w*(nFixStart-pData->nCurMin)) /(float)(pData->nCurMax - pData->nCurMin);
			rc_area.top = y;
			rc_area.right = x + (w*(nFixEnd-pData->nCurMin))/(float)(pData->nCurMax - pData->nCurMin);
			rc_area.bottom = y+h;

			int nColorIndex = RetriveRecTypeColor(pIntval->nType, pData->dwStatus);
			if(REC_COLOR_NONE == nColorIndex)
				continue;
			pColor = &(pData->atColorRec[nColorIndex]);			
			SetBrushColor(hdc, RGB2Pixel(hdc, pColor->u8_red, pColor->u8_green, pColor->u8_blue));
			FillBox(hdc, rc_area.left, rc_area.top, RECTW(rc_area), RECTH(rc_area));		
		}
	}

	/* 画游标指示的位置 */
	nPosX = x + (w*(pData->nPos - pData->nCurMin))/(float)(pData->nCurMax - pData->nCurMin);
	SetPenColor(hdc, pData->nColorInd);
	MoveTo(hdc, nPosX, rc_client.top-1);
	LineTo(hdc, nPosX, rc_client.bottom+1);

	/* 游标时间提示 */
	SetRect(&rc_text, nPosX, 2, 80, RECTW(rc_client));
	sprintf(szTime, " %02d:%02d:%02d", (pData->nPos)/(60*60), ((pData->nPos)/60)%60, (pData->nPos)%60);

	if(dwStyle & RIBS_TIP)
	{
		SetBkMode (hdc, BM_TRANSPARENT);
		SetTextColor(hdc, pData->nColorInd);
		if((rc_client.right - nPosX) >= 55)
		{
			TextOut(hdc, nPosX, 2, szTime);		
		}
		else
		{
			TextOut(hdc, rc_client.right-55, 2, szTime);
		}
/*		DrawText(hdc, szTime, -1, rc_text, DT_TOP);*/
	}

}

static void RefreshTimeLine(HWND hwnd, RECINFOBARDATA * pData, DWORD dwStyle, int lastPos)
{
	T_COLOR *pColor = NULL;
	RECT rc_client, rc_area, rc_text;
	int  i, x, y, w, h;
	int nFixStart = 0, nFixEnd = 0;
	int nPosX,lastNPosX;
	INTERVAL *pIntval = NULL;
	char szTime[32] = {'\0'};
	int startX = 0, endX = 0, width = 0;
	BOOL fGetDC = FALSE; 
	
	HDC hdc = GetClientDC(hwnd);
	fGetDC = TRUE; 

	GetClientRect (hwnd, &rc_client);

	/* 根据录像类型颜色、区间画出录像片段 */	
//	if(dwStyle & RIBS_BORDER)
	{
		x = rc_client.left+RIB_BORDER;
		y = rc_client.top+RIB_BORDER;
		w = RECTW(rc_client)-(RIB_BORDER<<1);
		h = RECTH(rc_client)- (RIB_BORDER<<1);	
	}
/*	
	else
	{
		x = rc_client.left;
		y = rc_client.top;
		w = RECTW(rc_client);
		h = RECTH(rc_client);	
	}
*/
	nPosX = x + (w*(pData->nPos - pData->nCurMin))/(float)(pData->nCurMax - pData->nCurMin);
	lastNPosX = x + (w*(lastPos - pData->nCurMin))/(float)(pData->nCurMax - pData->nCurMin);
	startX = nPosX >= lastNPosX ? lastNPosX : nPosX;
	endX = nPosX >= lastNPosX ? nPosX : lastNPosX;
	width = nPosX >= lastNPosX ? (nPosX-lastNPosX) : (lastNPosX-nPosX);
	
	SetBrushColor(hdc, RGB2Pixel(HDC_SCREEN, 0x05, 0x05, 0x05));
	if(dwStyle & RIBS_TIP) /*只刷新时间轴附近的背景*/
		FillBox(hdc, startX-2, rc_client.top, width+50+5, RECTH(rc_client));
	else
		FillBox(hdc, startX-2, rc_client.top, width+5, RECTH(rc_client));
	
	/* draw the border of this control by the style */
	if(dwStyle & RIBS_BORDER)
	{
		SetPenColor(hdc, pData->nColorBorder);
		SetPenWidth(hdc, RIB_BORDER);
		MoveTo(hdc, rc_client.left, rc_client.top);
		LineTo(hdc, rc_client.right, rc_client.top);
		LineTo(hdc, rc_client.right, rc_client.bottom);
		LineTo(hdc, rc_client.left, rc_client.bottom);
		LineTo(hdc, rc_client.left, rc_client.top);	
	}
	
	for (i = 0; i < MAX_INTRL_NUM; ++i)
	{
		pIntval = & (pData->interval[i]);
		if(pIntval->valid 
		   && RecinfoBarCheckType(hwnd, pIntval->nType, pData))
		{
			if(pIntval->nEnd <= pData->nCurMin 
			   || pIntval->nStart >= pData->nCurMax)
			{
				continue;
			}
			
			nFixStart 
				= (pIntval->nStart > pData->nCurMin) ? pIntval->nStart : pData->nCurMin;
			nFixEnd 
				= (pIntval->nEnd < pData->nCurMax) ? pIntval->nEnd : pData->nCurMax;
				
			rc_area.left = x + (w*(nFixStart-pData->nCurMin)) /(float)(pData->nCurMax - pData->nCurMin);
			rc_area.top = y;
			rc_area.right = x + (w*(nFixEnd-pData->nCurMin))/(float)(pData->nCurMax - pData->nCurMin);
			rc_area.bottom = y+h;
#if 0
			if(dwStyle & RIBS_TIP) /*只刷新时间轴附近的背景*/
			{
				if((startX > rc_area.right) || ((endX+100) < rc_area.left))  
					continue;
			}
			else
			{
				if(startX > rc_area.right || endX < rc_area.left)
					continue;
			}

#endif			
			int nColorIndex = RetriveRecTypeColor(pIntval->nType, pData->dwStatus);
			//if(REC_COLOR_NONE == nColorIndex)
			//	continue;
			pColor = &(pData->atColorRec[nColorIndex]);			
			SetBrushColor(hdc, RGB2Pixel(hdc, pColor->u8_red, pColor->u8_green, pColor->u8_blue));
			FillBox(hdc, rc_area.left, rc_area.top, RECTW(rc_area), RECTH(rc_area));		
		}
	}

	/* 画游标指示的位置 */
	SetPenColor(hdc, pData->nColorInd);
	MoveTo(hdc, nPosX, rc_client.top-1);
	LineTo(hdc, nPosX, rc_client.bottom+1);

	/* 游标时间提示 */
	SetRect(&rc_text, nPosX, 2, 80, RECTW(rc_client));
	sprintf(szTime, " %02d:%02d:%02d", (pData->nPos)/(60*60), ((pData->nPos)/60)%60, (pData->nPos)%60);

	if(dwStyle & RIBS_TIP)
	{
		SetBkMode (hdc, BM_TRANSPARENT);
		SetTextColor(hdc, pData->nColorInd);
		if((rc_client.right - nPosX) >= 55)
		{
			TextOut(hdc, nPosX, 2, szTime);		
		}
		else
		{
			TextOut(hdc, rc_client.right-55, 2, szTime);
		}
/*		DrawText(hdc, szTime, -1, rc_text, DT_TOP);*/
	}
	if(fGetDC)
		ReleaseDC(hdc);

}


static int NormalizeMousePos(HWND hwnd, RECINFOBARDATA* pData, int mousepos)
{
	RECT rcClient;
	int w, h;
	int len, pos;
	int nRetpos = 0;
	//RECT rcWindow;
	/*DWORD dwStyle = */GetWindowStyle(hwnd);
	GetClientRect (hwnd, &rcClient);

	int blank = 0;
//	if(dwStyle & RIBS_BORDER)
	{
		len = RECTW (rcClient) - (RIB_BORDER<<1);			
		w = RECTW(rcClient)-(RIB_BORDER<<1);
		h = RECTH(rcClient)- (RIB_BORDER<<1);
		(void)h;
	}
/*
	else
	{
		len = RECTW (rcClient);	
	}	
*/    
    if (mousepos < blank)
        pos = 0;
    else if (mousepos > rcClient.left + w + blank)
        pos = len ;
    else
        pos = mousepos - blank; 
	
	nRetpos = (pData->nCurMax - pData->nCurMin) * pos / (float)len + pData->nCurMin;
	
	return nRetpos;
}

#if 0
static void GetIndicatorRects(RECINFOBARDATA * pData, DWORD dwStyle, RECT *rc_client, RECT *rc_indicator)
{
    int x, y, w, h;
    int pos, min, max;
	int indx, indy, indw, indh;

	x = rc_client->left;
    y = rc_client->top;
    w = RECTWP (rc_client);
    h = RECTHP (rc_client);
    pos = pData->nPos;
    max = pData->nMax;
    min = pData->nMin;

    if (dwStyle & RIBS_BORDER) {
	    x += RIB_BORDER;
	    y += RIB_BORDER;
	    w -= RIB_BORDER << 1;
	    h -= RIB_BORDER << 1;
    }

	indx = x + (int)(pos - min) * w / (float)(max - min);
	indy = y;
	indw = 0;
	indh = h;

	SetRect(rc_indicator, indx, indy, indx+indw, indy+indh);
}
#endif
static void SetIndicatorPos(HWND hwnd, int new_pos)
{
   RECINFOBARDATA * pData = (RECINFOBARDATA *)GetWindowAdditionalData2(hwnd);

	int nCurIntrvl = pData->nCurMax - pData->nCurMin;
	int nOffset = 0;
	
	if (new_pos < pData->nMin)
	{
		new_pos = pData->nMin;
	}
	else if (new_pos >= pData->nMin
			 && new_pos < pData->nCurMin)
	{
		nOffset = pData->nPos - new_pos;	
		pData->nCurMin = ((pData->nCurMin - pData->nMin) > nOffset) ?
						 (pData->nCurMin - nOffset) : pData->nMin;
		pData->nCurMax = pData->nCurMin + nCurIntrvl;

		NotifyParentEx(hwnd, GetDlgCtrlID(hwnd), 
					   RIBN_CUR_MIN_CHANGE, (DWORD) pData->nCurMin);
		NotifyParentEx(hwnd, GetDlgCtrlID(hwnd), 
					   RIBN_CUR_MAX_CHANGE, (DWORD) pData->nCurMax);	
	}
	else if (new_pos > pData->nCurMax 
			 && new_pos <= pData->nMax)
	{
		nOffset = new_pos - pData->nPos;						 	
		pData->nCurMax = ((pData->nMax - pData->nCurMax) > nOffset) ?
						  (pData->nCurMax + nOffset) : pData->nMax;
		pData->nCurMin = pData->nCurMax - nCurIntrvl;

		NotifyParentEx(hwnd, GetDlgCtrlID(hwnd), 
					   RIBN_CUR_MIN_CHANGE, (DWORD) pData->nCurMin);
		NotifyParentEx(hwnd, GetDlgCtrlID(hwnd), 
					   RIBN_CUR_MAX_CHANGE, (DWORD) pData->nCurMax);

	}
	else if (new_pos > pData->nMax)
	{
		new_pos = pData->nMax;
	}
	
	/* 用户操作几乎不会出现 */
	if(pData->nPos == new_pos)
		return;

	pData->nPos = new_pos;	
}

static int GetCurMaxMinIntval(HWND hWnd)
{
	int nRet = 0;
	RECINFOBARDATA * pData = (RECINFOBARDATA *)GetWindowAdditionalData2(hWnd);
	switch(pData->ucZoomout)
	{
		case ZOOM_OUT_X0:
		{
			nRet = pData->nMax - pData->nMin;
			break;
		}

		case ZOOM_OUT_X1_3:
		{
			nRet = (pData->nMax - pData->nMin) / 3.0;
			break;
		}

		case ZOOM_OUT_X1_6:
		{
			nRet = (pData->nMax - pData->nMin) / 6.0;
			break;
		}

		case ZOOM_OUT_X1_12:
		{
			nRet = (pData->nMax - pData->nMin) / 12.0;
			break;
		}

		case ZOOM_OUT_X1_24:
		{
			nRet = (pData->nMax - pData->nMin) / 24.0;
			break;
		}

		case ZOOM_OUT_X1_48:
		{
			nRet = (pData->nMax - pData->nMin) / 48.0;
			break;
		}

		default:
		{
			nRet = pData->nMax - pData->nMin;
		}
	}

	return nRet;
}

static int SetCurrentMaxMin(HWND hWnd, int nCurMin, int nCurMax)
{
   	RECINFOBARDATA * pData = (RECINFOBARDATA *)GetWindowAdditionalData2(hWnd);
   	
	if(nCurMin < pData->nMin 
	   || nCurMax > pData->nMax
	   || nCurMax <= nCurMin
	   || (nCurMax - nCurMin) != GetCurMaxMinIntval(hWnd))
	{
		return -1;
	}
	
	pData->nCurMin = nCurMin;
	pData->nCurMax = nCurMax;
	
	if(pData->nPos > pData->nCurMax)
	{
		pData->nPos = pData->nCurMax;
	}
	else if(pData->nPos < pData->nCurMin)
	{
		pData->nPos = pData->nCurMin;
	}	
	
	return 0;
}
static int SetCurrentMaxMinOffset(HWND hWnd, int new_din)
{
	//int nDPos = 0;
	int nFixdin= 0;//, nCurIntval = 0;
   	RECINFOBARDATA * pData = (RECINFOBARDATA *)GetWindowAdditionalData2(hWnd);

	/*nCurIntval = */GetCurMaxMinIntval(hWnd);
	nFixdin = new_din;
	if(new_din >=0 
	   && ((pData->nCurMax+new_din) >= pData->nMax))
	{
		nFixdin = pData->nMax - pData->nCurMax;
	}
	else if(new_din < 0
			&& ((pData->nCurMin+new_din) <= pData->nMin))
	{
		nFixdin = pData->nMin - pData->nCurMin;
	}

	pData->nCurMax += nFixdin;
	pData->nCurMin += nFixdin;
	if(pData->nPos > pData->nCurMax)
	{
		pData->nPos = pData->nCurMax;
		NotifyParentEx(hWnd, GetDlgCtrlID(hWnd), 
					   RIBN_INDCHANGE, (DWORD) pData->nPos);
	}
	else if(pData->nPos < pData->nCurMin)
	{
		pData->nPos = pData->nCurMin;
		NotifyParentEx(hWnd, GetDlgCtrlID(hWnd), 
					   RIBN_INDCHANGE, (DWORD) pData->nPos);		
	}

	return 0;
}
static int CheckIntervalValid(const RECINFOBARDATA * pRecData, int rec_type, int low_limit, int hi_limit)
{
	//int i = 0;

	if((low_limit > hi_limit)
	   || (rec_type < 1)
	   || (rec_type > REC_TYPE_MASK))
		return 0;

	/* ****** 重叠区间属于合法 以下部分先屏蔽掉 ********* */
#if 0
	for (i = 0; i < MAX_INTRL_NUM; i++)
	{
		if(pRecData->interval[i].valid
		   &&((pRecData->interval[i].nStart <= low_limit && low_limit < pRecData->interval[i].nEnd)
		   	   || (pRecData->interval[i].nStart < hi_limit && hi_limit <= pRecData->interval[i].nEnd)
		   	   || (pRecData->interval[i].nStart > low_limit && hi_limit > pRecData->interval[i].nEnd)))
		{
			return 0;
		}
	}
#endif

	return 1;
}


static int RecinfoBarProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
	int i = 0;
	RECINFOBARDATA * pData = NULL;
#ifdef HICHIP_HI3521
	T_COLOR atColor[] =
	{
		{0x45, 0x45, 0x45, 0xFF},	//NONE 没有用到
		{0x37, 0x41, 0x88, 0xFF},	//MANUAL 黄颜色
		{0xcf, 0x2d, 0x24, 0xFF},	//ALARM 浅红色
		{0xA9, 0x50, 0xb7, 0xFF},	//MOTION 紫色
		{0x40, 0x00, 0x00, 0xFF},	//LOST	没有用到
		{0x45, 0x45, 0x45, 0xFF},	//H	没有用到
		{0x56, 0xaf, 0x48, 0xFF}	//TIMER 绿色
	};
#else	
	T_COLOR atColor[] =
	{
		{0x45, 0x45, 0x45, 0x00},	//NONE 没有用到
		{0x37, 0x41, 0x88, 0x00},	//MANUAL 黄颜色
		{0xcf, 0x2d, 0x24, 0x00},	//ALARM 浅红色
		{0xA9, 0x50, 0xb7, 0x00},	//MOTION 紫色
		{0x40, 0x00, 0x00, 0x00},	//LOST	没有用到
		{0x45, 0x45, 0x45, 0x00},	//H 没有用到
		{0x56, 0xaf, 0x48, 0x00},	//TIMER 绿色
		{0x00, 0x00, 0xff, 0x00}	//STARTUP 蓝色

	};
#endif
	
	if(MSG_CREATE != message)
		pData = (RECINFOBARDATA *)GetWindowAdditionalData2(hwnd);
		
	switch(message)
	{
		case MSG_CREATE:
			{
				if (!(pData = (RECINFOBARDATA *)malloc (sizeof (RECINFOBARDATA)))) 
				{
                	fprintf(stderr, "Create recinfobar control failure!\n");
                	return -1;
            	}

				pData->nMin = INTRL_MIN;
				pData->nMax = INTRL_MAX;
				pData->nCurMin = pData->nMin;
				pData->nCurMax = pData->nMax;
				pData->nPos =  0;
				pData->nColorBorder = RGB2Pixel(HDC_SCREEN, 0x06, 0x72, 0xBA);//PIXEL_lightgray;
				
				pData->nColorBkgnd = RGB2Pixel(HDC_SCREEN, 0x02, 0x00, 0x00);
				pData->nColorInd = RGB2Pixel(HDC_SCREEN, 0xff, 0xff, 0xff);				

				pData->nMousePos = 0;
				pData->dwStatus = 0;
				pData->ucZoomout = ZOOM_OUT_X0;
				memmove(pData->atColorRec, atColor, sizeof(atColor));
				memset(pData->interval, 0, (MAX_INTRL_NUM+1)*sizeof(INTERVAL));

				
				SetWindowFont(hwnd, GetSystemFont(SYSLOGFONT_FIXED));
				SetWindowAdditionalData2(hwnd, (DWORD)pData);
			}
			break;

		case MSG_DESTROY:
			{
				free(pData);
			}
			break;
		case MSG_ERASEBKGND:
			{
			    HDC hdc = (HDC)wParam; 
			    BOOL fGetDC = FALSE; 
			    if (hdc == 0) { 
			        hdc = GetClientDC (hwnd); 
			        fGetDC = TRUE; 
			    }
				RecinfoBarOnDraw(hwnd, hdc, pData, GetWindowStyle(hwnd));
			    if (fGetDC) 
			        ReleaseDC (hdc);
				return 0;
			}
		case MSG_NCPAINT:
			return 0;
		
		case MSG_PAINT:
			{
#if 1
				HDC hdc;

				hdc = BeginPaint(hwnd);
				/* add your code here */
				RecinfoBarOnDraw(hwnd, hdc, pData, GetWindowStyle(hwnd));
				EndPaint(hwnd, hdc);
#endif
				//return 0;
			}
			break;
			
		case MSG_LBUTTONDOWN:
			{
				if (GetCapture() != hwnd) 
				{					
					int mouseX = LOSWORD(lParam);				
					SetCapture (hwnd);					
					pData->dwStatus |= RIBST_DRAGGED;
					NotifyParent (hwnd, GetDlgCtrlID(hwnd), RIBN_STARTDRAG);			

					pData->nMousePos = mouseX;
				}

				return 0;
			}
			break;

		case MSG_MOUSEMOVE:
			{
				int mouseX = LOSWORD(lParam);
	            int mouseY = HISWORD(lParam);

				if(GetCapture() == hwnd)
				{
					ScreenToClient (hwnd, &mouseX, &mouseY);
					

					if(pData->dwStatus & RIBST_DRAGGED)
					{
						int dx = NormalizeMousePos(hwnd, pData, pData->nMousePos) - NormalizeMousePos(hwnd, pData, mouseX);
						SetCurrentMaxMinOffset(hwnd, dx);
						pData->nMousePos = mouseX;
						InvalidateRect(hwnd, NULL, TRUE);

						NotifyParentEx(hwnd, GetDlgCtrlID(hwnd), 
									   RIBN_CUR_MIN_CHANGE, (DWORD) pData->nCurMin);
						NotifyParentEx(hwnd, GetDlgCtrlID(hwnd), 
									   RIBN_CUR_MAX_CHANGE, (DWORD) pData->nCurMax);
					}
				}
				else
					break;
			}
			break;

		case MSG_LBUTTONUP:
			{
				if (GetCapture() == hwnd)
				{
					pData->dwStatus &= ~RIBST_DRAGGED;
					pData->nMousePos = 0;
					ReleaseCapture ();
					NotifyParent (hwnd, GetDlgCtrlID(hwnd), RIBN_STOPDRAG);
				}

				return 0;
			}
			break;
			
		case RIBM_ZOOMOUT:
			{
				RecinfoBarSetZoomout(hwnd, pData, (unsigned char) wParam);				
				InvalidateRect(hwnd, NULL, TRUE);
				
				NotifyParentEx(hwnd, GetDlgCtrlID(hwnd), 
							   RIBN_CUR_MIN_CHANGE, (DWORD) pData->nCurMin);
				NotifyParentEx(hwnd, GetDlgCtrlID(hwnd), 
							   RIBN_CUR_MAX_CHANGE, (DWORD) pData->nCurMax);

				return 0;
			}
			//break;
			
		case MSG_LBUTTONDBLCLK:
			{
				//if (GetCapture() == hwnd)
				{
					int mouseX = LOSWORD(lParam);		
					SetIndicatorPos(hwnd, NormalizeMousePos(hwnd, pData, mouseX));
					InvalidateRect(hwnd, NULL, TRUE);
					NotifyParentEx(hwnd, GetDlgCtrlID(hwnd), RIBN_INDCHANGE, (DWORD) pData->nPos);
					NotifyParent(hwnd, GetDlgCtrlID(hwnd), RIBN_LBUTTONDBLCLK);					
				}	
			}
			break;
			
		case RIBM_SETINTRL:
			{	
				if(0 == wParam)
					return -1;
					
				RECINTERVAL *pIntrval = (RECINTERVAL *) wParam;

				if(CheckIntervalValid(pData, pIntrval->nRecType, pIntrval->nStart, pIntrval->nEnd))
				{
					/* *** 修正区间起始和结束值 **** */
					if(pIntrval->nStart < pData->nMin)
						pIntrval->nStart = pData->nMin;
					if(pIntrval->nEnd > pData->nMax)
						pIntrval->nEnd = pData->nMax;
						
					for (i = 0; i < MAX_INTRL_NUM; i++)
					{
						if (!pData->interval[i].valid)
						{
							pData->interval[i].valid = (unsigned char)TRUE;
							pData->interval[i].nStart = pIntrval->nStart;
							pData->interval[i].nEnd = pIntrval->nEnd;
							pData->interval[i].nType = pIntrval->nRecType;
							InvalidateRect(hwnd, NULL, FALSE);
							
							return 0;
						}
					}

					return -1;
				}
				else
				{
					return -1;
				}				
			}
			break;
			
		case RIBM_SHOWINDC:
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
			
		case RIBM_SETINDCPOS:
			{
				int new_pos = (int) wParam;
				int lastPos = pData->nPos;	
				SetIndicatorPos(hwnd, new_pos);
				DWORD dwStyle = GetWindowStyle(hwnd);
				
				RefreshTimeLine(hwnd, pData, dwStyle, lastPos);
				//InvalidateRect(hwnd, NULL, FALSE);

				return 0;
			}
			break;

		case RIBM_CLEAR:
			{ 
				//pData->nPos = 0;
				memset(&(pData->interval[0]), 0, MAX_INTRL_NUM*sizeof(INTERVAL));				
				InvalidateRect(hwnd, NULL, FALSE);
			}
			break;

		case RIBM_GETINDCPOS:
			{
				int *pos = (int *)lParam;
				if(NULL == pos)
					return 1;
				
				*pos = pData->nPos;
				return 0;
			}
//			break;

		case RIBM_GETRECINTRL:
			{
				RECINTERVAL *pIntrvl = (RECINTERVAL *)lParam;
				if(NULL == pIntrvl)
					return 1;
					
				for (i = 0; i < MAX_INTRL_NUM; ++i)
				{
					if (pData->interval[i].valid 
						&& RecinfoBarCheckType(hwnd, pData->interval[i].nType, pData))
					{
						if(((pData->interval[i].nStart <= pData->nPos)
							 && (pData->interval[i].nEnd > pData->nPos))
						   || (!pData->interval[i+1].valid
						   	    && (pData->interval[i].nEnd == pData->nPos)))
						{
							pIntrvl->unValid = pData->interval[i].valid;
							pIntrvl->nRecType = pData->interval[i].nType;
							pIntrvl->nStart = pData->interval[i].nStart;
							pIntrvl->nEnd = pData->interval[i].nEnd;

							return 0;
						}						
					}
		
				}

				return 1;
			}
//			break;

		case RIBM_GETRECINTRL_BY_INDEX:
			{
				int index = (int)wParam;
				RECINTERVAL *pIntrvl = (RECINTERVAL *)lParam;
				printf("********* index = %d, valide = %d, type = %x, start = %d, end = %d\n",
						index, pData->interval[index].valid, pData->interval[index].nType, 
						pData->interval[index].nStart
,  pData->interval[index].nEnd);
				if(NULL == pIntrvl 
				  || index < 0 
				  || index >= MAX_INTRL_NUM
				  || !RecinfoBarCheckType(hwnd, pData->interval[index].nType, pData))
					return 1;

				pIntrvl->unValid = pData->interval[index].valid;
				pIntrvl->nRecType = pData->interval[index].nType;
				pIntrvl->nStart = pData->interval[index].nStart;
				pIntrvl->nEnd = pData->interval[index].nEnd;

				return 0;
			}
			//break;
			
		case RIBM_GETIND_INTRL_INDEX:
			{
				int nIndex = 0, nLeftMin = 0, nRighMin = 0, nTemp = 0;
				int *pIndexL = (int *)wParam;
				int *pIndexR = (int *)lParam;
				if (NULL == pIndexL || NULL == pIndexR)
					return -2;
					
				for (i = 0; i < MAX_INTRL_NUM; ++i)
				{
					if (pData->interval[i].valid 
						&& RecinfoBarCheckType(hwnd, pData->interval[i].nType, pData))
					{				
						if(((pData->interval[i].nStart <= pData->nPos)
							 && (pData->interval[i].nEnd > pData->nPos))
						  /* || (!pData->interval[i+1].valid
						   	    && (pData->interval[i].nEnd == pData->nPos))*/)
						{
							nIndex = i;

							break;
						}
					}
				}

				/* 没有找到，则寻找离游标左边最近的区间上限和游标右边最近的区间下限 */
				if (i == MAX_INTRL_NUM)
				{
					nIndex = -1;
					*pIndexL =  -1;
					*pIndexR = -1;
					nLeftMin = pData->nMax - pData->nMin;
					nRighMin = nLeftMin;
					
					for (i = 0; i < MAX_INTRL_NUM; ++i)
					{
						if (pData->interval[i].valid 
							&& RecinfoBarCheckType(hwnd, pData->interval[i].nType, pData))
						{
							nTemp = pData->nPos - pData->interval[i].nEnd;
							if((nTemp >= 0)
							   && (nTemp < nLeftMin))
							{
								nLeftMin = nTemp;
								*pIndexL = i;
							}

							nTemp = pData->interval[i].nStart - pData->nPos;
							if((nTemp > 0)
								&& (nTemp < nRighMin))
							{
								nRighMin = nTemp;
								*pIndexR = i;
							}
						}
					}
				}

				return nIndex;			
			}
			//break;


		case RIBM_SET_ZOOMOUT_PARAM:
			{
				RecinfoBarSetZoomout(hwnd, pData, (unsigned char) wParam);
				InvalidateRect(hwnd, NULL, TRUE);

				return 0;
			}
			//break;

		case RIBM_GET_CUR_MIN:
			{
				int *pValue = (int *)lParam;
				if(NULL == pValue)
				{
					return -1;
				}
				else
				{
					*pValue = pData->nCurMin;
					return 0;
				}
			}
			//break;

		case RIBM_GET_CUR_MAX:
			{
				int *pValue = (int *)lParam;
				if(NULL == pValue)
				{
					return -1;
				}
				else
				{
					*pValue = pData->nCurMax;
					return 0;
				}			
			}
			//break;
			
		case RIBM_SET_CUR_MINMAX:
			{
				int nRet = 
					SetCurrentMaxMin(hwnd, (int)wParam, (int) lParam);
				if(0 == nRet)
				{
					InvalidateRect(hwnd, NULL, TRUE);
					return 0;
				}	
				else
				{
					return -1;
				}
				
			}
			//break;

		case RIBM_SET_STAT_TYPE:
			{
				int nType = (int) wParam;
				if(nType < REC_MANNUAL || nType > REC_STARTUP)
					return 1;
					
				BOOL bSet = (BOOL) lParam;
				if(bSet)
				{
					pData->dwStatus |= (0x01<<nType);
				}
				else
				{
					pData->dwStatus &= ~(0x01<<nType);
				}

				InvalidateRect(hwnd, NULL, FALSE);
				
				return 0;
			}
			break;
			
		default:
			break;
	}
	
	return DefaultControlProc(hwnd, message, wParam, lParam);
}

#endif /* _CUSTOM_CTRL_RECINFOBAR */
