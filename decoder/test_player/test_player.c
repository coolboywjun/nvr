#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<storage_interface.h>
#include<string.h>
#include<player_interface.h>
#include<audio_params.h>

int storage_lib_init();
int storage_lib_uninit();
int player_storage(int ch,RecordQueryInfo*query,int index);
int player_storage_test_record_query(int chn,RecordQueryInfo*query,int index);
int storage_lib_record_query(int chn_mask, time_t begin_time, time_t end_time, 
							StorageRecordType	 record_type, int *shmid, 
							RecordQueryChnInfo **chn_info, RecordQueryInfo **query_info);
void storage_lib_record_query_free(int shmid, RecordQueryChnInfo *chn_info, RecordQueryInfo *query_info);

int main(int argc, char **argv)
{	
//	unsigned char*buf;
//	FILE *audio_fd;
	player_status channel_status[4];
	AV_AUDIO_INFO_S audio_enc_par={.u8AudioSamples=0,.u16EncodeType=2,.u8AudioChannels=1,.u8AudioBits=16};
	player_msg_init();	
	switch(atoi(argv[1]))
	{
		case 0:
			if(player_emergency_exit())
			{
				printf("player emergency exit fail\r\n");
			}
			break;
		case 1:
			if(player_real_play(atoi(argv[2])))
			{
				printf("set player play fail\r\n");
			}
			break;
		case 2:
			if(player_stop(atoi(argv[2])))
			{
				printf("set player stop fail\r\n");
			}
			break;
		case 3:
			if(player_replay_pause_clt(atoi(argv[2]),atoi(argv[3])))
			{
				printf("set player replay pause ctl fail\r\n");
			}
			break;
		case 4:
			if(player_exit())
			{
				printf("set player exit fail\r\n");
			}
			break;
		case 5:
			if(player_get_player_status(atoi(argv[2]),&channel_status[atoi(argv[2])]))
			{
				printf("get player status fail\r\n");
			}
			break;
		case 6:
			if(player_set_dis_mode(atoi(argv[2]),atoi(argv[3])))
			{
				printf("set player single display fail\r\n");
			}
			break;
		case 7:
			if(player_set_speed(atoi(argv[2]),atoi(argv[3])))
			{
				printf("set player set speed fail\r\n");
			}
			break;
		case 8:
			if(player_set_real_play_mode(atoi(argv[2])))
			{
				printf("set player set real play fail\r\n");
			}
			break;
		case 9:
			if(player_set_replay_mode(atoi(argv[2])))
			{
				printf("set player set record play fail\r\n");
			}
			break;
		case 11:
			if(player_down_frame(atoi(argv[2])))
			{
				printf("set player query play time fail\r\n");
			}
			break;
		case 12:
			if(player_up_frame(atoi(argv[2])))
			{
				printf("set player query play time fail\r\n");
			}
			break;
		case 15:
			if((-1==player_get_display_mode()))
			{
				printf("set player_get_display_mode fail\r\n");
			}
			break;
		case 16:			
			if((-1==player_get_preview_status(atoi(argv[2]))))
			{
				printf("set player_get_preview_status fail\r\n");
			}
			break;
		case 17:
			if((-1==player_set_audio_function(atoi(argv[2]),atoi(argv[3]))))
			{
				printf("set player_set_audio_function fail\r\n");
			}
			break;
		case 19:
			if((-1==player_alarm_audio(argv[2])))
			{
				printf("set player_alarm_audio fail\r\n");
			}
			break;
		case 20:
			if((-1==player_set_cap_num(atoi(argv[2]),atoi(argv[3]))))
			{
				printf("set player_set_cap_num fail\r\n");
			}
			break;
		case 21:
			if((-1==player_set_fb_alpha(atoi(argv[2]))))
			{
				printf("set player_set_fb_alpha fail\r\n");
			}
			break;
		case 22:
			if((-1==player_set_colorky(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]))))
			{
				printf("set player_set_colorky fail\r\n");
			}		
			break;	
		case 23:
			if((-1==player_start_phone_audio()))
			{
				printf("set player_phone_audio fail\r\n");
			}
			/*if(NULL==(buf=(unsigned char*)malloc(DATA_SIZE)))
			{
				printf("malloc buffer fail\r\n");
			}
			//head.data_buf=buf;
			if(NULL==(audio_fd=fopen("audio_g711u","rb")))
			{
				printf("open alarm file error\r\n");
				free(buf);
				return -1;
			}
			while(1)
			{
				static int cnt=0;
				cnt++;
				if(!audio_read_phone_file(audio_fd,&head,buf))
				{
					if((-1==player_start_phone_audio()))
					{
						printf("set player_phone_audio fail\r\n");
					}
					//printf("write cnt:%d\r\n",cnt);
				}
				else
				{					
					sleep(10);//?????
					printf("audio read phone data end\r\n");
					if((-1==player_stop_phone_audio()))
					{
						printf("set player_phone_audio fail\r\n");
					}
					break;
				}				
				usleep(5);
			}
			free(buf);*/
			break;
		case 24:
			if((-1==player_stop_alarm_audio()))
			{
				printf("set player_stop_alarm_audio fail\r\n");
			}
			break;
		case 25:
			if((-1==player_start_audio_record()))
			{
				printf("set player_start_audio_record fail\r\n");
			}
			break;
		case 26:
			if((-1==player_stop_audio_record()))
			{
				printf("set player_stop_audio_record fail\r\n");
			}
			break;			
		case 27:
			if((-1==player_set_record_par(&audio_enc_par)))
			{
				printf("set player_set_record_par fail\r\n");
			}
			break;
		case 28:
			if((-1==player_set_src_rsz(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]))))
			{
				printf("set player_set_src_rsz fail\r\n");
			}
			break;
		case 29:
			if((-1==player_set_scn_rsz(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]))))
			{
				printf("set player_set_scn_rsz fail\r\n");
			}
			break;
		case 30:
			if((-1==player_stop_phone_audio()))
			{
				printf("set player_stop_phone_audio fail\r\n");
			}
			break;
		default:;		
	}
	return 0;
}

int player_storage(int ch,RecordQueryInfo*query,int index)
{
	if(storage_lib_init() < 0)
	{
		printf("Failed to initialize storage lib\n");
		return -1;
	}
	if(player_storage_test_record_query(ch,query,index))
	{
		printf("Failed to query storage lib\n");
		return -1;
	}
	storage_lib_uninit();
	return 0;
}
int player_storage_test_record_query(int chn,RecordQueryInfo*query,int index)
{
	int i, ret;
	struct tm time_str;
	time_t begin_time, end_time;
	RecordQueryChnInfo *chn_info;
	RecordQueryInfo *query_info;
	int shmid;
	time_str.tm_year = 2000 - 1900;
	time_str.tm_mon = 1 - 1;
	time_str.tm_mday = 1;
	time_str.tm_hour = 0;
	time_str.tm_min = 0;
	time_str.tm_sec = 0;
	time_str.tm_isdst = -1;
	if(mktime(&time_str) == -1)
	{
		printf("make begin time failed.\n");
		return -1;
	}
	begin_time = mktime(&time_str);

	// end time : 2010-1-1 00:00:00
	time_str.tm_year = 2010 - 1900;
	time_str.tm_mon = 1 - 1;
	time_str.tm_mday = 1;
	time_str.tm_hour = 0;
	time_str.tm_min = 0;
	time_str.tm_sec = 0;
	time_str.tm_isdst = -1;
	if(mktime(&time_str) == -1)
	{
		printf("make begin time failed.\n");
		return -1;
	}
	end_time = mktime(&time_str);

	printf("query, chn%d, stream%d\n", chn, 0);
	ret = storage_lib_record_query(1<<chn,
							 	begin_time, end_time,RECORD_ALL_TYPE,
							 	&shmid, &chn_info, &query_info);
	if(ret < 0)
	{
		printf("Failed to query record\n");
		return -1;
	}
	printf("chn num: %d\n", chn_info->max_chn);
	printf("fragment num: %d\n", ret);
	for(i = 0; i < chn_info->max_chn; i++)
	{
		printf("chn%d fragment num: %d\n", i, chn_info->query_info_num[i]);
	}
	if(ret == 0)
		return 0;
	for(i = 0; i < ret; i++)
	{
		printf("chn%d disk_idx: %d basket_idx: %d bf_idx: %d begin_iframe_idx: %d iframe_num: %d btime: %ld etime: %ld\n", 
			query_info[i].chn,
			query_info[i].disk_idx, query_info[i].basket_idx,
			query_info[i].bf_idx, query_info[i].begin_iframe_idx,
			query_info[i].iframe_num, query_info[i].begin_time, 
			query_info[i].end_time);
	}
	memcpy(query, &query_info[index], sizeof(RecordQueryInfo));
	storage_lib_record_query_free(shmid, chn_info, query_info);
	return 0;
}

