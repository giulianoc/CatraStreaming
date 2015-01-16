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

#ifndef SDPMediaFor3GPP_h 
	#define SDPMediaFor3GPP_h

	#include "SDPMediaForRTSP.h"
	#include "MP4TrackInfo.h"

	#define SDP_MAXLONGLENGTH				64 + 1


	typedef class SDPMediaFor3GPP: public SDPMediaForRTSP {

		protected:
			SDPMediaFor3GPP (const SDPMediaFor3GPP &);

			SDPMediaFor3GPP &operator = (const SDPMediaFor3GPP &);


		public:
			SDPMediaFor3GPP (void);

			~SDPMediaFor3GPP (void);

			virtual Error appendToBuffer (Buffer_p pbSDP);

			/**
				pmtiHintVideoTrackInfo must be different by NULL if
					we are asking the SDP of an hint track
				ucVideoSystemsProfileLevel is used only if the codec is MPEG4
			*/
			static Error getSDPMediaVideoFromMediaInfo (
				MP4TrackInfo_p pmtiMediaVideoTrackInfo,
				MP4TrackInfo_p pmtiHintVideoTrackInfo,
				unsigned long ulVideoAvgBitRate,
				unsigned long ulVideoRTPTimeScale,
				unsigned char ucVideoSystemsProfileLevel,
				const char *pVideoPayloadNumber,
				MP4Atom:: Standard_t sStandard,
				Tracer_p ptTracer,
				SDPMedia *pSDPVideoMedia);

			/**
				pmtiHintAudioTrackInfo must be different by NULL if
					we are asking the SDP of an hint track
				ucChannels is used only if sStandard is MP4F_ISMA and
					cAudioCodecUsed is MP4F_CODEC_AAC
			*/
			static Error getSDPMediaAudioFromMediaInfo (
				MP4TrackInfo_p pmtiMediaAudioTrackInfo,
				MP4TrackInfo_p pmtiHintAudioTrackInfo,
				unsigned long ulAudioAvgBitRate,
				unsigned long ulAudioRTPTimeScale,
				const char *pAudioPayloadNumber,
				unsigned char ucChannels,
				MP4Atom:: Standard_t sStandard,
				Tracer_p ptTracer,
				SDPMedia_p pSDPAudioMedia);

		} SDPMediaFor3GPP_t, *SDPMediaFor3GPP_p;

#endif

