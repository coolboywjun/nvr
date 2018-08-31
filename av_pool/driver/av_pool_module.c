#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/net.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
//#include <asm/arch/gpio.h>
//#include <asm/arch/io.h>
#include "av_pool_module.h"
#include "av_pool_mng.h"
#include "mem_mng.h"

#define AV_POOL_MINOR 	241

static atomic_t gOpenCnt = ATOMIC_INIT(0);

static struct proc_dir_entry *pool_dir;
static atomic_t gAlarmStatus[AV_POOL_MAX_SUPPORT_CHN];
static wait_queue_head_t gAlarmWait;
static DEFINE_MUTEX(gAlarmMutex);

typedef struct
{
	atomic_t readCnt;
	atomic_t writeCnt;
}AvFileStatus;

static int av_pool_open(struct inode *inode, struct file *file)
{
	AvFileStatus *poolStatus;
	printk(KERN_INFO "Av pool open count: %d\n", atomic_inc_return(&gOpenCnt));
	poolStatus = kmalloc(GFP_KERNEL, sizeof(AvFileStatus) * AV_POOL_MAX_SUPPORT_CHN * AV_POOL_MAX_SUPPORT_STREAM);
	if(poolStatus == NULL)
		return -ENOMEM;
	memset(poolStatus, 0, sizeof(AvFileStatus) * AV_POOL_MAX_SUPPORT_CHN * AV_POOL_MAX_SUPPORT_STREAM);
	file->private_data = poolStatus;
	return 0;
}

static int 
av_pool_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Av pool open count: %d\n", atomic_dec_return(&gOpenCnt));
	if(file->private_data != NULL)
	{
		int i, j, m;
		int poolNo;
		AvFileStatus *poolStatus = (AvFileStatus *)file->private_data;
		for(i = 0; i < AV_POOL_MAX_SUPPORT_CHN; i++)
		{
			for(j = 0; j < AV_POOL_MAX_SUPPORT_STREAM; j++)
			{
				poolNo = i * AV_POOL_MAX_SUPPORT_STREAM + j;
				//printk(KERN_ERR "pool %d write open cnt: %d\n", poolNo, atomic_read(&poolStatus[poolNo].writeCnt));
				//printk(KERN_ERR "pool %d read open cnt: %d\n", poolNo, atomic_read(&poolStatus[poolNo].readCnt));
				for(m = 0; m < atomic_read(&poolStatus[poolNo].writeCnt); m++)
					av_pool_dec_pool_writing_cnt(i, j);
				for(m = 0; m < atomic_read(&poolStatus[poolNo].readCnt); m++)
					av_pool_dec_pool_reading_cnt(i, j);
			}
		}
		kfree(file->private_data);
		file->private_data = NULL;
	}
	return 0;
}

static int
av_pool_write_frame_data(AvWriteCmd __iomem *writeCmd)
{
	char *picsData[1];
	int picsLen[1];
	AvWriteCmd cmd;
	
	if(copy_from_user(&cmd, writeCmd, sizeof(AvWriteCmd)))
	{
		printk(KERN_ERR "Failed to copy write command from user.\n");
		return -EFAULT;
	}
	if(cmd.chn > AV_POOL_MAX_SUPPORT_CHN || cmd.streamNo > AV_POOL_MAX_SUPPORT_STREAM)
	{
		printk(KERN_ERR "The input channel or stream is wrong.\n");
		return -ERANGE;
	}
	
	picsData[0] = cmd.frameBuf;
	picsLen[0] = cmd.frameLen;
	av_pool_write_frame(cmd.chn,
							cmd.streamNo,
							cmd.hdrData,
							cmd.hdrLen,
							1,
							picsData,
							picsLen,
							cmd.frameLen,
							cmd.frameType,
							cmd.pts,
							cmd.frameNo);
	return 0;
}

static int
av_pool_read_frame_data(AvReadCmd __iomem *readCmd)
{
	AvReadCmd cmd;
	
	if(copy_from_user(&cmd, readCmd, sizeof(AvReadCmd)))
	{
		printk(KERN_ERR "Failed to copy read command from user.\n");
		return -EFAULT;
	}
	if(cmd.chn >= AV_POOL_MAX_SUPPORT_CHN || cmd.streamNo >= AV_POOL_MAX_SUPPORT_STREAM)
	{
		printk(KERN_ERR "The input channel or stream is wrong.\n");
		return -ERANGE;
	}
	cmd.frameLen = av_pool_read_frame(cmd.chn,
							cmd.streamNo,
							cmd.frameBuf,
							cmd.bufLen,
							&cmd.needIFrame,
							cmd.needFrameNo,
							&cmd.frameNo,
							&cmd.lastPos,
							&cmd.pts,
							cmd.maxDelay,
							&cmd.newestFrameNo,
							cmd.timeout);
	if(copy_to_user(readCmd, &cmd, sizeof(AvReadCmd)))
	{
		printk(KERN_ERR "Failed to copy read command to user.\n");
		return -EFAULT;
	}
	return cmd.frameLen;
}

static int
av_pool_max_frame_size(GetMaxFrameSize __iomem *maxFrameSizeCmd)
{
	GetMaxFrameSize cmd;

	if(copy_from_user(&cmd, maxFrameSizeCmd, sizeof(GetMaxFrameSize)))
	{
		printk(KERN_ERR "Failed to copy read command from user.\n");
		return -EFAULT;
	}
	
	if(cmd.chn > AV_POOL_MAX_SUPPORT_CHN || cmd.streamNo > AV_POOL_MAX_SUPPORT_STREAM)
	{
		printk(KERN_ERR "The input channel or stream is wrong.\n");
		return -ERANGE;
	}
	cmd.maxFrameSize = av_pool_get_max_frame_size(cmd.chn, cmd.streamNo);

	if(copy_to_user(maxFrameSizeCmd, &cmd, sizeof(GetMaxFrameSize)))
	{
		printk(KERN_ERR "Failed to copy read command to user.\n");
		return -EFAULT;
	}
	
	return (cmd.maxFrameSize >= 0) ? 0 : -EINVAL;
}

static long
av_pool_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	AvFileStatus *poolStatus = (AvFileStatus *)file->private_data;
	switch(cmd)
	{
		case AV_POOL_WRITE_FRAME:
			ret = av_pool_write_frame_data((AvWriteCmd __iomem*)arg);
			break;
		case AV_POOL_READ_FRAME:
			ret = av_pool_read_frame_data((AvReadCmd __iomem*)arg);
			break;
		case AV_POOL_GET_MAX_FRAME_SIZE:
			ret = av_pool_max_frame_size((GetMaxFrameSize __iomem *)arg);
			break;
		case AV_POOL_RESET_POOL:
			{
				AvPoolId cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				ret = av_pool_reset_pool(cmd.chn, cmd.streamNo);
			}
			break;
		case AV_POOL_SET_POOL_INFO:
			{
				AvPoolInfo cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				ret = av_pool_set_pool_info(cmd.chn, cmd.ipAddr, cmd.protocol,
										cmd.type, cmd.port);
			}
			break;
		case AV_POOL_GET_POOL_INFO:
			{
				AvPoolInfo cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				ret = av_pool_get_pool_info(cmd.chn, cmd.ipAddr, cmd.protocol,
										&cmd.type, &cmd.port);
				if(copy_to_user((void*)arg, &cmd, sizeof(cmd)))
					return -EFAULT;
			}
			break;
		case AV_POOL_SET_POOL_STATUS:
			{
				AvPoolStaus cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				ret = av_pool_set_pool_status(cmd.chn, cmd.streamNo, cmd.status);
			}
			break;
		case AV_POOL_GET_POOL_STATUS:
			{
				AvPoolStaus cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				ret = av_pool_get_pool_status(cmd.chn, cmd.streamNo, &cmd.status);
				if(copy_to_user((void*)arg, &cmd, sizeof(cmd)))
					return -EFAULT;
			}
			break;
		case AV_POOL_GET_FRAME_INFO:
			{
				AvPoolFrameInfo cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				ret = av_pool_get_frame_info(cmd.chn, cmd.streamNo, &cmd.writeFrameCnt, &cmd.readFrameCnt);
				if(copy_to_user((void*)arg, &cmd, sizeof(cmd)))
					return -EFAULT;
			}
			break;
		case AV_POOL_INC_READING_STATUS:
			{
				AvPoolId cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				ret = av_pool_inc_pool_reading_cnt(cmd.chn, cmd.streamNo);
				if(ret == 0)
					atomic_inc(&poolStatus[cmd.chn * AV_POOL_MAX_SUPPORT_STREAM + cmd.streamNo].readCnt);
			}
			break;
		case AV_POOL_DEC_READING_STATUS:
			{
				AvPoolId cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				ret = av_pool_dec_pool_reading_cnt(cmd.chn, cmd.streamNo);
				if(ret == 0)
					atomic_dec(&poolStatus[cmd.chn * AV_POOL_MAX_SUPPORT_STREAM + cmd.streamNo].readCnt);
			}
			break;
		case AV_POOL_GET_READING_STATUS:
			{
				AvPoolCnt cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				cmd.cnt = av_pool_get_pool_reading_cnt(cmd.chn, cmd.streamNo);
				ret = cmd.cnt;
				if(copy_to_user((void *)arg, &cmd, sizeof(cmd)))
					return -EFAULT;
			}
			break;
		case AV_POOL_INC_WRITING_STATUS:
			{
				AvPoolId cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				ret = av_pool_inc_pool_writing_cnt(cmd.chn, cmd.streamNo);
				if(ret == 0)
					atomic_inc(&poolStatus[cmd.chn * AV_POOL_MAX_SUPPORT_STREAM + cmd.streamNo].writeCnt);
			}
			break;
		case AV_POOL_DEC_WRITING_STATUS:
			{
				AvPoolId cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				ret = av_pool_dec_pool_writing_cnt(cmd.chn, cmd.streamNo);
				if(ret == 0)
					atomic_dec(&poolStatus[cmd.chn * AV_POOL_MAX_SUPPORT_STREAM + cmd.streamNo].writeCnt);
			}
			break;
		case AV_POOL_GET_WRITING_STATUS:
			{
				AvPoolCnt cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				cmd.cnt = av_pool_get_pool_writing_cnt(cmd.chn, cmd.streamNo);
				ret =cmd.cnt;
				if(copy_to_user((void *)arg, &cmd, sizeof(cmd)))
					return -EFAULT;
			}
			break;
		case AV_POOL_SET_IPNC_HANDLE:
			{
				AvPoolHandle cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				return av_pool_set_pool_ipnc_handle(cmd.chn, cmd.handle);
			}
			break;
		case AV_POOL_GET_IPNC_HANDLE:
			{
				AvPoolHandle cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				ret = av_pool_get_pool_ipnc_handle(cmd.chn, &cmd.handle);
				if(copy_to_user((void *)arg, &cmd, sizeof(cmd)))
					return -EFAULT;
			}
			break;
		case AV_POOL_SET_STREAM_HANDLE:
			{
				AvPoolHandle cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				return av_pool_set_pool_stream_handle(cmd.chn, cmd.streamNo, cmd.handle);
			}
			break;
		case AV_POOL_GET_STREAM_HANDLE:
			{
				AvPoolHandle cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				av_pool_get_pool_stream_handle(cmd.chn, cmd.streamNo, &cmd.handle);
				if(copy_to_user((void *)arg, &cmd, sizeof(cmd)))
					return -EFAULT;
			}
			break;
		case AV_POOL_SET_ALARM_STATUS:
			{
				AvPoolAlarm cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				if(cmd.chn >= AV_POOL_MAX_SUPPORT_CHN)
					return -EFAULT;
				mutex_lock(&gAlarmMutex);
				cmd.status = !!cmd.status;
				if(cmd.status != atomic_read(&gAlarmStatus[cmd.chn]))
				{
					wake_up_all(&gAlarmWait);
					atomic_set(&gAlarmStatus[cmd.chn], cmd.status);
				}
				mutex_unlock(&gAlarmMutex);
			}
			break;
		case AV_POOL_GET_ALARM_STATUS:
			{
				AvPoolAlarm cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				if(cmd.chn >= AV_POOL_MAX_SUPPORT_CHN)
					return -EFAULT;
				mutex_lock(&gAlarmMutex);
				if(av_pool_get_pool_writing_cnt(cmd.chn, 0) == 0)
					cmd.status = 0;
				else
					cmd.status = atomic_read(&gAlarmStatus[cmd.chn]);
				mutex_unlock(&gAlarmMutex);
				if(copy_to_user((void *)arg, &cmd, sizeof(cmd)))
					return -EFAULT;
			}
			break;
		case AV_POOL_RESET_ALARM_STATUS:
			{
				AvPoolAlarm cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				if(cmd.chn >= AV_POOL_MAX_SUPPORT_CHN)
					return -EFAULT;
				mutex_lock(&gAlarmMutex);
				if(atomic_read(&gAlarmStatus[cmd.chn]))
				{
					wake_up_all(&gAlarmWait);
					atomic_set(&gAlarmStatus[cmd.chn], 0);
				}
				mutex_unlock(&gAlarmMutex);
			}
			break;
		case AV_POOL_WAIT_ALARM_CHANGE:
			{
				AvPoolAlarm cmd;
				if(copy_from_user(&cmd, (void *)arg, sizeof(cmd)))
					return -EFAULT;
				if(cmd.chn >= AV_POOL_MAX_SUPPORT_CHN)
					return -EFAULT;
				mutex_lock(&gAlarmMutex);
				while(atomic_read(&gAlarmStatus[cmd.chn]) == cmd.status)
				{
					// status no changed
					mutex_unlock(&gAlarmMutex);
					interruptible_sleep_on(&gAlarmWait);
					if (signal_pending(current))
						return -EAGAIN;
					mutex_lock(&gAlarmMutex);
				}
				cmd.status = atomic_read(&gAlarmStatus[cmd.chn]);
				mutex_unlock(&gAlarmMutex);
				if(copy_to_user((void *)arg, &cmd, sizeof(cmd)))
					return -EFAULT;
			}
			break;
		default:
			printk(KERN_ERR "The cmd %x don't support!\n", cmd);
			ret = -EINVAL;
			break;
	}
	return ret;
}

static int av_pool_read_total_status(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;
	int chn;
	
	len += snprintf(page + len, (((count - len) >= 0) ? (count - len) : 0),
					"Open count is %d\n", atomic_read(&gOpenCnt));
	for(chn = 0; chn < AV_POOL_MAX_SUPPORT_CHN; chn++)
	{
		len += snprintf(page + len, (((count - len) >= 0) ? (count - len) : 0),
					"Chn%d alarm is %d\n", chn, atomic_read(&gAlarmStatus[chn]));
	}
	*eof = 1;
	return len;
}

static int av_pool_read_ch_status(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int i, j;
	int len = 0;
	char ipAddr[32];
	char protocol[32];
	int type, port;
	
	i = (int)data;
	av_pool_get_pool_info(i, ipAddr, protocol, &type, &port);
	if(strlen(ipAddr) != 0)
	{
		const char *typestr;
		long ipncHandle;
		if(type == SOCK_STREAM)
			typestr = "TCP";
		else if(type == SOCK_DGRAM)
			typestr = "UDP";
		else
			typestr = "UNKNOW";
		av_pool_get_pool_ipnc_handle(i, &ipncHandle);
		len += snprintf(page + len, (((count - len) >= 0) ? (count - len) : 0),
							"ch%d ==> protocol: %s, type: %s, ip: %s:%d, ipnc handle: 0x%08lx\n", 
							i, protocol, typestr, ipAddr, port, ipncHandle);
	}
	for(j = 0; j < AV_POOL_MAX_SUPPORT_STREAM; j++)
	{
		const char *name[] = {"Main", "Sub", "Audio"};
		const char *work;
		AvPoolWorkStatus status;
		unsigned int writeFrameCnt, readFrameCnt;
		long streamHandle;

		av_pool_get_pool_stream_handle(i, j, &streamHandle);
		av_pool_get_frame_info(i, j, &writeFrameCnt, &readFrameCnt);
		av_pool_get_pool_status(i, j, &status);
		if(status & POOL_READING && status & POOL_WRITING)
			work = "Writing and Reading";
		else if(status & POOL_READING)
			work = "Reading";
		else if(status & POOL_WRITING)
			work = "Writing";
		else
			work = "Idle";
			
		len += snprintf(page + len, (((count - len) >= 0) ? (count - len) : 0),
					"memory block ch%d name: %5s ==> startPhysAddress: 0x%08lx, startVirtAddress: 0x%08lx, len: 0x%lx\n",
					i, name[j], mem_block[i][j].startPhysAddr, mem_block[i][j].startVirtAddr, mem_block[i][j].memBlockLen);
		//len += snprintf(page + len, (((count - len) >= 0) ? (count - len) : 0),
		//			"ch%d name: %5s ==> Working status %s\n", i, name[j], work);
		len += snprintf(page + len, (((count - len) >= 0) ? (count - len) : 0),
					"ch%d name: %5s ==> Open for reading count: %d\n", i, name[j], av_pool_get_pool_reading_cnt(i, j));
		len += snprintf(page + len, (((count - len) >= 0) ? (count - len) : 0),
					"ch%d name: %5s ==> Open for writing count: %d\n", i, name[j], av_pool_get_pool_writing_cnt(i, j));
		len += snprintf(page + len, (((count - len) >= 0) ? (count - len) : 0),
							"ch%d name: %5s ==> write frame count : %u\n", i, name[j], writeFrameCnt);
		len += snprintf(page + len, (((count - len) >= 0) ? (count - len) : 0),
							"ch%d name: %5s ==> read frame count : %u\n", i, name[j], readFrameCnt);
		len += snprintf(page + len, (((count - len) >= 0) ? (count - len) : 0),
							"ch%d name: %5s ==> max frame size : %d\n", 
							i, name[j], av_pool_get_max_frame_size(i, j));
		len += snprintf(page + len, (((count - len) >= 0) ? (count - len) : 0),
							"ch%d name: %5s ==> stream handle : 0x%08lx\n\n", 
							i, name[j], streamHandle);
	}
	*eof = 1;
		
	return len;
}

static const struct file_operations av_pool_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl = av_pool_ioctl,
	.open		= av_pool_open,
	.release		= av_pool_release,
};

static struct miscdevice av_pool_miscdev = {
	.minor		= AV_POOL_MINOR,
	.name		= "av_pool",
	.fops			= &av_pool_fops,
};

static int __init av_pool_module_init(void)
{
	int res;
	
	init_waitqueue_head(&gAlarmWait);
	res = mem_mng_init();
	if(res < 0)
	{
		printk(KERN_ERR "Failed to initialize memory manager!\n");
		return res;
	}
	
	res = av_pool_init(AV_POOL_MAX_SUPPORT_CHN, AV_POOL_MAX_SUPPORT_STREAM);
	if(res < 0)
	{
		printk(KERN_ERR "Failed to intialize av pool.\n");
		res = misc_deregister(&av_pool_miscdev);
		return res;
	}
	
	res = misc_register(&av_pool_miscdev);
	if (res)
	{
		printk(KERN_ERR "Failed to register picxx device.\n");
		return res;
	}
	
	pool_dir = proc_mkdir("avpool", NULL);
	if(pool_dir)
	{
		int i;
		char name[32];
		create_proc_read_entry("status", 0, pool_dir, av_pool_read_total_status, NULL);
		for(i = 0; i < AV_POOL_MAX_SUPPORT_CHN; i++)
		{
			sprintf(name, "ch%d_status", i);
			create_proc_read_entry(name, 0, pool_dir, av_pool_read_ch_status, (void*)i);
		}
	}
	return 0;
}

static void __exit av_pool_module_exit(void)
{
	if(pool_dir)
	{
		int i;
		char name[32];
		remove_proc_entry("status", pool_dir);
		for(i = 0; i < AV_POOL_MAX_SUPPORT_CHN; i++)
		{
			sprintf(name, "ch%d_status", i);
			remove_proc_entry(name, pool_dir);
		}
		remove_proc_entry("avpool", NULL);
	}
	
	misc_deregister(&av_pool_miscdev);
		
	av_pool_uninit();
	mem_mng_exit();
	
}

module_init(av_pool_module_init);
module_exit(av_pool_module_exit);

MODULE_AUTHOR("JXJ xieqiang");
MODULE_DESCRIPTION("AV POOL FOR IPNC");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(AV_POOL_MINOR);


