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


#include "MP4RootAtom.h"
#include "FileIO.h"
#include <sys/types.h>
#ifdef WIN32
	#include <stdio.h>
#else
	#include <unistd.h>
#endif
#include <assert.h>



MP4RootAtom:: MP4RootAtom (void): MP4Atom ()

{

}


MP4RootAtom:: ~MP4RootAtom (void)

{

}



MP4RootAtom:: MP4RootAtom (const MP4RootAtom &)

{

	assert (1==0);

	// to do

}


MP4RootAtom &MP4RootAtom:: operator = (const MP4RootAtom &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4RootAtom:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile,
		__int64 ullAtomStartOffset, __int64 ullAtomSize,
		unsigned long ulHeaderSize,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4RootAtom:: init (MP4Atom_p pmaParentAtom,
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


Error MP4RootAtom:: write (int iFileDescriptor)

{

	std:: vector<MP4Atom_p>:: const_iterator	it;
	MP4Atom_p									pmaMp4Atom;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	for (it = _vChildrenAtoms. begin (); it != _vChildrenAtoms. end (); ++it)
	{
		pmaMp4Atom			= *it;

		if (pmaMp4Atom -> write (iFileDescriptor) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_WRITE_FAILED);
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


Error MP4RootAtom:: prepareChildrensAtomsInfo (void)

{

	ChildrenAtomInfo_t					ctiChildrenAtomInfo;


	if (_sStandard == MP4Atom:: MP4F_3GPP)
	{
		strcpy (ctiChildrenAtomInfo. _pType, "ftyp");
		ctiChildrenAtomInfo. _bMandatory		= true;
		ctiChildrenAtomInfo. _bOnlyOne			= true;

		_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
			ctiChildrenAtomInfo);
	}
	else			// MP4Atom:: MP4F_ISMA
		;

	strcpy (ctiChildrenAtomInfo. _pType, "moov");
	ctiChildrenAtomInfo. _bMandatory		= true;
	ctiChildrenAtomInfo. _bOnlyOne			= true;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "mdat");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= false;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "free");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= false;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "skip");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= false;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "udta");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= false;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);

	strcpy (ctiChildrenAtomInfo. _pType, "moof");
	ctiChildrenAtomInfo. _bMandatory		= false;
	ctiChildrenAtomInfo. _bOnlyOne			= false;

	_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
		ctiChildrenAtomInfo);


	return errNoError;
}


Error MP4RootAtom:: createProperties (
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


Error MP4RootAtom:: readProperties (
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


Error MP4RootAtom:: writeProperties (int iFileDescriptor)

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


Error MP4RootAtom:: getPropertiesSize (unsigned long *pulPropertiesSize)

{

	*pulPropertiesSize					= 0;


	return errNoError;
}


Error MP4RootAtom:: getType (char *pType)

{
	strcpy (pType, "root");


	return errNoError;
}


Error MP4RootAtom:: appendPropertiesDump (Buffer_p pbBuffer)

{


	return errNoError;
}


Error MP4RootAtom:: printPropertiesOnStdOutput (void)

{


	return errNoError;
}


Error MP4RootAtom:: moveMoovAtomAsLastAtom (void)

{

	char											pType [MP4_MAXTYPELENGTH];
	long											lChildIndex;
	long											lChildrenNumber;
	#ifdef WIN32
		__int64											ullMoovStartOffset;
		__int64											ullMoovSize;
		__int64											ullSize;
		__int64											llIncrementMoovStartOffset;
	#else
		unsigned long long								ullMoovStartOffset;
		unsigned long long								ullMoovSize;
		unsigned long long								ullSize;
		long long										llIncrementMoovStartOffset;
	#endif
	unsigned long									ulMoovHeaderSize;
	unsigned long									ulLocalHeaderSize;
	MP4Atom_p										pLocalMp4Atom;
	MP4Atom_p										pMp4FreeAtom;
	MP4Atom_p										pMp4MoovAtom;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	lChildrenNumber			= _vChildrenAtoms. size ();

	for (lChildIndex = 0; lChildIndex < lChildrenNumber; lChildIndex++)
	{
		pLocalMp4Atom				= _vChildrenAtoms [lChildIndex];

		if (pLocalMp4Atom -> getType (pType) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_GETTYPE_FAILED);
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

		if (!strcmp (pType, "moov"))
			break;
	}

	if (lChildIndex == lChildrenNumber)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ROOTATOM_MOOVATOMNOTFOUND);
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

	pMp4MoovAtom				= pLocalMp4Atom;

	if (lChildIndex == lChildrenNumber - 1)
	{
		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}

	if (pMp4MoovAtom -> getStartOffset (&ullMoovStartOffset) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETSTARTOFFSET_FAILED);
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

	if (pMp4MoovAtom -> getSize (&ullMoovSize, &ulMoovHeaderSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETSIZE_FAILED);
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

	if (MP4Atom:: buildAtom ("free", &pMp4FreeAtom, _bUseMP4ConsistencyCheck,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_BUILDATOM_FAILED);
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

	if (pMp4FreeAtom -> init (this, _pfFile, ullMoovStartOffset, ullMoovSize,
		ulMoovHeaderSize, _pmtMP4File, _lAtomLevel + 1, _bUse64Bits,
		_bUseMP4ConsistencyCheck, _sStandard, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete pLocalMp4Atom;
		pLocalMp4Atom				= (MP4Atom_p) NULL;

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	_vChildrenAtoms [lChildIndex]	= pMp4FreeAtom;

	llIncrementMoovStartOffset		= 0;

	for (; lChildIndex < lChildrenNumber; lChildIndex++)
	{
		pLocalMp4Atom				= _vChildrenAtoms [lChildIndex];

		if (pLocalMp4Atom -> getSize (&ullSize, &ulLocalHeaderSize) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_GETSIZE_FAILED);
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

		llIncrementMoovStartOffset			+= ullSize;
	}

	if (pMp4MoovAtom -> incrementStartOffset (llIncrementMoovStartOffset) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_INCREMENTSTARTOFFSET_FAILED);
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

	_ullAtomSize				+= ullMoovSize;

	_vChildrenAtoms. insert (_vChildrenAtoms. end (), pMp4MoovAtom);

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

