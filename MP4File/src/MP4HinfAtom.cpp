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


#include "MP4HinfAtom.h"
#include "FileIO.h"
#include <sys/types.h>
#ifdef WIN32
	#include <stdio.h>
#else
	#include <unistd.h>
#endif
#include <assert.h>



MP4HinfAtom:: MP4HinfAtom (void): MP4Atom ()

{

}


MP4HinfAtom:: ~MP4HinfAtom (void)

{

}



MP4HinfAtom:: MP4HinfAtom (const MP4HinfAtom &)

{

	assert (1==0);

	// to do

}


MP4HinfAtom &MP4HinfAtom:: operator = (const MP4HinfAtom &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4HinfAtom:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile,
		__int64 ullAtomStartOffset, __int64 ullAtomSize,
		unsigned long ulHeaderSize,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4HinfAtom:: init (MP4Atom_p pmaParentAtom,
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


Error MP4HinfAtom:: prepareChildrensAtomsInfo (void)

{

	ChildrenAtomInfo_t					ctiChildrenAtomInfo;


	strcpy (ctiChildrenAtomInfo. _pType, "trpy");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "nump");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "tpyl");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "maxr");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= false;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "dmed");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "dimm");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "drep");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "tmin");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "tmax");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "pmax");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "dmax");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "payt");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);


	return errNoError;
}


#ifdef WIN32
	Error MP4HinfAtom:: createChildAtoms (__int64 *pullChildrenSize)
#else
	Error MP4HinfAtom:: createChildAtoms (unsigned long long *pullChildrenSize)
#endif
{

	// hinf is special in that although all it's child atoms
	// are optional (on read), if we generate it for writing
	// we really want all the children

	std:: vector<ChildrenAtomInfo_t>:: const_iterator	it;
	ChildrenAtomInfo_t									caiChildrenAtomInfo;
	MP4Atom_p											pmaMP4ChildAtom;
	#ifdef WIN32
		__int64												ullChildSize;
	#else
		unsigned long long									ullChildSize;
	#endif


	*pullChildrenSize				= 0;

	for (it = _vChildrensAtomsInfo. begin ();
		it != _vChildrensAtomsInfo. end (); ++it)
	{
		caiChildrenAtomInfo			= *it;

		if (MP4Atom:: buildAtom (caiChildrenAtomInfo. _pType,
			&pmaMP4ChildAtom, _bUseMP4ConsistencyCheck, _ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_BUILDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
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
	}


	return errNoError;
}


Error MP4HinfAtom:: createProperties (
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


Error MP4HinfAtom:: readProperties (
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


Error MP4HinfAtom:: writeProperties (int iFileDescriptor)

{

/*
	#ifdef WIN32
		__int64						ullAtomPropertiesStartOffset;
		__int64						llCurrentPosition;
	#else
		unsigned long long			ullAtomPropertiesStartOffset;
		long long					llCurrentPosition;
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


Error MP4HinfAtom:: getPropertiesSize (unsigned long *pulPropertiesSize)

{

	*pulPropertiesSize					= 0;


	return errNoError;
}


Error MP4HinfAtom:: getType (char *pType)

{
	strcpy (pType, "hinf");


	return errNoError;
}


Error MP4HinfAtom:: appendPropertiesDump (Buffer_p pbBuffer)

{



	return errNoError;
}


Error MP4HinfAtom:: printPropertiesOnStdOutput (void)

{



	return errNoError;
}


