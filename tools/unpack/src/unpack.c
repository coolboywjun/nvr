

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <dirent.h>
#include <sys/mount.h>

#include <update.h>


#define IPNC_UPDATE_PACKET_MAGIC 0x188a4423
enum
{
IPNC_UPDATE_TYPE_NONE = 0,
IPNC_UPDATE_TYPE_KERNEL,
IPNC_UPDATE_TYPE_FILESYS,
IPNC_UPDATE_TYPE_CMD,
IPNC_UPDATE_TYPE_LOGO,
IPNC_UPDATE_TYPE_MAX,
};

typedef struct Tag_UpdateHdr
{
unsigned int magic;
int packetType;
int dataLen;
int offset;
int devIndex;
char dat[];
}UpdateHdr;

enum
{
IPNC_UPDATE_CMD_NULL = 0,
IPNC_UPDATE_CMD_ERASE = 1,
IPNC_UPDATE_CMD_FILE = 2,
IPNC_UPDATE_CMD_RESET = 3,
IPNC_UPDATE_CMD_SYSCFG = 4,
IPNC_UPDATE_CMD_MATCH = 5,
IPNC_UPDATE_CMD_ORDER = 6,
};

typedef struct Tag_EraseCmdPrm
{
int dev;
int offset;
int size;
}EraseCmdPrm;

typedef struct Tag_FileCmdPrm
{
int pathLen;
int dataLen;
char dat[];
}FileCmdPrm;

typedef struct Tag_UpdateCmdPrm
{
int cmd;
union
{
EraseCmdPrm erase;
FileCmdPrm file;
}u;
}UpdateCmdPrm;


static char *pUpdataBuf = NULL;
static int     sUpdateOff = 0;
static int     sUpdateRcvPer = 0;
static int	    sRecvLen = 0;

static NVR_UPGRADE_FILE_HDR_S  stUpdateFileHdr;	
static NVR_UPGRADE_FILE_INFO_S stUpdateFileInfo;
static int     sLastFileHdrOk = 0;
static int     sLastFileHdrLen = 0;
static int     sLastFileInfoOk = 0;
static int     sLastFileInfoLen = 0;

#define NVR_WRITE_BLOCK_LEN    (128*1024)
#define NVR_MAX_UPDATE_BUF	   (8096*1024)


#define PRINT(fmt, args...) \
do{	\
	printf("update " fmt, ##args); \
}while(0)

static int nvr_update_write_kernel(char *buf, int imglen, int devindex)
{
	FILE *fp;

	fp = fopen("uImage_nvr", "wb+");
	if(fp == NULL)
	{
		PRINT("Open uImage_nvr failed\n");
		return -1;
	}
	fseek(fp, 0, SEEK_SET);
	if(fwrite(buf, imglen, 1, fp) != 1)
	{
		PRINT("Write kernel to uImage_nvr failed\n");
		fclose(fp);
		return -1;
	}
	fclose(fp);
	
	return 0;
}

static int nvr_update_write_filesys(char *data,  int datalen, int devindex, int oft)
{
	FILE *fp;
	
	fp = fopen("nvr_target.ramdisk", "ab+");
	if(fp == NULL)
	{
		PRINT("Open nvr_target.ramdisk failed\n");
		return -1;
	}
	fseek(fp, oft, SEEK_SET);
	if(fwrite(data, datalen, 1, fp) != 1)
	{
		PRINT("Write filesys to nvr_target.ramdisk failed\n");
		fclose(fp);
		return -1;
	}
	fclose(fp);
	
	return 0;
}

static int nvr_update_do_cmd(char *data, int imglen)
{
	//UpdateCmdPrm CmdPrm;
	//UpdateCmdPrm *pPrm = &CmdPrm;
	int cmd;
	
	memcpy(&cmd, data, sizeof(int));
	switch(cmd)
	{
	case IPNC_UPDATE_CMD_RESET:
		PRINT("get reset cmd\n");
		//system("reboot");
		break;
	case IPNC_UPDATE_CMD_ERASE:
		PRINT("erase flash.\n");
		break;
	case IPNC_UPDATE_CMD_FILE:
		PRINT("cmd file.\n");
		break;
	case IPNC_UPDATE_CMD_SYSCFG:
		PRINT("update syscfg.\n");
		break;
	case IPNC_UPDATE_CMD_MATCH:
		PRINT("check hardware match.\n");
		break;
	case IPNC_UPDATE_CMD_ORDER:
		PRINT("run shell command.\n");
		break;
	default:
		PRINT("unknow cmd\r\n");
		break;

	}

	return 0;
}


static int nvr_update_write_logo(char *data, int imglen)
{
	FILE *fp;

	if(data == NULL || imglen < 1024)
	{
		PRINT("The logo file is wrong.\n");
		return -1;
	}
	fp = fopen("logo.bmp", "wb");
	if(fp == NULL)
	{
		PRINT("Open logo file failed.\n");
		return -1;
	}
	fwrite(data, imglen, 1, fp);
	fclose(fp);
	
	return 0;
}

static int nvr_update_write_packet(void *data, int len)
{
	UpdateHdr hdr;
	UpdateHdr *pHdr = &hdr;
	int ret = 0;
	
	PRINT("parseUpdatePacket = %d\r\n", len);
	memcpy(&hdr, data, sizeof(UpdateHdr));
	
	if(pHdr->magic != IPNC_UPDATE_PACKET_MAGIC)
	{	
		PRINT("update packet magic err\r\n");
		return -1;
	}

	if( (pHdr->dataLen + sizeof(UpdateHdr)) !=	len)
	{
		PRINT("data len err \r\n");
		return -1;		
	}

	switch(pHdr->packetType)
	{
		case IPNC_UPDATE_TYPE_KERNEL:
			PRINT("update kernel\r\n");
			ret = nvr_update_write_kernel(data + sizeof(UpdateHdr), pHdr->dataLen, pHdr->devIndex);
			PRINT("Update kernel finish!\r\n");
			break;
		case IPNC_UPDATE_TYPE_FILESYS:
			PRINT("update filesys\r\n");
			ret = nvr_update_write_filesys(data + sizeof(UpdateHdr), pHdr->dataLen, pHdr->devIndex, pHdr->offset);
			PRINT("update filesys finish!\r\n");
			break;
		case IPNC_UPDATE_TYPE_CMD:
			PRINT("run update cmd\r\n");
			ret = nvr_update_do_cmd(data + sizeof(UpdateHdr), pHdr->dataLen);
			PRINT("run update cmd finish!\r\n");
			break;
		case IPNC_UPDATE_TYPE_LOGO:
			PRINT("update boot logo\r\n");
			ret = nvr_update_write_logo(data + sizeof(UpdateHdr), pHdr->dataLen);
			PRINT("update boot logo finish!\r\n");
			break;
		default:
			PRINT("unknow packet\r\n");
			break;
	}

	return ret;
}


static int   nvr_update_cal_recv_per(int nLen, int nTotalLen)
{
	int nCurPer = 0;
	 
	sRecvLen += nLen;
	nCurPer = (long long)sRecvLen * 100/nTotalLen;
	if (nCurPer != sUpdateRcvPer || nCurPer >= 100) 
	{
		sUpdateRcvPer = nCurPer;
	}
	return 0;
}

static int   nvr_update_first_packet(char *pData, int nLen)
{
	//int nOffLen = 0;
	int nCopyLen;

	nCopyLen = nLen < (sizeof(NVR_UPGRADE_FILE_HDR_S)-sLastFileHdrLen) ? nLen : (sizeof(NVR_UPGRADE_FILE_HDR_S)-sLastFileHdrLen);
	memcpy(((char *)&stUpdateFileHdr+sLastFileHdrLen), pData, nCopyLen);
	sLastFileHdrLen += nCopyLen;
	if(sLastFileHdrLen != sizeof(NVR_UPGRADE_FILE_HDR_S))
	{
		return nCopyLen;	
	}
	
	sLastFileHdrOk = 1;
	//第一包数据则先拷贝文件头信息 及第一个文件信息
	//memcpy(&stUpdateFileHdr, pData, sizeof(NVR_UPGRADE_FILE_HDR_S));
	//memcpy(&stUpdateFileInfo, pData+sizeof(NVR_UPGRADE_FILE_HDR_S), 
	//	sizeof(NVR_UPGRADE_FILE_INFO_S));

	if (stUpdateFileHdr.u32Magic != UPDATE_FILE_HDR_MAGIC)
	{
		return -1;
	}

	//nOffLen = sizeof(NVR_UPGRADE_FILE_HDR_S)+sizeof(NVR_UPGRADE_FILE_INFO_S);
	//sLastFileInfoOk = 1;
	
	PRINT( "recv upgrade file num %u total file size %u\n", 
		stUpdateFileHdr.u32FileNum, stUpdateFileHdr.u32TotalLen);	
		
	return nCopyLen;
}

static int   nvr_update_write_hdr_info(char *pData, int nLen)
{
	int nCopyLen;

	nCopyLen = nLen < sizeof(NVR_UPGRADE_FILE_INFO_S)-sLastFileInfoLen ? nLen : (sizeof(NVR_UPGRADE_FILE_INFO_S)-sLastFileInfoLen);
	memcpy(((char *)&stUpdateFileInfo)+sLastFileInfoLen, pData, nCopyLen);
	sLastFileInfoLen += nCopyLen;
	if(sLastFileInfoLen != sizeof(NVR_UPGRADE_FILE_INFO_S))
	{
		return nCopyLen;	
	}
	
	sLastFileInfoOk = 1;
	sLastFileInfoLen = 0;
	sUpdateOff = 0;
	
	//memcpy(&stUpdateFileInfo, pData, sizeof(NVR_UPGRADE_FILE_INFO_S));
	PRINT( "recv upgrade file name %s file size %d\n", 
		stUpdateFileInfo.szFileName, stUpdateFileInfo.u32FileSize);
	
	return nCopyLen;
}

static int   nvr_update_write_file(char *pData, int nLen)
{
	int nCopyLen = 0;

	while (sUpdateOff + nLen >= stUpdateFileInfo.u32FileSize)
	{
		nCopyLen = (stUpdateFileInfo.u32FileSize - sUpdateOff);
		//拷贝此文件剩余长度
		memcpy(pUpdataBuf + sUpdateOff, pData, nCopyLen); 
		sUpdateOff 	+= nCopyLen;
		pData 		+= nCopyLen;
		nLen 		-= nCopyLen;

		PRINT( "write upgrade file name %s file size %d\n", 
			stUpdateFileInfo.szFileName, sUpdateOff);	
		//call ti write upgrade file 
		if(nvr_update_write_packet(pUpdataBuf, sUpdateOff) < 0)
		{
			PRINT( "Failed to update packet.\n");
			return -1;
		}
		sUpdateOff = 0;
		if (nLen >= sizeof(NVR_UPGRADE_FILE_INFO_S))//剩余文件内容长度长于升级文件信息头
		{
			memcpy((char *)&stUpdateFileInfo, pData, sizeof(NVR_UPGRADE_FILE_INFO_S));
			nLen -= sizeof(NVR_UPGRADE_FILE_INFO_S);
			pData += sizeof(NVR_UPGRADE_FILE_INFO_S);
			sLastFileInfoOk = 1;
			PRINT( "recv upgrade file name %s file size %d\n", 
				stUpdateFileInfo.szFileName, stUpdateFileInfo.u32FileSize);			
			//剩余内容大于当前文件内容，取当前文件内容
			if (nLen >= stUpdateFileInfo.u32FileSize) 
				continue ;
			else
			{
				memcpy(pUpdataBuf + sUpdateOff, pData, nLen);
				sUpdateOff = nLen;
			}
		}
		else if (nLen > 0) //剩余文件内容小于度长于升级文件信息头
		{
			memcpy((char *)&stUpdateFileInfo, pData, nLen);
			sLastFileInfoLen = nLen;
			sLastFileInfoOk = 0;
		}
		else	//无剩余内容则下一包包头则为文件信息头
		{
			sLastFileInfoLen = 0;
			sLastFileInfoOk = 0;
		}
		return 0;
	}
	
	return 0;
}

static int	nvr_update_write_data(char *pData, int nLen, int nPackNo, int nTotalLen)
{
	int nOffLen = 0;

	if(nPackNo == 0)
	{
		sLastFileHdrOk = 0;
		sLastFileHdrLen = 0;
		sLastFileInfoOk = 0;
		sLastFileInfoLen = 0;
		memset(&stUpdateFileHdr, 0, sizeof(NVR_UPGRADE_FILE_HDR_S));
		memset(&stUpdateFileInfo, 0, sizeof(NVR_UPGRADE_FILE_INFO_S));
	}
	
	if (sLastFileHdrOk == 0) // 第一包数据
	{
		nOffLen = nvr_update_first_packet(pData, nLen);
		nLen -= nOffLen;
		pData += nOffLen;
	}

	if (sLastFileInfoOk == 0)
	{
		nOffLen = nvr_update_write_hdr_info(pData, nLen);
		nLen -= nOffLen;
		pData += nOffLen;
	}
	
	if(nLen == 0)
		return 0;

	if(sLastFileInfoOk)
	{
		if (sUpdateOff + nLen > NVR_MAX_UPDATE_BUF)
		{
			return -1;
		}

		if (sUpdateOff + nLen >= stUpdateFileInfo.u32FileSize)
		{//文件结束
			if(nvr_update_write_file(pData, nLen) < 0)
				return -1;
		}
		else
		{
			memcpy(pUpdataBuf + sUpdateOff, pData, nLen);
			sUpdateOff += nLen;
		} 
	}

	return 0;
}

int	nvr_update_write(char *pData, int nLen, int nPackNo, int nTotalLen)
{
	int nRet = 0;
	int nBufSize = 0;
	
	if (pUpdataBuf == NULL && nPackNo == 0)
	{
		nBufSize = NVR_MAX_UPDATE_BUF;
		pUpdataBuf = malloc(nBufSize);
		if(pUpdataBuf == NULL)
		{
			PRINT("malloc for update buffer failed.\n");
			return -1;
		}
		sUpdateOff = 0;
		PRINT("Begin to update.\n");
	} 

	if (nLen == 0 && pUpdataBuf != NULL)
	{
		sUpdateOff = 0;
		sUpdateRcvPer = 0;	
		sRecvLen = 0;
		free(pUpdataBuf);   
		pUpdataBuf = NULL;
		PRINT("Finish update.\n");
		return 0;
	}

	nRet = nvr_update_write_data(pData, nLen, nPackNo,nTotalLen);
	if (nRet >= 0)
	{
		nvr_update_cal_recv_per(nLen, nTotalLen);
	}
	return nRet;
}


int nvr_update_get_percent()
{
	return sUpdateRcvPer;
}

int main(int argc, char *argv[])
{
	char *buff;
	FILE *fp;
	int totallen, len;
	int packet = 0;
	if(argc != 2 || argv[1] == NULL)
	{
		PRINT("usage: %s path\n", argv[0]);
		return -1;
	}
	fp = fopen(argv[1], "rb");
	if(fp == NULL)
	{
		PRINT("Open file %s failed.\n", argv[1]);
		return -1;
	}
	fseek(fp, SEEK_END, 0);
	totallen = ftell(fp);
	fseek(fp, SEEK_SET, 0);
	
	buff = malloc(1024 * 1024);
	while((len = fread(buff, 1, 1024*1024, fp)) > 0)
	{
		if(nvr_update_write(buff, len, packet++, totallen) < 0)
		{
			PRINT("Write update data failed.\n");
			break;
		}
	}
	free(buff);
	fclose(fp);
	
	return 0;
}


