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

#include "SDPFor3GPP.h"
#include "SDPMediaFor3GPP.h"
#include "Convert.h"
#include <assert.h>
#include <stdio.h>
#ifdef WIN32
	#include <windows.h>
#else
	#include <sys/time.h>
	#include <netinet/in.h>
#endif




SDPFor3GPP:: SDPFor3GPP (void): SDPForRTSP ()

{

}


SDPFor3GPP:: ~SDPFor3GPP (void)

{

}



SDPFor3GPP:: SDPFor3GPP (const SDPFor3GPP &)

{

	assert (1==0);

	// to do

}


SDPFor3GPP &SDPFor3GPP:: operator = (const SDPFor3GPP &)

{

	assert (1==0);

	// to do

	return *this;

}


Error SDPFor3GPP:: buildSDPMedia (SDPMedia_p *psmCurrentSDPMedia)

{

	if (((*psmCurrentSDPMedia) = new SDPMediaFor3GPP_t) ==
		(SDPMedia_p) NULL)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error SDPFor3GPP:: appendToBuffer (Buffer_p pbSDP)

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

	if (_psaControlSDPAttribute != (SDPAttribute_p) NULL)
	{
		if (_psaControlSDPAttribute -> appendToBuffer (pbSDP) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPATTRIBUTE_APPENDTOBUFFER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (_psaRangeSDPAttribute != (SDPAttribute_p) NULL)
	{
		if (_psaRangeSDPAttribute -> appendToBuffer (pbSDP) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPATTRIBUTE_APPENDTOBUFFER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
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


Error SDPFor3GPP:: getSDPFromMediaInfo (
	MP4File_p pmfFile,
	const char *pOriginAddress,
	MP4TrackInfo_p pmtiMediaVideoTrackInfo,
	unsigned long ulVideoAvgBitRate,
	unsigned long ulVideoRTPTimeScale,
	unsigned char ucVideoSystemsProfileLevel,
	MP4TrackInfo_p pmtiMediaAudioTrackInfo,
	unsigned long ulAudioAvgBitRate,
	unsigned long ulAudioRTPTimeScale,
	unsigned char ucChannels,
	MP4Atom:: Standard_t sStandard,
	Buffer_p pbSDP,
	unsigned long *pulVideoPayloadNumber,
	unsigned long *pulAudioPayloadNumber,
	Tracer_p ptTracer)

{

	Buffer_t				bLocalSDP;
	SDPFor3GPP_t			sSDPFor3GPP;
	unsigned long			pulFreePayloadsNumber [2];
	char					pVideoPayloadNumber [SDP_MAXLONGLENGTH];
	char					pAudioPayloadNumber [SDP_MAXLONGLENGTH];


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
		char				pNow [SDP_MAXLONGLENGTH];
		

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

		if (pmtiMediaVideoTrackInfo != (MP4TrackInfo_p) NULL)
			ulAvgBitRateInKbps			+= (ulVideoAvgBitRate / 1000);

		if (pmtiMediaAudioTrackInfo != (MP4TrackInfo_p) NULL)
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
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_NEW_FAILED);
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
			pmtiMediaVideoTrackInfo,
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
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_NEW_FAILED);
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

	if (bLocalSDP. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sSDPFor3GPP. finish () != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
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

		return err;
	}

	if (pmtiMediaVideoTrackInfo != (MP4TrackInfo_p) NULL &&
		pmtiMediaAudioTrackInfo != (MP4TrackInfo_p) NULL)
	{
		sprintf (pVideoPayloadNumber, "%lu", pulFreePayloadsNumber [0]);
		*pulVideoPayloadNumber		= pulFreePayloadsNumber [0];
		sprintf (pAudioPayloadNumber, "%lu", pulFreePayloadsNumber [1]);
		*pulAudioPayloadNumber		= pulFreePayloadsNumber [1];
	}
	else if (pmtiMediaVideoTrackInfo != (MP4TrackInfo_p) NULL)
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

	if (pmtiMediaVideoTrackInfo != (MP4TrackInfo_p) NULL)
	{
		SDPMedia_p				pSDPVideoMedia;


		if (sStandard == MP4Atom:: MP4F_3GPP)
			pSDPVideoMedia = new SDPMediaFor3GPP_t;
		else
			pSDPVideoMedia = new SDPMediaForRTSP_t;

		if (pSDPVideoMedia == (SDPMedia_p) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (SDPMediaFor3GPP:: getSDPMediaVideoFromMediaInfo (
			pmtiMediaVideoTrackInfo,
			(MP4TrackInfo_p) NULL,
			ulVideoAvgBitRate,
			ulVideoRTPTimeScale,
			ucVideoSystemsProfileLevel,
			pVideoPayloadNumber,
			sStandard,
			ptTracer,
			pSDPVideoMedia) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIAFOR3GPP_GETSDPMEDIAVIDEOFROMMEDIAINFO_FAILED);
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

			return err;
		}

		if (sSDPFor3GPP. addSDPMedia (pSDPVideoMedia) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_ADDSDPMEDIA_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPVideoMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPVideoMedia;

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (pmtiMediaAudioTrackInfo != (MP4TrackInfo_p) NULL)
	{
		SDPMedia_p				pSDPAudioMedia;


		if (sStandard == MP4Atom:: MP4F_3GPP)
			pSDPAudioMedia = new SDPMediaFor3GPP_t;
		else
			pSDPAudioMedia = new SDPMediaForRTSP_t;

		if (pSDPAudioMedia == (SDPMedia_p) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (SDPMediaFor3GPP:: getSDPMediaAudioFromMediaInfo (
			pmtiMediaAudioTrackInfo,
			(MP4TrackInfo_p) NULL,
			ulAudioAvgBitRate,
			ulAudioRTPTimeScale,
			pAudioPayloadNumber,
			ucChannels,
			sStandard,
			ptTracer,
			pSDPAudioMedia) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIAFOR3GPP_GETSDPMEDIAAUDIOFROMMEDIAINFO_FAILED);
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

			return err;
		}

		if (sSDPFor3GPP. addSDPMedia (pSDPAudioMedia) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_ADDSDPMEDIA_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPAudioMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPAudioMedia;

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
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


	return errNoError;
}


Error SDPFor3GPP:: getMediaInfoFromSDPFile (
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


