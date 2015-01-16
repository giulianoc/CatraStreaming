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


#include "Tracer.h"
#include "MP4File.h"
#include "MP4ElstAtom.h"
#include "MP4MdhdAtom.h"
#include "MP4MvhdAtom.h"
#include "MP4StszAtom.h"
#include "RTPUtility.h"
#include "FileIO.h"
#include "mpeg4.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#ifdef WIN32
	#include <direct.h>
#else
	#include <unistd.h>
#endif


int main (int argc, char **argv)

{

	const char							*pPathName;
	Tracer_t							gtTracer;
	long								lSecondsBetweenTwoCheckTraceToManage;
	MP4File_t							mp4File;
	Error_t								errGeneric;
	Boolean_t							bUse64Bits;
	Boolean_t							bUseMP4ConsistencyCheck;
	Boolean_t							bToBeModified;
	long								lTraceLevel;
	MP4Atom:: Standard_t				sStandard;
	// used for instant bit rate
	double								dSpecifiedRangeForInstantBitRate;
	unsigned long						ulSampleSizeForSpecifiedRange;
	unsigned long						ulRangesCounter;
	unsigned long						ulFileCacheSizeInBytes;


	{
		char		*pCopyright = "\n***************************************************************************\ncopyright            : (C) by CatraSoftware\nemail                : catrastreaming-support@catrasoftware.it\n***************************************************************************\n";

		std:: cout << pCopyright << std:: endl;
	}

	if (argc < 2 || argc > 5)
	{
		std:: cout
			<< "This tool creates: "
			<< std:: endl
			<< "  1. a file with all the information about the tracks included into the MP4 file"
			<< "  2. one file for each track including the relative bitstream"
			<< std:: endl
			<< std:: endl
			<< "-----------------------------------------------"
			<< std:: endl
			<< std:: endl;

		std:: cout << "Usage: " << argv [0]
			<< std:: endl << "\t"
			<< "<MP4 path name>"
			<< std:: endl << "\t"
			<< "<Specified range in seconds to calculate instant bit rate>. Default: 0.5"
			<< std:: endl << "\t"
			<< "<trace level 0..6 (0 -> all, 6 -> fatal error)>. Default: 6"
			<< std:: endl << "\t"
			"<consistencyCheck: 0, 1>. Default: 0"
			<< std:: endl;

		return 1;
	}

	pPathName								= argv [1];

	if (argc == 2)
	{
		dSpecifiedRangeForInstantBitRate		= 0.5;
		lTraceLevel								= 6;
		bUseMP4ConsistencyCheck					= false;
	}
	else if (argc == 3)
	{
		dSpecifiedRangeForInstantBitRate		= atof (argv [2]);
		lTraceLevel								= 6;
		bUseMP4ConsistencyCheck					= false;
	}
	else if (argc == 4)
	{
		dSpecifiedRangeForInstantBitRate		= atof (argv [2]);
		lTraceLevel								= atol (argv [3]);
		bUseMP4ConsistencyCheck					= false;
	}
	else if (argc == 5)
	{
		dSpecifiedRangeForInstantBitRate	= atof (argv [2]);
		lTraceLevel							= atol (argv [3]);
		bUseMP4ConsistencyCheck				= argv [4][0] == '0' ? false : true;
	}
	else			// if (argc == 5)
	{
		dSpecifiedRangeForInstantBitRate	= atof (argv [2]);
		lTraceLevel							= atol (argv [3]);
		bUseMP4ConsistencyCheck				= argv [4][0] == '0' ? false : true;
	}

	lSecondsBetweenTwoCheckTraceToManage		= 7;

	// I will not handle the error in case the directory already exists
	#ifdef WIN32
		_mkdir ("./trace");
	#else
		mkdir ("./trace", S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP |
			S_IXGRP | S_IROTH | S_IXOTH);
	#endif

	if ((errGeneric = gtTracer. init (
		"MP4TracksInfo",				// pName
		10,								// lCacheSizeOfTraceFile K-byte
		"./trace",						// pBaseTraceFileName
		"@MM@_@DD@_@HI24@_MP4TracksInfo",		// pBaseTraceFileName
		1000,							// lMaxTraceFileSize K-byte
        3600,							// lTraceFilePeriodInSecs
		false,							// bCompressedTraceFile
		false,							// bClosedFileToBeCopied
		(const char *) NULL,			// pClosedFilesRepository
		5,								// lTraceFilesNumberToMaintain
		true,							// bTraceOnFile
		true,							// bTraceOnTTY
		lTraceLevel,					// lTraceLevel
		lSecondsBetweenTwoCheckTraceToManage,
		3000,							// lMaxTracesNumber
		7532,							// lListenPort
		10000,							// lTracesNumberAllocatedOnOverflow
		1000)) != errNoError)			// lSizeOfEachBlockToGzip K-byte
	{
		std:: cerr << (const char *) errGeneric << std:: endl;

		errGeneric = TracerErrors (__FILE__, __LINE__,
			TRACER_TRACER_INIT_FAILED);
		std:: cerr << (const char *) errGeneric << std:: endl;

		return 1;
	}

	if ((errGeneric = gtTracer. start ()) != errNoError)
	{
		std:: cerr << (const char *) errGeneric << std:: endl;

		errGeneric = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_START_FAILED);
		std:: cerr << (const char *) errGeneric << std:: endl;

		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		return 1;
	}

	bUse64Bits						= false;
	bToBeModified					= false;
	ulFileCacheSizeInBytes			= 1000 * 1024;

	{
		unsigned long				ulFileNameLength;


		ulFileNameLength				= strlen (pPathName);

		if (tolower (pPathName [ulFileNameLength - 1]) == 'p' &&
			tolower (pPathName [ulFileNameLength - 2]) == 'g' &&
			pPathName [ulFileNameLength - 3] == '3')
		{
			sStandard			= MP4Atom:: MP4F_3GPP;
		}
		else
		{
			sStandard			= MP4Atom:: MP4F_ISMA;
		}
	}

	if ((errGeneric = mp4File. init (pPathName,
		bUse64Bits, bUseMP4ConsistencyCheck,
		bToBeModified, ulFileCacheSizeInBytes,
		sStandard, &gtTracer)) != errNoError)
	{
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		errGeneric = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_INIT_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			std:: cerr << ((const char *) errGeneric) << std:: endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		return 1;
	}

	{
		FileReader_p					pfFile;
		unsigned long					ulMediaSamplesNumber;
		unsigned long					ulMediaSampleIdentifier;
		unsigned long					ulCurrentSampleSize;
		unsigned long					ulSampleDescriptionIndex;
		unsigned char					*pucMediaSample;
		std:: vector<MP4TrackInfo_p>:: const_iterator	it;
		MP4TrackInfo_p					pmtiMP4TrackInfo;
		std:: vector<MP4TrackInfo_p>	*pvMP4TracksInfo;
		int								iFdTracksInfo;
		int								iFdBitstream;
		char							pBuffer [1024];
		MP4RootAtom_p					pmaRootAtom;
		MP4MvhdAtom_p					pmaMvhdAtom;
		// unsigned long					ulFirstEditMovieTime;
		unsigned long					ulMvhdTimeScale;
		unsigned long					ulMdhdTimeScale;
		#ifdef WIN32
			__int64							llFilePosition;
			__int64							ullSampleFileOffset;
			__int64							ullSampleStartTime;
			__int64							ullSampleDuration;
			__int64							ullMvhdDuration;
			__int64							ullBitstreamSize;
			__int64							llBytesWritten;
			__int64							ullBytesRead;
		#else
			long long						llFilePosition;
			unsigned long long				ullSampleFileOffset;
			unsigned long long				ullSampleStartTime;
			unsigned long long				ullSampleDuration;
			unsigned long long				ullMvhdDuration;
			unsigned long long				ullBitstreamSize;
			long long						llBytesWritten;
			unsigned long long				ullBytesRead;
		#endif
		double							dMdhdDuration;
		Boolean_t						bIsSyncSample;
		MP4Atom:: MP4Codec_t			cCodec;
		char							pCodecName [MP4F_MAXCODECUSEDLENGTH];
		MP4Atom_p						pmaAtom;
		RTPStreamRealTimeInfo_t			rsrtRTPStreamRealTimeInfo;
		MP4StscAtom_p					pmaStscAtom;
		MP4StszAtom_p					pmaStszAtom;
		MP4StcoAtom_p					pmaStcoAtom;
		MP4StssAtom_p					pmaStssAtom;
		MP4SttsAtom_p					pmaSttsAtom;
		unsigned long					ulTrackIdentifier;
		char							pHandlerType [
			MP4F_MAXHANDLERTYPELENGTH];



		if ((errGeneric = mp4File. getFile (&pfFile)) != errNoError)
		{
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			errGeneric = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_GETFILE_FAILED);
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			if ((errGeneric = mp4File. finish ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. cancel ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. finish (true)) != errNoError)
				std:: cerr << (const char *) errGeneric << std:: endl;

			return 1;
		}

		if ((errGeneric = mp4File. getTracksInfo (&pvMP4TracksInfo)) !=
			errNoError)
		{
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			errGeneric = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_GETTRACKSINFO_FAILED);
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			if ((errGeneric = mp4File. finish ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. cancel ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. finish (true)) != errNoError)
				std:: cerr << (const char *) errGeneric << std:: endl;

			return 1;
		}

		if ((errGeneric = mp4File. getRootAtom (&pmaRootAtom)) !=
			errNoError)
		{
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			errGeneric = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_GETROOTATOM_FAILED);
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			if ((errGeneric = mp4File. finish ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. cancel ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. finish (true)) != errNoError)
				std:: cerr << (const char *) errGeneric << std:: endl;

			return 1;
		}

		sprintf (pBuffer, "%s_info", pPathName);

		#ifdef WIN32
			if ((errGeneric = FileIO:: open (pBuffer,
				O_WRONLY | O_TRUNC | O_CREAT,
				_S_IREAD | _S_IWRITE, &iFdTracksInfo)) !=
				errNoError)
		#else
			if ((errGeneric = FileIO:: open (pBuffer,
				O_WRONLY | O_TRUNC | O_CREAT,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
				&iFdTracksInfo)) != errNoError)
		#endif
		{
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_OPEN_FAILED);
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if ((errGeneric = mp4File. finish ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. cancel ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. finish (true)) != errNoError)
				std:: cerr << (const char *) errGeneric << std:: endl;

			return 1;
		}

		if (pmaRootAtom -> searchAtom ("moov:0:mvhd:0",
			true, &pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "moov:0:mvhd:0");
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			FileIO:: close (iFdTracksInfo);
			if ((errGeneric = mp4File. finish ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. cancel ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. finish (true)) != errNoError)
				std:: cerr << (const char *) errGeneric << std:: endl;

			return 1;
		}
		else
			pmaMvhdAtom		= (MP4MvhdAtom_p) pmaAtom;

		if (pmaMvhdAtom -> getTimeScale (&ulMvhdTimeScale) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4MVHDATOM_GETTIMESCALE_FAILED);
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			FileIO:: close (iFdTracksInfo);
			if ((errGeneric = mp4File. finish ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. cancel ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. finish (true)) != errNoError)
				std:: cerr << (const char *) errGeneric << std:: endl;

			return 1;
		}

		if (pmaMvhdAtom -> getDuration (&ullMvhdDuration) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4MVHDATOM_GETDURATION_FAILED);
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			FileIO:: close (iFdTracksInfo);
			if ((errGeneric = mp4File. finish ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. cancel ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. finish (true)) != errNoError)
				std:: cerr << (const char *) errGeneric << std:: endl;

			return 1;
		}

		sprintf (pBuffer, "Movie duration: %lf, movie timescale: %lu\n",
			(double) ((double) ullMvhdDuration / (double) ulMvhdTimeScale),
			ulMvhdTimeScale);

		if ((errGeneric = FileIO:: writeChars (iFdTracksInfo,
			pBuffer, strlen (pBuffer), &llBytesWritten)) != errNoError)
		{
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITEBYTES_FAILED);
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			FileIO:: close (iFdTracksInfo);
			if ((errGeneric = mp4File. finish ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. cancel ()) != errNoError)
				std:: cerr << ((const char *) errGeneric) << std:: endl;
			if ((errGeneric = gtTracer. finish (true)) != errNoError)
				std:: cerr << (const char *) errGeneric << std:: endl;

			return 1;
		}

		for (it = pvMP4TracksInfo -> begin ();
			it != pvMP4TracksInfo -> end (); ++it)
		{
			pmtiMP4TrackInfo				= *it;

			if ((errGeneric = pmtiMP4TrackInfo -> getTrackIdentifier (
				&ulTrackIdentifier)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETTRACKIDENTIFIER_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				FileIO:: close (iFdTracksInfo);
				if ((errGeneric = mp4File. finish ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			if ((errGeneric = pmtiMP4TrackInfo -> getHandlerType (
				pHandlerType)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETHANDLERTYPE_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				FileIO:: close (iFdTracksInfo);
				if ((errGeneric = mp4File. finish ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			if ((errGeneric = pmtiMP4TrackInfo -> getSamplesNumber (
				&ulMediaSamplesNumber, true)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSAMPLESNUMBER_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				FileIO:: close (iFdTracksInfo);
				if ((errGeneric = mp4File. finish ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			if ((errGeneric = pmtiMP4TrackInfo -> getMdhdTimeScale (
				&ulMdhdTimeScale)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETMDHDTIMESCALE_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				FileIO:: close (iFdTracksInfo);
				if ((errGeneric = mp4File. finish ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			if ((errGeneric = pmtiMP4TrackInfo -> getDuration (
				&dMdhdDuration)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETDURATION_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				FileIO:: close (iFdTracksInfo);
				if ((errGeneric = mp4File. finish ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			// ulFirstEditMovieTime
			/*
			if ((errGeneric = pmtiMP4TrackInfo -> getElstAtom (
				&pmaElstAtom)) != errNoError)
			{
				ulFirstEditMovieTime					= 0;
			}
			else
			{
				unsigned long						ulEntriesNumber;
				unsigned long						ulEntryIndex;
				unsigned long						ulMediaTime;
				long								lMediaTime;


				if (pmaElstAtom -> getEntriesNumber (&ulEntriesNumber) !=
					errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
	
					FileIO:: close (iFdTracksInfo);
					if ((errGeneric = mp4File. finish ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. cancel ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. finish (true)) != errNoError)
						std:: cerr << (const char *) errGeneric << std:: endl;

					return 1;
				}

				for (ulEntryIndex = 0; ulEntryIndex < ulEntriesNumber;
					ulEntryIndex += 3)
				{
					if (pmaElstAtom -> getEditListTable (ulEntryIndex + 1,
						&ulMediaTime) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4PROPERTY_GETVALUE_FAILED);
						gtTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						FileIO:: close (iFdTracksInfo);
						if ((errGeneric = mp4File. finish ()) != errNoError)
							std:: cerr << ((const char *) errGeneric) << std:: endl;
						if ((errGeneric = gtTracer. cancel ()) != errNoError)
							std:: cerr << ((const char *) errGeneric) << std:: endl;
						if ((errGeneric = gtTracer. finish (true)) !=
							errNoError)
							std:: cerr << (const char *) errGeneric << std:: endl;

						return 1;
					}

					lMediaTime				= ulMediaTime;

					if (lMediaTime == -1)
					{
						// segmentDuration
						if (pmaElstAtom -> getEditListTable (ulEntryIndex,
							&ulFirstEditMovieTime) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4PROPERTY_GETVALUE_FAILED);
							gtTracer. trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							FileIO:: close (iFdTracksInfo);
							if ((errGeneric = mp4File. finish ()) != errNoError)
								std:: cerr << ((const char *) errGeneric) << std:: endl;
							if ((errGeneric = gtTracer. cancel ()) !=
								errNoError)
								std:: cerr << ((const char *) errGeneric) << std:: endl;
							if ((errGeneric = gtTracer. finish (true)) !=
								errNoError)
								std:: cerr << (const char *) errGeneric << std:: endl;

							return 1;
						}
					}
				}
			}
			*/

			if ((errGeneric = pmtiMP4TrackInfo -> getCodec (
				&cCodec)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETCODEC_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				FileIO:: close (iFdTracksInfo);
				if ((errGeneric = mp4File. finish ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			if (MP4Utility:: getCodecName (cCodec,
				pCodecName) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETCODECNAME_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				FileIO:: close (iFdTracksInfo);
				if ((errGeneric = mp4File. finish ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			if ((errGeneric = pmtiMP4TrackInfo -> getStscAtom (
				&pmaStscAtom, true)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSTSCATOM_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				FileIO:: close (iFdTracksInfo);
				if ((errGeneric = mp4File. finish ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			if ((errGeneric = pmtiMP4TrackInfo -> getStszAtom (
				&pmaStszAtom, true)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSTSZATOM_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				FileIO:: close (iFdTracksInfo);
				if ((errGeneric = mp4File. finish ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			if ((errGeneric = pmtiMP4TrackInfo -> getStcoAtom (
				&pmaStcoAtom, true)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSTCOATOM_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				FileIO:: close (iFdTracksInfo);
				if ((errGeneric = mp4File. finish ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			if ((errGeneric = pmtiMP4TrackInfo -> getStssAtom (
				&pmaStssAtom, true)) != errNoError)
			{
				if ((long) errGeneric == MP4F_MP4ATOM_ATOMNOTFOUND)
					pmaStssAtom		= (MP4StssAtom_p) NULL;
				else
				{
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4TRACKINFO_GETSTCOATOM_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					FileIO:: close (iFdTracksInfo);
					if ((errGeneric = mp4File. finish ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. cancel ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. finish (true)) != errNoError)
						std:: cerr << (const char *) errGeneric << std:: endl;

					return 1;
				}
			}

			if ((errGeneric = pmtiMP4TrackInfo -> getSttsAtom (
				&pmaSttsAtom, true)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4TRACKINFO_GETSTTSATOM_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				FileIO:: close (iFdTracksInfo);
				if ((errGeneric = mp4File. finish ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			/*
			sprintf (pBuffer,
				"\nTrack identifier: %lu, handler type: %s, Codec used: %s, samples: %lu, duration: %lf, mdhd timescale: %lu, first edit movie time: %lu\n",
				ulTrackIdentifier,
				pHandlerType,
				pCodecName,
				ulMediaSamplesNumber,
				dMdhdDuration,
				ulMdhdTimeScale,
				ulFirstEditMovieTime);
			*/
			sprintf (pBuffer,
				"\nTrack identifier: %lu, handler type: %s, Codec used: %s, samples: %lu, duration: %lf, mdhd timescale: %lu\n",
				ulTrackIdentifier,
				pHandlerType,
				pCodecName,
				ulMediaSamplesNumber,
				dMdhdDuration,
				ulMdhdTimeScale);

			if ((errGeneric = FileIO:: writeChars (iFdTracksInfo,
				pBuffer, strlen (pBuffer), &llBytesWritten)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_WRITEBYTES_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				FileIO:: close (iFdTracksInfo);
				if ((errGeneric = mp4File. finish ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			sprintf (pBuffer, "%s_TrackID_%lu_%s_bitstream",
				pPathName,
				ulTrackIdentifier,
				pHandlerType);

			#ifdef WIN32
				if ((errGeneric = FileIO:: open (pBuffer,
					O_WRONLY | O_TRUNC | O_CREAT | O_BINARY,
					_S_IREAD | _S_IWRITE, &iFdBitstream)) !=
					errNoError)
			#else
				if ((errGeneric = FileIO:: open (pBuffer,
					O_WRONLY | O_TRUNC | O_CREAT,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
					&iFdBitstream)) != errNoError)
			#endif
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_OPEN_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				FileIO:: close (iFdTracksInfo);
				if ((errGeneric = mp4File. finish ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}

			ullBitstreamSize					= 0;

			ulSampleSizeForSpecifiedRange			= 0;
			ulRangesCounter							= 1;

			for (ulMediaSampleIdentifier = 1;
				ulMediaSampleIdentifier <= ulMediaSamplesNumber;
				ulMediaSampleIdentifier++)
			{
				std:: cout << ".";
				std:: cout. flush ();

				if (ulMediaSampleIdentifier == 1)
				{
					if (!strcmp (pHandlerType,
						MP4_VIDEO_TRACK_TYPE))
					{
						switch (cCodec)
						{
							case MP4Atom:: MP4F_CODEC_MPEG4:
								{
									unsigned char				*pucESConfiguration;
									#ifdef WIN32
										__int64						ullESConfigurationSize;
									#else
										unsigned long long			ullESConfigurationSize;
									#endif

									if ((errGeneric = pmtiMP4TrackInfo ->
										getPointerTrackESConfiguration (
										&pucESConfiguration,
										&ullESConfigurationSize,
										sStandard)) != errNoError)
									{
										gtTracer. trace (Tracer:: TRACER_LERRR,
											(const char *) errGeneric,
											__FILE__, __LINE__);

										Error err = MP4FileErrors (
											__FILE__, __LINE__,
											MP4F_MP4TRACKINFO_GETCODEC_FAILED);
										gtTracer. trace (Tracer:: TRACER_LERRR,
											(const char *) err,
											__FILE__, __LINE__);

										FileIO:: close (iFdBitstream);
										FileIO:: close (iFdTracksInfo);
										if ((errGeneric = mp4File. finish ()) !=
											errNoError)
											std:: cerr << ((const char *) errGeneric)
												<< std:: endl;
										if ((errGeneric =
											gtTracer. cancel ()) != errNoError)
											std:: cerr << ((const char *) errGeneric)
												<< std:: endl;
										if ((errGeneric = gtTracer. finish (
											true)) != errNoError)
											std:: cerr << (const char *) errGeneric
												<< std:: endl;

										return 1;
									}

									if ((errGeneric = FileIO:: writeBytes (
										iFdBitstream,
										pucESConfiguration,
										ullESConfigurationSize,
										&llBytesWritten)) !=
										errNoError)
									{
										gtTracer. trace (Tracer:: TRACER_LERRR,
											(const char *) errGeneric, __FILE__, __LINE__);

										Error err = ToolsErrors (__FILE__, __LINE__,
											TOOLS_FILEIO_WRITEBYTES_FAILED);
										gtTracer. trace (Tracer:: TRACER_LERRR,
											(const char *) err, __FILE__, __LINE__);

										delete [] pucMediaSample;
										FileIO:: close (iFdBitstream);
										FileIO:: close (iFdTracksInfo);
										if ((errGeneric = mp4File. finish ()) != errNoError)
											std:: cerr << ((const char *) errGeneric) << std:: endl;
										if ((errGeneric = gtTracer. cancel ()) !=
											errNoError)
											std:: cerr << ((const char *) errGeneric) << std:: endl;
										if ((errGeneric = gtTracer. finish (true)) !=
											errNoError)
											std:: cerr << (const char *) errGeneric << std:: endl;

										return 1;
									}

									ullBitstreamSize			+= ullESConfigurationSize;

									break;
								}
							case MP4Atom:: MP4F_CODEC_H263:

								break;
							default:

								break;
						}
					}
					else
					{
					}
				}

				if ((errGeneric = RTPUtility:: getSampleInfoFromSampleNumber (
					pmaStscAtom, pmaStszAtom, pmaStcoAtom,
					ulMediaSampleIdentifier,
					&ullSampleFileOffset, &ulCurrentSampleSize,
					&ulSampleDescriptionIndex,
					&rsrtRTPStreamRealTimeInfo,
					&gtTracer)) != errNoError)
				{
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPUTILITY_GETSAMPLEINFOFROMSAMPLENUMBER_FAILED,
						1, ulMediaSampleIdentifier);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					FileIO:: close (iFdBitstream);
					FileIO:: close (iFdTracksInfo);
					if ((errGeneric = mp4File. finish ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. cancel ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. finish (true)) != errNoError)
						std:: cerr << (const char *) errGeneric << std:: endl;

					return 1;
				}

				if (pmaStssAtom != (MP4StssAtom_p) NULL)
				{
					if ((errGeneric = MP4Utility:: isSyncSample (
						pmaStssAtom, ulMediaSampleIdentifier,
						&bIsSyncSample, &gtTracer)) != errNoError)
					{
						gtTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4UTILITY_ISSYNCSAMPLE_FAILED,
							1, ulMediaSampleIdentifier);
						gtTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						FileIO:: close (iFdBitstream);
						FileIO:: close (iFdTracksInfo);
						if ((errGeneric = mp4File. finish ()) != errNoError)
							std:: cerr << ((const char *) errGeneric)
								<< std:: endl;
						if ((errGeneric = gtTracer. cancel ()) != errNoError)
							std:: cerr << ((const char *) errGeneric)
								<< std:: endl;
						if ((errGeneric = gtTracer. finish (true)) !=
							errNoError)
							std:: cerr << (const char *) errGeneric
								<< std:: endl;

						return 1;
					}
				}

				if ((pucMediaSample = new unsigned char [
					ulCurrentSampleSize]) == (unsigned char *) NULL)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_NEW_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					FileIO:: close (iFdBitstream);
					FileIO:: close (iFdTracksInfo);
					if ((errGeneric = mp4File. finish ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. cancel ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. finish (true)) != errNoError)
						std:: cerr << (const char *) errGeneric << std:: endl;

					return 1;
				}

				if ((errGeneric = pfFile -> seek (ullSampleFileOffset, SEEK_SET,
					&llFilePosition)) != errNoError)
				{
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_SEEK_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] pucMediaSample;
					FileIO:: close (iFdBitstream);
					FileIO:: close (iFdTracksInfo);
					if ((errGeneric = mp4File. finish ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. cancel ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. finish (true)) != errNoError)
						std:: cerr << (const char *) errGeneric << std:: endl;

					return 1;
				}

				if (pfFile -> readBytes (pucMediaSample,
					ulCurrentSampleSize, true, &ullBytesRead) != errNoError)
				{
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_READBYTES_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] pucMediaSample;
					FileIO:: close (iFdBitstream);
					FileIO:: close (iFdTracksInfo);
					if ((errGeneric = mp4File. finish ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. cancel ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. finish (true)) != errNoError)
						std:: cerr << (const char *) errGeneric << std:: endl;

					return 1;
				}

				if ((errGeneric = FileIO:: writeBytes (iFdBitstream,
					pucMediaSample, ulCurrentSampleSize,
					&llBytesWritten)) != errNoError)
				{
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_WRITEBYTES_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete [] pucMediaSample;
					FileIO:: close (iFdBitstream);
					FileIO:: close (iFdTracksInfo);
					if ((errGeneric = mp4File. finish ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. cancel ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. finish (true)) != errNoError)
						std:: cerr << (const char *) errGeneric << std:: endl;

					return 1;
				}

				delete [] pucMediaSample;

				ullBitstreamSize				+= ulCurrentSampleSize;

				if ((errGeneric = RTPUtility:: getSampleTimeFromSampleNumber (
					pmaSttsAtom,
					ulMediaSampleIdentifier, &ullSampleStartTime,
					&ullSampleDuration,
					&rsrtRTPStreamRealTimeInfo, &gtTracer)) != errNoError)
				{
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPUTILITY_GETSAMPLETIMEFROMSAMPLENUMBER_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					FileIO:: close (iFdBitstream);
					FileIO:: close (iFdTracksInfo);
					if ((errGeneric = mp4File. finish ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. cancel ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. finish (true)) != errNoError)
						std:: cerr << (const char *) errGeneric << std:: endl;


					return 1;
				}

				#ifdef WIN32
					sprintf (pBuffer, "SampleId %7lu, is sync sample: %s, size %6lu, duration: %9I64d, time: %9I64d %lf\n",
						ulMediaSampleIdentifier,
						pmaStssAtom != (MP4StssAtom_p) NULL ?
						(bIsSyncSample ? " 1" : " 0") :
						"NA",
						ulCurrentSampleSize,
						ullSampleDuration,
						ullSampleStartTime,
						((double) ullSampleStartTime / (double) ulMdhdTimeScale));
				#else
					sprintf (pBuffer, "SampleId %7lu, is sync sample: %s, size %6lu, duration: %9llu, time: %9llu %lf\n",
						ulMediaSampleIdentifier,
						pmaStssAtom != (MP4StssAtom_p) NULL ?
						(bIsSyncSample ? " 1" : " 0") :
						"NA",
						ulCurrentSampleSize,
						ullSampleDuration,
						ullSampleStartTime,
						((double) ullSampleStartTime / (double) ulMdhdTimeScale));
				#endif

				if ((errGeneric = FileIO:: writeChars (iFdTracksInfo,
					pBuffer, strlen (pBuffer), &llBytesWritten)) != errNoError)
				{
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_FILEIO_WRITEBYTES_FAILED);
					gtTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					FileIO:: close (iFdBitstream);
					FileIO:: close (iFdTracksInfo);
					if ((errGeneric = mp4File. finish ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. cancel ()) != errNoError)
						std:: cerr << ((const char *) errGeneric) << std:: endl;
					if ((errGeneric = gtTracer. finish (true)) != errNoError)
						std:: cerr << (const char *) errGeneric << std:: endl;


					return 1;
				}

				if (((double) ullSampleStartTime / (double) ulMdhdTimeScale) <
					(double) (dSpecifiedRangeForInstantBitRate *
					(double) ulRangesCounter))
				{
					ulSampleSizeForSpecifiedRange		+= ulCurrentSampleSize;
				}
				else
				{
					sprintf (pBuffer,
					"Instant bit rate at %lf time (TrackID: %lu): %lf kbps\n",
						((double) ullSampleStartTime /
						(double) ulMdhdTimeScale),
						ulTrackIdentifier,
						(double) (((double) ulSampleSizeForSpecifiedRange) /
						dSpecifiedRangeForInstantBitRate * 8.0 / 1000.0));

					if ((errGeneric = FileIO:: writeChars (iFdTracksInfo,
						pBuffer, strlen (pBuffer), &llBytesWritten)) != errNoError)
					{
						gtTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_FILEIO_WRITEBYTES_FAILED);
						gtTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						FileIO:: close (iFdBitstream);
						FileIO:: close (iFdTracksInfo);
						if ((errGeneric = mp4File. finish ()) != errNoError)
							std:: cerr << ((const char *) errGeneric) << std:: endl;
						if ((errGeneric = gtTracer. cancel ()) !=
							errNoError)
							std:: cerr << ((const char *) errGeneric) << std:: endl;
						if ((errGeneric = gtTracer. finish (true)) !=
							errNoError)
							std:: cerr << (const char *) errGeneric << std:: endl;


						return 1;
					}

					ulSampleSizeForSpecifiedRange			= 0;
					ulRangesCounter++;
				}
			}

			sprintf (pBuffer,
				"Real average bit rate of the track (ID: %lu): %lf kbps\n",
				ulTrackIdentifier,
				(double) ((double) ullBitstreamSize /
				dMdhdDuration *
				8.0 / 1000.0));

			if ((errGeneric = FileIO:: writeChars (iFdTracksInfo,
				pBuffer, strlen (pBuffer), &llBytesWritten)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_WRITEBYTES_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				FileIO:: close (iFdTracksInfo);
				if ((errGeneric = mp4File. finish ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					std:: cerr << ((const char *) errGeneric) << std:: endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					std:: cerr << (const char *) errGeneric << std:: endl;

				return 1;
			}


			FileIO:: close (iFdBitstream);
		}

		FileIO:: close (iFdTracksInfo);
	}

	std:: cout << std:: endl;

	if ((errGeneric = mp4File. finish ()) != errNoError)
	{
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		errGeneric = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_FINISH_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			std:: cerr << ((const char *) errGeneric) << std:: endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		return 1;
	}

	if ((errGeneric = gtTracer. cancel ()) != errNoError)
	{
		std:: cerr << (const char *) errGeneric << std:: endl;

		errGeneric = TracerErrors (__FILE__, __LINE__,
			TRACER_TRACER_CANCEL_FAILED);
		std:: cerr << (const char *) errGeneric << std:: endl;

		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		return 1;
	}

	if ((errGeneric = gtTracer. finish (true)) != errNoError)
	{
		std:: cerr << (const char *) errGeneric << std:: endl;

		return 1;
	}


	return 0;
}

