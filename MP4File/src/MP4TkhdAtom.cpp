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


#include "MP4TkhdAtom.h"
#include "FileIO.h"
#ifdef WIN32
	#include <stdio.h>
	#include <windows.h>
#else
	#include <unistd.h>
	#include <sys/time.h>
#endif
#include <assert.h>



MP4TkhdAtom:: MP4TkhdAtom (void): MP4Atom ()

{

}


MP4TkhdAtom:: ~MP4TkhdAtom (void)

{

}



MP4TkhdAtom:: MP4TkhdAtom (const MP4TkhdAtom &)

{

	assert (1==0);

	// to do

}


MP4TkhdAtom &MP4TkhdAtom:: operator = (const MP4TkhdAtom &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4TkhdAtom:: init (MP4Atom_p pmaParentAtom,
		FileReader_p pfFile,
		__int64 ullAtomStartOffset, __int64 ullAtomSize,
		unsigned long ulHeaderSize,
		PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
		Boolean_t bUseMP4ConsistencyCheck,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4TkhdAtom:: init (MP4Atom_p pmaParentAtom,
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


Error MP4TkhdAtom:: prepareChildrensAtomsInfo (void)

{

	return errNoError;
}


Error MP4TkhdAtom:: createProperties (
	unsigned long *pulPropertiesSize)

{

	unsigned long				ulValue;
	#ifdef WIN32
		__int64						ullValue;
	#else
		unsigned long long			ullValue;
	#endif
	float						fValue;
	Boolean_t					bIsImplicitProperty;
	#ifdef WIN32
		DWORD						tv;
	#else
		struct timeval				tv;
	#endif


	ulValue							= 0;
	ullValue						= 0;
	fValue							= 0.0;
	bIsImplicitProperty				= false;

	// 0: Version
	ulValue							= _bUse64Bits ? 1 : 0;
	if (_mui8pVersion. init ("Version", 1, &bIsImplicitProperty,
		&ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	ulValue							= 0;

	// 1: Flags
	if (_mui24pFlags. init ("Flags", 1, &bIsImplicitProperty, &ulValue,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER24BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	#ifdef WIN32
		tv			= GetTickCount ();
	#else
		if (gettimeofday (&tv, NULL) == -1)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_GETTIMEOFDAY_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	#endif

	if (_bUse64Bits)
	{
		#ifdef WIN32
			ullValue	= tv / 1000 + 209606400;	// MP4 start date is 1/1/1904
		#else
			ullValue	= tv. tv_sec + 209606400;	// MP4 start date is 1/1/1904
		#endif

		// 2: CreationTime
		if (_mui64pCreationTime. init ("CreationTime", 1, &bIsImplicitProperty,
			&ullValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 3: ModificationTime
		if (_mui64pModificationTime. init ("ModificationTime", 1,
			&bIsImplicitProperty, &ullValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		ullValue						= 0;
	}
	else
	{
		#ifdef WIN32
			ulValue	= tv / 1000 + 209606400;	// MP4 start date is 1/1/1904
		#else
			ulValue	= tv. tv_sec + 209606400;	// MP4 start date is 1/1/1904
		#endif

		// 2: CreationTime
		if (_mui32pCreationTime. init ("CreationTime",
			1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 3: ModificationTime
		if (_mui32pModificationTime. init ("ModificationTime",
			1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		ulValue							= 0;
	}

	// 4: TrackID
	if (_mui32pTrackID. init ("TrackID",
		1, &bIsImplicitProperty, &ulValue, _ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 5: Reserved1
	/*
	{
		unsigned char				**pucReserved1;


		if ((pucReserved1 = new unsigned char * [1]) == (unsigned char **) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if ((pucReserved1 [0] = new unsigned char [4]) ==
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

		memset (pucReserved1 [0], 0, 4);
		if (_mbpReserved1. init ("Reserved1", 1, &bIsImplicitProperty,
			4, pucReserved1, _ptTracer) != errNoError)
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
			{ (unsigned char *) "\0\0\0\0" };


		if (_mbpReserved1. init ("Reserved1", 1, &bIsImplicitProperty,
			4, pucReserved1, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	if (_bUse64Bits)
	{
		// 6: Duration
		if (_mui64pDuration. init ("Duration", 1,
			&bIsImplicitProperty, &ullValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		// 6: Duration
		if (_mui32pDuration. init ("Duration", 1,
			&bIsImplicitProperty, &ulValue, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	// 7: Reserved2
	/*
	{
		unsigned char				**pucReserved2;


		if ((pucReserved2 = new unsigned char * [1]) == (unsigned char **) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if ((pucReserved2 [0] = new unsigned char [12]) ==
			(unsigned char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucReserved2;
			pucReserved2			= (unsigned char **) NULL;

			return err;
		}

		memset (pucReserved2 [0], 0, 12);
		if (_mbpReserved2. init ("Reserved2", 1, &bIsImplicitProperty,
			12, pucReserved2, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucReserved2 [0];
			pucReserved2 [0]			= (unsigned char *) NULL;

			delete [] pucReserved2;
			pucReserved2				= (unsigned char **) NULL;

			return err;
		}

		delete [] pucReserved2 [0];
		pucReserved2 [0]			= (unsigned char *) NULL;

		delete [] pucReserved2;
		pucReserved2				= (unsigned char **) NULL;
	}
	*/
	{
		unsigned char				*pucReserved2 [1]	=
			{ (unsigned char *) "\0\0\0\0\0\0\0\0\0\0\0\0" };


		if (_mbpReserved2. init ("Reserved2", 1, &bIsImplicitProperty,
			12, pucReserved2, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	// 8: Volume
	if (_mf16pVolume. init ("Volume",
		1, &bIsImplicitProperty, &fValue, _ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 9: Reserved3
	/*
	{
		unsigned char				**pucReserved3;


		if ((pucReserved3 = new unsigned char * [1]) == (unsigned char **) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if ((pucReserved3 [0] = new unsigned char [38]) ==
			(unsigned char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucReserved3;
			pucReserved3			= (unsigned char **) NULL;

			return err;
		}

		memset (pucReserved3 [0], 0, 38);
		pucReserved3 [0][3]				= 0x01;
		pucReserved3 [0][19]			= 0x01;
		pucReserved3 [0][34]			= 0x40;
		if (_mbpReserved3. init ("Reserved3", 1, &bIsImplicitProperty,
			38, pucReserved3, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucReserved3 [0];
			pucReserved3 [0]			= (unsigned char *) NULL;

			delete [] pucReserved3;
			pucReserved3				= (unsigned char **) NULL;

			return err;
		}

		delete [] pucReserved3 [0];
		pucReserved3 [0]			= (unsigned char *) NULL;

		delete [] pucReserved3;
		pucReserved3				= (unsigned char **) NULL;
	}
	*/
	{
		unsigned char				*pucReserved3 [1];


		if ((pucReserved3 [0] = new unsigned char [38]) ==
			(unsigned char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		memset (pucReserved3 [0], 0, 38);
		pucReserved3 [0][3]				= 0x01;
		pucReserved3 [0][19]			= 0x01;
		pucReserved3 [0][34]			= 0x40;
		if (_mbpReserved3. init ("Reserved3", 1, &bIsImplicitProperty,
			38, pucReserved3, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucReserved3 [0];
			pucReserved3 [0]			= (unsigned char *) NULL;

			return err;
		}

		delete [] pucReserved3 [0];
		pucReserved3 [0]			= (unsigned char *) NULL;
	}

	// 10: TrackWidth
	if (_mf32pTrackWidth. init ("TrackWidth",
		1, &bIsImplicitProperty, &fValue, _ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 11: TrackHeight
	if (_mf32pTrackHeight. init ("TrackHeight",
		1, &bIsImplicitProperty, &fValue, _ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT32BITSPROPERTY_INIT_FAILED);
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


Error MP4TkhdAtom:: readProperties (
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

	// 0: Version
	if (_mui8pVersion. init ("Version", 1, _pfFile,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 1: Flags
	if (_mui24pFlags. init ("Flags", 1, _pfFile,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER24BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_bUse64Bits)
	{
		// 2: CreationTime
		if (_mui64pCreationTime. init ("CreationTime", 1, _pfFile,
			_ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 3: ModificationTime
		if (_mui64pModificationTime. init ("ModificationTime", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		// 2: CreationTime
		if (_mui32pCreationTime. init ("CreationTime",
			1, _pfFile, _ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// 3: ModificationTime
		if (_mui32pModificationTime. init ("ModificationTime",
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

	// 4: TrackID
	if (_mui32pTrackID. init ("TrackID",
		1, _pfFile, _ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 5: Reserved1
	if (_mbpReserved1. init ("Reserved1", 1,
		4, _pfFile, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_bUse64Bits)
	{
		// 6: Duration
		if (_mui64pDuration. init ("Duration", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		// 6: Duration
		if (_mui32pDuration. init ("Duration", 1,
			_pfFile, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	// 7: Reserved2
	if (_mbpReserved2. init ("Reserved2", 1,
		12, _pfFile, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 8: Volume
	if (_mf16pVolume. init ("Volume",
		1, _pfFile, _ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT16BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 9: Reserved3
	if (_mbpReserved3. init ("Reserved3", 1,
		38, _pfFile, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 10: TrackWidth
	if (_mf32pTrackWidth. init ("TrackWidth",
		1, _pfFile, _ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT32BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 11: TrackHeight
	if (_mf32pTrackHeight. init ("TrackHeight",
		1, _pfFile, _ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT32BITSPROPERTY_INIT_FAILED);
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


Error MP4TkhdAtom:: writeProperties (int iFileDescriptor)

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

	if (_mui8pVersion. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui24pFlags. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER24BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_bUse64Bits)
	{
		if (_mui64pCreationTime. write (iFileDescriptor) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (_mui64pModificationTime. write (iFileDescriptor) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		if (_mui32pCreationTime. write (iFileDescriptor) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if (_mui32pModificationTime. write (iFileDescriptor) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	if (_mui32pTrackID. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mbpReserved1. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_bUse64Bits)
	{
		if (_mui64pDuration. write (iFileDescriptor) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		if (_mui32pDuration. write (iFileDescriptor) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_WRITE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	if (_mbpReserved2. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mf16pVolume. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT16BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mbpReserved3. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mf32pTrackWidth. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT32BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mf32pTrackHeight. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT32BITSPROPERTY_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TkhdAtom:: appendPropertiesDump (Buffer_p pbBuffer)

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

	if (_mui8pVersion. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_APPENDDUMP_FAILED);
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

	if (_mui24pFlags. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER24BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_bUse64Bits)
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

		if (_mui64pCreationTime. appendDump (pbBuffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_APPENDDUMP_FAILED);
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

		if (_mui64pModificationTime. appendDump (pbBuffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_APPENDDUMP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
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

		if (_mui32pCreationTime. appendDump (pbBuffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_APPENDDUMP_FAILED);
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

		if (_mui32pModificationTime. appendDump (pbBuffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_APPENDDUMP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
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

	if (_mui32pTrackID. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_APPENDDUMP_FAILED);
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

	if (_mbpReserved1. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_bUse64Bits)
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

		if (_mui64pDuration. appendDump (pbBuffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_APPENDDUMP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
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

		if (_mui32pDuration. appendDump (pbBuffer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_APPENDDUMP_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
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

	if (_mbpReserved2. appendDump (pbBuffer) != errNoError)
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

	if (_mf16pVolume. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT16BITSPROPERTY_APPENDDUMP_FAILED);
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

	if (_mbpReserved3. appendDump (pbBuffer) != errNoError)
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

	if (_mf32pTrackWidth. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT32BITSPROPERTY_APPENDDUMP_FAILED);
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

	if (_mf32pTrackHeight. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT32BITSPROPERTY_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TkhdAtom:: printPropertiesOnStdOutput (void)

{

	long						lLevelIndex;


	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mui8pVersion. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mui24pFlags. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER24BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_bUse64Bits)
	{
		for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			std:: cout << "  ";

		if (_mui64pCreationTime. printOnStdOutput () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			std:: cout << "  ";

		if (_mui64pModificationTime. printOnStdOutput () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			std:: cout << "  ";

		if (_mui32pCreationTime. printOnStdOutput () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			std:: cout << "  ";

		if (_mui32pModificationTime. printOnStdOutput () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mui32pTrackID. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

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

	if (_bUse64Bits)
	{
		for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			std:: cout << "  ";

		if (_mui64pDuration. printOnStdOutput () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
			std:: cout << "  ";

		if (_mui32pDuration. printOnStdOutput () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mbpReserved2. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mf16pVolume. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT16BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mbpReserved3. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mf32pTrackWidth. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lLevelIndex = 0; lLevelIndex < _lAtomLevel + 1; lLevelIndex++)
		std:: cout << "  ";

	if (_mf32pTrackHeight. printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT32BITSPROPERTY_PRINTONSTDOUTPUT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TkhdAtom:: getPropertiesSize (unsigned long *pulPropertiesSize)

{

	unsigned long				ulSize;


	*pulPropertiesSize					= 0;

	if (_mui8pVersion. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mui24pFlags. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER24BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_bUse64Bits)
	{
		if (_mui64pCreationTime. getSize (&ulSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulPropertiesSize					+= ulSize;

		if (_mui64pModificationTime. getSize (&ulSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulPropertiesSize					+= ulSize;
	}
	else
	{
		if (_mui32pCreationTime. getSize (&ulSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulPropertiesSize					+= ulSize;

		if (_mui32pModificationTime. getSize (&ulSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulPropertiesSize					+= ulSize;
	}

	if (_mui32pTrackID. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mbpReserved1. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_bUse64Bits)
	{
		if (_mui64pDuration. getSize (&ulSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER64BITSPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulPropertiesSize					+= ulSize;
	}
	else
	{
		if (_mui32pDuration. getSize (&ulSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UINTEGER32BITSPROPERTY_GETSIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
		*pulPropertiesSize					+= ulSize;
	}

	if (_mbpReserved2. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mf16pVolume. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mbpReserved3. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mf32pTrackWidth. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mf32pTrackHeight. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FLOAT32BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;


	return errNoError;
}


Error MP4TkhdAtom:: getType (char *pType)

{
	strcpy (pType, "tkhd");


	return errNoError;
}


Error MP4TkhdAtom:: getTrackID (unsigned long *pulTrackID)

{

	if (_pmtMP4File -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mui32pTrackID. getValue (pulTrackID, 0) != errNoError)
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


Error MP4TkhdAtom:: setTrackID (unsigned long ulTrackID)

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

	if (_mui32pTrackID. setValue (ulTrackID, 0, &ulSizeChangedInBytes) !=
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
	Error MP4TkhdAtom:: getDuration (__int64 *pullDuration)
#else
	Error MP4TkhdAtom:: getDuration (unsigned long long *pullDuration)
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

	if (_bUse64Bits)
	{
		if (_mui64pDuration. getValue (pullDuration, 0) != errNoError)
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
	}
	else
	{
		unsigned long					ulDuration;


		if (_mui32pDuration. getValue (&ulDuration, 0) != errNoError)
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

		*pullDuration				= ulDuration;
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
	Error MP4TkhdAtom:: setDuration (__int64 ullDuration)
#else
	Error MP4TkhdAtom:: setDuration (unsigned long long ullDuration)
#endif

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

	if (_bUse64Bits)
	{
		if (_mui64pDuration. setValue (ullDuration, 0, &ulSizeChangedInBytes) !=
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
	}
	else
	{
		if (_mui32pDuration. setValue ((unsigned long) ullDuration,
			0, &ulSizeChangedInBytes) != errNoError)
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
	Error MP4TkhdAtom:: getModificationTime (
		__int64 *pullModificationTime)
#else
	Error MP4TkhdAtom:: getModificationTime (
		unsigned long long *pullModificationTime)
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

	if (_bUse64Bits)
	{
		if (_mui64pModificationTime. getValue (pullModificationTime, 0) !=
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
	}
	else
	{
		unsigned long					ulModificationTime;


		if (_mui32pModificationTime. getValue (&ulModificationTime, 0) !=
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

		*pullModificationTime				= ulModificationTime;
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
	Error MP4TkhdAtom:: setModificationTime (__int64 ullModificationTime)
#else
	Error MP4TkhdAtom:: setModificationTime (unsigned long long ullModificationTime)
#endif

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

	if (_bUse64Bits)
	{
		if (_mui64pModificationTime. setValue (ullModificationTime, 0,
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
	}
	else
	{
		if (_mui32pModificationTime. setValue (
			(unsigned long) ullModificationTime, 0,
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

