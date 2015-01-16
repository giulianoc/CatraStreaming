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

#include <assert.h>
#ifdef WIN32
	#include "Windows.h"
#endif
#include "LiveEvent.h"
#include "DateTime.h"


LiveEvent:: LiveEvent (void): Event ()

{

}


LiveEvent:: ~LiveEvent (void)

{

}


LiveEvent:: LiveEvent (const LiveEvent &)

{

	assert (1==0);
}


LiveEvent &LiveEvent:: operator = (const LiveEvent &)

{

	assert (1==0);

	return *this;
}


Error LiveEvent:: init (const char *pSource,
	long lTypeIdentifier,
	const char *pTypeIdentifier,
	Socket_p pssServerSocket,
	void *pvLiveSource,
	Tracer_p ptTracer)

{

	#ifdef WIN32
		__int64				ullLocalExpirationLocalDateTimeInMilliSecs;
	#else
		unsigned long long	ullLocalExpirationLocalDateTimeInMilliSecs;
	#endif


	_pssServerSocket		= (ServerSocket_p) pssServerSocket;
	_pvLiveSource			= pvLiveSource;
	_ptSystemTracer				= ptTracer;

	if (DateTime:: nowLocalInMilliSecs (
		&ullLocalExpirationLocalDateTimeInMilliSecs) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_DATETIME_NOWLOCALINMILLISECS_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (Event:: init (pSource,
		"StreamingServerProcessor",
		lTypeIdentifier,
		pTypeIdentifier,
		ullLocalExpirationLocalDateTimeInMilliSecs) !=
		errNoError)
	{
		Error err = EventsSetErrors (__FILE__, __LINE__,
			EVSET_EVENT_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error LiveEvent:: finish (void)

{

	if (Event:: finish () != errNoError)
	{
		Error err = EventsSetErrors (__FILE__, __LINE__,
			EVSET_EVENT_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error LiveEvent:: getLiveEventInfo (
	void **pvLiveSource,
	ServerSocket_p *pssServerSocket)

{

	*pvLiveSource			= _pvLiveSource;
	*pssServerSocket		= _pssServerSocket;


	return errNoError;
}

