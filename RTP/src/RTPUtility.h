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


#ifndef RTPUtility_h
	#define RTPUtility_h

	#include "MP4CttsAtom.h"
	#include "MP4DmaxAtom.h"
	#include "MP4File.h"
	#include "MP4HdlrAtom.h"
	#include "MP4MaxrAtom.h"
	#include "MP4MdhdAtom.h"
	#include "MP4MvhdAtom.h"
	#include "MP4RootAtom.h"
	#include "MP4StblAtom.h"
	#include "MP4StcoAtom.h"
	#include "MP4StscAtom.h"
	#include "MP4StssAtom.h"
	#include "MP4StszAtom.h"
	#include "MP4SttsAtom.h"
	#include "MP4TkhdAtom.h"
	#include "MP4TrakAtom.h"
	#include "RTPHintSample.h"
	#include "RTPErrors.h"
	#include "RTPStreamRealTimeInfo.h"
	#include <vector>


	// #define MP4F_NEWLINE					"\r\n"
	// #define MP4F_MAXPATHNAMELENGTH			1024 + 1
	// #define MP4F_MAXCODECUSEDLENGTH			255 + 1
	#define RTP_MAXLONGLENGTH				64 + 1

	typedef class RTPUtility {

		private:
			RTPUtility (const RTPUtility &);

			RTPUtility &operator = (const RTPUtility &);

		public:

		public:
			RTPUtility ();

			~RTPUtility ();

			#ifdef WIN32
				static Error readSample (MP4RootAtom_p pmaRootAtom,
					MP4StscAtom_p pmaStscAtom, MP4StszAtom_p pmaStszAtom,
					MP4StcoAtom_p pmaStcoAtom, MP4SttsAtom_p pmaSttsAtom,
					MP4CttsAtom_p pmaCttsAtom, MP4StssAtom_p pmaStssAtom,
					unsigned long ulSampleIdentifier,
					unsigned char *pucSampleBuffer,
					unsigned long ulSampleBufferSize,
					unsigned long *pulCurrentSampleSize,
					__int64 *pullSampleStartTime,
					__int64 *pullSampleDuration,
					unsigned long *pulSampleRenderingOffset,
					Boolean_p pbIsSyncSample,
					RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
					Tracer_p ptTracer);
			#else
				static Error readSample (MP4RootAtom_p pmaRootAtom,
					MP4StscAtom_p pmaStscAtom, MP4StszAtom_p pmaStszAtom,
					MP4StcoAtom_p pmaStcoAtom, MP4SttsAtom_p pmaSttsAtom,
					MP4CttsAtom_p pmaCttsAtom, MP4StssAtom_p pmaStssAtom,
					unsigned long ulSampleIdentifier,
					unsigned char *pucSampleBuffer,
					unsigned long ulSampleBufferSize,
					unsigned long *pulCurrentSampleSize,
					unsigned long long *pullSampleStartTime,
					unsigned long long *pullSampleDuration,
					unsigned long *pulSampleRenderingOffset,
					Boolean_p pbIsSyncSample,
					RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
					Tracer_p ptTracer);
			#endif


			#ifdef WIN32
				static Error getPointerToSample (MP4RootAtom_p pmaRootAtom,
					MP4StscAtom_p pmaStscAtom, MP4StszAtom_p pmaStszAtom,
					MP4StcoAtom_p pmaStcoAtom, MP4SttsAtom_p pmaSttsAtom,
					MP4CttsAtom_p pmaCttsAtom, MP4StssAtom_p pmaStssAtom,
					unsigned long ulSampleIdentifier,
					unsigned char **pucSampleBuffer,
					__int64 *pullSampleBufferToRead,
					unsigned long *pulCurrentSampleSize,
					__int64 *pullSampleStartTime,
					__int64 *pullSampleDuration,
					unsigned long *pulSampleRenderingOffset,
					Boolean_p pbIsSyncSample,
					RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
					Tracer_p ptTracer);
			#else
				static Error getPointerToSample (MP4RootAtom_p pmaRootAtom,
					MP4StscAtom_p pmaStscAtom, MP4StszAtom_p pmaStszAtom,
					MP4StcoAtom_p pmaStcoAtom, MP4SttsAtom_p pmaSttsAtom,
					MP4CttsAtom_p pmaCttsAtom, MP4StssAtom_p pmaStssAtom,
					unsigned long ulSampleIdentifier,
					unsigned char **pucSampleBuffer,
					unsigned long long *pullSampleBufferToRead,
					unsigned long *pulCurrentSampleSize,
					unsigned long long *pullSampleStartTime,
					unsigned long long *pullSampleDuration,
					unsigned long *pulSampleRenderingOffset,
					Boolean_p pbIsSyncSample,
					RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
					Tracer_p ptTracer);
			#endif

			#ifdef WIN32
				static Error appendToRTPPacketFromMdatAtom (
					MP4RootAtom_p pmaRootAtom, __int64 ullFileOffset,
					RTPPacket_p prpRTPPacket, __int64 llBytesToRead,
					Tracer_p ptTracer);
			#else
				static Error appendToRTPPacketFromMdatAtom (
					MP4RootAtom_p pmaRootAtom, unsigned long long ullFileOffset,
					RTPPacket_p prpRTPPacket, long long llBytesToRead,
					Tracer_p ptTracer);
			#endif

			#ifdef WIN32
				static Error getSampleInfoFromSampleNumber (
					MP4StscAtom_p pmaStscAtom, MP4StszAtom_p pmaStszAtom,
					MP4StcoAtom_p pmaStcoAtom,
					unsigned long ulSampleIdentifier,
					__int64 *pullSampleOffset,
					unsigned long *pulSampleSize,
					unsigned long *pulSampleDescriptionIndex,
					RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
					Tracer_p ptTracer);
			#else
				static Error getSampleInfoFromSampleNumber (
					MP4StscAtom_p pmaStscAtom, MP4StszAtom_p pmaStszAtom,
					MP4StcoAtom_p pmaStcoAtom,
					unsigned long ulSampleIdentifier,
					unsigned long long *pullSampleOffset,
					unsigned long *pulSampleSize,
					unsigned long *pulSampleDescriptionIndex,
					RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
					Tracer_p ptTracer);
			#endif

			static Error getSampleRangeSize (
				MP4StszAtom_p pmaStszAtom, unsigned long ulFirstSampleNumber,
				unsigned long ulLastSampleNumber,
				unsigned long *pulSampleRangeSize,
				RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
				Tracer_p ptTracer);

			#ifdef WIN32
				static Error getSampleNumberFromSampleTime (
					MP4SttsAtom_p pmaSttsAtom, __int64 ullSampleTime,
					unsigned long *pulSampleNumber,
					RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
					Tracer_p ptTracer);
			#else
				static Error getSampleNumberFromSampleTime (
					MP4SttsAtom_p pmaSttsAtom, unsigned long long ullSampleTime,
					unsigned long *pulSampleNumber,
					RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
					Tracer_p ptTracer);
			#endif

			#ifdef WIN32
				static Error getSampleTimeFromSampleNumber (
					MP4SttsAtom_p pmaSttsAtom, unsigned long ulSampleIdentifier,
					__int64 *pullSampleStartTime,
					__int64 *pullSampleDuration,
					RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
					Tracer_p ptTracer);
			#else
				static Error getSampleTimeFromSampleNumber (
					MP4SttsAtom_p pmaSttsAtom, unsigned long ulSampleIdentifier,
					unsigned long long *pullSampleStartTime,
					unsigned long long *pullSampleDuration,
					RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
					Tracer_p ptTracer);
			#endif

			static Error getChunkInfoFromSampleNumber (
				MP4StscAtom_p pmaStscAtom,
				unsigned long ulSampleNumber,				// start from 1
				unsigned long *pulSamplesPerChunk,
				unsigned long *pulChunkNumber,				// start from 1
				unsigned long *pulSampleDescriptionIndex,
				unsigned long *pulSampleOffsetInChunk,		// start from 0
				RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
				Tracer_p ptTracer);

			static Error getAvgBitRate (
				MP4TrakAtom_p pmaMediaTrakAtom,
				MP4HdlrAtom_p pmaMediaHdlrAtom,
				MP4MdhdAtom_p pmaMediaMdhdAtom,
				MP4StszAtom_p pmaStszAtom,
				MP4Atom:: MP4Codec_t cCodecUsed,	
				unsigned long *pulAvgBitRate,
				RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
				Tracer_p ptTracer);

			#ifdef WIN32
				static Error appendHintSampleToMdatAtom (
					MP4RootAtom_p pmaRootAtom,
					MP4SttsAtom_p pmaHintSttsAtom,
					MP4StszAtom_p pmaHintStszAtom,
					MP4StblAtom_p pmaHintStblAtom,
					MP4StscAtom_p pmaHintStscAtom,
					MP4TkhdAtom_p pmaHintTkhdAtom,
					MP4MdhdAtom_p pmaHintMdhdAtom,
					MP4MvhdAtom_p pmaMvhdAtom,
					MP4StcoAtom_p pmaHintStcoAtom,
					MP4DmaxAtom_p pmaHintDmaxAtom,
					MP4MaxrAtom_p pmaHintMaxrAtom,
					RTPHintSample_p prhsRTPHintSample,
					unsigned long ulHintSampleIdentifier,
					__int64 ullHintDuration,
					unsigned long ulCurrentHintBytesNumber,
					MP4MdatAtom_p pmaMdatAtom,
					Boolean_t bIsSyncHintSample,
					unsigned long *pulAllChunksBufferSize,
					unsigned long *pulCurrentChunkBufferSize,
					unsigned long *pulCurrentChunkSamplesNumber,
					__int64 *pullCurrentChunkDuration,
					__int64 *pullThisSec,
					unsigned long *pulBytesThisSec,
					__int64 *pullCurrentChunkOffset,
					RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
					Tracer_p ptTracer);
			#else
				static Error appendHintSampleToMdatAtom (
					MP4RootAtom_p pmaRootAtom,
					MP4SttsAtom_p pmaHintSttsAtom,
					MP4StszAtom_p pmaHintStszAtom,
					MP4StblAtom_p pmaHintStblAtom,
					MP4StscAtom_p pmaHintStscAtom,
					MP4TkhdAtom_p pmaHintTkhdAtom,
					MP4MdhdAtom_p pmaHintMdhdAtom,
					MP4MvhdAtom_p pmaMvhdAtom,
					MP4StcoAtom_p pmaHintStcoAtom,
					MP4DmaxAtom_p pmaHintDmaxAtom,
					MP4MaxrAtom_p pmaHintMaxrAtom,
					RTPHintSample_p prhsRTPHintSample,
					unsigned long ulHintSampleIdentifier,
					unsigned long long ullHintDuration,
					unsigned long ulCurrentHintBytesNumber,
					MP4MdatAtom_p pmaMdatAtom,
					Boolean_t bIsSyncHintSample,
					unsigned long *pulAllChunksBufferSize,
					unsigned long *pulCurrentChunkBufferSize,
					unsigned long *pulCurrentChunkSamplesNumber,
					unsigned long long *pullCurrentChunkDuration,
					unsigned long long *pullThisSec,
					unsigned long *pulBytesThisSec,
					unsigned long long *pullCurrentChunkOffset,
					RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
					Tracer_p ptTracer);
			#endif

			static Error getUShortRandom (unsigned short *pusShortRandom);

			static Error getULongRandom (unsigned long *pulLongRandom);

			/**
				This method centralizes the definition of the hint time scale.
				Depend of the codec parameter, it does not use always all
				the parameters.
					pmaMediaTrakAtom is always used
					cCodecUsed is always used
					if cCodecUsed is MPEG4, the parameters used are:
						ulObjectTypeIndication
						sStandard
						pmaMediaMdhdAtom
					if cCodecUsed is H263, the parameters used are:
						<none>
					if cCodecUsed is AAC, the parameters used are:
						ulObjectTypeIndication
						sStandard
						pmaMediaTrakAtom
					if cCodecUsed is AMRNB or AMRWB, the parameters used are:
						<none>
			*/
			static Error getRTPTimeScale (
				MP4TrakAtom_p pmaMediaTrakAtom,
				MP4MdhdAtom_p pmaMediaMdhdAtom,
				MP4Atom:: MP4Codec_t cCodecUsed,
				unsigned long ulObjectTypeIndication,
				MP4Atom:: Standard_t sStandard,
				unsigned long *pulRTPTimeScale,
				Tracer_p ptTracer);

			static Error createHintTrack (
				MP4RootAtom_p pmaRootAtom,
				std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo,
				MP4TrackInfo_p pmtiMediaTrackInfo,
				unsigned long ulMaxPayloadSize, Boolean_t bInterleave,
				MP4TrackInfo_p *pmtiHintTrackInfo, Tracer_p ptTracer);

			static Error createAllHintTracks (
				MP4File_p pmfFile,
				unsigned long ulMaxPayloadSize,
				Tracer_p ptTracer);

			static Error allocRtpPayloadNumber (unsigned long *pulPayloadNumber,
				std:: vector<MP4TrackInfo *> *pvMP4TracksInfo,
				Tracer_p ptTracer);

	} RTPUtility_t, *RTPUtility_p;

#endif

