/**
 * \file scrollbar.h
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
 * $Id: scrollbar.h 7365 2007-08-16 05:22:17Z xgwang $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2007 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_SCROLLBAR_H
#define _MGUI_CTRL_SCROLLBAR_H
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_scrollbar ScrollBar control
     *
     * \note The scrollbar is not implemented as a control in current version.
     * @{
     */

/**
 * \def CTRL_SCROLLBAR
 * \brief The class name of scrollbar control.
 */
#define CTRL_SCROLLBAR              ("scrollbar")

    /**
     * \defgroup ctrl_scrollbar_msgs Messages of scrollbar control
     * @{
     */

#define SBM_SETPOS                  0xF0E0
#define SBM_GETPOS                  0xF0E1
#define SBM_SETRANGE                0xF0E2
#define SBM_SETRANGEREDRAW          0xF0E6
#define SBM_GETRANGE                0xF0E3
#define SBM_ENABLE_ARROWS           0xF0E4
#define SBM_SETSCROLLINFO           0xF0E9
#define SBM_GETSCROLLINFO           0xF0EA

#define SBM_MSGMAX                  0xF0F0

    /** @} end of ctrl_scrollbar_msgs */

    /** @} end of ctrl_scrollbar */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_SCROLLBAR_H */

