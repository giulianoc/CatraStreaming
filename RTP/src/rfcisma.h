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

#ifndef rfcisma_h
	#define rfcisma_h

	#include "MP4MdatAtom.h"
	#include "MP4RootAtom.h"
	#include "MP4TrakAtom.h"
	// #include "MP4Utility.h"
	#include "RTPHintCreator.h"
	#include "RTPPacket.h"
	#include "Tracer.h"
	#include "MP4TrackInfo.h"
	#include "RTPErrors.h"

	#ifndef RTP_DEFAULTMAXRTPPAYLOADSIZE
		#define RTP_DEFAULTMAXRTPPAYLOADSIZE					1472
	#endif

	#define RTP_MAXCLOCKRATELENGTH					128 + 1
	#define RTP_MAXENCODINGPARAMETERSLENGTH			128 + 1
	/*
	#define MP4F_RFCISMAAUDIOPREFETCHER_SOURCE		"rfc isma Audio Prefetcher"
	#define MP4F_STREAMINGSERVERPROCESSOR_DESTINATION	"StreamingServerProcessor"
	*/

	Error rfcIsmaAudioHinter (MP4RootAtom_p pmaRootAtom,
		MP4TrackInfo_p pmtiMediaTrackInfo,
		unsigned long ulMaxPayloadSize, Boolean_t bInterleave,
		MP4TrackInfo_p pmtiHintTrackInfo,
		std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo,
		MP4Atom:: Standard_t sStandard,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer);

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
		unsigned long *pulCurrentSequenceNumber);

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
			Tracer_p ptTracer);
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
			Tracer_p ptTracer);
	#endif
	*/
#endif

