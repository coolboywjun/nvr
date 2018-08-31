/*
** $Id: internals.h 9227 2008-01-30 03:08:12Z xwyan $
**
** internals.h: this head file declares all internal types and data.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/05/22
*/

#ifndef GUI_INTERNALS_H
    #define GUI_INTERNALS_H

/******************* Internal data *******************************************/

#define DEF_NR_TIMERS       32

#if defined (__NOUNIX__) || defined (__uClinux__)
  #define DEF_MSGQUEUE_LEN    8
  #define SIZE_CLIPRECTHEAP   16
  #define SIZE_INVRECTHEAP    16
  #define SIZE_QMSG_HEAP      8
#else
 #ifdef _LITE_VERSION
  #define DEF_MSGQUEUE_LEN    16
  #define SIZE_CLIPRECTHEAP   16
  #define SIZE_INVRECTHEAP    32
  #define SIZE_QMSG_HEAP      16
 #else
  #define DEF_MSGQUEUE_LEN    16     /* default message queue length */
  #define SIZE_CLIPRECTHEAP   128
  #define SIZE_INVRECTHEAP    128
  #define SIZE_QMSG_HEAP      8
 #endif
#endif

/******************* Internal data of fix string module **********************/
#if defined (__NOUNIX__) || defined (__uClinux__)
  #define MAX_LEN_FIXSTR      64
  #define NR_HEAP             5
  #define LEN_BITMAP          (1+2+4+8+16)
#else
 #ifndef _LITE_VERSION
  #define MAX_LEN_FIXSTR      2048
  #define NR_HEAP             10
  #define LEN_BITMAP          (1+2+4+8+16+32+64+128+256+512)
 #else
  #define MAX_LEN_FIXSTR      64
  #define NR_HEAP             5
  #define LEN_BITMAP          (1+2+4+8+16)
 #endif
#endif

/******************** Handle type and child type. ***************************/
#define TYPE_HWND           0x01
    #define TYPE_MAINWIN    0x11
    #define TYPE_CONTROL    0x12
    #define TYPE_ROOTWIN    0x13
#define TYPE_HMENU          0x02
    #define TYPE_MENUBAR    0x21
    #define TYPE_PPPMENU    0x22
    #define TYPE_NMLMENU    0x23
#define TYPE_HACCEL         0x03
#define TYPE_HCURSOR        0x05
#define TYPE_HICON          0x07
#define TYPE_HDC            0x08
    #define TYPE_SCRDC      0x81
    #define TYPE_GENDC      0x82
    #define TYPE_MEMDC      0x83

#define TYPE_WINTODEL       0xF1
#define TYPE_UNDEFINED      0xFF

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#if defined (_LITE_VERSION) && !defined (_STAND_ALONE)

#include "zi-procs.h"

#else /* _LITE_VERSION && !_STAND_ALONE */

typedef struct _ZORDERNODE
{
     HWND hWnd;                     /* Handle of window */
 
     struct _ZORDERNODE* pNext;     /* Next window */
}ZORDERNODE;
typedef ZORDERNODE* PZORDERNODE;

typedef struct _ZORDERINFO
{
    int nNumber;                    /* Number of windows */

    HWND hWnd;                      /* Handle of host window */

    PZORDERNODE pTopMost;           /* the top most Z order node */
} ZORDERINFO;
typedef ZORDERINFO* PZORDERINFO;

#endif /* !_LITE_VERSION || _STAND_ALONE */

typedef struct _SCROLLWINDOWINFO
{
    int iOffx;
    int iOffy;
    const RECT* rc1;
    const RECT* rc2;
}SCROLLWINDOWINFO;
typedef SCROLLWINDOWINFO* PSCROLLWINDOWINFO;

#define SBS_NORMAL      0x00
#define SBS_DISABLED    0x01
#define SBS_HIDE        0x02
typedef struct _SCROLLBARINFO {
    int  minPos;        // min value of scroll range.
    int  maxPos;        // max value of scroll range.
    int  curPos;        // current scroll pos.
    int  pageStep;      // steps per page.
    int  barStart;      // start pixel of bar.
    int  barLen;        // length of bar.
    BYTE status;        // status of scroll bar.
}SCROLLBARINFO;
typedef SCROLLBARINFO* PSCROLLBARINFO;

typedef struct _CARETINFO {
    int     x;              // position of caret
    int     y;
    void*   pNormal;        // normal bitmap.
    void*   pXored;         // bit-Xored bitmap.
    
    PBITMAP pBitmap;        // user defined caret bitmap.

    int     nWidth;         // original size of caret
    int     nHeight;
    int     nBytesNr;       // number of bitmap bytes.

#ifdef _USE_NEWGAL
    BITMAP  caret_bmp;      // bitmap of caret.
#else
    int     nEffWidth;      // effective width
    int     nEffHeight;     // effective height;
#endif

    BOOL    fBlink;         // does blink?
    BOOL    fShow;          // show or hide currently.

    HWND    hOwner;         // the window owns the caret.
    UINT    uTime;          // the blink time.
}CARETINFO;
typedef CARETINFO* PCARETINFO;

typedef struct _QMSG
{
    MSG                 Msg;
    struct _QMSG*       next;
}QMSG;
typedef QMSG* PQMSG;

typedef struct _MSGQUEUE MSGQUEUE;
typedef MSGQUEUE* PMSGQUEUE;

#ifndef _LITE_VERSION
typedef struct _SYNCMSG
{
    MSG              Msg;
    int              retval;
    sem_t*           sem_handle;
    struct _SYNCMSG* pNext;
}SYNCMSG;
typedef SYNCMSG* PSYNCMSG;
#else
typedef BOOL (* IDLEHANDLER) (PMSGQUEUE msg_que);
#endif

struct _MAINWIN;
typedef struct _MAINWIN* PMAINWIN;

// the MSGQUEUE struct is a internal struct.
// using semaphores to implement message queue.
struct _MSGQUEUE
{
    DWORD dwState;              // message queue states

#ifndef _LITE_VERSION
    pthread_mutex_t lock;       // lock
    sem_t wait;                 // the semaphore for wait message
    sem_t sync_msg;             // the semaphore for sync message
#endif

    PQMSG  pFirstNotifyMsg;     // head of the notify message queue
    PQMSG  pLastNotifyMsg;      // tail of the notify message queue

#ifndef _LITE_VERSION
    PSYNCMSG pFirstSyncMsg;     // head of the sync message queue
    PSYNCMSG pLastSyncMsg;      // tail of the sync message queue
#else
    IDLEHANDLER OnIdle;         // Idle handler
#endif

#ifndef _LITE_VERSION
    PMAINWIN pRootMainWin;      // The root main window of this message queue.
#endif

    MSG* msg;                   /* post message buffer */
    int len;                    /* buffer len */
    int readpos, writepos;      /* positions for reading and writing */

    int FirstTimerSlot;         /* the first timer slot to be checked */
    DWORD TimerMask;            /* timer slots mask */

    int loop_depth;             /* message loop depth, for dialog boxes. */
};

BOOL InitFreeQMSGList (void);
void DestroyFreeQMSGList (void);
BOOL InitMsgQueue (PMSGQUEUE pMsgQueue, int iBufferLen);
void DestroyMsgQueue (PMSGQUEUE pMsgQueue);
BOOL QueueMessage (PMSGQUEUE msg_que, PMSG msg);

extern PMSGQUEUE __mg_dsk_msg_queue;

static inline BOOL QueueDeskMessage (PMSG msg)
{
    return QueueMessage (__mg_dsk_msg_queue, msg);
}

#ifdef _LITE_VERSION
static inline BOOL InitDskMsgQueue (void)
{
    return InitMsgQueue (__mg_dsk_msg_queue, 0);
}

static inline void DestroyDskMsgQueue (void)
{
    DestroyMsgQueue (__mg_dsk_msg_queue);
}

static inline void SetDskIdleHandler (IDLEHANDLER idle_handler)
{
    __mg_dsk_msg_queue->OnIdle = idle_handler;
}

#endif

#ifndef _LITE_VERSION

  #define MG_MUTEX_INIT(lock)      pthread_mutex_init(lock, NULL)
  #define MG_MUTEX_DESTROY(lock)   pthread_mutex_destroy(lock)
  #define MG_MUTEX_LOCK(lock)      pthread_mutex_lock(lock)
  #define MG_MUTEX_UNLOCK(lock)    pthread_mutex_unlock(lock)

  #define POST_MSGQ(pMsgQueue) \
  { \
    int sem_value; \
    /* Signal that the msg queue contains one more element for reading */ \
    sem_getvalue (&(pMsgQueue)->wait, &sem_value); \
    if (sem_value <= 0) \
        sem_post(&(pMsgQueue)->wait); \
  }

#else

  #define MG_MUTEX_INIT(lock)
  #define MG_MUTEX_DESTROY(lock)
  #define MG_MUTEX_LOCK(lock)
  #define MG_MUTEX_UNLOCK(lock)

  #define POST_MSGQ(pMsgQueue)

#endif

#define LOCK_MSGQ(pMsgQueue)     MG_MUTEX_LOCK(&(pMsgQueue)->lock)
#define UNLOCK_MSGQ(pMsgQueue)   MG_MUTEX_UNLOCK(&(pMsgQueue)->lock)

struct _wnd_element_data;

// this struct is an internal struct
typedef struct _MAINWIN
{
    /*
     * These fields are similiar with CONTROL struct.
     */
    short DataType;     // the data type.
    short WinType;      // the window type.

    int left, top;      // the position and size of main window.
    int right, bottom;

    int cl, ct;         // the position and size of client area.
    int cr, cb;

    DWORD dwStyle;      // the styles of main window.
    DWORD dwExStyle;    // the extended styles of main window.
    
    int iBkColor;       // the background color.
    HMENU hMenu;        // handle of menu.
    HACCEL hAccel;      // handle of accelerator table.
    HCURSOR hCursor;    // handle of cursor.
    HICON hIcon;        // handle of icon.
    HMENU hSysMenu;     // handle of system menu.
    PLOGFONT pLogFont;  // pointer to logical font.

    HDC   privCDC;      // the private client DC.
    INVRGN InvRgn;      // the invalid region of this main window.
    PGCRINFO pGCRInfo;  // pointer to global clip region info struct.

    // the Z order node.
#if defined (_LITE_VERSION) && !defined (_STAND_ALONE)
    int idx_znode;
#else
    PZORDERNODE pZOrderNode;
#endif

    PCARETINFO pCaretInfo;
                        // pointer to system caret info struct.

    DWORD dwAddData;    // the additional data.
    DWORD dwAddData2;   // the second addtional data.

    int (*MainWindowProc)(HWND, int, WPARAM, LPARAM);
                           // the address of main window procedure.

    char* spCaption;    // the caption of main window.
    int   id;           // the identifier of main window.

    SCROLLBARINFO vscroll;
                        // the vertical scroll bar information.
    SCROLLBARINFO hscroll;
                        // the horizital scroll bar information.

    struct _MAINWIN* pMainWin;
                        // the main window that contains this window.
                        // for main window, always be itself.

    HWND hParent;       // the parent of this window. 
                        // for main window, always be HWND_DESKTOP.
    
    /*
     * Child windows.
     */
    HWND hFirstChild;    // the handle of first child window.
    HWND hActiveChild;  // the currently active child window.
    HWND hOldUnderPointer;  // the old child window under pointer.
    HWND hPrimitive;    // the premitive child of mouse event.

    NOTIFPROC NotifProc;    // the notification callback procedure.

    /*
     * window element data.
     */
    struct _wnd_element_data* wed;
    
    /*
     * Main Window hosting. 
     * The following members are only implemented for main window.
     */
    struct _MAINWIN* pHosting;
                        // the hosting main window.
    struct _MAINWIN* pFirstHosted;
                        // the first hosted main window.
    struct _MAINWIN* pNextHosted;
                        // the next hosted main window.

    PMSGQUEUE pMessages;
                        // the message queue.

    GCRINFO GCRInfo;
                        // the global clip region info struct.
                        // put here to avoid invoking malloc function.

#ifndef _LITE_VERSION
    pthread_t th;        // the thread which creates this main window.
#endif

} MAINWIN;

/************************* Initialization/Termination ************************/

void __mg_init_local_sys_text (void);

#ifdef _LITE_VERSION

#ifdef _STAND_ALONE

BOOL IdleHandler4StandAlone (PMSGQUEUE msg_que);
BOOL StandAloneStartup (void);
void StandAloneCleanup (void);

#else /* _STAND_ALONE */

/*
 * Common for server and client.
 */

/* the zorder information of the server */
extern ZORDERINFO* __mg_zorder_info;

/* the client id is zero for the server */
extern int __mg_client_id; 

BOOL IsOnlyMe (void);
void* LoadSharedResource (void);
void UnloadSharedResource (void);
BOOL IdleHandler4Server (PMSGQUEUE msg_que);
void ServerCleanup (void);

/* Only for client. */
void* AttachSharedResource (void);
void UnattachSharedResource (void);
BOOL IdleHandler4Client (PMSGQUEUE msg_que);
BOOL ClientStartup (void);
void ClientCleanup (void);

#endif /* !_STAND_ALONE */

#endif /* _LITE_VERSION */

BOOL InitScreenDC (void* surface);
void TerminateScreenDC (void);

BOOL InitGDI (void);
void TerminateGDI (void);

BOOL InitFixStr (void);
void TerminateFixStr (void);

BOOL InitMenu (void);
void TerminateMenu (void);

BOOL InitMainWinMetrics (void);
BOOL InitWindowElementColors (void);

BOOL InitDesktop (void);
void TerminateDesktop (void);

/* return main window contains hWnd. */
PMAINWIN GetMainWindowPtrOfControl (HWND hWnd);

/* check whether hWnd is main window and return pointer to main window hWnd. */
PMAINWIN CheckAndGetMainWindowPtr (HWND hWnd);

PMAINWIN GetMainWindowPtrUnderPoint (int x, int y);

/* return message queue of window. */
PMSGQUEUE GetMsgQueue (HWND hWnd); 

/* return global clipping region of window. */
PGCRINFO GetGCRgnInfo (HWND hWnd); 

/* internal variables */
typedef struct _TRACKMENUINFO* PTRACKMENUINFO;

extern unsigned int __mg_timer_counter;

extern HWND __mg_capture_wnd;
extern HWND __mg_ime_wnd;
#if !defined (_LITE_VERSION) || defined (_STAND_ALONE)
extern PMAINWIN __mg_active_mainwnd;
extern PTRACKMENUINFO __mg_ptmi;
#endif
extern PMAINWIN __mg_dsk_win;
extern HWND __mg_hwnd_desktop;

extern int DesktopWinProc (HWND hWnd, 
                int message, WPARAM wParam, LPARAM lParam);
extern int SendSyncMessage (HWND hWnd, int msg, WPARAM wParam, LPARAM lParam);


#ifdef _LITE_VERSION
    extern unsigned int __mg_csrimgsize;
    extern unsigned int __mg_csrimgpitch;
#else
    extern pthread_mutex_t __mg_gdilock, __mg_mouselock;
    extern pthread_t __mg_desktop, __mg_parsor, __mg_timer;
#endif /* !_LITE_VERSION */


/* hWnd is a window, including HWND_DESKTOP */
#define MG_IS_WINDOW(hWnd)              \
            (hWnd &&                    \
             hWnd != HWND_INVALID &&    \
             ((PMAINWIN)hWnd)->DataType == TYPE_HWND)

/* hWnd is a normal window, not including HWND_DESKTOP */
#define MG_IS_NORMAL_WINDOW(hWnd)       \
        (hWnd != HWND_DESKTOP && MG_IS_WINDOW(hWnd))

/* hWnd is a main window */
#define MG_IS_MAIN_WINDOW(hWnd)         \
        (MG_IS_WINDOW(hWnd) && ((PMAINWIN)hWnd)->WinType == TYPE_MAINWIN)

#define MG_IS_NORMAL_MAIN_WINDOW(hWnd)  \
        (hWnd != HWND_DESKTOP && MG_IS_MAIN_WINDOW(hWnd))

#define MG_IS_DESTROYED_WINDOW(hWnd)    \
        (hWnd &&                        \
         (hWnd != HWND_INVALID) &&      \
        (((PMAINWIN)hWnd)->DataType == TYPE_WINTODEL))

#define MG_GET_WINDOW_PTR(hWnd)   ((PMAINWIN)hWnd)
#define MG_GET_CONTROL_PTR(hWnd)  ((PCONTROL)hWnd)

#define MG_CHECK_RET(condition, ret) \
            if (!(condition)) return ret

#define MG_CHECK(condition) \
            if (!(condition)) return

/* get main window pointer of a window, including desktop window */
static inline PMAINWIN getMainWindowPtr (HWND hWnd)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_WINDOW(hWnd), NULL);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    return pWin->pMainWin;
}

/* -------------------------------------------------------------------------- */
#ifndef _LITE_VERSION

/* Be careful: does not check validity of hWnd */
static inline BOOL BE_THIS_THREAD (HWND hWnd)
{
    PMAINWIN pMainWin = getMainWindowPtr(hWnd);
#ifdef WIN32
    if (pMainWin && pMainWin->th.p == pthread_self().p)
#else
    if (pMainWin && pMainWin->th == pthread_self())
#endif
        return TRUE;

    return FALSE;
}

MSGQUEUE* InitMsgQueueThisThread (void);
void FreeMsgQueueThisThread (void);

extern pthread_key_t __mg_threadinfo_key;
static inline MSGQUEUE* GetMsgQueueThisThread (void)
{
    MSGQUEUE* pMsgQueue;

    pMsgQueue = (MSGQUEUE*) pthread_getspecific (__mg_threadinfo_key);
#ifdef __VXWORKS__
    if (pMsgQueue == (void *)-1) {
	return NULL;
    }
#endif
    return pMsgQueue;
}

#endif /* !_LITE_VERSION */

#ifdef _LITE_VERSION
static inline void 
SetDesktopTimerFlag (void)
{
    __mg_dsk_msg_queue->dwState |= QS_DESKTIMER;
}
#else
static inline void 
AlertDesktopTimerEvent (void)
{
    __mg_dsk_msg_queue->TimerMask = 1;
    POST_MSGQ(__mg_dsk_msg_queue);
}
#endif

static inline void
SetMsgQueueTimerFlag (PMSGQUEUE pMsgQueue, int slot)
{
    pMsgQueue->TimerMask |= (0x01 << slot);
    POST_MSGQ (pMsgQueue);
}

static inline void 
RemoveMsgQueueTimerFlag (PMSGQUEUE pMsgQueue, int slot)
{
    pMsgQueue->TimerMask &= ~(0x01 << slot);
}

BOOL InitTimer (void);
BOOL InstallIntervalTimer (void);
BOOL UninstallIntervalTimer (void);

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_INTERNALS_H

