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


#ifndef MP4Utility_h
	#define MP4Utility_h

	#include "MP4CttsAtom.h"
	#include "MP4DmaxAtom.h"
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
	#include "MP4FileErrors.h"
	#include <vector>


	#define MP4F_NEWLINE					"\r\n"
	#define MP4F_MAXPATHNAMELENGTH			1024 + 1
	#define MP4F_MAXCODECUSEDLENGTH			255 + 1

	#define MP4F_SETDYNAMICPAYLOAD			255

	typedef class MP4Utility {

		private:
			MP4Utility (const MP4Utility &);

			MP4Utility &operator = (const MP4Utility &);

		public:

		public:
			MP4Utility ();

			~MP4Utility ();

			#ifdef WIN32
				static Error readFromMdatAtom (MP4RootAtom_p pmaRootAtom,
					__int64 ulFileOffset,
					unsigned char *pucBuffer, unsigned long ulBytesToRead,
					Tracer_p ptTracer);
			#else
				static Error readFromMdatAtom (MP4RootAtom_p pmaRootAtom,
					unsigned long long ulFileOffset,
					unsigned char *pucBuffer, unsigned long ulBytesToRead,
					Tracer_p ptTracer);
			#endif


			#ifdef WIN32
				static Error getPointerFromMdatAtom (MP4RootAtom_p pmaRootAtom,
					__int64 ullFileOffset,
					unsigned char **pucBuffer, __int64 *pullBytesToRead,
					Tracer_p ptTracer);
			#else
				static Error getPointerFromMdatAtom (MP4RootAtom_p pmaRootAtom,
					unsigned long long ullFileOffset,
					unsigned char **pucBuffer, unsigned long long *pullBytesToRead,
					Tracer_p ptTracer);
			#endif

			static Error getMaxSampleSize (
				MP4StszAtom_p pmaStszAtom, unsigned long *pulMaxSampleSize,
				unsigned long *pulSamplesNumber, Tracer_p ptTracer);

			static Error getSampleSize (
				MP4StszAtom_p pmaStszAtom, unsigned long ulSampleIdentifier,
				unsigned long *pulSampleSize, Boolean_p pbIsCommonSampleSize,
				Tracer_p ptTracer);

			static Error getSamplesNumber (
				MP4StszAtom_p pmaStszAtom, unsigned long *pulSamplesNumber,
				Tracer_p ptTracer);

			static Error getRenderingOffsetFromSampleNumber (
				MP4CttsAtom_p pmaCttsAtom, unsigned long ulSampleIdentifier,
				unsigned long *pulRenderingOffset, Tracer_p ptTracer);

			static Error isSyncSample (MP4StssAtom_p pmaStssAtom,
				unsigned long ulSampleIdentifier, Boolean_p pbIsSyncSample,
				Tracer_p ptTracer);

			static Error getPreviousSyncSample (MP4StssAtom_p pmaStssAtom,
				unsigned long ulSampleNumber,
				unsigned long *pulNewSampleNumber, Tracer_p ptTracer);

			static Error getChunkFirstLastSample (
				MP4StscAtom_p pmaStscAtom, unsigned long ulChunkNumber,
				unsigned long *pulFirstSample, unsigned long *pulLastSample,
				Tracer_p ptTracer);

			static Error getChunkOffsetFromChunkNumber (
				MP4StcoAtom_p pmaStcoAtom, unsigned long ulChunkNumber,
				unsigned long *pulChunkOffset, Tracer_p ptTracer);

			static Error updateSampleSizes (MP4StszAtom_p pmaStszAtom,
				unsigned long ulSampleIdentifier, unsigned long ulChunkBufferSize,
				Tracer_p ptTracer);

			#ifdef WIN32
				static Error updateSampleTimes (
					MP4SttsAtom_p pmaSttsAtom,
					__int64 ullHintDuration, Tracer_p ptTracer);
			#else
				static Error updateSampleTimes (
					MP4SttsAtom_p pmaSttsAtom,
					unsigned long long ullHintDuration, Tracer_p ptTracer);
			#endif

			#ifdef WIN32
				static Error updateRenderingOffsets (
					MP4StblAtom_p pmaStblAtom,
					unsigned long ulHintSampleIdentifier,
					__int64 ullRenderingOffset, Tracer_p ptTracer);
			#else
				static Error updateRenderingOffsets (
					MP4StblAtom_p pmaStblAtom,
					unsigned long ulHintSampleIdentifier,
					unsigned long long ullRenderingOffset, Tracer_p ptTracer);
			#endif

			static Error updateSyncSamples (
				MP4StblAtom_p pmaStblAtom,
				unsigned long ulSampleIdentifier, Boolean_t bIsSyncSample,
				Tracer_p ptTracer);

			static Error updateSampleToChunk (
				MP4StscAtom_p pmaStscAtom,
				unsigned long ulHintSampleIdentifier,
				unsigned long ulChunkIdentifier,
				unsigned long ulSamplesPerChunk, Tracer_p ptTracer);

			#ifdef WIN32
				static Error updateChunkOffsets (MP4StcoAtom_p pmaStcoAtom,
					__int64 ullChunkOffset, Tracer_p ptTracer);
			#else
				static Error updateChunkOffsets (MP4StcoAtom_p pmaStcoAtom,
					unsigned long long ullChunkOffset, Tracer_p ptTracer);
			#endif

			#ifdef WIN32
				static Error updateDurations (
					MP4TkhdAtom_p pmaTkhdAtom, MP4MdhdAtom_p pmaMdhdAtom,
					MP4MvhdAtom_p pmaMvhdAtom,
					__int64 ullHintDuration, Tracer_p ptTracer);
			#else
				static Error updateDurations (
					MP4TkhdAtom_p pmaTkhdAtom, MP4MdhdAtom_p pmaMdhdAtom,
					MP4MvhdAtom_p pmaMvhdAtom,
					unsigned long long ullHintDuration, Tracer_p ptTracer);
			#endif

			static Error updateModificationTimes (
				MP4MdhdAtom_p pmaMdhdAtom, MP4TkhdAtom_p pmaTkhdAtom,
				Tracer_p ptTracer);

			#ifdef WIN32
				static Error getPointerTrackESConfiguration (
					MP4TrakAtom_p pmaMediaTrakAtom, unsigned char **pucESConfiguration,
					__int64 *pullESConfigurationSize,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer);
			#else
				static Error getPointerTrackESConfiguration (
					MP4TrakAtom_p pmaMediaTrakAtom, unsigned char **pucESConfiguration,
					unsigned long long *pullESConfigurationSize,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer);
			#endif

			/*
			static Error buildSDPForMediaVideoTrack (
				MP4TrakAtom_p pmaMediaVideoTrack,
				MP4HdlrAtom_p pmaMediaHdlrAtom,
				MP4MdhdAtom_p pmaMediaMdhdAtom,
				MP4StszAtom_p pmaStszAtom,
				MP4Atom:: MP4Codec_t cCodecUsed,
				unsigned long ulObjectTypeIndication,
				unsigned long ulTrackIdentifier,
				unsigned long ulAvgBitRate,
				unsigned long ulPayloadNumber,
				MP4RootAtom_p pmaRootAtom,
				MP4Atom:: Standard_t sStandard,
				Buffer_p pbSDP,
				Tracer_p ptTracer);

			static Error buildSDPForMediaAudioTrack (
				MP4TrakAtom_p pmaMediaAudioTrack,
				MP4HdlrAtom_p pmaMediaHdlrAtom,
				MP4MdhdAtom_p pmaMediaMdhdAtom,
				MP4StszAtom_p pmaStszAtom,
				MP4Atom:: MP4Codec_t cCodecUsed,
				unsigned long ulObjectTypeIndication,
				unsigned long ulTrackIdentifier,
				unsigned long ulAvgBitRate,
				unsigned long ulPayloadNumber,
				MP4RootAtom_p pmaRootAtom,
				MP4Atom:: Standard_t sStandard,
				Buffer_p pbSDP,
				Tracer_p ptTracer);
			*/

			static Error getNewTrackIdentifier (MP4RootAtom_p pmaRootAtom,
				unsigned long *pulNewTrackIdentifier, Tracer_p ptTracer);

			static Error getCodecName (
				MP4Atom:: MP4Codec_t cCodec,
				char pCodecName [MP4F_MAXCODECUSEDLENGTH]);

	} MP4Utility_t, *MP4Utility_p;

#endif
