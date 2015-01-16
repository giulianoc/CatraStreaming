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

#ifndef SDPForRTSP_h 
	#define SDPForRTSP_h

	#include "SDP.h"
	#include "MP4Atom.h"


	typedef class SDPForRTSP: public SDP {

			SDPForRTSP (const SDPForRTSP &);

			SDPForRTSP &operator = (const SDPForRTSP &);

		protected:
			/**
				This method is redefined in order to build a SDPMediaForRTSP
				object
			*/
			virtual Error buildSDPMedia (SDPMedia_p *psmCurrentSDPMedia);


			SDPAttribute_p			_psaControlSDPAttribute;
			SDPAttribute_p			_psaRangeSDPAttribute;
			Boolean_t				_bIsNow;
			double					_dNptStartTime;
			double					_dNptEndTime;

		public:
			SDPForRTSP ();

			~SDPForRTSP ();

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

			virtual Error addAttribute (SDPAttribute_p psaSDPAttribute);

			Error getRangeAttributeInfo (
				Boolean_p pbIsNow,
				double *pdNptStartTime,
				double *pdNptEndTime);

			static Error getRangeAttributeInfo (
				const char *pRangeValue,
				Boolean_p pbIsNow,
				double *pdNptStartTime,
				double *pdNptEndTime,
				Tracer_p ptTracer);

			/**
				This method receives in input the SDP path name
				and returns in output the SDP (may be corrected according
				the ISMA standard to be used and all the information about
				the audio and video tracks.
			*/
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
				Buffer_p pbSDP,
				Tracer_p ptTracer);

		} SDPForRTSP_t, *SDPForRTSP_p;

#endif

