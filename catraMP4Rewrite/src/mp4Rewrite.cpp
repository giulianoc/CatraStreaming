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

	const char			*pSrcMP4PathName;
	const char			*pDestMP4PathName;
	Tracer_t			gtTracer;
	long				lSecondsBetweenTwoCheckTraceToManage;
	MP4File_t			mp4File;
	Error_t				errInit;
	Boolean_t			bUse64Bits;
	Boolean_t			bUseMP4ConsistencyCheck;
	Boolean_t			bToBeModified;
	long				lTraceLevel;


	if (argc != 5)
	{
		cout << "Usage: " << argv [0] << " <consistencyCheck: [0 | 1]> <trace level [0 | 6] (0 -> all, 6 -> fatal error)> <Src MP4 path name> <Dest MP4 path name>" << endl;

		return 1;
	}

	bUseMP4ConsistencyCheck		= argv [1][0] == '0' ? false : true;
	lTraceLevel					= atol (argv [2]);
	pSrcMP4PathName				= argv [3];
	pDestMP4PathName			= argv [4];

	lSecondsBetweenTwoCheckTraceToManage		= 7;

	if (gtTracer. init (
		"MP4Rewrite",					// pName
		10,								// lCacheSizeOfTraceFile K-byte
		"./MP4Rewrite",					// pBaseTraceFileName
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

	if ((errInit = mp4File. init (pSrcMP4PathName,
		bUse64Bits, bUseMP4ConsistencyCheck, bToBeModified, &gtTracer)) !=
		errNoError)
	{
		cerr << (const char *) errInit << endl;
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

