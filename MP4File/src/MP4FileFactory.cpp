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


#include "MP4FileFactory.h"
#include "FileIO.h"
#include <assert.h>
#include <iostream>



#ifdef SLACK_LIB
	void hashMapDestroy (void *item)
	{
	}
#else
	/* old
	bool eqstr:: operator()(const char* s1, const char* s2) const
	{

		return strcmp(s1, s2) == 0;
	}
	*/
#endif


MP4FileFactory:: MP4FileFactory (void)

{

}


MP4FileFactory:: ~MP4FileFactory (void)

{

}



MP4FileFactory:: MP4FileFactory (const MP4FileFactory &)

{

	assert (1==0);

	// to do

}


MP4FileFactory &MP4FileFactory:: operator = (const MP4FileFactory &)

{

	assert (1==0);

	// to do

	return *this;

}


Error MP4FileFactory:: init (unsigned long ulMaxMp4FilesNumberInMemory,
	unsigned long ulMp4FilesNumberToDeleteOnOverflow,
	Boolean_t bUseMP4ConsistencyCheck, Tracer_p ptTracer,
	unsigned long ulBucketsNumberForMP4FilesCache)

{

	if (ptTracer == (Tracer_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);

		return err;
	}

	_ulMaxMp4FilesNumberInMemory		= ulMaxMp4FilesNumberInMemory;
	_ulMp4FilesNumberToDeleteOnOverflow	= ulMp4FilesNumberToDeleteOnOverflow;
	_bUseMP4ConsistencyCheck			= bUseMP4ConsistencyCheck;

	_ptTracer							= ptTracer;

	#ifdef SLACK_LIB
		if ((_pmMP4FileSet = map_create_sized (ulBucketsNumberForMP4FilesCache,
			hashMapDestroy)) == (Map *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_HASHMAP_ERROR);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	#else
		if ((_phHasher = new BufferHasher_t) == (BufferHasher_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if ((_pcComparer = new BufferCmp_t) == (BufferCmp_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete _phHasher;

			return err;
		}

		if ((_pmpsMP4FileSet = new ElementSetHashMap_t (
			100, *_phHasher, *_pcComparer)) ==
			(ElementSetHashMap_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete _pcComparer;
			delete _phHasher;

			return err;
		}
	#endif

	#if defined(__CYGWIN__)
		if (_mtMP4FileSet. init (PMutex:: MUTEX_RECURSIVE) != errNoError)
	#else							// POSIX.1-1996 standard (HPUX 11)
		if (_mtMP4FileSet. init (PMutex:: MUTEX_FAST) != errNoError)
	#endif
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
																	            		return err;
		delete _pmpsMP4FileSet;
		delete _pcComparer;
		delete _phHasher;

		return err;
	}


	return errNoError;
}


Error MP4FileFactory:: finish (void)

{

	ElementSet_p								pesElement;


	#ifdef SLACK_LIB
		int				iMapHasNextRet;


		for (; (iMapHasNextRet = map_has_next (_pmMP4FileSet)) == 1; )
		{
			pesElement				= (ElementSet_p) map_next (_pmMP4FileSet);

			if ((pesElement -> _pmp4File) -> finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILE_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete (pesElement -> _pmp4File);
			pesElement -> _pmp4File	= (MP4File_p) NULL;

			delete pesElement;
			pesElement				= (ElementSet_p) NULL;
		}

		if (iMapHasNextRet == -1)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_HASHMAP_ERROR);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		map_destroy (&_pmMP4FileSet);
	#else
		ElementSetHashMap_t:: iterator				it;
		Buffer_p									pbKey;


		for (it = _pmpsMP4FileSet -> begin (); it != _pmpsMP4FileSet -> end ();
			++it)
		{
			pbKey					= it -> first;
			if (pbKey -> finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
			delete pbKey;

			pesElement				= it -> second;

			if ((pesElement -> _pmp4File) -> finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILE_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete (pesElement -> _pmp4File);
			pesElement -> _pmp4File	= (MP4File_p) NULL;

			delete pesElement;
			pesElement				= (ElementSet_p) NULL;
		}

		_pmpsMP4FileSet -> clear ();

		delete _pmpsMP4FileSet;
		delete _pcComparer;
		delete _phHasher;
	#endif

	if (_mtMP4FileSet. finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
																	            		return err;
	}


	return errNoError;
}


Error MP4FileFactory:: getMP4File (const char *pmp4FilePath,
	Boolean_t bUse64Bits, MP4File_p *pmp4File,
	Boolean_t bMP4FileToBeModified,
	unsigned long ulFileCacheSizeInBytes,
	MP4Atom:: Standard_t sStandard, Boolean_p pbIsMP4FileInCache)

{

	ElementSet_p									pesElement;
	Boolean_t										bMP4FileFound;
	Error_t											errFileInit;
	Buffer_p										pbFileNameAndFileTime;
	time_t											tFileTime;
	Error_t											errGetFileTime;


	if (pmp4FilePath == (const char *) NULL ||
		pmp4File == (MP4File_p *) NULL ||
		pbIsMP4FileInCache == (Boolean_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mtMP4FileSet. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errGetFileTime = FileIO:: getFileTime (pmp4FilePath,
		&tFileTime)) != errNoError)
	{
		Error					err;
		long					lError;
		unsigned long			ulSizeData;


		ulSizeData			= sizeof (int);
		errGetFileTime. getUserData ((void *) &lError, &ulSizeData);
		if (lError == ENOENT)
			err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_FILENOTFOUND, 1, pmp4FilePath);
		else
			err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_GETFILETIME_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4FileSet. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((pbFileNameAndFileTime = new Buffer_t) == (Buffer_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4FileSet. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbFileNameAndFileTime -> init (pmp4FilePath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete pbFileNameAndFileTime;

		if (_mtMP4FileSet. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbFileNameAndFileTime -> append (
		(unsigned long) tFileTime) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pbFileNameAndFileTime -> finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete pbFileNameAndFileTime;

		if (_mtMP4FileSet. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// psStandard initialization (used by _pmp4File -> init)
	/*
	{
		unsigned long				ulFileNameLength;


		ulFileNameLength				= strlen (pmp4FilePath);

		if (tolower (pmp4FilePath [ulFileNameLength - 1]) == 'p' &&
			tolower (pmp4FilePath [ulFileNameLength - 2]) == 'g' &&
			pmp4FilePath [ulFileNameLength - 3] == '3')
		{
			*psStandard			= MP4Atom:: MP4F_3GPP;
		}
		else
		{
			*psStandard			= MP4Atom:: MP4F_ISMA;
		}
	}
	*/

	// try to search the file inside the hash map
	#ifdef SLACK_LIB
		if ((pesElement = (ElementSet_p) map_get (_pmMP4FileSet,
			(const void *) ((const char *) bFileNameAndFileTime))) ==
			(ElementSet_p) NULL)
			bMP4FileFound				= false;
		else
			bMP4FileFound				= true;
	#else
		ElementSetHashMap_t:: iterator			it;
		
		it		= _pmpsMP4FileSet -> find (pbFileNameAndFileTime);

		if (it == _pmpsMP4FileSet -> end()) 
			bMP4FileFound				= false;
		else
		{
			// The iterator, it, is pointing to a record in _mpsMP4FileSet for id
			// bFileNameAndFileTime. The value of it->first will be the key
			// for the record which in this case is a string.
			// The value of it->second will be the record stored in the table
			// which for _mpsMP4FileSet is a ElementSet_p.
			bMP4FileFound				= true;
			pesElement					= it -> second;
		}

		/* old
		ElementSetHashMap_t:: const_iterator			it;


		it			= _mpsMP4FileSet. find (
			(const char *) bFileNameAndFileTime);

		if (it == _mpsMP4FileSet. end ())
			bMP4FileFound				= false;
		else
		{
			bMP4FileFound				= true;
			pesElement					= (*it). second;
		}
		*/
	#endif

	if (!bMP4FileFound)
	{
		*pbIsMP4FileInCache				= false;

		#ifdef SLACK_LIB
			ssize_t					ssMappingsNumber;

			ssMappingsNumber			= map_size (_pmMP4FileSet);
			if (ssMappingsNumber == -1)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_HASHMAP_ERROR);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pbFileNameAndFileTime -> finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete pbFileNameAndFileTime;

				if (_mtMP4FileSet. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (ssMappingsNumber >= _ulMaxMp4FilesNumberInMemory)
		#else
			if (_pmpsMP4FileSet -> size () >= _ulMaxMp4FilesNumberInMemory)
		#endif
		{
			if (removeOlderNotUsedMp4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_REMOVEOLDERNOTUSEDMP4FILE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pbFileNameAndFileTime -> finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete pbFileNameAndFileTime;

				if (_mtMP4FileSet. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if ((pesElement = new ElementSet_t) == (ElementSet_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pbFileNameAndFileTime -> finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pbFileNameAndFileTime;

			if (_mtMP4FileSet. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if ((pesElement -> _pmp4File = new MP4File_t) == (MP4File_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete pesElement;
			pesElement				= (ElementSet_p) NULL;

			if (pbFileNameAndFileTime -> finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pbFileNameAndFileTime;

			if (_mtMP4FileSet. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if ((errFileInit = (pesElement -> _pmp4File) -> init (
			pmp4FilePath, bUse64Bits, _bUseMP4ConsistencyCheck,
			bMP4FileToBeModified, ulFileCacheSizeInBytes,
			sStandard, _ptTracer)) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete pesElement -> _pmp4File;
			pesElement -> _pmp4File	= (MP4File_p) NULL;

			delete pesElement;
			pesElement				= (ElementSet_p) NULL;

			if (pbFileNameAndFileTime -> finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pbFileNameAndFileTime;

			if (_mtMP4FileSet. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errFileInit;
		}

		pesElement -> _lReferencesNumber		= 0;

		#ifdef SLACK_LIB
			if (map_add (_pmMP4FileSet, (const char *) bFileNameAndFileTime,
				pesElement))
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_HASHMAP_ERROR);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if ((pesElement -> _pmp4File) -> finish () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILE_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete pesElement -> _pmp4File;
				pesElement -> _pmp4File	= (MP4File_p) NULL;

				delete pesElement;
				pesElement				= (ElementSet_p) NULL;

				if (pbFileNameAndFileTime -> finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete pbFileNameAndFileTime;

				if (_mtMP4FileSet. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		#else
			// it is not used InsertWithoutDuplication because we
			// know the key does not exist inside the hash map
			_pmpsMP4FileSet -> insert (pbFileNameAndFileTime, pesElement);
//	old		_mpsMP4FileSet. insert (
//				make_pair ((const char *) bFileNameAndFileTime, pesElement));
		#endif
	}
	else
	{
		*pbIsMP4FileInCache					= true;

		if (pbFileNameAndFileTime -> finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4FileSet. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		delete pbFileNameAndFileTime;
	}

	pesElement -> _tLastUsedTime			= time (NULL);
	pesElement -> _lReferencesNumber		+= 1;

	*pmp4File								= pesElement -> _pmp4File;

	if (_mtMP4FileSet. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4FileFactory:: isMP4FileInCache (const char *pmp4FilePath,
	Boolean_p pbIsMP4FileInCache)

{

	Buffer_t										bFileNameAndFileTime;
	time_t											tFileTime;
	Error_t											errGetFileTime;


	if (pmp4FilePath == (const char *) NULL ||
		pbIsMP4FileInCache == (Boolean_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mtMP4FileSet. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errGetFileTime = FileIO:: getFileTime (pmp4FilePath, &tFileTime)) !=
		errNoError)
	{
		Error					err;
		long					lError;
		unsigned long			ulSizeData;


		ulSizeData			= sizeof (int);
		errGetFileTime. getUserData ((void *) &lError, &ulSizeData);
		if (lError == ENOENT)
			err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_FILENOTFOUND, 1, pmp4FilePath);
		else
			err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_GETFILETIME_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4FileSet. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bFileNameAndFileTime. init (pmp4FilePath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4FileSet. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bFileNameAndFileTime. append ((unsigned long) tFileTime) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bFileNameAndFileTime. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4FileSet. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	ElementSetHashMap_t:: iterator			it;

	it		= _pmpsMP4FileSet -> find (&bFileNameAndFileTime);

	if (it == _pmpsMP4FileSet -> end()) 
		*pbIsMP4FileInCache				= false;
	else
	{
		// The iterator, it, is pointing to a record in _mpsMP4FileSet for id
		// bFileNameAndFileTime. The value of it->first will be the key
		// for the record which in this case is a string.
		// The value of it->second will be the record stored in the table
		// which for _mpsMP4FileSet is a ElementSet_p.
		*pbIsMP4FileInCache					= true;
	}

	if (_mtMP4FileSet. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4FileFactory:: releaseMP4File (const char *pmp4FilePath)

{

	ElementSet_p									pesElement;
	Boolean_t										bMP4FileFound;
	Buffer_t										bFileNameAndFileTime;
	time_t											tFileTime;
	Error_t											errGetFileTime;


	if (pmp4FilePath == (const char *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mtMP4FileSet. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errGetFileTime = FileIO:: getFileTime (pmp4FilePath, &tFileTime)) !=
		errNoError)
	{
		Error					err;
		long					lError;
		unsigned long			ulSizeData;


		ulSizeData			= sizeof (int);
		errGetFileTime. getUserData ((void *) &lError, &ulSizeData);
		if (lError == ENOENT)
			err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILEFACTORY_FILENOTFOUND, 1, pmp4FilePath);
		else
			err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_GETFILETIME_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4FileSet. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bFileNameAndFileTime. init (pmp4FilePath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4FileSet. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bFileNameAndFileTime. append ((unsigned long) tFileTime) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bFileNameAndFileTime. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4FileSet. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	#ifdef SLACK_LIB
		if ((pesElement = (ElementSet_p) map_get (_pmMP4FileSet,
			(const void *) ((const char *) bFileNameAndFileTime))) ==
			(ElementSet_p) NULL)
			bMP4FileFound				= false;
		else
			bMP4FileFound				= true;
	#else
		ElementSetHashMap_t:: iterator			it;
		
		it		= _pmpsMP4FileSet -> find (&bFileNameAndFileTime);

		if (it == _pmpsMP4FileSet -> end()) 
			bMP4FileFound				= false;
		else
		{
			// The iterator, it, is pointing to a record in _mpsMP4FileSet for id
			// bFileNameAndFileTime. The value of it->first will be the key
			// for the record which in this case is a string.
			// The value of it->second will be the record stored in the table
			// which for _mpsMP4FileSet is a ElementSet_p.
			bMP4FileFound				= true;
			pesElement					= it -> second;
		}
		/* old
		ElementSetHashMap_t:: const_iterator			it;

		it			= _mpsMP4FileSet. find (
			(const char *) bFileNameAndFileTime);

		if (it == _mpsMP4FileSet. end ())
			bMP4FileFound				= false;
		else
		{
			bMP4FileFound				= true;
			pesElement					= (*it). second;
		}
		*/
	#endif

	if (!bMP4FileFound)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILEFACTORY_MP4FILENOTFOUND,
			1, pmp4FilePath);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bFileNameAndFileTime. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4FileSet. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pesElement -> _lReferencesNumber <= 0)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILEFACTORY_RELEASEOFAFILENOREQUIRED,
			1, pmp4FilePath);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bFileNameAndFileTime. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4FileSet. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	pesElement -> _lReferencesNumber		-= 1;

	if (bFileNameAndFileTime. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4FileSet. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtMP4FileSet. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4FileFactory:: removeOlderNotUsedMp4File ()

{

	ElementSet_p							pesOlderElementsToDelete;
	ElementSet_p							pesElementToInsert;
	ElementSet_p							pesElement;
	long									lIndex;
	long									lIndexWhereInsert;
	char									pmp4FilePath [
		MP4F_MAXPATHNAMELENGTH];
	Buffer_t								bFileNameAndFileTime;
	time_t									tFileTime;
	Error_t									errGetFileTime;


	if ((pesOlderElementsToDelete = new ElementSet_t [
		_ulMp4FilesNumberToDeleteOnOverflow]) == (ElementSet_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return errNoError;
	}

	// initialize pesOlderElementsToDelete with _tLastUsedTime = to
	// the current maximum value
	{
		for (lIndex = 0; lIndex < _ulMp4FilesNumberToDeleteOnOverflow; lIndex++)
		{
			(pesOlderElementsToDelete [lIndex]). _tLastUsedTime			=
				time (NULL) + 1000;
			(pesOlderElementsToDelete [lIndex]). _lReferencesNumber		= -1;
			(pesOlderElementsToDelete [lIndex]). _pmp4File				=
				(MP4File_p) NULL;
		}
	}

	// initialize pesOlderElementsToDelete with Elements to delete
	#ifdef SLACK_LIB
		int				iMapHasNextRet;


		for (; (iMapHasNextRet = map_has_next (_pmMP4FileSet)) == 1; )
		{
			pesElementToInsert		= (ElementSet_p) map_next (_pmMP4FileSet);

			if (findIndexWhereInsert (pesOlderElementsToDelete,
				pesElementToInsert, &lIndexWhereInsert) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_FINDINDEXWHEREINSERT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete pesOlderElementsToDelete;
				pesOlderElementsToDelete		= (ElementSet_p) NULL;

				return err;
			}

			if (lIndexWhereInsert < _ulMp4FilesNumberToDeleteOnOverflow)
			{
				for (lIndex = _ulMp4FilesNumberToDeleteOnOverflow - 1;
					lIndex > lIndexWhereInsert; lIndex--)
				{
					pesOlderElementsToDelete [lIndex]		=
						pesOlderElementsToDelete [lIndex - 1];
				}

				pesOlderElementsToDelete [lIndexWhereInsert]			=
					*pesElementToInsert;
			}
		}

		if (iMapHasNextRet == -1)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_HASHMAP_ERROR);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete pesOlderElementsToDelete;
			pesOlderElementsToDelete		= (ElementSet_p) NULL;

			return err;
		}
	#else
		ElementSetHashMap_t:: iterator	it;

		for (it = _pmpsMP4FileSet -> begin (); it != _pmpsMP4FileSet -> end ();
			++it)
		{
			pesElementToInsert			= it -> second;

			if (findIndexWhereInsert (pesOlderElementsToDelete,
				pesElementToInsert, &lIndexWhereInsert) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_FINDINDEXWHEREINSERT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete pesOlderElementsToDelete;
				pesOlderElementsToDelete		= (ElementSet_p) NULL;

				return err;
			}

			if (lIndexWhereInsert < _ulMp4FilesNumberToDeleteOnOverflow)
			{
				for (lIndex = _ulMp4FilesNumberToDeleteOnOverflow - 1;
					lIndex > lIndexWhereInsert; lIndex--)
				{
					pesOlderElementsToDelete [lIndex]		=
						pesOlderElementsToDelete [lIndex - 1];
				}

				pesOlderElementsToDelete [lIndexWhereInsert]			=
					*pesElementToInsert;
			}
		}
		/* old
		ElementSetHashMap_t:: const_iterator	it;

		for (it = _mpsMP4FileSet. begin (); it != _mpsMP4FileSet. end ();
			++it)
		{
			pesElementToInsert			= (*it). second;

			if (findIndexWhereInsert (pesOlderElementsToDelete,
				pesElementToInsert, &lIndexWhereInsert) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_FINDINDEXWHEREINSERT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete pesOlderElementsToDelete;
				pesOlderElementsToDelete		= (ElementSet_p) NULL;

				return err;
			}

			if (lIndexWhereInsert < _ulMp4FilesNumberToDeleteOnOverflow)
			{
				for (lIndex = _ulMp4FilesNumberToDeleteOnOverflow - 1;
					lIndex > lIndexWhereInsert; lIndex--)
				{
					pesOlderElementsToDelete [lIndex]		=
						pesOlderElementsToDelete [lIndex - 1];
				}

				pesOlderElementsToDelete [lIndexWhereInsert]			=
					*pesElementToInsert;
			}
		}
		*/
	#endif

	if (bFileNameAndFileTime. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete pesOlderElementsToDelete;
		pesOlderElementsToDelete		= (ElementSet_p) NULL;

		return err;
	}

	// delete Elements from _mpsMP4FileSet
	for (lIndex = 0; lIndex < _ulMp4FilesNumberToDeleteOnOverflow; lIndex++)
	{
		if (((pesOlderElementsToDelete [lIndex]). _pmp4File) -> getMP4FilePath (
			pmp4FilePath) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_GETMP4FILEPATH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bFileNameAndFileTime. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pesOlderElementsToDelete;
			pesOlderElementsToDelete		= (ElementSet_p) NULL;

			return err;
		}

		if ((errGetFileTime = FileIO:: getFileTime (pmp4FilePath,
			&tFileTime)) != errNoError)
		{
			Error					err;
			long					lError;
			unsigned long			ulSizeData;


			ulSizeData			= sizeof (int);
			errGetFileTime. getUserData ((void *) &lError, &ulSizeData);
			if (lError == ENOENT)
				err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_FILENOTFOUND, 1, pmp4FilePath);
			else
				err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_GETFILETIME_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bFileNameAndFileTime. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pesOlderElementsToDelete;
			pesOlderElementsToDelete		= (ElementSet_p) NULL;

			return err;
		}

		if (bFileNameAndFileTime. setBuffer (pmp4FilePath) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_SETBUFFER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bFileNameAndFileTime. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pesOlderElementsToDelete;
			pesOlderElementsToDelete		= (ElementSet_p) NULL;

			return err;
		}

		if (bFileNameAndFileTime. append (
			(unsigned long) tFileTime) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bFileNameAndFileTime. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pesOlderElementsToDelete;
			pesOlderElementsToDelete		= (ElementSet_p) NULL;

			return err;
		}

		#ifdef SLACK_LIB
			if ((pesElement = (ElementSet_p) map_get (_pmMP4FileSet,
				(const void *) ((const char *) bFileNameAndFileTime))) ==
				(ElementSet_p) NULL)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_MP4FILENOTFOUND,
					1, pmp4FilePath);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bFileNameAndFileTime. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete pesOlderElementsToDelete;
				pesOlderElementsToDelete		= (ElementSet_p) NULL;

				return err;
			}

			if (map_remove (_pmMP4FileSet, (const void *) pmp4FilePath))
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_HASHMAP_ERROR);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bFileNameAndFileTime. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete pesOlderElementsToDelete;
				pesOlderElementsToDelete		= (ElementSet_p) NULL;

				return err;
			}
		#else
			ElementSetHashMap_t:: iterator			it2;
			Buffer_p								pbKey;

			it2			= _pmpsMP4FileSet -> find (&bFileNameAndFileTime);

			if (it2 == _pmpsMP4FileSet -> end ())
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILEFACTORY_MP4FILENOTFOUND,
					1, pmp4FilePath);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bFileNameAndFileTime. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete pesOlderElementsToDelete;
				pesOlderElementsToDelete		= (ElementSet_p) NULL;

				return err;
			}

			pbKey					= it2 -> first;

			pesElement				= it2 -> second;

			_pmpsMP4FileSet -> Delete (&bFileNameAndFileTime);
		#endif

		if ((pesElement -> _pmp4File) -> finish () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete (pesElement -> _pmp4File);
			pesElement -> _pmp4File	= (MP4File_p) NULL;

			delete pesElement;
			pesElement				= (ElementSet_p) NULL;

			if (bFileNameAndFileTime. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pesOlderElementsToDelete;
			pesOlderElementsToDelete		= (ElementSet_p) NULL;

			return err;
		}

		delete (pesElement -> _pmp4File);
		pesElement -> _pmp4File	= (MP4File_p) NULL;

		delete pesElement;
		pesElement				= (ElementSet_p) NULL;

		if (pbKey -> finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bFileNameAndFileTime. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pesOlderElementsToDelete;
			pesOlderElementsToDelete		= (ElementSet_p) NULL;

			return err;
		}

		delete pbKey;
	}

	if (bFileNameAndFileTime. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete pesOlderElementsToDelete;
		pesOlderElementsToDelete		= (ElementSet_p) NULL;

		return err;
	}

	delete pesOlderElementsToDelete;
	pesOlderElementsToDelete		= (ElementSet_p) NULL;


	return errNoError;
}


Error MP4FileFactory:: findIndexWhereInsert (
	ElementSet_p pesOlderElementsToDelete,
	ElementSet_p pesElementToInsert, long *plIndexWhereInsert)

{

	long				lStartIndex;
	long				lEndIndex;
	long				lMiddleIndex;


	lStartIndex					= 0;
	lEndIndex					= _ulMp4FilesNumberToDeleteOnOverflow - 1;

	while (lStartIndex < lEndIndex)
	{
		lMiddleIndex			= (lEndIndex - lStartIndex) / 2;
		lMiddleIndex			+= lStartIndex;

		if ((pesOlderElementsToDelete [lMiddleIndex]). _tLastUsedTime <
			pesElementToInsert -> _tLastUsedTime)
		{
			if (lMiddleIndex + 1 < _ulMp4FilesNumberToDeleteOnOverflow)
				lStartIndex			= lMiddleIndex + 1;
			else
				lStartIndex			= lMiddleIndex;
		}
		else
		{
			if (lMiddleIndex - 1 >= 0)
				lEndIndex			= lMiddleIndex - 1;
			else
				lEndIndex			= lMiddleIndex;
		}
	}

	if (lStartIndex == _ulMp4FilesNumberToDeleteOnOverflow - 1)
	{
		if ((pesOlderElementsToDelete [lStartIndex]). _tLastUsedTime <
			pesElementToInsert -> _tLastUsedTime)
		{
			*plIndexWhereInsert				= lStartIndex + 1;
		}
		else
		{
			*plIndexWhereInsert				= lStartIndex;
		}
	}
	else
	{
		*plIndexWhereInsert				= lStartIndex;
	}


	return errNoError;
}
