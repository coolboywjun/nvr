/**
 * \file skin.h
 * \author Feynman Software (http://www.minigui.com).
 * \date 2003/10/10
 * 
 *  This file includes interfaces for skin module of MiniGUIExt library, 
 *  which provides skin support for MiniGUI applications.
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
 * $Id: skin.h 7369 2007-08-16 05:28:10Z xgwang $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2007 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */


#ifndef _MGUI_SKIN_H
#define _MGUI_SKIN_H

#ifdef _EXT_SKIN

    /**
     * \addtogroup mgext_fns Interfaces of the MiniGUI extension 
     *             library (libmgext)
     * @{
     */

    /**
     * \defgroup mgext_skin_fns Skin related routines
     *
     * @{
     */

/* skin messages (send to items) */
#define SKIN_MSG_LBUTTONDOWN    1
#define SKIN_MSG_LBUTTONUP      2
#define SKIN_MSG_CLICK          3
/*#define SKIN_MSG_DBCLICK      4 */
#define SKIN_MSG_MOUSEMOVE      5
#define SKIN_MSG_MOUSEDRAG      6
#define SKIN_MSG_SETFOCUS       7
#define SKIN_MSG_KILLFOCUS      8

/* type of item */

/**
 * \def SI_TYPE_NRMLABEL
 * \sa skin_item_s
*/
#define SI_TYPE_NRMLABEL        0x10000000

/**
 * \def SI_TYPE_BMPLABEL
 * \sa skin_item_s
*/
#define SI_TYPE_BMPLABEL        0x20000000

/**
 * \def SI_TYPE_CMDBUTTON
 * \sa skin_item_s
*/
#define SI_TYPE_CMDBUTTON       0x30000000

/**
 * \def SI_TYPE_CHKBUTTON
 * \sa skin_item_s
*/
#define SI_TYPE_CHKBUTTON       0x40000000

/**
 * \def SI_TYPE_NRMSLIDER
 * \sa skin_item_s
*/
#define SI_TYPE_NRMSLIDER       0x50000000

#ifdef _FIXED_MATH
/**
 * \def SI_TYPE_ROTSLIDER
 * \sa skin_item_s
*/
#define SI_TYPE_ROTSLIDER       0x60000000
#endif

/**
 * \def SI_TYPE_CONTROL
 * \brief The item is a control.
*/
#define SI_TYPE_CONTROL         0x70000000

/**
 * \def SI_TYPE_MASK
 * \brief The item type mask.
*/
#define SI_TYPE_MASK            0xF0000000

/* type of hittest shape */

/**
 * \def SI_TEST_SHAPE_RECT
 * \sa skin_item_s
*/
#define SI_TEST_SHAPE_RECT      0x01000000

#ifdef _USE_NEWGAL

/**
 * \def SI_TEST_SHAPE_ELLIPSE
 * \sa skin_item_s
*/
#define SI_TEST_SHAPE_ELLIPSE   0x02000000

/**
 * \def SI_TEST_SHAPE_LOZENGE
 * \sa skin_item_s
*/
#define SI_TEST_SHAPE_LOZENGE   0x03000000

/**
 * \def SI_TEST_SHAPE_LTRIANGLE
 * \sa skin_item_s
*/
#define SI_TEST_SHAPE_LTRIANGLE 0x04000000

/**
 * \def SI_TEST_SHAPE_RTRIANGLE
 * \sa skin_item_s
*/
#define SI_TEST_SHAPE_RTRIANGLE 0x05000000

/**
 * \def SI_TEST_SHAPE_UTRIANGLE
 * \sa skin_item_s
*/
#define SI_TEST_SHAPE_UTRIANGLE 0x06000000

/**
 * \def SI_TEST_SHAPE_DTRIANGLE
 * \sa skin_item_s
*/
#define SI_TEST_SHAPE_DTRIANGLE 0x07000000

#endif

#define SI_TEST_SHAPE_MASK      0x0F000000

/* common status of item */

/**
 * \def SI_STATUS_OK
 * \sa skin_item_s
*/
#define SI_STATUS_OK            0x00000001

/**
 * \def SI_STATUS_VISIBLE
 * \sa skin_item_s
*/
#define SI_STATUS_VISIBLE       0x00000002

/**
 * \def SI_STATUS_DISABLED
 * \sa skin_item_s
*/
#define SI_STATUS_DISABLED      0x00000004

/**
 * \def SI_STATUS_HILIGHTED
 * \sa skin_item_s
*/
#define SI_STATUS_HILIGHTED     0x00000008

#define SI_STATUS_MASK          0x0000000F

/* status of button item */

/**
 * \def SI_BTNSTATUS_CLICKED
 * \sa skin_item_s
*/
#define SI_BTNSTATUS_CLICKED    0x00000010

/**
 * \def SI_BTNSTATUS_CHECKED
 * \sa skin_item_s
*/
#define SI_BTNSTATUS_CHECKED    SI_BTNSTATUS_CLICKED

#define SI_STATUS_CLICKED       SI_BTNSTATUS_CLICKED

#define SI_BTNSTATUS_MASK       0x000000F0

/** Style of command button item */
#define SI_CMDBUTTON_2STATE     0x00000100

/* style of normal slider item */

/**
 * \def SI_NRMSLIDER_HORZ
 * \sa skin_item_s
*/
#define SI_NRMSLIDER_HORZ       0x00000100

/**
 * \def SI_NRMSLIDER_VERT
 * \sa skin_item_s
*/
#define SI_NRMSLIDER_VERT       0x00000200

/**
 * \def SI_NRMSLIDER_STATIC
 * \sa skin_item_s
*/
#define SI_NRMSLIDER_STATIC     0x00000400

#ifdef _FIXED_MATH

/* style of rotation slider item */

/**
 * \def SI_ROTSLIDER_CW
 * \sa skin_item_s
*/
#define SI_ROTSLIDER_CW         0x00000100

/**
 * \def SI_ROTSLIDER_ANTICW
 * \sa skin_item_s
*/
#define SI_ROTSLIDER_ANTICW     0x00000200

/**
 * \def SI_ROTSLIDER_STATIC
 * \sa skin_item_s
*/
#define SI_ROTSLIDER_STATIC     0x00000400

#endif /* _FIXED_MATH */

#define SI_STYLE_MASK           0x0000FF00

/**
 * \def CTRL_SKIN
 * \brief The class name of skin control. 
 */
#define CTRL_SKIN               "skin"

/**
 * \var typedef struct skin_item_ops_s skin_item_ops_t
 * \sa skin_item_ops_s
 */
typedef struct skin_item_ops_s skin_item_ops_t;

/**
 * \var typedef struct skin_head_s skin_head_t
 * \sa skin_head_s
 */
typedef struct skin_head_s skin_head_t;

/** Skin item information structure */
typedef struct skin_item_s
{
    /** The identifier of the item. */
    int id;

    /**
     * The style of the item.
     *
     * The style of an item is composed of five classes of information:
     *
     *  - The type of an item.
     *  - The hit-test shape type.
     *  - The common status.
     *  - The item specific status.
     *  - The item specific style.
     *
     * The type of an item can be one of the following values:
     *
     *  - SI_TYPE_NRMLABEL\n
     *    The item is a normal label.
     *  - SI_TYPE_BMPLABEL\n
     *    The item is a bitmap label.
     *  - SI_TYPE_CMDBUTTON\n
     *    The item is a command button.
     *  - SI_TYPE_CHKBUTTON\n
     *    The item is a check button.
     *  - SI_TYPE_NRMSLIDER\n
     *    The item is a normal slider.
     *  - SI_TYPE_ROTSLIDER\n
     *    The item is a rotation slider.
     *
     * The hit-test shape type can be one of the following values:
     *
     *  - SI_TEST_SHAPE_RECT\n
     *    The hit-test shape is a rect.
     *  - SI_TEST_SHAPE_ELLIPSE\n
     *    The hit-test shape is an ellipse.
     *  - SI_TEST_SHAPE_LOZENGE\n
     *    The hit-test shape is a lozenge.
     *  - SI_TEST_SHAPE_LTRIANGLE\n
     *    The hit-test shape is a left triangle.
     *  - SI_TEST_SHAPE_RTRIANGLE\n
     *    The hit-test shape is a right triangle.
     *  - SI_TEST_SHAPE_UTRIANGLE\n
     *    The hit-test shape is a up triangle.
     *  - SI_TEST_SHAPE_DTRIANGLE\n
     *    The hit-test shape is a down triangle.
     *
     * The common status of an item can be OR'ed with the following values:
     *
     *  - SI_STATUS_OK\n
     *    The basic status of an item, always be there.
     *  - SI_STATUS_VISIBLE\n
     *    The item is visble.
     *  - SI_STATUS_DISABLED\n
     *    The item is disabled.
     *  - SI_STATUS_HILIGHTED\n
     *    The item is hilighted.
     *
     *  The item specific status can be one of the following values:
     *
     *  - SI_BTNSTATUS_CLICKED\n
     *    The three-state button is clicked.
     *  - SI_BTNSTATUS_CHECKED\n
     *    The check button is ckecked.
     *
     *  The item specific style can be a valued OR'ed with the following values:
     *
     *  - SI_NRMSLIDER_HORZ\n
     *    The normal slider is horizontal.
     *  - SI_NRMSLIDER_VERT\n
     *    The normal slider is vertical.
     *  - SI_NRMSLIDER_STATIC\n
     *    The normal slider is static, i.e., the slider acts like a 
     *    progress bar.
     *  - SI_ROTSLIDER_CW\n
     *    The rotation slider is clockwise.
     *  - SI_ROTSLIDER_ANTICW\n
     *    The rotation slider is anti-clockwise.
     *  - SI_ROTSLIDER_STATIC\n
     *    The rotation slider is static, i.e., the slider acts like a 
     *    progress bar.
     */
    DWORD style;

    /** X-coordinate of the item in the skin. */
    int x;
    /** Y-coordinate of the item in the skin. */
    int y;

    /** The hit-test rectangle of the item. */
    RECT item_rc;

    /** The index of the item bitmap in the skin bitmap array. */
    int bmp_index;

    /** The tip text. */
    char* tip;

    /** The attached private data with the item by application. */
    DWORD attached;

    /** 
     * The type-specific data of the item, 
     * used to define extra information of the item.
     */
    void* type_data;

    /******** start of internal fields ********/
    /* The bounding rectangle of the item. */
    RECT shape;
    /* The hit-test rectangle of the item. */
    RECT rc_hittest;
    /* The region for mouse hittest. */
    CLIPRGN region;
    /* The operation routines of the item. */
    skin_item_ops_t *ops;
    /* The parent skin. */
    skin_head_t *hostskin;
} skin_item_t;

/** Skin item operations */
struct skin_item_ops_s
{
    /** Operations for initialization. */
    int (*init) (skin_head_t*, skin_item_t*);
    /** Operations for deinitialization. */
    int (*deinit) (skin_item_t*);

    /** Operations when creating the skin window. */
    int (*on_create) (skin_item_t*);
    /** Operations when destroying the skin window. */
    int (*on_destroy) (skin_item_t*);

    /** Operations for item drawing. */
    void (*draw_bg) (HDC, skin_item_t*);
    /** Operations for item drawing. */
    void (*draw_attached) (HDC, skin_item_t*);

    /** 
     * Operations for getting item value.
     * For button item, value is the click status, non-zero for clicked 
     * or checked. For slider, value is the position.
     * For label item, value is the label text.
     * For control item, value is the handle to control.
     */
    DWORD (*get_value) (skin_item_t *item);
    /** 
     * Operations for setting item value.
     * For button item, value is the click status, non-zero for clicked 
     * or checked. For slider, value is the position.
     * For label item, value is the label text.
     * For control item, value is the handle to control.
     */
    DWORD (*set_value) (skin_item_t *item, DWORD status);
    
    /** Item msg proc */
    int (*item_msg_proc) (skin_item_t* item, int message, 
                    WPARAM wparam, LPARAM lparam);
};

/** Slider information structure */
typedef struct sie_slider_s
{
    /** The minimal position of the thumb. */
    int min_pos;
    /** The maximal position of the thumb. */
    int max_pos;
    /** The current position of the thumb. */
    int cur_pos;
} sie_slider_t;

/** Normal label item info structure */
typedef struct si_nrmlabel_s
{
    /** The label string. */
    char* label;        

    /** The normal text color. */
    DWORD color;        
    /** The focus text color. */
    DWORD color_focus;  
    /** The click text color. */
    DWORD color_click;  
    /** The index of the logical font in the skin fonts array. */
    int font_index;
} si_nrmlabel_t;

/** Bitmap label item info structure */
typedef struct si_bmplabel_s
{
    /** The label string. */
    char* label;
    /** All label characters in the bitmap. */
    const char* label_chars;
} si_bmplabel_t;

/** Normal slider item info structure */
typedef struct si_nrmslider_s
{
    /** The slider information */
    sie_slider_t    slider_info;

    /** The index of the thumb bitmap in the skin bitmaps array. */
    int thumb_bmp_index;
} si_nrmslider_t;

#ifdef _FIXED_MATH

/** Rotation slider item info structure */
typedef struct si_rotslider_s
{
    /** The rotation radius. */
    int radius;
    /** The start degree. */
    int start_deg;
    /** The end degree. */
    int end_deg;
    /** The current degree. */
    int cur_pos;

    /** The index of the thumb bitmap in the skin bitmaps array. */
    int thumb_bmp_index;
} si_rotslider_t;

#endif /* _FIXED_MATH */

/**
 * \def SIE_BUTTON_CLICKED
 * \sa skin_event_cb_t
 */
#define SIE_BUTTON_CLICKED      0x00000001

/**
 * \def SIE_SLIDER_CHANGED
 * \sa skin_event_cb_t
 */
#define SIE_SLIDER_CHANGED      0x00000002

/**
 * \def SIE_GAIN_FOCUS
 * \sa skin_event_cb_t
 */
#define SIE_GAIN_FOCUS          0x00010000

/**
 * \def SIE_LOST_FOCUS
 * \sa skin_event_cb_t
 */
#define SIE_LOST_FOCUS          0x00020000

/**
 * \var int (* skin_event_cb_t) (HWND hwnd, skin_item_t* item, 
                int event, void* data)
 *
 * Event callback of skin item.
 *
 * The event can be one of the following values:
 *
 *  - SIE_GAIN_FOCUS\n
 *    The item gained the focus.
 *  - SIE_LOST_FOCUS\n
 *    The item losted the focus.
 *  - SIE_BUTTON_CLICKED\n
 *    The button item has been clicked.
 *  - SIE_SLIDER_CHANGED\n
 *    The position of the slider item has changed.
 */
typedef int (* skin_event_cb_t) (HWND hwnd, skin_item_t* item, 
                int event, void* data);
/**
 * \def MSG_CB_GOON
 * \sa skin_msg_cb_t
 */
#define MSG_CB_GOON     0

/**
 * \def MSG_CB_DEF_GOON
 * \sa skin_msg_cb_t
 */
#define MSG_CB_DEF_GOON 1

/**
 * \def MSG_CB_STOP
 * \sa skin_msg_cb_t
 */
#define MSG_CB_STOP     2

/**
 * \var int (* skin_msg_cb_t) (HWND hwnd, int message, 
                WPARAM wparam, LPARAM lparam, int* result)
 *
 * This is the type of message callback function of a skin window. 
 *
 * Before a skin window processes a message, it will call the message 
 * callback function if the application defined it.
 *
 * The application defined message callback function can process the message 
 * as a normal window procedure and return the result through the parameter 
 * \a result. 
 * However, the return value of a message callback function can be used
 * to control the behavior of the skin window's message handling procedure. 
 * It can be one of the following values:
 *
 *  - MSG_CB_GOON
 *    The message should be processed by skin window procedure, and
 *    the result of the callback will be ignored.
 *  - MSG_CB_DEF_GOON
 *    The message should be processed by the default window procedure, and
 *    the result of the callback will be ignored.
 *  - MSG_CB_STOP
 *    The message should not be processed, and the result is valid.
 */
typedef int (* skin_msg_cb_t) (HWND hwnd, int message, 
                WPARAM wparam, LPARAM lparam, int* result);

/**
 * \def SKIN_STYLE_NONE
 * \brief The style of the skin is none.
 */
#define SKIN_STYLE_NONE     0x00000000

/**
 * \def SKIN_STYLE_TOOLTIP
 * The skin has tooltip window.
 */
#define SKIN_STYLE_TOOLTIP  0x00000001

/**
 * \def SKIN_STYLE_MODAL
 * \brief The skin has modal window.
 */
#define SKIN_STYLE_MODAL    0x00000002

/** Skin header information structure */
struct skin_head_s
{
    /** The name of the skin. */
    char* name;

    /**
     * The style of the skin, can be OR'ed by the following values:
     *  - SKIN_STYLE_TOOLTIP
     *    The skin has tooltip window.
     */
    DWORD style;

    /** The pointer to the array of the bitmaps used by the skin. */
    const BITMAP* bmps;
    /** The pointer to the array of the logical fonts used by the skin. */
    const LOGFONT* fonts;

    /** The index of the background bitmap in the bitmap array. */
    int bk_bmp_index;

    /** The number of the items in this skin. */
    int nr_items;
    /** The pointer to the array of skin items. */
    skin_item_t* items;

    /** The attached private data with the skin by application. */
    DWORD attached;

    /******** start of internal fields ********/

    /* The event callback function of the skin. */
    skin_event_cb_t event_cb;

    /* The window message procedure of the skin window. */
    skin_msg_cb_t msg_cb;

    /**** The fields initialized when skin_init called. ****/
    /* The rectangle heap for hit-test regions. */
    BLOCKHEAP rc_heap;

    /**** 
     * The fields initialized when create_skin_main_window or 
     * create_skin_control called.
     ****/
    /* The handle of window in which the skin locates. */
    HWND hwnd;
    /* The handle of tool tip window. */
    HWND tool_tip;

    /* The old x and y. */
    int oldx, oldy;
    /* The current hilighted item. */
    skin_item_t* hilighted;

    /* cached identifier. */
    int cached_id;
    /* the item whose identifier is cached_id. */
    skin_item_t* cached_item;
};

/**
 * \fn BOOL skin_init (skin_head_t* skin, \
 *               skin_event_cb_t event_cb, skin_msg_cb_t msg_cb)
 * \brief Initializes a skin.
 *
 * This function initializes a skin \a skin before it can be used.
 * The initialization of a skin includes skin items initialization, etc, 
 * such as creating the hit-test region, allocating space for label, and so on.
 *
 * \param skin The pointer to the skin object. The skin can be
 *        a object returned by a hard-coded
 *        skin_head_t structure.
 * \param event_cb The item event callback of the skin.
 * \param msg_cb The window message callback of the skin window.
 *
 * \return TRUE for success, otherwise FALSE.
 *
 */
MGEXT_EXPORT BOOL skin_init (skin_head_t* skin, 
                skin_event_cb_t event_cb, skin_msg_cb_t msg_cb);

/**
 * \fn void skin_deinit (skin_head_t* skin)
 * \brief Deinitializes an initialized skin.
 *
 * This function deinitializes a skin, opposed to skin_init.
 *
 * \param skin The pointer to the skin object.
 *
 * \sa skin_init
 */
MGEXT_EXPORT void skin_deinit (skin_head_t* skin);

/**
 * \fn HWND create_skin_main_window_ex (skin_head_t* skin, HWND hosting, \
 *               int lx, int ty, int rx, int by, DWORD dwExStyle, BOOL modal)
 * \brief Creates a main window for a skin.
 *
 * This function creates a main window for the skin pointed to by \a skin.
 * The main window will have no caption, no menu, and no frame. However,
 * the window's caption string will be the name of the skin object.
 *
 * \param skin The pointer to the initialized skin object.
 * \param hosting The the hosting main window.
 * \param lx X-coordinate of the upper-left corner of the expected main window.
 * \param ty Y-coordinate of the upper-left corner of the expected main window.
 * \param rx X-coordinate of the lower-right corner of the expected main window.
 * \param by Y-coordinate of the lower-right corner of the expected main window.
 * \param dwExStyle The extended style of the expected control.
 * \param modal Whether to be a modal or modeless main window.
 *
 * \return The handle to the main window.
 *
 * \note The skin will store itself as the main window's additional data,
 *       so application should not call SetWindowAdditionalData to store
 *       other value.
 *
 * \sa create_skin_control
 */

MGEXT_EXPORT HWND create_skin_main_window_ex (skin_head_t* skin, HWND hosting, 
                int lx, int ty, int rx, int by, DWORD dwExStyle, BOOL modal);
/**
 * \def create_skin_main_window(skin, hosting, lx, ty, rx, by, modal)
 * \sa create_skin_main_window_ex (skin, hosting, lx, ty, rx, by, 0, modal)
 */
#define create_skin_main_window(skin, hosting, lx, ty, rx, by, modal) \
                create_skin_main_window_ex (skin, hosting, lx, ty, rx, by, 0, modal)
/**
 * \fn HWND create_skin_control (skin_head_t* skin, HWND parent, int id, \
 *               int x, int y, int w, int h)
 * \brief Creates a control for a skin.
 *
 * This function creates a control for the skin pointed to by \a skin.
 *
 * \param skin The pointer to the initialized skin object.
 * \param parent The handle to the parent window of the expected control.
 * \param id The identifier of the expected control.
 * \param x X-coordinate of the expected control in the parent window's 
 *        client area.
 * \param y Y-coordinate of the expected control in the parent window's 
 *        client area.
 * \param w The width of the expected control.
 * \param h The height of the expected control.
 *
 * \return The handle to the control.
 *
 * \note The skin will store itself as the control's additional data,
 *       so application should not call SetWindowAdditionalData to store 
 *       other value.
 *
 *       You can also create a skin control by calling CreateWindowEx in 
 *       the following manner:
 * \code
 *     CreateWindowEx (CTRL_SKIN, "", WS_VISIBLE | WS_CHILD, WS_EX_NONE, 
 *                     id, x, y, w, h, parent, (DWORD) skin);
 * \endcode
 *
 * \sa create_skin_main_window
 */
MGEXT_EXPORT HWND create_skin_control (skin_head_t* skin, HWND parent, 
                int id, int x, int y, int w, int h);

/**
 * \fn BOOL is_skin_main_window (HWND hwnd)
 * \brief Determines whether a window is a skin main window or not.
 *
 * The function determines whether a window specified by \a hwnd is a 
 * main window created by skin_main_window or not.
 *
 * \param hwnd The handle to the skin window.
 *
 * \return TRUE if it is , and FALSE if it isn't.
 *
 * \sa create_skin_main_window
 */
MGEXT_EXPORT BOOL is_skin_main_window (HWND hwnd);

/**
 * \fn void destroy_skin_window (HWND hwnd)
 * \brief Destroys a skin window.
 *
 * The function destroys the skin main window or control.
 *  
 * \param hwnd The handle to the skin window.
 *
 * \sa create_skin_main_window, create_skin_control
 */
MGEXT_EXPORT void destroy_skin_window (HWND hwnd);

/**
 * \fn skin_head_t* set_window_skin (HWND hwnd, skin_head_t *new_skin)
 * \brief Sets new skin.
 *
 * The function sets a new skin pointed to by \a new_skin for
 * the skin window \a hwnd.
 *
 * \param hwnd The handle to the skin window.
 * \param new_skin The pointer to the new skin object.
 *
 * \return The pointer to the old skin object.
 *
 * \sa skin_set_event_cb, skin_set_msg_cb
 */
MGEXT_EXPORT skin_head_t* set_window_skin (HWND hwnd, skin_head_t *new_skin);

/**
 * \fn skin_head_t* get_window_skin (HWND hwnd)
 * \brief Gets new skin.
 *
 * The function gets the skin of skin window \a hwnd.
 *
 * \param hwnd The handle to the skin window.
 *
 * \return The pointer to the skin object.
 *
 * \sa skin_set_event_cb, skin_set_msg_cb
 */
MGEXT_EXPORT skin_head_t* get_window_skin (HWND hwnd);

/**
 * \fn skin_event_cb_t skin_set_event_cb (skin_head_t* skin, \
 *               skin_event_cb_t event_cb)
 * \brief Sets new event callback of a skin.
 *
 * This function sets \a event_cb as the event callback of the skin \a skin.
 *
 * \param skin The pointer to the skin object.
 * \param event_cb The new event callback.
 *
 * \return The old event callback.
 *
 * \sa skin_set_msg_cb
 */
static inline skin_event_cb_t skin_set_event_cb (skin_head_t* skin, 
                skin_event_cb_t event_cb)
{
    skin_event_cb_t old = skin->event_cb;
    skin->event_cb = event_cb;
    return old;
}

/**
 * \fn skin_msg_cb_t skin_set_msg_cb (skin_head_t* skin, \
 *               skin_msg_cb_t msg_cb)
 * \brief Sets new message callback of a skin window.
 *
 * This function sets \a msg_cb as the message callback of the skin \a skin.
 *
 * \param skin The pointer to the skin object.
 * \param msg_cb The new message callback.
 *
 * \return The old message callback.
 *
 * \sa skin_set_event_cb
 */
static inline skin_msg_cb_t skin_set_msg_cb (skin_head_t* skin, 
                skin_msg_cb_t msg_cb)
{
    skin_msg_cb_t old = skin->msg_cb;
    skin->msg_cb = msg_cb;
    return old;
}

/**
 * \fn skin_item_t* skin_get_item (skin_head_t* skin, int id)
 * \brief Retrives a skin item through its identifier.
 *
 * This function retrives the pointer to a skin item by its identifier.
 *
 * \param skin The pointer to the skin.
 * \param id The identifier of the skin item.
 *
 * \return The pointer to the skin item, NULL on error.
 *
 * \sa skin_item_s
 */
MGEXT_EXPORT skin_item_t* skin_get_item (skin_head_t* skin, int id);

/**
 * \fn DWORD skin_get_item_status (skin_head_t* skin, int id)
 * \brief Retrives the common status of a skin item.
 *
 * This function retrives the common status of a skin item by its identifier.
 *
 * \param skin The pointer to the skin.
 * \param id The identifier of the skin item.
 *
 * \return The common status of the skin items, zero on error.
 *
 * \sa skin_item_s, skin_show_item, skin_enable_item, skin_set_hilited_item
 */
MGEXT_EXPORT DWORD skin_get_item_status (skin_head_t* skin, int id);

/**
 * \fn skin_item_t* skin_get_hilited_item (skin_head_t* skin)
 * \brief Gets the current highlighted skin item.
 *
 * \param skin The pointer to the skin.
 *
 * \return The pointer to the current highlighted skin item.
 *
 * \sa skin_item_s, skin_set_hilited_item
 */
MGEXT_EXPORT skin_item_t* skin_get_hilited_item (skin_head_t* skin);

/**
 * \fn DWORD skin_show_item (skin_head_t* skin, int id, BOOL show)
 * \brief Showes or hides a skin item.
 *
 * This function showes or hides a skin item whose identifier is \a id.
 *
 * \param skin The pointer to the skin.
 * \param id The identifier of the skin item.
 * \param show TRUE for show, FALSE for hide.
 *
 * \return The current common status of the skin items, zero when error.
 *
 * \sa skin_item_s, skin_get_item_status
 */
MGEXT_EXPORT DWORD skin_show_item (skin_head_t* skin, int id, BOOL show);

/**
 * \fn BOOL skin_enable_item (skin_head_t* skin, int id, BOOL enable)
 * \brief Enables or disables a skin item.
 *
 * This function enables or disables a skin item whose identifier is \a id.
 *
 * \param skin The pointer to the skin.
 * \param id The identifier of the skin item.
 * \param enable TRUE for enable, FALSE for disable.
 *
 * \return The current common status of the skin items, zero when error.
 *
 */
MGEXT_EXPORT DWORD skin_enable_item (skin_head_t* skin, int id, BOOL enable);

/**
 * \fn skin_item_t* skin_set_hilited_item (skin_head_t* skin, int id)
 * \brief Sets the current highlighted skin item.
 *
 * This function sets the current highlighted skin item.
 *
 * \param skin The pointer to the skin.
 * \param id The identifier of the skin item.
 *
 * \return The pointer to the old highlighted skin item.
 *
 * \sa skin_item_s, skin_get_hilited_item
 */
MGEXT_EXPORT skin_item_t* skin_set_hilited_item (skin_head_t* skin, int id);

/**
 * \fn BOOL skin_get_check_status (skin_head_t* skin, int id)
 * \brief Gets the check status of a check button item.
 *
 * This function gets the check status of a check button item,
 * checked or unchecked.
 *
 * \param skin The pointer to the skin.
 * \param id The identifier of the skin item.
 *
 * \return TRUE for checked, FALSE for unchecked.
 *
 * \sa skin_item_s, skin_set_check_status
 */
MGEXT_EXPORT BOOL skin_get_check_status (skin_head_t* skin, int id);

/**
 * \fn DWORD skin_set_check_status (skin_head_t* skin, \
 *               int id, BOOL check)
 * \brief Sets the check status of a check button item.
 *
 * This function sets the check status of a check button item.
 *
 * \param skin The pointer to the skin.
 * \param id The identifier of the skin item.
 * \param check Checks it or not.
 *
 * \return The current common status of the check button item.
 *
 * \sa skin_item_s, skin_get_check_status
 */
MGEXT_EXPORT DWORD skin_set_check_status (skin_head_t* skin, 
                int id, BOOL check);

/**
 * \fn const char* skin_get_item_label (skin_head_t* skin, int id)
 * \brief Gets the label string of a label item.
 *
 * This function returns the label string of a label item whose
 * identifier is \a id.
 *
 * \param skin The pointer to the skin.
 * \param id The identifier of the skin item.
 *
 * \return The pointer to the label string. You should not
 *         modify the content of the string directly.
 *
 * \sa skin_item_s, skin_set_item_label
 */
MGEXT_EXPORT const char* skin_get_item_label (skin_head_t* skin, int id);

/**
 * \fn BOOL skin_set_item_label (skin_head_t* skin, \
 *               int id, const char* label)
 * \brief Sets the label string of a label item.
 *
 * This function sets the label string of a label item and reflects
 * the appearance of the item if it is visible.
 *
 * \param skin The pointer to the skin.
 * \param id The identifier of the skin item.
 * \param label The pointer to the new label string.
 *
 * \return TRUE for success, FALSE on error.
 *
 * \sa skin_item_s, skin_get_item_label
 */
MGEXT_EXPORT BOOL skin_set_item_label (skin_head_t* skin, 
                int id, const char* label);

/**
 * \fn HWND skin_get_control_hwnd (skin_head_t* skin, int id)
 * \brief Gets the handle to a skin control item in the skin.
 *
 * This function returns the handle to a skin control item whose
 * identifier is \a id.
 *
 * \param skin The pointer to the skin.
 * \param id The identifier of the skin control item.
 *
 * \return The handle to the control.
 *
 * \sa skin_item_s
 */
MGEXT_EXPORT HWND skin_get_control_hwnd (skin_head_t* skin, int id);

/**
 * \fn int skin_get_thumb_pos (skin_head_t* skin, int id)
 * \brief Gets the thumb position of a slider item.
 *
 * This function gets the thumb position of a slider item.
 *
 * \param skin The pointer to the skin.
 * \param id The identifier of the skin item.
 *
 * \return The current thumb position.
 *
 * \sa skin_item_s, skin_set_thumb_pos
 */
MGEXT_EXPORT int skin_get_thumb_pos (skin_head_t* skin, int id);

/**
 * \fn BOOL skin_set_thumb_pos (skin_head_t* skin, int id, int pos)
 * \brief Sets the thumb position of a slider item.
 *
 * This function sets the thumb position of a slider item whose identifier
 * is \a id to the value of \a pos. It will redraw the item if the position
 * changed.
 *
 * \param skin The pointer to the skin.
 * \param id The identifier of the skin item.
 * \param pos The new position value.
 *
 * \return TRUE for success, FALSE on error.
 *
 * \sa skin_item_s, skin_get_thumb_pos
 */
MGEXT_EXPORT BOOL skin_set_thumb_pos (skin_head_t* skin, int id, int pos);

/**
 * \fn BOOL skin_get_slider_info (skin_head_t* skin, \
 *               int id, sie_slider_t* sie)
 * \brief Retrives the sliding information of a slider item.
 *
 * This function retrives the sliding information of a slider item whose
 * identifier is \a id.
 *
 * \param skin The pointer to the skin.
 * \param id The identifier of the skin item.
 * \param sie The pointer to a buffer which will be used to return the
 *        sliding information.
 *
 * \return TRUE for success, FALSE on error.
 *
 * \sa skin_item_s, skin_set_slider_info
 */
MGEXT_EXPORT BOOL skin_get_slider_info (skin_head_t* skin, 
                int id, sie_slider_t* sie);

/**
 * \fn BOOL skin_set_slider_info (skin_head_t* skin,  \
 *               int id, const sie_slider_t* sie)
 * \brief Sets the sliding information of a slider item.
 *
 * This function sets the sliding information of a slider item whose
 * identifier is \a id.
 *
 * \param skin The pointer to the skin.
 * \param id The identifier of the skin item.
 * \param sie The pointer to a sliding information structure.
 *
 * \return TRUE for success, FALSE on error.
 *
 */
MGEXT_EXPORT BOOL skin_set_slider_info (skin_head_t* skin, 
                int id, const sie_slider_t* sie);

/**
 * \fn int skin_scale_slider_pos (const sie_slider_t* org, \
 *               int new_min, int new_max)
 * \brief Scales the sliding position.
 *
 * This function is a helper function, it calculates and returns 
 * the new position value of a sliding information in the new sliding range
 * defined by \a new_max and \a new_min.
 * 
 * \param org The original sliding information, its range and current 
 *        position value.
 * \param new_min The new minimal value.
 * \param new_max The new maximal value.
 *
 * \return The new position value.
 *
 * \sa sie_slider_t
 */
MGEXT_EXPORT int skin_scale_slider_pos (const sie_slider_t* org, 
                int new_min, int new_max);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

    /** @} end of mgext_skin_fns */

    /** @} end of mgext_fns */

#endif /* _EXT_SKIN */

#endif /* _MGUI_SKIN_H */

