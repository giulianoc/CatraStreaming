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

#include "BeginningHookHttpGetThread.h"
#include "RTSPConnectionEvent.h"
#include "StreamingServerMessages.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>


BeginningHookHttpGetThread:: BeginningHookHttpGetThread (void): HttpGetThread ()

{

}


BeginningHookHttpGetThread:: ~BeginningHookHttpGetThread (void)

{

}


Error BeginningHookHttpGetThread:: init (
	unsigned long ulRTSPIdentifier,
	const char *pWebServerIpAddress, unsigned long ulWebServerPort,
	const char *pLocalIPAddressForHTTP,
	Buffer_p pbURLWithoutParameters, Buffer_p pbURLParameters,
	unsigned long ulTimeoutInSecs,
	StreamingServerEventsSet_p pesEventsSet,
	Tracer_p ptSystemTracer,
	unsigned long ulSequenceNumber)

{

	if (ulSequenceNumber > SS_BEGINNINGHOOKHTTPGETTHREAD_MAXSEQUENCENUMBER)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (HttpGetThread:: init (
		pWebServerIpAddress, ulWebServerPort,
		(const char *) (*pbURLWithoutParameters),
		(const char *) (*pbURLParameters),
		(const char *) NULL,
		(const char *) NULL,
		ulTimeoutInSecs, 0, ulTimeoutInSecs, 0,
		pLocalIPAddressForHTTP) != errNoError)
	{
		Error err = WebToolsErrors (__FILE__, __LINE__,
			WEBTOOLS_HTTPGETTHREAD_INIT_FAILED);
		ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	_ulSequenceNumber			= ulSequenceNumber;
	_ulRTSPIdentifier			= ulRTSPIdentifier;
	_pesEventsSet				= pesEventsSet;
	_ptSystemTracer				= ptSystemTracer;


	return errNoError;
}


Error BeginningHookHttpGetThread:: closingHttpGet (
	Error_p perr)

{

	Buffer_t					bBeginningHookResponse;
	char						pSequenceNumber [
		SS_BEGINNINGHOOKHTTPGETTHREAD_MAXLONGLENGTH];
	long						lEventTypeIdentifier;
	char						pEventTypeIdentifier [
		SS_BEGINNINGHOOKHTTPGETTHREAD_MAXLONGLENGTH];


	sprintf (pSequenceNumber, "%03ld", _ulSequenceNumber);

	if (bBeginningHookResponse. init (pSequenceNumber) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (*perr == errNoError)
	{
		const char			*pBeginOfHttpBody;
		const char			*pEndOfHttpBody;


		// response format:
		//	<HTTP header>
		//	
		//	<[AUTHORIZED | NOT_AUTHORIZED | FAILURE]\n
		//	<message>>\n
		if ((pBeginOfHttpBody = strstr (
			(const char *) _bHttpGetBodyResponse,
			"<AUTHORIZED\n")) != (char *) NULL &&
			(pEndOfHttpBody = strstr (pBeginOfHttpBody, ">\n")) !=
			(char *) NULL)
		{
			pBeginOfHttpBody			+= strlen ("<AUTHORIZED\n");
			

			if (bBeginningHookResponse. append (pBeginOfHttpBody,
				(unsigned long) (pEndOfHttpBody - pBeginOfHttpBody)) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bBeginningHookResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			lEventTypeIdentifier			= SS_EVENT_AUTHORIZATIONGRANTED;
			strcpy (pEventTypeIdentifier, "SS_EVENT_AUTHORIZATIONGRANTED");

			{
				Message msg = StreamingServerMessages (__FILE__, __LINE__,
					SS_BEGINNINGHOOKHTTPGETTHREAD_REQUESTAUTHORIZED,
					2,
					_ulRTSPIdentifier,
					((const char *) bBeginningHookResponse) + 3);
				_ptSystemTracer -> trace (Tracer:: TRACER_LINFO,
					(const char *) msg, __FILE__, __LINE__);
			}
		}
		else if ((pBeginOfHttpBody = strstr (
			(const char *) _bHttpGetBodyResponse,
			"<NOT_AUTHORIZED\n")) != (char *) NULL &&
			(pEndOfHttpBody = strstr (pBeginOfHttpBody, ">\n")) !=
			(char *) NULL)
		{
			pBeginOfHttpBody			+= strlen ("<NOT_AUTHORIZED\n");

			if (bBeginningHookResponse. append (pBeginOfHttpBody,
				(unsigned long) (pEndOfHttpBody - pBeginOfHttpBody)) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bBeginningHookResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			lEventTypeIdentifier			= SS_EVENT_AUTHORIZATIONDENIED;
			strcpy (pEventTypeIdentifier, "SS_EVENT_AUTHORIZATIONDENIED");

			{
				Message msg = StreamingServerMessages (__FILE__, __LINE__,
					SS_BEGINNINGHOOKHTTPGETTHREAD_REQUESTNOTAUTHORIZED,
					2,
					_ulRTSPIdentifier,
					((const char *) bBeginningHookResponse) + 3);
				_ptSystemTracer -> trace (Tracer:: TRACER_LINFO,
					(const char *) msg, __FILE__, __LINE__);
			}
		}
		else if ((pBeginOfHttpBody = strstr (
			(const char *) _bHttpGetBodyResponse,
			"<FAILURE\n")) != (char *) NULL &&
			(pEndOfHttpBody = strstr (pBeginOfHttpBody, ">\n")) !=
			(char *) NULL)
		{
			pBeginOfHttpBody			+= strlen ("<FAILURE\n");

			if (bBeginningHookResponse. append (pBeginOfHttpBody,
				(unsigned long) (pEndOfHttpBody - pBeginOfHttpBody)) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bBeginningHookResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			lEventTypeIdentifier			= SS_EVENT_AUTHORIZATIONERROR;
			strcpy (pEventTypeIdentifier, "SS_EVENT_AUTHORIZATIONERROR");

			{
				Message msg = StreamingServerMessages (__FILE__, __LINE__,
					SS_BEGINNINGHOOKHTTPGETTHREAD_FAILURE,
					2,
					_ulRTSPIdentifier,
					((const char *) bBeginningHookResponse) + 3);
				_ptSystemTracer -> trace (Tracer:: TRACER_LINFO,
					(const char *) msg, __FILE__, __LINE__);
			}
		}
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_BEGINNINGHOOKHTTPGETTHREAD_BEGINNINGHOOKHTTPGETRESPONSEWRONG,
				1, (const char *) _bHttpGetBodyResponse);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bBeginningHookResponse. append ((const char *) err) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bBeginningHookResponse. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			lEventTypeIdentifier			= SS_EVENT_AUTHORIZATIONERROR;
			strcpy (pEventTypeIdentifier, "SS_EVENT_AUTHORIZATIONERROR");
		}
	}
	else
	{
		{
			Message msg = StreamingServerMessages (__FILE__, __LINE__,
				SS_BEGINNINGHOOKHTTPGETTHREAD_HTTPGETERROR,
				2,
				_ulRTSPIdentifier,
				(const char *) (*perr));
			_ptSystemTracer -> trace (Tracer:: TRACER_LINFO,
				(const char *) msg, __FILE__, __LINE__);
		}

		if (bBeginningHookResponse. append ((const char *) (*perr)) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bBeginningHookResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		lEventTypeIdentifier			= SS_EVENT_AUTHORIZATIONERROR;
		strcpy (pEventTypeIdentifier, "SS_EVENT_AUTHORIZATIONERROR");
	}

	{
		Event_p						pevEvent;
		RTSPConnectionEvent_p		pevRTSPConnection;


		if (_pesEventsSet -> getFreeEvent (
			StreamingServerEventsSet:: SS_EVENTTYPE_RTSPCONNECTIONIDENTIFIER,
			&pevEvent) != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENTSSET_GETFREEEVENT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bBeginningHookResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		pevRTSPConnection			= (RTSPConnectionEvent_p) pevEvent;

		if (pevRTSPConnection -> init (
			SS_BEGINNINGHOOKHTTPGETTHREAD_SOURCE,
			lEventTypeIdentifier,
			pEventTypeIdentifier,
			_ulRTSPIdentifier, _ptSystemTracer,
			true, 0, &bBeginningHookResponse) != errNoError)
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

			if (bBeginningHookResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
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

			if (bBeginningHookResponse. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

	}

	if (bBeginningHookResponse. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}

