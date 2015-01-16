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


#include "MP4UnknownDescr.h"
#include "FileIO.h"
#ifdef WIN32
	#include <stdio.h>
#else
	#include <unistd.h>
#endif
#include <assert.h>



MP4UnknownDescr:: MP4UnknownDescr (void):
	MP4Descr ()

{

}


MP4UnknownDescr:: ~MP4UnknownDescr (void)

{

}



MP4UnknownDescr:: MP4UnknownDescr (
	const MP4UnknownDescr &)

{

	assert (1==0);

	// to do

}


MP4UnknownDescr &MP4UnknownDescr:: operator = (
	const MP4UnknownDescr &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4UnknownDescr:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile, __int64 ullAtomStartOffset,
		__int64 ullAtomSize, unsigned long ulHeaderSize,
		unsigned char ucNumBytesForDescriptorSize, unsigned long ulTag,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4UnknownDescr:: init (MP4Atom_p pmaParentAtom,
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


Error MP4UnknownDescr:: prepareChildrensAtomsInfo (void)

{


	return errNoError;
}


Error MP4UnknownDescr:: createProperties (
	unsigned long *pulPropertiesSize)

{

	Boolean_t					bIsImplicitProperty;


	bIsImplicitProperty				= false;


	// 0: Data
	/*
	{
		unsigned char				**pucData;


		if ((pucData = new unsigned char * [1]) == (unsigned char **) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if ((pucData [0] = new unsigned char [0]) == (unsigned char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucData;
			pucData			= (unsigned char **) NULL;

			return err;
		}

		memset (pucData [0], 0, 0);
		if (_mbpData. init ("Data", 1, &bIsImplicitProperty,
			0, pucData, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucData [0];
			pucData [0]			= (unsigned char *) NULL;

			delete [] pucData;
			pucData				= (unsigned char **) NULL;

			return err;
		}

		delete [] pucData [0];
		pucData [0]			= (unsigned char *) NULL;

		delete [] pucData;
		pucData				= (unsigned char **) NULL;
	}
	*/
	{
		unsigned char				*pucData [1];


		if (_mbpData. init ("Data", 1, &bIsImplicitProperty,
			0, pucData, _ptTracer) != errNoError)
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


Error MP4UnknownDescr:: readProperties (
	unsigned long *pulPropertiesSize)

{

	#ifdef WIN32
		__int64							ullDataBytes;
	#else
		unsigned long long				ullDataBytes;
	#endif
	char								pParentType [MP4_MAXTYPELENGTH];
	MP4Atom_p							pmaMP4ParentAtom;
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

	if (getParent (&pmaMP4ParentAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETPARENT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (pmaMP4ParentAtom -> getType (pParentType) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETPROPERTIESSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	/*
	 * commentato perche' non dovrebbe essere corretto.
	 * Qui _ullAtomSize rappresenta la size del descriptor, non di esds o iods
	 * quindi non si deve applicare la formula
	 * _ullAtomSize - (_bUse64Bits ? 16 : 8)
	if (!strcmp (pParentType, "esds") || !strcmp (pParentType, "iods"))
	{
		// if _bUse64Bits is true, the atom header should be:
		//      size        32 bits     (1)
		//      size        64 bits
		//      type        4 bytes
		// if _bUse64Bits is false, the atom header should be:
		//      size        32 bits
		//      type        4 bytes
		ullDataBytes				=
			(_ullAtomSize - (_bUse64Bits ? 16 : 8));
	}
	else
	*/
		ullDataBytes				=
			(_ullAtomSize - 1 - _ucNumBytesForDescriptorSize);

	// 0: Data
	if (_mbpData. init ("Data", 1, ullDataBytes, _pfFile,
		_ptTracer) != errNoError)
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


Error MP4UnknownDescr:: writeProperties (int iFileDescriptor)

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

	// 0: Data
	if (_mbpData. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4UnknownDescr:: appendPropertiesDump (Buffer_p pbBuffer)

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

	if (_mbpData. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4UnknownDescr:: printPropertiesOnStdOutput (void)

{

	long						lLevelIndex;


	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mbpData. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4UnknownDescr:: getPropertiesSize (
	unsigned long *pulPropertiesSize)

{

	unsigned long				ulSize;


	*pulPropertiesSize					= 0;

	if (_mbpData. getSize (&ulSize) != errNoError)
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


Error MP4UnknownDescr:: getType (char *pType)

{
	strcpy (pType, MP4_UNKNOWNDESCR_TYPE);


	return errNoError;
}

