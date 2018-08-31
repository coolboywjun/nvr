/**
 * \file control.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2001/12/29
 * 
 * This file includes interfaces of standard controls of MiniGUI.
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
 * $Id: control.h 7364 2007-08-16 05:20:20Z xgwang $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2007 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CONTROL_H
#define _MGUI_CONTROL_H
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "ctrl/ctrlhelper.h"

    /**
     * \defgroup controls Standard controls
     * @{
     */

/* NOTE: control messages start from 0xF000 to 0xFFFF */
#define MSG_FIRSTCTRLMSG    0xF000
#define MSG_LASTCTRLMSG     0xFFFF

    /** @} end of controls */

#ifdef _CTRL_STATIC
#include "ctrl/static.h"
#endif /* _CTRL_STATIC */

#ifdef _CTRL_BUTTON
#include "ctrl/button.h"
#endif /* _CTRL_BUTTON */

#if defined (_CTRL_SIMEDIT) || defined(_CTRL_SLEDIT) || defined(_CTRL_MLEDIT)
#include "ctrl/edit.h"
#endif /* _CTRL_EDIT || _CTRL_MEDIT */

#ifdef _CTRL_PROGRESSBAR
#include "ctrl/progressbar.h"
#endif /* _CTRL_PROGRESSBAR */

#ifdef _CTRL_LISTBOX
#include "ctrl/listbox.h"
#endif /* _CTRL_LISTBOX */

#ifdef _CTRL_PROPSHEET
#include "ctrl/propsheet.h"
#endif /* _CTRL_PROPSHEET*/

#ifdef _CTRL_COMBOBOX
#include "ctrl/combobox.h"
#endif /* _CTRL_COMBOBOX */

#ifdef _CTRL_TOOLBAR
#include "ctrl/toolbar.h"
#endif /* _CTRL_TOOLBAR */

#ifdef _CTRL_NEWTOOLBAR
#include "ctrl/newtoolbar.h"
#endif /* _CTRL_NEWTOOLBAR */

#ifdef _CTRL_MENUBUTTON
#include "ctrl/menubutton.h"
#endif /* _CTRL_MENUBUTTON */

#ifdef _CTRL_TRACKBAR
#include "ctrl/trackbar.h"
#endif /* _CTRL_TRACKBAR */

#ifdef _CTRL_SCROLLVIEW
#include "ctrl/scrollview.h"
#endif /* _CTRL_SCROLLVIEW */

#ifdef _CTRL_SCROLLBAR
#include "ctrl/scrollbar.h"
#endif /* _CTRL_SCROLLBAR */

#ifdef _CTRL_TEXTEDIT
#include "ctrl/textedit.h"
#endif /* _CTRL_TEXTEDIT */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CONTROL_H */

