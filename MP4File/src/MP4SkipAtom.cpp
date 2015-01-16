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


#include "MP4SkipAtom.h"
#include "MP4UuidAtom.h"
#include "FileIO.h"
#ifdef WIN32
	#include <stdio.h>
#else
	#include <unistd.h>
#endif
#include <assert.h>



MP4SkipAtom:: MP4SkipAtom (void): MP4Atom ()

{

}


MP4SkipAtom:: ~MP4SkipAtom (void)

{

}



MP4SkipAtom:: MP4SkipAtom (const MP4SkipAtom &)

{

	assert (1==0);

	// to do

}


MP4SkipAtom &MP4SkipAtom:: operator = (const MP4SkipAtom &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4SkipAtom:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile,
		__int64 ullAtomStartOffset, __int64 ullAtomSize,
		unsigned long ulHeaderSize,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4SkipAtom:: init (MP4Atom_p pmaParentAtom,
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


Error MP4SkipAtom:: prepareChildrensAtomsInfo (void)

{

	return errNoError;
}


Error MP4SkipAtom:: createProperties (
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


Error MP4SkipAtom:: readProperties (
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


Error MP4SkipAtom:: writeProperties (int iFileDescriptor)

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

	if (_bMDatPropertyToBeReadFromFile)
	{
		unsigned char			*pucMDatProperty;
		#ifdef WIN32
			__int64						llBytesWritten;
			__int64						ullCharsRead;
		#else
			long long					llBytesWritten;
			unsigned long long			ullCharsRead;
		#endif


		if ((pucMDatProperty = new unsigned char [
			_ullMDatPropertySize]) == (unsigned char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pfFile -> seek (_ullAtomStartOffset + _ulHeaderSize,
			SEEK_SET, &llCurrentPosition) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_SEEK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] pucMDatProperty;
			pucMDatProperty			= (unsigned char *) NULL;

			return err;
		}

		if (_pfFile -> readBytes (pucMDatProperty,
			_ullMDatPropertySize, true, &ullCharsRead) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_SEEK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] pucMDatProperty;
			pucMDatProperty			= (unsigned char *) NULL;

			return err;
		}

		if (FileIO:: writeBytes (iFileDescriptor, pucMDatProperty,
			_ullMDatPropertySize, &llBytesWritten) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITEBYTES_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] pucMDatProperty;
			pucMDatProperty			= (unsigned char *) NULL;

			return err;
		}

		delete [] pucMDatProperty;
		pucMDatProperty			= (unsigned char *) NULL;
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


Error MP4SkipAtom:: appendPropertiesDump (Buffer_p pbBuffer)

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


Error MP4SkipAtom:: printPropertiesOnStdOutput (void)

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


Error MP4SkipAtom:: getPropertiesSize (unsigned long *pulPropertiesSize)

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


Error MP4SkipAtom:: getType (char *pType)

{
	strcpy (pType, "skip");


	return errNoError;
}

