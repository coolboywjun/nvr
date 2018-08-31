/**
 * \file colordlg.h
 * \author Linxs <Linxs@minigui.net>
 * \date 2004/6/16
 * 
 *  This file includes interfaces of Color Selection Dialog Box.
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
 * $Id: colordlg.h 7369 2007-08-16 05:28:10Z xgwang $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2007 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef _MGUI_COLORDLG_H
#define _MGUI_COLORDLG_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef _USE_NEWGAL

    /**
     * \addtogroup mywins_fns Interfaces of MyWins module of MiniGUIExt library
     * @{
     */

    /**
     * \defgroup mywins_colorseldlg Color Selection Dialog Box
     * @{
     */

/**
 * The color data structure used by \a ColorSelDialog.
 * \sa ColorSelDialog.
 */
typedef struct _COLORDATA {
    /** Reserves, not used now. */
    DWORD           style;
    /** The value of the color  returned. */
    gal_pixel       pixel;
    /** The R, G, B value of the color returned. */
    Uint8           r, g, b;
    /** The H value of the color returned. */
    Uint16          h;
    /** The S, V value of the color returned. */
    Uint8           s, v;
} COLORDATA, *PCOLORDATA;

/* Internal use */
#define SELCOLOR_OK     IDOK
/* Internal use */
#define SELCOLOR_CANCEL IDCANCEL

#include "colorspace.h"

/**
 * \fn int ColorSelDialog (HWND hWnd, int x, int y, \
 *               int w, int h, PCOLORDATA pClrData)
 * \brief Creates a Color Selection Dialog Box.
 *
 * This function creates a Color Selection Dialog Box, and returns 
 * the values of the color selected by the user.
 *
 * \param hWnd The hosting main window.
 * \param x  x,y,w,h: The default position and size of the dialog box.
 * \param y  x,y,w,h: The default position and size of the dialog box.
 * \param w  x,y,w,h: The default position and size of the dialog box.
 * \param h  x,y,w,h: The default position and size of the dialog box.
 * \param pClrData The pointer to the COLORDATA structure.
 *
 * \retval IDOK     The user choosed a color and OK button clicked.
 * \retval IDCANCEL The user clicked the Cancel button.
 * 
 * \sa COLORDATA, ColorSelDialog
 */
MGEXT_EXPORT int ColorSelDialog (HWND hWnd, int x, int y, 
                int w, int h, PCOLORDATA pClrData);

    /** @} end of mywins_colorseldlg*/

    /** @} end of mywins_fns */

#endif /* _USE_NEWGAL */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_COLORDLG_H */


