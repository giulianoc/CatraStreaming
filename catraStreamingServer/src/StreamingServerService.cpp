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


#ifdef WIN32
#else
	#include "CatraStreamingConfig.h"
#endif
#include "GetCpuUsage.h"
#include "StreamingServerService.h"
#include "SubscriberTracer.h"
#include "DateTime.h"
#include "StreamingServerMessages.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "FileIO.h"
#include "System.h"
#include <stdlib.h>
#include <assert.h>
#ifdef WIN32
	// #include <Winsock2.h>
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
#include <signal.h>


void signalHandler (int iSignal);

Tracer_p						_gptSystemTracer			=
	(Tracer_p) NULL;
SubscriberTracer_p				_gptSubscriberTracer		=
	(SubscriberTracer_p) NULL;
StreamingServerService_p		_gpsssStreamingServerService	=
	(StreamingServerService_p) NULL;

#define TOOLS_SERVICE_DEBUGFILE			"./StreamingServerService.log"

void signalHandler (int iSignal)

{

	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_COREDUMP, 1, iSignal);
		_gptSystemTracer -> trace (Tracer:: TRACER_LFTAL,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_gptSystemTracer != (Tracer_p) NULL)
	{
		if (_gptSystemTracer -> flushOfTraces () != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_FLUSHOFTRACES_FAILED);	
			std:: cerr << (const char *) err << std:: endl;

			// return 1;
		}
	}

	if (_gptSubscriberTracer != (Tracer_p) NULL)
	{
		if (_gptSubscriberTracer -> flushOfTraces () != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_FLUSHOFTRACES_FAILED);	
			std:: cerr << (const char *) err << std:: endl;

			// return 1;
		}
	}

	if (_gpsssStreamingServerService != (StreamingServerService_p) NULL)
	{
		if (_gpsssStreamingServerService -> cancel () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_CANCEL_FAILED);
			_gptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}
	}

	if (_gptSystemTracer != (Tracer_p) NULL)
	{
		if (_gptSystemTracer -> flushOfTraces () != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_FLUSHOFTRACES_FAILED);	
			std:: cerr << (const char *) err << std:: endl;

			// return 1;
		}
	}

	if (_gptSubscriberTracer != (Tracer_p) NULL)
	{
		if (_gptSubscriberTracer -> flushOfTraces () != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_FLUSHOFTRACES_FAILED);	
			std:: cerr << (const char *) err << std:: endl;

			// return 1;
		}
	}


	exit (iSignal);
	// return 0;
}


StreamingServerService:: StreamingServerService (void):
	Service ()

{

}


StreamingServerService:: ~StreamingServerService (void)

{

}


Error StreamingServerService:: init (void)

{

	Error_t					errServiceInit;


	#ifdef WIN32
		if ((errServiceInit = Service:: init (
			SS_STREAMINGSERVERSERVICE_SERVICENAME,
			SS_STREAMINGSERVERSERVICE_SERVICEDESCRIPTION,
			(const char *) NULL, (const char *) NULL)) != errNoError)
	#else
		if ((errServiceInit = Service:: init (
			SS_STREAMINGSERVERSERVICE_SERVICENAME,
			SS_STREAMINGSERVERSERVICE_SERVICEDESCRIPTION)) != errNoError)
	#endif
	{
		/*
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errServiceInit, __FILE__, __LINE__);

		Error err = StreamingServerErrors (__FILE__, __LINE__,
			TOOLS_SERVICE_INIT_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/

		return errServiceInit;
	}


	return errNoError;
}


Error StreamingServerService:: finish (void)

{

	Error_t					errServiceFinish;


	if ((errServiceFinish = Service:: finish ()) != errNoError)
	{
		/*
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errServiceFinish, __FILE__, __LINE__);

		Error err = StreamingServerErrors (__FILE__, __LINE__,
			TOOLS_SERVICE_FINISH_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/

		return errServiceFinish;
	}


	return errNoError;
}


Error StreamingServerService:: onInit (void)

{

	Error_t					errStartTracer;
	char					*pConfigurationPathName;
	char					pConfigurationBuffer [SS_MAXLONGLENGTH];
	Error_t					errGetItemValue;
	unsigned long			ulMaxLiveSourcesNumber;


	if ((pConfigurationPathName =
		getenv (SS_STREAMINGSERVERSERVICE_CONFPATHNAMEENVIRONMENTVARIABLE)) ==
		(char *) NULL)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ENVIRONMENTVARIABLE_NOTDEFINED,
			1, SS_STREAMINGSERVERSERVICE_CONFPATHNAMEENVIRONMENTVARIABLE);

		return err;
	}

	{
		Error								errParseError;

		if ((errParseError = _cfConfiguration. init (
			pConfigurationPathName,
			"Streaming server configuration")) != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_INIT_FAILED);

			return errParseError;
		}
	}

	if ((errStartTracer = startTracer (&_cfConfiguration, &_tSystemTracer,
		"SystemLogs")) != errNoError)
	{
		// Error err = StreamingServerErrors (__FILE__, __LINE__,
		// 	SS_STARTTRACER_FAILED);
		// std:: cerr << (const char *) err << std:: endl;

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return errStartTracer;
	}

	if ((errStartTracer = startTracer (&_cfConfiguration, &_tSubscriberTracer,
		"SubscriberLogs")) != errNoError)
	{
		// Error err = StreamingServerErrors (__FILE__, __LINE__,
		// 	SS_STARTTRACER_FAILED);
		// std:: cerr << (const char *) err << std:: endl;

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return errStartTracer;
	}

	_gptSystemTracer						= &_tSystemTracer;
	_gptSubscriberTracer					= &_tSubscriberTracer;
	_gpsssStreamingServerService			= this;

	signal(SIGSEGV, signalHandler);
	signal(SIGINT, signalHandler);
	signal(SIGABRT, signalHandler);
	// signal(SIGBUS, signalHandler);

	/*
	if (checkLicense (&_cfConfiguration, &_tSystemTracer) !=
		errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_CHECKLICENSE_FAILED);
		tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}
	*/

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_INITIALIZING);
		_tSystemTracer. trace (
			Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (System:: getHostName (_pHostName,
		SS_STREAMINGSERVERSERVICE_MAXHOSTNAMELENGTH) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_SYSTEM_GETHOSTNAME_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	if ((errGetItemValue = _cfConfiguration. getItemValue ("StreamingServer",
		"Standard", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "StreamingServer", "Standard");
		_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	if (!strcmp (pConfigurationBuffer, "ISMA"))
		_sStandard			= MP4Atom:: MP4F_ISMA;
	else if (!strcmp (pConfigurationBuffer, "3GPP"))
		_sStandard			= MP4Atom:: MP4F_3GPP;
	else
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_CONFIGITEMWRONG,
			2, "StreamingServer", "Standard");
		_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	if (StreamingServerService:: onInitMP4FileFactory (
		&_cfConfiguration,
		&_mp4fMP4FileFactory,
		&_tSystemTracer) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_ONINITMP4FILEFACTORY_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	if ((errGetItemValue = _cfConfiguration. getItemValue ("StreamingServer",
		"MaxRTSPSessions", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "StreamingServer", "MaxRTSPSessions");
		_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}
	_ulMaxRTSPServerSessions			= strtoul (pConfigurationBuffer,
		(char **) NULL, 10);

	if ((errGetItemValue = _cfConfiguration. getItemValue ("System",
		"MaxLiveSourcesNumber", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "MaxLiveSourcesNumber");
		_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}
	ulMaxLiveSourcesNumber		= strtoul (pConfigurationBuffer,
		(char **) NULL, 10);

	{
		if ((_plsLiveSources = new LiveSource_t [ulMaxLiveSourcesNumber]) ==
			(LiveSource_p) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_NEW_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (stopTracer (&_tSubscriberTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (stopTracer (&_tSystemTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (_cfConfiguration. finish () != errNoError)
			{
				// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			return err;
		}
	}

	{
		long			lStreamingServerProcessorsNumber;

		if ((errGetItemValue = _cfConfiguration. getItemValue ("System",
			"StreamingServerProcessorsNumber", pConfigurationBuffer,
			SS_MAXLONGLENGTH)) != errNoError)
		{
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGetItemValue,
				__FILE__, __LINE__);

			Error err = ConfigurationErrors (__FILE__, __LINE__,
				CFG_CONFIG_GETITEMVALUE_FAILED,
				2, "System", "StreamingServerProcessorsNumber");
			_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] _plsLiveSources;
			_plsLiveSources		= (LiveSource_p) NULL;

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (stopTracer (&_tSubscriberTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (stopTracer (&_tSystemTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (_cfConfiguration. finish () != errNoError)
			{
				// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			return err;
		}
		lStreamingServerProcessorsNumber			=
			atol (pConfigurationBuffer);
		if (lStreamingServerProcessorsNumber <= 0)
			// da calcolare secondo il n.ro dei processori della macchina
			_ulStreamingServerProcessorsNumber		= 1;
		else
			_ulStreamingServerProcessorsNumber		=
				lStreamingServerProcessorsNumber;
	}

	if ((errGetItemValue = _cfConfiguration. getItemValue ("Scheduler",
		"SchedulerSleepTimeInMilliSecs", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "Scheduler", "SchedulerSleepTimeInMilliSecs");
		_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}
	_ulSchedulerSleepTimeInMilliSecs		= strtoul (pConfigurationBuffer,
		(char **) NULL, 10);

	if ((errGetItemValue = _cfConfiguration. getItemValue ("Scheduler",
		"CheckServerSocketPeriodInMilliSecs", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "Scheduler", "CheckServerSocketPeriodInMilliSecs");
		_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}
	_ulCheckServerSocketPeriodInMilliSecs		= strtoul (pConfigurationBuffer,
		(char **) NULL, 10);

	if ((errGetItemValue = _cfConfiguration. getItemValue ("Scheduler",
		"CheckSocketsPoolPeriodInMilliSecs", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "Scheduler", "CheckSocketsPoolPeriodInMilliSecs");
		_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}
	_ulCheckSocketsPoolPeriodInMilliSecs		= strtoul (pConfigurationBuffer,
		(char **) NULL, 10);

	_vFreeRTSPSessions. reserve (_ulMaxRTSPServerSessions);


	// EventsSet
	{
		if (_esEventsSet. init (
			StreamingServerEventsSet:: SS_EVENTTYPENUMBER,
			&_tSystemTracer) != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] _plsLiveSources;
			_plsLiveSources		= (LiveSource_p) NULL;

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (stopTracer (&_tSubscriberTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (stopTracer (&_tSystemTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (_cfConfiguration. finish () != errNoError)
			{
				// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			return err;
		}
	}

	// SocketsPool
	if (_spStreamingServerSocketsPool. init (&_esEventsSet,
		&_tSystemTracer) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETSPOOL_INIT_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_esEventsSet. finish () != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	if (_mtLiveSources. init (
		PMutex:: MUTEX_RECURSIVE) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_INIT_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_spStreamingServerSocketsPool. finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_esEventsSet. finish () != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	if ((errGetItemValue = _cfConfiguration. getItemValue (
		"StreamingServer",
		"ContentRootPath", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "StreamingServer", "ContentRootPath");
		_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_mtLiveSources. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_spStreamingServerSocketsPool. finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_esEventsSet. finish () != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	if (_bContentRootPath. init (pConfigurationBuffer) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_mtLiveSources. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_spStreamingServerSocketsPool. finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_esEventsSet. finish () != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	if ((_prsRTSPSessions = new RTSPSession_t [
		_ulMaxRTSPServerSessions]) == (RTSPSession_p) NULL)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_NEW_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_bContentRootPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (_mtLiveSources. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_spStreamingServerSocketsPool. finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_esEventsSet. finish () != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	if (StreamingServerService:: onInitRTSPSessions (
		&_cfConfiguration,
		(const char *) _bContentRootPath,
		_sStandard,
		&_ulMaxRTSPServerSessions,
		&ulMaxLiveSourcesNumber,
		&_esEventsSet,
		&_spStreamingServerSocketsPool,
		&_scScheduler,
		&_mtLiveSources,
		_plsLiveSources,
		&_mp4fMP4FileFactory,
		&_tSystemTracer,
		&_tSubscriberTracer,
		_prsRTSPSessions,	// OUT ...
		_pLocalIPAddressForRTSP,
		&_vFreeRTSPSessions,
		&_ulFileCacheSizeInBytes,
		&_ulRTSPRequestsPort) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_ONINITRTSPSESSIONS_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] _prsRTSPSessions;
		_prsRTSPSessions			= (RTSPSession_p) NULL;

		if (_bContentRootPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (_mtLiveSources. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_spStreamingServerSocketsPool. finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_esEventsSet. finish () != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

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
		_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);
			return err;
	}
	*/

	if (_mtStreamingServerStatistics. init (
		PMutex:: MUTEX_RECURSIVE) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_FINISH_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		_vFreeRTSPSessions. clear ();

		{
			unsigned long			ulRTSPSessionIndex;

			for (ulRTSPSessionIndex = 0;
				ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
				ulRTSPSessionIndex++)
			{
				if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
					errNoError)
				{
					Error err = StreamingServerErrors (
						__FILE__, __LINE__,
						SS_RTSPSESSION_FINISH_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		delete [] _prsRTSPSessions;
		_prsRTSPSessions			= (RTSPSession_p) NULL;

		if (_bContentRootPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtLiveSources. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_spStreamingServerSocketsPool. finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_esEventsSet. finish () != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	if (_mtFreeRTSPSessions. init (
		PMutex:: MUTEX_RECURSIVE) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_INIT_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtStreamingServerStatistics. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		_vFreeRTSPSessions. clear ();

		{
			unsigned long			ulRTSPSessionIndex;

			for (ulRTSPSessionIndex = 0;
				ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
				ulRTSPSessionIndex++)
			{
				if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
					errNoError)
				{
					Error err = StreamingServerErrors (
						__FILE__, __LINE__,
						SS_RTSPSESSION_FINISH_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		delete [] _prsRTSPSessions;
		_prsRTSPSessions			= (RTSPSession_p) NULL;

		if (_bContentRootPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtLiveSources. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_spStreamingServerSocketsPool. finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_esEventsSet. finish () != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	// server socket init
	{
		long					lMaxClients;
		Error_t					errServerSocketInit;
		SocketImpl_p			pServerSocketImpl;

		lMaxClients			= _ulMaxRTSPServerSessions;


		if ((errServerSocketInit = _ssServerSocket. init (
			_pLocalIPAddressForRTSP, _ulRTSPRequestsPort, true,
			SocketImpl:: STREAM, 0, 0, lMaxClients)) !=
			errNoError)
		{
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errServerSocketInit,
				__FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtFreeRTSPSessions. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtStreamingServerStatistics. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_vFreeRTSPSessions. clear ();

			{
				unsigned long			ulRTSPSessionIndex;

				for (ulRTSPSessionIndex = 0;
					ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
					ulRTSPSessionIndex++)
				{
					if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
						errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}

			delete [] _prsRTSPSessions;
			_prsRTSPSessions			= (RTSPSession_p) NULL;

			if (_bContentRootPath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtLiveSources. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_spStreamingServerSocketsPool. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_esEventsSet. finish () != errNoError)
			{
				Error err = EventsSetErrors (__FILE__, __LINE__,
					EVSET_EVENTSSET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] _plsLiveSources;
			_plsLiveSources		= (LiveSource_p) NULL;

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (stopTracer (&_tSubscriberTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (stopTracer (&_tSystemTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (_cfConfiguration. finish () != errNoError)
			{
				// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			return errServerSocketInit;
		}

		if (_ssServerSocket. getSocketImpl (&pServerSocketImpl) != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKET_GETSOCKETIMPL_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_ssServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtFreeRTSPSessions. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtStreamingServerStatistics. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_vFreeRTSPSessions. clear ();

			{
				unsigned long			ulRTSPSessionIndex;

				for (ulRTSPSessionIndex = 0;
					ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
					ulRTSPSessionIndex++)
				{
					if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
						errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}

			delete [] _prsRTSPSessions;
			_prsRTSPSessions			= (RTSPSession_p) NULL;

			if (_bContentRootPath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtLiveSources. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_spStreamingServerSocketsPool. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_esEventsSet. finish () != errNoError)
			{
				Error err = EventsSetErrors (__FILE__, __LINE__,
					EVSET_EVENTSSET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] _plsLiveSources;
			_plsLiveSources		= (LiveSource_p) NULL;

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (stopTracer (&_tSubscriberTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (stopTracer (&_tSystemTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (_cfConfiguration. finish () != errNoError)
			{
				// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			return err;
		}

		if (pServerSocketImpl -> setBlocking (false) != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETIMPL_SETBLOCKING_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_ssServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtFreeRTSPSessions. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtStreamingServerStatistics. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_vFreeRTSPSessions. clear ();

			{
				unsigned long			ulRTSPSessionIndex;

				for (ulRTSPSessionIndex = 0;
					ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
					ulRTSPSessionIndex++)
				{
					if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
						errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}

			delete [] _prsRTSPSessions;
			_prsRTSPSessions			= (RTSPSession_p) NULL;

			if (_bContentRootPath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtLiveSources. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_spStreamingServerSocketsPool. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_esEventsSet. finish () != errNoError)
			{
				Error err = EventsSetErrors (__FILE__, __LINE__,
					EVSET_EVENTSSET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] _plsLiveSources;
			_plsLiveSources		= (LiveSource_p) NULL;

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (stopTracer (&_tSubscriberTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (stopTracer (&_tSystemTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (_cfConfiguration. finish () != errNoError)
			{
				// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			return err;
		}

		if (pServerSocketImpl -> setMaxReceiveBuffer (32 * 1024) !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETIMPL_SETMAXRECEIVEBUFFER_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_ssServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtFreeRTSPSessions. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtStreamingServerStatistics. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_vFreeRTSPSessions. clear ();

			{
				unsigned long			ulRTSPSessionIndex;

				for (ulRTSPSessionIndex = 0;
					ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
					ulRTSPSessionIndex++)
				{
					if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
						errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}

			delete [] _prsRTSPSessions;
			_prsRTSPSessions			= (RTSPSession_p) NULL;

			if (_bContentRootPath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtLiveSources. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_spStreamingServerSocketsPool. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_esEventsSet. finish () != errNoError)
			{
				Error err = EventsSetErrors (__FILE__, __LINE__,
					EVSET_EVENTSSET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] _plsLiveSources;
			_plsLiveSources		= (LiveSource_p) NULL;

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (stopTracer (&_tSubscriberTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (stopTracer (&_tSystemTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (_cfConfiguration. finish () != errNoError)
			{
				// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			return err;
		}
	}

	// initialization of the processor
	{
		char					pLocalIPAddressForRTP [
			SCK_MAXIPADDRESSLENGTH];

		if ((errGetItemValue = _cfConfiguration. getItemValue (
			"IPConfiguration",
			"LocalIPAddressForRTP", pLocalIPAddressForRTP,
			SCK_MAXIPADDRESSLENGTH)) != errNoError)
		{
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGetItemValue,
				__FILE__, __LINE__);

			Error err = ConfigurationErrors (__FILE__, __LINE__,
				CFG_CONFIG_GETITEMVALUE_FAILED,
				2, "IPConfiguration", "LocalIPAddressForRTP");
			_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (_ssServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtFreeRTSPSessions. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtStreamingServerStatistics. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_vFreeRTSPSessions. clear ();

			{
				unsigned long			ulRTSPSessionIndex;

				for (ulRTSPSessionIndex = 0;
					ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
					ulRTSPSessionIndex++)
				{
					if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
						errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}

			delete [] _prsRTSPSessions;
			_prsRTSPSessions			= (RTSPSession_p) NULL;

			if (_bContentRootPath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtLiveSources. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_spStreamingServerSocketsPool. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_esEventsSet. finish () != errNoError)
			{
				Error err = EventsSetErrors (__FILE__, __LINE__,
					EVSET_EVENTSSET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] _plsLiveSources;
			_plsLiveSources		= (LiveSource_p) NULL;

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (stopTracer (&_tSubscriberTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (stopTracer (&_tSystemTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (_cfConfiguration. finish () != errNoError)
			{
				// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			return err;
		}

		// initialization processors
		if ((_pspStreamingServerProcessor = new StreamingServerProcessor_t [
			_ulStreamingServerProcessorsNumber]) ==
			(StreamingServerProcessor_p) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_NEW_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (_ssServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtFreeRTSPSessions. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtStreamingServerStatistics. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_vFreeRTSPSessions. clear ();

			{
				unsigned long			ulRTSPSessionIndex;

				for (ulRTSPSessionIndex = 0;
					ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
					ulRTSPSessionIndex++)
				{
					if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
						errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}

			delete [] _prsRTSPSessions;
			_prsRTSPSessions			= (RTSPSession_p) NULL;

			if (_bContentRootPath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtLiveSources. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_spStreamingServerSocketsPool. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_esEventsSet. finish () != errNoError)
			{
				Error err = EventsSetErrors (__FILE__, __LINE__,
					EVSET_EVENTSSET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] _plsLiveSources;
			_plsLiveSources		= (LiveSource_p) NULL;

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (stopTracer (&_tSubscriberTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (stopTracer (&_tSystemTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (_cfConfiguration. finish () != errNoError)
			{
				// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			return err;
		}

		{
			long				lProcessorIdentifier;

			_ullTotalBytesServed				= 0;
			_ullTotalLostPacketsNumber			= 0;
			_ullTotalSentPacketsNumber			= 0;

			for (lProcessorIdentifier = 0;
				lProcessorIdentifier < _ulStreamingServerProcessorsNumber;
				lProcessorIdentifier++)
			{
				if ((_pspStreamingServerProcessor [lProcessorIdentifier]).
					init (
					lProcessorIdentifier,
					_sStandard,
					&_esEventsSet,
					&_ssServerSocket,
					&_spStreamingServerSocketsPool,
					_prsRTSPSessions,
					_ulMaxRTSPServerSessions,
					&_mtFreeRTSPSessions,
					&_vFreeRTSPSessions,
					pLocalIPAddressForRTP,
					&_mtStreamingServerStatistics,
					&_mtLiveSources,
					&_ullTotalBytesServed,
					&_ullTotalLostPacketsNumber,
					&_ullTotalSentPacketsNumber,
					&_cfConfiguration,
					&_tSystemTracer,
					&_tSubscriberTracer) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_INIT_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err,
						__FILE__, __LINE__);

					while (--lProcessorIdentifier >= 0)
					{
						if ((_pspStreamingServerProcessor [lProcessorIdentifier]).
							finish () != errNoError)
						{
							Error err = StreamingServerErrors (__FILE__, __LINE__,
								SS_STREAMINGSERVERPROCESSOR_INIT_FAILED);
							_tSystemTracer. trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}
					}

					delete [] _pspStreamingServerProcessor;

					if (_ssServerSocket. finish () != errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SERVERSOCKET_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtFreeRTSPSessions. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtStreamingServerStatistics. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					_vFreeRTSPSessions. clear ();

					{
						unsigned long			ulRTSPSessionIndex;

						for (ulRTSPSessionIndex = 0;
							ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
							ulRTSPSessionIndex++)
						{
							if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
								errNoError)
							{
								Error err = StreamingServerErrors (
									__FILE__, __LINE__,
									SS_RTSPSESSION_FINISH_FAILED);
								_tSystemTracer. trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}
						}
					}

					delete [] _prsRTSPSessions;
					_prsRTSPSessions			= (RTSPSession_p) NULL;

					if (_bContentRootPath. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtLiveSources. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_spStreamingServerSocketsPool. finish () !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SOCKETSPOOL_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_esEventsSet. finish () != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENTSSET_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete [] _plsLiveSources;
					_plsLiveSources		= (LiveSource_p) NULL;

					if (_mp4fMP4FileFactory. finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILEFACTORY_INIT_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (stopTracer (&_tSubscriberTracer) != errNoError)
					{
						// Error err = StreamingServerErrors (
						//	__FILE__, __LINE__,
						// 	SS_STOPTRACER_FAILED);
						// std:: cerr << (const char *) err << std:: endl;
					}

					if (stopTracer (&_tSystemTracer) != errNoError)
					{
						// Error err = StreamingServerErrors (
						//	__FILE__, __LINE__,
						// 	SS_STOPTRACER_FAILED);
						// std:: cerr << (const char *) err << std:: endl;
					}

					if (_cfConfiguration. finish () != errNoError)
					{
						// Error err = ConfigurationFileErrors (
						//	__FILE__, __LINE__,
						// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
						// std:: cerr << (const char *) err << std:: endl;
					}

					return err;
				}
			}
		}
	}

	// initialization of the scheduler
	if (_scScheduler. init (
		_ulSchedulerSleepTimeInMilliSecs) != errNoError)
	{
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_SCHEDULER_INIT_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err,
			__FILE__, __LINE__);

		{
			unsigned long			ulProcessorIdentifier;

			for (ulProcessorIdentifier = 0; ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
				ulProcessorIdentifier++)
			{
				if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
					finish () != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_INIT_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		delete [] _pspStreamingServerProcessor;

		if (_ssServerSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtFreeRTSPSessions. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtStreamingServerStatistics. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		_vFreeRTSPSessions. clear ();

		{
			unsigned long			ulRTSPSessionIndex;

			for (ulRTSPSessionIndex = 0;
				ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
				ulRTSPSessionIndex++)
			{
				if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
					errNoError)
				{
					Error err = StreamingServerErrors (
						__FILE__, __LINE__,
						SS_RTSPSESSION_FINISH_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		delete [] _prsRTSPSessions;
		_prsRTSPSessions			= (RTSPSession_p) NULL;

		if (_bContentRootPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtLiveSources. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_spStreamingServerSocketsPool. finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_esEventsSet. finish () != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	if (_cspCheckSocketsPoolTimes. init (
		_ulCheckSocketsPoolPeriodInMilliSecs,
		&_spStreamingServerSocketsPool,
		&_tSystemTracer) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_CHECKSOCKETSPOOLTIMES_INIT_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		// bDestroyTimes
		if (_scScheduler. finish (false) != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_SCHEDULER_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err,
				__FILE__, __LINE__);
		}

		{
			unsigned long			ulProcessorIdentifier;

			for (ulProcessorIdentifier = 0;
				ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
				ulProcessorIdentifier++)
			{
				if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
					finish () != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_INIT_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		delete [] _pspStreamingServerProcessor;

		if (_ssServerSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtFreeRTSPSessions. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtStreamingServerStatistics. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		_vFreeRTSPSessions. clear ();

		{
			unsigned long			ulRTSPSessionIndex;

			for (ulRTSPSessionIndex = 0;
				ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
				ulRTSPSessionIndex++)
			{
				if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
					errNoError)
				{
					Error err = StreamingServerErrors (
						__FILE__, __LINE__,
						SS_RTSPSESSION_FINISH_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		delete [] _prsRTSPSessions;
		_prsRTSPSessions			= (RTSPSession_p) NULL;

		if (_bContentRootPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtLiveSources. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_spStreamingServerSocketsPool. finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_esEventsSet. finish () != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	if (_cssCheckServerSocketTimes. init (
		_ulCheckServerSocketPeriodInMilliSecs, &_esEventsSet,
		&_tSystemTracer) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_CHECKSERVERSOCKETTIMES_INIT_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_cspCheckSocketsPoolTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CHECKSOCKETSPOOLTIMES_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		// bDestroyTimes
		if (_scScheduler. finish (false) != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_SCHEDULER_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err,
				__FILE__, __LINE__);
		}

		{
			unsigned long			ulProcessorIdentifier;

			for (ulProcessorIdentifier = 0; ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
				ulProcessorIdentifier++)
			{
				if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
					finish () != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_INIT_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		delete [] _pspStreamingServerProcessor;

		if (_ssServerSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtFreeRTSPSessions. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtStreamingServerStatistics. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		_vFreeRTSPSessions. clear ();

		{
			unsigned long			ulRTSPSessionIndex;

			for (ulRTSPSessionIndex = 0;
				ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
				ulRTSPSessionIndex++)
			{
				if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
					errNoError)
				{
					Error err = StreamingServerErrors (
						__FILE__, __LINE__,
						SS_RTSPSESSION_FINISH_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		delete [] _prsRTSPSessions;
		_prsRTSPSessions			= (RTSPSession_p) NULL;

		if (_bContentRootPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtLiveSources. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_spStreamingServerSocketsPool. finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_esEventsSet. finish () != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	// initialization of the HttpStreamingServer
	{
		if ((errGetItemValue = _cfConfiguration. getItemValue (
			"IPConfiguration",
			"LocalIPAddressForHTTP", _pLocalIPAddressForHTTP,
			SCK_MAXIPADDRESSLENGTH)) != errNoError)
		{
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGetItemValue,
				__FILE__, __LINE__);

			Error err = ConfigurationErrors (__FILE__, __LINE__,
				CFG_CONFIG_GETITEMVALUE_FAILED,
				2, "IPConfiguration", "LocalIPAddressForHTTP");
			_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (_cssCheckServerSocketTimes. finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_CHECKSERVERSOCKETTIMES_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_cspCheckSocketsPoolTimes. finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_CHECKSOCKETSPOOLTIMES_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			// bDestroyTimes
			if (_scScheduler. finish (false) != errNoError)
			{
				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_SCHEDULER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err,
					__FILE__, __LINE__);
			}

			{
				unsigned long			ulProcessorIdentifier;

				for (ulProcessorIdentifier = 0;
					ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
					ulProcessorIdentifier++)
				{
					if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
						finish () != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_STREAMINGSERVERPROCESSOR_INIT_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}

			delete [] _pspStreamingServerProcessor;

			if (_ssServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtFreeRTSPSessions. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtStreamingServerStatistics. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_vFreeRTSPSessions. clear ();

			{
				unsigned long			ulRTSPSessionIndex;

				for (ulRTSPSessionIndex = 0;
					ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
					ulRTSPSessionIndex++)
				{
					if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
						errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}

			delete [] _prsRTSPSessions;
			_prsRTSPSessions			= (RTSPSession_p) NULL;

			if (_bContentRootPath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtLiveSources. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_spStreamingServerSocketsPool. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_esEventsSet. finish () != errNoError)
			{
				Error err = EventsSetErrors (__FILE__, __LINE__,
					EVSET_EVENTSSET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] _plsLiveSources;
			_plsLiveSources		= (LiveSource_p) NULL;

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (stopTracer (&_tSubscriberTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (stopTracer (&_tSystemTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (_cfConfiguration. finish () != errNoError)
			{
				// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			return err;
		}

		if ((errGetItemValue = _cfConfiguration. getItemValue (
			"IPConfiguration",
			"HTTPRequestsPort", pConfigurationBuffer,
			SS_MAXLONGLENGTH)) != errNoError)
		{
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGetItemValue,
				__FILE__, __LINE__);

			Error err = ConfigurationErrors (__FILE__, __LINE__,
				CFG_CONFIG_GETITEMVALUE_FAILED,
				2, "IPConfiguration", "HTTPRequestsPort");
			_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (_cssCheckServerSocketTimes. finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_CHECKSERVERSOCKETTIMES_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_cspCheckSocketsPoolTimes. finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_CHECKSOCKETSPOOLTIMES_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			// bDestroyTimes
			if (_scScheduler. finish (false) != errNoError)
			{
				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_SCHEDULER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err,
					__FILE__, __LINE__);
			}

			{
				unsigned long			ulProcessorIdentifier;

				for (ulProcessorIdentifier = 0;
					ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
					ulProcessorIdentifier++)
				{
					if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
						finish () != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_STREAMINGSERVERPROCESSOR_INIT_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}

			delete [] _pspStreamingServerProcessor;

			if (_ssServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtFreeRTSPSessions. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtStreamingServerStatistics. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_vFreeRTSPSessions. clear ();

			{
				unsigned long			ulRTSPSessionIndex;

				for (ulRTSPSessionIndex = 0;
					ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
					ulRTSPSessionIndex++)
				{
					if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
						errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}

			delete [] _prsRTSPSessions;
			_prsRTSPSessions			= (RTSPSession_p) NULL;

			if (_bContentRootPath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtLiveSources. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_spStreamingServerSocketsPool. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_esEventsSet. finish () != errNoError)
			{
				Error err = EventsSetErrors (__FILE__, __LINE__,
					EVSET_EVENTSSET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] _plsLiveSources;
			_plsLiveSources		= (LiveSource_p) NULL;

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (stopTracer (&_tSubscriberTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (stopTracer (&_tSystemTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (_cfConfiguration. finish () != errNoError)
			{
				// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			return err;
		}
		_ulHTTPRequestsPort			=
			(unsigned long) atol (pConfigurationBuffer);

		if (_hssHttpStreamingServer. init (
			_pLocalIPAddressForHTTP, _ulHTTPRequestsPort,
			(const char *) _bContentRootPath,
			&_mp4fMP4FileFactory,
			_sStandard,
			&_cfConfiguration,
			_ulFileCacheSizeInBytes,
			SS_STREAMINGSERVERSERVICE_HTTPSERVERRECEIVINGTIMEOUTINSECS,
			0,
			SS_STREAMINGSERVERSERVICE_HTTPSERVERSENDINGTIMEOUTINSECS,
			0,
			SS_STREAMINGSERVERSERVICE_MAXHTTPREQUESTS,
			_ulMaxRTSPServerSessions,
			_prsRTSPSessions,
			&_cssCheckServerSocketTimes,
			&_mtFreeRTSPSessions,
			&_vFreeRTSPSessions,
			&_mtStreamingServerStatistics,
			&_ullTotalBytesServed,
			&_ullTotalLostPacketsNumber,
			&_ullTotalSentPacketsNumber,
			&_tSystemTracer) != errNoError)
		{
			Error err = WebToolsErrors (__FILE__, __LINE__,
				WEBTOOLS_HTTPSERVER_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err,
				__FILE__, __LINE__);

			if (_cssCheckServerSocketTimes. finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_CHECKSERVERSOCKETTIMES_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_cspCheckSocketsPoolTimes. finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_CHECKSOCKETSPOOLTIMES_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			// bDestroyTimes
			if (_scScheduler. finish (false) != errNoError)
			{
				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_SCHEDULER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err,
					__FILE__, __LINE__);
			}

			{
				unsigned long			ulProcessorIdentifier;

				for (ulProcessorIdentifier = 0;
					ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
					ulProcessorIdentifier++)
				{
					if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
						finish () != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_STREAMINGSERVERPROCESSOR_INIT_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}

			delete [] _pspStreamingServerProcessor;

			if (_ssServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtFreeRTSPSessions. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtStreamingServerStatistics. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_vFreeRTSPSessions. clear ();

			{
				unsigned long			ulRTSPSessionIndex;

				for (ulRTSPSessionIndex = 0;
					ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
					ulRTSPSessionIndex++)
				{
					if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
						errNoError)
					{
						Error err = StreamingServerErrors (
							__FILE__, __LINE__,
							SS_RTSPSESSION_FINISH_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}
			}

			delete [] _prsRTSPSessions;
			_prsRTSPSessions			= (RTSPSession_p) NULL;

			if (_bContentRootPath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtLiveSources. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_spStreamingServerSocketsPool. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_esEventsSet. finish () != errNoError)
			{
				Error err = EventsSetErrors (__FILE__, __LINE__,
					EVSET_EVENTSSET_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] _plsLiveSources;
			_plsLiveSources		= (LiveSource_p) NULL;

			if (_mp4fMP4FileFactory. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_INIT_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (stopTracer (&_tSubscriberTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (stopTracer (&_tSystemTracer) != errNoError)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				// 	SS_STOPTRACER_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			if (_cfConfiguration. finish () != errNoError)
			{
				// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
				// std:: cerr << (const char *) err << std:: endl;
			}

			return err;
		}
	}

	if (initNICs (&_vIPAddresses) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVER_INITNICS_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_hssHttpStreamingServer. finish () != errNoError)
		{
			Error err = WebToolsErrors (__FILE__, __LINE__,
				WEBTOOLS_HTTPSERVER_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err,
				__FILE__, __LINE__);
		}

		if (_cssCheckServerSocketTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CHECKSERVERSOCKETTIMES_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_cspCheckSocketsPoolTimes. finish () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CHECKSOCKETSPOOLTIMES_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		// bDestroyTimes
		if (_scScheduler. finish (false) != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_SCHEDULER_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err,
				__FILE__, __LINE__);
		}

		{
			unsigned long			ulProcessorIdentifier;

			for (ulProcessorIdentifier = 0; ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
				ulProcessorIdentifier++)
			{
				if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
					finish () != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_INIT_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		delete [] _pspStreamingServerProcessor;

		if (_ssServerSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtFreeRTSPSessions. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtStreamingServerStatistics. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		_vFreeRTSPSessions. clear ();

		{
			unsigned long			ulRTSPSessionIndex;

			for (ulRTSPSessionIndex = 0;
				ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
				ulRTSPSessionIndex++)
			{
				if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
					errNoError)
				{
					Error err = StreamingServerErrors (
						__FILE__, __LINE__,
						SS_RTSPSESSION_FINISH_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
		}

		delete [] _prsRTSPSessions;
		_prsRTSPSessions			= (RTSPSession_p) NULL;

		if (_bContentRootPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtLiveSources. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_spStreamingServerSocketsPool. finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_esEventsSet. finish () != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete [] _plsLiveSources;
		_plsLiveSources		= (LiveSource_p) NULL;

		if (_mp4fMP4FileFactory. finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stopTracer (&_tSubscriberTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (stopTracer (&_tSystemTracer) != errNoError)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			// 	SS_STOPTRACER_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		if (_cfConfiguration. finish () != errNoError)
		{
			// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_INITIALIZED);
		_tSystemTracer. trace (
			Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}


	return errNoError;
}


Error StreamingServerService:: onInitMP4FileFactory (
	ConfigurationFile_p pcfConfiguration,
	MP4FileFactory_p pmp4fMP4FileFactory,
	Tracer_p ptSystemTracer)

{

	Error_t					errGetItemValue;
	char					pConfigurationBuffer [SS_MAXLONGLENGTH];
	unsigned long			ulMaxMp4FilesNumberInMemory;
	unsigned long			ulMp4FilesNumberToDeleteOnOverflow;
	Boolean_t				bUseMP4ConsistencyCheck;
	unsigned long			ulBucketsNumberForMP4FilesCache;


	if ((errGetItemValue = pcfConfiguration -> getItemValue ("Cache",
		"MaxMp4FilesNumberInMemory", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "Cache", "MaxMp4FilesNumberInMemory");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulMaxMp4FilesNumberInMemory			= 
		strtoul (pConfigurationBuffer, (char **) NULL, 10);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("Cache",
		"Mp4FilesNumberToDeleteOnOverflow",
		pConfigurationBuffer, SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "Cache", "MaxMp4FilesNumberInMemory");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulMp4FilesNumberToDeleteOnOverflow				=
		strtoul (pConfigurationBuffer, (char **) NULL, 10);

	if (ulMp4FilesNumberToDeleteOnOverflow >
		ulMaxMp4FilesNumberInMemory)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_CONFIGITEMWRONG,
2, "Cache", "MaxMp4FilesNumberInMemory or Mp4FilesNumberToDeleteOnOverflow");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("Cache",
		"UseMP4ConsistencyCheck", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "Cache", "UseMP4ConsistencyCheck");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	if (!strcmp (pConfigurationBuffer, "true"))
		bUseMP4ConsistencyCheck			= true;
	else
		bUseMP4ConsistencyCheck			= false;

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("Cache",
		"BucketsNumberForMP4FilesCache",
		pConfigurationBuffer, SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "Cache", "BucketsNumberForMP4FilesCache");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulBucketsNumberForMP4FilesCache				=
		strtoul (pConfigurationBuffer, (char **) NULL, 10);

	if (pmp4fMP4FileFactory -> init (ulMaxMp4FilesNumberInMemory,
		ulMp4FilesNumberToDeleteOnOverflow, bUseMP4ConsistencyCheck,
		ptSystemTracer, ulBucketsNumberForMP4FilesCache) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILEFACTORY_INIT_FAILED);
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	return errNoError;
}


Error StreamingServerService:: onInitRTSPSessions (
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
	unsigned long *pulRTSPRequestsPort)

{

	Error_t					errGetItemValue;
	char					pConfigurationBuffer [SS_MAXLONGLENGTH];
	char					pLocalIPAddressForRTP [
		SCK_MAXIPADDRESSLENGTH];
	unsigned long			ulRTP_RTCPStartingPort;
	unsigned long			ulRTP_RTCPReservedPorts;
	unsigned long			ulRTSP_RTCPTimeoutInSecs;
	unsigned long			ulPauseTimeoutInSecs;
	unsigned long			ulSendRTCPPacketsPeriodInMilliSecs;
	unsigned long			ulReceiveRTCPPacketsPeriodInMilliSecs;
	unsigned long			ulSendRTPMaxSleepTimeInMilliSecs;
	unsigned long			ulRTPPacketsNumberToPrefetch;
	unsigned long			ulSamplesNumberToPrefetch;
	char					pLocalIPAddressForRTCP [
		SCK_MAXIPADDRESSLENGTH];
	double					dMaxSpeedAllowed;
	unsigned long			ulPlayDelayTimeInMilliSeconds;
	Boolean_t				bIsOverBufferEnabled;
	unsigned long			ulSendingInterval;
	unsigned long			ulInitialWindowSizeInBytes;
	unsigned long			ulMaxSendAheadTimeInSec;
	float					fOverbufferRate;
	unsigned long			ulLocalRTP_RTCPStartingPort;
	unsigned long			ulMaxPayloadSizeInBytes;
	long					lRTSPSessionIndex;
	Boolean_t				bUseOfHintingTrackIfExist;
	Boolean_t				bBuildOfHintingTrackIfNotExist;
	char					pRequestLogFormat [
		SS_STREAMINGSERVERSERVICE_MAXREQUESTLOGFORMATLENGTH];
	Boolean_t				bFlushTraceAfterEachRequest;
	Boolean_t				bBeginningHookActivated;
	char					pBeginningHookServletPathName [
		SS_STREAMINGSERVERSERVICE_MAXSERVLETPATHNAMELENGTH];
	char					pBeginningHookWebServerIpAddress [
		SCK_MAXIPADDRESSLENGTH];
	unsigned long			ulBeginningHookWebServerPort;
	char					pBeginningHookLocalIPAddressForHTTP [
		SCK_MAXIPADDRESSLENGTH];
	unsigned long			ulBeginningHookHTTPRequestTimeoutInSecs;
	Boolean_t				bEndingHookActivated;
	char					pEndingHookServletPathName [
		SS_STREAMINGSERVERSERVICE_MAXSERVLETPATHNAMELENGTH];
	char					pEndingHookWebServerIpAddress [
		SCK_MAXIPADDRESSLENGTH];
	unsigned long			ulEndingHookWebServerPort;
	char					pEndingHookLocalIPAddressForHTTP [
		SCK_MAXIPADDRESSLENGTH];
	unsigned long			ulEndingHookHTTPRequestTimeoutInSecs;


	if ((errGetItemValue = pcfConfiguration -> getItemValue (
		"IPConfiguration",
		"LocalIPAddressForRTP", pLocalIPAddressForRTP,
		SCK_MAXIPADDRESSLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "IPConfiguration", "LocalIPAddressForRTP");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue (
		"IPConfiguration",
		"RTP_RTCPStartingPort", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "IPConfiguration", "RTP_RTCPStartingPort");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulRTP_RTCPStartingPort				= strtoul (pConfigurationBuffer,
		(char **) NULL, 10);

	if ((errGetItemValue = pcfConfiguration -> getItemValue (
		"IPConfiguration",
		"RTP_RTCPReservedPorts", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "IPConfiguration", "RTP_RTCPReservedPorts");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulRTP_RTCPReservedPorts			= strtoul (pConfigurationBuffer,
		(char **) NULL, 10);

	// check that there are enough sockets ports to handle
	//	the _ulMaxRTSPServerSessions number of RTSPSessions
	if (ulRTP_RTCPReservedPorts < (*pulMaxRTSPServerSessions) * 4)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVER_RTCPRESERVEDPORTSNOTSUFFICIENT,
			2, ulRTP_RTCPReservedPorts, *pulMaxRTSPServerSessions);
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("Scheduler",
		"RTSP_RTCPTimeout", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "Scheduler", "RTSP_RTCPTimeout");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulRTSP_RTCPTimeoutInSecs			= strtoul (pConfigurationBuffer,
		(char **) NULL, 10);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("Scheduler",
		"PauseTimeout", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "Scheduler", "PauseTimeout");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulPauseTimeoutInSecs		= strtoul (
		pConfigurationBuffer, (char **) NULL, 10);

	if (ulPauseTimeoutInSecs < ulRTSP_RTCPTimeoutInSecs)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_CONFIGITEMWRONG,
			2, "Scheduler", "PauseTimeout or RTSP_RTCPTimeout");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("Scheduler",
		"SendRTCPPacketsPeriodInMilliSecs",
		pConfigurationBuffer, SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "Scheduler", "SendRTCPPacketsPeriodInMilliSecs");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulSendRTCPPacketsPeriodInMilliSecs		=
		strtoul (pConfigurationBuffer, (char **) NULL, 10);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("Scheduler",
		"ReceiveRTCPPacketsPeriodInMilliSecs",
		pConfigurationBuffer, SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "Scheduler", "ReceiveRTCPPacketsPeriodInMilliSecs");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulReceiveRTCPPacketsPeriodInMilliSecs		=
		strtoul (pConfigurationBuffer, (char **) NULL, 10);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("System",
		"SendRTPMaxSleepTimeInMilliSecs",
		pConfigurationBuffer, SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "SendRTPMaxSleepTimeInMilliSecs");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulSendRTPMaxSleepTimeInMilliSecs		=
		strtoul (pConfigurationBuffer, (char **) NULL, 10);

	if ((errGetItemValue = pcfConfiguration -> getItemValue (
		"IPConfiguration",
		"LocalIPAddressForRTSP", pLocalIPAddressForRTSP,
		SCK_MAXIPADDRESSLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "IPConfiguration", "LocalIPAddressForRTSP");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue (
		"IPConfiguration",
		"LocalIPAddressForRTCP", pLocalIPAddressForRTCP,
		SCK_MAXIPADDRESSLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "IPConfiguration", "LocalIPAddressForRTCP");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue (
		"StreamingServer",
		"RTSPRequestsPort", pConfigurationBuffer, SS_MAXLONGLENGTH)) !=
		errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "StreamingServer", "RTSPRequestsPort");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulRTSPRequestsPort			=
		(unsigned long) atol (pConfigurationBuffer);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("System",
		"MaxSpeedAllowed", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "MaxSpeedAllowed");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	dMaxSpeedAllowed				= atof (pConfigurationBuffer);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("System",
		"PlayDelayTimeInMilliSeconds", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "PlayDelayTimeInMilliSeconds");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulPlayDelayTimeInMilliSeconds		= strtoul (
		pConfigurationBuffer, (char **) NULL, 10);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("System",
		"IsOverBufferEnabled", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "IsOverBufferEnabled");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	if (!strcmp (pConfigurationBuffer, "true"))
		bIsOverBufferEnabled			= true;
	else
		bIsOverBufferEnabled			= false;

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("System",
		"SendingInterval", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "SendingInterval");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulSendingInterval		= strtoul (pConfigurationBuffer,
		(char **) NULL, 10);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("System",
		"InitialWindowSizeInBytes", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "InitialWindowSizeInBytes");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulInitialWindowSizeInBytes		= strtoul (pConfigurationBuffer,
		(char **) NULL, 10);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("System",
		"MaxSendAheadTimeInSec", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "MaxSendAheadTimeInSec");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulMaxSendAheadTimeInSec		= strtoul (pConfigurationBuffer,
		(char **) NULL, 10);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("System",
		"OverbufferRate", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "OverbufferRate");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	fOverbufferRate		= atof (pConfigurationBuffer);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("System",
		"MaxPayloadSizeInBytes", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "MaxPayloadSizeInBytes");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulMaxPayloadSizeInBytes			= strtoul (
		pConfigurationBuffer, (char **) NULL, 10);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("Cache",
		"FileCacheSizeInBytes", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "Cache", "FileCacheSizeInBytes");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulFileCacheSizeInBytes			= strtoul (
		pConfigurationBuffer, (char **) NULL, 10);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("System",
		"RTPPacketsNumberToPrefetch", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "RTPPacketsNumberToPrefetch");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulRTPPacketsNumberToPrefetch			= strtoul (
		pConfigurationBuffer, (char **) NULL, 10);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("System",
		"SamplesNumberToPrefetch", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "SamplesNumberToPrefetch");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulSamplesNumberToPrefetch			= strtoul (
		pConfigurationBuffer, (char **) NULL, 10);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("System",
		"UseOfHintingTrackIfExist", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "UseOfHintingTrackIfExist");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	if (!strcmp (pConfigurationBuffer, "true"))
		bUseOfHintingTrackIfExist			= true;
	else
		bUseOfHintingTrackIfExist			= false;

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("System",
		"BuildOfHintingTrackIfNotExist", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "UseOfHintingTrackIfExist");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	if (!strcmp (pConfigurationBuffer, "true"))
		bBuildOfHintingTrackIfNotExist			= true;
	else
		bBuildOfHintingTrackIfNotExist			= false;

	if ((errGetItemValue = pcfConfiguration -> getItemValue (
		"SubscriberLogs",
		"RequestLogFormat", pRequestLogFormat,
		SS_STREAMINGSERVERSERVICE_MAXREQUESTLOGFORMATLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "SubscriberLogs", "RequestLogFormat");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue (
		"SubscriberLogs",
		"FlushTraceAfterEachRequest", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "SubscriberLogs", "FlushTraceAfterEachRequest");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	if (!strcmp (pConfigurationBuffer, "true"))
		bFlushTraceAfterEachRequest			= true;
	else
		bFlushTraceAfterEachRequest			= false;

	if ((errGetItemValue = pcfConfiguration -> getItemValue (
		"BeginningHook",
		"Activated", pConfigurationBuffer, SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "BeginningHook", "Activated");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	if (!strcmp (pConfigurationBuffer, "true"))
		bBeginningHookActivated			= true;
	else
		bBeginningHookActivated			= false;

	if ((errGetItemValue = pcfConfiguration -> getItemValue (
		"BeginningHook",
		"Servlet", pBeginningHookServletPathName,
		SS_STREAMINGSERVERSERVICE_MAXSERVLETPATHNAMELENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "BeginningHook", "Servlet");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue (
		"BeginningHook",
		"LocalIPAddressForHTTP", pBeginningHookLocalIPAddressForHTTP,
		SCK_MAXIPADDRESSLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "BeginningHook", "LocalIPAddressForHTTP");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue (
		"BeginningHook",
		"WebServerIPAddress", pBeginningHookWebServerIpAddress,
		SCK_MAXIPADDRESSLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "BeginningHook", "WebServerIPAddress");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue (
		"BeginningHook",
		"WebServerPort", pConfigurationBuffer, SS_MAXLONGLENGTH)) !=
		errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "BeginningHook", "WebServerPort");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulBeginningHookWebServerPort			=
		(unsigned long) atol (pConfigurationBuffer);

	if ((errGetItemValue = pcfConfiguration -> getItemValue (
		"BeginningHook",
		"HTTPRequestTimeoutInSecs", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "BeginningHook", "HTTPRequestTimeoutInSecs");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulBeginningHookHTTPRequestTimeoutInSecs			=
		(unsigned long) atol (pConfigurationBuffer);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("EndingHook",
		"Activated", pConfigurationBuffer, SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "EndingHook", "Activated");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	if (!strcmp (pConfigurationBuffer, "true"))
		bEndingHookActivated			= true;
	else
		bEndingHookActivated			= false;

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("EndingHook",
		"Servlet", pEndingHookServletPathName,
		SS_STREAMINGSERVERSERVICE_MAXSERVLETPATHNAMELENGTH)) !=
		errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "EndingHook", "Servlet");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("EndingHook",
		"LocalIPAddressForHTTP", pEndingHookLocalIPAddressForHTTP,
		SCK_MAXIPADDRESSLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "EndingHook", "LocalIPAddressForHTTP");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("EndingHook",
		"WebServerIPAddress", pEndingHookWebServerIpAddress,
		SCK_MAXIPADDRESSLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "EndingHook", "WebServerIPAddress");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("EndingHook",
		"WebServerPort", pConfigurationBuffer, SS_MAXLONGLENGTH)) !=
		errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "EndingHook", "WebServerPort");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulEndingHookWebServerPort			=
		(unsigned long) atol (pConfigurationBuffer);

	if ((errGetItemValue = pcfConfiguration -> getItemValue ("EndingHook",
		"HTTPRequestTimeoutInSecs", pConfigurationBuffer,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "EndingHook", "HTTPRequestTimeoutInSecs");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	ulEndingHookHTTPRequestTimeoutInSecs			=
		(unsigned long) atol (pConfigurationBuffer);

	ulLocalRTP_RTCPStartingPort			= ulRTP_RTCPStartingPort;

	for (lRTSPSessionIndex = 0;
		lRTSPSessionIndex < (long) (*pulMaxRTSPServerSessions);
		lRTSPSessionIndex++)
	{
		if ((prsRTSPSessions [lRTSPSessionIndex]). init (
			lRTSPSessionIndex, sStandard,
			pesEventsSet,
			pspStreamingServerSocketsPool,
			pscScheduler,
			ulRTSP_RTCPTimeoutInSecs,
			ulPauseTimeoutInSecs,
			*pulFileCacheSizeInBytes,
			ulSendRTCPPacketsPeriodInMilliSecs,
			ulReceiveRTCPPacketsPeriodInMilliSecs,
			ulSendRTPMaxSleepTimeInMilliSecs,
			pLocalIPAddressForRTSP,
			pLocalIPAddressForRTP,
			pLocalIPAddressForRTCP,
			*pulRTSPRequestsPort,
			dMaxSpeedAllowed,
			ulPlayDelayTimeInMilliSeconds,
			bIsOverBufferEnabled,
			ulSendingInterval,
			ulInitialWindowSizeInBytes,
			ulMaxSendAheadTimeInSec,
			fOverbufferRate,
			pContentRootPath,
			pmtLiveSources,
			plsLiveSources,
			*pulMaxLiveSourcesNumber,
			ulLocalRTP_RTCPStartingPort,
			ulLocalRTP_RTCPStartingPort +
			(ulRTP_RTCPReservedPorts / (*pulMaxRTSPServerSessions)) - 1,
			ulMaxPayloadSizeInBytes,
			ulRTPPacketsNumberToPrefetch,
			ulSamplesNumberToPrefetch,
			bUseOfHintingTrackIfExist,
			bBuildOfHintingTrackIfNotExist,
			pRequestLogFormat,
			pmp4fMP4FileFactory,
			bBeginningHookActivated,
			pBeginningHookServletPathName,
			pBeginningHookWebServerIpAddress,
			ulBeginningHookWebServerPort,
			pBeginningHookLocalIPAddressForHTTP,
			ulBeginningHookHTTPRequestTimeoutInSecs,
			bEndingHookActivated,
			pEndingHookServletPathName,
			pEndingHookWebServerIpAddress,
			ulEndingHookWebServerPort,
			pEndingHookLocalIPAddressForHTTP,
			ulEndingHookHTTPRequestTimeoutInSecs,
			bFlushTraceAfterEachRequest,
			ptSystemTracer,
			ptSubscriberTracer) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_INIT_FAILED);
			ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			pvFreeRTSPSessions -> clear ();

			while (--lRTSPSessionIndex >= 0)
			{
				if ((prsRTSPSessions [lRTSPSessionIndex]). finish () !=
					errNoError)
				{
					Error err = StreamingServerErrors (
						__FILE__, __LINE__,
						SS_RTSPSESSION_FINISH_FAILED);
					ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return err;
		}

		ulLocalRTP_RTCPStartingPort				+=
			(ulRTP_RTCPReservedPorts / (*pulMaxRTSPServerSessions));

		pvFreeRTSPSessions -> insert (pvFreeRTSPSessions -> end (),
			&(prsRTSPSessions [lRTSPSessionIndex]));
	}


	return errNoError;
}


Error StreamingServerService:: onStop (void)

{

	// to finish onStart
	if (_hssHttpStreamingServer. cancel () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_CANCEL_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	// to wait that the onStart method finishes
	{
		time_t							tUTCNow;
		PosixThread:: PThreadStatus_t	stRTPThreadState;


		if ((_pspStreamingServerProcessor [
			_ulStreamingServerProcessorsNumber - 1]).
			getThreadState (&stRTPThreadState) != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_GETTHREADSTATE_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		tUTCNow					= time (NULL);

		while (stRTPThreadState == PosixThread:: THREADLIB_STARTED ||
			stRTPThreadState == PosixThread:: THREADLIB_STARTED_AND_JOINED)
		{
			char aaa [256];
			sprintf (aaa, "State: %ld", (long) stRTPThreadState);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				aaa, __FILE__, __LINE__);

			if (time (NULL) - tUTCNow >= 60)
				break;

			if (PosixThread:: getSleep (1, 0) != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_GETSLEEP_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((_pspStreamingServerProcessor [
				_ulStreamingServerProcessorsNumber - 1]).
				getThreadState (&stRTPThreadState) != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_GETTHREADSTATE_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_FINISHING);
		_tSystemTracer. trace (
			Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (finishNICs (&_vIPAddresses) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVER_FINISHNICS_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_hssHttpStreamingServer. finish () != errNoError)
	{
		Error err = WebToolsErrors (__FILE__, __LINE__,
			WEBTOOLS_HTTPSERVER_FINISH_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err,
			__FILE__, __LINE__);
	}

	if (_cssCheckServerSocketTimes. finish () != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_CHECKSERVERSOCKETTIMES_FINISH_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_cspCheckSocketsPoolTimes. finish () != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_CHECKSOCKETSPOOLTIMES_FINISH_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	// bDestroyTimes
	if (_scScheduler. finish (false) != errNoError)
	{
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_SCHEDULER_FINISH_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err,
			__FILE__, __LINE__);
	}

	{
		unsigned long			ulProcessorIdentifier;

		for (ulProcessorIdentifier = 0;
			ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
			ulProcessorIdentifier++)
		{
			if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
				finish () != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERPROCESSOR_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}
	}

	delete [] _pspStreamingServerProcessor;

	if (_ssServerSocket. finish () != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SERVERSOCKET_FINISH_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_mtFreeRTSPSessions. finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_FINISH_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_mtStreamingServerStatistics. finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_FINISH_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	_vFreeRTSPSessions. clear ();

	{
		unsigned long			ulRTSPSessionIndex;

		for (ulRTSPSessionIndex = 0;
			ulRTSPSessionIndex < _ulMaxRTSPServerSessions;
			ulRTSPSessionIndex++)
		{
			if ((_prsRTSPSessions [ulRTSPSessionIndex]). finish () !=
				errNoError)
			{
				Error err = StreamingServerErrors (
					__FILE__, __LINE__,
					SS_RTSPSESSION_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}
	}

	delete [] _prsRTSPSessions;
	_prsRTSPSessions			= (RTSPSession_p) NULL;

	if (_bContentRootPath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_mtLiveSources. finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_FINISH_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_spStreamingServerSocketsPool. finish () != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETSPOOL_FINISH_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_esEventsSet. finish () != errNoError)
	{
		Error err = EventsSetErrors (__FILE__, __LINE__,
			EVSET_EVENTSSET_FINISH_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	delete [] _plsLiveSources;
	_plsLiveSources		= (LiveSource_p) NULL;

	if (_mp4fMP4FileFactory. finish () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILEFACTORY_INIT_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	// The next trace message cannot be moved at the end of this method
	// because the tracer will be already stopped
	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_FINISHED);
		_tSystemTracer. trace (
			Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (stopTracer (&_tSubscriberTracer) != errNoError)
	{
		// Error err = StreamingServerErrors (__FILE__, __LINE__,
		// 	SS_STOPTRACER_FAILED);
		// std:: cerr << (const char *) err << std:: endl;
	}

	if (stopTracer (&_tSystemTracer) != errNoError)
	{
		// Error err = StreamingServerErrors (__FILE__, __LINE__,
		// 	SS_STOPTRACER_FAILED);
		// std:: cerr << (const char *) err << std:: endl;
	}

	if (_cfConfiguration. finish () != errNoError)
	{
		// Error err = ConfigurationFileErrors (__FILE__, __LINE__,
		// 	CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
		// std:: cerr << (const char *) err << std:: endl;
	}


	return errNoError;
}


Error StreamingServerService:: onStart (void)

{

	unsigned long			ulProcessorIdentifier;
	Error_t					errProcessor;
	Error_t					errJoin;


	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_STARTING);
		_tSystemTracer. trace (
			Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	// start Processor
	for (ulProcessorIdentifier = 0;
		ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
		ulProcessorIdentifier++)
	{
		/*
		{
			long		lStackSize;
			char		pBuff [1024];

			lStackSize		= 20480 * 1024;
			if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
				setStackSize (lStackSize) != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_SETSTACKSIZE_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
				getStackSize (&lStackSize) != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_SETSTACKSIZE_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
			sprintf (pBuff, "Stack size: %ld", lStackSize);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR, pBuff,
				__FILE__, __LINE__);
		}
		*/

		if ((_pspStreamingServerProcessor [ulProcessorIdentifier]). start () !=
			errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_START_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err,
				__FILE__, __LINE__);

			while (--ulProcessorIdentifier >= 0)
			{
				if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
					cancel () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PTHREAD_CANCEL_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return err;
		}
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_STREAMINGSERVERPROCESSORSSTARTED);
		_tSystemTracer. trace (
			Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	// start scheduler
	{
		if (_scScheduler. start () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_SCHEDULER_START_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err,
				__FILE__, __LINE__);

			for (ulProcessorIdentifier = 0;
				ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
				ulProcessorIdentifier++)
			{
				if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
					cancel () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PTHREAD_CANCEL_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return err;
		}
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_SCHEDULERSTARTED);
		_tSystemTracer. trace (
			Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}


	{
		unsigned long			ulRTSPSessionIdentifier;
		RTSP_RTCPTimes_p		prrRTSP_RTCPTimes;
		PauseTimes_p			pptPauseTimes;
		SendRTCPPacketsTimes_p	psrptSendRTCPPacketsTimes;
		CheckReceivedRTCPPacketsTimes_p	pcrrptCheckReceivedRTCPPacketsTimes;


		for (ulRTSPSessionIdentifier = 0;
			ulRTSPSessionIdentifier < _ulMaxRTSPServerSessions;
			ulRTSPSessionIdentifier++)
		{
			if ((_prsRTSPSessions [ulRTSPSessionIdentifier]).
				getRTSP_RTCPTimes (&prrRTSP_RTCPTimes) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_GETRTSP_RTCPTIMES_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_scScheduler. cancel () != errNoError)
				{
					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_SCHEDULER_CANCEL_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err,
						__FILE__, __LINE__);
				}

				for (ulProcessorIdentifier = 0;
					ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
					ulProcessorIdentifier++)
				{
					if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
						cancel () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PTHREAD_CANCEL_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				return err;
			}

			if ((_prsRTSPSessions [ulRTSPSessionIdentifier]). getPauseTimes (
				&pptPauseTimes) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_GETPAUSETIMES_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_scScheduler. cancel () != errNoError)
				{
					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_SCHEDULER_CANCEL_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err,
						__FILE__, __LINE__);
				}

				for (ulProcessorIdentifier = 0;
					ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
					ulProcessorIdentifier++)
				{
					if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
						cancel () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PTHREAD_CANCEL_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				return err;
			}

			if ((_prsRTSPSessions [ulRTSPSessionIdentifier]).
				getSendRTCPPacketsTimes (&psrptSendRTCPPacketsTimes) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_GETSENDRTSPPACKETSTIMES_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_scScheduler. cancel () != errNoError)
				{
					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_SCHEDULER_CANCEL_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err,
						__FILE__, __LINE__);
				}

				for (ulProcessorIdentifier = 0;
					ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
					ulProcessorIdentifier++)
				{
					if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
						cancel () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PTHREAD_CANCEL_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				return err;
			}

			if ((_prsRTSPSessions [ulRTSPSessionIdentifier]).
				getCheckReceivedRTCPPacketsTimes (
				&pcrrptCheckReceivedRTCPPacketsTimes) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_GETCHECKRECEIVEDRTSPPACKETSTIMES_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_scScheduler. cancel () != errNoError)
				{
					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_SCHEDULER_CANCEL_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err,
						__FILE__, __LINE__);
				}

				for (ulProcessorIdentifier = 0;
					ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
					ulProcessorIdentifier++)
				{
					if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
						cancel () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PTHREAD_CANCEL_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				return err;
			}

			if (_scScheduler. activeTimes (prrRTSP_RTCPTimes) !=
				errNoError)
			{
				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_SCHEDULER_ACTIVETIMES_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err,
					__FILE__, __LINE__);

				if (_scScheduler. cancel () != errNoError)
				{
					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_SCHEDULER_CANCEL_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err,
						__FILE__, __LINE__);
				}

				for (ulProcessorIdentifier = 0;
					ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
					ulProcessorIdentifier++)
				{
					if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
						cancel () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PTHREAD_CANCEL_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				return err;
			}

			if (_scScheduler. activeTimes (pptPauseTimes) !=
				errNoError)
			{
				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_SCHEDULER_ACTIVETIMES_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err,
					__FILE__, __LINE__);

				if (_scScheduler. cancel () != errNoError)
				{
					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_SCHEDULER_CANCEL_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err,
						__FILE__, __LINE__);
				}

				for (ulProcessorIdentifier = 0;
					ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
					ulProcessorIdentifier++)
				{
					if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
						cancel () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PTHREAD_CANCEL_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				return err;
			}

			if (_scScheduler. activeTimes (psrptSendRTCPPacketsTimes) !=
				errNoError)
			{
				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_SCHEDULER_ACTIVETIMES_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err,
					__FILE__, __LINE__);

				if (_scScheduler. cancel () != errNoError)
				{
					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_SCHEDULER_CANCEL_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err,
						__FILE__, __LINE__);
				}

				for (ulProcessorIdentifier = 0;
					ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
					ulProcessorIdentifier++)
				{
					if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
						cancel () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PTHREAD_CANCEL_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				return err;
			}

			if (_scScheduler. activeTimes (
				pcrrptCheckReceivedRTCPPacketsTimes) != errNoError)
			{
				Error err = SchedulerErrors (__FILE__, __LINE__,
					SCH_SCHEDULER_ACTIVETIMES_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err,
					__FILE__, __LINE__);

				if (_scScheduler. cancel () != errNoError)
				{
					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_SCHEDULER_CANCEL_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err,
						__FILE__, __LINE__);
				}

				for (ulProcessorIdentifier = 0;
					ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
					ulProcessorIdentifier++)
				{
					if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
						cancel () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PTHREAD_CANCEL_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				return err;
			}
		}
	}

	if (_cspCheckSocketsPoolTimes. start () != errNoError)
	{
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_TIMES_START_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err,
			__FILE__, __LINE__);

		if (_scScheduler. cancel () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_SCHEDULER_CANCEL_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err,
				__FILE__, __LINE__);
		}

		for (ulProcessorIdentifier = 0;
			ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
			ulProcessorIdentifier++)
		{
			if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
				cancel () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_CANCEL_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		return err;
	}

	if (_scScheduler. activeTimes (&_cspCheckSocketsPoolTimes) !=
		errNoError)
	{
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_SCHEDULER_ACTIVETIMES_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err,
			__FILE__, __LINE__);

		if (_scScheduler. cancel () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_SCHEDULER_CANCEL_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err,
				__FILE__, __LINE__);
		}

		for (ulProcessorIdentifier = 0;
			ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
			ulProcessorIdentifier++)
		{
			if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
				cancel () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_CANCEL_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		return err;
	}

	if (_cssCheckServerSocketTimes. start () != errNoError)
	{
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_TIMES_START_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err,
			__FILE__, __LINE__);

		if (_scScheduler. cancel () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_SCHEDULER_CANCEL_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err,
				__FILE__, __LINE__);
		}

		for (ulProcessorIdentifier = 0;
			ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
			ulProcessorIdentifier++)
		{
			if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
				cancel () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_CANCEL_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		return err;
	}

	if (_scScheduler. activeTimes (&_cssCheckServerSocketTimes) !=
		errNoError)
	{
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_SCHEDULER_ACTIVETIMES_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err,
			__FILE__, __LINE__);

		if (_scScheduler. cancel () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_SCHEDULER_CANCEL_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err,
				__FILE__, __LINE__);
		}

		for (ulProcessorIdentifier = 0;
			ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
			ulProcessorIdentifier++)
		{
			if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
				cancel () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_CANCEL_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		return err;
	}

	if (_hssHttpStreamingServer. start () != errNoError)
	{
		Error err = WebToolsErrors (__FILE__, __LINE__,
			WEBTOOLS_HTTPSERVER_FINISH_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err,
			__FILE__, __LINE__);

		if (_scScheduler. cancel () != errNoError)
		{
			Error err = SchedulerErrors (__FILE__, __LINE__,
				SCH_SCHEDULER_CANCEL_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err,
				__FILE__, __LINE__);
		}

		for (ulProcessorIdentifier = 0;
			ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
			ulProcessorIdentifier++)
		{
			if ((_pspStreamingServerProcessor [ulProcessorIdentifier]).
				cancel () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_CANCEL_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_HTTPSTREAMINGSERVERSTARTED);
		_tSystemTracer. trace (
			Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	#ifdef WIN32
		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSERVICE_RUNNING,
				9, "Not available on Windows", "Windows",
				(const char *) _bContentRootPath, _pLocalIPAddressForRTSP,
				_ulRTSPRequestsPort, _ulMaxRTSPServerSessions,
				_pLocalIPAddressForHTTP, _ulHTTPRequestsPort,
				_sStandard == MP4Atom:: MP4F_ISMA ? "ISMA" : "3GPP");

			_tSystemTracer. trace (
				Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}
	#else
		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVERSERVICE_RUNNING,
				9, CatraStreaming_VERSION_MAJOR, "Unix/Linux",
				(const char *) _bContentRootPath, _pLocalIPAddressForRTSP,
				_ulRTSPRequestsPort, _ulMaxRTSPServerSessions,
				_pLocalIPAddressForHTTP, _ulHTTPRequestsPort,
				_sStandard == MP4Atom:: MP4F_ISMA ? "ISMA" : "3GPP");

			_tSystemTracer. trace (
				Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}
	#endif

	// Waiting of the last thread started
	if ((errProcessor = _hssHttpStreamingServer. join (&errJoin)) !=
		errNoError)
	{
		if (errProcessor != errNoError)
		{
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errProcessor, __FILE__, __LINE__);
		}
	}

	if (errJoin != errNoError)
	{
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errJoin, __FILE__, __LINE__);

		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_JOIN_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_SHUTDOWN);
		_tSystemTracer. trace (
			Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (cancel () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_CANCEL_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err,
			__FILE__, __LINE__);
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_NOTRUNNING);
		_tSystemTracer. trace (
			Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}


	return errNoError;
}


Error StreamingServerService:: cancel (void)

{

	unsigned long				ulProcessorIdentifier;
	Error_t						errCancel;
	Error_t						errProcessor;
	Error_t						errJoin;


	if ((errCancel = _hssHttpStreamingServer. cancel ()) != errNoError)
	{
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errCancel,
			__FILE__, __LINE__);

		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_CANCEL_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err,
			__FILE__, __LINE__);
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_HTTPSTREAMINGSERVERSTOPPED);
		_tSystemTracer. trace (
			Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	if ((errCancel = _scScheduler. cancel ()) != errNoError)
	{
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errCancel,
			__FILE__, __LINE__);

		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_SCHEDULER_CANCEL_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err,
			__FILE__, __LINE__);
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_SCHEDULERSTOPPED);
		_tSystemTracer. trace (
			Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	for (ulProcessorIdentifier = 0;
		ulProcessorIdentifier < _ulStreamingServerProcessorsNumber;
		ulProcessorIdentifier++)
	{
		if ((_pspStreamingServerProcessor [
			ulProcessorIdentifier]). cancel () != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERPROCESSOR_CANCEL_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err,
				__FILE__, __LINE__);
		}
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERSERVICE_STREAMINGSERVERPROCESSORSSTOPPED);
		_tSystemTracer. trace (
			Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}



	return errNoError;
}


Error StreamingServerService:: initNICs (
	std:: vector<SocketImpl:: IPAddress_t> *pvIPAddresses)

{


	if (SocketImpl:: getIPAddressesList (pvIPAddresses) !=
		errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_GETIPADDRESSESLIST_FAILED);
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}



	return errNoError;
}


Error StreamingServerService:: finishNICs (
	std:: vector<SocketImpl:: IPAddress_t> *pvIPAddresses)

{

	pvIPAddresses -> clear ();


	return errNoError;
}


/*
#ifdef HAVE_CORBA_H
	CORBA::Long StreamingServerService:: isRequestInCache (
		const char *pAssetPath,
		CORBA:: ULong_out bIsInCache,
		CORBA:: ULong_out ulConnectedUsers,
		CORBA:: ULong_out ulBandWidthUsageInbps)

	{

		Boolean_t				bIsLive;
		Boolean_t				bIsRequestInCache;
		RTSPSession_p			prsRTSPSession;
		std:: vector<RTSPSession_p>:: iterator		it;
		unsigned long			ulAudioBitRate;
		unsigned long			ulVideoBitRate;
		Error					errGetAvgBitRate;


		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVER_CORBA_ISREQUESTINCACHE,
				1, pAssetPath);
			_tSystemTracer. trace (Tracer:: TRACER_LDBG5, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (RTSPSession:: isLiveRequest (pAssetPath, &bIsLive) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_ISLIVEREQUEST_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return 1;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bIsLive)
		{
			bIsRequestInCache		= true;
		}
		else
		{
			if (_mp4fMP4FileFactory. isMP4FileInCache (
				pAssetPath, &bIsRequestInCache) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_ISMP4FILEINCACHE_FAILED);
				_tSystemTracer. trace (
					Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 2;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err);
			}
		}

		bIsInCache					= bIsRequestInCache;

		if (_mtActiveRTSPSessions. lock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_LOCK_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 3;
		}

		ulConnectedUsers			= _vActiveRTSPSessions. size ();

		ulBandWidthUsageInbps		= 0;

		for (it = _vActiveRTSPSessions. begin ();
			it != _vActiveRTSPSessions. end ();
			++it)
		{
			prsRTSPSession			= *it;

			if ((errGetAvgBitRate =
				prsRTSPSession -> getAvgBitRate (&ulAudioBitRate,
				&ulVideoBitRate)) != errNoError)
			{
				if ((long) errGetAvgBitRate == SS_RTSPSESSION_WRONGSTATE)
				{
					ulAudioBitRate			= 0;
					ulVideoBitRate			= 0;
				}
				else
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_GETAVGBITRATE_FAILED);
					_tSystemTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtActiveRTSPSessions. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_tSystemTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return 4;
				}
			}

			ulBandWidthUsageInbps		+= (ulAudioBitRate +
				ulVideoBitRate);
		}

		if (_mtActiveRTSPSessions. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 5;
		}

		ulBandWidthUsageInbps		*= 1000;


		return 0;
	}


	CORBA:: Long StreamingServerService:: setChangedConfiguration (
		const ::StreamingIDL:: ConfigurationItemsList &
		cilConfigurationItemsList)

	{
		long				lChangedConfigurationItemsNumber;
		long				lChangedConfigurationItemIndex;


		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVER_CORBA_SETCHANGEDCONFIGURATION);
			_tSystemTracer. trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}


		lChangedConfigurationItemsNumber		=
			cilConfigurationItemsList. length ();

		if (lChangedConfigurationItemsNumber > 0)
		{
			for (lChangedConfigurationItemIndex = 0;
				lChangedConfigurationItemIndex <
				lChangedConfigurationItemsNumber;
				lChangedConfigurationItemIndex++)
			{
				if (_cfConfiguration. modifyItemValue (
					cilConfigurationItemsList [
					lChangedConfigurationItemIndex]. pSectionName,
					cilConfigurationItemsList [
					lChangedConfigurationItemIndex]. pItemName,
					cilConfigurationItemsList [
					lChangedConfigurationItemIndex]. pItemValue) !=
					errNoError)
				{
					Error err = ConfigurationErrors (__FILE__, __LINE__,
						CFG_CONFIG_MODIFYITEMVALUE_FAILED);
					_tSystemTracer. trace (
						Tracer:: TRACER_LERRR, (const char *) err,
						__FILE__, __LINE__);

					return 1;
					// throw ::StreamingIDL:: StreamingExc ((const char *) err); 
				}
			}

			if (_cfConfiguration. save () != errNoError)
			{
				Error err = ConfigurationFileErrors (__FILE__, __LINE__,
					CFGFILE_CONFIGURATIONFILE_SAVE_FAILED);
				_tSystemTracer. trace (
					Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return 2;
				// throw ::StreamingIDL:: StreamingExc ((const char *) err); 
			}
		}


		return 0;
	}


	CORBA:: Long StreamingServerService:: getHostName (
		CORBA:: String_out pHostName)

	{

		pHostName				= CORBA:: string_dup (_pHostName);


		return 0;
	}


	CORBA:: Long StreamingServerService:: addContent (
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
				SS_STREAMINGSERVER_CORBA_ADDCONTENT,
				1, pContentName);
			_tSystemTracer. trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (bContentPathName. init (_pContentRootPath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 1;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. append ("/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 2;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. append (pContentName) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
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
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
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
				SS_STREAMINGSERVER_CONTENTTOCREATE,
				1, (const char *) bContentPathName);
			_tSystemTracer. trace (Tracer:: TRACER_LINFO, (const char *) msg,
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
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] pucContent;

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
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
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (FileIO:: close (iFileDescriptor) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_CLOSE_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] pucContent;

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 6;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (FileIO:: close (iFileDescriptor) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_CLOSE_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] pucContent;

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
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
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 8;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}


		return 0;
	}


	CORBA:: Long StreamingServerService:: deleteContent (
		const char *pContentName)

	{

		Buffer_t					bContentPathName;


		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVER_CORBA_DELETECONTENT,
				1, pContentName);
			_tSystemTracer. trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (bContentPathName. init (_pContentRootPath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 1;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. append ("/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 2;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. append (pContentName) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 3;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_STREAMINGSERVER_CONTENTTODELETE,
				1, (const char *) bContentPathName);
			_tSystemTracer. trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (FileIO:: remove ((const char *) bContentPathName) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_REMOVE_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bContentPathName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_tSystemTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return 4;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}

		if (bContentPathName. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_tSystemTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return 5;
			// throw ::StreamingIDL:: StreamingExc ((const char *) err);
		}


		return 0;
	}
#endif
*/


Error StreamingServerService:: startTracer (
	ConfigurationFile_p pcfConfiguration,
	Tracer_p ptTracer, char *pSectionName)

{

	char								pCacheSizeOfTraceFile [
		SS_MAXLONGLENGTH];
	char								pTraceFileName [
		SS_STREAMINGSERVERSERVICE_MAXTRACEFILELENGTH];
	char								pTraceDirectory [
		SS_STREAMINGSERVERSERVICE_MAXTRACEFILELENGTH];
	Error_t								errTracerInit;
	Error_t								errCreateDir;
	char								cDirectorySeparator;
	char								pMaxTraceFileSize [
		SS_MAXLONGLENGTH];
	char								pTraceFilePeriodInSecs [
		SS_MAXLONGLENGTH];
	char								pCompressedTraceFile [
		SS_STREAMINGSERVERSERVICE_MAXBOOLEANLENGTH];
	Boolean_t							bCompressedTraceFile;
	char								pTraceFilesNumberToMaintain [
		SS_MAXLONGLENGTH];
	long								lTraceFilesNumberToMaintain;
	char								pTraceOnTTY [
		SS_STREAMINGSERVERSERVICE_MAXBOOLEANLENGTH];
	Boolean_t							bTraceOnTTY;
	char								pTraceLevel [
		SS_MAXLONGLENGTH];
	long								lTraceLevel;
	char								pListenTracePort [
		SS_MAXLONGLENGTH];
	unsigned long						ulListenTracePort;
	long								lSecondsBetweenTwoCheckTraceToManage;
	Error_t								errGetItemValue;


	if ((errGetItemValue = pcfConfiguration -> getItemValue (pSectionName,
		"CacheSizeOfTraceFile", pCacheSizeOfTraceFile,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		// std:: cerr << (const char *) errGetItemValue << std:: endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, pSectionName, "CacheSizeOfTraceFile");
		// std:: cerr << (const char *) err << std:: endl;

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue (pSectionName,
		"TraceDirectory", pTraceDirectory,
		SS_STREAMINGSERVERSERVICE_MAXTRACEFILELENGTH)) != errNoError)
	{
		// std:: cerr << (const char *) errGetItemValue << std:: endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, pSectionName, "TraceDirectory");
		// std:: cerr << (const char *) err << std:: endl;

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue (pSectionName,
		"TraceFileName", pTraceFileName,
		SS_STREAMINGSERVERSERVICE_MAXTRACEFILELENGTH)) != errNoError)
	{
		// std:: cerr << (const char *) errGetItemValue << std:: endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, pSectionName, "TraceFileName");
		// std:: cerr << (const char *) err << std:: endl;

		return err;
	}

	#ifdef WIN32
		cDirectorySeparator				= '\\';
	#else
		cDirectorySeparator				= '/';
	#endif

	#ifdef WIN32
		if ((errCreateDir = FileIO:: createDirectory (pTraceDirectory,
			0)) != errNoError)
	#else
		if ((errCreateDir = FileIO:: createDirectory (pTraceDirectory,
			S_IRUSR | S_IWUSR | S_IXUSR |
			S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) != errNoError)
	#endif
	{
		if ((long) errCreateDir == TOOLS_MKDIR_FAILED)
		{
			int					iErrno;
			unsigned long		ulUserDataBytes;

			errCreateDir. getUserData (&iErrno, &ulUserDataBytes);
			if (iErrno != EEXIST)
			{
				// std:: cerr << (const char *) errCreateDir << std:: endl;

				return errCreateDir;
			}
		}
		else
		{
			// std:: cerr << (const char *) errCreateDir << std:: endl;

			return errCreateDir;
		}
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue (pSectionName,
		"MaxTraceFileSize", pMaxTraceFileSize,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		// std:: cerr << (const char *) errGetItemValue << std:: endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, pSectionName, "MaxTraceFileSize");
		// std:: cerr << (const char *) err << std:: endl;

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue (pSectionName,
		"TraceFilePeriodInSecs", pTraceFilePeriodInSecs,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		// std:: cerr << (const char *) errGetItemValue << std:: endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, pSectionName, "TraceFilePeriodInSecs");
		// std:: cerr << (const char *) err << std:: endl;

		return err;
	}

	if ((errGetItemValue = pcfConfiguration -> getItemValue (pSectionName,
		"CompressedTraceFile", pCompressedTraceFile,
		SS_STREAMINGSERVERSERVICE_MAXBOOLEANLENGTH)) != errNoError)
	{
		// std:: cerr << (const char *) errGetItemValue << std:: endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, pSectionName, "CompressedTraceFile");
		// std:: cerr << (const char *) err << std:: endl;

		return err;
	}
	if (!strcmp (pCompressedTraceFile, "true"))
		bCompressedTraceFile				= true;
	else
		bCompressedTraceFile				= false;

	if ((errGetItemValue = pcfConfiguration -> getItemValue (pSectionName,
		"TraceFilesNumberToMaintain",
		pTraceFilesNumberToMaintain,
		SS_MAXLONGLENGTH)) != errNoError)
	{
		// std:: cerr << (const char *) errGetItemValue << std:: endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, pSectionName, "TraceFilesNumberToMaintain");
		// std:: cerr << (const char *) err << std:: endl;

		return err;
	}

	lTraceFilesNumberToMaintain			= atol (pTraceFilesNumberToMaintain);

	if ((errGetItemValue = pcfConfiguration -> getItemValue (pSectionName,
		"TraceOnTTY", pTraceOnTTY,
		SS_STREAMINGSERVERSERVICE_MAXBOOLEANLENGTH)) != errNoError)
	{
		// std:: cerr << (const char *) errGetItemValue << std:: endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, pSectionName, "TraceOnTTY");
		// std:: cerr << (const char *) err << std:: endl;

		return err;
	}
	if (!strcmp (pTraceOnTTY, "true"))
		bTraceOnTTY							= true;
	else
		bTraceOnTTY							= false;

	if ((errGetItemValue = pcfConfiguration -> getItemValue (pSectionName,
		"TraceLevel", pTraceLevel, SS_MAXLONGLENGTH)) != errNoError)
	{
		// std:: cerr << (const char *) errGetItemValue << std:: endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, pSectionName, "TraceLevel");
		// std:: cerr << (const char *) err << std:: endl;

		return err;
	}

	if (!strcmp (pTraceLevel, "LDBG1"))
		lTraceLevel				= 0;
	else if (!strcmp (pTraceLevel, "LDBG2"))
		lTraceLevel				= 1;
	else if (!strcmp (pTraceLevel, "LDBG3"))
		lTraceLevel				= 2;
	else if (!strcmp (pTraceLevel, "LDBG4"))
		lTraceLevel				= 3;
	else if (!strcmp (pTraceLevel, "LDBG5"))
		lTraceLevel				= 4;
	else if (!strcmp (pTraceLevel, "LDBG6"))
		lTraceLevel				= 5;
	else if (!strcmp (pTraceLevel, "LINFO"))
		lTraceLevel				= 6;
	else if (!strcmp (pTraceLevel, "LMESG"))
		lTraceLevel				= 7;
	else if (!strcmp (pTraceLevel, "LWRNG"))
		lTraceLevel				= 8;
	else if (!strcmp (pTraceLevel, "LERRR"))
		lTraceLevel				= 9;
	else if (!strcmp (pTraceLevel, "LFTAL"))
		lTraceLevel				= 10;
	else
		lTraceLevel				= 6;

	if ((errGetItemValue = pcfConfiguration -> getItemValue (pSectionName,
		"ListenTracePort", pListenTracePort, SS_MAXLONGLENGTH)) != errNoError)
	{
		// std:: cerr << (const char *) errGetItemValue << std:: endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, pSectionName, "ListenTracePort");
		// std:: cerr << (const char *) err << std:: endl;

		return err;
	}

	ulListenTracePort		= atol (pListenTracePort);

	lSecondsBetweenTwoCheckTraceToManage		= 7;

	if ((errTracerInit = ptTracer -> init (
		pSectionName,					// pName
		atol (pCacheSizeOfTraceFile),	// lCacheSizeOfTraceFile K-byte
		pTraceDirectory,				// pBaseTraceFileName
		pTraceFileName,					// pBaseTraceFileName
		atol (pMaxTraceFileSize),		// lMaxTraceFileSize K-byte
		atol (pTraceFilePeriodInSecs),	// lTraceFilePeriodInSecs
		bCompressedTraceFile,			// bCompressedTraceFile
		false,							// bClosedFileToBeCopied
		(const char *) NULL,			// pClosedFilesRepository
		lTraceFilesNumberToMaintain,	// lTraceFilesNumberToMaintain
		true,							// bTraceOnFile
		bTraceOnTTY,					// bTraceOnTTY
		lTraceLevel,					// lTraceLevel
		lSecondsBetweenTwoCheckTraceToManage,
		3000,							// lMaxTracesNumber
		ulListenTracePort,				// lListenPort
		1000,							// lTracesNumberAllocatedOnOverflow
		1000)) != errNoError)			// lSizeOfEachBlockToGzip K-byte
	{
		// Error err = TracerErrors (__FILE__, __LINE__,
		// 	TRACER_TRACER_INIT_FAILED);	
		// std:: cerr << (const char *) err << std:: endl;

		// return err;
		return errTracerInit;
	}

	/*
	{
		long		lStackSize;
		char		pBuff [1024];

		if (ptTracer -> getStackSize (&lStackSize) != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_SETSTACKSIZE_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}
		sprintf (pBuff, "Stack size prima: %ld", lStackSize);
		std:: cout << pBuff << std:: endl;

		lStackSize		= 20480 * 1024;
		if (ptTracer -> setStackSize (lStackSize) != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_SETSTACKSIZE_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}

		if (ptTracer -> getStackSize (&lStackSize) != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_SETSTACKSIZE_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}
		sprintf (pBuff, "Stack size dopo: %ld", lStackSize);
		std:: cout << pBuff << std:: endl;
	}
	*/

	if (ptTracer -> start () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_START_FAILED);	
		// std:: cerr << (const char *) err << std:: endl;

		if (ptTracer -> finish (true) != errNoError)
		{
			// Error err = TracerErrors (__FILE__, __LINE__,
			// 	TRACER_TRACER_FINISH_FAILED);	
			// std:: cerr << (const char *) err << std:: endl;
		}

		return err;
	}


	return errNoError;
}


Error StreamingServerService:: stopTracer (Tracer_p ptTracer)

{

	if (ptTracer -> cancel () != errNoError)
	{
		Error err = TracerErrors (__FILE__, __LINE__,
			TRACER_TRACER_CANCEL_FAILED);	
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (ptTracer -> finish (true) != errNoError)
	{
		Error err = TracerErrors (__FILE__, __LINE__,
			TRACER_TRACER_FINISH_FAILED);	
		// std:: cerr << (const char *) err << std:: endl;

		return err;
	}


	return errNoError;
}


Error StreamingServerService:: checkLicense (
	ConfigurationFile_p pcfConfiguration,
	Tracer_p ptSystemTracer)

{

	char				pLicense [SS_STREAMINGSERVERSERVICE_MAXLICENSELENGTH];
	tm					tmDateTime;
	unsigned long		ulMilliSecs;
	char				pCurrentLocalDateTime [
		SS_STREAMINGSERVERSERVICE_DATETIMELENGTH];
	Error_t				errGetItemValue;


	if ((errGetItemValue = pcfConfiguration -> getItemValue ("System",
		"License", pLicense, SS_STREAMINGSERVERSERVICE_MAXLICENSELENGTH,
		0, Config:: CFG_ENCRIPTION)) != errNoError)
	{
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGetItemValue,
			__FILE__, __LINE__);

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "System", "License");
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (strlen (pLicense) != 10)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			// SS_LICENSECORRUPTED, 1, pLicense);
			SS_LICENSECORRUPTED);
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// year
	if (!isdigit (pLicense [0]) ||
		!isdigit (pLicense [1]) ||
		!isdigit (pLicense [2]) ||
		!isdigit (pLicense [3]))
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_LICENSECORRUPTED);
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (pLicense [4] != '-')
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_LICENSECORRUPTED);
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// month
	if (!isdigit (pLicense [5]) ||
		!isdigit (pLicense [6]))
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_LICENSECORRUPTED);
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (pLicense [7] != '-')
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_LICENSECORRUPTED);
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// day
	if (!isdigit (pLicense [8]) ||
		!isdigit (pLicense [9]))
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_LICENSECORRUPTED);
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (DateTime:: get_tm_LocalTime (&tmDateTime,
		&ulMilliSecs) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_DATETIME_GET_TM_LOCALTIME_FAILED);
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	sprintf (pCurrentLocalDateTime, "%04d-%02d-%02d",
		tmDateTime. tm_year + 1900,
		tmDateTime. tm_mon + 1,
		tmDateTime. tm_mday);

	if (strcmp (pCurrentLocalDateTime, pLicense) > 0)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			// SS_LICENSEEXPIRED, 1, pLicense);
			SS_LICENSEEXPIRED);
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error StreamingServerService:: appendStartScriptCommand (
	Buffer_p pbServiceScriptFile)

{

	if (pbServiceScriptFile -> append (
		"\t. /catraStreamingServer.sh" "\n"
		"\n"
		"# PID saved in " SS_STREAMINGSERVERSERVICE_PIDFILEPATHNAME "\n"
		"\tdaemon catraStreamingServer" "\n"
		"\techo \"Started\" >> "
		TOOLS_SERVICE_DEBUGFILE "\n"
		"\n"
		"\treturn 0"
		) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		/* _tSystemTracer is not yet initialized
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/

		return err;
	}


	return errNoError;
}


Error StreamingServerService:: appendStopScriptCommand (
	Buffer_p pbServiceScriptFile)

{

	if (pbServiceScriptFile -> append (
		"\t. /catraStreamingServer.sh" "\n"
		"\n"
		"\n"
		"\tif [ ! -f " SS_STREAMINGSERVERSERVICE_PIDFILEPATHNAME " ] ; then"
		"\n"
		"\t\techo \"PID file not found. Streaming Server is Not Running\"" "\n"
		"\t\techo \"PID file not found. Streaming Server is Not Running\" >> "
		TOOLS_SERVICE_DEBUGFILE "\n"
		"\n"
		"\t\treturn 1" "\n"
		"\tfi" "\n"
		"\n"
		"\tpIPAddressForHTTP=`getConfigurationItemValue $"
		) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		/* _tSystemTracer is not yet initialized
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/

		return err;
	}

	if (pbServiceScriptFile -> append (
		SS_STREAMINGSERVERSERVICE_CONFPATHNAMEENVIRONMENTVARIABLE
		) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		/* _tSystemTracer is not yet initialized
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/

		return err;
	}

	if (pbServiceScriptFile -> append (
		" IPConfiguration LocalIPAddressForHTTP`" "\n"
		) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		/* _tSystemTracer is not yet initialized
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/

		return err;
	}

	if (pbServiceScriptFile -> append (
		"\tulHTTPRequestsPort=`getConfigurationItemValue $"
		) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		/* _tSystemTracer is not yet initialized
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/

		return err;
	}

	if (pbServiceScriptFile -> append (
		SS_STREAMINGSERVERSERVICE_CONFPATHNAMEENVIRONMENTVARIABLE
		) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		/* _tSystemTracer is not yet initialized
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/

		return err;
	}

	if (pbServiceScriptFile -> append (
		" IPConfiguration HTTPRequestsPort`" "\n"
		) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		/* _tSystemTracer is not yet initialized
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/

		return err;
	}

	if (pbServiceScriptFile -> append (
		"\thttpGetThread $pIPAddressForHTTP $ulHTTPRequestsPort /catraStreamingServer \"?command=shutdown\" > /dev/null" "\n"
		"\n"
		"\tif [ \"$?\" = \"1\" ]; then" "\n"
		"\t\techo \"httpGetThread failed. Error: $?\"" "\n"
		"\t\techo \"httpGetThread failed. Error: $?\" >> "
		TOOLS_SERVICE_DEBUGFILE "\n"
		"\n"
		"\t\treturn 1" "\n"
		"\tfi" "\n"
		"\n"
		"\tNUM_SECONDS=0" "\n"
		"\tMAX_SECONDS=60" "\n"
		"\tCATRASERVERRUNNING=1" "\n"
		"\n"
		"\twhile [ \"$NUM_SECONDS\" -lt \"$MAX_SECONDS\" ] && [ \"$CATRASERVERRUNNING\" = \"1\" ]; do" "\n"
		"\t\trhstatus" "\n"
		"\t\tif [ \"$CATRASERVERRUNNING\" = \"1\" ]; then" "\n"
		"\t\t\tsleep 1" "\n"
		"\t\t\tNUM_SECONDS=`expr $NUM_SECONDS + 1`" "\n"
		"\t\tfi" "\n"
		"\tdone" "\n"
		"\tif [ ! \"$CATRASERVERRUNNING\" = \"0\" ]; then" "\n"
		"\t\techo \"error: catra streaming server is not stopped\" >> "
		TOOLS_SERVICE_DEBUGFILE "\n"
		"\t\treturn 1" "\n"
		"\tfi" "\n"
		"\n"
		"\techo \"catra streaming server is stopped in $NUM_SECONDS seconds\" >> "
		TOOLS_SERVICE_DEBUGFILE "\n"
		"\trm -f " SS_STREAMINGSERVERSERVICE_PIDFILEPATHNAME "\n"
		"\n"
		"\treturn 0"
		) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		/* _tSystemTracer is not yet initialized
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/

		return err;
	}


	return errNoError;
}


Error StreamingServerService:: appendStatusScriptCommand (
	Buffer_p pbServiceScriptFile)

{

	if (pbServiceScriptFile -> append (
		"\t. /catraStreamingServer.sh" "\n"
		"\n"
		"#\tpid=`ps -ef | grep \"/catraStreamingServer\" | grep -v grep | awk '{print $2}'`" "\n"
		"\tpid=`cat " SS_STREAMINGSERVERSERVICE_PIDFILEPATHNAME "`" "\n"
		"#\tif [ \"$pid\" = \"\" ]; then" "\n"
		"\tcheckpid $pid" "\n"
		"\tif [ \"$?\" = \"1\" ]; then" "\n"
		"\t\tCATRASERVERRUNNING=0" "\n"
		"\t\techo \"Streaming Server is Not Running\"" "\n"
		"\t\techo \"Streaming Server is Not Running\" >> "
		TOOLS_SERVICE_DEBUGFILE "\n"
		"\telse" "\n"
		"\t\tCATRASERVERRUNNING=1" "\n"
		"\t\techo \"catra streaming server (pid $pid) is running...\"" "\n"
		"\t\techo \"catra streaming server (pid $pid) is running...\" >> "
		TOOLS_SERVICE_DEBUGFILE "\n"
		"\tfi" "\n"
		"\n"
		"\treturn 0"
		) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		/* _tSystemTracer is not yet initialized
		_tSystemTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
		*/

		return err;
	}


	return errNoError;
}

