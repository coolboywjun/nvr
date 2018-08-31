#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "language.h"

static LanguageObj_t obj;

#define LAN_DIR "nvr_res/language"

static char * _ExpressionGetValue(char *expression, char *var, char *value)
{
	char *p = NULL;
	int n = 0;

	value[0] = 0;
	p = strstr(expression, var);
	if (p == NULL) 
		return NULL;

	p = strstr(expression, "=");
	if (p == NULL) 
		return NULL;
	p++;

	for (; p != NULL ; p++)
	{
		if ((*p != '\40')&&(*p != '\r')&&(*p != '\n'))
		{
			strcpy(value, p);
			break;
		}
	}
	
	p = value;
	for (; *p != 0 ; p++)
	{
		if (*p == '\40')
			memmove(p, p+1, strlen(p+1));

		if ((*p == '\r')||(*p == '\n'))
			*p = 0;
	}

	return value;
}

static int _GetLangunageInfo()
{	
	int nNum = 0;
	char line[256];
	char *p = NULL;
	int bGetDisplay = 0;
	int bGetSupport = 0;
	char szSupport[4];
	char szDir[64];
	
	sprintf(szDir, "%s/language_conf.ini", LAN_DIR);
	FILE *fconfig = fopen(szDir, "rb");
	if (fconfig == NULL) 
		return -1;

	while(fgets(line, 256, fconfig))
	{
		p = strstr(line, "\r\n");
		if (p) 
			*p = 0;
		else
		{
			p = strstr(line, "\n");
			if (p) *p=0;
			else continue;
		}

		if (!bGetDisplay)
		{
			p = _ExpressionGetValue(line, "display", obj.szDisplayLanguage);
			if (p != NULL) 
			{
				bGetDisplay = 1;
				continue;
			}
		}

		if (!bGetSupport)
		{
			p = _ExpressionGetValue(line, "support", szSupport);
			if (p != NULL) 
			{
				obj.nSupport = atoi(p);
				bGetSupport = 1;
				continue;
			}
		}

		if (bGetDisplay && bGetSupport)
		{
			if (nNum >= obj.nSupport) 
				break;

			obj.pSupportLanguage[nNum] = (char *)malloc(64);
			strcpy(obj.pSupportLanguage[nNum], line);

			if (strcmp(line, obj.szDisplayLanguage) == 0)
				obj.nDisplayLanIndex = nNum;
			nNum++;
		}
	}

	fclose(fconfig);

	return 0;

}

static void _GetLanguageString()
{
	char line[256];
	FILE *flang = NULL;
	int nLanguage = 0;
	char *p = NULL;
	for (nLanguage = 0; nLanguage < obj.nSupport; nLanguage++)
	{
		int nLine = 0;
		char szDir[64];		
		sprintf(szDir, "%s/%s", LAN_DIR, obj.pSupportLanguage[nLanguage]);
		flang = fopen(szDir, "rb");
		if (flang == NULL) return;

		while(fgets(line, 256, flang))
		{
			p = strstr(line, "\r\n");
			if (p) 
				*p = 0;
			else
			{
				p = strstr(line, "\n");
				if (p) *p=0;
				//else continue;
			}

			//printf("line = %s\r\n", line);
			obj.string[nLanguage][nLine] = (char *)malloc(strlen(line)+1);
			strcpy(obj.string[nLanguage][nLine], line);
			nLine++;
		}

		fclose(flang);
		flang = NULL;
	}
}

void SetInputLanguage()
{
	obj.nInputLanguageIndex = 0;
}
void SetDisplayLanguage(int index)
{
	obj.nDisplayLanIndex = index;
#if 0	
	int i = 0;
	
	if(NULL == selLanguage)
		return;
		
	for(i=0; i<obj.nSupport; i++)
	{
		if(strcmp(selLanguage, obj.pSupportLanguage[i]) == 0)
		{
			obj.nDisplayLanIndex = i;
			//printf("obj.nDisplayLanIndex: %d\n",obj.nDisplayLanIndex);
			return;
		}
	}

	if(i == obj.nSupport)
		return;
#endif	
}

void GetDisplayLanguage(char *chLanguage)
{
	if(NULL == chLanguage)
		return ;
	strcpy(chLanguage, obj.pSupportLanguage[obj.nDisplayLanIndex]);
}
void GetDisplayLanguageIndex(int *index)
{
	if(NULL == index)
		return ;
	if (obj.nDisplayLanIndex > 2)
		*index = 0;
	else 
		*index = obj.nDisplayLanIndex;
}

char *Translate(const char *string)
{	
	int n = 0;

	//if (string[0] >= 128)
	//	obj.nInputLanguageIndex = 0;
	//else
	//	obj.nInputLanguageIndex = 1;
	
	if(obj.nInputLanguageIndex == obj.nDisplayLanIndex)
	{	
		return string;
	}

	
	//printf("########## %d\r\n", obj.nInputLanguageIndex);
	
	for (n = 0; n < MAX_STRING_NUM; n++ )
	{	
		if (obj.string[obj.nInputLanguageIndex][n] == NULL) 
			break;

		//printf("string[0] = %c\r\n", string);

		if (strcmp(obj.string[obj.nInputLanguageIndex][n], string) == 0)
		{
			
			//printf("11111111111string = %s\r\n", string);
			if (obj.string[obj.nDisplayLanIndex][n]== NULL)
				break;

			//printf("find index = %d n = %d name  = %s==========\r\n", obj.nInputLanguageIndex, n, obj.pSupportLanguage[obj.nDisplayLanIndex]);
			//printf("22222222 = %s, disp index = %d\r\n", string, obj.nDisplayLanIndex);
			return obj.string[obj.nDisplayLanIndex][n];
		}
		
		//printf("22222222 = %s\r\n", string);
	}

	return string;
}


int InitLanguage()
{
	_GetLangunageInfo();
	_GetLanguageString();
	return 0;
}

int UninitLanguage()
{
	int nLanguage;
	int nLine;
	for (nLanguage = 0; nLanguage < obj.nSupport; nLanguage++)
	{
		if (obj.pSupportLanguage[nLanguage] != NULL)
		{
			free(obj.pSupportLanguage[nLanguage]);
			obj.pSupportLanguage[nLanguage] = NULL;
		}

		for (nLine = 0; nLine < MAX_STRING_NUM; nLine++)
		{
			if (obj.string[nLanguage][nLine] != NULL)
			{
				free(obj.string[nLanguage][nLine]);
				obj.string[nLanguage][nLine] = NULL;
			}
		}
	}

	memset(&obj, 0, sizeof(obj));

	return 0;

}

char *getLanguageName(int lanIndex)
{
	return obj.string[lanIndex][0];
}

int getSysSupportLanguages(void)
{
	return obj.nSupport;
}

