/**
 * \file mywindows.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2002/01/06
 * 
 *  This file includes interfaces for MyWins module of MiniGUIExt library, 
 *  which provides some useful and convenient user interface functions.
 *
 \verbatim

    Copyright (C) 2002-2007 Feynman Software
    Copyright (C) 1998-2002 Wei Yongming

    All rights reserved by Feynman Software.

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

 \endverbatim
 */

/*
 * $Id: mywindows.h 7486 2007-08-29 06:35:25Z xgwang $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2007 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef _MGUI_MYWINDOWS_H
    #define _MGUI_MYWINDOWS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup mgext_fns Interfaces of the MiniGUI extension 
     *             library (libmgext)
     * @{
     */

    /**
     * \addtogroup mywins_fns Interfaces of MyWins module of MiniGUIExt library
     * @{
     */

    /**
     * \defgroup mywins_helpers Useful helpers
     *
     * This module defines some useful helpers, such as tool-tip window,
     * progress window, and so on.
     *
     * @{
     */

/**
 * \fn int myMessageBox (HWND hwnd, DWORD dwStyle, const char* title,\
                const char* text, ...)
 * \brief Creates a message box.
 *
 * This function creates a message box calling \a MessageBox and passing
 * \a hwnd, \a dwStyle and \a title to it. This function also receives \a 
 * printf-like arguments to format a string.
 *
 * \param hwnd The hosting main window.
 * \param dwStyle The style of the message box.
 * \param title The title of the message box.
 * \param text The format string.
 *
 * \return Identifier of the button which closes the message box.
 *
 * \sa MessageBox, printf(3)
 */
MGEXT_EXPORT int myMessageBox (HWND hwnd, DWORD dwStyle, const char* title,
                const char* text, ...);

/**
 * \fn int myWinMessage (HWND hwnd, const char* title, \
                const char* button1, const char* text, ...)
 * \brief Creates a message box within only one button.
 *
 * This function creates a message box hosted to the main window \a hwnd,
 * displays a message and an application icon in the message box, and creates
 * a button which can be used to close the box. This function also 
 * receives \a printf-like arguments to format a string.
 *
 * This function can be used to display a information for the user.
 *
 * \param hwnd The hosting main window.
 * \param title The title of the message box.
 * \param button1 The text in the button.
 * \param text The format string.
 *
 * \return 0 indicates the message box was closed by clicking the only button.
 *
 * \sa myWinChoice, printf(3)
 */
MGEXT_EXPORT int myWinMessage (HWND hwnd, const char* title, 
                const char* button1, const char* text, ...);

/**
 * \fn int myWinChoice (HWND hwnd, const char* title, \
                const char* button1, const char* button2, \
                const char* text, ...)
 * \brief Creates a message box within two buttons.
 *
 * This function creates a message box hosted to the main window \a hwnd, 
 * displays a message and an application icon in the message box, and creates
 * two buttons in it. This function also receives \a printf-like arguments 
 * to format a string.
 *
 * This function can be used to prompt the user to choose one item 
 * between two.
 *
 * \param hwnd The hosting main window.
 * \param title The title of the message box.
 * \param button1 The title of the first button.
 * \param button2 The title of the second button.
 * \param text The format string.
 *
 * \return Either 0 or 1, indicates the message box was closed by 
 *         the first or second button.
 *
 * \sa myWinTernary, printf(3)
 */
MGEXT_EXPORT int myWinChoice (HWND hwnd, const char* title, 
                const char* button1, const char* button2, 
                const char* text, ...);

/**
 * \fn int myWinTernary (HWND hwnd, const char* title, \
                const char* button1, const char* button2, const char* button3, \
                const char* text, ...)
 * \brief Creates a message box within three buttons.
 *
 * This function creates a message box hosted to the main window \a hwnd, 
 * displays a message and an application icon in the message box, and creates
 * three buttons in it. This function also receives \a printf-like arguments 
 * to format a string.
 *
 * This function can be used to prompt the user to choose one item among 
 * three choices.
 *
 * \param hwnd The hosting main window.
 * \param title The title of the message box.
 * \param button1 The title of the first button.
 * \param button2 The title of the second button.
 * \param button3 The title of the third button.
 * \param text The format string.
 *
 * \return 0, 1, or 2, indicates the message box was closed by 
 *         the first, the second, or the third button.
 *
 * \sa myWinChoice, printf(3)
 */
MGEXT_EXPORT int myWinTernary (HWND hwnd, const char* title, 
                const char* button1, const char* button2, const char* button3,
                const char* text, ...);

/**
 * \fn void errorWindow (HWND hwnd, const char* str, const char* title)
 * \brief A MiniGUI edition of \a perror.
 *
 * This function creates a message box by using \a myMessageBox, and display the
 * current system error message. You can consider it as an alternative of 
 * \a perror.
 *
 * \param hwnd The hosting main window.
 * \param str The string will be appeared before the system error message.
 * \param title The title of the message box.
 *
 * \sa myMessageBox, perror(3)
 */
MGEXT_EXPORT void errorWindow (HWND hwnd, const char* str, const char* title);

/**
 * \fn HWND createStatusWin (HWND hParentWnd, int width, int height,\
                const char* title, const char* text, ...)
 * \brief Creates a status main window.
 *
 * This function creates a status main window and returns the handle to it.
 * You can call \a destroyStatusWin to destroy it. This function also 
 * receives \a printf-like arguments to format a string.
 *
 * \param hParentWnd The hosting main window.
 * \param width The width of the status window. 
 * \param height The height of the status window. 
 * \param title The title of the status window.
 * \param text The format string.
 *
 * \return The handle to the status window on success, HWND_INVALID on error.
 *
 * \sa destroyStatusWin
 */
MGEXT_EXPORT HWND createStatusWin (HWND hParentWnd, int width, int height, 
                const char* title, const char* text, ...);

/**
 * \fn void destroyStatusWin (HWND hwnd)
 * \brief Destroies a status window.
 *
 * This function destroies the specified status window \a hwnd, which 
 * is returned by \a createStatusWin.
 *
 * \param hwnd The handle to the status window.
 * 
 * \sa createStatusWin
 */
MGEXT_EXPORT void destroyStatusWin (HWND hwnd);

/**
 * \fn HWND createToolTipWin (HWND hParentWnd, int x, int y,\
                int timeout_ms, const char* text, ...)
 * \brief Creates a tool tip window.
 *
 * This function creates a tool tip window and returns the handle to it.
 * You can call \a destroyToolTipWin to destroy it. This function also 
 * receives \a printf-like arguments to format a string. 
 *
 * Note that the tool tip window will disappear automatically after the 
 * specified milliseconds by \a timeout_ms if \a timeout_ms is larger 
 * than 9 ms.
 *
 * \param hParentWnd The hosting main window.
 * \param x The position of the tool tip window.
 * \param y The position of the tool tip window.
 * \param timeout_ms The timeout value of the tool tip window. 
 * \param text The format string.
 *
 * \return The handle to the tool tip window on success, HWND_INVALID on error.
 *
 * \sa resetToolTipWin, destroyToolTipWin
 */
MGEXT_EXPORT HWND createToolTipWin (HWND hParentWnd, int x, int y, 
                int timeout_ms, const char* text, ...);

/**
 * \fn void resetToolTipWin (HWND hwnd, int x, int y,\
                const char* text, ...)
 * \brief Resets a tool tip window.
 *
 * This function resets the tool tip window specified by \a hwnd, including its
 * position, text displayed in it, and the visible status. If the tool tip is
 * invisible, it will become visible.
 *
 * This function also receives \a printf-like arguments to format a string. 
 *
 * \param hwnd The tool tip window handle returned by \a createToolTipWin.
 * \param x The new position of the tool tip window.
 * \param y The new position of the tool tip window.
 * \param text The new format string.
 *
 * \sa createToolTipWin, destroyToolTipWin
 */
MGEXT_EXPORT void resetToolTipWin (HWND hwnd, int x, int y, 
                const char* text, ...);

/**
 * \fn void destroyToolTipWin (HWND hwnd)
 * \brief Destroies a tool tip window.
 *
 * This function destroies the specified tool tip window \a hwnd, which 
 * is returned by \a createToolTipWin.
 *
 * \param hwnd The handle to the tool tip window.
 * 
 * \sa createToolTipWin
 */
MGEXT_EXPORT void destroyToolTipWin (HWND hwnd);

#ifdef _CTRL_PROGRESSBAR

/**
 * \fn HWND createProgressWin (HWND hParentWnd, const char *title,\
                const char *label, int id, int range)
 * \brief Creates a main window within a progress bar.
 *
 * This function creates a main window within a progress bar and 
 * returns the handle. You can call \a destroyProgressWin to destroy it. 
 *
 * Note that you can use \a SendDlgItemMessage to send a message to the 
 * progress bar in the main window in order to update the progress bar.
 *
 * \param hParentWnd The hosting main window.
 * \param title The title of the progress window.
 * \param label The text in the label of the progress bar.
 * \param id The identifier of the progress bar.
 * \param range The maximal value of the progress bar (minimal value is 0).
 *
 * \return The handle to the progress window on success, HWND_INVALID on error.
 *
 * \sa destroyProgressWin
 */
MGEXT_EXPORT HWND createProgressWin (HWND hParentWnd, const char* title, 
                const char* label, int id, int range);

/**
 * \fn void destroyProgressWin (HWND hwnd)
 * \brief Destroies progress window.
 *
 * This function destroies the specified progress window \a hwnd, which 
 * is returned by \a createProgressWin.
 *
 * \param hwnd The handle to the progress window.
 * 
 * \sa createProgressWin
 */
MGEXT_EXPORT void destroyProgressWin (HWND hwnd);

#endif /* _CTRL_PROGRESSBAR */

/**
 * Button info structure used by \a myWinMenu and \a myWinEntries function.
 * \sa myWinMenu, myWinEntries
 */
typedef struct _myWinButton
{
    /** Text of the button. */
    char*   text;
    /** Identifier of the button. */
    int     id;
    /** Styles of the button. */
    DWORD   flags;
} myWINBUTTON;

/* This is an internal structure. */
typedef struct _myWinMenuItems
{
    /* The pointer to the array of the item strings. */
    char**      items;
    /* The identifier of the listbox display the menu items. */
    int         listboxid;
    /* The pointer to the array of the selection status of the items. */
    int*        selected;
    /* The minimal button identifier. */
    int         minbuttonid;
    /* The maximal button identifier. */
    int         maxbuttonid;
} myWINMENUITEMS;

/**
 * \fn int myWinMenu (HWND hParentWnd, const char* title,\
                const char *label, int width, int listboxheight,\
                char **items, int *listItem, myWINBUTTON* buttons)
 * \brief Creates a menu main window for the user to select an item.
 *
 * This function creates a menu main window including a few buttons, 
 * and a list box with checkable item.
 * 
 * When the user click one of the buttons, this function will return the 
 * identifier of the button which leads to close the menu window, and 
 * the selections of the items via \a listItem.
 *
 * \param hParentWnd The hosting main window.
 * \param title The title of the menu main window.
 * \param label The label of the list box.
 * \param width The width of the menu main window.
 * \param listboxheight The height of the list box.
 * \param items The pointer to the array of the item strings.
 * \param listItem The pointer to the array of the check status of the items, 
 *        initial and returned.
 * \param buttons The buttons will be created.
 *
 * \return Returns the identifier of the button leading to close 
 *         the menu window on success, else on errors.
 *
 * \sa myWINBUTTON
 */
MGEXT_EXPORT int myWinMenu (HWND hParentWnd, const char* title, 
                const char* label, int width, int listboxheight, 
                char ** items, int * listItem, myWINBUTTON* buttons);

/**
 * Entry info structure used by \a myWinEntries function.
 * \sa myWinEntries
 */
typedef struct _myWinEntry
{
    /** The label of the entry. */
    char*   text;
    /** The pointer to the string of the entry. */
    char**  value;
    /** The maximal length of the entry in bytes. */
    int     maxlen;
    /** The styles of the entry. */
    DWORD   flags;
} myWINENTRY;

/* This is an internal structure. */
typedef struct _myWinEntryItems
{
    myWINENTRY* entries;
    int         entrycount;
    int         firstentryid;
    int         minbuttonid;
    int         maxbuttonid;
} myWINENTRYITEMS;

/**
 * \fn int myWinEntries (HWND hParentWnd, const char* title,\
                const char* label, int width, int editboxwidth,\
                BOOL fIME, myWINENTRY* items, myWINBUTTON* buttons)
 * \brief Creates a entry main window for the user to enter something.
 *
 * This function creates a entry main window including a few buttons
 * and a few entries.
 * 
 * When the user click one of the buttons, this function will return the 
 * identifier of the button which leads to close the menu window, and 
 * the entered strings.
 *
 * \param hParentWnd The hosting main window.
 * \param title The title of the menu main window.
 * \param label The label of the entries.
 * \param width The width of the menu main window.
 * \param editboxwidth The width of the edit boxes.
 * \param fIME Whether active the IME window (obsolete).
 * \param items The pointer to the array of the entries, initial and returned.
 * \param buttons The buttons will be created.
 *
 * \return Returns the identifier of the button leading to close the 
 *         menu window on success, else on errors.
 *
 * \sa myWINBUTTON, myWINENTRY
 */
MGEXT_EXPORT int myWinEntries (HWND hParentWnd, const char* title, 
                const char* label, int width, int editboxwidth, 
                BOOL fIME, myWINENTRY* items, myWINBUTTON* buttons);

/**
 * \fn int myWinHelpMessage (HWND hwnd, int width, int height,\
                const char* help_title, const char* help_msg)
 * \brief Creates a help message window.
 *
 * This function creates a help message window including a scrollable help 
 * message and a spin box. When the user click the OK button, this function 
 * will return.
 *
 * \param hwnd The hosting main window.
 * \param width The width of the help message window.
 * \param height The height of the help message window.
 * \param help_title The title of the window.
 * \param help_msg The help message.
 *
 * \return 0 on success, -1 on error.
 */
MGEXT_EXPORT int myWinHelpMessage (HWND hwnd, int width, int height,
                const char* help_title, const char* help_msg);

    /** @} end of mywins_helpers */

    /** @} end of mywins_fns */

    /** @} end of mgext_fns */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_MYWINDOWS_H */

