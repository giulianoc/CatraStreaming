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

#include "rfcisma.h"
#include "MP4DimmAtom.h"
#include "MP4DmedAtom.h"
#include "MP4MdhdAtom.h"
#include "MP4PmaxAtom.h"
#include "MP4SdpAtom.h"
#include "MP4StszAtom.h"
#include "MP4TpylAtom.h"
#include "MP4TrpyAtom.h"
#include "MP4Utility.h"
#include "RTPMessages.h"
#include "RTPHintPacketSampleData.h"
#include "RTPHintSample.h"
#include "RTPUtility.h"
#include "SDPMediaFor3GPP.h"
#include "aac.h"
#include "Convert.h"
#include <stdio.h>


Error rfcIsmaAudioHinter (MP4RootAtom_p pmaRootAtom,
	MP4TrackInfo_p pmtiMediaTrackInfo,
	unsigned long ulMaxPayloadSize, Boolean_t bInterleave,
	MP4TrackInfo_p pmtiHintTrackInfo,
	std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo,
	MP4Atom:: Standard_t sStandard,
	RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
	Tracer_p ptTracer)

{

	MP4MdhdAtom_p					pmaMediaMdhdAtom;
	unsigned long					ulMdhdTimeScale;
	MP4StszAtom_p					pmaMediaStszAtom;
	unsigned long					ulMediaSamplesNumber;
	unsigned long					ulMaxMediaSampleSize;
	MP4TrakAtom_p					pmaMediaTrakAtom;
	MP4Atom::MP4Codec_t				cCodec;
	unsigned long					ulObjectTypeIndication;
	MP4SttsAtom_p					pmaMediaSttsAtom;
	MP4HdlrAtom_p					pmaMediaHdlrAtom;
	unsigned long					ulMediaTrackIdentifier;

	MP4RtpAtom_p					pmaHintRtpAtom;
	MP4SdpAtom_p					pmaHintSdpAtom;
	MP4TrakAtom_p					pmaHintTrakAtom;
	unsigned long					ulHintTrackIdentifier;

	unsigned long					ulRTPTimeScale;
	unsigned long					ulMediaSampleIdentifier;
	unsigned long					ulMediaSampleSize;
	Boolean_t						bIsCommonSampleSize;
	unsigned long					ulPayloadNumber;
	unsigned char					ucMpeg4AudioType;
	#ifdef WIN32
		__int64						ullMaxLatency;
		__int64						ullMediaSampleStartTime;
		__int64						ullSampleDuration;
	#else
		unsigned long long			ullMaxLatency;
		unsigned long long			ullMediaSampleStartTime;
		unsigned long long			ullSampleDuration;
	#endif
	unsigned long					ulSamplesPerPacket;
	unsigned short					usMaxMediaSamplesPerRTPHintPacket;
	// char							pEncodingParameters [
	// 	RTP_MAXENCODINGPARAMETERSLENGTH];
	unsigned long					ulTimestampRandomOffset;
	unsigned long					ulSequenceNumberRandomOffset;


	if (pmtiMediaTrackInfo -> getMdhdAtom (&pmaMediaMdhdAtom) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETMDHDATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getMdhdTimeScale (&ulMdhdTimeScale) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETMDHDTIMESCALE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getStszAtom (&pmaMediaStszAtom, true) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSTSZATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getSamplesNumber (&ulMediaSamplesNumber, true) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSAMPLESNUMBER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getMaxSampleSize (&ulMaxMediaSampleSize, true) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETMAXSAMPLESIZE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getTrakAtom (&pmaMediaTrakAtom) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getCodec (&cCodec) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETCODEC_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getObjectTypeIndication (
		&ulObjectTypeIndication) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETCODEC_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// TrackAudioMpeg4Type (AAC, CELP, HXVC, ...) is the first 5 bits
	// of the ES configuration

	if (ulObjectTypeIndication != MP4_MPEG4_AUDIO_TYPE &&
		!MP4_IS_AAC_AUDIO_TYPE (ulObjectTypeIndication))
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RFC_NONAACAUDIOTYPE);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getSttsAtom (&pmaMediaSttsAtom, true) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSTTSATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getHdlrAtom (&pmaMediaHdlrAtom) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETHDLRATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getTrackIdentifier (
		&ulMediaTrackIdentifier) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETTRACKIDENTIFIER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (ulMediaSamplesNumber == 0)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RFC_NOMEDIASAMPLES);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		// find first non-zero size sample
		// we need to search in case an empty audio sample has been added
		// at the beginning of the track to achieve sync with video

		for (ulMediaSampleIdentifier = 1;
			ulMediaSampleIdentifier <= ulMediaSamplesNumber;
			ulMediaSampleIdentifier++)
		{
			if (MP4Utility:: getSampleSize (
				pmaMediaStszAtom,
				ulMediaSampleIdentifier, &ulMediaSampleSize,
				&bIsCommonSampleSize, ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETSAMPLESIZE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (ulMediaSampleSize > 0)
				break;
		}

		if (ulMediaSampleIdentifier == ulMediaSamplesNumber)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RFC_INVALIDDURATION);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (RTPUtility:: getSampleTimeFromSampleNumber (
			pmaMediaSttsAtom,
			ulMediaSampleIdentifier,
			&ullMediaSampleStartTime, &ullSampleDuration,
			prsrtRTPStreamRealTimeInfo, ptTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_GETSAMPLETIMEFROMSAMPLENUMBER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	{
		unsigned short				usSequenceNumberRandomOffset;


		if (RTPUtility:: getULongRandom (&ulTimestampRandomOffset) !=
			errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_GETULONGRANDOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (RTPUtility:: getUShortRandom (&usSequenceNumberRandomOffset) !=
			errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_GETULONGRANDOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		ulSequenceNumberRandomOffset				= usSequenceNumberRandomOffset;
	}

	if (RTPUtility:: getRTPTimeScale (
		pmaMediaTrakAtom,
		pmaMediaMdhdAtom, cCodec,
		ulObjectTypeIndication,
		sStandard, &ulRTPTimeScale,
		ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPUTILITY_GETRTPTIMESCALE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (MP4TrackInfo:: addHintTrack (pmaRootAtom,
		pmaMediaTrakAtom, ulMediaTrackIdentifier,
		ulMdhdTimeScale ? ulMdhdTimeScale : 1000,
		ulRTPTimeScale,
		ulTimestampRandomOffset, ulSequenceNumberRandomOffset,
		pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_ADDHINTTRACK_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiHintTrackInfo -> getTrakAtom (&pmaHintTrakAtom) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
			pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmtiHintTrackInfo -> getTrackIdentifier (
		&ulHintTrackIdentifier) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETTRACKIDENTIFIER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
			pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmtiHintTrackInfo -> getSdpAtom (&pmaHintSdpAtom, true) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSDPATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
			pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmtiHintTrackInfo -> getRtpAtom (&pmaHintRtpAtom, true) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETRTPATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
			pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (RTPUtility:: allocRtpPayloadNumber (&ulPayloadNumber, pvMP4TracksInfo,
		ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPUTILITY_ALLOCRTPPAYLOADNUMBER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
			pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// SDP initialization
	{
		SDPMedia_p			pSDPAudioMedia;
		Buffer_t			bSDP;
		unsigned long		ulAvgBitRate;
		char				pMediaEncodingName [SDP_ENCODINGNAMELENGTH];
		char				pMediaClockRate [SDP_CLOCKRATELENGTH];
		char				pMediaEncodingParameters [
			SDP_ENCODINGPARAMETERSLENGTH];
		MP4PaytAtom_p		pmaHintPaytAtom;
		Buffer_t			bRtpMap;
		char				pPayloadNumber [RTP_MAXLONGLENGTH];
		unsigned char		ucChannels;
		unsigned char					*pucESConfiguration;
		#ifdef WIN32
			__int64						ullESConfigurationSize;
		#else
			unsigned long long			ullESConfigurationSize;
		#endif



		if (RTPUtility:: getAvgBitRate (
			pmaMediaTrakAtom,
			pmaMediaHdlrAtom,
			pmaMediaMdhdAtom,
			pmaMediaStszAtom,
			cCodec,
			&ulAvgBitRate,
			prsrtRTPStreamRealTimeInfo,
			ptTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_GETAVGBITRATE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmtiMediaTrackInfo -> getPointerTrackESConfiguration (
			&pucESConfiguration, &ullESConfigurationSize,
			MP4Atom:: MP4F_ISMA) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETPOINTERTRACKESCONFIGURATION_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		ucMpeg4AudioType			=
			(unsigned char) (pucESConfiguration [0] >> 3);

		if (ulObjectTypeIndication == MP4_MPEG4_AUDIO_TYPE)
		{
			if (!MP4_IS_MPEG4_AAC_AUDIO_TYPE (ucMpeg4AudioType) &&
				ucMpeg4AudioType != MP4_MPEG4_CELP_AUDIO_TYPE)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RFC_NONAACAUDIOTYPE);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
					pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		ucChannels			= MP4AV_AacConfigGetChannels (pucESConfiguration);

		if (sStandard == MP4Atom:: MP4F_3GPP)
			pSDPAudioMedia = new SDPMediaFor3GPP_t;
		else
			pSDPAudioMedia = new SDPMediaForRTSP_t;

		if (pSDPAudioMedia == (SDPMedia_p) NULL)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		sprintf (pPayloadNumber, "%lu", ulPayloadNumber);

		if (SDPMediaFor3GPP:: getSDPMediaAudioFromMediaInfo (
			pmtiMediaTrackInfo, pmtiHintTrackInfo,
			ulAvgBitRate, ulRTPTimeScale,
			pPayloadNumber,
			ucChannels,
			sStandard, ptTracer,
			pSDPAudioMedia) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIAFOR3GPP_GETSDPMEDIAVIDEOFROMMEDIAINFO_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete pSDPAudioMedia;

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bSDP. init () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
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

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pSDPAudioMedia -> appendToBuffer (&bSDP) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIAFOR3GPP_APPENDTOBUFFER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pSDPAudioMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPAudioMedia;

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaHintSdpAtom -> setSdpText (&bSDP) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pSDPAudioMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPAudioMedia;

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
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

			if (pSDPAudioMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPAudioMedia;

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pSDPAudioMedia -> getMediaInfo (
			(SDPMedia:: SDPMediaType_p) NULL,
			(unsigned long *) NULL,
			(unsigned long *) NULL,
			pMediaEncodingName,
			pMediaClockRate,
			pMediaEncodingParameters,
			(unsigned long *) NULL) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_GETMEDIAINFO_FAILED);
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

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmtiHintTrackInfo -> getPaytAtom (&pmaHintPaytAtom, true) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETPAYTATOM_FAILED);
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

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bRtpMap. init (pMediaEncodingName) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
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

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bRtpMap. append ("/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pSDPAudioMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPAudioMedia;

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bRtpMap. append (pMediaClockRate) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pSDPAudioMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPAudioMedia;

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (strcmp (pMediaEncodingParameters, ""))
		{
			if (bRtpMap. append ("/") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRtpMap. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (pSDPAudioMedia -> finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPMEDIA_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete pSDPAudioMedia;

				if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
					pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRtpMap. append (pMediaEncodingParameters) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRtpMap. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (pSDPAudioMedia -> finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPMEDIA_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete pSDPAudioMedia;

				if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
					pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (pmaHintPaytAtom -> setRtpMap (&bRtpMap) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pSDPAudioMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPAudioMedia;

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bRtpMap. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
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

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pSDPAudioMedia -> finish () != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete pSDPAudioMedia;

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		delete pSDPAudioMedia;

		if (pmaHintPaytAtom -> setPayloadNumber (ulPayloadNumber) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (ulMaxPayloadSize == 0)
		{
			if (pmaHintRtpAtom -> setMaxPacketSize (
				RTP_DEFAULTMAXRTPPAYLOADSIZE) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4RTPATOM_GETMAXPACKETSIZE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
					pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (pmaHintRtpAtom -> setMaxPacketSize (ulMaxPayloadSize) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4RTPATOM_GETMAXPACKETSIZE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
					pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	ulSamplesPerPacket				= 0;

	if (bInterleave)
	{
		// compute how many maximum size samples would fit in a packet
		ulSamplesPerPacket				=
			(ulMaxPayloadSize - 2) / (ulMaxMediaSampleSize + 2);

		// can't interleave if this number is 0 or 1
		if (ulSamplesPerPacket < 2)
			bInterleave				= false;
	}

	if (ucMpeg4AudioType == MP4_MPEG4_CELP_AUDIO_TYPE)
	{
		// 200 ms max latency for ISMA profile 1
		ullMaxLatency					= ulMdhdTimeScale / 5;
	}
	else
	{
		// AAC

		// 500 ms max latency for ISMA profile 1
		ullMaxLatency					= ulMdhdTimeScale / 2;
	}

	usMaxMediaSamplesPerRTPHintPacket		=
		(unsigned short) (ullMaxLatency / ullSampleDuration);

	if (bInterleave)
	{
//		if (audioInterleaveHinter (mp4File,
//			mediaTrackId,
//			hintTrackId,
//			sampleDuration,
//			samplesPerGroup / samplesPerPacket,     // stride
//			samplesPerPacket,                       // bundle
//			RfcIsmaConcatenator,
//			pmaMdatAtom) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RFC_AUDIOINTERLEAVEHINTER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
				pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		/*
		if (audioConsecutiveHinter (pmaRootAtom,
			pmtiMediaTrackInfo -> _pmaTrakAtom,
			*pmaHintTrakAtom,
			(unsigned char) pucESConfiguration [0] >> 3,
			ullSampleDuration, 2, 2,
			(unsigned short) (ullMaxLatency / ullSampleDuration),
			ulPayloadNumber, ulMaxPayloadSize, ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_AUDIOCONSECUTIVEHINTER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return errNoError;
		}
		*/

		{
			unsigned long			ulMediaBytesInThisHint;
			unsigned short			usMediaSamplesNumberInThisHint;
			RTPHintCreator_t		rhcRTPHintCreator;
			unsigned long			ulStartMediaSampleForCurrentRTPPacket;
			unsigned char			ucMediaPacketHeaderSize;
			unsigned char			ucMediaSampleHeaderSize;


			ucMediaPacketHeaderSize		= 2;
			ucMediaSampleHeaderSize		= 2;

			ulMediaBytesInThisHint						= ucMediaPacketHeaderSize;
			usMediaSamplesNumberInThisHint				= 0;

			if (rhcRTPHintCreator. init (pmtiHintTrackInfo, pmaRootAtom,
				ulMaxMediaSampleSize, ptTracer) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTCREATOR_INIT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
					pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			ulStartMediaSampleForCurrentRTPPacket			= 1;

			for (ulMediaSampleIdentifier = 1;
				ulMediaSampleIdentifier <= ulMediaSamplesNumber;
				ulMediaSampleIdentifier++)
			{
				if (MP4Utility:: getSampleSize (pmaMediaStszAtom,
					ulMediaSampleIdentifier, &ulMediaSampleSize,
					&bIsCommonSampleSize,
					ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UTILITY_GETSAMPLESIZE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (rhcRTPHintCreator. finish (false) != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPHINTCREATOR_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
						pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
						errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				// sample won't fit in this packet
				// or we've reached the limit on samples per packet
				if (ulMediaSampleSize + ucMediaSampleHeaderSize >
					ulMaxPayloadSize - ulMediaBytesInThisHint ||
					usMediaSamplesNumberInThisHint == usMaxMediaSamplesPerRTPHintPacket)
				{
					if (usMediaSamplesNumberInThisHint > 0)
					{
						// this method creates one RTPHintSample with one packet
						{

							unsigned char				ucPayloadHeaderSize;
							unsigned char				pucPayloadHeader [2];
							unsigned short				usHeaderBitsNumber;
							unsigned long				ulLocalMediaSampleIdentifier;
							unsigned long				ulSampleSize;
							Boolean_t					bIsCommonSampleSize;
							unsigned long				ulEndMediaSampleForCurrentRTPPacket;
							#ifdef WIN32
								__int64					ullHintDuration;
							#else
								unsigned long long		ullHintDuration;
							#endif


							ulEndMediaSampleForCurrentRTPPacket		=
								ulMediaSampleIdentifier - 1;

							ullHintDuration							=
								usMediaSamplesNumberInThisHint * ullSampleDuration;

							if (ucMpeg4AudioType == MP4_MPEG4_CELP_AUDIO_TYPE)
								ucPayloadHeaderSize				= 1;
							else
								ucPayloadHeaderSize				= 2;

							if (rhcRTPHintCreator. addRTPHintSample () != errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RTPHINTCREATOR_ADDRTPHINTSAMPLE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
									pmtiHintTrackInfo, pvMP4TracksInfo,
									ptTracer) != errNoError)
								{
									Error err = MP4FileErrors (
										__FILE__, __LINE__,
									MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								return err;
							}

							if (rhcRTPHintCreator. addRTPHintPacketToCurrentRTPHintSample (
								false, 0, ulPayloadNumber, true) != errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RTPHINTCREATOR_ADDRTPHINTPACKETTOCURRENTRTPHINTSAMPLE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
									pmtiHintTrackInfo, pvMP4TracksInfo,
									ptTracer) != errNoError)
								{
									Error err = MP4FileErrors (
										__FILE__, __LINE__,
									MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								return err;
							}

							// * 16 means shift
							/*
							usHeaderBitsNumber			= usMediaSamplesNumberInThisHint *
								ucPayloadHeaderSize * 8;
							*/
							usHeaderBitsNumber			=
								(ulEndMediaSampleForCurrentRTPPacket - ulStartMediaSampleForCurrentRTPPacket + 1) *
								ucPayloadHeaderSize * 8;
							pucPayloadHeader [0]		= usHeaderBitsNumber >> 8;
							pucPayloadHeader [1]		= usHeaderBitsNumber & 0xFF;

							if (rhcRTPHintCreator. addRTPImmediateDataToCurrentRTPHintPacket (
								pucPayloadHeader, sizeof (pucPayloadHeader)) != errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RTPHINTCREATOR_ADDRTPIMMEDIATEDATATOCURRENTRTPHINTPACKET_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
									pmtiHintTrackInfo, pvMP4TracksInfo,
									ptTracer) != errNoError)
								{
									Error err = MP4FileErrors (
										__FILE__, __LINE__,
									MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								return err;
							}

							for (ulLocalMediaSampleIdentifier = ulStartMediaSampleForCurrentRTPPacket;
								ulLocalMediaSampleIdentifier <= ulEndMediaSampleForCurrentRTPPacket;
								ulLocalMediaSampleIdentifier++)
							{
								if (MP4Utility:: getSampleSize (pmaMediaStszAtom,
									ulLocalMediaSampleIdentifier, &ulSampleSize, &bIsCommonSampleSize,
									ptTracer) != errNoError)
								{
									Error err = MP4FileErrors (__FILE__, __LINE__,
										MP4F_MP4UTILITY_GETSAMPLESIZE_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);

									if (MP4TrackInfo:: deleteHintTrack (
										pmaRootAtom,
										pmtiHintTrackInfo, pvMP4TracksInfo,
										ptTracer) != errNoError)
									{
										Error err = MP4FileErrors (
											__FILE__, __LINE__,
									MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
										ptTracer -> trace (
											Tracer:: TRACER_LERRR,
											(const char *) err,
											__FILE__, __LINE__);
									}

									return err;
								}

								if (ucPayloadHeaderSize == 1)
								{
									// AU payload header is 6 bits of size
									// follow by 2 bits of the difference between sampleId's - 1
									pucPayloadHeader [0]			= (unsigned char) (ulSampleSize << 2);
								}
								else
								{
									// AU payload header is 13 bits of size
									// follow by 3 bits of the difference between sampleId's - 1
									pucPayloadHeader [0]			= (unsigned char) (ulSampleSize >> 5);
									pucPayloadHeader [1]			= (unsigned char) ((ulSampleSize & 0x1F) << 3);
								}

								if (ulLocalMediaSampleIdentifier != ulStartMediaSampleForCurrentRTPPacket)
								{
									pucPayloadHeader [ucPayloadHeaderSize - 1]		|=
										((ulLocalMediaSampleIdentifier - (ulLocalMediaSampleIdentifier - 1)) - 1); 
								}

								if (rhcRTPHintCreator. addRTPImmediateDataToCurrentRTPHintPacket (
									pucPayloadHeader, ucPayloadHeaderSize) != errNoError)
								{
									Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RTPHINTCREATOR_ADDRTPIMMEDIATEDATATOCURRENTRTPHINTPACKET_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);

									if (MP4TrackInfo:: deleteHintTrack (
										pmaRootAtom,
										pmtiHintTrackInfo, pvMP4TracksInfo,
										ptTracer) != errNoError)
									{
										Error err = MP4FileErrors (
											__FILE__, __LINE__,
									MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
										ptTracer -> trace (
											Tracer:: TRACER_LERRR,
											(const char *) err,
											__FILE__, __LINE__);
									}

									return err;
								}
							}

							for (ulLocalMediaSampleIdentifier = ulStartMediaSampleForCurrentRTPPacket;
								ulLocalMediaSampleIdentifier <= ulEndMediaSampleForCurrentRTPPacket;
								ulLocalMediaSampleIdentifier++)
							{
								if (MP4Utility:: getSampleSize (pmaMediaStszAtom,
									ulLocalMediaSampleIdentifier, &ulSampleSize, &bIsCommonSampleSize,
									ptTracer) != errNoError)
								{
									Error err = MP4FileErrors (__FILE__, __LINE__,
										MP4F_MP4UTILITY_GETSAMPLESIZE_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);

									if (MP4TrackInfo:: deleteHintTrack (
										pmaRootAtom,
										pmtiHintTrackInfo, pvMP4TracksInfo,
										ptTracer) != errNoError)
									{
										Error err = MP4FileErrors (
											__FILE__, __LINE__,
									MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
										ptTracer -> trace (
											Tracer:: TRACER_LERRR,
											(const char *) err,
											__FILE__, __LINE__);
									}

									return err;
								}

								if (rhcRTPHintCreator. addRTPReferenceSampleToCurrentRTPHintPacket (
									ulLocalMediaSampleIdentifier, 0, ulSampleSize) != errNoError)
								{
									Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RTPHINTCREATOR_ADDRTPREFERENCESAMPLETOCURRENTRTPHINTPACKET_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);

									if (MP4TrackInfo:: deleteHintTrack (
										pmaRootAtom,
										pmtiHintTrackInfo, pvMP4TracksInfo,
										ptTracer) != errNoError)
									{
										Error err = MP4FileErrors (
											__FILE__, __LINE__,
									MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
										ptTracer -> trace (
											Tracer:: TRACER_LERRR,
											(const char *) err,
											__FILE__, __LINE__);
									}

									return err;
								}
							}

							if (rhcRTPHintCreator. updateStatisticsForRTPHintPacket () !=
								errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RTPHINTCREATOR_UPDATESTATISTICSFORRTPHINTPACKET_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
									pmtiHintTrackInfo, pvMP4TracksInfo,
									ptTracer) != errNoError)
								{
									Error err = MP4FileErrors (
										__FILE__, __LINE__,
									MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								return err;
							}

							if (rhcRTPHintCreator. updateStatisticsForRTPHintSample () !=
								errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RTPHINTCREATOR_UPDATESTATISTICSFORRTPHINTSAMPLE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
									pmtiHintTrackInfo, pvMP4TracksInfo,
									ptTracer) != errNoError)
								{
									Error err = MP4FileErrors (
										__FILE__, __LINE__,
									MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								return err;
							}

							if (rhcRTPHintCreator. appendRTPHintSampleToMdatAtom (
								ullHintDuration, true,
								prsrtRTPStreamRealTimeInfo) != errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RTPHINTCREATOR_APPENDRTPHINTSAMPLETOMDATATOM_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
									pmtiHintTrackInfo, pvMP4TracksInfo,
									ptTracer) != errNoError)
								{
									Error err = MP4FileErrors (
										__FILE__, __LINE__,
									MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								return err;
							}
						}

						ulStartMediaSampleForCurrentRTPPacket			=
							ulMediaSampleIdentifier;
					}

					// start a new hint 
					usMediaSamplesNumberInThisHint			= 0;
					ulMediaBytesInThisHint					= ucMediaPacketHeaderSize;

					// fall thru
				}

				if (ulMediaSampleSize + ucMediaSampleHeaderSize <=
					ulMaxPayloadSize - ulMediaBytesInThisHint)
				{
					// sample is less than remaining payload size
					// add it to this hint
					ulMediaBytesInThisHint							+=
						(ulMediaSampleSize + ucMediaSampleHeaderSize);
					usMediaSamplesNumberInThisHint++;
				}
				/*
				else
				{
					// jumbo (big) frame, need to fragment it
					if (rfcIsmaFragmenter (pmaRootAtom, pmaMediaTrakAtom,
						pmaHintTrakAtom,
						pmaMdatAtom, usMediaSamplesNumberInThisHint * ullSampleDuration,
						ulMediaSampleIdentifier, ulMediaSampleSize, ullSampleDuration,
						ulPayloadNumber, ulMaxPayloadSize,
						&ulCurrentHintSampleIdentifier, &ulCurrentHintPacketIdentifier,
						&ulAllChunksBufferSize, &ulCurrentChunkBufferSize,
						&ulCurrentChunkSamplesNumber, &ullCurrentChunkDuration,
						&ullThisSec, &ulBytesThisSec,
						&ullCurrentChunkOffset, ptTracer) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILE_RFCISMAFRAGMENTER_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (rhcRTPHintCreator. finish (false) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_RTPHINTCREATOR_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					// start a new hint 
					usMediaSamplesNumberInThisHint			= 0;
					ulMediaBytesInThisHint					= ucMediaPacketHeaderSize;
				}
				*/
			}

			if (rhcRTPHintCreator. finish (true) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTCREATOR_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (MP4TrackInfo:: deleteHintTrack (pmaRootAtom,
					pmtiHintTrackInfo, pvMP4TracksInfo, ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_DELETEHINTTRACK_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}


	return errNoError;
}


Error rfcIsmaAudioPrefetcher (
	unsigned long ulMediaSamplesNumber,
	unsigned long ulPayloadNumber,
	unsigned long ulRTPTimeScale,
	unsigned long ulMdhdTimeScale,
	// unsigned long ulFirstEditRTPTime,
	unsigned long ulServerSSRC,
	MP4RootAtom_p pmaRootAtom,
	MP4TrackInfo_p pmtiMediaTrackInfo,
	unsigned long ulMaxPayloadSize,
	unsigned long ulRTPPacketsNumberToPrefetch,
	const char *pTrackURI,
	std:: vector<RTPPacket_p> *pvFreeRTPPackets,
	std:: vector<RTPPacket_p> *pvBuiltRTPPackets,
	std:: vector<double> *pvTransmissionTimeBuiltRTPPackets,
	RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
	Tracer_p ptTracer,
	unsigned long *pulCurrentMediaSampleIndex,
	unsigned long *pulCurrentSequenceNumber)

{

	MP4StszAtom_p					pmaMediaStszAtom;
	MP4SttsAtom_p					pmaMediaSttsAtom;
	MP4TrakAtom_p					pmaMediaTrakAtom;

	#ifdef WIN32
		__int64						ullMediaSampleStartTime;
		__int64						ullRelativeRTPPacketTransmissionTimeWithTimeScale;
	#else
		unsigned long long			ullMediaSampleStartTime;
		unsigned long long			ullRelativeRTPPacketTransmissionTimeWithTimeScale;
	#endif
	double						dRelativeRTPPacketTransmissionTimeInSecs;
	unsigned long				ulTrackReferenceIndex;
	long						lTLVTimestampOffset;
	unsigned long				ulBytesPerCompressionBlock;
	unsigned long				ulSamplePerCompressionBlock;
	RTPPacket_p					prpRTPPacket;
	unsigned long				ulCurrentPrefetchedRTPPacketsNumber;
	long						lRelativePacketTransmissionTime;
	unsigned long				ulStartMediaSampleForCurrentRTPPacket;
	unsigned long				ulMediaSampleSize;
	Boolean_t					bIsCommonSampleSize;
	unsigned char				ucMediaSampleHeaderSize;
	unsigned char				ucMediaPacketHeaderSize;
	unsigned long				ulMediaBytesInThisHint;
	unsigned short				usMediaSamplesNumberInThisHint;
	unsigned short				usMaxMediaSamplesPerRTPHintPacket;
	#ifdef WIN32
		__int64					ullSampleDuration;
	#else
		unsigned long long		ullSampleDuration;
	#endif
	unsigned char				ucMpeg4AudioType;
//	double						dOffsetFromStartTime;


	if (pmtiMediaTrackInfo -> getStszAtom (&pmaMediaStszAtom, true) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSTSZATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getSttsAtom (&pmaMediaSttsAtom, true) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSTTSATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getTrakAtom (&pmaMediaTrakAtom) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	ulTrackReferenceIndex					= 0;
	ulBytesPerCompressionBlock				= 1;
	ulSamplePerCompressionBlock				= 1;

	ulCurrentPrefetchedRTPPacketsNumber		= 0;

	{
		#ifdef WIN32
			__int64					ullESConfigurationSize;
			__int64					ullMaxLatency;
		#else
			unsigned long long		ullESConfigurationSize;
			unsigned long long		ullMaxLatency;
		#endif
		unsigned long				ulLocalMediaSampleIdentifier;
		unsigned char				*pucESConfiguration;


		for (ulLocalMediaSampleIdentifier = 1;
			ulLocalMediaSampleIdentifier <= ulMediaSamplesNumber;
			ulLocalMediaSampleIdentifier++)
		{
			if (MP4Utility:: getSampleSize (
				pmaMediaStszAtom,
				ulLocalMediaSampleIdentifier, &ulMediaSampleSize,
				&bIsCommonSampleSize, ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETSAMPLESIZE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (ulMediaSampleSize > 0)
				break;
		}

		if (ulLocalMediaSampleIdentifier == ulMediaSamplesNumber)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RFC_INVALIDDURATION);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (RTPUtility:: getSampleTimeFromSampleNumber (
			pmaMediaSttsAtom,
			ulLocalMediaSampleIdentifier,
			&ullMediaSampleStartTime, &ullSampleDuration,
			prsrtRTPStreamRealTimeInfo, ptTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_GETSAMPLETIMEFROMSAMPLENUMBER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pmtiMediaTrackInfo -> getPointerTrackESConfiguration (
			&pucESConfiguration, &ullESConfigurationSize,
			MP4Atom:: MP4F_ISMA) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETPOINTERTRACKESCONFIGURATION_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		ucMpeg4AudioType			=
			(unsigned char) (pucESConfiguration [0] >> 3);

		if (ucMpeg4AudioType == MP4_MPEG4_CELP_AUDIO_TYPE)
		{
			// 200 ms max latency for ISMA profile 1
			ullMaxLatency					= ulMdhdTimeScale / 5;
		}
		else
		{
			// AAC

			// 500 ms max latency for ISMA profile 1
			ullMaxLatency					= ulMdhdTimeScale / 2;
		}

		usMaxMediaSamplesPerRTPHintPacket		=
			(unsigned short) (ullMaxLatency / ullSampleDuration);
	}

	ulStartMediaSampleForCurrentRTPPacket	=
		*pulCurrentMediaSampleIndex;

	ucMediaPacketHeaderSize					= 2;
	ucMediaSampleHeaderSize					= 2;

	ulMediaBytesInThisHint						= ucMediaPacketHeaderSize;
	usMediaSamplesNumberInThisHint				= 0;

	for ( ;
		*pulCurrentMediaSampleIndex <= ulMediaSamplesNumber;
		(*pulCurrentMediaSampleIndex)++)
	{
		if (MP4Utility:: getSampleSize (pmaMediaStszAtom,
			*pulCurrentMediaSampleIndex, &ulMediaSampleSize, &bIsCommonSampleSize,
			ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_GETSAMPLESIZE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		// sample won't fit in this packet
		// or we've reached the limit on samples per packet
		if (ulMediaSampleSize + ucMediaSampleHeaderSize >
			ulMaxPayloadSize - ulMediaBytesInThisHint ||
			usMediaSamplesNumberInThisHint == usMaxMediaSamplesPerRTPHintPacket)
		{
			if (usMediaSamplesNumberInThisHint > 0)
			{
				// this method creates one RTPHintSample with one packet
				{
					unsigned char				ucPayloadHeaderSize;
					unsigned char				pucPayloadHeader [2];
					unsigned short				usHeaderBitsNumber;
					unsigned long				ulLocalMediaSampleIdentifier;
					unsigned long				ulSampleSize;
					Boolean_t					bIsCommonSampleSize;
					unsigned long				ulEndMediaSampleForCurrentRTPPacket;
					#ifdef WIN32
						__int64					ullHintDuration;
					#else
						unsigned long long		ullHintDuration;
					#endif


					ulEndMediaSampleForCurrentRTPPacket		=
						(*pulCurrentMediaSampleIndex) - 1;

					ullHintDuration							=
						usMediaSamplesNumberInThisHint * ullSampleDuration;

					if (ucMpeg4AudioType == MP4_MPEG4_CELP_AUDIO_TYPE)
						ucPayloadHeaderSize				= 1;
					else
						ucPayloadHeaderSize				= 2;

					// if (rhcRTPHintCreator. addRTPHintSample () != errNoError)
					// if (rhcRTPHintCreator. addRTPHintPacketToCurrentRTPHintSample (
					//	false, 0, ulPayloadNumber, true) != errNoError)

					// * 16 means shift
					// usHeaderBitsNumber			= usMediaSamplesNumberInThisHint *
					//	ucPayloadHeaderSize * 8;
					usHeaderBitsNumber			=
						(ulEndMediaSampleForCurrentRTPPacket - ulStartMediaSampleForCurrentRTPPacket + 1) *
						ucPayloadHeaderSize * 8;
					pucPayloadHeader [0]		= usHeaderBitsNumber >> 8;
					pucPayloadHeader [1]		= usHeaderBitsNumber & 0xFF;

					if (RTPUtility:: getSampleTimeFromSampleNumber (
						pmaMediaSttsAtom,
						ulStartMediaSampleForCurrentRTPPacket,
						&ullMediaSampleStartTime, &ullSampleDuration,
						prsrtRTPStreamRealTimeInfo, ptTracer) != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPUTILITY_GETSAMPLETIMEFROMSAMPLENUMBER_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return err;
					}

					// if (rhcRTPHintCreator. addRTPImmediateDataToCurrentRTPHintPacket (
					//	pucPayloadHeader, sizeof (pucPayloadHeader)) != errNoError)
					{
						if (pvFreeRTPPackets -> begin () == pvFreeRTPPackets -> end ())
						{
							Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RFC_RTPPACKETNOTAVAILABLE);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						prpRTPPacket			= *(pvFreeRTPPackets -> begin ());

						if (prpRTPPacket -> setVersion (2) != errNoError)
						{
							Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RTPPACKET_SETVERSION_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						if (prpRTPPacket -> setMarkerBit (true) != errNoError)
						{
							Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RTPPACKET_SETMARKERBIT_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (prpRTPPacket -> reset () != errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RTPPACKET_RESET_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (prpRTPPacket -> setPayloadType (
							(unsigned char) ulPayloadNumber) != errNoError)
						{
							Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RTPPACKET_SETPAYLOADTYPE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (prpRTPPacket -> reset () != errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RTPPACKET_RESET_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (prpRTPPacket -> setSequenceNumber (
							(unsigned short) (*pulCurrentSequenceNumber)) !=
							errNoError)
						{
							Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RTPPACKET_SETSEQUENCENUMBER_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (prpRTPPacket -> reset () != errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RTPPACKET_RESET_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (ulRTPTimeScale == ulMdhdTimeScale)
							ullRelativeRTPPacketTransmissionTimeWithTimeScale	= ullMediaSampleStartTime;
						else
							ullRelativeRTPPacketTransmissionTimeWithTimeScale	= ullMediaSampleStartTime *
								(((double) ulRTPTimeScale) / ((double) ulMdhdTimeScale));

						// ullRelativeRTPPacketTransmissionTimeWithTimeScale		+= ulFirstEditRTPTime;

						lTLVTimestampOffset					= 0;

						ullRelativeRTPPacketTransmissionTimeWithTimeScale		+= lTLVTimestampOffset;

						if (prpRTPPacket -> setTimeStamp ((unsigned long)
							ullRelativeRTPPacketTransmissionTimeWithTimeScale) !=
							errNoError)
						{
							Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RTPPACKET_SETTIMESTAMP_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (prpRTPPacket -> reset () != errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RTPPACKET_RESET_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (prpRTPPacket -> setServerSSRC (ulServerSSRC) !=
							errNoError)
						{
							Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RTPPACKET_SETSERVERSSRC_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (prpRTPPacket -> reset () != errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RTPPACKET_RESET_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (prpRTPPacket -> appendData (pucPayloadHeader, sizeof (pucPayloadHeader)) !=
							errNoError)
						{
							Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RTPPACKET_APPENDDATA_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (prpRTPPacket -> reset () != errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RTPPACKET_RESET_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
					}

					for (ulLocalMediaSampleIdentifier = ulStartMediaSampleForCurrentRTPPacket;
						ulLocalMediaSampleIdentifier <= ulEndMediaSampleForCurrentRTPPacket;
						ulLocalMediaSampleIdentifier++)
					{
						if (MP4Utility:: getSampleSize (pmaMediaStszAtom,
							ulLocalMediaSampleIdentifier, &ulSampleSize, &bIsCommonSampleSize,
							ptTracer) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4UTILITY_GETSAMPLESIZE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						if (ucPayloadHeaderSize == 1)
						{
							// AU payload header is 6 bits of size
							// follow by 2 bits of the difference between sampleId's - 1
							pucPayloadHeader [0]			= (unsigned char) (ulSampleSize << 2);
						}
						else
						{
							// AU payload header is 13 bits of size
							// follow by 3 bits of the difference between sampleId's - 1
							pucPayloadHeader [0]			= (unsigned char) (ulSampleSize >> 5);
							pucPayloadHeader [1]			= (unsigned char) ((ulSampleSize & 0x1F) << 3);
						}

						if (ulLocalMediaSampleIdentifier != ulStartMediaSampleForCurrentRTPPacket)
						{
							pucPayloadHeader [ucPayloadHeaderSize - 1]		|=
								((ulLocalMediaSampleIdentifier - (ulLocalMediaSampleIdentifier - 1)) - 1); 
						}

						// if (rhcRTPHintCreator. addRTPImmediateDataToCurrentRTPHintPacket (
						//	pucPayloadHeader, ucPayloadHeaderSize) != errNoError)
						{
							if (prpRTPPacket -> appendData (
								pucPayloadHeader, ucPayloadHeaderSize) !=
								errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RTPPACKET_APPENDDATA_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (prpRTPPacket -> reset () != errNoError)
								{
									Error err = RTPErrors (__FILE__, __LINE__,
										RTP_RTPPACKET_RESET_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								return err;
							}
						}
					}

					for (ulLocalMediaSampleIdentifier = ulStartMediaSampleForCurrentRTPPacket;
						ulLocalMediaSampleIdentifier <= ulEndMediaSampleForCurrentRTPPacket;
						ulLocalMediaSampleIdentifier++)
					{
						if (MP4Utility:: getSampleSize (pmaMediaStszAtom,
							ulLocalMediaSampleIdentifier, &ulSampleSize, &bIsCommonSampleSize,
							ptTracer) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4UTILITY_GETSAMPLESIZE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						// if (rhcRTPHintCreator. addRTPReferenceSampleToCurrentRTPHintPacket (
						//	ulLocalMediaSampleIdentifier, 0, ulSampleSize) != errNoError)
						{
							if (RTPHintPacketSampleData:: appendPayloadDataToRTPPacket (
								prpRTPPacket, pmaRootAtom,
								pmtiMediaTrackInfo,
								ulTrackReferenceIndex, ulSampleSize,
								ulLocalMediaSampleIdentifier, 0,
								ulBytesPerCompressionBlock,
								ulSamplePerCompressionBlock,
								prsrtRTPStreamRealTimeInfo,
								ptTracer) != errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RTPHINTPACKETSAMPLEDATA_APPENDPAYLOADDATATORTPPACKET_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (prpRTPPacket -> reset () != errNoError)
								{
									Error err = RTPErrors (__FILE__, __LINE__,
										RTP_RTPPACKET_RESET_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								return err;
							}
						}
					}

					{
						lRelativePacketTransmissionTime			= 0;

						dRelativeRTPPacketTransmissionTimeInSecs			=
							((double) ullMediaSampleStartTime /
							((double) ulMdhdTimeScale)) +
							((double) lRelativePacketTransmissionTime /
							((double) ulMdhdTimeScale));

						/*
						dOffsetFromStartTime									=
							(*pdRelativeRTPPacketTransmissionTimeInSecs) -
							dRelativeRequestedStartTimeInSecs;

						dRelativeRTPPacketTransmissionTimeInMilliSecs	=
							dRelativeRequestedStartTimeInSecs +
							(dOffsetFromStartTime / dRequestedSpeed);

						if (dRelativeRTPPacketTransmissionTimeInMilliSecs < 0.0)
							dRelativeRTPPacketTransmissionTimeInMilliSecs		= 0.0;

						dRelativeRTPPacketTransmissionTimeInMilliSecs		=
							(dRelativeRTPPacketTransmissionTimeInMilliSecs * 1000);
						*/

						pvBuiltRTPPackets -> insert (
							pvBuiltRTPPackets -> end (),
							prpRTPPacket);

						pvTransmissionTimeBuiltRTPPackets -> insert (
							pvTransmissionTimeBuiltRTPPackets -> end (),
							dRelativeRTPPacketTransmissionTimeInSecs * 1000);

						pvFreeRTPPackets -> erase (pvFreeRTPPackets -> begin ());

						(*pulCurrentSequenceNumber)++;
						ulCurrentPrefetchedRTPPacketsNumber++;

						/*
						{
							#ifdef WIN32
								Message msg = MP4FileMessages (__FILE__, __LINE__,
									MP4F_MP4FILE_BUILDANDPREFETCHEDRTPPACKET,
									6,
									pTrackURI,
									*pdRelativeRTPPacketTransmissionTimeInSecs,
									(*pulCurrentSequenceNumber) - 1,
									"true",
									(unsigned long) ullRelativeRTPPacketTransmissionTimeWithTimeScale,
									(__int64) (*prpRTPPacket));
							#else
								Message msg = MP4FileMessages (__FILE__, __LINE__,
									MP4F_MP4FILE_BUILDANDPREFETCHEDRTPPACKET,
									6,
									pTrackURI,
									*pdRelativeRTPPacketTransmissionTimeInSecs,
									(*pulCurrentSequenceNumber) - 1,
									"true",
									(unsigned long) ullRelativeRTPPacketTransmissionTimeWithTimeScale,
									(unsigned long long) (*prpRTPPacket));
							#endif
							ptTracer -> trace (Tracer:: TRACER_LDBG6,
								(const char *) msg, __FILE__, __LINE__);
						}
						*/
					}

					if (ulCurrentPrefetchedRTPPacketsNumber >=
						ulRTPPacketsNumberToPrefetch)
					{

						break;
					}
				}

				ulStartMediaSampleForCurrentRTPPacket			=
					*pulCurrentMediaSampleIndex;
			}

			// start a new hint 
			usMediaSamplesNumberInThisHint			= 0;
			ulMediaBytesInThisHint					= ucMediaPacketHeaderSize;

			// fall thru
		}

		if (ulMediaSampleSize + ucMediaSampleHeaderSize <=
			ulMaxPayloadSize - ulMediaBytesInThisHint)
		{
			// sample is less than remaining payload size
			// add it to this hint
			ulMediaBytesInThisHint							+=
				(ulMediaSampleSize + ucMediaSampleHeaderSize);
			usMediaSamplesNumberInThisHint++;
		}
		/*
		else
		{
			// jumbo (big) frame, need to fragment it
			if (rfcIsmaFragmenter (pmaRootAtom, pmaMediaTrakAtom,
				pmaHintTrakAtom,
				pmaMdatAtom, usMediaSamplesNumberInThisHint * ullSampleDuration,
				ulMediaSampleIdentifier, ulMediaSampleSize, ullSampleDuration,
				ulPayloadNumber, ulMaxPayloadSize,
				&ulCurrentHintSampleIdentifier, &ulCurrentHintPacketIdentifier,
				&ulAllChunksBufferSize, &ulCurrentChunkBufferSize,
				&ulCurrentChunkSamplesNumber, &ullCurrentChunkDuration,
				&ullThisSec, &ulBytesThisSec,
				&ullCurrentChunkOffset, ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILE_RFCISMAFRAGMENTER_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (rhcRTPHintCreator. finish (false) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTCREATOR_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// start a new hint 
			usMediaSamplesNumberInThisHint			= 0;
			ulMediaBytesInThisHint					= ucMediaPacketHeaderSize;
		}
		*/
	}


	return errNoError;
}


/*
#ifdef WIN32
	Error rfcIsmaFragmenter (
		MP4RootAtom_p pmaRootAtom,
		MP4TrakAtom_p pmaMediaTrakAtom,
		MP4TrakAtom_p pmaHintTrakAtom,
		MP4MdatAtom_p pmaMdatAtom,
		__int64 ullHintDuration,
		unsigned long ulMediaSampleIdentifier,
		unsigned long ulMediaSampleSize,
		__int64 ullSampleDuration,
		unsigned long ulPayloadNumber,
		unsigned long ulMaxPayloadSize,
		unsigned long *pulCurrentHintSampleIdentifier,
		unsigned long *pulCurrentHintPacketIdentifier,
		unsigned long *pulAllChunksBufferSize,
		unsigned long *pulCurrentChunkBufferSize,
		unsigned long *pulCurrentChunkSamplesNumber,
		__int64 *pullCurrentChunkDuration,
		__int64 *pullThisSec,
		unsigned long *pulBytesThisSec,
		__int64 *pullCurrentChunkOffset,
		Tracer_p ptTracer)
#else
	Error rfcIsmaFragmenter (
		MP4RootAtom_p pmaRootAtom,
		MP4TrakAtom_p pmaMediaTrakAtom,
		MP4TrakAtom_p pmaHintTrakAtom,
		MP4MdatAtom_p pmaMdatAtom,
		unsigned long long ullHintDuration,
		unsigned long ulMediaSampleIdentifier,
		unsigned long ulMediaSampleSize,
		unsigned long long ullSampleDuration,
		unsigned long ulPayloadNumber,
		unsigned long ulMaxPayloadSize,
		unsigned long *pulCurrentHintSampleIdentifier,
		unsigned long *pulCurrentHintPacketIdentifier,
		unsigned long *pulAllChunksBufferSize,
		unsigned long *pulCurrentChunkBufferSize,
		unsigned long *pulCurrentChunkSamplesNumber,
		unsigned long long *pullCurrentChunkDuration,
		unsigned long long *pullThisSec,
		unsigned long *pulBytesThisSec,
		unsigned long long *pullCurrentChunkOffset,
		Tracer_p ptTracer)
#endif

{

	RTPHintSample_p				prhsRTPHintSample;
	RTPHintPacket_p				prpRTPHintPacket;
	unsigned char				pucPayloadHeader [4];
	unsigned long				ulCurrentHintBytesNumber;
	MP4Atom_p					pmaAtom;


	// c'e' una gestione dei pacchetti molto strana
	// DA RIVEDERE
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NOTTESTED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

// 	MP4AddRtpHint(mp4File, hintTrackId);
// 	SRC:TRACK
// 	    m_pWriteHint = new MP4RtpHint(this);
// 
// 		// related to the packet
// 		m_pWriteHint->SetBFrame(isBFrame);		// false
// 		m_pWriteHint->SetTimestampOffset(timestampOffset);	// 0
// 		m_bytesThisHint = 0;
// 		m_writeHintId++;

	ulCurrentHintBytesNumber				=	0;

	if ((prhsRTPHintSample = new RTPHintSample_t) == (RTPHintSample_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	(*pulCurrentHintSampleIdentifier)				+= 1;

	if (prhsRTPHintSample -> init (pmaHintTrakAtom,
		*pulCurrentHintSampleIdentifier,
		ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_RTPHINTSAMPLE_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete prhsRTPHintSample;
		prhsRTPHintSample			= (RTPHintSample_p) NULL;

		return err;
	}

// 	MP4AddRtpPacket(mp4File, hintTrackId, false);
// 	    MP4RtpPacket* pPacket = m_pWriteHint->AddPacket();
// 			MP4RtpPacket* pPacket = new MP4RtpPacket(this);
// 			m_rtpPackets.Add(pPacket);
// 			((MP4Integer16Property*)m_pProperties[0])->IncrementValue();
// 			pPacket->SetBFrame(m_isBFrame);
// 			pPacket->SetTimestampOffset(m_timestampOffset);
// 		pPacket->Set( m_pPayloadNumberProperty->GetValue(), m_writePacketId++,
// 			setMbit);		false
// 		pPacket->SetTransmitOffset(transmitOffset);		0
// 		m_bytesThisHint += 12;
// 		if (m_bytesThisPacket > m_pPmax->GetValue()) {
// 			m_pPmax->SetValue(m_bytesThisPacket);
// 		}
// 		m_bytesThisPacket = 12;
// 		m_pNump->IncrementValue();
// 		m_pTrpy->IncrementValue(12); // RTP packet header size
// non so perche' ma BFrame e Timestamp la lib le setta
// a livello di hint (vedi MP4AddRtpHint)
// m_bytesThisHint += 12;

	if (prhsRTPHintSample -> createRTPHintPacket (&prpRTPHintPacket) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_RTPHINTSAMPLE_CREATERTPHINTPACKET_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (prhsRTPHintSample -> finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_RTPHINTSAMPLE_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		delete prhsRTPHintSample;
		prhsRTPHintSample			= (RTPHintSample_p) NULL;

		return err;
	}

	if (prpRTPHintPacket -> setBFrame (false) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_RTPHINTPACKET_SETBFRAME_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (prhsRTPHintSample -> finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_RTPHINTSAMPLE_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		delete prhsRTPHintSample;
		prhsRTPHintSample			= (RTPHintSample_p) NULL;

		return err;
	}

	// means no TVL information
	if (prpRTPHintPacket -> setTLVTimestampOffset (0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_RTPHINTPACKET_SETTLVTIMESTAMPOFFSET_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (prhsRTPHintSample -> finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_RTPHINTSAMPLE_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		delete prhsRTPHintSample;
		prhsRTPHintSample			= (RTPHintSample_p) NULL;

		return err;
	}

	if (prpRTPHintPacket -> setPayloadType (ulPayloadNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_RTPHINTPACKET_SETPAYLOADTYPE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (prhsRTPHintSample -> finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_RTPHINTSAMPLE_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		delete prhsRTPHintSample;
		prhsRTPHintSample			= (RTPHintSample_p) NULL;

		return err;
	}

	if (prpRTPHintPacket -> setSequenceNumber (
		(*pulCurrentHintPacketIdentifier)++) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_RTPHINTPACKET_SETSEQUENCENUMBER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (prhsRTPHintSample -> finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_RTPHINTSAMPLE_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		delete prhsRTPHintSample;
		prhsRTPHintSample			= (RTPHintSample_p) NULL;

		return err;
	}

	if (prpRTPHintPacket -> setMarkerBit (false) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_RTPHINTPACKET_SETMARKERBIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (prhsRTPHintSample -> finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_RTPHINTSAMPLE_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		delete prhsRTPHintSample;
		prhsRTPHintSample			= (RTPHintSample_p) NULL;

		return err;
	}

	// Note: CELP is never fragmented
	// so we assume the two byte AAC-hbr payload header
// 	u_int8_t payloadHeader[4];
// 	payloadHeader[0] = 0;
// 	payloadHeader[1] = 16;
// 	payloadHeader[2] = sampleSize >> 5;
// 	payloadHeader[3] = (sampleSize & 0x1F) << 3;

// 	MP4AddRtpImmediateData(mp4File, hintTrackId,
// 		(u_int8_t*)&payloadHeader, sizeof(payloadHeader));
// 		MP4RtpPacket* pPacket = m_pWriteHint->GetCurrentPacket();
//     	if (pBytes == NULL || numBytes == 0) {
// 			throw new MP4Error("no data", "AddImmediateData");
// 		}
// 	    if (numBytes > 14) {
// 			throw new MP4Error("data size is larger than 14 bytes",
// 				"AddImmediateData");
// 		}
// 		MP4RtpImmediateData* pData = new MP4RtpImmediateData(pPacket);
// 		pData->Set(pBytes, numBytes);
// 		pPacket->AddData(pData);
// 			m_rtpData.Add(pData);
// 			((MP4Integer16Property*)m_pProperties[12])->IncrementValue();
// 		m_bytesThisHint += numBytes;
// 		m_bytesThisPacket += numBytes;
// 		m_pDimm->IncrementValue(numBytes);
// 		m_pTpyl->IncrementValue(numBytes);
// 		m_pTrpy->IncrementValue(numBytes);
// m_bytesThisHint += numBytes;

	pucPayloadHeader [0]		= 0;
	pucPayloadHeader [1]		= 16;
	pucPayloadHeader [2]		= (unsigned char) (ulMediaSampleSize >> 5);
	pucPayloadHeader [3]		= (unsigned char) ((ulMediaSampleSize & 0x1F) << 3);

	if (prpRTPHintPacket -> createRTPImmediateData (pucPayloadHeader,
		sizeof (pucPayloadHeader)) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_RTPHINTPACKET_CREATERTPIMMEDIATEDATA_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (prhsRTPHintSample -> finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_RTPHINTSAMPLE_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		delete prhsRTPHintSample;
		prhsRTPHintSample			= (RTPHintSample_p) NULL;

		return err;
	}

	// u_int16_t sampleOffset = 0;
	// u_int16_t fragLength = maxPayloadSize - 4;

	// do {
	// 	MP4AddRtpSampleData(mp4File, hintTrackId,
	// 		sampleId, sampleOffset, fragLength);
	// 	    MP4RtpPacket* pPacket = m_pWriteHint->GetCurrentPacket();
	// 		MP4RtpSampleData* pData = new MP4RtpSampleData(pPacket);
	// 		pData->SetReferenceSample(sampleId, dataOffset, dataLength);
	// 		pPacket->AddData(pData);
	// 		m_bytesThisHint += dataLength;
	// 		m_bytesThisPacket += dataLength;
	// 		m_pDmed->IncrementValue(dataLength);
	// 		m_pTpyl->IncrementValue(dataLength);
	// 		m_pTrpy->IncrementValue(dataLength);

	// 	sampleOffset += fragLength;

	// 	if (sampleSize - sampleOffset > maxPayloadSize) {
	// 		fragLength = maxPayloadSize; 
	// 		MP4AddRtpPacket(mp4File, hintTrackId, false);
	//     		MP4RtpPacket* pPacket = m_pWriteHint->AddPacket();
	// 				MP4RtpPacket* pPacket = new MP4RtpPacket(this);
	// 				m_rtpPackets.Add(pPacket);
	// 				((MP4Integer16Property*)m_pProperties[0])->IncrementValue();
	// 				pPacket->SetBFrame(m_isBFrame);
	// 				pPacket->SetTimestampOffset(m_timestampOffset);
	// 			pPacket->Set( m_pPayloadNumberProperty->GetValue(), m_writePacketId++,
	// 				setMbit);	false
	// 			pPacket->SetTransmitOffset(transmitOffset);		0
	// 			m_bytesThisHint += 12;
	// 			if (m_bytesThisPacket > m_pPmax->GetValue()) {
	// 				m_pPmax->SetValue(m_bytesThisPacket);
	// 			}
	// 			m_bytesThisPacket = 12;
	// 			m_pNump->IncrementValue();
	// 			m_pTrpy->IncrementValue(12); // RTP packet header size
	// 	} else {
	// 		fragLength = sampleSize - sampleOffset; 
	// 		if (fragLength) {
	// 			MP4AddRtpPacket(mp4File, hintTrackId, true);
	//     			MP4RtpPacket* pPacket = m_pWriteHint->AddPacket();
	// 					MP4RtpPacket* pPacket = new MP4RtpPacket(this);
	// 					m_rtpPackets.Add(pPacket);
	// 					((MP4Integer16Property*)m_pProperties[0])->IncrementValue();
	// 					pPacket->SetBFrame(m_isBFrame);
	// 					pPacket->SetTimestampOffset(m_timestampOffset);
	// 				pPacket->Set( m_pPayloadNumberProperty->GetValue(), m_writePacketId++,
	// 					setMbit);		true
	// 				pPacket->SetTransmitOffset(transmitOffset);		0
	// 				m_bytesThisHint += 12;
	// 				if (m_bytesThisPacket > m_pPmax->GetValue()) {
	// 					m_pPmax->SetValue(m_bytesThisPacket);
	// 				}
	// 				m_bytesThisPacket = 12;
	// 				m_pNump->IncrementValue();
	// 				m_pTrpy->IncrementValue(12); // RTP packet header size
	// 		}
	// 	}
	// } while (sampleOffset < sampleSize);

	{
		unsigned short					usSampleOffset;
		unsigned short					usFragLength;


		usSampleOffset				= 0;
		usFragLength				= (unsigned short) (ulMaxPayloadSize - 4);

		do 
		{
			if (prpRTPHintPacket -> createRTPReferenceSample (
				ulMediaSampleIdentifier, usSampleOffset, usFragLength) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_RTPHINTPACKET_CREATERTPREFERENCESAMPLE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				if (prhsRTPHintSample -> finish () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTSAMPLE_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete prhsRTPHintSample;
				prhsRTPHintSample			= (RTPHintSample_p) NULL;

				return err;
			}
	
			// update ulCurrentHintBytesNumber according
			// this RTP hint packet
			// statistics: update max packet size if necessary
			// (pmaPmaxAtom -> setBytes)
			// statistics: update trpy according the bytes sent including
			// RTP headers (pmaTrpyAtom -> incrementBytes)
			{
				MP4PmaxAtom_p	pmaPmaxAtom;
				unsigned long	ulMaxPayloadAndPacketPropertiesBytesNumber;
				unsigned long	ulPayloadAndPacketPropertiesBytesNumber;


				if (prpRTPHintPacket ->
					getPayloadAndPacketPropertiesBytesNumber (
					&ulPayloadAndPacketPropertiesBytesNumber) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_RTPHINTPACKET_GETPAYLOADANDPACKETPROPERTIESBYTESNUMBER_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample			= (RTPHintSample_p) NULL;

					return err;
				}

				ulCurrentHintBytesNumber				+=
					ulPayloadAndPacketPropertiesBytesNumber;

				if (pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:pmax:0", true,
					&pmaAtom) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_SEARCHATOM_FAILED,
						1, "udta:0:hinf:0:pmax:0");
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample			= (RTPHintSample_p) NULL;

					return err;
				}
				else
					pmaPmaxAtom	= (MP4PmaxAtom_p) pmaAtom;

				if (pmaPmaxAtom -> getBytes (
					&ulMaxPayloadAndPacketPropertiesBytesNumber) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample			= (RTPHintSample_p) NULL;

					return err;
				}

				if (ulPayloadAndPacketPropertiesBytesNumber >
					ulMaxPayloadAndPacketPropertiesBytesNumber)
				{
					if (pmaPmaxAtom -> setBytes (
						ulPayloadAndPacketPropertiesBytesNumber) !=
						errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4PROPERTY_SETVALUE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (prhsRTPHintSample -> finish () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_RTPHINTSAMPLE_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						delete prhsRTPHintSample;
						prhsRTPHintSample		= (RTPHintSample_p) NULL;

						return err;
					}
				}

				{
					MP4TrpyAtom_p			pmaTrpyAtom;


					if (pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:trpy:0",
						true, &pmaAtom) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_SEARCHATOM_FAILED,
							1, "udta:0:hinf:0:trpy:0");
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (prhsRTPHintSample -> finish () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_RTPHINTSAMPLE_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						delete prhsRTPHintSample;
						prhsRTPHintSample			= (RTPHintSample_p) NULL;

						return err;
					}
					else
						pmaTrpyAtom	= (MP4TrpyAtom_p) pmaAtom;

					if (pmaTrpyAtom -> incrementBytes (
						ulPayloadAndPacketPropertiesBytesNumber) !=
						errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (prhsRTPHintSample -> finish () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_RTPHINTSAMPLE_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						delete prhsRTPHintSample;
						prhsRTPHintSample			= (RTPHintSample_p) NULL;

						return err;
					}
				}
			}

			// statistics: update tpyl according the bytes sent excluding
			// RTP headers (pmaTpylAtom -> incrementBytes)
			{
				unsigned long							ulPacketPayloadSize;
				MP4TpylAtom_p							pmaTpylAtom;


				if (prpRTPHintPacket -> getPayloadBytesNumber (
					&ulPacketPayloadSize) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTPACKET_GETPAYLOADBYTESNUMBER_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample			= (RTPHintSample_p) NULL;

					return err;
				}

				if (pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:tpyl:0",
					true, &pmaAtom) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_SEARCHATOM_FAILED,
						1, "udta:0:hinf:0:tpyl:0");
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample			= (RTPHintSample_p) NULL;

					return err;
				}
				else
					pmaTpylAtom	= (MP4TpylAtom_p) pmaAtom;

				if (pmaTpylAtom -> incrementBytes (ulPacketPayloadSize) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample			= (RTPHintSample_p) NULL;

					return err;
				}
			}

			usSampleOffset				+= usFragLength;
	
			if (ulMediaSampleSize - usSampleOffset > ulMaxPayloadSize)
			{
				usFragLength				= (unsigned short) ulMaxPayloadSize; 

				if (prhsRTPHintSample -> createRTPHintPacket (
					&prpRTPHintPacket) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTSAMPLE_CREATERTPHINTPACKET_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample			= (RTPHintSample_p) NULL;

					return err;
				}

				if (prpRTPHintPacket -> setBFrame (false) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTPACKET_SETBFRAME_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample			= (RTPHintSample_p) NULL;

					return err;
				}

				// means no TVL information
				if (prpRTPHintPacket -> setTLVTimestampOffset (0) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTPACKET_SETTLVTIMESTAMPOFFSET_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample			= (RTPHintSample_p) NULL;

					return err;
				}

				if (prpRTPHintPacket -> setPayloadType (ulPayloadNumber) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTPACKET_SETPAYLOADTYPE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample			= (RTPHintSample_p) NULL;

					return err;
				}

				if (prpRTPHintPacket -> setSequenceNumber (
					(*pulCurrentHintPacketIdentifier)++) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTPACKET_SETSEQUENCENUMBER_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample			= (RTPHintSample_p) NULL;

					return err;
				}

				if (prpRTPHintPacket -> setMarkerBit (false) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTPACKET_SETMARKERBIT_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample			= (RTPHintSample_p) NULL;

					return err;
				}
			}
			else
			{
				usFragLength			= (unsigned short) (ulMediaSampleSize - usSampleOffset); 

				if (usFragLength)
				{
					if (prhsRTPHintSample -> createRTPHintPacket (
						&prpRTPHintPacket) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_CREATERTPHINTPACKET_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (prhsRTPHintSample -> finish () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_RTPHINTSAMPLE_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						delete prhsRTPHintSample;
						prhsRTPHintSample			= (RTPHintSample_p) NULL;

						return err;
					}

					if (prpRTPHintPacket -> setBFrame (false) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTPACKET_SETBFRAME_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (prhsRTPHintSample -> finish () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_RTPHINTSAMPLE_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						delete prhsRTPHintSample;
						prhsRTPHintSample			= (RTPHintSample_p) NULL;

						return err;
					}

					// means no TVL information
					if (prpRTPHintPacket -> setTLVTimestampOffset (0) !=
						errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTPACKET_SETTLVTIMESTAMPOFFSET_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (prhsRTPHintSample -> finish () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_RTPHINTSAMPLE_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						delete prhsRTPHintSample;
						prhsRTPHintSample			= (RTPHintSample_p) NULL;

						return err;
					}

					if (prpRTPHintPacket -> setPayloadType (ulPayloadNumber) !=
						errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTPACKET_SETPAYLOADTYPE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (prhsRTPHintSample -> finish () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_RTPHINTSAMPLE_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						delete prhsRTPHintSample;
						prhsRTPHintSample			= (RTPHintSample_p) NULL;

						return err;
					}

					if (prpRTPHintPacket -> setSequenceNumber (
						(*pulCurrentHintPacketIdentifier)++) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTPACKET_SETSEQUENCENUMBER_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (prhsRTPHintSample -> finish () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_RTPHINTSAMPLE_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						delete prhsRTPHintSample;
						prhsRTPHintSample			= (RTPHintSample_p) NULL;

						return err;
					}

					if (prpRTPHintPacket -> setMarkerBit (true) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTPACKET_SETMARKERBIT_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (prhsRTPHintSample -> finish () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_RTPHINTSAMPLE_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						delete prhsRTPHintSample;
						prhsRTPHintSample			= (RTPHintSample_p) NULL;

						return err;
					}
				}
			}
		}
		while (usSampleOffset < ulMediaSampleSize);

		// update ulCurrentHintBytesNumber according
		// this RTP hint packet
		// statistics: update max packet size if necessary
		// (pmaPmaxAtom -> setBytes)
		// statistics: update trpy according the bytes sent including
		// RTP headers (pmaTrpyAtom -> incrementBytes)
		{
			MP4PmaxAtom_p	pmaPmaxAtom;
			unsigned long	ulMaxPayloadAndPacketPropertiesBytesNumber;
			unsigned long	ulPayloadAndPacketPropertiesBytesNumber;


			if (prpRTPHintPacket ->
				getPayloadAndPacketPropertiesBytesNumber (
				&ulPayloadAndPacketPropertiesBytesNumber) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_RTPHINTPACKET_GETPAYLOADANDPACKETPROPERTIESBYTESNUMBER_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (prhsRTPHintSample -> finish () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTSAMPLE_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete prhsRTPHintSample;
				prhsRTPHintSample			= (RTPHintSample_p) NULL;

				return err;
			}

			ulCurrentHintBytesNumber				+=
				ulPayloadAndPacketPropertiesBytesNumber;

			if (pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:pmax:0",
				true, &pmaAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED,
					1, "udta:0:hinf:0:pmax:0");
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (prhsRTPHintSample -> finish () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTSAMPLE_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete prhsRTPHintSample;
				prhsRTPHintSample			= (RTPHintSample_p) NULL;

				return err;
			}
			else
				pmaPmaxAtom	= (MP4PmaxAtom_p) pmaAtom;

			if (pmaPmaxAtom -> getBytes (
				&ulMaxPayloadAndPacketPropertiesBytesNumber) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (prhsRTPHintSample -> finish () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTSAMPLE_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete prhsRTPHintSample;
				prhsRTPHintSample			= (RTPHintSample_p) NULL;

				return err;
			}

			if (ulPayloadAndPacketPropertiesBytesNumber >
				ulMaxPayloadAndPacketPropertiesBytesNumber)
			{
				if (pmaPmaxAtom -> setBytes (
					ulPayloadAndPacketPropertiesBytesNumber) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_SETVALUE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample		= (RTPHintSample_p) NULL;

					return err;
				}
			}

			{
				MP4TrpyAtom_p			pmaTrpyAtom;


				if (pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:trpy:0",
					true, &pmaAtom) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_SEARCHATOM_FAILED,
						1, "udta:0:hinf:0:trpy:0");
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample			= (RTPHintSample_p) NULL;

					return err;
				}
				else
					pmaTrpyAtom	= (MP4TrpyAtom_p) pmaAtom;

				if (pmaTrpyAtom -> incrementBytes (
					ulPayloadAndPacketPropertiesBytesNumber) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (prhsRTPHintSample -> finish () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_RTPHINTSAMPLE_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete prhsRTPHintSample;
					prhsRTPHintSample			= (RTPHintSample_p) NULL;

					return err;
				}
			}
		}

		// statistics: update tpyl according the bytes sent excluding
		// RTP headers (pmaTpylAtom -> incrementBytes)
		{
			unsigned long							ulPacketPayloadSize;
			MP4TpylAtom_p							pmaTpylAtom;


			if (prpRTPHintPacket -> getPayloadBytesNumber (
				&ulPacketPayloadSize) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_RTPHINTPACKET_GETPAYLOADBYTESNUMBER_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (prhsRTPHintSample -> finish () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTSAMPLE_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete prhsRTPHintSample;
				prhsRTPHintSample			= (RTPHintSample_p) NULL;

				return err;
			}

			if (pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:tpyl:0",
				true, &pmaAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED,
					1, "udta:0:hinf:0:tpyl:0");
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (prhsRTPHintSample -> finish () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTSAMPLE_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete prhsRTPHintSample;
				prhsRTPHintSample			= (RTPHintSample_p) NULL;

				return err;
			}
			else
				pmaTpylAtom	= (MP4TpylAtom_p) pmaAtom;

			if (pmaTpylAtom -> incrementBytes (ulPacketPayloadSize) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (prhsRTPHintSample -> finish () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTSAMPLE_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete prhsRTPHintSample;
				prhsRTPHintSample			= (RTPHintSample_p) NULL;

				return err;
			}
		}
	}

	// MP4WriteRtpHint(mp4File, hintTrackId, sampleDuration);

	// statistics: update dimm according the immediate data bytes sent
	// (pmaDimmAtom -> incrementBytes)
	{
		unsigned long							ulPacketImmediateDataSize;
		MP4DimmAtom_p							pmaDimmAtom;


		if (prpRTPHintPacket -> getImmediateDataBytesNumber (
			&ulPacketImmediateDataSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_RTPHINTPACKET_GETIMMEDIATEDATABYTESNUMBER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (prhsRTPHintSample -> finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_RTPHINTSAMPLE_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			delete prhsRTPHintSample;
			prhsRTPHintSample			= (RTPHintSample_p) NULL;

			return err;
		}

		if (pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:dimm:0",
			true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "udta:0:hinf:0:dimm:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (prhsRTPHintSample -> finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_RTPHINTSAMPLE_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			delete prhsRTPHintSample;
			prhsRTPHintSample			= (RTPHintSample_p) NULL;

			return err;
		}
		else
			pmaDimmAtom	= (MP4DimmAtom_p) pmaAtom;

		if (pmaDimmAtom -> incrementBytes (ulPacketImmediateDataSize) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (prhsRTPHintSample -> finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_RTPHINTSAMPLE_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			delete prhsRTPHintSample;
			prhsRTPHintSample			= (RTPHintSample_p) NULL;

			return err;
		}
	}

	// statistics: update dmed according the bytes sent from media data
	// (pmaDmedAtom -> incrementBytes)
	{
		unsigned long							ulPacketMediaDataSize;
		MP4DmedAtom_p							pmaDmedAtom;


		if (prpRTPHintPacket -> getMediaDataBytesNumber (
			&ulPacketMediaDataSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_RTPHINTPACKET_GETMEDIADATABYTESNUMBER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (prhsRTPHintSample -> finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_RTPHINTSAMPLE_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			delete prhsRTPHintSample;
			prhsRTPHintSample			= (RTPHintSample_p) NULL;

			return err;
		}

		if (pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:dmed:0",
			true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "udta:0:hinf:0:dmed:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (prhsRTPHintSample -> finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_RTPHINTSAMPLE_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			delete prhsRTPHintSample;
			prhsRTPHintSample			= (RTPHintSample_p) NULL;

			return err;
		}
		else
			pmaDmedAtom	= (MP4DmedAtom_p) pmaAtom;

		if (pmaDmedAtom -> incrementBytes (ulPacketMediaDataSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (prhsRTPHintSample -> finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_RTPHINTSAMPLE_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			delete prhsRTPHintSample;
			prhsRTPHintSample			= (RTPHintSample_p) NULL;

			return err;
		}
	}

	{
		{
			Message msg = MP4FileMessages (__FILE__, __LINE__,
				MP4F_MP4FILE_APPENDHINTSAMPLETOMDATATOM, 2,
				(*pulCurrentHintSampleIdentifier),
				ullHintDuration);
			ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (MP4Utility:: appendHintSampleToMdatAtom (pmaRootAtom,
			pmaHintTrakAtom, prhsRTPHintSample,
			*pulCurrentHintSampleIdentifier, ullHintDuration,
			ulCurrentHintBytesNumber, pmaMdatAtom, true,
			pulAllChunksBufferSize, pulCurrentChunkBufferSize,
			pulCurrentChunkSamplesNumber, pullCurrentChunkDuration,
			pullThisSec, pulBytesThisSec, pullCurrentChunkOffset, ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_APPENDHINTSAMPLETOMDATATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (prhsRTPHintSample -> finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_RTPHINTSAMPLE_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			delete prhsRTPHintSample;
			prhsRTPHintSample			= (RTPHintSample_p) NULL;

			return err;
		}
	}

	if (prhsRTPHintSample -> finish () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_RTPHINTSAMPLE_FINISH_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);
	}

	delete prhsRTPHintSample;
	prhsRTPHintSample				= (RTPHintSample_p) NULL;



	return errNoError;
}
*/

