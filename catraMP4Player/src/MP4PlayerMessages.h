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


#ifndef MP4PlayerMessages_h
	#define MP4PlayerMessages_h

	#include "Error.h"
	#include <iostream>


	//
	// Click <a href="MP4FileMessages.C#MP4FileMessages" target=classContent>here</a> for the errors strings.
	//
	enum MP4PlayerMessagesCodes {

		// MP4Player
		MP4PL_WAITFOR,
		MP4PL_WAITINGTHEFINISHING,

		// RTSPClientSession
		MP4PL_RTSPCLIENTSESSION_STARTED,
		MP4PL_RTSPCLIENTSESSION_SENTTOSERVER,
		MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
		MP4PL_RTSPCLIENTSESSION_RECEIVEDRTCPMESSAGE,
		MP4PL_RTSPCLIENTSESSION_RECEIVEDRTPMESSAGE,
		MP4PL_RTSPCLIENTSESSIONFINISHED,
		MP4PL_RTSPCLIENTSESSION_TIMEOFDESCRIBERESPONSE,
		MP4PL_RTSPCLIENTSESSION_TIMEBETWEENDESCRIBEANDFIRSTPACKET,
		MP4PL_RTSPCLIENTSESSION_VIDEOBITSTREAMINFO,
		MP4PL_RTSPCLIENTSESSION_AUDIOBITSTREAMINFO,
		MP4PL_RTSPCLIENTSESSION_STARTOTREADINGPACKETSFROMSERVER,

		// common

		// Insert here other errors...

		MP4PL_MAXMESSAGES
	} ;

	#ifdef WIN32
		// warning C4267: 'argument' : conversion from 'size_t' to 'long', possible loss of data

		#pragma warning (disable : 4267)
	#endif
	// declaration of class error
	dclCustomMessageClass (MP4PlayerMessages, MP4PL_MAXMESSAGES)


#endif
