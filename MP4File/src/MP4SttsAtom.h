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


#ifndef MP4SttsAtom_h
	#define MP4SttsAtom_h

	#include "MP4Atom.h"
	#include "MP4FileErrors.h"



	typedef class MP4SttsAtom: public MP4Atom {

		private:
			MP4UInteger8BitsProperty_t				_mui8pVersion;
			MP4UInteger24BitsProperty_t				_mui24pFlags;
			MP4UInteger32BitsProperty_t				_mui32pEntriesNumber;
			MP4UInteger32BitsProperty_t				_mui32pTimeToSampleTable;

		protected:
			MP4SttsAtom (const MP4SttsAtom &);

			MP4SttsAtom &operator = (const MP4SttsAtom &);

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
			MP4SttsAtom ();

			virtual ~MP4SttsAtom ();

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

			Error getEntriesNumber (unsigned long *pulEntriesNumber);

			Error setEntriesNumber (unsigned long ulEntriesNumber);

			Error incrementEntriesNumber (long lIncrement);

			Error getTimeToSampleTable (unsigned long ulIndex,
				unsigned long *pulTimeToSampleTable);

			Error setTimeToSampleTable (unsigned long ulIndex,
				unsigned long ulTimeToSampleTable);

			Error incrementTimeToSampleTable (unsigned long ulIndex,
				long lIncrement);

	} MP4SttsAtom_t, *MP4SttsAtom_p;

#endif
