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


#include "MP4Utility.h"
#include "RTPHintSample.h"
#include "RTPMessages.h"
#include "RTPUtility.h"
#include <sys/types.h>
#ifdef WIN32
#else
	#include <unistd.h>
#endif
#include <assert.h>
#include <stdio.h>



RTPHintSample:: RTPHintSample (void)

{

	_stRTPHintSampleStatus			= RHS_BUILDED;

}


RTPHintSample:: ~RTPHintSample (void)

{

}



RTPHintSample:: RTPHintSample (const RTPHintSample &)

{

	assert (1==0);

	// to do

}


RTPHintSample &RTPHintSample:: operator = (const RTPHintSample &)

{

	assert (1==0);

	// to do

	return *this;

}


Error RTPHintSample:: init (MP4TrackInfo_p pmtiMP4HintTrackInfo,
	unsigned long ulHintSampleIdentifier, Tracer_p ptTracer)

{

	unsigned long					ulPropertiesSize;
//	MP4Atom_p						pmaAtom;


	if (_stRTPHintSampleStatus == RHS_INITIALIZED ||
		pmtiMP4HintTrackInfo == (MP4TrackInfo_p) NULL ||
		ptTracer == (Tracer_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);

		return err;
	}

	_pmtiMP4HintTrackInfo			= pmtiMP4HintTrackInfo;
	_ulHintSampleIdentifier			= ulHintSampleIdentifier;

	_ptTracer						= ptTracer;

	// statistics: packets number for all the hint tracks
	if (_pmtiMP4HintTrackInfo -> getNumpAtom (
		&_pmaHintNumpAtom, true) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETNUMPATOM_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}
	/*
	if (_pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:nump:0",
		true, &pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "udta:0:hinf:0:nump:0");
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}
	else
		_pmaNumpAtom		= (MP4NumpAtom_p) pmaAtom;
	*/

	if (createProperties (&ulPropertiesSize) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTSAMPLE_CREATEPROPERTIES_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	_stRTPHintSampleStatus			= RHS_INITIALIZED;


	return errNoError;
}


Error RTPHintSample:: init (MP4RootAtom_p pmaRootAtom,
	MP4TrackInfo_p pmtiMP4HintTrackInfo,
	unsigned char *pucRTPHintSampleBuffer,
	unsigned long ulMaxHintSampleSize,
	unsigned long ulHintSampleIdentifier,
	RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
	Tracer_p ptTracer)

{

	unsigned long					ulPropertiesSize;
	#ifdef WIN32
		__int64							ullRTPHintSampleOffset;
	#else
		unsigned long long				ullRTPHintSampleOffset;
	#endif
	unsigned long					ulRTPHintSampleLength;
	unsigned long					ulRTPHintSampleDescriptionIndex;
	unsigned long					ulPacketsNumber;
	unsigned long					ulRTPHintPacketIndex;
	unsigned long					ulRTPHintPacketSize;
	// MP4Atom_p						pmaAtom;
	MP4StscAtom_p					pmaHintStscAtom;
	MP4StszAtom_p					pmaHintStszAtom;
	MP4StcoAtom_p					pmaHintStcoAtom;


	if (_stRTPHintSampleStatus == RHS_INITIALIZED ||
		pmaRootAtom == (MP4RootAtom_p) NULL ||
		pmtiMP4HintTrackInfo == (MP4TrackInfo_p) NULL ||
		ptTracer == (Tracer_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);

		return err;
	}

	_pmtiMP4HintTrackInfo			= pmtiMP4HintTrackInfo;
	_ulHintSampleIdentifier			= ulHintSampleIdentifier;

	_ptTracer						= ptTracer;

	// statistics: packets number for all the hint tracks
	if (_pmtiMP4HintTrackInfo -> getNumpAtom (
		&_pmaHintNumpAtom, true) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETNUMPATOM_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}
	/*
	if (_pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:nump:0",
		true, &pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "udta:0:hinf:0:nump:0");
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}
	else
		_pmaNumpAtom		= (MP4NumpAtom_p) pmaAtom;
	*/

	if (_pmtiMP4HintTrackInfo -> getStscAtom (
		&pmaHintStscAtom, true) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSTSCATOM_FAILED);
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

	if (_pmtiMP4HintTrackInfo -> getStcoAtom (
		&pmaHintStcoAtom, true) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETSTCOATOM_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (RTPUtility:: getSampleInfoFromSampleNumber (
		pmaHintStscAtom, pmaHintStszAtom,
		pmaHintStcoAtom,
		_ulHintSampleIdentifier, &ullRTPHintSampleOffset,
		&ulRTPHintSampleLength, &ulRTPHintSampleDescriptionIndex,
		prsrtRTPStreamRealTimeInfo,
		_ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPUTILITY_GETSAMPLEINFOFROMSAMPLENUMBER_FAILED,
			1, _ulHintSampleIdentifier);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if ((pucRTPHintSampleBuffer = new unsigned char [ulRTPHintSampleLength]) ==
		(unsigned char *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}
	*/

	if (MP4Utility:: readFromMdatAtom (pmaRootAtom,
		ullRTPHintSampleOffset, pucRTPHintSampleBuffer,
		ulRTPHintSampleLength, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UTILITY_READFROMMDATATOM_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		// delete [] pucRTPHintSampleBuffer;
		// pucRTPHintSampleBuffer 				 = (unsigned char *) NULL;

		return err;
	}

	/*
	{
		#ifdef WIN32
			__int64					ullBytesToRead;
		#else
			unsigned long long		ullBytesToRead;
		#endif

		if (MP4Utility:: getPointerFromMdatAtom (pmaRootAtom,
			ullRTPHintSampleOffset, &pucRTPHintSampleBuffer,
			&ullBytesToRead, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_GETPOINTERFROMMDATATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	*/

	if (getProperties (pucRTPHintSampleBuffer, &ulPropertiesSize) !=
		errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTSAMPLE_GETPROPERTIES_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

 		// delete [] pucRTPHintSampleBuffer;
		// pucRTPHintSampleBuffer 				 = (unsigned char *) NULL;

		return err;
	}

	if (_mui16pPacketsNumber. getValue (&ulPacketsNumber, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		// delete [] pucRTPHintSampleBuffer;
		// pucRTPHintSampleBuffer 				 = (unsigned char *) NULL;

		return err;
	}

	pucRTPHintSampleBuffer		= pucRTPHintSampleBuffer + ulPropertiesSize;

	for (ulRTPHintPacketIndex = 0; ulRTPHintPacketIndex < ulPacketsNumber;
		ulRTPHintPacketIndex++)
	{
		if (getHintPacket (pucRTPHintSampleBuffer, &ulRTPHintPacketSize) !=
			errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTSAMPLE_GETHINTPACKET_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			// delete [] pucRTPHintSampleBuffer;
			// pucRTPHintSampleBuffer 				 = (unsigned char *) NULL;

			return err;
		}

		pucRTPHintSampleBuffer			+= ulRTPHintPacketSize;
	}

	// delete [] pucRTPHintSampleBuffer;
	// pucRTPHintSampleBuffer 				 = (unsigned char *) NULL;

	_stRTPHintSampleStatus			= RHS_INITIALIZED;


	return errNoError;
}


Error RTPHintSample:: finish (void)

{

	if (_stRTPHintSampleStatus != RHS_INITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_vRTPHintPackets. size () > 0)
	{
		std:: vector<RTPHintPacket_p>:: const_iterator	it;
		RTPHintPacket_p									pRTPHintPacket;


		for (it = _vRTPHintPackets. begin (); it != _vRTPHintPackets. end ();
			++it)
		{
			pRTPHintPacket			= *it;

			if (pRTPHintPacket -> finish () != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTPACKET_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			delete pRTPHintPacket;
			pRTPHintPacket			= (RTPHintPacket_p) NULL;

		}

		_vRTPHintPackets. clear ();
	}

	_stRTPHintSampleStatus			= RHS_BUILDED;


	return errNoError;
}


Error RTPHintSample:: getState (
	RTPHintSampleStatus_p psRTPHintSampleState)

{

	if (psRTPHintSampleState == (RTPHintSampleStatus_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);

		return err;
	}

	*psRTPHintSampleState		= _stRTPHintSampleStatus;


	return errNoError;
}


Error RTPHintSample:: createProperties (unsigned long *pulPropertiesSize)

{

	unsigned long				ulValue;
	Boolean_t					bIsImplicitProperty;


	if (_stRTPHintSampleStatus == RHS_INITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	ulValue							= 0;
	bIsImplicitProperty				= false;

	// 0: PacketsNumber
	if (_mui16pPacketsNumber. init ("PacketsNumber", 1, &bIsImplicitProperty,
		&ulValue, _ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// 1: Reserved
	/*
	{
		unsigned char				**pucReserved;


		if ((pucReserved = new unsigned char * [1]) == (unsigned char **) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		if ((pucReserved [0] = new unsigned char [2]) == (unsigned char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucReserved;
			pucReserved				= (unsigned char **) NULL;

			return err;
		}

		memset (pucReserved [0], 0, 2);
		if (_mbpReserved. init ("Reserved", 1, &bIsImplicitProperty,
			2, pucReserved, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			delete [] pucReserved [0];
			pucReserved [0]			= (unsigned char *) NULL;

			delete [] pucReserved;
			pucReserved				= (unsigned char **) NULL;

			return err;
		}

		delete [] pucReserved [0];
		pucReserved [0]				= (unsigned char *) NULL;

		delete [] pucReserved;
		pucReserved					= (unsigned char **) NULL;
	}
	*/
	{
		unsigned char				*pucReserved [1]	=
			{ (unsigned char *) "\0\0" };


		if (_mbpReserved. init ("Reserved", 1, &bIsImplicitProperty,
			2, pucReserved, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4BYTESPROPERTY_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}
	}

	if (getPropertiesSize (pulPropertiesSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETPROPERTIESSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintSample:: getProperties (
	const unsigned char *pucRTPHintSampleBuffer,
	unsigned long *pulPropertiesSize)

{

	unsigned long					ulSize;


	if (_stRTPHintSampleStatus == RHS_INITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*pulPropertiesSize					= 0;

	// 0: PacketsNumber
	if (_mui16pPacketsNumber. init ("PacketsNumber", 1,
		pucRTPHintSampleBuffer + *pulPropertiesSize,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER8BITSPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui16pPacketsNumber. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	// 1: Reserved
	if (_mbpReserved. init ("Reserved", 1, 2,
		pucRTPHintSampleBuffer + *pulPropertiesSize,
		_ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (getPropertiesSize (pulPropertiesSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETPROPERTIESSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintSample:: createRTPHintPacket (RTPHintPacket_p *prpRTPHintPacket)

{

	RTPHintPacket_p						prpLocalRTPHintPacket;


	if (_stRTPHintSampleStatus != RHS_INITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((prpLocalRTPHintPacket = new RTPHintPacket_t) == (RTPHintPacket_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (prpLocalRTPHintPacket -> init (_ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete prpLocalRTPHintPacket;
		prpLocalRTPHintPacket				= (RTPHintPacket_p) NULL;

		return err;
	}

	if (incrementPacketsNumber (1) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (prpLocalRTPHintPacket -> finish () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPHINTPACKET_INIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		delete prpLocalRTPHintPacket;
		prpLocalRTPHintPacket				= (RTPHintPacket_p) NULL;

		return err;
	}

	_vRTPHintPackets. insert (_vRTPHintPackets. end (), prpLocalRTPHintPacket);

	if (_pmaHintNumpAtom -> incrementPackets (1) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*prpRTPHintPacket				= prpLocalRTPHintPacket;


	return errNoError;
}


Error RTPHintSample:: getHintPacket (
	const unsigned char *pucRTPHintPacketBuffer,
	unsigned long *pulRTPHintPacketSize)

{

	RTPHintPacket_p						prpLocalRTPHintPacket;


	if (_stRTPHintSampleStatus == RHS_INITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((prpLocalRTPHintPacket = new RTPHintPacket_t) == (RTPHintPacket_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (prpLocalRTPHintPacket -> init (pucRTPHintPacketBuffer,
		pulRTPHintPacketSize, _ptTracer) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKET_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete prpLocalRTPHintPacket;
		prpLocalRTPHintPacket				= (RTPHintPacket_p) NULL;

		return err;
	}

	_vRTPHintPackets. insert (_vRTPHintPackets. end (), prpLocalRTPHintPacket);


	return errNoError;
}


Error RTPHintSample:: getPropertiesSize (unsigned long *pulPropertiesSize)

{

	unsigned long					ulSize;


	*pulPropertiesSize					= 0;

	if (_mui16pPacketsNumber. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UINTEGER16BITSPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;

	if (_mbpReserved. getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}
	*pulPropertiesSize					+= ulSize;


	return errNoError;
}


Error RTPHintSample:: getBytesNumberForMdat (
	unsigned long *pulBytesNumberForMdat,
	unsigned long *pulBytesNumberForEmbeddedData)

{

	if (_stRTPHintSampleStatus != RHS_INITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (getPropertiesSize (pulBytesNumberForMdat) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETPROPERTIESSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*pulBytesNumberForEmbeddedData				= 0;

	if (_vRTPHintPackets. size () > 0)
	{
		std:: vector<RTPHintPacket_p>:: const_iterator	it;
		RTPHintPacket_p								pRTPHintPacket;
		unsigned long								ulPacketBytesNumberForMdat;
		unsigned long						ulLocalBytesNumberForEmbeddedData;


		for (it = _vRTPHintPackets. begin ();
			it != _vRTPHintPackets. end (); ++it)
		{
			pRTPHintPacket			= *it;

			if (pRTPHintPacket -> getBytesNumberForMdat (
				&ulPacketBytesNumberForMdat,
				&ulLocalBytesNumberForEmbeddedData) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTPACKET_GETBYTESNUMBERFORMDAT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				return err;
			}

			*pulBytesNumberForMdat					+=
				ulPacketBytesNumberForMdat;
			*pulBytesNumberForEmbeddedData			+=
				ulLocalBytesNumberForEmbeddedData;
		}
	}


	return errNoError;
}


Error RTPHintSample:: appendToMdatAtom (MP4MdatAtom_p pmaMdatAtom,
	unsigned long *pulBytesNumberForMdat)

{

	unsigned char					*pucRTPHintSample;
	unsigned long					ulCurrentRTPHintSampleOffset;
	unsigned long					ulBytesNumberForEmbeddedData;
	unsigned long					ulCurrentRTPHintSampleOffsetForEmbeddedData;


	if (_stRTPHintSampleStatus != RHS_INITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// pulBytesNumberForMdat includes embedded data
	if (getBytesNumberForMdat (pulBytesNumberForMdat,
		&ulBytesNumberForEmbeddedData) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTSAMPLE_GETBYTESNUMBERFORMDAT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	{
		Message msg = RTPMessages (__FILE__, __LINE__,
			RTP_RTPHINTSAMPLE_HINTSAMPLEBYTESNUMBER, 2,
			(long) _ulHintSampleIdentifier, (long) (*pulBytesNumberForMdat));
		_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if ((pucRTPHintSample = new unsigned char [*pulBytesNumberForMdat]) ==
		(unsigned char *) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	ulCurrentRTPHintSampleOffset						= 0;
	ulCurrentRTPHintSampleOffsetForEmbeddedData			=
		*pulBytesNumberForMdat - ulBytesNumberForEmbeddedData;

	if (_mui16pPacketsNumber. getValue (
		pucRTPHintSample + ulCurrentRTPHintSampleOffset, 0) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] pucRTPHintSample;
		pucRTPHintSample					= (unsigned char *) NULL;

		return err;
	}
	ulCurrentRTPHintSampleOffset				+= 2;

	if (_mbpReserved. getData (0, 2, 0,
		pucRTPHintSample + ulCurrentRTPHintSampleOffset) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] pucRTPHintSample;
		pucRTPHintSample					= (unsigned char *) NULL;

		return err;
	}
	ulCurrentRTPHintSampleOffset				+= 2;

	{
		Message msg = RTPMessages (__FILE__, __LINE__,
			RTP_RTPHINTSAMPLE_HINTSAMPLEPACKETSNUMBER, 2,
			(long) _ulHintSampleIdentifier, (long) (_vRTPHintPackets. size ()));
		_ptTracer -> trace (Tracer:: TRACER_LDBG5, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (_vRTPHintPackets. size () > 0)
	{
		std:: vector<RTPHintPacket_p>:: const_iterator	it;
		RTPHintPacket_p									pRTPHintPacket;


		for (it = _vRTPHintPackets. begin (); it != _vRTPHintPackets. end ();
			++it)
		{
			pRTPHintPacket			= *it;

			// this method write the packet including the embedded data
			// at the end of the hint sample
			if (pRTPHintPacket -> getPacket (pucRTPHintSample,
				&ulCurrentRTPHintSampleOffset,
				&ulCurrentRTPHintSampleOffsetForEmbeddedData,
				*pulBytesNumberForMdat) != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPHINTPACKET_GETPACKET_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				delete [] pucRTPHintSample;
				pucRTPHintSample					= (unsigned char *) NULL;

				return err;
			}
		}
	}

	if (pmaMdatAtom -> appendData (pucRTPHintSample, *pulBytesNumberForMdat) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4MDATATOM_APPENDDATA_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] pucRTPHintSample;
		pucRTPHintSample					= (unsigned char *) NULL;

		return err;
	}

	delete [] pucRTPHintSample;
	pucRTPHintSample					= (unsigned char *) NULL;


	return errNoError;
}


Error RTPHintSample:: incrementPacketsNumber (long lIncrement)

{

	if (_stRTPHintSampleStatus != RHS_INITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_mui16pPacketsNumber. incrementValue (0, lIncrement) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintSample:: getHintSampleIdentifier (
	unsigned long *pulHintSampleIdentifier)

{

	if (_stRTPHintSampleStatus != RHS_INITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*pulHintSampleIdentifier				= _ulHintSampleIdentifier;


	return errNoError;
}


Error RTPHintSample:: getPacketsNumber (unsigned long *pulPacketsNumber)

{

	if (_stRTPHintSampleStatus != RHS_INITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*pulPacketsNumber				= _vRTPHintPackets. size ();


	return errNoError;
}


Error RTPHintSample:: getHintPacket (unsigned long ulPacketIndex,
	RTPHintPacket_p *prhpHintPacket)

{

	if (_stRTPHintSampleStatus != RHS_INITIALIZED)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*prhpHintPacket					= _vRTPHintPackets [ulPacketIndex];


	return errNoError;
}

