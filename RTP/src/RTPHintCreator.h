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


#ifndef RTPHintCreator_h
	#define RTPHintCreator_h

	#include "Tracer.h"
	#include "RTPErrors.h"
	#include "MP4MdatAtom.h"
	#include "MP4RootAtom.h"
	#include "MP4TrackInfo.h"
	#include "RTPHintSample.h"
	#include "RTPHintPacket.h"


	typedef class RTPHintCreator {

		public:
			typedef enum RTPHintCreatorStatus {
				RHC_BUILDED,
				RHC_INITIALIZED,
				RHC_RTPHINTSAMPLEINITIALIZED,
				RHC_RTPHINTPACKETINITIALIZED,
				RHC_RTPHINTSAMPLECONCLUDED
			} RTPHintCreatorStatus_t, *RTPHintCreatorStatus_p;


		protected:
			RTPHintCreatorStatus_t			_stRTPHintCreatorStatus;

			Tracer_p						_ptTracer;
			MP4RootAtom_p					_pmaRootAtom;
			MP4MdatAtom_p					_pmaMdatAtom;
			MP4TrackInfo_p					_pmtiMP4HintTrackInfo;

			unsigned long					_ulCurrentHintSampleIdentifier;
			unsigned long					_ulCurrentHintPacketIdentifier;
			unsigned long					_ulAllChunksBufferSize;
			unsigned long					_ulCurrentChunkBufferSize;
			unsigned long					_ulCurrentChunkSamplesNumber;
			unsigned long					_ulBytesThisSec;
			unsigned long					_ulCurrentHintBytesNumber;
			RTPHintSample_p					_prhsCurrentRTPHintSample;
			RTPHintPacket_p					_prpCurrentRTPHintPacket;
			#ifdef WIN32
				__int64						_ullCurrentChunkOffset;
				__int64						_ullCurrentChunkDuration;
				__int64						_ullThisSec;
			#else
				unsigned long long			_ullCurrentChunkOffset;
				unsigned long long			_ullCurrentChunkDuration;
				unsigned long long			_ullThisSec;
			#endif

			RTPHintCreator (const RTPHintCreator &);

			RTPHintCreator &operator = (const RTPHintCreator &);

		public:
			RTPHintCreator ();

			virtual ~RTPHintCreator ();

			Error init (MP4TrackInfo_p pmtiMP4HintTrackInfo,
				MP4RootAtom_p pmaRootAtom, unsigned long ulMaxMediaSampleSize,
				Tracer_p ptTracer);

			Error finish (Boolean_t bCorrectlyFinish);

			Error addRTPHintSample (void);

			Error addRTPHintPacketToCurrentRTPHintSample (
				Boolean_t bIsBFrame, unsigned long ulTLVTimestampOffset,
				unsigned long ulPayloadNumber, Boolean_t bMarkerBit);

			Error addRTPReferenceSampleToCurrentRTPHintPacket (
				unsigned long ulMediaSampleIdentifier,
				unsigned long ulDataOffset, unsigned long ulDataLength);

			Error addRTPImmediateDataToCurrentRTPHintPacket (
				unsigned char *pucImmediateData,
				unsigned long ulBytesNumber);

			Error updateStatisticsForRTPHintPacket (void);

			Error updateStatisticsForRTPHintSample (void);

			#ifdef WIN32
				Error appendRTPHintSampleToMdatAtom (
					__int64 ullHintDuration,
					Boolean_t bIsSyncHintSample,
					RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo);
			#else
				Error appendRTPHintSampleToMdatAtom (
					unsigned long long ullHintDuration,
					Boolean_t bIsSyncHintSample,
					RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo);
			#endif

	} RTPHintCreator_t, *RTPHintCreator_p;

#endif
