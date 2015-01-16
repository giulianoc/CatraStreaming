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

#include "RTSPSessionRedirection.h"
#include "StringTokenizer.h"
#include "FileIO.h"
#ifdef HAVE_CORBA_H
	#include "coss/CosNaming.h"
#endif


RTSPSessionRedirection:: RTSPSessionRedirection (void):
	RTSPSession ()

{

}


RTSPSessionRedirection:: ~RTSPSessionRedirection (void)

{

}


#if HAVE_CORBA_H
	Error RTSPSessionRedirection:: init (
		CORBA:: ORB_ptr porb,
		unsigned long ulIdentifier,
		unsigned long ulServersNumber,
		StreamingServerInfo_p pssiStreamingServerInfo,
		unsigned long ulMaxRTSPServerSessions,
		unsigned long ulMaxServerBandWidthInKbps,
		const char *pContentRootPath,
		Tracer_p ptTracer)
#else
	Error RTSPSessionRedirection:: init (
		unsigned long ulIdentifier,
		unsigned long ulServersNumber,
		StreamingServerInfo_p pssiStreamingServerInfo,
		unsigned long ulMaxRTSPServerSessions,
		unsigned long ulMaxServerBandWidthInKbps,
		const char *pContentRootPath,
		Tracer_p ptTracer)
#endif

{

	_ulIdentifier					= ulIdentifier;
	_ulServersNumber				= ulServersNumber;
	_ulMaxRTSPServerSessions		= ulMaxRTSPServerSessions;
	_ulMaxServerBandWidthInKbps		= ulMaxServerBandWidthInKbps;
	_ptTracer						= ptTracer;
	_rsRTSPStatus					= SS_RTSP_PREINIT;


	strcpy (_pLocalIPAddressForRTSP, "");
	_ulRTSPRequestsPort				= 0;
	_prpsRTPPacketsToSend			= (RTPPacketsToSend_p) NULL;
	_ulRTSP_RTCPTimeoutInSecs		= MAXLONG;
	_ulPauseTimeoutInSecs			= MAXLONG;

	// not used
	_pmp4fMP4FileFactory			= (MP4FileFactory_p) NULL;
	_ulIntervalInSecsBetweenRTCPPackets		= MAXLONG;
	strcpy (_pLocalIPAddressForRTP, "");
	strcpy (_pLocalIPAddressForRTCP, "");
	_dMaxSpeedAllowed				= MAXLONG;
	_rsRTPSession. _ulSenderRTPMaxSleepTimeInMilliSecs	= MAXLONG;
	_rsRTPSession. _ulPlayDelayTimeInMilliSeconds	= MAXLONG;
	_rsRTPSession. _ulIntervalBetweenBuckets		= MAXLONG;
	_rsRTPSession. _ulMaxSendAheadTimeInSec			= MAXLONG;
	_lReservedStartingPort			= MAXLONG;
	_lReservedFinishingPort			= MAXLONG;
	_rsRTPSession. _ulMaxPayloadSizeInBytes		= MAXLONG;
	_rsRTPSession. _ulMaxRTPPacketsNumberToPrefetch	= MAXLONG;
	_rsRTPSession. _ulRTPPacketsNumberForPrefetchingStep = MAXLONG;

	_lCurrentPortForSetup			= MAXLONG;
	_pmfFile						= (MP4File_p) NULL;
	_dMovieDuration					= -1;

	// _rsRTPSession not used
	// _llInitialLocalTimeInMilliSecs not used
	// _llInitialUTCTimeInMilliSecs not used


	#ifdef HAVE_CORBA_H
		_porb				= porb;
	#endif

	_pssiStreamingServerInfo		=
		pssiStreamingServerInfo;

	if (_csClientSocket. init () != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mtRTSP_RTCPTimeout. init (PMutex:: MUTEX_RECURSIVE) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_csClientSocket. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_CLIENTSOCKET_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bRequestURI. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSP_RTCPTimeout. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
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

		return err;
	}

	if (_bAssetPath. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bRequestURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSP_RTCPTimeout. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
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

		return err;
	}

	if (_mtRTSPSession. init (PMutex:: MUTEX_RECURSIVE) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bAssetPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSP_RTCPTimeout. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
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

		return err;
	}

	if (_bContentRootPath. init (pContentRootPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtRTSPSession. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bAssetPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bRequestURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtRTSP_RTCPTimeout. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
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

		return err;
	}


	return errNoError;
}


Error RTSPSessionRedirection:: finish (void)

{

	if (_bContentRootPath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
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

	if (_bAssetPath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bRequestURI. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_mtRTSP_RTCPTimeout. finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_FINISH_FAILED);
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


	return errNoError;
}


Error RTSPSessionRedirection:: processRTSPRequest (
	const char *pRTSPRequest)

{

	const char						*pLocalRTSPRequest;
	unsigned long					ulMethodLength;


	*praRTSPRequestInfo				= SS_NONE;

	pLocalRTSPRequest			= pRTSPRequest;

	while (pLocalRTSPRequest [0] != '\0')
	{
		if (strncmp (pLocalRTSPRequest, "DESCRIBE", strlen ("DESCRIBE")) == 0)
		{
			if (handleDESCRIBEMethod (pLocalRTSPRequest, &ulMethodLength,
				praRTSPRequestInfo) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_HANDLEDESCRIBEMETHOD_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
		else if (strncmp (pLocalRTSPRequest, "SETUP", strlen ("SETUP")) == 0)
		{
			if (handleSETUPMethod (pLocalRTSPRequest, &ulMethodLength,
				praRTSPRequestInfo) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_HANDLESETUPMETHOD_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_UNKNOWNRTSPREQUEST,
				1, pLocalRTSPRequest);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pLocalRTSPRequest			+= ulMethodLength;
	}


	return errNoError;
}


Error RTSPSessionRedirection:: handleDESCRIBEMethod (const char *pRTSPRequest,
	unsigned long *pulMethodLength)

{

	const char						*pPointerToRTSPRequest;
	StringTokenizer_t				stSpaceTokenizer;
	const char						*pToken;
	Buffer_t						bRTSPVersion;
	Boolean_t						bRTSPVersionRead;
	Buffer_t						bSDP;
	long							lSequenceNumber;
	unsigned long					ulBlockSize;
	Boolean_t						bAcceptSdp;
	Boolean_t						bRequestFinished;
	Error							errNextToken;
	Error							errWrite;
	char							pTimestamp [SS_MAXLONGLENGTH];
	Error_t							errGetInfoFromMP4File;
	Error_t							errGetMP4File;


	// no effect on the server state

	lSequenceNumber					= -1;
	ulBlockSize						= 1460;			// MaxPayloadSize
	bAcceptSdp						= false;
	bRTSPVersionRead				= false;
	bRequestFinished				= false;
	strcpy (pTimestamp, "");

	_tStartConnectionTime			= time (NULL);

	if (_rsRTSPStatus != RTSPSession:: SS_RTSP_PREINIT)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			1, (long) _rsRTSPStatus);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			bRTSPVersionRead ? (const char *) bRTSPVersion :
			SS_PREDEFINEDRTSPVERSION, 455, lSequenceNumber,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs,
			pTimestamp, false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bRTSPVersion. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			bRTSPVersionRead ? (const char *) bRTSPVersion :
			SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	pPointerToRTSPRequest		= pRTSPRequest;

	while (!bRequestFinished)
	{
		if (*pPointerToRTSPRequest == '\n' ||	// means "\n\n"
			strchr (pPointerToRTSPRequest, '\n') == (char *) NULL)
		{
			bRequestFinished				= true;

			*pulMethodLength			= (unsigned long)
				(pPointerToRTSPRequest - pRTSPRequest + 1);

			continue;
		}

		if (stSpaceTokenizer. init (pPointerToRTSPRequest,
			(long) (strchr (pPointerToRTSPRequest, '\n') - pPointerToRTSPRequest),
			" ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		pPointerToRTSPRequest		= strchr (pPointerToRTSPRequest, '\n') + 1;

		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!strcmp (pToken, "DESCRIBE"))
		{
			char					*pRTSPVersion;


			// 'DESCRIBE <Request-URI> <RTSP-Version>'
			// Request-URI
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((pRTSPVersion = strrchr (pToken, ' ')) == (char *) NULL)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_DESCRIBEURIWRONG, 1, pToken);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (_bRequestURI. setBuffer (pToken,
				(long) (pRTSPVersion - pToken)) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SETBUFFER_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			#ifdef LUXXON_REQUEST
				_bRequestURI. substitute ("req1.0?", "");
			#endif

			// initialize _bAssetPath
			{
				const char					*pAssetRelativePath;


				pAssetRelativePath				= strchr (
					(const char *) _bRequestURI, '/');
				if (pAssetRelativePath == (const char *) NULL ||
					*(pAssetRelativePath + 1) != '/')
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_DESCRIBEURIWRONG, 1, pToken);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						bRTSPVersionRead ? (const char *) bRTSPVersion :
						SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
						false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bRTSPVersion. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				pAssetRelativePath				+= 2;
				pAssetRelativePath				= strchr (pAssetRelativePath,
					'/');
				if (pAssetRelativePath == (const char *) NULL)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_DESCRIBEURIWRONG, 1, pToken);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						bRTSPVersionRead ? (const char *) bRTSPVersion :
						SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
						false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bRTSPVersion. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (_bAssetPath. setBuffer (
					(const char *) _bContentRootPath) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SETBUFFER_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						bRTSPVersionRead ? (const char *) bRTSPVersion :
						SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
						false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bRTSPVersion. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (_bAssetPath. append (pAssetRelativePath) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						bRTSPVersionRead ? (const char *) bRTSPVersion :
						SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
						false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bRTSPVersion. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				#ifdef WIN32
					// Since the URL contains a path with '/', we must convert them to '\'
					if (_bAssetPath. substitute ("/", "\\") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_SUBSTITUTE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (sendRTSPResponse (_psiClientSocketImpl,
							bRTSPVersionRead ? (const char *) bRTSPVersion :
							SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
							(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
							false) != errNoError)
						{
							Error err = StreamingServerErrors (__FILE__, __LINE__,
								SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bRTSPVersion. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				#endif
			}

			// <RTSP-Version>
			if (bRTSPVersion. setBuffer (pRTSPVersion + 1) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SETBUFFER_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			bRTSPVersionRead				= true;
		}
		else if (!strcmp (pToken, "CSeq:"))
		{
			// 'CSeq: <SequenceNumber>'
			// SequenceNumber
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			lSequenceNumber				= atol (pToken);
		}
		else if (!strcmp (pToken, "Timestamp:"))
		{
			// 'Timestamp: <Timestamp>'
			// Timestamp
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (pTimestamp, pToken);
		}
		else if (!strcmp (pToken, "Blocksize:"))
		{
			// 'Blocksize: <Blocksize>'
			// Blocksize
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			ulBlockSize				= atol (pToken);
		}
		else if (!strcmp (pToken, "Accept:"))
		{
			// 'Accept: <DescriptionContentType>, ..., <DescriptionContentType>'
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (strstr (pToken, "application/sdp"))
				bAcceptSdp				= true;
		}
		else
		{
			;
		}

		if (stSpaceTokenizer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// check for DESCRIBE request
	{
		if (!bRTSPVersionRead || lSequenceNumber == -1 || bAcceptSdp == false ||
			!strcmp ((const char *) _bAssetPath, ""))
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_DESCRIBEREQUESTWRONG,
				1, pRTSPRequest);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	{
		Buffer_t				bRedirectURI;
		Buffer_t				bResponse;
		StreamingServerInfo_p	pssiChoiceStreamingServer;
		Error_t					errChoiceStreamingServer;


		if ((errChoiceStreamingServer = choiceStreamingServerToRedirect (
			(const char *) _bAssetPath, &pssiChoiceStreamingServer)) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSIONREDIRECTION_CHOICESTREAMINGSERVERTOREDIRECT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if ((long) errChoiceStreamingServer ==
				SS_RTSPSESSIONREDIRECTION_NOSTREAMINGSERVERAVAILABLE)
			{
				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 503, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
			else
			{
				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bResponse. init () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bRedirectURI. init ("rtsp://") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// initialize bRedirectURI
		{
			char					pURLIPAddress [
				SCK_MAXIPADDRESSLENGTH];
			long					lRTSPPort;
			Buffer_t				bRelativePath;


			if (bRelativePath. init () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (parseURL ((const char *) _bRequestURI,
				pURLIPAddress, &lRTSPPort, &bRelativePath) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_PARSEURL_FAILED,
					1, (const char *) _bRequestURI);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRedirectURI. append (pssiChoiceStreamingServer -> _pIpAddress) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRedirectURI. append (":") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRedirectURI. append (pssiChoiceStreamingServer -> _ulPort) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRedirectURI. append ("/") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRedirectURI. append ((const char *) bRelativePath) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRelativePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (getRTSPResponse (bRTSPVersionRead ? (const char *) bRTSPVersion :
			SS_PREDEFINEDRTSPVERSION,
			301, lSequenceNumber, (const char *) NULL, 0,
			false, pTimestamp, &bResponse) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_GETRTSPRESPONSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRedirectURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION,
				500, lSequenceNumber, (const char *) NULL,
				_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bResponse. append ("Location: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRedirectURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION,
				500, lSequenceNumber, (const char *) NULL,
				_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bResponse. append ((const char *) bRedirectURI) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRedirectURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION,
				500, lSequenceNumber, (const char *) NULL,
				_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bRedirectURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION,
				500, lSequenceNumber, (const char *) NULL,
				_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bResponse. append (SS_NEWLINE SS_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION,
				500, lSequenceNumber, (const char *) NULL,
				_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDTOCLIENT,
				1, (const char *) bResponse);
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		if ((errWrite = _psiClientSocketImpl -> writeString (
			(const char *) bResponse)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errWrite, __FILE__, __LINE__);
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETIMPL_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION,
				500, lSequenceNumber, (const char *) NULL,
				_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (bRTSPVersion. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	_rsRTSPStatus			= RTSPSession:: SS_RTSP_INIT;


	return errNoError;
}


Error RTSPSessionRedirection:: handleSETUPMethod (const char *pRTSPRequest,
	unsigned long *pulMethodLength,
	RTSPRequestInfo_p praRTSPRequestInfo)

{

	const char						*pPointerToRTSPRequest;
	StringTokenizer_t				stSpaceTokenizer;
	const char						*pToken;
	Buffer_t						bString;
	unsigned long					ulBlockSize;
	Buffer_t						bURI;
	Boolean_t						bRTSPVersionRead;
	Buffer_t						bRTSPVersion;
	Buffer_t						bTransport;
	long							lSequenceNumber;
	long							lRTPClientPort;
	long							lRTCPClientPort;
	char							pSessionIdentifier [
		SS_MAXSESSIONIDENTIFIERLENGTH];

	Boolean_t						bRequestFinished;
	Error							errNextToken;
	Error							errWrite;
	char							pTimestamp [SS_MAXLONGLENGTH];


	bRTSPVersionRead				= false;
	ulBlockSize						= 1460;			// MaxPayloadSize
	lSequenceNumber					= -1;
	lRTPClientPort					= -1;
	lRTCPClientPort					= -1;
	strcpy (pSessionIdentifier, "");
	bRequestFinished				= false;
	strcpy (pTimestamp, "");

	// this method can be called in every states but
	if (_rsRTSPStatus == RTSPSession:: SS_RTSP_PLAYING ||
		_rsRTSPStatus == RTSPSession:: SS_RTSP_RECORDING)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPSESSION_WRONGSTATE,
			1, (long) _rsRTSPStatus);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			SS_PREDEFINEDRTSPVERSION, 455, lSequenceNumber,
			strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bRTSPVersion. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			bRTSPVersionRead ? (const char *) bRTSPVersion :
			SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
			strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bTransport. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sendRTSPResponse (_psiClientSocketImpl,
			bRTSPVersionRead ? (const char *) bRTSPVersion :
			SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
			strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bRTSPVersion. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bURI. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bTransport. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (sendRTSPResponse (_psiClientSocketImpl,
			bRTSPVersionRead ? (const char *) bRTSPVersion :
			SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
			strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
			(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
			false) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bRTSPVersion. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	pPointerToRTSPRequest		= pRTSPRequest;

	while (!bRequestFinished)
	{
		if (*pPointerToRTSPRequest == '\n' ||	// means "\n\n"
			strchr (pPointerToRTSPRequest, '\n') == (char *) NULL)
		{
			bRequestFinished				= true;

			*pulMethodLength			= (unsigned long)
				(pPointerToRTSPRequest - pRTSPRequest + 1);

			continue;
		}

		if (stSpaceTokenizer. init (pPointerToRTSPRequest,
			(long) (strchr (pPointerToRTSPRequest, '\n') - pPointerToRTSPRequest),
			" ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		pPointerToRTSPRequest		= strchr (pPointerToRTSPRequest, '\n') + 1;

		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!strcmp (pToken, "SETUP"))
		{
			// 'SETUP <Request-URI> <RTSP-Version>'
			// Request-URI
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bURI. setBuffer (pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SETBUFFER_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			#ifdef LUXXON_REQUEST
				bURI. substitute ("req1.0?", "");
			#endif

			// <RTSP-Version>
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRTSPVersion. setBuffer (pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SETBUFFER_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			bRTSPVersionRead				= true;
		}
		else if (!strcmp (pToken, "CSeq:"))
		{
			// 'CSeq: <SequenceNumber>'
			// SequenceNumber
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			lSequenceNumber				= atol (pToken);
		}
		else if (!strcmp (pToken, "Timestamp:"))
		{
			// 'Timestamp: <Timestamp>'
			// Timestamp
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (pTimestamp, pToken);
		}
		else if (!strcmp (pToken, "Blocksize:"))
		{
			// 'Blocksize: <Blocksize>'
			// Blocksize
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			ulBlockSize				= atol (pToken);
		}
		else if (!strcmp (pToken, "Transport:"))
		{
			if (stSpaceTokenizer. nextToken (&pToken, ";") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (strstr (pToken, "RTP/AVP") == (const char *) NULL ||
				strstr (pToken, "RTP/AVP/TCP") != (const char *) NULL)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_TRANSPORTNOTCOMPLIANT,
					1, pToken);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// parameters
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (strstr (pToken, "unicast") == (const char *) NULL)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_TRANSPORTNOTCOMPLIANT,
					1, pToken);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// Initialize lRTPClientPort and lRTCPClientPort
			{
				const char					*pRTPClientPort;
				char						*pRTCPClientPort;


				pRTPClientPort				= strstr (pToken, "client_port=");
				if (pRTPClientPort == (const char *) NULL)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SETUPTRANSPORTWRONG, 1, pToken);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
					}

					if (bURI. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bTransport. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						bRTSPVersionRead ? (const char *) bRTSPVersion :
						SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
						strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
						false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bRTSPVersion. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
				pRTPClientPort					+= strlen ("client_port");

				pRTCPClientPort					= strchr (pRTPClientPort, '-');
				if (pRTCPClientPort == (const char *) NULL)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SETUPTRANSPORTWRONG, 1, pToken);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
					}

					if (bURI. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bTransport. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (sendRTSPResponse (_psiClientSocketImpl,
						bRTSPVersionRead ? (const char *) bRTSPVersion :
						SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
						strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
						(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
						false) != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bRTSPVersion. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				lRTCPClientPort				= atol (pRTCPClientPort + 1);
				*pRTCPClientPort			= '\0';

				lRTPClientPort				= atol (pRTPClientPort + 1);
			}

			if (bTransport. setBuffer ("Transport: ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SETBUFFER_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bTransport. append ("RTP/AVP/UDP;unicast;client_port=") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bTransport. append (lRTPClientPort) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bTransport. append ("-") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bTransport. append (lRTCPClientPort) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (!strcmp (pToken, "Session:"))
		{
			// 'Session: <SessionIdentifier>[;timeout=...]'
			// SessionIdentifier
			if (stSpaceTokenizer. nextToken (&pToken, ";") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (pSessionIdentifier, pToken);
		}
		else
		{
			;
		}

		if (stSpaceTokenizer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// check for SETUP request
	{
		if (lSequenceNumber == -1 || lRTPClientPort == -1 ||
			lRTCPClientPort == -1 ||
			(_rsRTSPStatus == RTSPSession:: SS_RTSP_READY &&
			!strcmp (pSessionIdentifier, "")))
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SETUPREQUESTWRONG,
				1, pRTSPRequest);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (_rsRTSPStatus == RTSPSession:: SS_RTSP_PREINIT)
	{
		// the connection started with the SETUP RTSP command
		//		and we don't have the DESCRIBE RTSP command

		char					pURLIPAddress [
			SCK_MAXIPADDRESSLENGTH];
		long					lRTSPPort;
		Buffer_t				bRelativePath;
		Boolean_t				bMP4FileFound;
		Error_t					errGetMP4File;
		Error_t					errGetInfoFromMP4File;


		if (bRelativePath. init () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (parseURL ((const char *) bURI,
			pURLIPAddress, &lRTSPPort, &bRelativePath) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_PARSEURL_FAILED,
				1, (const char *) bURI);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRelativePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_bRequestURI. setBuffer ("rtsp://") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRelativePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_bRequestURI. append (pURLIPAddress) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRelativePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (lRTSPPort != -1)
		{
			if (_bRequestURI. append (":") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (_bRequestURI. append (lRTSPPort) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (_bRequestURI. append ((const char *) bRelativePath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRelativePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_bAssetPath. setBuffer (
			(const char *) _bContentRootPath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRelativePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_bAssetPath. append ((const char *) bRelativePath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRelativePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		#ifdef WIN32
			// Since the URL contains a path with '/', we must convert them to '\'
			if (_bAssetPath. substitute ("/", "\\") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SUBSTITUTE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		#endif

		bMP4FileFound			= false;

		while (!bMP4FileFound)
		{
			#ifdef WIN32
				if (strrchr ((const char *) _bAssetPath, '\\') == (char *) NULL)
			#else
				if (strrchr ((const char *) _bAssetPath, '/') == (char *) NULL)
			#endif
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SETUPURIWRONGBECAUSEFILENAME,
					1, (const char *) bURI);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			#ifdef WIN32
				*strrchr ((const char *) _bAssetPath, '\\')		= '\0';
			#else
				*strrchr ((const char *) _bAssetPath, '/')		= '\0';
			#endif

			*strrchr ((const char *) _bRequestURI, '/')		= '\0';

			if (FileIO:: exist ((const char *) _bAssetPath, &bMP4FileFound) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_EXIST_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
					strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
					false) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		_bVideoTrackInitializedFromSetup			= false;
		_bAudioTrackInitializedFromSetup			= false;

		if (bRelativePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pmp4fMP4FileFactory -> releaseMP4File (
				(const char *) _bAssetPath) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_GETMP4FILE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			_pmfFile				= (MP4File_p) NULL;

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 500, lSequenceNumber,
				strcmp (pSessionIdentifier, "") ? pSessionIdentifier :
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp,
				false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	{
		Buffer_t				bRedirectURI;
		Buffer_t				bResponse;
		StreamingServerInfo_p	pssiChoiceStreamingServer;
		Error_t					errChoiceStreamingServer;


		if ((errChoiceStreamingServer = choiceStreamingServerToRedirect (
			(const char *) _bAssetPath, &pssiChoiceStreamingServer)) !=
			errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSIONREDIRECTION_CHOICESTREAMINGSERVERTOREDIRECT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if ((long) errChoiceStreamingServer ==
				SS_RTSPSESSIONREDIRECTION_NOSTREAMINGSERVERAVAILABLE)
			{
				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 503, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}
			else
			{
				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bResponse. init () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bRedirectURI. init ("rtsp://") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
				(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// initialize bRedirectURI
		{
			char					pURLIPAddress [
				SCK_MAXIPADDRESSLENGTH];
			long					lRTSPPort;
			Buffer_t				bRelativePath;


			if (bRelativePath. init () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (parseURL ((const char *) bURI,
				pURLIPAddress, &lRTSPPort, &bRelativePath) !=
				errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_PARSEURL_FAILED,
					1, (const char *) bURI);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRedirectURI. append (
				pssiChoiceStreamingServer -> _pIpAddress) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRedirectURI. append (":") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRedirectURI. append (
				pssiChoiceStreamingServer -> _ulPort) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRedirectURI. append ("/") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRedirectURI. append ((const char *) bRelativePath) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRelativePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRelativePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRedirectURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURI. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bTransport. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (sendRTSPResponse (_psiClientSocketImpl,
					bRTSPVersionRead ? (const char *) bRTSPVersion :
					SS_PREDEFINEDRTSPVERSION, 400, lSequenceNumber,
					(const char *) NULL, _ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRTSPVersion. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (getRTSPResponse (bRTSPVersionRead ? (const char *) bRTSPVersion :
			SS_PREDEFINEDRTSPVERSION,
			301, lSequenceNumber, (const char *) NULL, 0,
			false, pTimestamp, &bResponse) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_GETRTSPRESPONSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRedirectURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION,
				500, lSequenceNumber, (const char *) NULL,
				_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bResponse. append ("Location:") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRedirectURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION,
				500, lSequenceNumber, (const char *) NULL,
				_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bResponse. append ((const char *) bRedirectURI) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRedirectURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION,
				500, lSequenceNumber, (const char *) NULL,
				_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bRedirectURI. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION,
				500, lSequenceNumber, (const char *) NULL,
				_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bResponse. append (SS_NEWLINE SS_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION,
				500, lSequenceNumber, (const char *) NULL,
				_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDTOCLIENT,
				1, (const char *) bResponse);
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		if ((errWrite = _psiClientSocketImpl -> writeString (
			(const char *) bResponse)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errWrite, __FILE__, __LINE__);
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETIMPL_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (sendRTSPResponse (_psiClientSocketImpl,
				bRTSPVersionRead ? (const char *) bRTSPVersion :
				SS_PREDEFINEDRTSPVERSION,
				500, lSequenceNumber, (const char *) NULL,
				_ulRTSP_RTCPTimeoutInSecs, pTimestamp, false) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_SENDRTSPRESPONSE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bResponse. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bURI. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bTransport. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRTSPVersion. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (bURI. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bTransport. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bRTSPVersion. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bTransport. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bRTSPVersion. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bRTSPVersion. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_rsRTSPStatus == RTSPSession:: SS_RTSP_INIT ||
		_rsRTSPStatus == RTSPSession:: SS_RTSP_PREINIT)
		_rsRTSPStatus			= RTSPSession:: SS_RTSP_READY;


	return errNoError;
}


Error RTSPSessionRedirection:: choiceStreamingServerToRedirect (
	const char *pAssetPath, StreamingServerInfo_p *pssiChoiceStreamingServer)

{


	CosNaming:: NamingContext_var			pnc;
	::StreamingIDL:: StreamingServer_var	pssStreamingServer;
	CORBA:: ULong							bIsInCache;
	CORBA:: ULong							ulConnectedUsersNumber;
	CORBA:: ULong							ulBandWidthUsageInbps;
	unsigned long							ulServerIndex;
	StreamingServerInfo_p			pssiStreamingServerLessStressedAvailable;
	unsigned long					ulStreamingServerLessStressedAvailable;


	try
	{
		CORBA::Object_var nsobj		=
			_porb -> resolve_initial_references ("NameService");

		pnc							=
			CosNaming::NamingContext::_narrow (nsobj);

		if (CORBA:: is_nil (pnc))
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_REGISTERTONAMINGSERVICE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	catch (CosNaming::NamingContext::NotFound &)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTNOTFOUND);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	catch (CosNaming::NamingContext::CannotProceed &)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTCANNOTPROCEED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	catch (CosNaming::NamingContext::InvalidName &)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTINVALIDNAME);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	catch (CORBA::SystemException &)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERSMANAGER_NAMINGCONTEXTGENERICERROR,
			1, "NameService");
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	catch (...)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_CORBA_GENERICERROR);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	pssiStreamingServerLessStressedAvailable	= (StreamingServerInfo_p) NULL;
	ulStreamingServerLessStressedAvailable		= (unsigned long) -1;

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

			if (pssStreamingServer -> isRequestInCache (
				pAssetPath,
				bIsInCache,
				ulConnectedUsersNumber,
				ulBandWidthUsageInbps) != 0)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVER_ISREQUESTINCACHE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
		}
		catch (::StreamingIDL:: StreamingExc &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_STREAMINGSERVERERROR,
				1, "isRequestInCache");
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		catch (CORBA::SystemException &)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERSMANAGER_STREAMINGSERVERERROR,
				1, "isRequestInCache");
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		catch (...)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_CORBA_GENERICERROR);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (bIsInCache && ulConnectedUsersNumber < _ulMaxRTSPServerSessions)
			break;

		if (ulConnectedUsersNumber < ulStreamingServerLessStressedAvailable &&
			ulConnectedUsersNumber < _ulMaxRTSPServerSessions &&
			ulBandWidthUsageInbps < _ulMaxServerBandWidthInKbps * 1000)
		{
			ulStreamingServerLessStressedAvailable		= ulConnectedUsersNumber;
			pssiStreamingServerLessStressedAvailable	= &(_pssiStreamingServerInfo [ulServerIndex]);
		}
	}

	if (ulServerIndex == _ulServersNumber)
	{
		// reached the end of the loop (no file already cached)
		if (pssiStreamingServerLessStressedAvailable == (StreamingServerInfo_p) NULL)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSIONREDIRECTION_NOSTREAMINGSERVERAVAILABLE);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
		else
			*pssiChoiceStreamingServer				= pssiStreamingServerLessStressedAvailable;
	}
	else
		*pssiChoiceStreamingServer			= &(_pssiStreamingServerInfo [ulServerIndex]);


	return errNoError;
}
