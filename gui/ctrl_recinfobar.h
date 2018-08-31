#ifndef CUSTOM_CTRL_RECINFOBAR
#define CUSTOM_CTRL_RECINFOBAR
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MAX_INTRL_NUM 	(1500)

#define CTRL_RECINFOBAR 		("recinfobar")

#define RIBLEN_TIP 				31

/* 放大倍数 */
enum RECBAR_ZOOMOUT_TYPE
{
	ZOOM_OUT_X0 = 0,		/* 原始 */
	ZOOM_OUT_X1_3,		/* 三倍放大 */
	ZOOM_OUT_X1_6,		/* 六倍放大 */
	ZOOM_OUT_X1_12,		/* 十二倍放大 */
	ZOOM_OUT_X1_24,		/* 二十四倍放大 */
	ZOOM_OUT_X1_48		/* 四十八倍放大 */
};

enum REC_TYPE_COLOR_INDEX
{	
	REC_COLOR_NONE = 0,
	REC_MANNUAL_COLOR,
	REC_ALARM_COLOR,
	REC_MOTION_COLOR,
	REC_LOST_COLOR,
	REC_HIDE_COLOR,
	REC_TIMER_COLOR,
	REC_STARTUP_COLOR,

};
enum REC_TYPE
{
//	REC_BEGIN,

	REC_MANNUAL,
	REC_ALARM,
	REC_MOTION,
	REC_LOST,
	REC_HIDE,
	REC_TIMER,
	REC_STARTUP,

//	REC_END = MAX_COLOR_NUM;
	
};
#define REC_MANNUAL_MASK	(0x01)
#define REC_ALARM_MASK		(0x02)
#define REC_MOTION_MASK		(0x04)
#define REC_LOST_MASK		(0x08)
#define REC_HIDE_MASK		(0x10)
#define REC_TIMER_MASK		(0x20)
#define REC_STARTUP_MASK	(0x40)
#define REC_TYPE_MASK  (0x7F)

typedef struct _RECINTERVAL
{
	unsigned char unValid;		/* 0 为非法的， 1为合法的 */
	int nRecType;
	int nStart;
	int nEnd;
}RECINTERVAL;


BOOL RegisterRecinfoBarControl (void);
void RecinfoBarControlCleanup (void);

    /**
     * \defgroup ctrl_recinfobar_styles Styles of recinfobar control
     * @{
     */

/**
 * \def RIBS_NOTIFY
 * \brief Causes the recinfobar to notify the parent window with a notification message 
 *         when the user clicks or doubleclicks the recinfobar.
 */
#define RIBS_NOTIFY                 0x0001L

/**
 * \def RIBS_VERTICAL
 * \brief The recinfobar control will be oriented vertically.
 */
#define RIBS_VERTICAL               0x0002L

/**
 * \def RIBS_BORDER
 * \brief The recinfobar control will have border.
 */
#define RIBS_BORDER                 0x0004L

/**
 * \def RIBS_TIP
 * \brief The recinfobar control will display tip string above the control.
 */
#define RIBS_TIP                    0x0008L

/**
 * \def TBS_NOTICK
 * \brief The recinfobar control will not display tick line beside the control.
 */
#define RIBS_NOTICK                 0x0010L

/* internally used style */
#define RIBS_FOCUS                  0x1000L

    /** @} end of ctrl_recinfobar_styles */

    /**
     * \defgroup ctrl_recinfobar_msgs Messages of recinfobar control
     * @{
     */

#define RIBST_FOCUS 			0x0040
#define RIBST_DRAGGED			0x0080
#define RIBST_COMMON_REC		0x0004
#define RIBST_MANUAL_REC		0x0008
#define RIBST_TIMER_REC			0x0010
#define RIBST_MOTION_REC		0x0020
#define RIBST_LOST_REC			0x0040
#define RIBST_ALARM_REC			0x0080
#define RIBST_RECTYPE_MASK 		0x00FC

/**
 * \def RIBM_SETRANGE
 * \brief Sets the range of minimum and maximum logical positions for the 
 *         slider in a trackbar.
 *
 * \code
 * RIBM_SETRANGE
 * int min;
 * int max;
 *
 * wParam = (WPARAM)min;
 * lParam = (LPARAM)max;
 * \endcode
 *
 * \param min Minimum position of the range.
 * \param max Maximum position of the range.
 *
 * \return Zero on success; otherwise -1.
 */
#define RIBM_SETRANGE               0xF090

/**
 * \def RIBM_GETMIN
 * \brief Gets the minimum logical position.
 *
 * \code
 * RIBM_GETMIN
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The minimum logical position.
 */
#define RIBM_GETMIN                 0xF091

/**
 * \def RIBM_GETMAX
 * \brief Gets the minimum logical position.
 *
 * \code
 * RIBM_GETMAX
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The minimum logical position.
 */
#define RIBM_GETMAX                 0xF092

/**
 * \def RIBM_SETINTRL
 * \brief Set a new interval.
 *
 * \code
 * RIBM_SETINTRL
 * RECINTERVAL intval;
 * intval.nType = [type];
 * intval.nStart = [start];
 * intval.nEnd = [end];
 * 
 * wParam = (WPARAM) &intval;
 * lParam = 0;
 * \endcode
 *
 * \param pos New logical position of the slider.
 *
 * \return Always be zero.
 */
#define RIBM_SETINTRL                 0xF093

/**
 * \def RIBM_GETIND_INTRL_INDEX
 * \brief Gets the index of intval, where indicator is in.
 *
 * \code
 * RIBM_GETIND_INTRL_INDEX
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current logical position of the indicator.
 */
#define RIBM_GETIND_INTRL_INDEX                 0xF094

/**
 * \def RIBM_GETINDCPOS
 * \brief Gets the current logical position of the indicator.
 *
 * \code
 * RIBM_GETINDCPOS
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current logical position of the indicator.
 */
#define RIBM_GETINDCPOS                 0xF095

/**
 * \def RIBM_GETINDCPOS
 * \brief Gets the current logical position of the indicator.
 *
 * \code
 * RIBM_GETINDCPOS
 * int nPos;
 * wParam = (WPARAM)nPos;
 * lParam = 0;
 * \endcode
 *
 * \return The current logical position of the indicator.
 */
#define RIBM_SETINDCPOS                 0xF096

/**
 * \def RIBM_GETRECTYPE
 * \brief Gets the record type of the interval, place where the indicator on.
 *
 * \code
 * RIBM_GETRECTYPE
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *logical
 * \return The record type of the interval, where the indicator on. 
 */
#define RIBM_GETRECTYPE                 0xF097

/**
 * \def RIBM_CLEAR
 * \brief clear all the contents
 *
 * \code
 * RIBM_CLEAR
 * BOOL bEraseBkgnd = FALSE;
 * bEraseBkgnd = (BOOL)wParam;
 * lParam = 0;
 * \endcode
 *logical
 * \return The record type of the interval, where the indicator on. 
 */
#define RIBM_CLEAR						0xF098

/** 
 * \def RIBM_GETRECTINTRL
 * \biref Gets the record interval of the indicaton on 
 *
 * \code
 * RIBM_GETRECINTRL
 * unsigned int interaval;
 *
 * wParam = (WPARAM )&interval;
 * lParam = 0;
 * int lowLimit = (LOWORD) interval;
 * int highLimit = (HIWORD) interval;
 * \endcode
 *
 * \return Zero on success; otherwise -1.
 */
#define RIBM_GETRECINTRL				0xF099

#define RIBM_GETRECINTRL_BY_INDEX		0xF09A

#define RIBM_ZOOMOUT					0xF09B

#define RIBM_SET_CUR_MINMAX				0xF09C

#define RIBM_SET_ZOOMOUT_PARAM			0xF09D

#define RIBM_GET_CUR_MIN				0xF09E

#define RIBM_GET_CUR_MAX				0xF09F

#define RIBM_SET_STAT_TYPE				0xF0A0

#define RIBM_SHOWINDC					0xF0A1

#define RIBR_MSGMAX                 0xF100   
/* modify end */

    /** @} end of ctrl_trackbar_msgs */

    /**
     * \defgroup ctrl_trackbar_ncs Notification codes of trackbar control
     * @{
     */

/**
 * \def RIBN_REACHMIN
 * \brief Notifies that the indicator has reached the minimum position.
 */
#define RIBN_REACHMIN                1

/**
 * \def RIBN_REACHMAX
 * \brief Notifies that the indicator has reached the maximum position.
 */
#define RIBN_REACHMAX                2

/**
 * \def RIBN_CHANGE
 * \brief Notifies that the position of the indicator has changed.
 */
#define RIBN_INDCHANGE                  3

/**
 * \def RIBN_STARTDRAG
 * \brief Notifies that the user start to drag the indicator.
 */
#define RIBN_STARTDRAG               4

/**
 * \def RIBN_STOPDRAG
 * \brief Notifies that the user stop to drag the indicator.
 */
#define RIBN_STOPDRAG                5


/**
 * \def RIBN_LBUTTONDBLCLK
 * \brief Notifies that the user double clicked.
 */
#define RIBN_LBUTTONDBLCLK			 6


#define RIBN_REACHLOW				7


#define RIBN_REACHHI				8

#define RIBN_CUR_MIN_CHANGE			9

#define RIBN_CUR_MAX_CHANGE			10

#define RIBN_ZOOMOUT_CHANGE			11


    /** @} end of ctrl_trackbar_ncs */

    /** @} end of ctrl_trackbar */

    /** @} end of controls */
#ifdef  __cplusplus
}
#endif

#endif /* CUSTOM_CTRL_RECINFOBAR */
