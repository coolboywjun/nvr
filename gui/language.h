#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__

#define MAX_STRING_NUM		500	//osd词条数目
#define MAX_LANGUAGE		5


typedef struct _LanguageObj
{
	int		nSupport;
	char	*pSupportLanguage[MAX_LANGUAGE];
	char    *string[MAX_LANGUAGE][MAX_STRING_NUM];

	int		nDisplayLanIndex;
	char	szDisplayLanguage[64];

	int		nInputLanguageIndex;  //0中文，1英文
}LanguageObj_t;

void SetDisplayLanguage(int index);

void GetDisplayLanguage(char *chLanguage);

void GetDisplayLanguageIndex(int *index);

void SetInputLanguage();

int InitLanguage();

int UninitLanguage();

char *Translate(const char *string);

char *getLanguageName(int lanIndex);

int getSysSupportLanguages(void);

#define LTR(string) Translate(string)
#endif
