
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
	MP4Atom.cpp
	MP4BytesProperty.cpp
	MP4Co64Atom.cpp
	MP4ContentClassificationDescr.cpp
	MP4CprtAtom.cpp
	MP4CttsAtom.cpp
	MP4D263Atom.cpp
	MP4DamrAtom.cpp
	MP4DecoderConfigDescr.cpp
	MP4DecoderSpecificDescr.cpp
	MP4Descr.cpp
	MP4DimmAtom.cpp
	MP4DinfAtom.cpp
	MP4DmaxAtom.cpp
	MP4DmedAtom.cpp
	MP4DpndAtom.cpp
	MP4DrefAtom.cpp
	MP4DrepAtom.cpp
	MP4EdtsAtom.cpp
	MP4ElstAtom.cpp
	MP4ESDescr.cpp
	MP4EsdsAtom.cpp
	MP4EsIdIncDescr.cpp
	MP4ExtensionDescr.cpp
	MP4File.cpp
	MP4FileErrors.cpp
	MP4FileFactory.cpp
	MP4FileMessages.cpp
	MP4Float16BitsProperty.cpp
	MP4Float32BitsProperty.cpp
	MP4FreeAtom.cpp
	MP4FtypAtom.cpp
	MP4HdlrAtom.cpp
	MP4HinfAtom.cpp
	MP4HintAtom.cpp
	MP4HmhdAtom.cpp
	MP4HntiAtom.cpp
	MP4InitialObjectDescr.cpp
	MP4IodsAtom.cpp
	MP4IpirAtom.cpp
	MP4MaxrAtom.cpp
	MP4MdatAtom.cpp
	MP4MdhdAtom.cpp
	MP4MdiaAtom.cpp
	MP4MfhdAtom.cpp
	MP4MinfAtom.cpp
	MP4MoofAtom.cpp
	MP4MoovAtom.cpp
	MP4Mp4aAtom.cpp
	MP4Mp4sAtom.cpp
	MP4Mp4vAtom.cpp
	MP4MpodAtom.cpp
	MP4MvexAtom.cpp
	MP4MvhdAtom.cpp
	MP4NmhdAtom.cpp
	MP4NumpAtom.cpp
	MP4ObjectDescr.cpp
	MP4PaytAtom.cpp
	MP4PmaxAtom.cpp
	MP4Property.cpp
	MP4RatingDescr.cpp
	MP4RootAtom.cpp
	MP4RtpAtom.cpp
	MP4S263Atom.cpp
	MP4SamrAtom.cpp
	MP4SawbAtom.cpp
	MP4SdpAtom.cpp
	MP4SkipAtom.cpp
	MP4SmhdAtom.cpp
	MP4SnroAtom.cpp
	MP4StblAtom.cpp
	MP4StcoAtom.cpp
	MP4StdpAtom.cpp
	MP4StringCountedProperty.cpp
	MP4StringFixedProperty.cpp
	MP4StringLastProperty.cpp
	MP4StringNullTerminatedProperty.cpp
	MP4StscAtom.cpp
	MP4StsdAtom.cpp
	MP4StshAtom.cpp
	MP4StssAtom.cpp
	MP4StszAtom.cpp
	MP4SttsAtom.cpp
	MP4SyncAtom.cpp
	MP4SyncLayerConfigDescr.cpp
	MP4TfhdAtom.cpp
	MP4TimsAtom.cpp
	MP4TkhdAtom.cpp
	MP4TmaxAtom.cpp
	MP4TminAtom.cpp
	MP4TpylAtom.cpp
	MP4TrackInfo.cpp
	MP4TrafAtom.cpp
	MP4TrakAtom.cpp
	MP4TrefAtom.cpp
	MP4TrexAtom.cpp
	MP4TrpyAtom.cpp
	MP4TrunAtom.cpp
	MP4TsroAtom.cpp
	MP4UdtaAtom.cpp
	MP4UInteger16BitsProperty.cpp
	MP4UInteger24BitsProperty.cpp
	MP4UInteger32BitsProperty.cpp
	MP4UInteger64BitsProperty.cpp
	MP4UInteger8BitsProperty.cpp
	MP4UnknAtom.cpp
	MP4UnknownDescr.cpp
	MP4UnknownOCIDescr.cpp
	MP4UrlAtom.cpp
	MP4UrnAtom.cpp
	MP4Utility.cpp
	MP4UuidAtom.cpp
	MP4VmhdAtom.cpp
	MP4WmfsAtom.cpp
)
SET (HEADERS
	MP4Atom.h
	MP4BytesProperty.h
	MP4Co64Atom.h
	MP4ContentClassificationDescr.h
	MP4CprtAtom.h
	MP4CttsAtom.h
	MP4D263Atom.h
	MP4DamrAtom.h
	MP4DecoderConfigDescr.h
	MP4DecoderSpecificDescr.h
	MP4Descr.h
	MP4DimmAtom.h
	MP4DinfAtom.h
	MP4DmaxAtom.h
	MP4DmedAtom.h
	MP4DpndAtom.h
	MP4DrefAtom.h
	MP4DrepAtom.h
	MP4EdtsAtom.h
	MP4ElstAtom.h
	MP4ESDescr.h
	MP4EsdsAtom.h
	MP4EsIdIncDescr.h
	MP4ExtensionDescr.h
	MP4FileErrors.h
	MP4FileFactory.h
	MP4File.h
	MP4FileMessages.h
	MP4Float16BitsProperty.h
	MP4Float32BitsProperty.h
	MP4FreeAtom.h
	MP4FtypAtom.h
	MP4HdlrAtom.h
	MP4HinfAtom.h
	MP4HintAtom.h
	MP4HmhdAtom.h
	MP4HntiAtom.h
	MP4InitialObjectDescr.h
	MP4IodsAtom.h
	MP4IpirAtom.h
	MP4MaxrAtom.h
	MP4MdatAtom.h
	MP4MdhdAtom.h
	MP4MdiaAtom.h
	MP4MfhdAtom.h
	MP4MinfAtom.h
	MP4MoofAtom.h
	MP4MoovAtom.h
	MP4Mp4aAtom.h
	MP4Mp4sAtom.h
	MP4Mp4vAtom.h
	MP4MpodAtom.h
	MP4MvexAtom.h
	MP4MvhdAtom.h
	MP4NmhdAtom.h
	MP4NumpAtom.h
	MP4ObjectDescr.h
	MP4PaytAtom.h
	MP4PmaxAtom.h
	MP4Property.h
	MP4RatingDescr.h
	MP4RootAtom.h
	MP4RtpAtom.h
	MP4S263Atom.h
	MP4SamrAtom.h
	MP4SawbAtom.h
	MP4SdpAtom.h
	MP4SkipAtom.h
	MP4SmhdAtom.h
	MP4SnroAtom.h
	MP4StblAtom.h
	MP4StcoAtom.h
	MP4StdpAtom.h
	MP4StringCountedProperty.h
	MP4StringFixedProperty.h
	MP4StringLastProperty.h
	MP4StringNullTerminatedProperty.h
	MP4StscAtom.h
	MP4StsdAtom.h
	MP4StshAtom.h
	MP4StssAtom.h
	MP4StszAtom.h
	MP4SttsAtom.h
	MP4SyncAtom.h
	MP4SyncLayerConfigDescr.h
	MP4TfhdAtom.h
	MP4TimsAtom.h
	MP4TkhdAtom.h
	MP4TmaxAtom.h
	MP4TminAtom.h
	MP4TpylAtom.h
	MP4TrackInfo.h
	MP4TrafAtom.h
	MP4TrakAtom.h
	MP4TrefAtom.h
	MP4TrexAtom.h
	MP4TrpyAtom.h
	MP4TrunAtom.h
	MP4TsroAtom.h
	MP4UdtaAtom.h
	MP4UInteger16BitsProperty.h
	MP4UInteger24BitsProperty.h
	MP4UInteger32BitsProperty.h
	MP4UInteger64BitsProperty.h
	MP4UInteger8BitsProperty.h
	MP4UnknAtom.h
	MP4UnknownDescr.h
	MP4UnknownOCIDescr.h
	MP4UrlAtom.h
	MP4UrnAtom.h
	MP4Utility.h
	MP4UuidAtom.h
	MP4VmhdAtom.h
	MP4WmfsAtom.h
)

include_directories ("${CATRALIBRARIES_INCLUDE_DIR}")
add_compile_options(-D_REENTRANT)

add_library (MP4File SHARED ${SOURCES} ${HEADERS})

if (APPLE)
	target_link_libraries (MP4File ${ERRMSG_LIBRARY})
	target_link_libraries (MP4File ${TOOLS_LIBRARY})
	target_link_libraries (MP4File ${PTHREAD_LIBRARY})
	target_link_libraries (MP4File ${TRACER_LIBRARY})
endif()

install (TARGETS MP4File DESTINATION lib)
install (FILES ${HEADERS} DESTINATION include)

