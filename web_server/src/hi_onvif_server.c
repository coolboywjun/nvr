#include <sys/prctl.h>
#include <pthread.h>
#include "net_api.h"
#include "DeviceBinding.nsmap"
#include "hi_onvif.h"
#include "hi_onvif_proc.h"
#include "onvifface.h"
#include "nvr_struct.h"
#include "McbXML.h"
#include "nvr_net_config.h"
#include "hi_onvif_subscribe.h"
#include "hi_platform_onvif.h"
#include "nvr_lib_interface.h"

#define ONVIF_MAJOR 2
#define ONVIF_MINOR 2

#define ETH_NAME "eth0"
#define ETH_NAME1 "ra0"




#define ALLOC_STRUCT(val, type) {\
       val = (type *) soap_malloc(soap, sizeof(type)); \
	if(val == NULL) \
	{\
		printf("malloc err\r\n");\
		return SOAP_FAULT;\
	}\
	memset(val, 0, sizeof(type)); \
}


#define ALLOC_STRUCT_INT(val, type, ret)  {\
       val = (type *) soap_malloc(soap, sizeof(type)); \
	if(val == NULL) \
	{\
		printf("malloc err\r\n");\
		return SOAP_FAULT;\
	}\
	*val = ret;\
}

#define ALLOC_STRUCT_NUM(val, type, num) {\
       val = (type *) soap_malloc(soap, sizeof(type)*num); \
	if(val == NULL) \
	{\
		printf("malloc err\r\n");\
		return SOAP_FAULT;\
	}\
	memset(val, 0, sizeof(type)*num); \
}

#define ALLOC_TOKEN(val, token) {\
	val = (char *) soap_malloc(soap, sizeof(char)*ONVIF_TOKEN_LEN); \
	if(val == NULL) \
	{\
		printf("malloc err\r\n");\
		return SOAP_FAULT;\
	}\
	memset(val, 0, sizeof(char)*ONVIF_TOKEN_LEN); \
	strcpy(val, token); \
	}


#define CHECK_USER { \
int ret = onvif_check_security(soap); \
if(ret != SOAP_OK) \
	return ret; \
}


int __tan__GetServiceCapabilities(struct soap* soap, struct _tan__GetServiceCapabilities *tan__GetServiceCapabilities, struct _tan__GetServiceCapabilitiesResponse *tan__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
	__FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

int  __tan__GetSupportedAnalyticsModules(struct soap* soap, struct _tan__GetSupportedAnalyticsModules *tan__GetSupportedAnalyticsModules, struct _tan__GetSupportedAnalyticsModulesResponse *tan__GetSupportedAnalyticsModulesResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoConfig ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}
 int  __tan__CreateAnalyticsModules(struct soap* soap, struct _tan__CreateAnalyticsModules *tan__CreateAnalyticsModules, struct _tan__CreateAnalyticsModulesResponse *tan__CreateAnalyticsModulesResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoConfig ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tan__DeleteAnalyticsModules(struct soap* soap, struct _tan__DeleteAnalyticsModules *tan__DeleteAnalyticsModules, struct _tan__DeleteAnalyticsModulesResponse *tan__DeleteAnalyticsModulesResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoConfig ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tan__GetAnalyticsModules(struct soap* soap, struct _tan__GetAnalyticsModules *tan__GetAnalyticsModules, struct _tan__GetAnalyticsModulesResponse *tan__GetAnalyticsModulesResponse)
{
	onvif_fault(soap,"ter:Action", "ter:AnalyticsModuleNotSupported ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tan__ModifyAnalyticsModules(struct soap* soap, struct _tan__ModifyAnalyticsModules *tan__ModifyAnalyticsModules, struct _tan__ModifyAnalyticsModulesResponse *tan__ModifyAnalyticsModulesResponse)
{
	onvif_fault(soap,"ter:Action", "ter:AnalyticsModuleNotSupported ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tan__GetSupportedRules(struct soap* soap, struct _tan__GetSupportedRules *tan__GetSupportedRules, struct _tan__GetSupportedRulesResponse *tan__GetSupportedRulesResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoConfig ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tan__CreateRules(struct soap* soap, struct _tan__CreateRules *tan__CreateRules, struct _tan__CreateRulesResponse *tan__CreateRulesResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoConfig ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tan__DeleteRules(struct soap* soap, struct _tan__DeleteRules *tan__DeleteRules, struct _tan__DeleteRulesResponse *tan__DeleteRulesResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoConfig ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tan__GetRules(struct soap* soap, struct _tan__GetRules *tan__GetRules, struct _tan__GetRulesResponse *tan__GetRulesResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoConfig ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tan__ModifyRules(struct soap* soap, struct _tan__ModifyRules *tan__ModifyRules, struct _tan__ModifyRulesResponse *tan__ModifyRulesResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoConfig ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tev__CreatePullPoint(struct soap* soap, struct _wsnt__CreatePullPoint *wsnt__CreatePullPoint, struct _wsnt__CreatePullPointResponse *wsnt__CreatePullPointResponse)
{
   __FUN_BEGIN("Temperarily no process!\r\n");
   __FUN_END("Temperarily no process!\r\n");
   return SOAP_FAULT;	
}

 int  __tev__GetServiceCapabilities(struct soap* soap, struct _tev__GetServiceCapabilities *tev__GetServiceCapabilities, struct _tev__GetServiceCapabilitiesResponse *tev__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tev__CreatePullPointSubscription(struct soap* soap, struct _tev__CreatePullPointSubscription *tev__CreatePullPointSubscription, struct _tev__CreatePullPointSubscriptionResponse *tev__CreatePullPointSubscriptionResponse)
{
	return SOAP_FAULT;
#if 0	
	 int nSubscribe;
    time_t nTimeOut = -1;
    char TopicExpression[256] = {0};
	
	struct _tev__CreatePullPointSubscription *ns1__CreatePullPointSubscription;
	struct _tev__CreatePullPointSubscriptionResponse *ns1__CreatePullPointSubscriptionResponse;
	__FUN_BEGIN("\r\n");
	ns1__CreatePullPointSubscription = tev__CreatePullPointSubscription;
	ns1__CreatePullPointSubscriptionResponse = tev__CreatePullPointSubscriptionResponse;
	if(tev__CreatePullPointSubscription->InitialTerminationTime != NULL)
	{
		PullPoint_TimeOut = __ns11__Parse_Duration_Type(tev__CreatePullPointSubscription->InitialTerminationTime);
	}
	
    if(__ns11__Check_TopicFilter(soap, ns1__CreatePullPointSubscription->Filter, TopicExpression) != SOAP_OK)
        return SOAP_FAULT;

    if(ns1__CreatePullPointSubscription->InitialTerminationTime)
    {
		int InitialTerminationTime;

        if((InitialTerminationTime = __ns11__Parse_Duration_Type(ns1__CreatePullPointSubscription->InitialTerminationTime)))
        {
            time_t nowtime = time(NULL);
            ns1__CreatePullPointSubscriptionResponse->wsnt__CurrentTime = nowtime;
            ns1__CreatePullPointSubscriptionResponse->wsnt__TerminationTime = nowtime + InitialTerminationTime;
            nTimeOut = InitialTerminationTime;
        }
        else if((InitialTerminationTime = __ns11__Parse_DateTime_Type(ns1__CreatePullPointSubscription->InitialTerminationTime)))
        {
            time_t nowtime = time(NULL);
            if(InitialTerminationTime>nowtime)
            {
               ns1__CreatePullPointSubscriptionResponse->wsnt__CurrentTime = nowtime;
                ns1__CreatePullPointSubscriptionResponse->wsnt__TerminationTime = InitialTerminationTime;
                nTimeOut = InitialTerminationTime - nowtime;
            }
            else{
                return SOAP_FAULT;//onvif_sender_UnacceptableInitialTerminationTimeFault_subcode(soap, "ter:InvalidArgVal", "Argument Value Invalid", "UnacceptableInitialTerminationTime");
            }
        }
    }
    //动态创建SubscriptionManager
   nSubscribe = tev_InsertSubscriptionToManager(&g_tev_SubscriptionManager, NULL, TopicExpression);
	   if(nSubscribe)
	   {
		   char localIP[128]; 
		   unsigned long ip;
		   ip = onvif_get_ipaddr(soap);
		   net_ip_n2a(ip, localIP, 128);
		   //hi_netcfg_get_ip_addr("eth0", localIP);
		   //printf("ip= %s\r\n", localIP);
		   ns1__CreatePullPointSubscriptionResponse->SubscriptionReference.Address = soap_malloc(soap, sizeof(char)*64);
		   sprintf(ns1__CreatePullPointSubscriptionResponse->SubscriptionReference.Address, 
				   "http://%s:8000/onvif/event_service?subscribe=%d", 
				   localIP, nSubscribe);
		   if (soap->header)
		   {
			   soap_default_SOAP_ENV__Header(soap, soap->header);
			   soap->header->wsa5__Action = (char*)"http://www.onvif.org/ver10/events/wsdl/EventPortType/CreatePullPointSubscriptionResponse";
		   }
	   }
	   else
		   return SOAP_FAULT;//onvif_receiver_SubscribeCreationFailedFault_subcode(soap, "ter:OutofMemory", "Out of Memory", "SubscribeCreationFailed");
		  	__FUN_END("\r\n");
#endif			
	   return SOAP_OK;
}

 int  __tev__GetEventProperties(struct soap* soap, struct _tev__GetEventProperties *tev__GetEventProperties, struct _tev__GetEventPropertiesResponse *tev__GetEventPropertiesResponse)
{
	return SOAP_FAULT;	
	__FUN_BEGIN("\r\n");

	tev__GetEventPropertiesResponse->__sizeTopicNamespaceLocation = g_tev_TopicNamespaceLocationNum;
    tev__GetEventPropertiesResponse->TopicNamespaceLocation = g_tev_TopicNamespaceLocation;
    tev__GetEventPropertiesResponse->wsnt__FixedTopicSet = xsd__boolean__false_;

    tev__GetEventPropertiesResponse->wstop__TopicSet = (struct wstop__TopicSetType *)soap_malloc(soap, sizeof(struct wstop__TopicSetType));
    tev__GetEventPropertiesResponse->wstop__TopicSet->documentation = NULL;
	tev__GetEventPropertiesResponse->wstop__TopicSet->__size = g_tev_TopicNum;
	tev__GetEventPropertiesResponse->wstop__TopicSet->__any = (char**)soap_malloc(soap, sizeof(char*)*g_tev_TopicNum);
	tev__GetEventPropertiesResponse->wstop__TopicSet->__anyAttribute = NULL;

	int i = 0;
	for (i = 0; i < g_tev_TopicNum; i++)
	{
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any[i] = (char*)soap_malloc(soap, DEV_LINE_LEN);
		memset(tev__GetEventPropertiesResponse->wstop__TopicSet->__any[i], 0, DEV_LINE_LEN);
		snprintf(tev__GetEventPropertiesResponse->wstop__TopicSet->__any[i], DEV_LINE_LEN-1,
				"<%s wstop:topic=\"true\">"
					"<tt:MessageDescription>"
						"<tt:Source>"
							"<tt:SimpleItem Name=\"%s\" Type=\"%s\" />"
						"</tt:Source>"
						"<tt:Data>"
							"<tt:SimpleItem Name=\"%s\" Type=\"%s\" />"
						"</tt:Data>"
						"<tt:Key>"
							"<tt:SimpleItem Name=\"%s\" Type=\"%s\" />"
						"</tt:Key>"						
					"</tt:MessageDescription>"
			"</%s>",
			g_tev_TopicSet[i].topic,
			g_tev_TopicSet[i].source.name, g_tev_TopicSet[i].source.type,
			g_tev_TopicSet[i].data.name, g_tev_TopicSet[i].data.type,
			g_tev_TopicSet[i].key.name, g_tev_TopicSet[i].key.type,
			g_tev_TopicSet[i].topic);
	}
															 
    tev__GetEventPropertiesResponse->__sizeTopicExpressionDialect = g_tev_TopicExpressionDialectNum;
    tev__GetEventPropertiesResponse->wsnt__TopicExpressionDialect = g_tev_TopicExpressionDialect;
    tev__GetEventPropertiesResponse->__sizeMessageContentFilterDialect = g_tev_MessageContentFilterDialectNum;
    tev__GetEventPropertiesResponse->MessageContentFilterDialect = g_tev_MessageContentFilterDialect;
    tev__GetEventPropertiesResponse->__sizeProducerPropertiesFilterDialect = 0;
    tev__GetEventPropertiesResponse->ProducerPropertiesFilterDialect = NULL;
    tev__GetEventPropertiesResponse->__sizeMessageContentSchemaLocation = g_tev_MessageContentSchemaLocationNum;
    tev__GetEventPropertiesResponse->MessageContentSchemaLocation = g_tev_MessageContentSchemaLocation;
	tev__GetEventPropertiesResponse->__size = 0;
	tev__GetEventPropertiesResponse->__any = NULL;

    if (soap->header)
    {
        soap_default_SOAP_ENV__Header(soap, soap->header);
        soap->header->wsa5__Action = (char*)"http://www.onvif.org/ver10/events/wsdl/EventPortType/GetEventPropertiesResponse";
    }
    
 		__FUN_END("\r\n");
	
    return SOAP_OK;
}

 int  __tev__Notify_(struct soap* soap, struct _wsnt__Notify *wsnt__Notify)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tev__Subscribe(struct soap* soap, struct _wsnt__Subscribe *wsnt__Subscribe, struct _wsnt__SubscribeResponse *wsnt__SubscribeResponse)
{
	return SOAP_FAULT;
#if 0	
	__FUN_BEGIN("\n");

	char TopicExpression[256] = {0};
	int nSubscribe;
	time_t nTimeOut = -1;

	if(wsnt__Subscribe != NULL)
	{
		if(wsnt__Subscribe->Filter != NULL)
		{
			if(__ns11__Check_TopicFilter(soap, wsnt__Subscribe->Filter, TopicExpression) != SOAP_OK)
				return SOAP_FAULT;
			if(TopicExpression == NULL)
				return SOAP_FAULT;
		}
			
	}
	if(wsnt__Subscribe->InitialTerminationTime)
	{
		int InitialTerminationTime;
		if((InitialTerminationTime = __ns11__Parse_Duration_Type(wsnt__Subscribe->InitialTerminationTime)))
        {
            time_t nowtime = time(NULL);
            wsnt__SubscribeResponse->CurrentTime = (time_t*)soap_malloc(soap, sizeof(time_t)*2);
            wsnt__SubscribeResponse->TerminationTime = wsnt__SubscribeResponse->CurrentTime +1;
            *wsnt__SubscribeResponse->CurrentTime = nowtime;
            *wsnt__SubscribeResponse->TerminationTime = nowtime + InitialTerminationTime;
            nTimeOut = InitialTerminationTime;
        }
        else if((InitialTerminationTime = __ns11__Parse_DateTime_Type(wsnt__Subscribe->InitialTerminationTime)))
        {
            time_t nowtime = time(NULL);
            if(InitialTerminationTime > nowtime)
            {
                wsnt__SubscribeResponse->CurrentTime = (time_t*)soap_malloc(soap, sizeof(time_t)*2);
                wsnt__SubscribeResponse->TerminationTime = wsnt__SubscribeResponse->CurrentTime +1;
                *wsnt__SubscribeResponse->CurrentTime = nowtime;
                *wsnt__SubscribeResponse->TerminationTime = InitialTerminationTime;
                nTimeOut = InitialTerminationTime - nowtime;
            }
            else
				{
                	return SOAP_FAULT;//return onvif_fault(soap, "ter:InvalidArgVal", "Argument Value Invalid");//onvif_sender_UnacceptableInitialTerminationTimeFault_subcode(soap, "ter:InvalidArgVal", "Argument Value Invalid", "UnacceptableInitialTerminationTime");
            }
        }		
}
	
	nSubscribe = tev_InsertSubscriptionToManager(&g_tev_SubscriptionManager, wsnt__Subscribe->ConsumerReference.Address, TopicExpression);
	if(nSubscribe)
	{
		char localIP[128]; 
		unsigned long ip;
		ip = onvif_get_ipaddr(soap);
		net_ip_n2a(ip, localIP, 128);
		//hi_netcfg_get_ip_addr("eth0", localIP);
		//printf("ip= %s\r\n", localIP);
		wsnt__SubscribeResponse->SubscriptionReference.Address = soap_malloc(soap, sizeof(char)*64);
		sprintf(wsnt__SubscribeResponse->SubscriptionReference.Address, 
		"http://%s:8000/onvif/event_service?subscribe=%d", 
		localIP, nSubscribe);
				   
		if (soap->header)
		{
			soap_default_SOAP_ENV__Header(soap, soap->header);
			soap->header->wsa5__Action = (char*)"http://docs.oasis-open.org/wsn/bw-2/NotificationProducer/SubscribeResponse";
		}
	  }
	  else
		return SOAP_FAULT;//onvif_receiver_SubscribeCreationFailedFault_subcode(soap, "ter:OutofMemory", "Out of Memory", "SubscribeCreationFailed");

	__FUN_END("\n");
	#endif
	return SOAP_OK;
}

 int  __tev__GetCurrentMessage(struct soap* soap, struct _wsnt__GetCurrentMessage *wsnt__GetCurrentMessage, struct _wsnt__GetCurrentMessageResponse *wsnt__GetCurrentMessageResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tev__GetMessages(struct soap* soap, struct _wsnt__GetMessages *wsnt__GetMessages, struct _wsnt__GetMessagesResponse *wsnt__GetMessagesResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tev__DestroyPullPoint(struct soap* soap, struct _wsnt__DestroyPullPoint *wsnt__DestroyPullPoint, struct _wsnt__DestroyPullPointResponse *wsnt__DestroyPullPointResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tev__Notify(struct soap* soap, struct _wsnt__Notify *wsnt__Notify)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tev__PullMessages(struct soap* soap, struct _tev__PullMessages *tev__PullMessages, struct _tev__PullMessagesResponse *tev__PullMessagesResponse)
{
	return SOAP_FAULT;	
	//tev__PullMessages->MessageLimit  限制response里面返回的message数量
	__FUN_BEGIN("\n");

	tev__PullMessagesResponse->CurrentTime = time(NULL);
	tev__PullMessagesResponse->TerminationTime = tev__PullMessagesResponse->CurrentTime + TEV_ALIVE_SEC;

	tev__PullMessagesResponse->__sizeNotificationMessage = 2;//if == 2 error

	
	tev__PullMessagesResponse->wsnt__NotificationMessage =
		(struct wsnt__NotificationMessageHolderType *)soap_malloc(soap,sizeof(struct wsnt__NotificationMessageHolderType)*tev__PullMessagesResponse->__sizeNotificationMessage);
	
	
	int i=0,j=123;;
	for(i = 0;i<tev__PullMessagesResponse->__sizeNotificationMessage;i++)
	{
		soap_default_wsnt__NotificationMessageHolderType( soap,	tev__PullMessagesResponse->wsnt__NotificationMessage+i);
		tev__PullMessagesResponse->wsnt__NotificationMessage[i].Topic = 
			(struct wsnt__TopicExpressionType *)soap_malloc(soap,sizeof(struct wsnt__TopicExpressionType));
		soap_default_wsnt__TopicExpressionType(soap,tev__PullMessagesResponse->wsnt__NotificationMessage[i].Topic);
		tev__PullMessagesResponse->wsnt__NotificationMessage[i].Topic->Dialect = "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet";
		tev__PullMessagesResponse->wsnt__NotificationMessage[i].Topic->__any = "tt:VideoAnalytics";

		tev__PullMessagesResponse->wsnt__NotificationMessage[i].Message.__any = (char *)soap_malloc(soap,sizeof(char)*ENDPOINT_ANY_LEN);
		memset(tev__PullMessagesResponse->wsnt__NotificationMessage[i].Message.__any,0,ENDPOINT_ANY_LEN);
		
		snprintf(tev__PullMessagesResponse->wsnt__NotificationMessage[i].Message.__any,ENDPOINT_ANY_LEN-1,
			"<tt:Message UtcTime=\"2012-07-07T15:00:00.%d\" PropertyOperation=\"Initialized\">"
				"<tt:Source>" 
					"<tt:SimpleItem Name=\"VideoSourceConfigurationToken\" Value=\"1\" />"
					"<tt:SimpleItem Name=\"VideoAnalyticsConfigurationToken\" Value=\"2\" />"
					"<tt:SimpleItem Name=\"Rule\" Value=\"MyImportantFence1\" />"
				"</tt:Source>"
				"<tt:Data>" 
					"<tt:SimpleItem Name=\"IsInside\" Value=\"false\" />"
				"</tt:Data> "
			"</tt:Message>",j);
		j=j+100;
	}
	
 	if(soap->header)
 	{
		 soap_default_SOAP_ENV__Header(soap, soap->header);
		soap->header->wsa5__Action =  (char *)"http://www.onvif.org/ver10/events/wsdl/PullPointSubscription/PullMessagesResponse";
 	}
	
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tev__SetSynchronizationPoint(struct soap* soap, struct _tev__SetSynchronizationPoint *tev__SetSynchronizationPoint, struct _tev__SetSynchronizationPointResponse *tev__SetSynchronizationPointResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tev__Renew(struct soap* soap, struct _wsnt__Renew *wsnt__Renew, struct _wsnt__RenewResponse *wsnt__RenewResponse)
{
	return SOAP_FAULT;	
	__FUN_BEGIN("\n");
	//printf("__depsm__Renew\n");
	if (soap->header == NULL || soap->header->wsa5__To == NULL)
		return SOAP_FAULT;
	char* tmp = strstr(soap->header->wsa5__To, "?subscribe=");
	if (tmp == NULL)
		return SOAP_FAULT;
	tmp += strlen("?subscribe=");
	int id = atoi(tmp);
	//printf("__ns11__Renew id(%d)\n", id);
	if (tev_RenewSubscriptionFromManager(&g_tev_SubscriptionManager, id) < 0)
		return SOAP_FAULT;
	
	wsnt__RenewResponse->CurrentTime = (time_t*)soap_malloc(soap, sizeof(time_t));
	*wsnt__RenewResponse->CurrentTime = time(NULL);
	if(wsnt__Renew->TerminationTime && PullPoint_TimeOut)
	{
		if(__ns11__Parse_Duration_Type(wsnt__Renew->TerminationTime) < PullPoint_TimeOut)
		{
			PullPoint_TimeOut = 0;
			return SOAP_FAULT;
		}
		PullPoint_TimeOut = 0;
	}
		
	wsnt__RenewResponse->TerminationTime = *wsnt__RenewResponse->CurrentTime + TEV_ALIVE_SEC;

	if (soap->header)
	{
		soap_default_SOAP_ENV__Header(soap, soap->header);
		soap->header->wsa5__Action = (char*)"http://docs.oasis-open.org/wsn/bw-2/SubscriptionManager/RenewResponse";
	}
#if 0
	time_t time_tm;
	time_t termtime_tm;
	time_t timeout;
	
	timeout = periodtol(ns6__Renew->TerminationTime);
	time_out += timeout;
	t_out += timeout;
	time_tm = time(NULL);
	termtime_tm = time_tm + timeout;
	wsnt__RenewResponse->TerminationTime = termtime_tm;
	wsnt__RenewResponse->CurrentTime = (time_t *) soap_malloc(soap, sizeof(time_t)); 
	if(wsnt__RenewResponse->CurrentTime == NULL)
	{
		__E("Failed to malloc for currentTime.\n");
		return SOAP_FAULT;
	}
	wsnt__RenewResponse->CurrentTime[0] = time_tm; 
	wsnt__RenewResponse->__size = 0;
	wsnt__RenewResponse->__any = NULL;
#endif
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tev__Unsubscribe(struct soap* soap, struct _wsnt__Unsubscribe *wsnt__Unsubscribe, struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse)
{
	return SOAP_FAULT;	
	__FUN_BEGIN("\n");
	if (soap->header == NULL || soap->header->wsa5__To == NULL)
		return SOAP_FAULT;
	
	char* tmp = strstr(soap->header->wsa5__To, "?subscribe=");
	if (tmp == NULL)
		return SOAP_FAULT;
	tmp += strlen("?subscribe=");
	int id = atoi(tmp);
	//printf("__depsm__Unsubscribe id(%d)\n", id);
	if (tev_DeleteSubscriptionFromManager(&g_tev_SubscriptionManager, id) < 0)
		return SOAP_FAULT;
	if (soap->header)
	{
		soap_default_SOAP_ENV__Header(soap, soap->header);
		soap->header->wsa5__Action = (char*)"http://docs.oasis-open.org/wsn/bw-2/SubscriptionManager/UnsubscribeResponse";
	}
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tev__PauseSubscription(struct soap* soap, struct _wsnt__PauseSubscription *wsnt__PauseSubscription, struct _wsnt__PauseSubscriptionResponse *wsnt__PauseSubscriptionResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tev__ResumeSubscription(struct soap* soap, struct _wsnt__ResumeSubscription *wsnt__ResumeSubscription, struct _wsnt__ResumeSubscriptionResponse *wsnt__ResumeSubscriptionResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tev__Renew_(struct soap* soap, struct _wsnt__Renew *wsnt__Renew, struct _wsnt__RenewResponse *wsnt__RenewResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tev__Unsubscribe_(struct soap* soap, struct _wsnt__Unsubscribe *wsnt__Unsubscribe, struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}
 
 int put_soap_header_wsa(struct soap* soap)
 {
	 time_t time_n;
	 struct tm *tm_t, tm;
	 char msgid[LARGE_INFO_LENGTH];
	 char macaddr[MACH_ADDR_LENGTH];	 
	 //soap->header->wsa__To = "http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous";
	 ALLOC_TOKEN(soap->header->wsa__To, "http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous");
	 ALLOC_TOKEN(soap->header->wsa__Action, "http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches");
	 ALLOC_STRUCT(soap->header->wsa__RelatesTo, struct wsa__Relationship);
	 ALLOC_TOKEN(soap->header->wsa__RelatesTo->__item, soap->header->wsa__MessageID);
	 soap->header->wsa__RelatesTo->RelationshipType = NULL;
	 soap->header->wsa__RelatesTo->__anyAttribute = NULL;
	 unsigned long ip;
	 ip = onvif_get_ipaddr(soap);
	 onvif_get_mac(ip, macaddr);
	 time_n = time(NULL);
	 tm_t=localtime_r(&time_n, &tm);
	 sprintf(msgid,"uuid:1319d68a-%d%d%d-%d%d-%d%d-%02X%02X%02X%02X%02X%02X",
			 tm_t->tm_wday,tm_t->tm_mday,tm_t->tm_mon,
			 tm_t->tm_year,tm_t->tm_hour,tm_t->tm_min,
			 tm_t->tm_sec,macaddr[0], macaddr[1], macaddr[2], 
			 macaddr[3], macaddr[4], macaddr[5]);
	 ALLOC_TOKEN(soap->header->wsa__MessageID, msgid)
	 return 0;
 }
 
 int put_soap_header_wsa5(struct soap* soap)
 {
	  time_t time_n;
	 struct tm *tm_t, tm;
	 char msgid[LARGE_INFO_LENGTH];
	 char macaddr[MACH_ADDR_LENGTH];
	 
	 soap->header->wsa5__To = "http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous";
	 soap->header->wsa5__Action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches";
	 soap->header->wsa5__RelatesTo = (struct wsa5__RelatesToType*)soap_malloc(soap, sizeof(struct wsa5__RelatesToType));
	 if(soap->header->wsa5__RelatesTo == NULL)
		 return soap->error;
	 soap->header->wsa5__RelatesTo->__item = soap->header->wsa5__MessageID;
	 soap->header->wsa5__RelatesTo->RelationshipType = NULL;
	 soap->header->wsa5__RelatesTo->__anyAttribute = NULL;
	 unsigned long ip;
	 ip = onvif_get_ipaddr(soap);
	 onvif_get_mac(ip, macaddr);
	 time_n = time(NULL);
	 tm_t=localtime_r(&time_n, &tm);
	 sprintf(msgid,"uuid:1319d68a-%d%d%d-%d%d-%d%d-%02X%02X%02X%02X%02X%02X",
			 tm_t->tm_wday,tm_t->tm_mday,tm_t->tm_mon,
			 tm_t->tm_year,tm_t->tm_hour,tm_t->tm_min,
			 tm_t->tm_sec,macaddr[0], macaddr[1], macaddr[2], 
			 macaddr[3], macaddr[4], macaddr[5]);
	 soap->header->wsa5__MessageID =(char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	 if(soap->header->wsa5__MessageID == NULL)
		 return soap->error;
	 strcpy(soap->header->wsa5__MessageID,msgid);
	 
	  return 0;
 }
 
 int  __dn__Probe(struct soap* soap, struct d__ProbeType *dn__Probe, struct d__ProbeMatchesType *dn__ProbeResponse)
 {
	 __FUN_BEGIN("\n");
	 
	 //struct d__ProbeType *ns1__Probe;
	 struct d__ProbeMatchesType *ns1__ProbeResponse;
 
	 int nIpAddr = 0;
	 NVR_DEV_NET_CFG_S stNetCfg;
	 nvr_logon_get_net_cfg(0, &stNetCfg);	 
	 if(soap->master == stOnvifSocket.eth0Socket)
	 {
	 	nIpAddr = stNetCfg.struEtherCfg[0].u32IpAddr;
	 }
	 else if(soap->master == stOnvifSocket.wifiSocket)
	 	nIpAddr = stNetCfg.struEtherCfg[1].u32IpAddr;	

	 //ns1__Probe = dn__Probe;
	 ns1__ProbeResponse = dn__ProbeResponse;
	 
	 if(put_soap_header_wsa(soap) != 0)
	 {
		 __E("put soap header error\r\n");
		 return SOAP_FAULT;
	 }
	 ALLOC_STRUCT(ns1__ProbeResponse->ProbeMatch, struct d__ProbeMatchType);	
	 ALLOC_STRUCT(ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference, struct wsa__EndpointReferenceType);	
	 ALLOC_STRUCT(ns1__ProbeResponse->ProbeMatch->Scopes, struct d__ScopesType);		 
	 ns1__ProbeResponse->__sizeProbeMatch = 1;
	 ns1__ProbeResponse->ProbeMatch->MetadataVersion = 1;
	 ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference->ReferenceParameters = NULL;
	 ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference->ReferenceProperties = NULL;
	
	 ALLOC_STRUCT_NUM(ns1__ProbeResponse->ProbeMatch->Scopes->__item, char, LARGE_INFO_LENGTH);
	 onvif_get_scopes(ns1__ProbeResponse->ProbeMatch->Scopes->__item, LARGE_INFO_LENGTH);
	 ns1__ProbeResponse->ProbeMatch->Scopes->MatchBy = NULL;
	 ALLOC_TOKEN(ns1__ProbeResponse->ProbeMatch->XAddrs, onvif_get_device_xaddr(nIpAddr));
	 ALLOC_STRUCT(ns1__ProbeResponse->ProbeMatch->Types, char*);
	 //strcpy(ns1__ProbeResponse->ProbeMatch->Types[0], "dn:NetworkVideoTransmitter");
	 ALLOC_TOKEN(ns1__ProbeResponse->ProbeMatch->Types[0], dn__Probe->Types[0]);
	 ALLOC_STRUCT(ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference->PortType, char*);
	 ALLOC_TOKEN(ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference->PortType[0], "ttl");
	 ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference->ServiceName= NULL;
	 ALLOC_TOKEN(ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference->Address, onvif_get_hwid(nIpAddr));
	__FUN_END("\n");
	 return SOAP_OK;
 }
 
 int  __dn__Probe1(struct soap* soap, struct d__ProbeType *dn__Probe, struct d__ProbeMatchesType *dn__ProbeResponse)
 {
	 __FUN_BEGIN("\n");
	 
	 int nIpAddr = 0;
	 NVR_DEV_NET_CFG_S stNetCfg;
	 nvr_logon_get_net_cfg(0, &stNetCfg);
	 //nvr_logon_get_net_cfg(0, &stNetCfg);
	 if(soap->master == stOnvifSocket.eth0Socket)
	 {
	 	nIpAddr = stNetCfg.struEtherCfg[0].u32IpAddr;
	 }
	 else if(soap->master == stOnvifSocket.wifiSocket)
	 	nIpAddr = stNetCfg.struEtherCfg[1].u32IpAddr;
	 //struct d__ProbeType *ns1__Probe;
	 struct d__ProbeMatchesType *ns1__ProbeResponse;
 
	 //ns1__Probe = dn__Probe;
	 ns1__ProbeResponse = dn__ProbeResponse;
	 
	 if(put_soap_header_wsa(soap) != 0)
	 {
		 __E("put soap header error\r\n");
		 return SOAP_FAULT;
	 }
 
	 ALLOC_STRUCT(ns1__ProbeResponse->ProbeMatch, struct d__ProbeMatchType);	
	 ALLOC_STRUCT(ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference, struct wsa__EndpointReferenceType);	
	 ALLOC_STRUCT(ns1__ProbeResponse->ProbeMatch->Scopes, struct d__ScopesType);		 
	 ns1__ProbeResponse->__sizeProbeMatch = 1;
	 ns1__ProbeResponse->ProbeMatch->MetadataVersion = 1;
	 ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference->ReferenceParameters = NULL;
	 ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference->ReferenceProperties = NULL;	
	 ALLOC_STRUCT_NUM(ns1__ProbeResponse->ProbeMatch->Scopes->__item, char, LARGE_INFO_LENGTH);
	 onvif_get_scopes(ns1__ProbeResponse->ProbeMatch->Scopes->__item, LARGE_INFO_LENGTH);
	 ns1__ProbeResponse->ProbeMatch->Scopes->MatchBy = NULL;
	 ALLOC_TOKEN(ns1__ProbeResponse->ProbeMatch->XAddrs, onvif_get_device_xaddr(nIpAddr));
	 ALLOC_STRUCT(ns1__ProbeResponse->ProbeMatch->Types, char*);
	 //strcpy(ns1__ProbeResponse->ProbeMatch->Types[0], "dn:NetworkVideoTransmitter");
	 ALLOC_TOKEN(ns1__ProbeResponse->ProbeMatch->Types[0], dn__Probe->Types[0]);
	 ALLOC_STRUCT(ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference->PortType, char*);
	 ALLOC_TOKEN(ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference->PortType[0], "ttl");
	 ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference->ServiceName= NULL;
	 ALLOC_TOKEN(ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference->Address, onvif_get_hwid(nIpAddr));											 
	 /*}
	 else
	 {
		 printf("error\r\n");
		 onvif_fault(soap,"ter:InvArgs","ter:InvArgs");
		 return SOAP_FAULT;
	 }
	 */
	 __FUN_END("\n");
	 return SOAP_OK;
 }


 int  __dn__Hello(struct soap* soap, struct d__HelloType *dn__Hello, struct d__ResolveType *dn__HelloResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __dn__Bye(struct soap* soap, struct d__ByeType *dn__Bye, struct d__ResolveType *dn__ByeResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tad__GetServiceCapabilities(struct soap* soap, struct _tad__GetServiceCapabilities *tad__GetServiceCapabilities, struct _tad__GetServiceCapabilitiesResponse *tad__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tad__DeleteAnalyticsEngineControl(struct soap* soap, struct _tad__DeleteAnalyticsEngineControl *tad__DeleteAnalyticsEngineControl, struct _tad__DeleteAnalyticsEngineControlResponse *tad__DeleteAnalyticsEngineControlResponse)
{
	onvif_fault(soap,"ter:Action", "ter:AnalyticsModuleNotSupported ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__CreateAnalyticsEngineControl(struct soap* soap, struct _tad__CreateAnalyticsEngineControl *tad__CreateAnalyticsEngineControl, struct _tad__CreateAnalyticsEngineControlResponse *tad__CreateAnalyticsEngineControlResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:AnalyticsEngineNotSupported ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__SetAnalyticsEngineControl(struct soap* soap, struct _tad__SetAnalyticsEngineControl *tad__SetAnalyticsEngineControl, struct _tad__SetAnalyticsEngineControlResponse *tad__SetAnalyticsEngineControlResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:AnalyticsEngineNotSupported ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__GetAnalyticsEngineControl(struct soap* soap, struct _tad__GetAnalyticsEngineControl *tad__GetAnalyticsEngineControl, struct _tad__GetAnalyticsEngineControlResponse *tad__GetAnalyticsEngineControlResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:AnalyticsEngineNotSupported ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__GetAnalyticsEngineControls(struct soap* soap, struct _tad__GetAnalyticsEngineControls *tad__GetAnalyticsEngineControls, struct _tad__GetAnalyticsEngineControlsResponse *tad__GetAnalyticsEngineControlsResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:AnalyticsEngineNotSupported ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__GetAnalyticsEngine(struct soap* soap, struct _tad__GetAnalyticsEngine *tad__GetAnalyticsEngine, struct _tad__GetAnalyticsEngineResponse *tad__GetAnalyticsEngineResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:AnalyticsEngineNotSupported ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__GetAnalyticsEngines(struct soap* soap, struct _tad__GetAnalyticsEngines *tad__GetAnalyticsEngines, struct _tad__GetAnalyticsEnginesResponse *tad__GetAnalyticsEnginesResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:AnalyticsEngineNotSupported ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__SetVideoAnalyticsConfiguration(struct soap* soap, struct _tad__SetVideoAnalyticsConfiguration *tad__SetVideoAnalyticsConfiguration, struct _tad__SetVideoAnalyticsConfigurationResponse *tad__SetVideoAnalyticsConfigurationResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:AnalyticsEngineNotSupported ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__SetAnalyticsEngineInput(struct soap* soap, struct _tad__SetAnalyticsEngineInput *tad__SetAnalyticsEngineInput, struct _tad__SetAnalyticsEngineInputResponse *tad__SetAnalyticsEngineInputResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:AnalyticsEngineNotSupported ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__GetAnalyticsEngineInput(struct soap* soap, struct _tad__GetAnalyticsEngineInput *tad__GetAnalyticsEngineInput, struct _tad__GetAnalyticsEngineInputResponse *tad__GetAnalyticsEngineInputResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:AnalyticsEngineNotSupported ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__GetAnalyticsEngineInputs(struct soap* soap, struct _tad__GetAnalyticsEngineInputs *tad__GetAnalyticsEngineInputs, struct _tad__GetAnalyticsEngineInputsResponse *tad__GetAnalyticsEngineInputsResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:AnalyticsEngineNotSupported ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__GetAnalyticsDeviceStreamUri(struct soap* soap, struct _tad__GetAnalyticsDeviceStreamUri *tad__GetAnalyticsDeviceStreamUri, struct _tad__GetAnalyticsDeviceStreamUriResponse *tad__GetAnalyticsDeviceStreamUriResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoAnalyticsEngineControl  ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__GetVideoAnalyticsConfiguration(struct soap* soap, struct _tad__GetVideoAnalyticsConfiguration *tad__GetVideoAnalyticsConfiguration, struct _tad__GetVideoAnalyticsConfigurationResponse *tad__GetVideoAnalyticsConfigurationResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoConfig ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__CreateAnalyticsEngineInputs(struct soap* soap, struct _tad__CreateAnalyticsEngineInputs *tad__CreateAnalyticsEngineInputs, struct _tad__CreateAnalyticsEngineInputsResponse *tad__CreateAnalyticsEngineInputsResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:invalidConfig ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__DeleteAnalyticsEngineInputs(struct soap* soap, struct _tad__DeleteAnalyticsEngineInputs *tad__DeleteAnalyticsEngineInputs, struct _tad__DeleteAnalyticsEngineInputsResponse *tad__DeleteAnalyticsEngineInputsResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:invalidConfig ");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tad__GetAnalyticsState(struct soap* soap, struct _tad__GetAnalyticsState *tad__GetAnalyticsState, struct _tad__GetAnalyticsStateResponse *tad__GetAnalyticsStateResponse)
{
	onvif_fault(soap,"ter:InvalidArgVal","ter:InvalidConfig"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

  int  __tls__GetServiceCapabilities(struct soap* soap, struct _tls__GetServiceCapabilities *tls__GetServiceCapabilities, struct _tls__GetServiceCapabilitiesResponse *tls__GetServiceCapabilitiesResponse)
 {
	 __FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	 return SOAP_FAULT;  
 }


 int  __tls__GetLayout(struct soap* soap, struct _tls__GetLayout *tls__GetLayout, struct _tls__GetLayoutResponse *tls__GetLayoutResponse)
 {
	 onvif_fault(soap,"ter:InvalidArgVal", "ter:NoVideoOutput"); 
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;  
 }
 
  int  __tls__SetLayout(struct soap* soap, struct _tls__SetLayout *tls__SetLayout, struct _tls__SetLayoutResponse *tls__SetLayoutResponse)
 {
	 onvif_fault(soap,"ter:InvalidArgVal", "ter:NoVideoOutput"); 
	 __FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;  
 }
 
  int  __tls__GetDisplayOptions(struct soap* soap, struct _tls__GetDisplayOptions *tls__GetDisplayOptions, struct _tls__GetDisplayOptionsResponse *tls__GetDisplayOptionsResponse)
 {
	 onvif_fault(soap,"ter:InvalidArgVal", "ter:NoVideoOutput"); 
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;  
 }
 
  int  __tls__GetPaneConfigurations(struct soap* soap, struct _tls__GetPaneConfigurations *tls__GetPaneConfigurations, struct _tls__GetPaneConfigurationsResponse *tls__GetPaneConfigurationsResponse)
 {
	 onvif_fault(soap,"ter:InvalidArgVal", "ter:NoVideoOutput"); 
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;  
 }
 
  int  __tls__GetPaneConfiguration(struct soap* soap, struct _tls__GetPaneConfiguration *tls__GetPaneConfiguration, struct _tls__GetPaneConfigurationResponse *tls__GetPaneConfigurationResponse)
 {
	 onvif_fault(soap,"ter:InvalidArgVal", "ter:NoVideoOutput"); 
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;
 }
 
  int  __tls__SetPaneConfigurations(struct soap* soap, struct _tls__SetPaneConfigurations *tls__SetPaneConfigurations, struct _tls__SetPaneConfigurationsResponse *tls__SetPaneConfigurationsResponse)
 {
	 onvif_fault(soap,"ter:InvalidArgVal", "ter:NoVideoOutput"); 
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;
 }
 
  int  __tls__SetPaneConfiguration(struct soap* soap, struct _tls__SetPaneConfiguration *tls__SetPaneConfiguration, struct _tls__SetPaneConfigurationResponse *tdis__SetPaneConfigurationResponse)
 {
	 onvif_fault(soap,"ter:InvalidArgVal", "ter:NoVideoOutput"); 
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;
 }
 
  int  __tls__CreatePaneConfiguration(struct soap* soap, struct _tls__CreatePaneConfiguration *tdis__CreatePaneConfiguration, struct _tls__CreatePaneConfigurationResponse *tls__CreatePaneConfigurationResponse)
 {
	 onvif_fault(soap,"ter:InvalidArgVal", "ter:NoVideoOutput"); 
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;
 }
 
  int  __tls__DeletePaneConfiguration(struct soap* soap, struct _tls__DeletePaneConfiguration *tls__DeletePaneConfiguration, struct _tls__DeletePaneConfigurationResponse *tls__DeletePaneConfigurationResponse)
 {
	 onvif_fault(soap,"ter:InvalidArgVal", "ter:NoVideoOutput"); 
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;
 }


 int  __tds__GetServices(struct soap* soap, struct _tds__GetServices *tds__GetServices, struct _tds__GetServicesResponse *tds__GetServicesResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetServiceCapabilities(struct soap* soap, struct _tds__GetServiceCapabilities *tds__GetServiceCapabilities, struct _tds__GetServiceCapabilitiesResponse *tds__GetServiceCapabilitiesResponse)
{



	ALLOC_STRUCT(tds__GetServiceCapabilitiesResponse->Capabilities, struct tds__DeviceServiceCapabilities);

	struct tds__NetworkCapabilities *Network;	/* required element of type tds:NetworkCapabilities */
	struct tds__SecurityCapabilities *Security;	/* required element of type tds:SecurityCapabilities */
	struct tds__SystemCapabilities *System;	/* required element of type tds:SystemCapabilities */
//	struct tds__MiscCapabilities *Misc;	/* optional element of type tds:MiscCapabilities */
	ALLOC_STRUCT(Network, struct tds__NetworkCapabilities);	
	ALLOC_STRUCT(Network->NTP, int);
	*Network->NTP = 3;
	ALLOC_STRUCT(Network->HostnameFromDHCP, enum xsd__boolean_);
	*Network->HostnameFromDHCP = xsd__boolean__false_;
	ALLOC_STRUCT(Network->Dot11Configuration, enum xsd__boolean_);
	*Network->Dot11Configuration = xsd__boolean__false_;
	ALLOC_STRUCT(Network->DynDNS, enum xsd__boolean_);
	*Network->DynDNS = xsd__boolean__false_;
	ALLOC_STRUCT(Network->IPVersion6, enum xsd__boolean_);
	*Network->IPVersion6 = xsd__boolean__false_;
	ALLOC_STRUCT(Network->ZeroConfiguration, enum xsd__boolean_);
	*Network->ZeroConfiguration = xsd__boolean__false_;
	ALLOC_STRUCT(Network->IPFilter, enum xsd__boolean_);
	*Network->IPFilter = xsd__boolean__false_;	
	
	ALLOC_STRUCT(Security, struct tds__SecurityCapabilities);
	ALLOC_STRUCT(Security->RELToken, enum xsd__boolean_);
	*Security->RELToken = xsd__boolean__false_;	
	ALLOC_STRUCT(Security->HttpDigest, enum xsd__boolean_);
	*Security->HttpDigest = xsd__boolean__false_;	
	ALLOC_STRUCT(Security->UsernameToken, enum xsd__boolean_);
	*Security->UsernameToken = xsd__boolean__false_;	
	ALLOC_STRUCT(Security->KerberosToken, enum xsd__boolean_);
	*Security->KerberosToken = xsd__boolean__false_;	
	ALLOC_STRUCT(Security->SAMLToken, enum xsd__boolean_);
	*Security->SAMLToken = xsd__boolean__false_;
	ALLOC_STRUCT(Security->X_x002e509Token, enum xsd__boolean_);
	*Security->X_x002e509Token = xsd__boolean__false_;
	ALLOC_STRUCT(Security->RemoteUserHandling, enum xsd__boolean_);
	*Security->RemoteUserHandling = xsd__boolean__false_;
	ALLOC_STRUCT(Security->Dot1X, enum xsd__boolean_);
	*Security->Dot1X = xsd__boolean__false_;
	ALLOC_STRUCT(Security->AccessPolicyConfig, enum xsd__boolean_);
	*Security->AccessPolicyConfig = xsd__boolean__false_;	
	ALLOC_STRUCT(Security->OnboardKeyGeneration, enum xsd__boolean_);
	*Security->OnboardKeyGeneration = xsd__boolean__false_;
	ALLOC_STRUCT(Security->TLS1_x002e2, enum xsd__boolean_);
	*Security->TLS1_x002e2 = xsd__boolean__false_;
	ALLOC_STRUCT(Security->TLS1_x002e1, enum xsd__boolean_);
	*Security->TLS1_x002e1 = xsd__boolean__false_;
	ALLOC_STRUCT(Security->TLS1_x002e0, enum xsd__boolean_);
	*Security->TLS1_x002e0 = xsd__boolean__false_;

	
	ALLOC_STRUCT(System, struct tds__SystemCapabilities);
	ALLOC_STRUCT(System->HttpSupportInformation, enum xsd__boolean_);
	*System->HttpSupportInformation = xsd__boolean__true_;
	ALLOC_STRUCT(System->HttpSystemLogging, enum xsd__boolean_);
	*System->HttpSystemLogging = xsd__boolean__true_;
	ALLOC_STRUCT(System->HttpSystemBackup, enum xsd__boolean_);
	*System->HttpSystemBackup = xsd__boolean__false_;
	ALLOC_STRUCT(System->HttpFirmwareUpgrade, enum xsd__boolean_);
	*System->HttpFirmwareUpgrade = xsd__boolean__true_;
	ALLOC_STRUCT(System->FirmwareUpgrade, enum xsd__boolean_);
	*System->FirmwareUpgrade = xsd__boolean__false_;
	ALLOC_STRUCT(System->SystemLogging, enum xsd__boolean_);
	*System->SystemLogging = xsd__boolean__true_;
	ALLOC_STRUCT(System->SystemBackup, enum xsd__boolean_);
	*System->SystemBackup = xsd__boolean__false_;
	ALLOC_STRUCT(System->RemoteDiscovery, enum xsd__boolean_);
	*System->RemoteDiscovery = xsd__boolean__false_;
	ALLOC_STRUCT(System->DiscoveryBye, enum xsd__boolean_);
	*System->DiscoveryBye = xsd__boolean__false_;
	ALLOC_STRUCT(System->DiscoveryResolve, enum xsd__boolean_);
	*System->DiscoveryResolve = xsd__boolean__false_;

		
	tds__GetServiceCapabilitiesResponse->Capabilities->Network = Network;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security = Security;
	tds__GetServiceCapabilitiesResponse->Capabilities->System = System;
	tds__GetServiceCapabilitiesResponse->Capabilities->Misc = NULL;

	
    __FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_OK;	
}

 int  __tds__GetDeviceInformation(struct soap* soap, struct _tds__GetDeviceInformation *tds__GetDeviceInformation, struct _tds__GetDeviceInformationResponse *tds__GetDeviceInformationResponse)
{
	NVR_DEV_INFO_S stDevInfo;
	NVR_DEV_NET_CFG_S stNetCfg;
	unsigned char *mac;
	char SerialNumber[LARGE_INFO_LENGTH];
	char HardwareId[LARGE_INFO_LENGTH];
	__FUN_BEGIN("\n");

	unsigned long ip;
	ip = onvif_get_ipaddr(soap);
	memset(&stNetCfg, 0, sizeof(stNetCfg));
	nvr_logon_get_dev_cfg(0, &stDevInfo);
	nvr_logon_get_net_cfg(0, &stNetCfg);
	if(ip == stNetCfg.struEtherCfg[0].u32IpAddr)
		mac = stNetCfg.struEtherCfg[0].u8MacAddr;
	else
		mac = stNetCfg.struEtherCfg[1].u8MacAddr;
	sprintf(SerialNumber,"%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	sprintf(HardwareId,"1419d68a-1dd2-11b2-a105-%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	
	tds__GetDeviceInformationResponse->Manufacturer = MANUFACTURER_NAME;
	tds__GetDeviceInformationResponse->Model = MODEL_NAME;
	tds__GetDeviceInformationResponse->FirmwareVersion = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	tds__GetDeviceInformationResponse->SerialNumber = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	tds__GetDeviceInformationResponse->HardwareId = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	if(tds__GetDeviceInformationResponse->FirmwareVersion == NULL ||
		tds__GetDeviceInformationResponse->SerialNumber == NULL ||
		tds__GetDeviceInformationResponse->HardwareId == NULL)
	{
		__E("Failed to malloc for information.\n");
		return SOAP_FAULT;
	}
	strcpy(tds__GetDeviceInformationResponse->FirmwareVersion, stDevInfo.stStableInfo.szSoftVer);
	strcpy(tds__GetDeviceInformationResponse->SerialNumber, SerialNumber);
	strcpy(tds__GetDeviceInformationResponse->HardwareId, HardwareId);
	__FUN_END("\n");

	return SOAP_OK;	
}



int  __tds__SetSystemDateAndTime(struct soap* soap, struct _tds__SetSystemDateAndTime *tds__SetSystemDateAndTime, struct _tds__SetSystemDateAndTimeResponse *tds__SetSystemDateAndTimeResponse)
{
	NVR_NTP_CFG_S stNtpCfg;
	NVR_TIME_S stTime;
	__FUN_BEGIN("\n");
	nvr_logon_get_ntp_cfg(0, &stNtpCfg);
	stNtpCfg.u8NtpOpen = tds__SetSystemDateAndTime->DateTimeType; //Manual = 0, NTP = 1
	stNtpCfg.u8EuroTime = tds__SetSystemDateAndTime->DaylightSavings;
	/* Time Zone */
	printf("===========zone============:%s\r\n", tds__SetSystemDateAndTime->TimeZone->TZ);
	if(tds__SetSystemDateAndTime->TimeZone)
	{
		stNtpCfg.nTimeZone = 8*3600;//nvr_get_GMT_TimeZone(tds__SetSystemDateAndTime->TimeZone->TZ);		
		printf("zone:%d\r\n", stNtpCfg.nTimeZone);		
	}
	
	nvr_logon_set_ntp_cfg(0, &stNtpCfg);
	
	if(tds__SetSystemDateAndTime->UTCDateTime)
	{
		nvr_logon_get_time_cfg(0, &stTime);		
		if(tds__SetSystemDateAndTime->UTCDateTime->Date)
		{
			stTime.u8Year = (unsigned char) (tds__SetSystemDateAndTime->UTCDateTime->Date->Year - 1900);
			stTime.u8Month = (unsigned char)tds__SetSystemDateAndTime->UTCDateTime->Date->Month;
			stTime.u8Day= (unsigned char)tds__SetSystemDateAndTime->UTCDateTime->Date->Day;
			printf("Y:%d, m:%d, d:%d\r\n", stTime.u8Year, stTime.u8Month, stTime.u8Day);
			if((stTime.u8Month>12) || (stTime.u8Day >31))
			{		
				onvif_fault(soap,"ter:InvalidArgVal", "ter:InvalidDateTime");
				return SOAP_FAULT;
			}	
		}
		
		if(tds__SetSystemDateAndTime->UTCDateTime->Time)
		{
			stTime.u8Hour = (unsigned char)tds__SetSystemDateAndTime->UTCDateTime->Time->Hour;
			stTime.u8Minute = (unsigned char)tds__SetSystemDateAndTime->UTCDateTime->Time->Minute;
			stTime.u8Second = (unsigned char)tds__SetSystemDateAndTime->UTCDateTime->Time->Second;
			if((stTime.u8Hour>24) || (stTime.u8Minute>60) || (stTime.u8Second>60))
			{		
				onvif_fault(soap,"ter:InvalidArgVal", "ter:InvalidDateTime");
				return SOAP_FAULT;
			}
		}

		//hi_timer_UTC_2_LocalTime(&stTime, &stLTTime, stNtpCfg.nTimeZone);
		
		//{
		//	printf("Y:%d, M:%d, D:%d\r\n", stLTTime.u8Year + 1900, stLTTime.u8Month, stLTTime.u8Day);
		//	printf("H:%d, M:%d, S:%d\r\n", stLTTime.u8Hour, stLTTime.u8Minute, stLTTime.u8Second);
		//}
		
		//nvr_logon_set_time_cfg(0, &stLTTime);
	}
	__FUN_END("\n");

	return SOAP_OK;
}


 int  __tds__GetSystemDateAndTime(struct soap* soap, struct _tds__GetSystemDateAndTime *tds__GetSystemDateAndTime, struct _tds__GetSystemDateAndTimeResponse *tds__GetSystemDateAndTimeResponse)
{
	NVR_NTP_CFG_S stNtpCfg;
	NVR_TIME_S stTime;
	NVR_TIME_S stUTCTime;
	__FUN_BEGIN("\n");
	nvr_logon_get_ntp_cfg(0, &stNtpCfg);
	nvr_logon_get_time_cfg(0, &stTime);	
	nvr_logon_get_time_cfg(0, &stUTCTime);
	//nvr_timer_LocalTime_2_UTC(&stTime, &stUTCTime, stNtpCfg.nTimeZone);

	ALLOC_STRUCT(tds__GetSystemDateAndTimeResponse->SystemDateAndTime, struct tt__SystemDateTime);
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->DateTimeType = (stNtpCfg.u8NtpOpen)?tt__SetDateTimeType__NTP:tt__SetDateTimeType__Manual; 
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->DaylightSavings = !!stNtpCfg.u8EuroTime;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->Extension = NULL;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->__anyAttribute = NULL;
	ALLOC_STRUCT(tds__GetSystemDateAndTimeResponse->SystemDateAndTime->TimeZone, struct tt__TimeZone);
	ALLOC_STRUCT(tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime, struct tt__DateTime);
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->TimeZone->TZ = "GMT+08";//hi_get_GMT_TZ(stNtpCfg.nTimeZone);//"GMT+05";//"NZST-12NZDT,M10.1.0/2,M3.3.0/3";//timezone;POSIX 1003.1
	ALLOC_STRUCT(tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Time, struct tt__Time);

	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Time->Hour = stUTCTime.u8Hour;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Time->Minute = stUTCTime.u8Minute;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Time->Second = stUTCTime.u8Second;
	ALLOC_STRUCT(tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Date, struct tt__Date);

	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Date->Year = stUTCTime.u8Year+1900;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Date->Month = stUTCTime.u8Month;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Date->Day = stUTCTime.u8Day;
	ALLOC_STRUCT(tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime, struct tt__DateTime);
	ALLOC_STRUCT(tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Time, struct tt__Time);

	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Time->Hour = stTime.u8Hour;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Time->Minute = stTime.u8Minute;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Time->Second = stTime.u8Second;
	
	ALLOC_STRUCT( tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Date,struct tt__Date);

	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Date->Year = stTime.u8Year+1900;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Date->Month = stTime.u8Month;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Date->Day = stTime.u8Day;
	__FUN_END("\n");
	return SOAP_OK; 
}

 int  __tds__SetSystemFactoryDefault(struct soap* soap, struct _tds__SetSystemFactoryDefault *tds__SetSystemFactoryDefault, struct _tds__SetSystemFactoryDefaultResponse *tds__SetSystemFactoryDefaultResponse)
{
	__FUN_BEGIN("\n");
	nvr_sw_reset();
	__FUN_END("\n");
	return SOAP_OK; 
}

 int  __tds__UpgradeSystemFirmware(struct soap* soap, struct _tds__UpgradeSystemFirmware *tds__UpgradeSystemFirmware, struct _tds__UpgradeSystemFirmwareResponse *tds__UpgradeSystemFirmwareResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

void hi_onvif_reboot_message(void* data)
{
	pthread_detach(pthread_self()); /*释放线程资源 */
	prctl(PR_SET_NAME, "ONVIFReboot");

	sleep(1);
	hi_onvif_bye(0);	
	nvr_restart();
}

 int  __tds__SystemReboot(struct soap* soap, struct _tds__SystemReboot *tds__SystemReboot, struct _tds__SystemRebootResponse *tds__SystemRebootResponse)
{
	 __FUN_BEGIN("\n");
	pthread_t thdid;

	if(pthread_create(&thdid, NULL, (void *)&hi_onvif_reboot_message, (void *)NULL) < 0)
	{
		return SOAP_FAULT;	
	}

	 tds__SystemRebootResponse->Message = "Rebooting";
	 __FUN_END("\n");
	return SOAP_OK;	 

}

 int  __tds__RestoreSystem(struct soap* soap, struct _tds__RestoreSystem *tds__RestoreSystem, struct _tds__RestoreSystemResponse *tds__RestoreSystemResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetSystemBackup(struct soap* soap, struct _tds__GetSystemBackup *tds__GetSystemBackup, struct _tds__GetSystemBackupResponse *tds__GetSystemBackupResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetSystemLog(struct soap* soap, struct _tds__GetSystemLog *tds__GetSystemLog, struct _tds__GetSystemLogResponse *tds__GetSystemLogResponse) 
{

	if(tds__GetSystemLog->LogType == tt__SystemLogType__System)
	{
		 ALLOC_STRUCT(tds__GetSystemLogResponse->SystemLog, struct tt__SystemLog);
		ALLOC_STRUCT_NUM(tds__GetSystemLogResponse->SystemLog->String, char, 128);
		sprintf(tds__GetSystemLogResponse->SystemLog->String, "system log: no fixed\r\n");

	}else if(tds__GetSystemLog->LogType == tt__SystemLogType__Access)
	{
		 ALLOC_STRUCT(tds__GetSystemLogResponse->SystemLog, struct tt__SystemLog);
		ALLOC_STRUCT_NUM(tds__GetSystemLogResponse->SystemLog->String, char, 128);
		sprintf(tds__GetSystemLogResponse->SystemLog->String, "access log: no fixed\r\n");
	}else	
		return SOAP_FAULT;

	return SOAP_OK;
}

 int  __tds__GetSystemSupportInformation(struct soap* soap, struct _tds__GetSystemSupportInformation *tds__GetSystemSupportInformation, struct _tds__GetSystemSupportInformationResponse *tds__GetSystemSupportInformationResponse) 
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetScopes(struct soap* soap, struct _tds__GetScopes *tds__GetScopes, struct _tds__GetScopesResponse *tds__GetScopesResponse) 
{
	int i, numScopes = 0;
	int n = 0;
	__FUN_BEGIN("\n");

	for(i = 0; i < ONVIF_SCOPES_NUM; i++)
	{
		if(strlen(gOnvifInfo.type_scope[i].scope) != 0)
			numScopes++;
		if(strlen(gOnvifInfo.name_scope[i].scope) != 0)
			numScopes++;
		if(strlen(gOnvifInfo.location_scope[i].scope) != 0)
			numScopes++;
		if(strlen(gOnvifInfo.hardware_scope[i].scope) != 0)
			numScopes++;			
	}
	
	tds__GetScopesResponse->Scopes = (struct tt__Scope *)soap_malloc(soap, sizeof(struct tt__Scope) * numScopes);
	if(tds__GetScopesResponse->Scopes == NULL)
	{
		__E("Failed to malloc for scopes.\n");
		return SOAP_FAULT;
	}
	for(i = 0; i < ONVIF_SCOPES_NUM; i++)
	{
		if(strlen(gOnvifInfo.type_scope[i].scope) != 0)
		{
			tds__GetScopesResponse->Scopes[n].ScopeDef = tt__ScopeDefinition__Fixed;
			tds__GetScopesResponse->Scopes[n].ScopeItem = gOnvifInfo.type_scope[i].scope;
			n++;
		}
	}
	for(i = 0; i < ONVIF_SCOPES_NUM; i++)
	{
		if(strlen(gOnvifInfo.name_scope[i].scope) != 0)
		{
			tds__GetScopesResponse->Scopes[n].ScopeDef = tt__ScopeDefinition__Fixed;
			tds__GetScopesResponse->Scopes[n].ScopeItem = gOnvifInfo.name_scope[i].scope;
			n++;
		}
	}
	for(i = 0; i < ONVIF_SCOPES_NUM; i++)
	{
		if(strlen(gOnvifInfo.location_scope[i].scope) != 0)
		{
			tds__GetScopesResponse->Scopes[n].ScopeDef = tt__ScopeDefinition__Fixed;
			tds__GetScopesResponse->Scopes[n].ScopeItem = gOnvifInfo.location_scope[i].scope;
			n++;
		}
	}
	for(i = 0; i < ONVIF_SCOPES_NUM; i++)
	{
		if(strlen(gOnvifInfo.hardware_scope[i].scope) != 0)
		{
			tds__GetScopesResponse->Scopes[n].ScopeDef = tt__ScopeDefinition__Fixed;
			tds__GetScopesResponse->Scopes[n].ScopeItem = gOnvifInfo.hardware_scope[i].scope;
			n++;
		}
	}
	
	tds__GetScopesResponse->__sizeScopes = numScopes;
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tds__SetScopes(struct soap* soap, struct _tds__SetScopes *tds__SetScopes, struct _tds__SetScopesResponse *tds__SetScopesResponse) 
{
	int i, j;
	int size = tds__SetScopes->__sizeScopes;
	__FUN_BEGIN("\n");

	for(i = 0; i < size; i++)
	{
		for(j = 0; j < ONVIF_SCOPES_NUM; j++)
		{
			if(strcmp(tds__SetScopes->Scopes[i], gOnvifInfo.type_scope[j].scope) == 0)
			{
				onvif_fault(soap,"ter:OperationProhibited", "ter:ScopeOverwrite");
				return SOAP_FAULT;
			}
			if(strcmp(tds__SetScopes->Scopes[i], gOnvifInfo.name_scope[j].scope) == 0)
			{
				onvif_fault(soap,"ter:OperationProhibited", "ter:ScopeOverwrite");
				return SOAP_FAULT;
			}
			if(strcmp(tds__SetScopes->Scopes[i], gOnvifInfo.location_scope[j].scope) == 0)
			{
				onvif_fault(soap,"ter:OperationProhibited", "ter:ScopeOverwrite");
				return SOAP_FAULT;
			}
			if(strcmp(tds__SetScopes->Scopes[i], gOnvifInfo.hardware_scope[j].scope) == 0)
			{
				onvif_fault(soap,"ter:OperationProhibited", "ter:ScopeOverwrite");
				return SOAP_FAULT;
			}
		}
	}
	__FUN_END("\n");

	return SOAP_OK;
}

 int  __tds__AddScopes(struct soap* soap, struct _tds__AddScopes *tds__AddScopes, struct _tds__AddScopesResponse *tds__AddScopesResponse) 
{
	onvif_fault(soap, "ter:Action", "ter:TooManyScopes");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__RemoveScopes(struct soap* soap, struct _tds__RemoveScopes *tds__RemoveScopes, struct _tds__RemoveScopesResponse *tds__RemoveScopesResponse) 
{
	onvif_fault(soap,"ter:OperationProhibited", "ter:FixedScope");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetDiscoveryMode(struct soap* soap, struct _tds__GetDiscoveryMode *tds__GetDiscoveryMode, struct _tds__GetDiscoveryModeResponse *tds__GetDiscoveryModeResponse) 
{
	//HI_DEV_ONVIF_CFG_S stOnvifCfg;
	__FUN_BEGIN("\n");
	//hi_platform_get_onvif_cfg(0, &stOnvifCfg);
	//tds__GetDiscoveryModeResponse->DiscoveryMode = stOnvifCfg.u8Disconvery;
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tds__SetDiscoveryMode(struct soap* soap, struct _tds__SetDiscoveryMode *tds__SetDiscoveryMode, struct _tds__SetDiscoveryModeResponse *tds__SetDiscoveryModeResponse) 
{
	//HI_DEV_ONVIF_CFG_S stOnvifCfg;
	unsigned char  _mode = tds__SetDiscoveryMode->DiscoveryMode;
	__FUN_BEGIN("\n");
	if(_mode == 0 || _mode == 1)
	{
		//hi_platform_get_onvif_cfg(0, &stOnvifCfg);
		//stOnvifCfg.u8Disconvery = _mode;
		//hi_platform_set_onvif_cfg(0, &stOnvifCfg);
	}
	else
	{
		onvif_fault(soap,"ter:InvalidArgVal", "ter:InvalidArgVal");
		return SOAP_FAULT;
	}
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tds__GetRemoteDiscoveryMode(struct soap* soap, struct _tds__GetRemoteDiscoveryMode *tds__GetRemoteDiscoveryMode, struct _tds__GetRemoteDiscoveryModeResponse *tds__GetRemoteDiscoveryModeResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:GetRemoteDiscoveryModeNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__SetRemoteDiscoveryMode(struct soap* soap, struct _tds__SetRemoteDiscoveryMode *tds__SetRemoteDiscoveryMode, struct _tds__SetRemoteDiscoveryModeResponse *tds__SetRemoteDiscoveryModeResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:SetRemoteDiscoveryModeNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetDPAddresses(struct soap* soap, struct _tds__GetDPAddresses *tds__GetDPAddresses, struct _tds__GetDPAddressesResponse *tds__GetDPAddressesResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:GetDiscoveryProxyNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tds__GetEndpointReference(struct soap* soap, struct _tds__GetEndpointReference *tds__GetEndpointReference, struct _tds__GetEndpointReferenceResponse *tds__GetEndpointReferenceResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:GetEndPointRefNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tds__GetRemoteUser(struct soap* soap, struct _tds__GetRemoteUser *tds__GetRemoteUser, struct _tds__GetRemoteUserResponse *tds__GetRemoteUserResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:RemoteUserNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tds__SetRemoteUser(struct soap* soap, struct _tds__SetRemoteUser *tds__SetRemoteUser, struct _tds__SetRemoteUserResponse *tds__SetRemoteUserResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:RemoteUserNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tds__GetUsers(struct soap* soap, struct _tds__GetUsers *tds__GetUsers, struct _tds__GetUsersResponse *tds__GetUsersResponse) 
{
	int userIndex = 0, j = 0;
	int nCount = 0;
	NVR_DEV_USER_CFG_S stUserCfg[NVR_MAX_USR_NUM];
	char *pUserName;

	__FUN_BEGIN("\n");
	for(userIndex = 0; userIndex < NVR_MAX_USR_NUM; userIndex++)
	{
		nvr_logon_get_user_cfg(userIndex, &stUserCfg[userIndex]);
		if(strlen(stUserCfg[userIndex].szUsrName) == 0)
			continue;
		nCount++;
	}

	tds__GetUsersResponse->User = (struct tt__User*)soap_malloc(soap, (nCount * sizeof(struct tt__User)));
	for (userIndex = 0; userIndex < NVR_MAX_USR_NUM; userIndex++)
	{
		if (strlen(stUserCfg[userIndex].szUsrName) == 0)
			continue;

		if (j >= nCount)	break;
		
		pUserName = soap_malloc(soap, NVR_USER_NAME_LEN);
		if(pUserName == NULL)
		{
			__E("Failed to malloc for user name.\n");
			return SOAP_FAULT;
		}
		memset(pUserName, 0, NVR_USER_NAME_LEN);
		strcpy(pUserName, stUserCfg[userIndex].szUsrName);

		tds__GetUsersResponse->User[j].Username = pUserName;
		if(userIndex == 0)	// admin
		{	
			tds__GetUsersResponse->User[j].UserLevel = tt__UserLevel__Administrator;
		}
		else	// user
		{	
			tds__GetUsersResponse->User[j].UserLevel = tt__UserLevel__User;
		}
		tds__GetUsersResponse->User[j].Password = NULL;
		tds__GetUsersResponse->User[j].Extension = NULL;
		tds__GetUsersResponse->User[j].__anyAttribute = NULL;
		j++;
	}	
	tds__GetUsersResponse->__sizeUser = nCount;
	__FUN_END("\n");
	return SOAP_OK; 
}

 int  __tds__CreateUsers(struct soap* soap, struct _tds__CreateUsers *tds__CreateUsers, struct _tds__CreateUsersResponse *tds__CreateUsersResponse) 
{
	int size = tds__CreateUsers->__sizeUser;
	int i, j, nCount = 0;
	NVR_DEV_USER_CFG_S stUserCfg[NVR_MAX_USR_NUM];
	__FUN_BEGIN("\n");
	for(i = 0; i < NVR_MAX_USR_NUM; i++)
	{
		nvr_logon_get_user_cfg(i, &stUserCfg[i]);
		if (strlen(stUserCfg[i].szUsrName) == 0)
			continue;
		nCount++;
	}

	if((size > NVR_MAX_USR_NUM - nCount)||(size < 0))
	{
		onvif_fault(soap,"ter:OperationProhibited","ter:TooManyUsers");
		return SOAP_FAULT;
	}

	for(i = 0; i < size; i++)
	{
		if(strlen(tds__CreateUsers->User[i].Username) > NVR_USER_NAME_LEN - 1)
		{
			onvif_fault(soap,"ter:OperationProhibited", "ter:UsernameTooLong");
			return SOAP_FAULT;
		}
		if(strlen(tds__CreateUsers->User[i].Password) > NVR_PASS_WORD_LEN - 1)
		{
			onvif_fault(soap,"ter:OperationProhibited", "ter:PasswordTooLong");
			return SOAP_FAULT;
		}

		
		/* check username already exist */	  
		for(j = 0; j < NVR_MAX_USR_NUM; j++)
		{
			if (strcmp(tds__CreateUsers->User[i].Username, stUserCfg[j].szUsrName) == 0)
			{
				onvif_fault(soap,"ter:OperationProhibited", "ter:UsernameClash");
				return SOAP_FAULT;
			}
		}

		
		/* Save user */
		for(j = 0; j < NVR_MAX_USR_NUM; j++)
		{
			if(strlen(stUserCfg[j].szUsrName) != 0)
				continue;
			if ((tds__CreateUsers->User[i].UserLevel == tt__UserLevel__Administrator)&&(j!=0))
			{
				onvif_fault(soap,"ter:OperationProhibited", "ter:UsernameClash");
				return SOAP_FAULT;
			}
				
			strcpy(stUserCfg[j].szUsrName, tds__CreateUsers->User[i].Username);
			strcpy(stUserCfg[j].szPsw, tds__CreateUsers->User[i].Password);			
			nvr_logon_set_user_cfg(j, &stUserCfg[j]);
			break;
		}		
	}

	__FUN_END("\n");

	return SOAP_OK;
}

 int  __tds__DeleteUsers(struct soap* soap, struct _tds__DeleteUsers *tds__DeleteUsers, struct _tds__DeleteUsersResponse *tds__DeleteUsersResponse) 
{
	int size = tds__DeleteUsers->__sizeUsername;
	int i, j;
	NVR_DEV_USER_CFG_S stUserCfg[NVR_MAX_USR_NUM];
	__FUN_BEGIN("\n");

	for(i = 0; i < NVR_MAX_USR_NUM; i++)
	{		
		nvr_logon_get_user_cfg(i, &stUserCfg[i]);
	}

	for(i = 0; i < size; i++)
	{
		for(j = 0; j < NVR_MAX_USR_NUM; j++)
		{
			if (strcmp(tds__DeleteUsers->Username[i], stUserCfg[j].szUsrName) == 0)
			{
				if(j == 0)	// 因为第0个为管理员，不能删除
				{
					onvif_fault(soap,"ter:InvalidArgVal", "ter:FixedUser");
					return SOAP_FAULT;
				}
				// delete the user
				memset(stUserCfg[j].szUsrName, 0, sizeof(stUserCfg[j].szUsrName));
				memset(stUserCfg[j].szPsw, 0, sizeof(stUserCfg[j].szPsw));
				break;
			}
		}
		if(j == NVR_MAX_USR_NUM)	// 没有找到 需要删除的用户名
		{
			onvif_fault(soap,"ter:InvalidArgVal", "ter:UsernameMissing");
			return SOAP_FAULT;	
		}
	}
	
	for(i = 0; i < NVR_MAX_USR_NUM; i++)
	{		
		nvr_logon_set_user_cfg(i, &stUserCfg[i]);
	}
	__FUN_END("\n");
	return SOAP_OK; 
}

 int  __tds__SetUser(struct soap* soap, struct _tds__SetUser *tds__SetUser, struct _tds__SetUserResponse *tds__SetUserResponse) 
{
	int size = tds__SetUser->__sizeUser;
	int i, j;
	NVR_DEV_USER_CFG_S stUserCfg[NVR_MAX_USR_NUM];
	__FUN_BEGIN("\n");
	for(i = 0; i < NVR_MAX_USR_NUM; i++)
	{
		nvr_logon_get_user_cfg(i, &stUserCfg[i]);
	}

	for(i = 0; i < size; i++)
	{
		if(strlen(tds__SetUser->User[i].Username) > NVR_USER_NAME_LEN - 1)
		{
			onvif_fault(soap,"ter:OperationProhibited", "ter:UsernameTooLong");
			return SOAP_FAULT;
		}

		if(strlen(tds__SetUser->User[i].Password) > NVR_PASS_WORD_LEN - 1)
		{
			onvif_fault(soap,"ter:OperationProhibited", "ter:PasswordTooLong");
			return SOAP_FAULT;
		}

		
		for(j = 0; j< NVR_MAX_USR_NUM; j++)
		{			
			if(strcmp(tds__SetUser->User[i].Username, stUserCfg[j].szUsrName) == 0)
			{				
				if ((tds__SetUser->User[i].UserLevel == tt__UserLevel__Administrator)&&(j!=0))
				{
					onvif_fault(soap,"ter:OperationProhibited", "ter:PropertyClash");
					return SOAP_FAULT;
				}
				strcpy(stUserCfg[j].szUsrName, tds__SetUser->User[i].Username);
				strcpy(stUserCfg[j].szPsw, tds__SetUser->User[i].Password);	
				nvr_logon_set_user_cfg(j, &stUserCfg[j]);
				break;
			}
		}
		
		// 找不到需要设置的用户名
		if (j == NVR_MAX_USR_NUM)
		{
			onvif_fault(soap,"ter:InvalidArgVal", "ter:UsernameMissing");
			return SOAP_FAULT;
		}
	}

	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tds__GetWsdlUrl(struct soap* soap, struct _tds__GetWsdlUrl *tds__GetWsdlUrl, struct _tds__GetWsdlUrlResponse *tds__GetWsdlUrlResponse) 
{
	tds__GetWsdlUrlResponse->WsdlUrl = "http://www.onvif.org/Documents/Specifications.aspx";
	return SOAP_OK;
}

 int  __tds__GetCapabilities(struct soap* soap, struct _tds__GetCapabilities *tds__GetCapabilities, struct _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse) 
{
//	HI_DEV_INFO_S stDevInfo;
	int size = tds__GetCapabilities->__sizeCategory;
	unsigned int ip = htonl(net_get_sock_ip(soap->socket));
	int i;
	struct tt__AnalyticsCapabilities *Analytics = NULL;	
	struct tt__DeviceCapabilities *Device = NULL;
	struct tt__EventCapabilities *Events= NULL;
	struct tt__ImagingCapabilities *Imaging= NULL;
	struct tt__MediaCapabilities *Media= NULL;
	struct tt__PTZCapabilities *PTZ= NULL;
	struct tt__CapabilitiesExtension *Extension= NULL;
	struct tt__Capabilities *Capabilities= NULL;
	//CHECK_USER;
	
	ALLOC_STRUCT(Capabilities, struct tt__Capabilities);	
	soap_default_tt__Capabilities(soap, Capabilities);
	for(i=0; i<size; i++)
	{		
		if(tds__GetCapabilities->Category[i] == tt__CapabilityCategory__All)
		{		
			ALLOC_STRUCT(Analytics, struct tt__AnalyticsCapabilities);	
			ALLOC_STRUCT(Device, struct tt__DeviceCapabilities);
			ALLOC_STRUCT(Events, struct tt__EventCapabilities);
			ALLOC_STRUCT(Imaging, struct tt__ImagingCapabilities);
			ALLOC_STRUCT(Media, struct tt__MediaCapabilities);
			ALLOC_STRUCT(PTZ, struct tt__PTZCapabilities);
			ALLOC_STRUCT(Extension, struct tt__CapabilitiesExtension);			
		}else if(tds__GetCapabilities->Category[i] == tt__CapabilityCategory__Analytics)
		{
			ALLOC_STRUCT(Analytics, struct tt__AnalyticsCapabilities);	
		}else if(tds__GetCapabilities->Category[i] == tt__CapabilityCategory__Device)
		{
			ALLOC_STRUCT(Device, struct tt__DeviceCapabilities);
		}else if(tds__GetCapabilities->Category[i] == tt__CapabilityCategory__Events)
		{
			ALLOC_STRUCT(Events, struct tt__EventCapabilities);
		}else if(tds__GetCapabilities->Category[i] == tt__CapabilityCategory__Imaging)
		{
			ALLOC_STRUCT(Imaging, struct tt__ImagingCapabilities);
		}else if(tds__GetCapabilities->Category[i] == tt__CapabilityCategory__Media)
		{
			ALLOC_STRUCT(Media, struct tt__MediaCapabilities);
		}else if(tds__GetCapabilities->Category[i] == tt__CapabilityCategory__PTZ)
		{
			ALLOC_STRUCT(PTZ, struct tt__PTZCapabilities);
		}
		
	}	

	if(Analytics)
	{	
		ALLOC_TOKEN(Analytics->XAddr, onvif_get_analytics_xaddr(ip));
		
		Analytics->RuleSupport = xsd__boolean__false_;
		Analytics->AnalyticsModuleSupport = xsd__boolean__false_;
		Analytics->__size = 0;
		Analytics->__any = NULL;
		Analytics->__anyAttribute = NULL;
	}

	if(Device)
	{
		struct tt__NetworkCapabilities *Network;
		struct tt__SystemCapabilities *System;
		struct tt__IOCapabilities *IO;
		struct tt__SecurityCapabilities *Security;
		ALLOC_TOKEN(Device->XAddr, onvif_get_device_xaddr(ip));
		ALLOC_STRUCT(Network, struct tt__NetworkCapabilities);
		ALLOC_STRUCT(Network->DynDNS, enum xsd__boolean_);
		*Network->DynDNS = xsd__boolean__false_;
		ALLOC_STRUCT(Network->IPVersion6, enum xsd__boolean_);
		*Network->IPVersion6 = xsd__boolean__false_;
		ALLOC_STRUCT(Network->ZeroConfiguration, enum xsd__boolean_);
		*Network->ZeroConfiguration = xsd__boolean__false_;
		ALLOC_STRUCT(Network->IPFilter, enum xsd__boolean_);
		*Network->IPFilter = xsd__boolean__false_;	
		ALLOC_STRUCT(Network->Extension, struct tt__NetworkCapabilitiesExtension);
		Network->Extension->__size = 0;
		Network->Extension->__any = NULL;		
		ALLOC_STRUCT(Network->Extension->Dot11Configuration, enum xsd__boolean_);
		*Network->Extension->Dot11Configuration = xsd__boolean__false_;
		Network->Extension->Extension = NULL;
		Network->Extension->__size = 0;
		Network->Extension->__any = NULL;
		Network->__anyAttribute = NULL;
		Device->Network = Network;

		ALLOC_STRUCT(System, struct tt__SystemCapabilities);			
		System->FirmwareUpgrade = xsd__boolean__false_;		
		System->SystemLogging = xsd__boolean__true_;	
		System->SystemBackup = xsd__boolean__false_;	
		System->RemoteDiscovery = xsd__boolean__false_;	
		System->DiscoveryBye = xsd__boolean__false_;		
		System->DiscoveryResolve = xsd__boolean__false_;
		ALLOC_STRUCT(System->Extension, struct tt__SystemCapabilitiesExtension);	
		ALLOC_STRUCT(System->Extension->HttpSystemLogging, enum xsd__boolean_);
		*System->Extension->HttpSystemLogging = xsd__boolean__true_;
		ALLOC_STRUCT(System->Extension->HttpFirmwareUpgrade, enum xsd__boolean_);
		*System->Extension->HttpFirmwareUpgrade = xsd__boolean__true_;
		ALLOC_STRUCT(System->Extension->HttpSystemBackup, enum xsd__boolean_);
		*System->Extension->HttpSystemBackup = xsd__boolean__false_;
		ALLOC_STRUCT(System->Extension->HttpSupportInformation, enum xsd__boolean_);
		*System->Extension->HttpSupportInformation = xsd__boolean__true_;
		System->Extension->__size = 0;
		System->Extension->__any = NULL;
		System->Extension->Extension = NULL;
		System->__sizeSupportedVersions = 4;		
		ALLOC_STRUCT_NUM(System->SupportedVersions, struct tt__OnvifVersion, 4);
		System->SupportedVersions[0].Major = 2;
		System->SupportedVersions[0].Minor= 10;
		System->SupportedVersions[1].Major = 2;
		System->SupportedVersions[1].Minor= 0;
		System->SupportedVersions[2].Major = 1;
		System->SupportedVersions[2].Minor= 2;
		System->SupportedVersions[3].Major = 1;
		System->SupportedVersions[3].Minor= 1;
		System->__anyAttribute = NULL;
		Device->System = System;

		ALLOC_STRUCT(IO, struct tt__IOCapabilities);
		ALLOC_STRUCT(IO->InputConnectors, int);
		*IO->InputConnectors = 2;
		ALLOC_STRUCT(IO->RelayOutputs, int);
		*IO->RelayOutputs = 2;
		IO->Extension = NULL;
		IO->__anyAttribute = NULL;
		Device->IO = IO;
		
		ALLOC_STRUCT(Security, struct tt__SecurityCapabilities);		
		Security->RELToken = xsd__boolean__false_; 		
		Security->KerberosToken = xsd__boolean__false_;		
		Security->SAMLToken = xsd__boolean__false_;		
		Security->X_x002e509Token = xsd__boolean__false_;	
		Security->AccessPolicyConfig = xsd__boolean__false_;			
		Security->OnboardKeyGeneration = xsd__boolean__false_;		
		Security->TLS1_x002e2 = xsd__boolean__false_;		
		Security->TLS1_x002e1 = xsd__boolean__false_;			
		ALLOC_STRUCT(Security->Extension, struct tt__SecurityCapabilitiesExtension);		
		Security->Extension->TLS1_x002e0 = xsd__boolean__false_;
		Security->Extension->Extension = NULL;
		Security->__anyAttribute = NULL;
		Security->__size = 0;
		Security->__any = NULL;
		Security->__anyAttribute = NULL;
		Device->Security = Security;	

		Device->__anyAttribute = NULL;
		
	}
	if(Events)
	{		
		ALLOC_TOKEN(Events->XAddr, onvif_get_device_xaddr(ip));
		printf("Events:%s\r\n", Events->XAddr);	
		Events->WSSubscriptionPolicySupport = 	xsd__boolean__true_;
		Events->WSPullPointSupport = xsd__boolean__true_;
		Events->WSPausableSubscriptionManagerInterfaceSupport = xsd__boolean__true_;
		Events->__size = 0;
		Events->__any = NULL;
		Events->__anyAttribute = NULL;
	}
	
	if(Imaging)
	{			
		ALLOC_TOKEN(Imaging->XAddr, onvif_get_imaging_xaddr(ip));	
		printf("Imaging.xAddr:%s\r\n", Imaging->XAddr);
		Imaging->__anyAttribute = NULL;
	}
	
	if(Media)
	{		
		ALLOC_TOKEN(Media->XAddr, onvif_get_media_xaddr(ip));
		printf("xAddr:%s\r\n", Media->XAddr);
		ALLOC_STRUCT(Media->StreamingCapabilities, struct tt__RealTimeStreamingCapabilities);
		ALLOC_STRUCT(Media->StreamingCapabilities->RTPMulticast,  enum xsd__boolean_);
		*Media->StreamingCapabilities->RTPMulticast = xsd__boolean__false_;
		printf("----1---\r\n");
		ALLOC_STRUCT(Media->StreamingCapabilities->RTP_USCORETCP,  enum xsd__boolean_);
		*Media->StreamingCapabilities->RTP_USCORETCP = xsd__boolean__false_;
		printf("----2---\r\n");
		ALLOC_STRUCT(Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP,  enum xsd__boolean_);
		*Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP = xsd__boolean__true_;
		printf("----3---\r\n");
		Media->StreamingCapabilities->__anyAttribute = NULL;
		Media->StreamingCapabilities->Extension = NULL;
		ALLOC_STRUCT(Media->Extension, struct tt__MediaCapabilitiesExtension);
		printf("----4---\r\n");
		ALLOC_STRUCT(Media->Extension->ProfileCapabilities, struct tt__ProfileCapabilities);
		Media->Extension->ProfileCapabilities->MaximumNumberOfProfiles = 7;
		Media->Extension->ProfileCapabilities->__size = 0;
		Media->Extension->ProfileCapabilities->__any = NULL;
		Media->Extension->ProfileCapabilities->__anyAttribute = NULL;
		Media->Extension->__size = 0;
		Media->Extension->__any = NULL;
		Media->Extension->__anyAttribute = NULL;
		Media->__size = 0;
		Media->__any = NULL;
		Media->__anyAttribute = NULL;
	}
	
	if(PTZ)
	{			
		ALLOC_TOKEN(PTZ->XAddr, onvif_get_ptz_xaddr(ip));
		PTZ->__size = 0;
		PTZ->__any = NULL;
		PTZ->__anyAttribute = NULL;
		printf("PTZ:%s\r\n", PTZ->XAddr);	
	}
	
	if(Extension)
	{
		ALLOC_STRUCT(Extension->DeviceIO, struct tt__DeviceIOCapabilities);		
		ALLOC_TOKEN(Extension->DeviceIO->XAddr, onvif_get_deviceio_xaddr(ip));
		printf("Extension->DeviceIO->XAddr:%s\r\n", Extension->DeviceIO->XAddr);	
		Extension->DeviceIO->VideoSources = 1;
		Extension->DeviceIO->VideoOutputs = 0;
		Extension->DeviceIO->AudioSources = 1;
		Extension->DeviceIO->AudioOutputs = 1;
		Extension->DeviceIO->RelayOutputs = 1;	
		Extension->DeviceIO->__size = 0;
		Extension->DeviceIO->__any = NULL;
		Extension->DeviceIO->__anyAttribute = NULL;
		Extension->Display = NULL;

		ALLOC_STRUCT(Extension->Recording, struct tt__RecordingCapabilities);
		ALLOC_TOKEN(Extension->Recording->XAddr, onvif_get_recordingcontrol_xaddr(ip));
		Extension->Recording->ReceiverSource = xsd__boolean__false_;
		Extension->Recording->MediaProfileSource = xsd__boolean__true_;
		Extension->Recording->DynamicTracks = xsd__boolean__false_;
		Extension->Recording->DynamicRecordings = xsd__boolean__false_;
		Extension->Recording->MaxStringLength = 128;		
		Extension->Recording->__size = 0;
		Extension->Recording->__any = NULL;
		Extension->Recording->__anyAttribute = NULL;
		
		ALLOC_STRUCT(Extension->Search, struct tt__SearchCapabilities);
		ALLOC_STRUCT_NUM(Extension->Search->XAddr, char, ONVIF_DEVICE_SERVICE_ADDR_SIZE);
		strcpy(Extension->Search->XAddr, onvif_get_recordingsearch_xaddr(ip));
		Extension->Search->MetadataSearch = xsd__boolean__false_;
		Extension->Search->__size = 0;
		Extension->Search->__any = NULL;
		Extension->Search->__anyAttribute = NULL;
		ALLOC_STRUCT(Extension->Replay, struct tt__ReplayCapabilities);
		ALLOC_TOKEN(Extension->Replay->XAddr, onvif_get_replay_xaddr(ip));
		Extension->Replay->__size = 0;
		Extension->Replay->__any = NULL;
		Extension->Replay->__anyAttribute = NULL;
		Extension->__size = 0;
		Extension->__any = NULL;		
		
	}

	Capabilities->Analytics = Analytics;
	Capabilities->Device = Device;
	Capabilities->Events = Events;
	Capabilities->Imaging = Imaging;
	Capabilities->Media = Media;
	Capabilities->PTZ = PTZ;
	Capabilities->Extension = Extension;
	Capabilities->__anyAttribute = NULL;

	tds__GetCapabilitiesResponse->Capabilities = Capabilities;
	
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tds__SetDPAddresses(struct soap* soap, struct _tds__SetDPAddresses *tds__SetDPAddresses, struct _tds__SetDPAddressesResponse *tds__SetDPAddressesResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:SetDiscoveryProxyNotSupported");
	return SOAP_FAULT;
}

 int  __tds__GetHostname(struct soap* soap, struct _tds__GetHostname *tds__GetHostname, struct _tds__GetHostnameResponse *tds__GetHostnameResponse) 
{
	__FUN_BEGIN("\n");
	NVR_DEV_NET_CFG_S stNet;
	struct tt__HostnameInformation *HostnameInformation;

	memset(&stNet, 0, sizeof(NVR_DEV_NET_CFG_S));
	nvr_logon_get_net_cfg(0, &stNet);
	
	if((HostnameInformation = (struct tt__HostnameInformation *)soap_malloc(soap, sizeof(struct tt__HostnameInformation))) == NULL)
	{
			__E("Failed to to malloc for HostnameInformation.\n");
			return SOAP_FAULT;
	}
	
	HostnameInformation->Extension = NULL;
	HostnameInformation->__anyAttribute = NULL;
	HostnameInformation->Name = "localhost";

	unsigned int ipaddr = htonl(net_get_sock_ip(soap->socket));
	
	if(ipaddr == stNet.struEtherCfg[0].u32IpAddr)
		HostnameInformation->FromDHCP = !!stNet.struEtherCfg[0].u8DhcpOn;
	else if(ipaddr == stNet.struEtherCfg[1].u32IpAddr)
		HostnameInformation->FromDHCP = !!stNet.struEtherCfg[1].u8DhcpOn;
	tds__GetHostnameResponse->HostnameInformation = HostnameInformation;
	__FUN_END("\n");
	return SOAP_OK;	
}

 int  __tds__SetHostname(struct soap* soap, struct _tds__SetHostname *tds__SetHostname, struct _tds__SetHostnameResponse *tds__SetHostnameResponse) 
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__SetHostnameFromDHCP(struct soap* soap, struct _tds__SetHostnameFromDHCP *tds__SetHostnameFromDHCP, struct _tds__SetHostnameFromDHCPResponse *tds__SetHostnameFromDHCPResponse) 
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetDNS(struct soap* soap, struct _tds__GetDNS *tds__GetDNS, struct _tds__GetDNSResponse *tds__GetDNSResponse) 
{
	NVR_DEV_NET_CFG_S stNetCfg;
	//HI_DEV_ONVIF_CFG_S stOnvifCfg;
	__FUN_BEGIN("\n");
	__D("\n");
	//hi_platform_get_onvif_cfg(0, &stOnvifCfg);
	nvr_logon_get_net_cfg(0, &stNetCfg);
	tds__GetDNSResponse->DNSInformation = (struct tt__DNSInformation *)soap_malloc(soap, sizeof(struct tt__DNSInformation));
	if(tds__GetDNSResponse->DNSInformation == NULL)
	{
		__E("Failed to malloc for dns information.\n");
		return SOAP_FAULT;
	}
	memset(tds__GetDNSResponse->DNSInformation, 0, sizeof(struct tt__DNSInformation));
	
	unsigned int ip = htonl(net_get_sock_ip(soap->socket));	
	
	if(ip == stNetCfg.struEtherCfg[0].u32IpAddr)
		tds__GetDNSResponse->DNSInformation->FromDHCP = !!stNetCfg.struEtherCfg[0].u8DhcpOn;
	else if(ip == stNetCfg.struEtherCfg[1].u32IpAddr)
		tds__GetDNSResponse->DNSInformation->FromDHCP = !!stNetCfg.struEtherCfg[1].u8DhcpOn;
#if 1
	tds__GetDNSResponse->DNSInformation->__sizeSearchDomain = 0;
	tds__GetDNSResponse->DNSInformation->SearchDomain = NULL;
#else
	tds__GetDNSResponse->DNSInformation->__sizeSearchDomain = 1;
	tds__GetDNSResponse->DNSInformation->SearchDomain = (char **)soap_malloc(soap, sizeof(char *));
	if(tds__GetDNSResponse->DNSInformation->SearchDomain == NULL)
	{
		__E("Failed to malloc for search domain.\n");
		return SOAP_FAULT;
	}
	tds__GetDNSResponse->DNSInformation->SearchDomain[0] = (char *)soap_malloc(soap, NVR_IP_ADDR_LEN);
	if(tds__GetDNSResponse->DNSInformation->SearchDomain[0] == NULL)
	{
		__E("Failed to malloc for SearchDomain[0].\n");
		return SOAP_FAULT;
	}
	strcpy(tds__GetDNSResponse->DNSInformation->SearchDomain[0], "www.szjxj.com");
#endif
	if(stNetCfg.u8DnsDhcp)
	{
		tds__GetDNSResponse->DNSInformation->__sizeDNSFromDHCP = 2;
		tds__GetDNSResponse->DNSInformation->DNSFromDHCP = soap_malloc(soap, sizeof(struct tt__IPAddress));
		if(tds__GetDNSResponse->DNSInformation->DNSFromDHCP == NULL)
		{
			__E("Failed to malloc for dns from dhcp.\n");
			return SOAP_FAULT;
		}
		tds__GetDNSResponse->DNSInformation->DNSFromDHCP[0].Type = tt__IPType__IPv4;
		tds__GetDNSResponse->DNSInformation->DNSFromDHCP[0].IPv4Address = (char *)soap_malloc(soap, sizeof(char) * NVR_IP_ADDR_LEN);
		if(tds__GetDNSResponse->DNSInformation->DNSFromDHCP[0].IPv4Address == NULL)
		{
			__E("Failed to malloc for ipv4 address.\n");
			return SOAP_FAULT;
		}__D("\n");
		snprintf(tds__GetDNSResponse->DNSInformation->DNSFromDHCP[0].IPv4Address,
				NVR_IP_ADDR_LEN,
				"%d.%d.%d.%d", 
				(stNetCfg.u32DnsIp1 >> 24) & 0xff,
				(stNetCfg.u32DnsIp1 >> 16) & 0xff,
				(stNetCfg.u32DnsIp1 >> 8) & 0xff,
				(stNetCfg.u32DnsIp1 >> 0) & 0xff);
		tds__GetDNSResponse->DNSInformation->DNSFromDHCP[0].IPv6Address = NULL;

		tds__GetDNSResponse->DNSInformation->DNSFromDHCP[1].Type = tt__IPType__IPv4;
		tds__GetDNSResponse->DNSInformation->DNSFromDHCP[1].IPv4Address = (char *)soap_malloc(soap, sizeof(char) * NVR_IP_ADDR_LEN);
		if(tds__GetDNSResponse->DNSInformation->DNSFromDHCP[1].IPv4Address == NULL)
		{
			__E("Failed to malloc for ipv4 address[0].\n");
			return SOAP_FAULT;
		}__D("\n");
		snprintf(tds__GetDNSResponse->DNSInformation->DNSFromDHCP[1].IPv4Address,
				NVR_IP_ADDR_LEN,
				"%d.%d.%d.%d", 
				(stNetCfg.u32DnsIp2 >> 24) & 0xff,
				(stNetCfg.u32DnsIp2 >> 16) & 0xff,
				(stNetCfg.u32DnsIp2 >> 8) & 0xff,
				(stNetCfg.u32DnsIp2 >> 0) & 0xff);
		tds__GetDNSResponse->DNSInformation->DNSFromDHCP[1].IPv6Address = NULL;
	}
	else
	{
	__D("\n");
		tds__GetDNSResponse->DNSInformation->__sizeDNSManual = 2;
		tds__GetDNSResponse->DNSInformation->DNSManual = soap_malloc(soap, sizeof(struct tt__IPAddress) * 2);
		if(tds__GetDNSResponse->DNSInformation->DNSManual == NULL)
		{
			__E("Failed to malloc for dns manual.\n");
			return SOAP_FAULT;
		}
		tds__GetDNSResponse->DNSInformation->DNSManual[0].Type = tt__IPType__IPv4;
		__D("\n");
		tds__GetDNSResponse->DNSInformation->DNSManual[0].IPv4Address = (char *)soap_malloc(soap, sizeof(char) * NVR_IP_ADDR_LEN);
		if(tds__GetDNSResponse->DNSInformation->DNSManual[0].IPv4Address == NULL)
		{
			__E("Failed to malloc for ipv4 address.\n");
			return SOAP_FAULT;
		}		
		snprintf(tds__GetDNSResponse->DNSInformation->DNSManual[0].IPv4Address,
				NVR_IP_ADDR_LEN,
				"%d.%d.%d.%d", 
				(stNetCfg.u32DnsIp1 >> 24) & 0xff,
				(stNetCfg.u32DnsIp1 >> 16) & 0xff,
				(stNetCfg.u32DnsIp1 >> 8) & 0xff,
				(stNetCfg.u32DnsIp1 >> 0) & 0xff);
		tds__GetDNSResponse->DNSInformation->DNSManual[0].IPv6Address = NULL;

		tds__GetDNSResponse->DNSInformation->DNSManual[1].Type = tt__IPType__IPv4;
		__D("\n");
		tds__GetDNSResponse->DNSInformation->DNSManual[1].IPv4Address = (char *)soap_malloc(soap, sizeof(char) * NVR_IP_ADDR_LEN);
		if(tds__GetDNSResponse->DNSInformation->DNSManual[1].IPv4Address == NULL)
		{
			__E("Failed to malloc for ipv4 address.\n");
			return SOAP_FAULT;
		}		
		snprintf(tds__GetDNSResponse->DNSInformation->DNSManual[1].IPv4Address,
				NVR_IP_ADDR_LEN,
				"%d.%d.%d.%d", 
				(stNetCfg.u32DnsIp2 >> 24) & 0xff,
				(stNetCfg.u32DnsIp2 >> 16) & 0xff,
				(stNetCfg.u32DnsIp2 >> 8) & 0xff,
				(stNetCfg.u32DnsIp2 >> 0) & 0xff);
		tds__GetDNSResponse->DNSInformation->DNSManual[1].IPv6Address = NULL;
	}
	tds__GetDNSResponse->DNSInformation->Extension = NULL;
	tds__GetDNSResponse->DNSInformation->__anyAttribute = NULL;
	__FUN_END("\n");

	return SOAP_OK;
}

 int  __tds__SetDNS(struct soap* soap, struct _tds__SetDNS *tds__SetDNS, struct _tds__SetDNSResponse *tds__SetDNSResponse) 
{
	int i;
	NVR_DEV_NET_CFG_S stNetCfg;
	//HI_DEV_ONVIF_CFG_S stOnvifCfg;
	unsigned int u32IpAddr;
	__FUN_BEGIN("\n");
	
	nvr_logon_get_net_cfg(0, &stNetCfg);
	//hi_platform_get_onvif_cfg(0, &stOnvifCfg);
	if(tds__SetDNS->FromDHCP)
	{
		stNetCfg.u8DnsDhcp = 1;
	}
#if 0	
	if(tds__SetDNS->__sizeSearchDomain > 0)
	{
		if(tds__SetDNS->SearchDomain != NULL)
		{
			if(tds__SetDNS->SearchDomain[0] != NULL)
			{
				strncpy(stOnvifCfg.szDomain, tds__SetDNS->SearchDomain[0], sizeof(stOnvifCfg.szDomain) - 1);
				stOnvifCfg.szDomain[sizeof(stOnvifCfg.szDomain) - 1] = '\0';
				hi_platform_set_onvif_cfg(0, &stOnvifCfg);
			}
		}
	}
#endif	
	if(tds__SetDNS->DNSManual != NULL)
	{
		int dnsIndex = 0;
		for(i = 0; i < tds__SetDNS->__sizeDNSManual && dnsIndex < 2; i++)
		{
			if(tds__SetDNS->DNSManual[i].Type == tt__IPType__IPv6)
			{
				onvif_fault(soap, "ter:InvalidArgVal", "ter:InvalidIPv6Address");
				return SOAP_FAULT;
			}
			if(tds__SetDNS->DNSManual[i].IPv4Address == NULL || 	
				!isValidIp4(tds__SetDNS->DNSManual[i].IPv4Address))
			{
				onvif_fault(soap, "ter:InvalidArgVal", "ter:InvalidIPv4Address");
				return SOAP_FAULT;
			}
			
			u32IpAddr = net_ip_a2n(tds__SetDNS->DNSManual[i].IPv4Address);
			if(dnsIndex == 0)
			{
				stNetCfg.u32DnsIp1 = u32IpAddr;
			}
			else if(dnsIndex == 1)
			{	
				stNetCfg.u32DnsIp2 = u32IpAddr;
			}
			dnsIndex++;
		}
	}
	nvr_logon_set_net_cfg(0, &stNetCfg);
	__FUN_END("\n");

	return SOAP_OK;
}

 int  __tds__GetNTP(struct soap* soap, struct _tds__GetNTP *tds__GetNTP, struct _tds__GetNTPResponse *tds__GetNTPResponse) 
{
	NVR_NTP_CFG_S stNtpCfg;
	struct tt__NTPInformation *NTPInformation;

	__FUN_BEGIN("\n");
	nvr_logon_get_ntp_cfg(0, &stNtpCfg);
	NTPInformation = (struct tt__NTPInformation*)soap_malloc(soap, sizeof(struct tt__NTPInformation));
	if(NTPInformation == NULL)
	{
		__E("Failed to malloc for ntp information.\n");
		return SOAP_FAULT;
	}
	memset(NTPInformation, 0, sizeof(struct tt__NTPInformation));
	NTPInformation->FromDHCP = xsd__boolean__false_;
	NTPInformation->__sizeNTPFromDHCP = 0;
	NTPInformation->NTPFromDHCP = NULL;
	NTPInformation->__sizeNTPManual = 1;
	if(stNtpCfg.u8NtpOpen)
	{
		NTPInformation->NTPManual = (struct tt__NetworkHost *)soap_malloc(soap, sizeof(struct tt__NetworkHost));
		if(NTPInformation->NTPManual == NULL)
		{
			__E("Failed to malloc for ntp manual.\n");
			return SOAP_FAULT;
		}
		NTPInformation->NTPManual->Type = tt__NetworkHostType__IPv4;
		NTPInformation->NTPManual->IPv4Address = (char *)soap_malloc(soap, NVR_IP_ADDR_LEN*sizeof(char));
		if(NTPInformation->NTPManual->IPv4Address == NULL)
		{
			__E("Failed to malloc for dns name.\n");
			return SOAP_FAULT;
		}
		strncpy(NTPInformation->NTPManual->IPv4Address,
				stNtpCfg.szNtpServer, sizeof(stNtpCfg.szNtpServer)-1);
		NTPInformation->NTPManual->IPv4Address[NVR_IP_ADDR_LEN - 1] = '\0';
		//NTPInformation->NTPManual->IPv6Address = NULL;
		NTPInformation->NTPManual->DNSname = NULL;
	}
	else
	{
		NTPInformation->NTPManual = NULL;
	}
	NTPInformation->Extension = NULL;
	NTPInformation->__anyAttribute = NULL;
	tds__GetNTPResponse->NTPInformation = NTPInformation;
	__FUN_END("\n");

	return SOAP_OK;
}

 int  __tds__SetNTP(struct soap* soap, struct _tds__SetNTP *tds__SetNTP, struct _tds__SetNTPResponse *tds__SetNTPResponse) 
{
	NVR_NTP_CFG_S stNtpCfg;
	__FUN_BEGIN("\n");
	if (tds__SetNTP->FromDHCP == xsd__boolean__true_) 
	{
		onvif_fault(soap,"ter:NotSupported", "ter:SetDHCPNotAllowed");
		return SOAP_FAULT;
	}
	if(tds__SetNTP->__sizeNTPManual > 0 && tds__SetNTP->NTPManual != NULL)
	{
		if (tds__SetNTP->NTPManual->IPv6Address != NULL || tds__SetNTP->NTPManual->Type == tt__NetworkHostType__IPv6)
		{
			onvif_fault(soap,"ter:InvalidArgVal", "ter:InvalidIPv6Address");
			return SOAP_FAULT;
		}
		if(tds__SetNTP->NTPManual->DNSname != NULL || tds__SetNTP->NTPManual->Type == tt__NetworkHostType__DNS)
		{
			onvif_fault(soap,"ter:InvalidArgVal", "ter:InvalidDnsName");
			return SOAP_FAULT;
		}
		if(tds__SetNTP->NTPManual->IPv4Address != NULL  && tds__SetNTP->NTPManual->Type == tt__NetworkHostType__IPv4)
		{
			nvr_logon_get_ntp_cfg(0, &stNtpCfg);
			if(!isValidIp4(tds__SetNTP->NTPManual->IPv4Address))
			{
				onvif_fault(soap,"ter:InvalidArgVal", "ter:InvalidIPv4Address");
				return SOAP_FAULT;
			}
			strncpy(stNtpCfg.szNtpServer, tds__SetNTP->NTPManual->IPv4Address, sizeof(stNtpCfg.szNtpServer) - 1);
			stNtpCfg.szNtpServer[sizeof(stNtpCfg.szNtpServer) - 1] = '\0';

			printf("=========onvif SetNTP=======:%d\r\n", stNtpCfg.nTimeZone);
			nvr_logon_set_ntp_cfg(0, &stNtpCfg);
		}
	}
	__FUN_END("\n");
	return SOAP_OK; 
}

 int  __tds__GetDynamicDNS(struct soap* soap, struct _tds__GetDynamicDNS *tds__GetDynamicDNS, struct _tds__GetDynamicDNSResponse *tds__GetDynamicDNSResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:DynamicDNSNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__SetDynamicDNS(struct soap* soap, struct _tds__SetDynamicDNS *tds__SetDynamicDNS, struct _tds__SetDynamicDNSResponse *tds__SetDynamicDNSResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:DynamicDNSNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetNetworkInterfaces(struct soap* soap, struct _tds__GetNetworkInterfaces *tds__GetNetworkInterfaces, struct _tds__GetNetworkInterfacesResponse *tds__GetNetworkInterfacesResponse) 
{
	NVR_DEV_NET_CFG_S stNetCfg;
	unsigned char *pMac;
	unsigned int u32IpAddr;
	unsigned char u8DhcpOn = 0;
	char _mac[64];
	char _IPAddr[64];
	char *pEthName = NULL;
	int nEth = 0;
	struct tt__NetworkInterface *NetworkInterfaces;
	__FUN_BEGIN("\n");
	
	unsigned long ip;

	ALLOC_STRUCT(NetworkInterfaces, struct tt__NetworkInterface);
	nvr_logon_get_net_cfg(0, &stNetCfg);
	
	ip = htonl(net_get_sock_ip(soap->socket));
	
	if(ip == stNetCfg.struEtherCfg[0].u32IpAddr)
	{
		printf("getinterface:eth0\r\n");
		pMac = stNetCfg.struEtherCfg[0].u8MacAddr;
		u32IpAddr = stNetCfg.struEtherCfg[0].u32IpAddr;
		u8DhcpOn = !!stNetCfg.struEtherCfg[0].u8DhcpOn;
		pEthName = ETH_NAME;
		nEth = 0;
	}else
	{
		printf("getinterface:ra0\r\n");
		pMac = stNetCfg.struEtherCfg[1].u8MacAddr;
		u32IpAddr = stNetCfg.struEtherCfg[1].u32IpAddr;
		u8DhcpOn = !!stNetCfg.struEtherCfg[1].u8DhcpOn;
		pEthName = ETH_NAME1;
		nEth = 1;
	}
	
	sprintf(_mac, "%02X:%02X:%02X:%02X:%02X:%02X", pMac[0], pMac[1], pMac[2], pMac[3], pMac[4], pMac[5]);
	sprintf(_IPAddr, "%d.%d.%d.%d", (u32IpAddr>>24) & 0xff, (u32IpAddr>>16) & 0xff, (u32IpAddr>>8) & 0xff,
			(u32IpAddr>>0) & 0xff);
	
	NetworkInterfaces->Enabled = xsd__boolean__true_;
	ALLOC_TOKEN(NetworkInterfaces->token, pEthName);	

	ALLOC_STRUCT(NetworkInterfaces->Info, struct tt__NetworkInterfaceInfo);
	ALLOC_TOKEN(NetworkInterfaces->Info->Name, pEthName);
	ALLOC_TOKEN(NetworkInterfaces->Info->HwAddress, _mac);
	ALLOC_STRUCT_INT(NetworkInterfaces->Info->MTU, int, 1500);
	ALLOC_STRUCT(NetworkInterfaces->Link, struct tt__NetworkInterfaceLink);
	ALLOC_STRUCT(NetworkInterfaces->Link->AdminSettings, struct tt__NetworkInterfaceConnectionSetting);
	switch(stNetCfg.struEtherCfg[nEth].u8NetcardSpeed == 0)
	{
		case 0:
			NetworkInterfaces->Link->AdminSettings->AutoNegotiation = xsd__boolean__true_;
			NetworkInterfaces->Link->AdminSettings->Speed = 100;
			NetworkInterfaces->Link->AdminSettings->Duplex = tt__Duplex__Full;
			break;
		case 1:
			NetworkInterfaces->Link->AdminSettings->AutoNegotiation = xsd__boolean__false_;
			NetworkInterfaces->Link->AdminSettings->Speed = 100;
			NetworkInterfaces->Link->AdminSettings->Duplex = tt__Duplex__Full;
			break;
		case 2:
			NetworkInterfaces->Link->AdminSettings->AutoNegotiation = xsd__boolean__false_;
			NetworkInterfaces->Link->AdminSettings->Speed = 10;
			NetworkInterfaces->Link->AdminSettings->Duplex = tt__Duplex__Full;
			break;
	}
	ALLOC_STRUCT(NetworkInterfaces->Link->OperSettings, struct tt__NetworkInterfaceConnectionSetting);
	NetworkInterfaces->Link->OperSettings->AutoNegotiation = xsd__boolean__true_;
	NetworkInterfaces->Link->OperSettings->Speed = 0;
	NetworkInterfaces->Link->OperSettings->Duplex = tt__Duplex__Full;
	ALLOC_STRUCT(NetworkInterfaces->IPv4, struct tt__IPv4NetworkInterface);
	
	NetworkInterfaces->IPv4->Enabled = xsd__boolean__true_;
	ALLOC_STRUCT(NetworkInterfaces->IPv4->Config, struct tt__IPv4Configuration);
	NetworkInterfaces->IPv4->Config->__sizeManual = 1;
	ALLOC_STRUCT(NetworkInterfaces->IPv4->Config->Manual, struct tt__PrefixedIPv4Address);
	ALLOC_TOKEN(NetworkInterfaces->IPv4->Config->Manual->Address, _IPAddr);
	NetworkInterfaces->IPv4->Config->Manual->PrefixLength = 24;
 	NetworkInterfaces->IPv4->Config->LinkLocal = NULL;
	NetworkInterfaces->IPv4->Config->FromDHCP = NULL;
	NetworkInterfaces->IPv4->Config->DHCP = u8DhcpOn;
	NetworkInterfaces->IPv4->Config->__size = 0;
	NetworkInterfaces->IPv4->Config->__any = NULL;
	NetworkInterfaces->IPv4->Config->__anyAttribute = NULL;
	NetworkInterfaces->IPv6 = NULL;
	NetworkInterfaces->Extension = NULL;
	NetworkInterfaces->__anyAttribute = NULL;

	tds__GetNetworkInterfacesResponse->__sizeNetworkInterfaces = 1;
	tds__GetNetworkInterfacesResponse->NetworkInterfaces = NetworkInterfaces;
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tds__SetNetworkInterfaces(struct soap* soap, struct _tds__SetNetworkInterfaces *tds__SetNetworkInterfaces, struct _tds__SetNetworkInterfacesResponse *tds__SetNetworkInterfacesResponse) 
{
	NVR_DEV_NET_CFG_S stNetCfg;
	NVR_DEV_ETHERNET_CFG_S *pEthernerCfg;
	char *pEthName = NULL;
	__FUN_BEGIN("\n");

	nvr_logon_get_net_cfg(0, &stNetCfg);
	unsigned long ip = htonl(net_get_sock_ip(soap->socket));	
	
	if(ip == stNetCfg.struEtherCfg[0].u32IpAddr)
	{
		pEthernerCfg = &stNetCfg.struEtherCfg[0];
		pEthName = ETH_NAME;
	}
	else
	{
		pEthernerCfg = &stNetCfg.struEtherCfg[1];
		pEthName = ETH_NAME1;
	}

	
	if(tds__SetNetworkInterfaces->InterfaceToken != NULL)
	{
		printf("ethname:%s\r\n", tds__SetNetworkInterfaces->InterfaceToken);
		if(strcmp(tds__SetNetworkInterfaces->InterfaceToken, pEthName))
		{
			onvif_fault(soap,"ter:InvalidArgVal", "ter:InvalidNetworkInterface");
			return SOAP_FAULT;
		}
	}
	
	if(tds__SetNetworkInterfaces->NetworkInterface == NULL)
	{
		onvif_fault(soap,"ter:InvalidArgVal", "ter:InvalidNetworkInterface");
		return SOAP_FAULT;
	}
/*	
	if(tds__SetNetworkInterfaces->NetworkInterface->MTU != NULL)
	{
		onvif_fault(soap,"ter:InvalidArgVal", "ter:SettingMTUNotSupported");
		return SOAP_FAULT;
	}
	if(tds__SetNetworkInterfaces->NetworkInterface->Link != NULL)
	{
		onvif_fault(soap,"ter:InvalidArgVal", "ter:SettingLinkValuesNotSupported");
		return SOAP_FAULT;
	}
*/
/*
	if(tds__SetNetworkInterfaces->NetworkInterface->IPv6 != NULL)
	{
		if(*tds__SetNetworkInterfaces->NetworkInterface->IPv6->Enabled)
		{	
			onvif_fault(soap,"ter:InvalidArgVal", "ter:IPv6NotSupported");
			return SOAP_FAULT;
		}
	}	
*/	
	if(tds__SetNetworkInterfaces->NetworkInterface->IPv4 == NULL)
	{
		onvif_fault(soap,"ter:InvalidArgVal", "ter:IPv4ValuesMissing");
		return SOAP_FAULT;
	}
	
	if(tds__SetNetworkInterfaces->NetworkInterface->IPv4->Enabled != NULL)
	{
		if(tds__SetNetworkInterfaces->NetworkInterface->IPv4->Manual != NULL)
		{
			if(tds__SetNetworkInterfaces->NetworkInterface->IPv4->Manual->Address != NULL)
			{
				if(isValidIp4(tds__SetNetworkInterfaces->NetworkInterface->IPv4->Manual->Address) == 0) // Check IP address
				{
					onvif_fault(soap, "ter:InvalidArgVal", "ter:InvalidIPv4Address");
					return SOAP_FAULT;
				}
				pEthernerCfg->u32IpAddr = net_ip_a2n(tds__SetNetworkInterfaces->NetworkInterface->IPv4->Manual->Address);
			
			}
		}
		
		if(tds__SetNetworkInterfaces->NetworkInterface->IPv4->DHCP != NULL)
		{
			pEthernerCfg->u8DhcpOn = *tds__SetNetworkInterfaces->NetworkInterface->IPv4->DHCP;
		}
		
		nvr_logon_set_net_cfg(0, &stNetCfg);
	}
	
	tds__SetNetworkInterfacesResponse->RebootNeeded = xsd__boolean__true_;
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tds__GetNetworkProtocols(struct soap* soap, struct _tds__GetNetworkProtocols *tds__GetNetworkProtocols, struct _tds__GetNetworkProtocolsResponse *tds__GetNetworkProtocolsResponse) 
{
	struct tt__NetworkProtocol *NetworkProtocols;
	__FUN_BEGIN("\n");
	tds__GetNetworkProtocolsResponse->__sizeNetworkProtocols = 2;
	NetworkProtocols = (struct tt__NetworkProtocol *)soap_malloc(soap,tds__GetNetworkProtocolsResponse->__sizeNetworkProtocols * sizeof(struct tt__NetworkProtocol)); 
	if(NetworkProtocols == NULL)
	{
		__E("Failed to malloc for network protocols.\n");
		return SOAP_FAULT;
	}
	NetworkProtocols[0].Name = tt__NetworkProtocolType__HTTP;
	NetworkProtocols[0].Enabled = xsd__boolean__true_;
	NetworkProtocols[0].__sizePort = 1;
	NetworkProtocols[0].Port = (int *)soap_malloc(soap, sizeof(int));
	if(NetworkProtocols[0].Port == NULL)
	{
		__E("Failed to malloc for network protocols.\n");
		return SOAP_FAULT;
	}
	NetworkProtocols[0].Port[0] = DEFAULT_ONVIF_SERVER_PORT;
	NetworkProtocols[0].Extension = NULL;
	NetworkProtocols[0].__anyAttribute = NULL;
	
	NetworkProtocols[1].Name = tt__NetworkProtocolType__RTSP;	
	NetworkProtocols[1].Enabled = xsd__boolean__true_;
	NetworkProtocols[1].__sizePort = 1;
	NetworkProtocols[1].Port = (int *)soap_malloc(soap, NetworkProtocols[1].__sizePort * sizeof(int));
	if(NetworkProtocols[1].Port == NULL)
	{
		__E("Failed to malloc for port.\n");
		return SOAP_FAULT;
	}
	NetworkProtocols[1].Port[0] = 554;
	NetworkProtocols[1].Extension = NULL;
	NetworkProtocols[1].__anyAttribute = NULL;
	
	tds__GetNetworkProtocolsResponse->NetworkProtocols = NetworkProtocols;
	__FUN_END("\n");
	return SOAP_OK; 
}

 int  __tds__SetNetworkProtocols(struct soap* soap, struct _tds__SetNetworkProtocols *tds__SetNetworkProtocols, struct _tds__SetNetworkProtocolsResponse *tds__SetNetworkProtocolsResponse) 
{
	int i;
	NVR_DEV_NET_CFG_S stNetCfg;
	__FUN_BEGIN("\n");

	if(tds__SetNetworkProtocols->NetworkProtocols == NULL)
	{
		return SOAP_FAULT;
	}
	for(i = 0; i < tds__SetNetworkProtocols->__sizeNetworkProtocols; i++)
	{
		switch(tds__SetNetworkProtocols->NetworkProtocols[0].Name)
		{
			case tt__NetworkProtocolType__HTTP:
				nvr_logon_get_net_cfg(0, &stNetCfg);
				if(tds__SetNetworkProtocols->NetworkProtocols[0].Enabled == xsd__boolean__false_)
				{
					return SOAP_FAULT;
				}
				if(tds__SetNetworkProtocols->NetworkProtocols[0].__sizePort > 0 
					&& tds__SetNetworkProtocols->NetworkProtocols[0].Port != NULL)
				{
					stNetCfg.u16HttpPort = tds__SetNetworkProtocols->NetworkProtocols[0].Port[0];
				}
				nvr_logon_set_net_cfg(0, &stNetCfg);
				return SOAP_OK;
			case tt__NetworkProtocolType__HTTPS:
				onvif_fault(soap,"ter:InvalidArgVal", "ter:ServiceNotSupported");
				return SOAP_FAULT;
			case tt__NetworkProtocolType__RTSP:
				return SOAP_FAULT;
			default:
				return SOAP_FAULT;
		}
	}
	__FUN_END("\n");
	return SOAP_FAULT;
}

 int  __tds__GetNetworkDefaultGateway(struct soap* soap, struct _tds__GetNetworkDefaultGateway *tds__GetNetworkDefaultGateway, struct _tds__GetNetworkDefaultGatewayResponse *tds__GetNetworkDefaultGatewayResponse) 
{
	NVR_DEV_NET_CFG_S stNetCfg;
	unsigned int u32Gateway = 0;
	char _GatewayAddress[NVR_IP_ADDR_LEN];
	__FUN_BEGIN("\n");
	nvr_logon_get_net_cfg(0, &stNetCfg);
	
	unsigned int ip = htonl(net_get_sock_ip(soap->socket));
	
	if(ip == stNetCfg.struEtherCfg[0].u32IpAddr)
		u32Gateway = stNetCfg.struEtherCfg[0].u32GateWay;
	else
		u32Gateway = stNetCfg.struEtherCfg[1].u32GateWay;
	
	snprintf(_GatewayAddress, sizeof(_GatewayAddress) - 1, 
			"%d.%d.%d.%d", 
			(u32Gateway >> 24) & 0xff,
			(u32Gateway >> 16) & 0xff,
			(u32Gateway >> 8) & 0xff,
			(u32Gateway >> 0) & 0xff);
	printf("GateWayAddress:%s\r\n", _GatewayAddress);

	ALLOC_STRUCT(tds__GetNetworkDefaultGatewayResponse->NetworkGateway, struct tt__NetworkGateway);
	tds__GetNetworkDefaultGatewayResponse->NetworkGateway->__sizeIPv4Address = 1;
	tds__GetNetworkDefaultGatewayResponse->NetworkGateway->__sizeIPv6Address = 0;
	ALLOC_STRUCT(tds__GetNetworkDefaultGatewayResponse->NetworkGateway->IPv4Address, char*);
	ALLOC_TOKEN(tds__GetNetworkDefaultGatewayResponse->NetworkGateway->IPv4Address[0], _GatewayAddress);
	__FUN_END("\n");
	return SOAP_OK; 
}

 int  __tds__SetNetworkDefaultGateway(struct soap* soap, struct _tds__SetNetworkDefaultGateway *tds__SetNetworkDefaultGateway, struct _tds__SetNetworkDefaultGatewayResponse *tds__SetNetworkDefaultGatewayResponse) 
{
	NVR_DEV_NET_CFG_S stNetCfg;
	__FUN_BEGIN("\n");
	if(tds__SetNetworkDefaultGateway->__sizeIPv6Address ||
		!tds__SetNetworkDefaultGateway->__sizeIPv4Address ||
		isValidIp4(tds__SetNetworkDefaultGateway->IPv4Address) == 0)
	{
		onvif_fault(soap,"ter:InvalidArgVal", "ter:InvalidGatewayAddress");
		return SOAP_FAULT;
	}
	nvr_logon_get_net_cfg(0, &stNetCfg);
	unsigned long ip = htonl(net_get_sock_ip(soap->socket));
	
	if(ip == stNetCfg.struEtherCfg[0].u32IpAddr)
		stNetCfg.struEtherCfg[0].u32GateWay = net_ip_a2n(tds__SetNetworkDefaultGateway->IPv4Address);
	else 
		stNetCfg.struEtherCfg[1].u32GateWay = net_ip_a2n(tds__SetNetworkDefaultGateway->IPv4Address);
	
	nvr_logon_set_net_cfg(0, &stNetCfg);
	__FUN_END("\n");

	return SOAP_OK;
}

 int  __tds__GetZeroConfiguration(struct soap* soap, struct _tds__GetZeroConfiguration *tds__GetZeroConfiguration, struct _tds__GetZeroConfigurationResponse *tds__GetZeroConfigurationResponse) 
{
	char _IPAddr[NVR_IP_ADDR_LEN];
	unsigned int u32IpAddr;
	NVR_DEV_NET_CFG_S stNetCfg;

	__FUN_BEGIN("\n");
	nvr_logon_get_net_cfg(0, &stNetCfg);
	u32IpAddr = onvif_get_ipaddr(soap);
	snprintf(_IPAddr, sizeof(_IPAddr), 
			"%d.%d.%d.%d",
			(u32IpAddr >> 24) & 0xff,
			(u32IpAddr >> 16) & 0xff,
			(u32IpAddr >> 8) & 0xff,
			(u32IpAddr >> 0) & 0xff);
	ALLOC_STRUCT(tds__GetZeroConfigurationResponse->ZeroConfiguration, struct tt__NetworkZeroConfiguration);
	tds__GetZeroConfigurationResponse->ZeroConfiguration->InterfaceToken = ETH_NAME;
	ALLOC_TOKEN(tds__GetZeroConfigurationResponse->ZeroConfiguration->InterfaceToken, ETH_NAME);
	tds__GetZeroConfigurationResponse->ZeroConfiguration->Enabled = 1;
	tds__GetZeroConfigurationResponse->ZeroConfiguration->__sizeAddresses = 1;
	ALLOC_STRUCT(tds__GetZeroConfigurationResponse->ZeroConfiguration->Addresses, char*);
	ALLOC_TOKEN(tds__GetZeroConfigurationResponse->ZeroConfiguration->Addresses[0], _IPAddr);
	ALLOC_TOKEN(tds__GetZeroConfigurationResponse->ZeroConfiguration->Addresses[1], LINK_LOCAL_ADDR)
	tds__GetZeroConfigurationResponse->ZeroConfiguration->Extension = NULL;
	tds__GetZeroConfigurationResponse->ZeroConfiguration->__anyAttribute = NULL;
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tds__SetZeroConfiguration(struct soap* soap, struct _tds__SetZeroConfiguration *tds__SetZeroConfiguration, struct _tds__SetZeroConfigurationResponse *tds__SetZeroConfigurationResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:InvalidNetworkInterface"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tds__GetIPAddressFilter(struct soap* soap, struct _tds__GetIPAddressFilter *tds__GetIPAddressFilter, struct _tds__GetIPAddressFilterResponse *tds__GetIPAddressFilterResponse) 
{
	 onvif_fault(soap,"ter:ActionNotSupported", "ter:IPAddressFilterNotSupported"); 
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;  

}

 int  __tds__SetIPAddressFilter(struct soap* soap, struct _tds__SetIPAddressFilter *tds__SetIPAddressFilter, struct _tds__SetIPAddressFilterResponse *tds__SetIPAddressFilterResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:IPAddressFilterNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__AddIPAddressFilter(struct soap* soap, struct _tds__AddIPAddressFilter *tds__AddIPAddressFilter, struct _tds__AddIPAddressFilterResponse *tds__AddIPAddressFilterResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:IPAddressFilterNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__RemoveIPAddressFilter(struct soap* soap, struct _tds__RemoveIPAddressFilter *tds__RemoveIPAddressFilter, struct _tds__RemoveIPAddressFilterResponse *tds__RemoveIPAddressFilterResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:IPAddressFilterNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetAccessPolicy(struct soap* soap, struct _tds__GetAccessPolicy *tds__GetAccessPolicy, struct _tds__GetAccessPolicyResponse *tds__GetAccessPolicyResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AccessPolicyNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__SetAccessPolicy(struct soap* soap, struct _tds__SetAccessPolicy *tds__SetAccessPolicy, struct _tds__SetAccessPolicyResponse *tds__SetAccessPolicyResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AccessPolicyNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__CreateCertificate(struct soap* soap, struct _tds__CreateCertificate *tds__CreateCertificate, struct _tds__CreateCertificateResponse *tds__CreateCertificateResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:CertificateNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetCertificates(struct soap* soap, struct _tds__GetCertificates *tds__GetCertificates, struct _tds__GetCertificatesResponse *tds__GetCertificatesResponse) 
{
	tds__GetCertificatesResponse->__sizeNvtCertificate = 0;	
	tds__GetCertificatesResponse->NvtCertificate = NULL;
	//ALLOC_STRUCT(tds__GetCertificatesResponse->NvtCertificate, struct tt__Certificate);
	return SOAP_OK;	
}

 int  __tds__GetCertificatesStatus(struct soap* soap, struct _tds__GetCertificatesStatus *tds__GetCertificatesStatus, struct _tds__GetCertificatesStatusResponse *tds__GetCertificatesStatusResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:CertificateNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__SetCertificatesStatus(struct soap* soap, struct _tds__SetCertificatesStatus *tds__SetCertificatesStatus, struct _tds__SetCertificatesStatusResponse *tds__SetCertificatesStatusResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:CertificateNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__DeleteCertificates(struct soap* soap, struct _tds__DeleteCertificates *tds__DeleteCertificates, struct _tds__DeleteCertificatesResponse *tds__DeleteCertificatesResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:CertificateNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetPkcs10Request(struct soap* soap, struct _tds__GetPkcs10Request *tds__GetPkcs10Request, struct _tds__GetPkcs10RequestResponse *tds__GetPkcs10RequestResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:Pkcs10NotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__LoadCertificates(struct soap* soap, struct _tds__LoadCertificates *tds__LoadCertificates, struct _tds__LoadCertificatesResponse *tds__LoadCertificatesResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:CertificateNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetClientCertificateMode(struct soap* soap, struct _tds__GetClientCertificateMode *tds__GetClientCertificateMode, struct _tds__GetClientCertificateModeResponse *tds__GetClientCertificateModeResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:CertificateNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__SetClientCertificateMode(struct soap* soap, struct _tds__SetClientCertificateMode *tds__SetClientCertificateMode, struct _tds__SetClientCertificateModeResponse *tds__SetClientCertificateModeResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:CertificateNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetRelayOutputs(struct soap* soap, struct _tds__GetRelayOutputs *tds__GetRelayOutputs, struct _tds__GetRelayOutputsResponse *tds__GetRelayOutputsResponse) 
{
#if 0	
	struct tt__RelayOutput *RelayOutputs;
	NVR_DEV_DI_CFG_S stDiCfg;
	NVR_DEV_INFO_S stDevInfo;
	char token[ONVIF_TOKEN_LEN];
	int i;

	__FUN_BEGIN("\n");
	//nvr_logon_get_dev_info(0, &stDevInfo);
	ALLOC_STRUCT_NUM(RelayOutputs, struct tt__RelayOutput, NVR_MAX_SUPPORT_DI);

	for(i = 0; i < NVR_MAX_SUPPORT_DI && i < stDevInfo.stStableInfo.u8DiNum; i++)
	{		
		memset(token, 0, sizeof(token));
		sprintf(token, "%s_%d", ONVIF_RELAY_OUTPUT_TOKEN, i);
		ALLOC_TOKEN(RelayOutputs[i].token, token)
		ALLOC_STRUCT(RelayOutputs[i].Properties, struct tt__RelayOutputSettings);
		//hi_platform_get_di_cfg(i, &stDiCfg);
		RelayOutputs[i].Properties->Mode = tt__RelayMode__Monostable;
		ALLOC_TOKEN(RelayOutputs[i].Properties->DelayTime, "PT10S");	
		RelayOutputs[i].Properties->IdleState = (stDiCfg.u8DiType == 0) ? tt__RelayIdleState__open : tt__RelayIdleState__closed;
		RelayOutputs[i].__size = 0;
		RelayOutputs[i].__any = NULL;
		RelayOutputs[i].__anyAttribute = NULL;
	}
	
	tds__GetRelayOutputsResponse->__sizeRelayOutputs = stDevInfo.stStableInfo.u8DiNum;
	tds__GetRelayOutputsResponse->RelayOutputs = RelayOutputs;

	__FUN_END("\n");
#endif
	return SOAP_OK;	
}

 int  __tds__SetRelayOutputSettings(struct soap* soap, struct _tds__SetRelayOutputSettings *tds__SetRelayOutputSettings, struct _tds__SetRelayOutputSettingsResponse *tds__SetRelayOutputSettingsResponse) 
{
#if 0	
	int i;
	char token[64];
	//HI_DEV_DI_CFG_S stDiCfg;
	NVR_DEV_INFO_S stDevInfo;
	
	__FUN_BEGIN("\n");
	nvr_logon_get_dev_cfg(0, &stDevInfo);
	for(i = 0; i < NVR_MAX_SUPPORT_DI  && i < stDevInfo.stStableInfo.u8DiNum; i++)
	{
		sprintf(token, "%s_%d", ONVIF_RELAY_OUTPUT_TOKEN, i);
		if(strcmp(token, tds__SetRelayOutputSettings->RelayOutputToken) == 0)
			break;
	}
	if(i == stDevInfo.stStableInfo.u8DiNum)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:RelayToken");
		return SOAP_FAULT;
	}
	
	//hi_platform_get_di_cfg(i, &stDiCfg);
	//stDiCfg.u8DiType = (tds__SetRelayOutputSettings->Properties->IdleState == tt__RelayIdleState__open) ? 0 : 1; 
	//hi_platform_set_di_cfg(i, &stDiCfg);

	__FUN_END("\n");
#endif
	return SOAP_OK; 
}

 int  __tds__SetRelayOutputState(struct soap* soap, struct _tds__SetRelayOutputState *tds__SetRelayOutputState, struct _tds__SetRelayOutputStateResponse *tds__SetRelayOutputStateResponse) 
{
	int i;
	int active;
	char token[64];
	NVR_DEV_INFO_S stDevInfo;

	nvr_logon_get_dev_cfg(0, &stDevInfo);
	for(i = 0; i < NVR_MAX_SUPPORT_DI && i < stDevInfo.stStableInfo.u8AlarmInputNum; i++)
	{
		sprintf(token, "%s_%d", ONVIF_RELAY_OUTPUT_TOKEN, i);
		if(strcmp(token, tds__SetRelayOutputState->RelayOutputToken) == 0)
			break;
	}
	if(i == stDevInfo.stStableInfo.u8AlarmInputNum)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:RelayToken");
		return SOAP_FAULT;
	}
	if(tds__SetRelayOutputState->LogicalState == tt__RelayLogicalState__active)
	{
		active = 1;
	}
	else
	{
		active = 0;
	}
	(void)active;
	//nvr_logon_set_ipnc_alarm_cfg(int port,NVR_DEV_IPNC_ALARM_CFG_S * pCfg)
	//hi_platform_set_alarm(0, active);
	return SOAP_OK;
}

 int  __tds__SendAuxiliaryCommand(struct soap* soap, struct _tds__SendAuxiliaryCommand *tds__SendAuxiliaryCommand, struct _tds__SendAuxiliaryCommandResponse *tds__SendAuxiliaryCommandResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AuxiliaryDataNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetCACertificates(struct soap* soap, struct _tds__GetCACertificates *tds__GetCACertificates, struct _tds__GetCACertificatesResponse *tds__GetCACertificatesResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:CertificateNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tds__LoadCertificateWithPrivateKey(struct soap* soap, struct _tds__LoadCertificateWithPrivateKey *tds__LoadCertificateWithPrivateKey, struct _tds__LoadCertificateWithPrivateKeyResponse *tds__LoadCertificateWithPrivateKeyResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:CertificateNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tds__GetCertificateInformation(struct soap* soap, struct _tds__GetCertificateInformation *tds__GetCertificateInformation, struct _tds__GetCertificateInformationResponse *tds__GetCertificateInformationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:CertificateNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__LoadCACertificates(struct soap* soap, struct _tds__LoadCACertificates *tds__LoadCACertificates, struct _tds__LoadCACertificatesResponse *tds__LoadCACertificatesResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:CertificateNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__CreateDot1XConfiguration(struct soap* soap, struct _tds__CreateDot1XConfiguration *tds__CreateDot1XConfiguration, struct _tds__CreateDot1XConfigurationResponse *tds__CreateDot1XConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:EAPMethodNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__SetDot1XConfiguration(struct soap* soap, struct _tds__SetDot1XConfiguration *tds__SetDot1XConfiguration, struct _tds__SetDot1XConfigurationResponse *tds__SetDot1XConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:EAPMethodNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetDot1XConfiguration(struct soap* soap, struct _tds__GetDot1XConfiguration *tds__GetDot1XConfiguration, struct _tds__GetDot1XConfigurationResponse *tds__GetDot1XConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:EAPMethodNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetDot1XConfigurations(struct soap* soap, struct _tds__GetDot1XConfigurations *tds__GetDot1XConfigurations, struct _tds__GetDot1XConfigurationsResponse *tds__GetDot1XConfigurationsResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:EAPMethodNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__DeleteDot1XConfiguration(struct soap* soap, struct _tds__DeleteDot1XConfiguration *tds__DeleteDot1XConfiguration, struct _tds__DeleteDot1XConfigurationResponse *tds__DeleteDot1XConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:EAPMethodNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetDot11Capabilities(struct soap* soap, struct _tds__GetDot11Capabilities *tds__GetDot11Capabilities, struct _tds__GetDot11CapabilitiesResponse *tds__GetDot11CapabilitiesResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:InvalidDot11"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tds__GetDot11Status(struct soap* soap, struct _tds__GetDot11Status *tds__GetDot11Status, struct _tds__GetDot11StatusResponse *tds__GetDot11StatusResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:InvalidDot11"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__ScanAvailableDot11Networks(struct soap* soap, struct _tds__ScanAvailableDot11Networks *tds__ScanAvailableDot11Networks, struct _tds__ScanAvailableDot11NetworksResponse *tds__ScanAvailableDot11NetworksResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:InvalidDot11"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__GetSystemUris(struct soap* soap, struct _tds__GetSystemUris *tds__GetSystemUris, struct _tds__GetSystemUrisResponse *tds__GetSystemUrisResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:SystemUrisNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__StartFirmwareUpgrade(struct soap* soap, struct _tds__StartFirmwareUpgrade *tds__StartFirmwareUpgrade, struct _tds__StartFirmwareUpgradeResponse *tds__StartFirmwareUpgradeResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:FirmwareUpgradeNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __tds__StartSystemRestore(struct soap* soap, struct _tds__StartSystemRestore *tds__StartSystemRestore, struct _tds__StartSystemRestoreResponse *tds__StartSystemRestoreResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:SystemRestoreNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int  __timg__GetServiceCapabilities(struct soap* soap, struct _timg__GetServiceCapabilities *timg__GetServiceCapabilities, struct _timg__GetServiceCapabilitiesResponse *timg__GetServiceCapabilitiesResponse) 
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_OK;	
}

 int  __timg__GetImagingSettings(struct soap* soap, struct _timg__GetImagingSettings *timg__GetImagingSettings, struct _timg__GetImagingSettingsResponse *timg__GetImagingSettingsResponse) 
{
	__FUN_BEGIN("\r\n");
	struct tt__ImagingSettings20 *timg__ImagingSettings;
	IPNC_DEV_IMA_CFG_S stIma;
	IPNC_DEV_IRCFMODE_CFG_S stIrcf;
	float Brightness = 0;
	float Contrast = 0;
	float Saturation = 0;
	float Sharpness = 0;
	int chn  = 0;
	enum tt__IrCutFilterMode IrCutFilterMode;

	if(timg__GetImagingSettings->VideoSourceToken == NULL)
		return SOAP_FAULT;
	
	if(timg__GetImagingSettings)
	{		
		for (chn = 0; chn < ONVIF_VIDEO_CHANNAL_NUM; chn++)
		{
			if(strcmp(timg__GetImagingSettings->VideoSourceToken, gOnvifInfo.OnvifVideoSource[chn].token) == 0)
			{
				break;
			}
		}

		if (chn == ONVIF_VIDEO_CHANNAL_NUM) 
			return SOAP_FAULT;	
	}

	memset(&stIma, 0, sizeof(IPNC_DEV_IMA_CFG_S));
	memset(&stIrcf, 0, sizeof(IPNC_DEV_IRCFMODE_CFG_S));
	nvr_get_ipnc_param(chn, IPNC_CMD_IMA_CFG, &stIma, sizeof(IPNC_DEV_IMA_CFG_S));
	nvr_get_ipnc_param(chn, IPNC_CMD_IRCFMODE_CFG, &stIrcf, sizeof(IPNC_DEV_IRCFMODE_CFG_S));
	//printf("ret = %d, ret1 = %d\r\n", ret, ret1);
	if((timg__ImagingSettings = (struct tt__ImagingSettings20 *)soap_malloc(soap, sizeof(struct tt__ImagingSettings20))) == NULL)
	{
		__E("Failed to malloc for ImagingSettings.\n");
		return SOAP_FAULT;
	}	
	memset(timg__ImagingSettings, 0, sizeof(struct tt__ImagingSettings20));
	timg__ImagingSettings->BacklightCompensation = NULL;
	
	timg__ImagingSettings->Brightness = (float *)soap_malloc(soap, sizeof(float));
	if(timg__ImagingSettings->Brightness == NULL)
	{
		__E("Failed to malloc for Brightness.\n");
		return SOAP_FAULT;
	}	
	if((timg__ImagingSettings->Contrast = (float *)soap_malloc(soap, sizeof(float))) == NULL)
	{
		__E("Failed to malloc for Contrast.\n");
		return SOAP_FAULT;
	}
	
	if((timg__ImagingSettings->ColorSaturation = (float *)soap_malloc(soap, sizeof(float))) == NULL)
	{
		__E("Failed to malloc for ColorSaturation.\n");
		return SOAP_FAULT;
	}
	if((timg__ImagingSettings->Sharpness = (float *)soap_malloc(soap, sizeof(float))) == NULL)
	{
		__E("Failed to malloc for Sharpness.\n");
		return SOAP_FAULT;
	}
	
	if(stIma.eSuppMask & IPNC_VCT_IMA_BRIGHTNESS)
	{
		Brightness = (float)stIma.struBrightness.u8Value;
		*timg__ImagingSettings->Brightness = Brightness;
	}
	
	if(stIma.eSuppMask & IPNC_VCT_IMA_CONTRAST)
	{
		Contrast = (float)stIma.struContrast.u8Value;
		*timg__ImagingSettings->Contrast = Contrast;
	}
	if(stIma.eSuppMask & IPNC_VCT_IMA_SATURATION)
	{
		Saturation = (float)stIma.struSaturation.u8Value;
		*timg__ImagingSettings->ColorSaturation  = 	Saturation;
	}
	if(stIma.eSuppMask & IPNC_VCT_IMA_SHARPNESS)
	{
		Sharpness = (float)stIma.struSharpness.u8Value;
		*timg__ImagingSettings->Sharpness  = Sharpness;
	}
	
	timg__ImagingSettings->Exposure = NULL;
	timg__ImagingSettings->Focus = NULL;
	if(stIrcf.u8Mode == 0)
		IrCutFilterMode = tt__IrCutFilterMode__AUTO;
	if(stIrcf.u8Mode == 1)
		IrCutFilterMode = tt__IrCutFilterMode__OFF;
	if(stIrcf.u8Mode == 2)
		IrCutFilterMode = tt__IrCutFilterMode__ON;
	timg__ImagingSettings->IrCutFilter = (enum tt__IrCutFilterMode *)soap_malloc(soap, sizeof(enum tt__IrCutFilterMode));
	*timg__ImagingSettings->IrCutFilter = IrCutFilterMode;
	timg__ImagingSettings->WideDynamicRange = NULL;
	timg__ImagingSettings->WhiteBalance = NULL;
	timg__ImagingSettings->Extension = NULL;
	timg__ImagingSettings->__anyAttribute = NULL;

	timg__GetImagingSettingsResponse->ImagingSettings  = timg__ImagingSettings;

	__FUN_END("\r\n");
	return SOAP_OK;
}

 int  __timg__SetImagingSettings(struct soap* soap, struct _timg__SetImagingSettings *timg__SetImagingSettings, struct _timg__SetImagingSettingsResponse *timg__SetImagingSettingsResponse) 
{
	__FUN_BEGIN("\r\n");
	IPNC_DEV_IMA_CFG_S stIma;
	IPNC_DEV_IRCFMODE_CFG_S stIrcf;
	float Brightness = 0;
	float Contrast = 0;
	float Saturation = 0;
	float Sharpness = 0;
	int chn = 0;
	enum tt__IrCutFilterMode IrCutFilterMode;

	if(timg__SetImagingSettings->VideoSourceToken == NULL)
		return SOAP_FAULT;
	if(timg__SetImagingSettings)
	{
		for (chn = 0; chn < ONVIF_VIDEO_CHANNAL_NUM; chn++)
		{
			if(strcmp(timg__SetImagingSettings->VideoSourceToken, gOnvifInfo.OnvifVideoSource[chn].token) == 0)
			{
				break;
			}
		}

		if (chn == ONVIF_VIDEO_CHANNAL_NUM) 
			return SOAP_FAULT;	
	}
	memset(&stIma, 0, sizeof(IPNC_DEV_IMA_CFG_S));
	memset(&stIrcf, 0, sizeof(IPNC_DEV_IRCFMODE_CFG_S));
	
	nvr_get_ipnc_param(chn, IPNC_CMD_IMA_CFG, &stIma, sizeof(IPNC_DEV_IMA_CFG_S));
	nvr_get_ipnc_param(chn, IPNC_CMD_IRCFMODE_CFG, &stIrcf, sizeof(IPNC_DEV_IRCFMODE_CFG_S));

	if(timg__SetImagingSettings->ImagingSettings)
	{
		if(timg__SetImagingSettings->ImagingSettings->Brightness)
		{
			if(ONVIF_IMAGING_MAX < *timg__SetImagingSettings->ImagingSettings->Brightness || 
				*timg__SetImagingSettings->ImagingSettings->Brightness < ONVIF_IMAGING_MIN)
			{
				onvif_fault(soap,"ter:InvalidArgVal", "ter:SettingsInvalid"); 
				return SOAP_FAULT;	
			}
			Brightness = *timg__SetImagingSettings->ImagingSettings->Brightness;
			stIma.struBrightness.u8Value = (int)Brightness;
		}
		if(timg__SetImagingSettings->ImagingSettings->Contrast)
		{
			if(ONVIF_IMAGING_MAX < *timg__SetImagingSettings->ImagingSettings->Contrast || 
				*timg__SetImagingSettings->ImagingSettings->Contrast < ONVIF_IMAGING_MIN)
			{
				onvif_fault(soap,"ter:InvalidArgVal", "ter:SettingsInvalid"); 
				return SOAP_FAULT;	
			}
			Contrast = *timg__SetImagingSettings->ImagingSettings->Contrast;
			stIma.struContrast.u8Value = (int)Contrast;
		}
		if(timg__SetImagingSettings->ImagingSettings->ColorSaturation)
		{
			if(ONVIF_IMAGING_MAX < *timg__SetImagingSettings->ImagingSettings->ColorSaturation || 
				*timg__SetImagingSettings->ImagingSettings->ColorSaturation < ONVIF_IMAGING_MIN)
			{
				onvif_fault(soap,"ter:InvalidArgVal", "ter:SettingsInvalid"); 
				return SOAP_FAULT;	
			}
			Saturation = *timg__SetImagingSettings->ImagingSettings->ColorSaturation;
			stIma.struSaturation.u8Value = (int)Saturation;
		}
		if(timg__SetImagingSettings->ImagingSettings->Sharpness)
		{
			if(ONVIF_IMAGING_MAX < *timg__SetImagingSettings->ImagingSettings->Sharpness || 
				*timg__SetImagingSettings->ImagingSettings->Sharpness < ONVIF_IMAGING_MIN)
			{
				onvif_fault(soap,"ter:InvalidArgVal", "ter:SettingsInvalid"); 
				return SOAP_FAULT;	
			}
			Sharpness = *timg__SetImagingSettings->ImagingSettings->Sharpness;
			stIma.struSharpness.u8Value = (int)Sharpness;
		}

		if(timg__SetImagingSettings->ImagingSettings->IrCutFilter)
		{
			IrCutFilterMode = *timg__SetImagingSettings->ImagingSettings->IrCutFilter;
			if(IrCutFilterMode == tt__IrCutFilterMode__AUTO)
				stIrcf.u8Mode = 0;
			else if(IrCutFilterMode == tt__IrCutFilterMode__OFF)
				stIrcf.u8Mode = 1;
			else if(IrCutFilterMode == tt__IrCutFilterMode__ON)
				stIrcf.u8Mode = 2;
			else
			{
				onvif_fault(soap,"ter:InvalidArgVal", "ter:SettingsInvalid"); 
				return SOAP_FAULT;	
			}
		}

		nvr_set_ipnc_param(chn, IPNC_CMD_IMA_CFG, &stIma, sizeof(IPNC_DEV_IMA_CFG_S));
		nvr_set_ipnc_param(chn, IPNC_CMD_IRCFMODE_CFG, &stIrcf, sizeof(IPNC_DEV_IRCFMODE_CFG_S));
	}
	return SOAP_OK;
	
	onvif_fault(soap, "ter:ActionNotSupported", "ter:NoImagingForSource");
	return SOAP_FAULT;
	__FUN_END("\r\n");
}

 int  __timg__GetOptions(struct soap* soap, struct _timg__GetOptions *timg__GetOptions, struct _timg__GetOptionsResponse *timg__GetOptionsResponse) 
{
	struct tt__ImagingOptions20 *ImagingOptions;
	IPNC_DEV_IRCFMODE_CFG_S stIrcf;
	float Max = ONVIF_IMAGING_MAX;
	float Min = ONVIF_IMAGING_MIN;
	int chn = 0;
	enum tt__IrCutFilterMode IrCutFilterMode;

	__FUN_BEGIN("\r\n");
	if(timg__GetOptions->VideoSourceToken == NULL)
		return SOAP_FAULT;	
	if(timg__GetOptions)
	{
		for (chn = 0; chn < ONVIF_VIDEO_CHANNAL_NUM; chn++)
		{
			if(strcmp(timg__GetOptions->VideoSourceToken, gOnvifInfo.OnvifVideoSource[chn].token) == 0)
			{
				break;
			}
		}

		if (chn == ONVIF_VIDEO_CHANNAL_NUM) 
			return SOAP_FAULT;	
	}

	memset(&stIrcf, 0, sizeof(IPNC_DEV_IRCFMODE_CFG_S));
	nvr_get_ipnc_param(chn, IPNC_CMD_IRCFMODE_CFG, &stIrcf, sizeof(IPNC_DEV_IRCFMODE_CFG_S));	
	if((ImagingOptions = (struct tt__ImagingOptions20 *)soap_malloc(soap, sizeof(struct tt__ImagingOptions20))) == NULL)
	{
		__E("Failed to malloc for tt__ImagingOptions20.\n");
		return SOAP_FAULT;
	}
	ImagingOptions->BacklightCompensation = NULL;
	
	ImagingOptions->Brightness = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(ImagingOptions->Brightness == NULL)
	{
		__E("Failed to malloc for Brightness.\n");
		return SOAP_FAULT;
	}	
	if((ImagingOptions->Contrast = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange))) == NULL)
	{
		__E("Failed to malloc for Contrast.\n");
		return SOAP_FAULT;
	}
	
	if((ImagingOptions->ColorSaturation = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange))) == NULL)
	{
		__E("Failed to malloc for ColorSaturation.\n");
		return SOAP_FAULT;
	}
	if((ImagingOptions->Sharpness = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange))) == NULL)
	{
		__E("Failed to malloc for Sharpness.\n");
		return SOAP_FAULT;
	}
	ImagingOptions->Brightness->Max = Max;
	ImagingOptions->Brightness->Min = Min;
	ImagingOptions->Contrast->Max = Max;
	ImagingOptions->Contrast->Min = Min;
	ImagingOptions->ColorSaturation->Max = Max;
	ImagingOptions->ColorSaturation->Min = Min;
	ImagingOptions->Sharpness->Max = Max;
	ImagingOptions->Sharpness->Min = Min;

	ImagingOptions->Exposure = NULL;
	ImagingOptions->Focus = NULL;
	if(stIrcf.u8Mode == 0)
		IrCutFilterMode = tt__IrCutFilterMode__AUTO;
	if(stIrcf.u8Mode == 1)
		IrCutFilterMode = tt__IrCutFilterMode__OFF;
	if(stIrcf.u8Mode == 2)
		IrCutFilterMode = tt__IrCutFilterMode__ON;
	ImagingOptions->IrCutFilterModes = (enum tt__IrCutFilterMode *)soap_malloc(soap, sizeof(enum tt__IrCutFilterMode));
	ImagingOptions->__sizeIrCutFilterModes =  1;
	*ImagingOptions->IrCutFilterModes = IrCutFilterMode;
	ImagingOptions->WideDynamicRange = NULL;
	ImagingOptions->WhiteBalance = NULL;
	ImagingOptions->Extension = NULL;
	ImagingOptions->__anyAttribute = NULL;

	timg__GetOptionsResponse->ImagingOptions  = ImagingOptions;
	
	__FUN_END("\r\n");
	return SOAP_OK;

	
}

 int  __timg__Move(struct soap* soap, struct _timg__Move *timg__Move, struct _timg__MoveResponse *timg__MoveResponse) 
{	
	__FUN_BEGIN("\n");
	if(timg__Move->VideoSourceToken == NULL)
		return SOAP_FAULT;	
/*	
	if(timg__Move)
	{
		if(strcmp(timg__Move->VideoSourceToken, ONVIF_MEDIA_VIDEO_MAIN_SOURCE_TOKEN) != 0)
		{
			onvif_fault(soap,"ter:InvalidArgVal", "ter:NoSource "); 
			return SOAP_FAULT;	
		}
		if(timg__Move->Focus->Continuous)
		{	
			if(timg__Move->Focus->Continuous->Speed > 0)
				Nvr_Client_Crtl_Ptz(PTZ_CMD_ZOOM_WIDE, (uint8_t)chn, 0, 0) 
				//hi_platform_control_ptz(PTZ_CMD_FOCUS_NEAR, 0, 0, 0);	
			if(timg__Move->Focus->Continuous->Speed < 0)
				Nvr_Client_Crtl_Ptz(PTZ_CMD_ZOOM_TELE, (uint8_t)chn, 0, 0) 
				//hi_platform_control_ptz(PTZ_CMD_FOCUS_FAR, 0, 0, 0);	
			
		}
	}
*/	
	__FUN_END("\r\n");
	return SOAP_OK;
		
}

 int  __timg__Stop(struct soap* soap, struct _timg__Stop *timg__Stop, struct _timg__StopResponse *timg__StopResponse) 
{
	__FUN_BEGIN("\n");
	if(timg__Stop->VideoSourceToken == NULL)
		return SOAP_FAULT;	
	if(timg__Stop)
	{
		if(strcmp(timg__Stop->VideoSourceToken, ONVIF_MEDIA_VIDEO_MAIN_SOURCE_TOKEN) != 0)
		{
			onvif_fault(soap,"ter:InvalidArgVal", "ter:NoSource "); 
			return SOAP_FAULT;	
		}
		//hi_platform_control_ptz(PTZ_CMD_STOP, 0, 0, 0);	
	}
	return SOAP_OK;
	__FUN_END("\n");
}

 int  __timg__GetStatus(struct soap* soap, struct _timg__GetStatus *timg__GetStatus, struct _timg__GetStatusResponse *timg__GetStatusResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:NoImagingForSource"); 
	return SOAP_FAULT;			
}

 int  __timg__GetMoveOptions(struct soap* soap, struct _timg__GetMoveOptions *timg__GetMoveOptions, struct _timg__GetMoveOptionsResponse *timg__GetMoveOptionsResponse) 
{

	struct tt__MoveOptions20 * MoveOptions;
	int chn = 0;
	float a = ONVIF_IMAGING_MIN;
	float b = ONVIF_IMAGING_MAX;
	__FUN_BEGIN("\n");

	if(timg__GetMoveOptions->VideoSourceToken== NULL)
		return SOAP_FAULT;	
	if(timg__GetMoveOptions)
	{
		for (chn = 0; chn < ONVIF_VIDEO_CHANNAL_NUM; chn++)
		{
			if(strcmp(timg__GetMoveOptions->VideoSourceToken, gOnvifInfo.OnvifVideoSource[chn].token) == 0)
			{
				break;
			}
		}

		if (chn == ONVIF_VIDEO_CHANNAL_NUM) 
			return SOAP_FAULT;	
	}

	if((MoveOptions = (struct tt__MoveOptions20 *)soap_malloc(soap, sizeof(struct tt__MoveOptions20))) == NULL)
	{
		__E("Failed to malloc for MoveOptions.\n");
		return SOAP_FAULT;
	}
	if((MoveOptions->Relative = (struct tt__RelativeFocusOptions20 *)soap_malloc(soap, sizeof(struct tt__RelativeFocusOptions20))) == NULL)
	{
		__E("Failed to malloc for Relative.\n");
		return SOAP_FAULT;
	}
	if((MoveOptions->Relative->Distance = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange))) == NULL)
	{
		__E("Failed to malloc for Distance.\n");
		return SOAP_FAULT;
	}
	MoveOptions->Relative->Distance->Max = b;
	MoveOptions->Relative->Distance->Min = a;
	MoveOptions->Absolute = NULL;
	MoveOptions->Continuous= NULL;
	MoveOptions->Relative->Speed = NULL;

	timg__GetMoveOptionsResponse->MoveOptions = MoveOptions;

	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tptz__GetServiceCapabilities(struct soap* soap, struct _tptz__GetServiceCapabilities *tptz__GetServiceCapabilities, struct _tptz__GetServiceCapabilitiesResponse *tptz__GetServiceCapabilitiesResponse) 
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tptz__GetConfigurations(struct soap* soap, struct _tptz__GetConfigurations *tptz__GetConfigurations, struct _tptz__GetConfigurationsResponse *tptz__GetConfigurationsResponse) 
{
	__FUN_BEGIN("\n");
	tptz__GetConfigurationsResponse->__sizePTZConfiguration = 1;
	tptz__GetConfigurationsResponse->PTZConfiguration = (struct tt__PTZConfiguration*)soap_malloc(soap, sizeof(struct tt__PTZConfiguration));
 	if(tptz__GetConfigurationsResponse->PTZConfiguration == NULL)
 	{
		__E("Failed to malloc for PTZConfiguration.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationsResponse->PTZConfiguration->Name = ONVIF_PTZ_NAME;
	tptz__GetConfigurationsResponse->PTZConfiguration->UseCount = 1;
	tptz__GetConfigurationsResponse->PTZConfiguration->token = ONVIF_PTZ_TOKEN;
	tptz__GetConfigurationsResponse->PTZConfiguration->NodeToken = ONVIF_PTZ_TOKEN;
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultAbsolutePantTiltPositionSpace = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace";
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultAbsoluteZoomPositionSpace = "http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace";
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultRelativePanTiltTranslationSpace = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace";
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultRelativeZoomTranslationSpace = "http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace ";
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultContinuousPanTiltVelocitySpace = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace ";
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultContinuousZoomVelocitySpace = "http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace ";
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultPTZSpeed = (struct tt__PTZSpeed*)soap_malloc(soap, sizeof(struct tt__PTZSpeed));
	if(tptz__GetConfigurationsResponse->PTZConfiguration->DefaultPTZSpeed == NULL)
	{
		__E("Failed to malloc for DefaultPTZSpeed.\n");
		return SOAP_FAULT;
	}
	
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultPTZSpeed->PanTilt = (struct tt__Vector2D *)soap_malloc(soap, sizeof(struct tt__Vector2D));
	if(tptz__GetConfigurationsResponse->PTZConfiguration->DefaultPTZSpeed->PanTilt == NULL)
	{
		__E("Failed to malloc for PanTilt.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultPTZSpeed->PanTilt->space = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace ";
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultPTZSpeed->PanTilt->x = 1.0;
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultPTZSpeed->PanTilt->y =0.5;
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultPTZSpeed->Zoom = (struct tt__Vector1D *)soap_malloc(soap, sizeof(struct tt__Vector1D));
	if(tptz__GetConfigurationsResponse->PTZConfiguration->DefaultPTZSpeed->Zoom == NULL)
	{
		__E("Failed to malloc for Zoom.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultPTZSpeed->Zoom->space = "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace ";
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultPTZSpeed->Zoom->x = 0.0;

	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultPTZTimeout = soap_malloc(soap, sizeof(char)*4);
	if(tptz__GetConfigurationsResponse->PTZConfiguration->DefaultPTZTimeout == NULL)
	{
		__E("Failed to malloc for ptztimeout.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationsResponse->PTZConfiguration->DefaultPTZTimeout = "PT5S";
	
	//#####################################################
	tptz__GetConfigurationsResponse->PTZConfiguration->PanTiltLimits = (struct tt__PanTiltLimits*)soap_malloc(soap, sizeof(struct tt__PanTiltLimits));
	if(tptz__GetConfigurationsResponse->PTZConfiguration->PanTiltLimits == NULL)
	{
		__E("Failed to malloc for PanTiltLimits.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationsResponse->PTZConfiguration->PanTiltLimits->Range = (struct tt__Space2DDescription*)soap_malloc(soap, sizeof(struct tt__Space2DDescription));
	if(tptz__GetConfigurationsResponse->PTZConfiguration->PanTiltLimits->Range == NULL)
	{
		__E("Failed to malloc for Range.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationsResponse->PTZConfiguration->PanTiltLimits->Range->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace";
	tptz__GetConfigurationsResponse->PTZConfiguration->PanTiltLimits->Range->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationsResponse->PTZConfiguration->PanTiltLimits->Range->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationsResponse->PTZConfiguration->PanTiltLimits->Range->XRange->Max= 1.0;
	tptz__GetConfigurationsResponse->PTZConfiguration->PanTiltLimits->Range->XRange->Min= - 1.0;

	tptz__GetConfigurationsResponse->PTZConfiguration->PanTiltLimits->Range->YRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationsResponse->PTZConfiguration->PanTiltLimits->Range->YRange == NULL)
	{
		__E("Failed to malloc for YRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationsResponse->PTZConfiguration->PanTiltLimits->Range->YRange->Max= 1.0;
	tptz__GetConfigurationsResponse->PTZConfiguration->PanTiltLimits->Range->YRange->Min= - 1.0;	

	//############################################################
	tptz__GetConfigurationsResponse->PTZConfiguration->ZoomLimits = (struct tt__ZoomLimits*)soap_malloc(soap, sizeof(struct tt__ZoomLimits));
	if(tptz__GetConfigurationsResponse->PTZConfiguration->ZoomLimits == NULL)
	{
		__E("Failed to malloc for ZoomLimits.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationsResponse->PTZConfiguration->ZoomLimits->Range = (struct tt__Space1DDescription*)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
	if(tptz__GetConfigurationsResponse->PTZConfiguration->ZoomLimits->Range == NULL)
	{
		__E("Failed to malloc for Range.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationsResponse->PTZConfiguration->ZoomLimits->Range->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace";
	tptz__GetConfigurationsResponse->PTZConfiguration->ZoomLimits->Range->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationsResponse->PTZConfiguration->ZoomLimits->Range->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationsResponse->PTZConfiguration->ZoomLimits->Range->XRange->Max= 1.0;
	tptz__GetConfigurationsResponse->PTZConfiguration->ZoomLimits->Range->XRange->Min= 0.0;
	tptz__GetConfigurationsResponse->PTZConfiguration->Extension = NULL;
	tptz__GetConfigurationsResponse->PTZConfiguration->__anyAttribute = NULL;
	
	__FUN_END("\n");
	return SOAP_OK;
	
}

 int  __tptz__GetPresets(struct soap* soap, struct _tptz__GetPresets *tptz__GetPresets, struct _tptz__GetPresetsResponse *tptz__GetPresetsResponse) 
{
	int i, presetNo = 0;
	struct tt__PTZPreset *Preset;
	HI_ONVIF_PTZ_CFG_S stPtzCfg;
	
	__FUN_BEGIN("\n");
	int index = 0, chn = 0;
	for (index = 0; index < ONVIF_PROFILE_NUM; index++)
		if (0 == strcmp(tptz__GetPresets->ProfileToken, gOnvifInfo.OnvifProfile[index].token))
			break;
	
	if (index == ONVIF_PROFILE_NUM)
			return SOAP_FAULT;
	
	chn = (index>>1);
	
	onvif_get_ptz_cfg(chn, &stPtzCfg);
	Preset = (struct tt__PTZPreset *)soap_malloc(soap, sizeof(struct tt__PTZPreset) * ONVIF_PTZ_PRESET_NUM);
	if(Preset == NULL)
	{
		__E("Faild to malloc for preset.\n");
		return SOAP_FAULT;
	}
	
	for(i = 1; i<ONVIF_PTZ_PRESET_NUM; i++)
	{		
		if(strlen(stPtzCfg.presetCfg[i].Name) == 0)
			continue;
		Preset[presetNo].Name = soap_malloc(soap, sizeof(char) * ONVIF_NAME_LEN);
		Preset[presetNo].token = soap_malloc(soap, sizeof(char) * ONVIF_TOKEN_LEN);
		if(Preset[presetNo].Name == NULL || Preset[presetNo].token == NULL)
		{
			__E("Failed to malloc for name and token.\n");
			return SOAP_FAULT;
		}
		strcpy(Preset[presetNo].Name, stPtzCfg.presetCfg[i].Name);
		strcpy(Preset[presetNo].token, stPtzCfg.presetCfg[i].token);
		Preset[presetNo].PTZPosition = NULL;
		presetNo++;
	}
	__D("presetNO : %d\n", presetNo);
	tptz__GetPresetsResponse->__sizePreset = presetNo;
	tptz__GetPresetsResponse->Preset = Preset;
	__FUN_END("\n");
	return SOAP_OK;	
}

 int  __tptz__SetPreset(struct soap* soap, struct _tptz__SetPreset *tptz__SetPreset, struct _tptz__SetPresetResponse *tptz__SetPresetResponse) 
{
	int i;
	int speed = 1;
	HI_ONVIF_PTZ_CFG_S stPtzCfg;
	__FUN_BEGIN("\n");

	int index = 0, chn = 0;
	for (index = 0; index < ONVIF_PROFILE_NUM; index++)
		if (0 == strcmp(tptz__SetPreset->ProfileToken, gOnvifInfo.OnvifProfile[index].token))
			break;
	
	if (index == ONVIF_PROFILE_NUM)
			return SOAP_FAULT;
	
	chn = (index>>1);
			
	onvif_get_ptz_cfg(chn, &stPtzCfg);
	
	tptz__SetPresetResponse->PresetToken = soap_malloc(soap, sizeof(char) * ONVIF_TOKEN_LEN);
	if(tptz__SetPresetResponse->PresetToken == NULL)
	{
		__E("Failed to malloc for preset token.\n");
		return SOAP_FAULT;
	}
	
	if(tptz__SetPreset->PresetName != NULL)
	{
		for (i = 1; i < ONVIF_PTZ_PRESET_NUM; i++)
		{
			if (stPtzCfg.presetCfg[i].Name[0] != '\0') 
				continue;
			strcpy(stPtzCfg.presetCfg[i].Name ,tptz__SetPreset->PresetName);
			strcpy(tptz__SetPresetResponse->PresetToken, stPtzCfg.presetCfg[i].token);
			printf("setpreset name %s index %d\r\n", tptz__SetPreset->PresetName, i);
			Nvr_Client_Crtl_Ptz(PTZ_CMD_SET_PRESET, chn, speed, i+1);
			break;
		}
	}
	onvif_set_ptz_cfg(chn, &stPtzCfg);
	
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tptz__RemovePreset(struct soap* soap, struct _tptz__RemovePreset *tptz__RemovePreset, struct _tptz__RemovePresetResponse *tptz__RemovePresetResponse) 
{
	int i;
	HI_ONVIF_PTZ_CFG_S stPtzCfg;
	__FUN_BEGIN("\n");

	int index = 0, chn = 0;
	for (index = 0; index < ONVIF_PROFILE_NUM; index++)
		if (0 == strcmp(tptz__RemovePreset->ProfileToken, gOnvifInfo.OnvifProfile[index].token))
			break;
	
	if (index == ONVIF_PROFILE_NUM)
		return SOAP_FAULT;
	
	chn = (index>>1);
	
	onvif_get_ptz_cfg(chn, &stPtzCfg);
	for(i = 0; i<ONVIF_PTZ_PRESET_NUM; i++)
	{
		if(strcmp(stPtzCfg.presetCfg[i].token, tptz__RemovePreset->PresetToken) == 0)
			break;
	}
	if(i == ONVIF_PTZ_PRESET_NUM)
	{
		__E("The preset no is error!\n");
		return SOAP_FAULT;
	}	
	printf("remove preset name %s index %d\r\n", stPtzCfg.presetCfg[i].Name, i);
	stPtzCfg.presetCfg[i].Name[0] = '\0';
	onvif_set_ptz_cfg(chn, &stPtzCfg);
	Nvr_Client_Crtl_Ptz(PTZ_CMD_CLR_PRESET, chn, 0, i+1);
	
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tptz__GotoPreset(struct soap* soap, struct _tptz__GotoPreset *tptz__GotoPreset, struct _tptz__GotoPresetResponse *tptz__GotoPresetResponse) 
{
	int i;
	HI_ONVIF_PTZ_CFG_S stPtzCfg;
	int speed = 1;
	__FUN_BEGIN("\n");
	
	int index = 0, chn = 0;
	for (index = 0; index < ONVIF_PROFILE_NUM; index++)
		if (0 == strcmp(tptz__GotoPreset->ProfileToken, gOnvifInfo.OnvifProfile[index].token))
			break;
	
	if (index == ONVIF_PROFILE_NUM)
		return SOAP_FAULT;
	
	chn = (index>>1);
	
	if(tptz__GotoPreset->PresetToken == NULL)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoToken");
		return SOAP_FAULT;
	}

	onvif_get_ptz_cfg(chn, &stPtzCfg);
	for(i = 0; i<ONVIF_PTZ_PRESET_NUM; i++)
	{
		if(strcmp(stPtzCfg.presetCfg[i].token, tptz__GotoPreset->PresetToken) == 0)
			break;
	}
	if(i == ONVIF_PTZ_PRESET_NUM)
	{
		__E("The preset no is error!\n");
		return SOAP_FAULT;
	}
	
	printf("goto preset name %s index %d\r\n", stPtzCfg.presetCfg[i].Name, i);
	Nvr_Client_Crtl_Ptz(PTZ_CMD_GOTO_PRESET, chn, speed, i+1);
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tptz__GetStatus(struct soap* soap, struct _tptz__GetStatus *tptz__GetStatus, struct _tptz__GetStatusResponse *tptz__GetStatusResponse) 
{
	__FUN_BEGIN("\n");	
		#if 1
		tptz__GetStatusResponse->PTZStatus = (struct tt__PTZStatus*)soap_malloc(soap, sizeof(struct tt__PTZStatus));
		if(tptz__GetStatusResponse->PTZStatus == NULL)
		{
			__E("Failed to malloc for PTZStatus.\n");
			return SOAP_FAULT;
		}

		tptz__GetStatusResponse->PTZStatus->Position = (struct tt__PTZVector*)soap_malloc(soap, sizeof(struct tt__PTZVector));
		if(tptz__GetStatusResponse->PTZStatus->Position == NULL)
		{
			__E("Failed to malloc for Position.\n");
			return SOAP_FAULT;
		}
		tptz__GetStatusResponse->PTZStatus->Position->PanTilt = (struct tt__Vector2D*)soap_malloc(soap, sizeof(struct tt__Vector2D));
		if(tptz__GetStatusResponse->PTZStatus->Position->PanTilt == NULL)
		{
			__E("Failed to malloc for PanTilt.\n");
			return SOAP_FAULT;
		}
		tptz__GetStatusResponse->PTZStatus->Position->PanTilt->space = (char *)soap_malloc(soap, sizeof(char) *64);
		tptz__GetStatusResponse->PTZStatus->Position->PanTilt->space = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace ";
		tptz__GetStatusResponse->PTZStatus->Position->PanTilt->x = 1.0;
		tptz__GetStatusResponse->PTZStatus->Position->PanTilt->y = 0.5;
	
		tptz__GetStatusResponse->PTZStatus->Position->Zoom = (struct tt__Vector1D*)soap_malloc(soap, sizeof(struct tt__Vector1D));
		if(tptz__GetStatusResponse->PTZStatus->Position->Zoom == NULL)
		{
			__E("Failed to malloc for Zoom.\n");
			return SOAP_FAULT;
		}
		tptz__GetStatusResponse->PTZStatus->Position->Zoom->space = (char *)soap_malloc(soap, sizeof(char) *64);
		tptz__GetStatusResponse->PTZStatus->Position->Zoom->space = "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace";
		tptz__GetStatusResponse->PTZStatus->Position->Zoom->x = 0.5;

		tptz__GetStatusResponse->PTZStatus->MoveStatus = (struct tt__PTZMoveStatus*)soap_malloc(soap, sizeof(struct tt__PTZMoveStatus));
		if(tptz__GetStatusResponse->PTZStatus->MoveStatus == NULL)
		{
			__E("Failed to malloc for MoveStatus.\n");
			return SOAP_FAULT;
		}
		
		tptz__GetStatusResponse->PTZStatus->MoveStatus->PanTilt = tt__MoveStatus__IDLE;
		tptz__GetStatusResponse->PTZStatus->MoveStatus->Zoom = tt__MoveStatus__IDLE;
		tptz__GetStatusResponse->PTZStatus->Error = NULL;
		tptz__GetStatusResponse->PTZStatus->__size = 1;
		tptz__GetStatusResponse->PTZStatus->__any = NULL;
		tptz__GetStatusResponse->PTZStatus->__anyAttribute = NULL;
		
		#endif
		
		__FUN_END("\n");
		return SOAP_OK;
}

 int  __tptz__GetConfiguration(struct soap* soap, struct _tptz__GetConfiguration *tptz__GetConfiguration, struct _tptz__GetConfigurationResponse *tptz__GetConfigurationResponse) 
{
	__FUN_BEGIN("\n");
	tptz__GetConfigurationResponse->PTZConfiguration = (struct tt__PTZConfiguration*)soap_malloc(soap, sizeof(struct tt__PTZConfiguration));
 	if(tptz__GetConfigurationResponse->PTZConfiguration == NULL)
 	{
		__E("Failed to malloc for PTZConfiguration.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationResponse->PTZConfiguration->Name = ONVIF_PTZ_NAME;
	tptz__GetConfigurationResponse->PTZConfiguration->UseCount = 1;
	tptz__GetConfigurationResponse->PTZConfiguration->token = ONVIF_PTZ_TOKEN;
	tptz__GetConfigurationResponse->PTZConfiguration->NodeToken = ONVIF_PTZ_TOKEN;
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultAbsolutePantTiltPositionSpace = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace";
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultAbsoluteZoomPositionSpace = "http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace";
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultRelativePanTiltTranslationSpace = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace";
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultRelativeZoomTranslationSpace = "http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace ";
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultContinuousPanTiltVelocitySpace = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace ";
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultContinuousZoomVelocitySpace = "http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace ";
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZSpeed = (struct tt__PTZSpeed*)soap_malloc(soap, sizeof(struct tt__PTZSpeed));
	if(tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZSpeed == NULL)
	{
		__E("Failed to malloc for DefaultPTZSpeed.\n");
		return SOAP_FAULT;
	}
	
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZSpeed->PanTilt = (struct tt__Vector2D *)soap_malloc(soap, sizeof(struct tt__Vector2D));
	if(tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZSpeed->PanTilt == NULL)
	{
		__E("Failed to malloc for PanTilt.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZSpeed->PanTilt->space = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace ";
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZSpeed->PanTilt->x = 1.0;
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZSpeed->PanTilt->y = 0.5;
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZSpeed->Zoom = (struct tt__Vector1D *)soap_malloc(soap, sizeof(struct tt__Vector1D));
	if(tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZSpeed->Zoom == NULL)
	{
		__E("Failed to malloc for Zoom.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZSpeed->Zoom->space = "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace ";
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZSpeed->Zoom->x = 0.0;

	tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZTimeout = soap_malloc(soap, sizeof(char) * 4);
	if(tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZTimeout == NULL)
	{
		__E("Failed to malloc for ptztimeout.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZTimeout = "PT30S";
	
	//#####################################################
	tptz__GetConfigurationResponse->PTZConfiguration->PanTiltLimits = (struct tt__PanTiltLimits*)soap_malloc(soap, sizeof(struct tt__PanTiltLimits));
	if(tptz__GetConfigurationResponse->PTZConfiguration->PanTiltLimits == NULL)
	{
		__E("Failed to malloc for PanTiltLimits.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationResponse->PTZConfiguration->PanTiltLimits->Range = (struct tt__Space2DDescription*)soap_malloc(soap, sizeof(struct tt__Space2DDescription));
	if(tptz__GetConfigurationResponse->PTZConfiguration->PanTiltLimits->Range == NULL)
	{
		__E("Failed to malloc for Range.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationResponse->PTZConfiguration->PanTiltLimits->Range->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace";
	tptz__GetConfigurationResponse->PTZConfiguration->PanTiltLimits->Range->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationResponse->PTZConfiguration->PanTiltLimits->Range->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationResponse->PTZConfiguration->PanTiltLimits->Range->XRange->Max= 1.0;
	tptz__GetConfigurationResponse->PTZConfiguration->PanTiltLimits->Range->XRange->Min= -1.0;

	tptz__GetConfigurationResponse->PTZConfiguration->PanTiltLimits->Range->YRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationResponse->PTZConfiguration->PanTiltLimits->Range->YRange == NULL)
	{
		__E("Failed to malloc for YRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationResponse->PTZConfiguration->PanTiltLimits->Range->YRange->Max= 1.0;
	tptz__GetConfigurationResponse->PTZConfiguration->PanTiltLimits->Range->YRange->Min= - 1.0;	

	//############################################################
	tptz__GetConfigurationResponse->PTZConfiguration->ZoomLimits = (struct tt__ZoomLimits*)soap_malloc(soap, sizeof(struct tt__ZoomLimits));
	if(tptz__GetConfigurationResponse->PTZConfiguration->ZoomLimits == NULL)
	{
		__E("Failed to malloc for ZoomLimits.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationResponse->PTZConfiguration->ZoomLimits->Range = (struct tt__Space1DDescription*)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
	if(tptz__GetConfigurationResponse->PTZConfiguration->ZoomLimits->Range == NULL)
	{
		__E("Failed to malloc for Range.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationResponse->PTZConfiguration->ZoomLimits->Range->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace";
	tptz__GetConfigurationResponse->PTZConfiguration->ZoomLimits->Range->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationResponse->PTZConfiguration->ZoomLimits->Range->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationResponse->PTZConfiguration->ZoomLimits->Range->XRange->Max= 1.0;
	tptz__GetConfigurationResponse->PTZConfiguration->ZoomLimits->Range->XRange->Min= 0.0;
	tptz__GetConfigurationResponse->PTZConfiguration->Extension = NULL;
	tptz__GetConfigurationResponse->PTZConfiguration->__anyAttribute = NULL;
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tptz__GetNodes(struct soap* soap, struct _tptz__GetNodes *tptz__GetNodes, struct _tptz__GetNodesResponse *tptz__GetNodesResponse) 
{
	__FUN_BEGIN("\n");
	tptz__GetNodesResponse->__sizePTZNode = 1;
	tptz__GetNodesResponse->PTZNode = (struct tt__PTZNode *)soap_malloc( soap, sizeof(struct tt__PTZNode));
	if(tptz__GetNodesResponse->PTZNode == NULL)
	{
		__E("Failed to malloc for PTZNode.\n");
		return SOAP_FAULT;
	}
	
	tptz__GetNodesResponse->PTZNode->token = ONVIF_PTZ_TOKEN;
	tptz__GetNodesResponse->PTZNode->Name = ONVIF_PTZ_NAME;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces = NULL;
	
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces = (struct tt__PTZSpaces *)soap_malloc(soap, sizeof(struct tt__PTZSpaces));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces == NULL)
	{
		__E("Failed to malloc for SupportedPTZSpaces.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->__sizeAbsolutePanTiltPositionSpace = 1;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->__sizeRelativePanTiltTranslationSpace = 1;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->__sizeContinuousPanTiltVelocitySpace = 1;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->__sizeAbsoluteZoomPositionSpace = 1;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->__sizeContinuousZoomVelocitySpace = 1;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->__sizePanTiltSpeedSpace = 1; 
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->__sizeRelativeZoomTranslationSpace =1;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->__sizeZoomSpeedSpace =1;

	//#############################################################################################
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace = (struct tt__Space2DDescription *)soap_malloc(soap,  sizeof(struct tt__Space2DDescription));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace == NULL)
	{
		__E("Failed to malloc for AbsolutePanTiltPositionSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace";

	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange->Max = 1.0;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange->Min = -1.0;

	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange == NULL)
	{
		__E("Failed to malloc for YRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange->Max = 1.0;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange->Min = -1.0;

	/////////////////////////////////////////////////////////////////////////////
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace == NULL)
	{
		__E("Failed to malloc for AbsoluteZoomPositionSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace";
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange->Max = 1.0;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange->Min = 0.0;



	//#############################################################################################
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace = (struct tt__Space2DDescription *)soap_malloc(soap,  sizeof(struct tt__Space2DDescription));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace == NULL)
	{
		__E("Failed to malloc for RelativePanTiltTranslationSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace";

	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange->Max = 1.0;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange->Min = -1.0;

	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange == NULL)
	{
		__E("Failed to malloc for YRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange->Max = 1.0;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange->Min = -1.0;

	///////////////////////////////////////////////////////////////////////////////////////////
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace == NULL)
	{
		__E("Failed to malloc for RelativeZoomTranslationSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace ";
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange->Max = 1.0;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange->Min = 0.0;

	//#############################################################################################
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace = (struct tt__Space2DDescription *)soap_malloc(soap,  sizeof(struct tt__Space2DDescription));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace == NULL)
	{
		__E("Failed to malloc for ContinuousPanTiltVelocitySpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace ";

	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange->Max = 1.0;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange->Min = -1.0;

	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange == NULL)
	{
		__E("Failed to malloc for YRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange->Max = 1.0;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange->Min = -1.0;

	///////////////////////////////////////////////////////////////////////////////////////////
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace == NULL)
	{
		__E("Failed to malloc for ContinuousZoomVelocitySpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace ";
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange->Max = 1.0;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange->Min = 0.0;

	//######################################################################################
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->PanTiltSpeedSpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->PanTiltSpeedSpace == NULL)
	{
		__E("Failed to malloc for PanTiltSpeedSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->PanTiltSpeedSpace->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace ";
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->PanTiltSpeedSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->PanTiltSpeedSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->PanTiltSpeedSpace->XRange->Max = 1.0;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->PanTiltSpeedSpace->XRange->Min = 0.0;


	//######################################################################################
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ZoomSpeedSpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ZoomSpeedSpace == NULL)
	{
		__E("Failed to malloc for ZoomSpeedSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ZoomSpeedSpace->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace";
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ZoomSpeedSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ZoomSpeedSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->ZoomSpeedSpace->XRange->Min = 0.0;

	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->Extension = NULL;
	tptz__GetNodesResponse->PTZNode->SupportedPTZSpaces->__anyAttribute = NULL;
	tptz__GetNodesResponse->PTZNode->MaximumNumberOfPresets = ONVIF_PTZ_PRESET_NUM;
	tptz__GetNodesResponse->PTZNode->HomeSupported = xsd__boolean__true_;
	tptz__GetNodesResponse->PTZNode->__sizeAuxiliaryCommands = 0;
	tptz__GetNodesResponse->PTZNode->AuxiliaryCommands = NULL;
	tptz__GetNodesResponse->PTZNode->Extension = NULL;
	tptz__GetNodesResponse->PTZNode->__anyAttribute = NULL;
	tptz__GetNodesResponse->PTZNode->FixedHomePosition = xsd__boolean__false_;
	
	__FUN_END("\n");
	return SOAP_OK;
	
}

 int  __tptz__GetNode(struct soap* soap, struct _tptz__GetNode *tptz__GetNode, struct _tptz__GetNodeResponse *tptz__GetNodeResponse) 
{
	__FUN_BEGIN("\n");
	if(strcmp(tptz__GetNode->NodeToken, ONVIF_PTZ_TOKEN) != 0)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoEntity");
		return SOAP_FAULT;
	}
	
	tptz__GetNodeResponse->PTZNode = (struct tt__PTZNode *)soap_malloc( soap, sizeof(struct tt__PTZNode));
	if(tptz__GetNodeResponse->PTZNode == NULL)
	{
		__E("Failed to malloc for PTZNode.\n");
		return SOAP_FAULT;
	}
	
	tptz__GetNodeResponse->PTZNode->token = ONVIF_PTZ_TOKEN;
	tptz__GetNodeResponse->PTZNode->Name = ONVIF_PTZ_NAME;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces = NULL;
	
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces = (struct tt__PTZSpaces *)soap_malloc(soap, sizeof(struct tt__PTZSpaces));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces == NULL)
	{
		__E("Failed to malloc for SupportedPTZSpaces.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->__sizeAbsolutePanTiltPositionSpace = 1;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->__sizeRelativePanTiltTranslationSpace = 1;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->__sizeContinuousPanTiltVelocitySpace = 1;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->__sizeAbsoluteZoomPositionSpace = 1;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->__sizeContinuousZoomVelocitySpace = 1;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->__sizePanTiltSpeedSpace = 1; 
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->__sizeRelativeZoomTranslationSpace =1;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->__sizeZoomSpeedSpace =1;

	//#############################################################################################
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace = (struct tt__Space2DDescription *)soap_malloc(soap,  sizeof(struct tt__Space2DDescription));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace == NULL)
	{
		__E("Failed to malloc for AbsolutePanTiltPositionSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace";

	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange->Max = 1.0;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange->Min = -1.0;

	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange == NULL)
	{
		__E("Failed to malloc for YRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange->Max = 1.0;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange->Min = -1.0;

	/////////////////////////////////////////////////////////////////////////////
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace == NULL)
	{
		__E("Failed to malloc for AbsoluteZoomPositionSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace";
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange->Max = 1.0;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange->Min = 0.0;



	//#############################################################################################
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace = (struct tt__Space2DDescription *)soap_malloc(soap,  sizeof(struct tt__Space2DDescription));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace == NULL)
	{
		__E("Failed to malloc for RelativePanTiltTranslationSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace";

	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange->Max = 1.0;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange->Min = -1.0;

	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange == NULL)
	{
		__E("Failed to malloc for YRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange->Max = 1.0;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange->Min = -1.0;

	///////////////////////////////////////////////////////////////////////////////////////////
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace == NULL)
	{
		__E("Failed to malloc for RelativeZoomTranslationSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace ";
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange->Max = 1.0;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange->Min = 0.0;

	//#############################################################################################
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace = (struct tt__Space2DDescription *)soap_malloc(soap,  sizeof(struct tt__Space2DDescription));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace == NULL)
	{
		__E("Failed to malloc for ContinuousPanTiltVelocitySpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace ";

	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange->Max = 1.0;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange->Min = -1.0;

	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange == NULL)
	{
		__E("Failed to malloc for YRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange->Max = 1.0;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange->Min = -1.0;

	///////////////////////////////////////////////////////////////////////////////////////////
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace == NULL)
	{
		__E("Failed to malloc for ContinuousZoomVelocitySpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace ";
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange->Max = 1.0;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange->Min = 0.0;

	//######################################################################################
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->PanTiltSpeedSpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->PanTiltSpeedSpace == NULL)
	{
		__E("Failed to malloc for PanTiltSpeedSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->PanTiltSpeedSpace->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace ";
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->PanTiltSpeedSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->PanTiltSpeedSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->PanTiltSpeedSpace->XRange->Max = 1.0;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->PanTiltSpeedSpace->XRange->Min = 0.0;


	//######################################################################################
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ZoomSpeedSpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ZoomSpeedSpace == NULL)
	{
		__E("Failed to malloc for ZoomSpeedSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ZoomSpeedSpace->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace";
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ZoomSpeedSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ZoomSpeedSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ZoomSpeedSpace->XRange->Min = 0.0;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->Extension = NULL;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->__anyAttribute = NULL;
	tptz__GetNodeResponse->PTZNode->MaximumNumberOfPresets = ONVIF_PTZ_PRESET_NUM;
	tptz__GetNodeResponse->PTZNode->HomeSupported = xsd__boolean__true_;
	tptz__GetNodeResponse->PTZNode->FixedHomePosition = xsd__boolean__false_;
	tptz__GetNodeResponse->PTZNode->__sizeAuxiliaryCommands = 1;
	tptz__GetNodeResponse->PTZNode->AuxiliaryCommands = (char **)soap_malloc(soap, sizeof(char) * 3);
	tptz__GetNodeResponse->PTZNode->AuxiliaryCommands[0] = (char *)soap_malloc(soap, sizeof(char) * 64);
	strcpy(tptz__GetNodeResponse->PTZNode->AuxiliaryCommands[0], "command");
	tptz__GetNodeResponse->PTZNode->Extension = NULL;	
	tptz__GetNodeResponse->PTZNode->__anyAttribute = NULL;	

	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tptz__SetConfiguration(struct soap* soap, struct _tptz__SetConfiguration *tptz__SetConfiguration, struct _tptz__SetConfigurationResponse *tptz__SetConfigurationResponse) 
{
	__FUN_BEGIN("\n");
	
	if(strcmp(tptz__SetConfiguration->PTZConfiguration->Name, "TestConfig") == 0)
	{
		onvif_fault(soap,"ter:ActionNotSupported", "ter:PTZNotSupported");
		return SOAP_FAULT;
	}
	__FUN_END("\n");
	return SOAP_OK;	
}

 int  __tptz__GetConfigurationOptions(struct soap* soap, struct _tptz__GetConfigurationOptions *tptz__GetConfigurationOptions, struct _tptz__GetConfigurationOptionsResponse *tptz__GetConfigurationOptionsResponse) 
{
	__FUN_BEGIN("\n");
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions = (struct tt__PTZConfigurationOptions *)soap_malloc(soap, sizeof(struct tt__PTZConfigurationOptions));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions == NULL)
	{
		__E("Failed to malloc for PTZConfigurationOptions.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTZTimeout = (struct tt__DurationRange*)soap_malloc(soap, sizeof(struct tt__DurationRange));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTZTimeout == NULL)
	{
		__E("Failed to malloc for PTZTimeout.\n");
		return SOAP_FAULT;
	}
	
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTZTimeout->Max = "PT30S";
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTZTimeout->Min = "PT1S";

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces = (struct tt__PTZSpaces *)soap_malloc(soap, sizeof(struct tt__PTZSpaces));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces == NULL)
	{
		__E("Failed to malloc for Spaces.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->__sizeAbsolutePanTiltPositionSpace = 1;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->__sizeRelativePanTiltTranslationSpace = 1;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->__sizeContinuousPanTiltVelocitySpace = 1;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->__sizeAbsoluteZoomPositionSpace = 1;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->__sizeContinuousZoomVelocitySpace = 1;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->__sizePanTiltSpeedSpace = 1; 
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->__sizeRelativeZoomTranslationSpace =1;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->__sizeZoomSpeedSpace =1;

	//#############################################################################################
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsolutePanTiltPositionSpace = (struct tt__Space2DDescription *)soap_malloc(soap,  sizeof(struct tt__Space2DDescription));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsolutePanTiltPositionSpace == NULL)
	{
		__E("Failed to malloc for AbsolutePanTiltPositionSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsolutePanTiltPositionSpace->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace";

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsolutePanTiltPositionSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsolutePanTiltPositionSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsolutePanTiltPositionSpace->XRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsolutePanTiltPositionSpace->XRange->Min = -1.0;

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsolutePanTiltPositionSpace->YRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsolutePanTiltPositionSpace->YRange == NULL)
	{
		__E("Failed to malloc for YRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsolutePanTiltPositionSpace->YRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsolutePanTiltPositionSpace->YRange->Min = -1.0;

	/////////////////////////////////////////////////////////////////////////////
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsoluteZoomPositionSpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsoluteZoomPositionSpace == NULL)
	{
		__E("Failed to malloc for AbsoluteZoomPositionSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsoluteZoomPositionSpace->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace";
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsoluteZoomPositionSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsoluteZoomPositionSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsoluteZoomPositionSpace->XRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->AbsoluteZoomPositionSpace->XRange->Min = 0.0;



	//#############################################################################################
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativePanTiltTranslationSpace = (struct tt__Space2DDescription *)soap_malloc(soap,  sizeof(struct tt__Space2DDescription));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativePanTiltTranslationSpace == NULL)
	{
		__E("Failed to malloc for RelativePanTiltTranslationSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativePanTiltTranslationSpace->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace";

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativePanTiltTranslationSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativePanTiltTranslationSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativePanTiltTranslationSpace->XRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativePanTiltTranslationSpace->XRange->Min = -1.0;

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativePanTiltTranslationSpace->YRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativePanTiltTranslationSpace->YRange == NULL)
	{
		__E("Failed to malloc for YRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativePanTiltTranslationSpace->YRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativePanTiltTranslationSpace->YRange->Min = -1.0;

	///////////////////////////////////////////////////////////////////////////////////////////
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativeZoomTranslationSpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativeZoomTranslationSpace == NULL)
	{
		__E("Failed to malloc for RelativeZoomTranslationSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativeZoomTranslationSpace->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace ";
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativeZoomTranslationSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativeZoomTranslationSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativeZoomTranslationSpace->XRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->RelativeZoomTranslationSpace->XRange->Min = 0.0;

	//#############################################################################################
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace = (struct tt__Space2DDescription *)soap_malloc(soap,  sizeof(struct tt__Space2DDescription));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace == NULL)
	{
		__E("Failed to malloc for ContinuousPanTiltVelocitySpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace ";

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->XRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->XRange->Min = -1.0;

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->YRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->YRange == NULL)
	{
		__E("Failed to malloc for YRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->YRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->YRange->Min = -1.0;

	///////////////////////////////////////////////////////////////////////////////////////////
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace == NULL)
	{
		__E("Failed to malloc for ContinuousZoomVelocitySpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace ";
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace->XRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace->XRange->Min = 0.0;

	//######################################################################################
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->PanTiltSpeedSpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->PanTiltSpeedSpace == NULL)
	{
		__E("Failed to malloc for PanTiltSpeedSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->PanTiltSpeedSpace->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace ";
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->PanTiltSpeedSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->PanTiltSpeedSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->PanTiltSpeedSpace->XRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->PanTiltSpeedSpace->XRange->Min = 0.0;


	//######################################################################################
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ZoomSpeedSpace= (struct tt__Space1DDescription *)soap_malloc(soap,  sizeof(struct tt__Space1DDescription));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ZoomSpeedSpace == NULL)
	{
		__E("Failed to malloc for ZoomSpeedSpace.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ZoomSpeedSpace->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace";
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ZoomSpeedSpace->XRange = (struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	if(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ZoomSpeedSpace->XRange == NULL)
	{
		__E("Failed to malloc for XRange.\n");
		return SOAP_FAULT;
	}
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ZoomSpeedSpace->XRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ZoomSpeedSpace->XRange->Min = 0.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->Extension = NULL;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->__anyAttribute = NULL;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->__size = 0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->__any = NULL;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->__anyAttribute = NULL;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTControlDirection = NULL;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Extension = NULL;
	__FUN_END("\n");
	
	return SOAP_OK;
}

 int  __tptz__GotoHomePosition(struct soap* soap, struct _tptz__GotoHomePosition *tptz__GotoHomePosition, struct _tptz__GotoHomePositionResponse *tptz__GotoHomePositionResponse) 
{
	__FUN_BEGIN("\n");
	int index = 0, chn = 0;
	for (index = 0; index < ONVIF_PROFILE_NUM; index++)
		if (0 == strcmp(tptz__GotoHomePosition->ProfileToken, gOnvifInfo.OnvifProfile[index].token))
			break;

	if (index == ONVIF_PROFILE_NUM)
		return SOAP_FAULT;
	
	chn = (index>>1);	
	Nvr_Client_Crtl_Ptz(PTZ_CMD_GOTO_PRESET, chn, 10, 1);
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tptz__SetHomePosition(struct soap* soap, struct _tptz__SetHomePosition *tptz__SetHomePosition, struct _tptz__SetHomePositionResponse *tptz__SetHomePositionResponse) 
{
	__FUN_BEGIN("\n");	
	int index = 0, chn = 0;
	for (index = 0; index < ONVIF_PROFILE_NUM; index++)
		if (0 == strcmp(tptz__SetHomePosition->ProfileToken, gOnvifInfo.OnvifProfile[index].token))
			break;

	if (index == ONVIF_PROFILE_NUM)
		return SOAP_FAULT;
	chn = (index>>1);
	
	if(ONVIF_PTZ_HOME_FIXED)
	{
		onvif_fault(soap,"ter:ActionNotSupported", "ter:CannotOverwriteHome");
		return SOAP_FAULT;
	}
	else
	{
		Nvr_Client_Crtl_Ptz(PTZ_CMD_SET_PRESET, chn, 0, 1);
		return SOAP_OK;
	}
	return SOAP_FAULT;	
}

 int  __tptz__ContinuousMove(struct soap* soap, struct _tptz__ContinuousMove *tptz__ContinuousMove, struct _tptz__ContinuousMoveResponse *tptz__ContinuousMoveResponse) 
{
	__FUN_BEGIN("\n");
	
	int speed = 30;
	float zoom = 0;
	int index = 0, chn = 0;
	if(tptz__ContinuousMove->Velocity == NULL)
		return SOAP_FAULT;
	
	for (index = 0; index < ONVIF_PROFILE_NUM; index++)
		if (0 == strcmp(tptz__ContinuousMove->ProfileToken, gOnvifInfo.OnvifProfile[index].token))
			break;

	if (index == ONVIF_PROFILE_NUM)
		return SOAP_FAULT;

	chn = (index>>1);
	
	if(tptz__ContinuousMove->Velocity)
	{
		if(tptz__ContinuousMove->Velocity->Zoom)
		{
			zoom = tptz__ContinuousMove->Velocity->Zoom->x;
			if(zoom > 0)
			{
				if(zoom < 0.3)
					speed = 2;
				else if(zoom < 0.6)
					speed = 1;
				else
					speed = 0;
				Nvr_Client_Crtl_Ptz(PTZ_CMD_ZOOM_SPEED, chn, speed, 0);				
				Nvr_Client_Crtl_Ptz(PTZ_CMD_ZOOM_TELE, chn, speed, 0);
			}
			else if(zoom < 0)
			{
				if(zoom > -0.3)
					speed = 2;
				else if(zoom > -0.6)
					speed = 1;
				else
					speed = 0;	
				Nvr_Client_Crtl_Ptz(PTZ_CMD_ZOOM_SPEED, chn, speed, 0);
				Nvr_Client_Crtl_Ptz(PTZ_CMD_ZOOM_WIDE, chn,  speed, 0);
			}

			#if 0
			fSpeed = tptz__ContinuousMove->Velocity->Zoom->x;
			if(fSpeed > 0)
				i = fSpeed/0.071428;
			else if(fSpeed < 0)
				i = -fSpeed/0.071428;
			switch(i)
			{
				case 8 :
					speed = 9*1;
					break;
				case 9:
					speed = 9*2;
					break;
				case 10:
					speed = 9*3;
					break;
				case 11:
					speed = 9*4;
					break;
				case 12:
					speed = 9*5;
					break;
				case 13:
					speed = 9*6;
					break;
				case 14:
					speed = 9*7;
					break;
				default :
					speed = 9*7;
					break;
			}
			gPTZSpeed = speed;

			#endif
		}
		
		if(tptz__ContinuousMove->Velocity->PanTilt)
		{
			if(tptz__ContinuousMove->Velocity->PanTilt->y == 0)
			{
				if(tptz__ContinuousMove->Velocity->PanTilt->x > 0)
				{
					speed = tptz__ContinuousMove->Velocity->PanTilt->x *100;
					Nvr_Client_Crtl_Ptz(PTZ_CMD_RIGHT, chn, speed, 0);
				}
				else if(tptz__ContinuousMove->Velocity->PanTilt->x < 0)
				{
					speed = - tptz__ContinuousMove->Velocity->PanTilt->x *100;					
					Nvr_Client_Crtl_Ptz(PTZ_CMD_LEFT, chn, speed, 0);
				}
			}
			else if(tptz__ContinuousMove->Velocity->PanTilt->x == 0)
			{
				if(tptz__ContinuousMove->Velocity->PanTilt->y > 0)
				{
					speed = tptz__ContinuousMove->Velocity->PanTilt->y *100;
					Nvr_Client_Crtl_Ptz(PTZ_CMD_UP, chn, speed, 0);
				}
				else if(tptz__ContinuousMove->Velocity->PanTilt->y < 0)
				{
					speed = - tptz__ContinuousMove->Velocity->PanTilt->y *100;
					Nvr_Client_Crtl_Ptz(PTZ_CMD_DOWN, chn, speed, 0);
				}
			}	
		}
	}
	__FUN_END("\n");
	
	return SOAP_OK;
}

 int  __tptz__RelativeMove(struct soap* soap, struct _tptz__RelativeMove *tptz__RelativeMove, struct _tptz__RelativeMoveResponse *tptz__RelativeMoveResponse) 
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_OK;	
}

 int  __tptz__SendAuxiliaryCommand(struct soap* soap, struct _tptz__SendAuxiliaryCommand *tptz__SendAuxiliaryCommand, struct _tptz__SendAuxiliaryCommandResponse *tptz__SendAuxiliaryCommandResponse) 
{
	__FUN_BEGIN("\n");
	tptz__SendAuxiliaryCommandResponse->AuxiliaryResponse = "command";
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __tptz__AbsoluteMove(struct soap* soap, struct _tptz__AbsoluteMove *tptz__AbsoluteMove, struct _tptz__AbsoluteMoveResponse *tptz__AbsoluteMoveResponse) 
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_OK;
}

 int  __tptz__Stop(struct soap* soap, struct _tptz__Stop *tptz__Stop, struct _tptz__StopResponse *tptz__StopResponse) 
{
	__FUN_BEGIN("\n");
	int index = 0, chn = 0;
	for (index = 0; index < ONVIF_PROFILE_NUM; index++)
		if (0 == strcmp(tptz__Stop->ProfileToken, gOnvifInfo.OnvifProfile[index].token))
			break;

	if (index == ONVIF_PROFILE_NUM)
		return SOAP_FAULT;

	chn = (index>>1);
	Nvr_Client_Crtl_Ptz(PTZ_CMD_STOP, chn, 0, 0);
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __trv__GetServiceCapabilities(struct soap* soap, struct _trv__GetServiceCapabilities *trcv__GetServiceCapabilities, struct _trv__GetServiceCapabilitiesResponse *trcv__GetServiceCapabilitiesResponse) 
 {
	 __FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	 return SOAP_FAULT;  
 }
 
  int  __trv__GetReceivers(struct soap* soap, struct _trv__GetReceivers *trv__GetReceivers, struct _trv__GetReceiversResponse *trv__GetReceiversResponse) 
 {
	 onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;
 }
 
  int  __trv__GetReceiver(struct soap* soap, struct _trv__GetReceiver *trv__GetReceiver, struct _trv__GetReceiverResponse *trv__GetReceiverResponse) 
 {
	 onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;
 }
 
  int  __trv__CreateReceiver(struct soap* soap, struct _trv__CreateReceiver *trv__CreateReceiver, struct _trv__CreateReceiverResponse *trv__CreateReceiverResponse) 
 {
	 onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;
 }
 
  int  __trv__DeleteReceiver(struct soap* soap, struct _trv__DeleteReceiver *trv__DeleteReceiver, struct _trv__DeleteReceiverResponse *trv__DeleteReceiverResponse) 
 {
	 onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;
 }
 
  int  __trv__ConfigureReceiver(struct soap* soap, struct _trv__ConfigureReceiver *trv__ConfigureReceiver, struct _trv__ConfigureReceiverResponse *trv__ConfigureReceiverResponse) 
 {
	 onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;
 }
 
  int  __trv__SetReceiverMode(struct soap* soap, struct _trv__SetReceiverMode *trv__SetReceiverMode, struct _trv__SetReceiverModeResponse *trv__SetReceiverModeResponse) 
 {
	 onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	 __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	 return SOAP_FAULT;
 }
  
int	__trv__GetReceiverState(struct soap* soap, struct _trv__GetReceiverState *trv__GetReceiverState, struct _trv__GetReceiverStateResponse *trv__GetReceiverStateResponse) 
{
  onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
  __FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
  return SOAP_FAULT;
}

  int  __trc__GetServiceCapabilities(struct soap* soap, struct _trc__GetServiceCapabilities *trc__GetServiceCapabilities, struct _trc__GetServiceCapabilitiesResponse *trec__GetServiceCapabilitiesResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__CreateRecording(struct soap* soap, struct _trc__CreateRecording *trc__CreateRecording, struct _trc__CreateRecordingResponse *trc__CreateRecordingResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__DeleteRecording(struct soap* soap, struct _trc__DeleteRecording *trc__DeleteRecording, struct _trc__DeleteRecordingResponse *trc__DeleteRecordingResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__GetRecordings(struct soap* soap, struct _trc__GetRecordings *trc__GetRecordings, struct _trc__GetRecordingsResponse *trc__GetRecordingsResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__SetRecordingConfiguration(struct soap* soap, struct _trc__SetRecordingConfiguration *trc__SetRecordingConfiguration, struct _trc__SetRecordingConfigurationResponse *trc__SetRecordingConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__GetRecordingConfiguration(struct soap* soap, struct _trc__GetRecordingConfiguration *trc__GetRecordingConfiguration, struct _trc__GetRecordingConfigurationResponse *trec__GetRecordingConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__CreateTrack(struct soap* soap, struct _trc__CreateTrack *trc__CreateTrack, struct _trc__CreateTrackResponse *trc__CreateTrackResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__DeleteTrack(struct soap* soap, struct _trc__DeleteTrack *trc__DeleteTrack, struct _trc__DeleteTrackResponse *trc__DeleteTrackResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__GetTrackConfiguration(struct soap* soap, struct _trc__GetTrackConfiguration *trc__GetTrackConfiguration, struct _trc__GetTrackConfigurationResponse *trc__GetTrackConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__SetTrackConfiguration(struct soap* soap, struct _trc__SetTrackConfiguration *trc__SetTrackConfiguration, struct _trc__SetTrackConfigurationResponse *trc__SetTrackConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:NotImplemented");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__CreateRecordingJob(struct soap* soap, struct _trc__CreateRecordingJob *trc__CreateRecordingJob, struct _trc__CreateRecordingJobResponse *trc__CreateRecordingJobResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoRecordingJob");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__DeleteRecordingJob(struct soap* soap, struct _trc__DeleteRecordingJob *trc__DeleteRecordingJob, struct _trc__DeleteRecordingJobResponse *trc__DeleteRecordingJobResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoRecordingJob");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__GetRecordingJobs(struct soap* soap, struct _trc__GetRecordingJobs *trc__GetRecordingJobs, struct _trc__GetRecordingJobsResponse *trc__GetRecordingJobsResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoRecordingJob");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__SetRecordingJobConfiguration(struct soap* soap, struct _trc__SetRecordingJobConfiguration *trc__SetRecordingJobConfiguration, struct _trc__SetRecordingJobConfigurationResponse *trc__SetRecordingJobConfigurationResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoRecordingJob");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__GetRecordingJobConfiguration(struct soap* soap, struct _trc__GetRecordingJobConfiguration *trc__GetRecordingJobConfiguration, struct _trc__GetRecordingJobConfigurationResponse *trc__GetRecordingJobConfigurationResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoRecordingJob");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__SetRecordingJobMode(struct soap* soap, struct _trc__SetRecordingJobMode *trc__SetRecordingJobMode, struct _trc__SetRecordingJobModeResponse *trc__SetRecordingJobModeResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoRecordingJob");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trc__GetRecordingJobState(struct soap* soap, struct _trc__GetRecordingJobState *trc__GetRecordingJobState, struct _trc__GetRecordingJobStateResponse *trc__GetRecordingJobStateResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoRecordingJob");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trp__GetServiceCapabilities(struct soap* soap, struct _trp__GetServiceCapabilities *trp__GetServiceCapabilities, struct _trp__GetServiceCapabilitiesResponse *trp__GetServiceCapabilitiesResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoRecordingJob");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trp__GetReplayUri(struct soap* soap, struct _trp__GetReplayUri *trp__GetReplayUri, struct _trp__GetReplayUriResponse *trp__GetReplayUriResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoProfile");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trp__GetReplayConfiguration(struct soap* soap, struct _trp__GetReplayConfiguration *trp__GetReplayConfiguration, struct _trp__GetReplayConfigurationResponse *trp__GetReplayConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:ReplayNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trp__SetReplayConfiguration(struct soap* soap, struct _trp__SetReplayConfiguration *trp__SetReplayConfiguration, struct _trp__SetReplayConfigurationResponse *trp__SetReplayConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:ReplayNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}
//video
 int  __trt__GetServiceCapabilities(struct soap* soap, struct _trt__GetServiceCapabilities *trt__GetServiceCapabilities, struct _trt__GetServiceCapabilitiesResponse *trt__GetServiceCapabilitiesResponse) 
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __trt__GetVideoSources(struct soap* soap, struct _trt__GetVideoSources *trt__GetVideoSources, struct _trt__GetVideoSourcesResponse *trt__GetVideoSourcesResponse) 
{
	int idx;
	HI_ONVIF_VIDEO_SOURCE_S OnvifVideoSource;
	struct tt__VideoSource *VideoSources;
	struct tt__VideoSource *pVideoSources;
	
	__FUN_BEGIN("\n");
	trt__GetVideoSourcesResponse->__sizeVideoSources = ONVIF_VIDEO_SOURCE_NUM;
	ALLOC_STRUCT_NUM(pVideoSources, struct tt__VideoSource, ONVIF_VIDEO_SOURCE_NUM);
	for(idx = 0; idx < ONVIF_VIDEO_SOURCE_NUM; idx++)
	{
		NVR_DEV_IMA_CFG_S stIma;
		//HI_DEV_3A_CFG_S st3a;	
		nvr_get_ipnc_param(0, IPNC_CMD_IMA_CFG, &stIma, sizeof(stIma));
		//hi_param_get_3a_cfg(idx, &st3a);		
		VideoSources = &pVideoSources[idx];
		onvif_get_video_source(idx, &OnvifVideoSource);	
		ALLOC_TOKEN(VideoSources->token, OnvifVideoSource.token);		
		VideoSources->Framerate = OnvifVideoSource.Framerate;	
		ALLOC_STRUCT(VideoSources->Resolution, struct tt__VideoResolution);	
		VideoSources->Resolution->Width = OnvifVideoSource.Resolution.Width;
		VideoSources->Resolution->Height = OnvifVideoSource.Resolution.Height;	
		ALLOC_STRUCT(VideoSources->Imaging, struct tt__ImagingSettings);
		ALLOC_STRUCT(VideoSources->Imaging->BacklightCompensation, struct tt__BacklightCompensation);
		VideoSources->Imaging->BacklightCompensation->Mode = tt__BacklightCompensationMode__OFF;
		ALLOC_STRUCT(VideoSources->Imaging->Brightness, float);
		ALLOC_STRUCT(VideoSources->Imaging->ColorSaturation, float);
		ALLOC_STRUCT(VideoSources->Imaging->Contrast, float);
		ALLOC_STRUCT(VideoSources->Imaging->Sharpness, float);
		ALLOC_STRUCT(VideoSources->Imaging->WhiteBalance, struct tt__WhiteBalance);
		ALLOC_STRUCT(VideoSources->Extension, struct tt__VideoSourceExtension);
		ALLOC_STRUCT(VideoSources->Extension->Imaging, struct tt__ImagingSettings20);
		ALLOC_STRUCT(VideoSources->Extension->Imaging->Brightness, float);
		ALLOC_STRUCT(VideoSources->Extension->Imaging->ColorSaturation, float);
		ALLOC_STRUCT(VideoSources->Extension->Imaging->Contrast, float);
		ALLOC_STRUCT(VideoSources->Extension->Imaging->Sharpness, float);
		//ALLOC_STRUCT(VideoSources->Extension->Imaging->WideDynamicRange, struct tt__WideDynamicRange20);
		ALLOC_STRUCT(VideoSources->Extension->Imaging->WhiteBalance, struct tt__WhiteBalance20);
		ALLOC_STRUCT(VideoSources->Extension->Imaging->WhiteBalance->CrGain, float);
		ALLOC_STRUCT(VideoSources->Extension->Imaging->WhiteBalance->CbGain, float);
		
		if(stIma.eSuppMask & VCT_IMA_BRIGHTNESS)
		{		
			*VideoSources->Imaging->Brightness = (float)stIma.struBrightness.u8Value;
			*VideoSources->Extension->Imaging->Brightness = (float)stIma.struBrightness.u8Value;
		}
			
		if(stIma.eSuppMask & VCT_IMA_CONTRAST)
		{			
			*VideoSources->Imaging->Contrast = (float)stIma.struContrast.u8Value;
			*VideoSources->Extension->Imaging->Contrast = (float)stIma.struContrast.u8Value;
		}
		
		if(stIma.eSuppMask & VCT_IMA_SATURATION)
		{		
			*VideoSources->Imaging->ColorSaturation  = (float)stIma.struSaturation.u8Value;
			*VideoSources->Extension->Imaging->ColorSaturation = (float)stIma.struSaturation.u8Value;
		}
		
		if(stIma.eSuppMask & VCT_IMA_SHARPNESS)
		{		
			*VideoSources->Imaging->Sharpness  = (float)stIma.struSharpness.u8Value;
			*VideoSources->Extension->Imaging->Sharpness = (float)stIma.struSharpness.u8Value;
		}

		VideoSources->Imaging->WhiteBalance->Mode = tt__WhiteBalanceMode__AUTO;	
		VideoSources->Extension->Imaging->WhiteBalance->Mode = tt__WhiteBalanceMode__AUTO;
		
		if(stIma.eSuppMask & VCT_IMA_RED)
		{		
			VideoSources->Imaging->WhiteBalance->CrGain = (float)stIma.struRed.u8Value;
			*VideoSources->Extension->Imaging->WhiteBalance->CrGain = (float)stIma.struRed.u8Value;
		}

		if(stIma.eSuppMask & VCT_IMA_BLUE)
		{		
			VideoSources->Imaging->WhiteBalance->CbGain = (float)stIma.struBlue.u8Value;
			*VideoSources->Extension->Imaging->WhiteBalance->CbGain = (float)stIma.struBlue.u8Value;
		}	
		

	}	
	
	trt__GetVideoSourcesResponse->VideoSources = pVideoSources;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetAudioSources(struct soap *soap, struct _trt__GetAudioSources *trt__GetAudioSources, struct _trt__GetAudioSourcesResponse *trt__GetAudioSourcesResponse)
{
	int idx;
	HI_ONVIF_AUDIO_SOURCE_S OnvifAudioSource;
	struct tt__AudioSource *AudioSources;

	__FUN_BEGIN("\n");
	ALLOC_STRUCT_NUM(AudioSources, struct tt__AudioSource, ONVIF_AUDIO_SOURCE_NUM);
	for(idx = 0; idx < ONVIF_AUDIO_SOURCE_NUM; idx++)
	{
		onvif_get_audio_source(idx, &OnvifAudioSource);
			
		ALLOC_TOKEN(AudioSources[idx ].token, OnvifAudioSource.token);
		AudioSources[idx ].Channels = OnvifAudioSource.Channels;
	}
	
	trt__GetAudioSourcesResponse->__sizeAudioSources = ONVIF_AUDIO_SOURCE_NUM;	
	trt__GetAudioSourcesResponse->AudioSources = AudioSources;
	__FUN_END("\n");
	
	return SOAP_OK;
}

 int  __trt__GetAudioOutputs(struct soap* soap, struct _trt__GetAudioOutputs *trt__GetAudioOutputs, struct _trt__GetAudioOutputsResponse *trt__GetAudioOutputsResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioOutputNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__CreateProfile(struct soap* soap, struct _trt__CreateProfile *trt__CreateProfile, struct _trt__CreateProfileResponse *trt__CreateProfileResponse) 
{
	#if 0
	int i;
	int count = 0;
	__FUN_BEGIN("\n");

	#if 1
	if(trt__CreateProfile->Name == NULL)
		return SOAP_FAULT;
	for(i = 0; i < ONVIF_PROFILE_NUM; i++)
	{
		if(gOnvifInfo.OnvifProfile[i].token != NULL)
			count ++;
		if(count == ONVIF_PROFILE_NUM)
		{
			//onvif_fault(soap,"ter:InvalidArgVal", "ter:MaxNVTProfiles"); 
			//return SOAP_FAULT;
		}
		if(strcmp(gOnvifInfo.OnvifProfile[i].Name, trt__CreateProfile->Name) == 0)
		{
			onvif_fault(soap,"ter:InvalidArgVal", "ter:ProfileExists"); 
			return SOAP_FAULT;
		}
	}
	#endif
	struct tt__Profile *Profile;

	Profile = (struct tt__Profile *)soap_malloc(soap, sizeof(struct tt__Profile));
	if(Profile == NULL)
	{
		__E("Failed to malloc for Profile.\n");
		return SOAP_FAULT;
	}
	Profile = &gOnvifInfo.Profile[0];
	Profile->Name = gOnvifInfo.OnvifProfile[0].Name;
	Profile->token = trt__CreateProfile->Token;
	Profile->fixed = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(enum xsd__boolean_));
	if(Profile->fixed == NULL)
	{
		__E("Failed to malloc for fixed.\n");
		return SOAP_FAULT;
	}
	*Profile->fixed = gOnvifInfo.OnvifProfile[0].fixed;	
	Profile->__anyAttribute = NULL;

	trt__CreateProfileResponse->Profile = Profile;
	__FUN_END("\n");
	return SOAP_OK;
	#endif
	__FUN_BEGIN("\n");
	struct tt__Profile *Profile;
	struct tt__VideoSourceConfiguration *VideoSourceConfiguration;
	struct tt__VideoEncoderConfiguration *VideoEncoderConfiguration;
	struct tt__AudioSourceConfiguration *AudioSourceConfiguration;
	struct tt__AudioEncoderConfiguration *AudioEncoderConfiguration;
	struct tt__PTZConfiguration *PTZConfiguration;

	HI_ONVIF_PROFILE_S OnvifProfile;
	HI_ONVIF_VIDEO_SOURCE_CFG_S OnvifVideoSourceCfg;
	HI_ONVIF_VIDEO_ENCODER_CFG_S OnvifVideoEncoderCfg;
	HI_ONVIF_AUDIO_SOURCE_CFG_S OnvifAudioSourceCfg;
	HI_ONVIF_AUDIO_ENCODER_CFG_S OnvifAudioEncoderCfg;

	NVR_DEV_VIDEO_CFG_S VideoCfg;

	if(trt__CreateProfile->Name == NULL)
		return SOAP_FAULT;
	unsigned long ip;
	ip = onvif_get_ipaddr(soap);
	
	if(trt__CreateProfile->Token == NULL)
	{
		printf(">>>>>>>the name is %s\r\n", trt__CreateProfile->Name);
		onvif_get_profile(0, ip, &OnvifProfile);
	}else if(onvif_get_profile_from_token(ip, trt__CreateProfile->Token, &OnvifProfile) < 0)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoProfile"); 
		return SOAP_FAULT;
	}
	
	ALLOC_STRUCT(Profile, struct tt__Profile);
	ALLOC_TOKEN(Profile->Name, OnvifProfile.Name);
	ALLOC_TOKEN(Profile->token, OnvifProfile.token);
	ALLOC_STRUCT(Profile->fixed,  enum xsd__boolean_);	
	*Profile->fixed = OnvifProfile.fixed;	// 这个profile不能删除
	
	onvif_get_video_source_cfg(OnvifProfile.videoSourceCfgIdx, &OnvifVideoSourceCfg);
	ALLOC_STRUCT(VideoSourceConfiguration,  struct tt__VideoSourceConfiguration);
	ALLOC_TOKEN(VideoSourceConfiguration->Name, OnvifVideoSourceCfg.Name);
	ALLOC_TOKEN(VideoSourceConfiguration->SourceToken, OnvifVideoSourceCfg.SourceToken);
	VideoSourceConfiguration->UseCount = OnvifVideoSourceCfg.UseCount;
	ALLOC_STRUCT(VideoSourceConfiguration->Bounds, struct tt__IntRectangle);	
	VideoSourceConfiguration->Bounds->y = OnvifVideoSourceCfg.Bounds.y;
	VideoSourceConfiguration->Bounds->x = OnvifVideoSourceCfg.Bounds.x;
	VideoSourceConfiguration->Bounds->width  = OnvifVideoSourceCfg.Bounds.width;
	VideoSourceConfiguration->Bounds->height = OnvifVideoSourceCfg.Bounds.height;
	Profile->VideoSourceConfiguration = VideoSourceConfiguration;
	
	/* AudioSourceConfiguration */
	onvif_get_audio_source_cfg(OnvifProfile.audioSourceCfgIdx, &OnvifAudioSourceCfg);
	ALLOC_STRUCT(AudioSourceConfiguration, struct tt__AudioSourceConfiguration);
	ALLOC_TOKEN(AudioSourceConfiguration->Name, OnvifAudioSourceCfg.Name);    		
	ALLOC_TOKEN(AudioSourceConfiguration->token, OnvifAudioSourceCfg.token);
	ALLOC_TOKEN(AudioSourceConfiguration->SourceToken, OnvifAudioSourceCfg.SourceToken);
	AudioSourceConfiguration->UseCount = OnvifAudioSourceCfg.UseCount;
	Profile->AudioSourceConfiguration = AudioSourceConfiguration;
		
	/*VideoEncoderConfiguration */
	onvif_get_video_encoder_cfg(OnvifProfile.videoEncoderCfgIdx, &OnvifVideoEncoderCfg);
	ALLOC_STRUCT(VideoEncoderConfiguration, struct tt__VideoEncoderConfiguration);	
	ALLOC_TOKEN(VideoEncoderConfiguration->Name, OnvifVideoEncoderCfg.Name);
	ALLOC_TOKEN(VideoEncoderConfiguration->token, OnvifVideoEncoderCfg.token);
	VideoEncoderConfiguration->UseCount = OnvifVideoEncoderCfg.UseCount;	
	VideoEncoderConfiguration->Encoding = OnvifVideoEncoderCfg.Encoding;
	ALLOC_STRUCT(VideoEncoderConfiguration->Resolution, struct tt__VideoResolution);		
	VideoEncoderConfiguration->Resolution->Width = OnvifVideoEncoderCfg.Resolution.Width;
	VideoEncoderConfiguration->Resolution->Height = OnvifVideoEncoderCfg.Resolution.Height;
	VideoEncoderConfiguration->Quality = OnvifVideoEncoderCfg.Quality; 
	ALLOC_STRUCT(VideoEncoderConfiguration->RateControl, struct tt__VideoRateControl);
	VideoEncoderConfiguration->RateControl->FrameRateLimit =  OnvifVideoEncoderCfg.FrameRate;   
	VideoEncoderConfiguration->RateControl->EncodingInterval = OnvifVideoEncoderCfg.Gop;
	VideoEncoderConfiguration->RateControl->BitrateLimit =  OnvifVideoEncoderCfg.Bitrate;
	
	VideoEncoderConfiguration->MPEG4 = NULL;

	//nvr_logon_get_video_cfg(0, &VideoCfg);
	ALLOC_STRUCT(VideoEncoderConfiguration->H264, struct tt__H264Configuration);
	
	VideoEncoderConfiguration->H264->GovLength = VideoCfg.struMainVenc.u8Gop;
	
	switch(VideoCfg.eEncodeLevel)
	{
	case H264_HIGH:
		VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__High;
		break;
	case H264_MAIN:
		VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Main;
		break;
	case H264_BASELINE:
		VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Baseline;
		break;			
	}
	ALLOC_STRUCT(VideoEncoderConfiguration->Multicast, struct tt__MulticastConfiguration);
	ALLOC_STRUCT(VideoEncoderConfiguration->Multicast->Address, struct tt__IPAddress);
	VideoEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
	ALLOC_TOKEN(VideoEncoderConfiguration->Multicast->Address->IPv4Address, ONVIF_WSD_MC_ADDR);
	
	VideoEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
	VideoEncoderConfiguration->Multicast->Port = 9090;
	VideoEncoderConfiguration->Multicast->TTL = 1500;
	VideoEncoderConfiguration->Multicast->AutoStart = xsd__boolean__false_;

	ALLOC_TOKEN(VideoEncoderConfiguration->SessionTimeout, "PT5S");	
	Profile->VideoEncoderConfiguration = VideoEncoderConfiguration;

	/* AudioEncoderConfiguration */

	onvif_get_audio_encoder_cfg(OnvifProfile.audioEncoderCfgIdx, &OnvifAudioEncoderCfg);
	ALLOC_STRUCT(AudioEncoderConfiguration, struct tt__AudioEncoderConfiguration);
	ALLOC_TOKEN(AudioEncoderConfiguration->Name, OnvifAudioEncoderCfg.Name);
	AudioEncoderConfiguration->UseCount = OnvifAudioEncoderCfg.UseCount;
	ALLOC_TOKEN(AudioEncoderConfiguration->token, OnvifAudioEncoderCfg.token);
	AudioEncoderConfiguration->Encoding = OnvifAudioEncoderCfg.Encoding;
	AudioEncoderConfiguration->Bitrate = OnvifAudioEncoderCfg.Bitrate;
	AudioEncoderConfiguration->SampleRate = OnvifAudioEncoderCfg.SampleRate;
	ALLOC_STRUCT(AudioEncoderConfiguration->Multicast, struct tt__MulticastConfiguration);
	ALLOC_STRUCT(AudioEncoderConfiguration->Multicast->Address, struct tt__IPAddress);
	AudioEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
	ALLOC_TOKEN(AudioEncoderConfiguration->Multicast->Address->IPv4Address, ONVIF_WSD_MC_ADDR);
	AudioEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
	AudioEncoderConfiguration->Multicast->Port = 9090;
	AudioEncoderConfiguration->Multicast->TTL = 1500;
	AudioEncoderConfiguration->Multicast->AutoStart = xsd__boolean__false_;	
	ALLOC_TOKEN(AudioEncoderConfiguration->SessionTimeout, "PT15S");		
	Profile->AudioEncoderConfiguration = AudioEncoderConfiguration;
	/* VideoAnalyticsConfiguration */
	Profile->VideoAnalyticsConfiguration = NULL;
	/* PTZConfiguration */	  
	ALLOC_STRUCT(PTZConfiguration, struct tt__PTZConfiguration);	
	ALLOC_TOKEN(PTZConfiguration->Name, ONVIF_PTZ_NAME);   
	PTZConfiguration->UseCount = 1;
	ALLOC_TOKEN(PTZConfiguration->token, ONVIF_PTZ_TOKEN);   
	ALLOC_TOKEN(PTZConfiguration->token, ONVIF_PTZ_TOKEN);  
	ALLOC_TOKEN(PTZConfiguration->NodeToken, ONVIF_PTZ_TOKEN);
	ALLOC_TOKEN(PTZConfiguration->DefaultAbsolutePantTiltPositionSpace, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
	ALLOC_TOKEN(PTZConfiguration->DefaultAbsoluteZoomPositionSpace ,"http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace");
	ALLOC_TOKEN(PTZConfiguration->DefaultRelativePanTiltTranslationSpace,  "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace");
	ALLOC_TOKEN(PTZConfiguration->DefaultRelativeZoomTranslationSpace,  "http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace ");
	ALLOC_TOKEN(PTZConfiguration->DefaultContinuousPanTiltVelocitySpace,  "http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace ");
	ALLOC_TOKEN(PTZConfiguration->DefaultContinuousZoomVelocitySpace,  "http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace ");
	ALLOC_STRUCT(PTZConfiguration->DefaultPTZSpeed, struct tt__PTZSpeed);
	ALLOC_STRUCT(PTZConfiguration->DefaultPTZSpeed->PanTilt, struct tt__Vector2D);    		   
	ALLOC_TOKEN(PTZConfiguration->DefaultPTZSpeed->PanTilt->space, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace ");
	PTZConfiguration->DefaultPTZSpeed->PanTilt->x = 1.0;
	PTZConfiguration->DefaultPTZSpeed->PanTilt->y =0.5;
	ALLOC_STRUCT(PTZConfiguration->DefaultPTZSpeed->Zoom, struct tt__Vector1D);    	
	ALLOC_TOKEN(PTZConfiguration->DefaultPTZSpeed->Zoom->space, "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace ");
	PTZConfiguration->DefaultPTZSpeed->Zoom->x = 0.0;
	ALLOC_TOKEN(PTZConfiguration->DefaultPTZTimeout, "PT5S");    

    	//#####################################################
    	ALLOC_STRUCT(PTZConfiguration->PanTiltLimits, struct tt__PanTiltLimits);
 	ALLOC_STRUCT(PTZConfiguration->PanTiltLimits->Range, struct tt__Space2DDescription);
   	ALLOC_TOKEN(PTZConfiguration->PanTiltLimits->Range->URI, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
    	ALLOC_STRUCT(PTZConfiguration->PanTiltLimits->Range->XRange, struct tt__FloatRange);    	
    	PTZConfiguration->PanTiltLimits->Range->XRange->Max= 1.0;
    	PTZConfiguration->PanTiltLimits->Range->XRange->Min= - 1.0;
    	ALLOC_STRUCT(PTZConfiguration->PanTiltLimits->Range->YRange, struct tt__FloatRange);    
    	PTZConfiguration->PanTiltLimits->Range->YRange->Max= 1.0;
    	PTZConfiguration->PanTiltLimits->Range->YRange->Min= - 1.0;	
    
    	//############################################################
    	ALLOC_STRUCT(PTZConfiguration->ZoomLimits, struct tt__ZoomLimits);  
    	ALLOC_STRUCT(PTZConfiguration->ZoomLimits->Range, struct tt__Space1DDescription);
    	ALLOC_TOKEN(PTZConfiguration->ZoomLimits->Range->URI, "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace");
 	ALLOC_STRUCT(PTZConfiguration->ZoomLimits->Range->XRange, struct tt__FloatRange);
      	PTZConfiguration->ZoomLimits->Range->XRange->Max= 1.0;
    	PTZConfiguration->ZoomLimits->Range->XRange->Min= 0.0;

	Profile->PTZConfiguration = PTZConfiguration;
	/* MetadataConfiguration */
	Profile->MetadataConfiguration = NULL;	

	//ALLOC_STRUCT(Profile->Extension, struct tt__ProfileExtension);
	//ALLOC_STRUCT(Profile->Extension->AudioOutputConfiguration, struct tt__ProfileExtension);	

	trt__CreateProfileResponse->Profile = Profile;
	__FUN_END("\n");
	return SOAP_OK;
}

int  __trt__GetProfile(struct soap* soap, struct _trt__GetProfile *trt__GetProfile, struct _trt__GetProfileResponse *trt__GetProfileResponse) 
{
	struct tt__Profile *Profile;
	struct tt__VideoSourceConfiguration *VideoSourceConfiguration;
	struct tt__VideoEncoderConfiguration *VideoEncoderConfiguration;
	struct tt__AudioSourceConfiguration *AudioSourceConfiguration;
	struct tt__AudioEncoderConfiguration *AudioEncoderConfiguration;
	struct tt__PTZConfiguration *PTZConfiguration;
	//NVR_DEV_VIDEO_CFG_S VideoCfg;

	int encodeLevel = 0;
	HI_ONVIF_PROFILE_S OnvifProfile;
	HI_ONVIF_VIDEO_SOURCE_CFG_S OnvifVideoSourceCfg;
	HI_ONVIF_VIDEO_ENCODER_CFG_S OnvifVideoEncoderCfg;
	HI_ONVIF_AUDIO_SOURCE_CFG_S OnvifAudioSourceCfg;
	HI_ONVIF_AUDIO_ENCODER_CFG_S OnvifAudioEncoderCfg;
	__FUN_BEGIN("\n");

	//int i = 0;
	//int chn = 0;
	unsigned long ip;
	ip = onvif_get_ipaddr(soap);
	
	//for(i = 0; i < ONVIF_PROFILE_NUM; i++)
	//{
	//	if(strcmp(trt__GetProfile->ProfileToken, gOnvifInfo.OnvifProfile[chn].token)==0)
	//		break;
	//}
	
	//if (chn == ONVIF_PROFILE_NUM) 
	//		return SOAP_FAULT;	
	
	if(trt__GetProfile->ProfileToken == NULL)
	{
		onvif_fault(soap,"ter:InvalidArgVal", "ter:InvalidInputToken"); 
		return SOAP_FAULT;
	}	

	if(onvif_get_profile_from_token(ip, trt__GetProfile->ProfileToken, &OnvifProfile) < 0)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoProfile"); 
		return SOAP_FAULT;
	}
	
	ALLOC_STRUCT(Profile, struct tt__Profile);
	ALLOC_TOKEN(Profile->Name, OnvifProfile.Name);
	ALLOC_TOKEN(Profile->token, OnvifProfile.token);
	ALLOC_STRUCT(Profile->fixed,  enum xsd__boolean_);	
	*Profile->fixed = OnvifProfile.fixed;	// 这个profile不能删除
	
	/* VideoSourceConfiguration */
	onvif_get_video_source_cfg(OnvifProfile.videoSourceCfgIdx, &OnvifVideoSourceCfg);
	ALLOC_STRUCT(VideoSourceConfiguration,  struct tt__VideoSourceConfiguration);
	ALLOC_TOKEN(VideoSourceConfiguration->Name, OnvifVideoSourceCfg.Name);
	ALLOC_TOKEN(VideoSourceConfiguration->SourceToken, OnvifVideoSourceCfg.SourceToken);
	VideoSourceConfiguration->UseCount = OnvifVideoSourceCfg.UseCount;
	ALLOC_STRUCT(VideoSourceConfiguration->Bounds, struct tt__IntRectangle);	
	VideoSourceConfiguration->Bounds->y = OnvifVideoSourceCfg.Bounds.y;
	VideoSourceConfiguration->Bounds->x = OnvifVideoSourceCfg.Bounds.x;
	VideoSourceConfiguration->Bounds->width  = OnvifVideoSourceCfg.Bounds.width;
	VideoSourceConfiguration->Bounds->height = OnvifVideoSourceCfg.Bounds.height;
	Profile->VideoSourceConfiguration = VideoSourceConfiguration;
	
	/* AudioSourceConfiguration */
	onvif_get_audio_source_cfg(OnvifProfile.audioSourceCfgIdx, &OnvifAudioSourceCfg);
	ALLOC_STRUCT(AudioSourceConfiguration, struct tt__AudioSourceConfiguration);
	ALLOC_TOKEN(AudioSourceConfiguration->Name, OnvifAudioSourceCfg.Name);    		
	ALLOC_TOKEN(AudioSourceConfiguration->token, OnvifAudioSourceCfg.token);
	ALLOC_TOKEN(AudioSourceConfiguration->SourceToken, OnvifAudioSourceCfg.SourceToken);
	AudioSourceConfiguration->UseCount = OnvifAudioSourceCfg.UseCount;
	Profile->AudioSourceConfiguration = AudioSourceConfiguration;

	/*VideoEncoderConfiguration */
	
	onvif_get_video_encoder_cfg(OnvifProfile.videoEncoderCfgIdx, &OnvifVideoEncoderCfg);
	ALLOC_STRUCT(VideoEncoderConfiguration, struct tt__VideoEncoderConfiguration);	
	ALLOC_TOKEN(VideoEncoderConfiguration->Name, OnvifVideoEncoderCfg.Name);
	ALLOC_TOKEN(VideoEncoderConfiguration->token, OnvifVideoEncoderCfg.token);
	VideoEncoderConfiguration->UseCount = OnvifVideoEncoderCfg.UseCount;	
	VideoEncoderConfiguration->Encoding = OnvifVideoEncoderCfg.Encoding;
	ALLOC_STRUCT(VideoEncoderConfiguration->Resolution, struct tt__VideoResolution);		
	VideoEncoderConfiguration->Resolution->Width = OnvifVideoEncoderCfg.Resolution.Width;
	VideoEncoderConfiguration->Resolution->Height = OnvifVideoEncoderCfg.Resolution.Height;
	VideoEncoderConfiguration->Quality = OnvifVideoEncoderCfg.Quality; 
	ALLOC_STRUCT(VideoEncoderConfiguration->RateControl, struct tt__VideoRateControl);
	VideoEncoderConfiguration->RateControl->FrameRateLimit =  OnvifVideoEncoderCfg.FrameRate;   
	VideoEncoderConfiguration->RateControl->EncodingInterval = OnvifVideoEncoderCfg.Gop;
	VideoEncoderConfiguration->RateControl->BitrateLimit =  OnvifVideoEncoderCfg.Bitrate;
	
	VideoEncoderConfiguration->MPEG4 = NULL;
	
	ALLOC_STRUCT(VideoEncoderConfiguration->H264, struct tt__H264Configuration);
	
	VideoEncoderConfiguration->H264->GovLength = OnvifVideoEncoderCfg.Gop;//VideoCfg.struMainVenc.u8Gop;
	encodeLevel = (int)OnvifVideoEncoderCfg.Quality;
	switch(encodeLevel)
	{
	case H264_HIGH:
		VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__High;
		break;
	case H264_MAIN:
		VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Main;
		break;
	case H264_BASELINE:
		VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Baseline;
		break;			
	}
	ALLOC_STRUCT(VideoEncoderConfiguration->Multicast, struct tt__MulticastConfiguration);
	ALLOC_STRUCT(VideoEncoderConfiguration->Multicast->Address, struct tt__IPAddress);
	VideoEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
	ALLOC_TOKEN(VideoEncoderConfiguration->Multicast->Address->IPv4Address, ONVIF_WSD_MC_ADDR);
	
	VideoEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
	VideoEncoderConfiguration->Multicast->Port = 9090;
	VideoEncoderConfiguration->Multicast->TTL = 1500;
	VideoEncoderConfiguration->Multicast->AutoStart = xsd__boolean__false_;

	ALLOC_TOKEN(VideoEncoderConfiguration->SessionTimeout, "PT5S");	
	Profile->VideoEncoderConfiguration = VideoEncoderConfiguration;

	/* AudioEncoderConfiguration */
	onvif_get_audio_encoder_cfg(OnvifProfile.audioEncoderCfgIdx, &OnvifAudioEncoderCfg);
	ALLOC_STRUCT(AudioEncoderConfiguration, struct tt__AudioEncoderConfiguration);
	ALLOC_TOKEN(AudioEncoderConfiguration->Name, OnvifAudioEncoderCfg.Name);
	AudioEncoderConfiguration->UseCount = OnvifAudioEncoderCfg.UseCount;
	ALLOC_TOKEN(AudioEncoderConfiguration->token, OnvifAudioEncoderCfg.token);
	AudioEncoderConfiguration->Encoding = OnvifAudioEncoderCfg.Encoding;
	AudioEncoderConfiguration->Bitrate = OnvifAudioEncoderCfg.Bitrate;
	AudioEncoderConfiguration->SampleRate = OnvifAudioEncoderCfg.SampleRate;
	ALLOC_STRUCT(AudioEncoderConfiguration->Multicast, struct tt__MulticastConfiguration);
	ALLOC_STRUCT(AudioEncoderConfiguration->Multicast->Address, struct tt__IPAddress);
	AudioEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
	ALLOC_TOKEN(AudioEncoderConfiguration->Multicast->Address->IPv4Address, ONVIF_WSD_MC_ADDR);
	AudioEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
	AudioEncoderConfiguration->Multicast->Port = 9090;
	AudioEncoderConfiguration->Multicast->TTL = 1500;
	AudioEncoderConfiguration->Multicast->AutoStart = xsd__boolean__false_;	
	ALLOC_TOKEN(AudioEncoderConfiguration->SessionTimeout, "PT15S");		
	Profile->AudioEncoderConfiguration = AudioEncoderConfiguration;
	/* VideoAnalyticsConfiguration */
	Profile->VideoAnalyticsConfiguration = NULL;
	/* PTZConfiguration */	  
	ALLOC_STRUCT(PTZConfiguration, struct tt__PTZConfiguration);	
	ALLOC_TOKEN(PTZConfiguration->Name, ONVIF_PTZ_NAME);   
	PTZConfiguration->UseCount = 1;
	ALLOC_TOKEN(PTZConfiguration->token, ONVIF_PTZ_TOKEN);   
	ALLOC_TOKEN(PTZConfiguration->token, ONVIF_PTZ_TOKEN);  
	ALLOC_TOKEN(PTZConfiguration->NodeToken, ONVIF_PTZ_TOKEN);
	ALLOC_TOKEN(PTZConfiguration->DefaultAbsolutePantTiltPositionSpace, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
	ALLOC_TOKEN(PTZConfiguration->DefaultAbsoluteZoomPositionSpace ,"http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace");
	ALLOC_TOKEN(PTZConfiguration->DefaultRelativePanTiltTranslationSpace,  "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace");
	ALLOC_TOKEN(PTZConfiguration->DefaultRelativeZoomTranslationSpace,  "http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace ");
	ALLOC_TOKEN(PTZConfiguration->DefaultContinuousPanTiltVelocitySpace,  "http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace ");
	ALLOC_TOKEN(PTZConfiguration->DefaultContinuousZoomVelocitySpace,  "http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace ");
	ALLOC_STRUCT(PTZConfiguration->DefaultPTZSpeed, struct tt__PTZSpeed);
	ALLOC_STRUCT(PTZConfiguration->DefaultPTZSpeed->PanTilt, struct tt__Vector2D);    		   
	ALLOC_TOKEN(PTZConfiguration->DefaultPTZSpeed->PanTilt->space, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace ");
	PTZConfiguration->DefaultPTZSpeed->PanTilt->x = 1.0;
	PTZConfiguration->DefaultPTZSpeed->PanTilt->y =0.5;
	ALLOC_STRUCT(PTZConfiguration->DefaultPTZSpeed->Zoom, struct tt__Vector1D);    	
	ALLOC_TOKEN(PTZConfiguration->DefaultPTZSpeed->Zoom->space, "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace ");
	PTZConfiguration->DefaultPTZSpeed->Zoom->x = 0.0;
	ALLOC_TOKEN(PTZConfiguration->DefaultPTZTimeout, "PT5S");    

    	//#####################################################
    	ALLOC_STRUCT(PTZConfiguration->PanTiltLimits, struct tt__PanTiltLimits);
 	ALLOC_STRUCT(PTZConfiguration->PanTiltLimits->Range, struct tt__Space2DDescription);
   	ALLOC_TOKEN(PTZConfiguration->PanTiltLimits->Range->URI, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
    	ALLOC_STRUCT(PTZConfiguration->PanTiltLimits->Range->XRange, struct tt__FloatRange);    	
    	PTZConfiguration->PanTiltLimits->Range->XRange->Max= 1.0;
    	PTZConfiguration->PanTiltLimits->Range->XRange->Min= - 1.0;
    	ALLOC_STRUCT(PTZConfiguration->PanTiltLimits->Range->YRange, struct tt__FloatRange);    
    	PTZConfiguration->PanTiltLimits->Range->YRange->Max= 1.0;
    	PTZConfiguration->PanTiltLimits->Range->YRange->Min= - 1.0;	
    
    	//############################################################
    	ALLOC_STRUCT(PTZConfiguration->ZoomLimits, struct tt__ZoomLimits);  
    	ALLOC_STRUCT(PTZConfiguration->ZoomLimits->Range, struct tt__Space1DDescription);
    	ALLOC_TOKEN(PTZConfiguration->ZoomLimits->Range->URI, "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace");
 	ALLOC_STRUCT(PTZConfiguration->ZoomLimits->Range->XRange, struct tt__FloatRange);
      	PTZConfiguration->ZoomLimits->Range->XRange->Max= 1.0;
    	PTZConfiguration->ZoomLimits->Range->XRange->Min= 0.0;

	Profile->PTZConfiguration = PTZConfiguration;
	/* MetadataConfiguration */
	Profile->MetadataConfiguration = NULL;	

	//ALLOC_STRUCT(Profile->Extension, struct tt__ProfileExtension);
	//ALLOC_STRUCT(Profile->Extension->AudioOutputConfiguration, struct tt__ProfileExtension);	

	trt__GetProfileResponse->Profile = Profile;
	__FUN_END("\n");
	return SOAP_OK;
}


/** 
 * @brief Get Profiles
 * @Any endpoint can ask for the existing media profiles of an NVT using the GetProfiles command.
 *   
 * @param Request	:_trt__GetProfile structure contains an empty message.      
 * @param Response	:_trt__GetProfileResponse structure contains contains a configuration of all available profiles.
 * @return               : On success - SOAP_OK.
 On failure - No command specific faults!

 */
int __trt__GetProfiles(struct soap *soap, struct _trt__GetProfiles *trt__GetProfiles, struct _trt__GetProfilesResponse *trt__GetProfilesResponse)
{
	int idx = 0;
	struct tt__Profile *Profile, *ProfileArray;
	struct tt__VideoSourceConfiguration *VideoSourceConfiguration;
	struct tt__VideoEncoderConfiguration *VideoEncoderConfiguration;
	struct tt__AudioSourceConfiguration *AudioSourceConfiguration;
	struct tt__AudioEncoderConfiguration *AudioEncoderConfiguration;
	struct tt__PTZConfiguration *PTZConfiguration;
	
	HI_ONVIF_PROFILE_S OnvifProfile;
	HI_ONVIF_VIDEO_SOURCE_CFG_S OnvifVideoSourceCfg;
	HI_ONVIF_VIDEO_ENCODER_CFG_S OnvifVideoEncoderCfg;
	HI_ONVIF_AUDIO_SOURCE_CFG_S OnvifAudioSourceCfg;
	HI_ONVIF_AUDIO_ENCODER_CFG_S OnvifAudioEncoderCfg;
	NVR_DEV_VIDEO_CFG_S VideoCfg;
	unsigned long ip;
	
	ip = onvif_get_ipaddr(soap);
	
	__FUN_BEGIN("\n");
	
	//printf("^^^^^^^^^^^^^^trt__GetProfile->ProfileToken = %s\r\n", trt__GetProfiles->ProfileToken);
	ALLOC_STRUCT_NUM(ProfileArray, struct tt__Profile, ONVIF_PROFILE_NUM);
	for(idx = 0; idx < ONVIF_PROFILE_NUM; idx++)
	{	
		int ret;
		int chn = (idx>>1);
		int streamNo = idx%2;
		IPNC_DEV_VENC_CFG_S stVencCfg;
		memset(&stVencCfg, 0, sizeof(IPNC_DEV_VENC_CFG_S));
		if (streamNo = 0)
			nvr_get_ipnc_param(chn, IPNC_CMD_VMAIN_CFG, &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S));
		else
			nvr_get_ipnc_param(chn, IPNC_CMD_VSUB_CFG, &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S));
		onvif_get_profile(idx, ip, &OnvifProfile);
		Profile = &ProfileArray[idx];
		ALLOC_TOKEN(Profile->Name, OnvifProfile.Name);	
		ALLOC_TOKEN(Profile->token, OnvifProfile.token);
		ALLOC_STRUCT(Profile->fixed, enum xsd__boolean_);
		*Profile->fixed = OnvifProfile.fixed;	// 这个profile不能删除		

		/* VideoSourceConfiguration */
		onvif_get_video_source_cfg(OnvifProfile.videoSourceCfgIdx, &OnvifVideoSourceCfg);
		ALLOC_STRUCT(VideoSourceConfiguration,  struct tt__VideoSourceConfiguration);
		ALLOC_TOKEN(VideoSourceConfiguration->token, OnvifVideoSourceCfg.token);
		ALLOC_TOKEN(VideoSourceConfiguration->Name, OnvifVideoSourceCfg.Name);
		ALLOC_TOKEN(VideoSourceConfiguration->SourceToken, OnvifVideoSourceCfg.SourceToken);
		VideoSourceConfiguration->UseCount = OnvifVideoSourceCfg.UseCount;
		ALLOC_STRUCT(VideoSourceConfiguration->Bounds, struct tt__IntRectangle);	
		VideoSourceConfiguration->Bounds->y = OnvifVideoSourceCfg.Bounds.y;
		VideoSourceConfiguration->Bounds->x = OnvifVideoSourceCfg.Bounds.x;
		VideoSourceConfiguration->Bounds->width  = stVencCfg.u16Width;//OnvifVideoSourceCfg.Bounds.width;
		VideoSourceConfiguration->Bounds->height = stVencCfg.u16Height;//OnvifVideoSourceCfg.Bounds.height;
		Profile->VideoSourceConfiguration = VideoSourceConfiguration;
		
		/* AudioSourceConfiguration */
		onvif_get_audio_source_cfg(OnvifProfile.audioSourceCfgIdx, &OnvifAudioSourceCfg);
		ALLOC_STRUCT(AudioSourceConfiguration, struct tt__AudioSourceConfiguration);
		ALLOC_TOKEN(AudioSourceConfiguration->Name, OnvifAudioSourceCfg.Name);    		
		ALLOC_TOKEN(AudioSourceConfiguration->token, OnvifAudioSourceCfg.token);
		ALLOC_TOKEN(AudioSourceConfiguration->SourceToken, OnvifAudioSourceCfg.SourceToken);
		AudioSourceConfiguration->UseCount = OnvifAudioSourceCfg.UseCount;
		Profile->AudioSourceConfiguration = AudioSourceConfiguration;
    		
		/*VideoEncoderConfiguration */
		onvif_get_video_encoder_cfg(OnvifProfile.videoEncoderCfgIdx, &OnvifVideoEncoderCfg);
		ALLOC_STRUCT(VideoEncoderConfiguration, struct tt__VideoEncoderConfiguration);	
		printf("OnvifProfile.videoEncoderCfgIdx = %d\r\n", OnvifProfile.videoEncoderCfgIdx);
		ALLOC_TOKEN(VideoEncoderConfiguration->Name, OnvifVideoEncoderCfg.Name);
		ALLOC_TOKEN(VideoEncoderConfiguration->token, OnvifVideoEncoderCfg.token);
		VideoEncoderConfiguration->UseCount = OnvifVideoEncoderCfg.UseCount;	
		VideoEncoderConfiguration->Encoding = OnvifVideoEncoderCfg.Encoding;
		ALLOC_STRUCT(VideoEncoderConfiguration->Resolution, struct tt__VideoResolution);		
		VideoEncoderConfiguration->Resolution->Width = stVencCfg.u16Width;//OnvifVideoEncoderCfg.Resolution.Width;
		VideoEncoderConfiguration->Resolution->Height = stVencCfg.u16Height;//OnvifVideoEncoderCfg.Resolution.Height;
		VideoEncoderConfiguration->Quality = OnvifVideoEncoderCfg.Quality; 
		ALLOC_STRUCT(VideoEncoderConfiguration->RateControl, struct tt__VideoRateControl);
		VideoEncoderConfiguration->RateControl->FrameRateLimit =  OnvifVideoEncoderCfg.FrameRate;   
		VideoEncoderConfiguration->RateControl->EncodingInterval = OnvifVideoEncoderCfg.Gop;
		VideoEncoderConfiguration->RateControl->BitrateLimit =  OnvifVideoEncoderCfg.Bitrate;
		
		VideoEncoderConfiguration->MPEG4 = NULL;
		
		//nvr_logon_get_video_cfg(0, &VideoCfg);
		ALLOC_STRUCT(VideoEncoderConfiguration->H264, struct tt__H264Configuration);
		if(idx == 0)
		{
			VideoEncoderConfiguration->H264->GovLength = VideoCfg.struMainVenc.u8Gop;
		}else
		{
			VideoEncoderConfiguration->H264->GovLength = VideoCfg.struSubVenc.u8Gop;
		}
		switch(VideoCfg.eEncodeLevel)
		{
		case H264_HIGH:
			VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__High;
			break;
		case H264_MAIN:
			VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Main;
			break;
		case H264_BASELINE:
			VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Baseline;
			break;			
		}
		ALLOC_STRUCT(VideoEncoderConfiguration->Multicast, struct tt__MulticastConfiguration);
		ALLOC_STRUCT(VideoEncoderConfiguration->Multicast->Address, struct tt__IPAddress);
		VideoEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
		ALLOC_TOKEN(VideoEncoderConfiguration->Multicast->Address->IPv4Address, ONVIF_WSD_MC_ADDR);
		
		VideoEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
		VideoEncoderConfiguration->Multicast->Port = 9090;
		VideoEncoderConfiguration->Multicast->TTL = 1500;
		VideoEncoderConfiguration->Multicast->AutoStart = xsd__boolean__false_;

		ALLOC_TOKEN(VideoEncoderConfiguration->SessionTimeout, "PT5S");	
		Profile->VideoEncoderConfiguration = VideoEncoderConfiguration;

		/* AudioEncoderConfiguration */
    
 		onvif_get_audio_encoder_cfg(OnvifProfile.audioEncoderCfgIdx, &OnvifAudioEncoderCfg);
		ALLOC_STRUCT(AudioEncoderConfiguration, struct tt__AudioEncoderConfiguration)
		ALLOC_TOKEN(AudioEncoderConfiguration->Name, OnvifAudioEncoderCfg.Name);
		AudioEncoderConfiguration->UseCount = OnvifAudioEncoderCfg.UseCount;
		ALLOC_TOKEN(AudioEncoderConfiguration->token, OnvifAudioEncoderCfg.token);
		AudioEncoderConfiguration->Encoding = OnvifAudioEncoderCfg.Encoding;
		AudioEncoderConfiguration->Bitrate = OnvifAudioEncoderCfg.Bitrate;
		AudioEncoderConfiguration->SampleRate = OnvifAudioEncoderCfg.SampleRate;
		ALLOC_STRUCT(AudioEncoderConfiguration->Multicast, struct tt__MulticastConfiguration);
		ALLOC_STRUCT(AudioEncoderConfiguration->Multicast->Address, struct tt__IPAddress);
		AudioEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
		ALLOC_TOKEN(AudioEncoderConfiguration->Multicast->Address->IPv4Address, ONVIF_WSD_MC_ADDR);
		AudioEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
		AudioEncoderConfiguration->Multicast->Port = 9090;
		AudioEncoderConfiguration->Multicast->TTL = 1500;
		AudioEncoderConfiguration->Multicast->AutoStart = xsd__boolean__false_;	
		ALLOC_TOKEN(AudioEncoderConfiguration->SessionTimeout, "PT15S");		
		Profile->AudioEncoderConfiguration = AudioEncoderConfiguration;
		/* VideoAnalyticsConfiguration */
		Profile->VideoAnalyticsConfiguration = NULL;
		/* PTZConfiguration */	  
		ALLOC_STRUCT(PTZConfiguration, struct tt__PTZConfiguration);	
		ALLOC_TOKEN(PTZConfiguration->Name, ONVIF_PTZ_NAME);   
    		PTZConfiguration->UseCount = 1;
    		ALLOC_TOKEN(PTZConfiguration->token, ONVIF_PTZ_TOKEN);   
    		ALLOC_TOKEN(PTZConfiguration->token, ONVIF_PTZ_TOKEN);  
    		ALLOC_TOKEN(PTZConfiguration->NodeToken, ONVIF_PTZ_TOKEN);
		ALLOC_TOKEN(PTZConfiguration->DefaultAbsolutePantTiltPositionSpace, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
    		ALLOC_TOKEN(PTZConfiguration->DefaultAbsoluteZoomPositionSpace ,"http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace");
    		ALLOC_TOKEN(PTZConfiguration->DefaultRelativePanTiltTranslationSpace,  "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace");
    		ALLOC_TOKEN(PTZConfiguration->DefaultRelativeZoomTranslationSpace,  "http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace ");
    		ALLOC_TOKEN(PTZConfiguration->DefaultContinuousPanTiltVelocitySpace,  "http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace ");
    		ALLOC_TOKEN(PTZConfiguration->DefaultContinuousZoomVelocitySpace,  "http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace ");
		ALLOC_STRUCT(PTZConfiguration->DefaultPTZSpeed, struct tt__PTZSpeed);
    		ALLOC_STRUCT(PTZConfiguration->DefaultPTZSpeed->PanTilt, struct tt__Vector2D);    		   
    		ALLOC_TOKEN(PTZConfiguration->DefaultPTZSpeed->PanTilt->space, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace ");
    		PTZConfiguration->DefaultPTZSpeed->PanTilt->x = 1.0;
    		PTZConfiguration->DefaultPTZSpeed->PanTilt->y =0.5;
		ALLOC_STRUCT(PTZConfiguration->DefaultPTZSpeed->Zoom, struct tt__Vector1D);    	
    		ALLOC_TOKEN(PTZConfiguration->DefaultPTZSpeed->Zoom->space, "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace ");
    		PTZConfiguration->DefaultPTZSpeed->Zoom->x = 0.0;
    		ALLOC_TOKEN(PTZConfiguration->DefaultPTZTimeout, "PT5S");    

	    	//#####################################################
	    	ALLOC_STRUCT(PTZConfiguration->PanTiltLimits, struct tt__PanTiltLimits);
	 	ALLOC_STRUCT(PTZConfiguration->PanTiltLimits->Range, struct tt__Space2DDescription);
	   	ALLOC_TOKEN(PTZConfiguration->PanTiltLimits->Range->URI, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
	    	ALLOC_STRUCT(PTZConfiguration->PanTiltLimits->Range->XRange, struct tt__FloatRange);    	
	    	PTZConfiguration->PanTiltLimits->Range->XRange->Max= 1.0;
	    	PTZConfiguration->PanTiltLimits->Range->XRange->Min= - 1.0;
	    	ALLOC_STRUCT(PTZConfiguration->PanTiltLimits->Range->YRange, struct tt__FloatRange);    
	    	PTZConfiguration->PanTiltLimits->Range->YRange->Max= 1.0;
	    	PTZConfiguration->PanTiltLimits->Range->YRange->Min= - 1.0;	
	    
	    	//############################################################
	    	ALLOC_STRUCT(PTZConfiguration->ZoomLimits, struct tt__ZoomLimits);  
	    	ALLOC_STRUCT(PTZConfiguration->ZoomLimits->Range, struct tt__Space1DDescription);
	    	ALLOC_TOKEN(PTZConfiguration->ZoomLimits->Range->URI, "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace");
	 	ALLOC_STRUCT(PTZConfiguration->ZoomLimits->Range->XRange, struct tt__FloatRange);
	      	PTZConfiguration->ZoomLimits->Range->XRange->Max= 1.0;
	    	PTZConfiguration->ZoomLimits->Range->XRange->Min= 0.0;
	
		Profile->PTZConfiguration = PTZConfiguration;
		/* MetadataConfiguration */
		Profile->MetadataConfiguration = NULL;	
	}
		
	trt__GetProfilesResponse->__sizeProfiles = ONVIF_PROFILE_NUM;
	trt__GetProfilesResponse->Profiles = ProfileArray;
	__FUN_END("\n");

	return SOAP_OK;
}

 int  __trt__DeleteProfile(struct soap* soap, struct _trt__DeleteProfile *trt__DeleteProfile, struct _trt__DeleteProfileResponse *trt__DeleteProfileResponse) 
{
	//onvif_fault(soap, "ter:Action", "ter:DeletionOfFixedProfile");
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_OK;
}

 int  __trt__AddVideoEncoderConfiguration(struct soap* soap, struct _trt__AddVideoEncoderConfiguration *trt__AddVideoEncoderConfiguration, struct _trt__AddVideoEncoderConfigurationResponse *trt__AddVideoEncoderConfigurationResponse) 
{
	onvif_fault(soap, "ter:Action", "ter:ConfigurationConflict");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__AddVideoSourceConfiguration(struct soap* soap, struct _trt__AddVideoSourceConfiguration *trt__AddVideoSourceConfiguration, struct _trt__AddVideoSourceConfigurationResponse *trt__AddVideoSourceConfigurationResponse) 
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

int __trt__AddAudioEncoderConfiguration(struct soap *soap, struct _trt__AddAudioEncoderConfiguration *trt__AddAudioEncoderConfiguration, struct _trt__AddAudioEncoderConfigurationResponse *trt__AddAudioEncoderConfigurationResponse)
{	
	//onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioNotSupported"); 
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_OK;
}

 int  __trt__AddAudioSourceConfiguration(struct soap* soap, struct _trt__AddAudioSourceConfiguration *trt__AddAudioSourceConfiguration, struct _trt__AddAudioSourceConfigurationResponse *trt__AddAudioSourceConfigurationResponse) 
{
	//onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioNotSupported"); 
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_OK;
}

int __trt__AddPTZConfiguration(struct soap *soap, struct _trt__AddPTZConfiguration *trt__AddPTZConfiguration, struct _trt__AddPTZConfigurationResponse *trt__AddPTZConfigurationResponse)
{	
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_OK;
}

 int  __trt__AddVideoAnalyticsConfiguration(struct soap* soap, struct _trt__AddVideoAnalyticsConfiguration *trt__AddVideoAnalyticsConfiguration, struct _trt__AddVideoAnalyticsConfigurationResponse *trt__AddVideoAnalyticsConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:VideoAnalyticsNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__AddMetadataConfiguration(struct soap* soap, struct _trt__AddMetadataConfiguration *trt__AddMetadataConfiguration, struct _trt__AddMetadataConfigurationResponse *trt__AddMetadataConfigurationResponse) 
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __trt__AddAudioOutputConfiguration(struct soap* soap, struct _trt__AddAudioOutputConfiguration *trt__AddAudioOutputConfiguration, struct _trt__AddAudioOutputConfigurationResponse *trt__AddAudioOutputConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioIn/OutNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;	
}

 int __trt__AddAudioDecoderConfiguration(struct soap *soap, struct _trt__AddAudioDecoderConfiguration *trt__AddAudioDecoderConfiguration, struct _trt__AddAudioDecoderConfigurationResponse *trt__AddAudioDecoderConfigurationResponse)
{	
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioIn/OutNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__RemoveVideoEncoderConfiguration(struct soap* soap, struct _trt__RemoveVideoEncoderConfiguration *trt__RemoveVideoEncoderConfiguration, struct _trt__RemoveVideoEncoderConfigurationResponse *trt__RemoveVideoEncoderConfigurationResponse) 
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __trt__RemoveVideoSourceConfiguration(struct soap* soap, struct _trt__RemoveVideoSourceConfiguration *trt__RemoveVideoSourceConfiguration, struct _trt__RemoveVideoSourceConfigurationResponse *trt__RemoveVideoSourceConfigurationResponse) 
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __trt__RemoveAudioEncoderConfiguration(struct soap* soap, struct _trt__RemoveAudioEncoderConfiguration *trt__RemoveAudioEncoderConfiguration, struct _trt__RemoveAudioEncoderConfigurationResponse *trt__RemoveAudioEncoderConfigurationResponse) 
{
	//onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioNotSupported"); 
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_OK;
}

 int  __trt__RemoveAudioSourceConfiguration(struct soap* soap, struct _trt__RemoveAudioSourceConfiguration *trt__RemoveAudioSourceConfiguration, struct _trt__RemoveAudioSourceConfigurationResponse *trt__RemoveAudioSourceConfigurationResponse) 
{
	//onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioNotSupported"); 
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_OK;
}

 int  __trt__RemovePTZConfiguration(struct soap* soap, struct _trt__RemovePTZConfiguration *trt__RemovePTZConfiguration, struct _trt__RemovePTZConfigurationResponse *trt__RemovePTZConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:PTZNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__RemoveVideoAnalyticsConfiguration(struct soap* soap, struct _trt__RemoveVideoAnalyticsConfiguration *trt__RemoveVideoAnalyticsConfiguration, struct _trt__RemoveVideoAnalyticsConfigurationResponse *trt__RemoveVideoAnalyticsConfigurationResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoConfig"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__RemoveMetadataConfiguration(struct soap* soap, struct _trt__RemoveMetadataConfiguration *trt__RemoveMetadataConfiguration, struct _trt__RemoveMetadataConfigurationResponse *trt__RemoveMetadataConfigurationResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoConfig"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__RemoveAudioOutputConfiguration(struct soap* soap, struct _trt__RemoveAudioOutputConfiguration *trt__RemoveAudioOutputConfiguration, struct _trt__RemoveAudioOutputConfigurationResponse *trt__RemoveAudioOutputConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioIn/OutNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__RemoveAudioDecoderConfiguration(struct soap* soap, struct _trt__RemoveAudioDecoderConfiguration *trt__RemoveAudioDecoderConfiguration, struct _trt__RemoveAudioDecoderConfigurationResponse *trt__RemoveAudioDecoderConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioIn/OutNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__GetVideoSourceConfigurations(struct soap* soap, struct _trt__GetVideoSourceConfigurations *trt__GetVideoSourceConfigurations, struct _trt__GetVideoSourceConfigurationsResponse *trt__GetVideoSourceConfigurationsResponse) 
{
	int idx;
	HI_ONVIF_VIDEO_SOURCE_CFG_S OnvifVideoSourceCfg;
	struct tt__VideoSourceConfiguration *VideoSourceConfiguration, *VideoSourceConfigurationArray;
	__FUN_BEGIN("\n");
	
	ALLOC_STRUCT_NUM(VideoSourceConfigurationArray,  struct tt__VideoSourceConfiguration, ONVIF_VIDEO_SOURCE_CFG_NUM);
	for(idx = 0; idx < ONVIF_VIDEO_SOURCE_CFG_NUM; idx++)
	{
		VideoSourceConfiguration = &VideoSourceConfigurationArray[idx];
		/* VideoSourceConfiguration */
		onvif_get_video_source_cfg(idx, &OnvifVideoSourceCfg);	
		ALLOC_TOKEN(VideoSourceConfiguration->Name, OnvifVideoSourceCfg.Name);
		ALLOC_TOKEN(VideoSourceConfiguration->SourceToken, OnvifVideoSourceCfg.SourceToken);
		VideoSourceConfiguration->UseCount = OnvifVideoSourceCfg.UseCount;
		ALLOC_STRUCT(VideoSourceConfiguration->Bounds, struct tt__IntRectangle);	
		VideoSourceConfiguration->Bounds->y = OnvifVideoSourceCfg.Bounds.y;
		VideoSourceConfiguration->Bounds->x = OnvifVideoSourceCfg.Bounds.x;
		VideoSourceConfiguration->Bounds->width  = OnvifVideoSourceCfg.Bounds.width;
		VideoSourceConfiguration->Bounds->height = OnvifVideoSourceCfg.Bounds.height;	
	}

	trt__GetVideoSourceConfigurationsResponse->__sizeConfigurations = ONVIF_VIDEO_SOURCE_CFG_NUM;
	trt__GetVideoSourceConfigurationsResponse->Configurations = VideoSourceConfigurationArray;
		__FUN_END("\n");
	return SOAP_OK;
}

 int  __trt__GetVideoEncoderConfigurations(struct soap* soap, struct _trt__GetVideoEncoderConfigurations *trt__GetVideoEncoderConfigurations, struct _trt__GetVideoEncoderConfigurationsResponse *trt__GetVideoEncoderConfigurationsResponse) 
{
	int idx;
	HI_ONVIF_VIDEO_ENCODER_CFG_S OnvifVideoEncoderCfg;;
	NVR_DEV_VIDEO_CFG_S VideoCfg;
	struct tt__VideoEncoderConfiguration *VideoEncoderConfiguration, *VideoEncoderConfigurationArray;
	__FUN_BEGIN("\n");
	
	ALLOC_STRUCT_NUM(VideoEncoderConfigurationArray, struct tt__VideoEncoderConfiguration, ONVIF_VIDEO_ENCODER_CFG_NUM);
	for(idx = 0; idx < ONVIF_VIDEO_ENCODER_CFG_NUM; idx++)
	{
		onvif_get_video_encoder_cfg(idx, &OnvifVideoEncoderCfg);	
		
		/*VideoEncoderConfiguration */
		VideoEncoderConfiguration = &VideoEncoderConfigurationArray[idx];
		//ALLOC_STRUCT(VideoEncoderConfiguration, struct tt__VideoEncoderConfiguration);	
		ALLOC_TOKEN(VideoEncoderConfiguration->Name, OnvifVideoEncoderCfg.Name);
		ALLOC_TOKEN(VideoEncoderConfiguration->token, OnvifVideoEncoderCfg.token);
		VideoEncoderConfiguration->UseCount = OnvifVideoEncoderCfg.UseCount;	
		VideoEncoderConfiguration->Encoding = OnvifVideoEncoderCfg.Encoding;
		ALLOC_STRUCT(VideoEncoderConfiguration->Resolution, struct tt__VideoResolution);		
		VideoEncoderConfiguration->Resolution->Width = OnvifVideoEncoderCfg.Resolution.Width;
		VideoEncoderConfiguration->Resolution->Height = OnvifVideoEncoderCfg.Resolution.Height;
		VideoEncoderConfiguration->Quality = OnvifVideoEncoderCfg.Quality; 
		ALLOC_STRUCT(VideoEncoderConfiguration->RateControl, struct tt__VideoRateControl);
		VideoEncoderConfiguration->RateControl->FrameRateLimit =  OnvifVideoEncoderCfg.FrameRate;   
		VideoEncoderConfiguration->RateControl->EncodingInterval = OnvifVideoEncoderCfg.Gop;
		VideoEncoderConfiguration->RateControl->BitrateLimit =  OnvifVideoEncoderCfg.Bitrate;
	
		VideoEncoderConfiguration->MPEG4 = NULL;

		//nvr_logon_get_video_cfg(0, &VideoCfg);
		ALLOC_STRUCT(VideoEncoderConfiguration->H264, struct tt__H264Configuration);
		if(idx == 0)
		{
			VideoEncoderConfiguration->H264->GovLength = VideoCfg.struMainVenc.u8Gop;
		}else
		{
			VideoEncoderConfiguration->H264->GovLength = VideoCfg.struSubVenc.u8Gop;
		}
		switch(VideoCfg.eEncodeLevel)
		{
		case H264_HIGH:
			VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__High;
			break;
		case H264_MAIN:
			VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Main;
			break;
		case H264_BASELINE:
			VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Baseline;
			break;			
		}
		
		ALLOC_STRUCT(VideoEncoderConfiguration->Multicast, struct tt__MulticastConfiguration);
		ALLOC_STRUCT(VideoEncoderConfiguration->Multicast->Address, struct tt__IPAddress);
		VideoEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
		ALLOC_TOKEN(VideoEncoderConfiguration->Multicast->Address->IPv4Address, ONVIF_WSD_MC_ADDR);
		
		VideoEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
		VideoEncoderConfiguration->Multicast->Port = 9090;
		VideoEncoderConfiguration->Multicast->TTL = 1500;
		VideoEncoderConfiguration->Multicast->AutoStart = xsd__boolean__false_;

		ALLOC_TOKEN(VideoEncoderConfiguration->SessionTimeout, "PT5S");	

	}

	trt__GetVideoEncoderConfigurationsResponse->__sizeConfigurations = ONVIF_VIDEO_ENCODER_CFG_NUM;
	trt__GetVideoEncoderConfigurationsResponse->Configurations = VideoEncoderConfigurationArray;
	__FUN_END("\n");

	return SOAP_OK;
}

 int  __trt__GetAudioSourceConfigurations(struct soap* soap, struct _trt__GetAudioSourceConfigurations *trt__GetAudioSourceConfigurations, struct _trt__GetAudioSourceConfigurationsResponse *trt__GetAudioSourceConfigurationsResponse) 
{
	int idx;
	HI_ONVIF_AUDIO_SOURCE_CFG_S OnvifAudioSourceCfg;
	struct tt__AudioSourceConfiguration *AudioSourceConfiguration, *AudioSourceConfigurationArray;
	
	__FUN_BEGIN("\n");
	ALLOC_STRUCT_NUM(AudioSourceConfigurationArray, struct tt__AudioSourceConfiguration, ONVIF_AUDIO_SOURCE_CFG_NUM);
	for(idx = 0; idx < ONVIF_AUDIO_SOURCE_CFG_NUM; idx++)
	{			
		/* AudioSourceConfiguration */
		onvif_get_audio_source_cfg(idx, &OnvifAudioSourceCfg);
		AudioSourceConfiguration = &AudioSourceConfigurationArray[idx];
		ALLOC_TOKEN(AudioSourceConfiguration->Name, OnvifAudioSourceCfg.Name);    		
		ALLOC_TOKEN(AudioSourceConfiguration->token, OnvifAudioSourceCfg.token);
		ALLOC_TOKEN(AudioSourceConfiguration->SourceToken, OnvifAudioSourceCfg.SourceToken);
		AudioSourceConfiguration->UseCount = OnvifAudioSourceCfg.UseCount;
	}

	trt__GetAudioSourceConfigurationsResponse->__sizeConfigurations = ONVIF_AUDIO_SOURCE_CFG_NUM;
	trt__GetAudioSourceConfigurationsResponse->Configurations = AudioSourceConfigurationArray;
	__FUN_END("\n");
	return SOAP_OK;
	//onvif_fault(soap, "ter:ActionNotSupported", "ter:AudioNotSupported");
	// SOAP_FAULT;
}

 int  __trt__GetAudioEncoderConfigurations(struct soap* soap, struct _trt__GetAudioEncoderConfigurations *trt__GetAudioEncoderConfigurations, struct _trt__GetAudioEncoderConfigurationsResponse *trt__GetAudioEncoderConfigurationsResponse) 
{
	int idx;
	HI_ONVIF_AUDIO_ENCODER_CFG_S OnvifAudioEncoderCfg;;
	struct tt__AudioEncoderConfiguration *AudioEncoderConfiguration, *AudioEncoderConfigurationArray;
	__FUN_BEGIN("\n");
	ALLOC_STRUCT_NUM(AudioEncoderConfigurationArray, struct tt__AudioEncoderConfiguration, ONVIF_AUDIO_SOURCE_CFG_NUM);
	for(idx = 0; idx < ONVIF_AUDIO_SOURCE_CFG_NUM; idx++)
	{
		onvif_get_audio_encoder_cfg(idx, &OnvifAudioEncoderCfg);
		AudioEncoderConfiguration = &AudioEncoderConfigurationArray[idx];		
		ALLOC_TOKEN(AudioEncoderConfiguration->Name, OnvifAudioEncoderCfg.Name);
		AudioEncoderConfiguration->UseCount = OnvifAudioEncoderCfg.UseCount;
		ALLOC_TOKEN(AudioEncoderConfiguration->token, OnvifAudioEncoderCfg.token);
		AudioEncoderConfiguration->Encoding = OnvifAudioEncoderCfg.Encoding;
		AudioEncoderConfiguration->Bitrate = OnvifAudioEncoderCfg.Bitrate;
		AudioEncoderConfiguration->SampleRate = OnvifAudioEncoderCfg.SampleRate;
		ALLOC_STRUCT(AudioEncoderConfiguration->Multicast, struct tt__MulticastConfiguration);
		ALLOC_STRUCT(AudioEncoderConfiguration->Multicast->Address, struct tt__IPAddress);
		AudioEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
		ALLOC_TOKEN(AudioEncoderConfiguration->Multicast->Address->IPv4Address, ONVIF_WSD_MC_ADDR);
		AudioEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
		AudioEncoderConfiguration->Multicast->Port = 9090;
		AudioEncoderConfiguration->Multicast->TTL = 1500;
		AudioEncoderConfiguration->Multicast->AutoStart = xsd__boolean__false_; 
		ALLOC_TOKEN(AudioEncoderConfiguration->SessionTimeout, "PT15S");			
		
	}

	trt__GetAudioEncoderConfigurationsResponse->__sizeConfigurations = ONVIF_AUDIO_ENCODER_CFG_NUM;
	trt__GetAudioEncoderConfigurationsResponse->Configurations = AudioEncoderConfigurationArray;
	__FUN_END("\n");

	return SOAP_OK;
	//onvif_fault(soap, "ter:ActionNotSupported", "ter:AudioNotSupported");
	//return SOAP_FAULT;
}
/** 
 * @brief Get Video Analytics Configurations
 * @This operation lists all video analytics configurations of a device.
 *   
 * @param Request	:_trt__GetVideoAnalyticsConfigurations structure contains message is empty.
 * @param Response	:_trt__GetVideoAnalyticsConfigurationsResponse structure contains a list of all existing video analytics configurations in the device.
 * @return               : On success - SOAP_OK.
 : On failure - Device does not support video analytics.
 */
int __trt__GetVideoAnalyticsConfigurations(struct soap *soap, struct _trt__GetVideoAnalyticsConfigurations *trt__GetVideoAnalyticsConfigurations, struct _trt__GetVideoAnalyticsConfigurationsResponse *trt__GetVideoAnalyticsConfigurationsResponse)
{
	onvif_fault(soap, "ter:ActionNotSupported", "ter:VideoAnalyticsNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__GetMetadataConfigurations(struct soap* soap, struct _trt__GetMetadataConfigurations *trt__GetMetadataConfigurations, struct _trt__GetMetadataConfigurationsResponse *trt__GetMetadataConfigurationsResponse) 
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __trt__GetAudioOutputConfigurations(struct soap* soap, struct _trt__GetAudioOutputConfigurations *trt__GetAudioOutputConfigurations, struct _trt__GetAudioOutputConfigurationsResponse *trt__GetAudioOutputConfigurationsResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioIn/OutNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__GetAudioDecoderConfigurations(struct soap* soap, struct _trt__GetAudioDecoderConfigurations *trt__GetAudioDecoderConfigurations, struct _trt__GetAudioDecoderConfigurationsResponse *trt__GetAudioDecoderConfigurationsResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioIn/OutNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__GetVideoSourceConfiguration(struct soap* soap, struct _trt__GetVideoSourceConfiguration *trt__GetVideoSourceConfiguration, struct _trt__GetVideoSourceConfigurationResponse *trt__GetVideoSourceConfigurationResponse) 
{
	HI_ONVIF_VIDEO_SOURCE_CFG_S OnvifVideoSourceCfg;
	struct tt__VideoSourceConfiguration *VideoSourceConfiguration;
	__FUN_BEGIN("\n");	
	if(onvif_get_video_source_cfg_from_token(trt__GetVideoSourceConfiguration->ConfigurationToken, &OnvifVideoSourceCfg)  < 0)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoConfig");
		return SOAP_FAULT;
	}	
	ALLOC_STRUCT(VideoSourceConfiguration,  struct tt__VideoSourceConfiguration);
	ALLOC_TOKEN(VideoSourceConfiguration->Name, OnvifVideoSourceCfg.Name);
	ALLOC_TOKEN(VideoSourceConfiguration->SourceToken, OnvifVideoSourceCfg.SourceToken);
	VideoSourceConfiguration->UseCount = OnvifVideoSourceCfg.UseCount;
	ALLOC_STRUCT(VideoSourceConfiguration->Bounds, struct tt__IntRectangle);	
	VideoSourceConfiguration->Bounds->y = OnvifVideoSourceCfg.Bounds.y;
	VideoSourceConfiguration->Bounds->x = OnvifVideoSourceCfg.Bounds.x;
	VideoSourceConfiguration->Bounds->width  = OnvifVideoSourceCfg.Bounds.width;
	VideoSourceConfiguration->Bounds->height = OnvifVideoSourceCfg.Bounds.height;

	trt__GetVideoSourceConfigurationResponse->Configuration = VideoSourceConfiguration;
	__FUN_END("\n");
	return SOAP_OK;
}


/** 
 * @brief Get Video Encoder Configuration
 * @This operation lists all existing video encoder configurations of an NVT.
 *   
 * @param Request	:_trt__GetVideoEncoderConfiguration structure contains an empty message.
 * @param Response	:_trt__GetVideoEncoderConfigurationResponse structure contains a list of all existing video encoder configurations in the NVT. 
 * @return               : On success - SOAP_OK.
 On failure - The requested configuration indicated with ConfigurationToken does not exist.
 */
int __trt__GetVideoEncoderConfiguration(struct soap *soap, struct _trt__GetVideoEncoderConfiguration *trt__GetVideoEncoderConfiguration, struct _trt__GetVideoEncoderConfigurationResponse *trt__GetVideoEncoderConfigurationResponse)
{ 
	HI_ONVIF_VIDEO_ENCODER_CFG_S OnvifVideoEncoderCfg;
	NVR_DEV_VIDEO_CFG_S VideoCfg;
	struct tt__VideoEncoderConfiguration *VideoEncoderConfiguration;
	__FUN_BEGIN("\n");

	if(onvif_get_video_encoder_cfg_from_token(trt__GetVideoEncoderConfiguration->ConfigurationToken, &OnvifVideoEncoderCfg) < 0)
	{
		__E("Failed to get video encoder cfg.\n");
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoConfig");
		return SOAP_FAULT;
	}
	/*VideoEncoderConfiguration */	
	ALLOC_STRUCT(VideoEncoderConfiguration, struct tt__VideoEncoderConfiguration);	
	ALLOC_TOKEN(VideoEncoderConfiguration->Name, OnvifVideoEncoderCfg.Name);
	ALLOC_TOKEN(VideoEncoderConfiguration->token, OnvifVideoEncoderCfg.token);
	VideoEncoderConfiguration->UseCount = OnvifVideoEncoderCfg.UseCount;	
	VideoEncoderConfiguration->Encoding = OnvifVideoEncoderCfg.Encoding;
	ALLOC_STRUCT(VideoEncoderConfiguration->Resolution, struct tt__VideoResolution);		
	VideoEncoderConfiguration->Resolution->Width = OnvifVideoEncoderCfg.Resolution.Width;
	VideoEncoderConfiguration->Resolution->Height = OnvifVideoEncoderCfg.Resolution.Height;
	VideoEncoderConfiguration->Quality = OnvifVideoEncoderCfg.Quality; 
	ALLOC_STRUCT(VideoEncoderConfiguration->RateControl, struct tt__VideoRateControl);
	VideoEncoderConfiguration->RateControl->FrameRateLimit =  OnvifVideoEncoderCfg.FrameRate;   
	VideoEncoderConfiguration->RateControl->EncodingInterval = OnvifVideoEncoderCfg.Gop;
	VideoEncoderConfiguration->RateControl->BitrateLimit =  OnvifVideoEncoderCfg.Bitrate;
	
	VideoEncoderConfiguration->MPEG4 = NULL;

	//nvr_logon_get_video_cfg(0, &VideoCfg);
	ALLOC_STRUCT(VideoEncoderConfiguration->H264, struct tt__H264Configuration);
	
	VideoEncoderConfiguration->H264->GovLength = VideoCfg.struMainVenc.u8Gop;
	
	switch(VideoCfg.eEncodeLevel)
	{
	case H264_HIGH:
		VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__High;
		break;
	case H264_MAIN:
		VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Main;
		break;
	case H264_BASELINE:
		VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Baseline;
		break;			
	}
	ALLOC_STRUCT(VideoEncoderConfiguration->Multicast, struct tt__MulticastConfiguration);
	ALLOC_STRUCT(VideoEncoderConfiguration->Multicast->Address, struct tt__IPAddress);
	VideoEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
	ALLOC_TOKEN(VideoEncoderConfiguration->Multicast->Address->IPv4Address, ONVIF_WSD_MC_ADDR);
	
	VideoEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
	VideoEncoderConfiguration->Multicast->Port = 9090;
	VideoEncoderConfiguration->Multicast->TTL = 1500;
	VideoEncoderConfiguration->Multicast->AutoStart = xsd__boolean__false_;

	ALLOC_TOKEN(VideoEncoderConfiguration->SessionTimeout, "PT5S");		
	trt__GetVideoEncoderConfigurationResponse->Configuration = VideoEncoderConfiguration;
	__FUN_END("\n");

	return SOAP_OK;
}

 int  __trt__GetAudioSourceConfiguration(struct soap* soap, struct _trt__GetAudioSourceConfiguration *trt__GetAudioSourceConfiguration, struct _trt__GetAudioSourceConfigurationResponse *trt__GetAudioSourceConfigurationResponse) 
{
//	int idx;
	HI_ONVIF_AUDIO_SOURCE_CFG_S OnvifAudioSourceCfg;
	struct tt__AudioSourceConfiguration *AudioSourceConfiguration;
	
	__FUN_BEGIN("\n");

	if(onvif_get_audio_source_cfg_from_token(trt__GetAudioSourceConfiguration->ConfigurationToken, &OnvifAudioSourceCfg)  < 0)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoConfig");
		return SOAP_FAULT;
	}		
	
	ALLOC_STRUCT(AudioSourceConfiguration, struct tt__AudioSourceConfiguration);
	ALLOC_TOKEN(AudioSourceConfiguration->Name, OnvifAudioSourceCfg.Name);    		
	ALLOC_TOKEN(AudioSourceConfiguration->token, OnvifAudioSourceCfg.token);
	ALLOC_TOKEN(AudioSourceConfiguration->SourceToken, OnvifAudioSourceCfg.SourceToken);
	AudioSourceConfiguration->UseCount = OnvifAudioSourceCfg.UseCount;
	AudioSourceConfiguration->__size = 0;
	AudioSourceConfiguration->__any = NULL;
	AudioSourceConfiguration->__anyAttribute = NULL;		


	trt__GetAudioSourceConfigurationResponse->Configuration =AudioSourceConfiguration;
	__FUN_END("\n");
	return SOAP_OK;

	//onvif_fault(soap, "ter:ActionNotSupported", "ter:AudioNotSupported");
	//return SOAP_FAULT;
}

 int  __trt__GetAudioEncoderConfiguration(struct soap* soap, struct _trt__GetAudioEncoderConfiguration *trt__GetAudioEncoderConfiguration, struct _trt__GetAudioEncoderConfigurationResponse *trt__GetAudioEncoderConfigurationResponse) 
{
#if 0	
	HI_ONVIF_AUDIO_ENCODER_CFG_S OnvifAudioEncoderCfg;;
	NVR_DEV_AUDIO_CFG_S stAutoCfg;
	struct tt__AudioEncoderConfiguration *AudioEncoderConfiguration;
	__FUN_BEGIN("\n");
	
	if(onvif_get_audio_encoder_cfg_from_token(trt__GetAudioEncoderConfiguration->ConfigurationToken, &OnvifAudioEncoderCfg) < 0)
	{
		__E("Failed to get Audio encoder cfg.\n");
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoConfig");
		return SOAP_FAULT;
	}
		
	memset(&stAutoCfg, 0, sizeof(NVR_DEV_AUDIO_CFG_S));
	nvr_logon_get_auto_cfg(0, &stAutoCfg);
	stAutoCfg.u8Open = 1;
	nvr_logon_set_auto_cfg(0, &stAutoCfg);
	
	ALLOC_STRUCT(AudioEncoderConfiguration, struct tt__AudioEncoderConfiguration)
	ALLOC_TOKEN(AudioEncoderConfiguration->Name, OnvifAudioEncoderCfg.Name);
	AudioEncoderConfiguration->UseCount = OnvifAudioEncoderCfg.UseCount;
	ALLOC_TOKEN(AudioEncoderConfiguration->token, OnvifAudioEncoderCfg.token);
	AudioEncoderConfiguration->Encoding = OnvifAudioEncoderCfg.Encoding;
	AudioEncoderConfiguration->Bitrate = OnvifAudioEncoderCfg.Bitrate;
	AudioEncoderConfiguration->SampleRate = OnvifAudioEncoderCfg.SampleRate;
	ALLOC_STRUCT(AudioEncoderConfiguration->Multicast, struct tt__MulticastConfiguration);
	ALLOC_STRUCT(AudioEncoderConfiguration->Multicast->Address, struct tt__IPAddress);
	AudioEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
	ALLOC_TOKEN(AudioEncoderConfiguration->Multicast->Address->IPv4Address, ONVIF_WSD_MC_ADDR);
	AudioEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
	AudioEncoderConfiguration->Multicast->Port = 9090;
	AudioEncoderConfiguration->Multicast->TTL = 1500;
	AudioEncoderConfiguration->Multicast->AutoStart = xsd__boolean__false_;	
	ALLOC_TOKEN(AudioEncoderConfiguration->SessionTimeout, OnvifAudioEncoderCfg.SessionTimeout);			
		
		
	trt__GetAudioEncoderConfigurationResponse->Configuration = AudioEncoderConfiguration;
	__FUN_END("\n");
#endif	
	return SOAP_OK;
	
}

 int  __trt__GetVideoAnalyticsConfiguration(struct soap* soap, struct _trt__GetVideoAnalyticsConfiguration *trt__GetVideoAnalyticsConfiguration, struct _trt__GetVideoAnalyticsConfigurationResponse *trt__GetVideoAnalyticsConfigurationResponse) 
{
	onvif_fault(soap, "ter:ActionNotSupported", "ter:VideoAnalyticsNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__GetMetadataConfiguration(struct soap* soap, struct _trt__GetMetadataConfiguration *trt__GetMetadataConfiguration, struct _trt__GetMetadataConfigurationResponse *trt__GetMetadataConfigurationResponse) 
{
	onvif_fault(soap, "ter:InvalidArgVal", "ter:NoConfig");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__GetAudioOutputConfiguration(struct soap* soap, struct _trt__GetAudioOutputConfiguration *trt__GetAudioOutputConfiguration, struct _trt__GetAudioOutputConfigurationResponse *trt__GetAudioOutputConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioIn/OutNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__GetAudioDecoderConfiguration(struct soap* soap, struct _trt__GetAudioDecoderConfiguration *trt__GetAudioDecoderConfiguration, struct _trt__GetAudioDecoderConfigurationResponse *trt__GetAudioDecoderConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioIn/OutNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

/** 
 * @brief Get Compatible Video Encoder Configurations
 * @brief This operation lists all the video encoder configurations of the NVT that are compatible with a certain media profile.
 *   
 * @param Request	:_trt__GetCompatibleVideoEncoderConfigurations structure contains the token of an existing media profile.
 * @param Response	:_trt__GetCompatibleVideoEncoderConfigurationsResponse structure contains a list of video encoder configurations that are compatible with the given medi                            a profile.
 * @return               : On success -  SOAP_OK.
 : On failure - The requested profile token ProfileToken does not exist.
 */
int __trt__GetCompatibleVideoEncoderConfigurations(struct soap *soap, struct _trt__GetCompatibleVideoEncoderConfigurations *trt__GetCompatibleVideoEncoderConfigurations, struct _trt__GetCompatibleVideoEncoderConfigurationsResponse *trt__GetCompatibleVideoEncoderConfigurationsResponse)
{ 
	int idx;
	NVR_DEV_VIDEO_CFG_S VideoCfg;
	HI_ONVIF_VIDEO_ENCODER_CFG_S OnvifVideoEncoderCfg;
	struct tt__VideoEncoderConfiguration *VideoEncoderConfiguration, *VideoEncoderConfigurationArray;
	__FUN_BEGIN("\n");
	
	ALLOC_STRUCT(VideoEncoderConfigurationArray, struct tt__VideoEncoderConfiguration);	

	for(idx = 0; idx < ONVIF_VIDEO_ENCODER_CFG_NUM; idx++)
	{
		onvif_get_video_encoder_cfg(idx, &OnvifVideoEncoderCfg);
		VideoEncoderConfiguration = &VideoEncoderConfigurationArray[idx];
		ALLOC_TOKEN(VideoEncoderConfiguration->Name, OnvifVideoEncoderCfg.Name);
		ALLOC_TOKEN(VideoEncoderConfiguration->token, OnvifVideoEncoderCfg.token);
		VideoEncoderConfiguration->UseCount = OnvifVideoEncoderCfg.UseCount;	
		VideoEncoderConfiguration->Encoding = OnvifVideoEncoderCfg.Encoding;
		ALLOC_STRUCT(VideoEncoderConfiguration->Resolution, struct tt__VideoResolution);		
		VideoEncoderConfiguration->Resolution->Width = OnvifVideoEncoderCfg.Resolution.Width;
		VideoEncoderConfiguration->Resolution->Height = OnvifVideoEncoderCfg.Resolution.Height;
		VideoEncoderConfiguration->Quality = OnvifVideoEncoderCfg.Quality; 
		ALLOC_STRUCT(VideoEncoderConfiguration->RateControl, struct tt__VideoRateControl);
		VideoEncoderConfiguration->RateControl->FrameRateLimit =  OnvifVideoEncoderCfg.FrameRate;	
		VideoEncoderConfiguration->RateControl->EncodingInterval = OnvifVideoEncoderCfg.Gop;
		VideoEncoderConfiguration->RateControl->BitrateLimit =	OnvifVideoEncoderCfg.Bitrate;
		
		VideoEncoderConfiguration->MPEG4 = NULL;

		//nvr_logon_get_video_cfg(0, &VideoCfg);
		ALLOC_STRUCT(VideoEncoderConfiguration->H264, struct tt__H264Configuration);
		if(idx == 0)
		{
			VideoEncoderConfiguration->H264->GovLength = VideoCfg.struMainVenc.u8Gop;
		}else
		{
			VideoEncoderConfiguration->H264->GovLength = VideoCfg.struSubVenc.u8Gop;
		}
		switch(VideoCfg.eEncodeLevel)
		{
		case H264_HIGH:
			VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__High;
			break;
		case H264_MAIN:
			VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Main;
			break;
		case H264_BASELINE:
			VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Baseline;
			break;			
		}

		
		ALLOC_STRUCT(VideoEncoderConfiguration->Multicast, struct tt__MulticastConfiguration);
		ALLOC_STRUCT(VideoEncoderConfiguration->Multicast->Address, struct tt__IPAddress);
		VideoEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
		ALLOC_TOKEN(VideoEncoderConfiguration->Multicast->Address->IPv4Address, ONVIF_WSD_MC_ADDR);
	
		VideoEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
		VideoEncoderConfiguration->Multicast->Port = 9090;
		VideoEncoderConfiguration->Multicast->TTL = 1500;
		VideoEncoderConfiguration->Multicast->AutoStart = xsd__boolean__false_;

		ALLOC_TOKEN(VideoEncoderConfiguration->SessionTimeout, "PT5S");	
		
	}

	trt__GetCompatibleVideoEncoderConfigurationsResponse->__sizeConfigurations = ONVIF_VIDEO_ENCODER_CFG_NUM;
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations = VideoEncoderConfigurationArray;
	__FUN_END("\n");

	return SOAP_OK;
}

 int  __trt__GetCompatibleVideoSourceConfigurations(struct soap* soap, struct _trt__GetCompatibleVideoSourceConfigurations *trt__GetCompatibleVideoSourceConfigurations, struct _trt__GetCompatibleVideoSourceConfigurationsResponse *trt__GetCompatibleVideoSourceConfigurationsResponse) 
{
	int idx;
	HI_ONVIF_VIDEO_SOURCE_CFG_S OnvifVideoSourceCfg;
	struct tt__VideoSourceConfiguration *VideoSourceConfiguration, *VideoSourceConfigurationArray;
	__FUN_BEGIN("\n");
	
	ALLOC_STRUCT_NUM(VideoSourceConfigurationArray,  struct tt__VideoSourceConfiguration, ONVIF_VIDEO_SOURCE_CFG_NUM);
	for(idx = 0; idx < ONVIF_VIDEO_SOURCE_CFG_NUM; idx++)
	{
		VideoSourceConfiguration = &VideoSourceConfigurationArray[idx];
		/* VideoSourceConfiguration */
		onvif_get_video_source_cfg(idx, &OnvifVideoSourceCfg);	
		ALLOC_TOKEN(VideoSourceConfiguration->Name, OnvifVideoSourceCfg.Name);
		ALLOC_TOKEN(VideoSourceConfiguration->SourceToken, OnvifVideoSourceCfg.SourceToken);
		VideoSourceConfiguration->UseCount = OnvifVideoSourceCfg.UseCount;
		ALLOC_STRUCT(VideoSourceConfiguration->Bounds, struct tt__IntRectangle);	
		VideoSourceConfiguration->Bounds->y = OnvifVideoSourceCfg.Bounds.y;
		VideoSourceConfiguration->Bounds->x = OnvifVideoSourceCfg.Bounds.x;
		VideoSourceConfiguration->Bounds->width  = OnvifVideoSourceCfg.Bounds.width;
		VideoSourceConfiguration->Bounds->height = OnvifVideoSourceCfg.Bounds.height;	
	}

	trt__GetCompatibleVideoSourceConfigurationsResponse->__sizeConfigurations = ONVIF_VIDEO_SOURCE_CFG_NUM;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations = VideoSourceConfigurationArray;
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __trt__GetCompatibleAudioEncoderConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioEncoderConfigurations *trt__GetCompatibleAudioEncoderConfigurations, struct _trt__GetCompatibleAudioEncoderConfigurationsResponse *trt__GetCompatibleAudioEncoderConfigurationsResponse) 
{
	int idx;
	HI_ONVIF_AUDIO_ENCODER_CFG_S OnvifAudioEncoderCfg;;
	struct tt__AudioEncoderConfiguration *AudioEncoderConfiguration, *AudioEncoderConfigurationArray;
	__FUN_BEGIN("\n");

	
	ALLOC_STRUCT_NUM(AudioEncoderConfigurationArray, struct tt__AudioEncoderConfiguration, ONVIF_AUDIO_SOURCE_CFG_NUM);
		for(idx = 0; idx < ONVIF_AUDIO_SOURCE_CFG_NUM; idx++)
		{
			onvif_get_audio_encoder_cfg(idx, &OnvifAudioEncoderCfg);
			AudioEncoderConfiguration = &AudioEncoderConfigurationArray[idx];		
			ALLOC_TOKEN(AudioEncoderConfiguration->Name, OnvifAudioEncoderCfg.Name);
			AudioEncoderConfiguration->UseCount = OnvifAudioEncoderCfg.UseCount;
			ALLOC_TOKEN(AudioEncoderConfiguration->token, OnvifAudioEncoderCfg.token);
			AudioEncoderConfiguration->Encoding = OnvifAudioEncoderCfg.Encoding;
			AudioEncoderConfiguration->Bitrate = OnvifAudioEncoderCfg.Bitrate;
			AudioEncoderConfiguration->SampleRate = OnvifAudioEncoderCfg.SampleRate;
			ALLOC_STRUCT(AudioEncoderConfiguration->Multicast, struct tt__MulticastConfiguration);
			ALLOC_STRUCT(AudioEncoderConfiguration->Multicast->Address, struct tt__IPAddress);
			AudioEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
			ALLOC_TOKEN(AudioEncoderConfiguration->Multicast->Address->IPv4Address, ONVIF_WSD_MC_ADDR);
			AudioEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
			AudioEncoderConfiguration->Multicast->Port = 9090;
			AudioEncoderConfiguration->Multicast->TTL = 1500;
			AudioEncoderConfiguration->Multicast->AutoStart = xsd__boolean__false_; 
			ALLOC_TOKEN(AudioEncoderConfiguration->SessionTimeout, "PT15S");			
			
		}
	
	trt__GetCompatibleAudioEncoderConfigurationsResponse->__sizeConfigurations = ONVIF_VIDEO_ENCODER_CFG_NUM;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations = AudioEncoderConfigurationArray;
	__FUN_END("\n");

	return SOAP_OK;	
	//onvif_fault(soap, "ter:ActionNotSupported", "ter:AudioNotSupported");
	//return SOAP_FAULT;
}

 int  __trt__GetCompatibleAudioSourceConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioSourceConfigurations *trt__GetCompatibleAudioSourceConfigurations, struct _trt__GetCompatibleAudioSourceConfigurationsResponse *trt__GetCompatibleAudioSourceConfigurationsResponse) 
{
	int idx;
	HI_ONVIF_AUDIO_SOURCE_CFG_S OnvifAudioSourceCfg;
	struct tt__AudioSourceConfiguration *AudioSourceConfiguration, *AudioSourceConfigurationArray;
	__FUN_BEGIN("\n");
	
	ALLOC_STRUCT_NUM(AudioSourceConfigurationArray, struct tt__AudioSourceConfiguration, ONVIF_AUDIO_SOURCE_CFG_NUM);
	for(idx = 0; idx < ONVIF_AUDIO_SOURCE_CFG_NUM; idx++)
	{			
		/* AudioSourceConfiguration */
		onvif_get_audio_source_cfg(idx, &OnvifAudioSourceCfg);
		AudioSourceConfiguration = &AudioSourceConfigurationArray[idx];
		ALLOC_TOKEN(AudioSourceConfiguration->Name, OnvifAudioSourceCfg.Name);			
		ALLOC_TOKEN(AudioSourceConfiguration->token, OnvifAudioSourceCfg.token);
		ALLOC_TOKEN(AudioSourceConfiguration->SourceToken, OnvifAudioSourceCfg.SourceToken);
		AudioSourceConfiguration->UseCount = OnvifAudioSourceCfg.UseCount;
	}

	trt__GetCompatibleAudioSourceConfigurationsResponse->__sizeConfigurations = ONVIF_AUDIO_SOURCE_CFG_NUM;
	trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations = AudioSourceConfigurationArray;
	__FUN_END("\n");
	return SOAP_OK;
	//onvif_fault(soap, "ter:ActionNotSupported", "ter:AudioNotSupported");
	//return SOAP_FAULT;
}

 int  __trt__GetCompatibleVideoAnalyticsConfigurations(struct soap* soap, struct _trt__GetCompatibleVideoAnalyticsConfigurations *trt__GetCompatibleVideoAnalyticsConfigurations, struct _trt__GetCompatibleVideoAnalyticsConfigurationsResponse *trt__GetCompatibleVideoAnalyticsConfigurationsResponse) 
{
	onvif_fault(soap, "ter:ActionNotSupported", "ter:VideoAnalyticsNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_OK;
}

 int  __trt__GetCompatibleMetadataConfigurations(struct soap* soap, struct _trt__GetCompatibleMetadataConfigurations *trt__GetCompatibleMetadataConfigurations, struct _trt__GetCompatibleMetadataConfigurationsResponse *trt__GetCompatibleMetadataConfigurationsResponse) 
{
	onvif_fault(soap, "ter:InvalidArgVal", "ter:NoConfig");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__GetCompatibleAudioOutputConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioOutputConfigurations *trt__GetCompatibleAudioOutputConfigurations, struct _trt__GetCompatibleAudioOutputConfigurationsResponse *trt__GetCompatibleAudioOutputConfigurationsResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioIn/OutNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__GetCompatibleAudioDecoderConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioDecoderConfigurations *trt__GetCompatibleAudioDecoderConfigurations, struct _trt__GetCompatibleAudioDecoderConfigurationsResponse *trt__GetCompatibleAudioDecoderConfigurationsResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioIn/OutNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__SetVideoSourceConfiguration(struct soap* soap, struct _trt__SetVideoSourceConfiguration *trt__SetVideoSourceConfiguration, struct _trt__SetVideoSourceConfigurationResponse *trt__SetVideoSourceConfigurationResponse) 
{
	HI_ONVIF_VIDEO_SOURCE_CFG_S OnvifVideoSourceCfg;
	HI_ONVIF_VIDEO_SOURCE_S OnvifVideoSource;

	if(trt__SetVideoSourceConfiguration->Configuration != NULL)
	{
		return SOAP_FAULT;
	}
	
	if(onvif_get_video_source_cfg_from_token(trt__SetVideoSourceConfiguration->Configuration->token, &OnvifVideoSourceCfg) < 0
		|| onvif_get_video_source_from_token(trt__SetVideoSourceConfiguration->Configuration->SourceToken, &OnvifVideoSource) < 0)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoConfig");
		return SOAP_FAULT;
	}
	onvif_fault(soap, "ter:InvalidArgVal", "ter:ConfigModify");
	return SOAP_FAULT;
}

 int  __trt__SetVideoEncoderConfiguration(struct soap* soap, struct _trt__SetVideoEncoderConfiguration *trt__SetVideoEncoderConfiguration, struct _trt__SetVideoEncoderConfigurationResponse *trt__SetVideoEncoderConfigurationResponse) 
{
	struct tt__VideoEncoderConfiguration *Configuration;
	IPNC_DEV_VENC_CFG_S stVencCfg;
	__FUN_BEGIN("\n");
	int index = 0, chn = 0, streamNo = 0;
	Configuration = trt__SetVideoEncoderConfiguration->Configuration;
	
	for (index = 0; index < ONVIF_VIDEO_ENCODER_CFG_NUM; index++)
	{
		if(strcmp(Configuration->token, gOnvifInfo.OnvifVideoEncoderCfg[index].token) == 0)
			break;
	}
	if (index == ONVIF_VIDEO_ENCODER_CFG_NUM) 
		goto ConfigModifyError;

	chn = index>>1;
	streamNo = index%2;

	if(streamNo== 0)
	{
		if(0 > nvr_get_ipnc_param(chn,IPNC_CMD_VMAIN_CFG, &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S)))
		{
			memset(&stVencCfg,0, sizeof(IPNC_DEV_VENC_CFG_S));
		}
	}
	else
	{
		if(0 > nvr_get_ipnc_param(chn,IPNC_CMD_VSUB_CFG, &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S)))
		{
			memset(&stVencCfg,0, sizeof(IPNC_DEV_VENC_CFG_S));
		}
	}
	
	if(Configuration->Encoding != tt__VideoEncoding__H264)
	{
		__E("The Encoding is not h264.\n");
		goto ConfigModifyError;
	}
	if(Configuration->Resolution != NULL)
	{
		if((Configuration->Resolution->Width != gOnvifInfo.MediaConfig.h264Options[0].Width
			&& Configuration->Resolution->Width != gOnvifInfo.MediaConfig.h264Options[1].Width) 
			|| (Configuration->Resolution->Height!= gOnvifInfo.MediaConfig.h264Options[0].Height
			&& Configuration->Resolution->Height!= gOnvifInfo.MediaConfig.h264Options[1].Height))
			return SOAP_FAULT;
		gOnvifInfo.MediaConfig.VideoResolutionconfig.Width = Configuration->Resolution->Width;
		gOnvifInfo.MediaConfig.VideoResolutionconfig.Height= Configuration->Resolution->Height;
		__D("Width = %d\n", Configuration->Resolution->Width);
		__D("Height = %d\n", Configuration->Resolution->Height);
	}

	stVencCfg.u8PicQuilty = Configuration->Quality;
	if(Configuration->RateControl != NULL)
	{
		stVencCfg.u8FrameRate = Configuration->RateControl->FrameRateLimit;
		stVencCfg.u32BitrateRate = Configuration->RateControl->BitrateLimit;
		stVencCfg.u8Gop = Configuration->RateControl->EncodingInterval;	
		
		gOnvifInfo.MediaConfig.VideoEncoderRateControl.FrameRateLimit =  Configuration->RateControl->FrameRateLimit;
		gOnvifInfo.MediaConfig.VideoEncoderRateControl.BitrateLimit = Configuration->RateControl->BitrateLimit;	
		gOnvifInfo.MediaConfig.VideoEncoderRateControl.EncodingInterval = Configuration->RateControl->EncodingInterval;	
		__D("FrameRate = %d\n", Configuration->RateControl->FrameRateLimit);
		__D("BitrateLimit = %d\n", Configuration->RateControl->BitrateLimit);
		__D("EncodingInterval = %d\n", Configuration->RateControl->EncodingInterval);
	}
	
	/*
	if(Configuration->H264 != NULL)
	{	
		if(Configuration->H264->GovLength < 1 || Configuration->H264->GovLength > 200)
		{
			goto ConfigModifyError;
		}
		//stVencCfg.u8Gop = Configuration->H264->GovLength;
	}
	*/
	if(streamNo == 0)
	{
		nvr_set_ipnc_param(chn,IPNC_CMD_VMAIN_CFG, &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S));
	}
	else
	{
		nvr_set_ipnc_param(chn,IPNC_CMD_VSUB_CFG, &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S));
	}
	__FUN_END("\n");
	return SOAP_OK;
	
ConfigModifyError:	
	onvif_fault(soap, "ter:InvalidArgVal", "ter:ConfigModify");
	return SOAP_FAULT;
}

 int  __trt__SetAudioSourceConfiguration(struct soap* soap, struct _trt__SetAudioSourceConfiguration *trt__SetAudioSourceConfiguration, struct _trt__SetAudioSourceConfigurationResponse *trt__SetAudioSourceConfigurationResponse) 
{
	HI_ONVIF_AUDIO_SOURCE_CFG_S OnvifAudioSourceCfg;
	HI_ONVIF_AUDIO_SOURCE_S OnvifAudioSource;
	int i;
	__FUN_BEGIN("\n");
	for(i = 0; i < ONVIF_AUDIO_SOURCE_CFG_NUM; i++)
	{
		if(strcmp(trt__SetAudioSourceConfiguration->Configuration->token, gOnvifInfo.OnvifAudioSource[i].token))
		{
			onvif_fault(soap, "ter:InvalidArgVal", "ter:ConfigModify");
			return SOAP_FAULT;
		}
	}
	if(onvif_get_audio_source_cfg_from_token(trt__SetAudioSourceConfiguration->Configuration->token, &OnvifAudioSourceCfg) < 0
		|| onvif_get_audio_source_from_token(trt__SetAudioSourceConfiguration->Configuration->SourceToken, &OnvifAudioSource) < 0)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoConfig");
		return SOAP_FAULT;
	}
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __trt__SetAudioEncoderConfiguration(struct soap* soap, struct _trt__SetAudioEncoderConfiguration *trt__SetAudioEncoderConfiguration, struct _trt__SetAudioEncoderConfigurationResponse *trt__SetAudioEncoderConfigurationResponse) 
{
	#if 0
		NVR_DEV_AUDIO_CFG_S stAutoCfg;
		struct tt__AudioEncoderConfiguration *Configuration;
		HI_ONVIF_AUDIO_ENCODER_CFG_S OnvifEncoderCfg;
		__FUN_BEGIN("\n");

		memset(&stAutoCfg, 0, sizeof(NVR_DEV_AUDIO_CFG_S));
		
		nvr_logon_get_auto_cfg(0, &stAutoCfg);
		stAutoCfg.u8Open = 1;
		nvr_logon_set_auto_cfg(0, &stAutoCfg);
		
		Configuration = trt__SetAudioEncoderConfiguration->Configuration;
		
		if(onvif_get_audio_encoder_cfg_from_token(Configuration->token, &OnvifEncoderCfg) < 0)
		{
			onvif_fault(soap, "ter:InvalidArgVal", "ter:NoConfig");
			return SOAP_FAULT;
		}
	
		if(Configuration->Encoding != tt__AudioEncoding__G711)
		{
			__E("The Encoding is not G711.\n");
			goto ConfigModifyError;
		}
		if(strcmp(Configuration->token, "AudioSourceConfigureToken") == 0)
		{
			nvr_logon_get_auto_cfg(0, &stAutoCfg);
		}
	
		if(Configuration->SampleRate)
		{
			if(Configuration->SampleRate != 8000)
			{
				__E("The SampleRate is invalid\r\n");
				goto ConfigModifyError;
			}
			stAutoCfg.u32SampleRate = Configuration->SampleRate;
		}
		
		if(strcmp(Configuration->token, "AudioSourceConfigureToken") == 0)
		{
			nvr_logon_set_auto_cfg(0, &stAutoCfg);
		}
		
		__FUN_END("\n");
		return SOAP_OK;
		
		ConfigModifyError:	
		onvif_fault(soap, "ter:InvalidArgVal", "ter:ConfigModify");
		return SOAP_FAULT;

	//onvif_fault(soap, "ter:ActionNotSupported", "ter:AudioNotSupported");
	#endif
	return SOAP_OK;
}

 int  __trt__SetVideoAnalyticsConfiguration(struct soap* soap, struct _trt__SetVideoAnalyticsConfiguration *trt__SetVideoAnalyticsConfiguration, struct _trt__SetVideoAnalyticsConfigurationResponse *trt__SetVideoAnalyticsConfigurationResponse) 
{
	onvif_fault(soap, "ter:ActionNotSupported", "ter:VideoAnalyticsNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__SetMetadataConfiguration(struct soap* soap, struct _trt__SetMetadataConfiguration *trt__SetMetadataConfiguration, struct _trt__SetMetadataConfigurationResponse *trt__SetMetadataConfigurationResponse) 
{
	onvif_fault(soap, "ter:InvalidArgVal", "ter:ConfigModify");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__SetAudioOutputConfiguration(struct soap* soap, struct _trt__SetAudioOutputConfiguration *trt__SetAudioOutputConfiguration, struct _trt__SetAudioOutputConfigurationResponse *trt__SetAudioOutputConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioIn/OutNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__SetAudioDecoderConfiguration(struct soap* soap, struct _trt__SetAudioDecoderConfiguration *trt__SetAudioDecoderConfiguration, struct _trt__SetAudioDecoderConfigurationResponse *trt__SetAudioDecoderConfigurationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioIn/OutNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__GetVideoSourceConfigurationOptions(struct soap* soap, struct _trt__GetVideoSourceConfigurationOptions *trt__GetVideoSourceConfigurationOptions, struct _trt__GetVideoSourceConfigurationOptionsResponse *trt__GetVideoSourceConfigurationOptionsResponse) 
{
	
	struct tt__VideoSourceConfigurationOptions *Options;
	struct tt__IntRectangleRange *BoundsRange;
	HI_ONVIF_VIDEO_SOURCE_CFG_S OnvifVideoSourceCfg;

#if 0
	int index = 0, chn = 0, streamNo = 0;
	for (index = 0; index < ONVIF_VIDEO_ENCODER_CFG_NUM; index++)
	{
		if(strcmp(trt__GetVideoSourceConfigurationOptions->ConfigurationToken, gOnvifInfo.OnvifVideoSourceCfg[index].token) == 0)
			break;
	}
	
	if (index == ONVIF_VIDEO_ENCODER_CFG_NUM)
		return SOAP_FAULT;
	
	chn = index>>1;
	streamNo = index%2;
#endif	
	if(onvif_get_video_source_cfg_from_token(trt__GetVideoSourceConfigurationOptions->ConfigurationToken, &OnvifVideoSourceCfg) < 0)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoConfig");
		return SOAP_FAULT;
	}

	ALLOC_STRUCT(Options, struct tt__VideoSourceConfigurationOptions);
	ALLOC_STRUCT(BoundsRange, struct tt__IntRectangleRange);	
	ALLOC_STRUCT(BoundsRange->XRange, struct tt__IntRange);	
	ALLOC_STRUCT(BoundsRange->YRange, struct tt__IntRange);
	ALLOC_STRUCT(BoundsRange->WidthRange, struct tt__IntRange);
	ALLOC_STRUCT(BoundsRange->HeightRange, struct tt__IntRange);	
	
	BoundsRange->XRange->Min = 0;
	BoundsRange->XRange->Max = 0;
	BoundsRange->YRange->Min = 0;
	BoundsRange->YRange->Max = 0;
	BoundsRange->WidthRange->Min = OnvifVideoSourceCfg.Bounds.width;
	BoundsRange->WidthRange->Max = OnvifVideoSourceCfg.Bounds.width;
	BoundsRange->HeightRange->Min = OnvifVideoSourceCfg.Bounds.height;
	BoundsRange->HeightRange->Max= OnvifVideoSourceCfg.Bounds.height;
	Options->BoundsRange = BoundsRange;
	
	Options->__sizeVideoSourceTokensAvailable = 1;	
	ALLOC_STRUCT_NUM(Options->VideoSourceTokensAvailable, char *, 1);
	ALLOC_TOKEN(Options->VideoSourceTokensAvailable[0], OnvifVideoSourceCfg.SourceToken);	

	trt__GetVideoSourceConfigurationOptionsResponse->Options = Options;
	__FUN_END("\n");
	
	return SOAP_OK;
}
 
int  __trt__GetVideoEncoderConfigurationOptions(struct soap* soap, struct _trt__GetVideoEncoderConfigurationOptions *trt__GetVideoEncoderConfigurationOptions, struct _trt__GetVideoEncoderConfigurationOptionsResponse *trt__GetVideoEncoderConfigurationOptionsResponse) 
{
	int ret;
	int index = 0, chn = 0, streamNo = 0;
	IPNC_DEV_VENC_CFG_S stVencCfg;
	
	HI_ONVIF_VIDEO_ENCODER_CFG_S OnvifVideoEncoderCfg;
	struct tt__VideoEncoderConfigurationOptions *Options;
	
	__FUN_BEGIN("\n"); 
	for (index = 0; index < ONVIF_VIDEO_ENCODER_CFG_NUM; index++)
	{
		if(strcmp(trt__GetVideoEncoderConfigurationOptions->ConfigurationToken, gOnvifInfo.OnvifVideoEncoderCfg[index].token) == 0)
			break;
	}
	
	if (index == ONVIF_VIDEO_ENCODER_CFG_NUM)
		return SOAP_FAULT;
	
	chn = index>>1;
	streamNo = index%2;
	
	memset(&stVencCfg, 0, sizeof(stVencCfg));
	if(streamNo == 0)
		ret = nvr_get_ipnc_param(chn,IPNC_CMD_VMAIN_CFG, &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S));
	else
		ret = nvr_get_ipnc_param(chn,IPNC_CMD_VSUB_CFG, &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S));

	if (ret < 0) //return SOAP_FAULT;
	{
		stVencCfg.u16Width = 1280;
		stVencCfg.u16Height = 720;
	}

	ALLOC_STRUCT(Options, struct tt__VideoEncoderConfigurationOptions);
	ALLOC_STRUCT(Options->QualityRange, struct tt__IntRange);	
	Options->QualityRange->Min = 0;
	Options->QualityRange->Max = 5;
	Options->JPEG = NULL;
	Options->MPEG4 = NULL;

	ALLOC_STRUCT(Options->H264, struct tt__H264Options);	
	ALLOC_STRUCT(Options->H264->GovLengthRange, struct tt__IntRange);
	Options->H264->GovLengthRange->Min = 1;
	Options->H264->GovLengthRange->Max = 75;
	ALLOC_STRUCT(Options->H264->FrameRateRange, struct tt__IntRange);
	Options->H264->FrameRateRange->Min = 10;
	Options->H264->FrameRateRange->Max = 30;	

	ALLOC_STRUCT(Options->H264->EncodingIntervalRange, struct tt__IntRange);	
	Options->H264->EncodingIntervalRange->Min = 0;
	Options->H264->EncodingIntervalRange->Max = 100;

	ALLOC_STRUCT_NUM(Options->H264->H264ProfilesSupported, enum tt__H264Profile, 3);	
	Options->H264->__sizeH264ProfilesSupported = 3;
	Options->H264->H264ProfilesSupported[0] = tt__H264Profile__High;
	Options->H264->H264ProfilesSupported[1] = tt__H264Profile__Main;
	Options->H264->H264ProfilesSupported[2] = tt__H264Profile__Baseline;
	
	Options->H264->__sizeResolutionsAvailable = 1;
	gOnvifInfo.MediaConfig.h264Options[streamNo].Width = stVencCfg.u16Width;
	gOnvifInfo.MediaConfig.h264Options[streamNo].Height = stVencCfg.u16Height;
	ALLOC_STRUCT(Options->H264->ResolutionsAvailable, struct tt__VideoResolution);
	*Options->H264->ResolutionsAvailable = gOnvifInfo.MediaConfig.h264Options[streamNo]; 
	
	Options->Extension = NULL;
	Options->__anyAttribute = NULL;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options = Options;
	
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __trt__GetAudioSourceConfigurationOptions(struct soap* soap, struct _trt__GetAudioSourceConfigurationOptions *trt__GetAudioSourceConfigurationOptions, struct _trt__GetAudioSourceConfigurationOptionsResponse *trt__GetAudioSourceConfigurationOptionsResponse) 
{
	struct tt__AudioSourceConfigurationOptions *AudioSourceConfigurationOptions;
	HI_ONVIF_AUDIO_SOURCE_CFG_S OnvifAudioSourceCfg;
//	int i;
	__FUN_BEGIN("\n");

	if(trt__GetAudioSourceConfigurationOptions->ConfigurationToken != NULL)
	{
		if(onvif_get_audio_source_cfg_from_token(trt__GetAudioSourceConfigurationOptions->ConfigurationToken, &OnvifAudioSourceCfg) < 0)
		{
			onvif_fault(soap, "ter:InvalidArgVal", "ter:NoAudioSource");
			return SOAP_FAULT;
		}
	}

	ALLOC_STRUCT(AudioSourceConfigurationOptions, struct tt__AudioSourceConfigurationOptions);
	
	AudioSourceConfigurationOptions->__sizeInputTokensAvailable = 1;
	ALLOC_STRUCT_NUM(AudioSourceConfigurationOptions->InputTokensAvailable, char *, 3);
	ALLOC_TOKEN(AudioSourceConfigurationOptions->InputTokensAvailable[0], OnvifAudioSourceCfg.SourceToken);
	

	trt__GetAudioSourceConfigurationOptionsResponse->Options = AudioSourceConfigurationOptions;
	__FUN_END("\n");

	return SOAP_OK; 
	//onvif_fault(soap, "ter:ActionNotSupported", "ter:AudioNotSupported");
	//return SOAP_FAULT;
}

 int  __trt__GetAudioEncoderConfigurationOptions(struct soap* soap, struct _trt__GetAudioEncoderConfigurationOptions *trt__GetAudioEncoderConfigurationOptions, struct _trt__GetAudioEncoderConfigurationOptionsResponse *trt__GetAudioEncoderConfigurationOptionsResponse) 
{
		HI_ONVIF_AUDIO_ENCODER_CFG_S OnvifAudioEncoderCfg;
		struct tt__AudioEncoderConfigurationOptions *Options;
	
		__FUN_BEGIN("\n");
		
		if(onvif_get_audio_encoder_cfg_from_token(trt__GetAudioEncoderConfigurationOptions->ConfigurationToken, &OnvifAudioEncoderCfg) < 0)
		{
			onvif_fault(soap, "ter:InvalidArgVal", "ter:NoConfig");
			return SOAP_FAULT;
		}
		
		Options = (struct tt__AudioEncoderConfigurationOptions *)soap_malloc(soap, sizeof(struct tt__AudioEncoderConfigurationOptions));
		if(Options == NULL)
		{
			__E("Failed to malloc for options.\n");
			return SOAP_FAULT;
		}
		Options->__sizeOptions = 1;
		Options->Options = (struct tt__AudioEncoderConfigurationOption *)soap_malloc(soap, sizeof(struct tt__AudioEncoderConfigurationOption));
		if(Options == NULL)
		{
			__E("Failed to malloc for options.\n");
			return SOAP_FAULT;
		}
		Options->Options->Encoding = OnvifAudioEncoderCfg.Encoding;
		Options->Options->BitrateList = (struct tt__IntList *)soap_malloc(soap, sizeof(struct tt__IntList));
		if(Options->Options->BitrateList == NULL)
		{
			__E("Failed to malloc for BitrateList.\n");
			return SOAP_FAULT;
		}
		Options->Options->SampleRateList = (struct tt__IntList *)soap_malloc(soap, sizeof(struct tt__IntList));
		if(Options->Options->SampleRateList == NULL)
		{
			__E("Failed to malloc for SampleRateList.\n");
			return SOAP_FAULT;
		}
		
		Options->Options->BitrateList->Items = (int *)soap_malloc(soap, sizeof(int)*4);
		Options->Options->SampleRateList->Items = (int *)soap_malloc(soap, sizeof(int)*4);
		Options->Options->BitrateList->__sizeItems = 1;
		*Options->Options->BitrateList->Items = OnvifAudioEncoderCfg.Bitrate;
		
		Options->Options->SampleRateList->__sizeItems = 1;
		*Options->Options->SampleRateList->Items = OnvifAudioEncoderCfg.SampleRate;
		Options->Options->__size = 0;
		Options->Options->__any = NULL;
		Options->Options->__anyAttribute = NULL;
		trt__GetAudioEncoderConfigurationOptionsResponse->Options = Options;
		
		__FUN_END("\n");
		return SOAP_OK;

	//onvif_fault(soap, "ter:ActionNotSupported", "ter:AudioNotSupported");
	//return SOAP_FAULT;
}

 int  __trt__GetMetadataConfigurationOptions(struct soap* soap, struct _trt__GetMetadataConfigurationOptions *trt__GetMetadataConfigurationOptions, struct _trt__GetMetadataConfigurationOptionsResponse *trt__GetMetadataConfigurationOptionsResponse) 
{
	onvif_fault(soap, "ter:InvalidArgVal", "ter:NoConfig");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__GetAudioOutputConfigurationOptions(struct soap* soap, struct _trt__GetAudioOutputConfigurationOptions *trt__GetAudioOutputConfigurationOptions, struct _trt__GetAudioOutputConfigurationOptionsResponse *trt__GetAudioOutputConfigurationOptionsResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioIn/OutNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__GetAudioDecoderConfigurationOptions(struct soap* soap, struct _trt__GetAudioDecoderConfigurationOptions *trt__GetAudioDecoderConfigurationOptions, struct _trt__GetAudioDecoderConfigurationOptionsResponse *trt__GetAudioDecoderConfigurationOptionsResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:AudioIn/OutNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__GetGuaranteedNumberOfVideoEncoderInstances(struct soap* soap, struct _trt__GetGuaranteedNumberOfVideoEncoderInstances *trt__GetGuaranteedNumberOfVideoEncoderInstances, struct _trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse *trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse) 
{
	__FUN_BEGIN("\n");
	if(strcmp(trt__GetGuaranteedNumberOfVideoEncoderInstances->ConfigurationToken, ONVIF_MEDIA_VIDEO_MAIN_CFG_TOKEN) != 0)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoConfig");
		return SOAP_FAULT;
	}
	
	trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse->TotalNumber = 1;
	return SOAP_OK; 
}

 int  __trt__GetStreamUri(struct soap* soap, struct _trt__GetStreamUri *trt__GetStreamUri, struct _trt__GetStreamUriResponse *trt__GetStreamUriResponse) 
{
	struct tt__StreamSetup *StreamSetup;
	struct tt__MediaUri *MediaUri;
	HI_ONVIF_PROFILE_S OnvifProfile;
	
	
	__FUN_BEGIN("\n");
	unsigned long ip;
	ip = onvif_get_ipaddr(soap);
	if(onvif_get_profile_from_token(ip, trt__GetStreamUri->ProfileToken, &OnvifProfile) < 0)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoProfile");
		return SOAP_FAULT;
	}
	
	if(trt__GetStreamUri->StreamSetup == NULL)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:InvalidStreamSetup");
		return SOAP_FAULT;
	}

	StreamSetup = trt__GetStreamUri->StreamSetup;
	if(StreamSetup->Stream != tt__StreamType__RTP_Unicast)
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:InvalidStreamSetup");
		return SOAP_FAULT;
	}

	ALLOC_STRUCT(MediaUri, struct tt__MediaUri);	
	ALLOC_TOKEN(MediaUri->Uri, OnvifProfile.streamUri);

	/*
	if(gOnvifInfo.MediaConfig.VideoResolutionconfig.Height == gOnvifInfo.MediaConfig.h264Options[1].Height && 
		gOnvifInfo.MediaConfig.VideoResolutionconfig.Width == gOnvifInfo.MediaConfig.h264Options[1].Width)
			onvif_get_profile(1, &OnvifProfile);
	*/
	
	MediaUri->InvalidAfterConnect = xsd__boolean__false_;
	MediaUri->InvalidAfterReboot = xsd__boolean__false_;
	ALLOC_TOKEN(MediaUri->Timeout, "PT3S");	
	trt__GetStreamUriResponse->MediaUri = MediaUri;
	__FUN_END("\n");
	return SOAP_OK;
}

 int  __trt__StartMulticastStreaming(struct soap* soap, struct _trt__StartMulticastStreaming *trt__StartMulticastStreaming, struct _trt__StartMulticastStreamingResponse *trt__StartMulticastStreamingResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoProfile");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__StopMulticastStreaming(struct soap* soap, struct _trt__StopMulticastStreaming *trt__StopMulticastStreaming, struct _trt__StopMulticastStreamingResponse *trt__StopMulticastStreamingResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:NoProfile");
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __trt__SetSynchronizationPoint(struct soap* soap, struct _trt__SetSynchronizationPoint *trt__SetSynchronizationPoint, struct _trt__SetSynchronizationPointResponse *trt__SetSynchronizationPointResponse) 
{
	int nStream = 0;
	if(strcmp(trt__SetSynchronizationPoint->ProfileToken, ONVIF_MEDIA_SUB_PROFILE_TOKEN) == 0)
	{
		nStream = 1;
	}
	else if(strcmp(trt__SetSynchronizationPoint->ProfileToken, ONVIF_MEDIA_MAIN_PROFILE_TOKEN) == 0)
	{
		nStream = 0;
	}
	else
	{
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoProfile");
		return SOAP_FAULT;
	}

	//ipc_avlib_video_request_iFrame(nCh, nStream); /*强制I 帧*/ //yyyyyy

	return SOAP_OK;	
}

 int  __trt__GetSnapshotUri(struct soap* soap, struct _trt__GetSnapshotUri *trt__GetSnapshotUri, struct _trt__GetSnapshotUriResponse *trt__GetSnapshotUriResponse) 
{
	__FUN_BEGIN("\n");
	char ipaddr[32];
	HI_ONVIF_PROFILE_S OnvifProfile;
	struct tt__MediaUri *MediaUri;
	unsigned long ip;	
	
	ip = htonl(net_get_sock_ip(soap->socket));	
	if(onvif_get_profile_from_token(ip, trt__GetSnapshotUri->ProfileToken, &OnvifProfile) < 0)
	{		
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoProfile");
		return SOAP_FAULT;
	}

	ALLOC_STRUCT(MediaUri, struct tt__MediaUri);	
	ALLOC_STRUCT_NUM(MediaUri->Uri, char, ONVIF_ADDRESS_LEN)
	net_ip_n2a(ip, ipaddr, 32);
	sprintf(MediaUri->Uri, "http://%s/snapshot.jpg", ipaddr);
	MediaUri->InvalidAfterConnect = xsd__boolean__false_;
	MediaUri->InvalidAfterReboot = xsd__boolean__false_;
	ALLOC_TOKEN(MediaUri->Timeout, "PT3S");
	MediaUri->__size = 0;
	MediaUri->__any = NULL;
	MediaUri->__anyAttribute = NULL;	
	trt__GetSnapshotUriResponse->MediaUri = MediaUri;

	__FUN_END("\n");
	return SOAP_OK;	
}

 int  __tse__GetServiceCapabilities(struct soap* soap, struct _tse__GetServiceCapabilities *tse__GetServiceCapabilities, struct _tse__GetServiceCapabilitiesResponse *tse__GetServiceCapabilitiesResponse) 
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;	
}

 int  __tse__GetRecordingSummary(struct soap* soap, struct _tse__GetRecordingSummary *tse__GetRecordingSummary, struct _tse__GetRecordingSummaryResponse *tse__GetRecordingSummaryResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:RecordingSummaryNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tse__GetRecordingInformation(struct soap* soap, struct _tse__GetRecordingInformation *tse__GetRecordingInformation, struct _tse__GetRecordingInformationResponse *tse__GetRecordingInformationResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:RecordingSummaryNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tse__GetMediaAttributes(struct soap* soap, struct _tse__GetMediaAttributes *tse__GetMediaAttributes, struct _tse__GetMediaAttributesResponse *tse__GetMediaAttributesResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:InvalidToken"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tse__FindRecordings(struct soap* soap, struct _tse__FindRecordings *tse__FindRecordings, struct _tse__FindRecordingsResponse *tse__FindRecordingsResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:RecordingNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tse__GetRecordingSearchResults(struct soap* soap, struct _tse__GetRecordingSearchResults *tse__GetRecordingSearchResults, struct _tse__GetRecordingSearchResultsResponse *tse__GetRecordingSearchResultsResponse) 
{
	onvif_fault(soap,"ter:ActionNotSupported", "ter:RecordingNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tse__FindEvents(struct soap* soap, struct _tse__FindEvents *tse__FindEvents, struct _tse__FindEventsResponse *tse__FindEventsResponse) 
{
	onvif_fault(soap,"ter:Action", "ter:ResourceProblem"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tse__GetEventSearchResults(struct soap* soap, struct _tse__GetEventSearchResults *tse__GetEventSearchResults, struct _tse__GetEventSearchResultsResponse *tse__GetEventSearchResultsResponse) 
{
	onvif_fault(soap,"ter:InvalidArgVal", "ter:InvalidToken"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tse__FindPTZPosition(struct soap* soap, struct _tse__FindPTZPosition *tse__FindPTZPosition, struct _tse__FindPTZPositionResponse *tse__FindPTZPositionResponse) 
{
	onvif_fault(soap, "ter:ActionNotSupported", "ter:PTZNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tse__GetPTZPositionSearchResults(struct soap* soap, struct _tse__GetPTZPositionSearchResults *tse__GetPTZPositionSearchResults, struct _tse__GetPTZPositionSearchResultsResponse *tse__GetPTZPositionSearchResultsResponse) 
{
	onvif_fault(soap, "ter:ActionNotSupported", "ter:PTZNotSupported"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tse__GetSearchState(struct soap* soap, struct _tse__GetSearchState *tse__GetSearchState, struct _tse__GetSearchStateResponse *tse__GetSearchStateResponse) 
{
	onvif_fault(soap, "ter:InvalidArgVal", "ter:InvalidToken"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tse__EndSearch(struct soap* soap, struct _tse__EndSearch *tse__EndSearch, struct _tse__EndSearchResponse *tse__EndSearchResponse) 
{
	onvif_fault(soap, "ter:InvalidArgVal", "ter:InvalidToken"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tse__FindMetadata(struct soap* soap, struct _tse__FindMetadata *tse__FindMetadata, struct _tse__FindMetadataResponse *tse__FindMetadataResponse) 
{
	onvif_fault(soap, "ter:Action", "ter:ResourceProblem"); 
	__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

 int  __tse__GetMetadataSearchResults(struct soap* soap, struct _tse__GetMetadataSearchResults *tse__GetMetadataSearchResults, struct _tse__GetMetadataSearchResultsResponse *tse__GetMetadataSearchResultsResponse) 
{
	onvif_fault(soap, "ter:InvalidArgVal", "ter:InvalidToken"); 
		__FUN_BEGIN("Onvif fault process!\r\n");
	 __FUN_END("Onvif fault process!\r\n");
        return SOAP_FAULT;
}

int __tptz__RemovePresetTour(struct soap* soap, struct _tptz__RemovePresetTour *tptz__RemovePresetTour, struct _tptz__RemovePresetTourResponse *tptz__RemovePresetTourResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;
}

 int  __tptz__GetPresetTours(struct soap* soap, struct _tptz__GetPresetTours *tptz__GetPresetTours, struct _tptz__GetPresetToursResponse *tptz__GetPresetToursResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;
}
 int  __tptz__GetPresetTour(struct soap* soap, struct _tptz__GetPresetTour *tptz__GetPresetTour, struct _tptz__GetPresetTourResponse *tptz__GetPresetTourResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;
}
 int  __tptz__GetPresetTourOptions(struct soap* soap, struct _tptz__GetPresetTourOptions *tptz__GetPresetTourOptions, struct _tptz__GetPresetTourOptionsResponse *tptz__GetPresetTourOptionsResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;
}
 int  __tptz__CreatePresetTour(struct soap* soap, struct _tptz__CreatePresetTour *tptz__CreatePresetTour, struct _tptz__CreatePresetTourResponse *tptz__CreatePresetTourResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;
}
 int  __tptz__ModifyPresetTour(struct soap* soap, struct _tptz__ModifyPresetTour *tptz__ModifyPresetTour, struct _tptz__ModifyPresetTourResponse *tptz__ModifyPresetTourResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;
}
 int  __tptz__OperatePresetTour(struct soap* soap, struct _tptz__OperatePresetTour *tptz__OperatePresetTour, struct _tptz__OperatePresetTourResponse *tptz__OperatePresetTourResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;
}

 int  __tmd__GetRelayOutputOptions(struct soap* soap, struct _tmd__GetRelayOutputOptions *tmd__GetRelayOutputOptions, struct _tmd__GetRelayOutputOptionsResponse *tmd__GetRelayOutputOptionsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetAudioSources(struct soap* soap, struct _trt__GetAudioSources *trt__GetAudioSources, struct _trt__GetAudioSourcesResponse *trt__GetAudioSourcesResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetAudioOutputs(struct soap* soap, struct _trt__GetAudioOutputs *trt__GetAudioOutputs, struct _trt__GetAudioOutputsResponse *trt__GetAudioOutputsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetVideoSources(struct soap* soap, struct _trt__GetVideoSources *trt__GetVideoSources, struct _trt__GetVideoSourcesResponse *trt__GetVideoSourcesResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetVideoOutputs(struct soap* soap, struct _tmd__GetVideoOutputs *tmd__GetVideoOutputs, struct _tmd__GetVideoOutputsResponse *tmd__GetVideoOutputsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetVideoSourceConfiguration(struct soap* soap, struct _tmd__GetVideoSourceConfiguration *tmd__GetVideoSourceConfiguration, struct _tmd__GetVideoSourceConfigurationResponse *tmd__GetVideoSourceConfigurationResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetVideoOutputConfiguration(struct soap* soap, struct _tmd__GetVideoOutputConfiguration *tmd__GetVideoOutputConfiguration, struct _tmd__GetVideoOutputConfigurationResponse *tmd__GetVideoOutputConfigurationResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetAudioSourceConfiguration(struct soap* soap, struct _tmd__GetAudioSourceConfiguration *tmd__GetAudioSourceConfiguration, struct _tmd__GetAudioSourceConfigurationResponse *tmd__GetAudioSourceConfigurationResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetAudioOutputConfiguration(struct soap* soap, struct _tmd__GetAudioOutputConfiguration *tmd__GetAudioOutputConfiguration, struct _tmd__GetAudioOutputConfigurationResponse *tmd__GetAudioOutputConfigurationResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__SetVideoSourceConfiguration(struct soap* soap, struct _tmd__SetVideoSourceConfiguration *tmd__SetVideoSourceConfiguration, struct _tmd__SetVideoSourceConfigurationResponse *tmd__SetVideoSourceConfigurationResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__SetVideoOutputConfiguration(struct soap* soap, struct _tmd__SetVideoOutputConfiguration *tmd__SetVideoOutputConfiguration, struct _tmd__SetVideoOutputConfigurationResponse *tmd__SetVideoOutputConfigurationResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__SetAudioSourceConfiguration(struct soap* soap, struct _tmd__SetAudioSourceConfiguration *tmd__SetAudioSourceConfiguration, struct _tmd__SetAudioSourceConfigurationResponse *tmd__SetAudioSourceConfigurationResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__SetAudioOutputConfiguration(struct soap* soap, struct _tmd__SetAudioOutputConfiguration *tmd__SetAudioOutputConfiguration, struct _tmd__SetAudioOutputConfigurationResponse *tmd__SetAudioOutputConfigurationResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetVideoSourceConfigurationOptions(struct soap* soap, struct _tmd__GetVideoSourceConfigurationOptions *tmd__GetVideoSourceConfigurationOptions, struct _tmd__GetVideoSourceConfigurationOptionsResponse *tmd__GetVideoSourceConfigurationOptionsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetVideoOutputConfigurationOptions(struct soap* soap, struct _tmd__GetVideoOutputConfigurationOptions *tmd__GetVideoOutputConfigurationOptions, struct _tmd__GetVideoOutputConfigurationOptionsResponse *tmd__GetVideoOutputConfigurationOptionsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetAudioSourceConfigurationOptions(struct soap* soap, struct _tmd__GetAudioSourceConfigurationOptions *tmd__GetAudioSourceConfigurationOptions, struct _tmd__GetAudioSourceConfigurationOptionsResponse *tmd__GetAudioSourceConfigurationOptionsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetAudioOutputConfigurationOptions(struct soap* soap, struct _tmd__GetAudioOutputConfigurationOptions *tmd__GetAudioOutputConfigurationOptions, struct _tmd__GetAudioOutputConfigurationOptionsResponse *tmd__GetAudioOutputConfigurationOptionsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetRelayOutputs(struct soap* soap, struct _tds__GetRelayOutputs *tds__GetRelayOutputs, struct _tds__GetRelayOutputsResponse *tds__GetRelayOutputsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__SetRelayOutputSettings(struct soap* soap, struct _tmd__SetRelayOutputSettings *tmd__SetRelayOutputSettings, struct _tmd__SetRelayOutputSettingsResponse *tmd__SetRelayOutputSettingsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__SetRelayOutputState(struct soap* soap, struct _tds__SetRelayOutputState *tds__SetRelayOutputState, struct _tds__SetRelayOutputStateResponse *tds__SetRelayOutputStateResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetDigitalInputs(struct soap* soap, struct _tmd__GetDigitalInputs *tmd__GetDigitalInputs, struct _tmd__GetDigitalInputsResponse *tmd__GetDigitalInputsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetSerialPorts(struct soap* soap, struct _tmd__GetSerialPorts *tmd__GetSerialPorts, struct _tmd__GetSerialPortsResponse *tmd__GetSerialPortsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetSerialPortConfiguration(struct soap* soap, struct _tmd__GetSerialPortConfiguration *tmd__GetSerialPortConfiguration, struct _tmd__GetSerialPortConfigurationResponse *tmd__GetSerialPortConfigurationResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__SetSerialPortConfiguration(struct soap* soap, struct _tmd__SetSerialPortConfiguration *tmd__SetSerialPortConfiguration, struct _tmd__SetSerialPortConfigurationResponse *tmd__SetSerialPortConfigurationResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__GetSerialPortConfigurationOptions(struct soap* soap, struct _tmd__GetSerialPortConfigurationOptions *tmd__GetSerialPortConfigurationOptions, struct _tmd__GetSerialPortConfigurationOptionsResponse *tmd__GetSerialPortConfigurationOptionsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tmd__SendReceiveSerialCommand(struct soap* soap, struct _tmd__SendReceiveSerialCommand *tmd__SendReceiveSerialCommand, struct _tmd__SendReceiveSerialCommandResponse *tmd__SendReceiveSerialCommandResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tae__GetSupportedActions(struct soap* soap, struct _tae__GetSupportedActions *tae__GetSupportedActions, struct _tae__GetSupportedActionsResponse *tae__GetSupportedActionsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tae__GetActions(struct soap* soap, struct _tae__GetActions *tae__GetActions, struct _tae__GetActionsResponse *tae__GetActionsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tae__CreateActions(struct soap* soap, struct _tae__CreateActions *tae__CreateActions, struct _tae__CreateActionsResponse *tae__CreateActionsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tae__DeleteActions(struct soap* soap, struct _tae__DeleteActions *tae__DeleteActions, struct _tae__DeleteActionsResponse *tae__DeleteActionsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tae__ModifyActions(struct soap* soap, struct _tae__ModifyActions *tae__ModifyActions, struct _tae__ModifyActionsResponse *tae__ModifyActionsResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tae__GetServiceCapabilities(struct soap* soap, struct _tae__GetServiceCapabilities *tae__GetServiceCapabilities, struct _tae__GetServiceCapabilitiesResponse *tae__GetServiceCapabilitiesResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tae__GetActionTriggers(struct soap* soap, struct _tae__GetActionTriggers *tae__GetActionTriggers, struct _tae__GetActionTriggersResponse *tae__GetActionTriggersResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tae__CreateActionTriggers(struct soap* soap, struct _tae__CreateActionTriggers *tae__CreateActionTriggers, struct _tae__CreateActionTriggersResponse *tae__CreateActionTriggersResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tae__DeleteActionTriggers(struct soap* soap, struct _tae__DeleteActionTriggers *tae__DeleteActionTriggers, struct _tae__DeleteActionTriggersResponse *tae__DeleteActionTriggersResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}
 int  __tae__ModifyActionTriggers(struct soap* soap, struct _tae__ModifyActionTriggers *tae__ModifyActionTriggers, struct _tae__ModifyActionTriggersResponse *tae__ModifyActionTriggersResponse)
{
		__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
		return SOAP_FAULT;
}

int  __tmd__GetServiceCapabilities(struct soap* soap, struct _tmd__GetServiceCapabilities *tmd__GetServiceCapabilities, struct _tmd__GetServiceCapabilitiesResponse *tmd__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
    __FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;
}

///////////////////////////////////////////////////////////////////
int _dn_Bye_send(struct soap* soap)
{
	char msgid[LARGE_INFO_LENGTH];
//	char macaddr[MACH_ADDR_LENGTH];
	time_t curTime;
	struct tm *ptm;
	struct tm tmTime;
	struct SOAP_ENV__Header header;
	//struct wsa__EndpointReferenceType wsa__EndpointReference;	
	
	NVR_DEV_NET_CFG_S stNetCfg;
	//soap_set_omode(pSoap, SOAP_ENC_ZLIB);
	memset(&header, 0, sizeof(struct SOAP_ENV__Header));	

	soap_default_SOAP_ENV__Header(soap, &header);						/* setting default values for header */
	soap->header = &header;
	
	unsigned char *pMac = NULL;
	unsigned long ip = htonl(net_get_sock_ip(soap->socket));	
	nvr_logon_get_net_cfg(0, &stNetCfg);	
	if(ip == stNetCfg.struEtherCfg[0].u32IpAddr)
		pMac = stNetCfg.struEtherCfg[0].u8MacAddr;
	else
		pMac = stNetCfg.struEtherCfg[1].u8MacAddr;
	
	curTime = time(NULL);
	ptm = localtime_r(&curTime, &tmTime);
	memset(msgid, 0, sizeof(msgid));
	sprintf(msgid,"uuid:1319d68a-%d%d%d-%d%d-%d%d-%02X%02X%02X%02X%02X%02X",
			ptm->tm_wday,ptm->tm_mday,ptm->tm_mon,
			ptm->tm_year,ptm->tm_hour,ptm->tm_min,
			ptm->tm_sec,pMac[0], pMac[1], pMac[2], 
			pMac[3], pMac[4], pMac[5]);
			
	 ALLOC_TOKEN(header.wsa__MessageID, msgid);		
	 ALLOC_TOKEN(header.wsa__To, "urn:schemas-xmlsoap-org:ws:2005:04:discovery");
	 ALLOC_TOKEN(header.wsa__Action, "http://schemas.xmlsoap.org/ws/2005/04/discovery/Bye");
	

	//if (soap_send___dnrd__sendBye(soap,NULL,NULL,NULL))
	//{
	//	soap_print_fault(soap, stderr);
	//}
	
	soap_destroy (soap);
	soap_end(soap);
	soap_done(soap);
	soap_free(soap);
	return SOAP_OK;
}

