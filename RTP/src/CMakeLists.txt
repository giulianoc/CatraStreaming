
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
	aac.cpp
	mbs.cpp
	mpeg4.cpp
	rfc2429.cpp
	rfc3016.cpp
	rfc3267.cpp
	rfcisma.cpp
	RTPErrors.cpp
	RTPHintCreator.cpp
	RTPHintPacket.cpp
	RTPHintPacketData.cpp
	RTPHintPacketImmediateData.cpp
	RTPHintPacketSampleData.cpp
	RTPHintSample.cpp
	RTPMessages.cpp
	RTPPacket.cpp
	RTPStreamRealTimeInfo.cpp
	RTPUtility.cpp
)
SET (HEADERS
	aac.h
	mbs.h
	mpeg4.h
	rfc2429.h
	rfc3016.h
	rfc3267.h
	rfcisma.h
	RTPErrors.h
	RTPHintCreator.h
	RTPHintPacketData.h
	RTPHintPacket.h
	RTPHintPacketImmediateData.h
	RTPHintPacketSampleData.h
	RTPHintSample.h
	RTPMessages.h
	RTPPacket.h
	RTPStreamRealTimeInfo.h
	RTPUtility.h
)

include_directories ("${CATRALIBRARIES_INCLUDE_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}/MP4File/src")
include_directories ("${PROJECT_SOURCE_DIR}/SDP/src")
add_compile_options(-D_REENTRANT)

add_library (RTP SHARED ${SOURCES} ${HEADERS})

#if (APPLE)
#	target_link_libraries (SDP ${CATRALIBRARIES_LIBRARIES})
#endif()

install (TARGETS RTP DESTINATION lib)
install (FILES ${HEADERS} DESTINATION include)

