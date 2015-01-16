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


#include "RTSP_RTCPTimes.h"
#include "RTSPConnectionEvent.h"
#include <assert.h>


RTSP_RTCPTimes:: RTSP_RTCPTimes (void): Times ()

{

}


RTSP_RTCPTimes:: ~RTSP_RTCPTimes (void)

{

}


RTSP_RTCPTimes:: RTSP_RTCPTimes (const RTSP_RTCPTimes &t)

{

	assert (1==0);

	// to do

	*this = t;
}


Error RTSP_RTCPTimes:: init (
	unsigned long ulPeriodInMilliSecs, unsigned long ulRTSPSessionIdentifier,
	StreamingServerEventsSet_p pesEventsSet, Tracer_p ptTracer)

{

	_ulRTSPSessionIdentifier	= ulRTSPSessionIdentifier;
	_pesEventsSet				= pesEventsSet;
	_ptSystemTracer				= ptTracer;

	if (Times:: init (ulPeriodInMilliSecs,
		SS_RTSP_RTCPTIMES_CLASSNAME) != errNoError)
	{
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_TIMES_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSP_RTCPTimes:: finish (void)

{

	if (Times:: finish () != errNoError)
	{
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_TIMES_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTSP_RTCPTimes:: handleTimeOut (void)

{

	Event_p							pevEvent;
	RTSPConnectionEvent_p			pevRTSPConnection;


	if (_mtTimesMutex. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return errNoError;
	}

	if (_schTimesStatus != SCHTIMES_STARTED)
	{
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_OPERATION_NOTALLOWED, 1, _schTimesStatus);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtTimesMutex. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}

	if (_pesEventsSet -> getFreeEvent (
		StreamingServerEventsSet:: SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER,
		&pevEvent) != errNoError)
	{
		Error err = EventsSetErrors (__FILE__, __LINE__,
			EVSET_EVENTSSET_GETFREEEVENT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtTimesMutex. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		// no event free found, do not return error to avoid
		//	to be de-actived by the scheduler
		return err;
	}

	pevRTSPConnection			= (RTSPConnectionEvent_p) pevEvent;

	if (pevRTSPConnection -> init (
		SS_RTSP_RTCPTIMES_SOURCE,
		SS_EVENT_RTSP_RTCPTIMEOUT, "SS_EVENT_RTSP_RTCPTIMEOUT",
		_ulRTSPSessionIdentifier, _ptSystemTracer,
		true, 0, (Buffer_p) NULL) != errNoError)
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

		if (_mtTimesMutex. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
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

		if (_mtTimesMutex. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}

	if (_mtTimesMutex. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}

