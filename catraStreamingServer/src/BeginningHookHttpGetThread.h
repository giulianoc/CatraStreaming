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

#ifndef BeginningHookHttpGetThread_h
	#define BeginningHookHttpGetThread_h

	#include "HttpGetThread.h"
	#include "StreamingServerEventsSet.h"
	#include "Tracer.h"


	#define SS_BEGINNINGHOOKHTTPGETTHREAD_SOURCE	"BeginningHookHTTPGETThread"
	#define SS_STREAMINGSERVERPROCESSOR_DESTINATION	"StreamingServerProcessor"

	#define SS_BEGINNINGHOOKHTTPGETTHREAD_MAXSEQUENCENUMBER		200
	#define SS_BEGINNINGHOOKHTTPGETTHREAD_MAXLONGLENGTH			512 + 1


	typedef class BeginningHookHttpGetThread: public HttpGetThread {

		private:
			StreamingServerEventsSet_p	_pesEventsSet;
			Tracer_p					_ptSystemTracer;
			unsigned long				_ulSequenceNumber;
			unsigned long				_ulRTSPIdentifier;


		protected:
			virtual Error closingHttpGet (Error_p perr);

		public:
			BeginningHookHttpGetThread (void);

			~BeginningHookHttpGetThread (void);

			Error init (
				unsigned long ulRTSPIdentifier,
				const char *pWebServerIpAddress, unsigned long ulWebServerPort,
				const char *pLocalIPAddressForHTTP,
				Buffer_p pbURLWithoutParameters, Buffer_p pbURLParameters,
				unsigned long ulTimeoutInSecs,
				StreamingServerEventsSet_p pesEventsSet,
				Tracer_p ptSystemTracer,
				unsigned long ulSequenceNumber);

	} BeginningHookHttpGetThread_t, *BeginningHookHttpGetThread_p;

#endif

