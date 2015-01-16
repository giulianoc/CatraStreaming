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


#include "MP4PlayerMessages.h"


ErrMsgBase:: ErrMsgsInfo MP4PlayerMessagesStr = {

	// MP4Player
	MP4PL_WAITFOR,
		"Wait for %lu seconds and %lu micro-seconds",
	MP4PL_WAITINGTHEFINISHING,
		"Waiting the finishing",

	// RTSPClientSession
	MP4PL_RTSPCLIENTSESSION_STARTED,
		"RTSP client session n. %lu started",
	MP4PL_RTSPCLIENTSESSION_SENTTOSERVER,
		"RTSP client session n. %lu. Message sent: '%s'",
	MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPRESPONSE,
		"RTSP client session n. %lu. Response received: '%s%s'",
	MP4PL_RTSPCLIENTSESSION_RECEIVEDRTCPMESSAGE,
		"RTSP client session n. %lu. Received %s RTCP packet (length: %lu)",
	MP4PL_RTSPCLIENTSESSION_RECEIVEDRTPMESSAGE,
		"RTSP client session n. %lu. Received %s RTP packet (length: %lu, Sequence number: %ld)",
	MP4PL_RTSPCLIENTSESSIONFINISHED,
		"RTSP client session n. %lu. Finished",
	MP4PL_RTSPCLIENTSESSION_TIMEOFDESCRIBERESPONSE,
		"RTSP client session n. %lu. Time of DESCRIBE response: %lu milli-seconds",
	MP4PL_RTSPCLIENTSESSION_TIMEBETWEENDESCRIBEANDFIRSTPACKET,
		"RTSP client session n. %lu. Time between DESCRIBE request and first packet received: %lld milli-seconds",
	MP4PL_RTSPCLIENTSESSION_VIDEOBITSTREAMINFO,
		"Video bitstream info. RTP packets number: %lu, RTP packets number received: %lu, start sequence number: %lu, last sequence number: %lu, bitstream size: %lu",
	MP4PL_RTSPCLIENTSESSION_AUDIOBITSTREAMINFO,
		"Audio bitstream info. RTP packets number: %lu, RTP packets number received: %lu, start sequence number: %lu, last sequence number: %lu, bitstream size: %lu",
	MP4PL_RTSPCLIENTSESSION_STARTOTREADINGPACKETSFROMSERVER,
		"RTSP client session n. %lu. Start to reading RTP/RTCP packets from the streaming server..."

	// common

	// Insert here other errors...

} ;

