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
#include "StringTokenizer.h"
#include "WebUtility.h"
#include "HttpGetThread.h"
#include "DateTime.h"
#include "RTSPUtility.h"
#include "SDPFor3GPP.h"
#include "SDPMediaFor3GPP.h"
#include "FileIO.h"
#include <sys/stat.h>
#ifdef WIN32
#else
	#include <netinet/in.h>
	#include <sys/time.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>



RTSPClientSession:: RTSPClientSession (void): PosixThread ()

{

}


RTSPClientSession:: ~RTSPClientSession (void)

{

}


Error RTSPClientSession:: init (
	const char *pURL,
	unsigned long ulVideoClientRTPPort,	unsigned long ulVideoClientRTCPPort,
	unsigned long ulAudioClientRTPPort,	unsigned long ulAudioClientRTCPPort,
	Boolean_t bTraceOnTTY, unsigned long ulSecondsToWaitRTPPackets,
	Tracer_p ptTracer)

{

	Error_t						errSocketInit;
	Error_t						errGeneric;


	_ulVideoClientRTPPort				= ulVideoClientRTPPort;
	_ulVideoClientRTCPPort				= ulVideoClientRTCPPort;
	_ulAudioClientRTPPort				= ulAudioClientRTPPort;
	_ulAudioClientRTCPPort				= ulAudioClientRTCPPort;
	_bTraceOnTTY						= bTraceOnTTY;
	_ulSecondsToWaitRTPPackets			= ulSecondsToWaitRTPPackets;
	_ptTracer							= ptTracer;
	
	if (_bURL. init (pURL) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (writeStatistics (
			(const char *) NULL, -1, -1,
			(const char *) err) != errNoError)
		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bRedirectedURL. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (writeStatistics (
			(const char *) NULL, -1, -1,
			(const char *) err) != errNoError)
		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bURL. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	_bIsRedirectedURL			= false;

	if (_bSession. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (writeStatistics (
			(const char *) NULL, -1, -1,
			(const char *) err) != errNoError)
		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRedirectedURL. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bURL. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (WebUtility:: parseURL ((const char *) _bURL, _pRTSPServerIPAddress,
		MP4PL_MAXIPADDRESSLENGTH, &_lRTSPServerPort) != errNoError)
	{
		Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_PARSEURL_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (writeStatistics (
			(const char *) NULL, -1, -1,
			(const char *) err) != errNoError)
		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bSession. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRedirectedURL. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bURL. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((errSocketInit = _csClientSocket. init (SocketImpl:: STREAM,
		30, 0, 30, 0, 0, true, (const char *) NULL, _pRTSPServerIPAddress,
		_lRTSPServerPort == -1 ? 554 : _lRTSPServerPort)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errSocketInit, __FILE__, __LINE__);

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_INIT_FAILED,
			2, _pRTSPServerIPAddress,
			(_lRTSPServerPort == -1 ? 554 : _lRTSPServerPort));
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (writeStatistics (
			(const char *) NULL, -1, -1,
			(const char *) errSocketInit) != errNoError)
		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bSession. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRedirectedURL. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bURL. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bSDP. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (writeStatistics (
			(const char *) NULL, -1, -1,
			(const char *) err) != errNoError)
		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_csClientSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bSession. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRedirectedURL. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bURL. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
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

		if (writeStatistics (
			(const char *) NULL, -1, -1,
			(const char *) err) != errNoError)
		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bSDP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_csClientSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bSession. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRedirectedURL. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bURL. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}


	return errNoError;
}


Error RTSPClientSession:: finish ()

{

	if (_bSDP. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_csClientSocket. finish () != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bSession. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bRedirectedURL. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bURL. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (PosixThread:: finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}


	return errNoError;
}


Error RTSPClientSession:: run (void)

{

	Buffer_t					bRtspHeaderResponse;
	Error_t						errGeneric;
	SocketImpl_p				psiClientRTSPSocketImpl;
	unsigned long				ulSequenceNumber;
	unsigned long				ulStatusCode;
	Boolean_t					bIsThereAudio;
	Boolean_t					bIsThereVideo;
	char						pControlValue [MP4PL_CONTROLVALUELENGTH];
	char						pVideoControlValue [MP4PL_CONTROLVALUELENGTH];
	char						pAudioControlValue [MP4PL_CONTROLVALUELENGTH];
	ServerSocket_t				sVideoRTPServerSocket;
	ServerSocket_t				sVideoRTCPServerSocket;
	ServerSocket_t				sAudioRTPServerSocket;
	ServerSocket_t				sAudioRTCPServerSocket;
	Error_t						errSocketInit;
	#ifdef WIN32
		__int64					ullDescribeRequest_UTCInMilliSecs;
		__int64					ullRedirectedDescribeRequest_UTCInMilliSecs;
		__int64					llTimeBetweenDescribeAndFirstPacket;
	#else
		unsigned long long		ullDescribeRequest_UTCInMilliSecs;
		unsigned long long		ullRedirectedDescribeRequest_UTCInMilliSecs;
		long long				llTimeBetweenDescribeAndFirstPacket;
	#endif
	char						pDescribeRequestTime [MP4PL_MAXDATETIMELENGTH];
	unsigned long				ulDescribeResponseTime;
	unsigned long				ulStartVideoSequenceNumber;
	unsigned long				ulStartAudioSequenceNumber;
	unsigned long				ulStartVideoTimeStamp;
	unsigned long				ulStartAudioTimeStamp;
	WebUtility:: HttpMethod_t	hmHttpMethod;


	{
		Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_STARTED);
		_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	if ((_erThreadReturn = _csClientSocket. getSocketImpl (
		&psiClientRTSPSocketImpl)) != errNoError)
	{
		// _erThreadReturn = SocketErrors (__FILE__, __LINE__,
		//	SCK_CLIENTSOCKET_GETSOCKETIMPL_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		if (writeStatistics (
			(const char *) NULL, -1, -1,
			(const char *) _erThreadReturn) != errNoError)
		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return _erThreadReturn;
	}

	ulSequenceNumber					= 0;

	if (bRtspHeaderResponse. init () != errNoError)
	{
		_erThreadReturn = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		if (writeStatistics (
			(const char *) NULL, -1, -1,
			(const char *) _erThreadReturn) != errNoError)
		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return _erThreadReturn;
	}

	// DESCRIBE management
	{
		SDPFor3GPP_t		sSDPFor3GPP;
		SDPMedia_p			psmSDPMediaVideo;
		SDPMedia_p			psmSDPMediaAudio;
		Error_t				errGetSDPMedia;


		{
			tm					tmDateTime;
			unsigned long		ulMilliSecs;


			if (DateTime:: get_tm_LocalTime (&tmDateTime,
				&ulMilliSecs) != errNoError)
			{
				_erThreadReturn = ToolsErrors (__FILE__, __LINE__,
					TOOLS_DATETIME_GET_TM_LOCALTIME_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (writeStatistics (
					(const char *) NULL, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}

			sprintf (pDescribeRequestTime, "%04d-%02d-%02d %02d:%02d:%02d:%lu",
				tmDateTime. tm_year + 1900,
				tmDateTime. tm_mon + 1,
				tmDateTime. tm_mday,
				tmDateTime. tm_hour,
				tmDateTime. tm_min,
				tmDateTime. tm_sec,
				ulMilliSecs);

			if (DateTime:: nowUTCInMilliSecs (
				&ullDescribeRequest_UTCInMilliSecs,
				(long *) NULL) != errNoError)
			{
				_erThreadReturn = ToolsErrors (__FILE__, __LINE__,
					TOOLS_DATETIME_NOWUTCINMILLISECS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (writeStatistics (
					(const char *) NULL, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}
		}

		if ((_erThreadReturn = sendDescribe ((const char *) _bURL,
			++ulSequenceNumber, psiClientRTSPSocketImpl)) != errNoError)
		{
			// _erThreadReturn = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
			//	MP4PL_RTSPCLIENTSESSION_SENDDESCRIBE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime,
				-1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		if ((_erThreadReturn = WebUtility:: readHttpHeaderAndBody (
			psiClientRTSPSocketImpl,
			30, 0, &bRtspHeaderResponse, &_bSDP, &hmHttpMethod,
			(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL,
			(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL)) != errNoError)
		{
			// if the answer of the DESCRIBE is a redirection the
			// streaming server answer does not contain the SDFP as body
			if ((long) _erThreadReturn !=
				WEBTOOLS_HTTPGETTHREAD_BODYTIMEOUTEXPIRED)
			{
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				Error err = WebToolsErrors (__FILE__, __LINE__,
					WEBTOOLS_WEBUTILITY_READHTTPHEADERANDBODY_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (writeStatistics (
					pDescribeRequestTime, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}
			else
				_erThreadReturn			= errNoError;
		}

		{
			Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
				2, (const char *) bRtspHeaderResponse,
				(const char *) _bSDP);
			_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
				__FILE__, __LINE__);
		}

		{
			#ifdef WIN32
				__int64					ullDescribeResponse_UTCInMilliSecs;
			#else
				unsigned long long		ullDescribeResponse_UTCInMilliSecs;
			#endif

			if (DateTime:: nowUTCInMilliSecs (
				&ullDescribeResponse_UTCInMilliSecs,
				(long *) NULL) != errNoError)
			{
				_erThreadReturn = ToolsErrors (__FILE__, __LINE__,
					TOOLS_DATETIME_NOWUTCINMILLISECS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (writeStatistics (
					(const char *) NULL, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}

			ulDescribeResponseTime		= (unsigned long)
				(ullDescribeResponse_UTCInMilliSecs -
				ullDescribeRequest_UTCInMilliSecs);
		}

		{
			Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_TIMEOFDESCRIBERESPONSE,
				1, ulDescribeResponseTime);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (RTSPUtility:: parseDESCRIBEResponse (
			(const char *) bRtspHeaderResponse, &ulStatusCode,
			&_bRedirectedURL, &ulSequenceNumber, &_bSession,
			(unsigned long *) NULL, (Buffer_p) NULL,
			_ptTracer) != errNoError)
		{
			_erThreadReturn = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_PARSEDESCRIBERESPONSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		// Moved Permanently???
		if (ulStatusCode == RTSP_STATUSCODE_MOVEDPERMANENTLY)
		{
			_bIsRedirectedURL			= true;

			if (WebUtility:: parseURL ((const char *) _bRedirectedURL,
				_pRTSPServerIPAddress, MP4PL_MAXIPADDRESSLENGTH,
				&_lRTSPServerPort) != errNoError)
			{
				_erThreadReturn = CatraMonitoringPlayerErrors (
					__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_PARSEURL_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (writeStatistics (
					pDescribeRequestTime, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}

			if (_csClientSocket. finish () != errNoError)
			{
				_erThreadReturn = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (writeStatistics (
					pDescribeRequestTime, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}

			if ((errSocketInit = _csClientSocket. init (SocketImpl:: STREAM,
				30, 0, 30, 0, 0, true, (const char *) NULL, _pRTSPServerIPAddress,
				_lRTSPServerPort == -1 ? 554 : _lRTSPServerPort)) != errNoError)
			{
				Error_t					errToTrace;

				#ifdef WIN32
					int						iErrno;
					unsigned long			ulUserDataBytes;

					errSocketInit. getUserData (&iErrno, &ulUserDataBytes);
					if (ulUserDataBytes != 0 &&
						iErrno == WSABASEERR + 60)	// WSAETIMEDOUT
					{
						errToTrace = CatraMonitoringPlayerErrors (
							__FILE__, __LINE__,
							MP4PL_RTSPCLIENTSESSION_ATTEMPTTOCONNECTTIMEDOUT);
					}
					else if (ulUserDataBytes != 0 &&
						iErrno == WSABASEERR + 61)	// WSAECONNREFUSED
					{
						errToTrace = CatraMonitoringPlayerErrors (
							__FILE__, __LINE__,
							MP4PL_RTSPCLIENTSESSION_CONNECTIONREJECTED);
					}
					else
						errToTrace			= errSocketInit;
				#else
						errToTrace			= errSocketInit;
				#endif

				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errToTrace, __FILE__, __LINE__);

				_erThreadReturn = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (writeStatistics (
					pDescribeRequestTime, -1, -1,
					(const char *) errToTrace) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}

			if ((_erThreadReturn = _csClientSocket. getSocketImpl (
				&psiClientRTSPSocketImpl)) != errNoError)
			{
				// _erThreadReturn = SocketErrors (__FILE__, __LINE__,
				//	SCK_CLIENTSOCKET_GETSOCKETIMPL_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (writeStatistics (
					pDescribeRequestTime, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}

			if (DateTime:: nowUTCInMilliSecs (
				&ullRedirectedDescribeRequest_UTCInMilliSecs,
				(long *) NULL) != errNoError)
			{
				_erThreadReturn = ToolsErrors (__FILE__, __LINE__,
					TOOLS_DATETIME_NOWUTCINMILLISECS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (writeStatistics (
					pDescribeRequestTime, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}

			if ((_erThreadReturn = sendDescribe ((const char *) _bRedirectedURL,
				++ulSequenceNumber, psiClientRTSPSocketImpl)) != errNoError)
			{
				// _erThreadReturn = CatraMonitoringPlayerErrors (
				//	__FILE__, __LINE__,
				//	MP4PL_RTSPCLIENTSESSION_SENDDESCRIBE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (writeStatistics (
					pDescribeRequestTime, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}

			if ((_erThreadReturn = WebUtility:: readHttpHeaderAndBody (
				psiClientRTSPSocketImpl,
				30, 0, &bRtspHeaderResponse, &_bSDP, &hmHttpMethod,
				(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL,
				(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL)) != errNoError)
			{
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				Error err = WebToolsErrors (__FILE__, __LINE__,
					WEBTOOLS_WEBUTILITY_READHTTPHEADERANDBODY_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (writeStatistics (
					pDescribeRequestTime, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}

			{
				Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
					2, (const char *) bRtspHeaderResponse,
					(const char *) _bSDP);
				_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
					__FILE__, __LINE__);
			}

			// updated ulDescribeResponseTime in case the response for the
			// redirected DESCRIBE took more time
			{
				#ifdef WIN32
					__int64					ullDescribeResponse_UTCInMilliSecs;
				#else
					unsigned long long		ullDescribeResponse_UTCInMilliSecs;
				#endif

				if (DateTime:: nowUTCInMilliSecs (
					&ullDescribeResponse_UTCInMilliSecs,
					(long *) NULL) != errNoError)
				{
					_erThreadReturn = ToolsErrors (__FILE__, __LINE__,
						TOOLS_DATETIME_NOWUTCINMILLISECS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) _erThreadReturn, __FILE__, __LINE__);

					if (writeStatistics (
						pDescribeRequestTime, -1, -1,
						(const char *) _erThreadReturn) != errNoError)
					{
						Error err = CatraMonitoringPlayerErrors (
							__FILE__, __LINE__,
							MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bRtspHeaderResponse. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return _erThreadReturn;
				}

				if (ullDescribeResponse_UTCInMilliSecs -
					ullRedirectedDescribeRequest_UTCInMilliSecs >
					ulDescribeResponseTime)
					ulDescribeResponseTime		= (unsigned long)
						(ullDescribeResponse_UTCInMilliSecs -
						ullRedirectedDescribeRequest_UTCInMilliSecs);
			}

			if (RTSPUtility:: parseDESCRIBEResponse (
				(const char *) bRtspHeaderResponse, &ulStatusCode,
				(Buffer_p) NULL, &ulSequenceNumber, &_bSession,
				(unsigned long *) NULL, (Buffer_p) NULL,
				_ptTracer) != errNoError)
			{
				_erThreadReturn = RTSPErrors (__FILE__, __LINE__,
					RTSP_RTSPUTILITY_PARSEDESCRIBERESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (writeStatistics (
					pDescribeRequestTime, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}
		}
		else
		{
			_bIsRedirectedURL			= false;
		}

		if (ulStatusCode != RTSP_STATUSCODE_OK)
		{
			_erThreadReturn = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPERROR,
				1, (const char *) bRtspHeaderResponse);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		if (sSDPFor3GPP. init (&_bSDP, _ptTracer) != errNoError)
		{
			_erThreadReturn = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		if (sSDPFor3GPP. getAttributeValue (
			"control", pControlValue) != errNoError)
		{
			_erThreadReturn = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_GETATTRIBUTEVALUE_FAILED,
				1, "control");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFOR3GPP_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		if ((errGetSDPMedia = sSDPFor3GPP. getSDPMedia (
			SDPMedia:: SDPMEDIA_VIDEO, 0,
			&psmSDPMediaVideo)) != errNoError)
		{
			if ((long) errGetSDPMedia != SDP_SDP_SDPMEDIANOTFOUND)
			{
				_erThreadReturn = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFOR3GPP_GETSDPMEDIA_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPFOR3GPP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (writeStatistics (
					pDescribeRequestTime, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}

			bIsThereVideo		= false;
		}
		else
		{
			bIsThereVideo		= true;

			if (((SDPMediaFor3GPP_p) psmSDPMediaVideo) -> getMediaInfo (
				(SDPMedia:: SDPMediaType_p) NULL,
				pVideoControlValue,
				(unsigned long *) NULL,
				(unsigned long *) NULL,
				(char *) NULL,
				(char *) NULL,
				(char *) NULL,
				(unsigned long *) NULL) != errNoError)
			{
				_erThreadReturn = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFOR3GPP_GETSDPMEDIA_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPFOR3GPP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (writeStatistics (
					pDescribeRequestTime, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}
		}

		if ((errGetSDPMedia = sSDPFor3GPP. getSDPMedia (
			SDPMedia:: SDPMEDIA_AUDIO, 0,
			&psmSDPMediaAudio)) != errNoError)
		{
			if ((long) errGetSDPMedia != SDP_SDP_SDPMEDIANOTFOUND)
			{
				_erThreadReturn = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFOR3GPP_GETSDPMEDIA_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPFOR3GPP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (writeStatistics (
					pDescribeRequestTime, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}

			bIsThereAudio		= false;
		}
		else
		{
			bIsThereAudio		= true;

			if (((SDPMediaFor3GPP_p) psmSDPMediaAudio) -> getMediaInfo (
				(SDPMedia:: SDPMediaType_p) NULL,
				pAudioControlValue,
				(unsigned long *) NULL,
				(unsigned long *) NULL,
				(char *) NULL,
				(char *) NULL,
				(char *) NULL,
				(unsigned long *) NULL) != errNoError)
			{
				_erThreadReturn = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFOR3GPP_GETSDPMEDIA_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPFOR3GPP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (writeStatistics (
					pDescribeRequestTime, -1, -1,
					(const char *) _erThreadReturn) != errNoError)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtspHeaderResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return _erThreadReturn;
			}
		}

		if (sSDPFor3GPP. finish () != errNoError)
		{
			_erThreadReturn = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		if (!bIsThereVideo && !bIsThereAudio)
		{
			_erThreadReturn = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_NOMEDIAFOUND);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}
	}

	if (bIsThereVideo)
	{
		if ((_erThreadReturn = sendSetup ((const char *) _bURL,
			pVideoControlValue, ++ulSequenceNumber,
			&_bSession,
			_ulVideoClientRTPPort, _ulVideoClientRTCPPort,
			psiClientRTSPSocketImpl)) != errNoError)
		{
			// _erThreadReturn = CatraMonitoringPlayerErrors (
			//		__FILE__, __LINE__,
			//	MP4PL_RTSPCLIENTSESSION_SENDSETUP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		if ((_erThreadReturn = WebUtility:: readHttpHeaderAndBody (
			psiClientRTSPSocketImpl,
			30, 0, &bRtspHeaderResponse, (Buffer_p) NULL, &hmHttpMethod,
			(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL,
			(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			Error err = WebToolsErrors (__FILE__, __LINE__,
				WEBTOOLS_WEBUTILITY_READHTTPHEADERANDBODY_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) err) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		{
			Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
				2, (const char *) bRtspHeaderResponse,
				"");
			_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
				__FILE__, __LINE__);
		}

		if ((_erThreadReturn = RTSPUtility:: parseSETUPResponse (
			(const char *) bRtspHeaderResponse,
			&ulStatusCode, &_bSession,
			(unsigned long *) NULL, (unsigned long *) NULL,
			&_ulVideoServerRTPPort, &_ulVideoServerRTCPPort, _ptTracer)) !=
			errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_PARSESETUPRESPONSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		if (ulStatusCode != RTSP_STATUSCODE_OK)
		{
			_erThreadReturn = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPERROR,
				1, (const char *) bRtspHeaderResponse);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		if ((_erThreadReturn = sVideoRTPServerSocket. init (
			(const char *) NULL, _ulVideoClientRTPPort,
			true, SocketImpl:: DGRAM, 30, 0)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) errSocketInit) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		if ((_erThreadReturn = sVideoRTCPServerSocket. init (
			(const char *) NULL, _ulVideoClientRTCPPort,
			true, SocketImpl:: DGRAM, 30, 0)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) errSocketInit) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sVideoRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}
	}

	if (bIsThereAudio)
	{
		if ((_erThreadReturn = sendSetup ((const char *) _bURL,
			pAudioControlValue, ++ulSequenceNumber,
			&_bSession,
			_ulAudioClientRTPPort, _ulAudioClientRTCPPort,
			psiClientRTSPSocketImpl)) != errNoError)
		{
			// _erThreadReturn = CatraMonitoringPlayerErrors (
			//	__FILE__, __LINE__,
			//	MP4PL_RTSPCLIENTSESSION_SENDSETUP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bIsThereVideo)
			{
				if (sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		if ((_erThreadReturn = WebUtility:: readHttpHeaderAndBody (
			psiClientRTSPSocketImpl,
			30, 0, &bRtspHeaderResponse, (Buffer_p) NULL, &hmHttpMethod,
			(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL,
			(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			Error err = WebToolsErrors (__FILE__, __LINE__,
				WEBTOOLS_WEBUTILITY_READHTTPHEADERANDBODY_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bIsThereVideo)
			{
				if (sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		{
			Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
				2, (const char *) bRtspHeaderResponse,
				"");
			_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
				__FILE__, __LINE__);
		}

		if ((_erThreadReturn = RTSPUtility:: parseSETUPResponse (
			(const char *) bRtspHeaderResponse,
			&ulStatusCode, &_bSession,
			(unsigned long *) NULL, (unsigned long *) NULL,
			&_ulAudioServerRTPPort, &_ulAudioServerRTCPPort, _ptTracer)) !=
			errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_PARSESETUPRESPONSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bIsThereVideo)
			{
				if (sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		if (ulStatusCode != RTSP_STATUSCODE_OK)
		{
			_erThreadReturn = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPERROR,
				1, (const char *) bRtspHeaderResponse);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bIsThereVideo)
			{
				if (sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		if ((_erThreadReturn = sAudioRTPServerSocket. init (
			(const char *) NULL, _ulAudioClientRTPPort,
			true, SocketImpl:: DGRAM, 30, 0)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) errSocketInit) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bIsThereVideo)
			{
				if (sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		if ((_erThreadReturn = sAudioRTCPServerSocket. init (
			(const char *) NULL, _ulAudioClientRTCPPort,
			true, SocketImpl:: DGRAM, 30, 0)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (writeStatistics (
				pDescribeRequestTime, -1, -1,
				(const char *) _erThreadReturn) != errNoError)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sAudioRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bIsThereVideo)
			{
				if (sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}
	}

	if ((_erThreadReturn = sendPlay ((const char *) _bURL,
		pControlValue,
		++ulSequenceNumber, &_bSession, psiClientRTSPSocketImpl)) !=
		errNoError)
	{
		// _erThreadReturn = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
		//	MP4PL_RTSPCLIENTSESSION_SENDPLAY_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		if (writeStatistics (
			pDescribeRequestTime, -1, -1,
			(const char *) _erThreadReturn) != errNoError)
		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bIsThereAudio)
		{
			if (sAudioRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sAudioRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (bIsThereVideo)
		{
			if (sVideoRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sVideoRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (bRtspHeaderResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return _erThreadReturn;
	}

	if ((_erThreadReturn = WebUtility:: readHttpHeaderAndBody (
		psiClientRTSPSocketImpl,
		30, 0, &bRtspHeaderResponse, (Buffer_p) NULL, &hmHttpMethod,
		(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL,
		(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		Error err = WebToolsErrors (__FILE__, __LINE__,
			WEBTOOLS_WEBUTILITY_READHTTPHEADERANDBODY_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (writeStatistics (
			pDescribeRequestTime, -1, -1,
			(const char *) _erThreadReturn) != errNoError)
		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bIsThereAudio)
		{
			if (sAudioRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sAudioRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (bIsThereVideo)
		{
			if (sVideoRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sVideoRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (bRtspHeaderResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return _erThreadReturn;
	}

	{
		Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
			2, (const char *) bRtspHeaderResponse,
			"");
		_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if ((_erThreadReturn = RTSPUtility:: parsePLAYResponse (
		(const char *) bRtspHeaderResponse,
		pVideoControlValue, pAudioControlValue,
		&ulStatusCode, (unsigned long *) NULL,
		&ulStartVideoSequenceNumber, &ulStartVideoTimeStamp,
		&ulStartAudioSequenceNumber, &ulStartAudioTimeStamp,
		_ptTracer)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_PARSEPLAYRESPONSE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (writeStatistics (
			pDescribeRequestTime, -1, -1,
			(const char *) _erThreadReturn) != errNoError)
		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bIsThereAudio)
		{
			if (sAudioRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sAudioRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (bIsThereVideo)
		{
			if (sVideoRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sVideoRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (bRtspHeaderResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return _erThreadReturn;
	}

	if (ulStatusCode != RTSP_STATUSCODE_OK)
	{
		_erThreadReturn = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPERROR,
			1, (const char *) bRtspHeaderResponse);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		if (writeStatistics (
			pDescribeRequestTime, -1, -1,
			(const char *) _erThreadReturn) != errNoError)
		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bIsThereAudio)
		{
			if (sAudioRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sAudioRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (bIsThereVideo)
		{
			if (sVideoRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sVideoRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (bRtspHeaderResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return _erThreadReturn;
	}

	{
		#ifdef WIN32
			__int64					ullFirstRTPPacket_UTCInMilliSecs;
		#else
			unsigned long long		ullFirstRTPPacket_UTCInMilliSecs;
		#endif

		ullFirstRTPPacket_UTCInMilliSecs			= 0;

		if ((_erThreadReturn = listenFirstRTPackets (
			&ullFirstRTPPacket_UTCInMilliSecs,
			bIsThereVideo, &sVideoRTPServerSocket,
			bIsThereAudio, &sAudioRTPServerSocket)) != errNoError)
		{
			// _erThreadReturn = CatraMonitoringPlayerErrors (
			//	__FILE__, __LINE__,
			//	MP4PL_RTSPCLIENTSESSION_GETRTPACKETS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) _erThreadReturn, __FILE__, __LINE__);

			#ifdef WIN32
				if (writeStatistics (
					pDescribeRequestTime, (__int64) ulDescribeResponseTime, -1,
					(const char *) _erThreadReturn) != errNoError)
			#else
				if (writeStatistics (
					pDescribeRequestTime, (long long) ulDescribeResponseTime,
					-1, (const char *) _erThreadReturn) != errNoError)
			#endif
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bIsThereAudio)
			{
				if ((errGeneric = sAudioRTCPServerSocket. finish ()) !=
					errNoError)
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

				if ((errGeneric = sAudioRTPServerSocket. finish ()) !=
					errNoError)
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);
			}

			if (bIsThereVideo)
			{
				if ((errGeneric = sVideoRTCPServerSocket. finish ()) !=
					errNoError)
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

				if ((errGeneric = sVideoRTPServerSocket. finish ()) !=
					errNoError)
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);
			}

			if (bRtspHeaderResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _erThreadReturn;
		}

		if (ullFirstRTPPacket_UTCInMilliSecs == 0)
			llTimeBetweenDescribeAndFirstPacket			= -1;
		else
			llTimeBetweenDescribeAndFirstPacket			=
				ullFirstRTPPacket_UTCInMilliSecs -
				ullDescribeRequest_UTCInMilliSecs;
	}

	{
		Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_TIMEBETWEENDESCRIBEANDFIRSTPACKET,
			1, llTimeBetweenDescribeAndFirstPacket == -1 ?
			0 : llTimeBetweenDescribeAndFirstPacket);
		_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

	#ifdef WIN32
		if (writeStatistics (
			pDescribeRequestTime, (__int64) ulDescribeResponseTime,
			llTimeBetweenDescribeAndFirstPacket,
			"Success") != errNoError)
	#else
		if (writeStatistics (
			pDescribeRequestTime,
			(long long) ulDescribeResponseTime,
			llTimeBetweenDescribeAndFirstPacket,
			"Success") != errNoError)
	#endif
	{
		_erThreadReturn = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		if (bIsThereAudio)
		{
			if ((errGeneric = sAudioRTCPServerSocket. finish ()) != errNoError)
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

			if ((errGeneric = sAudioRTPServerSocket. finish ()) != errNoError)
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);
		}

		if (bIsThereVideo)
		{
			if ((errGeneric = sVideoRTCPServerSocket. finish ()) != errNoError)
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

			if ((errGeneric = sVideoRTPServerSocket. finish ()) != errNoError)
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);
		}

		if (bRtspHeaderResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return _erThreadReturn;
	}

	if (sendTeardown ((const char *) _bURL,
		++ulSequenceNumber, &_bSession, psiClientRTSPSocketImpl) != errNoError)
	{
		_erThreadReturn = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_SENDTEARDOWN_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		if (bIsThereAudio)
		{
			if ((errGeneric = sAudioRTCPServerSocket. finish ()) != errNoError)
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

			if ((errGeneric = sAudioRTPServerSocket. finish ()) != errNoError)
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);
		}

		if (bIsThereVideo)
		{
			if ((errGeneric = sVideoRTCPServerSocket. finish ()) != errNoError)
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

			if ((errGeneric = sVideoRTPServerSocket. finish ()) != errNoError)
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);
		}

		if (bRtspHeaderResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return _erThreadReturn;
	}

	if ((_erThreadReturn = WebUtility:: readHttpHeaderAndBody (
		psiClientRTSPSocketImpl,
		30, 0, &bRtspHeaderResponse, (Buffer_p) NULL, &hmHttpMethod,
		(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL,
		(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		Error err = WebToolsErrors (__FILE__, __LINE__,
			WEBTOOLS_WEBUTILITY_READHTTPHEADERANDBODY_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bIsThereAudio)
		{
			if ((errGeneric = sAudioRTCPServerSocket. finish ()) != errNoError)
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

			if ((errGeneric = sAudioRTPServerSocket. finish ()) != errNoError)
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);
		}

		if (bIsThereVideo)
		{
			if ((errGeneric = sVideoRTCPServerSocket. finish ()) != errNoError)
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

			if ((errGeneric = sVideoRTPServerSocket. finish ()) != errNoError)
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);
		}

		if (bRtspHeaderResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return _erThreadReturn;
	}

	{
		Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
			2, (const char *) bRtspHeaderResponse,
			"");
		_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	/*
	if ((_erThreadReturn = RTSPUtility:: parseTEARDOWNResponse (
		(const char *) bRtspHeaderResponse,
		&ulStatusCode, (unsigned long *) NULL,
		_ptTracer)) != errNoError)
	{
		_erThreadReturn = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_PARSETEARDOWNRESPONSE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		if (bIsThereAudio)
		{
			if (sAudioRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sAudioRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (bIsThereVideo)
		{
			if (sVideoRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sVideoRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (bRtspHeaderResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return _erThreadReturn;
	}
	*/

	if (bIsThereAudio)
	{
		if (sAudioRTCPServerSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (sAudioRTPServerSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}
	}

	if (bIsThereVideo)
	{
		if (sVideoRTCPServerSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (sVideoRTPServerSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}
	}

	if (bRtspHeaderResponse. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	{
		Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSIONFINISHED);
		_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}


	return _erThreadReturn;
}


Error RTSPClientSession:: sendDescribe (const char *pURL,
	unsigned long ulSequenceNumber, SocketImpl_p psiClientRTSPSocketImpl)

{

	Buffer_t					bRTSPRequest;
	Error						errWrite;



	if (bRTSPRequest. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (RTSPUtility:: getDESCRIBERequest (
		pURL, ulSequenceNumber, "CatraMonitoringPlayer",
		&bRTSPRequest, _ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETDESCRIBEREQUEST_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bRTSPRequest. finish () != errNoError)
		{
				Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((errWrite = psiClientRTSPSocketImpl -> writeString (
		(const char *) bRTSPRequest, true, 0, 1000)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errWrite, __FILE__, __LINE__);

		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_SENDTOSERVERFAILED,
				1, (const char *) bRTSPRequest);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bRTSPRequest. finish () != errNoError)
		{
				Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_SENTTOSERVER,
			1, (const char *) bRTSPRequest);
		_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (bRTSPRequest. finish () != errNoError)
	{
			Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPClientSession:: sendSetup (const char *pURL,
	const char *pControlValue, unsigned long ulSequenceNumber,
	Buffer_p pbSession,
	unsigned long ulClientRTPPort, unsigned long ulClientRTCPPort,
	SocketImpl_p psiClientRTSPSocketImpl)

{

	Buffer_t					bRTSPRequest;
	Error						errWrite;



	if (bRTSPRequest. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (RTSPUtility:: getSETUPRequest (
		pURL, pControlValue, ulSequenceNumber,
		!strcmp ((const char *) (*pbSession), "") ? (const char *) NULL :
		(const char *) (*pbSession),
		ulClientRTPPort, ulClientRTCPPort,
		"CatraMonitoringPlayer", &bRTSPRequest, _ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETSETUPREQUEST_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bRTSPRequest. finish () != errNoError)
		{
				Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((errWrite = psiClientRTSPSocketImpl -> writeString (
		(const char *) bRTSPRequest, true, 0, 1000)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errWrite, __FILE__, __LINE__);

		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_SENDTOSERVERFAILED,
				1, (const char *) bRTSPRequest);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bRTSPRequest. finish () != errNoError)
		{
				Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_SENTTOSERVER,
			1, (const char *) bRTSPRequest);
		_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (bRTSPRequest. finish () != errNoError)
	{
			Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPClientSession:: sendPlay (const char *pURL,
	const char *pControlValue,
	unsigned long ulSequenceNumber, Buffer_p pbSession,
	SocketImpl_p psiClientRTSPSocketImpl)

{

	Buffer_t					bRTSPRequest;
	Error						errWrite;



	if (bRTSPRequest. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (RTSPUtility:: getPLAYRequest (
		pURL, pControlValue, ulSequenceNumber,
		(const char *) (*pbSession),
		"CatraMonitoringPlayer", &bRTSPRequest, _ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETSETUPREQUEST_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bRTSPRequest. finish () != errNoError)
		{
				Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((errWrite = psiClientRTSPSocketImpl -> writeString (
		(const char *) bRTSPRequest, true, 0, 1000)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errWrite, __FILE__, __LINE__);

		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_SENDTOSERVERFAILED,
				1, (const char *) bRTSPRequest);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bRTSPRequest. finish () != errNoError)
		{
				Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_SENTTOSERVER,
			1, (const char *) bRTSPRequest);
		_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (bRTSPRequest. finish () != errNoError)
	{
			Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPClientSession:: sendTeardown (const char *pURL,
	unsigned long ulSequenceNumber, Buffer_p pbSession,
	SocketImpl_p psiClientRTSPSocketImpl)

{

	Buffer_t					bRTSPRequest;
	Error						errWrite;



	if (bRTSPRequest. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (RTSPUtility:: getTEARDOWNRequest (
		pURL, ulSequenceNumber,
		(const char *) (*pbSession),
		"CatraMonitoringPlayer", &bRTSPRequest, _ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETSETUPREQUEST_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bRTSPRequest. finish () != errNoError)
		{
				Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((errWrite = psiClientRTSPSocketImpl -> writeString (
		(const char *) bRTSPRequest, true, 0, 1000)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errWrite, __FILE__, __LINE__);

		{
			Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_SENDTOSERVERFAILED,
				1, (const char *) bRTSPRequest);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bRTSPRequest. finish () != errNoError)
		{
				Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_SENTTOSERVER,
			1, (const char *) bRTSPRequest);
		_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}


	return errNoError;
}


#ifdef WIN32
	Error RTSPClientSession:: listenFirstRTPackets (
		__int64 *pullFirstRTPPacket_UTCInMilliSecs,
		Boolean_t bIsThereVideo, ServerSocket_p psVideoRTPServerSocket,
		Boolean_t bIsThereAudio, ServerSocket_p psAudioRTPServerSocket)
#else
	Error RTSPClientSession:: listenFirstRTPackets (
		unsigned long long *pullFirstRTPPacket_UTCInMilliSecs,
		Boolean_t bIsThereVideo, ServerSocket_p psVideoRTPServerSocket,
		Boolean_t bIsThereAudio, ServerSocket_p psAudioRTPServerSocket)
#endif

{

	SocketImpl_p				psiServerSocketImpl;
	Boolean_t					bPacketsFinished;
	Boolean_t					bIsReadyToRead;
	time_t						tStartListeningRTPPackets;
	unsigned long				ulMilliSecondsToWaitForRTPackets;

	unsigned long			ulTotalVideoBitstreamSize;
	unsigned long			ulTotalAudioBitstreamSize;
	unsigned long			ulLastVideoSequenceNumber;
	unsigned long			ulLastAudioSequenceNumber;
	Boolean_t				bReceivedFirstAudioRTPPacket;
	Boolean_t				bReceivedFirstVideoRTPPacket;

	{
		Message msg = CatraMonitoringPlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_STARTOTREADINGPACKETSFROMSERVER);
		_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	_ptTracer -> flushOfTraces ();

	// It is important to avoid to wait for a lot because
	// otherwise we lose packets
	// used in isReadyToRead
	ulMilliSecondsToWaitForRTPackets			= 100;

	ulTotalVideoBitstreamSize			= 0;
	ulTotalAudioBitstreamSize			= 0;
	ulLastVideoSequenceNumber			= 0;
	ulLastAudioSequenceNumber			= 0;
	bPacketsFinished					= false;
	tStartListeningRTPPackets						= time (NULL);
	bReceivedFirstAudioRTPPacket			= false;
	bReceivedFirstVideoRTPPacket			= false;

	while (!bPacketsFinished)
	{
		if (bIsThereAudio && bIsThereVideo)
		{
			if (bReceivedFirstAudioRTPPacket && bReceivedFirstVideoRTPPacket)
			{
				if (DateTime:: nowUTCInMilliSecs (
					pullFirstRTPPacket_UTCInMilliSecs,
					(long *) NULL) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_DATETIME_NOWUTCINMILLISECS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				break;
			}
		}
		else if (!bIsThereAudio && bIsThereVideo)
		{
			if (bReceivedFirstVideoRTPPacket)
			{
				if (DateTime:: nowUTCInMilliSecs (
					pullFirstRTPPacket_UTCInMilliSecs,
					(long *) NULL) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_DATETIME_NOWUTCINMILLISECS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				break;
			}
		}
		else if (bIsThereAudio && !bIsThereVideo)
		{
			if (bReceivedFirstAudioRTPPacket)
			{
				if (DateTime:: nowUTCInMilliSecs (
					pullFirstRTPPacket_UTCInMilliSecs,
					(long *) NULL) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_DATETIME_NOWUTCINMILLISECS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				break;
			}
		}

		if (time (NULL) - tStartListeningRTPPackets >
			_ulSecondsToWaitRTPPackets)
		{
			if (bIsThereAudio && bIsThereVideo)
			{
				if (!bReceivedFirstAudioRTPPacket &&
					!bReceivedFirstVideoRTPPacket)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_NORTPPACKETSRECEIVED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
				else if (bReceivedFirstAudioRTPPacket &&
					!bReceivedFirstVideoRTPPacket)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_NOAUDIORTPPACKETRECEIVED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
				else if (!bReceivedFirstAudioRTPPacket &&
					bReceivedFirstVideoRTPPacket)
				{
					Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_NOVIDEORTPPACKETRECEIVED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}
			else if (!bIsThereAudio && bIsThereVideo)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_NOVIDEORTPPACKETRECEIVED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
			else if (bIsThereAudio && !bIsThereVideo)
			{
				Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_NOAUDIORTPPACKETRECEIVED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (bIsThereVideo)
		{
			// Video RTP
			if (psVideoRTPServerSocket -> getSocketImpl (
				&psiServerSocketImpl) != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKET_GETSOCKETIMPL_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (psiServerSocketImpl -> isReadyForReading (&bIsReadyToRead,
				0, ulMilliSecondsToWaitForRTPackets) != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETIMPL_ISREADYFORREADING_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (bIsReadyToRead)
			{
				bReceivedFirstVideoRTPPacket		= true;
			}
		}

		if (bIsThereAudio)
		{
			// Audio RTP
			if (psAudioRTPServerSocket -> getSocketImpl (
				&psiServerSocketImpl) != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKET_GETSOCKETIMPL_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (psiServerSocketImpl -> isReadyForReading (&bIsReadyToRead,
				0, ulMilliSecondsToWaitForRTPackets) != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETIMPL_ISREADYFORREADING_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (bIsReadyToRead)
			{
				bReceivedFirstAudioRTPPacket		= true;
			}
		}
	}


	return errNoError;
}


#ifdef WIN32
	Error RTSPClientSession:: writeStatistics (const char *pDescribeRequestTime,
		__int64 llDescribeResponseTime, __int64 llTimeBetweenDescribeAndFirstPacket,
		const char *pError)
#else
	Error RTSPClientSession:: writeStatistics (const char *pDescribeRequestTime,
		long long llDescribeResponseTime, long long llTimeBetweenDescribeAndFirstPacket,
		const char *pError)
#endif

{

	char				pBuffer [MP4PL_MAXSTATISTICSLENGTH];
	char				pNewError [MP4PL_MAXSTATISTICSLENGTH];
	char				*pPointerToError;


	strcpy (pNewError, pError);

	while ((pPointerToError = strchr (pNewError, '\n')) != (char *) NULL)
		*pPointerToError			= ' ';

	while ((pPointerToError = strchr (pNewError, ',')) != (char *) NULL)
		*pPointerToError			= ';';

	#ifdef WIN32
		if (pDescribeRequestTime == (const char *) NULL)
			sprintf (pBuffer, "%s,NA,NA,NA,%s\n",
				(const char *) _bURL, pNewError);
		else if (llDescribeResponseTime == -1)
			sprintf (pBuffer, "%s,%s,NA,NA,%s\n",
				(const char *) _bURL,
				pDescribeRequestTime,
				pNewError);
		else if (llTimeBetweenDescribeAndFirstPacket == -1)
				sprintf (pBuffer, "%s,%s,%I64d,NA,%s\n",
					(const char *) _bURL, pDescribeRequestTime,
					llDescribeResponseTime,
					pNewError);
		else
			sprintf (pBuffer, "%s,%s,%I64d,%I64d,%s\n",
				(const char *) _bURL, pDescribeRequestTime,
				llDescribeResponseTime,
				llTimeBetweenDescribeAndFirstPacket,
				pNewError);
	#else
		if (pDescribeRequestTime == (const char *) NULL)
			sprintf (pBuffer, "%s,NA,NA,NA,%s\n",
				(const char *) _bURL, pNewError);
		else if (llDescribeResponseTime == -1)
			sprintf (pBuffer, "%s,%s,NA,NA,%s\n",
				(const char *) _bURL, pDescribeRequestTime, pNewError);
		else if (llTimeBetweenDescribeAndFirstPacket == -1)
				sprintf (pBuffer, "%s,%s,%lld,NA,%s\n",
					(const char *) _bURL, pDescribeRequestTime,
					llDescribeResponseTime,
					pNewError);
		else
			sprintf (pBuffer, "%s,%s,%lld,%lld,%s\n",
				(const char *) _bURL, pDescribeRequestTime,
				llDescribeResponseTime,
				llTimeBetweenDescribeAndFirstPacket,
				pNewError);
	#endif

	std:: cout << pBuffer << std:: endl;
	/*
	if (RTSPClientSession:: appendToStatisticsFile (pStatisticsFileName,
		pBuffer, _ptTracer) != errNoError)
	{
		Error err = CatraMonitoringPlayerErrors (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_APPENDTOSTATISTICSFILE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}
	*/


	return errNoError;
}


Error RTSPClientSession:: appendToStatisticsFile (const char *pStatisticsFileName,
	const char *pBuffer, Tracer_p ptTracer)

{

	#ifdef WIN32
		__int64							llBytesWritten;
	#else
		long long						llBytesWritten;
	#endif
	int							iFdStatistics;


	if (FileIO:: open (pStatisticsFileName,
		O_WRONLY | O_APPEND, &iFdStatistics) != errNoError)
	{
		// the first time he didn't find the file
		#ifdef WIN32
			if (FileIO:: open (pStatisticsFileName,
				O_WRONLY | O_TRUNC | O_CREAT,
				_S_IREAD | _S_IWRITE, &iFdStatistics) !=
				errNoError)
		#else
			if (FileIO:: open (pStatisticsFileName,
				O_WRONLY | O_TRUNC | O_CREAT,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
				&iFdStatistics) != errNoError)
		#endif
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_OPEN_FAILED, 1, pStatisticsFileName);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (FileIO:: writeChars (iFdStatistics,
		(char *) pBuffer, strlen (pBuffer),
		&llBytesWritten) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_WRITECHARS_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (FileIO:: close (iFdStatistics) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_CLOSE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (FileIO:: close (iFdStatistics) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_CLOSE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}

