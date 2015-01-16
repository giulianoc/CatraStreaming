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


#ifndef MP4BytesProperty_h
	#define MP4BytesProperty_h

	#include "MP4Property.h"
	#include "FileReader.h"
	// #include "RTPPacket.h"

	#define MP4_MAXLONGLENGTH					512 + 1


	typedef class MP4BytesProperty: public MP4Property {

		private:
			unsigned char				**_pucValues;
			#ifdef WIN32
				__int64						_ullBytesNumber;
			#else
				unsigned long long			_ullBytesNumber;
			#endif

		public:
			MP4BytesProperty (void);

			~MP4BytesProperty (void);

			#ifdef WIN32
				Error init (const char *pName, long lInstancesNumber,
					Boolean_p pbIsImplicitProperty,
					__int64 ullBytesNumber, unsigned char **pucValues,
					Tracer_p ptTracer);
			#else
				Error init (const char *pName, long lInstancesNumber,
					Boolean_p pbIsImplicitProperty,
					unsigned long long ullBytesNumber,
					unsigned char **pucValues,
					Tracer_p ptTracer);
			#endif

			#ifdef WIN32
				Error init (const char *pName, long lInstancesNumber,
					__int64 ullBytesNumber, FileReader_p pfFile,
					Tracer_p ptTracer);
			#else
				Error init (const char *pName, long lInstancesNumber,
					unsigned long long ullBytesNumber, FileReader_p pfFile,
					Tracer_p ptTracer);
			#endif

			#ifdef WIN32
				Error init (const char *pName, long lInstancesNumber,
					__int64 ullBytesNumber, const unsigned char *pucBuffer,
					Tracer_p ptTracer);
			#else
				Error init (const char *pName, long lInstancesNumber,
					unsigned long long ullBytesNumber,
					const unsigned char *pucBuffer,
					Tracer_p ptTracer);
			#endif

			#ifdef WIN32
				Error getData (
					__int64 ullOffset, __int64 llBytesToRead,
					long lIndex, unsigned char *pucBuffer);
			#else
				Error getData (
					unsigned long long ullOffset, long long llBytesToRead,
					long lIndex, unsigned char *pucBuffer);
			#endif


			/*
			#ifdef WIN32
				Error appendDataToRTPPacket (
					__int64 ullOffset, __int64 llBytesToRead,
					long lIndex, RTPPacket_p prpRTPPacket) const;
			#else
				Error appendDataToRTPPacket (
					unsigned long long ullOffset, long long llBytesToRead,
					long lIndex, RTPPacket_p prpRTPPacket) const;
			#endif
			*/

			#ifdef WIN32
				Error getPointerToData (unsigned long ulOffset, long lIndex,
					unsigned char **pucBuffer, __int64 *pullBytesNumber);
			#else
				Error getPointerToData (unsigned long ulOffset, long lIndex,
					unsigned char **pucBuffer,
					unsigned long long *pullBytesNumber);
			#endif

			#ifdef WIN32
				Error setData (unsigned char *pucValue,
					__int64 ullBytesNumber, long lIndex,
					long *plSizeChangedInBytes);
			#else
				Error setData (unsigned char *pucValue,
					unsigned long long ullBytesNumber, long lIndex,
					long *plSizeChangedInBytes);
			#endif

			#ifdef WIN32
				Error appendData (unsigned char *pucValue,
					__int64 ullBytesNumber, long lIndex,
					unsigned long *pulSizeChangedInBytes);
			#else
				Error appendData (unsigned char *pucValue,
					unsigned long long ullBytesNumber, long lIndex,
					unsigned long *pulSizeChangedInBytes);
			#endif

			virtual Error write (int iFileDescriptor);

			virtual Error getSize (unsigned long *pulSize);

			virtual Error getType (char *pType);

			virtual Error appendDump (Buffer_p pbBuffer);

			virtual Error printOnStdOutput (void);

	} MP4BytesProperty_t, *MP4BytesProperty_p;

#endif

