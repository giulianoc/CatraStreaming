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

#include "RequestForNexencoder.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "TelnetClient.h"
#include "SchedulerForEncoderMessages.h"
#include "FileIO.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <iostream.h>
#include <stdio.h>

#include "ncftp.h"


RequestForNexencoder:: RequestForNexencoder (void): RequestForEncoder ()

{

}


RequestForNexencoder:: ~RequestForNexencoder (void)

{

}


Error RequestForNexencoder:: init (const char *pName,
	const char *pSourcesPath, const char *pEncodedPath,
	const char *pProfilesPath, const char *pTemporaryPath,
	const char *pFTPUser, const char *pFTPPassword,
	const char *pFTPHost, const char *pFTPRootPath,
	const char *pFilesList,
	const char *pIpAddress, unsigned long ulPort,
	const char *pEncoderPathName,
	const char *pUser, const char *pPassword,
	Tracer_p ptTracer)

{

	Error					errTelnetInit;
	Error 					errTelnetFinish;
	Error					errVacuum;
//	SocketImpl_p			pSocketImpl;


	if (RequestForEncoder:: init (pName, pSourcesPath,
		pEncodedPath, pProfilesPath, pTemporaryPath,
		pFTPUser, pFTPPassword,
		pFTPHost, pFTPRootPath, pFilesList,
		ptTracer) != errNoError)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_REQUESTFORENCODER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	strcpy (_pIpAddress, pIpAddress);
	_ulPort				= ulPort;
	strcpy (_pEncoderPathName, pEncoderPathName);
	strcpy (_pUser, pUser);
	strcpy (_pPassword, pPassword);

	/*
	if ((errTelnetInit = _tcTelnetClient. init ((const char *) NULL,
		_pIpAddress, _ulPort,
		_pUser, _pPassword, 2, 0, TelnetClient:: SCK_WINDOWS_TELNETSERVER)) !=
		errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errTelnetInit, __FILE__, __LINE__);

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_TELNETCLIENT_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		// non ci preoccupiamo se al momento la connessione telnet non sia
		// attiva per due motivi:
		// 1. quando abbiamo time out la connessione telnet viene in ogni caso aperta
		// 2. se ora ritorniamo errore il processo viene chiuso
		return errNoError;
	}

	if (_tcTelnetClient. getSocketImpl (&pSocketImpl) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_GETSOCKETIMPL_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_tcTelnetClient. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_TELNETCLIENT_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (RequestForEncoder:: finish () != errNoError)
		{
			Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
				SFE_REQUESTFORENCODER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if ((errVacuum = pSocketImpl -> vacuumByTelnet (2, 0)) !=
		errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errVacuum, __FILE__, __LINE__);
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_VACUUM_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_tcTelnetClient. finish () != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_TELNETCLIENT_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (RequestForEncoder:: finish () != errNoError)
		{
			Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
				SFE_REQUESTFORENCODER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}
	*/


	return errNoError;
}


Error RequestForNexencoder:: finish (void)

{
	/*
	Error				errTelnetFinish;


	if ((errTelnetFinish = _tcTelnetClient. finish ()) !=
		errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errTelnetFinish, __FILE__, __LINE__);

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_TELNETCLIENT_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}
	*/

	if (RequestForEncoder:: finish () != errNoError)
	{
		Error err = SchedulerForEncoderErrors (__FILE__, __LINE__,
			SFE_REQUESTFORENCODER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);
	}


	return errNoError;
}


Error RequestForNexencoder:: createProfileInstance (const char *pProfileName,
	const char *pSourceFileName, Buffer_p pbProfilePath)

{

	Buffer_t				bProfile;
	Buffer_t				bProfileTemplatePath;
	Error_t					errRead;


	if (bProfileTemplatePath. init (_pProfilesPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bProfileTemplatePath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bProfileTemplatePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bProfileTemplatePath. append (pProfileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bProfileTemplatePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bProfileTemplatePath. append (".xml") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bProfileTemplatePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bProfile. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bProfileTemplatePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if ((errRead = bProfile. readBufferFromFile (
		(const char *) bProfileTemplatePath)) != errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) errRead,
			__FILE__, __LINE__);
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_READBUFFERFROMFILE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bProfile. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bProfileTemplatePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (pbProfilePath -> setBuffer (_pTemporaryPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bProfile. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bProfileTemplatePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (pbProfilePath -> append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_READBUFFERFROMFILE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bProfile. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bProfileTemplatePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (pbProfilePath -> append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_READBUFFERFROMFILE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bProfile. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bProfileTemplatePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (pbProfilePath -> append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_READBUFFERFROMFILE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bProfile. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bProfileTemplatePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (pbProfilePath -> append (pProfileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_READBUFFERFROMFILE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bProfile. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bProfileTemplatePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (pbProfilePath -> append (".xml") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_READBUFFERFROMFILE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bProfile. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bProfileTemplatePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bProfile. writeBufferOnFile ((const char *) (*pbProfilePath)) !=
		errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_WRITEBUFFERONFILE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bProfile. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bProfileTemplatePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bProfile. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

	   	if (FileIO:: remove ((const char *) (*pbProfilePath)) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_REMOVE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		if (bProfileTemplatePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bProfileTemplatePath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

	   	if (FileIO:: remove ((const char *) (*pbProfilePath)) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_REMOVE_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}


	return errNoError;
}


Error RequestForNexencoder:: encode (const char *pProfilePath,
	const char *pProfileName, const char *pSourceFileName)

{

	SocketImpl_p				pSocketImpl;
	char						pBuffer [SFE_MAXTELNETBUFFERLENGTH];
	long						lBufferLength;
	char						pEncoderCommand [SFE_MAXENCODERCOMMANDLENGTH];
	Error						errReadByTelnet;
	time_t						tStartTimeToWait;
	Error						errIsReady;
	Boolean_t					bIsReadyForReading;
	Error						errTelnetInit;
	Error						errTelnetFinish;
	Error						errVacuum;

	// All'uscita di questo metodo e' necessario mantenere la connessione telnet
	// attiva perche' l'encoder sta producendo il file

	// la connessione telnet, aperta nella init, la chiudiamo e la riapriamo
	// per essere sicuri che sia attiva
	/*
	if ((errTelnetFinish = _tcTelnetClient. finish ()) !=
		errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errTelnetFinish, __FILE__, __LINE__);

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_TELNETCLIENT_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);
	}
	*/

	if ((errTelnetInit = _tcTelnetClient. init ((const char *) NULL,
		_pIpAddress, _ulPort,
		_pUser, _pPassword, 2, 0, TelnetClient:: SCK_WINDOWS_TELNETSERVER)) !=
		errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errTelnetInit, __FILE__, __LINE__);

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_TELNETCLIENT_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_tcTelnetClient. getSocketImpl (&pSocketImpl) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_CLIENTSOCKET_GETSOCKETIMPL_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if ((errTelnetFinish = _tcTelnetClient. finish ()) !=
			errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errTelnetFinish, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_TELNETCLIENT_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if ((errVacuum = pSocketImpl -> vacuumByTelnet (2, 0)) !=
		errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errVacuum, __FILE__, __LINE__);
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_VACUUM_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if ((errTelnetFinish = _tcTelnetClient. finish ()) !=
			errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errTelnetFinish, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_TELNETCLIENT_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// don't put the extension of the encoded file (.3gp) because it
	// is put automatically by the encoder
	sprintf (pEncoderCommand, "\"%s\" %s\\%s\\%s %s %s\\%s\\%s_%s\r\n",
		_pEncoderPathName, _pSourcesPath, _pName, pSourceFileName,
		pProfilePath, _pEncodedPath, _pName, pSourceFileName,
		pProfileName);

	{
		Message msg = SchedulerForEncoderMessages (__FILE__, __LINE__,
			SFE_REQUESTFORNEXENCODER_COMMAND,
			2, _pName, pEncoderCommand);
		_ptTracer -> trace (Tracer:: TRACER_LINFO, (const char *) msg,
			__FILE__, __LINE__);
	}

	if (pSocketImpl -> writeString (pEncoderCommand) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETIMPL_WRITESTRING_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if ((errTelnetFinish = _tcTelnetClient. finish ()) !=
			errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errTelnetFinish, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_TELNETCLIENT_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// il codice che segue potrebbe essere sostituito
	// da un vacuumByTelnet, visto che non si utilizza
	// cio' che si legge
	tStartTimeToWait			= time (NULL);

	while (time (NULL) - tStartTimeToWait < 2)
	{
		if ((errIsReady = pSocketImpl -> isReadyForReading (
			&bIsReadyForReading, 2, 0)) != errNoError)
		{
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errIsReady, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKETIMPL_ISREADYFORREADING_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if ((errTelnetFinish = _tcTelnetClient. finish ()) !=
				errNoError)
			{
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errTelnetFinish, __FILE__, __LINE__);

				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_TELNETCLIENT_FINISH_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (bIsReadyForReading)
		{
			lBufferLength	= SFE_MAXTELNETBUFFERLENGTH - 1;

			if ((errReadByTelnet = pSocketImpl -> readByTelnet (
				pBuffer, &lBufferLength, true)) != errNoError)
			{
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errReadByTelnet, __FILE__, __LINE__);

				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETIMPL_READLINEBYTELNET_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if ((errTelnetFinish = _tcTelnetClient. finish ()) !=
					errNoError)
				{
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) errTelnetFinish, __FILE__, __LINE__);

					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_TELNETCLIENT_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}


	return errNoError;
}


Error RequestForNexencoder:: encodeFinished (Boolean_t bEncodeOK)

{

	Error				errTelnetFinish;


	if ((errTelnetFinish = _tcTelnetClient. finish ()) !=
		errNoError)
	{
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) errTelnetFinish, __FILE__, __LINE__);

		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_TELNETCLIENT_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RequestForNexencoder:: removeEncodedFile (const char *pSourceFileName,
	const char *pProfileName)

{

	Buffer_t						bEncodedFilePath;

	
	if (bEncodedFilePath. init (_pEncodedPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (bEncodedFilePath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (pSourceFileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append ("_") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (pProfileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (".3gp") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (FileIO:: remove ((const char *) bEncodedFilePath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_REMOVE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. setBuffer (_pEncodedPath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append ("\\") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (pSourceFileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append ("_") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append (pProfileName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. append ("_Info.txt") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (FileIO:: remove ((const char *) bEncodedFilePath) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEIO_REMOVE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bEncodedFilePath. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);
		}

		return err;
	}

	if (bEncodedFilePath. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


