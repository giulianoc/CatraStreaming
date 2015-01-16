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

#include "SDP.h"
#include <stdlib.h>
#include <assert.h>



SDP:: SDP (void)

{

}


SDP:: ~SDP (void)

{

}



SDP:: SDP (const SDP &)

{

	assert (1==0);

	// to do

}


SDP &SDP:: operator = (const SDP &)

{

	assert (1==0);

	// to do

	return *this;

}


Error SDP:: init (const char *pSDPPathName, Tracer_p ptTracer)

{

	Buffer_t				bSDP;
	Error_t					errReadBufferFromFile;


	if (bSDP. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errReadBufferFromFile = bSDP. readBufferFromFile (
		pSDPPathName)) != errNoError)
	{
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errReadBufferFromFile,
			__FILE__, __LINE__);

		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_READBUFFERFROMFILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bSDP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (init (&bSDP, ptTracer) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bSDP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bSDP. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error SDP:: init (Buffer_p pbSDP, Tracer_p ptTracer)

{

	StringTokenizer_t				stLineTokenizer;
	const char						*pType;
	const char						*pValue;
	const char						*pPointerToSDPLine;
	Boolean_t						bSDPFinished;
	SDPAnouncementState_t			sasState;
	SDPMedia_p						psmCurrentSDPMedia;
	Error_t							errNextToken;



	if (pbSDP == (Buffer_p) NULL ||
		ptTracer == (Tracer_p) NULL)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_ACTIVATIONWRONG);

		return err;
	}

	_ptTracer				= ptTracer;

	// if it is a windows file or the SDP is retrieved from a RTSP answer
	// it is necessary to replace the "\r\n" with "\n"
	if (pbSDP -> substitute ("\r\n", "\n") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SUBSTITUTE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	_ulVersion						= 0;

	strcpy (_pOriginUserName, "");
	strcpy (_pOriginSessionId, "");
	strcpy (_pOriginVersion, "");
	strcpy (_pOriginNetworkType, "");

	strcpy (_pOriginAddressType, "");
	strcpy (_pOriginAddress, "");

	strcpy (_pSessionName, "");
	strcpy (_pInformation, "");
	strcpy (_pURI, "");
	strcpy (_pEmailAddress, "");
	strcpy (_pPhoneNumber, "");

	strcpy (_pConnectionNetworkType, "");
	strcpy (_pConnectionAddressType, "");
	strcpy (_pConnectionAddress, "");

	strcpy (_pBandwidthModifier, "");
	_ulBandwidthValueInKbps			= 0;

	_ulTimesStart					= 0;
	_ulTimesStop					= 0;

	_vSDPAttributes. clear ();
	_vSDPMedia. clear ();


	pPointerToSDPLine		= (const char *) (*pbSDP);

	sasState				= SDP_UNKNOWN;

	bSDPFinished			= false;

	while (!bSDPFinished)
	{
		if (*pPointerToSDPLine == '\n' ||	// means "\n\n"
			strchr (pPointerToSDPLine, '\n') == (char *) NULL)
		{
			bSDPFinished				= true;

			continue;
		}

		if (stLineTokenizer. init (pPointerToSDPLine,
			(long) (strchr (pPointerToSDPLine, '\n') -
			pPointerToSDPLine), "=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}


		// v=0
		// o=root <nowUTC> <noUTC> IN IP4 <IPAddress RTSP>
		// s=<session name>
		// c=IN IP4 0.0.0.0
		// t=0 0
		// a=control:*
		// a=range:npt=0-<movieDuration>
		// m=<application> 0 RTP/AVP <payload> 
		// 		<application>=video | audio
		// b=AS:64
		// a=rtpmap:<payload> <???>/<ClockRate>[/<Channels>]
		// 		???=MP4V-ES|H263-2000|MP4A-LATM|mpeg4-generic|AMR|AMR-WB
		// a=mpeg4-esid:<trackIdentifier>
		// a=fmtp:<payload> <???>
		// 		<???> = profile-level-id=<SystemProfileLevel>;config=<ESconfig>
		// 			op.
		// 		<???> = framesize=<Width>-<Height>
		// 			op.
		// 		<????> = cpresent=0;config=<ESconfig>
		// 			op.
		// 		<????> = streamtype=5;profile-level-id=15;mode=CELP-vbr;config=<ESconfig>;SizeLength=6;IndexLength=2;IndexDeltaLength=2;Profile=0
		// 			op.
		// 		<????> = streamtype=5;profile-level-id=15;mode=AAC-hbr;config=<ESconfig>;SizeLength=13;IndexLength=3;IndexDeltaLength=3;Profile=1
		// 			op.
		// 		<????> = octet-align=1
		// a=control:trackID/<TrackID>

		// Type
		if (stLineTokenizer. nextToken (&pType) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stLineTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// Manage the Value
		if (pType [0] == SDP_VERSION)
		{
			// v=0
			if (sasState != SDP_UNKNOWN)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_WRONGSTATEFORTHISLINE,
					1, pPointerToSDPLine);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stLineTokenizer. nextToken (&pValue, "\n") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (setVersion (atol (pValue)) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_SETVERSION_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sasState			= SDP_SESSIONLEVEL;
		}
		else if (pType [0] == SDP_ORIGIN)
		{
			// o=<username> <session id> <version> <network type> <address type> <address>
			if (sasState != SDP_SESSIONLEVEL)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_WRONGSTATEFORTHISLINE,
					1, pPointerToSDPLine);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// username>
			if (stLineTokenizer. nextToken (&pValue, " ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (setOriginUserName (pValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_SETORIGINUSERNAME_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// <session id>
			if (stLineTokenizer. nextToken (&pValue, " ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (setOriginSessionId (pValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_SETORIGINSESSIONID_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// <version>
			if (stLineTokenizer. nextToken (&pValue, " ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (setOriginVersion (pValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_SETORIGINVERSION_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// <network type>
			if (stLineTokenizer. nextToken (&pValue, " ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (setOriginNetworkType (pValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_SETORIGINNETWORKTYPE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// <address type>
			if (stLineTokenizer. nextToken (&pValue, " ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (setOriginAddressType (pValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_SETORIGINADDRESSTYPE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// <address>
			if (stLineTokenizer. nextToken (&pValue, "\n") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (setOriginAddress (pValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_SETORIGINADDRESS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (pType [0] == SDP_SESSIONNAME)
		{
			// s=<session name>
			if (sasState != SDP_SESSIONLEVEL)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_WRONGSTATEFORTHISLINE,
					1, pPointerToSDPLine);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stLineTokenizer. nextToken (&pValue, "\n") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (setSessionName (pValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_SETSESSIONNAME_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (pType [0] == SDP_INFORMATION)
		{
			// i=[<session description>]
			if (sasState != SDP_SESSIONLEVEL &&
				sasState != SDP_MEDIALEVEL)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_WRONGSTATEFORTHISLINE,
					1, pPointerToSDPLine);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((errNextToken = stLineTokenizer. nextToken (&pValue, "\n")) !=
				errNoError)
			{
				if ((long) errNextToken == TOOLS_STRINGTOKENIZER_NOMORETOKEN)
				{
					// i=
					if (sasState == SDP_SESSIONLEVEL)
					{
						if (setInformation ("") != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_SETINFORMATION_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (stLineTokenizer. finish () != errNoError)
							{
								Error err = ToolsErrors (__FILE__, __LINE__,
									TOOLS_STRINGTOKENIZER_FINISH_FAILED);
								_ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							if (finish () != errNoError)
							{
								Error err = SDPErrors (__FILE__, __LINE__,
									SDP_SDP_FINISH_FAILED);
								_ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
					}
					else
					{
						if (psmCurrentSDPMedia -> setInformation (
							"") != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDPMEDIA_SETINFORMATION_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (stLineTokenizer. finish () != errNoError)
							{
								Error err = ToolsErrors (__FILE__, __LINE__,
									TOOLS_STRINGTOKENIZER_FINISH_FAILED);
								_ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							if (finish () != errNoError)
							{
								Error err = SDPErrors (__FILE__, __LINE__,
									SDP_SDP_FINISH_FAILED);
								_ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
					}
				}
				else
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				if (sasState == SDP_SESSIONLEVEL)
				{
					if (setInformation (pValue) != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_SETINFORMATION_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stLineTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
				else
				{
					if (psmCurrentSDPMedia -> setInformation (
						pValue) != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDPMEDIA_SETINFORMATION_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stLineTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
			}
		}
		else if (pType [0] == SDP_URI)
		{
			// u=<URI>
			if (sasState != SDP_SESSIONLEVEL)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_WRONGSTATEFORTHISLINE,
					1, pPointerToSDPLine);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stLineTokenizer. nextToken (&pValue, "\n") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (setURI (pValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_SETURI_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (pType [0] == SDP_EMAILADDRESS)
		{
			// e=<email address>
			if (sasState != SDP_SESSIONLEVEL)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_WRONGSTATEFORTHISLINE,
					1, pPointerToSDPLine);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stLineTokenizer. nextToken (&pValue, "\n") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (setEmailAddress (pValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_SETEMAILADDRESS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (pType [0] == SDP_PHONENUMBER)
		{
			// p=<phone number>
			if (sasState != SDP_SESSIONLEVEL)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_WRONGSTATEFORTHISLINE,
					1, pPointerToSDPLine);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stLineTokenizer. nextToken (&pValue, "\n") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (setPhoneNumber (pValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_SETPHONENUMBER_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (pType [0] == SDP_CONNECTIONDATA)
		{
			// c=<network type> <address type> <connection address>
			if (sasState != SDP_SESSIONLEVEL &&
				sasState != SDP_MEDIALEVEL)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_WRONGSTATEFORTHISLINE,
					1, pPointerToSDPLine);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stLineTokenizer. nextToken (&pValue, " ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (sasState == SDP_SESSIONLEVEL)
			{
				if (setConnectionNetworkType (pValue) != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_SETCONNECTIONNETWORKTYPE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				if (psmCurrentSDPMedia -> setConnectionNetworkType (
					pValue) != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPMEDIA_SETCONNECTIONNETWORKTYPE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			if (stLineTokenizer. nextToken (&pValue, " ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (sasState == SDP_SESSIONLEVEL)
			{
				if (setConnectionAddressType (pValue) != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_SETCONNECTIONADDRESSTYPE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				if (psmCurrentSDPMedia -> setConnectionAddressType (
					pValue) != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPMEDIA_SETCONNECTIONADDRESSTYPE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			if (stLineTokenizer. nextToken (&pValue, "\n") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (sasState == SDP_SESSIONLEVEL)
			{
				if (setConnectionAddress (pValue) != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_SETCONNECTIONADDRESS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				if (psmCurrentSDPMedia -> setConnectionAddress (
					pValue) != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPMEDIA_SETCONNECTIONADDRESS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
		}
		else if (pType [0] == SDP_BANDWIDTH)
		{
			// b=<modifier>:<bandwidth-value>
			if (sasState != SDP_SESSIONLEVEL &&
				sasState != SDP_MEDIALEVEL)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_WRONGSTATEFORTHISLINE,
					1, pPointerToSDPLine);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stLineTokenizer. nextToken (&pValue, ":") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (sasState == SDP_SESSIONLEVEL)
			{
				if (setBandwidthModifier (pValue) != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_SETBANDWIDTHMODIFIER_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				if (psmCurrentSDPMedia -> setBandwidthModifier (
					pValue) != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPMEDIA_SETBANDWIDTHMODIFIER_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			if (stLineTokenizer. nextToken (&pValue, "\n") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (sasState == SDP_SESSIONLEVEL)
			{
				if (setBandwidthValueInKbps (atol (pValue)) != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_SETBANDWIDTHVALUEINKBPS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				if (psmCurrentSDPMedia -> setBandwidthValueInKbps (
					atol (pValue)) != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPMEDIA_SETBANDWIDTHVALUEINKBPS_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
		}
		else if (pType [0] == SDP_TIMES)
		{
			// t=<start time>  <stop time>
			if (sasState != SDP_SESSIONLEVEL)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_WRONGSTATEFORTHISLINE,
					1, pPointerToSDPLine);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stLineTokenizer. nextToken (&pValue, " ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (setTimesStart (atol (pValue)) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_SETTIMESSTART_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stLineTokenizer. nextToken (&pValue, "\n") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (setTimesStop (atol (pValue)) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_SETTIMESSTOP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (pType [0] == SDP_ATTRIBUTE)
		{
			// a=<attribute>
			// a=<attribute>:<value>

			SDPAttribute_p			psaSDPAttribute;


			if (sasState != SDP_SESSIONLEVEL &&
				sasState != SDP_MEDIALEVEL)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_WRONGSTATEFORTHISLINE,
					1, pPointerToSDPLine);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stLineTokenizer. nextToken (&pValue, ":") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (!strcmp (pValue, SDP_RTPMAPATTRIBUTENAME))
			{
				SDPRtpMapAttribute_p			psaSDPRtpMapAttribute;


				// a=rtpmap:<payload type> <encoding name>/<clock rate>[/<encoding parameters>]
				if ((psaSDPRtpMapAttribute = new SDPRtpMapAttribute_t) ==
					(SDPRtpMapAttribute_p) NULL)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_NEW_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (strlen (pValue) >= SDP_ATTRIBUTENAMELENGTH)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FIELDTOOLONG,
						1, pValue);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPRtpMapAttribute;

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				strcpy (psaSDPRtpMapAttribute -> _pName, pValue);

				// payload type
				if (stLineTokenizer. nextToken (&pValue, " ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPRtpMapAttribute;

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (strlen (pValue) >= SDP_PAYLOADTYPELENGTH)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FIELDTOOLONG,
						1, pValue);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPRtpMapAttribute;

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				strcpy (psaSDPRtpMapAttribute -> _pPayloadType, pValue);

				// encoding name
				if (stLineTokenizer. nextToken (&pValue, "/") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPRtpMapAttribute;

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (strlen (pValue) >= SDP_ENCODINGNAMELENGTH)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FIELDTOOLONG,
						1, pValue);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPRtpMapAttribute;

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				strcpy (psaSDPRtpMapAttribute -> _pEncodingName, pValue);

				// clock rate
				if (stLineTokenizer. nextToken (&pValue, "/") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPRtpMapAttribute;

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (strlen (pValue) >= SDP_CLOCKRATELENGTH)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FIELDTOOLONG,
						1, pValue);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPRtpMapAttribute;

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				strcpy (psaSDPRtpMapAttribute -> _pClockRate, pValue);

				// encoding parameters
				if (stLineTokenizer. nextToken (&pValue, "\n") != errNoError)
					strcpy (psaSDPRtpMapAttribute -> _pEncodingParameters, "");
				else
				{
					if (strlen (pValue) >= SDP_ENCODINGPARAMETESLENGTH)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FIELDTOOLONG,
							1, pValue);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete psaSDPRtpMapAttribute;

						if (stLineTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					strcpy (psaSDPRtpMapAttribute -> _pEncodingParameters,
						pValue);
				}

				psaSDPAttribute		= psaSDPRtpMapAttribute;
			}
			else if (!strcmp (pValue, SDP_FMTPATTRIBUTENAME))
			{
				SDPFmtpAttribute_p			psaSDPFmtpAttribute;


				// a=fmtp:<format> <format specific parameters>
				if ((psaSDPFmtpAttribute = new SDPFmtpAttribute_t) ==
					(SDPFmtpAttribute_p) NULL)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_NEW_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (strlen (pValue) >= SDP_ATTRIBUTENAMELENGTH)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FIELDTOOLONG,
						1, pValue);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				strcpy (psaSDPFmtpAttribute -> _pName, pValue);

				// format
				if (stLineTokenizer. nextToken (&pValue, " ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (strlen (pValue) >= SDP_FORMATLENGTH)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FIELDTOOLONG,
						1, pValue);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				strcpy (psaSDPFmtpAttribute -> _pFormat, pValue);

				// format specific parameters
				if (stLineTokenizer. nextToken (&pValue, "\n") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (strlen (pValue) >= SDP_FORMATPARAMETERSLENGTH)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FIELDTOOLONG,
						1, pValue);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				strcpy (psaSDPFmtpAttribute -> _pFormatParameters, pValue);

				psaSDPAttribute		= psaSDPFmtpAttribute;
			}
			else
			{
				if (createUnknownSDPAttribute (&stLineTokenizer,
					pValue, &psaSDPAttribute) != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_CREATEUNKNOWNSDPATTRIBUTE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			if (sasState == SDP_SESSIONLEVEL)
			{
				if (addAttribute (psaSDPAttribute) != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_ADDATTRIBUTE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPAttribute;

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				if (psmCurrentSDPMedia -> addAttribute (psaSDPAttribute) !=
					errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPMEDIA_ADDATTRIBUTE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPAttribute;

					if (stLineTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
		}
		else if (pType [0] == SDP_MEDIA)
		{
			SDPMedia:: SDPMediaType_t			mtMediaType;


			// m=<media> <port> <transport> <fmt list>
			if (sasState != SDP_SESSIONLEVEL &&
				sasState != SDP_MEDIALEVEL)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_WRONGSTATEFORTHISLINE,
					1, pPointerToSDPLine);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stLineTokenizer. nextToken (&pValue, " ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (!strcmp (pValue, SDP_MEDIA_VIDEO))
				mtMediaType			= SDPMedia:: SDPMEDIA_VIDEO;
			else if (!strcmp (pValue, SDP_MEDIA_AUDIO))
				mtMediaType			= SDPMedia:: SDPMEDIA_AUDIO;
			else if (!strcmp (pValue, SDP_MEDIA_APPLICATION))
				mtMediaType			= SDPMedia:: SDPMEDIA_APPLICATION;
			else if (!strcmp (pValue, SDP_MEDIA_DATA))
				mtMediaType			= SDPMedia:: SDPMEDIA_DATA;
			else if (!strcmp (pValue, SDP_MEDIA_CONTROL))
				mtMediaType			= SDPMedia:: SDPMEDIA_CONTROL;
			else
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_WRONGMEDIATYPE,
					1, pValue);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (buildSDPMedia (&psmCurrentSDPMedia) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_BUILDSDPMEDIA_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (psmCurrentSDPMedia -> init (mtMediaType, _ptTracer) !=
				errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psmCurrentSDPMedia;

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (addSDPMedia (psmCurrentSDPMedia) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_ADDSDPMEDIA_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sasState			= SDP_MEDIALEVEL;

			if (stLineTokenizer. nextToken (&pValue, " ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (psmCurrentSDPMedia -> setPort (atol (pValue)) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETPORT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stLineTokenizer. nextToken (&pValue, " ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (psmCurrentSDPMedia -> setTransport (pValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETTRANSPORT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stLineTokenizer. nextToken (&pValue, "\n") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (psmCurrentSDPMedia -> setFmtList (pValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETFMTLIST_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SDPTYPENOTHANDLED,
				1, pPointerToSDPLine);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (stLineTokenizer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		pPointerToSDPLine		= strchr (pPointerToSDPLine, '\n') + 1;
	}


	return errNoError;
}


Error SDP:: init (
	unsigned long ulVersion,

	const char *pOriginUserName,
	const char *pOriginSessionId,
	const char *pOriginVersion,
	const char *pOriginNetworkType,
	const char *pOriginAddressType,
	const char *pOriginAddress,

	const char *pSessionName,
	const char *pInformation,
	const char *pURI,
	const char *pEmailAddress,
	const char *pPhoneNumber,

	const char *pConnectionNetworkType,
	const char *pConnectionAddressType,
	const char *pConnectionAddress,
	const char *pBandwidthModifier,
	unsigned long ulBandwidthValueInKbps,

	unsigned long ulTimesStart,
	unsigned long ulTimesStop,
	Tracer_p ptTracer)

{

	_ptTracer				= ptTracer;


	if (setVersion (ulVersion) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETVERSION_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setOriginUserName (pOriginUserName) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETORIGINUSERNAME_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setOriginSessionId (pOriginSessionId) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETORIGINSESSIONID_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setOriginVersion (pOriginVersion) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETORIGINVERSION_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setOriginNetworkType (pOriginNetworkType) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETORIGINNETWORKTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setOriginAddressType (pOriginAddressType) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETORIGINADDRESSTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setOriginAddress (pOriginAddress) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETORIGINADDRESS_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setSessionName (pSessionName) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETSESSIONNAME_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setInformation (pInformation) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETINFORMATION_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setURI (pURI) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETURI_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setEmailAddress (pEmailAddress) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETEMAILADDRESS_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setPhoneNumber (pPhoneNumber) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETPHONENUMBER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setConnectionNetworkType (pConnectionNetworkType) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETCONNECTIONNETWORKTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setConnectionAddressType (pConnectionAddressType) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETCONNECTIONADDRESSTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setConnectionAddress (pConnectionAddress) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETCONNECTIONADDRESS_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setBandwidthModifier (pBandwidthModifier) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETBANDWIDTHMODIFIER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setBandwidthValueInKbps (ulBandwidthValueInKbps) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETBANDWIDTHVALUEINKBPS_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setTimesStart (ulTimesStart) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETTIMESSTART_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (setTimesStop (ulTimesStop) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SETTIMESSTOP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error SDP:: finish (void)

{

	if (_vSDPMedia. size () > 0)
	{
		std:: vector<SDPMedia_p>:: const_iterator    it;
		SDPMedia_p				psmSDPMedia;

		for (it = _vSDPMedia. begin (); it != _vSDPMedia. end ();
			++it)
		{
			psmSDPMedia			= *it;

			if (psmSDPMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete psmSDPMedia;
			psmSDPMedia			= (SDPMedia_p) NULL;
		}

		_vSDPMedia. clear ();
	}

	if (_vSDPAttributes. size () > 0)
	{
		std:: vector<SDPAttribute_p>:: const_iterator    it;
		SDPAttribute_p				psaSDPAttribute;

		for (it = _vSDPAttributes. begin (); it != _vSDPAttributes. end ();
			++it)
		{
			psaSDPAttribute			= *it;

			delete psaSDPAttribute;
			psaSDPAttribute			= (SDPAttribute_p) NULL;
		}

		_vSDPAttributes. clear ();
	}


	return errNoError;
}


Error SDP:: setVersion (unsigned long ulVersion)

{

	_ulVersion				= ulVersion;


	return errNoError;
}


Error SDP:: setOriginUserName (const char *pOriginUserName)

{

	if (strlen (pOriginUserName) >= SDP_MAXORIGINUSERNAMELENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pOriginUserName);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pOriginUserName, pOriginUserName);


	return errNoError;
}


Error SDP:: setOriginSessionId (const char *pOriginSessionId)

{

	if (strlen (pOriginSessionId) >= SDP_MAXORIGINSESSIONIDLENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pOriginSessionId);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pOriginSessionId, pOriginSessionId);


	return errNoError;
}


Error SDP:: setOriginVersion (const char *pOriginVersion)

{

	if (strlen (pOriginVersion) >= SDP_MAXORIGINVERSIONLENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pOriginVersion);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pOriginVersion, pOriginVersion);


	return errNoError;
}


Error SDP:: setOriginNetworkType (const char *pOriginNetworkType)

{

	if (strlen (pOriginNetworkType) >= SDP_MAXORIGINNETWORKTYPELENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pOriginNetworkType);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pOriginNetworkType, pOriginNetworkType);


	return errNoError;
}


Error SDP:: setOriginAddressType (const char *pOriginAddressType)

{

	if (strlen (pOriginAddressType) >= SDP_MAXORIGINADDRESSTYPELENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pOriginAddressType);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pOriginAddressType, pOriginAddressType);


	return errNoError;
}


Error SDP:: setOriginAddress (const char *pOriginAddress)

{

	if (strlen (pOriginAddress) >= SDP_MAXORIGINADDRESSLENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pOriginAddress);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pOriginAddress, pOriginAddress);


	return errNoError;
}


Error SDP:: setSessionName (const char *pSessionName)

{

	if (strlen (pSessionName) >= SDP_MAXSESSIONNAMELENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pSessionName);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pSessionName, pSessionName);


	return errNoError;
}


Error SDP:: setInformation (const char *pInformation)

{

	if (strlen (pInformation) >= SDP_MAXINFORMATIONLENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pInformation);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pInformation, pInformation);


	return errNoError;
}


Error SDP:: setURI (const char *pURI)

{

	if (strlen (pURI) >= SDP_MAXURILENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pURI);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pURI, pURI);


	return errNoError;
}


Error SDP:: setEmailAddress (const char *pEmailAddress)

{

	if (strlen (pEmailAddress) >= SDP_MAXEMAILADDRESSLENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pEmailAddress);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pEmailAddress, pEmailAddress);


	return errNoError;
}


Error SDP:: setPhoneNumber (const char *pPhoneNumber)

{

	if (strlen (pPhoneNumber) >= SDP_MAXPHONENUMBERLENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pPhoneNumber);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pPhoneNumber, pPhoneNumber);


	return errNoError;
}


Error SDP:: setConnectionNetworkType (const char *pConnectionNetworkType)

{

	if (strlen (pConnectionNetworkType) >= SDP_MAXCONNECTIONNETWORKTYPELENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pConnectionNetworkType);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pConnectionNetworkType, pConnectionNetworkType);


	return errNoError;
}


Error SDP:: setConnectionAddressType (const char *pConnectionAddressType)

{

	if (strlen (pConnectionAddressType) >= SDP_MAXCONNECTIONADDRESSTYPELENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pConnectionAddressType);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pConnectionAddressType, pConnectionAddressType);


	return errNoError;
}


Error SDP:: setConnectionAddress (const char *pConnectionAddress)

{

	if (strlen (pConnectionAddress) >= SDP_MAXCONNECTIONADDRESSLENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pConnectionAddress);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pConnectionAddress, pConnectionAddress);


	return errNoError;
}


Error SDP:: setBandwidthModifier (const char *pBandwidthModifier)

{

	if (strlen (pBandwidthModifier) >= SDP_MAXBANDWIDTHMODIFIERLENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pBandwidthModifier);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pBandwidthModifier, pBandwidthModifier);


	return errNoError;
}


Error SDP:: setBandwidthValueInKbps (unsigned long ulBandwidthValueInKbps)

{

	_ulBandwidthValueInKbps		= ulBandwidthValueInKbps;


	return errNoError;
}


Error SDP:: setTimesStart (unsigned long ulTimesStart)

{

	_ulTimesStart		= ulTimesStart;


	return errNoError;
}


Error SDP:: setTimesStop (unsigned long ulTimesStop)

{

	_ulTimesStop		= ulTimesStop;


	return errNoError;
}


Error SDP:: createUnknownSDPAttribute (StringTokenizer_p pstLineTokenizer,
	const char *pAttributeName, SDPAttribute_p *psaSDPAttribute)

{
	const char				*pValue;
	SDPAttribute_p			psaLocalSDPAttribute;


	if ((psaLocalSDPAttribute = new SDPAttribute_t) ==
		(SDPAttribute_p) NULL)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (strlen (pAttributeName) >= SDP_ATTRIBUTENAMELENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FIELDTOOLONG,
			1, pAttributeName);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete psaLocalSDPAttribute;

		return err;
	}

	strcpy (psaLocalSDPAttribute -> _pName, pAttributeName);

	if (pstLineTokenizer -> nextToken (&pValue, "\n") != errNoError)
		strcpy (psaLocalSDPAttribute -> _pValue, "");
	else
	{
		if (strlen (pValue) >= SDP_ATTRIBUTEVALUELENGTH)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_FIELDTOOLONG,
				1, pValue);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete psaLocalSDPAttribute;

			return err;
		}

		strcpy (psaLocalSDPAttribute -> _pValue, pValue);
	}

	*psaSDPAttribute			= psaLocalSDPAttribute;


	return errNoError;
}


Error SDP:: getSDPMedia (
	SDPMedia:: SDPMediaType_t mtMediaType,
	unsigned long ulMediaIndex,
	SDPMedia_p *psmSDPMedia)

{

	std:: vector<SDPMedia_p>:: const_iterator    it;
	SDPMedia:: SDPMediaType_t		mtLocalMediaType;
	unsigned long					ulLocalMediaIndex;


	ulLocalMediaIndex			= 0;

	for (it = _vSDPMedia. begin (); it != _vSDPMedia. end ();
		++it)
	{
		*psmSDPMedia			= *it;

		if ((*psmSDPMedia) -> getMediaType (&mtLocalMediaType) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_GETMEDIATYPE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (mtLocalMediaType == mtMediaType)
		{
			if (ulLocalMediaIndex == ulMediaIndex)
				break;

			ulLocalMediaIndex++;
		}
	}

	if (it == _vSDPMedia. end () ||
		ulLocalMediaIndex != ulMediaIndex)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_SDPMEDIANOTFOUND);
		// _ptTracer -> trace (Tracer:: TRACER_LERRR,
		// 	(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error SDP:: buildSDPMedia (SDPMedia_p *psmCurrentSDPMedia)

{

	if (((*psmCurrentSDPMedia) = new SDPMedia_t) == (SDPMedia_p) NULL)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error SDP:: addAttribute (SDPAttribute_p psaSDPAttribute)

{

	_vSDPAttributes. insert (_vSDPAttributes. end (),
		psaSDPAttribute);


	return errNoError;
}


Error SDP:: getAttributeValue (
	const char *pAttributeName, char *pAttributeValue)

{

	std:: vector<SDPAttribute_p>:: const_iterator    it;
	SDPAttribute_p				psaSDPAttribute;

	for (it = _vSDPAttributes. begin (); it != _vSDPAttributes. end ();
		++it)
	{
		psaSDPAttribute			= *it;

		if (!strcmp (psaSDPAttribute -> _pName, pAttributeName))
		{
			strcpy (pAttributeValue, psaSDPAttribute -> _pValue);

			break;
		}
	}

	if (it == _vSDPAttributes. end ())
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_ATTRIBUTENOTFOUND,
			1, pAttributeName);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error SDP:: addSDPMedia (SDPMedia_p psmSDPMedia)

{

	_vSDPMedia. insert (_vSDPMedia. end (), psmSDPMedia);


	return errNoError;
}


Error SDP:: appendToBuffer (Buffer_p pbSDP)

{

	if (pbSDP -> append (SDP_VERSION) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbSDP -> append ("=") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbSDP -> append (_ulVersion) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbSDP -> append (SDP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (strcmp (_pOriginUserName, ""))
	{
		if (pbSDP -> append (SDP_ORIGIN) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append ("=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pOriginUserName) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (" ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pOriginSessionId) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (" ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pOriginVersion) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (" ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pOriginNetworkType) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (" ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pOriginAddressType) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (" ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pOriginAddress) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (SDP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (strcmp (_pSessionName, ""))
	{
		if (pbSDP -> append (SDP_SESSIONNAME) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append ("=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pSessionName) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (SDP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (strcmp (_pInformation, ""))
	{
		if (pbSDP -> append (SDP_INFORMATION) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append ("=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pInformation) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (SDP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (strcmp (_pURI, ""))
	{
		if (pbSDP -> append (SDP_URI) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append ("=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pURI) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (SDP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (strcmp (_pEmailAddress, ""))
	{
		if (pbSDP -> append (SDP_EMAILADDRESS) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append ("=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pEmailAddress) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (SDP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (strcmp (_pPhoneNumber, ""))
	{
		if (pbSDP -> append (SDP_PHONENUMBER) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append ("=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pPhoneNumber) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (SDP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (strcmp (_pConnectionNetworkType, ""))
	{
		if (pbSDP -> append (SDP_CONNECTIONDATA) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append ("=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pConnectionNetworkType) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (" ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pConnectionAddressType) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (" ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pConnectionAddress) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (SDP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (strcmp (_pBandwidthModifier, ""))
	{
		if (pbSDP -> append (SDP_BANDWIDTH) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append ("=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_pBandwidthModifier) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (":") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (_ulBandwidthValueInKbps) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbSDP -> append (SDP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbSDP -> append (SDP_TIMES) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbSDP -> append ("=") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbSDP -> append (_ulTimesStart) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbSDP -> append (" ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbSDP -> append (_ulTimesStop) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbSDP -> append (SDP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_vSDPAttributes. size () > 0)
	{
		std:: vector<SDPAttribute_p>:: const_iterator    it;
		SDPAttribute_p				psaSDPAttribute;

		for (it = _vSDPAttributes. begin (); it != _vSDPAttributes. end ();
			++it)
		{
			psaSDPAttribute			= *it;

			if (psaSDPAttribute -> appendToBuffer (pbSDP) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPATTRIBUTE_APPENDTOBUFFER_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
	}

	if (_vSDPMedia. size () > 0)
	{
		std:: vector<SDPMedia_p>:: const_iterator    it;
		SDPMedia_p				psmSDPMedia;

		for (it = _vSDPMedia. begin (); it != _vSDPMedia. end ();
			++it)
		{
			psmSDPMedia			= *it;

			if (psmSDPMedia -> appendToBuffer (pbSDP) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_APPENDTOBUFFER_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
	}


	return errNoError;
}

