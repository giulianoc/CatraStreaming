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

#include "rfc3267.h"
#include "MP4DimmAtom.h"
#include "MP4DmedAtom.h"
#include "MP4MaxrAtom.h"
#include "MP4MdatAtom.h"
#include "MP4MdhdAtom.h"
#include "MP4PmaxAtom.h"
#include "MP4RtpAtom.h"
#include "MP4SdpAtom.h"
#include "MP4TrpyAtom.h"
#include "MP4TpylAtom.h"
#include "MP4Utility.h"
#include "RTPMessages.h"
#include "RTPHintCreator.h"
#include "RTPHintPacketSampleData.h"
#include "RTPUtility.h"
#include "SDPMediaFor3GPP.h"
#include <stdio.h>
#ifdef WIN32
#else
	#include <netinet/in.h>
#endif


/*
Error rfc3267AudioHinter (MP4RootAtom_p pmaRootAtom,
	MP4TrackInfo_p pmtiMediaTrackInfo,
	unsigned long ulMaxPayloadSize,
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
	// unsigned long					ulObjectTypeIndication;
	MP4SttsAtom_p					pmaMediaSttsAtom;
	MP4HdlrAtom_p					pmaMediaHdlrAtom;
	unsigned long					ulMediaTrackIdentifier;
	MP4StscAtom_p					pmaMediaStscAtom;
	MP4StcoAtom_p					pmaMediaStcoAtom;
	MP4CttsAtom_p					pmaMediaCttsAtom;
	MP4StssAtom_p					pmaMediaStssAtom;
	Error_t							errGetStss;
	Error_t							errGetCtts;

	MP4TrakAtom_p					pmaHintTrakAtom;
	unsigned long					ulHintTrackIdentifier;
	MP4SdpAtom_p					pmaHintSdpAtom;
	MP4RtpAtom_p					pmaHintRtpAtom;

	unsigned long					ulPayloadNumber;
	unsigned long					ulRTPTimeScale;
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

	if (pmtiMediaTrackInfo -> getStscAtom (&pmaMediaStscAtom, true) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSTSCATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getStcoAtom (&pmaMediaStcoAtom, true) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSTCOATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errGetCtts = pmtiMediaTrackInfo -> getCttsAtom (
		&pmaMediaCttsAtom, true)) != errNoError)
	{
		if ((long) errGetCtts == MP4F_MP4ATOM_ATOMNOTFOUND)
			pmaMediaCttsAtom		= (MP4CttsAtom_p) NULL;
		else
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETCTTSATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if ((errGetStss = pmtiMediaTrackInfo -> getStssAtom (
		&pmaMediaStssAtom, true)) != errNoError)
	{
		if ((long) errGetStss == MP4F_MP4ATOM_ATOMNOTFOUND)
			pmaMediaStssAtom		= (MP4StssAtom_p) NULL;
		else
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSSATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
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

	if (RTPUtility:: getRTPTimeScale (
		pmaMediaTrakAtom,
		pmaMediaMdhdAtom, cCodec,
		0, // ulObjectTypeIndication is not used for AMR codec
		sStandard, &ulRTPTimeScale,
		ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPUTILITY_GETRTPTIMESCALE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
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
			pmtiMediaTrackInfo,
			pmtiHintTrackInfo,
			ulAvgBitRate, ulRTPTimeScale,
			pPayloadNumber,
			0, // ucChannels is used only if sStandard is MP4F_ISMA and
				// cAudioCodecUsed is MP4F_CODEC_AAC. It is not our case.
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

	{
		RTPHintCreator_t			rhcRTPHintCreator;
		unsigned char				*pucMediaSampleBuffer;

		unsigned long				ulHintMaxPayloadSize;
		#ifdef WIN32
			__int64						ullMediaSampleStartTime;
			__int64						ullMediaSampleDuration;
			__int64						ullCurrentHintSampleDuration;
//			__int64						ullSampleBufferToRead;
		#else
			unsigned long long			ullMediaSampleStartTime;
			unsigned long long			ullMediaSampleDuration;
			unsigned long long			ullCurrentHintSampleDuration;
//			unsigned long long			ullSampleBufferToRead;
		#endif
		unsigned long				ulMediaSampleIdentifier;
		unsigned long				ulCurrentMediaSampleSize;
//		unsigned long				ulMediaSampleRenderingOffset;
		Boolean_t					bIsSyncMediaSample;
		Boolean_t					bIsStartingTheRTPPacket;
//		unsigned long				ulMediaSampleIdentifierOfLastRTPPacket;


		if (pmaHintRtpAtom -> getMaxPacketSize (&ulHintMaxPayloadSize) !=
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

		if (rhcRTPHintCreator. init (pmtiHintTrackInfo,
			pmaRootAtom, ulMaxMediaSampleSize, ptTracer) != errNoError)
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

		if ((pucMediaSampleBuffer = new unsigned char [
			ulMaxMediaSampleSize]) == (unsigned char *) NULL)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_NEW_FAILED);
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

		ullCurrentHintSampleDuration				= 0;

		for (ulMediaSampleIdentifier = 1;
			ulMediaSampleIdentifier <= ulMediaSamplesNumber;
			ulMediaSampleIdentifier++)
		{
			if (RTPUtility:: readSample (pmaRootAtom,
				pmaMediaStscAtom, pmaMediaStszAtom,
				pmaMediaStcoAtom, pmaMediaSttsAtom,
				pmaMediaCttsAtom, pmaMediaStssAtom,
				ulMediaSampleIdentifier,
				pucMediaSampleBuffer, ulMaxMediaSampleSize,
				&ulCurrentMediaSampleSize,
				&ullMediaSampleStartTime, &ullMediaSampleDuration,
				(unsigned long *) NULL,		// &ulMediaSampleRenderingOffset,
				&bIsSyncMediaSample,
				prsrtRTPStreamRealTimeInfo,
				ptTracer) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPUTILITY_READSAMPLE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] pucMediaSampleBuffer;
				pucMediaSampleBuffer	= (unsigned char *) NULL;

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

			if (ulMediaSampleIdentifier == 1)
			{
//				ulMediaSampleIdentifierOfLastRTPPacket		=
//					ulMediaSampleIdentifier;

				if (rhcRTPHintCreator. addRTPHintSample () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPHINTCREATOR_ADDRTPHINTSAMPLE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				if (rhcRTPHintCreator. addRTPHintPacketToCurrentRTPHintSample (
					false, 0, ulPayloadNumber, false) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTCREATOR_ADDRTPHINTPACKETTOCURRENTRTPHINTSAMPLE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				bIsStartingTheRTPPacket		= true;
			}
			else if ((ulMediaSampleIdentifier - 1) %
				RTP_MAXAMRFRAMESNUMBERFOREACHRTPPACHET == 0)
//			else if (ulMediaSampleIdentifier -
//				ulMediaSampleIdentifierOfLastRTPPacket == MP4_MAXSAMPLESFOREACHRTPPACHET)
			{
//				ulMediaSampleIdentifierOfLastRTPPacket		=
//					ulMediaSampleIdentifier;

				if (rhcRTPHintCreator. updateStatisticsForRTPHintPacket () !=
					errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPHINTCREATOR_UPDATESTATISTICSFORRTPHINTPACKET_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				if (rhcRTPHintCreator. updateStatisticsForRTPHintSample () !=
					errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPHINTCREATOR_UPDATESTATISTICSFORRTPHINTSAMPLE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				if (rhcRTPHintCreator. appendRTPHintSampleToMdatAtom (
					ullCurrentHintSampleDuration, bIsSyncMediaSample,
					prsrtRTPStreamRealTimeInfo) !=
					errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPHINTCREATOR_APPENDRTPHINTSAMPLETOMDATATOM_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				ullCurrentHintSampleDuration				= 0;

				if (rhcRTPHintCreator. addRTPHintSample () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPHINTCREATOR_ADDRTPHINTSAMPLE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				if (rhcRTPHintCreator. addRTPHintPacketToCurrentRTPHintSample (
					false, 0, ulPayloadNumber, false) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTCREATOR_ADDRTPHINTPACKETTOCURRENTRTPHINTSAMPLE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				bIsStartingTheRTPPacket		= true;
			}
			else
				;

			if (bIsStartingTheRTPPacket)
			{
				unsigned char				pucData [
					RTP_MAXAMRFRAMESNUMBERFOREACHRTPPACHET];
				unsigned long				ulBytesToWrite;
				unsigned long				ulDataIndex;


				pucData [0]					= 0xF0;

				if (ulMediaSamplesNumber - ulMediaSampleIdentifier >=
					RTP_MAXAMRFRAMESNUMBERFOREACHRTPPACHET)
				{
					// the second +1 is for 0xF0
					ulBytesToWrite			=
						RTP_MAXAMRFRAMESNUMBERFOREACHRTPPACHET - 1 + 1;

					for (ulDataIndex = 0; ulDataIndex <
						RTP_MAXAMRFRAMESNUMBERFOREACHRTPPACHET - 1;
						ulDataIndex++)
						pucData [ulDataIndex + 1]	= pucMediaSampleBuffer [0] | 0x80;
				}
				else
				{
					// the second +1 is for 0xF0
					ulBytesToWrite			=
						ulMediaSamplesNumber - ulMediaSampleIdentifier + 1;

					for (ulDataIndex = 0;
						ulDataIndex < ulMediaSamplesNumber - ulMediaSampleIdentifier;
						ulDataIndex++)
						pucData [ulDataIndex + 1]	= pucMediaSampleBuffer [0] | 0x80;
				}

				if (rhcRTPHintCreator. addRTPImmediateDataToCurrentRTPHintPacket (
					pucData, ulBytesToWrite) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTCREATOR_ADDRTPIMMEDIATEDATATOCURRENTRTPHINTPACKET_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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
			}

			// offset will be 1 to cut 0x34 in case we are not starting the RTP packet
			// otherwise will be 0 if we are starting the RTP packet
			if (rhcRTPHintCreator. addRTPReferenceSampleToCurrentRTPHintPacket (
				ulMediaSampleIdentifier, bIsStartingTheRTPPacket ? 0 : 1,
				bIsStartingTheRTPPacket ? ulCurrentMediaSampleSize :
				ulCurrentMediaSampleSize - 1) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTCREATOR_ADDRTPREFERENCESAMPLETOCURRENTRTPHINTPACKET_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] pucMediaSampleBuffer;
				pucMediaSampleBuffer	= (unsigned char *) NULL;

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

			if (bIsStartingTheRTPPacket)
				bIsStartingTheRTPPacket			= false;

			ullCurrentHintSampleDuration			+= ullMediaSampleDuration;
		}

		if (rhcRTPHintCreator. updateStatisticsForRTPHintPacket () !=
			errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTCREATOR_UPDATESTATISTICSFORRTPHINTPACKET_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] pucMediaSampleBuffer;
			pucMediaSampleBuffer	= (unsigned char *) NULL;

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

		if (rhcRTPHintCreator. updateStatisticsForRTPHintSample () !=
			errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTCREATOR_UPDATESTATISTICSFORRTPHINTSAMPLE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] pucMediaSampleBuffer;
			pucMediaSampleBuffer	= (unsigned char *) NULL;

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

		if (rhcRTPHintCreator. appendRTPHintSampleToMdatAtom (
			ullCurrentHintSampleDuration, bIsSyncMediaSample,
			prsrtRTPStreamRealTimeInfo) !=
			errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTCREATOR_APPENDRTPHINTSAMPLETOMDATATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] pucMediaSampleBuffer;
			pucMediaSampleBuffer	= (unsigned char *) NULL;

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

		delete [] pucMediaSampleBuffer;
		pucMediaSampleBuffer	= (unsigned char *) NULL;

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


	return errNoError;
}
*/


Error rfc3267AudioHinter (MP4RootAtom_p pmaRootAtom,
	MP4TrackInfo_p pmtiMediaTrackInfo,
	unsigned long ulMaxPayloadSize,
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
	// unsigned long					ulObjectTypeIndication;
	MP4SttsAtom_p					pmaMediaSttsAtom;
	MP4HdlrAtom_p					pmaMediaHdlrAtom;
	unsigned long					ulMediaTrackIdentifier;
	MP4StscAtom_p					pmaMediaStscAtom;
	MP4StcoAtom_p					pmaMediaStcoAtom;
	MP4CttsAtom_p					pmaMediaCttsAtom;
	MP4StssAtom_p					pmaMediaStssAtom;
	Error_t							errGetStss;
	Error_t							errGetCtts;

	MP4TrakAtom_p					pmaHintTrakAtom;
	unsigned long					ulHintTrackIdentifier;
	MP4SdpAtom_p					pmaHintSdpAtom;
	MP4RtpAtom_p					pmaHintRtpAtom;

	unsigned long					ulPayloadNumber;
	unsigned long					ulRTPTimeScale;
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

/*
	if (pmtiMediaTrackInfo -> getObjectTypeIndication (
		&ulObjectTypeIndication) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETCODEC_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}
*/

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

	if (pmtiMediaTrackInfo -> getStscAtom (&pmaMediaStscAtom, true) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSTSCATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getStcoAtom (&pmaMediaStcoAtom, true) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSTCOATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errGetCtts = pmtiMediaTrackInfo -> getCttsAtom (
		&pmaMediaCttsAtom, true)) != errNoError)
	{
		if ((long) errGetCtts == MP4F_MP4ATOM_ATOMNOTFOUND)
			pmaMediaCttsAtom		= (MP4CttsAtom_p) NULL;
		else
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETCTTSATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if ((errGetStss = pmtiMediaTrackInfo -> getStssAtom (
		&pmaMediaStssAtom, true)) != errNoError)
	{
		if ((long) errGetStss == MP4F_MP4ATOM_ATOMNOTFOUND)
			pmaMediaStssAtom		= (MP4StssAtom_p) NULL;
		else
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSSATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
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

	if (RTPUtility:: getRTPTimeScale (
		pmaMediaTrakAtom,
		pmaMediaMdhdAtom, cCodec,
		0, // ulObjectTypeIndication is not used for AMR codec
		sStandard, &ulRTPTimeScale,
		ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPUTILITY_GETRTPTIMESCALE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
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
			pmtiMediaTrackInfo,
			pmtiHintTrackInfo,
			ulAvgBitRate, ulRTPTimeScale,
			pPayloadNumber,
			0, // ucChannels is used only if sStandard is MP4F_ISMA and
				// cAudioCodecUsed is MP4F_CODEC_AAC. It is not our case.
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

	{
		RTPHintCreator_t			rhcRTPHintCreator;
		unsigned char				*pucMediaSampleBuffer;

		unsigned long				ulHintMaxPayloadSize;
		#ifdef WIN32
			__int64						ullMediaSampleStartTime;
			__int64						ullMediaSampleDuration;
//			__int64						ullCurrentHintSampleDuration;
//			__int64						ullSampleBufferToRead;
		#else
			unsigned long long			ullMediaSampleStartTime;
			unsigned long long			ullMediaSampleDuration;
//			unsigned long long			ullCurrentHintSampleDuration;
//			unsigned long long			ullSampleBufferToRead;
		#endif
		unsigned long				ulMediaSampleIdentifier;
		unsigned long				ulCurrentMediaSampleSize;
//		unsigned long				ulMediaSampleRenderingOffset;
		Boolean_t					bIsSyncMediaSample;
//		Boolean_t					bIsStartingTheRTPPacket;
//		unsigned long				ulMediaSampleIdentifierOfLastRTPPacket;
		RTPPacket_t					rpRTPPacket;
		unsigned long				ulAMRFramesNumberInCurrentRTPPacket;
		unsigned long				ulAMRFramesNumberInMediaSample;
		// unsigned char				pucLastAMRFrameType [1];
		unsigned long				ulTrackReferenceIndex;
		unsigned long				ulBytesPerCompressionBlock;
		unsigned long				ulSamplePerCompressionBlock;


		if (pmaHintRtpAtom -> getMaxPacketSize (&ulHintMaxPayloadSize) !=
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

		if (rhcRTPHintCreator. init (pmtiHintTrackInfo,
			pmaRootAtom, ulMaxMediaSampleSize, ptTracer) != errNoError)
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

		if ((pucMediaSampleBuffer = new unsigned char [
			ulMaxMediaSampleSize]) == (unsigned char *) NULL)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_NEW_FAILED);
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

		// Since the samples data must be modified to build each RTP packet,
		// we will build the RTP packet and after we will add a RTPImmediateData
		// for the RTP packet of the hint track
		// Therefore we use the below rpRTPPacket just to build the RTP packet
		if (rpRTPPacket. init (ulMaxPayloadSize,
			ptTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] pucMediaSampleBuffer;
			pucMediaSampleBuffer	= (unsigned char *) NULL;

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

		ulTrackReferenceIndex						= 0;
		ulBytesPerCompressionBlock					= 1;
		ulSamplePerCompressionBlock					= 1;

		// ullCurrentHintSampleDuration				= 0;

		ulAMRFramesNumberInCurrentRTPPacket			= 0;

		// this algotithm is not optimized because we create one RTP packet
		// for each media sample. Really each RTP packet should contain
		// RTP_MAXAMRFRAMESNUMBERFOREACHRTPPACHET AMR frames and in case
		// the media sample contain, for ex., just 1 AMR frame, the RTP packet
		// will be very empty.
		// The prefetcher is more optimized.
		for (ulMediaSampleIdentifier = 1;
			ulMediaSampleIdentifier <= ulMediaSamplesNumber;
			ulMediaSampleIdentifier++)
		{
			if (RTPUtility:: readSample (pmaRootAtom,
				pmaMediaStscAtom, pmaMediaStszAtom,
				pmaMediaStcoAtom, pmaMediaSttsAtom,
				pmaMediaCttsAtom, pmaMediaStssAtom,
				ulMediaSampleIdentifier,
				pucMediaSampleBuffer, ulMaxMediaSampleSize,
				&ulCurrentMediaSampleSize,
				&ullMediaSampleStartTime, &ullMediaSampleDuration,
				(unsigned long *) NULL,		// &ulMediaSampleRenderingOffset,
				&bIsSyncMediaSample,
				prsrtRTPStreamRealTimeInfo,
				ptTracer) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPUTILITY_READSAMPLE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (rpRTPPacket. finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete [] pucMediaSampleBuffer;
				pucMediaSampleBuffer	= (unsigned char *) NULL;

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
			/*
			if (MP4Utility:: getPointerToSample (pmaRootAtom,
				pmtiMediaTrackInfo -> _pmaTrakAtom,
				ulMediaSampleIdentifier,
				&pucMediaSampleBuffer, &ullSampleBufferToRead,
				&ulCurrentMediaSampleSize,
				&ullMediaSampleStartTime, &ullMediaSampleDuration,
				(unsigned long *) NULL,	// &ulMediaSampleRenderingOffset,
				&bIsSyncMediaSample,
				ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETPOINTERTOSAMPLE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] pucMediaSampleBuffer;
				pucMediaSampleBuffer	= (unsigned char *) NULL;

				if (rhcRTPHintCreator. finish (false) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_RTPHINTCREATOR_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return errNoError;
			}
			*/

			// ullMediaSampleDuration / (ulMdhdTimeScale / 1000)
			//		is the duration in millisecs of the sample
			// since each AMR frame is 20 millisecs, dividing for 20 we have the
			//		AMR frames number
			ulAMRFramesNumberInMediaSample			=
				ullMediaSampleDuration / (ulMdhdTimeScale / 1000) / 20;

			if (ulAMRFramesNumberInMediaSample >
				RTP_MAXAMRFRAMESNUMBERFOREACHRTPPACHET)
			{
				// the solution implemented is that all the AMR frames inside the
				// media sample will be added to the RTP packet. Basically we will not split
				// the media samples to different RTP packets.
				// If we would like to do that we should use another parameter other than
				// pulCurrentMediaSampleIndex, like pulCurrentMediaFrameIndex.
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RFC_TOOAMRFRAMESNUMBERINONEMDIASAMPLE,
					1, ulAMRFramesNumberInMediaSample);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (rpRTPPacket. finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete [] pucMediaSampleBuffer;
				pucMediaSampleBuffer	= (unsigned char *) NULL;

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

			{
				#ifdef WIN32
					__int64				ullAMRFrameTypeIndexInRTPPacket;
				#else
					unsigned long long	ullAMRFrameTypeIndexInRTPPacket;
				#endif
				unsigned long			ulAMRFrameIndex;
				unsigned char			pucTOC [
					RTP_MAXAMRFRAMESNUMBERFOREACHRTPPACHET + 1];


				if (rpRTPPacket. reset () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_RESET_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (rpRTPPacket. finish () != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPPACKET_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				if (RTPHintPacketSampleData:: appendPayloadDataToRTPPacket (
					&rpRTPPacket, pmaRootAtom,
					pmtiMediaTrackInfo,
					ulTrackReferenceIndex,
					ulCurrentMediaSampleSize,
					ulMediaSampleIdentifier, 0,
					ulBytesPerCompressionBlock,
					ulSamplePerCompressionBlock,
					prsrtRTPStreamRealTimeInfo,
					ptTracer) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETSAMPLEDATA_APPENDPAYLOADDATATORTPPACKET_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (rpRTPPacket. finish () != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPPACKET_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				if (rhcRTPHintCreator. addRTPHintSample () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPHINTCREATOR_ADDRTPHINTSAMPLE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (rpRTPPacket. finish () != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPPACKET_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				if (rhcRTPHintCreator.
					addRTPHintPacketToCurrentRTPHintSample (
					false, 0, ulPayloadNumber, false) !=
					errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
		RTP_RTPHINTCREATOR_ADDRTPHINTPACKETTOCURRENTRTPHINTSAMPLE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (rpRTPPacket. finish () != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPPACKET_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				pucTOC [0]					= 0xF0;

				for (ulAMRFrameIndex = 0;
					ulAMRFrameIndex < ulAMRFramesNumberInMediaSample;
					ulAMRFrameIndex++)
				{
					#ifdef WIN32
						ullAMRFrameTypeIndexInRTPPacket		=
							(__int64) rpRTPPacket -
							((ulAMRFramesNumberInMediaSample -
								ulAMRFrameIndex) *
							(ulCurrentMediaSampleSize /
								ulAMRFramesNumberInMediaSample));
					#else
						ullAMRFrameTypeIndexInRTPPacket		=
							(unsigned long long) rpRTPPacket -
							((ulAMRFramesNumberInMediaSample -
								ulAMRFrameIndex) *
							(ulCurrentMediaSampleSize /
								ulAMRFramesNumberInMediaSample));
					#endif

					// at the beginning of the RTP packet we have the TOC
					// made by the AMR frames type (first byte of each
					// AMR frame) with the first bit to 1
					if (ulAMRFrameIndex + 1 == ulAMRFramesNumberInMediaSample)
					{
						pucTOC [ulAMRFrameIndex + 1]		=
							((unsigned char *) rpRTPPacket) [
							ullAMRFrameTypeIndexInRTPPacket];
					}
					else
					{
						pucTOC [ulAMRFrameIndex + 1]		=
							((unsigned char *) rpRTPPacket) [
							ullAMRFrameTypeIndexInRTPPacket] | 0x80;
					}
				}

				if (rhcRTPHintCreator.
					addRTPImmediateDataToCurrentRTPHintPacket (
					pucTOC, ulAMRFramesNumberInMediaSample + 1) !=
					errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
		RTP_RTPHINTCREATOR_ADDRTPIMMEDIATEDATATOCURRENTRTPHINTPACKET_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (rpRTPPacket. finish () != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPPACKET_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				for (ulAMRFrameIndex = 0;
					ulAMRFrameIndex < ulAMRFramesNumberInMediaSample;
					ulAMRFrameIndex++)
				{
					if (rhcRTPHintCreator.
						addRTPReferenceSampleToCurrentRTPHintPacket (
						ulMediaSampleIdentifier,
						ulAMRFrameIndex * (ulCurrentMediaSampleSize /
						ulAMRFramesNumberInMediaSample) + 1,
						(ulCurrentMediaSampleSize /
						ulAMRFramesNumberInMediaSample) - 1) != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTCREATOR_UPDATESTATISTICSFORRTPHINTPACKET_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (rpRTPPacket. finish () != errNoError)
						{
							Error err = RTPErrors (__FILE__, __LINE__,
								RTP_RTPPACKET_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						delete [] pucMediaSampleBuffer;
						pucMediaSampleBuffer	= (unsigned char *) NULL;

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
				}

				if (rhcRTPHintCreator.
					updateStatisticsForRTPHintPacket () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTCREATOR_UPDATESTATISTICSFORRTPHINTPACKET_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (rpRTPPacket. finish () != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPPACKET_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				if (rhcRTPHintCreator.
					updateStatisticsForRTPHintSample () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTCREATOR_UPDATESTATISTICSFORRTPHINTSAMPLE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (rpRTPPacket. finish () != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPPACKET_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				if (rhcRTPHintCreator.
					appendRTPHintSampleToMdatAtom (
					ullMediaSampleDuration, bIsSyncMediaSample,
					// ullCurrentHintSampleDuration, bIsSyncMediaSample,
					prsrtRTPStreamRealTimeInfo) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTCREATOR_APPENDRTPHINTSAMPLETOMDATATOM_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (rpRTPPacket. finish () != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPPACKET_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete [] pucMediaSampleBuffer;
					pucMediaSampleBuffer	= (unsigned char *) NULL;

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

				// ullCurrentHintSampleDuration            += ullMediaSampleDuration;
			}
		}

		if (rpRTPPacket. finish () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] pucMediaSampleBuffer;
			pucMediaSampleBuffer	= (unsigned char *) NULL;

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

		delete [] pucMediaSampleBuffer;
		pucMediaSampleBuffer	= (unsigned char *) NULL;

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


	return errNoError;
}




Error rfc3267AudioPrefetcher (
	unsigned long ulMediaSamplesNumber,
	unsigned char *pucMediaSampleBuffer,
	unsigned long ulMaxMediaSampleSize,
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
	MP4StscAtom_p					pmaMediaStscAtom;
	MP4StcoAtom_p					pmaMediaStcoAtom;
	MP4CttsAtom_p					pmaMediaCttsAtom;
	MP4StssAtom_p					pmaMediaStssAtom;
	Error_t							errGetStss;
	Error_t							errGetCtts;

	#ifdef WIN32
		__int64						ullMediaSampleStartTime;
		__int64						ullMediaSampleDuration;
//		__int64						ullSampleBufferToRead;
		__int64						ullRelativeRTPPacketTransmissionTimeWithTimeScale;
//		__int64						ullCurrentHintSampleDuration;
	#else
		unsigned long long			ullMediaSampleStartTime;
		unsigned long long			ullMediaSampleDuration;
//		unsigned long long			ullSampleBufferToRead;
		unsigned long long			ullRelativeRTPPacketTransmissionTimeWithTimeScale;
//		unsigned long long			ullCurrentHintSampleDuration;
	#endif
	double						dRelativeRTPPacketTransmissionTimeInSecs;
	unsigned long				ulCurrentMediaSampleSize;
//	unsigned long				ulMediaSampleRenderingOffset;
//	Boolean_t					bIsSyncMediaSample;
	unsigned long				ulTrackReferenceIndex;
	long						lTLVTimestampOffset;
	unsigned long				ulBytesPerCompressionBlock;
	unsigned long				ulSamplePerCompressionBlock;
	RTPPacket_p					prpRTPPacket;
	unsigned long				ulCurrentPrefetchedRTPPacketsNumber;
	long						lRelativePacketTransmissionTime;
//	double						dOffsetFromStartTime;
	unsigned long				ulTOCBytesNumber;
	unsigned char				pucLastAMRFrameType [1];
	unsigned long				ulAMRFramesNumberInCurrentRTPPacket;
	unsigned long				ulAMRFramesNumberInMediaSample;


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

	if (pmtiMediaTrackInfo -> getStscAtom (&pmaMediaStscAtom, true) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSTSCATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrackInfo -> getStcoAtom (&pmaMediaStcoAtom, true) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSTCOATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errGetCtts = pmtiMediaTrackInfo -> getCttsAtom (
		&pmaMediaCttsAtom, true)) != errNoError)
	{
		if ((long) errGetCtts == MP4F_MP4ATOM_ATOMNOTFOUND)
			pmaMediaCttsAtom		= (MP4CttsAtom_p) NULL;
		else
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETCTTSATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if ((errGetStss = pmtiMediaTrackInfo -> getStssAtom (
		&pmaMediaStssAtom, true)) != errNoError)
	{
		if ((long) errGetStss == MP4F_MP4ATOM_ATOMNOTFOUND)
			pmaMediaStssAtom		= (MP4StssAtom_p) NULL;
		else
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSSATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	ulTrackReferenceIndex					= 0;
	ulBytesPerCompressionBlock				= 1;
	ulSamplePerCompressionBlock				= 1;

	ulCurrentPrefetchedRTPPacketsNumber		= 0;

	ulAMRFramesNumberInCurrentRTPPacket			= 0;

	prpRTPPacket				= (RTPPacket_p) NULL;

	// our goal is to add not more than 7 AMR frame to one RTP packet
	for ( ;
		*pulCurrentMediaSampleIndex <= ulMediaSamplesNumber;
		(*pulCurrentMediaSampleIndex)++)
	{
		if (RTPUtility:: readSample (pmaRootAtom,
			pmaMediaStscAtom, pmaMediaStszAtom,
			pmaMediaStcoAtom, pmaMediaSttsAtom,
			pmaMediaCttsAtom, pmaMediaStssAtom,
			*pulCurrentMediaSampleIndex,
			pucMediaSampleBuffer, ulMaxMediaSampleSize,
			&ulCurrentMediaSampleSize,
			&ullMediaSampleStartTime, &ullMediaSampleDuration,
			(unsigned long *) NULL,		// &ulMediaSampleRenderingOffset,
			(Boolean_p) NULL,	// &bIsSyncMediaSample,
			prsrtRTPStreamRealTimeInfo,
			ptTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_READSAMPLE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
		/*
		if (MP4Utility:: getPointerToSample (pmaRootAtom,
			pmtiMediaTrackInfo -> _pmaTrakAtom,
			*pulCurrentMediaSampleIndex,
			&pucMediaSampleBuffer, &ullSampleBufferToRead,
			&ulCurrentMediaSampleSize,
			&ullMediaSampleStartTime, &ullMediaSampleDuration,
			(unsigned long *) NULL,	// &ulMediaSampleRenderingOffset,
			&bIsSyncMediaSample,
			ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_GETPOINTERTOSAMPLE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
		*/

		// ullMediaSampleDuration / (ulMdhdTimeScale / 1000)
		//		is the duration in millisecs of the sample
		// since each AMR frame is 20 millisecs, dividing for 20 we have the
		//		AMR frames number
		ulAMRFramesNumberInMediaSample			=
			ullMediaSampleDuration / (ulMdhdTimeScale / 1000) / 20;

		if (ulAMRFramesNumberInMediaSample >
			RTP_MAXAMRFRAMESNUMBERFOREACHRTPPACHET)
		{
			// the solution implemented is that all the AMR frames inside the
			// media sample will be added to the RTP packet. Basically we will not split
			// the media samples to different RTP packets.
			// If we would like to do that we should use another parameter other than
			// pulCurrentMediaSampleIndex, like pulCurrentMediaFrameIndex.
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RFC_TOOAMRFRAMESNUMBERINONEMDIASAMPLE,
				1, ulAMRFramesNumberInMediaSample);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (ulAMRFramesNumberInCurrentRTPPacket +
			ulAMRFramesNumberInMediaSample >
			RTP_MAXAMRFRAMESNUMBERFOREACHRTPPACHET ||
			prpRTPPacket == (RTPPacket_p) NULL)		// first time
		{
			/* if (rhcRTPHintCreator. addRTPHintSample () != errNoError)
				if (rhcRTPHintCreator. addRTPHintPacketToCurrentRTPHintSample (
					false, 0, ulPayloadNumber, false) != errNoError)
			*/

			if (prpRTPPacket != (RTPPacket_p) NULL)
			{
				// end a RTP packet
				{
					/*
					dOffsetFromStartTime									=
						(*pdRelativeRTPPacketTransmissionTimeInSecs) -
						dRelativeRequestedStartTimeInSecs;

					dRelativeRTPPacketTransmissionTimeInMilliSecs	=
						dRelativeRequestedStartTimeInSecs +
						(dOffsetFromStartTime / dRequestedSpeed);

					if (dRelativeRTPPacketTransmissionTimeInMilliSecs < 0.0)
						dRelativeRTPPacketTransmissionTimeInMilliSecs	= 0.0;

					dRelativeRTPPacketTransmissionTimeInMilliSecs		=
						(dRelativeRTPPacketTransmissionTimeInMilliSecs * 1000);
					*/

					// the last AMR frame type in TOC must be
					//	without the first bit to 1 ( | 0x80)
					/*
					((unsigned char *) (*prpRTPPacket)) [
						ulTOCBytesNumber - 1 + RTP_RTPPACKETHEADERLENGTH]	=
						pucLastAMRFrameType [0];
					*/
					if (prpRTPPacket -> modifyData (
						ulTOCBytesNumber - 1 + RTP_RTPPACKETHEADERLENGTH,
						pucLastAMRFrameType, 1) != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPPACKET_INSERTDATA_FAILED);
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
								"false",
			(unsigned long) ullRelativeRTPPacketTransmissionTimeWithTimeScale,
								(__int64) (*prpRTPPacket));
						#else
							Message msg = MP4FileMessages (__FILE__, __LINE__,
								MP4F_MP4FILE_BUILDANDPREFETCHEDRTPPACKET,
								6,
								pTrackURI,
								*pdRelativeRTPPacketTransmissionTimeInSecs,
								(*pulCurrentSequenceNumber) - 1,
								"false",
			(unsigned long) ullRelativeRTPPacketTransmissionTimeWithTimeScale,
								(unsigned long long) (*prpRTPPacket));
						#endif
						ptTracer -> trace (Tracer:: TRACER_LDBG6,
							(const char *) msg, __FILE__, __LINE__);
					}
					*/

					if (ulCurrentPrefetchedRTPPacketsNumber >=
						ulRTPPacketsNumberToPrefetch)
					{

						break;
					}
				}
			}

			// start a RTP packet
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

				if (prpRTPPacket -> setMarkerBit (false) != errNoError)
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

				if (ulRTPTimeScale == ulMdhdTimeScale)
					ullRelativeRTPPacketTransmissionTimeWithTimeScale	=
						ullMediaSampleStartTime;
				else
					ullRelativeRTPPacketTransmissionTimeWithTimeScale	=
						ullMediaSampleStartTime *
						(((double) ulRTPTimeScale) /
						((double) ulMdhdTimeScale));

				// ullRelativeRTPPacketTransmissionTimeWithTimeScale		+=
				//	ulFirstEditRTPTime;

				lTLVTimestampOffset					= 0;

				ullRelativeRTPPacketTransmissionTimeWithTimeScale		+=
					lTLVTimestampOffset;

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

				lRelativePacketTransmissionTime			= 0;

				dRelativeRTPPacketTransmissionTimeInSecs			=
					((double) ullMediaSampleStartTime /
					((double) ulMdhdTimeScale)) +
					((double) lRelativePacketTransmissionTime /
					((double) ulMdhdTimeScale));

				ulAMRFramesNumberInCurrentRTPPacket				= 0;
			}

			{
				unsigned char			pucTOC [1];


				pucTOC [0]					= 0xF0;

				if (prpRTPPacket -> appendData (
					pucTOC, 1) != errNoError)
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

				ulTOCBytesNumber					= 1;
			}

//			ullCurrentHintSampleDuration				= 0;
		}

		{
			#ifdef WIN32
				__int64				ullAMRFrameTypeIndexInRTPPacket;
			#else
				unsigned long long	ullAMRFrameTypeIndexInRTPPacket;
			#endif
			unsigned long			ulAMRFrameIndex;


			if (RTPHintPacketSampleData:: appendPayloadDataToRTPPacket (
				prpRTPPacket, pmaRootAtom,
				pmtiMediaTrackInfo,
				ulTrackReferenceIndex,
				ulCurrentMediaSampleSize,
				*pulCurrentMediaSampleIndex, 0,
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

// problema di divisioni non intere???
			for (ulAMRFrameIndex = 0;
				ulAMRFrameIndex < ulAMRFramesNumberInMediaSample;
				ulAMRFrameIndex++)
			{
				#ifdef WIN32
					ullAMRFrameTypeIndexInRTPPacket		=
						(__int64) (*prpRTPPacket) -
						((ulAMRFramesNumberInMediaSample - ulAMRFrameIndex) *
						(ulCurrentMediaSampleSize / ulAMRFramesNumberInMediaSample));
				#else
					ullAMRFrameTypeIndexInRTPPacket		=
						(unsigned long long) (*prpRTPPacket) -
						((ulAMRFramesNumberInMediaSample - ulAMRFrameIndex) *
						(ulCurrentMediaSampleSize / ulAMRFramesNumberInMediaSample));
				#endif

				// at the beginning of the RTP packet we have the TOC
				// made by the AMR frames type (first byte of each AMR frame)
				// with the first bit to 1
				pucLastAMRFrameType [0]		=
					((unsigned char *) (*prpRTPPacket)) [
					ullAMRFrameTypeIndexInRTPPacket] | 0x80;

				if (prpRTPPacket -> insertData (
					ulTOCBytesNumber + RTP_RTPPACKETHEADERLENGTH,
					pucLastAMRFrameType, 1) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_INSERTDATA_FAILED);
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

				ulTOCBytesNumber++;

				// save the last AMR frame type
				pucLastAMRFrameType [0]		=
					((unsigned char *) (*prpRTPPacket)) [
					ullAMRFrameTypeIndexInRTPPacket + 1];

				// + 1 below is because I just inserted one char
				if (prpRTPPacket -> deleteData (
					ullAMRFrameTypeIndexInRTPPacket + 1, 1) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_DELETEDATA_FAILED);
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

				ulAMRFramesNumberInCurrentRTPPacket++;
			}
		}


//		ullCurrentHintSampleDuration			+= ullMediaSampleDuration;
	}


	return errNoError;
}

