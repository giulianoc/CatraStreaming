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


#include "RTPErrors.h"



ErrMsgBase:: ErrMsgsInfo RTPErrorsStr = {

	// rfc...
	RTP_RFC_RFCISMAHINTER_FAILED,
		"The rfcIsmaHinter function failed",
	RTP_RFC_RFCISMAFRAGMENTER_FAILED,
		"The rfcIsmaFragmenter function failed",
	RTP_RFC_RFC3016VISUALHINTER_FAILED,
		"The rfc3016VisualHinter function failed",
	RTP_RFC_RFC3016VISUALPREFETCHER_FAILED,
		"The rfc3016VisualPrefetcher function failed",
	RTP_RFC_RFC3016AUDIOHINTER_FAILED,
		"The rfc3016AudioHinter function failed",
	RTP_RFC_RFC2429VISUALHINTER_FAILED,
		"The rfc2429VisualHinter function failed",
	RTP_RFC_RFC2429VISUALPREFETCHER_FAILED,
		"The rfc2429VisualPrefetcher function failed",
	RTP_RFC_RFC3267AUDIOHINTER_FAILED,
		"The rfc3267AudioHinter function failed",
	RTP_RFC_AUDIOINTERLEAVEHINTER_FAILED,
		"The audioInterleaveHinter function failed",
	RTP_RFC_RFC3016VISUALPREFETCHER_INSUFFICIENTRTPPACKETSNUMBER,
		"Insufficient RTP packets number. Free RTP packets number: %lu, current media sample size: %lu, max payload size: %lu",
	RTP_RFC_RFC3016AUDIOPREFETCHER_INSUFFICIENTRTPPACKETSNUMBER,
		"Insufficient RTP packets number. Free RTP packets number: %lu, current media sample size: %lu, max payload size: %lu",
	RTP_RFC_RFC2429VISUALPREFETCHER_INSUFFICIENTRTPPACKETSNUMBER,
		"Insufficient RTP packets number. Free RTP packets number: %lu, current media sample size: %lu, max payload size: %lu",
	RTP_RFC_RTPPACKETNOTAVAILABLE,
		"RTP packet not available",
	RTP_RFC_NOMEDIASAMPLES,
		"No media samples",
	RTP_RFC_NONAACAUDIOTYPE,
		"Can't hint non-AAC audio type",
	RTP_RFC_INVALIDDURATION,
		"Invalid duration",
	RTP_RFC_TOOAMRFRAMESNUMBERINONEMDIASAMPLE,
		"Too AMR frames number (%lu) in one media sample",


	// RTPHintCreator
	RTP_RTPHINTCREATOR_INIT_FAILED,
		"The init method of the RTPHintCreator class failed",
	RTP_RTPHINTCREATOR_FINISH_FAILED,
		"The finish method of the RTPHintCreator class failed",
	RTP_RTPHINTCREATOR_ADDRTPHINTSAMPLE_FAILED,
		"The addRTPHintSample method of the RTPHintCreator class failed",
	RTP_RTPHINTCREATOR_ADDRTPHINTPACKETTOCURRENTRTPHINTSAMPLE_FAILED,
		"The addRTPHintPacketToCurrentRTPHintSample method of the RTPHintCreator class failed",
	RTP_RTPHINTCREATOR_ADDRTPREFERENCESAMPLETOCURRENTRTPHINTPACKET_FAILED,
		"The addRTPReferenceSampleToCurrentRTPHintPacket method of the RTPHintCreator class failed",
	RTP_RTPHINTCREATOR_ADDRTPIMMEDIATEDATATOCURRENTRTPHINTPACKET_FAILED,
		"The addRTPImmediateDataToCurrentRTPHintPacket method of the RTPHintCreator class failed",
	RTP_RTPHINTCREATOR_UPDATESTATISTICSFORRTPHINTPACKET_FAILED,
		"The updateStatisticsForRTPHintPacket method of the RTPHintCreator class failed",
	RTP_RTPHINTCREATOR_UPDATESTATISTICSFORRTPHINTSAMPLE_FAILED,
		"The updateStatisticsForRTPHintSample method of the RTPHintCreator class failed",
	RTP_RTPHINTCREATOR_APPENDRTPHINTSAMPLETOMDATATOM_FAILED,
		"The appendRTPHintSampleToMdatAtom method of the RTPHintCreator class failed",

	// RTPHintSample
	RTP_RTPHINTSAMPLE_INIT_FAILED,
		"The init method of the RTPHintSample class failed",
	RTP_RTPHINTSAMPLE_FINISH_FAILED,
		"The finish method of the RTPHintSample class failed",
	RTP_RTPHINTSAMPLE_GETSTATE_FAILED,
		"The getState method of the RTPHintSample class failed",
	RTP_RTPHINTSAMPLE_CREATEPROPERTIES_FAILED,
		"The createProperties method of the RTPHintSample class failed",
	RTP_RTPHINTSAMPLE_GETPROPERTIES_FAILED,
		"The getProperties method of the RTPHintSample class failed",
	RTP_RTPHINTSAMPLE_CREATERTPHINTPACKET_FAILED,
		"The createRTPHintPacket method of the RTPHintSample class failed",
	RTP_RTPHINTSAMPLE_APPENDTOMDATATOM_FAILED,
		"The appendToMdatAtom method of the RTPHintSample class failed",
	RTP_RTPHINTSAMPLE_GETHINTSAMPLEIDENTIFIER_FAILED,
		"The getHintSampleIdentifier method of the RTPHintSample class failed",
	RTP_RTPHINTSAMPLE_GETBYTESNUMBERFORMDAT_FAILED,
		"The getBytesNumberForMdat method of the RTPHintSample class failed",
	RTP_RTPHINTSAMPLE_GETPACKETSNUMBER_FAILED,
		"The getPacketsNumber method of the RTPHintSample class failed",
	RTP_RTPHINTSAMPLE_GETHINTPACKET_FAILED,
		"The getHintPacket method of the RTPHintSample class failed",
	RTP_RTPHINTSAMPLE_NOTCONSISTENT,
		"RTPHintSample not consistent",

	// RTPHintPacket
	RTP_RTPHINTPACKET_INIT_FAILED,
		"The init method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_FINISH_FAILED,
		"The finish method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_CREATEPROPERTIES_FAILED,
		"The createProperties method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETPROPERTIES_FAILED,
		"The getProperties method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETRTPHINTPACKETDATA_FAILED,
		"The getRTPHintPacketData method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETRTPIMMEDIATEDATA_FAILED,
		"The getRTPImmediateData method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETRTPREFERENCEDATA_FAILED,
		"The getRTPReferenceData method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETENTRIESCOUNT_FAILED,
		"The getEntriesCount method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETEXTRAINFORMATIONTVL_FAILED,
		"The getExtraInformationTVL method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_SETBFRAME_FAILED,
		"The setBFrame method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETBFRAME_FAILED,
		"The getBFrame method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_SETTLVTIMESTAMPOFFSET_FAILED,
		"The setTLVTimestampOffset method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETTLVTIMESTAMPOFFSET_FAILED,
		"The getTLVTimestampOffset method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETPAYLOADTYPE_FAILED,
		"The getPayloadType method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_SETPAYLOADTYPE_FAILED,
		"The setPayloadType method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETSEQUENCENUMBER_FAILED,
		"The getSequenceNumber method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_SETSEQUENCENUMBER_FAILED,
		"The setSequenceNumber method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETMARKERBIT_FAILED,
		"The getMarkerBit method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_SETMARKERBIT_FAILED,
		"The setMarkerBit method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_CREATERTPIMMEDIATEDATA_FAILED,
	"The createRTPHintImmediateData method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_CREATERTPREFERENCESAMPLE_FAILED,
	"The createRTPHintReferenceSample method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_CREATEEMBEDDEDRTPREFERENCESAMPLE_FAILED,
		"The createEmbeddedRTPReferenceSample method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETBYTESNUMBERFORMDAT_FAILED,
		"The getBytesNumberForMdat method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETPAYLOADANDPACKETPROPERTIESBYTESNUMBER_FAILED,
		"The getPayloadAndPacketPropertiesBytesNumber method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETIMMEDIATEDATABYTESNUMBER_FAILED,
		"The getImmediateDataBytesNumber method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETMEDIADATABYTESNUMBER_FAILED,
		"The getMediaDataBytesNumber method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETPAYLOADBYTESNUMBER_FAILED,
		"The getPayloadBytesNumber method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETPACKET_FAILED,
		"The getPacket method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETRELATIVEPACKETTRANSMISSIONTIME_FAILED,
		"The getRelativePacketTransmissionTime method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETELEMENTSNUMBERINDATATABLE_FAILED,
		"The getElementsNumberInDataTable method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_GETHINTPACKETDATA_FAILED,
		"The getHintPacketData method of the RTPHintPacket class failed",
	RTP_RTPHINTPACKET_DATASOURCEWRONG,
		"Wrong 'data source' in the RTPHintPacket",

	// RTPHintPacketData
	RTP_RTPHINTPACKETDATA_INIT_FAILED,
		"The init method of the RTPHintPacketData class failed",
	RTP_RTPHINTPACKETDATA_FINISH_FAILED,
		"The finish method of the RTPHintPacketData class failed",
	RTP_RTPHINTPACKETDATA_CREATEPROPERTIES_FAILED,
		"The createProperties method of the RTPHintPacketData class failed",
	RTP_RTPHINTPACKETDATA_GETPACKETHINTDATA_FAILED,
		"The getPacketHintData method of the RTPHintPacketData class failed",
	RTP_RTPHINTPACKETDATA_APPENDPAYLOADDATATORTPPACKET_FAILED,
		"The appendPayloadDataToRTPPacket method of the RTPHintPacketData class failed",

	// RTPHintPacketImmediateData
	RTP_RTPHINTPACKETIMMEDIATEDATA_INIT_FAILED,
		"The init method of the RTPHintPacketImmediateData class failed",
	RTP_RTPHINTPACKETIMMEDIATEDATA_FINISH_FAILED,
		"The finish method of the RTPHintPacketImmediateData class failed",
	RTP_RTPHINTPACKETIMMEDIATEDATA_SETDATA_FAILED,
		"The setData method of the RTPHintPacketImmediateData class failed",
	RTP_RTPHINTPACKETIMMEDIATEDATE_GETIMMEDIATELENGTH_FAILED,
		"The getImmediateLength method of the RTPHintPacketImmediateData class failed",
	RTP_RTPHINTPACKETIMMEDIATEDATE_GETPAYLOADDATALENGTH_FAILED,
		"The getPayloadDataLength method of the RTPHintPacketImmediateData class failed",
	RTP_RTPHINTPACKETIMMEDIATEDATA_BYTESNOTCONSISTENT,
		"Bytes not consistent",

	// RTPHintPacketSampleData
	RTP_RTPHINTPACKETSAMPLEDATA_INIT_FAILED,
		"The init method of the RTPHintPacketSampleData class failed",
	RTP_RTPHINTPACKETSAMPLEDATA_FINISH_FAILED,
		"The finish method of the RTPHintPacketSampleData class failed",
	RTP_RTPHINTPACKETSAMPLEDATA_GETTRACKREFERENCEINDEX_FAILED,
"The getTrackReferenceIndex method of the RTPHintPacketSampleData class failed",
	RTP_RTPHINTPACKETSAMPLEDATA_GETSAMPLELENGTH_FAILED,
	"The getSampleLength method of the RTPHintPacketSampleData class failed",
	RTP_RTPHINTPACKETSAMPLEDATA_GETBYTESPERBLOCK_FAILED,
	"The getBytesPerBlock method of the RTPHintPacketSampleData class failed",
	RTP_RTPHINTPACKETSAMPLEDATA_GETSAMPLESPERBLOCK_FAILED,
	"The getSamplesPerBlock method of the RTPHintPacketSampleData class failed",
	RTP_RTPHINTPACKETSAMPLEDATA_GETSAMPLEOFFSET_FAILED,
	"The getSampleOffset method of the RTPHintPacketSampleData class failed",
	RTP_RTPHINTPACKETSAMPLEDATA_SETREFERENCESAMPLE_FAILED,
		"The setReferenceSample method of the RTPHintPacketSampleData class failed",
	RTP_RTPHINTPACKETSAMPLEDATA_SETEMBEDDEDREFERENCESAMPLE_FAILED,
		"The setEmbeddedReferenceSample method of the RTPHintPacketSampleData class failed",
	RTP_RTPHINTPACKETSAMPLEDATA_APPENDPAYLOADDATATORTPPACKET_FAILED,
		"The appendPayloadDataToRTPPacket method of the RTPHintPacketSampleData class failed",
	RTP_RTPHINTPACKETSAMPLEDATA_WRONGTRACKREFERENCEINDEX,
		"Wrong track reference index",

	// RTPPacket
	RTP_RTPPACKET_INIT_FAILED,
		"The init method of the RTPPacket class failed",
	RTP_RTPPACKET_FINISH_FAILED,
		"The finish method of the RTPPacket class failed",
	RTP_RTPPACKET_SETRTPPACKET_FAILED,
		"The setRTPPacket method of the RTPPacket class failed",
	RTP_RTPPACKET_SETRANDOMOFFSETS_FAILED,
		"The setRandomOffsets method of the RTPPacket class failed",
	RTP_RTPPACKET_RESET_FAILED,
		"The reset method of the RTPPacket class failed",
	RTP_RTPPACKET_SETVERSION_FAILED,
		"The setVersion method of the RTPPacket class failed",
	RTP_RTPPACKET_GETMARKERBIT_FAILED,
		"The getMarkerBit method of the RTPPacket class failed",
	RTP_RTPPACKET_SETMARKERBIT_FAILED,
		"The setMarkerBit method of the RTPPacket class failed",
	RTP_RTPPACKET_GETTIMESTAMP_FAILED,
		"The getTimestamp method of the RTPPacket class failed",
	RTP_RTPPACKET_SETTIMESTAMP_FAILED,
		"The setTimestamp method of the RTPPacket class failed",
	RTP_RTPPACKET_GETSEQUENCENUMBER_FAILED,
		"The getSequenceNumber method of the RTPPacket class failed",
	RTP_RTPPACKET_SETSEQUENCENUMBER_FAILED,
		"The setSequenceNumber method of the RTPPacket class failed",
	RTP_RTPPACKET_GETPAYLOADTYPE_FAILED,
		"The getPayloadType method of the RTPPacket class failed",
	RTP_RTPPACKET_SETPAYLOADTYPE_FAILED,
		"The setPayloadType method of the RTPPacket class failed",
	RTP_RTPPACKET_SETSERVERSSRC_FAILED,
		"The setServerSSRC method of the RTPPacket class failed",
	RTP_RTPPACKET_APPENDDATA_FAILED,
		"The appendData method of the RTPPacket class failed",
	RTP_RTPPACKET_INSERTDATA_FAILED,
		"The insertData method of the RTPPacket class failed",
	RTP_RTPPACKET_DELETEDATA_FAILED,
		"The deleteData method of the RTPPacket class failed",
	RTP_RTPPACKET_DATATOOBIGFORTHERTPPACKETSIZE,
		"Data too big (%llu) for the RTP packet size (%lu)",

	// RTPUtility
	RTP_RTPUTILITY_READSAMPLE_FAILED,
		"The readSample method of the RTPUtility class failed",
	RTP_RTPUTILITY_GETSAMPLENUMBERFROMSAMPLETIME_FAILED,
		"The getSampleNumberFromSampleTime method of the RTPUtility class failed",
	RTP_RTPUTILITY_GETSAMPLETIMEFROMSAMPLENUMBER_FAILED,
		"The getSampleTimeFromSampleNumber method of the RTPUtility class failed",  
	RTP_RTPUTILITY_APPENDHINTSAMPLETOMDATATOM_FAILED,
		"The appendHintsampleToMdatAtom method of the RTPUtility class failed",
	RTP_RTPUTILITY_GETSAMPLEINFOFROMSAMPLENUMBER_FAILED,
		"The method getSampleInfoFromSampleNumber of the RTPUtility class failed. Sample identifier: %ld",
	RTP_RTPUTILITY_GETCHUNKINFOFROMSAMPLENUMBER_FAILED,
		"The getChunkInfoFromSampleNumber method of the RTPUtility class failed",
	RTP_RTPUTILITY_GETSAMPLERANGESIZE_FAILED,
		"The getSampleRangeSize method of the RTPUtility class failed",
	RTP_RTPUTILITY_APPENDTORTPPACKETFROMMDATATOM_FAILED,
		"The appendToRTPPacketFromMdatAtom method of the RTPUtility class failed. Data offset: %llu, data length: %lld",
	RTP_RTPUTILITY_GETRTPTIMESCALE_FAILED,
		"The getRTPTimeScale method of the RTPUtility class failed",
	RTP_RTPUTILITY_GETAVGBITRATE_FAILED,
		"The getAvgBitRate method of the RTPUtility class failed",
	RTP_RTPUTILITY_GETULONGRANDOM_FAILED,
		"The getULongRandom method of the RTPUtility class failed",
	RTP_RTPUTILITY_GETUSHORTRANDOM_FAILED,
		"The getUShortRandom method of the RTPUtility class failed",
	RTP_RTPUTILITY_SAMPLESIZENOTCONSISTENT,
		"Sample size not consistent",
	RTP_RTPUTILITY_MEDIATIMETOOHIGH,
		"Media time too high (Sample time: %llu)",
	RTP_RTPUTILITY_SAMPLEIDENTIFIERTOOHIGH,
		"Sample identifier too high",
	RTP_RTPUTILITY_CREATEHINTTRACK_FAILED,
	"The createHintTrack method of the RTPUtility class failed (Track type: %s)",
	RTP_RTPUTILITY_CREATEALLHINTTRACKS_FAILED,
"The createAllHintTracks method of the RTPUtility class failed (Track type: %s)",
	RTP_RTPUTILITY_ALLOCRTPPAYLOADNUMBER_FAILED,
		"The allocRtpPayloadNumber method of the RTPUtility class failed",

	// common
	RTP_NEW_FAILED,
		"new failed",
	RTP_ACTIVATION_WRONG,
		"Activation wrong",
	RTP_NOTIMPLEMENTEDYET,
		"not implemented yet"



	// Insert here other errors...

} ;

