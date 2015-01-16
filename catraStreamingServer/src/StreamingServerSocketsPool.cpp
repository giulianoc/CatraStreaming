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


#include "StreamingServerSocketsPool.h"
#include "RTSPConnectionEvent.h"
#include "LiveEvent.h"
#include <assert.h>



StreamingServerSocketsPool:: StreamingServerSocketsPool (void):
	SocketsPool ()

{

}


StreamingServerSocketsPool:: ~StreamingServerSocketsPool (void)

{

}


StreamingServerSocketsPool:: StreamingServerSocketsPool (
	const StreamingServerSocketsPool &t)

{
	assert (1 == 0);

}


Error StreamingServerSocketsPool:: init (
	StreamingServerEventsSet_p pesEventsSet,
	Tracer_p ptTracer)

{

	_pesEventsSet			= pesEventsSet;
	_ptSystemTracer				= ptTracer;

	if (SocketsPool:: init (
		SS_STREAMINGSERVERSOCKETSPOOL_MAXSOCKETSNUMBER,
		0, 0, true) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETSPOOL_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error StreamingServerSocketsPool:: finish (void)

{

	if (SocketsPool:: finish () != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETSPOOL_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error StreamingServerSocketsPool:: updateSocketStatus (
	Socket_p pSocket, long lSocketType,
	void *pvSocketData,
	unsigned short usSocketCheckType)

{

	switch (lSocketType)
	{
		case SS_STREAMINGSERVERSOCKETPOOL_RTSPSESSIONSOCKET:
			{
				Event_p						pevEvent;
				RTSPConnectionEvent_p		pevRTSPConnection;
				unsigned long				ulRTSPSessionIdentifier;


				if (_pesEventsSet -> getFreeEvent (
			StreamingServerEventsSet:: SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER,
					&pevEvent) != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENTSSET_GETFREEEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				pevRTSPConnection			= (RTSPConnectionEvent_p) pevEvent;

				ulRTSPSessionIdentifier		= *((unsigned long *) pvSocketData);

				if (usSocketCheckType & SOCKETSTATUS_EXCEPTION)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERSOCKETSPOOL_EXCEPTIONONSOCKET,
						2, ulRTSPSessionIdentifier, lSocketType);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_pesEventsSet -> releaseEvent (
			StreamingServerEventsSet:: SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER,
						pevRTSPConnection) != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENTSSET_RELEASEEVENT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pevRTSPConnection -> init (
					SS_STREAMINGSERVERSOCKETSPOOL_SOURCE,
					SS_EVENT_RTSPCONNECTIONREADYTOREAD, "SS_EVENT_RTSPCONNECTIONREADYTOREAD",
					ulRTSPSessionIdentifier, _ptSystemTracer, true,
					0, (Buffer_p) NULL) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPCONNECTIONEVENT_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_pesEventsSet -> releaseEvent (
			StreamingServerEventsSet:: SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER,
						pevRTSPConnection) != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENTSSET_RELEASEEVENT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (_pesEventsSet -> addEvent (
					// SS_STREAMINGSERVERPROCESSOR_DESTINATION,
					pevRTSPConnection) != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENTSSET_ADDEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pevRTSPConnection -> finish () != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_RTSPCONNECTIONEVENT_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_pesEventsSet -> releaseEvent (
			StreamingServerEventsSet:: SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER,
						pevRTSPConnection) != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENTSSET_RELEASEEVENT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			break;
		case SS_STREAMINGSERVERSOCKETPOOL_LIVESERVERSOCKET:
			{
				Event_p						pevEvent;
				LiveEvent_p					pevLive;


				if (usSocketCheckType & SOCKETSTATUS_EXCEPTION)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERSOCKETSPOOL_EXCEPTIONONSOCKET,
						2, 0, lSocketType);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (_pesEventsSet -> getFreeEvent (
					StreamingServerEventsSet:: SS_EVENTTYPE_LIVEIDENTIFIER,
					&pevEvent) != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENTSSET_GETFREEEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				pevLive			= (LiveEvent_p) pevEvent;

				if (pevLive -> init (
					SS_STREAMINGSERVERSOCKETSPOOL_SOURCE,
					SS_EVENT_SENDLIVERTPPACKETS, "SS_EVENT_SENDLIVERTPPACKETS",
					pSocket, pvSocketData, _ptSystemTracer) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_LIVEEVENT_INIT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_pesEventsSet -> releaseEvent (
						StreamingServerEventsSet:: SS_EVENTTYPE_LIVEIDENTIFIER,
						pevLive) != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENTSSET_RELEASEEVENT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (_pesEventsSet -> addEvent (
					// SS_STREAMINGSERVERPROCESSOR_DESTINATION,
					pevLive) != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENTSSET_ADDEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pevLive -> finish () != errNoError)
					{
						Error err = StreamingServerErrors (__FILE__, __LINE__,
							SS_LIVEEVENT_FINISH_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_pesEventsSet -> releaseEvent (
						StreamingServerEventsSet:: SS_EVENTTYPE_LIVEIDENTIFIER,
						pevLive) != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENTSSET_RELEASEEVENT_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			break;
		default:
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_WRONGSOCKETTYPE,
					1, lSocketType);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
	}


	return errNoError;
}
