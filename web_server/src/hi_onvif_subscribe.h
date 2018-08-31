#include "soapStub.h"
#include "stdsoap2.h"
#include "nvr_struct.h"

#define TEV_TNL_SIZE 	6
#define TEV_URI_LEN 	128
#define	DEV_URI_LEN		128
#define DEV_STR_LEN		128
#define TEV_TS_SIZE 	2
#define TEV_TED_SIZE	2
#define TEV_MCFD_SIZE	1
#define TEV_MCSL_SIZE	1
#define LINE_SIZE		256

#define TEV_ALIVE_SEC	60

#define ENDPOINT_ANY_LEN	512

#define DEV_LINE_LEN		512



typedef struct
{
	char name[DEV_STR_LEN];
	char type[DEV_STR_LEN];
}tev_topic_simple_t;

typedef struct 
{
	char topic[DEV_STR_LEN];
	tev_topic_simple_t source;
	tev_topic_simple_t data;
	tev_topic_simple_t key;
}tev_topic_t;

typedef struct __tev_Subscription_t
{
	int id;
	char addr[DEV_URI_LEN];
	int tick;
	int flag;
	char topic[LINE_SIZE];
	struct __tev_Subscription_t* next;
}tev_Subscription_t;

typedef struct
{
	int state;
	pthread_t pid;

	int num;
	tev_Subscription_t* head;
	pthread_mutex_t mutex;
}tev_SubscriptionManager_t;

extern tev_SubscriptionManager_t g_tev_SubscriptionManager;
extern int PullPoint_TimeOut;
extern int g_tev_TopicNamespaceLocationNum;
extern char** g_tev_TopicNamespaceLocation;

extern int g_tev_TopicNum;
extern tev_topic_t* g_tev_TopicSet;

extern int g_tev_TopicExpressionDialectNum;
extern char** g_tev_TopicExpressionDialect;

extern int g_tev_MessageContentFilterDialectNum;
extern char** g_tev_MessageContentFilterDialect;

extern int g_tev_MessageContentSchemaLocationNum;
extern char** g_tev_MessageContentSchemaLocation;


int tev_Init();


int tev_RenewSubscriptionFromManager(tev_SubscriptionManager_t* SubscriptionManager, int id);
int tev_InsertSubscriptionToManager(tev_SubscriptionManager_t* SubscriptionManager, const char* addr,const char* topic);
int tev_DeleteSubscriptionFromManager(tev_SubscriptionManager_t* SubscriptionManager, int id);


int __ns11__Parse_Duration_Type(char *InitialTerminationTime);
int __ns11__Parse_DateTime_Type(char *InitialTerminationTime);
int __ns11__Check_TopicFilter(struct soap* soap,  struct wsnt__FilterType *Filter, char *Topic);





