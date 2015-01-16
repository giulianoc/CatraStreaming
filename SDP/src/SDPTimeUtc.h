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

#ifndef SDPTimeUtc_h
	#define SDPTimeUtc_h

	#include "SDPTimeBase.h"


	#define SDP_UTCIDENTIFIER			"clock"


	#define SDP_SDPUTCDATE_LENGTH		8
	#define SDP_SDPUTCTIME_LENGTH		6


	typedef class SDPTimeUtc: public SDPTimeBase

	{

		private:
			unsigned long			_ulYear;
			unsigned long			_ulMonth;
			unsigned long			_ulDay;
			unsigned long			_ulHours;
			unsigned long			_ulMinutes;
			unsigned long			_ulSeconds;

		public:
			SDPTimeUtc ();

			virtual ~SDPTimeUtc();

			/**
				From the rfc of the RTSP:
					utc-range =   "clock" "=" utc-time "-" [ utc-time ]
					utc-time  =   utc-date "T" utc-time "Z"
					utc-date  =   8DIGIT                    ; <YYYYMMDD>
					utc-time  =   6DIGIT [ "." fraction ]   ; <HHMMSS.fraction>

				Example for November 8, 1996 at 14h37 and 20 and
				a quarter seconds 
					UTC: 19961108T143720.25Z
			*/
			Error init (const char *pTimeUtcBuffer);

			Error finish (void);

	} SDPTimeUtc_t, *SDPTimeUtc_p;

#endif

