/////////////////////////////
/// 1, 已经完成取流和取流失败后重试

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/prctl.h>

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "Base64.hh"

#include "h264_parse.h"
#include "rtsp_client.h"

// By default, we request that the server stream its data using RTP/UDP.
// If, instead, you want to request that the server stream via RTP-over-TCP, change the following to True:
#define REQUEST_STREAMING_OVER_TCP True

#define DUMMY_SINK_RECEIVE_BUFFER_SIZE (512 * 1024)

// 0: no print
// 1: print information
#define RTSP_CLIENT_VERBOSITY_LEVEL 0 // by default, print verbose output from each "RTSPClient"



// RTSP 'response handlers':
void continueAfterOPTIONS(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString);

// Other event handler functions:
void subsessionAfterPlaying(void* clientData); // called when a stream's subsession (e.g., audio or video substream) ends
void subsessionByeHandler(void* clientData); // called when a RTCP "BYE" is received for a subsession
void streamTimerHandler(void* clientData);
// called at the end of a stream's expected aliveFlag (if the stream has not already signaled its end using a RTCP "BYE")

// Used to iterate through each stream's 'subsessions', setting up each one:
void setupNextSubsession(RTSPClient* rtspClient);

// Used to shut down and close a stream (including its "RTSPClient" object):
void shutdownStream(RTSPClient* rtspClient, int exitCode = 0);


void RtspRestartTaskFunc(void * clientData);

// A function that outputs a string that identifies each stream (for debugging output).  Modify this if you wish:
UsageEnvironment& operator<<(UsageEnvironment& env, const RTSPClient& rtspClient) {
	return env << "[URL:\"" << rtspClient.url() << "\"]: ";
}

// A function that outputs a string that identifies each subsession (for debugging output).  Modify this if you wish:
UsageEnvironment& operator<<(UsageEnvironment& env, const MediaSubsession& subsession) {
	return env << subsession.mediumName() << "/" << subsession.codecName();
}



class OurRTSPHandle;

class StreamClientState {
	public:
		StreamClientState();
		virtual ~StreamClientState();

	public:
		
		MediaSubsessionIterator* iter;
		MediaSession* session;
		MediaSubsession* subsession;
 		TaskToken streamTimerTask;
		double duration;
		int aliveFlag;
		void *userData;
};
// Implementation of "StreamClientState":

StreamClientState::StreamClientState()
	: iter(NULL), session(NULL), subsession(NULL), streamTimerTask(NULL), duration(0.0), aliveFlag(1) {
}

StreamClientState::~StreamClientState() {
	delete iter;
	if (session != NULL) {
		 UsageEnvironment& env = session->envir(); // alias
		 env.taskScheduler().unscheduleDelayedTask(streamTimerTask);
		Medium::close(session);
	}
}

class OurRTSPClient: public RTSPClient {
	public:
		static OurRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
				int verbosityLevel = 0,
				char const* applicationName = NULL,
				portNumBits tunnelOverHTTPPortNum = 0);
		OurRTSPClient(UsageEnvironment& env, char const* rtspURL,
				int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum);
		// called only by createNew();
		virtual ~OurRTSPClient();
		OurRTSPHandle *RTSPClientGetHandle() {return ourRtspHandle;}
		void RTSPClientSetHandle(OurRTSPHandle *ourRtspHandle) { this->ourRtspHandle = ourRtspHandle;}
		void RTSPRequestIFrame();
		static void RTSPCheckOptionTaskFunc(void *clientData);
		void RTSPCheckOption();
		
	public:
		StreamClientState scs;
		TaskToken checkOptionTask;
		OurRTSPHandle *ourRtspHandle;
		unsigned playCseq;
};
// Implementation of "OurRTSPClient":

OurRTSPClient* OurRTSPClient::createNew(UsageEnvironment& env, char const* rtspURL,
				int verbosityLevel,
				char const* applicationName,
				portNumBits tunnelOverHTTPPortNum) {
	return new OurRTSPClient(env, rtspURL, 
						verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

OurRTSPClient::OurRTSPClient(UsageEnvironment& env, char const* rtspURL,
				int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum)
	: RTSPClient(env,rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1) {
	checkOptionTask = NULL;
	ourRtspHandle = NULL;
}

OurRTSPClient::~OurRTSPClient() {
	UsageEnvironment& env = envir();
	env.taskScheduler().unscheduleDelayedTask(checkOptionTask);
	checkOptionTask = NULL;
}

void OurRTSPClient::RTSPRequestIFrame()
{
	sendUserCommand(*scs.session, NULL, "SET_PARAMETER");
}

void OurRTSPClient::RTSPCheckOptionTaskFunc(void *clientData)
{
	OurRTSPClient *ourRtspClient = (OurRTSPClient *)clientData;
	ourRtspClient->checkOptionTask = NULL;
	if(ourRtspClient->scs.aliveFlag != 1)
	{
		shutdownStream(ourRtspClient);
		return;
	}
	ourRtspClient->scs.aliveFlag = 0;
	ourRtspClient->sendOptionsCommand(continueAfterOPTIONS);
	ourRtspClient->checkOptionTask = ourRtspClient->envir().taskScheduler().scheduleDelayedTask(10000000,
			(TaskFunc*)RTSPCheckOptionTaskFunc, ourRtspClient);
}

void OurRTSPClient::RTSPCheckOption()
{
	checkOptionTask = envir().taskScheduler().scheduleDelayedTask(10000000,
			(TaskFunc*)RTSPCheckOptionTaskFunc, this);
}

class OurRTSPHandle
{
	public:		
		OurRTSPHandle(const char *rtspURL, const char *userName, const char *userPasswd, int bKeepAlive,
					RtspClientFrameCb frameCb, RtspClientBeginFrameCb beginFrameCb, 
					RtspClientEndFrameCb endFrameCb, void *rtspUserData);
		
		virtual ~OurRTSPHandle();
		
		void RtspConnectServer();
		void RtspDisconnectServer();
		void RtspReconnectServer();
		
		static void RtspRestartTaskFunc(void * clientData);
		void RtspRestart(int delayUs);
		static void RtspOpenTaskFunc(void *clientData);
		void RtspOpen();
		static void RtspCloseTaskFunc(void *clientData);
		void RtspClose();
		static void RtspRequestIFrameTaskFunc(void *clientData);
		void RtspRequestIFrame();
		void RtspRun();
		void RtspExit();
		static void RtspShutdownTaskFunc(void *clientData);
		void RtspShutdown();
		
	public:
		const char *rtspURL;
		const char *userName;
		const char *userPasswd;
		int bKeepAlive;
		RtspClientFrameCb frameCb;
		RtspClientBeginFrameCb beginFrameCb;
		RtspClientEndFrameCb endFrameCb;
		void *rtspUserData;
		int bCallBeginFrameCb;
		volatile int bRequestIFrame;
		
		OurRTSPClient *ourRtspClient;
		UsageEnvironment *env;
		TaskScheduler *scheduler;
		char quit;
		pthread_t thid;
		
	private:
		TaskToken restartSocketTask;
		TaskToken shutdownRtspTask;
};

OurRTSPHandle::OurRTSPHandle(const char *rtspURL, const char *userName, const char *userPasswd, int bKeepAlive,
					RtspClientFrameCb frameCb, RtspClientBeginFrameCb beginFrameCb, 
					RtspClientEndFrameCb endFrameCb, void *rtspUserData)
{
	scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);
	quit = 0;
	thid = 0;
	
	restartSocketTask = NULL;
	shutdownRtspTask = NULL;
	this->rtspURL = strDup(rtspURL);
	if(userName != NULL)
		this->userName = strDup(userName);
	else
		this->userName = NULL;
	if(userPasswd != NULL)
		this->userPasswd = strDup(userPasswd);
	else
		this->userPasswd = NULL;
	this->bKeepAlive = bKeepAlive;
	this->frameCb = frameCb;
	this->beginFrameCb = beginFrameCb;
	this->endFrameCb = endFrameCb;
	this->rtspUserData = rtspUserData;
	this->bCallBeginFrameCb = 0;
	this->bRequestIFrame = 0;
	ourRtspClient = NULL;
}

OurRTSPHandle::~OurRTSPHandle()
{
	env->taskScheduler().unscheduleDelayedTask(restartSocketTask);
	env->taskScheduler().unscheduleDelayedTask(shutdownRtspTask);
	env->reclaim();
	env = NULL;
	delete scheduler;
	if(rtspURL)
		delete []rtspURL;
	if(userName)
		delete []userName;
	if(userPasswd)
		delete []userPasswd;
	
	if(ourRtspClient != NULL)
		Medium::close(ourRtspClient);
}
	
void OurRTSPHandle::RtspConnectServer()
{
	if(ourRtspClient != NULL)
		return;
	ourRtspClient = OurRTSPClient::createNew(*env, rtspURL,
							RTSP_CLIENT_VERBOSITY_LEVEL);
	ourRtspClient->RTSPClientSetHandle(this);
	ourRtspClient->sendDescribeCommand(continueAfterDESCRIBE);
}

void OurRTSPHandle::RtspDisconnectServer()
{
	Medium::close(ourRtspClient);
	ourRtspClient = NULL;
}

void OurRTSPHandle::RtspReconnectServer()
{
	Medium::close(ourRtspClient);
	ourRtspClient = OurRTSPClient::createNew(*env, rtspURL, 
							RTSP_CLIENT_VERBOSITY_LEVEL);
}

void OurRTSPHandle::RtspRestartTaskFunc(void * clientData)
{
	OurRTSPHandle *ourRtspHandle = (OurRTSPHandle *)clientData;
	ourRtspHandle->RtspConnectServer();
}

void OurRTSPHandle::RtspRestart(int delayUs)
{
	restartSocketTask = env->taskScheduler().scheduleDelayedTask(delayUs,
					(TaskFunc*)OurRTSPHandle::RtspRestartTaskFunc, this);
}

void OurRTSPHandle::RtspOpenTaskFunc(void *clientData)
{
	OurRTSPHandle *ourRtspHandle = (OurRTSPHandle *)clientData;
	ourRtspHandle->RtspConnectServer();
}

void OurRTSPHandle::RtspOpen()
{
	env->taskScheduler().scheduleDelayedTask(0,
					(TaskFunc*)RtspOpenTaskFunc, this);
}

void OurRTSPHandle::RtspClose()
{
	shutdownStream(ourRtspClient, 1);
}

void OurRTSPHandle::RtspRun()
{
	env->taskScheduler().doEventLoop(&quit);	
}

void OurRTSPHandle::RtspExit()
{
	quit = 1;
}

void OurRTSPHandle::RtspShutdownTaskFunc(void *clientData)
{
	OurRTSPHandle *ourRtspHandle = (OurRTSPHandle *)clientData;
	ourRtspHandle->shutdownRtspTask = NULL;
	shutdownStream(ourRtspHandle->ourRtspClient, 1);
	ourRtspHandle->RtspRestart(0);
}

void OurRTSPHandle::RtspShutdown()
{
	shutdownRtspTask = env->taskScheduler().scheduleDelayedTask(0,
					(TaskFunc*)RtspShutdownTaskFunc, this);
}

void OurRTSPHandle::RtspRequestIFrameTaskFunc(void *clientData)
{
	OurRTSPHandle *ourRtspHandle = (OurRTSPHandle *)clientData;
	OurRTSPClient *ourRtspClient = ourRtspHandle->ourRtspClient;
	if(ourRtspClient)
	{
		//ourRtspClient->sendSetParameterCommand(*ourRtspClient->scs.session, NULL, NULL, NULL);
		//ourRtspClient->sendUserCommand(*ourRtspClient->scs.session, NULL, "SET_PARAMETER");
	}
}

void OurRTSPHandle::RtspRequestIFrame()
{
	bRequestIFrame = 1;
}

// Define a data sink (a subclass of "MediaSink") to receive the data for each subsession (i.e., each audio or video 'substream').
// In practice, this might be a class (or a chain of classes) that decodes and then renders the incoming audio or video.
// Or it might be a "FileSink", for outputting the received data into a file (as is done by the "openRTSP" application).
// In this example code, however, we define a simple 'dummy' sink that receives incoming data, but does nothing with it.

class OurRTSPSink: public MediaSink {
	public:
		static OurRTSPSink* createNew(UsageEnvironment& env,
				MediaSubsession& subsession, // identifies the kind of data that's being received
				char const* spsAndPps = NULL,
				char const* streamId = NULL,// identifies the stream itself (optional)
			       unsigned long size = 0); 
		int getH264Width() {return fWidth;}
		int getH264Height() {return fHeight;}
	private:
	  	OurRTSPSink(UsageEnvironment& env, MediaSubsession& subsession, char const *spsAndPps, char const* streamId, unsigned long size);
		// called only by "createNew()"
		virtual ~OurRTSPSink();

		void decodeSpsAndPps(char const* spsAndPps);
		static void afterGettingFrame(void* clientData, unsigned frameSize,
				unsigned numTruncatedBytes,
				struct timeval presentationTime,
				unsigned aliveFlagInMicroseconds);
		void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
				struct timeval presentationTime, unsigned aliveFlagInMicroseconds, void *privateData);

	private:
		// redefined virtual functions:
		virtual Boolean continuePlaying();
	protected:
	
		
	private:
		u_int8_t* fReceiveBuffer;
		MediaSubsession& fSubsession;
		char* fStreamId;
		char* fSpsAndPps;
		int fWidth;
		int fHeight;

		u_int8_t *frameBuffer;
		unsigned int	frameBufferSize;
		unsigned int	frameOffset;
		struct timeval lastTimeVal;
		RtspFrameType frameType;
		unsigned int 	frameNo;
};

OurRTSPSink* OurRTSPSink::createNew(UsageEnvironment& env, MediaSubsession& subsession, char const* spsAndPps, char const* streamId, unsigned long size) {

  return new OurRTSPSink(env, subsession, spsAndPps, streamId, size);
}

OurRTSPSink::OurRTSPSink(UsageEnvironment& env, MediaSubsession& subsession, char const *spsAndPps, char const* streamId,  unsigned long size)
	: MediaSink(env),
	fSubsession(subsession) {	

		fSpsAndPps = NULL;
		fWidth = 0;
		fHeight = 0;
		//printf("===========================\r\n");		
		//printf("===========================\r\n");
		//printf("%s\r\n", spsAndPps);
		//printf("===========================\r\n");		
		//printf("===========================\r\n");
		decodeSpsAndPps(spsAndPps);
		
		//fStreamId = strDup(streamId);
		fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
		frameBufferSize = 256 * 1024;
		frameBuffer = new u_int8_t[frameBufferSize];
		frameOffset = 0;
		frameType = RTSP_FRAME_UNKNOW;
		frameNo = 1;
		memset(&lastTimeVal, 0, sizeof(lastTimeVal));
	}

OurRTSPSink::~OurRTSPSink() {
	if(fSpsAndPps)
		delete[] fSpsAndPps;
	delete[] fReceiveBuffer;
	//delete[] fStreamId;
	delete[] frameBuffer;
}

void OurRTSPSink::decodeSpsAndPps(char const* spsAndPps)
{	
	fSpsAndPps = NULL;	
	fWidth = 0;
	fHeight = 0;
	if(spsAndPps != NULL)
	{
		char spsBase64[256] = {0};
		char ppsBase64[256] = {0};
		
		fSpsAndPps = strdup(spsAndPps);
		if(sscanf(fSpsAndPps, "%[^,],%s", spsBase64, ppsBase64) == 2)
		{
			int ret;
			//printf("spsBase64 = %s, ppsBase64 = %s\r\n", spsBase64, ppsBase64);
			unsigned int lenght;
			unsigned int width, height;
			unsigned char *sps = base64Decode(spsBase64, lenght);
			ret = sps_parse((char *)sps, lenght, &width, &height);
			if (ret < 0)
			{
				width = 0;
				height = 0;
				//printf("============sps parse failed=============\r\n");
			}
			fWidth = width;
			fHeight = height;
			//printf("fWidth = %d, fHeight = %d\r\n",fWidth, fHeight);
			delete[] sps;
			//unsigned char *pps = base64Decode(ppsBase64, lenght);
			//pps_prase((char *)pps, lenght);
			//delete[] pps;
		}
	}
}

void OurRTSPSink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
		struct timeval presentationTime, unsigned aliveFlagInMicroseconds) {
	OurRTSPSink* sink = (OurRTSPSink*)clientData;

	MediaSubsession& fsubsession = sink->fSubsession;

	RTSPClient* rtspClient = (RTSPClient*)(fsubsession.miscPtr);
	StreamClientState& scs = ((OurRTSPClient*)rtspClient)->scs;

	sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime,
								aliveFlagInMicroseconds, scs.userData);
}

//#define DEBUG_PRINT_EACH_RECEIVED_FRAME
//#define DEBUG_PRINT_NPT
// If you don't want to see debugging output for each received frame, then comment out the following line:
//#define DEBUG_PRINT_EACH_RECEIVED_FRAME 1
void OurRTSPSink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
		struct timeval presentationTime, unsigned /*aliveFlagInMicroseconds*/,void *privateData) {
	// We've just received a frame of data.  (Optionally) print out information about it:
#ifdef DEBUG_PRINT_EACH_RECEIVED_FRAME
	if (fStreamId != NULL) envir() << "Stream \"" << fStreamId << "\"; ";
	envir() << fSubsession.mediumName() << "/" << fSubsession.codecName() << ":\tReceived " << frameSize << " bytes";
	if (numTruncatedBytes > 0) envir() << " (with " << numTruncatedBytes << " bytes truncated)";
	char uSecsStr[6+1]; // used to output the 'microseconds' part of the presentation time
	sprintf(uSecsStr, "%06u", (unsigned)presentationTime.tv_usec);
	envir() << ".\tPresentation time: " << (int)presentationTime.tv_sec << "." << uSecsStr;
	if (fSubsession.rtpSource() != NULL && !fSubsession.rtpSource()->hasBeenSynchronizedUsingRTCP()) {
		envir() << "!"; // mark the debugging output to indicate that this presentation time is not RTCP-synchronized
	}
#ifdef DEBUG_PRINT_NPT
	envir() << "\tNPT: " << fSubsession.getNormalPlayTime(presentationTime);
#endif
	envir() << "\n";
#endif
	
	OurRTSPClient* ourRtspClient = (OurRTSPClient *)fSubsession.miscPtr;
	OurRTSPHandle *ourRtspHandle = ourRtspClient->RTSPClientGetHandle();
	const u_int8_t nalStartCode[4] = {0x00, 0x00, 0x00, 0x01};

	if(strcmp(fSubsession.mediumName(), "video") == 0)
	{		
		if(strcmp(fSubsession.codecName(),"H264") == 0)
		{
			if(lastTimeVal.tv_sec == 0 && lastTimeVal.tv_usec == 0)
			{
				lastTimeVal.tv_sec = presentationTime.tv_sec;
				lastTimeVal.tv_usec = presentationTime.tv_usec;
			}
			if(frameOffset != 0 && 
				lastTimeVal.tv_sec != 0 && lastTimeVal.tv_usec != 0 && 
				(presentationTime.tv_sec != lastTimeVal.tv_sec || presentationTime.tv_usec != lastTimeVal.tv_usec))
			{
				RtspFrameInfo frameInfo;
				
				frameInfo.frameBuffer = frameBuffer;
				frameInfo.frameSize = frameOffset;
				frameInfo.frameType = (frameType == RTSP_FRAME_UNKNOW) ? RTSP_VIDEO_FRAME_P : RTSP_VIDEO_FRAME_I;
				frameInfo.frameNo = frameNo++;
				frameInfo.timestamp = (unsigned long long)lastTimeVal.tv_sec * 1000000 + lastTimeVal.tv_usec;
				// notice by xq, the rtpHead is used for jxj platform,
				// we must get the width and  height form sps or pps later.
				//frameInfo.videoFrameInfo.width = fSource->rtpHead.video.width;
				//frameInfo.videoFrameInfo.height = fSource->rtpHead.video.height;
				//printf("@@@@@@@@w= %d\r\n", getH264Width());				
				//printf("@@@@@@@@h= %d\r\n", getH264Height());
				frameInfo.videoFrameInfo.width = getH264Width();
				frameInfo.videoFrameInfo.height = getH264Height();
				//frameInfo.videoFrameInfo.encodeType = RTSP_VIDEO_H264;
				if(ourRtspHandle->frameCb(&frameInfo, ourRtspHandle->rtspUserData) < 0)
				{
					ourRtspHandle->RtspShutdown();
					return;
				}
				
				frameOffset = 0;
				lastTimeVal.tv_sec = presentationTime.tv_sec;
				lastTimeVal.tv_usec = presentationTime.tv_usec;
				frameType = RTSP_FRAME_UNKNOW;
			}
			
			switch(fReceiveBuffer[0] & 0x1f)
			{
				case 0x5:
					frameType = RTSP_VIDEO_FRAME_I;
					break;
				default:
					break;
			}
			if(frameBufferSize < frameOffset + frameSize)
			{
				frameBufferSize = (frameOffset + frameSize + 0xffff) & (~0xffff);
				u_int8_t *newBuffer = new u_int8_t[frameBufferSize];
				memcpy(newBuffer, frameBuffer, frameOffset);
				delete frameBuffer;
				frameBuffer = newBuffer;
			}
			memcpy(frameBuffer+frameOffset, nalStartCode, sizeof(nalStartCode));
			frameOffset += sizeof(nalStartCode);
			memcpy(frameBuffer+frameOffset, fReceiveBuffer, frameSize);
			frameOffset += frameSize;
		}
	}
	else if(strcmp(fSubsession.mediumName(), "audio") == 0)//音频数据
	{
		RtspFrameInfo frameInfo;
		
		frameInfo.frameBuffer = fReceiveBuffer;
		frameInfo.frameSize = frameSize;
		frameInfo.frameType = RTSP_AUDIO_FRAME_A;
		frameInfo.frameNo = frameNo++;
		frameInfo.timestamp = (unsigned long long)presentationTime.tv_sec * 1000000 + presentationTime.tv_usec;
		frameInfo.audioFrameInfo.audioSample = 0;
		if(strcmp(fSubsession.codecName(),"PCM") == 0)
			frameInfo.audioFrameInfo.encodeType = 0;
		else if(strcmp(fSubsession.codecName(),"PCMA") == 0)
			frameInfo.audioFrameInfo.encodeType = 1;
		else if(strcmp(fSubsession.codecName(),"PCMU") == 0)
			frameInfo.audioFrameInfo.encodeType = 2;
		else if(strcmp(fSubsession.codecName(),"G726") == 0)
			frameInfo.audioFrameInfo.encodeType = 3;
		else
		{
			envir() << "The rtsp audio encode type " << fSubsession.codecName() << " don't support\n";
			return;
		}
		frameInfo.audioFrameInfo.audioChannel = 1;
		frameInfo.audioFrameInfo.audioBits = 16;

		if(ourRtspHandle->frameCb(&frameInfo, ourRtspHandle->rtspUserData) < 0)
		{
			ourRtspHandle->RtspShutdown();
			return;
		}
	}

	/*********************************************************/
	if(ourRtspHandle->bRequestIFrame)
	{
		ourRtspClient->RTSPRequestIFrame();
		ourRtspHandle->bRequestIFrame = 0;
	}
	/*********************************************************/
	// Then continue, to request the next frame of data:
	continuePlaying();
}

Boolean OurRTSPSink::continuePlaying() {
	if (fSource == NULL) return False; // sanity check (should not happen)
	// Request the next frame of data from our input source.  "afterGettingFrame()" will get called later, when it arrives:
	fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE,
			afterGettingFrame, this,
			onSourceClosure, this);
	return True;
}

void continueAfterOPTIONS(RTSPClient* rtspClient, int resultCode, char* resultString) {
	OurRTSPClient *ourRtspClient = (OurRTSPClient *)rtspClient;
	do {
		UsageEnvironment& env = rtspClient->envir(); // alias
		if (resultCode != 0) {
			env << *rtspClient << "Failed to get a option description: " << resultString << "\n";
			delete[] resultString;
			break;
		}
		delete[] resultString;
		ourRtspClient->scs.aliveFlag = 1;
		return;
	}while(0);
}

// Implementation of the RTSP 'response handlers':

void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString) {
  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((OurRTSPClient*)rtspClient)->scs; // alias

    if (resultCode != 0) {
      env << *rtspClient << "Failed to get a SDP description: " << resultString << "\n";
      delete[] resultString;
      break;
    }

    char* const sdpDescription = resultString;
    //env << *rtspClient << "Got a SDP description:\n" << sdpDescription << "\n";

    // Create a media session object from this SDP description:
    scs.session = MediaSession::createNew(env, sdpDescription);
    delete[] sdpDescription; // because we don't need it anymore
    if (scs.session == NULL) {
      env << *rtspClient << "Failed to create a MediaSession object from the SDP description: " << env.getResultMsg() << "\n";
      break;
    } else if (!scs.session->hasSubsessions()) {
      env << *rtspClient << "This session has no media subsessions (i.e., no \"m=\" lines)\n";
      break;
    }

    // Then, create and set up our data source objects for the session.  We do this by iterating over the session's 'subsessions',
    // calling "MediaSubsession::initiate()", and then sending a RTSP "SETUP" command, on each one.
    // (Each 'subsession' will have its own data source.)
    scs.iter = new MediaSubsessionIterator(*scs.session);
    setupNextSubsession(rtspClient);
    return;
  } while (0);

  // An unrecoverable error occurred with this stream.
  shutdownStream(rtspClient);
}

void setupNextSubsession(RTSPClient* rtspClient) {
  UsageEnvironment& env = rtspClient->envir(); // alias
  StreamClientState& scs = ((OurRTSPClient*)rtspClient)->scs; // alias
  OurRTSPHandle *ourRtspHandle = ((OurRTSPClient*)rtspClient)->RTSPClientGetHandle();
  scs.subsession = scs.iter->next();
  if (scs.subsession != NULL) {
    if (!scs.subsession->initiate()) {
      env << *rtspClient << "Failed to initiate the \"" << *scs.subsession << "\" subsession: " << env.getResultMsg() << "\n";
      setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
    } else {
      //env << *rtspClient << "Initiated the \"" << *scs.subsession
	//  << "\" subsession (client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1 << ")\n";
	  
      // Continue setting up this subsession, by sending a RTSP "SETUP" command:
      rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, REQUEST_STREAMING_OVER_TCP);
    }
    return;
  }

  if(ourRtspHandle->beginFrameCb != NULL && ourRtspHandle->bCallBeginFrameCb == 0)
  {
  	ourRtspHandle->bCallBeginFrameCb = 1;
  	ourRtspHandle->beginFrameCb(ourRtspHandle->rtspUserData);
  }
  
  // We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
  if (scs.session->absStartTime() != NULL) {
    // Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
    rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, scs.session->absStartTime(), scs.session->absEndTime());
  } else {
    scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
    rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY);
  }
}

void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString) {
  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((OurRTSPClient*)rtspClient)->scs; // alias

    if (resultCode != 0) {
      env << *rtspClient << "Failed to set up the \"" << *scs.subsession << "\" subsession: " << resultString << "\n";
      break;
    }

    //env << *rtspClient << "Set up the \"" << *scs.subsession
	//<< "\" subsession (client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1 << ")\n";

    // Having successfully setup the subsession, create a data sink for it, and call "startPlaying()" on it.
    // (This will prepare the data sink to receive data; the actual flow of data from the client won't start happening until later,
    // after we've sent a RTSP "PLAY" command.)
	//env << *rtspClient << "sps pps == " << scs.subsession->fmtp_spropparametersets() << "\n";
    scs.subsession->sink = OurRTSPSink::createNew(env, *scs.subsession, scs.subsession->fmtp_spropparametersets(), rtspClient->url());
      // perhaps use your own custom "MediaSink" subclass instead
    if (scs.subsession->sink == NULL) {
      env << *rtspClient << "Failed to create a data sink for the \"" << *scs.subsession
	  << "\" subsession: " << env.getResultMsg() << "\n";
      break;
    }

    //env << *rtspClient << "Created a data sink for the \"" << *scs.subsession << "\" subsession\n";
    scs.subsession->miscPtr = rtspClient; // a hack to let subsession handle functions get the "RTSPClient" from the subsession 
    scs.subsession->sink->startPlaying(*(scs.subsession->readSource()),
				       subsessionAfterPlaying, scs.subsession);
    // Also set a handler to be called if a RTCP "BYE" arrives for this subsession:
    if (scs.subsession->rtcpInstance() != NULL) {
      scs.subsession->rtcpInstance()->setByeHandler(subsessionByeHandler, scs.subsession);
    }
  } while (0);
  delete[] resultString;

  // Set up the next subsession, if any:
  setupNextSubsession(rtspClient);
}

void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString) {
  Boolean success = False;

  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((OurRTSPClient*)rtspClient)->scs; // alias

    if (resultCode != 0) {
      env << *rtspClient << "Failed to start playing session: " << resultString << "\n";
      break;
    }

    // Set a timer to be handled at the end of the stream's expected duration (if the stream does not already signal its end
    // using a RTCP "BYE").  This is optional.  If, instead, you want to keep the stream active - e.g., so you can later
    // 'seek' back within it and do another RTSP "PLAY" - then you can omit this code.
    // (Alternatively, if you don't want to receive the entire stream, you could set this timer for some shorter value.)
    if (scs.duration > 0) {
      unsigned const delaySlop = 2; // number of seconds extra to delay, after the stream's expected duration.  (This is optional.)
      scs.duration += delaySlop;
      unsigned uSecsToDelay = (unsigned)(scs.duration*1000000);
      scs.streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)streamTimerHandler, rtspClient);
    }

    //env << *rtspClient << "Started playing session";
    //if (scs.duration > 0) {
    //  env << " (for up to " << scs.duration << " seconds)";
    //}
    //env << "...\n";

    success = True;
  } while (0);
  delete[] resultString;

  if (!success) {
    // An unrecoverable error occurred with this stream.
    shutdownStream(rtspClient);
  }
  else
  {
	((OurRTSPClient*)rtspClient)->RTSPCheckOption();
  }
}


// Implementation of the other event handlers:

void subsessionAfterPlaying(void* clientData) {
  MediaSubsession* subsession = (MediaSubsession*)clientData;
  RTSPClient* rtspClient = (RTSPClient*)(subsession->miscPtr);

  // Begin by closing this subsession's stream:
  Medium::close(subsession->sink);
  subsession->sink = NULL;

  // Next, check whether *all* subsessions' streams have now been closed:
  MediaSession& session = subsession->parentSession();
  MediaSubsessionIterator iter(session);
  while ((subsession = iter.next()) != NULL) {
    if (subsession->sink != NULL) return; // this subsession is still active
  }

  // All subsessions' streams have now been closed, so shutdown the client:
  shutdownStream(rtspClient);
}

void subsessionByeHandler(void* clientData) {
  MediaSubsession* subsession = (MediaSubsession*)clientData;
  RTSPClient* rtspClient = (RTSPClient*)subsession->miscPtr;
  UsageEnvironment& env = rtspClient->envir(); // alias

  //env << *rtspClient << "Received RTCP \"BYE\" on \"" << *subsession << "\" subsession\n";

  // Now act as if the subsession had closed:
  subsessionAfterPlaying(subsession);
}

void streamTimerHandler(void* clientData) {
  OurRTSPClient* rtspClient = (OurRTSPClient*)clientData;
  StreamClientState& scs = rtspClient->scs; // alias

  scs.streamTimerTask = NULL;

  // Shut down the stream:
  shutdownStream(rtspClient);
}

void shutdownStream(RTSPClient* rtspClient, int exitCode) {
	if(rtspClient == NULL)
		return;
  UsageEnvironment& env = rtspClient->envir(); // alias
  StreamClientState& scs = ((OurRTSPClient*)rtspClient)->scs; // alias
  OurRTSPHandle *ourRtspHandle = ((OurRTSPClient*)rtspClient)->RTSPClientGetHandle();

  // First, check whether any subsessions have still to be closed:
  if (scs.session != NULL) { 
    Boolean someSubsessionsWereActive = False;
    MediaSubsessionIterator iter(*scs.session);
    MediaSubsession* subsession;

    while ((subsession = iter.next()) != NULL) {
      if (subsession->sink != NULL) {
	Medium::close(subsession->sink);
	subsession->sink = NULL;

	if (subsession->rtcpInstance() != NULL) {
	  subsession->rtcpInstance()->setByeHandler(NULL, NULL); // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
	}

	someSubsessionsWereActive = True;
      }
    }

    if (someSubsessionsWereActive) {
      // Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
      // Don't bother handling the response to the "TEARDOWN".
      rtspClient->sendTeardownCommand(*scs.session, NULL);
    }
  }
  
  if(ourRtspHandle->endFrameCb != NULL && ourRtspHandle->bCallBeginFrameCb)
  {
  	ourRtspHandle->bCallBeginFrameCb = 0;
  	ourRtspHandle->endFrameCb(ourRtspHandle->rtspUserData);
  }

  //env << *rtspClient << "Closing the stream.\n";
	//Medium::close(OurRTSPClient);
	ourRtspHandle->RtspDisconnectServer();
    // Note that this will also cause this stream's "StreamClientState" structure to get reclaimed.
	if(exitCode == 0)
	{
		ourRtspHandle->RtspRestart(3000000);
	}
}

static void *__rtspClientThreadFunc(void *data)
{
	OurRTSPHandle *ourRtspHandle = (OurRTSPHandle *)data;
	prctl(PR_SET_NAME, "RtspClient");
	ourRtspHandle->RtspOpen();
	ourRtspHandle->RtspRun();
	ourRtspHandle->RtspClose();
	return NULL;
}

int rtspClientOpenUrl(char const* rtspURL, const char *userName, const char *userPasswd, 
					int bKeepAlive, RtspClientFrameCb frameCb, 
					RtspClientBeginFrameCb beginFrameCb, RtspClientEndFrameCb endFrameCb,
					void *rtspUserData, long *rtspHandle)
{
	OurRTSPHandle *ourRtspHandle = new OurRTSPHandle(rtspURL, userName, userPasswd, bKeepAlive,
										frameCb, beginFrameCb, endFrameCb, rtspUserData);
	pthread_create(&ourRtspHandle->thid, NULL, __rtspClientThreadFunc, ourRtspHandle);
	*rtspHandle = (long)ourRtspHandle;
	return 0;
}

void rtspClientCloseUrl(long rtspHandle)
{
	OurRTSPHandle *ourRtspHandle = (OurRTSPHandle *)rtspHandle;
	if(ourRtspHandle == NULL)
		return;
	ourRtspHandle->RtspExit();
	pthread_join(ourRtspHandle->thid, NULL);
	delete ourRtspHandle;
}

void rtspClientRequestIFrame(long rtspHandle)
{
	OurRTSPHandle *ourRtspHandle = (OurRTSPHandle *)rtspHandle;
	if(ourRtspHandle == NULL)
		return;
	ourRtspHandle->RtspRequestIFrame();
}

int rtspClientInit()
{
	/* nothin */
	return 0;
}

void rtspClientUninit()
{
	/* nothing */
}

#if 0
static int gServerQuit = 0;

extern "C" {
static int __RtspClientFrameCb(RtspFrameInfo *rtspFrameInfo, void *rtspUserData)
{
	printf("Receive a frame, frame size: %lu.\n", rtspFrameInfo->frameSize);
	if(rtspFrameInfo->frameType != RTSP_AUDIO_FRAME_A)
	{
		static int cnt = 0;
		if(cnt++ == 100)
		{
			cnt = 0;
			printf("Write frame cb failed.\n");
			return -1;
		}
	}
	return 0;
}

static void rtspSigQuit(int signo)
{
	gServerQuit = 1;
}
}

void *thread_run(void *data)
{
	long rtspHandle2;
	while(!gServerQuit)
	{
		rtspClientOpenUrl("rtsp://192.168.5.185:7554/dev=JXJ-NVR-12345678/media=0/channel=0&level=0", NULL, NULL, 1, __RtspClientFrameCb, NULL, NULL, NULL, &rtspHandle2);
		sleep(random() % 5);
		rtspClientRequestIFrame(rtspHandle2);
		sleep(random() % 5);
		rtspClientCloseUrl(rtspHandle2);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	long rtspHandle1;
	long rtspHandle2;
	int i;
	pthread_t thid1, thid2, thid3;
	
	signal(SIGINT, rtspSigQuit);
	rtspClientInit();
	
//	pthread_create(&thid1, NULL, thread_run, NULL);
//	pthread_create(&thid2, NULL, thread_run, NULL);
//	pthread_create(&thid3, NULL, thread_run, NULL);
//	thread_run(NULL);

//	pthread_join(thid1, NULL);
//	pthread_join(thid2, NULL);
//	pthread_join(thid3, NULL);
//	rtspClientOpenUrl("rtsp://192.168.20.124:7554/dev=JXJ-NVR-12345678/media=0/channel=0&level=0", NULL, NULL, 1, __RtspClientFrameCb, NULL, NULL, NULL, &rtspHandle1);
	rtspClientOpenUrl("rtsp://192.168.1.11/0", NULL, NULL, 1, __RtspClientFrameCb, NULL, NULL, NULL, &rtspHandle1);
	
	while(!gServerQuit) pause();
	
	rtspClientCloseUrl(rtspHandle1);

	rtspClientUninit();
	return 0;
}
#endif

