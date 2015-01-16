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


#ifndef MP4D263Atom_h
	#define MP4D263Atom_h

	#include "MP4Atom.h"
	#include "MP4FileErrors.h"


	typedef class MP4D263Atom: public MP4Atom {

		private:
			MP4UInteger32BitsProperty_t			_mui32pVendor;
			MP4UInteger8BitsProperty_t			_mui8pDecoderVersion;
			MP4UInteger8BitsProperty_t			_mui8pH263Level;
			MP4UInteger8BitsProperty_t			_mui8pH263Profile;

		protected:
			MP4D263Atom (const MP4D263Atom &);

			MP4D263Atom &operator = (const MP4D263Atom &);

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
			MP4D263Atom ();

			/**
				Distruttore.
			*/
			virtual ~MP4D263Atom ();

			#ifdef WIN32
				virtual Error init (MP4Atom *pmaParentAtom,
					FileReader_p pfFile,
					__int64 ullAtomStartOffset,
					__int64 ullAtomSize,
					unsigned long ulHeaderSize,
					PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
					Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer);
			#else
				virtual Error init (MP4Atom *pmaParentAtom,
					FileReader_p pfFile,
					unsigned long long ullAtomStartOffset,
					unsigned long long ullAtomSize,
					unsigned long ulHeaderSize,
					PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
					Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer);
			#endif

			virtual Error getType (char *pType);

			Error getH263Level (unsigned long *pulH263Level);

			Error setH263Level (unsigned long ulH263Level);

			Error getH263Profile (unsigned long *pulH263Profile);

			Error setH263Profile (unsigned long ulH263Profile);

	} MP4D263Atom_t, *MP4D263Atom_p;

#endif


