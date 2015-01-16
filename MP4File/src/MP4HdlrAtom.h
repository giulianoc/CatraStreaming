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


#ifndef MP4HdlrAtom_h
	#define MP4HdlrAtom_h

	#include "MP4Atom.h"
	#include "MP4FileErrors.h"

	#define MP4F_MAXHANDLERTYPELENGTH				4 + 1


	typedef class MP4HdlrAtom: public MP4Atom {

		private:
			MP4UInteger8BitsProperty_t				_mui8pVersion;
			MP4UInteger24BitsProperty_t				_mui24pFlags;
			MP4BytesProperty_t						_mbpReserved1;
			MP4StringFixedProperty_t				_mstrfpHandlerType;
			MP4BytesProperty_t						_mbpReserved2;

			// INFO:
			// There is a spec incompatiblity between QT and MP4
			// QT says name field is a counted string
			// MP4 says name field is a null terminated string
			MP4StringLastProperty_t					_mstrlpName;

		protected:
			MP4HdlrAtom (const MP4HdlrAtom &);

			MP4HdlrAtom &operator = (const MP4HdlrAtom &);

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
			MP4HdlrAtom ();

			virtual ~MP4HdlrAtom ();

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

			Error getHandlerType (char *pHandlerType,
				unsigned long ulHandlerTypeLength);

			Error getHandlerType (Buffer_p pbHandlerType);

			Error setHandlerType (Buffer_p pbHandlerType);

			Error getName (Buffer_p pbName);

			Error setName (Buffer_p pbName);

	} MP4HdlrAtom_t, *MP4HdlrAtom_p;

#endif
