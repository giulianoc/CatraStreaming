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
#include "SDPFor3GPP.h"
#include "SDPMediaFor3GPP.h"
#include "HttpGetThread.h"
#include "WebUtility.h"
#include "RTSPUtility.h"
#include "FileIO.h"
#include "DateTime.h"
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
	const char *pURL, unsigned long ulRTSPSessionIdentifier,
	long lEndOfTheStreamingInSeconds,
	unsigned long ulVideoClientRTPPort,	unsigned long ulVideoClientRTCPPort,
	unsigned long ulAudioClientRTPPort,	unsigned long ulAudioClientRTCPPort,
	Boolean_t bTraceOnTTY, Tracer_p ptTracer)

{

	Error_t						errSocketInit;
	Error_t						errGeneric;


	_ulRTSPSessionIdentifier			= ulRTSPSessionIdentifier;
	_lEndOfTheStreamingInSeconds		= lEndOfTheStreamingInSeconds;
	_ulVideoClientRTPPort				= ulVideoClientRTPPort;
	_ulVideoClientRTCPPort				= ulVideoClientRTCPPort;
	_ulAudioClientRTPPort				= ulAudioClientRTPPort;
	_ulAudioClientRTCPPort				= ulAudioClientRTCPPort;
	_bTraceOnTTY						= bTraceOnTTY;
	_ptTracer							= ptTracer;

	_ulFirstVideoSequenceNumber			= 0;
	_ulLastVideoSequenceNumber			= (unsigned long) -1;
	_ulFirstVideoTimeStamp				= 0;
	_ulLastVideoTimeStamp				= 0;
	_ulFirstAudioSequenceNumber			= 0;
	_ulLastAudioSequenceNumber			= (unsigned long) -1;
	_ulFirstAudioTimeStamp				= 0;
	_ulLastAudioTimeStamp				= 0;
	_ulCurrentLostVideoPacketsNumber	= 0;
	_ulCurrentLostAudioPacketsNumber	= 0;

	_ulTotalVideoBitstreamSize			= 0;
	_ulTotalAudioBitstreamSize			= 0;

	_ullUTCInMilliSecsOfTheLastRTPPacketReceived		= 0;

	if ((_pURL = new char [strlen (pURL) + 1]) ==
		(char *) NULL)
	{
		Error err = MP4PlayerErrors (__FILE__, __LINE__,
			MP4PL_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pURL, pURL);

	if (WebUtility:: parseURL (_pURL,
		_pRTSPServerIPAddress, SCK_MAXIPADDRESSLENGTH,
		&_lRTSPServerPort, (Buffer_p) NULL, (Buffer_p) NULL) !=
		errNoError)
	{
		Error err = WebToolsErrors (__FILE__, __LINE__,
			WEBTOOLS_WEBUTILITY_PARSEURL_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete [] _pURL;
		_pURL			= (char *) NULL;

		return err;
	}

	_ulCurrentRTSPSequenceNumber					= 0;

	if ((errSocketInit = _csClientSocket. init (SocketImpl:: STREAM,
		30, 0, 30, 0, true, (const char *) NULL, _pRTSPServerIPAddress,
		_lRTSPServerPort == -1 ? 554 : _lRTSPServerPort)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errSocketInit, __FILE__, __LINE__);

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete [] _pURL;
		_pURL			= (char *) NULL;

		return err;
	}

	if (_csClientSocket. getSocketImpl (
		&_psiClientRTSPSocketImpl) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKET_GETSOCKETIMPL_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if ((errGeneric = _csClientSocket. finish ()) != errNoError)
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

		delete [] _pURL;
		_pURL			= (char *) NULL;

		return err;
	}

	if (_bRedirectedURL. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if ((errGeneric = _csClientSocket. finish ()) != errNoError)
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

		delete [] _pURL;
		_pURL			= (char *) NULL;

		return err;
	}

	if (_bRTSPBuffer. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bRedirectedURL. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if ((errGeneric = _csClientSocket. finish ()) != errNoError)
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

		delete [] _pURL;
		_pURL			= (char *) NULL;

		return err;
	}

	if (_bSDP. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bRTSPBuffer. finish () != errNoError)
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

		if ((errGeneric = _csClientSocket. finish ()) != errNoError)
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

		delete [] _pURL;
		_pURL			= (char *) NULL;

		return err;
	}

	if (_bSession. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bSDP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRTSPBuffer. finish () != errNoError)
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

		if ((errGeneric = _csClientSocket. finish ()) != errNoError)
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

		delete [] _pURL;
		_pURL			= (char *) NULL;

		return err;
	}

	if (_mtRTSPSession. init (PMutex:: MUTEX_RECURSIVE) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bSession. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
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

		if (_bRTSPBuffer. finish () != errNoError)
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

		if ((errGeneric = _csClientSocket. finish ()) != errNoError)
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

		delete [] _pURL;
		_pURL			= (char *) NULL;

		return err;
	}

	if (PosixThread:: init () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
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

		if (_bSDP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRTSPBuffer. finish () != errNoError)
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

		if ((errGeneric = _csClientSocket. finish ()) != errNoError)
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

		delete [] _pURL;
		_pURL			= (char *) NULL;

		return err;
	}


	return errNoError;
}


Error RTSPClientSession:: finish (Boolean_t bFinishRTSPHandshake)

{

	if (bFinishRTSPHandshake)
	{
		if (finalRTSPHandshake () != errNoError)
		{
			Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_FINALRTSPHANDSHAKE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}
	}

	if (_bIsThereVideo)
	{
		RTPPacketsMultiMap_t:: iterator		it;
		RTPPacketInfo_p						priRTPPacketInfo;


		for (it = _rmmVideoRTPPacketsSet. begin ();
			it != _rmmVideoRTPPacketsSet. end ();
			++it)
		{
			priRTPPacketInfo		= it -> second;

			delete [] (priRTPPacketInfo -> _pucRTPPacket);
			delete priRTPPacketInfo;
		}

		_rmmVideoRTPPacketsSet. clear ();
	}

	if (_bIsThereAudio)
	{
		RTPPacketsMultiMap_t:: iterator		it;
		RTPPacketInfo_p						priRTPPacketInfo;


		for (it = _rmmAudioRTPPacketsSet. begin ();
			it != _rmmAudioRTPPacketsSet. end ();
			++it)
		{
			priRTPPacketInfo		= it -> second;

			delete [] (priRTPPacketInfo -> _pucRTPPacket);
			delete priRTPPacketInfo;
		}

		_rmmAudioRTPPacketsSet. clear ();
	}

	if (PosixThread:: finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_mtRTSPSession. finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_FINISH_FAILED);
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

	if (_bSDP. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bRTSPBuffer. finish () != errNoError)
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

	if (_csClientSocket. finish () != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	delete [] _pURL;
	_pURL			= (char *) NULL;


	return errNoError;
}


Error RTSPClientSession:: getRTSPSessionIdentifier (
	unsigned long *pulRTSPSessionIdentifier)

{

	*pulRTSPSessionIdentifier			= _ulRTSPSessionIdentifier;


	return errNoError;
}


Error RTSPClientSession:: getStatisticsInformation (
	unsigned long *pulVideoPacketsNumberReceived,
	unsigned long *pulAudioPacketsNumberReceived,
	unsigned long *pulLostVideoPacketsNumber,
	unsigned long *pulLostAudioPacketsNumber)

{

	*pulVideoPacketsNumberReceived		=
		(_ulLastVideoSequenceNumber - _ulFirstVideoSequenceNumber + 1) -
		_ulCurrentLostVideoPacketsNumber;

	*pulAudioPacketsNumberReceived		=
		(_ulLastAudioSequenceNumber - _ulFirstAudioSequenceNumber + 1) -
		_ulCurrentLostAudioPacketsNumber;

	*pulLostVideoPacketsNumber			=
		_ulCurrentLostVideoPacketsNumber;
	*pulLostAudioPacketsNumber			=
		_ulCurrentLostAudioPacketsNumber;


	return errNoError;
}


Error RTSPClientSession:: initialRTSPHandshake (
	Boolean_p pbIsThereVideo,
	ServerSocket_p *psVideoRTPServerSocket,
	ServerSocket_p *psVideoRTCPServerSocket,
	Boolean_p pbIsThereAudio,
	ServerSocket_p *psAudioRTPServerSocket,
	ServerSocket_p *psAudioRTCPServerSocket)

{

	unsigned long				ulStatusCode;
	Error_t						errSocketInit;
	Error_t						errReadHttpResponce;
	Error_t						errWrite;
	WebUtility:: HttpMethod_t	hmHttpMethod;


	*pbIsThereVideo				= false;
	*pbIsThereAudio				= false;

	if (RTSPUtility:: getDESCRIBERequest (_pURL, ++_ulCurrentRTSPSequenceNumber,
		"CatraPlayer", &_bRTSPBuffer, _ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETDESCRIBEREQUEST_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errWrite = _psiClientRTSPSocketImpl -> writeString (
		(const char *) _bRTSPBuffer, true, 1, 0)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errWrite, __FILE__, __LINE__);

		{
			Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_SENDTOSERVERFAILED,
				2, _ulRTSPSessionIdentifier, (const char *) _bRTSPBuffer);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITESTRING_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = MP4PlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_SENTTOSERVER,
			2, _ulRTSPSessionIdentifier, (const char *) _bRTSPBuffer);
		_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

	if ((errReadHttpResponce = WebUtility:: readHttpHeaderAndBody (
		_psiClientRTSPSocketImpl,
		60, 0,	// the player wait 60 seconds to have the answer from the server
		&_bRTSPBuffer, &_bSDP, &hmHttpMethod, (Buffer_p) NULL,
		(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL)) != errNoError)
	{
		{
			Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_READHTTPRESPONSEFAILED,
				1, _ulRTSPSessionIdentifier);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		// if the answer of the DESCRIBE is a redirection the
		// streaming server answer does not contain the SDFP as body
		if ((long) errReadHttpResponce !=
			WEBTOOLS_HTTPGETTHREAD_BODYTIMEOUTEXPIRED)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errReadHttpResponce, __FILE__, __LINE__);

			Error err = WebToolsErrors (__FILE__, __LINE__,
				WEBTOOLS_WEBUTILITY_READHTTPHEADERANDBODY_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return errReadHttpResponce;
		}
	}

	{
		Message msg = MP4PlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
			3, _ulRTSPSessionIdentifier,
			(const char *) _bRTSPBuffer,
			(const char *) _bSDP);
		_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (RTSPUtility:: parseDESCRIBEResponse (
		(const char *) _bRTSPBuffer,
		&ulStatusCode, &_bRedirectedURL, &_ulCurrentRTSPSequenceNumber,
		&_bSession, (unsigned long *) NULL, (Buffer_p) NULL,
		_ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_PARSEDESCRIBERESPONSE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// Moved Permanently
	if (ulStatusCode == RTSP_STATUSCODE_MOVEDPERMANENTLY)
	{
		_bRedirectionPerformed			= true;

		if (WebUtility:: parseURL ((const char *) _bRedirectedURL,
			_pRTSPServerIPAddress, SCK_MAXIPADDRESSLENGTH, &_lRTSPServerPort,
			(Buffer_p) NULL, (Buffer_p) NULL) != errNoError)
		{
			Error err = WebToolsErrors (__FILE__, __LINE__,
				WEBTOOLS_WEBUTILITY_PARSEURL_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_csClientSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if ((errSocketInit = _csClientSocket. init (SocketImpl:: STREAM,
			30, 0, 30, 0, true, (const char *) NULL, _pRTSPServerIPAddress,
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
					errToTrace = MP4PlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_ATTEMPTTOCONNECTTIMEDOUT,
						1, _ulRTSPSessionIdentifier);
				}
				else if (ulUserDataBytes != 0 &&
					iErrno == WSABASEERR + 61)	// WSAECONNREFUSED
				{
					errToTrace = MP4PlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_CONNECTIONREJECTED,
						1, _ulRTSPSessionIdentifier);
				}
				else
					errToTrace			= errSocketInit;
			#else
					errToTrace			= errSocketInit;
			#endif

			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errToTrace, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_csClientSocket. getSocketImpl (
			&_psiClientRTSPSocketImpl) != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKET_GETSOCKETIMPL_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (RTSPUtility:: getDESCRIBERequest ((const char *) _bRedirectedURL,
			++_ulCurrentRTSPSequenceNumber, "CatraPlayer", &_bRTSPBuffer,
			_ptTracer) != errNoError)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_GETDESCRIBEREQUEST_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if ((errWrite = _psiClientRTSPSocketImpl -> writeString (
			(const char *) _bRTSPBuffer, true, 1, 0)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errWrite, __FILE__, __LINE__);

			{
				Error err = MP4PlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_SENDTOSERVERFAILED,
					2, _ulRTSPSessionIdentifier, (const char *) _bRTSPBuffer);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETIMPL_WRITESTRING_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		{
			Message msg = MP4PlayerMessages (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_SENTTOSERVER,
				2, _ulRTSPSessionIdentifier, (const char *) _bRTSPBuffer);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (WebUtility:: readHttpHeaderAndBody (
			_psiClientRTSPSocketImpl,
			60, 0, &_bRTSPBuffer, &_bSDP, &hmHttpMethod, (Buffer_p) NULL,
			(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL) != errNoError)
		{
			Error err = WebToolsErrors (__FILE__, __LINE__,
				WEBTOOLS_WEBUTILITY_READHTTPHEADERANDBODY_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		{
			Message msg = MP4PlayerMessages (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
				3, _ulRTSPSessionIdentifier,
				(const char *) _bRTSPBuffer,
				(const char *) _bSDP);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (RTSPUtility:: parseDESCRIBEResponse (
			(const char *) _bRTSPBuffer,
			&ulStatusCode, (Buffer_p) NULL, &_ulCurrentRTSPSequenceNumber,
			&_bSession, (unsigned long *) NULL, (Buffer_p) NULL,
			_ptTracer) != errNoError)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_PARSEDESCRIBERESPONSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		_bRedirectionPerformed			= false;
	}

	if (ulStatusCode != RTSP_STATUSCODE_OK)
	{
		Error err = MP4PlayerErrors (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_RECEIVEDERRORFROMSERVER,
			1, (const char *) _bRTSPBuffer);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		SDPFor3GPP_t			sSDPFor3GPP;
		SDPMedia_p				psmSDPMediaVideo;
		SDPMedia_p				psmSDPMediaAudio;
		Error_t					errGetSDPMedia;


		if (sSDPFor3GPP. init (&_bSDP, _ptTracer) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (sSDPFor3GPP. getAttributeValue ("control",
			_pControlValue) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_GETATTRIBUTEVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFOR3GPP_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if ((errGetSDPMedia = sSDPFor3GPP. getSDPMedia (
			SDPMedia:: SDPMEDIA_VIDEO,
			0, &psmSDPMediaVideo)) != errNoError)
		{
			if ((long) errGetSDPMedia != SDP_SDP_SDPMEDIANOTFOUND)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFOR3GPP_GETSDPMEDIA_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPFOR3GPP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			_bIsThereVideo			= false;
		}
		else
		{
			_bIsThereVideo			= true;

			if (((SDPMediaFor3GPP_p) psmSDPMediaVideo) -> getMediaInfo (
				(SDPMedia:: SDPMediaType_p) NULL,
				_pVideoControlValue,
				(unsigned long *) NULL,
				(unsigned long *) NULL,
				(char *) NULL,
				(char *) NULL,
				(char *) NULL,
				(unsigned long *) NULL) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_GETMEDIAINFO_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPFOR3GPP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if ((errGetSDPMedia = sSDPFor3GPP. getSDPMedia (
			SDPMedia:: SDPMEDIA_AUDIO,
			0, &psmSDPMediaAudio)) != errNoError)
		{
			if ((long) errGetSDPMedia != SDP_SDP_SDPMEDIANOTFOUND)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFOR3GPP_GETSDPMEDIA_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPFOR3GPP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			_bIsThereAudio			= false;
		}
		else
		{
			_bIsThereAudio			= true;

			if (((SDPMediaFor3GPP_p) psmSDPMediaAudio) -> getMediaInfo (
				(SDPMedia:: SDPMediaType_p) NULL,
				_pAudioControlValue,
				(unsigned long *) NULL,
				(unsigned long *) NULL,
				(char *) NULL,
				(char *) NULL,
				(char *) NULL,
				(unsigned long *) NULL) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_GETMEDIAINFO_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPFOR3GPP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (sSDPFor3GPP. finish () != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (!_bIsThereVideo && !_bIsThereAudio)
		{
			Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_NOMEDIAFOUND);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (_bIsThereVideo)
	{
		if (RTSPUtility:: getSETUPRequest (_pURL, _pVideoControlValue,
			++_ulCurrentRTSPSequenceNumber,
			!strcmp ((const char *) _bSession, "") ? (const char *) NULL :
			(const char *) _bSession,
			_ulVideoClientRTPPort, _ulVideoClientRTCPPort,
			"CatraPlayer", &_bRTSPBuffer,
			_ptTracer) != errNoError)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_GETSETUPREQUEST_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if ((errWrite = _psiClientRTSPSocketImpl -> writeString (
			(const char *) _bRTSPBuffer, true, 1, 0)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errWrite, __FILE__, __LINE__);

			{
				Error err = MP4PlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_SENDTOSERVERFAILED,
					2, _ulRTSPSessionIdentifier, (const char *) _bRTSPBuffer);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETIMPL_WRITESTRING_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		{
			Message msg = MP4PlayerMessages (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_SENTTOSERVER,
				2, _ulRTSPSessionIdentifier, (const char *) _bRTSPBuffer);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (WebUtility:: readHttpHeaderAndBody (
			_psiClientRTSPSocketImpl,
			60, 0, &_bRTSPBuffer, (Buffer_p) NULL, &hmHttpMethod,
			(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL,
			(Buffer_p) NULL) != errNoError)
		{
			Error err = WebToolsErrors (__FILE__, __LINE__,
				WEBTOOLS_WEBUTILITY_READHTTPHEADERANDBODY_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		{
			Message msg = MP4PlayerMessages (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
				3, _ulRTSPSessionIdentifier,
				(const char *) _bRTSPBuffer,
				"");
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (RTSPUtility:: parseSETUPResponse (
			(const char *) _bRTSPBuffer,
			&ulStatusCode, &_bSession,
			(unsigned long *) NULL, (unsigned long *) NULL,
			&_ulVideoServerRTPPort, &_ulVideoServerRTCPPort, _ptTracer) !=
			errNoError)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_PARSESETUPRESPONSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (ulStatusCode != RTSP_STATUSCODE_OK)
		{
			Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_RECEIVEDERRORFROMSERVER,
				1, (const char *) _bRTSPBuffer);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if ((errSocketInit = _sVideoRTPServerSocket. init (
			(const char *) NULL, _ulVideoClientRTPPort,
			true, SocketImpl:: DGRAM, 30, 0)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errSocketInit, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_sVideoRTPServerSocket. getSocketImpl (
			&_psiVideoRTPServerSocketImpl) != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKET_GETSOCKETIMPL_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if ((errSocketInit = _sVideoRTCPServerSocket. init (
			(const char *) NULL, _ulVideoClientRTCPPort,
			true, SocketImpl:: DGRAM, 30, 0)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errSocketInit, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_sVideoRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_sVideoRTCPServerSocket. getSocketImpl (
			&_psiVideoRTCPServerSocketImpl) != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKET_GETSOCKETIMPL_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_sVideoRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_sVideoRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		*pbIsThereVideo				= true;
		*psVideoRTPServerSocket		= &_sVideoRTPServerSocket;
		*psVideoRTCPServerSocket	= &_sVideoRTCPServerSocket;
	}

	if (_bIsThereAudio)
	{
		if (RTSPUtility:: getSETUPRequest (_pURL, _pAudioControlValue,
			++_ulCurrentRTSPSequenceNumber,
			!strcmp ((const char *) _bSession, "") ? (const char *) NULL :
			(const char *) _bSession,
			_ulAudioClientRTPPort, _ulAudioClientRTCPPort,
			"CatraPlayer", &_bRTSPBuffer,
			_ptTracer) != errNoError)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_GETSETUPREQUEST_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_bIsThereVideo)
			{
				if (_sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return err;
		}

		if ((errWrite = _psiClientRTSPSocketImpl -> writeString (
			(const char *) _bRTSPBuffer, true, 1, 0)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errWrite, __FILE__, __LINE__);

			{
				Error err = MP4PlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_SENDTOSERVERFAILED,
					2, _ulRTSPSessionIdentifier, (const char *) _bRTSPBuffer);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETIMPL_WRITESTRING_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_bIsThereVideo)
			{
				if (_sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return err;
		}

		{
			Message msg = MP4PlayerMessages (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_SENTTOSERVER,
				2, _ulRTSPSessionIdentifier, (const char *) _bRTSPBuffer);
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (WebUtility:: readHttpHeaderAndBody (
			_psiClientRTSPSocketImpl,
			60, 0, &_bRTSPBuffer, (Buffer_p) NULL, &hmHttpMethod,
			(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL,
			(Buffer_p) NULL) != errNoError)
		{
			Error err = WebToolsErrors (__FILE__, __LINE__,
				WEBTOOLS_WEBUTILITY_READHTTPHEADERANDBODY_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_bIsThereVideo)
			{
				if (_sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return err;
		}

		{
			Message msg = MP4PlayerMessages (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
				3, _ulRTSPSessionIdentifier,
				(const char *) _bRTSPBuffer,
				"");
			_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (RTSPUtility:: parseSETUPResponse (
			(const char *) _bRTSPBuffer,
			&ulStatusCode, &_bSession,
			(unsigned long *) NULL, (unsigned long *) NULL,
			&_ulAudioServerRTPPort, &_ulAudioServerRTCPPort, _ptTracer) !=
			errNoError)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_PARSESETUPRESPONSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_bIsThereVideo)
			{
				if (_sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}


			return err;
		}

		if (ulStatusCode != RTSP_STATUSCODE_OK)
		{
			Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_RECEIVEDERRORFROMSERVER,
				1, (const char *) _bRTSPBuffer);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_bIsThereVideo)
			{
				if (_sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return err;
		}

		if ((errSocketInit = _sAudioRTPServerSocket. init (
			(const char *) NULL, _ulAudioClientRTPPort,
			true, SocketImpl:: DGRAM, 30, 0)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errSocketInit, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_bIsThereVideo)
			{
				if (_sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return err;
		}

		if (_sAudioRTPServerSocket. getSocketImpl (
			&_psiAudioRTPServerSocketImpl) != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKET_GETSOCKETIMPL_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_bIsThereVideo)
			{
				if (_sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return err;
		}

		if ((errSocketInit = _sAudioRTCPServerSocket. init (
			(const char *) NULL, _ulAudioClientRTCPPort,
			true, SocketImpl:: DGRAM, 30, 0)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errSocketInit, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_sAudioRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bIsThereVideo)
			{
				if (_sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return err;
		}

		if (_sAudioRTCPServerSocket. getSocketImpl (
			&_psiAudioRTCPServerSocketImpl) != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKET_GETSOCKETIMPL_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_sAudioRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_sAudioRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bIsThereVideo)
			{
				if (_sVideoRTCPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_sVideoRTPServerSocket. finish () != errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_CLIENTSOCKET_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return err;
		}

		*pbIsThereAudio				= true;
		*psAudioRTPServerSocket		= &_sAudioRTPServerSocket;
		*psAudioRTCPServerSocket	= &_sAudioRTCPServerSocket;
	}

	if (RTSPUtility:: getPLAYRequest (
		_bRedirectionPerformed ? (const char *) _bRedirectedURL : _pURL,
		(const char *) NULL, // _pControlValue,
		++_ulCurrentRTSPSequenceNumber, (const char *) _bSession,
		"CatraPlayer", &_bRTSPBuffer,
		_ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETPLAYREQUEST_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bIsThereAudio)
		{
			if (_sAudioRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_sAudioRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (_bIsThereVideo)
		{
			if (_sVideoRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_sVideoRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}


		return err;
	}

	if ((errWrite = _psiClientRTSPSocketImpl -> writeString (
		(const char *) _bRTSPBuffer, true, 1, 0)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errWrite, __FILE__, __LINE__);

		{
			Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_SENDTOSERVERFAILED,
				2, _ulRTSPSessionIdentifier, (const char *) _bRTSPBuffer);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITESTRING_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bIsThereAudio)
		{
			if (_sAudioRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_sAudioRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (_bIsThereVideo)
		{
			if (_sVideoRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_sVideoRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}


		return err;
	}

	{
		Message msg = MP4PlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_SENTTOSERVER,
			2, _ulRTSPSessionIdentifier, (const char *) _bRTSPBuffer);
		_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (WebUtility:: readHttpHeaderAndBody (
		_psiClientRTSPSocketImpl,
		60, 0, &_bRTSPBuffer, (Buffer_p) NULL, &hmHttpMethod,
		(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL,
		(Buffer_p) NULL) != errNoError)
	{
		Error err = WebToolsErrors (__FILE__, __LINE__,
			WEBTOOLS_WEBUTILITY_READHTTPHEADERANDBODY_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bIsThereAudio)
		{
			if (_sAudioRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_sAudioRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (_bIsThereVideo)
		{
			if (_sVideoRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_sVideoRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		return err;
	}

	{
		Message msg = MP4PlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
			3, _ulRTSPSessionIdentifier,
			(const char *) _bRTSPBuffer,
			"");
		_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (RTSPUtility:: parsePLAYResponse (
		(const char *) _bRTSPBuffer,
		_pVideoControlValue, _pAudioControlValue,
		&ulStatusCode, (unsigned long *) NULL,
		&_ulFirstVideoSequenceNumber, &_ulFirstVideoTimeStamp,
		&_ulFirstAudioSequenceNumber, &_ulFirstAudioTimeStamp,
		_ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_PARSEPLAYRESPONSE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bIsThereAudio)
		{
			if (_sAudioRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_sAudioRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (_bIsThereVideo)
		{
			if (_sVideoRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_sVideoRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		return err;
	}

	if (ulStatusCode != RTSP_STATUSCODE_OK)
	{
		Error err = MP4PlayerErrors (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_RECEIVEDERRORFROMSERVER,
			1, (const char *) _bRTSPBuffer);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bIsThereAudio)
		{
			if (_sAudioRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_sAudioRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (_bIsThereVideo)
		{
			if (_sVideoRTCPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_sVideoRTPServerSocket. finish () != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_CLIENTSOCKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		return err;
	}


	return errNoError;
}


Error RTSPClientSession:: finalRTSPHandshake (void)

{

	unsigned long				ulStatusCode;
	Error_t						errWrite;
	WebUtility:: HttpMethod_t	hmHttpMethod;


	if (RTSPUtility:: getTEARDOWNRequest (
		_pURL, ++_ulCurrentRTSPSequenceNumber, (const char *) _bSession,
		"CatraPlayer", &_bRTSPBuffer,
		_ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETPLAYREQUEST_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errWrite = _psiClientRTSPSocketImpl -> writeString (
		(const char *) _bRTSPBuffer, true, 1, 0)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errWrite, __FILE__, __LINE__);

		{
			Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_SENDTOSERVERFAILED,
				2, _ulRTSPSessionIdentifier, (const char *) _bRTSPBuffer);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITESTRING_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = MP4PlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_SENTTOSERVER,
			2, _ulRTSPSessionIdentifier, (const char *) _bRTSPBuffer);
		_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (WebUtility:: readHttpHeaderAndBody (
		_psiClientRTSPSocketImpl,
		60, 0, &_bRTSPBuffer, (Buffer_p) NULL, &hmHttpMethod,
		(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL,
		(Buffer_p) NULL) != errNoError)
	{
		Error err = WebToolsErrors (__FILE__, __LINE__,
			WEBTOOLS_WEBUTILITY_READHTTPHEADERANDBODY_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = MP4PlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
			3, _ulRTSPSessionIdentifier,
			(const char *) _bRTSPBuffer,
			"");
		_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

	/*
	if ((_erThreadReturn = RTSPUtility:: parseTEARDOWNResponse (
		(const char *) _bRTSPBuffer,
		&ulStatusCode, (unsigned long *) NULL,
		_ptTracer)) != errNoError)
	{
		_erThreadReturn = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_PARSETEARDOWNRESPONSE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		return _erThreadReturn;
	}
	*/


	return errNoError;
}


Error RTSPClientSession:: processReceivingVideoRTPPacket (void)

{

	unsigned long				ulPacketLength;
	unsigned char				pucRTPPacket [
		MP4PL_MAXRTPPACKETLENGTH];
	Error_t						errRead;
	unsigned long				ulMicroSecondsToWaitForRTPackets;
	Boolean_t					bPacketsToRead;
	unsigned long				ulLocalSequenceNumber;


	ulPacketLength						= MP4PL_MAXRTPPACKETLENGTH;
	ulMicroSecondsToWaitForRTPackets	= 100;

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	bPacketsToRead			= true;

	while (bPacketsToRead)
	{
		if ((errRead = _psiVideoRTPServerSocketImpl -> read (pucRTPPacket,
			&ulPacketLength, true, 0, ulMicroSecondsToWaitForRTPackets,
			true)) != errNoError)
		{
			if ((long) errRead != SCK_NOTHINGTOREAD)
			{
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errRead, __FILE__, __LINE__);

				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETIMPL_READ_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			bPacketsToRead			= false;

			continue;
		}

		_ulTotalVideoBitstreamSize			+= (ulPacketLength - 12);

		ulLocalSequenceNumber				=
			ntohs (*((unsigned short *) (pucRTPPacket + 2)));

		if (_ulLastVideoSequenceNumber == (unsigned long) -1)
		{
			// first packet received
			if (_ulFirstVideoSequenceNumber == ulLocalSequenceNumber)
				;	// no packet lost
			else if (_ulFirstVideoSequenceNumber < ulLocalSequenceNumber)
				_ulCurrentLostVideoPacketsNumber	+=
					(ulLocalSequenceNumber - _ulFirstVideoSequenceNumber);
			else if (_ulFirstVideoSequenceNumber > ulLocalSequenceNumber)
				;	// case not possible
		}
		else
		{
			if (_ulLastVideoSequenceNumber + 1 == ulLocalSequenceNumber)
				;	// no packet lost
			else if (_ulLastVideoSequenceNumber + 1 < ulLocalSequenceNumber)
				_ulCurrentLostVideoPacketsNumber	+=
					(ulLocalSequenceNumber - (_ulLastVideoSequenceNumber + 1));
			else if (_ulLastVideoSequenceNumber + 1 > ulLocalSequenceNumber)
				_ulCurrentLostVideoPacketsNumber	-= 1;
		}

		_ulLastVideoSequenceNumber			= ulLocalSequenceNumber;
		_ulLastVideoTimeStamp				=
			ntohl (*((unsigned long *) (pucRTPPacket + 4)));

		{
			Message msg = MP4PlayerMessages (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_RECEIVEDRTPMESSAGE,
				4, _ulRTSPSessionIdentifier, "video", ulPacketLength,
				(long) _ulLastVideoSequenceNumber);
			_ptTracer -> trace (Tracer:: TRACER_LDBG5,
				(const char *) msg, __FILE__, __LINE__);
		}

		{
			RTPPacketInfo_p				priRTPPacketInfo;

			if ((priRTPPacketInfo = new RTPPacketInfo_t) ==
				(RTPPacketInfo_p) NULL)
			{
				Error err = MP4PlayerErrors (__FILE__, __LINE__,
					MP4PL_NEW_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((priRTPPacketInfo -> _pucRTPPacket =
				new unsigned char [ulPacketLength - 12]) ==
				(unsigned char *) NULL)
			{
				Error err = MP4PlayerErrors (__FILE__, __LINE__,
					MP4PL_NEW_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete priRTPPacketInfo;

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			memcpy (priRTPPacketInfo -> _pucRTPPacket,
				pucRTPPacket + 12, ulPacketLength - 12);

			priRTPPacketInfo -> _ulLength		= ulPacketLength - 12;
			priRTPPacketInfo -> _ulTimeStamp	= _ulLastVideoTimeStamp;

			_rmmVideoRTPPacketsSet. insert (
				std:: pair<unsigned long, RTPPacketInfo_p>(
				_ulLastVideoSequenceNumber, priRTPPacketInfo));
		}

		if (_bTraceOnTTY)
		{
			std:: cout << "RV ";
			std:: cout. flush ();
		}
	}

	if (DateTime:: nowUTCInMilliSecs (
		&_ullUTCInMilliSecsOfTheLastRTPPacketReceived, (long *) NULL) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_DATETIME_NOWUTCINMILLISECS_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPClientSession:: processReceivingVideoRTCPPacket (void)

{

	unsigned long				ulPacketLength;
	unsigned char				pucRTCPPacket [
		MP4PL_MAXRTCPPACKETLENGTH];
	Error_t						errRead;
	unsigned long				ulMicroSecondsToWaitForRTPackets;


	ulPacketLength						= MP4PL_MAXRTCPPACKETLENGTH;
	ulMicroSecondsToWaitForRTPackets	= 100;

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errRead = _psiVideoRTCPServerSocketImpl -> read (pucRTCPPacket,
		&ulPacketLength, true, 0, ulMicroSecondsToWaitForRTPackets,
		true)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errRead, __FILE__, __LINE__);

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_READ_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		Message msg = MP4PlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_RECEIVEDRTCPMESSAGE,
			3, _ulRTSPSessionIdentifier, "video", ulPacketLength);
		_ptTracer -> trace (Tracer:: TRACER_LDBG5,
			(const char *) msg, __FILE__, __LINE__);
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_bTraceOnTTY)
	{
		std:: cout << "CV ";
		std:: cout. flush ();
	}


	return errNoError;
}


Error RTSPClientSession:: processReceivingAudioRTPPacket (void)

{

	unsigned long				ulPacketLength;
	unsigned char				pucRTPPacket [
		MP4PL_MAXRTPPACKETLENGTH];
	Error_t						errRead;
	unsigned long				ulMicroSecondsToWaitForRTPackets;
	Boolean_t					bPacketsToRead;
	unsigned long				ulLocalSequenceNumber;


	ulPacketLength						= MP4PL_MAXRTPPACKETLENGTH;
	ulMicroSecondsToWaitForRTPackets	= 100;

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	bPacketsToRead			= true;

	while (bPacketsToRead)
	{
		if ((errRead = _psiAudioRTPServerSocketImpl -> read (pucRTPPacket,
			&ulPacketLength, true, 0, ulMicroSecondsToWaitForRTPackets,
			true)) != errNoError)
		{
			if ((long) errRead != SCK_NOTHINGTOREAD)
			{
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errRead, __FILE__, __LINE__);

				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETIMPL_READ_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			bPacketsToRead			= false;

			continue;
		}

		_ulTotalAudioBitstreamSize			+= (ulPacketLength - 12);

		ulLocalSequenceNumber				=
			ntohs (*((unsigned short *) (pucRTPPacket + 2)));

		if (_ulLastAudioSequenceNumber == (unsigned long) -1)
		{
			// first packet received
			if (_ulFirstAudioSequenceNumber == ulLocalSequenceNumber)
				;	// no packet lost
			else if (_ulFirstAudioSequenceNumber < ulLocalSequenceNumber)
				_ulCurrentLostAudioPacketsNumber	+=
					(ulLocalSequenceNumber - _ulFirstAudioSequenceNumber);
			else if (_ulFirstAudioSequenceNumber > ulLocalSequenceNumber)
				;	// case not possible
		}
		else
		{
			if (_ulLastAudioSequenceNumber + 1 == ulLocalSequenceNumber)
				;	// no packet lost
			else if (_ulLastAudioSequenceNumber + 1 < ulLocalSequenceNumber)
				_ulCurrentLostAudioPacketsNumber	+=
					(ulLocalSequenceNumber - (_ulLastAudioSequenceNumber + 1));
			else if (_ulLastAudioSequenceNumber + 1 > ulLocalSequenceNumber)
				_ulCurrentLostAudioPacketsNumber	-= 1;
		}

		_ulLastAudioSequenceNumber			= ulLocalSequenceNumber;
		_ulLastAudioTimeStamp				=
			ntohl (*((unsigned long *) (pucRTPPacket + 4)));

		{
			Message msg = MP4PlayerMessages (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_RECEIVEDRTPMESSAGE,
				4, _ulRTSPSessionIdentifier, "Audio", ulPacketLength,
				(long) _ulLastAudioSequenceNumber);
			_ptTracer -> trace (Tracer:: TRACER_LDBG5,
				(const char *) msg, __FILE__, __LINE__);
		}

		{
			RTPPacketInfo_p				priRTPPacketInfo;

			if ((priRTPPacketInfo = new RTPPacketInfo_t) ==
				(RTPPacketInfo_p) NULL)
			{
				Error err = MP4PlayerErrors (__FILE__, __LINE__,
					MP4PL_NEW_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((priRTPPacketInfo -> _pucRTPPacket =
				new unsigned char [ulPacketLength - 12]) ==
				(unsigned char *) NULL)
			{
				Error err = MP4PlayerErrors (__FILE__, __LINE__,
					MP4PL_NEW_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete priRTPPacketInfo;

				if (_mtRTSPSession. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			memcpy (priRTPPacketInfo -> _pucRTPPacket,
				pucRTPPacket + 12, ulPacketLength - 12);

			priRTPPacketInfo -> _ulLength		= ulPacketLength - 12;
			priRTPPacketInfo -> _ulTimeStamp	= _ulLastAudioTimeStamp;

			_rmmAudioRTPPacketsSet. insert (
				std:: pair<unsigned long, RTPPacketInfo_p>(
				_ulLastAudioSequenceNumber, priRTPPacketInfo));
		}

		if (_bTraceOnTTY)
		{
			std:: cout << "RA ";
			std:: cout. flush ();
		}
	}

	if (DateTime:: nowUTCInMilliSecs (
		&_ullUTCInMilliSecsOfTheLastRTPPacketReceived, (long *) NULL) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_DATETIME_NOWUTCINMILLISECS_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPClientSession:: processReceivingAudioRTCPPacket (void)

{

	unsigned long				ulPacketLength;
	unsigned char				pucRTCPPacket [
		MP4PL_MAXRTCPPACKETLENGTH];
	Error_t						errRead;
	unsigned long				ulMicroSecondsToWaitForRTPackets;


	ulPacketLength						= MP4PL_MAXRTCPPACKETLENGTH;
	ulMicroSecondsToWaitForRTPackets	= 100;

	if (_mtRTSPSession. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errRead = _psiAudioRTCPServerSocketImpl -> read (pucRTCPPacket,
		&ulPacketLength, false, 0, ulMicroSecondsToWaitForRTPackets,
		true)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errRead, __FILE__, __LINE__);

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_READ_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	{
		Message msg = MP4PlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_RECEIVEDRTCPMESSAGE,
			3, _ulRTSPSessionIdentifier, "video", ulPacketLength);
		_ptTracer -> trace (Tracer:: TRACER_LDBG5,
			(const char *) msg, __FILE__, __LINE__);
	}

	if (_mtRTSPSession. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_bTraceOnTTY)
	{
		std:: cout << "CA ";
		std:: cout. flush ();
	}


	return errNoError;
}


Error RTSPClientSession:: run (void)

{

	Boolean_t					bRTSPClientSessionFinished;
	#ifdef WIN32
		__int64					ullNowUTCInMilliSecs;
		__int64					ullLastRTSPOptionSent;
		__int64					ullStartRTSPClientSessionInMilliSeconds;
	#else
		unsigned long long		ullNowUTCInMilliSecs;
		unsigned long long		ullLastRTSPOptionSent;
		unsigned long long		ullStartRTSPClientSessionInMilliSeconds;
	#endif
	unsigned long				ulRTSPOptionPeriodInSeconds;
	unsigned long				ulStatusCode;
	Error_t						errWrite;
	Buffer_t					bRTSPOptionBuffer;
	WebUtility:: HttpMethod_t	hmHttpMethod;


	ulRTSPOptionPeriodInSeconds			= 60;

	if (bRTSPOptionBuffer. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (DateTime:: nowUTCInMilliSecs (
		&ullStartRTSPClientSessionInMilliSeconds, (long *) NULL) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_DATETIME_NOWUTCINMILLISECS_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bRTSPOptionBuffer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_ullUTCInMilliSecsOfTheLastRTPPacketReceived == 0)
	{
		if (DateTime:: nowUTCInMilliSecs (
			&_ullUTCInMilliSecsOfTheLastRTPPacketReceived, (long *) NULL) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_DATETIME_NOWUTCINMILLISECS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRTSPOptionBuffer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	ullLastRTSPOptionSent				=
		_ullUTCInMilliSecsOfTheLastRTPPacketReceived;

	bRTSPClientSessionFinished			= false;

	while (!bRTSPClientSessionFinished)
	{
		if (DateTime:: nowUTCInMilliSecs (
			&ullNowUTCInMilliSecs, (long *) NULL) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_DATETIME_NOWUTCINMILLISECS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRTSPOptionBuffer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_mtRTSPSession. lock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_LOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (ullNowUTCInMilliSecs -
			_ullUTCInMilliSecsOfTheLastRTPPacketReceived > 30000 ||
			(_lEndOfTheStreamingInSeconds != -1 &&
			_lEndOfTheStreamingInSeconds * 1000 <=
			(ullNowUTCInMilliSecs - ullStartRTSPClientSessionInMilliSeconds))
			)
		{
			bRTSPClientSessionFinished			= true;

			if (_mtRTSPSession. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			continue;
		}

		if (_mtRTSPSession. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (ullNowUTCInMilliSecs - ullLastRTSPOptionSent >
			ulRTSPOptionPeriodInSeconds * 1000)
		{
			ullLastRTSPOptionSent		= ullNowUTCInMilliSecs;

			if (RTSPUtility:: getOPTIONSRequest (
				_pURL, ++_ulCurrentRTSPSequenceNumber,
				"CatraPlayer", &bRTSPOptionBuffer,
				_ptTracer) != errNoError)
			{
				Error err = RTSPErrors (__FILE__, __LINE__,
					RTSP_RTSPUTILITY_GETOPTIONSREQUEST_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRTSPOptionBuffer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errWrite = _psiClientRTSPSocketImpl -> writeString (
				(const char *) bRTSPOptionBuffer, true, 1, 0)) != errNoError)
			{
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errWrite, __FILE__, __LINE__);

				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_SENDTOSERVERFAILED,
						2, _ulRTSPSessionIdentifier,
						(const char *) bRTSPOptionBuffer);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETIMPL_WRITESTRING_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRTSPOptionBuffer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			{
				Message msg = MP4PlayerMessages (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_SENTTOSERVER,
					2, _ulRTSPSessionIdentifier,
					(const char *) bRTSPOptionBuffer);
				_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
					__FILE__, __LINE__);
			}

			if (WebUtility:: readHttpHeaderAndBody (
				_psiClientRTSPSocketImpl,
				60, 0, &bRTSPOptionBuffer, (Buffer_p) NULL, &hmHttpMethod,
				(Buffer_p) NULL, (Buffer_p) NULL, (Buffer_p) NULL,
				(Buffer_p) NULL) != errNoError)
			{
				Error err = WebToolsErrors (__FILE__, __LINE__,
					WEBTOOLS_WEBUTILITY_READHTTPHEADERANDBODY_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRTSPOptionBuffer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			{
				Message msg = MP4PlayerMessages (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
					3, _ulRTSPSessionIdentifier,
					(const char *) bRTSPOptionBuffer,
					"");
				_ptTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
					__FILE__, __LINE__);
			}
		}

		if (PosixThread:: getSleep (1, 0) != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_GETSLEEP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRTSPOptionBuffer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (bRTSPOptionBuffer. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return _erThreadReturn;
}


Error RTSPClientSession:: saveBitStreams (void)

{

	if (_bIsThereVideo)
	{
		int									iFdBitstream;
		RTPPacketsMultiMap_t:: iterator		it;
		RTPPacketInfo_p						priRTPPacketInfo;
		char								pTimeStamp [MP4PL_MAXLONGLENGTH];
		#ifdef WIN32
			__int64							llBytesWritten;
		#else
			long long						llBytesWritten;
		#endif
		int									iFdTimeStamp;


		#ifdef WIN32
			if (FileIO:: open ("VideoBitstream.m4v",
				O_WRONLY | O_TRUNC | O_CREAT | O_BINARY,
				_S_IREAD | _S_IWRITE, &iFdBitstream) !=
				errNoError)
		#else
			if (FileIO:: open ("VideoBitstream.m4v",
				O_WRONLY | O_TRUNC | O_CREAT,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
				&iFdBitstream) != errNoError)
		#endif
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_OPEN_FAILED, 1, "VideoBitstream.m4v");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		#ifdef WIN32
			if (FileIO:: open ("VideoTimeStamp.txt",
				O_WRONLY | O_TRUNC | O_CREAT | O_TEXT,
				_S_IREAD | _S_IWRITE, &iFdTimeStamp) !=
				errNoError)
		#else
			if (FileIO:: open ("VideoTimeStamp.txt",
				O_WRONLY | O_TRUNC | O_CREAT,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
				&iFdTimeStamp) != errNoError)
		#endif
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_OPEN_FAILED, 1, "VideoBitstream.m4v");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (FileIO:: close (iFdBitstream) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_CLOSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		for (it = _rmmVideoRTPPacketsSet. begin ();
			it != _rmmVideoRTPPacketsSet. end ();
			++it)
		{
			priRTPPacketInfo		= it -> second;

			if (FileIO:: writeBytes (iFdBitstream,
				priRTPPacketInfo -> _pucRTPPacket,
				priRTPPacketInfo -> _ulLength, &llBytesWritten) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_WRITEBYTES_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (FileIO:: close (iFdTimeStamp) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_CLOSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (FileIO:: close (iFdBitstream) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_CLOSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			#ifdef WIN32
				sprintf (pTimeStamp, "%lu\r\n",
					priRTPPacketInfo -> _ulTimeStamp -
					_ulFirstVideoTimeStamp);
			#else
				sprintf (pTimeStamp, "%lu\n",
					priRTPPacketInfo -> _ulTimeStamp -
					_ulFirstVideoTimeStamp);
			#endif

			if (FileIO:: writeChars (iFdTimeStamp,
				pTimeStamp, strlen (pTimeStamp), &llBytesWritten) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_WRITEBYTES_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (FileIO:: close (iFdTimeStamp) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_CLOSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (FileIO:: close (iFdBitstream) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_CLOSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (FileIO:: close (iFdTimeStamp) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_CLOSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (FileIO:: close (iFdBitstream) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_CLOSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (FileIO:: close (iFdBitstream) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_CLOSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (_bIsThereAudio)
	{
		int									iFdBitstream;
		RTPPacketsMultiMap_t:: iterator		it;
		RTPPacketInfo_p						priRTPPacketInfo;
		char								pTimeStamp [MP4PL_MAXLONGLENGTH];
		#ifdef WIN32
			__int64							llBytesWritten;
		#else
			long long						llBytesWritten;
		#endif
		int									iFdTimeStamp;


		#ifdef WIN32
			if (FileIO:: open ("AudioBitstream.m4v",
				O_WRONLY | O_TRUNC | O_CREAT | O_BINARY,
				_S_IREAD | _S_IWRITE, &iFdBitstream) !=
				errNoError)
		#else
			if (FileIO:: open ("AudioBitstream.m4v",
				O_WRONLY | O_TRUNC | O_CREAT,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
				&iFdBitstream) != errNoError)
		#endif
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_OPEN_FAILED, 1, "AudioBitstream.m4v");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		#ifdef WIN32
			if (FileIO:: open ("AudioTimeStamp.txt",
				O_WRONLY | O_TRUNC | O_CREAT | O_TEXT,
				_S_IREAD | _S_IWRITE, &iFdTimeStamp) !=
				errNoError)
		#else
			if (FileIO:: open ("AudioTimeStamp.txt",
				O_WRONLY | O_TRUNC | O_CREAT,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
				&iFdTimeStamp) != errNoError)
		#endif
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_OPEN_FAILED, 1, "AudioBitstream.m4v");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (FileIO:: close (iFdBitstream) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_CLOSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		for (it = _rmmAudioRTPPacketsSet. begin ();
			it != _rmmAudioRTPPacketsSet. end ();
			++it)
		{
			priRTPPacketInfo		= it -> second;

			if (FileIO:: writeBytes (iFdBitstream,
				priRTPPacketInfo -> _pucRTPPacket,
				priRTPPacketInfo -> _ulLength, &llBytesWritten) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_WRITEBYTES_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (FileIO:: close (iFdTimeStamp) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_CLOSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (FileIO:: close (iFdBitstream) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_CLOSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			#ifdef WIN32
				sprintf (pTimeStamp, "%lu\r\n",
					priRTPPacketInfo -> _ulTimeStamp -
					_ulFirstAudioTimeStamp);
			#else
				sprintf (pTimeStamp, "%lu\n",
					priRTPPacketInfo -> _ulTimeStamp -
					_ulFirstAudioTimeStamp);
			#endif

			if (FileIO:: writeChars (iFdTimeStamp,
				pTimeStamp, strlen (pTimeStamp), &llBytesWritten) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_WRITEBYTES_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (FileIO:: close (iFdTimeStamp) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_CLOSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (FileIO:: close (iFdBitstream) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_CLOSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (FileIO:: close (iFdTimeStamp) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_CLOSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (FileIO:: close (iFdBitstream) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_CLOSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (FileIO:: close (iFdBitstream) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_CLOSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}



	return errNoError;
}


/*
#ifdef WIN32
	Error RTSPClientSession:: getRTPackets (unsigned long ulRTSPSessionIdentifier,
		DWORD *ptvTimeOfFirstRTPPacket,
		unsigned long ulStartVideoSequenceNumber,
		unsigned long ulStartVideoTimeStamp,
		unsigned long ulStartAudioSequenceNumber,
		unsigned long ulStartAudioTimeStamp,
		Boolean_t bIsThereVideo, ServerSocket_p psVideoRTPServerSocket,
		ServerSocket_p psVideoRTCPServerSocket,
		Boolean_t bIsThereAudio, ServerSocket_p psAudioRTPServerSocket,
		ServerSocket_p psAudioRTCPServerSocket,
		Boolean_t bTraceOnTTY, Boolean_t bBitstreamFile)
#else
	Error RTSPClientSession:: getRTPackets (unsigned long ulRTSPSessionIdentifier,
		timeval *ptvTimeOfFirstRTPPacket,
		unsigned long ulStartVideoSequenceNumber,
		unsigned long ulStartVideoTimeStamp,
		unsigned long ulStartAudioSequenceNumber,
		unsigned long ulStartAudioTimeStamp,
		Boolean_t bIsThereVideo, ServerSocket_p psVideoRTPServerSocket,
		ServerSocket_p psVideoRTCPServerSocket,
		Boolean_t bIsThereAudio, ServerSocket_p psAudioRTPServerSocket,
		ServerSocket_p psAudioRTCPServerSocket,
		Boolean_t bTraceOnTTY, Boolean_t bBitstreamFile)
#endif

{

	SocketImpl_p				psiServerSocketImpl;
	Error_t						errRead;
	Boolean_t					bPacketsFinished;
	unsigned long				ulPacketLength;
	unsigned char				pucRTCPPacket [
		MP4PL_MAXRTCPPACKETLENGTH];
	unsigned char				pucRTPPacket [
		MP4PL_MAXRTPPACKETLENGTH];
	time_t						tLastPacketTime;
	Boolean_t					bReceivedFirstRTPPacket;
	Boolean_t					bTimeOfFirstRTPPacketAlreadyTook;
	unsigned long				ulMilliSecondsToWaitForRTPackets;
	#ifdef WIN32
	#else
		struct timezone			tzTimeZone;
	#endif
	IDHasher				hHasher;
	IDEqualCmp				cComparer;
	RTPPacketsHashMap_t		rhmVideoRTPPacketsSet (100, hHasher, cComparer);
	RTPPacketsHashMap_t		rhmAudioRTPPacketsSet (100, hHasher, cComparer);

	unsigned long			ulTotalVideoBitstreamSize;
	unsigned long			ulTotalAudioBitstreamSize;
	unsigned long			ulLastVideoSequenceNumber;
	unsigned long			ulLastAudioSequenceNumber;


	{
		Message msg = MP4PlayerMessages (__FILE__, __LINE__,
			MP4PL_RTSPCLIENTSESSION_STARTOTREADINGPACKETSFROMSERVER,
			1, ulRTSPSessionIdentifier);
		_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	_ptTracer -> flushOfTraces ();

	// It is important to avoid to wait for a lot because
	// otherwise we lose packets
	// used in isReadyToRead
	ulMilliSecondsToWaitForRTPackets			= 300;

	ulTotalVideoBitstreamSize			= 0;
	ulTotalAudioBitstreamSize			= 0;
	_ulLastVideoSequenceNumber			= 0;
	ulLastAudioSequenceNumber			= 0;
	bPacketsFinished					= false;
	tLastPacketTime						= time (NULL);
	bReceivedFirstRTPPacket				= false;
	bTimeOfFirstRTPPacketAlreadyTook	= false;

	while (!bPacketsFinished)
	{
		if (bReceivedFirstRTPPacket && !bTimeOfFirstRTPPacketAlreadyTook)
		{
			#ifdef WIN32
				*ptvTimeOfFirstRTPPacket		= GetTickCount ();
			#else
				if (gettimeofday (ptvTimeOfFirstRTPPacket, &tzTimeZone) == -1)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_GETTIMEOFDAY_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			#endif

			bTimeOfFirstRTPPacketAlreadyTook		= true;
		}

		if (time (NULL) - tLastPacketTime > MP4PL_MAXSECONDSTOWAITRTPPACKETS)
		{
			bPacketsFinished			= true;

			continue;
		}

		if (bIsThereVideo)
		{
			// RTCP
			if (psVideoRTCPServerSocket -> getSocketImpl (
				&psiServerSocketImpl) != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKET_GETSOCKETIMPL_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			ulPacketLength				= MP4PL_MAXRTCPPACKETLENGTH;

			if ((errRead = psiServerSocketImpl -> read (pucRTCPPacket,
				&ulPacketLength, true, 0, ulMilliSecondsToWaitForRTPackets,
				true)) != errNoError)
			{
				if ((long) errRead != SCK_NOTHINGTOREAD)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SOCKETIMPL_READ_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (errRead == errNoError) // not SCK_NOTHINGTOREAD
			{
				tLastPacketTime				= time (NULL);

				{
					Message msg = MP4PlayerMessages (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_RECEIVEDRTCPMESSAGE,
						3, _ulRTSPSessionIdentifier, "video", ulPacketLength);
					_ptTracer -> trace (Tracer:: TRACER_LDBG5,
						(const char *) msg, __FILE__, __LINE__);
				}

				if (bTraceOnTTY)
				{
					std:: cout << "CV ";
					std:: cout. flush ();
				}
			}

			// RTP
			if (psVideoRTPServerSocket -> getSocketImpl (
				&psiServerSocketImpl) != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKET_GETSOCKETIMPL_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			ulPacketLength				= MP4PL_MAXRTPPACKETLENGTH;

			if ((errRead = psiServerSocketImpl -> read (pucRTPPacket,
				&ulPacketLength, true, 0, ulMilliSecondsToWaitForRTPackets,
				true)) != errNoError)
			{
				if ((long) errRead != SCK_NOTHINGTOREAD)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SOCKETIMPL_READ_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (errRead == errNoError) // not SCK_NOTHINGTOREAD
			{
				tLastPacketTime				= time (NULL);
				bReceivedFirstRTPPacket		= true;

				{
					Message msg = MP4PlayerMessages (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_RECEIVEDRTPMESSAGE,
						4, ulRTSPSessionIdentifier, "video", ulPacketLength,
						(long)
						ntohs (*((unsigned short *) (pucRTPPacket + 2))));
					_ptTracer -> trace (Tracer:: TRACER_LDBG5,
						(const char *) msg, __FILE__, __LINE__);
				}

				ulTotalVideoBitstreamSize			+= (ulPacketLength - 12);

				_ulLastVideoSequenceNumber			=
					ntohs (*((unsigned short *) (pucRTPPacket + 2)));

				{
					RTPPacketInfo_p				priRTPPacketInfo;

					if ((priRTPPacketInfo = new RTPPacketInfo_t) ==
						(RTPPacketInfo_p) NULL)
					{
						Error err = MP4PlayerErrors (__FILE__, __LINE__,
							MP4PL_NEW_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return err;
					}

					if ((priRTPPacketInfo -> _pucRTPPacket =
						new unsigned char [ulPacketLength - 12]) ==
						(unsigned char *) NULL)
					{
						Error err = MP4PlayerErrors (__FILE__, __LINE__,
							MP4PL_NEW_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete priRTPPacketInfo;

						return err;
					}
					memcpy (priRTPPacketInfo -> _pucRTPPacket,
						pucRTPPacket + 12, ulPacketLength - 12);

					priRTPPacketInfo -> _ulLength		= ulPacketLength - 12;
					priRTPPacketInfo -> _ulTimeStamp	=
						ntohl (*((unsigned long *) (pucRTPPacket + 4)));

					rhmVideoRTPPacketsSet. insert (
						ntohs (*((unsigned short *) (pucRTPPacket + 2))),
						priRTPPacketInfo);
				}

				if (bTraceOnTTY)
				{
					std:: cout << "RV ";
					std:: cout. flush ();
				}
			}
		}

		if (bIsThereAudio)
		{
			// RTCP
			if (psAudioRTCPServerSocket -> getSocketImpl (
				&psiServerSocketImpl) != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKET_GETSOCKETIMPL_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			ulPacketLength				= MP4PL_MAXRTCPPACKETLENGTH;

			if ((errRead = psiServerSocketImpl -> read (pucRTCPPacket,
				&ulPacketLength, true, 0, ulMilliSecondsToWaitForRTPackets,
				true)) != errNoError)
			{
				if ((long) errRead != SCK_NOTHINGTOREAD)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SOCKETIMPL_READ_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (errRead == errNoError) // not SCK_NOTHINGTOREAD
			{
				tLastPacketTime				= time (NULL);

				{
					Message msg = MP4PlayerMessages (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_RECEIVEDRTCPMESSAGE,
						3, _ulRTSPSessionIdentifier, "audio", ulPacketLength);
					_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
						__FILE__, __LINE__);
				}

				if (bTraceOnTTY)
				{
					std:: cout << "CA ";
					std:: cout. flush ();
				}
			}

			// RTP
			if (psAudioRTPServerSocket -> getSocketImpl (
				&psiServerSocketImpl) != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKET_GETSOCKETIMPL_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (psVideoRTPServerSocket -> getSocketImpl (
				&psiServerSocketImpl) != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKET_GETSOCKETIMPL_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			ulPacketLength				= MP4PL_MAXRTPPACKETLENGTH;

			if ((errRead = psiServerSocketImpl -> read (pucRTPPacket,
				&ulPacketLength, true, 0, ulMilliSecondsToWaitForRTPackets,
				true)) != errNoError)
			{
				if ((long) errRead != SCK_NOTHINGTOREAD)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SOCKETIMPL_READ_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (errRead == errNoError) // not SCK_NOTHINGTOREAD
			{
				tLastPacketTime				= time (NULL);
				bReceivedFirstRTPPacket		= true;

				{
					Message msg = MP4PlayerMessages (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_RECEIVEDRTPMESSAGE,
						4, ulRTSPSessionIdentifier, "audio", ulPacketLength,
						(long) ntohs (*((unsigned short *) (pucRTPPacket + 2))));
					_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
						__FILE__, __LINE__);
				}

				ulTotalAudioBitstreamSize			+= (ulPacketLength - 12);

				ulLastAudioSequenceNumber			=
					ntohs (*((unsigned short *) (pucRTPPacket + 2)));

				{
					RTPPacketInfo_p				priRTPPacketInfo;

					if ((priRTPPacketInfo = new RTPPacketInfo_t) ==
						(RTPPacketInfo_p) NULL)
					{
						Error err = MP4PlayerErrors (__FILE__, __LINE__,
							MP4PL_NEW_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return err;
					}

					if ((priRTPPacketInfo -> _pucRTPPacket =
						new unsigned char [ulPacketLength - 12]) ==
						(unsigned char *) NULL)
					{
						Error err = MP4PlayerErrors (__FILE__, __LINE__,
							MP4PL_NEW_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete priRTPPacketInfo;

						return err;
					}
					memcpy (priRTPPacketInfo -> _pucRTPPacket,
						pucRTPPacket + 12, ulPacketLength - 12);

					priRTPPacketInfo -> _ulLength		= ulPacketLength - 12;
					priRTPPacketInfo -> _ulTimeStamp	=
						ntohl (*((unsigned long *) (pucRTPPacket + 4)));

					rhmAudioRTPPacketsSet. insert (
						ntohs (*((unsigned short *) (pucRTPPacket + 2))),
						priRTPPacketInfo);
				}

				if (bTraceOnTTY)
				{
					std:: cout << "RA ";
					std:: cout. flush ();
				}
			}
		}
	}

	// Video
	if (BitstreamFile)
	{
		if (bIsThereVideo)
		{
			int									iFdBitstream;
			unsigned long						ulSequenceIndex;
			unsigned long						ulLostPackets;
			RTPPacketsHashMap_t:: iterator		it;
			RTPPacketInfo_p						priRTPPacketInfo;
			char								pTimeStamp [MP4PL_MAXLONGLENGTH];
			#ifdef WIN32
				__int64							llBytesWritten;
			#else
				long long						llBytesWritten;
			#endif
			int									iFdTimeStamp;


			#ifdef WIN32
				if (FileIO:: open ("VideoBitstream.m4v",
					O_WRONLY | O_TRUNC | O_CREAT | O_BINARY,
					_S_IREAD | _S_IWRITE, &iFdBitstream) !=
					errNoError)
			#else
				if (FileIO:: open ("VideoBitstream.m4v",
					O_WRONLY | O_TRUNC | O_CREAT,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
					&iFdBitstream) != errNoError)
			#endif
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_OPEN_FAILED, 1, "VideoBitstream.m4v");
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			#ifdef WIN32
				if (FileIO:: open ("VideoTimeStamp.txt",
					O_WRONLY | O_TRUNC | O_CREAT | O_TEXT,
					_S_IREAD | _S_IWRITE, &iFdTimeStamp) !=
					errNoError)
			#else
				if (FileIO:: open ("VideoTimeStamp.txt",
					O_WRONLY | O_TRUNC | O_CREAT,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
					&iFdTimeStamp) != errNoError)
			#endif
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_OPEN_FAILED, 1, "VideoBitstream.m4v");
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (FileIO:: close (iFdBitstream) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_CLOSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			ulLostPackets			= 0;

			for (ulSequenceIndex = _ulFirstVideoSequenceNumber;
				ulSequenceIndex <= _ulLastVideoSequenceNumber;
				ulSequenceIndex++)
			{
				it			= rhmVideoRTPPacketsSet. find (ulSequenceIndex);

				if (it == rhmVideoRTPPacketsSet. end ())
				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_VIDEORTPPACKETNOTRECEIVED,
						1, ulSequenceIndex);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					ulLostPackets++;
				}
				else
				{
					priRTPPacketInfo		= it -> second;

					rhmVideoRTPPacketsSet. Delete (it -> first);

					if (FileIO:: writeBytes (iFdBitstream,
						priRTPPacketInfo -> _pucRTPPacket,
						priRTPPacketInfo -> _ulLength) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_FILEIO_WRITEBYTES_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (FileIO:: close (iFdTimeStamp) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_FILEIO_CLOSE_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (FileIO:: close (iFdBitstream) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_FILEIO_CLOSE_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					#ifdef WIN32
						sprintf (pTimeStamp, "%lu\r\n",
							priRTPPacketInfo -> _ulTimeStamp - ulStartVideoTimeStamp);
					#else
						sprintf (pTimeStamp, "%lu\n",
							priRTPPacketInfo -> _ulTimeStamp - ulStartVideoTimeStamp);
					#endif

					if (FileIO:: writeChars (iFdTimeStamp,
						pTimeStamp, strlen (pTimeStamp), &llBytesWritten) !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_FILEIO_WRITEBYTES_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (FileIO:: close (iFdTimeStamp) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_FILEIO_CLOSE_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (FileIO:: close (iFdBitstream) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_FILEIO_CLOSE_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					delete [] (priRTPPacketInfo -> _pucRTPPacket);
					delete priRTPPacketInfo;
				}
			}

			if (FileIO:: close (iFdTimeStamp) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_CLOSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (FileIO:: close (iFdBitstream) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_CLOSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (FileIO:: close (iFdBitstream) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_CLOSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			rhmVideoRTPPacketsSet. clear ();

			{
				Message msg = MP4PlayerMessages (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_VIDEOBITSTREAMINFO,
					5,
					_ulLastVideoSequenceNumber - _ulFirstVideoSequenceNumber,
					_ulLastVideoSequenceNumber - _ulFirstVideoSequenceNumber -
					ulLostPackets,
					_ulFirstVideoSequenceNumber,
					_ulLastVideoSequenceNumber,
					ulTotalVideoBitstreamSize);
				_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
					__FILE__, __LINE__);
			}
		}

		if (bIsThereAudio)
		{
			int									iFdBitstream;
			unsigned long						ulSequenceIndex;
			unsigned long						ulLostPackets;
			RTPPacketsHashMap_t:: iterator		it;
			RTPPacketInfo_p						priRTPPacketInfo;
			char								pTimeStamp [MP4PL_MAXLONGLENGTH];
			#ifdef WIN32
				__int64							llBytesWritten;
			#else
				long long						llBytesWritten;
			#endif
			int									iFdTimeStamp;


			#ifdef WIN32
				if (FileIO:: open ("AudioBitstream.m4v",
					O_WRONLY | O_TRUNC | O_CREAT | O_BINARY,
					_S_IREAD | _S_IWRITE, &iFdBitstream) !=
					errNoError)
			#else
				if (FileIO:: open ("AudioBitstream.m4v",
					O_WRONLY | O_TRUNC | O_CREAT,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
					&iFdBitstream) != errNoError)
			#endif
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_OPEN_FAILED, 1, "AudioBitstream.m4v");
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			#ifdef WIN32
				if (FileIO:: open ("AudioTimeStamp.txt",
					O_WRONLY | O_TRUNC | O_CREAT | O_TEXT,
					_S_IREAD | _S_IWRITE, &iFdTimeStamp) !=
					errNoError)
			#else
				if (FileIO:: open ("AudioTimeStamp.txt",
					O_WRONLY | O_TRUNC | O_CREAT,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
					&iFdTimeStamp) != errNoError)
			#endif
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_OPEN_FAILED, 1, "VideoBitstream.m4v");
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (FileIO:: close (iFdBitstream) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_CLOSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			ulLostPackets			= 0;

			for (ulSequenceIndex = ulStartAudioSequenceNumber;
				ulSequenceIndex <= ulLastAudioSequenceNumber;
				ulSequenceIndex++)
			{
				it			= rhmAudioRTPPacketsSet. find (ulSequenceIndex);

				if (it == rhmAudioRTPPacketsSet. end ())
				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
						MP4PL_RTSPCLIENTSESSION_AUDIORTPPACKETNOTRECEIVED,
						1, ulSequenceIndex);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					ulLostPackets++;
				}
				else
				{
					priRTPPacketInfo		= it -> second;

					rhmAudioRTPPacketsSet. Delete (it -> first);

					if (FileIO:: writeBytes (iFdBitstream,
						priRTPPacketInfo -> _pucRTPPacket,
						priRTPPacketInfo -> _ulLength) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_FILEIO_WRITEBYTES_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (FileIO:: close (iFdTimeStamp) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_FILEIO_CLOSE_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (FileIO:: close (iFdBitstream) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_FILEIO_CLOSE_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					#ifdef WIN32
						sprintf (pTimeStamp, "%lu\r\n",
							priRTPPacketInfo -> _ulTimeStamp - ulStartAudioTimeStamp);
					#else
						sprintf (pTimeStamp, "%lu\n",
							priRTPPacketInfo -> _ulTimeStamp - ulStartAudioTimeStamp);
					#endif

					if (FileIO:: writeChars (iFdTimeStamp,
						pTimeStamp, strlen (pTimeStamp), &llBytesWritten) !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_FILEIO_WRITEBYTES_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (FileIO:: close (iFdTimeStamp) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_FILEIO_CLOSE_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (FileIO:: close (iFdBitstream) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_FILEIO_CLOSE_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					delete [] (priRTPPacketInfo -> _pucRTPPacket);
					delete priRTPPacketInfo;
				}
			}

			if (FileIO:: close (iFdTimeStamp) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_CLOSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (FileIO:: close (iFdBitstream) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_CLOSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (FileIO:: close (iFdBitstream) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_CLOSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			rhmAudioRTPPacketsSet. clear ();

			{
				Message msg = MP4PlayerMessages (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_AUDIOBITSTREAMINFO,
					5,
					ulLastAudioSequenceNumber - ulStartAudioSequenceNumber,
					ulLastAudioSequenceNumber - ulStartAudioSequenceNumber -
					ulLostPackets,
					ulStartAudioSequenceNumber,
					ulLastAudioSequenceNumber,
					ulTotalAudioBitstreamSize);
				_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
					__FILE__, __LINE__);
			}
		}
	}



	return errNoError;
}
*/

