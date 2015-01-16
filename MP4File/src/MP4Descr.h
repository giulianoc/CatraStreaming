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


#ifndef MP4Descr_h
	#define MP4Descr_h

	#include "MP4Atom.h"
	#include "MP4FileErrors.h"


	typedef class MP4Descr: public MP4Atom {

		protected:
			unsigned char				_ucNumBytesForDescriptorSize;
			unsigned long				_ulTag;

		protected:
			MP4Descr (const MP4Descr &);

			MP4Descr &operator = (const MP4Descr &);

			virtual Error readChildAtoms (void);

		public:
			MP4Descr ();

			virtual ~MP4Descr ();

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

			#ifdef WIN32
				virtual Error init (MP4Atom_p pmaParentAtom, FileReader_p pfFile,
					__int64 ullAtomStartOffset,
					unsigned char ucNumBytesForDescriptorSize,
					unsigned long ulTag, PMutex_p pmtMP4File, long lAtomLevel,
					Boolean_t bUse64Bits, Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard,
					Tracer_p ptTracer, __int64 *pullAtomSize);
			#else
				virtual Error init (MP4Atom_p pmaParentAtom, FileReader_p pfFile,
					unsigned long long ullAtomStartOffset,
					unsigned char ucNumBytesForDescriptorSize,
					unsigned long ulTag, PMutex_p pmtMP4File, long lAtomLevel,
					Boolean_t bUse64Bits, Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard,
					Tracer_p ptTracer, unsigned long long *pullAtomSize);
			#endif

			virtual Error write (int iFileDescriptor);

			virtual Error getTag (unsigned long *pulTag);

	} MP4Descr_t, *MP4Descr_p;

#endif

