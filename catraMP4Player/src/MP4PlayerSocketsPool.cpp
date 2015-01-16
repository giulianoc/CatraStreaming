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


#include "MP4PlayerSocketsPool.h"
#include "RTSPClientSession.h"
#include <assert.h>



MP4PlayerSocketsPool:: MP4PlayerSocketsPool (void):
	SocketsPool ()

{

}


MP4PlayerSocketsPool:: ~MP4PlayerSocketsPool (void)

{

}


MP4PlayerSocketsPool:: MP4PlayerSocketsPool (
	const MP4PlayerSocketsPool &t)

{
	assert (1 == 0);

}


Error MP4PlayerSocketsPool:: init (
	unsigned long ulMaxSocketsNumber,
	unsigned long ulCheckSocketsPoolPeriodInSeconds,
	unsigned long ulAdditionalCheckSocketsPoolPeriodInMicroseconds,
	Tracer_p ptTracer)

{

	_ptTracer				= ptTracer;

	if (SocketsPool:: init (ulMaxSocketsNumber,
		ulCheckSocketsPoolPeriodInSeconds,
		ulAdditionalCheckSocketsPoolPeriodInMicroseconds,
		false) != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETSPOOL_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4PlayerSocketsPool:: finish (void)

{

	if (SocketsPool:: finish () != errNoError)
	{
		Error err = SocketErrors (__FILE__, __LINE__,
			SCK_SOCKETSPOOL_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4PlayerSocketsPool:: updateSocketStatus (
	Socket_p pSocket, long lSocketType,
	void *pvSocketData,
	unsigned short usSocketCheckType)

{

	switch (lSocketType)
	{
		case MP4PL_MP4PLAYERSOCKETSPOOL_VIDEORTPSERVERSOCKET:
			{
				RTSPClientSession_p			prcsRTSPClientSession;
				unsigned long				ulRTSPSessionIdentifier;


				prcsRTSPClientSession	= ((RTSPClientSession_p) pvSocketData);

				if (prcsRTSPClientSession -> getRTSPSessionIdentifier (
					&ulRTSPSessionIdentifier) != errNoError)
				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_GETRTSPSESSIONIDENTIFIER_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (usSocketCheckType & SOCKETSTATUS_EXCEPTION)
				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
						MP4PL_MP4PLAYERSOCKETSPOOL_EXCEPTIONONSOCKET,
						2, ulRTSPSessionIdentifier, lSocketType);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (prcsRTSPClientSession ->
					processReceivingVideoRTPPacket () != errNoError)
				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_PROCESSRECEIVINGVIDEORTPPACKET_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			break;
		case MP4PL_MP4PLAYERSOCKETSPOOL_VIDEORTCPSERVERSOCKET:
			{
				RTSPClientSession_p			prcsRTSPClientSession;
				unsigned long				ulRTSPSessionIdentifier;


				prcsRTSPClientSession	= ((RTSPClientSession_p) pvSocketData);

				if (prcsRTSPClientSession -> getRTSPSessionIdentifier (
					&ulRTSPSessionIdentifier) != errNoError)
				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_GETRTSPSESSIONIDENTIFIER_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (usSocketCheckType & SOCKETSTATUS_EXCEPTION)
				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
						MP4PL_MP4PLAYERSOCKETSPOOL_EXCEPTIONONSOCKET,
						2, ulRTSPSessionIdentifier, lSocketType);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (prcsRTSPClientSession ->
					processReceivingVideoRTCPPacket () != errNoError)
				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_PROCESSRECEIVINGVIDEORTCPPACKET_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			break;
		case MP4PL_MP4PLAYERSOCKETSPOOL_AUDIORTPSERVERSOCKET:
			{
				RTSPClientSession_p			prcsRTSPClientSession;
				unsigned long				ulRTSPSessionIdentifier;


				prcsRTSPClientSession	= ((RTSPClientSession_p) pvSocketData);

				if (prcsRTSPClientSession -> getRTSPSessionIdentifier (
					&ulRTSPSessionIdentifier) != errNoError)
				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_GETRTSPSESSIONIDENTIFIER_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (usSocketCheckType & SOCKETSTATUS_EXCEPTION)
				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
						MP4PL_MP4PLAYERSOCKETSPOOL_EXCEPTIONONSOCKET,
						2, ulRTSPSessionIdentifier, lSocketType);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (prcsRTSPClientSession ->
					processReceivingAudioRTPPacket () != errNoError)
				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_PROCESSRECEIVINGAUDIORTPPACKET_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			break;
		case MP4PL_MP4PLAYERSOCKETSPOOL_AUDIORTCPSERVERSOCKET:
			{
				RTSPClientSession_p			prcsRTSPClientSession;
				unsigned long				ulRTSPSessionIdentifier;


				prcsRTSPClientSession	= ((RTSPClientSession_p) pvSocketData);

				if (prcsRTSPClientSession -> getRTSPSessionIdentifier (
					&ulRTSPSessionIdentifier) != errNoError)
				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
					MP4PL_RTSPCLIENTSESSION_GETRTSPSESSIONIDENTIFIER_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (usSocketCheckType & SOCKETSTATUS_EXCEPTION)
				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
						MP4PL_MP4PLAYERSOCKETSPOOL_EXCEPTIONONSOCKET,
						2, ulRTSPSessionIdentifier, lSocketType);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}

				if (prcsRTSPClientSession ->
					processReceivingAudioRTCPPacket () != errNoError)
				{
					Error err = MP4PlayerErrors (__FILE__, __LINE__,
				MP4PL_RTSPCLIENTSESSION_PROCESSRECEIVINGAUDIORTCPPACKET_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					return err;
				}
			}

			break;
		default:
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_WRONGSOCKETTYPE,
					1, lSocketType);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
	}


	return errNoError;
}

