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


#ifndef MP4MdatAtom_h
	#define MP4MdatAtom_h

	#include "MP4Atom.h"
	#include "MP4FileErrors.h"

	#define MP4F_BLOCKMEMORYSIZE		1024


	typedef class MP4MdatAtom: public MP4Atom {

		private:
			MP4BytesProperty_t		_mbpData;
			Boolean_t				_bMDatPropertyToBeReadFromFile;
			#ifdef WIN32
				__int64					_ullMDatPropertySize;
			#else
				unsigned long long		_ullMDatPropertySize;
			#endif
			unsigned char			*_pucPreAllocatedMDatProperty;
			unsigned long			_ulBlockMemoryAllocated;

		protected:
			MP4MdatAtom (const MP4MdatAtom &);

			MP4MdatAtom &operator = (const MP4MdatAtom &);

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
			MP4MdatAtom ();

			virtual ~MP4MdatAtom ();

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

			#ifdef WIN32
				virtual Error init (MP4Atom *pmaParentAtom,
					FileReader_p pfFile,
					__int64 ullAtomStartOffset, PMutex_p pmtMP4File, long lAtomLevel,
					Boolean_t bUse64Bits, Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer,
					__int64 *pullAtomSize);
			#else
				virtual Error init (MP4Atom *pmaParentAtom,
					FileReader_p pfFile,
					unsigned long long ullAtomStartOffset, PMutex_p pmtMP4File, long lAtomLevel,
					Boolean_t bUse64Bits, Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer,
					unsigned long long *pullAtomSize);
			#endif

			virtual Error finish (void);

			virtual Error getType (char *pType);

			#ifdef WIN32
				Error getData (__int64 ullFileOffset,
					unsigned long ulBytesToRead, unsigned char *pucBffer);
			#else
				Error getData (unsigned long long ullFileOffset,
					unsigned long ulBytesToRead, unsigned char *pucBffer);
			#endif

			#ifdef WIN32
				Error getPointerToData (__int64 ullFileOffset,
					unsigned char **pucBffer,
					__int64 *pullBytesToRead);
			#else
				Error getPointerToData (unsigned long long ullFileOffset,
					unsigned char **pucBffer,
					unsigned long long *pullBytesToRead);
			#endif

			/*
			#ifdef WIN32
				Error appendDataToRTPPacket (
					__int64 ullFileOffset,
					__int64 llBytesToRead, RTPPacket_p prpRTPPacket);
			#else
				Error appendDataToRTPPacket (
					unsigned long long ullFileOffset,
					long long llBytesToRead, RTPPacket_p prpRTPPacket);
			#endif
			*/

			#ifdef WIN32
				Error getPointerToData (
					__int64 ullFileOffset,
					__int64 ullBytesToRead,
					unsigned char **pucBffer);
			#else
				Error getPointerToData (
					unsigned long long ullFileOffset,
					unsigned long long ullBytesToRead,
					unsigned char **pucBffer);
			#endif

			Error appendData (unsigned char *pucData,
				unsigned long ulBytesNumber);

			#ifdef WIN32
				Error getDataStartOffset (__int64 *pullDataStartOffset);
			#else
				Error getDataStartOffset (unsigned long long *pullDataStartOffset);
			#endif

	} MP4MdatAtom_t, *MP4MdatAtom_p;

#endif

