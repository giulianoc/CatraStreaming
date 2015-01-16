
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

cmake_minimum_required(
	VERSION 2.8.12
)

project(
	CatraStreaming
)

set(CatraStreaming_VERSION_MAJOR 1)
set(CatraStreaming_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/CatraStreamingConfig.h.in"
	"${PROJECT_BINARY_DIR}/CatraStreamingConfig.h"
)


# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(LibCatraLibraries)
if (NOT CATRALIBRARIES_FOUND)
	MESSAGE(FATAL_ERROR "The CatraLibraries were not found")
endif()

find_package(LibCrypto)
find_package(ZLIB)

add_subdirectory(SDP/src)
add_subdirectory(MP4File/src)
add_subdirectory(RTCP/src)
add_subdirectory(RTP/src)
add_subdirectory(RTSP/src)
add_subdirectory(catraStreamingServer/src)
add_subdirectory(catraMonitoringPlayer/src)

