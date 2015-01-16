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


#include "CatraMonitoringPlayerMessages.h"


ErrMsgBase:: ErrMsgsInfo CatraMonitoringPlayerMessagesStr = {

	// MP4Player
	MP4PL_WAITFOR,
		"Wait for %lu seconds and %lu micro-seconds",

	// RTSPClientSession
	MP4PL_RTSPCLIENTSESSION_INIT,
		"RTSP client session for %s",
	MP4PL_RTSPCLIENTSESSION_STARTED,
		"RTSP client session started",
	MP4PL_RTSPCLIENTSESSION_SENTTOSERVER,
		"Message sent: '%s'",
	MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
		"Response received: '%s'",
	MP4PL_RTSPCLIENTSESSION_RECEIVEDRTCPMESSAGE,
		"Received %s RTCP packet (length: %ld)",
	MP4PL_RTSPCLIENTSESSION_RECEIVEDRTPMESSAGE,
		"Received %s RTP packet (length: %ld, Sequence number: %ld)",
	MP4PL_RTSPCLIENTSESSIONFINISHED,
		"Finished",
	MP4PL_RTSPCLIENTSESSION_TIMEOFDESCRIBERESPONSE,
		"Time of DESCRIBE response: %lu milli-seconds",
	MP4PL_RTSPCLIENTSESSION_TIMEBETWEENDESCRIBEANDFIRSTPACKET,
		"Time between DESCRIBE request and first packet received: %lld milli-seconds",
	MP4PL_RTSPCLIENTSESSION_VIDEOBITSTREAMINFO,
		"Video bitstream info. RTP packets number: %lu, RTP packets number received: %lu, start sequence number: %lu, last sequence number: %lu, bitstream size: %lu",
	MP4PL_RTSPCLIENTSESSION_AUDIOBITSTREAMINFO,
		"Audio bitstream info. RTP packets number: %lu, RTP packets number received: %lu, start sequence number: %lu, last sequence number: %lu, bitstream size: %lu",
	MP4PL_RTSPCLIENTSESSION_STARTOTREADINGPACKETSFROMSERVER,
		"Start to listen the first RTP packets from the streaming server..."

	// common

	// Insert here other errors...

} ;

