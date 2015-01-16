
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
	catraMonitoringPlayer.cpp
	CatraMonitoringPlayerErrors.cpp
	CatraMonitoringPlayerMessages.cpp
	RTSPClientSession.cpp
)

SET (HEADERS
	CatraMonitoringPlayerErrors.h
	CatraMonitoringPlayerMessages.h
	RTSPClientSession.h
)

include_directories ("${CATRALIBRARIES_INCLUDE_DIR}")
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

add_compile_options(-D_REENTRANT)

add_executable(catraMonitoringPlayer ${SOURCES})

target_link_libraries (catraMonitoringPlayer RTSP)
target_link_libraries (catraMonitoringPlayer RTP)
target_link_libraries (catraMonitoringPlayer SDP)
target_link_libraries (catraMonitoringPlayer MP4File)
target_link_libraries (catraMonitoringPlayer Tracer)
target_link_libraries (catraMonitoringPlayer Socket)
target_link_libraries (catraMonitoringPlayer Tools)
target_link_libraries (catraMonitoringPlayer WebTools)
target_link_libraries (catraMonitoringPlayer PThread)
target_link_libraries (catraMonitoringPlayer ErrMsg)
if(CRYPTO_FOUND)
	if (APPLE)
		target_link_libraries (catraMonitoringPlayer ${CRYPTO_LIBRARIES})
	else()
		target_link_libraries (catraMonitoringPlayer crypt)
	endif()
endif()
if (ZLIB_FOUND)
	target_link_libraries(catraMonitoringPlayer ${ZLIB_LIBRARIES})
endif()
target_link_libraries (catraMonitoringPlayer pthread)

