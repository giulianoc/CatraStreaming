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

#include "RTCPPacket.h"

#include <stdio.h>
#ifdef WIN32
	#include <winsock2.h>
#else
#endif


int RTCPPacket::GetVersion()
{
	unsigned long* theVersionPtr = (unsigned long*)&fReceiverPacketBuffer[kVersionOffset];
	unsigned long theVersion = ntohl(*theVersionPtr);
	return (int) ((theVersion  & kVersionMask) >> kVersionShift);
}

unsigned short RTCPPacket::GetHasPadding()
{
	unsigned long* theHasPaddingPtr = (unsigned long*)&fReceiverPacketBuffer[kHasPaddingOffset];
	unsigned long theHasPadding = ntohl(*theHasPaddingPtr);
	return (unsigned short) (theHasPadding & kHasPaddingMask);
}

int RTCPPacket::GetReportCount()
{
	unsigned long* theReportCountPtr = (unsigned long*)&fReceiverPacketBuffer[kReportCountOffset];
	unsigned long theReportCount = ntohl(*theReportCountPtr);
	return (int) ((theReportCount & kReportCountMask) >> kReportCountShift);
}

unsigned char  RTCPPacket::GetPacketType()
{
	unsigned long* thePacketTypePtr = (unsigned long*)&fReceiverPacketBuffer[kPacketTypeOffset];
	unsigned long thePacketType = ntohl(*thePacketTypePtr);
	return (unsigned char ) ((thePacketType & kPacketTypeMask) >> kPacketTypeShift);
}

unsigned short RTCPPacket::GetPacketLength()
{
	return (unsigned short) ( ntohl(*(unsigned long*)&fReceiverPacketBuffer[kPacketLengthOffset]) & kPacketLengthMask);
}

unsigned long RTCPPacket::GetPacketSSRC()
{
	return (unsigned long) ntohl(*(unsigned long*)&fReceiverPacketBuffer[kPacketSourceIDOffset]) ;
}

signed short RTCPPacket::GetHeader(){ return (signed short) ntohs(*(signed short*)&fReceiverPacketBuffer[0]) ;}

unsigned long RTCPReceiverPacket::GetReportSourceID(int inReportNum)
{
	return (unsigned long) ntohl(*(unsigned long*)&fRTCPReceiverReportArray[this->RecordOffset(inReportNum)+kReportSourceIDOffset]) ;
}

unsigned char  RTCPReceiverPacket::GetFractionLostPackets(int inReportNum)
{
	return (unsigned char ) ( (ntohl(*(unsigned long*)&fRTCPReceiverReportArray[this->RecordOffset(inReportNum)+kFractionLostOffset]) & kFractionLostMask) >> kFractionLostShift );
}


unsigned char  RTCPReceiverPacket::GetTotalLostPackets(int inReportNum)
{
	return (unsigned char ) ( ntohl(*(unsigned long*)&fRTCPReceiverReportArray[this->RecordOffset(inReportNum)+kTotalLostPacketsOffset]) & kTotalLostPacketsMask );
}


unsigned long RTCPReceiverPacket::GetHighestSeqNumReceived(int inReportNum)
{
	return (unsigned long) ntohl(*(unsigned long*)&fRTCPReceiverReportArray[this->RecordOffset(inReportNum)+kHighestSeqNumReceivedOffset]) ;
}

unsigned long RTCPReceiverPacket::GetJitter(int inReportNum)
{
	return (unsigned long) ntohl(*(unsigned long*)&fRTCPReceiverReportArray[this->RecordOffset(inReportNum)+kJitterOffset]) ;
}


unsigned long RTCPReceiverPacket::GetLastSenderReportTime(int inReportNum)
{
	return (unsigned long) ntohl(*(unsigned long*)&fRTCPReceiverReportArray[this->RecordOffset(inReportNum)+kLastSenderReportOffset]) ;
}


unsigned long RTCPReceiverPacket::GetLastSenderReportDelay(int inReportNum)
{
	return (unsigned long) ntohl(*(unsigned long*)&fRTCPReceiverReportArray[this->RecordOffset(inReportNum)+kLastSenderReportDelayOffset]) ;
}

//returns true if successful, false otherwise
unsigned short RTCPPacket::ParsePacket(unsigned char * inPacketBuffer, unsigned long inPacketLen)
{
	if (inPacketLen < kRTCPPacketSizeInBytes)
		return false;
	fReceiverPacketBuffer = inPacketBuffer;

	//the length of this packet can be no less than the advertised length (which is
	//in 32-bit words, so we must multiply) plus the size of the header (4 bytes)
	if (inPacketLen < (unsigned long)((this->GetPacketLength() * 4) + kRTCPHeaderSizeInBytes))
		return false;
	
	//do some basic validation on the packet
	if (this->GetVersion() != kSupportedRTCPVersion)
		return false;
		
	return true;
}

#ifdef DEBUG_RTCP_PACKETS
void RTCPReceiverPacket::Dump()//Override
{
	RTCPPacket::Dump();
	
	for (int i = 0;i<this->GetReportCount(); i++)
	{
		printf( "	[%d] rptSrcID==%lu, fracLost==%d, totLost==%d, highSeq#==%lu\n"
				"        jit==%lu, lastSRTime==%lu, lastSRDelay==%lu \n",
	                         i,
	                         this->GetReportSourceID(i),
	                         this->GetFractionLostPackets(i),
	                         this->GetTotalLostPackets(i),
	                         this->GetHighestSeqNumReceived(i),
	                         this->GetJitter(i),
	                         this->GetLastSenderReportTime(i),
	                         this->GetLastSenderReportDelay(i) );
	}


}
#endif


unsigned short RTCPReceiverPacket::ParseReceiverReport(unsigned char * inPacketBuffer, unsigned long inPacketLength)
{
	unsigned short ok = this->ParsePacket(inPacketBuffer, inPacketLength);
	if (!ok)
		return false;
	
	fRTCPReceiverReportArray = inPacketBuffer + kRTCPPacketSizeInBytes;
	
	//this is the maximum number of reports there could possibly be
	int theNumReports = (inPacketLength - kRTCPPacketSizeInBytes) / kReportBlockOffsetSizeInBytes;

	//if the number of receiver reports is greater than the theoretical limit, return an error.
	if (this->GetReportCount() > theNumReports)
		return false;
		
	return true;
}

unsigned long RTCPReceiverPacket::GetCumulativeFractionLostPackets()
{
	float avgFractionLost = 0;
	for (short i = 0; i < this->GetReportCount(); i++)
	{
		avgFractionLost += this->GetFractionLostPackets(i);
		avgFractionLost /= (i+1);
	}
	
	return (unsigned long)avgFractionLost;
}


unsigned long RTCPReceiverPacket::GetCumulativeJitter()
{
	float avgJitter = 0;
	for (short i = 0; i < this->GetReportCount(); i++)
	{
		avgJitter += this->GetJitter(i);
		avgJitter /= (i + 1);
	}
	
	return (unsigned long)avgJitter;
}


unsigned long RTCPReceiverPacket::GetCumulativeTotalLostPackets()
{
	unsigned long totalLostPackets = 0;
	for (short i = 0; i < this->GetReportCount(); i++)
	{
		totalLostPackets += this->GetTotalLostPackets(i);
	}
	
	return totalLostPackets;
}


int RTCPReceiverPacket::RecordOffset(int inReportNum) 
{
	return inReportNum*kReportBlockOffsetSizeInBytes;
}	



#ifdef DEBUG_RTCP_PACKETS
void RTCPPacket::Dump()
{
	
	printf("\n--\n");
	
	printf( "vers==%d, pad==%d, ReportCount==%d, type==%d, length==%d, sourceID==%ld\n",
			 this->GetVersion(),
             (int)this->GetHasPadding(),
             this->GetReportCount(),
             (int)this->GetPacketType(),
             (int)this->GetPacketLength(),
             this->GetPacketSourceID() );
}
#endif


