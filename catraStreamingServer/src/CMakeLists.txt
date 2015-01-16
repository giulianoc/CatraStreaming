
# Copyright (C) Giuliano Catrambone (giuliano.catrambone@catrasoftware.it)

# This program is free software; you can redistribute it and/or 
# modify it under the terms of the GNU General Public License 
# as published by the Free Software Foundation; either 
# version 2 of the License, or (at your option) any later 
# version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# Commercial use other than under the terms of the GNU General Public
# License is allowed only after express negotiation of conditions
# with the authors.

SET (SOURCES
	BeginningHookHttpGetThread.cpp
	catraStreamingServer.cpp
	CheckReceivedRTCPPacketsTimes.cpp
	CheckServerSocketTimes.cpp
	CheckSocketsPoolTimes.cpp
	HttpStreamingServer.cpp
	LiveEvent.cpp
	LiveSource.cpp
	PauseTimes.cpp
	RTPOverBufferWindow2.cpp
	RTPOverBufferWindow.cpp
	RTSPConnectionEvent.cpp
	RTSP_RTCPTimes.cpp
	RTSPSession.cpp
	SendRTCPPacketsTimes.cpp
	StreamingServerErrors.cpp
	StreamingServerEventsSet.cpp
	StreamingServerMessages.cpp
	StreamingServerProcessor.cpp
	StreamingServerService.cpp
	StreamingServerSocketsPool.cpp
	StreamingSessionTimes.cpp
	SubscriberTracer.cpp
)

SET (HEADERS
	BeginningHookHttpGetThread.h
	CheckReceivedRTCPPacketsTimes.h
	CheckServerSocketTimes.h
	CheckSocketsPoolTimes.h
	HttpStreamingServer.h
	LiveEvent.h
	LiveSource.h
	PauseTimes.h
	RTPOverBufferWindow2.h
	RTPOverBufferWindow.h
	RTSPConnectionEvent.h
	RTSP_RTCPTimes.h
	RTSPSession.h
	SendRTCPPacketsTimes.h
	StreamingServerErrors.h
	StreamingServerEventsSet.h
	StreamingServerMessages.h
	StreamingServerProcessor.h
	StreamingServerService.h
	StreamingServerSocketsPool.h
	StreamingSessionTimes.h
	SubscriberTracer.h
)

include_directories ("${CATRALIBRARIES_INCLUDE_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}/RTCP/src")
include_directories ("${PROJECT_SOURCE_DIR}/RTSP/src")
include_directories ("${PROJECT_SOURCE_DIR}/RTP/src")
include_directories ("${PROJECT_SOURCE_DIR}/SDP/src")
include_directories ("${PROJECT_SOURCE_DIR}/MP4File/src")
if(CRYPTO_FOUND)
	include_directories(${CRYPTO_INCLUDE_DIR})
endif()
if (ZLIB_FOUND)
	include_directories(${ZLIB_INCLUDE_DIRS})
endif()

#add_compile_options(-D_REENTRANT -DRTPOVERBUFFERWINDOW2 -DTRIAL_VERSION -pg)
add_compile_options(-D_REENTRANT -DRTPOVERBUFFERWINDOW2)

add_executable(catraStreamingServer ${SOURCES})

target_link_libraries (catraStreamingServer RTCP)
target_link_libraries (catraStreamingServer RTSP)
target_link_libraries (catraStreamingServer RTP)
target_link_libraries (catraStreamingServer SDP)
target_link_libraries (catraStreamingServer MP4File)
target_link_libraries (catraStreamingServer ConfigurationFile)
target_link_libraries (catraStreamingServer Configuration)
target_link_libraries (catraStreamingServer Scheduler)
target_link_libraries (catraStreamingServer EventsSet)
target_link_libraries (catraStreamingServer Tracer)
target_link_libraries (catraStreamingServer Socket)
target_link_libraries (catraStreamingServer Tools)
target_link_libraries (catraStreamingServer WebTools)
target_link_libraries (catraStreamingServer PThread)
target_link_libraries (catraStreamingServer ErrMsg)
if(CRYPTO_FOUND)
	if (APPLE)
		target_link_libraries (catraStreamingServer ${CRYPTO_LIBRARIES})
	else()
		target_link_libraries (catraStreamingServer crypt)
	endif()
endif()
if (ZLIB_FOUND)
	target_link_libraries(catraStreamingServer ${ZLIB_LIBRARIES})
endif()
target_link_libraries (catraStreamingServer pthread)

