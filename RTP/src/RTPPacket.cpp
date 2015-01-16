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


#include "RTPPacket.h"
#include <assert.h>
#ifdef WIN32
	// #include <winsock2.h>
#else
	#include <netinet/in.h>
#endif


RTPPacket:: RTPPacket (void)

{

}


RTPPacket:: ~RTPPacket (void)

{

}


Error RTPPacket:: init (unsigned long ulMaxRTPPayloadSize,
	Tracer_p ptTracer,
	unsigned long ulSequenceNumberRandomOffset,
	unsigned long ulTimestampRandomOffset)

{

	if (ptTracer == (Tracer_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	_ulMaxRTPPacketSize						= ulMaxRTPPayloadSize +
		RTP_RTPPACKETHEADERLENGTH;
	_ulSequenceNumberRandomOffset			= ulSequenceNumberRandomOffset;
	_ulTimestampRandomOffset				= ulTimestampRandomOffset;
	_ptTracer								= ptTracer;

	_bIsSequenceNumberInitialized			= false;
	_bIsTimeStampInitialized				= false;

	if ((_pucRTPPacket = new unsigned char [_ulMaxRTPPacketSize]) ==
		(unsigned char *) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	memset (_pucRTPPacket, 0, _ulMaxRTPPacketSize);

	// RTP_RTPPACKETHEADERLENGTH for the Flags (2) + sequence number (2) +
	// timestamp (4) + SSRC (4)
	_ullCurrentRTPPacketLength				= RTP_RTPPACKETHEADERLENGTH;


	return errNoError;
}


Error RTPPacket:: finish (void)

{

	delete [] _pucRTPPacket;
	_pucRTPPacket		= (unsigned char *) NULL;


	return errNoError;
}


Error RTPPacket:: setRTPPacket (unsigned char *pucRTPPacket,
	unsigned long ulRTPPacketLength)

{

	if (ulRTPPacketLength < RTP_RTPPACKETHEADERLENGTH ||
		ulRTPPacketLength > _ulMaxRTPPacketSize)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	memcpy (_pucRTPPacket, pucRTPPacket,
		ulRTPPacketLength);

	_bIsSequenceNumberInitialized			= true;
	_bIsTimeStampInitialized				= true;

	_ullCurrentRTPPacketLength				= ulRTPPacketLength;


	return errNoError;
}


Error RTPPacket:: setRandomOffsets (
	unsigned long ulSequenceNumberRandomOffset,
	unsigned long ulTimestampRandomOffset)


{


	if (_bIsSequenceNumberInitialized)
	{
		unsigned short		usSequenceNumber;
		unsigned long		ulSequenceNumberRandomOffsetUsed;


		// get the sequence number with the current random offset
		if (getSequenceNumber (&usSequenceNumber,
			&ulSequenceNumberRandomOffsetUsed) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_GETSEQUENCENUMBER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		_ulSequenceNumberRandomOffset			= ulSequenceNumberRandomOffset;

		// set sequence number with the new random offset
		if (setSequenceNumber (usSequenceNumber) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_GETSEQUENCENUMBER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else
		_ulSequenceNumberRandomOffset			= ulSequenceNumberRandomOffset;

	if (_bIsTimeStampInitialized)
	{
		unsigned long		ulTimeStamp;
		unsigned long		ulTimestampRandomOffsetUsed;


		// get the timestamp with the current random offset
		if (getTimeStamp (&ulTimeStamp,
			&ulTimestampRandomOffsetUsed) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_GETTIMESTAMP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		_ulTimestampRandomOffset				= ulTimestampRandomOffset;

		// set the time stamp with the new random offset
		if (setTimeStamp (ulTimeStamp) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_SETTIMESTAMP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else
		_ulTimestampRandomOffset				= ulTimestampRandomOffset;


	return errNoError;
}


Error RTPPacket:: reset ()

{

	memset (_pucRTPPacket, 0, _ulMaxRTPPacketSize);

	_bIsSequenceNumberInitialized			= false;
	_bIsTimeStampInitialized				= false;

	// RTP_RTPPACKETHEADERLENGTH for the Flags (2) + sequence number (2) +
	// timestamp (4) + SSRC (4)
	_ullCurrentRTPPacketLength				= RTP_RTPPACKETHEADERLENGTH;


	return errNoError;
}


Error RTPPacket:: setVersion (unsigned char ucVersion)

{

	// initialize to 0 the first 2 bits of the RTP packet
	*((unsigned short *) _pucRTPPacket)			=
		*((unsigned short *) _pucRTPPacket) & htons (0x3FFF);

	switch (ucVersion)
	{
		case 0:
			
			break;
		case 1:
			*((unsigned short *) _pucRTPPacket)			=
				*((unsigned short *) _pucRTPPacket) | htons (0x4000);
			
			break;
		case 2:
			*((unsigned short *) _pucRTPPacket)			=
				*((unsigned short *) _pucRTPPacket) | htons (0x8000);
			
			break;
		case 3:
			*((unsigned short *) _pucRTPPacket)			=
				*((unsigned short *) _pucRTPPacket) | htons (0xC000);
			
			break;
		default:
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_NEW_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
	}


	return errNoError;
}


Error RTPPacket:: getMarkerBit (Boolean_p pbMarkerBit)

{

	*pbMarkerBit			=
		ntohs (*((unsigned short *) _pucRTPPacket)) & 0x0080 ? true : false;


	return errNoError;
}


Error RTPPacket:: setMarkerBit (Boolean_t bMarkerBit)

{

	// initialize to 0 the marker bit of the RTP packet
	*((unsigned short *) _pucRTPPacket)			=
		*((unsigned short *) _pucRTPPacket) & htons (0xFF7F);

	if (bMarkerBit)
		*((unsigned short *) _pucRTPPacket)			=
			*((unsigned short *) _pucRTPPacket) | htons (0x0080);
	else
		*((unsigned short *) _pucRTPPacket)			=
			*((unsigned short *) _pucRTPPacket) | htons (0x0000);


	return errNoError;
}


Error RTPPacket:: getPayloadType (unsigned char *pucPayloadType)

{

	*pucPayloadType								=
		ntohs (*((unsigned short *) _pucRTPPacket)) & 0x007F;


	return errNoError;
}


Error RTPPacket:: setPayloadType (unsigned char ucPayloadType)

{

	unsigned short				usLocalPayloadType;


	// initialize to 0 the payload type bits of the RTP packet
	*((unsigned short *) _pucRTPPacket)			=
		*((unsigned short *) _pucRTPPacket) & htons (0xFF80);

	usLocalPayloadType							= ucPayloadType & 0x7F;

	*((unsigned short *) _pucRTPPacket)			=
		*((unsigned short *) _pucRTPPacket) | htons (usLocalPayloadType);


	return errNoError;
}


Error RTPPacket:: getSequenceNumber (unsigned short *pusSequenceNumber,
	unsigned long *pulSequenceNumberRandomOffsetUsed)

{

	*pusSequenceNumber						=
		ntohs (*((unsigned short *) (_pucRTPPacket + 2)));
	*pusSequenceNumber						= (unsigned short)
		(*pusSequenceNumber - _ulSequenceNumberRandomOffset);
	*pulSequenceNumberRandomOffsetUsed		=
		_ulSequenceNumberRandomOffset;


	return errNoError;
}


Error RTPPacket:: setSequenceNumber (unsigned short usSequenceNumber)

{

	_bIsSequenceNumberInitialized			= true;

	*((unsigned short *) (_pucRTPPacket + 2))		=
		htons ((unsigned short) (usSequenceNumber +
		_ulSequenceNumberRandomOffset));


	return errNoError;
}


Error RTPPacket:: getTimeStamp (unsigned long *pulTimeStamp,
	unsigned long *pulTimestampRandomOffsetUsed)

{

	*pulTimeStamp							=
		ntohl (*((unsigned long *) (_pucRTPPacket + 4)));
	*pulTimeStamp							=
		*pulTimeStamp - _ulTimestampRandomOffset;
	*pulTimestampRandomOffsetUsed			= _ulTimestampRandomOffset;


	return errNoError;
}


Error RTPPacket:: setTimeStamp (unsigned long ulTimeStamp)

{

	_bIsTimeStampInitialized				= true;

	*((unsigned long *) (_pucRTPPacket + 4))		=
		htonl (ulTimeStamp + _ulTimestampRandomOffset);


	return errNoError;
}


Error RTPPacket:: setServerSSRC (unsigned long ulServerSSRC)

{

	*((unsigned long *) (_pucRTPPacket + 8))		= htonl (ulServerSSRC);


	return errNoError;
}


#ifdef WIN32
	Error RTPPacket:: appendData (unsigned char *pucData,
		const __int64 &ullDataLength)
#else
	Error RTPPacket:: appendData (unsigned char *pucData,
		const unsigned long long &ullDataLength)
#endif

{

	if (_ullCurrentRTPPacketLength + ullDataLength > _ulMaxRTPPacketSize)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_DATATOOBIGFORTHERTPPACKETSIZE,
			2, _ullCurrentRTPPacketLength + ullDataLength,
			_ulMaxRTPPacketSize);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	memcpy (_pucRTPPacket + _ullCurrentRTPPacketLength, pucData,
		(size_t) ullDataLength);

	_ullCurrentRTPPacketLength			+= ullDataLength;


	return errNoError;
}


#ifdef WIN32
	Error RTPPacket:: insertData (const __int64 &ullDataIndex,
		unsigned char *pucData,
		const __int64 &ullDataLength)
#else
	Error RTPPacket:: insertData (const unsigned long long &ullDataIndex,
		unsigned char *pucData,
		const unsigned long long &ullDataLength)
#endif

{

	if (ullDataIndex < RTP_RTPPACKETHEADERLENGTH)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_ullCurrentRTPPacketLength + ullDataLength > _ulMaxRTPPacketSize)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_DATATOOBIGFORTHERTPPACKETSIZE,
			2, _ullCurrentRTPPacketLength + ullDataLength,
			_ulMaxRTPPacketSize);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// memmove copy memory area and the memory areas may overlap
	memmove (_pucRTPPacket + ullDataIndex + ullDataLength,
		_pucRTPPacket + ullDataIndex, (size_t) (_ullCurrentRTPPacketLength - ullDataIndex));

	memcpy (_pucRTPPacket + ullDataIndex, pucData,
		(size_t) ullDataLength);

	_ullCurrentRTPPacketLength			+= ullDataLength;


	return errNoError;
}


#ifdef WIN32
	Error RTPPacket:: deleteData (const __int64 &ullDataIndex,
		const __int64 &ullDataLengthToBeDeleted)
#else
	Error RTPPacket:: deleteData (const unsigned long long &ullDataIndex,
		const unsigned long long &ullDataLengthToBeDeleted)
#endif

{

	if (ullDataIndex < RTP_RTPPACKETHEADERLENGTH ||
		ullDataIndex + ullDataLengthToBeDeleted > _ullCurrentRTPPacketLength)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// memmove copy memory area and the memory areas may overlap
	memmove (_pucRTPPacket + ullDataIndex,
		_pucRTPPacket + ullDataIndex + ullDataLengthToBeDeleted,
		(size_t) (_ullCurrentRTPPacketLength - ullDataLengthToBeDeleted));

	_ullCurrentRTPPacketLength			-= ullDataLengthToBeDeleted;


	return errNoError;
}


#ifdef WIN32
	Error RTPPacket:: modifyData (const __int64 &ullDataIndex,
		unsigned char *pucData, const __int64 &ullDataLength)
#else
	Error RTPPacket:: modifyData (const unsigned long long &ullDataIndex,
		unsigned char *pucData, const unsigned long long &ullDataLength)
#endif

{

	if (ullDataIndex + ullDataLength > _ullCurrentRTPPacketLength)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_DATATOOBIGFORTHERTPPACKETSIZE,
			2, _ullCurrentRTPPacketLength + ullDataLength,
			_ulMaxRTPPacketSize);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	memcpy (_pucRTPPacket + ullDataIndex, pucData,
		(size_t) ullDataLength);


	return errNoError;
}


RTPPacket:: operator unsigned char * (void) const

{

	return _pucRTPPacket;
}


#ifdef WIN32
	RTPPacket:: operator __int64 (void) const
#else
	RTPPacket:: operator unsigned long long (void) const
#endif

{


	return _ullCurrentRTPPacketLength;
}

