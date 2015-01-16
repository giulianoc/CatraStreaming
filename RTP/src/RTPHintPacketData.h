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


#ifndef RTPHintPacketData_h
	#define RTPHintPacketData_h

	#include "Tracer.h"
	#include "MP4Property.h"
	#include "RTPErrors.h"
	#include "MP4TrackInfo.h"
	#include "MP4RootAtom.h"
	#include "RTPPacket.h"
	#include "RTPStreamRealTimeInfo.h"


	typedef class RTPHintPacketData {

		protected:
			Tracer_p						_ptTracer;


			RTPHintPacketData (const RTPHintPacketData &);

			RTPHintPacketData &operator = (const RTPHintPacketData &);

			virtual Error createProperties (
				unsigned long *pulPropertiesSize) = 0;

			virtual Error getPropertiesSize (
				unsigned long *pulPropertiesSize) = 0;

		public:
			RTPHintPacketData ();

			virtual ~RTPHintPacketData ();

			Error init (Tracer_p ptTracer);

			virtual Error finish (void);

			virtual Error getPacketHintData (
				unsigned char *pucRTPHintSample,
				unsigned long *pulCurrentRTPHintSampleOffset,
				unsigned long *pulCurrentRTPHintSampleOffsetForEmbeddedData,
				unsigned long ulRTPHintSampleSize) = 0;

			virtual Error appendPayloadDataToRTPPacket (
				RTPPacket_p prpRTPPacket,
				std:: vector<MP4TrackInfo_p> *pvReferencesTracksInfo,
				MP4RootAtom_p pmaRootAtom, MP4TrackInfo_p pmtiTrakInfo,
				RTPStreamRealTimeInfo_p prsrtRTPMediaStreamRealTimeInfo,
				RTPStreamRealTimeInfo_p prsrtRTPHintStreamRealTimeInfo) = 0;

	} RTPHintPacketData_t, *RTPHintPacketData_p;

#endif
