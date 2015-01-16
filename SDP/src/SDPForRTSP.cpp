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

#include "SDPForRTSP.h"
#include "SDPMediaForRTSP.h"
#include "SDPTimeNpt.h"
#include "SDPTimeSmpte.h"
#include "SDPTimeUtc.h"
#include <assert.h>



SDPForRTSP:: SDPForRTSP (void): SDP ()

{

}


SDPForRTSP:: ~SDPForRTSP (void)

{

}



SDPForRTSP:: SDPForRTSP (const SDPForRTSP &)

{

	assert (1==0);

	// to do

}


SDPForRTSP &SDPForRTSP:: operator = (const SDPForRTSP &)

{

	assert (1==0);

	// to do

	return *this;

}


Error SDPForRTSP:: init (const char *pSDPPathName, Tracer_p ptTracer)

{

	_psaControlSDPAttribute			= (SDPAttribute_p) NULL;
	_psaRangeSDPAttribute			= (SDPAttribute_p) NULL;

	if (SDP:: init (pSDPPathName, ptTracer) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error SDPForRTSP:: init (Buffer_p pbSDP, Tracer_p ptTracer)

{

	_psaControlSDPAttribute			= (SDPAttribute_p) NULL;
	_psaRangeSDPAttribute			= (SDPAttribute_p) NULL;

	if (SDP:: init (pbSDP, ptTracer) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error SDPForRTSP:: init (
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

	_psaControlSDPAttribute			= (SDPAttribute_p) NULL;
	_psaRangeSDPAttribute			= (SDPAttribute_p) NULL;

	if (SDP:: init (
		ulVersion,

		pOriginUserName,
		pOriginSessionId,
		pOriginVersion,
		pOriginNetworkType,
		pOriginAddressType,
		pOriginAddress,

		pSessionName,
		pInformation,
		pURI,
		pEmailAddress,
		pPhoneNumber,

		pConnectionNetworkType,
		pConnectionAddressType,
		pConnectionAddress,
		pBandwidthModifier,
		ulBandwidthValueInKbps,

		ulTimesStart,
		ulTimesStop,
		ptTracer) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error SDPForRTSP:: addAttribute (SDPAttribute_p psaSDPAttribute)

{

	if (!strcmp (psaSDPAttribute -> _pName, SDPFORRTSP_CONTROLATTRIBUTENAME))
		_psaControlSDPAttribute			= psaSDPAttribute;
	else if (!strcmp (psaSDPAttribute -> _pName, SDPFORRTSP_RANGEATTRIBUTENAME))
	{
		if (SDPForRTSP:: getRangeAttributeInfo (
			psaSDPAttribute -> _pValue,
			&_bIsNow, &_dNptStartTime, &_dNptEndTime,
			_ptTracer) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_ADDATTRIBUTE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		_psaRangeSDPAttribute			= psaSDPAttribute;
	}

	// the next call must be at the end  because the parent calling method
	//	delete the psaSDPAttribute pointer in case it receives an error
	if (SDP:: addAttribute (psaSDPAttribute) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_ADDATTRIBUTE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (!strcmp (psaSDPAttribute -> _pName,
			SDPFORRTSP_CONTROLATTRIBUTENAME))
			_psaControlSDPAttribute			= (SDPAttribute_p) NULL;
		else if (!strcmp (psaSDPAttribute -> _pName,
			SDPFORRTSP_RANGEATTRIBUTENAME))
			_psaRangeSDPAttribute			= (SDPAttribute_p) NULL;

		return err;
	}



	return errNoError;
}


Error SDPForRTSP:: buildSDPMedia (SDPMedia_p *psmCurrentSDPMedia)

{

	if (((*psmCurrentSDPMedia) = new SDPMediaForRTSP_t) ==
		(SDPMediaForRTSP_p) NULL)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error SDPForRTSP:: getRangeAttributeInfo (
	Boolean_p pbIsNow,
	double *pdNptStartTime,
	double *pdNptEndTime)

{

	if (_psaRangeSDPAttribute == (SDPAttribute_p) NULL)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_ATTRIBUTENOTFOUND,
			1, "range");
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pbIsNow				= _bIsNow;
	*pdNptStartTime			= _dNptStartTime;
	*pdNptEndTime			= _dNptEndTime;


	return errNoError;
}


Error SDPForRTSP:: getRangeAttributeInfo (
	const char *pRangeValue,
	Boolean_p pbIsNow,
	double *pdNptStartTime,
	double *pdNptEndTime,
	Tracer_p ptTracer)

{

	const char				*pStartRangeValue;
	const char				*pEndRangeValue;
	char					pRangeBuffer [SDP_ATTRIBUTEVALUELENGTH];


	if (!strncmp (pRangeValue,
		SDP_NPTIDENTIFIER, strlen (SDP_NPTIDENTIFIER)))
	{
		SDPTimeNpt_t			stnNptTime;


		if ((pStartRangeValue = strchr (pRangeValue,
			'=')) == (const char *) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_ATTRIBUTEVALUEWRONG,
				2, "range", pRangeValue);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pStartRangeValue		+= 1;

		if ((pEndRangeValue = strchr (pStartRangeValue,
			'-')) == (const char *) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_ATTRIBUTEVALUEWRONG,
				2, "range", pRangeValue);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pEndRangeValue		+= 1;

		if (*pStartRangeValue == '-')
		{
			// case: a=range:npt=-123
			*pdNptStartTime			= 0.0;
			*pbIsNow				= false;
		}
		else
		{
			// case: a=range:npt=123-[123]
			strncpy (pRangeBuffer, pStartRangeValue,
				pEndRangeValue - pStartRangeValue - 1);
			pRangeBuffer [pEndRangeValue - pStartRangeValue - 1]		= '\0';

			if (stnNptTime. init (pRangeBuffer) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMENPT_INIT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (stnNptTime. getTimeAsNpt (pdNptStartTime,
				pbIsNow) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMEBASE_GETTIMEASNPT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stnNptTime. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPTIMENPT_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stnNptTime. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMENPT_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (*pEndRangeValue == '\0')
		{
			// case: a=range:npt=123-
			*pdNptEndTime			= 0;
		}
		else
		{
			Boolean_t				bIsNow;


			// case: a=range:npt=[123]-123
			if (stnNptTime. init (pEndRangeValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMENPT_INIT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (stnNptTime. getTimeAsNpt (pdNptEndTime,
				&bIsNow) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMEBASE_GETTIMEASNPT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stnNptTime. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPTIMENPT_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stnNptTime. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMENPT_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
	}
	else if (!strncmp (pRangeValue,
		SDP_SMPTEIDENTIFIER, strlen (SDP_SMPTEIDENTIFIER)))
	{
		SDPTimeSmpte_t			stnSmpteTime;


		if ((pStartRangeValue = strchr (pRangeValue,
			'=')) == (char *) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_ATTRIBUTEVALUEWRONG,
				2, "range", pRangeValue);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pStartRangeValue		+= 1;

		if ((pEndRangeValue = strchr (pStartRangeValue,
			'-')) == (char *) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_ATTRIBUTEVALUEWRONG,
				2, "range", pRangeValue);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pEndRangeValue		+= 1;

		{
			// case: a=range:npt=123-[123]
			strncpy (pRangeBuffer, pStartRangeValue,
				pEndRangeValue - pStartRangeValue - 1);
			pRangeBuffer [pEndRangeValue - pStartRangeValue - 1]		= '\0';

			if (stnSmpteTime. init (pRangeBuffer) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMESMPTE_INIT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (stnSmpteTime. getTimeAsNpt (pdNptStartTime,
				pbIsNow) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMEBASE_GETTIMEASNPT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stnSmpteTime. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPTIMESMPTE_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stnSmpteTime. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMESMPTE_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (*pEndRangeValue == '\0')
		{
			// case: a=range:npt=123-
			*pdNptEndTime			= 0;
		}
		else
		{
			Boolean_t				bIsNow;


			// case: a=range:npt=[123]-123
			if (stnSmpteTime. init (pEndRangeValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMESMPTE_INIT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (stnSmpteTime. getTimeAsNpt (pdNptEndTime,
				&bIsNow) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMEBASE_GETTIMEASNPT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stnSmpteTime. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPTIMESMPTE_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stnSmpteTime. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMESMPTE_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
	}
	else if (!strncmp (pRangeValue,
		SDP_UTCIDENTIFIER, strlen (SDP_UTCIDENTIFIER)))
	{
		SDPTimeUtc_t			stnUtcTime;


		if ((pStartRangeValue = strchr (pRangeValue,
			'=')) == (char *) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_ATTRIBUTEVALUEWRONG,
				2, "range", pRangeValue);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pStartRangeValue		+= 1;

		if ((pEndRangeValue = strchr (pStartRangeValue,
			'-')) == (char *) NULL)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_ATTRIBUTEVALUEWRONG,
				2, "range", pRangeValue);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		pEndRangeValue		+= 1;

		{
			// case: a=range:npt=123-[123]
			strncpy (pRangeBuffer, pStartRangeValue,
				pEndRangeValue - pStartRangeValue - 1);
			pRangeBuffer [pEndRangeValue - pStartRangeValue - 1]		= '\0';

			if (stnUtcTime. init (pRangeBuffer) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMEUTC_INIT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (stnUtcTime. getTimeAsNpt (pdNptStartTime,
				pbIsNow) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMEBASE_GETTIMEASNPT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stnUtcTime. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPTIMEUTC_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stnUtcTime. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMEUTC_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (*pEndRangeValue == '\0')
		{
			// case: a=range:npt=123-
			*pdNptEndTime			= 0;
		}
		else
		{
			Boolean_t				bIsNow;


			// case: a=range:npt=[123]-123
			if (stnUtcTime. init (pEndRangeValue) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMEUTC_INIT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (stnUtcTime. getTimeAsNpt (pdNptEndTime,
				&bIsNow) != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMEBASE_GETTIMEASNPT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (stnUtcTime. finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPTIMEUTC_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (stnUtcTime. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPTIMEUTC_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
	}
	else
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_ATTRIBUTEVALUEWRONG,
			2, "range", pRangeValue);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error SDPForRTSP:: getMediaInfoFromSDPFile (
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
	SDPForRTSP_t			sSDPForRTSP;


	if (sSDPForRTSP. init (pSDPPathName, ptTracer) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPFORRTSP_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errGetSDPMedia = sSDPForRTSP. getSDPMedia (
		SDPMedia:: SDPMEDIA_VIDEO, 0, &psmSDPMedia)) != errNoError)
	{
		if ((long) errGetSDPMedia != SDP_SDP_SDPMEDIANOTFOUND)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFORRTSP_GETSDPMEDIA_FAILED);   
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPForRTSP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFORRTSP_FINISH_FAILED);
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

		if (((SDPMediaForRTSP_p) psmSDPMedia) -> getMediaInfo (
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

			if (sSDPForRTSP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFORRTSP_FINISH_FAILED);
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

	if ((errGetSDPMedia = sSDPForRTSP. getSDPMedia (
		SDPMedia:: SDPMEDIA_AUDIO, 0, &psmSDPMedia)) != errNoError)
	{
		if ((long) errGetSDPMedia != SDP_SDP_SDPMEDIANOTFOUND)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFORRTSP_GETSDPMEDIA_FAILED);   
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPForRTSP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFORRTSP_FINISH_FAILED);
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

			if (sSDPForRTSP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPFORRTSP_FINISH_FAILED);
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

	if (sSDPForRTSP. appendToBuffer (pbSDP) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPFORRTSP_APPENDTOBUFFER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (sSDPForRTSP. finish () != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFORRTSP_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (sSDPForRTSP. finish () != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPFORRTSP_FINISH_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


