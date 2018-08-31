#include <sys/prctl.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <pthread.h>
#include "hi_onvif_subscribe.h"
#include "stdsoap2.h"
#include "soapStub.h"
#include "soapH.h"
#include "McbXML.h"
#include "nvr_struct.h"
#include "hi_onvif.h"
#include <unistd.h>

//static HI_ALARM_INFO_S pAlarmInfo_onvif;
static int ALAM_FLAG;

tev_SubscriptionManager_t g_tev_SubscriptionManager;
int PullPoint_TimeOut;
int g_tev_TopicNamespaceLocationNum;
char** g_tev_TopicNamespaceLocation;

int g_tev_TopicNum;
tev_topic_t* g_tev_TopicSet;

int g_tev_TopicExpressionDialectNum;
char** g_tev_TopicExpressionDialect;

int g_tev_MessageContentFilterDialectNum;
char** g_tev_MessageContentFilterDialect;

int g_tev_MessageContentSchemaLocationNum;
char** g_tev_MessageContentSchemaLocation;


int __ns11__Parse_Duration_Type(char *InitialTerminationTime)
{
	//P£ªY£ªM£ªDT£ªH£ªM£ªS
	//char* buf = "P8DT2H1S";
	char *buf = InitialTerminationTime;
	char time[128] = {0};
	char buffer[128] = {0};
	
	
	char *p1,*p2,*p3,*p4;
	int e=0;
	long Time= 0;
	
/////////////////////////////////////////////
	p3 = NULL;
	p4 = NULL;
	p3 = strchr(buf, 'P');
	p4 = strchr(buf, 'T');
	if(p3 != NULL && p4 != NULL)
	{
		if(sscanf(p4, "%s", time) == 0)
			return 0;
		//printf("time=%s\r\n", time);
	}
	else
		return 0;

	p1 = NULL;
	p2 = NULL;
	memset(buffer, 0, 128);
	p1 = strchr(time, 'T');
	if(p1 != NULL)
	{
		p2 = strchr(time, 'S');
		if(p2 != NULL)
		{
			if(sscanf(p1,"%*[T]%[0-9]",buffer) != 0)
			{
				if(buffer != NULL)
					Time  = (e = atoi(buffer));
				else 
					return 0;
			}
		}
	}
	else
		return 0;
	#if 0
	///////////////////////////////////////////////
	p1 = NULL;
	p2 = NULL;
	memset(buffer, 0, 128);
	p1 = strchr(time, 'T');
	if(p1 != NULL)
	{
		p2 = strchr(time, 'H');
		if(p2 != NULL)
		{
			if(sscanf(p1,"%*[T]%[0-9]",buffer) != 0)
			{
				if(buffer != NULL)
					Time  = Time + (e = atoi(buffer));
				else 
					return 0;
				if(e < 0 || e > 23)
					return 0;
				//printf("Hour =%d\r\n", e);
			}
			p1 = NULL;
			p1 = strchr(time, 'M');
			if(p1 != NULL)
			{
				if(sscanf(p2,"%*[H]%[0-9]",buffer) != 0)
				{
					if(buffer != NULL)
						Time  = Time + (e = atoi(buffer));
					else
						return 0;
					if(e < 0 || e >59)
						return 0;
					//printf("minute= %d\r\n", e);	
				}
				p2 = NULL;
				p2 = strchr(time, 'S');
				if(p2 != NULL)
				{
					if(sscanf(p1,"%*[M]%[0-9]",buffer) != 0)
					{
					
						if(buffer != NULL)
							Time  = Time + (e = atoi(buffer));
						else
							return 0;
						if(e < 0 || e > 59)
							return 0;
						//printf("seconds =%d\r\n", e);				
					}	
				}				
			}
			else
			{
				p1 = NULL;
				p1 = strchr(time, 'S');
				if(p1 != NULL)
				{
					if(sscanf(p2,"%*[H]%[0-9]",buffer) != 0)
					{
					
						if(buffer != NULL)
							Time  = Time + (e = atoi(buffer));
						else
							return 0;
						if(e < 0 || e > 59)
							return 0;
						//printf("second= %d\r\n", e);				
					}	
				}		
				
			}				
		}
		else
		{	
			p2 = strchr(time, 'M');
			if(p2 != NULL)
			{
				if(sscanf(p1,"%*[T]%[0-9]",buffer) != 0)
				{
					if(buffer != NULL)
						Time  = Time + (e = atoi(buffer));
					else
						return 0;
					if(e < 0 || e >59)
						return 0;
					//printf("Minute =%d\r\n", e);	
				}
				p1 = NULL;
				memset(buffer, 0, 128);
				p1 = strchr(time, 'S');
				if(p1 != NULL)
				{
					if(sscanf(p2,"%*[M]%[0-9]",buffer) != 0)
					{
						if(buffer != NULL)
							Time  = Time + (e = atoi(buffer));
						else
							return 0;
						if(e < 0 || e > 59)
							return 0;
						//printf("Seconds =%d\r\n", e);				
					}	
				}				
			}
			else
			{
				memset(buffer, 0, 128);	
				p2 = strchr(time, 'S');
				if(p2 != NULL)
				{
					if(sscanf(p1,"%*[T]%[0-9]",buffer) != 0)
					{
					
						if(buffer != NULL)
							Time  = Time + (e = atoi(buffer));
						else
							return 0;
						if(e < 0 || e > 59)
							return 0;
						//printf("second =%d\r\n", e);				
					}	
				}		
			}
		}

	}
	else
		return 0;
	#endif
	//printf("All seconds = %d\r\n", Time);
	return Time;
}

int __ns11__Parse_DateTime_Type(char *InitialTerminationTime)
{
	//char* buf = "2041-10-9T12:24:57.321";
	char *buf = InitialTerminationTime;
	char date[128] = {0};
	char time[128] = {0};
	char buffer[128] = {0};	
	char *p1,*p2;
	int e=0;
	int Time;

	struct tm t;
/////////////////////////////////////////////////	
	p2 = NULL;
	p2 = strchr(buf, 'T');
	if(p2 != NULL)
	{	
		if(sscanf(buf, "%[^T]", date) == 0)
			return 0;
		//printf("date=%s\r\n", date);
		if(sscanf(p2, "%*[T]%s", time) == 0)
			return 0;
		//printf("time=%s\r\n", time);
	}
	if(date != NULL)
	{
		
		if(sscanf(date,"%[0-9]",buffer) != 0)
		{
			if(buffer != NULL)
				t.tm_year  = ((e = atoi(buffer)) - 1900);	
			else 
				return 0;
			if(e < 1902 || e > 2037)
				return 0;
			//printf("Year =%d\r\n", e);
		}
		else 
			return 0;
		p1 = NULL;
		p1 = strchr(date,'-');
		if(sscanf(p1,"%*[-]%[0-9]",buffer) != 0)
		{
			if(buffer != NULL)
				t.tm_mon = (e = atoi(buffer));	
			else 
				return 0;
			if(e < 0 || e >11)
						return 0;
			//printf("Month =%d\r\n", e);
		}
		else
			return 0;
		p1 = strchr(p1+1,'-');
		if(sscanf(p1,"%*[-]%[0-9]",buffer) != 0)
		{
			if(buffer != NULL)
				t.tm_mday = (e = atoi(buffer));	
			else 
				return 0;
			if(e < 1 || e > 31)
							return 0;
			//printf("Day =%d\r\n", e);
		}
		else
			return 0;
	}
	else
	 return 0;
	
	if(time != NULL)
	{
		
		if(sscanf(time,"%[0-9]",buffer) != 0)
		{
			if(buffer != NULL)
				t.tm_hour  = (e = atoi(buffer));	
			else 
				return 0;
			if(e < 0 || e > 23)
				return 0;
			//printf("Hour =%d\r\n", e);
		}
		else 
			return 0;
		p1 = NULL;
		p1 = strchr(time,':');
		if(sscanf(p1,"%*[:]%[0-9]",buffer) != 0)
		{
			if(buffer != NULL)
				t.tm_min = (e = atoi(buffer));	
			else 
				return 0;
			if(e < 0 || e > 59)
				return 0;
			//printf("mintues =%d\r\n", e);
		}
		else
			return 0;
		p1 = strchr(p1+1,':');
		if(sscanf(p1,"%*[:]%[0-9]",buffer) != 0)
		{
			if(buffer != NULL)
				t.tm_sec = (e = atoi(buffer));	
			else 
				return 0;
			if(e < 0 || e > 59)
				return 0;
			//printf("Second =%d\r\n", e);
		}
		else
			return 0;
	}
	else
	 return 0;
	
	if((Time = mktime(&t)) == -1)
		return 0;
	else
		return Time;
	
}


int __ns11__Check_TopicFilter(struct soap* soap,  struct wsnt__FilterType *Filter, char *Topic)
{
	if(Filter)
	{
		int i;
		McbXMLResults result;
		McbXMLElement *root, *pEntry;
        //LPCTSTR lpszXML="<wsnt:TopicExpression Dialect=\"http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet\" xmlns:wsnt=\"http://docs.oasis-open.org/wsn/b-2\" xmlns:tns1=\"http://www.onvif.org/ver10/topics\">tns1:VideoAnalytics/MotionDetection</wsnt:TopicExpression>";
        //LPCTSTR lpszXML="<wsnt:TopicExpression Dialect=\"http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet\" xmlns:wsnt=\"http://docs.oasis-open.org/wsn/b-2\">XNj</wsnt:TopicExpression>";
        //LPCTSTR lpszXML="<wsnt:MessageContent Dialect=\"http://www.onvif.org/ver10/tev/messageContentFilter/ItemFilter\" xmlns:wsnt=\"http://docs.oasis-open.org/wsn/b-2\" xmlns:tns1=\"http://www.onvif.org/ver10/schema\">boolean((//tns1:SimpleItem[@Name=\"X\"] )</wsnt:MessageContent>";
		
		for(i=0;i< Filter->__size;i++)
		{	
			//printf("%s\r\n", Filter->__any[i]);	
			//½âÎöXML
            root = McbParseXML(Filter->__any[i], &result);
			if(root==NULL)
                return SOAP_FAULT;//onvif_sender_InvalidTopicExpressionFault_subcode(soap, "ter:InvalidArgVal", "Argument Value Invalid", "InvalidTopicExpressionFault");

                //return onvif_sender_InvalidFilterFault_subcode(soap, "ter:InvalidArgVal", "Argument Value Invalid", "InvalidFilterFault", Filter->__any[i]);
            if((pEntry = McbFindElement(root, "wsnt:TopicExpression")) != NULL)
            {
                McbXMLAttribute *pAttr;
                McbXMLNode *pNode;
                McbXMLText *pText;
                int nIndex;

                nIndex = 0;
                if(pEntry->pEntries != NULL)
                
                while((pNode = McbEnumNodes(pEntry, &nIndex)))
                {	
                    if(pNode->type == eNodeText)
                    {
                        pText = pNode->node.pText;
                        if( pText->lpszValue == NULL)
                            return SOAP_FAULT;;//onvif_sender_InvalidTopicExpressionFault_subcode(soap, "ter:InvalidArgVal", "Argument Value Invalid", "InvalidTopicExpressionFault");
					
                        if(strcmp(pText->lpszValue, "VideoAnalytics/MotionDetection")
                            && strcmp(pText->lpszValue, "VideoSource/VideoLost"))
                            return SOAP_FAULT;//onvif_sender_InvalidTopicExpressionFault_subcode(soap, "ter:InvalidArgVal", "Argument Value Invalid", "TopicNotSupportedFault");
                        printf("\t     Text %s\r\n", pText->lpszValue);
                    }
                    else if(pNode->type == eNodeAttribute)
                    {		
                        pAttr = pNode->node.pAttrib;
                        if (strcmp(pAttr->lpszName, "Dialect") == 0)
                        {
                        	strcpy(Topic,pAttr->lpszValue);
                            if(pAttr->lpszValue==NULL || strncasecmp(pAttr->lpszValue, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet", 58))
                                return SOAP_FAULT;//onvif_sender_InvalidTopicExpressionFault_subcode(soap, "ter:InvalidArgVal", "Argument Value Invalid", "TopicExpressionDialectUnknownFault");
                        	printf("\tAttribute %s=%s\r\n", pAttr->lpszName, pAttr->lpszValue);
                        }	
                    }
                    else if(pNode->type == eNodeElement)
                        printf("\t  Element %s\r\n",pNode->node.pElement->lpszName);
                }
            }
            else if((pEntry = McbFindElement(root, "wsnt:MessageContent"))!=NULL)
            {
                McbXMLAttribute *pAttr;
                McbXMLNode *pNode;
                McbXMLText *pText;
                int nIndex;

                nIndex = 0;
                while((pNode = McbEnumNodes(pEntry, &nIndex)))
                {
                    if(pNode->type==eNodeText)
                    {
                        char *p,*e;
                        pText = pNode->node.pText;
                        if( pText->lpszValue==NULL)
                            return SOAP_FAULT;//onvif_sender_InvalidTopicExpressionFault_subcode(soap, "ter:InvalidArgVal", "Argument Value Invalid", "The Subscribe message contained an unsupported filter");
                        printf("\t     Text %s\r\n", pText->lpszValue);
                        p=strstr(pText->lpszValue, "@Name=\"");
                        if(p==NULL)
                            return SOAP_FAULT;//onvif_sender_InvalidTopicExpressionFault_subcode(soap, "ter:InvalidArgVal", "Argument Value Invalid", "The Subscribe message contained an unsupported filter");
                        p+=7;
                        e = strchr(p, '"');
                        if(e==NULL)
                            return SOAP_FAULT;//onvif_sender_InvalidTopicExpressionFault_subcode(soap, "ter:InvalidArgVal", "Argument Value Invalid", "The Subscribe message contained an unsupported filter");
                        *e = '\0';
                        if(strcmp(p, "VideoSourceConfigurationToken")==0)
                            ;
                        else if(strcmp(p, "VideoSourceToken")==0)
                            ;
                        else
                            return SOAP_FAULT;//onvif_sender_InvalidTopicExpressionFault_subcode(soap, "ter:InvalidArgVal", "Argument Value Invalid", "The Subscribe message contained an unsupported filter");
                            //return onvif_sender_InvalidFilterFault_subcode(soap, "ter:InvalidArgVal", "Argument Value Invalid", "The Subscribe message contained an unsupported filter", "wsnt:MessageContent");
                    }
                    else if(pNode->type==eNodeAttribute)
                    {
                        pAttr = pNode->node.pAttrib;
                        if (strcmp(pAttr->lpszName, "Dialect") == 0)
                            if(pAttr->lpszValue==NULL || strncasecmp(pAttr->lpszValue, "http://www.onvif.org/ver10/tev/messageContentFilter/ItemFilter", 62))
                                return SOAP_FAULT;//onvif_sender_InvalidTopicExpressionFault_subcode(soap, "ter:InvalidArgVal", "Argument Value Invalid", "TopicExpressionDialectUnknownFault");
                        printf("\tAttribute %s=%s\r\n", pAttr->lpszName, pAttr->lpszValue);
                    }
                    else if(pNode->type==eNodeElement)
                        printf("\t  Element %s\r\n",pNode->node.pElement->lpszName);
                }
            }
			
		}
	}
	return SOAP_OK;	
}


int tev_RenewSubscriptionFromManager(tev_SubscriptionManager_t* SubscriptionManager, int id)
{
	if(SubscriptionManager == NULL || id <= 0)
		return -1;
	if (SubscriptionManager->state == 0)
		return -1;


	pthread_mutex_lock(&SubscriptionManager->mutex);
	tev_Subscription_t* Subscription = SubscriptionManager->head;
	while (Subscription)
	{
		if (Subscription->id == id)
			break;
		Subscription = Subscription->next;
	}
	if (Subscription == NULL)
	{
		pthread_mutex_unlock(&SubscriptionManager->mutex);
		return -1;
	}
	
	Subscription->tick = TEV_ALIVE_SEC;
	pthread_mutex_unlock(&SubscriptionManager->mutex);

	return 0;
}

void tev_DelSubsription(tev_Subscription_t* Subscription)
{
	if(Subscription != NULL);
		free(Subscription);
}


int tev_DeleteSubscriptionFromManager(tev_SubscriptionManager_t* SubscriptionManager, int id)
{
	if(SubscriptionManager == NULL || id <= 0)
		return -1;

	if (SubscriptionManager->state == 0)
		return -1;

	pthread_mutex_lock(&SubscriptionManager->mutex);
	
	if (SubscriptionManager->head == NULL)
	{
		pthread_mutex_unlock(&SubscriptionManager->mutex);
		return -1;
	}

	tev_Subscription_t* Subscription = SubscriptionManager->head;
	tev_Subscription_t* prec = NULL;
	if (SubscriptionManager->head->id == id)
	{
		SubscriptionManager->head = SubscriptionManager->head->next;
	}
	else
	{
		while (1)
		{
			prec = Subscription;
			Subscription = Subscription->next;

			if (Subscription == NULL || Subscription->id == id)
				break;
		}
		if (Subscription == NULL)
		{
			pthread_mutex_unlock(&SubscriptionManager->mutex);
			return -1;
		}
		prec->next = Subscription->next;
	}

	tev_DelSubsription(Subscription);
	SubscriptionManager->num --;	
	pthread_mutex_unlock(&SubscriptionManager->mutex);

	return 0;
}

int tev_NotifySubscription(tev_Subscription_t* Subscription)
{
	if(Subscription == NULL)
		return -1;
		
	struct soap soap; 
	struct _wsnt__Notify  Notify;
	time_t   b_time; 
	struct   tm   *tim; 
	
	//static HI_ALARM_INFO_S pAlarmInfo;
	
	soap_init(&soap); 
	soap.recv_timeout = 1;
	soap_default__wsnt__Notify(&soap, &Notify);

	//hi_getalam(&pAlarmInfo); 
	
	Notify.__sizeNotificationMessage = 1;
	
	Notify.NotificationMessage =
		(struct wsnt__NotificationMessageHolderType *)soap_malloc(&soap,sizeof(struct wsnt__NotificationMessageHolderType));

	int i=0,j=123;;
	for(i = 0;i < Notify.__sizeNotificationMessage; i++)
	{
		soap_default_wsnt__NotificationMessageHolderType( &soap, Notify.NotificationMessage+i);
		Notify.NotificationMessage[i].Topic = 
			(struct wsnt__TopicExpressionType *)soap_malloc(&soap,sizeof(struct wsnt__TopicExpressionType));
		soap_default_wsnt__TopicExpressionType(&soap,Notify.NotificationMessage[i].Topic);
		
		Notify.NotificationMessage[i].Topic->Dialect = (char *)soap_malloc(&soap, sizeof(char)*ENDPOINT_ANY_LEN);
		memset(Notify.NotificationMessage[i].Topic->Dialect,0,ENDPOINT_ANY_LEN);
		Notify.NotificationMessage[i].Topic->Dialect = "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet";
		Notify.NotificationMessage[i].Topic->__any = (char *)soap_malloc(&soap, sizeof(char)*ENDPOINT_ANY_LEN);
		memset(Notify.NotificationMessage[i].Topic->__any,0,ENDPOINT_ANY_LEN);
		Notify.NotificationMessage[i].Topic->__any = g_tev_TopicSet[i].topic;//Subscription->topic;
		Notify.NotificationMessage[i].Message.__any = (char *)soap_malloc(&soap,sizeof(char)*ENDPOINT_ANY_LEN);
		memset(Notify.NotificationMessage[i].Message.__any,0,ENDPOINT_ANY_LEN);
		b_time=time(NULL); 
		tim=localtime(&b_time); 
		snprintf(Notify.NotificationMessage[i].Message.__any,ENDPOINT_ANY_LEN-1,
			"<tt:Message UtcTime=\"%d-%02d-%02dT%02d:%02d:%02d.%d\" PropertyOperation=\"Initialized\">"
				"<tt:Source>" 
					"<tt:SimpleItem Name=\"Alarm\" Value=\"Alarm parameters\" />"
				"</tt:Source>"
				"<tt:Data>" 
					"<tt:SimpleItem Name=\"AlarmType\" Value=\"%d\" />"
					"<tt:SimpleItem Name=\"AlarmInPutCh\" Value=\"%d\" />"
				"</tt:Data> "
			"</tt:Message>", (tim->tm_year + 1900), (tim->tm_mon + 1), tim->tm_mday,tim->tm_hour, tim->tm_min, tim->tm_sec, j, 0,//pAlarmInfo.u8AlarmType, 
							 0);//pAlarmInfo.u8AlarmInPutCh);
		j=j+100;
	}
	//printf("\rnotify client ipaddr: %s\r\n", Subscription->addr);
	if (soap_send___tev__Notify(&soap, Subscription->addr, NULL, &Notify))
	{
		soap_print_fault(&soap, stderr); 
	}
	soap_destroy(&soap);	
	soap_end(&soap);	
	soap_done(&soap);
	sleep(1);
	return 0;
}

static int tev_RunSubscriptionManager(void* param)
{
	if(param == NULL)
		return -1;
		
	int j = 0;

	tev_SubscriptionManager_t* SubscriptionManager = (tev_SubscriptionManager_t*)param;
	tev_Subscription_t* Subscription = NULL;

	pthread_detach(pthread_self());
	prctl(PR_SET_NAME, "ONVIFSub");
	while(SubscriptionManager->state)
	{	
		pthread_mutex_lock(&SubscriptionManager->mutex);
		Subscription = SubscriptionManager->head;
		while(Subscription)
		{
			//... get event from device	
			
			//if ((i&0x1) == (Subscription->id&0x1) && Subscription->flag)
			if (Subscription->id > 0 && Subscription->flag)
			{	
				tev_NotifySubscription(Subscription);
			}
			
			Subscription->tick --;
			if (Subscription->tick <= 0)
			{
				j = Subscription->id;
				Subscription = Subscription->next;
				pthread_mutex_unlock(&SubscriptionManager->mutex);
				tev_DeleteSubscriptionFromManager(SubscriptionManager, j);
				pthread_mutex_lock(&SubscriptionManager->mutex);
			}
			Subscription = Subscription->next;
		}
		pthread_mutex_unlock(&SubscriptionManager->mutex);
		sleep(1);
	}

	while (SubscriptionManager->head)
	{
		Subscription = SubscriptionManager->head;
		SubscriptionManager->head = SubscriptionManager->head->next;
		tev_DelSubsription(Subscription);
	}
	
	return 0;
}

static int tev_StartSubscriptionManager(tev_SubscriptionManager_t* SubscriptionManager)
{
	if(SubscriptionManager != NULL)
	{	
		SubscriptionManager->state = 1;
		
    	if (pthread_create(&SubscriptionManager->pid, NULL, (void*)tev_RunSubscriptionManager, (void*)SubscriptionManager) < 0)
    	{
    		perror("pthread_create tev_RunSubscriptionManager");
    		return -1;
    	}
    }
    return 0;
}

tev_Subscription_t* tev_NewSubscription(const char* addr,const char *topic)
{
	static int id = 1;
	tev_Subscription_t* Subscription = (tev_Subscription_t*)malloc(sizeof(tev_Subscription_t));
	Subscription->id = id++;
	Subscription->tick = TEV_ALIVE_SEC;
	strncpy(Subscription->topic,topic,LINE_SIZE);
	
	if(addr)
	{
		strncpy(Subscription->addr, addr, DEV_URI_LEN-1);
		Subscription->flag = 1;
	}
	else
	{
		memset(Subscription->addr,0,DEV_URI_LEN);
		Subscription->flag = 0; //pullpoint
	}
	return Subscription;
}



int tev_InsertSubscriptionToManager(tev_SubscriptionManager_t* SubscriptionManager, const char* addr,const char* topic)
{
	if(SubscriptionManager == NULL )
		return -1;

	if (SubscriptionManager->state == 0)
		return -1;

	int ret = 0;
	
	pthread_mutex_lock(&SubscriptionManager->mutex);
	tev_Subscription_t* Subscription = tev_NewSubscription(addr,topic);
	ret = Subscription->id;
	Subscription->next = SubscriptionManager->head;
	SubscriptionManager->head = Subscription;
	SubscriptionManager->num ++;
	pthread_mutex_unlock(&SubscriptionManager->mutex);


//	printf("tev_InsertSubscriptionToManager(%d)\n", ret);

	return ret;
}


int tev_Init()
{
	g_tev_TopicNamespaceLocation = (char**)malloc(TEV_TNL_SIZE);
	if(g_tev_TopicNamespaceLocation == NULL)
	{
		printf("Failed to malloc for g_tev_TopicNamespaceLocation.\n");
	}
	g_tev_TopicNamespaceLocation[0] = (char*)malloc(TEV_URI_LEN);
	if(g_tev_TopicNamespaceLocation[0] == NULL)
	{
		printf("Failed to malloc for g_tev_TopicNamespaceLocation[0].\n");
	}
	g_tev_TopicNamespaceLocationNum = 1;
	strncpy(g_tev_TopicNamespaceLocation[0], "http://www.onvif.org/ver10/topics", DEV_URI_LEN-1);

	g_tev_TopicSet = (tev_topic_t*)malloc(sizeof(tev_topic_t)*TEV_TS_SIZE);
	if(g_tev_TopicSet == NULL)
	{
		printf("Failed to malloc for g_tev_TopicSet.\n");
	}
	g_tev_TopicNum = 2;
	
	memset(g_tev_TopicSet, 0 ,sizeof(tev_topic_t)*TEV_TS_SIZE);
	strncpy(g_tev_TopicSet[0].topic, "tt:VideoAnalytics", DEV_STR_LEN-1);
	strncpy(g_tev_TopicSet[0].source.name, "VideoSourceConfigurationToken", DEV_STR_LEN-1);
	strncpy(g_tev_TopicSet[0].source.type, "tt:ReferenceToken", DEV_STR_LEN-1);
	strncpy(g_tev_TopicSet[0].data.name, "MotionActive", DEV_STR_LEN-1);
	strncpy(g_tev_TopicSet[0].data.type, "xsd:boolean", DEV_STR_LEN-1);
	strncpy(g_tev_TopicSet[0].key.name, "any", DEV_STR_LEN-1);
	strncpy(g_tev_TopicSet[0].key.type, "any", DEV_STR_LEN-1);
	
	memcpy(g_tev_TopicSet[1].topic, "tt:VideoLost", DEV_STR_LEN-1);
	memcpy(g_tev_TopicSet[1].source.name, "VideolostToken", DEV_STR_LEN-1);
	memcpy(g_tev_TopicSet[1].source.type, "tt:ReferenceToken", DEV_STR_LEN-1);
	memcpy(g_tev_TopicSet[1].data.name, "MotionActive", DEV_STR_LEN-1);
	memcpy(g_tev_TopicSet[1].data.type, "xsd:boolean", DEV_STR_LEN-1);
	memcpy(g_tev_TopicSet[1].key.name, "any", DEV_STR_LEN-1);
	memcpy(g_tev_TopicSet[1].key.type, "any", DEV_STR_LEN-1);

	g_tev_TopicExpressionDialect = (char**)malloc(TEV_TED_SIZE);
	if(g_tev_TopicExpressionDialect == NULL)
	{
		printf("Failed to malloc for g_tev_TopicExpressionDialect.\n");
	}
	g_tev_TopicExpressionDialect[0] =(char*)malloc(DEV_URI_LEN);
	if(g_tev_TopicExpressionDialect[0] == NULL)
	{
		printf("Failed to malloc for g_tev_TopicExpressionDialect[0].\n");
	}
	g_tev_TopicExpressionDialect[1] =(char*)malloc(DEV_URI_LEN);
	if(g_tev_TopicExpressionDialect[1] == NULL)
	{
		printf("Failed to malloc for g_tev_TopicExpressionDialect[1].\n");
	}
	g_tev_TopicExpressionDialectNum = 2;
	strncpy(g_tev_TopicExpressionDialect[0], "http://docs.oasis-open.org/wsn/t-1/TopicExpression/Concrete", DEV_URI_LEN-1);
	strncpy(g_tev_TopicExpressionDialect[1], "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet", DEV_URI_LEN-1);

	g_tev_MessageContentFilterDialect = (char**)malloc(TEV_MCFD_SIZE);
	if(g_tev_MessageContentFilterDialect == NULL)
	{
		printf("Failed to malloc for g_tev_MessageContentFilterDialect.\n");
	}
	g_tev_MessageContentFilterDialect[0] = (char*)malloc(DEV_URI_LEN);
	if(g_tev_MessageContentFilterDialect[0] == NULL)
	{
		printf("Failed to malloc for g_tev_MessageContentFilterDialect[0].\n");
	}
	g_tev_MessageContentFilterDialectNum = 1;
	strncpy(g_tev_MessageContentFilterDialect[0], "http://www.onvif.org/ver10/tev/messageContentFilter/ItemFilter", DEV_URI_LEN-1);

	g_tev_MessageContentSchemaLocation = (char**)malloc(TEV_MCSL_SIZE);
	if(g_tev_MessageContentSchemaLocation == NULL)
	{
		printf("Failed to malloc for g_tev_MessageContentSchemaLocation.\n");
	}
	g_tev_MessageContentSchemaLocation[0] = (char*)malloc(DEV_URI_LEN);
	if(g_tev_MessageContentSchemaLocation[0] == NULL)
	{
		printf("Failed to malloc for g_tev_MessageContentSchemaLocation[0].\n");
	}
	g_tev_MessageContentSchemaLocationNum = 1;
	strncpy(g_tev_MessageContentSchemaLocation[0], "", DEV_URI_LEN-1);
	
	memset(&g_tev_SubscriptionManager, 0, sizeof(tev_SubscriptionManager_t));
	memset(&PullPoint_TimeOut, 0 ,sizeof(int));
	memset(&ALAM_FLAG, 0 ,sizeof(int));
	//memset(&pAlarmInfo_onvif, 0 ,sizeof(HI_ALARM_INFO_S));
	pthread_mutex_init(&g_tev_SubscriptionManager.mutex, NULL);
	tev_StartSubscriptionManager(&g_tev_SubscriptionManager);
	
	return 0;
}

