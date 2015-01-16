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


#ifndef MP4Mp4vAtom_h
	#define MP4Mp4vAtom_h

	#include "MP4Atom.h"
	#include "MP4FileErrors.h"


	typedef class MP4Mp4vAtom: public MP4Atom {

		private:
			MP4BytesProperty_t					_mbpReserved1;
			MP4UInteger16BitsProperty_t			_mui16pDataReferenceIndex;
			MP4BytesProperty_t					_mbpReserved2;
			MP4UInteger16BitsProperty_t			_mui16pWidth;
			MP4UInteger16BitsProperty_t			_mui16pHeight;
			MP4BytesProperty_t					_mbpReserved3;
			MP4StringFixedProperty_t			_mstrfpCompressorName;
			MP4BytesProperty_t					_mbpReserved4;

		protected:
			MP4Mp4vAtom (const MP4Mp4vAtom &);

			MP4Mp4vAtom &operator = (const MP4Mp4vAtom &);

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
			MP4Mp4vAtom ();

			virtual ~MP4Mp4vAtom ();

			#ifdef WIN32
				virtual Error init (MP4Atom *pmaParentAtom,
					FileReader_p pfFile,
					__int64 ullAtomStartOffset, __int64 ullAtomSize,
					unsigned long ulHeaderSize,
					PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
					Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer);
			#else
				virtual Error init (MP4Atom *pmaParentAtom,
					FileReader_p pfFile,
					unsigned long long ullAtomStartOffset, unsigned long long ullAtomSize,
					unsigned long ulHeaderSize,
					PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
					Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer);
			#endif

			virtual Error getType (char *pType);

			Error getWidth (unsigned long *pulWidth);

			Error setWidth (unsigned long ulWidth);

			Error getHeight (unsigned long *pulHeight);

			Error setHeight (unsigned long ulHeight);

	} MP4Mp4vAtom_t, *MP4Mp4vAtom_p;

#endif

