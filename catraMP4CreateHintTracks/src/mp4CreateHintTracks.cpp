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
#include "RTPUtility.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#ifdef WIN32
	#include <direct.h>
#else
	#include <unistd.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <iostream>


int main (int argc, char **argv)

{

	const char							*pPathName;
	const char							*pHintedPathName;
	Tracer_t							gtTracer;
	long								lSecondsBetweenTwoCheckTraceToManage;
	MP4File_t							mp4File;
	Error_t								errGeneric;
	Boolean_t							bUse64Bits;
	Boolean_t							bUseMP4ConsistencyCheck;
	Boolean_t							bToBeModified;
	long								lTraceLevel;
	MP4Atom:: Standard_t				sStandard;
	Boolean_t							bTraceOnTTY;
	unsigned long						ulMaxPayloadSize;
	unsigned long						ulFileCacheSizeInBytes;


	{
		char		*pCopyright = "\n***************************************************************************\ncopyright            : (C) by CatraSoftware\nemail                : catrastreaming-support@catrasoftware.it\n***************************************************************************\n";

		std:: cout << pCopyright << std:: endl;
	}

	if (argc < 3 || argc > 8)
	{
		std:: cout << "Usage: " << argv [0]
			<< std:: endl << "\t\t"
			<< "<MP4 path name>"
			<< std:: endl << "\t\t"
			<< "<Hinted file name>"
			<< std:: endl << "\t\t"
			<< "<Standard: '3GPP' or 'ISMA'. Default: 3GPP>"
			<< std:: endl << "\t\t"
			<< "<Max payload size>. Default: 1472"
			<< std:: endl << "\t\t"
			<< "<trace level 0..6 (0 -> all, 6 -> fatal error)>. Default: 6"
			<< std:: endl << "\t\t"
			"<consistencyCheck: 0, 1>. Default: 0"
			<< std:: endl << "\t\t"
			<< "<Trace on the standard output 0 (false), 1 (true)>. Default: 1"
			<< std:: endl;

		return 1;
	}

	pPathName					= argv [1];
	pHintedPathName				= argv [2];

	if (argc == 3)
	{
		sStandard					= MP4Atom:: MP4F_3GPP;
		ulMaxPayloadSize			= 1472;
		lTraceLevel					= 6;
		bUseMP4ConsistencyCheck		= false;
		bTraceOnTTY					= true;
	}
	else if (argc == 4)
	{
		if (!strcmp (argv [3], "3GPP"))
			sStandard					= MP4Atom:: MP4F_3GPP;
		else if (!strcmp (argv [3], "ISMA"))
			sStandard					= MP4Atom:: MP4F_ISMA;
		else
			sStandard					= MP4Atom:: MP4F_3GPP;
		ulMaxPayloadSize			= 1472;
		lTraceLevel					= 6;
		bUseMP4ConsistencyCheck		= false;
		bTraceOnTTY					= true;
	}
	else if (argc == 5)
	{
		if (!strcmp (argv [3], "3GPP"))
			sStandard					= MP4Atom:: MP4F_3GPP;
		else if (!strcmp (argv [3], "ISMA"))
			sStandard					= MP4Atom:: MP4F_ISMA;
		else
			sStandard					= MP4Atom:: MP4F_3GPP;
		ulMaxPayloadSize			= atol (argv [4]);
		lTraceLevel					= 6;
		bUseMP4ConsistencyCheck		= false;
		bTraceOnTTY					= true;
	}
	else if (argc == 6)
	{
		if (!strcmp (argv [3], "3GPP"))
			sStandard					= MP4Atom:: MP4F_3GPP;
		else if (!strcmp (argv [3], "ISMA"))
			sStandard					= MP4Atom:: MP4F_ISMA;
		else
			sStandard					= MP4Atom:: MP4F_3GPP;
		ulMaxPayloadSize			= atol (argv [4]);
		lTraceLevel					= atol (argv [5]);
		bUseMP4ConsistencyCheck		= false;
		bTraceOnTTY					= true;
	}
	else if (argc == 7)
	{
		if (!strcmp (argv [3], "3GPP"))
			sStandard					= MP4Atom:: MP4F_3GPP;
		else if (!strcmp (argv [3], "ISMA"))
			sStandard					= MP4Atom:: MP4F_ISMA;
		else
			sStandard					= MP4Atom:: MP4F_3GPP;
		ulMaxPayloadSize			= atol (argv [4]);
		lTraceLevel					= atol (argv [5]);
		bUseMP4ConsistencyCheck		= argv [6][0] == '0' ? false : true;
		bTraceOnTTY					= true;
	}
	else if (argc == 8)
	{
		if (!strcmp (argv [3], "3GPP"))
			sStandard					= MP4Atom:: MP4F_3GPP;
		else if (!strcmp (argv [3], "ISMA"))
			sStandard					= MP4Atom:: MP4F_ISMA;
		else
			sStandard					= MP4Atom:: MP4F_3GPP;
		ulMaxPayloadSize			= atol (argv [4]);
		lTraceLevel					= atol (argv [5]);
		bUseMP4ConsistencyCheck		= argv [6][0] == '0' ? false : true;
		bTraceOnTTY					= argv [7][0] == '0' ? false : true;
	}
	else			// if (argc == 9)
	{
		if (!strcmp (argv [3], "3GPP"))
			sStandard					= MP4Atom:: MP4F_3GPP;
		else if (!strcmp (argv [3], "ISMA"))
			sStandard					= MP4Atom:: MP4F_ISMA;
		else
			sStandard					= MP4Atom:: MP4F_3GPP;
		ulMaxPayloadSize			= atol (argv [4]);
		lTraceLevel					= atol (argv [5]);
		bUseMP4ConsistencyCheck		= argv [6][0] == '0' ? false : true;
		bTraceOnTTY					= argv [7][0] == '0' ? false : true;
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
		"Mp4CreateHintTracks",			// pName
		10,								// lCacheSizeOfTraceFile K-byte
		"./trace",						// pBaseTraceFileName
		"@MM@_@DD@_@HI24@_Mp4CreateHintTracks",		// pBaseTraceFileName
		1000,							// lMaxTraceFileSize K-byte
        3600,							// lTraceFilePeriodInSecs
		false,							// bCompressedTraceFile
		false,							// bClosedFileToBeCopied
		(const char *) NULL,			// pClosedFilesRepository
		5,								// lTraceFilesNumberToMaintain
		true,							// bTraceOnFile
		bTraceOnTTY,					// bTraceOnTTY
		lTraceLevel,					// lTraceLevel
		lSecondsBetweenTwoCheckTraceToManage,
		3000,							// lMaxTracesNumber
		6532,							// lListenPort
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
	bToBeModified					= true;
	ulFileCacheSizeInBytes			= 1000 * 1024;

	if ((errGeneric = mp4File. init (pPathName,
		bUse64Bits, bUseMP4ConsistencyCheck, bToBeModified,
		ulFileCacheSizeInBytes, sStandard, &gtTracer)) != errNoError)
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

	if ((errGeneric = RTPUtility:: createAllHintTracks (
		&mp4File, ulMaxPayloadSize, &gtTracer)) !=
		errNoError)
	{
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		errGeneric = RTPErrors (__FILE__, __LINE__,
			RTP_RTPUTILITY_CREATEALLHINTTRACKS_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = mp4File. finish ()) != errNoError)
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			std:: cerr << ((const char *) errGeneric) << std:: endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		return 1;
	}

	std:: cout << mp4File << std:: endl;

	if ((errGeneric = mp4File. write (pHintedPathName)) !=
		errNoError)
	{
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		errGeneric = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_WRITE_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = mp4File. finish ()) != errNoError)
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			std:: cerr << ((const char *) errGeneric) << std:: endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			std:: cerr << (const char *) errGeneric << std:: endl;

		return 1;
	}

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

