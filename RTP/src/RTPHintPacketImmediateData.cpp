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


#include "RTPHintPacketImmediateData.h"
#include <sys/types.h>
#ifdef WIN32
#else
	#include <unistd.h>
#endif
#include <assert.h>
#include <stdio.h>



RTPHintPacketImmediateData:: RTPHintPacketImmediateData (void)

{

}


RTPHintPacketImmediateData:: ~RTPHintPacketImmediateData (void)

{

}



RTPHintPacketImmediateData:: RTPHintPacketImmediateData (
	const RTPHintPacketImmediateData &)

{

	assert (1==0);

	// to do

}


RTPHintPacketImmediateData &RTPHintPacketImmediateData:: operator = (
	const RTPHintPacketImmediateData &)

{

	assert (1==0);

	// to do

	return *this;

}


Error RTPHintPacketImmediateData:: init (Tracer_p ptTracer)

{

	if (RTPHintPacketData:: init (ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETDATA_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacketImmediateData:: init (
	const unsigned char *pucRTPHintPacketDataBuffer, Tracer_p ptTracer)

{

	if (RTPHintPacketData:: init (ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETDATA_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (setData ((unsigned char *) (pucRTPHintPacketDataBuffer + 2),
		*(pucRTPHintPacketDataBuffer + 1)) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETIMMEDIATEDATA_SETDATA_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacketImmediateData:: finish (void)

{

	if (RTPHintPacketData:: finish () != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETDATA_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacketImmediateData:: createProperties (
	unsigned long *pulPropertiesSize)

{

	unsigned long				ulValue;
	Boolean_t					bIsImplicitProperty;


	ulValue							= 0;
	bIsImplicitProperty				= false;

	if (_mui8pType. init ("Type", 1, &bIsImplicitProperty,
		&ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui8pImmediateLength. init ("ImmediateLength", 1, &bIsImplicitProperty,
		&ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 2: ImmediateData
	/*
	{
		unsigned char				**pucImmediateData;


		if ((pucImmediateData = new unsigned char * [1]) ==
			(unsigned char **) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if ((pucImmediateData [0] = new unsigned char [14]) ==
			(unsigned char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucImmediateData;
			pucImmediateData			= (unsigned char **) NULL;

			return err;
		}

		memset (pucImmediateData [0], 0, 14);
		if (_mbpImmediateData. init ("ImmediateData", 1, &bIsImplicitProperty,
			14, pucImmediateData, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucImmediateData [0];
			pucImmediateData [0]            = (unsigned char *) NULL;

			delete [] pucImmediateData;
			pucImmediateData                = (unsigned char **) NULL;

			return err;
		}

		delete [] pucImmediateData [0];
		pucImmediateData [0]			= (unsigned char *) NULL;

		delete [] pucImmediateData;
		pucImmediateData				= (unsigned char **) NULL;
	}
	*/
	{
		unsigned char				*pucImmediateData [1]	=
			{ (unsigned char *) "\0\0\0\0\0\0\0\0\0\0\0\0\0\0" };


		if (_mbpImmediateData. init ("ImmediateData", 1, &bIsImplicitProperty,
			14, pucImmediateData, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
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


Error RTPHintPacketImmediateData:: getPropertiesSize (
	unsigned long *pulPropertiesSize)

{

	unsigned long               ulSize;


	*pulPropertiesSize                  = 0;

	if (_mui8pType. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mui8pImmediateLength. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mbpImmediateData. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;


	return errNoError;
}


Error RTPHintPacketImmediateData:: getImmediateLength (
	unsigned long *pulImmediateLength)

{

	if (_mui8pImmediateLength. getValue (pulImmediateLength, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacketImmediateData:: setData (unsigned char *pucImmediateData,
	unsigned long ulBytesNumber)

{

	unsigned long					ulType;
	unsigned long					ulSizeChangedInBytes;
	long							lSizeChangedInBytes;
	unsigned char					pucLocalImmediateData [14];


	if (ulBytesNumber > 14)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	ulType					= 1;

	if (_mui8pType. setValue (ulType, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui8pImmediateLength. setValue (ulBytesNumber, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	memcpy (pucLocalImmediateData, pucImmediateData, ulBytesNumber);
	memset (pucLocalImmediateData + ulBytesNumber, 0, 14 - ulBytesNumber);

	if (_mbpImmediateData. setData (pucLocalImmediateData, 14,
		0, &lSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacketImmediateData:: getPacketHintData (
	unsigned char *pucRTPHintSample,
	unsigned long *pulCurrentRTPHintSampleOffset,
	unsigned long *pulCurrentRTPHintSampleOffsetForEmbeddedData,
	unsigned long ulRTPHintSampleSize)

{

	if (_mui8pType. getValue (
		pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulCurrentRTPHintSampleOffset					+= 1;

	if (_mui8pImmediateLength. getValue (
		pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulCurrentRTPHintSampleOffset					+= 1;

	if (_mbpImmediateData. getData (0, 14, 0,
		pucRTPHintSample + *pulCurrentRTPHintSampleOffset) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulCurrentRTPHintSampleOffset					+= 14;


	return errNoError;
}


Error RTPHintPacketImmediateData:: appendPayloadDataToRTPPacket (
	RTPPacket_p prpRTPPacket,
	std:: vector<MP4TrackInfo_p> *pvReferencesTracksInfo,
	MP4RootAtom_p pmaRootAtom, MP4TrackInfo_p pmtiTrakInfo,
	RTPStreamRealTimeInfo_p prsrtRTPMediaStreamRealTimeInfo,
	RTPStreamRealTimeInfo_p prsrtRTPHintStreamRealTimeInfo)

{

	unsigned long					ulDataLength;
	#ifdef WIN32
		__int64						ullBytesNumber;
	#else
		unsigned long long			ullBytesNumber;
	#endif
	unsigned char					*pucBuffer;


	if (getImmediateLength (&ulDataLength) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETIMMEDIATEDATE_GETIMMEDIATELENGTH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mbpImmediateData. getPointerToData (0, 0, &pucBuffer,
		&ullBytesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_GETPOINTERTODATA_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (ullBytesNumber < ulDataLength)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETIMMEDIATEDATA_BYTESNOTCONSISTENT);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (prpRTPPacket -> appendData (pucBuffer, ulDataLength) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPPACKET_APPENDDATA_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}

