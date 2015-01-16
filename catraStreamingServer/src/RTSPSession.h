/*
 Copyright (C) Giuliano Catrambone (giuliano.catrambone@catrasoftware.it)

 This program is free software; you can redistribute it and/or 
 modify it under the terms of the GNU General Public License 
 as published by the Free Software Foundation; either 
 version 2 of the License, or (at your option) any later 
 version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 Commercial use other than under the terms of the GNU General Public
 License is allowed only after express negotiation of conditions
 with the authors.
*/


#ifndef RTSPSession_h
	#define RTSPSession_h

	#include "MP4File.h"
	#include "MP4FileFactory.h"
	#include "RTPPacket.h"
	#include "RTPHintSample.h"
	#include "MP4TrakAtom.h"
	#include "MP4RootAtom.h"
	#include "MP4MdhdAtom.h"
	#include "MP4TrackInfo.h"
	#include "ClientSocket.h"
	#include "LiveSource.h"
	#include "StreamingServerEventsSet.h"
	#include "StreamingServerSocketsPool.h"
	#include "Scheduler.h"
	#include "ServerSocket.h"
	#ifdef RTPOVERBUFFERWINDOW
		#include "RTPOverBufferWindow.h"
	#elif RTPOVERBUFFERWINDOW2
		#include "RTPOverBufferWindow2.h"
	#else
	#endif
	#include "RTSP_RTCPTimes.h"
	#include "PauseTimes.h"
	#include "StreamingSessionTimes.h"
	#include "CheckReceivedRTCPPacketsTimes.h"
	#include "SendRTCPPacketsTimes.h"
	#include "SDPFor3GPP.h"
	#include <vector>

	#define SS_RTSPREQUESTLENGTH						(1024 * 5) + 1
	#define SS_MAXTRACKURILENGTH						1024 + 1
	// CR LF (13 10)
	#define SS_NEWLINE									"\r\n"
	#define SS_MAXLONGLENGTH							512 + 1
	#define SS_MAXSESSIONIDENTIFIERLENGTH				1024 + 1
	#define SS_RTSPSESSION_MAXTRACKSNUMBER				2
	#define SS_RTSPSESSION_MAXRTCPPACKETLENGTH			1024 * 2

	#define SS_RTSPSESSION_MAXURLPARAMETERLENGTH		512 + 1
	#define SS_RTSPSESSION_URLPARAMETER_STARTTIME		"StartTimeInSecs"
	#define SS_RTSPSESSION_URLPARAMETER_STREAMINGSESSIONTIMEOUT		"StreamingSessionTimeoutInSecs"

	#define SS_DEFAULTSDPGLOBALCONTROL					"*"
	#define SS_DEFAULTVIDEOCONTROL						"video"
	#define SS_DEFAULTAUDIOCONTROL						"audio"

	#define SS_STREAMINGSERVERPROCESSOR_DESTINATION		"StreamingServerProcessor"
	#define SS_RTSPSESSION_SOURCE						"RTSPSession"

	#define SS_RTSPSESSION_MAXLOGBUFFERLENGTH			512 + 1

	#define SS_RTSPSESSION_AUTHORIZATIONDISABLEDMESSAGE	\
		"Beginning hook disabled"


	typedef class RTSPSession
	{
		public:
			typedef enum RTSPCommand {
				SS_RTSP_DESCRIBE,
				SS_RTSP_ANNOUNCE,
				SS_RTSP_GET_PARAMETER,
				SS_RTSP_OPTIONS,
				SS_RTSP_PAUSE,
				SS_RTSP_PLAY,
				SS_RTSP_RECORD,
				SS_RTSP_REDIRECT,
				SS_RTSP_SETUP,
				SS_RTSP_SET_PARAMETER,
				SS_RTSP_TEARDOWN
			} RTSPCommand_t, *RTSPCommand_p;

			/*
				state           message received  next state
			------------------------------------------------------------
				Init            SETUP             Ready
								TEARDOWN          Init
				Ready           PLAY              Playing
								SETUP             Ready
								TEARDOWN          Init
								RECORD            Recording
				Playing         PLAY              Playing
								PAUSE             Ready
								TEARDOWN          Init
								SETUP             Playing
				Recording       RECORD            Recording
								PAUSE             Ready
								TEARDOWN          Init
								SETUP             Recording
			*/
			// The RTSP status are the states as described by the
			//	RTSP standard except for SS_RTSP_NOTUSED and SS_RTSP_CONNECTIONACCEPTED
			typedef enum RTSPStatus {
				SS_RTSP_NOTUSED,	// this object is not used
				SS_RTSP_CONNECTIONACCEPTED,	// before to read the RTSP DESCRIBE command
				SS_RTSP_INIT,		// after DESCRIBE
				SS_RTSP_READY,		// after SETUP
				SS_RTSP_PLAYING,
				SS_RTSP_RECORDING
			} RTSPStatus_t, *RTSPStatus_p;

		protected:
			typedef enum QualityOfService {
				kAllPackets = 0,
				kNoBFrames = 1,
				k90PercentPFrames = 2,
				k75PercentPFrames = 3,
				k50PercentPFrames = 4,
				kKeyFramesOnly = 5
			} QualityOfService_t, *QualityOfService_p;

			typedef struct TrackInfo {

				Boolean_t					_bTrackInitialized;

				char						_pTrackURI [SS_MAXTRACKURILENGTH];

				// initialized in addTrack
				Boolean_t					_bActivated;
				MP4TrackInfo_p				_pmtiTrackInfo;
				unsigned char				*_pucSampleBuffer;

				unsigned long				_ulRTPClientPort;
				unsigned long				_ulRTPServerPort;
				unsigned long				_ulRTCPClientPort;
				unsigned long				_ulRTCPServerPort;
				ClientSocket_t				_csRTPClientSocket;
				ClientSocket_t				_csRTCPClientSocket;
				ServerSocket_t				_ssRTCPServerSocket;
				// unsigned long				_ulFirstEditMovieTime;
				// unsigned long				_ulFirstEditMediaTime;
				unsigned long				_ulPayloadNumber;
				unsigned long				_ulRTPTimeScale;
				std:: vector<MP4TrackInfo_p>	_vReferencesTracksInfo;
				unsigned long				_ulFirstEditRTPTime;
				QualityOfService_t			_qosQualityOfService;
				unsigned long				_ulServerSSRC;
				unsigned long				_ulClientSSRC;
				unsigned short				_usRtpSequenceNumberRandomOffset;
				unsigned long				_ulRtpTimestampRandomOffset;

				// initialized in seek, prefetchNextPacketsUsing*Track
				unsigned long				_ulCurrentSampleIndex;
				// In case of streaming of an hint track
				//	this variable represent the current packet index
				// In case of streaming of a media track
				//	this variable represent the sequence number
				unsigned long				_ulCurrentPacketIndex;

				RTPStreamRealTimeInfo_t		_rsrtMediaStreamRealTimeInfo;
				// in case we are streaming an Hinted track,
				// we need two RTPStreamRealTimeInfo_t
				// because we work on both the tracks (media and hint)
				RTPStreamRealTimeInfo_t		_rsrtHintStreamRealTimeInfo;

				// RTCP SR packet and relative info
				char						_pRTCPSRPacket [36 + 60 + 28 + 8];
				unsigned long				_lRTCPSRPacketLength;
				unsigned long				_ulLastRelativeRTPPacketTransmissionTimeWithTimeScaleAndOffset;
				unsigned long				_ulSentRTPPacketsNumber;
				#ifdef WIN32
					__int64					_ullRTPPacketBytesSent;
				#else
					unsigned long long		_ullRTPPacketBytesSent;
				#endif
				unsigned long				_ulLostRTPPacketsNumber;
				unsigned long				_ulRTCPPacketsNumberReceived;
				#ifdef WIN32
					__int64					_ullRTCPBytesReceived;
				#else
					unsigned long long		_ullRTCPBytesReceived;
				#endif
				unsigned long				_ulJitter;

				// contain the RTP packets
				RTPPacket_p					_prpRTPPackets;
				RTPHintSample_t				_rhsCurrentRTPHintSample;

				std:: vector<RTPPacket_p>	_vFreeRTPPackets;
				std:: vector<RTPPacket_p>	_vBuiltRTPPackets;
				std:: vector<double>		_vTransmissionTimeBuiltRTPPackets;

			} TrackInfo_t, *TrackInfo_p;

			typedef struct RTPSession {

				// initialized in addRTPSessionToHandle
				#ifdef RTPOVERBUFFERWINDOW
					RTPOverBufferWindow_t		_robwRTPOverBufferWindow;
				#elif RTPOVERBUFFERWINDOW2
					RTPOverBufferWindow2_t		_robwRTPOverBufferWindow2;
				#else
				#endif
				Boolean_t					_bIsSessionFirstWrite;

				// initialized in addTrackToHandle
				TrackInfo_t					_ptiTracks [SS_RTSPSESSION_MAXTRACKSNUMBER];

				unsigned long				_ulSendRTPMaxSleepTimeInMilliSecs;

				// Initialized in handlePLAY
				double						_dRelativeRequestedStartTimeInSecs;
				double						_dRelativeRequestedStopTimeInSecs;
				double						_dRequestedSpeed;
				#ifdef WIN32
					__int64					_ullPlayTimeInMilliSeconds;
					__int64					_llNTPPlayTime;
					__int64					_ullLastSentRTCPPacketsInMilliSecs;
					__int64					_ullLastReceiveRTCPPacketsInMilliSecs;
				#else
					unsigned long long		_ullPlayTimeInMilliSeconds;
					long long				_llNTPPlayTime;
					unsigned long long		_ullLastSentRTCPPacketsInMilliSecs;
					unsigned long long		_ullLastReceiveRTCPPacketsInMilliSecs;
				#endif
				double						_dAdjustedPlayTimeInMilliSecs;

				double				_dRelativeTransmissionTimeInSecsOfLastSentRTPPacket;


				unsigned long				_ulCurrentMaxPayloadSizeInBytes;
			} RTPSession_t, *RTPSession_p;

		protected:
			// configuration parameters
			Buffer_t				_bRequestLogFormat;
			unsigned long			_ulRTSP_RTCPTimeoutInSecs;
			unsigned long			_ulPauseTimeoutInSecs;
			unsigned long			_ulFileCacheSizeInBytes;
			unsigned long			_ulSendRTCPPacketsPeriodInMilliSecs;
			unsigned long			_ulReceiveRTCPPacketsPeriodInMilliSecs;
			Buffer_t				_bContentRootPath;
			char					_pLocalIPAddressForRTSP [
				SCK_MAXIPADDRESSLENGTH];
			char					_pLocalIPAddressForRTP [
				SCK_MAXIPADDRESSLENGTH];
			char					_pLocalIPAddressForRTCP [
				SCK_MAXIPADDRESSLENGTH];
			unsigned long			_ulRTSPRequestsPort;
			double					_dMaxSpeedAllowed;
			unsigned long			_ulMaxPayloadSizeInBytes;
			Boolean_t				_bUseOfHintingTrackIfExist;
			Boolean_t				_bBuildOfHintingTrackIfNotExist;
			Boolean_t				_bFlushTraceAfterEachRequest;
			unsigned long			_ulMaxLiveSourcesNumber;
			unsigned long			_ulRTPPacketsNumberPreallocated;
			unsigned long			_ulRTPPacketsNumberToPrefetch;
			unsigned long			_ulSamplesNumberToPrefetch;
			Boolean_t				_bIsOverBufferEnabled;
			unsigned long			_ulSendingInterval;
			unsigned long			_ulInitialWindowSizeInBytes;
			unsigned long			_ulMaxSendAheadTimeInSec;
			float					_fOverbufferRate;
			// If any of our media data is going over UDP, make sure
			// to introduce an artificial delay in our send schedule,
			// so as to make sure the PLAY response gets to the client
			// before media packets do.
			unsigned long			_ulPlayDelayTimeInMilliSeconds;
			// integration
			Boolean_t				_bBeginningHookActivated;
			Buffer_t				_bBeginningHookServletPathName;
			Buffer_t				_bBeginningHookServletParameters;
			char					_pBeginningHookWebServerIpAddress [
				SCK_MAXIPADDRESSLENGTH];
			unsigned long			_ulBeginningHookWebServerPort;
			char					_pBeginningHookLocalIPAddressForHTTP [
				SCK_MAXIPADDRESSLENGTH];
			unsigned long			_ulBeginningHookTimeoutInSecs;
			Boolean_t				_bEndingHookActivated;
			Buffer_t				_bEndingHookServletPathName;
			Buffer_t				_bEndingHookServletParameters;
			char					_pEndingHookWebServerIpAddress [
				SCK_MAXIPADDRESSLENGTH];
			unsigned long			_ulEndingHookWebServerPort;
			char					_pEndingHookLocalIPAddressForHTTP [
				SCK_MAXIPADDRESSLENGTH];
			unsigned long			_ulEndingHookTimeoutInSecs;


			unsigned long			_ulIdentifier;
			MP4Atom:: Standard_t	_sStandard;
			RTSPCommand_t			_rcLastRTSPCommand;
			RTSPCommand_t			_rcPreviousRTSPCommand;
			StreamingServerEventsSet_p		_pesEventsSet;
			StreamingServerSocketsPool_p	_pspSocketsPool;
			Scheduler_p				_pscScheduler;
			Tracer_p				_ptSystemTracer;
			Tracer_p				_ptSubscriberTracer;
			RTSPStatus_t			_rsRTSPStatus;
			PMutex_t				_mtRTSPSession;
			MP4FileFactory_p		_pmp4fMP4FileFactory;
			RTPSession_t			_rsRTPSession;
			ClientSocket_t			_csClientSocket;
			SocketImpl_p			_psiClientSocketImpl;
			long					_lRTSPClientPort;
			RTSP_RTCPTimes_t		_rrtRTSP_RTCPTimes;
			PauseTimes_t			_ptPauseTimes;
			StreamingSessionTimes_t	_sstStreamingSessionTimes;
			CheckReceivedRTCPPacketsTimes_t
									_crrptCheckReceivedRTCPPacketsTimes;
			SendRTCPPacketsTimes_t	_srptSendRTCPPacketsTimes;
			unsigned long			_ulReservedStartingPort;
			unsigned long			_ulReservedFinishingPort;
			unsigned long			_ulCurrentPortForSetup;
			// live
			PMutex_p				_pmtLiveSources;
			LiveSource_p			_plsLiveSources;


			// initialized in handleDESCRIBEMethod or handleSETUPMethod
			// both off-line and live
			time_t					_tUTCStartConnectionTime;
			time_t					_tUTCEndConnectionTime;
			tm						_tmLocalDateTimeStartConnection;
			unsigned long			_ulStreamingTimeInSecs;
			unsigned long			_ulStartConnectionMilliSeconds;
			char					_pClientIPAddress [SCK_MAXIPADDRESSLENGTH];
			Boolean_t				_bIsLive;
			Buffer_t				_bRelativePathWithoutParameters;
			Buffer_t				_bURLParameters;	// aaa=bbb&ccc=ddd...
			Buffer_t				_bBeginningHookMessage;
			Buffer_t				_bAssetPath;
			Buffer_t				_bUserAgent;
			Boolean_t				_bVideoTrackFoundInSDP;
			Boolean_t				_bAudioTrackFoundInSDP;
			unsigned long			_ulVideoTrackIdentifier;
			unsigned long			_ulAudioTrackIdentifier;
			char					_pVideoTrackName [MP4F_MAXTRACKNAMELENGTH];
			char					_pAudioTrackName [MP4F_MAXTRACKNAMELENGTH];
			unsigned long			_ulVideoPayloadNumber;
			unsigned long			_ulAudioPayloadNumber;
			MP4Atom:: MP4Codec_t	_cVideoCodecUsed;
			MP4Atom:: MP4Codec_t	_cAudioCodecUsed;
			unsigned long			_ulVideoAvgBitRate;
			unsigned long			_ulAudioAvgBitRate;
			Buffer_t				_bVideoRequestURI;
			Buffer_t				_bAudioRequestURI;
			Boolean_t				_bStreamingSessionTimeoutActive;
			// initialized in handleDESCRIBEMethod and handleSETUPMethod
			Boolean_t				_bVideoTrackInitializedFromSetup;
			Boolean_t				_bAudioTrackInitializedFromSetup;

			// only for off-line
			MP4RootAtom_p			_pmaRootAtom;
			MP4File_p				_pmfFile;
			Boolean_t				_bStartTimeInSecsParameter;
			long					_lStartTimeInSecsParameter;
			MP4TrackInfo_p			_pmtiVideoMediaTrackInfo;
			MP4TrackInfo_p			_pmtiAudioMediaTrackInfo;
			MP4TrackInfo_p			_pmtiVideoHintTrackInfo;
			MP4TrackInfo_p			_pmtiAudioHintTrackInfo;
			// _dMovieDuration will be -1 if the DESCRIBE is not called
			double					_dMovieDuration;

			// only for live
			unsigned long			_ulLiveVideoRTPPacketsServerPort;
			unsigned long			_ulLiveAudioRTPPacketsServerPort;
			Boolean_t				_bIsFirstLiveVideoRTPPacketToSend;
			Boolean_t				_bIsFirstLiveAudioRTPPacketToSend;
			unsigned short			_usPLAYAnswerVideoSequenceNumber;
			unsigned long			_ulPLAYAnswerVideoTimeStamp;
			unsigned short			_usPLAYAnswerAudioSequenceNumber;
			unsigned long			_ulPLAYAnswerAudioTimeStamp;
			unsigned short
				_usPLAYAnswerAndFirstPacketToSendVideoSequenceNumberDifference;
			unsigned long
				_ulPLAYAnswerAndFirstPacketToSendVideoTimeStampDifference;
			unsigned short
				_usPLAYAnswerAndFirstPacketToSendAudioSequenceNumberDifference;
			unsigned long
				_ulPLAYAnswerAndFirstPacketToSendAudioTimeStampDifference;


			//	<PRE>
			//
			//	</PRE>
			Error sendRTSPResponse (SocketImpl_p psiClientSocketImpl,
				const char *pRTSPVersion, unsigned long ulStatusCode,
				long lRTSPSequenceNumber, const char *pSessionIdentifier,
				unsigned long ulSessionTimeout, const char *pTimestamp,
				Boolean_t bConnectionClose);

			//	<PRE>
			//
			//	</PRE>
			Error getRequestLog (
				Error_p perrError,
				Buffer_p pbRequestLog);

			//	<PRE>
			//
			//	</PRE>
			virtual Error handleDESCRIBEMethod (const char *pRTSPRequest,
				unsigned long *pulMethodLength);

			//	<PRE>
			//
			//	</PRE>
			Error getInfoFromMP4File (
				unsigned long ulMaxPayloadSizeInBytes, MP4File_p pmfFile,
				MP4RootAtom_p *pmaRootAtom,
				Boolean_p pbVideoTrackFoundInSDP,
				MP4TrackInfo_p *pmtiVideoMediaTrackInfo,
				MP4TrackInfo_p *pmtiVideoHintTrackInfo,
				Boolean_p pbAudioTrackFoundInSDP,
				MP4TrackInfo_p *pmtiAudioMediaTrackInfo,
				MP4TrackInfo_p *pmtiAudioHintTrackInfo,
				Boolean_t bUseOfHintingTrackIfExist,
				Boolean_t bBuildOfHintingTrackIfNotExist);

			//	<PRE>
			//
			//	</PRE>
			Error choiceMediaTracksToStream (MP4File_p pmfFile,
				Boolean_p pbVideoChosen,
				MP4TrackInfo_p *pmtiVideoMediaTrackInfo,
				Boolean_p pbAudioChosen,
				MP4TrackInfo_p *pmtiAudioMediaTrackInfo);

			//	<PRE>
			//
			//	</PRE>
			Error handleANNOUNCEMethod (const char *pRTSPRequest,
				unsigned long *pulMethodLength);

			//	<PRE>
			//
			//	</PRE>
			Error handleGET_PARAMETERMethod (const char *pRTSPRequest,
				unsigned long *pulMethodLength);

			//	<PRE>
			//
			//	</PRE>
			Error handleOPTIONSMethod (const char *pRTSPRequest,
				unsigned long *pulMethodLength);

			//	<PRE>
			//
			//	</PRE>
			Error handlePAUSEMethod (const char *pRTSPRequest,
				unsigned long *pulMethodLength);

			//	<PRE>
			//
			//	</PRE>
			Error activeTrack (TrackInfo_p ptiTrackInfo);

			//	<PRE>
			//
			//	</PRE>
			Error deactiveTrack (TrackInfo_p ptiTrackInfo);

			//	<PRE>
			//
			//	</PRE>
			Error handlePLAYMethod (const char *pRTSPRequest,
				unsigned long *pulMethodLength,
				RTSPCommand_t rcPreviousRTSPCommand);

			//	<PRE>
			//
			//	</PRE>
			Error seek (
				double dRequestedStartTimeInSecs,
				double dMaxBackupMovieTime,
				double *pdEffectiveStartTimeInSecs);

			//	<PRE>
			//
			//	</PRE>
			Error getCurrentPacketRTPTimeStampAndSequenceNumber (
				unsigned long ulTrackIdentifier,
	unsigned long *pulRelativeRTPPacketTransmissionTimeWithTimeScaleAndOffset,
				unsigned short *pusSequenceNumber);

			//	<PRE>
			//
			//	</PRE>
			Error getSSRC (
				unsigned long ulTrackIdentifier,
				unsigned long *pulSSRC);

			//	<PRE>
			//
			//	</PRE>
			Error getRTPPacket (TrackInfo_p ptiTrackInfo,
				MP4TrackInfo_p pmtiTrakInfo,
				RTPHintPacket_p prhpRTPHintPacket,
				RTPPacket_p prpRTPPacket,
				double *pdRelativeRTPPacketTransmissionTimeInSecs,
				RTPStreamRealTimeInfo_p prsrtRTPMediaStreamRealTimeInfo,
				RTPStreamRealTimeInfo_p prsrtRTPHintStreamRealTimeInfo);

			//	<PRE>
			//
			//	</PRE>
			Error handleRECORDMethod (const char *pRTSPRequest,
				unsigned long *pulMethodLength);

			//	<PRE>
			//
			//	</PRE>
			Error handleREDIRECTMethod (const char *pRTSPRequest,
				unsigned long *pulMethodLength);

			//	<PRE>
			//
			//	</PRE>
			virtual Error handleSETUPMethod (const char *pRTSPRequest,
				unsigned long *pulMethodLength);

			//	<PRE>
			//
			//	</PRE>
			Error initRTPSession (void);

			//	<PRE>
			//
			//	</PRE>
			Error finishRTPSession (void);

			//	<PRE>
			//
			//	</PRE>
			Error addTrackToStream (
				Boolean_t bIsLive,
				Boolean_t bIsVideoSetup,
				MP4Atom:: MP4Codec_t cLiveCodecUsed,
				const char *pTrackURI,
				MP4TrackInfo_p pmtiTrackInfo,
				std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo,
				Boolean_t bIsHintTrack,
				unsigned long ulPayloadNumber,
				unsigned long ulRTPClientPort, unsigned long ulRTPServerPort,
				unsigned long ulRTCPClientPort, unsigned long ulRTCPServerPort,
				unsigned long *pulTrackIdentifier);

			//	<PRE>
			//
			//	</PRE>
			Error getAndRemoveNextRTPPacketToSend (
				double *pdRelativeRTPPacketTransmissionTimeInMilliSecs,
				unsigned long *pulTrackIdentifier,
				RTPPacket_p *prpRTPPacket);

			//	<PRE>
			//
			//	</PRE>
			Error prefetchNextPacketsUsingHintTrack (
				unsigned long ulTrackIdentifier,
				Boolean_t bIsCalledFromHandlePLAY,
				Boolean_p pbIsAPacketPrefetched);

			//	<PRE>
			//
			//	</PRE>
			Error prefetchNextPacketsUsingMediaTrack (
				unsigned long ulTrackIdentifier,
				Boolean_t bIsCalledFromHandlePLAY,
				Boolean_p pbIsAPacketPrefetched);

			//	<PRE>
			//
			//	</PRE>
			Error deleteTrackToStream (Boolean_t bIsLive,
				unsigned long ulTrackIdentifier);

			//	<PRE>
			//
			//	</PRE>
			Error handleSET_PARAMETERMethod (const char *pRTSPRequest,
				unsigned long *pulMethodLength);

			//	<PRE>
			//
			//	</PRE>
			/*
			Error parseURL (const char *pURL,
				char *pIPAddress, long *plRTSPPort,
				Buffer_p pbRelativePath);
			*/

			Error parseURLParameters (
				Boolean_t bIsLive, Buffer_p pbURLParameters,

				Boolean_p pbStartTimeInSecsParameter,
				long *plStartTimeInSecsParameter,

				StreamingSessionTimes_p pssStreamingSessionTimes,
				Boolean_p pbStreamingSessionTimeoutActive,
				Scheduler_p pscScheduler
				);

			//	<PRE>
			//
			//	</PRE>
			Error handleTEARDOWNMethod (const char *pRTSPRequest,
				unsigned long *pulMethodLength,
				Boolean_p pbIsRTSPFinished);

			//	<PRE>
			//
			//	</PRE>
			#ifdef WIN32
				Error sendRTCPSRPacket (
					unsigned long ulTrackIdentifier,
					__int64 llUTCTimeInMilliSecs,
					Boolean_t bEndOfPartecipation);
			#else
				Error sendRTCPSRPacket (
					unsigned long ulTrackIdentifier,
					long long llUTCTimeInMilliSecs,
					Boolean_t bEndOfPartecipation);
			#endif

			//	<PRE>
			//
			//	</PRE>
			#ifdef WIN32
				Error buildRTCPSRPacket (__int64 llUTCTimeInMilliSecs,
					TrackInfo_p ptiTrackInfo,
					Boolean_t bIsItRTCPBye, __int64 llPlayTimeInMilliSeconds,
					__int64 llNTPPlayTime);
			#else
				Error buildRTCPSRPacket (long long llUTCTimeInMilliSecs,
					TrackInfo_p ptiTrackInfo,
					Boolean_t bIsItRTCPBye, long long llPlayTimeInMilliSeconds,
					long long llNTPPlayTime);
			#endif

			//	<PRE>
			//
			//	</PRE>
			Error tryToReadAndProcessRTCPPackets (
				unsigned long ulTrackIdentifier,
				unsigned long *pulRTCPPacketsNumber);

			//	<PRE>
			//
			//	</PRE>
			Error processRTCPPacket (
				TrackInfo_p ptiTrackInfo,
				char *pRTCPPacket, long lRTCPPacketLength);

			//	<PRE>
			//
			//	</PRE>
			Error getRTSPStatus (RTSPStatus_p prsRTSPStatus);

			//	<PRE>
			//
			//	</PRE>
			Error setRTSPStatus (RTSPStatus_t rsRTSPStatus);

		public:
			//	<PRE>
			//
			//	</PRE>
			RTSPSession (void);

			//	<PRE>
			//
			//	</PRE>
			~RTSPSession (void);

			//	<PRE>
			//
			//	</PRE>
			Error init (
				unsigned long ulIdentifier,
				MP4Atom:: Standard_t sStandard,
				StreamingServerEventsSet_p pesEventsSet,
				StreamingServerSocketsPool_p pspSocketsPool,
				Scheduler_p pscScheduler,
				unsigned long ulRTSP_RTCPTimeoutInSecs,
				unsigned long ulPauseTimeoutInSecs,
				unsigned long ulFileCacheSizeInBytes,
				unsigned long ulSendRTCPPacketsPeriodInMilliSecs,
				unsigned long ulReceiveRTCPPacketsPeriodInMilliSecs,
				unsigned long ulSendRTPMaxSleepTimeInMilliSecs,
				const char *pLocalIPAddressForRTSP,
				const char *pLocalIPAddressForRTP,
				const char *pLocalIPAddressForRTCP,
				unsigned long ulRTSPRequestsPort,
				double dMaxSpeedAllowed,
				unsigned long ulPlayDelayTimeInMilliSeconds,
				Boolean_t bIsOverBufferEnabled,
				unsigned long ulSendingInterval,
				unsigned long ulInitialWindowSizeInBytes,
				unsigned long ulMaxSendAheadTimeInSec,
				float fOverbufferRate,
				const char *pContentRootPath,
				PMutex_p pmtLiveSources,
				LiveSource_p plsLiveSources,
				unsigned long ulMaxLiveSourcesNumber,
				unsigned long ulRTP_RTCPStartingPort,
				unsigned long ulRTP_RTCPFinishingPort,
				unsigned long ulMaxPayloadSizeInBytes,
				unsigned long ulRTPPacketsNumberToPrefetch,
				unsigned long ulSamplesNumberToPrefetch,
				Boolean_t bUseOfHintingTrackIfExist,
				Boolean_t bBuildOfHintingTrackIfNotExist,
				const char *pRequestLogFormat,
				MP4FileFactory_p pmp4fMP4FileFactory,
				Boolean_t bBeginningHookActivated,
				const char *pBeginningHookServletPathName,
				const char *pBeginningHookWebServerIpAddress,
				unsigned long ulBeginningHookWebServerPort,
				const char *pBeginningHookLocalIPAddressForHTTP,
				unsigned long ulBeginningHookTimeoutInSecs,
				Boolean_t bEndingHookActivated,
				const char *pEndingHookServletPathName,
				const char *pEndingHookWebServerIpAddress,
				unsigned long ulEndingHookWebServerPort,
				const char *pEndingHookLocalIPAddressForHTTP,
				unsigned long ulEndingHookTimeoutInSecs,
				Boolean_t bFlushTraceAfterEachRequest,
				Tracer_p ptSystemTracer,
				Tracer_p ptSubscriberTracer);

			//	<PRE>
			//
			//	</PRE>
			Error finish (void);

			//	<PRE>
			//
			//	</PRE>
			Error getIdentifier (unsigned long *pulIdentifier);

			//	<PRE>
			//
			//	</PRE>
			Error getRTSP_RTCPTimes (
				RTSP_RTCPTimes_p *prrtRTSP_RTCPTimes);

			//	<PRE>
			//
			//	</PRE>
			Error getPauseTimes (
				PauseTimes_p *pptPauseTimes);

			//	<PRE>
			//
			//	</PRE>
			Error getStreamingSessionTimes (
				StreamingSessionTimes_p *psstStreamingSessionTimes);

			//	<PRE>
			//
			//	</PRE>
			Error getCheckReceivedRTCPPacketsTimes (
				CheckReceivedRTCPPacketsTimes_p *pcrrptCheckReceivedRTCPPacketsTimes);

			//	<PRE>
			//
			//	</PRE>
			Error getSendRTCPPacketsTimes (
				SendRTCPPacketsTimes_p *psrptSendRTCPPacketsTimes);

			//	<PRE>
			//
			//	</PRE>
			Error getVideoTrackIdentifier (
				unsigned long *pulTrackIdentifier);

			//	<PRE>
			//
			//	</PRE>
			Error getAudioTrackIdentifier (
				unsigned long *pulTrackIdentifier);

			//	<PRE>
			//
			//	</PRE>
			Error getRandomOffsets (
				unsigned long ulTrackIdentifier,
				unsigned short *pusRtpSequenceNumberRandomOffset,
				unsigned long *pulRtpTimestampRandomOffset);

			//	<PRE>
			//
			//	</PRE>
			Error acceptRTSPConnection (ServerSocket_p pssServerSocket);

			//	<PRE>
			//
			//	</PRE>
			Error getClientSocket (ClientSocket_p *pcsClientSocket);

			//	<PRE>
			//
			//	</PRE>
			#ifdef WIN32
				Error processRTSPRequest (
					__int64 *pullTotalBytesServed,
					__int64 *pullTotalLostPacketsNumber,
					__int64 *pullTotalSentRTPPacketsNumber,
					Boolean_p pbIsRTSPFinished);
			#else
				Error processRTSPRequest (
					unsigned long long *pullTotalBytesServed,
					unsigned long long *pullTotalLostPacketsNumber,
					unsigned long long *pullTotalSentRTPPacketsNumber,
					Boolean_p pbIsRTSPFinished);
			#endif

			//	<PRE>
			//
			//	</PRE>
			Error handleDESCRIBEResponse (
				unsigned long ulSequenceNumber,
				long lEventTypeIdentifier, const char *pErrorOrMessage);

			//	<PRE>
			//
			//	</PRE>
			Error addRTSPSessionForTheLiveSource (void);

			//	<PRE>
			//
			//	</PRE>
			Error deleteRTSPSessionForTheLiveSource (void);

			//	<PRE>
			//
			//	</PRE>
			Error sendLiveRTPPacket (
				Boolean_t bIsVideoRTPPacket,
				unsigned short usRTPPacketSequenceNumberFromLiveEncoder,
				unsigned long ulRTPPacketTimeStampFromLiveEncoder,
				RTPPacket_p prpLiveRTPPacket);

			//	<PRE>
			//
			//	De-alloc and initialize again all
			//		the RTSP session structures in order
			//		to handle a new RTSP session. This method
			//		prepare all the structures as they were
			//		after the init method.
			//		This method is called in cases of both
			//		error and normal closing the connection
			//
			//	</PRE>
			#ifdef WIN32
				Error closeRTSPConnection (
					Error_p perrError,
					__int64 *pullTotalBytesServed,
					__int64 *pullTotalLostPacketsNumber,
					__int64 *pullTotalSentRTPPacketsNumber);
			#else
				Error closeRTSPConnection (
					Error_p perrError,
					unsigned long long *pullTotalBytesServed,
					unsigned long long *pullTotalLostPacketsNumber,
					unsigned long long *pullTotalSentRTPPacketsNumber);
			#endif

			//	<PRE>
			//
			//	</PRE>
			Error sendRTPPackets (
				unsigned long *pulVideoRTPPacketsNumberSent,
				unsigned long *pulAudioRTPPacketsNumberSent);

			//	<PRE>
			//
			//	</PRE>
			#ifdef WIN32
				Error sendRTPPacket (
					double dAbsoluteRTPPacketTransmissionTimeInMilliSecs,
					unsigned long ulTrackIdentifier,
					RTPPacket_p prpRTPPacket,
					__int64 *pullSuggestedWakeupTimeInMilliSecs);
			#else
				Error sendRTPPacket (
					double dAbsoluteRTPPacketTransmissionTimeInMilliSecs,
					unsigned long ulTrackIdentifier,
					RTPPacket_p prpRTPPacket,
					unsigned long long *pullSuggestedWakeupTimeInMilliSecs);
			#endif

			//	<PRE>
			//
			//	</PRE>
			Error handleReceiverRTCPPackets (
				unsigned long *pulReceivedRTCPPacketsNumber);

			//	<PRE>
			//
			//	</PRE>
			Error handleSenderRTCPPackets (void);

			//	<PRE>
			//
			//	</PRE>
			Error prefetchNextPackets (
				Boolean_t bIsCalledFromHandlePLAY,
				Boolean_p pbIsAPacketPrefetched);

			#ifdef WIN32
				Error getFinalInfo (
					__int64 *pullTotalBytesServed,
					__int64 *pullTotalLostPacketsNumber,
					__int64 *pullTotalSentRTPPacketsNumber);
			#else
				Error getFinalInfo (
					unsigned long long *pullTotalBytesServed,
					unsigned long long *pullTotalLostPacketsNumber,
					unsigned long long *pullTotalSentRTPPacketsNumber);
			#endif

			//	<PRE>
			//
			//	</PRE>
			Error getAvgBitRate (
				unsigned long *pulAudioBitRate,
				unsigned long *pulVideoBitRate);

			//	<PRE>
			//
			//	</PRE>
			/*
			Error prefetchLivePacket (
				unsigned char *pucRTPPacket,
				unsigned long ulPacketLength,
				Boolean_t bIsVideoTrack,
				Boolean_p pbFreeRTPPacketFinished);
			*/

			//	<PRE>
			//
			//	</PRE>
			#ifdef WIN32
				Error getInfo (
					char *pClientIPAddress,
					Buffer_p pbRelativePathWithoutParameters,
					Buffer_p pbURLParameters,
					MP4Atom:: Standard_p psStandard,
					Boolean_p pbIsLive,
					double *pdDuration,
					MP4Atom:: MP4Codec_p pcVideoCodec,
					unsigned long *pulVideoPacketsNumberSent,
					__int64 *pullVideoBytesSent,
					unsigned long *pulVideoPacketsNumberLost,
					unsigned long *pulVideoAverageBitRate,
					unsigned long *pulVideoRTCPPacketsNumberReceived,
					__int64 *pullVideoRTCPBytesReceived,
					unsigned long *pulVideoJitter,
					MP4Atom:: MP4Codec_p pcAudioCodec,
					unsigned long *pulAudioPacketsNumberSent,
					__int64 *pullAudioBytesSent,
					unsigned long *pulAudioPacketsNumberLost,
					unsigned long *pulAudioAverageBitRate,
					unsigned long *pulAudioRTCPPacketsNumberReceived,
					__int64 *pullAudioRTCPBytesReceived,
					unsigned long *pulAudioJitter,
					__int64 *pullConnectedTimeInSeconds);
			#else
				Error getInfo (
					char *pClientIPAddress,
					Buffer_p pbRelativePathWithoutParameters,
					Buffer_p pbURLParameters,
					MP4Atom:: Standard_p psStandard,
					Boolean_p pbIsLive,
					double *pdDuration,
					MP4Atom:: MP4Codec_p pcVideoCodec,
					unsigned long *pulVideoPacketsNumberSent,
					unsigned long long *pullVideoBytesSent,
					unsigned long *pulVideoPacketsNumberLost,
					unsigned long *pulVideoAverageBitRate,
					unsigned long *pulVideoRTCPPacketsNumberReceived,
					unsigned long long *pullVideoRTCPBytesReceived,
					unsigned long *pulVideoJitter,
					MP4Atom:: MP4Codec_p pcAudioCodec,
					unsigned long *pulAudioPacketsNumberSent,
					unsigned long long *pullAudioBytesSent,
					unsigned long *pulAudioPacketsNumberLost,
					unsigned long *pulAudioAverageBitRate,
					unsigned long *pulAudioRTCPPacketsNumberReceived,
					unsigned long long *pullAudioRTCPBytesReceived,
					unsigned long *pulAudioJitter,
					unsigned long long *pullConnectedTimeInSeconds);
			#endif

			//	<PRE>
			//
			//	</PRE>
			Error getAverageBandWidthUsage (
				unsigned long *pulAverageBandWidthUsage);

			//	<PRE>
			//
			//	</PRE>
			static Error isLiveRequest (const char *pAssetPath,
				Boolean_p pbIsLive);

	} RTSPSession_t, *RTSPSession_p;

#endif
