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


#ifndef MP4File_h
	#define MP4File_h

	#include "MP4RootAtom.h"
	#include "MP4HdlrAtom.h"
	#include "MP4TrakAtom.h"
	#include "MP4MdatAtom.h"
	#include "MP4Utility.h"
	#include "FileReader.h"
	#include "MP4FileErrors.h"
	#include "MP4TrackInfo.h"
	// #include "RTPHintSample.h"
	#include <vector>


	#define MP4F_MAXTRAKPATHLENGTH			1024 + 1
	#define MP4F_MAXPATHNAMELENGTH			1024 + 1


	/**
	The MP4File handles a MP4 file.

	The following is a list of the main features of the MP4File library:

		* read the MP4 file (ISMA and 3GPP file)
		* write the MP4 file (ISMA and 3GPP file)
		* selection of the standard to apply according the extension file:
			3GPP for the ‘.3gp’ extension, ISMA for the ‘.mp4’ extension
		* create and add the hint tracks
		* add and delete atoms
		* change the properties of an atom
		* generate the Session Description Protocol (SDP)
		* management of the cache
		* management of a MP4 File Factory
										
	The steps to use the library are the following:

		* instantiate a MP4File object (MP4File_t mp4File)
		* initialize the MP4 file (mp4File. init (...))
		* use of the object (mp4File. … ())
		* finish of the object (mp4File. finish ())
	*/
	typedef class MP4File {

		public:
			typedef enum MP4FileStatus {
				MP4F_BUILDED,
				MP4F_INITIALIZED
			} MP4FileStatus_t, *MP4FileStatus_p;

		private:
			Boolean_t							_bInternalFile;
			FileReader_p						_pfFile;
			Boolean_t							_bUse64Bits;
			Boolean_t							_bUseMP4ConsistencyCheck;
			Tracer_p							_ptTracer;
			MP4RootAtom_t						_maRootAtom;
			MP4FileStatus_t						_smMP4FileStatus;
			PMutex_t							_mtMP4File;
			std:: vector<MP4TrackInfo_p>		_vMP4TracksInfo;

		protected:
			MP4File (const MP4File &);

			MP4File &operator = (const MP4File &);

			Error initializeTracksVector (void);

			friend std:: ostream &operator << (std:: ostream &osOutputStream,
				MP4File &maMP4File);

		public:
			/**
				Costruttore.
			*/
			MP4File ();

			/**
				Distruttore.
			*/
			~MP4File ();

			Error init (const char *pMP4FilePath, Boolean_t bUse64Bits,
				Boolean_t bUseMP4ConsistencyCheck, Boolean_t bToBeModified,
				unsigned long ulFileCacheSizeInBytes,
				MP4Atom:: Standard_t sStandard, Tracer_p ptTracer);

			Error init (FileReader_p pfFile, Boolean_t bUse64Bits,
				Boolean_t bUseMP4ConsistencyCheck, Boolean_t bToBeModified,
				MP4Atom:: Standard_t sStandard, Tracer_p ptTracer);

			Error finish (void);

			operator const char * (void) const;

			Error getDump (Buffer_p pbBuffer);

			Error getTracksInfo (
				std:: vector<MP4TrackInfo_p> **pvMP4TracksInfo);

			Error getHintTrakFromMediaTrakIdentifier (
				unsigned long *pulMediaTrackIdentifiers,
				unsigned long ulMediaTracksNumber,
				MP4TrakAtom_p *pmaHintTrakAtom);

			Error getHintTrackInfoFromMediaTrackInfo (
				MP4TrackInfo_p pmtiMediaTrakInfo,
				MP4TrackInfo_p *pmtiHintTrakInfo);

			Error getTrakInfoFromIdentifier (unsigned long ulTrackIdentifier,
				MP4TrackInfo_p *pmtiTrakInfo);

			Error getTrakInfoFromTrackPointer (MP4TrakAtom_p pmaTrakAtom,
				MP4TrackInfo_p *pmtiTrakInfo);

			Error getFile (FileReader_p *pfFile);

			Error getRootAtom (MP4RootAtom_p *pmaRootAtom);

			Error write (const char *pMP4PathName);

			Error getMP4FileStatus (MP4FileStatus_p psmMP4FileState);

			/**
				pAtomPath can be: <ChildType>:<InstanceIndex>:...
					...:<SearchedChildType>:<InstanceIndex>
				InstanceIndex may start from 0
			*/
			Error searchAtom (const char *pAtomPath,
				Boolean_t bIsSearchErrorTraceEnabled, MP4Atom **pmp4Atom);

			Error getMovieDuration (MP4TrackInfo_p pmtiVideoTrackInfo,
				double *pdMovieDuration);

			Error getFreePayloadsNumber (
				unsigned long ulPayloadsNumberToBeReturned,
				unsigned long *pulPayloadsNumber);

			/*
			Error getSDP (const char *pIPAddressForRTSP,
				RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
				Boolean_t bVideoTrack,
				MP4TrackInfo_p pmtiHintOrMediaVideoTrackInfo,
				char *pVideoTrackName, unsigned long *pulVideoPayloadNumber,
				Boolean_t bAudioTrack,
				MP4TrackInfo_p pmtiHintOrMediaAudioTrackInfo,
				char *pAudioTrackName, unsigned long *pulAudioPayloadNumber,
				double dMovieDuration, Buffer_p pbSDP);
			*/

			Error getMP4FilePath (char *pMP4FilePath);

	} MP4File_t, *MP4File_p;

#endif

