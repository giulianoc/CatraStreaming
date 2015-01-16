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


#include "MP4File.h"
#include "FileIO.h"
#include "MP4Atom.h"
#include "MP4Co64Atom.h"
#include "MP4CprtAtom.h"
#include "MP4CttsAtom.h"
#include "MP4D263Atom.h"
#include "MP4DamrAtom.h"
#include "MP4DimmAtom.h"
#include "MP4DinfAtom.h"
#include "MP4DmaxAtom.h"
#include "MP4DmedAtom.h"
#include "MP4DpndAtom.h"
#include "MP4DrefAtom.h"
#include "MP4DrepAtom.h"
#include "MP4EdtsAtom.h"
#include "MP4ElstAtom.h"
#include "MP4EsdsAtom.h"
#include "MP4FreeAtom.h"
#include "MP4FtypAtom.h"
#include "MP4HdlrAtom.h"
#include "MP4HinfAtom.h"
#include "MP4HintAtom.h"
#include "MP4HmhdAtom.h"
#include "MP4HntiAtom.h"
#include "MP4IodsAtom.h"
#include "MP4IpirAtom.h"
#include "MP4MaxrAtom.h"
#include "MP4MdatAtom.h"
#include "MP4MdhdAtom.h"
#include "MP4MdiaAtom.h"
#include "MP4MfhdAtom.h"
#include "MP4MinfAtom.h"
#include "MP4MoofAtom.h"
#include "MP4MoovAtom.h"
#include "MP4Mp4aAtom.h"
#include "MP4Mp4sAtom.h"
#include "MP4Mp4vAtom.h"
#include "MP4MpodAtom.h"
#include "MP4MvexAtom.h"
#include "MP4MvhdAtom.h"
#include "MP4MvhdAtom.h"
#include "MP4NmhdAtom.h"
#include "MP4NumpAtom.h"
#include "MP4PaytAtom.h"
#include "MP4PmaxAtom.h"
#include "MP4RtpAtom.h"
#include "MP4S263Atom.h"
#include "MP4SamrAtom.h"
#include "MP4SawbAtom.h"
#include "MP4SdpAtom.h"
#include "MP4SkipAtom.h"
#include "MP4SmhdAtom.h"
#include "MP4SnroAtom.h"
#include "MP4StblAtom.h"
#include "MP4StcoAtom.h"
#include "MP4StdpAtom.h"
#include "MP4StscAtom.h"
#include "MP4StsdAtom.h"
#include "MP4StshAtom.h"
#include "MP4StssAtom.h"
#include "MP4StszAtom.h"
#include "MP4SttsAtom.h"
#include "MP4SyncAtom.h"
#include "MP4TfhdAtom.h"
#include "MP4TimsAtom.h"
#include "MP4TkhdAtom.h"
#include "MP4TmaxAtom.h"
#include "MP4TminAtom.h"
#include "MP4TpylAtom.h"
#include "MP4TrafAtom.h"
#include "MP4TrakAtom.h"
#include "MP4TrefAtom.h"
#include "MP4TrexAtom.h"
#include "MP4TrpyAtom.h"
#include "MP4TrunAtom.h"
#include "MP4TsroAtom.h"
#include "MP4UdtaAtom.h"
#include "MP4UnknAtom.h"
#include "MP4UrlAtom.h"
#include "MP4UrnAtom.h"
#include "MP4UuidAtom.h"
#include "MP4VmhdAtom.h"
#include "MP4WmfsAtom.h"
#include "MP4ContentClassificationDescr.h"
#include "MP4DecoderConfigDescr.h"
#include "MP4DecoderSpecificDescr.h"
#include "MP4ESDescr.h"
#include "MP4EsIdIncDescr.h"
#include "MP4ExtensionDescr.h"
#include "MP4InitialObjectDescr.h"
#include "MP4ObjectDescr.h"
#include "MP4RatingDescr.h"
#include "MP4SyncLayerConfigDescr.h"
#include "MP4UnknownOCIDescr.h"
#include "MP4UnknownDescr.h"
#include "MP4FileMessages.h"
#include <sys/types.h>
#ifdef WIN32
#else
	#include <unistd.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>



MP4Atom:: MP4Atom (void)

{

}


MP4Atom:: ~MP4Atom (void)

{

}



MP4Atom:: MP4Atom (const MP4Atom &)

{

	assert (1==0);

	// to do

}


MP4Atom &MP4Atom:: operator = (const MP4Atom &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4Atom:: init (MP4Atom_p pmaParentAtom, FileReader_p pfFile,
		__int64 ullAtomStartOffset, __int64 ullAtomSize,
		unsigned long ulHeaderSize, PMutex_p pmtMP4File,
		long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4Atom:: init (MP4Atom_p pmaParentAtom, FileReader_p pfFile,
		unsigned long long ullAtomStartOffset, unsigned long long ullAtomSize,
		unsigned long ulHeaderSize, PMutex_p pmtMP4File,
		long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#endif

{

	unsigned long				ulPropertiesSize;


	if (pfFile == (FileReader_p) NULL ||
		ullAtomSize < 1 ||
		ulHeaderSize < 0 ||
		pmtMP4File == (PMutex_p) NULL ||
		ptTracer == (Tracer_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);

		return err;
	}

	_lAtomLevel						= lAtomLevel;
	_bUse64Bits						= bUse64Bits;
	_bUseMP4ConsistencyCheck		= bUseMP4ConsistencyCheck;
	_pmaParentAtom					= pmaParentAtom;
	_pfFile							= pfFile;

	_ullAtomStartOffset				= ullAtomStartOffset;
	_ullAtomSize					= ullAtomSize;
	_ulHeaderSize					= ulHeaderSize;
	_pmtMP4File						= pmtMP4File;
	_sStandard						= sStandard;
	_ptTracer						= ptTracer;

	if (readProperties (&ulPropertiesSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_READPROPERTIES_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (prepareChildrensAtomsInfo () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_PREPARECHILDRENATOMSINFO_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// _vChildrensAtomsInfo size is zero because just builded
	// _vChildrenAtoms size is zero because just builded

	if (readChildAtoms () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_READCHILDATOMS_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error MP4Atom:: init (MP4Atom_p pmaParentAtom, FileReader_p pfFile,
		__int64 ullAtomStartOffset, PMutex_p pmtMP4File, long lAtomLevel,
		Boolean_t bUse64Bits, Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer,
		__int64 *pullAtomSize)
#else
	Error MP4Atom:: init (MP4Atom_p pmaParentAtom, FileReader_p pfFile,
		unsigned long long ullAtomStartOffset, PMutex_p pmtMP4File, long lAtomLevel,
		Boolean_t bUse64Bits, Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer,
		unsigned long long *pullAtomSize)
#endif

{

	#ifdef WIN32
		__int64								ullChildrenSize;
	#else
		unsigned long long					ullChildrenSize;
	#endif
	unsigned long						ulPropertiesSize;
	char								pType [MP4_MAXTYPELENGTH];


	#ifdef WIN32
		if (pfFile == (FileReader_p) NULL ||
			pullAtomSize == (__int64 *) NULL ||
			pmtMP4File == (PMutex_p) NULL ||
			ptTracer == (Tracer_p) NULL)
	#else
		if (pfFile == (FileReader_p) NULL ||
			pullAtomSize == (unsigned long long *) NULL ||
			pmtMP4File == (PMutex_p) NULL ||
			ptTracer == (Tracer_p) NULL)
	#endif
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);

		return err;
	}

	_lAtomLevel						= lAtomLevel;
	_bUse64Bits						= bUse64Bits;
	_bUseMP4ConsistencyCheck		= bUseMP4ConsistencyCheck;
	_pmaParentAtom					= pmaParentAtom;
	_pfFile							= pfFile;

	_ullAtomStartOffset				= ullAtomStartOffset;
	_pmtMP4File						= pmtMP4File;
	_sStandard						= sStandard;
	_ptTracer						= ptTracer;


	_ulHeaderSize					= _bUse64Bits ? 16 : 8;
	_ullAtomSize					= _ulHeaderSize;

	if (createProperties (&ulPropertiesSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_CREATEPROPERTIES_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	_ullAtomSize					+= ulPropertiesSize;

	if (getType (pType) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (!strcmp (pType, "uuid"))
	{
		_ullAtomSize					+= MP4F_EXTENDEDTYPELENGTH;
	}

	if (prepareChildrensAtomsInfo () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_PREPARECHILDRENATOMSINFO_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// _vChildrensAtomsInfo size is zero because just builded
	// _vChildrenAtoms size is zero because just builded

	if (createChildAtoms (&ullChildrenSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_CREATECHILDATOMS_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	_ullAtomSize					+= ullChildrenSize;

	*pullAtomSize					= _ullAtomSize;


	return errNoError;
}


Error MP4Atom:: finish (void)

{

	if (_vChildrenAtoms. size () > 0)
	{
		std:: vector<MP4Atom_p>:: const_iterator	it;
		MP4Atom_p									pMp4Atom;


		for (it = _vChildrenAtoms. begin (); it != _vChildrenAtoms. end ();
			++it)
		{
			pMp4Atom			= *it;

			if (pMp4Atom -> finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			delete pMp4Atom;
			pMp4Atom			= (MP4Atom_p) NULL;
		}
		_vChildrenAtoms. clear ();
	}


	return errNoError;
}


Error MP4Atom:: write (int iFileDescriptor)

{

	char										pType [MP4_MAXTYPELENGTH];
	std:: vector<MP4Atom_p>:: const_iterator	it;
	MP4Atom_p									pmaMp4Atom;
	#ifdef WIN32
		__int64									llPosition;
		__int64									llBytesWritten;
	#else
		long long								llPosition;
		long long								llBytesWritten;
	#endif


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getType (pType) != errNoError)
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

	if (FileIO:: seek (iFileDescriptor, _ullAtomStartOffset, SEEK_SET,
		&llPosition) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_SEEK_FAILED);
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

	if (_bUse64Bits)
	{
		if (FileIO:: writeNetUnsignedInt32Bit (iFileDescriptor, 1) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITENETUNSIGNEDINT32BIT_FAILED);
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

		if (FileIO:: writeChars (iFileDescriptor, pType,
			MP4_MAXATOMTYPELENGTH - 1, &llBytesWritten) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITECHARS_FAILED);
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

		if (FileIO:: writeNetUnsignedInt64Bit (iFileDescriptor, _ullAtomSize) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITENETUNSIGNEDINT64BIT_FAILED);
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
	else
	{
		if (FileIO:: writeNetUnsignedInt32Bit (iFileDescriptor,
			(unsigned long) _ullAtomSize) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITENETUNSIGNEDINT32BIT_FAILED);
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

		if (FileIO:: writeChars (iFileDescriptor, pType,
			MP4_MAXATOMTYPELENGTH - 1, &llBytesWritten) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITECHARS_FAILED);
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

	if (writeProperties (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_WRITEPROPERTIES_FAILED);
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


Error MP4Atom:: getParent (MP4Atom_p *pParent)

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pParent				= _pmaParentAtom;

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


Error MP4Atom:: getChild (unsigned long ulChildIndex, MP4Atom_p *pParent)

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (ulChildIndex >= _vChildrenAtoms. size ())
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

	*pParent				= _vChildrenAtoms [ulChildIndex];

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


Error MP4Atom:: getChildIndex (MP4Atom_p pmaMP4Atom, long *plChildIndex)

{

	std:: vector<MP4Atom_p>:: const_iterator	it;
	MP4Atom_p									pMp4LocalAtom;


	*plChildIndex					= 0;

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
		pMp4LocalAtom			= *it;

		if (pMp4LocalAtom == pmaMP4Atom)
			break;

		*plChildIndex					+= 1;
	}

	if (it == _vChildrenAtoms. end ())
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_ATOMNOTFOUND, 1, "");
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


Error MP4Atom:: getChildrenNumber (long *plChildrenNumber)

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*plChildrenNumber					= _vChildrenAtoms. size ();

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
	Error MP4Atom:: getStartOffset (__int64 *pullStartOffset)
#else
	Error MP4Atom:: getStartOffset (unsigned long long *pullStartOffset)
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

	*pullStartOffset				= _ullAtomStartOffset;

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
	Error MP4Atom:: incrementStartOffset (__int64 llIncrementStartOffset)
#else
	Error MP4Atom:: incrementStartOffset (long long llIncrementStartOffset)
#endif

{

	std:: vector<MP4Atom_p>:: const_iterator	it;
	MP4Atom_p									pLocalMp4Atom;


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	_ullAtomStartOffset			+= llIncrementStartOffset;

	for (it = _vChildrenAtoms. begin (); it != _vChildrenAtoms. end (); ++it)
	{
		pLocalMp4Atom			= *it;

		if (pLocalMp4Atom -> incrementStartOffset (llIncrementStartOffset) !=
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
	Error MP4Atom:: getSize (__int64 *pullSize,
		unsigned long *pulHeaderSize)
#else
	Error MP4Atom:: getSize (unsigned long long *pullSize,
		unsigned long *pulHeaderSize)
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

	*pullSize				= _ullAtomSize;

	*pulHeaderSize			= _ulHeaderSize;

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


Error MP4Atom:: getStandard (Standard_p psStandard)

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*psStandard				= _sStandard;

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


Error MP4Atom:: addChild (const char *pType, MP4Atom_p *pmaMp4Atom,
	long lIndexToInsert)

{

	MP4Atom_p				pmaChildAtom;
	#ifdef WIN32
		__int64					ullChildSize;
		__int64					ullChildAtomStartOffset;
		__int64					ullLocalAtomSize;
	#else
		unsigned long long		ullChildSize;
		unsigned long long		ullChildAtomStartOffset;
		unsigned long long		ullLocalAtomSize;
	#endif
	unsigned long			ulChildrenNumber;
	unsigned long			ulChildIndex;
	unsigned long			ulLocalHeaderSize;


	if (pType == (const char *) NULL ||
		pmaMp4Atom == (MP4Atom_p *) NULL ||
		(lIndexToInsert < 0 && lIndexToInsert != -1))
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	ulChildrenNumber				= _vChildrenAtoms. size ();

	if (lIndexToInsert == -1)		// append the atom
		lIndexToInsert			= ulChildrenNumber;

	if ((unsigned long) lIndexToInsert > ulChildrenNumber)
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

	if (consistencyCheck (pType) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_CONSISTENCYCHECK_FAILED);
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

	if (MP4Atom:: buildAtom (pType, pmaMp4Atom, _bUseMP4ConsistencyCheck,
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

	if ((*pmaMp4Atom) -> init (this, _pfFile, 0,
		_pmtMP4File, _lAtomLevel + 1,
		_bUse64Bits, _bUseMP4ConsistencyCheck, _sStandard,
		_ptTracer, &ullChildSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete (*pmaMp4Atom);
		(*pmaMp4Atom)				= (MP4Atom_p) NULL;

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	ullChildAtomStartOffset				= _ullAtomStartOffset + _ullAtomSize;

	for (ulChildIndex = lIndexToInsert; ulChildIndex < ulChildrenNumber;
		ulChildIndex++)
	{
		pmaChildAtom				= _vChildrenAtoms [ulChildIndex];

		if (pmaChildAtom -> incrementStartOffset (ullChildSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_INCREMENTSTARTOFFSET_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete (*pmaMp4Atom);
			(*pmaMp4Atom)				= (MP4Atom_p) NULL;

			if (_pmtMP4File -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaChildAtom -> getSize (&ullLocalAtomSize, &ulLocalHeaderSize) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete (*pmaMp4Atom);
			(*pmaMp4Atom)				= (MP4Atom_p) NULL;

			if (_pmtMP4File -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		ullChildAtomStartOffset				-= ullLocalAtomSize;
	}

	if ((*pmaMp4Atom) -> incrementStartOffset (
		ullChildAtomStartOffset) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_INCREMENTSTARTOFFSET_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if ((*pmaMp4Atom) -> finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		delete (*pmaMp4Atom);
		(*pmaMp4Atom)				= (MP4Atom_p) NULL;

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	_vChildrenAtoms. insert (_vChildrenAtoms. begin () + lIndexToInsert,
		(*pmaMp4Atom));

	if (changeSizeManagement (ullChildSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_CHANGESIZEMANAGEMENT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if ((*pmaMp4Atom) -> finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		delete (*pmaMp4Atom);
		(*pmaMp4Atom)				= (MP4Atom_p) NULL;

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


Error MP4Atom:: deleteChild (MP4Atom_p pmaMp4AtomToDelete)

{

	std:: vector<MP4Atom_p>:: iterator			it;
	std:: vector<MP4Atom_p>:: iterator			itToDelete;
	#ifdef WIN32
		__int64										ullMp4AtomToDeleteSize;
	#else
		unsigned long long							ullMp4AtomToDeleteSize;
	#endif
	unsigned long								ulHeaderSize;
	MP4Atom_p									pLocalMp4Atom;


	if (pmaMp4AtomToDelete == (MP4Atom_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmaMp4AtomToDelete -> getSize (&ullMp4AtomToDeleteSize,
		&ulHeaderSize) != errNoError)
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

	for (it = _vChildrenAtoms. begin (); it != _vChildrenAtoms. end (); ++it)
	{
		pLocalMp4Atom			= *it;

		if (pLocalMp4Atom == pmaMp4AtomToDelete)
		{
			itToDelete				= it;

			break;
		}
	}

	if (it == _vChildrenAtoms. end ())
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

	for (++it; it != _vChildrenAtoms. end (); ++it)
	{
		pLocalMp4Atom				= *it;

		if (pLocalMp4Atom -> incrementStartOffset (-ullMp4AtomToDeleteSize) !=
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
	}

	_vChildrenAtoms. erase (itToDelete);

	if (changeSizeManagement (-ullMp4AtomToDeleteSize) != errNoError)
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

	if (pmaMp4AtomToDelete -> finish () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_FINISH_FAILED);
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

	delete pmaMp4AtomToDelete;
	pmaMp4AtomToDelete			= (MP4Atom_p) NULL;

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


Error MP4Atom:: addMdatChild (MP4Atom_p *pmaMp4Atom)

{

	unsigned long					ulChildIndex;
	unsigned long					ulChildrenNumber;
	long							lLastMdatIndex;
	MP4Atom_p						pLocalMp4Atom;
	char							pType [MP4_MAXTYPELENGTH];


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	ulChildrenNumber			= _vChildrenAtoms. size ();

	lLastMdatIndex				= -1;

	for (ulChildIndex = 0; ulChildIndex < ulChildrenNumber; ulChildIndex++)
	{
		pLocalMp4Atom			= _vChildrenAtoms [ulChildIndex];

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

		if (!strcmp (pType, "mdat"))
			lLastMdatIndex			= ulChildIndex;
	}

	if (lLastMdatIndex == -1)
		lLastMdatIndex				= 0;
	else
		lLastMdatIndex++;

	if (addChild ("mdat", pmaMp4Atom, lLastMdatIndex) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_ADDCHILD_FAILED);
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


Error MP4Atom:: searchAtom (const char *pAtomPath,
	Boolean_t bIsSearchErrorTraceEnabled, MP4Atom_p *pmp4Atom)

{

	char				pAtomTypeToSearch [MP4_MAXTYPELENGTH];
	long				lAtomIndexToSearch;
	const char			*pEndAtomIndex;
	MP4Atom_p			pLocalMp4Atom;
	Error_t				errSearchAtom;


	if (pAtomPath == (const char *) NULL ||
		pmp4Atom == (MP4Atom_p *) NULL ||
		strchr (pAtomPath, ':') == (char *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strncpy (pAtomTypeToSearch, pAtomPath, strchr (pAtomPath, ':') - pAtomPath);
	pAtomTypeToSearch [strchr (pAtomPath, ':') - pAtomPath]			= '\0';

	{
		const char				*pStartAtomIndex;
		char					pAtomIndexToSearch [MP4_MAXLONGLENGTH];


		pStartAtomIndex				= pAtomPath +
			(strchr (pAtomPath, ':') - pAtomPath) + 1;
		pEndAtomIndex				= strchr (pStartAtomIndex, ':');

		if (pEndAtomIndex == (const char *) NULL)
		{
			lAtomIndexToSearch			= atol (pStartAtomIndex);
		}
		else
		{
			strncpy (pAtomIndexToSearch, pStartAtomIndex,
				pEndAtomIndex - pStartAtomIndex);
			pAtomIndexToSearch [pEndAtomIndex - pStartAtomIndex]		= '\0';

			lAtomIndexToSearch			= atol (pAtomIndexToSearch);
		}
	}

	// initialize pLocalMp4Atom
	{
		char										pAtomType [
			MP4_MAXTYPELENGTH];
		std:: vector<MP4Atom_p>:: const_iterator	it;


		for (it = _vChildrenAtoms. begin (); it != _vChildrenAtoms. end ();
			++it)
		{
			pLocalMp4Atom			= *it;

			if (pLocalMp4Atom -> getType (pAtomType) != errNoError)
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

			if (!strcmp (pAtomType, pAtomTypeToSearch))
			{
				if (lAtomIndexToSearch == 0)
					break;
				else
					lAtomIndexToSearch--;
			}
		}

		if (it == _vChildrenAtoms. end ())
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_ATOMNOTFOUND, 1, pAtomTypeToSearch);
			if (bIsSearchErrorTraceEnabled)
			{
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

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

	if (pEndAtomIndex == (const char *) NULL)
		*pmp4Atom				= pLocalMp4Atom;
	else
	{
		if ((errSearchAtom = pLocalMp4Atom -> searchAtom (pEndAtomIndex + 1,
			bIsSearchErrorTraceEnabled, pmp4Atom)) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, pEndAtomIndex + 1);
			if (bIsSearchErrorTraceEnabled)
			{
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (_pmtMP4File -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errSearchAtom;
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


Error MP4Atom:: lockMP4File (void)

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4Atom:: unLockMP4File (void)

{

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


Error MP4Atom:: appendDump (Buffer_p pbBuffer)

{

	long									lLevelIndex;
	std:: vector<MP4Atom_p>:: const_iterator	it;
	MP4Atom_p								pMp4Atom;
	char									pAtomType [MP4_MAXTYPELENGTH];


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getType (pAtomType) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETTYPE_FAILED);
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

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel; lLevelIndex++)
	{
		if (pbBuffer -> append ("  ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

	if (!strcmp (pAtomType, "uuid"))
	{
		unsigned char			pucExtendedType [MP4F_EXTENDEDTYPELENGTH];
		long					lIndex;


		if (((MP4UuidAtom_p) this) -> getExtendedType (pucExtendedType) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UUIDATOM_GETEXTENDEDTYPE_FAILED);
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

		if (pbBuffer -> append ("Type: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

		if (pbBuffer -> append (pAtomType) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

		if (pbBuffer -> append (" (file offsets: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

		if (pbBuffer -> append (_ullAtomStartOffset) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

		if (pbBuffer -> append ("-") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

		if (pbBuffer -> append (_ullAtomStartOffset + _ullAtomSize) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

		if (pbBuffer -> append (", Extended type: '") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

		for (lIndex = 0; lIndex < MP4F_EXTENDEDTYPELENGTH; lIndex++)
		{
			if (pbBuffer -> append ((char) (pucExtendedType [lIndex])) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
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

		if (pbBuffer -> append ("')\n") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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
		if (pbBuffer -> append ("Type: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

		if (pbBuffer -> append (pAtomType) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

		if (pbBuffer -> append (" (file offsets: ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

		if (pbBuffer -> append (_ullAtomStartOffset) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

		if (pbBuffer -> append ("-") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

		if (pbBuffer -> append (_ullAtomStartOffset + _ullAtomSize) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

		if (pbBuffer -> append (")\n") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
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

	if (appendPropertiesDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_APPENDPROPERTIESDUMP_FAILED);
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

	for (it = _vChildrenAtoms. begin (); it != _vChildrenAtoms. end (); ++it)
	{
		pMp4Atom			= *it;

		if (pMp4Atom -> appendDump (pbBuffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_APPENDDUMP_FAILED);
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


Error MP4Atom:: printOnStdOutput (void)

{

	long									lLevelIndex;
	std:: vector<MP4Atom_p>:: const_iterator	it;
	MP4Atom_p								pMp4Atom;
	char									pAtomType [MP4_MAXTYPELENGTH];


	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getType (pAtomType) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETTYPE_FAILED);
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

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel; lLevelIndex++)
		std:: cout << "  ";

	if (!strcmp (pAtomType, "uuid"))
	{
		unsigned char			pucExtendedType [MP4F_EXTENDEDTYPELENGTH];
		long					lIndex;


		if (((MP4UuidAtom_p) this) -> getExtendedType (pucExtendedType) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UUIDATOM_GETEXTENDEDTYPE_FAILED);
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

		#ifdef WIN32
			std:: cout << "Type: " << pAtomType << " (file offsets: "
				<< _ullAtomStartOffset << "-"
				<< (_ullAtomStartOffset + _ullAtomSize)
				<< ", Extended type: '";
		#else
			std:: cout << "Type: " << pAtomType << " (file offsets: "
				<< _ullAtomStartOffset << "-"
				<< _ullAtomStartOffset + _ullAtomSize
				<< ", Extended type: '";
		#endif

		for (lIndex = 0; lIndex < MP4F_EXTENDEDTYPELENGTH; lIndex++)
			std:: cout << (char) (pucExtendedType [lIndex]);
		std:: cout << "')" << std:: endl;
	}
	else
		#ifdef WIN32
			std:: cout << "Type: " << pAtomType << " (file offsets: "
				<< _ullAtomStartOffset << "-"
				<< (_ullAtomStartOffset + _ullAtomSize) << ")"
				<< std:: endl;
		#else
			std:: cout << "Type: " << pAtomType << " (file offsets: "
				<< _ullAtomStartOffset << "-"
				<< _ullAtomStartOffset + _ullAtomSize << ")"
				<< std:: endl;
		#endif

	if (printPropertiesOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_PRINTPROPERTIESONSTDOUTPUT_FAILED);
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

	for (it = _vChildrenAtoms. begin (); it != _vChildrenAtoms. end (); ++it)
	{
		pMp4Atom			= *it;

		if (pMp4Atom -> printOnStdOutput () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_PRINTONSTDOUTPUT_FAILED);
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
	Error MP4Atom:: createChildAtoms (__int64 *pullChildrenSize)
#else
	Error MP4Atom:: createChildAtoms (unsigned long long *pullChildrenSize)
#endif

{

	std:: vector<ChildrenAtomInfo_t>:: const_iterator	it;
	ChildrenAtomInfo_t									caiChildrenAtomInfo;
	MP4Atom_p											pmaMP4ChildAtom;
	#ifdef WIN32
		__int64												ullChildSize;
	#else
		unsigned long long									ullChildSize;
	#endif
	Boolean_t											bIsDescriptor;


	*pullChildrenSize				= 0;

	for (it = _vChildrensAtomsInfo. begin ();
		it != _vChildrensAtomsInfo. end (); ++it)
	{
		caiChildrenAtomInfo			= *it;

		if (caiChildrenAtomInfo. _bMandatory &&
			caiChildrenAtomInfo. _bOnlyOne)
		{
			if (MP4Atom:: isDescriptor (caiChildrenAtomInfo. _pType,
				&bIsDescriptor) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_ISDESCRIPTOR_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if (bIsDescriptor)
			{
				unsigned long					ulTag;


				// getTag
				if (MP4Atom:: getDescriptorTag (caiChildrenAtomInfo. _pType,
					&ulTag, _ptTracer) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_GETDESCRIPTORTAG_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (MP4Atom:: buildDescriptor (ulTag,
					&pmaMP4ChildAtom, _bUseMP4ConsistencyCheck, _ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_BUILDATOM_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (((MP4Descr_p) pmaMP4ChildAtom) -> init (this, _pfFile,
					_ullAtomStartOffset + _ullAtomSize + *pullChildrenSize,
					4, // ucNumBytesForDescriptorSize
					ulTag, _pmtMP4File, _lAtomLevel + 1,
					_bUse64Bits, _bUseMP4ConsistencyCheck,
					_sStandard, _ptTracer, &ullChildSize) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete pmaMP4ChildAtom;
					pmaMP4ChildAtom				= (MP4Atom_p) NULL;

					return err;
				}
			}
			else
			{
				if (MP4Atom:: buildAtom (caiChildrenAtomInfo. _pType,
					&pmaMP4ChildAtom, _bUseMP4ConsistencyCheck, _ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_BUILDATOM_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

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
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete pmaMP4ChildAtom;
					pmaMP4ChildAtom				= (MP4Atom_p) NULL;

					return err;
				}
			}

			*pullChildrenSize				+= ullChildSize;

			_vChildrenAtoms. insert (_vChildrenAtoms. end (), pmaMP4ChildAtom);
		}
	}


	return errNoError;
}


Error MP4Atom:: readChildAtoms (void)

{

	unsigned long			ulPropertiesSize;
	unsigned long			ulChildAtomSize;
	#ifdef WIN32
		__int64					ullChildAtomStartOffset;
		__int64					ullChildAtomSize;
		__int64					ullCharsRead;
	#else
		unsigned long long		ullChildAtomStartOffset;
		unsigned long long		ullChildAtomSize;
		unsigned long long		ullCharsRead;
	#endif
	long					lChildHeaderSize;
	char					pType [MP4_MAXTYPELENGTH];
	MP4Atom_p				pmaMP4ChildAtom;
	Error_t					errBuildAtom;
	#ifdef WIN32
		__int64									llPosition;
	#else
		long long								llPosition;
	#endif


	if (getType (pType) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (getPropertiesSize (&ulPropertiesSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETPROPERTIESSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (!strcmp (pType, "uuid"))
	{
		// if _bUse64Bits is true, the atom header should be:
		//		size		32 bits		(1)
		//		size		64 bits
		//		type		4 bytes
		// if _bUse64Bits is false, the atom header should be:
		//		size		32 bits
		//		type		4 bytes
		ullChildAtomStartOffset			= _ullAtomStartOffset +
			_ulHeaderSize + MP4F_EXTENDEDTYPELENGTH + ulPropertiesSize;
	}
	else
	{
		// if _bUse64Bits is true, the atom header should be:
		//		size		32 bits		(1)
		//		size		64 bits
		//		type		4 bytes
		// if _bUse64Bits is false, the atom header should be:
		//		size		32 bits
		//		type		4 bytes
		ullChildAtomStartOffset			= _ullAtomStartOffset +
			_ulHeaderSize + ulPropertiesSize;
	}

	if (_pfFile -> seek (ullChildAtomStartOffset, SEEK_SET,
		&llPosition) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEREADER_SEEK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	while (ullChildAtomStartOffset < _ullAtomStartOffset + _ullAtomSize)
	{
		lChildHeaderSize					= 8;

		if (_pfFile -> readNetUnsignedInt32Bit (&ulChildAtomSize) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_READNETUNSIGNEDINT32BIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		ullChildAtomSize					= ulChildAtomSize;

		if (_pfFile -> readChars (pType, MP4_MAXATOMTYPELENGTH - 1,
			true, &ullCharsRead) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_READCHARS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		pType [MP4_MAXATOMTYPELENGTH - 1]				= '\0';

		if (ullChildAtomSize == 1)
		{
			// La lib mpeg4i ne legge 8 ma sembra usi solo gli ultimi 4
			// Verificare con il debug
			if (_pfFile -> readNetUnsignedInt64Bit (&ullChildAtomSize) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEREADER_READNETUNSIGNEDINT64BIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			lChildHeaderSize					+= 8;
		}

		if (ullChildAtomSize == 0)
		{
			#ifdef WIN32
				__int64				llCurrentPosition;
				__int64				llEndPosition;
			#else
				long long			llCurrentPosition;
				long long			llEndPosition;
			#endif


			if (_pfFile -> seek (0, SEEK_CUR, &llCurrentPosition) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEREADER_SEEK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if (_pfFile -> seek (0, SEEK_END, &llEndPosition) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEREADER_SEEK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			ullChildAtomSize			= llEndPosition - ullChildAtomStartOffset;

			if (_pfFile -> seek (llCurrentPosition, SEEK_SET, &llPosition) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEREADER_SEEK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
		}

		{
			char				pCurrentType [MP4_MAXTYPELENGTH];

			getType (pCurrentType);
			Message msg = MP4FileMessages (__FILE__, __LINE__,
				MP4F_MP4ATOM_FOUNDATOM, 4,
				pType, ullChildAtomStartOffset, (long) ullChildAtomSize,
				pCurrentType);
			_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (consistencyCheck (pType) != errNoError)
		{
			if (_bUseMP4ConsistencyCheck)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_CONSISTENCYCHECK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if ((errBuildAtom = MP4Atom:: buildAtom (pType, &pmaMP4ChildAtom,
			_bUseMP4ConsistencyCheck, _ptTracer)) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_BUILDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (pmaMP4ChildAtom -> init (this, _pfFile,
			ullChildAtomStartOffset, ullChildAtomSize, lChildHeaderSize,
			_pmtMP4File, _lAtomLevel + 1, _bUse64Bits, _bUseMP4ConsistencyCheck,
			_sStandard, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete pmaMP4ChildAtom;
			pmaMP4ChildAtom				= (MP4Atom_p) NULL;

			return err;
		}

		_vChildrenAtoms. insert (_vChildrenAtoms. end (), pmaMP4ChildAtom);

		ullChildAtomStartOffset			+= ullChildAtomSize;

		if (_pfFile -> seek (ullChildAtomStartOffset, SEEK_SET, &llPosition) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_SEEK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	if (consistencyCheck () != errNoError)
	{
		if (_bUseMP4ConsistencyCheck)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_CONSISTENCYCHECK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	// lseek su end se e' prima di end -> skip
	// non penso serve


	return errNoError;
}


Error MP4Atom:: consistencyCheck (const char *pTypeToInsert)

{

	std:: vector<ChildrenAtomInfo_t>:: const_iterator	it;
	ChildrenAtomInfo_t									caiChildrenAtomInfo;


	if (!strcmp (pTypeToInsert, "uuid") ||
		!strcmp (pTypeToInsert, "free"))
		return errNoError;

	for (it = _vChildrensAtomsInfo. begin ();
		it != _vChildrensAtomsInfo. end (); ++it)
	{
		caiChildrenAtomInfo			= *it;

		if (!strcmp (caiChildrenAtomInfo. _pType, pTypeToInsert))
			break;
	}

	if (it == _vChildrensAtomsInfo. end ())
	{
		char				pCurrentType [MP4_MAXTYPELENGTH];

		getType (pCurrentType);
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_ATOMNOTINCORRECTPOSITION,
			2, pTypeToInsert, pCurrentType);
		_ptTracer -> trace (Tracer:: TRACER_LWRNG, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (caiChildrenAtomInfo. _bOnlyOne)
	{
		long									lCurrentTypedAtomsNumber;
		std:: vector<MP4Atom_p>:: const_iterator	it;
		MP4Atom_p								pMp4Atom;
		char									pType [MP4_MAXTYPELENGTH];


		lCurrentTypedAtomsNumber			= 0;

		for (it = _vChildrenAtoms. begin (); it != _vChildrenAtoms. end ();
			++it)
		{
			pMp4Atom			= *it;

			if (pMp4Atom -> getType (pType) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_GETTYPE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if (!strcmp (pType, pTypeToInsert))
				lCurrentTypedAtomsNumber++;
		}

		if (lCurrentTypedAtomsNumber != 0)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_ATOMINSTANCESNOTCORRECTED,
				2, pTypeToInsert, lCurrentTypedAtomsNumber);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error MP4Atom:: consistencyCheck (void)

{

	// for each required
	// 	verify if exist almost one
	// 	if no
	// 		error
	std:: vector<ChildrenAtomInfo_t>:: const_iterator	it1;
	ChildrenAtomInfo_t									caiChildrenAtomInfo;
	std:: vector<MP4Atom_p>:: const_iterator			it2;
	MP4Atom_p											pMp4Atom;
	char												pType [
		MP4_MAXTYPELENGTH];


	for (it1 = _vChildrensAtomsInfo. begin ();
		it1 != _vChildrensAtomsInfo. end (); ++it1)
	{
		caiChildrenAtomInfo			= *it1;

		if (caiChildrenAtomInfo. _bMandatory)
		{
			for (it2 = _vChildrenAtoms. begin ();
				it2 != _vChildrenAtoms. end (); ++it2)
			{
				pMp4Atom			= *it2;

				if (pMp4Atom -> getType (pType) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_GETTYPE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (!strcmp (pType, caiChildrenAtomInfo. _pType))
					break;
			}

			if (it2 == _vChildrenAtoms. end ())
			{
				char				pCurrentType [MP4_MAXTYPELENGTH];

				getType (pCurrentType);
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_ATOMMANDATORYNOTEXIST,
					2, caiChildrenAtomInfo. _pType, pCurrentType);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
		}
	}


	return errNoError;
}


#ifdef WIN32
	Error MP4Atom:: changeSizeManagement (__int64 llIncrement)
#else
	Error MP4Atom:: changeSizeManagement (long long llIncrement)
#endif

{

	MP4Atom_p				pmaMP4GranParentAtom;
	MP4Atom_p				pmaMP4ParentAtom;
	MP4Atom_p				pmaMP4AtomToIncrement;
	long					lChildIndex;
	long					lChildrenNumber;


	if (getParent (&pmaMP4GranParentAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETPARENT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	pmaMP4ParentAtom				= this;

	while (pmaMP4GranParentAtom != (MP4Atom_p) NULL)
	{
		if (pmaMP4GranParentAtom -> getChildIndex (pmaMP4ParentAtom,
			&lChildIndex) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_GETCHILDINDEX_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (pmaMP4GranParentAtom -> getChildrenNumber (&lChildrenNumber) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_GETCHILDRENNUMBER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		for (lChildIndex++; lChildIndex < lChildrenNumber; lChildIndex++)
		{
			if (pmaMP4GranParentAtom -> getChild (lChildIndex,
				&pmaMP4AtomToIncrement) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_GETCHILD_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if (pmaMP4AtomToIncrement -> incrementStartOffset (
				llIncrement) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_INCREMENTSTARTOFFSET_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
		}

		pmaMP4ParentAtom -> _ullAtomSize				+= llIncrement;

		pmaMP4ParentAtom								= pmaMP4GranParentAtom;

		if (pmaMP4ParentAtom -> getParent (&pmaMP4GranParentAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_GETPARENT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	// pmaMP4ParentAtom is 'root'
	pmaMP4ParentAtom -> _ullAtomSize				+= llIncrement;


	return errNoError;
}



Error MP4Atom:: buildAtom (const char *pType, MP4Atom_p *pmaMP4Atom,
	Boolean_t bUseMP4ConsistencyCheck, Tracer_p ptTracer)

{

	// long				lTypeIdentifier;


	// lTypeIdentifier			=
	// 	(pType [0] << 24) | (pType [1] << 16) | (pType [2] << 8) | pType [3];

	if (!strcmp (pType, "co64"))
	{
		if ((*pmaMP4Atom = new MP4Co64Atom_t ()) == (MP4Co64Atom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "cprt"))
	{
		if ((*pmaMP4Atom = new MP4CprtAtom_t ()) == (MP4CprtAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "ctts"))
	{
		if ((*pmaMP4Atom = new MP4CttsAtom_t ()) == (MP4CttsAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "d263"))	// only 3GPP
	{
		if ((*pmaMP4Atom = new MP4D263Atom_t ()) == (MP4D263Atom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "damr"))	// only 3GPP
	{
		if ((*pmaMP4Atom = new MP4DamrAtom_t ()) == (MP4DamrAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "dimm"))
	{
		if ((*pmaMP4Atom = new MP4DimmAtom_t ()) == (MP4DimmAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "dinf"))
	{
		if ((*pmaMP4Atom = new MP4DinfAtom_t ()) == (MP4DinfAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "dmax"))
	{
		if ((*pmaMP4Atom = new MP4DmaxAtom_t ()) == (MP4DmaxAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "dmed"))
	{
		if ((*pmaMP4Atom = new MP4DmedAtom_t ()) == (MP4DmedAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "dpnd"))
	{
		if ((*pmaMP4Atom = new MP4DpndAtom_t ()) == (MP4DpndAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "dref"))
	{
		if ((*pmaMP4Atom = new MP4DrefAtom_t ()) == (MP4DrefAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "drep"))
	{
		if ((*pmaMP4Atom = new MP4DrepAtom_t ()) == (MP4DrepAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "edts"))
	{
		if ((*pmaMP4Atom = new MP4EdtsAtom_t ()) == (MP4EdtsAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "elst"))
	{
		if ((*pmaMP4Atom = new MP4ElstAtom_t ()) == (MP4ElstAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "esds"))
	{
		if ((*pmaMP4Atom = new MP4EsdsAtom_t ()) == (MP4EsdsAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "free"))
	{
		if ((*pmaMP4Atom = new MP4FreeAtom_t ()) == (MP4FreeAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "ftyp"))	// only 3GPP
	{
		if ((*pmaMP4Atom = new MP4FtypAtom_t ()) == (MP4FtypAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "hdlr"))
	{
		if ((*pmaMP4Atom = new MP4HdlrAtom_t ()) == (MP4HdlrAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "hinf"))
	{
		if ((*pmaMP4Atom = new MP4HinfAtom_t ()) == (MP4HinfAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "hint"))
	{
		if ((*pmaMP4Atom = new MP4HintAtom_t ()) == (MP4HintAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "hmhd"))
	{
		if ((*pmaMP4Atom = new MP4HmhdAtom_t ()) == (MP4HmhdAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "hnti"))
	{
		if ((*pmaMP4Atom = new MP4HntiAtom_t ()) == (MP4HntiAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "iods"))
	{
		if ((*pmaMP4Atom = new MP4IodsAtom_t ()) == (MP4IodsAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "ipir"))
	{
		if ((*pmaMP4Atom = new MP4IpirAtom_t ()) == (MP4IpirAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "maxr"))
	{
		if ((*pmaMP4Atom = new MP4MaxrAtom_t ()) == (MP4MaxrAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "mdat"))
	{
		if ((*pmaMP4Atom = new MP4MdatAtom_t ()) == (MP4MdatAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "mdhd"))
	{
		if ((*pmaMP4Atom = new MP4MdhdAtom_t ()) == (MP4MdhdAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "mdia"))
	{
		if ((*pmaMP4Atom = new MP4MdiaAtom_t ()) == (MP4MdiaAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "mfhd"))
	{
		if ((*pmaMP4Atom = new MP4MfhdAtom_t ()) == (MP4MfhdAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "minf"))
	{
		if ((*pmaMP4Atom = new MP4MinfAtom_t ()) == (MP4MinfAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "moof"))
	{
		if ((*pmaMP4Atom = new MP4MoofAtom_t ()) == (MP4MoofAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "moov"))
	{
		if ((*pmaMP4Atom = new MP4MoovAtom_t ()) == (MP4MoovAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "mp4a"))
	{
		if ((*pmaMP4Atom = new MP4Mp4aAtom_t ()) == (MP4Mp4aAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "mp4s"))
	{
		if ((*pmaMP4Atom = new MP4Mp4sAtom_t ()) == (MP4Mp4sAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "mp4v"))
	{
		if ((*pmaMP4Atom = new MP4Mp4vAtom_t ()) == (MP4Mp4vAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "mpod"))
	{
		if ((*pmaMP4Atom = new MP4MpodAtom_t ()) == (MP4MpodAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "mvex"))
	{
		if ((*pmaMP4Atom = new MP4MvexAtom_t ()) == (MP4MvexAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "mvhd"))
	{
		if ((*pmaMP4Atom = new MP4MvhdAtom_t ()) == (MP4MvhdAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "nmhd"))
	{
		if ((*pmaMP4Atom = new MP4NmhdAtom_t ()) == (MP4NmhdAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "nump"))
	{
		if ((*pmaMP4Atom = new MP4NumpAtom_t ()) == (MP4NumpAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "payt"))
	{
		if ((*pmaMP4Atom = new MP4PaytAtom_t ()) == (MP4PaytAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "pmax"))
	{
		if ((*pmaMP4Atom = new MP4PmaxAtom_t ()) == (MP4PmaxAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "rtp "))
	{
		if ((*pmaMP4Atom = new MP4RtpAtom_t ()) == (MP4RtpAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "s263"))	// only 3GPP
	{
		if ((*pmaMP4Atom = new MP4S263Atom_t ()) == (MP4S263Atom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "samr"))	// only 3GPP
	{
		if ((*pmaMP4Atom = new MP4SamrAtom_t ()) == (MP4SamrAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "sawb"))	// only 3GPP
	{
		if ((*pmaMP4Atom = new MP4SawbAtom_t ()) == (MP4SawbAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "sdp "))
	{
		if ((*pmaMP4Atom = new MP4SdpAtom_t ()) == (MP4SdpAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "skip"))
	{
		if ((*pmaMP4Atom = new MP4SkipAtom_t ()) == (MP4SkipAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "smhd"))
	{
		if ((*pmaMP4Atom = new MP4SmhdAtom_t ()) == (MP4SmhdAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "snro"))
	{
		if ((*pmaMP4Atom = new MP4SnroAtom_t ()) == (MP4SnroAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "stbl"))
	{
		if ((*pmaMP4Atom = new MP4StblAtom_t ()) == (MP4StblAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "stco"))
	{
		if ((*pmaMP4Atom = new MP4StcoAtom_t ()) == (MP4StcoAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "stdp"))
	{
		if ((*pmaMP4Atom = new MP4StdpAtom_t ()) == (MP4StdpAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "stsc"))
	{
		if ((*pmaMP4Atom = new MP4StscAtom_t ()) == (MP4StscAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "stsd"))
	{
		if ((*pmaMP4Atom = new MP4StsdAtom_t ()) == (MP4StsdAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "stsh"))
	{
		if ((*pmaMP4Atom = new MP4StshAtom_t ()) == (MP4StshAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "stss"))
	{
		if ((*pmaMP4Atom = new MP4StssAtom_t ()) == (MP4StssAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "stsz"))
	{
		if ((*pmaMP4Atom = new MP4StszAtom_t ()) == (MP4StszAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "stts"))
	{
		if ((*pmaMP4Atom = new MP4SttsAtom_t ()) == (MP4SttsAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "sync"))
	{
		if ((*pmaMP4Atom = new MP4SyncAtom_t ()) == (MP4SyncAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "tfhd"))
	{
		if ((*pmaMP4Atom = new MP4TfhdAtom_t ()) == (MP4TfhdAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "tims"))
	{
		if ((*pmaMP4Atom = new MP4TimsAtom_t ()) == (MP4TimsAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "tkhd"))
	{
		if ((*pmaMP4Atom = new MP4TkhdAtom_t ()) == (MP4TkhdAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "tmax"))
	{
		if ((*pmaMP4Atom = new MP4TmaxAtom_t ()) == (MP4TmaxAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "tmin"))
	{
		if ((*pmaMP4Atom = new MP4TminAtom_t ()) == (MP4TminAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "tpyl"))
	{
		if ((*pmaMP4Atom = new MP4TpylAtom_t ()) == (MP4TpylAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "traf"))
	{
		if ((*pmaMP4Atom = new MP4TrafAtom_t ()) == (MP4TrafAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "trak"))
	{
		if ((*pmaMP4Atom = new MP4TrakAtom_t ()) == (MP4TrakAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "tref"))
	{
		if ((*pmaMP4Atom = new MP4TrefAtom_t ()) == (MP4TrefAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "trex"))
	{
		if ((*pmaMP4Atom = new MP4TrexAtom_t ()) == (MP4TrexAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "trpy"))
	{
		if ((*pmaMP4Atom = new MP4TrpyAtom_t ()) == (MP4TrpyAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "trun"))
	{
		if ((*pmaMP4Atom = new MP4TrunAtom_t ()) == (MP4TrunAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "tsro"))
	{
		if ((*pmaMP4Atom = new MP4TsroAtom_t ()) == (MP4TsroAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "udta"))
	{
		if ((*pmaMP4Atom = new MP4UdtaAtom_t ()) == (MP4UdtaAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "url "))
	{
		if ((*pmaMP4Atom = new MP4UrlAtom_t ()) == (MP4UrlAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "urn "))
	{
		if ((*pmaMP4Atom = new MP4UrnAtom_t ()) == (MP4UrnAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "uuid"))
	{
		if ((*pmaMP4Atom = new MP4UuidAtom_t ()) == (MP4UuidAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "vmhd"))
	{
		if ((*pmaMP4Atom = new MP4VmhdAtom_t ()) == (MP4VmhdAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "wmfs"))
	{
		if ((*pmaMP4Atom = new MP4WmfsAtom_t ()) == (MP4WmfsAtom_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_ATOMTYPEUNKNOWN, 1, pType);
		ptTracer -> trace (Tracer:: TRACER_LWRNG, (const char *) err,
			__FILE__, __LINE__);

		if (bUseMP4ConsistencyCheck)
		{
			return err;
		}
		else
		{
			if ((*pmaMP4Atom = new MP4UnknAtom_t ()) == (MP4UnknAtom_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
		}
	}


	return errNoError;
}


Error MP4Atom:: buildDescriptor (unsigned long ulTag,
	MP4Atom_p *pmaMP4Descriptor, Boolean_t bUseMP4ConsistencyCheck,
	Tracer_p ptTracer)

{

	switch (ulTag)
	{
		case MP4_ESDESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4ESDescr_t ()) ==
				(MP4ESDescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case MP4_DECODERCONFIGDESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4DecoderConfigDescr_t ()) ==
				(MP4DecoderConfigDescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case MP4_DECODERSPECIFICDESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4DecoderSpecificDescr_t ()) ==
				(MP4DecoderSpecificDescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case MP4_SYNCLAYERCONFIGDESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4SyncLayerConfigDescr_t ()) ==
				(MP4SyncLayerConfigDescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
/*
		case MP4_CONTENTIDENTIFICATIONDESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4ContentIdentificationDescr_t ()) ==
				(MP4ContentIdentificationDescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case MP4_MORECONTENTIDENTIFICATIONDESCR_TAG:
			if ((*pmaMP4Descriptor =
				new MP4MoreContentIdentificationDescr_t ()) ==
				(MP4MoreContentIdentificationDescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case MP4_IPIDESCRPOINTER_TAG:
			if ((*pmaMP4Descriptor = new MP4IPIDescrPointer_t ()) ==
				(MP4IPIDescrPointer_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case MP4_IPMPDESCRPOINTER_TAG:
			if ((*pmaMP4Descriptor = new MP4IPMPDescrPointer_t ()) ==
				(MP4IPMPDescrPointer_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case MP4_IPMPDESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4IPMPDescr_t ()) ==
				(MP4IPMPDescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case MP4_QOSDESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4QOSDescr_t ()) ==
				(MP4QOSDescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case MP4_REGISTRATIONDESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4RegistrationDescr_t ()) ==
				(MP4RegistrationDescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
*/
		case MP4_ESIDINCDESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4EsIdIncDescr_t ()) ==
				(MP4EsIdIncDescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
/*
		case MP4_ESIDREFDESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4EsIdRefDescr_t ()) ==
				(MP4EsIdRefDescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
*/
		case MP4_INITIALOBJECTDESCR_TAG:
		case MP4_IODESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4InitialObjectDescr_t ()) ==
				(MP4InitialObjectDescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
/*
		case MP4_IODDESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4IODescr_t ()) ==
				(MP4IODescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
*/
		case MP4_OBJECTDESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4ObjectDescr_t ()) ==
				(MP4ObjectDescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
/*
		case MP4_ODESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4ODescr_t ()) ==
				(MP4ODescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
		case MP4_EXTENDEDPROFILELEVELDESCR_TAG:
			if ((*pmaMP4Descriptor = new MP4ExtendedProfileLevelDescr_t ()) ==
				(MP4ExtendedProfileLevelDescr_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			break;
*/
		default:
			if (ulTag >= MP4_OCISTARTDESCR_TAG &&
				ulTag <= MP4_OCIENDDESCR_TAG)
			{
				switch (ulTag)
				{
					case MP4_CONTENTCLASSIFICATIONDESCR_TAG:
						if ((*pmaMP4Descriptor =
							new MP4ContentClassificationDescr_t ()) ==
							(MP4ContentClassificationDescr_p) NULL)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_NEW_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						break;
/*
					case MP4_KEYWORDDESCR_TAG:
						if ((*pmaMP4Descriptor = new MP4KeywordDescr_t ()) ==
							(MP4KeywordDescr_p) NULL)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_NEW_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						break;
*/
					case MP4_RATINGDESCR_TAG:
						if ((*pmaMP4Descriptor = new MP4RatingDescr_t ()) ==
							(MP4RatingDescr_p) NULL)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_NEW_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						break;
/*
					case MP4_LANGUAGEDESCR_TAG:
						if ((*pmaMP4Descriptor = new MP4LanguageDescr_t ()) ==
							(MP4LanguageDescr_p) NULL)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_NEW_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						break;
					case MP4_SHORTTEXTUALDESCR_TAG:
						if ((*pmaMP4Descriptor =
							new MP4ShortTextualDescr_t ()) ==
							(MP4ShortTextualDescr_p) NULL)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_NEW_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						break;
					case MP4_EXPANDEDTEXTUALDESCR_TAG:
						if ((*pmaMP4Descriptor =
							new MP4ExpandedTextualDescr_t ()) ==
							(MP4ExpandedTextualDescr_p) NULL)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_NEW_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						break;
					case MP4_CONTENTCREATORNAMEDESCR_TAG:
						if ((*pmaMP4Descriptor =
							new MP4ContentCreatorNameDescr_t ()) ==
							(MP4ContentCreatorNameDescr_p) NULL)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_NEW_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						break;
					case MP4_CONTENTCREATIONNAMEDESCR_TAG:
						if ((*pmaMP4Descriptor =
							new MP4ContentCreationNameDescr_t ()) ==
							(MP4ContentCreationNameDescr_p) NULL)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_NEW_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						break;
					case MP4_OCICREATORNAMEDESCR_TAG:
						if ((*pmaMP4Descriptor =
							new MP4OCICreatorNameDescr_t ()) ==
							(MP4OCICreatorNameDescr_p) NULL)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_NEW_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						break;
					case MP4_OCICREATIONDATEDESCR_TAG:
						if ((*pmaMP4Descriptor =
							new MP4OCICreationDateDescr_t ()) ==
							(MP4OCICreationDateDescr_p) NULL)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_NEW_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						break;
					case MP4_SMPTECAMERAPOSITIONDESCR_TAG:
						if ((*pmaMP4Descriptor =
							new MP4SmpteCameraPositionDescr_t ()) ==
							(MP4SmpteCameraPositionDescr_p) NULL)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_NEW_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}

						break;
*/
					default:
						if ((*pmaMP4Descriptor = new MP4UnknownOCIDescr_t ()) ==
							(MP4UnknownOCIDescr_p) NULL)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_NEW_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							return err;
						}
				}
			}
			else if (ulTag >= MP4_EXTENDEDSTARTDESCR_TAGS &&
				ulTag <= MP4_EXTENDEDENDDESCR_TAGS)
			{
				if ((*pmaMP4Descriptor = new MP4ExtensionDescr_t ()) ==
					(MP4ExtensionDescr_p) NULL)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_NEW_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DESCRIPTORTAGUNKNOWN, 1, ulTag);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bUseMP4ConsistencyCheck)
				{
					return err;
				}
				else
				{
					if ((*pmaMP4Descriptor = new MP4UnknownDescr_t ()) ==
						(MP4UnknownDescr_p) NULL)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_NEW_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return err;
					}
				}
			}

			break;
	}


	return errNoError;
}


Error MP4Atom:: getDescriptorTag (const char *pType,
	unsigned long *pulTag, Tracer_p ptTracer)

{

	if (!strcmp (pType, MP4_ESDESCR_TYPE))
		*pulTag			= MP4_ESDESCR_TAG;
	else if (!strcmp (pType, MP4_DECODERCONFIGDESCR_TYPE))
		*pulTag			= MP4_DECODERCONFIGDESCR_TAG;
	else if (!strcmp (pType, MP4_DECODERSPECIFICDESCR_TYPE))
		*pulTag			= MP4_DECODERSPECIFICDESCR_TAG;
	else if (!strcmp (pType, MP4_SYNCLAYERCONFIGDESCR_TYPE))
		*pulTag			= MP4_SYNCLAYERCONFIGDESCR_TAG;
/*
	else if (!strcmp (pType, MP4_CONTENTIDENTIFICATIONDESCR_TYPE))
		*pulTag			= MP4_CONTENTIDENTIFICATIONDESCR_TAG;
	else if (!strcmp (pType, MP4_MORECONTENTIDENTIFICATIONDESCR_TYPE))
		*pulTag			= MP4_MORECONTENTIDENTIFICATIONDESCR_TAG;
	else if (!strcmp (pType, MP4_IPIDESCRPOINTER_TYPE))
		*pulTag			= MP4_IPIDESCRPOINTER_TAG;
	else if (!strcmp (pType, MP4_IPMPDESCRPOINTER_TYPE))
		*pulTag			= MP4_IPMPDESCRPOINTER_TAG;
	else if (!strcmp (pType, MP4_IPMPDESCR_TYPE))
		*pulTag			= MP4_IPMPDESCR_TAG;
	else if (!strcmp (pType, MP4_QOSDESCR_TYPE))
		*pulTag			= MP4_QOSDESCR_TAG;
	else if (!strcmp (pType, MP4_REGISTRATIONDESCR_TYPE))
		*pulTag			= MP4_REGISTRATIONDESCR_TAG;
*/
	else if (!strcmp (pType, MP4_ESIDINCDESCR_TYPE))
		*pulTag			= MP4_ESIDINCDESCR_TAG;
/*
	else if (!strcmp (pType, MP4_ESIDREFDESCR_TYPE))
		*pulTag			= MP4_ESIDREFDESCR_TAG;
*/
	else if (!strcmp (pType, MP4_INITIALOBJECTDESCR_TYPE))
		*pulTag			= MP4_INITIALOBJECTDESCR_TAG;
	else if (!strcmp (pType, MP4_IODESCR_TYPE))
		*pulTag			= MP4_IODESCR_TAG;
/*
	else if (!strcmp (pType, MP4_IODDESCR_TYPE))
		*pulTag			= MP4_IODDESCR_TAG;
*/
	else if (!strcmp (pType, MP4_OBJECTDESCR_TYPE))
		*pulTag			= MP4_OBJECTDESCR_TAG;
/*
	else if (!strcmp (pType, MP4_ODESCR_TYPE))
		*pulTag			= MP4_ODESCR_TAG;
	else if (!strcmp (pType, MP4_EXTENDEDPROFILELEVELDESCR_TYPE))
		*pulTag			= MP4_EXTENDEDPROFILELEVELDESCR_TAG;
*/
	else if (!strcmp (pType, MP4_CONTENTCLASSIFICATIONDESCR_TYPE))
		*pulTag			= MP4_CONTENTCLASSIFICATIONDESCR_TAG;
/*
	else if (!strcmp (pType, MP4_KEYWORDDESCR_TYPE))
		*pulTag			= MP4_KEYWORDDESCR_TAG;
*/
	else if (!strcmp (pType, MP4_RATINGDESCR_TYPE))
		*pulTag			= MP4_RATINGDESCR_TAG;
/*
	else if (!strcmp (pType, MP4_LANGUAGEDESCR_TYPE))
		*pulTag			= MP4_LANGUAGEDESCR_TAG;
	else if (!strcmp (pType, MP4_SHORTTEXTUALDESCR_TYPE))
		*pulTag			= MP4_SHORTTEXTUALDESCR_TAG;
	else if (!strcmp (pType, MP4_EXPANDEDTEXTUALDESCR_TYPE))
		*pulTag			= MP4_EXPANDEDTEXTUALDESCR_TAG;
	else if (!strcmp (pType, MP4_CONTENTCREATORNAMEDESCR_TYPE))
		*pulTag			= MP4_CONTENTCREATORNAMEDESCR_TAG;
	else if (!strcmp (pType, MP4_CONTENTCREATIONNAMEDESCR_TYPE))
		*pulTag			= MP4_CONTENTCREATIONNAMEDESCR_TAG;
	else if (!strcmp (pType, MP4_OCICREATORNAMEDESCR_TYPE))
		*pulTag			= MP4_OCICREATORNAMEDESCR_TAG;
	else if (!strcmp (pType, MP4_OCICREATIONDATEDESCR_TYPE))
		*pulTag			= MP4_OCICREATIONDATEDESCR_TAG;
	else if (!strcmp (pType, MP4_SMPTECAMERAPOSITIONDESCR_TYPE))
		*pulTag			= MP4_SMPTECAMERAPOSITIONDESCR_TAG;
*/
	else
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_DESCRIPTORTYPEUNKNOWN, 1, pType);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

	}


	return errNoError;
}


Error MP4Atom:: isDescriptor (const char *pType, Boolean_p pbIsDescriptor)

{

	unsigned long				ulTypeLength;


	ulTypeLength				= strlen (pType);

	if (ulTypeLength < strlen ("Descr"))
		*pbIsDescriptor				= false;
	else
	{
		if (!strcmp (pType + (ulTypeLength - strlen ("Descr")), "Descr"))
			*pbIsDescriptor					= true;
		else
			*pbIsDescriptor				= false;
	}


	return errNoError;
}

