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


#ifndef StreamingServerSocketsPool_h
	#define StreamingServerSocketsPool_h

	#include "SocketsPool.h"
	#include "ServerSocket.h"
	#include "StreamingServerEventsSet.h"
	#include "Tracer.h"

	#define SS_STREAMINGSERVERSOCKETSPOOL_MAXSOCKETSNUMBER		1024 * 3

	#define SS_STREAMINGSERVERPROCESSOR_DESTINATION		"StreamingServerProcessor"
	#define SS_STREAMINGSERVERSOCKETSPOOL_SOURCE		"StreamingServerSocketsPool"

	#define SS_STREAMINGSERVERSOCKETPOOL_STREAMINGSERVERSOCKET		1
	#define SS_STREAMINGSERVERSOCKETPOOL_RTSPSESSIONSOCKET			2
	#define SS_STREAMINGSERVERSOCKETPOOL_LIVESERVERSOCKET			3


	typedef class StreamingServerSocketsPool: public SocketsPool

	{
		private:
			StreamingServerEventsSet_p	_pesEventsSet;
			Tracer_p					_ptSystemTracer;

		protected:
			StreamingServerSocketsPool (const StreamingServerSocketsPool &t);

			virtual Error updateSocketStatus (Socket_p pSocket,
				long lSocketType, void *pvSocketData,
				unsigned short usSocketCheckType);

		public:
			StreamingServerSocketsPool (void);

			~StreamingServerSocketsPool (void);        

			Error init (
				StreamingServerEventsSet_p pesEventsSet,
				Tracer_p ptTracer);

			Error finish (void);

	} StreamingServerSocketsPool_t, *StreamingServerSocketsPool_p;

#endif
