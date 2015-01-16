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


#include "MP4TrackInfo.h"
#include "MP4PaytAtom.h"
#include "MP4SnroAtom.h"
#include "MP4TsroAtom.h"
#include "MP4UrlAtom.h"
#include "MP4Utility.h"
#include <assert.h>
#include <stdio.h>


MP4TrackInfo:: MP4TrackInfo (void)

{

}


MP4TrackInfo:: ~MP4TrackInfo (void)

{

}


Error MP4TrackInfo:: init (
	MP4TrakAtom_p pmaTrakAtom,
	Boolean_t bSaveErrorOfOptionalAtom,
	Tracer_p ptTracer)

{

	unsigned long			ulTrackIdentifier;


	if (_mtMP4TrackInfo. init (PMutex:: MUTEX_RECURSIVE) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	_pmaTrakAtom			= pmaTrakAtom;

	strcpy (_pHandlerType, "");
	strcpy (_pTrackName, "");

	_ptTracer				= ptTracer;

	_pmaCttsAtom		= (MP4CttsAtom_p) NULL;
	_pmaD263Atom		= (MP4D263Atom_p) NULL;
	_pmaAACAudioDecoderConfigDescr	=
		(MP4DecoderConfigDescr_p) NULL;
	_pmaMPEG4VideoDecoderConfigDescr	=
		(MP4DecoderConfigDescr_p) NULL;
	_pmaDimmAtom		= (MP4DimmAtom_p) NULL;
	_pmaDinfAtom		= (MP4DinfAtom_p) NULL;
	_pmaDmaxAtom		= (MP4DmaxAtom_p) NULL;
	_pmaDmedAtom		= (MP4DmedAtom_p) NULL;
	_pmaDrefAtom		= (MP4DrefAtom_p) NULL;
	_pmaElstAtom		= (MP4ElstAtom_p) NULL;
	_pmaHdlrAtom		= (MP4HdlrAtom_p) NULL;
	_pmaHinfAtom		= (MP4HinfAtom_p) NULL;
	_pmaHintAtom		= (MP4HintAtom_p) NULL;
	_pmaHmhdAtom		= (MP4HmhdAtom_p) NULL;
	_pmaMaxrAtom		= (MP4MaxrAtom_p) NULL;
	_pmaMdhdAtom		= (MP4MdhdAtom_p) NULL;
	_pmaMinfAtom		= (MP4MinfAtom_p) NULL;
	_pmaMp4vAtom		= (MP4Mp4vAtom_p) NULL;
	_pmaNumpAtom		= (MP4NumpAtom_p) NULL;
	_pmaPaytAtom		= (MP4PaytAtom_p) NULL;
	_pmaPmaxAtom		= (MP4PmaxAtom_p) NULL;
	_pmaRtpAtom			= (MP4RtpAtom_p) NULL;
	_pmaS263Atom		= (MP4S263Atom_p) NULL;
	_pmaSamrAtom		= (MP4SamrAtom_p) NULL;
	_pmaSawbAtom		= (MP4SawbAtom_p) NULL;
	_pmaSdpAtom			= (MP4SdpAtom_p) NULL;
	_pmaSnroAtom		= (MP4SnroAtom_p) NULL;
	_pmaStblAtom		= (MP4StblAtom_p) NULL;
	_pmaStcoAtom		= (MP4StcoAtom_p) NULL;
	_pmaStscAtom		= (MP4StscAtom_p) NULL;
	_pmaStsdAtom		= (MP4StsdAtom_p) NULL;
	_pmaStssAtom		= (MP4StssAtom_p) NULL;
	_pmaStszAtom		= (MP4StszAtom_p) NULL;
	_pmaSttsAtom		= (MP4SttsAtom_p) NULL;
	_pmaTimsAtom		= (MP4TimsAtom_p) NULL;
	_pmaTkhdAtom		= (MP4TkhdAtom_p) NULL;
	_pmaTpylAtom		= (MP4TpylAtom_p) NULL;
	_pmaTrpyAtom		= (MP4TrpyAtom_p) NULL;
	_pmaTsroAtom		= (MP4TsroAtom_p) NULL;

	_errCttsSearch		= errNoError;
	_errAACAudioDecoderConfigDescrSearch	= errNoError;
	_errMPEG4VideoDecoderConfigDescrSearch	= errNoError;
	_errDimmSearch		= errNoError;
	_errDmaxSearch		= errNoError;
	_errDmedSearch		= errNoError;
	_errHinfSearch		= errNoError;
	_errHintSearch		= errNoError;
	_errHmhdSearch		= errNoError;
	_errMaxrSearch		= errNoError;
	_errMp4vSearch		= errNoError;
	_errNumpSearch		= errNoError;
	_errPaytSearch		= errNoError;
	_errPmaxSearch		= errNoError;
	_errRtpSearch		= errNoError;
	_errS263Search		= errNoError;
	_errSamrSearch		= errNoError;
	_errSawbSearch		= errNoError;
	_errSdpSearch		= errNoError;
	_errSnroSearch		= errNoError;
	_errStblSearch		= errNoError;
	_errStcoSearch		= errNoError;
	_errStssSearch		= errNoError;
	_errTpylSearch		= errNoError;
	_errTrpySearch		= errNoError;
	_errTsroSearch		= errNoError;

	_bIsObjectTypeIndicationInitialized		= false;
	_ulObjectTypeIndication					= (unsigned long) -1;

	_cCodec					= MP4Atom:: MP4F_CODEC_UNKNOWN;

	_ulTrackIdentifier		= (unsigned long) -1;
	_ulMdhdTimeScale		= (unsigned long) -1;
	_ulTimsTimeScale		= (unsigned long) -1;
	_dDuration				= -1.0;
	_ulSamplesNumber		= (unsigned long) -1;
	_ulMaxSampleSize		= (unsigned long) -1;
	_ulAvgBitRate			= (unsigned long) -1;

	_bIs3GPPESConfigurrationInitialized			= false;
	_bIsISMAESConfigurrationInitialized			= false;

	if (getTrackIdentifier (&ulTrackIdentifier) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETTRACKIDENTIFIER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4TrackInfo. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (getHandlerType (_pHandlerType) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETHANDLERTYPE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4TrackInfo. finish () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// initialize _pTrackName
	{
		MP4SdpAtom_p				pmaSdpAtom;
		Buffer_t					bSdp;
		const char					*pBeginMediaControl;
		const char					*pEndMediaControl;
		Error_t						errGetSdp;

		if (!strcmp (_pHandlerType, MP4_HINT_TRACK_TYPE))
		{
			if ((errGetSdp = getSdpAtom (&pmaSdpAtom,
				bSaveErrorOfOptionalAtom)) != errNoError)
			{
				if ((long) errGetSdp != MP4F_MP4ATOM_ATOMNOTFOUND)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETSDPATOM_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtMP4TrackInfo. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
				else
					sprintf (_pTrackName, "trackID/%lu",
						ulTrackIdentifier);
			}
			else
			{
				if (bSdp. init () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_INIT_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtMP4TrackInfo. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pmaSdpAtom -> getSdpText (&bSdp) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bSdp. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtMP4TrackInfo. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if ((pBeginMediaControl = (const char *) strstr (
					(const char *) bSdp, "a=control:")) ==
					(const char *) NULL)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILE_SDPTEXTWRONG,
						1, (const char *) bSdp);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bSdp. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtMP4TrackInfo. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
				pBeginMediaControl			+= strlen ("a=control:");

				if ((pEndMediaControl = (const char *) strchr (
					pBeginMediaControl, '\r')) == (const char *) NULL &&
					(pEndMediaControl = (const char *) strchr (
					pBeginMediaControl, '\n')) == (const char *) NULL &&
					(pEndMediaControl = (const char *) strchr (
					pBeginMediaControl, '\0')) == (const char *) NULL)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4FILE_SDPTEXTWRONG, 1, (const char *) bSdp);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (bSdp. finish () != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (_mtMP4TrackInfo. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				memcpy (_pTrackName, pBeginMediaControl,
					pEndMediaControl - pBeginMediaControl);
				_pTrackName [
					pEndMediaControl - pBeginMediaControl]		= '\0';

				if (bSdp. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtMP4TrackInfo. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return errNoError;
				}
			}
		}
		else
		{
			sprintf (_pTrackName, "trackID/%lu",
				ulTrackIdentifier);
		}
	}

	{
		// initialize object type indication & codec
		// _ulObjectTypeIndication			=
		//	(unsigned long) -1;
		// _bIsObjectTypeIndicationInitialized					=
		//	false;

		// _cCodecUsed				= MP4Atom:: MP4F_CODEC_UNKNOWN;

		if (!strcmp (_pHandlerType, MP4_AUDIO_TRACK_TYPE))
		{
			MP4DecoderConfigDescr_p			pmaMediaDecoderConfigDescr;
			Error_t							errGet;
			MP4SamrAtom_p					pmaSamrAtom;
			MP4SawbAtom_p					pmaSawbAtom;
			// MP4Atom_p						pmaAtom;


			// this kind of path is good for ISO codec
			if ((errGet = getAACAudioDecoderConfigDescr (
				&pmaMediaDecoderConfigDescr, bSaveErrorOfOptionalAtom)) !=
				errNoError)
			{
				if ((long) errGet != MP4F_MP4ATOM_ATOMNOTFOUND)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETAACAUDIODECODERCONFIGDESCR_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtMP4TrackInfo. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				if (pmaMediaDecoderConfigDescr -> getObjectTypeIndication (
					&_ulObjectTypeIndication) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtMP4TrackInfo. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				_bIsObjectTypeIndicationInitialized					=
					true;

				_cCodec		= MP4Atom:: MP4F_CODEC_AAC;

				return errNoError;
			}

			/*
			strcpy (pAtomPath,
		"mdia:0:minf:0:stbl:0:stsd:0:mp4a:0:esds:0:ESDescr:0:DecoderConfigDescr:0");

			if ((errSearch = (mtiMP4TrackInfo. _pmaTrakAtom) -> searchAtom (
				pAtomPath, false, &pmaAtom)) != errNoError)
			{
				//Error err = MP4FileErrors (__FILE__, __LINE__,
				//	MP4F_MP4ATOM_SEARCHATOM_FAILED,
				//	1, pAtomPath);
				//_ptTracer -> trace (Tracer:: TRACER_LERRR,
				//	(const char *) err, __FILE__, __LINE__);

				//return err;
			}
			else
				pmaMediaDecoderConfigDescr		=
					(MP4DecoderConfigDescr_p) pmaAtom;
			*/

			if ((errGet = getSamrAtom (&pmaSamrAtom,
				bSaveErrorOfOptionalAtom)) != errNoError)
			{
				if ((long) errGet != MP4F_MP4ATOM_ATOMNOTFOUND)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETSAMRATOM_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtMP4TrackInfo. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				_cCodec					= MP4Atom:: MP4F_CODEC_AMRNB;

				return errNoError;
			}

			/*
			strcpy (pAtomPath, "mdia:0:minf:0:stbl:0:stsd:0:samr:0");

			if ((errSearch = (mtiMP4TrackInfo. _pmaTrakAtom) -> searchAtom (
				pAtomPath, false, &pmaAtom)) != errNoError)
			{
				// Error err = MP4FileErrors (__FILE__, __LINE__,
				//	MP4F_MP4FILE_TRACKNOTRECONIZED_FAILED,
				//	1, mtiMP4TrackInfo. _ulTrackIdentifier);
				//_ptTracer -> trace (Tracer:: TRACER_LERRR,
				//	(const char *) err, __FILE__, __LINE__);

				// return err;
			}
			else
				pmaSamrAtom		= (MP4SamrAtom_p) pmaAtom;
			*/

			if ((errGet = getSawbAtom (&pmaSawbAtom,
				bSaveErrorOfOptionalAtom)) != errNoError)
			{
				if ((long) errGet != MP4F_MP4ATOM_ATOMNOTFOUND)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETSAWBATOM_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtMP4TrackInfo. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				_cCodec				= MP4Atom:: MP4F_CODEC_AMRWB;

				return errNoError;
			}

			/*
			strcpy (pAtomPath, "mdia:0:minf:0:stbl:0:stsd:0:sawb:0");

			if ((errSearch = (mtiMP4TrackInfo. _pmaTrakAtom) -> searchAtom (
				pAtomPath, false, &pmaAtom)) != errNoError)
			{
				// Error err = MP4FileErrors (__FILE__, __LINE__,
				//	MP4F_MP4FILE_TRACKNOTRECONIZED_FAILED,
				//	1, mtiMP4TrackInfo. _ulTrackIdentifier);
				// _ptTracer -> trace (Tracer:: TRACER_LERRR,
				//	(const char *) err, __FILE__, __LINE__);

				// return err;
			}
			else
				pmaSawbAtom		= (MP4SawbAtom_p) pmaAtom;
			*/
		}
		else if (!strcmp (_pHandlerType, MP4_VIDEO_TRACK_TYPE))
		{
			MP4DecoderConfigDescr_p			pmaMediaDecoderConfigDescr;
			MP4S263Atom_p					pmaS263Atom;
			Error_t							errGet;
			// MP4Atom_p						pmaAtom;

			
			// this kind of path is good for ISO codec
			if ((errGet = getMPEG4VideoDecoderConfigDescr (
				&pmaMediaDecoderConfigDescr, bSaveErrorOfOptionalAtom)) !=
				errNoError)
			{
				if ((long) errGet != MP4F_MP4ATOM_ATOMNOTFOUND)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETMPEG4VIDEODECODERCONFIGDESCR_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtMP4TrackInfo. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				if (pmaMediaDecoderConfigDescr -> getObjectTypeIndication (
					&_ulObjectTypeIndication) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtMP4TrackInfo. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				_bIsObjectTypeIndicationInitialized					=
					true;

				_cCodec		= MP4Atom:: MP4F_CODEC_MPEG4;

				return errNoError;
			}

			/*
			strcpy (pAtomPath,
		"mdia:0:minf:0:stbl:0:stsd:0:mp4v:0:esds:0:ESDescr:0:DecoderConfigDescr:0");

			if ((errSearch = (mtiMP4TrackInfo. _pmaTrakAtom) -> searchAtom (
				pAtomPath, false, &pmaAtom)) != errNoError)
			{
				// Error err = MP4FileErrors (__FILE__, __LINE__,
				//	MP4F_MP4ATOM_SEARCHATOM_FAILED,
				//	1, pAtomPath);
				// _ptTracer -> trace (Tracer:: TRACER_LERRR,
				//	(const char *) err, __FILE__, __LINE__);

				// return err;
			}
			else
				pmaMediaDecoderConfigDescr		=
					(MP4DecoderConfigDescr_p) pmaAtom;
			*/

			if ((errGet = getS263Atom (&pmaS263Atom,
				bSaveErrorOfOptionalAtom)) != errNoError)
			{
				if ((long) errGet != MP4F_MP4ATOM_ATOMNOTFOUND)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETS263ATOM_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (_mtMP4TrackInfo. finish () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_FINISH_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
			else
			{
				_cCodec					= MP4Atom:: MP4F_CODEC_H263;

				return errNoError;
			}

			/*
			strcpy (pAtomPath, "mdia:0:minf:0:stbl:0:stsd:0:s263:0");

			if ((errSearch = (mtiMP4TrackInfo. _pmaTrakAtom) -> searchAtom (
				pAtomPath, false, &pmaAtom)) != errNoError)
			{
				// Error err = MP4FileErrors (__FILE__, __LINE__,
				//	MP4F_MP4FILE_TRACKNOTRECONIZED_FAILED,
				//	1, mtiMP4TrackInfo. _ulTrackIdentifier);
				// _ptTracer -> trace (Tracer:: TRACER_LERRR,
				//	(const char *) err, __FILE__, __LINE__);

				// return err;
			}
			else
				pmaS263Atom		= (MP4S263Atom_p) pmaAtom;
			*/
		}
		else
		{
			// hint track is initialized by the other init
		}
	}


	return errNoError;
}


Error MP4TrackInfo:: finish (void)

{

	if (_mtMP4TrackInfo. finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getTrackName (
	char *pTrackName)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (pTrackName, _pTrackName);

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getHandlerType (
	char *pHandlerType)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (!strcmp (_pHandlerType, ""))
	{
		MP4HdlrAtom_p			pmaHdlrAtom;


		// hdlr is mandatory
		if (getHdlrAtom (&pmaHdlrAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHDLRATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaHdlrAtom -> getHandlerType (_pHandlerType,
			MP4F_MAXHANDLERTYPELENGTH) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4HDLRATOM_GETHANDLERTYPE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	strcpy (pHandlerType, _pHandlerType);

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getCttsAtom (
	MP4CttsAtom_p *pmaCttsAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaCttsAtom == (MP4CttsAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4StblAtom_p		pmaStblAtom;
		Error_t				errGeneric;


		if (_errCttsSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errCttsSearch;
		}

		// Stbl is mandatory
		if (getStblAtom (&pmaStblAtom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTBLATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// below the argument is false because ctts is not mandatory
		if ((errGeneric = pmaStblAtom -> searchAtom (
			"ctts:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "ctts:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errCttsSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaCttsAtom			= (MP4CttsAtom_p) pmaAtom;
	}

	*pmaCttsAtom			= _pmaCttsAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getD263Atom (
	MP4D263Atom_p *pmaD263Atom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaD263Atom == (MP4D263Atom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4S263Atom_p		pmaS263Atom;


		// S263 is not mandatory
		if (getS263Atom (&pmaS263Atom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETS263ATOM_FAILED);
			/*
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// D263 is mandatory
		if (pmaS263Atom -> searchAtom (
			"d263:0", true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "d263:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			_pmaD263Atom			= (MP4D263Atom_p) pmaAtom;
	}

	*pmaD263Atom			= _pmaD263Atom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getAACAudioDecoderConfigDescr (
	MP4DecoderConfigDescr_p *pmaDecoderConfigDescr,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaAACAudioDecoderConfigDescr == (MP4DecoderConfigDescr_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		Error_t				errGeneric;


		if (_errAACAudioDecoderConfigDescrSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errAACAudioDecoderConfigDescrSearch;
		}

		// below the argument is false because DecoderConfigDescr is not mandatory
		if ((errGeneric = _pmaTrakAtom -> searchAtom (
			"mdia:0:minf:0:stbl:0:stsd:0:mp4a:0:esds:0:ESDescr:0:DecoderConfigDescr:0",
			false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mdia:0:minf:0:stbl:0:stsd:0:mp4a:0:esds:0:ESDescr:0:DecoderConfigDescr:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errAACAudioDecoderConfigDescrSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaAACAudioDecoderConfigDescr	= (MP4DecoderConfigDescr_p) pmaAtom;
	}

	*pmaDecoderConfigDescr			= _pmaAACAudioDecoderConfigDescr;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getMPEG4VideoDecoderConfigDescr (
	MP4DecoderConfigDescr_p *pmaDecoderConfigDescr,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaMPEG4VideoDecoderConfigDescr == (MP4DecoderConfigDescr_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		Error_t				errGeneric;


		if (_errMPEG4VideoDecoderConfigDescrSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errMPEG4VideoDecoderConfigDescrSearch;
		}

		// below the argument is false because DecoderConfigDescr is not mandatory
		if ((errGeneric = _pmaTrakAtom -> searchAtom (
			"mdia:0:minf:0:stbl:0:stsd:0:mp4v:0:esds:0:ESDescr:0:DecoderConfigDescr:0",
			false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mdia:0:minf:0:stbl:0:stsd:0:mp4v:0:esds:0:ESDescr:0:DecoderConfigDescr:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errMPEG4VideoDecoderConfigDescrSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaMPEG4VideoDecoderConfigDescr		= (MP4DecoderConfigDescr_p) pmaAtom;
	}

	*pmaDecoderConfigDescr			= _pmaMPEG4VideoDecoderConfigDescr;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getDimmAtom (
	MP4DimmAtom_p *pmaDimmAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaDimmAtom == (MP4DimmAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4HinfAtom_p		pmaHinfAtom;
		Error_t				errGeneric;


		if (_errDimmSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errDimmSearch;
		}

		// below the argument is false because hinf is not mandatory
		if ((errGeneric = getHinfAtom (&pmaHinfAtom,
			bSaveErrorOfOptionalAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHINFATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errGeneric;
		}

		// below the argument is false because dimm is not mandatory
		if ((errGeneric = pmaHinfAtom -> searchAtom (
			"dimm:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "dimm:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errDimmSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaDimmAtom			= (MP4DimmAtom_p) pmaAtom;
	}

	*pmaDimmAtom			= _pmaDimmAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getDinfAtom (
	MP4DinfAtom_p *pmaDinfAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaDinfAtom == (MP4DinfAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4MinfAtom_p		pmaMinfAtom;


		// minf is mandatory
		if (getMinfAtom (&pmaMinfAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMINFATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// dinf is mandatory
		if (pmaMinfAtom -> searchAtom (
			"dinf:0", true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "dinf:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			_pmaDinfAtom			= (MP4DinfAtom_p) pmaAtom;
	}

	*pmaDinfAtom			= _pmaDinfAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getDmaxAtom (
	MP4DmaxAtom_p *pmaDmaxAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaDmaxAtom == (MP4DmaxAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4HinfAtom_p		pmaHinfAtom;
		Error_t				errGeneric;


		if (_errDmaxSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errDmaxSearch;
		}

		// below the argument is false because hinf is not mandatory
		if ((errGeneric = getHinfAtom (&pmaHinfAtom,
			bSaveErrorOfOptionalAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHINFATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errGeneric;
		}

		// below the argument is false because dmax is not mandatory
		if ((errGeneric = pmaHinfAtom -> searchAtom (
			"dmax:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "dmax:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errDmaxSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaDmaxAtom			= (MP4DmaxAtom_p) pmaAtom;
	}

	*pmaDmaxAtom			= _pmaDmaxAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getDmedAtom (
	MP4DmedAtom_p *pmaDmedAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaDmedAtom == (MP4DmedAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4HinfAtom_p		pmaHinfAtom;
		Error_t				errGeneric;


		if (_errDmedSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errDmedSearch;
		}

		// below the argument is false because hinf is not mandatory
		if ((errGeneric = getHinfAtom (&pmaHinfAtom,
			bSaveErrorOfOptionalAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHINFATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errGeneric;
		}

		// below the argument is false because dmed is not mandatory
		if ((errGeneric = pmaHinfAtom -> searchAtom (
			"dmed:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "dmed:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errDmedSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaDmedAtom			= (MP4DmedAtom_p) pmaAtom;
	}

	*pmaDmedAtom			= _pmaDmedAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getDrefAtom (
	MP4DrefAtom_p *pmaDrefAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaDrefAtom == (MP4DrefAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4DinfAtom_p		pmaDinfAtom;


		// dinf is mandatory
		if (getDinfAtom (&pmaDinfAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETDINFATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// dref is mandaory
		if (pmaDinfAtom -> searchAtom (
			"dref:0", true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "dref:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			_pmaDrefAtom			= (MP4DrefAtom_p) pmaAtom;
	}

	*pmaDrefAtom			= _pmaDrefAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getElstAtom (
	MP4ElstAtom_p *pmaElstAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaElstAtom == (MP4ElstAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;


		// elst is mandatory
		if (_pmaTrakAtom -> searchAtom (
			"edts:0:elst:0", true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "edts:0:elst:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			_pmaElstAtom			= (MP4ElstAtom_p) pmaAtom;
	}

	*pmaElstAtom			= _pmaElstAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getHdlrAtom (
	MP4HdlrAtom_p *pmaHdlrAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaHdlrAtom == (MP4HdlrAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;


		// hdlr is mandatory
		if (_pmaTrakAtom -> searchAtom (
			"mdia:0:hdlr:0", true,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mdia:0:hdlr:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			_pmaHdlrAtom			= (MP4HdlrAtom_p) pmaAtom;
	}

	*pmaHdlrAtom			= _pmaHdlrAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getHinfAtom (
	MP4HinfAtom_p *pmaHinfAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaHinfAtom == (MP4HinfAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		Error_t				errGeneric;


		if (_errHinfSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errHinfSearch;
		}

		// below the argument is false because hinf is not mandatory
		if ((errGeneric = _pmaTrakAtom -> searchAtom (
			"udta:0:hinf:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "udta:0:hinf:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errHinfSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaHinfAtom			= (MP4HinfAtom_p) pmaAtom;
	}

	*pmaHinfAtom			= _pmaHinfAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getHintAtom (
	MP4HintAtom_p *pmaHintAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaHintAtom == (MP4HintAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		Error_t				errGeneric;


		if (_errHintSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errHintSearch;
		}

		// below the argument is false because hint is not mandatory
		if ((errGeneric = _pmaTrakAtom -> searchAtom (
			"tref:0:hint:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "tref:0:hint:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errHintSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaHintAtom			= (MP4HintAtom_p) pmaAtom;
	}

	*pmaHintAtom			= _pmaHintAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getHmhdAtom (
	MP4HmhdAtom_p *pmaHmhdAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaHmhdAtom == (MP4HmhdAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4MinfAtom_p		pmaMinfAtom;
		Error_t				errGeneric;


		if (_errHmhdSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errHmhdSearch;
		}

		// minf is mandatory
		if (getMinfAtom (&pmaMinfAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMINFATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// below the argument is false because hmhd is not mandatory
		if ((errGeneric = pmaMinfAtom -> searchAtom (
			"hmhd:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "hmhd:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errHmhdSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaHmhdAtom			= (MP4HmhdAtom_p) pmaAtom;
	}

	*pmaHmhdAtom			= _pmaHmhdAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getMaxrAtom (
	MP4MaxrAtom_p *pmaMaxrAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaMaxrAtom == (MP4MaxrAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4HinfAtom_p		pmaHinfAtom;
		Error_t				errGeneric;


		if (_errMaxrSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errMaxrSearch;
		}

		// below the argument is false because hinf is not mandatory
		if ((errGeneric = getHinfAtom (&pmaHinfAtom,
			bSaveErrorOfOptionalAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHINFATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errGeneric;
		}

		// below the argument is false because maxr is not mandatory
		if ((errGeneric = pmaHinfAtom -> searchAtom (
			"maxr:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "maxr:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errMaxrSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaMaxrAtom			= (MP4MaxrAtom_p) pmaAtom;
	}

	*pmaMaxrAtom			= _pmaMaxrAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getMdhdAtom (
	MP4MdhdAtom_p *pmaMdhdAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaMdhdAtom == (MP4MdhdAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;


		// mdhd is mandatory
		if (_pmaTrakAtom -> searchAtom (
			"mdia:0:mdhd:0", true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mdia:0:mdhd:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			_pmaMdhdAtom			= (MP4MdhdAtom_p) pmaAtom;
	}

	*pmaMdhdAtom			= _pmaMdhdAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getMinfAtom (
	MP4MinfAtom_p *pmaMinfAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaMinfAtom == (MP4MinfAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;


		// minf is mandatory
		if (_pmaTrakAtom -> searchAtom (
			"mdia:0:minf:0", true,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mdia:0:minf:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			_pmaMinfAtom			= (MP4MinfAtom_p) pmaAtom;
	}

	*pmaMinfAtom			= _pmaMinfAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getMp4vAtom (
	MP4Mp4vAtom_p *pmaMp4vAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaMp4vAtom == (MP4Mp4vAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4StsdAtom_p		pmaStsdAtom;
		Error_t				errGeneric;


		if (_errMp4vSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errMp4vSearch;
		}

		// stsd is mandatory
		if (getStsdAtom (&pmaStsdAtom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// below the argument is false because rtp is not mandatory
		if ((errGeneric = pmaStsdAtom -> searchAtom (
			"mp4v:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mp4v:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errMp4vSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaMp4vAtom			= (MP4Mp4vAtom_p) pmaAtom;
	}

	*pmaMp4vAtom			= _pmaMp4vAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getNumpAtom (
	MP4NumpAtom_p *pmaNumpAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaNumpAtom == (MP4NumpAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4HinfAtom_p		pmaHinfAtom;
		Error_t				errGeneric;


		if (_errNumpSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errNumpSearch;
		}

		// below the argument is false because hinf is not mandatory
		if ((errGeneric = getHinfAtom (&pmaHinfAtom,
			bSaveErrorOfOptionalAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHINFATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errGeneric;
		}

		// below the argument is false because nump is not mandatory
		if ((errGeneric = pmaHinfAtom -> searchAtom (
			"nump:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "nump:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errNumpSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaNumpAtom			= (MP4NumpAtom_p) pmaAtom;
	}

	*pmaNumpAtom			= _pmaNumpAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getPaytAtom (
	MP4PaytAtom_p *pmaPaytAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaPaytAtom == (MP4PaytAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4HinfAtom_p		pmaHinfAtom;
		Error_t				errGeneric;


		if (_errPaytSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errPaytSearch;
		}

		// below the argument is false because hinf is not mandatory
		if ((errGeneric = getHinfAtom (&pmaHinfAtom,
			bSaveErrorOfOptionalAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHINFATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errGeneric;
		}

		// below the argument is false because dmed is not mandatory
		if ((errGeneric = pmaHinfAtom -> searchAtom (
			"payt:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "payt:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errPaytSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaPaytAtom			= (MP4PaytAtom_p) pmaAtom;
	}

	*pmaPaytAtom			= _pmaPaytAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getPmaxAtom (
	MP4PmaxAtom_p *pmaPmaxAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaPmaxAtom == (MP4PmaxAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4HinfAtom_p		pmaHinfAtom;
		Error_t				errGeneric;


		if (_errPmaxSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errPmaxSearch;
		}

		// below the argument is false because hinf is not mandatory
		if ((errGeneric = getHinfAtom (&pmaHinfAtom,
			bSaveErrorOfOptionalAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHINFATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errGeneric;
		}

		// below the argument is false because pmax is not mandatory
		if ((errGeneric = pmaHinfAtom -> searchAtom (
			"pmax:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "pmax:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errPmaxSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaPmaxAtom			= (MP4PmaxAtom_p) pmaAtom;
	}

	*pmaPmaxAtom			= _pmaPmaxAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getRtpAtom (
	MP4RtpAtom_p *pmaRtpAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaRtpAtom == (MP4RtpAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4StsdAtom_p		pmaStsdAtom;
		Error_t				errGeneric;


		if (_errRtpSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errRtpSearch;
		}

		// stsd is mandatory
		if (getStsdAtom (&pmaStsdAtom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// below the argument is false because rtp is not mandatory
		if ((errGeneric = pmaStsdAtom -> searchAtom (
			"rtp :0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "rtp :0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errRtpSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaRtpAtom			= (MP4RtpAtom_p) pmaAtom;
	}

	*pmaRtpAtom			= _pmaRtpAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getS263Atom (
	MP4S263Atom_p *pmaS263Atom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaS263Atom == (MP4S263Atom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4StsdAtom_p		pmaStsdAtom;
		Error_t				errGeneric;


		if (_errS263Search != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errS263Search;
		}

		// stsd is mandatory
		if (getStsdAtom (&pmaStsdAtom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// below the argument is false because s263 is not mandatory
		if ((errGeneric = pmaStsdAtom -> searchAtom (
			"s263:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "s263:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errS263Search			= errGeneric;

			return errGeneric;
		}
		else
			_pmaS263Atom			= (MP4S263Atom_p) pmaAtom;
	}

	*pmaS263Atom			= _pmaS263Atom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getSamrAtom (
	MP4SamrAtom_p *pmaSamrAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaSamrAtom == (MP4SamrAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4StsdAtom_p		pmaStsdAtom;
		Error_t				errGeneric;


		if (_errSamrSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errSamrSearch;
		}

		// stsd is mandatory
		if (getStsdAtom (&pmaStsdAtom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// below the argument is false because samr is not mandatory
		if ((errGeneric = pmaStsdAtom -> searchAtom (
			"samr:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "samr:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errSamrSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaSamrAtom			= (MP4SamrAtom_p) pmaAtom;
	}

	*pmaSamrAtom			= _pmaSamrAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getSawbAtom (
	MP4SawbAtom_p *pmaSawbAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaSawbAtom == (MP4SawbAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4StsdAtom_p		pmaStsdAtom;
		Error_t				errGeneric;


		if (_errSawbSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errSawbSearch;
		}

		// stsd is mandatory
		if (getStsdAtom (&pmaStsdAtom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// below the argument is false because sawb is not mandatory
		if ((errGeneric = pmaStsdAtom -> searchAtom (
			"sawb:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "sawb:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errSawbSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaSawbAtom			= (MP4SawbAtom_p) pmaAtom;
	}

	*pmaSawbAtom			= _pmaSawbAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getSdpAtom (
	MP4SdpAtom_p *pmaSdpAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaSdpAtom == (MP4SdpAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		Error_t				errGeneric;


		if (_errSdpSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errSdpSearch;
		}

		// below the argument is false because sdp is not mandatory
		if ((errGeneric = _pmaTrakAtom -> searchAtom (
			"udta:0:hnti:0:sdp :0", false,
			&pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "udta:0:hnti:0:sdp :0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errSdpSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaSdpAtom			= (MP4SdpAtom_p) pmaAtom;
	}

	*pmaSdpAtom			= _pmaSdpAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getSnroAtom (
	MP4SnroAtom_p *pmaSnroAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaSnroAtom == (MP4SnroAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4RtpAtom_p		pmaRtpAtom;
		Error_t				errGeneric;


		if (_errSnroSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errSnroSearch;
		}

		// below the argument is false because rtp is not mandatory
		//	Since Rtp is not mandatory we will return errGeneric
		//	because the calling method can handle the error
		if ((errGeneric = getRtpAtom (&pmaRtpAtom,
			bSaveErrorOfOptionalAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETRTPATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errGeneric;
		}

		// below the argument is false because snro is not mandatory
		if ((errGeneric = pmaRtpAtom -> searchAtom (
			"snro:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "snro:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errSnroSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaSnroAtom			= (MP4SnroAtom_p) pmaAtom;
	}

	*pmaSnroAtom			= _pmaSnroAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getStblAtom (
	MP4StblAtom_p *pmaStblAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaStblAtom == (MP4StblAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4MinfAtom_p		pmaMinfAtom;
		Error_t				errGeneric;


		if (_errStblSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errStblSearch;
		}

		// minf is mandatory
		if (getMinfAtom (&pmaMinfAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMINFATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// below the argument is false because stbl is not mandatory
		if ((errGeneric = pmaMinfAtom -> searchAtom (
			"stbl:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "stbl:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errStblSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaStblAtom			= (MP4StblAtom_p) pmaAtom;
	}

	*pmaStblAtom			= _pmaStblAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getStcoAtom (
	MP4StcoAtom_p *pmaStcoAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaStcoAtom == (MP4StcoAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4StblAtom_p		pmaStblAtom;
		Error_t				errGeneric;


		if (_errStcoSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errStcoSearch;
		}

		// Stbl is mandatory
		if (getStblAtom (&pmaStblAtom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTBLATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// below the argument is false because stco is not mandatory
		if ((errGeneric = pmaStblAtom -> searchAtom (
			"stco:0", false, &pmaAtom)) != errNoError)
		{
			if ((long) errGeneric != MP4F_MP4ATOM_ATOMNOTFOUND)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED,
					1, "stco:0");
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4TrackInfo. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bSaveErrorOfOptionalAtom)
					_errStcoSearch			= errGeneric;

				return errGeneric;
			}
			else
			{
				// below the argument is false because co64 is not mandatory
				if ((errGeneric = pmaStblAtom -> searchAtom (
					"co64:0", false, &pmaAtom)) != errNoError)
				{
					/*
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_SEARCHATOM_FAILED,
						1, "co64:0");
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
					*/

					if (_mtMP4TrackInfo. unLock () != errNoError)
					{
						Error err = PThreadErrors (__FILE__, __LINE__,
							THREADLIB_PMUTEX_UNLOCK_FAILED);
						_ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					if (bSaveErrorOfOptionalAtom)
						_errStcoSearch			= errGeneric;

					return errGeneric;
				}
				else
					_pmaStcoAtom	= (MP4StcoAtom_p) pmaAtom;
			}
		}
		else
			_pmaStcoAtom			= (MP4StcoAtom_p) pmaAtom;
	}

	*pmaStcoAtom			= _pmaStcoAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getStscAtom (
	MP4StscAtom_p *pmaStscAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaStscAtom == (MP4StscAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4StblAtom_p		pmaStblAtom;


		// Stbl is mandatory
		if (getStblAtom (&pmaStblAtom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTBLATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// stsc is mandatory
		if (pmaStblAtom -> searchAtom (
			"stsc:0", true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "stsc:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			_pmaStscAtom			= (MP4StscAtom_p) pmaAtom;
	}

	*pmaStscAtom			= _pmaStscAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getStsdAtom (
	MP4StsdAtom_p *pmaStsdAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaStsdAtom == (MP4StsdAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4StblAtom_p		pmaStblAtom;


		// Stbl is mandatory
		if (getStblAtom (&pmaStblAtom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTBLATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// stsd is mandatory
		if (pmaStblAtom -> searchAtom (
			"stsd:0", true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "stsd:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			_pmaStsdAtom			= (MP4StsdAtom_p) pmaAtom;
	}

	*pmaStsdAtom			= _pmaStsdAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getStssAtom (
	MP4StssAtom_p *pmaStssAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaStssAtom == (MP4StssAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4StblAtom_p		pmaStblAtom;
		Error_t				errGeneric;


		if (_errStssSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errStssSearch;
		}

		// Stbl is mandatory
		if (getStblAtom (&pmaStblAtom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTBLATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// below the argument is false because stss is not mandatory
		if ((errGeneric = pmaStblAtom -> searchAtom (
			"stss:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "stss:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errStssSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaStssAtom			= (MP4StssAtom_p) pmaAtom;
	}

	*pmaStssAtom			= _pmaStssAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getStszAtom (
	MP4StszAtom_p *pmaStszAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaStszAtom == (MP4StszAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4StblAtom_p		pmaStblAtom;


		// Stbl is mandatory
		if (getStblAtom (&pmaStblAtom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTBLATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// stsz is mandatory
		if (pmaStblAtom -> searchAtom (
			"stsz:0", true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "stsz:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			_pmaStszAtom			= (MP4StszAtom_p) pmaAtom;
	}

	*pmaStszAtom			= _pmaStszAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getSttsAtom (
	MP4SttsAtom_p *pmaSttsAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaSttsAtom == (MP4SttsAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4StblAtom_p		pmaStblAtom;


		// Stbl is mandatory
		if (getStblAtom (&pmaStblAtom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTBLATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// stts is mandatory
		if (pmaStblAtom -> searchAtom (
			"stts:0", true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "stts:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			_pmaSttsAtom			= (MP4SttsAtom_p) pmaAtom;
	}

	*pmaSttsAtom			= _pmaSttsAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getTimsAtom (
	MP4TimsAtom_p *pmaTimsAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaTimsAtom == (MP4TimsAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4RtpAtom_p		pmaRtpAtom;
		Error_t				errRtpSearch;


		// below the argument is false because rtp is not mandatory
		//	Since Rtp is not mandatory we will return errRtpSearch
		//	because the calling method can handle the error
		if ((errRtpSearch = getRtpAtom (&pmaRtpAtom,
			bSaveErrorOfOptionalAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETRTPATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errRtpSearch;
		}

		// tims is mandatory
		if (pmaRtpAtom -> searchAtom (
			"tims:0", true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "tims:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			_pmaTimsAtom			= (MP4TimsAtom_p) pmaAtom;
	}

	*pmaTimsAtom			= _pmaTimsAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getTkhdAtom (
	MP4TkhdAtom_p *pmaTkhdAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaTkhdAtom == (MP4TkhdAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;


		// tkhd is mandatory
		if (_pmaTrakAtom -> searchAtom (
			"tkhd:0", true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "tkhd:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			_pmaTkhdAtom			= (MP4TkhdAtom_p) pmaAtom;
	}

	*pmaTkhdAtom			= _pmaTkhdAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getTpylAtom (
	MP4TpylAtom_p *pmaTpylAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaTpylAtom == (MP4TpylAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4HinfAtom_p		pmaHinfAtom;
		Error_t				errGeneric;


		if (_errTpylSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errTpylSearch;
		}

		// below the argument is false because hinf is not mandatory
		if ((errGeneric = getHinfAtom (&pmaHinfAtom,
			bSaveErrorOfOptionalAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHINFATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errGeneric;
		}

		// below the argument is false because tpyl is not mandatory
		if ((errGeneric = pmaHinfAtom -> searchAtom (
			"tpyl:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "udta:0:hinf:0:tpyl:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errTpylSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaTpylAtom			= (MP4TpylAtom_p) pmaAtom;
	}

	*pmaTpylAtom			= _pmaTpylAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getTrakAtom (
	MP4TrakAtom_p *pmaTrakAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pmaTrakAtom			= _pmaTrakAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getTrpyAtom (
	MP4TrpyAtom_p *pmaTrpyAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaTrpyAtom == (MP4TrpyAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4HinfAtom_p		pmaHinfAtom;
		Error_t				errGeneric;


		if (_errTrpySearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errTrpySearch;
		}

		// below the argument is false because hinf is not mandatory
		if ((errGeneric = getHinfAtom (&pmaHinfAtom,
			bSaveErrorOfOptionalAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHINFATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errGeneric;
		}

		// below the argument is false because trpy is not mandatory
		if ((errGeneric = pmaHinfAtom -> searchAtom (
			"trpy:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "trpy:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errTrpySearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaTrpyAtom			= (MP4TrpyAtom_p) pmaAtom;
	}

	*pmaTrpyAtom			= _pmaTrpyAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getTsroAtom (
	MP4TsroAtom_p *pmaTsroAtom,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_pmaTsroAtom == (MP4TsroAtom_p) NULL)
	{
		MP4Atom_p			pmaAtom;
		MP4RtpAtom_p		pmaRtpAtom;
		Error_t				errGeneric;


		if (_errTsroSearch != errNoError)
		{
			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return _errTsroSearch;
		}

		// below the argument is false because rtp is not mandatory
		//	Since Rtp is not mandatory we will return errGeneric
		//	because the calling method can handle the error
		if ((errGeneric = getRtpAtom (&pmaRtpAtom,
			bSaveErrorOfOptionalAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETRTPATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errGeneric;
		}

		// below the argument is false because tsro is not mandatory
		if ((errGeneric = pmaRtpAtom -> searchAtom (
			"tsro:0", false, &pmaAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "tsro:0");
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bSaveErrorOfOptionalAtom)
				_errTsroSearch			= errGeneric;

			return errGeneric;
		}
		else
			_pmaTsroAtom			= (MP4TsroAtom_p) pmaAtom;
	}

	*pmaTsroAtom			= _pmaTsroAtom;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getTrackIdentifier (
	unsigned long *pulTrackIdentifier)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_ulTrackIdentifier == (unsigned long) -1)
	{
		MP4TkhdAtom_p			pmaTkhdAtom;


		// tkhd is mandatory
		if (getTkhdAtom (&pmaTkhdAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETTKHDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaTkhdAtom -> getTrackID (
			&_ulTrackIdentifier) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	*pulTrackIdentifier			= _ulTrackIdentifier;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getMdhdTimeScale (
	unsigned long *pulMdhdTimeScale)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_ulMdhdTimeScale == (unsigned long) -1)
	{
		MP4MdhdAtom_p			pmaMdhdAtom;


		// mdhd is mandatory
		if (getMdhdAtom (&pmaMdhdAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMDHDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaMdhdAtom -> getTimeScale (&_ulMdhdTimeScale) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4MDHDATOM_GETTIMESCALE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_ulMdhdTimeScale == 0)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_NULLTIMESCALE);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			_ulMdhdTimeScale	= (unsigned long) -1;

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	*pulMdhdTimeScale		= _ulMdhdTimeScale;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getTimsTimeScale (
	unsigned long *pulTimsTimeScale,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_ulTimsTimeScale == (unsigned long) -1)
	{
		MP4TimsAtom_p		pmaTimsAtom;
		Error_t				errTimsSearch;


		// tims is child of rtp that is not mandatory
		if ((errTimsSearch = getTimsAtom (&pmaTimsAtom,
			bSaveErrorOfOptionalAtom)) != errNoError)
		{
			/*
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETTIMSATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
			*/

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errTimsSearch;
		}

		if (pmaTimsAtom -> getTimeScale (&_ulTimsTimeScale) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TIMSATOM_GETTIMESCALE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	*pulTimsTimeScale		= _ulTimsTimeScale;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getDuration (
	double *pdDuration)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_dDuration == -1.0)
	{
		MP4MdhdAtom_p			pmaMdhdAtom;
		unsigned long			ulMdhdTimeScale;
		#ifdef WIN32
			__int64						ullDuration;
		#else
			unsigned long long			ullDuration;
		#endif


		// mdhd is mandatory
		if (getMdhdAtom (&pmaMdhdAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMDHDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (getMdhdTimeScale (&ulMdhdTimeScale) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMDHDTIMESCALE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaMdhdAtom -> getDuration (&ullDuration) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4MDHDATOM_GETDURATION_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		_dDuration			= ((double) ullDuration) /
			((double) ulMdhdTimeScale);
	}

	*pdDuration		= _dDuration;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


/*
Error MP4TrackInfo:: getAvgBitRate (
	RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
	unsigned long *pulAvgBitRate)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_ulAvgBitRate == (unsigned long) -1)
	{
		MP4HdlrAtom_p			pmaMediaHdlrAtom;
		MP4MdhdAtom_p			pmaMediaMdhdAtom;
		MP4StszAtom_p			pmaMediaStszAtom;
		MP4Atom:: MP4Codec_t	cCodec;
		char					pHandlerType [MP4F_MAXHANDLERTYPELENGTH];

		// hdlr is mandatory
		if (getHdlrAtom (&pmaMediaHdlrAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETHDLRATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaMediaHdlrAtom -> getHandlerType (pHandlerType,
			MP4F_MAXHANDLERTYPELENGTH) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4HDLRATOM_GETHANDLERTYPE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!strcmp (pHandlerType, MP4_HINT_TRACK_TYPE))
		{
			// L'errore serve perchè sembra che il metodo
			// MP4Utility:: getAvgBitRate funziona solo con
			// media track e non con hint track.
			// Verificare il motivo per cui non può essere
			// applicato alle hint track
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_APPLIEDHINTTRACK);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// mdhd is mandatory
		if (getMdhdAtom (&pmaMediaMdhdAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMDHDATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// stsz is mandatory
		if (getStszAtom (&pmaMediaStszAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSZATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (getCodec (&cCodec) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETCODEC_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (MP4Utility:: getAvgBitRate (
			_pmaTrakAtom,
			pmaMediaHdlrAtom,
			pmaMediaMdhdAtom,
			pmaMediaStszAtom,
			cCodec,
			&_ulAvgBitRate,
			prsrtRTPStreamRealTimeInfo,
			_ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_GETAVGBITRATE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	*pulAvgBitRate		= _ulAvgBitRate;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}
*/


Error MP4TrackInfo:: getSamplesNumber (
	unsigned long *pulSamplesNumber,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_ulSamplesNumber == (unsigned long) -1)
	{
		MP4StszAtom_p			pmaStszAtom;


		// stsz is mandatory
		if (getStszAtom (&pmaStszAtom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSZATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaStszAtom -> getSamplesNumber (&_ulSamplesNumber) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	*pulSamplesNumber			= _ulSamplesNumber;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getMaxSampleSize (
	unsigned long *pulMaxSampleSize,
	Boolean_t bSaveErrorOfOptionalAtom)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_ulMaxSampleSize == (unsigned long) -1)
	{
		MP4StszAtom_p			pmaStszAtom;


		// stsz is mandatory
		if (getStszAtom (&pmaStszAtom, bSaveErrorOfOptionalAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSZATOM_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (MP4Utility:: getMaxSampleSize (pmaStszAtom,
			&_ulMaxSampleSize, &_ulSamplesNumber, _ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_GETMAXSAMPLESIZE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtMP4TrackInfo. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errNoError;
		}
	}

	*pulMaxSampleSize			= _ulMaxSampleSize;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getCodec (
	MP4Atom:: MP4Codec_t *pcCodec)

{


	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pcCodec			= _cCodec;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: getObjectTypeIndication (
	unsigned long *pulObjectTypeIndication)

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (!_bIsObjectTypeIndicationInitialized)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_OBJECTTYPEINDICATIONNOTINITIALIZED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtMP4TrackInfo. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	*pulObjectTypeIndication			= _ulObjectTypeIndication;

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error MP4TrackInfo:: getPointerTrackESConfiguration (
		unsigned char **pucESConfiguration,
		__int64 *pullESConfigurationSize,
		MP4Atom:: Standard_t sStandard)
#else
	Error MP4TrackInfo:: getPointerTrackESConfiguration (
		unsigned char **pucESConfiguration,
		unsigned long long *pullESConfigurationSize,
		MP4Atom:: Standard_t sStandard)
#endif

{

	if (_mtMP4TrackInfo. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (sStandard == MP4Atom:: MP4F_3GPP)
	{
		if (!_bIs3GPPESConfigurrationInitialized)
		{
			if (MP4Utility:: getPointerTrackESConfiguration (
				_pmaTrakAtom,
				&_puc3GPPESConfiguration, &_ull3GPPESConfigurationSize,
				MP4Atom:: MP4F_3GPP, _ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETPOINTERTRACKESCONFIGURATION_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4TrackInfo. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (_ull3GPPESConfigurationSize == 0)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_NOESCONFIGURATIONFOUND);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4TrackInfo. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			_bIs3GPPESConfigurrationInitialized		= true;
		}

		*pucESConfiguration			= _puc3GPPESConfiguration;
		*pullESConfigurationSize	= _ull3GPPESConfigurationSize;
	}
	else
	{
		if (!_bIsISMAESConfigurrationInitialized)
		{
			if (MP4Utility:: getPointerTrackESConfiguration (
				_pmaTrakAtom,
				&_pucISMAESConfiguration, &_ullISMAESConfigurationSize,
				MP4Atom:: MP4F_ISMA, _ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETPOINTERTRACKESCONFIGURATION_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4TrackInfo. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (_ullISMAESConfigurationSize == 0)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_NOESCONFIGURATIONFOUND);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtMP4TrackInfo. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			_bIsISMAESConfigurrationInitialized		= true;
		}

		*pucESConfiguration			= _pucISMAESConfiguration;
		*pullESConfigurationSize	= _ullISMAESConfigurationSize;
	}

	if (_mtMP4TrackInfo. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


/*
Error MP4TrackInfo:: buildAndSetSDPForHintTrack (MP4TrakAtom_p pmaHintTrakAtom,
	MP4TrakAtom_p pmaMediaTrakAtom,
	MP4MdhdAtom_p pmaMediaMdhdAtom,
	MP4HdlrAtom_p pmaMediaHdlrAtom,
	MP4StszAtom_p pmaMediaStszAtom,
	unsigned long ulAvgBitRate,
	MP4Atom:: MP4Codec_t cCodecUsed,
	const char *pPayloadName, unsigned long *pulPayloadNumber,
	unsigned long ulMaxPayloadSize,
	const char *pClockRate, const char *pEncodingParameters,
	std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo,
	MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)

{

	MP4Atom_p					pmaAtom;


	if (pulPayloadNumber == (unsigned long *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmaHintTrakAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (*pulPayloadNumber == MP4F_SETDYNAMICPAYLOAD)
	{
		if (allocRtpPayloadNumber (pulPayloadNumber, pvMP4TracksInfo,
			ptTracer) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_ALLOCRTPPAYLOADNUMBER_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	{

		Buffer_t					bRtpMap;
		unsigned long				ulMaxPacketSize;
		MP4PaytAtom_p				pmaHintPaytAtom;
		MP4RtpAtom_p				pmaHintRtpAtom;
		// MP4HdlrAtom_p				pmaMediaHdlrAtom;
		MP4SdpAtom_p				pmaHintSdpAtom;
		Buffer_t					bSDP;
		Buffer_t					bMediaType;
		char						pHandlerType [MP4F_MAXHANDLERTYPELENGTH];


		if (bRtpMap. init (pPayloadName) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bRtpMap. append ("/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bRtpMap. append (pClockRate) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pEncodingParameters != (const char *) NULL)
		{
			if (bRtpMap. append ("/") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRtpMap. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bRtpMap. append (pEncodingParameters) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRtpMap. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (pmaHintTrakAtom -> searchAtom ("udta:0:hinf:0:payt:0", true,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, "udta:0:hinf:0:payt:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			pmaHintPaytAtom		= (MP4PaytAtom_p) pmaAtom;

		if (pmaHintPaytAtom -> setRtpMap (&bRtpMap) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaHintPaytAtom -> setPayloadNumber (*pulPayloadNumber) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaHintTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:stsd:0:rtp :0",
			true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mdia:0:minf:0:stbl:0:stsd:0:rtp :0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			pmaHintRtpAtom		= (MP4RtpAtom_p) pmaAtom;

		if (ulMaxPayloadSize == 0)
			ulMaxPacketSize					= 1460;
		else
			ulMaxPacketSize					= ulMaxPayloadSize;

		if (pmaHintRtpAtom -> setMaxPacketSize (ulMaxPacketSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaMediaHdlrAtom -> getHandlerType (pHandlerType,
			MP4F_MAXHANDLERTYPELENGTH) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (!strcmp (pHandlerType, MP4_AUDIO_TRACK_TYPE))
		{
			if (bMediaType. init ("audio") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRtpMap. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else if (!strcmp (pHandlerType, MP4_VIDEO_TRACK_TYPE))
		{
			if (bMediaType. init ("video") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bRtpMap. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
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
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4UTILITY_WRONGHANDLERTYPE,
				1, pHandlerType);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bSDP. init ("m=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bSDP. append (bMediaType) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bSDP. append (" 0 RTP/AVP ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bSDP. append (*pulPayloadNumber) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// \n
		if (bSDP. append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// b=AS:...
		if (bSDP. append ("b=AS:") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bSDP. append ((unsigned long) (ulAvgBitRate / 1000)) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// \n
		if (bSDP. append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bSDP. append ("a=rtpmap:") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bSDP. append (*pulPayloadNumber) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bSDP. append (" ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bSDP. append (bRtpMap) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// \n
		if (bSDP. append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (sStandard == MP4Atom:: MP4F_3GPP)
			;
		else				// MP4Atom:: MP4F_ISMA
		{
			unsigned long					ulMediaTrackIdentifier;
			MP4TkhdAtom_p					pmaMediaTkhdAtom;


			if (bSDP. append ("a=mpeg4-esid:") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bMediaType. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtpMap. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaMediaTrakAtom -> searchAtom ("tkhd:0", true,
				&pmaAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, "tkhd:0");
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bMediaType. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtpMap. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			else
				pmaMediaTkhdAtom		= (MP4TkhdAtom_p) pmaAtom;

			if (pmaMediaTkhdAtom -> getTrackID (&ulMediaTrackIdentifier) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bMediaType. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtpMap. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (bSDP. append (ulMediaTrackIdentifier) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bMediaType. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtpMap. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// \n
			if (bSDP. append (MP4F_NEWLINE) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bSDP. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bMediaType. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (bRtpMap. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (pmaHintTrakAtom -> searchAtom ("udta:0:hnti:0:sdp :0", true,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, "udta:0:hnti:0:sdp :0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			pmaHintSdpAtom		= (MP4SdpAtom_p) pmaAtom;

		if (pmaHintSdpAtom -> setSdpText (&bSDP) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bSDP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bSDP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bMediaType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bMediaType. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bRtpMap. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bRtpMap. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
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


Error MP4TrackInfo:: addHintTrack (MP4RootAtom_p pmaRootAtom,
	MP4TrakAtom_p pmaMediaTrakAtom,
	unsigned long ulMediaTrackIdentifier,
	unsigned long ulMdhdTimeScale,
	unsigned long ulTimsTimeScale,
	unsigned long ulTimestampRandomOffset,
	unsigned long ulSequenceNumberRandomOffset,
	MP4TrackInfo_p pmtiMP4HintTrackInfo,
	std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo,
	Tracer_p ptTracer)

{

	MP4TrakAtom_p				pmaHintTrakAtom;
	unsigned long				ulHintTrackIdentifier;

	MP4Atom_p					pmaMoovAtom;
	MP4TkhdAtom_p				pmaHintTkhdAtom;
	MP4HdlrAtom_p				pmaHintHdlrAtom;
	MP4MdhdAtom_p				pmaHintMdhdAtom;
	MP4UrlAtom_p				pmaHintUrlAtom;
	MP4DrefAtom_p				pmaHintDrefAtom;
	MP4MinfAtom_p				pmaHintMinfAtom;
	MP4Atom_p					pmaHintHmhdAtom;
	MP4StsdAtom_p				pmaHintStsdAtom;
	MP4RtpAtom_p				pmaHintRtpAtom;
	MP4TimsAtom_p				pmaHintTimsAtom;
	MP4Atom_p					pmaHintTrefAtom;
	MP4HintAtom_p				pmaHintHintAtom;
	MP4Atom_p					pmaHintUdtaAtom;
	MP4Atom_p					pmaHintHntiAtom;
	MP4SdpAtom_p				pmaHintSdpAtom;
	MP4Atom_p					pmaHintHinfAtom;
	MP4MaxrAtom_p				pmaHintMaxrAtom;
	MP4TsroAtom_p				pmaHintTsroAtom;
	MP4SnroAtom_p				pmaHintSnroAtom;
	MP4Atom_p					pmaAtom;
	std:: vector<MP4TrackInfo_p>:: iterator   itHintTrackInserted;


	if (pmaRootAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (MP4Utility:: getNewTrackIdentifier (pmaRootAtom,
		&ulHintTrackIdentifier, ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4UTILITY_GETNEWTRACKIDENTIFIER_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaRootAtom -> searchAtom ("moov:0", true, &pmaMoovAtom) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_SEARCHATOM_FAILED, 1, "moov:0");
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

	if (pmaMoovAtom -> addChild ("trak", &pmaAtom, -1) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_ADDCHILD_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

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
		pmaHintTrakAtom					= (MP4TrakAtom_p) pmaAtom;

	// need to initialize MP4TrackInfo
	{
		if (pmaHintTrakAtom -> searchAtom ("tkhd:0",
			true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, "tkhd:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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
		else
			pmaHintTkhdAtom		= (MP4TkhdAtom_p) pmaAtom;
	}

	if (pmaHintTkhdAtom -> setTrackID (ulHintTrackIdentifier) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_DELETECHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
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

	// need to initialize MP4TrackInfo
	{
		if (pmaHintTrakAtom -> searchAtom ("mdia:0:hdlr:0",
			true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, "mdia:0:hdlr:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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
		else
			pmaHintHdlrAtom		= (MP4HdlrAtom_p) pmaAtom;
	}

	// set hint handler type
	{
		Buffer_t					bHandlerType;


		if (bHandlerType. init (MP4_HINT_TRACK_TYPE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

		if (pmaHintHdlrAtom -> setHandlerType (&bHandlerType) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bHandlerType. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

		if (bHandlerType. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

	if (pmtiMP4HintTrackInfo -> init (pmaHintTrakAtom, false, ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_DELETECHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
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

	// set hint name
	{
		Buffer_t					bName;


		if (bName. init ("Hint handler generated by CatraSoftware") !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_INIT_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

		if (pmaHintHdlrAtom -> setName (&bName) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (bName. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

		if (bName. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

	// set time scale
	{
		if (pmtiMP4HintTrackInfo -> getMdhdAtom (&pmaHintMdhdAtom) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMDHDATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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
		/*
		if ((pmtiMP4HintTrackInfo -> _pmaTrakAtom) -> searchAtom ("mdia:0:mdhd:0",
			true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, "mdia:0:mdhd:0");
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
		*/

		if (pmaHintMdhdAtom -> setTimeScale (ulMdhdTimeScale) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

	// pTrack = new MP4RtpHintTrack(this, pTrakAtom);

	// url management
	{
		// char				pAtomPath [MP4F_MAXPATHNAMELENGTH];
		unsigned long		ulFlags;


		if (pmtiMP4HintTrackInfo -> getDrefAtom (&pmaHintDrefAtom) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETDREFATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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
		/*
		strcpy (pAtomPath, "mdia:0:minf:0:dinf:0:dref:0");

		if ((pmtiMP4HintTrackInfo -> _pmaTrakAtom) -> searchAtom (pAtomPath,
			true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, pAtomPath);
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
			pmaHintDrefAtom		= (MP4DrefAtom_p) pmaAtom;
		*/

		if (pmaHintDrefAtom -> incrementEntryCount (1) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

		if (pmaHintDrefAtom -> addChild ("url ",
			&pmaAtom, -1) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_ADDCHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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
		else
			pmaHintUrlAtom		= (MP4UrlAtom_p) pmaAtom;

		if (pmaHintUrlAtom -> getFlags (&ulFlags) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

		ulFlags					|= 1;

		if (pmaHintUrlAtom -> setFlags (ulFlags) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

	// addChild hmhd
	{
		// char				pAtomPath [MP4F_MAXPATHNAMELENGTH];


		if (pmtiMP4HintTrackInfo -> getMinfAtom (&pmaHintMinfAtom) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETMINFATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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
		/*
		strcpy (pAtomPath, "mdia:0:minf:0");

		if ((pmtiMP4HintTrackInfo -> _pmaTrakAtom) -> searchAtom (pAtomPath, true,
			&pmaHintMinfAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, pAtomPath);
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
		*/

		if (pmaHintMinfAtom -> addChild ("hmhd", &pmaHintHmhdAtom, -1) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_ADDCHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

	// addChild rtp
	{
		// char				pAtomPath [MP4F_MAXPATHNAMELENGTH];


		if (pmtiMP4HintTrackInfo -> getStsdAtom (&pmaHintStsdAtom,
			false) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETSTSDATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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
		/*
		strcpy (pAtomPath, "mdia:0:minf:0:stbl:0:stsd:0");

		if ((pmtiMP4HintTrackInfo -> _pmaTrakAtom) -> searchAtom (pAtomPath, true,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, pAtomPath);
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
			pmaHintStsdAtom		= (MP4StsdAtom_p) pmaAtom;
		*/

		if (pmaHintStsdAtom -> incrementEntryCount (1) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

		if (pmaHintStsdAtom -> addChild ("rtp ",
			&pmaAtom, -1) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_ADDCHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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
		else
			pmaHintRtpAtom		= (MP4RtpAtom_p) pmaAtom;
	}

	// setPropertyValue tsro.Offset
	{
		if (pmaHintRtpAtom -> addChild ("tsro",
			&pmaAtom, -1) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_ADDCHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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
		else
			pmaHintTsroAtom		= (MP4TsroAtom_p) pmaAtom;

		if (pmaHintTsroAtom -> setOffset (ulTimestampRandomOffset) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

	// setPropertyValue snro.Offset
	{
		if (pmaHintRtpAtom -> addChild ("snro",
			&pmaAtom, -1) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_ADDCHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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
		else
			pmaHintSnroAtom		= (MP4SnroAtom_p) pmaAtom;

		if (pmaHintSnroAtom -> setOffset (ulSequenceNumberRandomOffset) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

	// setPropertyValue tims.TimeScale
	{
		// char				pAtomPath [MP4F_MAXPATHNAMELENGTH];


		if (pmtiMP4HintTrackInfo -> getTimsAtom (&pmaHintTimsAtom,
			false) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_GETTIMSATOM_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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
		/*
		strcpy (pAtomPath, "tims:0");

		if (pmaHintRtpAtom -> searchAtom (pAtomPath, true,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, pAtomPath);
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
			pmaHintTimsAtom		= (MP4TimsAtom_p) pmaAtom;
		*/

		if (pmaHintTimsAtom -> setTimeScale (ulTimsTimeScale) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

	if (pmaHintTrakAtom -> addChild (
		"tref", &pmaHintTrefAtom, -1) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_ADDCHILD_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmtiMP4HintTrackInfo -> finish () !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_DELETECHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
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

	// addChild hint
	{
		unsigned long						ulEntriesNumber;


		if (pmaHintTrefAtom -> addChild ("hint",
			&pmaAtom, -1) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_ADDCHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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
		else
			pmaHintHintAtom		= (MP4HintAtom_p) pmaAtom;

		if (pmaHintHintAtom -> getEntriesNumber (&ulEntriesNumber) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

		if (pmaHintHintAtom -> incrementEntriesNumber (1) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

		if (pmaHintHintAtom ->  setTracksReferencesTable (
			ulEntriesNumber, ulMediaTrackIdentifier) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (pmtiMP4HintTrackInfo -> finish () !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_FINISH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_DELETECHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
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

	if (pmaHintTrakAtom -> addChild (
		"udta", &pmaHintUdtaAtom, -1) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_ADDCHILD_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmtiMP4HintTrackInfo -> finish () !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_DELETECHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
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

	if (pmaHintUdtaAtom -> addChild ("hnti", &pmaHintHntiAtom, -1) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_ADDCHILD_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmtiMP4HintTrackInfo -> finish () !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_DELETECHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
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

	if (pmaHintHntiAtom -> addChild ("sdp ", &pmaAtom,
		-1) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_ADDCHILD_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmtiMP4HintTrackInfo -> finish () !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_DELETECHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
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
	else
		pmaHintSdpAtom		= (MP4SdpAtom_p) pmaAtom;

	if (pmaHintUdtaAtom -> addChild ("hinf", &pmaHintHinfAtom, -1) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_ADDCHILD_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmtiMP4HintTrackInfo -> finish () !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_DELETECHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
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

	if (pmtiMP4HintTrackInfo -> getMaxrAtom (&pmaHintMaxrAtom,
		false) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETMAXRATOM_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmtiMP4HintTrackInfo -> finish () !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_DELETECHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
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
	/*
	if ((pmtiMP4HintTrackInfo -> _pmaTrakAtom) -> searchAtom ("udta:0:hinf:0:maxr:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, "udta:0:hinf:0:maxr:0");
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
		pmaHintMaxrAtom		= (MP4MaxrAtom_p) pmaAtom;
	*/

	// 1 second
	if (pmaHintMaxrAtom -> setGranularity (1000) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmtiMP4HintTrackInfo -> finish () !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_DELETECHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
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

	itHintTrackInserted = pvMP4TracksInfo -> insert (
		pvMP4TracksInfo -> end (), pmtiMP4HintTrackInfo);

	if (pmaRootAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		pvMP4TracksInfo -> erase (itHintTrackInserted);

		if (pmtiMP4HintTrackInfo -> finish () !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4TRACKINFO_FINISH_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_DELETECHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}


	return errNoError;
}


Error MP4TrackInfo:: deleteHintTrack (MP4RootAtom_p pmaRootAtom,
	MP4TrackInfo_p pmtiMP4HintTrackInfo,
	std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo,
	Tracer_p ptTracer)

{

	MP4TrakAtom_p				pmaHintTrakAtom;
	MP4Atom_p					pmaMoovAtom;


	if (pmaRootAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

    if (pmtiMP4HintTrackInfo -> getTrakAtom (&pmaHintTrakAtom) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_GETTRAKATOM_FAILED);
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

	{
		std:: vector<MP4TrackInfo_p>:: iterator		itHintTrackToBeDeleted;
		MP4TrackInfo_p					pmtiMP4TrackInfo;


		for (itHintTrackToBeDeleted = pvMP4TracksInfo -> begin ();
			itHintTrackToBeDeleted != pvMP4TracksInfo -> end ();
			++itHintTrackToBeDeleted)
		{
			pmtiMP4TrackInfo				= *itHintTrackToBeDeleted;

			if (pmtiMP4TrackInfo == pmtiMP4HintTrackInfo)
				break;
		}

		if (itHintTrackToBeDeleted == pvMP4TracksInfo -> end ())
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_HINTTRACKNOTFOUND);
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

		pvMP4TracksInfo -> erase (itHintTrackToBeDeleted);
	}

	if (pmtiMP4HintTrackInfo -> finish () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4TRACKINFO_FINISH_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaRootAtom -> searchAtom ("moov:0", true, &pmaMoovAtom) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_SEARCHATOM_FAILED, 1, "moov:0");
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

	if (pmaMoovAtom -> deleteChild (pmaHintTrakAtom) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_DELETECHILD_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

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


/*
Error MP4TrackInfo:: allocRtpPayloadNumber (unsigned long *pulPayloadNumber,
	std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo, Tracer_p ptTracer)

{

	unsigned char					ucPayloadNumber;
	std:: vector<unsigned long>		vUsedPayloads;
	unsigned long					ulUsedPayloadIndex;
	std:: vector<MP4TrackInfo_p>:: const_iterator	it;
	MP4TrackInfo_p					pmtiMP4TrackInfo;
	MP4PaytAtom_p					pmaPaytAtom;
	MP4Atom_p						pmaAtom;



	if ((*pvMP4TracksInfo). size () == 0)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if ((*(pvMP4TracksInfo -> begin ())) -> _pmaTrakAtom ->
		lockMP4File () != errNoError)
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

		if ((pmtiMP4TrackInfo -> _pmaTrakAtom) -> searchAtom (
			"udta:0:hinf:0:payt:0", false, &pmaAtom) !=
			errNoError)
			continue;
		else
			pmaPaytAtom		= (MP4PaytAtom_p) pmaAtom;

		if (pmaPaytAtom -> getPayloadNumber (pulPayloadNumber) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if ((*(pvMP4TracksInfo -> begin ())) -> _pmaTrakAtom ->
				unLockMP4File () != errNoError)
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

		if ((*(pvMP4TracksInfo -> begin ())) -> _pmaTrakAtom ->
			unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	*pulPayloadNumber				= ucPayloadNumber;

	if ((*(pvMP4TracksInfo -> begin ())) -> _pmaTrakAtom ->
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
*/


