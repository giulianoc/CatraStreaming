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

#include <assert.h>
#include "LiveSource.h"



LiveSource:: LiveSource (void)

{

	_lssStatus			= SS_LIVESOURCE_NOTINITIALIZED;

}


LiveSource:: ~LiveSource (void)

{

}


LiveSource:: LiveSource (const LiveSource &)

{

	assert (1==0);
}


LiveSource &LiveSource:: operator = (const LiveSource &)

{

	assert (1==0);

	return *this;
}


Error LiveSource:: init (
	SocketsPool_p pspSocketsPool,
	const char *pLocalIPAddressForRTP,
	Boolean_t bVideoTrackFoundInSDP,
	unsigned long ulLiveVideoRTPPacketsServerPort,
	Boolean_t bAudioTrackFoundInSDP,
	unsigned long ulLiveAudioRTPPacketsServerPort,
	Tracer_p ptTracer)

{

	Error_t					errSocketInit;


	_ptSystemTracer			= ptTracer;

	if (_lssStatus == SS_LIVESOURCE_INITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	_pspSocketsPool		= pspSocketsPool;
	_bVideoTrack		= bVideoTrackFoundInSDP;
	_bAudioTrack		= bAudioTrackFoundInSDP;

	if (_bVideoTrack)
	{
		_ulVideoPort	= ulLiveVideoRTPPacketsServerPort;

		if ((errSocketInit = _ssLiveVideoServerSocket. init (
			pLocalIPAddressForRTP, _ulVideoPort,
			true, SocketImpl:: DGRAM, 30, 0)) != errNoError)
		{
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errSocketInit, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (_ssLiveVideoServerSocket. getSocketImpl (
			&_pVideoServerSocketImpl) != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKET_GETSOCKETIMPL_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_ssLiveVideoServerSocket. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (_rpLastVideoRTPPacket. init (
			SS_LIVESOURCE_MAXLIVERTPPACKETSIZE,
			_ptSystemTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_ssLiveVideoServerSocket. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		_bLastVideoRTPPacketFilled			= false;
	}

	if (_bAudioTrack)
	{
		_ulAudioPort	= ulLiveAudioRTPPacketsServerPort;

		if ((errSocketInit = _ssLiveAudioServerSocket. init (
			pLocalIPAddressForRTP, _ulAudioPort,
			true, SocketImpl:: DGRAM, 30, 0)) != errNoError)
		{
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) errSocketInit, __FILE__, __LINE__);

			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_bVideoTrack)
			{
				if (_rpLastVideoRTPPacket. finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_ssLiveVideoServerSocket. finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return err;
		}

		if (_ssLiveAudioServerSocket. getSocketImpl (
			&_pAudioServerSocketImpl) != errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SOCKET_GETSOCKETIMPL_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_ssLiveAudioServerSocket. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bVideoTrack)
			{
				if (_rpLastVideoRTPPacket. finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_ssLiveVideoServerSocket. finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return err;
		}

		if (_rpLastAudioRTPPacket. init (
			SS_LIVESOURCE_MAXLIVERTPPACKETSIZE,
			_ptSystemTracer) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_INIT_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_ssLiveAudioServerSocket. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_bVideoTrack)
			{
				if (_rpLastVideoRTPPacket. finish () != errNoError)
				{
					Error err = RTPErrors (__FILE__, __LINE__,
						RTP_RTPPACKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_ssLiveVideoServerSocket. finish () !=
					errNoError)
				{
					Error err = SocketErrors (__FILE__, __LINE__,
						SCK_SERVERSOCKET_FINISH_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}
			}

			return err;
		}

		_bLastAudioRTPPacketFilled			= false;
	}

	if (_mtLiveSource. init (PMutex:: MUTEX_RECURSIVE) != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_INIT_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_bAudioTrack)
		{
			if (_rpLastAudioRTPPacket. finish () != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPPACKET_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_ssLiveAudioServerSocket. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		if (_bVideoTrack)
		{
			if (_rpLastVideoRTPPacket. finish () != errNoError)
			{
				Error err = RTPErrors (__FILE__, __LINE__,
					RTP_RTPPACKET_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_ssLiveVideoServerSocket. finish () !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SERVERSOCKET_FINISH_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}
		}

		return err;
	}

	_lssStatus			= SS_LIVESOURCE_INITIALIZED;


	return errNoError;
}


Error LiveSource:: finish (void)

{

	if (_lssStatus == SS_LIVESOURCE_NOTINITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mtLiveSource. finish () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_FINISH_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		// return err;
	}

	{
		std:: vector<unsigned long>:: iterator	itRTSPSession;
		unsigned long		ulLocalRTSPSessionIdentifier;
		Boolean_t			bIsLiveSourceEmpty;


		for (itRTSPSession = _vActiveLiveRTSPSessions. begin ();
			itRTSPSession != _vActiveLiveRTSPSessions. end ();
			++itRTSPSession)
		{
			ulLocalRTSPSessionIdentifier		= *itRTSPSession;

			if (deleteRTSPSession (ulLocalRTSPSessionIdentifier,
				&bIsLiveSourceEmpty) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_LIVESOURCE_DELETERTSPSESSION_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				// return err;
			}
		}
	}

	_vActiveLiveRTSPSessions. clear ();

	if (_bVideoTrack)
	{
		if (_rpLastVideoRTPPacket. finish () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_ssLiveVideoServerSocket. finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			// return err;
		}

		_bLastVideoRTPPacketFilled			= false;
	}

	if (_bAudioTrack)
	{
		if (_rpLastAudioRTPPacket. finish () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_ssLiveAudioServerSocket. finish () !=
			errNoError)
		{
			Error err = SocketErrors (__FILE__, __LINE__,
				SCK_SERVERSOCKET_FINISH_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			// return err;
		}

		_bLastAudioRTPPacketFilled			= false;
	}

	_lssStatus			= SS_LIVESOURCE_NOTINITIALIZED;


	return errNoError;
}


Error LiveSource:: isInitialized (
	Boolean_p pbIsInitialized)

{

	if (_lssStatus == SS_LIVESOURCE_NOTINITIALIZED)
		*pbIsInitialized		= false;
	else
		*pbIsInitialized		= true;


	return errNoError;
}


Error LiveSource:: addRTSPSession (
	unsigned long ulRTSPSEssionIdentifier)

{

	std:: vector<unsigned long>:: iterator	itRTSPSession;
	unsigned long		ulLocalRTSPSessionIdentifier;
	Error_t				errReadRTPPacket;


	if (_lssStatus == SS_LIVESOURCE_NOTINITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mtLiveSource. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	for (itRTSPSession = _vActiveLiveRTSPSessions. begin ();
		itRTSPSession != _vActiveLiveRTSPSessions. end ();
		++itRTSPSession)
	{
		ulLocalRTSPSessionIdentifier		= *itRTSPSession;

		if (ulLocalRTSPSessionIdentifier == ulRTSPSEssionIdentifier)
			break;
	}

	if (itRTSPSession != _vActiveLiveRTSPSessions. end ())
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_LIVESOURCE_LIVERTSPSESSIONALREADYADDED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtLiveSource. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	_vActiveLiveRTSPSessions. insert (
		_vActiveLiveRTSPSessions. end (),
		ulRTSPSEssionIdentifier);

	if (_vActiveLiveRTSPSessions. size () == 1)
	{
		if (_bVideoTrack)
		{
			if ((errReadRTPPacket = readRTPPacket (
				true, SS_LIVESOURCE_TIMEOUTINSECSTOREADFIRSTRTPPACKET,
				false)) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_LIVESOURCE_READRTPPACKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_LIVESOURCE_NOFOUNDFIRSTLIVERTPPACKETTOREAD,
						3, ulRTSPSEssionIdentifier, "Video",
						SS_LIVESOURCE_TIMEOUTINSECSTOREADFIRSTRTPPACKET);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_mtLiveSource. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return errReadRTPPacket;
			}

			if (_pspSocketsPool -> addSocket (
				SocketsPool:: SOCKETSTATUS_READ |
				SocketsPool:: SOCKETSTATUS_EXCEPTION,
				SS_STREAMINGSERVERSOCKETPOOL_LIVESERVERSOCKET,
				&_ssLiveVideoServerSocket,
				this) != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_ADDSOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtLiveSource. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (_bAudioTrack)
		{
			if ((errReadRTPPacket = readRTPPacket (
				false, SS_LIVESOURCE_TIMEOUTINSECSTOREADFIRSTRTPPACKET,
				false)) != errNoError)
			{
				Error err = StreamingServerErrors (__FILE__, __LINE__,
					SS_LIVESOURCE_READRTPPACKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				{
					Error err = StreamingServerErrors (__FILE__, __LINE__,
						SS_LIVESOURCE_NOFOUNDFIRSTLIVERTPPACKETTOREAD,
						3, ulRTSPSEssionIdentifier, "Audio",
						SS_LIVESOURCE_TIMEOUTINSECSTOREADFIRSTRTPPACKET);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				if (_bVideoTrack)
				{
					LiveSource_p	plsLocalLiveSource;

					if (_pspSocketsPool -> deleteSocket (
						&_ssLiveVideoServerSocket,
						(void **) (&plsLocalLiveSource)) !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				if (_mtLiveSource. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return errReadRTPPacket;
			}

			if (_pspSocketsPool -> addSocket (
				SocketsPool:: SOCKETSTATUS_READ |
				SocketsPool:: SOCKETSTATUS_EXCEPTION,
				SS_STREAMINGSERVERSOCKETPOOL_LIVESERVERSOCKET,
				&_ssLiveAudioServerSocket,
				this) != errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_ADDSOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_bVideoTrack)
				{
					LiveSource_p	plsLocalLiveSource;

					if (_pspSocketsPool -> deleteSocket (
						&_ssLiveVideoServerSocket,
						(void **) (&plsLocalLiveSource)) !=
						errNoError)
					{
						Error err = SocketErrors (__FILE__, __LINE__,
							SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
						_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}
				}

				if (_mtLiveSource. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	if (_mtLiveSource. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error LiveSource:: deleteRTSPSession (
	unsigned long ulRTSPSEssionIdentifier,
	Boolean_p pbIsLiveSourceEmpty)

{

	std:: vector<unsigned long>:: iterator	itRTSPSession;
	unsigned long		ulLocalRTSPSessionIdentifier;


	if (_lssStatus == SS_LIVESOURCE_NOTINITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pbIsLiveSourceEmpty		= false;

	if (_mtLiveSource. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	for (itRTSPSession = _vActiveLiveRTSPSessions. begin ();
		itRTSPSession != _vActiveLiveRTSPSessions. end ();
		++itRTSPSession)
	{
		ulLocalRTSPSessionIdentifier		= *itRTSPSession;

		if (ulLocalRTSPSessionIdentifier == ulRTSPSEssionIdentifier)
			break;
	}

	if (itRTSPSession == _vActiveLiveRTSPSessions. end ())
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_LIVESOURCE_LIVERTSPSESSIONNOTFOUND);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (_mtLiveSource. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	_vActiveLiveRTSPSessions. erase (itRTSPSession);

	if (_vActiveLiveRTSPSessions. size () == 0)
	{
		if (_bVideoTrack)
		{
			LiveSource_p	plsLocalLiveSource;

			if (_pspSocketsPool -> deleteSocket (
				&_ssLiveVideoServerSocket,
				(void **) (&plsLocalLiveSource)) !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtLiveSource. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (_bAudioTrack)
		{
			LiveSource_p	plsLocalLiveSource;

			if (_pspSocketsPool -> deleteSocket (
				&_ssLiveAudioServerSocket,
				(void **) (&plsLocalLiveSource)) !=
				errNoError)
			{
				Error err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETSPOOL_DELETESOCKET_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (_mtLiveSource. unLock () != errNoError)
				{
					Error err = PThreadErrors (__FILE__, __LINE__,
						THREADLIB_PMUTEX_UNLOCK_FAILED);
					_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		*pbIsLiveSourceEmpty		= true;
	}

	if (_mtLiveSource. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error LiveSource:: readRTPPacket (
	ServerSocket_p pssLiveServerSocket,
	RTPPacket_p *prpRTPPacket,
	std:: vector<unsigned long> **pvActiveLiveRTSPSessions,
	Boolean_p pbIsVideoSocket)

{

	Error_t					errReadRTPPacket;


	if (_lssStatus == SS_LIVESOURCE_NOTINITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mtLiveSource. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	*pvActiveLiveRTSPSessions		= &_vActiveLiveRTSPSessions;

	if (pssLiveServerSocket == &_ssLiveVideoServerSocket)
		*pbIsVideoSocket			= true;
	else
		*pbIsVideoSocket			= false;

	if ((errReadRTPPacket = readRTPPacket (*pbIsVideoSocket, 0, true)) !=
		errNoError)
	{
		// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
		//	(const char *) errReadRTPPacket, __FILE__, __LINE__);

		if ((long) errReadRTPPacket != SS_LIVESOURCE_NOTHINGTOREAD)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_LIVESOURCE_READRTPPACKET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		if (_mtLiveSource. unLock () != errNoError)
		{
			Error err = PThreadErrors (__FILE__, __LINE__,
				THREADLIB_PMUTEX_UNLOCK_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errReadRTPPacket;
	}

	if (pssLiveServerSocket == &_ssLiveVideoServerSocket)
		*prpRTPPacket			= &_rpLastVideoRTPPacket;
	else
		*prpRTPPacket			= &_rpLastAudioRTPPacket;

	if (_mtLiveSource. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error LiveSource:: readRTPPacket (
	Boolean_t bIsVideoSocket,
	unsigned long ulSecondsToWait,
	Boolean_t bRemoveDataFromSocket)

{

	SocketImpl_p			pServerSocketImpl;
	Boolean_p				pbRTPPAcketFilled;
	unsigned long			ulPacketLength;
	unsigned char			pucRTPPacket [
		SS_LIVESOURCE_MAXLIVERTPPACKETSIZE];
	RTPPacket_p				prpRTPPacket;
	Error_t					errRead;


	if (_lssStatus == SS_LIVESOURCE_NOTINITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mtLiveSource. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (bIsVideoSocket)
	{
		pServerSocketImpl		= _pVideoServerSocketImpl;
		prpRTPPacket			= &_rpLastVideoRTPPacket;
		pbRTPPAcketFilled		= &_bLastVideoRTPPacketFilled;
	}
	else
	{
		pServerSocketImpl		= _pAudioServerSocketImpl;
		prpRTPPacket			= &_rpLastAudioRTPPacket;
		pbRTPPAcketFilled		= &_bLastAudioRTPPacketFilled;
	}

	{
		ulPacketLength				= SS_LIVESOURCE_MAXLIVERTPPACKETSIZE;

		if ((errRead = pServerSocketImpl -> read (pucRTPPacket,
			&ulPacketLength, true, ulSecondsToWait, 0, true,
			bRemoveDataFromSocket)) != errNoError)
		{
			Error				err;

			if ((long) errRead == SCK_NOTHINGTOREAD)
			{
				err = StreamingServerErrors (__FILE__, __LINE__,
					SS_LIVESOURCE_NOTHINGTOREAD);
				// _ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				//	(const char *) err, __FILE__, __LINE__);
			}
			else
			{
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) errRead, __FILE__, __LINE__);

				err = SocketErrors (__FILE__, __LINE__,
					SCK_SOCKETIMPL_READ_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			if (_mtLiveSource. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (prpRTPPacket -> reset () != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_RESET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtLiveSource. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (prpRTPPacket -> setRTPPacket (
			pucRTPPacket, ulPacketLength) != errNoError)
		{
			Error err = RTPErrors (__FILE__, __LINE__,
				RTP_RTPPACKET_SETRTPPACKET_FAILED);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtLiveSource. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		*pbRTPPAcketFilled			= true;
	}

	if (_mtLiveSource. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error LiveSource:: getLastRTPPacket (
	Boolean_t bIsVideoSocket,
	RTPPacket_p *prpRTPPacket)

{

	if (_lssStatus == SS_LIVESOURCE_NOTINITIALIZED)
	{
		Error err = StreamingServerErrors (__FILE__, __LINE__,
			SS_ACTIVATION_WRONG);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (_mtLiveSource. lock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_LOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (bIsVideoSocket)
	{
		if (!_bLastVideoRTPPacketFilled)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_LIVESOURCE_LASTRTPPACKETNOTFOUND);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtLiveSource. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			*prpRTPPacket			= &_rpLastVideoRTPPacket;
	}
	else
	{
		if (!_bLastAudioRTPPacketFilled)
		{
			Error err = StreamingServerErrors (__FILE__, __LINE__,
				SS_LIVESOURCE_LASTRTPPACKETNOTFOUND);
			_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (_mtLiveSource. unLock () != errNoError)
			{
				Error err = PThreadErrors (__FILE__, __LINE__,
					THREADLIB_PMUTEX_UNLOCK_FAILED);
				_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			*prpRTPPacket			= &_rpLastAudioRTPPacket;
	}

	if (_mtLiveSource. unLock () != errNoError)
	{
		Error err = PThreadErrors (__FILE__, __LINE__,
			THREADLIB_PMUTEX_UNLOCK_FAILED);
		_ptSystemTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}

