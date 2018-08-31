#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<player.h>
#include<string.h>
#include<player_interface.h>
#define PLAYERLIB_PF(fmt,args...) \
do\
{\
	if(1)\
	{\
		syslog(LOG_ERR,"(%s|%s|%d):" fmt,__FILE__, __func__, __LINE__,##args);\
	}\
	else\
	{\
		fprintf(stderr, "player_ser ERROR(%s|%s|%d): ", __FILE__, __func__, __LINE__);\
		fprintf(stderr, fmt"\r\n",##args); \
	}\
}\
while(0);

static int msg_id;
char player_send_msg(_pro_msg *player_msgbuf);
char player_get_msg(unsigned char ch,void *infor);
char player_emergency_exit(void)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_RESET;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_ALL_EXIT;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_real_play(unsigned char ch)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_PLAY;
	player_msgbuf.pro_data.channel=ch;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_stop(unsigned char ch)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_STOP;
	player_msgbuf.pro_data.channel=ch;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_replay_pause_clt(unsigned char ch,unsigned char ctl)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_PAUSE;
	player_msgbuf.pro_data.channel=ch;
	player_msgbuf.pro_data.pause=ctl;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_exit(void)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_ALL_EXIT;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_get_player_status(unsigned char ch,player_status *status)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_GET_PLAYER_STATUS;
	player_msgbuf.pro_data.channel=ch;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	if(player_get_msg(ch,status))
	{
		return -1;
	}
	return 0;
}
char player_set_dis_mode(unsigned char mode,unsigned char ch)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_SINGAL_DISPLAY;
	player_msgbuf.pro_data.single_en=mode;
	player_msgbuf.pro_data.channel=ch;
	if(mode&&(ch>=STREAM_CHANNEL_MAX))
	{
		PLAYERLIB_PF("single mode error,channel is:%d",ch);
		return -1;
	}
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_set_speed(unsigned char ch,unsigned char speed)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_SET_RATE;
	player_msgbuf.pro_data.channel=ch;
	player_msgbuf.pro_data.rate=speed;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_set_replay_mode(unsigned char ch)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_RECORD_PLAY;
	player_msgbuf.pro_data.channel=ch;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_set_real_play_mode(unsigned char ch)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_REAL_PLAY;
	player_msgbuf.pro_data.channel=ch;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_up_frame(unsigned char ch)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_UP_FRAME;
	player_msgbuf.pro_data.channel=ch;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_down_frame(unsigned char ch)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_DOWN_FRAME;
	player_msgbuf.pro_data.channel=ch;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_get_display_mode(void)
{
	_pro_msg player_msgbuf;	
	unsigned char mode;
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_GET_DISPLAY_MODE;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}	
	if(player_get_msg(0,&mode))
	{
		return -1;
	}
	return mode;
}
char player_set_audio_function(unsigned char ch,unsigned char cmd)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_SET_AUDIO;
	player_msgbuf.pro_data.channel=ch;
	player_msgbuf.pro_data.audio_ctl=cmd;	
	if(ch>=STREAM_CHANNEL_MAX)
	{
		PLAYERLIB_PF("set audio fun error,channel is:%d",ch);
		return -1;
	}
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_set_cap_num(unsigned char ch,unsigned char num)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_CAPTURE;
	player_msgbuf.pro_data.channel=ch;
	player_msgbuf.pro_data.cap_num=num;
	if(ch>=STREAM_CHANNEL_MAX)
	{
		PLAYERLIB_PF("set capture num error,channel is:%d",ch);
		return -1;
	}
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;	
}
char player_set_fb_alpha(unsigned char alpha)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_SET_FB_ALPHA;
	player_msgbuf.pro_data.alpha=alpha;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;	
}
char player_set_colorky(unsigned char r,unsigned char g,unsigned char b)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_SET_COLORKEY;
	player_msgbuf.pro_data.r=r;
	player_msgbuf.pro_data.g=g;
	player_msgbuf.pro_data.b=b;	
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;	
}	
char player_stop_alarm_audio(void)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_ALARM_AUDIO;
	player_msgbuf.pro_data.alarm_play=0;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_start_audio_record(void)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_START_AUDIO_RECORD;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_stop_audio_record(void)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_STOP_AUDIO_RECORD;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_set_record_par(AV_AUDIO_INFO_S *par)
{	
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_SET_AUDIO_REC_PAR;
	memcpy(&player_msgbuf.pro_data.audio_enc_par,par,sizeof(AV_AUDIO_INFO_S));
	PLAYERLIB_PF("sample:%d,encode type:%d,ch:%d,bit:%d",par->u8AudioSamples,par->u16EncodeType,par->u8AudioChannels,par->u8AudioBits);
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_alarm_audio(char *file)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_ALARM_AUDIO;
	if(strlen(file)<sizeof(player_msgbuf.pro_data.alarm_file))
	{
		memcpy(player_msgbuf.pro_data.alarm_file,file,strlen(file)+1);		
		player_msgbuf.pro_data.alarm_play=1;
		if(player_send_msg(&player_msgbuf))
		{
			return -1;
		}
		return 0;
	}
	return -1;
}
char player_stop_phone_audio(void)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_STOP_PHONE_AUDIO;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}

char player_start_phone_audio(void)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_START_PHONE_AUDIO;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;
}
char player_set_src_rsz(unsigned short x,unsigned short y,unsigned short w,unsigned short h)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_SET_SRC_RECT;
	player_msgbuf.pro_data.rect.x=x;
	player_msgbuf.pro_data.rect.y=y;
	player_msgbuf.pro_data.rect.width=w;
	player_msgbuf.pro_data.rect.height=h;
	if((x>=1280)||(y>=720)||(w>1280)||(h>720))
	{
		PLAYERLIB_PF("set source rect error,x:%d,y:%d,w:%d,h:%d",x,y,w,h);
		return -1;
	}
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;	
}
char player_set_scn_rsz(unsigned short x,unsigned short y,unsigned short w,unsigned short h)
{
	_pro_msg player_msgbuf;	
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_SET_SCN_RECT;
	player_msgbuf.pro_data.rect.x=x;
	player_msgbuf.pro_data.rect.y=y;
	player_msgbuf.pro_data.rect.width=w;
	player_msgbuf.pro_data.rect.height=h;
	if((x>=1280)||(y>=720)||(w>1280)||(h>720))
	{
		PLAYERLIB_PF("set screen rect error,x:%d,y:%d,w:%d,h:%d",x,y,w,h);
		return -1;
	}	
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}
	return 0;	
}

char player_get_preview_status(unsigned char ch)
{
	_pro_msg player_msgbuf;
	unsigned char status=0;	
	player_msgbuf.pro_data.channel=ch;
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_GET_PREVIEW_STATUS;
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}	
	if(player_get_msg(ch,&status))
	{
		return -1;
	}
	return status;
}
char player_replay_play(unsigned char ch,file_info *replay_info,unsigned char flag)
{
	_pro_msg player_msgbuf;	
	if(NULL==replay_info)
	{
		PLAYERLIB_PF("query is null!!!!!");
		return -1;
	}
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_CHOOSE_FILE;
	player_msgbuf.pro_data.channel=ch;
	player_msgbuf.pro_data.update_now=flag;	
	if(ch>=STREAM_CHANNEL_MAX)
	{
		memcpy(&player_msgbuf.pro_data.replay_infor,replay_info,sizeof(file_info)*STREAM_CHANNEL_MAX);
	}
	else
	{
		memcpy(&player_msgbuf.pro_data.replay_infor[ch],replay_info,sizeof(file_info));		
	}
	if(player_send_msg(&player_msgbuf))
	{
		return -1;
	}	
	return 0;
}

void player_msg_init(void)
{
	key_t keyval;
	keyval=ftok(".",PLAYER_PRO_IPC_KEY_ID);//获取一个消息队列的键值
	msg_id=msgget(keyval,IPC_CREAT|0666);//创建一个消息队列,并且设置读写权限。如果消息队列已经存在则打开
	PLAYERLIB_PF("msg queue creat success id:%d",msg_id);
}
char player_send_msg(_pro_msg *player_msgbuf)
{	
	if(msgsnd(msg_id,player_msgbuf,sizeof(player_msgbuf->pro_data),0))//0表示消息队列满的时候，就阻塞
	{
		PLAYERLIB_PF("send msg fail!!!,cmd:%d\r\n",player_msgbuf->pro_data.cmd);
		return -1;
	}	
	return 0; 
}
char player_get_msg(unsigned char ch,void *infor)
{		
	_pro_msg player_msgbuf;
	if((msgrcv(msg_id,&player_msgbuf,sizeof(player_msgbuf.pro_data),PLAYER_MSG_TYPE_SEND,0))<0)
	{
		PLAYERLIB_PF("player_interface rcv msg fail!!! strerror: %s", strerror(errno)); //转换错误码为对应的错误信息
//		PLAYERLIB_PF("player_interface rcv msg fail!!!\r\n");
		return -1;
	}	
	else
	{
		switch(player_msgbuf.pro_data.cmd)
		{
			case PLAYER_CMD_RET_STATUS:
				if(ch>=STREAM_CHANNEL_MAX)
				{
					memcpy(infor,player_msgbuf.pro_data.run_status,sizeof(player_status)*STREAM_CHANNEL_MAX);
				}
				else
				{
					memcpy(infor,&player_msgbuf.pro_data.run_status[ch],sizeof(player_status));
				}
				break;
			case PLAYER_CMD_RET_DISPLAY_MODE:
				*((u8*)infor)=player_msgbuf.pro_data.single_en;
				break;
			case PLAYER_CMD_RET_PREVIEW_STATUS:
				*((u8*)infor)=player_msgbuf.pro_data.preview_status;					
				PLAYERLIB_PF("player_interface rcv get stream preview:%d",*((u8*)infor));
				break;
			default:;
		}
	}
	return 0; 
}

