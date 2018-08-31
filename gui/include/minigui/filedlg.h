/**
 * \file filedlg.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2001/12/29
 * 
 *  This file includes interfaces of Open File Dialog Box.
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
 * $Id: filedlg.h 7369 2007-08-16 05:28:10Z xgwang $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2007 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef _MGUI_FILEDLG_H
#define _MGUI_FILEDLG_H

#include <sys/types.h>
#include <dirent.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup mywins_fns Interfaces of MyWins module of MiniGUIExt library
     * @{
     */

    /**
     * \defgroup mywins_filedlg Open File Dialog Box
     * @{
     */

/**
 * \def FILE_ERROR_OK
 * \sa OpenFileDialogEx, OpenFileDialog
 */
#define FILE_ERROR_OK           0

/**
 * \def FILE_ERROR_PARAMERR
 * \sa OpenFileDialogEx, OpenFileDialog
 */
#define FILE_ERROR_PARAMERR     -1

/**
 * \def FILE_ERROR_PATHNOTEXIST
 * \sa OpenFileDialogEx, OpenFileDialog
 */
#define FILE_ERROR_PATHNOTEXIST -2

/**
 * The file dialog box structure used by \a OpenFileDialogEx.
 * \sa OpenFileDialogEx.
 */
typedef struct _FILEDLGDATA
{
    /** Indicates to create a Save File or an Open File dialog box. */
	BOOL   	IsSave;
    /** The full path name of the file returned. */
    char   	filefullname[NAME_MAX + PATH_MAX + 1];
    /** The name of the file to be opened. */
    char   	filename[NAME_MAX + 1];
    /** The initial path of the dialog box. */
    char   	filepath[PATH_MAX + 1];
} FILEDLGDATA;

/** Data type of pointer to a FILEDLGDATA */
typedef FILEDLGDATA* PFILEDLGDATA;

/**
 * \fn OpenFileDialogEx (HWND hWnd, int lx, int ty, \ 
               int w, int h, PFILEDLGDATA pmwfi)
 * \brief Creates an Open File Dialog Box.
 *
 * This function creates an Open File Dialog Box, and returns the full 
 * path name of the file selected by user.
 *
 * \param hWnd The hosting main window.
 * \param lx lx: The default position and size of the dialog box.
 * \param ty ly: The default position and size of the dialog box.
 * \param w  w: The default position and size of the dialog box.
 * \param h  h: The default position and size of the dialog box.
 * \param pmwfi The pointer to the FILEDLGDATA structure.
 *
 * \return A positive integer on success, negative on error.
 *
 * \retval IDOK The user choosed a file and OK button clicked.
 * \retval IDCANCLE CANCEL button clicked.
 * \retval FILE_ERROR_PARAMERR You passed a wrong parameters.
 * \retval FILE_ERROR_PATHNOTEXIST The path specified in \a pmwfi 
 *         does not exist.
 * 
 * \sa FILEDLGDATA, OpenFileDialog
 */
MGEXT_EXPORT int OpenFileDialogEx (HWND hWnd, int lx, int ty, 
                int w, int h, PFILEDLGDATA pmwfi);

/**
 * \fn OpenFileDialog (HWND hWnd, BOOL isSave, PFILEDLGDATA pmwfi)
 * \brief Creates an Open File Dialog Box.
 *
 * This function creates an Open File Dialog Box, and returns the 
 * full path name of the file selected by user. 
 *
 * It is defined an inline function calling \a OpenFileDialogEx.
 *
 * \param hWnd The hosting main window.
 * \param isSave Indicates to create a Save File or an Open File dialog box.
 * \param pmwfi The pointer to the FILEDLGDATA structure.
 *
 * \return A positive integer on success, negative on error.
 *
 * \retval IDOK The user choosed a file and OK button clicked.
 * \retval IDCANCLE CANCEL button clicked.
 * \retval FILE_ERROR_PARAMERR You passed a wrong parameters.
 * \retval FILE_ERROR_PATHNOTEXIST The path specified in \a pmwfi does not exist.
 * 
 * \sa FILEDLGDATA, OpenFileDialogEx
 */
static inline int OpenFileDialog (HWND hWnd, BOOL isSave, PFILEDLGDATA pmwfi)
{
    pmwfi->IsSave = isSave;
    return OpenFileDialogEx (hWnd, 50, 50, 330, 280, pmwfi);
}

    /** @} end of mywins_filedlg */

    /** @} end of mywins_fns */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_FILEDLG_H */


