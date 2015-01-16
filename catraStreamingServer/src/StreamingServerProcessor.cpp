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


#include "StreamingServerProcessor.h"
#include "CheckServerSocketTimes.h"
#include "RTSP_RTCPTimes.h"
#include "DateTime.h"


StreamingServerProcessor:: StreamingServerProcessor (void): PosixThread ()

{

}


StreamingServerProcessor:: ~StreamingServerProcessor (void)

{

}


Error StreamingServerProcessor:: init (
	unsigned long ulProcessorIdentifier,
	MP4Atom:: Standard_t sStandard,
	StreamingServerEventsSet_p pesEventsSet,
	ServerSocket_p pssServerSocket,
	SocketsPool_p pspSocketsPool,
	RTSPSession_p prsRTSPSessions,
	unsigned long ulRTSPSessionsNumber,
	PMutex_p pmtFreeRTSPSessions,
	std:: vector<RTSPSession_p>	*pvFreeRTSPSessions,
	const char *pLocalIPAddressForRTP,
	PMutex_p pmtStreamingServerStatistics,
	PMutex_p pmtLiveSources,
	#ifdef WIN32
		__int64 *pullTotalBytesServed,
		__int64 *pullTotalLostPacketsNumber,
		__int64 *pullTotalSentPacketsNumber,
	#else
		unsigned long long *pullTotalBytesServed,
		unsigned long long *pullTotalLostPacketsNumber,
		unsigned long long *pullTotalSentPacketsNumber,
	#endif
	ConfigurationFile_p pcfConfiguration,
	Tracer_p ptTracer,
	Tracer_p ptSubscriberTracer)

{

	_ulProcessorIdentifier			= ulProcessorIdentifier;
	_sStandard						= sStandard;
	_pesEventsSet					= pesEventsSet;
	_pssServerSocket				= pssServerSocket;
	_pspSocketsPool					= pspSocketsPool;
	_pmtLiveSources					= pmtLiveSources;
	_prsRTSPSessions				= prsRTSPSessions;
	_ulRTSPSessionsNumber			= ulRTSPSessionsNumber;
	_pmtFreeRTSPSessions			= pmtFreeRTSPSessions;
	_pvFreeRTSPSessions				= pvFreeRTSPSessions;
	strcpy (_pLocalIPAddressForRTP, pLocalIPAddressForRTP);
	_pmtStreamingServerStatistics	= pmtStreamingServerStatistics;
	_pmtLiveSources					= pmtLiveSources;
	_pullTotalBytesServed			= pullTotalBytesServed;
	_pullTotalLostPacketsNumber		= pullTotalLostPacketsNumber;
	_pullTotalSentPacketsNumber		= pullTotalSentPacketsNumber;
	_pcfConfiguration				= pcfConfiguration;
	_ptSystemTracer					= ptTracer;
	_ptSubscriberTracer				= ptSubscriberTracer;


	#if defined(__CYGWIN__)
		if (_mtShutdown. init (PMutex:: MUTEX_RECURSIVE) !=
			errNoError)
	#else							// POSIX.1-1996 standard (HPUX 11)
		if (_mtShutdown. init (PMutex:: MUTEX_RECURSIVE) !=
			errNoError)
	#endif
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_bAuthorizationBuffer. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtShutdown. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (_bDestination. init (SS_STREAMINGSERVERPROCESSOR_DESTINATION) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bAuthorizationBuffer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtShutdown. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (PosixThread:: init () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bDestination. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_bAuthorizationBuffer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtShutdown. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}


	return errNoError;
}


Error StreamingServerProcessor:: finish ()

{

	if (PosixThread:: finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bDestination. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_bAuthorizationBuffer. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}

	if (_mtShutdown. finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}


	return errNoError;
}


Error StreamingServerProcessor:: run (void)

{

	Boolean_t			bIsShutdown;
	Error_t				errGetAndRemoveEvent;
	Error_t				errProcessEvent;
	Event_p				pevEvent;
	unsigned long		ulEventsCounter;


	bIsShutdown					= false;
	if (setIsShutdown (bIsShutdown) != errNoError)
	{
		_erThreadReturn = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_SETISSHUTDOWN_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) _erThreadReturn, __FILE__, __LINE__);

		return _erThreadReturn;
	}

	ulEventsCounter			= 1;

	while (!bIsShutdown)
	{
		if ((errGetAndRemoveEvent = _pesEventsSet -> getAndRemoveFirstEvent (
			&_bDestination, &pevEvent, true,
			SS_STREAMINGSERVERPROCESSOR_SLEEPTIMESECSWAITINGEVENT,
			SS_STREAMINGSERVERPROCESSOR_SLEEPTIMEMILLISECSWAITINGEVENT)) !=
			errNoError)
		{
			if ((long) errGetAndRemoveEvent ==
				EVSET_EVENTSSET_DESTINATIONNOTFOUND ||
				(long) errGetAndRemoveEvent == EVSET_EVENTSSET_NOEVENTSFOUND ||
				(long) errGetAndRemoveEvent ==
				EVSET_EVENTSSET_EVENTNOTEXPIREDYET)
			{
				if (PosixThread:: getSleep (0, 500000) != errNoError)
				{
					_erThreadReturn = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PTHREAD_GETSLEEP_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) _erThreadReturn, __FILE__, __LINE__);

					return _erThreadReturn;
				}

				if (getIsShutdown (&bIsShutdown) != errNoError)
				{
					_erThreadReturn = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_GETISSHUTDOWN_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) _erThreadReturn, __FILE__, __LINE__);

					return _erThreadReturn;
				}

				continue;
			}
			else
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errGetAndRemoveEvent, __FILE__, __LINE__);

				_erThreadReturn = EventsSetErrors (__FILE__, __LINE__,
					EVSET_EVENTSSET_GETANDREMOVEFIRSTEVENT_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				return _erThreadReturn;
			}
		}

		if ((errProcessEvent = processEvent (pevEvent)) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERPROCESSOR_PROCESSEVENT_FAILED,
				1, _ulProcessorIdentifier);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			// delete pevEvent;

			// return err;
		}

		if (!(ulEventsCounter % 30))
		{
			if (getIsShutdown (&bIsShutdown) != errNoError)
			{
				_erThreadReturn = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERPROCESSOR_GETISSHUTDOWN_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) _erThreadReturn, __FILE__, __LINE__);

				return _erThreadReturn;
			}

			if (bIsShutdown)
				break;

			ulEventsCounter			= 1;
		}

		ulEventsCounter++;
	}


	return _erThreadReturn;
}


Error StreamingServerProcessor:: cancel (void)

{

	time_t							tUTCNow;
	PosixThread:: PThreadStatus_t	stRTPThreadState;


	if (setIsShutdown (true) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_SETISSHUTDOWN_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getThreadState (&stRTPThreadState) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_GETTHREADSTATE_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	tUTCNow					= time (NULL);

	while (stRTPThreadState == THREADLIB_STARTED ||
		stRTPThreadState == THREADLIB_STARTED_AND_JOINED)
	{
		if (time (NULL) - tUTCNow >= 5)
			break;

		if (PosixThread:: getSleep (1, 0) != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_GETSLEEP_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (getThreadState (&stRTPThreadState) != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_GETTHREADSTATE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (stRTPThreadState == THREADLIB_STARTED ||
		stRTPThreadState == THREADLIB_STARTED_AND_JOINED)
	{
		if (PosixThread:: cancel () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_CANCEL_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error StreamingServerProcessor:: processEvent (Event_p pevEvent)

{

	long				lTypeIdentifier;
	Error_t				errTypeIdentifier;


	if ((errTypeIdentifier = pevEvent -> getTypeIdentifier (
		&lTypeIdentifier)) != errNoError)
	{
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errTypeIdentifier, __FILE__, __LINE__);

		Error err = EventsSetErrors (__FILE__, __LINE__,
			EVSET_EVENT_GETTYPEIDENTIFIER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pevEvent -> finish () != errNoError)
		{
			Error err = EventsSetErrors (__FILE__, __LINE__,
				EVSET_EVENT_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		/*
		???????????
		if (_pesEventsSet -> releaseRTSPConnectionEvent (
			pevRTSPConnection) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVEREVENTSSET_RELEASERTSPCONNECTIONEVENT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}
		*/

		return err;
	}

	switch (lTypeIdentifier)
	{
		case SS_EVENT_CHECKNEWRTSPCONNECTION:	// 0
			{
				RTSPConnectionEvent_p	pevRTSPConnection;


				pevRTSPConnection	= (RTSPConnectionEvent_p) pevEvent;

				if (handleNewRTSPConnectionEvent (pevRTSPConnection) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_HANDLENEWRTSPCONNECTIONEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pevRTSPConnection -> finish () != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENT_FINISH_FAILED);
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

				if (pevRTSPConnection -> finish () != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENT_FINISH_FAILED);
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
			}

			break;
		case SS_EVENT_RTSPCONNECTIONREADYTOREAD:	// 1
			{
				RTSPConnectionEvent_p	pevRTSPConnection;


				pevRTSPConnection	= (RTSPConnectionEvent_p) pevEvent;

				if (handleRTSPConnectionReadyToReadEvent (pevRTSPConnection) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_HANDLERTSPCONNECTIONREADYTOREADEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pevRTSPConnection -> finish () != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENT_FINISH_FAILED);
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

				if (pevRTSPConnection -> finish () != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENT_FINISH_FAILED);
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
			}

			break;
		case SS_EVENT_RTSP_RTCPTIMEOUT:	// 2
			{
				RTSPConnectionEvent_p	pevRTSPConnection;


				pevRTSPConnection	= (RTSPConnectionEvent_p) pevEvent;

				if (handleRTSP_RTCPTimeoutEvent (pevRTSPConnection) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_HANDLERTSP_RTCPTIMEOUTEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pevRTSPConnection -> finish () != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENT_FINISH_FAILED);
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

				if (pevRTSPConnection -> finish () != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENT_FINISH_FAILED);
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
			}

			break;
		case SS_EVENT_PAUSETIMEOUT:	// 3
			{
				RTSPConnectionEvent_p	pevRTSPConnection;


				pevRTSPConnection	= (RTSPConnectionEvent_p) pevEvent;

				if (handlePauseTimeoutEvent (pevRTSPConnection) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_HANDLEPAUSETIMEOUTEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pevRTSPConnection -> finish () != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENT_FINISH_FAILED);
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

				if (pevRTSPConnection -> finish () != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENT_FINISH_FAILED);
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
			}

			break;
		case SS_EVENT_RECEIVERTCPPACKETS:	// 4
			{
				RTSPConnectionEvent_p	pevRTSPConnection;


				pevRTSPConnection	= (RTSPConnectionEvent_p) pevEvent;

				if (handleReceiveRTCPPacketsEvent (pevRTSPConnection) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_HANDLERECEIVERTCPPACKETSEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pevRTSPConnection -> finish () != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENT_FINISH_FAILED);
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

				if (pevRTSPConnection -> finish () != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENT_FINISH_FAILED);
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
			}

			break;
		case SS_EVENT_SENDRTCPPACKETS:	// 5
			{
				RTSPConnectionEvent_p	pevRTSPConnection;


				pevRTSPConnection	= (RTSPConnectionEvent_p) pevEvent;

				if (handleSendRTCPPacketsEvent (pevRTSPConnection) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_HANDLESENDRTCPPACKETSEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pevRTSPConnection -> finish () != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENT_FINISH_FAILED);
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

				if (pevRTSPConnection -> finish () != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENT_FINISH_FAILED);
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
			}

			break;
		case SS_EVENT_SENDFILERTPPACKETS:		// 6
			{
				RTSPConnectionEvent_p	pevRTSPConnection;


				pevRTSPConnection	= (RTSPConnectionEvent_p) pevEvent;

				if (handleSendFileRTPPacketsEvent (pevRTSPConnection) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_HANDLESENDFILERTPPACKETSEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pevRTSPConnection -> finish () != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENT_FINISH_FAILED);
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

				if (pevRTSPConnection -> finish () != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENT_FINISH_FAILED);
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
			}

			break;
		case SS_EVENT_SENDLIVERTPPACKETS:		// 7
			{
				LiveEvent_p		pevLive;


				pevLive	= (LiveEvent_p) pevEvent;

				if (handleSendLiveRTPPacketsEvent (pevLive) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_HANDLESENDLIVERTPPACKETSEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pevLive -> finish () != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENT_FINISH_FAILED);
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

				if (pevLive -> finish () != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENT_FINISH_FAILED);
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
			}

			break;
		case SS_EVENT_STREAMINGSESSIONTIMEOUT:	// 8
			{
				RTSPConnectionEvent_p	pevRTSPConnection;


				pevRTSPConnection	= (RTSPConnectionEvent_p) pevEvent;

				if (handleStreamingSessionTimeoutEvent (pevRTSPConnection) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_HANDLESTREAMINGSESSIONTIMEOUTEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pevRTSPConnection -> finish () != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENT_FINISH_FAILED);
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

				if (pevRTSPConnection -> finish () != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENT_FINISH_FAILED);
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
			}

			break;
		case SS_EVENT_AUTHORIZATIONERROR:	// 9
			{
				RTSPConnectionEvent_p	pevRTSPConnection;


				pevRTSPConnection	= (RTSPConnectionEvent_p) pevEvent;

				if (handleAuthorizationErrorEvent (pevRTSPConnection) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_HANDLEAUTHORIZATIONERROREVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pevRTSPConnection -> finish () != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENT_FINISH_FAILED);
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

				if (pevRTSPConnection -> finish () != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENT_FINISH_FAILED);
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
			}

			break;
		case SS_EVENT_AUTHORIZATIONDENIED:	// 10
			{
				RTSPConnectionEvent_p	pevRTSPConnection;


				pevRTSPConnection	= (RTSPConnectionEvent_p) pevEvent;

				if (handleAuthorizationDeniedEvent (pevRTSPConnection) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_HANDLEAUTHORIZATIONDENIEDEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pevRTSPConnection -> finish () != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENT_FINISH_FAILED);
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

				if (pevRTSPConnection -> finish () != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENT_FINISH_FAILED);
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
			}

			break;
		case SS_EVENT_AUTHORIZATIONGRANTED:	// 11
			{
				RTSPConnectionEvent_p	pevRTSPConnection;


				pevRTSPConnection	= (RTSPConnectionEvent_p) pevEvent;

				if (handleAuthorizationGrantedEvent (pevRTSPConnection) !=
					errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_HANDLEAUTHORIZATIONGRANTEDEVENT_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pevRTSPConnection -> finish () != errNoError)
					{
						Error err = EventsSetErrors (__FILE__, __LINE__,
							EVSET_EVENT_FINISH_FAILED);
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

				if (pevRTSPConnection -> finish () != errNoError)
				{
					Error err = EventsSetErrors (__FILE__, __LINE__,
						EVSET_EVENT_FINISH_FAILED);
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
			}

			break;
		default:
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERPROCESSOR_EVENTUNKNOWN,
					1, lTypeIdentifier);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
	}


	return errNoError;
}


Error StreamingServerProcessor:: handleNewRTSPConnectionEvent (
	RTSPConnectionEvent_p pevRTSPConnection)

{

	RTSPSession_p			prsRTSPSession;
	Error					errAcceptConnection;
	Boolean_t				bFoundNewConnection;


	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_STARTEVENTHANDLER,
			2, (unsigned long) 9999, "SS_EVENT_CHECKNEWRTSPCONNECTION");
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG3, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (_pmtFreeRTSPSessions -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	bFoundNewConnection				= true;

	while (bFoundNewConnection)
	{
		if (_pvFreeRTSPSessions -> size () == 0)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERPROCESSOR_NOTFOUNDRTSPSESSIONFREE);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			bFoundNewConnection				= false;

			continue;
		}

		prsRTSPSession			= *(_pvFreeRTSPSessions -> begin ());

		if ((errAcceptConnection = prsRTSPSession -> acceptRTSPConnection (
			_pssServerSocket)) != errNoError)
		{
			if ((long) errAcceptConnection ==
				SS_RTSPSESSION_NORTSPCONNECTIONTOACCEPT_FAILED)
			{
				bFoundNewConnection				= false;

				continue;
			}
			else
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errAcceptConnection, __FILE__, __LINE__);

				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_ACCEPTRTSPCONNECTION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_pmtFreeRTSPSessions -> unLock () != errNoError)
				{
					_erThreadReturn = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) _erThreadReturn, __FILE__, __LINE__);
				}

				return err;
			}
		}

		_pvFreeRTSPSessions -> erase (_pvFreeRTSPSessions -> begin ());
	}

	if (_pmtFreeRTSPSessions -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error StreamingServerProcessor:: handleRTSPConnectionReadyToReadEvent (
	RTSPConnectionEvent_p pevRTSPConnection)

{

	unsigned long			ulRTSPSessionIdentifier;
	#ifdef WIN32
		__int64					ullLocalTotalBytesServed;
		__int64					ullLocalTotalLostPacketsNumber;
		__int64					ullLocalTotalSentPacketsNumber;
	#else
		unsigned long long		ullLocalTotalBytesServed;
		unsigned long long		ullLocalTotalLostPacketsNumber;
		unsigned long long		ullLocalTotalSentPacketsNumber;
	#endif
	Error_t					errProcessRequest;
	Boolean_t				bIsRTSPConnectionFinished;


	if (pevRTSPConnection -> getRTSPSessionIdentifier (
		&ulRTSPSessionIdentifier) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_GETRTSPSESSIONIDENTIFIER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_STARTEVENTHANDLER,
			2, ulRTSPSessionIdentifier, "SS_EVENT_RTSPCONNECTIONREADYTOREAD");
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if ((errProcessRequest =
		(_prsRTSPSessions [ulRTSPSessionIdentifier]). processRTSPRequest (
		&ullLocalTotalBytesServed,
		&ullLocalTotalLostPacketsNumber,
		&ullLocalTotalSentPacketsNumber,
		&bIsRTSPConnectionFinished)) != errNoError)
	{
		if ((long) errProcessRequest == SS_RTSPSESSION_WRONGSTATE)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_PROCESSRTSPREQUEST_FAILED);
			// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			//	(const char *) err, __FILE__, __LINE__);

			// return err;	it is used errNoError to avoid the trace of the error
			return errNoError;
		}
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_PROCESSRTSPREQUEST_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			// return err;
		}
	}

	if (bIsRTSPConnectionFinished)
	{
		if (_pmtFreeRTSPSessions -> lock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_LOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		_pvFreeRTSPSessions -> insert (_pvFreeRTSPSessions -> end (),
			&(_prsRTSPSessions [ulRTSPSessionIdentifier]));

		if (_pmtFreeRTSPSessions -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pmtStreamingServerStatistics -> lock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_LOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		(*_pullTotalBytesServed)			+= ullLocalTotalBytesServed;
		(*_pullTotalLostPacketsNumber)		+= ullLocalTotalLostPacketsNumber;
		(*_pullTotalSentPacketsNumber)		+= ullLocalTotalSentPacketsNumber;

		if (_pmtStreamingServerStatistics -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error StreamingServerProcessor:: handleRTSP_RTCPTimeoutEvent (
	RTSPConnectionEvent_p pevRTSPConnection)

{

	unsigned long			ulRTSPSessionIdentifier;


	if (pevRTSPConnection -> getRTSPSessionIdentifier (
		&ulRTSPSessionIdentifier) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_GETRTSPSESSIONIDENTIFIER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_STARTEVENTHANDLER,
			2, ulRTSPSessionIdentifier, "SS_EVENT_RTSP_RTCPTIMEOUT");
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	{
		Error errTimeout = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_RTSP_RTCPTIMEOUT,
			1, ulRTSPSessionIdentifier);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errTimeout, __FILE__, __LINE__);

		if (closeConnection (ulRTSPSessionIdentifier,
			&errTimeout) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERPROCESSOR_CLOSECONNECTION_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error StreamingServerProcessor:: handlePauseTimeoutEvent (
	RTSPConnectionEvent_p pevRTSPConnection)

{

	unsigned long			ulRTSPSessionIdentifier;


	if (pevRTSPConnection -> getRTSPSessionIdentifier (
		&ulRTSPSessionIdentifier) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_GETRTSPSESSIONIDENTIFIER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_STARTEVENTHANDLER,
			2, ulRTSPSessionIdentifier, "SS_EVENT_PAUSETIMEOUT");
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	{
		Error errTimeout = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_PAUSETIMEOUT,
			1, ulRTSPSessionIdentifier);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errTimeout, __FILE__, __LINE__);

		if (closeConnection (ulRTSPSessionIdentifier,
			&errTimeout) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERPROCESSOR_CLOSECONNECTION_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error StreamingServerProcessor:: handleStreamingSessionTimeoutEvent (
	RTSPConnectionEvent_p pevRTSPConnection)

{

	unsigned long			ulRTSPSessionIdentifier;


	if (pevRTSPConnection -> getRTSPSessionIdentifier (
		&ulRTSPSessionIdentifier) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_GETRTSPSESSIONIDENTIFIER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_STARTEVENTHANDLER,
			2, ulRTSPSessionIdentifier, "SS_EVENT_STREAMINGSESSIONTIMEOUT");
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	{
		Error errTimeout = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_STREAMINGSESSIONTIMEOUT,
			1, ulRTSPSessionIdentifier);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errTimeout, __FILE__, __LINE__);

		if (closeConnection (ulRTSPSessionIdentifier,
			&errTimeout) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERPROCESSOR_CLOSECONNECTION_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error StreamingServerProcessor:: handleReceiveRTCPPacketsEvent (
	RTSPConnectionEvent_p pevRTSPConnection)

{

	unsigned long			ulRTSPSessionIdentifier;
	unsigned long			ulReceivedRTCPPacketsNumber;
	Error_t					errHandleReceiver;


	if (pevRTSPConnection -> getRTSPSessionIdentifier (
		&ulRTSPSessionIdentifier) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_GETRTSPSESSIONIDENTIFIER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_STARTEVENTHANDLER,
			2, ulRTSPSessionIdentifier, "SS_EVENT_RECEIVERTCPPACKETS");
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if ((errHandleReceiver = (_prsRTSPSessions [ulRTSPSessionIdentifier]).
		handleReceiverRTCPPackets (
		&ulReceivedRTCPPacketsNumber)) != errNoError)
	{
		if ((long) errHandleReceiver == SS_RTSPSESSION_WRONGSTATE)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_HANDLERECEIVERRTCPPACKETS_FAILED);
			// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			//	(const char *) err, __FILE__, __LINE__);

			// return err; it is used errNoError to avoid the trace of the error
			return errNoError;
		}
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_HANDLERECEIVERRTCPPACKETS_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error StreamingServerProcessor:: handleSendRTCPPacketsEvent (
	RTSPConnectionEvent_p pevRTSPConnection)

{

	unsigned long			ulRTSPSessionIdentifier;
	Error_t					errHandleSender;


	if (pevRTSPConnection -> getRTSPSessionIdentifier (
		&ulRTSPSessionIdentifier) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_GETRTSPSESSIONIDENTIFIER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_STARTEVENTHANDLER,
			2, ulRTSPSessionIdentifier, "SS_EVENT_SENDRTCPPACKETS");
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if ((errHandleSender = (_prsRTSPSessions [ulRTSPSessionIdentifier]).
		handleSenderRTCPPackets ()) != errNoError)
	{
		if ((long) errHandleSender == SS_RTSPSESSION_WRONGSTATE)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_HANDLESENDERRTCPPACKETS_FAILED);
			// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			//	(const char *) err, __FILE__, __LINE__);

			// return err; it is used errNoError to avoid the trace of the error
			return errNoError;
		}
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_HANDLESENDERRTCPPACKETS_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	/*
	if ((errSendOption = (_prsRTSPSessions [ulRTSPSessionIdentifier]).
		sendRTSPOPTIONCommand ()) != errNoError)
	{
		if ((long) errSendOption == SS_RTSPSESSION_WRONGSTATE)
		{
			// Error err = StreamingServerErrors (__FILE__, __LINE__,
			//	SS_RTSPSESSION_SENDRTSPOPTIONCOMMAND_FAILED);
			// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			//	(const char *) err, __FILE__, __LINE__);

			// return err; it is used errNoError to avoid the trace of the error
			return errNoError;
		}
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTSPOPTIONCOMMAND_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	*/


	return errNoError;
}


Error StreamingServerProcessor:: handleSendFileRTPPacketsEvent (
	RTSPConnectionEvent_p pevRTSPConnection)

{

	unsigned long			ulRTSPSessionIdentifier;
	Error_t					errSendRTPPackets;
	unsigned long			ulVideoRTPPacketsNumberSent;
	unsigned long			ulAudioRTPPacketsNumberSent;


	if (pevRTSPConnection -> getRTSPSessionIdentifier (
		&ulRTSPSessionIdentifier) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_GETRTSPSESSIONIDENTIFIER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_STARTEVENTHANDLER,
			2, ulRTSPSessionIdentifier, "SS_EVENT_SENDFILERTPPACKETS");
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if ((errSendRTPPackets = (_prsRTSPSessions [
		ulRTSPSessionIdentifier]).
		sendRTPPackets (&ulVideoRTPPacketsNumberSent,
		&ulAudioRTPPacketsNumberSent)) !=
		errNoError)
	{
		if ((long) errSendRTPPackets == SS_RTSPSESSION_WRONGSTATE)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTPPACKETS_FAILED);
			// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			//	(const char *) err, __FILE__, __LINE__);

			// return err; it is used errNoError to avoid the trace of the error
			return errNoError;
		}
		else if ((long) errSendRTPPackets == SS_RTSPSESSION_NOMORERTPPACKETS)
			;
		else if ((long) errSendRTPPackets == SS_RTSPSESSION_REQUESTEDSTOPTIMEREACHED)
			;
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_SENDRTPPACKETS_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// if (ulVideoRTPPacketsNumberSent || ulAudioRTPPacketsNumberSent)
	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_SENTRTPPACKETSNUMBER,
			3, ulRTSPSessionIdentifier,
			ulVideoRTPPacketsNumberSent, ulAudioRTPPacketsNumberSent);
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}


	return errNoError;
}


Error StreamingServerProcessor:: handleSendLiveRTPPacketsEvent (
	LiveEvent_p pevLive)

{

	void				*pvLiveSource;
	LiveSource_p		plsLiveSource;
	ServerSocket_p		pssLiveServerSocket;
	RTPPacket_p			prpLiveRTPPacket;
	unsigned short		usRTPPacketSequenceNumberFromLiveEncoder;
	unsigned long		ulRTPPacketTimeStampFromLiveEncoder;
	unsigned long		ulSequenceNumberRandomOffset;
	unsigned long		ulTimeStampRandomOffset;

	Boolean_t			bRTPPacketFound;
	std:: vector<unsigned long>		*pvActiveLiveRTSPSessions;
	std:: vector<unsigned long>:: iterator		itActiveRTSPSession;
	Boolean_t			bIsVideoSocket;
	unsigned long		ulRTSPSessionIdentifier;
	Error_t				errReadRTPPacket;
	Error_t				errSendLiveRTPPacket;


	if (_pmtLiveSources -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pevLive -> getLiveEventInfo (
		&pvLiveSource,
		&pssLiveServerSocket) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_LIVEEVENT_GETLIVEEVENTINFO_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_pmtLiveSources -> unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	plsLiveSource				= (LiveSource_p) pvLiveSource;

	bRTPPacketFound				= true;

	while (bRTPPacketFound)
	{
		if ((errReadRTPPacket = plsLiveSource -> readRTPPacket (
			pssLiveServerSocket,
			&prpLiveRTPPacket, &pvActiveLiveRTSPSessions, &bIsVideoSocket)) !=
			errNoError)
		{
			if ((long) errReadRTPPacket == SS_ACTIVATION_WRONG)
			{
				// Error err = StreamingServerErrors (__FILE__, __LINE__,
				//	SS_LIVESOURCE_READRTPPACKET_FAILED);
				// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				//	(const char *) err, __FILE__, __LINE__);

				if (_pmtLiveSources -> unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				// return err; it is used errNoError to avoid the trace of the error
				return errNoError;
			}
			else if ((long) errReadRTPPacket == SS_LIVESOURCE_NOTHINGTOREAD)
			{
				bRTPPacketFound				= false;

				continue;
			}
			else
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_LIVESOURCE_READRTPPACKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_pmtLiveSources -> unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (prpLiveRTPPacket -> getSequenceNumber (
			&usRTPPacketSequenceNumberFromLiveEncoder,
			&ulSequenceNumberRandomOffset) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_GETSEQUENCENUMBER_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pmtLiveSources -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		usRTPPacketSequenceNumberFromLiveEncoder			+=
			((unsigned short) ulSequenceNumberRandomOffset);

		if (prpLiveRTPPacket -> getTimeStamp (
			&ulRTPPacketTimeStampFromLiveEncoder,
			&ulTimeStampRandomOffset) !=
			errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_GETTIMESTAMP_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_pmtLiveSources -> unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		ulRTPPacketTimeStampFromLiveEncoder			+=
			ulTimeStampRandomOffset;

		bRTPPacketFound			= true;

		for (itActiveRTSPSession =
			pvActiveLiveRTSPSessions -> begin ();
			itActiveRTSPSession !=
			pvActiveLiveRTSPSessions -> end ();
			++itActiveRTSPSession)
		{
			ulRTSPSessionIdentifier		= *itActiveRTSPSession;

			if ((errSendLiveRTPPacket =
				(_prsRTSPSessions [ulRTSPSessionIdentifier]).
				sendLiveRTPPacket (bIsVideoSocket,
				usRTPPacketSequenceNumberFromLiveEncoder,
				ulRTPPacketTimeStampFromLiveEncoder,
				prpLiveRTPPacket)) != errNoError)
			{
				if ((long) errSendLiveRTPPacket != SS_RTSPSESSION_WRONGSTATE)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_RTSPSESSION_SENDLIVERTPPACKET_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_pmtLiveSources -> unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
		}
	}

	if (_pmtLiveSources -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error StreamingServerProcessor:: handleAuthorizationErrorEvent (
	RTSPConnectionEvent_p pevRTSPConnection)

{

	unsigned long			ulRTSPSessionIdentifier;
	Error_t					errHandleDESCRIBEResponse;
	char					pSequenceNumber [SS_MAXLONGLENGTH];
	unsigned long			ulSequenceNumber;


	if (pevRTSPConnection -> getRTSPSessionIdentifier (
		&ulRTSPSessionIdentifier) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_GETRTSPSESSIONIDENTIFIER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_STARTEVENTHANDLER,
			2, ulRTSPSessionIdentifier, "SS_EVENT_AUTHORIZATIONERROR");
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (pevRTSPConnection -> getBuffer (&_bAuthorizationBuffer) !=
		errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_GETRTSPSESSIONIDENTIFIER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		{
			Error errAuthorizationError = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERPROCESSOR_AUTHORIZATIONERROR,
				2, ulRTSPSessionIdentifier,
				((const char *) _bAuthorizationBuffer) + 3);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errAuthorizationError, __FILE__, __LINE__);

			if (closeConnection (ulRTSPSessionIdentifier,
				&errAuthorizationError) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERPROCESSOR_CLOSECONNECTION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		return err;
	}

	strncpy (pSequenceNumber, (const char *) _bAuthorizationBuffer, 3);
	pSequenceNumber [3]			= '\0';

	ulSequenceNumber			= atol (pSequenceNumber);

	if ((errHandleDESCRIBEResponse = (_prsRTSPSessions [
		ulRTSPSessionIdentifier]). handleDESCRIBEResponse (
		ulSequenceNumber, SS_EVENT_AUTHORIZATIONERROR,
		((const char *) _bAuthorizationBuffer) + 3)) != errNoError)
	{
		if ((long) errHandleDESCRIBEResponse == SS_RTSPSESSION_WRONGSTATE)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_HANDLEDESCRIBERESPONSE_FAILED);
			// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			//	(const char *) err, __FILE__, __LINE__);

			{
				Error errAuthorizationError = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERPROCESSOR_AUTHORIZATIONERROR,
					2, ulRTSPSessionIdentifier,
					((const char *) _bAuthorizationBuffer) + 3);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errAuthorizationError, __FILE__, __LINE__);

				if (closeConnection (ulRTSPSessionIdentifier,
					&errAuthorizationError) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_CLOSECONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			// return err; it is used errNoError to avoid the trace of the error
			return errNoError;
		}
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_HANDLEDESCRIBERESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			{
				Error errAuthorizationError = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERPROCESSOR_AUTHORIZATIONERROR,
					2, ulRTSPSessionIdentifier,
					((const char *) _bAuthorizationBuffer) + 3);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errAuthorizationError, __FILE__, __LINE__);

				if (closeConnection (ulRTSPSessionIdentifier,
					&errAuthorizationError) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_CLOSECONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			return err;
		}
	}

	{
		Error errAuthorizationError = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_AUTHORIZATIONERROR,
			2, ulRTSPSessionIdentifier,
			((const char *) _bAuthorizationBuffer) + 3);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errAuthorizationError, __FILE__, __LINE__);

		if (closeConnection (ulRTSPSessionIdentifier,
			&errAuthorizationError) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERPROCESSOR_CLOSECONNECTION_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error StreamingServerProcessor:: handleAuthorizationDeniedEvent (
	RTSPConnectionEvent_p pevRTSPConnection)

{

	unsigned long			ulRTSPSessionIdentifier;
	Error_t					errHandleDESCRIBEResponse;
	char					pSequenceNumber [SS_MAXLONGLENGTH];
	unsigned long			ulSequenceNumber;


	if (pevRTSPConnection -> getRTSPSessionIdentifier (
		&ulRTSPSessionIdentifier) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_GETRTSPSESSIONIDENTIFIER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_STARTEVENTHANDLER,
			2, ulRTSPSessionIdentifier, "SS_EVENT_AUTHORIZATIONDENIED");
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (pevRTSPConnection -> getBuffer (&_bAuthorizationBuffer) !=
		errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_GETRTSPSESSIONIDENTIFIER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		{
			Error errAuthorizationError = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERPROCESSOR_AUTHORIZATIONDENIED,
				2, ulRTSPSessionIdentifier,
				((const char *) _bAuthorizationBuffer) + 3);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errAuthorizationError, __FILE__, __LINE__);

			if (closeConnection (ulRTSPSessionIdentifier,
				&errAuthorizationError) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERPROCESSOR_CLOSECONNECTION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		return err;
	}

	strncpy (pSequenceNumber, (const char *) _bAuthorizationBuffer, 3);
	pSequenceNumber [3]			= '\0';

	ulSequenceNumber			= atol (pSequenceNumber);

	if ((errHandleDESCRIBEResponse = (_prsRTSPSessions [
		ulRTSPSessionIdentifier]). handleDESCRIBEResponse (
		ulSequenceNumber, SS_EVENT_AUTHORIZATIONDENIED,
		((const char *) _bAuthorizationBuffer) + 3)) != errNoError)
	{
		if ((long) errHandleDESCRIBEResponse == SS_RTSPSESSION_WRONGSTATE)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_HANDLESENDERRTCPPACKETS_FAILED);
			// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			//	(const char *) err, __FILE__, __LINE__);

			{
				Error errAuthorizationError = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERPROCESSOR_AUTHORIZATIONDENIED,
					2, ulRTSPSessionIdentifier,
					((const char *) _bAuthorizationBuffer) + 3);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errAuthorizationError, __FILE__, __LINE__);

				if (closeConnection (ulRTSPSessionIdentifier,
					&errAuthorizationError) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_CLOSECONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			// return err; it is used errNoError to avoid the trace of the error
			return errNoError;
		}
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_HANDLEDESCRIBERESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			{
				Error errAuthorizationError = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERPROCESSOR_AUTHORIZATIONDENIED,
					2, ulRTSPSessionIdentifier,
					((const char *) _bAuthorizationBuffer) + 3);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errAuthorizationError, __FILE__, __LINE__);

				if (closeConnection (ulRTSPSessionIdentifier,
					&errAuthorizationError) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_CLOSECONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			return err;
		}
	}

	{
		Error errAuthorizationError = StreamingServerErrors (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_AUTHORIZATIONDENIED,
			2, ulRTSPSessionIdentifier,
			((const char *) _bAuthorizationBuffer) + 3);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errAuthorizationError, __FILE__, __LINE__);

		if (closeConnection (ulRTSPSessionIdentifier,
			&errAuthorizationError) != errNoError)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_STREAMINGSERVERPROCESSOR_CLOSECONNECTION_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error StreamingServerProcessor:: handleAuthorizationGrantedEvent (
	RTSPConnectionEvent_p pevRTSPConnection)

{

	unsigned long			ulRTSPSessionIdentifier;
	Error_t					errHandleDESCRIBEResponse;
	char					pSequenceNumber [SS_MAXLONGLENGTH];
	unsigned long			ulSequenceNumber;


	if (pevRTSPConnection -> getRTSPSessionIdentifier (
		&ulRTSPSessionIdentifier) != errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_GETRTSPSESSIONIDENTIFIER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_STREAMINGSERVERPROCESSOR_STARTEVENTHANDLER,
			2, ulRTSPSessionIdentifier, "SS_EVENT_AUTHORIZATIONGRANTED");
		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (pevRTSPConnection -> getBuffer (&_bAuthorizationBuffer) !=
		errNoError)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_GETBUFFER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		{
			if (closeConnection (ulRTSPSessionIdentifier,
				&err) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_STREAMINGSERVERPROCESSOR_CLOSECONNECTION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		return err;
	}

	strncpy (pSequenceNumber, (const char *) _bAuthorizationBuffer, 3);
	pSequenceNumber [3]			= '\0';

	ulSequenceNumber			= atol (pSequenceNumber);

	if ((errHandleDESCRIBEResponse = (_prsRTSPSessions [
		ulRTSPSessionIdentifier]). handleDESCRIBEResponse (
		ulSequenceNumber, SS_EVENT_AUTHORIZATIONGRANTED,
		((const char *) _bAuthorizationBuffer) + 3)) != errNoError)
	{
		if ((long) errHandleDESCRIBEResponse == SS_RTSPSESSION_WRONGSTATE)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_HANDLESENDERRTCPPACKETS_FAILED);
			// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			//	(const char *) err, __FILE__, __LINE__);

			{
				if (closeConnection (ulRTSPSessionIdentifier,
					&err) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_CLOSECONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			// return err; it is used errNoError to avoid the trace of the error
			return errNoError;
		}
		else
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTSPSESSION_HANDLEDESCRIBERESPONSE_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			{
				if (closeConnection (ulRTSPSessionIdentifier,
					&err) != errNoError)
				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_STREAMINGSERVERPROCESSOR_CLOSECONNECTION_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			return err;
		}
	}


	return errNoError;
}


Error StreamingServerProcessor:: closeConnection (
	unsigned long ulRTSPSessionIdentifier,
	Error_p perr)

{

	Error_t					errCloseRTSPConnection;
	#ifdef WIN32
		__int64					ullLocalTotalBytesServed;
		__int64					ullLocalTotalLostPacketsNumber;
		__int64					ullLocalTotalSentPacketsNumber;
	#else
		unsigned long long		ullLocalTotalBytesServed;
		unsigned long long		ullLocalTotalLostPacketsNumber;
		unsigned long long		ullLocalTotalSentPacketsNumber;
	#endif


	{
		if ((errCloseRTSPConnection = (_prsRTSPSessions [ulRTSPSessionIdentifier]).
			closeRTSPConnection (perr,
			&ullLocalTotalBytesServed,
			&ullLocalTotalLostPacketsNumber,
			&ullLocalTotalSentPacketsNumber)) != errNoError)
		{
			if ((long) errCloseRTSPConnection == SS_RTSPSESSION_WRONGSTATE)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
				// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				//	(const char *) err, __FILE__, __LINE__);

				// return err; it is used errNoError to avoid the trace of the error
				return errNoError;
			}
			else
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_RTSPSESSION_CLOSERTSPCONNECTION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				// return err;
			}
		}
	}

	if (_pmtFreeRTSPSessions -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	_pvFreeRTSPSessions -> insert (_pvFreeRTSPSessions -> end (),
		&(_prsRTSPSessions [ulRTSPSessionIdentifier]));

	if (_pmtFreeRTSPSessions -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmtStreamingServerStatistics -> lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	(*_pullTotalBytesServed)			+= ullLocalTotalBytesServed;
	(*_pullTotalLostPacketsNumber)		+= ullLocalTotalLostPacketsNumber;
	(*_pullTotalSentPacketsNumber)		+= ullLocalTotalSentPacketsNumber;

	if (_pmtStreamingServerStatistics -> unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error StreamingServerProcessor:: getIsShutdown (
	Boolean_p pbIsShutdown)

{

	if (_mtShutdown. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pbIsShutdown				= _bIsShutdown;

	if (_mtShutdown. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	return errNoError;
}


Error StreamingServerProcessor:: setIsShutdown (
	Boolean_t bIsShutdown)

{

	if (_mtShutdown. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	_bIsShutdown			= bIsShutdown;

	if (_mtShutdown. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	return errNoError;
}

