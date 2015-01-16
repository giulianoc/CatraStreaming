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


#include "RTSPErrors.h"



ErrMsgBase:: ErrMsgsInfo RTSPErrorsStr = {

	// RTSPUtility
	RTSP_RTSPUTILITY_GETDESCRIBEREQUEST_FAILED,
		"The method getDESCRIBERequest of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_PARSEDESCRIBEREQUEST_FAILED,
		"The method parseDESCRIBERequest of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_GETSDPFROMMEDIAINFO_FAILED,
		"The method getSDPFromMediaInfo of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_GETMEDIAINFOFROMSDPFILE_FAILED,
		"The method getMediaInfoFromSDPFile of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_GETDESCRIBERESPONSE_FAILED,
		"The method getDESCRIBEResponse of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_PARSEDESCRIBERESPONSE_FAILED,
		"The method parseDESCRIBEResponse of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_GETSETUPREQUEST_FAILED,
		"The method getSETUPRequest of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_PARSESETUPREQUEST_FAILED,
		"The method parseSETUPRequest of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_GETSETUPRESPONSE_FAILED,
		"The method getSETUPResponse of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_PARSESETUPRESPONSE_FAILED,
		"The method parseSETUPResponse of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_GETPLAYREQUEST_FAILED,
		"The method getPLAYRequest of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_PARSEPLAYREQUEST_FAILED,
		"The method parsePLAYRequest of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_GETPLAYRESPONSE_FAILED,
		"The method getPLAYResponse of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_PARSEPLAYRESPONSE_FAILED,
		"The method parsePLAYResponse of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_PARSEPAUSEREQUEST_FAILED,
		"The method parsePAUSERequest of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_GETPAUSERESPONSE_FAILED,
		"The method getPAUSEResponse of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_PARSETEARDOWNREQUEST_FAILED,
		"The method parseTEARDOWNRequest of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_GETTEARDOWNRESPONSE_FAILED,
		"The method getTEARDOWNResponse of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_PARSETEARDOWNRESPONSE_FAILED,
		"The method parseTEARDOWNResponse of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_GETOPTIONSREQUEST_FAILED,
		"The method getOPTIONSRequest of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_PARSEOPTIONSREQUEST_FAILED,
		"The method parseOPTIONSRequest of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_GETOPTIONSRESPONSE_FAILED,
		"The method getOPTIONSResponse of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_GETDATESTR_FAILED,
		"The method getDateStr of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_GETRTSPRESPONSE_FAILED,
		"The method getRTSPResponse of the RTSPUtility class failed",
	RTSP_RTSPUTILITY_DESCRIBEREQUESTWRONG,
		"Wrong DESCRIBE request: %s",
	RTSP_RTSPUTILITY_DESCRIBERESPONSEWRONG,
		"Wrong DESCRIBE response: %s",
	RTSP_RTSPUTILITY_SETUPREQUESTWRONG,
		"Wrong SETUP request: %s",
	RTSP_RTSPUTILITY_PLAYREQUESTWRONG,
		"Wrong PLAY request: %s",
	RTSP_RTSPUTILITY_PAUSEREQUESTWRONG,
		"Wrong PAUSE request: %s",
	RTSP_RTSPUTILITY_TEARDOWNREQUESTWRONG,
		"Wrong TEARDOWN request: %s",
	RTSP_RTSPUTILITY_OPTIONSREQUESTWRONG,
		"Wrong OPTIONS request: %s",
	RTSP_RTSPUTILITY_RTSPSTATUSCODEWRONG,
		"RTSP status code wrong (status code: %lu)",
	RTSP_RTSPUTILITY_TRANSPORTNOTCOMPLIANT,
		"Transport not compliant (Transport: %s)",
	RTSP_RTSPUTILITY_SETUPTRANSPORTWRONG,
		"Transport wrong in SETUP method. URI: %s",
	RTSP_RTSPUTILITY_WRONGRTPINFO,
		"Wrong RTP-Info in PLAY method. URI: %s",


	// common
	RTSP_NEW_FAILED,
		"The new failed"
	// Insert here other errors...

} ;

