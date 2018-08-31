/////////////////////////////////////////////////////////
//updatefilemake.c
////////////////////////////////////////////////////////////
//



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>


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

enum
{
IPNC_UPDATE_CFG_IR = 0,
IPNC_UPDATE_CFG_AUDIO = 1,
IPNC_UPDATE_CFG_REL = 2,
IPNC_UPDATE_CFG_DOME = 3,
IPNC_UPDATE_CFG_AIO= 4,
IPNC_UPDATE_CFG_PLATFORM = 5,
IPNC_UPDATE_CFG_LENSTYPE = 6,
IPNC_UPDATE_CFG_SAVE = 0xa,
};

enum
{
IPNC_IR_CLOSE = 0,
IPNC_IR_OPEN = 1,
};

enum
{
IPNC_AUDIO_INTER = 0,
IPNC_AUDIO_EXTER = 1,
};

enum
{
IPNC_RESOLUTION_720P = 2,
IPNC_RESOLUTION_1080P = 3,
};


enum
{
IPNC_SYS_CFG_IR = 0x4000,
IPNC_SYS_CFG_EXT_AUDIO = 0x8000, //外部音频
IPNC_SYS_CFG_EXT_DOME = 0x2000, //高速球 
IPNC_SYS_CFG_EXT_AIO = 0x1000, //高清一体机
IPNC_SYS_CFG_LENSTYPE = 0X00F0, //镜头类型
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

#define PLATFORM_MASK_ZSYH			(1<<0)
#define PLATFORM_MASK_JXJ			(1<<1)
#define PLATFORM_MASK_XLNVR			(1<<2)
#define PLATFORM_MASK_ONVIF			(1<<3)
#define PLATFORM_MASK_HXHT			(1<<4)
#define PLATFORM_MASK_GB28181     	(1<<5)

#define UPDATE_FILE_HDR_MAGIC 0x415159a2UL
typedef struct _TI_UPGRADE_FILE_HDR_S
{
unsigned int		u32Magic;		//update file magic flag 
unsigned int		u32SuitMachine;  //flag all the  suitable machine 
unsigned int		u32FileNum;		//update file num
unsigned int		u32TotalLen;		//update file total len
unsigned char		u8RebootFlag;	//the reboot flag
////////////////////////////////////////////////////a totol of 31 byte
unsigned char       u8Res[3];
unsigned int        	u32SoftVersion; //software version
unsigned char		u8Res1[24];
/////////////////////////////////////////////////////
}TI_UPGRADE_FILE_HDR_S, *LPTI_UPGRADE_FILE_HDR_S;

typedef struct _TI_UPGRADE_FILE_INFO_S
{
unsigned char		szFileName[128];
unsigned int		u32FileSize;
unsigned int		u32FileAttr;
unsigned char		u8Res[120];
}TI_UPGRADE_FILE_INFO_S, *LPTI_UPGRADE_FILE_INFO_S;




#define UPDATE_DES_PACKET_FLG  "updatepacket"
#define UPDATE_DES_PACKET_START  "{"
#define UPDATE_DES_PACKET_END  "}"


#define UDPATE_DES_PACKET_DEVINDEX "devindex"
#define UDPATE_DES_PACKET_TYPE  "packetType"
#define UDPATE_DES_PACKET_OFT  "offset"
#define UDPATE_DES_PACKET_FILE  "file"
#define UDPATE_DES_PACKET_CMD  "cmd"
#define UDPATE_DES_PACKET_SIZE  "size"
#define UDPATE_DES_PACKET_PATH  "path"
#define UDPATE_DES_PACKET_HARDWARE  "hardware"
typedef struct Tag_UpdateFileObj
{
FILE *fp;
char name[256];
int offset;
TI_UPGRADE_FILE_HDR_S hdr;
}UpdateFileObj;


typedef struct Tag_PacketObj
{
UpdateFileObj *pFileObj;
char name[256];
char path[128];
char hardware[256];
UpdateHdr hdr;
UpdateCmdPrm cmdprm;
}PacketObj;

/////////////////////////////////////////////////////////
UpdateFileObj * CreateUpdateFile(char *name)
{
	UpdateFileObj *obj;

	obj = (UpdateFileObj *)malloc(sizeof(UpdateFileObj));
	if(obj == NULL) return NULL;

	memset(obj, 0, sizeof(UpdateFileObj));

	strcpy(obj->name, name);

	printf("%s %d\n", __func__, __LINE__);	

	printf("crate file:%s\r\n", obj->name);
	obj->fp = fopen(obj->name, "wb");
	if(obj->fp == NULL)
	{
		free(obj);
		return NULL;
	}

	obj->hdr.u32Magic = UPDATE_FILE_HDR_MAGIC;
	obj->offset += fwrite(&obj->hdr, 1, sizeof(TI_UPGRADE_FILE_HDR_S), obj->fp);
	return obj;

}

//////////////////////////////////////////////////
int CloseUpdateFile(UpdateFileObj * obj)
{
	if(obj == NULL) return -1;
	if(obj->fp == NULL) return -1;

	fseek(obj->fp, 0, SEEK_SET);

	fwrite(&obj->hdr, 1, sizeof(TI_UPGRADE_FILE_HDR_S), obj->fp);

	fclose(obj->fp);
	
	free(obj);
	return 0;
}
////////////////////////////////////////////////////////////
int PutInUpdateFile(UpdateFileObj *obj, char *name, char *dat, int dataLen)
{
	TI_UPGRADE_FILE_INFO_S FileInfo; 
	int writelen = 0;
	if(obj == NULL) return -1;
	if(obj->fp == NULL) return -1;
	
	printf("file:%s\r\n", name);
	printf("size:%d\r\n", dataLen);
	memset(&FileInfo, 0, sizeof(TI_UPGRADE_FILE_INFO_S));	
	strcpy((char *)FileInfo.szFileName, name);
	FileInfo.u32FileSize = dataLen;

	printf("oft:%d\r\n", obj->offset);
	fseek(obj->fp, obj->offset, SEEK_SET);
	
	fwrite(&FileInfo, 1, sizeof(TI_UPGRADE_FILE_INFO_S), obj->fp);
	while(dataLen > writelen)
	{
		if( (dataLen - writelen) > 4*1024)
			writelen += fwrite(dat+writelen, 1, 4*1024,  obj->fp);
		else
			writelen += fwrite(dat+writelen, 1, (dataLen - writelen), obj->fp);
	}
	obj->offset += (sizeof(TI_UPGRADE_FILE_INFO_S) + dataLen);
	obj->hdr.u32TotalLen += (sizeof(TI_UPGRADE_FILE_INFO_S) + dataLen);
	obj->hdr.u32FileNum++;
	
	return 0;
}
//////////////////////////////////////////////////////
int GetValueInt(char * dat, int *val)
{
	int i, index;
	int len = strlen(dat);
	char str[32];
	memset(str, 0, sizeof(str));
	
	for(i = 0; i<len; i++) if(dat[i] != ' ') break;
	if(i == len)  return -1;	

	if(dat[i] != '=') return -1;
	i++;
	for(; i<len; i++) if(dat[i] != ' ') break;
	index = i;
	for(; i<len; i++)
	{
		if(dat[i] == ';') break;
		if(dat[i] == ' ') dat[i] = 0;
	}
	memcpy(str, &dat[index], i-index);

	*val = atoi(str);

	return i;
}
/////////////////////////////////////////////////////
int GetValueStr(char *dat, char *str, int datlen)
{
	int i, index;
	int len;	
	int flg = 1;

	
	len = strlen(dat);
	
	for(i = 0; i<len; i++) if(dat[i] != ' ') break;
	if(i == len)  return -1;	
	
	if(dat[i] != '=') return -1;
	i++;
	for(; i<len; i++) if(dat[i] != ' ') break;	

	if(dat[i] == '\"') { i++; flg = 1;} 
	index = i;
	for(; i<len; i++)
	{
		if(flg)
		{
			if(dat[i] == '\"') break;
		}else
		{
			if(dat[i] == ';') break;
		}
	}
	if(datlen < (i - index))
	{
		memcpy(str, &dat[index], datlen);
	}else
	{
		memcpy(str, &dat[index], i-index);
	}

	if(flg) 
	{ 
		for(; i<len; i++)	if(dat[i] == ';') break;
	}		
	
	return i;
}

//////////////////////////////////////////////////
int GetPacketType(char *str)
{
	 if(strncmp(str, "kernel", strlen("kernel")) == 0)
	 {
		return IPNC_UPDATE_TYPE_KERNEL;		
	 }else  if(strncmp(str, "filesys", strlen("filesys")) == 0)
	 {
		return IPNC_UPDATE_TYPE_FILESYS;		
	 }else  if(strncmp(str, "cmd", strlen("cmd")) == 0)
	 {
		return IPNC_UPDATE_TYPE_CMD;		
	 }else  if(strncmp(str, "logo", strlen("logo")) == 0)
	 {
	 	return IPNC_UPDATE_TYPE_LOGO;
	 }
	 
	return IPNC_UPDATE_TYPE_KERNEL;
}
///////////////////////////////////////////
int GetCmdType(char *str)
{
	if(strncmp(str, "reset", strlen("reset")) == 0)
	{
		return IPNC_UPDATE_CMD_RESET;
	}else if(strncmp(str, "file", strlen("file")) == 0)
	 {
		return IPNC_UPDATE_CMD_FILE;		
	 }else if(strncmp(str, "erase", strlen("erase")) == 0)
	 {
		return IPNC_UPDATE_CMD_ERASE;		
	 }else if(strncmp(str, "match", strlen("match")) == 0)
	 {
	 	return IPNC_UPDATE_CMD_MATCH;
	 }
	 
	 return IPNC_UPDATE_CMD_NULL;
}
///////////////////////////////////////////
int ProcessKernelPacket(PacketObj  *obj)
{
	FILE *fp;
	int filesize, readlen=0;	
	char *pBuf;	
	char *pStr;
	UpdateHdr *pHdr = NULL;

	if(obj->pFileObj == NULL) return -1;

	printf("kernel:%s\r\n", obj->name);
	fp = fopen(obj->name, "rb");
	if(fp == NULL) return -1;

	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	printf("k:size:%d\r\n", filesize);
	pBuf = (char *)malloc(filesize+sizeof(obj->hdr) + 10);
	if(pBuf == NULL) 
	{
		fclose(fp);
		return -1;
	}

	pHdr = (UpdateHdr *)pBuf;
	memcpy(pHdr, &obj->hdr, sizeof(obj->hdr));
	pHdr->dataLen = filesize;
	pHdr->magic = IPNC_UPDATE_PACKET_MAGIC;
	
	while(filesize > readlen )
	{
		if( (filesize-readlen) > 4*1024)	
			readlen += fread(pHdr->dat+readlen,  1,  4*1024, fp);
		else
			readlen += fread(pHdr->dat+readlen,  1,  (filesize-readlen), fp);
	}
	
	if(readlen != filesize)
	{
		free(pBuf);
		fclose(fp);
		return -1;
	}	
	
	pStr = strrchr(obj->name, '/');
	printf("file1:%s\r\n", pStr);
	PutInUpdateFile(obj->pFileObj, pStr + 1, pBuf, filesize+sizeof(obj->hdr));
	
	free(pBuf);
	fclose(fp);
	
	return 0;
}
////////////////////////////////////////////
int ProcessFileSysPacket(PacketObj  *obj)
{
	FILE *fp;
	int filesize, readlen = 0;	
	char *pBuf;	
	char *pStr;
	UpdateHdr *pHdr = NULL;
	int packetlen = 2048*1024;

	if(obj->pFileObj == NULL) return -1;

	printf("filesys:%s\r\n", obj->name);
	fp = fopen(obj->name, "rb");
	if(fp == NULL) return -1;

	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	printf("f:size:%d\r\n", filesize);
	if(packetlen < filesize)
	{
		pBuf = (char *)malloc(packetlen+sizeof(obj->hdr) + 10);
	}else
	{
		pBuf = (char *)malloc(filesize+sizeof(obj->hdr) + 10);
		packetlen = filesize;
	}
	
	if(pBuf == NULL) 
	{
		fclose(fp);
		return -1;
	}

	pHdr = (UpdateHdr *)pBuf;
	memcpy(pHdr, &obj->hdr, sizeof(obj->hdr));
	pHdr->magic = IPNC_UPDATE_PACKET_MAGIC;
	//pHdr->offset = 0;
	pStr = strrchr(obj->name, '/');
	
	if(pStr ==NULL)	pStr = obj->name;
	else pStr++;
	
	printf("file2:%s\r\n", pStr);
	
	while(filesize> 0)
	{
		if(filesize <  packetlen)
		{
			packetlen = filesize;
		}

		readlen = 0;
		
		while(packetlen > readlen )
		{
			if( (packetlen-readlen) > 4*1024)	
				readlen += fread(pHdr->dat+readlen,  1,  4*1024, fp);
			else
				readlen += fread(pHdr->dat+readlen,  1,  (packetlen-readlen), fp);
		}
		
		if(readlen != packetlen)
		{
			free(pBuf);
			fclose(fp);
			return -1;
		}
#if 0
		{
			static int index = 0;
			char name[32];
			FILE *fp1;
			sprintf(name, "packet%d.dat", index++);
			printf("create %s\r\n", name);
			fp1 = fopen(name, "wb");
			if(fp1)
			{
				fwrite(pHdr->dat, 1, packetlen, fp1);
				fclose(fp1);
			}

		}
#endif
		pHdr->dataLen = packetlen;
		PutInUpdateFile(obj->pFileObj, pStr, pBuf, packetlen+sizeof(obj->hdr));
		pHdr->offset += packetlen;
		filesize -= packetlen;
	}
	
	free(pBuf);
	fclose(fp);
	
	return 0;
}
///////////////////////////////////////////////
int ProcessCmdPacket(PacketObj *obj)
{
	char *pBuf = NULL;
	int filesize;
	int writelen = 0;
	UpdateCmdPrm *pPrm;				
	FILE *fp;
	
	
	switch(obj->cmdprm.cmd)
		{
		case IPNC_UPDATE_CMD_RESET:
			{	
				printf("reset cmd\r\n");
				pBuf = (char *)malloc(sizeof(obj->hdr) + sizeof(int) + 10);
				obj->hdr.dataLen = sizeof(int);
				memcpy(pBuf, &obj->hdr, sizeof(obj->hdr) );
				memcpy(pBuf+sizeof(obj->hdr), &obj->cmdprm.cmd, sizeof(int));
					
				PutInUpdateFile(obj->pFileObj, "reset", pBuf, sizeof(obj->hdr) + sizeof(int));
				free(pBuf);
			}
			break;
		case IPNC_UPDATE_CMD_FILE:
			{		
				char *pDat;
				int readlen = 0;
				printf("file cmd\r\n");
				fp = fopen(obj->name, "rb");
				if(fp == NULL) return -1;
				fseek(fp, 0, SEEK_END);
				filesize = ftell(fp);
				fseek(fp, 0, SEEK_SET);
				printf("f:size:%d\r\n", filesize);
				writelen = sizeof(obj->hdr) + sizeof(obj->cmdprm) + strlen(obj->path) + filesize;
				pBuf = (char *)malloc(writelen +10);
				obj->hdr.dataLen = writelen - sizeof(obj->hdr); 
				memcpy(pBuf, &obj->hdr, sizeof(obj->hdr) );
				pPrm = (UpdateCmdPrm *) (pBuf + sizeof(obj->hdr));				
				memcpy(pPrm, &obj->cmdprm, sizeof(obj->cmdprm));
				pPrm->u.file.pathLen = strlen(obj->path);
				pPrm->u.file.dataLen = filesize;
				
				memcpy(pBuf + sizeof(obj->hdr)+sizeof(UpdateCmdPrm), obj->path, pPrm->u.file.pathLen);
				pDat = pBuf + sizeof(obj->hdr)+sizeof(UpdateCmdPrm) + pPrm->u.file.pathLen;
				while(filesize > readlen)
					{
						if( (filesize - readlen) > 4*1024)
						 	readlen += fread(pDat+readlen, 1, 4*1024, fp);
						else
							readlen += fread(pDat+readlen, 1,  (filesize - readlen), fp);
					}
				
				PutInUpdateFile(obj->pFileObj, "file", pBuf, writelen);
				free(pBuf);
				fclose(fp);
			}
			

			break;
		case IPNC_UPDATE_CMD_ERASE:
			{
				printf("erase cmd\r\n");
				pBuf = (char *)malloc(sizeof(obj->hdr) + sizeof(int) + sizeof(EraseCmdPrm) + 10);
				obj->cmdprm.u.erase.dev = obj->hdr.devIndex;
				obj->cmdprm.u.erase.offset = obj->hdr.offset;
				obj->hdr.dataLen = sizeof(obj->cmdprm);
				memcpy(pBuf, &obj->hdr, sizeof(obj->hdr) );
				memcpy(pBuf + sizeof(obj->hdr), &obj->cmdprm.cmd, sizeof(int));
				memcpy(pBuf + sizeof(obj->hdr) + sizeof(int), &obj->cmdprm.u.erase, sizeof(EraseCmdPrm) );		
					
				PutInUpdateFile(obj->pFileObj, "erase", pBuf, sizeof(obj->hdr) + sizeof(int) + sizeof(EraseCmdPrm));
				free(pBuf);
			}

			break;
		case IPNC_UPDATE_CMD_MATCH:
			{
				printf("match cmd\r\n");
				int len;
				if(strlen(obj->hardware) == 0)
				{
					printf("Must input the hardware version.\n");
					return -1;
				}
				pBuf = (char *)malloc(sizeof(obj->hdr) + sizeof(int) + sizeof(obj->hardware) + 100);
				len = sizeof(int);
				len += sprintf(pBuf + sizeof(obj->hdr) + sizeof(int), "hardware=%s", obj->hardware) + 1;
				obj->hdr.dataLen = len;
				memcpy(pBuf, &obj->hdr, sizeof(obj->hdr));
				memcpy(pBuf + sizeof(obj->hdr), &obj->cmdprm.cmd, sizeof(int));
				PutInUpdateFile(obj->pFileObj, "match", pBuf, sizeof(obj->hdr) + len);
				free(pBuf);
			}
			break;
		default:

			return -1;
		}


	return 0;
		
}
///////////////////////////////////////////
int ProcessLogoPacket(PacketObj  *obj)
{
	FILE *fp;
	int filesize, readlen=0;	
	char *pBuf;	
	char *pStr;
	UpdateHdr *pHdr = NULL;

	if(obj->pFileObj == NULL) return -1;

	printf("logo:%s\r\n", obj->name);
	fp = fopen(obj->name, "rb");
	if(fp == NULL) return -1;

	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	printf("logo size:%d\r\n", filesize);
	pBuf = (char *)malloc(filesize+sizeof(obj->hdr) + 10);
	if(pBuf == NULL) 
	{
		fclose(fp);
		return -1;
	}

	pHdr = (UpdateHdr *)pBuf;
	memcpy(pHdr, &obj->hdr, sizeof(obj->hdr));
	pHdr->dataLen = filesize;
	pHdr->magic = IPNC_UPDATE_PACKET_MAGIC;
	
	while(filesize > readlen )
	{
		if( (filesize-readlen) > 4*1024)	
			readlen += fread(pHdr->dat+readlen,  1,  4*1024, fp);
		else
			readlen += fread(pHdr->dat+readlen,  1,  (filesize-readlen), fp);
	}
	
	if(readlen != filesize)
	{
		free(pBuf);
		fclose(fp);
		return -1;
	}	
	
	pStr = strrchr(obj->name, '/');
	printf("file1:%s\r\n", pStr);
	PutInUpdateFile(obj->pFileObj, pStr + 1, pBuf, filesize+sizeof(obj->hdr));
	
	free(pBuf);
	fclose(fp);
	
	return 0;
}
///////////////////////////////////////////////////////////
int ProcessPacketReq(PacketObj *obj)
{
	obj->hdr.magic = 	IPNC_UPDATE_PACKET_MAGIC;
	switch(obj->hdr.packetType)
		{
		case IPNC_UPDATE_TYPE_KERNEL:
			return ProcessKernelPacket(obj);
			break;
		case IPNC_UPDATE_TYPE_FILESYS:
			return ProcessFileSysPacket(obj);
			break;
		case IPNC_UPDATE_TYPE_CMD:
			printf("packet cmd\r\n");
			return ProcessCmdPacket(obj);
			break;
		case IPNC_UPDATE_TYPE_LOGO:
			return ProcessLogoPacket(obj);
			break;
		default:
			printf("unknow packet\r\n");

			break;
		}

	return 0;
}
////////////////////////////////////////////////
int GetFileLine(FILE *fp, char *dat, int len)
{
	if(fgets(dat, len, fp) == NULL) return -1;

	return 0;
}
///////////////////////////////////////////////////
int parseDesFile(char *name, UpdateFileObj *pObj)
{
	
	FILE *fp = NULL;	
	char dat[200];
	int len;
	int packetflg;
	int i;
	PacketObj  packet;
	char *dir;
	packet.pFileObj = pObj;
	
	fp = fopen(name, "r");
	if(fp == NULL)
	{
		printf("can't open des file\r\n");
		return -1;
	}
	// 更换工作目录
	dir = dirname(name);
	if(chdir(dir) != 0)
	{
		printf("Change workdir failed.\n");
	}

	while(1)
	{
			
		if(GetFileLine(fp, dat, 200) == -1) break;

		len = strlen(dat);
			
		if(len == 0) continue;

		for(i = 0; i<len; i++) if(dat[i] != ' ') break;

		if(i == len) continue;
		if(dat[i] == '#') continue;

		while(i<len)
		{
			for(; i<len; i++) if(dat[i] != ' ') break;
			if(i == len) continue;
			
			if( strncmp(&dat[i], UPDATE_DES_PACKET_FLG, strlen(UPDATE_DES_PACKET_FLG)) == 0)
			{
				memset(packet.name, 0, sizeof(packet.name));
				memset(packet.path, 0, sizeof(packet.path));
				memset(packet.hardware, 0, sizeof(packet.hardware));
				memset(&packet.hdr, 0, sizeof(packet.hdr));
				memset(&packet.cmdprm, 0, sizeof(packet.cmdprm));
				packetflg = 1;
				i +=  strlen(UPDATE_DES_PACKET_FLG);
				printf("%s\r\n", UPDATE_DES_PACKET_FLG);
			}else if( strncmp(&dat[i], UPDATE_DES_PACKET_START, strlen(UPDATE_DES_PACKET_START)) == 0)
			{
				if(packetflg == 1) packetflg = 2;
				i +=  strlen(UPDATE_DES_PACKET_START);
				printf("%s\r\n", UPDATE_DES_PACKET_START);
			}else if( strncmp(&dat[i], UPDATE_DES_PACKET_END, strlen(UPDATE_DES_PACKET_END)) == 0)
			{
				
				i +=  strlen(UPDATE_DES_PACKET_END);	
				if(packetflg == 2)
				{
					ProcessPacketReq(&packet);
				}
				if(packetflg == 2) packetflg = 0;
				printf("%s\r\n", UPDATE_DES_PACKET_END);
			}else if( strncmp(&dat[i], UDPATE_DES_PACKET_DEVINDEX, strlen(UDPATE_DES_PACKET_DEVINDEX)) == 0)
			{				
				i +=  strlen(UDPATE_DES_PACKET_DEVINDEX);
				if(packetflg == 2)
				{
					i += GetValueInt(&dat[i], &packet.hdr.devIndex);
				}
				printf("%s:%d\r\n", UDPATE_DES_PACKET_DEVINDEX, packet.hdr.devIndex);
			}else if( strncmp(&dat[i], UDPATE_DES_PACKET_TYPE, strlen(UDPATE_DES_PACKET_TYPE)) == 0)
			{				
				i +=  strlen(UDPATE_DES_PACKET_TYPE);
				if(packetflg == 2)
				{	char typestr[32] = {0,};
			
					i += GetValueStr(&dat[i], typestr, 32);

					packet.hdr.packetType = GetPacketType(typestr);
				}
				printf("%s:%d\r\n", UDPATE_DES_PACKET_TYPE, packet.hdr.packetType);
			}else if( strncmp(&dat[i], UDPATE_DES_PACKET_FILE, strlen(UDPATE_DES_PACKET_FILE)) == 0)
			{				
				i +=  strlen(UDPATE_DES_PACKET_FILE);
				if(packetflg == 2)
				{				
					i += GetValueStr(&dat[i], packet.name, 256);					
				}
				printf("%s:%s\r\n", UDPATE_DES_PACKET_FILE, packet.name);
			}else if( strncmp(&dat[i], UDPATE_DES_PACKET_OFT, strlen(UDPATE_DES_PACKET_OFT)) == 0)
			{				
				i +=  strlen(UDPATE_DES_PACKET_OFT);
				if(packetflg == 2)
				{				
					i += GetValueInt(&dat[i], &packet.hdr.offset);					
				}
				printf("%s:%d\r\n", UDPATE_DES_PACKET_OFT,packet.hdr.offset);
			}else  if( strncmp(&dat[i], UDPATE_DES_PACKET_CMD, strlen(UDPATE_DES_PACKET_CMD)) == 0)
			{				
				i +=  strlen(UDPATE_DES_PACKET_CMD);
				if(packetflg == 2)					
				{	char typestr[32] = {0,};			
					i += GetValueStr(&dat[i], typestr, 32);	
					packet.cmdprm.cmd = GetCmdType( typestr);
				}
				printf("%s:%d\r\n", UDPATE_DES_PACKET_CMD,packet.cmdprm.cmd);
			}else if( strncmp(&dat[i], UDPATE_DES_PACKET_SIZE, strlen(UDPATE_DES_PACKET_SIZE)) == 0)
			{				
				i +=  strlen(UDPATE_DES_PACKET_SIZE);
				if(packetflg == 2)					
				{	
					i += GetValueInt(&dat[i], &packet.cmdprm.u.erase.size);
				}
				printf("%s:%d\r\n", UDPATE_DES_PACKET_SIZE, packet.cmdprm.u.erase.size);
			}else if( strncmp(&dat[i], UDPATE_DES_PACKET_PATH, strlen(UDPATE_DES_PACKET_PATH)) == 0)
			{				
				i +=  strlen(UDPATE_DES_PACKET_PATH);
				if(packetflg == 2)
				{				
					i += GetValueStr(&dat[i], packet.path, 128);					
				}
				printf("%s:%s\r\n", UDPATE_DES_PACKET_PATH, packet.path);
			}else if( strncmp(&dat[i], UDPATE_DES_PACKET_HARDWARE, strlen(UDPATE_DES_PACKET_HARDWARE)) == 0) 
			{		
				i +=  strlen(UDPATE_DES_PACKET_HARDWARE);
				if(packetflg == 2)
				{	
					i += GetValueStr(&dat[i], packet.hardware, sizeof(packet.hardware));					
				}
				printf("%s:%s\r\n", UDPATE_DES_PACKET_HARDWARE, packet.hardware);
			}else
			{
				for(; i<len; i++)
				{
				if(dat[i] == ' ') break;
				if(dat[i] == '\"') break;
				}
			}			
			
		}		

	}


	fclose(fp);

	return 0;
}

////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	UpdateFileObj *pObj;
	
	//usage example: ./updatefilemake package.txt jxj_v1.1.1.1.bin

	if(argc < 3)
	{
		return -1;
	}

	//strncpy(sensorType, argv[2], sizeof(sensorType));
	
	pObj = CreateUpdateFile(argv[2]);

	if(pObj == NULL) return -1;

	parseDesFile(argv[1], pObj);	
	

	CloseUpdateFile(pObj);

	return 0;
}

