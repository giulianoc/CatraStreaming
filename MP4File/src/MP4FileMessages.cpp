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


#include "MP4FileMessages.h"


ErrMsgBase:: ErrMsgsInfo MP4FileMessagesStr = {

	// MP4File
	MP4F_MP4FILE_BUILDANDPREFETCHEDRTPPACKET,
		"Build and prefetched RTP packet. Track URI: %s, Relative transmission time: %lf, Sequence number: %ld, Marker bit: %s, Relative transmission time with time scale: %lu, RTP packet length: %llu",

	// MP4FileFactory

	// MP4Atom
	MP4F_MP4ATOM_FOUNDATOM,
		"Found the '%s' atom. Start offset: %ld, Size: %llu, Parent: '%s'",

	// MP4Descr
	MP4F_MP4ATOM_FOUNDDESCRATOM,
		"Found the '%s' descriptor atom. Start offset: %llu, Size: %llu, Number bytes for descriptor size: %ld, Parent: '%s'"

	// MP4RootAtom

	// MP4Property

	// common

	// Insert here other errors...

} ;

