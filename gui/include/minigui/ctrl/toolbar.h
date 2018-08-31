/**
 * \file toolbar.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2001/12/29
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
 * $Id: toolbar.h 7365 2007-08-16 05:22:17Z xgwang $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2007 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_TOOLBAR_H
#define _MGUI_CTRL_TOOLBAR_H
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_toolbar ToolBar control
     *
     * \note The argument \a dwAddData of \a CreateWindowEx function should specify
     *       the height and the width of item on toolbar. 
     * \note This control is reserved only for compatibility, new applications 
     *       should use the new tool bar control instead (NewToolBar).
     *
     * \code
     * int item_height = 16;
     * int item_width = 16;
     *
     * CreateWindowEx (CTRL_TOOLBAR, ..., MAKELONG (item_height, item_width));
     * \endcode
     *
     * \sa ctrl_newtoolbar
     *
     * @{
     */

/**
 * \def CTRL_TOOLBAR
 * \brief The class name of toolbar control.
 */
#define CTRL_TOOLBAR            ("toolbar")

/** Structure of the toolbar item info */
typedef struct _TOOLBARITEMINFO
{
    /** Reserved, do not use. */
    int    insPos;

    /**
     * Identifier of the item. When the user clicked the item, this control
     * will send a notification message to the parent window
     * with the notification code to be equal to this identifier.
     */
    int    id;

    /** Path of the bitmap for normal state of the item */
    char   NBmpPath[MAX_PATH+10];
    /** Path of the bitmap for hilighted state of the item */
    char   HBmpPath[MAX_PATH+10];
    /** Path of the bitmap for down state of the item */
    char   DBmpPath[MAX_PATH+10];

    /** Additional data of the item */
    DWORD  dwAddData;
} TOOLBARITEMINFO;
/**
 * \var typedef TOOLBARITEMINFO* PTOOLBARITEMINFO;
 * \brief Data type of the pointer to a TOOLBARITEMINFO.
 */
typedef TOOLBARITEMINFO* PTOOLBARITEMINFO;

    /**
     * \defgroup ctrl_toolbar_msgs Messages of toolbar control
     *
     * @{
     */

/**
 * \def TBM_ADDITEM
 * \brief Adds an item to a toolbar.
 *
 * \code
 * TBM_ADDITEM
 * TOOLBARITEMINFO *ptbii;
 *
 * wParam = 0;
 * lParam = (LPARAM)ptbii;
 * \endcode
 *
 * \param ptbii Pointer to the data storing the toolbar item info.
 */
#define TBM_ADDITEM             0xFE00

#define TBM_MSGMAX              0xFE10

    /** @} end of ctrl_toolbar_msgs */

    /** @} end of ctrl_toolbar */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_TOOLBAR_H */

