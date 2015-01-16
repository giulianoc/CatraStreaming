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

#include <stdlib.h>
#include <time.h>
#include <iostream>
#include "Tracer.h"
#include "SDPFor3GPP.h"
#include "SDPMediaFor3GPP.h"


int main (int iArgc, char **pArgv)

{

	Tracer_t				tTracer;
	SDPFor3GPP_t			sSDPFor3GPP;
	SDPMedia_p				psmSDPMedia;
	Error_t					errGetSDPMedia;
	Buffer_t				bSDPFor3GPP;
	const char				*pSDPPathName;


	{
		char		*pCopyright = "\n***************************************************************************\ncopyright            : (C) by CatraSoftware\nemail                : catrasoftware-support@catrasoftware.it\n***************************************************************************\n";

		std:: cout << pCopyright << std:: endl;
	}

	if (iArgc != 2)
	{
		std:: cout << "Usage: " << pArgv [0] << " <SDP path name>"
			<< std:: endl;

		return 1;
	}

	pSDPPathName				= pArgv [1];


	if (tTracer. init (
		"Test",							// pName
		10,								// lCacheSizeOfTraceFile K-byte
		"./",							// pBaseTraceFileName
		"trace",						// pBaseTraceFileName
		100,							// lMaxTraceFileSize K-byte
		100,							// lTraceFilePeriodInSecs
		false,							// bCompressedTraceFile
		false,							// bClosedFileToBeCopied
		(const char *) NULL,			// pClosedFilesRepository
		10,								// lTraceFilesNumberToMaintain
		true,							// bTraceOnFile
		true,							// bTraceOnTTY
		0,								// lTraceLevel
		7,								// lSecondsBetweenTwoCheckTraceToManage
		3000,                           // lMaxTracesNumber
		9999,							// lListenPort
		1000,                           // lTracesNumberAllocatedOnOverflow
		1000) != errNoError)            // lSizeOfEachBlockToGzip K-byte
	{
		Error err = TracerErrors (__FILE__, __LINE__,
			TRACER_TRACER_INIT_FAILED);
		std:: cerr << (const char *) err << std:: endl;

		return 1;
	}

	if (tTracer. start () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_START_FAILED);
		std:: cerr << (const char *) err << std:: endl;

		if (tTracer. finish (true) != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_FINISH_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}

		return 1;
	}

	if (bSDPFor3GPP. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);   
		tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (tTracer. cancel () != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_CANCEL_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}

		if (tTracer. finish (true) != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_FINISH_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}

		return 1;
	}

	if (sSDPFor3GPP. init (pSDPPathName, &tTracer) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPFOR3GPP_INIT_FAILED);   
		tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bSDPFor3GPP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);   
			tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (tTracer. cancel () != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_CANCEL_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}

		if (tTracer. finish (true) != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_FINISH_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}

		return 1;
	}

	if ((errGetSDPMedia = sSDPFor3GPP. getSDPMedia (SDPMedia:: SDPMEDIA_VIDEO,
		0, &psmSDPMedia)) != errNoError)
	{
		if ((long) errGetSDPMedia != SDP_SDP_SDPMEDIANOTFOUND)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_GETSDPMEDIA_FAILED);   
			tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);   
				tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (bSDPFor3GPP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);   
				tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (tTracer. cancel () != errNoError)
			{
				Error err = TracerErrors (__FILE__, __LINE__,
					TRACER_TRACER_CANCEL_FAILED);
				std:: cerr << (const char *) err << std:: endl;
			}

			if (tTracer. finish (true) != errNoError)
			{
				Error err = TracerErrors (__FILE__, __LINE__,
					TRACER_TRACER_FINISH_FAILED);
				std:: cerr << (const char *) err << std:: endl;
			}

			return 1;
		}
	}

	if ((long) errGetSDPMedia == SDP_SDP_SDPMEDIANOTFOUND)
		std:: cout << "Video not found" << std:: endl;
	else
	{
		SDPMedia:: SDPMediaType_t	mtMediaType;
		unsigned long			ulPayloadNumber;
		unsigned long			ulRTPPacketsServerPort;
		char					pMediaEncodingName [
			SDP_ENCODINGNAMELENGTH];
		char					pMediaControl [
			SDPFORRTSP_MAXCONTROLLENGTH];
		unsigned long			ulAvgBitRate;


		if (((SDPMediaForRTSP_p) psmSDPMedia) -> getMediaInfo (
			&mtMediaType,
			pMediaControl,
			&ulPayloadNumber,
			&ulRTPPacketsServerPort,
			pMediaEncodingName,
			(char *) NULL,
			(char *) NULL,
			&ulAvgBitRate) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_GETMEDIAINFO_FAILED);
			tTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);   
				tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (bSDPFor3GPP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);   
				tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (tTracer. cancel () != errNoError)
			{
				Error err = TracerErrors (__FILE__, __LINE__,
					TRACER_TRACER_CANCEL_FAILED);
				std:: cerr << (const char *) err << std:: endl;
			}

			if (tTracer. finish (true) != errNoError)
			{
				Error err = TracerErrors (__FILE__, __LINE__,
					TRACER_TRACER_FINISH_FAILED);
				std:: cerr << (const char *) err << std:: endl;
			}

			return 1;
		}

		std:: cout << "Video info: " << std:: endl
			<< "\tpMediaControl: " << pMediaControl
			<< "\tulPayloadNumber: " << ulPayloadNumber
			<< std:: endl
			<< "\tulRTPPacketsServerPort: " << ulRTPPacketsServerPort
			<< std:: endl
			<< "\tpMediaEncodingName: " << pMediaEncodingName
			<< std:: endl
			<< "\tulAvgBitRate: " << ulAvgBitRate
			<< std:: endl;
	}

	if ((errGetSDPMedia = sSDPFor3GPP. getSDPMedia (SDPMedia:: SDPMEDIA_AUDIO,
		0, &psmSDPMedia)) != errNoError)
	{
		if ((long) errGetSDPMedia != SDP_SDP_SDPMEDIANOTFOUND)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPFOR3GPP_GETSDPMEDIA_FAILED);   
			tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);   
				tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (bSDPFor3GPP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);   
				tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (tTracer. cancel () != errNoError)
			{
				Error err = TracerErrors (__FILE__, __LINE__,
					TRACER_TRACER_CANCEL_FAILED);
				std:: cerr << (const char *) err << std:: endl;
			}

			if (tTracer. finish (true) != errNoError)
			{
				Error err = TracerErrors (__FILE__, __LINE__,
					TRACER_TRACER_FINISH_FAILED);
				std:: cerr << (const char *) err << std:: endl;
			}

			return 1;
		}
	}

	if ((long) errGetSDPMedia == SDP_SDP_SDPMEDIANOTFOUND)
		std:: cout << "Audio not found" << std:: endl;
	else
	{
		SDPMedia:: SDPMediaType_t	mtMediaType;
		unsigned long			ulPayloadNumber;
		unsigned long			ulRTPPacketsServerPort;
		char					pMediaEncodingName [
			SDP_ENCODINGNAMELENGTH];
		char					pMediaControl [
			SDPFORRTSP_MAXCONTROLLENGTH];
		unsigned long			ulAvgBitRate;


		if (((SDPMediaForRTSP_p) psmSDPMedia) -> getMediaInfo (
			&mtMediaType,
			pMediaControl,
			&ulPayloadNumber,
			&ulRTPPacketsServerPort,
			pMediaEncodingName,
			(char *) NULL,
			(char *) NULL,
			&ulAvgBitRate) != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDPMEDIA_GETMEDIAINFO_FAILED);
			tTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (sSDPFor3GPP. finish () != errNoError)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SDP_FINISH_FAILED);   
				tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (bSDPFor3GPP. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_FINISH_FAILED);   
				tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);
			}

			if (tTracer. cancel () != errNoError)
			{
				Error err = TracerErrors (__FILE__, __LINE__,
					TRACER_TRACER_CANCEL_FAILED);
				std:: cerr << (const char *) err << std:: endl;
			}

			if (tTracer. finish (true) != errNoError)
			{
				Error err = TracerErrors (__FILE__, __LINE__,
					TRACER_TRACER_FINISH_FAILED);
				std:: cerr << (const char *) err << std:: endl;
			}

			return 1;
		}

		std:: cout << "Audio info: " << std:: endl
			<< "\tpMediaControl: " << pMediaControl
			<< "\tulPayloadNumber: " << ulPayloadNumber
			<< std:: endl
			<< "\tulRTPPacketsServerPort: " << ulRTPPacketsServerPort
			<< std:: endl
			<< "\tpMediaEncodingName: " << pMediaEncodingName
			<< std:: endl
			<< "\tulAvgBitRate: " << ulAvgBitRate
			<< std:: endl;
	}

	if (sSDPFor3GPP. appendToBuffer (&bSDPFor3GPP) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_APPENDTOBUFFER_FAILED);   
		tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (sSDPFor3GPP. finish () != errNoError)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SDP_FINISH_FAILED);   
			tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bSDPFor3GPP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);   
			tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (tTracer. cancel () != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_CANCEL_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}

		if (tTracer. finish (true) != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_FINISH_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}

		return 1;
	}

	std:: cout << (const char *) bSDPFor3GPP << std:: endl;

	if (sSDPFor3GPP. finish () != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDP_FINISH_FAILED);   
		tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bSDPFor3GPP. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);   
			tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (tTracer. cancel () != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_CANCEL_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}

		if (tTracer. finish (true) != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_FINISH_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}

		return 1;
	}

	if (bSDPFor3GPP. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);   
		tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (tTracer. cancel () != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_CANCEL_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}

		if (tTracer. finish (true) != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_FINISH_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}

		return 1;
	}

	if (tTracer. cancel () != errNoError)
	{
		Error err = TracerErrors (__FILE__, __LINE__,
			TRACER_TRACER_CANCEL_FAILED);   
		tTracer. trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (tTracer. finish (true) != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_FINISH_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}

		return 1;
	}
															
	if (tTracer. finish (true) != errNoError)
	{
		Error err = TracerErrors (__FILE__, __LINE__,
			TRACER_TRACER_FINISH_FAILED);   
		std:: cerr << (const char *) err << std:: endl;
																								
		return 1;
	}


	return 0;
}


