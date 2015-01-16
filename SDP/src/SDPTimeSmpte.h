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

#ifndef SDPTimeSmpte_h
	#define SDPTimeSmpte_h

	#include "SDPTimeBase.h"


    #define SDP_SMPTEIDENTIFIER			"smpte"


	typedef class SDPTimeSmpte: public SDPTimeBase

	{

		private:
			unsigned long			_ulHours;
			unsigned long			_ulMinutes;
			unsigned long			_ulSeconds;
			unsigned long			_ulFrames;
			unsigned long			_ulSubFrames;

		public:
			SDPTimeSmpte (void);

			virtual ~SDPTimeSmpte (void);

			/**
				Permissible inputs may look like one of these
					<hh>:<mm>:<ss>:<FF>.<ff>
					<hh>:<mm>:<ss>:<FF>
			*/
			Error init (const char *pTimeSmpteBuffer);

			Error init (
				unsigned long ulHours,
				unsigned long ulMinutes,
				unsigned long ulSeconds,
				unsigned long ulFrames,
				unsigned long ulSubFrames);

			Error finish (void);

	} SDPTimeSmpte_t, *SDPTimeSmpte_p;

#endif

