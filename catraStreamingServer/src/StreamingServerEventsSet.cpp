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

#include "StreamingServerEventsSet.h"
#include "StreamingServerMessages.h"
#include "RTSPConnectionEvent.h"
#include "LiveEvent.h"
#include <assert.h>



StreamingServerEventsSet:: StreamingServerEventsSet (void):
	EventsSet ()

{

}


StreamingServerEventsSet:: ~StreamingServerEventsSet (void)

{

}


StreamingServerEventsSet:: StreamingServerEventsSet (const StreamingServerEventsSet &)

{

	assert (1==0);
}


StreamingServerEventsSet &StreamingServerEventsSet:: operator = (
	const StreamingServerEventsSet &)

{

	assert (1==0);

	return *this;
}


Error StreamingServerEventsSet:: init (
	unsigned long ulNumberOfDifferentEventTypesToManage,
	Tracer_p ptTracer)

{

	_ptSystemTracer				= ptTracer;

	return EventsSet:: init (true, ulNumberOfDifferentEventTypesToManage);
}


Error StreamingServerEventsSet:: allocateMoreFreeUserEvents (
	unsigned long ulEventTypeIndex,
	unsigned long *_pulPreAllocatedEventsNumber,
	std:: vector<Event_p> *pvFreeEvents,
	std:: vector<Event_p> *pvPointersToAllocatedEvents)

{

	unsigned long			ulFreeEventsNumberAllocated;
	unsigned long			ulEventIndex;


	switch (ulEventTypeIndex)
	{
		case SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER:
			{
				// Important: if you will allocate a type derived
				//	from Event_t (next new), the type of the next variable
				//	must be the derived type and not Event_p
				RTSPConnectionEvent_p			pevRTSPConnectionEvents;


				ulFreeEventsNumberAllocated			=
					SS_DEFAULTNUMBEROFNEWFREEEVENTSALLOCATED;

				if ((pevRTSPConnectionEvents = new RTSPConnectionEvent_t [
					ulFreeEventsNumberAllocated]) ==
					(RTSPConnectionEvent_p) NULL)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_NEW_FAILED);

					return err;
				}

				(*_pulPreAllocatedEventsNumber)			+=
					ulFreeEventsNumberAllocated;

				{
					Message msg = StreamingServerMessages (__FILE__, __LINE__,
						SS_STREAMINGSERVEREVENTSSET_ALLOCATEDMOREEVENTS,
						3, "RTSPConnectionEvent",
						(*_pulPreAllocatedEventsNumber),
						(unsigned long) pevRTSPConnectionEvents);
					_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5,
						(const char *) msg, __FILE__, __LINE__);
				}

				pvFreeEvents -> reserve (*_pulPreAllocatedEventsNumber);

				pvPointersToAllocatedEvents -> insert (
					pvPointersToAllocatedEvents -> end (),
					pevRTSPConnectionEvents);

				for (ulEventIndex = 0;
					ulEventIndex < ulFreeEventsNumberAllocated;
					ulEventIndex++)
				{
					pvFreeEvents -> insert (
						pvFreeEvents -> end (),
						&(pevRTSPConnectionEvents [ulEventIndex]));
				}
			}

			break;
		case SS_EVENTTYPE_LIVEIDENTIFIER:
			{
				// Important: if you will allocate a type derived
				//	from Event_t (next new), the type of the next variable
				//	must be the derived type and not Event_p
				LiveEvent_p				pevLiveEvents;


				ulFreeEventsNumberAllocated			=
					SS_DEFAULTNUMBEROFNEWFREEEVENTSALLOCATED;

				if ((pevLiveEvents = new LiveEvent_t [
					ulFreeEventsNumberAllocated]) ==
					(LiveEvent_p) NULL)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_NEW_FAILED);

					return err;
				}

				(*_pulPreAllocatedEventsNumber)			+=
					ulFreeEventsNumberAllocated;

				{
					Message msg = StreamingServerMessages (__FILE__, __LINE__,
						SS_STREAMINGSERVEREVENTSSET_ALLOCATEDMOREEVENTS,
						3, "LiveEvent",
						(*_pulPreAllocatedEventsNumber),
						(unsigned long) pevLiveEvents);
					_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5,
						(const char *) msg, __FILE__, __LINE__);
				}

				pvFreeEvents -> reserve (*_pulPreAllocatedEventsNumber);

				pvPointersToAllocatedEvents -> insert (
					pvPointersToAllocatedEvents -> end (),
					pevLiveEvents);

				for (ulEventIndex = 0;
					ulEventIndex < ulFreeEventsNumberAllocated;
					ulEventIndex++)
				{
					pvFreeEvents -> insert (
						pvFreeEvents -> end (),
						&(pevLiveEvents [ulEventIndex]));
				}
			}

			break;
		default:
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_UNKNOWNEVENTTYPE);

			return err;
	}


	return errNoError;
}


Error StreamingServerEventsSet:: deleteAllocatedEvents (
	unsigned long ulEventTypeIndex,
	std:: vector<Event_p> *pvPointersToAllocatedEvents)

{

	std:: vector<Event_p>:: const_iterator	itPreAllocatedEvents;


	switch (ulEventTypeIndex)
	{
		case SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER:
			{
				// Important: if you will allocate a type derived
				//	from Event_t (next new), the type of the next variable
				//	must be the derived type and not Event_p
				RTSPConnectionEvent_p			pevRTSPConnectionEvents;


				for (itPreAllocatedEvents =
					pvPointersToAllocatedEvents -> begin ();
					itPreAllocatedEvents !=
					pvPointersToAllocatedEvents -> end ();
					++itPreAllocatedEvents)
				{
					pevRTSPConnectionEvents			=
						(RTSPConnectionEvent_p) (*itPreAllocatedEvents);

					delete [] pevRTSPConnectionEvents;
				}
			}

			break;
		case SS_EVENTTYPE_LIVEIDENTIFIER:
			{
				// Important: if you will allocate a type derived
				//	from Event_t (next new), the type of the next variable
				//	must be the derived type and not Event_p
				LiveEvent_p				pevLiveEvents;


				for (itPreAllocatedEvents =
					pvPointersToAllocatedEvents -> begin ();
					itPreAllocatedEvents !=
					pvPointersToAllocatedEvents -> end ();
					++itPreAllocatedEvents)
				{
					pevLiveEvents			=
						(LiveEvent_p) (*itPreAllocatedEvents);

					delete [] pevLiveEvents;
				}
			}

			break;
		default:
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_UNKNOWNEVENTTYPE);

			return err;
	}

	
	return errNoError;
}

