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


#ifndef CheckSocketsPoolTimes_h
	#define CheckSocketsPoolTimes_h

	#include "Times.h"
	#include "StreamingServerSocketsPool.h"
	#include "Tracer.h"

	#define SS_CHECKSOCKETSPOOLTIMES_PERIODINMILLISECS	200
	#define SS_CHECKSOCKETSPOOLTIMES_CLASSNAME			"CheckSocketsPoolTimes"
	#define SS_CHECKSOCKETSSTATUS_SECONDSTOWAIT			0
	#define SS_CHECKSOCKETSSTATUS_MICROSECONDSTOWAIT	400 * 1000


	typedef class CheckSocketsPoolTimes: public Times

	{
		protected:
			StreamingServerSocketsPool_p	_pspSocketsPool;
			Tracer_p						_ptSystemTracer;

			CheckSocketsPoolTimes (const CheckSocketsPoolTimes &t);

		public:
			CheckSocketsPoolTimes (void);

			virtual ~CheckSocketsPoolTimes (void);

			Error init (
				unsigned long ulPeriodInMilliSecs,
				StreamingServerSocketsPool_p pspSocketsPool,
				Tracer_p ptTracer);

			virtual Error finish (void);

			virtual Error handleTimeOut (void);

	} CheckSocketsPoolTimes_t, *CheckSocketsPoolTimes_p;

#endif

