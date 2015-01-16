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


#ifndef RequestForNexencoder_h
	#define RequestForNexencoder_h

	#include "RequestForEncoder.h"
	#include "Tracer.h"
	#include "TelnetClient.h"


	#define SFE_MAXIPADDRESSLENGTH					512 + 1
	#define SFE_MAXTELNETBUFFERLENGTH				1024 + 1
	#define SFE_MAXENCODERCOMMANDLENGTH				1024 * 3 + 1
	#define SFE_MAXUSERLENGTH						512 + 1
	#define SFE_MAXPASSWORDLENGTH					512 + 1


	typedef class RequestForNexencoder: public RequestForEncoder {

		private:
			unsigned long			_ulPort;
			char					_pIpAddress [SFE_MAXIPADDRESSLENGTH];
			char					_pEncoderPathName [SFE_MAXENCODERCOMMANDLENGTH];
			char					_pUser [SFE_MAXUSERLENGTH];
			char					_pPassword [SFE_MAXPASSWORDLENGTH];
			TelnetClient_t			_tcTelnetClient;

			virtual Error createProfileInstance (const char *pProfileName,
				const char *pSourceFileName, Buffer_p pbProfilePath);

			virtual Error encode (const char *pProfilePath,
				const char *pProfileName, const char *pSourceFileName);

			virtual Error encodeFinished (Boolean_t bEncodeOK);

		protected:
			virtual Error removeEncodedFile (const char *pSourceFileName,
				const char *pProfileName);

		public:
			RequestForNexencoder (void);

			~RequestForNexencoder (void);

			Error init (const char *pName,
				const char *pSourcesPath, const char *pEncodedPath,
				const char *pProfilesPath, const char *pTemporaryPath,
				const char *pFTPUser, const char *pFTPPassword,
				const char *pFTPHost, const char *pFTPRootPath,
				const char *pFilesList,
				const char *pIpAddress, unsigned long ulPort,
				const char *pEncoderPathName,
				const char *pUser, const char *pPassword,
				Tracer_p ptTracer);

			virtual Error finish (void);


	} RequestForNexencoder_t, *RequestForNexencoder_p;

#endif

