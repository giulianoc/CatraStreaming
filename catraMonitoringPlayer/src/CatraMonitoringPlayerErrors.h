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


#ifndef CatraMonitoringPlayerErrors_h
	#define CatraMonitoringPlayerErrors_h

	#include "Error.h"
	#include <iostream>

	enum CatraMonitoringPlayerErrorsCodes {

		// RTSPClientSession
		MP4PL_RTSPCLIENTSESSION_INIT_FAILED,
		MP4PL_RTSPCLIENTSESSION_FINISH_FAILED,
		MP4PL_RTSPCLIENTSESSION_APPENDTOSTATISTICSFILE_FAILED,
		MP4PL_RTSPCLIENTSESSION_SENDDESCRIBE_FAILED,
		MP4PL_RTSPCLIENTSESSION_SENDSETUP_FAILED,
		MP4PL_RTSPCLIENTSESSION_SENDPLAY_FAILED,
		MP4PL_RTSPCLIENTSESSION_GETRTPACKETS_FAILED,
		MP4PL_RTSPCLIENTSESSION_SENDTEARDOWN_FAILED,
		MP4PL_RTSPCLIENTSESSION_PARSEURL_FAILED,
		MP4PL_RTSPCLIENTSESSION_PARSEDESCRIBERESPONSE_FAILED,
		MP4PL_RTSPCLIENTSESSION_PARSESETUPRESPONSE_FAILED,
		MP4PL_RTSPCLIENTSESSION_PARSEPLAYRESPONSE_FAILED,
		MP4PL_RTSPCLIENTSESSION_PARSETEARDOWNRESPONSE_FAILED,
		MP4PL_RTSPCLIENTSESSION_WRITESTATISTICS_FAILED,
		MP4PL_RTSPCLIENTSESSION_SOCKETIMPLREADLINESFAILED,
		MP4PL_RTSPCLIENTSESSION_RTSPCONNECTIONDOWN,
		MP4PL_RTSPCLIENTSESSION_WRONGRTSPRESPONSE,
		MP4PL_RTSPCLIENTSESSION_SENDTOSERVERFAILED,
		MP4PL_RTSPCLIENTSESSION_RTPPACKETWRONG,
		MP4PL_RTSPCLIENTSESSION_VIDEORTPPACKETNOTRECEIVED,
		MP4PL_RTSPCLIENTSESSION_AUDIORTPPACKETNOTRECEIVED,
		MP4PL_RTSPCLIENTSESSION_ATTEMPTTOCONNECTTIMEDOUT,
		MP4PL_RTSPCLIENTSESSION_CONNECTIONREJECTED,
		MP4PL_RTSPCLIENTSESSION_NORTPPACKETSRECEIVED,
		MP4PL_RTSPCLIENTSESSION_NOVIDEORTPPACKETRECEIVED,
		MP4PL_RTSPCLIENTSESSION_NOAUDIORTPPACKETRECEIVED,
		MP4PL_RTSPCLIENTSESSION_NOMEDIAFOUND,
		MP4PL_RTSPCLIENTSESSION_RECEIVEDRTSPERROR,

		// common
		MP4PL_NEW_FAILED,
		MP4PL_LOCALTIME_R_FAILED,

		// Insert here other errors...

		MP4PL_MAXERRORS
	} ;

	#ifdef WIN32
		// warning C4267: 'argument' : conversion from 'size_t' to 'long', possible loss of data

		#pragma warning (disable : 4267)
	#endif
	// declaration of class error
	dclCustomErrorClass (CatraMonitoringPlayerErrors, MP4PL_MAXERRORS)


#endif
