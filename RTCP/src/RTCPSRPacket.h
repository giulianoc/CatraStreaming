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

#ifndef __RTCP_SR_PACKET__
#define __RTCP_SR_PACKET__

// #include "OSHeaders.h"
// #include "OS.h"

#ifdef WIN32
//	#include <winsock2.h>
#else
	#include <netinet/in.h> //definition of htonl
#endif

#define FOUR_CHARS_TO_INT( c1, c2, c3, c4 )  ( c1 << 24 | c2 << 16 | c3 << 8 | c4 )

class RTCPSRPacket
{
	public:
	
		enum
		{
			kSRPacketType = 200,	//UInt32
			kByePacketType = 203
		};

		RTCPSRPacket();
		~RTCPSRPacket() {}
		
		// ACCESSORS
		
		void*	GetSRPacket() 		{ return &fSenderReportBuffer[0]; }
		unsigned long	GetSRPacketLen() 	{ return fSenderReportWithServerInfoSize; }
		unsigned long	GetSRWithByePacketLen() { return fSenderReportWithServerInfoSize + kByeSizeInBytes; }
		
		void*	GetServerInfoPacket() { return &fSenderReportBuffer[fSenderReportSize]; }
		unsigned long	GetServerInfoPacketLen() { return kServerInfoSizeInBytes; }

		//
		// MODIFIERS
		
		//
		// FOR SR
		void	SetSSRC(unsigned long inSSRC);
		void	SetClientSSRC(unsigned long inClientSSRC);

		#ifdef WIN32
			__int64 HostToNetworkSInt64(__int64 hostOrdered);
			void	SetNTPTimestamp(__int64 inNTPTimestamp);
		#else
			long long HostToNetworkSInt64(long long hostOrdered);
			void	SetNTPTimestamp(long long inNTPTimestamp);
		#endif

		void	SetRTPTimestamp(unsigned long inRTPTimestamp);
		
		void	SetPacketCount(unsigned long inPacketCount);
		void	SetByteCount(unsigned long inByteCount);
		
		//
		// FOR SERVER INFO APP PACKET
		void	SetAckTimeout(unsigned long inAckTimeoutInMsec);

		//RTCP support requires generating unique CNames for each session.
		//This function generates a proper cName and returns its length. The buffer
		//passed in must be at least kMaxCNameLen
		enum
		{
			kMaxCNameLen = 60	//Uint32
		};
		static unsigned long			GetACName(char* ioCNameBuffer);

	private:
	
		enum
		{
			kSenderReportSizeInBytes = 36,
			kServerInfoSizeInBytes = 28,
			kByeSizeInBytes = 8
		};
		char		fSenderReportBuffer[kSenderReportSizeInBytes + kMaxCNameLen + kServerInfoSizeInBytes + kByeSizeInBytes];
		unsigned long		fSenderReportSize;
		unsigned long		fSenderReportWithServerInfoSize;

};



#endif //__RTCP_SR_PACKET__
