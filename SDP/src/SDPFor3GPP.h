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

#ifndef SDPFor3GPP_h 
	#define SDPFor3GPP_h

	#include "SDPForRTSP.h"
	#include "MP4File.h"

	#define SDP_MAXLONGLENGTH				64 + 1


	typedef class SDPFor3GPP: public SDPForRTSP {

			SDPFor3GPP (const SDPFor3GPP &);

			SDPFor3GPP &operator = (const SDPFor3GPP &);

		protected:
			/**
				This method is redefined in order to build a SDPMediaFor3GPP
				object
			*/
			virtual Error buildSDPMedia (SDPMedia_p *psmCurrentSDPMedia);

		public:
			SDPFor3GPP ();

			~SDPFor3GPP ();

			virtual Error appendToBuffer (Buffer_p pbSDP);

			/**
				This method receives in input information about
				the audio and video tracks and returns in output
				the SDP and the payload calculated for the tracks
				(pbSDP, pulVideoPayloadNumber, pulAudioPayloadNumber)
				The pointer to the track information
				(pmtiMediaAudioTrackInfo, pmtiMediaVideoTrackInfo)
				could be null in case the track does not exist.

				ucChannels will be used by this method only if
					the sStandard is MP4Atom:: MP4F_ISMA and
					the AudioCodec is MP4Atom:: MP4F_CODEC_AAC
				ucVideoSystemsProfileLevel is used only if the codec is MPEG4
			*/
			static Error getSDPFromMediaInfo (
				MP4File_p pmfFile,
				const char *pOriginAddress,
				MP4TrackInfo_p pmtiMediaVideoTrackInfo,
				unsigned long ulVideoAvgBitRate,
				unsigned long ulVideoRTPTimeScale,
				unsigned char ucVideoSystemsProfileLevel,
				MP4TrackInfo_p pmtiMediaAudioTrackInfo,
				unsigned long ulAudioAvgBitRate,
				unsigned long ulAudioRTPTimeScale,
				unsigned char ucChannels,
				MP4Atom:: Standard_t sStandard,
				Buffer_p pbSDP,
				unsigned long *pulVideoPayloadNumber,
				unsigned long *pulAudioPayloadNumber,
				Tracer_p ptTracer);

			/**
				This method receives in input the SDP path name
				and returns in output the SDP (may be corrected according
				the standard to be used and all the information about
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

		} SDPFor3GPP_t, *SDPFor3GPP_p;

#endif

