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


#ifndef RTPHintSample_h
	#define RTPHintSample_h

	#include "RTPHintPacket.h"
	#include "MP4MdatAtom.h"
	#include "MP4NumpAtom.h"
	#include "MP4RootAtom.h"
	#include "MP4TrakAtom.h"
	#include "MP4UInteger16BitsProperty.h"
	#include "MP4BytesProperty.h"
	#include "MP4TrackInfo.h"
	#include "Tracer.h"
	#include "MP4Property.h"
	#include "RTPErrors.h"
	#include "RTPStreamRealTimeInfo.h"
	#include <vector>


	typedef class RTPHintSample {

		public:
			typedef enum RTPHintSampleStatus {
				RHS_BUILDED,
				RHS_INITIALIZED,
			} RTPHintSampleStatus_t, *RTPHintSampleStatus_p;


		protected:
			MP4UInteger16BitsProperty_t		_mui16pPacketsNumber;
			MP4BytesProperty_t				_mbpReserved;

			MP4TrackInfo_p					_pmtiMP4HintTrackInfo;
			unsigned long					_ulHintSampleIdentifier;
			MP4NumpAtom_p					_pmaHintNumpAtom;
			Tracer_p						_ptTracer;

			std:: vector<RTPHintPacket_p>	_vRTPHintPackets;
			RTPHintSampleStatus_t			_stRTPHintSampleStatus;


			RTPHintSample (const RTPHintSample &);

			RTPHintSample &operator = (const RTPHintSample &);

			Error createProperties (unsigned long *pulPropertiesSize);

			Error getProperties (const unsigned char *pucRTPHintSampleBuffer,
				unsigned long *pulPropertiesSize);

			Error getPropertiesSize (unsigned long *pulPropertiesSize);

			Error getHintPacket (const unsigned char *pucRTPHintPacketBuffer,
				unsigned long *pulRTPHintPacketSize);

		public:
			RTPHintSample ();

			virtual ~RTPHintSample ();

			Error init (MP4TrackInfo_p pmtiMP4HintTrackInfo,
				unsigned long ulHintSampleIdentifier,
				Tracer_p ptTracer);

			Error init (MP4RootAtom_p pmaRootAtom,
				MP4TrackInfo_p pmtiMP4HintTrackInfo,
				unsigned char *pucRTPHintSampleBuffer,
				unsigned long ulMaxHintSampleSize,
				unsigned long ulHintSampleIdentifier,
				RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
				Tracer_p ptTracer);

			Error finish (void);

			Error getState (RTPHintSampleStatus_p psRTPHintSampleState);

			Error createRTPHintPacket (RTPHintPacket_p *prpRTPHintPacket);

			Error getBytesNumberForMdat (unsigned long *pulBytesNumberForMdat,
				unsigned long *pulBytesNumberForEmbeddedData);

			Error incrementPacketsNumber (long lIncrement);

			Error getHintSampleIdentifier (
				unsigned long *pulHintSampleIdentifier);

			Error appendToMdatAtom (MP4MdatAtom_p pmaMdatAtom,
				unsigned long *pulBytesNumber);

			Error getPacketsNumber (unsigned long *pulPacketsNumber);

			Error getHintPacket (unsigned long ulPacketIndex,
				RTPHintPacket_p *prhpHintPacket);

	} RTPHintSample_t, *RTPHintSample_p;

#endif

