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


#include "MP4RtpAtom.h"
#include "FileIO.h"
#include "MP4UuidAtom.h"
#ifdef WIN32
	#include <stdio.h>
#else
	#include <unistd.h>
#endif
#include <assert.h>



MP4RtpAtom:: MP4RtpAtom (void): MP4Atom ()

{

}


MP4RtpAtom:: ~MP4RtpAtom (void)

{

}



MP4RtpAtom:: MP4RtpAtom (const MP4RtpAtom &)

{

	assert (1==0);

	// to do

}


MP4RtpAtom &MP4RtpAtom:: operator = (const MP4RtpAtom &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4RtpAtom:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile,
		__int64 ullAtomStartOffset, __int64 ullAtomSize,
		unsigned long ulHeaderSize,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4RtpAtom:: init (MP4Atom_p pmaParentAtom,
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


Error MP4RtpAtom:: prepareChildrensAtomsInfo (void)

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

	if (!strcmp (pType, "stsd"))
	{
		strcpy (ctiChildrenAtomInfo. _pType, "tims");
		ctiChildrenAtomInfo. _bMandatory		= true;
		ctiChildrenAtomInfo. _bOnlyOne			= true;

		_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
			ctiChildrenAtomInfo);

		strcpy (ctiChildrenAtomInfo. _pType, "tsro");
		ctiChildrenAtomInfo. _bMandatory		= false;
		ctiChildrenAtomInfo. _bOnlyOne			= true;

		_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
			ctiChildrenAtomInfo);

		strcpy (ctiChildrenAtomInfo. _pType, "snro");
		ctiChildrenAtomInfo. _bMandatory		= false;
		ctiChildrenAtomInfo. _bOnlyOne			= true;

		_vChildrensAtomsInfo. insert (_vChildrensAtomsInfo. end (),
			ctiChildrenAtomInfo);
	}
	else		// pType == "hnti"
	{
	}


	return errNoError;
}


Error MP4RtpAtom:: createProperties (
	unsigned long *pulPropertiesSize)

{

	unsigned long		ulValue;
	char				pType [MP4_MAXTYPELENGTH];
	Boolean_t			bIsImplicitProperty;


	if (_pmaParentAtom -> getType (pType) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	ulValue							= 0;
	bIsImplicitProperty				= false;

	if (!strcmp (pType, "stsd"))
	{
		// 0: Reserved1
		/*
		{
			unsigned char				**pucReserved1;


			if ((pucReserved1 = new unsigned char * [1]) ==
				(unsigned char **) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if ((pucReserved1 [0] = new unsigned char [6]) ==
				(unsigned char *) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				delete [] pucReserved1;
				pucReserved1			= (unsigned char **) NULL;

				return err;
			}

			memset (pucReserved1 [0], 0, 6);
			if (_mbpReserved1. init ("Reserved1", 1, &bIsImplicitProperty,
				6, pucReserved1, _ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4BYTESPROPERTY_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				delete [] pucReserved1 [0];
				pucReserved1 [0]			= (unsigned char *) NULL;

				delete [] pucReserved1;
				pucReserved1				= (unsigned char **) NULL;

				return err;
			}

			delete [] pucReserved1 [0];
			pucReserved1 [0]			= (unsigned char *) NULL;

			delete [] pucReserved1;
			pucReserved1				= (unsigned char **) NULL;
		}
		*/
		{
			unsigned char				*pucReserved1 [1]	=
				{ (unsigned char *) "\0\0\0\0\0\0" };


			if (_mbpReserved1. init ("Reserved1", 1, &bIsImplicitProperty,
				6, pucReserved1, _ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4BYTESPROPERTY_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
		}

		// 1: DataReferenceIndex
		ulValue							= 1;
		if (_mui16pDataReferenceIndex. init ("DataReferenceIndex", 1,
			&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 2: HintTrakVersion
		ulValue							= 1;
		if (_mui16pHintTrakVersion. init ("HintTrakVersion", 1,
			&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 3: HighestCompatibleHintTrakVersion
		ulValue							= 1;
		if (_mui16pHighestCompatibleHintTrakVersion. init (
			"HighestCompatibleHintTrakVersion", 1,
			&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 4: MaxPacketSize
		ulValue							= 0;
		if (_mui32pMaxPacketSize. init ("MaxPacketSize",
			1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "hnti"))
	{
		unsigned long					ulBytesNumber;


		// 0: DescriptionFormat
		/*
		{
			char								**pDescriptionFormat;


			if ((pDescriptionFormat = new char * [1]) == (char **) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			if ((pDescriptionFormat [0] = new char [4]) == (char *) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_NEW_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				delete [] pDescriptionFormat;
				pDescriptionFormat			= (char **) NULL;

				return err;
			}

			strcpy (pDescriptionFormat [0], "sdp ");
			if (_mstrfpDescriptionFormat. init ("DescriptionFormat", 1,
				&bIsImplicitProperty, 4, pDescriptionFormat, _ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4STRINGFIXEDPROPERTY_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				delete [] pDescriptionFormat [0];
				pDescriptionFormat [0]			= (char *) NULL;

				delete [] pDescriptionFormat;
				pDescriptionFormat			= (char **) NULL;

				return err;
			}

			delete [] pDescriptionFormat [0];
			pDescriptionFormat [0]			= (char *) NULL;

			delete [] pDescriptionFormat;
			pDescriptionFormat			= (char **) NULL;
		}
		*/
		{
			char								*pDescriptionFormat [1]	=
				{ "sdp " };


			if (_mstrfpDescriptionFormat. init ("DescriptionFormat", 1,
				&bIsImplicitProperty, 4, pDescriptionFormat, _ptTracer) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4STRINGFIXEDPROPERTY_INIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}
		}

		// 1: SdpText
		if (_mstrlpSdpText. init ("SdpText", &bIsImplicitProperty,
			&ulBytesNumber, "", _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STRINGLASTPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_WRONGATOMPARENT, 1, pType);
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


Error MP4RtpAtom:: readProperties (
	unsigned long *pulPropertiesSize)

{

	char								pParentType [MP4_MAXTYPELENGTH];
	MP4Atom_p							pmaMP4ParentAtom;
	#ifdef WIN32
		__int64							ullCharsNumber;
	#else
		unsigned long long				ullCharsNumber;
	#endif
	unsigned long					ulLocalSize;
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
		ullCharsNumber					= _ullAtomSize -
			_ulHeaderSize - MP4F_EXTENDEDTYPELENGTH; 
	}
	else
	{
		ullCharsNumber					= _ullAtomSize - _ulHeaderSize; 
	}

	if (!strcmp (pParentType, "stsd"))
	{
		// 0: Reserved1
		if (_mbpReserved1. init ("Reserved1", 1,
			6, _pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 1: DataReferenceIndex
		if (_mui16pDataReferenceIndex. init ("DataReferenceIndex", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 2: HintTrakVersion
		if (_mui16pHintTrakVersion. init ("HintTrakVersion", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 3: HighestCompatibleHintTrakVersion
		if (_mui16pHighestCompatibleHintTrakVersion. init (
			"HighestCompatibleHintTrakVersion", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 4: MaxPacketSize
		if (_mui32pMaxPacketSize. init ("MaxPacketSize",
			1, _pfFile, _ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pParentType, "hnti"))
	{
		unsigned long					ulBytesNumber;


		// 0: DescriptionFormat
		if (_mstrfpDescriptionFormat. init ("DescriptionFormat", 1,
			4, _pfFile, _ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STRINGFIXEDPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		if (_mstrfpDescriptionFormat. getSize (&ulLocalSize) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STRINGFIXEDPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		ullCharsNumber			-= ulLocalSize;

		// 1: SdpText
		if (_mstrlpSdpText. init ("SdpText", (unsigned long) ullCharsNumber,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STRINGLASTPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_WRONGATOMPARENT, 1, pParentType);
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


Error MP4RtpAtom:: writeProperties (int iFileDescriptor)

{

	char							pType [MP4_MAXTYPELENGTH];
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

	if (_pmaParentAtom -> getType (pType) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (!strcmp (pType, "stsd"))
	{
		if (_mbpReserved1. write (iFileDescriptor) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (_mui16pDataReferenceIndex. write (iFileDescriptor) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (_mui16pHintTrakVersion. write (iFileDescriptor) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (_mui16pHighestCompatibleHintTrakVersion. write (iFileDescriptor) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (_mui32pMaxPacketSize. write (iFileDescriptor) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "hnti"))
	{
		if (_mstrfpDescriptionFormat. write (iFileDescriptor) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STRINGFIXEDPROPERTY_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (_mstrlpSdpText. write (iFileDescriptor) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STRINGLASTPROPERTY_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_WRONGATOMPARENT, 1, pType);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4RtpAtom:: getPropertiesSize (unsigned long *pulPropertiesSize)

{

	unsigned long				ulSize;
	char						pType [MP4_MAXTYPELENGTH];


	*pulPropertiesSize					= 0;


	if (_pmaParentAtom -> getType (pType) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (!strcmp (pType, "stsd"))
	{
		if (_mbpReserved1. getSize (&ulSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulPropertiesSize					+= ulSize;

		if (_mui16pDataReferenceIndex. getSize (&ulSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulPropertiesSize					+= ulSize;

		if (_mui16pHintTrakVersion. getSize (&ulSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulPropertiesSize					+= ulSize;

		if (_mui16pHighestCompatibleHintTrakVersion. getSize (&ulSize) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulPropertiesSize					+= ulSize;

		if (_mui32pMaxPacketSize. getSize (&ulSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulPropertiesSize					+= ulSize;
	}
	else if (!strcmp (pType, "hnti"))
	{
		if (_mstrfpDescriptionFormat. getSize (&ulSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STRINGFIXEDPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulPropertiesSize					+= ulSize;

		if (_mstrlpSdpText. getSize (&ulSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STRINGLASTPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulPropertiesSize					+= ulSize;
	}
	else
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_WRONGATOMPARENT, 1, pType);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4RtpAtom:: getType (char *pType)

{
	strcpy (pType, "rtp ");


	return errNoError;
}


Error MP4RtpAtom:: appendPropertiesDump (Buffer_p pbBuffer)

{

	long						lLevelIndex;
	char						pType [MP4_MAXTYPELENGTH];


	if (_pmaParentAtom -> getType (pType) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (!strcmp (pType, "stsd"))
	{
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

		if (_mbpReserved1. appendDump (pbBuffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_APPENDDUMP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

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

		if (_mui16pDataReferenceIndex. appendDump (pbBuffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_APPENDDUMP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

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

		if (_mui16pHintTrakVersion. appendDump (pbBuffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_APPENDDUMP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

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

		if (_mui16pHighestCompatibleHintTrakVersion. appendDump (pbBuffer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_APPENDDUMP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

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

		if (_mui32pMaxPacketSize. appendDump (pbBuffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_APPENDDUMP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "hnti"))
	{
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

		if (_mstrfpDescriptionFormat. appendDump (pbBuffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STRINGFIXEDPROPERTY_APPENDDUMP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

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

		if (_mstrlpSdpText. appendDump (pbBuffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STRINGLASTPROPERTY_APPENDDUMP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_WRONGATOMPARENT, 1, pType);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4RtpAtom:: printPropertiesOnStdOutput (void)

{

	long						lLevelIndex;
	char						pType [MP4_MAXTYPELENGTH];


	if (_pmaParentAtom -> getType (pType) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (!strcmp (pType, "stsd"))
	{
		for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			std:: cout << "  ";

		if (_mbpReserved1. printOnStdOutput () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_PRINTONSTDOUTPUT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			std:: cout << "  ";

		if (_mui16pDataReferenceIndex. printOnStdOutput () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			std:: cout << "  ";

		if (_mui16pHintTrakVersion. printOnStdOutput () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			std:: cout << "  ";

		if (_mui16pHighestCompatibleHintTrakVersion. printOnStdOutput () !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER16BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			std:: cout << "  ";

		if (_mui32pMaxPacketSize. printOnStdOutput () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else if (!strcmp (pType, "hnti"))
	{
		for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			std:: cout << "  ";

		if (_mstrfpDescriptionFormat. printOnStdOutput () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STRINGFIXEDPROPERTY_PRINTONSTDOUTPUT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			std:: cout << "  ";

		if (_mstrlpSdpText. printOnStdOutput () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STRINGLASTPROPERTY_PRINTONSTDOUTPUT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_WRONGATOMPARENT, 1, pType);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4RtpAtom:: getDescriptionFormat (Buffer_p pbDescriptionFormat)

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mstrfpDescriptionFormat. getValue (pbDescriptionFormat, 0) !=
		errNoError)
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


Error MP4RtpAtom:: getSdpText (Buffer_p pbSdpText)

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mstrlpSdpText. getValue (pbSdpText, 0) != errNoError)
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


Error MP4RtpAtom:: getMaxPacketSize (unsigned long *pulMaxPacketSize)

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui32pMaxPacketSize. getValue (pulMaxPacketSize, 0) != errNoError)
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


Error MP4RtpAtom:: setMaxPacketSize (unsigned long ulMaxPacketSize)

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

	if (_mui32pMaxPacketSize. setValue (ulMaxPacketSize, 0,
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

