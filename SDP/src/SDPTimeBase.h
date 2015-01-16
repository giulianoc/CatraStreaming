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

#ifndef SDPTimeBase_h 
	#define SDPTimeBase_h

	#include "SDPErrors.h"


	#define			SDP_MAXLONGLENGTH			64 + 1


	typedef class SDPTimeBase

	{

		public:
			typedef enum TimeType
			{
				SDP_TIMETYPE_SMPTE,
				SDP_TIMETYPE_UTC,
				SDP_TIMETYPE_NPT
			} TimeType_t, *TimeType_p;

		private:
			TimeType_t		_ttTimeType;

		protected:
			Boolean_t		_bIsNow;
			unsigned long	_ulNptTimeInSeconds;
			unsigned long	_ulFraction;

		public:
			SDPTimeBase (void);

			virtual ~SDPTimeBase (void);

			Error init (TimeType_t ttTimeType);

			Error finish (void);

			const Error getTimeType (TimeType_p pttTimeType) const;

			virtual Error getTimeAsNpt (
				double *pdNptTime, Boolean_p pbIsNow) const;

		} SDPTimeBase_t, *SDPTimeBase_p;

#endif

