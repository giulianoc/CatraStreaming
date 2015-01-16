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


#ifndef RTPMessages_h
	#define RTPMessages_h

	#include "Error.h"
	#include <iostream>


	enum RTPMessagesCodes {

		// RTPHintCreator
		RTP_RTPHINTCREATOR_APPENDHINTSAMPLETOMDATATOM,

		// RTPHintSample
		RTP_RTPHINTSAMPLE_HINTSAMPLEBYTESNUMBER,
		RTP_RTPHINTSAMPLE_HINTSAMPLEPACKETSNUMBER,

		// RTPHintPacket
		RTP_RTPHINTPACKET_PACKETDATANUMBER,
		RTP_RTPHINTPACKET_PACKETDATABYTESNUMBER,

		// common

		// Insert here other errors...

		RTP_MAXMESSAGES
	} ;

	#ifdef WIN32
		// warning C4267: 'argument' : conversion from 'size_t' to 'long', possible loss of data

		#pragma warning (disable : 4267)
	#endif

	// declaration of class error
	dclCustomMessageClass (RTPMessages, RTP_MAXMESSAGES)


#endif

