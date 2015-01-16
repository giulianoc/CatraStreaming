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

#include "StreamingServerService.h"
#include "stdlib.h"

#include "FileIO.h"
#include <sys/types.h>
#include <sys/stat.h>

#define TOOLS_SERVICE_DEBUGFILE		"./debugFile.log"

int main (int iArgc, char *pArgv [])

{

	Error_t								errService;
	StreamingServerService_t			sStreamingServerService;
	unsigned long		ulReservedArgumentsNumberFound;


	if (iArgc > 2 ||
		iArgc == 2 && !strcmp (pArgv [1], "-h") ||
		iArgc == 2 && !strcmp (pArgv [1], "--help"))
	{
		std:: cerr << "Usage: " << pArgv [0]
			<< " [ -i | -u | -h | -start | -stop | -nodaemon ]"
			<< std:: endl;

		return 1;
	}


	#ifdef WIN32
	#else
		if (iArgc == 1)		// no parameter
		{
			if ((errService = Service:: launchUnixDaemon (
				SS_STREAMINGSERVERSERVICE_PIDFILEPATHNAME)) != errNoError)
			{
				std:: cerr << (const char *) errService << std:: endl;

				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_SERVICE_LAUNCHUNIXDAEMON_FAILED);
				std:: cerr << (const char *) err << std:: endl;

				return 1;
			}
		}
	#endif

	if ((errService = sStreamingServerService. init ()) != errNoError)
	{
		std:: cerr << (const char *) errService << std:: endl;
		FileIO:: appendBuffer (TOOLS_SERVICE_DEBUGFILE,
			(const char *) errService, true);

		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_SERVICE_INIT_FAILED);
		FileIO:: appendBuffer (TOOLS_SERVICE_DEBUGFILE,
			(const char *) err, true);

		return 1;
	}

	if ((errService = sStreamingServerService. parseArguments (
		iArgc, pArgv, &ulReservedArgumentsNumberFound)) != errNoError)
	{
		std:: cerr << (const char *) errService << std:: endl;
		FileIO:: appendBuffer (TOOLS_SERVICE_DEBUGFILE,
			(const char *) errService, true);

		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_SERVICE_PARSEARGUMENTS_FAILED);
		std:: cerr << (const char *) err << std:: endl;
		FileIO:: appendBuffer (TOOLS_SERVICE_DEBUGFILE,
			(const char *) err, true);

		if (sStreamingServerService. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_SERVICE_FINISH_FAILED);
			std:: cerr << (const char *) err << std:: endl;
		}

		return 1;
	}

	if (ulReservedArgumentsNumberFound == 0)
	{
		if (sStreamingServerService. start (iArgc, pArgv) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_SERVICE_START_FAILED);
			std:: cerr << (const char *) err << std:: endl;
			FileIO:: appendBuffer (TOOLS_SERVICE_DEBUGFILE,
				(const char *) err, true);

			if (sStreamingServerService. finish () != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_SERVICE_FINISH_FAILED);
				std:: cerr << (const char *) err << std:: endl;
			}

			return 1;
		}
	}

	if (sStreamingServerService. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_SERVICE_FINISH_FAILED);
		std:: cerr << (const char *) err << std:: endl;
		FileIO:: appendBuffer (TOOLS_SERVICE_DEBUGFILE,
			(const char *) err, true);

		return 1;
	}


	return 0;
}

