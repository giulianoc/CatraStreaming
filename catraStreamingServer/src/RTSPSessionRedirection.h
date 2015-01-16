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


#ifndef RTSPSessionRedirection_h
	#define RTSPSessionRedirection_h

	#ifdef WIN32
	#else
		#include "CatraStreaming_OSConfig.h"
	#endif

	#include "RTSPSession.h"

	#ifdef HAVE_CORBA_H
		#include "StreamingIDL.h"
	#endif

	#define SS_MAXSTREAMINGNAMELENGTH			256 + 1


	typedef class RTSPSessionRedirection: public RTSPSession {

		public:
			typedef struct StreamingServerInfo {
				Boolean_t			_bIsActive;
				char				_pName [SS_MAXSTREAMINGNAMELENGTH];
				char				_pIpAddress [SCK_MAXIPADDRESSLENGTH];
				unsigned long		_ulPort;
			} StreamingServerInfo_t, *StreamingServerInfo_p;

		private:
			unsigned long				_ulServersNumber;
			unsigned long				_ulMaxRTSPServerSessions;
			unsigned long				_ulMaxServerBandWidthInKbps;
			StreamingServerInfo_p		_pssiStreamingServerInfo;
			CORBA:: ORB_ptr				_porb;

		private:
			//	<PRE>
			//
			//	</PRE>
			Error processRTSPRequest (const char *pRTSPRequest);

			//	<PRE>
			//
			//	</PRE>
			Error handleDESCRIBEMethod (const char *pRTSPRequest,
				unsigned long *pulMethodLength);

			//	<PRE>
			//
			//	</PRE>
			Error handleSETUPMethod (const char *pRTSPRequest,
				unsigned long *pulMethodLength);

			//	<PRE>
			//
			//	</PRE>
			Error choiceStreamingServerToRedirect (const char *pAssetPath,
				StreamingServerInfo_p *pssiChoiceStreamingServer);

		public:
			//	<PRE>
			//
			//	</PRE>
			RTSPSessionRedirection (void);

			//	<PRE>
			//
			//	</PRE>
			~RTSPSessionRedirection (void);

			//	<PRE>
			//
			//	</PRE>
			#if HAVE_CORBA_H
				Error init (
					CORBA:: ORB_ptr porb,
					unsigned long ulIdentifier,
					unsigned long ulServersNumber,
					StreamingServerInfo_p pssiStreamingServerInfo,
					unsigned long ulMaxRTSPServerSessions,
					unsigned long ulMaxServerBandWidthInKbps,
					const char *pContentRootPath,
					Tracer_p ptTracer);
			#else
				Error init (
					unsigned long ulIdentifier,
					unsigned long ulServersNumber,
					StreamingServerInfo_p pssiStreamingServerInfo,
					unsigned long ulMaxRTSPServerSessions,
					unsigned long ulMaxServerBandWidthInKbps,
					const char *pContentRootPath,
					Tracer_p ptTracer);
			#endif

			//	<PRE>
			//
			//	</PRE>
			Error finish (void);

	} RTSPSessionRedirection_t, *RTSPSessionRedirection_p;

#endif
