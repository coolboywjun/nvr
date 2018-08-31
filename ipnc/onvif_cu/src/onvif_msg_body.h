#ifndef ONVIF_MSG_BODY_H
#define ONVIF_MSG_BODY_H 

#define MSG_HTTP_HEADER \
"POST /onvif/media_service HTTP/1.1\r\n \
Host: %%s\r\n \
Content-Type: application/soap+xml; charset=utf-8\r\n \
Content-Length: %%d\r\n\r\n"


#define  PROBE_MSG \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n \
<Envelope xmlns:dn=\"http://www.onvif.org/ver10/network/wsdl\" xmlns=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<Header>\r\n \
  <wsa:MessageID xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">uuid:64b917bd-78f3-4b5e-aee9-83bfe8d53741</wsa:MessageID>\r\n \
  <wsa:To xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">urn:schemas-xmlsoap-org:ws:2005:04:discovery</wsa:To>\r\n \
  <wsa:Action xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</wsa:Action>\r\n \
</Header>\r\n \
<Body>\r\n \
 <Probe xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\">\r\n \
    <Types>dn:NetworkVideoTransmitter</Types>\r\n \
 <Scopes />\r\n \
 </Probe>\r\n \
</Body>\r\n \
</Envelope>\r\n\r\n"

/*
<?xml version="1.0" encoding="utf-8"?>
<Envelope xmlns:dn="http://www.onvif.org/ver10/network/wsdl" xmlns="http://www.w3.org/2003/05/soap-envelope">
<Header>
<wsa:MessageID xmlns:wsa="http://schemas.xmlsoap.org/ws/2004/08/addressing">uuid:b2c9253b-a328-478b-97d1-e4eceae083e0</wsa:MessageID>
<wsa:To xmlns:wsa="http://schemas.xmlsoap.org/ws/2004/08/addressing">urn:schemas-xmlsoap-org:ws:2005:04:discovery</wsa:To>
<wsa:Action xmlns:wsa="http://schemas.xmlsoap.org/ws/2004/08/addressing">http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</wsa:Action>
</Header>
<Body>
<Probe xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns="http://schemas.xmlsoap.org/ws/2005/04/discovery">
<Types>dn:NetworkVideoTransmitter</Types>
<Scopes />
</Probe>
</Body>
</Envelope>
*/

#define MSG_GET_CAPABILITY \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n \
<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
  <s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
    <GetCapabilities xmlns=\"http://www.onvif.org/ver10/device/wsdl\">\r\n \
      <Category>All</Category>\r\n \
    </GetCapabilities>\r\n \
  </s:Body>\r\n \
</s:Envelope>\r\n\r\n"

#define MSG_GET_PROFILES \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n \
<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
<GetProfiles xmlns=\"http://www.onvif.org/ver10/media/wsdl\"/>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"

#define MSG_GET_PROFILE \
"<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
<GetProfile xmlns=\"http://www.onvif.org/ver10/media/wsdl\">\r\n \
<ProfileToken>%s</ProfileToken>\r\n \
</GetProfile>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"

#define MSG_GET_SCOPES \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n \
<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
<GetScopes xmlns=\"http://www.onvif.org/ver10/device/wsdl\"/>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"

#define MSG_GET_VENC_CONFIGURATIONS \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n \
<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
  <s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
    <GetVideoEncoderConfigurations xmlns=\"http://www.onvif.org/ver10/media/wsdl\">\r\n \
  </s:Body>\r\n \
</s:Envelope>\r\n\r\n"

#define MSG_GET_VENC_CONFIGURATION_OPTIONS \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n \
<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
  <s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
    <GetVideoEncoderConfigurationOptions xmlns=\"http://www.onvif.org/ver10/media/wsdl\"> \
      <ConfigurationToken>%s</ConfigurationToken>\r\n \
    </GetVideoEncoderConfigurationOptions>\r\n \
  </s:Body>\r\n \
</s:Envelope>\r\n\r\n"	


#define MSG_SET_VENC_CONFIGURATIONS \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n \
<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
<SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\">\r\n \
  <Configuration token=\"%s\">\r\n \
  <Name xmlns=\"http://www.onvif.org/ver10/schema\">%s</Name>\r\n \
  <UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount>\r\n \
  <Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding>\r\n \
  <Resolution xmlns=\"http://www.onvif.org/ver10/schema\">\r\n \
    <Width>%d</Width>\r\n \
    <Height>%d</Height>\r\n \
  </Resolution>\r\n \
  <Quality xmlns=\"http://www.onvif.org/ver10/schema\">%.1f</Quality>\r\n \
  <RateControl xmlns=\"http://www.onvif.org/ver10/schema\">\r\n \
    <FrameRateLimit>%d</FrameRateLimit>\r\n \
    <EncodingInterval>%d</EncodingInterval>\r\n \
    <BitrateLimit>%d</BitrateLimit>\r\n \
  </RateControl>\r\n \
  <H264 xmlns=\"http://www.onvif.org/ver10/schema\">\r\n \
    <GovLength>%d</GovLength>\r\n \
    <H264Profile>%s</H264Profile>\r\n \
  </H264>\r\n \
  <Multicast xmlns=\"http://www.onvif.org/ver10/schema\">\r\n \
    <Address>\r\n \
      <Type>IPv4</Type>\r\n \
      <IPv4Address>239.255.255.250</IPv4Address>\r\n \
    </Address>\r\n \
    <Port>9090</Port>\r\n \
    <TTL>1500</TTL>\r\n \
    <AutoStart>false</AutoStart>\r\n \
  </Multicast>\r\n \
  <SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout>\r\n \
  </Configuration>\r\n \
    <ForcePersistence>false</ForcePersistence>\r\n \
  </SetVideoEncoderConfiguration>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"

#define MSG_GET_STREAM_URL \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n \
<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
  <GetStreamUri xmlns=\"http://www.onvif.org/ver10/media/wsdl\">\r\n \
    <StreamSetup>\r\n \
      <Stream xmlns=\"http://www.onvif.org/ver10/schema\">RTP-Unicast</Stream>\r\n \
      <Transport xmlns=\"http://www.onvif.org/ver10/schema\">\r\n \
        <Protocol>%s</Protocol>\r\n \
      </Transport>\r\n \
    </StreamSetup>\r\n \
    <ProfileToken>%s</ProfileToken>\r\n \
  </GetStreamUri>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"

#define MSG_GET_DEVICE_INFO \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n \
<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
  <GetDeviceInformation xmlns=\"http://www.onvif.org/ver10/device/wsdl\"/>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"

#define MSG_SYSTEM_REBOOT \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n \
<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
  <SystemReboot xmlns=\"http://www.onvif.org/ver10/device/wsdl\"/>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"

#define MSG_GET_IMAGE_OPTIONS \
"<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
<GetOptions xmlns=\"http://www.onvif.org/ver20/imaging/wsdl\">\r\n \
<VideoSourceToken>%s</VideoSourceToken>\r\n \
</GetOptions>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"


#define MSG_GET_IMAGE_SETTINGS \
"<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
<GetImagingSettings xmlns=\"http://www.onvif.org/ver20/imaging/wsdl\">\r\n \
<VideoSourceToken>%s</VideoSourceToken>\r\n \
</GetImagingSettings>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"


#define MSG_SET_IMAGE_SETTINGS \
"<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
<SetImagingSettings xmlns=\"http://www.onvif.org/ver20/imaging/wsdl\">\r\n \
<VideoSourceToken>%s</VideoSourceToken>\r\n \
<ImagingSettings>\r\n \
<BacklightCompensation xmlns=\"http://www.onvif.org/ver10/schema\">\r\n \
<Mode>OFF</Mode>\r\n \
<Level>0</Level>\r\n \
</BacklightCompensation>\r\n \
<Brightness xmlns=\"http://www.onvif.org/ver10/schema\">%.2f</Brightness>\r\n \
<ColorSaturation xmlns=\"http://www.onvif.org/ver10/schema\">%.2f</ColorSaturation>\r\n \
<Contrast xmlns=\"http://www.onvif.org/ver10/schema\">%.2f</Contrast>\r\n \
<IrCutFilter xmlns=\"http://www.onvif.org/ver10/schema\">%s</IrCutFilter>\r\n \
<Sharpness xmlns=\"http://www.onvif.org/ver10/schema\">%.2f</Sharpness>\r\n \
<WideDynamicRange xmlns=\"http://www.onvif.org/ver10/schema\">\r\n \
<Mode>OFF</Mode>\r\n \
</WideDynamicRange><WhiteBalance xmlns=\"http://www.onvif.org/ver10/schema\">\r\n \
<Mode>AUTO</Mode>\r\n \
<CrGain>0</CrGain>\r\n \
<CbGain>0</CbGain>\r\n \
</WhiteBalance>\r\n \
</ImagingSettings>\r\n \
<ForcePersistence>true</ForcePersistence>\r\n \
</SetImagingSettings>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"

#define MSG_GET_PRESETS \
"<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
<GetPresets xmlns=\"http://www.onvif.org/ver20/ptz/wsdl\">\r\n \
<ProfileToken>%s</ProfileToken>\r\n \
</GetPresets>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"

#define MSG_SET_PRESETS \
"<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
<SetPreset xmlns=\"http://www.onvif.org/ver20/ptz/wsdl\">\r\n \
<ProfileToken>%s</ProfileToken>\r\n \
<PresetName>%s</PresetName>\r\n \
</SetPreset>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"


#define MSG_GOTO_PRESETS \
"<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
<GotoPreset xmlns=\"http://www.onvif.org/ver20/ptz/wsdl\">\r\n \
<ProfileToken>%s</ProfileToken>\r\n \
<PresetToken>%s</PresetToken>\r\n \
</GotoPreset>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"	



#define MSG_PTZ_CONTINUOUS_MOVE \
"<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
<ContinuousMove xmlns=\"http://www.onvif.org/ver20/ptz/wsdl\">\r\n \
<ProfileToken>%s</ProfileToken>\r\n \
<Velocity>\r\n \
<PanTilt x=\"%.1f\" y=\"%.1f\" xmlns=\"http://www.onvif.org/ver10/schema\"/>\r\n \
</Velocity>\r\n \
</ContinuousMove>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"

#define MSG_PTZ_STOP \
"<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
<Stop xmlns=\"http://www.onvif.org/ver20/ptz/wsdl\">\r\n \
<ProfileToken>%s</ProfileToken>\r\n \
<PanTilt>true</PanTilt>\r\n \
<Zoom>false</Zoom>\r\n \
</Stop>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"

#define MSG_TIME_SET \
"<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\r\n \
<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n \
<SetSystemDateAndTime xmlns=\"http://www.onvif.org/ver10/device/wsdl\">\r\n \
<DateTimeType>Manual</DateTimeType>\r\n \
<DaylightSavings>false</DaylightSavings>\r\n \
<TimeZone>\r\n \
<TZ xmlns=\"http://www.onvif.org/ver10/schema\">%s%02d</TZ>\r\n \
</TimeZone>\r\n \
<UTCDateTime>\r\n \
<Time xmlns=\"http://www.onvif.org/ver10/schema\">\r\n \
<Hour>%d</Hour>\r\n \
<Minute>%d</Minute>\r\n \
<Second>%d</Second>\r\n \
</Time>\r\n \
<Date xmlns=\"http://www.onvif.org/ver10/schema\">\r\n \
<Year>%d</Year>\r\n \
<Month>%d</Month>\r\n \
<Day>%d</Day>\r\n \
</Date>\r\n \
</UTCDateTime>\r\n \
</SetSystemDateAndTime>\r\n \
</s:Body>\r\n \
</s:Envelope>\r\n\r\n"

#if 0
POST /onvif/services HTTP/1.1

Content-Type: application/soap+xml; charset=utf-8; action="http://www.onvif.org/ver10/media/wsdlGetProfile/"

Host: 192.168.5.250

Content-Length: 306

Accept-Encoding: gzip, deflate

Connection: Close



<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GetProfile xmlns="http://www.onvif.org/ver10/media/wsdl"><ProfileToken>MainStreamToken</ProfileToken></GetProfile></s:Body></s:Envelope>HTTP/1.0 200 OK

Server: gSOAP/2.8

Content-Type: application/soap+xml; charset=utf-8; action="http://www.onvif.org/ver10/media/wsdlGetProfile/"

Content-Length: 5387

Connection: close



<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://www.w3.org/2003/05/soap-envelope" xmlns:SOAP-ENC="http://www.w3.org/2003/05/soap-encoding" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:c14n="http://www.w3.org/2001/10/xml-exc-c14n#" xmlns:wsu="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd" xmlns:xenc="http://www.w3.org/2001/04/xmlenc#" xmlns:ds="http://www.w3.org/2000/09/xmldsig#" xmlns:wsse="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd" xmlns:xmime="http://tempuri.org/xmime.xsd" xmlns:xop="http://www.w3.org/2004/08/xop/include" xmlns:wsbf2="http://docs.oasis-open.org/wsrf/bf-2" xmlns:wsa="http://schemas.xmlsoap.org/ws/2004/08/addressing" xmlns:wsa5="http://www.w3.org/2005/08/addressing" xmlns:tt="http://www.onvif.org/ver10/schema" xmlns:wstop="http://docs.oasis-open.org/wsn/t-1" xmlns:wsr2="http://docs.oasis-open.org/wsrf/r-2" xmlns:daae="http://www.onvif.org/ver20/analytics/wsdl/AnalyticsEngineBinding" xmlns:dare="http://www.onvif.org/ver20/analytics/wsdl/RuleEngineBinding" xmlns:tan="http://www.onvif.org/ver20/analytics/wsdl" xmlns:decpp="http://www.onvif.org/ver10/events/wsdl/CreatePullPointBinding" xmlns:dee="http://www.onvif.org/ver10/events/wsdl/EventBinding" xmlns:denc="http://www.onvif.org/ver10/events/wsdl/NotificationConsumerBinding" xmlns:denf="http://www.onvif.org/ver10/events/wsdl/NotificationProducerBinding" xmlns:depp="http://www.onvif.org/ver10/events/wsdl/PullPointBinding" xmlns:depps="http://www.onvif.org/ver10/events/wsdl/PullPointSubscriptionBinding" xmlns:tev="http://www.onvif.org/ver10/events/wsdl" xmlns:depsm="http://www.onvif.org/ver10/events/wsdl/PausableSubscriptionManagerBinding" xmlns:wsnt="http://docs.oasis-open.org/wsn/b-2" xmlns:desm="http://www.onvif.org/ver10/events/wsdl/SubscriptionManagerBinding" xmlns:dndl="http://www.onvif.org/ver10/network/wsdl/DiscoveryLookupBinding" xmlns:dnrd="http://www.onvif.org/ver10/network/wsdl/RemoteDiscoveryBinding" xmlns:d="http://schemas.xmlsoap.org/ws/2005/04/discovery" xmlns:dn="http://www.onvif.org/ver10/network/wsdl" xmlns:tad="http://www.onvif.org/ver10/analyticsdevice/wsdl" xmlns:tae="http://www.onvif.org/ver10/actionengine/wsdl" xmlns:tds="http://www.onvif.org/ver10/device/wsdl" xmlns:timg="http://www.onvif.org/ver20/imaging/wsdl" xmlns:tls="http://www.onvif.org/ver10/display/wsdl" xmlns:tmd="http://www.onvif.org/ver10/deviceIO/wsdl" xmlns:tptz="http://www.onvif.org/ver20/ptz/wsdl" xmlns:trc="http://www.onvif.org/ver10/recording/wsdl" xmlns:trp="http://www.onvif.org/ver10/replay/wsdl" xmlns:trt="http://www.onvif.org/ver10/media/wsdl" xmlns:trv="http://www.onvif.org/ver10/receiver/wsdl" xmlns:tse="http://www.onvif.org/ver10/search/wsdl" xmlns:tns1="http://www.onvif.org/ver10/topics" xmlns:tnsn="http://www.eventextension.com/2012/event/topics" xmlns:ter="http://www.onvif.org/ver10/error" xmlns:xmime5="http://www.w3.org/2005/05/xmlmime"><SOAP-ENV:Body><trt:GetProfileResponse><trt:Profile fixed="true" token="MainStreamToken"><tt:Name>MainStreamProfile</tt:Name><tt:VideoSourceConfiguration token="VideoSourceConfigToken"><tt:Name>VideoSourceConfig</tt:Name><tt:UseCount>2</tt:UseCount><tt:SourceToken>VideoSourceConfigToken</tt:SourceToken><tt:Bounds height="480" width="640" y="0" x="0"></tt:Bounds></tt:VideoSourceConfiguration><tt:AudioSourceConfiguration token="G711"><tt:Name>G711</tt:Name><tt:UseCount>2</tt:UseCount><tt:SourceToken>G711</tt:SourceToken></tt:AudioSourceConfiguration><tt:VideoEncoderConfiguration token="VideoEncodeConfigToken_m"><tt:Name>VideoEncodeConfig_m</tt:Name><tt:UseCount>1</tt:UseCount><tt:Encoding>H264</tt:Encoding><tt:Resolution><tt:Width>1280</tt:Width><tt:Height>720</tt:Height></tt:Resolution><tt:Quality>52.4528313</tt:Quality><tt:RateControl><tt:FrameRateLimit>25</tt:FrameRateLimit><tt:EncodingInterval>2</tt:EncodingInterval><tt:BitrateLimit>2500</tt:BitrateLimit></tt:RateControl><tt:MPEG4><tt:GovLength>30</tt:GovLength><tt:Mpeg4Profile>SP</tt:Mpeg4Profile></tt:MPEG4><tt:H264><tt:GovLength>25</
tt:GovLength><tt:H264Profile>High</tt:H264Profile></tt:H264><tt:Multicast><tt:Address><tt:Type>IPv4</tt:Type><tt:IPv4Address>192.168.5.250</tt:IPv4Address></tt:Address><tt:Port>0</tt:Port><tt:TTL>0</tt:TTL><tt:AutoStart>false</tt:AutoStart></tt:Multicast><tt:SessionTimeout>PT0H12M0S</tt:SessionTimeout></tt:VideoEncoderConfiguration><tt:VideoAnalyticsConfiguration token="VideoAnalyticsConfigToken"><tt:Name>VideoAnalyticsConfig</tt:Name><tt:UseCount>2</tt:UseCount><tt:AnalyticsEngineConfiguration></tt:AnalyticsEngineConfiguration><tt:RuleEngineConfiguration></tt:RuleEngineConfiguration></tt:VideoAnalyticsConfiguration><tt:PTZConfiguration token="PtzToken"><tt:Name>Ptz</tt:Name><tt:UseCount>0</tt:UseCount><tt:NodeToken>PtzNodeToken</tt:NodeToken></tt:PTZConfiguration><tt:MetadataConfiguration token="MetaDataToken"><tt:Name>MetaData</tt:Name><tt:UseCount>1</tt:UseCount><tt:Analytics>false</tt:Analytics><tt:Multicast><tt:Address><tt:Type>IPv4</tt:Type><tt:IPv4Address>192.168.5.250</tt:IPv4Address></tt:Address><tt:Port>0</tt:Port><tt:TTL>0</tt:TTL><tt:AutoStart>false</tt:AutoStart></tt:Multicast><tt:SessionTimeout>PT0H12M0S</tt:SessionTimeout></tt:MetadataConfiguration></trt:Profile></trt:GetProfileResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>

#endif

#if 0
POST /onvif/ptz_service HTTP/1.1

Content-Type: application/soap+xml; charset=utf-8; action="http://www.onvif.org/ver20/ptz/wsdl/GetPresets"

Host: 192.168.5.221:8000

Content-Length: 309

Accept-Encoding: gzip, deflate

Connection: Close



<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GetPresets xmlns="http://www.onvif.org/ver20/ptz/wsdl"><ProfileToken>IPNCMainProfileToken</ProfileToken></GetPresets></s:Body></s:Envelope>

#endif
	

#endif

