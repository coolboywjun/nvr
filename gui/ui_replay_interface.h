#ifndef __UI_REPLAY_INTERFACE_H__
#define __UI_REPLAY_INTERFACE_H__

typedef enum
{
	EN_SLOW_RATE8 = 0,
	EN_SLOW_RATE4 = 1,
	EN_SLOW_RATE2 = 2,
	EN_NORMAL_RATE = 3,
	EN_SPEED_RATE2 = 4,
	EN_SPEED_RATE4 = 5,
	EN_SPEED_RATE8 = 6
}EN_PLAY_SPEED;

typedef enum
{
	EN_PLAY_FORWARD = 0,
	EN_PLAY_BACKWORD = 1,
}EN_PLAY_DIRICTION;


int ui_replay_start(time_t playTime, RecordInfo *pstRecord, int displaymode, int select_ch);

int ui_replay_get_time();
int ui_replay_set_mode(int displaymode, int select_ch,int sta);
int ui_replay_callback(void);
int ui_replay_stop(void);
int ui_replay_get_time(void);
int ui_replay_playforward(void);
int ui_replay_playback(void);

int ui_replay_clear_stream(EN_PLAY_SPEED speed);

int ui_set_speed_dir(int speed, int dir);


#endif
