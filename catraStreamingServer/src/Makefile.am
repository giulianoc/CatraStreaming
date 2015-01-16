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

#bin_SCRIPTS =	\
#	catraServerCtl.csh	\
#	CatraStreamingServer.cfg


#catraServerCtl.csh: catraServerCtl.csh.template
#	cp -f catraServerCtl.csh.template catraServerCtl.csh

#CatraStreamingServer.cfg: CatraStreamingServer.cfg.template
#	cp -f CatraStreamingServer.cfg.template CatraStreamingServer.cfg

#StreamingIDL.cpp: ../../StreamingIDL/StreamingIDL.idl
#	@echo "idl ../../StreamingIDL/StreamingIDL.idl";	\
#	idl ../../StreamingIDL/StreamingIDL.idl;	\
#	echo "mv StreamingIDL.cc StreamingIDL.cpp";	\
#	mv StreamingIDL.cc StreamingIDL.cpp;


bin_PROGRAMS = catraStreamingServer

#Important: do not use tab for the definition of a variable
#if HAVE_CORBA
#  CORBA_SOURCES = StreamingIDL.cpp StreamingIDL.h
#else
#  CORBA_SOURCES = 
#endif

catraStreamingServer_SOURCES =	\
	BeginningHookHttpGetThread.cpp	\
	BeginningHookHttpGetThread.h	\
	catraStreamingServer.cpp	\
	CheckReceivedRTCPPacketsTimes.cpp	\
	CheckReceivedRTCPPacketsTimes.h	\
	CheckServerSocketTimes.cpp	\
	CheckServerSocketTimes.h	\
	CheckSocketsPoolTimes.cpp	\
	CheckSocketsPoolTimes.h	\
	HttpStreamingServer.cpp	\
	HttpStreamingServer.h	\
	LiveEvent.cpp	\
	LiveEvent.h	\
	LiveSource.cpp	\
	LiveSource.h	\
	PauseTimes.cpp	\
	PauseTimes.h	\
	RTPOverBufferWindow2.cpp	\
	RTPOverBufferWindow2.h	\
	RTPOverBufferWindow.cpp	\
	RTPOverBufferWindow.h	\
	RTSPConnectionEvent.cpp	\
	RTSPConnectionEvent.h	\
	RTSP_RTCPTimes.cpp	\
	RTSP_RTCPTimes.h	\
	RTSPSession.cpp	\
	RTSPSession.h	\
	SendRTCPPacketsTimes.cpp	\
	SendRTCPPacketsTimes.h	\
	StreamingServerErrors.cpp	\
	StreamingServerErrors.h	\
	StreamingServerEventsSet.cpp	\
	StreamingServerEventsSet.h	\
	StreamingServerMessages.cpp	\
	StreamingServerMessages.h	\
	StreamingServerProcessor.cpp	\
	StreamingServerProcessor.h	\
	StreamingServerService.cpp	\
	StreamingServerService.h	\
	StreamingServerSocketsPool.cpp	\
	StreamingServerSocketsPool.h	\
	StreamingSessionTimes.cpp	\
	StreamingSessionTimes.h	\
	SubscriberTracer.cpp	\
	SubscriberTracer.h

catraStreamingServer_CPPFLAGS =	\
	-I${top_srcdir}/RTCP/src	\
	-I${top_srcdir}/RTSP/src	\
	-I${top_srcdir}/SDP/src	\
	-I${top_srcdir}/MP4File/src	\
	-I${top_srcdir}/RTP/src

catraStreamingServer_LDADD =	\
	${top_srcdir}/RTCP/src/libRTCP.la	\
	${top_srcdir}/RTSP/src/libRTSP.la	\
	${top_srcdir}/RTP/src/libRTP.la	\
	${top_srcdir}/SDP/src/libSDP.la	\
	${top_srcdir}/MP4File/src/libMP4File.la	\
	-lConfigurationFile	\
	-lConfiguration	\
	-lScheduler	\
	-lEventsSet	\
	-lTracer	\
	-lSocket	\
	-lTools	\
	-lWebTools	\
	-lPThread	\
	-lErrMsg	\
	@CRYPT_LIBRARY@	\
	-lpthread	\
	-lz	\
	-ldl

EXTRA_DIST =	\
	CatraStreamingServer.cfg	\
	catraStreamingServer.sh	\
	authorization.txt	\
	statistics.csh	\
	counterRequests.csh	\
	catraStreamingServer.vcproj	\
	RTSPSessionRedirection.cpp	\
	RTSPSessionRedirection.h	\
	StreamingServersManager.cpp	\
	StreamingServersManager.h	\
	StreamingServer.cpp.IDLImplementation	\
	StreamingServer.h.IDLImplementation

#AM_CXXFLAGS = -D_REENTRANT -DTRIAL_VERSION @PROFILING_FLAGS@
AM_CXXFLAGS = -D_REENTRANT -DRTPOVERBUFFERWINDOW2

