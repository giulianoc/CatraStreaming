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


#ifndef RTPHintPacketSampleData_h
	#define RTPHintPacketSampleData_h

	#include "RTPHintPacketData.h"
	#include "MP4TrackInfo.h"
	#include "Tracer.h"
	#include "MP4UInteger8BitsProperty.h"
	#include "MP4UInteger16BitsProperty.h"
	#include "MP4UInteger32BitsProperty.h"
	#include "RTPErrors.h"
	#include "RTPPacket.h"
	#include "RTPStreamRealTimeInfo.h"


	typedef class RTPHintPacketSampleData: public RTPHintPacketData {

		protected:
			MP4UInteger8BitsProperty_t				_mui8pType;
			MP4UInteger8BitsProperty_t				_mui8pTrackReferenceIndex;
			MP4UInteger16BitsProperty_t				_mui16pSampleLength;
			MP4UInteger32BitsProperty_t				_mui32pSampleIdentifier;
			MP4UInteger32BitsProperty_t				_mui32pSampleOffset;
			MP4UInteger16BitsProperty_t				_mui16pBytesPerBlock;
			MP4UInteger16BitsProperty_t				_mui16pSamplesPerBlock;

			unsigned char							*_pucEmbeddedData;

			RTPHintPacketSampleData (const RTPHintPacketSampleData &);

			RTPHintPacketSampleData &operator = (
				const RTPHintPacketSampleData &);

			virtual Error createProperties (unsigned long *pulPropertiesSize);

			virtual Error getPropertiesSize (
				unsigned long *pulPropertiesSize);

		public:
			RTPHintPacketSampleData ();

			virtual ~RTPHintPacketSampleData ();

			Error init (Tracer_p ptTracer);

			Error init (
				const unsigned char *pucRTPHintPacketDataBuffer,
				Tracer_p ptTracer);

			virtual Error finish (void);

			Error getTrackReferenceIndex (
				unsigned long *pulTrackReferenceIndex);

			Error getSampleLength (unsigned long *pulSampleLength);

			Error getSampleIdentifier (
				unsigned long *pulSampleIdentifier);

			Error getSampleOffset (
				unsigned long *pulSampleOffset);

			Error getBytesPerBlock (
				unsigned long *pulBytesPerBlock);

			Error getSamplesPerBlock (
				unsigned long *pulSamplesPerBlock);

			Error setReferenceSample (
				unsigned long ulMediaSampleIdentifier,
				unsigned long ulDataOffset, unsigned long ulDataLength);

			Error setEmbeddedReferenceSample (
				unsigned long ulHintSampleIdentifier,
				unsigned char *pucEmbeddedData,
				unsigned long ulEmbeddedDataLength,
				unsigned long ulDataOffset);

			virtual Error getPacketHintData (unsigned char *pucRTPHintSample,
				unsigned long *pulCurrentRTPHintSampleOffset,
				unsigned long *pulCurrentRTPHintSampleOffsetForEmbeddedData,
				unsigned long ulRTPHintSampleSize);

			virtual Error appendPayloadDataToRTPPacket (
				RTPPacket_p prpRTPPacket,
				std:: vector<MP4TrackInfo_p> *pvReferencesTracksInfo,
				MP4RootAtom_p pmaRootAtom, MP4TrackInfo_p pmtiTrakInfo,
				RTPStreamRealTimeInfo_p prsrtRTPMediaStreamRealTimeInfo,
				RTPStreamRealTimeInfo_p prsrtRTPHintStreamRealTimeInfo);

			static Error appendPayloadDataToRTPPacket (
				RTPPacket_p prpRTPPacket,
				MP4RootAtom_p pmaRootAtom,
				MP4TrackInfo_p pmtiMediaTrakInfo,
				unsigned long ulTrackReferenceIndex,
				unsigned long ulDataLength,
				unsigned long ulMediaSampleIdentifier,
				unsigned long ulDataOffset,
				unsigned long ulBytesPerCompressionBlock,
				unsigned long ulSamplePerCompressionBlock,
				RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
				Tracer_p ptTracer);

	} RTPHintPacketSampleData_t, *RTPHintPacketSampleData_p;

#endif
