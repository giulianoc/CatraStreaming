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

#ifndef SDPMedia_h 
	#define SDPMedia_h

	#include "SDPErrors.h"
	#include "Tracer.h"
	#include "StringTokenizer.h"
	#include "SDPAttribute.h"
	#include <vector>


	#define SDP_MAXTRANSPORTLENGTH				128 + 1
	#define SDP_MAXFMTLISTLENGTH				128 + 1

	#define SDP_MAXINFORMATIONLENGTH			128 + 1
	#define SDP_MAXCONNECTIONNETWORKTYPELENGTH	128 + 1
	#define SDP_MAXCONNECTIONADDRESSTYPELENGTH	128 + 1
	#define SDP_MAXCONNECTIONADDRESSLENGTH		128 + 1
	#define SDP_MAXBANDWIDTHMODIFIERLENGTH		128 + 1

	#define SDP_ENCODINGPARAMETESLENGTH			128 + 1

	#define SDP_MEDIA_VIDEO						"video"
	#define SDP_MEDIA_AUDIO						"audio"
	#define SDP_MEDIA_APPLICATION				"application"
	#define SDP_MEDIA_DATA						"data"
	#define SDP_MEDIA_CONTROL					"control"


	typedef class SDPMedia {

		public:
			typedef enum SDPMediaType {
				SDPMEDIA_VIDEO,
				SDPMEDIA_AUDIO,
				SDPMEDIA_APPLICATION,
				SDPMEDIA_DATA,
				SDPMEDIA_CONTROL,
				SDPMEDIA_UNKNOWN
			} SDPMediaType_t, *SDPMediaType_p;

		protected:
			Tracer_p					_ptTracer;

			SDPMediaType_t				_smtSDPMediaType;
			unsigned long				_ulPort;
			char						_pTransport [
				SDP_MAXTRANSPORTLENGTH];
			char						_pFmtList [
				SDP_MAXFMTLISTLENGTH];

			char						_pInformation [
				SDP_MAXINFORMATIONLENGTH];
			char						_pConnectionNetworkType [
				SDP_MAXCONNECTIONNETWORKTYPELENGTH];
			char						_pConnectionAddressType [
				SDP_MAXCONNECTIONADDRESSTYPELENGTH];
			char						_pConnectionAddress [
				SDP_MAXCONNECTIONADDRESSLENGTH];
			char						_pBandwidthModifier [
				SDP_MAXBANDWIDTHMODIFIERLENGTH];
			unsigned long				_ulBandwidthValueInKbps;

			std:: vector<SDPAttribute_p>	_vSDPAttributes;

			SDPRtpMapAttribute_p		_psaSDPRtpMapAttribute;
			SDPFmtpAttribute_p			_psaSDPFmtpAttribute;


			SDPMedia (const SDPMedia &);

			SDPMedia &operator = (const SDPMedia &);

			/**
				This method create a generic SDPAttribute and could be
				customized to manage specific attributes
				that are not defined by the SDP rfc
			*/
			virtual Error createUnknownSDPAttribute (
				StringTokenizer_p pstLineTokenizer,
				const char *pAttributeName, SDPAttribute_p *psaSDPAttribute);

		public:
			SDPMedia (void);

			~SDPMedia (void);

			virtual Error init (SDPMediaType_t smtSDPMediaType,
				Tracer_p ptTracer);

			virtual Error init (Buffer_p pbSDPMedia, Tracer_p ptTracer);

			Error finish (void);

			virtual Error appendToBuffer (Buffer_p pbSDP);

			Error getMediaType (SDPMediaType_p pmtMediaType);

			Error setMediaType (SDPMediaType_t mtMediaType);

			virtual Error addAttribute (SDPAttribute_p psaSDPAttribute);

			Error setPort (unsigned long ulPort);

			Error getPort (unsigned long *pulPort);

			Error setTransport (const char *pTransport);

			Error getTransport (char *pTransport);

			Error setFmtList (const char *pFmtList);

			Error getFmtList (char *pFmtList);

			Error setInformation (const char *pInformation);

			Error getInformation (char *pInformation);

			Error setConnectionNetworkType (
				const char *pConnectionNetworkType);

			Error getConnectionNetworkType (char *pConnectionNetworkType);

			Error setConnectionAddressType (
				const char *pConnectionAddressType);

			Error getConnectionAddressType (char *pConnectionAddressType);

			Error setConnectionAddress (const char *pConnectionAddress);

			Error getConnectionAddress (char *pConnectionAddress);

			Error setBandwidthModifier (const char *pBandwidthModifier);

			Error getBandwidthModifier (char *pBandwidthModifier);

			Error setBandwidthValueInKbps (
				unsigned long ulBandwidthValueInKbps);

			Error getBandwidthValueInKbps (
				unsigned long *pulBandwidthValueInKbps);

			/**
				This method returns information about the media.
				Every parameter could assume a NULL value and, in this
				case, the method will not use it
			*/
			Error getMediaInfo (
				SDPMediaType_p pmtMediaType,
				unsigned long *pulMediaPayloadType,
				unsigned long *pulMediaPort,
				char *pMediaEncodingName,
				char *pMediaClockRate,
				char *pMediaEncodingParameters,
				unsigned long *pulMediaBandwidthInKbps);

		} SDPMedia_t, *SDPMedia_p;

#endif

