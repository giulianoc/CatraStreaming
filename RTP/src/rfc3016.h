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

#ifndef rfc3016_h
	#define rfc3016_h

	#include "MP4RootAtom.h"
	#include "MP4TrakAtom.h"
	#include "MP4TrackInfo.h"
	#include "RTPPacket.h"
	#include "RTPStreamRealTimeInfo.h"
	#include "Tracer.h"
	#include "RTPErrors.h"


	#ifndef RTP_DEFAULTMAXRTPPAYLOADSIZE
		#define RTP_DEFAULTMAXRTPPAYLOADSIZE					1472
	#endif

	#define MP4F_MPEG4_VOSH_START           0xB0
	/*
	#define MP4F_MPEG4_VOL_START            0x20
	#define MP4F_MPEG4_GOV_START            0xB3
	#define MP4F_MPEG4_VOP_START            0xB6
	*/

	/*
	#define MP4F_RFC3016VISUALPREFETCHER_SOURCE		"rfc 3016 Visual Prefetcher"
	#define MP4F_RFC3016AUDIOPREFETCHER_SOURCE		"rfc 3016 Audio Prefetcher"
	#define MP4F_STREAMINGSERVERPROCESSOR_DESTINATION	"StreamingServerProcessor"
	*/

	Error rfc3016VisualHinter (MP4RootAtom_p pmaRootAtom,
		MP4TrackInfo_p pmtiMediaTrackInfo,
		unsigned long ulMaxPayloadSize,
		MP4TrackInfo_p pmtiHintTrackInfo,
		std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo,
		MP4Atom:: Standard_t sStandard,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer);

	Error rfc3016VisualPrefetcher (
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
		unsigned long *pulCurrentSequenceNumber);

	Error rfc3016AudioHinter (MP4RootAtom_p pmaRootAtom,
		MP4TrackInfo_p pmtiMediaTrackInfo,
		unsigned long ulMaxPayloadSize,
		MP4TrackInfo_p pmtiHintTrackInfo,
		std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo,
		MP4Atom:: Standard_t sStandard,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer);

	Error rfc3016AudioPrefetcher (
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
		unsigned long *pulCurrentSequenceNumber);

#endif

