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


#ifndef MP4UInteger64BitsProperty_h
	#define MP4UInteger64BitsProperty_h

	#include "MP4Property.h"
	#include "FileReader.h"

	#define MP4_MAXLONGLENGTH					512 + 1


	typedef class MP4UInteger64BitsProperty: public MP4Property {

		private:
			#ifdef WIN32
				__int64							*_pullValues;
			#else
				unsigned long long 				*_pullValues;
			#endif

		public:
			MP4UInteger64BitsProperty (void);

			~MP4UInteger64BitsProperty (void);

			#ifdef WIN32
				Error init (const char *pName, long lInstancesNumber,
					Boolean_p pbIsImplicitProperty, __int64 *pullValues,
					Tracer_p ptTracer);
			#else
				Error init (const char *pName, long lInstancesNumber,
					Boolean_p pbIsImplicitProperty, unsigned long long *pullValues,
					Tracer_p ptTracer);
			#endif

			Error init (const char *pName, long lInstancesNumber,
				FileReader_p pfFile, Tracer_p ptTracer);

			Error init (const char *pName, long lInstancesNumber,
				const unsigned char *pucBuffer, Tracer_p ptTracer);

			#ifdef WIN32
				Error getValue (__int64 *pullValue, long lIndex);
			#else
				Error getValue (unsigned long long *pullValue, long lIndex);
			#endif

			Error getValue (unsigned char *pucValue, long lIndex);

			#ifdef WIN32
				Error setValue (__int64 ullValue, long lIndex,
					unsigned long *pulSizeChangedInBytes);
			#else
				Error setValue (unsigned long long ullValue, long lIndex,
					unsigned long *pulSizeChangedInBytes);
			#endif

			#ifdef WIN32
				Error incrementValue (long lIndex, __int64 llIncrement);
			#else
				Error incrementValue (long lIndex, long long llIncrement);
			#endif
			virtual Error write (int iFileDescriptor);

			virtual Error getSize (unsigned long *pulSize);

			virtual Error getType (char *pType);

			virtual Error appendDump (Buffer_p pbBuffer);

			virtual Error printOnStdOutput (void);

	} MP4UInteger64BitsProperty_t, *MP4UInteger64BitsProperty_p;

#endif

