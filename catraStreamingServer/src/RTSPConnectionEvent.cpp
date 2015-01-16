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
#include "RTSPConnectionEvent.h"
#include "DateTime.h"


RTSPConnectionEvent:: RTSPConnectionEvent (void): Event ()

{

}


RTSPConnectionEvent:: ~RTSPConnectionEvent (void)

{

}


RTSPConnectionEvent:: RTSPConnectionEvent (const RTSPConnectionEvent &)

{

	assert (1==0);
}


RTSPConnectionEvent &RTSPConnectionEvent:: operator = (const RTSPConnectionEvent &)

{

	assert (1==0);

	return *this;
}


#ifdef WIN32
	Error RTSPConnectionEvent:: init (const char *pSource,
		long lTypeIdentifier, const char *pTypeIdentifier, unsigned long ulRTSPSessionIdentifier,
		Tracer_p ptTracer, Boolean_t bExpirationNow,
		__int64 ullExpirationLocalDateTimeInMilliSecs,
		Buffer_p pbBuffer)
#else
	Error RTSPConnectionEvent:: init (const char *pSource,
		long lTypeIdentifier, const char *pTypeIdentifier, unsigned long ulRTSPSessionIdentifier,
		Tracer_p ptTracer, Boolean_t bExpirationNow,
		unsigned long long ullExpirationLocalDateTimeInMilliSecs,
		Buffer_p pbBuffer)
#endif

{

	#ifdef WIN32
		__int64				ullLocalExpirationLocalDateTimeInMilliSecs;
	#else
		unsigned long long	ullLocalExpirationLocalDateTimeInMilliSecs;
	#endif


	// ulRTSPSessionIdentifier is -1 in case of new RTSP connection
	_ulRTSPSessionIdentifier	= ulRTSPSessionIdentifier;
	_ptSystemTracer					= ptTracer;

	if (pbBuffer == (Buffer_p) NULL)
	{
		_bBufferInitialized				= false;
	}
	else
	{
		if (_bBuffer. init ((const char *) (*pbBuffer)) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		_bBufferInitialized				= true;
	}

	if (bExpirationNow)
	{
		if (DateTime:: nowLocalInMilliSecs (
			&ullLocalExpirationLocalDateTimeInMilliSecs) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_DATETIME_NOWLOCALINMILLISECS_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else
		ullLocalExpirationLocalDateTimeInMilliSecs		=
			ullExpirationLocalDateTimeInMilliSecs;

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


Error RTSPConnectionEvent:: finish (void)

{

	if (Event:: finish () != errNoError)
	{
		Error err = EventsSetErrors (__FILE__, __LINE__,
			EVSET_EVENT_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_bBufferInitialized)
	{
		if (_bBuffer. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		_bBufferInitialized				= false;
	}


	return errNoError;
}


Error RTSPConnectionEvent:: getRTSPSessionIdentifier (
	unsigned long *pulRTSPSessionIdentifier)

{

	if (_ulRTSPSessionIdentifier == (unsigned long) -1)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_NEWRTSPCONNECTION);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pulRTSPSessionIdentifier			= _ulRTSPSessionIdentifier;


	return errNoError;
}


Error RTSPConnectionEvent:: getBuffer (
	Buffer_p pbBuffer)

{

	if (!_bBufferInitialized)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTSPCONNECTIONEVENT_BUFFERNOTINITIALIZED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbBuffer -> setBuffer ((const char *) _bBuffer) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}
