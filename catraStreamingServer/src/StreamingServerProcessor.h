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


#ifndef StreamingServerProcessor_h
	#define StreamingServerProcessor_h

	#include "PosixThread.h"
	#include "StreamingServerErrors.h"
	#include "StreamingServerEventsSet.h"
	#include "StreamingServerSocketsPool.h"
	#include "RTSPConnectionEvent.h"
	#include "LiveEvent.h"
	#include "ConfigurationFile.h"
	#include "Tracer.h"
	#include "RTSPSession.h"


	#define SS_STREAMINGSERVERPROCESSOR_DESTINATION	"StreamingServerProcessor"
	#define SS_STREAMINGSERVERPROCESSOR_SLEEPTIMESECSWAITINGEVENT		2
	#define SS_STREAMINGSERVERPROCESSOR_SLEEPTIMEMILLISECSWAITINGEVENT	0

	class StreamingServerProcessor: public PosixThread
	{
		private:
			unsigned long				_ulProcessorIdentifier;
			MP4Atom:: Standard_t		_sStandard;
			StreamingServerEventsSet_p	_pesEventsSet;
			ServerSocket_p				_pssServerSocket;
			SocketsPool_p				_pspSocketsPool;
			RTSPSession_p				_prsRTSPSessions;
			unsigned long				_ulRTSPSessionsNumber;
			PMutex_p					_pmtFreeRTSPSessions;
			std:: vector<RTSPSession_p>	*_pvFreeRTSPSessions;
			char						_pLocalIPAddressForRTP [
				SCK_MAXIPADDRESSLENGTH];
			PMutex_p					_pmtStreamingServerStatistics;
			PMutex_p					_pmtLiveSources;
			#ifdef WIN32
				__int64					*_pullTotalBytesServed;
				__int64					*_pullTotalLostPacketsNumber;
				__int64					*_pullTotalSentPacketsNumber;
			#else
				unsigned long long		*_pullTotalBytesServed;
				unsigned long long		*_pullTotalLostPacketsNumber;
				unsigned long long		*_pullTotalSentPacketsNumber;
			#endif
			ConfigurationFile_p			_pcfConfiguration;
			Tracer_p					_ptSystemTracer;
			Tracer_p					_ptSubscriberTracer;

			Buffer_t					_bDestination;
			Buffer_t					_bAuthorizationBuffer;

			PMutex_t					_mtShutdown;
			Boolean_t					_bIsShutdown;


			Error handleNewRTSPConnectionEvent (
				RTSPConnectionEvent_p pevNewRTSPConnection);

			Error handleRTSPConnectionReadyToReadEvent (
				RTSPConnectionEvent_p pevRTSPConnection);

			Error handleRTSP_RTCPTimeoutEvent (
				RTSPConnectionEvent_p pevRTSPConnection);

			Error handlePauseTimeoutEvent (
				RTSPConnectionEvent_p pevRTSPConnection);

			Error handleStreamingSessionTimeoutEvent (
				RTSPConnectionEvent_p pevRTSPConnection);

			Error handleReceiveRTCPPacketsEvent (
				RTSPConnectionEvent_p pevRTSPConnection);

			Error handleSendRTCPPacketsEvent (
				RTSPConnectionEvent_p pevRTSPConnection);

			Error handleSendFileRTPPacketsEvent (
				RTSPConnectionEvent_p pevRTSPConnection);

			Error handleSendLiveRTPPacketsEvent (
				LiveEvent_p pevLive);

			Error handleAuthorizationErrorEvent (
				RTSPConnectionEvent_p pevRTSPConnection);

			Error handleAuthorizationDeniedEvent (
				RTSPConnectionEvent_p pevRTSPConnection);

			Error handleAuthorizationGrantedEvent (
				RTSPConnectionEvent_p pevRTSPConnection);

			Error closeConnection (
				unsigned long ulRTSPSessionIdentifier,
				Error_p perr);

			Error getIsShutdown (Boolean_p pbIsShutdown);

			Error setIsShutdown (Boolean_t bIsShutdown);

		protected:
			virtual Error run (void);

			Error processEvent (Event_p pevEvent);

		public:
			StreamingServerProcessor (void);

			~StreamingServerProcessor (void);

			Error init (
				unsigned long ulProcessorIdentifier,
				MP4Atom:: Standard_t sStandard,
				StreamingServerEventsSet_p pesEventsSet,
				ServerSocket_p pssServerSocket,
				SocketsPool_p pspSocketsPool,
				RTSPSession_p prsRTSPSessions,
				unsigned long ulRTSPSessionsNumber,
				PMutex_p pmtFreeRTSPSessions,
				std:: vector<RTSPSession_p>	*pvFreeRTSPSessions,
				const char *pLocalIPAddressForRTP,
				PMutex_p pmtStreamingServerStatistics,
				PMutex_p pmtLiveSources,
				#ifdef WIN32
					__int64 *pullTotalBytesServed,
					__int64 *pullTotalLostPacketsNumber,
					__int64 *pullTotalSentPacketsNumber,
				#else
					unsigned long long *pullTotalBytesServed,
					unsigned long long *pullTotalLostPacketsNumber,
					unsigned long long *pullTotalSentPacketsNumber,
				#endif
				ConfigurationFile_p pcfConfiguration,
				Tracer_p ptTracer,
				Tracer_p ptSubscriberTracer);

			Error finish ();

			virtual Error cancel (void);
	} ;

	typedef class StreamingServerProcessor
		StreamingServerProcessor_t, *StreamingServerProcessor_p;

#endif
