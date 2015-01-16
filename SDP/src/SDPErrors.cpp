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


#include "SDPErrors.h"



ErrMsgBase:: ErrMsgsInfo SDPErrorsStr = {

	// SDP
	SDP_SDP_INIT_FAILED,
		"The init method of the class SDP failed",
	SDP_SDP_FINISH_FAILED,
		"The finish method of the class SDP failed",
	SDP_SDP_APPENDTOBUFFER_FAILED,
		"The appendToBuffer method of the class SDP failed",
	SDP_SDP_ADDATTRIBUTE_FAILED,
		"The addAttribute method of the class SDP failed",
	SDP_SDP_GETATTRIBUTEVALUE_FAILED,
		"The getAttributeValue method of the class SDP failed",
	SDP_SDP_SETVERSION_FAILED,
		"The setVersion method of the class SDP failed",
	SDP_SDP_SETORIGINUSERNAME_FAILED,
		"The setOriginUserName method of the class SDP failed",
	SDP_SDP_SETORIGINSESSIONID_FAILED,
		"The setOriginSessionId method of the class SDP failed",
	SDP_SDP_SETORIGINVERSION_FAILED,
		"The setOriginVersion method of the class SDP failed",
	SDP_SDP_SETORIGINNETWORKTYPE_FAILED,
		"The setOriginNetworkType method of the class SDP failed",
	SDP_SDP_SETORIGINADDRESSTYPE_FAILED,
		"The setOriginAddressType method of the class SDP failed",
	SDP_SDP_SETORIGINADDRESS_FAILED,
		"The setOriginAddress method of the class SDP failed",
	SDP_SDP_SETSESSIONNAME_FAILED,
		"The setSessionName method of the class SDP failed",
	SDP_SDP_SETINFORMATION_FAILED,
		"The setInformation method of the class SDP failed",
	SDP_SDP_SETURI_FAILED,
		"The setURI method of the class SDP failed",
	SDP_SDP_SETEMAILADDRESS_FAILED,
		"The setEmailAddress method of the class SDP failed",
	SDP_SDP_SETPHONENUMBER_FAILED,
		"The setPhoneNumber method of the class SDP failed",
	SDP_SDP_SETCONNECTIONNETWORKTYPE_FAILED,
		"The setConnectionNetworkType method of the class SDP failed",
	SDP_SDP_SETCONNECTIONADDRESSTYPE_FAILED,
		"The setConnectionAddressType method of the class SDP failed",
	SDP_SDP_SETCONNECTIONADDRESS_FAILED,
		"The setConnectionAddress method of the class SDP failed",
	SDP_SDP_SETBANDWIDTHMODIFIER_FAILED,
		"The setBandwidthModifier method of the class SDP failed",
	SDP_SDP_SETBANDWIDTHVALUEINKBPS_FAILED,
		"The setBandwidthValueInKbps method of the class SDP failed",
	SDP_SDP_SETTIMESSTART_FAILED,
		"The setTimesStart method of the class SDP failed",
	SDP_SDP_SETTIMESSTOP_FAILED,
		"The setTimesStop method of the class SDP failed",
	SDP_SDP_CREATEUNKNOWNSDPATTRIBUTE_FAILED,
		"The createUnknownSDPAttribute method of the class SDP failed",
	SDP_SDP_BUILDSDPMEDIA_FAILED,
		"The buildSDPMedia method of the class SDP failed",
	SDP_SDP_WRONGSTATEFORTHISLINE,
		"Wrong state for the line: %s",
	SDP_SDP_FIELDTOOLONG,
		"SDP field too long: %s",
	SDP_SDP_WRONGMEDIATYPE,
		"Wrong media type: %s",
	SDP_SDP_SDPTYPENOTHANDLED,
		"SDP type not handled: %s",
	SDP_SDP_SDPMEDIANOTFOUND,
		"SDPMedia not found",
	SDP_SDP_ATTRIBUTEVALUEWRONG,
		"The '%s' attribute has a wrong value ('%s')",
	SDP_SDP_ATTRIBUTENOTFOUND,
		"The '%s' attribute was not found",

	// SDPForRTSP
	SDP_SDPFORRTSP_GETRANGEATTRIBUTEINFO_FAILED,
		"The getRangeAttributeInfo method of the class SDPForRTSP failed",
	SDP_SDPFORRTSP_INIT_FAILED,
		"The init method of the class SDPForRTSP failed",
	SDP_SDPFORRTSP_FINISH_FAILED,
		"The finish method of the class SDPForRTSP failed",
	SDP_SDPFORRTSP_APPENDTOBUFFER_FAILED,
		"The appendToBuffer method of the class SDPForRTSP failed",
	SDP_SDPFORRTSP_GETSDPMEDIA_FAILED,
		"The getSDPMedia method of the class SDPForRTSP failed",

	// SDPFor3GPP
	SDP_SDPFOR3GPP_INIT_FAILED,
		"The init method of the class SDPFor3GPP failed",
	SDP_SDPFOR3GPP_FINISH_FAILED,
		"The finish method of the class SDPFor3GPP failed",
	SDP_SDPFOR3GPP_GETATTRIBUTEVALUE_FAILED,
	"The getAttributeValue method of the class SDPFor3GPP failed. Attribute name: '%s'",
	SDP_SDPFOR3GPP_APPENDTOBUFFER_FAILED,
		"The appendToBuffer method of the class SDPFor3GPP failed",
	SDP_SDPFOR3GPP_GETSDPMEDIA_FAILED,
		"The getSDPMedia method of the class SDPFor3GPP failed",
	SDP_SDPFOR3GPP_ADDSDPMEDIA_FAILED,
		"The addSDPMedia method of the class SDPFor3GPP failed",

	// SDPMedia
	SDP_SDPMEDIA_INIT_FAILED,
		"The init method of the class SDPMedia failed",
	SDP_SDPMEDIA_FINISH_FAILED,
		"The finish method of the class SDPMedia failed",
	SDP_SDPMEDIA_APPENDTOBUFFER_FAILED,
		"The appendToBuffer method of the class SDPMedia failed",
	SDP_SDPMEDIA_GETMEDIAINFO_FAILED,
		"The getMediaInfo method of the class SDPMedia failed",
	SDP_SDPMEDIA_GETMEDIATYPE_FAILED,
		"The getMediaType method of the class SDPMedia failed",
	SDP_SDPMEDIA_SETMEDIATYPE_FAILED,
		"The setMediaType method of the class SDPMedia failed",
	SDP_SDP_ADDSDPMEDIA_FAILED,
		"The addSDPMedia method of the class SDPMedia failed",
	SDP_SDPMEDIA_SETPORT_FAILED,
		"The setPort method of the class SDPMedia failed",
	SDP_SDPMEDIA_SETTRANSPORT_FAILED,
		"The setTransport method of the class SDPMedia failed",
	SDP_SDPMEDIA_GETFMTLIST_FAILED,
		"The getFmtlist method of the class SDPMedia failed",
	SDP_SDPMEDIA_SETFMTLIST_FAILED,
		"The setFmtlist method of the class SDPMedia failed",
	SDP_SDPMEDIA_SETINFORMATION_FAILED,
		"The setInformation method of the class SDPMedia failed",
	SDP_SDPMEDIA_SETCONNECTIONNETWORKTYPE_FAILED,
		"The setConnectionNetworkType method of the class SDPMedia failed",
	SDP_SDPMEDIA_SETCONNECTIONADDRESSTYPE_FAILED,
		"The setConnectionAddressType method of the class SDPMedia failed",
	SDP_SDPMEDIA_SETCONNECTIONADDRESS_FAILED,
		"The setConnectionAddress method of the class SDPMedia failed",
	SDP_SDPMEDIA_SETBANDWIDTHMODIFIER_FAILED,
		"The setBandwidthModifier method of the class SDPMedia failed",
	SDP_SDPMEDIA_SETBANDWIDTHVALUEINKBPS_FAILED,
		"The setBandwidthValueInKbps method of the class SDPMedia failed",
	SDP_SDPMEDIA_ADDATTRIBUTE_FAILED,
		"The addAttribute method of the class SDPMedia failed",
	SDP_SDPMEDIA_TRANSPORTTOOLONG,
		"Transport too long",
	SDP_SDPMEDIA_FMTLISTTOOLONG,
		"FmtList too long",
	SDP_SDPMEDIA_INFORMATIONTOOLONG,
		"Information too long",
	SDP_SDPMEDIA_CONNECTIONNETWORKTYPETOOLONG,
		"Connection network type too long",
	SDP_SDPMEDIA_CONNECTIONADDRESSTYPETOOLONG,
		"Connection address type too long",
	SDP_SDPMEDIA_CONNECTIONADDRESSTOOLONG,
		"Connection address too long",
	SDP_SDPMEDIA_BANDWIDTHMODIFIERTOOLONG,
		"Bandwidth modifier too long",
	SDP_SDPMEDIA_ATTRIBUTENOTFOUND,
		"The mandatory '%s' attribute was not found inside the SDP file",

	// SDPMediaFor3GPP
	SDP_SDPMEDIAFOR3GPP_APPENDTOBUFFER_FAILED,
		"The appendToBuffer method of the class SDPMediaFor3GPP failed",
	SDP_SDPMEDIAFOR3GPP_GETSDPMEDIAVIDEOFROMMEDIAINFO_FAILED,
		"The getSDPMediaVideoFromMediaInfo method of the class SDPMediaFor3GPP failed",
	SDP_SDPMEDIAFOR3GPP_GETSDPMEDIAAUDIOFROMMEDIAINFO_FAILED,
		"The getSDPMediaAudioFromMediaInfo method of the class SDPMediaFor3GPP failed",

	// SDPAttribute
	SDP_SDPATTRIBUTE_APPENDTOBUFFER_FAILED,
		"The appendToBuffer method of the class SDPAttribute failed",

	// SDPTimeBase
	SDP_SDPTIMEBASE_INIT_FAILED,
		"The init method of the class SDPTimeBase failed",
	SDP_SDPTIMEBASE_FINISH_FAILED,
		"The finish method of the class SDPTimeBase failed",
	SDP_SDPTIMEBASE_GETTIMEASNPT_FAILED,
		"The getTimeAsNpt method of the class SDPTimeBase failed",

	// SDPTimeNpt
	SDP_SDPTIMENPT_INIT_FAILED,
		"The init method of the class SDPTimeNpt failed",
	SDP_SDPTIMENPT_FINISH_FAILED,
		"The finish method of the class SDPTimeNpt failed",

	// SDPTimeSmpte
	SDP_SDPTIMESMPTE_INIT_FAILED,
		"The init method of the class SDPTimeSmpte failed",
	SDP_SDPTIMESMPTE_FINISH_FAILED,
		"The finish method of the class SDPTimeSmpte failed",

	// SDPTimeUtc
	SDP_SDPTIMEUTC_INIT_FAILED,
		"The init method of the class SDPTimeUtc failed",
	SDP_SDPTIMEUTC_FINISH_FAILED,
		"The finish method of the class SDPTimeUtc failed",
	SDP_SDPTIMEUTC_WRONGUTCBUFFER,
		"Wrong Utc buffer: %s",

	// common
	SDP_ACTIVATIONWRONG,
		"Activation failed",
	SDP_NEW_FAILED,
		"The new method failed",
	SDP_SSCANF_FAILED,
		"The sscanf failed"
	// Insert here other errors...

} ;

