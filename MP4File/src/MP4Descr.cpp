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


#include "MP4Descr.h"
#include "MP4FileMessages.h"
#include "FileIO.h"
#ifdef WIN32
	#include <stdio.h>
#else
	#include <unistd.h>
#endif
#include <assert.h>



MP4Descr:: MP4Descr (void): MP4Atom ()

{

}


MP4Descr:: ~MP4Descr (void)

{

}



MP4Descr:: MP4Descr (const MP4Descr &)

{

	assert (1==0);

	// to do

}


MP4Descr &MP4Descr:: operator = (const MP4Descr &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4Descr:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile, __int64 ullAtomStartOffset,
		__int64 ullAtomSize, unsigned long ulHeaderSize,
		unsigned char ucNumBytesForDescriptorSize, unsigned long ulTag,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4Descr:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile, unsigned long long ullAtomStartOffset,
		unsigned long long ullAtomSize, unsigned long ulHeaderSize,
		unsigned char ucNumBytesForDescriptorSize, unsigned long ulTag,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#endif

{

	_ucNumBytesForDescriptorSize			= ucNumBytesForDescriptorSize;
	_ulTag									= ulTag;

	return MP4Atom:: init (pmaParentAtom, pfFile,
		ullAtomStartOffset, ullAtomSize, ulHeaderSize,
		pmtMP4File, lAtomLevel, bUse64Bits, bUseMP4ConsistencyCheck,
		sStandard, ptTracer);
}


#ifdef WIN32
	Error MP4Descr:: init (MP4Atom_p pmaParentAtom, FileReader_p pfFile,
		__int64 ullAtomStartOffset, unsigned char ucNumBytesForDescriptorSize,
		unsigned long ulTag, PMutex_p pmtMP4File, long lAtomLevel,
		Boolean_t bUse64Bits, Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer,
		__int64 *pullAtomSize)
#else
	Error MP4Descr:: init (MP4Atom_p pmaParentAtom, FileReader_p pfFile,
		unsigned long long ullAtomStartOffset, unsigned char ucNumBytesForDescriptorSize,
		unsigned long ulTag, PMutex_p pmtMP4File, long lAtomLevel,
		Boolean_t bUse64Bits, Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer,
		unsigned long long *pullAtomSize)
#endif

{

	_ucNumBytesForDescriptorSize			= ucNumBytesForDescriptorSize;
	_ulTag									= ulTag;

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


		_ulHeaderSize					= _ucNumBytesForDescriptorSize + 1;
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

		/*
		if (!strcmp (pType, "uuid"))
		{
			_ullAtomSize					+= MP4F_EXTENDEDTYPELENGTH;
		}
		*/

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
}


Error MP4Descr:: readChildAtoms (void)

{

	unsigned long			ulPropertiesSize;
	unsigned long			ulChildHeaderSize;
	unsigned long			ulChildTag;
	unsigned long			ulChildAtomSize;
	#ifdef WIN32
		__int64					ullChildAtomStartOffset;
		__int64					ullChildAtomSize;
		__int64					llCurrentPosition;
	#else
		unsigned long long		ullChildAtomStartOffset;
		unsigned long long		ullChildAtomSize;
		long long				llCurrentPosition;
	#endif
	char					pType [MP4_MAXTYPELENGTH];
	MP4Atom_p				pmaMP4ChildDescriptor;
	unsigned char			ucNumBytesForDescriptorSize;
	unsigned long			ulStartTag;
	unsigned long			ulEndTag;
	Error_t					errReadChildTag;


	if (getPropertiesSize (&ulPropertiesSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETPROPERTIESSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (getType (pType) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	ullChildAtomStartOffset			= _ullAtomStartOffset +
		_ulHeaderSize + ulPropertiesSize;

	if (_pfFile -> seek (ullChildAtomStartOffset, SEEK_SET,
		&llCurrentPosition) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEREADER_SEEK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (ullChildAtomStartOffset < _ullAtomStartOffset + _ullAtomSize)
	{
		if (_pfFile -> readNetUnsignedInt8Bit (&ulChildTag) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_READNETUNSIGNEDINT8BIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		switch (ulChildTag)
		{
			case MP4_OCISTARTDESCR_TAG:
				ulStartTag			= ulChildTag;
				ulEndTag			= MP4_OCIENDDESCR_TAG;

				break;
			case MP4_EXTENDEDSTARTDESCR_TAGS:
				ulStartTag			= ulChildTag;
				ulEndTag			= MP4_EXTENDEDENDDESCR_TAGS;

				break;
			case MP4_CONTENTIDENTIFICATIONDESCR_TAG:
				ulStartTag			= ulChildTag;
				ulEndTag			= MP4_MORECONTENTIDENTIFICATIONDESCR_TAG;

				break;
			case MP4_IODESCR_TAG:
				ulStartTag			= ulChildTag;
				ulEndTag			= MP4_ODESCR_TAG;

				break;
			default:
				ulStartTag			= ulChildTag;
				ulEndTag			= ulChildTag;

				break;
		}

		while (ulChildTag >= ulStartTag && ulChildTag <= ulEndTag ||
			ullChildAtomStartOffset < _ullAtomStartOffset + _ullAtomSize)
		{
			// readDescriptorSize: from 1 to 4 bytes
			if (_pfFile -> readMP4DescriptorSize (&ulChildAtomSize,
				&ucNumBytesForDescriptorSize) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEREADER_READMP4DESCRIPTORSIZE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			ulChildHeaderSize		= 1 + ucNumBytesForDescriptorSize;

			ullChildAtomSize					= ulChildAtomSize +
				ulChildHeaderSize;

			if (MP4Atom:: buildDescriptor (ulChildTag,
				&pmaMP4ChildDescriptor, _bUseMP4ConsistencyCheck, _ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_BUILDDESCRIPTOR_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if (pmaMP4ChildDescriptor -> getType (pType) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_GETTYPE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				delete pmaMP4ChildDescriptor;
				pmaMP4ChildDescriptor				= (MP4Descr_p) NULL;

				return err;
			}

			{
				char				pCurrentType [MP4_MAXTYPELENGTH];

				getType (pCurrentType);
				Message msg = MP4FileMessages (__FILE__, __LINE__,
					MP4F_MP4ATOM_FOUNDDESCRATOM, 5,
					pType, ullChildAtomStartOffset, ullChildAtomSize,
					(long) ucNumBytesForDescriptorSize, pCurrentType);
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

					delete pmaMP4ChildDescriptor;
					pmaMP4ChildDescriptor				= (MP4Atom_p) NULL;

					return err;
				}
			}

			if (((MP4Descr_p) pmaMP4ChildDescriptor) -> init (this,
				_pfFile, ullChildAtomStartOffset, ullChildAtomSize,
				ulChildHeaderSize,
				ucNumBytesForDescriptorSize, ulChildTag, _pmtMP4File,
				_lAtomLevel + 1,
				_bUse64Bits, _bUseMP4ConsistencyCheck, _sStandard,
				_ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				delete pmaMP4ChildDescriptor;
				pmaMP4ChildDescriptor				= (MP4Atom_p) NULL;

				return err;
			}

			_vChildrenAtoms. insert (_vChildrenAtoms. end (),
				pmaMP4ChildDescriptor);

			ullChildAtomStartOffset			+= ullChildAtomSize;

			if (_pfFile -> seek (ullChildAtomStartOffset, SEEK_SET,
				&llCurrentPosition) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEREADER_SEEK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if ((errReadChildTag = _pfFile -> readNetUnsignedInt8Bit (
				&ulChildTag)) != errNoError)
			{
				if ((long) errReadChildTag != TOOLS_ACTIVATION_WRONG)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEREADER_READNETUNSIGNEDINT8BIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
				else
					// The file is finished and theren't anything to read
					break;
			}
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


Error MP4Descr:: getTag (unsigned long *pulTag)

{

	*pulTag						= _ulTag;


	return errNoError;
}


Error MP4Descr:: write (int iFileDescriptor)

{

	char										pType [MP4_MAXTYPELENGTH];
	unsigned long								ulTag;
	#ifdef WIN32
		__int64									ullAtomSize;
	#else
		unsigned long long						ullAtomSize;
	#endif
	std:: vector<MP4Atom_p>:: const_iterator	it;
	MP4Atom_p									pmaMp4Atom;
	#ifdef WIN32
		__int64									llCurrentPosition;
	#else
		long long								llCurrentPosition;
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

	if (!strcmp (pType, "esds") || !strcmp (pType, "iods"))
	{
		if (MP4Atom:: write (iFileDescriptor) != errNoError)
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

		if (_pmtMP4File -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}

	if (FileIO:: seek (iFileDescriptor, _ullAtomStartOffset, SEEK_SET,
		&llCurrentPosition) != errNoError)
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

	if (getTag (&ulTag) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4DESCR_GETTAG_FAILED);
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

	if (FileIO:: writeNetUnsignedInt8Bit (iFileDescriptor, ulTag) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_WRITENETUNSIGNEDINT8BIT_FAILED);
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

	ullAtomSize						= _ullAtomSize;

	// writeDescriptorSize: from 1 to 4 bytes
	if (FileIO:: writeMP4DescriptorSize (iFileDescriptor, ullAtomSize -
		1 - _ucNumBytesForDescriptorSize, false,
		_ucNumBytesForDescriptorSize) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_WRITEMP4DESCRIPTORSIZE_FAILED);
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

