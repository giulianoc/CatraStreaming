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


#include "MP4MdatAtom.h"
#include "MP4UuidAtom.h"
#include "FileIO.h"
#ifdef WIN32
	#include <stdio.h>
#else
	#include <unistd.h>
#endif
#include <assert.h>
#include <iostream>



MP4MdatAtom:: MP4MdatAtom (void): MP4Atom ()

{

}


MP4MdatAtom:: ~MP4MdatAtom (void)

{

}


MP4MdatAtom:: MP4MdatAtom (const MP4MdatAtom &)

{

	assert (1==0);

	// to do

}


MP4MdatAtom &MP4MdatAtom:: operator = (const MP4MdatAtom &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4MdatAtom:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile,
		__int64 ullAtomStartOffset, __int64 ullAtomSize,
		unsigned long ulHeaderSize,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4MdatAtom:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile,
		unsigned long long ullAtomStartOffset, unsigned long long ullAtomSize,
		unsigned long ulHeaderSize,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#endif

{

	_pucPreAllocatedMDatProperty	= (unsigned char *) NULL;
	_ulBlockMemoryAllocated			= 0;

	return MP4Atom:: init (pmaParentAtom, pfFile,
		ullAtomStartOffset, ullAtomSize, ulHeaderSize,
		pmtMP4File, lAtomLevel, bUse64Bits, bUseMP4ConsistencyCheck,
		sStandard, ptTracer);
}


#ifdef WIN32
	Error MP4MdatAtom:: init (MP4Atom *pmaParentAtom,
		FileReader_p pfFile,
		__int64 ullAtomStartOffset, PMutex_p pmtMP4File, long lAtomLevel,
		Boolean_t bUse64Bits, Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer,
		__int64 *pullAtomSize)
#else
	Error MP4MdatAtom:: init (MP4Atom *pmaParentAtom,
		FileReader_p pfFile,
		unsigned long long ullAtomStartOffset, PMutex_p pmtMP4File, long lAtomLevel,
		Boolean_t bUse64Bits, Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer,
		unsigned long long *pullAtomSize)
#endif

{

	_pucPreAllocatedMDatProperty	= (unsigned char *) NULL;
	_ulBlockMemoryAllocated			= 0;

	return MP4Atom:: init (pmaParentAtom,
		pfFile, ullAtomStartOffset, pmtMP4File, lAtomLevel,
		bUse64Bits, bUseMP4ConsistencyCheck,
		sStandard, ptTracer, pullAtomSize);
}


Error MP4MdatAtom:: finish (void)

{

	if (_ulBlockMemoryAllocated != 0)
	{
		delete [] _pucPreAllocatedMDatProperty;
		_pucPreAllocatedMDatProperty	=
			(unsigned char *) NULL;

		_ulBlockMemoryAllocated			= 0;
	}

	return MP4Atom:: finish ();
}


Error MP4MdatAtom:: prepareChildrensAtomsInfo (void)

{

	return errNoError;
}


Error MP4MdatAtom:: createProperties (
	unsigned long *pulPropertiesSize)

{

	Boolean_t					bIsImplicitProperty;


	bIsImplicitProperty				= false;


	// 0: Data
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

		_bMDatPropertyToBeReadFromFile		= false;
		_ullMDatPropertySize				= 0;
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


Error MP4MdatAtom:: readProperties (
	unsigned long *pulPropertiesSize)

{

	char								pParentType [MP4_MAXTYPELENGTH];
	MP4Atom_p							pmaMP4ParentAtom;
	#ifdef WIN32
		__int64							ullDataBytes;
	#else
		unsigned long long				ullDataBytes;
	#endif
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
	if (!strcmp (pParentType, "root"))
	{
		*pulPropertiesSize					= _ullAtomSize; 
	}
	else */ if (!strcmp (pParentType, "uuid"))
	{
		ullDataBytes					= _ullAtomSize -
			_ulHeaderSize - MP4F_EXTENDEDTYPELENGTH; 
	}
	else
	{
		ullDataBytes					= _ullAtomSize - _ulHeaderSize; 
	}

	// 0: Data
	/*
	if (_mbpData. init ("Data", 1, ullDataBytes, _pfFile,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*/
	if (_mbpData. init ("Data", 1, 0, _pfFile,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	_bMDatPropertyToBeReadFromFile		= true;
	_ullMDatPropertySize				= ullDataBytes;

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


Error MP4MdatAtom:: writeProperties (int iFileDescriptor)

{

	#ifdef WIN32
		__int64						ullAtomPropertiesStartOffset;
		__int64						llCurrentPosition;
		__int64						llBytesWritten;
		__int64						ullCharsRead;
	#else
		unsigned long long			ullAtomPropertiesStartOffset;
		long long					llCurrentPosition;
		long long					llBytesWritten;
		unsigned long long			ullCharsRead;
	#endif


	ullAtomPropertiesStartOffset			=
		_ullAtomStartOffset + _ulHeaderSize;

	if (FileIO:: seek (iFileDescriptor, ullAtomPropertiesStartOffset,
		SEEK_SET, &llCurrentPosition) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_SEEK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_bMDatPropertyToBeReadFromFile)
	{
		if (_ulBlockMemoryAllocated * MP4F_BLOCKMEMORYSIZE <
			_ullMDatPropertySize)
		{
			if (_ulBlockMemoryAllocated != 0)
			{
				delete [] _pucPreAllocatedMDatProperty;
				_pucPreAllocatedMDatProperty		=
					(unsigned char *) NULL;
			}

			do
			{
				_ulBlockMemoryAllocated++;
			}
			while (_ulBlockMemoryAllocated * MP4F_BLOCKMEMORYSIZE <
				_ullMDatPropertySize);

			if ((_pucPreAllocatedMDatProperty = new unsigned char [
				_ulBlockMemoryAllocated * MP4F_BLOCKMEMORYSIZE]) ==
					(unsigned char *) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				_ulBlockMemoryAllocated			= 0;

				return err;
			}
		}

		if (_pfFile -> seek (_ullAtomStartOffset + _ulHeaderSize,
			SEEK_SET, &llCurrentPosition) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_SEEK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pfFile -> readBytes (_pucPreAllocatedMDatProperty,
			_ullMDatPropertySize, true, &ullCharsRead) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_SEEK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (FileIO:: writeBytes (iFileDescriptor,
			_pucPreAllocatedMDatProperty,
			_ullMDatPropertySize, &llBytesWritten) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITEBYTES_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		if (_mbpData. write (iFileDescriptor) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error MP4MdatAtom:: appendPropertiesDump (Buffer_p pbBuffer)

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

	if (_bMDatPropertyToBeReadFromFile)
	{
		if (pbBuffer -> append ("Data") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbBuffer -> append (" = ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbBuffer -> append ("...") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbBuffer -> append (" <binary data: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbBuffer -> append (_ullMDatPropertySize) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbBuffer -> append (" byte(s)>") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbBuffer -> append ("\n") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		if (_mbpData. appendDump (pbBuffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_APPENDDUMP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error MP4MdatAtom:: printPropertiesOnStdOutput (void)

{

	long						lLevelIndex;


	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_bMDatPropertyToBeReadFromFile)
	{
		std:: cout << "Data" << " = ";

		std:: cout << "...";

		std:: cout << " <binary data: " << _ullMDatPropertySize << " byte(s)>";

		std:: cout << std:: endl;
		std:: cout. flush ();
	}
	else
	{
		if (_mbpData. printOnStdOutput () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_PRINTONSTDOUTPUT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error MP4MdatAtom:: getPropertiesSize (unsigned long *pulPropertiesSize)

{

	unsigned long				ulSize;


	*pulPropertiesSize					= 0;

	if (_bMDatPropertyToBeReadFromFile)
	{
		*pulPropertiesSize				+= _ullMDatPropertySize;
	}
	else
	{
		if (_mbpData. getSize (&ulSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulPropertiesSize					+= ulSize;
	}


	return errNoError;
}


Error MP4MdatAtom:: getType (char *pType)

{
	strcpy (pType, "mdat");


	return errNoError;
}


#ifdef WIN32
	Error MP4MdatAtom:: getData (__int64 ullFileOffset,
		unsigned long ulBytesToRead, unsigned char *pucBffer)
#else
	Error MP4MdatAtom:: getData (unsigned long long ullFileOffset,
		unsigned long ulBytesToRead, unsigned char *pucBffer)
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

	if (ullFileOffset < _ullAtomStartOffset + _ulHeaderSize ||
		ullFileOffset + ulBytesToRead > _ullAtomStartOffset + _ullAtomSize)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		// _ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
		// 	__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bMDatPropertyToBeReadFromFile)
	{
		#ifdef WIN32
			__int64						llCurrentPosition;
			__int64						ullCharsRead;
		#else
			long long					llCurrentPosition;
			unsigned long long			ullCharsRead;
		#endif

		if (_pfFile -> seek (ullFileOffset,
			SEEK_SET, &llCurrentPosition) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_SEEK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pmtMP4File -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_pfFile -> readBytes (pucBffer,
			ulBytesToRead, true, &ullCharsRead) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_SEEK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

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
	else
	{
		if (_mbpData. getData (
			ullFileOffset - (_ullAtomStartOffset + _ulHeaderSize),
			ulBytesToRead, 0, pucBffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_GETDATA_FAILED);
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


#ifdef WIN32
	Error MP4MdatAtom:: getPointerToData (__int64 ullFileOffset,
		unsigned char **pucBffer, __int64 *pullBytesToRead)
#else
	Error MP4MdatAtom:: getPointerToData (unsigned long long ullFileOffset,
		unsigned char **pucBffer, unsigned long long *pullBytesToRead)
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

	if (_bMDatPropertyToBeReadFromFile)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
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

	if (ullFileOffset < _ullAtomStartOffset + _ulHeaderSize ||
		ullFileOffset > _ullAtomStartOffset + _ullAtomSize)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		// _ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
		// 	__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mbpData. getPointerToData (
		ullFileOffset - (_ullAtomStartOffset + _ulHeaderSize),
		0, pucBffer, pullBytesToRead) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_GETPOINTERTODATA_FAILED);
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


/*
#ifdef WIN32
	Error MP4MdatAtom:: appendDataToRTPPacket (
		__int64 ullFileOffset,
		__int64 llBytesToRead, RTPPacket_p prpRTPPacket)
#else
	Error MP4MdatAtom:: appendDataToRTPPacket (
		unsigned long long ullFileOffset,
		long long llBytesToRead, RTPPacket_p prpRTPPacket)
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

	if (ullFileOffset < _ullAtomStartOffset + _ulHeaderSize ||
		ullFileOffset + llBytesToRead > _ullAtomStartOffset + _ullAtomSize)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		// _ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
		// 	__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bMDatPropertyToBeReadFromFile)
	{
		#ifdef WIN32
			__int64						llCurrentPosition;
		#else
			long long					llCurrentPosition;
		#endif


		if (_ulBlockMemoryAllocated * MP4F_BLOCKMEMORYSIZE <
			llBytesToRead)
		{
			if (_ulBlockMemoryAllocated != 0)
			{
				delete [] _pucPreAllocatedMDatProperty;
				_pucPreAllocatedMDatProperty		=
					(unsigned char *) NULL;
			}

			do
			{
				_ulBlockMemoryAllocated++;
			}
			while (_ulBlockMemoryAllocated * MP4F_BLOCKMEMORYSIZE <
				llBytesToRead);

			if ((_pucPreAllocatedMDatProperty = new unsigned char [
				_ulBlockMemoryAllocated * MP4F_BLOCKMEMORYSIZE]) ==
					(unsigned char *) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				_ulBlockMemoryAllocated			= 0;

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

		if (_pfFile -> seek (ullFileOffset,
			SEEK_SET, &llCurrentPosition) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_SEEK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pmtMP4File -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_pfFile -> readBytes (_pucPreAllocatedMDatProperty,
			llBytesToRead) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_SEEK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pmtMP4File -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (prpRTPPacket -> appendData (
			_pucPreAllocatedMDatProperty, llBytesToRead) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_RTPPACKET_APPENDDATA_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

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
	else
	{
		if (_mbpData. appendDataToRTPPacket (
			ullFileOffset - (_ullAtomStartOffset + _ulHeaderSize),
			llBytesToRead, 0, prpRTPPacket) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_APPENDDATATORTPPACKET_FAILED);
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
*/


#ifdef WIN32
	Error MP4MdatAtom:: getPointerToData (
		__int64 ullFileOffset, __int64 ullBytesToRead,
		unsigned char **pucBffer)
#else
	Error MP4MdatAtom:: getPointerToData (
		unsigned long long ullFileOffset, unsigned long long ullBytesToRead,
		unsigned char **pucBffer)
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

	if (ullFileOffset < _ullAtomStartOffset + _ulHeaderSize ||
		ullFileOffset + ullBytesToRead > _ullAtomStartOffset + _ullAtomSize)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		// _ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
		// 	__FILE__, __LINE__);

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bMDatPropertyToBeReadFromFile)
	{
		#ifdef WIN32
			__int64						llCurrentPosition;
			__int64						ullCharsRead;
		#else
			long long					llCurrentPosition;
			unsigned long long			ullCharsRead;
		#endif


		if (_ulBlockMemoryAllocated * MP4F_BLOCKMEMORYSIZE <
			ullBytesToRead)
		{
			if (_ulBlockMemoryAllocated != 0)
			{
				delete [] _pucPreAllocatedMDatProperty;
				_pucPreAllocatedMDatProperty		=
					(unsigned char *) NULL;
			}

			do
			{
				_ulBlockMemoryAllocated++;
			}
			while (_ulBlockMemoryAllocated * MP4F_BLOCKMEMORYSIZE <
				ullBytesToRead);

			if ((_pucPreAllocatedMDatProperty = new unsigned char [
				_ulBlockMemoryAllocated * MP4F_BLOCKMEMORYSIZE]) ==
					(unsigned char *) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				_ulBlockMemoryAllocated			= 0;

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

		if (_pfFile -> seek (ullFileOffset,
			SEEK_SET, &llCurrentPosition) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_SEEK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pmtMP4File -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_pfFile -> readBytes (_pucPreAllocatedMDatProperty,
			ullBytesToRead, true, &ullCharsRead) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_SEEK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pmtMP4File -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		*pucBffer			= _pucPreAllocatedMDatProperty;
	}
	else
	{
		#ifdef WIN32
			__int64						ullLocalBytesToRead;
		#else
			unsigned long long			ullLocalBytesToRead;
		#endif


		if (_mbpData. getPointerToData (
			ullFileOffset - (_ullAtomStartOffset + _ulHeaderSize),
			0, pucBffer, &ullLocalBytesToRead) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_GETPOINTERTODATA_FAILED);
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

		if (ullLocalBytesToRead < ullBytesToRead)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4MDATATOM_BYTESTOREADTOOBIG);
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


Error MP4MdatAtom:: appendData (unsigned char *pucData,
	unsigned long ulBytesNumber)

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

	if (_bMDatPropertyToBeReadFromFile)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
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

	if (_mbpData. appendData (pucData, ulBytesNumber, 0,
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


#ifdef WIN32
	Error MP4MdatAtom:: getDataStartOffset (__int64 *pullDataStartOffset)
#else
	Error MP4MdatAtom:: getDataStartOffset (unsigned long long *pullDataStartOffset)
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

	*pullDataStartOffset		= _ullAtomStartOffset + _ulHeaderSize;

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
