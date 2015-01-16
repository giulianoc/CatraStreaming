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

#include "TimesForVidiator.h"
#include "TimesForNexencoder.h"
#include "ConfigurationFile.h"
#include "Tracer.h"
#include "Scheduler.h"
#include "SchedulerForEncoderErrors.h"
#include "SchedulerForEncoderMessages.h"
#include "vector.h"
#include <stdlib.h>
#include <time.h>
#include <iostream.h>


#define SFE_MAXLONGLENGTH							512 + 1
#define SFE_MAXTRACEFILELENGTH						1024 + 1
#define SFE_MAXSECTIONNAMELENGTH					512 + 1
#define SFE_MAXENCODERLENGTH						512 + 1


int main (int iArgc, char **pArgv)

{

	ConfigurationFile_t					cfConfiguration;
	Error_t								errParseError;
	Error_t								errGeneric;
	char								pCacheSizeOfTraceFile [
		SFE_MAXLONGLENGTH];
	char								pBaseTraceFileName [
		SFE_MAXTRACEFILELENGTH];
	char								pMaxTraceFileSize [
		SFE_MAXLONGLENGTH];
	char								pCompressedTraceFile [
		SFE_MAXLONGLENGTH];
	Boolean_t							bCompressedTraceFile;
	char								pTraceOnTTY [
		SFE_MAXLONGLENGTH];
	Boolean_t							bTraceOnTTY;
	char								pTraceLevel [
		SFE_MAXLONGLENGTH];
	long								lTraceLevel;
	char								pListenTracePort [
		SFE_MAXLONGLENGTH];
	Tracer_t							tTracer;
	long								lSecondsBetweenTwoCheckTraceToManage;
	char								pSourcesPath [SFE_MAXPATHLENGTH];
	char								pEncodedPath [SFE_MAXPATHLENGTH];
	char								pProfilesPath [SFE_MAXPATHLENGTH];
	char								pTemporaryPath [SFE_MAXPATHLENGTH];
	Scheduler_t							schScheduler;
	char								pEncoder [SFE_MAXENCODERLENGTH];
	char								pFTPHost [SFE_MAXHOSTLENGTH];
	char								pFTPUser [SFE_MAXUSERLENGTH];
	char								pFTPPassword [SFE_MAXPASSWORDLENGTH];
	char								pFTPRootPath [SFE_MAXPATHLENGTH];
	char								pSchedule [SCH_MAXSCHEDULELENGTH];
	char								pFilesList [SFE_MAXPATHLENGTH];
	vector<Buffer_p>					vEncodingSessions;
	vector<Buffer_p>					vEncodingSessionFTPInfo;
	vector<Times_p>						vEncodingSessionTimes;


	{
		char		*pCopyright = "\n***************************************************************************\ncopyright            : (C) by Giuliano Catrambone\nemail                : giuliano.catrambone@tin.it\n***************************************************************************\n";

		cout << pCopyright << endl;
	}

	if (iArgc < 2)
	{
		cout << "Main functionalities:"
			<< endl << "\t1. Scheduler to call external encoders"
			<< endl << "\t2. FTP protocol to take the sources file"
			<< endl << "\t3. Telnet protocol to take the sources file"
			<< endl
			<< endl << "Remark 1: The tool create a 'trace' directory to put his logs"
			<< endl << endl
			<< "----------------------------------"
			<< endl << endl
			<< "Usage: " << pArgv [0]
			<< endl
			<< "\t<config path name>"
			<< endl << endl
			<< endl << "Example: " << pArgv [0] << " SchedulerForEncoder.cfg"
			<< endl;

		return 1;
	}

	if ((errParseError = cfConfiguration. init (pArgv [1],
		"Scheduler for encoder configuration")) != errNoError)
	{
		cerr << (const char *) errParseError << endl;
		Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			CFGFILE_CONFIGURATIONFILE_INIT_FAILED);
		cerr << (const char *) err << endl;

		return 1;
	}

	if ((errGetItemValue = cfConfiguration. getItemValue (
		"SchedulerForEncoder",
		"SourcesPath", pSourcesPath)) != errNoError)
	{
		cerr << (const char *) errGetItemValue << endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "SchedulerForEncoder", "SourcesPath");
		cerr << (const char *) err << endl;

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}

		return 1;
	}

	if ((errGetItemValue = cfConfiguration. getItemValue (
		"SchedulerForEncoder",
		"EncodedPath", pEncodedPath)) != errNoError)
	{
		cerr << (const char *) errGetItemValue << endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "SchedulerForEncoder", "EncodedPath");
		cerr << (const char *) err << endl;

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}

		return 1;
	}

	if ((errGetItemValue = cfConfiguration. getItemValue (
		"SchedulerForEncoder",
		"ProfilesPath", pProfilesPath)) != errNoError)
	{
		cerr << (const char *) errGetItemValue << endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "SchedulerForEncoder", "ProfilesPath");
		cerr << (const char *) err << endl;

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}

		return 1;
	}

	if ((errGetItemValue = cfConfiguration. getItemValue (
		"SchedulerForEncoder",
		"TemporaryPath", pTemporaryPath)) != errNoError)
	{
		cerr << (const char *) errGetItemValue << endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "SchedulerForEncoder", "TemporaryPath");
		cerr << (const char *) err << endl;

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}

		return 1;
	}

	if ((errGetItemValue = cfConfiguration. getItemValue (
		"SchedulerForEncoderLogs",
		"CacheSizeOfTraceFile", pCacheSizeOfTraceFile)) != errNoError)
	{
		cerr << (const char *) errGetItemValue << endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "SchedulerForEncoderLogs", "CacheSizeOfTraceFile");
		cerr << (const char *) err << endl;

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}

		return 1;
	}

	if ((errGetItemValue = cfConfiguration. getItemValue (
		"SchedulerForEncoderLogs",
		"BaseTraceFileName", pBaseTraceFileName)) != errNoError)
	{
		cerr << (const char *) errGetItemValue << endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "SchedulerForEncoderLogs", "BaseTraceFileName");
		cerr << (const char *) err << endl;

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}

		return 1;
	}

	if ((errGetItemValue = cfConfiguration. getItemValue (
		"SchedulerForEncoderLogs",
		"MaxTraceFileSize", pMaxTraceFileSize)) != errNoError)
	{
		cerr << (const char *) errGetItemValue << endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "SchedulerForEncoderLogs", "MaxTraceFileSize");
		cerr << (const char *) err << endl;

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}

		return 1;
	}

	if ((errGetItemValue = cfConfiguration. getItemValue (
		"SchedulerForEncoderLogs",
		"CompressedTraceFile", pCompressedTraceFile)) != errNoError)
	{
		cerr << (const char *) errGetItemValue << endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "SchedulerForEncoderLogs", "CompressedTraceFile");
		cerr << (const char *) err << endl;

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}

		return 1;
	}

	if (!strcmp (pCompressedTraceFile, "false"))
		bCompressedTraceFile				= false;
	else
		bCompressedTraceFile				= true;

	if ((errGetItemValue = cfConfiguration. getItemValue (
		"SchedulerForEncoderLogs",
		"TraceOnTTY", pTraceOnTTY)) != errNoError)
	{
		cerr << (const char *) errGetItemValue << endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "SchedulerForEncoderLogs", "TraceOnTTY");
		cerr << (const char *) err << endl;

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}

		return 1;
	}

	if (!strcmp (pTraceOnTTY, "true"))
		bTraceOnTTY							= true;
	else
		bTraceOnTTY							= false;

	if ((errGetItemValue = cfConfiguration. getItemValue (
		"SchedulerForEncoderLogs",
		"TraceLevel", pTraceLevel)) != errNoError)
	{
		cerr << (const char *) errGetItemValue << endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "SchedulerForEncoderLogs", "TraceLevel");
		cerr << (const char *) err << endl;

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}

		return 1;
	}

	if (!strcmp (pTraceLevel, "LDBG1"))
		lTraceLevel				= 0;
	else if (!strcmp (pTraceLevel, "LDBG2"))
		lTraceLevel				= 1;
	else if (!strcmp (pTraceLevel, "LDBG3"))
		lTraceLevel				= 2;
	else if (!strcmp (pTraceLevel, "LDBG4"))
		lTraceLevel				= 3;
	else if (!strcmp (pTraceLevel, "LDBG5"))
		lTraceLevel				= 4;
	else if (!strcmp (pTraceLevel, "LDBG6"))
		lTraceLevel				= 5;
	else if (!strcmp (pTraceLevel, "LINFO"))
		lTraceLevel				= 6;
	else if (!strcmp (pTraceLevel, "LMESG"))
		lTraceLevel				= 7;
	else if (!strcmp (pTraceLevel, "LWRNG"))
		lTraceLevel				= 8;
	else if (!strcmp (pTraceLevel, "LERRR"))
		lTraceLevel				= 9;
	else if (!strcmp (pTraceLevel, "LFTAL"))
		lTraceLevel				= 10;
	else
		lTraceLevel				= 6;

	if ((errGetItemValue = cfConfiguration. getItemValue (
		"SchedulerForEncoderLogs",
		"ListenTracePort", pListenTracePort)) != errNoError)
	{
		cerr << (const char *) errGetItemValue << endl;

		Error err = ConfigurationErrors (__FILE__, __LINE__,
			CFG_CONFIG_GETITEMVALUE_FAILED,
			2, "SchedulerForEncoderLogs", "ListenTracePort");
		cerr << (const char *) err << endl;

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}

		return 1;
	}

	lSecondsBetweenTwoCheckTraceToManage		= 7;

	if (tTracer. init (
		"SchedulerForEncoderLogsTracer",// pName
		atol (pCacheSizeOfTraceFile),	// lCacheSizeOfTraceFile K-byte
		pBaseTraceFileName,				// pBaseTraceFileName
		atol (pMaxTraceFileSize),		// lMaxTraceFileSize K-byte
		bCompressedTraceFile,			// bCompressedTraceFile
		20,								// lTraceFilesNumberToMaintain
		true,							// bTraceOnFile
		bTraceOnTTY,					// bTraceOnTTY
		lTraceLevel,					// lTraceLevel
		lSecondsBetweenTwoCheckTraceToManage,
		3000,							// lMaxTracesNumber
		atol (pListenTracePort),		// lListenPort
		1000,							// lTracesNumberAllocatedOnOverflow
		1000) != errNoError)			// lSizeOfEachBlockToGzip K-byte
	{
		Error err = TracerErrors (__FILE__, __LINE__,
			TRACER_TRACER_INIT_FAILED);	
		cerr << (const char *) err << endl;

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}

		return 1;
	}

	if (tTracer. start () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_START_FAILED);	
		cerr << (const char *) err << endl;

		if (tTracer. finish (true) != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_FINISH_FAILED);	
			cerr << (const char *) err << endl;
		}

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}

		return 1;
	}

	if (schScheduler. init () != errNoError)
	{
		errGeneric = SchedulerErrors (__FILE__, __LINE__,
			SCH_SCHEDULER_INIT_FAILED);
		tTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = tTracer. cancel ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = tTracer. finish (true)) != errNoError)
			cerr << (const char *) errGeneric << endl;

		if ((errGeneric = cfConfiguration. finish ()) != errNoError)
			cerr << (const char *) errGeneric << endl;

		return 1;
	}

	if (schScheduler. start () != errNoError)
	{
		errGeneric = SchedulerErrors (__FILE__, __LINE__,
			SCH_SCHEDULER_START_FAILED);
		tTracer. trace (Tracer:: TRACER_LERRR,
			(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = schScheduler. finish (true)) != errNoError)
			tTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = tTracer. cancel ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = tTracer. finish (true)) != errNoError)
			cerr << (const char *) errGeneric << endl;

		if ((errGeneric = cfConfiguration. finish ()) != errNoError)
			cerr << (const char *) errGeneric << endl;

		return 1;
	}

	{
		long					lSectionsNumber;
		long					lSectionIndex;
		ConfigurationSection_t	csCfgSection;
		char					pSectionName [SFE_MAXSECTIONNAMELENGTH];


		if ((errGeneric = cfConfiguration. getSectionsNumber (
			&lSectionsNumber)) != errNoError)
		{
			tTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			errGeneric = TracerErrors (__FILE__, __LINE__,
				CFG_CONFIG_GETSECTIONSNUMBER_FAILED);
			tTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

			if ((errGeneric = schScheduler. cancel ()) != errNoError)
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);
			if ((errGeneric = schScheduler. finish (true)) != errNoError)
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

			if ((errGeneric = tTracer. cancel ()) != errNoError)
				cerr << ((const char *) errGeneric) << endl;
			if ((errGeneric = tTracer. finish (true)) != errNoError)
				cerr << (const char *) errGeneric << endl;

			if ((errGeneric = cfConfiguration. finish ()) != errNoError)
				cerr << (const char *) errGeneric << endl;

			return 1;
		}

		for (lSectionIndex = 0; lSectionIndex < lSectionsNumber; lSectionIndex++)
		{
			if ((errGeneric = cfConfiguration. getCfgSectionByIndex (
				lSectionIndex, &csCfgSection)) != errNoError)
			{
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				errGeneric = TracerErrors (__FILE__, __LINE__,
					CFG_CONFIG_GETCFGSECTIONBYINDEX_FAILED);
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				vEncodingSessionTimes. clear ();

				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
						++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessions. clear ();
				}
				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessionFTPInfo. begin ();
						it != vEncodingSessionFTPInfo. end (); ++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessionFTPInfo. clear ();
				}

				if ((errGeneric = schScheduler. cancel ()) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);
				if ((errGeneric = schScheduler. finish (true)) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

				if ((errGeneric = tTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = tTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				if ((errGeneric = cfConfiguration. finish ()) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return 1;
			}

			if ((errGeneric = csCfgSection. getSectionName (pSectionName)) !=
				errNoError)
			{
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				errGeneric = TracerErrors (__FILE__, __LINE__,
					CFG_CONFIGURATIONSECTION_GETSECTIONNAME_FAILED);
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				vEncodingSessionTimes. clear ();

				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
						++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessions. clear ();
				}
				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessionFTPInfo. begin ();
						it != vEncodingSessionFTPInfo. end (); ++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessionFTPInfo. clear ();
				}

				if ((errGeneric = csCfgSection. finish ()) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);
				if ((errGeneric = schScheduler. cancel ()) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);
				if ((errGeneric = schScheduler. finish (true)) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

				if ((errGeneric = tTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = tTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				if ((errGeneric = cfConfiguration. finish ()) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return 1;
			}

			if ((errGeneric = csCfgSection. finish ()) != errNoError)
			{
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGeneric, __FILE__, __LINE__);

				Error err = ConfigurationErrors (__FILE__, __LINE__,
					CFG_CONFIGURATIONSECTION_FINISH_FAILED);
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				vEncodingSessionTimes. clear ();

				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
						++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessions. clear ();
				}
				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessionFTPInfo. begin ();
						it != vEncodingSessionFTPInfo. end (); ++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessionFTPInfo. clear ();
				}

				if ((errGeneric = schScheduler. cancel ()) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);
				if ((errGeneric = schScheduler. finish (true)) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

				if ((errGeneric = tTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = tTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				if ((errGeneric = cfConfiguration. finish ()) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return 1;
			}

			if (strncmp (pSectionName, "MSITE_", strlen ("MSITE_")))
				continue;

			{
				std:: vector<Buffer_p>:: const_iterator	it;
				Buffer_p									pbEncodingSession;


				for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
					++it)
				{
					pbEncodingSession			= *it;

					if (!strcmp ((const char *) (*pbEncodingSession),
						pSectionName + strlen ("MSITE_")))
						break;
				}

				if (it != vEncodingSessions. end ())
				{
					Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_FOUNDTWOEQUALSESSIONENCODING,
						1, pSectionName + strlen ("MSITE_"));
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if ((pbEncodingSession = new Buffer_t) == (Buffer_p) NULL)
				{
					Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_NEW_FAILED);
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if (pbEncodingSession -> init (pSectionName + strlen ("MSITE_")) !=
					errNoError)
				{
					Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_NEW_FAILED);
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete pbEncodingSession;

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				vEncodingSessions. insert (vEncodingSessions. begin (),
					pbEncodingSession);
			}

			if ((errGetItemValue = cfConfiguration. getItemValue (
				pSectionName,
				"Encoder", pEncoder)) != errNoError)
			{
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGetItemValue, __FILE__, __LINE__);

				Error err = ConfigurationErrors (__FILE__, __LINE__,
					CFG_CONFIG_GETITEMVALUE_FAILED,
					2, pSectionName, "Encoder");
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				vEncodingSessionTimes. clear ();

				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
						++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessions. clear ();
				}
				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessionFTPInfo. begin ();
						it != vEncodingSessionFTPInfo. end (); ++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessionFTPInfo. clear ();
				}

				if ((errGeneric = schScheduler. cancel ()) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);
				if ((errGeneric = schScheduler. finish (true)) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

				if ((errGeneric = tTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = tTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				if ((errGeneric = cfConfiguration. finish ()) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return 1;
			}

			if ((errGetItemValue = cfConfiguration. getItemValue (
				pSectionName,
				"FTPHost", pFTPHost)) != errNoError)
			{
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGetItemValue, __FILE__, __LINE__);

				Error err = ConfigurationErrors (__FILE__, __LINE__,
					CFG_CONFIG_GETITEMVALUE_FAILED,
					2, pSectionName, "FTPHost");
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				vEncodingSessionTimes. clear ();

				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
						++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessions. clear ();
				}
				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessionFTPInfo. begin ();
						it != vEncodingSessionFTPInfo. end (); ++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessionFTPInfo. clear ();
				}

				if ((errGeneric = schScheduler. cancel ()) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);
				if ((errGeneric = schScheduler. finish (true)) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

				if ((errGeneric = tTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = tTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				if ((errGeneric = cfConfiguration. finish ()) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return 1;
			}

			if ((errGetItemValue = cfConfiguration. getItemValue (
				pSectionName,
				"FTPUser", pFTPUser)) != errNoError)
			{
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGetItemValue, __FILE__, __LINE__);

				Error err = ConfigurationErrors (__FILE__, __LINE__,
					CFG_CONFIG_GETITEMVALUE_FAILED,
					2, pSectionName, "FTPUser");
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				vEncodingSessionTimes. clear ();

				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
						++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessions. clear ();
				}
				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessionFTPInfo. begin ();
						it != vEncodingSessionFTPInfo. end (); ++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessionFTPInfo. clear ();
				}

				if ((errGeneric = schScheduler. cancel ()) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);
				if ((errGeneric = schScheduler. finish (true)) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

				if ((errGeneric = tTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = tTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				if ((errGeneric = cfConfiguration. finish ()) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return 1;
			}

			if ((errGetItemValue = cfConfiguration. getItemValue (
				pSectionName,
				"FTPPassword", pFTPPassword, 0, Config:: CFG_ENCRIPTION)) !=
				errNoError)
			{
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGetItemValue, __FILE__, __LINE__);

				Error err = ConfigurationErrors (__FILE__, __LINE__,
					CFG_CONFIG_GETITEMVALUE_FAILED,
					2, pSectionName, "FTPPassword");
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				vEncodingSessionTimes. clear ();

				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
						++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessions. clear ();
				}
				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessionFTPInfo. begin ();
						it != vEncodingSessionFTPInfo. end (); ++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessionFTPInfo. clear ();
				}

				if ((errGeneric = schScheduler. cancel ()) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);
				if ((errGeneric = schScheduler. finish (true)) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

				if ((errGeneric = tTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = tTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				if ((errGeneric = cfConfiguration. finish ()) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return 1;
			}

			if ((errGetItemValue = cfConfiguration. getItemValue (
				pSectionName,
				"FTPRootPath", pFTPRootPath)) != errNoError)
			{
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGetItemValue, __FILE__, __LINE__);

				Error err = ConfigurationErrors (__FILE__, __LINE__,
					CFG_CONFIG_GETITEMVALUE_FAILED,
					2, pSectionName, "FTPRootPath");
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				vEncodingSessionTimes. clear ();

				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
						++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessions. clear ();
				}
				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessionFTPInfo. begin ();
						it != vEncodingSessionFTPInfo. end (); ++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessionFTPInfo. clear ();
				}

				if ((errGeneric = schScheduler. cancel ()) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);
				if ((errGeneric = schScheduler. finish (true)) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

				if ((errGeneric = tTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = tTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				if ((errGeneric = cfConfiguration. finish ()) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return 1;
			}

			{
				std:: vector<Buffer_p>:: const_iterator	it;
				Buffer_p									pbEncodingSessionFTPInfo;


				for (it = vEncodingSessionFTPInfo. begin ();
					it != vEncodingSessionFTPInfo. end (); ++it)
				{
					pbEncodingSessionFTPInfo			= *it;

					if (strstr ((const char *) (*pbEncodingSessionFTPInfo),
						pFTPHost) != (char *) NULL &&
						strstr ((const char *) (*pbEncodingSessionFTPInfo),
						pFTPRootPath) != (char *) NULL)
						break;
				}

				if (it != vEncodingSessionFTPInfo. end ())
				{
					Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_FOUNDTWOEQUALSESSIONENCODINGFTPINFO,
						2, pFTPHost, pFTPRootPath);
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if ((pbEncodingSessionFTPInfo = new Buffer_t) == (Buffer_p) NULL)
				{
					Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_NEW_FAILED);
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if (pbEncodingSessionFTPInfo -> init (pFTPHost) != errNoError)
				{
					Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_NEW_FAILED);
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete pbEncodingSessionFTPInfo;

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if (pbEncodingSessionFTPInfo -> append (pFTPRootPath) != errNoError)
				{
					Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_NEW_FAILED);
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					pbEncodingSessionFTPInfo -> finish ();
					delete pbEncodingSessionFTPInfo;

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				vEncodingSessionFTPInfo. insert (vEncodingSessionFTPInfo. begin (),
					pbEncodingSessionFTPInfo);
			}

			if ((errGetItemValue = cfConfiguration. getItemValue (
				pSectionName,
				"FilesList", pFilesList)) != errNoError)
			{
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGetItemValue, __FILE__, __LINE__);

				Error err = ConfigurationErrors (__FILE__, __LINE__,
					CFG_CONFIG_GETITEMVALUE_FAILED,
					2, pSectionName, "FilesList");
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				vEncodingSessionTimes. clear ();

				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
						++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessions. clear ();
				}
				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessionFTPInfo. begin ();
						it != vEncodingSessionFTPInfo. end (); ++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessionFTPInfo. clear ();
				}

				if ((errGeneric = schScheduler. cancel ()) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);
				if ((errGeneric = schScheduler. finish (true)) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

				if ((errGeneric = tTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = tTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				if ((errGeneric = cfConfiguration. finish ()) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return 1;
			}

			if ((errGetItemValue = cfConfiguration. getItemValue (
				pSectionName,
				"Schedule", pSchedule)) != errNoError)
			{
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) errGetItemValue, __FILE__, __LINE__);

				Error err = ConfigurationErrors (__FILE__, __LINE__,
					CFG_CONFIG_GETITEMVALUE_FAILED,
					2, pSectionName, "Schedule");
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				vEncodingSessionTimes. clear ();

				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
						++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessions. clear ();
				}
				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessionFTPInfo. begin ();
						it != vEncodingSessionFTPInfo. end (); ++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessionFTPInfo. clear ();
				}

				if ((errGeneric = schScheduler. cancel ()) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);
				if ((errGeneric = schScheduler. finish (true)) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

				if ((errGeneric = tTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = tTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				if ((errGeneric = cfConfiguration. finish ()) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return 1;
			}

			{
				Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
					SFE_SCHEDULERFORENCODER_FOUNDMSITETOSCHEDULE,
					3, pSectionName + strlen ("MSITE_"), pEncoder, pSchedule);
				tTracer. trace (Tracer:: TRACER_LINFO, (const char *) msg,
					__FILE__, __LINE__);
			}

			if (!strcmp (pEncoder, "Vidiator"))
			{
				TimesForVidiator_p		pvrTimesForVidiator;
				char					pVidiatorIPAdress [
					SFE_MAXIPADDRESSLENGTH];
				char					pVidiatorPort [
					SFE_MAXLONGLENGTH];


				if ((errGetItemValue = cfConfiguration. getItemValue (
					"Vidiator",
					"IPAddress", pVidiatorIPAdress)) != errNoError)
				{
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGetItemValue, __FILE__, __LINE__);

					Error err = ConfigurationErrors (__FILE__, __LINE__,
						CFG_CONFIG_GETITEMVALUE_FAILED,
						2, "Vidiator", "IPAddress");
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if ((errGetItemValue = cfConfiguration. getItemValue (
					"Vidiator",
					"Port", pVidiatorPort)) != errNoError)
				{
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGetItemValue, __FILE__, __LINE__);

					Error err = ConfigurationErrors (__FILE__, __LINE__,
						CFG_CONFIG_GETITEMVALUE_FAILED,
						2, "Vidiator", "Port");
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if ((pvrTimesForVidiator = new TimesForVidiator_t) ==
					(TimesForVidiator_p) NULL)
				{
					Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_NEW_FAILED);
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if (pvrTimesForVidiator -> init (pSectionName + strlen ("MSITE_"),
					pSchedule, pSourcesPath, pEncodedPath, pProfilesPath, pTemporaryPath,
					pFTPUser, pFTPPassword, pFTPHost, pFTPRootPath, pFilesList,
					pVidiatorIPAdress, atol (pVidiatorPort), &tTracer) != errNoError)
				{
					Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_TIMESFORVIDIATOR_INIT_FAILED);
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete pvrTimesForVidiator;

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if ((errGeneric = pvrTimesForVidiator -> start ()) != errNoError)
				{
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_TIMES_START_FAILED);
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if ((errGeneric = pvrTimesForVidiator -> finish ()) !=	errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					delete pvrTimesForVidiator;

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				vEncodingSessionTimes. insert (vEncodingSessionTimes. begin (),
					pvrTimesForVidiator);
			}
			else if (!strcmp (pEncoder, "Nexencoder"))
			{
				TimesForNexencoder_p		pvrTimesForNexencoder;
				char					pNexencoderIPAdress [
					SFE_MAXIPADDRESSLENGTH];
				char					pNexencoderPort [
					SFE_MAXLONGLENGTH];
				char					pNexencoderPathName [
					SFE_MAXPATHLENGTH];
				char					pNexencoderUser [
					SFE_MAXUSERLENGTH];
				char					pNexencoderPassword [
					SFE_MAXPASSWORDLENGTH];


				if ((errGetItemValue = cfConfiguration. getItemValue (
					"Nexencoder",
					"IPAddress", pNexencoderIPAdress)) != errNoError)
				{
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGetItemValue, __FILE__, __LINE__);

					Error err = ConfigurationErrors (__FILE__, __LINE__,
						CFG_CONFIG_GETITEMVALUE_FAILED,
						2, "Nexencoder", "IPAddress");
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if ((errGetItemValue = cfConfiguration. getItemValue (
					"Nexencoder",
					"Port", pNexencoderPort)) != errNoError)
				{
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGetItemValue, __FILE__, __LINE__);

					Error err = ConfigurationErrors (__FILE__, __LINE__,
						CFG_CONFIG_GETITEMVALUE_FAILED,
						2, "Nexencoder", "Port");
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if ((errGetItemValue = cfConfiguration. getItemValue (
					"Nexencoder",
					"PathName", pNexencoderPathName)) != errNoError)
				{
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGetItemValue, __FILE__, __LINE__);

					Error err = ConfigurationErrors (__FILE__, __LINE__,
						CFG_CONFIG_GETITEMVALUE_FAILED,
						2, "Nexencoder", "PathName");
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if ((errGetItemValue = cfConfiguration. getItemValue (
					"Nexencoder",
					"User", pNexencoderUser)) != errNoError)
				{
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGetItemValue, __FILE__, __LINE__);

					Error err = ConfigurationErrors (__FILE__, __LINE__,
						CFG_CONFIG_GETITEMVALUE_FAILED,
						2, "Nexencoder", "User");
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if ((errGetItemValue = cfConfiguration. getItemValue (
					"Nexencoder",
					"Password", pNexencoderPassword, 0,
					Config:: CFG_ENCRIPTION)) != errNoError)
				{
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGetItemValue, __FILE__, __LINE__);

					Error err = ConfigurationErrors (__FILE__, __LINE__,
						CFG_CONFIG_GETITEMVALUE_FAILED,
						2, "Nexencoder", "Password");
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if ((pvrTimesForNexencoder = new TimesForNexencoder_t) ==
					(TimesForNexencoder_p) NULL)
				{
					Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_NEW_FAILED);
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if (pvrTimesForNexencoder -> init (pSectionName + strlen ("MSITE_"),
					pSchedule, pSourcesPath, pEncodedPath, pProfilesPath, pTemporaryPath,
					pFTPUser, pFTPPassword, pFTPHost, pFTPRootPath, pFilesList,
					pNexencoderIPAdress, atol (pNexencoderPort), pNexencoderPathName,
					pNexencoderUser, pNexencoderPassword, &tTracer) != errNoError)
				{
					Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
						SFE_TIMESFORNEXENCODER_INIT_FAILED);
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					delete pvrTimesForNexencoder;

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				if ((errGeneric = pvrTimesForNexencoder -> start ()) != errNoError)
				{
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_TIMES_START_FAILED);
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if ((errGeneric = pvrTimesForNexencoder -> finish ()) !=	errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					delete pvrTimesForNexencoder;

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}

				vEncodingSessionTimes. insert (vEncodingSessionTimes. begin (),
					pvrTimesForNexencoder);
			}
			else
			{
				Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
					SFE_SCHEDULERFORENCODER_ENCODERNAMEWRONG,
					1, pEncoder);
				tTracer. trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				vEncodingSessionTimes. clear ();

				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
						++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessions. clear ();
				}
				{
					std:: vector<Buffer_p>:: const_iterator		it;
					Buffer_p									pbSessionEncoding;

					for (it = vEncodingSessionFTPInfo. begin ();
						it != vEncodingSessionFTPInfo. end (); ++it)
					{
						pbSessionEncoding			= *it;
						pbSessionEncoding -> finish ();
						delete pbSessionEncoding;
					}
					vEncodingSessionFTPInfo. clear ();
				}

				if ((errGeneric = schScheduler. cancel ()) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);
				if ((errGeneric = schScheduler. finish (true)) != errNoError)
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

				if ((errGeneric = tTracer. cancel ()) != errNoError)
					cerr << ((const char *) errGeneric) << endl;
				if ((errGeneric = tTracer. finish (true)) != errNoError)
					cerr << (const char *) errGeneric << endl;

				if ((errGeneric = cfConfiguration. finish ()) != errNoError)
					cerr << (const char *) errGeneric << endl;

				return 1;
			}
		}

		{
			std:: vector<Times_p>:: const_iterator		it;
			Times_p										ptTimes;

			for (it = vEncodingSessionTimes. begin (); it != vEncodingSessionTimes. end ();
				++it)
			{
				ptTimes				= *it;

				if ((errGeneric = schScheduler. activeTimes (ptTimes)) !=
					errNoError)
				{
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) errGeneric, __FILE__, __LINE__);

					Error err = SchedulerErrors (__FILE__, __LINE__,
						SCH_SCHEDULER_ACTIVETIMES_FAILED);
					tTracer. trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					vEncodingSessionTimes. clear ();

					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
							++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessions. clear ();
					}
					{
						std:: vector<Buffer_p>:: const_iterator		it;
						Buffer_p									pbSessionEncoding;

						for (it = vEncodingSessionFTPInfo. begin ();
							it != vEncodingSessionFTPInfo. end (); ++it)
						{
							pbSessionEncoding			= *it;
							pbSessionEncoding -> finish ();
							delete pbSessionEncoding;
						}
						vEncodingSessionFTPInfo. clear ();
					}

					if ((errGeneric = schScheduler. cancel ()) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);
					if ((errGeneric = schScheduler. finish (true)) != errNoError)
						tTracer. trace (Tracer:: TRACER_LERRR,
							(const char *) errGeneric, __FILE__, __LINE__);

					if ((errGeneric = tTracer. cancel ()) != errNoError)
						cerr << ((const char *) errGeneric) << endl;
					if ((errGeneric = tTracer. finish (true)) != errNoError)
						cerr << (const char *) errGeneric << endl;

					if ((errGeneric = cfConfiguration. finish ()) != errNoError)
						cerr << (const char *) errGeneric << endl;

					return 1;
				}
			}
		}
	}

	vEncodingSessionTimes. clear ();

	{
		std:: vector<Buffer_p>:: const_iterator		it;
		Buffer_p									pbSessionEncoding;

		for (it = vEncodingSessions. begin (); it != vEncodingSessions. end ();
			++it)
		{
			pbSessionEncoding			= *it;
			pbSessionEncoding -> finish ();
			delete pbSessionEncoding;
		}
		vEncodingSessions. clear ();
	}
	{
		std:: vector<Buffer_p>:: const_iterator		it;
		Buffer_p									pbSessionEncoding;

		for (it = vEncodingSessionFTPInfo. begin ();
			it != vEncodingSessionFTPInfo. end (); ++it)
		{
			pbSessionEncoding			= *it;
			pbSessionEncoding -> finish ();
			delete pbSessionEncoding;
		}
		vEncodingSessionFTPInfo. clear ();
	}

	if (schScheduler. join () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PTHREAD_JOIN_FAILED);

		if ((errGeneric = schScheduler. cancel ()) != errNoError)
			tTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);
		if ((errGeneric = schScheduler. finish (true)) != errNoError)
			tTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = tTracer. cancel ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = tTracer. finish (true)) != errNoError)
			cerr << (const char *) errGeneric << endl;

		if ((errGeneric = cfConfiguration. finish ()) != errNoError)
			cerr << (const char *) errGeneric << endl;

		return 1;
	}

	if (schScheduler. cancel () != errNoError)
	{
		errGeneric = SchedulerErrors (__FILE__, __LINE__,
			SCH_SCHEDULER_CANCEL_FAILED);

		if ((errGeneric = schScheduler. finish (true)) != errNoError)
			tTracer. trace (Tracer:: TRACER_LERRR,
				(const char *) errGeneric, __FILE__, __LINE__);

		if ((errGeneric = tTracer. cancel ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = tTracer. finish (true)) != errNoError)
			cerr << (const char *) errGeneric << endl;

		if ((errGeneric = cfConfiguration. finish ()) != errNoError)
			cerr << (const char *) errGeneric << endl;

		return 1;
	}

	if (schScheduler. finish (true) != errNoError)
	{
		errGeneric = SchedulerErrors (__FILE__, __LINE__,
			SCH_SCHEDULER_FINISH_FAILED);

		if ((errGeneric = tTracer. cancel ()) != errNoError)
			cerr << ((const char *) errGeneric) << endl;
		if ((errGeneric = tTracer. finish (true)) != errNoError)
			cerr << (const char *) errGeneric << endl;

		if ((errGeneric = cfConfiguration. finish ()) != errNoError)
			cerr << (const char *) errGeneric << endl;

		return 1;
	}

	if (tTracer. cancel () != errNoError)
	{
		Error err = TracerErrors (__FILE__, __LINE__,
			TRACER_TRACER_CANCEL_FAILED);	
		cerr << (const char *) err << endl;

		if (tTracer. finish (true) != errNoError)
		{
			Error err = TracerErrors (__FILE__, __LINE__,
				TRACER_TRACER_FINISH_FAILED);	
			cerr << (const char *) err << endl;
		}

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}
    
		return 1;
	}

	if (tTracer. finish (true) != errNoError)
	{
		Error err = TracerErrors (__FILE__, __LINE__,
			TRACER_TRACER_FINISH_FAILED);	
		cerr << (const char *) err << endl;

		if (cfConfiguration. finish () != errNoError)
		{
			Error err = ConfigurationFileErrors (__FILE__, __LINE__,
				CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
			cerr << (const char *) err << endl;
		}
    
		return 1;
	}

	if (cfConfiguration. finish () != errNoError)
	{
		Error err = ConfigurationFileErrors (__FILE__, __LINE__,
			CFGFILE_CONFIGURATIONFILE_FINISH_FAILED);
		cerr << (const char *) err << endl;
    
		return 1;
	}
    

	return 0;
}


