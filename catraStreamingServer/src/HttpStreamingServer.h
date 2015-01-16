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


#ifndef HTTPServer_h
	#define HTTPServer_h

	#include "HttpServer.h"
	#include "Tracer.h"
	#include "RTSPSession.h"
	#include "CheckServerSocketTimes.h"
	#include "ConfigurationFile.h"
	#include "StreamingServerErrors.h"

	#define SS_HTTPSTREAMINGSERVER_SHUTDOWNCOMMAND	\
		"/catraStreamingServer?command=shutdown"
	#define SS_HTTPSTREAMINGSERVER_GETCONNECTEDUSERSCOMMAND	\
		"/catraStreamingServer?command=getConnectedUsers"
	#define SS_HTTPSTREAMINGSERVER_GETSERVERINFOCOMMAND	\
		"/catraStreamingServer?command=getServerInfo"
	#define SS_HTTPSTREAMINGSERVER_GETCONTENTSCOMMAND	\
		"/catraStreamingServer?command=getContents&Directory="
	#define SS_HTTPSTREAMINGSERVER_ADDDIRECTORYCOMMAND	\
		"/catraStreamingServer?command=addDirectory&NewDirectory="
	#define SS_HTTPSTREAMINGSERVER_DELETEDIRECTORYCOMMAND	\
		"/catraStreamingServer?command=deleteDirectory&DirectoryToBeDeleted="
	#define SS_HTTPSTREAMINGSERVER_DUMPCONTENTCOMMAND	\
		"/catraStreamingServer?command=dumpContent&ContentRelativePath="
	#define SS_HTTPSTREAMINGSERVER_GETTRACKSINFOCOMMAND	\
		"/catraStreamingServer?command=getTracksInfo&ContentRelativePath="
	#define SS_HTTPSTREAMINGSERVER_GETCONFIGURATIONCOMMAND	\
		"/catraStreamingServer?command=getConfiguration"
	#define SS_HTTPSTREAMINGSERVER_SETCONFIGURATIONVALUECOMMAND	\
		"/catraStreamingServer?command=setConfigurationValue&SectionName"
		// "/catraStreamingServer?command=setConfigurationValue&SectionName=aaa&ItemName=aaa&NewItemValue=aaa"
	
	// other commands could be also 'addContent', 'deleteContent'


	#define SS_HTTPSTREAMINGSERVER_MAXSECTIONNAMELENGTH			512 + 1
	#define SS_HTTPSTREAMINGSERVER_MAXSECTIONCOMMENTLENGTH		1024 * 2
	#define SS_HTTPSTREAMINGSERVER_MAXITEMCOMMENTLENGTH			1024 * 5
	#define SS_HTTPSTREAMINGSERVER_MAXITEMNAMELENGTH			512 + 1
	#define SS_HTTPSTREAMINGSERVER_MAXITEMVALUELENGTH			1024 * 2

	typedef class HttpStreamingServer: public HttpServer {

		private:
			Buffer_t				_bContentRootPath;
			MP4FileFactory_p		_pmp4fMP4FileFactory;
			MP4Atom:: Standard_t    _sStandard;
			ConfigurationFile_p		_pcfConfiguration;
			unsigned long			_ulFileCacheSizeInBytes;
			unsigned long			_ulMaxRTSPServerSessions;
			RTSPSession_p			_prsRTSPSessions;
			CheckServerSocketTimes_p	_pcssCheckServerSocketTimes;
			time_t					_tServerStartTime;

			PMutex_p				_pmtFreeRTSPSessions;
			std:: vector<RTSPSession_p>		*_pvFreeRTSPSessions;

			PMutex_p				_pmtStreamingServerStatistics;
			#ifdef WIN32
				__int64				*_pullTotalBytesServed;
				__int64				*_pullTotalLostPacketsNumber;
				__int64				*_pullTotalSentPacketsNumber;
			#else
				unsigned long long	*_pullTotalBytesServed;
				unsigned long long	*_pullTotalLostPacketsNumber;
				unsigned long long	*_pullTotalSentPacketsNumber;
			#endif

			Tracer_p				_ptSystemTracer;


			Error appendXMLForConnectedUsers (
				unsigned long ulMaxRTSPServerSessions,
				RTSPSession_p prsRTSPSessions,
				Buffer_p pbResponseBody,
				unsigned long *pulConnectedUsersNumber);

			Error appendXMLForGetTracksInfo (
				MP4File_p pmp4File, Buffer_p pbResponseBody);

			Error appendXMLForGetConfiguration (
				ConfigurationFile_p pcfConfiguration, Buffer_p pbResponseBody);

			Error appendXMLForGetContents (
				Buffer_p pbDirectory, Buffer_p pbResponseBody);

		protected:
			virtual Error httpGetReceived (
				const char *pRemoteAddress,
				long lRemotePort,
				Buffer_p pbURL,
				Buffer_p pbHeaders,
				SocketImpl_p pClientSocketImpl);

		public:
			HttpStreamingServer (void);

			~HttpStreamingServer (void);

			#ifdef WIN32
				Error init (
					const char *pServerLocalIpAddress,
					long lServerPort,
					const char *pContentRootPath,
					MP4FileFactory_p pmp4fMP4FileFactory,
					MP4Atom:: Standard_t sStandard,
					ConfigurationFile_p pcfConfiguration,
					unsigned long ulFileCacheSizeInBytes,
					unsigned long ulReceivingTimeoutInSeconds,
					unsigned long ulReceivingTimeoutInMicroSeconds,
					unsigned long ulSendingTimeoutInSeconds,
					unsigned long ulSendingTimeoutInMicroSeconds,
					unsigned long ulMaxClients,
					unsigned long ulMaxRTSPServerSessions,
					RTSPSession_p prsRTSPSessions,
					CheckServerSocketTimes_p pcssCheckServerSocketTimes,
					PMutex_p pmtFreeRTSPSessions,
					std:: vector<RTSPSession_p> *pvFreeRTSPSessions,
					PMutex_p pmtStreamingServerStatistics,
					__int64 *pullTotalBytesServed,
					__int64 *pullTotalLostPacketsNumber,
					__int64 *pullTotalSentPacketsNumber,
					Tracer_p ptSystemTracer);
			#else
				Error init (
					const char *pServerLocalIpAddress,
					long lServerPort,
					const char *pContentRootPath,
					MP4FileFactory_p pmp4fMP4FileFactory,
					MP4Atom:: Standard_t sStandard,
					ConfigurationFile_p pcfConfiguration,
					unsigned long ulFileCacheSizeInBytes,
					unsigned long ulReceivingTimeoutInSeconds,
					unsigned long ulReceivingTimeoutInMicroSeconds,
					unsigned long ulSendingTimeoutInSeconds,
					unsigned long ulSendingTimeoutInMicroSeconds,
					unsigned long ulMaxClients,
					unsigned long ulMaxRTSPServerSessions,
					RTSPSession_p prsRTSPSessions,
					CheckServerSocketTimes_p pcssCheckServerSocketTimes,
					PMutex_p pmtFreeRTSPSessions,
					std:: vector<RTSPSession_p> *pvFreeRTSPSessions,
					PMutex_p pmtStreamingServerStatistics,
					unsigned long long *pullTotalBytesServed,
					unsigned long long *pullTotalLostPacketsNumber,
					unsigned long long *pullTotalSentPacketsNumber,
					Tracer_p ptSystemTracer);
			#endif


			virtual Error finish (void);

	} HttpStreamingServer_t, *HttpStreamingServer_p;

#endif

