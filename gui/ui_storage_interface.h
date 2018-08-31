#ifndef _UI_PARA_INTERFACE_H_
#define _UI_PARA_INTERFACE_H_
#include "nvr_lib_interface.h"
#include "storage_interface.h"
#include "player_interface.h"


typedef struct _ChRecordInfo
{	
	int	fragement;  //录像条数
	RecordQueryInfo *pstRecordHead; //录像数组首位置
}ChRecordInfo;

//一天所有录像信息
typedef struct _RecordInfo
{
	time_t  tm_fistfileTime;  //-1时表示一天中没有录像
	//一天的起始时间,
	time_t  tm_start;
	//一天的结束时间,
	time_t	tm_end;
	ChRecordInfo stChRecord[NVR_MAX_CHANNEL_NUM];
}RecordInfo;


typedef struct tag_PlayCtrl_t
{
	int    bfullscreen;  //1整屏，0非整屏
	int	   dlgBkColor;  //用于恢复
	int	   chSelect;
	int    displaymode;  //0 四分屏， 1 全屏
	time_t indicatorTime;   //回放时间轴
	int	   speed;
	int	   playdir;
	int    status;  	//0 stop, 1 start
}PlayCtrl_t;


time_t ui_param_day_to_time_t(int year, int month, int day, 
									int hour, int min, int sec);

int ui_param_get_record_query_list(int chn, time_t beginTime, time_t endTime, RecordQueryInfo **queryHead, int *nListSize, int record_type);


int ui_param_get_one_day_record(RecordInfo *pstRecordInfo, int year, int month, int day, int record_type);

int ui_param_free_record_info(RecordInfo *pstRecordInfo);

int ui_query_info_get_fragement_by_time(int ch, RecordInfo *pstRecordInfo, time_t time,RecordQueryInfo *pstFragement);

int ui_query_info_get_first_record_time(RecordInfo *pstRecordInfo, time_t *recordTime);

int ui_query_get_next_fragement(int ch, RecordInfo *pstRecordInfo, time_t curtime,RecordQueryInfo *pstNextFragement);

int ui_query_record_exist(time_t start, time_t end, RecordInfo *pstRecordInfo);

int ui_query_other_chanel_record_exist(int chn, time_t start, time_t end, RecordInfo *pstRecordInfo);

#endif
