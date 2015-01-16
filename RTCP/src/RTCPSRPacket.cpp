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

#include <string.h>
#ifdef WIN32
	#include <winsock2.h>
#else
	#include <sys/time.h>
#endif
#include "RTCPSRPacket.h"
#include <stdio.h>


void	RTCPSRPacket::SetSSRC(unsigned long inSSRC)
{
	// Set SSRC in SR
	((unsigned long*)&fSenderReportBuffer)[1] = htonl(inSSRC);
	
	// Set SSRC in SDES
	((unsigned long*)&fSenderReportBuffer)[8] = htonl(inSSRC);
	
	// Set SSRC in SERVER INFO
	// Assert((fSenderReportSize & 3) == 0);
	((unsigned long*)&fSenderReportBuffer)[(fSenderReportSize >> 2) + 1] = htonl(inSSRC);

	// Set SSRC in BYE
	// Assert((fSenderReportWithServerInfoSize & 3) == 0);
	((unsigned long*)&fSenderReportBuffer)[(fSenderReportWithServerInfoSize >> 2) + 1] = htonl(inSSRC);
}

void	RTCPSRPacket::SetClientSSRC(unsigned long inClientSSRC)
{
	//
	// Set Client SSRC in SERVER INFO
	((unsigned long*)&fSenderReportBuffer)[(fSenderReportSize >> 2) + 3] = htonl(inClientSSRC);	
}

void	RTCPSRPacket::SetRTPTimestamp(unsigned long inRTPTimestamp)
{
	((unsigned long*)&fSenderReportBuffer)[4] = htonl(inRTPTimestamp);
}

void	RTCPSRPacket::SetPacketCount(unsigned long inPacketCount)
{
	((unsigned long*)&fSenderReportBuffer)[5] = htonl(inPacketCount);
}

void	RTCPSRPacket::SetByteCount(unsigned long inByteCount)
{
	((unsigned long*)&fSenderReportBuffer)[6] = htonl(inByteCount);
}	

void	RTCPSRPacket::SetAckTimeout(unsigned long inAckTimeoutInMsec)
{
	((unsigned long*)&fSenderReportBuffer)[(fSenderReportWithServerInfoSize >> 2) - 1] = htonl(inAckTimeoutInMsec);
}

RTCPSRPacket::RTCPSRPacket()
{
	// Write as much of the Sender Report as is possible
	char theTempCName[kMaxCNameLen];
	unsigned long cNameLen = RTCPSRPacket::GetACName(theTempCName);
	
	//write the SR & SDES headers
	unsigned long* theSRWriter = (unsigned long*)&fSenderReportBuffer;
	*theSRWriter = htonl(0x80c80006);
	theSRWriter += 7; //number of UInt32s in an SR.
	
	//SDES length is the length of the CName, plus 2 32bit words, plus the 32bit word for the SSRC
	*theSRWriter = htonl(0x81ca0000 + (cNameLen >> 2) + 1);
	::memcpy(&fSenderReportBuffer[kSenderReportSizeInBytes], theTempCName, cNameLen);
	fSenderReportSize = kSenderReportSizeInBytes + cNameLen;

/*
 SERVER INFO PACKET FORMAT
struct qtss_rtcp_struct
{
	RTCPHeader		header;
	unsigned long			ssrc;		// ssrc of rtcp originator
	OSType			name;
	unsigned long			senderSSRC;
	signed short			reserved;
	signed short			length;		// bytes of data (atoms) / 4
	// qtsi_rtcp_atom structures follow
};
*/

	//
	// Write the SERVER INFO APP packet
	unsigned long* theAckInfoWriter = (unsigned long*)&fSenderReportBuffer[fSenderReportSize];
	*theAckInfoWriter = htonl(0x81cc0006);
	theAckInfoWriter += 2;
	*(theAckInfoWriter++) = htonl(FOUR_CHARS_TO_INT('q', 't', 's', 'i')); // Ack Info APP name
	theAckInfoWriter++; // leave space for the ssrc (again)
	*(theAckInfoWriter++) = htonl(2); // 2 UInt32s for the 'at' field
	*(theAckInfoWriter++) = htonl(FOUR_CHARS_TO_INT( 'a', 't', 0, 4 ));
	fSenderReportWithServerInfoSize = (char*)(theAckInfoWriter+1) - fSenderReportBuffer;	
	
	unsigned long* theByeWriter = (unsigned long*)&fSenderReportBuffer[fSenderReportWithServerInfoSize];
	*theByeWriter = htonl(0x81cb0001);
}

unsigned long RTCPSRPacket::GetACName(char* ioCNameBuffer)
{
	static char*	sCNameBase = "QTSS";
	#ifdef WIN32
		DWORD llMilliseconds;
		llMilliseconds		= GetTickCount ();
	#else
		long long llMilliseconds;
		struct timeval                      tvTimeval;
		gettimeofday (&tvTimeval, NULL);
		llMilliseconds	= (tvTimeval. tv_sec * 1000 + (tvTimeval. tv_usec / 1000));
	#endif

	//clear out the whole buffer
	::memset(ioCNameBuffer, 0, kMaxCNameLen);
	
	//cName identifier
	ioCNameBuffer[0] = 1;
	
	//Unique cname is constructed from the base name and the current time
	::sprintf(&ioCNameBuffer[1], " %s%lld", sCNameBase, llMilliseconds / 1000);
	unsigned long cNameLen = ::strlen(ioCNameBuffer);
	//2nd byte of CName should be length
	ioCNameBuffer[1] = cNameLen - 2;//don't count indicator or length byte

	// This function assumes that the cName is the only item in this SDES chunk
	// (see RTP rfc for details).
	// The RFC says that the item (the cName) should not be NULL terminated, but
	// the chunk *must* be NULL terminated. And padded to a 32-bit boundary.
	//
	// ::sprintf already put a NULL terminator in the cName buffer. So all we have to
	// do is pad out to the boundary.
	cNameLen += 1; //add on the NULL character
	unsigned long paddedLength = cNameLen + (4 - (cNameLen % 4));
	
	// Pad, and zero out as we pad.
	for (; cNameLen < paddedLength; cNameLen++)
		ioCNameBuffer[cNameLen] = '\0';
	
	// Assert((cNameLen % 4) == 0);
	return cNameLen;
}


#ifdef WIN32
__int64  RTCPSRPacket:: HostToNetworkSInt64(__int64 hostOrdered)
{
#if BIGENDIAN
		    return hostOrdered;
#else
			    return (__int64) (  (__int64)  (hostOrdered << 56) | (__int64)  (((__int64) 0x00ff0000 << 32) & (hostOrdered << 40))
								        | (__int64)  ( ((__int64)  0x0000ff00 << 32) & (hostOrdered << 24)) | (__int64)  (((__int64)  0x000000ff << 32) & (hostOrdered << 8))
										        | (__int64)  ( ((__int64)  0x00ff0000 << 8) & (hostOrdered >> 8)) | (__int64)     ((__int64)  0x00ff0000 & (hostOrdered >> 24))
												        | (__int64)  (  (__int64)  0x0000ff00 & (hostOrdered >> 40)) | (__int64)  ((__int64)  0x00ff & (hostOrdered >> 56)) );
#endif
}
#else	// !WIN32
long long  RTCPSRPacket:: HostToNetworkSInt64(long long hostOrdered)
{
#if BIGENDIAN
		    return hostOrdered;
#else
			    return (long long) (  (unsigned long long)  (hostOrdered << 56) | (unsigned long long)  (((unsigned long long) 0x00ff0000 << 32) & (hostOrdered << 40))
								        | (unsigned long long)  ( ((unsigned long long)  0x0000ff00 << 32) & (hostOrdered << 24)) | (unsigned long long)  (((unsigned long long)  0x000000ff << 32) & (hostOrdered << 8))
										        | (unsigned long long)  ( ((unsigned long long)  0x00ff0000 << 8) & (hostOrdered >> 8)) | (unsigned long long)     ((unsigned long long)  0x00ff0000 & (hostOrdered >> 24))
												        | (unsigned long long)  (  (unsigned long long)  0x0000ff00 & (hostOrdered >> 40)) | (unsigned long long)  ((unsigned long long)  0x00ff & (hostOrdered >> 56)) );
#endif
}
#endif	// WIN32



#ifdef WIN32
void	RTCPSRPacket::SetNTPTimestamp(__int64 inNTPTimestamp)
{
#if ALLOW_NON_WORD_ALIGN_ACCESS
	((__int64*)&fSenderReportBuffer)[1] = HostToNetworkSInt64(inNTPTimestamp);
#else
	__int64 temp = HostToNetworkSInt64(inNTPTimestamp);
	::memcpy(&((__int64*)&fSenderReportBuffer)[1], &temp, sizeof(temp));
#endif
}
#else	// !WIN32
void	RTCPSRPacket::SetNTPTimestamp(long long inNTPTimestamp)
{
#if ALLOW_NON_WORD_ALIGN_ACCESS
	((long long*)&fSenderReportBuffer)[1] = HostToNetworkSInt64(inNTPTimestamp);
#else
	long long temp = HostToNetworkSInt64(inNTPTimestamp);
	::memcpy(&((long long*)&fSenderReportBuffer)[1], &temp, sizeof(temp));
#endif
}
#endif	// WIN32

