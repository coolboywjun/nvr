#ifndef HI_DIANXIN_TURN_H
#define HI_DIANXIN_TURN_H

typedef enum 
{
   StunTypeUnknown=0,
   StunTypeFailure,
   StunTypeOpen,
   StunTypeBlocked,

   StunTypeIndependentFilter,
   StunTypeDependentFilter,
   StunTypePortDependedFilter,
   StunTypeDependentMapping,

   //StunTypeConeNat,
   //StunTypeRestrictedNat,
   //StunTypePortRestrictedNat,
   //StunTypeSymNat,
   
   StunTypeFirewall,
} NatType;


int hi_turn_get_relay_info(char *pRelayIp, int nIpLen, int *pRelayPort);

int hi_turn_start_client(const char *pTurnServerIp, int port, const char *turnServerUser, const char *turnServerPass);

void hi_turn_stop_client();

NatType hi_turn_get_nat_type(char *pNetName, const char *pTurnServer, int nTurnPort);

int hi_turn_client_init();

void hi_turn_client_uninit();



#endif

