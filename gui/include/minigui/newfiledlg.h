/**
 * \file newfiledlg.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2004/05/09
 * 
 *  This file includes interfaces of new Open File Dialog Box.
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
 * $Id: newfiledlg.h 7820 2007-10-11 07:29:00Z xwyan $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2007 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef _MGUI_NEWFILEDLG_H
#define _MGUI_NEWFILEDLG_H
#ifndef WIN32
#include <sys/types.h>
#include <dirent.h>
#else
#include <io.h>
#define access	_access
#define F_OK	0
#define R_OK	1
#define W_OK	2
#ifdef __MINIGUI_LIB__
#include <win32_dirent.h>
#else
#include <dirent.h>
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup mywins_fns Interfaces of MyWins module of MiniGUIExt library
     * @{
     */

    /**
     * \defgroup mywins_newfiledlg Open File Dialog Box
     * @{
     */

/**
 * \def FILE_ERROR_OK
 * \brief Open file success.
 */
#define FILE_ERROR_OK           0

/**
 * \def FILE_ERROR_PARAMERR
 * \brief Wrong parameters.
 */
#define FILE_ERROR_PARAMERR     -1

/**
 * \def FILE_ERROR_PARAMERR
 * \brief Path doesn't exist.
 */
#define FILE_ERROR_PATHNOTEXIST -2

/**
 * \def MAX_FILTER_LEN
 * \brief The maximum length of filter string.
 */
#define MAX_FILTER_LEN          255

/**
 * \def MY_NAMEMAX
 * \brief The maximum length of name.
 */
#define MY_NAMEMAX      127

/**
 * \def MY_PATHMAX
 * \brief The maximum length of path.
 */
#define MY_PATHMAX      255
/**
 * The file dialog box structure used by \a ShowOpenDialog.
 * \sa ShowOpenDialog.
 */
typedef struct _NEWFILEDLGDATA
{
    /** Indicates to create a Save File or an Open File dialog box. */
    BOOL    IsSave;
    /** The full path name of the file returned. */
    char    filefullname[MY_NAMEMAX + MY_PATHMAX + 1];
    /** The name of the file to be opened. */
    char    filename[MY_NAMEMAX + 1];
    /** The initial path of the dialog box. */
    char    filepath[MY_PATHMAX + 1];
    /**
     * The filter string, for example: 
     * All file (*.*)|Text file (*.txt;*.TXT)
     */
    char    filter[MAX_FILTER_LEN + 1];
    /** The initial index of the filter*/
    int     filterindex;
} NEWFILEDLGDATA;
/** Data type of pointer to a NEWFILEDLGDATA */
typedef NEWFILEDLGDATA* PNEWFILEDLGDATA;

/**
 * \fn int ShowOpenDialog (HWND hWnd, int lx, int ty,\
                int w, int h, PNEWFILEDLGDATA pnfdd)
 * \brief Creates an Open File Dialog Box.
 *
 * This function creates an Open File Dialog Box, and returns 
 * the full path name of the file selected by user.
 *
 * \param hWnd The hosting main window.
 * \param lx lx,ty,w,h: The default position and size of the dialog box.
 * \param ty lx,ty,w,h: The default position and size of the dialog box.
 * \param w  lx,ty,w,h: The default position and size of the dialog box.
 * \param h  lx,ty,w,h: The default position and size of the dialog box.
 * \param pnfdd The pointer to the NEWFILEDLGDATA structure.
 *
 * \retval IDOK The user choosed a file and OK button clicked.
 * \retval IDCANCLE CANCEL button clicked.
 * 
 * \sa NEWFILEDLGDATA, ShowOpenDialog
 */
MGEXT_EXPORT int ShowOpenDialog (HWND hWnd, int lx, int ty, 
                int w, int h, PNEWFILEDLGDATA pnfdd);

    /** @} end of mywins_newfiledlg */

    /** @} end of mywins_fns */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_NEWFILEDLG_H */


