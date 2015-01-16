
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
	SDP.cpp
	SDPErrors.cpp
	SDPFor3GPP.cpp
	SDPForRTSP.cpp
	SDPMedia.cpp
	SDPMediaFor3GPP.cpp
	SDPMediaForRTSP.cpp
	SDPTimeBase.cpp
	SDPTimeNpt.cpp
	SDPTimeSmpte.cpp
	SDPTimeUtc.cpp
)
SET (HEADERS
	SDPAttribute.h
	SDPErrors.h
	SDPFor3GPP.h
	SDPForRTSP.h
	SDP.h
	SDPMediaFor3GPP.h
	SDPMediaForRTSP.h
	SDPMedia.h
	SDPTimeBase.h
	SDPTimeNpt.h
	SDPTimeSmpte.h
	SDPTimeUtc.h
)

include_directories ("${CATRALIBRARIES_INCLUDE_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}/MP4File/src")
add_compile_options(-D_REENTRANT)

add_library (SDP SHARED ${SOURCES} ${HEADERS})

#if (APPLE)
#	target_link_libraries (SDP ${CATRALIBRARIES_LIBRARIES})
#endif()

install (TARGETS SDP DESTINATION lib)
install (FILES ${HEADERS} DESTINATION include)

