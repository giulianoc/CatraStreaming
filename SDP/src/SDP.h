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

#ifndef SDP_h 
	#define SDP_h

	#include "SDPErrors.h"
	#include "SDPMedia.h"
	#include "Buffer.h"
	#include "StringTokenizer.h"
	#include "Tracer.h"
	#include <vector>


	#define SDP_MAXORIGINUSERNAMELENGTH			128 + 1
	#define SDP_MAXORIGINSESSIONIDLENGTH		128 + 1
	#define SDP_MAXORIGINVERSIONLENGTH			128 + 1
	#define SDP_MAXORIGINNETWORKTYPELENGTH		128 + 1
	#define SDP_MAXORIGINADDRESSTYPELENGTH		128 + 1
	#define SDP_MAXORIGINADDRESSLENGTH			128 + 1

	#define SDP_MAXSESSIONNAMELENGTH			128 + 1
	#define SDP_MAXURILENGTH					128 + 1
	#define SDP_MAXEMAILADDRESSLENGTH			128 + 1
	#define SDP_MAXPHONENUMBERLENGTH			128 + 1


	typedef class SDP {

		private:
			typedef enum SDPAnouncementState {
				SDP_SESSIONLEVEL,
				SDP_MEDIALEVEL,
				SDP_UNKNOWN
			} SDPAnouncementState_t, *SDPAnouncementState_p;

		protected:
			Tracer_p					_ptTracer;

			unsigned long				_ulVersion;
			char						_pOriginUserName [
				SDP_MAXORIGINUSERNAMELENGTH];
			char						_pOriginSessionId [
				SDP_MAXORIGINSESSIONIDLENGTH];
			char						_pOriginVersion [
				SDP_MAXORIGINVERSIONLENGTH];
			char						_pOriginNetworkType [
				SDP_MAXORIGINNETWORKTYPELENGTH];
			char						_pOriginAddressType [
				SDP_MAXORIGINADDRESSTYPELENGTH];
			char						_pOriginAddress [
				SDP_MAXORIGINADDRESSLENGTH];

			char						_pSessionName [
				SDP_MAXSESSIONNAMELENGTH];
			char						_pInformation [
				SDP_MAXINFORMATIONLENGTH];
			char						_pURI [
				SDP_MAXURILENGTH];
			char						_pEmailAddress [
				SDP_MAXEMAILADDRESSLENGTH];
			char						_pPhoneNumber [
				SDP_MAXPHONENUMBERLENGTH];

			char						_pConnectionNetworkType [
				SDP_MAXCONNECTIONNETWORKTYPELENGTH];
			char						_pConnectionAddressType [
				SDP_MAXCONNECTIONADDRESSTYPELENGTH];
			char						_pConnectionAddress [
				SDP_MAXCONNECTIONADDRESSLENGTH];

			char						_pBandwidthModifier [
				SDP_MAXBANDWIDTHMODIFIERLENGTH];
			unsigned long				_ulBandwidthValueInKbps;

			unsigned long				_ulTimesStart;
			unsigned long				_ulTimesStop;

			std:: vector<SDPAttribute_p>	_vSDPAttributes;
			std:: vector<SDPMedia_p>		_vSDPMedia;


			SDP (const SDP &);

			SDP &operator = (const SDP &);

			/**
				This method create a generic SDPAttribute and could be
				customized to manage specific attributes
				that are not defined by the SDP rfc
			*/
			virtual Error createUnknownSDPAttribute (
				StringTokenizer_p pstLineTokenizer,
				const char *pAttributeName, SDPAttribute_p *psaSDPAttribute);

			/**
				This method build a SDPMedia and could be customized
				to manage specific SDPMedia objects
			*/
			virtual Error buildSDPMedia (SDPMedia_p *psmCurrentSDPMedia);

		public:
			SDP ();

			~SDP ();

			virtual Error init (const char *pSDPPathName, Tracer_p ptTracer);

			virtual Error init (Buffer_p pbSDP, Tracer_p ptTracer);

			virtual Error init (
				unsigned long ulVersion,
				const char *pOriginUserName,
				const char *pOriginSessionId,
				const char *pOriginVersion,
				const char *pOriginNetworkType,
				const char *pOriginAddressType,
				const char *pOriginAddress,

				const char *pSessionName,
				const char *pInformation,
				const char *pURI,
				const char *pEmailAddress,
				const char *pPhoneNumber,

				const char *pConnectionNetworkType,
				const char *pConnectionAddressType,
				const char *pConnectionAddress,
				const char *pBandwidthModifier,
				unsigned long ulBandwidthValueInKbps,

				unsigned long ulTimesStart,
				unsigned long ulTimesStop,
				Tracer_p ptTracer);

			Error finish (void);

			virtual Error appendToBuffer (Buffer_p pbSDP);

			Error setVersion (unsigned long ulVersion);

			Error setOriginUserName (const char *pOriginUserName);

			Error setOriginSessionId (const char *pOriginSessionId);

			Error setOriginVersion (const char *pOriginVersion);

			Error setOriginNetworkType (const char *pOriginNetworkType);

			Error setOriginAddressType (const char *pOriginAddressType);

			Error setOriginAddress (const char *pOriginAddress);

			Error setSessionName (const char *pSessionName);

			Error setInformation (const char *pInformation);

			Error setURI (const char *pURI);

			Error setEmailAddress (const char *pEmailAddress);

			Error setPhoneNumber (const char *pPhoneNumber);

			Error setConnectionNetworkType (const char *pConnectionNetworkType);

			Error setConnectionAddressType (const char *pConnectionAddressType);

			Error setConnectionAddress (const char *pConnectionAddress);

			Error setBandwidthModifier (const char *pBandwidthModifier);

			Error setBandwidthValueInKbps (
				unsigned long ulBandwidthValueInKbps);

			Error setTimesStart (unsigned long ulTimesStart);

			Error setTimesStop (unsigned long ulTimesStop);

			Error getSDPMedia (
				SDPMedia:: SDPMediaType_t mtMediaType,
				unsigned long ulMediaIndex,
				SDPMedia_p *psmSDPMedia);

			virtual Error addAttribute (SDPAttribute_p psaSDPAttribute);

			Error getAttributeValue (
				const char *pAttributeName, char *pAttributeValue);

			Error addSDPMedia (SDPMedia_p psmSDPMedia);

		} SDP_t, *SDP_p;

#endif

