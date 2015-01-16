

// Class that tracks packets that are part of the "overbuffer". That is,
// packets that are being sent ahead of time. This class can be used
// to make sure the server isn't overflowing the client's overbuffer size.
// Time is mesured in milliseconds
// The packets in the window are distributed within the buckets according
// them transmission time
// Each element of the buckets contains the sum of the size
// of one or more packets

// Rules:
//	1. If the packet transmission time is inside the bucket,
//		the packet must be sent
//	2. If _usOverbufferingEnabled is true,
//		if the packet transmission time is inside the bucket extended of _ulSendAheadDurationInMsec,
//			the packet must be sent
//	3. ...

#ifndef RTPOverBufferWindow_h
	#define RTPOverBufferWindow_h

	#include "Tracer.h"
	#include "StreamingServerErrors.h"
	#include "StreamingServerMessages.h"

	typedef class RTPOverBufferWindow2

	{
		private:
			// max bytes inside the window
			long				_lWindowSizeInBytes;
			// Bytes sent since last RTCP report
			long				_lBytesSentSinceLastReport;
			// millisecond of a bucket
			long				_lSendInterval;

			// bytes sent during last second
			long				_lBytesDuringLastSecond;
			// Start of the last second in milliseconds
			#ifdef WIN32
				__int64				_llLastSecondStart;
			#else
				long long			_llLastSecondStart;
			#endif

			long				_lBytesDuringPreviousSecond;
			#ifdef WIN32
				__int64				_llPreviousSecondStart;
			#else
				long long			_llPreviousSecondStart;
			#endif

			// bytes sent inside a bucket
			long				_lBytesDuringBucket;
			// begin of a bucket in milliseconds
			#ifdef WIN32
				__int64				_llBucketBegin;
				__int64				_llPreviousBucketBegin;
			#else
				long long			_llBucketBegin;
				long long			_llPreviousBucketBegin;
			#endif

			// milliseconds that we are ahead
			//	It is: llTransmitTime - llCurrentTime
			#ifdef WIN32
				__int64				_llBucketTimeAhead;
				__int64				_llPreviousBucketTimeAhead;
			#else
				long long			_llBucketTimeAhead;
				long long			_llPreviousBucketTimeAhead;
			#endif

			// max send ahead time in milliseconds
			unsigned long		_ulMaxSendAheadTime;

			unsigned short		_usWriteBurstBeginning;
			unsigned short		_usOverbufferingEnabled;
			
			float				_fOverbufferRate;
			// 
			unsigned long		_ulSendAheadDurationInMsec;
			
			// begin of the overbuffer windows in milliseconds
			#ifdef WIN32
				__int64				_llOverbufferWindowBegin;
			#else
				long long			_llOverbufferWindowBegin;
			#endif

		public:
			RTPOverBufferWindow2 ();

			~RTPOverBufferWindow2 ();

			Error init (
				unsigned long ulSendInterval,	// = 50, dal file di conf.
				unsigned long ulInitialWindowSizeInBytes, // = ULONG_MAX (= -1) da <limits.h>
				unsigned long ulMaxSendAheadTimeInSec, // = 25, dal file di conf.
				float fOverbufferRate);	// = 2.0, dal file di conf.

			void resetOverBufferWindow (void);

			unsigned long getSendInterval (void)
			{
				return _lSendInterval;
			}

			// This may be negative!
			long availableSpaceInWindow (void)
			{
				return _lWindowSizeInBytes - _lBytesSentSinceLastReport;
			}

			//
			// The window size may be changed at any time
			void setWindowSize (unsigned long inWindowSizeInBytes);

			//
			// Without changing the window size, you can enable / disable all overbuffering
			// using these calls. Defaults to enabled
			void turnOffOverbuffering (void)
			{
				_usOverbufferingEnabled		= false;
			}

			void turnOnOverbuffering (void)
			{
				_usOverbufferingEnabled		= true;
			}

			unsigned short *overbufferingEnabledPtr (void)
			{
				return &_usOverbufferingEnabled;
			}

			//
			// If the overbuffer window is full, this returns a time in the future when
			// enough space will open up for this packet. Otherwise, returns -1.
			//
			// The overbuffer window is full if the byte count is filled up, or if the
			// bitrate is above the max play rate.
			#ifdef WIN32
				__int64 checkTransmitTime (
					const __int64 &llTransmitTime,
					const __int64 &llCurrentTime,
					unsigned long ulPacketSize);
			#else
				long long checkTransmitTime (
					const long long &llTransmitTime,
					const long long &llCurrentTime,
					unsigned long ulPacketSize);
			#endif

			//
			// Remembers that this packet has been sent
			void addPacketToWindow (unsigned long ulPacketSize);

			//
			// As time passes, transmit times that were in the future become transmit
			// times that are in the past or present. Call this function to empty
			// those old packets out of the window, freeing up space in the window.
			#ifdef WIN32
				void emptyOutWindow (const __int64 &llCurrentTime);
			#else
				void emptyOutWindow (const long long &llCurrentTime);
			#endif

			//
			// MarkBeginningOfWriteBurst
			// Call this on the first write of a write burst for a client. This
			// allows the overbuffer window to track whether the bitrate of the movie
			// is above the play rate.
			void markBeginningOfWriteBurst (void)
			{
				_usWriteBurstBeginning		= true;
			}

	} RTPOverBufferWindow2_t, *RTPOverBufferWindow2_p;

#endif
