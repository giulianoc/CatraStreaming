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


#ifndef RTSPUtility_h
	#define RTSPUtility_h

	#include "MP4File.h"
	#include "RTSPErrors.h"
	// #include <vector>


	#define RTSP_MAXLONGLENGTH					64 + 1
	#define RTSP_MAXDATELENGTH					512 + 1
	#define RTSP_PREDEFINEDRTSPVERSION			"RTSP/1.0"
	#define RTSP_STATUSCODE_OK					200
	#define RTSP_STATUSCODE_MOVEDPERMANENTLY	301

	// CR LF (13 10)
	#define RTSP_NEWLINE				"\r\n"


	typedef class RTSPUtility {

		private:
			RTSPUtility (const RTSPUtility &);

			RTSPUtility &operator = (const RTSPUtility &);

		public:
			RTSPUtility ();

			~RTSPUtility ();

			/**
				This method returns a buffer containing the DESCRIBE request
			*/
			static Error getDESCRIBERequest (
				const char *pURL,
				unsigned long ulSequenceNumber,
				const char *pUserAgent,
				Buffer_p pbRTSPRequest,
				Tracer_p ptTracer);

			/**
				This method parses the DESCRIBE request and
				provides in output all the fields of the DESCRIBE.

				The pRTSPRequest argument contains the RTSP DESCRIBE header
				where each line is separated by \r\n. The last line must
				finish with \r\n\r\n (end RTSP header).

				The pbRTSPVersion parameter could be NULL and
				in this case is not filled by this method
			*/
			static Error parseDESCRIBERequest (
				const char *pRTSPRequest,

				char *pRTSPServerIPAddress,
				unsigned long ulIPAddressBufferLength,
				long *plRTSPServerPort,
				Buffer_p pbRelativePathWithoutParameters,
				Buffer_p pbURLParameters,
				Buffer_p pbRTSPVersion,
				Buffer_p pbUserAgent,
				long *plSequenceNumber,
				char *pTimestamp,
				unsigned long *pulRequestedPayloadSizeInBytes,
				unsigned long *pulRequestLength,
				Tracer_p ptTracer);

			/**
				This method receives in input information about
				the audio and video tracks and returns in output
				the SDP and the payload calculated for the tracks
				(pbSDP, pulVideoPayloadNumber, pulAudioPayloadNumber)
				The pointer to the track information
				(pmtiHintOrMediaAudioTrackInfo, pmtiHintOrMediaVideoTrackInfo)
				could be null in case the track does not exist.
			static Error getSDPFromMediaInfo (
				MP4File_p pmfFile,
				const char *pOriginAddress,
				MP4TrackInfo_p pmtiHintOrMediaVideoTrackInfo,
				MP4Atom:: MP4Codec_t cVideoCodecUsed,
				unsigned long ulVideoAvgBitRate,
				char *pVideoTrackName,
				MP4TrackInfo_p pmtiHintOrMediaAudioTrackInfo,
				MP4Atom:: MP4Codec_t cAudioCodecUsed,
				unsigned long ulAudioAvgBitRate,
				char *pAudioTrackName,
				MP4Atom:: Standard_t sStandard,
				Buffer_p pbSDP,
				unsigned long *pulVideoPayloadNumber,
				unsigned long *pulAudioPayloadNumber,
				Tracer_p ptTracer);

				This method receives in input the SDP path name
				and returns in output the SDP (may be corrected according
				the standard to be used and all the information about
				the audio and video tracks.
			static Error getMediaInfoFromSDPFile (
				const char *pSDPPathName,
				Boolean_p pbVideoTrackFoundInSDP,
				MP4Atom:: MP4Codec_p pcVideoCodecUsed,
				unsigned long *pulVideoAvgBitRate,
				unsigned long *pulVideoPayloadNumber,
				char *pVideoTrackName,
				unsigned long *pulVideoPort,
				Boolean_p pbAudioTrackFoundInSDP,
				MP4Atom:: MP4Codec_p pcAudioCodecUsed,
				unsigned long *pulAudioAvgBitRate,
				unsigned long *pulAudioPayloadNumber,
				char *pAudioTrackName,
				unsigned long *pulAudioPort,
				MP4Atom:: Standard_t sStandard,
				Buffer_p pbSDP,
				Tracer_p ptTracer);
			*/

			/**
			*/
			static Error getDESCRIBEResponse (
				unsigned long ulSequenceNumber,
				unsigned long ulRTSP_RTCPTimeoutInSecs,
				const char *pTimestamp,
				Buffer_p pbSDP,
				Buffer_p pbResponse,
				Tracer_p ptTracer);

			static Error parseDESCRIBEResponse (
				const char *pRTSPResponse,

				unsigned long *pulStatusCode,
				Buffer_p pbLocation,
				unsigned long *pulSequenceNumber,

				Buffer_p pbSession,
				unsigned long *pulTimeout,

				Buffer_p pbSDP,

				Tracer_p ptTracer);

			static Error getSETUPRequest (
				const char *pURL,
				const char *pControlValue,
				unsigned long ulSequenceNumber,
				const char *pSession,
				unsigned long ulClientRTPPort, unsigned long ulClientRTCPPort,
				const char *pUserAgent,
				Buffer_p pbRTSPRequest,
				Tracer_p ptTracer);

			static Error parseSETUPRequest (
				const char *pRTSPRequest,

				Buffer_p pbRTSPVersion,
				Buffer_p pbTransport,
				Buffer_p pbURI,
				long *plSequenceNumber,
				unsigned long *pulRTPClientPort,
				unsigned long *pulRTCPClientPort,
				char *pSessionIdentifier,
				char *pTimestamp,
				unsigned long *pulRequestedPayloadSizeInBytes,
				unsigned long *pulRequestLength,
				Tracer_p ptTracer);

			/**
			*/
			static Error getSETUPResponse (
				unsigned long ulSequenceNumber,
				const char *pSessionIdentifier,
				unsigned long ulRTSP_RTCPTimeoutInSecs,
				const char *pTimestamp,
				Buffer_p pbTransport,
				unsigned long ulRTPServerPort,
				unsigned long ulRTCPServerPort,
				Buffer_p pbResponse,
				Tracer_p ptTracer);

			/**
			*/
			static Error parseSETUPResponse (
				const char *pRTSPResponse,

				unsigned long *pulStatusCode,
				Buffer_p pbSession,
				unsigned long *pulTimeout,
				unsigned long *pulSequenceNumber,
				unsigned long *pulServerRTPPort,
				unsigned long *pulServerRTCPPort,

				Tracer_p ptTracer);

			/**
			*/
			static Error getPLAYRequest (
				const char *pURL,
				const char *pControlValue,
				unsigned long ulSequenceNumber,
				const char *pSession,
				const char *pUserAgent,
				Buffer_p pbRTSPRequest,
				Tracer_p ptTracer);

			/**
			*/
			static Error parsePLAYRequest (
				const char *pRTSPRequest,

				Buffer_p pbRTSPVersion,
				Buffer_p pbURI,
				long *plSequenceNumber,
				char *pTimestamp,
				Boolean_p pbHasSpeed,
				double *pdSpeed,
				Boolean_p pbRangeFound,
				Boolean_p pbIsNow,
				double *pdNptStartTime,
				double *pdNptEndTime,
				char *pSessionIdentifier,
				unsigned long *pulRequestLength,
				Tracer_p ptTracer);

			static Error getPLAYResponse (
				unsigned long ulSequenceNumber,
				const char *pSessionIdentifier,
				unsigned long ulRTSP_RTCPTimeoutInSecs,
				const char *pTimestamp,

				Boolean_t bHasSpeed,
				double *pdSpeed,
				Boolean_t bHasRange,
				Boolean_t bIsNow,
				double *pdNptStartTime,
				double *pdNptEndTime,

				Boolean_t bVideoRTPInfo,
				const char *pVideoRequestURI,
				unsigned long ulVideoSequenceNumber,
				unsigned long ulVideoTimeStamp,

				Boolean_t bAudioRTPInfo,
				const char *pAudioRequestURI,
				unsigned long ulAudioSequenceNumber,
				unsigned long ulAudioTimeStamp,

				Buffer_p pbResponse,
				Tracer_p ptTracer);

			static Error parsePLAYResponse (
				const char *pRTSPResponse,
				const char *pVideoControl,
				const char *pAudioControl,

				unsigned long *pulStatusCode,
				unsigned long *pulSequenceNumber,
				unsigned long *pulStartVideoSequenceNumber,
				unsigned long *ulStartVideoTimeStamp,
				unsigned long *pulStartAudioSequenceNumber,
				unsigned long *ulStartAudioTimeStamp,

				Tracer_p ptTracer);

			/**
			*/
			static Error parsePAUSERequest (
				const char *pRTSPRequest,

				Buffer_p pbRTSPVersion,
				Buffer_p pbURI,
				long *plSequenceNumber,
				char *pTimestamp,
				char *pSessionIdentifier,
				unsigned long *pulRequestLength,
				Tracer_p ptTracer);

			/**
			*/
			static Error getPAUSEResponse (
				unsigned long ulSequenceNumber,
				const char *pSessionIdentifier,
				unsigned long ulRTSP_RTCPTimeoutInSecs,
				const char *pTimestamp,

				Buffer_p pbResponse,
				Tracer_p ptTracer);

			static Error getTEARDOWNRequest (
				const char *pURL,
				unsigned long ulSequenceNumber,
				const char *pSession,
				const char *pUserAgent,
				Buffer_p pbRTSPRequest,
				Tracer_p ptTracer);

			static Error parseTEARDOWNRequest (
				const char *pRTSPRequest,

				Buffer_p pbRTSPVersion,
				Buffer_p pbURI,
				long *plSequenceNumber,
				char *pTimestamp,
				char *pSessionIdentifier,
				unsigned long *pulRequestLength,
				Tracer_p ptTracer);

			static Error getTEARDOWNResponse (
				unsigned long ulSequenceNumber,
				const char *pSessionIdentifier,
				unsigned long ulRTSP_RTCPTimeoutInSecs,
				const char *pTimestamp,

				Buffer_p pbResponse,
				Tracer_p ptTracer);

			static Error parseTEARDOWNResponse (
				const char *pRTSPResponse,

				unsigned long *pulStatusCode,
				unsigned long *pulSequenceNumber,

				Tracer_p ptTracer);

			static Error getOPTIONSRequest (
				const char *pURL,
				unsigned long ulSequenceNumber,
				const char *pUserAgent,
				Buffer_p pbRTSPRequest,
				Tracer_p ptTracer);

			/**
			*/
			static Error parseOPTIONSRequest (
				const char *pRTSPRequest,

				Buffer_p pbRTSPVersion,
				Buffer_p pbURI,
				long *plSequenceNumber,
				char *pTimestamp,
				char *pSessionIdentifier,
				unsigned long *pulRequestLength,
				Tracer_p ptTracer);

			/**
			*/
			static Error getOPTIONSResponse (
				unsigned long ulSequenceNumber,
				const char *pSessionIdentifier,
				unsigned long ulRTSP_RTCPTimeoutInSecs,
				const char *pTimestamp,
				Boolean_t bIsPreviousRTSPCommandPAUSE,

				Buffer_p pbResponse,
				Tracer_p ptTracer);

			/**
			*/
			static Error getRTSPResponse (const char *pVersion,
				unsigned long ulStatusCode,
				long lSequenceNumber, const char *pSessionIdentifier,
				unsigned long ulSessionTimeoutInSecs,
				Boolean_t bConnectionClose,
				const char *pTimestamp, Buffer_p pbResponse,
				Tracer_p ptTracer);

			/**
			*/
			static Error getDateStr (char pDate [RTSP_MAXDATELENGTH],
				Tracer_p ptTracer);

	} RTSPUtility_t, *RTSPUtility_p;

#endif

