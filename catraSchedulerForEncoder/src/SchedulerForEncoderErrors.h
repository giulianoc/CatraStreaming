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


#ifndef SchedulerForEncoderErrors_h
	#define SchedulerForEncoderErrors_h

	#include "Error.h"

	enum SchedulerForEncoderErrorsCodes {

		// schedulerForEncoder
		SFE_SCHEDULERFORENCODER_ENCODERNAMEWRONG,
		SFE_FOUNDTWOEQUALSESSIONENCODING,
		SFE_FOUNDTWOEQUALSESSIONENCODINGFTPINFO,

		// TimesForVidiator
		SFE_TIMESFORVIDIATOR_INIT_FAILED,

		// TimesForNexencoder
		SFE_TIMESFORNEXENCODER_INIT_FAILED,

		// RequestForEncoder
		SFE_REQUESTFORENCODER_INIT_FAILED,
		SFE_REQUESTFORENCODER_FINISH_FAILED,
		SFE_REQUESTFORENCODER_CREATEDIRECTORIES_FAILED,
		SFE_REQUESTFORENCODER_DOWNLOADFILESLIST_FAILED,
		SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
		SFE_REQUESTFORENCODER_UPDATEFILESLIST_FAILED,
		SFE_REQUESTFORENCODER_UPLOADFILESLIST_FAILED,
		SFE_REQUESTFORENCODER_DOWNLOADSOURCEFILE_FAILED,
		SFE_REQUESTFORENCODER_REMOVESOURCEFILE_FAILED,
		SFE_REQUESTFORENCODER_UPLOADENCODEDFILE_FAILED,
		SFE_REQUESTFORENCODER_REMOVEENCODEDFILE_FAILED,
		SFE_REQUESTFORENCODER_ENCODE_FAILED,
		SFE_REQUESTFORENCODER_ENCODEFINISHED_FAILED,
		SFE_REQUESTFORENCODER_FILESLISTWRONG,
		SFE_REQUESTFORENCODER_ENCODEDFILENOTPRODUCED,

		// RequestForVidiator
		SFE_REQUESTFORVIDIATOR_INIT_FAILED,
		SFE_REQUESTFORVIDIATOR_FINISH_FAILED,
		SFE_REQUESTFORVIDIATOR_CREATEPROFILEINSTANCE_FAILED,
		SFE_REQUESTFORVIDIATOR_WRONGRESPONSERECEIVEDFROMVIDIATOR,


		// RequestForNexencoder
		SFE_REQUESTFORNEXENCODER_INIT_FAILED,
		SFE_REQUESTFORNEXENCODER_FINISH_FAILED,

		// common
		SFE_NEW_FAILED,
		SFE_FTPOPENHOST_FAILED,
		SFE_FTPCHDIR_FAILED,
		SFE_FTPGETFILES3_FAILED,
		SFE_FTPPUTFILES3_FAILED,
		SFE_FTPCLOSEHOST_FAILED,

		// Insert here other errors...

		SFE_MAXERRORS
	} ;

	// declaration of class error
	dclCustomErrorClass (SchedulerForEncoderErrors, SFE_MAXERRORS)

#endif
