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


#ifndef MP4ContentClassificationDescr_h
	#define MP4ContentClassificationDescr_h

	#include "MP4Descr.h"
	#include "MP4FileErrors.h"



	typedef class MP4ContentClassificationDescr: public MP4Descr {

		private:
			MP4UInteger32BitsProperty_t			_mui32pClassificationEntity;
			MP4UInteger16BitsProperty_t			_mui16pClassificationTable;
			MP4BytesProperty_t					_mbpContentClassificationData;

		protected:
			MP4ContentClassificationDescr (
				const MP4ContentClassificationDescr &);

			MP4ContentClassificationDescr &operator = (
				const MP4ContentClassificationDescr &);

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
			MP4ContentClassificationDescr ();

			/**
				Distruttore.
			*/
			virtual ~MP4ContentClassificationDescr ();

			#ifdef WIN32
				virtual Error init (MP4Atom *pmaParentAtom,
					FileReader_p pfFile,
					__int64 ullAtomStartOffset, __int64 ullAtomSize,
					unsigned long ulHeaderSize,
					unsigned char ucNumBytesForDescriptorSize,
					unsigned long ulTag,
					PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
					Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer);
			#else
				virtual Error init (MP4Atom *pmaParentAtom,
					FileReader_p pfFile,
					unsigned long long ullAtomStartOffset,
					unsigned long long ullAtomSize,
					unsigned long ulHeaderSize,
					unsigned char ucNumBytesForDescriptorSize,
					unsigned long ulTag,
					PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
					Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer);
			#endif

			virtual Error getType (char *pType);

	} MP4ContentClassificationDescr_t, *MP4ContentClassificationDescr_p;

#endif

