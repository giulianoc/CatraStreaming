/*
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 *
 */

#ifndef _RTCPACKPACKET_H_
#define _RTCPACKPACKET_H_

// #include "OSHeaders.h"
#include <stdlib.h>

#ifdef WIN32
//	#include <winsock2.h>
#else
	#include <netinet/in.h>
#endif

class RTCPAckPacket
{
	public:
	
/*

		RTCP app ACK packet

		# bytes   description
		-------   -----------
		4         rtcp header
		4         SSRC of receiver
		4         app type ('qtak')
		2         reserved (set to 0)
		2         seqNum

*/

		//
		// This class is not derived from RTCPPacket as a performance optimization.
		// Instead, it is assumed that the RTCP packet validation has already been
		// done.
		RTCPAckPacket() : fRTCPAckBuffer(NULL), fAckMaskSize(0) {}
		virtual ~RTCPAckPacket() {}
		
		// Returns true if this is an Ack packet, false otherwise.
		// Assumes that inPacketBuffer is a pointer to a valid RTCP packet header.
		unsigned short ParseAckPacket(unsigned char * inPacketBuffer, unsigned long inPacketLen);

		unsigned short GetAckSeqNum();
		unsigned long GetAckMaskSizeInBits() { return fAckMaskSize * 8; }
		unsigned short IsNthBitEnabled(unsigned long inBitNumber);
		unsigned short GetPacketLength();

	private:
	
		unsigned char * fRTCPAckBuffer;
		unsigned long fAckMaskSize;

		unsigned short IsAckPacketType();
		
		enum
		{	kAckPacketType = 'qtak',
			kOldAckPacketType = 'ack ', // Remove this
			kAppPacketTypeOffset 	= 8,
			kAckSeqNumOffset 		= 16,
			kAckMaskOffset 			= 20,
			kPacketLengthMask = 0x0000FFFFUL,
		};
		
		unsigned short IsOldAckType(unsigned long theAppType)  { return (theAppType == kOldAckPacketType); } // Remove this
		unsigned short IsAckType(unsigned long theAppType) { return (IsOldAckType(theAppType) || (theAppType == kAckPacketType) );}
};




/*
6.6 Ack Packet format

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P| subtype |   PT=APP=204  |             length            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           SSRC/CSRC                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                          name (ASCII)  = 'qtak'               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           SSRC/CSRC                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |          Reserved             |          Seq num              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        Mask...                                |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   
 */

#endif //_RTCPAPPPACKET_H_
