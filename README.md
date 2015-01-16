
THIS README IS OBSOLETE AND HAS TO BE UPDATED

CatraStreaming
==============

CatraStreaming is a collection of libraries and tools useful
for streaming environment. The most important component is the 3GPP
and ISMA streaming server.

CatraStreaming is free software.   For copyright  information please  see the
file COPYING, which contains the GNU Public Library License.


Prerequisits - Unix
===================

Before trying to compile the CatraStreaming make sure you have installed the
following software packages:

   o gnu make version 3.7 or newer (required)
   o gcc compiler and library (required). We recommend gcc 3.4.2 or newer
   o CatraLibraries 1.4 or newer (required)
		Download catralibraries from
			http://sourceforge.net/projects/catralibraries,
			build and install it
   o mico 2.3.11 or newer (optional)
		If you do not have mico, the streaming server will not have a CORBA
			interface and the GUI cannot be used
		Download mico from http://www.mico.org/down.html
			build and install it
   o JacORB 1.4.1 (optional)
		If you do not have JacORB, the GUI cannot be used
		Download JacORB from http://www.jacorb.org
		Copy the jacorb.jar file into the CatraStreamingGUI/jar directory
			of the catrastreaming package
   o JRE 1.4 or newer (SUN's Java Runtime) (optional)
		If you do not have the JRE, the GUI cannot be used


Prerequisits - Win32 (Windows 95/98/ME/NT/2000/XP)
==================================================

	o Microsoft's Visual C++ compiler 7.0
	o CatraLibraries 1.4 or newer (required)
		Download catralibraries from
			http://sourceforge.net/projects/catralibraries
			and build it
		Create the link 'catralibraries' (must be brother
			of the catrastreaming directory) to catralibraries-X.X
	o pthreads-win32 2005-01-25 or newer (required)
		Download the pthreads-dll-2005-01-25 directory from
			ftp://sources.redhat.com/pub/pthreads-win32/
		Create the link 'pthreads-dll' (must be brother
			of the catrastreaming directory) to pthreads-dll-2005-01-25
	o mico 2.3.11 or newer (optional)
		If you do not have mico, the streaming server will not have a CORBA
			interface and the GUI cannot be used
		Download mico from http://www.mico.org/down.html
			and build it
		Create the link 'mico' (must be brother
			of the catrastreaming directory) to mico-X.X
   o JacORB 1.4.1 (optional)
		If you do not have JacORB, the GUI cannot be used
		Download JacORB from http://www.jacorb.org
		Copy the jacorb.jar file into the CatraStreamingGUI/jar directory
			of the catrastreaming package
   o JRE 1.4 or newer (SUN's Java Runtime) (optional)
		If you do not have the JRE, the GUI cannot be used


Installation - Unix
===================

For installation perform the following steps:
	- verify the "Prerequisits - Unix"
	- ./configure
	- make
	- make install (you must be logged as root to execute this command)
For detailed instructions please see the file INSTALL.


Installation - Win32 (Windows 95/98/ME/NT/2000/XP)
==================================================

For installation perform the following steps:
	- verify the "Prerequisits - Win32 (Windows 95/98/ME/NT/2000/XP)"
	- explode the catrastreaming package as brother
		of the catralibraries directory
	- open the CatraStreaming.sln project into
		the catrastreaming directory with
		Microsoft's Visual C++ compiler 7.0
	- rebuild all the project ('Build -> Rebuild Solution' menù item)
	- all the libraries and executables will be into the
		relative 'Debug' directories


Configuration Streaming Server - Unix
=====================================

In case you have installed mico (optional package implementing
the CORBA specification):
	initialize the following environment variable
	(below is used the syntax for a csh shell):
		setenv CATRASTREAMINGSERVERPATH <absolute path>
		setenv NSDSERVERIP <NSD Server IP>
		setenv NSDSERVERPORT <NSD Server Port>
		setenv CORBAPATH <absolute path>
	If you left the default installation directory, the CATRASTREAMINGSERVERPATH
	environment variable must be initialized to '/usr/local/bin'.

	The NSDSERVERIP variable is the IP address where will run the 
	CORBA Naming Service.
	The NSD Server is a common component shared by all the CatraStreaming Server
	modules and GUIs, and for this reason it must be unique within the platform.
	The NSD Server though it represents a standalone component in the
	CatraStreaming Platform can be installed together
	with a CatraStreaming Server.

If you specified an installation directory different
by the default (/usr/local/bin), update the
PATH and LD_LIBRARY_PATH environment variables.

The CatraStreaming Server configuration is easily performed through
the modification of the configuration file:
$CATRASTREAMINGSERVERPATH/CatraStreamingServer.cfg.
Though this file includes several configuration parameters, for the sake of use
a minimal set of them is reported in order to start the server and
make it works:
[StreamingServer]
	Name = <unique logical name. Each CatraStreaming Server must have a unique
		logical name within the platform>.
	ContentRootPath = <absolute path of the ContentsDir>
	Standard = <Standard to be used: '3GPP' for streaming on mobile or 'ISMA' for streaming on internet>
[IPConfiguration]
	LogicalIPAddressForRTSP = <RTSP listening address>
	LogicalIPAddressForRTP = <IP address for RTP traffic>
	LogicalIPAddressForRTCP = <IP address for RTCP traffic>
[SystemLogs]
	BaseTraceFileName = <absolute path name for system trace files>
[SubscriberLogs]
	BaseTraceFileName = <absolute path name for subscriber trace files>


Configuration Streaming Server - Win32 (Windows 95/98/ME/NT/2000/XP)
====================================================================

The CatraStreaming Server configuration is easily performed through
the modification of the configuration file:
$CATRASTREAMINGSERVERPATH/CatraStreamingServer.cfg.
Though this file includes several configuration parameters, for the sake of use
a minimal set of them is reported in order to start the server and
make it works:
[StreamingServer]
	Name = <unique logical name. Each CatraStreaming Server must have a unique
		logical name within the platform>.
	ContentRootPath = <absolute path of the ContentsDir>
	Standard = <Standard to be used: '3GPP' for streaming on mobile or 'ISMA' for streaming on internet>
[IPConfiguration]
	LogicalIPAddressForRTSP = <RTSP listening address>
	LogicalIPAddressForRTP = <IP address for RTP traffic>
	LogicalIPAddressForRTCP = <IP address for RTCP traffic>
[SystemLogs]
	BaseTraceFileName = <absolute path name for system trace files>
[SubscriberLogs]
	BaseTraceFileName = <absolute path name for subscriber trace files>



Configuration Streaming GUI - Unix/Win32 (Windows 95/98/ME/NT/2000/XP)
======================================================================

The Streaming GUI works only if you have installed mico (optional package
implementing the CORBA specification).
Therefore, in case you have installed mico, follow the next steps
to configure the CatraStreaming GUI:
	1. Copy the jacorb_properties.template file from the JacORB
		package to the home directory of the user that will run the applet.
		Rename it in jacorb.properties.
		On a Windows system the home directory should be
		‘C:\Documents and Settings\<username>’,
		on a Linux system will be ‘~<username>’
	2. Modify the jacorb.properties just copied initializing
		the “ORBInitRef.NameService” field to
		“corbaloc::<NSDSERVERIP>:<NSDSERVERPORT>/NameService”.
		<NSDSERVERIP> and <NSDSERVERPORT> must be replaced respectively
		with the right IP address and socket port of the unique NSD server
		installed.
	3. Copy the orb.properties file from the JacORB package
		to the <JREX.Y_HOME>\lib directory
	4. Copy the CatraStreamingGUI/CatraStreamingGUI.policy file from the
		catrastreaming package to the <JREX.Y_HOME>\lib\security directory
	5. Modify the <JREX.Y_HOME>\lib\security\java.security file to change
		the value of the ‘policy.url.1’ field and replacing it
		with the ‘file:${java.home}/lib/security/CatraStreamingGUI.policy’
		string


Run Streaming Server - Unix
===========================

In case you have installed mico (optional package implementing
the CORBA specification):
	- the NSD Server is a common component shared by all
		the CatraStreaming Server modules and GUIs, and for this reason
		it must be unique within the platform.
	- the NSD Server though it represents a standalone component in the
		CatraStreaming Platform can be installed together
		with a CatraStreaming Server.

	- first, run the CORBA Naming Service:
		catraServerCtl.csh nsd start

	- second, run the Streaming Server:
		catraServerCtl.csh server start

In case you did not install mico, to run the Streaming Server executes the next
	command:
	nohup $CATRASTREAMINGSERVERPATH/catraStreamingServer -config $CATRASTREAMINGSERVERPATH/CatraStreamingServer.cfg &


Run Streaming Server - Win32 (Windows 95/98/ME/NT/2000/XP)
==========================================================

The NSD Server is a common component shared by all the CatraStreaming Server
modules and GUIs, and for this reason it must be unique within the platform.
The NSD Server though it represents a standalone component in the
CatraStreaming Platform can be installed together with a CatraStreaming Server.

To run the CORBA Naming Service:
	............

To run the Streaming Server:
	............


Run Streaming Server GUI - Unix/Win32 (Windows 95/98/ME/NT/2000/XP)
===================================================================

To run the Streaming Server GUI, double click on the StreamingGUI.html
file from the CatraStreamingGUI directory.
The first activity on the GUI that you could do is to add a 'site'
(just with a logical name) and add a Streaming Server into the site.


Documentation
=============

	Please refer the documents inside the doc subdirectory.

