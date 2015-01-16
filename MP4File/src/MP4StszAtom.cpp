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


#include "MP4StszAtom.h"
#include "FileIO.h"
#ifdef WIN32
	#include <stdio.h>
#else
	#include <unistd.h>
#endif
#include <assert.h>



MP4StszAtom:: MP4StszAtom (void): MP4Atom ()

{

}


MP4StszAtom:: ~MP4StszAtom (void)

{

}



MP4StszAtom:: MP4StszAtom (const MP4StszAtom &)

{

	assert (1==0);

	// to do

}


MP4StszAtom &MP4StszAtom:: operator = (const MP4StszAtom &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4StszAtom:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile,
		__int64 ullAtomStartOffset, __int64 ullAtomSize,
		unsigned long ulHeaderSize,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4StszAtom:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile,
		unsigned long long ullAtomStartOffset, unsigned long long ullAtomSize,
		unsigned long ulHeaderSize,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#endif

{

	return MP4Atom:: init (pmaParentAtom, pfFile,
		ullAtomStartOffset, ullAtomSize, ulHeaderSize,
		pmtMP4File, lAtomLevel, bUse64Bits, bUseMP4ConsistencyCheck,
		sStandard, ptTracer);
}


Error MP4StszAtom:: prepareChildrensAtomsInfo (void)

{

	return errNoError;
}


Error MP4StszAtom:: createProperties (
	unsigned long *pulPropertiesSize)

{

	unsigned long				ulValue;
	Boolean_t					bIsImplicitProperty;


	ulValue							= 0;
	bIsImplicitProperty				= false;

	// 0: Version
	if (_mui8pVersion. init ("Version", 1, &bIsImplicitProperty,
		&ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 1: Flags
	if (_mui24pFlags. init ("Flags", 1, &bIsImplicitProperty, &ulValue,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER24BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 2: CommonSampleSize
	if (_mui32pCommonSampleSize. init ("CommonSampleSize",
		1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 3: SamplesNumber
	if (_mui32pSamplesNumber. init ("SamplesNumber",
		1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 4: SampleSizeTable
	// 		SampleSize
	if (_mui32pSampleSizeTable. init ("SampleSizeTable",
		ulValue * 1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
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


Error MP4StszAtom:: readProperties (
	unsigned long *pulPropertiesSize)

{

	unsigned long				ulSamplesNumber;
	unsigned long				ulCommonSampleSize;
	unsigned long				ulValue;
	Boolean_t					bIsImplicitProperty;
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

	// 0: Version
	if (_mui8pVersion. init ("Version", 1, _pfFile,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 1: Flags
	if (_mui24pFlags. init ("Flags", 1, _pfFile,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER24BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 2: CommonSampleSize
	if (_mui32pCommonSampleSize. init ("CommonSampleSize",
		1, _pfFile, _ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui32pCommonSampleSize. getValue (&ulCommonSampleSize, 0) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 3: SamplesNumber
	if (_mui32pSamplesNumber. init ("SamplesNumber",
		1, _pfFile, _ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (ulCommonSampleSize == 0)
	{
		if (_mui32pSamplesNumber. getValue (&ulSamplesNumber, 0) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 4: SampleSizeTable
		// 		SampleSize
		if (_mui32pSampleSizeTable. init ("SampleSizeTable",
			ulSamplesNumber * 1, _pfFile, _ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		// 4: SampleSizeTable
		// 		SampleSize

		ulValue					= 0;
		bIsImplicitProperty		= false;

		if (_mui32pSampleSizeTable. init ("SampleSizeTable",
			ulValue * 1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
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


Error MP4StszAtom:: writeProperties (int iFileDescriptor)

{

	#ifdef WIN32
		__int64							ullAtomPropertiesStartOffset;
		__int64							llCurrentPosition;
	#else
		unsigned long long				ullAtomPropertiesStartOffset;
		long long						llCurrentPosition;
	#endif


	ullAtomPropertiesStartOffset			= _ullAtomStartOffset + _ulHeaderSize;

	if (FileIO:: seek (iFileDescriptor, ullAtomPropertiesStartOffset,
		SEEK_SET, &llCurrentPosition) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_SEEK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui8pVersion. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui24pFlags. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER24BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui32pCommonSampleSize. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui32pSamplesNumber. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui32pSampleSizeTable. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4StszAtom:: getPropertiesSize (unsigned long *pulPropertiesSize)

{

	unsigned long				ulSize;


	*pulPropertiesSize					= 0;

	if (_mui8pVersion. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mui24pFlags. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER24BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mui32pCommonSampleSize. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mui32pSamplesNumber. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mui32pSampleSizeTable. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;


	return errNoError;
}


Error MP4StszAtom:: getType (char *pType)

{
	strcpy (pType, "stsz");


	return errNoError;
}


Error MP4StszAtom:: appendPropertiesDump (Buffer_p pbBuffer)

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

	if (_mui8pVersion. appendDump (pbBuffer) != errNoError)
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

	if (_mui24pFlags. appendDump (pbBuffer) != errNoError)
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

	if (_mui32pCommonSampleSize. appendDump (pbBuffer) != errNoError)
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

	if (_mui32pSamplesNumber. appendDump (pbBuffer) != errNoError)
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

	if (_mui32pSampleSizeTable. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4StszAtom:: printPropertiesOnStdOutput (void)

{

	long						lLevelIndex;


	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mui8pVersion. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mui24pFlags. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER24BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mui32pCommonSampleSize. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mui32pSamplesNumber. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mui32pSampleSizeTable. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4StszAtom:: getSamplesNumber (unsigned long *pulSamplesNumber)

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui32pSamplesNumber. getValue (pulSamplesNumber, 0) != errNoError)
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


Error MP4StszAtom:: setSamplesNumber (unsigned long ulSamplesNumber)

{

	unsigned long					ulSizeChangedInBytes;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui32pSamplesNumber. setValue (ulSamplesNumber, 0,
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


Error MP4StszAtom:: incrementSamplesNumber (long llIncrement)

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui32pSamplesNumber. incrementValue (0, llIncrement) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
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


Error MP4StszAtom:: getCommonSampleSize (unsigned long *pulCommonSampleSize)

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui32pCommonSampleSize. getValue (pulCommonSampleSize, 0) !=
		errNoError)
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


Error MP4StszAtom:: setCommonSampleSize (unsigned long ulCommonSampleSize)

{

	unsigned long					ulSizeChangedInBytes;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui32pCommonSampleSize. setValue (ulCommonSampleSize, 0,
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


Error MP4StszAtom:: getSampleSizeTable (unsigned long ulIndex,
	unsigned long *pulSampleSizeTable)

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui32pSampleSizeTable. getValue (pulSampleSizeTable, ulIndex) !=
		errNoError)
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


Error MP4StszAtom:: setSampleSizeTable (unsigned long ulIndex,
	unsigned long ulSampleSizeTable)

{

	unsigned long					ulSizeChangedInBytes;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui32pSampleSizeTable. setValue (ulSampleSizeTable, ulIndex,
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

