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

#include "RTSPSession.h"
#include "StringTokenizer.h"
#include "FileIO.h"
#include "Convert.h"
#include "MP4ElstAtom.h"
#include "MP4HintAtom.h"
#include "MP4InitialObjectDescr.h"
#include "MP4MvhdAtom.h"
#include "MP4SnroAtom.h"
#include "MP4StszAtom.h"
#include "MP4TkhdAtom.h"
#include "MP4TimsAtom.h"
#include "MP4TsroAtom.h"
#include "RTPUtility.h"
#include "RTSPUtility.h"
#include "aac.h"
#include "rfcisma.h"
#include "rfc2429.h"
#include "rfc3016.h"
#include "mpeg4.h"
#include "rfc3267.h"
#include "StreamingServerMessages.h"
#include "MP4FileMessages.h"

#include "RTSPConnectionEvent.h"

#include "RTCPPacket.h"
#include "RTCPSRPacket.h"
#include "RTCPAckPacket.h"
#include "RTCPAPPPacket.h"

#include "SDPMediaFor3GPP.h"

#ifdef WIN32
	#include "Windows.h"
#endif

#include "DateTime.h"
#include "BeginningHookHttpGetThread.h"


RTSPSession:: RTSPSession (void)

{

}


RTSPSession:: ~RTSPSession (void)

{

}


Error RTSPSession:: init (
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
	Tracer_p ptSubscriberTracer)

{

	unsigned long					ulTrackIdentifier;
	long							lRTPPacketIndex;
	TrackInfo_p						ptiTrackInfo;


	_ulIdentifier					= ulIdentifier;
	_sStandard						= sStandard;
	_pesEventsSet					= pesEventsSet;
	_pspSocketsPool					= pspSocketsPool;
	_pscScheduler					= pscScheduler;
	_ulRTSP_RTCPTimeoutInSecs		= ulRTSP_RTCPTimeoutInSecs;
	_ulPauseTimeoutInSecs			= ulPauseTimeoutInSecs;
	_ulFileCacheSizeInBytes			=
		ulFileCacheSizeInBytes;
	_ulSendRTCPPacketsPeriodInMilliSecs			=
		ulSendRTCPPacketsPeriodInMilliSecs;
	_ulReceiveRTCPPacketsPeriodInMilliSecs	=
		ulReceiveRTCPPacketsPeriodInMilliSecs;
	strcpy (_pLocalIPAddressForRTSP, pLocalIPAddressForRTSP);
	strcpy (_pLocalIPAddressForRTP, pLocalIPAddressForRTP);
	strcpy (_pLocalIPAddressForRTCP, pLocalIPAddressForRTCP);
	_ulRTSPRequestsPort					= ulRTSPRequestsPort;
	_dMaxSpeedAllowed				= dMaxSpeedAllowed;
	_rsRTPSession. _ulSendRTPMaxSleepTimeInMilliSecs	=
		ulSendRTPMaxSleepTimeInMilliSecs;
	_ulPlayDelayTimeInMilliSeconds			=
		ulPlayDelayTimeInMilliSeconds;
	_bIsOverBufferEnabled					=
		bIsOverBufferEnabled;
	_ulSendingInterval						=
		ulSendingInterval;
	_ulInitialWindowSizeInBytes				=
		ulInitialWindowSizeInBytes;
	_fOverbufferRate						=
		fOverbufferRate;
	_ulMaxSendAheadTimeInSec				=
		ulMaxSendAheadTimeInSec;

	_pmtLiveSources					= pmtLiveSources;
	_plsLiveSources					= plsLiveSources;
	_ulMaxLiveSourcesNumber			= ulMaxLiveSourcesNumber;

	_ulReservedStartingPort			= ulRTP_RTCPStartingPort;
	_ulReservedFinishingPort		= ulRTP_RTCPFinishingPort;
	_ulMaxPayloadSizeInBytes		= ulMaxPayloadSizeInBytes;
	_rsRTPSession. _ulCurrentMaxPayloadSizeInBytes		=
		_ulMaxPayloadSizeInBytes;

	_ulRTPPacketsNumberToPrefetch	=
		ulRTPPacketsNumberToPrefetch;
	_ulSamplesNumberToPrefetch		=
		ulSamplesNumberToPrefetch;

	//	During the prefetching of the RTP packets we consider the following
	//	configuration parameters: _ulRTPPacketsNumberToPrefetch for some rfc and
	//	_ulSamplesNumberToPrefetch for other rfc. Basically, for some rfc
	//	the server prefetches max _ulRTPPacketsNumberToPrefetch RTP packets and
	//	for other rfc the server prefetches max _ulSamplesNumberToPrefetch
	//	samples.

	//	The streaming server must pre-allocates the RTP packets to be used for
	//	prefetching. For the rfc using _ulSamplesNumberToPrefetch I need
	//	to preallocate the RTP packets for that samples.
	//  Obvious, the RTP packets necessary for one sample
	//	depends from the size of the sample.
	//  Since I observed that with a bitrate of 1000Kbps, the maximum
	//	sample size
	//	was around 60KB, the maximum number of RTP packets necessary
	//	in that case, considering
	//	a max packet size of 1472 bytes (payload), is 41 (= 60000 / 1472).
	//	In our case we guess 50 RTP packets for the bigger sample.
	//	Therefore:
	if (_ulRTPPacketsNumberToPrefetch >
		_ulSamplesNumberToPrefetch * 50)
		_ulRTPPacketsNumberPreallocated	=
			_ulRTPPacketsNumberToPrefetch;
	else
		_ulRTPPacketsNumberPreallocated	=
			_ulSamplesNumberToPrefetch * 50;

	_bUseOfHintingTrackIfExist		= bUseOfHintingTrackIfExist;
	_bBuildOfHintingTrackIfNotExist	= bBuildOfHintingTrackIfNotExist;
	_pmp4fMP4FileFactory			= pmp4fMP4FileFactory;
	_bFlushTraceAfterEachRequest	= bFlushTraceAfterEachRequest;
	_ptSystemTracer					= ptSystemTracer;
	_ptSubscriberTracer				= ptSubscriberTracer;


	// integration variable
	_bBeginningHookActivated		= bBeginningHookActivated;
	strcpy (_pBeginningHookWebServerIpAddress,
		pBeginningHookWebServerIpAddress);
	_ulBeginningHookWebServerPort	= ulBeginningHookWebServerPort;
	strcpy (_pBeginningHookLocalIPAddressForHTTP,
		pBeginningHookLocalIPAddressForHTTP);
	_ulBeginningHookTimeoutInSecs	= ulBeginningHookTimeoutInSecs;
	_bEndingHookActivated				= bEndingHookActivated;
	strcpy (_pEndingHookWebServerIpAddress,
		pEndingHookWebServerIpAddress);
	_ulEndingHookWebServerPort			= ulEndingHookWebServerPort;
	strcpy (_pEndingHookLocalIPAddressForHTTP,
		pEndingHookLocalIPAddressForHTTP);
	_ulEndingHookTimeoutInSecs			= ulEndingHookTimeoutInSecs;

	_rsRTSPStatus					= SS_RTSP_NOTUSED;
	_ulCurrentPortForSetup			= ulRTP_RTCPStartingPort;
	_pmfFile						= (MP4File_p) NULL;
	_dMovieDuration					= -1;
	_ulVideoTrackIdentifier			= SS_RTSPSESSION_MAXTRACKSNUMBER;
	_ulAudioTrackIdentifier			= SS_RTSPSESSION_MAXTRACKSNUMBER;

	{
		const char					*pBeginningOfTheParameters;


		if ((pBeginningOfTheParameters = strchr (
			pBeginningHookServletPathName,
			'?')) != (const char *) NULL)
		{
			if (_bBeginningHookServletPathName. init (
				pBeginningHookServletPathName,
				pBeginningOfTheParameters -
				pBeginningHookServletPathName) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (_bBeginningHookServletParameters. init (
				pBeginningOfTheParameters + 1) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_bBeginningHookServletPathName. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (_bBeginningHookServletPathName. init (
				pBeginningHookServletPathName) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (_bBeginningHookServletParameters. init () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_bBeginningHookServletPathName. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (_bBeginningHookServletPathName. substitute (
			"%61", "=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_bBeginningHookServletParameters. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bBeginningHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if ((pBeginningOfTheParameters = strchr (
			pEndingHookServletPathName,
			'?')) != (const char *) NULL)
		{
			if (_bEndingHookServletPathName. init (
				pEndingHookServletPathName,
				pBeginningOfTheParameters -
				pEndingHookServletPathName) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_bBeginningHookServletParameters. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_bBeginningHookServletPathName. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (_bEndingHookServletParameters. init (
				pBeginningOfTheParameters + 1) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_bEndingHookServletPathName. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_bBeginningHookServletParameters. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_bBeginningHookServletPathName. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (_bEndingHookServletPathName. init (
				pEndingHookServletPathName) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_bBeginningHookServletParameters. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_bBeginningHookServletPathName. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (_bEndingHookServletParameters. init () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_bEndingHookServletPathName. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_bBeginningHookServletParameters. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_bBeginningHookServletPathName. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (_bEndingHookServletPathName. substitute (
			"%61", "=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_bEndingHookServletParameters. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bEndingHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bBeginningHookServletParameters. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bBeginningHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	{
		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			ptiTrackInfo -> _bTrackInitialized		= false;

			if ((ptiTrackInfo -> _prpRTPPackets = new RTPPacket_t [
				_ulRTPPacketsNumberPreallocated]) == (RTPPacket_p) NULL)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_NEW_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_bEndingHookServletParameters. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_bEndingHookServletPathName. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_bBeginningHookServletParameters. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_bBeginningHookServletPathName. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			(ptiTrackInfo -> _vFreeRTPPackets). reserve (
				_ulRTPPacketsNumberPreallocated);
			(ptiTrackInfo -> _vBuiltRTPPackets). reserve (
				_ulRTPPacketsNumberPreallocated);
			(ptiTrackInfo -> _vTransmissionTimeBuiltRTPPackets). reserve (
				_ulRTPPacketsNumberPreallocated);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [lRTPPacketIndex]). init (
					_ulMaxPayloadSizeInBytes,
					_ptSystemTracer) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					while (--lRTPPacketIndex >= 0)
					{
						if (((ptiTrackInfo -> _prpRTPPackets) [
							lRTPPacketIndex]). finish () != errNoError)
						{
							Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RTPPACKET_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}
					}

					delete [] (ptiTrackInfo -> _prpRTPPackets);
					ptiTrackInfo -> _prpRTPPackets		= (RTPPacket_p) NULL;


					if (_bEndingHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_bEndingHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_bBeginningHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_bBeginningHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				(ptiTrackInfo -> _vFreeRTPPackets). insert (
					(ptiTrackInfo -> _vFreeRTPPackets). end (),
					&((ptiTrackInfo -> _prpRTPPackets) [lRTPPacketIndex]));
			}
		}
	}

	if (_bRequestLogFormat. init (pRequestLogFormat) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_rrtRTSP_RTCPTimes. init (_ulRTSP_RTCPTimeoutInSecs * 1000,
		_ulIdentifier, _pesEventsSet, _ptSystemTracer) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSP_RTCPTIMES_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bRequestLogFormat. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_ptPauseTimes. init (ulPauseTimeoutInSecs * 1000,
		_ulIdentifier, _pesEventsSet, _ptSystemTracer) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_PAUSETIMES_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_rrtRTSP_RTCPTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSP_RTCPTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestLogFormat. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_crrptCheckReceivedRTCPPacketsTimes. init (
		_ulReceiveRTCPPacketsPeriodInMilliSecs,
		_ulIdentifier, _pesEventsSet, _ptSystemTracer) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_CHECKRECEIVEDRTCPPACKETSTIMES_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_ptPauseTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_PAUSETIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_rrtRTSP_RTCPTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSP_RTCPTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestLogFormat. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_srptSendRTCPPacketsTimes. init (
		_ulSendRTCPPacketsPeriodInMilliSecs,
		_ulIdentifier, _pesEventsSet, _ptSystemTracer) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_SENDRTCPPACKETSTIMES_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_crrptCheckReceivedRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CHECKRECEIVEDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_ptPauseTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_PAUSETIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_rrtRTSP_RTCPTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSP_RTCPTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestLogFormat. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_csClientSocket. init () != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_srptSendRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_SENDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_crrptCheckReceivedRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CHECKRECEIVEDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_ptPauseTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_PAUSETIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_rrtRTSP_RTCPTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSP_RTCPTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestLogFormat. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bRelativePathWithoutParameters. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_csClientSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_srptSendRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_SENDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_crrptCheckReceivedRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CHECKRECEIVEDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_ptPauseTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_PAUSETIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_rrtRTSP_RTCPTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSP_RTCPTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestLogFormat. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bURLParameters. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bRelativePathWithoutParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_srptSendRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_SENDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_crrptCheckReceivedRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CHECKRECEIVEDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_csClientSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_ptPauseTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_PAUSETIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_rrtRTSP_RTCPTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSP_RTCPTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestLogFormat. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bBeginningHookMessage. init ("NA") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bURLParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRelativePathWithoutParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_srptSendRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_SENDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_crrptCheckReceivedRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CHECKRECEIVEDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_csClientSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_ptPauseTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_PAUSETIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_rrtRTSP_RTCPTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSP_RTCPTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestLogFormat. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bAssetPath. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bBeginningHookMessage. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bURLParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRelativePathWithoutParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_srptSendRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_SENDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_crrptCheckReceivedRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CHECKRECEIVEDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_csClientSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_ptPauseTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_PAUSETIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_rrtRTSP_RTCPTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSP_RTCPTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestLogFormat. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bUserAgent. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bAssetPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookMessage. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bURLParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRelativePathWithoutParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_srptSendRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_SENDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_crrptCheckReceivedRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CHECKRECEIVEDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_csClientSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_ptPauseTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_PAUSETIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_rrtRTSP_RTCPTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSP_RTCPTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestLogFormat. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bVideoRequestURI. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bUserAgent. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bAssetPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookMessage. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bURLParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRelativePathWithoutParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_srptSendRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_SENDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_crrptCheckReceivedRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CHECKRECEIVEDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_csClientSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_ptPauseTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_PAUSETIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_rrtRTSP_RTCPTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSP_RTCPTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestLogFormat. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bAudioRequestURI. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bVideoRequestURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bUserAgent. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bAssetPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookMessage. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bURLParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRelativePathWithoutParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_srptSendRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_SENDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_crrptCheckReceivedRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CHECKRECEIVEDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_csClientSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_ptPauseTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_PAUSETIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_rrtRTSP_RTCPTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSP_RTCPTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestLogFormat. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtRTSPSession. init (PMutex:: MUTEX_RECURSIVE) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bAudioRequestURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bVideoRequestURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bUserAgent. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bAssetPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookMessage. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bURLParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRelativePathWithoutParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_srptSendRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_SENDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_crrptCheckReceivedRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CHECKRECEIVEDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_csClientSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_ptPauseTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_PAUSETIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_rrtRTSP_RTCPTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSP_RTCPTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestLogFormat. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bContentRootPath. init (pContentRootPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bAudioRequestURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bVideoRequestURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bUserAgent. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bAssetPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookMessage. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bURLParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRelativePathWithoutParameters. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_srptSendRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_SENDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_crrptCheckReceivedRTCPPacketsTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CHECKRECEIVEDRTCPPACKETSTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_csClientSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_ptPauseTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_PAUSETIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_rrtRTSP_RTCPTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSP_RTCPTIMES_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestLogFormat. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();
		}

		if (_bEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	#ifdef RTPOVERBUFFERWINDOW
	#elif RTPOVERBUFFERWINDOW2
		if ((_rsRTPSession. _robwRTPOverBufferWindow2).
			init (_ulSendingInterval, _ulInitialWindowSizeInBytes,
			_ulMaxSendAheadTimeInSec, _fOverbufferRate) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTPOVERBUFFERWINDOW_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_bContentRootPath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtRTSPSession. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bAudioRequestURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bVideoRequestURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bUserAgent. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bAssetPath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bBeginningHookMessage. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bURLParameters. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bRelativePathWithoutParameters. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_srptSendRTCPPacketsTimes. finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_SENDRTCPPACKETSTIMES_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_crrptCheckReceivedRTCPPacketsTimes. finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_CHECKRECEIVEDRTCPPACKETSTIMES_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_csClientSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_ptPauseTimes. finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_PAUSETIMES_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_rrtRTSP_RTCPTimes. finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSP_RTCPTIMES_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bRequestLogFormat. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			for (ulTrackIdentifier = 0;
				ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
				ulTrackIdentifier++)
			{
				ptiTrackInfo							=
					&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

				for (lRTPPacketIndex = 0;
					lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
					lRTPPacketIndex++)
				{
					if (((ptiTrackInfo -> _prpRTPPackets) [
						lRTPPacketIndex]). finish () != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPPACKET_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				delete [] (ptiTrackInfo -> _prpRTPPackets);
				ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;

				(ptiTrackInfo -> _vFreeRTPPackets). clear ();
			}

			if (_bEndingHookServletParameters. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bEndingHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bBeginningHookServletParameters. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bBeginningHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_bIsOverBufferEnabled)
			(_rsRTPSession. _robwRTPOverBufferWindow2). turnOnOverbuffering ();
		else
			(_rsRTPSession. _robwRTPOverBufferWindow2). turnOffOverbuffering ();
	#else
	#endif


	return errNoError;
}


Error RTSPSession:: finish (void)

{

	if (_bContentRootPath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_mtRTSPSession. finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bAudioRequestURI. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bVideoRequestURI. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bUserAgent. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bAssetPath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bBeginningHookMessage. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bURLParameters. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bRelativePathWithoutParameters. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_srptSendRTCPPacketsTimes. finish () != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_SENDRTCPPACKETSTIMES_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_crrptCheckReceivedRTCPPacketsTimes. finish () != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_CHECKRECEIVEDRTCPPACKETSTIMES_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_csClientSocket. finish () != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_ptPauseTimes. finish () != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_PAUSETIMES_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_rrtRTSP_RTCPTimes. finish () != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSP_RTCPTIMES_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bRequestLogFormat. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	{
		unsigned long					ulTrackIdentifier;
		long							lRTPPacketIndex;
		TrackInfo_p						ptiTrackInfo;


		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			ptiTrackInfo							=
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]);

			for (lRTPPacketIndex = 0;
				lRTPPacketIndex < (long) (_ulRTPPacketsNumberPreallocated);
				lRTPPacketIndex++)
			{
				if (((ptiTrackInfo -> _prpRTPPackets) [
					lRTPPacketIndex]). finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			(ptiTrackInfo -> _vFreeRTPPackets). clear ();

			delete [] (ptiTrackInfo -> _prpRTPPackets);
			ptiTrackInfo -> _prpRTPPackets			= (RTPPacket_p) NULL;
		}
	}

	if (_bEndingHookServletParameters. finish () !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bEndingHookServletPathName. finish () !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bBeginningHookServletParameters. finish () !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bBeginningHookServletPathName. finish () !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}


	return errNoError;
}


Error RTSPSession:: handleDESCRIBEMethod (const char *pRTSPRequest,
	unsigned long *pulMethodLength)

{

	char							pRTSPServerIPAddress [
		SCK_MAXIPADDRESSLENGTH];
	long							lRTSPServerPort;
	StringTokenizer_t				stSpaceTokenizer;
	long							lSequenceNumber;
	char							pTimestamp [SS_MAXLONGLENGTH];
	RTSPStatus_t					rsRTSPStatus;
	unsigned long					ulRequestedPayloadSizeInBytes;
	Error_t							errParseRTSP;


	// no effect on the server state

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			"", false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "DESCRIBE", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			"", false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((errParseRTSP = RTSPUtility:: parseDESCRIBERequest (
		pRTSPRequest,

		pRTSPServerIPAddress,
		SCK_MAXIPADDRESSLENGTH,
		&lRTSPServerPort,
		&_bRelativePathWithoutParameters,
		&_bURLParameters,
		(Buffer_p) NULL,
		&_bUserAgent,
		&lSequenceNumber,
		pTimestamp,
		&ulRequestedPayloadSizeInBytes,
		pulMethodLength,
		_ptSystemTracer)) != errNoError)
	{
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errParseRTSP, __FILE__, __LINE__);

		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_PARSEDESCRIBEREQUEST_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// initialize _bAssetPath
	{
		if (_bAssetPath. setBuffer (
			(const char *) _bContentRootPath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_bAssetPath. append (
			(const char *) _bRelativePathWithoutParameters) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		#ifdef WIN32
			// Since the URL contains a path with '/', we must convert them to '\'
			if (_bAssetPath. substitute ("/", "\\") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		#endif
	}

	if (ulRequestedPayloadSizeInBytes == 0 || // no indication on max payload size
		ulRequestedPayloadSizeInBytes > _ulMaxPayloadSizeInBytes)
	{
		if (ulRequestedPayloadSizeInBytes != 0)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_REQUESTEDPAYLOADSIZETOOBIG,
				2, ulRequestedPayloadSizeInBytes, _ulMaxPayloadSizeInBytes);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		_rsRTPSession. _ulCurrentMaxPayloadSizeInBytes		=
			_ulMaxPayloadSizeInBytes;
	}
	else
		_rsRTPSession. _ulCurrentMaxPayloadSizeInBytes		=
			ulRequestedPayloadSizeInBytes;

	if (_bBeginningHookActivated)
	{
		BeginningHookHttpGetThread_p	phgHTTPBeginningHook;
		Buffer_t				bLocalBeginningHookServletPathName;
		Buffer_t				bLocalBeginningHookServletParameters;


		if (bLocalBeginningHookServletPathName. init (
			(const char *) _bBeginningHookServletPathName) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
				pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bLocalBeginningHookServletParameters. init (
			(const char *) _bBeginningHookServletParameters) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bLocalBeginningHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
				pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (strstr ((const char *) bLocalBeginningHookServletParameters,
			"$PlayerURL$") != (char *) NULL)
		{
			if (strcmp ((const char *) _bURLParameters, ""))
			{
				Buffer_t				bPlayerURL;


				if (bPlayerURL. init (
					(const char *) _bRelativePathWithoutParameters) !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bLocalBeginningHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalBeginningHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
						pTimestamp, false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				// use of '@' instead of '?'
				if (bPlayerURL. append ("@") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bPlayerURL. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalBeginningHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalBeginningHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
						pTimestamp, false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (bPlayerURL. append (
					(const char *) _bURLParameters) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bPlayerURL. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalBeginningHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalBeginningHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
						pTimestamp, false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (bPlayerURL. substitute ("=", "#") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bPlayerURL. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalBeginningHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalBeginningHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
						pTimestamp, false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (bPlayerURL. substitute ("&", "~") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bPlayerURL. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalBeginningHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalBeginningHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
						pTimestamp, false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (bLocalBeginningHookServletParameters. substitute (
					"$PlayerURL$",
					(const char *) bPlayerURL) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bPlayerURL. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalBeginningHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalBeginningHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
						pTimestamp, false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (bPlayerURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bLocalBeginningHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalBeginningHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
						pTimestamp, false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				if (bLocalBeginningHookServletParameters. substitute (
					"$PlayerURL$",
					(const char *) _bRelativePathWithoutParameters) !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bLocalBeginningHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalBeginningHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
						pTimestamp, false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
		}

		if (bLocalBeginningHookServletParameters. substitute (
			"$PlayerIP$", _pClientIPAddress) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bLocalBeginningHookServletParameters. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalBeginningHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
				pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// ask for the authorization
		if ((phgHTTPBeginningHook = new BeginningHookHttpGetThread_t) ==
			(BeginningHookHttpGetThread_p) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_NEW_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bLocalBeginningHookServletParameters. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalBeginningHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
				pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_HTTPGETREQUEST,
				5, _ulIdentifier, _pBeginningHookWebServerIpAddress,
				_ulBeginningHookWebServerPort,
				(const char *) bLocalBeginningHookServletPathName,
				(const char *) bLocalBeginningHookServletParameters);
			_ptSystemTracer -> trace (Tracer:: TRACER_LINFO,
				(const char *) msg, __FILE__, __LINE__);
		}

		// The GET HTTP will be:
		//	_bBeginningHookServletPathName + bBeginningHookServletParameters
		if (phgHTTPBeginningHook -> init (_ulIdentifier,
			_pBeginningHookWebServerIpAddress, _ulBeginningHookWebServerPort,
			_pBeginningHookLocalIPAddressForHTTP,
			&bLocalBeginningHookServletPathName,
			&bLocalBeginningHookServletParameters,
			_ulBeginningHookTimeoutInSecs,
			_pesEventsSet, _ptSystemTracer, lSequenceNumber) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_BEGINNINGHOOKHTTPGETTHREAD_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete phgHTTPBeginningHook;

			if (bLocalBeginningHookServletParameters. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalBeginningHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
				pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bLocalBeginningHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (phgHTTPBeginningHook -> finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_BEGINNINGHOOKHTTPGETTHREAD_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete phgHTTPBeginningHook;

			if (bLocalBeginningHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
				pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bLocalBeginningHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (phgHTTPBeginningHook -> finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_BEGINNINGHOOKHTTPGETTHREAD_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete phgHTTPBeginningHook;

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
				pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (phgHTTPBeginningHook -> start () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_START_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (phgHTTPBeginningHook -> finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_BEGINNINGHOOKHTTPGETTHREAD_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete phgHTTPBeginningHook;

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
				pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (phgHTTPBeginningHook -> detach () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_DETACH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (phgHTTPBeginningHook -> cancel () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_CANCEL_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (phgHTTPBeginningHook -> finish () != errNoError)
			{
				Error err = WebToolsErrors (__FILE__, __LINE__,
					WEBTOOLS_HTTPGETTHREAD_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete phgHTTPBeginningHook;

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
				pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		if (handleDESCRIBEResponse (lSequenceNumber,
			SS_EVENT_AUTHORIZATIONGRANTED,
			SS_RTSPSESSION_AUTHORIZATIONDISABLEDMESSAGE) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_HANDLEDESCRIBERESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			/*
			sendRTSPResponse is called inside the handleDESCRIBEResponse method
			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
				pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
			*/

			return err;
		}
	}


	return errNoError;
}


Error RTSPSession:: handleDESCRIBEResponse (
	unsigned long ulSequenceNumber,
	long lEventTypeIdentifier, const char *pErrorOrMessage)

{

	Buffer_t				bResponse;
	Buffer_t				bSDP;
	Error					errWrite;
	char					pTimestamp [SS_MAXLONGLENGTH];
	Error_t					errGetInfoFromMP4File;
	Error_t					errGetMP4File;
	Boolean_t				bIsMP4FileInCache;
	RTSPStatus_t			rsRTSPStatus;


	// no effect on the server state

	strcpy (pTimestamp, "");

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, ulSequenceNumber,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			pTimestamp, false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "DESCRIBE", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, ulSequenceNumber,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			pTimestamp, false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bBeginningHookMessage. setBuffer (pErrorOrMessage) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 400, ulSequenceNumber,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			pTimestamp, false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (lEventTypeIdentifier == SS_EVENT_AUTHORIZATIONERROR)
	{
		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			pTimestamp, false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}
	else if (lEventTypeIdentifier == SS_EVENT_AUTHORIZATIONDENIED)
	{
		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 401, ulSequenceNumber,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			pTimestamp, false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}
	// else if (lEventTypeIdentifier == SS_EVENT_AUTHORIZATIONGRANTED)

	if (isLiveRequest ((const char *) _bAssetPath,
		&_bIsLive) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_ISLIVEREQUEST_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 400, ulSequenceNumber,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bIsLive)
	{
		_bIsFirstLiveVideoRTPPacketToSend			= true;
		_bIsFirstLiveAudioRTPPacketToSend			= true;
	}

	if (parseURLParameters (_bIsLive, &_bURLParameters,

		&_bStartTimeInSecsParameter, &_lStartTimeInSecsParameter,

		&_sstStreamingSessionTimes,
		&_bStreamingSessionTimeoutActive,
		_pscScheduler
		) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_PARSEURLPARAMETERS_FAILED,
			1, (const char *) _bURLParameters);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bSDP. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION,
			500, ulSequenceNumber, (const char *) NULL,
			_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bIsLive)
	{
		if (_sStandard == MP4Atom:: MP4F_3GPP)
		{
			if (SDPFor3GPP:: getMediaInfoFromSDPFile (
				(const char *) _bAssetPath,
				&_bVideoTrackFoundInSDP,
				&_cVideoCodecUsed,
				&_ulVideoAvgBitRate,
				&_ulVideoPayloadNumber,
				_pVideoTrackName,
				&_ulLiveVideoRTPPacketsServerPort,
				&_bAudioTrackFoundInSDP,
				&_cAudioCodecUsed,
				&_ulAudioAvgBitRate,
				&_ulAudioPayloadNumber,
				_pAudioTrackName,
				&_ulLiveAudioRTPPacketsServerPort,
				&bSDP,
				_ptSystemTracer) != errNoError)
			{
				Error err = RTSPErrors (__FILE__, __LINE__,
					RTSP_RTSPUTILITY_GETMEDIAINFOFROMSDPFILE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION,
					500, ulSequenceNumber, (const char *) NULL,
					_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else			// MP4Atom:: MP4F_ISMA
		{
			if (SDPForRTSP:: getMediaInfoFromSDPFile (
				(const char *) _bAssetPath,
				&_bVideoTrackFoundInSDP,
				&_cVideoCodecUsed,
				&_ulVideoAvgBitRate,
				&_ulVideoPayloadNumber,
				_pVideoTrackName,
				&_ulLiveVideoRTPPacketsServerPort,
				&_bAudioTrackFoundInSDP,
				&_cAudioCodecUsed,
				&_ulAudioAvgBitRate,
				&_ulAudioPayloadNumber,
				_pAudioTrackName,
				&_ulLiveAudioRTPPacketsServerPort,
				&bSDP,
				_ptSystemTracer) != errNoError)
			{
				Error err = RTSPErrors (__FILE__, __LINE__,
					RTSP_RTSPUTILITY_GETMEDIAINFOFROMSDPFILE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION,
					500, ulSequenceNumber, (const char *) NULL,
					_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}
	else
	{
		unsigned long			ulVideoRTPTimeScale;
		unsigned long			ulAudioRTPTimeScale;
		unsigned char			ucVideoSystemsProfileLevel;
		unsigned char			ucChannels;


		if ((errGetMP4File = _pmp4fMP4FileFactory -> getMP4File (
			(const char *) _bAssetPath, false, &_pmfFile,
			_bBuildOfHintingTrackIfNotExist,
			_ulFileCacheSizeInBytes, _sStandard,
			&bIsMP4FileInCache)) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((long) errGetMP4File == MP4F_MP4FILEFACTORY_FILENOTFOUND)
			{
				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION,
					404, ulSequenceNumber, (const char *) NULL,
					_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
			else
			{
				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION,
					500, ulSequenceNumber, (const char *) NULL,
					_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return errGetMP4File;
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_MP4FILEINCACHE,
				3, _ulIdentifier, (const char *) _bAssetPath,
				bIsMP4FileInCache ? "true" : "false");
			_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if ((errGetInfoFromMP4File = getInfoFromMP4File (
			_rsRTPSession. _ulCurrentMaxPayloadSizeInBytes,
			_pmfFile, &_pmaRootAtom,
			&_bVideoTrackFoundInSDP, &_pmtiVideoMediaTrackInfo,
			&_pmtiVideoHintTrackInfo,
			&_bAudioTrackFoundInSDP, &_pmtiAudioMediaTrackInfo,
			&_pmtiAudioHintTrackInfo, _bUseOfHintingTrackIfExist,
			_bBuildOfHintingTrackIfNotExist)) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_GETINFOFROMMP4FILE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((long) errGetInfoFromMP4File == TOOLS_FILEIO_OPEN_FAILED ||
				(long) errGetInfoFromMP4File ==
				MP4F_MP4FILEFACTORY_FILENOTFOUND)
			{
				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION, 404, ulSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
					pTimestamp, false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
			else if ((long) errGetInfoFromMP4File ==
				SS_RTSPSESSION_CODECSNOTSUPPORTED)
			{
				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION, 415, ulSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
					pTimestamp, false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
			else
			{
				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
					pTimestamp, false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			if (_pmp4fMP4FileFactory -> releaseMP4File (
				(const char *) _bAssetPath) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_pmfFile				= (MP4File_p) NULL;

			return err;
		}

		{
			if (_bVideoTrackFoundInSDP)
			{
				if (_pmtiVideoMediaTrackInfo -> getTrackName (
					_pVideoTrackName) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETTRACKNAME_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
						pTimestamp, false) != errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_pmp4fMP4FileFactory -> releaseMP4File (
						(const char *) _bAssetPath) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					_pmfFile				= (MP4File_p) NULL;

					return err;
				}

				if (_pmtiVideoMediaTrackInfo -> getCodec (
					&_cVideoCodecUsed) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETCODEC_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
						pTimestamp, false) != errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_pmp4fMP4FileFactory -> releaseMP4File (
						(const char *) _bAssetPath) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					_pmfFile				= (MP4File_p) NULL;

					return err;
				}

				{
					RTPStreamRealTimeInfo_t		rsrtRTPStreamRealTimeInfo;
					MP4TrakAtom_p				pmaMediaTrakAtom;
					MP4HdlrAtom_p				pmaMediaHdlrAtom;
					MP4MdhdAtom_p				pmaMediaMdhdAtom;
					MP4StszAtom_p				pmaMediaStszAtom;
					unsigned long				ulObjectTypeIndication;


					if (_pmtiVideoMediaTrackInfo -> getTrakAtom (
						&pmaMediaTrakAtom) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
							(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
							pTimestamp, false) != errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;

						return err;
					}

					if (_pmtiVideoMediaTrackInfo -> getHdlrAtom (
						&pmaMediaHdlrAtom) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETHDLRATOM_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
							(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
							pTimestamp, false) != errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;

						return err;
					}

					if (_pmtiVideoMediaTrackInfo -> getMdhdAtom (
						&pmaMediaMdhdAtom) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETMDHDATOM_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
							(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
							pTimestamp, false) != errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;

						return err;
					}

					if (_pmtiVideoMediaTrackInfo -> getStszAtom (
						&pmaMediaStszAtom, true) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETSTSZATOM_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
							(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
							pTimestamp, false) != errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;

						return err;
					}

					if (RTPUtility:: getAvgBitRate (
						pmaMediaTrakAtom,
						pmaMediaHdlrAtom,
						pmaMediaMdhdAtom,
						pmaMediaStszAtom,
						_cVideoCodecUsed,
						&_ulVideoAvgBitRate,
						&rsrtRTPStreamRealTimeInfo,
						_ptSystemTracer) != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPUTILITY_GETAVGBITRATE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
							(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
							pTimestamp, false) != errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;

						return err;
					}

					if (_cVideoCodecUsed == MP4Atom:: MP4F_CODEC_MPEG4)
					{
						if (_pmtiVideoMediaTrackInfo ->
							getObjectTypeIndication (&ulObjectTypeIndication) !=
							errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETOBJECTTYPEINDICATION_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (bSDP. finish () != errNoError)
							{
								Error err = ToolsErrors (__FILE__, __LINE__,
									TOOLS_BUFFER_FINISH_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							if (sendRTSPResponse (_psiClientSocketImpl,
								RTSP_PREDEFINEDRTSPVERSION,
								500, ulSequenceNumber, (const char *) NULL,
								_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
								errNoError)
							{
								Error err = StreamingServerErrors (
									__FILE__, __LINE__,
									SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							if (_pmp4fMP4FileFactory -> releaseMP4File (
								(const char *) _bAssetPath) != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							_pmfFile				= (MP4File_p) NULL;

							return err;
						}
					}
					else
						ulObjectTypeIndication      = 0;

					if (RTPUtility:: getRTPTimeScale (
						pmaMediaTrakAtom,
						pmaMediaMdhdAtom,
						_cVideoCodecUsed,
						ulObjectTypeIndication,
						_sStandard, &ulVideoRTPTimeScale,
						_ptSystemTracer) != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPUTILITY_GETRTPTIMESCALE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION,
							500, ulSequenceNumber, (const char *) NULL,
							_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
							errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;

						return err;
					}

					// to calculate ucVideoSystemsProfileLevel
					if (_cVideoCodecUsed == MP4Atom:: MP4F_CODEC_MPEG4)
					{
						unsigned char				pucVoshStartCode [4];
						unsigned char				*pucESConfiguration;
						#ifdef WIN32
							__int64                     ullESConfigurationSize;
						#else
							unsigned long long          ullESConfigurationSize;
						#endif


						if (_pmtiVideoMediaTrackInfo ->
							getPointerTrackESConfiguration (
							&pucESConfiguration, &ullESConfigurationSize,
							_sStandard) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETPOINTERTRACKESCONFIGURATION_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (bSDP. finish () != errNoError)
							{
								Error err = ToolsErrors (__FILE__, __LINE__,
									TOOLS_BUFFER_FINISH_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							if (sendRTSPResponse (_psiClientSocketImpl,
								RTSP_PREDEFINEDRTSPVERSION,
								500, ulSequenceNumber, (const char *) NULL,
								_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
								errNoError)
							{
								Error err = StreamingServerErrors (
									__FILE__, __LINE__,
									SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							if (_pmp4fMP4FileFactory -> releaseMP4File (
								(const char *) _bAssetPath) != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							_pmfFile				= (MP4File_p) NULL;

							return err;
						}

						// attempt to get a valid profile-level
						pucVoshStartCode [0]			= 0x00;
						pucVoshStartCode [1]			= 0x00;
						pucVoshStartCode [2]			= 0x01;
						pucVoshStartCode [3]			= MP4F_MPEG4_VOSH_START;

						ucVideoSystemsProfileLevel			= 0xFE;

						if (ullESConfigurationSize >= 5 &&
							!memcmp (pucESConfiguration, pucVoshStartCode, 4))
						{
							if (_sStandard == MP4Atom:: MP4F_3GPP)
								ucVideoSystemsProfileLevel	=
									pucESConfiguration [4];
							else			// MP4Atom:: MP4F_ISMA
								ucVideoSystemsProfileLevel	=
									MP4AV_Mpeg4VideoToSystemsProfileLevel (
									pucESConfiguration [4]);
						}

						if (ucVideoSystemsProfileLevel == 0xFE)
						{
							MP4InitialObjectDescr_p		pmaInitialObjectDescr;
							MP4Atom_p					pmaAtom;
							char						pAtomPath [
								MP4F_MAXPATHNAMELENGTH];
							unsigned char		ucVisualProfileLevelIndication;


							strcpy (pAtomPath,
								"moov:0:iods:0:InitialObjectDescr:0");

							if (_pmaRootAtom -> searchAtom (pAtomPath, true,
								&pmaAtom) != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_SEARCHATOM_FAILED, 1,
									"moov:0:iods:0:InitialObjectDescr:0");
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (bSDP. finish () != errNoError)
								{
									Error err = ToolsErrors (__FILE__, __LINE__,
										TOOLS_BUFFER_FINISH_FAILED);
									_ptSystemTracer -> trace (
										Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								if (sendRTSPResponse (_psiClientSocketImpl,
									RTSP_PREDEFINEDRTSPVERSION,
									500, ulSequenceNumber, (const char *) NULL,
									_ulRTSP_RTCPTimeoutInSecs, pTimestamp,
									false) != errNoError)
								{
									Error err = StreamingServerErrors (
										__FILE__, __LINE__,
										SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
									_ptSystemTracer -> trace (
										Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								if (_pmp4fMP4FileFactory -> releaseMP4File (
									(const char *) _bAssetPath) != errNoError)
								{
									Error err = MP4FileErrors (
										__FILE__, __LINE__,
										MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
									_ptSystemTracer -> trace (
										Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								_pmfFile				= (MP4File_p) NULL;

								return err;
							}
							else
								pmaInitialObjectDescr	=
									(MP4InitialObjectDescr_p) pmaAtom;

							if (pmaInitialObjectDescr ->
								getVisualProfileLevelIndication (
								&ucVisualProfileLevelIndication) != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4INITIALOBJECTDESCR_GETVISUALPROFILELEVELINDICATION_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (bSDP. finish () != errNoError)
								{
									Error err = ToolsErrors (__FILE__, __LINE__,
										TOOLS_BUFFER_FINISH_FAILED);
									_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								if (sendRTSPResponse (_psiClientSocketImpl,
									RTSP_PREDEFINEDRTSPVERSION,
									500, ulSequenceNumber, (const char *) NULL,
									_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
									errNoError)
								{
									Error err = StreamingServerErrors (
										__FILE__, __LINE__,
										SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
									_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								if (_pmp4fMP4FileFactory -> releaseMP4File (
									(const char *) _bAssetPath) != errNoError)
								{
									Error err = MP4FileErrors (__FILE__, __LINE__,
										MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
									_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								_pmfFile				= (MP4File_p) NULL;

								return err;
							}

							if (ucVisualProfileLevelIndication > 0 &&
								ucVisualProfileLevelIndication < 0xFE)
							{
								ucVideoSystemsProfileLevel		=
									ucVisualProfileLevelIndication;
							}
							else
							{
								ucVideoSystemsProfileLevel		= 1;
							}
						}
					}
					else
						ucVideoSystemsProfileLevel			= 0;
				}
			}
			else
			{
				ucVideoSystemsProfileLevel			= 0;
				ulVideoRTPTimeScale					= 0;
			}

			if (_bAudioTrackFoundInSDP)
			{
				if (_pmtiAudioMediaTrackInfo -> getTrackName (
					_pAudioTrackName) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETTRACKNAME_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
						pTimestamp, false) != errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_pmp4fMP4FileFactory -> releaseMP4File (
						(const char *) _bAssetPath) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					_pmfFile				= (MP4File_p) NULL;

					return err;
				}

				if (_pmtiAudioMediaTrackInfo -> getCodec (
					&_cAudioCodecUsed) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETCODEC_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
						pTimestamp, false) != errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_pmp4fMP4FileFactory -> releaseMP4File (
						(const char *) _bAssetPath) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					_pmfFile				= (MP4File_p) NULL;


					return err;
				}

				{
					RTPStreamRealTimeInfo_t		rsrtRTPStreamRealTimeInfo;
					MP4TrakAtom_p				pmaMediaTrakAtom;
					MP4HdlrAtom_p				pmaMediaHdlrAtom;
					MP4MdhdAtom_p				pmaMediaMdhdAtom;
					MP4StszAtom_p				pmaMediaStszAtom;
					unsigned long				ulObjectTypeIndication;


					if (_pmtiAudioMediaTrackInfo -> getTrakAtom (
						&pmaMediaTrakAtom) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
							(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
							pTimestamp, false) != errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;

						return err;
					}

					if (_pmtiAudioMediaTrackInfo -> getHdlrAtom (
						&pmaMediaHdlrAtom) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETHDLRATOM_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
							(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
							pTimestamp, false) != errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;

						return err;
					}

					if (_pmtiAudioMediaTrackInfo -> getMdhdAtom (
						&pmaMediaMdhdAtom) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETMDHDATOM_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
							(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
							pTimestamp, false) != errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;

						return err;
					}

					if (_pmtiAudioMediaTrackInfo -> getStszAtom (
						&pmaMediaStszAtom, true) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETSTSZATOM_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
							(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
							pTimestamp, false) != errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;

						return err;
					}

					if (RTPUtility:: getAvgBitRate (
						pmaMediaTrakAtom,
						pmaMediaHdlrAtom,
						pmaMediaMdhdAtom,
						pmaMediaStszAtom,
						_cAudioCodecUsed,
						&_ulAudioAvgBitRate,
						&rsrtRTPStreamRealTimeInfo,
						_ptSystemTracer) != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPUTILITY_GETAVGBITRATE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION, 500, ulSequenceNumber,
							(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
							pTimestamp, false) != errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;

						return err;
					}

					if (_cAudioCodecUsed == MP4Atom:: MP4F_CODEC_AAC)
					{
						if (_pmtiAudioMediaTrackInfo ->
							getObjectTypeIndication (&ulObjectTypeIndication) !=
							errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETOBJECTTYPEINDICATION_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (bSDP. finish () != errNoError)
							{
								Error err = ToolsErrors (__FILE__, __LINE__,
									TOOLS_BUFFER_FINISH_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							if (sendRTSPResponse (_psiClientSocketImpl,
								RTSP_PREDEFINEDRTSPVERSION,
								500, ulSequenceNumber, (const char *) NULL,
								_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
								errNoError)
							{
								Error err = StreamingServerErrors (
									__FILE__, __LINE__,
									SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							if (_pmp4fMP4FileFactory -> releaseMP4File (
								(const char *) _bAssetPath) != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							_pmfFile				= (MP4File_p) NULL;

							return err;
						}
					}
					else
						ulObjectTypeIndication      = 0;

					if (RTPUtility:: getRTPTimeScale (
						pmaMediaTrakAtom,
						pmaMediaMdhdAtom,
						_cAudioCodecUsed,
						ulObjectTypeIndication,
						_sStandard, &ulAudioRTPTimeScale,
						_ptSystemTracer) != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPUTILITY_GETRTPTIMESCALE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION,
							500, ulSequenceNumber, (const char *) NULL,
							_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
							errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;

						return err;
					}
				}

				if (_sStandard == MP4Atom:: MP4F_ISMA &&
					_cAudioCodecUsed == MP4Atom:: MP4F_CODEC_AAC)
				{
					unsigned char				*pucESConfiguration;
					#ifdef WIN32
						__int64						ullESConfigurationSize;
					#else
						unsigned long long			ullESConfigurationSize;
					#endif


					if (_pmtiAudioMediaTrackInfo ->
						getPointerTrackESConfiguration (
						&pucESConfiguration, &ullESConfigurationSize,
						_sStandard) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETPOINTERTRACKESCONFIGURATION_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION,
							500, ulSequenceNumber, (const char *) NULL,
							_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
							errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;

						return err;
					}

					ucChannels					=
						MP4AV_AacConfigGetChannels (pucESConfiguration);
				}
				else
					ucChannels					= 1;
			}
			else
			{
				ucChannels							= 0;
				ulVideoRTPTimeScale					= 0;
			}
		}

		if (_pmfFile -> getMovieDuration (
			_bVideoTrackFoundInSDP ? _pmtiVideoMediaTrackInfo :
			(MP4TrackInfo_p) NULL, &_dMovieDuration) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_GETMOVIEDURATION_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION,
				500, ulSequenceNumber, (const char *) NULL,
				_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_pmp4fMP4FileFactory -> releaseMP4File (
				(const char *) _bAssetPath) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_pmfFile				= (MP4File_p) NULL;

			return err;
		}

		if (SDPFor3GPP:: getSDPFromMediaInfo (
			_pmfFile,
			_pLocalIPAddressForRTSP,
			_bVideoTrackFoundInSDP ? _pmtiVideoMediaTrackInfo :
			(MP4TrackInfo_p) NULL,
			_ulVideoAvgBitRate,
			ulVideoRTPTimeScale,
			ucVideoSystemsProfileLevel,
			_bAudioTrackFoundInSDP ? _pmtiAudioMediaTrackInfo :
			(MP4TrackInfo_p) NULL,
			_ulAudioAvgBitRate,
			ulAudioRTPTimeScale,
			ucChannels,
			_sStandard,
			&bSDP,
			&_ulVideoPayloadNumber,
			&_ulAudioPayloadNumber,
			_ptSystemTracer) != errNoError)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_GETSDPFROMMEDIAINFO_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION,
				500, ulSequenceNumber, (const char *) NULL,
				_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_pmp4fMP4FileFactory -> releaseMP4File (
				(const char *) _bAssetPath) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_pmfFile				= (MP4File_p) NULL;

			return err;
		}
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_RTSPSESSION_TRACKSCHOSEN,
			3, _ulIdentifier, _bVideoTrackFoundInSDP ?
			_pVideoTrackName : "No video chosen",
			_bAudioTrackFoundInSDP ?
			_pAudioTrackName : "No audio chosen");
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

	_bVideoTrackInitializedFromSetup				= false;
	_bAudioTrackInitializedFromSetup				= false;

	if (bResponse. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bSDP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (!_bIsLive)
		{
			if (_pmp4fMP4FileFactory -> releaseMP4File (
				(const char *) _bAssetPath) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_pmfFile				= (MP4File_p) NULL;
		}

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION,
			500, ulSequenceNumber, (const char *) NULL,
			_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (RTSPUtility:: getDESCRIBEResponse (
		ulSequenceNumber, _ulRTSP_RTCPTimeoutInSecs,
		pTimestamp, &bSDP, &bResponse, _ptSystemTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETDESCRIBERESPONSE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bSDP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (!_bIsLive)
		{
			if (_pmp4fMP4FileFactory -> releaseMP4File (
				(const char *) _bAssetPath) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_pmfFile				= (MP4File_p) NULL;
		}

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION,
			500, ulSequenceNumber, (const char *) NULL,
			_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((errWrite = _psiClientSocketImpl -> writeString (
		(const char *) bResponse, true, 1, 0)) != errNoError)
	{
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errWrite, __FILE__, __LINE__);
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bSDP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (!_bIsLive)
		{
			if (_pmp4fMP4FileFactory -> releaseMP4File (
				(const char *) _bAssetPath) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_pmfFile				= (MP4File_p) NULL;
		}

		/*
			does not have sense since the write fails
		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION,
			500, ulSequenceNumber, (const char *) NULL,
			_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}
		*/

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_RTSPSESSION_SENDTOCLIENT,
			2, _ulIdentifier, (const char *) bResponse);
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (bResponse. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bSDP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (!_bIsLive)
		{
			if (_pmp4fMP4FileFactory -> releaseMP4File (
				(const char *) _bAssetPath) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_pmfFile				= (MP4File_p) NULL;
		}

		return err;
	}

	if (bSDP. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (!_bIsLive)
		{
			if (_pmp4fMP4FileFactory -> releaseMP4File (
				(const char *) _bAssetPath) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_pmfFile				= (MP4File_p) NULL;
		}

		return err;
	}

	if (setRTSPStatus (RTSPSession:: SS_RTSP_INIT) !=
		errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: handleSETUPMethod (const char *pRTSPRequest,
	unsigned long *pulMethodLength)

{

	Buffer_t						bResponse;
	Buffer_t						bString;
	Buffer_t						bURI;
	Buffer_t						bTransport;
	long							lSequenceNumber;
	unsigned long					ulRTPClientPort;
	unsigned long					ulRTCPClientPort;
	unsigned long					ulRTPServerPort;
	unsigned long					ulRTCPServerPort;
	char							pSessionIdentifier [
		SS_MAXSESSIONIDENTIFIERLENGTH];
	unsigned long 					ulRequestedPayloadSizeInBytes;
	unsigned long					ulAddedTrackIdentifier;

	Error							errWrite;
	char							pTimestamp [SS_MAXLONGLENGTH];
	RTSPStatus_t					rsRTSPStatus;


	strcpy (pSessionIdentifier, "");

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// this method can be called in every states
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_INIT &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_READY &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_PLAYING &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_RECORDING)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "SETUP", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_ulReservedFinishingPort - _ulCurrentPortForSetup < 1)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_PORTSNOTAVAILABLE);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	ulRTPServerPort				= _ulCurrentPortForSetup;
	ulRTCPServerPort			= _ulCurrentPortForSetup + 1;

	if (bTransport. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bURI. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bTransport. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (RTSPUtility:: parseSETUPRequest (
		pRTSPRequest,

		(Buffer_p) NULL,
		&bTransport,
		&bURI,
		&lSequenceNumber,
		&ulRTPClientPort,
		&ulRTCPClientPort,
		pSessionIdentifier,
		pTimestamp,
		&ulRequestedPayloadSizeInBytes,
		pulMethodLength,
		_ptSystemTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_PARSESETUPREQUEST_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bTransport. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
		}

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// another (the other checks are inside the parse method) check
	//	for SETUP request
	if (rsRTSPStatus == RTSPSession:: SS_RTSP_READY &&
		!strcmp (pSessionIdentifier, ""))
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_SETUPREQUESTWRONG,
			1, pRTSPRequest);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bTransport. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
			strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulRequestedPayloadSizeInBytes == 0 || // no indication on max payload size
		ulRequestedPayloadSizeInBytes > _ulMaxPayloadSizeInBytes)
	{
		if (ulRequestedPayloadSizeInBytes != 0)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_REQUESTEDPAYLOADSIZETOOBIG,
				2, ulRequestedPayloadSizeInBytes, _ulMaxPayloadSizeInBytes);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		_rsRTPSession. _ulCurrentMaxPayloadSizeInBytes		=
			_ulMaxPayloadSizeInBytes;
	}
	else
		_rsRTPSession. _ulCurrentMaxPayloadSizeInBytes		=
			ulRequestedPayloadSizeInBytes;

	if (rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
/*
				TO BE REVIEWED

		// the connection started with the SETUP RTSP command
		//		and we don't have the DESCRIBE RTSP command

		char					pURLIPAddress [
			SCK_MAXIPADDRESSLENGTH];
		long					lRTSPPort;
		Buffer_t				bRelativePath;
		Boolean_t				bMP4FileFound;
		Error_t					errGetMP4File;
		Boolean_t				bIsMP4FileInCache;
		Error_t					errGetInfoFromMP4File;


		if (bRelativePath. init () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (parseURL ((const char *) bURI,
			pURLIPAddress, &lRTSPPort, &bRelativePath) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_PARSEURL_FAILED,
				1, (const char *) bURI);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRelativePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_bURLWithoutParameters. setBuffer (
			(const char *) bRelativePath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRelativePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_bAssetPath. setBuffer (
			(const char *) _bContentRootPath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRelativePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_bAssetPath. append ((const char *) bRelativePath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRelativePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		#ifdef WIN32
			// Since the URL contains a path with '/', we must convert them to '\'
			if (_bAssetPath. substitute ("/", "\\") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		#endif

		bMP4FileFound			= false;

		while (!bMP4FileFound)
		{
			#ifdef WIN32
				if (strrchr ((const char *) _bAssetPath, '\\') == (char *) NULL)
			#else
				if (strrchr ((const char *) _bAssetPath, '/') == (char *) NULL)
			#endif
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SETUPURIWRONGBECAUSEFILENAME,
					1, (const char *) bURI);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			#ifdef WIN32
				*strrchr ((const char *) _bAssetPath, '\\')		= '\0';
			#else
				*strrchr ((const char *) _bAssetPath, '/')		= '\0';
			#endif

			*strrchr ((const char *) _bURLWithoutParameters, '/')		= '\0';

			if (FileIO:: exist ((const char *) _bAssetPath, &bMP4FileFound) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_EXIST_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (isLiveRequest ((const char *) _bAssetPath,
			&_bIsLive) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_ISLIVEREQUEST_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRelativePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_bIsLive)
		{
			_bIsFirstLiveVideoRTPPacketToSend			= true;
			_bIsFirstLiveAudioRTPPacketToSend			= true;
		}

		if (_bIsLive)
		{
			Buffer_t			bSDP;


			if (bSDP. init () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bSDP. readBufferFromFile ((const char *) _bAssetPath) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_READBUFFERFROMFILE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// the parseAndAdjustLiveSDP method was deleted
			// if (parseAndAdjustLiveSDP (&bSDP,

			_ulVideoAvgBitRate			*= 1000;
			_ulAudioAvgBitRate			*= 1000;

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if ((errGetMP4File = _pmp4fMP4FileFactory -> getMP4File (
				(const char *) _bAssetPath, false, &_pmfFile,
				_bBuildOfHintingTrackIfNotExist,
				_ulFileCacheSizeInBytes, &_sStandard,
				&bIsMP4FileInCache)) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return errGetMP4File;
			}

			{
				Message msg = StreamingServerMessages (__FILE__, __LINE__,
					SS_RTSPSESSION_MP4FILEINCACHE,
					3, _ulIdentifier, (const char *) _bAssetPath,
					bIsMP4FileInCache ? "true" : "false");
				_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
					__FILE__, __LINE__);
			}

			if ((errGetInfoFromMP4File = getInfoFromMP4File (
				_rsRTPSession. _ulCurrentMaxPayloadSizeInBytes,
				_pmfFile, &_pmaRootAtom,
				&_bVideoTrackFoundInSDP, &_pmtiVideoMediaTrackInfo,
				&_pmtiVideoHintTrackInfo,
				&_bAudioTrackFoundInSDP, &_pmtiVideoMediaTrackInfo,
				&_pmtiVideoHintTrackInfo,
				_bUseOfHintingTrackIfExist,
				_bBuildOfHintingTrackIfNotExist)) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_GETINFOFROMMP4FILE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if ((long) errGetInfoFromMP4File == TOOLS_FILEIO_OPEN_FAILED ||
					(long) errGetInfoFromMP4File == MP4F_MP4FILEFACTORY_FILENOTFOUND)
				{
					if (sendRTSPResponse (_psiClientSocketImpl,
						bRTSPVersionRead ? (const char *) bRTSPVersion :
						RTSP_PREDEFINEDRTSPVERSION, 404, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
						errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
				else if ((long) errGetInfoFromMP4File == SS_RTSPSESSION_CODECSNOTSUPPORTED)
				{
					if (sendRTSPResponse (_psiClientSocketImpl,
						bRTSPVersionRead ? (const char *) bRTSPVersion :
						RTSP_PREDEFINEDRTSPVERSION, 415, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
						errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
				else
				{
					if (sendRTSPResponse (_psiClientSocketImpl,
						bRTSPVersionRead ? (const char *) bRTSPVersion :
						RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
						errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				if (!_bIsLive)
				{
					if (_pmp4fMP4FileFactory -> releaseMP4File (
						(const char *) _bAssetPath) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					_pmfFile				= (MP4File_p) NULL;
				}

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			{
				if (_bVideoTrackFoundInSDP)
				{
					if (_pmtiVideoMediaTrackInfo -> getCodec (
						&_cVideoCodecUsed) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETCODEC_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (!_bIsLive)
						{
							if (_pmp4fMP4FileFactory -> releaseMP4File (
								(const char *) _bAssetPath) != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							_pmfFile				= (MP4File_p) NULL;
						}

						if (bRelativePath. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bURI. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bTransport. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							bRTSPVersionRead ? (const char *) bRTSPVersion :
							RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
							strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
							(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
							false) != errNoError)
						{
							Error err = StreamingServerErrors (__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bRTSPVersion. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
				else
				{
					_cVideoCodecUsed		= MP4Atom:: MP4F_CODEC_UNKNOWN;
				}

				if (_bAudioTrackFoundInSDP)
				{
					if (_pmtiAudioMediaTrackInfo -> getCodec (
						&_cAudioCodecUsed) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETCODEC_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (!_bIsLive)
						{
							if (_pmp4fMP4FileFactory -> releaseMP4File (
								(const char *) _bAssetPath) != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							_pmfFile				= (MP4File_p) NULL;
						}

						if (bRelativePath. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bURI. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bTransport. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							bRTSPVersionRead ? (const char *) bRTSPVersion :
							RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
							strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
							(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
							false) != errNoError)
						{
							Error err = StreamingServerErrors (__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bRTSPVersion. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
				else
				{
					_cAudioCodecUsed		= MP4Atom:: MP4F_CODEC_UNKNOWN;
				}
			}

			{
				MP4TrackInfo_p		pmtiVideoTrackInfo;


				if (_bVideoTrackFoundInSDP)
					pmtiVideoTrackInfo		= _pmtiVideoHintTrackInfo != (MP4TrackInfo_p) NULL ?
						_pmtiVideoHintTrackInfo : _pmtiVideoMediaTrackInfo;
				else
					pmtiVideoTrackInfo		= (MP4TrackInfo_p) NULL;

				if (_pmfFile -> getMovieDuration (
					pmtiVideoTrackInfo, &_dMovieDuration) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILE_GETMOVIEDURATION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (!_bIsLive)
					{
						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;
					}

					if (bRelativePath. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bURI. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bTransport. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						bRTSPVersionRead ? (const char *) bRTSPVersion :
						RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
						strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
						false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bRTSPVersion. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			_bVideoTrackInitializedFromSetup			= false;
			_bAudioTrackInitializedFromSetup			= false;

			if (bRelativePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (!_bIsLive)
				{
					if (_pmp4fMP4FileFactory -> releaseMP4File (
						(const char *) _bAssetPath) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					_pmfFile				= (MP4File_p) NULL;
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
*/
	}

	// RTP session
	{
		MP4TrackInfo_p				pmtiHintOrMediaTrackInfo;
		const char					*pTrackName;
		unsigned long				ulPayloadNumber;
		Error_t						errSetTrack;
		Boolean_t					bIsVideoSetup;
		Boolean_t					bIsHintTrack;
		Error_t						errAddTrack;
		std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo;


		// if SS_RTSP_CONNECTIONACCEPTED means that SETUP is the first RTSP command
		// if SS_RTSP_INIT means that we already had had DESCRIBE
		if (rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
			rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
		{
			if (initRTPSession () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_INITRTPSESSION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
				{
					if (!_bIsLive)
					{
						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;
					}
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// session identifier construction
			{
				char			pRandomNumber [SS_MAXLONGLENGTH];
				unsigned long	ulULongRandom;


				do
				{
					if (RTPUtility:: getULongRandom (&ulULongRandom) !=
						errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPUTILITY_GETULONGRANDOM_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (finishRTPSession () != errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
						{
							if (!_bIsLive)
							{
								if (_pmp4fMP4FileFactory -> releaseMP4File (
									(const char *) _bAssetPath) != errNoError)
								{
									Error err = MP4FileErrors (
										__FILE__, __LINE__,
										MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
									_ptSystemTracer -> trace (
										Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								_pmfFile				= (MP4File_p) NULL;
							}
						}

						if (bURI. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bTransport. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
							strcmp (pSessionIdentifier, "") ?
							pSessionIdentifier : (const char *) NULL,
							_ulRTSP_RTCPTimeoutInSecs,
							pTimestamp, false) != errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					sprintf (pRandomNumber, "%lu", ulULongRandom);
					strcat (pSessionIdentifier, pRandomNumber);
				}
				while (strlen (pSessionIdentifier) <= 8);
			}
		}

		if (_bVideoTrackFoundInSDP && (pTrackName = strstr (
			(const char *) bURI, _pVideoTrackName)) !=
			(char *) NULL &&
			*(pTrackName + strlen (_pVideoTrackName)) == '\0')
		{
			if (!_bIsLive)
			{
				if (_pmtiVideoHintTrackInfo != (MP4TrackInfo_p) NULL)
				{
					pmtiHintOrMediaTrackInfo	= _pmtiVideoHintTrackInfo;
					bIsHintTrack				= true;
				}
				else
				{
					pmtiHintOrMediaTrackInfo	= _pmtiVideoMediaTrackInfo;
					bIsHintTrack				= false;
				}
			}
			else
			{
				bIsHintTrack				= false;
				pmtiHintOrMediaTrackInfo	= (MP4TrackInfo_p) NULL;
			}

			ulPayloadNumber			= _ulVideoPayloadNumber;

			bIsVideoSetup				= true;

			if (_bVideoRequestURI. setBuffer ((const char *) bURI) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SETBUFFER_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
					rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
				{
					if (finishRTPSession () != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				if (rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
				{
					if (!_bIsLive)
					{
						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;
					}
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (_bAudioTrackFoundInSDP &&
			(pTrackName = strstr ((const char *) bURI,
			_pAudioTrackName)) != (char *) NULL &&
			*(pTrackName + strlen (_pAudioTrackName)) == '\0')
		{
			if (!_bIsLive)
			{
				if (_pmtiAudioHintTrackInfo != (MP4TrackInfo_p) NULL)
				{
					pmtiHintOrMediaTrackInfo	= _pmtiAudioHintTrackInfo;
					bIsHintTrack				= true;
				}
				else
				{
					pmtiHintOrMediaTrackInfo	= _pmtiAudioMediaTrackInfo;
					bIsHintTrack				= false;
				}
			}
			else
			{
				bIsHintTrack				= false;
				pmtiHintOrMediaTrackInfo	= (MP4TrackInfo_p) NULL;
			}

			ulPayloadNumber			= _ulAudioPayloadNumber;

			bIsVideoSetup				= false;

			if (_bAudioRequestURI. setBuffer (
				(const char *) bURI) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SETBUFFER_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
					rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
				{
					if (finishRTPSession () != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				if (rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
				{
					if (!_bIsLive)
					{
						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;
					}
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SETUPURIWRONGBECAUSETRACKNAME,
				3, (const char *) bURI, _pVideoTrackName,
				_pAudioTrackName);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
				rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
			{
				if (finishRTPSession () != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			if (rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
			{
				if (!_bIsLive)
				{
					if (_pmp4fMP4FileFactory -> releaseMP4File (
						(const char *) _bAssetPath) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					_pmfFile				= (MP4File_p) NULL;
				}
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// initialization of pvMP4TracksInfo
		if (!_bIsLive)
		{
			if (_pmfFile -> getTracksInfo (&pvMP4TracksInfo) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILE_GETTRACKSINFO_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
					rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
				{
					if (finishRTPSession () != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				if (rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
				{
					if (!_bIsLive)
					{
						if (_pmp4fMP4FileFactory -> releaseMP4File (
							(const char *) _bAssetPath) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						_pmfFile				= (MP4File_p) NULL;
					}
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
			pvMP4TracksInfo		= (std:: vector<MP4TrackInfo_p> *) NULL;

		if ((errAddTrack = addTrackToStream (_bIsLive, bIsVideoSetup,
			_bIsLive ?
			(bIsVideoSetup ? _cVideoCodecUsed : _cAudioCodecUsed) :
			MP4Atom:: MP4F_CODEC_UNKNOWN,
			(const char *) bURI,
			pmtiHintOrMediaTrackInfo,
			pvMP4TracksInfo,
			bIsHintTrack, ulPayloadNumber,
			ulRTPClientPort, ulRTPServerPort,
			ulRTCPClientPort, ulRTCPServerPort,
			&ulAddedTrackIdentifier)) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_ADDTRACKTOSTREAM_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
				rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
			{
				if (finishRTPSession () != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			if (rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
			{
				if (!_bIsLive)
				{
					if (_pmp4fMP4FileFactory -> releaseMP4File (
						(const char *) _bAssetPath) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					_pmfFile				= (MP4File_p) NULL;
				}
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errAddTrack;
		}

		if (bIsVideoSetup)
		{
			_bVideoTrackInitializedFromSetup		= true;
			_ulVideoTrackIdentifier					= ulAddedTrackIdentifier;
		}
		else
		{
			_bAudioTrackInitializedFromSetup		= true;
			_ulAudioTrackIdentifier					= ulAddedTrackIdentifier;
		}
	}

	if (bResponse. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (deleteTrackToStream (_bIsLive, ulAddedTrackIdentifier) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_DELETETRACKTOSTREAM_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
			rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
		{
			if (finishRTPSession () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
		{
			if (!_bIsLive)
			{
				if (_pmp4fMP4FileFactory -> releaseMP4File (
					(const char *) _bAssetPath) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				_pmfFile				= (MP4File_p) NULL;
			}
		}

		if (bURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bTransport. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
			strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (RTSPUtility:: getSETUPResponse (
		lSequenceNumber,
		pSessionIdentifier,
		_ulRTSP_RTCPTimeoutInSecs,
		pTimestamp,
		&bTransport,
		ulRTPServerPort,
		ulRTCPServerPort,
		&bResponse,
		_ptSystemTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETSETUPRESPONSE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (deleteTrackToStream (_bIsLive, ulAddedTrackIdentifier) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_DELETETRACKTOSTREAM_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
			rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
		{
			if (finishRTPSession () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
		{
			if (!_bIsLive)
			{
				if (_pmp4fMP4FileFactory -> releaseMP4File (
					(const char *) _bAssetPath) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				_pmfFile				= (MP4File_p) NULL;
			}
		}

		if (bURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bTransport. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
			strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((errWrite = _psiClientSocketImpl -> writeString (
		(const char *) bResponse, true, 1, 0)) != errNoError)
	{
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errWrite, __FILE__, __LINE__);

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (deleteTrackToStream (_bIsLive, ulAddedTrackIdentifier) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_DELETETRACKTOSTREAM_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
			rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
		{
			if (finishRTPSession () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
		{
			if (!_bIsLive)
			{
				if (_pmp4fMP4FileFactory -> releaseMP4File (
					(const char *) _bAssetPath) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				_pmfFile				= (MP4File_p) NULL;
			}
		}

		if (bURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bTransport. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		/*
			does not have sense since the write fails
		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
			strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}
		*/

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_RTSPSESSION_SENDTOCLIENT,
			2, _ulIdentifier, (const char *) bResponse);
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

	_ulCurrentPortForSetup		+=2;

	if (bResponse. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (deleteTrackToStream (_bIsLive, ulAddedTrackIdentifier) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_DELETETRACKTOSTREAM_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
			rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
		{
			if (finishRTPSession () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
		{
			if (!_bIsLive)
			{
				if (_pmp4fMP4FileFactory -> releaseMP4File (
					(const char *) _bAssetPath) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				_pmfFile				= (MP4File_p) NULL;
			}
		}

		if (bURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bTransport. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bURI. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (deleteTrackToStream (_bIsLive, ulAddedTrackIdentifier) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_DELETETRACKTOSTREAM_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
			rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
		{
			if (finishRTPSession () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
		{
			if (!_bIsLive)
			{
				if (_pmp4fMP4FileFactory -> releaseMP4File (
					(const char *) _bAssetPath) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				_pmfFile				= (MP4File_p) NULL;
			}
		}

		if (bTransport. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bTransport. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (deleteTrackToStream (_bIsLive, ulAddedTrackIdentifier) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_DELETETRACKTOSTREAM_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
			rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
		{
			if (finishRTPSession () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
		{
			if (!_bIsLive)
			{
				if (_pmp4fMP4FileFactory -> releaseMP4File (
					(const char *) _bAssetPath) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				_pmfFile				= (MP4File_p) NULL;
			}
		}

		return err;
	}

	if (rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
		rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
	{
		if (setRTSPStatus (RTSPSession:: SS_RTSP_READY) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error RTSPSession:: handlePLAYMethod (const char *pRTSPRequest,
	unsigned long *pulMethodLength, RTSPCommand_t rcPreviousRTSPCommand)

{

	Buffer_t						bString;

	long							lSequenceNumber;
	char							pSessionIdentifier [
		SS_MAXSESSIONIDENTIFIERLENGTH];
	Boolean_t						bHasSpeed;
	Error							errWrite;
	Boolean_t						bRangeFound;
	Boolean_t						bIsNow;
	double							dNptStartTime;
	double							dNptEndTime;
	double							dSpeed;
	char							pTimestamp [SS_MAXLONGLENGTH];
	RTSPStatus_t					rsRTSPStatus;

	unsigned long					ulVideoCurrentPacketTimeStamp = 0;
	unsigned short					usVideoCurrentPacketSequenceNumber = 0;
	unsigned long					ulAudioCurrentPacketTimeStamp = 0;
	unsigned short					usAudioCurrentPacketSequenceNumber = 0;

	Boolean_t						bHasRange;
	Boolean_t						bHasSeek;
	Boolean_t						bRTPInfo;


	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus != RTSPSession:: SS_RTSP_READY)
		// && rsRTSPStatus != RTSPSession:: SS_RTSP_PLAYING)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "PLAY", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (RTSPUtility:: parsePLAYRequest (
		pRTSPRequest,

		(Buffer_p) NULL,
		(Buffer_p) NULL,
		&lSequenceNumber,
		pTimestamp,
		&bHasSpeed,
		&dSpeed,
		&bRangeFound,
		&bIsNow,
		&dNptStartTime,
		&dNptEndTime,
		pSessionIdentifier,
		pulMethodLength,
		_ptSystemTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_PARSEPLAYREQUEST_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
			strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (!_bIsLive &&	// 0 0 0 0
		bRangeFound &&
		_bStartTimeInSecsParameter &&
		_rcPreviousRTSPCommand == SS_RTSP_SETUP)
	{
		bHasRange						= true;
		bIsNow							= false;

		if ((double) _lStartTimeInSecsParameter >= _dMovieDuration ||
			_lStartTimeInSecsParameter < 0)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_STARTTIMEPARAMETERWRONG,
				1, _lStartTimeInSecsParameter);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		dNptStartTime					= _lStartTimeInSecsParameter;
		if (dNptEndTime == 0.0)		// not initialized into the range
			dNptEndTime					= _dMovieDuration;
		bHasSeek						= true;
		bRTPInfo						= true;
	}
	else if (!_bIsLive &&	// 0 0 0 1
		bRangeFound &&
		_bStartTimeInSecsParameter &&
		_rcPreviousRTSPCommand == SS_RTSP_PAUSE)
	{
		bHasRange						= true;
		bIsNow							= false;
		// dNptStartTime					= dNptStartTime;
		if (dNptEndTime == 0.0)		// not initialized into the range
			dNptEndTime					= _dMovieDuration;
		bHasSeek						= true;
		bRTPInfo						= true;
	}
	else if (!_bIsLive &&	// 0 0 1 0
		bRangeFound &&
		!_bStartTimeInSecsParameter &&
		_rcPreviousRTSPCommand == SS_RTSP_SETUP)
	{
		bHasRange						= true;
		bIsNow							= false;
		// dNptStartTime					= dNptStartTime;
		if (dNptEndTime == 0.0)		// not initialized into the range
			dNptEndTime					= _dMovieDuration;
		bHasSeek						= true;
		bRTPInfo						= true;
	}
	else if (!_bIsLive &&	// 0 0 1 1
		bRangeFound &&
		!_bStartTimeInSecsParameter &&
		_rcPreviousRTSPCommand == SS_RTSP_PAUSE)
	{
		bHasRange						= true;
		bIsNow							= false;
		// dNptStartTime					= dNptStartTime;
		if (dNptEndTime == 0.0)		// not initialized into the range
			dNptEndTime					= _dMovieDuration;
		bHasSeek						= true;
		bRTPInfo						= true;
	}
	else if (!_bIsLive &&	// 0 1 0 0
		!bRangeFound &&
		_bStartTimeInSecsParameter &&
		_rcPreviousRTSPCommand == SS_RTSP_SETUP)
	{
		bHasRange						= true;
		bIsNow							= false;

		if ((double) _lStartTimeInSecsParameter >= _dMovieDuration ||
			_lStartTimeInSecsParameter < 0)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_STARTTIMEPARAMETERWRONG,
				1, _lStartTimeInSecsParameter);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		dNptStartTime					= _lStartTimeInSecsParameter;
		dNptEndTime						= _dMovieDuration;
		bHasSeek						= true;
		bRTPInfo						= true;
	}
	else if (!_bIsLive &&	// 0 1 0 1
		!bRangeFound &&
		_bStartTimeInSecsParameter &&
		_rcPreviousRTSPCommand == SS_RTSP_PAUSE)
	{
		bHasRange						= true;
		bIsNow							= false;
		dNptStartTime					= -1.0; // range con solo stop
		dNptEndTime						= _dMovieDuration;
		bHasSeek						= false;
		bRTPInfo						= false;
	}
	else if (!_bIsLive &&	// 0 1 1 0
		!bRangeFound &&
		!_bStartTimeInSecsParameter &&
		_rcPreviousRTSPCommand == SS_RTSP_SETUP)
	{
		bHasRange						= true;
		bIsNow							= false;
		dNptStartTime					= 0.0;
		dNptEndTime						= _dMovieDuration;
		bHasSeek						= true;
		bRTPInfo						= true;
	}
	else if (!_bIsLive &&	// 0 1 1 1
		!bRangeFound &&
		!_bStartTimeInSecsParameter &&
		_rcPreviousRTSPCommand == SS_RTSP_PAUSE)
	{
		bHasRange						= true;
		bIsNow							= false;
		dNptStartTime					= -1.0; // range con solo stop
		dNptEndTime						= _dMovieDuration;
		bHasSeek						= false;
		bRTPInfo						= false;
	}
	else if (_bIsLive &&	// 1 ? ? 0
		// bRangeFound &&
		// _bStartTimeInSecsParameter &&
		_rcPreviousRTSPCommand == SS_RTSP_SETUP)
	{
		bHasRange						= true;
		bIsNow							= true;
		// dNptStartTime					= 0.0;
		// dNptEndTime						= _dMovieDuration;
		bHasSeek						= false;
		bRTPInfo						= true;
	}
	else if (_bIsLive &&	// 1 ? ? 1
		// bRangeFound &&
		// _bStartTimeInSecsParameter &&
		_rcPreviousRTSPCommand == SS_RTSP_PAUSE)
	{
		bHasRange						= true;
		bIsNow							= true;
		// dNptStartTime					= 0.0;
		// dNptEndTime						= _dMovieDuration;
		bHasSeek						= false;
		bRTPInfo						= true;
	}

	if (bHasSeek)
	{
		Boolean_t				bIsAPacketPrefetched;
		double					dEffectiveStartTimeInSecs;

		{
			RTPPacket_p				prpRTPPacket;
			unsigned long			ulTrackIdentifier;


			for (ulTrackIdentifier = 0;
				ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
				ulTrackIdentifier++)
			{
				while ((((_rsRTPSession. _ptiTracks) [
					ulTrackIdentifier]). _vBuiltRTPPackets). begin () !=
					(((_rsRTPSession. _ptiTracks) [
					ulTrackIdentifier]). _vBuiltRTPPackets). end ())
				{
					prpRTPPacket			= *((((_rsRTPSession.
						_ptiTracks) [ulTrackIdentifier]).
						_vBuiltRTPPackets). begin ());

					if (prpRTPPacket -> reset () != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPPACKET_RESET_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (_mtRTSPSession. unLock () != errNoError)
						{
							Error err = PThreadErrors (__FILE__, __LINE__,
								THREADLIB_PMUTEX_UNLOCK_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							RTSP_PREDEFINEDRTSPVERSION, 500,
							lSequenceNumber,
							strcmp (pSessionIdentifier, "") ?
							pSessionIdentifier : (const char *) NULL,
							_ulRTSP_RTCPTimeoutInSecs, pTimestamp,
							false) != errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
						_vFreeRTPPackets).  insert (
						(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
						_vFreeRTPPackets). end (), prpRTPPacket);

					(((_rsRTPSession. _ptiTracks) [
						ulTrackIdentifier]). _vBuiltRTPPackets). erase (
						(((_rsRTPSession. _ptiTracks) [
						ulTrackIdentifier]). _vBuiltRTPPackets). begin ());

					(((_rsRTPSession. _ptiTracks) [
						ulTrackIdentifier]).
						_vTransmissionTimeBuiltRTPPackets). erase (
						(((_rsRTPSession. _ptiTracks) [
						ulTrackIdentifier]).
						_vTransmissionTimeBuiltRTPPackets). begin ());
				}
			}
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_SEEKCALLED,
				2, _ulIdentifier, dNptStartTime);
			_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6,
				(const char *) msg, __FILE__, __LINE__);
		}

		if (seek (dNptStartTime, 3.0,
			&dEffectiveStartTimeInSecs) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SEEK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
				pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		dNptStartTime			= dEffectiveStartTimeInSecs;

		_rsRTPSession. _ullLastSentRTCPPacketsInMilliSecs			= 0;
		_rsRTPSession. _ullLastReceiveRTCPPacketsInMilliSecs		= 0;

		if (prefetchNextPackets (true, &bIsAPacketPrefetched) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_PREFETCHNEXTPACKETS_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!bIsAPacketPrefetched)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_NOPACKETPREFETCHED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// initialization of _rsRTPSession. _dRelativeRequestedStartTimeInSecs
	//	and _rsRTPSession. _dRelativeRequestedStopTimeInSecs
	if (!_bIsLive)
	{
		if (_bVideoTrackFoundInSDP)
		{
			/*
			sprintf (pStartTimeInSeconds, "%.3lf",
				(*((((_rsRTPSession. _ptiTracks) [
				_ulVideoTrackIdentifier]).
				_vTransmissionTimeBuiltRTPPackets). begin ())) / 1000);
			*/
			if (
				(((_rsRTPSession. _ptiTracks) [
				_ulVideoTrackIdentifier]).
				_vTransmissionTimeBuiltRTPPackets). begin () !=
				(((_rsRTPSession. _ptiTracks) [
				_ulVideoTrackIdentifier]).
				_vTransmissionTimeBuiltRTPPackets). end ())
			{
				_rsRTPSession. _dRelativeRequestedStartTimeInSecs	=
					(*((((_rsRTPSession. _ptiTracks) [
					_ulVideoTrackIdentifier]).
					_vTransmissionTimeBuiltRTPPackets). begin ())) / 1000;
			}
			else if (bHasRange)
			{
				_rsRTPSession. _dRelativeRequestedStartTimeInSecs	=
					dNptStartTime;
			}
			else
			{
				_rsRTPSession. _dRelativeRequestedStartTimeInSecs	=
					_rsRTPSession.
					_dRelativeTransmissionTimeInSecsOfLastSentRTPPacket;
			}
		}
		else
		{
			/*
			sprintf (pStartTimeInSeconds, "%.3lf",
				(*((((_rsRTPSession. _ptiTracks) [
				_ulAudioTrackIdentifier]).
				_vTransmissionTimeBuiltRTPPackets). begin ())) / 1000);
			*/
			if (
				(((_rsRTPSession. _ptiTracks) [
				_ulAudioTrackIdentifier]).
				_vTransmissionTimeBuiltRTPPackets). begin () !=
				(((_rsRTPSession. _ptiTracks) [
				_ulAudioTrackIdentifier]).
				_vTransmissionTimeBuiltRTPPackets). end ())
			{
				_rsRTPSession. _dRelativeRequestedStartTimeInSecs	=
					(*((((_rsRTPSession. _ptiTracks) [
					_ulAudioTrackIdentifier]).
					_vTransmissionTimeBuiltRTPPackets). begin ())) / 1000;
			}
			else if (bHasRange)
			{
				_rsRTPSession. _dRelativeRequestedStartTimeInSecs	=
					dNptStartTime;
			}
			else
			{
				_rsRTPSession. _dRelativeRequestedStartTimeInSecs	=
					_rsRTPSession.
					_dRelativeTransmissionTimeInSecsOfLastSentRTPPacket;
			}
		}

		if (bHasRange)
		{
			_rsRTPSession. _dRelativeRequestedStopTimeInSecs	=
				dNptEndTime;
		}
		else
		{
			_rsRTPSession. _dRelativeRequestedStopTimeInSecs	= -1.0;
		}
	}

	if (DateTime:: nowLocalInMilliSecs (
		&(_rsRTPSession. _ullPlayTimeInMilliSeconds)) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_DATETIME_NOWLOCALINMILLISECS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
			strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		// calculate the NTP Play Time

		// used in RTCP SR packet
		#ifdef WIN32
			_rsRTPSession. _llNTPPlayTime					=
				(__int64) (0.5 + (_rsRTPSession. _ullPlayTimeInMilliSeconds +
				_ulPlayDelayTimeInMilliSeconds +
				(24 * 60 * 60 * ((70 * 365) + 17))) * 4294967.296);
		#else
			_rsRTPSession. _llNTPPlayTime					=
				(long long) (0.5 + (_rsRTPSession. _ullPlayTimeInMilliSeconds +
				_ulPlayDelayTimeInMilliSeconds +
				(24 * 60 * 60 * ((70 * 365) + 17))) * 4294967.296);
		#endif
	}

	if (!_bIsLive)
	{
		_rsRTPSession. _dAdjustedPlayTimeInMilliSecs		=
			_rsRTPSession. _ullPlayTimeInMilliSeconds +
			_ulPlayDelayTimeInMilliSeconds -
			(_rsRTPSession. _dRelativeRequestedStartTimeInSecs * 1000);

		_rsRTPSession. _dRequestedSpeed		= bHasSpeed ? dSpeed : 1.0;

		if (_rsRTPSession. _dRequestedSpeed > _dMaxSpeedAllowed)
			_rsRTPSession. _dRequestedSpeed			= _dMaxSpeedAllowed;
		else if (_rsRTPSession. _dRequestedSpeed <= 0.0)
			_rsRTPSession. _dRequestedSpeed			= 1.0;
		else
			;

		_rsRTPSession. _bIsSessionFirstWrite			= true;
	}
	else
	{
		// _bIsLive is true

		if (_rcPreviousRTSPCommand == SS_RTSP_SETUP)
		{
			if (addRTSPSessionForTheLiveSource () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_ADDRTSPSESSIONFORTHELIVESOURCE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	if (bRTPInfo)
	{
		// unsigned long							ulSSRC;

		if (_bVideoTrackInitializedFromSetup)
		{
			if (getCurrentPacketRTPTimeStampAndSequenceNumber (
				_ulVideoTrackIdentifier,
				&ulVideoCurrentPacketTimeStamp,
				&usVideoCurrentPacketSequenceNumber) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
		SS_RTSPSESSION_GETCURRENTPACKETRTPTIMESTAMPANDSEQUENCENUMBER_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (_bIsLive)
			{
				// long					lRTPPacketIndex;


				_usPLAYAnswerVideoSequenceNumber		=
					usVideoCurrentPacketSequenceNumber;

				_ulPLAYAnswerVideoTimeStamp				=
					ulVideoCurrentPacketTimeStamp;
			}
		}

		if (_bAudioTrackInitializedFromSetup)
		{
			if (getCurrentPacketRTPTimeStampAndSequenceNumber (
				_ulAudioTrackIdentifier,
				&ulAudioCurrentPacketTimeStamp,
				&usAudioCurrentPacketSequenceNumber) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
		SS_RTSPSESSION_GETCURRENTPACKETRTPTIMESTAMPANDSEQUENCENUMBER_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (_bIsLive)
			{
				// long					lRTPPacketIndex;


				_usPLAYAnswerAudioSequenceNumber		=
					usAudioCurrentPacketSequenceNumber;

				_ulPLAYAnswerAudioTimeStamp				=
					ulAudioCurrentPacketTimeStamp;
			}
		}
	}

	{
		Buffer_t						bResponse;


		if (bResponse. init () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (RTSPUtility:: getPLAYResponse (
			lSequenceNumber,
			pSessionIdentifier,
			_ulRTSP_RTCPTimeoutInSecs,
			pTimestamp,

			bHasSpeed,
			&dSpeed,
			bHasRange,
			bIsNow,
			&dNptStartTime,
			&dNptEndTime,

			bRTPInfo && _bVideoTrackInitializedFromSetup,
			(const char *) _bVideoRequestURI,
			usVideoCurrentPacketSequenceNumber,
			ulVideoCurrentPacketTimeStamp,

			bRTPInfo && _bAudioTrackInitializedFromSetup,
			(const char *) _bAudioRequestURI,
			usAudioCurrentPacketSequenceNumber,
			ulAudioCurrentPacketTimeStamp,

			&bResponse,
			_ptSystemTracer) != errNoError)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_GETPLAYRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if ((errWrite = _psiClientSocketImpl -> writeString (
			(const char *) bResponse, true, 1, 0)) != errNoError)
		{
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errWrite, __FILE__, __LINE__);
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETIMPL_WRITE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			/*
				does not have sense since the write fails
			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
			*/

			return err;
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDTOCLIENT,
				2, _ulIdentifier, (const char *) bResponse);
			_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (bResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

	}

	{
		unsigned long					ulTrackIdentifier;


		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_bTrackInitialized))
				continue;

			if (activeTrack (&((_rsRTPSession. _ptiTracks) [
				ulTrackIdentifier])) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_ACTIVETRACK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	if (setRTSPStatus (RTSPSession:: SS_RTSP_PLAYING) !=
		errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (!_bIsLive)
	{
		Event_p							pevEvent;
		RTSPConnectionEvent_p			pevRTSPConnection;


		if (_pesEventsSet -> getFreeEvent (
			StreamingServerEventsSet:: SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER,
			&pevEvent) != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_GETFREEEVENT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pevRTSPConnection			= (RTSPConnectionEvent_p) pevEvent;

		if (pevRTSPConnection -> init (
			SS_RTSPSESSION_SOURCE,
			SS_EVENT_SENDFILERTPPACKETS, "SS_EVENT_SENDFILERTPPACKETS",
			_ulIdentifier, _ptSystemTracer,
			true, 0, (Buffer_p) NULL) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPCONNECTIONEVENT_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pesEventsSet -> releaseEvent (
			StreamingServerEventsSet:: SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER,
				pevRTSPConnection) != errNoError)
			{
				Error err = EventsSetErrors (__FILE__, __LINE__,
					EVSET_EVENTSSET_RELEASEEVENT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_pesEventsSet -> addEvent (
			// SS_STREAMINGSERVERPROCESSOR_DESTINATION,
			pevRTSPConnection) != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_ADDEVENT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pevRTSPConnection -> finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPCONNECTIONEVENT_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_pesEventsSet -> releaseEvent (
			StreamingServerEventsSet:: SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER,
				pevRTSPConnection) != errNoError)
			{
				Error err = EventsSetErrors (__FILE__, __LINE__,
					EVSET_EVENTSSET_RELEASEEVENT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}


	return errNoError;
}


Error RTSPSession:: handlePAUSEMethod (const char *pRTSPRequest,
	unsigned long *pulMethodLength)

{

	long							lSequenceNumber;
	char							pSessionIdentifier [
		SS_MAXSESSIONIDENTIFIERLENGTH];
	Error							errWrite;
	char							pTimestamp [SS_MAXLONGLENGTH];
	RTSPStatus_t					rsRTSPStatus;



	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus != RTSPSession:: SS_RTSP_PLAYING &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_RECORDING)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "PAUSE", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LWRNG,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (RTSPUtility:: parsePAUSERequest (
		pRTSPRequest,

		(Buffer_p) NULL,
		(Buffer_p) NULL,
		&lSequenceNumber,
		pTimestamp,
		pSessionIdentifier,
		pulMethodLength,
		_ptSystemTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_PARSEPAUSEREQUEST_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		Buffer_t						bResponse;

		if (bResponse. init () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (RTSPUtility:: getPAUSEResponse (
			lSequenceNumber,
			pSessionIdentifier,
			_ulRTSP_RTCPTimeoutInSecs,
			pTimestamp,
			&bResponse,
			_ptSystemTracer) != errNoError)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_GETPAUSERESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if ((errWrite = _psiClientSocketImpl -> writeString (
			(const char *) bResponse, true, 1, 0)) != errNoError)
		{
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errWrite, __FILE__, __LINE__);
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETIMPL_WRITE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			/*
				does not have sense since the write fails
			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
			*/

			return err;
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDTOCLIENT,
				2, _ulIdentifier, (const char *) bResponse);
			_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (bResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	{
		unsigned long				ulTrackIdentifier;
		#ifdef WIN32
			__int64					ullPauseTimeInMilliSeconds;
		#else
			unsigned long long		ullPauseTimeInMilliSeconds;
		#endif


		if (_mtRTSPSession. lock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_LOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (DateTime:: nowLocalInMilliSecs (
			&ullPauseTimeInMilliSeconds) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_DATETIME_NOWLOCALINMILLISECS_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		_ulStreamingTimeInSecs			+= ((unsigned long)
			(ullPauseTimeInMilliSeconds -
			_rsRTPSession. _ullPlayTimeInMilliSeconds) / 1000);

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_bActivated))
				continue;

			/*
			{
				RTPPacket_p				prpRTPPacket;


				while ((((_rsRTPSession. _ptiTracks) [
					ulTrackIdentifier]). _vBuiltRTPPackets). begin () !=
					(((_rsRTPSession. _ptiTracks) [
					ulTrackIdentifier]). _vBuiltRTPPackets). end ())
				{
					prpRTPPacket			= *((((_rsRTPSession. _ptiTracks) [
						ulTrackIdentifier]). _vBuiltRTPPackets). begin ());

					if (prpRTPPacket -> reset () != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPPACKET_RESET_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (_mtRTSPSession. unLock () != errNoError)
						{
							Error err = PThreadErrors (__FILE__, __LINE__,
								THREADLIB_PMUTEX_UNLOCK_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bResponse. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bURI. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bRTSPVersion. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vFreeRTPPackets).
						insert (
						(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vFreeRTPPackets). end (),
						prpRTPPacket);

					(((_rsRTPSession. _ptiTracks) [
						ulTrackIdentifier]). _vBuiltRTPPackets). erase (
						(((_rsRTPSession. _ptiTracks) [
						ulTrackIdentifier]). _vBuiltRTPPackets). begin ());

					(((_rsRTPSession. _ptiTracks) [
						ulTrackIdentifier]). _vTransmissionTimeBuiltRTPPackets). erase (
						(((_rsRTPSession. _ptiTracks) [
						ulTrackIdentifier]). _vTransmissionTimeBuiltRTPPackets). begin ());
				}
			}
			*/

			if (deactiveTrack (&((_rsRTPSession. _ptiTracks) [
				ulTrackIdentifier])) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_DEACTIVETRACK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (setRTSPStatus (RTSPSession:: SS_RTSP_READY) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// da gestire con la nuova architettura
	// _tLastPauseTime			= time (NULL);


	return errNoError;
}


Error RTSPSession:: handleTEARDOWNMethod (const char *pRTSPRequest,
	unsigned long *pulMethodLength, Boolean_p pbIsRTSPFinished)

{

	Buffer_t						bURI;
	long							lSequenceNumber;
	char							pSessionIdentifier [
		SS_MAXSESSIONIDENTIFIERLENGTH];

	Error							errWrite;
	char							pTimestamp [SS_MAXLONGLENGTH];
	RTSPStatus_t					rsRTSPStatus;


	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus != RTSPSession:: SS_RTSP_READY &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_PLAYING &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_RECORDING)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "TEARDOWN", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bURI. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (RTSPUtility:: parseTEARDOWNRequest (
		pRTSPRequest,

		(Buffer_p) NULL,
		&bURI,
		&lSequenceNumber,
		pTimestamp,
		pSessionIdentifier,
		pulMethodLength,
		_ptSystemTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_PARSETEARDOWNREQUEST_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		Buffer_t						bResponse;

		if (bResponse. init () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				pSessionIdentifier, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (RTSPUtility:: getTEARDOWNResponse (
			lSequenceNumber, pSessionIdentifier,
			_ulRTSP_RTCPTimeoutInSecs,
			pTimestamp, &bResponse,
			_ptSystemTracer) != errNoError)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_GETTEARDOWNRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				pSessionIdentifier, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if ((errWrite = _psiClientSocketImpl -> writeString (
			(const char *) bResponse, true, 1, 0)) != errNoError)
		{
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errWrite, __FILE__, __LINE__);
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETIMPL_WRITE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			/*
				does not have sense since the write fails
			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
			*/

			return err;
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDTOCLIENT,
				2, _ulIdentifier, (const char *) bResponse);
			_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (bResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	{
		if (_bVideoTrackInitializedFromSetup &&
			!strcmp ((const char *) _bVideoRequestURI,
			(const char *) bURI))
		{
			/*
				It is not useful to move the RTP packets into
				the _vFreeRTPPackets vector because in the next
				command, we will delete the track

			if (_prpsRTPPacketsToSend -> moveRTPPacketsToVector (
				_ulIdentifier, _ulVideoTrackIdentifier,
				&(((_rsRTPSession. _ptiTracks) [_ulVideoTrackIdentifier]).
				_vFreeRTPPackets)) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTPPACKETSTOSEND_MOVERTPPACKETSTOVECTOR_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			*/

			if (deleteTrackToStream (_bIsLive, _ulVideoTrackIdentifier) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_DELETETRACKTOSTREAM_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			_bVideoTrackInitializedFromSetup		= false;

			if (!_bAudioTrackInitializedFromSetup)
			{
				// commented because this state is initialized by closeRTSPConnection
				//	called next to this method
				// rsRTSPStatus					= RTSPSession:: SS_RTSP_NOTUSED;
				*pbIsRTSPFinished				= true;

				// commented because the finishRTPSession is called by closeRTSPConnection
				//	called next to this method
				/*
				if (finishRTPSession () != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bURI. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bRTSPVersion. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
				*/
			}
			else
				// in this case the RTSP status doesn't change
				;
		}
		else if (_bAudioTrackInitializedFromSetup &&
			!strcmp ((const char *) _bAudioRequestURI,
			(const char *) bURI))
		{
			/*
				It is not useful to move the RTP packets into
				the _vFreeRTPPackets vector because in the next
				command, we will delete the track

			if (_prpsRTPPacketsToSend -> moveRTPPacketsToVector (
				_ulIdentifier, _ulAudioTrackIdentifier,
				&(((_rsRTPSession. _ptiTracks) [_ulAudioTrackIdentifier]).
				_vFreeRTPPackets)) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTPPACKETSTOSEND_MOVERTPPACKETSTOVECTOR_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			*/

			if (deleteTrackToStream (_bIsLive, _ulAudioTrackIdentifier) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_DELETETRACKTOSTREAM_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			_bAudioTrackInitializedFromSetup		= false;

			if (!_bVideoTrackInitializedFromSetup)
			{
				// commented because this state is initialized by closeRTSPConnection
				//	called next to this method
				// rsRTSPStatus					= RTSPSession:: SS_RTSP_NOTUSED;
				*pbIsRTSPFinished				= true;

				// commented because the finishRTPSession is called by closeRTSPConnection
				//	called next to this method
				/*
				if (finishRTPSession () != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bURI. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bRTSPVersion. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
				*/
			}
			else
				// in this case the RTSP status doesn't change
				;
		}
		else
		{
			// TEARDOWN is for all the session

			// commented because this state is initialized by closeRTSPConnection
			//	called next to this method
			// rsRTSPStatus					= RTSPSession:: SS_RTSP_NOTUSED;
			*pbIsRTSPFinished				= true;

			if (_bVideoTrackInitializedFromSetup)
			{
				/*
					It is not useful to move the RTP packets into
					the _vFreeRTPPackets vector because in the next
					command, we will delete the track

				if (_prpsRTPPacketsToSend -> moveRTPPacketsToVector (
					_ulIdentifier, _ulVideoTrackIdentifier,
					&(((_rsRTPSession. _ptiTracks) [_ulVideoTrackIdentifier]).
					_vFreeRTPPackets)) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTPPACKETSTOSEND_MOVERTPPACKETSTOVECTOR_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bURI. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bRTSPVersion. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
				*/

				if (deleteTrackToStream (_bIsLive, _ulVideoTrackIdentifier) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_DELETETRACKTOSTREAM_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bURI. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			if (_bAudioTrackInitializedFromSetup)
			{
				/*
					It is not useful to move the RTP packets into
					the _vFreeRTPPackets vector because in the next
					command, we will delete the track

				if (_prpsRTPPacketsToSend -> moveRTPPacketsToVector (
					_ulIdentifier, _ulAudioTrackIdentifier,
					&(((_rsRTPSession. _ptiTracks) [_ulAudioTrackIdentifier]).
					_vFreeRTPPackets)) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTPPACKETSTOSEND_MOVERTPPACKETSTOVECTOR_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bURI. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bRTSPVersion. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
				*/

				if (deleteTrackToStream (_bIsLive, _ulAudioTrackIdentifier) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_DELETETRACKTOSTREAM_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bURI. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			// commented because the finishRTPSession is called by closeRTSPConnection
			//	called next to this method
			/*
			if (finishRTPSession () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			*/
		}
	}

	if (*pbIsRTSPFinished)
	{
		if (_rcPreviousRTSPCommand == SS_RTSP_PLAY)
		{
			#ifdef WIN32
				__int64					ullTeardownTimeInMilliSeconds;
			#else
				unsigned long long		ullTeardownTimeInMilliSeconds;
			#endif

			if (DateTime:: nowLocalInMilliSecs (
				&ullTeardownTimeInMilliSeconds) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_DATETIME_NOWLOCALINMILLISECS_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			_ulStreamingTimeInSecs			+= ((unsigned long)
				(ullTeardownTimeInMilliSeconds -
				_rsRTPSession. _ullPlayTimeInMilliSeconds) / 1000);
		}
	}

	if (bURI. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: handleANNOUNCEMethod (const char *pRTSPRequest,
	unsigned long *pulMethodLength)

{

	// no effect on the server state

	RTSPStatus_t					rsRTSPStatus;


	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			"", false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus == RTSPSession:: SS_RTSP_NOTUSED ||
		rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "ANNOUNCE", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			"", false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (sendRTSPResponse (_psiClientSocketImpl,
		RTSP_PREDEFINEDRTSPVERSION, 501, -1,
		(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
		"", false) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pulMethodLength			= (unsigned long)
		(strchr (pRTSPRequest, '\0') - pRTSPRequest);


	return errNoError;
}


Error RTSPSession:: handleGET_PARAMETERMethod (const char *pRTSPRequest,
	unsigned long *pulMethodLength)

{

	// no effect on the server state

	RTSPStatus_t					rsRTSPStatus;


	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			"", false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus == RTSPSession:: SS_RTSP_NOTUSED ||
		rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "GET_PARAMETER", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			"", false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (sendRTSPResponse (_psiClientSocketImpl,
		RTSP_PREDEFINEDRTSPVERSION, 501, -1,
		(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "", false) !=
		errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pulMethodLength			= (unsigned long)
		(strchr (pRTSPRequest, '\0') - pRTSPRequest);


	return errNoError;
}


Error RTSPSession:: handleOPTIONSMethod (const char *pRTSPRequest,
	unsigned long *pulMethodLength)

{

	long							lSequenceNumber;
	char							pSessionIdentifier [
		SS_MAXSESSIONIDENTIFIERLENGTH];
	Error							errWrite;
	char							pTimestamp [SS_MAXLONGLENGTH];

	// no effect on the server state

	/*
	RTSPStatus_t					rsRTSPStatus;


	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			"", false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus == RTSPSession:: SS_RTSP_NOTUSED ||
		rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "OPTIONS", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			"", false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	*/

	if (RTSPUtility:: parseOPTIONSRequest (
		pRTSPRequest,

		(Buffer_p) NULL,
		(Buffer_p) NULL,
		&lSequenceNumber,
		pTimestamp,
		pSessionIdentifier,
		pulMethodLength,
		_ptSystemTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_PARSETEARDOWNREQUEST_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 500, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		Buffer_t						bResponse;

		if (bResponse. init () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				pSessionIdentifier, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (RTSPUtility:: getOPTIONSResponse (
			lSequenceNumber,
			!strcmp (pSessionIdentifier, "") ? (const char *) NULL :
			pSessionIdentifier,
			_ulRTSP_RTCPTimeoutInSecs,
			pTimestamp,
			_rcPreviousRTSPCommand == SS_RTSP_PAUSE,
	
			&bResponse,
			_ptSystemTracer) != errNoError)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_GETOPTIONSRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				pSessionIdentifier, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if ((errWrite = _psiClientSocketImpl -> writeString (
			(const char *) bResponse, true, 1, 0)) != errNoError)
		{
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errWrite, __FILE__, __LINE__);
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETIMPL_WRITE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			/*
				does not have sense since the write fails
			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				RTSP_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
			*/

			return err;
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDTOCLIENT,
				2, _ulIdentifier, (const char *) bResponse);
			_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (bResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error RTSPSession:: handleRECORDMethod (const char *pRTSPRequest,
	unsigned long *pulMethodLength)

{

	RTSPStatus_t					rsRTSPStatus;


	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			"", false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus == RTSPSession:: SS_RTSP_NOTUSED ||
		rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "RECORD", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			"", false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (sendRTSPResponse (_psiClientSocketImpl,
		RTSP_PREDEFINEDRTSPVERSION, 501, -1,
		(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
		false) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	*pulMethodLength			= (unsigned long)
		(strchr (pRTSPRequest, '\0') - pRTSPRequest);

	/*
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_READY &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_RECORDING)
	{
		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION,
			455, -1, (const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "",
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	_rsRTSPStatus			= RTSPSession:: SS_RTSP_RECORDING;
	*/


	return errNoError;
}


Error RTSPSession:: handleREDIRECTMethod (const char *pRTSPRequest,
	unsigned long *pulMethodLength)

{

	// but can a server receive REDIRECT???

	RTSPStatus_t					rsRTSPStatus;


	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			"", false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus == RTSPSession:: SS_RTSP_NOTUSED ||
		rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "REDIRECT", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			"", false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (sendRTSPResponse (_psiClientSocketImpl,
		RTSP_PREDEFINEDRTSPVERSION, 501, -1,
		(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "", false) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	*pulMethodLength			= (unsigned long)
		(strchr (pRTSPRequest, '\0') - pRTSPRequest);


	return errNoError;
}


Error RTSPSession:: handleSET_PARAMETERMethod (const char *pRTSPRequest,
	unsigned long *pulMethodLength)

{

	// no effect on the server state

	RTSPStatus_t					rsRTSPStatus;


	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			"", false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus == RTSPSession:: SS_RTSP_NOTUSED ||
		rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "SET_PARAMETER", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			RTSP_PREDEFINEDRTSPVERSION, 455, -1,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			"", false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (sendRTSPResponse (_psiClientSocketImpl,
		RTSP_PREDEFINEDRTSPVERSION, 501, -1,
		(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, "", false) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	*pulMethodLength			= (unsigned long)
		(strchr (pRTSPRequest, '\0') - pRTSPRequest);


	return errNoError;
}


Error RTSPSession:: getRTSPStatus (RTSPStatus_p prsRTSPStatus)

{

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*prsRTSPStatus			= _rsRTSPStatus;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: setRTSPStatus (RTSPStatus_t rsRTSPStatus)

{

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	_rsRTSPStatus				= rsRTSPStatus;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: getIdentifier (
	unsigned long *pulIdentifier)

{

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pulIdentifier			= _ulIdentifier;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: getRTSP_RTCPTimes (
	RTSP_RTCPTimes_p *prrtRTSP_RTCPTimes)

{

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*prrtRTSP_RTCPTimes			= &_rrtRTSP_RTCPTimes;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: getPauseTimes (
	PauseTimes_p *pptPauseTimes)

{

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pptPauseTimes			= &_ptPauseTimes;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: getStreamingSessionTimes (
	StreamingSessionTimes_p *pssStreamingSessionTimes)

{

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pssStreamingSessionTimes		= &_sstStreamingSessionTimes;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: getCheckReceivedRTCPPacketsTimes (
	CheckReceivedRTCPPacketsTimes_p *pcrrptCheckReceivedRTCPPacketsTimes)

{

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pcrrptCheckReceivedRTCPPacketsTimes			= &_crrptCheckReceivedRTCPPacketsTimes;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: getSendRTCPPacketsTimes (
	SendRTCPPacketsTimes_p *psrptSendRTCPPacketsTimes)

{

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*psrptSendRTCPPacketsTimes			= &_srptSendRTCPPacketsTimes;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: getClientSocket (
	ClientSocket_p *pcsClientSocket)

{

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pcsClientSocket			= &_csClientSocket;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: getRandomOffsets (
	unsigned long ulTrackIdentifier,
	unsigned short *pusRtpSequenceNumberRandomOffset,
	unsigned long *pulRtpTimestampRandomOffset)

{

	RTSPStatus_t					rsRTSPStatus;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus != SS_RTSP_PLAYING)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "getRandomOffsets", (long) rsRTSPStatus);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		//	(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	*pusRtpSequenceNumberRandomOffset		=
		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_usRtpSequenceNumberRandomOffset;

	*pulRtpTimestampRandomOffset			=
		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_ulRtpTimestampRandomOffset;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: getVideoTrackIdentifier (
	unsigned long *pulTrackIdentifier)

{

	RTSPStatus_t					rsRTSPStatus;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus != SS_RTSP_PLAYING)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "getVideoTrackIdentifier", (long) rsRTSPStatus);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		//	(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bVideoTrackFoundInSDP)
		*pulTrackIdentifier		= _ulVideoTrackIdentifier;
	else
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_TRACKNOTFOUND);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: getAudioTrackIdentifier (
	unsigned long *pulTrackIdentifier)

{

	RTSPStatus_t					rsRTSPStatus;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus != SS_RTSP_PLAYING)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "getAudioTrackIdentifier", (long) rsRTSPStatus);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		//	(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bAudioTrackFoundInSDP)
		*pulTrackIdentifier		= _ulAudioTrackIdentifier;
	else
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_TRACKNOTFOUND);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: acceptRTSPConnection (
	ServerSocket_p pssServerSocket)

{

	Error_t							errAcceptConnection;
	RTSPStatus_t					rsRTSPStatus;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus != SS_RTSP_NOTUSED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "acceptRTSPConnection", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((errAcceptConnection = pssServerSocket -> acceptConnection (
		&_csClientSocket)) != errNoError)
	{
		Boolean_t				bIsThereError;


		bIsThereError			= false;

		if ((long) errAcceptConnection == SCK_ACCEPT_FAILED)
		{
			int					iErrno;
			unsigned long		ulUserDataBytes;

			errAcceptConnection. getUserData (&iErrno, &ulUserDataBytes);
			#ifdef WIN32
				if (iErrno == WSAEWOULDBLOCK)
			#else
				if (iErrno == EAGAIN)
			#endif
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_NORTSPCONNECTIONTOACCEPT_FAILED);
				// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				//	(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			else
				bIsThereError			= true;
		}
		else
			bIsThereError			= true;

		if (bIsThereError)
		{
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errAcceptConnection,
				__FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_ACCEPTCONNECTION_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	_tUTCStartConnectionTime			= time (NULL);
	_ulStreamingTimeInSecs				= 0;

	if (DateTime:: get_tm_LocalTime (
		&_tmLocalDateTimeStartConnection,
		&_ulStartConnectionMilliSeconds) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_DATETIME_GET_TM_LOCALTIME_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_rrtRTSP_RTCPTimes. start () != errNoError)
	{
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_TIMES_START_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_pspSocketsPool -> addSocket (
		SocketsPool:: SOCKETSTATUS_READ |
		SocketsPool:: SOCKETSTATUS_EXCEPTION,
		SS_STREAMINGSERVERSOCKETPOOL_RTSPSESSIONSOCKET,
		&_csClientSocket, &_ulIdentifier) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETSPOOL_ADDSOCKET_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_rrtRTSP_RTCPTimes. stop () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_TIMES_STOP_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_csClientSocket. getSocketImpl (
		&_psiClientSocketImpl) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKET_GETSOCKETIMPL_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		{
			unsigned long			*pulRTSPIdentifier;


			if (_pspSocketsPool -> deleteSocket (
				&_csClientSocket, (void **) (&pulRTSPIdentifier)) !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (_rrtRTSP_RTCPTimes. stop () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_TIMES_STOP_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_psiClientSocketImpl -> getRemoteAddress (
		_pClientIPAddress, SCK_MAXIPADDRESSLENGTH) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_GETREMOTEADDRESS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		{
			unsigned long			*pulRTSPIdentifier;


			if (_pspSocketsPool -> deleteSocket (
				&_csClientSocket, (void **) (&pulRTSPIdentifier)) !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (_rrtRTSP_RTCPTimes. stop () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_TIMES_STOP_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_psiClientSocketImpl -> getRemotePort (
		&_lRTSPClientPort) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_GETREMOTEPORT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		{
			unsigned long			*pulRTSPIdentifier;


			if (_pspSocketsPool -> deleteSocket (
				&_csClientSocket, (void **) (&pulRTSPIdentifier)) !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (_rrtRTSP_RTCPTimes. stop () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_TIMES_STOP_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_psiClientSocketImpl -> setNoDelay (true) !=
		errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_SETNODELAY_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		{
			unsigned long			*pulRTSPIdentifier;


			if (_pspSocketsPool -> deleteSocket (
				&_csClientSocket, (void **) (&pulRTSPIdentifier)) !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (_rrtRTSP_RTCPTimes. stop () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_TIMES_STOP_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_psiClientSocketImpl -> setKeepAlive (true) !=
		errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_SETKEEPALIVE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		{
			unsigned long			*pulRTSPIdentifier;


			if (_pspSocketsPool -> deleteSocket (
				&_csClientSocket, (void **) (&pulRTSPIdentifier)) !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (_rrtRTSP_RTCPTimes. stop () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_TIMES_STOP_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_psiClientSocketImpl -> setMaxSendBuffer (8 * 1024) !=
		errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_SETMAXSENDBUFFER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		{
			unsigned long			*pulRTSPIdentifier;


			if (_pspSocketsPool -> deleteSocket (
				&_csClientSocket, (void **) (&pulRTSPIdentifier)) !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (_rrtRTSP_RTCPTimes. stop () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_TIMES_STOP_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_RTSPSESSION_CONNECTIONARRIVED,
			3, _ulIdentifier, _pClientIPAddress, _lRTSPClientPort);
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (setRTSPStatus (RTSPSession:: SS_RTSP_CONNECTIONACCEPTED) !=
		errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


/*
Error RTSPSession:: addRTSPSessionForTheLiveSource (void)

{

	LiveEvent:: LiveSource_p		plsLiveSource;
	Boolean_t			bIsLiveSourceFound;
	Error_t				errSocketInit;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_rsRTSPStatus != RTSPSession:: SS_RTSP_PLAYING)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (!_bIsLive)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_NOLIVERTSPSESSION);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// check if LiveSource already exist
	{
		unsigned long		ulPortToSearch;
		Socket_p			pssLiveServerSocket;
		Error_t				errSearch;


		if (_bVideoTrackFoundInSDP)
			ulPortToSearch		= _ulLiveVideoRTPPacketsServerPort;
		else
			ulPortToSearch		= _ulLiveAudioRTPPacketsServerPort;

		if ((errSearch = _pspSocketsPool -> findServerSocket (
			ulPortToSearch,
			&pssLiveServerSocket, (void **) (&plsLiveSource))) !=
			errNoError)
		{
			if ((long) errSearch == SCK_SOCKETSPOOL_SOCKETNOTFOUND)
				bIsLiveSourceFound		= false;
			else
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINDSERVERSOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
			bIsLiveSourceFound		= true;
	}

	if (!bIsLiveSourceFound)
	{
		// new live source

		if ((plsLiveSource = new LiveEvent:: LiveSource_t) ==
			(LiveEvent:: LiveSource_p) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_NEW_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		plsLiveSource -> _bVideoTrack		= _bVideoTrackFoundInSDP;
		plsLiveSource -> _bAudioTrack		= _bAudioTrackFoundInSDP;

		if (plsLiveSource -> _bVideoTrack)
		{
			plsLiveSource -> _ulVideoPort	= _ulLiveVideoRTPPacketsServerPort;

			if ((errSocketInit = (plsLiveSource -> _ssLiveVideoServerSocket). init (
				_pLocalIPAddressForRTP,
				plsLiveSource -> _ulVideoPort,
				true, SocketImpl:: DGRAM)) != errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errSocketInit, __FILE__, __LINE__);

				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete plsLiveSource;

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (_pspSocketsPool -> addSocket (
				SocketsPool:: SOCKETSTATUS_READ |
				SocketsPool:: SOCKETSTATUS_EXCEPTION,
				SS_STREAMINGSERVERSOCKETPOOL_LIVESERVERSOCKET,
				&(plsLiveSource -> _ssLiveVideoServerSocket),
				plsLiveSource) != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_ADDSOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if ((plsLiveSource -> _ssLiveVideoServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete plsLiveSource;

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (plsLiveSource -> _bAudioTrack)
		{
			plsLiveSource -> _ulAudioPort	= _ulLiveAudioRTPPacketsServerPort;

			if ((errSocketInit = (plsLiveSource -> _ssLiveAudioServerSocket). init (
				_pLocalIPAddressForRTP,
				plsLiveSource -> _ulAudioPort,
				true, SocketImpl:: DGRAM)) != errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errSocketInit, __FILE__, __LINE__);

				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (plsLiveSource -> _bVideoTrack)
				{
					LiveEvent:: LiveSource_p	plsLocalLiveSource;

					if (_pspSocketsPool -> deleteSocket (
						&(plsLiveSource -> _ssLiveVideoServerSocket),
						(void **) (&plsLocalLiveSource)) !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((plsLiveSource -> _ssLiveVideoServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				delete plsLiveSource;

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (_pspSocketsPool -> addSocket (
				SocketsPool:: SOCKETSTATUS_READ |
				SocketsPool:: SOCKETSTATUS_EXCEPTION,
				SS_STREAMINGSERVERSOCKETPOOL_LIVESERVERSOCKET,
				&(plsLiveSource -> _ssLiveAudioServerSocket),
				plsLiveSource) != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_ADDSOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if ((plsLiveSource -> _ssLiveAudioServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (plsLiveSource -> _bVideoTrack)
				{
					LiveEvent:: LiveSource_p	plsLocalLiveSource;

					if (_pspSocketsPool -> deleteSocket (
						&(plsLiveSource -> _ssLiveVideoServerSocket),
						(void **) (&plsLocalLiveSource)) !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((plsLiveSource -> _ssLiveVideoServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				delete plsLiveSource;

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}


	{
		std:: vector<unsigned long>:: iterator	itRTSPSession;
		unsigned long		ulLocalRTSPSessionIdentifier;

		for (itRTSPSession = (plsLiveSource -> _vActiveLiveRTSPSessions). begin ();
			itRTSPSession != (plsLiveSource -> _vActiveLiveRTSPSessions). end ();
			++itRTSPSession)
		{
			ulLocalRTSPSessionIdentifier		= *itRTSPSession;

			if (ulLocalRTSPSessionIdentifier == _ulIdentifier)
				break;
		}

		if (itRTSPSession == (plsLiveSource -> _vActiveLiveRTSPSessions). end ())
		{
			(plsLiveSource -> _vActiveLiveRTSPSessions). insert (
				(plsLiveSource -> _vActiveLiveRTSPSessions). end (),
				_ulIdentifier);
		}
	}


	return errNoError;
}


Error RTSPSession:: deleteRTSPSessionForTheLiveSource (void)

{

	LiveEvent:: LiveSource_p	plsLiveSource;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_rsRTSPStatus == RTSPSession:: SS_RTSP_NOTUSED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (!_bIsLive)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_NOLIVERTSPSESSION);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		unsigned long		ulPortToSearch;
		Socket_p			pssLiveServerSocket;


		if (_bVideoTrackFoundInSDP)
			ulPortToSearch		= _ulLiveVideoRTPPacketsServerPort;
		else
			ulPortToSearch		= _ulLiveAudioRTPPacketsServerPort;

		if (_pspSocketsPool -> findServerSocket (
			ulPortToSearch,
			&pssLiveServerSocket, (void **) (&plsLiveSource)) !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINDSERVERSOCKET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			plsLiveSource		= (LiveEvent:: LiveSource_p) NULL;

			// if (_mtRTSPSession. unLock () != errNoError)
			// {
			//	Error err = PThreadErrors (__FILE__, __LINE__,
			//		THREADLIB_PMUTEX_UNLOCK_FAILED);
			//	_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			//		(const char *) err, __FILE__, __LINE__);
			//}

			// return err;
		}
	}

	if (plsLiveSource != (LiveEvent:: LiveSource_p) NULL)
	{
		std:: vector<unsigned long>:: iterator	itRTSPSession;
		unsigned long		ulLocalRTSPSessionIdentifier;


		for (itRTSPSession = (plsLiveSource -> _vActiveLiveRTSPSessions). begin ();
			itRTSPSession != (plsLiveSource -> _vActiveLiveRTSPSessions). end ();
			++itRTSPSession)
		{
			ulLocalRTSPSessionIdentifier		= *itRTSPSession;

			if (ulLocalRTSPSessionIdentifier == _ulIdentifier)
				break;
		}

		if (itRTSPSession == (plsLiveSource -> _vActiveLiveRTSPSessions). end ())
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERPROCESSOR_LIVERTSPSESSIONNOTFOUND);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			//if (_mtRTSPSession. unLock () != errNoError)
			//{
			//	Error err = PThreadErrors (__FILE__, __LINE__,
			//		THREADLIB_PMUTEX_UNLOCK_FAILED);
			//	_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			//		(const char *) err, __FILE__, __LINE__);
			//}

			//return err;
		}
		else
		{
			(plsLiveSource -> _vActiveLiveRTSPSessions). erase (itRTSPSession);

			if ((plsLiveSource -> _vActiveLiveRTSPSessions). size () == 0)
			{
				if (plsLiveSource -> _bVideoTrack)
				{
					LiveEvent:: LiveSource_p	plsLocalLiveSource;

					if (_pspSocketsPool -> deleteSocket (
						&(plsLiveSource -> _ssLiveVideoServerSocket),
						(void **) (&plsLocalLiveSource)) !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						//if (_mtRTSPSession. unLock () != errNoError)
						//{
						//	Error err = PThreadErrors (__FILE__, __LINE__,
						//		THREADLIB_PMUTEX_UNLOCK_FAILED);
						//	_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						//		(const char *) err, __FILE__, __LINE__);
						//}

						// return err;
					}

					if ((plsLiveSource -> _ssLiveVideoServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						//if (_mtRTSPSession. unLock () != errNoError)
						//{
						//	Error err = PThreadErrors (__FILE__, __LINE__,
						//		THREADLIB_PMUTEX_UNLOCK_FAILED);
						//	_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						//		(const char *) err, __FILE__, __LINE__);
						//}

						//return err;
					}
				}

				if (plsLiveSource -> _bAudioTrack)
				{
					LiveEvent:: LiveSource_p	plsLocalLiveSource;

					if (_pspSocketsPool -> deleteSocket (
						&(plsLiveSource -> _ssLiveAudioServerSocket),
						(void **) (&plsLocalLiveSource)) !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						//if (_mtRTSPSession. unLock () != errNoError)
						//{
						//	Error err = PThreadErrors (__FILE__, __LINE__,
						//		THREADLIB_PMUTEX_UNLOCK_FAILED);
						//	_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						//		(const char *) err, __FILE__, __LINE__);
						//}

						//return err;
					}

					if ((plsLiveSource -> _ssLiveAudioServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						//if (_mtRTSPSession. unLock () != errNoError)
						//{
						//	Error err = PThreadErrors (__FILE__, __LINE__,
						//		THREADLIB_PMUTEX_UNLOCK_FAILED);
						//	_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						//		(const char *) err, __FILE__, __LINE__);
						//}

						//return err;
					}
				}

				delete plsLiveSource;
			}
		}
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}
*/


Error RTSPSession:: addRTSPSessionForTheLiveSource (void)

{

	LiveSource_p		plsLiveSource;
	Boolean_t			bIsLiveSourceFound;
	Error_t				errSocketInit;
	unsigned long		ulSelectedLiveSource;
	RTSPStatus_t		rsRTSPStatus;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus != RTSPSession:: SS_RTSP_READY)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "addRTSPSessionForTheLiveSource",
			(long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (!_bIsLive)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_NOLIVERTSPSESSION);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_pmtLiveSources -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// check if LiveSource already exist
	{
		unsigned long		ulPortToSearch;
		Socket_p			pssLiveServerSocket;
		Error_t				errSearch;


		if (_bVideoTrackFoundInSDP)
			ulPortToSearch		= _ulLiveVideoRTPPacketsServerPort;
		else
			ulPortToSearch		= _ulLiveAudioRTPPacketsServerPort;

		// findServerSocket is not very efficient
		if ((errSearch = _pspSocketsPool -> findServerSocket (
			ulPortToSearch,
			&pssLiveServerSocket, (void **) (&plsLiveSource))) !=
			errNoError)
		{
			if ((long) errSearch == SCK_SOCKETSPOOL_SOCKETNOTFOUND)
				bIsLiveSourceFound		= false;
			else
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINDSERVERSOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_pmtLiveSources -> unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
			bIsLiveSourceFound		= true;
	}

	if (!bIsLiveSourceFound)
	{
		Boolean_t			bIsInitialized;


		for (ulSelectedLiveSource = 0;
			ulSelectedLiveSource < _ulMaxLiveSourcesNumber;
			ulSelectedLiveSource++)
		{
			if ((_plsLiveSources [ulSelectedLiveSource]).
				isInitialized (&bIsInitialized) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_LIVESOURCE_ISINITIALIZED_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_pmtLiveSources -> unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (!bIsInitialized)
				break;
		}

		if (ulSelectedLiveSource == _ulMaxLiveSourcesNumber)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_NOLIVESOURCEAVAILABLE);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pmtLiveSources -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		plsLiveSource	= &(_plsLiveSources [ulSelectedLiveSource]);

		if (plsLiveSource -> init (
			_pspSocketsPool,
			_pLocalIPAddressForRTP,
			_bVideoTrackFoundInSDP,
			_ulLiveVideoRTPPacketsServerPort,
			_bAudioTrackFoundInSDP,
			_ulLiveAudioRTPPacketsServerPort,
			_ptSystemTracer) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_LIVESOURCE_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pmtLiveSources -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (plsLiveSource -> addRTSPSession (
		_ulIdentifier) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_LIVESOURCE_ADDRTSPSESSION_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (!bIsLiveSourceFound)
		{
			if (plsLiveSource -> finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_LIVESOURCE_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (_pmtLiveSources -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_pmtLiveSources -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: deleteRTSPSessionForTheLiveSource (void)

{

	LiveSource_p		plsLiveSource;
	RTSPStatus_t		rsRTSPStatus;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus == RTSPSession:: SS_RTSP_NOTUSED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "deleteRTSPSessionForTheLiveSource",
			(long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (!_bIsLive)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_NOLIVERTSPSESSION);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_pmtLiveSources -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		unsigned long		ulPortToSearch;
		Socket_p			pssLiveServerSocket;


		if (_bVideoTrackFoundInSDP)
			ulPortToSearch		= _ulLiveVideoRTPPacketsServerPort;
		else
			ulPortToSearch		= _ulLiveAudioRTPPacketsServerPort;

		// findServerSocket is not very efficient
		if (_pspSocketsPool -> findServerSocket (
			ulPortToSearch,
			&pssLiveServerSocket, (void **) (&plsLiveSource)) !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINDSERVERSOCKET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pmtLiveSources -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	{
		Boolean_t		bIsLiveSourceEmpty;


		if (plsLiveSource -> deleteRTSPSession (
			_ulIdentifier, &bIsLiveSourceEmpty) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_LIVESOURCE_DELETERTSPSESSION_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pmtLiveSources -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bIsLiveSourceEmpty)
		{
			if (plsLiveSource -> finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_LIVESOURCE_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_pmtLiveSources -> unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	if (_pmtLiveSources -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: sendLiveRTPPacket (
	Boolean_t bIsVideoRTPPacket,
	unsigned short usRTPPacketSequenceNumberFromLiveEncoder,
	unsigned long ulRTPPacketTimeStampFromLiveEncoder,
	RTPPacket_p prpLiveRTPPacket)

{

	unsigned long		ulTrackIdentifier;
	double				dAbsoluteRTPPacketTransmissionTimeInMilliSecs;
	#ifdef WIN32
		__int64				ullSuggestedWakeupTimeInMilliSecs;
	#else
		unsigned long long	ullSuggestedWakeupTimeInMilliSecs;
	#endif
	RTSPStatus_t		rsRTSPStatus;
	unsigned short		usPLAYAnswerAndFirstPacketToSendSequenceNumberDifference;
	unsigned long		ulPLAYAnswerAndFirstPacketToSendTimeStampDifference;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus != RTSPSession:: SS_RTSP_PLAYING)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "sendLiveRTPPacket", (long) rsRTSPStatus);

		if (rsRTSPStatus != RTSPSession:: SS_RTSP_READY)	// PAUSE
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bIsVideoRTPPacket)
	{
		if (_bIsFirstLiveVideoRTPPacketToSend)
		{
			/*
			{
			unsigned char		ucMpeg4VopType;
			unsigned char		*pucPayload;
			unsigned long		ulPayloadLength;

			pucPayload			=
				(unsigned char *) (*prpLiveRTPPacket);
			ulPayloadLength		= (unsigned long)
				((unsigned long long) (*prpLiveRTPPacket));

			ucMpeg4VopType		= MP4AV_Mpeg4GetVopType (
				pucPayload + 12,
				ulPayloadLength - 12);

			if (ucMpeg4VopType != 'I')
			{
				char		pBuff [1000];
				Boolean_t	bMarkerBit;

				prpLiveRTPPacket -> getMarkerBit (&bMarkerBit);
				sprintf (pBuff, "Skipped frame: VopType: '%c', video marker bit: %ld",
					ucMpeg4VopType, bMarkerBit);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					pBuff, __FILE__, __LINE__);


				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return errNoError;
			}

			{
				char		pMpeg4VopType [1024];
				Boolean_t	bMarkerBit;

				prpLiveRTPPacket -> getMarkerBit (&bMarkerBit);
				sprintf (pMpeg4VopType, "VopType: %c, video marker bit: %ld",
					ucMpeg4VopType, bMarkerBit);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					pMpeg4VopType, __FILE__, __LINE__);
			}
			}
			*/

			// Calcolate the difference between the sequence number
			// included in the PLAY answer and the sequence number
			// of the first RTP packet to send in order to have
			// the correction factor for all the packets to send

			_usPLAYAnswerAndFirstPacketToSendVideoSequenceNumberDifference	=
				usRTPPacketSequenceNumberFromLiveEncoder -
				_usPLAYAnswerVideoSequenceNumber;

			_ulPLAYAnswerAndFirstPacketToSendVideoTimeStampDifference	=
				ulRTPPacketTimeStampFromLiveEncoder -
				_ulPLAYAnswerVideoTimeStamp;

			_bIsFirstLiveVideoRTPPacketToSend			= false;
		}

		usPLAYAnswerAndFirstPacketToSendSequenceNumberDifference		=
			_usPLAYAnswerAndFirstPacketToSendVideoSequenceNumberDifference;

		ulPLAYAnswerAndFirstPacketToSendTimeStampDifference		=
			_ulPLAYAnswerAndFirstPacketToSendVideoTimeStampDifference;

		if (_bVideoTrackFoundInSDP)
			ulTrackIdentifier		= _ulVideoTrackIdentifier;
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_TRACKNOTFOUND);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		if (_bIsFirstLiveAudioRTPPacketToSend)
		{

			// we will wait that the first video packet is sent
			// before to send the audio packets
			// if (_bVideoTrackFoundInSDP && _bIsFirstLiveVideoRTPPacketToSend)
			// 	return errNoError;

			// Calcolate the difference between the sequence number
			// included in the PLAY answer and the sequence number
			// of the first RTP packet to send in order to have
			// the correction factor for all the packets to send

			_usPLAYAnswerAndFirstPacketToSendAudioSequenceNumberDifference	=
				usRTPPacketSequenceNumberFromLiveEncoder -
				_usPLAYAnswerAudioSequenceNumber;

			_ulPLAYAnswerAndFirstPacketToSendAudioTimeStampDifference	=
				ulRTPPacketTimeStampFromLiveEncoder -
				_ulPLAYAnswerAudioTimeStamp;

			_bIsFirstLiveAudioRTPPacketToSend			= false;
		}

/*
{
char ppp [1000];
Boolean_t	bMarkerBit;

prpLiveRTPPacket -> getMarkerBit (&bMarkerBit);
sprintf (ppp, "audio marker bit: %ld",
	bMarkerBit);
_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
	ppp, __FILE__, __LINE__);
}
*/

		usPLAYAnswerAndFirstPacketToSendSequenceNumberDifference		=
			_usPLAYAnswerAndFirstPacketToSendAudioSequenceNumberDifference;

		ulPLAYAnswerAndFirstPacketToSendTimeStampDifference		=
			_ulPLAYAnswerAndFirstPacketToSendAudioTimeStampDifference;

		if (_bAudioTrackFoundInSDP)
			ulTrackIdentifier		= _ulAudioTrackIdentifier;
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_TRACKNOTFOUND);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (prpLiveRTPPacket -> setSequenceNumber (
		usRTPPacketSequenceNumberFromLiveEncoder -
		usPLAYAnswerAndFirstPacketToSendSequenceNumberDifference) !=
		errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_SETSEQUENCENUMBER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (prpLiveRTPPacket -> setTimeStamp (
		ulRTPPacketTimeStampFromLiveEncoder -
		ulPLAYAnswerAndFirstPacketToSendTimeStampDifference) !=
		errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_SETTIMESTAMP_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	dAbsoluteRTPPacketTransmissionTimeInMilliSecs		= 0;

	if (sendRTPPacket (
		dAbsoluteRTPPacketTransmissionTimeInMilliSecs,	// not used because live
		ulTrackIdentifier,
		prpLiveRTPPacket,
		&ullSuggestedWakeupTimeInMilliSecs) !=		// not used because live
		errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_SENDRTPPACKET_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error RTSPSession:: closeRTSPConnection (
		Error_p perrError,
		__int64 *pullTotalBytesServed,
		__int64 *pullTotalLostPacketsNumber,
		__int64 *pullTotalSentRTPPacketsNumber)
#else
	Error RTSPSession:: closeRTSPConnection (
		Error_p perrError,
		unsigned long long *pullTotalBytesServed,
		unsigned long long *pullTotalLostPacketsNumber,
		unsigned long long *pullTotalSentRTPPacketsNumber)
#endif

{

	unsigned long				ulTrackIdentifier;
	RTSPStatus_t					rsRTSPStatus;


	*pullTotalBytesServed			= 0;
	*pullTotalLostPacketsNumber		= 0;
	*pullTotalSentRTPPacketsNumber		= 0;

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus == RTSPSession:: SS_RTSP_NOTUSED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "closeRTSPConnection", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	_tUTCEndConnectionTime			= time (NULL);

	for (ulTrackIdentifier = 0;
		ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
		ulTrackIdentifier++)
	{
		*pullTotalBytesServed			+= ((_rsRTPSession. _ptiTracks) [
			ulTrackIdentifier]). _ullRTPPacketBytesSent;

		*pullTotalLostPacketsNumber			+= ((_rsRTPSession. _ptiTracks) [
			ulTrackIdentifier]). _ulLostRTPPacketsNumber;

		*pullTotalSentRTPPacketsNumber			+= ((_rsRTPSession. _ptiTracks) [
			ulTrackIdentifier]). _ulSentRTPPacketsNumber;
	}

	// update of the streaming time
	if (rsRTSPStatus == RTSPSession:: SS_RTSP_PLAYING)
	{
		if (*perrError != errNoError)
		{
			// update the streaming time only in case there is an
			//	error and we are in PLAYING state. If we do not have
			//	any error the streaming time is already updated by the
			//	TEARDOWN or PAUSE methods
			#ifdef WIN32
				__int64					ullErrorTimeInMilliSeconds;
			#else
				unsigned long long		ullErrorTimeInMilliSeconds;
			#endif

			if (DateTime:: nowLocalInMilliSecs (
				&ullErrorTimeInMilliSeconds) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_DATETIME_NOWLOCALINMILLISECS_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			_ulStreamingTimeInSecs			+= ((unsigned long)
				(ullErrorTimeInMilliSeconds -
				_rsRTPSession. _ullPlayTimeInMilliSeconds) / 1000);
		}
	}

	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		if (_bStreamingSessionTimeoutActive)
		{
			if (_pscScheduler -> deactiveTimes (&_sstStreamingSessionTimes) !=
				errNoError)
			{
				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_SCHEDULER_DEACTIVETIMES_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err,
					__FILE__, __LINE__);

				/*
				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
				*/
			}

			if (_sstStreamingSessionTimes. finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSESSIONTIMES_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				/*
				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
				*/
			}
		}
	}

	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		if (_bIsLive)
		{
			if (deleteRTSPSessionForTheLiveSource () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_DELETERTSPSESSIONFORTHELIVESOURCE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				/*
				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
				*/
			}
		}
	}

	if (rsRTSPStatus == SS_RTSP_READY ||
		rsRTSPStatus == SS_RTSP_PLAYING)
	{
		// This case (SS_RTSP_READY || SS_RTSP_PLAYING)
		//	should be the case in which we don't have
		//	a normal finish of the connection but
		//	we have an error.

		for (ulTrackIdentifier = 0;
			ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			ulTrackIdentifier++)
		{
			if (((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_bTrackInitialized)
			{
				if (deleteTrackToStream (_bIsLive, ulTrackIdentifier) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_DELETETRACKTOSTREAM_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					/*
					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
					*/
				}
			}
		}

		if (finishRTPSession () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_FINISHRTPSESSION_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			/*
			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
			*/
		}
	}

	_ulCurrentPortForSetup			= _ulReservedStartingPort;

	_rsRTPSession. _ulCurrentMaxPayloadSizeInBytes		=
		_ulMaxPayloadSizeInBytes;

	{
		unsigned long		*pulRTSPIdentifier;

		if (_pspSocketsPool -> deleteSocket (
			&_csClientSocket, (void **) (&pulRTSPIdentifier)) !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			/*
			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
			*/
		}
	}

	if (_csClientSocket. finish () != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		/*
		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
		*/
	}

	if (_csClientSocket. init () != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		/*
		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
		*/
	}

	if (_rrtRTSP_RTCPTimes. stop () != errNoError)
	{
		/*
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_TIMES_STOP_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/
	}

	if (_srptSendRTCPPacketsTimes. stop () != errNoError)
	{
		/*
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_TIMES_STOP_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/
	}

	if (_crrptCheckReceivedRTCPPacketsTimes. stop () !=
		errNoError)
	{
		/*
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_TIMES_STOP_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/
	}

	if (_ptPauseTimes. stop () != errNoError)
	{
		/*
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_TIMES_STOP_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/
	}

	// HTTPGETTHREAD con detach

	{
		Buffer_t			bRequestLog;


		if (bRequestLog. init () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			/*
			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
			*/
		}

		if (getRequestLog (perrError,
			&bRequestLog) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_GETREQUESTLOG_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			/*
			if (bRequestLog. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
			*/
		}

		_ptSystemTracer -> trace (Tracer:: TRACER_LINFO,
			(const char *) bRequestLog, __FILE__, __LINE__);
		_ptSubscriberTracer -> trace (Tracer:: TRACER_LINFO,
			(const char *) bRequestLog, __FILE__, __LINE__);

		if (_bFlushTraceAfterEachRequest)
			_ptSubscriberTracer -> flushOfTraces ();

		if (bRequestLog. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			/*
			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
			*/
		}
	}

	if (_bEndingHookActivated)
	{
		HttpGetThread_p			phgHTTPEndingHook;
		Buffer_t				bLocalEndingHookServletPathName;
		Buffer_t				bLocalEndingHookServletParameters;


		if (bLocalEndingHookServletPathName. init (
			(const char *) _bEndingHookServletPathName) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bLocalEndingHookServletParameters. init (
			(const char *) _bEndingHookServletParameters) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bLocalEndingHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (strstr ((const char *) bLocalEndingHookServletParameters,
			"$PlayerURL$") != (char *) NULL)
		{
			if (strcmp ((const char *) _bURLParameters, ""))
			{
				Buffer_t				bPlayerURL;


				if (bPlayerURL. init (
					(const char *) _bRelativePathWithoutParameters) !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bLocalEndingHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				// use of '@' instead of '?'
				if (bPlayerURL. append ("@") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bPlayerURL. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				if (bPlayerURL. append (
					(const char *) _bURLParameters) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bPlayerURL. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				if (bPlayerURL. substitute ("=", "#") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bPlayerURL. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				if (bPlayerURL. substitute ("&", "~") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bPlayerURL. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				if (bLocalEndingHookServletParameters. substitute (
					"$PlayerURL$",
					(const char *) bPlayerURL) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bPlayerURL. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				if (bPlayerURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bLocalEndingHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}
			else
			{
				if (bLocalEndingHookServletParameters. substitute (
					"$PlayerURL$",
					(const char *) _bRelativePathWithoutParameters) !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bLocalEndingHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}
		}

		if (bLocalEndingHookServletParameters. substitute (
			"$PlayerIP$", _pClientIPAddress) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bLocalEndingHookServletParameters. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalEndingHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED)
		{
			if (bLocalEndingHookServletParameters. substitute (
				"$BeginningHookMessage$",
				(const char *) _bBeginningHookMessage) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bLocalEndingHookServletParameters. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalEndingHookServletPathName. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}
		else
		{
			if (bLocalEndingHookServletParameters. substitute (
				"$BeginningHookMessage$",
				"NA") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bLocalEndingHookServletParameters. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalEndingHookServletPathName. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED &&
			rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED &&
			rsRTSPStatus != RTSPSession:: SS_RTSP_INIT)
		{
			if (_bIsLive)
			{
				if (bLocalEndingHookServletParameters. substitute (
					"$LastRelativeTransmissionTimeInSecs$",
					"NA") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bLocalEndingHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}
			else
			{
				if (bLocalEndingHookServletParameters. substitute (
					"$LastRelativeTransmissionTimeInSecs$", (unsigned long)
		_rsRTPSession. _dRelativeTransmissionTimeInSecsOfLastSentRTPPacket) !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bLocalEndingHookServletParameters. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalEndingHookServletPathName. finish () !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}
		}
		else
		{
			if (bLocalEndingHookServletParameters. substitute (
				"$LastRelativeTransmissionTimeInSecs$",
				"NA") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bLocalEndingHookServletParameters. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalEndingHookServletPathName. finish () !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		// ask for the EndingHook
		if ((phgHTTPEndingHook = new HttpGetThread_t) ==
			(HttpGetThread_p) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_NEW_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bLocalEndingHookServletParameters. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalEndingHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		// The GET HTTP will be:
		//	_bEndingHookServletPathName + bEndingHookServletParameters
		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_HTTPGETREQUEST,
				5, _ulIdentifier, _pEndingHookWebServerIpAddress,
				_ulEndingHookWebServerPort,
				(const char *) bLocalEndingHookServletPathName,
				(const char *) bLocalEndingHookServletParameters);
			_ptSystemTracer -> trace (Tracer:: TRACER_LINFO,
				(const char *) msg, __FILE__, __LINE__);
		}

		if (phgHTTPEndingHook -> init (
			_pEndingHookWebServerIpAddress, _ulEndingHookWebServerPort,
			(const char *) bLocalEndingHookServletPathName,
			(const char *) bLocalEndingHookServletParameters,
			(const char *) NULL,
			(const char *) NULL,
			_ulEndingHookTimeoutInSecs, 0, _ulEndingHookTimeoutInSecs, 0,
			_pEndingHookLocalIPAddressForHTTP) != errNoError)
		{
			Error err = WebToolsErrors (__FILE__, __LINE__,
				WEBTOOLS_HTTPGETTHREAD_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete phgHTTPEndingHook;

			if (bLocalEndingHookServletParameters. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalEndingHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (bLocalEndingHookServletParameters. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (phgHTTPEndingHook -> finish () != errNoError)
			{
				Error err = WebToolsErrors (__FILE__, __LINE__,
					WEBTOOLS_HTTPGETTHREAD_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete phgHTTPEndingHook;

			if (bLocalEndingHookServletPathName. finish () !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (bLocalEndingHookServletPathName. finish () !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (phgHTTPEndingHook -> finish () != errNoError)
			{
				Error err = WebToolsErrors (__FILE__, __LINE__,
					WEBTOOLS_HTTPGETTHREAD_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete phgHTTPEndingHook;
		}

		if (phgHTTPEndingHook -> start () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_START_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (phgHTTPEndingHook -> finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_BEGINNINGHOOKHTTPGETTHREAD_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete phgHTTPEndingHook;
		}

		if (phgHTTPEndingHook -> detach () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_DETACH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (phgHTTPEndingHook -> cancel () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_CANCEL_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (phgHTTPEndingHook -> finish () != errNoError)
			{
				Error err = WebToolsErrors (__FILE__, __LINE__,
					WEBTOOLS_HTTPGETTHREAD_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete phgHTTPEndingHook;
		}
	}

	if (_bBeginningHookMessage. setBuffer ("NA") !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_pmfFile != (MP4File_p) NULL)
	{
		if (_pmp4fMP4FileFactory -> releaseMP4File (
			(const char *) _bAssetPath) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_RELEASEMP4FILE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			/*
			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
			*/
		}

		_pmfFile				= (MP4File_p) NULL;
	}

	_dMovieDuration					= -1;
	_ulVideoTrackIdentifier			= SS_RTSPSESSION_MAXTRACKSNUMBER;
	_ulAudioTrackIdentifier			= SS_RTSPSESSION_MAXTRACKSNUMBER;

	if (setRTSPStatus (RTSPSession:: SS_RTSP_NOTUSED) !=
		errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_RTSPSESSION_CONNECTIONCLOSED,
			3, _ulIdentifier, _pClientIPAddress, _lRTSPClientPort);
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}


	return errNoError;
}


Error RTSPSession:: getRequestLog (
	Error_p perrError,
	Buffer_p pbRequestLog)

{


	char				pBuffer [SS_RTSPSESSION_MAXLOGBUFFERLENGTH];
	RTSPStatus_t		rsRTSPStatus;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbRequestLog -> setBuffer (
		(const char *) _bRequestLogFormat) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);


		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// Start connection date time
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED)
	{
		sprintf (pBuffer, "%04d",
			_tmLocalDateTimeStartConnection. tm_year + 1900);

		if (pbRequestLog -> substitute ("%SYYYY%", pBuffer) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		sprintf (pBuffer, "%02d",
			_tmLocalDateTimeStartConnection. tm_mon + 1);

		if (pbRequestLog -> substitute ("%SMM%", pBuffer) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		sprintf (pBuffer, "%02d",
			_tmLocalDateTimeStartConnection. tm_mday);

		if (pbRequestLog -> substitute ("%SDD%", pBuffer) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		sprintf (pBuffer, "%02d",
			_tmLocalDateTimeStartConnection. tm_hour);

		if (pbRequestLog -> substitute ("%SHH%", pBuffer) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		sprintf (pBuffer, "%02d",
			_tmLocalDateTimeStartConnection. tm_min);

		if (pbRequestLog -> substitute ("%SMI%", pBuffer) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		sprintf (pBuffer, "%02d",
			_tmLocalDateTimeStartConnection. tm_sec);

		if (pbRequestLog -> substitute ("%SSS%", pBuffer) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		sprintf (pBuffer, "%04ld",
			_ulStartConnectionMilliSeconds);

		if (pbRequestLog -> substitute ("%SMILL%", pBuffer) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%SYYYY%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%SMM%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%SDD%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%SHH%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%SMI%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%SSS%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%SMILL%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// Client IP address
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED)
	{
		if (pbRequestLog -> substitute ("%CIP%", _pClientIPAddress) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%CIP%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// URL without parameters
	// URL parameters
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		if (pbRequestLog -> substitute ("%RPWP%",
			(const char *) _bRelativePathWithoutParameters) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%URLP%",
			(const char *) _bURLParameters) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%RPWP%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%URLP%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// User agent
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		if (pbRequestLog -> substitute ("%USERAGENT%",
			(const char *) _bUserAgent) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%USERAGENT%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// Status
	if (*perrError != errNoError)
	{
		if (pbRequestLog -> substitute ("%STATUS%",
			(const char *) (*perrError)) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// Replace the '\n' included into the Error
		//	with ' '
		if (pbRequestLog -> substitute ("\n",
			" ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%STATUS%",
			"Success") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// Standard used
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		if (_bIsLive)
		{
			if (pbRequestLog -> substitute ("%STD%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (_sStandard == MP4Atom:: MP4F_3GPP)
			{
				if (pbRequestLog -> substitute ("%STD%", "3GPP") !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				if (pbRequestLog -> substitute ("%STD%", "ISMA") !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%STD%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}


	// Connection live?
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		if (_bIsLive)
		{
			if (pbRequestLog -> substitute ("%CL%", "true") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (pbRequestLog -> substitute ("%CL%", "false") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%CL%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// Duration
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		if (_bIsLive)
		{
			if (pbRequestLog -> substitute ("%DUR%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			sprintf (pBuffer, "%.3lf", _dMovieDuration);

			if (pbRequestLog -> substitute ("%DUR%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%DUR%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// Video/audio codec
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		if (_bVideoTrackFoundInSDP)
		{
			if (MP4Utility:: getCodecName (_cVideoCodecUsed,
				pBuffer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETCODECNAME_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbRequestLog -> substitute ("%VCOD%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (pbRequestLog -> substitute ("%VCOD%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (_bAudioTrackFoundInSDP)
		{
			if (MP4Utility:: getCodecName (_cAudioCodecUsed,
				pBuffer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETCODECNAME_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbRequestLog -> substitute ("%ACOD%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (pbRequestLog -> substitute ("%ACOD%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%VCOD%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%ACOD%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// bit rate
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		if (_bVideoTrackFoundInSDP)
		{
			sprintf (pBuffer, "%lu", _ulVideoAvgBitRate);
			if (pbRequestLog -> substitute ("%VBR%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (pbRequestLog -> substitute ("%VBR%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (_bAudioTrackFoundInSDP)
		{
			sprintf (pBuffer, "%lu", _ulAudioAvgBitRate);
			if (pbRequestLog -> substitute ("%ABR%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (pbRequestLog -> substitute ("%ABR%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%VBR%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%ABR%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// sent RTP packets number
	// Sent RTP bytes
	// lost RTP packets number
	// received RTCP packets number
	// received RTCP bytes
	// jitter
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		if (_bVideoTrackFoundInSDP && _bVideoTrackInitializedFromSetup)
		{
			sprintf (pBuffer, "%lu",
				((_rsRTPSession. _ptiTracks) [
				_ulVideoTrackIdentifier]). _ulSentRTPPacketsNumber);
			if (pbRequestLog -> substitute ("%VPS%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sprintf (pBuffer, "%llu",
				((_rsRTPSession. _ptiTracks) [
				_ulVideoTrackIdentifier]). _ullRTPPacketBytesSent);
			if (pbRequestLog -> substitute ("%VBS%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sprintf (pBuffer, "%lu",
				((_rsRTPSession. _ptiTracks) [
				_ulVideoTrackIdentifier]). _ulLostRTPPacketsNumber);
			if (pbRequestLog -> substitute ("%VPL%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sprintf (pBuffer, "%lu",
				((_rsRTPSession. _ptiTracks) [
				_ulVideoTrackIdentifier]). _ulRTCPPacketsNumberReceived);
			if (pbRequestLog -> substitute ("%VRTCPP%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sprintf (pBuffer, "%lu",
				((_rsRTPSession. _ptiTracks) [
				_ulVideoTrackIdentifier]). _ullRTCPBytesReceived);
			if (pbRequestLog -> substitute ("%VRTCPB%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sprintf (pBuffer, "%lu",
				((_rsRTPSession. _ptiTracks) [
				_ulVideoTrackIdentifier]). _ulJitter);
			if (pbRequestLog -> substitute ("%VJIT%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (pbRequestLog -> substitute ("%VPS%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbRequestLog -> substitute ("%VBS%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbRequestLog -> substitute ("%VPL%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbRequestLog -> substitute ("%VRTCPP%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbRequestLog -> substitute ("%VRTCPB%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbRequestLog -> substitute ("%VJIT%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (_bAudioTrackFoundInSDP && _bAudioTrackInitializedFromSetup)
		{
			sprintf (pBuffer, "%lu",
				((_rsRTPSession. _ptiTracks) [
				_ulAudioTrackIdentifier]). _ulSentRTPPacketsNumber);
			if (pbRequestLog -> substitute ("%APS%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sprintf (pBuffer, "%llu",
				((_rsRTPSession. _ptiTracks) [
				_ulAudioTrackIdentifier]). _ullRTPPacketBytesSent);
			if (pbRequestLog -> substitute ("%ABS%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sprintf (pBuffer, "%lu",
				((_rsRTPSession. _ptiTracks) [
				_ulAudioTrackIdentifier]). _ulLostRTPPacketsNumber);
			if (pbRequestLog -> substitute ("%APL%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sprintf (pBuffer, "%lu",
				((_rsRTPSession. _ptiTracks) [
				_ulAudioTrackIdentifier]). _ulRTCPPacketsNumberReceived);
			if (pbRequestLog -> substitute ("%ARTCPP%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sprintf (pBuffer, "%lu",
				((_rsRTPSession. _ptiTracks) [
				_ulAudioTrackIdentifier]). _ullRTCPBytesReceived);
			if (pbRequestLog -> substitute ("%ARTCPB%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sprintf (pBuffer, "%lu",
				((_rsRTPSession. _ptiTracks) [
				_ulAudioTrackIdentifier]). _ulJitter);
			if (pbRequestLog -> substitute ("%AJIT%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (pbRequestLog -> substitute ("%APS%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbRequestLog -> substitute ("%ABS%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbRequestLog -> substitute ("%APL%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbRequestLog -> substitute ("%ARTCPP%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbRequestLog -> substitute ("%ARTCPB%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbRequestLog -> substitute ("%AJIT%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%VPS%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%VBS%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%VPL%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%VRTCPP%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%VRTCPB%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%VJIT%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%APS%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%ABS%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%APL%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%ARTCPP%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%ARTCPB%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbRequestLog -> substitute ("%AJIT%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// Connection time
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED)
	{
		#ifdef WIN32
			sprintf (pBuffer, "%llu",
				(__int64) (_tUTCEndConnectionTime - _tUTCStartConnectionTime));
		#else
			sprintf (pBuffer, "%llu",
			(unsigned long long) (_tUTCEndConnectionTime - _tUTCStartConnectionTime));
		#endif
		if (pbRequestLog -> substitute ("%CT%", pBuffer) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%CT%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// Last relative transmission time
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_INIT)
	{
		if (_bIsLive)
		{
			if (pbRequestLog -> substitute ("%LRT%", "NA") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			sprintf (pBuffer, "%.3lf",
				_rsRTPSession. _dRelativeTransmissionTimeInSecsOfLastSentRTPPacket);
			if (pbRequestLog -> substitute ("%LRT%", pBuffer) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%LRT%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// Streaming time
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_CONNECTIONACCEPTED &&
		rsRTSPStatus != RTSPSession:: SS_RTSP_INIT)
	{
		sprintf (pBuffer, "%lu", _ulStreamingTimeInSecs);
		if (pbRequestLog -> substitute ("%STREAMTIME%", pBuffer) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%STREAMTIME%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// BeginningHook time message
	if (rsRTSPStatus != RTSPSession:: SS_RTSP_NOTUSED)
	{
		if (pbRequestLog -> substitute ("%AUTHORIZATIONMESSAGE%",
			(const char *) _bBeginningHookMessage) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		if (pbRequestLog -> substitute ("%AUTHORIZATIONMESSAGE%", "NA") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: sendRTPPackets (
	unsigned long *pulVideoRTPPacketsNumberSent,
	unsigned long *pulAudioRTPPacketsNumberSent)

{

	Boolean_t			bIsNecessaryToWait;
	Error_t				errGetNextRTPPacket;
	Error_t				errSendCurrentPacket;
	double				dRelativeRTPPacketTransmissionTimeInMilliSecs;
	double				dAbsoluteRTPPacketTransmissionTimeInMilliSecs;
	double				dOffsetFromStartTimeInSecs;
	unsigned long		ulSelectedTrackIdentifier;
	RTPPacket_p			prpRTPPacket;
	#ifdef WIN32
		__int64					ullSuggestedWakeupTimeInMilliSecs;
	#else
		unsigned long long		ullSuggestedWakeupTimeInMilliSecs;
	#endif
	RTSPStatus_t		rsRTSPStatus;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus != SS_RTSP_PLAYING)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "sendRTPPackets", (long) rsRTSPStatus);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		//	(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	*pulVideoRTPPacketsNumberSent			= 0;
	*pulAudioRTPPacketsNumberSent			= 0;

	bIsNecessaryToWait			= false;

	while (!bIsNecessaryToWait)
	{
		if ((errGetNextRTPPacket = getAndRemoveNextRTPPacketToSend (
			&dRelativeRTPPacketTransmissionTimeInMilliSecs,
			&ulSelectedTrackIdentifier, &prpRTPPacket)) !=
			errNoError)
		{
			if ((long) errGetNextRTPPacket != SS_RTSPSESSION_NOMORERTPPACKETS)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errGetNextRTPPacket, __FILE__, __LINE__);

				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_GETANDREMOVENEXTRTPPACKETTOSEND_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			else
			{
				unsigned long				ulLocalTrackIdentifier;

				for (ulLocalTrackIdentifier = 0;
					ulLocalTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
					ulLocalTrackIdentifier++)
				{
					if (!(((_rsRTPSession. _ptiTracks) [
						ulLocalTrackIdentifier]). _bActivated))
						continue;

					if (deactiveTrack (&((_rsRTPSession. _ptiTracks) [
						ulLocalTrackIdentifier])) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_DEACTIVETRACK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (_mtRTSPSession. unLock () != errNoError)
						{
							Error err = PThreadErrors (__FILE__, __LINE__,
								THREADLIB_PMUTEX_UNLOCK_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}

				if (setRTSPStatus (RTSPSession:: SS_RTSP_READY) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				// update of the streaming time
				{
					#ifdef WIN32
						__int64					ullEndClipTimeInMilliSeconds;
					#else
						unsigned long long		ullEndClipTimeInMilliSeconds;
					#endif

					if (DateTime:: nowLocalInMilliSecs (
						&ullEndClipTimeInMilliSeconds) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_DATETIME_NOWLOCALINMILLISECS_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (_mtRTSPSession. unLock () != errNoError)
						{
							Error err = PThreadErrors (__FILE__, __LINE__,
								THREADLIB_PMUTEX_UNLOCK_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					_ulStreamingTimeInSecs			+= ((unsigned long)
						(ullEndClipTimeInMilliSeconds -
						_rsRTPSession. _ullPlayTimeInMilliSeconds) / 1000);

					// we simulate the PAUSE COMMAND to avoid
					//	that in handlTEARDOWN the
					//	streaming time is again updated
					_rcPreviousRTSPCommand			= _rcLastRTSPCommand;
					_rcLastRTSPCommand				= SS_RTSP_PAUSE;

					if (_ptPauseTimes. start () != errNoError)
					{
						Error err = SchedulerErrors (__FILE__, __LINE__,
							SCH_TIMES_START_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (_mtRTSPSession. unLock () != errNoError)
						{
							Error err = PThreadErrors (__FILE__, __LINE__,
								THREADLIB_PMUTEX_UNLOCK_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}

 				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return errGetNextRTPPacket;
			}
		}

		if (_rsRTPSession. _dRelativeRequestedStopTimeInSecs != -1.0 &&
			dRelativeRTPPacketTransmissionTimeInMilliSecs / 1000 >=
			_rsRTPSession. _dRelativeRequestedStopTimeInSecs)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_REQUESTEDSTOPTIMEREACHED,
				1, _rsRTPSession. _dRelativeRequestedStopTimeInSecs);
			_ptSystemTracer -> trace (Tracer:: TRACER_LWRNG,
				(const char *) err, __FILE__, __LINE__);

			(((_rsRTPSession. _ptiTracks) [ulSelectedTrackIdentifier]).
				_vBuiltRTPPackets). insert (
				(((_rsRTPSession. _ptiTracks) [ulSelectedTrackIdentifier]).
				_vBuiltRTPPackets). begin (),
				prpRTPPacket);

			(((_rsRTPSession. _ptiTracks) [ulSelectedTrackIdentifier]).
				_vTransmissionTimeBuiltRTPPackets). insert (
				(((_rsRTPSession. _ptiTracks) [ulSelectedTrackIdentifier]).
				_vTransmissionTimeBuiltRTPPackets). begin (),
				dRelativeRTPPacketTransmissionTimeInMilliSecs);

			{
				unsigned long				ulLocalTrackIdentifier;

				for (ulLocalTrackIdentifier = 0;
					ulLocalTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
					ulLocalTrackIdentifier++)
				{
					if (!(((_rsRTPSession. _ptiTracks) [
						ulLocalTrackIdentifier]). _bActivated))
						continue;

					if (deactiveTrack (&((_rsRTPSession. _ptiTracks) [
						ulLocalTrackIdentifier])) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_DEACTIVETRACK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (_mtRTSPSession. unLock () != errNoError)
						{
							Error err = PThreadErrors (__FILE__, __LINE__,
								THREADLIB_PMUTEX_UNLOCK_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
			}

			if (setRTSPStatus (RTSPSession:: SS_RTSP_READY) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// update of the streaming time
			{
				#ifdef WIN32
					__int64					ullEndClipTimeInMilliSeconds;
				#else
					unsigned long long		ullEndClipTimeInMilliSeconds;
				#endif

				if (DateTime:: nowLocalInMilliSecs (
					&ullEndClipTimeInMilliSeconds) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_DATETIME_NOWLOCALINMILLISECS_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				_ulStreamingTimeInSecs			+= ((unsigned long)
					(ullEndClipTimeInMilliSeconds -
					_rsRTPSession. _ullPlayTimeInMilliSeconds) / 1000);
			}

			// we simulate the PAUSE COMMAND to avoid
			//	that a next TEARDOWN COMMAND
			//	(in handlTEARDOWNMethod) update again the streaming time
			{
				_rcPreviousRTSPCommand			= _rcLastRTSPCommand;
				_rcLastRTSPCommand				= SS_RTSP_PAUSE;

				if (_ptPauseTimes. start () != errNoError)
				{
					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_TIMES_START_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (_rrtRTSP_RTCPTimes. stop () != errNoError)
				{
					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_TIMES_STOP_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (_rrtRTSP_RTCPTimes. start () != errNoError)
				{
					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_TIMES_START_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		{
			dOffsetFromStartTimeInSecs									=
				(dRelativeRTPPacketTransmissionTimeInMilliSecs / 1000) -
				(_rsRTPSession. _dRelativeRequestedStartTimeInSecs);

			// now in secs yet
			dAbsoluteRTPPacketTransmissionTimeInMilliSecs	=
				_rsRTPSession. _dRelativeRequestedStartTimeInSecs +
				(dOffsetFromStartTimeInSecs / _rsRTPSession. _dRequestedSpeed);

			dAbsoluteRTPPacketTransmissionTimeInMilliSecs	=
				dAbsoluteRTPPacketTransmissionTimeInMilliSecs * 1000;

			dAbsoluteRTPPacketTransmissionTimeInMilliSecs	+=
				_rsRTPSession. _dAdjustedPlayTimeInMilliSecs;

			if (dAbsoluteRTPPacketTransmissionTimeInMilliSecs < 0.0)
				dAbsoluteRTPPacketTransmissionTimeInMilliSecs		= 0.0;
		}

		if ((errSendCurrentPacket =
			sendRTPPacket (
			dAbsoluteRTPPacketTransmissionTimeInMilliSecs,
			ulSelectedTrackIdentifier, prpRTPPacket,
			&ullSuggestedWakeupTimeInMilliSecs)) !=
			errNoError)
		{
			if ((long) errSendCurrentPacket ==
				SS_RTSPSESSION_TOOEARLYTOSENDTHEPACKET)
			{
				(((_rsRTPSession. _ptiTracks) [ulSelectedTrackIdentifier]).
					_vBuiltRTPPackets). insert (
					(((_rsRTPSession. _ptiTracks) [
					ulSelectedTrackIdentifier]). _vBuiltRTPPackets). begin (),
					prpRTPPacket);

				(((_rsRTPSession. _ptiTracks) [ulSelectedTrackIdentifier]).
					_vTransmissionTimeBuiltRTPPackets). insert (
					(((_rsRTPSession. _ptiTracks) [ulSelectedTrackIdentifier]).
					_vTransmissionTimeBuiltRTPPackets). begin (),
					dRelativeRTPPacketTransmissionTimeInMilliSecs);

				{
					Event_p						pevEvent;
					RTSPConnectionEvent_p			pevRTSPConnection;

					if (_pesEventsSet -> getFreeEvent (
			StreamingServerEventsSet:: SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER,
						&pevEvent) != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENTSSET_GETFREEEVENT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (_mtRTSPSession. unLock () != errNoError)
						{
							Error err = PThreadErrors (__FILE__, __LINE__,
								THREADLIB_PMUTEX_UNLOCK_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					pevRTSPConnection		= (RTSPConnectionEvent_p) pevEvent;

					if (pevRTSPConnection -> init (
						SS_RTSPSESSION_SOURCE,
						SS_EVENT_SENDFILERTPPACKETS, "SS_EVENT_SENDFILERTPPACKETS",
						_ulIdentifier, _ptSystemTracer,
						false, ullSuggestedWakeupTimeInMilliSecs,
						(Buffer_p) NULL) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPCONNECTIONEVENT_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (_pesEventsSet -> releaseEvent (
			StreamingServerEventsSet:: SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER,
							pevRTSPConnection) != errNoError)
						{
							Error err = EventsSetErrors (__FILE__, __LINE__,
								EVSET_EVENTSSET_RELEASEEVENT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_mtRTSPSession. unLock () != errNoError)
						{
							Error err = PThreadErrors (__FILE__, __LINE__,
								THREADLIB_PMUTEX_UNLOCK_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (_pesEventsSet -> addEvent (
						// SS_STREAMINGSERVERPROCESSOR_DESTINATION,
						pevRTSPConnection) != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENTSSET_ADDEVENT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pevRTSPConnection -> finish () != errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPCONNECTIONEVENT_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_pesEventsSet -> releaseEvent (
			StreamingServerEventsSet:: SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER,
							pevRTSPConnection) != errNoError)
						{
							Error err = EventsSetErrors (__FILE__, __LINE__,
								EVSET_EVENTSSET_RELEASEEVENT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (_mtRTSPSession. unLock () != errNoError)
						{
							Error err = PThreadErrors (__FILE__, __LINE__,
								THREADLIB_PMUTEX_UNLOCK_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}

				bIsNecessaryToWait		= true;

				continue;
			}
			else
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errSendCurrentPacket, __FILE__, __LINE__);

				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTPPACKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				// the RTP packet is removed from vector so
				//	it should be added to _vFreeRTPPackets to avoid
				//	to lose the packet
				if (prpRTPPacket -> reset () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_RESET_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				(((_rsRTPSession. _ptiTracks) [ulSelectedTrackIdentifier]).
					_vFreeRTPPackets). insert ((((_rsRTPSession. _ptiTracks) [
					ulSelectedTrackIdentifier]). _vFreeRTPPackets). end (),
					prpRTPPacket);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		_rsRTPSession. _dRelativeTransmissionTimeInSecsOfLastSentRTPPacket	=
			dRelativeRTPPacketTransmissionTimeInMilliSecs / 1000;

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			// the RTP packet is removed from vector so
			//	it should be added to _vFreeRTPPackets to avoid
			//	to lose the packet
			(((_rsRTPSession. _ptiTracks) [ulSelectedTrackIdentifier]).
				_vFreeRTPPackets). insert ((((_rsRTPSession. _ptiTracks) [
				ulSelectedTrackIdentifier]). _vFreeRTPPackets). end (),
				prpRTPPacket);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		(((_rsRTPSession. _ptiTracks) [ulSelectedTrackIdentifier]).
			_vFreeRTPPackets). insert ((((_rsRTPSession. _ptiTracks) [
			ulSelectedTrackIdentifier]). _vFreeRTPPackets). end (),
			prpRTPPacket);

		if (ulSelectedTrackIdentifier == _ulVideoTrackIdentifier)
			(*pulVideoRTPPacketsNumberSent)++;
		else if (ulSelectedTrackIdentifier == _ulAudioTrackIdentifier)
			(*pulAudioRTPPacketsNumberSent)++;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: getAndRemoveNextRTPPacketToSend (
	double *pdRelativeRTPPacketTransmissionTimeInMilliSecs,
	unsigned long *pulTrackIdentifier,
	RTPPacket_p *prpRTPPacket)

{

	Boolean_t			bFoundRTPPacket;
	double				dLocalRelativeRTPPacketTransmissionTimeInMilliSecs;
	RTPPacket_p			prpLocalRTPPacket;
	unsigned long		ulLocalTrackIdentifier;
	Boolean_t			bIsAPacketPrefetched;
	RTSPStatus_t		rsRTSPStatus;
	char				pHandlerType [MP4F_MAXHANDLERTYPELENGTH];


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// the RTSP state must be PLAYING. I added also the READY state
	//	because this method is called also by the handlePLAYmethod to
	//	retrieve the seq. nu. and timestamp of the first RTP packet.
	if (rsRTSPStatus != SS_RTSP_PLAYING)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "getAndRemoveNextRTPPacketToSend",
			(long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	bFoundRTPPacket				= false;

	// loop to determine which RTP packet will be sent before (audio or video)
	for (ulLocalTrackIdentifier = 0;
		ulLocalTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
		ulLocalTrackIdentifier++)
	{
		if (!(((_rsRTPSession. _ptiTracks) [ulLocalTrackIdentifier]).
			_bTrackInitialized))
			continue;

		if ((((_rsRTPSession. _ptiTracks) [
			ulLocalTrackIdentifier]). _vBuiltRTPPackets). begin () ==
			(((_rsRTPSession. _ptiTracks) [
			ulLocalTrackIdentifier]). _vBuiltRTPPackets). end ())
		{
			if ((((_rsRTPSession. _ptiTracks) [ulLocalTrackIdentifier]).
				_pmtiTrackInfo) -> getHandlerType (pHandlerType) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETHANDLERTYPE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (!strcmp (pHandlerType, MP4_HINT_TRACK_TYPE))
			{
				if (prefetchNextPacketsUsingHintTrack (
					ulLocalTrackIdentifier, false,
					&bIsAPacketPrefetched) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_PREFETCHNEXTPACKETSUSINGHINTTRACK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				if (prefetchNextPacketsUsingMediaTrack (
					ulLocalTrackIdentifier, false,
					&bIsAPacketPrefetched) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_PREFETCHNEXTPACKETSUSINGMEDIATRACK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			if ((((_rsRTPSession. _ptiTracks) [
				ulLocalTrackIdentifier]). _vBuiltRTPPackets). begin () ==
				(((_rsRTPSession. _ptiTracks) [
				ulLocalTrackIdentifier]). _vBuiltRTPPackets). end ())
				continue;
		}

		prpLocalRTPPacket			= *((((_rsRTPSession. _ptiTracks) [
			ulLocalTrackIdentifier]). _vBuiltRTPPackets). begin ());

		dLocalRelativeRTPPacketTransmissionTimeInMilliSecs	=
			*((((_rsRTPSession. _ptiTracks) [
			ulLocalTrackIdentifier]). _vTransmissionTimeBuiltRTPPackets).
			begin ());

		if (!bFoundRTPPacket || 
			dLocalRelativeRTPPacketTransmissionTimeInMilliSecs <
			*pdRelativeRTPPacketTransmissionTimeInMilliSecs) 
        {
            bFoundRTPPacket			= true;
			*pdRelativeRTPPacketTransmissionTimeInMilliSecs	=
				dLocalRelativeRTPPacketTransmissionTimeInMilliSecs; 
			*pulTrackIdentifier		= ulLocalTrackIdentifier;
			*prpRTPPacket			= prpLocalRTPPacket;
		}
	}

	if (!bFoundRTPPacket)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_NOMORERTPPACKETS);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		//	(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	(((_rsRTPSession. _ptiTracks) [
		*pulTrackIdentifier]). _vBuiltRTPPackets). erase (
		(((_rsRTPSession. _ptiTracks) [
		*pulTrackIdentifier]). _vBuiltRTPPackets). begin ());

	(((_rsRTPSession. _ptiTracks) [
		*pulTrackIdentifier]). _vTransmissionTimeBuiltRTPPackets). erase (
		(((_rsRTPSession. _ptiTracks) [
		*pulTrackIdentifier]). _vTransmissionTimeBuiltRTPPackets). begin ());

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}



	return errNoError;
}


#ifdef WIN32
	Error RTSPSession:: sendRTPPacket (
		double dAbsoluteRTPPacketTransmissionTimeInMilliSecs,
		unsigned long ulTrackIdentifier,
		RTPPacket_p prpRTPPacket,
		__int64 *pullSuggestedWakeupTimeInMilliSecs)
#else
	Error RTSPSession:: sendRTPPacket (
		double dAbsoluteRTPPacketTransmissionTimeInMilliSecs,
		unsigned long ulTrackIdentifier,
		RTPPacket_p prpRTPPacket,
		unsigned long long *pullSuggestedWakeupTimeInMilliSecs)
#endif


{

	SocketImpl_p							pClientSocketImpl;
	Error_t									errWrite;
	// unsigned short						usSequenceNumber;
	unsigned long			ulRelativeRTPPacketTransmissionTimeWithTimeScale;
	// unsigned char						ucPayload;
	// unsigned long						ulSequenceNumberRandomOffsetUsed;
	unsigned long							ulTimestampRandomOffsetUsed;
	// Boolean_t							bMarkerBit;
	#ifdef WIN32
		__int64							ullPacketLength;
	#else
		unsigned long long				ullPacketLength;
	#endif
	RTSPStatus_t		rsRTSPStatus;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus != SS_RTSP_PLAYING)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "sendRTPPacket", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulTrackIdentifier >= SS_RTSPSESSION_MAXTRACKSNUMBER)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_bTrackInitialized))
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_TRACKNOTINITIALIZED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	/*
		We must not consider if the track is deactived and we
		must send what we prefetched. This because inn case the player sends
		PAUSE or TEARDOWN, the procedure cuts all
		the RTP packets from the prefetched structure and we don't have
		problems.
		When the stream will finish the prefetch procedure put as deactived the
		track but in any case we should continue to stream until the packets
		will be finished in the prefetched structure

	if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _bActivated))
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_TRACKNOTACTIVED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	*/

	#ifdef WIN32
		ullPacketLength				= (__int64) (*prpRTPPacket);
	#else
		ullPacketLength				= (unsigned long long) (*prpRTPPacket);
	#endif

	if (!_bIsLive)
	{
		#ifdef WIN32
			// __int64							llCurrentPacketDelay;
			__int64							ullNowLocalTimeInMilliSecs;
			__int64							llSuggestedWakeupTime;
		#else
			// long long						llCurrentPacketDelay;
			unsigned long long				ullNowLocalTimeInMilliSecs;
			long long						llSuggestedWakeupTime;
		#endif

		// to handle the player buffer simulation and
		// to decide when to send the RTP packet

		if (DateTime:: nowLocalInMilliSecs (
			&ullNowLocalTimeInMilliSecs) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_DATETIME_NOWLOCALINMILLISECS_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		/*
		#ifdef WIN32
			llCurrentPacketDelay			= (__int64) (
				(double) llNowTimeInMilliSecs -
				dAbsoluteRTPPacketTransmissionTimeInMilliSecs);
		#else
			llCurrentPacketDelay			= (long long) (
				(double) llNowTimeInMilliSecs -
				dAbsoluteRTPPacketTransmissionTimeInMilliSecs);
		#endif
		*/

		#ifdef RTPOVERBUFFERWINDOW
			if ((_rsRTPSession. _robwRTPOverBufferWindow). emptyOutWindow (
				ullNowLocalTimeInMilliSecs) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTPOVERBUFFERWINDOW_EMPTYOUTWINDOW_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		#elif RTPOVERBUFFERWINDOW2
			(_rsRTPSession. _robwRTPOverBufferWindow2). emptyOutWindow (
				ullNowLocalTimeInMilliSecs);
		#else
		#endif

		if (_rsRTPSession. _bIsSessionFirstWrite)
		{
			_rsRTPSession. _bIsSessionFirstWrite	= false;

			#ifdef RTPOVERBUFFERWINDOW
				if ((_rsRTPSession. _robwRTPOverBufferWindow).
					markBeginningOfWriteBurst () != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTPOVERBUFFERWINDOW_MARKBEGINNINGOFWRITEBURST_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			#elif RTPOVERBUFFERWINDOW2
				(_rsRTPSession. _robwRTPOverBufferWindow2).
					markBeginningOfWriteBurst ();
			#else
			#endif
		}

		#ifdef RTPOVERBUFFERWINDOW
			{
				Error_t							errAddPacketToWindow;

				if ((errAddPacketToWindow =
					(_rsRTPSession. _robwRTPOverBufferWindow).
					addPacketToWindow (
					dAbsoluteRTPPacketTransmissionTimeInMilliSecs,
					ullNowLocalTimeInMilliSecs, (unsigned long) ullPacketLength,
					&llSuggestedWakeupTime)) != errNoError)
				{
					if ((long) errAddPacketToWindow ==
						SS_RTPOVERBUFFERWINDOW_PACKETBEFOREWINDOW)
					{
						llSuggestedWakeupTime				= 0;
					}
					else
					{
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) errAddPacketToWindow,
							__FILE__, __LINE__);

						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTPOVERBUFFERWINDOW_ADDPACKETTOWINDOW_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (_mtRTSPSession. unLock () != errNoError)
						{
							Error err = PThreadErrors (__FILE__, __LINE__,
								THREADLIB_PMUTEX_UNLOCK_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
			}
		#elif RTPOVERBUFFERWINDOW2
			llSuggestedWakeupTime			=
				(_rsRTPSession. _robwRTPOverBufferWindow2). checkTransmitTime (
				dAbsoluteRTPPacketTransmissionTimeInMilliSecs,
				ullNowLocalTimeInMilliSecs, (unsigned long) ullPacketLength);
		#else
		#endif


//		if (bIsAPacketOfASyncSample && llSuggestedWakeupTime >
//			llNowTimeInMilliSecs)
		#ifdef WIN32
			if (llSuggestedWakeupTime > (__int64) ullNowLocalTimeInMilliSecs)
		#else
			if (llSuggestedWakeupTime > (long long) ullNowLocalTimeInMilliSecs)
		#endif
		{
			if (llSuggestedWakeupTime - ullNowLocalTimeInMilliSecs >
				_rsRTPSession. _ulSendRTPMaxSleepTimeInMilliSecs)
			{
				*pullSuggestedWakeupTimeInMilliSecs		=
					ullNowLocalTimeInMilliSecs +
					_rsRTPSession. _ulSendRTPMaxSleepTimeInMilliSecs;
			}
			else
			{
				*pullSuggestedWakeupTimeInMilliSecs		=
					llSuggestedWakeupTime;
			}

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_TOOEARLYTOSENDTHEPACKET,
				1, (unsigned long)
				(*pullSuggestedWakeupTimeInMilliSecs -
				ullNowLocalTimeInMilliSecs));
			// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			//	(const char *) err, __FILE__, __LINE__);
/*
char aaa [256];
sprintf (aaa, "TransmitTime: \n%lf, Now: \n%llu, Returned suggested wake up: \n%lld, Real suggested wake up: \n%llu",
	dAbsoluteRTPPacketTransmissionTimeInMilliSecs,
	ullNowLocalTimeInMilliSecs,
	llSuggestedWakeupTime, *pullSuggestedWakeupTimeInMilliSecs);
_ptSystemTracer -> trace (Tracer:: TRACER_LERRR, aaa,
	__FILE__, __LINE__);
*/

			return err;
		}
	}

	if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _csRTPClientSocket).
		getSocketImpl (&pClientSocketImpl) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKET_GETSOCKETIMPL_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (prpRTPPacket -> getTimeStamp (
		&ulRelativeRTPPacketTransmissionTimeWithTimeScale,
		&ulTimestampRandomOffsetUsed) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_GETTIMESTAMP_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	/*
	unsigned short	usSequenceNumber;
	unsigned long	ulSequenceNumberRandomOffsetUsed;
	unsigned char	ucPayload;
	Boolean_t		bMarkerBit;
	if (prpRTPPacket -> getSequenceNumber (&usSequenceNumber,
		&ulSequenceNumberRandomOffsetUsed) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_GETSEQUENCENUMBER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	if (prpRTPPacket -> getPayloadType (&ucPayload) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_GETPAYLOADTYPE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (prpRTPPacket -> getMarkerBit (&bMarkerBit) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_GETMARKERBIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_RTSPSESSION_SENDRTPPACKET,
			13,
			_pClientIPAddress,
			((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _ulRTPClientPort,
			((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _pTrackURI,
			(unsigned long) usSequenceNumber,
			ulSequenceNumberRandomOffsetUsed,
			(unsigned long) (usSequenceNumber + ulSequenceNumberRandomOffsetUsed),
			bMarkerBit ? "true" : "false",
			(double) ((double)
			ulRelativeRTPPacketTransmissionTimeWithTimeScale /
			(double) (((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_ulRTPTimeScale)),
			ulTimestampRandomOffsetUsed,
			ulRelativeRTPPacketTransmissionTimeWithTimeScale +
			ulTimestampRandomOffsetUsed,
			dAbsoluteRTPPacketTransmissionTimeInMilliSecs,
			(long) ucPayload, ullPacketLength);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) msg,
			__FILE__, __LINE__);
	}
	*/

	if (!_bIsLive)
	{
		#ifdef RTPOVERBUFFERWINDOW
		#elif RTPOVERBUFFERWINDOW2
			(_rsRTPSession. _robwRTPOverBufferWindow2). addPacketToWindow (
				(unsigned long) ullPacketLength);
		#else
		#endif
	}

	if ((errWrite = pClientSocketImpl -> write (
		(void *) ((unsigned char *) (*prpRTPPacket)),
		(long) ullPacketLength, true, 1, 0, _pClientIPAddress,
		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _ulRTPClientPort)) !=
		errNoError)
	{
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errWrite, __FILE__, __LINE__);

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_ulLastRelativeRTPPacketTransmissionTimeWithTimeScaleAndOffset	=
		ulRelativeRTPPacketTransmissionTimeWithTimeScale +
		ulTimestampRandomOffsetUsed;

	((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_ulSentRTPPacketsNumber			+= 1;
	((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_ullRTPPacketBytesSent			+= ullPacketLength;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: handleReceiverRTCPPackets (
	unsigned long *pulReceivedRTCPPacketsNumber)

{

	unsigned long					ulTrackIdentifier;
	#ifdef WIN32
		__int64						ullNowUTCTimeInMilliSecs;
	#else
		unsigned long long			ullNowUTCTimeInMilliSecs;
	#endif
	RTSPStatus_t		rsRTSPStatus;


	*pulReceivedRTCPPacketsNumber		= 0;

	if (DateTime:: nowUTCInMilliSecs (
		&ullNowUTCTimeInMilliSecs,
		(long *) NULL) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_DATETIME_NOWUTCINMILLISECS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	/*
	if (ullNowUTCTimeInMilliSecs -
		_rsRTPSession. _ullLastReceiveRTCPPacketsInMilliSecs <
		_ulReceiveRTCPPacketsPeriodInMilliSecs)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_TOOEARLYTORECEIVERTCPPACKETS);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		//	(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	*/

	if (rsRTSPStatus != SS_RTSP_PLAYING &&
		rsRTSPStatus != SS_RTSP_READY)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "handleReceiverRTCPPackets", (long) rsRTSPStatus);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		// 	(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	for (ulTrackIdentifier = 0;
		ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
		ulTrackIdentifier++)
	{
		if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_bTrackInitialized))
			continue;

		if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _bActivated))
			continue;

		//	RTCP server
		{
			unsigned long							ulRTCPPacketsNumber;

			if (tryToReadAndProcessRTCPPackets (ulTrackIdentifier,
				&ulRTCPPacketsNumber) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_TRYTOREADANDPROCESSRTCPPACKETS_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			(*pulReceivedRTCPPacketsNumber)			+=
				ulRTCPPacketsNumber;
		}
	}

	_rsRTPSession. _ullLastReceiveRTCPPacketsInMilliSecs	=
		ullNowUTCTimeInMilliSecs;

	if (*pulReceivedRTCPPacketsNumber > 0)
	{
		if (_rrtRTSP_RTCPTimes. stop () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_TIMES_STOP_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_rrtRTSP_RTCPTimes. start () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_TIMES_START_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: handleSenderRTCPPackets (void)

{

	unsigned long					ulTrackIdentifier;
	#ifdef WIN32
		__int64						ullNowUTCTimeInMilliSecs;
	#else
		unsigned long long			ullNowUTCTimeInMilliSecs;
	#endif
	RTSPStatus_t		rsRTSPStatus;


	if (DateTime:: nowUTCInMilliSecs (
		&ullNowUTCTimeInMilliSecs,
		(long *) NULL) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_DATETIME_NOWUTCINMILLISECS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	/*
	if (ullNowUTCTimeInMilliSecs -
		_rsRTPSession. _ullLastSentRTCPPacketsInMilliSecs <
		_ulSendRTCPPacketsPeriodInMilliSecs)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_TOOEARLYTOSENDRTCPPACKETS);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		//	(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	*/

	if (rsRTSPStatus != SS_RTSP_PLAYING &&
		rsRTSPStatus != SS_RTSP_READY)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "handleSenderRTCPPackets", (long) rsRTSPStatus);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		// 	(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	for (ulTrackIdentifier = 0;
		ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
		ulTrackIdentifier++)
	{
		if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_bTrackInitialized))
			continue;

		if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _bActivated))
			continue;

		if (sendRTCPSRPacket (ulTrackIdentifier,
			ullNowUTCTimeInMilliSecs, false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTCPSRPACKET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	_rsRTPSession. _ullLastSentRTCPPacketsInMilliSecs	=
		ullNowUTCTimeInMilliSecs;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: tryToReadAndProcessRTCPPackets (
	unsigned long ulTrackIdentifier,
	unsigned long *pulRTCPPacketsNumber)

{

	Error_t											errRead;
	SocketImpl_p									pServerSocketImpl;
	char											pRTCPPacket [
		SS_RTSPSESSION_MAXRTCPPACKETLENGTH];
	unsigned long									ulPacketLength;



	*pulRTCPPacketsNumber			= 0;

	//	RTCP server
	if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_ssRTCPServerSocket).  getSocketImpl (&pServerSocketImpl) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKET_GETSOCKETIMPL_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	ulPacketLength				= SS_RTSPSESSION_MAXRTCPPACKETLENGTH;

	if ((errRead = pServerSocketImpl -> read (pRTCPPacket,
		&ulPacketLength, true, 0, 1000, true, true)) != errNoError)
	{
		if ((long) errRead != SCK_NOTHINGTOREAD)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETIMPL_READ_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (errRead == errNoError)	// errRead could be SCK_NOTHINGTOREAD
	{
		if (processRTCPPacket (&((_rsRTPSession. _ptiTracks) [
			ulTrackIdentifier]), pRTCPPacket, ulPacketLength) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_PROCESSRTCPPACKET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		(*pulRTCPPacketsNumber)++;
	}


	return errNoError;
}


Error RTSPSession:: processRTCPPacket (
	TrackInfo_p ptiTrackInfo,
	char *pRTCPPacket, long lRTCPPacketLength)

{

	long						lCurrentRTCPPacketLength;


	lCurrentRTCPPacketLength			= lRTCPPacketLength;

	while (lCurrentRTCPPacketLength > 0)
	{
		// Due to the variable-type nature of RTCP packets,
		// this is a bit unusual...  We initially treat the packet
		// as a generic RTCPPacket in order to determine its'
		// actual packet type.  Once that is figgered out, we treat it as its'
		// actual packet type

		RTCPPacket rtcpPacket;

		if (!rtcpPacket.ParsePacket((unsigned char*)pRTCPPacket,
			lCurrentRTCPPacketLength))
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_RTCPMALFORMED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		ptiTrackInfo -> _ulRTCPPacketsNumberReceived	+=
			1;
		ptiTrackInfo -> _ullRTCPBytesReceived			+=
			lCurrentRTCPPacketLength;

		switch (rtcpPacket.GetPacketType())
		{
			case RTCPPacket::kReceiverPacketType:
			{
				RTCPReceiverPacket receiverPacket;

				if (!receiverPacket. ParseReceiverReport (
					(unsigned char*)pRTCPPacket, lCurrentRTCPPacketLength))
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_RTCPMALFORMED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				//
				// Set the Client SSRC based on latest RTCP
				ptiTrackInfo -> _ulClientSSRC
					= rtcpPacket. GetPacketSSRC ();

				ptiTrackInfo -> _ulLostRTPPacketsNumber	=
					receiverPacket. GetCumulativeTotalLostPackets ();

				ptiTrackInfo -> _ulJitter	=
					receiverPacket.GetCumulativeJitter ();

				/*
                fFractionLostPackets = receiverPacket.GetCumulativeFractionLostPackets();
                fJitter = receiverPacket.GetCumulativeJitter();
                
                UInt32 curTotalLostPackets = receiverPacket.GetCumulativeTotalLostPackets();
                
                // Workaround for client problem.  Sometimes it appears to report a bogus lost packet count.
                // Since we can't have lost more packets than we sent, ignore the packet if that seems to be the case.
                if (curTotalLostPackets - fTotalLostPackets <= fPacketCount - fLastPacketCount)
                {
                    // if current value is less than the old value, that means that the packets are out of order
                    //  just wait for another packet that arrives in the right order later and for now, do nothing
                    if (curTotalLostPackets > fTotalLostPackets)
                    {   
                        //increment the server total by the new delta
                        QTSServerInterface::GetServer()->IncrementTotalRTPPacketsLost(curTotalLostPackets - fTotalLostPackets);
                        fCurPacketsLostInRTCPInterval = curTotalLostPackets - fTotalLostPackets;
    //                  qtss_printf("fCurPacketsLostInRTCPInterval = %d\n", fCurPacketsLostInRTCPInterval);
                        fTotalLostPackets = curTotalLostPackets;
                    }
                    else if(curTotalLostPackets == fTotalLostPackets)
                    {
                        fCurPacketsLostInRTCPInterval = 0;
    //                  qtss_printf("fCurPacketsLostInRTCPInterval set to 0\n");
                    }
                    
                                    
                    fPacketCountInRTCPInterval = fPacketCount - fLastPacketCount;
                    fLastPacketCount = fPacketCount;
                }
				*/

				{
					Message msg = StreamingServerMessages (__FILE__, __LINE__,
						SS_RTSPSESSION_RECEIVEDRECEIVERRTCPMESSAGE,
						7,
						_ulIdentifier,
						_pClientIPAddress,
						ptiTrackInfo -> _ulRTCPClientPort,
						ptiTrackInfo -> _pTrackURI,
						(unsigned long) receiverPacket. GetCumulativeFractionLostPackets (),
						(unsigned long) receiverPacket. GetCumulativeTotalLostPackets (),
						(unsigned long) receiverPacket. GetCumulativeJitter ());
					_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
						__FILE__, __LINE__);
				}

#ifdef DEBUG_RTCP_PACKETS
				receiverPacket.Dump();
#endif
			}
			break;

			case RTCPPacket::kAPPPacketType:
			{
				//
				// Check and see if this is an Ack packet. If it is, update the UDP Resender
				RTCPAckPacket theAckPacket;
                unsigned char* packetBuffer = rtcpPacket.GetPacketBuffer();
                unsigned long packetLen = (rtcpPacket.GetPacketLength() * 4) + RTCPPacket::kRTCPHeaderSizeInBytes;
                
                
                if (theAckPacket.ParseAckPacket(packetBuffer, packetLen))
                {
					long pippo;
					pippo = 10;
					/*
                    if (NULL != fTracker && false == fTracker->ReadyForAckProcessing()) // this stream must be ready to receive acks.  Between RTSP setup and sending of first packet on stream we must protect against a bad ack.
                    {   fSession->GetSessionMutex()->Unlock();
                        return;//abort if we receive an ack when we haven't sent anything.
                    }
                        
                    this->PrintPacketPrefEnabled( (char*)packetBuffer,  packetLen, RTPStream::rtcpACK);
                    // Only check for ack packets if we are using Reliable UDP
                    if (fTransportType == qtssRTPTransportTypeReliableUDP)
                    {
                        UInt16 theSeqNum = theAckPacket.GetAckSeqNum();
                        fResender.AckPacket(theSeqNum, curTime);
                        //qtss_printf("Got ack: %d\n",theSeqNum);
                        
                        for (UInt16 maskCount = 0; maskCount < theAckPacket.GetAckMaskSizeInBits(); maskCount++)
                        {
                            if (theAckPacket.IsNthBitEnabled(maskCount))
                            {
                                fResender.AckPacket( theSeqNum + maskCount + 1, curTime);
                                //qtss_printf("Got ack in mask: %d\n",theSeqNum + maskCount + 1);
                            }
                        }
                        
                    */
                }
                else
                {  
					long pippo;
					pippo = 10;
				   /*
                   this->PrintPacketPrefEnabled( (char*) packetBuffer, packetLen, RTPStream::rtcpAPP);
                   //
                    // If it isn't an ACK, assume its the qtss APP packet
                    RTCPCompressedQTSSPacket compressedQTSSPacket;
                    if (!compressedQTSSPacket.ParseCompressedQTSSPacket((UInt8*)currentPtr.Ptr, currentPtr.Len))
                        return;//abort if we discover a malformed app packet

                    fReceiverBitRate =      compressedQTSSPacket.GetReceiverBitRate();
                    fAvgLateMsec =          compressedQTSSPacket.GetAverageLateMilliseconds();
                    
                    fPercentPacketsLost =   compressedQTSSPacket.GetPercentPacketsLost();
                    fAvgBufDelayMsec =      compressedQTSSPacket.GetAverageBufferDelayMilliseconds();
                    fIsGettingBetter = (UInt16)compressedQTSSPacket.GetIsGettingBetter();
                    fIsGettingWorse = (UInt16)compressedQTSSPacket.GetIsGettingWorse();
                    fNumEyes =              compressedQTSSPacket.GetNumEyes();
                    fNumEyesActive =        compressedQTSSPacket.GetNumEyesActive();
                    fNumEyesPaused =        compressedQTSSPacket.GetNumEyesPaused();
                    fTotalPacketsRecv =     compressedQTSSPacket.GetTotalPacketReceived();
                    fTotalPacketsDropped =  compressedQTSSPacket.GetTotalPacketsDropped();
                    fTotalPacketsLost =     compressedQTSSPacket.GetTotalPacketsLost();
                    fClientBufferFill =     compressedQTSSPacket.GetClientBufferFill();
                    fFrameRate =            compressedQTSSPacket.GetFrameRate();
                    fExpectedFrameRate =    compressedQTSSPacket.GetExpectedFrameRate();
                    fAudioDryCount =        compressedQTSSPacket.GetAudioDryCount();
                    
//                  if (fPercentPacketsLost == 0)
//                  {
//                      qtss_printf("***\n");
//                      fCurPacketsLostInRTCPInterval = 0;
//                  }
                    //
                    // Update our overbuffer window size to match what the client is telling us
                    if (fTransportType != qtssRTPTransportTypeUDP)
                    {
//                      qtss_printf("Setting over buffer to %d\n", compressedQTSSPacket.GetOverbufferWindowSize());
                        fSession->GetOverbufferWindow()->SetWindowSize(compressedQTSSPacket.GetOverbufferWindowSize());
                    }
                    
#ifdef DEBUG_RTCP_PACKETS
                compressedQTSSPacket.Dump();
#endif
                }
				*/
            }

				{
					Message msg = StreamingServerMessages (__FILE__, __LINE__,
						SS_RTSPSESSION_RECEIVEDAPPRTCPMESSAGE,
						4,
						_ulIdentifier,
						_pClientIPAddress,
						ptiTrackInfo -> _ulRTCPClientPort,
						ptiTrackInfo -> _pTrackURI);
					_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
						__FILE__, __LINE__);
				}
			}

			break;

			case RTCPPacket::kSDESPacketType:
			{
#ifdef DEBUG_RTCP_PACKETS
				SourceDescriptionPacket sedsPacket;
				if (!sedsPacket.ParsePacket((unsigned char*)pRTCPPacket, lCurrentRTCPPacketLength))
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_RTCPMALFORMED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				sedsPacket.Dump();
#endif
				{
					Message msg = StreamingServerMessages (__FILE__, __LINE__,
						SS_RTSPSESSION_RECEIVEDSDESRTCPMESSAGE,
						4,
						_ulIdentifier,
						_pClientIPAddress,
						ptiTrackInfo -> _ulRTCPClientPort,
						ptiTrackInfo -> _pTrackURI);
					_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
						__FILE__, __LINE__);
				}
			}
			break;

			//default:
			//	WarnV(false, "Unknown RTCP Packet Type");
			break;

		}


		pRTCPPacket += (rtcpPacket.GetPacketLength() * 4 ) + 4;
		lCurrentRTCPPacketLength -= (rtcpPacket.GetPacketLength() * 4 ) + 4;
	}

	// Invoke the RTCP modules, allowing them to process this packet
	/* INIZIO GIULIANO
	QTSS_RoleParams theParams;
	theParams.rtcpProcessParams.inRTPStream = this;
	theParams.rtcpProcessParams.inClientSession = fSession;
	theParams.rtcpProcessParams.inRTCPPacketData = inPacket->Ptr;
	theParams.rtcpProcessParams.inRTCPPacketDataLen = inPacket->Len;

	// We don't allow async events from this role, so just set an empty module state.
	OSThreadDataSetter theSetter(&sRTCPProcessModuleState, NULL);

	//no matter what happens (whether or not this is a valid packet) reset the timeouts
	fSession->RefreshTimeout();
	if (fSession->GetRTSPSession() != NULL)
		fSession->GetRTSPSession()->RefreshTimeout();

	// Invoke RTCP processing modules
	for (unsigned long x = 0; x < QTSServerInterface::GetNumModulesInRole(QTSSModule::kRTCPProcessRole); x++)
		(void)QTSServerInterface::GetModule(QTSSModule::kRTCPProcessRole, x)->CallDispatch(QTSS_RTCPProcess_Role, &theParams);

	fSession->GetSessionMutex()->Unlock();
	FINE GIULIANO */


	return errNoError;
}


#ifdef WIN32
	Error RTSPSession:: sendRTCPSRPacket (
		unsigned long ulTrackIdentifier,
		__int64 llUTCTimeInMilliSecs,
		Boolean_t bEndOfPartecipation)
#else
	Error RTSPSession:: sendRTCPSRPacket (
		unsigned long ulTrackIdentifier,
		long long llUTCTimeInMilliSecs,
		Boolean_t bEndOfPartecipation)
#endif

{

	SocketImpl_p								pClientSocketImpl;
	Error_t										errWrite;


	if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_csRTCPClientSocket).  getSocketImpl (&pClientSocketImpl) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKET_GETSOCKETIMPL_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (buildRTCPSRPacket (llUTCTimeInMilliSecs,
		&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]),
		bEndOfPartecipation,
		_rsRTPSession. _ullPlayTimeInMilliSeconds +
		_ulPlayDelayTimeInMilliSeconds,
		_rsRTPSession. _llNTPPlayTime) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_BUILDRTCPSRPACKET_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_RTSPSESSION_SENDRTCPPACKET,
			9,
			_ulIdentifier,
			(double) (llUTCTimeInMilliSecs / 1000),
			_pClientIPAddress,
			((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_ulRTCPClientPort,
			((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _pTrackURI,
			((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_lRTCPSRPacketLength,
			bEndOfPartecipation ? "true" : "false",
			((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_ulSentRTPPacketsNumber,
			((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_ullRTPPacketBytesSent);
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if ((errWrite = pClientSocketImpl -> write (
		(void *) ((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_pRTCPSRPacket,
		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _lRTCPSRPacketLength,
		true, 1, 0, _pClientIPAddress,
		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_ulRTCPClientPort)) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error RTSPSession:: buildRTCPSRPacket (__int64 llUTCTimeInMilliSecs,
		TrackInfo_p ptiTrackInfo,
		Boolean_t bIsItRTCPBye, __int64 llPlayTimeInMilliSeconds,
		__int64 llNTPPlayTime)
#else
	Error RTSPSession:: buildRTCPSRPacket (long long llUTCTimeInMilliSecs,
		TrackInfo_p ptiTrackInfo,
		Boolean_t bIsItRTCPBye, long long llPlayTimeInMilliSeconds,
		long long llNTPPlayTime)
#endif

{
        //
        // This will roll over, after which payloadByteCount will be all
		// messed up. But because it is a 32 bit number, that is bound
		// to happen eventually, and we are limited by the RTCP packet format
		// in that respect, so this is pretty much ok.
	unsigned long payloadByteCount = (unsigned long)
		(ptiTrackInfo -> _ullRTPPacketBytesSent -
		(12 * ptiTrackInfo -> _ulSentRTPPacketsNumber));

	RTCPSRPacket theSR;
	theSR.SetSSRC(ptiTrackInfo -> _ulServerSSRC);
	theSR.SetClientSSRC(ptiTrackInfo -> _ulClientSSRC);
	#ifdef WIN32
		theSR.SetNTPTimestamp(llNTPPlayTime +
			(__int64)(.5 + (llUTCTimeInMilliSecs - llPlayTimeInMilliSeconds) *
			4294967.296L));
	#else
		theSR.SetNTPTimestamp(llNTPPlayTime +
			(long long)(.5 + (llUTCTimeInMilliSecs - llPlayTimeInMilliSeconds) *
			4294967.296L));
	#endif
	// The RTP timestamp describes the same instant as the NPT timestamp
	// but is measured in the same units as the timestamps issued in the
	// data packets of the sender These two timestamps allow to synchronise
	// a receiver's playout clock rate with the sampling clock rate
	// of the sender.
	theSR.SetRTPTimestamp(ptiTrackInfo -> _ulLastRelativeRTPPacketTransmissionTimeWithTimeScaleAndOffset);
	theSR.SetPacketCount(ptiTrackInfo -> _ulSentRTPPacketsNumber);
	theSR.SetByteCount(payloadByteCount);

	// min is 20, max is 100
	// you can change this value dinamically
	// (see RTPBandwidthTracker::UpdateAckTimeout)
	theSR.SetAckTimeout(100);

	ptiTrackInfo -> _lRTCPSRPacketLength = theSR.GetSRPacketLen();
	if (bIsItRTCPBye)
		ptiTrackInfo -> _lRTCPSRPacketLength = theSR.GetSRWithByePacketLen();


	memcpy (ptiTrackInfo -> _pRTCPSRPacket, theSR.GetSRPacket(),
		ptiTrackInfo -> _lRTCPSRPacketLength);


	return errNoError;
}


Error RTSPSession:: sendRTSPResponse (SocketImpl_p psiClientSocketImpl,
	const char *pRTSPVersion, unsigned long ulStatusCode,
	long lRTSPSequenceNumber, const char *pSessionIdentifier,
	unsigned long ulSessionTimeout, const char *pTimestamp,
	Boolean_t bConnectionClose)

{

	Buffer_t						bResponse;
	Error_t							errWrite;


	if (bResponse. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (RTSPUtility:: getRTSPResponse (pRTSPVersion, ulStatusCode,
		lRTSPSequenceNumber,
		pSessionIdentifier, ulSessionTimeout, bConnectionClose,
		pTimestamp, &bResponse, _ptSystemTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETRTSPRESPONSE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((errWrite = psiClientSocketImpl -> writeString (
		(const char *) bResponse, true, 1, 0)) != errNoError)
	{
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errWrite, __FILE__, __LINE__);
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_RTSPSESSION_SENDTOCLIENT,
			2, _ulIdentifier, (const char *) bResponse);
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (bResponse. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error RTSPSession:: processRTSPRequest (
		__int64 *pullTotalBytesServed,
		__int64 *pullTotalLostPacketsNumber,
		__int64 *pullTotalSentRTPPacketsNumber,
		Boolean_p pbIsRTSPFinished)
#else
	Error RTSPSession:: processRTSPRequest (
		unsigned long long *pullTotalBytesServed,
		unsigned long long *pullTotalLostPacketsNumber,
		unsigned long long *pullTotalSentRTPPacketsNumber,
		Boolean_p pbIsRTSPFinished)
#endif

{

	char					pRTSPRequest [SS_RTSPREQUESTLENGTH];
	unsigned long			ulRTSPResponseLength;
	Error_t					errRead;
	const char				*pLocalRTSPRequest;
	unsigned long			ulMethodLength;
	Error_t					errHandleMethod;
	RTSPStatus_t			rsRTSPStatus;
	Error_t					errTimes;


	*pullTotalBytesServed			= 0;
	*pullTotalLostPacketsNumber		= 0;
	*pullTotalSentRTPPacketsNumber		= 0;
	*pbIsRTSPFinished				= false;

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		*pbIsRTSPFinished				= true;

		if (closeRTSPConnection (&err,
			pullTotalBytesServed,
			pullTotalLostPacketsNumber,
			pullTotalSentRTPPacketsNumber) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		*pbIsRTSPFinished				= true;

		if (closeRTSPConnection (&err,
			pullTotalBytesServed,
			pullTotalLostPacketsNumber,
			pullTotalSentRTPPacketsNumber) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus == RTSPSession:: SS_RTSP_NOTUSED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "processRTSPRequest", (long) rsRTSPStatus);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		// 	(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	ulRTSPResponseLength		= SS_RTSPREQUESTLENGTH - 1;

	if ((errRead = _psiClientSocketImpl -> read (
		pRTSPRequest, &ulRTSPResponseLength, false,
		0, 0, true, true)) != errNoError)
	{
		if ((long) errRead == SCK_READ_EOFREACHED)
		{
			Error errConnectionDown = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_RTSPCONNECTIONDOWN,
				3, _ulIdentifier, _pClientIPAddress, _lRTSPClientPort);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errConnectionDown, __FILE__, __LINE__);

			*pbIsRTSPFinished				= true;

			if (closeRTSPConnection (&errConnectionDown,
				pullTotalBytesServed,
				pullTotalLostPacketsNumber,
				pullTotalSentRTPPacketsNumber) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errConnectionDown;
		}
		/*
		else if ((long) errReadLines == SCK_NOTHINGTOREAD)
		{
			// ???
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errReadLines, __FILE__, __LINE__);

			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_NOTHINGTOREAD,
				1, _ulIdentifier);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errReadLines;
		}
		*/
		else
		{
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errRead, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETIMPL_READ_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			*pbIsRTSPFinished				= true;

			if (closeRTSPConnection (&errRead,
				pullTotalBytesServed,
				pullTotalLostPacketsNumber,
				pullTotalSentRTPPacketsNumber) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	pRTSPRequest [ulRTSPResponseLength]			= '\0';

	#ifdef VIDIATORSTRESSOR
		Buffer_t		bBuffer;
		bBuffer. init (pRTSPRequest);
		bBuffer. substitute ("Cseq:", "CSeq:");
		bBuffer. substitute ("Transport:", "Transport: ");
		strcpy (pRTSPRequest, (const char *) bBuffer);
		bBuffer. finish ();
	#endif
	/*
	{
		char                *pEndRTSPHeader;



		if ((pEndRTSPHeader = strstr (pRTSPRequest, "\r\n\r\n")) !=
			(char *) NULL)
		{
			// Since the last char is '\0', I'm sure that after "\r\n\r\n"
			//	we have at least a char of the buffer. For that reason I can
			//	initialize it with '\0' to be sure to close the buffer read
			//	with just the RTSP header
			*(pEndRTSPHeader + 4)			= '\0';
			ulRTSPResponseLength			= strlen (pEndRTSPHeader);
		}
	}
	*/

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_RTSPSESSION_RECEIVEDRTSPMESSAGE,
			4, _ulIdentifier,
			_pClientIPAddress, _lRTSPClientPort, pRTSPRequest);
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

	pLocalRTSPRequest			= pRTSPRequest;

	while (pLocalRTSPRequest [0] != '\0')
	{
		if (strncmp (pLocalRTSPRequest, "DESCRIBE", strlen ("DESCRIBE")) == 0)
		{
			_rcPreviousRTSPCommand			= _rcLastRTSPCommand;
			_rcLastRTSPCommand				= SS_RTSP_DESCRIBE;

			if ((errHandleMethod = handleDESCRIBEMethod (
				pLocalRTSPRequest, &ulMethodLength)) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_HANDLEDESCRIBEMETHOD_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&errHandleMethod,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. stop ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_STOP_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. start ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_START_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (strncmp (pLocalRTSPRequest, "ANNOUNCE",
			strlen ("ANNOUNCE")) == 0)
		{
			// For the management of the PLAY RTSP command (handlePLAYMethod),
			// it is important to know the previous RTSP command if it is SETUP
			// or PAUSE. If we will have a scenario like
			//	SETUP ANNOUNCE PLAY, the handlePLAYMethod does not work. For
			// that reason the ANNOUNCE method will not influence the
			// _rcPreviousRTSPCommand and _rcLastRTSPCommand variables.

			// _rcPreviousRTSPCommand			= _rcLastRTSPCommand;
			// _rcLastRTSPCommand				= SS_RTSP_ANNOUNCE;

			if ((errHandleMethod = handleANNOUNCEMethod (
				pLocalRTSPRequest, &ulMethodLength)) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_HANDLEANNOUNCEMETHOD_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&errHandleMethod,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. stop ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_STOP_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. start ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_START_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (strncmp (pLocalRTSPRequest, "GET_PARAMETER",
			strlen ("GET_PARAMETER")) == 0)
		{
			// For the management of the PLAY RTSP command (handlePLAYMethod),
			// it is important to know the previous RTSP command if it is SETUP
			// or PAUSE. If we will have a scenario like
			//	SETUP GET_PARAMETER PLAY, the handlePLAYMethod does not work.
			// For that reason the GET_PARAMETER method will not influence the
			// _rcPreviousRTSPCommand and _rcLastRTSPCommand variables.

			// _rcPreviousRTSPCommand			= _rcLastRTSPCommand;
			// _rcLastRTSPCommand				= SS_RTSP_GET_PARAMETER;

			if ((errHandleMethod = handleGET_PARAMETERMethod (
				pLocalRTSPRequest, &ulMethodLength)) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_HANDLEGET_PARAMETERMETHOD_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&errHandleMethod,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. stop ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_STOP_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. start ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_START_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (strncmp (pLocalRTSPRequest, "OPTIONS",
			strlen ("OPTIONS")) == 0)
		{
			// For the management of the PLAY RTSP command (handlePLAYMethod),
			// it is important to know the previous RTSP command if it is SETUP
			// or PAUSE. If we will have a scenario like
			//	SETUP OPTIONS PLAY, the handlePLAYMethod does not work. For
			// that reason the OPTIONS method will not influence the
			// _rcPreviousRTSPCommand and _rcLastRTSPCommand variables.

			// _rcPreviousRTSPCommand			= _rcLastRTSPCommand;
			// _rcLastRTSPCommand				= SS_RTSP_OPTIONS;

			if ((errHandleMethod = handleOPTIONSMethod (
				pLocalRTSPRequest, &ulMethodLength)) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_HANDLEOPTIONSMETHOD_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&errHandleMethod,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. stop ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_STOP_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. start ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_START_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (strncmp (pLocalRTSPRequest, "PAUSE",
			strlen ("PAUSE")) == 0)
		{
			_rcPreviousRTSPCommand			= _rcLastRTSPCommand;
			_rcLastRTSPCommand				= SS_RTSP_PAUSE;

			if ((errHandleMethod = handlePAUSEMethod (
				pLocalRTSPRequest, &ulMethodLength)) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_HANDLEPAUSEMETHOD_FAILED,
					1, _ulIdentifier);

				// The catraStreamingPlatform goes in a PAUSE state once
				// the clip is finished. Some player (like QuickTime) sends
				// the PAUSE RTSP command once the clip is finished and in that
				// case the catraStreaminghServer raise an error.
				// The next check avoid that the catraStreamingServer log
				// as error when it receives a PAUSE command and
				// it is already in that state
				if ((unsigned long) errHandleMethod ==
					SS_RTSPSESSION_WRONGSTATE)
				{
					_ptSystemTracer -> trace (Tracer:: TRACER_LWRNG,
						(const char *) err, __FILE__, __LINE__);
					err			= errHandleMethod;
				}
				else
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&errHandleMethod,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _ptPauseTimes. start ()) != errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_START_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. stop ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_STOP_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. start ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_START_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (strncmp (pLocalRTSPRequest, "PLAY",
			strlen ("PLAY")) == 0)
		{
			_rcPreviousRTSPCommand			= _rcLastRTSPCommand;
			_rcLastRTSPCommand				= SS_RTSP_PLAY;

			if ((errHandleMethod = handlePLAYMethod (
				pLocalRTSPRequest, &ulMethodLength,
				_rcPreviousRTSPCommand)) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_HANDLEPLAYMETHOD_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&errHandleMethod,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (_rcPreviousRTSPCommand == SS_RTSP_PAUSE)
			{
				if ((errTimes = _ptPauseTimes. stop ()) !=
					errNoError)
				{
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) errTimes, __FILE__, __LINE__);

					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_TIMES_STOP_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					*pbIsRTSPFinished				= true;

					if (closeRTSPConnection (&err,
						pullTotalBytesServed,
						pullTotalLostPacketsNumber,
						pullTotalSentRTPPacketsNumber) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. stop ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_STOP_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. start ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_START_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// _rcPreviousRTSPCommand could be also PAUSE
			if (_rcPreviousRTSPCommand == RTSPSession:: SS_RTSP_SETUP)
			{
				if ((errTimes = _srptSendRTCPPacketsTimes. start ()) !=
					errNoError)
				{
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) errTimes, __FILE__, __LINE__);

					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_TIMES_START_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					*pbIsRTSPFinished				= true;

					if (closeRTSPConnection (&err,
						pullTotalBytesServed,
						pullTotalLostPacketsNumber,
						pullTotalSentRTPPacketsNumber) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if ((errTimes = _crrptCheckReceivedRTCPPacketsTimes. start ()) !=
					errNoError)
				{
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) errTimes, __FILE__, __LINE__);

					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_TIMES_START_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					*pbIsRTSPFinished				= true;

					if (closeRTSPConnection (&err,
						pullTotalBytesServed,
						pullTotalLostPacketsNumber,
						pullTotalSentRTPPacketsNumber) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
		}
		else if (strncmp (pLocalRTSPRequest, "RECORD",
			strlen ("RECORD")) == 0)
		{
			_rcPreviousRTSPCommand			= _rcLastRTSPCommand;
			_rcLastRTSPCommand				= SS_RTSP_RECORD;

			if ((errHandleMethod = handleRECORDMethod (
				pLocalRTSPRequest, &ulMethodLength)) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_HANDLERECORDMETHOD_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&errHandleMethod,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. stop ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_STOP_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. start ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_START_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (strncmp (pLocalRTSPRequest, "REDIRECT",
			strlen ("REDIRECT")) == 0)
		{
			_rcPreviousRTSPCommand			= _rcLastRTSPCommand;
			_rcLastRTSPCommand				= SS_RTSP_REDIRECT;

			if ((errHandleMethod = handleREDIRECTMethod (
				pLocalRTSPRequest, &ulMethodLength)) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_HANDLEREDIRECTMETHOD_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&errHandleMethod,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. stop ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_STOP_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. start ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_START_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (strncmp (pLocalRTSPRequest, "SETUP",
			strlen ("SETUP")) == 0)
		{
			_rcPreviousRTSPCommand			= _rcLastRTSPCommand;
			_rcLastRTSPCommand				= SS_RTSP_SETUP;

			if ((errHandleMethod = handleSETUPMethod (
				pLocalRTSPRequest, &ulMethodLength)) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_HANDLESETUPMETHOD_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&errHandleMethod,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. stop ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_STOP_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. start ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_START_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (strncmp (pLocalRTSPRequest, "SET_PARAMETER",
			strlen ("SET_PARAMETER")) == 0)
		{
			// For the management of the PLAY RTSP command (handlePLAYMethod),
			// it is important to know the previous RTSP command if it is SETUP
			// or PAUSE. If we will have a scenario like
			//	SETUP SET_PARAMETER PLAY, the handlePLAYMethod does not work.
			// For that reason the SET_PARAMETER method will not influence the
			// _rcPreviousRTSPCommand and _rcLastRTSPCommand variables.

			// _rcPreviousRTSPCommand			= _rcLastRTSPCommand;
			// _rcLastRTSPCommand				= SS_RTSP_SET_PARAMETER;

			if ((errHandleMethod = handleSET_PARAMETERMethod (
				pLocalRTSPRequest, &ulMethodLength)) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_HANDLESET_PARAMETERMETHOD_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&errHandleMethod,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. stop ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_STOP_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errTimes = _rrtRTSP_RTCPTimes. start ()) !=
				errNoError)
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTimes, __FILE__, __LINE__);

				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_START_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (strncmp (pLocalRTSPRequest, "TEARDOWN",
			strlen ("TEARDOWN")) == 0)
		{
			_rcPreviousRTSPCommand			= _rcLastRTSPCommand;
			_rcLastRTSPCommand				= SS_RTSP_TEARDOWN;

			if ((errHandleMethod = handleTEARDOWNMethod (
				pLocalRTSPRequest, &ulMethodLength,
				pbIsRTSPFinished)) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_HANDLETEARDOWNMETHOD_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				*pbIsRTSPFinished				= true;

				if (closeRTSPConnection (&errHandleMethod,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (*pbIsRTSPFinished)
			{
				Error_t		err;


				err				= errNoError;

				if (closeRTSPConnection (&err,
					pullTotalBytesServed,
					pullTotalLostPacketsNumber,
					pullTotalSentRTPPacketsNumber) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtRTSPSession. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
		}
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_UNKNOWNRTSPREQUEST,
				1, pLocalRTSPRequest);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			*pbIsRTSPFinished				= true;

			if (closeRTSPConnection (&err,
				pullTotalBytesServed,
				pullTotalLostPacketsNumber,
				pullTotalSentRTPPacketsNumber) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		pLocalRTSPRequest			+= ulMethodLength;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		*pbIsRTSPFinished				= true;

		if (closeRTSPConnection (&err,
			pullTotalBytesServed,
			pullTotalLostPacketsNumber,
			pullTotalSentRTPPacketsNumber) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}


	return errNoError;
}


Error RTSPSession:: isLiveRequest (const char *pAssetPath,
	Boolean_p pbIsLive)

{
	const char			*pExtension;


	pExtension			= pAssetPath;
	pExtension			+= (strlen (pAssetPath) - 4);

	if (!strcmp (pExtension, ".sdp"))
		*pbIsLive			= true;
	else
		*pbIsLive			= false;


	return errNoError;
}


/*
Error RTSPSession:: parseAndAdjustLiveSDP (
	Buffer_p pbSDP,
	Boolean_p pbVideoTrackFoundInSDP,
	Boolean_p pbAudioTrackFoundInSDP,
	unsigned long *pulVideoPayloadNumber,
	unsigned long *pulAudioPayloadNumber,
	char *pVideoTrackName,
	char *pAudioTrackName,
	long *plLiveVideoRTPPacketsServerPort,
	long *plLiveAudioRTPPacketsServerPort,
	MP4Atom:: MP4Codec_p pcLiveVideoCodecUsed,
	MP4Atom:: MP4Codec_p pcLiveAudioCodecUsed,
	unsigned long *pulLiveVideoAvgBitRate,
	unsigned long *pulLiveAudioAvgBitRate)

{

	const char				*pVideoMedia;
	const char				*pAudioMedia;
	char					*pBeginToken;
	StringTokenizer_t		stSpaceTokenizer;
	const char				*pToken;
	Error					errNextToken;
	Boolean_t				bVideoControlAdded;
	Boolean_t				bAudioControlAdded;
	char					pAvgBitRate [SS_MAXLONGLENGTH];


	*pcLiveVideoCodecUsed		= MP4Atom:: MP4F_CODEC_UNKNOWN;
	*pcLiveAudioCodecUsed		= MP4Atom:: MP4F_CODEC_UNKNOWN;

	*pulLiveVideoAvgBitRate		= 0;
	*pulLiveAudioAvgBitRate		= 0;

	// inside the SDP we must have SS_NEWLINE
	if (strstr ((const char *) (*pbSDP),
		SS_NEWLINE) == (char *) NULL)
	{
		if (pbSDP -> substitute ("\n", SS_NEWLINE) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// the SDP must finish with one SS_NEWLINE
	{
		if (pbSDP -> strip (Buffer:: STRIPTYPE_TRAILING,
			SS_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SUBSTITUTE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (SS_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if ((pVideoMedia = strstr ((const char *) (*pbSDP),
		SS_NEWLINE "m=video")) != (char *) NULL)
	{
		*pbVideoTrackFoundInSDP			= true;
	}
	else
	{
		*pbVideoTrackFoundInSDP			= false;
	}
	pVideoMedia			+= strlen (SS_NEWLINE);

	if ((pAudioMedia = strstr ((const char *) (*pbSDP),
		SS_NEWLINE "m=audio")) != (char *) NULL)
	{
		*pbAudioTrackFoundInSDP			= true;
	}
	else
	{
		*pbAudioTrackFoundInSDP			= false;
	}
	pAudioMedia			+= strlen (SS_NEWLINE);

	// add the range field if not already inside the SDP
	if (strstr ((const char *) (*pbSDP),
		"a=range:npt=now-") == (char *) NULL)
	{
		if (*pbVideoTrackFoundInSDP &&
			*pbAudioTrackFoundInSDP)
		{
			if (pVideoMedia < pAudioMedia)
			{
				if (pbSDP -> substitute ("m=video",
					"a=range:npt=now-"
					SS_NEWLINE "m=video") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}
			else
			{
				if (pbSDP -> substitute ("m=audio",
					"a=range:npt=now-"
					SS_NEWLINE "m=audio") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			pVideoMedia = strstr ((const char *) (*pbSDP),
				SS_NEWLINE "m=video");
			pVideoMedia			+= strlen (SS_NEWLINE);

			pAudioMedia = strstr ((const char *) (*pbSDP),
				SS_NEWLINE "m=audio");
			pAudioMedia			+= strlen (SS_NEWLINE);
		}
		else if (*pbVideoTrackFoundInSDP)
		{
			if (pbSDP -> substitute ("m=video",
				"a=range:npt=now-"
				SS_NEWLINE "m=video") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			pVideoMedia = strstr ((const char *) (*pbSDP),
				SS_NEWLINE "m=video");
			pVideoMedia			+= strlen (SS_NEWLINE);
		}
		else if (*pbAudioTrackFoundInSDP)
		{
			if (pbSDP -> substitute ("m=audio",
				"a=range:npt=now-"
				SS_NEWLINE "m=audio") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			pAudioMedia = strstr ((const char *) (*pbSDP),
				SS_NEWLINE "m=audio");
			pAudioMedia			+= strlen (SS_NEWLINE);
		}
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_WRONGLIVESDP,
				1, "no media found");
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// add the control field if not already inside the SDP
	bVideoControlAdded			= false;
	bAudioControlAdded			= false;

	if (strstr ((const char *) (*pbSDP),
		"a=control:") == (char *) NULL)
	{
		if (*pbVideoTrackFoundInSDP &&
			*pbAudioTrackFoundInSDP)
		{
			if (pVideoMedia < pAudioMedia)
			{
				if (pbSDP -> substitute ("m=video",
					"a=control:" SS_DEFAULTSDPGLOBALCONTROL
					SS_NEWLINE "m=video") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (pbSDP -> substitute ("m=audio",
					"a=control:" SS_DEFAULTVIDEOCONTROL
					SS_NEWLINE "m=audio") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (pbSDP -> append ("a=control:"
					SS_DEFAULTAUDIOCONTROL SS_NEWLINE) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}
			else
			{
				if (pbSDP -> substitute ("m=audio",
					"a=control:" SS_DEFAULTSDPGLOBALCONTROL
					SS_NEWLINE "m=audio") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (pbSDP -> substitute ("m=video",
					"a=control:" SS_DEFAULTAUDIOCONTROL
					SS_NEWLINE "m=video") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SUBSTITUTE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (pbSDP -> append ("a=control:"
					SS_DEFAULTVIDEOCONTROL SS_NEWLINE) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			pVideoMedia = strstr ((const char *) (*pbSDP),
				SS_NEWLINE "m=video");
			pVideoMedia			+= strlen (SS_NEWLINE);

			pAudioMedia = strstr ((const char *) (*pbSDP),
				SS_NEWLINE "m=audio");
			pAudioMedia			+= strlen (SS_NEWLINE);

			bVideoControlAdded			= true;
			bAudioControlAdded			= true;
		}
		else if (*pbVideoTrackFoundInSDP)
		{
			if (pbSDP -> substitute ("m=video",
				"a=control:" SS_DEFAULTSDPGLOBALCONTROL
				SS_NEWLINE "m=video") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbSDP -> append ("a=control:"
				SS_DEFAULTVIDEOCONTROL SS_NEWLINE) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			pVideoMedia = strstr ((const char *) (*pbSDP),
				SS_NEWLINE "m=video");
			pVideoMedia			+= strlen (SS_NEWLINE);

			bVideoControlAdded			= true;
		}
		else if (*pbAudioTrackFoundInSDP)
		{
			if (pbSDP -> substitute ("m=audio",
				"a=control:" SS_DEFAULTSDPGLOBALCONTROL
				SS_NEWLINE "m=audio") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbSDP -> append ("a=control:"
				SS_DEFAULTAUDIOCONTROL SS_NEWLINE) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			pAudioMedia = strstr ((const char *) (*pbSDP),
				SS_NEWLINE "m=audio");
			pAudioMedia			+= strlen (SS_NEWLINE);

			bAudioControlAdded			= true;
		}
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_WRONGLIVESDP,
				1, "no media found");
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// video
	if (*pbVideoTrackFoundInSDP)
	{
		if (stSpaceTokenizer. init (pVideoMedia,
			(long) (strstr (pVideoMedia, SS_NEWLINE) - pVideoMedia),
			" ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		// m=video
		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// socket RTP port
		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		*plLiveVideoRTPPacketsServerPort		=
			atol (pToken);

		// RTP/AVP
		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// Payload number
		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		*pulVideoPayloadNumber			= atol (pToken);

		if (stSpaceTokenizer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (bVideoControlAdded)
		{
			strcpy (pVideoTrackName, SS_DEFAULTVIDEOCONTROL);
		}
		else
		{
			if ((pBeginToken = strstr (pVideoMedia, "a=control:")) ==
				(char *) NULL)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_WRONGLIVESDP,
					1, "no video control found");
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			pBeginToken				+= strlen ("a=control:");

			strncpy (pVideoTrackName, pBeginToken,
				strstr (pBeginToken, SS_NEWLINE) - pBeginToken);
			pVideoTrackName [strstr (pBeginToken, SS_NEWLINE) - pBeginToken]	=
				'\0';
		}

		// avg bit rate
		if ((pBeginToken = strstr (pVideoMedia, "b=AS:")) ==
			(char *) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_WRONGLIVESDP,
				1, "no video AS found");
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pBeginToken				+= strlen ("b=AS:");

		strncpy (pAvgBitRate, pBeginToken,
			strstr (pBeginToken, SS_NEWLINE) - pBeginToken);
		pAvgBitRate [strstr (pBeginToken, SS_NEWLINE) - pBeginToken]	=
			'\0';

		*pulLiveVideoAvgBitRate			= atol (pAvgBitRate);

		// codec
		if ((pBeginToken = strstr (pVideoMedia, "a=rtpmap:")) ==
			(char *) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_WRONGLIVESDP,
				1, "no video rtpmap found");
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pBeginToken				= strchr (pBeginToken, ' ');

		while (*pBeginToken == ' ')
			pBeginToken++;

		if (!strncmp (pBeginToken, "H263-", 5))
			*pcLiveVideoCodecUsed		= MP4Atom:: MP4F_CODEC_H263;
		else if (!strncmp (pBeginToken, "MP4V-ES", 7))
			*pcLiveVideoCodecUsed		= MP4Atom:: MP4F_CODEC_MPEG4;
		else
			*pcLiveVideoCodecUsed		= MP4Atom:: MP4F_CODEC_UNKNOWN;
	}

	// audio
	if (*pbAudioTrackFoundInSDP)
	{
		if (stSpaceTokenizer. init (pAudioMedia,
			(long) (strstr (pAudioMedia, SS_NEWLINE) - pVideoMedia),
			" ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		// m=audio
		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// socket RTP port
		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		*plLiveAudioRTPPacketsServerPort		=
			atol (pToken);

		// RTP/AVP
		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// Payload number
		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		*pulAudioPayloadNumber			= atol (pToken);

		if (stSpaceTokenizer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (bAudioControlAdded)
		{
			strcpy (pAudioTrackName, SS_DEFAULTAUDIOCONTROL);
		}
		else
		{
			if ((pBeginToken = strstr (pAudioMedia, "a=control:")) ==
				(char *) NULL)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_WRONGLIVESDP,
					1, "no video control found");
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			pBeginToken				+= strlen ("a=control:");

			strncpy (pAudioTrackName, pBeginToken,
				strstr (pBeginToken, SS_NEWLINE) - pBeginToken);
			pAudioTrackName [strstr (pBeginToken, SS_NEWLINE) - pBeginToken]	=
				'\0';
		}

		// avg bit rate
		if ((pBeginToken = strstr (pAudioMedia, "b=AS:")) ==
			(char *) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_WRONGLIVESDP,
				1, "no audio AS found");
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pBeginToken				+= strlen ("b=AS:");

		strncpy (pAvgBitRate, pBeginToken,
			strstr (pBeginToken, SS_NEWLINE) - pBeginToken);
		pAvgBitRate [strstr (pBeginToken, SS_NEWLINE) - pBeginToken]	=
			'\0';

		*pulLiveAudioAvgBitRate			= atol (pAvgBitRate);

		// codec
		if ((pBeginToken = strstr (pAudioMedia, "a=rtpmap:")) ==
			(char *) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_WRONGLIVESDP,
				1, "no audio rtpmap found");
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pBeginToken				= strchr (pBeginToken, ' ');

		while (*pBeginToken == ' ')
			pBeginToken++;

		if (!strncmp (pBeginToken, "MP4A-LATM", 9))
			*pcLiveAudioCodecUsed		= MP4Atom:: MP4F_CODEC_AAC;
		else if (!strncmp (pBeginToken, "AMR-WB", 6))
			*pcLiveAudioCodecUsed		= MP4Atom:: MP4F_CODEC_AMRWB;
		else if (!strncmp (pBeginToken, "AMR", 3))
			*pcLiveAudioCodecUsed		= MP4Atom:: MP4F_CODEC_AMRNB;
		else if (!strncmp (pBeginToken, "mpeg4-generic", 13))
			*pcLiveAudioCodecUsed		= MP4Atom:: MP4F_CODEC_AAC;
		else
			*pcLiveAudioCodecUsed		= MP4Atom:: MP4F_CODEC_UNKNOWN;
	}


	return errNoError;
}
*/


Error RTSPSession:: getInfoFromMP4File (
	unsigned long ulMaxPayloadSizeInBytes, MP4File_p pmfFile,
	MP4RootAtom_p *pmaRootAtom,
	Boolean_p pbVideoTrackFoundInSDP, MP4TrackInfo_p *pmtiVideoMediaTrackInfo,
	MP4TrackInfo_p *pmtiVideoHintTrackInfo,
	Boolean_p pbAudioTrackFoundInSDP, MP4TrackInfo_p *pmtiAudioMediaTrackInfo,
	MP4TrackInfo_p *pmtiAudioHintTrackInfo,
	Boolean_t bUseOfHintingTrackIfExist,
	Boolean_t bBuildOfHintingTrackIfNotExist)

{

	Error_t					errChoiceMediaTracks;
	Error_t					errHintTrack;
	// MP4Atom_p				pmaAtom;
	std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo;


	*pmtiVideoHintTrackInfo		= (MP4TrackInfo_p) NULL;
	*pmtiAudioHintTrackInfo		= (MP4TrackInfo_p) NULL;

	if (pmfFile -> getRootAtom (pmaRootAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_GETROOTATOM_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmfFile -> getTracksInfo (&pvMP4TracksInfo) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_GETTRACKSINFO_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errChoiceMediaTracks = choiceMediaTracksToStream (
		pmfFile,
		pbVideoTrackFoundInSDP, pmtiVideoMediaTrackInfo,
		pbAudioTrackFoundInSDP, pmtiAudioMediaTrackInfo)) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_CHOICEMEDIATRACKSTOSTREAM_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return errChoiceMediaTracks;
	}

	if (*pbVideoTrackFoundInSDP && bUseOfHintingTrackIfExist)
	{
		if ((errHintTrack = pmfFile ->
			getHintTrackInfoFromMediaTrackInfo (
			*pmtiVideoMediaTrackInfo,
			pmtiVideoHintTrackInfo)) != errNoError)
		{
			if ((long) errHintTrack == MP4F_MP4FILE_HINTTRACKNOTFOUND)
			{
				{
					Message msg = StreamingServerMessages (__FILE__, __LINE__,
						SS_RTSPSESSION_VIDEOHINTTRACKNOTFOUND,
						1, _ulIdentifier);
					_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6,
						(const char *) msg, __FILE__, __LINE__);
				}

				if (bBuildOfHintingTrackIfNotExist)
				{
					{
						Message msg = StreamingServerMessages (
							__FILE__, __LINE__,
							SS_RTSPSESSION_CREATEVIDEOHINTTRACK);
						_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6,
							(const char *) msg, __FILE__, __LINE__);
					}

					if (RTPUtility:: createHintTrack (*pmaRootAtom,
						pvMP4TracksInfo,
						*pmtiVideoMediaTrackInfo, ulMaxPayloadSizeInBytes,
						false, pmtiVideoHintTrackInfo, _ptSystemTracer) !=
						errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPUTILITY_CREATEHINTTRACK_FAILED, 1, "video");
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return err;
					}
				}
			}
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILE_GETHINTTRACKINFOFROMMEDIATRACKINFO_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
	}

	if (*pbAudioTrackFoundInSDP && bUseOfHintingTrackIfExist)
	{
		if ((errHintTrack = pmfFile ->
			getHintTrackInfoFromMediaTrackInfo (
			*pmtiAudioMediaTrackInfo,
			pmtiAudioHintTrackInfo)) != errNoError)
		{
			if ((long) errHintTrack == MP4F_MP4FILE_HINTTRACKNOTFOUND)
			{
				{
					Message msg = StreamingServerMessages (__FILE__, __LINE__,
						SS_RTSPSESSION_AUDIOHINTTRACKNOTFOUND,
						1, _ulIdentifier);
					_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6,
						(const char *) msg, __FILE__, __LINE__);
				}

				if (bBuildOfHintingTrackIfNotExist)
				{
					{
						Message msg = StreamingServerMessages (
							__FILE__, __LINE__,
							SS_RTSPSESSION_CREATEAUDIOHINTTRACK);
						_ptSystemTracer -> trace (Tracer:: TRACER_LDBG6,
							(const char *) msg, __FILE__, __LINE__);
					}

					if (RTPUtility:: createHintTrack (*pmaRootAtom,
						pvMP4TracksInfo,
						*pmtiAudioMediaTrackInfo, ulMaxPayloadSizeInBytes,
						false, pmtiAudioHintTrackInfo, _ptSystemTracer) !=
						errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPUTILITY_CREATEHINTTRACK_FAILED, 1, "audio");
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return err;
					}
				}
			}
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILE_GETHINTTRACKINFOFROMMEDIATRACKINFO_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
	}


	return errNoError;
}


Error RTSPSession:: choiceMediaTracksToStream (MP4File_p pmfFile,
	Boolean_p pbVideoChosen, MP4TrackInfo_p *pmtiVideoMediaTrackInfo,
	Boolean_p pbAudioChosen, MP4TrackInfo_p *pmtiAudioMediaTrackInfo)

{

	std:: vector<MP4TrackInfo_p>		*pvMP4TracksInfo;
	Boolean_t					bVideoMediaTrackFound;
	Boolean_t					bAudioMediaTrackFound;


	bVideoMediaTrackFound		= false;
	bAudioMediaTrackFound		= false;
	*pbVideoChosen				= false;
	*pbAudioChosen				= false;

	if (pmfFile -> getTracksInfo (&pvMP4TracksInfo) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_GETTRACKSINFO_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pvMP4TracksInfo -> size () > 0)
	{
		std:: vector<MP4TrackInfo_p>:: const_iterator	it;
		MP4TrackInfo_p				pmtiMP4TrackInfo;
		MP4Atom:: MP4Codec_t		cCodec;
		char						pHandlerType [MP4F_MAXHANDLERTYPELENGTH];


		for (it = pvMP4TracksInfo -> begin ();
			it != pvMP4TracksInfo -> end (); ++it)
		{
			/* take the last track
			if (*pbVideoChosen && *pbAudioChosen)
				break;
			*/

			pmtiMP4TrackInfo				= *it;

			if (pmtiMP4TrackInfo -> getHandlerType (
				pHandlerType) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETHANDLERTYPE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pmtiMP4TrackInfo -> getCodec (
				&cCodec) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETCODEC_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (!strcmp (pHandlerType, MP4_AUDIO_TRACK_TYPE))
			{
				bAudioMediaTrackFound			= true;

				// take the last audio track
				//if (*pbAudioChosen)
				//	continue;

				if (cCodec != MP4Atom:: MP4F_CODEC_UNKNOWN)
				{
					*pbAudioChosen			= true;

					*pmtiAudioMediaTrackInfo		= pmtiMP4TrackInfo;

					// *_pmaAudioMediaTrack			=
					//	mtiMP4TrackInfo. _pmaTrakAtom;
				}
			}
			else if (!strcmp (pHandlerType, MP4_VIDEO_TRACK_TYPE))
			{
				bVideoMediaTrackFound			= true;

				// take the last video track
				//if (*pbVideoChosen)
				//	continue;

				if (cCodec != MP4Atom:: MP4F_CODEC_UNKNOWN)
				{
					*pbVideoChosen			= true;

					*pmtiVideoMediaTrackInfo		= pmtiMP4TrackInfo;

					// *_pmaVideoMediaTrack			=
					//	mtiMP4TrackInfo. _pmaTrakAtom;
				}
			}
			else
			{
			}
		}
	}

	if (!bVideoMediaTrackFound)
	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_RTSPSESSION_VIDEOMEDIATRACKNOTFOUND);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) msg, __FILE__, __LINE__);
	}

	if (!bAudioMediaTrackFound)
	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_RTSPSESSION_AUDIOMEDIATRACKNOTFOUND);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) msg, __FILE__, __LINE__);
	}

	if (!*pbVideoChosen && !*pbAudioChosen)
	{
		if (!bVideoMediaTrackFound && !bAudioMediaTrackFound)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_NOMEDIATRACKS);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_CODECSNOTSUPPORTED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else if (!*pbVideoChosen)
	{
		if (bVideoMediaTrackFound)
		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_VIDEOCODECNOTSUPPORTED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) msg, __FILE__, __LINE__);
		}
	}
	else if (!*pbAudioChosen)
	{
		if (bAudioMediaTrackFound)
		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_AUDIOCODECNOTSUPPORTED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) msg, __FILE__, __LINE__);
		}
	}
	else
		;


	return errNoError;
}


Error RTSPSession:: activeTrack (TrackInfo_p ptiTrackInfo)

{

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	ptiTrackInfo -> _bActivated			= true;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: deactiveTrack (TrackInfo_p ptiTrackInfo)

{


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	ptiTrackInfo -> _bActivated			= false;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: seek (
	double dRequestedStartTimeInSecs,
	double dMaxBackupMovieTime,
	double *pdEffectiveStartTimeInSecs)

{

	unsigned long					ulSampleIndex;
	unsigned long					ulSyncSampleIndex;
	#ifdef WIN32
		__int64						llSampleTime;
		__int64						ullSyncSampleTime;
		__int64						ullSampleDuration;
	#else
		long long					llSampleTime;
		unsigned long long			ullSyncSampleTime;
		unsigned long long			ullSampleDuration;
	#endif
	double							dSyncMovieTime;
	double							dSyncMovieTimeForAllTracks;
	Error_t							errGetSampleNumber;
	Error_t							errGetStss;
	double							dDefinitiveSeekTime;
	RTPHintSample:: RTPHintSampleStatus_t	sRTPHintSampleState;
	unsigned long					ulTrackIdentifier;
	MP4SttsAtom_p					pmaSttsAtom;
	MP4StssAtom_p					pmaStssAtom;
	unsigned long					ulMaxSampleSize;
	char							pHandlerType [MP4F_MAXHANDLERTYPELENGTH];
	unsigned long					ulMdhdTimeScale;
	RTPStreamRealTimeInfo_p			prsrtStreamRealTimeInfo;


	if (_bIsLive)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	dSyncMovieTimeForAllTracks			= dRequestedStartTimeInSecs;

	// this first loop is used to calcolate the sample time nearest to dRequestedStartTimeInSecs
	// (<= than dRequestedStartTimeInSecs) related to a sync sample for each tracks
	for (ulTrackIdentifier = 0;
		ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
		ulTrackIdentifier++)
	{
		if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _bTrackInitialized))
			continue;

		if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_pmtiTrackInfo) -> getMdhdTimeScale (&ulMdhdTimeScale) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMDHDTIMESCALE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		llSampleTime					=
			dRequestedStartTimeInSecs * ulMdhdTimeScale;
		/*
		llSampleTime					-=
			(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _ulFirstEditMediaTime);
		*/

		if (llSampleTime < 0)
			llSampleTime				= 0;

		if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_pmtiTrackInfo) -> getSttsAtom (&pmaSttsAtom, true) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTTSATOM_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_pmtiTrackInfo) -> getHandlerType (pHandlerType) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHANDLERTYPE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (!strcmp (pHandlerType, MP4_HINT_TRACK_TYPE))
			prsrtStreamRealTimeInfo		=
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _rsrtHintStreamRealTimeInfo);
		else
			prsrtStreamRealTimeInfo		=
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _rsrtMediaStreamRealTimeInfo);

		if ((errGetSampleNumber = RTPUtility:: getSampleNumberFromSampleTime (
			pmaSttsAtom, llSampleTime, &ulSampleIndex,
			prsrtStreamRealTimeInfo,
			_ptSystemTracer)) != errNoError)
		{
			if ((long) errGetSampleNumber == RTP_RTPUTILITY_MEDIATIMETOOHIGH)
				continue;
			else
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPUTILITY_GETSAMPLENUMBERFROMSAMPLETIME_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if ((errGetStss = (((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_pmtiTrackInfo) -> getStssAtom (&pmaStssAtom, true)) != errNoError)
		{
			if ((long) errGetStss == MP4F_MP4ATOM_ATOMNOTFOUND)
				pmaStssAtom		= (MP4StssAtom_p) NULL;
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSTSSATOM_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (MP4Utility:: getPreviousSyncSample (
			pmaStssAtom, ulSampleIndex, &ulSyncSampleIndex, _ptSystemTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_GETPREVIOUSSYNCSAMPLE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (ulSampleIndex == ulSyncSampleIndex)
			continue;

		// cambiare il metodo da getSampleTimeFromSampleNumber a getSampleTimeFromSampleIndex
		if (RTPUtility:: getSampleTimeFromSampleNumber (
			pmaSttsAtom, ulSyncSampleIndex, &ullSyncSampleTime,
			&ullSampleDuration,
			prsrtStreamRealTimeInfo,
			_ptSystemTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_GETSAMPLETIMEFROMSAMPLENUMBER_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		/*
		ullSyncSampleTime					+=
			((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _ulFirstEditMediaTime;
		*/
		dSyncMovieTime			= ((double) ullSyncSampleTime) /
			(double) ulMdhdTimeScale;

		if (dSyncMovieTime < dSyncMovieTimeForAllTracks)
			dSyncMovieTimeForAllTracks				= dSyncMovieTime;
	}

	if (dRequestedStartTimeInSecs - dSyncMovieTimeForAllTracks <=
		dMaxBackupMovieTime)
		dDefinitiveSeekTime					= dSyncMovieTimeForAllTracks;
	else
		dDefinitiveSeekTime					= dRequestedStartTimeInSecs;

	*pdEffectiveStartTimeInSecs				= dDefinitiveSeekTime;

	for (ulTrackIdentifier = 0;
		ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
		ulTrackIdentifier++)
	{
		if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_bTrackInitialized))
			continue;

		if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_pmtiTrackInfo) -> getMdhdTimeScale (&ulMdhdTimeScale) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMDHDTIMESCALE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		llSampleTime				=
			(dDefinitiveSeekTime * ulMdhdTimeScale);
		/*
		llSampleTime					-=
			(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _ulFirstEditMediaTime);
		*/

		if (llSampleTime < 0)
			llSampleTime				= 0;

		if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_pmtiTrackInfo) -> getSttsAtom (&pmaSttsAtom, true) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTTSATOM_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_pmtiTrackInfo) -> getHandlerType (pHandlerType) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHANDLERTYPE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (!strcmp (pHandlerType, MP4_HINT_TRACK_TYPE))
			prsrtStreamRealTimeInfo		=
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _rsrtHintStreamRealTimeInfo);
		else
			prsrtStreamRealTimeInfo		=
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _rsrtMediaStreamRealTimeInfo);

		if ((errGetSampleNumber = RTPUtility:: getSampleNumberFromSampleTime (
			pmaSttsAtom, llSampleTime, &ulSampleIndex,
			prsrtStreamRealTimeInfo,
			_ptSystemTracer)) != errNoError)
		{
			if ((long) errGetSampleNumber == RTP_RTPUTILITY_MEDIATIMETOOHIGH)
				continue;
			else
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPUTILITY_GETSAMPLENUMBERFROMSAMPLETIME_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _ulCurrentSampleIndex	=
			ulSampleIndex;

		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _ulCurrentPacketIndex	= 0;

		/*
		// the reset is necessary because if the user presses rewind, forward, etc,
		// we have a lot of pause and play RTSP commands and each time we shall
		// reset the RTP packet prefetched by the last RTP session.
		non necessario perche' i pacchetti vengono eliminati
		in handlePLAUSEmethod
		{
			if (_prpsRTPPacketsToSend -> moveRTPPacketsToVector (
				_ulIdentifier, ulTrackIdentifier,
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_vFreeRTPPackets)) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTPPACKETSTOSEND_MOVERTPPACKETSTOVECTOR_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_ulRTPPacketsNumberPrefetched)			= 0;
		}
		*/

		if (!strcmp (pHandlerType, MP4_HINT_TRACK_TYPE))
		{
			if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_rhsCurrentRTPHintSample).  getState (&sRTPHintSampleState) !=
				errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTSAMPLE_GETSTATE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (sRTPHintSampleState == RTPHintSample:: RHS_INITIALIZED)
			{
				if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_rhsCurrentRTPHintSample).  finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPHINTSAMPLE_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_pmtiTrackInfo) -> getMaxSampleSize (&ulMaxSampleSize, true) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETMAXSAMPLESIZE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_rhsCurrentRTPHintSample. init (_pmaRootAtom,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_pmtiTrackInfo,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_pucSampleBuffer, ulMaxSampleSize,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_ulCurrentSampleIndex,
				prsrtStreamRealTimeInfo,
				_ptSystemTracer) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTSAMPLE_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
	}


	return errNoError;
}


Error RTSPSession:: getCurrentPacketRTPTimeStampAndSequenceNumber (
	unsigned long ulTrackIdentifier,
	unsigned long *pulRelativeRTPPacketTransmissionTimeWithTimeScaleAndOffset,
	unsigned short *pusSequenceNumber)

{

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (!_bIsLive)
	{
		unsigned long				ulTimestampRandomOffsetUsed;
		unsigned long				ulSequenceNumberRandomOffsetUsed;
		RTPPacket_p					prpRTPPacket;

		/*
		if (_pesEventsSet -> getRTPTimeStampAndSequenceNumberOfFirstRTPPacket (
			SS_STREAMINGSERVERPROCESSOR_DESTINATION,
			_ulIdentifier, ulTrackIdentifier,
			pulRelativeRTPPacketTransmissionTimeWithTimeScaleAndOffset,
			pusSequenceNumber) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVEREVENTSSET_GETRTPTIMESTAMPANDSEQUENCENUMBEROFFIRSTRTPPACKET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		*/

		if (
			(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_vBuiltRTPPackets). begin () ==
			(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_vBuiltRTPPackets). end ()
			)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_NOPREFETCHEDRTPPACKET);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		prpRTPPacket			= *((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_vBuiltRTPPackets). begin ());

		if (prpRTPPacket -> getTimeStamp (
			pulRelativeRTPPacketTransmissionTimeWithTimeScaleAndOffset,
			&ulTimestampRandomOffsetUsed) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_GETTIMESTAMP_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		*pulRelativeRTPPacketTransmissionTimeWithTimeScaleAndOffset		+=
			ulTimestampRandomOffsetUsed;

		if (prpRTPPacket -> getSequenceNumber (
			pusSequenceNumber, &ulSequenceNumberRandomOffsetUsed) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_GETSEQUENCENUMBER_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		*pusSequenceNumber					+= ((unsigned short) ulSequenceNumberRandomOffsetUsed);
	}
	else
	{
		LiveSource_p		plsLiveSource;
		RTPPacket_p			prpRTPPacket;
		unsigned long		ulTimestampRandomOffsetUsed;
		unsigned long		ulSequenceNumberRandomOffsetUsed;


		if (_pmtLiveSources -> lock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_LOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// check if LiveSource already exist
		{
			unsigned long		ulPortToSearch;
			Socket_p			pssLiveServerSocket;
			Error_t				errSearch;


			if (_bVideoTrackFoundInSDP)
				ulPortToSearch		= _ulLiveVideoRTPPacketsServerPort;
			else
				ulPortToSearch		= _ulLiveAudioRTPPacketsServerPort;

			// findServerSocket is not very efficient
			if ((errSearch = _pspSocketsPool -> findServerSocket (
				ulPortToSearch,
				&pssLiveServerSocket, (void **) (&plsLiveSource))) !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINDSERVERSOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_pmtLiveSources -> unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		{
			Error_t				errGetLastRTPPacket;
			Boolean_t			bIsVideoSocket;


			if (ulTrackIdentifier == _ulVideoTrackIdentifier)
				bIsVideoSocket			= true;
			else if (ulTrackIdentifier == _ulAudioTrackIdentifier)
				bIsVideoSocket			= false;
			else
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_ACTIVATION_WRONG);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_pmtLiveSources -> unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errGetLastRTPPacket = plsLiveSource -> getLastRTPPacket (
				bIsVideoSocket, &prpRTPPacket)) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_LIVESOURCE_GETLASTRTPPACKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_pmtLiveSources -> unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (prpRTPPacket -> getTimeStamp (
			pulRelativeRTPPacketTransmissionTimeWithTimeScaleAndOffset,
			&ulTimestampRandomOffsetUsed) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_GETTIMESTAMP_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pmtLiveSources -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		*pulRelativeRTPPacketTransmissionTimeWithTimeScaleAndOffset		+=
			ulTimestampRandomOffsetUsed;

		if (prpRTPPacket -> getSequenceNumber (
			pusSequenceNumber, &ulSequenceNumberRandomOffsetUsed) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_GETSEQUENCENUMBER_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pmtLiveSources -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		*pusSequenceNumber					+= ((unsigned short) ulSequenceNumberRandomOffsetUsed);

		if (_pmtLiveSources -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: getSSRC (
	unsigned long ulTrackIdentifier,
	unsigned long *pulSSRC)

{


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pulSSRC					= ((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_ulServerSSRC;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: prefetchNextPackets (
	Boolean_t bIsCalledFromHandlePLAY,
	Boolean_p pbIsAPacketPrefetched)

{

	// Boolean_t			bIsLockable;
	unsigned long		ulTrackIdentifier;
	Boolean_t			bLocalIsAPacketPrefetched;
	RTSPStatus_t		rsRTSPStatus;
	char				pHandlerType [MP4F_MAXHANDLERTYPELENGTH];


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// the RTSP state must be PLAYING. I added also the READY state
	//	because this method is called also by the handlePLAYmethod to
	//	retrieve the seq. nu. and timestamp of the first RTP packet.
	if (!(rsRTSPStatus == SS_RTSP_PLAYING ||
		(rsRTSPStatus == SS_RTSP_READY && bIsCalledFromHandlePLAY)))
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "prefetchNextPackets", (long) rsRTSPStatus);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		//	(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bIsLive)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	bLocalIsAPacketPrefetched				= false;
	*pbIsAPacketPrefetched					= false;

	for (ulTrackIdentifier = 0;
		ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
		ulTrackIdentifier++)
	{
		if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_bTrackInitialized))
			continue;

		if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_pmtiTrackInfo) -> getHandlerType (pHandlerType) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHANDLERTYPE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!strcmp (pHandlerType, MP4_HINT_TRACK_TYPE))
		{
			if (prefetchNextPacketsUsingHintTrack (
				ulTrackIdentifier, bIsCalledFromHandlePLAY,
				&bLocalIsAPacketPrefetched) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_PREFETCHNEXTPACKETSUSINGHINTTRACK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (prefetchNextPacketsUsingMediaTrack (
				ulTrackIdentifier,
				bIsCalledFromHandlePLAY,
				&bLocalIsAPacketPrefetched) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_PREFETCHNEXTPACKETSUSINGMEDIATRACK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (!(*pbIsAPacketPrefetched) && bLocalIsAPacketPrefetched)
			*pbIsAPacketPrefetched					= true;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


/*
Error RTSPSession:: prefetchLivePacket (
	unsigned char *pucRTPPacket,
	unsigned long ulPacketLength,
	Boolean_t bIsVideoTrack,
	Boolean_p pbFreeRTPPacketFinished)

{

	Boolean_t					bIsLockable;
	unsigned long				ulTrackIdentifier;
	RTPPacket_p					prpRTPPacket;
	#ifdef WIN32
		__int64					llNowInMilliSecs;
	#else
		long long				llNowInMilliSecs;
	#endif
	double						dAbsoluteRTPPacketTransmissionTimeInMilliSecs;
	unsigned short				usSequenceNumber;
	unsigned long				ulSequenceNumberRandomOffsetUsed;



	// this method locks a mutex.
	// If the mutex is already locked
	//		bIsLockable will be false
	// else
	//		bIsLockable will be true
	if (_mtRTSPSession. tryLock (&bIsLockable) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (!bIsLockable)
		return errNoError;

	//if (_mtRTSPSession. lock () != errNoError)
	//{
	//	Error err = PThreadErrors (__FILE__, __LINE__,
	//		THREADLIB_PMUTEX_LOCK_FAILED);
	//	_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
	//		(const char *) err, __FILE__, __LINE__);

	//	return err;
	//}

	if (_rsRTSPStatus != SS_RTSP_PLAYING)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "", (long) rsRTSPStatus);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		//	(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bIsVideoTrack)
		ulTrackIdentifier			= _ulVideoTrackIdentifier;
	else
		ulTrackIdentifier			= _ulAudioTrackIdentifier;

	// the handlePLAY needs almost one RTP packet prefetched
	if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _bActivated))
	{
		*pbFreeRTPPacketFinished			= false;

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}

	if (
		(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vFreeRTPPackets). begin () ==
		(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vFreeRTPPackets). end ()
		)
	{
		{
			RTPPacket_p				prpRTPPacket;


			while ((((_rsRTPSession. _ptiTracks) [
				ulTrackIdentifier]). _vBuiltRTPPackets). begin () !=
				(((_rsRTPSession. _ptiTracks) [
				ulTrackIdentifier]). _vBuiltRTPPackets). end ())
			{
				prpRTPPacket			= *((((_rsRTPSession. _ptiTracks) [
					ulTrackIdentifier]). _vBuiltRTPPackets). begin ());

				(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vFreeRTPPackets).
					insert (
					(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vFreeRTPPackets). end (),
					prpRTPPacket);

				(((_rsRTPSession. _ptiTracks) [
					ulTrackIdentifier]). _vBuiltRTPPackets). erase (
					(((_rsRTPSession. _ptiTracks) [
					ulTrackIdentifier]). _vBuiltRTPPackets). begin ());

				(((_rsRTPSession. _ptiTracks) [
					ulTrackIdentifier]). _vTransmissionTimeBuiltRTPPackets). erase (
					(((_rsRTPSession. _ptiTracks) [
					ulTrackIdentifier]). _vTransmissionTimeBuiltRTPPackets). begin ());
			}

			(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulRTPPacketsNumberPrefetched)			= 0;
		}
		// Error err = MP4FileErrors (__FILE__, __LINE__,
		//	MP4F_MP4FILE_RTPPACKETNOTAVAILABLE);
		//_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		//	(const char *) err, __FILE__, __LINE__);

		// return err;
	}

	prpRTPPacket			= *((((_rsRTPSession. _ptiTracks) [
		ulTrackIdentifier]). _vFreeRTPPackets). begin ());

	if (prpRTPPacket -> setRTPPacket (
		pucRTPPacket, ulPacketLength) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_SETSEQUENCENUMBER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (prpRTPPacket -> getSequenceNumber (
		&usSequenceNumber, &ulSequenceNumberRandomOffsetUsed) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_GETSEQUENCENUMBER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (DateTime:: nowLocalInMilliSecs (
		&llNowInMilliSecs) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_DATETIME_NOWLOCALINMILLISECS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	dAbsoluteRTPPacketTransmissionTimeInMilliSecs	= llNowInMilliSecs;

	(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vBuiltRTPPackets).
		insert (
		(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vBuiltRTPPackets). end (),
		prpRTPPacket);

	(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vTransmissionTimeBuiltRTPPackets).
		insert (
		(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_vTransmissionTimeBuiltRTPPackets). end (),
		dAbsoluteRTPPacketTransmissionTimeInMilliSecs);

	(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vFreeRTPPackets). erase (
		(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vFreeRTPPackets). begin ());

	(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_ulRTPPacketsNumberPrefetched)++;

	// {
	//	#ifdef WIN32
	//		Message msg = MP4FileMessages (__FILE__, __LINE__,
	//			MP4F_MP4FILE_BUILDANDPREFETCHEDRTPPACKET,
	//			6,
	//			pTrackURI,
	//			dRelativeRTPPacketTransmissionTimeInSecs,
	//			(*pulCurrentSequenceNumber) - 1,
	//			bIsLastPacket ? "true" : "false",
	//			(unsigned long) ullRelativeRTPPacketTransmissionTimeWithTimeScale,
	//			(__int64) (*prpRTPPacket));
	//	#else
	//		Message msg = MP4FileMessages (__FILE__, __LINE__,
	//			MP4F_MP4FILE_BUILDANDPREFETCHEDRTPPACKET,
	//			6,
	//			pTrackURI,
	//			dRelativeRTPPacketTransmissionTimeInSecs,
	//			(*pulCurrentSequenceNumber) - 1,
	//			bIsLastPacket ? "true" : "false",
	//			(unsigned long) ullRelativeRTPPacketTransmissionTimeWithTimeScale,
	//			(unsigned long long) (*prpRTPPacket));
	//	#endif
	//	_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5,
	//		(const char *) msg, __FILE__, __LINE__);
	//}

	if (
		(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vFreeRTPPackets). begin () ==
		(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vFreeRTPPackets). end ()
		)
		*pbFreeRTPPacketFinished			= true;
	else
		*pbFreeRTPPacketFinished			= false;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}
*/


Error RTSPSession:: prefetchNextPacketsUsingMediaTrack (
	unsigned long ulTrackIdentifier,
	Boolean_t bIsCalledFromHandlePLAY,
	Boolean_p pbIsAPacketPrefetched)

{

	// unsigned long					ulInitialRTPPacketsNumberPrefetched;
	Error_t							errPrefetcher;
	MP4Atom:: MP4Codec_t			cCodec;
	unsigned long					ulObjectTypeIndication;
	unsigned long					ulSamplesNumber;
	unsigned long					ulMaxSampleSize;
	RTSPStatus_t					rsRTSPStatus;
	unsigned long					ulMdhdTimeScale;


	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pbIsAPacketPrefetched			= false;

	if (rsRTSPStatus == SS_RTSP_READY && bIsCalledFromHandlePLAY)
		// is called from the handlePLAYmethod just to calculate
		//	the timestamp and sequence number of the first packet
		//	in order to answer to the PLAY method
		;
	else
	{
		if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _bActivated))
			return errNoError;
	}

	/*
	ulInitialRTPPacketsNumberPrefetched					=
		(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_vBuiltRTPPackets). size ();

	if (ulInitialRTPPacketsNumberPrefetched < _ulRTPPacketsNumberToPrefetch)
	*/

	{
		if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_pmtiTrackInfo) -> getCodec (&cCodec) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETCODEC_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_pmtiTrackInfo) -> getSamplesNumber (&ulSamplesNumber, true) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSAMPLESNUMBER_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_pmtiTrackInfo) -> getMaxSampleSize (&ulMaxSampleSize, true) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMAXSAMPLESIZE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_pmtiTrackInfo) -> getMdhdTimeScale (
			&ulMdhdTimeScale) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMDHDTIMESCALE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (cCodec == MP4Atom:: MP4F_CODEC_MPEG4)
		{
			if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_pmtiTrackInfo) -> getObjectTypeIndication (
				&ulObjectTypeIndication) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETOBJECTTYPEINDICATION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			switch (ulObjectTypeIndication)
			{
				case MP4_MPEG4_VIDEO_TYPE:
					{
						// rfc3016

/*
		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _mtiTrakInfo. _pmaTrakAtom,
		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _ulCurrentSampleIndex,
		_prpsRTPPacketsToSend -> addRTPPacketToSend ????????
*/

						if ((errPrefetcher = rfc3016VisualPrefetcher (
							ulSamplesNumber,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_pucSampleBuffer,
							ulMaxSampleSize,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulPayloadNumber,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulRTPTimeScale,
							ulMdhdTimeScale,
							// ((_rsRTPSession. _ptiTracks) [
							//	ulTrackIdentifier]). _ulFirstEditRTPTime,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulServerSSRC,
							_pmaRootAtom,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_pmtiTrackInfo,
							_rsRTPSession. _ulCurrentMaxPayloadSizeInBytes,
							// _ulRTPPacketsNumberToPrefetch -
							//	ulInitialRTPPacketsNumberPrefetched,
							_ulSamplesNumberToPrefetch,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_pTrackURI,
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_vFreeRTPPackets),
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_vBuiltRTPPackets),
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_vTransmissionTimeBuiltRTPPackets),
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_rsrtMediaStreamRealTimeInfo),
							_ptSystemTracer,
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulCurrentSampleIndex),
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulCurrentPacketIndex)
							)) != errNoError)
						{
							Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RFC_RFC3016VISUALPREFETCHER_FAILED, 1,
								((_rsRTPSession. _ptiTracks) [
								ulTrackIdentifier]). _ulCurrentSampleIndex);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}
					}

					break;
				default:
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_CODECUNKNOWN);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return err;
					}
			}
		}
		else if (cCodec == MP4Atom:: MP4F_CODEC_H263)
		{
			// rfc2429

			if ((errPrefetcher = rfc2429VisualPrefetcher (
				ulSamplesNumber,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_pucSampleBuffer,
				ulMaxSampleSize,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulPayloadNumber,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulRTPTimeScale,
				ulMdhdTimeScale,
				// ((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				//	_ulFirstEditRTPTime,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulServerSSRC,
				_pmaRootAtom,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_pmtiTrackInfo,
				_rsRTPSession. _ulCurrentMaxPayloadSizeInBytes,
				// _ulRTPPacketsNumberToPrefetch -
				//	ulInitialRTPPacketsNumberPrefetched,
				_ulSamplesNumberToPrefetch,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _pTrackURI,
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_vFreeRTPPackets),
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_vBuiltRTPPackets),
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_vTransmissionTimeBuiltRTPPackets),
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_rsrtMediaStreamRealTimeInfo),
				_ptSystemTracer,
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulCurrentSampleIndex),
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulCurrentPacketIndex)
				)) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RFC_RFC2429VISUALPREFETCHER_FAILED, 1,
					((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulCurrentSampleIndex);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
		else if (cCodec == MP4Atom:: MP4F_CODEC_AAC)
		{
			if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_pmtiTrackInfo) -> getObjectTypeIndication (
				&ulObjectTypeIndication) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETOBJECTTYPEINDICATION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			// ulObjectTypeIndication is the TrackAudioType
			switch (ulObjectTypeIndication)
			{
				case MP4_MPEG4_AUDIO_TYPE:
				case MP4_MPEG2_AAC_MAIN_AUDIO_TYPE:
				case MP4_MPEG2_AAC_LC_AUDIO_TYPE:
				case MP4_MPEG2_AAC_SSR_AUDIO_TYPE:
					if (_sStandard == MP4Atom:: MP4F_3GPP)
					{
						// rfc3016
						if ((errPrefetcher = rfc3016AudioPrefetcher (
							ulSamplesNumber,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_pucSampleBuffer,
							ulMaxSampleSize,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulPayloadNumber,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulRTPTimeScale,
							ulMdhdTimeScale,
							// ((_rsRTPSession. _ptiTracks) [
							//	ulTrackIdentifier]). _ulFirstEditRTPTime,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulServerSSRC,
							_pmaRootAtom,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_pmtiTrackInfo,
							_rsRTPSession. _ulCurrentMaxPayloadSizeInBytes,
							// _ulRTPPacketsNumberToPrefetch -
							//	ulInitialRTPPacketsNumberPrefetched,
							_ulSamplesNumberToPrefetch,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_pTrackURI,
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_vFreeRTPPackets),
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_vBuiltRTPPackets),
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_vTransmissionTimeBuiltRTPPackets),
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_rsrtMediaStreamRealTimeInfo),
							_ptSystemTracer,
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulCurrentSampleIndex),
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulCurrentPacketIndex)
							)) != errNoError)
						{
							Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RFC_RFC3016VISUALPREFETCHER_FAILED, 1,
								((_rsRTPSession. _ptiTracks) [
								ulTrackIdentifier]). _ulCurrentSampleIndex);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}
					}
					else
					{
						// rfcIsma
						if ((errPrefetcher = rfcIsmaAudioPrefetcher (
							ulSamplesNumber,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulPayloadNumber,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulRTPTimeScale,
							ulMdhdTimeScale,
							// ((_rsRTPSession. _ptiTracks) [
							//	ulTrackIdentifier]). _ulFirstEditRTPTime,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulServerSSRC,
							_pmaRootAtom,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_pmtiTrackInfo,
							_rsRTPSession. _ulCurrentMaxPayloadSizeInBytes,
							// _ulRTPPacketsNumberToPrefetch -
							//	ulInitialRTPPacketsNumberPrefetched,
							_ulRTPPacketsNumberToPrefetch,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_pTrackURI,
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_vFreeRTPPackets),
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_vBuiltRTPPackets),
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_vTransmissionTimeBuiltRTPPackets),
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_rsrtMediaStreamRealTimeInfo),
							_ptSystemTracer,
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulCurrentSampleIndex),
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulCurrentPacketIndex)
							)) != errNoError)
						{
							Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RFC_RFC3016VISUALPREFETCHER_FAILED, 1,
								((_rsRTPSession. _ptiTracks) [
								ulTrackIdentifier]). _ulCurrentSampleIndex);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}
					}

					break;
				default:
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_CODECUNKNOWN);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return err;
					}
			}
		}
		else if (cCodec == MP4Atom:: MP4F_CODEC_AMRNB ||
			cCodec == MP4Atom:: MP4F_CODEC_AMRWB)
		{
			// rfc3267

			if (rfc3267AudioPrefetcher (
				ulSamplesNumber,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_pucSampleBuffer,
				ulMaxSampleSize,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulPayloadNumber,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulRTPTimeScale,
				ulMdhdTimeScale,
				// ((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				//	_ulFirstEditRTPTime,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulServerSSRC,
				_pmaRootAtom,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_pmtiTrackInfo,
				_rsRTPSession. _ulCurrentMaxPayloadSizeInBytes,
				// _ulRTPPacketsNumberToPrefetch -
				//	ulInitialRTPPacketsNumberPrefetched,
				_ulRTPPacketsNumberToPrefetch,
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _pTrackURI,
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_vFreeRTPPackets),
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_vBuiltRTPPackets),
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_vTransmissionTimeBuiltRTPPackets),
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_rsrtMediaStreamRealTimeInfo),
				_ptSystemTracer,
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulCurrentSampleIndex),
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulCurrentPacketIndex)
				) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RFC_RFC3016VISUALPREFETCHER_FAILED, 1,
					((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulCurrentSampleIndex);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
		else
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_CODECUNKNOWN);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_vBuiltRTPPackets). size () > 0)
		*pbIsAPacketPrefetched			= true;


	return errNoError;
}


Error RTSPSession:: prefetchNextPacketsUsingHintTrack (
	unsigned long ulTrackIdentifier,
	Boolean_t bIsCalledFromHandlePLAY,
	Boolean_p pbIsAPacketPrefetched)

{

	Boolean_t						bBFrame;
	Boolean_t						bPacketFound;
	unsigned long					ulPacketsNumberInCurrentSample;
	RTPHintPacket_p					prhpRTPHintPacket;
	// unsigned long					ulInitialRTPPacketsNumberPrefetched;
	Boolean_t						bPacketsFinished;
	unsigned long					ulSamplesNumber;
	unsigned long					ulMaxSampleSize;
	RTSPStatus_t		rsRTSPStatus;


	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pbIsAPacketPrefetched			= false;

	if (rsRTSPStatus == SS_RTSP_READY && bIsCalledFromHandlePLAY)
		// is called from the handlePLAYmethod just to calculate
		//	the timestamp and sequence number of the first packet
		//	in order to answer to the PLAY method
		;
	else
	{
		if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _bActivated))
			return errNoError;
	}

	/*
	ulInitialRTPPacketsNumberPrefetched					=
		(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
		_vBuiltRTPPackets). size ();

	if (ulInitialRTPPacketsNumberPrefetched >= _ulRTPPacketsNumberToPrefetch)
	{
		*pbIsAPacketPrefetched			= true;

		return errNoError;
	}
	*/
	if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _pmtiTrackInfo) ->
		getSamplesNumber (&ulSamplesNumber, true) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSAMPLESNUMBER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _pmtiTrackInfo) ->
		getMaxSampleSize (&ulMaxSampleSize, true) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETMAXSAMPLESIZE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// prhsCurrentRTPHintSample is already initialized by seek
	do
	{
		(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_ulCurrentPacketIndex)++;

		// Do we need to move to the next sample?
		// Ricordati che getPacketsNumber e'
		// ntohs (*((unsigned short *) pointerToMdat))
		if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_rhsCurrentRTPHintSample).
			getPacketsNumber (&ulPacketsNumberInCurrentSample) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTSAMPLE_GETPACKETSNUMBER_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		// check if it is necessary to load the next sample
		if (((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_ulCurrentPacketIndex > ulPacketsNumberInCurrentSample)
		{
			// if (ptiHintTrackInfo -> qosQualityOfService >=
			// kKeyFramesOnly)
			// else if (ptiHintTrackInfo -> qosQualityOfService >=
			// k90PercentPFrames)
			// else
			{
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulCurrentPacketIndex						= 1;

				bPacketsFinished			= false;

				do
				{
					if (((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
						_ulCurrentSampleIndex < ulSamplesNumber)
					{
						(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
							_ulCurrentSampleIndex)++;

						if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
							_rhsCurrentRTPHintSample). finish () != errNoError)
						{
							Error err = RTPErrors (
								__FILE__, __LINE__,
								RTP_RTPHINTSAMPLE_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
							_rhsCurrentRTPHintSample). init (
							_pmaRootAtom, ((_rsRTPSession. _ptiTracks) [
							ulTrackIdentifier]). _pmtiTrackInfo,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_pucSampleBuffer,
							ulMaxSampleSize,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
							_ulCurrentSampleIndex,
							&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_rsrtHintStreamRealTimeInfo),
							_ptSystemTracer) != errNoError)
						{
							Error err = RTPErrors (
								__FILE__, __LINE__,
								RTP_RTPHINTSAMPLE_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
							_rhsCurrentRTPHintSample). getPacketsNumber (
							&ulPacketsNumberInCurrentSample) != errNoError)
						{
							Error err = RTPErrors (
								__FILE__, __LINE__,
								RTP_RTPHINTSAMPLE_GETPACKETSNUMBER_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}
					}
					else
					{
						// no more sample available

						bPacketsFinished			= true;

						if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
							_vBuiltRTPPackets). size () > 0)
						{
							// we arrived to the last packets but we have
							// prefetched almost one packet.

							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_ulCurrentPacketIndex					=
								ulPacketsNumberInCurrentSample + 1;
						}
					}
				}
				while (!bPacketsFinished &&
					ulPacketsNumberInCurrentSample == 0);

				if (bPacketsFinished)
					break;
			}
		}

		if ((((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_rhsCurrentRTPHintSample). getHintPacket (
			((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_ulCurrentPacketIndex - 1, &prhpRTPHintPacket) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTSAMPLE_GETHINTPACKET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		// check if the packet include data that is a part of a b-frame
		if (prhpRTPHintPacket -> getBFrame (&bBFrame) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKET_GETBFRAME_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (bBFrame && ((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_qosQualityOfService >= kNoBFrames)
		{
			bPacketFound				= false;
		}
		else
		{
			RTPPacket_p			prpRTPPacket;
			double				dLastRelativeRTPPacketTransmissionTimeInSecs;


			if (!(*pbIsAPacketPrefetched))
				*pbIsAPacketPrefetched			= true;

			bPacketFound							= true;

			if (
				(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_vFreeRTPPackets). begin () ==
				(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
				_vFreeRTPPackets). end ()
				)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_RTPFREEPACKETSVECTORNOTCONSISTENCY);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			prpRTPPacket			= *((((_rsRTPSession. _ptiTracks) [
				ulTrackIdentifier]). _vFreeRTPPackets). begin ());

			if (getRTPPacket (
				&((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]),
				((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_pmtiTrackInfo,
				prhpRTPHintPacket, prpRTPPacket,
				&dLastRelativeRTPPacketTransmissionTimeInSecs,
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_rsrtMediaStreamRealTimeInfo),
				&(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_rsrtHintStreamRealTimeInfo)) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_GETPACKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
			else
			{
				unsigned short				usSequenceNumber;
				unsigned long				ulRelativeRTPPacketTransmissionTimeWithTimeScale;
				unsigned long				ulSequenceNumberRandomOffsetUsed;
				unsigned long				ulTimestampRandomOffsetUsed;
				Boolean_t					bMarkerBit;
				Boolean_t					bIsAPacketOfASyncSample;
				// double			dRelativeRTPPacketTransmissionTimeInSecs;
				MP4StssAtom_p				pmaStssAtom;
				Error_t						errGetStss;


				/*
				{
					double							dOffsetFromStartTime;


					dOffsetFromStartTime									=
						_rsRTPSession.
						_dLastRelativeRTPPacketTransmissionTimeInSecs -
						_rsRTPSession. _dRelativeRequestedStartTimeInSecs;

					dRelativeRTPPacketTransmissionTimeInMilliSecs	=
						_rsRTPSession. _dRelativeRequestedStartTimeInSecs +
						(dOffsetFromStartTime /
						_rsRTPSession. _dRequestedSpeed);

					if (dRelativeRTPPacketTransmissionTimeInMilliSecs < 0.0)
						dRelativeRTPPacketTransmissionTimeInMilliSecs	= 0.0;

					dRelativeRTPPacketTransmissionTimeInMilliSecs		=
						(dRelativeRTPPacketTransmissionTimeInMilliSecs * 1000);
				}
				*/

				if ((errGetStss = (((_rsRTPSession. _ptiTracks) [
					ulTrackIdentifier]). _pmtiTrackInfo) -> getStssAtom (
					&pmaStssAtom, true)) != errNoError)
				{
					if ((long) errGetStss == MP4F_MP4ATOM_ATOMNOTFOUND)
						pmaStssAtom		= (MP4StssAtom_p) NULL;
					else
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETSTSSATOM_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (prpRTPPacket -> reset () != errNoError)
						{
							Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RTPPACKET_RESET_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}

				if (MP4Utility:: isSyncSample (pmaStssAtom,
					((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_ulCurrentSampleIndex,
					&bIsAPacketOfASyncSample, _ptSystemTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UTILITY_ISSYNCSAMPLE_FAILED, 1,
						((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
						_ulCurrentSampleIndex);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prpRTPPacket -> reset () != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPPACKET_RESET_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_vFreeRTPPackets). erase (
					(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_vFreeRTPPackets). begin ());

				(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_vBuiltRTPPackets). insert (
					(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_vBuiltRTPPackets). end (),
					prpRTPPacket);

				(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_vTransmissionTimeBuiltRTPPackets). insert (
					(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
					_vTransmissionTimeBuiltRTPPackets). end (),
					dLastRelativeRTPPacketTransmissionTimeInSecs * 1000);

				if (prpRTPPacket -> getSequenceNumber (&usSequenceNumber,
					&ulSequenceNumberRandomOffsetUsed) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_GETSEQUENCENUMBER_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (prpRTPPacket -> getTimeStamp (
					&ulRelativeRTPPacketTransmissionTimeWithTimeScale,
					&ulTimestampRandomOffsetUsed) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_GETTIMESTAMP_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (prpRTPPacket -> getMarkerBit (&bMarkerBit) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_GETMARKERBIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				{
					#ifdef WIN32
						Message msg = MP4FileMessages (__FILE__, __LINE__,
							MP4F_MP4FILE_BUILDANDPREFETCHEDRTPPACKET,
							6,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_pTrackURI,
							dLastRelativeRTPPacketTransmissionTimeInSecs,
							(long) usSequenceNumber,
							bMarkerBit ? "true" : "false",
							ulRelativeRTPPacketTransmissionTimeWithTimeScale,
							(__int64) (*prpRTPPacket));
					#else
						Message msg = MP4FileMessages (__FILE__, __LINE__,
							MP4F_MP4FILE_BUILDANDPREFETCHEDRTPPACKET,
							6,
							((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
								_pTrackURI,
							dLastRelativeRTPPacketTransmissionTimeInSecs,
							(long) usSequenceNumber,
							bMarkerBit ? "true" : "false",
							ulRelativeRTPPacketTransmissionTimeWithTimeScale,
							(unsigned long long) (*prpRTPPacket));
					#endif
					_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5,
						(const char *) msg, __FILE__, __LINE__);
				}
			}
		}
	}
	while (!bPacketFound ||
		(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vBuiltRTPPackets).
		size () < _ulRTPPacketsNumberToPrefetch);


	return errNoError;
}


Error RTSPSession:: getRTPPacket (TrackInfo_p ptiHintTrackInfo,
	MP4TrackInfo_p pmtiTrakInfo, RTPHintPacket_p prhpRTPHintPacket,
	RTPPacket_p prpRTPPacket, double *pdRelativeRTPPacketTransmissionTimeInSecs,
	RTPStreamRealTimeInfo_p prsrtRTPMediaStreamRealTimeInfo,
	RTPStreamRealTimeInfo_p prsrtRTPHintStreamRealTimeInfo)

{

	#ifdef WIN32
		__int64								ullSampleStartTime;
		__int64								ullSampleDuration;
		__int64								ullRelativeRTPPacketTransmissionTimeWithTimeScale;
	#else
		unsigned long long					ullSampleStartTime;
		unsigned long long					ullSampleDuration;
		unsigned long long					ullRelativeRTPPacketTransmissionTimeWithTimeScale;
	#endif
	unsigned long						ulElementsNumberInDataTable;
	unsigned long						ulElementIndex;
	long								lTLVTimestampOffset;
	long								lRelativePacketTransmissionTime;
	unsigned long						ulRTPSequenceNumber;
	Boolean_t							bMarkerBit;
	unsigned long						ulPayloadType;
	RTPHintPacketData_p					prhpdRTPHintPacketData;
	MP4SttsAtom_p						pmaSttsAtom;
	unsigned long						ulMdhdTimeScale;



	if (pmtiTrakInfo -> getSttsAtom (&pmaSttsAtom, true) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSTTSATOM_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiTrakInfo -> getMdhdTimeScale (
		&ulMdhdTimeScale) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETMDHDTIMESCALE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// initialization of the ulRelativeRTPPacketTransmissionTimeInSecs
	
	if (RTPUtility:: getSampleTimeFromSampleNumber (
		pmaSttsAtom, ptiHintTrackInfo -> _ulCurrentSampleIndex,
		&ullSampleStartTime, &ullSampleDuration,
		prsrtRTPHintStreamRealTimeInfo, _ptSystemTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPUTILITY_GETSAMPLETIMEFROMSAMPLENUMBER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (ptiHintTrackInfo -> _ulRTPTimeScale == ulMdhdTimeScale)
		ullRelativeRTPPacketTransmissionTimeWithTimeScale	= ullSampleStartTime;
	else
		ullRelativeRTPPacketTransmissionTimeWithTimeScale	= ullSampleStartTime *
			(((double) ptiHintTrackInfo -> _ulRTPTimeScale) /
			((double) ulMdhdTimeScale));

	// ullRelativeRTPPacketTransmissionTimeWithTimeScale		+= ptiHintTrackInfo -> _ulFirstEditRTPTime;

	if (prhpRTPHintPacket -> getTLVTimestampOffset (
		&lTLVTimestampOffset) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_GETTLVTIMESTAMPOFFSET_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	ullRelativeRTPPacketTransmissionTimeWithTimeScale		+= lTLVTimestampOffset;

	// Calculation of the current packet transmission time in secs
	// The next line is commented because we want that the packets are
	// sent now without considering a first edit media time
	// ullSampleStartTime					+= ptiHintTrackInfo -> _ulFirstEditMediaTime;

	if (prhpRTPHintPacket -> getRelativePacketTransmissionTime (
		&lRelativePacketTransmissionTime) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_GETRELATIVEPACKETTRANSMISSIONTIME_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pdRelativeRTPPacketTransmissionTimeInSecs			=
		((double) ullSampleStartTime /
		((double) ulMdhdTimeScale)) +
		((double) lRelativePacketTransmissionTime /
		((double) ulMdhdTimeScale));


	// BUILD THE RTP PACKET TO SEND
	if (prpRTPPacket -> setVersion (2) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_SETVERSION_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (prhpRTPHintPacket -> getMarkerBit (&bMarkerBit) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_GETMARKERBIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (prpRTPPacket -> setMarkerBit (bMarkerBit) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_SETMARKERBIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (prhpRTPHintPacket -> getPayloadType (&ulPayloadType) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_GETPAYLOADTYPE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (prpRTPPacket -> setPayloadType (
		(unsigned char) ulPayloadType) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_SETPAYLOADTYPE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (prhpRTPHintPacket -> getSequenceNumber (&ulRTPSequenceNumber) !=
		errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_GETSEQUENCENUMBER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (prpRTPPacket -> setSequenceNumber (
		(unsigned short) ulRTPSequenceNumber) !=
		errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_SETSEQUENCENUMBER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (prpRTPPacket -> setTimeStamp ((unsigned long)
		ullRelativeRTPPacketTransmissionTimeWithTimeScale) !=
		errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_SETTIMESTAMP_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (prpRTPPacket -> setServerSSRC (
		ptiHintTrackInfo -> _ulServerSSRC) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_SETSERVERSSRC_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (prhpRTPHintPacket -> getElementsNumberInDataTable (
		&ulElementsNumberInDataTable) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_GETELEMENTSNUMBERINDATATABLE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	for (ulElementIndex = 0; ulElementIndex < ulElementsNumberInDataTable;
		ulElementIndex++ )
	{
		if (prhpRTPHintPacket -> getHintPacketData (ulElementIndex,
			&prhpdRTPHintPacketData) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKET_GETHINTPACKETDATA_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (prpRTPPacket -> reset () != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPPACKET_RESET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (prhpdRTPHintPacketData -> appendPayloadDataToRTPPacket (
			prpRTPPacket, &(ptiHintTrackInfo -> _vReferencesTracksInfo),
			_pmaRootAtom, pmtiTrakInfo,
			prsrtRTPMediaStreamRealTimeInfo,
			prsrtRTPHintStreamRealTimeInfo) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKETDATA_APPENDPAYLOADDATATORTPPACKET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (prpRTPPacket -> reset () != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPPACKET_RESET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}


	return errNoError;
}


/*
Error RTSPSession:: parseURL (const char *pURL,
	char *pIPAddress, long *plRTSPPort,
	Buffer_p pbRelativePath)

{

	StringTokenizer_t				stURLTokenizer;
	Error							errNextToken;
	const char						*pToken;
	const char						*pColon;


	if (stURLTokenizer. init (pURL, -1, "/") !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_STRINGTOKENIZER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errNextToken = stURLTokenizer. nextToken (&pToken)) != errNoError)
	{
		if ((long) errNextToken != TOOLS_STRINGTOKENIZER_NOMORETOKEN)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stURLTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// IP address[:port]
	if ((errNextToken = stURLTokenizer. nextToken (&pToken)) != errNoError)
	{
		if ((long) errNextToken != TOOLS_STRINGTOKENIZER_NOMORETOKEN)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stURLTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if ((pColon = strchr (pToken, ':')) != (char *) NULL)
	{
		strncpy (pIPAddress, pToken, pColon - pToken);
		pIPAddress [pColon - pToken]		= '\0';

		*plRTSPPort			= atol (pColon + 1);
	}
	else
	{
		strcpy (pIPAddress, pToken);

		*plRTSPPort			= -1;
	}

	// <relative path>
	if ((errNextToken = stURLTokenizer. nextToken (&pToken, "?")) != errNoError)
	{
		if ((long) errNextToken != TOOLS_STRINGTOKENIZER_NOMORETOKEN)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stURLTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (pbRelativePath -> setBuffer (pToken) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (stURLTokenizer. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_STRINGTOKENIZER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}
*/


Error RTSPSession:: parseURLParameters (
	Boolean_t bIsLive, Buffer_p pbURLParameters,

	Boolean_p pbStartTimeInSecsParameter, long *plStartTimeInSecsParameter,

	StreamingSessionTimes_p pssStreamingSessionTimes,
	Boolean_p pbStreamingSessionTimeoutActive,
	Scheduler_p pscScheduler)

{

	StringTokenizer_t		stURLParametersTokenizer;
	Error					errNextToken;
	const char				*pToken;
	const char				*pEqual;
	char					pFieldName [
		SS_RTSPSESSION_MAXURLPARAMETERLENGTH];
	Boolean_t				bParametersFinished;


	#ifdef TRIAL_VERSION
		if (strcmp ((const char *) (*pbURLParameters), ""))
		{
			if (pbURLParameters -> append ("&") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (pbURLParameters -> append (
			SS_RTSPSESSION_URLPARAMETER_STREAMINGSESSIONTIMEOUT) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		// 5 min.
		if (pbURLParameters -> append ("=300") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
		/*
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) "\n", __FILE__, __LINE__);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) (*pbURLParameters), __FILE__, __LINE__);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) "\n", __FILE__, __LINE__);
		*/
	#endif


	*pbStartTimeInSecsParameter			= false;
	*pbStreamingSessionTimeoutActive	= false;

	if (stURLParametersTokenizer. init (
		(const char *) (*pbURLParameters), -1, "&") !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_STRINGTOKENIZER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	bParametersFinished				= false;

	while (!bParametersFinished)
	{
		if ((errNextToken = stURLParametersTokenizer. nextToken (
			&pToken)) != errNoError)
		{
			if ((long) errNextToken == TOOLS_STRINGTOKENIZER_NOMORETOKEN)
			{
				bParametersFinished				= true;

				continue;
			}
			else
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stURLParametersTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if ((pEqual = strchr (pToken, '=')) == (char *) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_URLPARAMETERWRONG,
				1, pToken);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stURLParametersTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		strncpy (pFieldName, pToken, pEqual - pToken);
		pFieldName [pEqual - pToken]		= '\0';

		if (!strcmp (pFieldName, SS_RTSPSESSION_URLPARAMETER_STARTTIME))
		{
			if (bIsLive)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_URLPARAMETERNOTAPPLICABLETOLIVE,
					1, pFieldName);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				continue;
			}

			*plStartTimeInSecsParameter			= atol (pEqual + 1);
			*pbStartTimeInSecsParameter			= true;
		}
		else if (!strcmp (pFieldName,
			SS_RTSPSESSION_URLPARAMETER_STREAMINGSESSIONTIMEOUT))
		{
			unsigned long			ulStreamingTimeoutInSecs;


			ulStreamingTimeoutInSecs			= atol (pEqual + 1);

			#ifdef TRIAL_VERSION
				if (*pbStreamingSessionTimeoutActive)
				{
					if (pscScheduler -> deactiveTimes (
						pssStreamingSessionTimes) != errNoError)
					{
						Error err = SchedulerErrors (__FILE__, __LINE__,
							SCH_SCHEDULER_DEACTIVETIMES_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err,
							__FILE__, __LINE__);

						if (pssStreamingSessionTimes -> finish () !=
							errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_STREAMINGSESSIONTIMES_FINISH_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						continue;
					}

					if (pssStreamingSessionTimes -> finish () !=
						errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_STREAMINGSESSIONTIMES_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						continue;
					}
				}
			#endif

			if (pssStreamingSessionTimes -> init (
				ulStreamingTimeoutInSecs * 1000,
				_ulIdentifier, _pesEventsSet, _ptSystemTracer) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSESSIONTIMES_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				continue;
			}

			if (pscScheduler -> activeTimes (pssStreamingSessionTimes) !=
				errNoError)
			{
				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_SCHEDULER_ACTIVETIMES_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err,
					__FILE__, __LINE__);

				if (pssStreamingSessionTimes -> finish () != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSESSIONTIMES_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				continue;
			}

			if (pssStreamingSessionTimes -> start () != errNoError)
			{
				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_TIMES_START_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pscScheduler -> deactiveTimes (pssStreamingSessionTimes) !=
					errNoError)
				{
					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_SCHEDULER_DEACTIVETIMES_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err,
						__FILE__, __LINE__);
				}

				if (pssStreamingSessionTimes -> finish () != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSESSIONTIMES_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				continue;
			}

			*pbStreamingSessionTimeoutActive			= true;
		}
		else
		{
			// no action for URL parameters not handled by the server
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_URLPARAMETERUNKNOWN,
				1, pFieldName);
			_ptSystemTracer -> trace (Tracer:: TRACER_LWRNG,
				(const char *) err, __FILE__, __LINE__);
		}
	}

	if (stURLParametersTokenizer. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_STRINGTOKENIZER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: initRTPSession (void)

{

	unsigned long			ulTrackIdentifier;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	#ifdef RTPOVERBUFFERWINDOW
		if ((_rsRTPSession. _robwRTPOverBufferWindow). init (
			_ulSendingInterval, 0,
			_ulMaxSendAheadTimeInSec, _ptSystemTracer) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTPOVERBUFFERWINDOW_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	#elif RTPOVERBUFFERWINDOW2
		(_rsRTPSession. _robwRTPOverBufferWindow2).
			resetOverBufferWindow ();
	#else
	#endif

	// non penso serve questo loop perchè i valori sono inizializzati
	// da addTrack
	for (ulTrackIdentifier = 0;
		ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
		ulTrackIdentifier++)
	{
		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_ullRTPPacketBytesSent			= 0;

		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_ulLostRTPPacketsNumber		= 0;

		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_ulSentRTPPacketsNumber		= 0;

		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_ulRTCPPacketsNumberReceived	= 0;

		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_ullRTCPBytesReceived			= 0;

		((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]).
			_ulJitter						= 0;
	}

	_rsRTPSession. _dRelativeTransmissionTimeInSecsOfLastSentRTPPacket		=
		0.0;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: finishRTPSession (void)

{

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	#ifdef RTPOVERBUFFERWINDOW
		if ((_rsRTPSession. _robwRTPOverBufferWindow). finish () !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTPOVERBUFFERWINDOW_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	#elif RTPOVERBUFFERWINDOW2
	#else
	#endif

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: addTrackToStream (Boolean_t bIsLive,
	Boolean_t bIsVideoSetup,
	MP4Atom:: MP4Codec_t cLiveCodecUsed,
	const char *pTrackURI,
	MP4TrackInfo_p pmtiTrackInfo,
	std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo,
	Boolean_t bIsHintTrack,	unsigned long ulPayloadNumber,
	unsigned long ulRTPClientPort, unsigned long ulRTPServerPort,
	unsigned long ulRTCPClientPort, unsigned long ulRTCPServerPort,
	unsigned long *pulTrackIdentifier)

{

	TrackInfo_p					ptiTrackInfo;
	Error_t						errSocketInit;
	// SocketImpl_p				pClientSocketImpl;
	// MP4ElstAtom_p				pmaElstAtom;
	// MP4MdhdAtom_p				pmaMdhdAtom;
	unsigned long				ulEntriesNumber;
	unsigned long				ulEntryIndex;
	// unsigned long				ulSampleTime;
	// long						lSampleTime;
	MP4MvhdAtom_p				pmmaMvhdAtom;
	unsigned long				ulMvhdTimeScale;
	// MP4RtpAtom_p				pmraRtpAtom;
	MP4TsroAtom_p				pmaTsroAtom;
	MP4SnroAtom_p				pmaSnroAtom;
	MP4HintAtom_p				pmaHintAtom;
	unsigned long				ulTrackRefenrence;
	MP4Atom_p					pmaAtom;


	{
		for (*pulTrackIdentifier = 0;
			*pulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
			(*pulTrackIdentifier)++)
		{
			if (!(((_rsRTPSession. _ptiTracks) [*pulTrackIdentifier]). _bTrackInitialized))
				break;
		}

		if (*pulTrackIdentifier >= SS_RTSPSESSION_MAXTRACKSNUMBER)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_TRACKSNOMOREAVAILABLE);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		ptiTrackInfo				=
			&((_rsRTPSession. _ptiTracks) [*pulTrackIdentifier]);
	}

	strcpy (ptiTrackInfo -> _pTrackURI, pTrackURI);

	if (deactiveTrack (ptiTrackInfo) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_DEACTIVETRACK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (!bIsLive)
	{
		ptiTrackInfo -> _pmtiTrackInfo	= pmtiTrackInfo;
		/*
		if (_pmfFile -> getTrakInfoFromTrackPointer (
			pmaTrakAtom, &(ptiTrackInfo -> _mtiTrakInfo)) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_GETTRAKINFOFROMTRACKPOINTER_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
		*/
	}
	else
	{
		/*
		if (bIsVideoSetup)
			strcpy ((ptiTrackInfo -> _mtiTrakInfo). _pHandlerType,
				"vide");
		else
			strcpy ((ptiTrackInfo -> _mtiTrakInfo). _pHandlerType,
				"soun");
		(ptiTrackInfo -> _mtiTrakInfo). _cCodecUsed		=
			cLiveCodecUsed;
		*/
	}

	ptiTrackInfo -> _ulRTPClientPort			= ulRTPClientPort;
	ptiTrackInfo -> _ulRTPServerPort			= ulRTPServerPort;
	ptiTrackInfo -> _ulRTCPClientPort			= ulRTCPClientPort;
	ptiTrackInfo -> _ulRTCPServerPort			= ulRTCPServerPort;

	if ((errSocketInit = (ptiTrackInfo -> _csRTPClientSocket). init (
		SocketImpl:: DGRAM, 0, 0, 0, 0, 0, true, _pLocalIPAddressForRTP)) !=
		errNoError)
	{
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errSocketInit, __FILE__, __LINE__);
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return errSocketInit;
	}

	/*
	if (((*ptiTrackInfo) -> _csRTPClientSocket).
		getSocketImpl (&pClientSocketImpl) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKET_GETSOCKETIMPL_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete (*ptiTrackInfo);
		(*ptiTrackInfo)				= (RTPSession:: TrackInfo_p) NULL;

		return err;
	}

	// Set the size of the RTSPSession's send buffer to an appropriate max size
	// based on the bitrate of the movie. This has 2 benefits:
	// 1) Each socket normally defaults to 32 K. A smaller buffer prevents the
	// system from getting buffer starved if lots of clients get flow-controlled
	// 2) We may need to scale up buffer sizes for high-bandwidth movies
	// in order to maximize thruput, and we may need to scale down buffer
	// sizes for low-bandwidth movies to prevent us from buffering lots
	// of data that the client can't use
	// If we don't know any better, assume maximum buffer size.
	// see RTPSession.cpp 317
	if (pClientSocketImpl -> setMaxSendBuffer (32768) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete (*ptiTrackInfo);
		(*ptiTrackInfo)				= (RTPSession:: TrackInfo_p) NULL;

		return err;
	}

	if (pClientSocketImpl -> setMaxReceiveBuffer (32768) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete (*ptiTrackInfo);
		(*ptiTrackInfo)				= (RTPSession:: TrackInfo_p) NULL;

		return err;
	}
	*/

	if ((errSocketInit = (ptiTrackInfo -> _csRTCPClientSocket). init (
		SocketImpl:: DGRAM, 0, 0, 0, 0, 0, true, _pLocalIPAddressForRTCP)) !=
		errNoError)
	{
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errSocketInit, __FILE__, __LINE__);
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if ((ptiTrackInfo -> _csRTPClientSocket). finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errSocketInit;
	}

	/*
	if ((errSocketInit = ((*ptiTrackInfo) -> _ssRTPServerSocket). init (
		(*ptiTrackInfo) -> _pLocalIPAddressForRTP,
		(*ptiTrackInfo) -> _ulRTPServerPort, true,
		SocketImpl:: DGRAM)) != errNoError)
	{
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errSocketInit, __FILE__, __LINE__);
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (((*ptiTrackInfo) -> _csRTCPClientSocket). finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (((*ptiTrackInfo) -> _csRTPClientSocket). finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete (*ptiTrackInfo);
		(*ptiTrackInfo)				=
			(RTSPSession:: RTPSession:: TrackInfo_p) NULL;


		return err;
	}
	*/

	if ((errSocketInit = (ptiTrackInfo -> _ssRTCPServerSocket). init (
		_pLocalIPAddressForRTCP, ptiTrackInfo -> _ulRTCPServerPort,
		true, SocketImpl:: DGRAM, 0, 0)) != errNoError)
	{
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errSocketInit, __FILE__, __LINE__);
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		/*
		if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}
		*/

		if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if ((ptiTrackInfo -> _csRTPClientSocket). finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errSocketInit;
	}

	if (!bIsLive)
	{
		Error_t					errGet;


		// _ulFirstEditMovieTime
		/*
		if ((errGet = (ptiTrackInfo -> _pmtiTrackInfo) ->
			getElstAtom (&pmaElstAtom)) != errNoError)
		{
			if ((long) errGet == MP4F_MP4ATOM_SEARCHATOM_FAILED)
			{
				ptiTrackInfo -> _ulFirstEditMovieTime		= 0;
				pmaElstAtom									= (MP4ElstAtom_p) NULL;
			}
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETELSTATOM_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (pmaElstAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			for (ulEntryIndex = 0; ulEntryIndex < ulEntriesNumber;
				ulEntryIndex += 3)
			{
				if (pmaElstAtom -> getEditListTable (ulEntryIndex + 1,
					&ulSampleTime) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				lSampleTime				= ulSampleTime;

				if (lSampleTime == -1)
				{
					// segmentDuration
					if (pmaElstAtom -> getEditListTable (ulEntryIndex,
						&(ptiTrackInfo -> _ulFirstEditMovieTime)) !=
						errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4PROPERTY_GETVALUE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if ((ptiTrackInfo -> _ssRTCPServerSocket).
							finish () != errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_SERVERSOCKET_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if ((ptiTrackInfo -> _csRTCPClientSocket).
							finish () != errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_CLIENTSOCKET_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
							errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_CLIENTSOCKET_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
			}
		}
		*/

		/*
		if (((ptiTrackInfo -> _mtiTrakInfo). _pmaTrakAtom) ->
			searchAtom ("edts:0:elst:0",
			false, &pmaAtom) != errNoError)
		{
			ptiTrackInfo -> _ulFirstEditMovieTime		= 0;
			pmeaElstAtom									= (MP4ElstAtom_p) NULL;
		}
		else
		{
			pmeaElstAtom		= (MP4ElstAtom_p) pmaAtom;

			if (pmeaElstAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			for (ulEntryIndex = 0; ulEntryIndex < ulEntriesNumber;
				ulEntryIndex += 3)
			{
				if (pmeaElstAtom -> getEditListTable (ulEntryIndex + 1,
					&ulSampleTime) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				lSampleTime				= ulSampleTime;

				if (lSampleTime == -1)
				{
					// segmentDuration
					if (pmeaElstAtom -> getEditListTable (ulEntryIndex,
						&(ptiTrackInfo -> _ulFirstEditMovieTime)) !=
						errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4PROPERTY_GETVALUE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if ((ptiTrackInfo -> _ssRTCPServerSocket).
							finish () != errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_SERVERSOCKET_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if ((ptiTrackInfo -> _csRTCPClientSocket).
							finish () != errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_CLIENTSOCKET_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
							errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_CLIENTSOCKET_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
			}
		}
		*/

		// ...
		if (_pmfFile -> searchAtom ("moov:0:mvhd:0",
			true, &pmaAtom) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_ATOMNOTEXIST, 1, "moov:0:mvhd:0");
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			/*
			if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
			*/

			if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((ptiTrackInfo -> _csRTPClientSocket). finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			pmmaMvhdAtom		= (MP4MvhdAtom_p) pmaAtom;

		if (pmmaMvhdAtom -> getTimeScale (&ulMvhdTimeScale) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			/*
			if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
			*/

			if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((ptiTrackInfo -> _csRTPClientSocket). finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		/*
		if (((ptiTrackInfo -> _mtiTrakInfo). _pmaTrakAtom) ->
			searchAtom ("mdia:0:mdhd:0",
			true, &pmaAtom) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_ATOMNOTEXIST, 1, "mdia:0:mdhd:0");
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((ptiTrackInfo -> _csRTPClientSocket). finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			ptiTrackInfo -> _pmmaMdhdAtom	= (MP4MdhdAtom_p) pmaAtom;

		if ((ptiTrackInfo -> _pmmaMdhdAtom) -> getTimeScale (
			&(ptiTrackInfo -> _ulMdhdTimeScale)) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((ptiTrackInfo -> _csRTPClientSocket). finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		*/

		/*
		if (ulMvhdTimeScale > 0)
			ptiTrackInfo -> _ulFirstEditMediaTime		=
				(ptiTrackInfo -> _ulFirstEditMovieTime / ulMvhdTimeScale) *
				ptiTrackInfo -> _ulMdhdTimeScale;
		else
		{
			// error ???
			ptiTrackInfo -> _ulFirstEditMediaTime		=
				ptiTrackInfo -> _ulFirstEditMovieTime *
				ptiTrackInfo -> _ulMdhdTimeScale;
		}
		*/
	}

	// ulPayloadNumber
	ptiTrackInfo -> _ulPayloadNumber		= ulPayloadNumber;

	if (!bIsLive)
	{
		unsigned long			ulMaxSampleSize;
		Error_t					errGet;

		// _ulSamplesNumber and _ulMaxSampleSize
		/*
		if (MP4Utility:: getMaxSampleSize (
			(ptiTrackInfo -> _mtiTrakInfo). _pmaTrakAtom,
			&(ptiTrackInfo -> _ulMaxSampleSize),
			&(ptiTrackInfo -> _ulSamplesNumber), _ptSystemTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_GETMAXSAMPLESIZE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((ptiTrackInfo -> _csRTPClientSocket). finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		*/

		if ((ptiTrackInfo -> _pmtiTrackInfo) -> getMaxSampleSize (
			&ulMaxSampleSize, true) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMAXSAMPLESIZE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((ptiTrackInfo -> _csRTPClientSocket). finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if ((ptiTrackInfo -> _pucSampleBuffer = new unsigned char [
			ulMaxSampleSize]) == (unsigned char *) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_NEW_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			/*
			if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
			*/

			if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((ptiTrackInfo -> _csRTPClientSocket). finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// _ulRTPTimeScale
		// the tims atom is the right place to get the RTP time scale.
		// If it does not exist the hint time scale must be calculated.
		if ((errGet = (ptiTrackInfo -> _pmtiTrackInfo) ->
			getTimsTimeScale (&(ptiTrackInfo -> _ulRTPTimeScale), true)) !=
			errNoError)
		{
			if ((long) errGet == MP4F_MP4ATOM_ATOMNOTFOUND)
			{
				MP4TrakAtom_p				pmaTrakAtom;
				MP4HdlrAtom_p				pmaHdlrAtom;
				MP4MdhdAtom_p				pmaMdhdAtom;
				MP4Atom:: MP4Codec_t		cCodec;
				unsigned long				ulObjectTypeIndication;


				if ((ptiTrackInfo -> _pmtiTrackInfo) -> getTrakAtom (
					&pmaTrakAtom) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] ptiTrackInfo -> _pucSampleBuffer;
					ptiTrackInfo -> _pucSampleBuffer		=
						(unsigned char *) NULL;

					if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					/*
					if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
					*/

					if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if ((ptiTrackInfo -> _pmtiTrackInfo) -> getHdlrAtom (
					&pmaHdlrAtom) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETHDLRATOM_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] ptiTrackInfo -> _pucSampleBuffer;
					ptiTrackInfo -> _pucSampleBuffer		=
						(unsigned char *) NULL;

					if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					/*
					if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
					*/

					if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if ((ptiTrackInfo -> _pmtiTrackInfo) -> getMdhdAtom (
					&pmaMdhdAtom) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETMDHDATOM_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] ptiTrackInfo -> _pucSampleBuffer;
					ptiTrackInfo -> _pucSampleBuffer		=
						(unsigned char *) NULL;

					if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					/*
					if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
					*/

					if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if ((ptiTrackInfo -> _pmtiTrackInfo) -> getCodec (
					&cCodec) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETCODEC_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] ptiTrackInfo -> _pucSampleBuffer;
					ptiTrackInfo -> _pucSampleBuffer		=
						(unsigned char *) NULL;

					if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					/*
					if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
					*/

					if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (cCodec == MP4Atom:: MP4F_CODEC_MPEG4 ||
					cCodec == MP4Atom:: MP4F_CODEC_AAC)	// ISO codecs
				{
					if ((ptiTrackInfo -> _pmtiTrackInfo) ->
						getObjectTypeIndication (
						&ulObjectTypeIndication) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETOBJECTTYPEINDICATION_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] ptiTrackInfo -> _pucSampleBuffer;
						ptiTrackInfo -> _pucSampleBuffer		=
							(unsigned char *) NULL;

						if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
							errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_SERVERSOCKET_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						/*
						if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
							errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_SERVERSOCKET_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}
						*/

						if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
							errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_CLIENTSOCKET_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
							errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_CLIENTSOCKET_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
				else
				{
					ulObjectTypeIndication		= (unsigned long) -1;
				}

				if (RTPUtility:: getRTPTimeScale (
					pmaTrakAtom, pmaMdhdAtom,
					cCodec, ulObjectTypeIndication,
					_sStandard, &(ptiTrackInfo -> _ulRTPTimeScale),
					_ptSystemTracer) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPUTILITY_GETRTPTIMESCALE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] ptiTrackInfo -> _pucSampleBuffer;
					ptiTrackInfo -> _pucSampleBuffer		=
						(unsigned char *) NULL;

					if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					/*
					if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
					*/

					if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTIMSTIMESCALE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] ptiTrackInfo -> _pucSampleBuffer;
				ptiTrackInfo -> _pucSampleBuffer		=
					(unsigned char *) NULL;

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				/*
				if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
				*/

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		/*
		if (((ptiTrackInfo -> _mtiTrakInfo). _pmaTrakAtom) ->
			searchAtom (
			"mdia:0:minf:0:stbl:0:stsd:0:rtp :0:tims:0", false,
			&pmaAtom) != errNoError)
		{
			if (MP4Utility:: getHintTimeScale (
				(ptiTrackInfo -> _mtiTrakInfo). _pmaTrakAtom,
				(ptiTrackInfo -> _mtiTrakInfo). _cCodecUsed,
				(ptiTrackInfo -> _mtiTrakInfo). _ulObjectTypeIndication,
				_sStandard, &(ptiTrackInfo -> _ulHintTimeScale),
				_ptSystemTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETHINTTIMESCALE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] ptiTrackInfo -> _pucSampleBuffer;
				ptiTrackInfo -> _pucSampleBuffer		=
					(unsigned char *) NULL;

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			pmraTimsAtom		= (MP4TimsAtom_p) pmaAtom;

			if (pmraTimsAtom -> getTimeScale (
				&(ptiTrackInfo -> _ulHintTimeScale)) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] ptiTrackInfo -> _pucSampleBuffer;
				ptiTrackInfo -> _pucSampleBuffer		=
					(unsigned char *) NULL;

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		*/
	}

	if (!bIsLive)
	{
		// ulRtpTimestampRandomOffset
		Error_t					errGet;


		// _ulRtpTimestampRandomOffset
		if ((errGet = (ptiTrackInfo -> _pmtiTrackInfo) ->
			getTsroAtom (&pmaTsroAtom, true)) != errNoError)
		{
			if ((long) errGet == MP4F_MP4ATOM_ATOMNOTFOUND)
			{
				if (RTPUtility:: getULongRandom (
					&(ptiTrackInfo -> _ulRtpTimestampRandomOffset)) !=
					errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPUTILITY_GETULONGRANDOM_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] ptiTrackInfo -> _pucSampleBuffer;
					ptiTrackInfo -> _pucSampleBuffer		=
						(unsigned char *) NULL;

					if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					/*
					if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
					*/

					if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTSROATOM_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] ptiTrackInfo -> _pucSampleBuffer;
				ptiTrackInfo -> _pucSampleBuffer		=
					(unsigned char *) NULL;

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				/*
				if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
				*/

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (pmaTsroAtom -> getOffset (
				&(ptiTrackInfo -> _ulRtpTimestampRandomOffset)) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] ptiTrackInfo -> _pucSampleBuffer;
				ptiTrackInfo -> _pucSampleBuffer		=
					(unsigned char *) NULL;

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				/*
				if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
				*/

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		/*
		if (((ptiTrackInfo -> _mtiTrakInfo). _pmaTrakAtom) ->
			searchAtom (
			"mdia:0:minf:0:stbl:0:stsd:0:rtp :0:tsro:0", false,
			&pmaAtom) != errNoError)
		{
			if (MP4Utility:: getULongRandom (
				&(ptiTrackInfo -> _ulRtpTimestampRandomOffset)) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETULONGRANDOM_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] ptiTrackInfo -> _pucSampleBuffer;
				ptiTrackInfo -> _pucSampleBuffer		=
					(unsigned char *) NULL;

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			pmraTsroAtom		= (MP4TsroAtom_p) pmaAtom;

			if (pmraTsroAtom -> getOffset (
				&(ptiTrackInfo -> _ulRtpTimestampRandomOffset)) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] ptiTrackInfo -> _pucSampleBuffer;
				ptiTrackInfo -> _pucSampleBuffer		=
					(unsigned char *) NULL;

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		*/

		// ulRtpSequenceNumberRandomOffset
		if ((errGet = (ptiTrackInfo -> _pmtiTrackInfo) ->
			getSnroAtom (&pmaSnroAtom, true)) != errNoError)
		{
			if ((long) errGet == MP4F_MP4ATOM_ATOMNOTFOUND)
			{
				if (RTPUtility:: getUShortRandom (
					&(ptiTrackInfo -> _usRtpSequenceNumberRandomOffset)) !=
					errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPUTILITY_GETULONGRANDOM_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] ptiTrackInfo -> _pucSampleBuffer;
					ptiTrackInfo -> _pucSampleBuffer		=
						(unsigned char *) NULL;

					if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					/*
					if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
					*/

					if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSNROATOM_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] ptiTrackInfo -> _pucSampleBuffer;
				ptiTrackInfo -> _pucSampleBuffer		=
					(unsigned char *) NULL;

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				/*
				if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
				*/

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			unsigned long				ulRtpSequenceNumberRandomOffset;


			if (pmaSnroAtom -> getOffset (
				&ulRtpSequenceNumberRandomOffset) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] ptiTrackInfo -> _pucSampleBuffer;
				ptiTrackInfo -> _pucSampleBuffer		=
					(unsigned char *) NULL;

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				/*
				if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
				*/

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			ptiTrackInfo -> _usRtpSequenceNumberRandomOffset			=
				(unsigned short) ulRtpSequenceNumberRandomOffset;
		}

		/*
		if (((ptiTrackInfo -> _mtiTrakInfo). _pmaTrakAtom) ->
			searchAtom (
			"mdia:0:minf:0:stbl:0:stsd:0:rtp :0:snro:0", false,
			&pmaAtom) != errNoError)
		{
			if (MP4Utility:: getUShortRandom (
				&(ptiTrackInfo -> _usRtpSequenceNumberRandomOffset)) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETULONGRANDOM_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] ptiTrackInfo -> _pucSampleBuffer;
				ptiTrackInfo -> _pucSampleBuffer		=
					(unsigned char *) NULL;

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			unsigned long				ulRtpSequenceNumberRandomOffset;


			pmraSnroAtom		= (MP4SnroAtom_p) pmaAtom;

			if (pmraSnroAtom -> getOffset (
				&ulRtpSequenceNumberRandomOffset) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] ptiTrackInfo -> _pucSampleBuffer;
				ptiTrackInfo -> _pucSampleBuffer		=
					(unsigned char *) NULL;

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			ptiTrackInfo -> _usRtpSequenceNumberRandomOffset			=
				(unsigned short) ulRtpSequenceNumberRandomOffset;
		}
		*/
	}
	else
	{
		ptiTrackInfo -> _ulRtpTimestampRandomOffset			= 0;

		ptiTrackInfo -> _usRtpSequenceNumberRandomOffset	= 0;
	}

	if (!bIsLive)
	{
		// _vReferencesTracksInfo
		{
			Boolean_t			bTrefAtomExist;
			Error_t				errGet;


			bTrefAtomExist				= true;

			if ((errGet = (ptiTrackInfo -> _pmtiTrackInfo) ->
				getHintAtom (&pmaHintAtom, true)) != errNoError)
			{
				if ((long) errGet == MP4F_MP4ATOM_ATOMNOTFOUND)
				{
					bTrefAtomExist			= false;
				}
				else
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETHINTATOM_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] ptiTrackInfo -> _pucSampleBuffer;
					ptiTrackInfo -> _pucSampleBuffer		=
						(unsigned char *) NULL;

					if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					/*
					if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
					*/

					if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			/*
			if (((ptiTrackInfo -> _mtiTrakInfo). _pmaTrakAtom) ->
				searchAtom (
				"tref:0:hint:0", true, &pmaAtom) != errNoError)
			{
				bTrefAtomExist			= false;
			}
			else
				pmhaHintAtom		= (MP4HintAtom_p) pmaAtom;
			*/

			if (bTrefAtomExist)
			{
				if (pmaHintAtom -> getEntriesNumber (&ulEntriesNumber) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] ptiTrackInfo -> _pucSampleBuffer;
					ptiTrackInfo -> _pucSampleBuffer		=
						(unsigned char *) NULL;

					if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					/*
					if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
					*/

					if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if ((ptiTrackInfo -> _csRTPClientSocket). finish () != errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_CLIENTSOCKET_INIT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				for (ulEntryIndex = 0; ulEntryIndex < ulEntriesNumber;
					ulEntryIndex++)
				{
					if (pmaHintAtom -> getTracksReferencesTable (ulEntryIndex,
						&ulTrackRefenrence) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4PROPERTY_GETVALUE_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] ptiTrackInfo -> _pucSampleBuffer;
						ptiTrackInfo -> _pucSampleBuffer		=
							(unsigned char *) NULL;

						if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
							errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_SERVERSOCKET_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						/*
						if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
							errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_SERVERSOCKET_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}
						*/

						if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
							errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_CLIENTSOCKET_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
							errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_CLIENTSOCKET_INIT_FAILED);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (ulTrackRefenrence == 0)
						continue;

					{
						Boolean_t				bTrackFound;
						MP4TrackInfo_p			pmtiMP4TrackInfo;


						bTrackFound			= false;

						if (pvMP4TracksInfo -> size () > 0)
						{
							std:: vector<MP4TrackInfo_p>:: const_iterator	it;
							unsigned long				ulTrackID;


							for (it = pvMP4TracksInfo -> begin ();
								it != pvMP4TracksInfo -> end (); ++it)
							{
								pmtiMP4TrackInfo				= *it;

								if (pmtiMP4TrackInfo -> getTrackIdentifier (
									&ulTrackID) != errNoError)
								{
									Error err = MP4FileErrors (
										__FILE__, __LINE__,
								MP4F_MP4TRACKINFO_GETTRACKIDENTIFIER_FAILED);
									_ptSystemTracer -> trace (
										Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);

									delete [] ptiTrackInfo -> _pucSampleBuffer;
									ptiTrackInfo -> _pucSampleBuffer		=
										(unsigned char *) NULL;

									if ((ptiTrackInfo -> _ssRTCPServerSocket).
										finish () != errNoError)
									{
										Error err = SocketErrors (
											__FILE__, __LINE__,
											SCK_SERVERSOCKET_INIT_FAILED);
										_ptSystemTracer -> trace (
											Tracer:: TRACER_LERRR,
											(const char *) err,
											__FILE__, __LINE__);
									}

									/*
									if (((*ptiTrackInfo) -> _ssRTPServerSocket).
										finish () != errNoError)
									{
										Error err = SocketErrors (
											__FILE__, __LINE__,
											SCK_SERVERSOCKET_INIT_FAILED);
										_ptSystemTracer -> trace (
											Tracer:: TRACER_LERRR,
											(const char *) err,
											__FILE__, __LINE__);
									}
									*/

									if ((ptiTrackInfo -> _csRTCPClientSocket).
										finish () != errNoError)
									{
										Error err = SocketErrors (
											__FILE__, __LINE__,
											SCK_CLIENTSOCKET_INIT_FAILED);
										_ptSystemTracer -> trace (
											Tracer:: TRACER_LERRR,
											(const char *) err,
											__FILE__, __LINE__);
									}

									if ((ptiTrackInfo -> _csRTPClientSocket).
										finish () != errNoError)
									{
										Error err = SocketErrors (
											__FILE__, __LINE__,
											SCK_CLIENTSOCKET_INIT_FAILED);
										_ptSystemTracer -> trace (
											Tracer:: TRACER_LERRR,
											(const char *) err,
											__FILE__, __LINE__);
									}

									return err;
								}

								if (ulTrackRefenrence == ulTrackID)
								{
									bTrackFound			= true;

									break;
								}
							}
						}

						if (!bTrackFound)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_REFERENCETRACKNOTFOUND);
							_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] ptiTrackInfo -> _pucSampleBuffer;
							ptiTrackInfo -> _pucSampleBuffer		=
								(unsigned char *) NULL;

							if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
								errNoError)
							{
								Error err = SocketErrors (__FILE__, __LINE__,
									SCK_SERVERSOCKET_INIT_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							/*
							if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
								errNoError)
							{
								Error err = SocketErrors (__FILE__, __LINE__,
									SCK_SERVERSOCKET_INIT_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}
							*/

							if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
								errNoError)
							{
								Error err = SocketErrors (__FILE__, __LINE__,
									SCK_CLIENTSOCKET_INIT_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							if ((ptiTrackInfo -> _csRTPClientSocket).
								finish () != errNoError)
							{
								Error err = SocketErrors (__FILE__, __LINE__,
									SCK_CLIENTSOCKET_INIT_FAILED);
								_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
						else
							(ptiTrackInfo -> _vReferencesTracksInfo). insert (
								(ptiTrackInfo -> _vReferencesTracksInfo). end (),
								pmtiMP4TrackInfo);
					}
				}
			}
		}

		// _ulFirstEditRTPTime
		/*
		if (ulMvhdTimeScale > 0)
			ptiTrackInfo -> _ulFirstEditRTPTime		=
				ptiTrackInfo -> _ulFirstEditMovieTime / ulMvhdTimeScale *
				ptiTrackInfo -> _ulHintTimeScale;
		else
		{
			// error ???
			ptiTrackInfo -> _ulFirstEditRTPTime		=
				ptiTrackInfo -> _ulFirstEditMovieTime *
				ptiTrackInfo -> _ulHintTimeScale;
		}
		*/
	}

	// _qosQualityOfService
	ptiTrackInfo -> _qosQualityOfService		= kAllPackets;

	if (RTPUtility:: getULongRandom (&(ptiTrackInfo -> _ulServerSSRC)) !=
		errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPUTILITY_GETULONGRANDOM_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (!bIsLive)
		{
			delete [] ptiTrackInfo -> _pucSampleBuffer;
			ptiTrackInfo -> _pucSampleBuffer		= (unsigned char *) NULL;
		}

		if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		/*
		if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}
		*/

		if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// At the begin it is 0, when the server receives the first RTCP packet,
	// it is initialized
	ptiTrackInfo -> _ulClientSSRC							= 0;

	// _ulSentRTPPacketsNumber
	ptiTrackInfo -> _ulSentRTPPacketsNumber					= 0;

	// _ulPacketBytesSent
	ptiTrackInfo -> _ullRTPPacketBytesSent					= 0;

	ptiTrackInfo -> _ulLostRTPPacketsNumber					= 0;

	ptiTrackInfo -> _ulRTCPPacketsNumberReceived			= 0;

	ptiTrackInfo -> _ullRTCPBytesReceived					= 0;

	ptiTrackInfo -> _ulJitter								= 0;

	if (!_bIsLive)
	{
		long					lRTPPacketIndex;


		for (lRTPPacketIndex = 0;
			lRTPPacketIndex < (long) _ulRTPPacketsNumberPreallocated;
			lRTPPacketIndex++)
		{
			if (((ptiTrackInfo -> _prpRTPPackets) [lRTPPacketIndex]).
				setRandomOffsets (
				ptiTrackInfo -> _usRtpSequenceNumberRandomOffset,
				ptiTrackInfo -> _ulRtpTimestampRandomOffset) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPPACKET_SETRANDOMOFFSETS_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (!bIsLive)
				{
					delete [] ptiTrackInfo -> _pucSampleBuffer;
					ptiTrackInfo -> _pucSampleBuffer		=
						(unsigned char *) NULL;
				}

				if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				/*
				if (((*ptiTrackInfo) -> _ssRTPServerSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
				*/

				if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if ((ptiTrackInfo -> _csRTPClientSocket). finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	ptiTrackInfo -> _bTrackInitialized		= true;


	return errNoError;
}


Error RTSPSession:: deleteTrackToStream (Boolean_t bIsLive,
	unsigned long ulTrackIdentifier)

{

	RTPHintSample:: RTPHintSampleStatus_t		sRTPHintSampleState;
	TrackInfo_p									ptiTrackInfo;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	ptiTrackInfo		= &((_rsRTPSession. _ptiTracks) [
		ulTrackIdentifier]);

	if (!ptiTrackInfo -> _bTrackInitialized)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_TRACKNOTINITIALIZED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		RTPPacket_p				prpRTPPacket;


		while ((((_rsRTPSession. _ptiTracks) [
			ulTrackIdentifier]). _vBuiltRTPPackets). begin () !=
			(((_rsRTPSession. _ptiTracks) [
			ulTrackIdentifier]). _vBuiltRTPPackets). end ())
		{
			prpRTPPacket			= *((((_rsRTPSession. _ptiTracks) [
				ulTrackIdentifier]). _vBuiltRTPPackets). begin ());

			(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vFreeRTPPackets).
				insert (
				(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _vFreeRTPPackets). end (),
				prpRTPPacket);

			(((_rsRTPSession. _ptiTracks) [
				ulTrackIdentifier]). _vBuiltRTPPackets). erase (
				(((_rsRTPSession. _ptiTracks) [
				ulTrackIdentifier]). _vBuiltRTPPackets). begin ());

			(((_rsRTPSession. _ptiTracks) [
				ulTrackIdentifier]). _vTransmissionTimeBuiltRTPPackets). erase (
				(((_rsRTPSession. _ptiTracks) [
				ulTrackIdentifier]). _vTransmissionTimeBuiltRTPPackets).
				begin ());
		}
	}

	{
		unsigned long			ulRTPPacketIndex;


		for (ulRTPPacketIndex = 0;
			ulRTPPacketIndex < _ulRTPPacketsNumberPreallocated;
			ulRTPPacketIndex++)
		{
			if (((ptiTrackInfo -> _prpRTPPackets) [
				ulRTPPacketIndex]). reset () != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPPACKET_RESET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	if (!bIsLive)
	{
		(ptiTrackInfo -> _vReferencesTracksInfo). clear ();

		if ((ptiTrackInfo -> _rhsCurrentRTPHintSample). getState (
			&sRTPHintSampleState) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTSAMPLE_GETSTATE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (sRTPHintSampleState == RTPHintSample:: RHS_INITIALIZED)
		{
			if ((ptiTrackInfo -> _rhsCurrentRTPHintSample). finish () !=
				errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTSAMPLE_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		delete [] ptiTrackInfo -> _pucSampleBuffer;
		ptiTrackInfo -> _pucSampleBuffer		= (unsigned char *) NULL;
	}

	if ((ptiTrackInfo -> _ssRTCPServerSocket). finish () != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SERVERSOCKET_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((ptiTrackInfo -> _csRTCPClientSocket). finish () !=
		errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((ptiTrackInfo -> _csRTPClientSocket). finish () != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	ptiTrackInfo -> _bTrackInitialized		= false;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error RTSPSession:: getFinalInfo (
		__int64 *pullTotalBytesServed,
		__int64 *pullTotalLostPacketsNumber,
		__int64 *pullTotalSentRTPPacketsNumber)
#else
	Error RTSPSession:: getFinalInfo (
		unsigned long long *pullTotalBytesServed,
		unsigned long long *pullTotalLostPacketsNumber,
		unsigned long long *pullTotalSentRTPPacketsNumber)
#endif

{

	unsigned long				ulTrackIdentifier;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pullTotalBytesServed			= 0;
	*pullTotalLostPacketsNumber		= 0;
	*pullTotalSentRTPPacketsNumber		= 0;

	for (ulTrackIdentifier = 0;
		ulTrackIdentifier < SS_RTSPSESSION_MAXTRACKSNUMBER;
		ulTrackIdentifier++)
	{
		// we don't need the next check because the session is finished and
		//	the track should not be initialized
		// if (!(((_rsRTPSession. _ptiTracks) [ulTrackIdentifier]). _bTrackInitialized))
		//	continue;

		*pullTotalBytesServed			+= ((_rsRTPSession. _ptiTracks) [
			ulTrackIdentifier]). _ullRTPPacketBytesSent;

		*pullTotalLostPacketsNumber			+= ((_rsRTPSession. _ptiTracks) [
			ulTrackIdentifier]). _ulLostRTPPacketsNumber;

		*pullTotalSentRTPPacketsNumber			+= ((_rsRTPSession. _ptiTracks) [
			ulTrackIdentifier]). _ulSentRTPPacketsNumber;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	return errNoError;
}


Error RTSPSession:: getAvgBitRate (
	unsigned long *pulAudioBitRate,
	unsigned long *pulVideoBitRate)

{

	RTSPStatus_t		rsRTSPStatus;


	*pulAudioBitRate			= 0;
	*pulVideoBitRate			= 0;

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus == RTSPSession:: SS_RTSP_NOTUSED ||
		rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "getAvgBitRate", (long) rsRTSPStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bVideoTrackFoundInSDP)
	{
		*pulVideoBitRate				= _ulVideoAvgBitRate;
	}

	if (_bAudioTrackFoundInSDP)
	{
		*pulAudioBitRate				= _ulAudioAvgBitRate;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error RTSPSession:: getInfo (
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
		__int64 *pullConnectedTimeInSeconds)
#else
	Error RTSPSession:: getInfo (
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
		unsigned long long *pullConnectedTimeInSeconds)
#endif

{

	RTSPStatus_t		rsRTSPStatus;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus == RTSPSession:: SS_RTSP_NOTUSED ||
		rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
		rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "getInfo", (long) rsRTSPStatus);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		// 	(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	strcpy (pClientIPAddress, _pClientIPAddress);

	if (pbRelativePathWithoutParameters -> setBuffer (
		(const char *) _bRelativePathWithoutParameters) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbURLParameters -> setBuffer (
		(const char *) _bURLParameters) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	*psStandard				= _sStandard;

	*pbIsLive		= _bIsLive;

	if (_bIsLive)
		*pdDuration				= -1.0;
	else
		*pdDuration				= _dMovieDuration;

	if (_bVideoTrackFoundInSDP)
	{
		*pcVideoCodec			= _cVideoCodecUsed;

		*pulVideoPacketsNumberSent		= 
			((_rsRTPSession. _ptiTracks) [_ulVideoTrackIdentifier]). _ulSentRTPPacketsNumber;

		*pullVideoBytesSent				=
			((_rsRTPSession. _ptiTracks) [_ulVideoTrackIdentifier]). _ullRTPPacketBytesSent;

		*pulVideoPacketsNumberLost		=
			((_rsRTPSession. _ptiTracks) [_ulVideoTrackIdentifier]). _ulLostRTPPacketsNumber;

		*pulVideoAverageBitRate			= _ulVideoAvgBitRate;

		*pulVideoRTCPPacketsNumberReceived	=
			((_rsRTPSession. _ptiTracks) [_ulVideoTrackIdentifier]). _ulRTCPPacketsNumberReceived;
		*pullVideoRTCPBytesReceived		=
			((_rsRTPSession. _ptiTracks) [_ulVideoTrackIdentifier]). _ullRTCPBytesReceived;
		*pulVideoJitter					=
			((_rsRTPSession. _ptiTracks) [_ulVideoTrackIdentifier]). _ulJitter;
	}
	else
	{
		*pcVideoCodec			= MP4Atom:: MP4F_CODEC_UNKNOWN;

		*pulVideoPacketsNumberSent		= 0;

		*pullVideoBytesSent				= 0;

		*pulVideoPacketsNumberLost		= 0;

		*pulVideoAverageBitRate			= 0;

		*pulVideoRTCPPacketsNumberReceived	= 0;

		*pullVideoRTCPBytesReceived		= 0;

		*pulVideoJitter					= 0;
	}

	if (_bAudioTrackFoundInSDP)
	{
		*pcAudioCodec			= _cAudioCodecUsed;

		*pulAudioPacketsNumberSent		= 
			((_rsRTPSession. _ptiTracks) [_ulAudioTrackIdentifier]). _ulSentRTPPacketsNumber;

		*pullAudioBytesSent				=
			((_rsRTPSession. _ptiTracks) [_ulAudioTrackIdentifier]). _ullRTPPacketBytesSent;

		*pulAudioPacketsNumberLost		=
			((_rsRTPSession. _ptiTracks) [_ulAudioTrackIdentifier]). _ulLostRTPPacketsNumber;

		*pulAudioAverageBitRate			= _ulAudioAvgBitRate;

		*pulAudioRTCPPacketsNumberReceived	=
			((_rsRTPSession. _ptiTracks) [_ulAudioTrackIdentifier]). _ulRTCPPacketsNumberReceived;
		*pullAudioRTCPBytesReceived		=
			((_rsRTPSession. _ptiTracks) [_ulAudioTrackIdentifier]). _ullRTCPBytesReceived;
		*pulAudioJitter					=
			((_rsRTPSession. _ptiTracks) [_ulAudioTrackIdentifier]). _ulJitter;
	}
	else
	{
		*pcAudioCodec			= MP4Atom:: MP4F_CODEC_UNKNOWN;

		*pulAudioPacketsNumberSent		= 0;

		*pullAudioBytesSent				= 0;

		*pulAudioPacketsNumberLost		= 0;

		*pulAudioAverageBitRate			= 0;

		*pulAudioRTCPPacketsNumberReceived	= 0;

		*pullAudioRTCPBytesReceived		= 0;

		*pulAudioJitter					= 0;
	}

	*pullConnectedTimeInSeconds			= time (NULL) - _tUTCStartConnectionTime;

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPSession:: getAverageBandWidthUsage (
	unsigned long *pulAverageBandWidthUsage)

{

	RTSPStatus_t		rsRTSPStatus;


	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getRTSPStatus (&rsRTSPStatus) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_GETRTSPSTATUS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (rsRTSPStatus == RTSPSession:: SS_RTSP_NOTUSED ||
		rsRTSPStatus == RTSPSession:: SS_RTSP_CONNECTIONACCEPTED ||
		rsRTSPStatus == RTSPSession:: SS_RTSP_INIT)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			3, _ulIdentifier, "getAverageBandWidthUsage", (long) rsRTSPStatus);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		// 	(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	*pulAverageBandWidthUsage			= 0;

	if (_bVideoTrackFoundInSDP)
	{
		*pulAverageBandWidthUsage			+= _ulVideoAvgBitRate;
	}

	if (_bAudioTrackFoundInSDP)
	{
		*pulAverageBandWidthUsage			+= _ulAudioAvgBitRate;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}
