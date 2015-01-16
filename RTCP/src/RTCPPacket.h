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

//#define DEBUG_RTCP_PACKETS 1


#ifndef _RTCPPACKET_H_
#define _RTCPPACKET_H_

#include <stdlib.h>

#include <sys/types.h>
#ifdef WIN32
//	#include <winsock2.h>
#else
	#include <netinet/in.h>
#endif

// #include "OSHeaders.h"

class RTCPPacket 
{
public:

	// Packet types
	enum
	{
		kReceiverPacketType 	= 201,	//UInt32
		kSDESPacketType 		= 202,	//UInt32
		kAPPPacketType  		= 204	//UInt32
	};
	

	RTCPPacket() : fReceiverPacketBuffer(NULL) {}
	virtual ~RTCPPacket() {}

	//Call this before any accessor method. Returns true if successful, false otherwise
	unsigned short ParsePacket(unsigned char * inPacketBuffer, unsigned long inPacketLen);

	int GetVersion();
	unsigned short GetHasPadding();
	int GetReportCount();
	unsigned char  GetPacketType();
	unsigned short GetPacketLength();	//in 32-bit words
	unsigned long GetPacketSSRC();
	signed short GetHeader();
	unsigned char * GetPacketBuffer() { return fReceiverPacketBuffer; }
	
	unsigned short IsValidPacket();
	
	#ifdef DEBUG_RTCP_PACKETS
	virtual void Dump();
	#endif

	enum
	{
		kRTCPPacketSizeInBytes = 8,		//All are UInt32s
		kRTCPHeaderSizeInBytes = 4
	};
		
protected:
	
	unsigned char * fReceiverPacketBuffer;
	
	enum
	{
		kVersionOffset = 0,
			kVersionMask = 0xC0000000UL,
			kVersionShift = 30,
		kHasPaddingOffset = 0,
			kHasPaddingMask = 0x20000000UL,
		kReportCountOffset = 0,
			kReportCountMask = 0x1F000000UL,
			kReportCountShift = 24,
		kPacketTypeOffset = 0,
			kPacketTypeMask = 0x00FF0000UL,
			kPacketTypeShift = 16,
		kPacketLengthOffset = 0,
			kPacketLengthMask = 0x0000FFFFUL,
		kPacketSourceIDOffset = 4,	//packet sender SSRC
		kPacketSourceIDSize = 4,	//
		kSupportedRTCPVersion = 2
	};

};




class SourceDescriptionPacket : public RTCPPacket

{

public:
	
	SourceDescriptionPacket() : RTCPPacket() {}
	
	unsigned short ParseSourceDescription(unsigned char * inPacketBuffer, unsigned long inPacketLength)
							{ return ParsePacket(inPacketBuffer, inPacketLength); }

private:	
};




class RTCPReceiverPacket  : public RTCPPacket
{
public:

	RTCPReceiverPacket() : RTCPPacket(), fRTCPReceiverReportArray(NULL) {}

	//Call this before any accessor method. Returns true if successful, false otherwise
	unsigned short ParseReceiverReport(unsigned char * inPacketBuffer, unsigned long inPacketLength);

	unsigned long GetReportSourceID(int inReportNum);
	 unsigned char  GetFractionLostPackets(int inReportNum);
	 unsigned char  GetTotalLostPackets(int inReportNum);
	unsigned long GetHighestSeqNumReceived(int inReportNum);
	unsigned long GetJitter(int inReportNum);
	unsigned long GetLastSenderReportTime(int inReportNum);
	unsigned long GetLastSenderReportDelay(int inReportNum);	//expressed in units of 1/65536 seconds

	unsigned long GetCumulativeFractionLostPackets();
	unsigned long GetCumulativeTotalLostPackets();
	unsigned long GetCumulativeJitter();

	unsigned short IsValidPacket();
	
	#ifdef DEBUG_RTCP_PACKETS
	virtual void Dump(); //Override
	#endif
	
protected:
	int RecordOffset(int inReportNum);

	unsigned char * fRTCPReceiverReportArray;	//points into fReceiverPacketBuffer

	enum
	{
		kReportBlockOffsetSizeInBytes = 24,		//All are UInt32s

		kReportBlockOffset = kPacketSourceIDOffset + kPacketSourceIDSize,
	
		kReportSourceIDOffset = 0,	//SSRC for this report
		kFractionLostOffset = 4,
			kFractionLostMask = 0xFF000000UL,
			kFractionLostShift = 24,
		kTotalLostPacketsOffset = 4,
			kTotalLostPacketsMask = 0x00FFFFFFUL,
		kHighestSeqNumReceivedOffset = 8,
		kJitterOffset = 12,
		kLastSenderReportOffset = 16,
		kLastSenderReportDelayOffset = 20
	};

};

/**************  RTCPPacket  inlines **************/

/**************  RTCPReceiverPacket  inlines **************/

/*
Receiver Report
---------------
 0                   1                   2                   3
 0 0 0 1 1 1 1 1
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|V=2|P|    RC   |   PT=RR=201   |             length            | header
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                     SSRC of packet sender                     |
+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
|                 SSRC_1 (SSRC of first source)                 | report
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ block
| fraction lost |       cumulative number of packets lost       |   1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           extended highest sequence number received           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                      interarrival jitter                      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                         last SR (LSR)                         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                   delay since last SR (DLSR)                  |
+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
|                 SSRC_2 (SSRC of second source)                | report
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ block
:                               ...                             :   2
+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
|                  profile-specific extensions                  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+



*/

#endif //_RTCPPACKET_H_
