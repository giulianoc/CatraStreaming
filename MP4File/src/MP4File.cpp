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


#include "MP4DecoderConfigDescr.h"
#include "MP4HdlrAtom.h"
#include "MP4HintAtom.h"
#include "MP4MdhdAtom.h"
#include "MP4MvhdAtom.h"
#include "MP4PaytAtom.h"
#include "MP4RtpAtom.h"
#include "MP4S263Atom.h"
#include "MP4SamrAtom.h"
#include "MP4SawbAtom.h"
#include "MP4SdpAtom.h"
#include "MP4StszAtom.h"
#include "MP4TkhdAtom.h"
#include "MP4TpylAtom.h"
#include "FileIO.h"
#include "MP4File.h"
#include "MP4FileMessages.h"
// #include "rfcisma.h"
// #include "rfc2429.h"
// #include "rfc3016.h"
// #include "rfc3267.h"
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef WIN32
	#include <stdio.h>
	// #include <Winsock2.h>
#else
	#include <netinet/in.h>
#endif


MP4File:: MP4File (void)

{

	_smMP4FileStatus		= MP4F_BUILDED;
}


MP4File:: ~MP4File (void)

{

}



MP4File:: MP4File (const MP4File &)

{

	assert (1==0);

	// to do

}


MP4File &MP4File:: operator = (const MP4File &)

{

	assert (1==0);

	// to do

	return *this;

}


Error MP4File:: init (const char *pMP4FilePath, Boolean_t bUse64Bits,
	Boolean_t bUseMP4ConsistencyCheck, Boolean_t bToBeModified,
	unsigned long ulFileCacheSizeInBytes,
	MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)

{

	Error_t						errRootInit;
	Error						errFileInit;


	if (_smMP4FileStatus != MP4F_BUILDED)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_OPERATIONNOTALLOWED, 1, _smMP4FileStatus);

		return err;
	}

	if (pMP4FilePath == (char *) NULL ||
		ptTracer == (Tracer_p) NULL ||
		strlen (pMP4FilePath) > MP4F_MAXPATHNAMELENGTH - 1)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);

		return err;
	}

	_bUse64Bits						= bUse64Bits;
	_bUseMP4ConsistencyCheck		= bUseMP4ConsistencyCheck;
	_ptTracer						= ptTracer;

	_bInternalFile				= true;

	if ((_pfFile = new FileReader_t) == (FileReader_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((errFileInit = _pfFile -> init (pMP4FilePath,
		ulFileCacheSizeInBytes)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errFileInit,
			__FILE__, __LINE__);
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEREADER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete _pfFile;
		_pfFile				= (FileReader_p) NULL;

		return err;
	}

	if (_mtMP4File. init (PMutex:: MUTEX_RECURSIVE) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_pfFile -> finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		delete _pfFile;
		_pfFile				= (FileReader_p) NULL;

		return err;
	}

	if ((errRootInit = _maRootAtom. init ((MP4Atom_p) NULL,
		_pfFile, 0, (unsigned long) (*_pfFile), 0, &_mtMP4File,
		0, _bUse64Bits, _bUseMP4ConsistencyCheck, sStandard, _ptTracer)) !=
		errNoError)
	{
		// Error err = MP4FileErrors (__FILE__, __LINE__,
		// 	MP4F_MP4ATOM_INIT_FAILED);
		// _ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
		// 	__FILE__, __LINE__);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRootInit,
			__FILE__, __LINE__);

		if (_mtMP4File. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_pfFile -> finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		delete _pfFile;
		_pfFile				= (FileReader_p) NULL;

		// return err;
		return errRootInit;
	}

	_smMP4FileStatus		= MP4F_INITIALIZED;

	if (initializeTracksVector () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_INITIALIZETRACKSVECTOR_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		_smMP4FileStatus		= MP4F_BUILDED;

		if (_maRootAtom. finish () != errNoError)
		{
				Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (_mtMP4File. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_pfFile -> finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		delete _pfFile;
		_pfFile				= (FileReader_p) NULL;


		return err;
	}

	if (bToBeModified)
	{
		if (_maRootAtom. moveMoovAtomAsLastAtom () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ROOTATOM_MOVEMOOVATOMASLASTATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			_smMP4FileStatus		= MP4F_BUILDED;

			if (_maRootAtom. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (_mtMP4File. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_pfFile -> finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEREADER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			delete _pfFile;
			_pfFile				= (FileReader_p) NULL;

			return err;
		}
	}


	return errNoError;
}


Error MP4File:: init (FileReader_p pfFile, Boolean_t bUse64Bits,
	Boolean_t bUseMP4ConsistencyCheck, Boolean_t bToBeModified,
	MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)

{

	Error_t						errRootInit;


	if (_smMP4FileStatus != MP4F_BUILDED)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_OPERATIONNOTALLOWED, 1, _smMP4FileStatus);

		return err;
	}

	if (pfFile == (FileReader_p) NULL ||
		ptTracer == (Tracer_p) NULL ||
		strlen ((const char *) (*pfFile)) > MP4F_MAXPATHNAMELENGTH - 1)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);

		return err;
	}

	_bUse64Bits						= bUse64Bits;
	_bUseMP4ConsistencyCheck		= bUseMP4ConsistencyCheck;
	_ptTracer						= ptTracer;

	_bInternalFile				= false;

	_pfFile						= pfFile;

	if (_mtMP4File. init (PMutex:: MUTEX_RECURSIVE) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errRootInit = _maRootAtom. init ((MP4Atom_p) NULL,
		_pfFile, 0, (unsigned long) (*_pfFile), 0, &_mtMP4File,
		0, _bUse64Bits, _bUseMP4ConsistencyCheck, sStandard, _ptTracer)) !=
		errNoError)
	{
		// Error err = MP4FileErrors (__FILE__, __LINE__,
		// 	MP4F_MP4ATOM_INIT_FAILED);
		// _ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
		// 	__FILE__, __LINE__);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRootInit,
			__FILE__, __LINE__);

		if (_mtMP4File. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		// return err;
		return errRootInit;
	}

	_smMP4FileStatus		= MP4F_INITIALIZED;

	if (initializeTracksVector () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_INITIALIZETRACKSVECTOR_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		_smMP4FileStatus		= MP4F_BUILDED;

		if (_maRootAtom. finish () != errNoError)
		{
				Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (_mtMP4File. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bToBeModified)
	{
		if (_maRootAtom. moveMoovAtomAsLastAtom () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ROOTATOM_MOVEMOOVATOMASLASTATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			_smMP4FileStatus		= MP4F_BUILDED;

			if (_maRootAtom. finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (_mtMP4File. finish () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}


	return errNoError;
}


Error MP4File:: finish (void)

{

	Error_t						errGeneric;


	if (_smMP4FileStatus != MP4F_INITIALIZED)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_OPERATIONNOTALLOWED, 1, _smMP4FileStatus);

		return err;
	}

	if (_vMP4TracksInfo. size () > 0)
	{
		std:: vector<MP4TrackInfo_p>:: const_iterator	it;
		MP4TrackInfo_p					pmtiMP4TrackInfo;


		for (it = _vMP4TracksInfo. begin (); it != _vMP4TracksInfo. end ();
			++it)
		{
			pmtiMP4TrackInfo				= *it;

			if (pmtiMP4TrackInfo -> finish () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete pmtiMP4TrackInfo;
			pmtiMP4TrackInfo		= (MP4TrackInfo_p) NULL;
		}

		_vMP4TracksInfo. clear ();
	}

	if ((errGeneric = _mtMP4File. finish ()) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_maRootAtom. finish () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);
	}

	if (_bInternalFile)
	{
		if (_pfFile -> finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		delete _pfFile;
		_pfFile				= (FileReader_p) NULL;
	}

	_smMP4FileStatus		= MP4F_BUILDED;


	return errNoError;
}


Error MP4File:: getFile (FileReader_p *pfFile)

{
	
	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pfFile				= _pfFile;

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4File:: getRootAtom (MP4RootAtom_p *pmaRootAtom)

{
	
	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pmaRootAtom			= &_maRootAtom;

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4File:: write (const char *pMP4PathName)

{

	int						iFileDescriptor;


	// Since the file is not read completely (ie mdat, free and skip
	//	atoms are not read from file) we cannot write the same file

	if (_smMP4FileStatus != MP4F_INITIALIZED ||
		(!strcmp (pMP4PathName, (const char *) (*_pfFile))))
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_OPERATIONNOTALLOWED, 1, _smMP4FileStatus);

		return err;
	}

	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	#ifdef WIN32
		if (FileIO:: open (pMP4PathName, O_RDWR | O_TRUNC | O_CREAT | O_BINARY,
			_S_IREAD | _S_IWRITE, &iFileDescriptor) != errNoError)
	#else
		if (FileIO:: open (pMP4PathName, O_RDWR | O_TRUNC | O_CREAT,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, &iFileDescriptor) !=
			errNoError)
	#endif
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_OPEN_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_maRootAtom. write (iFileDescriptor) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_WRITE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (FileIO:: close (iFileDescriptor) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_CLOSE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (FileIO:: close (iFileDescriptor) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_CLOSE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4File:: getMP4FileStatus (MP4FileStatus_p psmMP4FileState)

{

	if (psmMP4FileState == (MP4FileStatus_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);

		return err;
	}

	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*psmMP4FileState		= _smMP4FileStatus;

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4File:: searchAtom (const char *pAtomPath,
	Boolean_t bIsSearchErrorTraceEnabled, MP4Atom_p *pmp4Atom)

{

	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_smMP4FileStatus != MP4F_INITIALIZED)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_OPERATIONNOTALLOWED, 1, _smMP4FileStatus);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_maRootAtom. searchAtom (pAtomPath, bIsSearchErrorTraceEnabled,
		pmp4Atom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, pAtomPath);
		if (bIsSearchErrorTraceEnabled)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4File:: getMovieDuration (MP4TrackInfo_p pmtiVideoTrackInfo,
	double *pdMovieDuration)

{

	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiVideoTrackInfo == (MP4TrackInfo_p) NULL)
	{
		MP4Atom_p				pmaAtom;
		MP4MvhdAtom_p			pmaMvhdAtom;
		unsigned long			ulMoovTimeScale;
		#ifdef WIN32
			__int64					ullMoovDuration;
		#else
			unsigned long long		ullMoovDuration;
		#endif


		if (searchAtom ("moov:0:mvhd:0", true,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_SEARCHATOM_FAILED, 1, "moov:0:mvhd:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			pmaMvhdAtom		= (MP4MvhdAtom_p) pmaAtom;

		if (pmaMvhdAtom -> getTimeScale (&ulMoovTimeScale) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4MVHDATOM_GETTIMESCALE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaMvhdAtom -> getDuration (&ullMoovDuration) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4MVHDATOM_GETDURATION_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (ulMoovTimeScale == 0)
		{
			*pdMovieDuration			= 0.0;
		}
		else
		{
			*pdMovieDuration			= (double) ullMoovDuration /
				(double) ulMoovTimeScale;
		}
	}
	else
	{
		//MP4MdhdAtom_p			pmaMdhdAtom;
		// unsigned long			ulVideoTimeScale;
		// #ifdef WIN32
		//	__int64					ullVideoDuration;
		//#else
		//	unsigned long long		ullVideoDuration;
		//#endif
		// std:: vector<MP4TrackInfo_t>:: const_iterator	it;
		// MP4TrackInfo_t				mtiMP4TrackInfo;
		// MP4TrakAtom_p			pmaLocalTrakAtom;


		if (pmtiVideoTrackInfo -> getDuration (pdMovieDuration) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETDURATION_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		/*
		if (pmaVideoTrakAtom -> searchAtom ("mdia:0:mdhd:0", true,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, "mdia:0:mdhd:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			pmaMdhdAtom		= (MP4MdhdAtom_p) pmaAtom;

		if (pmaMdhdAtom -> getTimeScale (&ulVideoTimeScale) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4MDHDATOM_GETTIMESCALE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaMdhdAtom -> getDuration (&ullVideoDuration) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4MDHDATOM_GETDURATION_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (ulVideoTimeScale == 0)
		{
			*pdMovieDuration			= 0.0;
		}
		else
		{
			*pdMovieDuration			= (double) ullVideoDuration /
				(double) ulVideoTimeScale;
		}
		*/
	}

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


/*
Error MP4File:: getSDP (const char *pIPAddressForRTSP,
	RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
	Boolean_t bVideoTrack, MP4TrackInfo_p pmtiHintOrMediaVideoTrackInfo,
	char *pVideoTrackName, unsigned long *pulVideoPayloadNumber,
	Boolean_t bAudioTrack, MP4TrackInfo_p pmtiHintOrMediaAudioTrackInfo,
	char *pAudioTrackName, unsigned long *pulAudioPayloadNumber,
	double dMovieDuration, Buffer_p pbSDP)

{

	Buffer_t						bString;
	unsigned long					ulNow;
	char							pNow [MP4_MAXLONGLENGTH];
	Boolean_t						bSessionName;
	MP4Atom:: Standard_t			sStandard;
	unsigned long					ulFreePayloadNumber;


	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_maRootAtom. getStandard (&sStandard) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETSTANDARD_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bVideoTrack)
	{
		if (pmtiHintOrMediaVideoTrackInfo -> getTrackName (
			pVideoTrackName) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETTRACKNAME_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (bAudioTrack)
	{
		if (pmtiHintOrMediaAudioTrackInfo -> getTrackName (
			pAudioTrackName) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETTRACKNAME_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (bString. init ("") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbSDP -> setBuffer ("v=0" MP4F_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bString. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbSDP -> append ("o=root ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bString. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	ulNow				= time (NULL);
	sprintf (pNow, "%lu", ulNow);

	if (pbSDP -> append (pNow) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bString. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbSDP -> append (" ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bString. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbSDP -> append (pNow) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bString. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbSDP -> append (" IN IP4 ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bString. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbSDP -> append (pIPAddressForRTSP) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bString. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bString. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	bSessionName				= false;

	// Verify if this movie has a global SDP atom
	// the s SDP field should be included here
	{
		MP4RtpAtom_p				pmraRtpAtom;
		MP4Atom_p					pmaAtom;


		if (searchAtom ("moov:0:udta:0:hnti:0:rtp :0",
			false, &pmaAtom) != errNoError)
		{
			// global SDP not found
			;
		}
		else
		{
			pmraRtpAtom			= (MP4RtpAtom_p) pmaAtom;

			if (pmraRtpAtom -> getDescriptionFormat (&bString) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (!strcmp ((const char *) bString, "sdp "))
			{
				if (pmraRtpAtom -> getSdpText (&bString) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bString. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtMP4File. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (strstr ((const char *) bString, "s="))
					bSessionName				= true;

				if (pbSDP -> append ((const char *) bString) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bString. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtMP4File. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
		}
	}

	if (!bSessionName)
	{
//		char				*pSessionName;


		if (pbSDP -> append ("s=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bString. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		#ifdef WIN32
			if ((strrchr ((const char *) (*_pfFile), '\\') + 1) == (char *) NULL)
		#else
			if ((strrchr ((const char *) (*_pfFile), '/') + 1) == (char *) NULL)
		#endif
		{
			if (pbSDP -> append ((const char *) (*_pfFile)) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
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
//			if (pbSDP -> append (pSessionName) != errNoError)
			#ifdef WIN32
				if (pbSDP -> append (strrchr (
					(const char *) (*_pfFile), '\\') + 1) != errNoError)
			#else
				if (pbSDP -> append (strrchr (
					(const char *) (*_pfFile), '/') + 1) != errNoError)
			#endif
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bString. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

//	if (pbSDP -> append ("u=http:///"MP4F_NEWLINE) != errNoError)
//	if (pbSDP -> append ("e=admin@"MP4F_NEWLINE") != errNoError)

	{
		// see rfc2326 RTSP pag. 82
		if (pbSDP -> append ("c=IN IP4 0.0.0.0") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bString. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

//		if (pbSDP -> append (pServerIPAddress) != errNoError)

		if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bString. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (pbSDP -> append ("t=0 0" MP4F_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bString. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// URL for aggregate control
//	if (pbSDP -> append ("a=control:trackID" MP4F_NEWLINE) != errNoError)
	if (pbSDP -> append ("a=control:*" MP4F_NEWLINE) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bString. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// "a=range:npt=0-%f"
	{
		char					pMovieDuration [MP4_MAXLONGLENGTH];


		if (pbSDP -> append ("a=range:npt=0-") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bString. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		sprintf (pMovieDuration, "%.3lf", dMovieDuration);

		if (pbSDP -> append (pMovieDuration) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bString. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bString. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// LUXXON
	// pbSDP -> append ("a=X-allowrecord" MP4F_NEWLINE);
	// pbSDP -> append ("a=X-wmfversion:1.0" MP4F_NEWLINE);

	if (MP4TrackInfo:: allocRtpPayloadNumber (&ulFreePayloadNumber,
		&_vMP4TracksInfo, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_ALLOCRTPPAYLOADNUMBER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (bString. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// SdpText for video hint traks
	if (bVideoTrack)
	{
		MP4SdpAtom_p		pmaSdpAtom;
		Error_t				errGetSdpAtom;
		unsigned long		ulAvgBitRate;


		if ((errGetSdpAtom = pmtiHintOrMediaVideoTrackInfo -> getSdpAtom (
			&pmaSdpAtom)) != errNoError)
		{
			MP4TrakAtom_p		pmaLocalTrakAtom;
			MP4HdlrAtom_p		pmaHdlrAtom;
			MP4MdhdAtom_p		pmaMdhdAtom;
			MP4StszAtom_p		pmaStszAtom;
			MP4Atom::MP4Codec_t	cCodec;
			unsigned long		ulObjectTypeIndication;
			unsigned long		ulTrackIdentifier;


			if ((long) errGetSdpAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSDPATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// creation of the SDP

			*pulVideoPayloadNumber			=
				ulFreePayloadNumber++;

			if (pmtiHintOrMediaVideoTrackInfo -> getTrakAtom (
				&pmaLocalTrakAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmtiHintOrMediaVideoTrackInfo -> getCodec (
				&cCodec) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETCODEC_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (cCodec == MP4Atom:: MP4F_CODEC_MPEG4)	// ISO codec
			{
				if (pmtiHintOrMediaVideoTrackInfo -> getObjectTypeIndication (
					&ulObjectTypeIndication) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETOBJECTTYPEINDICATION_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bString. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtMP4File. unLock () != errNoError)
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
				ulObjectTypeIndication		= (unsigned long) -1;
			}

			if (pmtiHintOrMediaVideoTrackInfo -> getTrackIdentifier (
				&ulTrackIdentifier) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRACKIDENTIFIER_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmtiHintOrMediaVideoTrackInfo -> getHdlrAtom (
				&pmaHdlrAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETHDLRATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmtiHintOrMediaVideoTrackInfo -> getMdhdAtom (
				&pmaMdhdAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETMDHDATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmtiHintOrMediaVideoTrackInfo -> getStszAtom (
				&pmaStszAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSTSZATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmtiHintOrMediaVideoTrackInfo -> getAvgBitRate (
				prsrtRTPStreamRealTimeInfo,
				&ulAvgBitRate) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETAVGBITRATE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (MP4Utility:: buildSDPForMediaVideoTrack (
				pmaLocalTrakAtom,
				pmaHdlrAtom,
				pmaMdhdAtom,
				pmaStszAtom,
				cCodec,
				ulObjectTypeIndication,
				ulTrackIdentifier,
				ulAvgBitRate,
				*pulVideoPayloadNumber, &_maRootAtom, sStandard,
				pbSDP, _ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_BUILDSDPFORMEDIAVIDEOTRACK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
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
			if (pmaSdpAtom -> getSdpText (&bString) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			{
				char				*pBeginPayloadNumber;
				char				*pEndPayloadNumber;
				char				pPayloadNumber [MP4_MAXLONGLENGTH];

				if ((pBeginPayloadNumber = strstr ((const char *) bString, "a=rtpmap:")) ==
					(char *) NULL)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILE_SDPTEXTWRONG);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bString. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtMP4File. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				pBeginPayloadNumber		+= strlen ("a=rtpmap:");

				if ((pEndPayloadNumber = strstr (pBeginPayloadNumber, " ")) ==
					(char *) NULL)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILE_SDPTEXTWRONG);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bString. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtMP4File. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				strncpy (pPayloadNumber, pBeginPayloadNumber,
					pEndPayloadNumber - pBeginPayloadNumber);
				pPayloadNumber [pEndPayloadNumber - pBeginPayloadNumber]	=
					'\0';

				*pulVideoPayloadNumber		= atol (pPayloadNumber);
			}

			if (pbSDP -> append ((const char *) bString) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	// SdpText for audio hint traks
	if (bAudioTrack)
	{
		MP4SdpAtom_p		pmaSdpAtom;
		Error_t				errGetSdpAtom;
		unsigned long		ulAvgBitRate;


		if ((errGetSdpAtom = pmtiHintOrMediaAudioTrackInfo -> getSdpAtom (
			&pmaSdpAtom)) != errNoError)
		{
			MP4TrakAtom_p		pmaLocalTrakAtom;
			MP4HdlrAtom_p		pmaHdlrAtom;
			MP4MdhdAtom_p		pmaMdhdAtom;
			MP4StszAtom_p		pmaStszAtom;
			MP4Atom::MP4Codec_t	cCodec;
			unsigned long		ulObjectTypeIndication;
			unsigned long		ulTrackIdentifier;


			if ((long) errGetSdpAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSDPATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// creation of the SDP

			*pulAudioPayloadNumber			=
				ulFreePayloadNumber++;

			if (pmtiHintOrMediaAudioTrackInfo -> getTrakAtom (
				&pmaLocalTrakAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmtiHintOrMediaAudioTrackInfo -> getCodec (
				&cCodec) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETCODEC_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (cCodec == MP4Atom:: MP4F_CODEC_AAC)	// ISO codec
			{
				if (pmtiHintOrMediaAudioTrackInfo -> getObjectTypeIndication (
					&ulObjectTypeIndication) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETOBJECTTYPEINDICATION_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bString. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtMP4File. unLock () != errNoError)
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
				ulObjectTypeIndication		= (unsigned long) -1;

			if (pmtiHintOrMediaAudioTrackInfo -> getTrackIdentifier (
				&ulTrackIdentifier) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRACKIDENTIFIER_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmtiHintOrMediaAudioTrackInfo -> getHdlrAtom (
				&pmaHdlrAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETHDLRATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmtiHintOrMediaAudioTrackInfo -> getMdhdAtom (
				&pmaMdhdAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETMDHDATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmtiHintOrMediaAudioTrackInfo -> getStszAtom (
				&pmaStszAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSTSZATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmtiHintOrMediaAudioTrackInfo -> getAvgBitRate (
				prsrtRTPStreamRealTimeInfo,
				&ulAvgBitRate) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETAVGBITRATE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (MP4Utility:: buildSDPForMediaAudioTrack (
				pmaLocalTrakAtom,
				pmaHdlrAtom,
				pmaMdhdAtom,
				pmaStszAtom,
				cCodec,
				ulObjectTypeIndication,
				ulTrackIdentifier,
				ulAvgBitRate,
				*pulAudioPayloadNumber, &_maRootAtom, sStandard,
				pbSDP, _ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_BUILDSDPFORMEDIAAUDIOTRACK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
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
			if (pmaSdpAtom -> getSdpText (&bString) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			{
				char				*pBeginPayloadNumber;
				char				*pEndPayloadNumber;
				char				pPayloadNumber [MP4_MAXLONGLENGTH];

				if ((pBeginPayloadNumber = strstr ((const char *) bString, "a=rtpmap:")) ==
					(char *) NULL)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILE_SDPTEXTWRONG);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bString. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtMP4File. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				pBeginPayloadNumber		+= strlen ("a=rtpmap:");

				if ((pEndPayloadNumber = strstr (pBeginPayloadNumber, " ")) ==
					(char *) NULL)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILE_SDPTEXTWRONG);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bString. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtMP4File. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				strncpy (pPayloadNumber, pBeginPayloadNumber,
					pEndPayloadNumber - pBeginPayloadNumber);
				pPayloadNumber [pEndPayloadNumber - pBeginPayloadNumber]	=
					'\0';

				*pulAudioPayloadNumber		= atol (pPayloadNumber);
			}

			if (pbSDP -> append ((const char *) bString) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	if (bString. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}
*/


Error MP4File:: getMP4FilePath (char *pMP4FilePath)

{

	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (strlen ((const char *) (*_pfFile)) < MP4F_MAXPATHNAMELENGTH)
		strcpy (pMP4FilePath, (const char *) (*_pfFile));
	else
	{
		strncpy (pMP4FilePath, (const char *) (*_pfFile),
			MP4F_MAXPATHNAMELENGTH - 1);
		pMP4FilePath [MP4F_MAXPATHNAMELENGTH - 1]			= '\0';
	}

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4File:: getTracksInfo (
	std:: vector<MP4TrackInfo_p> **pvMP4TracksInfo)

{

	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pvMP4TracksInfo				= &_vMP4TracksInfo;

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4File:: getHintTrakFromMediaTrakIdentifier (
	unsigned long *pulMediaTrackIdentifiers,
	unsigned long ulMediaTracksNumber,
	MP4TrakAtom_p *pmaHintTrakAtom)

{

	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_vMP4TracksInfo. size () > 0)
	{
		std:: vector<MP4TrackInfo_p>:: const_iterator	it;
		MP4TrackInfo_p					pmtiMP4TrackInfo;
		MP4HintAtom_p					pmaHintHintAtom;
		unsigned long					ulMediaTrackIndex;
		unsigned long					ulEntryIndex;
		unsigned long					ulEntriesNumber;
		unsigned long					ulLocalMediaTrackIdentifier;
//		MP4Atom_p						pmaAtom;
		char							pHandlerType [
			MP4F_MAXHANDLERTYPELENGTH];


		for (it = _vMP4TracksInfo. begin (); it != _vMP4TracksInfo. end ();
			++it)
		{
			pmtiMP4TrackInfo				= *it;

			if (pmtiMP4TrackInfo -> getHandlerType (
				pHandlerType) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETHANDLERTYPE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (strcmp (pHandlerType, MP4_HINT_TRACK_TYPE))
				continue;

			if (pmtiMP4TrackInfo -> getHintAtom (
				&pmaHintHintAtom, true) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETHINTATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			/*
			if (mtiMP4TrackInfo. _pmaTrakAtom -> searchAtom ("tref:0:hint:0",
				true, &pmaAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, "tref:0:hint:0");
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			else
				pmaHintHintAtom		= (MP4HintAtom_p) pmaAtom;
			*/

			if (pmaHintHintAtom -> getEntriesNumber (&ulEntriesNumber) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4HINTATOM_GETENTRIESNUMBER_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			for (ulMediaTrackIndex = 0; ulMediaTrackIndex < ulMediaTracksNumber;
				ulMediaTrackIndex++)
			{
				for (ulEntryIndex = 0; ulEntryIndex < ulEntriesNumber;
					ulEntryIndex++)
				{
					if (pmaHintHintAtom ->  getTracksReferencesTable (
						ulEntryIndex, &ulLocalMediaTrackIdentifier) !=
						errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4HINTATOM_GETTRACKSREFERENCESTABLE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (_mtMP4File. unLock () != errNoError)
						{
							Error err = PThreadErrors (__FILE__, __LINE__,
								THREADLIB_PMUTEX_UNLOCK_FAILED);
							_ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pulMediaTrackIdentifiers [ulMediaTrackIndex] ==
						ulLocalMediaTrackIdentifier)
						break;
				}

				if (ulEntryIndex == ulEntriesNumber)
					break;
			}

			if (ulMediaTrackIndex == ulMediaTracksNumber)
				break;
		}

		if (it == _vMP4TracksInfo. end ())
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_HINTTRACKNOTFOUND);
			// _ptTracer -> trace (Tracer:: TRACER_LERRR,
			// 	(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
		{
			if (pmtiMP4TrackInfo -> getTrakAtom (
				pmaHintTrakAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4File:: getHintTrackInfoFromMediaTrackInfo (
	MP4TrackInfo_p pmtiMediaTrakInfo,
	MP4TrackInfo_p *pmtiHintTrakInfo)

{

	unsigned long			ulMediaTrackIdentifier;


	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmtiMediaTrakInfo -> getTrackIdentifier (
		&ulMediaTrackIdentifier) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETTRACKIDENTIFIER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_vMP4TracksInfo. size () > 0)
	{
		std:: vector<MP4TrackInfo_p>:: const_iterator	it;
		MP4TrackInfo_p					pmtiMP4TrackInfo;
		MP4HintAtom_p					pmaHintHintAtom;
		// unsigned long					ulMediaTrackIndex;
		// unsigned long					ulEntryIndex;
		unsigned long					ulEntriesNumber;
		unsigned long					ulLocalMediaTrackIdentifier;
//		MP4Atom_p						pmaAtom;
		char							pHandlerType [
			MP4F_MAXHANDLERTYPELENGTH];


		for (it = _vMP4TracksInfo. begin (); it != _vMP4TracksInfo. end ();
			++it)
		{
			pmtiMP4TrackInfo				= *it;

			if (pmtiMP4TrackInfo -> getHandlerType (
				pHandlerType) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETHANDLERTYPE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (strcmp (pHandlerType, MP4_HINT_TRACK_TYPE))
				continue;

			if (pmtiMP4TrackInfo -> getHintAtom (
				&pmaHintHintAtom, true) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETHINTATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			/*
			if (mtiMP4TrackInfo. _pmaTrakAtom -> searchAtom ("tref:0:hint:0",
				true, &pmaAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, "tref:0:hint:0");
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			else
				pmaHintHintAtom		= (MP4HintAtom_p) pmaAtom;
			*/

			if (pmaHintHintAtom -> getEntriesNumber (&ulEntriesNumber) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4HINTATOM_GETENTRIESNUMBER_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (ulEntriesNumber != 1)
				continue;

			if (pmaHintHintAtom ->  getTracksReferencesTable (
				0, &ulLocalMediaTrackIdentifier) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4HINTATOM_GETTRACKSREFERENCESTABLE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (ulMediaTrackIdentifier == ulLocalMediaTrackIdentifier)
				break;
		}

		if (it == _vMP4TracksInfo. end ())
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_HINTTRACKNOTFOUND);
			// _ptTracer -> trace (Tracer:: TRACER_LERRR,
			// 	(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
		{
			*pmtiHintTrakInfo		= pmtiMP4TrackInfo;
		}
	}

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4File:: getTrakInfoFromIdentifier (unsigned long ulTrackIdentifier,
	MP4TrackInfo_p *pmtiTrakInfo)

{

	unsigned long			ulLocalTrackIdentifier;


	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_vMP4TracksInfo. size () > 0)
	{
		std:: vector<MP4TrackInfo_p>:: const_iterator	it;


		for (it = _vMP4TracksInfo. begin (); it != _vMP4TracksInfo. end ();
			++it)
		{
			*pmtiTrakInfo				= *it;

			if ((*pmtiTrakInfo) -> getTrackIdentifier (
				&ulLocalTrackIdentifier) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRACKIDENTIFIER_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (ulLocalTrackIdentifier == ulTrackIdentifier)
				break;
		}

		if (it == _vMP4TracksInfo. end ())
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_ACTIVATION_WRONG);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4File:: getTrakInfoFromTrackPointer (MP4TrakAtom_p pmaTrakAtom,
	MP4TrackInfo_p *pmtiTrakInfo)

{

	MP4TrakAtom_p			pmaLocalTrakAtom;


	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_vMP4TracksInfo. size () > 0)
	{
		std:: vector<MP4TrackInfo_p>:: const_iterator	it;


		for (it = _vMP4TracksInfo. begin (); it != _vMP4TracksInfo. end ();
			++it)
		{
			*pmtiTrakInfo				= *it;

			if ((*pmtiTrakInfo) -> getTrakAtom (
				&pmaLocalTrakAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4File. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaLocalTrakAtom == pmaTrakAtom)
				break;
		}

		if (it == _vMP4TracksInfo. end ())
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_ACTIVATION_WRONG);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4File:: getDump (Buffer_p pbBuffer)

{

	#ifdef WIN32
		__int64								ullRootSize;
	#else
		unsigned long long					ullRootSize;
	#endif
	unsigned long						ulHeaderSize;


	if (pbBuffer -> setBuffer ("") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_maRootAtom. getSize (&ullRootSize,
		&ulHeaderSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbBuffer -> append ("Dumping for the '") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbBuffer -> append ((const char *) (*_pfFile)) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbBuffer -> append ("' MP4 file (length: ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbBuffer -> append (ullRootSize) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pbBuffer -> append ("):\n\n") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_maRootAtom. appendDump (pbBuffer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_APPENDDUMP_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4File:: getFreePayloadsNumber (
	unsigned long ulPayloadsNumberToBeReturned,
	unsigned long *pulPayloadsNumber)

{

	unsigned char					ucPayloadNumber;
	std:: vector<unsigned long>		vUsedPayloads;
	unsigned long					ulUsedPayloadIndex;
	std:: vector<MP4TrackInfo_p>:: const_iterator	it;
	MP4TrackInfo_p					pmtiMP4TrackInfo;
	MP4PaytAtom_p					pmaPaytAtom;
	MP4Atom_p						pmaAtom;
	unsigned long					ulLocalPayloadNumber;
	unsigned long					ulPayloadNumberFound;
	MP4TrakAtom_p					pmaTrakAtom;


	if (ulPayloadsNumberToBeReturned == 0)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mtMP4File. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	for (it = _vMP4TracksInfo. begin ();
		it != _vMP4TracksInfo. end (); ++it)
	{
		pmtiMP4TrackInfo				= *it;

		if (pmtiMP4TrackInfo -> getTrakAtom (&pmaTrakAtom) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaTrakAtom -> searchAtom (
			"udta:0:hinf:0:payt:0", false, &pmaAtom) !=
			errNoError)
			continue;
		else
			pmaPaytAtom		= (MP4PaytAtom_p) pmaAtom;

		if (pmaPaytAtom -> getPayloadNumber (&ulLocalPayloadNumber) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4File. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		vUsedPayloads. insert (vUsedPayloads. end (), ulLocalPayloadNumber);
	}

	ulPayloadNumberFound			= 0;

	for (ucPayloadNumber = 96; ucPayloadNumber < 128; ucPayloadNumber++)
	{
		for (ulUsedPayloadIndex = 0;
			ulUsedPayloadIndex < vUsedPayloads. size (); ulUsedPayloadIndex++)
		{
			if (ucPayloadNumber == vUsedPayloads [ulUsedPayloadIndex])
				break;
		}

		if (ulUsedPayloadIndex == vUsedPayloads. size ())
		{
			pulPayloadsNumber [ulPayloadNumberFound]		= ucPayloadNumber;

			ulPayloadNumberFound++;
		}

		if (ulPayloadsNumberToBeReturned == ulPayloadNumberFound)
			break;
	}

	if (ucPayloadNumber == 128)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UTILITY_NOPAYLOADAVAILABLE);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4File. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_mtMP4File. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


std:: ostream &operator << (std:: ostream &osOutputStream, MP4File_t &mfMP4File)

{

	#ifdef WIN32
		__int64								ullRootSize;
	#else
		unsigned long long					ullRootSize;
	#endif
	unsigned long						ulHeaderSize;


	if ((mfMP4File. _mtMP4File). lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		(mfMP4File. _ptTracer) -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return osOutputStream;
	}

	if ((mfMP4File. _maRootAtom). getSize (&ullRootSize,
		&ulHeaderSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETSIZE_FAILED);
		(mfMP4File. _ptTracer) -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if ((mfMP4File. _mtMP4File). unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			(mfMP4File. _ptTracer) -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return osOutputStream;
	}

	#ifdef WIN32
		std:: cout << "Dumping for the '" << (const char *) (*(mfMP4File. _pfFile))
			<< "' MP4 file (length: " << (unsigned long) ullRootSize << "):"
			<< std:: endl << std:: endl;
	#else
		std:: cout << "Dumping for the '" << (const char *) (*(mfMP4File. _pfFile))
			<< "' MP4 file (length: " << ullRootSize << "):"
			<< std:: endl << std:: endl;
	#endif

	if ((mfMP4File. _maRootAtom). printOnStdOutput () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_PRINTONSTDOUTPUT_FAILED);
		(mfMP4File. _ptTracer) -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if ((mfMP4File. _mtMP4File). unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			(mfMP4File. _ptTracer) -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return osOutputStream;
	}

	if ((mfMP4File. _mtMP4File). unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		(mfMP4File. _ptTracer) -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return osOutputStream;
	}


	return osOutputStream;
}


MP4File:: operator const char * (void) const

{

	return (const char *) (*_pfFile);
}


Error MP4File:: initializeTracksVector (void)

{

	MP4TrakAtom_p				pmaTrakAtom;
	MP4TrackInfo_p				pmtiMP4TrackInfo;
	MP4Atom_p					pmaAtom;
	unsigned long				ulTrackIndex;
	char						pTrackPath [MP4F_MAXTRAKPATHLENGTH];

	// MP4TkhdAtom_p				pmaTkhdAtom;
	// MP4HdlrAtom_p				pmaHdlrAtom;
	// MP4SdpAtom_p				pmaSdpAtom;
	// Buffer_t					bSdp;
	// const char					*pBeginMediaControl;
	// const char					*pEndMediaControl;
	// char						pAtomPath [MP4F_MAXPATHNAMELENGTH];


	_vMP4TracksInfo. clear ();

	for (ulTrackIndex = 0; ; ulTrackIndex++)
	{
		sprintf (pTrackPath, "moov:0:trak:%lu", ulTrackIndex);

		if (searchAtom (pTrackPath, false,
			&pmaAtom) != errNoError)
			break;		// tracks finished
		else
			pmaTrakAtom		=
				(MP4TrakAtom_p) pmaAtom;

		if ((pmtiMP4TrackInfo = new MP4TrackInfo_t) == (MP4TrackInfo_p) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pmtiMP4TrackInfo -> init (pmaTrakAtom, true, _ptTracer) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete pmtiMP4TrackInfo;
			pmtiMP4TrackInfo		= (MP4TrackInfo_p) NULL;

			return err;
		}

		_vMP4TracksInfo. insert (_vMP4TracksInfo. end (), pmtiMP4TrackInfo);

		/*
		// initialize mtiMP4TrackInfo. _pTrackName
		if (!strcmp (mtiMP4TrackInfo. _pHandlerType, MP4_HINT_TRACK_TYPE))
		{
			if ((mtiMP4TrackInfo. _pmaTrakAtom) -> searchAtom (
				"udta:0:hnti:0:sdp :0", false, &pmaAtom) !=
				errNoError)
			{
				// there isn't sdp atom

				sprintf (mtiMP4TrackInfo. _pTrackName, "trackID/%lu",
					mtiMP4TrackInfo. _ulTrackIdentifier);
			}
			else
			{
				pmaSdpAtom		= (MP4SdpAtom_p) pmaAtom;

				if (bSdp. init () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return errNoError;
				}

				if (pmaSdpAtom -> getSdpText (&bSdp) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bSdp. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if ((pBeginMediaControl = (const char *) strstr (
					(const char *) bSdp, "a=control:")) ==
					(const char *) NULL)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILE_SDPTEXTWRONG, 1, (const char *) bSdp);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bSdp. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
				pBeginMediaControl			+= strlen ("a=control:");

				if ((pEndMediaControl = (const char *) strchr (
					pBeginMediaControl, '\r')) == (const char *) NULL &&
					(pEndMediaControl = (const char *) strchr (
					pBeginMediaControl, '\n')) == (const char *) NULL &&
					(pEndMediaControl = (const char *) strchr (
					pBeginMediaControl, '\0')) == (const char *) NULL)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILE_SDPTEXTWRONG, 1, (const char *) bSdp);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bSdp. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				memcpy (mtiMP4TrackInfo. _pTrackName, pBeginMediaControl,
					pEndMediaControl - pBeginMediaControl);
				mtiMP4TrackInfo. _pTrackName [
					pEndMediaControl - pBeginMediaControl]		= '\0';

				if (bSdp. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return errNoError;
				}
			}
		}
		else
		{
			sprintf (mtiMP4TrackInfo. _pTrackName, "trackID/%lu",
				mtiMP4TrackInfo. _ulTrackIdentifier);
		}

		// initialize object type indication & codec
		// insert in _vMP4TracksInfo
		mtiMP4TrackInfo. _ulObjectTypeIndication			=
			(unsigned long) -1;
		mtiMP4TrackInfo. _cCodecUsed						=
			MP4Atom:: MP4F_CODEC_UNKNOWN;

		if (!strcmp (mtiMP4TrackInfo. _pHandlerType, MP4_AUDIO_TRACK_TYPE))
		{
			MP4DecoderConfigDescr_p			pmaMediaDecoderConfigDescr;
			Error_t							errSearch;
			MP4SamrAtom_p					pmaSamrAtom;
			MP4SawbAtom_p					pmaSawbAtom;
			MP4Atom_p						pmaAtom;


			// this kind of path is good for ISO codec
			strcpy (pAtomPath,
		"mdia:0:minf:0:stbl:0:stsd:0:mp4a:0:esds:0:ESDescr:0:DecoderConfigDescr:0");

			if ((errSearch = (mtiMP4TrackInfo. _pmaTrakAtom) -> searchAtom (
				pAtomPath, false, &pmaAtom)) != errNoError)
			{
				//Error err = MP4FileErrors (__FILE__, __LINE__,
				//	MP4F_MP4ATOM_SEARCHATOM_FAILED,
				//	1, pAtomPath);
				//_ptTracer -> trace (Tracer:: TRACER_LERRR,
				//	(const char *) err, __FILE__, __LINE__);

				// return err;
			}
			else
				pmaMediaDecoderConfigDescr		=
					(MP4DecoderConfigDescr_p) pmaAtom;

			if (errSearch == errNoError)
			{
				if (pmaMediaDecoderConfigDescr -> getObjectTypeIndication (
					&(mtiMP4TrackInfo. _ulObjectTypeIndication)) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				mtiMP4TrackInfo. _cCodecUsed		= MP4Atom:: MP4F_CODEC_AAC;

				_vMP4TracksInfo. insert (_vMP4TracksInfo. end (), mtiMP4TrackInfo);

				continue;
			}

			strcpy (pAtomPath, "mdia:0:minf:0:stbl:0:stsd:0:samr:0");

			if ((errSearch = (mtiMP4TrackInfo. _pmaTrakAtom) -> searchAtom (
				pAtomPath, false, &pmaAtom)) != errNoError)
			{
				//Error err = MP4FileErrors (__FILE__, __LINE__,
				//	MP4F_MP4FILE_TRACKNOTRECONIZED_FAILED,
				//	1, mtiMP4TrackInfo. _ulTrackIdentifier);
				//_ptTracer -> trace (Tracer:: TRACER_LERRR,
				//	(const char *) err, __FILE__, __LINE__);

				//return err;
			}
			else
				pmaSamrAtom		= (MP4SamrAtom_p) pmaAtom;

			if (errSearch == errNoError)
			{
				mtiMP4TrackInfo. _cCodecUsed						=
					MP4Atom:: MP4F_CODEC_AMRNB;

				_vMP4TracksInfo. insert (_vMP4TracksInfo. end (), mtiMP4TrackInfo);

				continue;
			}

			strcpy (pAtomPath, "mdia:0:minf:0:stbl:0:stsd:0:sawb:0");

			if ((errSearch = (mtiMP4TrackInfo. _pmaTrakAtom) -> searchAtom (
				pAtomPath, false, &pmaAtom)) != errNoError)
			{
				//Error err = MP4FileErrors (__FILE__, __LINE__,
				//	MP4F_MP4FILE_TRACKNOTRECONIZED_FAILED,
				//	1, mtiMP4TrackInfo. _ulTrackIdentifier);
				//_ptTracer -> trace (Tracer:: TRACER_LERRR,
				//	(const char *) err, __FILE__, __LINE__);

				// return err;
			}
			else
				pmaSawbAtom		= (MP4SawbAtom_p) pmaAtom;

			if (errSearch == errNoError)
			{
				mtiMP4TrackInfo. _cCodecUsed						=
					MP4Atom:: MP4F_CODEC_AMRWB;

				_vMP4TracksInfo. insert (_vMP4TracksInfo. end (), mtiMP4TrackInfo);

				continue;
			}
		}
		else if (!strcmp (mtiMP4TrackInfo. _pHandlerType, MP4_VIDEO_TRACK_TYPE))
		{
			MP4DecoderConfigDescr_p			pmaMediaDecoderConfigDescr;
			MP4S263Atom_p					pmaS263Atom;
			Error_t							errSearch;
			MP4Atom_p						pmaAtom;

			
			// this kind of path is good for ISO codec
			strcpy (pAtomPath,
		"mdia:0:minf:0:stbl:0:stsd:0:mp4v:0:esds:0:ESDescr:0:DecoderConfigDescr:0");

			if ((errSearch = (mtiMP4TrackInfo. _pmaTrakAtom) -> searchAtom (
				pAtomPath, false, &pmaAtom)) != errNoError)
			{
				//Error err = MP4FileErrors (__FILE__, __LINE__,
				//	MP4F_MP4ATOM_SEARCHATOM_FAILED,
				//	1, pAtomPath);
				//_ptTracer -> trace (Tracer:: TRACER_LERRR,
				//	(const char *) err, __FILE__, __LINE__);

				//return err;
			}
			else
				pmaMediaDecoderConfigDescr		=
					(MP4DecoderConfigDescr_p) pmaAtom;

			if (errSearch == errNoError)
			{
				if (pmaMediaDecoderConfigDescr -> getObjectTypeIndication (
					&(mtiMP4TrackInfo. _ulObjectTypeIndication)) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				mtiMP4TrackInfo. _cCodecUsed		= MP4Atom:: MP4F_CODEC_MPEG4;

				_vMP4TracksInfo. insert (_vMP4TracksInfo. end (), mtiMP4TrackInfo);

				continue;
			}

			strcpy (pAtomPath, "mdia:0:minf:0:stbl:0:stsd:0:s263:0");

			if ((errSearch = (mtiMP4TrackInfo. _pmaTrakAtom) -> searchAtom (
				pAtomPath, false, &pmaAtom)) != errNoError)
			{
				// Error err = MP4FileErrors (__FILE__, __LINE__,
				//	MP4F_MP4FILE_TRACKNOTRECONIZED_FAILED,
				//	1, mtiMP4TrackInfo. _ulTrackIdentifier);
				//_ptTracer -> trace (Tracer:: TRACER_LERRR,
				//	(const char *) err, __FILE__, __LINE__);

				//return err;
			}
			else
				pmaS263Atom		= (MP4S263Atom_p) pmaAtom;

			if (errSearch == errNoError)
			{
				mtiMP4TrackInfo. _cCodecUsed						=
					MP4Atom:: MP4F_CODEC_H263;

				_vMP4TracksInfo. insert (_vMP4TracksInfo. end (), mtiMP4TrackInfo);

				continue;
			}
		}
		else
		{
		}
		*/
	}


	return errNoError;
}
