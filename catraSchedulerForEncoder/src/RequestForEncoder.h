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


#ifndef RequestForEncoder_h
	#define RequestForEncoder_h

	#include "PosixThread.h"
	#include "Tracer.h"
	#include "SchedulerForEncoderErrors.h"


	#define SFE_MAXNAMELENGTH							512 + 1
	#define SFE_MAXPATHLENGTH							1024 + 1
	#define SFE_MAXHOSTLENGTH							512 + 1
	#define SFE_MAXUSERLENGTH							512 + 1
	#define SFE_MAXPASSWORDLENGTH						512 + 1


	typedef class RequestForEncoder: public PosixThread {

		public:
			typedef struct FTPFile {
				char			_pSourceFileName [SFE_MAXPATHLENGTH];
				char			_pProfileName [SFE_MAXPATHLENGTH];
				Boolean_t		_bFileEncoded;
			} FTPFile_t, *FTPFile_p;

		private:
			FTPFile_p				_pffFTPFiles;
			Buffer_t				_bFilesListContent;

			char					_pFTPUser [SFE_MAXUSERLENGTH];
			char					_pFTPPassword [SFE_MAXPASSWORDLENGTH];
			char					_pFTPHost [SFE_MAXHOSTLENGTH];
			char					_pFTPRootPath [SFE_MAXPATHLENGTH];
			char					_pFilesList [SFE_MAXPATHLENGTH];

			virtual Error run (void);

			virtual Error createProfileInstance (const char *pProfileName,
				const char *pSourceFileName, Buffer_p pbProfilePath) = 0;

			virtual Error encode (const char *pProfilePath,
				const char *pProfileName, const char *pSourceFileName) = 0;

			virtual Error encodeFinished (Boolean_t bEncodeOK) = 0;

			Error createDirectories (void);

			Error removeDirectories (void);

		protected:
			Tracer_p				_ptTracer;
			char					_pName [SFE_MAXNAMELENGTH];
			char					_pTemporaryPath [SFE_MAXPATHLENGTH];
			char					_pProfilesPath [SFE_MAXPATHLENGTH];
			char					_pSourcesPath [SFE_MAXPATHLENGTH];
			char					_pEncodedPath [SFE_MAXPATHLENGTH];


			Error downloadFilesList (FTPFile_p *pffFTPFiles,
				unsigned long *pulFilesNumber);

			Error removeFilesList (void);

			Error updateFilesList (
				const char *pSourceFileName,
				const char *pProfileName,
				Boolean_t bFileEncoded);

			Error uploadFilesList (void);

			Error downloadSourceFile (
				const char *pSourceFileName);

			Error removeSourceFile (const char *pSourceFileName);

			Error uploadEncodedFile (
				const char *pSourceFileName, const char *pProfileName);

			virtual Error removeEncodedFile (const char *pSourceFileName,
				const char *pProfileName);

		public:
			RequestForEncoder (void);

			~RequestForEncoder (void);

			Error init (const char *pName,
				const char *pSourcesPath, const char *pEncodedPath,
				const char *pProfilesPath, const char *pTemporaryPath,
				const char *pFTPUser, const char *pFTPPassword,
				const char *pFTPHost, const char *pFTPRootPath,
				const char *pFilesList, Tracer_p ptTracer);

			virtual Error finish (void);

	} RequestForEncoder_t, *RequestForEncoder_p;

#endif

