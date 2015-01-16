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


#include "MP4SyncLayerConfigDescr.h"
#include "FileIO.h"
#ifdef WIN32
	#include <stdio.h>
#else
	#include <unistd.h>
#endif
#include <assert.h>



MP4SyncLayerConfigDescr:: MP4SyncLayerConfigDescr (void): MP4Descr ()

{

}


MP4SyncLayerConfigDescr:: ~MP4SyncLayerConfigDescr (void)

{

}



MP4SyncLayerConfigDescr:: MP4SyncLayerConfigDescr (
	const MP4SyncLayerConfigDescr &)

{

	assert (1==0);

	// to do

}


MP4SyncLayerConfigDescr &MP4SyncLayerConfigDescr:: operator = (
	const MP4SyncLayerConfigDescr &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4SyncLayerConfigDescr:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile, __int64 ullAtomStartOffset,
		__int64 ullAtomSize, unsigned long ulHeaderSize,
		unsigned char ucNumBytesForDescriptorSize, unsigned long ulTag,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4SyncLayerConfigDescr:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile, unsigned long long ullAtomStartOffset,
		unsigned long long ullAtomSize, unsigned long ulHeaderSize,
		unsigned char ucNumBytesForDescriptorSize, unsigned long ulTag,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#endif

{

	return MP4Descr:: init (pmaParentAtom, pfFile,
		ullAtomStartOffset, ullAtomSize, ulHeaderSize,
		ucNumBytesForDescriptorSize, ulTag, pmtMP4File,
		lAtomLevel, bUse64Bits, bUseMP4ConsistencyCheck,
		sStandard, ptTracer);
}


Error MP4SyncLayerConfigDescr:: prepareChildrensAtomsInfo (void)

{

	return errNoError;
}


Error MP4SyncLayerConfigDescr:: createProperties (
	unsigned long *pulPropertiesSize)

{

	unsigned long				ulValue;
	#ifdef WIN32
		__int64						ullValue;
	#else
		unsigned long long			ullValue;
	#endif
	Boolean_t					bIsImplicitProperty;
	unsigned char				ucPredefined;
	Boolean_t					bDurationFlag;


	ulValue							= 0;
	ullValue						= 0;
	bIsImplicitProperty				= false;

	// 0: Predefined
	ulValue							= 2;
	if (_mui8pPredefined. init ("Predefined", 1, &bIsImplicitProperty,
		&ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	ulValue							= 0;

	if (getPredefined (&ucPredefined) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (ucPredefined == 0)
		bIsImplicitProperty				= false;
	else
		bIsImplicitProperty				= true;

	// 1: UseAccessUnitStartFlag (1 bit) + UseAccessUnitEndFlag (1 bit) +
	// 		UseRandomAccessPointFlag (1 bit) +
	// 		HasRandomAccessUnitsOnlyFlag (1 bits) + UsePaddingFlag (1 bits) +
	// 		UseTimeStampsFlag (1 bits) + UseIdleFlag (1 bits) +
	// 		DurationFlag (1 bits)
	if (_mui8pSyncLayerConfigDescrFlags1. init (
		"SyncLayerConfigDescrFlags1: UseAccessUnitStartFlag (1 bit) + UseAccessUnitEndFlag (1 bit) + UseRandomAccessPointFlag (1 bit) + HasRandomAccessUnitsOnlyFlag (1 bits) + UsePaddingFlag (1 bits) + UseTimeStampsFlag (1 bits) + UseIdleFlag (1 bits) + DurationFlag (1 bits)",
		1, &bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (setUseTimeStampsFlag (true) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 2: TimeStampResolution
	if (_mui32pTimeStampResolution. init ("TimeStampResolution", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 3: OCRResolution
	if (_mui32pOCRResolution. init ("OCRResolution", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 4: TimeStampLength
	if (_mui8pTimeStampLength. init ("TimeStampLength", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 5: OCRLength
	if (_mui8pOCRLength. init ("OCRLength", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 6: AULength
	if (_mui8pAULength. init ("AULength", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 7: InstantBitrateLength
	if (_mui8pInstantBitrateLength. init ("InstantBitrateLength", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 8: degradationPriortyLength (4 bits) + AUSeqNumLength (5 bits) +
	//		packetSeqNumLength (5 bits) + reserved (2 bits)
	if (_mui16pSyncLayerConfigDescrFlags2. init (
		"SyncLayerConfigDescrFlags2: degradationPriortyLength (4 bits) + AUSeqNumLength (5 bits) + packetSeqNumLength (5 bits) + reserved (2 bits)",
		1, &bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (setReserved (3) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (getDurationFlag (&bDurationFlag) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bDurationFlag)
		bIsImplicitProperty				= false;
	else
		bIsImplicitProperty				= true;

	// 9: TimeScale
	if (_mui32pTimeScale. init ("TimeScale", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 10: AccessUnitDuration
	if (_mui16pAccessUnitDuration. init ("AccessUnitDuration", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 11: CompositionUnitDuration
	if (_mui16pCompositionUnitDuration. init ("CompositionUnitDuration", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	/*
	if (getUseTimeStampsFlag (&bUseTimeStampsFlag) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (!bUseTimeStampsFlag)
		bIsImplicitProperty				= false;
	else
		bIsImplicitProperty				= true;
	*/
	// the next fields is implicit because the TimeStampLength field is set to 0
	bIsImplicitProperty				= true;

	_ulTimeStampLength				= 64;

	// 12: StartDecodingTimeStamp
	if (_mui64pStartDecodingTimeStamp. init ("StartDecodingTimeStamp", 1,
		&bIsImplicitProperty, &ullValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 13: StartCompositionTimeStamp
	if (_mui64pStartCompositionTimeStamp. init ("StartCompositionTimeStamp", 1,
		&bIsImplicitProperty, &ullValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	bIsImplicitProperty				= false;

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


Error MP4SyncLayerConfigDescr:: readProperties (
	unsigned long *pulPropertiesSize)

{

	Boolean_t					bIsImplicitProperty;
	unsigned long				ulValue;
	#ifdef WIN32
		__int64						ullValue;
	#else
		unsigned long long			ullValue;
	#endif
	unsigned char				ucPredefined;
	unsigned char				ucTimeStampLength;
	Boolean_t					bDurationFlag;
	Boolean_t					bUseTimeStampsFlag;
	#ifdef WIN32
		__int64						llCurrentFilePosition;
	#else
		long long					llCurrentFilePosition;
	#endif


	if (_pfFile -> seek (_ullAtomStartOffset + _ulHeaderSize,
		SEEK_SET, &llCurrentFilePosition) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEREADER_SEEK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	bIsImplicitProperty				= true;
	ulValue							= 0;
	ullValue						= 0;

	// 0: Predefined
	if (_mui8pPredefined. init ("Predefined", 1, _pfFile,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (getPredefined (&ucPredefined) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (ucPredefined == 0)
	{
		// 1: UseAccessUnitStartFlag (1 bit) + UseAccessUnitEndFlag (1 bit) +
		// 		UseRandomAccessPointFlag (1 bit) +
		// 		HasRandomAccessUnitsOnlyFlag (1 bits) +
		// 		UsePaddingFlag (1 bits) +
		// 		UseTimeStampsFlag (1 bits) + UseIdleFlag (1 bits) +
		// 		DurationFlag (1 bits)
		if (_mui8pSyncLayerConfigDescrFlags1. init (
			"SyncLayerConfigDescrFlags1: UseAccessUnitStartFlag (1 bit) + UseAccessUnitEndFlag (1 bit) + UseRandomAccessPointFlag (1 bit) + HasRandomAccessUnitsOnlyFlag (1 bits) + UsePaddingFlag (1 bits) + UseTimeStampsFlag (1 bits) + UseIdleFlag (1 bits) + DurationFlag (1 bits)",
			1, _pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 2: TimeStampResolution
		if (_mui32pTimeStampResolution. init ("TimeStampResolution", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 3: OCRResolution
		if (_mui32pOCRResolution. init ("OCRResolution", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 4: TimeStampLength
		if (_mui8pTimeStampLength. init ("TimeStampLength", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 5: OCRLength
		if (_mui8pOCRLength. init ("OCRLength", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 6: AULength
		if (_mui8pAULength. init ("AULength", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 7: InstantBitrateLength
		if (_mui8pInstantBitrateLength. init ("InstantBitrateLength", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 8: degradationPriortyLength (4 bits) + AUSeqNumLength (5 bits) +
		//		packetSeqNumLength (5 bits) + reserved (2 bits) +
		if (_mui16pSyncLayerConfigDescrFlags2. init (
			"SyncLayerConfigDescrFlags2: degradationPriortyLength (4 bits) + AUSeqNumLength (5 bits) + packetSeqNumLength (5 bits) + reserved (2 bits)",
			1, _pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		// 1: UseAccessUnitStartFlag (1 bit) + UseAccessUnitEndFlag (1 bit) +
		// 		UseRandomAccessPointFlag (1 bit) +
		// 		HasRandomAccessUnitsOnlyFlag (1 bits) +
		// 		UsePaddingFlag (1 bits) +
		// 		UseTimeStampsFlag (1 bits) + UseIdleFlag (1 bits) +
		// 		DurationFlag (1 bits)
		if (_mui8pSyncLayerConfigDescrFlags1. init (
			"SyncLayerConfigDescrFlags1: UseAccessUnitStartFlag (1 bit) + UseAccessUnitEndFlag (1 bit) + UseRandomAccessPointFlag (1 bit) + HasRandomAccessUnitsOnlyFlag (1 bits) + UsePaddingFlag (1 bits) + UseTimeStampsFlag (1 bits) + UseIdleFlag (1 bits) + DurationFlag (1 bits)",
			1, &bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 2: TimeStampResolution
		if (_mui32pTimeStampResolution. init ("TimeStampResolution", 1,
			&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 3: OCRResolution
		if (_mui32pOCRResolution. init ("OCRResolution", 1,
			&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 4: TimeStampLength
		if (_mui8pTimeStampLength. init ("TimeStampLength", 1,
			&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (ucPredefined == 1)
		{
			if (setUseTimeStampsFlag (false) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_SETVALUE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if (setTimeStampResolution (1000) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_SETVALUE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if (setTimeStampLength (32) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_SETVALUE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
		}
		else if (ucPredefined == 2)
		{
			if (setUseTimeStampsFlag (true) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_SETVALUE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
		}

		// 5: OCRLength
		if (_mui8pOCRLength. init ("OCRLength", 1,
			&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 6: AULength
		if (_mui8pAULength. init ("AULength", 1,
			&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 7: InstantBitrateLength
		if (_mui8pInstantBitrateLength. init ("InstantBitrateLength", 1,
			&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 8: degradationPriortyLength (4 bits) + AUSeqNumLength (5 bits) +
		//		packetSeqNumLength (5 bits) + reserved (2 bits) +
		if (_mui16pSyncLayerConfigDescrFlags2. init (
			"SyncLayerConfigDescrFlags2: degradationPriortyLength (4 bits) + AUSeqNumLength (5 bits) + packetSeqNumLength (5 bits) + reserved (2 bits)",
			1, &bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	if (getDurationFlag (&bDurationFlag) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bDurationFlag)
	{
		// 9: TimeScale
		if (_mui32pTimeScale. init ("TimeScale", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 10: AccessUnitDuration
		if (_mui16pAccessUnitDuration. init ("AccessUnitDuration", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 11: CompositionUnitDuration
		if (_mui16pCompositionUnitDuration. init ("CompositionUnitDuration", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		// 9: TimeScale
		if (_mui32pTimeScale. init ("TimeScale", 1,
			&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 10: AccessUnitDuration
		if (_mui16pAccessUnitDuration. init ("AccessUnitDuration", 1,
			&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 11: CompositionUnitDuration
		if (_mui16pCompositionUnitDuration. init ("CompositionUnitDuration", 1,
			&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	if (getTimeStampLength (&ucTimeStampLength) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (getUseTimeStampsFlag (&bUseTimeStampsFlag) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (ucTimeStampLength < 64)
	{
		_ulTimeStampLength			= ucTimeStampLength;
	}
	else
	{
		_ulTimeStampLength			= 64;
	}

	switch (_ulTimeStampLength)
	{
		case 0:
			// 12: StartDecodingTimeStamp
			if (_mui64pStartDecodingTimeStamp. init (
				"StartDecodingTimeStamp", 1,
				&bIsImplicitProperty, &ullValue, _ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			// 13: StartCompositionTimeStamp
			if (_mui64pStartCompositionTimeStamp. init (
				"StartCompositionTimeStamp",
				1, &bIsImplicitProperty, &ullValue, _ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			break;
		case 8:
			if (!bUseTimeStampsFlag)
			{
				// 12: StartDecodingTimeStamp
				if (_mui8pStartDecodingTimeStamp. init (
					"StartDecodingTimeStamp", 1,
					_pfFile, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				// 13: StartCompositionTimeStamp
				if (_mui8pStartCompositionTimeStamp. init (
					"StartCompositionTimeStamp",
					1, _pfFile, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}
			else
			{
				// 12: StartDecodingTimeStamp
				if (_mui8pStartDecodingTimeStamp. init (
					"StartDecodingTimeStamp", 1,
					&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				// 13: StartCompositionTimeStamp
				if (_mui8pStartCompositionTimeStamp. init (
					"StartCompositionTimeStamp",
					1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			break;
		case 16:
			if (!bUseTimeStampsFlag)
			{
				// 12: StartDecodingTimeStamp
				if (_mui16pStartDecodingTimeStamp. init (
					"StartDecodingTimeStamp", 1,
					_pfFile, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				// 13: StartCompositionTimeStamp
				if (_mui16pStartCompositionTimeStamp. init (
					"StartCompositionTimeStamp",
					1, _pfFile, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}
			else
			{
				// 12: StartDecodingTimeStamp
				if (_mui16pStartDecodingTimeStamp. init (
					"StartDecodingTimeStamp", 1,
					&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				// 13: StartCompositionTimeStamp
				if (_mui16pStartCompositionTimeStamp. init (
					"StartCompositionTimeStamp",
					1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			break;
		case 24:
			if (!bUseTimeStampsFlag)
			{
				// 12: StartDecodingTimeStamp
				if (_mui24pStartDecodingTimeStamp. init (
					"StartDecodingTimeStamp", 1,
					_pfFile, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				// 13: StartCompositionTimeStamp
				if (_mui24pStartCompositionTimeStamp. init (
					"StartCompositionTimeStamp",
					1, _pfFile, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}
			else
			{
				// 12: StartDecodingTimeStamp
				if (_mui24pStartDecodingTimeStamp. init (
					"StartDecodingTimeStamp", 1,
					&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				// 13: StartCompositionTimeStamp
				if (_mui24pStartCompositionTimeStamp. init (
					"StartCompositionTimeStamp",
					1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			break;
		case 32:
			if (!bUseTimeStampsFlag)
			{
				// 12: StartDecodingTimeStamp
				if (_mui32pStartDecodingTimeStamp. init (
					"StartDecodingTimeStamp", 1,
					_pfFile, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				// 13: StartCompositionTimeStamp
				if (_mui32pStartCompositionTimeStamp. init (
					"StartCompositionTimeStamp",
					1, _pfFile, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}
			else
			{
				// 12: StartDecodingTimeStamp
				if (_mui32pStartDecodingTimeStamp. init (
					"StartDecodingTimeStamp", 1,
					&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				// 13: StartCompositionTimeStamp
				if (_mui32pStartCompositionTimeStamp. init (
					"StartCompositionTimeStamp",
					1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			break;
		case 64:
			if (!bUseTimeStampsFlag)
			{
				// 12: StartDecodingTimeStamp
				if (_mui64pStartDecodingTimeStamp. init (
					"StartDecodingTimeStamp", 1,
					_pfFile, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				// 13: StartCompositionTimeStamp
				if (_mui64pStartCompositionTimeStamp. init (
					"StartCompositionTimeStamp",
					1, _pfFile, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}
			else
			{
				// 12: StartDecodingTimeStamp
				if (_mui64pStartDecodingTimeStamp. init (
					"StartDecodingTimeStamp", 1,
					&bIsImplicitProperty, &ullValue, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				// 13: StartCompositionTimeStamp
				if (_mui64pStartCompositionTimeStamp. init (
					"StartCompositionTimeStamp",
					1, &bIsImplicitProperty, &ullValue, _ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			break;
		default:
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4SYNCLAYERCONFIGDESCR_WRONGTIMESTAMPLENGTH);
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


Error MP4SyncLayerConfigDescr:: writeProperties (int iFileDescriptor)

{

	#ifdef WIN32
		__int64							ullAtomPropertiesStartOffset;
		__int64							llCurrentPosition;
	#else
		unsigned long long				ullAtomPropertiesStartOffset;
		long long						llCurrentPosition;
	#endif


	ullAtomPropertiesStartOffset			= _ullAtomStartOffset +
		1 + _ucNumBytesForDescriptorSize;

	if (FileIO:: seek (iFileDescriptor, ullAtomPropertiesStartOffset,
		SEEK_SET, &llCurrentPosition) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_SEEK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 0: Predefined
	if (_mui8pPredefined. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 1: UseAccessUnitStartFlag (1 bit) + UseAccessUnitEndFlag (1 bit) +
	// 		UseRandomAccessPointFlag (1 bit) +
	// 		HasRandomAccessUnitsOnlyFlag (1 bits) +
	// 		UsePaddingFlag (1 bits) +
	// 		UseTimeStampsFlag (1 bits) + UseIdleFlag (1 bits) +
	// 		DurationFlag (1 bits)
	if (_mui8pSyncLayerConfigDescrFlags1. write (iFileDescriptor) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 2: TimeStampResolution
	if (_mui32pTimeStampResolution. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 3: OCRResolution
	if (_mui32pOCRResolution. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 4: TimeStampLength
	if (_mui8pTimeStampLength. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 5: OCRLength
	if (_mui8pOCRLength. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 6: AULength
	if (_mui8pAULength. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 7: InstantBitrateLength
	if (_mui8pInstantBitrateLength. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 8: degradationPriortyLength (4 bits) + AUSeqNumLength (5 bits) +
	//		packetSeqNumLength (5 bits) + reserved (2 bits) +
	if (_mui16pSyncLayerConfigDescrFlags2. write (iFileDescriptor) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 9: TimeScale
	if (_mui32pTimeScale. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 10: AccessUnitDuration
	if (_mui16pAccessUnitDuration. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 11: CompositionUnitDuration
	if (_mui16pCompositionUnitDuration. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 12: StartDecodingTimeStamp
	// 13: StartCompositionTimeStamp
	switch (_ulTimeStampLength)
	{
		case 0:
			if (_mui64pStartDecodingTimeStamp. write (iFileDescriptor) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_WRITE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if (_mui64pStartCompositionTimeStamp. write (iFileDescriptor) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_WRITE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 8:
			if (_mui8pStartDecodingTimeStamp. write (iFileDescriptor) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER8BITSPROPERTY_WRITE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if (_mui8pStartCompositionTimeStamp. write (iFileDescriptor) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER8BITSPROPERTY_WRITE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 16:
			if (_mui16pStartDecodingTimeStamp. write (iFileDescriptor) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER16BITSPROPERTY_WRITE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if (_mui16pStartCompositionTimeStamp. write (iFileDescriptor) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER16BITSPROPERTY_WRITE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 24:
			if (_mui24pStartDecodingTimeStamp. write (iFileDescriptor) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER24BITSPROPERTY_WRITE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if (_mui24pStartCompositionTimeStamp. write (iFileDescriptor) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER24BITSPROPERTY_WRITE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 32:
			if (_mui32pStartDecodingTimeStamp. write (iFileDescriptor) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER32BITSPROPERTY_WRITE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if (_mui32pStartCompositionTimeStamp. write (iFileDescriptor) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER32BITSPROPERTY_WRITE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 64:
			if (_mui64pStartDecodingTimeStamp. write (iFileDescriptor) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_WRITE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if (_mui64pStartCompositionTimeStamp. write (iFileDescriptor) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_WRITE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		default:
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4SYNCLAYERCONFIGDESCR_WRONGTIMESTAMPLENGTH);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
	}


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: getPropertiesSize (
	unsigned long *pulPropertiesSize)

{

	unsigned long					ulSize;


	*pulPropertiesSize				= 0;

	// 0: Predefined
	if (_mui8pPredefined. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	// 1: UseAccessUnitStartFlag (1 bit) + UseAccessUnitEndFlag (1 bit) +
	// 		UseRandomAccessPointFlag (1 bit) +
	// 		HasRandomAccessUnitsOnlyFlag (1 bits) +
	// 		UsePaddingFlag (1 bits) +
	// 		UseTimeStampsFlag (1 bits) + UseIdleFlag (1 bits) +
	// 		DurationFlag (1 bits)
	if (_mui8pSyncLayerConfigDescrFlags1. getSize (&ulSize) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	// 2: TimeStampResolution
	if (_mui32pTimeStampResolution. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	// 3: OCRResolution
	if (_mui32pOCRResolution. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	// 4: TimeStampLength
	if (_mui8pTimeStampLength. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	// 5: OCRLength
	if (_mui8pOCRLength. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	// 6: AULength
	if (_mui8pAULength. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	// 7: InstantBitrateLength
	if (_mui8pInstantBitrateLength. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	// 8: degradationPriortyLength (4 bits) + AUSeqNumLength (5 bits) +
	//		packetSeqNumLength (5 bits) + reserved (2 bits) +
	if (_mui16pSyncLayerConfigDescrFlags2. getSize (&ulSize) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	// 9: TimeScale
	if (_mui32pTimeScale. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	// 10: AccessUnitDuration
	if (_mui16pAccessUnitDuration. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	// 11: CompositionUnitDuration
	if (_mui16pCompositionUnitDuration. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	// 12: StartDecodingTimeStamp
	// 13: StartCompositionTimeStamp
	switch (_ulTimeStampLength)
	{
		case 0:
			if (_mui64pStartDecodingTimeStamp. getSize (&ulSize) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_GETSIZE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
			*pulPropertiesSize					+= ulSize;

			if (_mui64pStartCompositionTimeStamp. getSize (&ulSize) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_GETSIZE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
			*pulPropertiesSize					+= ulSize;

			break;
		case 8:
			if (_mui8pStartDecodingTimeStamp. getSize (&ulSize) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER8BITSPROPERTY_GETSIZE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
			*pulPropertiesSize					+= ulSize;

			if (_mui8pStartCompositionTimeStamp. getSize (&ulSize) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER8BITSPROPERTY_GETSIZE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
			*pulPropertiesSize					+= ulSize;

			break;
		case 16:
			if (_mui16pStartDecodingTimeStamp. getSize (&ulSize) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
			*pulPropertiesSize					+= ulSize;

			if (_mui16pStartCompositionTimeStamp. getSize (&ulSize) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
			*pulPropertiesSize					+= ulSize;

			break;
		case 24:
			if (_mui24pStartDecodingTimeStamp. getSize (&ulSize) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER24BITSPROPERTY_GETSIZE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
			*pulPropertiesSize					+= ulSize;

			if (_mui24pStartCompositionTimeStamp. getSize (&ulSize) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER24BITSPROPERTY_GETSIZE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
			*pulPropertiesSize					+= ulSize;

			break;
		case 32:
			if (_mui32pStartDecodingTimeStamp. getSize (&ulSize) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
			*pulPropertiesSize					+= ulSize;

			if (_mui32pStartCompositionTimeStamp. getSize (&ulSize) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
			*pulPropertiesSize					+= ulSize;

			break;
		case 64:
			if (_mui64pStartDecodingTimeStamp. getSize (&ulSize) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_GETSIZE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
			*pulPropertiesSize					+= ulSize;

			if (_mui64pStartCompositionTimeStamp. getSize (&ulSize) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_GETSIZE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
			*pulPropertiesSize					+= ulSize;

			break;
		default:
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4SYNCLAYERCONFIGDESCR_WRONGTIMESTAMPLENGTH);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
	}

	return errNoError;
}


Error MP4SyncLayerConfigDescr:: getType (char *pType)

{
	strcpy (pType, MP4_SYNCLAYERCONFIGDESCR_TYPE);


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: appendPropertiesDump (Buffer_p pbBuffer)

{

	long						lLevelIndex;


	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
	{
		if (pbBuffer -> append ("  ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// 0: Predefined
	if (_mui8pPredefined. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
	{
		if (pbBuffer -> append ("  ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// 1: UseAccessUnitStartFlag (1 bit) + UseAccessUnitEndFlag (1 bit) +
	// 		UseRandomAccessPointFlag (1 bit) +
	// 		HasRandomAccessUnitsOnlyFlag (1 bits) +
	// 		UsePaddingFlag (1 bits) +
	// 		UseTimeStampsFlag (1 bits) + UseIdleFlag (1 bits) +
	// 		DurationFlag (1 bits)
	if (_mui8pSyncLayerConfigDescrFlags1. appendDump (pbBuffer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
	{
		if (pbBuffer -> append ("  ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// 2: TimeStampResolution
	if (_mui32pTimeStampResolution. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
	{
		if (pbBuffer -> append ("  ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// 3: OCRResolution
	if (_mui32pOCRResolution. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
	{
		if (pbBuffer -> append ("  ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// 4: TimeStampLength
	if (_mui8pTimeStampLength. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
	{
		if (pbBuffer -> append ("  ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// 5: OCRLength
	if (_mui8pOCRLength. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
	{
		if (pbBuffer -> append ("  ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// 6: AULength
	if (_mui8pAULength. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
	{
		if (pbBuffer -> append ("  ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// 7: InstantBitrateLength
	if (_mui8pInstantBitrateLength. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
	{
		if (pbBuffer -> append ("  ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// 8: degradationPriortyLength (4 bits) + AUSeqNumLength (5 bits) +
	//		packetSeqNumLength (5 bits) + reserved (2 bits) +
	if (_mui16pSyncLayerConfigDescrFlags2. appendDump (pbBuffer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
	{
		if (pbBuffer -> append ("  ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// 9: TimeScale
	if (_mui32pTimeScale. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
	{
		if (pbBuffer -> append ("  ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// 10: AccessUnitDuration
	if (_mui16pAccessUnitDuration. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
	{
		if (pbBuffer -> append ("  ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// 11: CompositionUnitDuration
	if (_mui16pCompositionUnitDuration. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 12: StartDecodingTimeStamp
	// 13: StartCompositionTimeStamp
	switch (_ulTimeStampLength)
	{
		case 0:
			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			{
				if (pbBuffer -> append ("  ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (_mui64pStartDecodingTimeStamp. appendDump (pbBuffer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_APPENDDUMP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			{
				if (pbBuffer -> append ("  ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (_mui64pStartCompositionTimeStamp. appendDump (pbBuffer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_APPENDDUMP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 8:
			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			{
				if (pbBuffer -> append ("  ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (_mui8pStartDecodingTimeStamp. appendDump (pbBuffer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER8BITSPROPERTY_APPENDDUMP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			{
				if (pbBuffer -> append ("  ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (_mui8pStartCompositionTimeStamp. appendDump (pbBuffer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER8BITSPROPERTY_APPENDDUMP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 16:
			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			{
				if (pbBuffer -> append ("  ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (_mui16pStartDecodingTimeStamp. appendDump (pbBuffer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER16BITSPROPERTY_APPENDDUMP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			{
				if (pbBuffer -> append ("  ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (_mui16pStartCompositionTimeStamp. appendDump (pbBuffer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER16BITSPROPERTY_APPENDDUMP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 24:
			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			{
				if (pbBuffer -> append ("  ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (_mui24pStartDecodingTimeStamp. appendDump (pbBuffer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER24BITSPROPERTY_APPENDDUMP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			{
				if (pbBuffer -> append ("  ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (_mui24pStartCompositionTimeStamp. appendDump (pbBuffer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER24BITSPROPERTY_APPENDDUMP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 32:
			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			{
				if (pbBuffer -> append ("  ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (_mui32pStartDecodingTimeStamp. appendDump (pbBuffer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER32BITSPROPERTY_APPENDDUMP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			{
				if (pbBuffer -> append ("  ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (_mui32pStartCompositionTimeStamp. appendDump (pbBuffer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER32BITSPROPERTY_APPENDDUMP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 64:
			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			{
				if (pbBuffer -> append ("  ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (_mui64pStartDecodingTimeStamp. appendDump (pbBuffer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_APPENDDUMP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			{
				if (pbBuffer -> append ("  ") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			if (_mui64pStartCompositionTimeStamp. appendDump (pbBuffer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_APPENDDUMP_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		default:
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4SYNCLAYERCONFIGDESCR_WRONGTIMESTAMPLENGTH);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
	}


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: printPropertiesOnStdOutput (void)

{

	long						lLevelIndex;


	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	// 0: Predefined
	if (_mui8pPredefined. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	// 1: UseAccessUnitStartFlag (1 bit) + UseAccessUnitEndFlag (1 bit) +
	// 		UseRandomAccessPointFlag (1 bit) +
	// 		HasRandomAccessUnitsOnlyFlag (1 bits) +
	// 		UsePaddingFlag (1 bits) +
	// 		UseTimeStampsFlag (1 bits) + UseIdleFlag (1 bits) +
	// 		DurationFlag (1 bits)
	if (_mui8pSyncLayerConfigDescrFlags1. printOnStdOutput () !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	// 2: TimeStampResolution
	if (_mui32pTimeStampResolution. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	// 3: OCRResolution
	if (_mui32pOCRResolution. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	// 4: TimeStampLength
	if (_mui8pTimeStampLength. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	// 5: OCRLength
	if (_mui8pOCRLength. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	// 6: AULength
	if (_mui8pAULength. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	// 7: InstantBitrateLength
	if (_mui8pInstantBitrateLength. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	// 8: degradationPriortyLength (4 bits) + AUSeqNumLength (5 bits) +
	//		packetSeqNumLength (5 bits) + reserved (2 bits) +
	if (_mui16pSyncLayerConfigDescrFlags2. printOnStdOutput () !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	// 9: TimeScale
	if (_mui32pTimeScale. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	// 10: AccessUnitDuration
	if (_mui16pAccessUnitDuration. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	// 11: CompositionUnitDuration
	if (_mui16pCompositionUnitDuration. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 12: StartDecodingTimeStamp
	// 13: StartCompositionTimeStamp
	switch (_ulTimeStampLength)
	{
		case 0:
			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
				std:: cout << "  ";

			if (_mui64pStartDecodingTimeStamp. printOnStdOutput () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
				std:: cout << "  ";

			if (_mui64pStartCompositionTimeStamp. printOnStdOutput () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 8:
			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
				std:: cout << "  ";

			if (_mui8pStartDecodingTimeStamp. printOnStdOutput () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER8BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
				std:: cout << "  ";

			if (_mui8pStartCompositionTimeStamp. printOnStdOutput () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER8BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 16:
			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
				std:: cout << "  ";

			if (_mui16pStartDecodingTimeStamp. printOnStdOutput () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER16BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
				std:: cout << "  ";

			if (_mui16pStartCompositionTimeStamp. printOnStdOutput () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER16BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 24:
			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
				std:: cout << "  ";

			if (_mui24pStartDecodingTimeStamp. printOnStdOutput () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER24BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
				std:: cout << "  ";

			if (_mui24pStartCompositionTimeStamp. printOnStdOutput () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER24BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 32:
			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
				std:: cout << "  ";

			if (_mui32pStartDecodingTimeStamp. printOnStdOutput () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
				std:: cout << "  ";

			if (_mui32pStartCompositionTimeStamp. printOnStdOutput () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case 64:
			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
				std:: cout << "  ";

			if (_mui64pStartDecodingTimeStamp. printOnStdOutput () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
				std:: cout << "  ";

			if (_mui64pStartCompositionTimeStamp. printOnStdOutput () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UINTEGER64BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		default:
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4SYNCLAYERCONFIGDESCR_WRONGTIMESTAMPLENGTH);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
	}


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: getPredefined (
	unsigned char *pucPredefined)

{

	unsigned long						ulPredefined;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui8pPredefined. getValue (&ulPredefined, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	*pucPredefined				= (unsigned char) ulPredefined;

	if (_pmtMP4File -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: setPredefined (
	unsigned char ucPredefined)

{

	unsigned long						ulPredefined;
	unsigned long						ulSizeChangedInBytes;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	ulPredefined				= ucPredefined;

	if (_mui8pPredefined. setValue (ulPredefined, 0, &ulSizeChangedInBytes) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulSizeChangedInBytes > 0)
	{
		if (changeSizeManagement (ulSizeChangedInBytes) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_CHANGESIZEMANAGEMENT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (_pmtMP4File -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (_pmtMP4File -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: getDurationFlag (Boolean_p pbDurationFlag)

{

	unsigned long						ulFalgs;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui8pSyncLayerConfigDescrFlags1. getValue (&ulFalgs, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulFalgs & 0x0001)
		*pbDurationFlag				= true;
	else
		*pbDurationFlag				= false;

	if (_pmtMP4File -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: setDurationFlag (Boolean_t bDurationFlag)

{

	unsigned long						ulFalgs;
	unsigned long						ulSizeChangedInBytes;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui8pSyncLayerConfigDescrFlags1. getValue (&ulFalgs, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bDurationFlag)
		ulFalgs					|= 0x0001;
	else
		ulFalgs					&= 0xFFFE;

	if (_mui8pSyncLayerConfigDescrFlags1. setValue (ulFalgs, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulSizeChangedInBytes > 0)
	{
		if (changeSizeManagement (ulSizeChangedInBytes) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_CHANGESIZEMANAGEMENT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (_pmtMP4File -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (_pmtMP4File -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: getUseTimeStampsFlag (
	Boolean_p pbUseTimeStampsFlag)

{

	unsigned long						ulFalgs;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui8pSyncLayerConfigDescrFlags1. getValue (&ulFalgs, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulFalgs & 0x0020)
		*pbUseTimeStampsFlag				= true;
	else
		*pbUseTimeStampsFlag				= false;

	if (_pmtMP4File -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: setUseTimeStampsFlag (
	Boolean_t bUseTimeStampsFlag)

{

	unsigned long						ulFalgs;
	unsigned long						ulSizeChangedInBytes;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui8pSyncLayerConfigDescrFlags1. getValue (&ulFalgs, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bUseTimeStampsFlag)
		ulFalgs					|= 0x0020;
	else
		ulFalgs					&= 0xFFDF;

	if (_mui8pSyncLayerConfigDescrFlags1. setValue (ulFalgs, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulSizeChangedInBytes > 0)
	{
		if (changeSizeManagement (ulSizeChangedInBytes) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_CHANGESIZEMANAGEMENT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (_pmtMP4File -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (_pmtMP4File -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: getReserved (
	unsigned char *pucReserved)

{

	unsigned long						ulFalgs;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui16pSyncLayerConfigDescrFlags2. getValue (&ulFalgs, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	*pucReserved							= (unsigned char) (ulFalgs & 0x03);

	if (_pmtMP4File -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: setReserved (
	unsigned char ucReserved)

{

	unsigned long						ulFalgs;
	unsigned long						ulSizeChangedInBytes;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui16pSyncLayerConfigDescrFlags2. getValue (&ulFalgs, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	ulFalgs					|= ucReserved;

	if (_mui16pSyncLayerConfigDescrFlags2. setValue (ulFalgs, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulSizeChangedInBytes > 0)
	{
		if (changeSizeManagement (ulSizeChangedInBytes) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_CHANGESIZEMANAGEMENT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (_pmtMP4File -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (_pmtMP4File -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: getTimeStampResolution (
	unsigned long *pulTimeStampResolution)

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui32pTimeStampResolution. getValue (pulTimeStampResolution,
		0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_pmtMP4File -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: setTimeStampResolution (
	unsigned long ulTimeStampResolution)

{

	unsigned long						ulSizeChangedInBytes;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui32pTimeStampResolution. setValue (ulTimeStampResolution, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulSizeChangedInBytes > 0)
	{
		if (changeSizeManagement (ulSizeChangedInBytes) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_CHANGESIZEMANAGEMENT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (_pmtMP4File -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (_pmtMP4File -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: getTimeStampLength (
	unsigned char *pucTimeStampLength)

{

	unsigned long					ulTimeStampLength;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui8pTimeStampLength. getValue (&ulTimeStampLength,
		0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	*pucTimeStampLength				= (unsigned char) ulTimeStampLength;

	if (_pmtMP4File -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4SyncLayerConfigDescr:: setTimeStampLength (
	unsigned char ucTimeStampLength)

{

	unsigned long						ulTimeStampLength;
	unsigned long						ulSizeChangedInBytes;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	ulTimeStampLength				= ucTimeStampLength;

	if (_mui8pTimeStampLength. setValue (ulTimeStampLength, 0,
		&ulSizeChangedInBytes) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulSizeChangedInBytes > 0)
	{
		if (changeSizeManagement (ulSizeChangedInBytes) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_CHANGESIZEMANAGEMENT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (_pmtMP4File -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (_pmtMP4File -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}

