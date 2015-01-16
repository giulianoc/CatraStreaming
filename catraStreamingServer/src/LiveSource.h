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

#ifndef LiveSource_h
	#define LiveSource_h

	#include "Tracer.h"
	#include "StreamingServerSocketsPool.h"
	#include "ServerSocket.h"
	#include "RTPPacket.h"
	#include "StreamingServerErrors.h"
	#include <vector>

	#define SS_LIVESOURCE_MAXLIVERTPPACKETSIZE					1024 * 4
	#define SS_LIVESOURCE_TIMEOUTINSECSTOREADFIRSTRTPPACKET		3


	typedef class LiveSource

	{
		private:
			typedef enum LiveSourceStatus {
				SS_LIVESOURCE_NOTINITIALIZED,
				SS_LIVESOURCE_INITIALIZED
			} LiveSourceStatus_t, *LiveSourceStatus_p;

		private:
			PMutex_t				_mtLiveSource;
			Tracer_p				_ptSystemTracer;
			LiveSourceStatus_t		_lssStatus;

			SocketsPool_p			_pspSocketsPool;

			Boolean_t				_bVideoTrack;
			unsigned long			_ulVideoPort;
			ServerSocket_t			_ssLiveVideoServerSocket;
			SocketImpl_p			_pVideoServerSocketImpl;
			RTPPacket_t				_rpLastVideoRTPPacket;
			Boolean_t				_bLastVideoRTPPacketFilled;

			Boolean_t				_bAudioTrack;
			unsigned long			_ulAudioPort;
			ServerSocket_t			_ssLiveAudioServerSocket;
			SocketImpl_p			_pAudioServerSocketImpl;
			RTPPacket_t				_rpLastAudioRTPPacket;
			Boolean_t				_bLastAudioRTPPacketFilled;

			std:: vector<unsigned long>	_vActiveLiveRTSPSessions;

		protected:
			LiveSource (const LiveSource &);

			LiveSource &operator = (const LiveSource &);

			Error readRTPPacket (
				Boolean_t bIsVideoSocket,
				unsigned long ulSecondsToWait,
				Boolean_t bRemoveDataFromSocket);

		public:
			LiveSource (void);

			~LiveSource (void);

			Error init (
				SocketsPool_p pspSocketsPool,
				const char *pLocalIPAddressForRTP,
				Boolean_t bVideoTrackFoundInSDP,
				unsigned long ulLiveVideoRTPPacketsServerPort,
				Boolean_t bAudioTrackFoundInSDP,
				unsigned long ulLiveAudioRTPPacketsServerPort,
				Tracer_p ptTracer);

			Error finish (void);

			Error isInitialized (
				Boolean_p pbIsInitialized);

			Error addRTSPSession (
				unsigned long ulRTSPSEssionIdentifier);

			Error deleteRTSPSession (
				unsigned long ulRTSPSEssionIdentifier,
				Boolean_p pbIsLiveSourceEmpty);

			Error readRTPPacket (
				ServerSocket_p pssLiveServerSocket,
				RTPPacket_p *prpRTPPacket,
				std:: vector<unsigned long> **pvActiveLiveRTSPSessions,
				Boolean_p pbIsVideoSocket);

			Error getLastRTPPacket (
				Boolean_t bIsVideoSocket,
				RTPPacket_p *prpRTPPacket);

	} LiveSource_t, *LiveSource_p;

#endif
