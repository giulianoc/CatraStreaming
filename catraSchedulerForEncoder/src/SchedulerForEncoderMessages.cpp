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


#include "SchedulerForEncoderMessages.h"


MessagesInfo SchedulerForEncoderMessagesStr = {

	// schedulerForEncoder
	SFE_SCHEDULERFORENCODER_FOUNDMSITETOSCHEDULE,
		"Found the '%s' MSITE using the '%s' encoder and the following schedule: %s",

	// TimesForVidiator
	SFE_TIMESFORVIDIATOR_TIMEOUT,
		"Vidiator encoding session: %s. Next time out: %s",
	SFE_TIMESFORVIDIATOR_PREVIOUSSCHEDULINGNOTFINISHED,
		"The '%s' previous encoding session is not finished",

	// TimesForNexencoder
	SFE_TIMESFORNEXENCODER_TIMEOUT,
		"Nexencoder encoding session: %s. Next time out: %s",
	SFE_TIMESFORNEXENCODER_PREVIOUSSCHEDULINGNOTFINISHED,
		"The '%s' previous encoding session is not finished",

	// RequestForEncoder
	SFE_REQUESTFORENCODER_STARTENCODINGSESSION,
		"Encoding session: %s. Start encoding session",
	SFE_REQUESTFORENCODER_FINISHENCODINGSESSION,
		"Encoding session: %s. Finish encoding session",
	SFE_REQUESTFORENCODER_FILETOENCODE,
		"Encoding session: %s. File to encode: '%s', profile to use: '%s'",
	SFE_REQUESTFORENCODER_DOWNLOADFILESLIST,
		"Encoding session: %s. Download files list",
	SFE_REQUESTFORENCODER_DOWNLOADSOURCEFILE,
		"Encoding session: %s. Download the '%s' source file",
	SFE_REQUESTFORENCODER_ENCODE,
		"Encoding session: %s. Encode with profile: '%s', source file: '%s'",
	SFE_REQUESTFORENCODER_UPLOADENCODEDFILE,
		"Encoding session: %s. Upload the encoded file, source file: '%s'",
	SFE_REQUESTFORENCODER_UPLOADFILESLISTUPDATED,
		"Encoding session: %s. Upload the updated files list",
	SFE_REQUESTFORENCODER_WAITENCODEDFILE,
		"Encoding session: %s. Wait the encoded file, source file: '%s'",
	SFE_REQUESTFORENCODER_FOUNDFILEALREADYENCODED,
		"Encoding session: %s. Found the '%s' file (profile: '%s') already encoded",
	SFE_REQUESTFORNEXENCODER_COMMAND,
		"Encoding session: %s. Command for Nexencoder: %s",

	// RequestForVidiator
	SFE_REQUESTFORVIDIATOR_HTTPREQUESTTOVIDIATOR,
		"Encoding session: %s. HTTP request to Vidiator: '%s'",
	SFE_REQUESTFORVIDIATOR_HTTPRESPONSEFROMVIDIATOR,
		"Encoding session: %s. HTTP response from Vidiator: '%s'",
	SFE_REQUESTFORVIDIATOR_CREATEPROFILE,
		"Encoding session: %s. Create profile: '%s', source file: '%s'",

	// common
	SFE_FTPLOG,
		"FTP log: '%s'"


	// Insert here other messagges...

} ;

