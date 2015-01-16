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

#include "SDPMediaFor3GPP.h"
#include "Convert.h"
#include <assert.h>
#include <stdio.h>
#ifdef WIN32
	#include <windows.h>
#else
	#include <netinet/in.h>
#endif



SDPMediaFor3GPP:: SDPMediaFor3GPP (void): SDPMediaForRTSP ()

{

}


SDPMediaFor3GPP:: ~SDPMediaFor3GPP (void)

{

}



SDPMediaFor3GPP:: SDPMediaFor3GPP (const SDPMediaFor3GPP &)

{

	assert (1==0);

	// to do

}


SDPMediaFor3GPP &SDPMediaFor3GPP:: operator = (const SDPMediaFor3GPP &)

{

	assert (1==0);

	// to do

	return *this;

}


Error SDPMediaFor3GPP:: appendToBuffer (Buffer_p pbSDP)

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

	if (_psaSDPRtpMapAttribute != (SDPRtpMapAttribute_p) NULL)
	{
		if (_psaSDPRtpMapAttribute -> appendToBuffer (pbSDP) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPATTRIBUTE_APPENDTOBUFFER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (_psaSDPFmtpAttribute != (SDPFmtpAttribute_p) NULL)
	{
		if (_psaSDPFmtpAttribute -> appendToBuffer (pbSDP) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPATTRIBUTE_APPENDTOBUFFER_FAILED);
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
			psaSDPAttribute            = *it;

			if (psaSDPAttribute == _psaSDPRtpMapAttribute ||
				psaSDPAttribute == _psaSDPFmtpAttribute ||
				psaSDPAttribute == _psaControlSDPAttribute)
				continue;

			// some specific codec need more information to add to the SDP
			// and, in this way, we add other information respect
			// to the common one.
			// For example, for H263 it is necessary the attribute
			//		a=framesize:96 128-96
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


	return errNoError;
}


/* 3GPP per Quicktime
Error SDPMediaFor3GPP:: appendToBuffer (Buffer_p pbSDP)

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

	if (_psaControlSDPAttribute != (SDPAttribute_p) NULL)
	{
		char		*p = strchr (_psaControlSDPAttribute -> _pValue, '/');

		*p   = '=';
		if (_psaControlSDPAttribute -> appendToBuffer (pbSDP) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPATTRIBUTE_APPENDTOBUFFER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pbSDP -> append ("a=mpeg4-esid:");
		pbSDP -> append (p + 1);
		pbSDP -> append (SDP_NEWLINE);
	}

	if (_psaSDPRtpMapAttribute != (SDPRtpMapAttribute_p) NULL)
	{
		if (_psaSDPRtpMapAttribute -> appendToBuffer (pbSDP) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPATTRIBUTE_APPENDTOBUFFER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (_psaSDPFmtpAttribute != (SDPFmtpAttribute_p) NULL)
	{
			if (pbSDP -> append (SDP_ATTRIBUTE) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append ("=") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (_psaSDPFmtpAttribute -> _pName) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (":") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (_psaSDPFmtpAttribute -> _pFormat) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (" ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			Buffer bbb;
			bbb. init (_psaSDPFmtpAttribute -> _pFormatParameters);
			bbb. substitute (";", "; ");
			if (pbSDP -> append ((const char *) bbb) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			pbSDP -> append (";");

			if (pbSDP -> append (SDP_NEWLINE) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

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

		pbSDP -> append (";");

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
			psaSDPAttribute            = *it;

			if (psaSDPAttribute == _psaSDPRtpMapAttribute ||
				psaSDPAttribute == _psaSDPFmtpAttribute ||
				psaSDPAttribute == _psaControlSDPAttribute)
				continue;

			// some specific codec need more information to add to the SDP
			// and, in this way, we add other information respect
			// to the common one.
			// For example, for H263 it is necessary the attribute
			//		a=framesize:96 128-96
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
*/


Error SDPMediaFor3GPP:: getSDPMediaVideoFromMediaInfo (
	MP4TrackInfo_p pmtiMediaVideoTrackInfo,
	MP4TrackInfo_p pmtiHintVideoTrackInfo,
	unsigned long ulVideoAvgBitRate,
	unsigned long ulVideoRTPTimeScale,
	unsigned char ucVideoSystemsProfileLevel,
	const char *pVideoPayloadNumber,
	MP4Atom:: Standard_t sStandard,
	Tracer_p ptTracer,
	SDPMedia_p pSDPVideoMedia)

{

	Buffer_t				bLocalSDP;
	char					pVideoLocalPayloadNumber [SDP_MAXLONGLENGTH];


	if (bLocalSDP. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// video SDP atom
	{
		Error_t					errGetSdpAtom;
		MP4SdpAtom_p				pmaSdpAtom;


		if ((errGetSdpAtom = pmtiMediaVideoTrackInfo ->
			getSdpAtom (&pmaSdpAtom, true)) != errNoError)
		{
			if ((long) errGetSdpAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSDPATOM_FAILED);
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
		else
		{
			if (pmaSdpAtom -> getSdpText (&bLocalSDP) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4SDPATOM_GETSDPTEXT_FAILED);
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

	if (pSDPVideoMedia -> init (
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

	if (strstr ((const char *) bLocalSDP, "m=") == (char *) NULL)
	{
		if (pSDPVideoMedia -> setMediaType (
			SDPMedia:: SDPMEDIA_VIDEO) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_SETMEDIATYPE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPVideoMedia -> finish () != errNoError)
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

			if (pSDPVideoMedia -> finish () != errNoError)
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

			if (pSDPVideoMedia -> finish () != errNoError)
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

			if (pSDPVideoMedia -> finish () != errNoError)
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

		strcpy (pVideoLocalPayloadNumber, pVideoPayloadNumber);
	}
	else
	{
		if (pSDPVideoMedia -> getFmtList (pVideoLocalPayloadNumber) !=
			errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_GETFMTLIST_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPVideoMedia -> finish () != errNoError)
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

			if (pSDPVideoMedia -> finish () != errNoError)
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

			if (pSDPVideoMedia -> finish () != errNoError)
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
		MP4Atom:: MP4Codec_t		cVideoCodecUsed;


		if (pmtiMediaVideoTrackInfo -> getCodec (
			&cVideoCodecUsed) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETCODEC_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPVideoMedia -> finish () != errNoError)
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

		if ((psaSDPRtpMapAttribute = new SDPRtpMapAttribute_t) ==
			(SDPRtpMapAttribute_p) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPVideoMedia -> finish () != errNoError)
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
			pVideoLocalPayloadNumber);

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

			if (pSDPVideoMedia -> finish () != errNoError)
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
			ulVideoRTPTimeScale);

		strcpy (psaSDPRtpMapAttribute -> _pEncodingParameters, "");

		if (pSDPVideoMedia -> addAttribute (
			psaSDPRtpMapAttribute) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_ADDATTRIBUTE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete psaSDPRtpMapAttribute;

			if (pSDPVideoMedia -> finish () != errNoError)
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

	if (pmtiHintVideoTrackInfo != (MP4TrackInfo_p) NULL &&
		strstr ((const char *) bLocalSDP, "a=mpeg4-esid:") ==
		(char *) NULL)
	{
		SDPAttribute_p			psaSDPAttribute;
		unsigned long			ulTrackIdentifier;


		if ((psaSDPAttribute = new SDPAttribute_t) ==
			(SDPAttribute_p) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPVideoMedia -> finish () != errNoError)
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

		if (pmtiMediaVideoTrackInfo ->
			getTrackIdentifier (&ulTrackIdentifier) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETTRACKIDENTIFIER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete psaSDPAttribute;

			if (pSDPVideoMedia -> finish () != errNoError)
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

			if (pSDPVideoMedia -> finish () != errNoError)
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
		MP4Atom:: MP4Codec_t		cVideoCodecUsed;


		if (pmtiMediaVideoTrackInfo -> getCodec (
			&cVideoCodecUsed) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETCODEC_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPVideoMedia -> finish () != errNoError)
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

		if ((psaSDPFmtpAttribute = new SDPFmtpAttribute_t) ==
			(SDPFmtpAttribute_p) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPVideoMedia -> finish () != errNoError)
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

		strcpy (psaSDPFmtpAttribute -> _pFormat, pVideoLocalPayloadNumber);

		if (cVideoCodecUsed == MP4Atom:: MP4F_CODEC_MPEG4)
		{
			unsigned char				*pucESConfiguration;
			#ifdef WIN32
				__int64						ullESConfigurationSize;
			#else
				unsigned long long			ullESConfigurationSize;
			#endif
			char						*pESConfigToBase16;
			MP4Mp4vAtom_p				pmaMp4vAtom;
			unsigned long				ulWidth;
			unsigned long				ulHeight;


			if (pmtiMediaVideoTrackInfo -> getMp4vAtom (
				&pmaMp4vAtom, true) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETMP4VATOM_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPFmtpAttribute;

				if (pSDPVideoMedia -> finish () != errNoError)
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

				if (pSDPVideoMedia -> finish () != errNoError)
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

				if (pSDPVideoMedia -> finish () != errNoError)
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

			if (pmtiMediaVideoTrackInfo ->
				getPointerTrackESConfiguration (
				&pucESConfiguration, &ullESConfigurationSize,
				sStandard) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETPOINTERTRACKESCONFIGURATION_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPFmtpAttribute;

				if (pSDPVideoMedia -> finish () != errNoError)
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
				if ((pESConfigToBase16 = new char [
					(unsigned int) (ullESConfigurationSize * 2 + 1)]) ==
					(char *) NULL)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UTILITY_NOPAYLOADAVAILABLE);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (pSDPVideoMedia -> finish () != errNoError)
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

					if (pSDPVideoMedia -> finish () != errNoError)
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
				(unsigned long) ucVideoSystemsProfileLevel,
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


			if (pmtiMediaVideoTrackInfo -> getS263Atom (
				&pmaS263Atom, true) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETS263ATOM_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPFmtpAttribute;

				if (pSDPVideoMedia -> finish () != errNoError)
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

				if (pSDPVideoMedia -> finish () != errNoError)
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

				if (pSDPVideoMedia -> finish () != errNoError)
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

			if (pmtiMediaVideoTrackInfo -> getD263Atom (
				&pmaD263Atom, true) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETD263ATOM_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPFmtpAttribute;

				if (pSDPVideoMedia -> finish () != errNoError)
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

				if (pSDPVideoMedia -> finish () != errNoError)
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

				if (pSDPVideoMedia -> finish () != errNoError)
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
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_NEW_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (pSDPVideoMedia -> finish () != errNoError)
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
					pVideoLocalPayloadNumber, ulWidth, ulHeight);

				if (pSDPVideoMedia -> addAttribute (
					psaSDPAttribute) != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPMEDIA_ADDATTRIBUTE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPAttribute;

					delete psaSDPFmtpAttribute;

					if (pSDPVideoMedia -> finish () != errNoError)
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

			if (pSDPVideoMedia -> finish () != errNoError)
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

			if (pSDPVideoMedia -> finish () != errNoError)
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
		char					pVideoTrackName [MP4F_MAXTRACKNAMELENGTH];


		if (pmtiHintVideoTrackInfo != (MP4TrackInfo_p) NULL)
		{
			if (pmtiHintVideoTrackInfo -> getTrackName (
				pVideoTrackName) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRACKNAME_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pSDPVideoMedia -> finish () != errNoError)
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
			if (pmtiMediaVideoTrackInfo -> getTrackName (
				pVideoTrackName) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRACKNAME_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pSDPVideoMedia -> finish () != errNoError)
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

		if ((psaSDPAttribute = new SDPAttribute_t) ==
			(SDPAttribute_p) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPVideoMedia -> finish () != errNoError)
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

			if (pSDPVideoMedia -> finish () != errNoError)
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


	return errNoError;
}


Error SDPMediaFor3GPP:: getSDPMediaAudioFromMediaInfo (
	MP4TrackInfo_p pmtiMediaAudioTrackInfo,
	MP4TrackInfo_p pmtiHintAudioTrackInfo,
	unsigned long ulAudioAvgBitRate,
	unsigned long ulAudioRTPTimeScale,
	const char *pAudioPayloadNumber,
	unsigned char ucChannels,
	MP4Atom:: Standard_t sStandard,
	Tracer_p ptTracer,
	SDPMedia_p pSDPAudioMedia)

{

	Buffer_t				bLocalSDP;
	char					pAudioLocalPayloadNumber [SDP_MAXLONGLENGTH];
	MP4Atom:: MP4Codec_t	cAudioCodecUsed;


	if (pmtiMediaAudioTrackInfo -> getCodec (
		&cAudioCodecUsed) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETCODEC_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (bLocalSDP. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// audio SDP atom
	{
		Error_t					errGetSdpAtom;
		MP4SdpAtom_p				pmaSdpAtom;


		if ((errGetSdpAtom = pmtiMediaAudioTrackInfo ->
			getSdpAtom (&pmaSdpAtom, true)) != errNoError)
		{
			if ((long) errGetSdpAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSDPATOM_FAILED);
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
		else
		{
			if (pmaSdpAtom -> getSdpText (&bLocalSDP) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4SDPATOM_GETSDPTEXT_FAILED);
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

	if (pSDPAudioMedia -> init (
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

	if (strstr ((const char *) bLocalSDP, "m=") == (char *) NULL)
	{
		if (pSDPAudioMedia -> setMediaType (
			SDPMedia:: SDPMEDIA_AUDIO) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_SETMEDIATYPE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPAudioMedia -> finish () != errNoError)
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

			if (pSDPAudioMedia -> finish () != errNoError)
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

			if (pSDPAudioMedia -> finish () != errNoError)
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

			if (pSDPAudioMedia -> finish () != errNoError)
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

		strcpy (pAudioLocalPayloadNumber, pAudioPayloadNumber);
	}
	else
	{
		if (pSDPAudioMedia -> getFmtList (pAudioLocalPayloadNumber) !=
			errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_GETFMTLIST_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPAudioMedia -> finish () != errNoError)
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

			if (pSDPAudioMedia -> finish () != errNoError)
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

			if (pSDPAudioMedia -> finish () != errNoError)
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
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPAudioMedia -> finish () != errNoError)
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
			pAudioLocalPayloadNumber);

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

			if (pSDPAudioMedia -> finish () != errNoError)
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
			ulAudioRTPTimeScale);

		if (sStandard == MP4Atom:: MP4F_ISMA &&
			cAudioCodecUsed == MP4Atom:: MP4F_CODEC_AAC)
		{
			if (ucChannels != 1)
			{
				sprintf (psaSDPRtpMapAttribute -> _pEncodingParameters,
					"%u", ucChannels);
			}
			else
				strcpy (psaSDPRtpMapAttribute -> _pEncodingParameters,
					"");
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

			if (pSDPAudioMedia -> finish () != errNoError)
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

	if (pmtiHintAudioTrackInfo != (MP4TrackInfo_p) NULL &&
		// cAudioCodecUsed == MP4Atom:: MP4F_CODEC_AAC &&
		strstr ((const char *) bLocalSDP, "a=mpeg4-esid:") ==
		(char *) NULL)
	{
		SDPAttribute_p			psaSDPAttribute;
		unsigned long			ulTrackIdentifier;


		if ((psaSDPAttribute = new SDPAttribute_t) ==
			(SDPAttribute_p) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPAudioMedia -> finish () != errNoError)
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

		if (pmtiMediaAudioTrackInfo ->
			getTrackIdentifier (&ulTrackIdentifier) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETTRACKIDENTIFIER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete psaSDPAttribute;

			if (pSDPAudioMedia -> finish () != errNoError)
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

			if (pSDPAudioMedia -> finish () != errNoError)
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
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPAudioMedia -> finish () != errNoError)
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

		strcpy (psaSDPFmtpAttribute -> _pFormat, pAudioLocalPayloadNumber);

		if (cAudioCodecUsed == MP4Atom:: MP4F_CODEC_AAC)
		{
			unsigned char				*pucESConfiguration;
			#ifdef WIN32
				__int64						ullESConfigurationSize;
			#else
				unsigned long long			ullESConfigurationSize;
			#endif
			char						*pESConfigToBase16;


			if (pmtiMediaAudioTrackInfo ->
				getPointerTrackESConfiguration (
				&pucESConfiguration, &ullESConfigurationSize,
				sStandard) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETPOINTERTRACKESCONFIGURATION_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete psaSDPFmtpAttribute;

				if (pSDPAudioMedia -> finish () != errNoError)
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

					if (pSDPAudioMedia -> finish () != errNoError)
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

					if (pSDPAudioMedia -> finish () != errNoError)
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
				if ((pESConfigToBase16 = new char [
					(unsigned int) (ullESConfigurationSize * 2 + 1)]) ==
					(char *) NULL)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_NEW_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete psaSDPFmtpAttribute;

					if (pSDPAudioMedia -> finish () != errNoError)
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

					if (pSDPAudioMedia -> finish () != errNoError)
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

			if (pSDPAudioMedia -> finish () != errNoError)
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

			if (pSDPAudioMedia -> finish () != errNoError)
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
		char					pAudioTrackName [MP4F_MAXTRACKNAMELENGTH];


		if (pmtiHintAudioTrackInfo != (MP4TrackInfo_p) NULL)
		{
			if (pmtiHintAudioTrackInfo -> getTrackName (
				pAudioTrackName) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRACKNAME_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pSDPAudioMedia -> finish () != errNoError)
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
			if (pmtiMediaAudioTrackInfo -> getTrackName (
				pAudioTrackName) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRACKNAME_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pSDPAudioMedia -> finish () != errNoError)
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

		if ((psaSDPAttribute = new SDPAttribute_t) ==
			(SDPAttribute_p) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pSDPAudioMedia -> finish () != errNoError)
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

			if (pSDPAudioMedia -> finish () != errNoError)
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


	return errNoError;
}


