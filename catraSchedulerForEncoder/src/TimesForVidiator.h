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


#ifndef TimesForVidiator_h
	#define TimesForVidiator_h

	#include "Times.h"
	#include "RequestForVidiator.h"
	#include "Tracer.h"
	#include "SchedulerForEncoderErrors.h"


	#define SFE_MAXNAMELENGTH							512 + 1


	typedef class TimesForVidiator: public Times {

		private:
			Tracer_p					_ptTracer;
			char						_pName [SFE_MAXNAMELENGTH];
			RequestForVidiator_t		_rfvRequestForVidiator;


		public:
			TimesForVidiator (void);

			~TimesForVidiator (void);

			Error init (const char *pName, const char *pSchedule,
				const char *pSourcesPath, const char *pEncodedPath,
				const char *pProfilesPath, const char *pTemporaryPath,
				const char *pFTPUser, const char *pFTPPassword,
				const char *pFTPHost, const char *pFTPRootPath,
				const char *pFilesList,
				const char *pIpAddress, unsigned long ulPort,
				Tracer_p ptTracer);

			Error finish (void);

			virtual Error handleTimeOut (void);

	} TimesForVidiator_t, *TimesForVidiator_p;

#endif

