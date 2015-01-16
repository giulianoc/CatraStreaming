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


#include "SchedulerForEncoderErrors.h"


ErrorsInfo SchedulerForEncoderErrorsStr = {

	// schedulerForEncoder
	SFE_SCHEDULERFORENCODER_ENCODERNAMEWRONG,
		"Wrong encoder name: %s",
	SFE_FOUNDTWOEQUALSESSIONENCODING,
		"It is not possible to have two equal session encoding. Session name: %s",
	SFE_FOUNDTWOEQUALSESSIONENCODINGFTPINFO,
		"It is not possible to have two equal session encoding FTP info. FTP host: %s, FTP root path: %s",

	// TimesForVidiator
	SFE_TIMESFORVIDIATOR_INIT_FAILED,
		"The init method of the TimesForVidiator object failed",

	// TimesForNexencoder
	SFE_TIMESFORNEXENCODER_INIT_FAILED,
		"The init method of the TimesForNexencoder object failed",

	// RequestForEncoder
	SFE_REQUESTFORENCODER_INIT_FAILED,
		"The init method of the RequestForEncoder object failed",
	SFE_REQUESTFORENCODER_FINISH_FAILED,
		"The init method of the RequestForEncoder object failed",
	SFE_REQUESTFORENCODER_CREATEDIRECTORIES_FAILED,
		"The createDirectories method of the RequestForEncoder object failed",
	SFE_REQUESTFORENCODER_DOWNLOADFILESLIST_FAILED,
		"Encoding session: %s. The downloadFilesList method of the RequestForEncoder object failed",
	SFE_REQUESTFORENCODER_REMOVEFILESLIST_FAILED,
		"Encoding session: %s. The removeFilesList method of the RequestForEncoder object failed",
	SFE_REQUESTFORENCODER_UPDATEFILESLIST_FAILED,
		"Encoding session: %s. The updateFilesList method of the RequestForEncoder object failed",
	SFE_REQUESTFORENCODER_UPLOADFILESLIST_FAILED,
		"Encoding session: %s. The uploadFilesList method of the RequestForEncoder object failed",
	SFE_REQUESTFORENCODER_DOWNLOADSOURCEFILE_FAILED,
		"Encoding session: %s. The downloadSourceFile method of the RequestForEncoder object failed",
	SFE_REQUESTFORENCODER_REMOVESOURCEFILE_FAILED,
		"Encoding session: %s. The removeSourceFile method of the RequestForEncoder object failed",
	SFE_REQUESTFORENCODER_UPLOADENCODEDFILE_FAILED,
		"Encoding session: %s. The uploadEncodedFile method of the RequestForEncoder object failed",
	SFE_REQUESTFORENCODER_REMOVEENCODEDFILE_FAILED,
		"Encoding session: %s. The removeEncodedFile method of the RequestForEncoder object failed",
	SFE_REQUESTFORENCODER_ENCODE_FAILED,
		"Encoding session: %s. The encode method of the RequestForEncoder object failed",
	SFE_REQUESTFORENCODER_ENCODEFINISHED_FAILED,
		"Encoding session: %s. The encodeFinished method of the RequestForEncoder object failed",
	SFE_REQUESTFORENCODER_FILESLISTWRONG,
		"Encoding session: %s. Wrong Files List. Line: %s",
	SFE_REQUESTFORENCODER_ENCODEDFILENOTPRODUCED,
		"Encoding session: %s. Encoded file (%s) not produced by the encoder",

	// RequestForVidiator
	SFE_REQUESTFORVIDIATOR_INIT_FAILED,
		"The init method of the RequestForVidiator object failed",
	SFE_REQUESTFORVIDIATOR_FINISH_FAILED,
		"The finish method of the RequestForVidiator object failed",
	SFE_REQUESTFORVIDIATOR_CREATEPROFILEINSTANCE_FAILED,
		"Encoding session: %s. The createProfileInstance method of the RequestForVidiator object failed",
	SFE_REQUESTFORVIDIATOR_WRONGRESPONSERECEIVEDFROMVIDIATOR,
		"Encoding session: %s. Wrong response received from vidiator: '%s'",

	// RequestForNexencoder
	SFE_REQUESTFORNEXENCODER_INIT_FAILED,
		"The init method of the RequestForNexencoder object failed",
	SFE_REQUESTFORNEXENCODER_FINISH_FAILED,
		"The finish method of the RequestForNexencoder object failed",

	// common
	SFE_NEW_FAILED,
		"new failed",
	SFE_FTPOPENHOST_FAILED,
		"Encoding session: %s. FTPOpenHost failed (%s)",
	SFE_FTPCHDIR_FAILED,
		"Encoding session: %s. FTPChdir failed (%s)",
	SFE_FTPGETFILES3_FAILED,
		"Encoding session: %s. FTPGetFiles3 failed (%s)",
	SFE_FTPPUTFILES3_FAILED,
		"Encoding session: %s. FTPPutFiles3 failed (%s)",
	SFE_FTPCLOSEHOST_FAILED,
		"Encoding session: %s. FTPCloseHost failed (%s)"


	// Insert here other errors...

} ;

