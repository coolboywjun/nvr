/**
 * \file mgext.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2002/01/06
 * 
 * The header file of MiniGUI extension library.
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
 * $Id: mgext.h 7369 2007-08-16 05:28:10Z xgwang $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2007 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef MGEXT_H
#define MGEXT_H

#ifdef  __cplusplus
extern "C" {
#endif

    /**
     * \addtogroup mgext_fns Interfaces of the MiniGUI extension 
     *             library (libmgext)
     * @{
     */

/**
 * \fn BOOL InitMiniGUIExt (void)
 * \brief Initializes MiniGUI extension library.
 *
 * This function initializes MiniGUI extension library. You should call
 * this function before using any API provided by MiniGUIExt library.
 *
 * \return This function returns TRUE when success, otherwise FALSE.
 *
 * \sa MiniGUIExtCleanUp
 */
MGEXT_EXPORT BOOL InitMiniGUIExt (void);

/**
 * \fn MiniGUIExtCleanUp (void)
 * \brief Cleans up the MiniGUI extension library.
 *
 * This function cleans up the MiniGUI extension library. You should call
 * this function when you no longer use APIs provided by MiniGUIExt library.
 *
 * \sa InitMiniGUIExt
 */
MGEXT_EXPORT void MiniGUIExtCleanUp (void);

    /**
     * \addtogroup mgext_controls Controls defined in libmgext
     * @{
     */

    /** @} end of mgext_controls */

    /** @} end of mgext_fns */

#ifdef _EXT_CTRL_SPINBOX
#include "ext/spinbox.h"
#endif /* _EXT_CTRL_SPINBOX */

#ifdef _EXT_CTRL_MONTHCAL
#include "ext/monthcal.h"
#endif /* _EXT_CTRL_MONTHCAL */

#ifdef _EXT_CTRL_COOLBAR
#include "ext/coolbar.h"
#endif /* _CTRL_COOLBAR */

#ifdef _EXT_CTRL_LISTVIEW
#include "ext/listview.h"
#endif  /* _EXT_CTRL_LISTVIEW */

#ifdef _EXT_CTRL_TREEVIEW
#include "ext/treeview.h"
#endif /* _EXT_CTRL_TREEVIEW */

#ifdef _EXT_CTRL_GRIDVIEW
#include "ext/gridview.h"
#endif /* _EXT_CTRL_GRIDVIEW */

#ifdef _EXT_CTRL_ICONVIEW
#include "ext/iconview.h"
#endif  /* _EXT_CTRL_ICONVIEW */

#ifdef _EXT_CTRL_ANIMATION
#include "ext/animation.h"
#endif  /* _EXT_CTRL_ANIMATION */

#ifdef  __cplusplus
}
#endif

#endif /* MGEXT_H */

