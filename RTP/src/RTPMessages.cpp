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


#include "RTPMessages.h"


ErrMsgBase:: ErrMsgsInfo RTPMessagesStr = {

	// RTPHintCreator
	RTP_RTPHINTCREATOR_APPENDHINTSAMPLETOMDATATOM,
		"Append hint sample to mdat atom. Hint sample identifier: %lu, hint duration: %llu",

	// RTPHintSample
	RTP_RTPHINTSAMPLE_HINTSAMPLEBYTESNUMBER,
	"Hint sample bytes number. Hint sample identifier: %ld, bytes number: %ld",
	RTP_RTPHINTSAMPLE_HINTSAMPLEPACKETSNUMBER,
"Hint sample packets number. Hint sample identifier: %ld, packets number: %ld",

	// RTPHintPacket
	RTP_RTPHINTPACKET_PACKETDATANUMBER,
		"Hint packet data number. Number: %ld",
	RTP_RTPHINTPACKET_PACKETDATABYTESNUMBER,
		"Hint packet data bytes number. Bytes number: %ld"

	// common

	// Insert here other errors...

} ;

