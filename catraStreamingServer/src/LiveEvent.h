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

#ifndef LiveEvent_h
	#define LiveEvent_h

	#include "Event.h"
	#include "Tracer.h"
	#include "ServerSocket.h"
	#include "StreamingServerErrors.h"
	#include <vector>

	#define SS_EVENT_SENDLIVERTPPACKETS					7


	typedef class LiveEvent: public Event

	{
		/*
		public:
			typedef struct LiveSource
			{
				Boolean_t						_bVideoTrack;
				Boolean_t						_bAudioTrack;

				unsigned long					_ulVideoPort;
				ServerSocket_t					_ssLiveVideoServerSocket;

				unsigned long					_ulAudioPort;
				ServerSocket_t					_ssLiveAudioServerSocket;

				std:: vector<unsigned long>		_vActiveLiveRTSPSessions;
			} LiveSource_t, *LiveSource_p;
		*/

		private:
			ServerSocket_p		_pssServerSocket;
			void				*_pvLiveSource;

		protected:
			Tracer_p			_ptSystemTracer;


			LiveEvent (const LiveEvent &);

			LiveEvent &operator = (const LiveEvent &);


		public:
			LiveEvent (void);

			virtual ~LiveEvent (void);

			Error init (const char *pSource,
				long lTypeIdentifier,
				const char *pTypeIdentifier,
				Socket_p pssServerSocket,
				void *pvLiveSource,
				Tracer_p ptTracer);

			virtual Error finish (void);

			Error getLiveEventInfo (
				void **pvLiveSource,
				ServerSocket_p *pssServerSocket);

	} LiveEvent_t, *LiveEvent_p;

#endif
