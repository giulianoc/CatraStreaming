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


#include "Tracer.h"
#include "FileIO.h"
#include "RTSPClientSession.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include "PMutex.h"
#ifdef WIN32
	#include <direct.h>
	#include <stdio.h>
#else
	#include <sys/time.h>
#endif


#define MP4PL_MAXSTATISTICSTITLELENGTH			1024 + 1


int main (int argc, char **argv)

{

	const char					*pRTSPURL;
	const char					*pLogsDirectory;
	RTSPClientSession_t			rcsRTSPClientSession;

	unsigned long				ulTraceLevel;
	unsigned long				ulSecondsBetweenTwoCheckTraceToManage;
	Error						errGeneric;
	Error						errJoin;
	Tracer_t					gtTracer;
	unsigned long				ulStartingPort;
	Boolean_t					bTraceOnTTY;
	unsigned long				ulSecondsToWaitRTPPackets;


	if (argc < 3 || argc > 7)
	{
		{
			char		*pCopyright = "\n***************************************************************************\ncopyright            : (C) by CatraSoftware\nemail                : catrastreaming-support@catrasoftware.it\n***************************************************************************\n";

			std:: cout << pCopyright << std:: endl;
		}

		std:: cout << std:: endl << std:: endl
			<< "Usage: " << argv [0]
			<< std:: endl
			<< "\t<RTSP URL>"
			<< std:: endl
			<< "\t<Directory for the logs>"
			<< std:: endl
			<< "\t<Starting port for RTPs/RTCPs)>. Default: 2000"
			<< std:: endl
			<< "\t<Timeout in seconds waiting RTP Packets>. Default: 15"
			<< std:: endl
			<< "\t<trace level 0..6 (0 -> all, 6 -> fatal error)>. Default: 0"
			<< std:: endl << std:: endl
			<< "\t<Trace on the standard output 0 (false), 1 (true)>. Default: 0"
			<< std:: endl
			<< std:: endl << "Example: " << argv [0] << " rtsp://10.214.96.83/ad1.3gp 2000 15 5 0"
			<< std:: endl << "Output format: <URL, RTSP Describe time,RTSP Describe response time (millisecs),Time between Describe and the RTP first packet (millisecs),Success|Error description>"
			<< std:: endl;

		return 1;
	}

	pRTSPURL					= argv [1];
	pLogsDirectory				= argv [2];

	if (argc == 3)
	{
		ulStartingPort				= 2000;
		ulSecondsToWaitRTPPackets	= 15;
		ulTraceLevel				= 0;
		bTraceOnTTY					= false;
	}
	else if (argc == 4)
	{
		ulStartingPort				= atol (argv [3]);
		ulSecondsToWaitRTPPackets	= 15;
		ulTraceLevel				= 0;
		bTraceOnTTY					= false;
	}
	else if (argc == 5)
	{
		ulStartingPort				= atol (argv [3]);
		ulSecondsToWaitRTPPackets	= atol (argv [4]);
		ulTraceLevel				= 0;
		bTraceOnTTY					= false;
	}
	else if (argc == 6)
	{
		ulStartingPort				= atol (argv [3]);
		ulSecondsToWaitRTPPackets	= atol (argv [4]);
		ulTraceLevel				= atol (argv [5]);
		bTraceOnTTY					= false;
	}
	else
	{
		ulStartingPort				= atol (argv [3]);
		ulSecondsToWaitRTPPackets	= atol (argv [4]);
		ulTraceLevel				= atol (argv [5]);
		bTraceOnTTY					=
			atol (argv [6]) == 1 ? true : false;
	}

	ulSecondsBetweenTwoCheckTraceToManage		= 2;

	// I will not handle the error in case the directory already exists
	#ifdef WIN32
		_mkdir (pLogsDirectory);
	#else
		mkdir (pLogsDirectory,
			S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	#endif

	if ((errGeneric = gtTracer. init (
		"CatraMonitoringPlayer",		// pName
		-1,								// lCacheSizeOfTraceFile K-byte
		pLogsDirectory,						// pBaseTraceFileName
		"@MM@_@DD@_@HI24@_CatraMonitoringPlayer",		// pBaseTraceFileName
		1000,							// lMaxTraceFileSize K-byte
		60 * 5,							// lTraceFilePeriodInSecs
		false,							// bCompressedTraceFile
		false,							// bClosedFileToBeCopied
		(const char *) NULL,			// pClosedFilesRepository
		9999,							// lTraceFilesNumberToMaintain
		true,							// bTraceOnFile
		bTraceOnTTY,					// bTraceOnTTY
		ulTraceLevel,					// lTraceLevel
		ulSecondsBetweenTwoCheckTraceToManage,
		3000,							// lMaxTracesNumber
		7532,							// lListenPort
		10000,							// lTracesNumberAllocatedOnOverflow
		1000)) != errNoError)			// lSizeOfEachBlockToGzip K-byte
	{
		std:: cerr << (const char *) errGeneric << std:: endl;

		errGeneric = TracerErrors (__FILE__, __LINE__,
			TRACER_TRACER_INIT_FAILED);
		std:: cerr << (const char *) errGeneric << std:: endl;

		// return 1;
		exit(1);
	}

	if ((errGeneric = gtTracer. start ()) != errNoError)
	{
		std:: cerr << (const char *) errGeneric << std:: endl;

		errGeneric = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_START_FAILED);
		std:: cerr << (const char *) errGeneric << std:: endl;

		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		// return 1;
		exit(1);
	}

	{
		Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_INIT,
			1, pRTSPURL);
		gtTracer. trace (Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	if ((errGeneric = rcsRTSPClientSession. init (pRTSPURL,
		ulStartingPort + 1, ulStartingPort + 2,
		ulStartingPort + 3,	ulStartingPort + 4, bTraceOnTTY,
		ulSecondsToWaitRTPPackets, &gtTracer)) != errNoError)
	{
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_INIT_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			std:: cerr << ((const char *) errGeneric) << std:: endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		// return 1;
		exit(1);
	}

	if ((errGeneric = rcsRTSPClientSession. start ()) !=
		errNoError)
	{
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_START_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		// manca il finish dei thread

		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			std:: cerr << ((const char *) errGeneric) << std:: endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		// return 1;
		exit(1);
	}

	if ((errGeneric = rcsRTSPClientSession. join (&errJoin)) !=
		errNoError)
	{
		// already traced inside the thread
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		if (errJoin != errNoError)
		{
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errJoin, __FILE__, __LINE__);

			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_JOIN_FAILED);
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			std:: cerr << ((const char *) errGeneric) << std:: endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		// return 1;
		exit(1);
	}

	if (errJoin != errNoError)
	{
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errJoin, __FILE__, __LINE__);

		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_JOIN_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if ((errGeneric = rcsRTSPClientSession. finish ()) !=
		errNoError)
	{
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_FINISH_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			std:: cerr << ((const char *) errGeneric) << std:: endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		// return 1;
		exit(1);
	}

	if ((errGeneric = gtTracer. cancel ()) != errNoError)
		std:: cerr << ((const char *) errGeneric) << std:: endl;
	if ((errGeneric = gtTracer. finish (true)) != errNoError)
		std:: cerr << (const char *) errGeneric << std:: endl;


	// return 0;
	exit(0);
}

