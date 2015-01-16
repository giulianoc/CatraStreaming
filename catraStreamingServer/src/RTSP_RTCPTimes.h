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


#ifndef RTSP_RTCPTimes_h
	#define RTSP_RTCPTimes_h

	#include "Times.h"
	#include "StreamingServerEventsSet.h"
	#include "Tracer.h"

	#define SS_RTSP_RTCPTIMES_CLASSNAME				"RTSP_RTCPTime"
	#define SS_RTSP_RTCPTIMES_SOURCE				"Scheduler"
	#define SS_STREAMINGSERVERPROCESSOR_DESTINATION	"StreamingServerProcessor"


	typedef class RTSP_RTCPTimes: public Times

	{
		protected:
			unsigned long			_ulRTSPSessionIdentifier;
			StreamingServerEventsSet_p				_pesEventsSet;
			Tracer_p				_ptSystemTracer;

			RTSP_RTCPTimes (const RTSP_RTCPTimes &t);

		public:
			RTSP_RTCPTimes (void);

			virtual ~RTSP_RTCPTimes (void);

			Error init (
				unsigned long ulPeriodInMilliSecs,
				unsigned long ulRTSPSessionIdentifier,
				StreamingServerEventsSet_p pesEventsSet, Tracer_p ptTracer);

			virtual Error finish (void);

			virtual Error handleTimeOut (void);

	} RTSP_RTCPTimes_t, *RTSP_RTCPTimes_p;

#endif

