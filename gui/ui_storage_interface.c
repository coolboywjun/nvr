#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <pthread.h>
#include "storage_interface.h"
#include "player_interface.h"
#include "ipnc_lib.h"
#include "nvr_lib_interface.h"
#include "storage_lib.h"

#include "ui_storage_interface.h"


time_t ui_param_day_to_time_t(int year, int month, int day, 
									int hour, int min, int sec)
{
	struct tm time_str;
	
	time_str.tm_year = year - 1900;
	time_str.tm_mon = month - 1;
	time_str.tm_mday = day;
	time_str.tm_hour = hour;
	time_str.tm_min = min;
	time_str.tm_sec = sec;
	time_str.tm_isdst = -1;

	return mktime(&time_str);
}

int ui_param_get_record_query_list(int chn, time_t beginTime, time_t endTime, RecordQueryInfo **queryHead, int *nListSize, int record_type)
{
	int i, ret;
	RecordQueryChnInfo *chn_info;
	RecordQueryInfo *query_info;
	int shmid;

	*queryHead = NULL;
	*nListSize = 0;

	ret = storage_lib_record_query(1<<chn,
							 	beginTime, endTime, record_type/*RECORD_ALL_TYPE*/,
							 	&shmid, &chn_info, &query_info);
	if(ret < 0)
	{
		printf("Failed to query record\n");
		return -1;
	}
	printf("fragment num: %d\n", ret);
	
	if(ret == 0)
	{	
		storage_lib_record_query_free(shmid, chn_info, query_info);	
		return 0;
	}
	
	*queryHead = (RecordQueryInfo *)malloc(ret*sizeof(RecordQueryInfo));
	//for (i = 0; i < ret; i++)
		memcpy(*queryHead, query_info, ret*sizeof(RecordQueryInfo));
	*nListSize = ret;
	storage_lib_record_query_free(shmid, chn_info, query_info);	
	return 0;
}


int ui_param_get_one_day_record(RecordInfo *pstRecordInfo, int year, int month, int day, int record_type)
{
	int ch;	
	int ret;
	time_t tm_Start, tm_End;
	ChRecordInfo *pstChRecord = NULL;
	//y-m-d h:m:s
	tm_Start = ui_param_day_to_time_t(year, month, day ,0, 0, 0);
	tm_End = tm_Start+3600*24;
	pstRecordInfo->tm_start = tm_Start;
	pstRecordInfo->tm_end = tm_End;
	
	for (ch = 0; ch < NVR_MAX_CHANNEL_NUM; ch++)
	{
		pstChRecord = &pstRecordInfo->stChRecord[ch];
		ret = ui_param_get_record_query_list(ch, tm_Start, tm_End, &pstChRecord->pstRecordHead, &pstChRecord->fragement, record_type);
		if (ret < 0 || pstChRecord->fragement == 0)	
			continue;
	}

	ret = ui_query_info_get_first_record_time(pstRecordInfo, &pstRecordInfo->tm_fistfileTime);
	if (ret < 0)
		pstRecordInfo->tm_fistfileTime =(time_t)-1;
	return 0;
}


int ui_param_free_record_info(RecordInfo *pstRecordInfo)
{	
	int ch; 
	ChRecordInfo *pstChRecord = NULL;
	if (pstRecordInfo == NULL)
		return 0;
	for (ch = 0; ch < NVR_MAX_CHANNEL_NUM; ch++)
	{		
		pstChRecord = &pstRecordInfo->stChRecord[ch];
		if (pstChRecord->pstRecordHead!= NULL)
		{
			free(pstChRecord->pstRecordHead);
			pstChRecord->pstRecordHead = NULL;
		}
		pstChRecord->fragement = 0;
		pstRecordInfo->tm_end = 0;
		pstRecordInfo->tm_start = 0;
	}
	return 0;
}



//当天的第一个录像
int ui_query_info_get_first_record_time(RecordInfo *pstRecordInfo, time_t *recordTime)
{
	int ch = 0, i = 0;
	time_t time_min = -1;
	int fragement = 0;
	int flag = 0;
	ChRecordInfo *pstChRecord = NULL;	
	RecordQueryInfo *pstQuery = NULL;
	*recordTime = 0;
	
	for (ch = 0; ch < NVR_MAX_CHANNEL_NUM; ch++)
	{
		pstChRecord = &pstRecordInfo->stChRecord[ch];
		fragement = pstChRecord->fragement;
		if (fragement == 0)
			continue;

		for (i = 0; i < fragement; i++)
		{			
			pstQuery = &pstChRecord->pstRecordHead[i];
			if (flag == 0)
			{
				time_min = pstQuery->begin_time;
				flag = 1;
			}
			else
			{
				if (pstQuery->begin_time < time_min)
					time_min = pstQuery->begin_time;
			}
		}
	}
	
	if (time_min < pstRecordInfo->tm_start)
		time_min = pstRecordInfo->tm_start;
	
	if (time_min == 0)
	{
		*recordTime = -1;
		return -1;
	}
	
	*recordTime = time_min;

	return 0;
}

//根据时间curtime，获取该通道下一段录像
int ui_query_get_next_fragement(int ch, RecordInfo *pstRecordInfo, time_t fileBegintime,RecordQueryInfo *pstNextFragement)
{
	int i = 0;
	int fragementNo = 0;
	int minIndex = -1;
	int flag = 0;
	time_t tm = 0;
	RecordQueryInfo *pstQuery = NULL;
	ChRecordInfo *pstChRecord = NULL;

	if (pstNextFragement == NULL)
		goto err;
	
	pstChRecord = &pstRecordInfo->stChRecord[ch];
	fragementNo = pstChRecord->fragement;
	if (fragementNo == 0) 
		goto err;

	for (i = 0; i < fragementNo; i++)
	{
		pstQuery = &pstChRecord->pstRecordHead[i];
		if (pstQuery->begin_time <= fileBegintime) 
			continue;

		if (flag == 0)
		{
			minIndex = i;
			flag = 1;
			tm = pstQuery->begin_time;
		}
		else
		{
			if (pstQuery->begin_time < tm)
			{
				minIndex = i;
				tm = pstQuery->begin_time;
			}
		}
	}

	if (minIndex == -1)
		goto err;

	//printf("ch[%d] next fragement %d\r\n", ch, minIndex);
	
	memcpy(pstNextFragement, &pstChRecord->pstRecordHead[minIndex], sizeof(RecordQueryInfo));
	return 0;

err:
	memset(pstNextFragement, 0, sizeof(RecordQueryInfo));
	return -1;
}

//判断在时间段start -- end内是否有录像
int ui_query_record_exist(time_t start, time_t end, RecordInfo *pstRecordInfo)
{
	int ch = 0, i = 0;
	//time_t time_min = 0;
	int fragement = 0;
	ChRecordInfo *pstChRecord = NULL;	
	RecordQueryInfo *pstQuery = NULL;
	
	for (ch = 0; ch < NVR_MAX_CHANNEL_NUM; ch++)
	{
		pstChRecord = &pstRecordInfo->stChRecord[ch];
		fragement = pstChRecord->fragement;
		if (fragement == 0)
			continue;

		for (i = 0; i < fragement; i++)
		{			
			pstQuery = &pstChRecord->pstRecordHead[i];
			if (((pstQuery->begin_time <= start)&&(pstQuery->end_time >= start))||
				((pstQuery->begin_time <= end)&&(pstQuery->end_time >= end)))
				return 1;
		}
	}
	return 0;
}

int ui_query_other_chanel_record_exist(int chn, time_t start, time_t end, RecordInfo *pstRecordInfo)
{
	int ch = 0, i = 0;
	//time_t time_min = 0;
	int fragement = 0;
	ChRecordInfo *pstChRecord = NULL;	
	RecordQueryInfo *pstQuery = NULL;
	
	for (ch = 0; ch < NVR_MAX_CHANNEL_NUM; ch++)
	{
		if (ch == chn)	continue;
		
		pstChRecord = &pstRecordInfo->stChRecord[ch];
		fragement = pstChRecord->fragement;
		if (fragement == 0)
			continue;

		for (i = 0; i < fragement; i++)
		{			
			pstQuery = &pstChRecord->pstRecordHead[i];
			if (((pstQuery->begin_time <= start)&&(pstQuery->end_time >= start))||
				((pstQuery->begin_time <= end)&&(pstQuery->end_time >= end)))
				return 1;
		}
	}
	return 0;
}


//查找游标应该播放的时间
//点击回放游标时，迅速定位回放录像位置

