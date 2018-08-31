
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <pthread.h>


#include <libcedarv.h>
#include<dec_inc/cedarv_osal_linux.h>


#define PRINT(fmt, args...)	fprintf(stderr, fmt, ##args)

volatile int quit = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

cedarv_decoder_t* decoder_open(int video_width, int video_height)
{
	int status = 0;
	cedarv_decoder_t *decoder;
	cedarv_stream_info_t stream_info;	
	
	decoder = cedarvDecInit(&status);
	if(status < 0)
	{
		PRINT("cedarvDecInit failed.\n");
		return NULL;
	}
	
	memset(&stream_info, 0, sizeof(stream_info));
	stream_info.format			 = CEDARV_STREAM_FORMAT_H264;//STREAM_FORMAT_H264;
	stream_info.sub_format		 = CEDARV_SUB_FORMAT_UNKNOW;
	stream_info.container_format =CONTAINER_FORMAT_UNKNOW;
	stream_info.video_width 	 = video_width;
	stream_info.video_height	 = video_height;
	stream_info.frame_rate		 = 25000;
	stream_info.frame_duration	 =0;
	stream_info.aspect_ratio	 = stream_info.video_width/stream_info.video_height*1000;//?????
	stream_info.init_data_len	 = 0;
	stream_info.init_data		 = 0;
	stream_info.is_pts_correct	= 0;


	status = decoder->set_vstream_info(decoder, &stream_info);
	if(status < 0)
	{
		PRINT("set vstream info failed.\n");
		cedarvDecExit(decoder);
		return NULL;
	}

	status = decoder->open(decoder);
	if(status < 0)
	{
		PRINT("open decoder failed.\n");
		cedarvDecExit(decoder);
		return NULL;
	}
	
	status = decoder->ioctrl(decoder, CEDARV_COMMAND_PLAY, 0);
	if(status < 0)
	{
		PRINT("ioctrl decoder failed.\n");
		cedarvDecExit(decoder);
		return NULL;
	}
	return decoder;
}

void decoder_close(cedarv_decoder_t *decoder)
{
	decoder->ioctrl(decoder, CEDARV_COMMAND_STOP, 0);
	decoder->close(decoder);
	cedarvDecExit(decoder);
}

#define DL()			
//(pthread_mutex_lock(&mutex))
#define DNL()		
//(pthread_mutex_unlock(&mutex))

int decoder_pro(const char *file)
{
	FILE *fp;
	u32 bufsize0,bufsize1;
	u8 *buf0,*buf1;
	int frame_len;
	cedarv_decoder_t *decoder;
	cedarv_stream_data_info_t data_info;
	cedarv_picture_t picture;
	unsigned long long pre_pts = 0, cur_pts;
	struct timeval tv;
	int frame_cnt = 1;
	
	fp = fopen(file, "rb+");
	if(fp == NULL)
	{
		PRINT("Failed to open file %s\n", file);
		return -1;
	}
	DL();
	decoder = decoder_open(640, 352);
	DNL();
	if(decoder < 0)
	{
		fclose(fp);
		return -1;
	}
	gettimeofday(&tv, NULL);
	pre_pts = (unsigned long long)tv.tv_sec * 1000000 + tv.tv_usec;
	while(!quit || feof(fp))
	{
		if(fread(&frame_len, sizeof(frame_len), 1, fp) != 1)
		{
			PRINT("Read video file %s frame length failed.\n", file);
			break;
		}
		DL();
		if(decoder->request_write(decoder,frame_len, &buf0, &bufsize0, &buf1, &bufsize1) < 0)
		{
			PRINT("decode request write failed.\n");
			DNL();
			break;
		}
		DNL();
		if(bufsize0 >= frame_len)
		{
			fread(buf0, frame_len, 1, fp);
			cedarx_cache_op((long)buf0,(long)(buf0+frame_len),1);
		}
		else
		{
			fread(buf0, bufsize0, 1, fp);
			cedarx_cache_op((long)buf0,(long)(buf0+bufsize0),1);
			fread(buf1, frame_len-bufsize0, 1, fp);
			cedarx_cache_op((long)buf1,(long)(buf1+bufsize1),1);
		}
		data_info.flags = CEDARV_FLAG_FIRST_PART | CEDARV_FLAG_LAST_PART;
		data_info.length = frame_len;
		data_info.pts = 0;
		data_info.type = 0;
		DL();
		if(decoder->update_data(decoder, &data_info) < 0)
		{
			DNL();
			PRINT("deocer update data failed.\n");
			break;
		}
		if(decoder->decode(decoder) < 0)
		{
			DNL();
			PRINT("decode failed.\n");
			break;
		}
		while(decoder->picture_ready(decoder))
		{
			// request display frame buffer.
			memset(&picture, 0, sizeof(picture));
			if(decoder->display_request(decoder, &picture) < 0)
			{
				PRINT("decode request display frame buffer failed.\n");
				break;
			}
			// display the frame buffer
			//gettimeofday(&tv, NULL);
			//cur_pts = tv.tv_sec * 1000000 + tv.tv_usec;
			//PRINT("th%d %llu\n", (int)pthread_self(), cur_pts - pre_pts);
			//pre_pts = cur_pts;
			frame_cnt++;
			decoder->display_release(decoder, picture.id);
		}
		DNL();
	}
	gettimeofday(&tv, NULL);
	cur_pts = (unsigned long long)tv.tv_sec * 1000000 + tv.tv_usec;
	PRINT("th%d frame cnt: %d, average ms %llu\n", (int)pthread_self(), frame_cnt, (cur_pts - pre_pts)/frame_cnt);
	DL();
	decoder_close(decoder);
	DNL();
	fclose(fp);
	
	return 0;
}

void *decoder_thread(void *data)
{
	const char *filepath = (char *)data;
	while(!quit) 
	{
		if(decoder_pro(filepath) < 0)
			break;
	}
	return NULL;
}

void decoder_signal(int signo)
{
	quit = 1;
}

#define DECODE_THREAD_NUM	8
int main(int argc, char *argv[])
{
	int i;
	pthread_t thid[8];
	signal(SIGINT, decoder_signal);

	if(argc != 2)
	{
		PRINT("usage: %s filepath.\n", argv[0]);
		return 0;
	}
	cedarx_hardware_init(0);
	
	for(i = 0; i<DECODE_THREAD_NUM; i++)
	{
		pthread_create(&thid[i], NULL, decoder_thread, argv[1]);
	}
	
	for(i = 0; i<DECODE_THREAD_NUM; i++)
		pthread_join(thid[i], NULL);
		
	cedarx_hardware_exit(0);
	
	return 0;
}


