
#include "RTPOverBufferWindow.h"

#ifdef MPATROL
	#include "mpatrol.h"
#endif


RTPOverBufferWindow:: RTPOverBufferWindow (void)

{

	_stRTPOverBufferWindowStatus			= ROBW_BUILDED;

}


RTPOverBufferWindow:: ~RTPOverBufferWindow (void)

{

}


Error RTPOverBufferWindow:: init (
	unsigned long ulIntervalBetweenBuckets,
	unsigned long ulInitialMaxWindowSizeInBytes,
	unsigned long ulMaxSendAheadTimeInSec,
	Tracer_p ptTracer)

{

	if (_stRTPOverBufferWindowStatus == ROBW_INITIALIZED ||
		ptTracer == (Tracer_p) NULL)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);

		return err;
	}

	_ptSystemTracer								= ptTracer;
	_ulMaxWindowSizeInBytes					= ulInitialMaxWindowSizeInBytes;
	_ulCurrentBytesInWindow					= 0;
	_ulIntervalBetweenBuckets				= ulIntervalBetweenBuckets;
	_ulStartBucketIndex						= 0;
	_ullAbsoluteStartTransmitTimeInMilliSecs	= 0;
	_bWriteBurstBeginning					= true;
	_bOverBufferingEnabled					= true;
	_ulStartBucketInThisWriteBurst			= 0;

	// The number of buckets is a function of how much time
	// each bucket represents, times the amount of allowed send
	// ahead time we have.
	_ulBucketsNumber				=
		((ulMaxSendAheadTimeInSec * 1000) / ulIntervalBetweenBuckets) + 1;

	if ((_pulBuckets = new unsigned long [_ulBucketsNumber]) ==
		(unsigned long *) NULL)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_NEW_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	::memset (_pulBuckets, 0, sizeof (unsigned long) * _ulBucketsNumber);

	_stRTPOverBufferWindowStatus			= ROBW_INITIALIZED;


	return errNoError;
}


Error RTPOverBufferWindow:: finish (void)

{

	if (_stRTPOverBufferWindowStatus != ROBW_INITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	delete [] _pulBuckets;

	_stRTPOverBufferWindowStatus			= ROBW_BUILDED;


	return errNoError;
}


#ifdef WIN32
	Error RTPOverBufferWindow:: addPacketToWindow (
		const __int64 ullAbsolutePacketTransmissionTimeInMilliSecs,
		const __int64 ullAbsoluteCurrentTimeInMilliSecs,
		unsigned long ulPacketSize, __int64 *pllSuggestedWakeupTime)
#else
	Error RTPOverBufferWindow:: addPacketToWindow (
		const unsigned long long ullAbsolutePacketTransmissionTimeInMilliSecs,
		const unsigned long long ullAbsoluteCurrentTimeInMilliSecs,
		unsigned long ulPacketSize, long long *pllSuggestedWakeupTime)
#endif

{

	#ifdef WIN32
		__int64						ullLocalStartTransmitTime;
	#else
		unsigned long long			ullLocalStartTransmitTime;
	#endif
	unsigned long				ulBytesAvailable;
	unsigned long				ulLocalStartBucketIndex;
	unsigned long				ulBucketIndex;
	unsigned long				ulAdjustedBucketIndex;


	if (_stRTPOverBufferWindowStatus != ROBW_INITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_ulCurrentBytesInWindow == 0)
	{
		_ullAbsoluteStartTransmitTimeInMilliSecs	= ullAbsoluteCurrentTimeInMilliSecs;
		_ulStartBucketIndex						= 0;
	}
	
	if (ullAbsolutePacketTransmissionTimeInMilliSecs <=
		_ullAbsoluteStartTransmitTimeInMilliSecs)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_RTPOVERBUFFERWINDOW_PACKETBEFOREWINDOW);
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		// 	(const char *) err, __FILE__, __LINE__);

		/*
		{
			int fd = 0;
			char pBuffer [512];

			fd = ::open ("./GIUwindow.my", O_WRONLY | O_APPEND | O_CREAT,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

			sprintf (pBuffer, "%ld - %ld 1\n", _ulMaxWindowSizeInBytes, -1);

			::write (fd, pBuffer, strlen (pBuffer));

			::close (fd);
		}
		*/

		return err;
	}

	// Check to see if this packet can fit in the window.
	// If it can't, return the time at which it will be able to fit.
	// If the packet is bigger than the window itself, no amount of
	// finagling will get it into the window, so this packet
	// must be sent on time.
	if (ulPacketSize > _ulMaxWindowSizeInBytes || !_bOverBufferingEnabled)
	{
		*pllSuggestedWakeupTime			=
			ullAbsolutePacketTransmissionTimeInMilliSecs + _ulIntervalBetweenBuckets;

		/*
		{
			int fd = 0;
			char pBuffer [512];

			fd = ::open ("./GIUwindow.my", O_WRONLY | O_APPEND | O_CREAT,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

			sprintf (pBuffer, "%ld - %lld 2\n", _ulMaxWindowSizeInBytes,
				llAbsolutePacketTransmissionTimeInMilliSecs + _ulIntervalBetweenBuckets);

			::write (fd, pBuffer, strlen (pBuffer));

			::close (fd);
		}
		*/

		return errNoError;
	}

	ulBytesAvailable			=
		_ulMaxWindowSizeInBytes - _ulCurrentBytesInWindow;

	ullLocalStartTransmitTime			= _ullAbsoluteStartTransmitTimeInMilliSecs;
	ulLocalStartBucketIndex				= _ulStartBucketIndex;

	while (ulBytesAvailable < ulPacketSize)
	{
		ulBytesAvailable				+= _pulBuckets[ulLocalStartBucketIndex];

		ulLocalStartBucketIndex++;
		if (ulLocalStartBucketIndex == _ulBucketsNumber)
			ulLocalStartBucketIndex = 0;

		if (ulLocalStartBucketIndex != _ulStartBucketIndex)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_RTPOVERBUFFERWINDOW_WINDOWVARIABLESNOTCONSISTENT);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		ullLocalStartTransmitTime		+= _ulIntervalBetweenBuckets;
	}

	// If there isn't enough space right now, return a time in the future where
	// there will be space. Always exaggerate the times by 1 bucket.
	// This is simply so that we do more work at once.
	if (ullLocalStartTransmitTime > ullAbsoluteCurrentTimeInMilliSecs)
	{
		*pllSuggestedWakeupTime			=
			ullLocalStartTransmitTime + _ulIntervalBetweenBuckets;

		/*
		{
			int fd = 0;
			char pBuffer [512];

			fd = ::open ("./GIUwindow.my", O_WRONLY | O_APPEND | O_CREAT,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

			sprintf (pBuffer, "%ld - %lld 3\n", _ulMaxWindowSizeInBytes,
				llLocalStartTransmitTime + _ulIntervalBetweenBuckets);

			::write (fd, pBuffer, strlen (pBuffer));

			::close (fd);
		}
		*/

		return errNoError;
	}

	// The implementation of this window doesn't track individual packets
	// that you add. That would be too memory intensive.
	// Rather, it throws packets into "buckets" that are separated
	// by a fixed time interval.

	ulBucketIndex				= (unsigned long) (((
		ullAbsolutePacketTransmissionTimeInMilliSecs -
		_ullAbsoluteStartTransmitTimeInMilliSecs) /
		_ulIntervalBetweenBuckets) +
		_ulStartBucketIndex);

	ulAdjustedBucketIndex		= ulBucketIndex;

	if (ulAdjustedBucketIndex >= _ulBucketsNumber)
	{
		ulAdjustedBucketIndex		-= _ulBucketsNumber;

		// Check to see if we are so far ahead we can't put
		// this packet in a bucket.
		if (ulAdjustedBucketIndex >= _ulStartBucketIndex)
		{
			*pllSuggestedWakeupTime			=
				ullAbsoluteCurrentTimeInMilliSecs + _ulIntervalBetweenBuckets;

			/*
			{
				int fd = 0;
				char pBuffer [512];

				fd = ::open ("./GIUwindow.my", O_WRONLY | O_APPEND | O_CREAT,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

				sprintf (pBuffer, "%ld - %lld 4\n", _ulMaxWindowSizeInBytes,
					llAbsoluteCurrentTimeInMilliSecs + _ulIntervalBetweenBuckets);

				::write (fd, pBuffer, strlen (pBuffer));

				::close (fd);
			}
			*/

			return errNoError;
		}
	}

	if (_bWriteBurstBeginning)
	{
		_bWriteBurstBeginning						= false;
		_ulStartBucketInThisWriteBurst				= ulBucketIndex;
	}

	// Or that our play rate has exceeded the max allowed play rate.
	if (ulBucketIndex - _ulStartBucketInThisWriteBurst > SS_MAXPLAYRATEALLOWED)
	{
		{
			*pllSuggestedWakeupTime			=
				ullAbsoluteCurrentTimeInMilliSecs + _ulIntervalBetweenBuckets;

			/*
			{
				int fd = 0;
				char pBuffer [512];

				fd = ::open ("./GIUwindow.my", O_WRONLY | O_APPEND | O_CREAT,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

				sprintf (pBuffer, "%ld - %lld 5\n", _ulMaxWindowSizeInBytes,
					llAbsoluteCurrentTimeInMilliSecs + _ulIntervalBetweenBuckets);

				::write (fd, pBuffer, strlen (pBuffer));

				::close (fd);
			}
			*/

			return errNoError;
		}
	}

	_pulBuckets[ulAdjustedBucketIndex] += ulPacketSize;
	_ulCurrentBytesInWindow += ulPacketSize;


	/*
	{
		int fd = 0;
		char pBuffer [512];

		fd = ::open ("./GIUwindow.my", O_WRONLY | O_APPEND | O_CREAT,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

		sprintf (pBuffer, "%ld - %ld 6\n", _ulMaxWindowSizeInBytes,
			-1);

		::write (fd, pBuffer, strlen (pBuffer));

		::close (fd);
	}
	*/

	*pllSuggestedWakeupTime			= -1;


	return errNoError;
}


#ifdef WIN32
	Error RTPOverBufferWindow:: emptyOutWindow (
		const __int64 ullAbsoluteCurrentTimeInMilliSecs)
#else
	Error RTPOverBufferWindow:: emptyOutWindow (
		const unsigned long long ullAbsoluteCurrentTimeInMilliSecs)
#endif

{

	if (_stRTPOverBufferWindowStatus != ROBW_INITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// As time passes, buckets get zeroed out, emptying out the window
	while (ullAbsoluteCurrentTimeInMilliSecs >
		(_ullAbsoluteStartTransmitTimeInMilliSecs +
		_ulIntervalBetweenBuckets))
	{
		// For some reason there are more packets in the buckets
		// than in the window.
		if (_pulBuckets[_ulStartBucketIndex] > _ulCurrentBytesInWindow)
			_ulCurrentBytesInWindow				= 0;
		else
			_ulCurrentBytesInWindow				-=
				_pulBuckets [_ulStartBucketIndex];
		// this will wrap below 0 and later assert unless
		// we test before subtracting.
		_pulBuckets [_ulStartBucketIndex]		= 0;

		if (_ulCurrentBytesInWindow == 0)
			break;

		_ullAbsoluteStartTransmitTimeInMilliSecs +=
			_ulIntervalBetweenBuckets;
		_ulStartBucketIndex++;
		
		if (_ulStartBucketIndex == _ulBucketsNumber)
			_ulStartBucketIndex = 0;
	}


	return errNoError;
}


Error RTPOverBufferWindow:: getBytesInWindow (unsigned long *pulBytesInWindow)

{

	if (_stRTPOverBufferWindowStatus != ROBW_INITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*pulBytesInWindow				= _ulCurrentBytesInWindow;


	return errNoError;
}


Error RTPOverBufferWindow:: getIntervalBetweenBuckets (
	unsigned long *pulIntervalBetweenBuckets)

{

	if (_stRTPOverBufferWindowStatus != ROBW_INITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*pulIntervalBetweenBuckets				= _ulIntervalBetweenBuckets;


	return errNoError;
}


Error RTPOverBufferWindow:: getAvailableSpaceInWindow (
	long *plCurrentBytesInWindow)

{

	if (_stRTPOverBufferWindowStatus != ROBW_INITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*plCurrentBytesInWindow				=
		_ulMaxWindowSizeInBytes - _ulCurrentBytesInWindow;


	return errNoError;
}


Error RTPOverBufferWindow:: setWindowSizeInByte (
	unsigned long ulWindowSizeInBytes)

{

	if (_stRTPOverBufferWindowStatus != ROBW_INITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	{
		Message msg = StreamingServerMessages (__FILE__, __LINE__,
			SS_RTPOVERBUFFERWINDOW_WINDOWSIZE,
			1, ulWindowSizeInBytes);

		_ptSystemTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	_ulMaxWindowSizeInBytes				= ulWindowSizeInBytes;


	return errNoError;
}


Error RTPOverBufferWindow:: turnOffOverbuffering (void)

{

	if (_stRTPOverBufferWindowStatus != ROBW_INITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	_bOverBufferingEnabled				= false;


	return errNoError;
}


Error RTPOverBufferWindow:: TurnOnOverbuffering (void)

{

	if (_stRTPOverBufferWindowStatus != ROBW_INITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	_bOverBufferingEnabled				= true;


	return errNoError;
}


Error RTPOverBufferWindow:: markBeginningOfWriteBurst (void)

{

	if (_stRTPOverBufferWindowStatus != ROBW_INITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	_bWriteBurstBeginning				= true;


	return errNoError;
}

