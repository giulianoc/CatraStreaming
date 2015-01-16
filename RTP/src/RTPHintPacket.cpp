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


#include "RTPHintPacket.h"
#include "RTPHintPacketImmediateData.h"
#include "RTPHintPacketSampleData.h"
#include "RTPMessages.h"
#include <sys/types.h>
#ifdef WIN32
	// #include <winsock2.h>
#else
	#include <unistd.h>
	#include <netinet/in.h>
#endif
#include <assert.h>
#include <stdio.h>



RTPHintPacket:: RTPHintPacket (void)

{

}


RTPHintPacket:: ~RTPHintPacket (void)

{

}



RTPHintPacket:: RTPHintPacket (const RTPHintPacket &)

{

	assert (1==0);

	// to do

}


RTPHintPacket &RTPHintPacket:: operator = (const RTPHintPacket &)

{

	assert (1==0);

	// to do

	return *this;

}


Error RTPHintPacket:: init (Tracer_p ptTracer)

{

	unsigned long					ulPropertiesSize;


	if (ptTracer == (Tracer_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);

		return err;
	}

	_ptTracer						= ptTracer;

	if (createProperties (&ulPropertiesSize) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_CREATEPROPERTIES_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// ulPropertiesSize will be 12 because:
	// 	4 for Relative packet transmission
	// 	2 for RTP header info
	// 	2 for RTP sequence number
	// 	2 for Flags
	// 	2 for entry count
	// 	TVL information now are implicit. If this information will be
	// 	activated with a specific method (today not known) the next
	// 	2 variables will be updated
	_ulBytesNumberForMdat							= ulPropertiesSize;
	_ulPayloadAndPacketPropertiesBytesNumber		= ulPropertiesSize;

	_ulBytesNumberForEmbeddedData					= 0;

	_ulImmediateDataBytesNumber						= 0;
	_ulMediaDataBytesNumber							= 0;
	_ulPayloadBytesNumber							= 0;


	return errNoError;
}


Error RTPHintPacket:: init (const unsigned char *pucRTPHintPacketBuffer,
	unsigned long *pulRTPHintPacketSize, Tracer_p ptTracer)

{

	unsigned long					ulPropertiesSize;
	unsigned long					ulRTPHintPacketDataNumber;
	unsigned long					ulRTPHintPacketDataIndex;
	const unsigned char				*pucRTPHintPacketDataBuffer;
	unsigned long					ulRTPHintPacketDataSize;


	if (ptTracer == (Tracer_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);

		return err;
	}

	_ptTracer						= ptTracer;

	if (getProperties (pucRTPHintPacketBuffer, &ulPropertiesSize) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_GETPROPERTIES_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// ulPropertiesSize will be almost 12 because:
	// 	4 for Relative packet transmission
	// 	2 for RTP header info
	// 	2 for RTP sequence number
	// 	2 for Flags
	// 	2 for entry count
	// 	TVL information.
	_ulBytesNumberForMdat							= ulPropertiesSize;
	_ulPayloadAndPacketPropertiesBytesNumber		= ulPropertiesSize;

	_ulBytesNumberForEmbeddedData					= 0;

	_ulImmediateDataBytesNumber						= 0;
	_ulMediaDataBytesNumber							= 0;
	_ulPayloadBytesNumber							= 0;

	if (getEntriesCount (&ulRTPHintPacketDataNumber) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_GETENTRIESCOUNT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*pulRTPHintPacketSize			= ulPropertiesSize;

    pucRTPHintPacketDataBuffer		= pucRTPHintPacketBuffer + ulPropertiesSize;

	for (ulRTPHintPacketDataIndex = 0;
		ulRTPHintPacketDataIndex < ulRTPHintPacketDataNumber;
		ulRTPHintPacketDataIndex++)
	{
		if (getRTPHintPacketData (pucRTPHintPacketDataBuffer,
			&ulRTPHintPacketDataSize) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKET_GETRTPHINTPACKETDATA_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		pucRTPHintPacketDataBuffer				+= ulRTPHintPacketDataSize;

		*pulRTPHintPacketSize					+= ulRTPHintPacketDataSize;
	}


	return errNoError;
}


Error RTPHintPacket:: finish (void)

{

	if (_vRTPHintPacketData. size () > 0)
	{
		std:: vector<RTPHintPacketData_p>:: const_iterator  it;
		RTPHintPacketData_p									pRTPHintPacketData;


		for (it = _vRTPHintPacketData. begin ();
			it != _vRTPHintPacketData. end (); ++it)
		{
			pRTPHintPacketData				= *it;

			if (pRTPHintPacketData -> finish () != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTPACKETDATA_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			delete pRTPHintPacketData;
			pRTPHintPacketData				= (RTPHintPacketData_p) NULL;
		}

		_vRTPHintPacketData. clear ();
	}


	return errNoError;
}


Error RTPHintPacket:: createProperties (unsigned long *pulPropertiesSize)

{

	unsigned long				ulValue;
	Boolean_t					bIsImplicitProperty;


	ulValue							= 0;
	bIsImplicitProperty				= false;

	// 0: RelativePacketTransmissionTime
	if (_mui32pRelativePacketTransmissionTime. init (
		"RelativePacketTransmissionTime", 1, &bIsImplicitProperty, &ulValue,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	// 1: RTPHeaderInfo
	if (_mui16pRTPHeaderInfo. init ("RTPHeaderInfo", 1, &bIsImplicitProperty,
		&ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 2: RTPSequenceNumber
	if (_mui16pRTPSequenceNumber. init ("RTPSequenceNumber", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 3: Flags
	if (_mui16pFlags. init ("Flags", 1, &bIsImplicitProperty, &ulValue,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 4: EntriesCount
	if (_mui16pEntriesCount. init ("EntriesCount", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	bIsImplicitProperty				= true;

	// 5: ExtraInformationSize
	ulValue							= 16;
	if (_mui32pExtraInformationSize. init ("ExtraInformationSize",
		1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 6: TVLSize
	ulValue							= 12;
	if (_mui32pTVLSize. init ("TVLSize", 1, &bIsImplicitProperty, &ulValue,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	ulValue							= 0;

	// 7: TVLType
	/*
	{
		char								**pTVLType;


		if ((pTVLType = new char * [1]) == (char **) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if ((pTVLType [0] = new char [4]) == (char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pTVLType;
			pTVLType					= (char **) NULL;

			return err;
		}

		memcpy (pTVLType [0], "rtpo", 4);
		if (_mstrfpTVLType. init ("TVLType", 1, &bIsImplicitProperty,
			4, pTVLType, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pTVLType [0];
			pTVLType [0]			= (char *) NULL;

			delete [] pTVLType;
			pTVLType				= (char **) NULL;

			return err;
		}

		delete [] pTVLType [0];
		pTVLType [0]			= (char *) NULL;

		delete [] pTVLType;
		pTVLType			= (char **) NULL;
	}
	*/
	{
		char							*pTVLType [1]	=
			{ "rtpo" };


		if (_mstrfpTVLType. init ("TVLType", 1, &bIsImplicitProperty,
			4, pTVLType, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	// 8: TVLTimestampOffset
	if (_mui32pTLVTimestampOffset. init ("TVLTimestampOffset",
		1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (getPropertiesSize (pulPropertiesSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETPROPERTIESSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacket:: getProperties (
	const unsigned char *pucRTPHintSampleBuffer,
	unsigned long *pulPropertiesSize)

{

	unsigned long               ulSize;
	Boolean_t					bIsImplicitProperty;
	unsigned long				ulValue;
	Boolean_t					bExtraInformationTVL;


	*pulPropertiesSize                  = 0;

	// 0: RelativePacketTransmissionTime
	if (_mui32pRelativePacketTransmissionTime. init (
		"RelativePacketTransmissionTime", 1,
		pucRTPHintSampleBuffer + *pulPropertiesSize,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui32pRelativePacketTransmissionTime. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	// 1: RTPHeaderInfo
	if (_mui16pRTPHeaderInfo. init ("RTPHeaderInfo", 1,
		pucRTPHintSampleBuffer + *pulPropertiesSize, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui16pRTPHeaderInfo. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	// 2: RTPSequenceNumber
	if (_mui16pRTPSequenceNumber. init ("RTPSequenceNumber", 1,
		pucRTPHintSampleBuffer + *pulPropertiesSize, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui16pRTPSequenceNumber. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	// 3: Flags
	if (_mui16pFlags. init ("Flags", 1,
		pucRTPHintSampleBuffer + *pulPropertiesSize,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui16pFlags. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	// 4: EntriesCount
	if (_mui16pEntriesCount. init ("EntriesCount", 1,
		pucRTPHintSampleBuffer + *pulPropertiesSize, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui16pEntriesCount. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (getExtraInformationTVL (&bExtraInformationTVL) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_GETEXTRAINFORMATIONTVL_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bExtraInformationTVL)
	{
		// see the QT manual
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_NOTIMPLEMENTEDYET);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		/* QT implementation: (_pCurrentSamplePacket point to the begin of the packet, means to _mui32pRelativePacketTransmissionTime)

		// Extra Information TLV is present
		char			*TLVParser;
		char			*TLVEnd;


		ulTLVSize				=
			*((unsigned long *)
			(phtiHintTrackInfo -> _pCurrentSamplePacket + 12));
		ulTLVSize				= ntohl (ulTLVSize);

		TLVParser		= phtiHintTrackInfo -> _pCurrentSamplePacket + 16;
		TLVEnd			= TLVParser + ulTLVSize;

		while (TLVParser < TLVEnd)
		{
			if (ntohl (*((unsigned long *) (TLVParser + 4))) ==
				'r' << 24 | 't' << 16 | 'p' << 8 | 'o')
			{
				// This is the RTP timestamp TLV
				lTLVTimestampOffset			= *((long *)(TLVParser + 8));
			}

			// Skip onto the next TLV entry
			TLVParser		+= ntohl (*(unsigned long *) TLVParser);
		}
		*/

		return err;
	}
	else
	{
		bIsImplicitProperty				= true;

		// 5: ExtraInformationSize
		ulValue							= 16;
		if (_mui32pExtraInformationSize. init ("ExtraInformationSize",
			1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 6: TVLSize
		ulValue							= 12;
		if (_mui32pTVLSize. init ("TVLSize", 1, &bIsImplicitProperty, &ulValue,
			_ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		ulValue							= 0;

		// 7: TVLType
		/*
		{
			char								**pTVLType;


			if ((pTVLType = new char * [1]) == (char **) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if ((pTVLType [0] = new char [4]) == (char *) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				delete [] pTVLType;
				pTVLType					= (char **) NULL;

				return err;
			}

			memcpy (pTVLType [0], "rtpo", 4);
			if (_mstrfpTVLType. init ("TVLType", 1, &bIsImplicitProperty,
				4, pTVLType, _ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4BYTESPROPERTY_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				delete [] pTVLType [0];
				pTVLType [0]			= (char *) NULL;

				delete [] pTVLType;
				pTVLType				= (char **) NULL;

				return err;
			}

			delete [] pTVLType [0];
			pTVLType [0]			= (char *) NULL;

			delete [] pTVLType;
			pTVLType			= (char **) NULL;
		}
		*/
		{
			char							*pTVLType [1]	=
				{ "rtpo" };


			if (_mstrfpTVLType. init ("TVLType", 1, &bIsImplicitProperty,
				4, pTVLType, _ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4BYTESPROPERTY_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
		}

		// 8: TVLTimestampOffset
		if (_mui32pTLVTimestampOffset. init ("TVLTimestampOffset",
			1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	if (getPropertiesSize (pulPropertiesSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETPROPERTIESSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacket:: getRTPHintPacketData (
	const unsigned char *pucRTPHintPacketDataBuffer,
	unsigned long *pulRTPHintPacketDataSize)

{

	if (*pucRTPHintPacketDataBuffer == 0x00)
	{
		// No-Op Data Mode
	}
	else if (*pucRTPHintPacketDataBuffer == 0x01)
	{
		// Immediate Data Mode
		if (getRTPImmediateData (pucRTPHintPacketDataBuffer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKET_GETRTPIMMEDIATEDATA_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (*pucRTPHintPacketDataBuffer == 0x02)
	{
		// Sample Mode
		if (getRTPReferenceSample (pucRTPHintPacketDataBuffer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKET_GETRTPREFERENCEDATA_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (*pucRTPHintPacketDataBuffer == 0x03)
	{
		// Sample Description Data Mode
		// I don't think we have this case
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_NOTIMPLEMENTEDYET);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return errNoError;
	}
	else
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_DATASOURCEWRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return errNoError;
	}

	*pulRTPHintPacketDataSize				= 16;


	return errNoError;
}


Error RTPHintPacket:: getPropertiesSize (unsigned long *pulPropertiesSize)

{

	unsigned long               ulSize;


	*pulPropertiesSize                  = 0;

	if (_mui32pRelativePacketTransmissionTime. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mui16pRTPHeaderInfo. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mui16pRTPSequenceNumber. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mui16pFlags. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mui16pEntriesCount. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mui32pExtraInformationSize. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mui32pTVLSize. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mstrfpTVLType. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4STRINGFIXEDPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mui32pTLVTimestampOffset. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;


	return errNoError;
}


Error RTPHintPacket:: getBFrame (Boolean_p pbBFrame)

{

	unsigned long							ulFlags;


	if (_mui16pFlags. getValue (&ulFlags, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (ulFlags & 0x0002)
		*pbBFrame				= true;
	else
		*pbBFrame				= false;


	return errNoError;
}


Error RTPHintPacket:: setBFrame (Boolean_t bBFrame)

{

	unsigned long							ulFlags;
	unsigned long							ulSizeChangedInBytes;


	if (_mui16pFlags. getValue (&ulFlags, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bBFrame)
		ulFlags					|= 0x0002;
	else
		ulFlags					&= 0xFFFD;

	if (_mui16pFlags. setValue (ulFlags, 0, &ulSizeChangedInBytes) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacket:: getRelativePacketTransmissionTime (
	long *plRelativePacketTransmissionTime)

{

	unsigned long					ulRelativePacketTransmissionTime;


	if (_mui32pRelativePacketTransmissionTime. getValue (
		&ulRelativePacketTransmissionTime, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*plRelativePacketTransmissionTime			=
		(long) ulRelativePacketTransmissionTime;


	return errNoError;
}


Error RTPHintPacket:: getTLVTimestampOffset (long *plTLVTimestampOffset)

{

	unsigned long					ulTLVTimestampOffset;


	if (_mui32pTLVTimestampOffset. getValue (&ulTLVTimestampOffset, 0) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*plTLVTimestampOffset			= (long) ulTLVTimestampOffset;


	return errNoError;
}


Error RTPHintPacket:: setTLVTimestampOffset (unsigned long ulTLVTimestampOffset)

{

	unsigned long							ulFlags;
	unsigned long							ulSizeChangedInBytes;


	if (_mui16pFlags. getValue (&ulFlags, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (ulTLVTimestampOffset != 0)
	{
		ulFlags								|= 0x0004;
	}
	else
	{
		// B is 1011 in binary
		ulFlags								&= 0xFFFB;
	}

	if (_mui16pFlags. setValue (ulFlags, 0, &ulSizeChangedInBytes) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui32pTLVTimestampOffset. setValue (ulTLVTimestampOffset, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacket:: getMarkerBit (Boolean_p pbMarkerBit)

{

	unsigned long							ulRTPHeaderInfo;


	if (_mui16pRTPHeaderInfo. getValue (&ulRTPHeaderInfo, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (ulRTPHeaderInfo & 0x0080)
		*pbMarkerBit					= true;
	else
		*pbMarkerBit					= false;


	return errNoError;
}


Error RTPHintPacket:: setMarkerBit (Boolean_t bMarkerBit)

{

	unsigned long							ulRTPHeaderInfo;
	unsigned long							ulSizeChangedInBytes;


	if (_mui16pRTPHeaderInfo. getValue (&ulRTPHeaderInfo, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bMarkerBit)
		ulRTPHeaderInfo					|= 0x0080;
	else
		ulRTPHeaderInfo					&= 0xFF7F;

	if (_mui16pRTPHeaderInfo. setValue (ulRTPHeaderInfo, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacket:: getPayloadType (unsigned long *pulPayloadNumber)

{

	unsigned long							ulRTPHeaderInfo;


	if (_mui16pRTPHeaderInfo. getValue (&ulRTPHeaderInfo, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*pulPayloadNumber				= ulRTPHeaderInfo & 0x0000007F;


	return errNoError;
}


Error RTPHintPacket:: setPayloadType (unsigned long ulPayloadNumber)

{

	unsigned long							ulRTPHeaderInfo;
	unsigned short							usRTPHeaderInfo;
	unsigned char							ucPayloadNumber;
	unsigned long							ulSizeChangedInBytes;


	if (_mui16pRTPHeaderInfo. getValue (&ulRTPHeaderInfo, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	usRTPHeaderInfo					= (unsigned short) ulRTPHeaderInfo;
	ucPayloadNumber					= (unsigned char) ulPayloadNumber;

	usRTPHeaderInfo					&= 0xFF80;
	ucPayloadNumber					&= 0x7F;
	usRTPHeaderInfo					|= ucPayloadNumber;

	ulRTPHeaderInfo					= usRTPHeaderInfo;

	if (_mui16pRTPHeaderInfo. setValue (ulRTPHeaderInfo, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacket:: getSequenceNumber (unsigned long *pulSequenceNumber)

{

	if (_mui16pRTPSequenceNumber. getValue (pulSequenceNumber, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacket:: setSequenceNumber (unsigned long ulSequenceNumber)

{

	unsigned long							ulSizeChangedInBytes;


	if (_mui16pRTPSequenceNumber. setValue (ulSequenceNumber, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacket:: getEntriesCount (unsigned long *pulEntriesCount)

{

	if (_mui16pEntriesCount. getValue (pulEntriesCount, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacket:: incrementEntriesCount (long lIncrement)

{

	if (_mui16pEntriesCount. incrementValue (0, lIncrement) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacket:: getExtraInformationTVL (Boolean_p pbExtraInformationTVL)

{

	unsigned long						ulFlags;


	if (_mui16pFlags. getValue (&ulFlags, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (ulFlags & 0x4)
		*pbExtraInformationTVL					= true;
	else
		*pbExtraInformationTVL					= false;


	return errNoError;
}


Error RTPHintPacket:: createRTPImmediateData (unsigned char *pucImmediateData,
	unsigned long ulBytesNumber)

{

	RTPHintPacketImmediateData_p			pridRTPImmediateData;


	if ((pridRTPImmediateData = new RTPHintPacketImmediateData_t) ==
		(RTPHintPacketImmediateData_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (pridRTPImmediateData -> init (_ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETIMMEDIATEDATA_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete pridRTPImmediateData;
		pridRTPImmediateData			= (RTPHintPacketImmediateData_p) NULL;

		return err;
	}

	if (pridRTPImmediateData -> setData (pucImmediateData, ulBytesNumber) !=
		errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETIMMEDIATEDATA_SETDATA_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pridRTPImmediateData -> finish () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKETIMMEDIATEDATA_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete pridRTPImmediateData;
		pridRTPImmediateData			= (RTPHintPacketImmediateData_p) NULL;

		return err;
	}

	_vRTPHintPacketData. insert (_vRTPHintPacketData. end (),
		pridRTPImmediateData);

	if (incrementEntriesCount (1) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	_ulBytesNumberForMdat							+= 16;
	_ulPayloadAndPacketPropertiesBytesNumber		+= ulBytesNumber;
	_ulImmediateDataBytesNumber						+= ulBytesNumber;
	_ulPayloadBytesNumber							+= ulBytesNumber;


	return errNoError;
}


Error RTPHintPacket:: getRTPImmediateData (
	const unsigned char *pucRTPHintPacketDataBuffer)

{

	RTPHintPacketImmediateData_p			pridRTPImmediateData;


	if ((pridRTPImmediateData = new RTPHintPacketImmediateData_t) ==
		(RTPHintPacketImmediateData_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (pridRTPImmediateData -> init (pucRTPHintPacketDataBuffer, _ptTracer) !=
		errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETIMMEDIATEDATA_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete pridRTPImmediateData;
		pridRTPImmediateData			= (RTPHintPacketImmediateData_p) NULL;

		return err;
	}

	_vRTPHintPacketData. insert (_vRTPHintPacketData. end (),
		pridRTPImmediateData);

	_ulBytesNumberForMdat							+= 16;
	_ulPayloadAndPacketPropertiesBytesNumber		+=
		(*(pucRTPHintPacketDataBuffer + 1));
	_ulImmediateDataBytesNumber						+=
		(*(pucRTPHintPacketDataBuffer + 1));
	_ulPayloadBytesNumber							+=
		(*(pucRTPHintPacketDataBuffer + 1));


	return errNoError;
}


Error RTPHintPacket:: createRTPReferenceSample (
	unsigned long ulMediaSampleIdentifier,
	unsigned long ulDataOffset, unsigned long ulDataLength)

{

	RTPHintPacketSampleData_p				prsdRTPHintPacketSampleData;


	if ((prsdRTPHintPacketSampleData = new RTPHintPacketSampleData_t) ==
		(RTPHintPacketSampleData_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (prsdRTPHintPacketSampleData -> init (_ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETSAMPLEDATA_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete prsdRTPHintPacketSampleData;
		prsdRTPHintPacketSampleData			= (RTPHintPacketSampleData_p) NULL;

		return err;
	}

	if (prsdRTPHintPacketSampleData -> setReferenceSample (
		ulMediaSampleIdentifier, ulDataOffset, ulDataLength) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETSAMPLEDATA_SETREFERENCESAMPLE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prsdRTPHintPacketSampleData -> finish () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKETSAMPLEDATA_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete prsdRTPHintPacketSampleData;
		prsdRTPHintPacketSampleData			= (RTPHintPacketSampleData_p) NULL;

		return err;
	}

	_vRTPHintPacketData. insert (_vRTPHintPacketData. end (),
		prsdRTPHintPacketSampleData);

	if (incrementEntriesCount (1) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	_ulBytesNumberForMdat							+= 16;
	_ulPayloadAndPacketPropertiesBytesNumber		+= ulDataLength;
	_ulMediaDataBytesNumber							+= ulDataLength;
	_ulPayloadBytesNumber							+= ulDataLength;


	return errNoError;
}


Error RTPHintPacket:: createEmbeddedRTPReferenceSample (
	unsigned long ulHintSampleIdentifier,
	unsigned char *pucEmbeddedData, unsigned long ulEmbeddedDataLength)

{

	RTPHintPacketSampleData_p				prsdRTPHintPacketSampleData;


	if ((prsdRTPHintPacketSampleData = new RTPHintPacketSampleData_t) ==
		(RTPHintPacketSampleData_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (prsdRTPHintPacketSampleData -> init (_ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETSAMPLEDATA_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete prsdRTPHintPacketSampleData;
		prsdRTPHintPacketSampleData			= (RTPHintPacketSampleData_p) NULL;

		return err;
	}

	// ulDataOffset is 0 because I will know the real offset when
	// the hint sample will be written on the disk
	if (prsdRTPHintPacketSampleData -> setEmbeddedReferenceSample (
		ulHintSampleIdentifier, pucEmbeddedData, ulEmbeddedDataLength, 0) !=
		errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETSAMPLEDATA_SETEMBEDDEDREFERENCESAMPLE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (prsdRTPHintPacketSampleData -> finish () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKETSAMPLEDATA_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete prsdRTPHintPacketSampleData;
		prsdRTPHintPacketSampleData			= (RTPHintPacketSampleData_p) NULL;

		return err;
	}

	_vRTPHintPacketData. insert (_vRTPHintPacketData. end (),
		prsdRTPHintPacketSampleData);

	if (incrementEntriesCount (1) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	_ulBytesNumberForMdat							+= (16 +
		ulEmbeddedDataLength);
	_ulBytesNumberForEmbeddedData					+= ulEmbeddedDataLength;
	_ulPayloadAndPacketPropertiesBytesNumber		+= ulEmbeddedDataLength;
	_ulPayloadBytesNumber							+= ulEmbeddedDataLength;


	return errNoError;
}


Error RTPHintPacket:: getRTPReferenceSample (
	const unsigned char *pucRTPHintPacketDataBuffer)

{

	RTPHintPacketSampleData_p				prsdRTPHintPacketSampleData;


	if ((prsdRTPHintPacketSampleData = new RTPHintPacketSampleData_t) ==
		(RTPHintPacketSampleData_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (prsdRTPHintPacketSampleData -> init (pucRTPHintPacketDataBuffer,
		_ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETSAMPLEDATA_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete prsdRTPHintPacketSampleData;
		prsdRTPHintPacketSampleData			= (RTPHintPacketSampleData_p) NULL;

		return err;
	}

	_vRTPHintPacketData. insert (_vRTPHintPacketData. end (),
		prsdRTPHintPacketSampleData);

	_ulBytesNumberForMdat							+= 16;
	_ulPayloadAndPacketPropertiesBytesNumber		+= ntohs (
		*((unsigned short *) (pucRTPHintPacketDataBuffer + 2)));
	_ulMediaDataBytesNumber							+= ntohs (
		*((unsigned short *) (pucRTPHintPacketDataBuffer + 2)));
	_ulPayloadBytesNumber							+= ntohs (
		*((unsigned short *) (pucRTPHintPacketDataBuffer + 2)));


	return errNoError;
}


Error RTPHintPacket:: getBytesNumberForMdat (
	unsigned long *pulBytesNumberForMdat,
	unsigned long *pulBytesNumberForEmbeddedData)

{

	*pulBytesNumberForMdat				= _ulBytesNumberForMdat;
	*pulBytesNumberForEmbeddedData		= _ulBytesNumberForEmbeddedData;


	return errNoError;
}


Error RTPHintPacket:: getPayloadAndPacketPropertiesBytesNumber (
	unsigned long *pulPayloadAndPacketPropertiesBytesNumber)

{

	*pulPayloadAndPacketPropertiesBytesNumber				=
		_ulPayloadAndPacketPropertiesBytesNumber;


	return errNoError;
}


Error RTPHintPacket:: getImmediateDataBytesNumber (
	unsigned long *pulImmediateDataBytesNumber)

{

	*pulImmediateDataBytesNumber				= _ulImmediateDataBytesNumber;


	return errNoError;
}


Error RTPHintPacket:: getMediaDataBytesNumber (
	unsigned long *pulMediaDataBytesNumber)

{

	*pulMediaDataBytesNumber						= _ulMediaDataBytesNumber;


	return errNoError;
}


Error RTPHintPacket:: getPayloadBytesNumber (
	unsigned long *pulPayloadBytesNumber)

{

	*pulPayloadBytesNumber						= _ulPayloadBytesNumber;


	return errNoError;
}


Error RTPHintPacket:: getPacket (unsigned char *pucRTPHintSample,
	unsigned long *pulCurrentRTPHintSampleOffset,
	unsigned long *pulCurrentRTPHintSampleOffsetForEmbeddedData,
	unsigned long ulRTPHintSampleSize)

{

	Boolean_t						bExtraInformationTVL;


	if (_mui32pRelativePacketTransmissionTime. getValue (
		pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulCurrentRTPHintSampleOffset					+= 4;

	if (_mui16pRTPHeaderInfo. getValue (
		pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulCurrentRTPHintSampleOffset					+= 2;

	if (_mui16pRTPSequenceNumber. getValue (
		pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulCurrentRTPHintSampleOffset					+= 2;

	if (_mui16pFlags. getValue (
		pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulCurrentRTPHintSampleOffset					+= 2;

	if (_mui16pEntriesCount. getValue (
		pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulCurrentRTPHintSampleOffset					+= 2;

	if (getExtraInformationTVL (&bExtraInformationTVL) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_GETEXTRAINFORMATIONTVL_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bExtraInformationTVL)
	{
		if (_mui32pExtraInformationSize. getValue (
			pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulCurrentRTPHintSampleOffset					+= 4;

		if (_mui32pTVLSize. getValue (
			pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulCurrentRTPHintSampleOffset					+= 4;

		if (_mstrfpTVLType. getValue (
			pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulCurrentRTPHintSampleOffset					+= 4;

		if (_mui32pTLVTimestampOffset. getValue (
			pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulCurrentRTPHintSampleOffset					+= 4;
	}

	{
		Message msg = RTPMessages (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_PACKETDATANUMBER, 1,
			(long) (_vRTPHintPacketData. size ()));
		_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (_vRTPHintPacketData. size () > 0)
	{
		std:: vector<RTPHintPacketData_p>:: const_iterator  it;
		RTPHintPacketData_p									pRTPHintPacketData;
		// unsigned long									ulPacketDataBytesNumber;


		for (it = _vRTPHintPacketData. begin ();
			it != _vRTPHintPacketData. end (); ++it)
		{
			pRTPHintPacketData				= *it;

			if (pRTPHintPacketData -> getPacketHintData (pucRTPHintSample,
				pulCurrentRTPHintSampleOffset,
				pulCurrentRTPHintSampleOffsetForEmbeddedData,
				ulRTPHintSampleSize) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTPACKETDATA_GETPACKETHINTDATA_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			/*
			// if we want this message we should initialize
			// the ulPacketDataBytesNumber variable
			{
				Message msg = MP4FileMessages (__FILE__, __LINE__,
					MP4F_RTPHINTPACKET_PACKETDATABYTESNUMBER, 1,
					(long) (ulPacketDataBytesNumber));
				_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
					__FILE__, __LINE__);
			}
			*/
		}
	}


	return errNoError;
}


Error RTPHintPacket:: getElementsNumberInDataTable (
	unsigned long *pusElementsNumberInDataTable)

{

	*pusElementsNumberInDataTable				= _vRTPHintPacketData. size ();


	return errNoError;
}


Error RTPHintPacket:: getHintPacketData (unsigned long ulIndexInDataTable,
	RTPHintPacketData_p *prhpdRTPHintPacketData)

{

	*prhpdRTPHintPacketData			= _vRTPHintPacketData [ulIndexInDataTable];


	return errNoError;
}


