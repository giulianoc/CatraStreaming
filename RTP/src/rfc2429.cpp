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

#include "rfc2429.h"
#include "MP4DimmAtom.h"
#include "MP4DmedAtom.h"
#include "MP4S263Atom.h"
#include "MP4MaxrAtom.h"
#include "MP4MdatAtom.h"
#include "MP4MdhdAtom.h"
#include "MP4PmaxAtom.h"
#include "MP4RtpAtom.h"
#include "MP4SdpAtom.h"
#include "MP4TrpyAtom.h"
#include "MP4TpylAtom.h"
#include "MP4Utility.h"
#include "RTPHintCreator.h"
#include "RTPMessages.h"
#include "RTPHintPacketSampleData.h"
#include "RTPUtility.h"
#include "SDPMediaFor3GPP.h"
#include <stdio.h>
#ifdef WIN32
#else
	#include <netinet/in.h>
#endif


Error rfc2429VisualHinter (MP4RootAtom_p pmaRootAtom,
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
	MP4S263Atom_p					pmaMediaS263Atom;
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
	std:: vector<MP4TrackInfo_p>:: const_iterator   itHintTrackInserted;



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
			MP4F_MP4TRACKINFO_GETOBJECTTYPEINDICATION_FAILED);
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

	if (pmtiMediaTrackInfo -> getS263Atom (
		&pmaMediaS263Atom, true) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETS263ATOM_FAILED);
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

	if (RTPUtility:: getRTPTimeScale (
		pmaMediaTrakAtom,
		pmaMediaMdhdAtom,
		cCodec,
		0, // ulObjectTypeIndication is not used for H263
		sStandard, &ulRTPTimeScale,
		ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPUTILITY_GETRTPTIMESCALE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// if (sStandard == MP4Atom:: MP4F_3GPP)
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
				RTP_RTPUTILITY_GETUSHORTRANDOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		ulSequenceNumberRandomOffset			= usSequenceNumberRandomOffset;
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
		SDPMedia_p			pSDPVideoMedia;
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
			pSDPVideoMedia = new SDPMediaFor3GPP_t;
		else
			pSDPVideoMedia = new SDPMediaForRTSP_t;

		if (pSDPVideoMedia == (SDPMedia_p) NULL)
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

		if (SDPMediaFor3GPP:: getSDPMediaVideoFromMediaInfo (
			pmtiMediaTrackInfo, pmtiHintTrackInfo, ulAvgBitRate, ulRTPTimeScale,
			0, // ucVideoSystemsProfileLevel not used because cCodec is H263
			pPayloadNumber, sStandard, ptTracer,
			pSDPVideoMedia) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIAFOR3GPP_GETSDPMEDIAVIDEOFROMMEDIAINFO_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete pSDPVideoMedia;

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

			if (pSDPVideoMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPVideoMedia;

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

		if (pSDPVideoMedia -> appendToBuffer (&bSDP) != errNoError)
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

			if (pSDPVideoMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPVideoMedia;

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

			if (pSDPVideoMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPVideoMedia;

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

			if (pSDPVideoMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPVideoMedia;

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

		if (pSDPVideoMedia -> getMediaInfo (
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

			if (pSDPVideoMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPVideoMedia;

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

			if (pSDPVideoMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPVideoMedia;

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

			if (pSDPVideoMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPVideoMedia;

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

			if (pSDPVideoMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPVideoMedia;

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

			if (pSDPVideoMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPVideoMedia;

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

				if (pSDPVideoMedia -> finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPMEDIA_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete pSDPVideoMedia;

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

				if (pSDPVideoMedia -> finish () != errNoError)
				{
					Error err = SDPErrors (__FILE__, __LINE__,
						SDP_SDPMEDIA_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete pSDPVideoMedia;

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

			if (pSDPVideoMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPVideoMedia;

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

			if (pSDPVideoMedia -> finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDPMEDIA_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pSDPVideoMedia;

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

		if (pSDPVideoMedia -> finish () != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete pSDPVideoMedia;

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

		delete pSDPVideoMedia;

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
//			__int64						ullSampleBufferToRead;
		#else
			unsigned long long			ullMediaSampleStartTime;
			unsigned long long			ullMediaSampleDuration;
//			unsigned long long			ullSampleBufferToRead;
		#endif
		unsigned long				ulMediaSampleIdentifier;
		unsigned long				ulCurrentMediaSampleSize;
//		unsigned long				ulMediaSampleRenderingOffset;
		Boolean_t					bIsSyncMediaSample;
		Boolean_t					bIsBFrame;
		unsigned long				ulCurrentMediaSampleOffset;
		unsigned long				ulRemainingMediaSampleSize;
		Boolean_t					bIsLastPacket;
		unsigned long				ulRTPHintPacketDataLength;
		Boolean_t					bIsStartingTheSample;


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
				(unsigned long *) NULL,	// &ulMediaSampleRenderingOffset,
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

			/* ????????????????????
				bisogna capire quando abbiamo un B-frame.
				Per ora lo inizializziamo a false
			ucMpeg4VopType		= MP4AV_Mpeg4GetVopType (pucMediaSampleBuffer,
				ulCurrentMediaSampleSize);

			bIsBFrame			= (ucMpeg4VopType == 'B') ? true : false;
			*/
			bIsBFrame			= false;

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

			// If the RTP packet starts with the beginning of the sample I shall replace
			//	the first two bytes of the sample from 0x000080 to 0x040080.
			//	To do this I add an immediate RTP data for the 0x0400 bytes and
			//	I start to reference the sample starting from the byte number three
			// otherwise if the RTP packet doesn't start with the beginning of the sample,
			//	I shall add 0x0000
			ulCurrentMediaSampleOffset			= 2;
			ulRemainingMediaSampleSize			= ulCurrentMediaSampleSize;

			ulRemainingMediaSampleSize			-= 2;

			bIsStartingTheSample			= true;

			while (ulRemainingMediaSampleSize)
			{
				if (ulRemainingMediaSampleSize <= ulHintMaxPayloadSize -
//					(bIsStartingTheSample ? 0 : 2))
					2)
				{
					ulRTPHintPacketDataLength	= ulRemainingMediaSampleSize;
					bIsLastPacket				= true;
				}
				else
				{
					ulRTPHintPacketDataLength	= ulHintMaxPayloadSize -
//						(bIsStartingTheSample ? 0 : 2);
						2;
					bIsLastPacket				= false;
				}

				if (rhcRTPHintCreator. addRTPHintPacketToCurrentRTPHintSample (
					bIsBFrame, 0, ulPayloadNumber, bIsLastPacket) != errNoError)
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

				if (bIsStartingTheSample)
				{
					unsigned char					pucData [2];


					pucData [0]					= pucMediaSampleBuffer [0] | 0x04;
					pucData [1]					= 0x00;

					if (rhcRTPHintCreator. addRTPImmediateDataToCurrentRTPHintPacket (
						pucData, 2) != errNoError)
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

					bIsStartingTheSample			= false;
				}
				else
				{
					unsigned char					pucData [2];


					pucData [0]					= 0x00;
					pucData [1]					= 0x00;
 
					if (rhcRTPHintCreator. addRTPImmediateDataToCurrentRTPHintPacket (
						pucData, 2) != errNoError)
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

				if (rhcRTPHintCreator. addRTPReferenceSampleToCurrentRTPHintPacket (
					ulMediaSampleIdentifier, ulCurrentMediaSampleOffset,
					ulRTPHintPacketDataLength) != errNoError)
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

				ulCurrentMediaSampleOffset		+= ulRTPHintPacketDataLength;
				ulRemainingMediaSampleSize		-= ulRTPHintPacketDataLength;
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
				ullMediaSampleDuration, bIsSyncMediaSample,
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


Error rfc2429VisualPrefetcher (
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
	unsigned long ulSamplesNumberToPrefetch,
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
	#else
		unsigned long long			ullMediaSampleStartTime;
		unsigned long long			ullMediaSampleDuration;
//		unsigned long long			ullSampleBufferToRead;
		unsigned long long			ullRelativeRTPPacketTransmissionTimeWithTimeScale;
	#endif
	double						dRelativeRTPPacketTransmissionTimeInSecs;
	unsigned long				ulCurrentMediaSampleSize;
//	unsigned long				ulMediaSampleRenderingOffset;
//	Boolean_t					bIsSyncMediaSample;
	Boolean_t					bIsBFrame;
	unsigned long				ulCurrentMediaSampleOffset;
	unsigned long				ulRemainingMediaSampleSize;
	Boolean_t					bIsLastPacket;
	unsigned long				ulRTPHintPacketDataLength;
	unsigned long				ulTrackReferenceIndex;
	long						lTLVTimestampOffset;
	unsigned long				ulBytesPerCompressionBlock;
	unsigned long				ulSamplePerCompressionBlock;
	RTPPacket_p					prpRTPPacket;
	unsigned long				ulCurrentPrefetchedSamplesNumber;
	long						lRelativePacketTransmissionTime;
	unsigned char				pucData [2];
	Boolean_t					bIsStartingTheSample;
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

	ulCurrentPrefetchedSamplesNumber		= 0;

	for ( ;
		*pulCurrentMediaSampleIndex <= ulMediaSamplesNumber;
		(*pulCurrentMediaSampleIndex)++)
	{
		if (ulCurrentPrefetchedSamplesNumber >=
			ulSamplesNumberToPrefetch)
		{

			break;
		}

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

			return errNoError;
		}

		/*
		if (MP4Utility:: getPointerToSample (pmaRootAtom,
			pmtiMediaTrackInfo -> _pmaTrakAtom,
			*pulCurrentMediaSampleIndex,
			&pucMediaSampleBuffer, &ullSampleBufferToRead,
			&ulCurrentMediaSampleSize,
			&ullMediaSampleStartTime, &ullMediaSampleDuration,
			&ulMediaSampleRenderingOffset, &bIsSyncMediaSample,
			ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_GETPOINTERTOSAMPLE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return errNoError;
		}
		*/

		if (((double) ulCurrentMediaSampleSize) /
			((double) (ulMaxPayloadSize - 2)) >=
			(double) (pvFreeRTPPackets -> size ()))
		{
			if (ulCurrentPrefetchedSamplesNumber == 0)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RFC_RFC2429VISUALPREFETCHER_INSUFFICIENTRTPPACKETSNUMBER,
					3, (unsigned long) (pvFreeRTPPackets -> size ()),
					ulCurrentMediaSampleSize, ulMaxPayloadSize - 2);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		}

		bIsBFrame			= false;

		// If the RTP packet starts with the beginning of the sample I shall replace
		//	the first two bytes of the sample from 0x000080 to 0x040080.
		//	To do this I add an immediate RTP data for the 0x0400 bytes and
		//	I start to reference the sample starting from the byte number three
		// otherwise if the RTP packet doesn't start with the beginning of the sample,
		//	I shall add 0x0000
		ulCurrentMediaSampleOffset			= 2;
		ulRemainingMediaSampleSize			= ulCurrentMediaSampleSize;

		ulRemainingMediaSampleSize			-= 2;

		bIsStartingTheSample			= true;

		while (ulRemainingMediaSampleSize)
		{
			if (ulRemainingMediaSampleSize <= ulMaxPayloadSize -
//				(bIsStartingTheSample ? 0 : 2))
				2)
			{
				ulRTPHintPacketDataLength	= ulRemainingMediaSampleSize;
				bIsLastPacket				= true;
			}
			else
			{
				ulRTPHintPacketDataLength	= ulMaxPayloadSize -
//					(bIsStartingTheSample ? 0 : 2);
					2;
				bIsLastPacket				= false;
			}

			/*
			if (rhcRTPHintCreator. addRTPHintPacketToCurrentRTPHintSample (
				bIsBFrame, 0, ulPayloadNumber, bIsLastPacket) != errNoError)

			if (rhcRTPHintCreator. addRTPImmediateDataToCurrentRTPHintPacket (
				pucSampleSize, ulSampleSizeIndex) != errNoError)

			if (rhcRTPHintCreator. addRTPReferenceSampleToCurrentRTPHintPacket (
				*pulCurrentMediaSampleIndex, ulCurrentMediaSampleOffset,
				ulRTPHintPacketDataLength) != errNoError)
			*/

			if (bIsStartingTheSample)
			{
				pucData [0]					= pucMediaSampleBuffer [0] | 0x04;
				pucData [1]					= 0x00;

				bIsStartingTheSample			= false;
			}
			else
			{
				pucData [0]					= 0x00;
				pucData [1]					= 0x00;
			}

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

				if (prpRTPPacket -> setMarkerBit (bIsLastPacket) != errNoError)
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

				if (prpRTPPacket -> appendData (pucData, 2) !=
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

				if (RTPHintPacketSampleData:: appendPayloadDataToRTPPacket (
					prpRTPPacket, pmaRootAtom,
					pmtiMediaTrackInfo,
					ulTrackReferenceIndex, ulRTPHintPacketDataLength,
					*pulCurrentMediaSampleIndex, ulCurrentMediaSampleOffset,
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

				lRelativePacketTransmissionTime			= 0;

				dRelativeRTPPacketTransmissionTimeInSecs		=
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

				/*
				{
					#ifdef WIN32
						Message msg = MP4FileMessages (__FILE__, __LINE__,
							MP4F_MP4FILE_BUILDANDPREFETCHEDRTPPACKET,
							6,
							pTrackURI,
							*pdRelativeRTPPacketTransmissionTimeInSecs,
							(*pulCurrentSequenceNumber) - 1,
							bIsLastPacket ? "true" : "false",
							(unsigned long) ullRelativeRTPPacketTransmissionTimeWithTimeScale,
							(__int64) (*prpRTPPacket));
					#else
						Message msg = MP4FileMessages (__FILE__, __LINE__,
							MP4F_MP4FILE_BUILDANDPREFETCHEDRTPPACKET,
							6,
							pTrackURI,
							*pdRelativeRTPPacketTransmissionTimeInSecs,
							(*pulCurrentSequenceNumber) - 1,
							bIsLastPacket ? "true" : "false",
							(unsigned long) ullRelativeRTPPacketTransmissionTimeWithTimeScale,
							(unsigned long long) (*prpRTPPacket));
					#endif
					ptTracer -> trace (Tracer:: TRACER_LDBG6,
						(const char *) msg, __FILE__, __LINE__);
				}
				*/
			}

			ulCurrentMediaSampleOffset		+= ulRTPHintPacketDataLength;
			ulRemainingMediaSampleSize		-= ulRTPHintPacketDataLength;
		}

		ulCurrentPrefetchedSamplesNumber++;
	}


	return errNoError;
}

