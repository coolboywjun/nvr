/**
 * \file ctrlhelper.h
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
 * $Id: ctrlhelper.h 7365 2007-08-16 05:22:17Z xgwang $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2007 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_CTRLHELPER_H
#define _MGUI_CTRL_CTRLHELPER_H
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup fns Functions
     * @{
     */

    /**
     * \addtogroup global_fns Global/general functions
     * @{
     */

    /**
     * \addtogroup misc_fns Miscellaneous functions
     * @{
     */

/**
 * \enum SBPolicyType
 * \brief Scroll bar display policies in scrolled window
 */
typedef enum
{
  /**
   * The scroll bar is always visible
   */
  SB_POLICY_ALWAYS,
  /**
   * The scroll bar is shown or hided automatically
   */
  SB_POLICY_AUTOMATIC,
  /** The scroll bar is never visbile
   */
  SB_POLICY_NEVER
} SBPolicyType;

/* Internal use */
int EditOnEraseBackground (HWND hWnd, HDC hdc, const RECT* pClipRect);

/**
 * \fn void GUIAPI DrawBoxFromBitmap (HDC hdc, const RECT* box, const BITMAP* bmp, BOOL h_v, BOOL do_clip)
 * \brief Draws a box from bitmap.
 *
 * \param hdc The handle to drawing context.
 * \param box The rectangle of the box.
 * \param bmp The pointer to the BITMAP object.
 * \param h_v Draw the box along horizental or vertical.
 * \param do_clip Does clip the drawing in the box?
 */
MG_EXPORT void GUIAPI DrawBoxFromBitmap (HDC hdc, const RECT* box, const BITMAP* bmp, BOOL h_v, BOOL do_clip);

#define DF_3DBOX_NORMAL     0x0000
#define DF_3DBOX_PRESSED    0x0001
#define DF_3DBOX_FOCUS		0x0002
#define DF_3DBOX_DISABLE	0x0004
#define DF_3DBOX_STATEMASK  0x000F
#define DF_3DBOX_NOTFILL    0x0000
#define DF_3DBOX_FILL       0x0010

/**
 * \fn void GUIAPI Draw3DControlFrameEx (HDC hdc, HWND hwnd, int x0, int y0, int x1, int y1, DWORD flags, gal_pixel fillc)
 * \brief Draws a 3D style frame control.
 *
 * \param hdc The handle to drawing context.
 * \param hwnd Tell the function to draw with the color definitions of this window.
 * \param x0  Specify the x-coordinate of the upper-left corner.
 * \param y0  Specify the y-coordinate of the upper-left corner.
 * \param x1  Specify the x-coordinate of the lower-right corner.  
 * \param y1  Specify the y-coordinate of the lower-right corner.
 * \param flags Specify the drawing flag, can be or'ed with the following flags:
 *              - DF_3DBOX_NORMAL\n The box is displayed with normal status.
 *              - DF_3DBOX_PRESSED\n The box is pressed.
 *              - DF_3DBOX_NOTFILL\n Do not fill the box.
 *              - DF_3DBOX_FILL\n Fill the box and the \a fillc defined the color.
 * \param fillc Specify the pixel value to fill the box when DF_3DBOX_FILL specified.
 *
 * \sa DrawFlatControlFrameEx
 */
MG_EXPORT void GUIAPI Draw3DControlFrameEx (HDC hdc, HWND hwnd, int x0, int y0, int x1, int y1, 
            DWORD flags, gal_pixel fillc);

/**
 * \def Draw3DControlFrame(hdc, l, t, r, b, fillc, updown)
 * \brief Back compatibility definitions 
 * \sa Draw3DControlFrameEx
 */
#define Draw3DControlFrame(hdc, l, t, r, b, fillc, updown) \
            Draw3DControlFrameEx(hdc, HWND_DESKTOP, l, t, r, b, \
            (updown?DF_3DBOX_NORMAL:DF_3DBOX_PRESSED) | ((fillc)?DF_3DBOX_FILL:0), fillc)

/**
 * \fn void GUIAPI DrawFlatControlFrameEx(HDC hdc, HWND hwnd, int x0, int y0, int x1, int y1, int corner, DWORD flags, gal_pixel fillc)
 * \brief Draws a flat frame control with triangle corner.
 *
 * \param hdc The handle to drawing context.
 * \param hwnd Tell the function to draw with the color definitions of this window.
 * \param x0  Specifies the x-coordinate of the upper-left corner.
 * \param y0  Specifies the y-coordinate of the upper-left corner.
 * \param x1  Specifies the x-coordinate of the lower-right corner.
 * \param y1  Specifies the y-coordinate of the lower-right corner.
 * \param corner Specifies the length of the edge of the triangle corner.
 * \param flags Specify the drawing flag, can be or'ed with the following flags:
 *              - DF_3DBOX_NORMAL\n The box is displayed with normal status.
 *              - DF_3DBOX_PRESSED\n The box is pressed.
 *              - DF_3DBOX_NOTFILL\n Do not fill the box.
 *              - DF_3DBOX_FILL\n Fill the box and the \a fillc defined the color.
 * \param fillc Specify the pixel value to fill the box when DF_3DBOX_FILL specified.
 *
 * \sa Draw3DControlFrameEx
 */
MG_EXPORT void GUIAPI DrawFlatControlFrameEx (HDC hdc, HWND hwnd, int x0, int y0, int x1, int y1, 
            int corner, DWORD flags, gal_pixel fillc);
/**
 * \def DrawFlatControlFrame(hdc, x0, y0, x1, y1, fillc, updown)
 * \brief Back compatibility definitions 
 * \sa DrawFlatControlFrameEx
 */
#define DrawFlatControlFrame(hdc, x0, y0, x1, y1, fillc, updown) \
            DrawFlatControlFrameEx(hdc, HWND_DESKTOP, x0, y0, x1, y1, 3, \
            (updown?DF_3DBOX_NORMAL:DF_3DBOX_PRESSED) | ((fillc)?DF_3DBOX_FILL:0), fillc)

/**
 * \fn void GUIAPI Draw3DThickFrameEx (HDC hdc, HWND hwnd, int l, int t, int r, int b, DWORD flags, gal_pixel fillc)
 * \brief Draws a thick 3D frame.
 *
 * This function draws a thin 3D frame (the border is 2-pixel wide), 
 * and fills the frame if a fill flag is specified.
 *
 * \param hdc The device context.
 * \param hwnd Tell the function to draw with the color definitions of this window.
 * \param l The x-coordinate of upper-left corner of the frame.
 * \param t The y-coordinate of upper-left corner of the frame.
 * \param r The x-coordinate of lower-right corner of the frame.
 * \param b The y-coordinate of lower-right corner of the frame.
 * \param flags Specify the drawing flag, can be or'ed with the following flags:
 *              - DF_3DBOX_NORMAL\n The box is displayed with normal status.
 *              - DF_3DBOX_PRESSED\n The box is pressed.
 *              - DF_3DBOX_NOTFILL\n Do not fill the box.
 *              - DF_3DBOX_FILL\n Fill the box and the \a fillc defined the color.
 * \param fillc Specify the pixel value to fill the box when DF_3DBOX_FILL specified.
 *
 * \sa Draw3DControlFrameEx, DrawFlatControlFrameEx
 */
MG_EXPORT void GUIAPI Draw3DThickFrameEx (HDC hdc, HWND hwnd,
                int l, int t, int r, int b, DWORD flags, gal_pixel fillc);

/**
 * \def Draw3DUpThickFrame
 * \brief Back compatibility definitions 
 * \sa Draw3DThickFrameEx
 */
#define Draw3DUpThickFrame(hdc, l, t, r, b, fillc) \
            Draw3DThickFrameEx(hdc, HWND_DESKTOP, l, t, r, b, DF_3DBOX_NORMAL | ((fillc)?DF_3DBOX_FILL:0), fillc)

/**
 * \def Draw3DDownThickFrame
 * \brief Back compatibility definitions 
 * \sa Draw3DThickFrameEx
 */
#define Draw3DDownThickFrame(hdc, l, t, r, b, fillc) \
            Draw3DThickFrameEx(hdc, HWND_DESKTOP, l, t, r, b, DF_3DBOX_PRESSED | ((fillc)?DF_3DBOX_FILL:0), fillc)

/**
 * \def Draw3DUpFrame
 * \sa Draw3DUpThickFrame
 */
#define Draw3DUpFrame   Draw3DUpThickFrame

/**
 * \def Draw3DDownFrame
 * \sa Draw3DUpThickFrame
 */
#define Draw3DDownFrame Draw3DDownThickFrame

/**
 * \fn void GUIAPI Draw3DThinFrameEx (HDC hdc, HWND hwnd, int l, int t, int r, int b, DWORD flags, gal_pixel fillc)
 * \brief Draws a thin 3D frame.
 *
 * This function draws a thin 3D frame (the border is 1-pixel wide), 
 * and fills the frame if a fill flag is specified.
 *
 * \param hdc The device context.
 * \param hwnd Tell the function to draw with the color definitions of this window.
 * \param l The x-coordinate of upper-left corner of the frame.
 * \param t The y-coordinate of upper-left corner of the frame.
 * \param r The x-coordinate of lower-right corner of the frame.
 * \param b The y-coordinate of lower-right corner of the frame.
 * \param flags Specify the drawing flag, can be or'ed with the following flags:
 *              - DF_3DBOX_NORMAL\n The box is displayed with normal status.
 *              - DF_3DBOX_PRESSED\n The box is pressed.
 *              - DF_3DBOX_NOTFILL\n Do not fill the box.
 *              - DF_3DBOX_FILL\n Fill the box and the \a fillc defined the color.
 * \param fillc Specify the pixel value to fill the box when DF_3DBOX_FILL specified.
 *
 * \sa Draw3DControlFrameEx, DrawFlatControlFrameEx
 */
MG_EXPORT void GUIAPI Draw3DThinFrameEx (HDC hdc, HWND hwnd,
                int l, int t, int r, int b, DWORD flags, gal_pixel fillc);

/**
 * \def Draw3DUpThinFrame(hdc, l, t, r, b, fillc)
 * \brief Back compatibility definitions
 * \sa Draw3DThinFrameEx
 */
#define Draw3DUpThinFrame(hdc, l, t, r, b, fillc) \
            Draw3DThinFrameEx(hdc, HWND_DESKTOP, l, t, r, b, DF_3DBOX_NORMAL | ((fillc)?DF_3DBOX_FILL:0), fillc)

/**
 * \def Draw3DDownThinFrame(hdc, l, t, r, b, fill)
 * \sa Draw3DThinFrameEx
 */
#define Draw3DDownThinFrame(hdc, l, t, r, b, fillc) \
            Draw3DThinFrameEx(hdc, HWND_DESKTOP, l, t, r, b, DF_3DBOX_PRESSED | ((fillc)?DF_3DBOX_FILL:0), fillc)

/**
 * \fn void GUIAPI Draw3DBorderEx (HDC hdc, HWND hwnd, int l, int t, int r, int b)
 * \brief Draws a 3D rectangle border.
 *
 * This function draws a 3D retangle border which is 2-pixel wide.
 *
 * \param hdc The device context.
 * \param hwnd Tell the function to draw with the color definitions of this window.
 * \param l The x-coordinate of upper-left corner of the rectangle.
 * \param t The y-coordinate of upper-left corner of the rectangle.
 * \param r The x-coordinate of lower-right corner of the rectangle.
 * \param b The y-coordinate of lower-right corner of the rectangle.
 *
 * \sa Draw3DThinFrameEx, Draw3DThickFrameEx 
 */
MG_EXPORT void GUIAPI Draw3DBorderEx (HDC hdc, HWND hwnd, int l, int t, int r, int b);

/**
 * \def Draw3DBorder(hdc, l, t, r, b)
 * \brief Back compatibility definitions 
 * \sa Draw3DBorderEx
 */
#define Draw3DBorder(hdc, l, t, r, b)   \
            Draw3DBorderEx(hdc, HWND_DESKTOP, l, t, r, b)

/**
 * \fn void GUIAPI DisabledTextOutEx (HDC hdc, HWND hwnd, int x, int y, const char* szText)
 * \brief Outputs disabled (grayed) text.
 *
 * This function outputs a grayed text at the specified position.
 *
 * \param hdc The device context.
 * \param hwnd Tell the function to draw with the color definitions of this window.
 * \param x The x-coordinate of start point.
 * \param y The y-coordinate of start point.
 * \param szText The null-terminated text to be outputted.
 *
 * \sa TextOut, DrawText
 */
MG_EXPORT void GUIAPI DisabledTextOutEx (HDC hdc, HWND hwnd, int x, int y, const char* szText);

/**
 * \fn void GUIAPI NotifyParentEx (HWND hwnd, int id, int code, DWORD add_data)
 * \brief Sends a notification message to the parent.
 *
 * By default, the notification from a control will be sent to its parent
 * window within a MSG_COMMAND messsage.
 *
 * Since version 1.2.6, MiniGUI defines the Nofication Callback Procedure 
 * for control. You can specify a callback function for a control by calling
 * \a SetNotificationCallback to receive and handle the notification from 
 * the control.
 *
 * If you have defined the Notificaton Callback Procedure for the control,
 * calling NotifyParentEx will call the notification callback procedure,
 * not send the notification message to the parent.
 *
 * \param hwnd The handle to current control window.
 * \param id The identifier of current control.
 * \param code The notification code.
 * \param add_data The additional data of the notification.
 *
 * \sa SetNotificationCallback
 */
MG_EXPORT void GUIAPI NotifyParentEx (HWND hwnd, int id, int code, DWORD add_data);

/**
 * \def NotifyParent(hwnd, id, code)
 * \brief Sends a notification message to the parent, 
 *        but without additional data.
 *
 * \param hwnd The handle to current control window.
 * \param id The identifier of current control.
 * \param code The notification code.
 *
 * \note This function is actually a macro of NotifyParentEx with 
 *       \a dwAddData being zero.
 *
 * \sa NotifiyParentEx
 */
#define NotifyParent(hwnd, id, code) \
                NotifyParentEx(hwnd, id, code, 0)

/**
 * \var typedef int (*STRCMP) (const char* s1, const char* s2, size_t n)
 * \brief Type of general strncmp function.
 *
 * The function compares the two strings \a s1 and \a s2. It returns
 * an integer less than, equal to, or greater than zero if  \a s1 is found,
 * respectively, to be less than, to match, or be greater than \a s2.
 *
 * Note that it only compares the first (at most) \a n characters of s1 and s2. 
 */
typedef int (*STRCMP) (const char* s1, const char* s2, size_t n);

MG_EXPORT int GUIAPI DefaultPageProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

    /** @} end of misc_fns */

    /** @} end of global_fns */

    /** @} end of fns */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_CTRLHELPER_H */

