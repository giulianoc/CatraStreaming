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


#include "MP4UdtaAtom.h"
#include "FileIO.h"
#include <sys/types.h>
#ifdef WIN32
	#include <stdio.h>
#else
	#include <unistd.h>
#endif
#include <assert.h>



MP4UdtaAtom:: MP4UdtaAtom (void): MP4Atom ()

{

}


MP4UdtaAtom:: ~MP4UdtaAtom (void)

{

}



MP4UdtaAtom:: MP4UdtaAtom (const MP4UdtaAtom &)

{

	assert (1==0);

	// to do

}


MP4UdtaAtom &MP4UdtaAtom:: operator = (const MP4UdtaAtom &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4UdtaAtom:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile,
		__int64 ullAtomStartOffset, __int64 ullAtomSize,
		unsigned long ulHeaderSize,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4UdtaAtom:: init (MP4Atom_p pmaParentAtom,
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


Error MP4UdtaAtom:: prepareChildrensAtomsInfo (void)

{

	ChildrenAtomInfo_t					ctiChildrenAtomInfo;
	char								pType [MP4_MAXTYPELENGTH];


	if (_pmaParentAtom -> getType (pType) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (ctiChildrenAtomInfo. _pType, "cprt");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= false;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "hnti");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	if (!strcmp (pType, "trak"))
	{
		strcpy (ctiChildrenAtomInfo. _pType, "hinf");
		ctiChildrenAtomInfo. _bMandatory		= false;
		ctiChildrenAtomInfo. _bOnlyOne			= true;

		_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
			ctiChildrenAtomInfo);
	}
	else
	{
		strcpy (ctiChildrenAtomInfo. _pType, "wmfs");
		ctiChildrenAtomInfo. _bMandatory		= false;
		ctiChildrenAtomInfo. _bOnlyOne			= true;

		_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
			ctiChildrenAtomInfo);
	}


	return errNoError;
}


Error MP4UdtaAtom:: createProperties (
	unsigned long *pulPropertiesSize)

{

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


Error MP4UdtaAtom:: readProperties (
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


Error MP4UdtaAtom:: writeProperties (int iFileDescriptor)

{

/*
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
*/


	return errNoError;
}


Error MP4UdtaAtom:: getPropertiesSize (unsigned long *pulPropertiesSize)

{

	*pulPropertiesSize					= 0;


	return errNoError;
}


Error MP4UdtaAtom:: appendPropertiesDump (Buffer_p pbBuffer)

{


	return errNoError;
}


Error MP4UdtaAtom:: printPropertiesOnStdOutput (void)

{


	return errNoError;
}


Error MP4UdtaAtom:: getType (char *pType)

{
	strcpy (pType, "udta");


	return errNoError;
}


