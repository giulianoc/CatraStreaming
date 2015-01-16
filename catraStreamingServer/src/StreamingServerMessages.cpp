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


#include "StreamingServerMessages.h"

#ifdef MPATROL
	#include "mpatrol.h"
#endif


ErrMsgBase:: ErrMsgsInfo StreamingServerMessagesStr = {

	// StreamingServerService
	SS_STREAMINGSERVERSERVICE_INITIALIZING,
		"Streaming server initializing",
	SS_STREAMINGSERVERSERVICE_INITIALIZED,
		"Streaming server initialized",
	SS_STREAMINGSERVERSERVICE_STARTING,
		"Streaming server starting...",
	SS_STREAMINGSERVERSERVICE_STREAMINGSERVERPROCESSORSSTARTED,
		"Streaming server processors started",
	SS_STREAMINGSERVERSERVICE_SCHEDULERSTARTED,
		"Scheduler started",
	SS_STREAMINGSERVERSERVICE_HTTPSTREAMINGSERVERSTARTED,
		"HTTP Streaming Server started",
	SS_STREAMINGSERVERSERVICE_HTTPSTREAMINGSERVERSTOPPED,
		"HTTP Streaming Server stopped",
	SS_STREAMINGSERVERSERVICE_SCHEDULERSTOPPED,
		"Scheduler stopped",
	SS_STREAMINGSERVERSERVICE_STREAMINGSERVERPROCESSORSSTOPPED,
		"Streaming Server Processors stopped",
	SS_STREAMINGSERVERSERVICE_RUNNING,
		"Streaming server listening and running... CatraStreamingServer version: %s, Operative System: %s, Content root path: %s, RTSP Server IP address: %s, RTSP Server port: %lu, Max RTSP Streaming Sessions: %lu, HTTP Server IP address: %s, HTTP Server Port: %lu, Standard used: %s",
	SS_STREAMINGSERVERSERVICE_SHUTDOWN,
		"Streaming server is going down...",
	SS_STREAMINGSERVERSERVICE_NOTRUNNING,
		"Streaming server not running...",
	SS_STREAMINGSERVERSERVICE_FINISHING,
		"Streaming server finishing...",
	SS_STREAMINGSERVERSERVICE_FINISHED,
		"Streaming server finished",

	// HttpStreamingServer
	SS_HTTPSTREAMINGSERVER_COMMANDRECEIVED,
		"HTTP command received: %s",
	SS_HTTPSTREAMINGSERVER_HTTPRESPONSE,
		"HTTP response. Header: %s, Body: %s",

	// BeginningHookHttpGetThread
	SS_BEGINNINGHOOKHTTPGETTHREAD_REQUESTAUTHORIZED,
		"RTSPSession identifier: %lu. Request authorized: %s",
	SS_BEGINNINGHOOKHTTPGETTHREAD_REQUESTNOTAUTHORIZED,
		"RTSPSession identifier: %lu. Request not authorized: %s",
	SS_BEGINNINGHOOKHTTPGETTHREAD_FAILURE,
		"RTSPSession identifier: %lu. Failure requesting authorization: %s",
	SS_BEGINNINGHOOKHTTPGETTHREAD_HTTPGETERROR,
		"RTSPSession identifier: %lu. HTTP GET error: %s",

	// catraStreamingServer
	/*
	SS_STREAMINGSERVER_INITIALIZE,
		"Initialize the StreamingServer process no. %lu",
	SS_STREAMINGSERVER_FINISHED,
		"StreamingServer finished. Process no. %lu",
	SS_STREAMINGSERVER_PROCESSSTARTED,
		"StreamingServer process started. ServerID: %lu, PID: %ld",
	SS_STREAMINGSERVERSMANAGER_INITIALIZE,
		"Initialize the StreamingServersManager thread",
	SS_STREAMINGSERVERSMANAGER_FINISHED,
		"StreamingServersManager thread finished",
	*/

	// StreamingServer
	/*
	SS_STREAMINGSERVER_RUNNING,
		"StreamingServer thread is running. Content root path: %s, Local IP address: %s, Port: %lu, Clients queue size: %ld",
	SS_STREAMINGSERVER_CORBA_PROVA,
		"StreamingServer: received 'prova' CORBA method",
	SS_STREAMINGSERVER_CORBA_ACTIVATE,
		"StreamingServer: received 'activate' CORBA method",
	SS_STREAMINGSERVER_CORBA_DEACTIVATE,
		"StreamingServer: received 'deactivate' CORBA method",
	SS_STREAMINGSERVER_CORBA_SHUTDOWN,
		"StreamingServer: received 'shutdown' CORBA method",
	SS_STREAMINGSERVER_CORBA_GETSERVERCONNECTEDUSERS,
		"StreamingServer: received 'getServerConnectedUsers' CORBA method.",
	SS_STREAMINGSERVER_CORBA_GETSERVERINFO,
		"StreamingServer: received 'getServerInfo' CORBA method.",
	SS_STREAMINGSERVER_CORBA_ISREQUESTINCACHE,
		"StreamingServer: received 'isRequestInCache' CORBA method. File path: %s",
	SS_STREAMINGSERVER_CORBA_NAMEREGISTRATION,
		"CORBA name registered to NameServise: %s",
	SS_STREAMINGSERVER_DIRECTORYTOCREATE,
		"Directory to create: %s",
	SS_STREAMINGSERVER_DIRECTORYTODELETE,
		"Directory to delete: %s",
	SS_STREAMINGSERVER_CONTENTTOCREATE,
		"Content to create: %s",
	SS_STREAMINGSERVER_CONTENTTODELETE,
		"Content to delete: %s",
	SS_STREAMINGSERVER_CONTENTTODUMP,
		"Content to dump: %s",
	SS_STREAMINGSERVER_CONTENTTOHAVETRACKSINFO,
		"Content to have tracks info: %s",
	SS_STREAMINGSERVER_CORBA_SETCHANGEDCONFIGURATION,
		"StreamingServersManager: received 'setChangedConfiguration' CORBA method",
	SS_STREAMINGSERVER_CORBA_GETCONFIGURATION,
		"StreamingServersManager: received 'getConfiguration' CORBA method",
	SS_STREAMINGSERVER_CORBA_GETNICS,
		"StreamingServersManager: received 'getNICs' CORBA method",
	SS_STREAMINGSERVER_CORBA_GETCONTENTS,
		"StreamingServersManager: received 'getContents' CORBA method. Directory: %s",
	SS_STREAMINGSERVER_CORBA_ADDDIRECTORY,
		"StreamingServersManager: received 'addDirectory' CORBA method. Path name: %s",
	SS_STREAMINGSERVER_CORBA_DELETEDIRECTORY,
		"StreamingServersManager: received 'deleteDirectory' CORBA method. Path name: %s",
	SS_STREAMINGSERVER_CORBA_ADDCONTENT,
		"StreamingServersManager: received 'addContent' CORBA method. Path name: %s",
	SS_STREAMINGSERVER_CORBA_DELETECONTENT,
		"StreamingServersManager: received 'deleteContent' CORBA method. Path name: %s",
	SS_STREAMINGSERVER_CORBA_DUMPCONTENT,
		"StreamingServersManager: received 'dumpContent' CORBA method. Path name: %s",
	SS_STREAMINGSERVER_CORBA_GETTRACKSINFO,
		"StreamingServersManager: received 'getTracksInfo' CORBA method. Path name: %s",
	*/

	// StreamingServerProcessor
	SS_STREAMINGSERVERPROCESSOR_SENTRTPPACKETSNUMBER,
		"RTSPSession identifier: %lu. Sent %lu Video RTP Packets and %lu Audio RTP Packets",
	SS_STREAMINGSERVERPROCESSOR_STARTEVENTHANDLER,
		"RTSPSession identifier: %lu. Received event %s",

	// StreamingServerEventsSet
	SS_STREAMINGSERVEREVENTSSET_ALLOCATEDMOREEVENTS,
		"Allocated more %s events. Total number: %lu, address in decimal of the allocated events: %lu",

	// StreamingServersManager
	/*
	SS_STREAMINGSERVERSMANAGER_RUNNING,
		"StreamingServerServersManager thread is running",
	SS_STREAMINGSERVERSMANAGER_RTSPSERVER,
		"RTSP server. Local IP address: %s, Port: %lu, Clients queue size: %ld",
	SS_STREAMINGSERVERSMANAGER_DIRECTORYTOCREATE,
		"Directory to create: %s",
	SS_STREAMINGSERVERSMANAGER_DIRECTORYTODELETE,
		"Directory to delete: %s",
	SS_STREAMINGSERVERSMANAGER_CONTENTTOCREATE,
		"Content to create: %s",
	SS_STREAMINGSERVERSMANAGER_CONTENTTODELETE,
		"Content to delete: %s",
	SS_STREAMINGSERVERSMANAGER_CONTENTTODUMP,
		"Content to dump: %s",
	SS_STREAMINGSERVERSMANAGER_CONTENTTOHAVETRACKSINFO,
		"Content to have tracks info: %s",
	SS_STREAMINGSERVERSMANAGER_CORBA_PROVA,
		"StreamingServersManager: received 'prova' CORBA method",
	SS_STREAMINGSERVERSMANAGER_CORBA_ACTIVATE,
		"StreamingServersManager: received 'activate' CORBA method",
	SS_STREAMINGSERVERSMANAGER_CORBA_DEACTIVATE,
		"StreamingServersManager: received 'deactivate' CORBA method",
	SS_STREAMINGSERVERSMANAGER_CORBA_SHUTDOWN,
		"StreamingServersManager: received 'deactivateServer' CORBA method",
	SS_STREAMINGSERVERSMANAGER_CORBA_SETCHANGEDCONFIGURATION,
		"StreamingServersManager: received 'setChangedConfiguration' CORBA method",
	SS_STREAMINGSERVERSMANAGER_CORBA_GETCONFIGURATION,
		"StreamingServersManager: received 'getConfiguration' CORBA method",
	SS_STREAMINGSERVERSMANAGER_CORBA_GETNICS,
		"StreamingServersManager: received 'getNICs' CORBA method",
	SS_STREAMINGSERVERSMANAGER_CORBA_GETCONTENTS,
		"StreamingServersManager: received 'getContents' CORBA method. Directory: %s",
	SS_STREAMINGSERVERSMANAGER_CORBA_ADDDIRECTORY,
		"StreamingServersManager: received 'addDirectory' CORBA method. Path name: %s",
	SS_STREAMINGSERVERSMANAGER_CORBA_DELETEDIRECTORY,
		"StreamingServersManager: received 'deleteDirectory' CORBA method. Path name: %s",
	SS_STREAMINGSERVERSMANAGER_CORBA_ADDCONTENT,
		"StreamingServersManager: received 'addContent' CORBA method. Path name: %s",
	SS_STREAMINGSERVERSMANAGER_CORBA_DELETECONTENT,
		"StreamingServersManager: received 'deleteContent' CORBA method. Path name: %s",
	SS_STREAMINGSERVERSMANAGER_CORBA_DUMPCONTENT,
		"StreamingServersManager: received 'dumpContent' CORBA method. Path name: %s",
	SS_STREAMINGSERVERSMANAGER_CORBA_GETTRACKSINFO,
		"StreamingServersManager: received 'getTracksInfo' CORBA method. Path name: %s",
	SS_STREAMINGSERVERSMANAGER_CORBA_GETSERVERCONNECTEDUSERS,
		"StreamingServersManager: received 'getServerConnectedUsers' CORBA method.",
	SS_STREAMINGSERVERSMANAGER_CORBA_GETSERVERINFO,
		"StreamingServersManager: received 'getServerInfo' CORBA method.",
	SS_STREAMINGSERVERSMANAGER_CORBA_NAMEREGISTRATION,
		"CORBA name registered to NameServise: %s",
	SS_STREAMINGSERVERSMANAGER_FREERTSPSESSIONSNUMBER,
		"Free RTSP sessions number: %lu",
	*/

	// RTSPSession
	SS_RTSPSESSION_SENDRTPPACKET,
		"Send RTP packet. IPAddress: %s, RTP Remote port: %ld, Track URI: %s, Sequence number: (relative (%lu) + offset (%lu)): %lu, Marker bit: %s, RTP Timestamp (relative (%lf) + offset (%lu)): %lu, Absolute transmission time: %lf, Payload: %ld, RTP packet length: %llu",
	SS_RTSPSESSION_RECEIVEDRTSPMESSAGE,
		"RTSPSession identifier: %lu. Received RTSP message from %s:%lu. Message: '%s'",
	SS_RTSPSESSION_CONNECTIONARRIVED,
		"RTSPSession identifier: %lu. Connection arrived from %s:%lu.",
	SS_RTSPSESSION_CONNECTIONCLOSED,
		"RTSPSession identifier: %lu. Connection from %s:%lu closed.",
	SS_RTSPSESSION_SENDTOCLIENT,
		"RTSPSession identifier: %lu. Sent to client: '%s'",
	SS_RTSPSESSION_MP4FILEINCACHE,
		"RTSPSession identifier: %lu. Is the '%s' MP4 file in cache? '%s'",
	SS_RTSPSESSION_VIDEOMEDIATRACKNOTFOUND,
		"Video media track not found.",
	SS_RTSPSESSION_AUDIOMEDIATRACKNOTFOUND,
		"Audio media track not found.",
	SS_RTSPSESSION_VIDEOCODECNOTSUPPORTED,
		"Video codec not supported. Streaming only for the audio media track",
	SS_RTSPSESSION_AUDIOCODECNOTSUPPORTED,
		"Audio codec not supported. Streaming only for the video media track",
	SS_RTSPSESSION_TRACKSCHOSEN,
		"RTSPSession identifier: %lu. Video track name to stream: %s, audio track name to stream: %s",
	SS_RTSPSESSION_VIDEOHINTTRACKNOTFOUND,
		"RTSPSession identifier: %lu. Video hint track not found",
	SS_RTSPSESSION_AUDIOHINTTRACKNOTFOUND,
		"RTSPSession identifier: %lu. Audio hint track not found",
	SS_RTSPSESSION_CREATEVIDEOHINTTRACK,
		"Create video hint track.",
	SS_RTSPSESSION_CREATEAUDIOHINTTRACK,
		"Create audio hint track.",
	SS_RTSPSESSION_RTSP_RTCPTIMEOUTEXPIRED,
		"RTSP and RTCP timeout expired, the streaming server will close the connection to the client (RTSP_RTCPTimeoutInSecs: %lu)",
	SS_RTSPSESSION_PAUSETIMEOUTEXPIRED,
		"Pause timeout expired, the streaming server will close the connection to the client (PauseTimeoutInSecs: %lu)",
	SS_RTSPSESSION_RECEIVEDRECEIVERRTCPMESSAGE,
		"RTSPSession identifier: %lu. Received Receiver RTCP message. IPAddress: %s, RTCP Remote port: %ld, Track URI: %s, CumulativeFractionLostPackets: %lu, CumulativeTotalLostPackets: %lu, CumulativeJitter: %lu",
	SS_RTSPSESSION_RECEIVEDAPPRTCPMESSAGE,
		"RTSPSession identifier: %lu. Received APP RTCP message. IPAddress: %s, RTCP Remote port: %ld, Track URI: %s",
	SS_RTSPSESSION_RECEIVEDSDESRTCPMESSAGE,
		"RTSPSession identifier: %lu. Received SDES RTCP message. IPAddress: %s, RTCP Remote port: %ld, Track URI: %s",
	SS_RTSPSESSION_SENDRTCPPACKET,
		"RTSPSession identifier: %lu. Send RTCP packet. RTCP time (secs): %lf, IPAddress: %s, RTCP Remote port: %ld, Track URI: %s, RTCP Packet length: %ld, End of partecipation: %s, Sent packets number: %lu, Sent packets bytes: %llu",
	SS_RTSPSESSION_SEEKCALLED,
		"RTSPSession identifier: %lu. Called the seek method. Start time in secs: %lf.",
	SS_RTSPSESSION_HTTPGETREQUEST,
		"RTSPSession identifier: %lu. HTTP GET request. WebServerIPAddress: %s, WebServerPort: %lu, GET request: %s%s",

	// RTPOverBufferWindow
	SS_RTPOVERBUFFERWINDOW_WINDOWSIZE,
		"New size of the window size"


	// common

	// Insert here other errors...

} ;

