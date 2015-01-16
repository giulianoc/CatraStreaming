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


#include "RTSPClientSession.h"
#include "MP4PlayerSocketsPool.h"
#include "Tracer.h"
#include "FileIO.h"
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
	long						lEndOfTheStreamingInSeconds;
	unsigned long				ulRTSPSessionsNumber;
	unsigned long				ulRTSPSessionIdentifier;
	unsigned long				ulTraceLevel;
	unsigned long				ulSecondsBetweenTwoCheckTraceToManage;
	RTSPClientSession_p			prcsRTSPClientSession;
	MP4PlayerSocketsPool		mpsMP4PlayerSocketsPool;
	unsigned long				ulCheckSocketsPoolPeriodInSeconds;
	unsigned long			ulAdditionalCheckSocketsPoolPeriodInMicroseconds;
	Boolean_t					bIsThereVideo;
	ServerSocket_p				psVideoRTPServerSocket;
	ServerSocket_p				psVideoRTCPServerSocket;
	Boolean_t					bIsThereAudio;
	ServerSocket_p				psAudioRTPServerSocket;
	ServerSocket_p				psAudioRTCPServerSocket;
	unsigned long				ulIntervalInMilliSecs;
	unsigned long				ulIntervalInSeconds;
	unsigned long				ulIntervalInMicroSeconds;
	Error						errGeneric;
	Error						errJoin;
	Tracer_t					gtTracer;
	unsigned long				ulStartingPort;
	Boolean_t					bBitstreamFile;
	Boolean_t					bTraceOnTTY;
	unsigned long				ulVideoPacketsNumberReceived;
	unsigned long				ulAudioPacketsNumberReceived;
	unsigned long				ulLostVideoPacketsNumber;
	unsigned long				ulLostAudioPacketsNumber;


	{
		char		*pCopyright = "\n***************************************************************************\ncopyright            : (C) by CatraSoftware\nemail                : catrastreaming-support@catrasoftware.it\n***************************************************************************\n";

		std:: cout << pCopyright << std:: endl;
	}

	if (argc < 2 || argc > 9)
	{
		std:: cout << "Main functionalities:"
			<< std:: endl << "\t1. RTSP communications with the streaming server"
			<< std:: endl << "\t2. multiple requests to the server"
			<< std:: endl << "\t3. save the received bitstreams"
			<< std:: endl << "\t4. trace statistics information to be used to create graphics"
			<< std:: endl
			<< std:: endl << "Remark 1: The tool create a 'trace' directory to put his logs"
			<< std:: endl
			<< std:: endl << "Remark 2: If you catch the bitstream, be carefull because the network"
			<< std:: endl << "\tcan drop some packets. To see if some pachets are dropped by the network"
			<< std:: endl << "\tlook the trace file for 'RTP packet not received'. Note that the player"
			<< std:: endl << "\tis not able to catch the last dropped packets (if there are)."
			<< std:: endl
			<< std:: endl << "Remark 3: If you run 2 instances of this tool on the same machine,"
			<< std:: endl << "\tremember to set correctly the <Starting port for RTPs/RTCPs)>"
			<< std:: endl << "\tparameter to avoid that both the mp4players use the same ports."
			<< std:: endl << "\tNotes that a request including audio and video uses 4 ports"
			<< std:: endl
			<< std:: endl << "Remark 4: In the standard output:"
			<< std:: endl << "\tCV means that the player received a RTCP packet for the video track"
			<< std:: endl << "\tRV means that the player received a RTP packet for the video track"
			<< std:: endl << "\tCA means that the player received a RTCP packet for the audio track"
			<< std:: endl << "\tRA means that the player received a RTP packet for the audio track"
			<< std:: endl << std:: endl
			<< "----------------------------------"
			<< std:: endl << std:: endl
			<< "Usage: " << argv [0]
			<< std:: endl
			<< "\t<RTSP URL>"
			<< std:: endl
			<< "\t<end of the request in seconds. -1 means the end of the clip>. Default: -1"
			<< std:: endl
			<< "\t<Retries number>. Default: 1"
			<< std:: endl
			<< "\t<Interval in milli-seconds between 2 requests>. Default: 2000"
			<< std:: endl
			<< "\t<Starting port for RTPs/RTCPs)>. Default: 2000"
			<< std:: endl
			<< "\t<Bitstream files: false (0), true (1))>. Default: 0"
			<< std:: endl
			<< "\t<trace level 0..6 (0 -> all (LDBG1), 6 -> fatal error (LINFO))>. Default: 5"
			<< std:: endl
			<< "\t<Trace on the standard output 0 (false), 1 (true)>. Default: 1"
			<< std:: endl << std:: endl
			<< std:: endl << "Example: " << argv [0] << " rtsp://10.214.96.83/ad1.3gp -1 1 0 2000 1 0 1"
			<< std:: endl;

		return 1;
	}

	pRTSPURL					= argv [1];

	if (argc == 2)
	{
		lEndOfTheStreamingInSeconds			= -1;
		ulRTSPSessionsNumber				= 1;
		ulIntervalInMilliSecs				= 2000;
		ulStartingPort						= 2000;
		bBitstreamFile						= false;
		ulTraceLevel						= 5;
		bTraceOnTTY							= 1;
	}
	else if (argc == 3)
	{
		lEndOfTheStreamingInSeconds			= atol (argv [2]);
		ulRTSPSessionsNumber				= 1;
		ulIntervalInMilliSecs				= 2000;
		ulStartingPort						= 2000;
		bBitstreamFile						= false;
		ulTraceLevel						= 5;
		bTraceOnTTY							= 1;
	}
	else if (argc == 4)
	{
		lEndOfTheStreamingInSeconds			= atol (argv [2]);
		ulRTSPSessionsNumber				= atol (argv [3]);
		ulIntervalInMilliSecs				= 2000;
		ulStartingPort						= 2000;
		bBitstreamFile						= false;
		ulTraceLevel						= 5;
		bTraceOnTTY							= 1;
	}
	else if (argc == 5)
	{
		lEndOfTheStreamingInSeconds			= atol (argv [2]);
		ulRTSPSessionsNumber				= atol (argv [3]);
		ulIntervalInMilliSecs				= atol (argv [4]);
		ulStartingPort						= 2000;
		bBitstreamFile						= false;
		ulTraceLevel						= 5;
		bTraceOnTTY							= 1;
	}
	else if (argc == 6)
	{
		lEndOfTheStreamingInSeconds			= atol (argv [2]);
		ulRTSPSessionsNumber				= atol (argv [3]);
		ulIntervalInMilliSecs				= atol (argv [4]);
		ulStartingPort						= atol (argv [5]);
		bBitstreamFile						= false;
		ulTraceLevel						= 5;
		bTraceOnTTY							= 1;
	}
	else if (argc == 7)
	{
		lEndOfTheStreamingInSeconds			= atol (argv [2]);
		ulRTSPSessionsNumber				= atol (argv [3]);
		ulIntervalInMilliSecs				= atol (argv [4]);
		ulStartingPort						= atol (argv [5]);
		bBitstreamFile						=
			atol (argv [6]) == 1 ? true : false;
		ulTraceLevel						= 5;
		bTraceOnTTY							= 1;
	}
	else if (argc == 8)
	{
		lEndOfTheStreamingInSeconds			= atol (argv [2]);
		ulRTSPSessionsNumber				= atol (argv [3]);
		ulIntervalInMilliSecs				= atol (argv [4]);
		ulStartingPort						= atol (argv [5]);
		bBitstreamFile						=
			atol (argv [6]) == 1 ? true : false;
		ulTraceLevel						= atol (argv [7]);
		bTraceOnTTY							= 1;
	}
	else if (argc == 9)
	{
		lEndOfTheStreamingInSeconds			= atol (argv [2]);
		ulRTSPSessionsNumber				= atol (argv [3]);
		ulIntervalInMilliSecs				= atol (argv [4]);
		ulStartingPort						= atol (argv [5]);
		bBitstreamFile						=
			atol (argv [6]) == 1 ? true : false;
		ulTraceLevel						= atol (argv [7]);
		bTraceOnTTY							= atol (argv [8]);
	}
	else // if (argc == 10)
	{
		lEndOfTheStreamingInSeconds			= atol (argv [2]);
		ulRTSPSessionsNumber				= atol (argv [3]);
		ulIntervalInMilliSecs				= atol (argv [4]);
		ulStartingPort						= atol (argv [5]);
		bBitstreamFile						=
			atol (argv [6]) == 1 ? true : false;
		ulTraceLevel						= atol (argv [7]);
		bTraceOnTTY							= atol (argv [8]);
	}

	ulSecondsBetweenTwoCheckTraceToManage		= 7;

	// I will not handle the error in case the directory already exists
	#ifdef WIN32
		_mkdir ("./trace");
	#else
		mkdir ("./trace",
			S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP |
			S_IXGRP | S_IROTH | S_IXOTH);
	#endif

	if ((errGeneric = gtTracer. init (
		"MP4Player",					// pName
		-1,								// lCacheSizeOfTraceFile K-byte
		"./trace",						// pBaseTraceFileName
		"@MM@_@DD@_@HI24@_MP4Player",	// pBaseTraceFileName
		1000,							// lMaxTraceFileSize K-byte
		60 * 5,							// lTraceFilePeriodInSecs
		false,							// bCompressedTraceFile
		false,							// bClosedFileToBeCopied
		(const char *) NULL,			// pClosedFilesRepository
		1000,							// lTraceFilesNumberToMaintain
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

		return 1;
	}

	if ((errGeneric = gtTracer. start ()) != errNoError)
	{
		std:: cerr << (const char *) errGeneric << std:: endl;

		errGeneric = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_START_FAILED);
		std:: cerr << (const char *) errGeneric << std:: endl;

		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		return 1;
	}

	if ((prcsRTSPClientSession = new RTSPClientSession_t [
		ulRTSPSessionsNumber]) == (RTSPClientSession_p) NULL)
	{
		Error err = MP4PlayerErrors (__FILE__, __LINE__,
			MP4PL_NEW_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			std:: cerr << ((const char *) errGeneric) << std:: endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		return 1;
	}

	ulCheckSocketsPoolPeriodInSeconds						= 0;
	ulAdditionalCheckSocketsPoolPeriodInMicroseconds		= 200000;

	if (mpsMP4PlayerSocketsPool. init (
		ulRTSPSessionsNumber + ulRTSPSessionsNumber * 4,
		ulCheckSocketsPoolPeriodInSeconds,
		ulAdditionalCheckSocketsPoolPeriodInMicroseconds,
		&gtTracer) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETSPOOL_INIT_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete [] prcsRTSPClientSession;

		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			std:: cerr << ((const char *) errGeneric) << std:: endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		return 1;
	}

	if (mpsMP4PlayerSocketsPool. start () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_START_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (mpsMP4PlayerSocketsPool. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINISH_FAILED);
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete [] prcsRTSPClientSession;

		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			std:: cerr << ((const char *) errGeneric) << std:: endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		return 1;
	}

	ulIntervalInSeconds			= ulIntervalInMilliSecs / 1000;
	ulIntervalInMicroSeconds	= (ulIntervalInMilliSecs -
		(ulIntervalInSeconds * 1000)) * 1000;

	for (ulRTSPSessionIdentifier = 0;
		ulRTSPSessionIdentifier < ulRTSPSessionsNumber;
		ulRTSPSessionIdentifier++)
	{
		if ((errGeneric = (prcsRTSPClientSession [ulRTSPSessionIdentifier]).
			init (
			pRTSPURL, ulRTSPSessionIdentifier + 1,
			lEndOfTheStreamingInSeconds,
			ulStartingPort + 1, ulStartingPort + 2,
			ulStartingPort + 3,	ulStartingPort + 4,
			bTraceOnTTY, &gtTracer)) != errNoError)
		{
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_INIT_FAILED);
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			// manca il finish dei thread
			if (mpsMP4PlayerSocketsPool. cancel () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_CANCEL_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (mpsMP4PlayerSocketsPool. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINISH_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] prcsRTSPClientSession;

			if ((errGeneric = gtTracer. cancel ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. finish (true)) != errNoError)
				std:: cerr << (const char *) errGeneric << std:: endl;

			return 1;
		}

		if ((errGeneric = (prcsRTSPClientSession [ulRTSPSessionIdentifier]).
			initialRTSPHandshake (
			&bIsThereVideo,
			&psVideoRTPServerSocket,
			&psVideoRTCPServerSocket,
			&bIsThereAudio,
			&psAudioRTPServerSocket,
			&psAudioRTCPServerSocket
			)) != errNoError)
		{
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_INITIALRTSPHANDSHAKE_FAILED);
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			// manca il finish dei thread

			if (mpsMP4PlayerSocketsPool. cancel () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_CANCEL_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (mpsMP4PlayerSocketsPool. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINISH_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] prcsRTSPClientSession;

			if ((errGeneric = gtTracer. cancel ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. finish (true)) != errNoError)
				std:: cerr << (const char *) errGeneric << std:: endl;

			return 1;
		}

		if (bIsThereVideo)
		{
			if (mpsMP4PlayerSocketsPool. addSocket (
				SocketsPool:: SOCKETSTATUS_READ |
				SocketsPool:: SOCKETSTATUS_EXCEPTION,
				MP4PL_MP4PLAYERSOCKETSPOOL_VIDEORTPSERVERSOCKET,
				psVideoRTPServerSocket,
				&(prcsRTSPClientSession [ulRTSPSessionIdentifier])) !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_ADDSOCKET_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				// manca il finish dei thread

				if (mpsMP4PlayerSocketsPool. cancel () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PTHREAD_CANCEL_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (mpsMP4PlayerSocketsPool. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SOCKETSPOOL_FINISH_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete [] prcsRTSPClientSession;

				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			if (mpsMP4PlayerSocketsPool. addSocket (
				SocketsPool:: SOCKETSTATUS_READ |
				SocketsPool:: SOCKETSTATUS_EXCEPTION,
				MP4PL_MP4PLAYERSOCKETSPOOL_VIDEORTCPSERVERSOCKET,
				psVideoRTCPServerSocket,
				&(prcsRTSPClientSession [ulRTSPSessionIdentifier])) !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_ADDSOCKET_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				// manca il finish dei thread

				if (mpsMP4PlayerSocketsPool. cancel () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PTHREAD_CANCEL_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (mpsMP4PlayerSocketsPool. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SOCKETSPOOL_FINISH_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete [] prcsRTSPClientSession;

				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}
		}

		if (bIsThereAudio)
		{
			if (mpsMP4PlayerSocketsPool. addSocket (
				SocketsPool:: SOCKETSTATUS_READ |
				SocketsPool:: SOCKETSTATUS_EXCEPTION,
				MP4PL_MP4PLAYERSOCKETSPOOL_AUDIORTPSERVERSOCKET,
				psAudioRTPServerSocket,
				&(prcsRTSPClientSession [ulRTSPSessionIdentifier])) !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_ADDSOCKET_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				// manca il finish dei thread

				if (mpsMP4PlayerSocketsPool. cancel () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PTHREAD_CANCEL_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (mpsMP4PlayerSocketsPool. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SOCKETSPOOL_FINISH_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete [] prcsRTSPClientSession;

				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			if (mpsMP4PlayerSocketsPool. addSocket (
				SocketsPool:: SOCKETSTATUS_READ |
				SocketsPool:: SOCKETSTATUS_EXCEPTION,
				MP4PL_MP4PLAYERSOCKETSPOOL_AUDIORTCPSERVERSOCKET,
				psAudioRTCPServerSocket,
				&(prcsRTSPClientSession [ulRTSPSessionIdentifier])) !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_ADDSOCKET_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				// manca il finish dei thread

				if (mpsMP4PlayerSocketsPool. cancel () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PTHREAD_CANCEL_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (mpsMP4PlayerSocketsPool. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SOCKETSPOOL_FINISH_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete [] prcsRTSPClientSession;

				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}
		}

		ulStartingPort			+= 4;

		if ((errGeneric = (prcsRTSPClientSession [ulRTSPSessionIdentifier]).
			start ()) != errNoError)
		{
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_START_FAILED);
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			// manca il finish dei thread

			if (mpsMP4PlayerSocketsPool. cancel () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_CANCEL_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (mpsMP4PlayerSocketsPool. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINISH_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] prcsRTSPClientSession;

			if ((errGeneric = gtTracer. cancel ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. finish (true)) != errNoError)
				std:: cerr << (const char *) errGeneric << std:: endl;

			return 1;
		}

		if (ulRTSPSessionIdentifier + 1 != ulRTSPSessionsNumber)
		{
			{
				Message msg = MP4PlayerMessages (__FILE__, __LINE__,
					MP4PL_WAITFOR,
					2, ulIntervalInSeconds, ulIntervalInMicroSeconds);
				gtTracer. trace (Tracer:: TRACER_LDBG5, (const char *) msg,
					__FILE__, __LINE__);
			}

			if ((errGeneric = PosixThread:: getSleep (
				ulIntervalInSeconds, ulIntervalInMicroSeconds)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_GETSLEEP_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				// manca il finish dei thread

				if (mpsMP4PlayerSocketsPool. cancel () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PTHREAD_CANCEL_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (mpsMP4PlayerSocketsPool. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SOCKETSPOOL_FINISH_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete [] prcsRTSPClientSession;

				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}
		}
	}

	{
		Message msg = MP4PlayerMessages (__FILE__, __LINE__,
			MP4PL_WAITINGTHEFINISHING);
		gtTracer. trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

	for (ulRTSPSessionIdentifier = 0;
		ulRTSPSessionIdentifier < ulRTSPSessionsNumber;
		ulRTSPSessionIdentifier++)
	{
		if ((errGeneric = (prcsRTSPClientSession [ulRTSPSessionIdentifier]).
			join (&errJoin)) != errNoError)
		{
			/* already traced inside the thread
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);
			*/

			if (errJoin != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errJoin, __FILE__, __LINE__);

				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_JOIN_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((prcsRTSPClientSession [ulRTSPSessionIdentifier]).
				getStatisticsInformation (
					&ulVideoPacketsNumberReceived,
					&ulAudioPacketsNumberReceived,
					&ulLostVideoPacketsNumber,
					&ulLostAudioPacketsNumber) != errNoError)
			{
				Error err = MP4PlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_GETSTATISTICSINFORMATION_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			std:: cout
				<< pRTSPURL << ","
				<< (const char *) errGeneric << ","
				<< ulVideoPacketsNumberReceived << ","
				<< ulAudioPacketsNumberReceived << ","
				<< ulLostVideoPacketsNumber << ","
				<< ulLostAudioPacketsNumber << std:: endl;

			/* don't care if a thread give to us an error
			if ((errGeneric = gtTracer. cancel ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. finish (true)) != errNoError)
				std:: cerr << (const char *) errGeneric << std:: endl;

			return 1;
			*/
		}
		else
		{
			if ((prcsRTSPClientSession [ulRTSPSessionIdentifier]).
				getStatisticsInformation (
					&ulVideoPacketsNumberReceived,
					&ulAudioPacketsNumberReceived,
					&ulLostVideoPacketsNumber,
					&ulLostAudioPacketsNumber) != errNoError)
			{
				Error err = MP4PlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_GETSTATISTICSINFORMATION_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			std:: cout
				<< pRTSPURL << ","
				<< "SUCCESS" << ","
				<< ulVideoPacketsNumberReceived << ","
				<< ulAudioPacketsNumberReceived << ","
				<< ulLostVideoPacketsNumber << ","
				<< ulLostAudioPacketsNumber << std:: endl;
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

		// scrivere un messaggio conle seguenti informazioni:
		// ulRTSPSessionIdentifier, pDescribeTime, llDescribeResponseTime,
		// llTimeBetweenDescribeAndFirstPacket, packets lost, Error or Success

		if (bBitstreamFile && ulRTSPSessionIdentifier == 0)
		{
			if ((errGeneric = (prcsRTSPClientSession [ulRTSPSessionIdentifier]).
				saveBitStreams ()) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4PlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_SAVEBITSTREAMS_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				// manca il finish dei thread

				if (mpsMP4PlayerSocketsPool. cancel () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PTHREAD_CANCEL_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (mpsMP4PlayerSocketsPool. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SOCKETSPOOL_FINISH_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete [] prcsRTSPClientSession;

				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}
		}

		if ((errGeneric = (prcsRTSPClientSession [ulRTSPSessionIdentifier]).
			finish (true)) != errNoError)
		{
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_FINISH_FAILED);
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			// manca il finish dei thread

			if (mpsMP4PlayerSocketsPool. cancel () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PTHREAD_CANCEL_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (mpsMP4PlayerSocketsPool. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_FINISH_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete [] prcsRTSPClientSession;

			if ((errGeneric = gtTracer. cancel ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. finish (true)) != errNoError)
				std:: cerr << (const char *) errGeneric << std:: endl;

			return 1;
		}
	}

	if (mpsMP4PlayerSocketsPool. cancel () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_CANCEL_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (mpsMP4PlayerSocketsPool. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_FINISH_FAILED);
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete [] prcsRTSPClientSession;

		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			std:: cerr << ((const char *) errGeneric) << std:: endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		return 1;
	}

	if (mpsMP4PlayerSocketsPool. finish () != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETSPOOL_FINISH_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete [] prcsRTSPClientSession;

		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			std:: cerr << ((const char *) errGeneric) << std:: endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		return 1;
	}

	delete [] prcsRTSPClientSession;

	if ((errGeneric = gtTracer. cancel ()) != errNoError)
		std:: cerr << ((const char *) errGeneric) << std:: endl;
	if ((errGeneric = gtTracer. finish (true)) != errNoError)
		std:: cerr << (const char *) errGeneric << std:: endl;


	return 0;
}

