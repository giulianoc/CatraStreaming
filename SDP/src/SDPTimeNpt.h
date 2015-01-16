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

#ifndef SDPTimeNpt_h
	#define SDPTimeNpt_h

	#include "SDPTimeBase.h"


	#define SDP_NPTIDENTIFIER			"npt"
	#define SDP_NOW						"now"
	#define SDP_NOWLENGTH				3


	typedef class SDPTimeNpt: public SDPTimeBase

	{

		public:
			typedef enum NptType {
				SDP_NPTTYPE_SEC,
				SDP_NPTTYPE_TIME,
				SDP_NPTTYPE_NOW
			} NptType_t, *NptType_p;

		private:

		public:
			SDPTimeNpt (void);

			virtual ~SDPTimeNpt (void);

			/**
				From the rfc of RTSP:
					npt-range = ( npt-time "-" [ npt-time ] ) | ( "-" npt-time )
					npt-time  = "now" | npt-sec | npt-hhmmss
					npt-sec   = 1*DIGIT [ "." *DIGIT ]
					npt-hhmmss = npt-hh ":" npt-mm ":" npt-ss [ "." *DIGIT ]
					npt-hh    = 1*DIGIT     ; any positive number
					npt-mm    = 1*2DIGIT    ; 0-59
					npt-ss    = 1*2DIGIT    ; 0-59

					Examples:
						npt=123.45-125
						npt=12:05:35.3-
						npt=now-
			*/
			Error init (const char *pTimeNptBuffer);

			Error finish (void);

	} SDPTimeNpt_t, *SDPTimeNpt_p;

#endif

