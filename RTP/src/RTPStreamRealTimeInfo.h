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


#ifndef RTPStreamRealTimeInfo_h
	#define RTPStreamRealTimeInfo_h


	typedef class RTPStreamRealTimeInfo
	{
		public:
			struct getChunkInfoFromSampleNumber_t
			{
				unsigned long			_ulSampleNumber_IN;

				unsigned long			_ulSamplesPerChunk_OUT;
				unsigned long			_ulChunkNumber_OUT;
				unsigned long			_ulSampleDescriptionIndex_OUT;
				unsigned long			_ulSampleOffsetInChunk_OUT;

				unsigned long			_ulEntryIndex_Internal;
				unsigned long			_ulCurrentSample_Internal;
				unsigned long			_ulLastFirstChunk_Internal;
				unsigned long			_ulLastSamplesPerChunk_Internal;
				unsigned long			_ulLastSampleDescriptionIndex_Internal;
			} getChunkInfoFromSampleNumber;

			struct getSampleTimeFromSampleNumber_t
			{
				unsigned long			_ulSampleIdentifier_IN;

				#ifdef WIN32
					__int64					_ullSampleStartTime_OUT;
					__int64					_ullSampleDuration_OUT;
				#else
					unsigned long long		_ullSampleStartTime_OUT;
					unsigned long long		_ullSampleDuration_OUT;
				#endif

				unsigned long			_ulEntryIndex_Internal;
				unsigned long			_ulCurrentSampleIdentifier_Internal;
				unsigned long			_ulCurrentSampleStartTime_Internal;
			} getSampleTimeFromSampleNumber;

			struct getSampleNumberFromSampleTime_t
			{
				#ifdef WIN32
					__int64				_ullSampleTime_IN;
				#else
					unsigned long long	_ullSampleTime_IN;
				#endif

				unsigned long			_ulSampleNumber_OUT;

				unsigned long			_ulEntryIndex_Internal;
				unsigned long			_ulCurrentSampleIdentifier_Internal;
				#ifdef WIN32
					__int64					_ullCurrentSampleStartTime_Internal;
				#else
					unsigned long long		_ullCurrentSampleStartTime_Internal;
				#endif
			} getSampleNumberFromSampleTime;

			struct getSampleRangeSizeFromFirstAndLastSample_t
			{
				unsigned long			ulFirstSampleNumber_IN;
				unsigned long			ulLastSampleNumber_IN;

				unsigned long			ulSampleRangeSize_OUT;
			} getSampleRangeSizeFromFirstAndLastSample;

			RTPStreamRealTimeInfo (void)
			{
				resetGetChunkInfoFromSampleNumber ();
				resetGetSampleTimeFromSampleNumber ();
				resetGetSampleNumberFromSampleTime ();
				resetGetSampleRangeSizeFromFirstAndLastSample (
					(unsigned long) -1);
			} ;

			void resetGetChunkInfoFromSampleNumber (void)
			{
				getChunkInfoFromSampleNumber. _ulSampleNumber_IN		=
					(unsigned long) -1;

				// internal
				getChunkInfoFromSampleNumber. _ulEntryIndex_Internal	=
					0;
				getChunkInfoFromSampleNumber. _ulCurrentSample_Internal	=
					1;
				getChunkInfoFromSampleNumber. _ulLastFirstChunk_Internal		=
					1;
				getChunkInfoFromSampleNumber. _ulLastSamplesPerChunk_Internal	=
					1;
				getChunkInfoFromSampleNumber. _ulLastSampleDescriptionIndex_Internal	=
					0;
			} ;

			void resetGetSampleTimeFromSampleNumber (void)
			{
				getSampleTimeFromSampleNumber. _ulSampleIdentifier_IN		=
					(unsigned long) -1;

				// internal
				getSampleTimeFromSampleNumber. _ulEntryIndex_Internal	=
					0;
				getSampleTimeFromSampleNumber. _ulCurrentSampleIdentifier_Internal	=
					1;
				getSampleTimeFromSampleNumber. _ulCurrentSampleStartTime_Internal	=
					0;
			} ;

			void resetGetSampleNumberFromSampleTime (void)
			{
				#ifdef WIN32
					getSampleNumberFromSampleTime. _ullSampleTime_IN		=
						(__int64) -1;
				#else
					getSampleNumberFromSampleTime. _ullSampleTime_IN		=
						(unsigned long long) -1;
				#endif

				// internal
				getSampleNumberFromSampleTime. _ulEntryIndex_Internal	=
					0;
				getSampleNumberFromSampleTime. _ulCurrentSampleIdentifier_Internal	=
					1;
				getSampleNumberFromSampleTime. _ullCurrentSampleStartTime_Internal	=
					0;
			} ;

			void resetGetSampleRangeSizeFromFirstAndLastSample (
				unsigned long ulLastSampleNumber)
			{
				getSampleRangeSizeFromFirstAndLastSample.
					ulFirstSampleNumber_IN			= (unsigned long) -1;

				getSampleRangeSizeFromFirstAndLastSample.
					ulLastSampleNumber_IN			= ulLastSampleNumber;


				getSampleRangeSizeFromFirstAndLastSample.
					ulSampleRangeSize_OUT			= 0;
			} ;

	} RTPStreamRealTimeInfo_t, *RTPStreamRealTimeInfo_p;

#endif
