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


#ifndef RTPHintPacketImmediateData_h
	#define RTPHintPacketImmediateData_h

	#include "RTPHintPacketData.h"
	#include "Tracer.h"
	#include "MP4UInteger8BitsProperty.h"
	#include "MP4BytesProperty.h"
	#include "RTPErrors.h"


	typedef class RTPHintPacketImmediateData: public RTPHintPacketData {

		protected:
			MP4UInteger8BitsProperty_t				_mui8pType;
			MP4UInteger8BitsProperty_t				_mui8pImmediateLength;
			MP4BytesProperty_t						_mbpImmediateData;


			RTPHintPacketImmediateData (const RTPHintPacketImmediateData &);

			RTPHintPacketImmediateData &operator = (
				const RTPHintPacketImmediateData &);

			Error createProperties (unsigned long *pulPropertiesSize);

			virtual Error getPropertiesSize (
				unsigned long *pulPropertiesSize);

		public:
			RTPHintPacketImmediateData ();

			virtual ~RTPHintPacketImmediateData ();

			Error init (Tracer_p ptTracer);

			Error init (
				const unsigned char *pucRTPHintPacketDataBuffer,
				Tracer_p ptTracer);

			virtual Error finish (void);

			Error getImmediateLength (
				unsigned long *pulImmediateLength);

			Error setData (unsigned char *pucImmediateData,
				unsigned long ulBytesNumber);

			virtual Error getPacketHintData (
				unsigned char *pucRTPHintSample,
				unsigned long *pulCurrentRTPHintSampleOffset,
				unsigned long *pulCurrentRTPHintSampleOffsetForEmbeddedData,
				unsigned long ulRTPHintSampleSize);

			virtual Error appendPayloadDataToRTPPacket (
				RTPPacket_p prpRTPPacket,
				std:: vector<MP4TrackInfo_p> *pvReferencesTracksInfo,
				MP4RootAtom_p pmaRootAtom, MP4TrackInfo_p pmtiTrakInfo,
				RTPStreamRealTimeInfo_p prsrtRTPMediaStreamRealTimeInfo,
				RTPStreamRealTimeInfo_p prsrtRTPHintStreamRealTimeInfo);

	} RTPHintPacketImmediateData_t, *RTPHintPacketImmediateData_p;

#endif

