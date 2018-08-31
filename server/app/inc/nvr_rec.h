
#ifndef __NVR_REC_H__
#define __NVR_REC_H__

int nvr_rec_alarm(int chn, int happen);

int nvr_rec_manual(int chn, int start);

int nvr_rec_update(int chn);

int nvr_rec_init();

void nvr_rec_uninit();


#endif

