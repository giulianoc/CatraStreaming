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


#include "RTSPUtility.h"
#include "Convert.h"
#include "DateTime.h"
#include "MP4InitialObjectDescr.h"
#include "RTPUtility.h"
#include "SDPForRTSP.h"
#include "StringTokenizer.h"
#include "WebUtility.h"
#include "aac.h"
#include "mpeg4.h"
#include "rfc3016.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#ifdef WIN32
	#include <windows.h>
#else
// 	#include <sys/time.h>
	#include <netinet/in.h>
#endif


RTSPUtility:: RTSPUtility (void)

{

}


RTSPUtility:: ~RTSPUtility (void)

{

}



RTSPUtility:: RTSPUtility (const RTSPUtility &)

{

	assert (1==0);

	// to do

}


RTSPUtility &RTSPUtility:: operator = (const RTSPUtility &)

{

	assert (1==0);

	// to do

	return *this;

}


Error RTSPUtility:: getDESCRIBERequest (
	const char *pURL,
	unsigned long ulSequenceNumber,
	const char *pUserAgent,
	Buffer_p pbRTSPRequest,
	Tracer_p ptTracer)

{

	if (pbRTSPRequest -> setBuffer ("DESCRIBE ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (pURL) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (" " RTSP_PREDEFINEDRTSPVERSION RTSP_NEWLINE) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append ("CSeq: ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (ulSequenceNumber) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (RTSP_NEWLINE "Accept: application/sdp"
		RTSP_NEWLINE "User-Agent: ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (pUserAgent) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (RTSP_NEWLINE RTSP_NEWLINE) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPUtility:: parseDESCRIBERequest (
	const char *pRTSPRequest,

	char *pRTSPServerIPAddress,
	unsigned long ulIPAddressBufferLength,
	long *plRTSPServerPort,
	Buffer_p pbRelativePathWithoutParameters,
	Buffer_p pbURLParameters,
	Buffer_p pbRTSPVersion,
	Buffer_p pbUserAgent,
	long *plSequenceNumber,
	char *pTimestamp,
	unsigned long *pulRequestedPayloadSizeInBytes,
	unsigned long *pulRequestLength,
	Tracer_p ptTracer)

{

	const char						*pPointerToRTSPRequest;
	StringTokenizer_t				stSpaceTokenizer;
	const char						*pToken;
	Buffer_t						bURL;
	Boolean_t						bRTSPVersionRead;
	Boolean_t						bAcceptSdp;
	Boolean_t						bRequestFinished;


	strcpy (pRTSPServerIPAddress, "");
	*plRTSPServerPort				= -1;
	*plSequenceNumber				= -1;
	strcpy (pTimestamp, "");
	*pulRequestedPayloadSizeInBytes	= 0;
	*pulRequestLength				= 0;

	if (pbRelativePathWithoutParameters -> setBuffer ("") != errNoError ||
		pbURLParameters -> setBuffer ("") != errNoError ||
		pbUserAgent -> setBuffer ("") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPVersion != (Buffer_p) NULL)
	{
		if (pbRTSPVersion -> setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (bURL. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	bRTSPVersionRead				= false;
	bAcceptSdp						= false;
	bRequestFinished				= false;

	pPointerToRTSPRequest		= pRTSPRequest;

	while (!bRequestFinished)
	{
		if (*pPointerToRTSPRequest == '\r' ||	// means "\r\n\r\n"
			strchr (pPointerToRTSPRequest, '\r') == (char *) NULL)
		{
			bRequestFinished				= true;

			*pulRequestLength			= (unsigned long)
				(pPointerToRTSPRequest - pRTSPRequest + 2);

			continue;
		}

		if (stSpaceTokenizer. init (pPointerToRTSPRequest,
			(long) (strchr (pPointerToRTSPRequest, '\r') -
			pPointerToRTSPRequest), " ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bURL. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		pPointerToRTSPRequest		= strchr (pPointerToRTSPRequest, '\n') + 1;

		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bURL. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!strcmp (pToken, "DESCRIBE"))
		{
			const char					*pRTSPVersion;


			// 'DESCRIBE <Request-URI (rtsp://...)> <Request-Paramters> <RTSP-Version>'
			// Request-URI
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if ((pRTSPVersion = strrchr (pToken, ' ')) == (const char *) NULL)
			{
				Error err = RTSPErrors (__FILE__, __LINE__,
					RTSP_RTSPUTILITY_DESCRIBEREQUESTWRONG,
					1, pToken);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bURL. setBuffer (pToken,
				(long) (pRTSPVersion - pToken)) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SETBUFFER_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// <RTSP-Version>
			if (pbRTSPVersion != (Buffer_p) NULL)
			{
				if (pbRTSPVersion -> setBuffer (pRTSPVersion + 1) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SETBUFFER_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bURL. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
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
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*plSequenceNumber				= atol (pToken);
		}
		else if (!strcmp (pToken, "Timestamp:"))
		{
			// 'Timestamp: <Timestamp>'
			// Timestamp
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
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
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*pulRequestedPayloadSizeInBytes				=
				atol (pToken);
		}
		else if (!strcmp (pToken, "Accept:"))
		{
			// 'Accept: <DescriptionContentType>, ..., <DescriptionContentType>'
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (strstr (pToken, "application/sdp"))
				bAcceptSdp				= true;
		}
		else if (!strcmp (pToken, "User-Agent:"))
		{
			// 'User-Agent: ...'
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbUserAgent -> setBuffer (pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SETBUFFER_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			;
		}

		if (stSpaceTokenizer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bURL. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (WebUtility:: parseURL ((const char *) bURL,
		pRTSPServerIPAddress, ulIPAddressBufferLength, plRTSPServerPort,
		pbRelativePathWithoutParameters,
		pbURLParameters) != errNoError)
	{
		Error err = WebToolsErrors (__FILE__, __LINE__,
			WEBTOOLS_WEBUTILITY_PARSEURL_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bURL. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bURL. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// check for DESCRIBE request
	{
		if (!bRTSPVersionRead || *plSequenceNumber == -1 ||
			bAcceptSdp == false ||
			!strcmp ((const char *) (*pbRelativePathWithoutParameters), ""))
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_DESCRIBEREQUESTWRONG,
				1, pRTSPRequest);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


/*
Error RTSPUtility:: getSDPFromMediaInfo (
	MP4File_p pmfFile,
	const char *pOriginAddress,
	MP4TrackInfo_p pmtiHintOrMediaVideoTrackInfo,
	MP4Atom:: MP4Codec_t cVideoCodecUsed,
	unsigned long ulVideoAvgBitRate,
	char *pVideoTrackName,
	MP4TrackInfo_p pmtiHintOrMediaAudioTrackInfo,
	MP4Atom:: MP4Codec_t cAudioCodecUsed,
	unsigned long ulAudioAvgBitRate,
	char *pAudioTrackName,
	MP4Atom:: Standard_t sStandard,
	Buffer_p pbSDP,
	unsigned long *pulVideoPayloadNumber,
	unsigned long *pulAudioPayloadNumber,
	Tracer_p ptTracer)

{

	Buffer_t				bLocalSDP;
	SDPFor3GPP_t			sSDPFor3GPP;
	unsigned long			pulFreePayloadsNumber [2];
	char					pVideoPayloadNumber [RTSP_MAXLONGLENGTH];
	char					pAudioPayloadNumber [RTSP_MAXLONGLENGTH];


	if (bLocalSDP. init ("") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// Global SDP
	{
		MP4RtpAtom_p				pmraRtpAtom;
		MP4Atom_p					pmaAtom;


		// Verify if this movie has a global SDP atom
		// the s SDP field should be included here
		if (pmfFile -> searchAtom ("moov:0:udta:0:hnti:0:rtp :0",
			false, &pmaAtom) != errNoError)
		{
			// global SDP not found
			;
		}
		else
		{
			pmraRtpAtom			= (MP4RtpAtom_p) pmaAtom;

			if (pmraRtpAtom -> getDescriptionFormat (&bLocalSDP) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (!strcmp ((const char *) bLocalSDP, "sdp "))
			{
				if (pmraRtpAtom -> getSdpText (&bLocalSDP) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				// add the mandatory version (v=0) to the SDP if it does not
				// exist otherwise the SDPFor3GPP class returns an error
				if (strstr ((const char *) bLocalSDP, "v=") == (char *) NULL)
				{
					if (bLocalSDP. insertAt (0, "v=0" SDP_NEWLINE) !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_INSERTAT_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bLocalSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
			}
		}
	}

	if (sSDPFor3GPP. init (
		&bLocalSDP, ptTracer) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bLocalSDP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (strstr ((const char *) bLocalSDP, "v=") == (char *) NULL)
	{
		if (sSDPFor3GPP. setVersion (0) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETVERSION_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (strstr ((const char *) bLocalSDP, "o=") == (char *) NULL)
	{
		time_t				tNow;
		char				pNow [RTSP_MAXLONGLENGTH];
		

		tNow			= time (NULL);
		sprintf (pNow, "%lu", (unsigned long) tNow);

		if (sSDPFor3GPP. setOriginUserName ("catraserver") != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETORIGINUSERNAME_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (sSDPFor3GPP. setOriginSessionId (pNow) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETORIGINSESSIONID_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (sSDPFor3GPP. setOriginVersion (pNow) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETORIGINVERSION_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (sSDPFor3GPP. setOriginNetworkType ("IN") != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETORIGINNETWORKTYPE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (sSDPFor3GPP. setOriginAddressType ("IP4") != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETORIGINADDRESSTYPE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (sSDPFor3GPP. setOriginAddress (pOriginAddress) !=
			errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETORIGINADDRESS_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (strstr ((const char *) bLocalSDP, "s=") == (char *) NULL)
	{
		const char				*pSessionName;


		#ifdef WIN32
			pSessionName		= strrchr ((const char *) (*pmfFile), '\\');
		#else
			pSessionName		= strrchr ((const char *) (*pmfFile), '/');
		#endif

		if (pSessionName == (char *) NULL)
			pSessionName			= (const char *) (*pmfFile);
		else
			pSessionName			+= 1;

		if (sSDPFor3GPP. setSessionName (pSessionName) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETSESSIONNAME_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (strstr ((const char *) bLocalSDP, "c=") == (char *) NULL)
	{
		if (sSDPFor3GPP. setConnectionNetworkType ("IN") != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETCONNECTIONNETWORKTYPE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (sSDPFor3GPP. setConnectionAddressType ("IP4") != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETCONNECTIONADDRESSTYPE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (sSDPFor3GPP. setConnectionAddress ("0.0.0.0") != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETCONNECTIONADDRESS_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// t=0 0 MP4F_NEWLINE		is by default in sSDPFor3GPP

	if (strstr ((const char *) bLocalSDP, "b=") == (char *) NULL)
	{
		unsigned long			ulAvgBitRateInKbps;


		if (sSDPFor3GPP. setBandwidthModifier ("AS") != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETBANDWIDTHMODIFIER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		ulAvgBitRateInKbps			= 0;

		if (pmtiHintOrMediaVideoTrackInfo != (MP4TrackInfo_p) NULL)
			ulAvgBitRateInKbps			+= (ulVideoAvgBitRate / 1000);

		if (pmtiHintOrMediaAudioTrackInfo != (MP4TrackInfo_p) NULL)
			ulAvgBitRateInKbps			+= (ulAudioAvgBitRate / 1000);

		if (sSDPFor3GPP. setBandwidthValueInKbps (ulAvgBitRateInKbps) !=
			errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETBANDWIDTHVALUEINKBPS_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (strstr ((const char *) bLocalSDP, "a=control:") == (char *) NULL)
	{
		SDPAttribute_p			psaSDPAttribute;


		if ((psaSDPAttribute = new SDPAttribute_t) ==
			(SDPAttribute_p) NULL)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		strcpy (psaSDPAttribute -> _pName, "control");

		strcpy (psaSDPAttribute -> _pValue, "*");

		if (sSDPFor3GPP. addAttribute (psaSDPAttribute) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETCONNECTIONADDRESS_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete psaSDPAttribute;

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (strstr ((const char *) bLocalSDP, "a=range") == (char *) NULL)
	{
		double 						dMovieDuration;
		SDPAttribute_p				psaSDPRangeAttribute;


		if (pmfFile -> getMovieDuration (
			pmtiHintOrMediaVideoTrackInfo,
			&dMovieDuration) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_GETMOVIEDURATION_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if ((psaSDPRangeAttribute = new SDPAttribute_t) ==
			(SDPAttribute_p) NULL)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		strcpy (psaSDPRangeAttribute -> _pName, "range");
		sprintf (psaSDPRangeAttribute -> _pValue,
			"npt=0-%.3lf", dMovieDuration);

		if (sSDPFor3GPP. addAttribute (psaSDPRangeAttribute) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_SETCONNECTIONADDRESS_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete psaSDPRangeAttribute;

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (pmfFile -> getFreePayloadsNumber (2, pulFreePayloadsNumber) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_GETFREEPAYLOADSNUMBER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sSDPFor3GPP. finish () != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (bLocalSDP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmtiHintOrMediaVideoTrackInfo != (MP4TrackInfo_p) NULL &&
		pmtiHintOrMediaAudioTrackInfo != (MP4TrackInfo_p) NULL)
	{
		sprintf (pVideoPayloadNumber, "%lu", pulFreePayloadsNumber [0]);
		*pulVideoPayloadNumber		= pulFreePayloadsNumber [0];
		sprintf (pAudioPayloadNumber, "%lu", pulFreePayloadsNumber [1]);
		*pulAudioPayloadNumber		= pulFreePayloadsNumber [1];
	}
	else if (pmtiHintOrMediaVideoTrackInfo != (MP4TrackInfo_p) NULL)
	{
		sprintf (pVideoPayloadNumber, "%lu", pulFreePayloadsNumber [0]);
		*pulVideoPayloadNumber		= pulFreePayloadsNumber [0];
		*pulAudioPayloadNumber		= 0;
	}
	else
	{
		sprintf (pAudioPayloadNumber, "%lu", pulFreePayloadsNumber [0]);
		*pulVideoPayloadNumber		= 0;
		*pulAudioPayloadNumber		= pulFreePayloadsNumber [1];
	}

	if (pmtiHintOrMediaVideoTrackInfo != (MP4TrackInfo_p) NULL)
	{
		SDPMedia_p				pSDPVideoMedia;


		if (bLocalSDP. setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// video SDP atom
		{
			Error_t					errGetSdpAtom;
			MP4SdpAtom_p				pmaSdpAtom;


			if ((errGetSdpAtom = pmtiHintOrMediaVideoTrackInfo ->
				getSdpAtom (&pmaSdpAtom, true)) != errNoError)
			{
				if ((long) errGetSdpAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETSDPATOM_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				if (pmaSdpAtom -> getSdpText (&bLocalSDP) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4SDPATOM_GETSDPTEXT_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
					
					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
		}

		if ((pSDPVideoMedia = new SDPMediaFor3GPP_t) ==
			(SDPMediaFor3GPP_p) NULL)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pSDPVideoMedia -> init (
			&bLocalSDP, ptTracer) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete pSDPVideoMedia;

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (sSDPFor3GPP. addSDPMedia (pSDPVideoMedia) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_ADDSDPMEDIA_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete pSDPVideoMedia;

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (strstr ((const char *) bLocalSDP, "m=") == (char *) NULL)
		{
			if (pSDPVideoMedia -> setMediaType (
				SDPMedia:: SDPMEDIA_VIDEO) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETMEDIATYPE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pSDPVideoMedia -> setPort (0) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETPORT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pSDPVideoMedia -> setTransport ("RTP/AVP") != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETTRANSPORT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pSDPVideoMedia -> setFmtList (pVideoPayloadNumber) !=
				errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETFMTLIST_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (pSDPVideoMedia -> getFmtList (pVideoPayloadNumber) !=
				errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_GETFMTLIST_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (strstr ((const char *) bLocalSDP, "b=") == (char *) NULL)
		{
			if (pSDPVideoMedia -> setBandwidthModifier (
				"AS") != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETBANDWIDTHMODIFIER_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pSDPVideoMedia -> setBandwidthValueInKbps (
				ulVideoAvgBitRate / 1000) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETBANDWIDTHVALUEINKBPS_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (strstr ((const char *) bLocalSDP, "a=rtpmap:") == (char *) NULL)
		{
			SDPRtpMapAttribute_p		psaSDPRtpMapAttribute;


			if ((psaSDPRtpMapAttribute = new SDPRtpMapAttribute_t) ==
				(SDPRtpMapAttribute_p) NULL)
			{
				Error err = RTSPErrors (__FILE__, __LINE__,
					RTSP_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (psaSDPRtpMapAttribute -> _pName, "rtpmap");

			strcpy (psaSDPRtpMapAttribute -> _pPayloadType,
				pVideoPayloadNumber);

			if (cVideoCodecUsed == MP4Atom:: MP4F_CODEC_MPEG4)
				strcpy (psaSDPRtpMapAttribute -> _pEncodingName, "MP4V-ES");
			else if (cVideoCodecUsed == MP4Atom:: MP4F_CODEC_H263)
				strcpy (psaSDPRtpMapAttribute -> _pEncodingName, "H263-2000");
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_CODECUNKNOWN);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPRtpMapAttribute;

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			{
				MP4TrakAtom_p				pmaMediaVideoTrack;
				MP4MdhdAtom_p				pmaMediaMdhdAtom;
				unsigned long				ulObjectTypeIndication;
				unsigned long				ulRTPTimeScale;


				if (pmtiHintOrMediaVideoTrackInfo ->
					getTrakAtom (&pmaMediaVideoTrack) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPRtpMapAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pmtiHintOrMediaVideoTrackInfo ->
					getMdhdAtom (&pmaMediaMdhdAtom) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETMDHDATOM_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPRtpMapAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (cVideoCodecUsed == MP4Atom:: MP4F_CODEC_MPEG4)
				{
					if (pmtiHintOrMediaVideoTrackInfo ->
						getObjectTypeIndication (&ulObjectTypeIndication) !=
						errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETOBJECTTYPEINDICATION_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete psaSDPRtpMapAttribute;

						if (sSDPFor3GPP. finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bLocalSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
				else
					ulObjectTypeIndication		= 0;

				if (RTPUtility:: getRTPTimeScale (
					pmaMediaVideoTrack,
					pmaMediaMdhdAtom,
					cVideoCodecUsed,
					ulObjectTypeIndication,
					sStandard, &ulRTPTimeScale,
					ptTracer) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPUTILITY_GETRTPTIMESCALE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPRtpMapAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				sprintf (psaSDPRtpMapAttribute -> _pClockRate, "%lu",
					ulRTPTimeScale);
			}

			strcpy (psaSDPRtpMapAttribute -> _pEncodingParameters, "");

			if (pSDPVideoMedia -> addAttribute (
				psaSDPRtpMapAttribute) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_ADDATTRIBUTE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPRtpMapAttribute;

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (sStandard == MP4Atom:: MP4F_ISMA &&
			strstr ((const char *) bLocalSDP, "a=mpeg4-esid:") ==
			(char *) NULL)
		{
			SDPAttribute_p			psaSDPAttribute;
			unsigned long			ulTrackIdentifier;


			if ((psaSDPAttribute = new SDPAttribute_t) ==
				(SDPAttribute_p) NULL)
			{
				Error err = RTSPErrors (__FILE__, __LINE__,
					RTSP_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (psaSDPAttribute -> _pName, "mpeg4-esid");

			if (pmtiHintOrMediaVideoTrackInfo ->
				getTrackIdentifier (&ulTrackIdentifier) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRACKIDENTIFIER_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPAttribute;

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sprintf (psaSDPAttribute -> _pValue, "%lu",
				ulTrackIdentifier);

			if (pSDPVideoMedia -> addAttribute (
				psaSDPAttribute) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_ADDATTRIBUTE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPAttribute;

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (strstr ((const char *) bLocalSDP, "a=fmtp:") == (char *) NULL)
		{
			SDPFmtpAttribute_p		psaSDPFmtpAttribute;


			if ((psaSDPFmtpAttribute = new SDPFmtpAttribute_t) ==
				(SDPFmtpAttribute_p) NULL)
			{
				Error err = RTSPErrors (__FILE__, __LINE__,
					RTSP_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (psaSDPFmtpAttribute -> _pName, SDP_FMTPATTRIBUTENAME);

			strcpy (psaSDPFmtpAttribute -> _pFormat, pVideoPayloadNumber);

			if (cVideoCodecUsed == MP4Atom:: MP4F_CODEC_MPEG4)
			{
				unsigned char				*pucESConfiguration;
				#ifdef WIN32
					__int64						ullESConfigurationSize;
				#else
					unsigned long long			ullESConfigurationSize;
				#endif
				unsigned char				ucSystemsProfileLevel;
				char						*pESConfigToBase16;
				MP4Mp4vAtom_p				pmaMp4vAtom;
				unsigned long				ulWidth;
				unsigned long				ulHeight;


				if (pmtiHintOrMediaVideoTrackInfo -> getMp4vAtom (
					&pmaMp4vAtom, true) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETMP4VATOM_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pmaMp4vAtom -> getWidth (&ulWidth) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4MP4VATOM_GETWIDTH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pmaMp4vAtom -> getHeight (&ulHeight) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4MP4VATOM_GETHEIGHT_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pmtiHintOrMediaVideoTrackInfo ->
					getPointerTrackESConfiguration (
					&pucESConfiguration, &ullESConfigurationSize,
					sStandard) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETPOINTERTRACKESCONFIGURATION_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				{
					unsigned char				pucVoshStartCode [4];


					// attempt to get a valid profile-level
					pucVoshStartCode [0]			= 0x00;
					pucVoshStartCode [1]			= 0x00;
					pucVoshStartCode [2]			= 0x01;
					pucVoshStartCode [3]			= MP4F_MPEG4_VOSH_START;

					ucSystemsProfileLevel			= 0xFE;

					if (ullESConfigurationSize >= 5 &&
						!memcmp (pucESConfiguration, pucVoshStartCode, 4))
					{
						if (sStandard == MP4Atom:: MP4F_3GPP)
							ucSystemsProfileLevel	= pucESConfiguration [4];
						else			// MP4Atom:: MP4F_ISMA
							ucSystemsProfileLevel	=
								MP4AV_Mpeg4VideoToSystemsProfileLevel (
								pucESConfiguration [4]);
					}

					if (ucSystemsProfileLevel == 0xFE)
					{
						MP4InitialObjectDescr_p			pmaInitialObjectDescr;
						MP4Atom_p						pmaAtom;
						char							pAtomPath [
							MP4F_MAXPATHNAMELENGTH];
						unsigned char			ucVisualProfileLevelIndication;
						MP4RootAtom_p			pmaRootAtom;


						if (pmfFile -> getRootAtom (&pmaRootAtom) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILE_GETROOTATOM_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete psaSDPFmtpAttribute;

							if (sSDPFor3GPP. finish () != errNoError)
							{
								Error err = SDPErrors (__FILE__, __LINE__,
									SDP_SDP_FINISH_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							if (bLocalSDP. finish () != errNoError)
							{
								Error err = ToolsErrors (__FILE__, __LINE__,
									TOOLS_BUFFER_FINISH_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						strcpy (pAtomPath,
							"moov:0:iods:0:InitialObjectDescr:0");

						if (pmaRootAtom -> searchAtom (pAtomPath, true,
							&pmaAtom) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_SEARCHATOM_FAILED, 1,
								"moov:0:iods:0:InitialObjectDescr:0");
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete psaSDPFmtpAttribute;

							if (sSDPFor3GPP. finish () != errNoError)
							{
								Error err = SDPErrors (__FILE__, __LINE__,
									SDP_SDP_FINISH_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							if (bLocalSDP. finish () != errNoError)
							{
								Error err = ToolsErrors (__FILE__, __LINE__,
									TOOLS_BUFFER_FINISH_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
						else
							pmaInitialObjectDescr	=
								(MP4InitialObjectDescr_p) pmaAtom;

						if (pmaInitialObjectDescr ->
							getVisualProfileLevelIndication (
							&ucVisualProfileLevelIndication) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4INITIALOBJECTDESCR_GETVISUALPROFILELEVELINDICATION_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete psaSDPFmtpAttribute;

							if (sSDPFor3GPP. finish () != errNoError)
							{
								Error err = SDPErrors (__FILE__, __LINE__,
									SDP_SDP_FINISH_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							if (bLocalSDP. finish () != errNoError)
							{
								Error err = ToolsErrors (__FILE__, __LINE__,
									TOOLS_BUFFER_FINISH_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (ucVisualProfileLevelIndication > 0 &&
							ucVisualProfileLevelIndication < 0xFE)
						{
							ucSystemsProfileLevel		=
								ucVisualProfileLevelIndication;
						}
						else
						{
							ucSystemsProfileLevel		= 1;
						}
					}

					if ((pESConfigToBase16 = new char [
						(unsigned int) (ullESConfigurationSize * 2 + 1)]) ==
						(char *) NULL)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4UTILITY_NOPAYLOADAVAILABLE);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete psaSDPFmtpAttribute;

						if (sSDPFor3GPP. finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bLocalSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					// convert it into ASCII form
					if (Convert:: binaryToBase16 (pucESConfiguration,
						(unsigned long) ullESConfigurationSize,
						pESConfigToBase16,
						(unsigned long) (ullESConfigurationSize * 2 + 1)) !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_CONVERT_BINARYTOBASE16_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						delete psaSDPFmtpAttribute;

						if (sSDPFor3GPP. finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bLocalSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}

				sprintf (psaSDPFmtpAttribute -> _pFormatParameters,
					"profile-level-id=%lu;framesize=%lu-%lu;config=%s",
					(unsigned long) ucSystemsProfileLevel,
					ulWidth, ulHeight,
					pESConfigToBase16);

				delete [] pESConfigToBase16;
				pESConfigToBase16			= (char *) NULL;
			}
			else if (cVideoCodecUsed == MP4Atom:: MP4F_CODEC_H263)
			{
				MP4S263Atom_p				pmaS263Atom;
				MP4D263Atom_p				pmaD263Atom;
				unsigned long				ulWidth;
				unsigned long				ulHeight;
				unsigned long				ulH263Profile;
				unsigned long				ulH263Level;


				if (pmtiHintOrMediaVideoTrackInfo -> getS263Atom (
					&pmaS263Atom, true) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETS263ATOM_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pmaS263Atom -> getWidth (&ulWidth) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4S263ATOM_GETWIDTH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pmaS263Atom -> getHeight (&ulHeight) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4S263ATOM_GETHEIGHT_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pmtiHintOrMediaVideoTrackInfo -> getD263Atom (
					&pmaD263Atom, true) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETD263ATOM_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pmaD263Atom -> getH263Level (&ulH263Level) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4D263ATOM_GETH263LEVEL_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pmaD263Atom -> getH263Profile (&ulH263Profile) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4D263ATOM_GETH263PROFILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				sprintf (psaSDPFmtpAttribute -> _pFormatParameters,
					"profile=%lu;level=%lu;framesize=%lu-%lu",
					ulH263Profile, ulH263Level, ulWidth, ulHeight);

				{
					SDPAttribute_p			psaSDPAttribute;


					if ((psaSDPAttribute = new SDPAttribute_t) ==
						(SDPAttribute_p) NULL)
					{
						Error err = RTSPErrors (__FILE__, __LINE__,
							RTSP_NEW_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete psaSDPFmtpAttribute;

						if (sSDPFor3GPP. finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bLocalSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					strcpy (psaSDPAttribute -> _pName, "framesize");

					sprintf (psaSDPAttribute -> _pValue, "%s %lu-%lu",
						pVideoPayloadNumber, ulWidth, ulHeight);

					if (pSDPVideoMedia -> addAttribute (
						psaSDPAttribute) != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDPMEDIA_ADDATTRIBUTE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete psaSDPAttribute;

						delete psaSDPFmtpAttribute;

						if (sSDPFor3GPP. finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bLocalSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
			}
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_CODECUNKNOWN);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPFmtpAttribute;

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pSDPVideoMedia -> addAttribute (
				psaSDPFmtpAttribute) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_ADDATTRIBUTE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPFmtpAttribute;

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (strstr ((const char *) bLocalSDP, "a=control:") ==
			(char *) NULL)
		{
			SDPAttribute_p			psaSDPAttribute;


			if ((psaSDPAttribute = new SDPAttribute_t) ==
				(SDPAttribute_p) NULL)
			{
				Error err = RTSPErrors (__FILE__, __LINE__,
					RTSP_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (psaSDPAttribute -> _pName, "control");

			strcpy (psaSDPAttribute -> _pValue, pVideoTrackName);

			if (pSDPVideoMedia -> addAttribute (
				psaSDPAttribute) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_ADDATTRIBUTE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPAttribute;

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	if (pmtiHintOrMediaAudioTrackInfo != (MP4TrackInfo_p) NULL)
	{
		SDPMedia_p				pSDPAudioMedia;


		if (bLocalSDP. setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// audio SDP atom
		{
			Error_t					errGetSdpAtom;
			MP4SdpAtom_p			pmaSdpAtom;


			if ((errGetSdpAtom = pmtiHintOrMediaAudioTrackInfo ->
				getSdpAtom (&pmaSdpAtom, true)) != errNoError)
			{
				if ((long) errGetSdpAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETSDPATOM_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				if (pmaSdpAtom -> getSdpText (&bLocalSDP) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4SDPATOM_GETSDPTEXT_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
					
					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
		}

		if ((pSDPAudioMedia = new SDPMediaFor3GPP_t) ==
			(SDPMediaFor3GPP_p) NULL)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pSDPAudioMedia -> init (
			&bLocalSDP, ptTracer) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete pSDPAudioMedia;

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (sSDPFor3GPP. addSDPMedia (pSDPAudioMedia) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_ADDSDPMEDIA_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete pSDPAudioMedia;

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bLocalSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (strstr ((const char *) bLocalSDP, "m=") == (char *) NULL)
		{
			if (pSDPAudioMedia -> setMediaType (
				SDPMedia:: SDPMEDIA_AUDIO) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETMEDIATYPE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pSDPAudioMedia -> setPort (0) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETPORT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pSDPAudioMedia -> setTransport ("RTP/AVP") != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETTRANSPORT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pSDPAudioMedia -> setFmtList (pAudioPayloadNumber) !=
				errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETFMTLIST_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (pSDPAudioMedia -> getFmtList (pAudioPayloadNumber) !=
				errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_GETFMTLIST_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (strstr ((const char *) bLocalSDP, "b=") == (char *) NULL)
		{
			if (pSDPAudioMedia -> setBandwidthModifier (
				"AS") != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETBANDWIDTHMODIFIER_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pSDPAudioMedia -> setBandwidthValueInKbps (
				ulAudioAvgBitRate / 1000) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_SETBANDWIDTHVALUEINKBPS_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (strstr ((const char *) bLocalSDP, "a=rtpmap:") == (char *) NULL)
		{
			SDPRtpMapAttribute_p		psaSDPRtpMapAttribute;


			if ((psaSDPRtpMapAttribute = new SDPRtpMapAttribute_t) ==
				(SDPRtpMapAttribute_p) NULL)
			{
				Error err = RTSPErrors (__FILE__, __LINE__,
					RTSP_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (psaSDPRtpMapAttribute -> _pName, "rtpmap");

			strcpy (psaSDPRtpMapAttribute -> _pPayloadType,
				pAudioPayloadNumber);

			if (cAudioCodecUsed == MP4Atom:: MP4F_CODEC_AAC)
			{
				if (sStandard == MP4Atom:: MP4F_3GPP)
					strcpy (psaSDPRtpMapAttribute -> _pEncodingName,
						"MP4A-LATM");
				else	// isma
					strcpy (psaSDPRtpMapAttribute -> _pEncodingName,
						"mpeg4-generic");
			}
			else if (cAudioCodecUsed == MP4Atom:: MP4F_CODEC_AMRNB)
				strcpy (psaSDPRtpMapAttribute -> _pEncodingName, "AMR");
			else if (cAudioCodecUsed == MP4Atom:: MP4F_CODEC_AMRWB)
				strcpy (psaSDPRtpMapAttribute -> _pEncodingName, "AMR-WB");
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_CODECUNKNOWN);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPRtpMapAttribute;

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			{
				MP4TrakAtom_p				pmaMediaAudioTrack;
				MP4MdhdAtom_p				pmaMediaMdhdAtom;
				unsigned long				ulObjectTypeIndication;
				unsigned long				ulRTPTimeScale;


				if (pmtiHintOrMediaAudioTrackInfo ->
					getTrakAtom (&pmaMediaAudioTrack) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPRtpMapAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pmtiHintOrMediaAudioTrackInfo ->
					getMdhdAtom (&pmaMediaMdhdAtom) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETMDHDATOM_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPRtpMapAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (cAudioCodecUsed == MP4Atom:: MP4F_CODEC_AAC)
				{
					if (pmtiHintOrMediaAudioTrackInfo ->
						getObjectTypeIndication (&ulObjectTypeIndication) !=
						errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_GETOBJECTTYPEINDICATION_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete psaSDPRtpMapAttribute;

						if (sSDPFor3GPP. finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bLocalSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
				else
					ulObjectTypeIndication		= 0;

				if (RTPUtility:: getRTPTimeScale (
					pmaMediaAudioTrack,
					pmaMediaMdhdAtom,
					cAudioCodecUsed,
					ulObjectTypeIndication,
					sStandard, &ulRTPTimeScale,
					ptTracer) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPUTILITY_GETRTPTIMESCALE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPRtpMapAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				sprintf (psaSDPRtpMapAttribute -> _pClockRate, "%lu",
					ulRTPTimeScale);
			}

			if (sStandard == MP4Atom:: MP4F_ISMA &&
				cAudioCodecUsed == MP4Atom:: MP4F_CODEC_AAC)
			{
				unsigned char				*pucESConfiguration;
				#ifdef WIN32
					__int64						ullESConfigurationSize;
				#else
					unsigned long long			ullESConfigurationSize;
				#endif
				char						*pESConfigToBase16;


				if (pmtiHintOrMediaAudioTrackInfo ->
					getPointerTrackESConfiguration (
					&pucESConfiguration, &ullESConfigurationSize,
					sStandard) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETPOINTERTRACKESCONFIGURATION_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPRtpMapAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				{
					unsigned char					ucChannels;


					if ((pESConfigToBase16 = new char [
						(unsigned int) (ullESConfigurationSize * 2 + 1)]) ==
						(char *) NULL)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_NEW_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete psaSDPRtpMapAttribute;

						if (sSDPFor3GPP. finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bLocalSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					// convert it into ASCII form
					if (Convert:: binaryToBase16 (pucESConfiguration,
						(unsigned long) ullESConfigurationSize,
						pESConfigToBase16,
						(unsigned long) (ullESConfigurationSize * 2 + 1)) !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_CONVERT_BINARYTOBASE16_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						delete psaSDPRtpMapAttribute;

						if (sSDPFor3GPP. finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bLocalSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					ucChannels					=
						MP4AV_AacConfigGetChannels (pucESConfiguration);

					if (ucChannels != 1)
					{
						sprintf (psaSDPRtpMapAttribute -> _pEncodingParameters,
							"%u", ucChannels);
					}
					else
						strcpy (psaSDPRtpMapAttribute -> _pEncodingParameters,
							"");

					delete [] pESConfigToBase16;
					pESConfigToBase16			= (char *) NULL;
				}
			}
			else
				strcpy (psaSDPRtpMapAttribute -> _pEncodingParameters, "");

			if (pSDPAudioMedia -> addAttribute (
				psaSDPRtpMapAttribute) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_ADDATTRIBUTE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPRtpMapAttribute;

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (sStandard == MP4Atom:: MP4F_ISMA &&
			cAudioCodecUsed == MP4Atom:: MP4F_CODEC_AAC &&
			strstr ((const char *) bLocalSDP, "a=mpeg4-esid:") ==
			(char *) NULL)
		{
			SDPAttribute_p			psaSDPAttribute;
			unsigned long			ulTrackIdentifier;


			if ((psaSDPAttribute = new SDPAttribute_t) ==
				(SDPAttribute_p) NULL)
			{
				Error err = RTSPErrors (__FILE__, __LINE__,
					RTSP_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (psaSDPAttribute -> _pName, "mpeg4-esid");

			if (pmtiHintOrMediaAudioTrackInfo ->
				getTrackIdentifier (&ulTrackIdentifier) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRACKIDENTIFIER_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPAttribute;

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sprintf (psaSDPAttribute -> _pValue, "%lu",
				ulTrackIdentifier);

			if (pSDPAudioMedia -> addAttribute (
				psaSDPAttribute) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_ADDATTRIBUTE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPAttribute;

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (strstr ((const char *) bLocalSDP, "a=fmtp:") == (char *) NULL)
		{
			SDPFmtpAttribute_p		psaSDPFmtpAttribute;


			if ((psaSDPFmtpAttribute = new SDPFmtpAttribute_t) ==
				(SDPFmtpAttribute_p) NULL)
			{
				Error err = RTSPErrors (__FILE__, __LINE__,
					RTSP_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (psaSDPFmtpAttribute -> _pName, "fmtp");

			strcpy (psaSDPFmtpAttribute -> _pFormat, pAudioPayloadNumber);

			if (cAudioCodecUsed == MP4Atom:: MP4F_CODEC_AAC)
			{
				unsigned char				*pucESConfiguration;
				#ifdef WIN32
					__int64						ullESConfigurationSize;
				#else
					unsigned long long			ullESConfigurationSize;
				#endif
				char						*pESConfigToBase16;


				if (pmtiHintOrMediaAudioTrackInfo ->
					getPointerTrackESConfiguration (
					&pucESConfiguration, &ullESConfigurationSize,
					sStandard) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETPOINTERTRACKESCONFIGURATION_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (sSDPFor3GPP. finish () != errNoError)
					{
						Error err = SDPErrors (__FILE__, __LINE__,
							SDP_SDP_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bLocalSDP. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (sStandard == MP4Atom:: MP4F_3GPP)
				{
					// see StreamMuxConfig() in ISO document, audio part, sub1
					unsigned short				us;
					#ifdef WIN32
						__int64						ul;
						__int64						ulTemp;
					#else
						unsigned long long			ul;
						unsigned long long			ulTemp;
					#endif
					unsigned char				pucStreamMuxConfig [6];
					unsigned long				ulStreamMuxConfigSize		= 6;


					us						= ntohs (
						*((unsigned short *) pucESConfiguration));
					ul							= us;
					ul							= ul << 17;
					ulTemp						= 0x000040;
					ulTemp						= ulTemp << 40;
					ul							= ul | ulTemp;

					pucStreamMuxConfig [0]		= (unsigned char) ((ul >> (5 * 8)) & 0x00000000000000FF);
					pucStreamMuxConfig [1]		= (unsigned char) ((ul >> (4 * 8)) & 0x00000000000000FF);
					pucStreamMuxConfig [2]		= (unsigned char) ((ul >> (3 * 8)) & 0x00000000000000FF);
					pucStreamMuxConfig [3]		= (unsigned char) ((ul >> (2 * 8)) & 0x00000000000000FF);
					pucStreamMuxConfig [4]		= (unsigned char) ((ul >> (1 * 8)) & 0x00000000000000FF);
					pucStreamMuxConfig [5]		= (unsigned char) ((ul >> (0 * 8)) & 0x00000000000000FF);

					if ((pESConfigToBase16 =
						new char [ulStreamMuxConfigSize * 2 + 1]) ==
						(char *) NULL)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_NEW_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete psaSDPFmtpAttribute;

						if (sSDPFor3GPP. finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bLocalSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					// convert it into ASCII form
					if (Convert:: binaryToBase16 (pucStreamMuxConfig,
						ulStreamMuxConfigSize,
						pESConfigToBase16, ulStreamMuxConfigSize * 2 + 1) !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_CONVERT_BINARYTOBASE16_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						delete psaSDPFmtpAttribute;

						if (sSDPFor3GPP. finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bLocalSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (((unsigned char) pucESConfiguration [0] >> 3) ==
						MP4_MPEG4_AAC_LC_AUDIO_TYPE)
					{
						sprintf (psaSDPFmtpAttribute -> _pFormatParameters,
							"profile-level-id=15;object=2;cpresent=0;config=%s",
							pESConfigToBase16);
					}
					else	// should be MP4_MPEG4_AAC_LTP_AUDIO_TYPE
					{
						sprintf (psaSDPFmtpAttribute -> _pFormatParameters,
							"profile-level-id=15;object=4;cpresent=0;config=%s",
							pESConfigToBase16);
					}
				}
				else	// isma
				{
					if ((pESConfigToBase16 =
						new char [(unsigned int) (ullESConfigurationSize * 2 + 1)]) ==
						(char *) NULL)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_NEW_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete psaSDPFmtpAttribute;

						if (sSDPFor3GPP. finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bLocalSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					// convert it into ASCII form
					if (Convert:: binaryToBase16 (pucESConfiguration,
						(unsigned long) ullESConfigurationSize,
						pESConfigToBase16,
						(unsigned long) (ullESConfigurationSize * 2 + 1)) !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_CONVERT_BINARYTOBASE16_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						delete psaSDPFmtpAttribute;

						if (sSDPFor3GPP. finish () != errNoError)
						{
							Error err = SDPErrors (__FILE__, __LINE__,
								SDP_SDP_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (bLocalSDP. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (((unsigned char) pucESConfiguration [0] >> 3) ==
						MP4_MPEG4_CELP_AUDIO_TYPE)
					{
						sprintf (psaSDPFmtpAttribute -> _pFormatParameters,
							"streamtype=5;profile-level-id=15;mode=CELP-vbr;config=%s;SizeLength=6;IndexLength=2;IndexDeltaLength=2;Profile=0",
							pESConfigToBase16);
					}
					else
					{
						sprintf (psaSDPFmtpAttribute -> _pFormatParameters,
							"streamtype=5;profile-level-id=15;mode=AAC-hbr;config=%s;SizeLength=13;IndexLength=3;IndexDeltaLength=3;Profile=1",
							pESConfigToBase16);
					}
				}

				delete [] pESConfigToBase16;
				pESConfigToBase16			= (char *) NULL;
			}
			else if (cAudioCodecUsed == MP4Atom:: MP4F_CODEC_AMRNB ||
				cAudioCodecUsed == MP4Atom:: MP4F_CODEC_AMRWB)
			{
				strcpy (psaSDPFmtpAttribute -> _pFormatParameters,
					"octet-align=1");
			}
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_CODECUNKNOWN);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPFmtpAttribute;

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pSDPAudioMedia -> addAttribute (
				psaSDPFmtpAttribute) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_ADDATTRIBUTE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPFmtpAttribute;

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (strstr ((const char *) bLocalSDP, "a=control:") ==
			(char *) NULL)
		{
			SDPAttribute_p			psaSDPAttribute;


			if ((psaSDPAttribute = new SDPAttribute_t) ==
				(SDPAttribute_p) NULL)
			{
				Error err = RTSPErrors (__FILE__, __LINE__,
					RTSP_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (psaSDPAttribute -> _pName, "control");

			strcpy (psaSDPAttribute -> _pValue, pAudioTrackName);

			if (pSDPAudioMedia -> addAttribute (
				psaSDPAttribute) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_ADDATTRIBUTE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPAttribute;

				if (sSDPFor3GPP. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDP_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bLocalSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	if (sSDPFor3GPP. appendToBuffer (pbSDP) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPFOR3GPP_APPENDTOBUFFER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sSDPFor3GPP. finish () != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
		}

		if (bLocalSDP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (sSDPFor3GPP. finish () != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPFOR3GPP_FINISH_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bLocalSDP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bLocalSDP. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPUtility:: getMediaInfoFromSDPFile (
	const char *pSDPPathName,
	Boolean_p pbVideoTrackFoundInSDP,
	MP4Atom:: MP4Codec_p pcVideoCodecUsed,
	unsigned long *pulVideoAvgBitRate,
	unsigned long *pulVideoPayloadNumber,
	char *pVideoTrackName,
	unsigned long *pulVideoPort,
	Boolean_p pbAudioTrackFoundInSDP,
	MP4Atom:: MP4Codec_p pcAudioCodecUsed,
	unsigned long *pulAudioAvgBitRate,
	unsigned long *pulAudioPayloadNumber,
	char *pAudioTrackName,
	unsigned long *pulAudioPort,
	MP4Atom:: Standard_t sStandard,
	Buffer_p pbSDP,
	Tracer_p ptTracer)

{

	Error_t					errGetSDPMedia;
	SDPMedia_p				psmSDPMedia;
	SDPFor3GPP_t			sSDPFor3GPP;


	if (sSDPFor3GPP. init (pSDPPathName, ptTracer) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPFOR3GPP_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errGetSDPMedia = sSDPFor3GPP. getSDPMedia (
		SDPMedia:: SDPMEDIA_VIDEO, 0, &psmSDPMedia)) != errNoError)
	{
		if ((long) errGetSDPMedia != SDP_SDP_SDPMEDIANOTFOUND)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_GETSDPMEDIA_FAILED);   
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFOR3GPP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if ((long) errGetSDPMedia == SDP_SDP_SDPMEDIANOTFOUND)
	{
		*pbVideoTrackFoundInSDP				= false;
		*pulVideoPayloadNumber				= 0;
		strcpy (pVideoTrackName, "");
		*pulVideoPort						= 0;
		*pcVideoCodecUsed					= MP4Atom:: MP4F_CODEC_UNKNOWN;
		*pulVideoAvgBitRate					= 0;
	}
	else
	{
		SDPMedia:: SDPMediaType_t	mtMediaType;
		char					pMediaEncodingName [
			SDP_ENCODINGNAMELENGTH];


		*pbVideoTrackFoundInSDP				= true;

		if (((SDPMediaFor3GPP_p) psmSDPMedia) -> getMediaInfo (
			&mtMediaType,
			pVideoTrackName,
			pulVideoPayloadNumber,
			pulVideoPort,
			pMediaEncodingName,
			(char *) NULL,
			(char *) NULL,
			pulVideoAvgBitRate) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_GETMEDIAINFO_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFOR3GPP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		(*pulVideoAvgBitRate)		*= 1000;

		if (!strncmp (pMediaEncodingName, "H263-", 5))
			*pcVideoCodecUsed		= MP4Atom:: MP4F_CODEC_H263;
		else if (!strncmp (pMediaEncodingName, "MP4V-ES", 7))
			*pcVideoCodecUsed		= MP4Atom:: MP4F_CODEC_MPEG4;
		else
			*pcVideoCodecUsed		= MP4Atom:: MP4F_CODEC_UNKNOWN;

	}

	if ((errGetSDPMedia = sSDPFor3GPP. getSDPMedia (
		SDPMedia:: SDPMEDIA_AUDIO, 0, &psmSDPMedia)) != errNoError)
	{
		if ((long) errGetSDPMedia != SDP_SDP_SDPMEDIANOTFOUND)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_GETSDPMEDIA_FAILED);   
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFOR3GPP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if ((long) errGetSDPMedia == SDP_SDP_SDPMEDIANOTFOUND)
	{
		*pbAudioTrackFoundInSDP				= false;
		*pulAudioPayloadNumber				= 0;
		strcpy (pAudioTrackName, "");
		*pulAudioPort						= 0;
		*pcAudioCodecUsed					= MP4Atom:: MP4F_CODEC_UNKNOWN;
		*pulAudioAvgBitRate					= 0;
	}
	else
	{
		SDPMedia:: SDPMediaType_t	mtMediaType;
		char					pMediaEncodingName [
			SDP_ENCODINGNAMELENGTH];


		*pbAudioTrackFoundInSDP				= true;

		if (((SDPMediaForRTSP_p) psmSDPMedia) -> getMediaInfo (
			&mtMediaType,
			pAudioTrackName,
			pulAudioPayloadNumber,
			pulAudioPort,
			pMediaEncodingName,
			(char *) NULL,
			(char *) NULL,
			pulAudioAvgBitRate) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_GETMEDIAINFO_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFOR3GPP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		*pulAudioAvgBitRate		*= 1000;

		if (!strncmp (pMediaEncodingName, "MP4A-LATM", 9))
			*pcAudioCodecUsed		= MP4Atom:: MP4F_CODEC_AAC;
		else if (!strncmp (pMediaEncodingName, "AMR-WB", 6))
			*pcAudioCodecUsed		= MP4Atom:: MP4F_CODEC_AMRWB;
		else if (!strncmp (pMediaEncodingName, "AMR", 3))
			*pcAudioCodecUsed		= MP4Atom:: MP4F_CODEC_AMRNB;
		else if (!strncmp (pMediaEncodingName, "mpeg4-generic", 13))
			*pcAudioCodecUsed		= MP4Atom:: MP4F_CODEC_AAC;
		else
			*pcAudioCodecUsed		= MP4Atom:: MP4F_CODEC_UNKNOWN;
	}

	if (sSDPFor3GPP. appendToBuffer (pbSDP) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPFOR3GPP_APPENDTOBUFFER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sSDPFor3GPP. finish () != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (sSDPFor3GPP. finish () != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPFOR3GPP_FINISH_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}
*/


Error RTSPUtility:: getDESCRIBEResponse (
	unsigned long ulSequenceNumber,
	unsigned long ulRTSP_RTCPTimeoutInSecs,
	const char *pTimestamp,
	Buffer_p pbSDP,
	Buffer_p pbResponse,
	Tracer_p ptTracer)

{

	if (getRTSPResponse (RTSP_PREDEFINEDRTSPVERSION,
		200, ulSequenceNumber, (const char *) NULL, ulRTSP_RTCPTimeoutInSecs,
		false, pTimestamp, pbResponse, ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETRTSPRESPONSE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append ("Content-Length: ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append ((unsigned long) (*pbSDP)) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append (RTSP_NEWLINE "Content-Type: application/sdp") !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// Content-Base
	// HTTP 1.1 shall be supported by the handsets as specificated
	//	by the 3GPP TS 26.134 Rel. 6.
	//	HTTP 1.1 does not support anymore 'Content-Base', therefore it
	//	is deleted. However rtsp rfc (2326) specifies that this field
	//	is optional.
	/*
	{
		if (pbResponse -> append (RTSP_NEWLINE "Content-Base: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append ("rtsp://") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append (pURLIPAddress) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_ulRTSPRequestsPort != 554)
		{
			if (pbResponse -> append (":") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbResponse -> append (_ulRTSPRequestsPort) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (pbResponse -> append ("/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	*/

	if (pbResponse -> append (RTSP_NEWLINE RTSP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append ((const char *) (*pbSDP)) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

/*
	if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}
*/

	return errNoError;
}


Error RTSPUtility:: parseDESCRIBEResponse (
	const char *pRTSPResponse,

	unsigned long *pulStatusCode,
	Buffer_p pbLocation,
	unsigned long *pulSequenceNumber,

	Buffer_p pbSession,
	unsigned long *pulTimeout,

	Buffer_p pbSDP,

	Tracer_p ptTracer)

{

	const char						*pPointerToRTSPResponse;
	StringTokenizer_t				stSpaceTokenizer;
	const char						*pToken;
	Boolean_t						bResponseFinished;
	Error_t							errNextToken;


	*pulStatusCode				= 0;
	*pulSequenceNumber			= 0;
	bResponseFinished			= false;

	if (pbSession != (Buffer_p) NULL)
	{
		if (pbSession -> setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pulTimeout != (unsigned long *) NULL)
		*pulTimeout					= 0;

	if (pbSDP != (Buffer_p) NULL)
	{
		if (pbSDP -> setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbLocation != (Buffer_p) NULL)
	{
		if (pbLocation -> setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	pPointerToRTSPResponse		= pRTSPResponse;

	while (!bResponseFinished)
	{
		if (*pPointerToRTSPResponse == '\r' ||	// means "\r\n\r\n"
			strchr (pPointerToRTSPResponse, '\r') == (char *) NULL)
		{
			bResponseFinished				= true;

			if (*pulStatusCode == RTSP_STATUSCODE_OK)
			{
				pPointerToRTSPResponse		+= 2;	// skip '\r\n'

				if (pbSDP != (Buffer_p) NULL)
				{
					if (pbSDP -> setBuffer (pPointerToRTSPResponse) !=
						errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_SETBUFFER_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return err;
					}
				}
			}

			continue;
		}

		if (stSpaceTokenizer. init (pPointerToRTSPResponse,
			(long) (strchr (pPointerToRTSPResponse, '\r') -
			pPointerToRTSPResponse), " ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pPointerToRTSPResponse		= strchr (pPointerToRTSPResponse, '\n') + 1;

		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!strcmp (pToken, RTSP_PREDEFINEDRTSPVERSION))
		{
			// 'RTSP/1.0 200 OK
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*pulStatusCode			= atol (pToken);
		}
		else if (!strcmp (pToken, "Session:"))
		{
			// Session: <session>[;timeout=XX]
			if (stSpaceTokenizer. nextToken (&pToken, ";") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbSession != (Buffer_p) NULL)
			{
				if (pbSession -> setBuffer (pToken) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SETBUFFER_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			if (pulTimeout != (unsigned long *) NULL)
			{
				if ((errNextToken = stSpaceTokenizer. nextToken (
					&pToken, "=")) != errNoError)
				{
					if ((long) errNextToken !=
						TOOLS_STRINGTOKENIZER_NOMORETOKEN)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}

				// could be also TOOLS_STRINGTOKENIZER_NOMORETOKEN)
				if (errNextToken == errNoError)
				{
					if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					*pulTimeout			= atol (pToken);
				}
			}
		}
		else if (!strcmp (pToken, "Location:"))
		{
			// Location: <new URL>
			if (pbLocation != (Buffer_p) NULL)
			{
				if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pbLocation -> setBuffer (pToken) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SETBUFFER_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
		}
		else if (!strcmp (pToken, "CSeq:"))
		{
			// 'CSeq: <SequenceNumber>'
			// SequenceNumber
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*pulSequenceNumber				= atol (pToken);
		}
		else if (!strcmp (pToken, "Date:"))
		{
			// 'Date: Thu, 14 Apr 2005 15:11:42 GMT'
			// Date
			/*
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			*/
		}
		else if (!strcmp (pToken, "Server:"))
		{
			// 'Server: <Server>'
			// Server
			/*
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			*/
		}
		else if (!strcmp (pToken, "Content-Length:"))
		{
			// 'Content-Length: <Content-Length>'
			/*
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			*/
		}
		else if (!strcmp (pToken, "Content-Type:"))
		{
			// 'Content-Type: <Content-Type>'
			/*
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			*/
		}
		else if (!strcmp (pToken, "Content-Base:"))
		{
			// 'Content-Base: <Content-Base>'
			/*
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			*/
		}
		else
		{
			;
		}

		if (stSpaceTokenizer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// check for DESCRIBE response
	{
		if (*pulStatusCode == 0)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_DESCRIBERESPONSEWRONG,
				1, pRTSPResponse);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error RTSPUtility:: getSETUPRequest (
	const char *pURL,
	const char *pControlValue,
	unsigned long ulSequenceNumber,
	const char *pSession,
	unsigned long ulClientRTPPort, unsigned long ulClientRTCPPort,
	const char *pUserAgent,
	Buffer_p pbRTSPRequest,
	Tracer_p ptTracer)

{

	if (pbRTSPRequest -> setBuffer ("SETUP ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (strchr (pURL, '?') != (char *) NULL)
	{
		if (pbRTSPRequest -> append (pURL,
			strchr (pURL, '?') - pURL) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		if (pbRTSPRequest -> append (pURL) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbRTSPRequest -> append ("/") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (pControlValue) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (" " RTSP_PREDEFINEDRTSPVERSION
		RTSP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append ("CSeq: ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (ulSequenceNumber) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (RTSP_NEWLINE
		"Transport: RTP/AVP/UDP;unicast;client_port=") !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (ulClientRTPPort) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append ("-") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (ulClientRTCPPort) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (RTSP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pSession != (const char *) NULL)
	{
		if (pbRTSPRequest -> append ("Session: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbRTSPRequest -> append (pSession) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbRTSPRequest -> append (RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pUserAgent != (const char *) NULL)
	{
		if (pbRTSPRequest -> append ("User-Agent: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbRTSPRequest -> append (pUserAgent) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbRTSPRequest -> append (RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbRTSPRequest -> append (RTSP_NEWLINE) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPUtility:: parseSETUPRequest (
	const char *pRTSPRequest,

	Buffer_p pbRTSPVersion,
	Buffer_p pbTransport,
	Buffer_p pbURI,
	long *plSequenceNumber,
	unsigned long *pulRTPClientPort,
	unsigned long *pulRTCPClientPort,
	char *pSessionIdentifier,
	char *pTimestamp,
	unsigned long *pulRequestedPayloadSizeInBytes,
	unsigned long *pulRequestLength,
	Tracer_p ptTracer)

{

	const char						*pPointerToRTSPRequest;
	StringTokenizer_t				stSpaceTokenizer;
	const char						*pToken;
	// Boolean_t						bRTSPVersionRead;
	Boolean_t						bRequestFinished;
	// Error							errNextToken;
	Boolean_t						bLocalRTPPortInitialized;
	Boolean_t						bLocalRTCPPortInitialized;


	*plSequenceNumber				= -1;
	bLocalRTPPortInitialized		= false;
	bLocalRTCPPortInitialized		= false;
	strcpy (pSessionIdentifier, "");
	strcpy (pTimestamp, "");
	*pulRequestedPayloadSizeInBytes	= 0;
	*pulRequestLength				= 0;

	if (pbTransport -> setBuffer ("") != errNoError ||
		pbURI -> setBuffer ("") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPVersion != (Buffer_p) NULL)
	{
		if (pbRTSPVersion -> setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// bRTSPVersionRead				= false;
	bRequestFinished				= false;

	pPointerToRTSPRequest		= pRTSPRequest;

	while (!bRequestFinished)
	{
		if (*pPointerToRTSPRequest == '\r' ||	// means "\r\n\r\n"
			strchr (pPointerToRTSPRequest, '\r') == (char *) NULL)
		{
			bRequestFinished				= true;

			*pulRequestLength			= (unsigned long)
				(pPointerToRTSPRequest - pRTSPRequest + 2);

			continue;
		}

		if (stSpaceTokenizer. init (pPointerToRTSPRequest,
			(long) (strchr (pPointerToRTSPRequest, '\r') -
			pPointerToRTSPRequest), " ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pPointerToRTSPRequest		= strchr (pPointerToRTSPRequest, '\n') + 1;

		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
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
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbURI -> setBuffer (pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SETBUFFER_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			#ifdef LUXXON_REQUEST
				pbURI -> substitute ("req1.0?", "");
			#endif

			// <RTSP-Version>
			if (pbRTSPVersion != (Buffer_p) NULL)
			{
				if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pbRTSPVersion -> setBuffer (pToken) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_SETBUFFER_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			// bRTSPVersionRead				= true;
		}
		else if (!strcmp (pToken, "CSeq:"))
		{
			// 'CSeq: <SequenceNumber>'
			// SequenceNumber
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*plSequenceNumber				= atol (pToken);
		}
		else if (!strcmp (pToken, "Timestamp:"))
		{
			// 'Timestamp: <Timestamp>'
			// Timestamp
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
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
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*pulRequestedPayloadSizeInBytes				=
				atol (pToken);
		}
		else if (!strcmp (pToken, "Transport:"))
		{
			// i.e. for a RealMedia Player:  Transport: RTP/AVP;unicast;client_port=6970-6971;mode=play,RTP/AVP/TCP;unicast;mode=play
			if (stSpaceTokenizer. nextToken (&pToken, ";") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (strstr (pToken, "RTP/AVP") == (const char *) NULL ||
				strstr (pToken, "RTP/AVP/TCP") != (const char *) NULL)
			{
				Error err = RTSPErrors (__FILE__, __LINE__,
					RTSP_RTSPUTILITY_TRANSPORTNOTCOMPLIANT,
					1, pToken);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// parameters
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (strstr (pToken, "unicast") == (const char *) NULL)
			{
				Error err = RTSPErrors (__FILE__, __LINE__,
					RTSP_RTSPUTILITY_TRANSPORTNOTCOMPLIANT,
					1, pToken);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// Initialize ulRTPClientPort and ulRTCPClientPort
			{
				const char			*pRTPClientPort;
				char				*pRTCPClientPort;
				char				pClientPorts [RTSP_MAXLONGLENGTH];


				strcpy (pClientPorts, pToken);

				pRTPClientPort			= strstr (pClientPorts, "client_port=");
				if (pRTPClientPort == (const char *) NULL)
				{
					Error err = RTSPErrors (__FILE__, __LINE__,
						RTSP_RTSPUTILITY_SETUPTRANSPORTWRONG, 1, pToken);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
				pRTPClientPort					+= strlen ("client_port");

				pRTCPClientPort			= (char *) strchr (pRTPClientPort, '-');
				if (pRTCPClientPort == (char *) NULL)
				{
					Error err = RTSPErrors (__FILE__, __LINE__,
						RTSP_RTSPUTILITY_SETUPTRANSPORTWRONG, 1, pToken);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				{
					char			*pEndOfRTCPClientPort;


					pEndOfRTCPClientPort	= pRTCPClientPort + 1;
					while (isdigit (*pEndOfRTCPClientPort))
						pEndOfRTCPClientPort++;
					*pEndOfRTCPClientPort			= '\0';
				}

				*pulRTCPClientPort			= atol (pRTCPClientPort + 1);
				*pRTCPClientPort			= '\0';

				*pulRTPClientPort			= atol (pRTPClientPort + 1);

				bLocalRTPPortInitialized		= true;
				bLocalRTCPPortInitialized		= true;
			}

			if (pbTransport -> setBuffer ("Transport: ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SETBUFFER_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbTransport -> append ("RTP/AVP/UDP;unicast;client_port=") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbTransport -> append (*pulRTPClientPort) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbTransport -> append ("-") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbTransport -> append (*pulRTCPClientPort) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
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
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
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
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// check for SETUP request
	{
		if (*plSequenceNumber == -1 ||
			bLocalRTPPortInitialized == false ||
			bLocalRTCPPortInitialized == false)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_SETUPREQUESTWRONG,
				1, pRTSPRequest);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	

	return errNoError;
}


Error RTSPUtility:: getSETUPResponse (
	unsigned long ulSequenceNumber,
	const char *pSessionIdentifier,
	unsigned long ulRTSP_RTCPTimeoutInSecs,
	const char *pTimestamp,
	Buffer_p pbTransport,
	unsigned long ulRTPServerPort,
	unsigned long ulRTCPServerPort,
	Buffer_p pbResponse,
	Tracer_p ptTracer)

{

	if (getRTSPResponse (
		RTSP_PREDEFINEDRTSPVERSION, 200, ulSequenceNumber,
		pSessionIdentifier, ulRTSP_RTCPTimeoutInSecs, false,
		pTimestamp, pbResponse, ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETRTSPRESPONSE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append ((const char *) (*pbTransport)) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append (";server_port=") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append (ulRTPServerPort) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append ("-") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append (ulRTCPServerPort) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append (RTSP_NEWLINE RTSP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPUtility:: parseSETUPResponse (
	const char *pRTSPResponse,

	unsigned long *pulStatusCode,
	Buffer_p pbSession,
	unsigned long *pulTimeout,
	unsigned long *pulSequenceNumber,
	unsigned long *pulServerRTPPort,
	unsigned long *pulServerRTCPPort,

	Tracer_p ptTracer)

{

	const char						*pPointerToRTSPResponse;
	StringTokenizer_t				stSpaceTokenizer;
	const char						*pToken;
	Boolean_t						bResponseFinished;
	Error_t							errNextToken;


	*pulStatusCode				= 0;
	if (pulTimeout != (unsigned long *) NULL)
		*pulTimeout					= 0;

	if (pulSequenceNumber != (unsigned long *) NULL)
		*pulSequenceNumber			= 0;

	*pulServerRTPPort			= 0;
	*pulServerRTCPPort			= 0;
	bResponseFinished			= false;


	if (pbSession -> setBuffer ("") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	pPointerToRTSPResponse		= pRTSPResponse;

	while (!bResponseFinished)
	{
		if (*pPointerToRTSPResponse == '\r' ||	// means "\r\n\r\n"
			strchr (pPointerToRTSPResponse, '\r') == (char *) NULL)
		{
			bResponseFinished				= true;

			continue;
		}

		if (stSpaceTokenizer. init (pPointerToRTSPResponse,
			(long) (strchr (pPointerToRTSPResponse, '\r') -
			pPointerToRTSPResponse), " ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pPointerToRTSPResponse		= strchr (pPointerToRTSPResponse, '\n') + 1;

		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!strcmp (pToken, RTSP_PREDEFINEDRTSPVERSION))
		{
			// 'RTSP/1.0 200 OK
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*pulStatusCode			= atol (pToken);
		}
		else if (!strcmp (pToken, "Session:"))
		{
			// Session: <session>[;timeout=XX]
			if (stSpaceTokenizer. nextToken (&pToken, ";") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pbSession -> setBuffer (pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_SETBUFFER_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pulTimeout != (unsigned long *) NULL)
			{
				if ((errNextToken = stSpaceTokenizer. nextToken (
					&pToken, "=")) != errNoError)
				{
					if ((long) errNextToken !=
						TOOLS_STRINGTOKENIZER_NOMORETOKEN)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}

				if ((long) errNextToken == TOOLS_STRINGTOKENIZER_NOMORETOKEN)
					break;

				if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				*pulTimeout			= atol (pToken);
			}
		}
		else if (!strcmp (pToken, "CSeq:"))
		{
			// 'CSeq: <SequenceNumber>'
			// SequenceNumber
			if (pulSequenceNumber != (unsigned long *) NULL)
			{
				if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				*pulSequenceNumber				= atol (pToken);
			}
		}
		else if (!strcmp (pToken, "Transport:"))
		{
			// Transport: RTP/AVP/UDP;unicast;client_port=2001-2002;server_port=20022-20023
			if (stSpaceTokenizer. nextToken (&pToken, ";") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stSpaceTokenizer. nextToken (&pToken, ";") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stSpaceTokenizer. nextToken (&pToken, ";") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stSpaceTokenizer. nextToken (&pToken, "=") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stSpaceTokenizer. nextToken (&pToken, "-") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*pulServerRTPPort			= atol (pToken);

			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*pulServerRTCPPort			= atol (pToken);
		}
		else if (!strcmp (pToken, "Date:"))
		{
			// 'Date: Thu, 14 Apr 2005 15:11:42 GMT'
			// Date
			/*
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			*/
		}
		else if (!strcmp (pToken, "Server:"))
		{
			// 'Server: <Server>'
			// Server
			/*
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			*/
		}
		else
		{
			;
		}

		if (stSpaceTokenizer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// check for SETUP response
	{
		if (*pulStatusCode == 0)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_DESCRIBERESPONSEWRONG,
				1, pRTSPResponse);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error RTSPUtility:: getPLAYRequest (
	const char *pURL,
	const char *pControlValue,
	unsigned long ulSequenceNumber,
	const char *pSession,
	const char *pUserAgent,
	Buffer_p pbRTSPRequest,
	Tracer_p ptTracer)

{

	if (pbRTSPRequest -> setBuffer ("PLAY ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (strchr (pURL, '?') != (char *) NULL)
	{
		if (pbRTSPRequest -> append (pURL,
			strchr (pURL, '?') - pURL) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		if (pbRTSPRequest -> append (pURL) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pControlValue != (const char *) NULL)
	{
		if (pbRTSPRequest -> append ("/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbRTSPRequest -> append (pControlValue) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbRTSPRequest -> append (" " RTSP_PREDEFINEDRTSPVERSION RTSP_NEWLINE) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append ("CSeq: ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (ulSequenceNumber) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (RTSP_NEWLINE "Session: ") !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (pSession) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (RTSP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pUserAgent != (const char *) NULL)
	{
		if (pbRTSPRequest -> append ("User-Agent: ") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbRTSPRequest -> append (pUserAgent) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbRTSPRequest -> append (RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbRTSPRequest -> append (RTSP_NEWLINE) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPUtility:: parsePLAYRequest (
	const char *pRTSPRequest,

	Buffer_p pbRTSPVersion,
	Buffer_p pbURI,
	long *plSequenceNumber,
	char *pTimestamp,
	Boolean_p pbHasSpeed,
	double *pdSpeed,
	Boolean_p pbRangeFound,
	Boolean_p pbIsNow,
	double *pdNptStartTime,
	double *pdNptEndTime,
	char *pSessionIdentifier,
	unsigned long *pulRequestLength,
	Tracer_p ptTracer)

{

	const char						*pPointerToRTSPRequest;
	StringTokenizer_t				stSpaceTokenizer;
	const char						*pToken;
	// Boolean_t						bRTSPVersionRead;
	Boolean_t						bRequestFinished;
	// Error							errNextToken;


	*plSequenceNumber				= -1;
	strcpy (pTimestamp, "");
	*pbHasSpeed						= false;
	*pdSpeed						= 0;
	*pbRangeFound					= false;
	*pbIsNow						= false;
	*pdNptStartTime					= 0;
	*pdNptEndTime					= 0;
	strcpy (pSessionIdentifier, "");
	*pulRequestLength				= 0;

	if (pbURI != (Buffer_p) NULL)
	{
		if (pbURI -> setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbRTSPVersion != (Buffer_p) NULL)
	{
		if (pbRTSPVersion -> setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// bRTSPVersionRead				= false;
	bRequestFinished				= false;

	pPointerToRTSPRequest		= pRTSPRequest;

	while (!bRequestFinished)
	{
		if (*pPointerToRTSPRequest == '\r' ||	// means "\r\n\r\n"
			strchr (pPointerToRTSPRequest, '\r') == (char *) NULL)
		{
			bRequestFinished				= true;

			*pulRequestLength			= (unsigned long)
				(pPointerToRTSPRequest - pRTSPRequest + 2);

			continue;
		}

		if (stSpaceTokenizer. init (pPointerToRTSPRequest,
			(long) (strchr (pPointerToRTSPRequest, '\r') -
			pPointerToRTSPRequest), " ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pPointerToRTSPRequest		= strchr (pPointerToRTSPRequest, '\n') + 1;

		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!strcmp (pToken, "PLAY"))
		{
			// 'PLAY <Request-URI> <RTSP-Version>'
			// Request-URI
			if (pbURI != (Buffer_p) NULL ||
				pbRTSPVersion != (Buffer_p) NULL)
			{
				if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pbURI != (Buffer_p) NULL)
				{
					if (pbURI -> setBuffer (pToken) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_SETBUFFER_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}

				// <RTSP-Version>
				if (pbRTSPVersion != (Buffer_p) NULL)
				{
					if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbRTSPVersion -> setBuffer (pToken) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_SETBUFFER_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
			}

			// bRTSPVersionRead				= true;
		}
		else if (!strcmp (pToken, "CSeq:"))
		{
			// 'CSeq: <SequenceNumber>'
			// SequenceNumber
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*plSequenceNumber				= atol (pToken);
		}
		else if (!strcmp (pToken, "Timestamp:"))
		{
			// 'Timestamp: <Timestamp>'
			// Timestamp
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (pTimestamp, pToken);
		}
		else if (!strcmp (pToken, "Speed:"))
		{
			// 'Speed: <Speed>'

			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*pdSpeed				= atof (pToken);
			*pbHasSpeed				= true;
		}
		else if (!strcmp (pToken, "Range:"))
		{
			// Range: npt=0-....
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (SDPForRTSP:: getRangeAttributeInfo (
				pToken, pbIsNow, pdNptStartTime, pdNptEndTime,
				ptTracer) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFORRTSP_GETRANGEATTRIBUTEINFO_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*pbRangeFound						= true;
		}
		else if (!strcmp (pToken, "Session:"))
		{
			// 'Session: <SessionIdentifier>[;timeout=...]'
			// SessionIdentifier
			if (stSpaceTokenizer. nextToken (&pToken, ";") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
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
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// check for PLAY request
	{
		if (*plSequenceNumber == -1 || !strcmp (pSessionIdentifier, ""))
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_PLAYREQUESTWRONG,
				1, pRTSPRequest);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error RTSPUtility:: getPLAYResponse (
	unsigned long ulSequenceNumber,
	const char *pSessionIdentifier,
	unsigned long ulRTSP_RTCPTimeoutInSecs,
	const char *pTimestamp,

	Boolean_t bHasSpeed,
	double *pdSpeed,
	Boolean_t bHasRange,
	Boolean_t bIsNow,
	double *pdNptStartTime,
	double *pdNptEndTime,

	Boolean_t bVideoRTPInfo,
	const char *pVideoRequestURI,
	unsigned long ulVideoSequenceNumber,
	unsigned long ulVideoTimeStamp,

	Boolean_t bAudioRTPInfo,
	const char *pAudioRequestURI,
	unsigned long ulAudioSequenceNumber,
	unsigned long ulAudioTimeStamp,

	Buffer_p pbResponse,
	Tracer_p ptTracer)

{

	if (getRTSPResponse (
		RTSP_PREDEFINEDRTSPVERSION, 200, ulSequenceNumber,
		pSessionIdentifier, ulRTSP_RTCPTimeoutInSecs, false,
		pTimestamp, pbResponse, ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETRTSPRESPONSE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (bHasSpeed)
	{
		if (pbResponse -> append ("Speed: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append (*pdSpeed) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (bHasRange)
	{
		if (pbResponse -> append ("Range: npt=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (bIsNow)
		{
			if (pbResponse -> append ("now-") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
		else
		{
			if (*pdNptStartTime != -1.0)
			{
				if (pbResponse -> append (*pdNptStartTime) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (pbResponse -> append ("-") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbResponse -> append (*pdNptEndTime) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (bVideoRTPInfo || bAudioRTPInfo)
	{
		if (pbResponse -> append ("RTP-Info: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (bVideoRTPInfo)
		{
			if (pbResponse -> append ("url=") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbResponse -> append (pVideoRequestURI) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbResponse -> append (";seq=") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbResponse -> append (ulVideoSequenceNumber) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbResponse -> append (";rtptime=") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbResponse -> append (ulVideoTimeStamp) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (bAudioRTPInfo)
			{
				if (pbResponse -> append (", ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}
		}

		if (bAudioRTPInfo)
		{
			if (pbResponse -> append ("url=") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbResponse -> append (pAudioRequestURI) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbResponse -> append (";seq=") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbResponse -> append (ulAudioSequenceNumber) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbResponse -> append (";rtptime=") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbResponse -> append (ulAudioTimeStamp) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPUtility:: parsePLAYResponse (
	const char *pRTSPResponse,
	const char *pVideoControl,
	const char *pAudioControl,

	unsigned long *pulStatusCode,
	unsigned long *pulSequenceNumber,
	unsigned long *pulStartVideoSequenceNumber,
	unsigned long *ulStartVideoTimeStamp,
	unsigned long *pulStartAudioSequenceNumber,
	unsigned long *ulStartAudioTimeStamp,

	Tracer_p ptTracer)

{

	const char						*pPointerToRTSPResponse;
	StringTokenizer_t				stSpaceTokenizer;
	const char						*pToken;
	Boolean_t						bResponseFinished;
	Error_t							errNextToken;


	*pulStatusCode						= 0;

	if (pulSequenceNumber != (unsigned long *) NULL)
		*pulSequenceNumber					= 0;

	*pulStartVideoSequenceNumber		= 0;
	*ulStartVideoTimeStamp				= 0;
	*pulStartAudioSequenceNumber		= 0;
	*ulStartAudioTimeStamp				= 0;
	bResponseFinished			= false;


	pPointerToRTSPResponse		= pRTSPResponse;

	while (!bResponseFinished)
	{
		if (*pPointerToRTSPResponse == '\r' ||	// means "\r\n\r\n"
			strchr (pPointerToRTSPResponse, '\r') == (char *) NULL)
		{
			bResponseFinished				= true;

			continue;
		}

		if (stSpaceTokenizer. init (pPointerToRTSPResponse,
			(long) (strchr (pPointerToRTSPResponse, '\r') -
			pPointerToRTSPResponse), " ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pPointerToRTSPResponse		= strchr (pPointerToRTSPResponse, '\n') + 1;

		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!strcmp (pToken, RTSP_PREDEFINEDRTSPVERSION))
		{
			// 'RTSP/1.0 200 OK
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*pulStatusCode			= atol (pToken);
		}
		else if (!strcmp (pToken, "CSeq:"))
		{
			// 'CSeq: <SequenceNumber>'
			// SequenceNumber
			if (pulSequenceNumber != (unsigned long *) NULL)
			{
				if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				*pulSequenceNumber				= atol (pToken);
			}
		}
		else if (!strcmp (pToken, "RTP-Info:"))
		{
			// RTP-Info: url=rtsp://10.214.138.1/un/1.3gp/dynstream/2;seq=1;rtptime=180000, url=rtsp://10.214.138.1/un/1.3gp/dynstream/1;seq=1;rtptime=180000

			Boolean_t					bAreURLFinished;
			Boolean_t					bVideoURLFound;
			Boolean_t					bIsSeqParameter;

			bAreURLFinished			= false;

			while (!bAreURLFinished)
			{
				if ((errNextToken =
					stSpaceTokenizer. nextToken (&pToken, "=")) != errNoError)
				{
					if ((long) errNextToken !=
						TOOLS_STRINGTOKENIZER_NOMORETOKEN)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
					else
					{
						bAreURLFinished			= true;

						continue;
					}
				}

				if (stSpaceTokenizer. nextToken (&pToken, ";") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (strstr (pToken, pVideoControl) != (char *) NULL)
				{
					bVideoURLFound			= true;
				}
				else if (strstr (pToken, pAudioControl) != (char *) NULL)
				{
					bVideoURLFound			= false;
				}
				else
				{
					Error err = RTSPErrors (__FILE__, __LINE__,
						RTSP_RTSPUTILITY_WRONGRTPINFO,
						1, pToken);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (stSpaceTokenizer. nextToken (&pToken, "=") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (!strcmp (pToken, "seq"))
					bIsSeqParameter			= true;
				else if (!strcmp (pToken, "rtptime"))
					bIsSeqParameter			= false;
				else
				{
					Error err = RTSPErrors (__FILE__, __LINE__,
						RTSP_RTSPUTILITY_WRONGRTPINFO,
						1, pToken);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (stSpaceTokenizer. nextToken (&pToken, ";") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (bVideoURLFound)
				{
					if (bIsSeqParameter)
						*pulStartVideoSequenceNumber	= atol (pToken);
					else
						*ulStartVideoTimeStamp			= atol (pToken);
				}
				else
				{
					if (bIsSeqParameter)
						*pulStartAudioSequenceNumber	= atol (pToken);
					else
						*ulStartAudioTimeStamp			= atol (pToken);
				}

				if (stSpaceTokenizer. nextToken (&pToken, "=") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (!strcmp (pToken, "seq"))
					bIsSeqParameter			= true;
				else if (!strcmp (pToken, "rtptime"))
					bIsSeqParameter			= false;
				else
				{
					Error err = RTSPErrors (__FILE__, __LINE__,
						RTSP_RTSPUTILITY_WRONGRTPINFO,
						1, pToken);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (stSpaceTokenizer. nextToken (&pToken, ",") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (bVideoURLFound)
				{
					if (bIsSeqParameter)
						*pulStartVideoSequenceNumber	= atol (pToken);
					else
						*ulStartVideoTimeStamp			= atol (pToken);
				}
				else
				{
					if (bIsSeqParameter)
						*pulStartAudioSequenceNumber	= atol (pToken);
					else
						*ulStartAudioTimeStamp			= atol (pToken);
				}
			}
		}
		else if (!strcmp (pToken, "Date:"))
		{
			// 'Date: Thu, 14 Apr 2005 15:11:42 GMT'
			// Date
			/*
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			*/
		}
		else if (!strcmp (pToken, "Server:"))
		{
			// 'Server: <Server>'
			// Server
			/*
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bURL. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			*/
		}
		else
		{
			;
		}

		if (stSpaceTokenizer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// check for PLAY response
	{
		if (*pulStatusCode == 0)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_DESCRIBERESPONSEWRONG,
				1, pRTSPResponse);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error RTSPUtility:: parsePAUSERequest (
	const char *pRTSPRequest,

	Buffer_p pbRTSPVersion,
	Buffer_p pbURI,
	long *plSequenceNumber,
	char *pTimestamp,
	char *pSessionIdentifier,
	unsigned long *pulRequestLength,
	Tracer_p ptTracer)

{

	const char						*pPointerToRTSPRequest;
	StringTokenizer_t				stSpaceTokenizer;
	const char						*pToken;
	// Boolean_t						bRTSPVersionRead;
	Boolean_t						bRequestFinished;
	// Error							errNextToken;


	*plSequenceNumber				= -1;
	strcpy (pTimestamp, "");
	strcpy (pSessionIdentifier, "");
	*pulRequestLength				= 0;

	if (pbURI != (Buffer_p) NULL)
	{
		if (pbURI -> setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbRTSPVersion != (Buffer_p) NULL)
	{
		if (pbRTSPVersion -> setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// bRTSPVersionRead				= false;
	bRequestFinished				= false;

	pPointerToRTSPRequest		= pRTSPRequest;

	while (!bRequestFinished)
	{
		if (*pPointerToRTSPRequest == '\r' ||	// means "\r\n\r\n"
			strchr (pPointerToRTSPRequest, '\r') == (char *) NULL)
		{
			bRequestFinished				= true;

			*pulRequestLength			= (unsigned long)
				(pPointerToRTSPRequest - pRTSPRequest + 2);

			continue;
		}

		if (stSpaceTokenizer. init (pPointerToRTSPRequest,
			(long) (strchr (pPointerToRTSPRequest, '\r') -
			pPointerToRTSPRequest), " ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pPointerToRTSPRequest		= strchr (pPointerToRTSPRequest, '\n') + 1;

		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!strcmp (pToken, "PAUSE"))
		{
			// 'PAUSE <Request-URI> <RTSP-Version>'
			// Request-URI
			if (pbURI != (Buffer_p) NULL ||
				pbRTSPVersion != (Buffer_p) NULL)
			{
				if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pbURI != (Buffer_p) NULL)
				{
					if (pbURI -> setBuffer (pToken) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_SETBUFFER_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}

				// <RTSP-Version>
				if (pbRTSPVersion != (Buffer_p) NULL)
				{
					if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbRTSPVersion -> setBuffer (pToken) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_SETBUFFER_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
			}

			// bRTSPVersionRead				= true;
		}
		else if (!strcmp (pToken, "CSeq:"))
		{
			// 'CSeq: <SequenceNumber>'
			// SequenceNumber
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*plSequenceNumber				= atol (pToken);
		}
		else if (!strcmp (pToken, "Timestamp:"))
		{
			// 'Timestamp: <Timestamp>'
			// Timestamp
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (pTimestamp, pToken);
		}
		else if (!strcmp (pToken, "Session:"))
		{
			// 'Session: <SessionIdentifier>[;timeout=...]'
			// SessionIdentifier
			if (stSpaceTokenizer. nextToken (&pToken, ";") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
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
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// check for PAUSE request
	{
		if (*plSequenceNumber == -1 || !strcmp (pSessionIdentifier, ""))
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_PAUSEREQUESTWRONG,
				1, pRTSPRequest);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error RTSPUtility:: getPAUSEResponse (
	unsigned long ulSequenceNumber,
	const char *pSessionIdentifier,
	unsigned long ulRTSP_RTCPTimeoutInSecs,
	const char *pTimestamp,

	Buffer_p pbResponse,
	Tracer_p ptTracer)

{

	if (getRTSPResponse (RTSP_PREDEFINEDRTSPVERSION, 200, ulSequenceNumber,
		pSessionIdentifier, ulRTSP_RTCPTimeoutInSecs, false, pTimestamp,
		pbResponse, ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETRTSPRESPONSE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPUtility:: getTEARDOWNRequest (
	const char *pURL,
	unsigned long ulSequenceNumber,
	const char *pSession,
	const char *pUserAgent,
	Buffer_p pbRTSPRequest,
	Tracer_p ptTracer)

{

	if (pbRTSPRequest -> setBuffer ("TEARDOWN ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (strchr (pURL, '?') != (char *) NULL)
	{
		if (pbRTSPRequest -> append (pURL,
			strchr (pURL, '?') - pURL) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		if (pbRTSPRequest -> append (pURL) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbRTSPRequest -> append (" " RTSP_PREDEFINEDRTSPVERSION RTSP_NEWLINE) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append ("CSeq: ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (ulSequenceNumber) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (RTSP_NEWLINE "Session: ") !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (pSession) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (RTSP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pUserAgent != (const char *) NULL)
	{
		if (pbRTSPRequest -> append ("User-Agent: ") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbRTSPRequest -> append (pUserAgent) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbRTSPRequest -> append (RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbRTSPRequest -> append (RTSP_NEWLINE) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPUtility:: parseTEARDOWNRequest (
	const char *pRTSPRequest,

	Buffer_p pbRTSPVersion,
	Buffer_p pbURI,
	long *plSequenceNumber,
	char *pTimestamp,
	char *pSessionIdentifier,
	unsigned long *pulRequestLength,
	Tracer_p ptTracer)

{

	const char						*pPointerToRTSPRequest;
	StringTokenizer_t				stSpaceTokenizer;
	const char						*pToken;
	// Boolean_t						bRTSPVersionRead;
	Boolean_t						bRequestFinished;
	// Error							errNextToken;


	*plSequenceNumber				= -1;
	strcpy (pTimestamp, "");
	strcpy (pSessionIdentifier, "");
	*pulRequestLength				= 0;

	if (pbURI != (Buffer_p) NULL)
	{
		if (pbURI -> setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbRTSPVersion != (Buffer_p) NULL)
	{
		if (pbRTSPVersion -> setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// bRTSPVersionRead				= false;
	bRequestFinished				= false;

	pPointerToRTSPRequest		= pRTSPRequest;

	while (!bRequestFinished)
	{
		if (*pPointerToRTSPRequest == '\r' ||	// means "\r\n\r\n"
			strchr (pPointerToRTSPRequest, '\r') == (char *) NULL)
		{
			bRequestFinished				= true;

			*pulRequestLength			= (unsigned long)
				(pPointerToRTSPRequest - pRTSPRequest + 2);

			continue;
		}

		if (stSpaceTokenizer. init (pPointerToRTSPRequest,
			(long) (strchr (pPointerToRTSPRequest, '\r') -
			pPointerToRTSPRequest), " ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pPointerToRTSPRequest		= strchr (pPointerToRTSPRequest, '\n') + 1;

		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!strcmp (pToken, "TEARDOWN"))
		{
			// 'TEARDOWN <Request-URI> <RTSP-Version>'
			// Request-URI
			if (pbURI != (Buffer_p) NULL ||
				pbRTSPVersion != (Buffer_p) NULL)
			{
				if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pbURI != (Buffer_p) NULL)
				{
					if (pbURI -> setBuffer (pToken) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_SETBUFFER_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}

			// <RTSP-Version>
				if (pbRTSPVersion != (Buffer_p) NULL)
				{
					if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbRTSPVersion -> setBuffer (pToken) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_SETBUFFER_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
			}

			// bRTSPVersionRead				= true;
		}
		else if (!strcmp (pToken, "CSeq:"))
		{
			// 'CSeq: <SequenceNumber>'
			// SequenceNumber
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*plSequenceNumber				= atol (pToken);
		}
		else if (!strcmp (pToken, "Timestamp:"))
		{
			// 'Timestamp: <Timestamp>'
			// Timestamp
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (pTimestamp, pToken);
		}
		else if (!strcmp (pToken, "Session:"))
		{
			// 'Session: <SessionIdentifier>[;timeout=...]'
			// SessionIdentifier
			if (stSpaceTokenizer. nextToken (&pToken, ";") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
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
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// check for TEARDOWN request
	{
		if (*plSequenceNumber == -1)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_TEARDOWNREQUESTWRONG,
				1, pRTSPRequest);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error RTSPUtility:: getTEARDOWNResponse (
	unsigned long ulSequenceNumber,
	const char *pSessionIdentifier,
	unsigned long ulRTSP_RTCPTimeoutInSecs,
	const char *pTimestamp,

	Buffer_p pbResponse,
	Tracer_p ptTracer)

{

	if (getRTSPResponse (
		RTSP_PREDEFINEDRTSPVERSION, 200, ulSequenceNumber,
		(const char *) NULL, ulRTSP_RTCPTimeoutInSecs, false,
		pTimestamp, pbResponse, ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETRTSPRESPONSE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPUtility:: parseTEARDOWNResponse (
	const char *pRTSPResponse,

	unsigned long *pulStatusCode,
	unsigned long *pulSequenceNumber,

	Tracer_p ptTracer)

{

	const char						*pPointerToRTSPResponse;
	StringTokenizer_t				stSpaceTokenizer;
	const char						*pToken;
	Boolean_t						bResponseFinished;
	// Error_t							errNextToken;


	*pulStatusCode						= 0;

	if (pulSequenceNumber != (unsigned long *) NULL)
		*pulSequenceNumber					= 0;

	bResponseFinished			= false;


	pPointerToRTSPResponse		= pRTSPResponse;

	while (!bResponseFinished)
	{
		if (*pPointerToRTSPResponse == '\r' ||	// means "\r\n\r\n"
			strchr (pPointerToRTSPResponse, '\r') == (char *) NULL)
		{
			bResponseFinished				= true;

			continue;
		}

		if (stSpaceTokenizer. init (pPointerToRTSPResponse,
			(long) (strchr (pPointerToRTSPResponse, '\r') -
			pPointerToRTSPResponse), " ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pPointerToRTSPResponse		= strchr (pPointerToRTSPResponse, '\n') + 1;

		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!strcmp (pToken, RTSP_PREDEFINEDRTSPVERSION))
		{
			// 'RTSP/1.0 200 OK
			if (stSpaceTokenizer. nextToken (&pToken, "") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*pulStatusCode			= atol (pToken);
		}
		else if (!strcmp (pToken, "CSeq:"))
		{
			// 'CSeq: <SequenceNumber>'
			// SequenceNumber
			if (pulSequenceNumber != (unsigned long *) NULL)
			{
				if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				*pulSequenceNumber				= atol (pToken);
			}
		}
		else
		{
			;
		}

		if (stSpaceTokenizer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// check for TEARDOWN response
	{
		if (*pulStatusCode == 0)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_DESCRIBERESPONSEWRONG,
				1, pRTSPResponse);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error RTSPUtility:: getOPTIONSRequest (
	const char *pURL,
	unsigned long ulSequenceNumber,
	const char *pUserAgent,
	Buffer_p pbRTSPRequest,
	Tracer_p ptTracer)

{

	if (pbRTSPRequest -> setBuffer ("OPTIONS ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (strchr (pURL, '?') != (char *) NULL)
	{
		if (pbRTSPRequest -> append (pURL,
			strchr (pURL, '?') - pURL) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		if (pbRTSPRequest -> append (pURL) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbRTSPRequest -> append (" " RTSP_PREDEFINEDRTSPVERSION RTSP_NEWLINE) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append ("CSeq: ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (ulSequenceNumber) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbRTSPRequest -> append (RTSP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pUserAgent != (const char *) NULL)
	{
		if (pbRTSPRequest -> append ("User-Agent: ") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbRTSPRequest -> append (pUserAgent) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbRTSPRequest -> append (RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbRTSPRequest -> append (RTSP_NEWLINE) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSPUtility:: parseOPTIONSRequest (
	const char *pRTSPRequest,

	Buffer_p pbRTSPVersion,
	Buffer_p pbURI,
	long *plSequenceNumber,
	char *pTimestamp,
	char *pSessionIdentifier,
	unsigned long *pulRequestLength,
	Tracer_p ptTracer)

{

	const char						*pPointerToRTSPRequest;
	StringTokenizer_t				stSpaceTokenizer;
	const char						*pToken;
	// Boolean_t						bRTSPVersionRead;
	Boolean_t						bRequestFinished;
	// Error							errNextToken;


	*plSequenceNumber				= -1;
	strcpy (pTimestamp, "");
	strcpy (pSessionIdentifier, "");
	*pulRequestLength				= 0;

	if (pbURI != (Buffer_p) NULL)
	{
		if (pbURI -> setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbRTSPVersion != (Buffer_p) NULL)
	{
		if (pbRTSPVersion -> setBuffer ("") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// bRTSPVersionRead				= false;
	bRequestFinished				= false;

	pPointerToRTSPRequest		= pRTSPRequest;

	while (!bRequestFinished)
	{
		if (*pPointerToRTSPRequest == '\r' ||	// means "\r\n\r\n"
			strchr (pPointerToRTSPRequest, '\r') == (char *) NULL)
		{

			bRequestFinished				= true;

			*pulRequestLength			= (unsigned long)
				(pPointerToRTSPRequest - pRTSPRequest + 2);

			continue;
		}

		if (stSpaceTokenizer. init (pPointerToRTSPRequest,
			(long) (strchr (pPointerToRTSPRequest, '\r') -
			pPointerToRTSPRequest), " ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pPointerToRTSPRequest		= strchr (pPointerToRTSPRequest, '\n') + 1;

		if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (stSpaceTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!strcmp (pToken, "OPTIONS"))
		{
			// 'OPTIONS * <RTSP-Version>'
			// *
			if (pbURI != (Buffer_p) NULL ||
				pbRTSPVersion != (Buffer_p) NULL)
			{
				if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (stSpaceTokenizer. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pbURI != (Buffer_p) NULL)
				{
					if (pbURI -> setBuffer (pToken) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_SETBUFFER_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}

				// <RTSP-Version>
				if (pbRTSPVersion != (Buffer_p) NULL)
				{
					if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbRTSPVersion -> setBuffer (pToken) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_SETBUFFER_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (stSpaceTokenizer. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_STRINGTOKENIZER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
			}

			// bRTSPVersionRead				= true;
		}
		else if (!strcmp (pToken, "CSeq:"))
		{
			// 'CSeq: <SequenceNumber>'
			// SequenceNumber
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*plSequenceNumber				= atol (pToken);
		}
		else if (!strcmp (pToken, "Timestamp:"))
		{
			// 'Timestamp: <Timestamp>'
			// Timestamp
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			strcpy (pTimestamp, pToken);
		}
		else if (!strcmp (pToken, "Require:"))
		{
			// 'Require: <something>'

			/*
			if (stSpaceTokenizer. nextToken (&pToken) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			*/

			// pToken
		}
		else if (!strcmp (pToken, "Session:"))
		{
			// 'Session: <SessionIdentifier>[;timeout=...]'
			// SessionIdentifier
			if (stSpaceTokenizer. nextToken (&pToken, ";") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stSpaceTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
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
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// check for OPTIONS request
	{
		if (*plSequenceNumber == -1)
		{
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_OPTIONSREQUESTWRONG,
				1, pRTSPRequest);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error RTSPUtility:: getOPTIONSResponse (
	unsigned long ulSequenceNumber,
	const char *pSessionIdentifier,
	unsigned long ulRTSP_RTCPTimeoutInSecs,
	const char *pTimestamp,
	Boolean_t bIsPreviousRTSPCommandPAUSE,

	Buffer_p pbResponse,
	Tracer_p ptTracer)

{

	if (getRTSPResponse (
		RTSP_PREDEFINEDRTSPVERSION, 200, ulSequenceNumber,
		pSessionIdentifier, ulRTSP_RTCPTimeoutInSecs, false,
		pTimestamp, pbResponse, ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETRTSPRESPONSE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (bIsPreviousRTSPCommandPAUSE)
	{
		if (pbResponse -> append (
			"Public: OPTIONS, DESCRIBE, SETUP, PLAY, TEARDOWN"
			RTSP_NEWLINE RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		if (pbResponse -> append (
			"Public: OPTIONS, DESCRIBE, SETUP, PLAY, PAUSE, TEARDOWN"
			RTSP_NEWLINE RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	
	return errNoError;
}


Error RTSPUtility:: getRTSPResponse (const char *pVersion,
	unsigned long ulStatusCode,
	long lSequenceNumber, const char *pSessionIdentifier,
	unsigned long ulSessionTimeoutInSecs,
	Boolean_t bConnectionClose,
	const char *pTimestamp, Buffer_p pbResponse,
	Tracer_p ptTracer)

{

	char						pDate [RTSP_MAXDATELENGTH];


	if (pbResponse -> setBuffer (pVersion) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append (" ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append (ulStatusCode) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	switch (ulStatusCode)
	{
		case 100:
			if (pbResponse -> append (" Continue") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 200:
			if (pbResponse -> append (" OK") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 201:
			if (pbResponse -> append (" Created") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 250:
			if (pbResponse -> append (" Low on Storage Space") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 300:
			if (pbResponse -> append (" Multiple Choices") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 301:
			if (pbResponse -> append (" Moved Permanently") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 302:
			if (pbResponse -> append (" Moved Temporarily") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 303:
			if (pbResponse -> append (" See Other") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 304:
			if (pbResponse -> append (" Not Modified") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 305:
			if (pbResponse -> append (" Use Proxy") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 400:
			if (pbResponse -> append (" Bad Request") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 401:
			if (pbResponse -> append (" Unauthorized") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 402:
			if (pbResponse -> append (" Payment Required") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 403:
			if (pbResponse -> append (" Forbidden") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 404:
			if (pbResponse -> append (" Not Found") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 405:
			if (pbResponse -> append (" Method Not Allowed") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 406:
			if (pbResponse -> append (" Not Acceptable") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 407:
			if (pbResponse -> append (" Proxy Authentication Required") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 408:
			if (pbResponse -> append (" Request Time-out") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 410:
			if (pbResponse -> append (" Gone") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 411:
			if (pbResponse -> append (" Length Required") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 412:
			if (pbResponse -> append (" Precondition Failed") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 413:
			if (pbResponse -> append (" Request Entity Too Large") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 414:
			if (pbResponse -> append (" Request-URI Too Large") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 415:
			if (pbResponse -> append (" Unsupported Media Type") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 451:
			if (pbResponse -> append (" Parameter Not Understood") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 452:
			if (pbResponse -> append (" Conference Not Found") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 453:
			if (pbResponse -> append (" Not Enough Bandwidth") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 454:
			if (pbResponse -> append (" Session Not Found") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 455:
			if (pbResponse -> append (" Method Not Valid in This State") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pbResponse -> append ("Allow: TEARDOWN") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 456:
			if (pbResponse -> append (" Header Field Not Valid for Resource") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 457:
			if (pbResponse -> append (" Invalid Range") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 458:
			if (pbResponse -> append (" Parameter Is Read-Only") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 459:
			if (pbResponse -> append (" Aggregate operation not allowed") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 460:
			if (pbResponse -> append (" Only aggregate operation allowed") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 461:
			if (pbResponse -> append (" Unsupported transport") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 462:
			if (pbResponse -> append (" Destination unreachable") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 500:
			if (pbResponse -> append (" Internal Server Error") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 501:
			if (pbResponse -> append (" Not Implemented") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 502:
			if (pbResponse -> append (" Bad Gateway") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 503:
			if (pbResponse -> append (" Service Unavailable") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 504:
			if (pbResponse -> append (" Gateway Time-out") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 505:
			if (pbResponse -> append (" RTSP Version not supported") !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 551:
			if (pbResponse -> append (" Option not supported") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		default:
			Error err = RTSPErrors (__FILE__, __LINE__,
				RTSP_RTSPUTILITY_RTSPSTATUSCODEWRONG,
				1, ulStatusCode);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
	}

	if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getDateStr (pDate, ptTracer) != errNoError)
	{
		Error err = RTSPErrors (__FILE__, __LINE__,
			RTSP_RTSPUTILITY_GETDATESTR_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (lSequenceNumber != -1)
	{
		if (pbResponse -> append ("CSeq: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append (lSequenceNumber) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// Date parameter
	{
		if (pbResponse -> append ("Date: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append (pDate) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (strcmp (pTimestamp, ""))
	{
		if (pbResponse -> append ("Timestamp: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append (pTimestamp) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pSessionIdentifier != (const char *) NULL)
	{
		if (pbResponse -> append ("Session: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append (pSessionIdentifier) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		/**/
		if (pbResponse -> append (";timeout=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append (ulSessionTimeoutInSecs) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
		/**/

		if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbResponse -> append ("Server: CatraStreamingServer") !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// Expires parameter
	/*
	{
		if (pbResponse -> append ("Expires: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append (pDate) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	*/

	if (bConnectionClose)
	{
		if (pbResponse -> append ("Connection: Close") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbResponse -> append (RTSP_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error RTSPUtility:: getDateStr (
	char pDate [RTSP_MAXDATELENGTH],
	Tracer_p ptTracer)

{

	tm					tmDateTime;
	unsigned long		ulMilliSecs;
	char				pLocalDate [RTSP_MAXDATELENGTH];


	if (DateTime:: get_tm_LocalTime (&tmDateTime,
		&ulMilliSecs) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_LOCALTIME_R_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	switch (tmDateTime. tm_wday)
	{
		case 0:
			strcpy (pDate, "Sun");

			break;
		case 1:
			strcpy (pDate, "Mon");

			break;
		case 2:
			strcpy (pDate, "Tue");

			break;
		case 3:
			strcpy (pDate, "Wed");

			break;
		case 4:
			strcpy (pDate, "Thu");

			break;
		case 5:
			strcpy (pDate, "Fri");

			break;
		case 6:
			strcpy (pDate, "Sat");

			break;
		default:
			;

			// write error
	}

	strcat (pDate, ", ");

	sprintf (pLocalDate, "%02d", tmDateTime. tm_mday);

	strcat (pDate, pLocalDate);

	strcat (pDate, " ");

	switch (tmDateTime. tm_mon)
	{
		case 0:
			strcat (pDate, "Jan");

			break;
		case 1:
			strcat (pDate, "Feb");

			break;
		case 2:
			strcat (pDate, "Mar");

			break;
		case 3:
			strcat (pDate, "Apr");

			break;
		case 4:
			strcat (pDate, "May");

			break;
		case 5:
			strcat (pDate, "Jun");

			break;
		case 6:
			strcat (pDate, "Jul");

			break;
		case 7:
			strcat (pDate, "Aug");

			break;
		case 8:
			strcat (pDate, "Sep");

			break;
		case 9:
			strcat (pDate, "Oct");

			break;
		case 10:
			strcat (pDate, "Nov");

			break;
		case 11:
			strcat (pDate, "Dec");

			break;
		default:
			;

			// write error
	}

	sprintf (pLocalDate, "%04d %02d:%02d:%02d GMT",
		tmDateTime. tm_year + 1900,
		tmDateTime. tm_hour,
		tmDateTime. tm_min,
		tmDateTime. tm_sec);

	strcat (pDate, " ");

	strcat (pDate, pLocalDate);


	return errNoError;
}


