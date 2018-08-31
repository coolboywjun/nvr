#ifndef _TI_PTZ_CMD_H_
#define _TI_PTZ_CMD_H_

#if 0
typedef enum _TI_PTZ_CMD_E_
{
	PTZ_CMD_UP			=	0,	//上
	PTZ_CMD_SENSE		=	1,	//

	PTZ_CMD_DOWN		=	2,	//下
	PTZ_CMD_LEFT		=	3,	//左
	PTZ_CMD_RIGHT		=	4,	//右
	PTZ_CMD_LEFT_UP		=	5,	//左上
	PTZ_CMD_LEFT_DOWN	=	6,	//左下
	PTZ_CMD_RIGHT_UP	=	7,	//右上
	PTZ_CMD_RIGHT_DOWN	=	8,	//右下
	PTZ_CMD_AUTOSCAN	=	9,	//自动扫描
	PTZ_CMD_MANUALSCAN	=	10,	//手动扫描
	PTZ_CMD_FOCUS_NEAR	=	11, //聚焦+
	PTZ_CMD_FOCUS_FAR	=	12, //聚焦-
	PTZ_CMD_IRIS_CLOSE	=	13, //光圈+
	PTZ_CMD_IRIS_OPEN	=	14,	//光圈-
	PTZ_CMD_ZOOM_WIDE	=	15,	//变倍+
	PTZ_CMD_ZOOM_TELE	=	16, //变倍-
	PTZ_CMD_STOP		=	17,	//停止

	PTZ_CMD_SET_PRESET	=	18, //预置位设置
	PTZ_CMD_CLR_PRESET	=	19,	//预置位清楚
	PTZ_CMD_GOTO_PRESET	=	20, //预置位调用
	PTZ_CMD_FLIP		=	21, //翻转
	PTZ_CMD_GOTO_ZEROPAN=	22, //零位调用
	PTZ_CMD_SET_AUX		=	23, //设置辅助开关
	PTZ_CMD_CLR_AUX		=	24, //清除辅助开关
	PTZ_CMD_REMOTE_RESET=	25, //远程恢复
	PTZ_CMD_ZONE_START	=	26, //设置花样扫描开始
	PTZ_CMD_ZONE_END	=	27, //设置花样扫描结束
	PTZ_CMD_WR_CHAR		=	28, //写字符
	PTZ_CMD_CLR_SCR		=	29, //清楚字符
	PTZ_CMD_ALM_ACK		=	30, //报警确认
	PTZ_CMD_ZONE_SCAN_ON=	31, //开启花样扫描
	PTZ_CMD_ZONE_SCAN_OFF=	32, //停止谎言扫描
	PTZ_CMD_PTN_START	=	33, //
	PTZ_CMD_PTN_STOP	=	34, //
	PTZ_CMD_RUN_PTN		=	35, //
	PTZ_CMD_ZOOM_SPEED	=	36, //变倍速度
	PTZ_CMD_FOCUS_SPEED	=	37, //聚焦速度
	PTZ_CMD_RESET_CAMERA=	38, //相机复位
	PTZ_CMD_AUTO_FOCUS	=	39, //自动聚焦
	PTZ_CMD_AUTO_IRIS	=	40,	//自动光圈 
	PTZ_CMD_AUTO_AGC	=	41, //自动增益
	PTZ_CMD_BACKLIGHT_COMP=42,	//
	PTZ_CMD_AUTO_WB		  =	43,	//自动白平衡
	PTZ_CMD_DEV_PHASE_DELAY=44,	//
	PTZ_CMD_SET_SHTTER_SPEED=45,//设置快门速度
	PTZ_CMD_ADJ_LL_PHASE_DELAY=46,
	PTZ_CMD_ADJ_WB_RB		=47,//调整自动白平衡红蓝
	PTZ_CMD_ADJ_WB_MG		=48,//调整自动白平衡
	PTZ_CMD_ADJ_GAIN		=49,//调整自动增益
	PTZ_CMD_AUTO_IRIS_LV	=50,//调整自动光圈level
	PTZ_CMD_AUTO_IRIS_PEAK	=51,//调整自动光圈peak
	PTZ_CMD_QUERY			=52,//查询
	
	PTZ_CMD_INVALID			=53,
	
/* BEGIN: Added by xq, 2011/12/13 */
	PTZ_CMD_SET_CRUISE	= 1000,		// 设置巡航扫描
	PTZ_CMD_SET_CRUISE_START,		// 自动巡航扫描 
	PTZ_CMD_SET_GUARD_POS,			// 设置看守位
	PTZ_CMD_GOTO_GUARD_POS,			// 调用看守位
	PTZ_CMD_SET_AUTOSCAN_SETTING,	// 设置自动扫描
	PTZ_CMD_SET_AUTOSCAN_START,		// 开启自动扫描 
	PTZ_CMD_SET_ABSCAN_SETTING,		// 设置AB两点扫描
	PTZ_CMD_SET_ABSCAN_LEFT_POS,	// 设置AB两点左限位 
	PTZ_CMD_SET_ABSCAN_RIGHT_POS,	// 设置AB两点右限位
	PTZ_CMD_SET_ABSCAN_START,		// 开启AB两点扫描
	PTZ_CMD_SET_IDLE_SETTING,		// 设置闲置动作
	PTZ_CMD_SET_ONTIME_SETTING,		// 设置定时动作
	PTZ_CMD_SET_ALARM_SETTING,		// 设置报警联运时的动作
	PTZ_CMD_TRIG_ALARM,				// 报警触发
	PTZ_CMD_CLR_ALARM,
	PTZ_CMD_SET_IR_MODE,			// 红外灯功能
	PTZ_CMD_SET_DAY_NIGHT_MODE,		// 日夜模式
	PTZ_CMD_SET_180_AUTO_FLIP,		// 180自动翻转
	PTZ_CMD_SET_IRIS_MODE,			// 光圈工作模式
	PTZ_CMD_SET_BLC_MODE,			// 背光补偿
	PTZ_CMD_SET_ZOOM_DISPLAY,		// 变位显示
	PTZ_CMD_SET_FRAME_NOISE,		// 帧降噪
	PTZ_CMD_SET_WB_MODE,			// 白平衡模式
	PTZ_CMD_SET_WDR_MODE,			// 宽动态
	PTZ_CMD_SET_AUTO_ZOOM,			// 自动变倍
	PTZ_CMD_GET_ZOOM,				// 获取变倍大小
	PTZ_CMD_QUERY_STATUS,			// 查询球机状态
	PTZ_CMD_GET_VERSION_ID,			// 获取高速球版本号
	PTZ_CMD_SET_BRIGHT,				// 设置亮度
	PTZ_CMD_SET_CONTRAST,			// 设置对比度
	PTZ_CMD_SET_SATURATION,			// 设置饱和度
	PTZ_CMD_SET_SHARPNESS,			// 设置锐度
	PTZ_CMD_SET_RED,				// 设置红色
	PTZ_CMD_SET_BLUE,				// 设置蓝色
	PTZ_CMD_GET_SOFT_ID,			// 获取软件ID
	PTZ_CMD_GET_FEATURE_INFO,		// 获取功能信息
	PTZ_CMD_GET_CAMERA_INFO,		// 获取机芯信息
	PTZ_CMD_GET_FRAME_RATE,			// 获取球机当前帧率
	
	PTZ_CMD_SET_DEFAULT = 1100,		// 恢复默认出厂设置
	PTZ_CMD_INVALID_CMD = 1200,		// 无效命令
/* END:   Added by xq, 2011/12/13 */
} TI_PTZ_CMD_E;
#endif

#endif /*  #ifndef _TI_PTZ_CMD_H_ */
