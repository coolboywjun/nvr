/*
** $Id: ctrlclass.h 7337 2007-08-16 03:44:29Z xgwang $
**
** ctrlclass.h: the head file of Control Class module.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/5/21
*/

#ifndef GUI_CTRLCLASS_H
    #define GUI_CTRLCLASS_H

#define MAXLEN_CLASSNAME    15
typedef struct _CTRLCLASSINFO
{
    char      name[MAXLEN_CLASSNAME + 1];
                                // class name
    /*
     * common properties of this class
     */
    DWORD     dwStyle;          // Default control styles.
    DWORD     dwExStyle;        // Default control extended styles.

    HCURSOR   hCursor;          // control cursor
    int       iBkColor;         // control background color.
    
    int (*ControlProc)(HWND, int, WPARAM, LPARAM);
                                // control procedure.

    DWORD dwAddData;            // the additional data.

    int nUseCount;              // use count.
    struct _CTRLCLASSINFO*  next;
                                // next class info
}CTRLCLASSINFO;
typedef CTRLCLASSINFO* PCTRLCLASSINFO;

typedef struct tagCONTROL
{
    /*
     * These fields are similiar with MAINWIN struct.
     */
    short DataType;         // the data type
    short WinType;          // the window type

    int left, top;          // the position of control in main window's
    int right, bottom;      // client area.

    int cl, ct;             // the positio of control client in main window's
    int cr, cb;             // client area.

    DWORD dwStyle;          // the styles of child window.
    DWORD dwExStyle;        // the extended styles of child window.

    int iBkColor;		    // the background color.
    HMENU hMenu;		    // handle of menu.
    HACCEL hAccel;          // handle of accelerator table.
    HCURSOR hCursor;	    // handle of cursor.
    HICON hIcon;		    // handle of icon.
    HMENU hSysMenu;         // handle of system menu.
    PLOGFONT pLogFont;      // pointer to logical font.

    HDC   privCDC;          // the private client DC.
    INVRGN InvRgn;          // the invalid region of this control.
    PGCRINFO pGCRInfo;      // pointer to global clip region info struct.

    // the Z order node, 
    // only for control with WS_EX_CTRLASMAINWIN.
#if defined (_LITE_VERSION) && !defined (_STAND_ALONE)
    int idx_znode;
#else
    PZORDERNODE pZOrderNode;
#endif

    PCARETINFO pCaretInfo;  // pointer to system caret info struct.

    DWORD dwAddData;        // the additional data.
    DWORD dwAddData2;       // the second addtional data.

    int (*ControlProc) (HWND, int, WPARAM, LPARAM);

    char* spCaption;         // the caption of control.
    int   id;                // the identifier of child window.

    SCROLLBARINFO vscroll;   // the vertical scroll bar information.
    SCROLLBARINFO hscroll;   // the horizital scroll bar information.

    PMAINWIN pMainWin;       // the main window that contains this control.

    struct tagCONTROL* pParent;// the parent of this control.

    /*
     * Child windows.
     */
    struct tagCONTROL* children;
                             // the first child control.
    struct tagCONTROL* active;
                             // the active child control.
    struct tagCONTROL* old_under_pointer;
                             // the old under pointer child control.
    struct tagCONTROL* primitive;
                             // the primitive child of mouse event.

    NOTIFPROC notif_proc;    // the notification callback procedure.

    /*
     * window element data.
     */
    struct _wnd_element_data* wed;
    
    /*
     * The following members are only implemented for control.
     */
    struct tagCONTROL* next;   // the next sibling control.
    struct tagCONTROL* prev;   // the prev sibling control.

    PCTRLCLASSINFO pcci;     // pointer to Control Class Info struct.

}CONTROL;
typedef CONTROL* PCONTROL;

/* Function definitions */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitControlClass (void);
void TerminateControlClass (void);

PCTRLCLASSINFO GetControlClassInfo (const char* szClassName);
int ControlClassDataOp (int Operation, PWNDCLASS pWndClass);

MG_EXPORT int AddNewControlClass (PWNDCLASS pWndClass);
MG_EXPORT BOOL SetWindowExStyle (HWND hWnd, DWORD dwExStyle);
int DeleteControlClass (const char* szClassName);
void EmptyControlClassInfoTable (void);
PCONTROL Control (HWND hWnd);   // check whether hWnd is control
                                // and return pointer to control hWnd.

#ifdef _DEBUG
void DumpCtrlClassInfoTable (void);
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_CTRLCLASS_H */

