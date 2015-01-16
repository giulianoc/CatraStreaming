
#include "RTPOverBufferWindow2.h"


RTPOverBufferWindow2:: RTPOverBufferWindow2 (void)

{

}


RTPOverBufferWindow2:: ~RTPOverBufferWindow2 (void)

{

}


Error RTPOverBufferWindow2:: init (
	unsigned long ulSendInterval,
	unsigned long ulInitialWindowSizeInBytes,
	unsigned long ulMaxSendAheadTimeInSec,
	float fOverbufferRate)

{

	_lWindowSizeInBytes				= ulInitialWindowSizeInBytes;
	_lBytesSentSinceLastReport		= 0;
	_lSendInterval					= ulSendInterval;
	_lBytesDuringLastSecond			= 0;
	_llLastSecondStart				= -1;
	_lBytesDuringPreviousSecond		= 0;
	_llPreviousSecondStart			= -1;
	_lBytesDuringBucket				= 0;
	_llBucketBegin					= 0;
	_llBucketTimeAhead				= 0;
	_llPreviousBucketTimeAhead		= 0;
	_ulMaxSendAheadTime				= ulMaxSendAheadTimeInSec * 1000;
	_usWriteBurstBeginning			= false;
	_usOverbufferingEnabled			= true;
	_fOverbufferRate				= fOverbufferRate;
	_ulSendAheadDurationInMsec		= 1000;
	_llOverbufferWindowBegin		= -1;

	if (_lSendInterval == 0)
	{
		_usOverbufferingEnabled		= false;
		_lSendInterval				= 200;
	}

	if (_fOverbufferRate < 1.0)
		_fOverbufferRate			= 1.0;


	return errNoError;
}


#ifdef WIN32
	__int64 RTPOverBufferWindow2:: checkTransmitTime (
		const __int64 &llTransmitTime,
		const __int64 &llCurrentTime,
		unsigned long ulPacketSize)
#else
	long long RTPOverBufferWindow2:: checkTransmitTime (
		const long long &llTransmitTime,
		const long long &llCurrentTime,
		unsigned long ulPacketSize)
#endif

{

	// if this is the beginning of a bucket interval, roll over figures from last time.
	// accumulate statistics over the period of a second
	if (llCurrentTime - _llBucketBegin > _lSendInterval)
	{
		// start a new bucket
		_llPreviousBucketBegin			= _llBucketBegin;
		_llBucketBegin					= llCurrentTime;
		if (_llPreviousBucketBegin == 0)
			_llPreviousBucketBegin		= _llBucketBegin - _lSendInterval;
		_lBytesDuringBucket				= 0;
		if (llCurrentTime - _llLastSecondStart > 1000)
		{
			_lBytesDuringPreviousSecond	= _lBytesDuringLastSecond;
			_lBytesDuringLastSecond		= 0;
			_llPreviousSecondStart		= _llLastSecondStart;
			_llLastSecondStart			= llCurrentTime;
		}

		_llPreviousBucketTimeAhead		= _llBucketTimeAhead;
	}

	if (_llOverbufferWindowBegin == -1)
		_llOverbufferWindowBegin		= llCurrentTime;

	/*
	std:: cout << "_lBytesDuringLastSecond: "
		<< _lBytesDuringLastSecond * 8
		<< std:: endl;
	*/

	if ((llTransmitTime <= llCurrentTime + _lSendInterval) || 
		(_usOverbufferingEnabled && 
		(llTransmitTime <= llCurrentTime + _lSendInterval + _ulSendAheadDurationInMsec)))
	{
		//
		// If this happens, this packet needs to be sent regardless of overbuffering

		return -1;
	}

	if (!_usOverbufferingEnabled || (_lWindowSizeInBytes == 0))
		return llTransmitTime;

	// if the client is running low on memory, wait a while for it to be freed up
	// there's nothing magic about these numbers, we're just trying to be conservative
	if ((_lWindowSizeInBytes != -1) &&
		(ulPacketSize * 5 > _lWindowSizeInBytes - _lBytesSentSinceLastReport))
	{

		return llCurrentTime + (_lSendInterval * 5);  // client reports don't come that often
	}

	// if we're far enough ahead, then wait until it's time to send more packets
	if (llTransmitTime - llCurrentTime > _ulMaxSendAheadTime)
		return llTransmitTime - _ulMaxSendAheadTime + _lSendInterval;

// during the first second just send packets normally
//    if (_llPreviousSecondStart == -1)
//        return llCurrentTime + _lSendInterval;

	// now figure if we want to send this packet during this bucket.  We have two limitations.
	// First we scale up bitrate slowly, so we should only try and send a little more than we
	// sent recently (averaged over a second or two).  However, we always try and send at
	// least the current bitrate and never more than double.
//    long currentBitRate = _lBytesDuringBucket * 1000 / (llCurrentTime - _llPreviousBucketBegin);
//  long averageBitRate = (_lBytesDuringPreviousSecond + _lBytesDuringLastSecond) * 1000 / (llCurrentTime - _llPreviousSecondStart);
//    long averageBitRate = _lBytesDuringPreviousSecond * 1000 / (_llLastSecondStart - _llPreviousSecondStart);
	_llBucketTimeAhead			= llTransmitTime - llCurrentTime;
//	printf("Current br = %d, average br = %d (cta = %qd, pta = %qd)\n", currentBitRate, averageBitRate, currentTimeAhead, _llPreviousBucketTimeAhead);

	// always try and stay as far ahead as we were before
	if (_llBucketTimeAhead < _llPreviousBucketTimeAhead)
		return -1;

	// but don't send at more that double the bitrate (for any given time we should only get further
	// ahead by that amount of time)
	//printf("cta - pta = %qd, ct - pbb = %qd\n", _llBucketTimeAhead - _llPreviousBucketTimeAhead, long long((llCurrentTime - _llPreviousBucketBegin) * (_fOverbufferRate - 1.0)));
	if (_llBucketTimeAhead - _llPreviousBucketTimeAhead >
		((llCurrentTime - _llPreviousBucketBegin) * (_fOverbufferRate - 1.0)))
	{
		#ifdef WIN32
			_llBucketTimeAhead			= _llPreviousBucketTimeAhead +
				(__int64) ((llCurrentTime - _llPreviousBucketBegin) * (_fOverbufferRate - 1.0));
		#else
			_llBucketTimeAhead			= _llPreviousBucketTimeAhead +
				(long long) ((llCurrentTime - _llPreviousBucketBegin) * (_fOverbufferRate - 1.0));
		#endif


		return llCurrentTime + _lSendInterval;		// this will get us to the next bucket
	}

	// don't send more than 10% over the average bitrate for the previous second
//    if (currentBitRate > averageBitRate * 11 / 10)
//        return llCurrentTime + _lSendInterval;       // this will get us to the next bucket

	return -1;  // send this packet
}


void RTPOverBufferWindow2:: resetOverBufferWindow (void)

{

    _lBytesDuringLastSecond			= 0;
    _llLastSecondStart				= -1;
    _lBytesDuringPreviousSecond		= 0;
    _llPreviousSecondStart			= -1;
    _lBytesDuringBucket				= 0;
    _llBucketBegin					= 0;
    _llBucketTimeAhead				= 0;
    _llPreviousBucketTimeAhead		= 0;
	_llOverbufferWindowBegin		= -1;

}


void RTPOverBufferWindow2:: addPacketToWindow (
	unsigned long ulPacketSize)

{

	_lBytesDuringBucket				+= ulPacketSize;
	_lBytesDuringLastSecond			+= ulPacketSize;
	_lBytesSentSinceLastReport		+= ulPacketSize;

}


#ifdef WIN32
	void RTPOverBufferWindow2:: emptyOutWindow (
		const __int64 &llCurrentTime)
#else
	void RTPOverBufferWindow2:: emptyOutWindow (
		const long long &llCurrentTime)
#endif

{

	// no longer needed
}


void RTPOverBufferWindow2:: setWindowSize (unsigned long inWindowSizeInBytes)

{

	_lWindowSizeInBytes			= inWindowSizeInBytes;
	_lBytesSentSinceLastReport	= 0;

}
