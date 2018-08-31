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
#include "ctrl_skbkey.h"

#ifdef _CTRL_SKBKEY_H_

#define SKBKST_FOCUSED     			0x00000001L
#define SKBKST_MOUSE_MOVE_ON		0x00000002L
#define SKBKST_CHECKED				0x00000004L
#define SKBKST_PUSHED 				0x00000008L

typedef struct
{
	int nBkColor;
	int nTxtColorNormal;
	int nTxtColorFocus;
	int nFrColorNormal;
	int nFrColorFocus;

	DWORD dwStatus;
}T_SKBKDATA;



BOOL DrawSKBKeyRoundRect(HDC hdc, int x0, int y0, int x1, int y1, int rw, int rh);

static int SKBKeyControlProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam);

BOOL RegisterSKBKeyControl(void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_SKBK_CUSTOM;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = GetWindowElementColor (BKC_CONTROL_DEF);
    WndClass.WinProc     = SKBKeyControlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}


void SKBKeyControlCleanup(void)
{
    // do nothing.
    return;
}

static int SKBKeyControlProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	RECT rcClient;
	DWORD dwStyle;
	PCONTROL pCtrl;
	char acText[16] = {0};
	UINT uFormat = DT_CENTER | DT_VCENTER|DT_SINGLELINE;

	pCtrl   = Control(hwnd);
    dwStyle = pCtrl->dwStyle;
	T_SKBKDATA	*pSkbkdata = NULL;

	if (MSG_CREATE != message)
		pSkbkdata = (T_SKBKDATA *) GetWindowAdditionalData2(hwnd);
	
	switch (message)
	{
		case MSG_CREATE:
			{
				if (!(pSkbkdata = calloc(1, sizeof(T_SKBKDATA))))
				{
					return -1;
				}
				SetWindowFont(hwnd, GetSystemFont(SYSLOGFONT_FIXED));
				pSkbkdata->nBkColor = RGB2Pixel(HDC_SCREEN, 0x1f, 0x1c, 0x31);
				pSkbkdata->nTxtColorNormal = PIXEL_lightwhite;
				pSkbkdata->nTxtColorFocus =  PIXEL_lightwhite;
				pSkbkdata->nFrColorNormal = RGB2Pixel(HDC_SCREEN, 0x04, 0x77, 0xbf);
				pSkbkdata->nFrColorFocus = RGB2Pixel(HDC_SCREEN, 0x5a, 0x3d, 0x85);
				pSkbkdata->dwStatus = 0;

				SetWindowAdditionalData2 (hwnd, (DWORD) pSkbkdata);
			}
			break;
			
		case MSG_DESTROY:
			{
				free(pSkbkdata);
			}
			break;

		case MSG_PAINT :
			{
				hdc = BeginPaint(hwnd);
				
				//SetBkMode(hdc, BM_TRANSPARENT);
				if ((pSkbkdata->dwStatus & SKBKST_FOCUSED)
					|| (pSkbkdata->dwStatus & SKBKST_PUSHED)
					|| (pSkbkdata->dwStatus & SKBKST_CHECKED))
				{					
					SetTextColor(hdc, pSkbkdata->nTxtColorFocus);
					SetPenColor(hdc, pSkbkdata->nFrColorFocus);
				}
				else
				{
					SetTextColor(hdc, pSkbkdata->nTxtColorNormal);
					SetPenColor(hdc, pSkbkdata->nFrColorNormal);
				}

				GetClientRect(hwnd, &rcClient);
				rcClient.left 	+= 2;
				rcClient.top  	+= 2;
				rcClient.right  -= 2;
				rcClient.bottom -= 2;
				
				SetBrushColor(hdc, pSkbkdata->nBkColor);				
				FillBox(hdc, rcClient.left, rcClient.top,
					RECTW(rcClient), RECTH(rcClient));
#if 1
				DrawSKBKeyRoundRect(hdc,rcClient.left, rcClient.top, 
					rcClient.right, rcClient.bottom, 3, 3);
#else
				MoveTo(hdc, rcClient.left, rcClient.top);
				LineTo(hdc, rcClient.right, rcClient.top);
				LineTo(hdc, rcClient.right, rcClient.bottom);
				LineTo(hdc, rcClient.left, rcClient.bottom);
				LineTo(hdc, rcClient.left, rcClient.top);
#endif
				SetBkMode(hdc, BM_TRANSPARENT);
				GetWindowText(hwnd, acText, 15);
				
				if (pSkbkdata->dwStatus & SKBKST_PUSHED)
				{
					int l, t, r, b;
					l = rcClient.left;
					t = rcClient.top+2;
					r= rcClient.right;
					b = rcClient.bottom;
					SetRect(&rcClient,l, t, r, b);
				}
				DrawText(hdc, acText, -1, &rcClient, uFormat);				
				EndPaint(hwnd, hdc);
			}
			return 0;
		
		case MSG_LBUTTONDBLCLK:
		case MSG_LBUTTONDOWN:
			{
				if (GetCapture () == hwnd)
                	break;
            	SetCapture (hwnd);

            	pSkbkdata->dwStatus |= SKBKST_PUSHED;
				if (SKBKS_CHECKED == (dwStyle & SKBKS_TYPEMASK))
				{
					if (pSkbkdata->dwStatus & SKBKST_CHECKED)
						pSkbkdata->dwStatus &= ~SKBKST_CHECKED;
					else
						pSkbkdata->dwStatus |= SKBKST_CHECKED;

				}
				
				InvalidateRect(hwnd, NULL, FALSE);
				NotifyParent(hwnd, GetDlgCtrlID(hwnd), SKBKN_CLICKED);
				NotifyParent(hwnd, GetDlgCtrlID(hwnd), SKBKN_PUSHED);
				
			}
			break;

		case MSG_LBUTTONUP:
			{
            	int x, y;				
	            if (GetCapture() != hwnd)
	                break;	            
	            ReleaseCapture ();	 
	            
				x = LOSWORD(lParam);
            	y = HISWORD(lParam);
            	ScreenToClient (GetParent (hwnd), &x, &y);
            	pSkbkdata->dwStatus &= ~SKBKST_PUSHED;
				
            	InvalidateRect(hwnd, NULL, FALSE);

				NotifyParent(hwnd, GetDlgCtrlID(hwnd), SKBKN_UNPUSHED);            	
			}
			break;

		// 该消息只能用于面板按钮按下的情况
		case SKBKM_PUSHDOWN:
			{
            	pSkbkdata->dwStatus |= SKBKST_PUSHED;
            	InvalidateRect(hwnd, NULL, FALSE);
            	
 				NotifyParent(hwnd, GetDlgCtrlID(hwnd), SKBKN_PUSHED);
			}
			return 0;

		// 该消息只能用于面板按钮弹起的情况
		case SKBKM_PUSHUP:
			{
            	pSkbkdata->dwStatus &= ~SKBKST_PUSHED;
            	InvalidateRect(hwnd, NULL, FALSE);

				NotifyParent(hwnd, GetDlgCtrlID(hwnd), SKBKN_UNPUSHED);            	
			}
			return 0;
			
		case MSG_MOUSEMOVEIN :
			{
				if ((BOOL)wParam)
					pSkbkdata->dwStatus |= SKBKST_FOCUSED;
				else
					pSkbkdata->dwStatus &= ~SKBKST_FOCUSED;

				InvalidateRect(hwnd, NULL, FALSE);
			}
			return 0;

		case MSG_SETTEXT :
			{
	            SetWindowCaption (hwnd, (char*)lParam);
	            InvalidateRect (hwnd, NULL, FALSE);
			}
			break;
			
		case SKBKM_SETCHECKED:
			{
				if (SKBKS_CHECKED == (dwStyle & SKBKS_TYPEMASK))
				{
					if (wParam)
						pSkbkdata->dwStatus |= SKBKST_CHECKED;
					else
						pSkbkdata->dwStatus &= ~SKBKST_CHECKED;

					InvalidateRect(hwnd, NULL, FALSE);
				}
			}
			return 0;
			
		default :
			break;
	}
	
	return DefaultControlProc(hwnd, message, wParam, lParam);
}

BOOL DrawSKBKeyRoundRect(HDC hdc, int x0, int y0, int x1, int y1, int rw, int rh)
{
    int cw, ch, a;

    if (x0 >= x1 || y0 >= y1)
        return FALSE;
    
    cw = rw*2;
    ch = rh*2;
   
    if (cw > (x1-x0) || ch > (y1-y0) || cw < 0 || ch < 0)
        return FALSE;
    
    if (rw == 0 || rh == 0) {
        POINT pts [5];
        pts[0].x = x0; pts[0].y = y0;
        pts[1].x = x1; pts[1].y = y0;
        pts[2].x = x1; pts[2].y = y1;
        pts[3].x = x0; pts[3].y = y1;
        pts[4].x = x0; pts[4].y = y0;

        //FillBox (hdc, x0, y0, (x1-x0), (y1-y0));
        PolyLineEx(hdc, pts, 5);
        return TRUE;
    }

    a = 90*64;
    
#if 0
    FillArcEx (hdc, x0, y0, cw, ch, a, a);
    FillArcEx (hdc, x1-cw, y0, cw, ch, 0, a);
    FillArcEx (hdc, x1-cw, y1-ch, cw, ch, -a, a);
    FillArcEx (hdc, x0, y1-ch, cw, ch, -a, -a);
    
    FillBox (hdc,x0 + rw, y0, (x1-x0)-cw, rh);
    FillBox (hdc, x0, y0 + rh, x1-x0, (y1-y0)-ch);
    FillBox (hdc,x0 + rw, y1-rh, (x1-x0)-cw, rh);
#endif

    ArcEx (hdc, x0, y0, cw, ch, a, a);
    ArcEx (hdc, x1-cw, y0, cw, ch, 0, a);
    ArcEx (hdc, x1-cw, y1-ch, cw, ch, -a, a);
    ArcEx (hdc, x0, y1-ch, cw, ch, -a, -a);

    LineEx (hdc, x0 + rw, y0, x1 - rw, y0);
    LineEx (hdc, x0 + rw, y1, x1 - rw, y1);
    LineEx (hdc, x0, y0 + rh, x0, y1 - rh);
    LineEx (hdc, x1, y0 + rh, x1, y1 - rh);

    return TRUE;
}

#endif
