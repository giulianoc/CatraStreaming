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


#include "CheckSocketsPoolTimes.h"
#include <assert.h>


CheckSocketsPoolTimes:: CheckSocketsPoolTimes (void): Times ()

{

}


CheckSocketsPoolTimes:: ~CheckSocketsPoolTimes (void)

{

}


CheckSocketsPoolTimes:: CheckSocketsPoolTimes (const CheckSocketsPoolTimes &t)

{

	assert (1==0);

	// to do

	*this = t;
}


Error CheckSocketsPoolTimes:: init (
	unsigned long ulPeriodInMilliSecs,
	StreamingServerSocketsPool_p pspSocketsPool,
	Tracer_p ptTracer)

{

	_pspSocketsPool				= pspSocketsPool;
	_ptSystemTracer					= ptTracer;

	if (Times:: init (ulPeriodInMilliSecs,
		SS_CHECKSOCKETSPOOLTIMES_CLASSNAME) != errNoError)
	{
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_TIMES_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error CheckSocketsPoolTimes:: finish (void)

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


Error CheckSocketsPoolTimes:: handleTimeOut (void)

{

	Error				errCheckSocketsStatus;


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

	if ((errCheckSocketsStatus = _pspSocketsPool -> checkSocketsStatus (
		SS_CHECKSOCKETSSTATUS_SECONDSTOWAIT,
		SS_CHECKSOCKETSSTATUS_MICROSECONDSTOWAIT)) != errNoError)
	{
		if ((long) errCheckSocketsStatus !=
			SCK_SOCKETSPOOL_SOCKETSTATUSNOTCHANGED)
		{
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errCheckSocketsStatus,
				__FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETSPOOL_CHECKSOCKETSSTATUS_FAILED);
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
