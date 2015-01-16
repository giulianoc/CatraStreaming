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


#ifndef RTSPClientSession_h
	#define RTSPClientSession_h

	#include "PosixThread.h"
	#include "CatraMonitoringPlayerErrors.h"
	#include "CatraMonitoringPlayerMessages.h"
	#include "ClientSocket.h"
	#include "ServerSocket.h"
	#include "Tracer.h"
	#ifdef WIN32
		#include <windows.h>
	#else
	#endif



	#define MP4PL_RTSPREQUESTLENGTH						(1024 * 4) + 1
	#define MP4PL_CONTROLVALUELENGTH					512 + 1
	#define MP4PL_SESSIONLENGTH							512 + 1
	#define MP4PL_MAXIPADDRESSLENGTH					512 + 1
	#define MP4PL_MAXSESSIONIDENTIFIERLENGTH			1024 + 1
	#define MP4PL_MAXLONGLENGTH							512 + 1
	#define MP4PL_MAXRTCPPACKETLENGTH					1024 * 2
	#define MP4PL_MAXRTPPACKETLENGTH					1024 * 2
	#define MP4PL_MAXRTPINFOFIELDLENGTH					1024 + 1
	#define MP4PL_MAXSTATISTICSLENGTH					(1024 * 5) + 1
	#define MP4PL_MAXPATHNAMELENGTH						1024 + 1
	#define MP4PL_MAXDATETIMELENGTH						512 + 1

	#define MP4PL_MOVEDPERMANENTLYCHECK					"RTSP/1.0 301"


	typedef class RTSPClientSession: public PosixThread
	{
		private:
			Tracer_p				_ptTracer;
			Boolean_t				_bTraceOnTTY;
			unsigned long			_ulSecondsToWaitRTPPackets;

			Buffer_t				_bURL;
			Buffer_t				_bRedirectedURL;
			Boolean_t				_bIsRedirectedURL;
			Buffer_t				_bSession;
			Buffer_t				_bSDP;
			ClientSocket_t			_csClientSocket;
			char					_pRTSPServerIPAddress [
				MP4PL_MAXIPADDRESSLENGTH];
			long					_lRTSPServerPort;
			unsigned long			_ulVideoClientRTPPort;
			unsigned long			_ulVideoClientRTCPPort;
			unsigned long			_ulVideoServerRTPPort;
			unsigned long			_ulVideoServerRTCPPort;
			unsigned long			_ulAudioClientRTPPort;
			unsigned long			_ulAudioClientRTCPPort;
			unsigned long			_ulAudioServerRTPPort;
			unsigned long			_ulAudioServerRTCPPort;
			int						_iFdStatistics;

		Error parsePlayResponse (const char *pPlayResponse,
			const char *pVideoControlValue,
			unsigned long *pulStartVideoSequenceNumber,
			unsigned long *pulStartVideoTimeStamp,
			const char *pAudioControlValue,
			unsigned long *pulStartAudioSequenceNumber,
			unsigned long *pulStartAudioTimeStamp);

		Error parseTeardownResponse (const char *pPlayResponse);

		Error sendDescribe (const char *pURL,
			unsigned long ulSequenceNumber,
			SocketImpl_p psiClientSocketImpl);

		Error sendSetup (const char *pURL,
			const char *pControlValue, unsigned long ulSequenceNumber,
			Buffer_p pbSession, unsigned long ulClientRTPPort,
			unsigned long ulClientRTCPPort,
			SocketImpl_p psiClientSocketImpl);

		Error sendPlay (const char *pURL,
			const char *pControlValue,
			unsigned long ulSequenceNumber, Buffer_p pbSession,
			SocketImpl_p psiClientSocketImpl);

		#ifdef WIN32
			Error listenFirstRTPackets (
				__int64 *pullFirstRTPPacket_UTCInMilliSecs,
				Boolean_t bIsThereVideo, ServerSocket_p psVideoRTPServerSocket,
				Boolean_t bIsThereAudio, ServerSocket_p psAudioRTPServerSocket);
		#else
			Error listenFirstRTPackets (
				unsigned long long *pullFirstRTPPacket_UTCInMilliSecs,
				Boolean_t bIsThereVideo, ServerSocket_p psVideoRTPServerSocket,
				Boolean_t bIsThereAudio, ServerSocket_p psAudioRTPServerSocket);
		#endif

		Error sendTeardown (const char *pURL,
			unsigned long ulSequenceNumber, Buffer_p pbSession,
			SocketImpl_p psiClientSocketImpl);

		#ifdef WIN32
			Error writeStatistics (const char *pDescribeRequestTime,
				__int64 llDescribeResponseTime,
				__int64 llTimeBetweenDescribeAndFirstPacket,
				const char *pError);
		#else
			Error writeStatistics (const char *pDescribeRequestTime,
				long long llDescribeResponseTime,
				long long llTimeBetweenDescribeAndFirstPacket,
				const char *pError);
		#endif

		protected:
			virtual Error run (void);


		public:
			RTSPClientSession (void);

			~RTSPClientSession (void);

			Error init (const char *pURL,
				unsigned long ulVideoClientRTPPort,
				unsigned long ulVideoClientRTCPPort,
				unsigned long ulAudioClientRTPPort,
				unsigned long ulAudioClientRTCPPort,
				Boolean_t bTraceOnTTY, unsigned long ulSecondsToWaitRTPPackets,
				Tracer_p ptTracer);

			Error finish ();

			static Error appendToStatisticsFile (
				const char *pStatisticsFileName,
				const char *pBuffer, Tracer_p ptTracer);

	} RTSPClientSession_t, *RTSPClientSession_p;

#endif

