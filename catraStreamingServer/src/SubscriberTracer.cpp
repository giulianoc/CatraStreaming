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


#include "SubscriberTracer.h"
#include <assert.h>


SubscriberTracer:: SubscriberTracer (void): Tracer ()

{

}


SubscriberTracer:: ~SubscriberTracer (void)

{

}


Error SubscriberTracer:: performTrace (time_t tCurrentUtcTime, long lTraceLevel,
	const char *pTraceMessage, const char *pFileName, long lFileLine,
	unsigned long ulThreadId, long lUpdateStackDeep)

{

	long		lTraceMessageLength;
	long		lCompleteTraceMessageLength;


//	ex trace format:
//	[date time][traceLevel][fileName fileLine][sessionName threadId][traceMsg]

	if (lUpdateStackDeep < 0)
		_lFunctionsStackDeep		-= 1;

	lTraceMessageLength					= strlen (pTraceMessage);

	lCompleteTraceMessageLength			= lTraceMessageLength;

	// a causa dello '\n' finale
	lCompleteTraceMessageLength			+= 1;

	if (_lCacheSizeOfTraceFile * 1000 < lCompleteTraceMessageLength)
	{
		Error err = TracerErrors (__FILE__, __LINE__,
			TRACER_TRACETOOLONG);
		std:: cerr << (const char *) err << std:: endl;

		return err;
	}

	if (!_bTraceFileCacheActive ||
		(_lCurrentCacheBusyInBytes + lCompleteTraceMessageLength >=
		_lCacheSizeOfTraceFile * 1000) ||
		time (NULL) - _tStartTraceFile > _lTraceFilePeriodInSecs)
	{
		if (flushTraceFileCache () != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_FLUSHTRACEFILECACHE_FAILED);
			std:: cerr << (const char *) err << std:: endl;

			return err;
		}
	}

	/*
	if (fillCompleteTraceMessage (_pTraceFileCache + _lCurrentCacheBusy,
		pCurrentDate, pCurrentTime, lTraceLevel, lTraceLevelLabelLength,
		pFileName, lFileNameLength,
		pFileLine, lFileLineLength,
		pThreadId, lThreadIdLength,
		pTraceMessage, lTraceMessageLength) != errNoError)
	*/
	{

		long			lIndex;
		long			lCurrentCompleteTrace;
		char			*pCompleteTraceMessageToFill;



		pCompleteTraceMessageToFill		=
			_pTraceFileCache + _lCurrentCacheBusyInBytes;
		lCurrentCompleteTrace			= 0;

	//	ex format trace
	//	[date time][traceLevel][fileName fileLine][sessionName threadId][traceMsg]

		for (lIndex = 0; lIndex < _lFunctionsStackDeep; lIndex++)
			pCompleteTraceMessageToFill [lCurrentCompleteTrace++]	= '\t';

		for (lIndex = 0; lIndex < lTraceMessageLength; lIndex++)
		{
			pCompleteTraceMessageToFill [lCurrentCompleteTrace++]		=
				pTraceMessage [lIndex];
		}

		pCompleteTraceMessageToFill [lCurrentCompleteTrace]			= '\0';
	}

	_pTraceFileCache [_lCurrentCacheBusyInBytes + lCompleteTraceMessageLength - 1]	=
		'\n';
	_pTraceFileCache [
		_lCurrentCacheBusyInBytes + lCompleteTraceMessageLength]		= '\0';

	_lCurrentCacheBusyInBytes				+= lCompleteTraceMessageLength;

	if (lUpdateStackDeep > 0)
		_lFunctionsStackDeep		+= 1;

	if (_bTraceOnTTY)
	{
		std:: cout << _pTraceFileCache + (_lCurrentCacheBusyInBytes -
				lCompleteTraceMessageLength);
		std:: cout. flush ();
/*
		long lIndex;
		for (lIndex = 0; lIndex < lCompleteTraceMessageLength; lIndex++)
			printf ("%c", _pTraceFileCache [_lCurrentCacheBusy -
				lCompleteTraceMessageLength + lIndex]);
*/
	}


	return errNoError;
}

