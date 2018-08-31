#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DBG(...)

//////////////////////////////////////////
static int isdir(char *path)
{
 	struct stat sta;
	 if(lstat(path,&sta)<0)
 	 {
      		return 0;
  	 }
	 
 	 if(S_ISDIR(sta.st_mode)||S_ISLNK(sta.st_mode))
        {
 	   return 1;
	 }
	 
   return 0;
}

/////////////////////////////////////////////
static char * GetExeName(char *path)
{
	char *pStr;
	pStr = strrchr(path, '/');

	if(pStr == NULL) return path;

	return pStr+1;	
} 
 

///////////////////////////////////////////////////////////
int GetProcPid(const char *cmd)
{
	DIR *procDir;
	struct dirent *p;
	char buf[128];
	//char link[128];
	char cmdline[128];
	//char *pStr;
	FILE *fp;
	int pid = -1;
	int mypid;
	if(cmd == NULL) return -1;
	
	procDir = opendir("/proc/");
	if(procDir == NULL)
	{
		DBG("open proc dir err\r\n");
		return -1;
	}
	mypid = getpid();
	while((p=readdir(procDir)))
	{
	    if((strcmp(p->d_name,".")==0)||(strcmp(p->d_name,"..")==0))
      			 continue;
		
	    sprintf(buf, "/proc/%s", p->d_name);

	    if(isdir(buf))
	    {
	    	if(atoi(p->d_name) == 0)
	    		continue;
	    	if(atoi(p->d_name) == mypid)
	    		continue;
	    /*
		strcat(buf, "/exe");
		
		if(access(buf,F_OK)==0)
		{       
		  memset(link, 0, sizeof(link));	
                if(readlink(buf, link,sizeof(link))==-1)
              	  continue;
		
		  pStr =GetExeName(link);
		  if(pStr == NULL) continue;
		  
                if(strcmp(pStr, cmd)==0)
                {			
			pid = atoi(p->d_name);
			break;
                }
				
          	}
          */
          	strcat(buf, "/cmdline");
          	if(access(buf, F_OK) == 0)
          	{
          		if((fp = fopen(buf, "r")) != NULL)
          		{
          			memset(cmdline, 0, sizeof(cmdline));
				if(fgets(cmdline, sizeof(cmdline)-1, fp) != NULL)
				{
					if(strstr(cmdline, "wdt") == NULL && strstr(cmdline, cmd) != NULL)
					{
						pid = atoi(p->d_name);
						fclose(fp);
						break;
					}
				}
				fclose(fp);
          		}
          	}
	    }
	}
	closedir(procDir);

	return pid;
}
/////////////////////////////////////////////
int CheckProcStat(int pid)
{
	char buf[128];
	char line[80];
	FILE *fp;
	char *pStr;
	int sta = 1;
	
	sprintf(buf, "/proc/%d", pid);

	if(!isdir(buf)) return 0;

	/* 加入这一段的原因是如果读取cmdline会卡死，
	说明系统当前运行不正常*/
	sprintf(buf, "/proc/%d/cmdline", pid);
	fp = fopen(buf, "r");
	if(fp == NULL) return 0;
	if(fgets(line, sizeof(line), fp) == NULL)
	{
		fclose(fp);
		return 0;
	}
	fclose(fp);

	//strcat(buf, "/stat");
	sprintf(buf, "/proc/%d/stat", pid);
	fp = fopen(buf, "r");
	if(fp == NULL) return 0;
	if(fgets(line, sizeof(line), fp) == NULL)
	{
		fclose(fp);
		return 0;
	}
	fclose(fp);	

	pStr = strchr(line,' ');
	if(pStr == NULL) return 0;
	while((*pStr) == ' ') pStr++;	
	pStr = strchr(pStr+1, ' ');
	if(pStr == NULL) return 0;
	while((*pStr) == ' ') pStr++;	

	switch(pStr[0])
		{
	case 'S':
		sta = 1;
		break;
	case 'R':
		sta = 1;
		break;
 	case 'T':
		sta = 1;
		break;
	case 'Z':
		sta = 0;
		break;
	case 'X':		
		sta = 0;
		break;
	case 'D':
		sta = 1;
	default:
		
		break;
		}
	
	return sta;
}

//////////////////////////////////
#ifdef PROC_TEST
int main(int argc, char *argv[])
{
	int pid;

	DBG("argc:%d\r\n", argc);
	if(argc < 2) return 0;

	DBG("Get %s PID\r\n", argv[1]);
	pid = GetProcPid(argv[1]);

	DBG("pid:%d\r\n", pid);


	return 0;
}

#endif

#ifdef CHECK_TEST
int main(int argc, char *argv[])
{
	int pid;

	if(argc < 2) return 0;
	pid = atoi(argv[1]);

	if(CheckProcStat( pid))
	{
		DBG("proc run \r\n");
	}else
	{
		DBG("proc no run\r\n");
	}

	return 0;
}
#endif

