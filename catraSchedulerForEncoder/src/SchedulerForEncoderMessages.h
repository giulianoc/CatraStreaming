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


#ifndef SchedulerForEncoderMessages_h
	#define SchedulerForEncoderMessages_h

	#include "Message.h"

	//
	// Click <a href="SchedulerForEncoderMessages.C#SchedulerForEncoderMessages" target=classContent>here</a> for the Messages strings.
	//
	enum SchedulerForEncoderMessagesCodes {

		// schedulerForEncoder
		SFE_SCHEDULERFORENCODER_FOUNDMSITETOSCHEDULE,

		// TimesForVidiator
		SFE_TIMESFORVIDIATOR_TIMEOUT,
		SFE_TIMESFORVIDIATOR_PREVIOUSSCHEDULINGNOTFINISHED,

		// TimesForNexencoder
		SFE_TIMESFORNEXENCODER_TIMEOUT,
		SFE_TIMESFORNEXENCODER_PREVIOUSSCHEDULINGNOTFINISHED,

		// RequestForEncoder
		SFE_REQUESTFORENCODER_STARTENCODINGSESSION,
		SFE_REQUESTFORENCODER_FINISHENCODINGSESSION,
		SFE_REQUESTFORENCODER_FILETOENCODE,
		SFE_REQUESTFORENCODER_DOWNLOADFILESLIST,
		SFE_REQUESTFORENCODER_DOWNLOADSOURCEFILE,
		SFE_REQUESTFORENCODER_ENCODE,
		SFE_REQUESTFORENCODER_UPLOADENCODEDFILE,
		SFE_REQUESTFORENCODER_UPLOADFILESLISTUPDATED,
		SFE_REQUESTFORENCODER_WAITENCODEDFILE,
		SFE_REQUESTFORENCODER_FOUNDFILEALREADYENCODED,
		SFE_REQUESTFORNEXENCODER_COMMAND,

		// RequestForVidiator
		SFE_REQUESTFORVIDIATOR_HTTPREQUESTTOVIDIATOR,
		SFE_REQUESTFORVIDIATOR_HTTPRESPONSEFROMVIDIATOR,
		SFE_REQUESTFORVIDIATOR_CREATEPROFILE,

		// common
		SFE_FTPLOG,

		// Insert here other Messages...

		SFE_MAXMESSAGES
	} ;

	// declaration of class error
	dclCustomMessageClass (SchedulerForEncoderMessages, SFE_MAXMESSAGES)

#endif
