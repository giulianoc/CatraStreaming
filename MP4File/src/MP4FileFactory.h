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


#ifndef MP4FileFactory_h
	#define MP4FileFactory_h

	#include "MP4File.h"
	#include "MP4FileErrors.h"
	#include "PMutex.h"
	#if defined(__CYGWIN__) || defined (WIN32)
		#define MAXLONG			0x7fffffff
	#else
		#include <values.h>
	#endif

	#ifdef SLACK_LIB
		#include "slack/config.h"
		#include "slack/map.h"
	#else	// my_hash_map
		#include "my_hash_map.h"
	#endif



	#ifdef SLACK_LIB
	#else
		/* old
		struct eqstr
		{
			// realized in the .cpp file
			bool operator()(const char* s1, const char* s2) const;
		};
		*/
	#endif


	typedef class MP4FileFactory {

		private:
			typedef struct ElementSet {
				time_t				_tLastUsedTime;
				long				_lReferencesNumber;
				MP4File_p			_pmp4File;
			} ElementSet_t, *ElementSet_p;

			#ifdef SLACK_LIB
			#else
				typedef struct BufferHasher: MyHasherModel<Buffer_p>
				{
					public:
						int operator() (Buffer_p const &key) const
						{
							int i, result = 0;
							const int length = strlen (
								(const char *) (*key));
							for(i=0; i<length ; i++)
							{
								result = result*5 + (*key)[(long) i];
							}
							// GENERIC_HASH is defined in my_hash_map.H

							return GENERIC_intHASH(result);
						}
				} BufferHasher_t, *BufferHasher_p;

				typedef struct BufferCmp
				{
					bool operator()(const Buffer_p x, const Buffer_p y) const
					{
						return !strcmp((const char *) (*x), (const char *) (*y));
					}
				} BufferCmp_t, *BufferCmp_p;

				typedef my_hash_map<Buffer_p, ElementSet_p, BufferHasher, BufferCmp>
					ElementSetHashMap_t, *ElementSetHashMap_p;
				// typedef hash_map<const char*, ElementSet_p, hash<const char*>,
				//	eqstr> ElementSetHashMap_t, *ElementSetHashMap_p;
			#endif

		private:
			Tracer_p				_ptTracer;
			unsigned long			_ulMaxMp4FilesNumberInMemory;
			unsigned long			_ulMp4FilesNumberToDeleteOnOverflow;
			Boolean_t				_bUseMP4ConsistencyCheck;
			#ifdef SLACK_LIB
				Map						*_pmMP4FileSet;
			#else
				BufferHasher_p			_phHasher;
				BufferCmp_p				_pcComparer;
				ElementSetHashMap_p		_pmpsMP4FileSet;
			#endif
			PMutex_t				_mtMP4FileSet;

			Error removeOlderNotUsedMp4File ();

			Error findIndexWhereInsert (ElementSet_p pesOlderElementsToDelete,
				ElementSet_p pesElementToInsert, long *plIndexWhereInsert);

		protected:
			MP4FileFactory (const MP4FileFactory &);

			MP4FileFactory &operator = (const MP4FileFactory &);

		public:
			MP4FileFactory ();

			~MP4FileFactory ();

			Error init (unsigned long ulMaxMp4FilesNumberInMemory,
				unsigned long ulMp4FilesNumberToDeleteOnOverflow,
				Boolean_t bUseMP4ConsistencyCheck, Tracer_p ptTracer,
				unsigned long ulBucketsNumberForMP4FilesCache);

			Error finish (void);

			Error getMP4File (const char *pmp4FilePath,
				Boolean_t bUse64Bits, MP4File_p *pmp4File,
				Boolean_t bMP4FileToBeModified,
				unsigned long ulFileCacheSizeInBytes,
				MP4Atom:: Standard_t sStandard,
				Boolean_p pbIsMP4FileInCache);

			Error isMP4FileInCache (const char *pmp4FilePath,
				Boolean_p pbIsMP4FileInCache);

			Error releaseMP4File (const char *pmp4FilePath);

	} MP4FileFactory_t, *MP4FileFactory_p;

#endif

