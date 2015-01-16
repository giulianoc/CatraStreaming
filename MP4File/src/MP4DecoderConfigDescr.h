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


#ifndef MP4DecoderConfigDescr_h
	#define MP4DecoderConfigDescr_h

	#include "MP4Descr.h"
	#include "MP4UInteger8BitsProperty.h"
	#include "MP4UInteger24BitsProperty.h"
	#include "MP4UInteger32BitsProperty.h"
	#include "MP4FileErrors.h"


	typedef class MP4DecoderConfigDescr: public MP4Descr {

		private:
			MP4UInteger8BitsProperty_t			_mui8pObjectTypeIndication;
			MP4UInteger8BitsProperty_t			_mui8pDecoderConfigDescrFlags;
			MP4UInteger24BitsProperty_t			_mui24pBufferSizeDB;
			MP4UInteger32BitsProperty_t			_mui32pMaxBitRate;
			MP4UInteger32BitsProperty_t			_mui32pAvgBitRate;

		protected:
			MP4DecoderConfigDescr (const MP4DecoderConfigDescr &);

			MP4DecoderConfigDescr &operator = (const MP4DecoderConfigDescr &);

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
			/**
				Costruttore.
			*/
			MP4DecoderConfigDescr ();

			/**
				Distruttore.
			*/
			virtual ~MP4DecoderConfigDescr ();

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

			Error getObjectTypeIndication (
				unsigned long *pulObjectTypeIndication);

			Error setObjectTypeIndication (
				unsigned long ulObjectTypeIndication);

			Error getAvgBitRate (unsigned long *pulAvgBitRate);

			Error setAvgBitRate (unsigned long ulAvgBitRate);

	} MP4DecoderConfigDescr_t, *MP4DecoderConfigDescr_p;

#endif
