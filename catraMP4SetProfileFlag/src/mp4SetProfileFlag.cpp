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
#include "MP4DecoderSpecificDescr.h"
#include "MP4InitialObjectDescr.h"
#include "MP4File.h"
#include <iostream.h>
#include <stdlib.h>
#ifdef WIN32
#else
	#include <unistd.h>
#endif
#include <errno.h>
#include <stdio.h>


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
	unsigned long						ulVisualProfileLevelIndication;
	MP4Atom:: Standard_t				sStandard;


	{
		char		*pCopyright = "\n***************************************************************************\ncopyright            : (C) by Giuliano Catrambone\nemail                : giuliano.catrambone@tin.it\n***************************************************************************\n";

		cout << pCopyright << endl;
	}

	if (argc < 3 || argc > 5)
	{
		cout << "Usage: " << argv [0]
			<< endl << "\t\t"
			<< "<MP4 path name>"
			<< endl << "\t\t"
			<< "<VisualProfileLevelIndication. (3GPP: 8 for level 0)>"
			<< endl << "\t\t"
			<< "<trace level 0..6 (0 -> all, 6 -> fatal error)>. Default: 6"
			<< endl << "\t\t"
			"<consistencyCheck: 0, 1>. Default: 0"
			<< endl;

		return 1;
	}

	pPathName								= argv [1];
	ulVisualProfileLevelIndication			= atol (argv [2]);

	if (argc == 3)
	{
		lTraceLevel					= 6;
		bUseMP4ConsistencyCheck		= false;
	}
	else if (argc == 4)
	{
		lTraceLevel					= atol (argv [3]);
		bUseMP4ConsistencyCheck		= false;
	}
	else if (argc == 5)
	{
		lTraceLevel					= atol (argv [3]);
		bUseMP4ConsistencyCheck		= argv [4][0] == '0' ? false : true;
	}
	else
	{
		lTraceLevel					= atol (argv [3]);
		bUseMP4ConsistencyCheck		= argv [4][0] == '0' ? false : true;
	}

	lSecondsBetweenTwoCheckTraceToManage		= 7;

	if ((errGeneric = gtTracer. init (
		"MP4SetProfileFlag",					// pName
		10,								// lCacheSizeOfTraceFile K-byte
		"./trace/MP4SetProfileFlag",	// pBaseTraceFileName
		1000,							// lMaxTraceFileSize K-byte
		false,							// bCompressedTraceFile
		5,								// lTraceFilesNumberToMaintain
		true,							// bTraceOnFile
		true,							// bTraceOnTTY
		lTraceLevel,					// lTraceLevel
		lSecondsBetweenTwoCheckTraceToManage,
		3000,							// lMaxTracesNumber
		7531,							// lListenPort
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
	bToBeModified					= false;

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
		bUse64Bits, bUseMP4ConsistencyCheck, bToBeModified,
		sStandard, &gtTracer)) !=
		errNoError)
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

	{
		MP4DecoderSpecificDescr_p		pmaMediaDecoderSpecificDescr;
		// MP4InitialObjectDescr_p			pmaInitialObjectDescr;
		unsigned char					*pucESConfiguration;
		unsigned long					ulESConfigurationSize;
		std:: vector<MP4Utility:: MP4TrackInfo_t>:: const_iterator	it;
		MP4Utility:: MP4TrackInfo_t		mtiMP4TrackInfo;
		vector<MP4Utility:: MP4TrackInfo_t>		vMP4TracksInfo;



		if ((errGeneric = mp4File. getTracksInfo (vMP4TracksInfo)) !=
			errNoError)
		{
			gtTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			errGeneric = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4FILE_GETTRACKSINFO_FAILED);
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

		for (it = vMP4TracksInfo. begin (); it != vMP4TracksInfo. end (); ++it)
		{
			mtiMP4TrackInfo				= *it;

			if (strcmp (mtiMP4TrackInfo. _pCodecUsed, "MPEG4"))
				continue;

			#ifdef WIN32
				MP4Atom_p		pmaForWindows;
				if ((errGeneric = (mtiMP4TrackInfo. _pmaTrakAtom) -> searchAtom (
					"mdia:0:minf:0:stbl:0:stsd:0:mp4v:0:esds:0:ESDescr:0:DecoderConfigDescr:0:DecoderSpecificDescr:0",
					true, &pmaForWindows)) !=
					errNoError)
			#else
				if ((errGeneric = (mtiMP4TrackInfo. _pmaTrakAtom) -> searchAtom (
					"mdia:0:minf:0:stbl:0:stsd:0:mp4v:0:esds:0:ESDescr:0:DecoderConfigDescr:0:DecoderSpecificDescr:0",
					true, &((MP4Atom_p) pmaMediaDecoderSpecificDescr))) !=
					errNoError)
			#endif
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED,
					1, "mdia:0:minf:0:stbl:0:stsz:0");
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if ((errGeneric = mp4File. finish ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return err;
			}
			#ifdef WIN32
				else
					pmaMediaDecoderSpecificDescr		=
						(MP4DecoderSpecificDescr_p) pmaForWindows;
			#else
			#endif

			if ((errGeneric = pmaMediaDecoderSpecificDescr -> getPointerToInfo (
				&pucESConfiguration, &ulESConfigurationSize)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if ((errGeneric = mp4File. finish ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return err;
			}

			pucESConfiguration [4]			= ulVisualProfileLevelIndication;

			break;

			/*
			if ((errGeneric = (mtiMP4TrackInfo. _pmaTrakAtom) -> searchAtom (
				"moov:0:iods:0:InitialObjectDescr:0",
				true, &((MP4Atom_p) pmaInitialObjectDescr))) !=
				errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED,
					1, "mdia:0:minf:0:stbl:0:stsz:0");
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if ((errGeneric = mp4File. finish ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return err;
			}

			if ((errGeneric =
				pmaInitialObjectDescr -> setVisualProfileLevelIndication (
				0)) != errNoError)
			{
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4INITIALOBJECTDESCR_SETVISUALPROFILELEVELINDICATION_FAILED);
				gtTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if ((errGeneric = mp4File. finish ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = gtTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = gtTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return err;
			}
			*/
		}
		
		if (it == vMP4TracksInfo. end ())
		{
			cerr << "No MPEG4 tracks" << endl;

			if ((errGeneric = gtTracer. cancel ()) != errNoError)
				cerr << ((const char *) errGeneric) << endl;
			if ((errGeneric = gtTracer. finish (true)) != errNoError)
				cerr << (const char *) errGeneric << endl;

			return 1;
		}
	}

	// cout << mp4File << endl;

	if ((errGeneric = mp4File. write ()) != errNoError)
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
