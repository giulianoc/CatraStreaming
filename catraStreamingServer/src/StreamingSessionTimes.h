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


#ifndef StreamingSessionTimes_h
	#define StreamingSessionTimes_h

	#include "Times.h"
	#include "StreamingServerEventsSet.h"
	#include "Tracer.h"

	#define SS_STREAMINGSESSIONTIMES_CLASSNAME		"StreamingSessionTimes"
	#define SS_STREAMINGSESSIONTIMES_SOURCE			"Scheduler"
	#define SS_STREAMINGSERVERPROCESSOR_DESTINATION	"StreamingServerProcessor"

	#define SS_EVENT_STREAMINGSESSIONTIMEOUT			8


	typedef class StreamingSessionTimes: public Times

	{
		protected:
			unsigned long			_ulRTSPSessionIdentifier;
			StreamingServerEventsSet_p				_pesEventsSet;
			Tracer_p				_ptSystemTracer;

			StreamingSessionTimes (const StreamingSessionTimes &t);

		public:
			StreamingSessionTimes (void);

			virtual ~StreamingSessionTimes (void);

			Error init (
				unsigned long ulPeriod,
				unsigned long ulRTSPSessionIdentifier,
				StreamingServerEventsSet_p pesEventsSet, Tracer_p ptTracer);

			virtual Error finish (void);

			virtual Error handleTimeOut (void);

	} StreamingSessionTimes_t, *StreamingSessionTimes_p;

#endif

