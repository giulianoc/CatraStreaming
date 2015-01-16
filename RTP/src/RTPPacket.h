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


#ifndef RTPPacket_h
	#define RTPPacket_h

	#include "Tracer.h"
	#include "RTPErrors.h"

	#define RTP_RTPPACKETHEADERLENGTH			12


	typedef class RTPPacket
	{
		private:
			Tracer_p				_ptTracer;
			unsigned long			_ulMaxRTPPacketSize;
			unsigned long			_ulSequenceNumberRandomOffset;
			unsigned long			_ulTimestampRandomOffset;

			Boolean_t				_bIsSequenceNumberInitialized;
			Boolean_t				_bIsTimeStampInitialized;

			unsigned char			*_pucRTPPacket;
			#ifdef WIN32
				__int64					_ullCurrentRTPPacketLength;
			#else
				unsigned long long		_ullCurrentRTPPacketLength;
			#endif

		public:
			RTPPacket (void);

			~RTPPacket (void);

			Error init (unsigned long ulMaxRTPPayloadSize,
				Tracer_p ptTracer,
				unsigned long ulSequenceNumberRandomOffset = 0,
				unsigned long ulTimestampRandomOffset = 0);

			Error finish ();

			Error setRTPPacket (unsigned char *pucRTPPacket,
				unsigned long ulRTPPacketLength);

			Error setRandomOffsets (
				unsigned long ulSequenceNumberRandomOffset,
				unsigned long ulTimestampRandomOffset);

			Error reset ();

			operator unsigned char * (void) const;

			#ifdef WIN32
				operator __int64 (void) const;
			#else
				operator unsigned long long (void) const;
			#endif

			Error setVersion (unsigned char ucVersion);

			Error getMarkerBit (Boolean_p pbMarkerBit);

			Error setMarkerBit (Boolean_t bMarkerBit);

			Error getPayloadType (unsigned char *pucPayloadType);

			Error setPayloadType (unsigned char ucPayloadType);

			Error getSequenceNumber (unsigned short *pusSequenceNumber,
				unsigned long *pulSequenceNumberRandomOffsetUsed);

			Error setSequenceNumber (unsigned short usSequenceNumber);

			Error getTimeStamp (unsigned long *pulTimeStamp,
				unsigned long *pulTimestampRandomOffsetUsed);

			Error setTimeStamp (unsigned long ulTimeStamp);

			Error setServerSSRC (unsigned long ulServerSSRC);

			#ifdef WIN32
				Error appendData (unsigned char *pucData,
					const __int64 &ullDataLength);
			#else
				Error appendData (unsigned char *pucData,
					const unsigned long long &ullDataLength);
			#endif

			#ifdef WIN32
				Error insertData (const __int64 &ullDataIndex,
					unsigned char *pucData,
					const __int64 &ullDataLength);
			#else
				Error insertData (const unsigned long long &ullDataIndex,
					unsigned char *pucData,
					const unsigned long long &ullDataLength);
			#endif

			#ifdef WIN32
				Error deleteData (const __int64 &ullDataIndex,
					const __int64 &ullDataLengthToBeDeleted);
			#else
				Error deleteData (const unsigned long long &ullDataIndex,
					const unsigned long long &ullDataLengthToBeDeleted);
			#endif

			#ifdef WIN32
				Error modifyData (const __int64 &ullDataIndex,
					unsigned char *pucData, const __int64 &ullDataLength);
			#else
				Error modifyData (const unsigned long long &ullDataIndex,
					unsigned char *pucData, const unsigned long long &ullDataLength);
			#endif

	} RTPPacket_t, *RTPPacket_p;

#endif

