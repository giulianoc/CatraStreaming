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

#include "TimesForNexencoder.h"
#include "SchedulerForEncoderMessages.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <iostream.h>
#include <stdio.h>



TimesForNexencoder:: TimesForNexencoder (void): Times ()

{

}


TimesForNexencoder:: ~TimesForNexencoder (void)

{

}


Error TimesForNexencoder:: init (const char *pName,
	const char *pSchedule,
	const char *pSourcesPath, const char *pEncodedPath,
	const char *pProfilesPath,
	const char *pTemporaryPath,
	const char *pFTPUser, const char *pFTPPassword,
	const char *pFTPHost, const char *pFTPRootPath,
	const char *pFilesList,
	const char *pIpAddress, unsigned long ulPort,
	const char *pPathName,
	const char *pUser, const char *pPassword,
	Tracer_p ptTracer)

{

	Error_t					errInit;


	strcpy (_pName, pName);
	_ptTracer			= ptTracer;

	if (_rfnRequestForNexencoder. init (pName, pSourcesPath, pEncodedPath,
		pProfilesPath, pTemporaryPath, pFTPUser, pFTPPassword,
		pFTPHost, pFTPRootPath,
		pFilesList, pIpAddress, ulPort, pPathName,
		pUser, pPassword,
		ptTracer) != errNoError)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_REQUESTFORNEXENCODER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((errInit = Times:: init (pSchedule)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errInit,
			__FILE__, __LINE__);

		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_TIMES_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_rfnRequestForNexencoder. finish () != errNoError)
		{
			Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
				SFE_REQUESTFORNEXENCODER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}


	return errNoError;
}


Error TimesForNexencoder:: finish (void)

{

	if (Times:: finish () != errNoError)
	{
		Error err = SchedulerErrors (__FILE__, __LINE__,
			SCH_TIMES_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);
	}

	if (_rfnRequestForNexencoder. finish () != errNoError)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_REQUESTFORNEXENCODER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}


	return errNoError;
}


Error TimesForNexencoder:: handleTimeOut (void)

{

	PosixThread:: PThreadStatus_t				tsPThreadStatus;

	{
		Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
			SFE_TIMESFORNEXENCODER_TIMEOUT,
			2, _pName, _pNextExpirationDateTime);
		_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (_rfnRequestForNexencoder. getThreadState (&tsPThreadStatus) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_GETTHREADSTATE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return errNoError;
	}

	if (tsPThreadStatus == PosixThread:: THREADLIB_INITIALIZED)
	{
		if (_rfnRequestForNexencoder. start () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PTHREAD_START_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return errNoError;
		}
	}
	else
	{
		Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
			SFE_TIMESFORNEXENCODER_PREVIOUSSCHEDULINGNOTFINISHED,
			1, _pName);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) msg, __FILE__, __LINE__);

		return errNoError;
	}


	return errNoError;
}

