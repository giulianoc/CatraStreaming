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


#include "MP4DecoderConfigDescr.h"
#include "MP4DecoderSpecificDescr.h"
#include "MP4DrefAtom.h"
#include "MP4HintAtom.h"
#include "MP4InitialObjectDescr.h"
#include "MP4MdatAtom.h"
#include "MP4PaytAtom.h"
#include "MP4RtpAtom.h"
#include "MP4S263Atom.h"
#include "MP4SdpAtom.h"
#include "MP4SnroAtom.h"
#include "MP4StsdAtom.h"
#include "MP4TimsAtom.h"
#include "MP4TsroAtom.h"
#include "MP4UrlAtom.h"
#include "MP4Utility.h"
#include "RTPUtility.h"
#include "aac.h"
#include "Convert.h"
#include "mpeg4.h"
#include "rfc3016.h"
#include "rfc3267.h"
#include "rfc2429.h"
#include "rfcisma.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#ifdef WIN32
	#include <windows.h>
#else
	#include <sys/time.h>
	#include <netinet/in.h>
#endif


RTPUtility:: RTPUtility (void)

{

}


RTPUtility:: ~RTPUtility (void)

{

}



RTPUtility:: RTPUtility (const RTPUtility &)

{

	assert (1==0);

	// to do

}


RTPUtility &RTPUtility:: operator = (const RTPUtility &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error RTPUtility:: readSample (MP4RootAtom_p pmaRootAtom,
		MP4StscAtom_p pmaStscAtom, MP4StszAtom_p pmaStszAtom,
		MP4StcoAtom_p pmaStcoAtom, MP4SttsAtom_p pmaSttsAtom,
		MP4CttsAtom_p pmaCttsAtom, MP4StssAtom_p pmaStssAtom,
		unsigned long ulSampleIdentifier,
		unsigned char *pucSampleBuffer, unsigned long ulSampleBufferSize,
		unsigned long *pulCurrentSampleSize,
		__int64 *pullSampleStartTime,
		__int64 *pullSampleDuration,
		unsigned long *pulSampleRenderingOffset, Boolean_p pbIsSyncSample,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer)
#else
	Error RTPUtility:: readSample (MP4RootAtom_p pmaRootAtom,
		MP4StscAtom_p pmaStscAtom, MP4StszAtom_p pmaStszAtom,
		MP4StcoAtom_p pmaStcoAtom, MP4SttsAtom_p pmaSttsAtom,
		MP4CttsAtom_p pmaCttsAtom, MP4StssAtom_p pmaStssAtom,
		unsigned long ulSampleIdentifier,
		unsigned char *pucSampleBuffer, unsigned long ulSampleBufferSize,
		unsigned long *pulCurrentSampleSize,
		unsigned long long *pullSampleStartTime,
		unsigned long long *pullSampleDuration,
		unsigned long *pulSampleRenderingOffset, Boolean_p pbIsSyncSample,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer)
#endif

{

	#ifdef WIN32
		__int64								ullSampleFileOffset;
	#else
		unsigned long long					ullSampleFileOffset;
	#endif
	unsigned long						ulSampleDescriptionIndex;


	if (pmaRootAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getSampleInfoFromSampleNumber (
		pmaStscAtom, pmaStszAtom, pmaStcoAtom,
		ulSampleIdentifier,
		&ullSampleFileOffset, pulCurrentSampleSize,
		&ulSampleDescriptionIndex,
		prsrtRTPStreamRealTimeInfo,
		ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPUTILITY_GETSAMPLEINFOFROMSAMPLENUMBER_FAILED,
			1, ulSampleIdentifier);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pucSampleBuffer != (unsigned char *) NULL)
	{
		if (*pulCurrentSampleSize > ulSampleBufferSize)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_ACTIVATION_WRONG);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (MP4Utility:: readFromMdatAtom (pmaRootAtom, ullSampleFileOffset,
			pucSampleBuffer, *pulCurrentSampleSize, ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_READFROMMDATATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (getSampleTimeFromSampleNumber (pmaSttsAtom, ulSampleIdentifier,
		pullSampleStartTime, pullSampleDuration,
		prsrtRTPStreamRealTimeInfo, ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPUTILITY_GETSAMPLETIMEFROMSAMPLENUMBER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}

	if (pulSampleRenderingOffset != (unsigned long *) NULL)
	{
		if (MP4Utility:: getRenderingOffsetFromSampleNumber (
			pmaCttsAtom, ulSampleIdentifier, pulSampleRenderingOffset,
			ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_GETRENDERINGOFFSETFROMSAMPLENUMBER_FAILED,
				1, ulSampleIdentifier);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errNoError;
		}
	}

	if (pbIsSyncSample != (Boolean_p) NULL)
	{
		if (MP4Utility:: isSyncSample (pmaStssAtom, ulSampleIdentifier,
			pbIsSyncSample, ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_ISSYNCSAMPLE_FAILED,
				1, ulSampleIdentifier);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errNoError;
		}
	}

	if (pmaRootAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error RTPUtility:: getPointerToSample (MP4RootAtom_p pmaRootAtom,
		MP4StscAtom_p pmaStscAtom, MP4StszAtom_p pmaStszAtom,
		MP4StcoAtom_p pmaStcoAtom, MP4SttsAtom_p pmaSttsAtom,
		MP4CttsAtom_p pmaCttsAtom, MP4StssAtom_p pmaStssAtom,
		unsigned long ulSampleIdentifier,
		unsigned char **pucSampleBuffer, __int64 *pullSampleBufferToRead,
		unsigned long *pulCurrentSampleSize,
		__int64 *pullSampleStartTime,
		__int64 *pullSampleDuration,
		unsigned long *pulSampleRenderingOffset, Boolean_p pbIsSyncSample,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer)
#else
	Error RTPUtility:: getPointerToSample (MP4RootAtom_p pmaRootAtom,
		MP4StscAtom_p pmaStscAtom, MP4StszAtom_p pmaStszAtom,
		MP4StcoAtom_p pmaStcoAtom, MP4SttsAtom_p pmaSttsAtom,
		MP4CttsAtom_p pmaCttsAtom, MP4StssAtom_p pmaStssAtom,
		unsigned long ulSampleIdentifier,
		unsigned char **pucSampleBuffer, unsigned long long *pullSampleBufferToRead,
		unsigned long *pulCurrentSampleSize,
		unsigned long long *pullSampleStartTime,
		unsigned long long *pullSampleDuration,
		unsigned long *pulSampleRenderingOffset, Boolean_p pbIsSyncSample,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer)
#endif

{

	#ifdef WIN32
		__int64								ullSampleFileOffset;
	#else
		unsigned long long					ullSampleFileOffset;
	#endif
	unsigned long						ulSampleDescriptionIndex;


	if (pmaRootAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (getSampleInfoFromSampleNumber (
		pmaStscAtom, pmaStszAtom, pmaStcoAtom,
		ulSampleIdentifier,
		&ullSampleFileOffset, pulCurrentSampleSize,
		&ulSampleDescriptionIndex,
		prsrtRTPStreamRealTimeInfo,
		ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPUTILITY_GETSAMPLEINFOFROMSAMPLENUMBER_FAILED,
			1, ulSampleIdentifier);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pucSampleBuffer != (unsigned char **) NULL)
	{
		if (MP4Utility:: getPointerFromMdatAtom (pmaRootAtom,
			ullSampleFileOffset, pucSampleBuffer, pullSampleBufferToRead,
			ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_GETPOINTERFROMMDATATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (*pulCurrentSampleSize > *pullSampleBufferToRead)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_SAMPLESIZENOTCONSISTENT);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (getSampleTimeFromSampleNumber (pmaSttsAtom, ulSampleIdentifier,
		pullSampleStartTime, pullSampleDuration,
		prsrtRTPStreamRealTimeInfo, ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPUTILITY_GETSAMPLETIMEFROMSAMPLENUMBER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}

	if (pulSampleRenderingOffset != (unsigned long *) NULL)
	{
		if (MP4Utility:: getRenderingOffsetFromSampleNumber (
			pmaCttsAtom, ulSampleIdentifier, pulSampleRenderingOffset,
			ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_GETRENDERINGOFFSETFROMSAMPLENUMBER_FAILED,
				1, ulSampleIdentifier);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errNoError;
		}
	}

	if (pbIsSyncSample != (Boolean_p) NULL)
	{
		if (MP4Utility:: isSyncSample (pmaStssAtom, ulSampleIdentifier,
			pbIsSyncSample, ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_ISSYNCSAMPLE_FAILED,
				1, ulSampleIdentifier);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errNoError;
		}
	}

	if (pmaRootAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error RTPUtility:: appendToRTPPacketFromMdatAtom (
		MP4RootAtom_p pmaRootAtom, __int64 ullFileOffset,
		RTPPacket_p prpRTPPacket, __int64 llBytesToRead,
		Tracer_p ptTracer)
#else
	Error RTPUtility:: appendToRTPPacketFromMdatAtom (
		MP4RootAtom_p pmaRootAtom, unsigned long long ullFileOffset,
		RTPPacket_p prpRTPPacket, long long llBytesToRead,
		Tracer_p ptTracer)
#endif

{

	char				pAtomPath [MP4F_MAXPATHNAMELENGTH];
	unsigned long		ulMdatIndex;
	MP4MdatAtom_p		pmaMdatAtom;
	unsigned char		*pucData;
	Error_t				errGetData;
	MP4Atom_p			pmaAtom;


	if (pmaRootAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	for (ulMdatIndex = 0; ; ulMdatIndex++)
	{
		sprintf (pAtomPath, "mdat:%lu", ulMdatIndex);

		if (pmaRootAtom -> searchAtom (pAtomPath, false,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mdat");
			// ptTracer -> trace (Tracer:: TRACER_LERRR,
			// 	(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			pmaMdatAtom		= (MP4MdatAtom_p) pmaAtom;

		if ((errGetData = pmaMdatAtom -> getPointerToData (
			ullFileOffset, llBytesToRead, &pucData)) != errNoError)
		{
			if ((long) errGetData != MP4F_ACTIVATION_WRONG)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4MDATATOM_GETPOINTERTODATA_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaRootAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			if (prpRTPPacket -> appendData (
				pucData, llBytesToRead) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPPACKET_APPENDDATA_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaRootAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			break;
		}
		/*
		if ((errGetData = pmaMdatAtom -> appendDataToRTPPacket (
			ullFileOffset, llBytesToRead, prpRTPPacket)) != errNoError)
		{
			if ((long) errGetData != MP4F_ACTIVATION_WRONG)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4MDATATOM_APPENDDATATORTPPACKET_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaRootAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
			break;
		*/
	}

	if (pmaRootAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error RTPUtility:: getSampleInfoFromSampleNumber (
		MP4StscAtom_p pmaStscAtom, MP4StszAtom_p pmaStszAtom,
		MP4StcoAtom_p pmaStcoAtom,
		unsigned long ulSampleIdentifier,
		__int64 *pullSampleFileOffset, unsigned long *pulSampleSize,
		unsigned long *pulSampleDescriptionIndex,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer)
#else
	Error RTPUtility:: getSampleInfoFromSampleNumber (
		MP4StscAtom_p pmaStscAtom, MP4StszAtom_p pmaStszAtom,
		MP4StcoAtom_p pmaStcoAtom,
		unsigned long ulSampleIdentifier,
		unsigned long long *pullSampleFileOffset, unsigned long *pulSampleSize,
		unsigned long *pulSampleDescriptionIndex,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer)
#endif

{

	unsigned long			ulSamplesPerChunk;
	unsigned long			ulChunkNumber;
	unsigned long			ulSampleOffsetInChunk;
	Boolean_t				bIsCommonSampleSize;


	if (pmaStscAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// pulSampleDescriptionIndex
	if (getChunkInfoFromSampleNumber (pmaStscAtom,
		ulSampleIdentifier, &ulSamplesPerChunk, &ulChunkNumber,
		pulSampleDescriptionIndex, &ulSampleOffsetInChunk,
		prsrtRTPStreamRealTimeInfo, ptTracer) !=
		errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPUTILITY_GETCHUNKINFOFROMSAMPLENUMBER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStscAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// pulSampleSize
	if (MP4Utility:: getSampleSize (pmaStszAtom, ulSampleIdentifier,
		pulSampleSize, &bIsCommonSampleSize, ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UTILITY_GETSAMPLESIZE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStscAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// pullSampleFileOffset
	{

		unsigned long			ulChunkOffset;


		if (MP4Utility:: getChunkOffsetFromChunkNumber (pmaStcoAtom,
			ulChunkNumber, &ulChunkOffset, ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_GETCHUNKOFFSETFROMCHUNKNUMBER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStscAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		{
			unsigned long		ulSampleRangeSize;


			if (getSampleRangeSize (
				pmaStszAtom, ulSampleIdentifier - ulSampleOffsetInChunk,
				ulSampleIdentifier, &ulSampleRangeSize,
				prsrtRTPStreamRealTimeInfo, ptTracer) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPUTILITY_GETSAMPLERANGESIZE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStscAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			*pullSampleFileOffset		= ulSampleRangeSize + ulChunkOffset;
		}

		/* vechio non performante metodo
		if (!bIsCommonSampleSize)
		{
			unsigned long			ulSampleIndex;
			unsigned long			ulLocalSampleSize;

			*pullSampleFileOffset			= ulChunkOffset;

			for (ulSampleIndex = ulSampleIdentifier - ulSampleOffsetInChunk;
				ulSampleIndex < ulSampleIdentifier; ulSampleIndex++)
			{
				if (getSampleSize (pmaStszAtom, ulSampleIndex,
					&ulLocalSampleSize, &bIsCommonSampleSize, ptTracer) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4UTILITY_GETSAMPLESIZE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pmaStscAtom -> unLockMP4File () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				(*pullSampleFileOffset)			+= ulLocalSampleSize;
			}
		}
		else
		{
			*pullSampleFileOffset			= ulChunkOffset +
				((*pulSampleSize) * ulSampleOffsetInChunk);
		}
		*/
	}

	if (pmaStscAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPUtility:: getSampleRangeSize (
	MP4StszAtom_p pmaStszAtom, unsigned long ulFirstSampleNumber,
	unsigned long ulLastSampleNumber, unsigned long *pulSampleRangeSize,
	RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo, Tracer_p ptTracer)

{

	/*
	MP4StszAtom_p				pmsaStszAtom;
	MP4Atom_p					pmaAtom;
	*/
	unsigned long				ulCommonSampleSize;
	unsigned long				ulSampleSize;
	unsigned long				ulSampleIndex;


	if (pmaStszAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (ulLastSampleNumber == (unsigned long) -1)
	{
		if (MP4Utility:: getSamplesNumber (pmaStszAtom,
			&ulLastSampleNumber, ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_GETSAMPLESNUMBER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStszAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		ulLastSampleNumber			+= 1;
	}

	if ((prsrtRTPStreamRealTimeInfo ->
		getSampleRangeSizeFromFirstAndLastSample).
		ulFirstSampleNumber_IN == ulFirstSampleNumber &&
		(prsrtRTPStreamRealTimeInfo ->
		getSampleRangeSizeFromFirstAndLastSample).
		ulLastSampleNumber_IN == ulLastSampleNumber)
	{
		*pulSampleRangeSize			=
			(prsrtRTPStreamRealTimeInfo ->
			getSampleRangeSizeFromFirstAndLastSample).
			ulSampleRangeSize_OUT;

		if (pmaStszAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}

	if ((prsrtRTPStreamRealTimeInfo ->
		getSampleRangeSizeFromFirstAndLastSample).
		ulFirstSampleNumber_IN != ulFirstSampleNumber ||
		(prsrtRTPStreamRealTimeInfo ->
		getSampleRangeSizeFromFirstAndLastSample).
		ulLastSampleNumber_IN > ulLastSampleNumber)
	{
		prsrtRTPStreamRealTimeInfo ->
			resetGetSampleRangeSizeFromFirstAndLastSample (
			ulFirstSampleNumber);
	}

	/*
	{
		char aaa [1024];
		sprintf (aaa, "First: %5lu, Last: %5lu",
			ulFirstSampleNumber, ulLastSampleNumber);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			aaa, __FILE__, __LINE__);
	}
	*/

	/*
	if (pmaTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:stsz:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "mdia:0:minf:0:stbl:0:stsz:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmsaStszAtom		= (MP4StszAtom_p) pmaAtom;
	*/

	if (pmaStszAtom -> getCommonSampleSize (&ulCommonSampleSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStszAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulCommonSampleSize == 0)
	{
		*pulSampleRangeSize					=
			(prsrtRTPStreamRealTimeInfo ->
			getSampleRangeSizeFromFirstAndLastSample).
			ulSampleRangeSize_OUT;

		ulSampleIndex						=
			(prsrtRTPStreamRealTimeInfo ->
			getSampleRangeSizeFromFirstAndLastSample).
			ulLastSampleNumber_IN;

		for (;
			ulSampleIndex < ulLastSampleNumber; ulSampleIndex++)
		{
			if (pmaStszAtom -> getSampleSizeTable (ulSampleIndex - 1,
				&ulSampleSize) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStszAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			(*pulSampleRangeSize)				+= ulSampleSize;
		}
	}
	else
	{
		*pulSampleRangeSize						=
			ulCommonSampleSize * (ulLastSampleNumber - ulFirstSampleNumber);
	}

	(prsrtRTPStreamRealTimeInfo -> getSampleRangeSizeFromFirstAndLastSample).
		ulSampleRangeSize_OUT			= *pulSampleRangeSize;

	(prsrtRTPStreamRealTimeInfo -> getSampleRangeSizeFromFirstAndLastSample).
		ulFirstSampleNumber_IN		= ulFirstSampleNumber;
	(prsrtRTPStreamRealTimeInfo -> getSampleRangeSizeFromFirstAndLastSample).
		ulLastSampleNumber_IN		= ulLastSampleNumber;

	if (pmaStszAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


/*
#ifdef WIN32
	Error RTPUtility:: getSampleNumberFromSampleTime (
		MP4SttsAtom_p pmaSttsAtom, __int64 ullSampleTime,
		unsigned long *pulSampleNumber, Tracer_p ptTracer)
#else
	Error RTPUtility:: getSampleNumberFromSampleTime (
		MP4SttsAtom_p pmaSttsAtom, unsigned long long ullSampleTime,
		unsigned long *pulSampleNumber, Tracer_p ptTracer)
#endif

{

	unsigned long				ulEntriesNumber;
	unsigned long				ulEntryIndex;
	unsigned long				ulSampleCount;
	unsigned long				ulSampleDuration;
	unsigned long				ulCurrentSample;
	#ifdef WIN32
		__int64						ullCurrentSampleTime;
	#else
		unsigned long long			ullCurrentSampleTime;
	#endif


	if (pmaSttsAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmaSttsAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	ulCurrentSample						= 1;
	ullCurrentSampleTime					= 0;

	for (ulEntryIndex = 0; ulEntryIndex < ulEntriesNumber; ulEntryIndex++)
	{
		if (pmaSttsAtom -> getTimeToSampleTable (2 * ulEntryIndex,
			&ulSampleCount) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaSttsAtom -> getTimeToSampleTable (2 * ulEntryIndex + 1,
			&ulSampleDuration) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (ullCurrentSampleTime + (ulSampleDuration * ulSampleCount) <
			ullSampleTime)
		{
			ullCurrentSampleTime			+= (ulSampleDuration * ulSampleCount);
			ulCurrentSample					+= ulSampleCount;
		}
		else
		{
			*pulSampleNumber				= ulCurrentSample +
				((ullSampleTime - ullCurrentSampleTime) / ulSampleDuration);

			break;
		}
	}

	if (ulEntryIndex == ulEntriesNumber)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UTILITY_MEDIATIMETOOHIGH,
			1, ullSampleTime);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaSttsAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}
*/


#ifdef WIN32
	Error RTPUtility:: getSampleNumberFromSampleTime (
		MP4SttsAtom_p pmaSttsAtom, __int64 ullSampleTime,
		unsigned long *pulSampleNumber,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer)
#else
	Error RTPUtility:: getSampleNumberFromSampleTime (
		MP4SttsAtom_p pmaSttsAtom, unsigned long long ullSampleTime,
		unsigned long *pulSampleNumber,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer)
#endif

{

	if ((prsrtRTPStreamRealTimeInfo -> getSampleNumberFromSampleTime).
		_ullSampleTime_IN == ullSampleTime)
	{
		*pulSampleNumber			=
			(prsrtRTPStreamRealTimeInfo -> getSampleNumberFromSampleTime).
			_ulSampleNumber_OUT;

		return errNoError;
	}

	{
		/*
		MP4SttsAtom_p				pmsaSttsAtom;
		MP4Atom_p					pmaAtom;
		*/
		unsigned long				ulEntriesNumber;
		unsigned long				ulEntryIndex;
		unsigned long				ulSampleCount;
		unsigned long				ulSampleDuration;
		unsigned long				ulCurrentSampleIdentifier;
		#ifdef WIN32
			__int64						ullCurrentSampleStartTime;
		#else
			unsigned long long			ullCurrentSampleStartTime;
		#endif


		if (pmaSttsAtom -> lockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		/*
		if (pmaTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:stts:0", true,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mdia:0:minf:0:stbl:0:stts:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			pmsaSttsAtom		= (MP4SttsAtom_p) pmaAtom;
		*/

		if (pmaSttsAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if ((prsrtRTPStreamRealTimeInfo -> getSampleNumberFromSampleTime).
			_ullSampleTime_IN > ullSampleTime)
		{
			prsrtRTPStreamRealTimeInfo -> resetGetSampleNumberFromSampleTime ();
		}

		ulEntryIndex						=
			(prsrtRTPStreamRealTimeInfo -> getSampleNumberFromSampleTime). _ulEntryIndex_Internal;
		ulCurrentSampleIdentifier			=
			(prsrtRTPStreamRealTimeInfo -> getSampleNumberFromSampleTime). _ulCurrentSampleIdentifier_Internal;
		ullCurrentSampleStartTime			=
			(prsrtRTPStreamRealTimeInfo -> getSampleNumberFromSampleTime). _ullCurrentSampleStartTime_Internal;

		for (; ulEntryIndex < ulEntriesNumber; ulEntryIndex++)
		{
			if (pmaSttsAtom -> getTimeToSampleTable (2 * ulEntryIndex,
				&ulSampleCount) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				prsrtRTPStreamRealTimeInfo -> resetGetSampleNumberFromSampleTime ();

				if (pmaSttsAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaSttsAtom -> getTimeToSampleTable (2 * ulEntryIndex + 1,
				&ulSampleDuration) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				prsrtRTPStreamRealTimeInfo -> resetGetSampleNumberFromSampleTime ();

				if (pmaSttsAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (ullCurrentSampleStartTime + (ulSampleDuration * ulSampleCount) <
				ullSampleTime)
			{
				ullCurrentSampleStartTime		+= (ulSampleDuration * ulSampleCount);
				ulCurrentSampleIdentifier		+= ulSampleCount;
			}
			else
			{
				*pulSampleNumber				= ulCurrentSampleIdentifier +
					((ullSampleTime - ullCurrentSampleStartTime) / ulSampleDuration);

				break;
			}
		}

		if (ulEntryIndex == ulEntriesNumber)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_MEDIATIMETOOHIGH,
				1, ullSampleTime);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			prsrtRTPStreamRealTimeInfo -> resetGetSampleNumberFromSampleTime ();

			if (pmaSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		(prsrtRTPStreamRealTimeInfo -> getSampleNumberFromSampleTime).
			_ulSampleNumber_OUT					= *pulSampleNumber;

		(prsrtRTPStreamRealTimeInfo -> getSampleNumberFromSampleTime). _ullSampleTime_IN	=
			ullSampleTime;

		(prsrtRTPStreamRealTimeInfo -> getSampleNumberFromSampleTime). _ulEntryIndex_Internal	=
			ulEntryIndex;
		(prsrtRTPStreamRealTimeInfo -> getSampleNumberFromSampleTime). _ulCurrentSampleIdentifier_Internal	=
			ulCurrentSampleIdentifier;
		(prsrtRTPStreamRealTimeInfo -> getSampleNumberFromSampleTime). _ullCurrentSampleStartTime_Internal	=
			ullCurrentSampleStartTime;

		if (pmaSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


/*
#ifdef WIN32
	Error RTPUtility:: getSampleTimeFromSampleNumber (
		MP4SttsAtom_p pmaSttsAtom, unsigned long ulSampleIdentifier,
		__int64 *pullSampleStartTime,
		__int64 *pullSampleDuration, Tracer_p ptTracer)
#else
	Error RTPUtility:: getSampleTimeFromSampleNumber (
		MP4SttsAtom_p pmaSttsAtom, unsigned long ulSampleIdentifier,
		unsigned long long *pullSampleStartTime,
		unsigned long long *pullSampleDuration, Tracer_p ptTracer)
#endif

{

	unsigned long				ulEntriesNumber;
	unsigned long				ulEntryIndex;
	unsigned long				ulSampleCount;
	unsigned long				ulSampleDuration;
	unsigned long				ulCurrentSampleIdentifier;
	unsigned long				ulCurrentSampleStartTime;


	if (pmaSttsAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	#ifdef WIN32
		if (pullSampleStartTime == (__int64 *) NULL ||
			pullSampleDuration == (__int64 *) NULL)
	#else
		if (pullSampleStartTime == (unsigned long long *) NULL ||
			pullSampleDuration == (unsigned long long *) NULL)
	#endif
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaSttsAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	ulCurrentSampleIdentifier			= 1;
	ulCurrentSampleStartTime			= 0;

	for (ulEntryIndex = 0; ulEntryIndex < ulEntriesNumber; ulEntryIndex++)
	{
		if (pmaSttsAtom -> getTimeToSampleTable (2 * ulEntryIndex,
			&ulSampleCount) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaSttsAtom -> getTimeToSampleTable (2 * ulEntryIndex + 1,
			&ulSampleDuration) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (ulSampleIdentifier <= ulCurrentSampleIdentifier + ulSampleCount - 1)
		{
			break;
		}
		else
		{
			ulCurrentSampleIdentifier			+= ulSampleCount;
			ulCurrentSampleStartTime			+=
				(ulSampleCount * ulSampleDuration);
		}
	}

	if (ulEntryIndex == ulEntriesNumber)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UTILITY_SAMPLEIDENTIFIERTOOHIGH);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	*pullSampleStartTime		= ulCurrentSampleStartTime +
		((ulSampleIdentifier - ulCurrentSampleIdentifier) * ulSampleDuration);
	*pullSampleDuration			= ulSampleDuration;

	if (pmaSttsAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}
*/


#ifdef WIN32
	Error RTPUtility:: getSampleTimeFromSampleNumber (
		MP4SttsAtom_p pmaSttsAtom, unsigned long ulSampleIdentifier,
		__int64 *pullSampleStartTime,
		__int64 *pullSampleDuration,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer)
#else
	Error RTPUtility:: getSampleTimeFromSampleNumber (
		MP4SttsAtom_p pmaSttsAtom, unsigned long ulSampleIdentifier,
		unsigned long long *pullSampleStartTime,
		unsigned long long *pullSampleDuration,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer)
#endif

{

	#ifdef WIN32
		if (pullSampleStartTime == (__int64 *) NULL ||
			pullSampleDuration == (__int64 *) NULL)
	#else
		if (pullSampleStartTime == (unsigned long long *) NULL ||
			pullSampleDuration == (unsigned long long *) NULL)
	#endif
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((prsrtRTPStreamRealTimeInfo -> getSampleTimeFromSampleNumber).
		_ulSampleIdentifier_IN == ulSampleIdentifier)
	{
		*pullSampleStartTime			=
			(prsrtRTPStreamRealTimeInfo -> getSampleTimeFromSampleNumber).
			_ullSampleStartTime_OUT;
		*pullSampleDuration					=
			(prsrtRTPStreamRealTimeInfo -> getSampleTimeFromSampleNumber).
			_ullSampleDuration_OUT;

		return errNoError;
	}

	{
		unsigned long				ulEntriesNumber;
		unsigned long				ulEntryIndex;
		unsigned long				ulSampleCount;
		unsigned long				ulSampleDuration;
		unsigned long				ulCurrentSampleIdentifier;
		unsigned long				ulCurrentSampleStartTime;


		if (pmaSttsAtom -> lockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pmaSttsAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if ((prsrtRTPStreamRealTimeInfo -> getSampleTimeFromSampleNumber).
			_ulSampleIdentifier_IN > ulSampleIdentifier)
		{
			prsrtRTPStreamRealTimeInfo -> resetGetSampleTimeFromSampleNumber ();
		}

		ulEntryIndex						=
			(prsrtRTPStreamRealTimeInfo -> getSampleTimeFromSampleNumber). _ulEntryIndex_Internal;
		ulCurrentSampleIdentifier			=
			(prsrtRTPStreamRealTimeInfo -> getSampleTimeFromSampleNumber). _ulCurrentSampleIdentifier_Internal;
		ulCurrentSampleStartTime			=
			(prsrtRTPStreamRealTimeInfo -> getSampleTimeFromSampleNumber). _ulCurrentSampleStartTime_Internal;

		for (; ulEntryIndex < ulEntriesNumber; ulEntryIndex++)
		{
			if (pmaSttsAtom -> getTimeToSampleTable (2 * ulEntryIndex,
				&ulSampleCount) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				prsrtRTPStreamRealTimeInfo -> resetGetSampleTimeFromSampleNumber ();

				if (pmaSttsAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaSttsAtom -> getTimeToSampleTable (2 * ulEntryIndex + 1,
				&ulSampleDuration) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				prsrtRTPStreamRealTimeInfo -> resetGetSampleTimeFromSampleNumber ();

				if (pmaSttsAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (ulSampleIdentifier <= ulCurrentSampleIdentifier + ulSampleCount - 1)
			{
				break;
			}
			else
			{
				ulCurrentSampleIdentifier			+= ulSampleCount;
				ulCurrentSampleStartTime			+=
					(ulSampleCount * ulSampleDuration);
			}
		}

		if (ulEntryIndex == ulEntriesNumber)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_SAMPLEIDENTIFIERTOOHIGH);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			prsrtRTPStreamRealTimeInfo -> resetGetSampleTimeFromSampleNumber ();

			if (pmaSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		*pullSampleStartTime		= ulCurrentSampleStartTime +
			((ulSampleIdentifier - ulCurrentSampleIdentifier) * ulSampleDuration);
		*pullSampleDuration			= ulSampleDuration;

		(prsrtRTPStreamRealTimeInfo -> getSampleTimeFromSampleNumber).
			_ullSampleStartTime_OUT			= *pullSampleStartTime;
		(prsrtRTPStreamRealTimeInfo -> getSampleTimeFromSampleNumber).
			_ullSampleDuration_OUT			= *pullSampleDuration;

		(prsrtRTPStreamRealTimeInfo -> getSampleTimeFromSampleNumber). _ulSampleIdentifier_IN	=
			ulSampleIdentifier;

		(prsrtRTPStreamRealTimeInfo -> getSampleTimeFromSampleNumber). _ulEntryIndex_Internal	=
			ulEntryIndex;
		(prsrtRTPStreamRealTimeInfo -> getSampleTimeFromSampleNumber). _ulCurrentSampleIdentifier_Internal	=
			ulCurrentSampleIdentifier;
		(prsrtRTPStreamRealTimeInfo -> getSampleTimeFromSampleNumber). _ulCurrentSampleStartTime_Internal	=
			ulCurrentSampleStartTime;

		if (pmaSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


/*
Error RTPUtility:: getChunkInfoFromSampleNumber (
	MP4StscAtom_p pmaStscAtom,
	unsigned long ulSampleNumber,					// start from 1
	unsigned long *pulSamplesPerChunk,
	unsigned long *pulChunkNumber,				// start from 1
	unsigned long *pulSampleDescriptionIndex,
	unsigned long *pulSampleOffsetInChunk,		// start from 0
	Tracer_p ptTracer)

{

	unsigned long				ulEntriesNumber;
	unsigned long				ulEntryIndex;
	unsigned long				ulFirstChunk;
	unsigned long				ulSamplesPerChunk;
	unsigned long				ulSampleDescriptionIndex;
	unsigned long				ulCurrentSample;
	unsigned long				ulLastFirstChunk;
	unsigned long				ulLastSamplesPerChunk;
	unsigned long				ulLastSampleDescriptionIndex;


	if (pmaStscAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmaStscAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStscAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	ulCurrentSample						= 1;
	ulLastFirstChunk					= 1;
	ulLastSamplesPerChunk				= 1;
	ulLastSampleDescriptionIndex		= 0;

	for (ulEntryIndex = 0; ulEntryIndex < ulEntriesNumber; ulEntryIndex++)
	{
		if (pmaStscAtom -> getSampleToChunkTable (3 * ulEntryIndex,
			&ulFirstChunk) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STSCATOM_GETSAMPLETOCHUNKTABLE_FAILED,
				1, (long) (3 * ulEntryIndex));
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStscAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaStscAtom -> getSampleToChunkTable (3 * ulEntryIndex + 1,
			&ulSamplesPerChunk) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STSCATOM_GETSAMPLETOCHUNKTABLE_FAILED,
				1, (long) (3 * ulEntryIndex + 1));
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStscAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaStscAtom -> getSampleToChunkTable (3 * ulEntryIndex + 2,
			&ulSampleDescriptionIndex) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STSCATOM_GETSAMPLETOCHUNKTABLE_FAILED,
				1, (long) (3 * ulEntryIndex + 2));
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStscAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		ulCurrentSample			= ulCurrentSample +
			(ulFirstChunk - ulLastFirstChunk) * ulLastSamplesPerChunk;

		if (ulSampleNumber < ulCurrentSample)
		{
			if (pmaStscAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errNoError;
		}

		*pulChunkNumber				= ulFirstChunk +
			((ulSampleNumber - ulCurrentSample) / ulSamplesPerChunk);
		*pulSampleDescriptionIndex	= ulSampleDescriptionIndex;
		*pulSampleOffsetInChunk		= ulSampleNumber - (ulCurrentSample +
			(((*pulChunkNumber) - ulFirstChunk) * ulSamplesPerChunk));
		*pulSamplesPerChunk			= ulSamplesPerChunk;

		ulLastFirstChunk				= ulFirstChunk;
		ulLastSampleDescriptionIndex	= ulSampleDescriptionIndex;
		ulLastSamplesPerChunk			= ulSamplesPerChunk;
	}

	if (pmaStscAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}
*/

Error RTPUtility:: getChunkInfoFromSampleNumber (
	MP4StscAtom_p pmaStscAtom,
	unsigned long ulSampleNumber,					// start from 1
	unsigned long *pulSamplesPerChunk,
	unsigned long *pulChunkNumber,				// start from 1
	unsigned long *pulSampleDescriptionIndex,
	unsigned long *pulSampleOffsetInChunk,		// start from 0
	RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
	Tracer_p ptTracer)

{

	if ((prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
		_ulSampleNumber_IN == ulSampleNumber)
	{
		*pulSamplesPerChunk				=
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
			_ulSamplesPerChunk_OUT;
		*pulChunkNumber					=
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
			_ulChunkNumber_OUT;
		*pulSampleDescriptionIndex		=
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
			_ulSampleDescriptionIndex_OUT;
		*pulSampleOffsetInChunk			=
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
			_ulSampleOffsetInChunk_OUT;

		return errNoError;
	}

	{
		unsigned long				ulEntriesNumber;
		unsigned long				ulEntryIndex;
		unsigned long				ulFirstChunk;
		unsigned long				ulSamplesPerChunk;
		unsigned long				ulSampleDescriptionIndex;
		unsigned long				ulCurrentSample;
		unsigned long				ulPrevCurrentSample;
		unsigned long				ulLastFirstChunk;
		unsigned long				ulLastSamplesPerChunk;
		unsigned long				ulLastSampleDescriptionIndex;
	//	MP4Atom_p					pmaAtom;


		if (pmaStscAtom -> lockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		/*
		if (pmaTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:stsc:0", true,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mdia:0:minf:0:stbl:0:stsc:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			pmaStscAtom		= (MP4StscAtom_p) pmaAtom;
		*/

		if (pmaStscAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStscAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if ((prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
			_ulSampleNumber_IN > ulSampleNumber)
		{
			prsrtRTPStreamRealTimeInfo -> resetGetChunkInfoFromSampleNumber ();
		}
		else
		{
			// E' necessario aggiornare _ulChunkNumber_OUT e _ulSampleOffsetInChunk_OUT perchè
			//	dipendono da ulSampleNumber (input)
			//	e nel caso non entra nel loop perchè ulEntryIndex >= ulEntriesNumber (ultimo chunk)
			//	non verrebbero aggiornati
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber). _ulChunkNumber_OUT			=
				(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber). _ulLastFirstChunk_Internal +
				((ulSampleNumber - (prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber). _ulCurrentSample_Internal) /
				(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber). _ulLastSamplesPerChunk_Internal);
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber). _ulSampleOffsetInChunk_OUT	=
				ulSampleNumber - ((prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber). _ulCurrentSample_Internal +
				((((prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber). _ulChunkNumber_OUT) -
				(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber). _ulLastFirstChunk_Internal) *
				(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber). _ulLastSamplesPerChunk_Internal));

			// E' necessario aggiornare le variabili di output perchè altrimenti non
			//	sarebbero inizializzate nel caso in cui si entra nell'if all'interno
			//	del loop nella prima iterazione
			*pulChunkNumber				= (prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
				_ulChunkNumber_OUT;
			*pulSampleDescriptionIndex	= (prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
				_ulSampleDescriptionIndex_OUT;
			*pulSampleOffsetInChunk		= (prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
				_ulSampleOffsetInChunk_OUT;
			*pulSamplesPerChunk			= (prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
				_ulSamplesPerChunk_OUT;
		}

		ulEntryIndex						=
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber). _ulEntryIndex_Internal;
		ulCurrentSample						=
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber). _ulCurrentSample_Internal;
		ulLastFirstChunk					=
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber). _ulLastFirstChunk_Internal;
		ulLastSamplesPerChunk				=
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber). _ulLastSamplesPerChunk_Internal;
		ulLastSampleDescriptionIndex		=
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber). _ulLastSampleDescriptionIndex_Internal;

		for (; ulEntryIndex < ulEntriesNumber; ulEntryIndex++)
		{
			if (pmaStscAtom -> getSampleToChunkTable (3 * ulEntryIndex,
				&ulFirstChunk) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4STSCATOM_GETSAMPLETOCHUNKTABLE_FAILED,
					1, (long) (3 * ulEntryIndex));
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				prsrtRTPStreamRealTimeInfo -> resetGetChunkInfoFromSampleNumber ();

				if (pmaStscAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaStscAtom -> getSampleToChunkTable (3 * ulEntryIndex + 1,
				&ulSamplesPerChunk) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4STSCATOM_GETSAMPLETOCHUNKTABLE_FAILED,
					1, (long) (3 * ulEntryIndex + 1));
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				prsrtRTPStreamRealTimeInfo -> resetGetChunkInfoFromSampleNumber ();

				if (pmaStscAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaStscAtom -> getSampleToChunkTable (3 * ulEntryIndex + 2,
				&ulSampleDescriptionIndex) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4STSCATOM_GETSAMPLETOCHUNKTABLE_FAILED,
					1, (long) (3 * ulEntryIndex + 2));
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				prsrtRTPStreamRealTimeInfo -> resetGetChunkInfoFromSampleNumber ();

				if (pmaStscAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			ulPrevCurrentSample		= ulCurrentSample;
			ulCurrentSample			= ulCurrentSample +
				(ulFirstChunk - ulLastFirstChunk) * ulLastSamplesPerChunk;

			if (ulSampleNumber < ulCurrentSample)
			{
				if (pmaStscAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					prsrtRTPStreamRealTimeInfo -> resetGetChunkInfoFromSampleNumber ();

					return err;
				}

				(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
					_ulSampleNumber_IN			= ulSampleNumber;

				(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
					_ulEntryIndex_Internal		= ulEntryIndex;
				(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
					_ulCurrentSample_Internal	= ulPrevCurrentSample;

				return errNoError;
			}

			*pulChunkNumber				= ulFirstChunk +
				((ulSampleNumber - ulCurrentSample) / ulSamplesPerChunk);
			*pulSampleDescriptionIndex	= ulSampleDescriptionIndex;
			*pulSampleOffsetInChunk		= ulSampleNumber - (ulCurrentSample +
				(((*pulChunkNumber) - ulFirstChunk) * ulSamplesPerChunk));
			*pulSamplesPerChunk			= ulSamplesPerChunk;

			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
				_ulSamplesPerChunk_OUT		= *pulSamplesPerChunk;
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
				_ulChunkNumber_OUT			= *pulChunkNumber;
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
				_ulSampleDescriptionIndex_OUT	= *pulSampleDescriptionIndex;
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
				_ulSampleOffsetInChunk_OUT		= *pulSampleOffsetInChunk;

			ulLastFirstChunk				= ulFirstChunk;
			ulLastSampleDescriptionIndex	= ulSampleDescriptionIndex;
			ulLastSamplesPerChunk			= ulSamplesPerChunk;

			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
				_ulLastFirstChunk_Internal	= ulLastFirstChunk;
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
				_ulLastSamplesPerChunk_Internal		= ulLastSamplesPerChunk;
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
				_ulLastSampleDescriptionIndex_Internal	= ulLastSampleDescriptionIndex;
		}

		// before to exit
		(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
			_ulSampleNumber_IN			= ulSampleNumber;

		*pulSamplesPerChunk			=
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
			_ulSamplesPerChunk_OUT;
		*pulChunkNumber				=
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
			_ulChunkNumber_OUT;
		*pulSampleDescriptionIndex	=
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
			_ulSampleDescriptionIndex_OUT;
		*pulSampleOffsetInChunk		=
			(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
			_ulSampleOffsetInChunk_OUT;

		(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
			_ulEntryIndex_Internal		= ulEntryIndex;
		(prsrtRTPStreamRealTimeInfo -> getChunkInfoFromSampleNumber).
			_ulCurrentSample_Internal	= ulCurrentSample;

		if (pmaStscAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error RTPUtility:: getAvgBitRate (
	MP4TrakAtom_p pmaMediaTrakAtom,
	MP4HdlrAtom_p pmaMediaHdlrAtom,
	MP4MdhdAtom_p pmaMediaMdhdAtom,
	MP4StszAtom_p pmaStszAtom,
	MP4Atom:: MP4Codec_t cCodecUsed,
	unsigned long *pulAvgBitRate,
	RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
	Tracer_p ptTracer)

{

	char							pHandlerType [MP4F_MAXHANDLERTYPELENGTH];
	// MP4HdlrAtom_p					pmaMediaHdlrAtom;
	// MP4Atom_p						pmaAtom;


	if (pmaMediaTrakAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if (pmaMediaTrakAtom -> searchAtom ("mdia:0:hdlr:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "mdia:0:hdlr:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaMediaHdlrAtom		= (MP4HdlrAtom_p) pmaAtom;
	*/

	if (pmaMediaHdlrAtom -> getHandlerType (pHandlerType,
		MP4F_MAXHANDLERTYPELENGTH) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (cCodecUsed == MP4Atom:: MP4F_CODEC_MPEG4 ||
		cCodecUsed == MP4Atom:: MP4F_CODEC_AAC)
	{
		MP4DecoderConfigDescr_p		pmaMediaDecoderConfigDescr;
		MP4Atom_p					pmaAtom;


		if (!strcmp (pHandlerType, MP4_AUDIO_TRACK_TYPE))
		{
			if (pmaMediaTrakAtom -> searchAtom (
				"mdia:0:minf:0:stbl:0:stsd:0:mp4a:0:esds:0:ESDescr:0:DecoderConfigDescr:0", true,
				&pmaAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED, 1,
					"mdia:0:minf:0:stbl:0:stsd:0:mp4a:0:esds:0:ESDescr:0:DecoderConfigDescr:0");
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			else
				pmaMediaDecoderConfigDescr		=
					(MP4DecoderConfigDescr_p) pmaAtom;
		}
		else if (!strcmp (pHandlerType, MP4_VIDEO_TRACK_TYPE))
		{
			if (pmaMediaTrakAtom -> searchAtom (
				"mdia:0:minf:0:stbl:0:stsd:0:mp4v:0:esds:0:ESDescr:0:DecoderConfigDescr:0", true,
				&pmaAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED, 1,
					"mdia:0:minf:0:stbl:0:stsd:0:mp4v:0:esds:0:ESDescr:0:DecoderConfigDescr:0");
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			else
				pmaMediaDecoderConfigDescr		=
					(MP4DecoderConfigDescr_p) pmaAtom;
		}
		else
		{
			// application
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_WRONGHANDLERTYPE,
				1, pHandlerType);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaMediaDecoderConfigDescr -> getAvgBitRate (pulAvgBitRate) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4DECODERCONFIGDESCR_GETAVGBITRATE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		/*
		*pulAvgBitRate				=
			((double) (((double) (*pulAvgBitRate)) / 1000)) -
			((unsigned long) ((*pulAvgBitRate) / 1000)) > 0.5 ?
			(*pulAvgBitRate) / 1000 + 1 : (*pulAvgBitRate) / 1000;
		*/
	}
	else if (cCodecUsed == MP4Atom:: MP4F_CODEC_H263 ||
		cCodecUsed == MP4Atom:: MP4F_CODEC_AMRNB ||
		cCodecUsed == MP4Atom:: MP4F_CODEC_AMRWB)
	{
		unsigned long			ulSamplesSize;
		// MP4MdhdAtom_p			pmaMediaMdhdAtom;
		unsigned long			ulMdhdTimeScale;
		#ifdef WIN32
			__int64					ullMdhdDuration;
		#else
			unsigned long long		ullMdhdDuration;
		#endif


		if (getSampleRangeSize (pmaStszAtom, 1,
			(unsigned long) -1, &ulSamplesSize,
			prsrtRTPStreamRealTimeInfo, ptTracer) !=
			errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_GETSAMPLERANGESIZE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		/*
		if (pmaMediaTrakAtom -> searchAtom ("mdia:0:mdhd:0",
			true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mdia:0:mdhd:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			pmaMediaMdhdAtom		= (MP4MdhdAtom_p) pmaAtom;
		*/

		if (pmaMediaMdhdAtom -> getTimeScale (
			&ulMdhdTimeScale) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4MDHDATOM_GETTIMESCALE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaMediaMdhdAtom -> getDuration (&ullMdhdDuration) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4MDHDATOM_GETDURATION_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		/*
		*pulAvgBitRate				=
			(unsigned long) ((double) ulSamplesSize /
			((double) ullMdhdDuration / (double) ulMdhdTimeScale) *
			8.0 / 1000.0);
		*/
		*pulAvgBitRate				=
			(unsigned long) ((double) ulSamplesSize /
			((double) ullMdhdDuration / (double) ulMdhdTimeScale) *
			8.0);
	}
	else
	{
		// Here I don't know the id. of the track
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_CODECUNKNOWN,
			1, 9999999);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	return errNoError;
}


#ifdef WIN32
	Error RTPUtility:: appendHintSampleToMdatAtom (
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
		Tracer_p ptTracer)
#else
	Error RTPUtility:: appendHintSampleToMdatAtom (
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
		Tracer_p ptTracer)
#endif

{

	unsigned long					ulRTPTimeScale;
	unsigned long					ulHintBytesNumberForMdat;
	// MP4StcoAtom_p					pmaStcoAtom;
	// MP4DmaxAtom_p					pmaDmaxAtom;
	// MP4MaxrAtom_p					pmaMaxrAtom;
	unsigned long					ulDurationPerChunk;
	Error_t							errSearchAtom;
	// MP4Atom_p						pmaAtom;


	if (pmaHintSttsAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if ((errSearchAtom = pmaHintTrakAtom -> searchAtom (
		"mdia:0:minf:0:stbl:0:stco:0", false,
		&pmaAtom)) != errNoError)
	{
		if ((long) errSearchAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mdia:0:minf:0:stbl:0:stco:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
		{
			if ((errSearchAtom = pmaHintTrakAtom -> searchAtom (
				"mdia:0:minf:0:stbl:0:co64:0", true,
				&pmaAtom)) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED,
					1, "mdia:0:minf:0:stbl:0:co64:0");
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaRootAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			else
				pmaStcoAtom		= (MP4StcoAtom_p) pmaAtom;
		}
	}
	else
		pmaStcoAtom		= (MP4StcoAtom_p) pmaAtom;

	if (pmaHintTrakAtom -> searchAtom ("mdia:0:mdhd:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "mdia:0:mdhd:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaHintMdhdAtom		= (MP4MdhdAtom_p) pmaAtom;

	if (pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:dmax:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "udta:0:hinf:0:dmax:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaDmaxAtom		= (MP4DmaxAtom_p) pmaAtom;

	if (pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:maxr:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "udta:0:hinf:0:maxr:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaMaxrAtom		= (MP4MaxrAtom_p) pmaAtom;
	*/

	if (pmaHintMdhdAtom -> getTimeScale (
		&ulRTPTimeScale) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	ulDurationPerChunk				= ulRTPTimeScale;

	if (prhsRTPHintSample -> appendToMdatAtom (pmaMdatAtom,
		&ulHintBytesNumberForMdat) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTSAMPLE_APPENDTOMDATATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	(*pulAllChunksBufferSize)			+= ulHintBytesNumberForMdat;
	(*pulCurrentChunkBufferSize)		+= ulHintBytesNumberForMdat;
	(*pulCurrentChunkSamplesNumber)++;

	// add hint duration
	(*pullCurrentChunkDuration)		+= ullHintDuration;

	if (MP4Utility:: updateSampleSizes (pmaHintStszAtom,
		ulHintSampleIdentifier, ulHintBytesNumberForMdat, ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UTILITY_UPDATESAMPLESIZES_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (MP4Utility:: updateSampleTimes (
		pmaHintSttsAtom, ullHintDuration,
		ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UTILITY_UPDATESAMPLETIMES_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (MP4Utility:: updateRenderingOffsets (pmaHintStblAtom,
		ulHintSampleIdentifier, 0, ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UTILITY_UPDATERENDERINGOFFSETS_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (MP4Utility:: updateSyncSamples (pmaHintStblAtom,
		ulHintSampleIdentifier, bIsSyncHintSample, ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UTILITY_UPDATESYNCSAMPLES_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (*pullCurrentChunkDuration >= ulDurationPerChunk &&
		*pulCurrentChunkBufferSize > 0)
	{
		unsigned long				ulEntriesNumber;


		if (pmaHintStcoAtom -> getEntriesNumber (&ulEntriesNumber) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (MP4Utility:: updateSampleToChunk (pmaHintStscAtom,
			ulHintSampleIdentifier, ulEntriesNumber + 1,
			*pulCurrentChunkSamplesNumber, ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_UPDATESAMPLETOCHUNK_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (MP4Utility:: updateChunkOffsets (pmaHintStcoAtom,
			*pullCurrentChunkOffset, ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_UPDATECHUNKOFFSET_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		(*pullCurrentChunkOffset)				+= (*pulCurrentChunkBufferSize);

		*pulCurrentChunkSamplesNumber		= 0;
		*pullCurrentChunkDuration			= 0;
		*pulCurrentChunkBufferSize			= 0;
	}

	if (MP4Utility:: updateDurations (
		pmaHintTkhdAtom, pmaHintMdhdAtom, pmaMvhdAtom,
		ullHintDuration, ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UTILITY_UPDATEDURATIONS_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (MP4Utility:: updateModificationTimes (
		pmaHintMdhdAtom, pmaHintTkhdAtom, ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UTILITY_UPDATEMODIFICATIONSTIMES_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// statistics: update hint duration if necessary
	{
		unsigned long						ulCurrentHintDuration;


		if (pmaHintDmaxAtom -> getMilliSecs (&ulCurrentHintDuration) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (ullHintDuration > ulCurrentHintDuration)
		{
			if (pmaHintDmaxAtom -> setMilliSecs ((unsigned long) ullHintDuration) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_SETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	// statistics: update maxr atom if necessary
	{
		#ifdef WIN32
			__int64									ullSampleStartTime;
			__int64									ullSampleDuration;
		#else
			unsigned long long						ullSampleStartTime;
			unsigned long long						ullSampleDuration;
		#endif
		unsigned long							ulBytes;


		if (getSampleTimeFromSampleNumber (pmaHintSttsAtom,
			ulHintSampleIdentifier, &ullSampleStartTime, &ullSampleDuration,
			prsrtRTPStreamRealTimeInfo, ptTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_GETSAMPLETIMEFROMSAMPLENUMBER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errNoError;
		}

		if (ullSampleStartTime < *pullThisSec + ulRTPTimeScale)
		{
			(*pulBytesThisSec)		+= ulCurrentHintBytesNumber;
		}
		else
		{
			if (pmaHintMaxrAtom -> getBytes (&ulBytes) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (*pulBytesThisSec > ulBytes)
			{
				if (pmaHintMaxrAtom -> setBytes (*pulBytesThisSec) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_SETVALUE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}

			*pullThisSec					= ullSampleStartTime -
				(ullSampleStartTime % ulRTPTimeScale);
			*pulBytesThisSec		= ulCurrentHintBytesNumber;
		}
	}

	if (pmaHintSttsAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPUtility:: getUShortRandom (unsigned short *pusShortRandom)

{

	// max value 65535
	do
	{
		/*
		#ifdef WIN32
			ulSequenceNumberRandomOffset				= rand () * rand ();
		#else
		*/
			*pusShortRandom								= rand ();
		// #endif
	}
	while (*pusShortRandom < 1000 || *pusShortRandom > 40000);


	return errNoError;
}


Error RTPUtility:: getULongRandom (unsigned long *pulLongRandom)

{

	// max value 4294967295
	do
	{
		#ifdef WIN32
			*pulLongRandom				= rand () * rand ();
		#else
			*pulLongRandom				= rand ();
		#endif
	}
	while (*pulLongRandom < 200000 || *pulLongRandom > 1500000000);


	return errNoError;
}


Error RTPUtility:: getRTPTimeScale (
	MP4TrakAtom_p pmaMediaTrakAtom,
	MP4MdhdAtom_p pmaMediaMdhdAtom,
	MP4Atom:: MP4Codec_t cCodecUsed,
	unsigned long ulObjectTypeIndication,
	MP4Atom:: Standard_t sStandard,
	unsigned long *pulRTPTimeScale,
	Tracer_p ptTracer)

{

// 	MP4Atom_p			pmaAtom;


	if (pmaMediaTrakAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	{
		/*
		MP4HdlrAtom_p			pmaMediaHdlrAtom;


		if (pmaMediaTrakAtom -> searchAtom ("mdia:0:hdlr:0", true,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mdia:0:hdlr:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			pmaMediaHdlrAtom		= (MP4HdlrAtom_p) pmaAtom;
		*/
	}

	if (cCodecUsed == MP4Atom:: MP4F_CODEC_MPEG4)
	{
		switch (ulObjectTypeIndication)
		{
			case MP4_MPEG4_VIDEO_TYPE:
				{
					// rfc3016

					if (sStandard == MP4Atom:: MP4F_3GPP)
					{
						*pulRTPTimeScale				= 90000;
					}
					else
					{
						unsigned long				ulMediaTimeScale;
						/*
						MP4MdhdAtom_p				pmaMediaMdhdAtom;


						if (pmaMediaTrakAtom -> searchAtom (
							"mdia:0:mdhd:0", true, &pmaAtom) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, "mdia:0:mdhd:0");
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (pmaMediaTrakAtom ->
								unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
						else
							pmaMediaMdhdAtom	= (MP4MdhdAtom_p) pmaAtom;
						*/

						if (pmaMediaMdhdAtom -> getTimeScale (
							&ulMediaTimeScale) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4PROPERTY_GETVALUE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (pmaMediaTrakAtom ->
								unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						*pulRTPTimeScale		=
							ulMediaTimeScale ? ulMediaTimeScale : 1000;
					}
				}

				break;
			default:
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_CODECUNKNOWN);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pmaMediaTrakAtom ->
						unLockMP4File () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
		}
	}
	else if (cCodecUsed == MP4Atom:: MP4F_CODEC_H263)
	{
		// rfc2429

		// if (sStandard == MP4Atom:: MP4F_3GPP)
		{
			*pulRTPTimeScale			= 90000;
		}
		/*
		else
		{
			sprintf (pClockRate, "%lu", ulMediaTimeScale ? ulMediaTimeScale : 1000);
		}
		*/
	}
	else if (cCodecUsed == MP4Atom:: MP4F_CODEC_AAC)
	{
		// ulObjectTypeIndication is the TrackAudioType
		switch (ulObjectTypeIndication)
		{
			case MP4_MPEG4_AUDIO_TYPE:
			case MP4_MPEG2_AAC_MAIN_AUDIO_TYPE:
			case MP4_MPEG2_AAC_LC_AUDIO_TYPE:
			case MP4_MPEG2_AAC_SSR_AUDIO_TYPE:
				if (sStandard == MP4Atom:: MP4F_3GPP)
				{
					// rfc3016

					unsigned long				ulSamplingRate;
					unsigned char				*pucESConfiguration;
					#ifdef WIN32
						__int64						ullESConfigurationSize;
					#else
						unsigned long long			ullESConfigurationSize;
					#endif


					if (MP4Utility:: getPointerTrackESConfiguration (
						pmaMediaTrakAtom,
						&pucESConfiguration, &ullESConfigurationSize, MP4Atom:: MP4F_3GPP,
						ptTracer) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4UTILITY_GETPOINTERTRACKESCONFIGURATION_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (ullESConfigurationSize != 2)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILE_WRONGESCONFIGURATIONFOUND);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					ulSamplingRate		= MP4AV_AacConfigGetSamplingRate (
						pucESConfiguration);

					// only 3GPP
					*pulRTPTimeScale		= ulSamplingRate;
				}
				else
				{
					// rfcIsma

					unsigned long				ulMediaTimeScale;


					{
						/*
						MP4MdhdAtom_p				pmaMediaMdhdAtom;


						if (pmaMediaTrakAtom -> searchAtom ("mdia:0:mdhd:0", true,
							&pmaAtom) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, "mdia:0:mdhd:0");
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
						else
							pmaMediaMdhdAtom	= (MP4MdhdAtom_p) pmaAtom;
						*/

						if (pmaMediaMdhdAtom -> getTimeScale (&ulMediaTimeScale) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4PROPERTY_GETVALUE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
					}

					*pulRTPTimeScale		= ulMediaTimeScale ? ulMediaTimeScale : 1000;
				}

				break;
			default:
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_CODECUNKNOWN);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
		}
	}
	else if (cCodecUsed == MP4Atom:: MP4F_CODEC_AMRNB ||
		cCodecUsed == MP4Atom:: MP4F_CODEC_AMRWB)
	{
		// rfc3267

		if (cCodecUsed == MP4Atom:: MP4F_CODEC_AMRNB)
			*pulRTPTimeScale				= 8000;
		else	// if (cCodecUsed == MP4Atom:: MP4F_CODEC_AMRWB)
			*pulRTPTimeScale				= 16000;
	}
	else
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_CODECUNKNOWN);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaMediaTrakAtom ->
		unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPUtility:: createHintTrack (
	MP4RootAtom_p pmaRootAtom,
	std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo,
	MP4TrackInfo_p pmtiMediaTrackInfo,
	unsigned long ulMaxPayloadSize, Boolean_t bInterleave,
	MP4TrackInfo_p *pmtiHintTrackInfo, Tracer_p ptTracer)

{

	MP4Atom:: Standard_t		sStandard;
	RTPStreamRealTimeInfo_t		rsrtRTPStreamRealTimeInfo;


	if ((*pmtiHintTrackInfo = new MP4TrackInfo_t) ==
		(MP4TrackInfo_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmaRootAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete *pmtiHintTrackInfo;
		*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

		return err;
	}

	if (pmaRootAtom -> getStandard (&sStandard) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETSTANDARD_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		delete *pmtiHintTrackInfo;
		*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

		return err;
	}

	// hint trak generation
	{
		char			pHandlerType [MP4F_MAXHANDLERTYPELENGTH];
		MP4Atom:: MP4Codec_t		cCodec;
		unsigned long				ulObjectTypeIndication;
		unsigned long				ulTrackIdentifier;


		if (pmtiMediaTrackInfo -> getHandlerType (
			pHandlerType) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHANDLERTYPE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete *pmtiHintTrackInfo;
			*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

			return err;
		}

		if (pmtiMediaTrackInfo -> getTrackIdentifier (
			&ulTrackIdentifier) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETTRACKIDENTIFIER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete *pmtiHintTrackInfo;
			*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

			return err;
		}

		if (!strcmp (pHandlerType, MP4_AUDIO_TRACK_TYPE))
		{
			if (pmtiMediaTrackInfo -> getCodec (
				&cCodec) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETCODEC_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaRootAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete *pmtiHintTrackInfo;
				*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

				return err;
			}

			if (cCodec == MP4Atom:: MP4F_CODEC_AAC)
			{
				if (pmtiMediaTrackInfo -> getObjectTypeIndication (
					&ulObjectTypeIndication) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETOBJECTTYPEINDICATION_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pmaRootAtom -> unLockMP4File () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete *pmtiHintTrackInfo;
					*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

					return err;
				}

				// ulObjectTypeIndication is the TrackAudioType
				switch (ulObjectTypeIndication)
				{
					case MP4_MPEG4_AUDIO_TYPE:
					case MP4_MPEG2_AAC_MAIN_AUDIO_TYPE:
					case MP4_MPEG2_AAC_LC_AUDIO_TYPE:
					case MP4_MPEG2_AAC_SSR_AUDIO_TYPE:
						if (sStandard == MP4Atom:: MP4F_3GPP)
						{
							if (rfc3016AudioHinter (pmaRootAtom,
								pmtiMediaTrackInfo,
								ulMaxPayloadSize,
								*pmtiHintTrackInfo, pvMP4TracksInfo,
								sStandard,
								&rsrtRTPStreamRealTimeInfo,
								ptTracer) != errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RFC_RFC3016AUDIOHINTER_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (pmaRootAtom -> unLockMP4File () !=
									errNoError)
								{
									Error err = MP4FileErrors (__FILE__, __LINE__,
										MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								delete *pmtiHintTrackInfo;
								*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

								return err;
							}
						}
						else		// MP4F_ISMA
						{
							if (rfcIsmaAudioHinter (pmaRootAtom,
								pmtiMediaTrackInfo,
								ulMaxPayloadSize,
								bInterleave, *pmtiHintTrackInfo,
								pvMP4TracksInfo, sStandard,
								&rsrtRTPStreamRealTimeInfo, ptTracer) !=
								errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RFC_RFCISMAHINTER_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (pmaRootAtom -> unLockMP4File () !=
									errNoError)
								{
									Error err = MP4FileErrors (
										__FILE__, __LINE__,
										MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								delete *pmtiHintTrackInfo;
								*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

								return err;
							}
						}

						break;
					// case MP4_MPEG1_AUDIO_TYPE:
					// case MP4_MPEG2_AUDIO_TYPE:
					default:
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_CODECUNKNOWN,
								1, ulTrackIdentifier);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (pmaRootAtom -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							delete *pmtiHintTrackInfo;
							*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

							return err;
						}
				}
			}
			else if (cCodec == MP4Atom:: MP4F_CODEC_AMRNB ||
				cCodec == MP4Atom:: MP4F_CODEC_AMRWB)
			{
				if (rfc3267AudioHinter (pmaRootAtom,
					pmtiMediaTrackInfo,
					ulMaxPayloadSize, *pmtiHintTrackInfo, pvMP4TracksInfo,
					sStandard,
					&rsrtRTPStreamRealTimeInfo,
					ptTracer) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RFC_RFC3267AUDIOHINTER_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pmaRootAtom -> unLockMP4File () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete *pmtiHintTrackInfo;
					*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

					return err;
				}
			}
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_CODECUNKNOWN,
					1, ulTrackIdentifier);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaRootAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete *pmtiHintTrackInfo;
				*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

				return err;
			}
		}
		else if (!strcmp (pHandlerType, MP4_VIDEO_TRACK_TYPE))
		{
			if (pmtiMediaTrackInfo -> getCodec (
				&cCodec) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETCODEC_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaRootAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete *pmtiHintTrackInfo;
				*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

				return err;
			}

			if (cCodec == MP4Atom:: MP4F_CODEC_MPEG4)
			{
				if (pmtiMediaTrackInfo -> getObjectTypeIndication (
					&ulObjectTypeIndication) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETOBJECTTYPEINDICATION_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pmaRootAtom -> unLockMP4File () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete *pmtiHintTrackInfo;
					*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

					return err;
				}

				switch (ulObjectTypeIndication)
				{
					case MP4_MPEG4_VIDEO_TYPE:
						{
							if (rfc3016VisualHinter (pmaRootAtom,
								pmtiMediaTrackInfo,
								ulMaxPayloadSize, *pmtiHintTrackInfo,
								pvMP4TracksInfo, sStandard,
								&rsrtRTPStreamRealTimeInfo,
								ptTracer) != errNoError)
							{
								Error err = RTPErrors (__FILE__, __LINE__,
									RTP_RFC_RFC3016VISUALHINTER_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (pmaRootAtom -> unLockMP4File () !=
									errNoError)
								{
									Error err = MP4FileErrors (
										__FILE__, __LINE__,
										MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								delete *pmtiHintTrackInfo;
								*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

								return err;
							}
						}

						break;
					default:
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_CODECUNKNOWN,
								1, ulTrackIdentifier);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (pmaRootAtom -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							delete *pmtiHintTrackInfo;
							*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

							return err;
						}
				}
			}
			else if (cCodec == MP4Atom:: MP4F_CODEC_H263)
			{
				if (rfc2429VisualHinter (pmaRootAtom,
					pmtiMediaTrackInfo,
					ulMaxPayloadSize, *pmtiHintTrackInfo, pvMP4TracksInfo,
					sStandard,
					&rsrtRTPStreamRealTimeInfo,
					ptTracer) != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RFC_RFC2429VISUALHINTER_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pmaRootAtom -> unLockMP4File () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					delete *pmtiHintTrackInfo;
					*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

					return err;
				}
			}
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_CODECUNKNOWN,
					1, ulTrackIdentifier);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaRootAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				delete *pmtiHintTrackInfo;
				*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

				return err;
			}
		}
		else
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_TRACKTYPEUNKNOWN,
				1, pHandlerType);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			delete *pmtiHintTrackInfo;
			*pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

			return err;
		}
	}

	if (pmaRootAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		// commented because already inserted into the vector
		// delete *pmtiHintTrackInfo;
		// *pmtiHintTrackInfo			= (MP4TrackInfo_p) NULL;

		return err;
	}


	return errNoError;
}


Error RTPUtility:: createAllHintTracks (
	MP4File_p pmfFile,
	unsigned long ulMaxPayloadSize,
	Tracer_p ptTracer)

{

	Buffer_t					bHandlerType;
	MP4RootAtom_p				pmaRootAtom;
	std:: vector<MP4TrackInfo_p>	*pvMP4TracksInfo;


	if (pmfFile -> getRootAtom (&pmaRootAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_GETROOTATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmaRootAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmfFile -> getTracksInfo (&pvMP4TracksInfo) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_GETTRACKSINFO_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (bHandlerType. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pvMP4TracksInfo -> size () > 0)
	{
		std:: vector<MP4TrackInfo_p>:: const_iterator	it;
		MP4TrackInfo_p					pmtiMP4TrackInfo;
		MP4TrackInfo_p					pmtiHintMP4TrackInfo;
		Error_t							errHintTrack;
//		MP4Atom_p						pmaAtom;
		char							pHandlerType [
			MP4F_MAXHANDLERTYPELENGTH];


		for (it = pvMP4TracksInfo -> begin (); it != pvMP4TracksInfo -> end ();
			++it)
		{
			pmtiMP4TrackInfo				= *it;

			if (pmtiMP4TrackInfo -> getHandlerType (
				pHandlerType) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETHANDLERTYPE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bHandlerType. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (pmaRootAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (!(!strcmp (pHandlerType, MP4_VIDEO_TRACK_TYPE) ||
				!strcmp (pHandlerType, MP4_AUDIO_TRACK_TYPE)))
				continue;

			if ((errHintTrack = pmfFile -> getHintTrackInfoFromMediaTrackInfo (
				pmtiMP4TrackInfo, &pmtiHintMP4TrackInfo)) !=
				errNoError)
			{
				if ((long) errHintTrack == MP4F_MP4FILE_HINTTRACKNOTFOUND)
				{
					if (createHintTrack (pmaRootAtom, pvMP4TracksInfo,
						pmtiMP4TrackInfo,
						ulMaxPayloadSize, false, &pmtiHintMP4TrackInfo,
						ptTracer) != errNoError)
					{
						Error err = RTPErrors (__FILE__, __LINE__,
							RTP_RTPUTILITY_CREATEHINTTRACK_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (bHandlerType. finish () != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_FINISH_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						if (pmaRootAtom -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					it			= pvMP4TracksInfo -> begin ();
				}
				else
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FILE_GETHINTTRAKFROMMEDIATRAKIDENTIFIER_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bHandlerType. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (pmaRootAtom -> unLockMP4File () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
		}
	}

	if (bHandlerType. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaRootAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPUtility:: allocRtpPayloadNumber (unsigned long *pulPayloadNumber,
	std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo, Tracer_p ptTracer)

{

	unsigned char					ucPayloadNumber;
	std:: vector<unsigned long>		vUsedPayloads;
	unsigned long					ulUsedPayloadIndex;
	std:: vector<MP4TrackInfo_p>:: const_iterator	it;
	MP4TrackInfo_p					pmtiMP4TrackInfo;
	MP4PaytAtom_p					pmaPaytAtom;
	MP4TrakAtom_p					pmaTrakAtom;
	Error_t							errGeneric;



	if ((*pvMP4TracksInfo). size () == 0)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((*(pvMP4TracksInfo -> begin ())) -> getTrakAtom (&pmaTrakAtom) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmaTrakAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	for (it = (*pvMP4TracksInfo). begin ();
		it != (*pvMP4TracksInfo). end (); ++it)
	{
		pmtiMP4TrackInfo				= *it;

		if ((errGeneric = pmtiMP4TrackInfo -> getPaytAtom (&pmaPaytAtom,
			false)) != errNoError)
		{
			if ((long) errGeneric == MP4F_MP4ATOM_ATOMNOTFOUND)
				continue;
			else
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETPAYTATOM_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (pmaPaytAtom -> getPayloadNumber (pulPayloadNumber) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		vUsedPayloads. insert (vUsedPayloads. end (), *pulPayloadNumber);
	}

	for (ucPayloadNumber = 96; ucPayloadNumber < 128; ucPayloadNumber++)
	{
		for (ulUsedPayloadIndex = 0;
			ulUsedPayloadIndex < vUsedPayloads. size (); ulUsedPayloadIndex++)
		{
			if (ucPayloadNumber == vUsedPayloads [ulUsedPayloadIndex])
				break;
		}

		if (ulUsedPayloadIndex == vUsedPayloads. size ())
			break;
	}

	if (ucPayloadNumber == 128)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UTILITY_NOPAYLOADAVAILABLE);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	*pulPayloadNumber				= ucPayloadNumber;

	if (pmaTrakAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}

