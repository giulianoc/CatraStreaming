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


#ifndef MP4Property_h
	#define MP4Property_h

	#include "Tracer.h"
	#include "MP4FileErrors.h"


	#define MP4_MAXPROPERTYNAMELENGTH					512 + 1


	typedef class MP4Property {

		protected:
			char					_pName [MP4_MAXPROPERTYNAMELENGTH];
			Tracer_p				_ptTracer;
			Boolean_t				_bIsImplicitProperty;
			long					_lInstancesNumber;

		public:
			MP4Property (void);

			~MP4Property (void);

			Error init (const char *pName, long lInstancesNumber,
				Boolean_p pbIsImplicitProperty, Tracer_p ptTracer);

			Error getName (Buffer_p pbValue);

			virtual Error write (int iFileDescriptor) = 0;

			virtual Error getSize (unsigned long *pulSize) = 0;

			virtual Error getType (char *pType) = 0;

			virtual Error appendDump (Buffer_p pbBuffer) = 0;

			virtual Error printOnStdOutput (void) = 0;

	} MP4Property_t, *MP4Property_p;

#endif

