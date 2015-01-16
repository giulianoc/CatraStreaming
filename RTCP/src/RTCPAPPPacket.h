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

#ifndef _RTCPAPPPACKET_H_
#define _RTCPAPPPACKET_H_

#include "RTCPPacket.h"

#define FOUR_CHARS_TO_INT( c1, c2, c3, c4 )  ( c1 << 24 | c2 << 16 | c3 << 8 | c4 )

#define TW0_CHARS_TO_INT( c1, c2 )  ( c1 << 8 | c2 )


/****** RTCPCompressedQTSSPacket is the packet type that the client actually sends ******/
class RTCPCompressedQTSSPacket : public RTCPPacket
{
public:

	RTCPCompressedQTSSPacket();
	virtual ~RTCPCompressedQTSSPacket() {}
	
	//Call this before any accessor method. Returns true if successful, false otherwise
	unsigned short ParseCompressedQTSSPacket(unsigned char * inPacketBuffer, unsigned long inPacketLength);

	unsigned long GetReportSourceID();
	unsigned short GetAppPacketVersion();
	unsigned short GetAppPacketLength();	//In 'UInt32's
	unsigned long GetAppPacketName();
	
	unsigned long GetReceiverBitRate() {return fReceiverBitRate;}
	unsigned short GetAverageLateMilliseconds()	{return fAverageLateMilliseconds;}
	unsigned short GetPercentPacketsLost()	{return fPercentPacketsLost;}
	unsigned short GetAverageBufferDelayMilliseconds()	{return fAverageBufferDelayMilliseconds;}
	unsigned short GetIsGettingBetter()	{return fIsGettingBetter;}
	unsigned short GetIsGettingWorse()	{return fIsGettingWorse;}
	unsigned long GetNumEyes()	{return fNumEyes;}
	unsigned long GetNumEyesActive()	{return fNumEyesActive;}
	unsigned long GetNumEyesPaused()	{return fNumEyesPaused;}
	unsigned long GetOverbufferWindowSize()	{return fOverbufferWindowSize;}
	
	//Proposed - are these there yet?
	unsigned long GetTotalPacketReceived()	{return fTotalPacketsReceived;}
	unsigned short GetTotalPacketsDropped()	{return fTotalPacketsDropped;}
	unsigned short GetTotalPacketsLost()	{return fTotalPacketsLost;}
	unsigned short GetClientBufferFill()	{return fClientBufferFill;}
	unsigned short GetFrameRate()	{return fFrameRate;}
	unsigned short GetExpectedFrameRate()	{return fExpectedFrameRate;}
	unsigned short GetAudioDryCount()	{return fAudioDryCount;}
	
	#ifdef DEBUG_RTCP_PACKETS
	virtual void Dump(); //Override
	char	mDumpArray[1024];
	#endif
	
	unsigned char * GetRTCPAPPDataBuffer()	{return fRTCPAPPDataBuffer;}

private:
	unsigned char * fRTCPAPPDataBuffer;	//points into fReceiverPacketBuffer

	void ParseAndStore();
	unsigned long GetUInt32FromMemory(unsigned long* inP);

	
	unsigned long fReceiverBitRate;
	unsigned short fAverageLateMilliseconds;
	unsigned short fPercentPacketsLost;
	unsigned short fAverageBufferDelayMilliseconds;
	unsigned short fIsGettingBetter;
	unsigned short fIsGettingWorse;
	unsigned long fNumEyes;
	unsigned long fNumEyesActive;
	unsigned long fNumEyesPaused;
	unsigned long fOverbufferWindowSize;
	
	//Proposed - are these there yet?
	unsigned long fTotalPacketsReceived;
	unsigned short fTotalPacketsDropped;
	unsigned short fTotalPacketsLost;
	unsigned short fClientBufferFill;
	unsigned short fFrameRate;
	unsigned short fExpectedFrameRate;
	unsigned short fAudioDryCount;
	
	enum
	{
		kAppNameOffset = 0,	//four App identifier				//All are UInt32
		kReportSourceIDOffset = 4,	//SSRC for this report
		kAppPacketVersionOffset = 8,
			kAppPacketVersionMask = 0xFFFF0000UL,
			kAppPacketVersionShift = 16,
		kAppPacketLengthOffset = 8,
			kAppPacketLengthMask = 0x0000FFFFUL,
		kQTSSDataOffset = 12,
	
	//Individual item offsets/masks
		kQTSSItemTypeOffset = 0,	//SSRC for this report
			kQTSSItemTypeMask = 0xFFFF0000UL,
			kQTSSItemTypeShift = 16,
		kQTSSItemVersionOffset = 0,
			kQTSSItemVersionMas = 0x0000FF00UL,
			kQTSSItemVersionShift = 8,
		kQTSSItemLengthOffset = 0,
			kQTSSItemLengthMask = 0x000000FFUL,
		kQTSSItemDataOffset = 4,
	
		kSupportedCompressedQTSSVersion = 0
	};
	
	//version we support currently


};

/****** RTCPqtssPacket is apparently no longer sent by the client ******/
class RTCPqtssPacket : public RTCPPacket
{
public:
	
	RTCPqtssPacket() : RTCPPacket(), fRTCPAPPDataBuffer(NULL) {}
	virtual ~RTCPqtssPacket() {}
	
	//Call this before any accessor method. Returns true if successful, false otherwise
	unsigned short ParseQTSSPacket(unsigned char * inPacketBuffer, unsigned long inPacketLength);

	unsigned long GetReportSourceID();
	unsigned short GetAppPacketVersion();
	unsigned short GetAppPacketLength();	//In 'UInt32's
	
	unsigned long GetReceiverBitRate() {return fReceiverBitRate;}
	unsigned long GetAverageLateMilliseconds()	{return fAverageLateMilliseconds;}
	unsigned long GetPercentPacketsLost()	{return fPercentPacketsLost;}
	unsigned long GetAverageBufferDelayMilliseconds()	{return fAverageBufferDelayMilliseconds;}
	unsigned short GetIsGettingBetter()	{return fIsGettingBetter;}
	unsigned short GetIsGettingWorse()	{return fIsGettingWorse;}
	unsigned long GetNumEyes()	{return fNumEyes;}
	unsigned long GetNumEyesActive()	{return fNumEyesActive;}
	unsigned long GetNumEyesPaused()	{return fNumEyesPaused;}
	
	//Proposed - are these there yet?
	unsigned long GetTotalPacketReceived()	{return fTotalPacketsReceived;}
	unsigned long GetTotalPacketsDropped()	{return fTotalPacketsDropped;}
	unsigned long GetClientBufferFill()	{return fClientBufferFill;}
	unsigned long GetFrameRate()	{return fFrameRate;}
	unsigned long GetExpectedFrameRate()	{return fExpectedFrameRate;}
	unsigned long GetAudioDryCount()	{return fAudioDryCount;}

	
private:
	unsigned char * fRTCPAPPDataBuffer;	//points into fReceiverPacketBuffer

	void ParseAndStore();

	unsigned long fReportSourceID;
	unsigned short fAppPacketVersion;
	unsigned short fAppPacketLength;	//In 'UInt32's
	
	unsigned long fReceiverBitRate;
	unsigned long fAverageLateMilliseconds;
	unsigned long fPercentPacketsLost;
	unsigned long fAverageBufferDelayMilliseconds;
	unsigned short fIsGettingBetter;
	unsigned short fIsGettingWorse;
	unsigned long fNumEyes;
	unsigned long fNumEyesActive;
	unsigned long fNumEyesPaused;
	
	//Proposed - are these there yet?
	unsigned long fTotalPacketsReceived;
	unsigned long fTotalPacketsDropped;
	unsigned long fClientBufferFill;
	unsigned long fFrameRate;
	unsigned long fExpectedFrameRate;
	unsigned long fAudioDryCount;
	
	enum
	{
		//THESE SHIFTS DO NOT WORK ON LITTLE-ENDIAN PLATFORMS! I HAVEN'T FIXED
		//THIS BECAUSE THIS PACKET IS NO LONGER USED...
		
		kAppNameOffset = 0,	//four App identifier			//All are UInt32s
		kReportSourceIDOffset = 4,	//SSRC for this report
		kAppPacketVersionOffset = 8,
			kAppPacketVersionMask = 0xFFFF0000UL,
			kAppPacketVersionShift = 16,
		kAppPacketLengthOffset = 8,
			kAppPacketLengthMask = 0x0000FFFFUL,
		kQTSSDataOffset = 12,
	
		//Individual item offsets/masks
		kQTSSItemTypeOffset = 0,	//SSRC for this report
		kQTSSItemVersionOffset = 4,
			kQTSSItemVersionMask = 0xFFFF0000UL,
			kQTSSItemVersionShift = 16,
		kQTSSItemLengthOffset = 4,
			kQTSSItemLengthMask = 0x0000FFFFUL,
		kQTSSItemDataOffset = 8,

		//version we support currently
		kSupportedQTSSVersion = 0
	};
	

};


/****************  RTCPCompressedQTSSPacket inlines *******************************/

/*
6.6 APP: Application-defined RTCP packet

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P| subtype |   PT=APP=204  |             length            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           SSRC/CSRC                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                          name (ASCII)                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                   application-dependent data                  |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   
 */

#endif //_RTCPAPPPACKET_H_
