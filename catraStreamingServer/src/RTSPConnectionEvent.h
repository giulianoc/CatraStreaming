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

#ifndef RTSPConnectionEvent_h
	#define RTSPConnectionEvent_h

	#include "Event.h"
	#include "Tracer.h"
	#include "StreamingServerErrors.h"

	#define SS_EVENT_RTSPCONNECTIONREADYTOREAD			1
	#define SS_EVENT_RTSP_RTCPTIMEOUT					2
	#define SS_EVENT_SENDFILERTPPACKETS					6
	#define SS_EVENT_AUTHORIZATIONERROR					9
	#define SS_EVENT_AUTHORIZATIONDENIED				10
	#define SS_EVENT_AUTHORIZATIONGRANTED				11


	typedef class RTSPConnectionEvent: public Event

	{

		private:
			unsigned long		_ulRTSPSessionIdentifier;

		protected:
			Tracer_p			_ptSystemTracer;
			Boolean_t			_bBufferInitialized;
			Buffer_t			_bBuffer;


			RTSPConnectionEvent (const RTSPConnectionEvent &);

			RTSPConnectionEvent &operator = (const RTSPConnectionEvent &);


		public:
			RTSPConnectionEvent (void);

			virtual ~RTSPConnectionEvent (void);

			#ifdef WIN32
				Error init (const char *pSource,
					long lTypeIdentifier,
					const char *pTypeIdentifier,
					unsigned long ulRTSPSessionIdentifier,
					Tracer_p ptTracer, Boolean_t bExpirationNow,
					__int64 ullExpirationLocalDateTimeInMilliSecs,
					Buffer_p pbBuffer);
			#else
				Error init (const char *pSource,
					long lTypeIdentifier,
					const char *pTypeIdentifier,
					unsigned long ulRTSPSessionIdentifier,
					Tracer_p ptTracer, Boolean_t bExpirationNow,
					unsigned long long ullExpirationLocalDateTimeInMilliSecs,
					Buffer_p pbBuffer);
			#endif

			virtual Error finish (void);

			Error getRTSPSessionIdentifier (
				unsigned long *pulRTSPSessionIdentifier);

			Error getBuffer (Buffer_p pbBuffer);

	} RTSPConnectionEvent_t, *RTSPConnectionEvent_p;

#endif
