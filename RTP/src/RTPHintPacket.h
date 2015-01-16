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


#ifndef RTPHintPacket_h
	#define RTPHintPacket_h

	#include "RTPHintPacketData.h"
	#include "Tracer.h"
	#include "MP4UInteger32BitsProperty.h"
	#include "MP4UInteger16BitsProperty.h"
	#include "MP4StringFixedProperty.h"
	#include "RTPErrors.h"
	#include <vector>


	typedef class RTPHintPacket {

		protected:
			MP4UInteger32BitsProperty_t
				_mui32pRelativePacketTransmissionTime;
			MP4UInteger16BitsProperty_t			_mui16pRTPHeaderInfo;
			MP4UInteger16BitsProperty_t			_mui16pRTPSequenceNumber;
			MP4UInteger16BitsProperty_t			_mui16pFlags;
			MP4UInteger16BitsProperty_t			_mui16pEntriesCount;
			MP4UInteger32BitsProperty_t			_mui32pExtraInformationSize;
			MP4UInteger32BitsProperty_t			_mui32pTVLSize;
			MP4StringFixedProperty_t			_mstrfpTVLType;
			MP4UInteger32BitsProperty_t			_mui32pTLVTimestampOffset;

			Tracer_p							_ptTracer;

			unsigned long						_ulBytesNumberForEmbeddedData;
			unsigned long						_ulBytesNumberForMdat;

			// Payload + packet properties without packed data properties
			unsigned long			_ulPayloadAndPacketPropertiesBytesNumber;
			unsigned long						_ulImmediateDataBytesNumber;
			unsigned long						_ulMediaDataBytesNumber;
			unsigned long						_ulPayloadBytesNumber;
			std:: vector<RTPHintPacketData_p>	_vRTPHintPacketData;


			RTPHintPacket (const RTPHintPacket &);

			RTPHintPacket &operator = (const RTPHintPacket &);

			Error createProperties (unsigned long *pulPropertiesSize);

			Error getProperties (
				const unsigned char *pucRTPHintSampleBuffer,
				unsigned long *pulPropertiesSize);

			Error getRTPHintPacketData (
				const unsigned char *pucRTPHintPacketDataBuffer,
				unsigned long *pulRTPHintPacketDataSize);

			Error getRTPImmediateData (
				const unsigned char *pucRTPHintPacketDataBuffer);

			Error getRTPReferenceSample (
				const unsigned char *pucRTPHintPacketDataBuffer);

			Error getPropertiesSize (unsigned long *pulPropertiesSize);

		public:
			RTPHintPacket ();

			virtual ~RTPHintPacket ();

			Error init (Tracer_p ptTracer);

			Error init (const unsigned char *pucRTPHintPacketBuffer,
				unsigned long *pulRTPHintPacketSize, Tracer_p ptTracer);

			Error finish (void);

			Error getEntriesCount (unsigned long *pulEntriesCount);

			Error getExtraInformationTVL (Boolean_p pbExtraInformationTVL);

			Error getBFrame (Boolean_p pbBFrame);

			Error setBFrame (Boolean_t bBFrame);

			Error getRelativePacketTransmissionTime (
				long *plRelativePacketTransmissionTime);

			Error getTLVTimestampOffset (long *plTLVTimestampOffset);

			Error setTLVTimestampOffset (unsigned long ulTLVTimestampOffset);

			Error getMarkerBit (Boolean_p pbMarkerBit);

			Error setMarkerBit (Boolean_t bMarkerBit);

			Error getPayloadType (unsigned long *pulPayloadNumber);

			Error setPayloadType (unsigned long ulPayloadNumber);

			Error getSequenceNumber (unsigned long *pulSequenceNumber);

			Error setSequenceNumber (unsigned long ulSequenceNumber);

			Error incrementEntriesCount (long lIncrement);

			Error createRTPImmediateData (unsigned char *pucImmediateData,
				unsigned long ulBytesNumber);

			Error createRTPReferenceSample (
				unsigned long ulMediaSampleIdentifier,
				unsigned long ulDataOffset, unsigned long ulDataLength);

			Error createEmbeddedRTPReferenceSample (
				unsigned long ulHintSampleIdentifier,
				unsigned char *pucEmbeddedData,
				unsigned long ulEmbeddedDataLength);

			Error getBytesNumberForMdat (unsigned long *pulBytesNumberForMdat,
				unsigned long *pulBytesNumberForEmbeddedData);

			Error getPayloadAndPacketPropertiesBytesNumber (
				unsigned long *pulPayloadAndPacketPropertiesBytesNumber);

			Error getImmediateDataBytesNumber (
				unsigned long *pulImmediateDataBytesNumber);

			Error getMediaDataBytesNumber (
				unsigned long *pulImmediateDataBytesNumber);

			Error getPayloadBytesNumber (unsigned long *pulPayloadBytesNumber);

			Error getPacket (unsigned char *pucRTPHintSample,
				unsigned long *pulCurrentRTPHintSampleOffset,
				unsigned long *pulCurrentRTPHintSampleOffsetForEmbeddedData,
				unsigned long ulRTPHintSampleSize);

			Error getElementsNumberInDataTable (
				unsigned long *pusElementsNumberInDataTable);

			Error getHintPacketData (unsigned long ulIndexInDataTable,
				RTPHintPacketData_p *prhpdRTPHintPacketData);

	} RTPHintPacket_t, *RTPHintPacket_p;

#endif

