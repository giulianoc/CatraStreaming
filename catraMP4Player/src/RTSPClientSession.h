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
	#include "MP4PlayerErrors.h"
	#include "MP4PlayerMessages.h"
	#include "ClientSocket.h"
	#include "ServerSocket.h"
	#include "Tracer.h"
	#include <map>
	#ifdef WIN32
		#include <windows.h>
	#else
	#endif


	#define MP4PL_RTSPREQUESTLENGTH						(1024 * 4) + 1
	#define MP4PL_CONTROLVALUELENGTH					128 + 1
	#define MP4PL_SESSIONLENGTH							128 + 1
	#define MP4PL_MAXLONGLENGTH							128 + 1
	#define MP4PL_MAXRTCPPACKETLENGTH					1024 * 2
	#define MP4PL_MAXRTPPACKETLENGTH					1024 * 2
	#define MP4PL_MAXRTPINFOFIELDLENGTH					1024 + 1
	#define MP4PL_MAXSTATISTICSLENGTH					(1024 * 2) + 1
	#define MP4PL_MAXPATHNAMELENGTH						1024 + 1
	#define MP4PL_MAXDATETIMELENGTH						128 + 1

	#define MP4PL_MAXSECONDSTOWAITRTPPACKETS			10


	typedef class RTSPClientSession: public PosixThread
	{
		private:
			typedef struct RTPPacketInfo
			{
				unsigned long			_ulLength;
				unsigned long			_ulTimeStamp;
				unsigned char			*_pucRTPPacket;
			} RTPPacketInfo_t, *RTPPacketInfo_p;

			struct ltstr
			{
				bool operator() (
					unsigned long ulSequenceNumber1,
					unsigned long ulSequenceNumber2) const
				{
					return ulSequenceNumber1 < ulSequenceNumber2;
				}
			};

			typedef std:: multimap<unsigned long, RTPPacketInfo_p,
				ltstr> RTPPacketsMultiMap_t, *RTPPacketsMultiMap_p;

		private:
			unsigned long			_ulRTSPSessionIdentifier;
			long					_lEndOfTheStreamingInSeconds;
			Tracer_p				_ptTracer;
			Buffer_t				_bRTSPBuffer;
			Boolean_t				_bTraceOnTTY;
			PMutex_t				_mtRTSPSession;

			ClientSocket_t			_csClientSocket;
			SocketImpl_p			_psiClientRTSPSocketImpl;
			ServerSocket_t			_sVideoRTPServerSocket;
			SocketImpl_p			_psiVideoRTPServerSocketImpl;
			ServerSocket_t			_sVideoRTCPServerSocket;
			SocketImpl_p			_psiVideoRTCPServerSocketImpl;
			ServerSocket_t			_sAudioRTPServerSocket;
			SocketImpl_p			_psiAudioRTPServerSocketImpl;
			ServerSocket_t			_sAudioRTCPServerSocket;
			SocketImpl_p			_psiAudioRTCPServerSocketImpl;
			unsigned long			_ulCurrentRTSPSequenceNumber;

			char					*_pURL;
			Boolean_t				_bRedirectionPerformed;
			Buffer_t				_bRedirectedURL;
			Buffer_t				_bSDP;
			Buffer_t				_bSession;
			char					_pRTSPServerIPAddress [
				SCK_MAXIPADDRESSLENGTH];
			long					_lRTSPServerPort;
			char					_pControlValue [
				MP4PL_CONTROLVALUELENGTH];

			Boolean_t				_bIsThereVideo;
			char					_pVideoControlValue [
				MP4PL_CONTROLVALUELENGTH];
			unsigned long			_ulVideoClientRTPPort;
			unsigned long			_ulVideoClientRTCPPort;
			unsigned long			_ulVideoServerRTPPort;
			unsigned long			_ulVideoServerRTCPPort;

			Boolean_t				_bIsThereAudio;
			char					_pAudioControlValue [
				MP4PL_CONTROLVALUELENGTH];
			unsigned long			_ulAudioClientRTPPort;
			unsigned long			_ulAudioClientRTCPPort;
			unsigned long			_ulAudioServerRTPPort;
			unsigned long			_ulAudioServerRTCPPort;

			unsigned long			_ulFirstVideoSequenceNumber;
			unsigned long			_ulLastVideoSequenceNumber;
			unsigned long			_ulFirstVideoTimeStamp;
			unsigned long			_ulLastVideoTimeStamp;
			unsigned long			_ulFirstAudioSequenceNumber;
			unsigned long			_ulLastAudioSequenceNumber;
			unsigned long			_ulFirstAudioTimeStamp;
			unsigned long			_ulLastAudioTimeStamp;

			unsigned long			_ulCurrentLostVideoPacketsNumber;
			unsigned long			_ulCurrentLostAudioPacketsNumber;

			unsigned long			_ulTotalVideoBitstreamSize;
			unsigned long			_ulTotalAudioBitstreamSize;

			#ifdef WIN32
				__int64			_ullUTCInMilliSecsOfTheLastRTPPacketReceived;
			#else
				unsigned long long
					_ullUTCInMilliSecsOfTheLastRTPPacketReceived;
			#endif

			RTPPacketsMultiMap_t	_rmmVideoRTPPacketsSet;
			RTPPacketsMultiMap_t	_rmmAudioRTPPacketsSet;

		/*
		#ifdef WIN32
			Error getRTPackets (unsigned long ulRTSPSessionIdentifier,
				DWORD *ptvTimeOfFirstRTPPacket,
				unsigned long ulStartVideoSequenceNumber,
				unsigned long ulStartVideoTimeStamp,
				unsigned long ulStartAudioSequenceNumber,
				unsigned long ulStartAudioTimeStamp,
				Boolean_t bIsThereVideo, ServerSocket_p psVideoRTPServerSocket,
				ServerSocket_p psVideoRTCPServerSocket,
				Boolean_t bIsThereAudio, ServerSocket_p psAudioRTPServerSocket,
				ServerSocket_p psAudioRTCPServerSocket,
				Boolean_t bTraceOnTTY, Boolean_t bBitstreamFile);
		#else
			Error getRTPackets (unsigned long ulRTSPSessionIdentifier,
				timeval *ptvTimeOfFirstRTPPacket,
				unsigned long ulStartVideoSequenceNumber,
				unsigned long ulStartVideoTimeStamp,
				unsigned long ulStartAudioSequenceNumber,
				unsigned long ulStartAudioTimeStamp,
				Boolean_t bIsThereVideo, ServerSocket_p psVideoRTPServerSocket,
				ServerSocket_p psVideoRTCPServerSocket,
				Boolean_t bIsThereAudio, ServerSocket_p psAudioRTPServerSocket,
				ServerSocket_p psAudioRTCPServerSocket,
				Boolean_t bTraceOnTTY, Boolean_t bBitstreamFile);
		#endif
		*/

		protected:
			virtual Error run (void);


		public:
			RTSPClientSession (void);

			~RTSPClientSession (void);

			Error init (const char *pURL, unsigned long ulRTSPSessionIdentifier,
				long lEndOfTheStreamingInSeconds,
				unsigned long ulVideoClientRTPPort,
				unsigned long ulVideoClientRTCPPort,
				unsigned long ulAudioClientRTPPort,
				unsigned long ulAudioClientRTCPPort,
				Boolean_t bTraceOnTTY, Tracer_p ptTracer);

			Error finish (Boolean_t bFinishRTSPHandshake);

			Error getRTSPSessionIdentifier (
				unsigned long *pulRTSPSessionIdentifier);

			Error getStatisticsInformation (
				unsigned long *pulVideoPacketsNumberReceived,
				unsigned long *pulAudioPacketsNumberReceived,
				unsigned long *pulLostVideoPacketsNumber,
				unsigned long *pulLostAudioPacketsNumber);

			Error initialRTSPHandshake (
				Boolean_p pbIsThereVideo,
				ServerSocket_p *psVideoRTPServerSocket,
				ServerSocket_p *psVideoRTCPServerSocket,
				Boolean_p pbIsThereAudio,
				ServerSocket_p *psAudioRTPServerSocket,
				ServerSocket_p *psAudioRTCPServerSocket);

			Error finalRTSPHandshake (void);

			Error processReceivingVideoRTPPacket (void);

			Error processReceivingVideoRTCPPacket (void);

			Error processReceivingAudioRTPPacket (void);

			Error processReceivingAudioRTCPPacket (void);

			Error saveBitStreams (void);

	} RTSPClientSession_t, *RTSPClientSession_p;

#endif

