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
#include <iostream.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>


int main (int argc, char **argv)

{

	const char							*pSrcMP4PathName;
	const char							*pDestMP4PathName;
	Tracer_t							gtTracer;
	long								lSecondsBetweenTwoCheckTraceToManage;
	MP4File_t							mp4File;
	Error_t								errInit;
	Boolean_t							bUse64Bits;
	Boolean_t							bUseMP4FileConsistencyCheck;
	Boolean_t							bToBeModified;
	long								lTraceLevel;
	const char							*pParentAtomPath;
	MP4Atom_p							pmaParentAtom;
	const char							*pAtomName;
	MP4Atom_p							pmaAtomAdded;
	MP4Atom:: Standard_t				sStandard;


	if (argc < 5 || argc > 7)
	{
		cout << "Usage: " << argv [0]
			<< endl << "\t\t"
			<< "<Src MP4 path name>"
			<< endl << "\t\t"
			<< "<Dest MP4 path name>"
			<< endl << "\t\t"
	<< "<Parent atom path> (ie: \"moov:0:trak:2\", put \"\" in case of root)"
			<< endl << "\t\t"
			<< "<Atom name> (ie: stss)"
			<< endl << "\t\t"
			<< "<trace level 0..6 (0 -> all, 6 -> fatal error)>. Default: 6"
			<< endl << "\t\t"
			"<MP4FileConsistencyCheck: 0, 1>. Default: 0"
			<< endl;

		return 1;
	}

	pSrcMP4PathName					= argv [1];
	pDestMP4PathName				= argv [2];
	pParentAtomPath					= argv [3];
	pAtomName						= argv [4];

	if (argc == 5)
	{
		lTraceLevel						= 6;
		bUseMP4FileConsistencyCheck		= false;
	}
	else if (argc == 6)
	{
		lTraceLevel						= atol (argv [5]);
		bUseMP4FileConsistencyCheck		= false;
	}
	else if (argc == 7)
	{
		lTraceLevel						= atol (argv [5]);
		bUseMP4FileConsistencyCheck		= argv [6][0] == '0' ? false : true;
	}
	else
	{
		lTraceLevel						= atol (argv [5]);
		bUseMP4FileConsistencyCheck		= argv [6][0] == '0' ? false : true;
	}

	lSecondsBetweenTwoCheckTraceToManage		= 7;

	if (gtTracer. init (
		"mp4AddAtom",					// pName
		10,								// lCacheSizeOfTraceFile K-byte
		"./trace/mp4AddAtom",					// pBaseTraceFileName
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
		1000) != errNoError)			// lSizeOfEachBlockToGzip K-byte
	{
		cerr << "Tracer. init failed" << endl;

		return 1;
	}

	if (gtTracer. start () != errNoError)
	{
		cerr << "Tracer. start failed" << endl;

		gtTracer. finish (true);

		return 1;
	}

	bUse64Bits						= false;
	bToBeModified					= false;

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

	if ((errInit = mp4File. init (pSrcMP4PathName,
		bUse64Bits, bUseMP4FileConsistencyCheck, bToBeModified,
		sStandard, &gtTracer)) != errNoError)
	{
		cerr << (const char *) errInit << endl;
		gtTracer. cancel ();
		gtTracer. finish (true);

		return 1;
	}

	if (!strcmp (pParentAtomPath, ""))
	{
		MP4RootAtom_p			pmaRootAtom;


		if (mp4File. getRootAtom (&pmaRootAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_GETROOTATOM_FAILED);
			cerr << (const char *) err << endl;

			mp4File. finish ();
			gtTracer. cancel ();
			gtTracer. finish (true);

			return 1;
		}

		pmaParentAtom				= pmaRootAtom;
	}
	else
	{
		if (mp4File. searchAtom (pParentAtomPath,
			false, &pmaParentAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_SEARCHATOM_FAILED, 1, pParentAtomPath);
			cerr << (const char *) err << endl;

			mp4File. finish ();
			gtTracer. cancel ();
			gtTracer. finish (true);

			return 1;
		}
	}

	if (pmaParentAtom -> addChild (pAtomName, &pmaAtomAdded, -1) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_ADDCHILD_FAILED);
		cerr << (const char *) err << endl;

		mp4File. finish ();
		gtTracer. cancel ();
		gtTracer. finish (true);

		return 1;
	}

	// cout << mp4File << endl;

	mp4File. write (pDestMP4PathName);

	if (mp4File. finish () != errNoError)
	{
		cerr << "mp4File. finish failed" << endl;

		gtTracer. cancel ();
		gtTracer. finish (true);

		return 1;
	}

	if (gtTracer. cancel () != errNoError)
	{
		cout << "Tracer. cancel failed" << endl;
		gtTracer. finish (true);

		return 1;
	}

	if (gtTracer. finish (true) != errNoError)
	{
		cout << "Tracer. finish failed" << endl;

		return 1;
	}


	return 0;
}

