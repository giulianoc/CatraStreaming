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


#ifndef StreamingServersManager_h
	#define StreamingServersManager_h

	#ifdef WIN32
	#else
		#include "CatraStreaming_OSConfig.h"
	#endif

	#include "PosixThread.h"
	#include "StreamingServerErrors.h"
	#include "ConfigurationFile.h"
	#include "Tracer.h"
	#include "RTSPSessionRedirection.h"
	#ifdef HAVE_CORBA_H
		#include "StreamingIDL.h"
	#endif
	#include "vector"


	#define SS_MAXLONGLENGTH					512 + 1
	#define SS_MAXCONTENTROOTPATHLENGTH			512 + 1

	#define SS_MAXCONFIGURATIONITEMS			1000
	#define SS_MAXSECTIONNAMELENGTH				512 + 1
	#define SS_MAXITEMCOMMENTLENGTH				1024 + 1
	#define SS_MAXITEMNAMELENGTH				512 + 1
	#define SS_MAXITEMVALUELENGTH				512 + 1

	#define SS_MAXFILES							10000
	#define SS_MAXDIRECTORIES					1000

	#define SS_MAXHOSTNAMELENGTH				1024 + 1
	#define SS_MAXNICS							100

	#ifdef HAVE_CORBA_H
		class StreamingServersManager:
			virtual public POA_StreamingIDL:: StreamingServersManager,
			public PosixThread
	#else
		class StreamingServersManager: public PosixThread
	#endif
	{

		private:
			ConfigurationFile_p			_pcfConfiguration;
			Tracer_p					_ptTracer;
			time_t						_tServersManagerStartTime;

			CORBA:: ORB_ptr				_porb;
			PortableServer:: ObjectId	*_poid;
			CORBA:: Object_var			_pref;
			char						_pStreamingName [
				SS_MAXSTREAMINGNAMELENGTH];
			MP4FileFactory_t			_mp4fMP4FileFactory;
			RTSPSessionRedirection:: StreamingServerInfo_p
				_pssiStreamingServerInfo;
			RTSPSessionRedirection_p	_prsRTSPSessionsRedirection;
			unsigned long				_ulMaxServerBandWidthInKbps;
			unsigned long				_ulServersNumber;
			unsigned long				_ulMaxRTSPServerSessions;
			char						_pLocalIPAddressForRTSP [
				SCK_MAXIPADDRESSLENGTH];
			unsigned long				_ulRTSPRequestsPort;
			std:: vector<RTSPSessionRedirection_p>	_vFreeRTSPSessions;
			std:: vector<RTSPSessionRedirection_p>	_vActiveRTSPSessions;
			unsigned long			_ulSleepTimeInMilliSecsWaitingRTSPRequests;

			PMutex_t					_mtShutdown;
			Boolean_t					_bIsShutdown;
			std:: vector<char *>		_vNICs;
			char						_pHostName [SS_MAXHOSTNAMELENGTH];
			char						_pContentRootPath [
				SS_MAXCONTENTROOTPATHLENGTH];


			Error registerToNamingService (
				const char *pStreamingServerName);

			Error getIsShutdown (Boolean_p pbIsShutdown);

			Error setIsShutdown (Boolean_t bIsShutdown);

			Error initNICs (std:: vector<char *> *pvNICs);

			Error finishNICs (std:: vector<char *> *pvNICs);

		protected:
			virtual Error run (void);

		public:
			StreamingServersManager (void);

			~StreamingServersManager (void);

			#if HAVE_CORBA_H
				Error init (
					CORBA:: ORB_ptr porb,
					ConfigurationFile_p pcfConfiguration,
					Tracer_p ptTracer);
			#else
				Error init (
					ConfigurationFile_p pcfConfiguration,
					Tracer_p ptTracer);
			#endif

			Error finish ();

			virtual Error cancel (void);

			#ifdef WIN32
				Error getServerInfo (
					unsigned long *pulPlayersNumberConnected,
					__int64 *pullTotalBytesServed,
					__int64 *pullTotalLostPacketsNumber,
					__int64 *pullTotalSentPacketsNumber,
					unsigned long *pulBandWidthUsageInbps);
			#else
				Error getServerInfo (
					unsigned long *pulPlayersNumberConnected,
					unsigned long long *pullTotalBytesServed,
					unsigned long long *pullTotalLostPacketsNumber,
					unsigned long long *pullTotalSentPacketsNumber,
					unsigned long *pulBandWidthUsageInbps);
			#endif

			#ifdef HAVE_CORBA_H
    			virtual CORBA:: Long prova ();

    			virtual CORBA:: Long activate ();

    			virtual CORBA:: Long deactivate ();

    			virtual CORBA:: Long shutdown ();

    			virtual CORBA:: Long setChangedConfiguration (
					const ::StreamingIDL:: ConfigurationItemsList &
					cilConfigurationItemsList);

    			virtual CORBA:: Long getConfiguration (
					::StreamingIDL:: ConfigurationItemsWithInfoList_out
					cilConfigurationItemsWithInfoList);

				virtual CORBA:: Long getHostName (CORBA:: String_out pHostName);

//    			virtual CORBA:: Long getNICs (
//					::StreamingIDL:: StringsList_out lsNICs);
//					throw (::StreamingIDL:: StreamingExc);

				virtual CORBA:: Long getContents (const char *pDirectory,
					::StreamingIDL:: StringsList_out lsFileNames,
					::StreamingIDL:: StringsList_out lsDirectories);

				virtual CORBA:: Long addDirectory (const char *pDirectoryName);

				virtual CORBA:: Long deleteDirectory (
					const char *pDirectoryName);

				virtual CORBA:: Long addContent (
					const char *pContentName,
					const ::StreamingIDL:: BytesList &blContent);

				virtual CORBA:: Long deleteContent (const char *pContentName);

				virtual CORBA:: Long dumpContent (const char *pContentName,
					CORBA:: String_out pDump);

				virtual CORBA:: Long getTracksInfo (const char *pContentName,
					CORBA:: String_out pTracksInfo);


				virtual CORBA:: Long getServerConnectedUsers (
					::StreamingIDL:: ServerConnectedUsersList_out
					scuServerConnectedUsersList);

				virtual CORBA:: Long getServerInfo (
					::StreamingIDL:: ServerInfo_out siServerInfo);
			#endif
	} ;

	typedef class StreamingServersManager
		StreamingServersManager_t, *StreamingServersManager_p;

#endif
