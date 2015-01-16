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


#ifndef MP4TrackInfo_h
	#define MP4TrackInfo_h

	#include "MP4CttsAtom.h"
	#include "MP4D263Atom.h"
	#include "MP4DecoderConfigDescr.h"
	#include "MP4DmaxAtom.h"
	#include "MP4DmedAtom.h"
	#include "MP4DimmAtom.h"
	#include "MP4DinfAtom.h"
	#include "MP4DrefAtom.h"
	#include "MP4ElstAtom.h"
	#include "MP4HdlrAtom.h"
	#include "MP4HinfAtom.h"
	#include "MP4HintAtom.h"
	#include "MP4HmhdAtom.h"
	#include "MP4MaxrAtom.h"
	#include "MP4MdhdAtom.h"
	#include "MP4MinfAtom.h"
	#include "MP4Mp4vAtom.h"
	#include "MP4NumpAtom.h"
	#include "MP4PaytAtom.h"
	#include "MP4PmaxAtom.h"
	#include "MP4RootAtom.h"
	#include "MP4RtpAtom.h"
	#include "MP4S263Atom.h"
	#include "MP4SamrAtom.h"
	#include "MP4SawbAtom.h"
	#include "MP4SdpAtom.h"
	#include "MP4SnroAtom.h"
	#include "MP4StblAtom.h"
	#include "MP4StcoAtom.h"
	#include "MP4StscAtom.h"
	#include "MP4StsdAtom.h"
	#include "MP4StssAtom.h"
	#include "MP4StszAtom.h"
	#include "MP4SttsAtom.h"
	#include "MP4TimsAtom.h"
	#include "MP4TkhdAtom.h"
	#include "MP4TpylAtom.h"
	#include "MP4TrakAtom.h"
	#include "MP4TrpyAtom.h"
	#include "MP4TsroAtom.h"
	#include "MP4FileErrors.h"
	#include "PMutex.h"
	#include "Tracer.h"


	#define MP4F_MAXTRACKNAMELENGTH			512 + 1


	typedef class MP4TrackInfo
	{
		private:
			Tracer_p			_ptTracer;
			PMutex_t			_mtMP4TrackInfo;
			MP4CttsAtom_p		_pmaCttsAtom;
			MP4D263Atom_p		_pmaD263Atom;
			MP4DecoderConfigDescr_p		_pmaAACAudioDecoderConfigDescr;
			MP4DecoderConfigDescr_p		_pmaMPEG4VideoDecoderConfigDescr;
			MP4DimmAtom_p		_pmaDimmAtom;
			MP4DinfAtom_p		_pmaDinfAtom;
			MP4DmaxAtom_p		_pmaDmaxAtom;
			MP4DmedAtom_p		_pmaDmedAtom;
			MP4DrefAtom_p		_pmaDrefAtom;
			MP4ElstAtom_p		_pmaElstAtom;
			MP4HdlrAtom_p		_pmaHdlrAtom;
			MP4HinfAtom_p		_pmaHinfAtom;
			MP4HintAtom_p		_pmaHintAtom;
			MP4HmhdAtom_p		_pmaHmhdAtom;
			MP4MaxrAtom_p		_pmaMaxrAtom;
			MP4MdhdAtom_p		_pmaMdhdAtom;
			MP4MinfAtom_p		_pmaMinfAtom;
			MP4Mp4vAtom_p		_pmaMp4vAtom;
			MP4NumpAtom_p		_pmaNumpAtom;
			MP4PaytAtom_p		_pmaPaytAtom;
			MP4PmaxAtom_p		_pmaPmaxAtom;
			MP4RtpAtom_p		_pmaRtpAtom;
			MP4S263Atom_p		_pmaS263Atom;
			MP4SamrAtom_p		_pmaSamrAtom;
			MP4SawbAtom_p		_pmaSawbAtom;
			MP4SdpAtom_p		_pmaSdpAtom;
			MP4SnroAtom_p		_pmaSnroAtom;
			MP4StblAtom_p		_pmaStblAtom;
			MP4StcoAtom_p		_pmaStcoAtom;
			MP4StscAtom_p		_pmaStscAtom;
			MP4StsdAtom_p		_pmaStsdAtom;
			MP4StssAtom_p		_pmaStssAtom;
			MP4StszAtom_p		_pmaStszAtom;
			MP4SttsAtom_p		_pmaSttsAtom;
			MP4TimsAtom_p		_pmaTimsAtom;
			MP4TkhdAtom_p		_pmaTkhdAtom;
			MP4TpylAtom_p		_pmaTpylAtom;
			MP4TrakAtom_p		_pmaTrakAtom;
			MP4TrpyAtom_p		_pmaTrpyAtom;
			MP4TsroAtom_p		_pmaTsroAtom;

			// For performance reason if we receive an error searching an
			//	not manadatory atom, we will not search it again but return
			//	directly the error
			// We are assuming that the file is not changed
			// This behaviour is also manageable through the
			// bSaveErrorOfOptionalAtom parameter
			Error_t				_errCttsSearch;
			Error_t				_errAACAudioDecoderConfigDescrSearch;
			Error_t				_errMPEG4VideoDecoderConfigDescrSearch;
			Error_t				_errDimmSearch;
			Error_t				_errDmaxSearch;
			Error_t				_errDmedSearch;
			Error_t				_errHinfSearch;
			Error_t				_errHintSearch;
			Error_t				_errHmhdSearch;
			Error_t				_errMaxrSearch;
			Error_t				_errMp4vSearch;
			Error_t				_errNumpSearch;
			Error_t				_errPaytSearch;
			Error_t				_errPmaxSearch;
			Error_t				_errRtpSearch;
			Error_t				_errS263Search;
			Error_t				_errSamrSearch;
			Error_t				_errSawbSearch;
			Error_t				_errSdpSearch;
			Error_t				_errSnroSearch;
			Error_t				_errStblSearch;
			Error_t				_errStcoSearch;
			Error_t				_errStssSearch;
			Error_t				_errTpylSearch;
			Error_t				_errTrpySearch;
			Error_t				_errTsroSearch;

			unsigned long		_ulTrackIdentifier;
			char				_pHandlerType [MP4F_MAXHANDLERTYPELENGTH];
			char				_pTrackName [MP4F_MAXTRACKNAMELENGTH];

			// significant only for AAC & MPEG4 codecs,
			Boolean_t			_bIsObjectTypeIndicationInitialized;
			unsigned long		_ulObjectTypeIndication;

			MP4Atom:: MP4Codec_t	_cCodec;


			unsigned long		_ulMdhdTimeScale;
			unsigned long		_ulTimsTimeScale;
			double				_dDuration;
			unsigned long		_ulSamplesNumber;
			unsigned long		_ulMaxSampleSize;
			unsigned long		_ulAvgBitRate;

			Boolean_t			_bIs3GPPESConfigurrationInitialized;
			unsigned char		*_puc3GPPESConfiguration;
			#ifdef WIN32
				__int64			_ull3GPPESConfigurationSize;
			#else
				unsigned long long	_ull3GPPESConfigurationSize;
			#endif
			Boolean_t			_bIsISMAESConfigurrationInitialized;
			unsigned char		*_pucISMAESConfiguration;
			#ifdef WIN32
				__int64			_ullISMAESConfigurationSize;
			#else
				unsigned long long	_ullISMAESConfigurationSize;
			#endif

		public:
			MP4TrackInfo (void);

			~MP4TrackInfo (void);

			Error init (
				MP4TrakAtom_p pmaTrakAtom,
				Boolean_t bSaveErrorOfOptionalAtom,
				Tracer_p ptTracer);

			Error finish (void);

			Error getTrackName (
				char *pTrackName);

			Error getHandlerType (
				char *pHandlerType);

			Error getCttsAtom (
				MP4CttsAtom_p *pmaCttsAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getD263Atom (
				MP4D263Atom_p *pmaD263Atom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getAACAudioDecoderConfigDescr (
				MP4DecoderConfigDescr_p *pmaDecoderConfigDescr,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getMPEG4VideoDecoderConfigDescr (
				MP4DecoderConfigDescr_p *pmaDecoderConfigDescr,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getDimmAtom (
				MP4DimmAtom_p *pmaDimmAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getDinfAtom (
				MP4DinfAtom_p *pmaDinfAtom);

			Error getDmaxAtom (
				MP4DmaxAtom_p *pmaDmaxAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getDmedAtom (
				MP4DmedAtom_p *pmaDmedAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getDrefAtom (
				MP4DrefAtom_p *pmaDrefAtom);

			Error getElstAtom (
				MP4ElstAtom_p *pmaElstAtom);

			Error getHdlrAtom (
				MP4HdlrAtom_p *pmaHdlrAtom);

			Error getHinfAtom (
				MP4HinfAtom_p *pmaHinfAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getHintAtom (
				MP4HintAtom_p *pmaHintAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getHmhdAtom (
				MP4HmhdAtom_p *pmaHmhdAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getMaxrAtom (
				MP4MaxrAtom_p *pmaMaxrAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getMdhdAtom (
				MP4MdhdAtom_p *pmaMdhdAtom);

			Error getMinfAtom (
				MP4MinfAtom_p *pmaMinfAtom);

			Error getMp4vAtom (
				MP4Mp4vAtom_p *pmaMp4vAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getNumpAtom (
				MP4NumpAtom_p *pmaNumpAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getPaytAtom (
				MP4PaytAtom_p *pmaPaytAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getPmaxAtom (
				MP4PmaxAtom_p *pmaPmaxAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getRtpAtom (
				MP4RtpAtom_p *pmaRtpAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getS263Atom (
				MP4S263Atom_p *pmaS263Atom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getSamrAtom (
				MP4SamrAtom_p *pmaSamrAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getSawbAtom (
				MP4SawbAtom_p *pmaSawbAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getSdpAtom (
				MP4SdpAtom_p *pmaSdpAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getSnroAtom (
				MP4SnroAtom_p *pmaSnroAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getStblAtom (
				MP4StblAtom_p *pmaStblAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getStcoAtom (
				MP4StcoAtom_p *pmaStcoAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getStscAtom (
				MP4StscAtom_p *pmaStscAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getStsdAtom (
				MP4StsdAtom_p *pmaStsdAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getStssAtom (
				MP4StssAtom_p *pmaStssAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getStszAtom (
				MP4StszAtom_p *pmaStszAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getSttsAtom (
				MP4SttsAtom_p *pmaSttsAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getTimsAtom (
				MP4TimsAtom_p *pmaTimsAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getTkhdAtom (
				MP4TkhdAtom_p *pmaTkhdAtom);

			Error getTpylAtom (
				MP4TpylAtom_p *pmaTpylAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getTrakAtom (
				MP4TrakAtom_p *pmaTrakAtom);

			Error getTrpyAtom (
				MP4TrpyAtom_p *pmaTrpyAtom,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getTsroAtom (
				MP4TsroAtom_p *pmaTsroAtom,
				Boolean_t bSaveErrorOfOptionalAtom);


			Error getTrackIdentifier (
				unsigned long *pulTrackIdentifier);

			Error getMdhdTimeScale (
				unsigned long *pulMdhdTimeScale);

			Error getTimsTimeScale (
				unsigned long *pulTimsTimeScale,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getDuration (
				double *pdDuration);

			Error getSamplesNumber (
				unsigned long *pulSamplesNumber,
				Boolean_t bSaveErrorOfOptionalAtom);

			Error getMaxSampleSize (
				unsigned long *pulMaxSampleSize,
				Boolean_t bSaveErrorOfOptionalAtom);

			/*
			Error getAvgBitRate (
				RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
				unsigned long *pulAvgBitRate);
			*/

			Error getCodec (
				MP4Atom:: MP4Codec_t *pcCodec);

			Error getObjectTypeIndication (
				unsigned long *pulObjectTypeIndication);

			#ifdef WIN32
				Error getPointerTrackESConfiguration (
					unsigned char **pucESConfiguration,
					__int64 *pullESConfigurationSize,
					MP4Atom:: Standard_t sStandard);
			#else
				Error getPointerTrackESConfiguration (
					unsigned char **pucESConfiguration,
					unsigned long long *pullESConfigurationSize,
					MP4Atom:: Standard_t sStandard);
			#endif

			static Error addHintTrack (MP4RootAtom_p pmaRootAtom,
				MP4TrakAtom_p pmaMediaTrakAtom,
				unsigned long ulMediaTrackIdentifier,
				unsigned long ulMdhdTimeScale,
				unsigned long ulTimsTimeScale,
				unsigned long ulTimestampRandomOffset,
				unsigned long ulSequenceNumberRandomOffset,
				MP4TrackInfo *pmtiMP4HintTrackInfo,
				std:: vector<MP4TrackInfo *> *pvMP4TracksInfo,
				Tracer_p ptTracer);

			static Error deleteHintTrack (MP4RootAtom_p pmaRootAtom,
				MP4TrackInfo *pmtiMP4HintTrackInfo,
				std:: vector<MP4TrackInfo *> *pvMP4TracksInfo,
				Tracer_p ptTracer);

	} MP4TrackInfo_t, *MP4TrackInfo_p;

#endif
