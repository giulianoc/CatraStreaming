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

#include "SDPMedia.h"
#include <assert.h>
#include <stdlib.h>



SDPMedia:: SDPMedia (void)

{

}


SDPMedia:: ~SDPMedia (void)

{

}



SDPMedia:: SDPMedia (const SDPMedia &)

{

	assert (1==0);

	// to do

}


SDPMedia &SDPMedia:: operator = (const SDPMedia &)

{

	assert (1==0);

	// to do

	return *this;

}


Error SDPMedia:: init (SDPMediaType_t smtSDPMediaType, Tracer_p ptTracer)

{

	_smtSDPMediaType			= smtSDPMediaType;
	_ptTracer					= ptTracer;

	_ulPort						= 0;
	strcpy (_pTransport, "");
	strcpy (_pFmtList, "");
	strcpy (_pInformation, "");
	strcpy (_pConnectionNetworkType, "");
	strcpy (_pConnectionAddressType, "");
	strcpy (_pConnectionAddress, "");
	strcpy (_pBandwidthModifier, "");
	_ulBandwidthValueInKbps		= 0;

	_vSDPAttributes. clear ();


	_psaSDPRtpMapAttribute		= (SDPRtpMapAttribute_p) NULL;
	_psaSDPFmtpAttribute		= (SDPFmtpAttribute_p) NULL;


	return errNoError;
}


Error SDPMedia:: init (Buffer_p pbSDPMedia, Tracer_p ptTracer)

{

	StringTokenizer_t				stLineTokenizer;
	const char						*pType;
	const char						*pValue;
	const char						*pPointerToSDPMediaLine;
	Boolean_t						bSDPFinished;



	if (pbSDPMedia == (Buffer_p) NULL ||
		ptTracer == (Tracer_p) NULL)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_ACTIVATIONWRONG);

		return err;
	}

	if (SDPMedia:: init (SDPMEDIA_UNKNOWN, ptTracer) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPMEDIA_INIT_FAILED);

		return err;
	}

	pPointerToSDPMediaLine		= (const char *) (*pbSDPMedia);

	bSDPFinished				= false;

	while (!bSDPFinished)
	{
		if (*pPointerToSDPMediaLine == '\n' ||	// means "\n\n"
			strchr (pPointerToSDPMediaLine, '\n') == (char *) NULL)
		{
			bSDPFinished				= true;

			continue;
		}

		if (stLineTokenizer. init (pPointerToSDPMediaLine,
			(long) (strchr (pPointerToSDPMediaLine, '\n') -
			pPointerToSDPMediaLine), "=") != errNoError)
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
		if (pType [0] == SDP_INFORMATION)
		{
			// i=<session description>
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

			if (setInformation (pValue) != errNoError)
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
		else if (pType [0] == SDP_CONNECTIONDATA)
		{
			// c=<network type> <address type> <connection address>
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

			if (setConnectionNetworkType (pValue) != errNoError)
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

			if (setConnectionAddressType (pValue) != errNoError)
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

			if (setConnectionAddress (pValue) != errNoError)
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
		else if (pType [0] == SDP_BANDWIDTH)
		{
			// b=<modifier>:<bandwidth-value>
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

			if (setBandwidthModifier (pValue) != errNoError)
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

			if (setBandwidthValueInKbps (atol (pValue)) != errNoError)
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
		else if (pType [0] == SDP_ATTRIBUTE)
		{
			// a=<attribute>
			// a=<attribute>:<value>

			SDPAttribute_p			psaSDPAttribute;


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

				psaSDPAttribute				= psaSDPRtpMapAttribute;
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

				psaSDPAttribute			= psaSDPFmtpAttribute;
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

			if (addAttribute (psaSDPAttribute) != errNoError)
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
		else if (pType [0] == SDP_MEDIA)
		{
			// m=<media> <port> <transport> <fmt list>
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
				_smtSDPMediaType			= SDPMedia:: SDPMEDIA_VIDEO;
			else if (!strcmp (pValue, SDP_MEDIA_AUDIO))
				_smtSDPMediaType			= SDPMedia:: SDPMEDIA_AUDIO;
			else if (!strcmp (pValue, SDP_MEDIA_APPLICATION))
				_smtSDPMediaType			= SDPMedia:: SDPMEDIA_APPLICATION;
			else if (!strcmp (pValue, SDP_MEDIA_DATA))
				_smtSDPMediaType			= SDPMedia:: SDPMEDIA_DATA;
			else if (!strcmp (pValue, SDP_MEDIA_CONTROL))
				_smtSDPMediaType			= SDPMedia:: SDPMEDIA_CONTROL;
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

			if (setPort (atol (pValue)) != errNoError)
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

			if (setTransport (pValue) != errNoError)
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

			if (setFmtList (pValue) != errNoError)
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
				1, pPointerToSDPMediaLine);
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

		pPointerToSDPMediaLine		= strchr (pPointerToSDPMediaLine, '\n') + 1;
	}


	return errNoError;
}


Error SDPMedia:: finish (void)

{

	if (_vSDPAttributes. size () > 0)
	{
		std:: vector<SDPAttribute_p>:: const_iterator    it;
		SDPAttribute_p				psaSDPAttribute;

		for (it = _vSDPAttributes. begin (); it != _vSDPAttributes. end ();
			++it)
		{
			psaSDPAttribute            = *it;

			delete psaSDPAttribute;
			psaSDPAttribute			= (SDPAttribute_p) NULL;
		}

		_vSDPAttributes. clear ();
	}


	return errNoError;
}


Error SDPMedia:: getMediaType (
	SDPMediaType_p pmtMediaType)

{

	*pmtMediaType			= _smtSDPMediaType;


	return errNoError;
}


Error SDPMedia:: setMediaType (
	SDPMediaType_t mtMediaType)

{

	_smtSDPMediaType			= mtMediaType;


	return errNoError;
}


Error SDPMedia:: addAttribute (SDPAttribute_p psaSDPAttribute)

{

	_vSDPAttributes. insert (_vSDPAttributes. end (),
		psaSDPAttribute);

	if (!strcmp (psaSDPAttribute -> _pName, SDP_RTPMAPATTRIBUTENAME))
		_psaSDPRtpMapAttribute		= (SDPRtpMapAttribute_p) psaSDPAttribute;
	else if (!strcmp (psaSDPAttribute -> _pName, SDP_FMTPATTRIBUTENAME))
		_psaSDPFmtpAttribute		= (SDPFmtpAttribute_p) psaSDPAttribute;


	return errNoError;
}


Error SDPMedia:: setPort (unsigned long ulPort)

{

	_ulPort			= ulPort;


	return errNoError;
}


Error SDPMedia:: getPort (unsigned long *pulPort)

{

	*pulPort			= _ulPort;


	return errNoError;
}


Error SDPMedia:: setTransport (const char *pTransport)

{

	if (strlen (pTransport) >= SDP_MAXTRANSPORTLENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPMEDIA_TRANSPORTTOOLONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pTransport, pTransport);


	return errNoError;
}


Error SDPMedia:: getTransport (char *pTransport)

{

	strcpy (pTransport, _pTransport);


	return errNoError;
}


Error SDPMedia:: setFmtList (const char *pFmtList)

{

	if (strlen (pFmtList) >= SDP_MAXFMTLISTLENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPMEDIA_FMTLISTTOOLONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pFmtList, pFmtList);


	return errNoError;
}


Error SDPMedia:: getFmtList (char *pFmtList)

{

	strcpy (pFmtList, _pFmtList);


	return errNoError;
}


Error SDPMedia:: setInformation (const char *pInformation)

{

	if (strlen (pInformation) >= SDP_MAXINFORMATIONLENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPMEDIA_INFORMATIONTOOLONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pInformation, pInformation);


	return errNoError;
}


Error SDPMedia:: getInformation (char *pInformation)

{

	strcpy (pInformation, _pInformation);


	return errNoError;
}


Error SDPMedia:: setConnectionNetworkType (const char *pConnectionNetworkType)

{

	if (strlen (pConnectionNetworkType) >= SDP_MAXCONNECTIONNETWORKTYPELENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPMEDIA_CONNECTIONNETWORKTYPETOOLONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pConnectionNetworkType, pConnectionNetworkType);


	return errNoError;
}


Error SDPMedia:: getConnectionNetworkType (char *pConnectionNetworkType)

{

	strcpy (pConnectionNetworkType, _pConnectionNetworkType);


	return errNoError;
}


Error SDPMedia:: setConnectionAddressType (const char *pConnectionAddressType)

{

	if (strlen (pConnectionAddressType) >= SDP_MAXCONNECTIONADDRESSTYPELENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPMEDIA_CONNECTIONADDRESSTYPETOOLONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pConnectionAddressType, pConnectionAddressType);


	return errNoError;
}


Error SDPMedia:: getConnectionAddressType (char *pConnectionAddressType)

{

	strcpy (pConnectionAddressType, _pConnectionAddressType);


	return errNoError;
}


Error SDPMedia:: setConnectionAddress (const char *pConnectionAddress)

{

	if (strlen (pConnectionAddress) >= SDP_MAXCONNECTIONADDRESSLENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPMEDIA_CONNECTIONADDRESSTOOLONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pConnectionAddress, pConnectionAddress);


	return errNoError;
}


Error SDPMedia:: getConnectionAddress (char *pConnectionAddress)

{

	strcpy (pConnectionAddress, _pConnectionAddress);


	return errNoError;
}


Error SDPMedia:: setBandwidthModifier (const char *pBandwidthModifier)

{

	if (strlen (pBandwidthModifier) >= SDP_MAXBANDWIDTHMODIFIERLENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPMEDIA_BANDWIDTHMODIFIERTOOLONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (_pBandwidthModifier, pBandwidthModifier);


	return errNoError;
}


Error SDPMedia:: getBandwidthModifier (char *pBandwidthModifier)

{

	strcpy (pBandwidthModifier, _pBandwidthModifier);


	return errNoError;
}


Error SDPMedia:: setBandwidthValueInKbps (unsigned long ulBandwidthValueInKbps)

{

	_ulBandwidthValueInKbps			= ulBandwidthValueInKbps;


	return errNoError;
}


Error SDPMedia:: getBandwidthValueInKbps (
	unsigned long *pulBandwidthValueInKbps)

{

	*pulBandwidthValueInKbps			= _ulBandwidthValueInKbps;


	return errNoError;
}


Error SDPMedia:: getMediaInfo (
	SDPMediaType_p pmtMediaType,
	unsigned long *pulMediaPayloadType,
	unsigned long *pulMediaPort,
	char *pMediaEncodingName,
	char *pMediaClockRate,
	char *pMediaEncodingParameters,
	unsigned long *pulMediaBandwidthInKbps)

{

	std:: vector<SDPAttribute_p>:: const_iterator    it;
	SDPAttribute_p						psaSDPAttribute;
	SDPRtpMapAttribute_p				psaSDPRtpMapAttribute;


	if (pmtMediaType != (SDPMediaType_p) NULL)
		*pmtMediaType			= _smtSDPMediaType;

	if (pulMediaPayloadType != (unsigned long *) NULL ||
		pMediaEncodingName != (char *) NULL)
	{
		for (it = _vSDPAttributes. begin (); it != _vSDPAttributes. end ();
			++it)
		{
			psaSDPAttribute            = *it;

			if (!strcmp (psaSDPAttribute -> _pName, SDP_RTPMAPATTRIBUTENAME))
				break;
		}

		if (it == _vSDPAttributes. end ())
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_ATTRIBUTENOTFOUND,
				1, SDP_RTPMAPATTRIBUTENAME);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		psaSDPRtpMapAttribute		= (SDPRtpMapAttribute_p) psaSDPAttribute;

		if (pulMediaPayloadType != (unsigned long *) NULL)
		{
			*pulMediaPayloadType			=
				atol (psaSDPRtpMapAttribute -> _pPayloadType);
		}

		if (pMediaEncodingName != (char *) NULL)
		{
			strcpy (pMediaEncodingName,
				psaSDPRtpMapAttribute -> _pEncodingName);
		}

		if (pMediaClockRate != (char *) NULL)
		{
			strcpy (pMediaClockRate,
				psaSDPRtpMapAttribute -> _pClockRate);
		}

		if (pMediaEncodingParameters != (char *) NULL)
		{
			strcpy (pMediaEncodingParameters,
				psaSDPRtpMapAttribute -> _pEncodingParameters);
		}
	}

	if (pulMediaPort != (unsigned long *) NULL)
		*pulMediaPort					= _ulPort;


	if (pulMediaBandwidthInKbps != (unsigned long *) NULL)
		*pulMediaBandwidthInKbps		= _ulBandwidthValueInKbps;


	return errNoError;
}


Error SDPMedia:: createUnknownSDPAttribute (StringTokenizer_p pstLineTokenizer,
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


Error SDPMedia:: appendToBuffer (Buffer_p pbSDP)

{

	if (pbSDP -> append (SDP_MEDIA) != errNoError)
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

	if (_smtSDPMediaType == SDPMEDIA_VIDEO)
	{
		if (pbSDP -> append (SDP_MEDIA_VIDEO) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else if (_smtSDPMediaType == SDPMEDIA_AUDIO)
	{
		if (pbSDP -> append (SDP_MEDIA_AUDIO) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else if (_smtSDPMediaType == SDPMEDIA_APPLICATION)
	{
		if (pbSDP -> append (SDP_MEDIA_APPLICATION) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else if (_smtSDPMediaType == SDPMEDIA_DATA)
	{
		if (pbSDP -> append (SDP_MEDIA_DATA) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else if (_smtSDPMediaType == SDPMEDIA_CONTROL)
	{
		if (pbSDP -> append (SDP_MEDIA_CONTROL) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_WRONGMEDIATYPE, 1, "");
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

	if (pbSDP -> append (_ulPort) != errNoError)
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

	if (pbSDP -> append (_pTransport) != errNoError)
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

	if (pbSDP -> append (_pFmtList) != errNoError)
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


	return errNoError;
}


