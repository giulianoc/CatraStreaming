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


#include "MP4Utility.h"
#include "RTPHintPacketSampleData.h"
#include "RTPUtility.h"
#include <sys/types.h>
#ifdef WIN32
	// #include <Winsock2.h>
#else
	#include <unistd.h>
	#include <netinet/in.h>
#endif
#include <assert.h>
#include <stdio.h>



RTPHintPacketSampleData:: RTPHintPacketSampleData (void)

{

}


RTPHintPacketSampleData:: ~RTPHintPacketSampleData (void)

{

}



RTPHintPacketSampleData:: RTPHintPacketSampleData (
	const RTPHintPacketSampleData &)

{

	assert (1==0);

	// to do

}


RTPHintPacketSampleData &RTPHintPacketSampleData:: operator = (
	const RTPHintPacketSampleData &)

{

	assert (1==0);

	// to do

	return *this;

}


Error RTPHintPacketSampleData:: init (Tracer_p ptTracer)

{

	if (RTPHintPacketData:: init (ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETDATA_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	_pucEmbeddedData			= (unsigned char *) NULL;


	return errNoError;
}


Error RTPHintPacketSampleData:: init (
	const unsigned char *pucRTPHintPacketDataBuffer,
	Tracer_p ptTracer)

{


	if (RTPHintPacketData:: init (ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETDATA_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	_pucEmbeddedData			= (unsigned char *) NULL;

	if (*(pucRTPHintPacketDataBuffer + 1) == 0)
	{
		unsigned long				ulMediaSampleIdentifier;
		unsigned long				ulDataOffset;
		unsigned long				ulDataLength;


		ulMediaSampleIdentifier				=
			ntohl (*((unsigned long *) (pucRTPHintPacketDataBuffer + 4)));

		ulDataOffset						=
			ntohl (*((unsigned long *) (pucRTPHintPacketDataBuffer + 8)));

		ulDataLength						=
			ntohs (*((unsigned short *) (pucRTPHintPacketDataBuffer + 2)));


		if (setReferenceSample (ulMediaSampleIdentifier, ulDataOffset,
			ulDataLength) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKETSAMPLEDATA_SETREFERENCESAMPLE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (*(pucRTPHintPacketDataBuffer + 1) == (unsigned char) -1)
	{
		unsigned long				ulHintSampleIdentifier;
		unsigned long				ulEmbeddedDataLength;
		unsigned long				ulCurrentRTPHintSampleOffsetForEmbeddedData;


		ulHintSampleIdentifier							=
			ntohl (*((unsigned long *) (pucRTPHintPacketDataBuffer + 4)));

		ulCurrentRTPHintSampleOffsetForEmbeddedData		=
			ntohl (*((unsigned long *) (pucRTPHintPacketDataBuffer + 8)));

		ulEmbeddedDataLength							=
			ntohs (*((unsigned short *) (pucRTPHintPacketDataBuffer + 2)));

		if (setEmbeddedReferenceSample (
			ulHintSampleIdentifier,
			(unsigned char *) (pucRTPHintPacketDataBuffer +
			ulCurrentRTPHintSampleOffsetForEmbeddedData),
			ulEmbeddedDataLength,
			ulCurrentRTPHintSampleOffsetForEmbeddedData) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKETSAMPLEDATA_SETEMBEDDEDREFERENCESAMPLE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETSAMPLEDATA_WRONGTRACKREFERENCEINDEX);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}



	return errNoError;
}


Error RTPHintPacketSampleData:: finish (void)

{

	if (_pucEmbeddedData != (unsigned char *) NULL)
	{
		delete [] _pucEmbeddedData;
		_pucEmbeddedData			= (unsigned char *) NULL;
	}

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


Error RTPHintPacketSampleData:: createProperties (
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

	if (_mui8pTrackReferenceIndex. init ("TrackReferenceIndex", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui16pSampleLength. init ("SampleLength", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui32pSampleIdentifier. init ("SampleIdentifier", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui32pSampleOffset. init ("SampleOffset", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui16pBytesPerBlock. init ("BytesPerBlock", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui16pSamplesPerBlock. init ("SamplesPerBlock", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
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


Error RTPHintPacketSampleData:: getPropertiesSize (
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

	if (_mui8pTrackReferenceIndex. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mui16pSampleLength. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mui32pSampleIdentifier. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mui32pSampleOffset. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mui16pBytesPerBlock. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;

	if (_mui16pSamplesPerBlock. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize                  += ulSize;


	return errNoError;
}


Error RTPHintPacketSampleData:: getTrackReferenceIndex (
	unsigned long *pulTrackReferenceIndex)

{

	if (_mui8pTrackReferenceIndex. getValue (pulTrackReferenceIndex, 0) !=
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


Error RTPHintPacketSampleData:: getSampleLength (
	unsigned long *pulSampleLength)

{

	if (_mui16pSampleLength. getValue (pulSampleLength, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacketSampleData:: getSampleIdentifier (
	unsigned long *pulSampleIdentifier)

{

	if (_mui32pSampleIdentifier. getValue (pulSampleIdentifier, 0) !=
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


Error RTPHintPacketSampleData:: getSampleOffset (
	unsigned long *pulSampleOffset)

{

	if (_mui32pSampleOffset. getValue (pulSampleOffset, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacketSampleData:: getBytesPerBlock (
	unsigned long *pulBytesPerBlock)

{

	if (_mui16pBytesPerBlock. getValue (pulBytesPerBlock, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacketSampleData:: getSamplesPerBlock (
	unsigned long *pulSamplesPerBlock)

{

	if (_mui16pSamplesPerBlock. getValue (pulSamplesPerBlock, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacketSampleData:: setReferenceSample (
	unsigned long ulMediaSampleIdentifier,
	unsigned long ulDataOffset, unsigned long ulDataLength)

{

	unsigned long					ulType;
	unsigned long					ulTrackReferenceIndex;
	unsigned long					ulBytesPerBlock;
	unsigned long					ulSamplesPerBlock;
	unsigned long					ulSizeChangedInBytes;


	ulType								= 2;
	if (_mui8pType. setValue (ulType, 0, &ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__, 
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	ulTrackReferenceIndex				= 0;
	if (_mui8pTrackReferenceIndex. setValue (ulTrackReferenceIndex, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__, 
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui16pSampleLength. setValue (ulDataLength, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__, 
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui32pSampleIdentifier. setValue (ulMediaSampleIdentifier, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__, 
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui32pSampleOffset. setValue (ulDataOffset, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__, 
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	ulBytesPerBlock					= 1;
	if (_mui16pBytesPerBlock. setValue (ulBytesPerBlock, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__, 
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	ulSamplesPerBlock				= 1;
	if (_mui16pSamplesPerBlock. setValue (ulSamplesPerBlock, 0,
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


Error RTPHintPacketSampleData:: setEmbeddedReferenceSample (
	unsigned long ulHintSampleIdentifier, unsigned char *pucEmbeddedData,
	unsigned long ulEmbeddedDataLength, unsigned long ulDataOffset)

{

	unsigned long					ulType;
	unsigned long					ulTrackReferenceIndex;
	unsigned long					ulBytesPerBlock;
	unsigned long					ulSamplesPerBlock;
	unsigned long					ulSizeChangedInBytes;


	if ((_pucEmbeddedData = new unsigned char [ulEmbeddedDataLength]) ==
		(unsigned char *) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__, 
			RTP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	memcpy (_pucEmbeddedData, pucEmbeddedData, ulEmbeddedDataLength);

	ulType								= 2;
	if (_mui8pType. setValue (ulType, 0, &ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__, 
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] _pucEmbeddedData;
		_pucEmbeddedData			= (unsigned char *) NULL;

		return err;
	}

	ulTrackReferenceIndex				= (unsigned long) -1;
	if (_mui8pTrackReferenceIndex. setValue (ulTrackReferenceIndex, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__, 
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] _pucEmbeddedData;
		_pucEmbeddedData			= (unsigned char *) NULL;

		return err;
	}

	if (_mui16pSampleLength. setValue (ulEmbeddedDataLength, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__, 
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] _pucEmbeddedData;
		_pucEmbeddedData			= (unsigned char *) NULL;

		return err;
	}

	if (_mui32pSampleIdentifier. setValue (ulHintSampleIdentifier, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__, 
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] _pucEmbeddedData;
		_pucEmbeddedData			= (unsigned char *) NULL;

		return err;
	}

	// _mui32pSampleOffset is initialized to 0 because we will know this
	// value when we will write the hint sample in the file
	// (see the getPacketData method)
	if (_mui32pSampleOffset. setValue (ulDataOffset, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__, 
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] _pucEmbeddedData;
		_pucEmbeddedData			= (unsigned char *) NULL;

		return err;
	}

	ulBytesPerBlock					= 1;
	if (_mui16pBytesPerBlock. setValue (ulBytesPerBlock, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__, 
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] _pucEmbeddedData;
		_pucEmbeddedData			= (unsigned char *) NULL;

		return err;
	}

	ulSamplesPerBlock				= 1;
	if (_mui16pSamplesPerBlock. setValue (ulSamplesPerBlock, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__, 
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] _pucEmbeddedData;
		_pucEmbeddedData			= (unsigned char *) NULL;

		return err;
	}


	return errNoError;
}


Error RTPHintPacketSampleData:: getPacketHintData (
	unsigned char *pucRTPHintSample,
	unsigned long *pulCurrentRTPHintSampleOffset,
	unsigned long *pulCurrentRTPHintSampleOffsetForEmbeddedData,
	unsigned long ulRTPHintSampleSize)

{

	if (_pucEmbeddedData != (unsigned char *) NULL)
	{
		unsigned long					ulEmbeddedDataLength;
		unsigned long					ulSizeChangedInBytes;


		if (_mui16pSampleLength. getValue (&ulEmbeddedDataLength, 0) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		memcpy (
			pucRTPHintSample + *pulCurrentRTPHintSampleOffsetForEmbeddedData,
			_pucEmbeddedData, ulEmbeddedDataLength);

		if (_mui32pSampleOffset. setValue (
			*pulCurrentRTPHintSampleOffsetForEmbeddedData, 0,
			&ulSizeChangedInBytes) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		*pulCurrentRTPHintSampleOffsetForEmbeddedData			+=
			ulEmbeddedDataLength;
	}

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

	if (_mui8pTrackReferenceIndex. getValue (
		pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulCurrentRTPHintSampleOffset					+= 1;

	if (_mui16pSampleLength. getValue (
		pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulCurrentRTPHintSampleOffset					+= 2;

	if (_mui32pSampleIdentifier. getValue (
		pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulCurrentRTPHintSampleOffset					+= 4;

	if (_mui32pSampleOffset. getValue (
		pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulCurrentRTPHintSampleOffset					+= 4;

	if (_mui16pBytesPerBlock. getValue (
		pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulCurrentRTPHintSampleOffset					+= 2;

	if (_mui16pSamplesPerBlock. getValue (
		pucRTPHintSample + *pulCurrentRTPHintSampleOffset, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulCurrentRTPHintSampleOffset					+= 2;


	return errNoError;
}


Error RTPHintPacketSampleData:: appendPayloadDataToRTPPacket (
	RTPPacket_p prpRTPPacket,
	std:: vector<MP4TrackInfo_p> *pvReferencesTracksInfo,
	MP4RootAtom_p pmaRootAtom, 	MP4TrackInfo_p pmtiTrakInfo,
	RTPStreamRealTimeInfo_p prsrtRTPMediaStreamRealTimeInfo,
	RTPStreamRealTimeInfo_p prsrtRTPHintStreamRealTimeInfo)


{

	unsigned long				ulTrackReferenceIndex;
	unsigned long				ulTrackSampleNumber;
	unsigned long				ulOffsetWithinSample;
	#ifdef WIN32
		__int64						llLengthToRead;
	#else
		long long					llLengthToRead;
	#endif



	if (getTrackReferenceIndex (&ulTrackReferenceIndex) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETSAMPLEDATA_GETTRACKREFERENCEINDEX_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	{
		unsigned long				ulLengthToRead;

		if (getSampleLength (&ulLengthToRead) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKETSAMPLEDATA_GETSAMPLELENGTH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		llLengthToRead				= ulLengthToRead;
	}

	if (getSampleIdentifier (&ulTrackSampleNumber) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETSAMPLEDATA_GETBYTESPERBLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (getSampleOffset (&ulOffsetWithinSample) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETSAMPLEDATA_GETSAMPLEOFFSET_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (ulTrackReferenceIndex == (unsigned long) -1)
	{
		if (RTPHintPacketSampleData:: appendPayloadDataToRTPPacket (
			prpRTPPacket, pmaRootAtom, pmtiTrakInfo,
			ulTrackReferenceIndex,
			llLengthToRead,
			ulTrackSampleNumber,
			ulOffsetWithinSample,
			0,		// will not be used
			0,		// will not be used
			prsrtRTPHintStreamRealTimeInfo,
			_ptTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKETSAMPLEDATA_APPENDPAYLOADDATATORTPPACKET_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		unsigned long				ulBytesPerCompressionBlock;
		unsigned long				ulSamplePerCompressionBlock;
		MP4TrackInfo_p				pmtiMediaTrackInfo;


		if (getBytesPerBlock (&ulBytesPerCompressionBlock) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKETSAMPLEDATA_GETBYTESPERBLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (ulBytesPerCompressionBlock == 0)
			ulBytesPerCompressionBlock			= 1;

		if (getSamplesPerBlock (&ulSamplePerCompressionBlock) !=
			errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKETSAMPLEDATA_GETSAMPLESPERBLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (ulSamplePerCompressionBlock == 0)
			ulSamplePerCompressionBlock			= 1;

		pmtiMediaTrackInfo		= (*pvReferencesTracksInfo) [
			ulTrackReferenceIndex];

		if (RTPHintPacketSampleData:: appendPayloadDataToRTPPacket (
			prpRTPPacket, pmaRootAtom, 	pmtiMediaTrackInfo,
			ulTrackReferenceIndex,
			llLengthToRead,
			ulTrackSampleNumber,
			ulOffsetWithinSample,
			ulBytesPerCompressionBlock,
			ulSamplePerCompressionBlock,
			prsrtRTPMediaStreamRealTimeInfo,
			_ptTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKETSAMPLEDATA_APPENDPAYLOADDATATORTPPACKET_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error RTPHintPacketSampleData:: appendPayloadDataToRTPPacket (
	RTPPacket_p prpRTPPacket,
	MP4RootAtom_p pmaRootAtom,
	MP4TrackInfo_p pmtiTrakInfo,
	unsigned long ulTrackReferenceIndex,
	unsigned long ulDataLength,
	unsigned long ulMediaSampleIdentifier,
	unsigned long ulDataOffset,
	unsigned long ulBytesPerCompressionBlock,
	unsigned long ulSamplePerCompressionBlock,
	RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
	Tracer_p ptTracer)

{

	// unsigned long				ulTrackReferenceIndex;
	unsigned long				ulTrackSampleNumber;
	unsigned long				ulOffsetWithinSample;
	unsigned long				ulSampleLength;
	unsigned long				ulSampleDescriptionIndex;
	#ifdef WIN32
		__int64						llLengthToRead;
		__int64						ullDataOffset;
	#else
		long long					llLengthToRead;
		unsigned long long			ullDataOffset;
	#endif



	llLengthToRead				= ulDataLength;

	ulTrackSampleNumber			= ulMediaSampleIdentifier;

	ulOffsetWithinSample		= ulDataOffset;

	if (ulTrackReferenceIndex == (unsigned long) -1)
	{
		MP4TrackInfo_p		pmtiHintTrakInfo;
		MP4StscAtom_p		pmaHintStscAtom;
		MP4StszAtom_p		pmaHintStszAtom;
		MP4StcoAtom_p		pmaHintStcoAtom;


		// in this case the pmaTrakAtom parameter must be an hint track
		pmtiHintTrakInfo			= pmtiTrakInfo;

		if (pmtiHintTrakInfo -> getStscAtom (&pmaHintStscAtom, true) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSCATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pmtiHintTrakInfo -> getStszAtom (&pmaHintStszAtom, true) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSZATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pmtiHintTrakInfo -> getStcoAtom (&pmaHintStcoAtom, true) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTCOATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (RTPUtility:: getSampleInfoFromSampleNumber (
			pmaHintStscAtom, pmaHintStszAtom, pmaHintStcoAtom,
			ulTrackSampleNumber, &ullDataOffset, &ulSampleLength,
			&ulSampleDescriptionIndex,
			prsrtRTPStreamRealTimeInfo,
			ptTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_GETSAMPLEINFOFROMSAMPLENUMBER_FAILED,
				1, ulTrackSampleNumber);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		ullDataOffset				=
			ullDataOffset + ulOffsetWithinSample;

		if (RTPUtility:: appendToRTPPacketFromMdatAtom (pmaRootAtom,
			ullDataOffset, prpRTPPacket, llLengthToRead,
			ptTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_APPENDTORTPPACKETFROMMDATATOM_FAILED,
				2, ullDataOffset, llLengthToRead);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		Boolean_t				bIsCompressed;
		MP4TrackInfo_p			pmtiMediaTrakInfo;
		MP4StscAtom_p			pmaMediaStscAtom;
		MP4StszAtom_p			pmaMediaStszAtom;
		MP4StcoAtom_p			pmaMediaStcoAtom;


		// in this case the pmaTrakAtom parameter must be a media track
		pmtiMediaTrakInfo			= pmtiTrakInfo;

		/*
		if (getBytesPerBlock (&ulBytesPerCompressionBlock) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_RTPHINTPACKETSAMPLEDATA_GETBYTESPERBLOCK_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (ulBytesPerCompressionBlock == 0)
			ulBytesPerCompressionBlock			= 1;

		if (getSamplesPerBlock (&ulSamplePerCompressionBlock) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_RTPHINTPACKETSAMPLEDATA_GETSAMPLESPERBLOCK_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (ulSamplePerCompressionBlock == 0)
			ulSamplePerCompressionBlock			= 1;

		pmaMediaTrakAtom		= (*pvReferencesTracks) [ulTrackReferenceIndex];
		*/

		if (pmtiMediaTrakInfo -> getStscAtom (&pmaMediaStscAtom, true) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSCATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pmtiMediaTrakInfo -> getStszAtom (&pmaMediaStszAtom, true) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSZATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pmtiMediaTrakInfo -> getStcoAtom (&pmaMediaStcoAtom, true) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTCOATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (RTPUtility:: getSampleInfoFromSampleNumber (
			pmaMediaStscAtom, pmaMediaStszAtom, pmaMediaStcoAtom,
			ulTrackSampleNumber, &ullDataOffset, &ulSampleLength,
			&ulSampleDescriptionIndex,
			prsrtRTPStreamRealTimeInfo,
			ptTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_GETSAMPLEINFOFROMSAMPLENUMBER_FAILED,
				1, ulTrackSampleNumber);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		bIsCompressed				= false;

		if ((1 == ulSamplePerCompressionBlock &&
			1 == ulBytesPerCompressionBlock && ulSampleLength == 1) ||
			// special case the data is compressed and
			// the sample's byte offset is really a byte offset
			// into a chunk
			(!(1 == ulSamplePerCompressionBlock &&
			1 == ulBytesPerCompressionBlock))
			// compressed data is normally defined by
			// ulBytesPerCompressionBlock or ulSamplePerCompressionBlock
			)
		{
			// printf("track = %ld sample  = %ld is compressed \n",this,
			// ulTrackSampleNumber);
			// printf("is compressed ulBytesPerCompressionBlock = %ld
			// ulSamplePerCompressionBlock = %d ulSampleLength = %ld\n",
			// ulBytesPerCompressionBlock, ulSamplePerCompressionBlock,
			// ulSampleLength);
			bIsCompressed = true;
		}

		// Get the information about this sample and compute an offset.
		// If the BPCB and SPCB are 1, then we use the standard sample
		// routines to get the location of this sample, otherwise
		// we have to compute it ourselves.
		if (bIsCompressed)
		{
			// Media track sample compressed

			unsigned long		ulSamplesPerChunk;
			unsigned long		ulChunkNumber;
			unsigned long		ulSampleOffsetInChunk;
			unsigned long		ulChunkOffset;
			unsigned long		ulCompressionBlocksToSkip;
			#ifdef WIN32
				__int64				llEndOfSampleInChunk;
				__int64				llSampleFirstPartLength;
				__int64				llRemainingLength;
			#else
				long long			llEndOfSampleInChunk;
				long long			llSampleFirstPartLength;
				long long			llRemainingLength;
			#endif
			unsigned long		ulFirstSampleNumber;
			unsigned long		ulLastSampleNumber;
			unsigned long		ulSampleRangeSize;


			ulCompressionBlocksToSkip	= (unsigned long)
				((double) ulOffsetWithinSample /
				(double) ulBytesPerCompressionBlock);

			ulTrackSampleNumber			+= ulCompressionBlocksToSkip *
				ulSamplePerCompressionBlock;
			ulOffsetWithinSample		-= ulCompressionBlocksToSkip *
				ulBytesPerCompressionBlock;

			// readoffset should always be 0 after this 
			// start gathering chunk info to check sample length
			// against chunk length
			if (RTPUtility:: getChunkInfoFromSampleNumber (
				pmaMediaStscAtom,
				ulTrackSampleNumber, &ulSamplesPerChunk,
				&ulChunkNumber, &ulSampleDescriptionIndex,
				&ulSampleOffsetInChunk,
				prsrtRTPStreamRealTimeInfo,
				ptTracer) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPUTILITY_GETSAMPLEINFOFROMSAMPLENUMBER_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (MP4Utility:: getChunkOffsetFromChunkNumber (
				pmaMediaStcoAtom, ulChunkNumber, &ulChunkOffset,
				ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETCHUNKOFFSETFROMCHUNKNUMBER_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			#ifdef WIN32
				ullDataOffset				= (__int64)
					(ulChunkOffset  + ((double) ulSampleOffsetInChunk *
					((double) ulBytesPerCompressionBlock /
					(double) ulSamplePerCompressionBlock)));
			#else
				ullDataOffset				= (unsigned long long)
					(ulChunkOffset  + ((double) ulSampleOffsetInChunk *
					((double) ulBytesPerCompressionBlock /
					(double) ulSamplePerCompressionBlock)));
			#endif

			if (MP4Utility:: getChunkFirstLastSample (pmaMediaStscAtom,
				ulChunkNumber, &ulFirstSampleNumber,
				&ulLastSampleNumber, ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETCHUNKFIRSTLASTSAMPLE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (RTPUtility:: getSampleRangeSize (pmaMediaStszAtom,
				ulFirstSampleNumber, ulLastSampleNumber,
				&ulSampleRangeSize, prsrtRTPStreamRealTimeInfo,
				ptTracer) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPUTILITY_GETSAMPLERANGESIZE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			ulSampleRangeSize				= (unsigned long)
				((double) ulSampleRangeSize *
				((double) ulBytesPerCompressionBlock /
				(double) ulSamplePerCompressionBlock));

			llEndOfSampleInChunk				=
				ulSampleRangeSize + ulChunkOffset;
			// the first piece length = maxlen - start
			llSampleFirstPartLength			=
				llEndOfSampleInChunk - ullDataOffset;
			// the read - first piece is either <= 0 or the remaining
			// amount.                             
			llRemainingLength				=
				llLengthToRead - llSampleFirstPartLength;

			if ((llRemainingLength > 0) && (llSampleFirstPartLength > 0))
				// this packet is split across chunks
			{
				llLengthToRead				= llSampleFirstPartLength;
			}
			else
			{
				// this is still needed. For some movies the compressed
				// split packet calculation doesn't match the simple
				// dataOffset calc below --a problem with
				// sampleOffsetInChunk
				llRemainingLength			= 0;
				ullDataOffset					=
					(unsigned long) (ulChunkOffset +
					ulOffsetWithinSample + (ulSampleOffsetInChunk *
					((double) ulBytesPerCompressionBlock /
					(double) ulSamplePerCompressionBlock)));
			}

			if (RTPUtility:: appendToRTPPacketFromMdatAtom (
				pmaRootAtom, ullDataOffset,
				prpRTPPacket, llLengthToRead, ptTracer) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPUTILITY_APPENDTORTPPACKETFROMMDATATOM_FAILED,
					2, ullDataOffset, llLengthToRead);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			// loop if packet is split across more than just two chunks
			while (llRemainingLength > 0)
			{
				// set the read to what is left
				llLengthToRead			= llRemainingLength;
				// The rest of the sample is in the next N chunks
				ulChunkNumber++;

				if (MP4Utility:: getChunkOffsetFromChunkNumber (
					pmaMediaStcoAtom, ulChunkNumber, &ulChunkOffset,
					ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETCHUNKOFFSETFROMCHUNKNUMBER_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				// the location of the data starting at the beginning
				// of the chunk
				ullDataOffset				= ulChunkOffset;   

				if (MP4Utility:: getChunkFirstLastSample (pmaMediaStscAtom,
					ulChunkNumber, &ulFirstSampleNumber,
					&ulLastSampleNumber, ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UTILITY_GETCHUNKFIRSTLASTSAMPLE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (RTPUtility:: getSampleRangeSize (pmaMediaStszAtom,
					ulFirstSampleNumber, ulLastSampleNumber,
					&ulSampleRangeSize, prsrtRTPStreamRealTimeInfo,
					ptTracer) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPUTILITY_GETSAMPLERANGESIZE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				ulSampleRangeSize				= (unsigned long)
					((double) ulSampleRangeSize *
					((double) ulBytesPerCompressionBlock /
					(double) ulSamplePerCompressionBlock));

				// read in the whole chunk and keep going
				if (ulSampleRangeSize < llRemainingLength) 
				{
					llRemainingLength			-= ulSampleRangeSize;
					llLengthToRead				= ulSampleRangeSize;
				}
				else
				{
					// done reading this packet
					llRemainingLength			= 0; 
				}

				if (RTPUtility:: appendToRTPPacketFromMdatAtom (
					pmaRootAtom, ullDataOffset,
					prpRTPPacket, llLengthToRead, ptTracer) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPUTILITY_APPENDTORTPPACKETFROMMDATATOM_FAILED,
						2, ullDataOffset, llLengthToRead);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}
		}
		else
		{
			Error_t				errAppend;


			// Media track sample not compressed
			ullDataOffset				=
				ullDataOffset + ulOffsetWithinSample;

			// NOTA BENE:
			// read according ulSampleDescriptionIndex
			// (see QTHintTrack.cpp line 679)

			if ((errAppend = RTPUtility:: appendToRTPPacketFromMdatAtom (
				pmaRootAtom, ullDataOffset,
				prpRTPPacket, llLengthToRead, ptTracer)) != errNoError)
			{
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errAppend, __FILE__, __LINE__);

				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPUTILITY_APPENDTORTPPACKETFROMMDATATOM_FAILED,
					2, ullDataOffset, llLengthToRead);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
	}


	return errNoError;
}

