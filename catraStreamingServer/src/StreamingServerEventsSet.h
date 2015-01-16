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

#ifndef StreamingServerEventsSet_h
	#define StreamingServerEventsSet_h


	#include "StreamingServerErrors.h"
	#include "EventsSet.h"
	#include "Tracer.h"
	#include "vector"

	#define SS_DEFAULTNUMBEROFNEWFREEEVENTSALLOCATED		500


	typedef class StreamingServerEventsSet: public EventsSet

	{
		public:
			enum EventType
			{
				SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER				= 0,
				SS_EVENTTYPE_LIVEIDENTIFIER,

				SS_EVENTTYPENUMBER
			} EventType_t, *EventType_p;

		private:
			Tracer_p			_ptSystemTracer;

		protected:
			StreamingServerEventsSet (const StreamingServerEventsSet &);

			StreamingServerEventsSet &operator = (
				const StreamingServerEventsSet &);

			virtual Error allocateMoreFreeUserEvents (
				unsigned long ulEventTypeIndex,
				unsigned long *_pulPreAllocatedEventsNumber,
				std:: vector<Event_p> *pvFreeEvents,
				std:: vector<Event_p> *pvPointersToAllocatedEvents);

			virtual Error deleteAllocatedEvents (unsigned long ulEventTypeIndex,
				std:: vector<Event_p> *pvPointersToAllocatedEvents);

		public:
			StreamingServerEventsSet (void);

			~StreamingServerEventsSet (void);

			Error init (
				unsigned long ulNumberOfDifferentEventTypesToManage,
				Tracer_p ptTracer);

	} StreamingServerEventsSet_t, *StreamingServerEventsSet_p;

#endif
