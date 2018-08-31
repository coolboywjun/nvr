enum{
	AUDIO_RECORD_START_CMD=1,
	AUDIO_RECORD_STOP_CMD,
	AUDIO_RECORD_EXIT_CMD,
};
void audio_record_set_cmd(u8 cmd);
void audio_record_thr_exit(void);
void audio_record_set_cmd(u8 cmd);
int audio_record_thr_create(void *param);
void audio_record_set_rec_par(AV_AUDIO_INFO_S*par);
int audio_record_thr_delete(void);
u32 audio_get_record_cnt(void);
