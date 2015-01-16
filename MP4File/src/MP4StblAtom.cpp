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


#include "MP4StblAtom.h"
#include "FileIO.h"
#include <sys/types.h>
#ifdef WIN32
	#include <stdio.h>
#else
	#include <unistd.h>
#endif
#include <assert.h>



MP4StblAtom:: MP4StblAtom (void): MP4Atom ()

{

}


MP4StblAtom:: ~MP4StblAtom (void)

{

}



MP4StblAtom:: MP4StblAtom (const MP4StblAtom &)

{

	assert (1==0);

	// to do

}


MP4StblAtom &MP4StblAtom:: operator = (const MP4StblAtom &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4StblAtom:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile,
		__int64 ullAtomStartOffset, __int64 ullAtomSize,
		unsigned long ulHeaderSize,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4StblAtom:: init (MP4Atom_p pmaParentAtom,
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


Error MP4StblAtom:: prepareChildrensAtomsInfo (void)

{

	ChildrenAtomInfo_t					ctiChildrenAtomInfo;


	strcpy (ctiChildrenAtomInfo. _pType, "stsd");
	ctiChildrenAtomInfo. _bMandatory		= true;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "stts");
	ctiChildrenAtomInfo. _bMandatory		= true;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "ctts");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "stsz");
	ctiChildrenAtomInfo. _bMandatory		= true;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "stsc");
	ctiChildrenAtomInfo. _bMandatory		= true;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "stco");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "co64");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "stss");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "stsh");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "stdp");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);


	return errNoError;
}


#ifdef WIN32
	Error MP4StblAtom:: createChildAtoms (__int64 *pullChildrenSize)
#else
	Error MP4StblAtom:: createChildAtoms (unsigned long long *pullChildrenSize)
#endif
{

	MP4Atom_p											pmaMP4ChildAtom;
	#ifdef WIN32
		__int64												ullChildSize;
	#else
		unsigned long long									ullChildSize;
	#endif


	if (MP4Atom:: createChildAtoms (pullChildrenSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_CREATECHILDATOMS_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// but we also need one of the chunk offset atoms
	
	if (_bUse64Bits)
	{
		if (MP4Atom:: buildAtom ("co64", &pmaMP4ChildAtom,
			_bUseMP4ConsistencyCheck, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_BUILDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		if (MP4Atom:: buildAtom ("stco", &pmaMP4ChildAtom,
			_bUseMP4ConsistencyCheck, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_BUILDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	if (pmaMP4ChildAtom -> init (this, _pfFile,
		_ullAtomStartOffset + _ullAtomSize + *pullChildrenSize,
		_pmtMP4File, _lAtomLevel + 1,
		_bUse64Bits, _bUseMP4ConsistencyCheck,
		_sStandard, _ptTracer, &ullChildSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete pmaMP4ChildAtom;
		pmaMP4ChildAtom				= (MP4Atom_p) NULL;

		return err;
	}

	*pullChildrenSize				+= ullChildSize;

	_vChildrenAtoms. insert (_vChildrenAtoms. end (), pmaMP4ChildAtom);


	return errNoError;
}


Error MP4StblAtom:: createProperties (
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


Error MP4StblAtom:: readProperties (
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


Error MP4StblAtom:: writeProperties (int iFileDescriptor)

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


Error MP4StblAtom:: getPropertiesSize (unsigned long *pulPropertiesSize)

{

	*pulPropertiesSize					= 0;


	return errNoError;
}


Error MP4StblAtom:: getType (char *pType)

{
	strcpy (pType, "stbl");


	return errNoError;
}


Error MP4StblAtom:: appendPropertiesDump (Buffer_p pbBuffer)

{


	return errNoError;
}


Error MP4StblAtom:: printPropertiesOnStdOutput (void)

{


	return errNoError;
}


