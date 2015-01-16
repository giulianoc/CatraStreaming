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

#include "RequestForEncoder.h"
#include "FileIO.h"
#include "StringTokenizer.h"
#include "SchedulerForEncoderMessages.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <iostream.h>
#include <stdio.h>

#include "ncftp.h"


Tracer_p			pgtTracer;


void MyFTPLogProc (const FTPCIPtr pFTPConnectionInfo, char *pFTPLog)

{

	{
		Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
			SFE_FTPLOG, 1, pFTPLog);
		pgtTracer -> trace (Tracer:: TRACER_LDBG6, (const char *) msg,
			__FILE__, __LINE__);
	}

}

RequestForEncoder:: RequestForEncoder (void): PosixThread ()

{

}


RequestForEncoder:: ~RequestForEncoder (void)

{

}


Error RequestForEncoder:: init (const char *pName,
	const char *pSourcesPath, const char *pEncodedPath,
	const char *pProfilesPath, const char *pTemporaryPath,
	const char *pFTPUser, const char *pFTPPassword,
	const char *pFTPHost, const char *pFTPRootPath,
	const char *pFilesList, Tracer_p ptTracer)

{

	strcpy (_pName, pName);
	strcpy (_pSourcesPath, pSourcesPath);
	strcpy (_pEncodedPath, pEncodedPath);
	strcpy (_pProfilesPath, pProfilesPath);
	strcpy (_pTemporaryPath, pTemporaryPath);
	strcpy (_pFTPUser, pFTPUser);
	strcpy (_pFTPPassword, pFTPPassword);
	strcpy (_pFTPHost, pFTPHost);
	strcpy (_pFTPRootPath, pFTPRootPath);
	strcpy (_pFilesList, pFilesList);

	_ptTracer			= ptTracer;
	pgtTracer			= ptTracer;

	_pffFTPFiles		= (FTPFile_p) NULL;

	if (_bFilesListContent. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (PosixThread:: init () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_bFilesListContent. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}


	return errNoError;
}


Error RequestForEncoder:: finish (void)

{

	if (PosixThread:: finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);
	}

	if (_bFilesListContent. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);
	}

	if (_pffFTPFiles != (FTPFile_p) NULL)
		delete [] _pffFTPFiles;


	return errNoError;
}


Error RequestForEncoder:: run (void)

{

	FTPFile_p					pffFTPFiles;
	unsigned long				ulFilesNumber;
	unsigned long				ulFileIndex;
	Buffer_t					bProfilePath;
	Error						errRemove;


	{
		Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
			SFE_REQUESTFORENCODER_STARTENCODINGSESSION,
			1, _pName);
		_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	{
		Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
			SFE_REQUESTFORENCODER_DOWNLOADFILESLIST,
			1, _pName);
		_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (createDirectories () != errNoError)
	{
		_erThreadReturn = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_REQUESTFORENCODER_CREATEDIRECTORIES_FAILED,
			1, _pName);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
			__FILE__, __LINE__);

		_ptTracer -> flushOfTraces ();

		return _erThreadReturn;
	}

	if (downloadFilesList (&pffFTPFiles, &ulFilesNumber) !=
		errNoError)
	{
		_erThreadReturn = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_REQUESTFORENCODER_DOWNLOADFILESLIST_FAILED,
			1, _pName);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
			__FILE__, __LINE__);

		removeDirectories ();

		_ptTracer -> flushOfTraces ();

		return _erThreadReturn;
	}

	if (bProfilePath. init () != errNoError)
	{
		_erThreadReturn = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
			__FILE__, __LINE__);

		if (removeFilesList () != errNoError)
		{
			Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
				SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
				1, _pName);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		removeDirectories ();

		_ptTracer -> flushOfTraces ();

		return _erThreadReturn;
	}

	for (ulFileIndex = 0; ulFileIndex < ulFilesNumber; ulFileIndex++)
	{
		if ((pffFTPFiles [ulFileIndex]). _bFileEncoded)
		{
			{
				Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
					SFE_REQUESTFORENCODER_FOUNDFILEALREADYENCODED,
					3, _pName,
					(pffFTPFiles [ulFileIndex]). _pSourceFileName,
					(pffFTPFiles [ulFileIndex]). _pProfileName);
				_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
					__FILE__, __LINE__);
			}

			continue;
		}

		{
			Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
				SFE_REQUESTFORENCODER_FILETOENCODE,
				3, _pName, (pffFTPFiles [ulFileIndex]). _pSourceFileName,
				(pffFTPFiles [ulFileIndex]). _pProfileName);
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		{
			Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
				SFE_REQUESTFORENCODER_DOWNLOADSOURCEFILE,
				2, _pName, (pffFTPFiles [ulFileIndex]). _pSourceFileName);
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (downloadSourceFile (
			(pffFTPFiles [ulFileIndex]). _pSourceFileName) != errNoError)
		{
			_erThreadReturn = SchedulerForEncoderErrors (__FILE__, __LINE__,
				SFE_REQUESTFORENCODER_DOWNLOADSOURCEFILE_FAILED,
				1, _pName);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
				__FILE__, __LINE__);

			if (bProfilePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (removeFilesList () != errNoError)
			{
				Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
					SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
					1, _pName);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			removeDirectories ();

			_ptTracer -> flushOfTraces ();

			return _erThreadReturn;
		}

		{
			Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
				SFE_REQUESTFORVIDIATOR_CREATEPROFILE,
				3, _pName, (pffFTPFiles [ulFileIndex]). _pProfileName,
				(pffFTPFiles [ulFileIndex]). _pSourceFileName);
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (createProfileInstance (
			(pffFTPFiles [ulFileIndex]). _pProfileName,
			(pffFTPFiles [ulFileIndex]). _pSourceFileName,
			&bProfilePath) != errNoError)
		{
			_erThreadReturn = SchedulerForEncoderErrors (__FILE__, __LINE__,
				SFE_REQUESTFORVIDIATOR_CREATEPROFILEINSTANCE_FAILED,
				1, _pName);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
				__FILE__, __LINE__);

			if (removeSourceFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName) !=
				errNoError)
			{
				Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
					SFE_REQUESTFORENCODER_REMOVESOURCEFILE_FAILED,
					1, _pName);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (bProfilePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (removeFilesList () != errNoError)
			{
				Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
					SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
					1, _pName);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			removeDirectories ();

			_ptTracer -> flushOfTraces ();

			return _erThreadReturn;
		}

		{
			Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
				SFE_REQUESTFORENCODER_ENCODE,
				3, _pName, (pffFTPFiles [ulFileIndex]). _pProfileName,
				(pffFTPFiles [ulFileIndex]). _pSourceFileName);
			_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
				__FILE__, __LINE__);
		}

		if (encode ((const char *) bProfilePath,
			(pffFTPFiles [ulFileIndex]). _pProfileName,
			(pffFTPFiles [ulFileIndex]). _pSourceFileName) != errNoError)
		{
			(pffFTPFiles [ulFileIndex]). _bFileEncoded		= false;

			_erThreadReturn = SchedulerForEncoderErrors (__FILE__, __LINE__,
				SFE_REQUESTFORENCODER_ENCODE_FAILED,
				1, _pName);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
				__FILE__, __LINE__);

			if ((errRemove = FileIO:: remove ((const char *) bProfilePath)) != errNoError)
			{
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRemove,
					__FILE__, __LINE__);
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_REMOVE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (removeSourceFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName) !=
				errNoError)
			{
				Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
					SFE_REQUESTFORENCODER_REMOVESOURCEFILE_FAILED,
					1, _pName);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (bProfilePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (removeFilesList () != errNoError)
			{
				Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
					SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
					1, _pName);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			removeDirectories ();

			_ptTracer -> flushOfTraces ();

			return _erThreadReturn;
		}

		(pffFTPFiles [ulFileIndex]). _bFileEncoded		= true;

		// if ((pffFTPFiles [ulFileIndex]). _bFileEncoded)
		{
			time_t				tmRequestTime;
			char				pEncodedFilePath [SFE_MAXPATHLENGTH];
			Boolean_t			bEncodedFileExist;

			sprintf (pEncodedFilePath, "%s\\%s\\%s_%s.3gp",
				_pEncodedPath, _pName,
				(pffFTPFiles [ulFileIndex]). _pSourceFileName,
				(pffFTPFiles [ulFileIndex]). _pProfileName);

			tmRequestTime			= time (NULL);
			bEncodedFileExist			= false;

			// wait max 1 hour to wait the encoded file
			while (!bEncodedFileExist && time (NULL) - tmRequestTime < 60 * 60)
			{
				if (FileIO:: exist (pEncodedFilePath, &bEncodedFileExist) != errNoError)
				{
					_erThreadReturn = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_EXIST_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
						__FILE__, __LINE__);

					if (encodeFinished (false) != errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_ENCODEFINISHED_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (removeEncodedFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName,
						(pffFTPFiles [ulFileIndex]). _pProfileName) !=
						errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVEENCODEDFILE_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if ((errRemove = FileIO:: remove ((const char *) bProfilePath)) !=
						errNoError)
					{
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRemove,
							__FILE__, __LINE__);
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_FILEIO_REMOVE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (removeSourceFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName) !=
						errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVESOURCEFILE_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (bProfilePath. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (removeFilesList () != errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					removeDirectories ();

					_ptTracer -> flushOfTraces ();

					return _erThreadReturn;
				}

				{
					Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
						SFE_REQUESTFORENCODER_WAITENCODEDFILE,
						2, _pName, (pffFTPFiles [ulFileIndex]). _pSourceFileName);
					_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
						__FILE__, __LINE__);
				}

				if (PosixThread:: getSleep (30, 0) != errNoError)
				{
					_erThreadReturn = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PTHREAD_GETSLEEP_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
						__FILE__, __LINE__);

					if (encodeFinished (false) != errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_ENCODEFINISHED_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (removeEncodedFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName,
						(pffFTPFiles [ulFileIndex]). _pProfileName) != errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVEENCODEDFILE_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if ((errRemove = FileIO:: remove ((const char *) bProfilePath)) !=
						errNoError)
					{
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRemove,
							__FILE__, __LINE__);
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_FILEIO_REMOVE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (removeSourceFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName) !=
						errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVESOURCEFILE_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (bProfilePath. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (removeFilesList () != errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					removeDirectories ();

					_ptTracer -> flushOfTraces ();

					return _erThreadReturn;
				}
			}

			if (encodeFinished (bEncodedFileExist) != errNoError)
			{
				_erThreadReturn = SchedulerForEncoderErrors (__FILE__, __LINE__,
					SFE_REQUESTFORENCODER_ENCODEFINISHED_FAILED,
					1, _pName);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
					__FILE__, __LINE__);

				if (removeEncodedFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName,
					(pffFTPFiles [ulFileIndex]). _pProfileName) != errNoError)
				{
					Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_REQUESTFORENCODER_REMOVEENCODEDFILE_FAILED,
						1, _pName);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
						__FILE__, __LINE__);
				}

				if ((errRemove = FileIO:: remove ((const char *) bProfilePath)) !=
					errNoError)
				{
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRemove,
						__FILE__, __LINE__);
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_REMOVE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
						__FILE__, __LINE__);
				}

				if (removeSourceFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName) !=
					errNoError)
				{
					Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_REQUESTFORENCODER_REMOVESOURCEFILE_FAILED,
						1, _pName);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
						__FILE__, __LINE__);
				}

				if (bProfilePath. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
						__FILE__, __LINE__);
				}

				if (removeFilesList () != errNoError)
				{
					Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
						1, _pName);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
						__FILE__, __LINE__);
				}

				removeDirectories ();

				_ptTracer -> flushOfTraces ();

				return _erThreadReturn;
			}

			if (!bEncodedFileExist)
			{
				Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
					SFE_REQUESTFORENCODER_ENCODEDFILENOTPRODUCED,
					2, _pName, pEncodedFilePath);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}
			else
			{
				{
					Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
						SFE_REQUESTFORENCODER_UPLOADENCODEDFILE,
						2, _pName,
						(pffFTPFiles [ulFileIndex]). _pSourceFileName);
					_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
						__FILE__, __LINE__);
				}

				if (uploadEncodedFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName,
					(pffFTPFiles [ulFileIndex]). _pProfileName) !=
					errNoError)
				{
					_erThreadReturn = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_REQUESTFORENCODER_UPLOADENCODEDFILE_FAILED,
						1, _pName);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
						__FILE__, __LINE__);

					if (removeEncodedFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName,
						(pffFTPFiles [ulFileIndex]). _pProfileName) != errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVEENCODEDFILE_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if ((errRemove = FileIO:: remove ((const char *) bProfilePath)) !=
						errNoError)
					{
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRemove,
							__FILE__, __LINE__);
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_FILEIO_REMOVE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (removeSourceFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName) !=
						errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVESOURCEFILE_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (bProfilePath. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (removeFilesList () != errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					removeDirectories ();

					_ptTracer -> flushOfTraces ();

					return _erThreadReturn;
				}

				if (updateFilesList (
					(pffFTPFiles [ulFileIndex]). _pSourceFileName,
					(pffFTPFiles [ulFileIndex]). _pProfileName,
					true) != errNoError)
				{
					_erThreadReturn = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_REQUESTFORENCODER_UPDATEFILESLIST_FAILED,
						1, _pName);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
						__FILE__, __LINE__);

					if (removeEncodedFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName,
						(pffFTPFiles [ulFileIndex]). _pProfileName) !=
						errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVEENCODEDFILE_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if ((errRemove = FileIO:: remove ((const char *) bProfilePath)) !=
						errNoError)
					{
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRemove,
							__FILE__, __LINE__);
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_FILEIO_REMOVE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (removeSourceFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName) !=
						errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVESOURCEFILE_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (bProfilePath. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (removeFilesList () != errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					removeDirectories ();

					_ptTracer -> flushOfTraces ();

					return _erThreadReturn;
				}

				{
					Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
						SFE_REQUESTFORENCODER_UPLOADFILESLISTUPDATED,
						1, _pName);
					_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
						__FILE__, __LINE__);
				}

				if (uploadFilesList () != errNoError)
				{
					_erThreadReturn = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_REQUESTFORENCODER_UPLOADFILESLIST_FAILED,
						1, _pName);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
						__FILE__, __LINE__);

					if (removeEncodedFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName,
						(pffFTPFiles [ulFileIndex]). _pProfileName) !=
						errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVEENCODEDFILE_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if ((errRemove = FileIO:: remove ((const char *) bProfilePath)) !=
						errNoError)
					{
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRemove,
							__FILE__, __LINE__);
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_FILEIO_REMOVE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (removeSourceFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName) !=
						errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVESOURCEFILE_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (bProfilePath. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (removeFilesList () != errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					removeDirectories ();

					_ptTracer -> flushOfTraces ();

					return _erThreadReturn;
				}

				if (removeEncodedFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName,
					(pffFTPFiles [ulFileIndex]). _pProfileName) !=
					errNoError)
				{
					_erThreadReturn = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_REQUESTFORENCODER_REMOVEENCODEDFILE_FAILED,
						1, _pName);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
						__FILE__, __LINE__);

					if ((errRemove = FileIO:: remove ((const char *) bProfilePath)) !=
						errNoError)
					{
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRemove,
							__FILE__, __LINE__);
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_FILEIO_REMOVE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (removeSourceFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName) !=
						errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVESOURCEFILE_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (bProfilePath. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					if (removeFilesList () != errNoError)
					{
						Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
							SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
							1, _pName);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);
					}

					removeDirectories ();

					_ptTracer -> flushOfTraces ();

					return _erThreadReturn;
				}
			}
		}

		if ((errRemove = FileIO:: remove ((const char *) bProfilePath)) !=
			errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRemove,
				__FILE__, __LINE__);
			_erThreadReturn = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_REMOVE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
				__FILE__, __LINE__);

			if (removeSourceFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName) !=
				errNoError)
			{
				Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
					SFE_REQUESTFORENCODER_REMOVESOURCEFILE_FAILED,
					1, _pName);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (bProfilePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (removeFilesList () != errNoError)
			{
				Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
					SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
					1, _pName);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			removeDirectories ();

			_ptTracer -> flushOfTraces ();

			return _erThreadReturn;
		}

		if (removeSourceFile ((pffFTPFiles [ulFileIndex]). _pSourceFileName) !=
			errNoError)
		{
			_erThreadReturn = SchedulerForEncoderErrors (__FILE__, __LINE__,
				SFE_REQUESTFORENCODER_REMOVESOURCEFILE_FAILED,
				1, _pName);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
				__FILE__, __LINE__);

			if (bProfilePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (removeFilesList () != errNoError)
			{
				Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
					SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
					1, _pName);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			removeDirectories ();

			_ptTracer -> flushOfTraces ();

			return _erThreadReturn;
		}
	}

	if (bProfilePath. finish () != errNoError)
	{
		_erThreadReturn = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
			__FILE__, __LINE__);

		if (removeFilesList () != errNoError)
		{
			Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
				SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
				1, _pName);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		removeDirectories ();

		_ptTracer -> flushOfTraces ();

		return _erThreadReturn;
	}

	if (removeFilesList () != errNoError)
	{
		_erThreadReturn = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
			1, _pName);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) _erThreadReturn,
			__FILE__, __LINE__);

		removeDirectories ();

		_ptTracer -> flushOfTraces ();

		return _erThreadReturn;
	}

	removeDirectories ();

	{
		Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
			SFE_REQUESTFORENCODER_FINISHENCODINGSESSION,
			1, _pName);
		_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	_ptTracer -> flushOfTraces ();


	return _erThreadReturn;
}


Error RequestForEncoder:: createDirectories (void)

{

	Buffer_t			bDirectory;


	if (bDirectory. init (_pTemporaryPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bDirectory. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bDirectory. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	// I will not handle the error in case the directory already exists
	#ifdef WIN32
		_mkdir ((const char *) bDirectory);
	#else
		mkdir ((const char *) bDirectory,
			S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	#endif

	if (bDirectory. setBuffer (_pSourcesPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		removeDirectories ();

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bDirectory. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		removeDirectories ();

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bDirectory. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		removeDirectories ();

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	// I will not handle the error in case the directory already exists
	#ifdef WIN32
		_mkdir ((const char *) bDirectory);
	#else
		mkdir ((const char *) bDirectory,
			S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	#endif

	if (bDirectory. setBuffer (_pEncodedPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		removeDirectories ();

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bDirectory. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		removeDirectories ();

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bDirectory. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		removeDirectories ();

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	// I will not handle the error in case the directory already exists
	#ifdef WIN32
		_mkdir ((const char *) bDirectory);
	#else
		mkdir ((const char *) bDirectory,
			S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	#endif

	if (bDirectory. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		removeDirectories ();

		return err;
	}


	return errNoError;
}


Error RequestForEncoder:: removeDirectories (void)

{

	Buffer_t			bDirectory;


	if (bDirectory. init (_pTemporaryPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bDirectory. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bDirectory. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	// I will not handle the error in case the directory doesn't exists
	#ifdef WIN32
		_rmdir ((const char *) bDirectory);
	#else
		rmdir ((const char *) bDirectory);
	#endif

	if (bDirectory. setBuffer (_pSourcesPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bDirectory. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bDirectory. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	// I will not handle the error in case the directory doesn't exists
	#ifdef WIN32
		_rmdir ((const char *) bDirectory);
	#else
		rmdir ((const char *) bDirectory);
	#endif

	if (bDirectory. setBuffer (_pEncodedPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bDirectory. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bDirectory. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bDirectory. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	// I will not handle the error in case the directory doesn't exists
	#ifdef WIN32
		_rmdir ((const char *) bDirectory);
	#else
		rmdir ((const char *) bDirectory);
	#endif

	if (bDirectory. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RequestForEncoder:: downloadFilesList (FTPFile_p *pffFTPFiles,
	unsigned long *pulFilesNumber)

{

	FTPLibraryInfo					li;
	FTPConnectionInfo				ci;
	int								iFTPRet;
	Buffer_t						bFilesListPath;
	StringTokenizer_t				stNewLineTokenizer;
	const char						*pToken;
	long							lFilesNumber;
	long							lFileIndex;


	if (_pffFTPFiles != (FTPFile_p) NULL)
		delete [] _pffFTPFiles;

	if (bFilesListPath. init (_pTemporaryPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	
	if (bFilesListPath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	FTPInitLibrary (&li);
	FTPInitConnectionInfo (&li, &ci, kDefaultFTPBufSize);
	// ci. debugLog			= stdout;
	ci. debugLogProc		= MyFTPLogProc;
	strcpy (ci. user, _pFTPUser);
	strcpy (ci. pass, _pFTPPassword);
	strcpy (ci. host, _pFTPHost);

	if ((iFTPRet = FTPOpenHost (&ci)) < 0)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_FTPOPENHOST_FAILED,
			2, _pName, FTPStrError (iFTPRet));
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if ((iFTPRet = FTPChdir (&ci, _pFTPRootPath)) < 0)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_FTPCHDIR_FAILED,
			2, _pName, FTPStrError (iFTPRet));
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		FTPCloseHost (&ci);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if ((iFTPRet = FTPGetFiles3 (&ci, _pFilesList,
		(const char *) bFilesListPath,
		kRecursiveNo, kGlobNo, kTypeBinary, kResumeNo, kAppendNo,
		kDeleteNo, kTarNo, kNoFTPConfirmResumeDownloadProc, 0)) < 0)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_FTPGETFILES3_FAILED,
			2, _pName, FTPStrError (iFTPRet));
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (removeFilesList () != errNoError)
		{
			Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
				SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
				1, _pName);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		FTPCloseHost (&ci);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if ((iFTPRet = FTPCloseHost (&ci)) < 0)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_FTPCLOSEHOST_FAILED,
			2, _pName, FTPStrError (iFTPRet));
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. append (_pFilesList) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (_bFilesListContent. readBufferFromFile (
		(const char *) bFilesListPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_READBUFFERFROMFILE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (stNewLineTokenizer. init ((const char *) _bFilesListContent,
		-1, "\n") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_STRINGTOKENIZER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	
		return err;
	}

	if (stNewLineTokenizer. tokensNumber (&lFilesNumber) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_STRINGTOKENIZER_TOKENSNUMBER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (stNewLineTokenizer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((_pffFTPFiles = new FTPFile_t [lFilesNumber]) ==
		(FTPFile_p) NULL)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_STRINGTOKENIZER_TOKENSNUMBER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (stNewLineTokenizer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	for (lFileIndex = 0; lFileIndex < lFilesNumber; lFileIndex++)
	{
		if (stNewLineTokenizer. nextToken (&pToken) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_STRINGTOKENIZER_NEXTTOKEN_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] _pffFTPFiles;

			if (stNewLineTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (strchr (pToken, ',') == (char *) NULL)
		{
			/*
			Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
				SFE_REQUESTFORENCODER_FILESLISTWRONG,
				1, pToken);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] _pffFTPFiles;

			if (stNewLineTokenizer. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_STRINGTOKENIZER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
			*/
			break;
		}

		strncpy ((_pffFTPFiles [lFileIndex]). _pSourceFileName,
			pToken, strchr (pToken, ',') - pToken);
		(_pffFTPFiles [lFileIndex]). _pSourceFileName [
			strchr (pToken, ',') - pToken]			= '\0';

		pToken			+= (strchr (pToken, ',') - pToken + 1);

		if (strchr (pToken, ',') == (char *) NULL)
		{
			strcpy ((_pffFTPFiles [lFileIndex]). _pProfileName,
				pToken);
			(_pffFTPFiles [lFileIndex]). _bFileEncoded			=
				false;
		}
		else
		{
			strncpy ((_pffFTPFiles [lFileIndex]). _pProfileName,
				pToken, strchr (pToken, ',') - pToken);
			(_pffFTPFiles [lFileIndex]). _pProfileName [
				strchr (pToken, ',') - pToken]			= '\0';

			pToken			+= (strchr (pToken, ',') - pToken + 1);

			if (!strcmp (pToken, "OK"))
				(_pffFTPFiles [lFileIndex]). _bFileEncoded			=
					true;
			else
			{
				Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
					SFE_REQUESTFORENCODER_FILESLISTWRONG,
					1, pToken);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				delete [] _pffFTPFiles;

				if (stNewLineTokenizer. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_STRINGTOKENIZER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	if (stNewLineTokenizer. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_STRINGTOKENIZER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete [] _pffFTPFiles;

		return err;
	}

	*pffFTPFiles			= _pffFTPFiles;
	// *pulFilesNumber			= lFilesNumber;
	*pulFilesNumber			= lFileIndex;


	return errNoError;
}


Error RequestForEncoder:: removeFilesList (void)

{

	Buffer_t						bFilesListPath;
	Error							errRemove;


	if (bFilesListPath. init (_pTemporaryPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bFilesListPath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. append (_pFilesList) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if ((errRemove = FileIO:: remove ((const char *) bFilesListPath)) !=
		errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRemove,
			__FILE__, __LINE__);
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_REMOVE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RequestForEncoder:: updateFilesList (
	const char *pSourceFileName,
	const char *pProfileName,
	Boolean_t bFileEncoded)

{

	Buffer_t						bFilesListOldRow;
	Buffer_t						bFilesListNewRow;
	Buffer_t						bFilesListPath;


	if (bFilesListOldRow. init (pSourceFileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bFilesListNewRow. init (pSourceFileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListOldRow. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListOldRow. append (",") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListNewRow. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bFilesListOldRow. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListNewRow. append (",") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListNewRow. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bFilesListOldRow. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListOldRow. append (pProfileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListNewRow. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bFilesListOldRow. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListNewRow. append (pProfileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListNewRow. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bFilesListOldRow. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListNewRow. append (",OK") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListNewRow. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bFilesListOldRow. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (_bFilesListContent. substitute ((const char *) bFilesListOldRow,
		(const char *) bFilesListNewRow) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SUBSTITUTE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListNewRow. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bFilesListOldRow. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListNewRow. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListOldRow. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListOldRow. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bFilesListPath. init (_pTemporaryPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	
	if (bFilesListPath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. append (_pFilesList) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (_bFilesListContent. writeBufferOnFile ((const char *) bFilesListPath) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_WRITEBUFFERONFILE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return errNoError;
	}


	return errNoError;
}


Error RequestForEncoder:: uploadFilesList (void)

{

	FTPLibraryInfo					li;
	FTPConnectionInfo				ci;
	int								iFTPRet;
	Buffer_t						bFilesListPath;


	if (bFilesListPath. init (_pTemporaryPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bFilesListPath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bFilesListPath. append (_pFilesList) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	FTPInitLibrary (&li);
	FTPInitConnectionInfo (&li, &ci, kDefaultFTPBufSize);
	// ci. debugLog			= stdout;
	ci. debugLogProc		= MyFTPLogProc;
	strcpy (ci. user, _pFTPUser);
	strcpy (ci. pass, _pFTPPassword);
	strcpy (ci. host, _pFTPHost);

	if ((iFTPRet = FTPOpenHost (&ci)) < 0)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_FTPOPENHOST_FAILED,
			2, _pName, FTPStrError (iFTPRet));
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if ((iFTPRet = FTPPutFiles3 (&ci, (const char *) bFilesListPath, _pFTPRootPath,
		kRecursiveNo, kGlobNo, kTypeBinary, kAppendNo,
		NULL, NULL, kResumeNo, kDeleteNo, kNoFTPConfirmResumeUploadProc, 0)) < 0)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_FTPPUTFILES3_FAILED,
			2, _pName, FTPStrError (iFTPRet));
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		FTPCloseHost (&ci);

		if (bFilesListPath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if ((iFTPRet = FTPCloseHost (&ci)) < 0)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_FTPCLOSEHOST_FAILED,
			2, _pName, FTPStrError (iFTPRet));
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bFilesListPath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return errNoError;
	}


	return errNoError;
}


Error RequestForEncoder:: downloadSourceFile (
	const char *pSourceFileName)

{

	FTPLibraryInfo					li;
	FTPConnectionInfo				ci;
	int								iFTPRet;
	Buffer_t						bSourceFileNamePath;


	if (bSourceFileNamePath. init (_pSourcesPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bSourceFileNamePath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bSourceFileNamePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bSourceFileNamePath. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bSourceFileNamePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	FTPInitLibrary (&li);
	FTPInitConnectionInfo (&li, &ci, kDefaultFTPBufSize);
	// ci. debugLog			= stdout;
	ci. debugLogProc		= MyFTPLogProc;
	strcpy (ci. user, _pFTPUser);
	strcpy (ci. pass, _pFTPPassword);
	strcpy (ci. host, _pFTPHost);

	if ((iFTPRet = FTPOpenHost (&ci)) < 0)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_FTPOPENHOST_FAILED,
			2, _pName, FTPStrError (iFTPRet));
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bSourceFileNamePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if ((iFTPRet = FTPChdir (&ci, _pFTPRootPath)) < 0)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_FTPCHDIR_FAILED,
			2, _pName, FTPStrError (iFTPRet));
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		FTPCloseHost (&ci);

		if (bSourceFileNamePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if ((iFTPRet = FTPGetFiles3 (&ci, pSourceFileName,
		(const char *) bSourceFileNamePath,
		kRecursiveNo, kGlobNo, kTypeBinary, kResumeYes, kAppendNo,
		kDeleteNo, kTarNo, kNoFTPConfirmResumeDownloadProc, 0)) < 0)
	{
		if (iFTPRet != kErrLocalSameAsRemote)
		{
			Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
				SFE_FTPGETFILES3_FAILED,
				2, _pName, FTPStrError (iFTPRet));
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (removeSourceFile (pSourceFileName) != errNoError)
			{
				Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
					SFE_REQUESTFORENCODER_REMOVESOURCEFILE_FAILED,
					1, _pName);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			FTPCloseHost (&ci);

			if (bSourceFileNamePath. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			return err;
		}
	}

	if ((iFTPRet = FTPCloseHost (&ci)) < 0)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_FTPCLOSEHOST_FAILED,
			2, _pName, FTPStrError (iFTPRet));
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bSourceFileNamePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bSourceFileNamePath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RequestForEncoder:: removeSourceFile (const char *pSourceFileName)

{

	Buffer_t						bSourceFileNamePath;
	Error							errRemove;


	if (bSourceFileNamePath. init (_pSourcesPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bSourceFileNamePath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bSourceFileNamePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bSourceFileNamePath. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bSourceFileNamePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bSourceFileNamePath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bSourceFileNamePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bSourceFileNamePath. append (pSourceFileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bSourceFileNamePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if ((errRemove = FileIO:: remove ((const char *) bSourceFileNamePath)) !=
		errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRemove,
			__FILE__, __LINE__);
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_REMOVE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bSourceFileNamePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bSourceFileNamePath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RequestForEncoder:: uploadEncodedFile (
	const char *pSourceFileName, const char *pProfileName)

{

	FTPLibraryInfo					li;
	FTPConnectionInfo				ci;
	int								iFTPRet;
	Buffer_t						bEncodedFilePath;


	if (bEncodedFilePath. init (_pEncodedPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bEncodedFilePath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (pSourceFileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append ("_") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (pProfileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (".3gp") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	FTPInitLibrary (&li);
	FTPInitConnectionInfo (&li, &ci, kDefaultFTPBufSize);
	// ci. debugLog			= stdout;
	ci. debugLogProc		= MyFTPLogProc;
	strcpy (ci. user, _pFTPUser);
	strcpy (ci. pass, _pFTPPassword);
	strcpy (ci. host, _pFTPHost);

	if ((iFTPRet = FTPOpenHost (&ci)) < 0)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_FTPOPENHOST_FAILED,
			2, _pName, FTPStrError (iFTPRet));
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if ((iFTPRet = FTPPutFiles3 (&ci, (const char *) bEncodedFilePath, _pFTPRootPath,
		kRecursiveNo, kGlobNo, kTypeBinary, kAppendNo,
		NULL, NULL, kResumeNo, kDeleteNo, kNoFTPConfirmResumeUploadProc, 0)) < 0)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_FTPPUTFILES3_FAILED,
			2, _pName, FTPStrError (iFTPRet));
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		FTPCloseHost (&ci);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if ((iFTPRet = FTPCloseHost (&ci)) < 0)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_FTPCLOSEHOST_FAILED,
			2, _pName, FTPStrError (iFTPRet));
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RequestForEncoder:: removeEncodedFile (const char *pSourceFileName,
	const char *pProfileName)

{

	Buffer_t						bEncodedFilePath;
	Error							errRemove;

	
	if (bEncodedFilePath. init (_pEncodedPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bEncodedFilePath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (pSourceFileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append ("_") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (pProfileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (".3gp") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if ((errRemove = FileIO:: remove ((const char *) bEncodedFilePath)) !=
		errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRemove,
			__FILE__, __LINE__);
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_REMOVE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}

