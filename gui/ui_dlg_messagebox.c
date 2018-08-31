#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>
#include <minigui/skin.h>

#include "ui_config.h"
#include "myedit.h"

#include "gui_core.h"
#include "ui_dlg_head.h"


#ifdef _TINY_SCREEN

#define MB_MARGIN    2
#define MB_BUTTONW   40
#define MB_BUTTONH   20
#define MB_TEXTW     120

#else

#define MB_MARGIN    10
#define MB_BUTTONW   55//80
#define MB_BUTTONH   25//26
#define MB_TEXTW     300

#endif

#define MESSAGEBOX_BKCOLOR   0XFF283C5A

static char captionData[32] = {0};

static DLGTEMPLATE MsgBoxData = 
{
    WS_ABSSCRPOS |/* WS_CAPTION | */WS_BORDER, 
    WS_EX_NONE, 0, 0, 0, 0, NULL, 0, 0, 0, NULL, 0L
};

static RECT MessageTileRect = {0,0,80,30};

static int MsgBoxProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{

    switch (message) {
    case MSG_INITDIALOG:
    {
/*		  HWND hFocus = GetDlgDefPushButton (hWnd);
		   if (hFocus)
		       SetFocus (hFocus);
        SetWindowAdditionalData (hWnd, (DWORD)lParam);
       return 0;
*/  	
	push_open_dlg(&headOpenDlg, hWnd);
	SetWindowBkColor(hWnd, MESSAGEBOX_BKCOLOR);
	break;
    }

	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hWnd);	
        MessageTileRect.right = MsgBoxData.w;
        Gui_WriteTitleName(hdc, &MessageTileRect, captionData);
        EndPaint (hWnd, hdc);		
        break;
	}

	case MSG_RBUTTONUP:
    {
		//PostMessage(hWnd, MSG_CLOSE, 0, 0);	
        break;
    }
    
    case MSG_COMMAND:
    {
        switch (wParam) {
        case IDOK:
        case IDCANCEL:
        case IDABORT:
        case IDRETRY:
        case IDIGNORE:
        case IDYES:
        case IDNO:
            if (GetDlgItem (hWnd, wParam))
                EndDialog (hWnd, wParam);
			
            break;
        }
        break;
    }

    case MSG_CHAR:
    {
        int id = 0;
        
        if (HIBYTE (wParam))
            break;
        switch (LOBYTE (wParam)) {
        case 'Y':
        case 'y':
            id = IDYES;
            break;
        case 'N':
        case 'n':
            id = IDNO;
            break;
        case 'A':
        case 'a':
            id = IDABORT;
            break;
        case 'R':
        case 'r':
            id = IDRETRY;
            break;
        case 'I':
        case 'i':
            id = IDIGNORE;
            break;
        }
        
        if (id != 0 && GetDlgItem (hWnd, id))
            EndDialog (hWnd, id);
        break;
    }

    case MSG_CLOSE:
    {
    	HWND hWind;
    	pop_open_dlg(&headOpenDlg, &hWind);
    	
        if (GetDlgItem (hWnd, IDCANCEL)) {
            EndDialog (hWnd, IDCANCEL);
        }
        else if (GetDlgItem (hWnd, IDIGNORE)) {
            EndDialog (hWnd, IDIGNORE);
        }
        else if (GetDlgItem (hWnd, IDNO)) {
            EndDialog (hWnd, IDNO);
        }
        else if (GetDlgItem (hWnd, IDOK)) {
            EndDialog (hWnd, IDOK);
		}
        break;
	}
    default:
        break;
    }

    return DefaultDialogProc (hWnd, message, wParam, lParam);
}

static void get_box_xy (HWND hParentWnd, DWORD dwStyle, DLGTEMPLATE* MsgBoxData)
{
    RECT rcTemp;

    if (dwStyle & MB_BASEDONPARENT) {
        GetWindowRect (hParentWnd, &rcTemp);
    }
    else {
        rcTemp = g_rcDesktop;
    }

    switch (dwStyle & MB_ALIGNMASK) {
        case MB_ALIGNCENTER:
            MsgBoxData->x = rcTemp.left + (RECTW(rcTemp) - MsgBoxData->w)/2;
            MsgBoxData->y = rcTemp.top + (RECTH(rcTemp) - MsgBoxData->h)/2;
            break;

        case MB_ALIGNTOPLEFT:
            MsgBoxData->x = rcTemp.left;
            MsgBoxData->y = rcTemp.top;
            break;

        case MB_ALIGNBTMLEFT:
            MsgBoxData->x = rcTemp.left;
            MsgBoxData->y = rcTemp.bottom - MsgBoxData->h;
            break;

        case MB_ALIGNTOPRIGHT:
            MsgBoxData->x = rcTemp.right - MsgBoxData->w;
            MsgBoxData->y = rcTemp.top;
            break;

        case MB_ALIGNBTMRIGHT:
            MsgBoxData->x = rcTemp.right - MsgBoxData->w;
            MsgBoxData->y = rcTemp.bottom - MsgBoxData->h;
            break;
    }

    if ((MsgBoxData->x + MsgBoxData->w) > g_rcDesktop.right) {
        MsgBoxData->x = g_rcDesktop.right - MsgBoxData->w;
    }

    if ((MsgBoxData->y + MsgBoxData->h) > g_rcDesktop.bottom) {
        MsgBoxData->y = g_rcDesktop.bottom - MsgBoxData->h;
    }
}

static int GUIAPI InitMessageBox (HWND hParentWnd, const char* pszText, 
                      const char* pszCaption, DWORD dwStyle)
{


    int i, nButtons, buttonx;
    RECT rcText, rcButtons, rcIcon;
    int width, height;
    CTRLDATA  CtrlData [5] = 
	{
	    {
	    	GUI_BUTTON, 
	        BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP | BS_BITMAP, 
	        0, 0, 0, 0, 0, NULL, (DWORD)&button_pic[0], WS_EX_TRANSPARENT
	    },
	    {
	    	GUI_BUTTON, 
	        BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE | BS_BITMAP, 
	        0, 0, 0, 0, 0, NULL, (DWORD)&button_pic[0], WS_EX_TRANSPARENT
	    },
	    {
	    	GUI_BUTTON,
	        BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE | BS_BITMAP, 
	        0, 0, 0, 0, 0, NULL, (DWORD)&button_pic[0], WS_EX_TRANSPARENT
	    },

	};
#if 1
    if (pszCaption)
    {
		MsgBoxData.caption  = LTR(pszCaption);
    }
    else
    {
        MsgBoxData.caption  = "MiniGUI";
    }
    strcpy(captionData, MsgBoxData.caption);
#endif
    switch (dwStyle & MB_TYPEMASK) {
    case MB_OK:
        MsgBoxData.controlnr = 1;
        CtrlData [0].caption = LTR(GetSysText (IDS_MGST_OK));
        CtrlData [0].id      = IDOK;
        break;
    case MB_OKCANCEL:
        MsgBoxData.controlnr = 2;
        CtrlData [0].caption = LTR(GetSysText (IDS_MGST_OK));
        CtrlData [0].id      = IDOK;
        CtrlData [1].caption = LTR((dwStyle & MB_CANCELASBACK) ? 
                        GetSysText (IDS_MGST_PREV) : GetSysText (IDS_MGST_CANCEL));
        CtrlData [1].id      = IDCANCEL;
        break;
    case MB_YESNO:
        MsgBoxData.controlnr = 2;
        CtrlData [0].caption = LTR("是");//LTR(GetSysText (IDS_MGST_YES));
        CtrlData [0].id      = IDYES;
        CtrlData [1].caption = LTR("否");//LTR(GetSysText (IDS_MGST_NO));
        CtrlData [1].id      = IDNO;
        break;
    case MB_RETRYCANCEL:
        MsgBoxData.controlnr = 2;
        CtrlData [0].caption = LTR(GetSysText (IDS_MGST_RETRY));
        CtrlData [0].id      = IDRETRY;
        CtrlData [1].caption = LTR((dwStyle & MB_CANCELASBACK) ? 
                        GetSysText (IDS_MGST_PREV) : GetSysText (IDS_MGST_CANCEL));
        CtrlData [1].id      = IDCANCEL;
        break;
    case MB_ABORTRETRYIGNORE:
        MsgBoxData.controlnr = 3;
        CtrlData [0].caption = LTR(GetSysText (IDS_MGST_ABORT));
        CtrlData [0].id      = IDABORT;
        CtrlData [1].caption = LTR(GetSysText (IDS_MGST_RETRY));
        CtrlData [1].id      = IDRETRY;
        CtrlData [2].caption = LTR(GetSysText (IDS_MGST_IGNORE));
        CtrlData [2].id      = IDIGNORE;
        break;
    case MB_YESNOCANCEL:
        MsgBoxData.controlnr = 3;
        CtrlData [0].caption = LTR(GetSysText (IDS_MGST_YES));
        CtrlData [0].id      = IDYES;
        CtrlData [1].caption = LTR(GetSysText (IDS_MGST_NO));
        CtrlData [1].id      = IDNO;
        CtrlData [2].caption = LTR((dwStyle & MB_CANCELASBACK) ?
                        GetSysText (IDS_MGST_PREV) : GetSysText (IDS_MGST_CANCEL));
        CtrlData [2].id      = IDCANCEL;
        break;
    }

    switch (dwStyle & MB_DEFMASK) {
    case MB_DEFBUTTON1:
        CtrlData [0].dwStyle |= BS_DEFPUSHBUTTON;
        break;
    case MB_DEFBUTTON2:
        if (MsgBoxData.controlnr > 1)
            CtrlData [1].dwStyle |= BS_DEFPUSHBUTTON;
        break;
    case MB_DEFBUTTON3:
        if (MsgBoxData.controlnr > 2)
            CtrlData [2].dwStyle |= BS_DEFPUSHBUTTON;
        break;
    }

    nButtons = MsgBoxData.controlnr;
    rcButtons.left   = 0;
    rcButtons.top    = 0;
    rcButtons.bottom = MB_BUTTONH;
    rcButtons.right  = MsgBoxData.controlnr * MB_BUTTONW + 
                (MsgBoxData.controlnr - 1) * (MB_MARGIN << 1);

    rcIcon.left   = 0;
    rcIcon.top    = 0;
    rcIcon.right  = 0;
    rcIcon.bottom = 0;
    if (dwStyle & MB_ICONMASK) {
        int id_icon = -1;
        i = MsgBoxData.controlnr;
        CtrlData [i].class_name= "static";
        CtrlData [i].dwStyle   = WS_VISIBLE | SS_ICON | WS_GROUP;
        CtrlData [i].x         = MB_MARGIN;
        CtrlData [i].y         = MB_MARGIN;
#ifdef _TINY_SCREEN
        CtrlData [i].w         = 16;
        CtrlData [i].h         = 16;
#else
        CtrlData [i].w         = 32;
        CtrlData [i].h         = 32;
#endif
        CtrlData [i].id        = IDC_STATIC;
        CtrlData [i].caption   = "Hello";
        switch (dwStyle & MB_ICONMASK) {
        case MB_ICONSTOP:
            id_icon = IDI_STOP;
            break;
        case MB_ICONINFORMATION:
            id_icon = IDI_INFORMATION;
            break;
        case MB_ICONEXCLAMATION:
            id_icon = IDI_EXCLAMATION;
            break;
        case MB_ICONQUESTION:
            id_icon = IDI_QUESTION;
            break;
        }

#ifdef _TINY_SCREEN
        if (id_icon != -1) {
            CtrlData [i].dwAddData = GetSmallSystemIcon (id_icon);
        }
        rcIcon.right  = 16;
        rcIcon.bottom = 16;
#else
        if (id_icon != -1) {
            CtrlData [i].dwAddData = GetLargeSystemIcon (id_icon);
            MsgBoxData.hIcon       = GetSmallSystemIcon (id_icon);
        }
        rcIcon.right  = 32;
        rcIcon.bottom = 32;
#endif

        MsgBoxData.controlnr ++;
    }

    rcText.left = 0;
    rcText.top  = 0;
    rcText.right = rcButtons.right + (MB_MARGIN << 1);
    rcText.right = MAX (rcText.right, MB_TEXTW);
    rcText.bottom = GetSysCharHeight ();

    SelectFont (HDC_SCREEN, GetSystemFont (SYSLOGFONT_CONTROL));
    DrawText (HDC_SCREEN, pszText, -1, &rcText, 
                DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EXPANDTABS | DT_CALCRECT);
#ifdef _TINY_SCREEN
    rcText.right = MAX (rcText.right, MB_TEXTW);
#endif   
    i = MsgBoxData.controlnr;

    CtrlData [i].class_name= CTRL_STATIC;
    CtrlData [i].dwStyle   = WS_VISIBLE | SS_LEFT | WS_GROUP;
    CtrlData [i].x         = RECTW (rcIcon) + (MB_MARGIN << 1);
    CtrlData [i].y         = MB_MARGIN+20;
    CtrlData [i].w         = RECTW (rcText);
    CtrlData [i].h         = RECTH (rcText);
    CtrlData [i].id        = IDC_STATIC;
    CtrlData [i].caption   = LTR(pszText);
    CtrlData [i].dwAddData = 0;
    CtrlData [i].dwExStyle = WS_EX_TRANSPARENT;//由于static控件的字体颜色为白色，设置其背景为透明

    MsgBoxData.controlnr ++;

    width = MAX (RECTW (rcText), RECTW (rcButtons)) + RECTW (rcIcon)
                + (MB_MARGIN << 2)
                + (GetMainWinMetrics(MWM_BORDER) << 1);
    height = MAX (RECTH (rcText), RECTH (rcIcon)) + RECTH (rcButtons)
                + MB_MARGIN + (MB_MARGIN << 1) 
                + (GetMainWinMetrics (MWM_BORDER) << 1) 
                + GetMainWinMetrics (MWM_CAPTIONY);
    
    buttonx = (width - RECTW (rcButtons)) >> 1;
    for (i = 0; i < nButtons; i++) {
        CtrlData[i].x = buttonx + i*(MB_BUTTONW + MB_MARGIN);
        CtrlData[i].y = MAX (RECTH (rcIcon), RECTH (rcText)) + (MB_MARGIN<<1)+20;
        CtrlData[i].w = MB_BUTTONW;
        CtrlData[i].h = MB_BUTTONH;
    }

    MsgBoxData.w = width;
    MsgBoxData.h = height;
    get_box_xy (hParentWnd, dwStyle, &MsgBoxData);
    
	MsgBoxData.controls = CtrlData;	  
	return DialogBoxIndirectParam (&MsgBoxData, hParentWnd, MsgBoxProc, (LPARAM)dwStyle);
}

int GUIAPI CreateMessageBoxWin(HWND hParent, const char* pszText, 
                      const char* pszCaption, DWORD dwStyle)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "MessageBox" , 0 , 0);
#endif
	memset(captionData, 0, sizeof(captionData)/sizeof(captionData[0]));
	
	return InitMessageBox(hParent, pszText, pszCaption, dwStyle);
}

