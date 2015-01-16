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


#ifndef RequestForVidiator_h
	#define RequestForVidiator_h

	#include "RequestForEncoder.h"
	#include "Tracer.h"


	#define SFE_MAXIPADDRESSLENGTH					512 + 1
	#define SFE_MAXHTTPREQUESTLENGTH				1024 * 3 + 1
	#define SFE_MAXHTTPRESPONSELENGTH				1024 * 3 + 1
	#define SFE_NEWLINE								"\n"


	typedef class RequestForVidiator: public RequestForEncoder {

		private:
			unsigned long			_ulPort;
			char					_pIpAddress [SFE_MAXIPADDRESSLENGTH];

			virtual Error createProfileInstance (const char *pProfileName,
				const char *pSourceFileName, Buffer_p pbProfilePath);

			virtual Error encode (const char *pProfilePath,
				const char *pProfileName, const char *pSourceFileName);

			virtual Error encodeFinished (Boolean_t bEncodeOK);

		public:
			RequestForVidiator (void);

			~RequestForVidiator (void);

			Error init (const char *pName,
				const char *pSourcesPath, const char *pEncodedPath,
				const char *pProfilesPath, const char *pTemporaryPath,
				const char *pFTPUser, const char *pFTPPassword,
				const char *pFTPHost, const char *pFTPRootPath,
				const char *pFilesList,
				const char *pIpAddress, unsigned long ulPort,
				Tracer_p ptTracer);

			Error finish (void);


	} RequestForVidiator_t, *RequestForVidiator_p;

#endif

