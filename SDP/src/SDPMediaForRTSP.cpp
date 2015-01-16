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

#include "SDPMediaForRTSP.h"
#include <assert.h>



SDPMediaForRTSP:: SDPMediaForRTSP (void): SDPMedia ()

{

}


SDPMediaForRTSP:: ~SDPMediaForRTSP (void)

{

}



SDPMediaForRTSP:: SDPMediaForRTSP (const SDPMediaForRTSP &)

{

	assert (1==0);

	// to do

}


SDPMediaForRTSP &SDPMediaForRTSP:: operator = (const SDPMediaForRTSP &)

{

	assert (1==0);

	// to do

	return *this;

}


Error SDPMediaForRTSP:: init (SDPMediaType_t smtSDPMediaType,
	Tracer_p ptTracer)

{

	_psaControlSDPAttribute			= (SDPAttribute_p) NULL;

	if (SDPMedia:: init (smtSDPMediaType, ptTracer) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPMEDIA_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error SDPMediaForRTSP:: init (Buffer_p pbSDPMedia, Tracer_p ptTracer)

{

	_psaControlSDPAttribute			= (SDPAttribute_p) NULL;

	if (SDPMedia:: init (pbSDPMedia, ptTracer) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPMEDIA_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error SDPMediaForRTSP:: addAttribute (SDPAttribute_p psaSDPAttribute)

{

	if (SDPMedia:: addAttribute (psaSDPAttribute) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPMEDIA_ADDATTRIBUTE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (!strcmp (psaSDPAttribute -> _pName, SDPFORRTSP_CONTROLATTRIBUTENAME))
		_psaControlSDPAttribute			= psaSDPAttribute;


	return errNoError;
}


Error SDPMediaForRTSP:: getMediaInfo (
	SDPMediaType_p pmtMediaType,
	char *pMediaControl,
	unsigned long *pulMediaPayloadType,
	unsigned long *pulMediaPort,
	char *pMediaEncodingName,
	char *pMediaClockRate,
	char *pMediaEncodingParameters,
	unsigned long *pulMediaBandwidthInKbps)

{

	if (pMediaControl != (char *) NULL)
	{
		if (_psaControlSDPAttribute == (SDPAttribute_p) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_ATTRIBUTENOTFOUND,
				1, "control");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		strcpy (pMediaControl, _psaControlSDPAttribute -> _pValue);
	}

	if (SDPMedia:: getMediaInfo (
		pmtMediaType,
		pulMediaPayloadType,
		pulMediaPort,
		pMediaEncodingName,
		pMediaClockRate,
		pMediaEncodingParameters,
		pulMediaBandwidthInKbps) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPMEDIA_GETMEDIAINFO_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}

