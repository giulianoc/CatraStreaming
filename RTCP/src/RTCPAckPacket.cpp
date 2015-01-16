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


#include "RTCPAckPacket.h"
#include "RTCPPacket.h"

#ifdef WIN32
	#include <winsock2.h>
#else
#endif

#include <stdio.h>


unsigned short RTCPAckPacket::IsNthBitEnabled(unsigned long inBitNumber)
{
	// Don't need to do endian conversion because we're dealing with 8-bit numbers
	unsigned char  bitMask = 128;
	return *(fRTCPAckBuffer + kAckMaskOffset + (inBitNumber >> 3)) & (bitMask >>= inBitNumber & 7);
}



unsigned short RTCPAckPacket::ParseAckPacket(unsigned char * inPacketBuffer, unsigned long inPacketLen)
{
	fRTCPAckBuffer = inPacketBuffer;

	//
	// Check whether this is an ack packet or not.
	if ((inPacketLen < kAckMaskOffset) || (!this->IsAckPacketType()))
		return false;
	
	// Assert(inPacketLen == (unsigned long)((this->GetPacketLength() * 4)) + RTCPPacket::kRTCPHeaderSizeInBytes);
	fAckMaskSize = inPacketLen - kAckMaskOffset;
	return true;
}

unsigned short RTCPAckPacket::IsAckPacketType()
{
	// While we are moving to a new type, check for both
	unsigned long theAppType = ntohl(*(unsigned long*)&fRTCPAckBuffer[kAppPacketTypeOffset]);
	return this->IsAckType(theAppType);
}

unsigned short RTCPAckPacket::GetAckSeqNum()
{
	return (unsigned short) (ntohl(*(unsigned long*)&fRTCPAckBuffer[kAckSeqNumOffset]));
}

unsigned short RTCPAckPacket::GetPacketLength()
{
	return (unsigned short) ( ntohl(*(unsigned long*)fRTCPAckBuffer) & kPacketLengthMask);
}

