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


#ifndef MP4PlayerSocketsPool_h
	#define MP4PlayerSocketsPool_h

	#include "SocketsPool.h"
	// #include "ServerSocket.h"
	#include "Tracer.h"

	// #define SS_STREAMINGSERVERSOCKETSPOOL_MAXSOCKETSNUMBER		1024 * 3

	#define MP4PL_MP4PLAYERSOCKETSPOOL_VIDEORTPSERVERSOCKET			1
	#define MP4PL_MP4PLAYERSOCKETSPOOL_VIDEORTCPSERVERSOCKET		2
	#define MP4PL_MP4PLAYERSOCKETSPOOL_AUDIORTPSERVERSOCKET			3
	#define MP4PL_MP4PLAYERSOCKETSPOOL_AUDIORTCPSERVERSOCKET		4


	typedef class MP4PlayerSocketsPool: public SocketsPool

	{
		private:
			Tracer_p					_ptTracer;

		protected:
			MP4PlayerSocketsPool (const MP4PlayerSocketsPool &t);

			virtual Error updateSocketStatus (Socket_p pSocket,
				long lSocketType, void *pvSocketData,
				unsigned short usSocketCheckType);

		public:
			MP4PlayerSocketsPool (void);

			~MP4PlayerSocketsPool (void);        

			Error init (
				unsigned long ulMaxSocketsNumber,
				unsigned long ulCheckSocketsPoolPeriodInSeconds,
				unsigned long ulAdditionalCheckSocketsPoolPeriodInMicroseconds,
				Tracer_p ptTracer);

			Error finish (void);

	} MP4PlayerSocketsPool_t, *MP4PlayerSocketsPool_p;

#endif

