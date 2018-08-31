/**
 * \file window.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2002/01/26
 * 
 * This file includes windowing interfaces of MiniGUI.
 *
 \verbatim

    Copyright (C) 2002-2007 Feynman Software.
    Copyright (C) 1998-2002 Wei Yongming.

    All rights reserved by Feynman Software.

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

 \endverbatim
 */

/* 
 * $Id: window.h 8282 2007-11-27 09:22:35Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2007 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef _MGUI_WINDOW_H
#define _MGUI_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \defgroup msgs Messages
     * @{
     */

/** Definitions of common messages. */
#define MSG_NULLMSG         0x0000
#define MSG_SYNCMSG         0x0000

    /**
     * \defgroup mouse_msgs Mouse event messages
     * @{
     */

/* Group 1 from 0x0001 to 0x000F, the mouse messages. */
#define MSG_FIRSTMOUSEMSG   0x0001

/**
 * \def MSG_LBUTTONDOWN
 * \brief Left mouse button down message.
 *
 * This message is posted to the window when the user presses down
 * the left button of the mouse in the client area of the window.
 *
 * \code
 * MSG_LBUTTONDOWN
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_LBUTTONUP, key_defs
 *
 * Example:
 * \include buttondown.c
 */
#define MSG_LBUTTONDOWN     0x0001

/**
 * \def MSG_LBUTTONUP
 * \brief Left mouse button up message.
 *
 * This message is posted to the window when the user releases up
 * the left button of the mouse in the client area of the window.
 *
 * \code
 * MSG_LBUTTONUP
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_LBUTTONDOWN, key_defs
 */
#define MSG_LBUTTONUP       0x0002

/**
 * \def MSG_LBUTTONDBLCLK
 * \brief Left mouse button double clicked message.
 *
 * This message is posted to the window when the user double clicks
 * the left button of the mouse in the client area of the window.
 *
 * \code
 * MSG_LBUTTONDBLCLK
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_RBUTTONDBLCLK, key_defs
 */
#define MSG_LBUTTONDBLCLK   0x0003

/**
 * \def MSG_MOUSEMOVE
 * \brief The mouse moved message.
 *
 * This message is posted to the window when the user moves the mouse
 * in the client area of the window.
 *
 * \code
 * MSG_MOUSEMOVE
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa key_defs
 */
#define MSG_MOUSEMOVE       0x0004

/**
 * \def MSG_RBUTTONDOWN
 * \brief Right mouse button down message.
 *
 * This message is posted to the window when the user presses down
 * the right button of the mouse in the client area of the window.
 *
 * \code
 * MSG_RBUTTONDOWN
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_RBUTTONUP, key_defs
 *
 * Example:
 * \include buttondown.c
 */
#define MSG_RBUTTONDOWN     0x0005

/**
 * \def MSG_RBUTTONUP
 * \brief Right mouse button up message.
 *
 * This message is posted to the window when the user releases up
 * the right button of the mouse in the client area of the window.
 *
 * \code
 * MSG_RBUTTONUP
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_RBUTTONDOWN, key_defs
 */
#define MSG_RBUTTONUP       0x0006

/**
 * \def MSG_RBUTTONDBLCLK
 * \brief Right mouse button double clicked message.
 *
 * This message is posted to the window when the user double clicks
 * the right button of the mouse in the client area of the window.
 *
 * \code
 * MSG_RBUTTONDBLCLK
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_LBUTTONDBLCLK, key_defs
 */
#define MSG_RBUTTONDBLCLK   0x0007

#define MSG_NCMOUSEOFF      0x0007

/**
 * \def MSG_NCLBUTTONDOWN
 * \brief Left mouse button down message in the non-client area.
 *
 * This message is posted to the window when the user presses down
 * the left button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCLBUTTONDOWN
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCLBUTTONUP, MSG_NCHITTEST
 */
#define MSG_NCLBUTTONDOWN   0x0008

/**
 * \def MSG_NCLBUTTONUP
 * \brief Left mouse button up message in the non-client area.
 *
 * This message is posted to the window when the user releases up
 * the left button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCLBUTTONUP
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCLBUTTONDOWN, MSG_NCHITTEST
 */
#define MSG_NCLBUTTONUP     0x0009

/**
 * \def MSG_NCLBUTTONDBLCLK
 * \brief Left mouse button double clicked in the non-client area.
 *
 * This message is posted to the window when the user double clicks
 * the left button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCLBUTTONDBLCLK
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCRBUTTONDBLCLK, MSG_NCHITTEST
 */
#define MSG_NCLBUTTONDBLCLK 0x000A

/**
 * \def MSG_NCMOUSEMOVE
 * \brief Mouse moves in the non-client area.
 *
 * This message is posted to the window when the user moves the mouse
 * in the non-client area of the window.
 *
 * \code
 * MSG_NCMOUSEMOVE
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCHITTEST
 */
#define MSG_NCMOUSEMOVE     0x000B

/**
 * \def MSG_NCRBUTTONDOWN
 * \brief Right mouse button down message in the non-client area.
 *
 * This message is posted to the window when the user presses down
 * the right button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCRBUTTONDOWN
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCRBUTTONUP, MSG_NCHITTEST
 */
#define MSG_NCRBUTTONDOWN   0x000C

/**
 * \def MSG_NCRBUTTONUP
 * \brief Right mouse button up message in the non-client area.
 *
 * This message is posted to the window when the user releases up
 * the right button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCRBUTTONUP
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCRBUTTONDOWN, MSG_NCHITTEST
 */
#define MSG_NCRBUTTONUP     0x000D

/**
 * \def MSG_NCRBUTTONDBLCLK
 * \brief Right mouse button double clicked in the non-client area.
 *
 * This message is posted to the window when the user double clicks
 * the right button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCRBUTTONDBLCLK
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCLBUTTONDBLCLK, MSG_NCHITTEST
 */
#define MSG_NCRBUTTONDBLCLK 0x000E

#define MSG_LASTMOUSEMSG    0x000F

    /** @} end of mouse_msgs */

    /**
     * \defgroup key_msgs Key event messages
     * @{
     */

/* Group 2 from 0x0010 to 0x001F, the key messages. */
#define MSG_FIRSTKEYMSG     0x0010

/**
 * \def MSG_KEYDOWN
 * \brief User presses a key down.
 *
 * This message is posted to the current active window when the user 
 * presses a key down.
 *
 * \code
 * MSG_KEYDOWN
 * int scancode = (int)wParam;
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 * \param scancode The scan code of the pressed key.
 * \param key_flags The shift key status when this message occurred.
 *
 * \sa MSG_KEYUP, key_defs
 *
 * Example:
 *
 * \include keydown.c
 */
#define MSG_KEYDOWN         0x0010

/**
 * \def MSG_CHAR
 * \brief A character translated from MSG_KEYDOWN message.
 *
 * This message is translated from a MSG_KEYDOWN message by \a TranslateMessage 
 * and sent to the current active window.
 *
 * \code
 * MSG_CHAR
 * int ch = (int)wParam;
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 * \param ch The ASCII code of the pressed key.
 * \param key_flags The shift key status when this message occurred.
 *
 * \sa MSG_SYSCHAR, TranslateMessage, key_defs
 */
#define MSG_CHAR            0x0011

/**
 * \def MSG_KEYUP
 * \brief User releases up a key.
 *
 * This message is posted to the current active window when the user 
 * releases up a key.
 *
 * \code
 * MSG_KEYUP
 * int scancode = (int)wParam;
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 * \param scancode The scan code of the released key.
 * \param key_flags The shift key status when this message occurred.
 *
 * \sa MSG_KEYDOWN, key_defs
 */
#define MSG_KEYUP           0x0012

/**
 * \def MSG_SYSKEYDOWN
 * \brief User presses down a key when \<Alt\> key is down.
 *
 * This message is posted to the current active window when the user 
 * presses down a key as \<Alt\> key is down.
 *
 * \code
 * MSG_SYSKEYDOWN
 * int scancode = (int)wParam;
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 * \param scancode The scan code of the pressed key.
 * \param key_flags The shift key status when this message occurred.
 *
 * \sa MSG_SYSKEYUP, MSG_SYSCHAR, key_defs
 */
#define MSG_SYSKEYDOWN      0x0013

/**
 * \def MSG_SYSCHAR
 * \brief A system character translated from MSG_SYSKEYDOWN message.
 *
 * This message is translated from a MSG_SYSKEYDOWN message by 
 * \a TranslateMessage and sent to the current active window.
 *
 * \code
 * MSG_SYSCHAR
 * int ch = (int)wParam;
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 * \param ch The ASCII code of the pressed key.
 * \param key_flags The shift key status when this message occurred.
 *
 * \sa MSG_CHAR, TranslateMessage, key_defs
 */
#define MSG_SYSCHAR         0x0014

/**
 * \def MSG_SYSKEYUP
 * \brief User releases up a key when \<Alt\> key is down.
 *
 * This message is posted to the current active window when the user 
 * releases up a key as \<Alt\> key is down.
 *
 * \code
 * MSG_SYSKEYUP
 * int scancode = (int)wParam;
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 * \param scancode The scan code of the released key.
 * \param key_flags The shift key status when this message occurred.
 *
 * \sa MSG_SYSKEYDOWN, key_defs
 */
#define MSG_SYSKEYUP        0x0015

/**
 * \def MSG_KEYSYM
 * \brief A key symbol translated from MSG_KEYDOWN messages.
 *
 * This message is translated from a MSG_KEYDOWN message by 
 * \a TranslateMessage and sent to the current active window.
 *
 * Note that one translation may generate a key symbol made by more than one 
 * character, e.g., when using default keymap, DEL key will generate the 
 * key symbol "^[[3~".
 *
 * \code
 * MSG_KEYSYM
 * int index = HIBYTE (wParam);
 * int keysym = LOBYTE (wParam);
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 * \param index The index of the key symbol in one translation, 
 *        zero for a new translation, and the \a keysym is the first symbol.
 * \param keysym The code of the key symbol.
 * \param key_flags The shift key status when this message occurred.
 *
 * \sa MSG_SYSCHAR, TranslateMessage, key_defs
 */
#define MSG_KEYSYM  0x0016

/* keyboard longpress supported */

/**
 * \def MSG_KEYLONGPRESS
 * \brief A key is long pressed.
 *
 * This message is sent when a key is pressed exceed user-defined long 
 * time value.
 */
#define MSG_KEYLONGPRESS    0x0017

/**
 * \def MSG_KEYALWAYSPRESS
 * \brief A key is always pressed.
 *
 * This message is sent when a key is pressed to exceed user-defined
 * always time value.
 */
#define MSG_KEYALWAYSPRESS  0x0018

/**
 * \def DEF_LPRESS_TIME
 * \brief Default long pressed time of a key.
 *
 * \sa MSG_KEYLONGPRESS
 */
#define DEF_LPRESS_TIME     500

/**
 * \def DEF_APRESS_TIME
 * \brief Default always pressed time of a key.
 *
 * \sa MSG_KEYALWAYSPRESS
 */
#define DEF_APRESS_TIME     1000
/**
 * \def DEF_INTERVAL_TIME
 * \brief Default send MSG_KEYLONGPRESS in interval value. 
 *
 */
#define DEF_INTERVAL_TIME   200

extern unsigned int __mg_key_longpress_time;
extern unsigned int __mg_key_alwayspress_time;
extern unsigned int __mg_interval_time;

/**
 * \def SetKeyLongPressTime(time)
 * \brief User set default long pressed time of a key.
 */
#define SetKeyLongPressTime(time)       \
    do {                                \
        __mg_key_longpress_time = time; \
    } while (0)

/**
 * \def SetKeyAlwaysPressTime(time)
 * \brief User set default always pressed time of a key.
 */
#define SetKeyAlwaysPressTime(time)     \
    do {                                \
        __mg_key_alwayspress_time = time; \
    } while (0)

/**
 * \def SetIntervalTime(time)
 * \brief User set default interval time that MSG_KEYLONGPRESS is sent.
 */
#define SetIntervalTime(time)           \
    do {                                \
        __mg_interval_time = time;      \
    } while (0)

#define MSG_LASTKEYMSG      0x001F

    /** @} end of key_msgs */

    /**
     * \defgroup post_event_msgs Post mouse/key event messages
     * @{
     */

/* Group 3 from 0x0020 to 0x005F, the post mouse/key event messages. */
#define MSG_FIRSTPOSTMSG    0x0020

/**
 * \def MSG_SETCURSOR
 * \brief Sets cursor shape in the client area.
 *
 * This message is posted to the window under the cursor when the user moves
 * the mouse in order to give the chance to change the cursor shape.
 * The default handler set the cursor shape to the default cursor of the window.
 * If you set a new cursor shape, your message handler should return 
 * immediately.
 *
 * \code
 * MSG_SETCURSOR
 * int cx = LOSWORD (lParam);
 * int cy = HISWORD (lParam);
 * \endcode
 *
 * \param cx,cy The client coordinates of the cursor.
 *
 * Example:
 *
 * \include setcursor.c
 *
 * \sa MSG_NCSETCURSOR
 */
#define MSG_SETCURSOR       0x0020

    #define HT_UNKNOWN          0x00
    #define HT_OUT              0x01
    #define HT_MENUBAR          0x02
    #define HT_TRANSPARENT      0x03
    #define HT_BORDER           0x04
    #define HT_CLIENT           0x0C

    #define HT_NEEDCAPTURE      0x10
    #define HT_ICON             0x14
    #define HT_CLOSEBUTTON      0x15
    #define HT_MAXBUTTON        0x16
    #define HT_MINBUTTON        0x17
    #define HT_HSCROLL          0x18
    #define HT_VSCROLL          0x19
    
    #define HT_DRAGGABLE        0x20
    #define HT_CAPTION          0x21
    #define HT_BORDER_TOP       0x23
    #define HT_BORDER_BOTTOM    0x24
    #define HT_BORDER_LEFT      0x25
    #define HT_BORDER_RIGHT     0x26
    #define HT_CORNER_TL        0x27
    #define HT_CORNER_TR        0x28
    #define HT_CORNER_BL        0x29
    #define HT_CORNER_BR        0x2A

    #define SBPOS_LEFTARROW     0x81
    #define SBPOS_RIGHTARROW    0x82
    #define SBPOS_LEFTSPACE     0x83
    #define SBPOS_RIGHTSPACE    0x84
    #define SBPOS_UPARROW       0x85
    #define SBPOS_DOWNARROW     0x86
    #define SBPOS_UPSPACE       0x87
    #define SBPOS_DOWNSPACE     0x88
    #define SBPOS_THUMB         0x89
    #define SBPOS_UNKNOWN       0x80
    #define SBPOS_MASK          0x80

/**
 * \def MSG_NCHITTEST
 * \brief Hit test in non-client area. 
 * This is an async message.
 */
#define MSG_NCHITTEST       0x0021      

/**
 * \def MSG_HITTEST
 * \brief Hit test in non-client area. 
 *
 * \sa MSG_NCHITTEST
 */
#define MSG_HITTEST         MSG_NCHITTEST

/**
 * \def MSG_CHANGESIZE
 * \brief Change window size.
 */
#define MSG_CHANGESIZE      0x0022

/*reserved*/
#define MSG_QUERYNCRECT     0x0023

/**
 * \def MSG_QUERYCLIENTAREA
 * \brief Query client area. 
 */
#define MSG_QUERYCLIENTAREA 0x0024

/**
 * \def MSG_SIZECHANGING
 * \brief Indicates the size of the window is being changed.
 *
 * This message is sent to the window when the size is being changed.
 * If you want to control the actual position and size of the window when 
 * the size is being changed (this may be caused by \a MoveWindow or 
 * other functions), you should handle this message, and return the actual
 * position and size of the window through the second parameter.
 *
 * \code
 * MSG_SIZECHANGING
 * const RECT* rcExpect = (const RECT*)wParam;
 * RECT* rcResult = (RECT*)lParam;
 * \endcode
 *
 * \param rcExpect The expected size of the window after changing.
 * \param rcResult The actual size of the window after changing.
 *
 * Example:
 *
 * \include msg_sizechanging.c
 */
#define MSG_SIZECHANGING    0x0025

/**
 * \def MSG_SIZECHANGED
 * \brief Indicates the size of the window has been changed.
 *
 * This message is sent to the window when the size has been changed.
 * If you want adjust the size of the client area of the window, 
 * you should handle this message, change the values of the client area,
 * and return non-zero value to indicate that the client area has been 
 * modified.
 *
 * \code
 * MSG_SIZECHANGED
 * RECT* rcClient = (RECT*)lParam;
 * \endcode
 *
 * \param rcClient The pointer to a RECT structure which contains 
 *        the new client area.
 * 
 * Example:
 *
 * \include msg_sizechanged.c
 */
#define MSG_SIZECHANGED     0x0026

/**
 * \def MSG_CSIZECHANGED
 * \brief Indicates the size of the client area of the window has been changed.
 *
 * This message is sent as a notification to the window when the size of 
 * the client area has been changed.  
 *
 * \code
 * MSG_CSIZECHANGED
 * int client_width = (int)wParam;
 * int client_height = (int)lParam;
 * \endcode
 * 
 * \param client_width The width of the client area.
 * \param client_height The height of the client area.
 */
#define MSG_CSIZECHANGED     0x0027

/**
 * \def MSG_SETFOCUS
 * \brief Indicates that the window has gained the input focus.
 *
 * This message is sent to the window procedure 
 * after the window gains the input focus.
 */
#define MSG_SETFOCUS        0x0030

/**
 * \def MSG_KILLFOCUS
 * \brief Indicates that the window has lost the input focus.
 *
 * This message is sent to the window procedure 
 * after the window losts the input focus.
 */
#define MSG_KILLFOCUS       0x0031

/**
 * \def MSG_MOUSEACTIVE
 * \brief Indicates that the window has gained the input focus because
 * the user clicked the window.
 *
 * This message is sent to the window procedure 
 * after the user clicked the window and it has gained the input focus.
 */
#define MSG_MOUSEACTIVE     0x0032
/**
 * \def MSG_ACTIVE
 * \brief Indicates that the window has gained the input focus because
 * the user clicked the window.
 *
 * This message is sent to the window procedure 
 * after the user clicked the window and it has gained the input focus.
 */
#define MSG_ACTIVE          0x0033

/**
 * \def MSG_CHILDHIDDEN
 * \brief Hide child window.
 */
#define MSG_CHILDHIDDEN     0x0034
    #define RCTM_CLICK          1
    #define RCTM_KEY            2
    #define RCTM_MESSAGE        3
    #define RCTM_SHOWCTRL       4

/**
 * \def MSG_ACTIVEMENU
 * \brief Indicates that the user activates the menu bar and tracks it.
 *
 * This message is sent to the window procedure when the user
 * activates the menu bar and tracks it.
 *
 * If you want to change the states of menu items in the submenu 
 * before displaying it, you can handle this message.
 *
 * \code
 * MSG_ACTIVEMENU
 * int pos = (int)wParam;
 * HMENU submenu = (HMENU)lParam;
 * \endcode
 * 
 * \param pos The position of the activated submenu. The position value of the 
 *        first submenu is 0.
 * \param submenu The handle to the activated submenu.
 *
 * Example:
 *
 * \include activemenu.c
 */
#define MSG_ACTIVEMENU      0x0040

/**
 * \def MSG_DEACTIVEMENU
 * \brief Indicates the end of the tracking of a menu bar or a popup menu.
 *
 * This message is sent to the window procedure when the user has
 * closed the tracking menu bar or popup menu.
 *
 * \code
 * MSG_DEACTIVEMENU
 * HMENU menubar = (HMENU)wParam;
 * HMENU submenu = (HMENU)lParam;
 * \endcode
 *
 * \param menubar The handle to the menu bar. It will be zero when the 
 *        deactivated menu is a popup menu.
 * \param submenu The handle to the submenu.
 */
#define MSG_DEACTIVEMENU    0x0041

/* Scroll bar notifying code */
#define SB_LINEUP           0x01
#define SB_LINEDOWN         0x02
#define SB_LINELEFT         0x03
#define SB_LINERIGHT        0x04
#define SB_PAGEUP           0x05
#define SB_PAGEDOWN         0x06
#define SB_PAGELEFT         0x07
#define SB_PAGERIGHT        0x08
#define SB_THUMBPOSITION    0x09
#define SB_THUMBTRACK       0x0A
#define SB_ENDSCROLL        0x0B
    
/**
 * \def MSG_HSCROLL
 * \brief Indicates that the user has clicked the horizontal scroll bar.
 *
 * This message is sent to the window procedure when the user has clicked
 * the horizontal scroll bar and changed the position of the thumb.
 *
 * \code
 * MSG_HSCROLL
 * int hs_nc = (int)wParam;
 * \endcode
 *
 * \param hs_nc The scrolling code, can be one of the following values:
 *      - SB_LINELEFT\n
 *        The user clicked the left arrow on the bar.
 *      - SB_LINERIGHT\n
 *        The user clicked the right arrow on the bar.
 *      - SB_PAGELEFT\n
 *        The user clicked the left page area on the bar.
 *      - SB_PAGERIGHT\n
 *        The user clicked the right page area on the bar.
 *      - SB_THUMBPOSITION\n
 *        The user set a new thumb position.
 *      - SB_THUMBTRACK\n
 *        The user is draging and tracking the thumb.
 *      - SB_ENDSCROLL\n
 *        The end of scrolling.
 */
#define MSG_HSCROLL         0x0042

/**
 * \def MSG_VSCROLL
 * \brief Indicates that the user has clicked the vertical scroll bar.
 *
 * This message is sent to the window procedure when the user has clicked
 * the vertical scroll bar and changed the position of the thumb.
 *
 * \code
 * MSG_HSCROLL
 * int vs_nc = (int)wParam;
 * \endcode
 *
 * \param vs_nc The scrolling code, can be one of the following values:
 *      - SB_LINEUP\n
 *        The user clicked the up arrow on the bar.
 *      - SB_LINEDOWN\n
 *        The user clicked the down arrow on the bar.
 *      - SB_PAGEUP\n
 *        The user clicked the up page area on the bar.
 *      - SB_PAGEDOWN\n
 *        The user clicked the down page area on the bar.
 *      - SB_THUMBPOSITION\n
 *        The user set a new thumb position.
 *      - SB_THUMBTRACK\n
 *        The user is draging and tracking the thumb.
 *      - SB_ENDSCROLL\n
 *        The end of scrolling.
 */
#define MSG_VSCROLL         0x0043

/**
 * \def MSG_NCSETCURSOR
 * \brief Sets cursor shape in the non-client area.
 *
 * This message is posted to the window under the cursor when the user moves
 * the mouse in order to give the chance to change the cursor shape.
 * The default handler set the cursor shape to the default cursor of the window.
 * If you set a new cursor shape, your message handler should return 
 * immediately.
 *
 * \sa MSG_SETCURSOR
 */
#define MSG_NCSETCURSOR     0x0044

/**
 * \def MSG_MOUSEMOVEIN
 * \brief Indicates the mouse is moved in/out the area of the window.
 *
 * This message is posted to the window when the user moves the mouse
 * in/out the area of the window.
 *
 * \code
 * MSG_MOUSEMOVEIN
 * BOOL in_out = (BOOL)wParam;
 * \endcode
 *
 * \param in_out Indicates whether the mouse has been moved in the window
 *      or out the window.
 */
#define MSG_MOUSEMOVEIN     0x0050

/** 
 * \def MSG_WINDOWDROPPED
 * \brief Indicates that user dropped window.
 * server to client; (wParam, lParam): result rectangle. 
 */
#define MSG_WINDOWDROPPED   0x0051

#define MSG_LASTPOSTMSG     0x005F

    /** @} end of post_event_msgs */

    /**
     * \defgroup creation_msgs Window creation messages
     * @{
     */

    /* Group 4 from 0x0060 to 0x009F, the creation messages. */
#define MSG_FIRSTCREATEMSG  0x0060

/**
 * \def MSG_CREATE
 * \brief Indicates the window has been created, and gives you a chance to initialize your private objects.
 *
 * This messages is sent to the window after the window has been created 
 * and registered to the system. You can initialize your own objects when 
 * you receive this message, and return zero to the system in order to 
 * indicates the success of your initialization. If you return non-zero to 
 * the system after handled this message, the created window will be 
 * destroyed immediately.
 *
 * \code
 * MSG_CREATE for main windows:
 * PMAINWINCREATE create_info = (PMAINWINCREATE)lParam;
 *
 * MSG_CREATE for controls:
 * HWND parent = (HWND)wParam;
 * DWORD add_data = (DWORD)lParam;
 * \endcode
 *
 * \param create_info The pointer to the MAINWINCREATE structure which is 
 *        passed to CreateMainWindow function.
 * \param parent The handle to the parent window of the control.
 * \param add_data The first additional data passed to CreateWindowEx function.
 *
 * \sa CreateMainWindow, CreateWindowEx, MAINWINCREATE
 */
#define MSG_CREATE          0x0060

/**
 * \def MSG_NCCREATE
 * \brief Indicates the window has been created, but has not registered to 
 *        the system.
 *
 * This message is sent to the window after the window has been created, 
 * but not registered the system. Like MSG_CREATE message, you can 
 * initialize your own objects when you receive this message, but can not 
 * create child windows of the window, and can not get a device context 
 * to paint.
 *
 * If you return non-zero to the system after handled this message, 
 * the created window will be destroyed immediately.
 *
 * \code
 * MSG_NCCREATE for main windows:
 * PMAINWINCREATE create_info = (PMAINWINCREATE)lParam;
 *
 * MSG_NCCREATE for controls:
 * DWORD add_data = (DWORD)lParam;
 * \endcode
 *
 * \param create_info The pointer to the MAINWINCREATE structure which is 
 *        passed to CreateMainWindow function.
 * \param add_data The first additional data passed to CreateWindowEx function.
 *
 * \sa CreateMainWindow, CreateWindowEx, MAINWINCREATE
 */
#define MSG_NCCREATE        0x0061

/*Not use*/
#define MSG_INITPANES       0x0062
#define MSG_DESTROYPANES    0x0063

/**
 * \def MSG_DESTROY
 * \brief Indicates the window will be destroyed.
 *
 * This message is sent to the window when \a DestroyMainWindow 
 * or \a DestroyWindow is calling. You can destroy your private objects
 * when receiving this message.
 *
 * If you return non-zero to the system after handle this message, the process 
 * of \a DestroyMainWindow and \a DestroyWindow will return immediately.
 *
 * \sa DestroyMainWindow, DestroyWindow
 */
#define MSG_DESTROY         0x0064

/*Not use*/
#define MSG_NCDESTROY       0x0065

/**
 * \def MSG_CLOSE
 * \brief Indicates the user has clicked the closing box on the caption.
 *
 * This message is sent to the window when the user has clicked the closing box
 * on the caption of the window.
 */
#define MSG_CLOSE           0x0066

/*Not use*/
#define MSG_NCCALCSIZE      0x0067

/**
 * \def MSG_MAXIMIZE
 * \brief Indicates the user has clicked the maximizing box on the caption.
 *
 * This message is sent to the window when the user has clicked the maximizing
 * box on the caption of the window.
 */
#define MSG_MAXIMIZE		0x0068

/**
 * \def MSG_MINIMIZE
 * \brief Indicates the user has clicked the minimizing box on the caption.
 *
 * This message is sent to the window when the user has clicked the 
 * minimizing box on the caption of the window.
 */
#define MSG_MINIMIZE		0x0069

#define MSG_LASTCREATEMSG   0x009F

    /** @} end of creation_msgs */

    /**
     * \defgroup paint_msgs Window painting messages
     * @{
     */

/* Group 5 from 0x00A0 to 0x00CF, the paint messages. */
#define MSG_FIRSTPAINTMSG   0x00A0

/**
 * \def MSG_SHOWWINDOW
 * \brief Indicates that the window has been shown or hidden.
 *
 * This message is sent to the window when the window has been shown
 * or hidden (due to the calling of the function ShowWindow).
 *
 * \code
 * MSG_SHOWWINDOW
 * int show_cmd = (int)wParam;
 * \endcode
 *
 * \param show_cmd The command to show or hide, can be one of 
 * the following values:
 *      - SW_SHOW\n
 *        Shows the window.
 *      - SW_HIDE\n
 *        Hides the window.
 *      - SW_SHOWNORMAL\n
 *        Shows the window, and if the window is a main window 
 *        sets it to be the topmost main window.
 *
 * \sa ShowWindow
 */
#define MSG_SHOWWINDOW      0x00A0 

/**
 * \def MSG_ERASEBKGND
 * \brief Sent to the window to erase the background.
 *
 * This message is sent to the window if the whole or a part of the background
 * should be erased.
 *
 * \code
 * MSG_ERASEBKGND
 * const RECT* inv_rect = (const RECT*)lParam;
 * \endcode
 *
 * \param inv_rect The pointer to a RECT structure contains the rectangle
 *        should be erase. The rectangle is in client coordinates system. 
 *        If it is NULL, the whole client area should be erased.
 */
#define MSG_ERASEBKGND      0x00B0      /* this is an async message. */

/**
 * \def MSG_PAINT
 * \brief Sent to the window if the window contains an invalid region.
 *
 * This message is sent to the window if the window contains an invalid region.
 *
 * \code
 * MSG_PAINT
 * const CLIPRGN* inv_rgn = (const CLIPRGN*) lParam;
 * \endcode
 *
 * \param inv_rgn The pointer to the invalid region of the window.
 */
#define MSG_PAINT           0x00B1

/**
 * \def MSG_NCPAINT
 * \brief Indicates that paints non-client area. 
 */
#define MSG_NCPAINT         0x00B2

/**
 * \def MSG_NCACTIVATE
 * \brief Indicates that active non-client area of main window. 
 */
#define MSG_NCACTIVATE      0x00B3
/**
 * \def MSG_SYNCPAINT
 * \brief Indicates that actives and paints main window synchronously. 
 */
#define MSG_SYNCPAINT       0x00B4

#define MSG_LASTPAINTMSG    0x00CF

    /** @} end of paint_msgs */

    /**
     * \defgroup desktop_msgs Internal desktop messages
     * @{
     */

/* Group 6 from 0x00D0 to 0x00EF, the internal desktop messages. */
#define MSG_FIRSTSESSIONMSG 0x00D0

#define MSG_STARTSESSION    0x00D0
#define MSG_QUERYENDSESSION 0x00D1
#define MSG_ENDSESSION      0x00D2
#define MSG_REINITSESSION   0x00D3

#define MSG_ERASEDESKTOP    0x00DE
#define MSG_PAINTDESKTOP    0x00DF

#define MSG_DT_MOUSEOFF     0x00E0
#define MSG_DT_LBUTTONDOWN  0x00E1
#define MSG_DT_LBUTTONUP    0x00E2
#define MSG_DT_LBUTTONDBLCLK    0x00E3
#define MSG_DT_MOUSEMOVE    0x00E4
#define MSG_DT_RBUTTONDOWN  0x00E5
#define MSG_DT_RBUTTONUP    0x00E6
#define MSG_DT_RBUTTONDBLCLK    0x00E7

#define MSG_DT_KEYOFF       0x00D8 
#define MSG_DT_KEYDOWN      0x00E8
#define MSG_DT_CHAR         0x00E9
#define MSG_DT_KEYUP        0x00EA
#define MSG_DT_SYSKEYDOWN   0x00EB
#define MSG_DT_SYSCHAR      0x00EC
#define MSG_DT_SYSKEYUP     0x00ED
#define MSG_DT_KEYLONGPRESS     0x00EE
#define MSG_DT_KEYALWAYSPRESS   0x00EF

#define MSG_LASTSESSIONMSG  0x00EF

    /** @} end of desktop_msgs */

    /**
     * \defgroup window_msgs Internal window management messages
     * @{
     */

/* Group 7 from 0x00F0 to 0x010F, Internal window management messages. */
#define MSG_FIRSTWINDOWMSG  0x00F0

#define MSG_ADDNEWMAINWIN   0x00F0
#define MSG_REMOVEMAINWIN   0x00F1
#define MSG_MOVETOTOPMOST   0x00F2 
#define MSG_SETACTIVEMAIN   0x00F3
#define MSG_GETACTIVEMAIN   0x00F4
#define MSG_SHOWMAINWIN     0x00F5
#define MSG_HIDEMAINWIN     0x00F6
#define MSG_MOVEMAINWIN     0x00F7
#define MSG_SETCAPTURE      0x00F8
#define MSG_GETCAPTURE      0x00F9

#define MSG_ENDTRACKMENU    0x00FA
#define MSG_TRACKPOPUPMENU  0x00FB
#define MSG_CLOSEMENU       0x00FC
#define MSG_SCROLLMAINWIN   0x00FD
#define MSG_CARET_CREATE    0x00FE
#define MSG_CARET_DESTROY   0x00FF

#define MSG_ENABLEMAINWIN   0x0100
#define MSG_ISENABLED       0x0101

#define MSG_SETWINCURSOR    0x0102

#define MSG_GETNEXTMAINWIN  0x0103

#define MSG_SHOWGLOBALCTRL  0x010A
#define MSG_HIDEGLOBALCTRL  0x010B

typedef struct _DRAGINFO {
    int     location;
    int     init_x, init_y;
} DRAGINFO;

/* client to server; wParam: hwnd, lParam: DRAGINFO. */
#define MSG_STARTDRAGWIN    0x010C
/* client to server; wParam: hwnd, lParam: 0L. */
#define MSG_CANCELDRAGWIN   0x010D

#define MSG_CHANGECAPTION   0x010E

#define MSG_LASTWINDOWMSG   0x010F

    /** @} end of window_msgs */

    /**
     * \defgroup ctrl_msgs Dialog and control messages
     * @{
     */

/* Group 8 from 0x0120 to 0x013F, the dialog and control messages. */
#define MSG_FIRSTCONTROLMSG 0x0120

/**
 * \def MSG_COMMAND
 * \brief The command message, indicates a notification message from child 
 *        window, or the user has selected a menu item.
 *
 * This message sent to the window when the user has selected a menu item, or
 * a child window has sent a notification message to the parent.
 *
 * \code
 * MSG_COMMAND
 * int id = LOWORD(wParam);
 * int code = HIWORD(wParam);
 * HWND hwnd = (HWND)lParam;
 * \endcode
 *
 * \param id The identifier of the menu item or the child window.
 * \param code The notification code.
 * \param hwnd The handle to the control.
 *
 * \sa NotifyParent
 */
#define MSG_COMMAND         0x0120

/**
 * \def MSG_SYSCOMMAND
 * \brief The system command message.
 */
#define MSG_SYSCOMMAND      0x0121

/**
 * \def MSG_GETDLGCODE
 * \brief Get dialog code.
 */
#define MSG_GETDLGCODE      0x0122

/**
 * \def MSG_INITDIALOG
 * \brief Ready to initialize the controls in a dialog box.
 *
 * This message is sent to the dialog in order that you can initialize the 
 * controls in the dialog box.
 *
 * \code
 * MSG_INITDIALOG
 * HWND focus_hwnd = (HWND)wParam;
 * LPARAM lparam = (LPARAM)lParam;
 * \endcode
 *
 * \param focus_hwnd The handle to the control which will gain the input focus.
 * \param lparam The parameter passed into the dialog box 
 *        through \a DialogBoxIndirectParam function.
 *
 * \return Returns non-zero value to set the input focus to \a focus_hwnd, 
 *         else do not set focus.
 *
 * \sa DialogBoxIndirectParam
 *
 * Example:
 *
 * \include msg_initdialog.c
 */
#define MSG_INITDIALOG      0x0123

/*Reserved*/
#define MSG_NEXTDLGCTRL     0x0124
#define MSG_ENTERIDLE       0x0125

/**
 * \def MSG_DLG_GETDEFID
 * \brief Get default push button ID first.
 */
#define MSG_DLG_GETDEFID    0x0126

/**
 * \def MSG_DLG_SETDEFID
 * \brief Set default push button ID first.
 */
#define MSG_DLG_SETDEFID    0x0127

/* #define MSG_DLG_REPOSITION  0x0128 */

/**
 * \def MSG_ISDIALOG
 * \brief Sends to a window to query whether the window is a dialog window.
 *
 * \note This is a asynchronical message.
 */
#define MSG_ISDIALOG        0x0128

/**
 * \def MSG_INITPAGE
 * \brief Ready to initialize the controls in a property page.
 *
 * This message is sent to the page in order that you can initialize the 
 * controls in the page.
 *
 * \code
 * MSG_INITPAGE
 * DWORD add_data = (DWORD)lParam;
 * \endcode
 *
 * \param add_data The additional data in DLGTEMPLATE structure passed 
 *        through PSM_ADDPAGE message.
 *
 * \sa ctrl_propsheet
 */
#define MSG_INITPAGE        0x0129

/**
 * \def MSG_SHOWPAGE
 * \brief Indicates the page will be shown or hidden.
 *
 * This message is sent to the page when the page will be shown or hidden.
 *
 * \code
 * MSG_SHOWPAGE
 * HWND focus_hwnd = (HWND)wParam;
 * int show_cmd = (int)lParam;
 * \endcode
 *
 * \param focus_hwnd The handle to the child which will gain the input focus 
 *        if showing the page.
 * \param show_cmd The show command, can be one of the following values:
 *      - SW_SHOW\n The page will be shown.
 *      - SW_HIDE\n The page will be hidden.
 *
 * \sa ctrl_propsheet
 */
#define MSG_SHOWPAGE        0x012A

/**
 * \def MSG_SHEETCMD
 * \brief Indicates that a PSM_SHEETCMD message had been sent to the 
 * PropertySheet control.
 *
 * This message is sent to the property page when the property sheet contains 
 * the page received the PSM_SHEETCMD message.
 *
 * \code
 * MSG_SHEETCMD
 * WPARAM param1 = wParam;
 * LPARAM param2 = lParam;
 * \endcode
 *
 * \param param1 The wParam of PSM_SHEETCMD message.
 * \param param2 The lParam of PSM_SHEETCMD message.
 *
 * \sa ctrl_propsheet, PSM_SHEETCMD
 */
#define MSG_SHEETCMD        0x012B

/**
 * \def MSG_INITCONTAINER
 * \brief This message will be sent to the container window procedure after
 * the container window is created.
 *
 * This message is sent to the container in order that you can initialize the 
 * controls in the container.
 *
 * \code
 * MSG_INITCONTAINER
 * DWORD add_data = (DWORD)lParam;
 * \endcode
 *
 * \param add_data The additional data in \a CONTAINERINFO structure passed 
 *        through the argument of \a dwAddData when creating the ScrollView 
 *        control by calling CreateWindowEx.
 *
 * \sa ctrl_scrollview, CONTAINERINFO
 */
#define MSG_INITCONTAINER   0x012C

/**
 * \def MSG_SVCONTCMD
 * \brief This message will be sent to the parent of the ScrollView control 
 *        when the container of the ScrollView control reveived a MSG_COMMAND
 *        message.
 *
 * This message will be sent to the parent of the ScrollView when
 * the container of the ScrollView control reveived a MSG_COMMAND message.
 * Generally, the notification of the child control in the container will be
 * sent via MSG_COMMAND to the container. If you have not defined your own
 * window procedure for the container, this message gives a chance for the 
 * parent of the ScrollView control to handle the notifications come from 
 * the controls in the container.
 *
 * Note that you can also define your window procedure for the container, and
 * handle the notification from the child control in this procedure.
 *
 * \code
 * MSG_SVCONTCMD
 * WPARAM param1 = wParam;
 * WPARAM param2 = lParam;
 * \endcode
 *
 * \param param1 The wParam of MSG_COMMAND message.
 * \param param2 The lParam of MSG_COMMAND message.
 *
 * \sa ctrl_scrollview, MSG_COMMAND
 */
#define MSG_SVCONTCMD       0x012D

/**
 * \def MSG_FREEZECTRL
 * \brief You can send this message to freeze or thaw the paint action of 
 * the control.
 *
 * \code
 * MSG_FREEZECTRL
 * BOOL bFrozen;
 *
 * wParam = bFrozen
 * lParam = 0;
 *
 * \endcode
 *
 * \note Note that implemented only in scrollview and listview.
 *
 * \param bFrozen to freeze or to thaw.
 */
#define MSG_FREEZECTRL      0x012E

/**
 * \def MSG_FONTCHANGING
 * \brief Indicates the user is trying to change the font of the window.
 *
 * This message is sent to the window when the user is trying to change
 * the font of the window by calling \a SetWindowFont.
 * If you return non-zero after handling this message, \a SetWindowFont
 * will return immediately, i.e., the default window font will not change.
 *
 * \code
 * MSG_FONTCHANGING
 * PLOGFONT log_font = (PLOGFONT)lParam;
 * \endcode
 *
 * \param log_font The pointer to the new window logical font.
 *
 * \sa SetWindowFont
 */
#define MSG_FONTCHANGING    0x0130


/**
 * \def MSG_FONTCHANGED
 * \brief Indicates the window font has been changed.
 *
 * This message is sent to the window after the window font has changed.
 * Some window should be repainted to reflect the new window font.
 */
#define MSG_FONTCHANGED     0x0131

/**
 * \def MSG_GETTEXTLENGTH
 * \brief Sent to the control to get the length of the text.
 *
 * This message is sent to the control when you calling \a GetWindowTextLength
 * function to get the lenght of the text.
 *
 * \code
 * MSG_GETTEXTLENGTH
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The length of the text.
 *
 * \sa GetWindowTextLength
 */
#define MSG_GETTEXTLENGTH   0x0132

/**
 * \def MSG_GETTEXT
 * \brief Sent to the control to get the text.
 *
 * This message is sent to the control when you calling \a GetWindowText
 * function to get the text.
 *
 * \code
 * MSG_GETTEXT
 * int max_len;
 * char* text_buf;
 *
 * wParam = (WPARAM)max_len;
 * lParam = (LPARAM)text_buf;
 * \endcode
 *
 * \param max_len The maximal number of characters can be copied to the buffer.
 * \param text_buf The pointer to a buffer receives the text.
 * \return The length of the window text string.
 *
 * \sa GetWindowText
 */
#define MSG_GETTEXT         0x0133

/**
 * \def MSG_SETTEXT
 * \brief Sent to the control to set the text.
 *
 * This message is sent to the control when you calling \a SetWindowText
 * function to set the text.
 *
 * \code
 * MSG_GETTEXT
 * char* text_buf;
 *
 * wParam = 0;
 * lParam = (LPARAM)text_buf;
 * \endcode
 *
 * \param text_buf The pointer to a buffer contains the text.
 * \return The length of the text string.
 *
 * \sa SetWindowText
 */
#define MSG_SETTEXT         0x0134

/**
 * \def MSG_ENABLE
 * \brief Indicates the window is disabled/enabled.
 *
 * This message is sent to the window if the window has been disabled or enabled.
 *
 * \code
 * MSG_ENABLE
 * BOOL enabled = (BOOL)wParam;
 * \endcode
 *
 * \param enabled Indicates whether the window was disabled or enabled.
 */
#define MSG_ENABLE          0x0135

#define MSG_LASTCONTROLMSG  0x013F

    /** @} end of ctrl_msgs */

    /**
     * \defgroup system_msgs System messages
     * @{
     */

/* Group 9 from 0x0140 to 0x016F, the system messages. */
#define MSG_FIRSTSYSTEMMSG  0x0140

#define MSG_QUIT            0x0140

/**
 * \def MSG_IDLE
 * \brief Indicates the system enters idle loop.
 *
 * This message is sent to the all main windows when the system
 * enters idle loop.
 */
#define MSG_IDLE            0x0142

#define MSG_TIMEOUT         0x0143

/**
 * \def MSG_TIMER
 * \brief Indicates a timer has expired.
 *
 * This message is sent to the window when a timer expired.
 *
 * \code
 * MSG_TIMER
 * int timer_id = (int)wParam;
 * DWORD tick_count = (DWORD)lParam;
 * \endcode
 *
 * \param timer_id The identifier of the timer has expired.
 * \param tick_count The tick count when the timer had expired.
 */
#define MSG_TIMER           0x0144

#define MSG_CARETBLINK      0x0145

#ifdef _LITE_VERSION

/**
 * \def MSG_FDEVENT
 * \brief Indicates an event of registered file descriptor occurred.
 *
 * You can use \a RegisterListenFD to register a file desciptor to 
 * MiniGUI-Processes for listening.
 *
 * When there is a read/write/except event on the fd, MiniGUI
 * will post a MSG_FDEVENT message with \a wParam being equal to
 * MAKELONG (fd, type), and the \a lParam being set to be the context
 * to the target window.
 *
 * \code
 * MSG_FDEVENT
 * int fd = LOWORD(wParam);
 * int type = HIWORD(wParam);
 * void* context = (void*)lParam;
 * \endcode
 *
 * \param fd The listened file descriptor.
 * \param type The event type.
 * \param context A context value.
 *
 * \note Only available on MiniGUI-Processes.
 *
 * \sa RegisterListenFD
 */
#define MSG_FDEVENT         0x0146

/**
 * \def MSG_SRVNOTIFY
 * \brief Indicates a notification from the server of MiniGUI-Processes.
 *
 * This message will be broadcasted to all of the main windows
 * in a client process when the client receives a MSG_SRVNOTIFY message
 * from the server.
 *
 * The server, i.e. 'mginit' defines the meaning of two parameters of 
 * this message.
 *
 * \note Only available on MiniGUI-Processes.
 */
#define MSG_SRVNOTIFY       0x0147

#ifndef _STAND_ALONE
    #define MSG_UPDATECLIWIN    0x0148
#endif

#endif /* _LITE_VERSION */

/**
 * \def MSG_DOESNEEDIME
 * \brief Sends to a window to query whether the window needs to open 
 *        IME window.
 *
 * The system will send this message when the window gain the input focus
 * to determine whether the window needs to open IME window.
 *
 * The application should handle this message and return TRUE when
 * the window need IME window.  Default window procedure returns FALSE.
 *
 * \note This is a asynchronical message.
 */
#define MSG_DOESNEEDIME     0x0150

/*Internal use*/
#define MSG_IME_REGISTER    0x0151
#define MSG_IME_UNREGISTER  0x0152
#define MSG_IME_OPEN        0x0153
#define MSG_IME_CLOSE       0x0154
#define MSG_IME_SETSTATUS   0x0156
#define MSG_IME_GETSTATUS   0x0157
    #define IS_ENABLE       1
    #define IS_FULLCHAR     2
    #define IS_FULLPUNC     3
    #define IS_METHOD       4
#define MSG_IME_SETTARGET   0x0158
#define MSG_IME_GETTARGET   0x0159

#define MSG_SHOWMENU        0x0160
#define MSG_HIDEMENU        0x0161

#define MSG_ADDTIMER        0x0162
#define MSG_REMOVETIMER     0x0163
#define MSG_RESETTIMER      0x0164

#define MSG_WINDOWCHANGED   0x0165

#define MSG_BROADCASTMSG    0x0166

#define MSG_REGISTERWNDCLASS    0x0167
#define MSG_UNREGISTERWNDCLASS  0x0168
#define MSG_NEWCTRLINSTANCE     0x0169
#define MSG_REMOVECTRLINSTANCE  0x016A
#define MSG_GETCTRLCLASSINFO    0x016B
#define MSG_CTRLCLASSDATAOP     0x016C
    #define CCDOP_GETCCI        0x01
    #define CCDOP_SETCCI        0x02

#define MSG_REGISTERKEYHOOK     0x016D
#define MSG_REGISTERMOUSEHOOK   0x016E

#define MSG_LASTSYSTEMMSG   0x016F

    /** @} end of system_msgs */

    /**
     * \defgroup menu_msgs Internal menu messages
     * @{
     */

/* Group 10 from 0x0170 to 0x018F, the menu messages */
#define MSG_FIRSTMENUMSG    0x0170

/*Internal use*/
#define MSG_INITMENU        0x0170
#define MSG_INITMENUPOPUP   0x0171
#define MSG_MENUSELECT      0x0172
#define MSG_MENUCHAR        0x0173
#define MSG_ENTERMENULOOP   0x0174
#define MSG_EXITMENULOOP    0x0175
#define MSG_CONTEXTMENU     0x0176
#define MSG_NEXTMENU        0x0177

#define MSG_LASTMENUMSG     0x018F

    /** @} end of menu_msgs */

    /**
     * \defgroup user_msgs User-defined  messages
     * @{
     */

#define MSG_FIRSTUSERMSG    0x0800


/**
 * \def MSG_USER
 * \brief The first user-defined message.
 *
 * MiniGUI reserved the range from 0x0800 to 0xEFFF for user-defined messages.
 * MSG_USER is the first user-defined message you can use it by your own.
 */
#define MSG_USER            0x0800

#define MOUSE_UP         0x0001  //向上滑动
#define MOUSE_DOWN       0x0002  //向下滑动
#define MSG_MOUSE_SCROLL     (MSG_USER+201)  //滚动w= 方向+速度, l=xy

#define MSG_LASTUSERMSG     0xEFFF

    /** @} end of user_msgs */

    /** @} end of msgs */

    /**
     * \addtogroup fns Functions
     * @{
     */

    /**
     * \defgroup msg_fns Message functions
     * @{
     */

    /**
     * \defgroup msg_pass_fns Message passing functions
     * @{
     */

/**
 * The message structure.
 * \sa GetMessage, PostMessage, msgs
 */
  /*add  by  jason 20110624*/ 
#define MSG_IME_LBD_SWITCH          MSG_USER + 100  
#define MSG_IME_LBD_HIDE            MSG_USER + 101  
#define MSG_IME_LBD_SHOW            MSG_USER + 102 
#define MSG_EDIT_SETTEXT_END        MSG_USER + 104    
#define MSG_SETSLEDITFOCUS          MSG_USER + 103



#define WS_EX_EDIT_CHARINPUT   0x00000010L
#define WS_EX_EDIT_ALPINPUT    0x00000040L
#define WS_EX_EDIT_MINALPINPUT   0x00000200L
#define WS_EX_EDIT_NUMINPUT    0x00008000L
#define WS_EX_EDIT_HZINPUT     0x00080000L
#define WS_EX_EDIT_T           0x00004000L
#define WS_EX_EDIT_INPUTMASK   0x00088250L


typedef struct _MSG
{
	/** The handle to the window which receives this message. */
    HWND             hwnd;
	/** The message identifier. */
    int              message;
    /** The first parameter of the message (32-bit integer). */
    WPARAM           wParam;
    /** The second parameter of the message (32-bit integer). */
    LPARAM           lParam;
    /** Time*/
    unsigned int     time;
#ifndef _LITE_VERSION
    /** Addtional data*/
    void*            pAdd;
#endif
} MSG;
typedef MSG* PMSG;

#define QS_NOTIFYMSG        0x10000000
#ifndef _LITE_VERSION
  #define QS_SYNCMSG        0x20000000
#else
  #define QS_DESKTIMER      0x20000000
#endif
#define QS_POSTMSG          0x40000000
#define QS_QUIT             0x80000000
#define QS_INPUT            0x01000000
#define QS_PAINT            0x02000000
#define QS_TIMER            0x0000FFFF
#define QS_EMPTY            0x00000000

/**
 * \def PM_NOREMOVE
 *
 * \sa PeekMessage PeekMessageEx
 */
#define PM_NOREMOVE     0x0000

/**
 * \def PM_REMOVE
 *
 * \sa PeekMessage PeekMessageEx PeekPostMessage
 */
#define PM_REMOVE       0x0001

/**
 * \def PM_NOYIELD
 *
 * \sa PeekMessage PeekMessageEx PeekPostMessage
 */
#define PM_NOYIELD      0x0002

/**
 * \fn BOOL PeekMessageEx (PMSG pMsg, HWND hWnd, \
 *               int iMsgFilterMin, int iMsgFilterMax, \
 *               BOOL bWait, UINT uRemoveMsg)
 * \brief Peeks a message from the message queue of a main window.
 *
 * This functions peek a message from the message queue of the window \a hWnd;
 * if \a bWait is TRUE, it will wait for the message, else return immediatly.
 *
 * \param pMsg Pointer to the result message.
 * \param hWnd The handle to the window.
 * \param iMsgFilterMin The min identifier of the message that should be peeked.
 * \param iMsgFilterMax The max identifier of the message that should be peeked.
 * \param bWait Whether to wait for a message.
 * \param uRemoveMsg Whether remove the message from the message queue.
 *      Should be the following values:
 *      - PM_NOREMOVE\n
 *        Leave it in the message queue.
 *      - PM_REMOVE
 *        Remove it from the message queue.
 *      - PM_NOYIELD
 *        Nouse now.
 *        
 * \return TRUE if there is a message peeked, or FALSE.
 *
 * \sa GetMessage, PeekPostMessage, HavePendingMessage, PostMessage
 */               
MG_EXPORT BOOL GUIAPI PeekMessageEx (PMSG pMsg, HWND hWnd, 
                int iMsgFilterMin, int iMsgFilterMax, 
                BOOL bWait, UINT uRemoveMsg);

/**
 * \fn BOOL GetMessage (PMSG pMsg, HWND hMainWnd)
 * \brief Gets a message from the message queue of a main window.
 *
 * This function gets a message from the message queue of the main window 
 * \a hMainWnd, and returns until there is a message in the message queue.
 *
 * \param pMsg Pointer to the result message.
 * \param hMainWnd Handle to the window.
 *
 * \return FALSE on MSG_QUIT have been found or on error, else gets a message.
 *
 * \sa HavePendingMessage, PostQuitMessage, MSG
 *
 * Example:
 *
 * \include getmessage.c
 */
static inline BOOL GUIAPI GetMessage (PMSG pMsg, HWND hWnd)
{
    return PeekMessageEx (pMsg, hWnd, 0, 0, TRUE, PM_REMOVE);
}

/**
 * \fn BOOL WaitMessage (PMSG pMsg, HWND hMainWnd)
 * \brief Waits for a message from the message queue of a main window.
 *
 * This function waits for a message from the message queue of the main 
 * window \a hMainWnd, and returns until there is a message in the message 
 * queue. Unlike \a GetMessage, this function does not remove the message 
 * from the message queue.
 *
 * \param pMsg Pointer to the result message.
 * \param hMainWnd Handle to the window.
 *
 * \return 0 on MSG_QUIT have been found, else gets a message.
 *
 * \sa HavePendingMessage, PostQuitMessage, MSG
 */
MG_EXPORT BOOL GUIAPI WaitMessage (PMSG pMsg, HWND hMainWnd);

/**
 * \fn BOOL HavePendingMessage (HWND hMainWnd)
 * \brief Checks if there is any pending message in the message queue of 
 * a main window.
 *
 * This function checks whether there is any pending message in the
 * message queue of the main window \a hMainWnd.
 *
 * \param hMainWnd The handle to the main window.
 *
 * \return TRUE for pending message, FALSE for empty message queue.
 *
 * \sa GetMessage, MSG
 */
MG_EXPORT BOOL GUIAPI HavePendingMessage (HWND hMainWnd);

/**
 * \fn BOOL PeekMessage (PMSG pMsg, HWND hWnd, \
 *               int iMsgFilterMin, int iMsgFilterMax, UINT uRemoveMsg)
 * \brief Peeks a message from the message queue of a main window
 *
 * This functions peek a message from the message queue of the window \a hWnd 
 * and returns immediatly. Unlike \a GetMessage, this function does not wait 
 * for a message.
 *
 * \param pMsg Pointer to the result message.
 * \param hWnd The handle to the window.
 * \param iMsgFilterMin The min identifier of the message that should be peeked.
 * \param iMsgFilterMax The max identifier of the message that should be peeked.
 * \param uRemoveMsg Whether remove the message from the message queue.
 *      Should be the following values:
 *      - PM_NOREMOVE\n
 *        Leave it in the message queue.
 *      - PM_REMOVE
 *        Remove it from the message queue.
 *      - PM_NOYIELD
 *        Nouse now.
 *        
 * \return TRUE if there is a message peeked, or FALSE.
 *
 * \sa GetMessage, PeekPostMessage, HavePendingMessage, PostMessage
 */               
static inline BOOL GUIAPI PeekMessage (PMSG pMsg, HWND hWnd, int iMsgFilterMin, 
                         int iMsgFilterMax, UINT uRemoveMsg)
{
    return PeekMessageEx (pMsg, hWnd, iMsgFilterMin, iMsgFilterMax, 
                           FALSE, uRemoveMsg);
}

/**
 * \fn BOOL PeekPostMessage (PMSG pMsg, HWND hWnd, \
                int iMsgFilterMin, int iMsgFilterMax, UINT uRemoveMsg)
 * \brief Peeks a post message from the message queue of a main window
 *
 * This functions peek a message from the message queue of the window \a hWnd 
 * and returns immediatly. Unlike \a PeekMessage, this function only peek a 
 * post message.
 *
 * \param pMsg Pointer to the result message.
 * \param hWnd The handle to the window.
 * \param iMsgFilterMin The min identifier of the message that should be peeked.
 * \param iMsgFilterMax The max identifier of the message that should be peeked.
 * \param uRemoveMsg Whether remove the message from the message queue.
 *      Should be the following values:
 *      - PM_NOREMOVE\n
 *        Leave it in the message queue.
 *      - PM_REMOVE
 *        Remove it from the message queue.
 *      - PM_NOYIELD
 *        Nouse now.
 *        
 * \return TRUE if there is a message peeked, or FALSE.
 *
 * \sa GetMessage, PeekMessage, HavePendingMessage, PostMessage
 */               
MG_EXPORT BOOL GUIAPI PeekPostMessage (PMSG pMsg, HWND hWnd, int iMsgFilterMin, 
                int iMsgFilterMax, UINT uRemoveMsg);

/**
 * \fn int PostMessage (HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam)
 * \brief Posts a message into the message queue of a window and returns 
 * immediatly.
 *
 * This functions posts a message into the message queue of the window \a hWnd 
 * and returns immediately.
 *
 * \param hWnd The handle to the window.
 * \param iMsg The identifier of the message.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \return ERR_OK on success, < 0 on errors.
 *
 * \retval ERR_OK Post message successfully.
 * \retval ERR_QUEUE_FULL The message queue is full.
 * \retval ERR_INV_HWND Invalid window handle.
 *
 * \sa SendMessage
 */               
MG_EXPORT int GUIAPI PostMessage (HWND hWnd, int iMsg, 
                WPARAM wParam, LPARAM lParam);

/**
 * \fn int SendMessage (HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam)
 * \brief Sends a message to a window.
 *
 * This function sends a message to the window \a hWnd, and will return 
 * until the message-handling process returns.
 *
 * \param hWnd The handle to the window.
 * \param iMsg The identifier of the message.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 * \return The return value of the message handler.
 *
 * \sa PostMessage
 */
MG_EXPORT int GUIAPI SendMessage (HWND hWnd, int iMsg, 
                WPARAM wParam, LPARAM lParam);

/**
 * \fn void GUIAPI SetAutoRepeatMessage (HWND hwnd, int msg, \
            WPARAM wParam, LPARAM lParam)
 * \brief Sets the auto-repeat message.
 *
 * This function sets the auto-repeat message. When the default message
 * procedure receives an MSG_IDLE message, the default handler will send
 * the auto-repeat message to the target window as a notification message.
 *
 * \param hwnd The handle to the target window. Set it to zero
 *        to disable the auto-repeat message.
 * \param msg The identifier of the auto-repeat message.
 * \param wParam The first parameter of the auto-repeat message.
 * \param lParam The second parameter of the auto-repeat message.
 */
MG_EXPORT void GUIAPI SetAutoRepeatMessage (HWND hwnd, int msg, 
                WPARAM wParam, LPARAM lParam);

#ifdef _LITE_VERSION

/**
 * \def SendAsyncMessage
 * \brief Is an alias of \a SendMessage for MiniGUI-Processes 
 *        and MiniGUI-Standalone.
 * \sa SendMessage
 */ 
#define SendAsyncMessage SendMessage

#ifndef _STAND_ALONE

#define CLIENTS_TOPMOST          -1
#define CLIENTS_ALL              -2
#define CLIENTS_EXCEPT_TOPMOST   -3
#define CLIENT_ACTIVE            -4

/**
 * \fn int Send2Client (MSG* msg, int cli)
 * \brief Sends a message to a client.
 *
 * This function sends a message to the specified client \a cli.
 *
 * \param msg The pointer to the message.
 * \param cli Either be the identifier of the targe client or one of the 
 *        following values:
 *      - CLIENT_ACTIVE\n
 *        The current active client on the topmost layer.
 *      - CLIENTS_TOPMOST\n
 *        All clients in the topmost layer.
 *      - CLIENTS_EXCEPT_TOPMOST\n
 *        All clients except clients in the topmost layer.
 *      - CLIENTS_ALL\n
 *        All clients.
 *
 * \return The number of bytes sent, < 0 on error.
 *
 * \retval SOCKERR_OK       Read data successfully.
 * \retval SOCKERR_IO       There are some I/O errors occurred.
 * \retval SOCKERR_CLOSED   The socket has been closed by the peer.
 * \retval SOCKERR_INVARG   You passed invalid arguments.
 *
 * \note This function is only defined for MiniGUI-Processes, and
 *       can be called only by the server, i.e. \a mginit.
 *
 * \sa Send2TopMostClients, Send2ActiveWindow
 */
int GUIAPI Send2Client (MSG* msg, int cli);

/**
 * \fn BOOL Send2TopMostClients (int iMsg, WPARAM wParam, LPARAM lParam)
 * \brief Sends a message to all clients in the topmost layer.
 *
 * This function sends the message specified by (\a iMsg, \a wParam, \a lParam) 
 * to all clients in the topmost layer.
 *
 * \param iMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 * \return TRUE on success, FALSE on error.
 *
 * \note This function is only defined for MiniGUI-Processes, and
 *       can be called only by the server, i.e. \a mginit.
 *
 * \note The message will be sent to the virtual desktop of the target client.
 *
 * \sa Send2Client, Send2ActiveWindow
 */
BOOL GUIAPI Send2TopMostClients (int iMsg, WPARAM wParam, LPARAM lParam);

/**
 * \fn BOOL Send2ActiveWindow (const MG_Layer* layer, \
                 int iMsg, WPARAM wParam, LPARAM lParam);
 * \brief Sends a message to the active window in layer.
 *
 * This function sends the message specified by (\a iMsg, \a wParam, \a lParam) 
 * to the current active window in the specific layer (\a layer).
 *
 * \param layer The pointer to the layer.
 * \param iMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \note This function is only defined for MiniGUI-Processes, and
 *       can be called only by the server, i.e. \a mginit.
 *
 * \sa Send2Client
 */
BOOL GUIAPI Send2ActiveWindow (const MG_Layer* layer, 
                 int iMsg, WPARAM wParam, LPARAM lParam);

#endif /* !_STAND_ALONE */

#else /* _LITE_VERSION */

/**
 * \fn int PostSyncMessage (HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam)
 * \brief Posts a synchronical message to a window which is in different thread.
 *
 * This function posts the synchronical message specified by 
 * (\a iMsg, \a wParam, \a lParam) to the window \a hWnd which 
 * is in different thread. This function will return until 
 * the message is handled by the window procedure.
 *
 * \note The destination window must belong to other thread.
 *
 * \param hWnd The handle to the window.
 * \param iMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \return The return value of the message handler.
 *
 * \sa SendMessage
 */
int GUIAPI PostSyncMessage (HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam);

/**
 * \fn int SendAsyncMessage (HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam)
 * \brief Sends an asynchronical message to a window.
 *
 * This function sends the asynchronical message specified by 
 * (\a iMsg, \a wParam, \a lParam) to the window \a hWnd 
 * which is in different thread. This function calls 
 * the window procedure immediately, so it is very dangerous.
 *
 * \param hWnd The handle to the window.
 * \param iMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \return The return value of the message handler.
 *
 * \note This function may corrupt your data.
 *
 * \sa PostSyncMessage
 */
int GUIAPI SendAsyncMessage (HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam);
#endif

/**
 * \fn int SendNotifyMessage (HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam)
 * \brief Sends a notification message to a window.
 *
 * This function sends the notification message specified by 
 * (\a iMsg, \a wParam, \a lParam) to the window \a hWnd. This function 
 * puts the notication message in the message queue and then returns 
 * immediately.
 *
 * \param hWnd The handle to the window.
 * \param iMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \return 0 if all OK, < 0 on error.
 *
 * \sa SendMessage, PostMessage
 */
MG_EXPORT int GUIAPI SendNotifyMessage (HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam);

/**
 * \fn int BroadcastMessage (int iMsg, WPARAM wParam, LPARAM lParam)
 * \brief Broadcasts a message to all main window on the desktop.
 *
 * This function posts the message specified by (\a iMsg, \a wParam, \a lParam)
 * to all the main windows on the desktop.
 *
 * \param iMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 * \return 0 if all OK, < 0 on error.
 *
 * \sa PostMessage
 */
MG_EXPORT int GUIAPI BroadcastMessage (int iMsg, WPARAM wParam, LPARAM lParam);

/**
 * \fn int PostQuitMessage (HWND hWnd)
 * \brief Puts a MSG_QUIT message into the message queue of a main window.
 *
 * This function puts a MSG_QUIT message into the message queue of the
 * main window \a hWnd. The next call to \a GetMessage will return 0.
 *
 * \param hWnd The handle to the main window.
 * \return 0 if all OK, < 0 on error.
 *
 * \sa GetMessage
 */
MG_EXPORT int GUIAPI PostQuitMessage (HWND hWnd);

#define KBD_LAYOUT_DEFAULT  "default"
#define KBD_LAYOUT_FRPC     "frpc"
#define KBD_LAYOUT_FR       "fr"
#define KBD_LAYOUT_DE       "de"
#define KBD_LAYOUT_DELATIN1 "delatin1"
#define KBD_LAYOUT_IT       "it"
#define KBD_LAYOUT_ES       "es"
#define KBD_LAYOUT_ESCP850  "escp850"

/**
 * \fn BOOL SetKeyboardLayout (const char* kbd_layout)
 * \brief Sets a new keyboard layout.
 *
 * This function sets the keymaps to translate key scancodes to MSG_CHAR
 * or MSG_KEYSYM messages. The default keymaps is for US PC keyboard 
 * layout, you can call this function to set a different keyboard layout.
 * The argument of \a kbd_layout specifies the name of the keyboard layout.
 *
 * \param kbd_layout The keyboard layout name. It can be 
 *        one of the following values:
 *
 *      - KBD_LAYOUT_DEFAULT\n
 *        The default keyboard layout, i.e., US PC.
 *      - KBD_LAYOUT_FRPC\n
 *        The France PC keyboard layout.
 *      - KBD_LAYOUT_FR\n
 *        The France keyboard layout.
 *      - KBD_LAYOUT_DE\n
 *        The German keyboard layout.
 *      - KBD_LAYOUT_DELATIN1\n
 *        The German Latin1 keyboard layout.
 *      - KBD_LAYOUT_IT\n
 *        The Italian keyboard layout.
 *      - KBD_LAYOUT_ES\n
 *        The Spanish keyboard layout.
 *      - KBD_LAYOUT_ESCP850\n
 *        The Spanish CP850 keyboard layout.
 *
 * \return TRUE for success, otherwise FALSE.
 *
 * \sa TranslateMessage, MSG_CHAR, MSG_KEYSYM
 */
MG_EXPORT BOOL GUIAPI SetKeyboardLayout (const char* kbd_layout);

/**
 * \fn BOOL TranslateMessage (PMSG pMsg)
 * \brief Translates key down and key up messages to MSG_CHAR 
 *        message and post it into the message queue.
 *
 * This function translates key down and key up message to an MSG_CHAR 
 * message or some MSG_KEYSYM messages, and send the message(s) to 
 * the window procedure as a notification message. If the message is 
 * not a key message, this function does nothing.
 *
 * The behavior of this function is inflected by the current
 * keyboard layout. The default keyboard layout is US PC keyboard, but
 * you can call \a SetKeyboardLayout function to set a different keyboard
 * layout.
 *
 * \param pMsg The pointer of message.
 * \return A boolean indicates whether the message is a key message.
 *
 * \sa SetKeyboardLayout, MSG_CHAR, MSG_KEYSYM
 */
MG_EXPORT BOOL GUIAPI TranslateMessage (PMSG pMsg);

/**
 * \fn BOOL GUIAPI TranslateKeyMsgToChar (int message, \
                WPARAM wParam, LPARAM lParam, WORD *ch)
 * \brief Translates a key down and key up message to a corresponding character.
 *
 * This function translates a key down and key up message to a character.
 * If the message is not a key message, this function does nothing.
 *
 * The behavior of this function is inflected by the current
 * keyboard layout. The default keyboard layout is US PC keyboard, but
 * you can call \a SetKeyboardLayout function to set a different keyboard
 * layout.
 *
 * \param message The type of message.
 * \param wParam Message parameter.
 * \param lParam Message parameter.
 * \param ch A string buffer for storing translated characters.
 *
 * \return A boolean indicates whether the message is a key message.
 *
 * \sa TranslateMessage
 */
MG_EXPORT BOOL GUIAPI TranslateKeyMsgToChar (int message, 
                WPARAM wParam, LPARAM lParam, WORD *ch);

/**
 * \fn int DispatchMessage (PMSG pMsg)
 * \brief Dispatches a message to the window's callback procedure.
 *
 * This function dispatches the message pointed to by \a pMsg to the
 * target window's callback procedure.
 *
 * \param pMsg The pointer to the message.
 * \return The return value of the message handler.
 *
 * \sa GetMessage
 *
 * Example:
 *
 * \include getmessage.c
 */
MG_EXPORT int GUIAPI DispatchMessage (PMSG pMsg);

/**
 * \fn int ThrowAwayMessages (HWND pMainWnd)
 * \brief Removes all messages in the message queue associated with a window.
 *
 * This function removes all messages which are associated with 
 * the specified window \a pMainWnd.
 *
 * \param pMainWnd The handle to the window.
 * \return The number of thrown messages.
 *
 * \sa EmptyMessageQueue
 */
MG_EXPORT int GUIAPI ThrowAwayMessages (HWND pMainWnd);

#ifdef _LITE_VERSION

/**
 * \fn BOOL EmptyMessageQueue (HWND hWnd)
 * \brief Empties a message queue.
 *
 * This function empties the message queue of the main window \a hWnd.
 *
 * \param hWnd The handle to the main window.
 *
 * \return TRUE on all success, FALSE on error.
 *
 * \note Only defined for MiniGUI-Processes.
 *
 * \sa ThrowAwayMessages
 */
MG_EXPORT BOOL GUIAPI EmptyMessageQueue (HWND hWnd);

#endif

#ifdef _MSG_STRING

/**
 * \fn const char* GUIAPI Message2Str (int message)
 * \brief Translates a message identifier to the message string.
 *
 * This function returns the message string of the message identifier \a message.
 * E.g. this function will return the string of "MSG_CHAR" for MSG_CHAR message.
 * 
 * \param message The message identifier.
 * \return The message string.
 *
 * \note Only available if defined _MSG_STRING.
 *
 * \sa PrintMessage
 */
MG_EXPORT const char* GUIAPI Message2Str (int message);

/**
 * \fn void GUIAPI PrintMessage (FILE* fp, HWND hWnd, \
                int iMsg, WPARAM wParam, LPARAM lParam)
 * \brief Prints a message in readable string form to a stdio stream.
 *
 * This function prints the message specified by (\a iMsg, \a wParam, \a lParam)
 * in readable string form to the stdio stream \a fp.
 *
 * \param fp The pointer to the FILE object.
 * \param hWnd The target window of the message.
 * \param iMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \sa Message2Str
 */
MG_EXPORT void GUIAPI PrintMessage (FILE* fp, HWND hWnd, 
                int iMsg, WPARAM wParam, LPARAM lParam);

#endif

    /** @} end of msg_pass_fns */

    /**
     * \defgroup msg_hook_fns Message or event hook functions
     * @{
     */

/**
 * \def HOOK_GOON
 * \sa SRVEVTHOOK  RegisterKeyHookWindow RegisterMouseHookWindow
 */
#define HOOK_GOON       0

/**
 * \def HOOK_STOP
 * \sa SRVEVTHOOK  RegisterKeyHookWindow RegisterMouseHookWindow
 */
#define HOOK_STOP       1

#if !defined (_LITE_VERSION) || defined (_STAND_ALONE)

/**
 * \var typedef int (* MSGHOOK)(void* context, HWND dst_wnd, \
 *                int msg, WPARAM wparam, LPARAM lparam)
 *
 * \brief Type of message hook function.
 */
typedef int (* MSGHOOK)(void* context, HWND dst_wnd, 
                int msg, WPARAM wparam, LPARAM lparam);

/**
 * Structure defines a message hook.
 */
typedef struct _HOOKINFO
{
    /** the context which will be passed to the hook function. */
    void* context;
    /** the pointer to the hook function. */
    MSGHOOK hook;
} HOOKINFO;

/**
 * \fn MSGHOOK GUIAPI RegisterKeyMsgHook (void* context, MSGHOOK hook)
 * \brief Registers a key message hook.
 *
 * This function registers a key message hook pointed to by \a hook.
 *
 * When the desktop receives a key message, it will send it to the hook first,
 * and passes the \a context value to the hook as the first argument.
 *
 * \param context The context value will be passed to the hook.
 * \param hook The hook. This function will unregister the old hook if 
 *        \a hook is NULL.
 * \return The old hook.
 *
 * \sa UnregisterHook, KEYMSGHOOK
 */
MG_EXPORT MSGHOOK GUIAPI RegisterKeyMsgHook (void* context, MSGHOOK hook);

/**
 * \fn MSGHOOK GUIAPI RegisterMouseMsgHook (void* context, MSGHOOK hook)
 * \brief Registers a mouse message hook.
 *
 * This function registers a mouse message hook pointed to by \a hook.
 *
 * When the desktop receives a mouse message, it will send it to the hook 
 * first, and passes the \a context value to the hook as the first argument.
 *
 * \param context The context value will be passed to the hook.
 * \param hook The hook. This function will unregister the old hook if 
 *        \a hook is NULL.
 * \return The old hook.
 *
 * \sa UnregisterHook, KEYMSGHOOK
 */
MG_EXPORT MSGHOOK GUIAPI RegisterMouseMsgHook (void* context, MSGHOOK hook);

#endif /* !_LITE_VERSION || _STAND_ALONE */

#ifdef _LITE_VERSION

/**
 * \var typedef int (* SRVEVTHOOK) (PMSG pMsg)
 * \brief The type of the event hook.
 *
 * You can call \a SetServerEventHook to set an event hook
 * in the server of the MiniGUI-Processes.
 *
 * If the event hook returns HOOK_GOON, \a mginit will continue to
 * handle the event, and send it to the active client.
 * If the hook returns HOOK_STOP, \a mginit will cancel normal handling.
 *
 * \sa SetServerEventHook
 */
typedef int (* SRVEVTHOOK) (PMSG pMsg);

/**
 * \fn void GUIAPI SetServerEventHook (SRVEVTHOOK SrvEvtHook)
 * \brief Sets an event hook in the server of MiniGUI-Processes.
 *
 * This function sets the event hook as \a SrvEvtHook in the server, 
 * i.e. mginit, of MiniGUI-Processes.
 *
 * \param SrvEvtHook The pointer to the hook, NULL to cancel the hook.
 *
 * \note Only defined for MiniGUI-Processes, and only can be used by the server.
 *
 * \sa SRVEVTHOOK
 */
MG_EXPORT SRVEVTHOOK GUIAPI SetServerEventHook (SRVEVTHOOK SrvEvtHook);

#ifndef _STAND_ALONE

/**
 * \fn HWND GUIAPI RegisterKeyHookWindow (HWND hwnd, DWORD flag)
 * \brief Registers a key message hook window.
 *
 * This function registers a key message hook pointed to by \a hook.
 *
 * When the desktop receives a key message, it will send it to the hook first,
 * and passes the \a context value to the hook as the first argument.
 *
 * \param hwnd The hook hwnd. This function will unregister the old hook if 
 *        \a hwnd is HWND_NULL.
 * \param flag Indicates whether stop or continue handling the hooked messages.
 *        HOOK_GOON to continue, HOOK_STOP to stop.
 *
 * \return The handle of old hook window.
 *
 * \note This function be be called by a client of MiniGUI-Processes.
 *       For the server, you can use SetServerEventHook.
 *
 * \sa RegisterMouseHookWindow, SetServerEventHook 
 */
MG_EXPORT HWND GUIAPI RegisterKeyHookWindow (HWND hwnd, DWORD flag);

/**
 * \fn HWND GUIAPI RegisterMouseHookWindow (HWND hwnd, DWORD flag)
 * \brief Registers a mouse message hook window.
 *
 * This function registers a mouse message hook pointed to by \a hook.
 *
 * When the desktop receives a mouse message, it will send it to the hook 
 * first, and passes the \a context value to the hook as the first argument.
 *
 * \param hwnd The hook hwnd. This function will unregister the old hook if 
 *        \a hook is HWND_NULL.
 * \param flag Indicates whether stop or continue handling the hooked messages.
 *        HOOK_GOON to continue, HOOK_STOP to stop.
 *
 * \return The handle of old hook window.
 *
 * \note This function be be called by a client of MiniGUI-Processes.
 *       For the server, you can use SetServerEventHook.
 *
 * \sa RegisterKeyHookWindow, SetServerEventHook 
 */
MG_EXPORT HWND GUIAPI RegisterMouseHookWindow (HWND hwnd, DWORD flag);

#endif /* !_STAND_ALONE */
#endif /* _LITE_VERSION */


    /** @} end of msg_hook_fns */

    /** @} end of msg_fns */

    /** @} end of fns */

    /**
     * \defgroup styles Window styles
     * @{
     */

/**
 * \def WS_NONE
 * \brief None style.
 */
#define WS_NONE             0x00000000L

#define WS_OVERLAPPED       0x00000000L   /*Not supported*/

/**
 * \def WS_ABSSCRPOS
 * \brief Creates a main window whose position is based on absolute 
 *        screen coordinates.
 */
#define WS_ABSSCRPOS        0x80000000L

/**
 * \def WS_CHILD
 * \brief Indicates the window is a child.
 */
#define WS_CHILD            0x40000000L

/**
 * \def WS_CAPTION
 * \brief Creates a main window with caption.
 */
#define WS_CAPTION          0x20000000L

/**
 * \def WS_SYSMENU
 * \brief Creates a main window with system menu.
 */
#define WS_SYSMENU          0x10000000L

/**
 * \def WS_VISIBLE
 * \brief Creates a window initially visible.
 */
#define WS_VISIBLE          0x08000000L

/**
 * \def WS_DISABLED
 * \brief Creates a window initially disabled.
 */
#define WS_DISABLED         0x04000000L

/* Main window states -- not supported so far */
#define WS_MINIMIZE         0x02000000L
#define WS_MAXIMIZE         0x01000000L

#define WS_DLGFRAME         0x00800000L

/**
 * \def WS_BORDER
 * \brief Creates a window with border.
 */
#define WS_BORDER           0x00400000L

/**
 * \def WS_THICKFRAME
 * \brief Creates a window with thick frame.
 */
#define WS_THICKFRAME       0x00200000L

/**
 * \def WS_THINFRAME
 * \brief Creates a window with thin frame.
 */
#define WS_THINFRAME        0x00100000L

/**
 * \def WS_VSCROLL
 * \brief Creates a window with vertical scroll bar.
 */
#define WS_VSCROLL          0x00080000L

/**
 * \def WS_HSCROLL
 * \brief Creates a window with horizontal scroll bar.
 */
#define WS_HSCROLL          0x00040000L

/**
 * \def WS_MINIMIZEBOX
 * \brief Creates a window with minimizing box on caption.
 */
#define WS_MINIMIZEBOX      0x00020000L

/**
 * \def WS_MAXIMIZEBOX
 * \brief Creates a window with maximizing box on caption.
 */
#define WS_MAXIMIZEBOX      0x00010000L

/**
 * \def WS_GROUP
 * \brief Indicates the control is the leader of a group.
 */
#define WS_GROUP            0x00020000L

/**
 * \def WS_TABSTOP
 * \brief Indicates the user can set the input focus to the control by 
 *        using Tab key.
 */
#define WS_TABSTOP          0x00010000L

/**
 * \def WS_EX_NONE
 * \brief None extended window style.
 */
#define WS_EX_NONE              0x00000000L     /* supported */

/**
 * \def WS_EX_USEPRIVATECDC
 * \brief The window has its own private client device context.
 *
 * \sa CreatePrivateClientDC
 */
#define WS_EX_USEPRIVATECDC     0x00000002L     /* supported */

/*Reserved*/
#define WS_EX_NOPARENTNOTIFY    0x00000004L

/**
 * \def WS_EX_TOPMOST
 * \brief The main window is a topmost (always on top) window.
 */
#define WS_EX_TOPMOST           0x00000008L     /* supported */

/**
 * \def WS_EX_TRANSPARENT
 * \brief The window is transparent.
 * \note This style is only implemented for controls.
 */
#define WS_EX_TRANSPARENT       0x00000020L

/**
 * \def WS_EX_TOOLWINDOW
 * \brief The main window is a tool window, which can not gain the input focus.
 */
#define WS_EX_TOOLWINDOW        0x00000080L     /* supported */

/**
 * \def WS_EX_USEPARENTFONT
 * \brief The window uses the default font of its parent.
 */
#define WS_EX_USEPARENTFONT     0x00000100L     /* supported */

#define WS_EX_CONTEXTHELP       0x00000400L     /*Not supported*/

/**
 * \def WS_EX_USEPARENTCURSOR
 * \brief The window uses the default cursor of its parent.
 */
#define WS_EX_USEPARENTCURSOR   0x00000800L     /* supported */

/*Reserved*/
#define WS_EX_RIGHT             0x00001000L
#define WS_EX_LEFT              0x00000000L
#define WS_EX_RTLREADING        0x00002000L
#define WS_EX_LTRREADING        0x00000000L
#define WS_EX_LEFTSCROLLBAR     0x00004000L
#define WS_EX_RIGHTSCROLLBAR    0x00000000L

#define WS_EX_CONTROLPARENT     0x00010000L
#define WS_EX_STATICEDGE        0x00020000L
#define WS_EX_APPWINDOW         0x00040000L

/* internal use */
#define WS_EX_MODALDISABLED     0x01000000L

/* Obsolete style. */
#define WS_EX_IMECOMPOSE        0x10000000L

/**
 * \def WS_EX_DLGHIDE
 * \brief The dialog won't show immediately after it is created.
 */
#define WS_EX_DLGHIDE           0x00100000L
/**
 * \def WS_EX_NOCLOSEBOX
 * \brief The main window has no closing box on its caption.
 */
#define WS_EX_NOCLOSEBOX        0x20000000L

/**
 * \def WS_EX_CTRLASMAINWIN
 * \brief The control can be displayed out of the main window which contains the control.
 */
#define WS_EX_CTRLASMAINWIN     0x40000000L

/**
 * \def WS_EX_CLIPCHILDREN
 * \brief When paint the window, the children areas will be clipped.
 */
#define WS_EX_CLIPCHILDREN      0x80000000L

    /** @} end of styles */

    /**
     * \addtogroup fns Functions
     * @{
     */

    /**
     * \defgroup window_fns Windowing functions
     * @{
     */

    /**
     * \defgroup window_create_fns Window creating/destroying
     * @{
     */

/**
 * \var typedef int (* WNDPROC)(HWND, int, WPARAM, LPARAM)
 * \brief Type of the window callback procedure.
 */
typedef int (* WNDPROC)(HWND, int, WPARAM, LPARAM);

extern MG_EXPORT HWND __mg_hwnd_desktop;

/**
 * \def HWND_DESKTOP
 * \brief Desktop window handle.
 */
#define HWND_DESKTOP        __mg_hwnd_desktop

/**
 * \def HWND_NULL
 * \brief Null window handle.
 */
#define HWND_NULL           0

/**
 * \def HWND_INVALID
 * \brief Invalid window handle.
 */
#define HWND_INVALID        0xFFFFFFFF

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    #define HWND_OTHERPROC  0xFFFFFFFE
#endif

/**
 * Structure defines a main window.
 */
typedef struct _MAINWINCREATE
{
    /** The style of the main window */
    DWORD dwStyle;

    /** The extended style of the main window */
    DWORD dwExStyle;

    /** The caption of the main window */
    const char* spCaption;

    /** The handle to the menu of the main window */
    HMENU hMenu;

    /** The handle to the cursor of the main window */
    HCURSOR hCursor;

    /** The handle to the icon of the main window */
    HICON hIcon;

    /** The hosting main window */
    HWND  hHosting;

    /** The window callback procedure */
    int (*MainWindowProc)(HWND, int, WPARAM, LPARAM);

    /** The position of the main window in the screen coordinates */
    int lx, ty, rx, by;

    /** The pixel value of background color of the main window */
    int iBkColor;

    /** The first private data associated with the main window */
    DWORD dwAddData;

    /** Reserved, do not use */
    DWORD dwReserved;
}MAINWINCREATE;
typedef MAINWINCREATE* PMAINWINCREATE;

#ifndef _LITE_VERSION

/**
 * \fn int GUIAPI CreateThreadForMainWindow(pthread_t* thread, \
 *                                    pthread_attr_t* attr, \
 *                                    void * (*start_routine)(void *), \
 *                                    void * arg);
 *
 * \brief Create a thread for main window. 
 *
 * \sa pthread_create
 */
MG_EXPORT int GUIAPI CreateThreadForMainWindow(pthread_t* thread, 
                                     pthread_attr_t* attr, 
                                     void * (*start_routine)(void *), 
                                     void * arg);

/**
 * \fn pthread_t GUIAPI GetMainWinThread(HWND hMainWnd)
 * \brief Get the thread id which main window belongs to. 
 *
 * \return Thread id.
 */
MG_EXPORT pthread_t GUIAPI GetMainWinThread(HWND hMainWnd);

/**
 * \fn int GUIAPI WaitMainWindowClose(HWND hWnd, void** returnval)
 * \brief Suspends execution of the calling thread which main window belongs
 * to until the target thread terminates, unless the target thread has already 
 * terminated. 
 *
 * \return non-NULL value on success.
 */

MG_EXPORT int GUIAPI WaitMainWindowClose(HWND hWnd, void** returnval);
#endif

/**
 * \fn void GUIAPI MainWindowThreadCleanup(HWND hMainWnd)
 * \brief Cleans up system resource associated with a main window.
 *
 * This function cleans up the system resource such as message queue associated
 * with the main window \a hMainWnd. \a DestroyMainWindow does not
 * destroy all resource used by a main window, therefore, you should call
 * this function after calling \a DestroyMainWindow and skipping out from
 * the message loop. After calling this function, the main window object
 * will destroyed actually.
 *
 * \param hMainWnd The handle to the main window.
 *
 * \sa DestroyMainWindow
 */
MG_EXPORT void GUIAPI MainWindowThreadCleanup(HWND hMainWnd);

/**
 * \def MainWindowCleanup(hwnd)
 * \brief Is an alias of \a MainWindowThreadCleanup
 *
 * \sa MainWindowThreadCleanup
 */
#define MainWindowCleanup(hwnd)      MainWindowThreadCleanup(hwnd)

/**
 * \fn HWND GUIAPI CreateMainWindow (PMAINWINCREATE pCreateStruct)
 * \brief Creates a main window.
 * 
 * This function creates a main window by using information in 
 * the structure pointed to by \a pCreateStruct, and returns 
 * the handle to the main window.
 *
 * \param pCreateStruct The pointer to a MAINWINCREATE structure.
 * \return The handle to the new main window; HWND_INVALID indicates an error.
 *
 * \sa MAINWINCREATE, styles
 *
 * Example:
 *
 * \include createmainwindow.c
 */
MG_EXPORT HWND GUIAPI CreateMainWindow (PMAINWINCREATE pCreateStruct);

/**
 * \fn BOOL GUIAPI DestroyMainWindow (HWND hWnd)
 * \brief Destroys a main window.
 *
 * This function destroys the main window specified by \a hWnd.
 * It does not release all system resource used by the main window.
 * You should call \a MainWindowThreadCleanup to destroy the main window
 * actually.
 *
 * \param hWnd The handle to the main window.
 * \return TRUE on success, FALSE on error.
 *
 * \sa MainWindowThreadCleanup
 *
 * Example:
 *
 * \include destroymainwindow.c
 */
MG_EXPORT BOOL GUIAPI DestroyMainWindow (HWND hWnd);

int GUIAPI PreDefMainWinProc (HWND hWnd, int message, 
                WPARAM wParam, LPARAM lParam);

int GUIAPI PreDefDialogProc (HWND hWnd, 
                int message, WPARAM wParam, LPARAM lParam);

int GUIAPI PreDefControlProc (HWND hWnd, int message, 
                WPARAM wParam, LPARAM lParam);

/**
 * \fn int DefaultWindowProc (HWND hWnd, int message, \
                WPARAM wParam, LPARAM lParam)
 * \brief The default window callback procedure.
 *
 * This window procedure can be used for main windows, dialog boxes,
 * and child windows.
 *
 * This function is the default window callback procedure.
 * You should call this function for all messages you do not want to handle
 * in your window procedure.
 *
 * \param hWnd The handle to the window.
 * \param message The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 */
MG_EXPORT int GUIAPI DefaultWindowProc (HWND hWnd, int message, 
                WPARAM wParam, LPARAM lParam);

/**
 * \var WNDPROC __mg_def_proc[3]
 * \brief The default window callback procedure array
 *
*/
extern MG_EXPORT WNDPROC __mg_def_proc[3];

/**
 * \def DefaultMainWinProc
 * \brief Is the default main window callback procedure.
 *
 * This function is the default main window callback procedure.
 * You should call this function for all messages, you do not want to handle
 * in your main window procedure.
 *
 * \param hWnd The handle to the window.
 * \param message The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 */
#define DefaultMainWinProc (__mg_def_proc[0])

/**
 * \def DefaultDialogProc
 * \brief The default dialog box procedure.
 *
 * This function is the default dialog box procedure. 
 * You should call this function in your dialog box procedure 
 * to process the unhandled messages.
 *
 * \param hWnd The handle to the window.
 * \param message The message identifier.
 * \param wParam The first message parameter.
 * \param lParam The second message parameter.
 *
 * \return The return value of the message handler.
 *
 */
#define DefaultDialogProc  (__mg_def_proc[1])

/**
 * \def DefaultControlProc
 * \brief The default control callback procedure.
 *
 * This function is the default control callback procedure.
 * You should call this function for all messages, you do not want to handle
 * in your own control procedure.
 *
 * \param hWnd The handle to the window.
 * \param message The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 */
#define DefaultControlProc (__mg_def_proc[2])

#ifdef _DEBUG
MG_EXPORT void GUIAPI DumpWindow (FILE* fp, HWND hWnd);
#endif /* _DEBUG */

    /**
     * \defgroup window_general_fns General window operations
     * @{
     */

#define SW_HIDE              0x0000
#define SW_SHOW              0x0010
#define SW_SHOWNORMAL        0x0100

/**
 * \fn void GUIAPI UpdateWindow (HWND hWnd, BOOL bErase)
 * \brief Updates a window.
 *
 * This function updates the window specified by \a hWnd.
 * It will redraw the caption, the frame, and the menu bar of the window.
 * It will invalidate the client area of the window as well, and 
 * if \b bErase is TRUE, the client area will be erased by using 
 * background color.
 *
 * \param hWnd The handle to the window.
 * \param bErase Indicates whether to erase the client area of the window.
 *
 * \sa InvalidateRect
 */
MG_EXPORT void GUIAPI UpdateWindow (HWND hWnd, BOOL bErase);

/**
 * \fn BOOL GUIAPI ShowWindow (HWND hWnd, int iCmdShow)
 * \brief Shows or hides a window.
 *
 * This function shows or hides the window specified by \a hWnd.
 * 
 * \param hWnd The handle to the window.
 * \param iCmdShow The command to show or hide, can be one of 
 * the following values:
 *      - SW_SHOW\n
 *        Shows the window.
 *      - SW_HIDE\n
 *        Hides the window.
 *      - SW_SHOWNORMAL\n
 *        Shows the window, and if the window is a main window 
 *        sets it to be the top most main window.
 * \return TRUE on sucess, otherwise FALSE.
 */
MG_EXPORT BOOL GUIAPI ShowWindow (HWND hWnd, int iCmdShow);

/**
 * \fn BOOL GUIAPI EnableWindow (HWND hWnd, BOOL fEnable)
 * \brief Enables of disables a window.
 *
 * This function enables or disables mouse and keyboard input 
 * to the specified window \a hWnd. When input is disabled, the window 
 * does not receive input such as mouse clicks and key presses. 
 * When input is enabled, the window receives all input.
 *
 * \param hWnd The handle to the window.
 * \param fEnable Indicates whether to enable or disable the window, 
 *        TRUE for enable.
 * \return The old enable state of the window.
 *
 * \sa IsWindowEnabled
 */
MG_EXPORT BOOL GUIAPI EnableWindow (HWND hWnd, BOOL fEnable);

/**
 * \fn BOOL GUIAPI IsWindowEnabled (HWND hWnd)
 * \brief Determines whether the specified window is enabled for mouse 
 *        and keyboard input.
 *
 * This function returns the enable/disable state of the window specified by 
 * \a hWnd.
 *
 * \param hWnd The handle to the window.
 *
 * \return The enable/disable state of the window, TRUE for enabled.
 * 
 * \sa EnableWindow
 */
MG_EXPORT BOOL GUIAPI IsWindowEnabled (HWND hWnd);

/**
 * \fn BOOL GUIAPI GetClientRect(HWND hWnd, PRECT prc)
 * \brief Retrives the client rectangle of a window.
 *
 * This function retrives the coordinates of the client area of 
 * the window specified by \a hWnd. The client coordinates specify 
 * the upper-left and lower-right corners of the client area. 
 * Because client coordinates are relative to the upper-left corner of 
 * a window's client area, the coordinates of the upper-left corner 
 * are always (0,0).
 *
 * \param hWnd The handle to the window.
 * \param prc The pointer to a RECT structure receives the client rectangle.
 *
 * \return TRUE on sucess, otherwise FALSE.
 *
 * \note Note that the coordinates of the upper-left corner are always zero.
 *
 * \sa MoveWindow
 */
MG_EXPORT BOOL GUIAPI GetClientRect(HWND hWnd, PRECT prc);

/**
 * \fn int GUIAPI GetWindowBkColor (HWND hWnd)
 * \brief Returns the current background color of a window.
 *
 * This function returns the pixel value of the current background color of 
 * the window specified by \a hWnd.
 *
 * \param hWnd The handle to the window.
 *
 * \return The pixel value of the background color.
 *
 * \sa SetWindowBkColor
 */
MG_EXPORT int GUIAPI GetWindowBkColor (HWND hWnd);

/**
 * \fn int GUIAPI SetWindowBkColor (HWND hWnd, int new_bkcolor)
 * \brief Sets the background color of a window.
 *
 * This function sets the background color of the specified window \a hWnd
 * to be new pixel value \a new_backcolor. You should call \a UpdateWindow
 * or \a InvalidateRect in order that the new background color runs into
 * affect.
 *
 * \param hWnd The handle to the window.
 * \param new_bkcolor The pixel value of the new background color.
 * \return The pixel value of the old background color.
 *
 * \sa GetWindowBkColor
 */
MG_EXPORT int GUIAPI SetWindowBkColor (HWND hWnd, int new_bkcolor);

/**
 * \fn PLOGFONT GUIAPI GetWindowFont (HWND hWnd)
 * \brief Retrives the default font of a window.
 *
 * This function retrives the default font of the specified 
 * window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The default logical font of the window.
 *
 * \sa SetWindowFont
 */
MG_EXPORT PLOGFONT GUIAPI GetWindowFont (HWND hWnd);

/**
 * \fn PLOGFONT GUIAPI SetWindowFont (HWND hWnd, PLOGFONT pLogFont)
 * \brief Sets the default font of a window.
 *
 * This function sets the default font of the specified window \a hWnd
 * to be the logical font \a pLogFont. This function will send an
 * MSG_FONTCHANGING message to the window. If the handler of the message
 * returns non-zero value, this function will return immediately with the 
 * unchanged default font. Or, after the new default font set, this function 
 * will send an MSG_FONTCHANGED message to the window as a notification.
 *
 * \param hWnd The handle to the window.
 * \param pLogFont The new default logical font. If it is NULL, this function
 * will set the default font to be the system wchar font.
 *
 * \return The old default logical font of the window, NULL on error.
 *
 * \sa GetWindowFont, GetSystemFont
 */
MG_EXPORT PLOGFONT GUIAPI SetWindowFont (HWND hWnd, PLOGFONT pLogFont);

/**
 * \fn HCURSOR GUIAPI GetWindowCursor (HWND hWnd)
 * \brief Retrives the current cursor of a window.
 *
 * This function retrives the current cursor of the specified
 * window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The handle to the cursor.
 *
 * \sa SetWindowCursor
 */
MG_EXPORT HCURSOR GUIAPI GetWindowCursor (HWND hWnd);

/**
 * \fn HCURSOR GUIAPI SetWindowCursor (HWND hWnd, HCURSOR hNewCursor)
 * \brief Sets the current cursor of a window.
 *
 * This function sets the current cursor of the specified window \a hWnd with
 * argument \a hNewCursor.
 *
 * \param hWnd The handle to the window.
 * \param hNewCursor The handle to the new cursor.
 * \return The handle to the old cursor.
 *
 * \sa GetWindowCursor
 */
MG_EXPORT HCURSOR GUIAPI SetWindowCursor (HWND hWnd, HCURSOR hNewCursor);

/**
 * \fn HICON GUIAPI GetWindowIcon (HWND hWnd)
 * \brief Retrives the current icon of a window.
 *
 * This function retrives the current icon of the specified
 * window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The handle to the icon.
 *
 * \sa SetWindowIcon
 */
MG_EXPORT HICON GUIAPI GetWindowIcon (HWND hWnd);

/**
 * \fn HICON GUIAPI SetWindowIcon (HWND hWnd, HICON hIcon, BOOL bRedraw)
 * \brief Sets the current icon of a window.
 *
 * This function sets the current icon of the specified window \a hWnd with
 * argument \a hIcon.
 *
 * \param hWnd The handle to the window.
 * \param hIcon The handle to the new icon.
 * \param bRedraw Indicates whether to update the whole window.
 *
 * \return The handle to the old icon.
 *
 * \note Note that MiniGUI only provides icon support for main window so far.
 *
 * \sa GetWindowIcon
 */
MG_EXPORT HICON GUIAPI SetWindowIcon (HWND hWnd, HICON hIcon, BOOL bRedraw);

/**
 * \fn DWORD GUIAPI GetWindowStyle (HWND hWnd)
 * \brief Retrives the style of a window.
 *
 * This function retrives the style of the window specified by \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The style of the window.
 *
 * \sa GetWindowExStyle
 */
MG_EXPORT DWORD GUIAPI GetWindowStyle (HWND hWnd);

/**
 * \fn DWORD GUIAPI GetWindowExStyle (HWND hWnd)
 * \brief Retrives the extended style of a window.
 *
 * This function retrives the extended style of the window specified by \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The extended style of the window.
 *
 * \sa GetWindowStyle
 */
MG_EXPORT DWORD GUIAPI GetWindowExStyle (HWND hWnd);

/**
 * \fn BOOL GUIAPI ExcludeWindowStyle (HWND hWnd, DWORD dwStyle)
 * \brief Removes the specific style of a window.
 *
 * This function removes the specific style of the window 
 * specified by \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param dwStyle The specific style which will be removed.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa ExcludeWindowStyle
 */
MG_EXPORT BOOL GUIAPI ExcludeWindowStyle (HWND hWnd, DWORD dwStyle);

/**
 * \fn BOOL GUIAPI IncludeWindowStyle (HWND hWnd, DWORD dwStyle)
 * \brief Includes the specific style of a window.
 *
 * This function includes the specific style of the window 
 * specified by \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param dwStyle The specific style which will be included.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa IncludeWindowStyle
 */
MG_EXPORT BOOL GUIAPI IncludeWindowStyle (HWND hWnd, DWORD dwStyle);

/**
 * \fn BOOL GUIAPI ExcludeWindowExStyle (HWND hWnd, DWORD dwStyle)
 * \brief Removes the specific extended style of a window.
 *
 * This function removes the specific extended style of the window 
 * specified by \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param dwStyle The specific extended style which will be removed.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa ExcludeWindowStyle
 */
MG_EXPORT BOOL GUIAPI ExcludeWindowExStyle (HWND hWnd, DWORD dwStyle);

/**
 * \fn BOOL GUIAPI IncludeWindowExStyle (HWND hWnd, DWORD dwStyle)
 * \brief Includes the specific extended style of a window.
 *
 * This function includes the specific extended style of the window 
 * specified by \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param dwStyle The specific extended style which will be included.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa IncludeWindowStyle
 */
MG_EXPORT BOOL GUIAPI IncludeWindowExStyle (HWND hWnd, DWORD dwStyle);

/**
 * \fn WNDPROC GUIAPI GetWindowCallbackProc (HWND hWnd)
 * \brief Retrives the callback procedure of a window.
 *
 * This function retrives the window callback procedure of the specified window
 * \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The callback procedure of the window.
 *
 * \sa SetWindowCallbackProc
 */
MG_EXPORT WNDPROC GUIAPI GetWindowCallbackProc (HWND hWnd);

/**
 * \fn WNDPROC GUIAPI SetWindowCallbackProc (HWND hWnd, WNDPROC newProc)
 * \brief Sets the callback procedure of a window.
 *
 * This function sets the window callback procedure of the specified window
 * \a hWnd to be the procedure \a newProc.
 *
 * \param hWnd The handle to the window.
 * \param newProc The new callback procedure of the window.
 * \return The old callback procedure of the window.
 *
 * \sa GetWindowCallbackProc
 *
 * Example:
 *
 * \include subclass.c
 */
MG_EXPORT WNDPROC GUIAPI SetWindowCallbackProc (HWND hWnd, WNDPROC newProc);

/**
 * \fn DWORD GUIAPI GetWindowAdditionalData (HWND hWnd)
 * \brief Retrives the first additional data of a window.
 *
 * This function retrives the first additional data of the specified window
 * \a hWnd.
 * 
 * \param hWnd The handle to the window.
 * \return The first additional data of the window.
 *
 * \sa SetWindowAdditionalData
 */
MG_EXPORT DWORD GUIAPI GetWindowAdditionalData (HWND hWnd);

/**
 * \fn DWORD GUIAPI SetWindowAdditionalData (HWND hWnd, DWORD newData)
 * \brief Sets the first additional data of a window.
 *
 * This function sets the first additional data of the specified window
 * \a hWnd.
 * 
 * \param hWnd The handle to the window.
 * \param newData The new first additional data of the window.
 * \return The old first additional data of the window.
 *
 * \note For a dialog box created by \a DialogBoxIndirectParam, 
 *       its second additional data used internally by the system to save 
 *       the return value of the box. So you should avoid setting the 
 *       second additional data of a dialog box.
 *
 * \sa GetWindowAdditionalData
 */
MG_EXPORT DWORD GUIAPI SetWindowAdditionalData (HWND hWnd, DWORD newData);

/**
 * \fn DWORD GUIAPI GetWindowAdditionalData2 (HWND hWnd)
 * \brief Retrives the second additional data of a window.
 *
 * This function retrives the second additional data of the specified window
 * \a hWnd.
 * 
 * \param hWnd The handle to the window.
 * \return The second additional data of the window.
 *
 * \sa SetWindowAdditionalData2
 */
MG_EXPORT DWORD GUIAPI GetWindowAdditionalData2 (HWND hWnd);

/**
 * \fn DWORD GUIAPI SetWindowAdditionalData2 (HWND hWnd, DWORD newData)
 * \brief Sets the second additional data of a window.
 *
 * This function sets the second additional data of the specified window
 * \a hWnd.
 * 
 * \param hWnd The handle to the window.
 * \param newData The new second additional data of the window.
 * \return The old second additional data of the window.
 *
 * \note For a control created by \a CreateWindowEx, its second additional
 *       data is reserved for internal use. So you should avoid setting 
 *       the second additional data of a control.
 *
 * \sa GetWindowAdditionalData2
 */
MG_EXPORT DWORD GUIAPI SetWindowAdditionalData2 (HWND hWnd, DWORD newData);

/**
 * \fn DWORD GUIAPI GetWindowClassAdditionalData (HWND hWnd)
 * \brief Retrives the additional data of a control class.
 *
 * This function retrives the additional data of the control class to which
 * the specified control \a hWnd belongs.
 *
 * \param hWnd The handle to the control.
 *
 * \return The additional data of the control class to which the control 
 *         belongs.
 *
 * \note Note that this function always returns zero for the main window.
 *
 * \sa SetWindowClassAdditionalData
 */
MG_EXPORT DWORD GUIAPI GetWindowClassAdditionalData (HWND hWnd);

/**
 * \fn DWORD GUIAPI SetWindowClassAdditionalData (HWND hWnd, DWORD newData)
 * \brief Sets the additional data of a control class.
 *
 * This function sets the additional data of the control class to which
 * the specified control \a hWnd belongs.
 *
 * \param hWnd The handle to the control.
 * \param newData The new additional data of the control class.
 * \return The old additional data of the control class.
 *
 * \note This function always does nothing and returns zero for the main window.
 *
 * \sa SetWindowClassAdditionalData
 */
MG_EXPORT DWORD GUIAPI SetWindowClassAdditionalData (HWND hWnd, DWORD newData);

/**
 * \fn const char* GUIAPI GetWindowCaption (HWND hWnd)
 * \brief Retrives the caption of a window.
 *
 * This function retrives the caption of the specified window \a hWnd.
 *
 * \param hWnd The handle to the window.
 *
 * \return The pointer to the caption string of the window.
 *
 * \sa SetWindowCaption
 */
MG_EXPORT const char* GUIAPI GetWindowCaption (HWND hWnd);

/**
 * \fn BOOL GUIAPI SetWindowCaption (HWND hWnd, const char* spCaption)
 * \brief Sets the caption of a window.
 *
 * This function sets the caption of the specified window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param spCaption The pointer to the new caption of the window.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa GetWindowCaption, SetWindowText
 */
MG_EXPORT BOOL GUIAPI SetWindowCaption (HWND hWnd, const char* spCaption);

/**
 * \fn BOOL GUIAPI InvalidateRect(HWND hWnd, const RECT* prc, BOOL bEraseBkgnd)
 * \brief Makes a rectangle region in the client area of a window invalid.
 *
 * This function adds a rectangle pointed to by \a prc to the specified 
 * window's update region. The update region represents the portion of 
 * the window's client area that must be redrawn, and erase background 
 * if argument \a bReaseBkgnd is set.
 *
 * \param hWnd The handle to the window.
 * \param prc The pointer to a RECT structure which defines the 
 *        invalid rectangle.
 * \param bEraseBkgnd Indicates whether the background should be erased.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa MSG_PAINT
 */
MG_EXPORT BOOL GUIAPI InvalidateRect(HWND hWnd, 
                const RECT* prc, BOOL bEraseBkgnd);

/**
 * \fn HDC GUIAPI BeginPaint(HWND hWnd)
 * \brief Prepares a window for painting.
 *
 * This function prepares the specified window \a hWnd for painting.
 * This function is called in the handler of MSG_PAINT message normally.
 * It returns a device context including the update region of the window.
 * When you are done with the device context, you should call \a EndPaint
 * to finish the painting of the window.
 *
 * \param hWnd The handle to the window.
 * \return The device context.
 *
 * \sa EndPaint
 */
MG_EXPORT HDC GUIAPI BeginPaint(HWND hWnd);

/**
 * \fn void GUIAPI EndPaint(HWND hWnd, HDC hdc)
 * \brief Marks the end of painting in a window.
 *
 * This function marks the end of painting in the specified window. 
 * This function is required for each call to the \a BeginPaint function, 
 * but only after painting is complete.
 *
 * \param hWnd The handle to the window.
 * \param hdc The device context returned by BeginPaint.
 *
 * \sa BeginPaint
 */
MG_EXPORT void GUIAPI EndPaint(HWND hWnd, HDC hdc);

/**
 * \fn BOOL GUIAPI GetUpdateRect (HWND hWnd, RECT* update_rect)
 * \brief Retrives the bounding box of the update region of a window.
 *
 * This function retrives the bounding box of the update region of
 * the specified window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param update_rect The pointer to a RECT structure which will contains 
 *        the bounding box of the update region.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa InvalidateRect
 */
MG_EXPORT BOOL GUIAPI GetUpdateRect (HWND hWnd, RECT* update_rect);

/**
 * \fn int GUIAPI ClientWidthToWindowWidth (DWORD dwStyle, int cw)
 * \brief Calculates main window width from the width of the client area.
 *
 * This function calculates the main window width from the width of the 
 * client area.
 *
 * \param dwStyle The style of the main window.
 * \param cw The width of the client area.
 *
 * \return The width of the main window.
 *
 * \sa ClientHeightToWindowHeight
 */
MG_EXPORT int GUIAPI ClientWidthToWindowWidth (DWORD dwStyle, int cw);

/**
 * \fn int GUIAPI ClientHeightToWindowHeight (DWORD dwStyle, int ch, \
                BOOL hasMenu)
 * \brief Calculates main window height from the height of the client area.
 *
 * This function calculates the main window height from the height of
 * the client area.
 *
 * \param dwStyle The style of the main window.
 * \param ch The height of the client area.
 * \param hasMenu Indicates whether the main window has menu.
 * \return The height of the main window.
 *
 * \sa ClientWidthToWindowWidth
 */
MG_EXPORT int GUIAPI ClientHeightToWindowHeight (DWORD dwStyle, 
                int ch, BOOL hasMenu);

/**
 * \fn void GUIAPI ClientToScreen (HWND hWnd, int* x, int* y)
 * \brief Converts the client coordinates of a point to screen coordinates.
 *
 * This function converts the client coordinates of the specified point 
 * \a (*x,*y) in the specified window \a hWnd to screen coordinates.
 *
 * \param hWnd The handle to the window.
 * \param x The pointer to the x coordinate.
 * \param y The pointer to the y coordinate.
 * 
 * \sa ScreenToClient
 */
MG_EXPORT void GUIAPI ClientToScreen (HWND hWnd, int* x, int* y);

/**
 * \fn void GUIAPI ScreenToClient (HWND hWnd, int* x, int* y)
 * \brief Converts the screen coordinates of a point to client coordinates.
 *
 * This function converts the screen coordinates of the specified point 
 * \a (*x,*y) to client coordinates of the specified window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param x The pointer to the x coordinate.
 * \param y The pointer to the y coordinate.
 * 
 * \sa ClientToScreen
 */
MG_EXPORT void GUIAPI ScreenToClient (HWND hWnd, int* x, int* y);

/**
 * \fn void GUIAPI WindowToScreen (HWND hWnd, int* x, int* y)
 * \brief Converts the window coordinates of a point to screen coordinates.
 *
 * This function converts the window coordinates of the specified point 
 * \a (*x,*y) in the specified window \a hWnd to the screen coordinates.
 *
 * \param hWnd The handle to the window.
 * \param x The pointer to the x coordinate.
 * \param y The pointer to the y coordinate.
 * 
 * \sa ScreenToWindow
 */
MG_EXPORT void GUIAPI WindowToScreen (HWND hWnd, int* x, int* y);

/**
 * \fn void GUIAPI ScreenToWindow (HWND hWnd, int* x, int* y)
 * \brief Converts the screen coordinates of a point to window coordinates.
 *
 * This function converts the screen coordinates of the specified point 
 * \a (*x,*y) to the window coordinates of the specfied window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param x The pointer to the x coordinate.
 * \param y The pointer to the y coordinate.
 * 
 * \sa WindowToScreen
 */
MG_EXPORT void GUIAPI ScreenToWindow (HWND hWnd, int* x, int* y);

/**
 * \fn BOOL GUIAPI IsMainWindow (HWND hWnd)
 * \brief Determines whether a window is a main window.
 *
 * This function determines whether the specified window \a hWnd is 
 * a main window or not.
 *
 * \param hWnd The handle to the window.
 *
 * \return TRUE for main window, otherwise FALSE.
 *
 * \sa IsControl, IsWindow
 */
MG_EXPORT BOOL GUIAPI IsMainWindow (HWND hWnd);

/**
 * \fn BOOL GUIAPI IsControl (HWND hWnd)
 * \brief Determines whether a window is a control.
 *
 * This function determines whether the specified window \a hWnd is a control.
 *
 * \param hWnd The handle to the window.
 *
 * \return TRUE for control, otherwise FALSE.
 *
 * \sa IsMainWindow, IsWindow
 */
MG_EXPORT BOOL GUIAPI IsControl (HWND hWnd);

/**
 * \fn BOOL GUIAPI IsWindow (HWND hWnd)
 * \brief Determines whether a window handle identifies an existing window.
 *
 * This function determines whether the specified window handle \a hWnd 
 * identifies an existing window.
 *
 * \param hWnd The window handle.
 *
 * \return TRUE for window, otherwise FALSE.
 *
 * \sa IsMainWindow, IsControl
 */
MG_EXPORT BOOL GUIAPI IsWindow (HWND hWnd);

/**
 * \fn BOOL GUIAPI IsDialog (HWND hWnd)
 * \brief Determines whether a window handle identifies a dialog window.
 *
 * This function determines whether the specified window handle \a hWnd 
 * identifies a dialog window.
 *
 * \param hWnd The window handle.
 *
 * \return TRUE for dialog window, otherwise FALSE.
 *
 * \sa IsMainWindow, IsControl
 */
MG_EXPORT BOOL GUIAPI IsDialog (HWND hWnd);

/**
 * \fn HWND GUIAPI GetParent (HWND hWnd)
 * \brief Retrieves the handle to a child window's parent window.
 *
 * This function retrieves the handle to the specified child window's 
 * parent window.
 *
 * \param hWnd The handle to the child window.
 *
 * \return The handle to the parent, HWND_INVALID indicates an error.
 *
 * \note For a main window, this function always returns 0. 
 *       For HWND_DESKTOP or an invalid window handle, 
 *       HWND_INVALID will be returned.
 *
 * \sa GetMainWindowHandle
 */
MG_EXPORT HWND GUIAPI GetParent (HWND hWnd);

/**
 * \fn HWND GUIAPI GetMainWindowHandle (HWND hWnd)
 * \brief Retrives the handle to the main window contains a window.
 *
 * This function retrives the handle to the main window which contains the
 * specified window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The handle to the main window, HWND_INVALID indicates an error.
 *
 * \note For a main window, this function always returns the handle to itself.
 *       For HWND_DESKTOP, HWND_DESKTOP is returned.
 *
 * \sa GetParent
 */
MG_EXPORT HWND GUIAPI GetMainWindowHandle (HWND hWnd);

/**
 * \fn BOOL GUIAPI IsWindowVisible (HWND hWnd)
 * \brief Retrieves the visibility state of the specified window.
 *
 * This function retrives the visibility state of the specified window \a hWnd.
 *
 * \param hWnd [in] Handle to the window to test.
 * \return If the specified window, its parent window, its parent's parent 
 * window, and so forth, all have the WS_VISIBLE style, the return value is 
 * nonzero. Otherwise the return value is zero.
 *
 * \sa ShowWindow
 */
MG_EXPORT BOOL GUIAPI IsWindowVisible (HWND hWnd);

/**
 * \fn BOOL GUIAPI GetWindowRect (HWND hWnd, PRECT prc)
 * \brief Retrives the dimensions of the bounding rectangle of a window. 
 *
 * This function retrives the dimension of the bounding rectangle of
 * the specified window \a hWnd. The dimensions are given in parent's
 * client coordinates (screen coordinates for main window) that are 
 * relative to the upper-left corner of the parent's client area (screen).
 *
 * \param hWnd The handle to the window.
 * \param prc The pointer to a RECT structure which will contains the 
 *        window rectangle.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa GetClientRect
 */
MG_EXPORT BOOL GUIAPI GetWindowRect (HWND hWnd, PRECT prc);

/**
 * \fn HWND GUIAPI GetNextChild (HWND hWnd, HWND hChild)
 * \brief Retrives the next control in a window.
 *
 * This function retrives the next child of the specified 
 * window \a hWnd. If you pass 0 for the argument
 * of \a hChild, the function will return the first child of the window.
 *
 * \param hWnd The handle to the window.
 * \param hChild The handle to a child of the window.
 * \return The handle of the next child. If you pass HWND_NULL for
 *         the argument of \a hChild, it will return the first child of the 
 *         window. When the child you passed is the last one, this function 
 *         will return 0. If hWnd is not the parent of hChild, this function 
 *         will return HWND_INVALID.
 *
 * \sa GetNextMainWindow
 *
 * \note Do not use HWND_DESKTOP for \a hWnd or \a hChild.
 *
 * Example:
 *
 * \include nextcontrol.c
 */
MG_EXPORT HWND GUIAPI GetNextChild (HWND hWnd, HWND hChild);

/**
 * \fn HWND GUIAPI GetNextMainWindow (HWND hMainWnd)
 * \brief Retrives the next main window in the system according to the zorder.
 *
 * This function retrives the next main window of the specified 
 * main window \a hMainWnd.
 *
 * \param hMainWnd The handle to the main window.
 *
 * \return The handle of the next main window. If you pass HWND_NULL 
 *         for the argument of \a hMainWnd, it will return the topmost main 
 *         window. When the main window you passed is the bottommost one, 
 *         this function will return HWND_NULL. If hMainWnd is not a main 
 *         window, the function will return HWND_INVALID.
 *
 * \sa GetNextChild
 */
MG_EXPORT HWND GUIAPI GetNextMainWindow (HWND hMainWnd);

/**
 * \fn HWND GUIAPI GetHosting (HWND hMainWnd)
 * \brief Retrives the hosting main window of a main window.
 *
 * This function retrives the hosting main window of the specified 
 * main window \a hWnd. The hosting main window creates the message queue,
 * which shared with all hosted main window of it.
 * The hosting window of a top-level main window is HWND_DESKTOP.
 *
 * HWND_DESKTOP has no hosting window, do not use this function for it.
 *
 * \param hMainWnd The handle to the main window.
 *
 * \return The handle to the hosting main window. A valid window must have a 
 *         hosting window.  If error occurs, such as a invalid handle or 
 *         HWND_DESKTOP is passed as \a hMainWnd, HWND_INVALID is returned.
 *
 * \sa GetFirstHosted, GetNextHosted
 */
MG_EXPORT HWND GUIAPI GetHosting (HWND hMainWnd);

/**
 * \fn HWND GUIAPI GetFirstHosted (HWND hHosting)
 * \brief Retrives the first hosted main window of a main window.
 *
 * This function retrives the first hosted main window of 
 * the specified main window \a hMainWnd.
 *
 * For MiniGUI-Threads, HWND_DESKTOP has no any "hosted" window, 
 * so do not use this function for it.
 *
 * \param hHosting The handle to the hosting main window.
 *
 * \return The handle to the first hosted main window. If an invalid window
 *         handle is passed for \a hHosting, HWND_INVALID will be returned. 
 *         If tihs main window do not have a hosted window, HWND_NULL 
 *         will be returned.
 *
 * \sa GetHosting, GetNextHosted
 */
MG_EXPORT HWND GUIAPI GetFirstHosted (HWND hHosting);

/**
 * \fn HWND GUIAPI GetNextHosted (HWND hHosting, HWND hHosted)
 * \brief Retrives the next hosted main window of a main window.
 *
 * This function retrives the next hosted main window of the specified
 * main window \a hHosting.
 *
 * For MiniGUI-Threads, HWND_DESKTOP has no any "hosted" window, 
 * so do not use this function for it.
 *
 * \param hHosting The handle to the hosting main window.
 * \param hHosted The handle to a known hosted main window. This function
 *        will return the next hosted main window.
 *
 * \return Handle to the next hosted main window. 
 *         Returns 0 when \a hHosted is the last hosted main window; 
 *         HWND_INVALID when error occurs. If \a hHosted is 0, this call
 *         equals GetFirstHosted(hHosting).
 *         If an invalid window handle is passed as \a hHosting
 *         or \a hHosted, HWND_INVALID will be returned; If \a hHosted is 
 *         not the hosted window of \a hHosting, HWND_INVALID will be returned.
 *
 * \sa GetFirstHosted
 */
MG_EXPORT HWND GUIAPI GetNextHosted (HWND hHosting, HWND hHosted);

/**
 * \fn int GUIAPI GetWindowTextLength (HWND hWnd)
 * \brief Retrieves the length of a window's text.
 *
 * This function retrieves the length, in characters, of the specified 
 * window's text. The function retrieves the length of the text by sending 
 * an MSG_GETTEXTLENGTH message to the window.
 *
 * \param hWnd The handle to the window.
 *
 * \return The length of the window's text.
 *
 * \sa GetWindowText
 */
MG_EXPORT int GUIAPI GetWindowTextLength (HWND hWnd);

/**
 * \fn int GUIAPI GetWindowText (HWND hWnd, char* spString, int nMaxLen)
 * \brief Copies the text of a window's into a buffer.
 *
 * This function copies the text of the specified window \a hWnd into the buffer
 * pointed to by \a spString. The function gets the window text by sending
 * an MSG_GETTEXT message to the window.
 *
 * \param hWnd The handle to the window.
 * \param spString The pointer to a buffer receives the text.
 * \param nMaxLen The maximal number of characters can be copied to the buffer.
 *
 * \return The length of the window text string.
 *
 * \sa GetWindowCaption, SetWindowText
 */
MG_EXPORT int GUIAPI GetWindowText (HWND hWnd, char* spString, int nMaxLen);

/**
 * \fn BOOL GUIAPI SetWindowText (HWND hWnd, const char* spString)
 * \brief Sets the text of a window.
 *
 * This function copies the string in the buffer pointed to by \a spString
 * to be the text of the specified window \a hWnd. The function sets
 * the window text by sending an MSG_SETTEXT message to the window.
 *
 * \param hWnd The handle to the window.
 * \param spString The pointer to the buffer.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa SetWindowCaption, GetWindowText
 */
MG_EXPORT BOOL GUIAPI SetWindowText (HWND hWnd, const char* spString);

/**
 * \fn HWND GUIAPI GetFocusChild (HWND hParent)
 * \brief Retrieves the handle to the window's active child that has 
 *        the keyboard focus.
 *
 * This function retrives the handle to the window's active child that has
 * the keyboard focus.
 *
 * \param hParent The handle to the parent window.
 *
 * \return The handle to the active child.
 *
 * \sa SetFocusChild, SetNullFoucs
 */
MG_EXPORT HWND GUIAPI GetFocusChild (HWND hParent);

/**
 * \fn HWND GUIAPI SetNullFocus (HWND hParent)
 * \brief Cancels the current active child and set the focus child to be null.
 *
 * This function cancels the current active child and set the focus child
 * of the window \a hParent to be null.
 *
 * \param hParent The handle to the parent window.
 *
 * \return The handle to the old active child.
 *
 * \sa GetFocusChild, SetFocusChild
 */
MG_EXPORT HWND GUIAPI SetNullFocus (HWND hParent);

/**
 * \fn HWND GUIAPI SetFocusChild (HWND hWnd)
 * \brief Sets the active child of a window.
 *
 * This function sets the specified window \a hWnd as the active child of 
 * its parent.
 *
 * \param hWnd The handle to the window.
 *
 * \return The handle to the old active child of its parent.
 *
 * \sa GetFocusChild, SetNullFocus
 */
MG_EXPORT HWND GUIAPI SetFocusChild (HWND hWnd);

/**
 * \def SetFocus
 * \sa SetFocusChild
 */
#define SetFocus SetFocusChild

/**
 * \def GetFocus
 * \sa GetFocusChild
 */
#define GetFocus GetFocusChild

/**
 * \fn HWND GUIAPI GetActiveWindow (void)
 * \brief Retrieves the main window handle to the active main window.
 *
 * This function retrives the main window handle to the active main window 
 * which receives the input.
 *
 * \return The handle to the active main window.
 *
 * \sa SetActiveWindow, GetFocusChild
 */
MG_EXPORT HWND GUIAPI GetActiveWindow (void);

/**
 * \fn HWND GUIAPI SetActiveWindow (HWND hMainWnd)
 * \brief Sets a main window to be the active main window.
 *
 * This function sets the specified main window \a hMainWnd to be the 
 * active main window which receives the input.
 *
 * \param hMainWnd The handle to the new active main window.
 *
 * \return The handle to the old active main window.
 *
 * \sa GetActiveWindow, SetFocusChild
 */
MG_EXPORT HWND GUIAPI SetActiveWindow (HWND hMainWnd);

/**
 * \def GetForegroundWindow
 * \sa GetActiveWindow
 */
#define GetForegroundWindow GetActiveWindow

/**
 * \def SetForegroundWindow
 * \sa SetActiveWindow
 */
#define SetForegroundWindow SetActiveWindow

/**
 * \fn HWND GUIAPI GetCapture(void)
 * \brief Retrives the handle to the window (if any) that has captured 
 * the mouse.
 *
 * This function retrieves the handle to the window (if any) that has captured 
 * the mouse.  Only one window at a time can capture the mouse; this window 
 * receives mouse input whether or not the cursor is within its borders.
 *
 * \return The handle to the window that has captured the mouse, 0 for
 *         no window captures the mouse.
 *
 * \sa SetCapture
 */
MG_EXPORT HWND GUIAPI GetCapture(void);

/**
 * \fn HWND GUIAPI SetCapture(HWND hWnd)
 * \brief Sets the mouse capture to the specified window.
 *
 * This function sets the mouse capture to the specified window \a hWnd. 
 * Once a window has captured the mouse, all mouse input is directed to 
 * that window, regardless of whether the cursor is within the borders 
 * of that window. Only one window at a time can capture the mouse. 
 *
 * \param hWnd The handle to the window.
 *
 * \return The old capture window.
 *
 * \sa GetCapture
 */
MG_EXPORT HWND GUIAPI SetCapture(HWND hWnd);

/**
 * \fn void GUIAPI ReleaseCapture(void)
 * \brief Releases the mouse capture from a window and restores normal mouse 
 *        input processing.
 *
 * This function releases the mouse capture from a window and restores normal 
 * mouse input processing. A window that has captured the mouse receives 
 * all mouse input, regardless of the position of the cursor.
 *
 * \sa GetCapture, SetCapture
 */
MG_EXPORT void GUIAPI ReleaseCapture(void);

/**
 * \fn HWND GUIAPI GetWindowUnderCursor(void)
 * \brief Retrives the handle to the window (if any) which is just 
 *        beneath the mouse cursor.
 *
 * This function retrieves the handle to the window (if any) that is under 
 * the mouse cursor. If a parent window and a child window are all under 
 * the mouse cursor, the handle to the child window will be returned.
 *
 * \return The handle to the window under the mouse cursor, 
 *         0 for no window is under the mouse.
 */
MG_EXPORT HWND GUIAPI GetWindowUnderCursor(void);

/**
 * \fn BOOL GUIAPI MoveWindow (HWND hWnd, int x, int y, int w, int h, \
 *               BOOL fPaint)
 * \brief Changes the position and dimensions of a window.
 *
 * This function changes the position and dimensions of the specified window 
 * \a hWnd.  For a main window, the position and dimensions are relative to 
 * the upper-left corner of the screen. For a control, they are relative to 
 * the upper-left corner of the parent window's client area.
 *
 * \param hWnd The handle to the window.
 * \param x The new x coordinate of the upper-left corner of the window.
 * \param y The new y coordinate of the upper-left corner of the window.
 * \param w The new width of the window.
 * \param h The new height of the window.
 * \param fPaint Indicates whether the window should be repainted.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa ScrollWindow
 */
MG_EXPORT BOOL GUIAPI MoveWindow (HWND hWnd, int x, int y, int w, int h, 
                BOOL fPaint);

/**
 * \fn void GUIAPI ScrollWindow (HWND hWnd, int iOffx, int iOffy, \
                const RECT* rc1, const RECT* rc2)
 * \brief Scrolls the content of a window's client area.
 *
 * This function scrolls the content of the specified window's client area.
 *
 * \param hWnd The handle to the window.
 * \param iOffx The new x coordinate of the origin in the client coordinates 
 *        system after scrolling.
 * \param iOffy The new y coordinate of the origin in the client coordinates 
 *        system after scrolling.
 * \param rc1 The rectangle of the area which will be scrolled actually. 
 *        NULL for whole client area.
 * \param rc2 A rectangle, all children covered totally by this rectangle 
 *        will be moved after scrolling. All of the children will be 
 *        moved if \a rc2 is NULL.
 *
 * \sa MoveWindow
 */
MG_EXPORT void GUIAPI ScrollWindow (HWND hWnd, int iOffx, int iOffy, 
                const RECT* rc1, const RECT* rc2);

#define MWM_MINWIDTH            0
#define MWM_MINHEIGHT           1
#define MWM_BORDER              2
#define MWM_THICKFRAME          3
#define MWM_THINFRAME           4
#define MWM_CAPTIONY            5
#define MWM_ICONX               6
#define MWM_ICONY               7
#define MWM_MENUBARY            8
#define MWM_MENUBAROFFX         9
#define MWM_MENUBAROFFY         10
#define MWM_MENUITEMY           11 
#define MWM_INTERMENUITEMX      12
#define MWM_INTERMENUITEMY      13
#define MWM_MENUITEMOFFX        14
#define MWM_MENUTOPMARGIN       15
#define MWM_MENUBOTTOMMARGIN    16
#define MWM_MENULEFTMARGIN      17
#define MWM_MENURIGHTMARGIN     18
#define MWM_MENUITEMMINX        19
#define MWM_MENUSEPARATORY      20
#define MWM_MENUSEPARATORX      21
#define MWM_SB_HEIGHT           22
#define MWM_SB_WIDTH            23
#define MWM_SB_INTERX           24
#define MWM_CXVSCROLL           25
#define MWM_CYVSCROLL           26
#define MWM_CXHSCROLL           27
#define MWM_CYHSCROLL           28
#define MWM_MINBARLEN           29
#define MWM_DEFBARLEN           30

#define MWM_ITEM_NUMBER         31

extern MG_EXPORT int WinMainMetrics [];

/**
 * \def GetMainWinMetrics
 * \brief Get main window metrics.
 */
#define GetMainWinMetrics(iItem)    (WinMainMetrics[iItem])

/**
 * \fn int GUIAPI GetWindowElementMetricsEx (HWND hwnd, Uint16 item)
 * \brief Returns the metrics value of an element of a window.
 *
 * This function returns the metrics value of the specific window element
 * of the window specified by \a hwnd.
 *
 * \param hwnd The handle to the window. If you pass 0 for
 *        this argument, this function will return the default element metrics.
 * \param item The number of the window element, can be one of the following 
 *        value: 
 *
 *          - MWM_MINWIDTH\n
 *            The minimal width of a main window.
 *          - MWM_MINHEIGHT\n
 *            The minimal height of a main window.
 *          - MWM_BORDER\n
 *            The width of the border of a main window.
 *          - MWM_THICKFRAME\n
 *            The width of the thick frame of a main window.
 *          - MWM_THINFRAME\n
 *            The width of the thin frame of a main window.
 *          - MWM_CAPTIONY\n
 *            The height of the caption of a main window.
 *          - MWM_ICONX\n
 *            The width of an icon.
 *          - MWM_ICONY\n
 *            The height of an icon.
 *          - MWM_MENUBARY\n
 *            The height of the menu bar of a main window.
 *          - MWM_MENUBAROFFX\n
 *            The x-offset of the menu bar of a main window.
 *          - MWM_MENUBAROFFY\n
 *            The y-offset of the menu bar of a main window.
 *          - MWM_MENUITEMY\n
 *            The height of a menu item.
 *          - MWM_INTERMENUITEMX\n
 *            The horizontal interval space between two menu items.
 *          - MWM_INTERMENUITEMY\n
 *            The vertical interval space between two menu items.
 *          - MWM_MENUITEMOFFX\n
 *            The horizontal offset of a menu item.
 *          - MWM_MENUTOPMARGIN\n
 *            The top margin of a menu item.
 *          - MWM_MENUBOTTOMMARGIN\n
 *            The bottom margin of a menu item.
 *          - MWM_MENULEFTMARGIN\n
 *            The bottom margin of a menu item.
 *          - MWM_MENURIGHTMARGIN\n
 *            The right margin of a menu item.
 *          - MWM_MENUITEMMINX\n
 *            The minimal width of a menu item.
 *          - MWM_MENUSEPARATORY\n
 *            The height of a separator menu item.
 *          - MWM_MENUSEPARATORX\n
 *            The width of a separator menu item.
 *          - MWM_SB_HEIGHT\n
 *            The height of the button on the scroll bar.
 *          - MWM_SB_WIDTH\n
 *            The width of the button on the scroll bar.
 *          - MWM_SB_INTERX\n
 *          - MWM_CXVSCROLL\n
 *            The width of a vertical scroll bar.
 *          - MWM_CYVSCROLL\n
 *            The height of a vertical scroll bar.
 *          - MWM_CXHSCROLL\n
 *            The width of a horizontal scroll bar.
 *          - MWM_CYHSCROLL\n
 *            The height of a horizontal scroll bar.
 *          - MWM_MINBARLEN\n
 *            The minimal track-bar length of a scroll bar.
 *          - MWM_DEFBARLEN\n
 *            The default track-bar length of a scroll bar.
 *
 * \return The metrics value of the window element, -1 for error.
 *
 * \sa SetWindowElementMetricsEx
 */
MG_EXPORT int GUIAPI GetWindowElementMetricsEx (HWND hwnd, Uint16 item);

/**
 * \fn int GUIAPI SetWindowElementMetricsEx (HWND hwnd, Uint16 item, \
                int new_value)
 * \brief Sets the metrics value of an element of a window.
 *
 * This function sets the metrics value of the specific window element of 
 * the window specified by \a hwnd and returns the old value.
 *
 * \param hwnd The handle to the window. If you pass 0 for this argument, 
 *        this function will set and return the default element metrics.
 * \param item The number of the window element.
 * \param new_value The new metrics value of the window element.
 *
 * \return The old metrics value of the window element, -1 for error.
 *
 * \sa GetWindowElementMetricsEx
 */
MG_EXPORT int GUIAPI SetWindowElementMetricsEx (HWND hwnd, Uint16 item, 
                int new_value);

#define BKC_CAPTION_NORMAL          0
#define FGC_CAPTION_NORMAL          1
#define BKC_CAPTION_ACTIVED         2
#define FGC_CAPTION_ACTIVED         3
#define BKC_CAPTION_DISABLED        4
#define FGC_CAPTION_DISABLED        5
#define WEC_FRAME_NORMAL            6
#define WEC_FRAME_ACTIVED           7
#define WEC_FRAME_DISABLED          8

#define BKC_MENUBAR_NORMAL          9
#define FGC_MENUBAR_NORMAL          10
#define BKC_MENUBAR_HILITE          11
#define FGC_MENUBAR_HILITE          12
#define FGC_MENUBAR_DISABLED        13

#define BKC_MENUITEM_NORMAL         14
#define FGC_MENUITEM_NORMAL         15
#define BKC_MENUITEM_HILITE         16
#define FGC_MENUITEM_HILITE         17
#define FGC_MENUITEM_DISABLED       18
#define BKC_PPPMENUTITLE            19
#define FGC_PPPMENUTITLE            20

#define FGC_MENUITEM_FRAME	        21

#define WEC_3DBOX_NORMAL            22
#define WED_3DBOX_REVERSE           23
#define WEC_3DBOX_LIGHT             24
#define WEC_3DBOX_DARK              25

#define WEC_FLAT_BORDER             26

#define BKC_CONTROL_DEF             27

#define FGC_CONTROL_NORMAL          28
#define FGC_CONTROL_DISABLED        29

#define BKC_HILIGHT_NORMAL          30
#define BKC_HILIGHT_LOSTFOCUS       31
#define FGC_HILIGHT_NORMAL          32
#define FGC_HILIGHT_DISABLED        33

#define BKC_DESKTOP                 34
#define BKC_DIALOG		            35
#define BKC_TIP			            36

#define WEC_ITEM_NUMBER             37

/* back compitabilty defines */
#define BKC_BUTTON_DEF              WEC_3DBOX_NORMAL
#define BKC_BUTTON_PUSHED           WEC_3DBOX_DARK
#define FGC_BUTTON_NORMAL           WED_3DBOX_REVERSE
#define FGC_BUTTON_PUSHED           WED_3DBOX_REVERSE

#define BKC_EDIT_DEF                WEC_3DBOX_LIGHT
#define BKC_EDIT_DISABLED           WEC_3DBOX_NORMAL

#define WEC_3DFRAME_LEFT_OUTER      WED_3DBOX_REVERSE
#define WEC_3DFRAME_LEFT_INNER      WEC_3DBOX_DARK
#define WEC_3DFRAME_TOP_OUTER       WED_3DBOX_REVERSE
#define WEC_3DFRAME_TOP_INNER       WEC_3DBOX_DARK
#define WEC_3DFRAME_RIGHT_OUTER     WEC_3DBOX_LIGHT
#define WEC_3DFRAME_RIGHT_INNER     WEC_3DBOX_NORMAL
#define WEC_3DFRAME_BOTTOM_OUTER    WEC_3DBOX_LIGHT
#define WEC_3DFRAME_BOTTOM_INNER    WEC_3DBOX_NORMAL

#define WEC_3DFRAME_LEFT            WEC_3DBOX_LIGHT
#define WEC_3DFRAME_TOP             WEC_3DBOX_LIGHT
#define WEC_3DFRAME_RIGHT           WEC_3DBOX_DARK
#define WEC_3DFRAME_BOTTOM          WEC_3DBOX_DARK


extern MG_EXPORT gal_pixel WinElementColors[];

/**
 * \def GetWindowElementColor
 * \brief Get window element color.
 */
#define GetWindowElementColor(iItem)    (WinElementColors[iItem])

/**
 * \fn gal_pixel GUIAPI GetWindowElementColorEx (HWND hwnd, Uint16 item)
 * \brief Returns the pixel value used to draw the specific window 
 *        element of a window.
 *
 * This function returns the pixel value used to draw the specific window 
 * element of the window specified by \a hwnd.
 *
 * \param hwnd The handle to the window. If you pass 0 for this argument, 
 *        this function will return the default element pixel value.
 * \param item The number of window element, can be one of the following 
 *        values:
 *
 *          - BKC_CAPTION_NORMAL\n
 *            The background color of caption in normal status.
 *          - FGC_CAPTION_NORMAL\n
 *            The foreground color of caption in normal status.
 *          - BKC_CAPTION_ACTIVED\n
 *            The background color of caption in active status.
 *          - FGC_CAPTION_ACTIVED\n
 *            The foreground color of caption in active status.
 *          - BKC_CAPTION_DISABLED\n
 *            The background color of caption in disabled status.
 *          - FGC_CAPTION_DISABLED\n
 *            The foreground color of caption in disabled status.
 *          - WEC_FRAME_NORMAL\n
 *            The color of window frame in normal status.
 *          - WEC_FRAME_ACTIVED\n
 *            The color of window frame in active status.
 *          - WEC_FRAME_DISABLED\n
 *            The color of window frame in disabled status.
 *          - FGC_MENUBAR_NORMAL\n
 *            The foreground color of menubar in normal status.
 *          - BKC_MENUBAR_HILITE\n
 *            The background color of menubar in high-lighted status.
 *          - FGC_MENUBAR_HILITE\n
 *            The foreground color of menubar in high-lighted status.
 *          - FGC_MENUBAR_DISABLED\n
 *            The foreground color of menubar in disalbed status.
 *          - FGC_MENUITEM_NORMAL\n
 *            The foreground color of menu item in normal status.
 *          - BKC_MENUITEM_HILITE\n
 *            The background color of menu item in high-lighted status.
 *          - FGC_MENUITEM_HILITE\n
 *            The foreground color of menu item in high-lighted status.
 *          - FGC_MENUITEM_DISABLED\n
 *            The foreground color of menu item in disabled status.
 *          - BKC_PPPMENUTITLE\n
 *            The background color of title of popup menu.
 *          - FGC_PPPMENUTITLE\n
 *            The foreground color of title of popup menu.
 *          - WED_3DBOX_REVERSE\n
 *            The reverse color of a 3D box.
 *          - WEC_3DBOX_LIGHT\n
 *            The light color of a 3D box.
 *          - WEC_3DBOX_DARK\n
 *            The dark color of a 3D box.
 *          - FGC_CONTROL_DISABLED\n
 *            The default foreground color of control in disabled status.
 *          - BKC_HILIGHT_LOSTFOCUS\n
 *            The background color of high-lighted but losted focus control.
 *          - FGC_HILIGHT_NORMAL\n
 *            The foreground color of high-lighted control in normal status.
 *          - FGC_HILIGHT_DISABLED\n
 *            The foreground color of high-lighted control in disabled status.
 *          - BKC_DIALOG\n
 *            The default background color of dialog box.
 *          - BKC_TIP\n
 *            The default background color of a tool-tip window.
 *
 * \return The pixel value of the window element.
 *
 * \sa SetWindowElementColorEx
 */
MG_EXPORT gal_pixel GUIAPI GetWindowElementColorEx (HWND hwnd, Uint16 item);

/**
 * \fn gal_pixel GUIAPI SetWindowElementColorEx (HWND hwnd, Uint16 item, \
 *               gal_pixel new_value);
 * \brief Sets the pixel value used to draw the specific window element of 
 *        a window.
 *
 * This function sets the pixel value used to draw the specific window element 
 * of the window specified by \a hwnd and returns the old pixel value.
 *
 * \param hwnd The handle to the window. If you pass 0 for this argument, 
 *        this function will set and return the default element pixel value.
 * \param item The number of window elment item.
 * \param new_value The new pixel value of the window element.
 *
 * \return The old pixel value of the window element.
 *
 * \sa GetWindowElementColorEx
 */
MG_EXPORT gal_pixel GUIAPI SetWindowElementColorEx (HWND hwnd, Uint16 item, 
                gal_pixel new_value);

    /** @} end of window_general_fns */

    /**
     * \defgroup sys_bmps System bitmap/icon operations
     * @{
     */

#define SYSBMP_CAPBTNS              0
#define SYSBMP_ARROWS               1
#define SYSBMP_SCROLLBAR_HBG        2
#define SYSBMP_SCROLLBAR_HSLIDER    3
#define SYSBMP_SCROLLBAR_VBG        4
#define SYSBMP_SCROLLBAR_VSLIDER    5

#define SYSBMP_ITEM_NUMBER          6

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)

/**
 * \fn PBITMAP GUIAPI GetSystemBitmap (int id)
 * \brief Retrives the system bitmap object by identifier.
 *
 * This function returns the system bitmap object by its identifier.
 *
 * \param id The identifier of the system bitmap object, can be
 * one of the following values:
 *
 *      - SYSBMP_CAPBTNS\n      Bitmap of the caption buttons.
 *      - SYSBMP_ARROWS\n       Bitmap of the scroll bar arrows.
 *      - SYSBMP_SCROLLBAR_HBG\n
 *        Bitmap of the background of the horizontal scrollbar.
 *      - SYSBMP_SCROLLBAR_HSLIDER\n
 *        Bitmap of the slider of the horizontal scrollbar.
 *      - SYSBMP_SCROLLBAR_VBG\n
 *        Bitmap of the background of the vertical scrollbar.
 *      - SYSBMP_SCROLLBAR_VSLIDER\n
 *        Bitmap of the slider of the vertical scrollbar.
 *
 * \return The pointer to the system bitmap object.
 *
 * \note For MiniGUI-Threads, this function is defined
 * as a macro, and have the same semantics as this function.
 *
 * \sa GetLargeSystemIcon, GetSmallSystemIcon
 */
MG_EXPORT PBITMAP GUIAPI GetSystemBitmap (int id);
#else
extern MG_EXPORT BITMAP SystemBitmap[];
#define GetSystemBitmap(iItem)      (SystemBitmap + iItem)

#endif

#define IDI_APPLICATION         0
#define IDI_HAND                1
#define IDI_STOP                IDI_HAND
#define IDI_QUESTION            2
#define IDI_EXCLAMATION         3
#define IDI_ASTERISK            4
#define IDI_INFORMATION         IDI_ASTERISK

#define SYSICO_ITEM_NUMBER      5

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)

/**
 * \fn HICON GUIAPI GetLargeSystemIcon (int id)
 * \brief Retrives a large system icon by its identifier.
 *
 * This function retrives the handle to a large (32x32) system icon 
 * by its identifier \a id.
 *
 * \param id The identifier of the icon. It can be one of the following values:
 *      - IDI_APPLICATION\n The application icon.
 *      - IDI_STOP\n        The stop icon.
 *      - IDI_QUESTION\n    The question mark icon.
 *      - IDI_EXCLAMATION\n The exclamation mark icon.
 *      - IDI_INFORMATION\n The information mark icon.
 *
 * \return The handle to the icon.
 *
 * \note For MiniGUI-Threads, this function is defined as a macro, 
 *       and have the same semantics as this function.
 *
 * \sa GetSystemBitmap
 */
MG_EXPORT HICON GUIAPI GetLargeSystemIcon (int id);

/**
 * \fn HICON GUIAPI GetSmallSystemIcon (int id)
 * \brief Retrives a small system icon by its identifier.
 *
 * This function retrives the handle to a small (16x16) system icon by 
 * its identifier \a id.
 *
 * \param id The identifier of the icon. It can be one of the following values:
 *      - IDI_APPLICATION\n The application icon.
 *      - IDI_STOP\n        The stop icon.
 *      - IDI_QUESTION\n    The question mark icon.
 *      - IDI_EXCLAMATION\n The exclamation mark icon.
 *      - IDI_INFORMATION\n The information mark icon.
 *
 * \return The handle to the icon.
 *
 * \note For MiniGUI-Threads, this function is defined as a macro, 
 *       and have the same semantics as this function.
 *
 * \sa GetSystemBitmap
 */
MG_EXPORT HICON GUIAPI GetSmallSystemIcon (int id);
#else

extern MG_EXPORT HICON LargeSystemIcon[];
#define GetLargeSystemIcon(iItem)   (LargeSystemIcon[iItem])

extern MG_EXPORT HICON SmallSystemIcon[];
#define GetSmallSystemIcon(iItem)   (SmallSystemIcon[iItem])

#endif

#define STOCKBMP_CAPTION              "caption"
#define STOCKBMP_BUTTON               "button"
#define STOCKBMP_DOWNARROW            "downarrow"
#define STOCKBMP_UPDOWNARROW          "updownarrow"
#define STOCKBMP_LEFTRIGHTARROW       "leftrightarrow"
#define STOCKBMP_CHECKMARK            "checkmark"
#define STOCKBMP_TRACKBAR_HBG         "trackbar_hbg"
#define STOCKBMP_TRACKBAR_HSLIDER     "trackbar_hslider"
#define STOCKBMP_TRACKBAR_VBG         "trackbar_vbg"
#define STOCKBMP_TRACKBAR_VSLIDER     "trackbar_vslider"
#define STOCKBMP_SPINBOX_HORZ         "spinbox_horz"
#define STOCKBMP_SPINBOX_VERT         "spinbox_vert"
#define STOCKBMP_LVFOLD               "lvfold"
#define STOCKBMP_LVUNFOLD             "lvunfold"

#define STOCKBMP_IMECTRLBTN           "IMEctrlbtn"
#define STOCKBMP_LOGO                 "logo"

/**
 * \fn const BITMAP* GetStockBitmap (const char* name, int ckey_x, int ckey_y)
 * \brief Gets a stock bitmap object.
 *
 * This function gets a stock bitmap object.
 * It will also set the color key of the BITMAP object by using the 
 * pixel of (ckey_x, ckey_y) if the position is valid when loading the 
 * stock bitmap for the first time.
 *
 * \param name The key name in the section of "bitmapinfo" in MiniGUI.cfg.
 * \param ckey_x The point of (ckey_x, ckey_y) defines the pixel in the bitmap 
 *        will be used as the color key. If either of \a ckey_x and \a ckey_y 
 *        is less than zero, there is no color key.
 * \param ckey_y The point of (ckey_x, ckey_y) defines the pixel in the bitmap 
 *        will be used as color key. If either of ckey_x and ckey_y is 
 *        less than zero, there is no color key.
 *
 * \return NULL on error, else the pointer to the system bitmap.
 *
 * \sa LoadSystemBitmapEx, LoadBitmapFromFile, UnloadBitmap
 */
MG_EXPORT const BITMAP* GUIAPI GetStockBitmap (const char* name, 
                int ckey_x, int ckey_y);

/**
 * \fn BOOL GUIAPI LoadSystemBitmapEx (PBITMAP pBitmap, \
 *               const char* szItemName, int ckey_x, int ckey_y)
 * \brief Loads a bitmap from information defined in MiniGUI.cfg.
 *
 * This function loads a bitmap from information defined in MiniGUI.cfg.
 * It will also set the color key of the BITMAP object by using the 
 * pixel of (ckey_x, ckey_y) if the pixel is valid.
 *
 * \param pBitmap The pointer to a bitmap object.
 * \param szItemName The key name in the section of "bitmapinfo" in MiniGUI.cfg.
 * \param ckey_x The point of (ckey_x, ckey_y) defines the pixel which
 *        will be used as color key in the bitmap. If either of ckey_x and 
 *        ckey_y is less than zero, there is no color key.
 * \param ckey_y See ckey_x.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa LoadSystemIcon, LoadBitmapFromFile, UnloadBitmap
 */
MG_EXPORT BOOL GUIAPI LoadSystemBitmapEx (PBITMAP pBitmap, 
                const char* szItemName, int ckey_x, int ckey_y);

/**
 * \def LoadSystemBitmap(pBitmap, szItemName)
 * \brief Loads a bitmap from information defined in MiniGUI.cfg.
 *
 * \note Defined as a macro calling \a LoadSystemBitmapEx without
 *       definition of color key pixel.
 *
 * \sa LoadSystemBitmapEx
 */
#define LoadSystemBitmap(pBitmap, szItemName) \
        LoadSystemBitmapEx (pBitmap, szItemName, -1, -1)

#define SYSICON_FOLD                "fold"
#define SYSICON_UNFOLD              "unfold"
#define SYSICON_FT_DIR              "dir"
#define SYSICON_FT_FILE             "file"

/**
 * \fn HICON GUIAPI LoadSystemIcon (const char* szItemName, int which)
 * \brief Loads an icon from information defined in MiniGUI.cfg.
 *
 * This function loads an icon from information defined in MiniGUI.cfg.
 *
 * \param szItemName The key name in the section of "iconinfo" in MiniGUI.cfg.
 * \param which Tell the function to load which icon.
 *
 * \return The handle to the loaded icon.
 *
 * \sa LoadSystemBitmap, LoadIconFromFile, DestroyIcon
 */
MG_EXPORT HICON GUIAPI LoadSystemIcon (const char* szItemName, int which);

    /** @} end of sys_bmps */

    /**
     * \defgroup scrollbar_fns Scroll bar operations
     * @{
     */

#define SB_HORZ     1
#define SB_VERT     2

/**
 * \fn BOOL GUIAPI EnableScrollBar (HWND hWnd, int iSBar, BOOL bEnable)
 * \brief Enables or disables one scroll bar arrows.
 *
 * This function enables or disables one scroll bar arrows.
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of 
 *        the following values:
 *          - SB_HORZ\n
 *            The horizontal scroll bar.
 *          - SB_VERT\n
 *            The vertical scroll bar.
 * \param bEnable Indicates whether to enable or disable the scroll bar.
 * \return TRUE on success, FALSE on error.
 *
 * \sa ShowScrollBar
 */
MG_EXPORT BOOL GUIAPI EnableScrollBar (HWND hWnd, int iSBar, BOOL bEnable);

/**
 * \fn BOOL GUIAPI GetScrollPos (HWND hWnd, int iSBar, int* pPos)
 * \brief Retrieves the current position of the scroll box (thumb) in the 
 *        specified scroll bar.
 *
 * This function retrieves the current position of the scroll box (thumb) in 
 * the specified scroll bar. The current position is a relative value that 
 * depends on the current scrolling range.
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of 
 *        the following values:
 *          - SB_HORZ\n
 *            The horizontal scroll bar.
 *          - SB_VERT\n
 *            The vertical scroll bar.
 * \param pPos The pointer to a integer which receives the position value.
 * \return TRUE on success, FALSE on error.
 *
 * \sa SetScrollPos
 */
MG_EXPORT BOOL GUIAPI GetScrollPos (HWND hWnd, int iSBar, int* pPos);

/**
 * \fn BOOL GUIAPI GetScrollRange (HWND hWnd, int iSBar, int* pMinPos, int* pMaxPos)
 * \brief Retrives the minimum and maximum position values for the specified 
 * scroll bar.
 *
 * This function retrives the minimum and maximum position values for 
 * the specified scroll bar.
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of 
 *        the following values:
 *          - SB_HORZ\n
 *            The horizontal scroll bar.
 *          - SB_VERT\n
 *            The vertical scroll bar.
 * \param pMinPos The pointer to a integer which receives the minimum 
 *        position value.
 * \param pMaxPos The pointer to a integer which receives the maximum 
 *        position value.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa SetScrollRange
 */
MG_EXPORT BOOL GUIAPI GetScrollRange (HWND hWnd, int iSBar, 
                int* pMinPos, int* pMaxPos);

/**
 * \fn BOOL GUIAPI SetScrollPos (HWND hWnd, int iSBar, int iNewPos)
 * \brief Sets the position of the scroll box (thumb) of the specified 
 * scroll bar.
 *
 * This function sets the position of the scroll box (thumb) of the specified 
 * scroll bar, and if needed, redraws the scroll bar to reflect the new 
 * position of the scroll box.
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of 
 *        the following values:
 *          - SB_HORZ\n
 *            The horizontal scroll bar.
 *          - SB_VERT\n
 *            The vertical scroll bar.
 * \param iNewPos The new position value.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa GetScrollPos
 */
MG_EXPORT BOOL GUIAPI SetScrollPos (HWND hWnd, int iSBar, int iNewPos);

/**
 * \fn BOOL GUIAPI SetScrollRange (HWND hWnd, int iSBar, \
 *               int iMinPos, int iMaxPos)
 * \brief Sets the minimum and maximum position values for the specified 
 * scroll bar.
 *
 * This function sets the minimum and maximum position values for the 
 * specified scroll bar, and if needed, redraws the scroll bar to reflect 
 * the new position values of the scroll box.
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of 
 *        the following values:
 *          - SB_HORZ\n
 *            The horizontal scroll bar.
 *          - SB_VERT\n
 *            The vertical scroll bar.
 * \param iMinPos The new minimum position value.
 * \param iMaxPos The new maximum position value.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa GetScrollRange
 */
MG_EXPORT BOOL GUIAPI SetScrollRange (HWND hWnd, int iSBar, 
                int iMinPos, int iMaxPos);

/**
 * \fn BOOL GUIAPI ShowScrollBar (HWND hWnd, int iSBar, BOOL bShow)
 * \brief Shows or hides the specified scroll bar.
 *
 * This function shows or hides the specified scroll bar.
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of 
 *        the following values:
 *          - SB_HORZ\n
 *            The horizontal scroll bar.
 *          - SB_VERT\n
 *            The vertical scroll bar.
 * \param bShow Indicates whether show or hide the scrollbar.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa EnableScrollBar
 */
MG_EXPORT BOOL GUIAPI ShowScrollBar (HWND hWnd, int iSBar, BOOL bShow);

#define SIF_RANGE           0x0001
#define SIF_PAGE            0x0002
#define SIF_POS             0x0004
#define SIF_DISABLENOSCROLL 0x0008
#define SIF_TRACKPOS        0x0010
#define SIF_ALL             (SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS)

/**
 * Scroll bar information structure.
 */
typedef struct _SCROLLINFO
{
    /** Size of the structrue in bytes */
    UINT    cbSize;
    /**
     * A flag indicates which fields contain valid values, 
     * can be OR'ed value of the following values:
     *      - SIF_RANGE\n
     *        Retrives or sets the range of the scroll bar.
     *      - SIF_PAGE\n
     *        Retrives or sets the page size of the scroll bar.
     *      - SIF_POS\n
     *        Retrives or sets the position of the scroll bar.
     *      - SIF_DISABLENOSCROLL\n
     *        Hides the scroll when disabled, not implemented so far.
     */
    UINT    fMask;
    /** The minimum position value of the scroll bar */
    int     nMin;
    /** The maximum position value of the scroll bar */
    int     nMax;
    /** The page size of the scroll bar */
    UINT    nPage;
    /** The position value of the scroll bar */
    int     nPos;
#if 0
    int     nTrackPos;
#endif
} SCROLLINFO, *PSCROLLINFO;

/**
 * \fn BOOL GUIAPI SetScrollInfo (HWND hWnd, int iSBar, \
 *                const SCROLLINFO* lpsi, BOOL fRedraw)
 * \brief Sets the parameters of a scroll bar.
 *
 * This function sets the parameters of a scroll bar, including the 
 * minimum and maximum scrolling positions, the page size, and the position 
 * of the scroll box (thumb). The function also redraws the scroll bar, 
 * if requested.
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of 
 *        the following values:
 *          - SB_HORZ\n
 *            The horizontal scroll bar.
 *          - SB_VERT\n
 *            The vertical scroll bar.
 * \param lpsi The pointer to the structure of scroll bar information.
 * \param fRedraw A boolean indicates whether to redraw the scroll bar.
 * \return TRUE on success, FALSE on error.
 *
 * \sa GetScrollInfo, SCROLLINFO
 */
MG_EXPORT BOOL GUIAPI SetScrollInfo (HWND hWnd, int iSBar, 
                const SCROLLINFO* lpsi, BOOL fRedraw);

/**
 * \fn BOOL GUIAPI GetScrollInfo (HWND hWnd, int iSBar, PSCROLLINFO lpsi)
 * \brief Retrieves the parameters of a scroll bar.
 *
 * This function retrieves the parameters of a scroll bar, including the 
 * minimum and maximum scrolling positions, the page size, and the position 
 * of the scroll box (thumb).
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of 
 *        the following values:
 *          - SB_HORZ\n
 *            The horizontal scroll bar.
 *          - SB_VERT\n
 *            The vertical scroll bar.
 * \param lpsi The pointer to a structure of SCROLLINFO which receives the 
 *        parameters.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa SetScrollInfo, SCROLLINFO
 */
MG_EXPORT BOOL GUIAPI GetScrollInfo (HWND hWnd, int iSBar, PSCROLLINFO lpsi);

    /** @} end of scrollbar_fns */

    /**
     * \defgroup class_fns Window class operations
     * @{
     */

/* Class styles -- not supported so far */
#define CS_VREDRAW          0x0001
#define CS_HREDRAW          0x0002
#define CS_KEYCVTWINDOW     0x0004
#define CS_DBLCLKS          0x0008

#define CS_OWNDC            0x0020
#define CS_CLASSDC          0x0040
#define CS_PARENTDC         0x0080

#define CS_NOKEYCVT         0x0100
#define CS_NOCLOSE          0x0200
#define CS_SAVEBITS         0x0800

#define CS_BYTEALIGNCLIENT  0x1000
#define CS_BYTEALIGNWINDOW  0x2000
#define CS_GLOBALCLASS      0x4000
#define CS_IME              0x8000

#define COP_STYLE           0x0001
#define COP_HCURSOR         0x0002
#define COP_BKCOLOR         0x0004
#define COP_WINPROC         0x0008
#define COP_ADDDATA         0x0010

/**
 * Structure defines a window class
 */
typedef struct _WNDCLASS
{
    /** The class name */
    char*   spClassName;

    /** Internal field, operation type */
    DWORD   opMask;

    /** Window style for all instances of this window class */
    DWORD   dwStyle;

    /** Extended window style for all instances of this window class */
    DWORD   dwExStyle;

    /** Cursor handle to all instances of this window class */
    HCURSOR hCursor;

    /** Background color pixel value of all instances of this window class */
    int     iBkColor;

    /** Window callback procedure of all instances of this window class */
    int     (*WinProc) (HWND, int, WPARAM, LPARAM);

    /** The private additional data associated with this window class */
    DWORD dwAddData;
} WNDCLASS;
typedef WNDCLASS* PWNDCLASS;

#define MAINWINCLASSNAME    ("MAINWINDOW")
#define ROOTWINCLASSNAME    ("ROOTWINDOW")

/**
 * \fn BOOL GUIAPI RegisterWindowClass (PWNDCLASS pWndClass)
 * \brief Registers a window class. 
 *
 * This function registers a window class. 
 * Later on, you can create a window of the registered class.
 *
 * \param pWndClass The pointer to a WNDCLASS structure which specifies 
 *        the information of the window class.
 *
 * \return TRUE on success, FALSE on error.
 * 
 * \sa UnregisterWindowClass, WNDCLASS
 *
 * Example:
 *
 * \include registerwindowclass.c
 */
MG_EXPORT BOOL GUIAPI RegisterWindowClass (PWNDCLASS pWndClass);

/**
 * \fn BOOL GUIAPI UnregisterWindowClass (const char* szClassName)
 * \brief Undoes the effect of \a RegisterWindowClass.
 *
 * This function unregisters a registered window class specified by 
 * \a szClassName.
 * 
 * \param szClassName The name of the class to be unregistered.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa RegisterWindowClass
 */
MG_EXPORT BOOL GUIAPI UnregisterWindowClass (const char* szClassName);

/**
 * \fn const char* GUIAPI GetClassName (HWND hWnd)
 * \brief Retrieves the name of the class to which the specified window belongs.
 *
 * This function retrieves the name of the class to which 
 * the specified window \a hWnd belongs.
 *
 * \param hWnd The handle to the window.
 *
 * \return The pointer to a const class name string, NULL on error.
 *
 * \sa RegisterWindowClass
 */
MG_EXPORT const char* GUIAPI GetClassName (HWND hWnd);

/**
 * \fn BOOL GUIAPI GetWindowClassInfo (PWNDCLASS pWndClass)
 * \brief Retrieves the information of the specified window class.
 *
 * This function retrives the information of a window class.
 * The window class to be retrived is specified by \a pWndClass->spClassName.
 *
 * \param pWndClass The pointer to a WNDCLASS structure, which specifies the 
 *        window class to be retrived via \a spClassName field, and
 *        returns the information through other fields.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa SetWindowClassInfo
 */
MG_EXPORT BOOL GUIAPI GetWindowClassInfo (PWNDCLASS pWndClass);

/**
 * \fn BOOL GUIAPI SetWindowClassInfo (const WNDCLASS* pWndClass)
 * \brief Sets the information of the specified window class.
 *
 * This function sets the information of a window class.
 * The window class to be operated is specified by \a pWndClass->spClassName.
 *
 * \param pWndClass The pointer to a WNDCLASS structure, which specifies
 *        the new information of the window class.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa GetWindowClassInfo
 */
MG_EXPORT BOOL GUIAPI SetWindowClassInfo (const WNDCLASS* pWndClass);

    /** @} end of class_fns */

    /**
     * \defgroup control_fns Control creating/destroying
     * @{
     */

/**
 * \fn HWND GUIAPI CreateWindowEx (const char* spClassName, \
 *                const char* spCaption, DWORD dwStyle, DWORD dwExStyle, \
 *               int id, int x, int y, int w, int h, HWND hParentWnd, \
 *               DWORD dwAddData)
 * \brief Creates a child window with extended style.
 *
 * This function creates a child window (also known as "control") with 
 * extended style. It specifies the window class, the window title, 
 * the window style, the window extended style, the initial position, 
 * and the size of the window.  The function also specifies the 
 * window's parent or owner.
 *
 * \param spClassName The class name of the control.
 * \param spCaption The caption of the control.
 * \param dwStyle The control style.
 * \param dwExStyle The extended control style.
 * \param id The identifier of the control.
 * \param x x,y: The initial position of the control in the parent window.
 * \param y x,y: The initial position of the control in the parent window.
 * \param w The initial width of the control.
 * \param h The initial height of the control.
 * \param hParentWnd The handle to the parent window.
 * \param dwAddData The first private additional data of the control.
 *        Note that some control classes use this value to initialize 
 *        some properties of the new control instance. For these control 
 *        classes, you should pass a valid value to it.
 *
 * \return The handle to the new control, HWND_INVALID on error.
 *
 * \sa CreateMainWindow, CTRLDATA
 *
 * Example:
 *
 * \include createwindow.c
 */
MG_EXPORT HWND GUIAPI CreateWindowEx (const char* spClassName, 
                const char* spCaption, DWORD dwStyle, DWORD dwExStyle, 
                int id, int x, int y, int w, int h, HWND hParentWnd, 
                DWORD dwAddData);

/**
 * \fn BOOL GUIAPI DestroyWindow (HWND hWnd)
 * \brief Destroys a specified control.
 *
 * This function destroys the specified control \a hWnd, which is created
 * by \a CreateWindowEx.
 *
 * \param hWnd The handle to the control.
 * \return TRUE on success, FALSE on error.
 *
 * \sa CreateWindowEx
 */
MG_EXPORT BOOL GUIAPI DestroyWindow (HWND hWnd);

/**
 * \var typedef void (* NOTIFPROC)(HWND hwnd, int id, int nc, DWORD add_data)
 * \brief Type of the notification callback procedure.
 *
 * This is the function type of Notification Callback Procedure.
 * If you set the Notification Callback Procedure for a control,
 * when a notification occurred the control will call this callback
 * procedure.
 *
 * \sa SetNotificationCallback
 */
typedef void (* NOTIFPROC) (HWND hwnd, int id, int nc, DWORD add_data);

/**
 * \fn NOTIFPROC GUIAPI SetNotificationCallback (HWND hwnd, \
 *                NOTIFPROC notif_proc)
 * \brief Sets a new notification callback procedure for a control.
 *
 * This function sets the new notification callback procedure (\a notif_proc) 
 * for the control of \a hwnd.
 *
 * By default, the notification from a control will be sent to its parent
 * window within a MSG_COMMAND messsage.
 *
 * Since version 1.2.6, MiniGUI defines the Notification Callback Procedure 
 * for control. You can specify a callback function for a control by calling
 * \a SetNotificationCallback to receive and handle the notifications from 
 * the control.
 *
 * If you did not set the notification callback function for a control,
 * the notification will be sent to its parent as same as the earlier 
 * version of MiniGUI.
 *
 * \param hwnd The handle to the control.
 * \param notif_proc The new notification callback procedure, can be NULL.
 * \return The old notification callback procedure.
 *
 * \sa NOTIFPROC, GetNotificationCallback
 */
MG_EXPORT NOTIFPROC GUIAPI SetNotificationCallback (HWND hwnd, 
                NOTIFPROC notif_proc);

/**
 * \fn NOTIFPROC GUIAPI GetNotificationCallback (HWND hwnd)
 * \brief Gets the notification callback procedure of a control.
 *
 * This function gets the new notification callback procedure of 
 * the control of \a hwnd.
 *
 * \param hwnd The handle to the control.
 *
 * \return The notification callback procedure.
 *
 * \sa NOTIFPROC, SetNotificationCallback
 */
MG_EXPORT NOTIFPROC GUIAPI GetNotificationCallback (HWND hwnd);

/**
 * \def CreateWindow(class_name, caption, style, id, x, y, w, h, parent, add_data)
 * \brief A simplified version of \a CreateWindowEx.
 *
 * \sa CreateWindowEx
 */
#define CreateWindow(class_name, caption, style,        \
                id, x, y, w, h, parent, add_data)       \
        CreateWindowEx(class_name, caption, style, 0,   \
                        id, x, y, w, h, parent, add_data)

    /** @} end of control_fns */

/******************************** Timer Support ******************************/
    /**
     * \defgroup timer_fns Timer operations
     * @{
     */

/**
 * \var typedef BOOL (* TIMERPROC)(HWND, int, DWORD)
 * \brief Type of the timer callback procedure.
 *
 * This is the prototype of the callback procedure of a timer created by SetTimerEx.
 * MiniGUI will call the timer procedure instead of sending MSG_TIMER message.
 *
 * If the return value of a timer procedure is FALSE, the timer will be killed
 * by MiniGUI automatically. This can be used to implement a one-shot timer.
 *
 * \sa SetTimerEx
 */
typedef BOOL (* TIMERPROC)(HWND, int, DWORD);

/**
 * \fn BOOL GUIAPI SetTimerEx (HWND hWnd, int id, unsigned int speed, \
 *              TIMERPROC timer_proc)
 * \brief Creates a timer with the specified timeout value.
 *
 * This function creates a timer with the specified timeout value \a speed.
 * Note that the timeout value is in unit of 10 ms.
 * When the timer expires, an MSG_TIMER message will be send to the 
 * window \a hWnd if \a timer_proc is NULL, otherwise MiniGUI will call
 * \a timer_proc by passing \a hWnd, \a id, and the tick count when this
 * timer had expired to this callback procedure.
 *
 * \param hWnd The window receives the MSG_TIMER message. If \a timer_proc
 *        is not NULL, MiniGUI will call \a timer_proc instead sending
 *        MSG_TIMER message to this window. If you use timer callback 
 *        procedure, \a hWnd can be any value you can pass.
 * \param id The identifier of the timer, will be passed to the window
 *        with MSG_TIMER message as the first parameter of the message.
 * \param speed The timeout value of the timer. Note that the timeout value 
 *        is in unit of 10 ms.
 * \param timer_proc The timer callback procedure. If this argument is NULL,
 *        MiniGUI will send MSG_TIMER to the window procedure of \a hWnd.
 * \return TRUE on success, FALSE on error.
 * 
 * \sa SetTimer, ResetTimerEx, KillTimer, MSG_TIMER
 *
 * \note You should set, reset, and kill a timer in the same thread if your
 *       MiniGUI is configured as MiniGUI-Threads.
 *
 * Example:
 *
 * \include settimer.c
 */
MG_EXPORT BOOL GUIAPI SetTimerEx (HWND hWnd, int id, unsigned int speed, 
                TIMERPROC timer_proc);

/**
 * \def SetTimer(hwnd, id, speed)
 * \brief The backward compatibility version of SetTimerEx.
 *
 * \sa SetTimerEx
 */
#define SetTimer(hwnd, id, speed) \
                SetTimerEx(hwnd, id, speed, NULL)

/**
 * \fn int GUIAPI KillTimer (HWND hWnd, int id)
 * \brief Destroys a timer.
 *
 * This function destroys the specified timer \a id.
 *
 * \param hWnd The window owns the timer.
 * \param id The identifier of the timer. If \a id equals or is less than 0, 
 *        this function will kill all timers in the system.
 *
 * \return The number of actually killed timer.
 *
 * \sa SetTimer
 */
MG_EXPORT int GUIAPI KillTimer (HWND hWnd, int id);

/**
 * \fn BOOL GUIAPI ResetTimerEx (HWND hWnd, int id, unsigned int speed, \
 *              TIMERPROC timer_proc)
 * \brief Adjusts a timer with a different timeout value or different
 *        timer callback procedure.
 *
 * This function resets a timer with the specified timeout \a speed value.
 *
 * \param hWnd The window owns the timer.
 * \param id The identifier of the timer.
 * \param speed The new timeout value.
 * \param timer_proc The new timer callback procedure. If \a timer_proc
 *        is 0xFFFFFFFF, the setting of timer callback procedure will
 *        not changed.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa SetTimerEx
 */
MG_EXPORT BOOL GUIAPI ResetTimerEx (HWND hWnd, int id, unsigned int speed,
                TIMERPROC timer_proc);

/**
 * \def ResetTimer(hwnd, id, speed)
 * \brief The backward compatibility version of ResetTimerEx.
 *
 * \sa ResetTimerEx
 */
#define ResetTimer(hwnd, id, speed) \
                ResetTimerEx(hwnd, id, speed, (TIMERPROC)0xFFFFFFFF)

/**
 * \fn unsigned int GUIAPI GetTickCount (void)
 * \brief Retrieves the tick counts that have elapsed since MiniGUI was started.
 *
 * This function retrieves the tick counts that have elapsed since MiniGUI 
 * was started. It is limited to the resolution of the system timer, i.e. 
 * for a general Linux box, the returned tick count value is in unit of 10ms.
 *
 * \return The tick counts value that have elapsed since MiniGUI was started.
 */
MG_EXPORT unsigned int GUIAPI GetTickCount (void);

/**
 * \fn BOOL GUIAPI IsTimerInstalled (HWND hWnd, int id)
 * \brief Determines whether a timer is installed.
 *
 * This function determines whether a timer with identifier \a id of 
 * a window \a hwnd has been installed.
 *
 * \param hWnd The window owns the timer.
 * \param id The identifier of the timer.
 *
 * \return TRUE for installed, otherwise FALSE. 
 *
 * \sa SetTimer, HaveFreeTimer
 */
MG_EXPORT BOOL GUIAPI IsTimerInstalled (HWND hWnd, int id);

/**
 * \fn BOOL GUIAPI HaveFreeTimer (void)
 * \brief Determines whether there is any free timer slot in the system.
 *
 * This function determines whether there is any free timer slot in the
 * system.
 *
 * \return TRUE for yes, otherwise FALSE. 
 *
 * \sa IsTimerInstalled
 */
MG_EXPORT BOOL GUIAPI HaveFreeTimer (void);

    /** @} end of timer_fns */

    /**
     * \defgroup ime_fns IME Window functions
     * @{
     */

#ifdef _IME_GB2312

/**
 * \fn int GBIMEWinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
 * \brief The window callback procedure of the builit-in GB2312 Input Method 
 *        window.
 *
 * This function is the window callback procedure of the built-in GB2312 
 * (a charset for simplified Chinese) Input Method window.
 *
 * \note This function defined for _IME_GB2312. You can disable GB2312 IME 
 * support by using
 * \code
 * ./configure --disable-imegb2312
 * \endcode
 *
 * \sa GBIMEWindowEx
 */
int GBIMEWinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

#ifdef _LITE_VERSION

/**
 * \fn HWND GBIMEWindowEx (HWND hosting, int lx, int ty, int rx, int by, \
 *               BOOL two_lines)
 * \brief Creates a GB2312 input method window.
 *
 * This function creates a GB2312 (a charset for simplified Chinese) input 
 * method window. Before calling this window, you must create a main window 
 * which acts as the hosting main window of the IME window.
 *
 * \param hosting The hosting window of the IME window. 
 *        Do not set it to be HWND_DESKTOP.
 * \param lx lx,ty,rx,by: The position and size of the IME window.
 * \param ty lx,ty,rx,by: The position and size of the IME window.
 * \param rx lx,ty,rx,by: The position and size of the IME window.
 * \param by lx,ty,rx,by: The position and size of the IME window.
 * \param two_lines Indicates whether the IME window contains two lines.
 *
 * \return The handle to the IME window.
 *
 * \note This function only define for MiniGUI-Processes, and can be 
 *       called only by the server of MiniGUI-Processes.
 *
 * \sa GBIMEWindow
 */
MG_EXPORT HWND GBIMEWindowEx (HWND hosting, int lx, int ty, int rx, int by, 
                BOOL two_lines);

/**
 * \def GBIMEWindow(hosting)
 * \brief The simplified version of \a GBIMEWindowEx.
 *
 * \param hosting The hosting window of the IME window. 
 * \return The handle to the IME window.
 *
 * \note For MiniGUI-Threads, \a GBIMEWindow defined as a function:
 *
 * \code 
 *      HWND GBIMEWindow (HWND hosting);
 * \endcode
 *
 * \note Calling this function will create a thread and start the 
 *       GB2312 IME window in this thread.
 *
 * \sa GBIMEWindowEx
 */
#define GBIMEWindow(hosting) GBIMEWindowEx(hosting, 0, 0, 0, 0, TRUE)

#else

MG_EXPORT HWND GBIMEWindow (HWND hosting);

#endif
#endif

/**
 * \fn int GUIAPI RegisterIMEWindow (HWND hWnd)
 * \brief Registers an IME window.
 *
 * This function registers the specified window \a hWnd as the IME window of 
 * the MiniGUI. After that, the keyboard input will be sent to IME window 
 * first. Note that only one IME window can be registered.
 *
 * \param hWnd The handle to your IME window.
 *
 * \return ERR_OK on success, otherwise less than zero.
 *
 * \retval ERR_OK Success.
 * \retval ERR_IME_TOOMUCHIMEWND Already have an IME window registered.
 * \retval ERR_INV_HWND Invalid main window handle.
 *
 * \sa UnregisterIMEWindow
 */
MG_EXPORT int GUIAPI RegisterIMEWindow (HWND hWnd);

/**
 * \fn int GUIAPI UnregisterIMEWindow (HWND hWnd)
 * \brief Unregisters an IME window.
 *
 * This function undoes the effect of \a RegisterIMEWindow.
 *
 * \param hWnd The handle to the current IME window.
 * \return ERR_OK on success, otherwise less than zero.
 *
 * \retval ERR_OK Success.
 * \retval ERR_IME_NOSUCHIMEWND The window is not the current IME window.
 *
 * \sa RegisterIMEWindow
 */
MG_EXPORT int GUIAPI UnregisterIMEWindow (HWND hWnd);

/**
 * \fn int GUIAPI GetIMEStatus (int StatusCode)
 * \brief Retrives status of the current IME window.
 *
 * This function retrives status of the current IME window.
 *
 * \param StatusCode The item to be retrived, can be one of the following 
 * values:
 *      - IS_ENABLE\n
 *        Is the IME window enabled?
 *      - IS_FULLCHAR\n
 *        Whether translate half character to full character?
 *      - IS_FULLPUNC\n
 *        Whether tranlsate half punctuate mark to full mark?
 *      - IS_METHOD\n
 *        Which input method?
 *
 * \return The status of the item specified by \a StatusCode, 
 *        ERR_IME_NOIMEWND if error occurred.
 *
 * \sa SetIMEStatus
 */
MG_EXPORT int GUIAPI GetIMEStatus (int StatusCode);

/**
 * \fn int GUIAPI SetIMEStatus (int StatusCode, int Value)
 * \brief Sets the status of the current IME window.
 *
 * This function sets the status of the current IME window.
 *
 * \param StatusCode The item to be set, can be one of the following values:
 *      - IS_ENABLE\n
 *        Enabled or disable IME window.
 *      - IS_FULLCHAR\n
 *        Whether translate half character to full character?
 *      - IS_FULLPUNC\n
 *        Whether tranlsate half punctuate mark to full mark?
 *      - IS_METHOD\n
 *        Which input method?
 * \param Value The status value of the item.
 * \return ERR_OK on success, otherwise less than zero.
 *
 * \retval ERR_OK Success.
 * \retval ERR_IME_NOIMEWND There is no any IME window registered.
 *
 * \sa GetIMEStatus
 */
MG_EXPORT int GUIAPI SetIMEStatus (int StatusCode, int Value);

    /** @} end of ime_fns */

    /**
     * \defgroup accel_fns Accelerator operations
     * @{
     */

/**
 * \fn HACCEL GUIAPI CopyAcceleratorTable (HACCEL hacc)
 * \brief Copies the specified accelerator table.
 *
 * This function copies the specified accelerator table. 
 * The function is used to obtain the accelerator table data that 
 * corresponds to an accelerator table handle.
 *
 * \param hacc The handle to the accelerator table.
 * \return The handle to the copied new accelerator table.
 */
MG_EXPORT HACCEL GUIAPI CopyAcceleratorTable (HACCEL hacc);

/**
 * \fn int GUIAPI DeleteAccelerators (HACCEL hacc, int key, DWORD keymask)
 * \brief Deletes an accelerator from the accelerator table.
 *
 * This function deletes an accelerator from the accelerator table.
 *
 * \param hacc The handle to the accelerator table.
 * \param key The key value of the accelerator.
 * \param keymask The shift key state.
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa AddAccelerators, DestroyAcceleratorTable
 */
MG_EXPORT int GUIAPI DeleteAccelerators (HACCEL hacc, int key, DWORD keymask);

#define ACCEL_SHIFT     KS_SHIFT
#define ACCEL_ALT       KS_ALT
#define ACCEL_CTRL      KS_CTRL

/**
 * \fn int GUIAPI AddAccelerators (HACCEL hacc, int key, \
 *               DWORD keymask, WPARAM wParam, LPARAM lParam)
 * \brief Adds an accelerator to the accelerator table.
 *
 * This function adds an accelerator to the accelerator table. 
 * It's called after you call \a CreateAcceleratorTable function.
 *
 * \param hacc The handle to the accelerator table.
 * \param key The key value of the accelerator. Note that you can define the 
 *        key value by either scancode or ASCII code. If you define the key 
 *        with scancode, the key value should be equal to (scancode + 256).
 * \param keymask The shift key state, can be OR'ed value of the following 
 *        values:
 *      - KS_SHIFT\n
 *        Shift key should be pressed.
 *      - KS_ALT\n
 *        Alt key should be pressed.
 *      - KS_CTRL\n
 *        Ctrl key should be pressed.
 * \param wParam The first parameter of the MSG_COMMAND when receiving such 
 *        a accelerator key.
 * \param lParam The second parameter of the MSG_COMMAND when receiving such 
 *        a accelerator key.
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa CreateAcceleratorTable, DeleteAccelerators
 */
MG_EXPORT int GUIAPI AddAccelerators (HACCEL hacc, int key, 
                DWORD keymask, WPARAM wParam, LPARAM lParam);

/**
 * \fn int GUIAPI DestroyAcceleratorTable (HACCEL hacc)
 * \brief Destroys an accelerator table.
 *
 * This function destroys the specified accelerator table \a hacc. 
 * Before closing a window, you must call this function to destroy 
 * each accelerator table that is created by using \a CreateAcceleratorTable 
 * function.
 *
 * \param hacc The handle to the accelerator table.
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa CreateAcceleratorTable
 */
MG_EXPORT int GUIAPI DestroyAcceleratorTable (HACCEL hacc);

/**
 * \fn HACCEL GUIAPI CreateAcceleratorTable (HWND hWnd)
 * \brief Creates an empty accelerator table.
 *
 * This function creates an empty accelerator table.
 *
 * \param hWnd The handle to the main window.
 * \return The handle to the new accelerator table, zero when error.
 *
 * \sa AddAccelerators, DestroyAcceleratorTable
 */
MG_EXPORT HACCEL GUIAPI CreateAcceleratorTable (HWND hWnd);

/**
 * \fn int GUIAPI TranslateAccelerator (HACCEL hAccel, PMSG pMsg)
 * \brief Translates an accelerator key message to MSG_COMMAND messge 
 * and sends it to the window procedure.
 *
 * \param hAccel The handle to the accelerator table.
 * \param pMsg The pointer to the MSG structure to be translated.
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa TranslateMessage
 */
MG_EXPORT int GUIAPI TranslateAccelerator (HACCEL hAccel, PMSG pMsg);

    /** @} end of accel_fns */

    /**
     * \defgroup caret_fns Caret operations
     * @{
     */

/**
 * \fn BOOL GUIAPI CreateCaret (HWND hWnd, PBITMAP pBitmap, \
 *               int nWidth, int nHeight)
 * \brief Creates a new shape for the system caret and assigns ownership of 
 * the caret to the specified window. 
 *
 * This function creates a new shape for the system caret and assigns 
 * ownership of the caret to the specified window \a hWnd. The caret shape 
 * can be a line, a block, or a bitmap.
 *
 * \param hWnd The owner of the caret.
 * \param pBitmap The bitmap shape of the caret. It can be NULL.
 * \param nWidth The width of the caret.
 * \param nHeight The height of the caret.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa ActiveCaret, ChangeCaretSize
 */
MG_EXPORT BOOL GUIAPI CreateCaret (HWND hWnd, PBITMAP pBitmap, 
                int nWidth, int nHeight);

/**
 * \fn BOOL GUIAPI ChangeCaretSize (HWND hWnd, int newWidth, int newHeight)
 * \brief Changes the size of the caret.
 *
 * This function change the size of the caret owned by the specified window
 * \a hWnd. Note that the new size of the caret should be smaller than
 * the original size which is specified when you call \a CreateCaret.
 *
 * \param hWnd The owner of the caret.
 * \param newWidth The new width of the caret.
 * \param newHeight The new height of the caret.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa CreateCaret
 */
MG_EXPORT BOOL GUIAPI ChangeCaretSize (HWND hWnd, int newWidth, int newHeight);

/**
 * \fn BOOL GUIAPI ActiveCaret (HWND hWnd)
 * \brief Activates the caret owned by a window.
 *
 * This function activates the caret owned by the specified window \a hWnd.
 * After activating the caret, the caret will begin flashing automatically.
 *
 * \param hWnd The owner of the caret.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa HideCaret, ShowCaret
 */
MG_EXPORT BOOL GUIAPI ActiveCaret (HWND hWnd);

/**
 * \fn UINT GUIAPI GetCaretBlinkTime (HWND hWnd)
 * \brief Returns the elapsed time, in milliseconds, required to invert the 
 *        caret's pixels.
 *
 * This function returns the elapsed time, in milliseconds, required to 
 * invert the caret's pixels. The default blink time of a caret is 500 
 * milliseconds.
 *
 * \param hWnd The owner of the caret.
 *
 * \return The elapsed time in milliseconds. Returns zero on error.
 *
 * \sa SetCaretBlinkTime
 */
MG_EXPORT UINT GUIAPI GetCaretBlinkTime (HWND hWnd);

/**
 * \fn BOOL GUIAPI SetCaretBlinkTime (HWND hWnd, UINT uTime)
 * \brief Sets the caret blink time to the specified number of milliseconds.
 *
 * This function sets the caret blink time to the specified number of 
 * milliseconds. The blink time is the elapsed time, in milliseconds, 
 * required to invert the caret's pixels. The default blink time of a caret 
 * is 500 milliseconds.
 *
 * \param hWnd The owner of the caret.
 * \param uTime New blink time in milliseconds.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa GetCaretBlinkTime
 */
MG_EXPORT BOOL GUIAPI SetCaretBlinkTime (HWND hWnd, UINT uTime);

/**
 * \fn BOOL GUIAPI DestroyCaret (HWND hWnd)
 * \brief Destroys a caret.
 *
 * This function destroys the caret's current shape, 
 * frees the caret from the window, and removes the caret from the screen.
 *
 * \param hWnd The owner of the caret.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa CreateCaret
 */
MG_EXPORT BOOL GUIAPI DestroyCaret (HWND hWnd);

/**
 * \fn BOOL GUIAPI HideCaret (HWND hWnd)
 * \brief Hides a caret.
 *
 * This function removes the caret from the screen. 
 * Hiding a caret does not destroy its current shape or invalidate the 
 * insertion point.
 *
 * \param hWnd The owner of the caret.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa ShowCaret
 */
MG_EXPORT BOOL GUIAPI HideCaret (HWND hWnd);

/**
 * \fn BOOL GUIAPI ShowCaret (HWND hWnd)
 * \brief Shows a caret.
 *
 * This function makes the caret visible on the screen at the caret's 
 * current position. When the caret becomes visible, it begins flashing 
 * automatically.
 *
 * \param hWnd The owner of the caret.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa HideCaret
 */
MG_EXPORT BOOL GUIAPI ShowCaret (HWND hWnd);

/**
 * \fn BOOL GUIAPI SetCaretPos (HWND hWnd, int x, int y)
 * \brief Moves the caret to a new position.
 *
 * This function moves the caret to the specified coordinates \a (x,y).
 *
 * \param hWnd The owner of the caret.
 * \param x x,y: The position of the caret in client coordiantes of 
 *        the owner window.
 * \param y x,y: The position of the caret in client coordiantes of
 *        the owner window.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa GetCaretPos
 */
MG_EXPORT BOOL GUIAPI SetCaretPos (HWND hWnd, int x, int y);

/**
 * \fn BOOL GUIAPI GetCaretPos (HWND hWnd, PPOINT pPt)
 * \brief Gets the caret position.
 *
 * This function copies the caret's position, in client coordinates, 
 * to the specified POINT structure \a pPt.
 *
 * \param hWnd The owner of the caret.
 * \param pPt The pointer to the buffer saves the caret's position.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa SetCaretPos
 */
MG_EXPORT BOOL GUIAPI GetCaretPos (HWND hWnd, PPOINT pPt);

    /** @} end of caret_fns */

    /**
     * \defgroup menu_fns Menu operations
     * @{
     */

/* Menu flags */
#define MF_INSERT           0x00000000L
#define MF_CHANGE           0x00000080L
#define MF_APPEND           0x00000100L
#define MF_DELETE           0x00000200L
#define MF_REMOVE           0x00001000L

#define MF_BYCOMMAND        0x00000000L
#define MF_BYPOSITION       0x00000400L

#define MF_SEPARATOR        0x00000800L

#define MF_ENABLED          0x00000000L
#define MF_GRAYED           0x00000001L
#define MF_DISABLED         0x00000002L

#define MF_UNCHECKED        0x00000000L
#define MF_CHECKED          0x00000008L
#define MF_USECHECKBITMAPS  0x00000200L

#define MF_STRING           0x00000000L
#define MF_BITMAP           0x00000004L
#define MF_OWNERDRAW        0x00000100L

#define MF_POPUP            0x00000010L
#define MF_MENUBARBREAK     0x00000020L
#define MF_MENUBREAK        0x00000040L

#define MF_UNHILITE         0x00000000L
#define MF_HILITE           0x00000080L

#define MF_DEFAULT          0x00001000L
#define MF_SYSMENU          0x00002000L
#define MF_HELP             0x00004000L
#define MF_RIGHTJUSTIFY     0x00004000L

#define MF_MOUSESELECT      0x00008000L
#define MF_END              0x00000080L

#define MFT_STRING          MF_STRING
#define MFT_BITMAP          MF_BITMAP
#define MFT_BMPSTRING       0x00010000L
#define MFT_MENUBARBREAK    MF_MENUBARBREAK
#define MFT_MENUBREAK       MF_MENUBREAK
#define MFT_OWNERDRAW       MF_OWNERDRAW
#define MFT_RADIOCHECK      0x00000200L
#define MFT_SEPARATOR       MF_SEPARATOR
#define MFT_RIGHTORDER      0x00002000L
#define MFT_RIGHTJUSTIFY    MF_RIGHTJUSTIFY

#define MFS_GRAYED          0x00000003L
#define MFS_DISABLED        MFS_GRAYED
#define MFS_CHECKED         MF_CHECKED
#define MFS_HILITE          MF_HILITE
#define MFS_ENABLED         MF_ENABLED
#define MFS_UNCHECKED       MF_UNCHECKED
#define MFS_UNHILITE        MF_UNHILITE
#define MFS_DEFAULT         MF_DEFAULT

/* System Menu Command Values */
  #define SC_SIZE         0xF000
  #define SC_MOVE         0xF010
  #define SC_MINIMIZE     0xF020
  #define SC_MAXIMIZE     0xF030
  #define SC_NEXTWINDOW   0xF040
  #define SC_PREVWINDOW   0xF050
  #define SC_CLOSE        0xF060
  #define SC_VSCROLL      0xF070
  #define SC_HSCROLL      0xF080
  #define SC_MOUSEMENU    0xF090
  #define SC_KEYMENU      0xF100
  #define SC_ARRANGE      0xF110
  #define SC_RESTORE      0xF120
  #define SC_TASKLIST     0xF130
  #define SC_SCREENSAVE   0xF140
  #define SC_HOTKEY       0xF150

  #define SC_DEFAULT      0xF160
  #define SC_MONITORPOWER 0xF170
  #define SC_CONTEXTHELP  0xF180
  #define SC_SEPARATOR    0xF00F

/* MENUITEMINFO flags */
  #define MIIM_STATE       0x00000001
  #define MIIM_ID          0x00000002
  #define MIIM_SUBMENU     0x00000004
  #define MIIM_CHECKMARKS  0x00000008
  #define MIIM_TYPE        0x00000010
  #define MIIM_DATA        0x00000020
  #define MIIM_STRING      0x00000040
  #define MIIM_BITMAP      0x00000080
  #define MIIM_FTYPE       0x00000100


/* Flags for TrackPopupMenu */
  #define TPM_LEFTBUTTON  0x0000L
  #define TPM_RIGHTBUTTON 0x0002L
  #define TPM_LEFTALIGN   0x0000L
  #define TPM_CENTERALIGN 0x0004L
  #define TPM_RIGHTALIGN  0x0008L
  #define TPM_TOPALIGN        0x0000L
  #define TPM_VCENTERALIGN    0x0010L
  #define TPM_BOTTOMALIGN     0x0020L

  #define TPM_HORIZONTAL      0x0000L     /* Horz alignment matters more */
  #define TPM_VERTICAL        0x0040L     /* Vert alignment matters more */
  #define TPM_NONOTIFY        0x0080L     /* Don't send any notification msgs */
  #define TPM_RETURNCMD       0x0100L

  #define TPM_DESTROY         0x1000L       /* Destroy menu after tracking */
  #define TPM_SYSCMD          0x2000L       /* Send system command */
  #define TPM_DEFAULT         0x0000L       /* Default tracking flag */

/* return codes for MSG_MENUCHAR */
#define MNC_IGNORE  0
#define MNC_CLOSE   1
#define MNC_EXECUTE 2
#define MNC_SELECT  3

/**
 * Structure defines a menu item
 */
typedef struct _MENUITEMINFO {

    /**
     * Used by \a GetMenuItemInfo and \a SetMenuItemInfo functions, 
     * can be OR'ed with the following values:
     *      - MIIM_STATE\n
     *        Get/set the state of the menu item.
     *      - MIIM_ID\n
     *        Get/set the identifier of the menu item.
     *      - MIIM_SUBMENU\n
     *        Get/set the sub-menu of the menu item.
     *      - MIIM_CHECKMARKS\n
     *        Get/set the check/uncheck bitmap.
     *      - MIIM_TYPE\n
     *        Get/set the type and the type data of the menu item.
     *      - MIIM_DATA\n
     *        Get/set the item data of the menu item.
     */
    UINT                mask;

    /**
     * Type of menu item, can be one of the following values:
     *      - MFT_STRING\n
     *        A normal string menu item.
     *
     *      - MFT_BITMAP\n
     *        A bitmap menu item. When the menu item is this type, 
     *        the member \a typedata will be the pointer to the normal BITMAP object,
     *        the member \a uncheckedbmp will be the pointer to the hilighted BITMAP object,
     *        the member \a checkedbmp will be the pointer to the checked BITMAP object.
     *
     *      - MFT_BMPSTRING\n
     *        A bitmap menu item followed by a string. When the menu item is this type,
     *        the member \a typedata will be the pointer to the text string,
     *        the member \a uncheckedbmp will be the pointer to the normal BITMAP object,
     *        and the member \a checkedbmp will be the pointer to the checked BITMAP object.
     *
     *      - MFT_SEPARATOR
     *        A separator in menu.
     *
     *      - MFT_RADIOCHECK\n
     *        A normal string with a radio check mark.
     */
    UINT                type;

    /**
     * State of the menu item, can be one of the following values:
     *      - MFS_GRAYED\n
     *        The menu item is disabled.
     *      - MFS_DISABLED\n
     *        The menu item is disabled.
     *      - MFS_CHECKED\n
     *        The menu item is checked.
     *      - MFS_ENABLED\n
     *        The menu item is enabled.
     *      - MFS_UNCHECKED\n
     *        The menu item is unchecked.
     */
    UINT                state;

    /** The identifier of the menu item */
    int                 id;

    /** The handle to the sub-menu if this menu contains a sub menu */
    HMENU               hsubmenu;

    /** The pointer to a BITMAP object used for unchecked/highlited bitmap menu item */
    PBITMAP             uncheckedbmp;

    /** The pointer to a BITMAP object used for checked bitmap menu item */
    PBITMAP             checkedbmp;

    /** The private data attached to the menu item */
    DWORD               itemdata; 

    /** The data of this menu item, used to pass the string or the pointer to the BITMAP object of the menu item */
    DWORD               typedata;

    /** 
     * Used by \a GetMenuItemInfo function to indicate the maximal length 
     * of the string
     */
    UINT                cch;
} MENUITEMINFO;
typedef MENUITEMINFO* PMENUITEMINFO;

/**
 * \fn HMENU GUIAPI CreateMenu (void)
 * \brief Creates an empty menu.
 *
 * This function creates a menu. The menu is initially empty, but it can be 
 * filled with menu items by using the \a InsertMenuItem functions.
 *
 * \return The handle to the menu, zero when error.
 *
 * \sa InsertMenuItem
 */
MG_EXPORT HMENU GUIAPI CreateMenu (void);

/**
 * \fn HMENU GUIAPI CreatePopupMenu ( PMENUITEMINFO pmii)
 * \brief Creates a drop-down menu or submenu.
 *
 * This function creates a drop-down menu or submenu. The menu is initially 
 * empty. You can insert or append menu items by using the \a InsertMenuItem 
 * function.
 *
 * \param pmii Menu item information used to create the popup menu.
 *
 * \return The handle to the popup menu.
 *
 * \sa InsertMenuItem, MENUITEMINFO
 */
MG_EXPORT HMENU GUIAPI CreatePopupMenu ( PMENUITEMINFO pmii);

/**
 * \fn HMENU GUIAPI CreateSystemMenu (HWND hwnd, DWORD dwStyle)
 * \brief Creates a system menu.
 *
 * This function creates a system menu for the main window specified by 
 * \a hwnd.
 *
 * \param hwnd The handle to the main window.
 * \param dwStyle The style of the main window.
 *
 * \return The handle to the system menu.
 *
 * \sa CreateMenu, CreatePopupMenu
 */
MG_EXPORT HMENU GUIAPI CreateSystemMenu (HWND hwnd, DWORD dwStyle);

/**
 * \fn int GUIAPI InsertMenuItem (HMENU hmnu, int item, \
 *                UINT flag, PMENUITEMINFO pmii)
 * \brief Inserts a new menu item at the specified position in a menu.
 *
 * This function inserts a new menu item specified by \a pmmi at the
 * specified position (determined by \a item and \a flag) in the menu \a hmnu.
 *
 * \param hmnu The handle to the menu.
 * \param item The insertion position.
 * \param flag Indicates the position base, can be one of the following values:
 *      - MF_BYCOMMAND\n
 *        The position value \a item is based on the command identifier.
 *        The new menu item will insert bellow the item whose id is \a item.
 *      - MF_BYPOSITION\n
 *        The position value \a item is based on the position in the menu.
 * \param pmii Menu item information used to create the popup menu.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \retval ERR_INVALID_HANDLE \a hmnu is not a handle to menu.
 * \retval ERR_RES_ALLOCATION Can not allocate new menu item.
 * \retval ERR_INVALID_HMENU \a hmnu is an invalid menu.
 *
 * \sa RemoveMenu, MENUITEMINFO
 */
MG_EXPORT int GUIAPI InsertMenuItem (HMENU hmnu, int item, 
                            UINT flag, PMENUITEMINFO pmii);

/**
 * \fn int GUIAPI RemoveMenu (HMENU hmnu, int item, UINT flag)
 * \brief Deletes a menu item or detaches a submenu from the specified menu.
 *
 * This function deletes a menu item or detaches a submenu from the specified 
 * menu \a hmnu. If the item is a normal menu item, the function will delete 
 * the item. If the item is a submenu, the function will detache the submenu 
 * for the menu, but not delete the submenu.
 *
 * \param hmnu The handle to the menu.
 * \param item The position of the menu item or submenu.
 * \param flag Indicates the position base, can be one of the following values:
 *      - MF_BYCOMMAND\n
 *        The position value \a item is based on the command identifier.
 *      - MF_BYPOSITION\n
 *        The position value \a item is based on the position in the menu.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \retval ERR_INVALID_HANDLE \a hmnu is not a handle to menu.
 * \retval ERR_INVALID_HMENU \a hmnu is an invalid menu.
 *
 * \sa InsertMenuItem, DeleteMenu
 */
MG_EXPORT int GUIAPI RemoveMenu (HMENU hmnu, int item, UINT flag);

/**
 * \fn int GUIAPI DeleteMenu (HMENU hmnu, int item, UINT flag)
 * \brief Deletes an item from the specified menu.
 *
 * This function deletes an item from the specified menu \a hmnu.
 * Either the item is a normal menu item or a submenu, this function
 * will delete the item.
 *
 * \param hmnu The handle to the menu.
 * \param item The position of the menu item or submenu.
 * \param flag Indicates the position base, can be one of the following values:
 *      - MF_BYCOMMAND\n
 *        The position value \a item is based on the command identifier.
 *      - MF_BYPOSITION\n
 *        The position value \a item is based on the position in the menu.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \retval ERR_INVALID_HANDLE \a hmnu is not a handle to menu.
 * \retval ERR_INVALID_HMENU \a hmnu is an invalid menu.
 *
 * \sa InsertMenuItem, RemoveMenu
 */
MG_EXPORT int GUIAPI DeleteMenu (HMENU hmnu, int item, UINT flag);

/**
 * \fn int GUIAPI DestroyMenu (HMENU hmnu)
 * \brief Destroys the specified menu and frees any memory that the menu 
 * occupies.
 *
 * This function destroys the specified menu \a hmnu and frees any memory 
 * that the menu occupies.
 *
 * \param hmnu The handle to the menu.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \retval ERR_INVALID_HANDLE \a hmnu is not a handle to menu.
 *
 * \sa RemoveMenu, DeleteMenu
 */
MG_EXPORT int GUIAPI DestroyMenu (HMENU hmnu);

/**
 * \fn int GUIAPI IsMenu (HMENU hmnu)
 * \brief Determines whether a handle is a menu handle.
 *
 * This function determines whether the handle specified by \a hmnu is a 
 * menu handle.
 *
 * \param hmnu The handle to a menu.
 *
 * \return The function returns 0 for none menu handle, 
 *         otherwise the type of the menu.
 *
 * \sa CreateMenu
 */
MG_EXPORT int GUIAPI IsMenu (HMENU hmnu);

/**
 * \fn HMENU GUIAPI SetMenu (HWND hwnd, HMENU hmnu)
 * \brief Assigns a new menu to the specified main window.
 *
 * This functionn assigns the specified menu \a hmnu to 
 * the main window specified by \a hwnd.
 *
 * \param hwnd The handle to the main window.
 * \param hmnu The handle to the new menu.
 *
 * \return The handle to the old menu of the window.
 * 
 * \sa GetMenu
 */
MG_EXPORT HMENU GUIAPI SetMenu (HWND hwnd, HMENU hmnu);

/**
 * \fn HMENU GUIAPI GetMenu (HWND hwnd)
 * \brief Retrieves the handle to the menu assigned to the given main window.
 *
 * This function retrives the handle to the menu assigned to 
 * the given main window \a hwnd.
 *
 * \param hwnd The handle to the main window.
 * \return The handle to the menu of the window.
 *
 * \sa SetMenu
 */
MG_EXPORT HMENU GUIAPI GetMenu (HWND hwnd);

/**
 * \fn void GUIAPI DrawMenuBar (HWND hwnd)
 * \brief Redraws the menu bar of the specified main window.
 *
 * This function redraws the menu bar of the specified main window. 
 * If the menu bar changes after the system has created the window, 
 * this function must be called to draw the changed menu bar.
 *
 * \param hwnd The handle to the main window.
 *
 * \sa TrackMenuBar
 */
MG_EXPORT void GUIAPI DrawMenuBar (HWND hwnd);

/**
 * \fn int GUIAPI TrackMenuBar (HWND hwnd, int pos)
 * \brief Displays the specified submenu.
 *
 * This function displays the specified submenu in the menu bar of 
 * the specified main window \a hwnd.
 *
 * \param hwnd The handle to the main window.
 * \param pos The position of the submenu. The position value of the 
 *      first submenu is 0.
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa TrackPopupMenu, CreateMenu
 */
MG_EXPORT int GUIAPI TrackMenuBar (HWND hwnd, int pos);

/**
 * \fn int GUIAPI TrackPopupMenu (HMENU hmnu, UINT uFlags, \
 *               int x, int y, HWND hwnd)
 * \brief Displays and tracks a popup menu.
 *
 * This function displays a shortcut menu at the specified location 
 * and tracks the selection of items on the menu. The shortcut menu 
 * can appear anywhere on the screen.
 *
 * \param hmnu The handle to the popup menu.
 * \param uFlags The tracking flags, can be OR'ed value of the following values:
 *      - TPM_LEFTALIGN\n
 *        Horz alignement is left.
 *      - TPM_CENTERALIGN\n
 *        Horz alignement is center.
 *      - TPM_RIGHTALIGN\n
 *        Horz alignement is right.
 *      - TPM_TOPALIGN\n
 *        Vert alignement is top.
 *      - TPM_VCENTERALIGN\n
 *        Vert alignement is center.
 *      - TPM_BOTTOMALIGN\n
 *        Vert alignement is bottom.
 *      - TPM_DESTROY\n
 *        Destroys the popup menu after finishing tracking.
 *      - TPM_SYSCMD\n
 *        Sends an MSG_SYSCOMMAND message to the window when the use select a 
 *        menu item.
 * \param x The x coordinate of the position of the popup menu.
 * \param y The y coordinate of the position of the popup menu.
 * \param hwnd The handle to the window which will receive 
 *        the MSG_COMMAND or MSG_SYSCOMMAND message.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa TrackMenuBar, CreatePopupMenu
 */
MG_EXPORT int GUIAPI TrackPopupMenu (HMENU hmnu, UINT uFlags, 
                int x, int y, HWND hwnd);

/**
 * \fn HMENU GUIAPI GetMenuBarItemRect (HWND hwnd, int pos, RECT* prc)
 * \brief Retrieves the rect of a menu bar item.
 *
 * This function retrieves the rect of the menu bar item specified 
 * by the parameter \a pos.
 *
 * \param hwnd The handle to the main window.
 * \param pos The position of the item. The position value of the 
 *        first item is 0.
 * \param prc The pointer to a RECT object, which will receive the rectangle.
 *
 * \return The function returns the handle to the menu bar if success,
 *      otherwise returns 0.
 *
 * \sa TrackMenuBar
 */
MG_EXPORT HMENU GUIAPI GetMenuBarItemRect (HWND hwnd, int pos, RECT* prc);

#define HMF_UPITEM      0x01
#define HMF_DOWNITEM    0x02
#define HMF_DEFAULT     0x00

/**
 * \fn BOOL GUIAPI HiliteMenuBarItem (HWND hwnd, int pos, UINT flag)
 * \brief Draws the specified menubar item with 3D effect.
 *
 * This function draws the specified menubar item with 3D effect.
 *
 * \param hwnd The handle to the main window.
 * \param pos The position of the item. The position value of the 
 *        first item is 0.
 * \param flag The drawing flag, can be one of the following values:
 *      - HMF_UPITEM\n
 *        Up item.
 *      - HMF_DOWNITEM\n
 *        Down item.
 *      - HMF_DEFAULT\n
 *        Default.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa TrackMenuBar
 */
MG_EXPORT BOOL GUIAPI HiliteMenuBarItem (HWND hwnd, int pos, UINT flag);

/**
 * \fn int GUIAPI GetMenuItemCount (HMENU hmnu)
 * \brief Determines the number of items in a menu.
 *
 * This function determines the number of items in the specified menu \a hmnu.
 *
 * \param hmnu The handle to the menu.
 *
 * \return The number of the items in the menu.
 *
 * \sa GetMenuItemInfo
 */
MG_EXPORT int GUIAPI GetMenuItemCount (HMENU hmnu);

/**
 * \fn int GUIAPI GetMenuItemID (HMENU hmnu, int pos)
 * \brief Retrieves the menu item identifier of a menu item at specified 
 *        position in a menu.
 *
 * This function retrieves the menu item identifier of a menu item at 
 * the specified position \a pos in the specified menu \a hmnu.
 *
 * \param hmnu The handle to the menu.
 * \param pos The position of the menu item. The position value of the 
 *        first item is 0.
 *
 * \return The identifier of the items in the menu.
 *
 * \sa GetMenuItemInfo
 */
MG_EXPORT int GUIAPI GetMenuItemID (HMENU hmnu, int pos); 

/**
 * \fn int GUIAPI GetMenuItemInfo (HMENU hmnu, int item, \
 *               UINT flag, PMENUITEMINFO pmii)
 * \brief Retrieves information about a menu item.
 *
 * This function retrieves information about a menu item, and returns the
 * information via \a pmii.
 *
 * \param hmnu [in] Handle to the menu that contains the menu item.
 * \param item The position of the menu item or submenu.
 * \param flag Indicates the position base, can be one of the following values:
 *      - MF_BYCOMMAND\n
 *        The position value \a item is based on the command identifier.
 *      - MF_BYPOSITION\n
 *        The position value \a item is based on the position in the menu.
 * \param pmii [in, out] Pointer to a \a MENUITEMINFO structure that specifies 
 * the information to retrieve and receive information about the menu item.
 * 
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa SetMenuItemInfo, MENUITEMINFO
 */
MG_EXPORT int GUIAPI GetMenuItemInfo (HMENU hmnu, int item, 
                            UINT flag, PMENUITEMINFO pmii);

/*Reserved*/
int GUIAPI GetMenuItemRect (HWND hwnd, HMENU hmnu, 
                int item, PRECT prc);

/**
 * \fn HMENU GUIAPI GetPopupSubMenu (HMENU hpppmnu)
 * \brief Retrieves the submenu of the specified popup menu.
 *
 * This function retrieves the submenu of the specified popup menu.
 *
 * \param hpppmnu The handle to the popup menu.
 *
 * \return The handle to the submenu of the popup menu.
 *
 * \sa CreatePopupMenu
 */
MG_EXPORT HMENU GUIAPI GetPopupSubMenu (HMENU hpppmnu);

/**
 * \fn HMENU GUIAPI StripPopupHead (HMENU hpppmnu)
 * \brief Strips the title of the popup menu.
 *
 * This function strips the title of the popup menu \a hpppmnu.
 *
 * \param hpppmnu The handle to the popup menu.
 *
 * \return The handle to the submenu whose title has been stripped.
 *
 * \sa GetPopupSubMenu
 */
MG_EXPORT HMENU GUIAPI StripPopupHead (HMENU hpppmnu);

/**
 * \fn HMENU GUIAPI GetSubMenu (HMENU hmnu, int pos)
 * \brief Retrieves the handle to the submenu activated by the specified menu 
 *        item.
 *
 * This function retrieves the handle to the drop-down menu or submenu 
 * activated by the specified menu item.
 *
 * \param hmnu The handle to the menu.
 * \param pos The position of the menu item. The position value of the 
 *        first item is 0.
 *
 * \return The handle to the submenu; 0 if the menu item can not activate 
 *         a submenu.
 *
 * \sa GetPopupSubMenu
 */
MG_EXPORT HMENU GUIAPI GetSubMenu (HMENU hmnu, int pos);

/**
 * \fn HMENU GUIAPI GetSystemMenu (HWND hwnd, BOOL flag)
 * \brief Allows the application to access the window menu (also known as 
 *        the system menu) for copying and modifying.
 *
 * This function returns the handle to the system menu of the main window.
 * This allows the application to access the window menu (also known as
 * the system menu) for copying and modifying.
 *
 * \param hwnd The handle to the main window.
 * \param flag Ignored currently; reserved for future use.
 *
 * \return The handle to the system menu; 0 if the main window has not 
 *         a system menu.
 * 
 * \sa GetMenu, SetMenu
 */
MG_EXPORT HMENU GUIAPI GetSystemMenu (HWND hwnd, BOOL flag);

/**
 * \fn UINT GUIAPI EnableMenuItem (HMENU hmnu, int item, UINT flag)
 * \brief Enables, disables, or grays the specified menu item.
 *
 * This function enables, disables, or grays the specified menu item.
 *
 * \param hmnu [in] Handle to the menu.
 * \param item [in] Specifies the menu item or submenu to be enabled, disabled,
 * or grayed, as determined by the \a flag parameter. This parameter specifies
 * an item in a menu bar, menu or submenu.
 * \param flag [in] Controls the interpretation of the \a item parameter and 
 * indicates whether the menu item is enabled, disabled, or grayed. This 
 * parameter must be a combination of either MF_BYPOSITION or MF_BYCOMMAND.
 *
 * \return The return value specifies the previous state of the menu item.
 * If the menu item does not exist, the return value is -1.
 *
 * \sa GetMenuItemInfo
 */
MG_EXPORT UINT GUIAPI EnableMenuItem (HMENU hmnu, int item, UINT flag);

/**
 * \fn int GUIAPI CheckMenuRadioItem (HMENU hmnu, int first, int last, \
 *               int checkitem, UINT flag)
 * \brief Checks a specified menu item and makes it a radio item.
 *
 * This function checks a specified menu item and makes it a radio item. 
 * At the same time, the function unchecks all other menu items in the 
 * associated group and clears the radio-item type flag for those items.
 *
 * \param hmnu The handle to the menu.
 * \param first The position of the first item in the group.
 * \param last The position of the last item in the group.
 * \param checkitem The position of the menu item to check.
 * \param flag Indicates the position base, can be one of the following values:
 *      - MF_BYCOMMAND\n
 *        The position value \a item is based on the command identifier.
 *      - MF_BYPOSITION\n
 *        The position value \a item is based on the position in the menu.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa SetMenuItemInfo, MENUITEMINFO
 */
MG_EXPORT int GUIAPI CheckMenuRadioItem (HMENU hmnu, int first, int last, 
                            int checkitem, UINT flag);

/**
 * \fn int GUIAPI SetMenuItemBitmaps (HMENU hmnu, int item, \
 *                UINT flag, PBITMAP hBmpUnchecked, PBITMAP hBmpChecked)
 * \brief Associates the specified bitmap with a menu item.
 *
 * This function associates the specified bitmap with a menu item. 
 * Whether the menu item is checked or unchecked, the system displays the 
 * appropriate bitmap next to the menu item.
 *
 * \param hmnu The handle to the menu.
 * \param item The position of the menu item.
 * \param flag Indicates the position base, can be one of the following values:
 *      - MF_BYCOMMAND\n
 *        The position value \a item is based on the command identifier.
 *      - MF_BYPOSITION\n
 *        The position value \a item is based on the position in the menu.
 * \param hBmpUnchecked The pointer to the unchecked BITMAP object.
 * \param hBmpChecked The pointer to the checked BITMAP object.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa SetMenuItemInfo, MENUITEMINFO
 */
MG_EXPORT int GUIAPI SetMenuItemBitmaps (HMENU hmnu, int item, UINT flag, 
                            PBITMAP hBmpUnchecked, PBITMAP hBmpChecked);

/**
 * \fn int GUIAPI SetMenuItemInfo (HMENU hmnu, int item, \
 *               UINT flag, PMENUITEMINFO pmii)
 * \brief Changes information about a menu item.
 *
 * This function changes information about the specified menu item.
 *
 * \param hmnu [in] Handle to the menu that contains the menu item.
 * \param item [in] Specifies the identifier or position of the menu item 
 * to change. The meaning of this parameter depends on the value of \a flag.
 * \param flag Indicates the position base, can be one of the following values:
 *      - MF_BYCOMMAND\n
 *        The position value \a item is based on the command identifier.
 *      - MF_BYPOSITION\n
 *        The position value \a item is based on the position in the menu.
 * \param pmii The pointer to a MENUITEMINFO structure contains the information.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa GetMenuItemInfo, MENUITEMINFO
 */
MG_EXPORT int GUIAPI SetMenuItemInfo (HMENU hmnu, int item, 
                            UINT flag, PMENUITEMINFO pmii);

    /** @} end of menu_fns */

    /**
     * \defgroup dialog_fns Dialog operations
     * @{
     */

/* Dialog codes */
/**
 * \def DLGC_WANTARROWS
 * \brief Control wants arrow keys.
 */
#define DLGC_WANTARROWS     0x0001     
/**
 * \def DLGC_WANTTAB
 * \brief Control wants tab keys
 */
#define DLGC_WANTTAB        0x0002    
/**
 * \def DLGC_WANTALLKEYS
 * \brief Control wants all keys.
 */
#define DLGC_WANTALLKEYS    0x0004   
/**
 * \def DLGC_WANTCHARS
 * \brief Want MSG_CHAR messages.
 */
#define DLGC_WANTCHARS      0x0008  

/**
 * \def DLGC_WANTENTER
 * \brief Control wants enter keys.
 */
#define DLGC_WANTENTER      0x0010 
/**
 * \def DLGC_HASSETSEL
 * \brief Understands EM_SETSEL message.
 */
#define DLGC_HASSETSEL      0x0080    

/**
 * \def DLGC_DEFPUSHBUTTON
 * \brief Default pushbutton.
 */
#define DLGC_DEFPUSHBUTTON  0x0100   
/**
 * \def DLGC_PUSHBUTTON
 * \brief Non-default pushbutton.
 */
#define DLGC_PUSHBUTTON     0x0200  
/**
 * \def DLGC_RADIOBUTTON
 * \brief Radio button.
 */
#define DLGC_RADIOBUTTON    0x0400 
/**
 * \def DLGC_3STATE
 * \brief 3 States button item.
 */
#define DLGC_3STATE         0x0800
/**
 * \def DLGC_STATIC
 * \brief Static item: don't include.
 */
#define DLGC_STATIC         0x1000 
/**
 * \def DLGC_BUTTON
 * \brief Button item: can be checked.
 */
#define DLGC_BUTTON         0x2000    

/**
 * Structure which defines a control.
 */
typedef struct _CTRLDATA
{
    /** Class name of the control */
    const char* class_name;             
    /** Control style */
    DWORD       dwStyle;                
    /** Control position in dialog */
    int         x, y, w, h;             
    /** Control identifier */
    int         id;                     
    /** Control caption */
    const char* caption;                
    /** Additional data */
    DWORD       dwAddData;              
    /** Control extended style */
    DWORD       dwExStyle;              
} CTRLDATA;
typedef CTRLDATA* PCTRLDATA;

/**
 * Structure which defines a dialogbox.
 *
 * Example:
 * 
 * \include dlgtemplate.c
 */
typedef struct _DLGTEMPLATE
{
    /** Dialog box style */
    DWORD       dwStyle;                
    /** Dialog box extended style */
    DWORD       dwExStyle;              
    /** Dialog box position */
    int         x, y, w, h;             
    /** Dialog box caption */
    const char* caption;                
    /** Dialog box icon */
    HICON       hIcon;                  
    /** Dialog box menu */
    HMENU       hMenu;                  
    /** Number of controls */
    int         controlnr;              
    /** Poiter to control array */
    PCTRLDATA   controls;               
    /** Addtional data, must be zero */
    DWORD       dwAddData;              
} DLGTEMPLATE;
typedef DLGTEMPLATE* PDLGTEMPLATE;

/**
 * \fn HWND GUIAPI CreateMainWindowIndirectParam (PDLGTEMPLATE pDlgTemplate, \
 *               HWND hOwner, WNDPROC WndProc, LPARAM lParam) 
 * \brief Uses a dialog template to create a modeless main window and 
 *        controls in it, and pass a parameter to the window procedure.
 *
 * This function uses a dialog template pointed to by \a pDlgTemplate
 * to create a modeless main window and controls in it. The parameter 
 * specified by \a lParam will be passed to the window procedure as the 
 * second paramter of MSG_INITDIALOG message.
 *
 * \param pDlgTemplate The pointer to a DLGTEMPLATE structure.
 * \param hOwner The handle to the hosting main window.
 * \param WndProc The window procedure of the new main window.
 * \param lParam The parameter will be passed to the window procedure.
 *
 * \return Handle to the new main window, HWND_INVALID on error.
 *
 * \sa DestroyMainWindowIndirect, DialogBoxIndirectParam, DLGTEMPLATE
 */
MG_EXPORT HWND GUIAPI CreateMainWindowIndirectParam (PDLGTEMPLATE pDlgTemplate, 
                HWND hOwner, WNDPROC WndProc, LPARAM lParam);

/**
 * \def CreateMainWindowIndirect(pDlgTemplate, hOwner, WndProc)
 * \brief An simplified version of \a CreateMainWindowIndirectParam.
 *
 * This macro calls \a CreateMainWindowIndirectParam with \a lParam set to be 0.
 */
#define CreateMainWindowIndirect(pDlgTemplate, hOwner, WndProc) \
            CreateMainWindowIndirectParam(pDlgTemplate, hOwner, WndProc, 0)

/**
 * \fn BOOL GUIAPI DestroyMainWindowIndirect (HWND hMainWin)
 * \brief Destroys a main window created by \a CreateMainWindowIndirectParam.
 *
 * This function destroys the main window which was created by 
 * \a CreateMainWindowIndirectParam function.
 *
 * \param hMainWin The handle to the main window.
 *
 * \sa CreateMainWindowIndirectParam
 */
MG_EXPORT BOOL GUIAPI DestroyMainWindowIndirect (HWND hMainWin);

/**
 * \fn int GUIAPI DialogBoxIndirectParam (PDLGTEMPLATE pDlgTemplate, \
 *               HWND hOwner, WNDPROC DlgProc, LPARAM lParam)
 * \brief Creates a modal dialog box from a dialog box template in memory.
 *
 * This function creates a modal dialog box from a dialog box template 
 * in memory. Before displaying the dialog box, the function passes an 
 * application-defined value to the dialog box procedure as the second 
 * parameter of the MSG_INITDIALOG message. An application can use this 
 * value to initialize the controls in the dialog box.
 *
 * \param pDlgTemplate The pointer to a DLGTEMPLATE structure.
 * \param hOwner The handle to the hosting main window.
 * \param DlgProc The window procedure of the new dialog box.
 * \param lParam The parameter will be passed to the window procedure.
 *
 * \return Return value of the dialog box, i.e., the second argument
 *         passed to EndDialog function which closes the dialog box.
 *
 * \sa EndDialog, CreateMainWindowIndirectParam, DLGTEMPLATE
 *
 * Example:
 *
 * \include dialogbox.c
 */
MG_EXPORT int GUIAPI DialogBoxIndirectParam (PDLGTEMPLATE pDlgTemplate, 
                    HWND hOwner, WNDPROC DlgProc, LPARAM lParam);

/**
 * \fn BOOL GUIAPI EndDialog (HWND hDlg, int endCode)
 * \brief Destroys a modal dialog box, causing MiniGUI to end any processing 
 *        for the dialog box.
 *
 * This function destroys the modal dialog box \a hDlg created by 
 * \a DialogBoxIndirectParam and ends any processing for the dialog box. 
 * The argument \a endCode will be returned by \a DialogBoxIndirectParam 
 * as the return value.
 *
 * \param hDlg The handle to the dialog box.
 * \param endCode The value will be returned by \a DialogBoxIndirectParam.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa DialogBoxIndirectParam
 */
MG_EXPORT BOOL GUIAPI EndDialog (HWND hDlg, int endCode);

/**
 * \fn void GUIAPI DestroyAllControls (HWND hWnd)
 * \brief Destroys all controls in a window.
 *
 * This function destroys all controls (child windows) in a window.
 *
 * \param hWnd The handle to the window.
 *
 * \sa DestroyWindow
 */
MG_EXPORT void GUIAPI DestroyAllControls (HWND hWnd);

/**
 * \fn HWND GUIAPI GetDlgDefPushButton (HWND hWnd)
 * \brief Gets the default push button control in a window.
 *
 * This function gets the handle to the default push button 
 * (with BS_DEFPUSHBUTTON style) in the specified window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The handle to the default push button, 
 *         zero for non default push button in the window.
 */
MG_EXPORT HWND GUIAPI GetDlgDefPushButton (HWND hWnd);

/**
 * \fn int GUIAPI GetDlgCtrlID (HWND hwndCtl)
 * \brief Gets the integer identifier of a control.
 *
 * This function gets the integer identifier of the control \a hwndCtl.
 *
 * \param hwndCtl The handle to the control.
 *
 * \return The identifier of the control, -1 for error.
 *
 * \sa GetDlgItem
 */
MG_EXPORT int GUIAPI GetDlgCtrlID (HWND hwndCtl);

/**
 * \fn HWND GUIAPI GetDlgItem (HWND hDlg, int nIDDlgItem)
 * \brief Retrives the handle to a control in a dialog box.
 *
 * This function retrives the handle to a control, whose identifier is 
 * \a nIDDlgItem, in the specified dialog box \a hDlg.
 *
 * \param hDlg The handle to the dialog box.
 * \param nIDDlgItem The identifier of the control.
 *
 * \return The handle to the control, zero for not found.
 */
MG_EXPORT HWND GUIAPI GetDlgItem (HWND hDlg, int nIDDlgItem);

/**
 * \fn UINT GUIAPI GetDlgItemInt (HWND hDlg, int nIDDlgItem, \
 *               BOOL *lpTranslated, BOOL bSigned)
 * \brief Translates the text of a control in a dialog box into an integer 
 *        value.
 *
 * This function translates the text of the control, whose identifier is 
 * \a nIDDlgItem in the dialog box \a hDlg into an integer value.
 *
 * \param hDlg The handle to the dialog box.
 * \param nIDDlgItem The identifier of the control.
 * \param lpTranslated The pointer to a boolean value, which indicates whether
 *        translated successfully.
 * \param bSigned Indicates whether handle the text as a signed integer.
 *
 * \return The translated 32-bit integer.
 *
 * \note MiniGUI uses \a strtol or \a strtoul to convert the string value 
 * to a 32-bit integer, and pass the base as 0. Thus, the valid string value 
 * should be in the following forms:
 *
 *  - [+|-]0x[0-9|A-F]*\n
 *    Will be read in base 16.
 *  - [+|-]0[0-7]*\n
 *    Will be read in base 8.
 *  - [+|-][1-9][0-9]*\n
 *    Will be read in base 10.
 *
 * \sa GetDlgItemText, SetDlgItemInt
 */
MG_EXPORT UINT GUIAPI GetDlgItemInt (HWND hDlg, int nIDDlgItem, 
                BOOL *lpTranslated, BOOL bSigned);

/**
 * \fn int GUIAPI GetDlgItemText (HWND hDlg, int nIDDlgItem, \
 *               char* lpString, int nMaxCount)
 * \brief Retrieves the title or text associated with a control in a dialog box.
 *
 * This function retrives the title or text associated with a control, whose
 * identifier is \a nIDDlgItem in the dialog box \a hDlg.
 *
 * \param hDlg The handle to the dialog box.
 * \param nIDDlgItem The identifier of the control.
 * \param lpString The pointer to a buffer which receives the text.
 * \param nMaxCount The maximal length of the string, not including 
 *        the null character.
 *
 * \return The length of the null-terminated text.
 *
 * \note The buffer should at least have size of (\a nMaxCount + 1).
 *
 * \sa GetDlgItemInt, GetDlgItemText2
 */
MG_EXPORT int GUIAPI GetDlgItemText (HWND hDlg, int nIDDlgItem, 
                char* lpString, int nMaxCount);

/**
 * \fn char* GUIAPI GetDlgItemText2 (HWND hDlg, int id, int* lenPtr)
 * \brief Retrieves the title or text associated with a control in a dialog box.
 *
 * This function is similiar as \a GetDlgItemText function,
 * but it allocates memory for the text and returns the pointer 
 * to the allocated buffer. You should free the buffer when done by using
 * \a free function.
 *
 * \param hDlg The handle to the dialog box.
 * \param id The identifier of the control.
 * \param lenPtr The pointer to an integer which receives the length of the text
 *        if it is not NULL.
 *
 * \return The pointer to the allocated buffer.
 *
 * \sa GetDlgItemText
 */
MG_EXPORT char* GUIAPI GetDlgItemText2 (HWND hDlg, int id, int* lenPtr);

/**
 * \fn HWND GUIAPI GetNextDlgGroupItem (HWND hDlg, \
 *               HWND hCtl, BOOL bPrevious)
 * \brief Retrieves the handle to the first control in a group of controls 
 *        that precedes (or follows) the specified control in a dialog box.
 *
 * This function retrieves the handle to the first control in a group of 
 * controls that precedes (or follows) the specified control \a hCtl in 
 * the dialog box \a hDlg.
 *
 * \param hDlg The handle to the dialog box.
 * \param hCtl The handle to the control.
 * \param bPrevious A boolean value indicates to retrive the preceding or 
 *        following control. TRUE for preceding control.
 *
 * \return The handle to the preceding or following control.
 *
 * \sa GetNextDlgTabItem
 */
MG_EXPORT HWND GUIAPI GetNextDlgGroupItem (HWND hDlg, 
                HWND hCtl, BOOL bPrevious);

/**
 * \fn HWND GUIAPI GetNextDlgTabItem (HWND hDlg, HWND hCtl, BOOL bPrevious)
 * \brief Retrieves the handle to the first control that has the WS_TABSTOP 
 *        style that precedes (or follows) the specified control.
 *
 * This function retrieves the handle to the first control that has the 
 * WS_TABSTOP style that precedes (or follows) the specified control \a hCtl
 * in the dialog box \a hDlg.
 *
 * \param hDlg The handle to the dialog box.
 * \param hCtl The handle to the control.
 * \param bPrevious A boolean value indicates to retrive the preceding or 
 *        following control. TRUE for preceding control.
 *
 * \return The handle to the preceding or following control.
 *
 * \sa GetNextDlgGroupItem
 */
MG_EXPORT HWND GUIAPI GetNextDlgTabItem (HWND hDlg, HWND hCtl, BOOL bPrevious);

/**
 * \fn int GUIAPI SendDlgItemMessage (HWND hDlg, int nIDDlgItem, \
 *               int message, WPARAM wParam, LPARAM lParam)
 * \brief Sends a message to the specified control in a dialog box.
 *
 * This function sends a message specified by (\a message, \a wParam, \a lParam)
 * to the specified control whose identifier is \a nIDDlgItem in the dialog 
 * box \a hDlg.
 *
 * \param hDlg The handle to the dialog box.
 * \param nIDDlgItem The identifier of the control.
 * \param message The message identifier.
 * \param wParam The first message parameter.
 * \param lParam The second message parameter.
 *
 * \return The return value of the message handler.
 *
 * \sa SendMessage, GetDlgItem
 */
MG_EXPORT int GUIAPI SendDlgItemMessage ( HWND hDlg, int nIDDlgItem, 
                int message, WPARAM wParam, LPARAM lParam);

/**
 * \fn BOOL GUIAPI SetDlgItemInt (HWND hDlg, int nIDDlgItem, \
 *               UINT uValue, BOOL bSigned)
 * \brief Sets the text of a control in a dialog box to the string 
 *        representation of a specified integer value.
 *
 * This function sets the text of the control whose identifier is \a nIDDlgItem 
 * in the dialog box \a hDlg to the string representation of 
 * the specified integer value \a nValue.
 *
 * \param hDlg The handle to the dialog box.
 * \param nIDDlgItem The identifier of the control.
 * \param uValue The 32-bit integer value.
 * \param bSigned A boolean value indicates whether the integer value is a 
 *        signed integer.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa GetDlgItemInt, SetDlgItemText
 */
MG_EXPORT BOOL GUIAPI SetDlgItemInt (HWND hDlg, int nIDDlgItem, 
                UINT uValue, BOOL bSigned);

/**
 * \fn BOOL GUIAPI SetDlgItemText (HWND hDlg, int nIDDlgItem, \
 *               const char* lpString)
 * \brief Sets the title or text of a control in a dialog box.
 *
 * This function sets the title or text of the control whose identifier 
 * is \a nIDDlgItem in the dialog box \a hDlg to the string pointed to 
 * by \a lpString.
 *
 * \param hDlg The handle to the dialog box.
 * \param nIDDlgItem The identifier of the control.
 * \param lpString The pointer to the string.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa GetDlgItemText, SetDlgItemInt
 */
MG_EXPORT BOOL GUIAPI SetDlgItemText (HWND hDlg, int nIDDlgItem, 
                const char* lpString);

#ifdef _CTRL_BUTTON

/**
 * \fn void GUIAPI CheckDlgButton (HWND hDlg, int nIDDlgItem, int nCheck)
 * \brief Changes the check status of a button control.
 *
 * This function changes the check status of the button control whose 
 * identifier is \a nIDDlgItem in the dialog box \a hDlg.
 *
 * \param hDlg The handle to the dialog box.
 * \param nIDDlgItem The identifier of the control.
 * \param nCheck The state of the button. If the button is a normal button,
 *        the value being zero means the button is checked, otherwise unchecked.
 *        If the button is a special button with three states, the value 
 *        can be one of the following values:
 *          - BST_UNCHECKED\n
 *            The button is unchecked.
 *          - BST_CHECKED\n
 *            The button is checked.
 *          - BST_INDETERMINATE\n
 *            The button is in indeterminate state.
 *
 * \sa CheckRadioButton, IsDlgButtonChecked
 */
MG_EXPORT void GUIAPI CheckDlgButton (HWND hDlg, int nIDDlgItem, int nCheck);

/**
 * \fn void GUIAPI CheckRadioButton (HWND hDlg, \
 *               int idFirstButton, int idLastButton, int idCheckButton)
 * \brief Adds a check mark to (checks) a specified radio button in a group 
 *        and removes a check mark from (clears) all other radio buttons in 
 *        the group.
 *
 * This function adds a check mark to (checks) the specified radio button 
 * \a idCheckButton in a group between \a idFirstButton and \a idLastButton, 
 * and removes a check mark from (clears) all other radio buttons in the group.
 *
 * \param hDlg The handle to the dialog box.
 * \param idFirstButton The identifier of the first control in the group.
 * \param idLastButton The identifier of the last control in the group.
 * \param idCheckButton The identifier of the control to be checked.
 *
 * \sa CheckDlgButton
 */
MG_EXPORT void GUIAPI CheckRadioButton (HWND hDlg, 
                int idFirstButton, int idLastButton, int idCheckButton);

/**
 * \fn int GUIAPI IsDlgButtonChecked (HWND hDlg, int idButton)
 * \brief Determines whether a button control has a check mark next to it or 
 *        whether a three-state button control is grayed, checked, or neither.
 *
 * This function determines whether the button control whose identifier is 
 * \a idButton has a check mark next to it or whether a three-state button 
 * control is grayed, checked, or neither.
 *
 * \param hDlg The handle to the dialog box.
 * \param idButton The identifier of the button.
 *
 * \return The check state of the button. If the button is a normal button,
 *        the value being zero means the button is checked, otherwise unchecked.
 *        If the button is a special button with three states, the value 
 *        can be one of the following values:
 *          - BST_UNCHECKED\n
 *            The button is unchecked.
 *          - BST_CHECKED\n
 *            The button is checked.
 *          - BST_INDETERMINATE\n
 *            The button is in indeterminate state.
 *
 * \sa CheckDlgButton
 */
MG_EXPORT int  GUIAPI IsDlgButtonChecked (HWND hDlg, int idButton); 
#endif

     /** @} end of dialog_fns */

     /**
      * \defgroup msgbox_fns Message box operations
      * @{
      */

#if defined(_CTRL_STATIC) && defined (_CTRL_BUTTON)

/* Standard control IDs */
#define IDC_STATIC    0
#define IDOK          1
#define IDCANCEL      2
#define IDABORT       3
#define IDRETRY       4
#define IDIGNORE      5
#define IDYES         6
#define IDNO          7

#define MINID_RESERVED      0xF001
#define MAXID_RESERVED      0xFFFF

#define MB_OK                   0x00000000
#define MB_OKCANCEL             0x00000001
#define MB_YESNO                0x00000002
#define MB_RETRYCANCEL          0x00000003
#define MB_ABORTRETRYIGNORE     0x00000004
#define MB_YESNOCANCEL          0x00000005
#define MB_CANCELASBACK         0x00000008  /* customized style */
#define MB_TYPEMASK             0x00000007

#define MB_ICONHAND             0x00000010
#define MB_ICONQUESTION         0x00000020
#define MB_ICONEXCLAMATION      0x00000030
#define MB_ICONASTERISK         0x00000040
#define MB_ICONMASK             0x000000F0

#define MB_ICONINFORMATION      MB_ICONASTERISK
#define MB_ICONSTOP             MB_ICONHAND

#define MB_DEFBUTTON1           0x00000000
#define MB_DEFBUTTON2           0x00000100
#define MB_DEFBUTTON3           0x00000200
#define MB_DEFMASK              0x00000F00

/* #define MB_APPLMODAL            0x00000000 */
/* #define MB_SYSTEMMODAL          0x00001000 */
/* #define MB_TASKMODAL            0x00002000 */

#define MB_NOFOCUS              0x00008000

#define MB_ALIGNCENTER          0x00000000
#define MB_ALIGNTOPLEFT         0x00010000
#define MB_ALIGNBTMLEFT         0x00020000
#define MB_ALIGNTOPRIGHT        0x00030000
#define MB_ALIGNBTMRIGHT        0x00040000
#define MB_ALIGNMASK            0x00070000

#define MB_BASEDONPARENT        0x00080000  /* default is desktop. */

/**
 * \fn int GUIAPI MessageBox (HWND hParentWnd, const char* pszText, \
 *               const char* pszCaption, DWORD dwStyle)
 * \brief Displays a message box within one, two, or three push buttons.
 *
 * This function displays a message box within one, two, or three buttons, 
 * and returns the identifier of the button clicked by the user. You can 
 * pass \a dwStyle argument with different values to control the number of 
 * buttons, the button titles, and so on.
 *
 * \param hParentWnd The handle to the hosting main window.
 * \param pszText The message text will be displayed in the message box.
 * \param pszCaption The caption of the message box.
 * \param dwStyle Specifies the contents and behavior of the dialog box. 
 * This parameter can be a combination of flags from the following groups 
 * of flags.
 *
 *          To indicate the buttons displayed in the message box, specify 
 *          one of the following values.
 *          - MB_OK\n
 *            Create a message box within only one button whose title is "OK".
 *          - MB_OKCANCEL\n
 *            Create a message box within two buttons whose title are "OK" 
 *            and "Cancel" respectively.
 *          - MB_YESNO\n
 *            Create a message box within two buttons whose title are "Yes" 
 *            and "No" respectively.
 *          - MB_RETRYCANCEL\n
 *            Create a message box within two buttons whose title are "Retry" 
 *            and "Cancel" respectively.
 *          - MB_ABORTRETRYIGNORE\n
 *            Create a message box within three buttons whose title are 
 *            "Abort", "Retry" and "Ignore" respectively.
 *          - MB_YESNOCANCEL\n
 *            Create a message box within three buttons whose title are 
 *            "Yes", "No" and "Cancel" respectively.
 *          - MB_CANCELASBACK\n
 *            Title "Cancel" replaced with title "Previous".
 *
 *          To display an icon in the message box, specify one of the following
 *          values.
 *          - MB_ICONHAND\n
 *            Display a hand/stop icon in the message box.
 *          - MB_ICONQUESTION\n
 *            Display a question mark icon in the message box.
 *          - MB_ICONEXCLAMATION\n
 *            Display a exclamation mark icon in the message box.
 *          - MB_ICONASTERISK\n
 *            Display an information icon in the message box.
 *          - MB_ICONINFORMATION\n
 *            Display an information icon in the message box.
 *          - MB_ICONSTOP\n
 *            Display a hand/stop icon in the message box.
 *
 *          To indicate the default button, specify one of the following values.
 *          - MB_DEFBUTTON1\n
 *            The first button is the default button. 
 *          - MB_DEFBUTTON2\n
 *            The second button is the default button. 
 *          - MB_DEFBUTTON3\n
 *            The third button is the default button. 
 *
 *          To indicate the default text alignment, specify one of the following
 *          values.
 *          - MB_ALIGNCENTER\n
 *            The message box is center aligned.
 *          - MB_ALIGNTOPLEFT\n
 *            The message box is upper-left aligned.
 *          - MB_ALIGNBTMLEFT\n
 *            The message box is lower-left aligned.
 *          - MB_ALIGNTOPRIGHT\n
 *            The message box is upper-right aligned.
 *          - MB_ALIGNBTMRIGHT\n
 *            The message box is lower-right aligned.
 *
 *          To indicate the default align based on hosting window, specify the following value.
 *          - MB_BASEDONPARENT\n
 *            A flag indicates that the alignments above are based on the 
 *            hosting window, not the desktop. 
 *
 * \return The identifier of the button clicked by user, can be one of the 
 *         following values:
 *          - IDOK\n
 *            The "OK" button clicked.
 *          - IDCANCEL\n
 *            The "Cancel" or "Previous" button clicked.
 *          - IDABORT\n
 *            The "Abort" button clicked.
 *          - IDRETRY\n
 *            The "Retry" button clicked.
 *          - IDIGNORE\n
 *            The "Ignore" button clicked.
 *          - IDYES\n
 *            The "Yes" button clicked.
 *          - IDNO\n
 *            The "No" button clicked.
 *
 * \note This function will call \a GetSysText to translate the button 
 *       titles to localized text.
 *
 * \sa GetSysText, DialogBoxIndirectParam
 */
MG_EXPORT int GUIAPI MessageBox (HWND hParentWnd, const char* pszText, 
                      const char* pszCaption, DWORD dwStyle);

#endif /* _CTRL_STATIC && _CTRL_BUTTON */

/**
 * \fn void GUIAPI MessageBeep (DWORD dwBeep)
 * \brief Makes a sound of beep.
 *
 * This function makes a sound of beep. We ignore \a dwBeep argument so far.
 *
 * \param dwBeep The beep type, ignored so far.
 *
 * \sa Beep, Ping, Tone
 */
MG_EXPORT void GUIAPI MessageBeep (DWORD dwBeep);

    /** @} end of dialog_fns */

    /** @} end of window_fns */

    /** @} end of fns */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_WINDOW_H */

