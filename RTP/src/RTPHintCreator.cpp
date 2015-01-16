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


#include "RTPHintCreator.h"
#include "RTPMessages.h"
#include "MP4DimmAtom.h"
#include "MP4DmedAtom.h"
#include "MP4HmhdAtom.h"
#include "MP4MaxrAtom.h"
#include "MP4MdhdAtom.h"
#include "MP4PmaxAtom.h"
#include "MP4TpylAtom.h"
#include "MP4TrpyAtom.h"
#include "MP4Utility.h"
#include "RTPUtility.h"
#ifdef WIN32
#else
#endif
#include <assert.h>



RTPHintCreator:: RTPHintCreator (void)

{

	_stRTPHintCreatorStatus			= RHC_BUILDED;

}


RTPHintCreator:: ~RTPHintCreator (void)

{

}



RTPHintCreator:: RTPHintCreator (const RTPHintCreator &)

{

	assert (1==0);

	// to do

}


RTPHintCreator &RTPHintCreator:: operator = (const RTPHintCreator &)

{

	assert (1==0);

	// to do

	return *this;

}


Error RTPHintCreator:: init (MP4TrackInfo_p pmtiMP4HintTrackInfo,
	MP4RootAtom_p pmaRootAtom, unsigned long ulMaxMediaSampleSize,
	Tracer_p ptTracer)

{

	MP4Atom_p				pmaAtom;


	_pmaRootAtom						= pmaRootAtom;
	_ptTracer							= ptTracer;
	_pmtiMP4HintTrackInfo				= pmtiMP4HintTrackInfo;

	if (_stRTPHintCreatorStatus != RHC_BUILDED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_pmaRootAtom -> addMdatChild (&pmaAtom) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_ADDCHILD_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	else
		_pmaMdatAtom		= (MP4MdatAtom_p) pmaAtom;

	if (_pmaMdatAtom -> getDataStartOffset (&_ullCurrentChunkOffset) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4MDATATOM_GETDATASTARTOFFSET_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (_pmaRootAtom -> deleteChild (_pmaMdatAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_DELETECHILD_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	_ulCurrentHintSampleIdentifier				= 0;
	_ulCurrentHintPacketIdentifier				= 0;
	_ulAllChunksBufferSize						= 0;
	_ulCurrentChunkBufferSize					= 0;
	_ulCurrentChunkSamplesNumber				= 0;
	_ullCurrentChunkDuration					= 0;
	_ullThisSec									= 0;
	_ulBytesThisSec								= 0;

	_stRTPHintCreatorStatus			= RHC_INITIALIZED;


	return errNoError;
}


Error RTPHintCreator:: finish (Boolean_t bCorrectlyFinish)

{

	// MP4Atom_p				pmaAtom;
	// MP4TrakAtom_p			pmaHintTrakAtom;


	/*
	if (_pmtiMP4HintTrackInfo -> getTrakAtom (&pmaHintTrakAtom) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}
	*/

	if (bCorrectlyFinish)
	{
		if (_stRTPHintCreatorStatus != RHC_INITIALIZED)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_ACTIVATION_WRONG);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		// if in appendHintSampleToMdatAtom the chunk is not full (
		//		means *pullCurrentChunkDuration >= ulDurationPerChunk &&
		//		*pulCurrentChunkBufferSize > 0)
		//	in any case, if we have somethink in the chunk, we need to update
		//	the chunks atoms
		if (_ulCurrentChunkBufferSize > 0)
		{
			MP4StscAtom_p					pmaHintStscAtom;
			unsigned long					ulEntriesNumber;
			MP4StcoAtom_p					pmaHintStcoAtom;


			if (_pmtiMP4HintTrackInfo -> getStcoAtom (&pmaHintStcoAtom,
				true) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSTCOATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (pmaHintStcoAtom -> getEntriesNumber (&ulEntriesNumber) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (_pmtiMP4HintTrackInfo -> getStscAtom (&pmaHintStscAtom, true) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSTSCATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (MP4Utility:: updateSampleToChunk (pmaHintStscAtom,
				_ulCurrentHintSampleIdentifier, ulEntriesNumber + 1,
				_ulCurrentChunkSamplesNumber, _ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_UPDATESAMPLETOCHUNK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			if (MP4Utility:: updateChunkOffsets (pmaHintStcoAtom,
				_ullCurrentChunkOffset, _ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_UPDATECHUNKOFFSET_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			(_ullCurrentChunkOffset)			+= (_ulCurrentChunkBufferSize);

			_ulCurrentChunkSamplesNumber		= 0;
			_ullCurrentChunkDuration			= 0;
			_ulCurrentChunkBufferSize			= 0;
		}

		// statistics
		{
			MP4HmhdAtom_p						pmaHintHmhdAtom;

			if (_pmtiMP4HintTrackInfo -> getHmhdAtom (&pmaHintHmhdAtom, true) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETHMHDATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			// statistics: pmaHintHmhdAtom -> setMaxPduSize
			// Pdu: Packet data unit
			{
				MP4PmaxAtom_p			pmaHintPmaxAtom;
				unsigned long			ulMaxPayloadAndPacketPropertiesBytesNumber;


				if (_pmtiMP4HintTrackInfo -> getPmaxAtom (&pmaHintPmaxAtom,
					true) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETPMAXATOM_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (pmaHintPmaxAtom -> getBytes (
					&ulMaxPayloadAndPacketPropertiesBytesNumber) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (pmaHintHmhdAtom -> setMaxPduSize (
					ulMaxPayloadAndPacketPropertiesBytesNumber) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_SETVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			// statistics: pmaHintHmhdAtom -> setAvgPduSize
			// Pdu: Packet data unit
			{
				MP4NumpAtom_p					pmaHintNumpAtom;
				#ifdef WIN32
					__int64							ullPackets;
					__int64							ullBytes;
				#else
					unsigned long long				ullPackets;
					unsigned long long				ullBytes;
				#endif
				MP4TrpyAtom_p					pmaHintTrpyAtom;


				if (_pmtiMP4HintTrackInfo -> getNumpAtom (&pmaHintNumpAtom,
					true) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETNUMPATOM_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (_pmtiMP4HintTrackInfo -> getTrpyAtom (&pmaHintTrpyAtom,
					true) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETTRPYATOM_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (pmaHintNumpAtom -> getPackets (&ullPackets) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (ullPackets)
				{
					if (pmaHintTrpyAtom -> getBytes (&ullBytes) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4PROPERTY_GETVALUE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return err;
					}

					if (pmaHintHmhdAtom -> setAvgPduSize (
						(unsigned long) (ullBytes / ullPackets)) !=
						errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4PROPERTY_SETVALUE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return err;
					}
				}
			}

			// statistics: update hmhd::MaxBitRate
			{
				unsigned long							ulBytes;
				MP4MaxrAtom_p							pmaHintMaxrAtom;


				if (_pmtiMP4HintTrackInfo -> getMaxrAtom (&pmaHintMaxrAtom,
					true) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETMAXRATOM_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (pmaHintMaxrAtom -> getBytes (&ulBytes) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (pmaHintHmhdAtom -> setMaxBitRate (ulBytes * 8) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			// statistics: pmaHintHmhdAtom -> setAvgBitRate
			{
				MP4MdhdAtom_p					pmaHintMdhdAtom;
				MP4TrpyAtom_p					pmaHintTrpyAtom;
				#ifdef WIN32
					__int64							ullBytes;
					__int64							ullDuration;
				#else
					unsigned long long				ullBytes;
					unsigned long long				ullDuration;
				#endif
				unsigned long					ulRTPTimeScale;


				if (_pmtiMP4HintTrackInfo -> getMdhdAtom (&pmaHintMdhdAtom) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETMDHDATOM_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (_pmtiMP4HintTrackInfo -> getTrpyAtom (&pmaHintTrpyAtom,
					true) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETTRPYATOM_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (pmaHintMdhdAtom -> getDuration (&ullDuration) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
						__FILE__, __LINE__);

					return err;
				}

				if (ullDuration)
				{
					if (pmaHintTrpyAtom -> getBytes (&ullBytes) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4PROPERTY_GETVALUE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return err;
					}

					if (pmaHintMdhdAtom -> getTimeScale (&ulRTPTimeScale) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4PROPERTY_GETVALUE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
							__FILE__, __LINE__);

						return err;
					}

					if (pmaHintHmhdAtom -> setAvgBitRate (
						(unsigned long) (ullBytes * 8 * ulRTPTimeScale / ullDuration)) !=
						errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4PROPERTY_SETVALUE_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						return err;
					}
				}
			}
		}
	}
	else
	{
		if (_stRTPHintCreatorStatus == RHC_RTPHINTSAMPLEINITIALIZED ||
			_stRTPHintCreatorStatus == RHC_RTPHINTPACKETINITIALIZED)
		{
			if (_prhsCurrentRTPHintSample -> finish () != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTSAMPLE_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			delete _prhsCurrentRTPHintSample;
			_prhsCurrentRTPHintSample				= (RTPHintSample_p) NULL;
		}

		if (_pmaRootAtom -> deleteChild (_pmaMdatAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_DELETECHILD_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	_stRTPHintCreatorStatus			= RHC_BUILDED;


	return errNoError;
}


Error RTPHintCreator:: addRTPHintSample (void)

{

	if (_stRTPHintCreatorStatus != RHC_INITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	_ulCurrentHintBytesNumber					= 0;

	if ((_prhsCurrentRTPHintSample = new RTPHintSample_t) ==
		(RTPHintSample_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	_ulCurrentHintSampleIdentifier				+= 1;

	if (_prhsCurrentRTPHintSample -> init (_pmtiMP4HintTrackInfo,
		_ulCurrentHintSampleIdentifier, _ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTSAMPLE_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete _prhsCurrentRTPHintSample;
		_prhsCurrentRTPHintSample			= (RTPHintSample_p) NULL;

		return err;
	}

	_stRTPHintCreatorStatus			= RHC_RTPHINTSAMPLEINITIALIZED;


	return errNoError;
}


Error RTPHintCreator:: addRTPHintPacketToCurrentRTPHintSample (
	Boolean_t bIsBFrame, unsigned long ulTLVTimestampOffset,
	unsigned long ulPayloadNumber, Boolean_t bMarkerBit)

{

	if (_stRTPHintCreatorStatus != RHC_RTPHINTSAMPLEINITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_prhsCurrentRTPHintSample -> createRTPHintPacket (
		&_prpCurrentRTPHintPacket) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTSAMPLE_CREATERTPHINTPACKET_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_prpCurrentRTPHintPacket -> setBFrame (bIsBFrame) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_SETBFRAME_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// means no TVL information
	if (_prpCurrentRTPHintPacket -> setTLVTimestampOffset (
		ulTLVTimestampOffset) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_SETTLVTIMESTAMPOFFSET_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_prpCurrentRTPHintPacket -> setPayloadType (ulPayloadNumber) !=
		errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_SETPAYLOADTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_prpCurrentRTPHintPacket -> setSequenceNumber (
		_ulCurrentHintPacketIdentifier++) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_SETSEQUENCENUMBER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_prpCurrentRTPHintPacket -> setMarkerBit (bMarkerBit) !=
		errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_SETMARKERBIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	_stRTPHintCreatorStatus			= RHC_RTPHINTPACKETINITIALIZED;


	return errNoError;
}


Error RTPHintCreator:: addRTPReferenceSampleToCurrentRTPHintPacket (
	unsigned long ulMediaSampleIdentifier,
	unsigned long ulDataOffset, unsigned long ulDataLength)

{

	if (_stRTPHintCreatorStatus != RHC_RTPHINTPACKETINITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_prpCurrentRTPHintPacket -> createRTPReferenceSample (
		ulMediaSampleIdentifier, ulDataOffset,
		ulDataLength) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_CREATERTPREFERENCESAMPLE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintCreator:: addRTPImmediateDataToCurrentRTPHintPacket (
	unsigned char *pucImmediateData, unsigned long ulBytesNumber)

{

	if (_stRTPHintCreatorStatus != RHC_RTPHINTPACKETINITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_prpCurrentRTPHintPacket -> createRTPImmediateData (
		pucImmediateData, ulBytesNumber) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_CREATERTPIMMEDIATEDATA_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintCreator:: updateStatisticsForRTPHintPacket (void)

{

//	MP4Atom_p				pmaAtom;


	if (_stRTPHintCreatorStatus != RHC_RTPHINTPACKETINITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// update _ulCurrentHintBytesNumber according
	// this RTP hint packet
	// statistics: update max packet size if necessary
	// (pmaPmaxAtom -> setBytes)
	// statistics: update trpy according the bytes sent including
	// RTP headers (pmaTrpyAtom -> incrementBytes)
	{
		MP4PmaxAtom_p	pmaHintPmaxAtom;
		unsigned long	ulMaxPayloadAndPacketPropertiesBytesNumber;
		unsigned long	ulPayloadAndPacketPropertiesBytesNumber;


		if (_prpCurrentRTPHintPacket ->
			getPayloadAndPacketPropertiesBytesNumber (
			&ulPayloadAndPacketPropertiesBytesNumber) !=
			errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKET_GETPAYLOADANDPACKETPROPERTIESBYTESNUMBER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		_ulCurrentHintBytesNumber				+=
			ulPayloadAndPacketPropertiesBytesNumber;

		if (_pmtiMP4HintTrackInfo -> getPmaxAtom (&pmaHintPmaxAtom, true) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETPMAXATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		/*
		if (_pmaHintTrakAtom -> searchAtom (
			"udta:0:hinf:0:pmax:0", true,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "udta:0:hinf:0:pmax:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
		else
			pmaPmaxAtom	= (MP4PmaxAtom_p) pmaAtom;
		*/

		if (pmaHintPmaxAtom -> getBytes (
			&ulMaxPayloadAndPacketPropertiesBytesNumber) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (ulPayloadAndPacketPropertiesBytesNumber >
			ulMaxPayloadAndPacketPropertiesBytesNumber)
		{
			if (pmaHintPmaxAtom -> setBytes (
				ulPayloadAndPacketPropertiesBytesNumber) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_SETVALUE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		{
			MP4TrpyAtom_p			pmaHintTrpyAtom;


			if (_pmtiMP4HintTrackInfo -> getTrpyAtom (
				&pmaHintTrpyAtom, true) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRPYATOM_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
			/*
			if (_pmaHintTrakAtom -> searchAtom (
				"udta:0:hinf:0:trpy:0", true,
				&pmaAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED,
					1, "udta:0:hinf:0:trpy:0");
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
			else
				pmaTrpyAtom	= (MP4TrpyAtom_p) pmaAtom;
			*/

			if (pmaHintTrpyAtom -> incrementBytes (
				ulPayloadAndPacketPropertiesBytesNumber) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
	}

	// statistics: update tpyl according the bytes sent excluding
	// RTP headers (pmaTpylAtom -> incrementBytes)
	{
		unsigned long							ulPacketPayloadSize;
		MP4TpylAtom_p							pmaHintTpylAtom;


		if (_prpCurrentRTPHintPacket -> getPayloadBytesNumber (
			&ulPacketPayloadSize) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKET_GETPAYLOADBYTESNUMBER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pmtiMP4HintTrackInfo -> getTpylAtom (
			&pmaHintTpylAtom, true) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETTPYLATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
		/*
		if (_pmaHintTrakAtom -> searchAtom (
			"udta:0:hinf:0:tpyl:0", true,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "udta:0:hinf:0:tpyl:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
		else
			pmaTpylAtom	= (MP4TpylAtom_p) pmaAtom;
		*/

		if (pmaHintTpylAtom -> incrementBytes (ulPacketPayloadSize) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	_stRTPHintCreatorStatus			= RHC_RTPHINTSAMPLEINITIALIZED;


	return errNoError;
}


Error RTPHintCreator:: updateStatisticsForRTPHintSample (void)

{

//	MP4Atom_p				pmaAtom;


	if (_stRTPHintCreatorStatus != RHC_RTPHINTSAMPLEINITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// statistics: update dimm according the immediate data bytes sent
	// (pmaDimmAtom -> incrementBytes)
	{
		unsigned long						ulPacketImmediateDataSize;
		MP4DimmAtom_p						pmaHintDimmAtom;


		if (_prpCurrentRTPHintPacket -> getImmediateDataBytesNumber (
			&ulPacketImmediateDataSize) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKET_GETIMMEDIATEDATABYTESNUMBER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pmtiMP4HintTrackInfo -> getDimmAtom (
			&pmaHintDimmAtom, true) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETTRPYATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		/*
		if (_pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:dimm:0",
			true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "udta:0:hinf:0:dimm:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
		else
			pmaDimmAtom	= (MP4DimmAtom_p) pmaAtom;
		*/

		if (pmaHintDimmAtom -> incrementBytes (
			ulPacketImmediateDataSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	// statistics: update dmed according the bytes sent from media data
	// (pmaDmedAtom -> incrementBytes)
	{
		unsigned long							ulPacketMediaDataSize;
		MP4DmedAtom_p							pmaHintDmedAtom;


		if (_prpCurrentRTPHintPacket -> getMediaDataBytesNumber (
			&ulPacketMediaDataSize) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKET_GETMEDIADATABYTESNUMBER_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pmtiMP4HintTrackInfo -> getDmedAtom (
			&pmaHintDmedAtom, true) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETDMEDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		/*
		if (_pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:dmed:0",
			true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "udta:0:hinf:0:dmed:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
		else
			pmaDmedAtom	= (MP4DmedAtom_p) pmaAtom;
		*/

		if (pmaHintDmedAtom -> incrementBytes (ulPacketMediaDataSize) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	_stRTPHintCreatorStatus			= RHC_RTPHINTSAMPLECONCLUDED;


	return errNoError;
}


#ifdef WIN32
	Error RTPHintCreator:: appendRTPHintSampleToMdatAtom (
		__int64 ullHintDuration,
		Boolean_t bIsSyncHintSample,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo)
#else
	Error RTPHintCreator:: appendRTPHintSampleToMdatAtom (
		unsigned long long ullHintDuration,
		Boolean_t bIsSyncHintSample,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo)
#endif

{

	MP4SttsAtom_p			pmaHintSttsAtom;
	MP4StszAtom_p			pmaHintStszAtom;
	MP4StblAtom_p			pmaHintStblAtom;
	MP4StscAtom_p			pmaHintStscAtom;
	MP4TkhdAtom_p			pmaHintTkhdAtom;
	MP4MdhdAtom_p			pmaHintMdhdAtom;
	MP4StcoAtom_p			pmaHintStcoAtom;
	MP4DmaxAtom_p			pmaHintDmaxAtom;
	MP4MaxrAtom_p			pmaHintMaxrAtom;
	MP4MvhdAtom_p			pmaMvhdAtom;
	MP4Atom_p				pmaAtom;


	if (_stRTPHintCreatorStatus != RHC_RTPHINTSAMPLECONCLUDED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	{
		{
			Message msg = RTPMessages (__FILE__, __LINE__,
				RTP_RTPHINTCREATOR_APPENDHINTSAMPLETOMDATATOM, 2,
				_ulCurrentHintSampleIdentifier,
				ullHintDuration);
			_ptTracer -> trace (Tracer:: TRACER_LDBG5,
				(const char *) msg, __FILE__, __LINE__);
		}

		if (_pmtiMP4HintTrackInfo -> getSttsAtom (
			&pmaHintSttsAtom, true) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTTSATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pmtiMP4HintTrackInfo -> getStszAtom (
			&pmaHintStszAtom, true) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSZATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pmtiMP4HintTrackInfo -> getStblAtom (
			&pmaHintStblAtom, true) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTBLATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pmtiMP4HintTrackInfo -> getStscAtom (
			&pmaHintStscAtom, true) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSCATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pmtiMP4HintTrackInfo -> getTkhdAtom (
			&pmaHintTkhdAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETTKHDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pmtiMP4HintTrackInfo -> getMdhdAtom (
			&pmaHintMdhdAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMDHDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pmtiMP4HintTrackInfo -> getStcoAtom (
			&pmaHintStcoAtom, true) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTCOATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pmtiMP4HintTrackInfo -> getDmaxAtom (
			&pmaHintDmaxAtom, true) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETDMAXATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pmtiMP4HintTrackInfo -> getMaxrAtom (
			&pmaHintMaxrAtom, true) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMAXRATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_pmaRootAtom -> searchAtom ("moov:0:mvhd:0", true,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_SEARCHATOM_FAILED, 1, "moov:0:mvhd:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
		else
			pmaMvhdAtom		= (MP4MvhdAtom_p) pmaAtom;

		if (RTPUtility:: appendHintSampleToMdatAtom (_pmaRootAtom,
			pmaHintSttsAtom, pmaHintStszAtom, pmaHintStblAtom,
			pmaHintStscAtom, pmaHintTkhdAtom, pmaHintMdhdAtom,
			pmaMvhdAtom, pmaHintStcoAtom, pmaHintDmaxAtom,
			pmaHintMaxrAtom, _prhsCurrentRTPHintSample,
			_ulCurrentHintSampleIdentifier, ullHintDuration,
			_ulCurrentHintBytesNumber, _pmaMdatAtom, bIsSyncHintSample,
			&_ulAllChunksBufferSize, &_ulCurrentChunkBufferSize,
			&_ulCurrentChunkSamplesNumber, &_ullCurrentChunkDuration,
			&_ullThisSec, &_ulBytesThisSec, &_ullCurrentChunkOffset,
			prsrtRTPStreamRealTimeInfo, _ptTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPUTILITY_APPENDHINTSAMPLETOMDATATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (_prhsCurrentRTPHintSample -> finish () != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTSAMPLE_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	delete _prhsCurrentRTPHintSample;
	_prhsCurrentRTPHintSample				= (RTPHintSample_p) NULL;

	_stRTPHintCreatorStatus			= RHC_INITIALIZED;


	return errNoError;
}

