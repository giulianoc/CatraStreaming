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


#ifndef StreamingServerService_h
	#define StreamingServerService_h

	#include "StreamingServerErrors.h"
	#include "Service.h"
	#include "ConfigurationFile.h"
	#include "SubscriberTracer.h"
	#include "RTSPSession.h"
	#include "StreamingServerEventsSet.h"
	#include "Scheduler.h"
	#include "HttpStreamingServer.h"
	#include "CheckServerSocketTimes.h"
	#include "CheckSocketsPoolTimes.h"
	#include "StreamingServerSocketsPool.h"
	#include "StreamingServerProcessor.h"
	#include <vector>


	#define SS_STREAMINGSERVERSERVICE_PIDFILEPATHNAME	\
		"/var/run/catraStreamingServer.pid"

	#define SS_STREAMINGSERVERSERVICE_CONFPATHNAMEENVIRONMENTVARIABLE	\
		"CATRASTREAMINGCONFIGURATIONPATHNAME"
	#define SS_STREAMINGSERVERSERVICE_SERVICENAME	"catraStreaming"
	#define SS_STREAMINGSERVERSERVICE_SERVICEDESCRIPTION	"Catra Streaming Platform"

	#define SS_STREAMINGSERVERSERVICE_MAXTRACEFILELENGTH			1024 + 1
	#define SS_STREAMINGSERVERSERVICE_MAXHOSTNAMELENGTH				128 + 1
	#define SS_STREAMINGSERVERSERVICE_MAXREQUESTLOGFORMATLENGTH		1024
	#define SS_STREAMINGSERVERSERVICE_MAXSERVLETPATHNAMELENGTH		512 + 1

	#define SS_STREAMINGSERVERSERVICE_MAXBOOLEANLENGTH				512 + 1
	#define SS_STREAMINGSERVERSERVICE_MAXLICENSELENGTH				512 + 1
	#define SS_STREAMINGSERVERSERVICE_DATETIMELENGTH				256 + 1

	#define SS_STREAMINGSERVERSERVICE_MAXHTTPREQUESTS				30
	#define SS_STREAMINGSERVERSERVICE_HTTPSERVERRECEIVINGTIMEOUTINSECS	5
	#define SS_STREAMINGSERVERSERVICE_HTTPSERVERSENDINGTIMEOUTINSECS	5
/*
	#define SS_STREAMINGSERVERSERVICE_MAXLONGLENGTH					512 + 1

	#define SS_STREAMINGSERVERSERVICE_MAXCONFIGURATIONITEMS			1000
	#define SS_STREAMINGSERVERSERVICE_MAXSECTIONNAMELENGTH			512 + 1
	#define SS_STREAMINGSERVERSERVICE_MAXITEMCOMMENTLENGTH			1024 * 5
	#define SS_STREAMINGSERVERSERVICE_MAXITEMNAMELENGTH				512 + 1
	#define SS_STREAMINGSERVERSERVICE_MAXITEMVALUELENGTH			1024 * 2

	#define SS_STREAMINGSERVERSERVICE_MAXFILES						10000
	#define SS_STREAMINGSERVERSERVICE_MAXDIRECTORIES				1000

	#define SS_STREAMINGSERVERSERVICE_MAXNICS						100

*/


	class StreamingServerService: public Service
	{
		private:
			ConfigurationFile_t			_cfConfiguration;
			Tracer_t					_tSystemTracer;
			SubscriberTracer_t			_tSubscriberTracer;
			std:: vector<SocketImpl:: IPAddress_t> _vIPAddresses;
			char						_pHostName [
				SS_STREAMINGSERVERSERVICE_MAXHOSTNAMELENGTH];
			Buffer_t					_bContentRootPath;

			StreamingServerProcessor_p	_pspStreamingServerProcessor;
			unsigned long				_ulStreamingServerProcessorsNumber;

			MP4Atom:: Standard_t    	_sStandard;

			MP4FileFactory_t			_mp4fMP4FileFactory;
			RTSPSession_p				_prsRTSPSessions;
			unsigned long				_ulMaxRTSPServerSessions;
			char						_pLocalIPAddressForRTSP [
				SCK_MAXIPADDRESSLENGTH];
			unsigned long				_ulRTSPRequestsPort;
			unsigned long				_ulFileCacheSizeInBytes;
			std:: vector<RTSPSession_p>	_vFreeRTSPSessions;
			PMutex_t					_mtFreeRTSPSessions;

			unsigned long				_ulCheckServerSocketPeriodInMilliSecs;
			unsigned long				_ulCheckSocketsPoolPeriodInMilliSecs;
			unsigned long				_ulSchedulerSleepTimeInMilliSecs;

			StreamingServerEventsSet_t	_esEventsSet;
			StreamingServerSocketsPool_t	_spStreamingServerSocketsPool;
			Scheduler_t					_scScheduler;
			ServerSocket_t				_ssServerSocket;

			CheckServerSocketTimes_t	_cssCheckServerSocketTimes;
			CheckSocketsPoolTimes_t		_cspCheckSocketsPoolTimes;

			char						_pLocalIPAddressForHTTP [
				SCK_MAXIPADDRESSLENGTH];
			unsigned long				_ulHTTPRequestsPort;
			HttpStreamingServer_t		_hssHttpStreamingServer;

			#ifdef WIN32
				__int64					_ullTotalBytesServed;
				__int64					_ullTotalLostPacketsNumber;
				__int64					_ullTotalSentPacketsNumber;
			#else
				unsigned long long		_ullTotalBytesServed;
				unsigned long long		_ullTotalLostPacketsNumber;
				unsigned long long		_ullTotalSentPacketsNumber;
			#endif

			PMutex_t					_mtStreamingServerStatistics;
			PMutex_t					_mtLiveSources;
			LiveSource_p				_plsLiveSources;


			Error startTracer (
				ConfigurationFile_p pcfConfiguration,
				Tracer_p ptTracer, char *pSectionName);

			Error stopTracer (
				Tracer_p ptTracer);

			Error checkLicense (
				ConfigurationFile_p pcfConfiguration,
				Tracer_p ptSystemTracer);

			Error initNICs (
				std:: vector<SocketImpl:: IPAddress_t> *pvIPAddresses);

			Error finishNICs (
				std:: vector<SocketImpl:: IPAddress_t> *pvIPAddresses);

			static Error onInitMP4FileFactory (
				ConfigurationFile_p pcfConfiguration,
				MP4FileFactory_p pmp4fMP4FileFactory,
				Tracer_p ptSystemTracer);

			static Error onInitRTSPSessions (
				ConfigurationFile_p pcfConfiguration,
				const char *pContentRootPath,
				MP4Atom:: Standard_t sStandard,
				unsigned long *pulMaxRTSPServerSessions,
				unsigned long *pulMaxLiveSourcesNumber,
				StreamingServerEventsSet_p pesEventsSet,
				StreamingServerSocketsPool_p pspStreamingServerSocketsPool,
				Scheduler_p pscScheduler,
				PMutex_p pmtLiveSources,
				LiveSource_p plsLiveSources,
				MP4FileFactory_p pmp4fMP4FileFactory,
				Tracer_p ptSystemTracer,
				SubscriberTracer_p ptSubscriberTracer,
				RTSPSession_p prsRTSPSessions,	// OUT ...
				char *pLocalIPAddressForRTSP,
				std:: vector<RTSPSession_p> *pvFreeRTSPSessions,
				unsigned long *pulFileCacheSizeInBytes,
				unsigned long *pulRTSPRequestsPort);

		protected:
			/**
				Only on unix/linux environment, this method must be
				re-defined in order to set the command that will be
				used by the script inside the /etc/rc.d/ined.d directory
				to start the service.
			*/
			virtual Error appendStartScriptCommand (
				Buffer_p pbServiceScriptFile);

			/**
				Only on unix/linux environment, this method must be
				re-defined in order to set the command that will be
				used by the script inside the /etc/rc.d/ined.d directory
				to start the service.
			*/
			virtual Error appendStopScriptCommand (
				Buffer_p pbServiceScriptFile);

			/**
				Only on unix/linux environment, this method must be
				re-defined in order to set the command that will be
				used by the script inside the /etc/rc.d/ined.d directory
				to start the service.
			*/
			virtual Error appendStatusScriptCommand (
				Buffer_p pbServiceScriptFile);

			virtual Error onInit (void);

			virtual Error onStop (void);

			virtual Error onStart (void);

		public:
			StreamingServerService (void);

			~StreamingServerService (void);

			Error init (void);

			Error finish ();

			Error cancel (void);

			/*
			#ifdef HAVE_CORBA_H
				virtual CORBA::Long isRequestInCache (
					const char *pAssetPath,
					CORBA:: ULong_out bIsInCache,
					CORBA:: ULong_out ulConnectedUsers,
					CORBA:: ULong_out ulBandWidthUsageInbps);

				virtual CORBA:: Long setChangedConfiguration (
					const ::StreamingIDL:: ConfigurationItemsList &
					cilConfigurationItemsList);

				virtual CORBA:: Long getHostName (CORBA:: String_out pHostName);

// 			virtual CORBA:: Long getNICs (
//				::StreamingIDL:: StringsList_out lsNICs);
//				throw (::StreamingIDL:: StreamingExc);

				virtual CORBA:: Long addContent (
					const char *pContentName,
					const ::StreamingIDL:: BytesList &blContent);

				virtual CORBA:: Long deleteContent (const char *pContentName);
			#endif
			*/
	} ;

	typedef class StreamingServerService
		StreamingServerService_t, *StreamingServerService_p;

#endif

