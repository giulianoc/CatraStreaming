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


#include "MP4PlayerErrors.h"


ErrMsgBase:: ErrMsgsInfo MP4PlayerErrorsStr = {

	// RTSPClientSession
	MP4PL_RTSPCLIENTSESSION_INIT_FAILED,
		"The init method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_FINISH_FAILED,
		"The finish method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_INITIALRTSPHANDSHAKE_FAILED,
		"The initialRTSPHandshake method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_FINALRTSPHANDSHAKE_FAILED,
		"The finalRTSPHandshake method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_GETRTSPSESSIONIDENTIFIER_FAILED,
		"The getRTSPSessionIdentifier method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_GETSTATISTICSINFORMATION_FAILED,
		"The getStatisticsInformation method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_PROCESSRECEIVINGVIDEORTPPACKET_FAILED,
		"The processReceivingVideoRTPPacket method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_PROCESSRECEIVINGVIDEORTCPPACKET_FAILED,
		"The processReceivingVideoRTCPPacket method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_PROCESSRECEIVINGAUDIORTPPACKET_FAILED,
		"The processReceivingAudioRTPPacket method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_PROCESSRECEIVINGAUDIORTCPPACKET_FAILED,
		"The processReceivingAudioRTCPPacket method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_SAVEBITSTREAMS_FAILED,
		"The saveBitStreams method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_APPENDTOSTATISTICSFILE_FAILED,
	"The appendToStatisticsFile method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_SENDDESCRIBE_FAILED,
		"The sendDescribe method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_SENDSETUP_FAILED,
		"The sendSetup method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_SENDPLAY_FAILED,
		"The sendPlay method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_GETRTPACKETS_FAILED,
		"The getRTPackets method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_SENDTEARDOWN_FAILED,
		"The sendTeardown method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_PARSEURL_FAILED,
		"RTSP client session n. %lu. The parseURL method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_PARSEDESCRIBERESPONSE_FAILED,
		"RTSP client session n. %lu. The parseDescribeResponse method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_PARSESETUPRESPONSE_FAILED,
		"RTSP client session n. %lu. The parseSetupResponse method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_PARSEPLAYRESPONSE_FAILED,
		"RTSP client session n. %lu. The parsePlayResponse method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_PARSETEARDOWNRESPONSE_FAILED,
		"RTSP client session n. %lu. The parseTeardownResponse method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED,
		"The writeStatistics method of the RTSPClientSession class failed",
	MP4PL_RTSPCLIENTSESSION_SOCKETIMPLREADLINESFAILED,
		"RTSP client session n. %lu. SocketImpl:: readLines failed.",
	MP4PL_RTSPCLIENTSESSION_RTSPCONNECTIONDOWN,
		"RTSP client session n. %lu. Lose the RTSP connection",
	MP4PL_RTSPCLIENTSESSION_WRONGRTSPRESPONSE,
		"RTSP client session n. %lu. Received wrong RTSP response: '%s'",
	MP4PL_RTSPCLIENTSESSION_SENDTOSERVERFAILED,
		"RTSP client session n. %lu. Failed to send the RTSP message: '%s'",
	MP4PL_RTSPCLIENTSESSION_RTPPACKETWRONG,
		"Wrong RTP packet",
	MP4PL_RTSPCLIENTSESSION_VIDEORTPPACKETNOTRECEIVED,
		"Video RTP packet not received (Sequence number: %lu)",
	MP4PL_RTSPCLIENTSESSION_AUDIORTPPACKETNOTRECEIVED,
		"Audio RTP packet not received (Sequence number: %lu)",
	MP4PL_RTSPCLIENTSESSION_ATTEMPTTOCONNECTTIMEDOUT,
		"RTSP client session n. %lu. Attempt to connect timed out without establishing a connection",
	MP4PL_RTSPCLIENTSESSION_CONNECTIONREJECTED,
		"RTSP client session n. %lu. The attempt to connect was forcefully rejected",
	MP4PL_RTSPCLIENTSESSION_NOMEDIAFOUND,
		"No media found",
	MP4PL_RTSPCLIENTSESSION_RECEIVEDERRORFROMSERVER,
		"Received an error from the streaming server. Server response: %s",
	MP4PL_RTSPCLIENTSESSION_READHTTPRESPONSEFAILED,
		"RTSP client session n. %lu. HttpGetThread:: readHttpResponse failed",

	// MP4PlayerSocketsPool
	MP4PL_MP4PLAYERSOCKETSPOOL_EXCEPTIONONSOCKET,
		"RTSP client session n. %lu. Exception of the %ld socket type",

	// common
	MP4PL_NEW_FAILED,
		"new failed",
	MP4PL_LOCALTIME_R_FAILED,
		"localtime_r failed"

	// Insert here other errors...

} ;

