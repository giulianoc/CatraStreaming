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


#include "StreamingServersManager.h"
#include "StreamingServerMessages.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "FileIO.h"
#include <stdlib.h>
#include <assert.h>
#ifdef WIN32
	#include <Winsock2.h>
	#include <direct.h>
#else
	#include <netdb.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <sys/utsname.h>
	#include <unistd.h>
	#include <dirent.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef HAVE_CORBA_H
	#include "coss/CosNaming.h"
#endif
#include "GetCpuUsage.h"


#ifdef HAVE_CORBA_H
	StreamingServersManager:: StreamingServersManager (void): PosixThread (),
		POA_StreamingIDL:: StreamingServersManager ()
#else
	StreamingServersManager:: StreamingServersManager (void): PosixThread ()
#endif

{

}


StreamingServersManager:: ~StreamingServersManager (void)

{

}


#if HAVE_CORBA_H
	Error StreamingServersManager:: init (
		CORBA:: ORB_ptr porb,
		ConfigurationFile_p pcfConfiguration,
		Tracer_p ptTracer)
#else
	Error StreamingServersManager:: init (
		ConfigurationFile_p pcfConfiguration,
		Tracer_p ptTracer)
#endif

{

	char					pConfigurationBuffer [SS_MAXLONGLENGTH];
	#ifdef WIN32
	#else
		struct utsname		unUtsname;
	#endif


	_pcfConfiguration						= pcfConfiguration;
	_ptTracer								= ptTracer;

	_tServersManagerStartTime				= time (NULL);

	// host name initialization
	#ifdef WIN32
		WSADATA			wsaData;

		if (WSAStartup (MAKEWORD (2, 2), &wsaData))
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_WSASTARTUP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (gethostname (_pHostName, SS_MAXHOSTNAMELENGTH) == -1)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_GETHOSTNAME_FAILED,
				1, WSAGetLastError ());
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (WSACleanup ())
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_WSACLEANUP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (WSACleanup ())
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_WSACLEANUP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	#else
		if (uname (&unUtsname) == -1)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_UNAME_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
		strcpy (_pHostName, unUtsname. nodename);
	#endif

	// mp4fMP4FileFactory initialization
	{
		unsigned long			ulMaxMp4FilesNumberInMemory;
		unsigned long			ulMp4FilesNumberToDeleteOnOverflow;
		Boolean_t				bUseMP4ConsistencyCheck;
		unsigned long			ulBucketsNumberForMP4FilesCache;

		if ((errGetItemValue = _pcfConfiguration -> getItemValue ("Cache",
			"MaxMp4FilesNumberInMemory", pConfigurationBuffer)) !=
			errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errGetItemValue,
				__FILE__, __LINE__);

			Error err = ConfigurationErrors (__FILE__, __LINE__,
				CFG_CONFIG_GETITEMVALUE_FAILED,
				2, "Cache", "MaxMp4FilesNumberInMemory");
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		ulMaxMp4FilesNumberInMemory			= 
			strtoul (pConfigurationBuffer, (char **) NULL, 10);

		if ((errGetItemValue = _pcfConfiguration -> getItemValue ("Cache",
			"Mp4FilesNumberToDeleteOnOverflow",
			pConfigurationBuffer)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errGetItemValue,
				__FILE__, __LINE__);

			Error err = ConfigurationErrors (__FILE__, __LINE__,
				CFG_CONFIG_GETITEMVALUE_FAILED,
				2, "Cache", "MaxMp4FilesNumberInMemory");
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		ulMp4FilesNumberToDeleteOnOverflow				=
			strtoul (pConfigurationBuffer, (char **) NULL, 10);

		if ((errGetItemValue = _pcfConfiguration -> getItemValue ("Cache",
			"UseMP4ConsistencyCheck", pConfigurationBuffer)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errGetItemValue,
				__FILE__, __LINE__);

			Error err = ConfigurationErrors (__FILE__, __LINE__,
				CFG_CONFIG_GETITEMVALUE_FAILED,
				2, "Cache", "UseMP4ConsistencyCheck");
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		if (!strcmp (pConfigurationBuffer, "true"))
			bUseMP4ConsistencyCheck			= true;
		else
			bUseMP4ConsistencyCheck			= false;

		if ((errGetItemValue = _pcfConfiguration -> getItemValue ("Cache",
			"BucketsNumberForMP4FilesCache",
			pConfigurationBuffer)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errGetItemValue,
				__FILE__, __LINE__);

			Error err = ConfigurationErrors (__FILE__, __LINE__,
				CFG_CONFIG_GETITEMVALUE_FAILED,
				2, "Cache", "BucketsNumberForMP4FilesCache");
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		ulBucketsNumberForMP4FilesCache				=
			strtoul (pConfigurationBuffer, (char **) NULL, 10);

		if (_mp4fMP4FileFactory. init (ulMaxMp4FilesNumberInMemory,
			ulMp4FilesNumberToDeleteOnOverflow, bUseMP4ConsistencyCheck,
			_ptTracer, ulBucketsNumberForMP4FilesCache) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	if ((errGetItemValue = _pcfConfiguration -> getItemValue (
		"StreamingServersManager",
		"MaxRTSPSessions", pConfigurationBuffer)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "StreamingServersManager", "MaxRTSPSessions");
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	_ulMaxRTSPServerSessions			= strtoul (pConfigurationBuffer,
		(char **) NULL, 10);

	if ((errGetItemValue = _pcfConfiguration -> getItemValue ("System",
		"ContentRootPath", _pContentRootPath)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "ContentRootPath");
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	_vFreeRTSPSessions. reserve (_ulMaxRTSPServerSessions);

	if ((errGetItemValue = _pcfConfiguration -> getItemValue (
		"StreamingServersManager",
		"ServersNumber", pConfigurationBuffer)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "StreamingServersManager", "ServersNumber");
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	_ulServersNumber			=
		(unsigned long) atol (pConfigurationBuffer);

	if ((errGetItemValue = _pcfConfiguration -> getItemValue (
		"IPConfiguration",
		"LocalIPAddressForRTSP", _pLocalIPAddressForRTSP)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "IPConfiguration", "LocalIPAddressForRTSP");
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((errGetItemValue = _pcfConfiguration -> getItemValue (
		"StreamingServersManager",
		"RTSPRequestsPort", pConfigurationBuffer)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "StreamingServersManager", "RTSPRequestsPort");
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	_ulRTSPRequestsPort			=
		(unsigned long) atol (pConfigurationBuffer);

	// streaming servers vector initialization
	{
		unsigned long			ulServerIndex;
		char					pServerIdentifier [SS_MAXLONGLENGTH];
		char					pServerName [SS_MAXSTREAMINGNAMELENGTH];


		if ((errGetItemValue = _pcfConfiguration -> getItemValue (
			"StreamingServersManager",
			"Name", pServerName)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errGetItemValue,
				__FILE__, __LINE__);

			Error err = ConfigurationErrors (__FILE__, __LINE__,
				CFG_CONFIG_GETITEMVALUE_FAILED,
				2, "StreamingServersManager", "Name");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if ((_pssiStreamingServerInfo =
			new RTSPSessionRedirection:: StreamingServerInfo_t [
			_ulServersNumber]) ==
			(RTSPSessionRedirection:: StreamingServerInfo_p) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		for (ulServerIndex = 0; ulServerIndex < _ulServersNumber;
			ulServerIndex++)
		{
			(_pssiStreamingServerInfo [ulServerIndex]). _bIsActive	=
				true;

			strcpy ((_pssiStreamingServerInfo [ulServerIndex]). _pName,
				pServerName);
			strcat ((_pssiStreamingServerInfo [ulServerIndex]). _pName,
				"_Server");
			sprintf (pServerIdentifier, "%lu", ulServerIndex + 1);
			strcat ((_pssiStreamingServerInfo [ulServerIndex]). _pName,
				pServerIdentifier);

			strcpy ((_pssiStreamingServerInfo [ulServerIndex]). _pIpAddress,
				_pLocalIPAddressForRTSP);

			(_pssiStreamingServerInfo [ulServerIndex]). _ulPort		=
				_ulRTSPRequestsPort + ulServerIndex + 1;
		}
	}

	#ifdef HAVE_CORBA_H
		_porb				= porb;
	#endif

	// initialization of the RTSP sessions (prsRTSPSessionsRedirection)
	{
		long							lRTSPSessionIndex;



		if ((errGetItemValue = _pcfConfiguration -> getItemValue ("System",
			"MaxServerBandWidthInKbps", pConfigurationBuffer)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errGetItemValue,
				__FILE__, __LINE__);

			Error err = ConfigurationErrors (__FILE__, __LINE__,
				CFG_CONFIG_GETITEMVALUE_FAILED,
				2, "System", "MaxServerBandWidthInKbps");
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] _pssiStreamingServerInfo;
			_pssiStreamingServerInfo	=
				(RTSPSessionRedirection:: StreamingServerInfo_p) NULL;

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		_ulMaxServerBandWidthInKbps			=
			(unsigned long) atol (pConfigurationBuffer);

		if ((_prsRTSPSessionsRedirection = new RTSPSessionRedirection_t [
			_ulMaxRTSPServerSessions]) == (RTSPSessionRedirection_p) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] _pssiStreamingServerInfo;
			_pssiStreamingServerInfo	=
				(RTSPSessionRedirection:: StreamingServerInfo_p) NULL;

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		for (lRTSPSessionIndex = 0;
			lRTSPSessionIndex < (long) _ulMaxRTSPServerSessions;
			lRTSPSessionIndex++)
		{
			#ifdef HAVE_CORBA_H
				if ((_prsRTSPSessionsRedirection [lRTSPSessionIndex]). init (
					_porb,
					lRTSPSessionIndex,
					_ulServersNumber,
					_pssiStreamingServerInfo,
					_ulMaxRTSPServerSessions,
					_ulMaxServerBandWidthInKbps,
					_pContentRootPath,
					_ptTracer) != errNoError)
			#else
				if ((_prsRTSPSessionsRedirection [lRTSPSessionIndex]). init (
					lRTSPSessionIndex,
					_ulServersNumber,
					_pssiStreamingServerInfo,
					_ulMaxRTSPServerSessions,
					_ulMaxServerBandWidthInKbps,
					_pContentRootPath,
					_ptTracer) != errNoError)
			#endif
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				_vFreeRTSPSessions. clear ();

				while (--lRTSPSessionIndex >= 0)
				{
					if ((_prsRTSPSessionsRedirection [lRTSPSessionIndex]). finish () !=
						errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				delete [] _prsRTSPSessionsRedirection;
				_prsRTSPSessionsRedirection			= (RTSPSessionRedirection_p) NULL;

				delete [] _pssiStreamingServerInfo;
				_pssiStreamingServerInfo	=
					(RTSPSessionRedirection:: StreamingServerInfo_p) NULL;

				if (_mp4fMP4FileFactory. finish () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILEFACTORY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			_vFreeRTSPSessions. insert (_vFreeRTSPSessions. end (),
				&(_prsRTSPSessionsRedirection [lRTSPSessionIndex]));
		}
	}

	// From the STL documentation: A vector's iterators are invalidated
	//	when its memory is reallocated. Additionally, inserting or deleting
	//	an element in the middle of a vector invalidates all iterators
	//	that point to elements following the insertion or deletion point.
	//	It follows that you can prevent a vector's iterators from being
	//	invalidated if you use reserve() to preallocate as much memory 
	//	as the vector will ever use, and if all insertions and deletions
	//	are at the vector's end. 
	_vActiveRTSPSessions. reserve (_ulMaxRTSPServerSessions);
	_vActiveRTSPSessions. clear ();

	// register CORBA
	{
		if ((errGetItemValue = _pcfConfiguration -> getItemValue (
			"StreamingServersManager",
			"Name", _pStreamingName)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errGetItemValue,
				__FILE__, __LINE__);

			Error err = ConfigurationErrors (__FILE__, __LINE__,
				CFG_CONFIG_GETITEMVALUE_FAILED,
				2, "StreamingServersManager", "Name");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			_vFreeRTSPSessions. clear ();

			{
				unsigned long			ulRTSPSessionIndex;

				for (ulRTSPSessionIndex = 0;
					ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
					ulRTSPSessionIndex++)
				{
					if ((_prsRTSPSessionsRedirection [ulRTSPSessionIndex]). finish () !=
						errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}

			delete [] _prsRTSPSessionsRedirection;
			_prsRTSPSessionsRedirection			= (RTSPSessionRedirection_p) NULL;

			delete [] _pssiStreamingServerInfo;
			_pssiStreamingServerInfo	=
				(RTSPSessionRedirection:: StreamingServerInfo_p) NULL;

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		strcat (_pStreamingName, "_ServersManager");

		#ifdef HAVE_CORBA_H
			_pref				= (CORBA:: Object_var) NULL;

			{
				Message msg = StreamingServerMessages (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_CORBA_NAMEREGISTRATION,
					1, _pStreamingName);
				_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
					__FILE__, __LINE__);
			}

			if (registerToNamingService (_pStreamingName) !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_REGISTERTONAMINGSERVICE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				_vFreeRTSPSessions. clear ();

				{
					unsigned long			ulRTSPSessionIndex;

					for (ulRTSPSessionIndex = 0;
						ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
						ulRTSPSessionIndex++)
					{
						if ((_prsRTSPSessionsRedirection [ulRTSPSessionIndex]). finish () !=
							errNoError)
						{
							Error err = StreamingServerErrors (
								__FILE__, __LINE__,
								SS_RTSPSESSION_FINISH_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}
					}
				}

				delete [] _prsRTSPSessionsRedirection;
				_prsRTSPSessionsRedirection			= (RTSPSessionRedirection_p) NULL;

				delete [] _pssiStreamingServerInfo;
				_pssiStreamingServerInfo	=
					(RTSPSessionRedirection:: StreamingServerInfo_p) NULL;

				if (_mp4fMP4FileFactory. finish () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILEFACTORY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		#endif
	}

	if ((errGetItemValue = _pcfConfiguration -> getItemValue ("System",
		"SleepTimeInMilliSecsWaitingRTSPRequests",
		pConfigurationBuffer)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "SleepTimeInMilliSecsWaitingRTSPRequests");
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		_vFreeRTSPSessions. clear ();

		{
			unsigned long			ulRTSPSessionIndex;

			for (ulRTSPSessionIndex = 0;
				ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
				ulRTSPSessionIndex++)
			{
				if ((_prsRTSPSessionsRedirection [ulRTSPSessionIndex]). finish () !=
					errNoError)
				{
					Error err = StreamingServerErrors (
						__FILE__, __LINE__,
						SS_RTSPSESSION_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		delete [] _prsRTSPSessionsRedirection;
		_prsRTSPSessionsRedirection			= (RTSPSessionRedirection_p) NULL;

		delete [] _pssiStreamingServerInfo;
		_pssiStreamingServerInfo	=
			(RTSPSessionRedirection:: StreamingServerInfo_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	_ulSleepTimeInMilliSecsWaitingRTSPRequests		=
		strtoul (pConfigurationBuffer,
		(char **) NULL, 10);

	/*
	PThreadSchedulingPolicy_t spSchedulingPolicy;
	_rsRTPSessions. getSchedulingPolicy (&spSchedulingPolicy);

	long		lPriority;
	_rsRTPSessions. getPriority (&lPriority);
	
	long ll = sched_get_priority_max(SCHED_FIFO);
	ll = sched_get_priority_max(SCHED_RR);
	ll = sched_get_priority_max(SCHED_OTHER);

	_rsRTPSessions. setPriority (ll);
	*/

	/*
	long lStackSize		= 3072 * 1024;
	if ((_prsRTSPSessions [ulRTSPSessionIndex]). setStackSize (
		lStackSize) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_SETSTACKSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);
			return err;
	}
	*/

	#if defined(__CYGWIN__)
		if (_mtShutdown. init (PMutex:: MUTEX_RECURSIVE) !=
			errNoError)
	#else							// POSIX.1-1996 standard (HPUX 11)
		if (_mtShutdown. init (PMutex:: MUTEX_RECURSIVE) !=
			errNoError)
	#endif
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		_vFreeRTSPSessions. clear ();

		{
			unsigned long			ulRTSPSessionIndex;

			for (ulRTSPSessionIndex = 0;
				ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
				ulRTSPSessionIndex++)
			{
				if ((_prsRTSPSessionsRedirection [ulRTSPSessionIndex]). finish () !=
					errNoError)
				{
					Error err = StreamingServerErrors (
						__FILE__, __LINE__,
						SS_RTSPSESSION_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		delete [] _prsRTSPSessionsRedirection;
		_prsRTSPSessionsRedirection			= (RTSPSessionRedirection_p) NULL;

		delete [] _pssiStreamingServerInfo;
		_pssiStreamingServerInfo	=
			(RTSPSessionRedirection:: StreamingServerInfo_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (initNICs (&_vNICs) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERSMANAGER_INITNICS_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtShutdown. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		_vFreeRTSPSessions. clear ();

		{
			unsigned long			ulRTSPSessionIndex;

			for (ulRTSPSessionIndex = 0;
				ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
				ulRTSPSessionIndex++)
			{
				if ((_prsRTSPSessionsRedirection [ulRTSPSessionIndex]). finish () !=
					errNoError)
				{
					Error err = StreamingServerErrors (
						__FILE__, __LINE__,
						SS_RTSPSESSION_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		delete [] _prsRTSPSessionsRedirection;
		_prsRTSPSessionsRedirection			= (RTSPSessionRedirection_p) NULL;

		delete [] _pssiStreamingServerInfo;
		_pssiStreamingServerInfo	=
			(RTSPSessionRedirection:: StreamingServerInfo_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (PosixThread:: init () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (finishNICs (&_vNICs) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_FINISHNICS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtShutdown. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		_vFreeRTSPSessions. clear ();

		{
			unsigned long			ulRTSPSessionIndex;

			for (ulRTSPSessionIndex = 0;
				ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
				ulRTSPSessionIndex++)
			{
				if ((_prsRTSPSessionsRedirection [ulRTSPSessionIndex]). finish () !=
					errNoError)
				{
					Error err = StreamingServerErrors (
						__FILE__, __LINE__,
						SS_RTSPSESSION_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		delete [] _prsRTSPSessionsRedirection;
		_prsRTSPSessionsRedirection			= (RTSPSessionRedirection_p) NULL;

		delete [] _pssiStreamingServerInfo;
		_pssiStreamingServerInfo	=
			(RTSPSessionRedirection:: StreamingServerInfo_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}


	return errNoError;
}


Error StreamingServersManager:: finish ()

{

	if (PosixThread:: finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (finishNICs (&_vNICs) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERSMANAGER_FINISHNICS_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_mtShutdown. finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	_vActiveRTSPSessions. clear ();

	_vFreeRTSPSessions. clear ();

	{
		unsigned long			ulRTSPSessionIndex;

		for (ulRTSPSessionIndex = 0;
			ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
			ulRTSPSessionIndex++)
		{
			if ((_prsRTSPSessionsRedirection [ulRTSPSessionIndex]). finish () !=
				errNoError)
			{
				Error err = StreamingServerErrors (
					__FILE__, __LINE__,
					SS_RTSPSESSION_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}
	}

	delete [] _prsRTSPSessionsRedirection;
	_prsRTSPSessionsRedirection			= (RTSPSessionRedirection_p) NULL;

	delete [] _pssiStreamingServerInfo;
	_pssiStreamingServerInfo	=
		(RTSPSessionRedirection:: StreamingServerInfo_p) NULL;

	if (_mp4fMP4FileFactory. finish () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILEFACTORY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}


	return errNoError;
}


Error StreamingServersManager:: run (void)

{

	ServerSocket_t				ssServerSocket;
	SocketImpl_p				pServerSocketImpl;
	long						lMaxClients;
	Boolean_t					bIsShutdown;
	Error						errAcceptConnection;
	Error_t						errServerSocketInit;
	Error_t						errGetFirstFreeConnection;
	RTSPSessionRedirection_p	prsRTSPSessionRedirection;
	std:: vector<RTSPSessionRedirection_p>:: iterator		it;
	std:: vector<RTSPSessionRedirection_p>:: iterator		itToDelete;
	Boolean_t					bWasThereConnectionAccepted;
	Boolean_t					bWasThereRTSPCommands;
	Boolean_t					bWasThereRTSPCommand;
	RTSPSession:: RTSPRequestInfo_t		raRTSPRequestInfo;


	lMaxClients			= 120;

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVER_RUNNING, 3,
			_pLocalIPAddressForRTSP, _ulRTSPRequestsPort, lMaxClients);
		_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	if ((errServerSocketInit = ssServerSocket. init (_pLocalIPAddressForRTSP,
		_ulRTSPRequestsPort, true, SocketImpl:: STREAM, lMaxClients)) !=
		errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errServerSocketInit,
			__FILE__, __LINE__);
		_erThreadReturn = SocketErrors (__FILE__, __LINE__,
			SCK_SERVERSOCKET_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		return _erThreadReturn;
	}

	if (ssServerSocket. getSocketImpl (&pServerSocketImpl) != errNoError)
	{
		_erThreadReturn = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKET_GETSOCKETIMPL_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		if (ssServerSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return _erThreadReturn;
	}

	if (pServerSocketImpl -> setBlocking (false) != errNoError)
	{
		_erThreadReturn = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_SETBLOCKING_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		if (ssServerSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return _erThreadReturn;
	}

	bIsShutdown					= false;
	if (setIsShutdown (bIsShutdown) != errNoError)
	{
		_erThreadReturn = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVER_SETISSHUTDOWN_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		if (ssServerSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return _erThreadReturn;
	}

	while (!bIsShutdown)
	{
		bWasThereConnectionAccepted			= false;

		// accept all the connections in the queue
		do
		{
			if (_vFreeRTSPSessions. size () == 0)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_NOTFOUNDRTSPSESSIONFREE);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				break;
			}

			prsRTSPSessionRedirection			= *(_vFreeRTSPSessions. begin ());

			if ((errAcceptConnection = prsRTSPSessionRedirection -> acceptRTSPConnection (
				&ssServerSocket)) != errNoError)
			{
				if ((long) errAcceptConnection == SS_RTSPSESSION_NORTSPCONNECTIONTOACCEPT_FAILED)
					;
				else
				{
					_erThreadReturn = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_ACCEPTRTSPCONNECTION_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) _erThreadReturn, __FILE__, __LINE__);

					if (ssServerSocket. finish () != errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return _erThreadReturn;
				}
			}
			else
			{
				bWasThereConnectionAccepted			= true;

				_vFreeRTSPSessions. erase (_vFreeRTSPSessions. begin ());

				_vActiveRTSPSessions. insert (_vActiveRTSPSessions. end (),
					prsRTSPSessionRedirection);
			}

			if (getIsShutdown (&bIsShutdown) != errNoError)
			{
				_erThreadReturn = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_GETISSHUTDOWN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (ssServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}
		}
		while (errAcceptConnection == errNoError && !bIsShutdown);
		// we will exit from the previous loop in the following cases:
		//	1. there aren't external connections
		//	2. the process shall do shutdown
		//	3. there aren't RTSPSession object available

		if (bIsShutdown)
			continue;

		bWasThereRTSPCommands			= false;

		// From the STL documentation: A vector's iterators are invalidated
		//	when its memory is reallocated. Additionally, inserting or deleting
		//	an element in the middle of a vector invalidates all iterators
		//	that point to elements following the insertion or deletion point.
		//	It follows that you can prevent a vector's iterators from being
		//	invalidated if you use reserve() to preallocate as much memory 
		//	as the vector will ever use, and if all insertions and deletions
		//	are at the vector's end. 
		for (it = _vActiveRTSPSessions. begin (); it != _vActiveRTSPSessions. end ();
			/* ++it look below */)
		{
			prsRTSPSessionRedirection			= *it;

			if (prsRTSPSessionRedirection -> checkAndProcessRequest (
				&bWasThereRTSPCommand, &raRTSPRequestInfo) != errNoError)
			{
				// I don't want to shutdown the server since there are some errors
			}

			switch (raRTSPRequestInfo)
			{
				case RTSPSession:: SS_NONE:
					++it;

					break;
				case RTSPSession:: SS_RTSPREQUEST_FINISHED:
					{
						if (it == _vActiveRTSPSessions. begin ())
						{
							_vActiveRTSPSessions. erase (it);
							it			= _vActiveRTSPSessions. begin ();
						}
						else
						{
							itToDelete			= it;
							it--;

							_vActiveRTSPSessions. erase (itToDelete);
							it++;
						}

						_vFreeRTSPSessions. insert (_vFreeRTSPSessions. end (),
							prsRTSPSessionRedirection);
					}

					break;
				case RTSPSession:: SS_RTSPREQUEST_TOBEHANDLEDASOFFLINE:
				case RTSPSession:: SS_RTSPREQUEST_TOBEHANDLEDASLIVE:
				default:
					{
						_erThreadReturn = StreamingServerErrors (__FILE__, __LINE__,
							SS_STREAMINGSERVER_RTSPREQUESTINFOWRONG);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) _erThreadReturn, __FILE__, __LINE__);

						if (ssServerSocket. finish () != errNoError)
						{
							Error err = SocketErrors (__FILE__, __LINE__,
								SCK_SERVERSOCKET_FINISH_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return _erThreadReturn;
					}

					break;
			}

			if (bWasThereRTSPCommand && !bWasThereRTSPCommands)
				bWasThereRTSPCommands			= true;

			if (getIsShutdown (&bIsShutdown) != errNoError)
			{
				_erThreadReturn = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVER_GETISSHUTDOWN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (ssServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}

			if (bIsShutdown)
				break;
		}

		if (bIsShutdown)
			continue;

		if (!bWasThereRTSPCommands && !bWasThereConnectionAccepted)
		{
			if (PosixThread:: getSleep (
				(unsigned long) (_ulSleepTimeInMilliSecsWaitingRTSPRequests / 1000),
				(_ulSleepTimeInMilliSecsWaitingRTSPRequests -
				(((unsigned long) (_ulSleepTimeInMilliSecsWaitingRTSPRequests / 1000)) * 1000)) *
				1000) != errNoError)
			{
				_erThreadReturn = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_GETSLEEP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (ssServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}
		}
	}

	if (ssServerSocket. finish () != errNoError)
	{
		_erThreadReturn = SocketErrors (__FILE__, __LINE__,
			SCK_SERVERSOCKET_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		return _erThreadReturn;
	}


	return _erThreadReturn;
}


Error StreamingServersManager:: cancel (void)

{

	time_t							tUTCNow;
	PosixThread:: PThreadStatus_t	stRTPThreadState;


	if (setIsShutdown (true) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVER_SETISSHUTDOWN_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getThreadState (&stRTPThreadState) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_GETTHREADSTATE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	tUTCNow					= time (NULL);

	while (stRTPThreadState == THREADLIB_STARTED ||
		stRTPThreadState == THREADLIB_STARTED_AND_JOINED)
	{
		if (time (NULL) - tUTCNow >= 5)
			break;

		if (PosixThread:: getSleep (1, 0) != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_GETSLEEP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (getThreadState (&stRTPThreadState) != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_GETTHREADSTATE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (stRTPThreadState == THREADLIB_STARTED ||
		stRTPThreadState == THREADLIB_STARTED_AND_JOINED)
	{
		if (PosixThread:: cancel () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_CANCEL_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error StreamingServersManager:: getIsShutdown (
	Boolean_p pbIsShutdown)

{

	if (_mtShutdown. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pbIsShutdown				= _bIsShutdown;

	if (_mtShutdown. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	return errNoError;
}


Error StreamingServersManager:: setIsShutdown (
	Boolean_t bIsShutdown)

{

	if (_mtShutdown. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	_bIsShutdown			= bIsShutdown;

	if (_mtShutdown. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	return errNoError;
}


Error StreamingServersManager:: initNICs (
	std:: vector<char *> *pvNICs)

{
	/*
	#ifdef WIN32
		struct addrinfo				aiAddrInfo;
		struct addrinfo				aiAddrInfoHints;
		struct sockaddr_in			*psaSockAddr;
		struct addrinfo				*paiNextAddrInfo;

		aiAddrInfoHints. ai_family			= AF_INET;
		aiAddrInfoHints. ai_socktype		= 0;
		aiAddrInfoHints. ai_protocol		= 0;
		aiAddrInfoHints. ai_addrlen			= 0;
		aiAddrInfoHints. ai_canonname		= (char *) NULL;
		aiAddrInfoHints. ai_addr			= (struct sockaddr *) NULL;
		aiAddrInfoHints. ai_next			= (struct addrinfo *) NULL;

		if (getaddrinfo(_pHostName,
			(const char FAR *) NULL,
			&aiAddrInfoHints,
			&aiAddrInfo))
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_GETADDRINFO_FAILED,
				1, WSAGetLastError ());
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 1;
			// throw ::StreamingGUIIDL:: StreamingGUIExc ((const char *) err);
		}

		paiNextAddrInfo			= &aiAddrInfo;

		while (paiNextAddrInfo != (struct addrinfo  *) NULL)
		{
			psaSockAddr			= paiNextAddrInfo -> ai_addr;

			if (slStringsList. length () + 1 > SS_MAXNICS)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_LISTTOOLONG);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return 2;
				// throw ::StreamingGUIIDL:: StreamingGUIExc (
				//	(const char *) err);
			}

			slStringsList. length (slStringsList. length () + 1);
			slStringsList [slStringsList.length () - 1]		=
				CORBA:: string_dup (psaSockAddr -> sin_addr);

			paiNextAddrInfo		= paiNextAddrInfo -> ai_next;
		}
	#else
	*/
		struct hostent							*phHostent;
		struct in_addr							*piaInAddr;
		char									*pNIC;

		if ((phHostent = gethostbyname (_pHostName)) == (hostent *) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_WRONGHOSTNAME,
				1, _pHostName);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		switch (phHostent -> h_addrtype)
		{
			case AF_INET:
				while ((piaInAddr =
					(struct in_addr *) *(phHostent -> h_addr_list)++) !=
					(struct in_addr *) NULL)
				{
					if ((pNIC = new char [strlen (inet_ntoa (*piaInAddr)) + 1]) ==
						(char *) NULL)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_NEW_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						{
							std:: vector<char *>:: const_iterator		it;

							for (it = pvNICs -> begin (); it != pvNICs -> end ();
								++it)
							{
								pNIC				= *it;

								delete [] pNIC;
								pNIC				= (char *) NULL;
							}

							pvNICs -> clear ();
						}

						return err;
					}
					strcpy (pNIC, inet_ntoa (*piaInAddr));

					pvNICs -> insert (pvNICs -> end (), pNIC);
				}

				break;
			default:
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERSMANAGER_UNKNOWNADDRESSTYPE,
						1, (long) phHostent -> h_addrtype);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				break;
		}
	// #endif


	return errNoError;
}


Error StreamingServersManager:: finishNICs (
	std:: vector<char *> *pvNICs)

{

	std:: vector<char *>:: const_iterator		it;
	char										*pNIC;


	for (it = pvNICs -> begin (); it != pvNICs -> end ();
		++it)
	{
		pNIC				= *it;

		delete [] pNIC;
		pNIC				= (char *) NULL;
	}

	pvNICs -> clear ();


	return errNoError;
}


#ifdef HAVE_CORBA_H

	Error StreamingServersManager:: registerToNamingService (
		const char *pStreamingServerName)

	{
		CORBA::Object_var			poaobj;
		PortableServer::POA_var		poa;

		try
		{
			// POA initialization
			poaobj			= _porb -> resolve_initial_references ("RootPOA");
			poa				= PortableServer::POA::_narrow (poaobj);

			if (_pref == (CORBA:: Object_var) NULL)
			{
				_poid			= poa -> activate_object (this);
				_pref			= poa -> id_to_reference (*_poid);
			}

			CORBA::Object_var nsobj					=
				_porb -> resolve_initial_references ("NameService");

			CosNaming::NamingContext_var pnc			=
				CosNaming::NamingContext::_narrow (nsobj);

			if (CORBA::is_nil (pnc))
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_REGISTERTONAMINGSERVICE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			{
				CosNaming:: Name			name;
				name. length (1);
				name [0]. id			= CORBA::string_dup (pStreamingServerName);
				name [0]. kind			= CORBA::string_dup ("");
	  
				// rebind because if it is alread binded we will receive an error
				pnc -> rebind (name, _pref);
			}
		}
		catch (CosNaming::NamingContext::NotFound &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTNOTFOUND);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			poa -> deactivate_object (*_poid);
			_pref		= (CORBA:: Object_var) NULL;

			return err;
		}
		catch (CosNaming::NamingContext::CannotProceed &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTCANNOTPROCEED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			poa -> deactivate_object (*_poid);
			_pref		= (CORBA:: Object_var) NULL;

			return err;
		}
		catch (CosNaming::NamingContext::InvalidName &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTINVALIDNAME);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			poa -> deactivate_object (*_poid);
			_pref		= (CORBA:: Object_var) NULL;

			return err;
		}
		catch (CORBA::SystemException &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTGENERICERROR,
				1, pStreamingServerName);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			poa -> deactivate_object (*_poid);
			_pref		= (CORBA:: Object_var) NULL;

			return err;
		}
		catch (...)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CORBA_GENERICERROR);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			poa -> deactivate_object (*_poid);
			_pref		= (CORBA:: Object_var) NULL;

			return err;
		}


		return errNoError;
	}

	CORBA:: Long StreamingServersManager:: prova ()

	{

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_PROVA);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}


		return 0;
	}


	CORBA:: Long StreamingServersManager:: activate ()

	{

		CosNaming:: NamingContext_var			pnc;
		::StreamingIDL:: StreamingServer_var	pssStreamingServer;
		unsigned long							ulServerIndex;

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_ACTIVATE);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		try
		{
			CORBA::Object_var nsobj		=
				_porb -> resolve_initial_references ("NameService");

			pnc							=
				CosNaming::NamingContext::_narrow (nsobj);

			if (CORBA::is_nil (pnc))
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_REGISTERTONAMINGSERVICE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 1;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
		}
		catch (CosNaming::NamingContext::NotFound &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTNOTFOUND);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 2;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CosNaming::NamingContext::CannotProceed &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTCANNOTPROCEED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 3;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CosNaming::NamingContext::InvalidName &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTINVALIDNAME);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 4;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CORBA::SystemException &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTGENERICERROR,
				1, "NameService");
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 5;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (...)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CORBA_GENERICERROR);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 6;
		}

		for (ulServerIndex = 0; ulServerIndex < _ulServersNumber; ulServerIndex++)
		{
			try
			{
				CosNaming:: Name			name;
				name. length (1);
				name [0]. id			= CORBA::string_dup ((_pssiStreamingServerInfo [ulServerIndex]). _pName);
				name [0]. kind			= CORBA::string_dup ("");
	  
				pssStreamingServer		=
					::StreamingIDL:: StreamingServer:: _narrow (pnc -> resolve (name));

				if (pssStreamingServer -> activate () != 0)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVER_ACTIVATE_FAILED,
						1, ulServerIndex + 1);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
						__FILE__, __LINE__);

					return 7;
					// throw ::StreamingIDL:: StreamingExc ((const char *) err);
				}

				(_pssiStreamingServerInfo [ulServerIndex]). _bIsActive		= true;
			}
			catch (::StreamingIDL:: StreamingExc &)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_STREAMINGSERVERERROR,
					1, "activate");
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 8;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
			catch (CORBA::SystemException &)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_STREAMINGSERVERERROR,
					1, "activate");
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 9;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
			catch (...)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_CORBA_GENERICERROR);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 10;
			}
		}


		return 0;
	}


	CORBA:: Long StreamingServersManager:: deactivate ()

	{

		CosNaming:: NamingContext_var			pnc;
		::StreamingIDL:: StreamingServer_var	pssStreamingServer;
		unsigned long							ulServerIndex;

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_DEACTIVATE);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		try
		{
			CORBA::Object_var nsobj		=
				_porb -> resolve_initial_references ("NameService");

			pnc							=
				CosNaming::NamingContext::_narrow (nsobj);

			if (CORBA::is_nil (pnc))
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_REGISTERTONAMINGSERVICE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 1;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
		}
		catch (CosNaming::NamingContext::NotFound &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTNOTFOUND);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 2;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CosNaming::NamingContext::CannotProceed &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTCANNOTPROCEED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 3;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CosNaming::NamingContext::InvalidName &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTINVALIDNAME);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 4;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CORBA::SystemException &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTGENERICERROR,
				1, "NameService");
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 5;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (...)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CORBA_GENERICERROR);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 6;
		}

		for (ulServerIndex = 0; ulServerIndex < _ulServersNumber; ulServerIndex++)
		{
			try
			{
				CosNaming:: Name			name;
				name. length (1);
				name [0]. id			= CORBA::string_dup ((_pssiStreamingServerInfo [ulServerIndex]). _pName);
				name [0]. kind			= CORBA::string_dup ("");
	  
				pssStreamingServer		=
					::StreamingIDL:: StreamingServer:: _narrow (pnc -> resolve (name));

				if (pssStreamingServer -> deactivate () != 0)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVER_DEACTIVATE_FAILED,
						1, ulServerIndex + 1);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
						__FILE__, __LINE__);

					return 7;
					// throw ::StreamingIDL:: StreamingExc ((const char *) err);
				}

				(_pssiStreamingServerInfo [ulServerIndex]). _bIsActive		= false;
			}
			catch (::StreamingIDL:: StreamingExc &)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_STREAMINGSERVERERROR,
					1, "deactivate");
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 8;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
			catch (CORBA::SystemException &)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_STREAMINGSERVERERROR,
					1, "deactivate");
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 9;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
			catch (...)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_CORBA_GENERICERROR);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 10;
			}
		}


		return 0;
	}


	CORBA:: Long StreamingServersManager:: shutdown ()

	{

		CosNaming:: NamingContext_var			pnc;
		::StreamingIDL:: StreamingServer_var	pssStreamingServer;
		unsigned long							ulServerIndex;

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_SHUTDOWN);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		try
		{
			CORBA::Object_var nsobj		=
				_porb -> resolve_initial_references ("NameService");

			pnc							=
				CosNaming::NamingContext::_narrow (nsobj);

			if (CORBA::is_nil (pnc))
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_REGISTERTONAMINGSERVICE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 1;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
		}
		catch (CosNaming::NamingContext::NotFound &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTNOTFOUND);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 2;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CosNaming::NamingContext::CannotProceed &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTCANNOTPROCEED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 3;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CosNaming::NamingContext::InvalidName &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTINVALIDNAME);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 4;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CORBA::SystemException &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTGENERICERROR,
				1, "NameService");
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 5;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (...)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CORBA_GENERICERROR);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 6;
		}

		for (ulServerIndex = 0; ulServerIndex < _ulServersNumber; ulServerIndex++)
		{
			try
			{
				CosNaming:: Name			name;
				name. length (1);
				name [0]. id			= CORBA::string_dup ((_pssiStreamingServerInfo [ulServerIndex]). _pName);
				name [0]. kind			= CORBA::string_dup ("");
	  
				pssStreamingServer		=
					::StreamingIDL:: StreamingServer:: _narrow (pnc -> resolve (name));

				if (pssStreamingServer -> shutdown () != 0)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVER_SHUTDOWN_FAILED,
						1, ulServerIndex + 1);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
						__FILE__, __LINE__);

					return 7;
					// throw ::StreamingIDL:: StreamingExc ((const char *) err);
				}
			}
			catch (::StreamingIDL:: StreamingExc &)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_STREAMINGSERVERERROR,
					1, "shutdown");
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 8;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
			catch (CORBA::SystemException &)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_STREAMINGSERVERERROR,
					1, "shutdown");
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 9;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
			catch (...)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_CORBA_GENERICERROR);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 10;
			}
		}

		try
		{
			CORBA::Object_var			poaobj;
			PortableServer::POA_var		poa;

			// POA initialization
			poaobj			= _porb -> resolve_initial_references ("RootPOA");
			poa				= PortableServer:: POA:: _narrow (poaobj);

			poa -> deactivate_object (*_poid);
			_porb -> shutdown (FALSE);
		}
		catch (CORBA::SystemException &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_SHUTDOWNCORBAFAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 11;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err); 
		}
		catch (...)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CORBA_GENERICERROR);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 12;
		}

		if (setIsShutdown (true) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_SETISSHUTDOWN_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 13;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err); 
		}


		return 0;
	}


	CORBA:: Long StreamingServersManager:: setChangedConfiguration (
		const ::StreamingIDL:: ConfigurationItemsList &
		cilConfigurationItemsList)

	{
		long				lChangedConfigurationItemsNumber;
		long				lChangedConfigurationItemIndex;


		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_SETCHANGEDCONFIGURATION);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}


		lChangedConfigurationItemsNumber		= cilConfigurationItemsList. length ();

		if (lChangedConfigurationItemsNumber > 0)
		{
			for (lChangedConfigurationItemIndex = 0;
				lChangedConfigurationItemIndex < lChangedConfigurationItemsNumber;
				lChangedConfigurationItemIndex++)
			{
				if (_pcfConfiguration -> modifyItemValue (cilConfigurationItemsList [
					lChangedConfigurationItemIndex]. pSectionName,
					cilConfigurationItemsList [
					lChangedConfigurationItemIndex]. pItemName,
					cilConfigurationItemsList [
					lChangedConfigurationItemIndex]. pItemValue) !=
					errNoError)
				{
					Error err = ConfigurationErrors (__FILE__, __LINE__,
						CFG_CONFIG_MODIFYITEMVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
						__FILE__, __LINE__);

					return 1;
					// throw ::StreamingIDL:: StreamingExc ((const char *) err); 
				}
			}

			if (_pcfConfiguration -> save () != errNoError)
			{
				Error err = ConfigurationFileErrors (__FILE__, __LINE__,
					CFGFILE_CONFIGURATIONFILE_SAVE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 2;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err); 
			}
		}


		return 0;
	}


	CORBA:: Long StreamingServersManager:: getConfiguration (
		::StreamingIDL:: ConfigurationItemsWithInfoList_out
		cilConfigurationItemsWithInfoList)

	{

		long					lSectionsNumber;
		long					lSectionIndex;
		ConfigurationSection_t	csCfgSection;
		long					lItemsNumber;
		long					lItemIndex;
		ConfigurationItem_t		ciCfgItem;
		char					pSectionName [SS_MAXSECTIONNAMELENGTH];
		char					pItemComment [SS_MAXITEMCOMMENTLENGTH];
		char					pItemName [SS_MAXITEMNAMELENGTH];
		char					pItemValue [SS_MAXITEMVALUELENGTH];
		Error_t					errGetCfgItem;
		::StreamingIDL:: ConfigurationItemsWithInfoList
			cilLocalConfigurationItemsWithInfoList (SS_MAXCONFIGURATIONITEMS);



		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_GETCONFIGURATION);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		cilLocalConfigurationItemsWithInfoList. length (0);

		if (_pcfConfiguration -> getSectionsNumber (&lSectionsNumber) !=
			errNoError)
		{
			Error err = ConfigurationErrors (__FILE__, __LINE__,
				CFG_CONFIG_GETSECTIONSNUMBER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 1;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err); 
		}

		for (lSectionIndex = 0; lSectionIndex < lSectionsNumber;
			lSectionIndex++)
		{
			if (_pcfConfiguration -> getCfgSectionByIndex (
				lSectionIndex, &csCfgSection) != errNoError)
			{
				Error err = ConfigurationErrors (__FILE__, __LINE__,
					CFG_CONFIG_GETCFGSECTIONBYINDEX_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return 2;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}

			if (csCfgSection. getSectionName (pSectionName) != errNoError)
			{
				Error err = ConfigurationErrors (__FILE__, __LINE__,
					CFG_CONFIGURATIONSECTION_GETSECTIONNAME_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return 3;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}

			if (_pcfConfiguration -> getItemsNumber (lSectionIndex,
				&lItemsNumber) != errNoError)
			{
				Error err = ConfigurationErrors (__FILE__, __LINE__,
					CFG_CONFIG_GETITEMSNUMBER_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return 4;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}

			if (cilLocalConfigurationItemsWithInfoList. length () + lItemsNumber >
				SS_MAXCONFIGURATIONITEMS)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_LISTTOOLONG);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return 5;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}

			cilLocalConfigurationItemsWithInfoList. length (
				cilLocalConfigurationItemsWithInfoList. length () + lItemsNumber);

			for (lItemIndex = 0; lItemIndex < lItemsNumber; lItemIndex++)
			{
				if ((errGetCfgItem = _pcfConfiguration -> getCfgItemByIndex (
					lSectionIndex, lItemIndex, &ciCfgItem)) != errNoError)
				{
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) errGetCfgItem, __FILE__, __LINE__);

					Error err = ConfigurationErrors (__FILE__, __LINE__,
						CFG_CONFIG_GETCFGITEMBYINDEX_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return 6;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				if (ciCfgItem. getItemComment (pItemComment) != errNoError)
				{
					Error err = ConfigurationErrors (__FILE__, __LINE__,
						CFG_CONFIGURATIONITEM_GETITEMCOMMENT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return 7;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				if (ciCfgItem. getItemName (pItemName) != errNoError)
				{
					Error err = ConfigurationErrors (__FILE__, __LINE__,
						CFG_CONFIGURATIONITEM_GETITEMNAME_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return 8;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				if (!strcmp (pSectionName, "StreamingServersManager") &&
					!strcmp (pItemName, "Name"))
				{
					cilLocalConfigurationItemsWithInfoList. length (
						cilLocalConfigurationItemsWithInfoList. length () - 1);

					if (ciCfgItem. finish () != errNoError)
					{
						Error err = ConfigurationErrors (__FILE__, __LINE__,
							CFG_CONFIGURATIONITEM_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return 9;
						// throw ::StreamingIDL:: StreamingExc (
						//	(const char *) err);
					}

					continue;
				}

				if (ciCfgItem. getItemValue (pItemValue) != errNoError)
				{
					Error err = ConfigurationErrors (__FILE__, __LINE__,
						CFG_CONFIGURATIONITEM_GETITEMVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return 10;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				cilLocalConfigurationItemsWithInfoList [
					cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
					lItemIndex]. ciConfigurationItem. pSectionName					=
					CORBA:: string_dup (pSectionName);
				cilLocalConfigurationItemsWithInfoList [
					cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
					lItemIndex]. ciConfigurationItem. pItemComment						=
					CORBA:: string_dup (pItemComment);
				cilLocalConfigurationItemsWithInfoList [
					cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
					lItemIndex]. ciConfigurationItem. pItemName						=
					CORBA:: string_dup (pItemName);
				cilLocalConfigurationItemsWithInfoList [
					cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
					lItemIndex]. ciConfigurationItem. pItemValue					=
					CORBA:: string_dup (pItemValue);

				cilLocalConfigurationItemsWithInfoList [
					cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
					lItemIndex]. itItemType						=
					::StreamingIDL:: NUMBER;

				if ((!strcmp (pSectionName, "IPConfiguration") &&
					!strcmp (pItemName, "LocalIPAddressForRTSP")) ||
					(!strcmp (pSectionName, "IPConfiguration") &&
					!strcmp (pItemName, "LocalIPAddressForRTP")) ||
					(!strcmp (pSectionName, "IPConfiguration") &&
					!strcmp (pItemName, "LocalIPAddressForRTCP")))
				{
					std:: vector<char *>:: const_iterator		it;
					char										*pNIC;
					unsigned long								ulNICsNumber;

					cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. itItemType						=
						::StreamingIDL:: ENUMERATIVE;

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues). length (0);

					for (it = _vNICs. begin (), ulNICsNumber = 0; it != _vNICs. end ();
						++it, ulNICsNumber++)
					{
						pNIC				= *it;

						(cilLocalConfigurationItemsWithInfoList [
							cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
							lItemIndex]. pValues). length (ulNICsNumber + 1);

						(cilLocalConfigurationItemsWithInfoList [
							cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
							lItemIndex]. pValues) [ulNICsNumber]		=
							CORBA:: string_dup (pNIC);
					}

					cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pSelectionMode					=
						CORBA:: string_dup ("SINGLE");
				}
				else if (
					(!strcmp (pSectionName, "Logs") &&
					!strcmp (pItemName, "CompressedTraceFile")) ||
					(!strcmp (pSectionName, "Logs") &&
					!strcmp (pItemName, "TraceOnTTY")) ||
					(!strcmp (pSectionName, "System") &&
					!strcmp (pItemName, "UseOfHintingTrackIfExist")) ||
					(!strcmp (pSectionName, "System") &&
					!strcmp (pItemName, "BuildOfHintingTrackIfNotExist"))
					)
				{
					cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. itItemType						=
						::StreamingIDL:: ENUMERATIVE;

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues). length (2);

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues) [0]		=
						CORBA:: string_dup ("false");

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues) [1]		=
						CORBA:: string_dup ("true");

					cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pSelectionMode					=
						CORBA:: string_dup ("SINGLE");
				}
				else if ((!strcmp (pSectionName, "Logs") &&
					!strcmp (pItemName, "TraceLevel")))
				{
					cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. itItemType						=
						::StreamingIDL:: ENUMERATIVE;

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues). length (11);

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues) [0]		=
						CORBA:: string_dup ("LDBG1");

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues) [1]		=
						CORBA:: string_dup ("LDBG2");

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues) [2]		=
						CORBA:: string_dup ("LDBG3");

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues) [3]		=
						CORBA:: string_dup ("LDBG4");

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues) [4]		=
						CORBA:: string_dup ("LDBG5");

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues) [5]		=
						CORBA:: string_dup ("LDBG6");

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues) [6]		=
						CORBA:: string_dup ("LINFO");

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues) [7]		=
						CORBA:: string_dup ("LMESG");

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues) [8]		=
						CORBA:: string_dup ("LWNRG");

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues) [9]		=
						CORBA:: string_dup ("LERRR");

					(cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pValues) [10]		=
						CORBA:: string_dup ("LFTAL");

					cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. pSelectionMode					=
						CORBA:: string_dup ("SINGLE");
				}
				else if ((!strcmp (pSectionName, "System") &&
					!strcmp (pItemName, "ContentRootPath")) ||
					(!strcmp (pSectionName, "Logs") &&
					!strcmp (pItemName, "BaseTraceFileName")))
				{
					cilLocalConfigurationItemsWithInfoList [
						cilLocalConfigurationItemsWithInfoList.length () - lItemsNumber +
						lItemIndex]. itItemType						=
						::StreamingIDL:: TEXT;
				}
				else
				{
				}

				if (ciCfgItem. finish () != errNoError)
				{
					Error err = ConfigurationErrors (__FILE__, __LINE__,
						CFG_CONFIGURATIONITEM_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return 11;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}
			}

			if (csCfgSection. finish () != errNoError)
			{
				Error err = ConfigurationErrors (__FILE__, __LINE__,
					CFG_CONFIGURATIONSECTION_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return 12;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
		}

		cilConfigurationItemsWithInfoList			= new 
			::StreamingIDL:: ConfigurationItemsWithInfoList (
			cilLocalConfigurationItemsWithInfoList);


		return 0;
	}


	CORBA:: Long StreamingServersManager:: getHostName (
		CORBA:: String_out pHostName)

	{

		pHostName				= CORBA:: string_dup (_pHostName);


		return 0;
	}


/*
	CORBA:: Long StreamingServersManager:: getNICs (
		::StreamingIDL:: StringsList_out lsNICs)
//		throw (::StreamingIDL:: StreamingExc)

	{
		::StreamingIDL:: StringsList			slStringsList (SS_MAXNICS);

		slStringsList. length (0);

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_GETNICS);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		#ifdef WIN32
			struct addrinfo				aiAddrInfo;
			struct addrinfo				aiAddrInfoHints;
			struct sockaddr_in			*psaSockAddr;
			struct addrinfo				*paiNextAddrInfo;

			aiAddrInfoHints. ai_family			= AF_INET;
			aiAddrInfoHints. ai_socktype		= 0;
			aiAddrInfoHints. ai_protocol		= 0;
			aiAddrInfoHints. ai_addrlen			= 0;
			aiAddrInfoHints. ai_canonname		= (char *) NULL;
			aiAddrInfoHints. ai_addr			= (struct sockaddr *) NULL;
			aiAddrInfoHints. ai_next			= (struct addrinfo *) NULL;

			if (getaddrinfo(_pHostName,
				(const char FAR *) NULL,
				&aiAddrInfoHints,
				&aiAddrInfo))
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_GETADDRINFO_FAILED,
					1, WSAGetLastError ());
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return 1;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}

			paiNextAddrInfo			= &aiAddrInfo;

			while (paiNextAddrInfo != (struct addrinfo  *) NULL)
			{
				psaSockAddr			= paiNextAddrInfo -> ai_addr;

				if (slStringsList. length () + 1 > SS_MAXNICS)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERSMANAGER_LISTTOOLONG);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return 2;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				slStringsList. length (slStringsList. length () + 1);
				slStringsList [slStringsList.length () - 1]		=
					CORBA:: string_dup (psaSockAddr -> sin_addr);

				paiNextAddrInfo		= paiNextAddrInfo -> ai_next;
			}
		#else
			struct hostent							*phHostent;
			struct in_addr							*piaInAddr;

			if ((phHostent = gethostbyname (_pHostName)) == (hostent *) NULL)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_WRONGHOSTNAME,
					1, _pHostName);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return 3;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}

			switch (phHostent -> h_addrtype)
			{
				case AF_INET:
					while ((piaInAddr =
						(struct in_addr *) *(phHostent -> h_addr_list)++) !=
						(struct in_addr *) NULL)
					{
						if (slStringsList. length () + 1 > SS_MAXNICS)
						{
							Error err = StreamingServerErrors (__FILE__, __LINE__,
								SS_STREAMINGSERVERSMANAGER_LISTTOOLONG);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return 4;
							// throw ::StreamingIDL:: StreamingExc (
							//	(const char *) err);
						}

						slStringsList. length (slStringsList. length () + 1);
						slStringsList [slStringsList.length () - 1]		=
							CORBA:: string_dup (inet_ntoa (*piaInAddr));
					}

					break;
				default:
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_STREAMINGSERVERSMANAGER_UNKNOWNADDRESSTYPE,
							1, (long) phHostent -> h_addrtype);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return 5;
						// throw ::StreamingIDL:: StreamingExc (
						//	(const char *) err);
					}

					break;
			}
		#endif


		lsNICs			= new ::StreamingIDL:: StringsList (slStringsList);


		return 0;
	}
*/


	CORBA:: Long StreamingServersManager:: getContents (
		const char *pDirectory,
		::StreamingIDL:: StringsList_out lsFileNames,
		::StreamingIDL:: StringsList_out lsDirectories)

	{

		::StreamingIDL:: StringsList		lsLocalFileNames (SS_MAXFILES);
		::StreamingIDL:: StringsList		lsLocalDirectories (
			SS_MAXDIRECTORIES);
		FileIO:: Directory_t				dDirectory;
		Error_t								errReadDirectory;
		Buffer_t							bDirectoryEntry;
		Boolean_t							bIsDirectory;
		Buffer_t							bDirectory;
		// Buffer_t							bDirectoryEntry;


		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_GETCONTENTS,
				1, pDirectory);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		lsLocalFileNames. length (0);
		lsLocalDirectories. length (0);

		if (bDirectory. init (_pContentRootPath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 1;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bDirectory. append ("/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bDirectory. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 2;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bDirectory. append (pDirectory) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bDirectory. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 3;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bDirectoryEntry. init () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bDirectory. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 4;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (FileIO:: openDirectory ((const char *) bDirectory,
			&dDirectory) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_OPENDIRECTORY_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bDirectoryEntry. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bDirectory. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 5;
			// throw ::StreamingIDL:: StreamingExc (
			//	(const char *) err);
		}

		while ((errReadDirectory = FileIO:: readDirectory (&dDirectory,
			&bDirectoryEntry, &bIsDirectory)) == errNoError)
		{
			if (bIsDirectory)
			{
				if (lsLocalDirectories. length () + 1 > SS_MAXDIRECTORIES)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERSMANAGER_LISTTOOLONG);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (FileIO:: closeDirectory (&dDirectory) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_FILEIO_CLOSEDIRECTORY_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bDirectoryEntry. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bDirectory. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return 6;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				lsLocalDirectories. length (lsLocalDirectories. length () + 1);
				lsLocalDirectories [lsLocalDirectories. length () - 1]		=
					CORBA:: string_dup ((const char *) bDirectoryEntry);
			}
			else
			{
				if (lsLocalFileNames. length () + 1 > SS_MAXFILES)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERSMANAGER_LISTTOOLONG);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (FileIO:: closeDirectory (&dDirectory) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_FILEIO_CLOSEDIRECTORY_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bDirectoryEntry. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bDirectory. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return 7;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				lsLocalFileNames. length (lsLocalFileNames. length () + 1);
				lsLocalFileNames [lsLocalFileNames. length () - 1]		=
					CORBA:: string_dup ((const char *) bDirectoryEntry);
			}
		}

		if ((long) errReadDirectory != TOOLS_FILEIO_DIRECTORYFILESFINISHED)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_READDIRECTORY_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (FileIO:: closeDirectory (&dDirectory) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_CLOSEDIRECTORY_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bDirectoryEntry. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bDirectory. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 8;
			// throw ::StreamingIDL:: StreamingExc (
			//	(const char *) err);
		}

		if (FileIO:: closeDirectory (&dDirectory) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_CLOSEDIRECTORY_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bDirectoryEntry. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bDirectory. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 9;
			// throw ::StreamingIDL:: StreamingExc (
			//	(const char *) err);
		}

		if (bDirectoryEntry. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bDirectory. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 10;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 11;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		lsDirectories		= new ::StreamingIDL:: StringsList (
			lsLocalDirectories);
		lsFileNames			= new ::StreamingIDL:: StringsList (
			lsLocalFileNames);


		return 0;
	}


	CORBA:: Long StreamingServersManager:: addDirectory (
		const char *pDirectoryName)

	{

		Buffer_t							bDirectory;
		int									iMode;
		Error_t								errCreateDirectory;

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_ADDDIRECTORY,
				1, pDirectoryName);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (bDirectory. init (_pContentRootPath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 1;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bDirectory. append ("/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bDirectory. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 2;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bDirectory. append (pDirectoryName) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bDirectory. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 3;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_DIRECTORYTOCREATE,
				1, (const char *) bDirectory);
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		#ifdef WIN32
			iMode		= 0;
		#else
			iMode		= S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
		#endif

		if ((errCreateDirectory = FileIO:: createDirectory (
			(const char *) bDirectory, iMode)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errCreateDirectory, __FILE__, __LINE__);

			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_CREATEDIRECTORY_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bDirectory. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 4;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 5;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}


		return 0;
	}


	CORBA:: Long StreamingServersManager:: deleteDirectory (
		const char *pDirectoryName)

	{

		Buffer_t							bDirectory;
		Error_t								errRemoveDirectory;

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_DELETEDIRECTORY,
				1, pDirectoryName);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (bDirectory. init (_pContentRootPath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 1;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bDirectory. append ("/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bDirectory. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 2;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bDirectory. append (pDirectoryName) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bDirectory. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 3;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_DIRECTORYTODELETE,
				1, (const char *) bDirectory);
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		if ((errRemoveDirectory = FileIO:: removeDirectory (
			(const char *) bDirectory)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errRemoveDirectory, __FILE__, __LINE__);

			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_REMOVEDIRECTORY_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bDirectory. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 4;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 5;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}


		return 0;
	}


	CORBA:: Long StreamingServersManager:: addContent (
		const char *pContentName, const ::StreamingIDL:: BytesList &
		blContent)

	{

		int							iFileDescriptor;
		unsigned char				*pucContent;
		unsigned long				ulContentLength;
		unsigned long				ulContentIndex;
		Buffer_t					bContentPathName;


		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_ADDCONTENT,
				1, pContentName);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (bContentPathName. init (_pContentRootPath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 1;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. append ("/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 2;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. append (pContentName) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 3;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		ulContentLength				= blContent. length ();

		if ((pucContent = new unsigned char [ulContentLength]) ==
			(unsigned char *) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 4;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		for (ulContentIndex = 0; ulContentIndex < ulContentLength;
			ulContentIndex++)
			pucContent [ulContentIndex]			= blContent [ulContentIndex];

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CONTENTTOCREATE,
				1, (const char *) bContentPathName);
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		#ifdef WIN32
			if (FileIO:: open ((const char *) bContentPathName,
				O_WRONLY | O_TRUNC | O_CREAT,
				_S_IREAD | _S_IWRITE, &iFileDescriptor) !=
				errNoError)
		#else
			if (FileIO:: open ((const char *) bContentPathName,
				O_WRONLY | O_TRUNC | O_CREAT,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
				&iFileDescriptor) != errNoError)
		#endif
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_OPEN_FAILED,
				1, (const char *) bContentPathName);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] pucContent;

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 5;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (FileIO:: writeBytes (iFileDescriptor,
			pucContent, ulContentLength) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITEBYTES_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (FileIO:: close (iFileDescriptor) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_CLOSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] pucContent;

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 6;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (FileIO:: close (iFileDescriptor) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_CLOSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] pucContent;

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 7;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		delete [] pucContent;

		if (bContentPathName. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 8;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}


		return 0;
	}


	CORBA:: Long StreamingServersManager:: deleteContent (
		const char *pContentName)

	{

		Buffer_t					bContentPathName;


		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_DELETECONTENT,
				1, pContentName);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (bContentPathName. init (_pContentRootPath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 1;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. append ("/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 2;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. append (pContentName) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 3;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CONTENTTODELETE,
				1, (const char *) bContentPathName);
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (FileIO:: remove ((const char *) bContentPathName) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_REMOVE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 4;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 5;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}


		return 0;
	}


	CORBA:: Long StreamingServersManager:: dumpContent (
		const char *pContentName, CORBA:: String_out pDump)

	{

		Buffer_t					bContentPathName;
		MP4File_p					pmp4File;
		MP4Atom:: Standard_t		sStandard;
		Boolean_t					bIsMP4FileInCache;
		Buffer_t					bDump;


		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_DUMPCONTENT,
				1, pContentName);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (bContentPathName. init (_pContentRootPath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 1;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. append ("/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 2;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. append (pContentName) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 3;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bDump. init ("", -1, 100000) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 4;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CONTENTTODUMP,
				1, (const char *) bContentPathName);
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (_mp4fMP4FileFactory. getMP4File (
			(const char *) bContentPathName,
			false, &pmp4File, &sStandard,
			&bIsMP4FileInCache) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bDump. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 5;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bIsMP4FileInCache)
		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_MP4FILEINCACHE,
				2, (const char *) bContentPathName, "true");
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}
		else
		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_MP4FILEINCACHE,
				2, (const char *) bContentPathName, "false");
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (pmp4File -> getDump (&bDump) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mp4fMP4FileFactory. releaseMP4File (
				(const char *) bContentPathName) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bDump. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 6;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (_mp4fMP4FileFactory. releaseMP4File (
			(const char *) bContentPathName) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bDump. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 7;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		pDump				= CORBA:: string_dup ((const char *) bDump);

		if (bDump. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 8;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 9;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}


		return 0;
	}


	CORBA:: Long StreamingServersManager:: getTracksInfo (
		const char *pContentName, CORBA:: String_out pTracksInfo)

	{

		Buffer_t						bContentPathName;
		MP4File_p						pmp4File;
		MP4Atom:: Standard_t			sStandard;
		Boolean_t						bIsMP4FileInCache;
		Buffer_t						bTracksInfo;
		char							pCodecName [MP4F_MAXCODECUSEDLENGTH];
		std:: vector<MP4Utility:: MP4TrackInfo_t>	vMP4TracksInfo;


		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_GETTRACKSINFO,
				1, pContentName);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (bContentPathName. init (_pContentRootPath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 1;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. append ("/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 2;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. append (pContentName) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 3;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bTracksInfo. init ("", -1, 10000) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 4;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CONTENTTOHAVETRACKSINFO,
				1, (const char *) bContentPathName);
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (_mp4fMP4FileFactory. getMP4File (
			(const char *) bContentPathName,
			false, &pmp4File, &sStandard,
			&bIsMP4FileInCache) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bTracksInfo. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 5;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bIsMP4FileInCache)
		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_MP4FILEINCACHE,
				2, (const char *) bContentPathName, "true");
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}
		else
		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_MP4FILEINCACHE,
				2, (const char *) bContentPathName, "false");
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (pmp4File -> getTracksInfo (vMP4TracksInfo) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_GETTRACKSINFO_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mp4fMP4FileFactory. releaseMP4File (
				(const char *) bContentPathName) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTracksInfo. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 6;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bTracksInfo. setBuffer (
			"\tTrack Identifier\t\tHandler Type\t\tCodec used\t\tTrack Name\n") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mp4fMP4FileFactory. releaseMP4File (
				(const char *) bContentPathName) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTracksInfo. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 7;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (vMP4TracksInfo. size () > 0)
		{
			std:: vector<MP4Utility:: MP4TrackInfo_t>:: const_iterator	it;
			MP4Utility:: MP4TrackInfo_t						mtiMP4TrackInfo;


			for (it = vMP4TracksInfo. begin (); it != vMP4TracksInfo. end ();
				++it)
			{
				mtiMP4TrackInfo				= *it;

				if (bTracksInfo. append ("\t") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mp4fMP4FileFactory. releaseMP4File (
						(const char *) bContentPathName) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bTracksInfo. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bContentPathName. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return 8;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				if (bTracksInfo. append (
					mtiMP4TrackInfo. _ulTrackIdentifier) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mp4fMP4FileFactory. releaseMP4File (
						(const char *) bContentPathName) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bTracksInfo. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bContentPathName. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return 9;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				if (bTracksInfo. append ("\t\t\t\t") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mp4fMP4FileFactory. releaseMP4File (
						(const char *) bContentPathName) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bTracksInfo. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bContentPathName. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return 10;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				if (bTracksInfo. append (mtiMP4TrackInfo. _pHandlerType) !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mp4fMP4FileFactory. releaseMP4File (
						(const char *) bContentPathName) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bTracksInfo. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bContentPathName. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return 11;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				if (bTracksInfo. append ("\t\t\t") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mp4fMP4FileFactory. releaseMP4File (
						(const char *) bContentPathName) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bTracksInfo. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bContentPathName. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return 12;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				if (MP4Utility:: getCodecName (mtiMP4TrackInfo. _cCodecUsed,
					pCodecName) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UTILITY_GETCODECNAME_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mp4fMP4FileFactory. releaseMP4File (
						(const char *) bContentPathName) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bTracksInfo. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bContentPathName. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return 13;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				if (bTracksInfo. append (pCodecName) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mp4fMP4FileFactory. releaseMP4File (
						(const char *) bContentPathName) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bTracksInfo. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bContentPathName. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return 13;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				if (bTracksInfo. append ("\t\t\t") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mp4fMP4FileFactory. releaseMP4File (
						(const char *) bContentPathName) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bTracksInfo. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bContentPathName. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return 14;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				if (bTracksInfo. append (mtiMP4TrackInfo. _pTrackName) !=
					errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mp4fMP4FileFactory. releaseMP4File (
						(const char *) bContentPathName) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bTracksInfo. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bContentPathName. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return 15;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}

				if (bTracksInfo. append ("\n") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mp4fMP4FileFactory. releaseMP4File (
						(const char *) bContentPathName) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bTracksInfo. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bContentPathName. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return 16;
					// throw ::StreamingIDL:: StreamingExc (
					//	(const char *) err);
				}
			}
		}

		if (_mp4fMP4FileFactory. releaseMP4File (
			(const char *) bContentPathName) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bTracksInfo. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 17;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		pTracksInfo			= CORBA:: string_dup ((const char *) bTracksInfo);

		if (bTracksInfo. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 18;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 19;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}


		return 0;
	}


	CORBA:: Long StreamingServersManager:: getServerConnectedUsers (
		::StreamingIDL:: ServerConnectedUsersList_out
		scuServerConnectedUsersList)

	{

		::StreamingIDL:: ServerConnectedUsersList
			scuLocalConnectedUsersList (_ulMaxRTSPServerSessions);

		::StreamingIDL:: ServerConnectedUsersList_var
			scuLocalServerConnectedUsersList;

		CosNaming:: NamingContext_var			pnc;
		::StreamingIDL:: StreamingServer_var	pssStreamingServer;
		unsigned long							ulServerIndex;
		unsigned long							ulConnectedUsersIndex;

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_GETSERVERCONNECTEDUSERS);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		try
		{
			CORBA::Object_var nsobj		=
				_porb -> resolve_initial_references ("NameService");

			pnc							=
				CosNaming::NamingContext::_narrow (nsobj);

			if (CORBA::is_nil (pnc))
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_REGISTERTONAMINGSERVICE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 1;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
		}
		catch (CosNaming::NamingContext::NotFound &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTNOTFOUND);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 2;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CosNaming::NamingContext::CannotProceed &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTCANNOTPROCEED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 3;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CosNaming::NamingContext::InvalidName &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTINVALIDNAME);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 4;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CORBA::SystemException &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTGENERICERROR,
				1, "NameService");
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 5;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (...)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CORBA_GENERICERROR);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 6;
		}

		scuLocalConnectedUsersList. length (0);

		for (ulServerIndex = 0; ulServerIndex < _ulServersNumber; ulServerIndex++)
		{
			try
			{
				CosNaming:: Name			name;
				name. length (1);
				name [0]. id			= CORBA::string_dup ((_pssiStreamingServerInfo [ulServerIndex]). _pName);
				name [0]. kind			= CORBA::string_dup ("");
	  
				pssStreamingServer		=
					::StreamingIDL:: StreamingServer:: _narrow (pnc -> resolve (name));

				if (pssStreamingServer -> getServerConnectedUsers (
					scuLocalServerConnectedUsersList. out ()) != 0)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVER_GETSERVERCONNECTEDUSERS_FAILED,
						1, ulServerIndex + 1);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
						__FILE__, __LINE__);

					return 7;
					// throw ::StreamingIDL:: StreamingExc ((const char *) err);
				}
			}
			catch (::StreamingIDL:: StreamingExc &)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_STREAMINGSERVERERROR,
					1, "getServerConnectedUsers");
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 8;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
			catch (CORBA::SystemException &)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_STREAMINGSERVERERROR,
					1, "getServerConnectedUsers");
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 9;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
			catch (...)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_CORBA_GENERICERROR);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 10;
			}

			scuLocalConnectedUsersList. length (
				scuLocalConnectedUsersList. length () +
				scuLocalServerConnectedUsersList -> length ());

			for (ulConnectedUsersIndex = 0; ulConnectedUsersIndex <
				scuLocalServerConnectedUsersList -> length ();
				ulConnectedUsersIndex++)
			{
				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. pClientIPAddress			=
					CORBA:: string_dup (scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. pClientIPAddress);

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulTotalBitRate				=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulTotalBitRate;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ullTotalBytesSent			=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ullTotalBytesSent;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulTotalLostPacketsNumber	=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulTotalLostPacketsNumber;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulTotalSentPacketsNumber	=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulTotalSentPacketsNumber;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ullTimeConnectedInSeconds	=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ullTimeConnectedInSeconds;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. pURL						=
					CORBA:: string_dup (scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. pURL);

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulTimesNumberStreamed		=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulTimesNumberStreamed;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulServerRTSPPort			=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulServerRTSPPort;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulServerRTPAudioPort		=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulServerRTPAudioPort;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulServerRTCPAudioPort		=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulServerRTCPAudioPort;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulServerRTPVideoPort		=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulServerRTPVideoPort;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulServerRTCPVideoPort		=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulServerRTCPVideoPort;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulClientRTSPPort			=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulClientRTSPPort;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulClientRTPAudioPort		=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulClientRTPAudioPort;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulClientRTCPAudioPort		=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulClientRTCPAudioPort;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulClientRTPVideoPort		=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulClientRTPVideoPort;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulClientRTCPVideoPort		=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulClientRTCPVideoPort;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulAudioBitRate				=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulAudioBitRate;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. pAudioCodec					=
					CORBA:: string_dup (scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. pAudioCodec);

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulVideoBitRate				=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulVideoBitRate;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. pVideoCodec					=
					CORBA:: string_dup (scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. pVideoCodec);

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ullAudioBytesSent			=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ullAudioBytesSent;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ullVideoBytesSent			=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ullVideoBytesSent;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulAudioPacketsNumberLost	=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulAudioPacketsNumberLost;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulVideoPacketsNumberLost	=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulVideoPacketsNumberLost;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulAudioPacketsNumberSent	=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulAudioPacketsNumberSent;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulVideoPacketsNumberSent	=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulVideoPacketsNumberSent;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulAudioPacketsNumberPrefetched	=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulAudioPacketsNumberPrefetched;

				scuLocalConnectedUsersList [
					scuLocalConnectedUsersList. length () -
					scuLocalServerConnectedUsersList -> length () +
					ulConnectedUsersIndex]. ulVideoPacketsNumberPrefetched	=
					scuLocalServerConnectedUsersList [
					ulConnectedUsersIndex]. ulVideoPacketsNumberPrefetched;
			}
		}

		scuServerConnectedUsersList			= new 
			::StreamingIDL:: ServerConnectedUsersList (
			scuLocalConnectedUsersList);


		return 0;
	}


	CORBA:: Long StreamingServersManager:: getServerInfo (
		::StreamingIDL:: ServerInfo_out siServerInfo)

	{

		::StreamingIDL:: ServerInfo				siLocalInfo;
		::StreamingIDL:: ServerInfo_var			siLocalServerInfo;

		CosNaming:: NamingContext_var			pnc;
		::StreamingIDL:: StreamingServer_var	pssStreamingServer;
		unsigned long							ulServerIndex;
		GetCpuUsage_t							gcuGetCpuUsage;


		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_CORBA_GETSERVERINFO);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		try
		{
			CORBA::Object_var nsobj		=
				_porb -> resolve_initial_references ("NameService");

			pnc							=
				CosNaming::NamingContext::_narrow (nsobj);

			if (CORBA::is_nil (pnc))
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_REGISTERTONAMINGSERVICE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 1;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
		}
		catch (CosNaming::NamingContext::NotFound &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTNOTFOUND);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 2;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CosNaming::NamingContext::CannotProceed &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTCANNOTPROCEED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 3;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CosNaming::NamingContext::InvalidName &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTINVALIDNAME);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 4;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (CORBA::SystemException &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTGENERICERROR,
				1, "NameService");
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 5;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}
		catch (...)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CORBA_GENERICERROR);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 6;
		}

		siLocalInfo. ssStatus				= ::StreamingIDL:: STARTED;
		siLocalInfo. ulUpTimeInMinutes	= (time (NULL) - _tServersManagerStartTime) / 60;
		siLocalInfo. pServerVersion		= CORBA:: string_dup (".....");
		siLocalInfo. ulCPUUsage			= gcuGetCpuUsage. getCpuUsage ();
		siLocalInfo. ulMemoryUsage		= 0;	// da calcolare

		siLocalInfo. ulPlayersNumberConnected	= 0;
		siLocalInfo. ulBandWidthUsageInbps		= 0;
		siLocalInfo. ullTotalBytesServed		= 0;
		siLocalInfo. ullTotalLostPacketsNumber	= 0;
		siLocalInfo. ullTotalSentPacketsNumber	= 0;

		for (ulServerIndex = 0; ulServerIndex < _ulServersNumber; ulServerIndex++)
		{
			if (!((_pssiStreamingServerInfo [ulServerIndex]). _bIsActive))
				continue;

			try
			{
				CosNaming:: Name			name;
				name. length (1);
				name [0]. id			= CORBA::string_dup ((_pssiStreamingServerInfo [ulServerIndex]). _pName);
				name [0]. kind			= CORBA::string_dup ("");
	  
				pssStreamingServer		=
					::StreamingIDL:: StreamingServer:: _narrow (pnc -> resolve (name));

				if (pssStreamingServer -> getServerInfo (
					siLocalServerInfo. out ()) != 0)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVER_GETSERVERINFO_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
						__FILE__, __LINE__);

					return 7;
					// throw ::StreamingIDL:: StreamingExc ((const char *) err);
				}
			}
			catch (::StreamingIDL:: StreamingExc &)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_STREAMINGSERVERERROR,
					1, "getServerInfo");
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 7;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
			catch (CORBA::SystemException &)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERSMANAGER_STREAMINGSERVERERROR,
					1, "getServerInfo");
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 8;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
			catch (...)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_CORBA_GENERICERROR);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 9;
			}

			siLocalInfo. ulPlayersNumberConnected	+=
				siLocalServerInfo -> ulPlayersNumberConnected;
			siLocalInfo. ulBandWidthUsageInbps		+=
				siLocalServerInfo -> ulBandWidthUsageInbps;
			siLocalInfo. ullTotalBytesServed		+=
				siLocalServerInfo -> ullTotalBytesServed;
			siLocalInfo. ullTotalLostPacketsNumber	+=
				siLocalServerInfo -> ullTotalLostPacketsNumber;
			siLocalInfo. ullTotalSentPacketsNumber	+=
				siLocalServerInfo -> ullTotalSentPacketsNumber;
		}

		siServerInfo			= new ::StreamingIDL:: ServerInfo (
			siLocalInfo);


		return 0;
	}

#endif
