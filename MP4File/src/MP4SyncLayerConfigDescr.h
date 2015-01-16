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


#ifndef MP4SyncLayerConfigDescr_h
	#define MP4SyncLayerConfigDescr_h

	#include "MP4Descr.h"
	#include "MP4UInteger8BitsProperty.h"
	#include "MP4UInteger16BitsProperty.h"
	#include "MP4UInteger24BitsProperty.h"
	#include "MP4UInteger32BitsProperty.h"
	#include "MP4UInteger64BitsProperty.h"
	#include "MP4FileErrors.h"



	typedef class MP4SyncLayerConfigDescr: public MP4Descr {

		private:
			MP4UInteger8BitsProperty_t		_mui8pPredefined;
			MP4UInteger8BitsProperty_t		_mui8pSyncLayerConfigDescrFlags1;
			MP4UInteger32BitsProperty_t		_mui32pTimeStampResolution;
			MP4UInteger32BitsProperty_t		_mui32pOCRResolution;
			MP4UInteger8BitsProperty_t		_mui8pTimeStampLength;
			MP4UInteger8BitsProperty_t		_mui8pOCRLength;
			MP4UInteger8BitsProperty_t		_mui8pAULength;
			MP4UInteger8BitsProperty_t		_mui8pInstantBitrateLength;
			MP4UInteger16BitsProperty_t		_mui16pSyncLayerConfigDescrFlags2;
			MP4UInteger32BitsProperty_t		_mui32pTimeScale;
			MP4UInteger16BitsProperty_t		_mui16pAccessUnitDuration;
			MP4UInteger16BitsProperty_t		_mui16pCompositionUnitDuration;
			unsigned long					_ulTimeStampLength;
			MP4UInteger8BitsProperty_t		_mui8pStartDecodingTimeStamp;
			MP4UInteger16BitsProperty_t		_mui16pStartDecodingTimeStamp;
			MP4UInteger24BitsProperty_t		_mui24pStartDecodingTimeStamp;
			MP4UInteger32BitsProperty_t		_mui32pStartDecodingTimeStamp;
			MP4UInteger64BitsProperty_t		_mui64pStartDecodingTimeStamp;
			MP4UInteger8BitsProperty_t		_mui8pStartCompositionTimeStamp;
			MP4UInteger16BitsProperty_t		_mui16pStartCompositionTimeStamp;
			MP4UInteger24BitsProperty_t		_mui24pStartCompositionTimeStamp;
			MP4UInteger32BitsProperty_t		_mui32pStartCompositionTimeStamp;
			MP4UInteger64BitsProperty_t		_mui64pStartCompositionTimeStamp;

		protected:
			MP4SyncLayerConfigDescr (const MP4SyncLayerConfigDescr &);

			MP4SyncLayerConfigDescr &operator = (
				const MP4SyncLayerConfigDescr &);

			virtual Error prepareChildrensAtomsInfo (void);

			virtual Error createProperties (
				unsigned long *pulPropertiesSize);

			virtual Error readProperties (
				unsigned long *pulPropertiesSize);

			virtual Error writeProperties (int iFileDescriptor);

			virtual Error getPropertiesSize (
				unsigned long *pulPropertiesSize);

			virtual Error appendPropertiesDump (Buffer_p pbBuffer);

			virtual Error printPropertiesOnStdOutput (void);

		public:
			MP4SyncLayerConfigDescr ();

			virtual ~MP4SyncLayerConfigDescr ();

			#ifdef WIN32
				virtual Error init (MP4Atom *pmaParentAtom,
					FileReader_p pfFile,
					__int64 ullAtomStartOffset, __int64 ullAtomSize,
					unsigned long ulHeaderSize,
					unsigned char ucNumBytesForDescriptorSize, unsigned long ulTag,
					PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
					Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer);
			#else
				virtual Error init (MP4Atom *pmaParentAtom,
					FileReader_p pfFile,
					unsigned long long ullAtomStartOffset, unsigned long long ullAtomSize,
					unsigned long ulHeaderSize,
					unsigned char ucNumBytesForDescriptorSize, unsigned long ulTag,
					PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
					Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer);
			#endif

			virtual Error getType (char *pType);

			Error getPredefined (unsigned char *pucPredefined);

			Error setPredefined (unsigned char ucPredefined);

			Error getDurationFlag (Boolean_p pbDurationFlag);

			Error setDurationFlag (Boolean_t bDurationFlag);

			Error getUseTimeStampsFlag (Boolean_p pbUseTimeStampsFlag);

			Error setUseTimeStampsFlag (Boolean_t bUseTimeStampsFlag);

			Error getReserved (unsigned char *pucReserved);

			Error setReserved (unsigned char ucReserved);

			Error getTimeStampResolution (
				unsigned long *pulTimeStampResolution);

			Error setTimeStampResolution (unsigned long ulTimeStampResolution);

			Error getTimeStampLength (unsigned char *pucTimeStampLength);

			Error setTimeStampLength (unsigned char ucTimeStampLength);

	} MP4SyncLayerConfigDescr_t, *MP4SyncLayerConfigDescr_p;

#endif

