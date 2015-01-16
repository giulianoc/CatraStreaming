
// Class that tracks packets that are part of the "overbuffer". That is,
// packets that are being sent ahead of time. This class can be used
// to make sure the server isn't overflowing the client's overbuffer size.
// Time is mesured in milliseconds
// The packets in the window are distributed within the buckets according
// them transmission time
// Each element of the buckets contains the sum of the size
// of one or more packets

#ifndef RTPOverBufferWindow_h
	#define RTPOverBufferWindow_h

	#define SS_MAXPLAYRATEALLOWED				2

	#include "Tracer.h"
	#include "StreamingServerErrors.h"
	#include "StreamingServerMessages.h"


	typedef class RTPOverBufferWindow {

		public:
			typedef enum RTPOverBufferWindowStatus {
				ROBW_BUILDED,
				ROBW_INITIALIZED,
			} RTPOverBufferWindowStatus_t, *RTPOverBufferWindowStatus_p;

		private:
			RTPOverBufferWindowStatus_t		_stRTPOverBufferWindowStatus;
			Tracer_p				_ptSystemTracer;
			unsigned long			_ulMaxWindowSizeInBytes;
			unsigned long			_ulCurrentBytesInWindow;
			unsigned long			*_pulBuckets;
			unsigned long			_ulBucketsNumber;
			unsigned long			_ulIntervalBetweenBuckets;
			unsigned long			_ulStartBucketIndex;
			#ifdef WIN32
				__int64				_ullAbsoluteStartTransmitTimeInMilliSecs;
			#else
				unsigned long long	_ullAbsoluteStartTransmitTimeInMilliSecs;
			#endif
			Boolean_t				_bWriteBurstBeginning;
			Boolean_t				_bOverBufferingEnabled;
			unsigned long			_ulStartBucketInThisWriteBurst;

		public:
			RTPOverBufferWindow ();

			~RTPOverBufferWindow ();

			Error init (unsigned long ulIntervalBetweenBuckets,
				unsigned long ulInitialMaxWindowSizeInBytes,
				unsigned long ulMaxSendAheadTimeInSec,
				Tracer_p ptTracer);

			Error finish (void);

			// Puts a packet in its proper place in the window. Make sure that
			// packets added here have transmit times that are in the future.
			// Only packets that are being sent ahead of time are part
			// of the overbuffer window.
			// If the overbuffer window is full, this returns a time
			// in the future when enough space will open up for this packet.
			// Otherwise, returns a time in the past or -1.
			// The overbuffer window is full if the byte count is filled up,
			// or if the bitrate is above the max play rate.
			#ifdef WIN32
				Error addPacketToWindow (
					const __int64 ullAbsolutePacketTransmissionTimeInMilliSecs,
					const __int64 ullAbsoluteCurrentTimeInMilliSecs,
					unsigned long ulPacketSize,
					__int64 *pllSuggestedWakeupTime);
			#else
				Error addPacketToWindow (
					const unsigned long long ullAbsolutePacketTransmissionTimeInMilliSecs,
					const unsigned long long ullAbsoluteCurrentTimeInMilliSecs,
					unsigned long ulPacketSize,
					long long *pllSuggestedWakeupTime);
			#endif

			// As time passes, transmit times that were in the future
			// become transmit times that are in the past or present.
			// Call this function to empty those old packets out of the window,
			// freeing up space in the window.
			#ifdef WIN32
				Error emptyOutWindow (
					const __int64 llAbsoluteCurrentTimeInMilliSecs);
			#else
				Error emptyOutWindow (
					const unsigned long long ullAbsoluteCurrentTimeInMilliSecs);
			#endif

			Error getBytesInWindow (unsigned long *pulBytesInWindow);

			Error getIntervalBetweenBuckets (
				unsigned long *pulIntervalBetweenBuckets);

			Error getAvailableSpaceInWindow (long *plCurrentBytesInWindow);

			Error setWindowSizeInByte (unsigned long ulWindowSizeInBytes);

			Error turnOffOverbuffering (void);

			Error TurnOnOverbuffering (void);

			// markBeginningOfWriteBurst
			// Call this on the first write of a write burst for a client. This
			// allows the overbuffer window to track whether
			// the bitrate of the movie is above the play rate.
			Error markBeginningOfWriteBurst (void);

	} RTPOverBufferWindow_t, *RTPOverBufferWindow_p;

#endif

