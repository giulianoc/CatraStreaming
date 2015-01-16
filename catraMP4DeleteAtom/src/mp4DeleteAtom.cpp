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
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream.h>
#include <stdlib.h>
#ifdef WIN32
	#include <direct.h>
#else
	#include <unistd.h>
#endif
#include <errno.h>
#include <stdio.h>


int main (int argc, char **argv)

{

	const char							*pSrcMP4PathName;
	const char							*pDestMP4PathName;
	Tracer_t							gtTracer;
	long								lSecondsBetweenTwoCheckTraceToManage;
	MP4File_t							mp4File;
	Error_t								errGeneric;
	Boolean_t							bUse64Bits;
	Boolean_t							bUseMP4FileConsistencyCheck;
	Boolean_t							bToBeModified;
	long								lTraceLevel;
	const char							*pAtomPathToDelete;
	MP4Atom_p							pmaAtomToDelete;
	MP4Atom_p							pmaParentAtomToDelete;
	MP4Atom:: Standard_t				sStandard;


	{
		char		*pCopyright = "\n***************************************************************************\ncopyright            : (C) by Giuliano Catrambone\nemail                : giuliano.catrambone@tin.it\n***************************************************************************\n";

		cout << pCopyright << endl;
	}

	if (argc < 4 || argc > 6)
	{
		cout << "Usage: " << argv [0]
			<< endl << "\t\t"
			<< "<Src MP4 path name>"
			<< endl << "\t\t"
			<< "<Dest MP4 path name>"
			<< endl << "\t\t"
			<< "<Atom path to delete> (ie: moov:0:trak:2)"
			<< endl << "\t\t"
			<< "<trace level 0..6 (0 -> all, 6 -> fatal error)>. Default: 6"
			<< endl << "\t\t"
			"<MP4FileConsistencyCheck: 0, 1>. Default: 0"
			<< endl;

		return 1;
	}

	pSrcMP4PathName					= argv [1];
	pDestMP4PathName				= argv [2];
	pAtomPathToDelete				= argv [3];

	if (argc == 4)
	{
		lTraceLevel						= 6;
		bUseMP4FileConsistencyCheck		= false;
	}
	else if (argc == 5)
	{
		lTraceLevel						= atol (argv [4]);
		bUseMP4FileConsistencyCheck		= false;
	}
	else if (argc == 6)
	{
		lTraceLevel						= atol (argv [4]);
		bUseMP4FileConsistencyCheck		= argv [5][0] == '0' ? false : true;
	}
	else
	{
		lTraceLevel						= atol (argv [4]);
		bUseMP4FileConsistencyCheck		= argv [5][0] == '0' ? false : true;
	}

	lSecondsBetweenTwoCheckTraceToManage		= 7;

	// I will not handle the error in case the directory already exists
	#ifdef WIN32
		_mkdir ("./trace");
	#else
		mkdir ("./trace",
			S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	#endif

	if ((errGeneric = gtTracer. init (
		"mp4DeleteAtom",					// pName
		10,								// lCacheSizeOfTraceFile K-byte
		"./trace/mp4DeleteAtom",		// pBaseTraceFileName
		1000,							// lMaxTraceFileSize K-byte
		false,							// bCompressedTraceFile
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
		cerr << (const char *) errGeneric << endl;

		errGeneric = TracerErrors (__FILE__, __LINE__,
			TRACER_TRACER_INIT_FAILED);
		cerr << (const char *) errGeneric << endl;

		return 1;
	}

	if ((errGeneric = gtTracer. start ()) != errNoError)
	{
		cerr << (const char *) errGeneric << endl;

		errGeneric = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_START_FAILED);
		cerr << (const char *) errGeneric << endl;

		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			cerr << (const char *) errGeneric << endl;

		return 1;
	}

	bUse64Bits						= false;
	bToBeModified					= true;

	{
		unsigned long				ulFileNameLength;


		ulFileNameLength				= strlen (pSrcMP4PathName);

		if (tolower (pSrcMP4PathName [ulFileNameLength - 1]) == 'p' &&
			tolower (pSrcMP4PathName [ulFileNameLength - 2]) == 'g' &&
			pSrcMP4PathName [ulFileNameLength - 3] == '3')
		{
			sStandard			= MP4Atom:: MP4F_3GPP;
		}
		else
		{
			sStandard			= MP4Atom:: MP4F_ISMA;
		}
	}

	if ((errGeneric = mp4File. init (pSrcMP4PathName,
		bUse64Bits, bUseMP4FileConsistencyCheck, bToBeModified, sStandard,
		&gtTracer)) != errNoError)
	{
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		errGeneric = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_INIT_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			cerr << (const char *) errGeneric << endl;

		return 1;
	}

	if ((errGeneric = mp4File. searchAtom (pAtomPathToDelete,
		false, &pmaAtomToDelete)) != errNoError)
	{
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		errGeneric = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_SEARCHATOM_FAILED, 1, pAtomPathToDelete);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = mp4File. finish ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			cerr << (const char *) errGeneric << endl;

		return 1;
	}

	if ((errGeneric = pmaAtomToDelete -> getParent (&pmaParentAtomToDelete)) !=
		errNoError)
	{
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		errGeneric = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_GETPARENT_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = mp4File. finish ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			cerr << (const char *) errGeneric << endl;

		return 1;
	}

	if (pmaParentAtomToDelete == (MP4Atom_p) NULL)
	{
		errGeneric = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = mp4File. finish ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			cerr << (const char *) errGeneric << endl;

		return 1;
	}

	if ((errGeneric = pmaParentAtomToDelete -> deleteChild (pmaAtomToDelete)) !=
		errNoError)
	{
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		errGeneric = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_DELETECHILD_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = mp4File. finish ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			cerr << (const char *) errGeneric << endl;

		return 1;
	}

	// cout << mp4File << endl;

	if ((errGeneric = mp4File. write (pDestMP4PathName)) !=
		errNoError)
	{
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		errGeneric = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_DELETECHILD_FAILED);
		gtTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = mp4File. finish ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = gtTracer. cancel ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			cerr << (const char *) errGeneric << endl;

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
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			cerr << (const char *) errGeneric << endl;

		return 1;
	}

	if ((errGeneric = gtTracer. cancel ()) != errNoError)
	{
		cerr << (const char *) errGeneric << endl;

		errGeneric = TracerErrors (__FILE__, __LINE__,
			TRACER_TRACER_CANCEL_FAILED);
		cerr << (const char *) errGeneric << endl;

		if ((errGeneric = gtTracer. finish (true)) != errNoError)
			cerr << (const char *) errGeneric << endl;

		return 1;
	}

	if ((errGeneric = gtTracer. finish (true)) != errNoError)
	{
		cerr << (const char *) errGeneric << endl;

		return 1;
	}


	return 0;
}

