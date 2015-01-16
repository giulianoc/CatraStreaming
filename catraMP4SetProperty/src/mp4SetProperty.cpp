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
#include "MP4FtypAtom.h"
#include "Buffer.h"
#include <sys/stat.h>
#include <sys/types.h>
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
	const char							*pAtomType;
	const char							*pAtomPath;
	const char							*pPropertyName;
	const char							*pPropertyValueIndex;
	const char							*pPropertyValue;
	MP4Atom:: Standard_t				sStandard;


	if (argc < 7 || argc > 10)
	{
		cout << "Usage: " << argv [0]
			<< endl << "\t\t"
			<< "<Src MP4 path name>"
			<< endl << "\t\t"
			<< "<Dest MP4 path name>"
			<< endl << "\t\t"
			<< "<Atom type> (ie: ftyp)"
			<< endl << "\t\t"
			<< "<Atom path> (ie: ftyp:0)"
			<< endl << "\t\t"
			<< "<Property name> (ie: Brand)"
			<< endl << "\t\t"
			<< "<Property value index> (ie: 0)"
			<< endl << "\t\t"
			<< "<Property value> (ie: 3gp4)"
			<< endl << "\t\t"
			<< "<trace level 0..6 (0 -> all, 6 -> fatal error)>. Default: 6"
			<< endl << "\t\t"
			"<MP4FileConsistencyCheck: 0, 1>. Default: 0"
			<< endl;

		return 1;
	}

	pSrcMP4PathName					= argv [1];
	pDestMP4PathName				= argv [2];
	pAtomType						= argv [3];
	pAtomPath						= argv [4];
	pPropertyName					= argv [5];
	pPropertyValueIndex				= argv [6];
	pPropertyValue					= argv [7];

	if (argc == 8)
	{
		lTraceLevel						= 6;
		bUseMP4FileConsistencyCheck		= false;
	}
	else if (argc == 9)
	{
		lTraceLevel						= atol (argv [8]);
		bUseMP4FileConsistencyCheck		= false;
	}
	else if (argc == 10)
	{
		lTraceLevel						= atol (argv [8]);
		bUseMP4FileConsistencyCheck		= argv [9][0] == '0' ? false : true;
	}
	else
	{
		lTraceLevel						= atol (argv [8]);
		bUseMP4FileConsistencyCheck		= argv [9][0] == '0' ? false : true;
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

	// I will not handle the error in case the directory already exists
	mkdir ("./trace",
		S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

	if ((errInit = mp4File. init (pSrcMP4PathName,
		bUse64Bits, bUseMP4FileConsistencyCheck, bToBeModified,
		sStandard, &gtTracer)) != errNoError)
	{
		cerr << (const char *) errInit << endl;
		gtTracer. cancel ();
		gtTracer. finish (true);

		return 1;
	}

	if (!strcmp (pAtomType, "ftyp"))
	{
		MP4FtypAtom_p						pmaFtypAtom;

		if (mp4File. searchAtom (pAtomPath,
			false, &((MP4Atom_p) pmaFtypAtom)) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_SEARCHATOM_FAILED, 1, "ftyp:0");
			cerr << (const char *) err << endl;

			mp4File. finish ();
			gtTracer. cancel ();
			gtTracer. finish (true);

			return 1;
		}

		if (!strcmp (pPropertyName, "MajorBrand"))
		{
			Buffer_t			bMajorBrand;

			if (bMajorBrand. init (pPropertyValue) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				cerr << (const char *) err << endl;

				mp4File. finish ();
				gtTracer. cancel ();
				gtTracer. finish (true);

				return 1;
			}

			if (pmaFtypAtom -> setMajorBrand (&bMajorBrand) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FTYPATOM_SETMAJORBRAND_FAILED);
				cerr << (const char *) err << endl;

				bMajorBrand. finish ();
				mp4File. finish ();
				gtTracer. cancel ();
				gtTracer. finish (true);

				return 1;
			}

			if (bMajorBrand. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				cerr << (const char *) err << endl;

				mp4File. finish ();
				gtTracer. cancel ();
				gtTracer. finish (true);

				return 1;
			}
		}
		else if (!strcmp (pPropertyName, "MinorVersion"))
		{
			if (pmaFtypAtom -> setMinorVersion (
				(unsigned long) atol (pPropertyValue)) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FTYPATOM_SETMINORVERSION_FAILED);
				cerr << (const char *) err << endl;

				mp4File. finish ();
				gtTracer. cancel ();
				gtTracer. finish (true);

				return 1;
			}
		}
		else if (!strcmp (pPropertyName, "CompatibleBrandsCount"))
		{
			if (pmaFtypAtom -> setCompatibleBrandsCount (
				(unsigned long) atol (pPropertyValue)) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FTYPATOM_SETCOMPATIBLEBRANDSCOUNT_FAILED);
				cerr << (const char *) err << endl;

				mp4File. finish ();
				gtTracer. cancel ();
				gtTracer. finish (true);

				return 1;
			}
		}
		else if (!strcmp (pPropertyName, "Brand"))
		{
			Buffer_t			bBrand;

			if (bBrand. init (pPropertyValue) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_INIT_FAILED);
				cerr << (const char *) err << endl;

				mp4File. finish ();
				gtTracer. cancel ();
				gtTracer. finish (true);

				return 1;
			}

			if (pmaFtypAtom -> setBrand (
				(unsigned long) atol (pPropertyValueIndex),
				&bBrand) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4FTYPATOM_SETBRAND_FAILED);
				cerr << (const char *) err << endl;

				bBrand. finish ();
				mp4File. finish ();
				gtTracer. cancel ();
				gtTracer. finish (true);

				return 1;
			}

			if (bBrand. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);
				cerr << (const char *) err << endl;

				mp4File. finish ();
				gtTracer. cancel ();
				gtTracer. finish (true);

				return 1;
			}
		}

		mp4File. write (pDestMP4PathName);
	}
	else
		cout << "Not implemented yet" << endl;

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

