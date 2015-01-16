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


#include "MP4SamrAtom.h"
#include "FileIO.h"
#ifdef WIN32
	#include <stdio.h>
#else
	#include <unistd.h>
#endif
#include <assert.h>



MP4SamrAtom:: MP4SamrAtom (void): MP4Atom ()

{

}


MP4SamrAtom:: ~MP4SamrAtom (void)

{

}



MP4SamrAtom:: MP4SamrAtom (const MP4SamrAtom &)

{

	assert (1==0);

	// to do

}


MP4SamrAtom &MP4SamrAtom:: operator = (const MP4SamrAtom &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4SamrAtom:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile,
		__int64 ullAtomStartOffset, __int64 ullAtomSize,
		unsigned long ulHeaderSize,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4SamrAtom:: init (MP4Atom_p pmaParentAtom,
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


Error MP4SamrAtom:: prepareChildrensAtomsInfo (void)

{

	ChildrenAtomInfo_t					ctiChildrenAtomInfo;


	strcpy (ctiChildrenAtomInfo. _pType, "damr");
	ctiChildrenAtomInfo. _bMandatory        = true;
	ctiChildrenAtomInfo. _bOnlyOne          = true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);


	return errNoError;
}


Error MP4SamrAtom:: createProperties (
	unsigned long *pulPropertiesSize)

{

	unsigned long				ulValue;
	Boolean_t					bIsImplicitProperty;


	ulValue							= 0;
	bIsImplicitProperty				= false;

	// 0: Reserved1
	/*
	{
		unsigned char				**pucReserved1;


		if ((pucReserved1 = new unsigned char * [1]) == (unsigned char **) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if ((pucReserved1 [0] = new unsigned char [6]) ==
			(unsigned char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucReserved1;
			pucReserved1			= (unsigned char **) NULL;

			return err;
		}

		memset (pucReserved1 [0], 0, 6);
		if (_mbpReserved1. init ("Reserved1", 1, &bIsImplicitProperty,
			6, pucReserved1, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucReserved1 [0];
			pucReserved1 [0]			= (unsigned char *) NULL;

			delete [] pucReserved1;
			pucReserved1				= (unsigned char **) NULL;

			return err;
		}

		delete [] pucReserved1 [0];
		pucReserved1 [0]			= (unsigned char *) NULL;

		delete [] pucReserved1;
		pucReserved1				= (unsigned char **) NULL;
	}
	*/
	{
		unsigned char				*pucReserved1 [1]	=
			{ (unsigned char *) "\0\0\0\0\0\0" };


		if (_mbpReserved1. init ("Reserved1", 1, &bIsImplicitProperty,
			6, pucReserved1, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	// 1: DataReferenceIndex
	if (_mui16pDataReferenceIndex. init ("DataReferenceIndex", 1,
		&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 2: Reserved2
	/*
	{
		unsigned char				**pucReserved2;


		if ((pucReserved2 = new unsigned char * [1]) == (unsigned char **) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if ((pucReserved2 [0] = new unsigned char [16]) ==
			(unsigned char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucReserved2;
			pucReserved2			= (unsigned char **) NULL;

			return err;
		}

		memset (pucReserved2 [0], 0, 16);
		if (_mbpReserved2. init ("Reserved2", 1, &bIsImplicitProperty,
			16, pucReserved2, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucReserved2 [0];
			pucReserved2 [0]			= (unsigned char *) NULL;

			delete [] pucReserved2;
			pucReserved2				= (unsigned char **) NULL;

			return err;
		}

		delete [] pucReserved2 [0];
		pucReserved2 [0]			= (unsigned char *) NULL;

		delete [] pucReserved2;
		pucReserved2				= (unsigned char **) NULL;
	}
	*/
	{
		unsigned char				*pucReserved2 [1]	=
			{ (unsigned char *) "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" };


		if (_mbpReserved2. init ("Reserved2", 1, &bIsImplicitProperty,
			16, pucReserved2, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	// 3: TimeScale
	if (_mui16pTimeScale. init ("TimeScale", 1, &bIsImplicitProperty,
		&ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 4: Reserved3
	/*
	{
		unsigned char				**pucReserved3;


		if ((pucReserved3 = new unsigned char * [1]) == (unsigned char **) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if ((pucReserved3 [0] = new unsigned char [2]) ==
			(unsigned char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucReserved3;
			pucReserved3			= (unsigned char **) NULL;

			return err;
		}

		memset (pucReserved3 [0], 0, 2);
		if (_mbpReserved3. init ("Reserved3", 1, &bIsImplicitProperty,
			2, pucReserved3, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucReserved3 [0];
			pucReserved3 [0]			= (unsigned char *) NULL;

			delete [] pucReserved3;
			pucReserved3				= (unsigned char **) NULL;

			return err;
		}

		delete [] pucReserved3 [0];
		pucReserved3 [0]			= (unsigned char *) NULL;

		delete [] pucReserved3;
		pucReserved3				= (unsigned char **) NULL;
	}
	*/
	{
		unsigned char				*pucReserved3 [1]	=
			{ (unsigned char *) "\0\0" };


		if (_mbpReserved3. init ("Reserved3", 1, &bIsImplicitProperty,
			2, pucReserved3, _ptTracer) != errNoError)
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


Error MP4SamrAtom:: readProperties (
	unsigned long *pulPropertiesSize)

{
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

	// 0: Reserved1
	if (_mbpReserved1. init ("Reserved1", 1, 6, _pfFile,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 1: DataReferenceIndex
	if (_mui16pDataReferenceIndex. init ("DataReferenceIndex", 1,
		_pfFile, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 2: Reserved2
	if (_mbpReserved2. init ("Reserved2", 1, 16, _pfFile,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 3: TimeScale
	if (_mui16pTimeScale. init ("TimeScale", 1, _pfFile,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 4: Reserved3
	if (_mbpReserved3. init ("Reserved3", 1,
		2, _pfFile, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_INIT_FAILED);
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


Error MP4SamrAtom:: writeProperties (int iFileDescriptor)

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

	if (_mbpReserved1. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui16pDataReferenceIndex. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mbpReserved2. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui16pTimeScale. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mbpReserved3. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4SamrAtom:: getPropertiesSize (unsigned long *pulPropertiesSize)

{

	unsigned long				ulSize;


	*pulPropertiesSize					= 0;

	if (_mbpReserved1. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mui16pDataReferenceIndex. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mbpReserved2. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mui16pTimeScale. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mbpReserved3. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;


	return errNoError;
}


Error MP4SamrAtom:: getType (char *pType)

{
	strcpy (pType, "samr");


	return errNoError;
}


Error MP4SamrAtom:: appendPropertiesDump (Buffer_p pbBuffer)

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

	if (_mbpReserved1. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_APPENDDUMP_FAILED);
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

	if (_mui16pDataReferenceIndex. appendDump (pbBuffer) != errNoError)
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

	if (_mbpReserved2. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_APPENDDUMP_FAILED);
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

	if (_mui16pTimeScale. appendDump (pbBuffer) != errNoError)
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

	if (_mbpReserved3. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4SamrAtom:: printPropertiesOnStdOutput (void)

{

	long						lLevelIndex;


	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mbpReserved1. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mui16pDataReferenceIndex. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mbpReserved2. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mui16pTimeScale. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mbpReserved3. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


