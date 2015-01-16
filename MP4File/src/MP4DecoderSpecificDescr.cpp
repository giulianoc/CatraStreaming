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


#include "MP4DecoderSpecificDescr.h"
#include "FileIO.h"
#ifdef WIN32
	#include <stdio.h>
#else
	#include <unistd.h>
#endif
#include <assert.h>



MP4DecoderSpecificDescr:: MP4DecoderSpecificDescr (void):
	MP4Descr ()

{

}


MP4DecoderSpecificDescr:: ~MP4DecoderSpecificDescr (void)

{

}



MP4DecoderSpecificDescr:: MP4DecoderSpecificDescr (
	const MP4DecoderSpecificDescr &)

{

	assert (1==0);

	// to do

}


MP4DecoderSpecificDescr &MP4DecoderSpecificDescr:: operator = (
	const MP4DecoderSpecificDescr &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4DecoderSpecificDescr:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile, __int64 ullAtomStartOffset,
		__int64 ullAtomSize, unsigned long ulHeaderSize,
		unsigned char ucNumBytesForDescriptorSize, unsigned long ulTag,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4DecoderSpecificDescr:: init (MP4Atom_p pmaParentAtom,
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


Error MP4DecoderSpecificDescr:: prepareChildrensAtomsInfo (void)

{

	return errNoError;
}


Error MP4DecoderSpecificDescr:: createProperties (
	unsigned long *pulPropertiesSize)

{

	Boolean_t					bIsImplicitProperty;


	bIsImplicitProperty				= false;

	/*
	{
		unsigned char				**pucInfo;


		if ((pucInfo = new unsigned char * [1]) ==
			(unsigned char **) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if ((pucInfo [0] = new unsigned char [0]) ==
			(unsigned char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucInfo;
			pucInfo						= (unsigned char **) NULL;

			return err;
		}

		memset (pucInfo [0], 0, 0);
		if (_mbpInfo. init ("Info", 1,
			&bIsImplicitProperty, 0, pucInfo, _ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucInfo [0];
			pucInfo [0]					= (unsigned char *) NULL;

			delete [] pucInfo;
			pucInfo						= (unsigned char **) NULL;

			return err;
		}

		delete [] pucInfo [0];
		pucInfo [0]						= (unsigned char *) NULL;

		delete [] pucInfo;
		pucInfo							= (unsigned char **) NULL;
	}
	*/
	{
		unsigned char				*pucInfo [1];


		if (_mbpInfo. init ("Info", 1,
			&bIsImplicitProperty, 0, pucInfo, _ptTracer) !=
			errNoError)
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


Error MP4DecoderSpecificDescr:: readProperties (
	unsigned long *pulPropertiesSize)

{

	char							pParentType [MP4_MAXTYPELENGTH];
	MP4Atom_p						pmaMP4ParentAtom;
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
		if (_mbpInfo. init ("Info", 1,
			_ullAtomSize - (_bUse64Bits ? 16 : 8),
			_iFileDescriptor, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
	*/
		if (_mbpInfo. init ("Info", 1,
			_ullAtomSize - (1 + _ucNumBytesForDescriptorSize),
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	// }

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


Error MP4DecoderSpecificDescr:: writeProperties (int iFileDescriptor)

{

	#ifdef WIN32
		__int64						ullAtomPropertiesStartOffset;
		__int64						llCurrentPosition;
	#else
		unsigned long long			ullAtomPropertiesStartOffset;
		long long					llCurrentPosition;
	#endif


	ullAtomPropertiesStartOffset			= _ullAtomStartOffset +
		1 +_ucNumBytesForDescriptorSize;

	if (FileIO:: seek (iFileDescriptor, ullAtomPropertiesStartOffset,
		SEEK_SET, &llCurrentPosition) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_SEEK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mbpInfo. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4DecoderSpecificDescr:: getPropertiesSize (
	unsigned long *pulPropertiesSize)

{

	unsigned long					ulSize;


	*pulPropertiesSize				= 0;

	if (_mbpInfo. getSize (&ulSize) != errNoError)
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


Error MP4DecoderSpecificDescr:: getType (char *pType)

{
	strcpy (pType, MP4_DECODERSPECIFICDESCR_TYPE);


	return errNoError;
}


Error MP4DecoderSpecificDescr:: appendPropertiesDump (Buffer_p pbBuffer)

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

	if (_mbpInfo. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4DecoderSpecificDescr:: printPropertiesOnStdOutput (void)

{

	long						lLevelIndex;


	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mbpInfo. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error MP4DecoderSpecificDescr:: getPointerToInfo (unsigned char **pucInfo,
		__int64 *pullInfoSize)
#else
	Error MP4DecoderSpecificDescr:: getPointerToInfo (unsigned char **pucInfo,
		unsigned long long *pullInfoSize)
#endif

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mbpInfo. getPointerToData (0, 0, pucInfo, pullInfoSize) != errNoError)
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


Error MP4DecoderSpecificDescr:: setInfo (unsigned char *pucInfo,
	unsigned long ulInfoSize)

{

	long					lSizeChangedInBytes;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mbpInfo. setData (pucInfo, ulInfoSize, 0, &lSizeChangedInBytes) !=
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

	if (lSizeChangedInBytes != 0)
	{
		if (changeSizeManagement (lSizeChangedInBytes) != errNoError)
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

