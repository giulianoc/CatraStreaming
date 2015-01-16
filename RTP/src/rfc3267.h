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

#ifndef rfc3267_h
	#define rfc3267_h

	#include "MP4RootAtom.h"
	#include "MP4TrakAtom.h"
	#include "MP4TrackInfo.h"
	#include "RTPPacket.h"
	#include "RTPStreamRealTimeInfo.h"
	#include "Tracer.h"
	#include "RTPErrors.h"


	#ifndef RTP_DEFAULTMAXRTPPAYLOADSIZE
		#define RTP_DEFAULTMAXRTPPAYLOADSIZE					1472
	#endif

	// each frame AMR is 20 ms
	//	7 frame AMR means 140ms
	// mail of Jayank:
	// 	Server could put 50 frames into one RTP packet but its not practical
	//	and thats why server is doing this. On an average on a phone there could
	//	be 200ms of latency. So anything above this wouldnt be good
	//	for a client side. So as being a conservative we are using 150ms
	//	as the max latency for AMR packets. Now what this means. It means that
	//	if you lost a RTP packet you won't loose more then 150ms of speech data
	//	or if you do interleaving then you at the most need to have a latency
	//	of 150ms....
	//	Think about it, if you put 50 frames in one RTP packet that counts
	//	to 50x20ms=1 sec, you might loose one seconds of data which is too much.	//	Now this 150ms/20ms=7 frames at the most you can get.
	#define RTP_MAXAMRFRAMESNUMBERFOREACHRTPPACHET		7
	#define RTP_MAXCLOCKRATELENGTH						128 + 1
	/*
	#define MP4F_RFC3267AUDIOPREFETCHER_SOURCE		"rfc 3267 Audio Prefetcher"
	#define MP4F_STREAMINGSERVERPROCESSOR_DESTINATION	"StreamingServerProcessor"
	*/


	Error rfc3267AudioHinter (MP4RootAtom_p pmaRootAtom,
		MP4TrackInfo_p pmtiMediaTrackInfo,
		unsigned long ulMaxPayloadSize,
		MP4TrackInfo_p pmtiHintTrackInfo,
		std:: vector<MP4TrackInfo_p> *pvMP4TracksInfo,
		MP4Atom:: Standard_t sStandard,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer);

	Error rfc3267AudioPrefetcher (
		unsigned long ulMediaSamplesNumber,
		unsigned char *pucMediaSampleBuffer,
		unsigned long ulMaxMediaSampleSize,
		unsigned long ulPayloadNumber,
		unsigned long ulRTPTimeScale,
		unsigned long ulMdhdTimeScale,
		// unsigned long ulFirstEditRTPTime,
		unsigned long ulServerSSRC,
		MP4RootAtom_p pmaRootAtom,
		MP4TrackInfo_p pmtiMediaTrackInfo,
		unsigned long ulMaxPayloadSize,
		unsigned long ulRTPPacketsNumberToPrefetch,
		const char *pTrackURI,
		std:: vector<RTPPacket_p> *pvFreeRTPPackets,
		std:: vector<RTPPacket_p> *pvBuiltRTPPackets,
		std:: vector<double> *pvTransmissionTimeBuiltRTPPackets,
		RTPStreamRealTimeInfo_p prsrtRTPStreamRealTimeInfo,
		Tracer_p ptTracer,
		unsigned long *pulCurrentMediaSampleIndex,
		unsigned long *pulCurrentSequenceNumber);

#endif

